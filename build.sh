# Clean up old module
echo "Removing old module..."
sudo rmmod usb_keylogger 2>/dev/null

# Build the module
echo "Building driver..."
make || { echo "Build failed!"; exit 1; }

# Insert the module
echo "Inserting module..."
sudo insmod usb_keylogger.ko || { echo "Failed to insert module!"; exit 1; }

# Check if the module is loaded
echo "Checking if module is loaded..."
if lsmod | grep -q "usb_keylogger"; then
    echo "Module loaded successfully."
else
    echo "Module not loaded. Check kernel logs (dmesg) for errors."
    exit 1
fi

# Check if /proc files are created
echo "Checking /proc files..."
if [ -f "/proc/usb_keylogger" ] && [ -f "/proc/usb_keylogger_encrypted" ]; then
    echo "/proc files created successfully."
else
    echo "/proc files not found. Check kernel logs (dmesg) for errors."
    exit 1
fi

# Set permissions for /proc files (optional)
echo "Setting permissions for /proc files..."
sudo chmod 0444 /proc/usb_keylogger
sudo chmod 0444 /proc/usb_keylogger_encrypted

# Print kernel logs for debugging
echo "Printing kernel logs (dmesg)..."
dmesg | grep usb_keylogger

echo "Driver built and inserted successfully!"
