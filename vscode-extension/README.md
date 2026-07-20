# Latgalīte Language Support

VS Code language support for the Latgalīte interpreter in this repository.
Files ending in `.lat` are recognized automatically.

## Features

- Syntax highlighting for declarations, types, control flow, word and symbol
  operators, booleans, numbers, strings, comments, functions, and built-ins.
- Correct handling of Latgalīte's UTF-8 identifiers and case-sensitive
  keywords.
- Matching and automatic closing for parentheses, brackets, braces, and
  double-quoted strings.
- Block indentation and line/block comment commands.

The extension is declarative: it has no runtime code, dependencies, or build
step.

## Install in VSCodium

A ready-to-install package is included in this folder. From the
`vscode-extension` directory, run:

```bash
codium --install-extension ./latgalite-language-support-0.1.0.vsix
```

Alternatively, open VSCodium's Extensions view, choose **Install from VSIX...**
from the `...` menu, and select `latgalite-language-support-0.1.0.vsix`.

After installation, open a `.lat` file. VSCodium should select **Latgalīte** as
its language mode automatically.

## Try it without installing

1. Open the `vscode-extension` folder as the root folder in VS Code or
   VSCodium.
2. Press `F5` and select **Run Latgalīte Extension** if prompted.
3. In the Extension Development Host window, open any `.lat` file from the
   parent project.

To inspect how a token was classified, run **Developer: Inspect Editor Tokens
and Scopes** from the Command Palette.

## Rebuild the package

After changing the grammar or configuration, regenerate the VSIX with the VS
Code Extension Manager:

```bash
npx @vscode/vsce package --no-dependencies --allow-missing-repository
```

VS Code users can install the same package by replacing `codium` with `code` in
the installation command above.
