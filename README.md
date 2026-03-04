# pman

## Overview
Project lifecycle manager in C. Automates bootstrapping and tracks project state.

## Features
- 12 language presets.
- Subcommands: `init`, `list`, `status`, `prune`.
- Custom templates in `~/.config/pman/templates/`.
- Input validation and path traversal protection.
- Optional project tracking.

## Installation
```bash
make
make setup
sudo make install
```

## Usage
`pman init <lang> [name]`
`pman list`
`pman status`
`pman prune`

### Options
- `-n, --no-track`: Skip registration in the project registry.

## Configuration
`~/.dinitrc` stores author metadata.
