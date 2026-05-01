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
    char *path = malloc(PATH_MAX);
    if (!path) return NULL;
    snprintf(path, PATH_MAX, "%s/.config/pman/registry.txt", home);
    
    // Ensure directory exists
    char config_dir[PATH_MAX];
    snprintf(config_dir, PATH_MAX, "%s/.config/pman", home);
    mkdir_p(config_dir);
    
    return path;
}

void register_project(const char *path, const char *name, const char *lang) {
    if (strchr(name, '|') || strchr(path, '|') || strchr(lang, '|')) return;
    char *reg_path = get_registry_path();
    if (!reg_path) return;

    char absolute_path[PATH_MAX];
    if (realpath(path, absolute_path) == NULL) {
        strncpy(absolute_path, path, sizeof(absolute_path)-1);
        absolute_path[sizeof(absolute_path)-1] = '\0';
    }

    FILE *f = fopen(reg_path, "a");
    if (f) {
        fprintf(f, "%s|%s|%s|%ld\n", absolute_path, name, lang, (long)time(NULL));
        fclose(f);
    }
    free(reg_path);
}

int get_project_path(const char *name, char *dest, size_t dest_size) {
    char *reg_path = get_registry_path();
    if (!reg_path) return 1;

    FILE *f = fopen(reg_path, "r");
    free(reg_path);
    if (!f) return 1;

    char line[PATH_MAX + 256];
    int found = 1;
    while (fgets(line, sizeof(line), f)) {
        char *p = strtok(line, "|");
        char *n = strtok(NULL, "|");
        if (n && strcmp(n, name) == 0) {
            strncpy(dest, p, dest_size - 1);
            dest[dest_size - 1] = '\0';
            found = 0;
            break;
        }
    }
    fclose(f);
    return found;
}

void list_projects(void) {
    char *reg_path = get_registry_path();
    if (!reg_path) return;
    FILE *f = fopen(reg_path, "r");
    free(reg_path);
    if (!f) return;

    printf("%-20s | %-10s | %s\n", "NAME", "LANG", "PATH");
    char line[PATH_MAX + 256];
    while (fgets(line, sizeof(line), f)) {
        char *p = strtok(line, "|");
        char *n = strtok(NULL, "|");
        char *l = strtok(NULL, "|");
        if (p && n && l) {
            bool exists = (access(p, F_OK) == 0);
            printf("%-20s | %-10s | %s%s\n", n, l, p, exists ? "" : " [MISSING]");
        }
    }
    fclose(f);
}

void check_all_status(void) {
    char *reg_path = get_registry_path();
    if (!reg_path) return;
    FILE *f = fopen(reg_path, "r");
    free(reg_path);
    if (!f) return;

    char line[PATH_MAX + 256];
    char original_cwd[PATH_MAX];
    if (!getcwd(original_cwd, sizeof(original_cwd))) {
        fclose(f);
        return;
    }

    while (fgets(line, sizeof(line), f)) {
        char *p = strtok(line, "|");
        char *n = strtok(NULL, "|");
        if (p && n && access(p, F_OK) == 0) {
            printf("[%s]\n", n);
            if (chdir(p) == 0) {
                char *args[] = {"git", "status", "-s", NULL};
                run_command_args(args, true);
                if (chdir(original_cwd) != 0) { /* ignore */ }
            }
        }
    }
    fclose(f);
}

void prune_registry(void) {
    char *reg_path = get_registry_path();
    if (!reg_path) return;
    
    char tmp_path[PATH_MAX];
    snprintf(tmp_path, sizeof(tmp_path), "%s.XXXXXX", reg_path);
    int fd = mkstemp(tmp_path);
    if (fd == -1) { free(reg_path); return; }

    FILE *in = fopen(reg_path, "r");
    if (!in) { close(fd); unlink(tmp_path); free(reg_path); return; }

    FILE *out = fdopen(fd, "w");
    char line[PATH_MAX + 256];
    int pruned = 0;
    while (fgets(line, sizeof(line), in)) {
        char line_copy[PATH_MAX + 256];
        strcpy(line_copy, line);
        char *p = strtok(line_copy, "|");
        if (p && access(p, F_OK) == 0) fputs(line, out);
        else pruned++;
    }
    fclose(in);
    fclose(out);

    if (pruned > 0) rename(tmp_path, reg_path);
    else unlink(tmp_path);
    
    printf("Pruned: %d\n", pruned);
    free(reg_path);
}
