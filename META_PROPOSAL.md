# Proposal: DInit Meta-Project Tracker (DHub)

## 1. Objective
To extend `DInit` from a standalone initialization utility into a comprehensive project lifecycle manager that tracks, versions, and monitors all projects created via the tool from a centralized meta-interface.

## 2. Core Components
### A. The Project Registry (`registry.json`)
A persistent, machine-readable store located at `~/.config/dinit/registry.json`. 
- **Schema:**
  ```json
  [
    {
      "name": "my-app",
      "path": "/home/user/Projects/my-app",
      "lang": "rust",
      "timestamp": "2026-03-04T20:15:00Z",
      "git_remote": "git@github.com:user/my-app.git"
    }
  ]
  ```

### B. The Integration Hook
A lightweight addition to `DInit`'s execution flow that triggers a registration event upon successful project creation. This keeps the registry synchronized without user intervention.

### C. The Meta-VCS (Automatic Snapshotting)
If the parent directory (e.g., `~/Projects`) is detected as a Git repository, `DInit` can automatically perform an "Atomic Parent Commit" after a sub-project is created. This allows for a versioned history of the *entire workstation state*.

## 3. Proposed Commands (`dtrack`)
- `dtrack list`: Tabular view of all projects, their languages, and sizes.
- `dtrack status`: Rapid scan of all registered paths to identify uncommitted changes (utilizing `git status -s`).
- `dtrack prune`: Automated cleanup of registry entries for directories that have been deleted from the filesystem.
- `dtrack update-all`: Concurrent execution of dependency updates (e.g., `npm update`, `cargo update`) across the entire registry.

## 4. Architectural Considerations
- **Non-Intrusive Design:** The tracking must be "opt-in" or completely silent, ensuring `DInit` remains a viable standalone tool for users who do not want a registry.
- **Concurrency:** Status checks should be multi-threaded to handle registries with hundreds of projects.
- **Portability:** Use simple file-locking to prevent registry corruption during simultaneous `dinit` executions.

---
*Drafted: March 4, 2026*
