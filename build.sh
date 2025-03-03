echo "Removing old module!"
sudo rmmod (our driver.ko here)

echo "Building Driver now!"
make ||{ echo "Build failed!"; exit 1; }

echo "Inserting module!"
sudo insmod "our driver.ko here"

echo "Driver inserted!"
sudo mknod /dev/(our device name here) c 240 0 //replace with major and minor!

echo "Device node created!"
sudo chmod 666 /dev/(our device name here)

echo "Permissions set!"
echo "Driver built and inserted!"
