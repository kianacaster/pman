#!/bin/bash
cat <<'EOF' > "main.py"
def main():
    print("Hello from PMan!")

if __name__ == "__main__":
    main()
EOF

cat <<'EOF' > ".gitignore"
__pycache__/
*.py[cod]
*$py.class
.venv/
venv/
ENV/
EOF

touch requirements.txt
python3 -m venv .venv

if [ "$PMAN_USE_README" = "true" ]; then
  cat <<EOF > README.md
# $PMAN_PROJECT_NAME
**Author:** $PMAN_AUTHOR ($PMAN_EMAIL)
EOF
fi
