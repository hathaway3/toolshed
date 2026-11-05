#!/bin/bash

# ToolShed Release Helper Script

# Get the current version from build/unix/Makefile (derived from git tag or fallback)
VERSION=$(make -s -C build/unix print-version)

echo "Current detected version: $VERSION"
read -p "Enter version tag to release (e.g., v$VERSION): " TAG

if [ -z "$TAG" ]; then
    echo "Tag cannot be empty. Aborting."
    exit 1
fi

# Strip leading v/V to update FALLBACK_VERSION in build/unix/rules.mak if needed
CLEAN_VER="${TAG#[vV]}"
sed -i.bak "s/^FALLBACK_VERSION = .*/FALLBACK_VERSION = $CLEAN_VER/" build/unix/rules.mak && rm -f build/unix/rules.mak.bak

echo "Creating tag $TAG..."
git tag -a "$TAG" -m "Release $TAG"

echo "Pushing tag $TAG to origin..."
echo "This will trigger the GitHub Action 'Make Release'."
git push origin "$TAG"

echo "Done! You can monitor the progress at: https://github.com/$(git remote get-url origin | sed 's/.*github.com[:\/]\(.*\)\.git/\1/')/actions"
