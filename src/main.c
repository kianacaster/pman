#define _POSIX_C_SOURCE 200809L
#include "config.h"
#include "languages.h"
#include "registry.h"
#include "tui.h"
#include "utils.h"
#include "version.h"
#include <getopt.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* --- Types & Definitions --- */

typedef struct {
    int argc;
    char **argv;
    PManConfig config;
} AppContext;

typedef int (*CommandHandler)(AppContext *ctx);

typedef struct {
    const char *name;
    const char *description;
    CommandHandler handler;
} Command;

/* --- Logic: Project Initialization --- */

static int execute_init(ProjectConfig cfg, const char *lang, const char *path) {
    char final_path[PATH_MAX];
    
    if (!create_and_enter_dir(path)) {
        fprintf(stderr, "Error: Could not access directory '%s'\n", path);
        return 1;
    }

    if (!getcwd(final_path, sizeof(final_path))) {
        perror("getcwd");
        return 1;
    }

    char *custom_path = get_custom_template_path(lang);
    if (custom_path) {
        init_custom(cfg, custom_path);
        if (cfg.track) register_project(final_path, cfg.project_name, lang);
        printf("\nInitialized custom project: %s\n", cfg.project_name);
        free(custom_path);
        return 0;
    }

    for (int i = 0; i < num_language_presets; i++) {
        if (strcmp(lang, language_presets[i].name) == 0) {
            init_language(cfg, &language_presets[i]);
            if (cfg.track) register_project(final_path, cfg.project_name, lang);
            printf("\nInitialized %s project: %s\n", lang, cfg.project_name);
            return 0;
        }
    }

    fprintf(stderr, "Error: Unsupported language '%s'\n", lang);
    return 1;
}

/* --- Command Handlers --- */

static int handle_init(AppContext *ctx) {
    ProjectConfig cfg = {
        .use_git = true, .use_readme = true, .use_license = true,
        .verbose = ctx->config.verbose, .track = true,
        .user_config = ctx->config
    };

    char *target_dir = NULL;
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
    optind = 1; // Reset getopt for subcommand parsing
    while ((opt = getopt_long(ctx->argc, ctx->argv, "d:grlnv", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'd': target_dir = optarg; break;
            case 'g': cfg.use_git = false; break;
            case 'r': cfg.use_readme = false; break;
            case 'l': cfg.use_license = false; break;
            case 'n': cfg.track = false; break;
            case 'v': cfg.verbose = true; break;
            default: return 1;
        }
    }

    if (optind >= ctx->argc) {
        fprintf(stderr, "Usage: pman init <lang> [name]\n");
        return 1;
    }

    const char *lang = ctx->argv[optind++];
    const char *proj_arg = (optind < ctx->argc) ? ctx->argv[optind] : NULL;
    
    /* Resolve Project Name & Path */
    const char *work_dir = target_dir ? target_dir : (proj_arg ? proj_arg : ".");
    
    if (target_dir) {
        char *tmp = strdup(target_dir);
        cfg.project_name = strdup(basename(tmp));
        free(tmp);
    } else if (proj_arg) {
        cfg.project_name = proj_arg;
    } else {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd))) {
            char *tmp = strdup(cwd);
            cfg.project_name = strdup(basename(tmp));
            free(tmp);
        } else {
            cfg.project_name = "new_project";
        }
    }

    if (!is_safe_name(cfg.project_name)) {
        fprintf(stderr, "Error: Project name '%s' is unsafe or invalid.\n", cfg.project_name);
        return 1;
    }

    return execute_init(cfg, lang, work_dir);
}

static int handle_list(AppContext *ctx) {
    (void)ctx;
    list_projects();
    return 0;
}

static int handle_status(AppContext *ctx) {
    (void)ctx;
    check_all_status();
    return 0;
}

static int handle_prune(AppContext *ctx) {
    (void)ctx;
    prune_registry();
    return 0;
}

static int handle_uninstall(AppContext *ctx) {
    (void)ctx;
    // We assume the script is in the source directory if running from source,
    // but the binary should probably know where it's installed.
    // For simplicity, we'll try to run it from a common location or relative to the project.
    
    // Check if uninstall.sh exists in current directory or /usr/local/share/pman/ (if we had that)
    // For now, let's just use system() to call the script if it exists in the current directory
    // or we can just implement the logic here.
    // Given the user wants a script, let's try to call it.
    
    // Better yet, let's just implement the prompt and logic directly in C for better integration
    // but the user specifically asked for a script too.
    
    char response[10];
    char *user = getenv("USER");
    printf("pman will now be uninstalled for user %s [y/N]: ", user ? user : "unknown");
    if (fgets(response, sizeof(response), stdin) == NULL) return 1;
    
    if (response[0] != 'y' && response[0] != 'Y') {
        printf("Uninstall cancelled.\n");
        return 0;
    }

    printf("Removing binary...\n");
    system("sudo rm -f /usr/local/bin/pman");

    printf("Removing configuration and data...\n");
    char cmd[PATH_MAX];
    char *home = getenv("HOME");
    if (home) {
        snprintf(cmd, sizeof(cmd), "rm -f %s/.pmanrc", home);
        system(cmd);
        snprintf(cmd, sizeof(cmd), "rm -rf %s/.config/pman", home);
        system(cmd);
    }

    printf("pman has been successfully uninstalled.\n");
    return 0;
}

/* --- Dispatcher Configuration --- */

static const Command COMMANDS[] = {
    {"init",   "Initialize a new project",           handle_init},
    {"list",   "List all registered projects",       handle_list},
    {"status", "Check Git status of all projects",   handle_status},
    {"prune",  "Remove missing projects from registry", handle_prune},
    {"uninstall", "Uninstall pman and remove all data", handle_uninstall}
};

static void print_usage(void) {
    printf("PMan - Project Manager\n\nUsage: pman <command> [options]\n\nCommands:\n");
    for (size_t i = 0; i < sizeof(COMMANDS)/sizeof(Command); i++) {
        printf("  %-8s %s\n", COMMANDS[i].name, COMMANDS[i].description);
    }
    printf("\nGlobal Options:\n"
           "  -v, --version       Print version\n");
    printf("\nInit Options:\n"
           "  -d, --dir <path>    Target directory\n"
           "  -g, --no-git        Skip Git\n"
           "  -r, --no-readme     Skip README\n"
           "  -l, --no-license    Skip LICENSE\n"
           "  -n, --no-track      Skip registration\n"
           "  -v, --verbose       Enable verbose output\n");
}

/* --- Main Entry Point --- */

int main(int argc, char *argv[]) {
    if (argc > 1 && (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)) {
        printf("pman %s\n", PMAN_VERSION);
        return 0;
    }

    PManConfig user_cfg = load_config();
    if (argc < 2) {
        run_tui(user_cfg);
        return 0;
    }

    AppContext app = {
        .argc = argc,
        .argv = argv,
        .config = user_cfg
    };

    for (size_t i = 0; i < sizeof(COMMANDS)/sizeof(Command); i++) {
        if (strcmp(argv[1], COMMANDS[i].name) == 0) {
            app.argc--; // Shift arguments for the subcommand
            app.argv++;
            return COMMANDS[i].handler(&app);
        }
    }

    print_usage();
    return 1;
}
