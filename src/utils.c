#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <ctype.h>
#include "utils.h"

int run_command(const char *cmd, bool verbose) {
    if (verbose) {
        printf("Running: %s\n", cmd);
        return system(cmd);
    }
    char silent_cmd[1024];
    snprintf(silent_cmd, sizeof(silent_cmd), "%s > /dev/null 2>&1", cmd);
    return system(silent_cmd);
}

int zip_file(const char *src_path, const char *dest_path) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "zip -r %s %s", dest_path, src_path);
    int ret = system(cmd);
    if (ret == 0) {
        return 0;
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

static void mkdir_p(const char *path) {
    char tmp[1024];
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
    char *path = malloc(1024);
    if (!path) return NULL;
    snprintf(path, 1024, "%s/.config/pman/templates/%s/init.sh", home, lang);
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
    if (!path || strlen(path) == 0 || strlen(path) > 512) return false;
    if (strstr(path, "..")) return false;
    const char *bad_chars = ";|&><`$\\!*()[]{}'\"";
    if (strpbrk(path, bad_chars)) return false;
    return true;
}
