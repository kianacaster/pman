#ifndef REGISTRY_H
#define REGISTRY_H

#include "languages.h"

void register_project(const char *path, const char *name, const char *lang);
void list_projects(void);
void check_all_status(void);
void prune_registry(void);

#endif
