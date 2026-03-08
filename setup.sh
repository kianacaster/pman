#!/bin/bash

[ ! -f "pman" ] && exit 1

RC_FILE="$HOME/.pmanrc"
CUR_AUTHOR=$(grep "author=" "$RC_FILE" 2>/dev/null | cut -d'=' -f2)
CUR_EMAIL=$(grep "email=" "$RC_FILE" 2>/dev/null | cut -d'=' -f2)
CUR_LICENSE=$(grep "license=" "$RC_FILE" 2>/dev/null | cut -d'=' -f2)

read -p "Author [$CUR_AUTHOR]: " NEW_AUTHOR
read -p "Email [$CUR_EMAIL]: " NEW_EMAIL
read -p "License [$CUR_LICENSE]: " NEW_LICENSE

cat <<EOF > "$RC_FILE"
author=${NEW_AUTHOR:-$CUR_AUTHOR}
email=${NEW_EMAIL:-$CUR_EMAIL}
license=${NEW_LICENSE:-$CUR_LICENSE}
verbose=false
EOF
