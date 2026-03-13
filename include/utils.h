#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

int run_command(const char *cmd, bool verbose);
int zip_file(const char *src_path, const char *dest_path);
bool write_to_file(const char *path, const char *content);
bool write_formatted(const char *path, const char *fmt, const char *arg);
bool write_readme(const char *path, const char *fmt, const char *name, const char *author, const char *email);
bool create_and_enter_dir(const char *name);
char* get_custom_template_path(const char *lang);
bool is_safe_name(const char *name);
bool is_safe_path(const char *path);

#endif
