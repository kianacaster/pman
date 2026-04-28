#define _POSIX_C_SOURCE 200809L
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

#define BOLD_CYAN "\033[1;36m"
#define RESET "\033[0m"

static void clear_stdin(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

void run_tui(PManConfig user_cfg) {
  printf(BOLD_CYAN "--- pman: Project Setup Wizard ---\n" RESET);
  char lang[32], name[128], buf[8];
  printf("\nLanguage: ");
  scanf("%31s", lang);
  clear_stdin();
  printf("Project Name (leave blank for current directory): ");
  if (fgets(name, sizeof(name), stdin)) {
    name[strcspn(name, "\n")] = 0;
  }
  if (strlen(name) > 0 && !is_safe_name(name)) {
    fprintf(stderr, "Error: Unsafe name.\n");
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
  printf("Track in registry? (Y/n): ");
  fgets(buf, sizeof(buf), stdin);
  bool track = (buf[0] != 'n' && buf[0] != 'N');

  ProjectConfig cfg = {use_git, use_readme, use_license, user_cfg.verbose,
                       track,   NULL,       user_cfg};
  char final_path[PATH_MAX];
  bool needs_free = false;
  if (strlen(name) > 0) {
    cfg.project_name = name;
    if (!create_and_enter_dir(cfg.project_name))
      return;
    if (!getcwd(final_path, sizeof(final_path)))
      return;
  } else {
    if (!getcwd(final_path, sizeof(final_path)))
      return;
    char *tmp = strdup(final_path);
    cfg.project_name = strdup(basename(tmp));
    free(tmp);
    needs_free = true;
  }

  if (!is_safe_name(cfg.project_name)) {
    fprintf(stderr, "Error: Current directory name is unsafe.\n");
    if (needs_free) free((char *)cfg.project_name);
    return;
  }

  char *custom_path = get_custom_template_path(lang);
  if (custom_path) {
    init_custom(cfg, custom_path);
    if (cfg.track)
      register_project(final_path, cfg.project_name, lang);
    free(custom_path);
    if (needs_free) free((char *)cfg.project_name);
    return;
  }

  for (int i = 0; i < num_language_presets; i++) {
    if (strcmp(lang, language_presets[i].name) == 0) {
      init_language(cfg, &language_presets[i]);
      if (cfg.track)
        register_project(final_path, cfg.project_name, lang);
      if (needs_free) free((char *)cfg.project_name);
      return;
    }
  }
  printf("Error: Unsupported language: %s\n", lang);
  if (needs_free) free((char *)cfg.project_name);
}
