#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h>  // Для basename

void create_directory(const char *path) {
    if (mkdir(path, 0755) == -1) {
        perror("Error creating directory");
    } else {
        printf("Directory %s created successfully\n", path);
    }
}

void list_directory(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

// если в директории есть файлы - удалить их тоже
void remove_directory(const char *path) {
    struct stat st;
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    char fullpath[4096];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (lstat(fullpath, &st) == -1) {
            perror("lstat");
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            remove_directory(fullpath);
        } else {
            if (unlink(fullpath) == -1) {
                perror("unlink");
            }
        }
    }

    closedir(dir);

    if (rmdir(path) == -1) {
        perror("rmdir");
    } else {
        printf("Directory %s removed successfully\n", path);
    }
}

void create_file(const char *path) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error creating file");
    } else {
        printf("File %s created successfully\n", path);
        close(fd);
    }
}

void read_file(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return;
    }

    char buffer[256];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }

    if (bytes_read == -1) {
        perror("Error reading file");
    }

    close(fd);
}

void remove_file(const char *path) {
    if (unlink(path) == -1) {
        perror("Error removing file");
    } else {
        printf("File %s removed successfully\n", path);
    }
}

void create_symlink(const char *target, const char *linkpath) {
    if (symlink(target, linkpath) == -1) {
        perror("Error creating symbolic link");
    } else {
        printf("Symbolic link %s -> %s created successfully\n", linkpath, target);
    }
}

void read_symlink(const char *path) {
    char target[1024];
    ssize_t len = readlink(path, target, sizeof(target) - 1);
    if (len == -1) {
        perror("Error reading symbolic link");
    } else {
        target[len] = '\0';
        printf("Symbolic link %s points to %s\n", path, target);
    }
}

void read_symlink_target(const char *path) {
    char target[1024];
    ssize_t len = readlink(path, target, sizeof(target) - 1);
    if (len == -1) {
        perror("Error reading symbolic link");
    } else {
        target[len] = '\0';
        read_file(target);
    }
}

void remove_symlink(const char *path) {
    if (unlink(path) == -1) {
        perror("Error removing symbolic link");
    } else {
        printf("Symbolic link %s removed successfully\n", path);
    }
}

void create_hardlink(const char *target, const char *linkpath) {
    if (link(target, linkpath) == -1) {
        perror("Error creating hard link");
    } else {
        printf("Hard link %s -> %s created successfully\n", linkpath, target);
    }
}

void remove_hardlink(const char *path) {
    if (unlink(path) == -1) {
        perror("Error removing hard link");
    } else {
        printf("Hard link %s removed successfully\n", path);
    }
}

void file_info(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        perror("Error getting file info");
    } else {
        printf("Permissions: %o\n", statbuf.st_mode & 0777);
        printf("Hard links: %ld\n", statbuf.st_nlink);
    }
}

void change_permissions(const char *path, mode_t mode) {
    if (chmod(path, mode) == -1) {
        perror("Error changing permissions");
    } else {
        printf("Permissions for %s changed to %o\n", path, mode);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <operation> <path>\n", argv[0]);
        return 1;
    }

    const char *operation = basename(argv[0]); 
    const char *path = argv[1];  

    if (strcmp(operation, "create_directory") == 0) {
        create_directory(path);
    } else if (strcmp(operation, "list_directory") == 0) {
        list_directory(path);
    } else if (strcmp(operation, "remove_directory") == 0) {
        remove_directory(path);
    } else if (strcmp(operation, "create_file") == 0) {
        create_file(path);
    } else if (strcmp(operation, "read_file") == 0) {
        read_file(path);
    } else if (strcmp(operation, "remove_file") == 0) {
        remove_file(path);
    } else if (strcmp(operation, "create_symlink") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s create_symlink <target> <link>\n", argv[0]);
            return 1;
        }
        create_symlink(argv[2], path);
    } else if (strcmp(operation, "read_symlink") == 0) {
        read_symlink(path);
    } else if (strcmp(operation, "read_symlink_target") == 0) {
        read_symlink_target(path);
    } else if (strcmp(operation, "remove_symlink") == 0) {
        remove_symlink(path);
    } else if (strcmp(operation, "create_hardlink") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s create_hardlink <target> <link>\n", argv[0]);
            return 1;
        }
        create_hardlink(argv[2], path);
    } else if (strcmp(operation, "remove_hardlink") == 0) {
        remove_hardlink(path);
    } else if (strcmp(operation, "file_info") == 0) {
        file_info(path);
    } else if (strcmp(operation, "change_permissions") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s change_permissions <path> <mode>\n", argv[0]);
            return 1;
        }
        change_permissions(path, strtol(argv[2], NULL, 8));
    } else {
        fprintf(stderr, "Unknown operation: %s\n", operation);
        return 1;
    }

    return 0;
}
