# EasyRPG Player online fork

Adds multiplayer functionality. 

-Syncs other players sprites and movement.

-Syncs other players SFX with volume depending on distance to the player.

-Syncs other players weather graphics.

-Syncs other players animation frames.

-Syncs other players facing direction.

-Adds posibility of switch sync (custom switch whitelist needed for each game)

-Displays players nicknames over player sprites.

-Adds API for main player sprite change.

-Adds chat API and in-game chat.


Builds for the emscripten target only.

Server: https://gitlab.com/CataractJustice/YNOnline

## Documentation

Documentation is available at the documentation wiki: https://wiki.easyrpg.org

## Building

Follow these steps:

1) Set up emscripten toolchain

```
cd
mkdir workdir
cd workdir
git clone https://github.com/EasyRPG/buildscripts
cd buildscripts
cd emscripten
./0_build_everything.sh
cd emsdk-portable
source ./emsdk_env.sh
```

2) Build liblcf
```
cd ~/workdir
git clone https://github.com/EasyRPG/liblcf
cd liblcf
export EM_PKG_CONFIG_PATH=$HOME/workdir/buildscripts/emscripten/lib/pkgconfig
autoreconf -fi
emconfigure ./configure --prefix=$HOME/workdir/buildscripts/emscripten --disable-shared
make install
```

3) Build ynoclient
```
cd ~/workdir
git clone https://github.com/twig33/ynoclient
cd ynoclient
./cmake_build.sh
cd build
ninja
```

The files you want are build/index.wasm and build/index.js

## Source files of interest
Check [the initial commit.](https://github.com/twig33/ynoclient/commit/218c56586b598a9e3889ed74cd606ed699d159ca)
