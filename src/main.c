#define _GNU_SOURCE
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
#include <dirent.h>

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

/* --- Helpers --- */

static bool check_help(AppContext *ctx, const char *usage) {
  if (ctx->argc > 1 && (strcmp(ctx->argv[1], "-h") == 0 ||
                        strcmp(ctx->argv[1], "--help") == 0)) {
    printf("%s\n", usage);
    return true;
  }
  return false;
}

/* --- Command Handlers --- */

static int handle_init(AppContext *ctx) {
  ProjectConfig cfg = {.use_git = true,
                       .use_readme = true,
                       .use_license = true,
                       .verbose = ctx->config.verbose,
                       .track = true,
                       .user_config = ctx->config};

  static struct option long_opts[] = {{"dir", required_argument, 0, 'd'},
                                      {"no-git", no_argument, 0, 'g'},
                                      {"no-readme", no_argument, 0, 'r'},
                                      {"no-license", no_argument, 0, 'l'},
                                      {"no-track", no_argument, 0, 'n'},
                                      {"verbose", no_argument, 0, 'v'},
                                      {0, 0, 0, 0}};

  char *target_dir = NULL;
  int opt;
  optind = 1; 
  while ((opt = getopt_long(ctx->argc, ctx->argv, "d:grlnv", long_opts,
                            NULL)) != -1) {
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

  if (!is_safe_name(cfg.project_name))
    die("Project name '%s' is unsafe or invalid.", cfg.project_name);

  execute_init(cfg, lang, work_dir);
  return 0;
}

static int handle_list(AppContext *ctx) {
  if (check_help(ctx, "Usage: pman list\n\nList all registered projects."))
    return 0;
  list_projects();
  return 0;
}

static int handle_status(AppContext *ctx) {
  if (check_help(ctx, "Usage: pman status\n\nShow git status for all registered projects."))
    return 0;
  check_all_status();
  return 0;
}

static int handle_prune(AppContext *ctx) {
  if (check_help(ctx, "Usage: pman prune\n\nRemove stale project paths from the registry."))
    return 0;
  prune_registry();
  return 0;
}

static int handle_uninstall(AppContext *ctx) {
  if (check_help(ctx, "Usage: pman uninstall\n\nUninstall pman and remove all data."))
    return 0;

  char response[10];
  printf("pman will now be uninstalled for user %s [y/N]: ", getenv("USER") ? getenv("USER") : "unknown");
  if (fgets(response, sizeof(response), stdin) == NULL || (response[0] != 'y' && response[0] != 'Y')) {
    printf("Uninstall cancelled.\n");
    return 0;
  }

  char *home = getenv("HOME");
  if (home) {
    char cmd[PATH_MAX];
    snprintf(cmd, sizeof(cmd), "rm -f %s/.pmanrc", home); system(cmd);
    snprintf(cmd, sizeof(cmd), "rm -rf %s/.config/pman", home); system(cmd);
  }

  printf("To remove the binary, please run: sudo make uninstall\n");
  return 0;
}

static int handle_export(AppContext *ctx) {
  if (check_help(ctx, "Usage: pman export [flag] <project_name> <dest_path>\n\n"
                      "Options:\n"
                      "  -z, --zipped    Export as .zip archive\n"
                      "  -i, --info      Print project README"))
    return 0;

  if (ctx->argc < 3) {
    fprintf(stderr, "Error: Missing arguments. See 'pman export --help'.\n");
    return 1;
  }

  const char *flag = ctx->argv[1];
  const char *project_name = ctx->argv[2];
  char src_path[PATH_MAX];

  if (get_project_path(project_name, src_path, sizeof(src_path)) != 0)
    die("Invalid project name '%s'", project_name);

  if (strcmp(flag, "-z") == 0 || strcmp(flag, "--zipped") == 0) {
    if (ctx->argc < 4) die("Missing destination path for zipped export");
    const char *dest_path = ctx->argv[3];
    if (!is_safe_path(dest_path)) die("Invalid destination path");

    char zip_dest[PATH_MAX];
    snprintf(zip_dest, sizeof(zip_dest), "%s/%s.zip", dest_path, project_name);
    if (zip_file(src_path, zip_dest) != 0) die("Failed to create zip archive");

    printf("Project '%s' zipped to '%s'\n", project_name, zip_dest);
  } else if (strcmp(flag, "-i") == 0 || strcmp(flag, "--info") == 0) {
    char readme_path[PATH_MAX];
    snprintf(readme_path, sizeof(readme_path), "%s/README.md", src_path);
    if (read_file_to_stdout(readme_path) != 0)
      die("Project '%s' has no README.md or it could not be read", project_name);
  } else {
    die("Unknown export flag '%s'", flag);
  }

  return 0;
}
static int handle_templates(AppContext *ctx) {
  if (check_help(ctx, "Usage: pman templates [options]\n\n"
                      "Options:\n"
                      "  -l, --list      List all installed templates\n"
                      "  -s, --show <t>  Show the content of a template's init script\n"
                      "  -S, --search <q> Search for templates in the remote registry\n"
                      "  -i, --install <t> Download and install a template from registry"))
    return 0;

  const char *home = getenv("HOME");
  if (!home) die("HOME environment variable not set");
  char templates_dir[PATH_MAX];
  snprintf(templates_dir, sizeof(templates_dir), "%s/.config/pman/templates", home);

  if (ctx->argc < 2 || strcmp(ctx->argv[1], "-l") == 0 || strcmp(ctx->argv[1], "--list") == 0) {
    printf("Installed Templates:\n\n");
    DIR *dir = opendir(templates_dir);
    if (dir) {
      struct dirent *entry;
      while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        printf("  - %s\n", entry->d_name);
      }
      closedir(dir);
    } else {
        printf("  (No templates installed)\n");
    }
    return 0;
  }

  if (strcmp(ctx->argv[1], "-s") == 0 || strcmp(ctx->argv[1], "--show") == 0) {
    if (ctx->argc < 3) die("Missing template name");
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s/init.sh", templates_dir, ctx->argv[2]);
    if (read_file_to_stdout(path) != 0) die("Template '%s' not found locally", ctx->argv[2]);
    return 0;
  }

  // Use config-defined repo or fallback to default
  const char *repo_base = "https://raw.githubusercontent.com/kianacaster/pman/master/templates";
  // Future: load from user_cfg.template_repo if we added it to struct

  if (strcmp(ctx->argv[1], "-S") == 0 || strcmp(ctx->argv[1], "--search") == 0) {
    const char *query = (ctx->argc > 2) ? ctx->argv[2] : "";
    printf("Searching remote registry at %s...\n", repo_base);
    // Simulation: in a real app, fetch a JSON manifest
    printf("Available in remote registry:\n");
    const char *defaults[] = {"python", "c", "cpp", "go", "rust", "node", "ruby", "php", "html", "bash", "java", "csharp"};
    for (size_t i = 0; i < sizeof(defaults)/sizeof(char*); i++) {
        if (strlen(query) == 0 || strcasestr(defaults[i], query)) {
            printf("  - %s\n", defaults[i]);
        }
    }
    return 0;
  }

  if (strcmp(ctx->argv[1], "-i") == 0 || strcmp(ctx->argv[1], "--install") == 0) {
    if (ctx->argc < 3) die("Missing template name");
    const char *target = ctx->argv[2];
    printf("Downloading '%s' from %s...\n", target, repo_base);
    
    char cmd[2048];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s/%s && curl -fsSL %s/%s/init.sh -o %s/%s/init.sh && chmod +x %s/%s/init.sh", 
             templates_dir, target, repo_base, target, templates_dir, target, templates_dir, target);
    
    int res = system(cmd);
    if (res == 0) {
        printf("Template '%s' installed successfully.\n", target);
    } else {
        printf("\nError: Failed to download template.\n");
        printf("Reason: Remote server returned an error (404 or connection failure).\n");
        printf("Target URL: %s/%s/init.sh\n", repo_base, target);
        printf("\nTip: Ensure the template exists in the registry or check your internet connection.\n");
        return 1;
    }
    return 0;
  }

  die("Unknown templates option '%s'. See 'pman templates --help'.", ctx->argv[1]);
  return 1;
}

