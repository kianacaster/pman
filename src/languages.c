#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "languages.h"
#include "utils.h"
#include "templates.h"

static void init_git(bool verbose) {
    run_command("git init", verbose);
    run_command("git branch -m main", verbose);
    run_command("git add .", verbose);
    run_command("git commit -m 'Initial commit from DInit'", verbose);
}

static void create_license(ProjectConfig cfg) {
    if (!cfg.use_license) return;
    if (strcmp(cfg.user_config.license, "MIT") == 0 || strcmp(cfg.user_config.license, "mit") == 0) {
        write_formatted("LICENSE", mit_license_template, cfg.user_config.author);
    } else {
        write_formatted("LICENSE", "License: %s\n", cfg.user_config.license);
    }
}

void init_python(ProjectConfig cfg) {
    printf("Initializing Python project: %s\n", cfg.project_name);
    write_to_file("main.py", python_main_template);
    write_to_file(".gitignore", python_gitignore_template);
    run_command("touch requirements.txt", cfg.verbose);
    if (cfg.use_readme) write_readme("README.md", python_readme_template, cfg.project_name, cfg.user_config.author, cfg.user_config.email);
    create_license(cfg);
    run_command("python3 -m venv .venv", cfg.verbose);
    if (cfg.use_git) init_git(cfg.verbose);
}

void init_c(ProjectConfig cfg) {
    printf("Initializing C project: %s\n", cfg.project_name);
    write_to_file("main.c", c_main_template);
    write_to_file(".gitignore", "*.o\nmain\n");
    write_to_file("Makefile", "all:\n\tgcc main.c -o main\nclean:\n\trm -f main\n");
    if (cfg.use_readme) write_readme("README.md", c_readme_template, cfg.project_name, cfg.user_config.author, cfg.user_config.email);
    create_license(cfg);
    if (cfg.use_git) init_git(cfg.verbose);
}

void init_cpp(ProjectConfig cfg) {
    printf("Initializing C++ project: %s\n", cfg.project_name);
    write_to_file("main.cpp", cpp_main_template);
    write_to_file(".gitignore", "*.o\nmain\n");
    write_to_file("Makefile", cpp_makefile_template);
    if (cfg.use_readme) write_readme("README.md", cpp_readme_template, cfg.project_name, cfg.user_config.author, cfg.user_config.email);
    create_license(cfg);
    if (cfg.use_git) init_git(cfg.verbose);
}

void init_java(ProjectConfig cfg) {
    printf("Initializing Java project: %s\n", cfg.project_name);
    write_to_file("Main.java", java_main_template);
    write_to_file(".gitignore", java_gitignore_template);
    if (cfg.use_readme) write_readme("README.md", java_readme_template, cfg.project_name, cfg.user_config.author, cfg.user_config.email);
    create_license(cfg);
    if (cfg.use_git) init_git(cfg.verbose);
}

void init_go(ProjectConfig cfg) {
    printf("Initializing Go project: %s\n", cfg.project_name);
    write_to_file("main.go", go_main_template);
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "go mod init %s", cfg.project_name);
    run_command(cmd, cfg.verbose);
    if (cfg.use_readme) write_readme("README.md", go_readme_template, cfg.project_name, cfg.user_config.author, cfg.user_config.email);
    create_license(cfg);
    if (cfg.use_git) init_git(cfg.verbose);
}

void init_rust(ProjectConfig cfg) {
    printf("Initializing Rust project: %s\n", cfg.project_name);
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "cargo init --name %s .", cfg.project_name);
    run_command(cmd, cfg.verbose);
    if (cfg.use_readme) write_readme("README.md", rust_readme_template, cfg.project_name, cfg.user_config.author, cfg.user_config.email);
    create_license(cfg);
}

void init_node(ProjectConfig cfg) {
    printf("Initializing Node project: %s\n", cfg.project_name);
    write_to_file("index.js", node_index_template);
    write_to_file(".gitignore", node_gitignore_template);
    run_command("npm init -y", cfg.verbose);
    if (cfg.use_readme) write_readme("README.md", node_readme_template, cfg.project_name, cfg.user_config.author, cfg.user_config.email);
    create_license(cfg);
    if (cfg.use_git) init_git(cfg.verbose);
}

void init_csharp(ProjectConfig cfg) {
    printf("Initializing C# project: %s\n", cfg.project_name);
    run_command("dotnet new console --force", cfg.verbose);
    write_to_file(".gitignore", "bin/\nobj/\n");
    if (cfg.use_readme) write_readme("README.md", csharp_readme_template, cfg.project_name, cfg.user_config.author, cfg.user_config.email);
    create_license(cfg);
    if (cfg.use_git) init_git(cfg.verbose);
}

void init_ruby(ProjectConfig cfg) {
    printf("Initializing Ruby project: %s\n", cfg.project_name);
    write_to_file("main.rb", ruby_main_template);
    write_to_file("Gemfile", "source 'https://rubygems.org'\n");
    if (cfg.use_readme) write_readme("README.md", ruby_readme_template, cfg.project_name, cfg.user_config.author, cfg.user_config.email);
    create_license(cfg);
    if (cfg.use_git) init_git(cfg.verbose);
}

void init_php(ProjectConfig cfg) {
    printf("Initializing PHP project: %s\n", cfg.project_name);
    write_to_file("index.php", php_index_template);
    if (cfg.use_readme) write_readme("README.md", php_readme_template, cfg.project_name, cfg.user_config.author, cfg.user_config.email);
    create_license(cfg);
    if (cfg.use_git) init_git(cfg.verbose);
}

void init_html(ProjectConfig cfg) {
    printf("Initializing HTML/Web project: %s\n", cfg.project_name);
    write_to_file("index.html", html_index_template);
    write_to_file("style.css", html_style_template);
    write_to_file("script.js", html_script_template);
    if (cfg.use_readme) write_readme("README.md", html_readme_template, cfg.project_name, cfg.user_config.author, cfg.user_config.email);
    create_license(cfg);
    if (cfg.use_git) init_git(cfg.verbose);
}

void init_bash(ProjectConfig cfg) {
    printf("Initializing Bash project: %s\n", cfg.project_name);
    write_to_file("main.sh", bash_script_template);
    run_command("chmod +x main.sh", cfg.verbose);
    if (cfg.use_readme) write_readme("README.md", bash_readme_template, cfg.project_name, cfg.user_config.author, cfg.user_config.email);
    create_license(cfg);
    if (cfg.use_git) init_git(cfg.verbose);
}

void init_custom(ProjectConfig cfg, const char *script_path) {
    printf("Initializing custom project using: %s\n", script_path);
    setenv("DINIT_PROJECT_NAME", cfg.project_name, 1);
    setenv("DINIT_AUTHOR", cfg.user_config.author, 1);
    setenv("DINIT_EMAIL", cfg.user_config.email, 1);
    setenv("DINIT_LICENSE", cfg.user_config.license, 1);
    run_command(script_path, cfg.verbose);
    if (cfg.use_git) init_git(cfg.verbose);
}

const LanguagePreset language_presets[] = {
    {"python", init_python}, {"c", init_c}, {"cpp", init_cpp}, 
    {"java", init_java}, {"go", init_go}, {"rust", init_rust}, 
    {"node", init_node}, {"csharp", init_csharp}, {"ruby", init_ruby}, 
    {"php", init_php}, {"html", init_html}, {"bash", init_bash}
};
const int num_language_presets = sizeof(language_presets) / sizeof(LanguagePreset);
