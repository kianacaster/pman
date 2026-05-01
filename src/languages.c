#define _POSIX_C_SOURCE 200112L
#include "languages.h"
#include "utils.h"
#include "registry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

static void init_git(bool verbose) {
  run_command("git init", verbose);
  run_command("git branch -m main", verbose);
  run_command("git add .", verbose);
  run_command("git commit -m 'Initial commit from PMan'", verbose);
}

void init_custom(ProjectConfig cfg, const char *script_path) {
  setenv("PMAN_PROJECT_NAME", cfg.project_name, 1);
  setenv("PMAN_AUTHOR", cfg.user_config.author, 1);
  setenv("PMAN_EMAIL", cfg.user_config.email, 1);
  setenv("PMAN_LICENSE", cfg.user_config.license, 1);
  setenv("PMAN_USE_README", cfg.use_readme ? "true" : "false", 1);
  
  run_command(script_path, cfg.verbose);
  
  if (cfg.use_git && access(".git", F_OK) != 0) {
      init_git(cfg.verbose);
  }
}

void execute_init(ProjectConfig cfg, const char *lang, const char *path) {
  char final_path[PATH_MAX];

  if (!create_and_enter_dir(path))
    die("Could not access directory '%s'", path);

  if (!getcwd(final_path, sizeof(final_path)))
    die("getcwd failed");

  char *custom_path = get_custom_template_path(lang);
  if (custom_path) {
    init_custom(cfg, custom_path);
    if (cfg.track)
      register_project(final_path, cfg.project_name, lang);
    printf("\nInitialized project: %s (template: %s)\n", cfg.project_name, lang);
    free(custom_path);
    return;
  }

  die("Template '%s' not found locally. Try: pman templates -S %s", lang, lang);
}
