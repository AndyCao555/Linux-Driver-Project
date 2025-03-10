echo "Removing old module!"
sudo rmmod Driver.ko

echo "Building Driver now!"
make ||{ echo "Build failed!"; exit 1; }

echo "Inserting module!"
sudo insmod Driver.ko

echo "Driver inserted!"
sudo mknod /dev/usb_keylogger c 240 0 #replace with major and minor!

echo "Device node created!"
sudo chmod 666 /dev/usb_keylogger

echo "Permissions set!"
echo "Driver built and inserted!"
