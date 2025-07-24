#!/bin/bash

AMEBA=$1
BASE_REPO=$2
DEFAULT_MATTER_VERSION="main"

echo "Setting up for $AMEBA"

if [[ "$AMEBA" == "amebaz2" || "$AMEBA" == "amebaz2plus" || "$AMEBA" == "amebad" ]]; then
    git fetch --all --prune
    available_version=($(git branch -r | grep -o 'origin/release/v[0-9.]*' | sed 's|origin/release/||' | grep -v '^v1.3$' | sort -u))
fi

available_version+=("main")

echo "Available Matter Versions:"
for i in "${!available_version[@]}"; do
    echo "$((i+1))) ${available_version[$i]}"
done

read -p "Enter Matter Version (number or name, default: $DEFAULT_MATTER_VERSION): " USER_INPUT
USER_INPUT=${USER_INPUT:-$DEFAULT_MATTER_VERSION}

if [[ "$USER_INPUT" =~ ^[0-9]+$ ]]; then
    INDEX=$((USER_INPUT - 1))
    if [[ $INDEX -ge 0 && $INDEX -lt ${#available_version[@]} ]]; then
        MATTER_VERSION="${available_version[$INDEX]}"
    else
        echo "Invalid selection number."
        exit 1
    fi
else
    MATTER_VERSION="$USER_INPUT"
fi

valid=false
for version in "${available_version[@]}"; do
    if [ "$MATTER_VERSION" == "$version" ]; then
        valid=true
        break
    fi
done

if [ "$valid" != "true" ]; then
    echo "Invalid input of Matter Version"
    exit 1
fi

if [ "$MATTER_VERSION" != "main" ]; then
    MATTER_BRANCH="release/$MATTER_VERSION"
else
    MATTER_BRANCH="main"
fi

echo "Selected Matter version: $MATTER_VERSION ($MATTER_BRANCH)"

current_branch=$(git rev-parse --abbrev-ref HEAD)

if [ "$current_branch" != "$MATTER_BRANCH" ]; then
    echo "Switching from '$current_branch' to '$MATTER_BRANCH'"
    git fetch origin || exit 1
    git checkout "$MATTER_BRANCH" || exit 1
    git pull origin "$MATTER_BRANCH" || exit 1
else
    echo "Already on branch '$MATTER_BRANCH'"
fi
