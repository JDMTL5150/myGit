# Jacob's Version Control (JVC)

A minimal Git-like version control system written in C++, built as a learning project to understand how VCS tools work under the hood. JVC implements local repositories, commits, branches, staging, and basic history—without relying on Git itself.

---

## Features

- **Repository:** Initialize a repo (`init`), inspect or rename it (`repo`, `repo -r`)
- **Staging:** Add files (`add`), clear the stage (`clear`), remove a file from stage (`clear -f <file>`), ignore files (`add -i <file>`)
- **Commits:** Commit staged changes with a message; view history with `log`
- **Branches:** List branches, create a branch, remove a branch (`branch -r <name>`)
- **Checkout:** Restore working tree to a given commit
- **Diff:** Show changes between a commit and its parent
- **Status:** Show currently staged files
- **Delete:** Remove the local `.jvc` repository (with confirmation)

---

## Build

Requirements: C++17-capable compiler (e.g. g++), standard library with `<filesystem>`.

```bash
cd src
make
```

The binary is built as `../out/myGit`. Run it from anywhere (or add `out/` to your PATH / symlink as `jvc`):

```bash
./out/myGit help
# or, if you use it as 'jvc':
jvc help
```

---

## Quick Start

```bash
# Initialize a repository (run from the directory you want to track)
jvc init my_project

# Stage files and commit
jvc add file1.txt file2.txt
jvc commit "Initial commit"

# Check status and history
jvc status
jvc log

# Create and use branches
jvc branch feature-x
jvc branch -r old_branch   # remove a branch
```

---

## Command Reference

| Command | Usage | Description |
|--------|--------|-------------|
| `init` | `jvc init <repo_name>` | Create a new JVC repository in the current directory |
| `add` | `jvc add <file> [file ...]` | Stage file(s) for the next commit |
| `add -i` | `jvc add -i <file>` | Add a file to `.jvcignore` (do not track) |
| `commit` | `jvc commit <message>` | Create a commit from the current staging area |
| `status` | `jvc status` | List files currently staged |
| `log` | `jvc log` | Show commit history |
| `diff` | `jvc diff <commit_id>` | Show changes between that commit and its parent |
| `checkout` | `jvc checkout <commit_id>` | Restore working tree to the given commit |
| `branch` | `jvc branch` | List all branches |
| `branch` | `jvc branch <name>` | Create a new branch |
| `branch -r` | `jvc branch -r <name>` | Remove a branch |
| `repo` | `jvc repo` | Print repository info |
| `repo -r` | `jvc repo -r <new_name>` | Rename the repository |
| `clear` | `jvc clear` | Unstage all files (clear staging area) |
| `clear -f` | `jvc clear -f <file>` | Remove a single file from the staging area |
| `delete` | `jvc delete y` | Delete the local `.jvc` repo (must confirm with `y`) |
| `help` | `jvc help` | Show available commands and flags |

---

## Repository Layout

After `jvc init`, a `.jvc` directory is created in the current folder. It holds config, staged files, commit metadata, snapshots, and branch refs—similar in spirit to Git’s `.git` directory.

---

## Project Status

This is a **work in progress** and for **learning purposes**. It is not intended to replace Git or to be used for critical data. Contributions and ideas are welcome.
