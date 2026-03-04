#ifndef LANGUAGES_H
#define LANGUAGES_H

#include <stdbool.h>
#include "config.h"

typedef struct {
    bool use_git;
    bool use_readme;
    bool use_license;
    bool verbose;
    const char *project_name;
    DInitConfig user_config;
} ProjectConfig;

typedef void (*InitFunc)(ProjectConfig cfg);

typedef struct {
    const char *name;
    InitFunc init_func;
} LanguagePreset;

void init_python(ProjectConfig cfg);
void init_c(ProjectConfig cfg);
void init_cpp(ProjectConfig cfg);
void init_java(ProjectConfig cfg);
void init_go(ProjectConfig cfg);
void init_rust(ProjectConfig cfg);
void init_node(ProjectConfig cfg);
void init_csharp(ProjectConfig cfg);
void init_ruby(ProjectConfig cfg);
void init_php(ProjectConfig cfg);
void init_html(ProjectConfig cfg);
void init_bash(ProjectConfig cfg);

void init_custom(ProjectConfig cfg, const char *script_path);

extern const LanguagePreset language_presets[];
extern const int num_language_presets;

#endif
