#!/bin/bash
cat <<'EOF' > "index.html"
<!DOCTYPE html>
<html>
<head><title>PMan Web Project</title></head>
<body><h1>Hello from PMan!</h1></body>
</html>
EOF

if [ "$PMAN_USE_README" = "true" ]; then
  cat <<EOF > README.md
# $PMAN_PROJECT_NAME
**Author:** $PMAN_AUTHOR ($PMAN_EMAIL)
EOF
fi
