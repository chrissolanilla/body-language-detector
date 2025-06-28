# Body Language Detection GUI app and backend
this is an app meant to assist visually impaired users get nonverbal information such as body language from video feed
## Installation
To build the app, run this:

    git submodule init --init --recursive
    mkdir build
    cd build
    cmake .. -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON # compile flags for lsp
    ninja

## running the app
    cd build
    ./executable or whatever its named at this point

## why no python?
Its much harder to use python with gtk and this version of opencv. C++ is cooler anyways(and i guess its faster)
