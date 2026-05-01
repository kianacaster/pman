#!/bin/bash
cat <<'EOF' > "main.sh"
#!/bin/bash
echo "Hello from PMan Bash project!"
EOF
chmod +x main.sh

if [ "$PMAN_USE_README" = "true" ]; then
  cat <<EOF > README.md
# $PMAN_PROJECT_NAME
**Author:** $PMAN_AUTHOR ($PMAN_EMAIL)
EOF
fi
