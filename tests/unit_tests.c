#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "utils.h"

void test_is_safe_name() {
    printf("Testing is_safe_name...\n");
    assert(is_safe_name("my_project") == true);
    assert(is_safe_name("project-123") == true);
    assert(is_safe_name("ProjectName") == true);
    
    assert(is_safe_name("my;project") == false);
    assert(is_safe_name("my project") == false);
    assert(is_safe_name("my|project") == false);
    assert(is_safe_name("../etc") == false);
    assert(is_safe_name("") == false);
    assert(is_safe_name(NULL) == false);
    printf("  [PASS] is_safe_name\n");
}

void test_is_safe_path() {
    printf("Testing is_safe_path...\n");
    assert(is_safe_path("/home/user/projects") == true);
    assert(is_safe_path("./local/path") == true);
    
    assert(is_safe_path("/home/user;rm -rf /") == false);
    assert(is_safe_path("/home/user|echo hacked") == false);
    assert(is_safe_path("/home/user/$(whoami)") == false);
    printf("  [PASS] is_safe_path\n");
}

int main() {
    printf("--- pman: Unit Test Suite ---\n");
    test_is_safe_name();
    test_is_safe_path();
    printf("All unit tests passed successfully.\n");
    return 0;
}
