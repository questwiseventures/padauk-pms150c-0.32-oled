# GitHub publication preparation

This folder is a local repository-ready copy of v1.0.1. No Git repository, remote, account connection, upload, tag or GitHub release was created. Existing releases and working projects remain untouched.

## Suggested repository settings

- Name: `padauk-pms150c-0.32-oled` (selected local name; no GitHub repository created).
- Description: copy the one-line text from [REPOSITORY_DESCRIPTION.txt](REPOSITORY_DESCRIPTION.txt).
- Visibility: choose public/private when publishing.
- License: not assigned by this preparation. Confirm appropriate source and artwork redistribution terms before selecting one; no LICENSE file was invented.
- Optional topics: `padauk`, `pms150c`, `mini-c`, `oled`, `ssd1315`, `embedded`.

## Local review

Use this folder as the repository root, with README.md visible at the top level. The application_template is the default starting point; examples remain separate self-contained projects. Original source/config/project files and accepted validation logs match the release ZIP. Minor display-encoding cleanup may affect documentation only; details are in GITHUB_PREPARATION.json.

.gitignore excludes IDE outputs/caches and local converter scratch, but retains .PRJ/.PRE/.C/.h files. .gitattributes disables text conversion for MCU files so Git does not silently alter their accepted bytes. Documentation and host Python use LF in Git. MANIFEST.sha256 was refreshed for this prepared repository, not written back to the release. UPSTREAM_RELEASE.sha256 identifies the original ZIP, which is not embedded in this repository folder.

Run optional host checks from this folder with Python/Pillow available:

```powershell
py -3 -B -m unittest discover -s tests -p test_release.py -v
```

A local host pass does not compile Mini-C. Minimal example IDE/hardware validation remains pending; preserve that distinction in any publication.

## Publish when ready (not performed)

1. Choose repository account/name/visibility and license approach.
2. With GitHub Desktop or Git, create the local repository here, review the file list and commit it. Do not publish the entire historical working tree. Git is not required for using the MCU project itself.
3. Publish/connect the repository to your chosen GitHub account. Review files and the top-level README online.
4. Prepare a draft GitHub Release tagged v1.0.1 using [.github/RELEASE_DRAFT.md](.github/RELEASE_DRAFT.md). Match the tag to the intended commit.
5. Attach the existing original v1.0.1 ZIP and its SHA256 file from your local releases folder. Keep them unchanged; their checksum differs from GitHub's automatic source ZIP, which includes these preparation files.
6. Review the draft and publish only when explicitly ready. No publication step has been carried out here.
