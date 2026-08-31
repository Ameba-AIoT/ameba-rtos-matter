#!/bin/sh
# Configures git to use project hooks from tools/scripts/hooks/.
# Run once after cloning: sh tools/scripts/hooks/install.sh

REPO_ROOT=$(git rev-parse --show-toplevel 2>/dev/null) || { echo "Not a git repository."; exit 1; }

git -C "$REPO_ROOT" config core.hooksPath tools/scripts/hooks
echo "Git hooks enabled (core.hooksPath = tools/scripts/hooks)"
echo "Active hooks: pre-push (commit message validation)"
