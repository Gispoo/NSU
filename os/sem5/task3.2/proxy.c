#define _POSIX_C_SOURCE 200809L

#include <arpa/inet.h>
#include <curl/curl.h>
#include <netdb.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// gcc proxy.c -lcurl -lpthread

int PORT = 8080;
const int BUFFER_SIZE = 1024 * 8;
const int DEFAULT_DST_PORT = 80;
const int LISTEN_BACKLOG = 10;
const int SUCCESS = 0;
const int FAILURE = -1;

#define MAX_HOST_LEN 256
#define MAX_PORT_LEN 6

static atomic_int request_cnt = 0;

typedef struct
{
	int client_socket;
} handle_client_args_t;

char *read_get_request(const int client_socket)
{
	char buffer[BUFFER_SIZE];
	char *request = NULL;
	ssize_t bytes_read;
	size_t total_read = 0;

	while ((bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0)
	{
		char *new_request = realloc(request, total_read + bytes_read + 1);
		if (new_request == NULL)
		{
			fprintf(stderr, "read_get_request: failed to realloc\n");
			free(request);
			return NULL;
		}
		request = new_request;
		request[total_read] = '\0';

		buffer[bytes_read] = '\0';
		strncat(request, buffer, bytes_read);
		total_read += bytes_read;

		if (strstr(request, "\r\n\r\n"))
			break;
	}

	if (bytes_read == -1)
	{
		fprintf(stderr, "read_get_request: failed to read from client\n");
		free(request);
		return NULL;
	}

	return request;
}

int is_valid_request(const char *request)
{
	if (request == NULL)
		return FAILURE;

	char method[16], path[2048], version[16];
	if (sscanf(request, "%15s %2047s %15s", method, path, version) != 3)
	{
		fprintf(stderr, "is_valid_request: incorrect format of the initial line\n");
		return FAILURE;
	}

	if (strcmp(method, "GET") != 0)
	{
		fprintf(stderr, "is_valid_request: the %s method is not supported\n", method);
		return FAILURE;
	}

	if (strcmp(version, "HTTP/1.0") != 0 && strcmp(version, "HTTP/1.1") != 0)
	{
		fprintf(stderr, "is_valid_request: The %s protocol version is not supported\n", version);
		return FAILURE;
	}
	return SUCCESS;
}

char *get_header_value(const char *request, const char *key)
{
	if (request == NULL || key == NULL)
		return NULL;

	char *line = strstr(request, key);
	if (line == NULL)
		return NULL;

	// заголовок должен начинаться с новой строки или начала текста
	size_t key_len = strlen(key);
	if (!(line - request > 1 && *(line - 1) == '\n' && line[key_len] == ':'))
		return NULL;

	line += key_len + 1;
	while (*line == ' ')
		line++;

	char *value_end = strstr(line, "\r\n");
	if (value_end == NULL)
		value_end = strchr(line, '\0');

	size_t value_len = value_end - line;
	char *value = (char *)malloc(value_len + 1);
	if (value == NULL)
	{
		fprintf(stderr, "get_header_value: failed to malloc\n");
		return NULL;
	}
	strncpy(value, line, value_len);
	value[value_len] = '\0';
	return value;
}

// "Connection: keep-alive" -> "Connection: close"
void replace_connection_header(char *request)
{
	const char *header_to_find = "Connection: keep-alive";
	const char *replacement = "Connection: close";

	char *header_start = strstr(request, header_to_find);
	if (header_start != NULL)
	{
		size_t find_len = strlen(header_to_find);
		size_t replace_len = strlen(replacement);

		memmove(header_start + replace_len, header_start + find_len, strlen(header_start + find_len) + 1);
		memcpy(header_start, replacement, replace_len);
	}
}

int addr_from_url(const char *buffer, char *host, char *port)
{
	char method[16], url[8192], protocol[16];
	sscanf(buffer, "%15s %2048s %16s", method, url, protocol);

	CURLU *curl_url_obj = curl_url();
	if (!curl_url_obj)
	{
		fprintf(stderr, "addr_from_url: failed to create object CURLU\n");
		return FAILURE;
	}

	CURLUcode res = curl_url_set(curl_url_obj, CURLUPART_URL, url, 0);
	if (res != CURLUE_OK)
	{
		fprintf(stderr, "addr_from_url: curl_url_set error: %s\n", curl_easy_strerror(res));
		curl_url_cleanup(curl_url_obj);
		return FAILURE;
	}

	char *curl_host, *curl_port;
	if (curl_url_get(curl_url_obj, CURLUPART_HOST, &curl_host, 0) != CURLUE_OK)
	{
		fprintf(stderr, "addr_from_url: failed to get host from URL\n");
		return FAILURE;
	}
	strcpy(host, curl_host);
	curl_free(curl_host); // curl_url_get сам выделяет память

	if (curl_url_get(curl_url_obj, CURLUPART_PORT, &curl_port, 0) != CURLUE_OK)
	{
		sprintf(port, "%d", DEFAULT_DST_PORT);
	}
	else
	{
		strcpy(port, curl_port);
		curl_free(curl_port);
	}

	curl_url_cleanup(curl_url_obj);
	return SUCCESS;
}

int addr_from_headers(const char *buffer, char *host, char *port)
{
	const char *host_header = "Host:";
	char *host_start = strstr(buffer, host_header);
	if (host_start == NULL)
	{
		fprintf(stderr, "addr_from_headers: failed to find Host header\n");
		return FAILURE;
	}

	host_start += strlen(host_header);
	while (*host_start == ' ')
		host_start++;

	char *host_end = strstr(host_start, "\r\n");
	if (host_end == NULL)
	{
		fprintf(stderr, "addr_from_headers: failed to find end of Host header\n");
		return FAILURE;
	}

	char host_str[MAX_HOST_LEN] = {0};
	strncpy(host_str, host_start, host_end - host_start);
	char *port_start = strchr(host_str, ':');
	if (port_start)
	{
		strcpy(port, port_start + 1);
		*port_start = '\0';
	}
	else
	{
		sprintf(port, "%d", DEFAULT_DST_PORT);
	}
	strcpy(host, host_str);
	return SUCCESS;
}

int extract_addr(const char *buffer, char *host, char *port)
{
	if (addr_from_url(buffer, host, port) == SUCCESS)
		return SUCCESS;
	if (addr_from_headers(buffer, host, port) == SUCCESS)
		return SUCCESS;

	fprintf(stderr, "extract_addr: failed to extract address\n");
	return FAILURE;
}

int connect_to_server(const char *host, const char *port)
{
	struct addrinfo hints;
	struct addrinfo *res;
	int server_socket;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(host, port, &hints, &res) != 0)
	{
		fprintf(stderr, "connect_to_server: failed to resolve host\n");
		return FAILURE;
	}

	server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (server_socket == -1)
	{
		fprintf(stderr, "connect_to_server: failed to create socket for server\n");
		freeaddrinfo(res);
		return FAILURE;
	}

	if (connect(server_socket, res->ai_addr, res->ai_addrlen) == -1)
	{
		fprintf(stderr, "connect_to_server: failed to connect to server\n");
		close(server_socket);
		freeaddrinfo(res);
		return FAILURE;
	}

	freeaddrinfo(res);
	return server_socket;
}

