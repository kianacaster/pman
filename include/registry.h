#ifndef REGISTRY_H
#define REGISTRY_H

#include <stddef.h>
#include "languages.h"

void register_project(const char *path, const char *name, const char *lang);
int get_project_path(const char *name, char *dest, size_t dest_size);
void list_projects(void);
void check_all_status(void);
void prune_registry(void);

#endif
