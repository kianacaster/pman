#!/bin/bash
cat <<'EOF' > "index.js"
console.log('Hello from PMan Node project!');
EOF

cat <<'EOF' > ".gitignore"
node_modules/
dist/
.env
EOF

npm init -y

if [ "$PMAN_USE_README" = "true" ]; then
  cat <<EOF > README.md
# $PMAN_PROJECT_NAME
**Author:** $PMAN_AUTHOR ($PMAN_EMAIL)
EOF
fi
