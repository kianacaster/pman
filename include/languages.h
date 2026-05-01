#ifndef LANGUAGES_H
#define LANGUAGES_H

#include <stdbool.h>
#include "config.h"

typedef struct {
    bool use_git;
    bool use_readme;
    bool use_license;
    bool verbose;
    bool track;
    const char *project_name;
    PManConfig user_config;
} ProjectConfig;

void init_custom(ProjectConfig cfg, const char *script_path);
void execute_init(ProjectConfig cfg, const char *lang, const char *path);

#endif
