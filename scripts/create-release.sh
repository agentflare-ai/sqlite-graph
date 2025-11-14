#!/bin/bash

# Script to create a new release
# Usage: ./scripts/create-release.sh [version]

set -e

# Get version from argument or VERSION file
if [ -n "$1" ]; then
    VERSION="$1"
else
    VERSION=$(cat VERSION 2>/dev/null || echo '')
    if [ -z "$VERSION" ]; then
        echo "Error: VERSION file not found or empty"
        exit 1
    fi
fi

# Validate version format
if [[ ! "$VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+(-[a-zA-Z0-9\.]+)?$ ]]; then
    echo "Error: Invalid version format: $VERSION"
    echo "Expected format: X.Y.Z or X.Y.Z-suffix (e.g., 0.1.0-alpha.0)"
    exit 1
fi

# Add 'v' prefix if not present
if [[ "$VERSION" != v* ]]; then
    TAG="v$VERSION"
else
    TAG="$VERSION"
fi

echo "Creating release $TAG..."

# Check if tag already exists
if git rev-parse "$TAG" >/dev/null 2>&1; then
    echo "Error: Tag $TAG already exists"
    exit 1
fi

# Check if we're on main branch
CURRENT_BRANCH=$(git branch --show-current)
if [ "$CURRENT_BRANCH" != "main" ]; then
    echo "Warning: Not on main branch (current: $CURRENT_BRANCH)"
    read -p "Continue anyway? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Check for uncommitted changes
if [ -n "$(git status --porcelain)" ]; then
    echo "Error: There are uncommitted changes"
    echo "Please commit or stash your changes before creating a release"
    exit 1
fi

# Create and push tag
echo "Creating tag $TAG..."
git tag -a "$TAG" -m "Release $TAG

See CHANGELOG.md for details."

echo "Pushing tag to origin..."
git push origin "$TAG"

echo ""
echo "✅ Release $TAG created successfully!"
echo ""
echo "GitHub Actions will now:"
echo "  1. Build the extension for multiple platforms"
echo "  2. Run tests"
echo "  3. Create a GitHub release with binaries"
echo ""
echo "Monitor progress at:"
echo "  https://github.com/agentflare-ai/sqlite-graph/actions"
echo ""
echo "Once complete, the release will be available at:"
echo "  https://github.com/agentflare-ai/sqlite-graph/releases/tag/$TAG"
