#!/bin/bash

baseDir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")
libdir="$baseDir/libs"

ulimit -c unlimited
chmod u+x "$baseDir/DailyDesktopWallpaperPlus"
export LD_LIBRARY_PATH=$libdir
exec "$baseDir/DailyDesktopWallpaperPlus"