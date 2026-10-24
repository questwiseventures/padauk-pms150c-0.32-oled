# GitHub publication preparation

This folder is a local repository-ready copy of v1.0.1. These instructions do not track the live GitHub publication status. Repository uploads and release publication are managed by the owner. Existing releases and working projects remain untouched.

## Suggested repository settings

- Name: `padauk-pms150c-0.32-oled`.
- Description: copy the one-line text from [REPOSITORY_DESCRIPTION.txt](REPOSITORY_DESCRIPTION.txt).
- Visibility: choose public/private when publishing.
- License: not assigned by this preparation. Confirm appropriate source and artwork redistribution terms before selecting one; no LICENSE file was invented.
- Suggested topics: `padauk`, `pms150c`, `ssd1315`, `oled`, `mini-c`, `embedded`, `microcontroller`. Also available in [REPOSITORY_TOPICS.txt](REPOSITORY_TOPICS.txt). Set the description and topics using the gear beside **About** on the repository page; these local files do not apply GitHub settings automatically.

## Local review

Use this folder as the repository root, with README.md visible at the top level. The application_template is the default starting point; examples remain separate self-contained projects. Original source/config/project files and accepted validation logs match the release ZIP. Minor display-encoding cleanup may affect documentation only; details are in GITHUB_PREPARATION.json.

.gitignore excludes IDE outputs/caches and local converter scratch, but retains .PRJ/.PRE/.C/.h files. .gitattributes disables text conversion for MCU files so Git does not silently alter their accepted bytes. Documentation and host Python use LF in Git. MANIFEST.sha256 was refreshed for this prepared repository, not written back to the release. UPSTREAM_RELEASE.sha256 identifies the original ZIP, which is not embedded in this repository folder.

Run optional host checks from this folder with Python/Pillow available:

```powershell
py -3 -B -m unittest discover -s tests -p test_release.py -v
```

A local host pass does not compile Mini-C. Minimal example IDE/hardware validation remains pending; preserve that distinction in any publication.

## Publication steps

1. Choose repository account/name/visibility and license approach.
2. Using the GitHub web interface, open **Add file > Upload files** and drag the contents of this folder, including its subfolders, into the upload area. Commit the upload. Alternatively, use GitHub Desktop or Git. Upload only this prepared folder's contents, not the entire historical working tree.
3. Review files and the top-level README online, including the subfolders. Apply the description and topics in **About**.
4. Prepare a draft GitHub Release tagged v1.0.1 using [.github/RELEASE_DRAFT.md](.github/RELEASE_DRAFT.md). Match the tag to the intended commit.
5. Attach the existing original v1.0.1 ZIP and its SHA256 file from your local releases folder. Keep them unchanged; their checksum differs from GitHub's automatic source ZIP, which includes these preparation files.
6. Review the draft and publish only when explicitly ready.
