#! /bin/bash
# Build googletest library used for testing solver

GTEST_DIR="third_party/gtest/googletest/"

if ! [[ -d third_party/gtest ]]; then
    echo "  CLONE  $(basename ${GTEST_DIR})"
    git clone https://github.com/google/googletest.git third_party/gtest \
        > /dev/null 2>&1
    if ! [[ -d "${GTEST_DIR}" ]]; then
        echo "  ERROR    cannot clone repository" >&2
        exit 1
    fi
fi


if ! [[ -e "$GTEST_DIR"libgtest.a ]]; then
    echo "  BUILD  $(basename ${GTEST_DIR})"
    g++ -isystem $GTEST_DIR/include -I${GTEST_DIR} \
        -pthread -c ${GTEST_DIR}/src/gtest-all.cc -o ${GTEST_DIR}/gtest-all.o \
        > /dev/null 2>&1
    ar -rv ${GTEST_DIR}/libgtest.a ${GTEST_DIR}/gtest-all.o > /dev/null 2>&1
fi

if ! [[ -e "$GTEST_DIR"libgtest_main.a ]]; then
    g++ -isystem $GTEST_DIR/include -I${GTEST_DIR} \
        -pthread -c ${GTEST_DIR}/src/gtest_main.cc \
        -o ${GTEST_DIR}/gtest_main.o > /dev/null 2>&1
    ar -rv ${GTEST_DIR}/libgtest_main.a ${GTEST_DIR}/gtest_main.o \
       > /dev/null 2>&1
fi
