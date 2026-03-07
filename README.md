# pman

**Project lifecycle manager for C99.** Bootstraps projects and tracks repository health across the filesystem.

## 🚀 Overview

Fragmented project setups increase cognitive load and lead to inconsistent environments. `pman` provides a single interface to automate bootstrapping and maintain a centralized index of scattered projects.

## ✨ Core Features

### Standardization

Ensures every project starts with a consistent structure. Generates boilerplate code, `.gitignore` files, and entry points based on 12 built-in language presets or user-defined templates.

### Automation

Replaces repetitive setup tasks with one command.

- Initializes Git repositories with standard branch naming.
- Creates virtual environments and dependency files.
- Generates LICENSE and README files using author metadata.
- Executes language-specific initialization (e.g., `go mod init`, `npm init`).[^1_1]


### Centralization

Maintains a system-wide registry to audit projects across different directories.

- `pman status`: Batch audits Git health for all registered projects.
- `pman list`: Identifies active and missing project paths.
- `pman prune`: Removes dead paths from the index.


### Extensibility

Define custom bootstrapping logic by placing shell scripts in `~/.config/pman/templates/`. Scripts receive project metadata via environment variables.[^1_1]

**Example: Running `pman init c99 myproject` in terminal, showing automated Git init, file creation, and success message.**

## 🛠️ Custom Templates

1. `mkdir -p ~/.config/pman/templates/<template_name>`
2. `touch ~/.config/pman/templates/<template_name>/init.sh`
3. `chmod +x ~/.config/pman/templates/<template_name>/init.sh`

**Environment variables:**

- `PMAN_PROJECT_NAME`: Target project name.
- `PMAN_AUTHOR`: Author name from config.
- `PMAN_EMAIL`: Author email from config.
- `PMAN_LICENSE`: License type from config.

**Example `init.sh`:**

```bash
#!/bin/bash
mkdir src docs
touch src/main.c
echo "Created by $PMAN_AUTHOR" > README.md
```

Invoke: `pman init <template_name> [name]`

**Example: Custom template execution creating directories and files.**

## 📦 Installation

```bash
make
make setup
sudo make install
```

Clone the repository first.

## 🚀 Usage

```
- Initialize in new directory: `pman init <lang> <name>`
```

- Initialize current directory: `pman init <lang>`
- Audit all projects: `pman status`
- List projects: `pman list`
- Clean registry: `pman prune`


### Flags

- `-d, --dir <path>`: Specify target directory.
- `-g, --no-git`: Disable Git initialization.
- `-r, --no-readme`: Disable README generation.
- `-l, --no-license`: Disable LICENSE generation.
- `-n, --no-track`: Disable registry tracking.
- `-v, --verbose`: Print command execution details.

**Example: `pman status` output showing project health across multiple repos.**

## ⚙️ Configuration

Edit `~/.pmanrc`:

- `author`: Name for LICENSE and README.
- `email`: Contact info.
- `license`: Default (e.g., MIT).

<div align="center">⁂</div>
