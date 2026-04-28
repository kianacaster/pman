#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include "registry.h"
#include "utils.h"

static char* get_registry_path(void) {
    const char *home = getenv("HOME");
    if (!home) return NULL;
    char config_dir[1024];
    snprintf(config_dir, 1024, "%s/.config/pman", home);
    mkdir_p(config_dir);
    char *path = malloc(1024);
    if (!path) return NULL;
    snprintf(path, 1024, "%s/.config/pman/registry.txt", home);
    return path;
}

void register_project(const char *path, const char *name, const char *lang) {
    if (strchr(name, '|') || strchr(path, '|') || strchr(lang, '|')) return;
    char *reg_path = get_registry_path();
    if (!reg_path) return;
    char absolute_path[PATH_MAX];
    if (realpath(path, absolute_path) == NULL) {
        strncpy(absolute_path, path, sizeof(absolute_path)-1);
    }
    FILE *f = fopen(reg_path, "a");
    if (f) {
        time_t now = time(NULL);
        fprintf(f, "%s|%s|%s|%ld\n", absolute_path, name, lang, (long)now);
        fclose(f);
    }
    free(reg_path);
}

int get_project_path(const char *name, char *dest, size_t dest_size) {
    if (!name || !dest || dest_size == 0) return 1;

    char *reg_path = get_registry_path();
    if (!reg_path) return 1;

    FILE *f = fopen(reg_path, "r");
    if (!f) {
        free(reg_path);
        return 1;
    }

    char line[PATH_MAX + 256];
    int found = 1;

    while (fgets(line, sizeof(line), f)) {
        char *line_path = strtok(line, "|");
        char *line_name = strtok(NULL, "|");
        if (line_name && strcmp(line_name, name) == 0) {
            strncpy(dest, line_path, dest_size - 1);
            dest[dest_size - 1] = '\0';
            found = 0;
            break;
        }
    }

    fclose(f);
    free(reg_path);
    return found;
}

void list_projects(void) {
    char *reg_path = get_registry_path();
    if (!reg_path) return;
    FILE *f = fopen(reg_path, "r");
    if (!f) return;
    printf("%-20s | %-10s | %s\n", "NAME", "LANG", "PATH");
    char line[2048];
    while (fgets(line, sizeof(line), f)) {
        char *p = strtok(line, "|");
        char *n = strtok(NULL, "|");
        char *l = strtok(NULL, "|");
        if (p && n && l) {
            struct stat st;
            if (stat(p, &st) == 0) {
                printf("%-20s | %-10s | %s\n", n, l, p);
            } else {
                printf("%-20s | %-10s | %s [MISSING]\n", n, l, p);
            }
        }
    }
    fclose(f);
    free(reg_path);
}

void check_all_status(void) {
    char *reg_path = get_registry_path();
    if (!reg_path) return;
    FILE *f = fopen(reg_path, "r");
    if (!f) return;
    char line[2048];
    while (fgets(line, sizeof(line), f)) {
        char *line_copy = strdup(line);
        char *p = strtok(line_copy, "|");
        char *n = strtok(NULL, "|");
        if (p && n) {
            struct stat st;
            if (stat(p, &st) == 0) {
                printf("[%s]\n", n);
                char cmd[PATH_MAX + 64];
                snprintf(cmd, sizeof(cmd), "cd '%s' && git status -s", p);
                system(cmd);
            }
        }
        free(line_copy);
    }
    fclose(f);
    free(reg_path);
}

void prune_registry(void) {
    char *reg_path = get_registry_path();
    if (!reg_path) return;
    char tmp_path[1024];
    snprintf(tmp_path, sizeof(tmp_path), "%s.XXXXXX", reg_path);
    int fd = mkstemp(tmp_path);
    if (fd == -1) {
        free(reg_path);
        return;
    }
    FILE *in = fopen(reg_path, "r");
    if (!in) {
        close(fd);
        unlink(tmp_path);
        free(reg_path);
        return;
    }
    FILE *out = fdopen(fd, "w");
    char line[2048];
    int pruned = 0;
    while (fgets(line, sizeof(line), in)) {
        char line_copy[2048];
        strncpy(line_copy, line, sizeof(line_copy)-1);
        char *p = strtok(line_copy, "|");
        if (p) {
            struct stat st;
            if (stat(p, &st) == 0) fputs(line, out);
            else pruned++;
        }
    }
    fclose(in);
    fclose(out);
    if (pruned > 0) rename(tmp_path, reg_path);
    else unlink(tmp_path);
    printf("Pruned: %d\n", pruned);
    free(reg_path);
}
