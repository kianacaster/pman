#!/bin/bash

export TEST_DIR=$(pwd)/test_run
rm -rf "$TEST_DIR"
mkdir -p "$TEST_DIR/home"
export HOME="$TEST_DIR/home"
PMAN="$(pwd)/pman"

cat <<EOF > "$HOME/.dinitrc"
author=Test
email=test@test.com
license=MIT
verbose=false
EOF

mkdir -p "$TEST_DIR/run1" "$TEST_DIR/run2"

fail() { echo "FAIL: $1"; exit 1; }

get_dir_signature() {
    find "$1" -type f -not -path '*/.git/*' | sort | xargs cat | sha256sum | awk '{print $1}'
}

LANGUAGES=("python" "c" "cpp" "java" "go" "rust" "node" "ruby" "php" "html" "bash")

cd "$TEST_DIR/run1" || exit 1
for lang in "${LANGUAGES[@]}"; do
    if [[ "$lang" =~ ^(rust|node|go)$ ]] && ! command -v "$lang" &>/dev/null; then continue; fi
    $PMAN init "$lang" "t_$lang" >/dev/null 2>&1 || fail "$lang"
done

cd "$TEST_DIR/run1" && $PMAN init c integrity >/dev/null 2>&1
SIG1=$(get_dir_signature "integrity")
cd "$TEST_DIR/run2" && $PMAN init c integrity >/dev/null 2>&1
SIG2=$(get_dir_signature "integrity")
[ "$SIG1" != "$SIG2" ] && fail "Integrity"

cd "$TEST_DIR" || exit 1
$PMAN init python "inj;touch H" 2>/dev/null
[ -f H ] && fail "Injection"
$PMAN init -d "../out" c 2>/dev/null
[ -d "$TEST_DIR/../out" ] && fail "Traversal"

echo "OK"
rm -rf "$TEST_DIR"
