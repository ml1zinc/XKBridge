# X11 Keyboard Bridge (xkbridge)
## About

XKBridge is a command-line program for converting mistype text from one language to other.

XKBridge taking selected text, convert to other and put chosen variant to clipboard.

Useful for multi language keyboard layout.

Example: Руддщ цщкдв! ⇨ Hello world

## Usage

1. Select some text.
2. run XKBridge:
```shell
xkbridge
```
3. Choose one of variant. Also possible choose by num key (0-3 by default or 1-4 with flag `-n`).

If there is no need to keep case of text, use `-c` for convert to lowercase.

### Advice

Much more convenient bind XKBridge to some shortcut.

And run XKBridge by shortcut.

## Build

### Build Deps
 - libx11
 - libx11-dev (Debian and derivatives distro)
 - libx11-devel (VoidOS)

### Build

```shell
make
```
