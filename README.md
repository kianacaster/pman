# pman

Project lifecycle manager for C99. Bootstraps projects and tracks repository health across the filesystem.

## Motivation
Fragmented project setups increase cognitive load and lead to inconsistent environments. `pman` provides a single interface to automate bootstrapping and maintain a centralized index of scattered projects.

## Core Features

### Standardization
Ensures every project starts with a consistent structure. It generates boilerplate code, `.gitignore` files, and entry points based on 12 built-in language presets or user-defined templates.

### Automation
Replaces repetitive setup tasks with one command.
- Initializes Git repositories with standard branch naming.
- Creates virtual environments and dependency files.
- Generates LICENSE and README files using author metadata.
- Executes language-specific initialization (e.g., `go mod init`, `npm init`).

### Centralization
Maintains a system-wide registry to audit projects across different directories.
- `pman status`: Batch audits Git health for all registered projects.
- `pman list`: Identifies active and missing project paths.
- `pman prune`: Removes dead paths from the index.

### Extensibility
Users can define custom bootstrapping logic by placing shell scripts in `~/.config/pman/templates/`. These scripts receive project metadata via environment variables for specialized setups.

## Custom Templates
Create a directory and an executable script:
1. `mkdir -p ~/.config/pman/templates/<template_name>`
2. `touch ~/.config/pman/templates/<template_name>/init.sh`
3. `chmod +x ~/.config/pman/templates/<template_name>/init.sh`

The script receives these environment variables:
- `PMAN_PROJECT_NAME`: Target project name.
- `PMAN_AUTHOR`: Author name from config.
- `PMAN_EMAIL`: Author email from config.
- `PMAN_LICENSE`: License type from config.

Example `init.sh`:
```bash
#!/bin/bash
mkdir src docs
touch src/main.c
echo "Created by $PMAN_AUTHOR" > README.md
```
Invoke the template:
`pman init <template_name> [name]`

## Install
```bash
make
make setup
sudo make install
```

## Use
Initialize a project in a new directory:
`pman init <lang> <name>`

Initialize in the current directory:
`pman init <lang>`

Audit Git status of all projects:
`pman status`

List all projects:
`pman list`

Clean the registry:
`pman prune`

### Flags
- `-d, --dir <path>`: Specify target directory.
- `-g, --no-git`: Disable Git initialization.
- `-r, --no-readme`: Disable README generation.
- `-l, --no-license`: Disable LICENSE generation.
- `-n, --no-track`: Disable registry tracking.
- `-v, --verbose`: Print command execution details.

## Config
The `.pmanrc` file in the home directory stores author metadata:
- `author`: Name for LICENSE and README.
- `email`: Contact info.
- `license`: Default license type (e.g., MIT).
