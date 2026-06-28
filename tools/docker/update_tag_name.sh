#!/usr/bin/env bash
set -euo pipefail

if [ $# -ne 1 ]; then
    echo "Usage: $0 <TAG_NAME>"
    exit 1
fi

NEW_TAG="$1"

find . -name Dockerfile | while read -r dockerfile; do
    echo "Updating $dockerfile"

    sed -i \
        -E "s|^ARG TAG_NAME=.*$|ARG TAG_NAME=${NEW_TAG}|" \
        "$dockerfile"
done

echo "Done. Updated TAG_NAME to '${NEW_TAG}'."
