#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct {
    char author[128];
    char email[128];
    char license[32];
    bool verbose;
} DInitConfig;

DInitConfig load_config(void);

#endif