int is_valid_response(const char *buffer)
{
	if (buffer == NULL)
		return FAILURE;

	char version[16], status_code[4], status_text[256];
	if (sscanf(buffer, "%15s %3s %255s", version, status_code, status_text) != 3)
	{
		fprintf(stderr, "is_valid_response: incorrect format of the initial line\n");
		return FAILURE;
	}

	if (strcmp(version, "HTTP/1.0") != 0 && strcmp(version, "HTTP/1.1") != 0)
	{
		fprintf(stderr, "is_valid_response: The %s protocol version is not supported\n", version);
		return FAILURE;
	}

    int code = atoi(status_code);
    if (code < 200 || code > 599)
	{
		fprintf(stderr, "is_valid_response: invalid status code %s\n", status_code);
		return FAILURE;
	}
	return SUCCESS;
}

int get_response_len(const char *buffer)
{
	char *header_val = get_header_value(buffer, "Content-Length");
	if (header_val == NULL)
		return -1;
	const int content_len = atoi(header_val);
	if (content_len == 0)
		return -1;
	free(header_val);

	char *empty_line_start = strstr(buffer, "\r\n\r\n");
	if (empty_line_start == NULL)
		empty_line_start = strstr(buffer, "\0");
	if (empty_line_start == NULL)
		return -1;
	const int bytes_before_body = empty_line_start - buffer;
	return bytes_before_body + 4 + content_len;
}

int response_forwarding(const int client_socket, const int server_socket)
{
	char buffer[BUFFER_SIZE];
	char *response = NULL;
	ssize_t bytes_read;
	size_t total_read = 0;
	int response_len;

	while (1)
	{
		bytes_read = recv(server_socket, buffer, sizeof(buffer), 0);
		if (bytes_read == -1)
		{
			fprintf(stderr, "response_forwarding: failed to read from server\n");
			free(response);
			return FAILURE;
		}
		else if (bytes_read == 0)
			break;
		buffer[bytes_read] = '\0';

		if (total_read == 0)
		{
			if (is_valid_response(buffer) == FAILURE)
			{
				fprintf(stderr, "response_forwarding: invalid response\n");
				return FAILURE;
			}
			response_len = get_response_len(buffer);
		}

		if (send(client_socket, buffer, bytes_read, 0) == -1)
		{
			fprintf(stderr, "handle_client: failed to send part of response to client\n");
			free(response);
    		return FAILURE;
		}

		char *new_response = realloc(response, total_read + bytes_read + 1);
		if (new_response == NULL)
		{
			fprintf(stderr, "response_forwarding: failed to realloc\n");
			free(response);
			return FAILURE;
		}
		response = new_response;
		response[total_read] = '\0';
		strncat(response, buffer, bytes_read);
		total_read += bytes_read;

		if ((response_len != -1 && total_read >= response_len) ||
			(response_len == -1 && strstr(response, "\r\n\r\n")))
			break;
	}

	// printf("%s", response);
	// printf("response len: %ld\n", strlen(response));
	printf("total response len: %ld\n", total_read);

	free(response);
	return SUCCESS;
}

