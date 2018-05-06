# 8bkc-chooser

The first thing you see when you start your PocketSprite is the chooser. It's main tasks are to allow you to upload/delete files (including apps) via WiFi Access Point and launch apps that are present on the filesystem.

## Applications
The chooser makes use of [APPFS](https://github.com/PocketSprite/8bkc-sdk/blob/master/8bkc-components/appfs/include/appfs.h) and a [custom bootloader](https://github.com/PocketSprite/8bkc-sdk/tree/master/8bkc-components/bootloader) from [8bkc-sdk](https://github.com/PocketSprite/8bkc-sdk). This allows you to boot custom esp image files (.bin files from build folder) from the filesystem without touching the base system. 

As an example updating [GnuBoy](https://github.com/PocketSprite/8bkc-gnuboy) is done by cloning the repository, compiling with make, renaming build/gnuboy.bin to gnuboy.app and uploading it via Wifi.

## Building chooser
1. Follow the instructions linked in the Readme of the [8bkc-sdk](https://github.com/PocketSprite/8bkc-sdk)
2. Install ImageMagick (convert) and xxd
3. Clone the repository and it's submodules (git clone --recursive https://github.com/PocketSprite/8bkc-chooser.git)
4. Go into the 8bkc-chooser directory and run make
5. If the build was successfull the binary is located at build/chooser.bin

## Updating chooser
To make sure the PocketSprite can not be bricked the chooser app is included read only in the factory image. If a chooser.app is present on the APPFS the bootloader tries to boot that first instead of the factory partition. To update the chooser simply rename the chooser.bin from above to chooser.app and upload it via WiFi.

# PocketSprite recovery
If for any reason your PocketSprite does not boot anymore (e.g. you uploaded a broken chooser) you can:

1. Reset the PocketSprite by pressing and holding Power+Start+Select
2. Go into recovery mode by releasing only the Power button and keeping Start+Select pressed

From there you can erase the flash, reset the nvs and do a factory reset
