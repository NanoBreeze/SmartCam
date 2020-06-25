#!/bin/sh
# Post-build script for build-root stuff

echo "BLOOOP"
echo "About to chmod"
echo $?
chmod 755 $TARGET_DIR/bin/busybox
echo $HOST_DIR
echo $STAGING_DIR

echo "echoing done"