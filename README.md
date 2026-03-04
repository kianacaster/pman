# pman

## Overview
Project lifecycle manager in C. Automates bootstrapping and tracks project state.

## Features
- 12 language presets.
- Subcommands: `init`, `list`, `status`, `prune`.
- Custom templates in `~/.config/pman/templates/`.
- Input validation and path traversal protection.

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

## Configuration
`~/.dinitrc` stores author metadata.
