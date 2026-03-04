# Contributing to pman

## Technical Standards
- **Language:** C99.
- **Dependencies:** None (Standard Library only).
- **Security:** All input must pass `is_safe_name` and `is_safe_path`. No `system()` calls with unsanitized strings.
- **Style:** K&R or similar concise style. No comments unless logic is non-trivial.

## Development Roadmap
### Phase 1: Performance & Scale
- **Concurrent Status:** Implement `status` command using `pthreads` for parallel Git scanning.
- **Registry Backend:** Transition from flat-file to a structured JSON or SQLite backend for better query performance.

### Phase 2: Workflow Integration
- **Command Alias:** `pman open <name>` - Automatically navigate to and open the project in the user's `$EDITOR`.
- **Hooks:** Pre/Post initialization shell hooks for custom environment setup.

### Phase 3: Advanced UI
- **Full TUI:** Implement an `ncurses` based dashboard for project management.

## Good First Issues
1. **Add Language Preset:** Implement a new `init_<lang>` function in `src/languages.c` (e.g., Swift, Kotlin, Perl).
2. **Version Flag:** Implement `-V` and `--version` flags in `src/main.c`.
3. **Template Improvement:** Expand `.gitignore` or `README.md` static strings in `include/templates.h`.
4. **Registry Export:** Create a command to export the registry to Markdown or CSV.
5. **Color Toggle:** Add an option to disable ANSI colors for plain-text environments.

## Submission Process
1. Fork the repository.
2. Create a feature branch.
3. Ensure `make test` passes 100%.
4. Submit a Pull Request with a concise description of changes.
