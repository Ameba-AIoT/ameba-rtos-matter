# Contributing

## Commit Message Rules

Commit messages follow this format, with an optional IC (chip) tag:

```
type(component): [IC] subject

Body explaining what this commit does and why. (mandatory)
```

Example:

```
fix(dct): [amebaz2plus] guard DCT access before initialization

The DCT was read from the AT-command thread before matter_init()
finished, occasionally returning stale values. Serialize the read
onto the Matter event loop so it always runs after initialization.
```

### Rules

- **`type`** — required. One of:
  `feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `build`,
  `ci`, `chore`, `revert`.
- **`(component)`** — required. The area of the codebase; see the
  [Components](#components) table below.
- **`[IC]`** — optional. Include only for **chip-specific** changes; **omit
  it for common (chip-agnostic) changes**. One of:
  `amebaz2`, `amebaz2plus`, `amebad`, `amebalite`, `amebadplus`, `amebasmart`,
  `amebagreen2`.
- **`!`** — optional. Append before the colon to mark a breaking change,
  e.g. `feat(api)!: rename matter_clear_all_fabric`.
- **subject** — required, at least 3 characters. Imperative mood, no
  trailing period.
- **body** — mandatory. Explain *what* the change does and *why*.

### What each type means

| Type       | Use for                                                        |
| ---------- | -------------------------------------------------------------- |
| `feat`     | A new feature                                                  |
| `fix`      | A bug fix                                                      |
| `docs`     | Documentation only                                             |
| `style`    | Formatting/whitespace, no behavior change                      |
| `refactor` | Code change that neither fixes a bug nor adds a feature        |
| `perf`     | A change that improves performance                             |
| `test`     | Adding or fixing tests                                         |
| `build`    | Build system, Makefiles, or dependency changes                 |
| `ci`       | CI configuration and scripts (workflows, hooks)                |
| `chore`    | Maintenance that doesn't fit the above                         |
| `revert`   | Reverting a previous commit                                    |

### Components

| Component    | Use for                                              |
| ------------ | ---------------------------------------------------- |
| `api`        | Public Matter API layer                              |
| `atcmd`      | AT command interface                                 |
| `ble`        | Bluetooth Low Energy / BLE commissioning             |
| `core`       | Core Matter integration and event loop               |
| `data-model` | Matter data model and clusters                       |
| `dct`        | Device Configuration Table storage                   |
| `docs`       | Documentation                                        |
| `drivers`    | Hardware and peripheral drivers                      |
| `examples`   | Example applications                                 |
| `fs`         | Filesystem                                           |
| `kvs`        | Key-value store                                      |
| `lwip`       | lwIP TCP/IP stack integration                        |
| `mbedtls`    | mbedTLS crypto integration                           |
| `ota`        | Over-the-air update                                  |
| `scripts`    | Build, CI, and tooling scripts (workflows, hooks)    |
| `sdk`        | Vendor SDK integration                               |
| `soc`        | SoC-specific code                                    |
| `timer`      | Timer and system layer                               |
| `tools`      | Developer tools                                      |
| `util`       | Utilities and helpers                                |
| `wifi`       | Wi-Fi stack integration                              |

### Examples

```
feat(dct): serialize post-init DCT operations onto Matter event loop
fix(api): [amebaz2plus] guard null pointer in fabric lookup
docs(docs): update RTL8721F Matter support status to WIP
ci(scripts): make the [IC] tag optional for common changes
feat(api)!: rename matter_clear_all_fabric
```

## Enforcement

The commit format is validated in two places, using the same rule:

- **Locally** — the `pre-push` hook rejects non-conforming commits before
  they leave your machine. Enable it once after cloning:

  ```sh
  sh tools/scripts/hooks/install.sh
  ```

  The same hook also runs the restyle check (`tools/scripts/restyle_check.sh`).

- **In CI** — the `Check PR Commits` workflow
  (`.github/workflows/check_pr_commits.yml`) re-validates every commit in a
  pull request.

If a commit is flagged, reword it with `git rebase -i <base-sha>`.

## Pull Requests

PR descriptions must include both a `This PR addresses:` section and a
`Verification:` section (see the pull request template). This is enforced by
the `Check PR Template Format` workflow.
