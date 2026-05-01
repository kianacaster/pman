# pman

Easily initialise your project directories

## Installation

Prerequisites: `gcc`, `make`, `curl`.

```bash
make release
sudo make install
make setup
```

## Usage

`pman [GLOBAL_OPTIONS] <COMMAND> [COMMAND_OPTIONS] [ARGUMENTS]`

### Global Options

* `-h, --help`: Display usage information.
* `-V, --version`: Display version information.
* `-v, --verbose`: Enable verbose output.

### Commands

#### init
Initialise a project using a template.
`pman init [options] <template> [name]`

* `-d, --dir <path>`: Specify target directory.
* `-g, --no-git`: Disable Git initialisation.
* `-r, --no-readme`: Disable README generation.
* `-l, --no-license`: Disable LICENSE generation.
* `-n, --no-track`: Disable registry tracking.

#### list
Display all registered projects and their status.
`pman list`

#### status
Execute `git status -s` across all registered projects.
`pman status`

#### prune
Remove projects with non-existent paths from the registry.
`pman prune`

#### templates
Manage initialisation templates.
`pman templates [option] [argument]`

* `-l, --list`: List installed templates.
* `-i, --install <name>`: Download template from remote repository.
* `-S, --search <query>`: Search remote repository for templates.
* `-s, --show <name>`: Display local template script content.

#### export 
`pman export [flag] <project_name> [dest_path]`

* `-z, --zipped`: Export project as a .zip archive.
* `-i, --info`: Display project README.

#### uninstall
Remove configuration, registry, and templates.
`pman uninstall`

## Configuration

Configuration is stored in `~/.pmanrc` using `key=value` format.

* `author`: Project author name.
* `email`: Project author email.
* `license`: Default license type.
* `verbose`: Boolean (true/false).

## Template Specification

Templates are shell scripts located at `~/.config/pman/templates/<name>/init.sh`. The following environment variables are exported to the script:

* `PMAN_PROJECT_NAME`: Target project name.
* `PMAN_AUTHOR`: Configured author name.
* `PMAN_EMAIL`: Configured author email.
* `PMAN_LICENSE`: Configured license type.
* `PMAN_USE_README`: Boolean ("true"/"false").
