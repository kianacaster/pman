#ifndef REGISTRY_H
#define REGISTRY_H

#include "languages.h"

void register_project(const char *path, const char *name, const char *lang);
int get_project_path(const char *name, char *dest);
void list_projects(void);
void check_all_status(void);
void prune_registry(void);

#endif
