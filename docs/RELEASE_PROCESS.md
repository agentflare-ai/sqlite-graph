# Release Process

This document describes how to create releases for the SQLite Graph Database Extension.

## Prerequisites

- Write access to the repository
- All changes merged to `main` branch
- VERSION file updated
- CHANGELOG.md updated with release notes

## Creating a Release

### Option 1: GitHub Actions UI (Recommended)

Create a release directly from GitHub's web interface:

1. Go to the [Actions tab](https://github.com/agentflare-ai/sqlite-graph/actions)
2. Select **"Create Release"** workflow from the left sidebar
3. Click **"Run workflow"** button
4. Configure the release:
   - **Version**: Leave empty to use VERSION file, or specify (e.g., `0.1.0-alpha.1`)
   - **Skip branch validation**: Check to allow releases from non-main branches
5. Click **"Run workflow"**

The workflow will:
1. Validate the version format
2. Check that the tag doesn't already exist
3. Verify you're on the main branch (unless skipped)
4. Create and push the git tag
5. Trigger the release build workflow automatically

### Option 2: Command Line Script

Use the provided script to create a release:

```bash
# Create release using version from VERSION file
./scripts/create-release.sh

# Or specify a version explicitly
./scripts/create-release.sh 0.1.0-alpha.1
```

The script will:
1. Validate the version format
2. Check for uncommitted changes
3. Create a git tag with the version
4. Push the tag to trigger the release workflow

### Option 3: Manual Tag Creation

If you prefer to create a release manually:

```bash
# Create and push a tag
VERSION=$(cat VERSION)
git tag -a "v$VERSION" -m "Release v$VERSION"
git push origin "v$VERSION"
```

## Release Workflow

When a tag is pushed (format: `v*`), GitHub Actions will automatically:

1. **Build the extension** for multiple platforms:
   - Ubuntu 20.04 (linux-x86_64)
   - Ubuntu 22.04 (linux-x86_64-u22)
   - Ubuntu 24.04 (linux-x86_64-u24)

2. **Run tests** to verify functionality

3. **Create release artifacts**:
   - `libgraph.so` - Shared library
   - `libgraph_static.a` - Static library (if available)
   - Checksums for verification

4. **Publish GitHub Release** with:
   - Release notes from CHANGELOG.md
   - Platform-specific tarballs
   - Combined SHA256SUMS.txt

## Version Format

Versions follow [Semantic Versioning](https://semver.org/):

- **Major.Minor.Patch** (e.g., `1.0.0`)
- **Major.Minor.Patch-Suffix** (e.g., `0.1.0-alpha.0`)

Suffixes indicate pre-release versions:
- `alpha` - Early testing, API may change significantly
- `beta` - Feature complete, testing for stability
- `rc` - Release candidate, final testing before stable release

## Pre-release vs Stable Release

The release workflow automatically detects pre-release versions:
- Versions with `alpha`, `beta`, or `rc` are marked as pre-releases
- Other versions are marked as stable releases

## Troubleshooting

### Build Failures

If the release build fails:

1. Check the [Actions tab](https://github.com/agentflare-ai/sqlite-graph/actions)
2. Review build logs for errors
3. Fix issues and create a new tag (increment patch version)

### Missing Dependencies

The release workflow requires:
- SQLite amalgamation
- Unity testing framework

These are downloaded by `scripts/vendor.sh`. If downloads fail:
- Check internet connectivity in GitHub Actions
- Verify download URLs are still valid
- Consider bundling dependencies in the repository

### Failed Tests

If tests fail during release:
- Review test logs in GitHub Actions
- Fix failing tests
- Create a new release with incremented version

## Post-Release

After a successful release:

1. Verify the release appears at:
   https://github.com/agentflare-ai/sqlite-graph/releases

2. Test downloading and using the released binaries

3. Update documentation if needed

4. Announce the release (social media, mailing lists, etc.)

## Emergency Release Deletion

If a release is published with critical issues:

```bash
# Delete the tag locally
git tag -d v0.1.0-alpha.0

# Delete the tag remotely
git push origin :refs/tags/v0.1.0-alpha.0

# Delete the GitHub release via web UI
# https://github.com/agentflare-ai/sqlite-graph/releases
```

Then fix issues and create a new release with incremented version.
