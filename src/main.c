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
#include <sys/stat.h>
#include <unistd.h>

/* --- Types & Definitions --- */

typedef struct {
  int argc;
  char **argv;
  int cmd_idx;
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
    if (cfg.track)
      register_project(final_path, cfg.project_name, lang);
    printf("\nInitialized custom project: %s\n", cfg.project_name);
    free(custom_path);
    return 0;
  }

  for (int i = 0; i < num_language_presets; i++) {
    if (strcmp(lang, language_presets[i].name) == 0) {
      init_language(cfg, &language_presets[i]);
      if (cfg.track)
        register_project(final_path, cfg.project_name, lang);
      printf("\nInitialized %s project: %s\n", lang, cfg.project_name);
      return 0;
    }
  }

  fprintf(stderr, "Error: Unsupported language '%s'\n", lang);
  return 1;
}

/* --- Command Handlers --- */

static int handle_init(AppContext *ctx) {
  ProjectConfig cfg = {.use_git = true,
                       .use_readme = true,
                       .use_license = true,
                       .verbose = ctx->config.verbose,
                       .track = true,
                       .user_config = ctx->config};

  char *target_dir = NULL;
  static struct option long_opts[] = {{"dir", required_argument, 0, 'd'},
                                      {"no-git", no_argument, 0, 'g'},
                                      {"no-readme", no_argument, 0, 'r'},
                                      {"no-license", no_argument, 0, 'l'},
                                      {"no-track", no_argument, 0, 'n'},
                                      {"verbose", no_argument, 0, 'v'},
                                      {0, 0, 0, 0}};

  int opt;
  optind = 1; // Reset getopt for subcommand parsing
  while ((opt = getopt_long(ctx->argc, ctx->argv, "d:grlnv", long_opts,
                            NULL)) != -1) {
    switch (opt) {
    case 'd':
      target_dir = optarg;
      break;
    case 'g':
      cfg.use_git = false;
      break;
    case 'r':
      cfg.use_readme = false;
      break;
    case 'l':
      cfg.use_license = false;
      break;
    case 'n':
      cfg.track = false;
      break;
    case 'v':
      cfg.verbose = true;
      break;
    default:
      return 1;
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
    fprintf(stderr, "Error: Project name '%s' is unsafe or invalid.\n",
            cfg.project_name);
    return 1;
  }

  return execute_init(cfg, lang, work_dir);
}

static int handle_list(AppContext *ctx) {
  if (ctx->argc > 1 && (strcmp(ctx->argv[1], "-h") == 0 ||
                        strcmp(ctx->argv[1], "--help") == 0)) {
    printf("pman list - List all registered projects\n\n");
    printf("Usage: pman list\n\n");
    printf("Options:\n");
    printf("  %-20s %s\n", "-h, --help", "Show this help");
    printf("\nDescription:\n");
    printf("  Prints all projects currently tracked in the pman registry,\n");
    printf("  along with their paths and language.\n");
    return 0;
  }

  (void)ctx;
  list_projects();
  return 0;
}

static int handle_status(AppContext *ctx) {
  if (ctx->argc > 1 && (strcmp(ctx->argv[1], "-h") == 0 ||
                        strcmp(ctx->argv[1], "--help") == 0)) {
    printf("pman status - Show git status for all registered projects\n\n");
    printf("Usage: pman status\n\n");
    printf("Options:\n");
    printf("  %-20s %s\n", "-h, --help", "Show this help");
    printf("\nDescription:\n");
    printf(
        "  Iterates all projects in the registry and runs 'git status -s'\n");
    printf(
        "  on each one. Projects whose paths no longer exist are skipped.\n");
    return 0;
  }

  (void)ctx;
  check_all_status();
  return 0;
}

static int handle_prune(AppContext *ctx) {
  if (ctx->argc > 1 && (strcmp(ctx->argv[1], "-h") == 0 ||
                        strcmp(ctx->argv[1], "--help") == 0)) {
    printf("pman prune - Remove stale project paths from the registry\n\n");
    printf("Usage: pman prune\n\n");
    printf("Options:\n");
    printf("  %-20s %s\n", "-h, --help", "Show this help");
    printf("\nDescription:\n");
    printf("  Scans the registry for projects whose paths no longer exist\n");
    printf("  on the filesystem and removes them. The registry is updated\n");
    printf(
        "  atomically — changes only apply if at least one path was pruned.\n");
    printf("  Prints the number of entries removed.\n");
    return 0;
  }

  (void)ctx;
  prune_registry();
  return 0;
}

static int handle_uninstall(AppContext *ctx) {
  if (ctx->argc > 1 && (strcmp(ctx->argv[1], "-h") == 0 ||
                        strcmp(ctx->argv[1], "--help") == 0)) {
    printf("pman uninstall - Uninstall pman and remove all data\n\n");
    printf("Usage: pman uninstall\n\n");
    printf("Options:\n");
    printf("  %-20s %s\n", "-h, --help", "Show this help");
    printf("\nDescription:\n");
    printf("  Removes the pman binary, configuration file (~/.pmanrc)\n");
    printf("  and all data in ~/.config/pman/. You will be prompted\n");
    printf("  for confirmation before anything is deleted.\n");
    return 0;
  }

  (void)ctx;
  char response[10];
  char *user = getenv("USER");
  printf("pman will now be uninstalled for user %s [y/N]: ",
         user ? user : "unknown");
  if (fgets(response, sizeof(response), stdin) == NULL)
    return 1;

  if (response[0] != 'y' && response[0] != 'Y') {
    printf("Uninstall cancelled.\n");
    return 0;
  }

  printf("Removing configuration and data...\n");
  char cmd[PATH_MAX];
  char *home = getenv("HOME");
  if (home) {
    snprintf(cmd, sizeof(cmd), "rm -f %s/.pmanrc", home);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "rm -rf %s/.config/pman", home);
    system(cmd);
  }

  printf("To remove the binary, please run: sudo make uninstall\n");
  printf("pman configuration has been uninstalled.\n");
  return 0;
}

