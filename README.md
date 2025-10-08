# absent
A simple X tiling window manager

## Description

The main motivation behind creating **absent** was the lack of dynamic window managers that efficiently handle multiple monitors while remaining easy to configure. This project aims to provide a window manager that is:

- **Dynamic**
- **Simple to configure**
- **Efficient with multiple monitors** 
- **Lightweight and performant**  

# Installation

Dependencies

```console
gcc, make, libconfig, xcb, xcb-util, xcb-proto, xcb-util-keysyms, xcb-util-cursor, xcb-util-wm (xcb-util-icccm), xkbcommon, xcb-cursors, xcb-randr
```

Install window manager

- clone this repo and run

```console
sudo make install
```

Uninstalling window manager

- to remove all binaries and session file run

```console
sudo make uninstall
```

# Configuration
Default configguration file is `/etc/absent/absent.cfg`, you can copy and modify it in `~/.config/absent/absent.cfg`. If there is an error in your configuration file absent will automaticly parce default configuration instead of yours and print error logs in `/tmp/absent.log`.
