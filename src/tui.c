#define _GNU_SOURCE
#include "tui.h"
#include "languages.h"
#include "registry.h"
#include "utils.h"
#include <libgen.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define BOLD_CYAN "\033[1;36m"
#define RESET "\033[0m"

static void clear_stdin(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

void run_tui(PManConfig user_cfg) {
  printf(BOLD_CYAN "--- pman: Project Setup Wizard ---\n" RESET);
  
  const char *home = getenv("HOME");
  if (home) {
      char templates_dir[PATH_MAX];
      snprintf(templates_dir, sizeof(templates_dir), "%s/.config/pman/templates", home);
      DIR *d = opendir(templates_dir);
      if (d) {
          printf("\nAvailable Templates:\n");
          struct dirent *entry;
          while ((entry = readdir(d)) != NULL) {
              if (entry->d_name[0] == '.') continue;
              printf("  - %s\n", entry->d_name);
          }
          closedir(d);
      }
  }

  char lang[32], name[128], buf[8];

  printf("\nLanguage/Template: ");
  if (scanf("%31s", lang) != 1) return;
  clear_stdin();

  printf("Project Name (leave blank for current directory): ");
  if (fgets(name, sizeof(name), stdin)) {
    name[strcspn(name, "\n")] = 0;
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

  printf("Track in registry? (Y/n): ");
  fgets(buf, sizeof(buf), stdin);
  bool track = (buf[0] != 'n' && buf[0] != 'N');

  ProjectConfig cfg = {use_git, use_readme, use_license, user_cfg.verbose,
                       track,   NULL,       user_cfg};

  const char *work_dir = (strlen(name) > 0) ? name : ".";

  if (strlen(name) > 0) {
    cfg.project_name = name;
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

  // Note: if cfg.project_name was strdupped in the "else" block, 
  // we are technically leaking it here since it's a TUI exit, 
  // but for a CLI tool it's acceptable. For cleanliness:
  // if (work_dir[0] == '.') free((char*)cfg.project_name);
}
