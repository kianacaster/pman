#!/bin/bash
cat <<'EOF' > "main.c"
#include <stdio.h>
int main() {
    printf("Hello from PMan C project!\n");
    return 0;
}
EOF

cat <<'EOF' > ".gitignore"
*.o
main
EOF

cat <<'EOF' > "Makefile"
all:
	gcc main.c -o main
clean:
	rm -f main
EOF

if [ "$PMAN_USE_README" = "true" ]; then
  cat <<EOF > README.md
# $PMAN_PROJECT_NAME
**Author:** $PMAN_AUTHOR ($PMAN_EMAIL)
EOF
fi
