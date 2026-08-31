#!/bin/sh

CHECK_ONLY=0

if [ "$1" = "--check" ]; then
    CHECK_ONLY=1
fi

# Get git repository root
REPO_ROOT=$(git rev-parse --show-toplevel 2>/dev/null) || exit 1
cd "$REPO_ROOT" || exit 1

# =========================
# Hook installation check
# =========================
_hooks_path=$(git config core.hooksPath 2>/dev/null)
if [ "$_hooks_path" != "tools/scripts/hooks" ] && [ ! -f "$REPO_ROOT/.git/hooks/pre-push" ]; then
    echo "WARNING: Git hooks are not installed — commit message and restyle checks will not run on push."
    echo "         Run once to enable:  sh tools/scripts/hooks/install.sh"
    echo ""
fi


# =========================
# Canonical header template
# __YEAR__ is replaced per-file with the file's creation year
# =========================
HEADER_TEMPLATE=$(mktemp)
HEADER_FILE=$(mktemp)

cat << 'EOF' > "$HEADER_TEMPLATE"
/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) __YEAR__, Realtek Semiconductor Corporation. All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
EOF


# =========================
# Detect the year a file was created (first added to git).
# Falls back to the current year for files not yet committed.
# =========================
get_file_year() {
    year=$(git log --follow --diff-filter=A --format=%ad --date=format:%Y -- "$1" 2>/dev/null | tail -1)
    if [ -z "$year" ]; then
        year=$(date +%Y)
    fi
    echo "$year"
}


# =========================
# Strip ALL leading headers
# =========================
strip_leading_comments() {
    file="$1"
    tmpfile=$(mktemp)

    awk '
    BEGIN { in_comment=0; started=0 }

    {
        if (!started) {

            # start of any block comment
            if ($0 ~ /^[[:space:]]*\/\*/) {
                in_comment=1
                next
            }

            # inside comment block
            if (in_comment) {
                if ($0 ~ /\*\//) {
                    in_comment=0
                }
                next
            }

            # skip empty lines before code
            if ($0 ~ /^[[:space:]]*$/) {
                next
            }

            # first real code line reached
            started=1
        }

        print
    }
    ' "$file" > "$tmpfile"

    mv "$tmpfile" "$file"
}


# =========================
# Get changed files
# Exclude third-party code
# RESTYLE_BASE env var overrides the diff base (used by CI for PR checks)
# =========================
if [ -n "$RESTYLE_BASE" ]; then
    changed_files=$(
        git diff --name-only "$RESTYLE_BASE...HEAD" |
        grep -E '\.(c|cpp|h|hpp)$' |
        grep -Ev '^(common/lwip/lwip_v2\.1\.2|common/mbedtls/)'
    )
else
    changed_files=$(
        git diff --name-only HEAD~1 HEAD |
        grep -E '\.(c|cpp|h|hpp)$' |
        grep -Ev '^(common/lwip/lwip_v2\.1\.2|common/mbedtls/)'
    )
fi


if [ -n "$changed_files" ]; then
    echo
    echo "Processing changed files:"

    for f in $changed_files
    do
        echo "$f"

        if [ "$CHECK_ONLY" -eq 1 ]; then
            # Check: must contain canonical Realtek marker
            if ! grep -q "Realtek Semiconductor Corporation" "$f"; then
                echo "  -> Missing Realtek header: $f"
                rm -f "$HEADER_FILE" "$HEADER_TEMPLATE"
                exit 1
            fi
        else
            echo "  -> Normalizing header"

            # build canonical header with this file's creation year
            year=$(get_file_year "$f")
            sed "s/__YEAR__/$year/" "$HEADER_TEMPLATE" > "$HEADER_FILE"

            # remove ALL existing top headers
            strip_leading_comments "$f"

            # prepend canonical header
            tmpfile=$(mktemp)
            cat "$HEADER_FILE" "$f" > "$tmpfile"
            mv "$tmpfile" "$f"
        fi
    done


    # =========================
    # Run astyle
    # =========================
    if [ "$CHECK_ONLY" -eq 1 ]; then
        astyle --style=linux \
               --attach-namespaces \
               -p -xg -H -U -k3 -j \
               -xC160 -xL -T4 -z2 \
               --indent=spaces=4 \
               --indent-continuation=4 \
               --dry-run $changed_files | grep -q "Formatted" && exit 1
    else
        astyle --style=linux \
               --attach-namespaces \
               -p -xg -H -U -k3 -j \
               -xC160 -xL -T4 -z2 \
               --indent=spaces=4 \
               --indent-continuation=4 \
               -n -q $changed_files
    fi
fi


rm -f "$HEADER_FILE" "$HEADER_TEMPLATE"