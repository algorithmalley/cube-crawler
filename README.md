# cube-crawler
A toy project to solve/scramble a Rubiks cube using Lego Mindstorms EV3

## Cloning

Clone including submodules directly, with:
```git clone --recurse-submodules https://github.com/algorithmalley/cube-crawler.git```. (Or fetch the submodules after a regular clone, with: ```git submodule update --recursive```).

## Building

### ev3dev-lang-cpp

The c++ lib to support the ev3dev is used as a submodule. To be able to use it in cube-crawler, cross-build it once with:
```
cd external/ev3dev-lang-cpp
mkdir build
docker run --rm -it -v .:/src -w /src ev3cc
cd build
export CXX=/usr/bin/arm-linux-gnueabi-g++
export CC=/usr/bin/arm-linux-gnueabi-gcc
cmake .. -DCMAKE_BUILD_TYPE=Release -DEV3DEV_PLATFORM=EV3
make
cd ..
exit
```

### cube-crawler

With the ev3dev-lang-cpp in place, we run:
```
mkdir build
docker run --rm -it -v .:/src -w /src ev3cc
cd build
export CXX=/usr/bin/arm-linux-gnueabi-g++
cmake ..
make
cd ..
exit
```

## Copying

Put the executable on the ev3:
```
scp build/cube-crawler robot@ev3dev:~
```

