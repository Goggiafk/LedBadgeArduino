


PIO_BIN_DIR=.pio/build/featheresp32
WLED_FW_PATH=$PIO_BIN_DIR/WLED_FW.bin


curl -s https://api.github.com/repos/Aircoookie/WLED/releases/latest \
| grep "ESP32.bin" \
| grep "https" \
| cut -d : -f 2,3 \
| tr -d \" \
| wget -O $WLED_FW_PATH -i -

# 352 = 0x160 - exact number of 4096b blocks in 0x160000 flash area
littlefs_create -i $PIO_BIN_DIR/littlefs.bin -c 352 -s src/wled_presets

esptool.py -t --chip ESP32 merge_bin -o release.bin --flash_mode dio --flash_size 16MB \
0x1000 $PIO_BIN_DIR/bootloader.bin \
0x8000 $PIO_BIN_DIR/partitions.bin \
0x10000 $PIO_BIN_DIR/firmware.bin \
0x210000 $WLED_FW_PATH \
0x410000 $PIO_BIN_DIR/littlefs.bin



