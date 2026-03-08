# pman

**Project Manager** Bootstraps projects and tracks repository health across the filesystem.

## 🚀 Overview

pman` provides a single interface to automate project directory initialisation and maintain a centralized index of scattered projects.

## ✨ Core Features

### Standardization

Ensures every project starts with a consistent structure. Generates boilerplate code, `.gitignore` files, and entry points based on 12 built-in language presets or user-defined templates.

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

Define custom bootstrapping logic by placing shell scripts in `~/.config/pman/templates/`. Scripts receive project metadata via environment variables.

## 🛠️ Custom Templates

1. `mkdir -p ~/.config/pman/templates/<template_name>`
2. `touch ~/.config/pman/templates/<template_name>/init.sh`
3. `chmod +x ~/.config/pman/templates/<template_name>/init.sh`

**Environment variables:**

- `PMAN_PROJECT_NAME`: Target project name.
- `PMAN_AUTHOR`: Author name from config.
- `PMAN_EMAIL`: Author email from config.
- `PMAN_LICENSE`: License type from config.

**Example `mytemplate.sh`:**

```bash
#!/bin/bash
mkdir src docs
touch src/main.c
echo "Created by $PMAN_AUTHOR" > README.md
```

Invoke: `pman init mytemplate`

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

## ⚙️ Configuration

Edit `~/.pmanrc`:

- `author`: Name for LICENSE and README.
- `email`: Contact info.
- `license`: Default (e.g., MIT).

<div align="center">⁂</div>
