#!/bin/sh
# Post-build script for build-root stuff

#echo "BLOOOP"
#echo "About to chmod"
#echo $?
#chmod 755 $TARGET_DIR/bin/busybox
#echo $HOST_DIR
#echo $STAGING_DIR

#echo "echoing done"

# create a file containing the git commit this build was created from
git rev-parse HEAD > $TARGET_DIR/build_id.txt