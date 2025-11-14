# GitHub Actions UI Workflow - Quick Reference

## Creating a Release via GitHub UI

### Step 1: Navigate to Actions Tab
Go to: https://github.com/agentflare-ai/sqlite-graph/actions

### Step 2: Select "Create Release" Workflow
Look for "Create Release" in the left sidebar workflow list

### Step 3: Click "Run workflow"
You'll see a dropdown button in the top-right that says "Run workflow"

### Step 4: Configure Inputs

The UI will show two input fields:

1. **Release version** (optional)
   - Description: "Release version (e.g., 0.1.0-alpha.0 or leave empty to use VERSION file)"
   - Leave empty to use the VERSION file from the repository
   - Or enter a specific version like: `0.1.0-alpha.1`

2. **Skip branch validation** (checkbox)
   - Description: "Skip branch validation (allow releases from non-main branches)"
   - Default: Unchecked
   - Check this only if you need to create a release from a non-main branch

### Step 5: Click "Run workflow"
The workflow will start immediately and show in the workflows list

### Step 6: Monitor Progress
- Click on the running workflow to see real-time logs
- The workflow will show a summary with:
  - Version and tag created
  - Link to monitor the release build
  - Link to the upcoming release page

## Workflow Behavior

### What the Workflow Does
1. ✅ Checks out the repository code
2. ✅ Determines version (from input or VERSION file)
3. ✅ Validates version format (X.Y.Z or X.Y.Z-suffix)
4. ✅ Checks if tag already exists (fails if it does)
5. ✅ Validates branch is main (unless skip_validation is checked)
6. ✅ Creates an annotated git tag
7. ✅ Pushes the tag to GitHub
8. ✅ Shows success summary with links

### What Happens Next
Once the tag is pushed:
- The "Release" workflow automatically triggers
- Builds are created for Ubuntu 22.04 and 24.04
- Tests are run (optional, won't block release)
- GitHub Release is published with binaries
- Badges update automatically

## Advantages of UI Workflow

✅ **No Local Setup Required** - Works from any device with GitHub access
✅ **No Git Commands** - Point and click interface
✅ **Visual Feedback** - Real-time logs and summaries
✅ **Branch Override** - Can create releases from feature branches if needed
✅ **Audit Trail** - GitHub records who triggered the workflow
✅ **Easy Rollback** - Just delete the tag via UI if needed

## Comparison with CLI Script

| Feature | GitHub UI | CLI Script |
|---------|-----------|------------|
| Local clone required | No | Yes |
| Git installed required | No | Yes |
| Checks uncommitted changes | No | Yes |
| Branch validation | Yes (optional) | Yes (required) |
| Version validation | Yes | Yes |
| Remote execution | Yes | No |
| Visual feedback | Rich UI | Terminal only |

Both methods create the same git tag and trigger the same release workflow.

## Troubleshooting

### Workflow Not Visible
- Ensure the workflow file is merged to your branch
- Check you have the right repository permissions

### "Run workflow" Button Disabled
- Ensure you're logged in with write access
- Check repository permissions

### Workflow Fails
- Check the logs for specific error messages
- Common issues:
  - Tag already exists
  - Not on main branch (and skip_validation not checked)
  - Invalid version format

### Tag Created But No Release
- The release workflow is separate and triggered by the tag
- Check the "Release" workflow in the Actions tab
- Look for any failures in the build steps
