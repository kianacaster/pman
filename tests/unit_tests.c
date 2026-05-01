#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "utils.h"
#include "config.h"
#include "registry.h"

#define TEST_HOME "test_home_unit"

void setup_test_env() {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "rm -rf %s", TEST_HOME);
    system(cmd);
    mkdir(TEST_HOME, 0755);
    setenv("HOME", TEST_HOME, 1);
}

void cleanup_test_env() {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "rm -rf %s", TEST_HOME);
    system(cmd);
}

void test_is_safe_name() {
    printf("Testing is_safe_name...\n");
    assert(is_safe_name("my_project") == true);
    assert(is_safe_name("project-123") == true);
    assert(is_safe_name("ProjectName") == true);
    assert(is_safe_name("a") == true);
    
    assert(is_safe_name("my;project") == false);
    assert(is_safe_name("my project") == false);
    assert(is_safe_name("my|project") == false);
    assert(is_safe_name("../etc") == false);
    assert(is_safe_name("") == false);
    assert(is_safe_name(NULL) == false);
    assert(is_safe_name("very_long_name_exceeding_one_hundred_and_twenty_eight_characters_long_name_exceeding_one_hundred_and_twenty_eight_characters_long_name") == false);
    printf("  [PASS] is_safe_name\n");
}

void test_is_safe_path() {
    printf("Testing is_safe_path...\n");
    assert(is_safe_path("/home/user/projects") == true);
    assert(is_safe_path("./local/path") == true);
    assert(is_safe_path("relative/path") == true);
    
    assert(is_safe_path("/home/user;rm -rf /") == false);
    assert(is_safe_path("/home/user|echo hacked") == false);
    assert(is_safe_path("/home/user/$(whoami)") == false);
    assert(is_safe_path("../traversal") == false);
    assert(is_safe_path("/path/with/../traversal") == false);
    assert(is_safe_path("path/with/&/ampersand") == false);
    printf("  [PASS] is_safe_path\n");
}

void test_config_loading() {
    printf("Testing config loading...\n");
    
    // Test default config
    PManConfig cfg = load_config();
    if (strcmp(cfg.author, "Unknown Author") != 0) {
        printf("  [FAIL] Expected 'Unknown Author', got '%s'\n", cfg.author);
        assert(0);
    }
    assert(cfg.verbose == false);

    // Test custom config
    FILE *f = fopen(TEST_HOME "/.pmanrc", "w");
    fprintf(f, "author=Test User\nemail=test@example.com\nlicense=GPL\nverbose=true\n");
    fclose(f);

    cfg = load_config();
    assert(strcmp(cfg.author, "Test User") == 0);
    assert(strcmp(cfg.email, "test@example.com") == 0);
    assert(strcmp(cfg.license, "GPL") == 0);
    assert(cfg.verbose == true);
    printf("  [PASS] config_loading\n");
}

void test_registry_operations() {
    printf("Testing registry operations...\n");
    
    // Ensure directory exists for registry
    mkdir(TEST_HOME "/.config", 0755);
    mkdir(TEST_HOME "/.config/pman", 0755);

    // Create a dummy project directory
    mkdir(TEST_HOME "/dummy_proj", 0755);
    
    register_project(TEST_HOME "/dummy_proj", "dummy", "c");
    
    // Check if registry file exists and has content
    FILE *f = fopen(TEST_HOME "/.config/pman/registry.txt", "r");
    assert(f != NULL);
    char line[1024];
    assert(fgets(line, sizeof(line), f) != NULL);
    assert(strstr(line, "dummy") != NULL);
    assert(strstr(line, "c") != NULL);
    fclose(f);

    // Test pruning
    rmdir(TEST_HOME "/dummy_proj");
    prune_registry();
    
    // Registry should now be empty (or missing the line)
    f = fopen(TEST_HOME "/.config/pman/registry.txt", "r");
    if (f) {
        assert(fgets(line, sizeof(line), f) == NULL);
        fclose(f);
    }
    printf("  [PASS] registry_operations\n");
}

void test_utils_files() {
    printf("Testing utility file operations...\n");
    
    assert(write_file(TEST_HOME "/test.txt", "hello world") == true);
    FILE *f = fopen(TEST_HOME "/test.txt", "r");
    char buf[16];
    fgets(buf, sizeof(buf), f);
    assert(strcmp(buf, "hello world") == 0);
    fclose(f);

    assert(write_file_fmt(TEST_HOME "/fmt.txt", "Hello %s", "PMan") == true);
    f = fopen(TEST_HOME "/fmt.txt", "r");
    fgets(buf, sizeof(buf), f);
    assert(strcmp(buf, "Hello PMan") == 0);
    fclose(f);

    assert(create_and_enter_dir(TEST_HOME "/subdir") == true);
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    assert(strstr(cwd, "subdir") != NULL);
    chdir(".."); // Go back to original test dir
    printf("  [PASS] utils_files\n");
}

int main() {
    printf("--- PMan: Comprehensive Unit Test Suite ---\n");
    setup_test_env();

    test_is_safe_name();
    test_is_safe_path();
    test_config_loading();
    test_registry_operations();
    test_utils_files();

    cleanup_test_env();
    printf("All unit tests passed.\n");
    return 0;
}
