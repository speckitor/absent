# absent
Simple and flexible X tiling window manager

[Video example](https://youtu.be/6glkO97ToIY)

# Installation

Dependencies

```console
gcc, make, xcb, xcb-util, xcb-proto, xcb-util-keysyms, xcb-util-cursor, xcb-util-wm, xkbcommon, xorg-server, xorg-xinit
```

Install window manager

- clone the repository 

```console
git clone https://github.com/speckitor/absent
```

- compile and install wm with make

```console
sudo make
```

Removing window manager

- to remove all binaries and session file run

```console
sudo make clean
```

# Configuration
The `config.h` file contains all configuration. The `autostartabsent` file contains commands that are executed when the window manager starts (can be disabled in `config.h` or modified). After editing these files you need to recompile wm.
