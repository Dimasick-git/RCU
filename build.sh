#!/bin/sh

CORES="$(nproc --all)"

SRC="Source/Atmosphere/stratosphere/loader/"
DEST="build/stratosphere/loader/"
mkdir -p "dist/atmosphere/kips/"
mkdir -p "$DEST"

echo
echo "*** Patching loader ***"
cp -vr "$SRC"/. "$DEST"/
echo

echo "*** Patching exosphere ***"
EXO_SRC="Source/Atmosphere-Patches"
EXO_DEST="build/exosphere/program/source/smc"
LIBEXO_DEST="build/libraries/libexosphere/include/exosphere/secmon"

cp -v "$EXO_SRC/secmon_emc_access_table_data.inc"       "$EXO_DEST/"
cp -v "$EXO_SRC/secmon_define_emc_access_table.inc"     "$EXO_DEST/"
cp -v "$EXO_SRC/secmon_rtc_pmc_access_table_data.inc"   "$EXO_DEST/"
cp -v "$EXO_SRC/secmon_define_rtc_pmc_access_table.inc" "$EXO_DEST/"
cp -v "$EXO_SRC/secmon_smc_register_access.cpp"         "$EXO_DEST/"
cp -v "$EXO_SRC/secmon_smc_handler.cpp"                 "$EXO_DEST/"
cp -v "$EXO_SRC/secmon_memory_layout.hpp"               "$LIBEXO_DEST/"
echo

echo "CORES: $CORES"
echo

echo "*** Compiling loader ***"
cd build/stratosphere/loader || exit 1
make -j$CORES
hactool -t kip1 out/nintendo_nx_arm64_armv8a/release/loader.kip --uncompress=rcu.kip
cd ../../../ # exit
cp -v build/stratosphere/loader/rcu.kip dist/atmosphere/kips/rcu.kip

echo
echo "*** Compiling exosphere ***"
cd build/exosphere
make -j$CORES
cd ../../
cp -v build/exosphere/out/nintendo_nx_arm64_armv8a/release/exosphere.bin dist/atmosphere/exosphere.bin

cd Source/ryazha-clk/
./build.sh
cp -r dist/ ../../

cd ../../

echo "*** Compiling ryazha-clk-monitor ***"
cd Source/Ryazha-CLK-Monitor/
make -j$CORES
cp -v Ryazha-CLK-Monitor.ovl ../../dist/switch/.overlays/Ryazha-CLK-Monitor.ovl