static int handle_export(AppContext *ctx) {

  // the input of this command is 'pman export [flag] <project_name>
  // <dest_path>' where the flag tells how to export the project

  if (ctx->argc <= 1 || (strcmp(ctx->argv[1], "-h") == 0 ||
                         strcmp(ctx->argv[1], "--help") == 0)) {
    printf("pman export - Export a project\n\n");
    printf("Usage: pman export [flag] <project_name> <dest_path>\n\n");
    printf("Options:\n");
    printf("  %-20s %s\n", "-h, --help", "Show this help");
    printf("  %-20s %s\n", "-z, --zipped",
           "Export the project as a .zip archive");
    printf("  %-20s %s\n", "-i, --info",
           "Print the project's README to stdout");
    printf("\nDescription:\n");
    printf("  Exports a project to the specified destination path.\n");
    printf("  Use -z to get a compressed archive, or -i to read\n");
    printf("  the project's README directly in the terminal.\n");

    return 0;
  }

  const char *arg = ctx->argv[1];
  if (strcmp(arg, "-z") == 0 || strcmp(arg, "--zipped") == 0) {

    // reading the project name
    char *project_name = ctx->argv[2];
    char src_path[256];
    if (get_project_path(project_name, src_path) == 1) {
      fprintf(stderr, "invalid project name\n");
      return 1;
    }

    // reading the destination path:
    char *dest_path = ctx->argv[3];
    if (!is_safe_path(dest_path)) {
      fprintf(stderr, "invalid destination path\n");
      return 1;
    }

    char zip_dest[PATH_MAX];
    snprintf(zip_dest, sizeof(zip_dest), "%s/%s.zip", dest_path, project_name);

    if (zip_file(src_path, zip_dest) != 0) {
      fprintf(stderr, "error during zipping the file\n");
      return 1;
    }

    printf("%s correctly zipped and saved in: %s\n", project_name, dest_path);
    return 0;
  }

  if (strcmp(arg, "-i") == 0 || strcmp(arg, "--info") == 0) {

    // getting the project path
    char *project_name = ctx->argv[2];
    char src_dir[256];

    if (get_project_path(project_name, src_dir) == 1) {
      fprintf(stderr, "invalid project name\n");
      return 1;
    }

    char path[4096];
    snprintf(path, sizeof(path), "%s/README.md", src_dir);
    struct stat st;

    if (stat(path, &st) != 0) {
      fprintf(stderr, "%s has no README\n", project_name);
      return 1;
    }

    FILE *f = fopen(path, "r");

    if (!f) {
      fprintf(stderr, "error reading the file\n");
    }

    char buf[1024];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
      fwrite(buf, 1, n, stdout);

    fclose(f);
    return 0;
  }

  return 1;
}
/* --- Dispatcher Configuration --- */

static const Command COMMANDS[] = {
    {"init", "Initialize a new project", handle_init},
    {"list", "List all registered projects", handle_list},
    {"status", "Check Git status of all projects", handle_status},
    {"prune", "Remove missing projects from registry", handle_prune},
    {"uninstall", "Uninstall pman and remove all data", handle_uninstall},
    {"export", "Export the file project for easy sharing", handle_export}};

static void print_usage(void) {
  printf("PMan - Project Manager\n\nUsage: pman <command> "
         "[options]\n\nCommands:\n");
  for (size_t i = 0; i < sizeof(COMMANDS) / sizeof(Command); i++) {
    printf("  %-8s %s\n", COMMANDS[i].name, COMMANDS[i].description);
  }
  printf("\nGlobal Options:\n"
         "  -v, --version       Print version\n"
         "  -v, --verbose       Enable verbose output\n");
  printf("\nInit Options:\n"
         "  -d, --dir <path>    Target directory\n"
         "  -g, --no-git        Skip Git\n"
         "  -r, --no-readme     Skip README\n"
         "  -l, --no-license    Skip LICENSE\n"
         "  -n, --no-track      Skip registration\n");
}

/* --- Main Entry Point --- */

int main(int argc, char *argv[]) {

  PManConfig user_cfg = load_config();

  if (argc < 2) {
    run_wizard(user_cfg);
    return 0;
  }

  int cmd_idx = 1; // the index of the expected index of the main command

  // checks if at argv[1] there's a flag by checking if first char is '-'
  if (argv[1][0] == '-') {
    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-V") == 0) {
      printf("pman %s\n", PMAN_VERSION);
      return 0;
    } else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
      print_usage();
      return 0;
      // here i assume that verbose is a global flag even if it is available
      // only for pman init
    } else if (strcmp(argv[1], "--verbose") == 0 ||
               strcmp(argv[1], "-v") == 0) {
      user_cfg.verbose = true;
      cmd_idx = 2;
    } else {
      fprintf(stderr, "pman: unknown option '%s'\n", argv[1]);
      fprintf(stderr, "Run 'pman -h' for usage.\n");
      return 1;
    }
  }

  if (cmd_idx >= argc) {
    run_wizard(user_cfg);
    return 0;
  }

  AppContext app = {.argc = argc, .argv = argv, .config = user_cfg};

  app.cmd_idx = cmd_idx;
  for (size_t i = 0; i < sizeof(COMMANDS) / sizeof(Command); i++) {
    if (strcmp(argv[cmd_idx], COMMANDS[i].name) == 0) {
      app.argc = argc - cmd_idx;
      app.argv = argv + cmd_idx;
      return COMMANDS[i].handler(&app);
    }
  }

  print_usage();
  return 1;
}
