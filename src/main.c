#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <libgen.h>
#include <unistd.h>
#include <limits.h>
#include "languages.h"
#include "tui.h"
#include "config.h"
#include "utils.h"
#include "registry.h"

static void usage(void) {
    printf("pman - Project Manager\n\n");
    printf("Usage: pman <command> [options]\n\n");
    printf("Commands:\n");
    printf("  init <lang> [name]  Initialize a new project\n");
    printf("  list                List all registered projects\n");
    printf("  status              Check Git status of all projects\n");
    printf("  prune               Remove missing projects from registry\n");
    printf("  (no command)        Launch interactive wizard\n\n");
    printf("Init Options:\n");
    printf("  -d, --dir <path>    Target directory for initialization\n");
    printf("  -g, --no-git        Skip Git initialization\n");
    printf("  -r, --no-readme     Skip README generation\n");
    printf("  -l, --no-license    Skip LICENSE generation\n");
    printf("  -n, --no-track      Skip registration in project registry\n");
    printf("  -v, --verbose       Enable verbose output\n");
}

static int handle_init(int argc, char *argv[], DInitConfig user_cfg) {
    ProjectConfig cfg = {true, true, true, user_cfg.verbose, true, NULL, user_cfg};
    char *target_dir = NULL;
    char *proj_arg = NULL;

    static struct option long_opts[] = {
        {"dir", required_argument, 0, 'd'},
        {"no-git", no_argument, 0, 'g'},
        {"no-readme", no_argument, 0, 'r'},
        {"no-license", no_argument, 0, 'l'},
        {"no-track", no_argument, 0, 'n'},
        {"verbose", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    int opt;
    optind = 1;
    while ((opt = getopt_long(argc, argv, "d:grlnv", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'd': target_dir = optarg; break;
            case 'g': cfg.use_git = false; break;
            case 'r': cfg.use_readme = false; break;
            case 'l': cfg.use_license = false; break;
            case 'n': cfg.track = false; break;
            case 'v': cfg.verbose = true; break;
        }
    }

    if (optind >= argc) {
        usage();
        return 1;
    }

    const char *lang = argv[optind++];
    if (optind < argc) proj_arg = argv[optind++];

    if (target_dir && !is_safe_path(target_dir)) {
        fprintf(stderr, "Error: Unsafe path detected.\n");
        return 1;
    }
    if (proj_arg && !is_safe_name(proj_arg)) {
        fprintf(stderr, "Error: Unsafe name detected.\n");
        return 1;
    }

    char final_path[PATH_MAX];
    if (target_dir) {
        char *tmp = strdup(target_dir);
        cfg.project_name = strdup(basename(tmp));
        free(tmp);
        if (!create_and_enter_dir(target_dir)) return 1;
    } else if (proj_arg) {
        cfg.project_name = proj_arg;
        if (!create_and_enter_dir(cfg.project_name)) return 1;
    } else {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd))) {
            char *tmp = strdup(cwd);
            cfg.project_name = strdup(basename(tmp));
            free(tmp);
        } else {
            cfg.project_name = "my_project";
        }
    }

    if (!getcwd(final_path, sizeof(final_path))) {
        perror("getcwd");
        return 1;
    }

    if (!is_safe_name(cfg.project_name)) {
        fprintf(stderr, "Error: Resolved project name is unsafe.\n");
        return 1;
    }

    char *custom_path = get_custom_template_path(lang);
    if (custom_path) {
        init_custom(cfg, custom_path);
        if (cfg.track) register_project(final_path, cfg.project_name, lang);
        printf("\nSuccessfully initialized custom project: %s\n", cfg.project_name);
        free(custom_path);
        return 0;
    }

    for (int i = 0; i < num_language_presets; i++) {
        if (strcmp(lang, language_presets[i].name) == 0) {
            language_presets[i].init_func(cfg);
            if (cfg.track) register_project(final_path, cfg.project_name, lang);
            printf("\nSuccessfully initialized %s project: %s\n", lang, cfg.project_name);
            return 0;
        }
    }

    printf("Error: Unsupported language: %s\n", lang);
    return 1;
}

int main(int argc, char *argv[]) {
    DInitConfig user_cfg = load_config();

    if (argc == 1) {
        run_tui(user_cfg);
        return 0;
    }

    char *cmd = argv[1];

    if (strcmp(cmd, "init") == 0) {
        return handle_init(argc - 1, argv + 1, user_cfg);
    } else if (strcmp(cmd, "list") == 0) {
        list_projects();
    } else if (strcmp(cmd, "status") == 0) {
        check_all_status();
    } else if (strcmp(cmd, "prune") == 0) {
        prune_registry();
    } else {
        usage();
        return 1;
    }

    return 0;
}
