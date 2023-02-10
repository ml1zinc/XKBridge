# X11 Keyboard Bridge (xkbridge)
## About

XKBridge is a command-line program for converting mistype text from one language to others.

XKBridge taking selected text, convert to other and put chosen variant to clipboard.

Useful for multi-language keyboard layout.

Example: Руддщ цщкдв! ⇨ Hello world :

![example](https://github.com/ml1zinc/XKBridge/raw/main/example.png)

## Usage

1. Select some text.
2. run XKBridge:
```shell
xkbridge
```
3. Choose one of variant. Also, possible to choose by num key (0-3 by default or 1-4 with flag `-n`).

To close the window without selection press `Esc` or `q`.

If there is no need to keep case of text, use `-c` to convert to lowercase.

### Advice

Much more convenient, to bind XKBridge to some shortcut.

And run XKBridge by shortcut.

## Build

### Build Deps
 - libx11
 - libx11-dev (Debian and derivatives distro)
 - libx11-devel (VoidOS)

### Build
Default build:
```shell
make
```

#### Debug
For debbuging purpose build:
```shell
make debug
```
