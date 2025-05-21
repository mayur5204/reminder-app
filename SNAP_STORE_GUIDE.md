# Publishing to the Snap Store

This document provides guidance on publishing the Reminder App to the Snap Store.

## Prerequisites

1. A Snapcraft account (sign up at https://snapcraft.io/account)
2. Snapcraft CLI tools installed:
   ```bash
   sudo snap install snapcraft --classic
   ```
3. A properly configured `snap/snapcraft.yaml` file (this should be the case now).

## Steps to Publish

### 1. Log in to Snapcraft

```bash
snapcraft login
```

You'll be prompted to authenticate with your Ubuntu One account.

### 2. Build the Snap Package

Use the improved build script from the project root directory:

```bash
./fix-build-snap.sh
```

This script will:
- Check for required dependencies (snapcraft, LXD/Multipass).
- Offer to install LXD if not found.
- Build the snap in a clean environment (LXD is recommended).
- The built snap will be named something like `reminder-app_1.0_amd64.snap`.
- Offer to install it locally for testing.

### 3. Register the Snap Name

Before publishing for the first time, you need to register your snap name. We are using `reminder-app`.

```bash
snapcraft register reminder-app
```

- If this name is already taken or if your registration is pending manual review by the Snap Store team, you might need to wait or choose a different name. If you change the name, update it in `snap/snapcraft.yaml` and rebuild.
- Name registration can take a few days if manual review is required.

### 4. Upload to the Snap Store

Once you've successfully built your snap and (ideally) tested it locally, you can upload it to the Snap Store. Push it to the `stable` channel (or another channel like `candidate` or `beta` if you prefer).

```bash
snapcraft upload --release=stable reminder-app_1.0_amd64.snap
```

Replace `reminder-app_1.0_amd64.snap` with the actual filename of your built snap.

### 5. Complete Store Listing

After successfully uploading your snap:

1. Visit your Snapcraft publisher dashboard: https://snapcraft.io/publisher/snaps
2. Select your snap (`reminder-app`).
3. Complete the store listing. This is crucial for users to find and understand your app. Include:
    - A clear and concise summary and description (you can reuse parts from `snapcraft.yaml`).
    - High-quality screenshots of the application.
    - A distinctive icon (already included in the snap).
    - Contact information or a link to your project (e.g., GitHub repository).
    - Category selection.

### 6. Troubleshooting Publication Issues

#### Name Registration Pending

- This is a standard process for new snap names to ensure quality and prevent name squatting.
- Wait for the Snap Store review team to process your request. You should receive an email notification.

#### Build Failures during `snapcraft` command

- **Check `snapcraft.yaml` syntax:** Ensure it's valid YAML.
- **Missing build dependencies:** Verify all `build-packages` in `snapcraft.yaml` are correct and sufficient.
- **Path issues:** Ensure all file paths in `snapcraft.yaml` (e.g., for icons, desktop files) are correct relative to the `snap/snapcraft.yaml` file or the project root, depending on the context within the YAML.
- **Permissions:** Sometimes, file permissions within the build environment can cause issues. The `dump` plugin with `install` commands is generally robust.
- **LXD/Multipass issues:** Ensure LXD or Multipass is set up correctly. `sudo lxd init --auto` is a good starting point for LXD.
- **Debug:** Run `snapcraft --debug` or `snapcraft --enable-trace-build` for more verbose output. Check logs, often found in `~/snap/snapcraft/common/log/` or within the `parts/` or `prime/` directories of your build.

#### Store Rejection or Review Feedback

- If your snap is rejected or if you receive feedback from the Snap Store reviewers, carefully read their comments.
- Address any policy violations, security concerns, or functionality issues they point out.
- Make the necessary changes, rebuild the snap, and re-upload it.

## Automating with GitHub Actions

For continuous integration and deployment (CI/CD):

1.  **Set up GitHub Repository:** Ensure your project is on GitHub.
2.  **Snapcraft Login Secret:**
    *   Generate a login token: `snapcraft export-login snap_login.txt`
    *   Store the contents of `snap_login.txt` as a secret in your GitHub repository (e.g., `SNAPCRAFT_LOGIN_TOKEN`).
    *   **Important:** Delete `snap_login.txt` after adding it as a secret.
3.  **GitHub Workflows:** Use or adapt the workflows in `.github/workflows/`:
    *   `build-snap.yml`: Builds the snap on every push/PR.
    *   `publish-snap.yml`: Publishes the snap to the store on new tags or releases (manual trigger recommended for actual publishing).

## Maintaining Your Snap

Once your snap is published:

1.  **Monitor:** Check your Snapcraft dashboard for download statistics, ratings, and reviews.
2.  **Address Issues:** Respond to user feedback and fix reported bugs.
3.  **Publish Updates:** When you have a new version:
    *   Update the `version` string in `snap/snapcraft.yaml`.
    *   Rebuild the snap: `./fix-build-snap.sh`
    *   Upload the new version: `snapcraft upload --release=stable reminder-app_NEW_VERSION_amd64.snap`

## Key Resources

-   [Snapcraft Documentation](https://snapcraft.io/docs)
-   [Publishing your snap](https://snapcraft.io/docs/publishing-your-snap)
-   [Snap Store Policies](https://snapcraft.io/docs/store-policies)
-   [Snapcraft YAML Reference](https://snapcraft.io/docs/snapcraft-yaml-reference)
