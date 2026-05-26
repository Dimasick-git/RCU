#!/bin/bash
set -e

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DIST_DIR="$ROOT_DIR/dist"
CORES="$(nproc --all)"

if [[ -n "$1" ]]; then
    DIST_DIR="$1"
fi

echo
echo "*** Compiling ryazha-clk ***"
TITLE_ID="$(grep -oP '"title_id":\s*"0x\K(\w+)' "$ROOT_DIR/sysmodule/perms.json")"

pushd "$ROOT_DIR/sysmodule"
make -j$CORES
popd > /dev/null

mkdir -p "$DIST_DIR/atmosphere/contents/$TITLE_ID/flags"
cp -vf "$ROOT_DIR/sysmodule/out/ryazha-clk.nsp" "$DIST_DIR/atmosphere/contents/$TITLE_ID/exefs.nsp"
>"$DIST_DIR/atmosphere/contents/$TITLE_ID/flags/boot2.flag"
cp -vf "$ROOT_DIR/sysmodule/toolbox.json" "$DIST_DIR/atmosphere/contents/$TITLE_ID/toolbox.json"

echo
echo "*** Compiling ryazha-clk-overlay ***"
pushd "$ROOT_DIR/overlay"
make -j$CORES
popd > /dev/null

mkdir -p "$DIST_DIR/switch/.overlays"
cp -vf "$ROOT_DIR/overlay/out/ryazha-clk-overlay.ovl" "$DIST_DIR/switch/.overlays/ryazha-clk-overlay.ovl"
echo

echo "*** Copying assets ***"
mkdir -p "$DIST_DIR/config/ryazha-clk"
cp -vf "$ROOT_DIR/config.ini.template" "$DIST_DIR/config/ryazha-clk/config.ini.template"
mkdir -p "$DIST_DIR/config/ultrahand/assets/notifications"
cp -vf  "$ROOT_DIR/assets/hoc.rgba" "$DIST_DIR/config/ultrahand/assets/notifications/hoc.rgba"

echo
echo "*** Copying lang ***"
cp -vr "$ROOT_DIR/overlay/lang/" "$DIST_DIR/config/ryazha-clk/lang/"
echo
