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

```bash
cd libs/<subsystem>/
# make changes
git commit -m "feat(<scope>): <description>"
git push origin main
```

### Updating a submodule reference in the main repo

After pushing changes to a sub-repo, bump the pin in the main repo:

```bash
cd libs/<subsystem>
git pull origin main
cd ../..
git add libs/<subsystem>
git commit -m "build(deps): update <subsystem> to vX.Y.Z

Brief summary of what changed.
Refs: <subsystem-repo> vX.Y.Z"
git push origin main
```

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

### Allowed Scopes

```
core, renderer, audio, physics, scripting, editor,
ecs, input, math, assets, platform, build, ci, docs, deps
```

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

Append `!` to the type/scope, and add a `BREAKING CHANGE` footer:

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
