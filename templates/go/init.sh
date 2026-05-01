#!/bin/bash
cat <<'EOF' > "main.go"
package main
import "fmt"
func main() {
    fmt.Println("Hello from PMan Go project!")
}
EOF

if [ -n "$PMAN_PROJECT_NAME" ]; then
    go mod init "$PMAN_PROJECT_NAME"
fi

if [ "$PMAN_USE_README" = "true" ]; then
  cat <<EOF > README.md
# $PMAN_PROJECT_NAME
**Author:** $PMAN_AUTHOR ($PMAN_EMAIL)
EOF
fi
