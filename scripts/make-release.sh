#!/bin/sh

openmmc_root=$(realpath ..)
afc_v3_1_build_dir="${openmmc_root}/build/releases/afcv3.1"
afc_v4_build_dir="${openmmc_root}/build/releases/afcv4.0"
bin_dir="${openmmc_root}/build/releases/binaries"
tag=$(git describe --tags)

set -e

rm -rf "${openmmc_root}/build/releases/"
mkdir -p "$bin_dir"
mkdir -p "$afc_v3_1_build_dir"
mkdir -p "$afc_v4_build_dir"

cd "$afc_v3_1_build_dir"
cmake "$openmmc_root" -DBOARD=afc -DVERSION=3.1 -DCMAKE_BUILD_TYPE=RelWithDebInfo -DDISABLE_WATCHDOG=OFF -DBOARD_RTM=8sfp
make

cd "$afc_v4_build_dir"
cmake "$openmmc_root" -DBOARD=afc -DVERSION=4.0 -DCMAKE_BUILD_TYPE=RelWithDebInfo -DDISABLE_WATCHDOG=OFF -DBOARD_RTM=lamp
make

cp "${afc_v3_1_build_dir}/out/openMMC.elf" "${bin_dir}/openMMC-afcv3.1-8sfp-${tag}.elf"
cp "${afc_v3_1_build_dir}/out/openMMC.bin" "${bin_dir}/openMMC-afcv3.1-8sfp-${tag}.bin"
[[ -e "${afc_v3_1_build_dir}/out/openMMC.hpm" ]] && cp "${afc_v3_1_build_dir}/out/openMMC.hpm" "${bin_dir}/openMMC-afcv3.1-8sfp-${tag}.hpm"

cp "${afc_v3_1_build_dir}/out/newboot.elf" "${bin_dir}/newboot-afcv3.1-${tag}.elf"
cp "${afc_v3_1_build_dir}/out/newboot.bin" "${bin_dir}/newboot-afcv3.1-${tag}.bin"
[[ -e "${afc_v3_1_build_dir}/out/newboot.hpm" ]] && cp "${afc_v3_1_build_dir}/out/newboot.hpm" "${bin_dir}/newboot-afcv3.1-${tag}.hpm"

cp "${afc_v4_build_dir}/out/openMMC.elf" "${bin_dir}/openMMC-afcv4.0-lamp-${tag}.elf"
cp "${afc_v4_build_dir}/out/openMMC.bin" "${bin_dir}/openMMC-afcv4.0-lamp-${tag}.bin"
[[ -e "${afc_v4_build_dir}/out/openMMC.hpm" ]] && cp "${afc_v4_build_dir}/out/openMMC.hpm" "${bin_dir}/openMMC-afcv4.0-lamp-${tag}.hpm"

cp "${afc_v4_build_dir}/out/newboot.elf" "${bin_dir}/newboot-afcv4.0-${tag}.elf"
cp "${afc_v4_build_dir}/out/newboot.bin" "${bin_dir}/newboot-afcv4.0-${tag}.bin"
[[ -e "${afc_v4_build_dir}/out/newboot.hpm" ]] && cp "${afc_v4_build_dir}/out/newboot.hpm" "${bin_dir}/newboot-afcv4.0-${tag}.hpm"

cd "${bin_dir}"
sha256sum * > SHA256SUMS
