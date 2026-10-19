# Contributing to A.T.O.M

A.T.O.M (ATOM's Technically Optimized Machine) is a game engine developed in a single repository. This document covers the workflow and commit conventions contributors should follow.

---

## Repository Structure

This is the **main repo** and the authoritative mirror sent to the university. Every engine module lives here as a regular directory under `modules/`, and all development happens in this repository.

The only exception is the ECS, [Blob-ECS](https://github.com/guilec06/Blob-ECS), which is an independent project included as a Git submodule at `modules/core/include/core/ECS` (see [The Blob-ECS submodule](#the-blob-ecs-submodule)).

Building and running is described in the [README](README.md).

---

## Getting Started

```bash
git clone --recurse-submodules git@github.com:ATOM-s-Technically-Optimized-Machine/A.T.O.M.git
cd A.T.O.M
```

If you already cloned without submodules:

```bash
git submodule update --init
```

---

## Workflow

> **Note:** The workflow is not finalized yet. This section will be updated as new workflows are decided upon.

Work in a branch, not directly on `main`:

```bash
git checkout -b feat/<description>
# make changes, in any number of modules
git commit -m "feat(<scope>): <description>"
git push origin feat/<description>
# open a PR — only integrators merge into main
```

Rules to always keep in mind:
- **Never push directly to `main`.** Always work in a branch and open a PR. Only designated integrators merge into `main`.
- **All changes go through PRs.** No exceptions, regardless of how small the change is.

---

## The Blob-ECS submodule

The repository pins a **commit SHA** of Blob-ECS, not a branch. A `+` prefix in `git submodule status` means the checked-out commit differs from the pinned one.

### Sync after someone else updated the pin

```bash
git pull
git submodule update
```

`git submodule update` (without `--remote`) checks out the **pinned commit**, which is what you want when syncing.

### Change the ECS

Changes to the ECS are made in the Blob-ECS repository, then the pin is bumped here:

```bash
cd modules/core/include/core/ECS
git checkout -b <branch>
# make your changes, commit, push, merge in Blob-ECS
git checkout main && git pull

cd -   # back to the repo root
git checkout -b build/bump-blob-ecs
git add modules/core/include/core/ECS
git commit -m "build(deps): bump Blob-ECS to <short-sha>"
# open a PR
```

To pick up the latest Blob-ECS `main` without changing it: `git submodule update --remote modules/core/include/core/ECS`, then commit the new pin the same way.

The submodule URL is HTTPS so anyone (and CI) can clone it. To push over SSH from inside the submodule: `git remote set-url --push origin git@github.com:guilec06/Blob-ECS.git`.

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

- The engine carries a single version for all modules (e.g. `v0.1.0`), recorded in `CHANGELOG.md`.
- Blob-ECS is versioned in its own repository; `CHANGELOG.md` records which Blob-ECS commit each engine release pins.

---

## University Mirror

The main repo is automatically mirrored to the university Git server on every push to `main`. **Do not push directly to the university remote.**

---

## License

By contributing, you agree that your contributions will be licensed under the [GNU Lesser General Public License v2.1](LICENSE).
