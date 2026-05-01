#!/bin/bash

export TEST_DIR=$(pwd)/test_run_int
rm -rf "$TEST_DIR"
mkdir -p "$TEST_DIR/home"
export HOME="$TEST_DIR/home"
PMAN="$(pwd)/pman"

# Setup initial .pmanrc
cat <<EOF > "$HOME/.pmanrc"
author=TestBot
email=bot@test.com
license=MIT
verbose=false
EOF

# Deploy templates to test HOME
mkdir -p "$HOME/.config/pman/templates"
cp -r "$(pwd)/templates/"* "$HOME/.config/pman/templates/"

fail() { echo "FAIL: $1"; exit 1; }

echo "--- PMan: Comprehensive Integration Test Suite ---"

# 1. Test all language presets basic init
mkdir -p "$TEST_DIR/basic"
cd "$TEST_DIR/basic" || fail "cd basic"
LANGUAGES=("python" "c" "cpp" "java" "go" "rust" "node" "ruby" "php" "html" "bash")
for lang in "${LANGUAGES[@]}"; do
    echo "  Testing init $lang..."
    # Skip languages that require binaries if they aren't installed (node, rust, go)
    if [[ "$lang" =~ ^(rust|node|go)$ ]] && ! command -v "$lang" &>/dev/null; then 
        echo "    (Skipping binary-dependent init for $lang)"
        continue
    fi
    $PMAN init "$lang" "proj_$lang" || fail "init $lang"
    [ -d "proj_$lang" ] || fail "dir not created for $lang"
done

# 2. Test init flags
echo "  Testing init flags (-g, -r, -l, -n)..."
cd "$TEST_DIR/basic" || fail "cd basic"
$PMAN init -g -r -l -n c flag_test >/dev/null 2>&1 || fail "init flags"
[ -d "flag_test" ] || fail "flag_test dir missing"
[ -d "flag_test/.git" ] && fail "git initialized but -g used"
[ -f "flag_test/README.md" ] && fail "README created but -r used"
[ -f "flag_test/LICENSE" ] && fail "LICENSE created but -l used"

# 3. Test Registry (list, status, prune)
echo "  Testing registry commands..."
# Proj 'flag_test' was init with -n, so it shouldn't be in list
$PMAN list | grep "flag_test" && fail "flag_test found in list but -n used"

# Add a trackable project
$PMAN init c track_me >/dev/null 2>&1
$PMAN list | grep "track_me" >/dev/null || fail "track_me missing from list"

# Test Status (should run git status)
$PMAN status | grep "\[track_me\]" >/dev/null || fail "status output missing project header"

# Test Prune
rm -rf track_me
$PMAN list | grep "track_me" | grep "\[MISSING\]" >/dev/null || fail "MISSING tag not shown for deleted project"
$PMAN prune | grep "Pruned: 1" >/dev/null || fail "prune failed to remove missing project"
$PMAN list | grep "track_me" && fail "track_me still in list after prune"

# 4. Test Custom Templates
echo "  Testing custom templates..."
mkdir -p "$HOME/.config/pman/templates/myscript"
cat <<EOF > "$HOME/.config/pman/templates/myscript/init.sh"
#!/bin/bash
touch CUSTOM_SUCCESS
echo "Project: \$PMAN_PROJECT_NAME" > INFO
EOF
chmod +x "$HOME/.config/pman/templates/myscript/init.sh"

$PMAN init myscript custom_test >/dev/null 2>&1 || fail "custom init"
[ -f "custom_test/CUSTOM_SUCCESS" ] || fail "custom script didn't run"
grep "Project: custom_test" "custom_test/INFO" >/dev/null || fail "env vars not passed to custom script"

# 5. Security & Edge Cases
echo "  Testing security and edge cases..."
# Path Traversal
$PMAN init c "../../hacked" 2>/dev/null && fail "allowed path traversal"
# Command Injection
$PMAN init c "proj;touch INJECTED" 2>/dev/null
[ -f "INJECTED" ] && fail "allowed command injection in project name"
# Invalid Language
$PMAN init non-existent-lang myproj 2>/dev/null && fail "allowed invalid language"

echo "OK - All integration tests passed."
rm -rf "$TEST_DIR"
