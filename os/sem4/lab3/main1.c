#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUFSIZE 8192

void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

void reverse_file(const char *source, const char *dest, mode_t mode) {
    int src_fd = open(source, O_RDONLY);
    if (src_fd == -1) {
        perror("open source");
        return;
    }

    struct stat st;
    if (fstat(src_fd, &st) == -1) {
        perror("fstat");
        close(src_fd);
        return;
    }
    off_t file_size = st.st_size;

    int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (dest_fd == -1) {
        perror("open dest");
        close(src_fd);
        return;
    }

    char *buf = malloc(BUFSIZE);
    if (!buf) {
        perror("malloc");
        close(src_fd);
        close(dest_fd);
        return;
    }

    off_t remaining = file_size;
    while (remaining > 0) {
        size_t chunk = remaining < BUFSIZE ? (size_t)remaining : BUFSIZE;
        off_t offset = remaining - chunk;

        if (lseek(src_fd, offset, SEEK_SET) == -1) {
            perror("lseek");
            break;
        }
        if (read(src_fd, buf, chunk) != (ssize_t)chunk) {
            perror("read");
            break;
        }

        for (size_t i = 0; i < chunk / 2; i++) {
            char tmp = buf[i];
            buf[i] = buf[chunk - 1 - i];
            buf[chunk - 1 - i] = tmp;
        }
        if (write(dest_fd, buf, chunk) != (ssize_t)chunk) {
            perror("write");
            break;
        }
        remaining -= chunk;
    }

    free(buf);
    close(src_fd);
    close(dest_fd);
}


void copy_directory(const char *src_dir, const char *dest_dir) {
    DIR *dir = opendir(src_dir); 
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char src_path[1024], dest_path[1024];

        // Формируем полный путь к исходному файлу
        snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);

        // Создаём перевёрнутое имя файла
        char reversed_name[256];
        strncpy(reversed_name, entry->d_name, sizeof(reversed_name));
        reversed_name[sizeof(reversed_name) - 1] = '\0';
        reverse_string(reversed_name);

        // Формируем полный путь к целевому файлу
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, reversed_name);

        // Получаем информацию о файле
        struct stat statbuf;
        if (stat(src_path, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        // Если это обычный файл — копируем его задом наперед
        if (S_ISREG(statbuf.st_mode)) {
            reverse_file(src_path, dest_path, statbuf.st_mode);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *source_dir = argv[1];
    char dest_dir[1024];

    // Выделяем только имя каталога без пути (basename)
    const char *last_slash = strrchr(source_dir, '/');
    const char *basename = (last_slash != NULL) ? last_slash + 1 : source_dir;

    // Копируем имя каталога и переворачиваем его
    char reversed_name[256];
    strncpy(reversed_name, basename, sizeof(reversed_name));
    reversed_name[sizeof(reversed_name) - 1] = '\0';
    reverse_string(reversed_name);

    // Формируем путь к целевому каталогу
    if (last_slash != NULL) {
        snprintf(dest_dir, sizeof(dest_dir), "%.*s/%s", (int)(last_slash - source_dir), source_dir, reversed_name);
    } else {
        snprintf(dest_dir, sizeof(dest_dir), "%s", reversed_name);
    }

    // Создаём новый каталог
    if (mkdir(dest_dir, 0755) == -1) {
        perror("mkdir");
        return EXIT_FAILURE;
    }

    // Копируем файлы из исходного каталога в новый
    copy_directory(source_dir, dest_dir);

    return EXIT_SUCCESS;
}

// файл на 1г
// tail -c 16 nib.eliftset | hexdump -C
//  head -c 16 testfile.bin | hexdump -C