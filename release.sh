#!/bin/bash

# ToolShed Release Helper Script

# Get the current version from build/unix/rules.mak
VERSION=$(grep "^VERSION =" build/unix/rules.mak | cut -d' ' -f3)

echo "Current version in rules.mak: $VERSION"
read -p "Enter version tag to release (e.g., v$VERSION): " TAG

if [ -z "$TAG" ]; then
    echo "Tag cannot be empty. Aborting."
    exit 1
fi

echo "Creating tag $TAG..."
git tag -a "$TAG" -m "Release $TAG"

echo "Pushing tag $TAG to origin..."
echo "This will trigger the GitHub Action 'Make Release'."
git push origin "$TAG"

echo "Done! You can monitor the progress at: https://github.com/$(git remote get-url origin | sed 's/.*github.com[:\/]\(.*\)\.git/\1/')/actions"
