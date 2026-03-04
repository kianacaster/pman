#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <libgen.h>
#include "tui.h"
#include "languages.h"
#include "utils.h"
#include "registry.h"

#define BOLD_CYAN  "\033[1;36m"
#define RESET      "\033[0m"

static void clear_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void run_tui(DInitConfig user_cfg) {
    printf(BOLD_CYAN "--- pman: Project Setup Wizard ---\n" RESET);
    
    char lang[32], name[128], buf[8];
    
    printf("\nLanguage: ");
    scanf("%31s", lang);
    clear_stdin();

    printf("Project Name (leave blank for current directory): ");
    if (fgets(name, sizeof(name), stdin)) {
        name[strcspn(name, "\n")] = 0;
    }

    /* SECURITY: Validate TUI input */
    if (strlen(name) > 0 && !is_safe_name(name)) {
        fprintf(stderr, "Error: Unsafe project name. Use only alphanumeric, _ and -.\n");
        return;
    }

    printf("Initialize Git? (Y/n): ");
    fgets(buf, sizeof(buf), stdin);
    bool use_git = (buf[0] != 'n' && buf[0] != 'N');

    printf("Create README? (Y/n): ");
    fgets(buf, sizeof(buf), stdin);
    bool use_readme = (buf[0] != 'n' && buf[0] != 'N');

    printf("Create LICENSE? (Y/n): ");
    fgets(buf, sizeof(buf), stdin);
    bool use_license = (buf[0] != 'n' && buf[0] != 'N');

    ProjectConfig cfg = {use_git, use_readme, use_license, user_cfg.verbose, NULL, user_cfg};

    char final_path[1024];
    if (strlen(name) > 0) {
        cfg.project_name = name;
        if (!create_and_enter_dir(cfg.project_name)) return;
        if (!getcwd(final_path, sizeof(final_path))) return;
    } else {
        if (!getcwd(final_path, sizeof(final_path))) return;
        char *tmp = strdup(final_path);
        cfg.project_name = strdup(basename(tmp));
        free(tmp);
    }

    /* Verify directory-derived name is also safe */
    if (!is_safe_name(cfg.project_name)) {
        fprintf(stderr, "Error: Current directory name is unsafe for initialization.\n");
        return;
    }

    char *custom_path = get_custom_template_path(lang);
    if (custom_path) {
        init_custom(cfg, custom_path);
        register_project(final_path, cfg.project_name, lang);
        free(custom_path);
        return;
    }

    for (int i = 0; i < num_language_presets; i++) {
        if (strcmp(lang, language_presets[i].name) == 0) {
            language_presets[i].init_func(cfg);
            register_project(final_path, cfg.project_name, lang);
            return;
        }
    }

    printf("Error: Unsupported language or missing custom template: %s\n", lang);
}