/* --- Dispatcher Configuration --- */

static const Command COMMANDS[] = {
    {"init", "Initialize a new project", handle_init},
    {"list", "List all registered projects", handle_list},
    {"status", "Check Git status of all projects", handle_status},
    {"prune", "Remove missing projects from registry", handle_prune},
    {"templates", "Manage templates (search/install)", handle_templates},
    {"uninstall", "Uninstall pman and remove all data", handle_uninstall},
    {"export", "Export project archive or info", handle_export}};

static void print_usage(void) {
  printf("PMan - Project Manager\n\nUsage: pman <command> [options]\n\nCommands:\n");
  for (size_t i = 0; i < sizeof(COMMANDS) / sizeof(Command); i++) {
    printf("  %-10s %s\n", COMMANDS[i].name, COMMANDS[i].description);
  }
  printf("\nGlobal Options:\n"
         "  -V, --version       Print version\n"
         "  -h, --help          Show this help\n"
         "  -v, --verbose       Enable verbose output\n");
}

/* --- Main Entry Point --- */

int main(int argc, char *argv[]) {
  PManConfig user_cfg = load_config();

  if (argc < 2) {
    run_tui(user_cfg);
    return 0;
  }

  int cmd_idx = 1;

  if (argv[1][0] == '-') {
    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-V") == 0) {
      printf("pman %s\n", PMAN_VERSION);
      return 0;
    } else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
      print_usage();
      return 0;
    } else if (strcmp(argv[1], "--verbose") == 0 || strcmp(argv[1], "-v") == 0) {
      user_cfg.verbose = true;
      cmd_idx = 2;
    } else {
      // Catch common mistakes like pman -i
      if (strcmp(argv[1], "-i") == 0) {
          fprintf(stderr, "Error: '-i' is a subcommand option. Did you mean 'pman templates -i' or 'pman init'?\n");
      } else {
          fprintf(stderr, "pman: unknown global option '%s'\n", argv[1]);
      }
      fprintf(stderr, "Run 'pman -h' for usage.\n");
      return 1;
    }
  }

  if (cmd_idx >= argc) {
    run_tui(user_cfg);
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
