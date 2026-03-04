# pman

## Abstract
pman is a high-performance, zero-dependency project lifecycle manager authored in C. It serves as both a deterministic bootstrapping utility and a centralized project registry.

## Features
- 12 standardized language presets.
- Hierarchical template resolution (built-in and custom).
- Centralized registry for project tracking.
- Secure design with strict input validation.

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
`~/.dinitrc` stores user metadata for automated file generation.