void handle_client(const int client_socket)
{
	char *request = read_get_request(client_socket);
	if (request == NULL)
	{
		fprintf(stderr, "handle_client: failed to read request\n");
		return;
	}

	if (is_valid_request(request) == FAILURE)
	{
		fprintf(stderr, "handle_client: invalid request\n");
		free(request);
		return;
	}
	replace_connection_header(request);
	// printf("%s", request);
	printf("request len: %ld\n", strlen(request));

	char dst_host[MAX_HOST_LEN] = "0";
	char dst_port[MAX_PORT_LEN] = "0";
	extract_addr(request, dst_host, dst_port);
	printf("dst_addr: %s:%s\n", dst_host, dst_port);

	int server_socket = connect_to_server(dst_host, dst_port);
	if (server_socket == -1)
	{
		fprintf(stderr, "handle_client: failed to connect to server\n");
		free(request);
		return;
	}

	if (send(server_socket, request, strlen(request), 0) == -1)
	{
		fprintf(stderr, "handle_client: failed to forward request to server\n");
		free(request);
		close(server_socket);
		return;
	}
	free(request);

	if (response_forwarding(client_socket, server_socket) == FAILURE)
	{
		fprintf(stderr, "handle_client: failed to forward response to client\n");
		close(server_socket);
		return;
	}
	close(server_socket);
}

void *handle_client_wrapper(void *inp_args)
{
	printf("\033[36mrequest %d: start\033[0m\n", atomic_load(&request_cnt));

	handle_client_args_t *args = (handle_client_args_t *)inp_args;
	int client_socket = args->client_socket;
	free(args);

	handle_client(client_socket);
	close(client_socket);

	printf("\033[36mrequest %d: finish\033[0m\n", atomic_load(&request_cnt));
	atomic_fetch_add(&request_cnt, 1);
	return NULL;
}

int create_proxy_socket()
{
	int proxy_socket;
	struct sockaddr_in proxy_addr;

	// AF_INET - IPv4
	proxy_socket = socket(AF_INET, SOCK_STREAM, 0); 
	if (proxy_socket == -1)
	{
		fprintf(stderr, "create_proxy_socket: failed to create socket\n");
		return FAILURE;
	}

	int opt = 1;
	if (setsockopt(proxy_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		fprintf(stderr, "create_proxy_socket: failed to create socket\n");
		close(proxy_socket);
		return FAILURE;
	}

	memset(&proxy_addr, 0, sizeof(proxy_addr));
	proxy_addr.sin_family = AF_INET;
	proxy_addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 (все интерфейсы)
	proxy_addr.sin_port = htons(PORT);

	while (bind(proxy_socket, (struct sockaddr *)&proxy_addr, sizeof(proxy_addr)) == -1)
	{
		PORT += 1;
		memset(&proxy_addr, 0, sizeof(proxy_addr));
		proxy_addr.sin_family = AF_INET;
		proxy_addr.sin_addr.s_addr = INADDR_ANY;
		proxy_addr.sin_port = htons(PORT);

		if (PORT >= 65535)
		{
			close(proxy_socket);
			fprintf(stderr, "create_proxy_socket: bind failed\n");
			return FAILURE;
		}
	}

	if (listen(proxy_socket, LISTEN_BACKLOG) == -1)
	{
		close(proxy_socket);
		fprintf(stderr, "create_proxy_socket: listen failed\n");
		return FAILURE;
	}

	return proxy_socket;
}

int main()
{
	int proxy_socket = create_proxy_socket();
	if (proxy_socket == FAILURE)
		return FAILURE;
	printf("Proxy server is running on port %d; pid = %d\n", PORT, getpid());

	while (1)
	{
		struct sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		int client_socket = accept(proxy_socket, (struct sockaddr *)&client_addr, &client_addr_len);
		if (client_socket == -1)
		{
			fprintf(stderr, "main: accept failed\n");
			continue;
		}

		handle_client_args_t *args = malloc(sizeof(handle_client_args_t));
		if (!args)
		{
			fprintf(stderr, "main: failed to allocate memory for handle_client_args_t\n");
			close(client_socket);
			continue;
		}
		args->client_socket = client_socket;

		pthread_t thread;
		if (pthread_create(&thread, NULL, handle_client_wrapper, args) != 0)
		{
			fprintf(stderr, "main: failed to create thread\n");
			free(args);
			close(client_socket);
			continue;
		}
		pthread_detach(thread);
	}

	close(proxy_socket);
	return 0;
}

// http://lib.ru
// http://zhurnal.lib.ru
// http://httpforever.com
// http://info.cern.ch
// http://neverssl.com

