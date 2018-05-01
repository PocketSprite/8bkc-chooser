#
# Main Makefile. This is basically the same as a component makefile.
#
# This Makefile should, at the very least, just include $(SDK_PATH)/make/component_common.mk. By default, 
# this will take the sources in the src/ directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#

COMPONENTS_EXTRA_CLEAN := graphics.inc graphics.rgba
gui.o: graphics.inc

COMPILEDATE:=\"$(shell date "+%d %b %Y")\"
GITREV:=\"$(shell git rev-parse HEAD | cut -b 1-10)\"

CFLAGS += -DCOMPILEDATE="$(COMPILEDATE)" -DGITREV="$(GITREV)"

include $(IDF_PATH)/make/component_common.mk


graphics.inc: $(COMPONENT_PATH)/graphics.xcf
	convert $^ -background none -layers flatten graphics.rgba
	#convert $^ -background none -layers flatten -crop 80x192+0+0 graphics.png
	cat graphics.rgba | xxd -i > graphics.inc
