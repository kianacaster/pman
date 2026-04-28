#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "config.h"

PManConfig load_config(void) {
    PManConfig cfg;
    strcpy(cfg.author, "Unknown Author");
    strcpy(cfg.email, "unknown@example.com");
    strcpy(cfg.license, "MIT");
    cfg.verbose = false;

    const char *home = getenv("HOME");
    if (!home) return cfg;

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/.pmanrc", home);

    FILE *f = fopen(path, "r");
    if (!f) return cfg;

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char *key = strtok(line, "=");
        char *val = strtok(NULL, "\n");

        if (key && val) {
            if (strcmp(key, "author") == 0) {
                strncpy(cfg.author, val, sizeof(cfg.author) - 1);
                cfg.author[sizeof(cfg.author) - 1] = '\0';
            }
            else if (strcmp(key, "email") == 0) {
                strncpy(cfg.email, val, sizeof(cfg.email) - 1);
                cfg.email[sizeof(cfg.email) - 1] = '\0';
            }
            else if (strcmp(key, "license") == 0) {
                strncpy(cfg.license, val, sizeof(cfg.license) - 1);
                cfg.license[sizeof(cfg.license) - 1] = '\0';
            }
            else if (strcmp(key, "verbose") == 0) cfg.verbose = (strcmp(val, "true") == 0 || strcmp(val, "1") == 0);
        }
    }

    fclose(f);
    return cfg;
}
