# Contributing to A.T.O.M

A.T.O.M (ATOM's Technically Optimized Machine) is a multi-repo game engine. This document covers the workflow and commit conventions contributors should follow.

---

## Repository Structure

This is the **main repo**. It assembles all subsystems via Git submodules and is the authoritative mirror sent to the university.

Day-to-day development happens **in the sub-repos**. The main repo is updated when sub-repo changes need to be integrated.

---

## Getting Started

```bash
git clone --recurse-submodules git@github.com:ATOM-s-Technically-Optimized-Machine/A.T.O.M.git
cd A.T.O.M
```

If you already cloned without submodules:

```bash
git submodule update --init --recursive
```

---

## Workflow

> **Note:** The workflow is not finalized yet. This section will be updated as new workflows are decided upon.

### Working on a subsystem

Work in a branch, not directly on `main`:

```bash
cd modules/<subsystem>/
git checkout -b feat/<description>
# make changes
git commit -m "feat(<scope>): <description>"
git push origin feat/<description>
# open a PR to merge into the subsystem's main when ready
```

### Updating a submodule reference in the main repo

The main repo pin **does not need to be updated after every submodule commit**. Update it only when a set of changes is ready to be integrated — typically after a subsystem PR is merged.

```bash
cd modules/<subsystem>
git pull origin main
cd ../..
git add modules/<subsystem>
git commit -m "build(deps): update <subsystem> to vX.Y.Z

Brief summary of what changed.
Refs: <subsystem-repo> vX.Y.Z"
# push to a branch and open a PR, or push directly to main if you have access
git push origin main
```

---

## Submodule Memo

All subsystems live under `modules/` as Git submodules. The main repo tracks a **pinned commit SHA** for each submodule — not a branch.

Two rules to always keep in mind:
- **Never push directly to `main`** in a submodule unless you are the integrator. Work in a branch and open a PR.
- **The main repo pin does not need to reflect every submodule commit.** Only bump it when a feature or fix is ready to be integrated.

### Check submodule status

```bash
git submodule status
```

Each line shows the pinned commit SHA, the path, and the tag if any. A `+` prefix means the checked-out commit differs from what the main repo pins.

---

### Clone with submodules

```bash
git clone --recurse-submodules git@github.com:ATOM-s-Technically-Optimized-Machine/A.T.O.M.git
```

Already cloned without submodules:

```bash
git submodule update --init --recursive
```

---

### Work on a submodule

```bash
cd modules/<subsystem>
git checkout -b feat/<description>
# make your changes
git add <files>
git commit -m "feat(<scope>): <description>"
git push origin feat/<description>
# open a PR — do not push directly to main
```

Once the PR is merged and you are ready to integrate into the main repo:

```bash
cd modules/<subsystem>
git checkout main && git pull origin main

# back in the main repo, update the pin
cd ../..
git add modules/<subsystem>
git commit -m "build(deps): bump <subsystem>"
git push origin main
```

The pin update to the main repo is a separate, deliberate step — not something that happens automatically after every submodule commit.

---

### Pull latest changes from a submodule remote

```bash
# update one submodule to its remote's latest commit
git submodule update --remote modules/<subsystem>

# update all submodules at once
git submodule update --remote
```

Only bump the pin in the main repo when the update is intentional and ready to integrate:

```bash
git add modules/<subsystem>
git commit -m "build(deps): update <subsystem> to latest"
# push to a branch and open a PR, or directly to main if appropriate
git push origin main
```

---

### Revert the main repo to a previous submodule pin

Use this when you want the main repo to point to an older commit of a submodule without touching the submodule's own history.

```bash
# find the main repo commit where the submodule was at the right state
git log --oneline

# check out that specific main repo commit's version of the submodule pin
git checkout <main-repo-commit-sha> -- modules/<subsystem>

# verify the submodule now points to the right commit
git submodule status

# commit the reverted pin
git add modules/<subsystem>
git commit -m "revert(deps): pin <subsystem> back to <commit-sha>"
git push origin main
```

---

### Revert a submodule to a previous commit

Use this when you need to undo changes inside a submodule itself.

```bash
cd modules/<subsystem>

# find the commit you want to go back to
git log --oneline

# check out that commit
git checkout <commit-sha>

# push as a new commit on main (never force-push shared branches)
git revert <bad-commit-sha>   # or create a fix commit
git push origin main

# back in the main repo, update the pin to the reverted state
cd ../..
git add modules/<subsystem>
git commit -m "revert(deps): revert <subsystem> to <commit-sha>"
git push origin main
```

---

### Sync after someone else updated a submodule pin

```bash
git pull origin main
git submodule update --recursive
```

`git submodule update` (without `--remote`) checks out the **pinned commit** recorded in the main repo, not the latest remote — which is the correct behavior when syncing.

---

## Commit Convention

This project uses **[Conventional Commits v1.0.0](https://www.conventionalcommits.org/en/v1.0.0/)**.

### Format

```
<type>(<scope>): <description>

[optional body]

[optional footer(s)]
```

### Types

| Type       | When to use                                      |
|------------|--------------------------------------------------|
| `feat`     | A new feature                                    |
| `fix`      | A bug fix                                        |
| `build`    | Build system or dependency changes               |
| `refactor` | Code change that neither fixes a bug nor adds a feature |
| `perf`     | Performance improvement                          |
| `test`     | Adding or fixing tests                           |
| `docs`     | Documentation only                               |
| `ci`       | CI/CD workflow changes                           |
| `chore`    | Maintenance tasks (no production code change)    |

### Scopes

A scope should identify **what part of the project** was changed, at the level a reviewer would care about.

**Use a scope when:**
- The change is contained to a specific subsystem or module (e.g. `renderer`, `core`, `ecs`)
- The change targets infrastructure or tooling concerns (e.g. `ci`, `build`, `deps`, `docs`)

**Do not use a scope when:**
- The change cuts across the whole codebase with no clear owner
- You would have to list more than one scope — split the commit instead

**Keep scopes short and lowercase.** Prefer the subsystem or directory name as it exists in the repo.

### Examples

```bash
feat(ecs): implement component pool allocator
fix(renderer): correct UV coordinate flipping on OpenGL
perf(renderer): batch draw calls to reduce API overhead
refactor(ecs): split system registry into separate file
docs(readme): add build instructions for Linux
test(physics): add raycasting unit tests
ci(mirror): fix university push workflow
chore(deps): bump GLFW to 3.4
```

### Breaking Changes

Append `!` to the type/scope, and/or add a `BREAKING CHANGE` footer:

```
feat(scripting)!: redesign script lifecycle API

BREAKING CHANGE: Script::init() now takes a Context reference instead of raw pointer.
```

---

## Versioning

- Each sub-repo is versioned independently (e.g. `core-v0.2.1`, `renderer-v0.1.3`).
- The main repo carries the engine-level version (e.g. `v0.1.0`).
- `CHANGELOG.md` in the main repo records which sub-repo versions each engine release includes.

---

## University Mirror

The main repo is automatically mirrored to the university Git server on every push to `main`. **Do not push directly to the university remote.**

---

## License

By contributing, you agree that your contributions will be licensed under the [GNU Lesser General Public License v2.1](LICENSE).
