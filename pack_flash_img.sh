#!/bin/bash

curPath=$(readlink -f "$(dirname "$0")")
_UBOOT_FILE=$curPath/u-boot/u-boot-sunxi-with-spl.bin
_DTB_FILE=$curPath/linux/arch/arm/boot/dts/suniv-f1c100s-licheepi-nano.dtb
_KERNEL_FILE=$curPath/linux/arch/arm/boot/zImage
_ROOTFS_FILE=$curPath/rootfs-spi-flash.tar.gz
_MOD_FILE=$curPath/linux/out/lib/modules
_IMG_FILE=firmware.bin
_SCREEN_PRAM=480272


if [ -f $_IMG_FILE ] ; then
    echo "Image exist,rename it to .old"
    mv $_IMG_FILE $_IMG_FILE.old
    echo "Creating new an Image"
fi

rm -rf ./_temp 
mkdir _temp &&\
cd _temp &&\
echo "Generate bin..."
dd if=/dev/zero of=flashimg.bin bs=1M count=16 &&\
echo "Packing Uboot..."
dd if=$_UBOOT_FILE of=flashimg.bin bs=1K conv=notrunc &&\
echo "Packing dtb..."
dd if=$_DTB_FILE of=flashimg.bin bs=1K seek=1024  conv=notrunc &&\
echo "Packing zImage..."
cp $_KERNEL_FILE ./zImage &&\
dd if=./zImage of=flashimg.bin bs=1K seek=1088  conv=notrunc &&\
mkdir rootfs
echo "Packing rootfs..."
tar -zxvf $_ROOTFS_FILE -C ./rootfs >/dev/null &&\
cp -r $_MOD_FILE  rootfs/lib/modules/ &&\
mkfs.jffs2 -s 0x100 -e 0x10000 --pad=0xAF0000 -d rootfs/ -o jffs2.img &&\
dd if=jffs2.img of=flashimg.bin  bs=1K seek=5184  conv=notrunc &&\
mv ./flashimg.bin ../$_IMG_FILE &&\
sync
echo "Bin update done!"
cd .. &&\
rm -rf ./_temp 
echo "You configure your LCD parameters as $_SCREEN_PRAM"
echo "Pack $_IMG_FILE finished"
exit






