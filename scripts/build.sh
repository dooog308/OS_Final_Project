IMAGE=./arch/x86_64/boot/bzImage 
ROOTFS=./root.img 
KERNEL_PATH=/home/dooog308/OS_final_project/OS_Final_Project

cd $KERNEL_PATH
rm root.img
rm -r tmp

#git will ignore empty directory, so we need to recreat it.
cd _install
mkdir dev
mkdir proc
mkdir sys
cd ../

#build rootfs
dd if=/dev/zero of=root.img bs=1M count=2048
mkfs.ext4 -F root.img
mkdir tmp
sudo mount -o loop root.img tmp/
cd tmp/
sudo cp -a ../_install/. ./
cd ../
sudo umount tmp

#compile kernel and launch qemu
make -j12
qemu-system-x86_64 -vga std -kernel $IMAGE -drive file=$ROOTFS,format=raw,media=disk,index=0,id=x0 -append "root=/dev/sda rw"
