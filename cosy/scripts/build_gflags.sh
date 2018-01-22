#! /bin/bash
# Build googlegflags library used for options

LIB_DIR='third_party/gflags/'

if ! [[ -d third_party/gflags ]]; then
    git clone https://github.com/gflags/gflags.git ${LIB_DIR}
fi

if [[ -e "${LIB_DIR}"lib/libgflags.a ]] ; then
    exit 0
fi

mkdir -p "$LIB_DIR/build"
cd "$LIB_DIR/build"
cmake ..
make
cd -
