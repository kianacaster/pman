# pman

A CLI tool to easily initialise and manage your project directories. 

## Overview

Skip the boring boilerplate every time you start a new project. Generate `.gitignore` files, and entry points based on 12 built-in languages or create your own.


Replaces repetitive setup tasks with one command.

- Initializes Git repositories with standard branch naming.
- Creates virtual environments and dependency files.
- Generates LICENSE and README files using author metadata.
- Executes language-specific initialization (e.g., `go mod init`, `npm init`).


### A centralised directory 

Maintains a system-wide registry to audit projects across different directories.

- `pman status`: Batch audits Git health for all registered projects.
- `pman list`: Identifies active and missing project paths.
- `pman prune`: Removes dead paths from the index.


### Customisation

Create your own templates by placing shell scripts in `~/.config/pman/templates/`. Scripts receive project metadata via environment variables.

1. `mkdir -p ~/.config/pman/templates/<template_name>`
2. `touch ~/.config/pman/templates/<template_name>/init.sh`
3. `chmod +x ~/.config/pman/templates/<template_name>/init.sh`

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

## Installation
Prereqs: Make and gcc

```bash
git clone https://www.github.com/kianacaster/pman.git
cd pman
make release
sudo make install PREFIX=/usr/local
```

## Usage

- Initialize in new directory: `pman init <lang> <name>`
- Initialize current directory: `pman init <lang>`
- Audit all projects: `pman status`
- List projects: `pman list`
- Clean registry: `pman prune`
- Export project: `pman export [flag] <name> <dest>`

### Flags

- `-d, --dir <path>`: Specify target directory.
- `-g, --no-git`: Disable Git initialization.
- `-r, --no-readme`: Disable README generation.
- `-l, --no-license`: Disable LICENSE generation.
- `-n, --no-track`: Disable registry tracking.
- `-v, --verbose`: Print command execution details.
- `-z, --zipped`: Export project as a .zip archive.
- `-i, --info`: Print the project's README to stdout.

## Configuration

Edit `~/.pmanrc`:

- `author`: Name for LICENSE and README.
- `email`: Contact info.
- `license`: Default (e.g., MIT).
