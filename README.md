# R-Type

## What is it ???

A multiplayer version of the classic R-Type game made by yours trully, Serveur Robuste's team.

## Build & Run

### Linux

#### Dependencies

Make sur you have these packages installed:

```
build-essentials \
cmake \
git \
libx11-dev \
libxrandr-dev \
libxinerama-dev \
libxcursor-dev \
libxi-dev \
libgl1-mesa-dev \
libasound2-dev \
mesa-common-dev
```

#### Build

Then you can just use the provided script to build & run both the client & server:

```bash
./scripts/build_linux.sh
```

Once this script has been ran, you should see both the client & server executable at the root of your project.

If not, you will find them under /build/bin/Debug/r-type_xxx or /build/bin/Release/r-type_xxx depending on what you choose while running the build script.


#### Run

```bash
# Launches the client
r-type_client
```

```bash
# launches the server
r-type_server -p 8080
```


### Windows

Not done wet :)
