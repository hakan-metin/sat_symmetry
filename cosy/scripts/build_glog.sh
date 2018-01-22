#! /bin/bash
# Build googlelog library used for logging


GLOG_DIR='third_party/glog/'

if ! [[ -d "${GLOG_DIR}" ]]; then
    echo "  CLONE    $(basename ${GLOG_DIR})"
    git clone https://github.com/google/glog.git "${GLOG_DIR}" > /dev/null 2>&1
    if ! [[ -d "${GLOG_DIR}" ]]; then
        echo "  ERROR    cannot clone repository" >&2
        exit 1
    fi
fi

if ! [[ -e "${GLOG_DIR}"lib/libglog.a ]] ; then

    cd "${GLOG_DIR}"

    echo "  AUTOGEN  $(basename ${GLOG_DIR})"
    ./autogen.sh > /dev/null 2>&1

    echo " CONFIGURE $(basename ${GLOG_DIR})"
    ./configure --prefix=$PWD > /dev/null 2>&1

    echo "   MAKE    $(basename ${GLOG_DIR})"
    make  > /dev/null 2>&1

    make install  > /dev/null 2>&1
    cd - > /dev/null 2>&1

fi
