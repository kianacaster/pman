#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include "utils.h"

int run_command(const char *cmd, bool verbose) {
    if (verbose) {
        printf("Running: %s\n", cmd);
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        if (!verbose) {
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull != -1) {
                dup2(devnull, STDOUT_FILENO);
                dup2(devnull, STDERR_FILENO);
                close(devnull);
            }
        }
        execl("/bin/sh", "sh", "-c", cmd, (char *)NULL);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
    return -1;
}

int run_command_args(char *const argv[], bool verbose) {
    if (verbose) {
        printf("Running:");
        for (int i = 0; argv[i]; i++) printf(" %s", argv[i]);
        printf("\n");
    }

    pid_t pid = fork();
    if (pid == 0) {
        if (!verbose) {
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull != -1) {
                dup2(devnull, STDOUT_FILENO);
                dup2(devnull, STDERR_FILENO);
                close(devnull);
            }
        }
        execvp(argv[0], argv);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
    return -1;
}

int zip_file(const char *src_path, const char *dest_path) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp("zip", "zip", "-r", dest_path, src_path, (char *)NULL);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status) == 0 ? 0 : 1;
    }
    return 1;
}

bool write_to_file(const char *path, const char *content) {
    FILE *f = fopen(path, "w");
    if (!f) return false;
    fputs(content, f);
    fclose(f);
    return true;
}

bool write_formatted(const char *path, const char *fmt, const char *arg) {
    FILE *f = fopen(path, "w");
    if (!f) return false;
    fprintf(f, fmt, arg);
    fclose(f);
    return true;
}

bool write_readme(const char *path, const char *fmt, const char *name, const char *author, const char *email) {
    FILE *f = fopen(path, "w");
    if (!f) return false;
    fprintf(f, fmt, name, author, email);
    fclose(f);
    return true;
}

void mkdir_p(const char *path) {
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;
    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    if (len >= sizeof(tmp)) return;
    if (tmp[len - 1] == '/') tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    mkdir(tmp, 0755);
}

bool create_and_enter_dir(const char *name) {
    if (strcmp(name, ".") == 0) return true;
    mkdir_p(name);
    if (chdir(name) != 0) return false;
    return true;
}

char* get_custom_template_path(const char *lang) {
    const char *home = getenv("HOME");
    if (!home) return NULL;
    char *path = malloc(PATH_MAX);
    if (!path) return NULL;
    snprintf(path, PATH_MAX, "%s/.config/pman/templates/%s/init.sh", home, lang);
    struct stat st;
    if (stat(path, &st) == 0 && (st.st_mode & S_IXUSR)) return path;
    free(path);
    return NULL;
}

bool is_safe_name(const char *name) {
    if (!name || strlen(name) == 0 || strlen(name) > 128) return false;
    for (int i = 0; name[i]; i++) {
        if (!isalnum(name[i]) && name[i] != '_' && name[i] != '-') return false;
    }
    return true;
}

bool is_safe_path(const char *path) {
    if (!path || strlen(path) == 0 || strlen(path) > PATH_MAX) return false;
    if (strstr(path, "..")) return false;
    const char *bad_chars = ";|&><`$\\!*()[]{}'\"";
    if (strpbrk(path, bad_chars)) return false;
    return true;
}
