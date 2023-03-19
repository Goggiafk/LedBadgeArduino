


PIO_BIN_DIR=.pio/build/cyberbadge_smol/
WLED_FW_PATH=$PIO_BIN_DIR/WLED_FW.bin


curl -s https://api.github.com/repos/Aircoookie/WLED/releases/latest \
| grep "ESP32.bin" \
| grep "https" \
| cut -d : -f 2,3 \
| tr -d \" \
| wget -O $WLED_FW_PATH -i -

# 352 = 0x160 - exact number of 4096b blocks in 0x80000 flash area
littlefs_create -i $PIO_BIN_DIR/littlefs.bin -c 176 -s data

esptool.py -t --chip ESP32 merge_bin -o release.bin --flash_mode dio --flash_size 4MB \
0x1000 $PIO_BIN_DIR/bootloader.bin \
0x8000 $PIO_BIN_DIR/partitions.bin \
0xe000 $PIO_BIN_DIR/littlefs.bin \
0x180000 $PIO_BIN_DIR/firmware.bin \
0x2B0000 $WLED_FW_PATH




