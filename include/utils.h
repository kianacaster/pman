#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>

void die(const char *fmt, ...);
int run_command(const char *cmd, bool verbose);
int run_command_args(char *const argv[], bool verbose);
int zip_file(const char *src_path, const char *dest_path);

bool write_file(const char *path, const char *content);
bool write_file_fmt(const char *path, const char *fmt, ...);
int read_file_to_stdout(const char *path);

bool create_and_enter_dir(const char *name);
void mkdir_p(const char *path);
char* get_custom_template_path(const char *lang);
bool is_safe_name(const char *name);
bool is_safe_path(const char *path);

#endif
