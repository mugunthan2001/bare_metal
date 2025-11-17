# Source code files
BOOT2 = boot2
COMPCRC = compCrc32
CRCVALUE = crc
LNKSCRIPT = link.ld
TARGET = RPI-RP2
USER = $(shell whoami)
TARGET_DEST = "/media/$(USER)/$(TARGET)/"

# Directory to create temporary build files in
BUILDDIR = build

# Compilation related variables
TOOLCHAIN = arm-none-eabi-
CFLAGS ?= -mcpu=cortex-m0plus
LDFLAGS ?= -T link.ld -nostdlib

# Utilities path
UTILS = ../utils

build: makeDir $(BUILDDIR)/$(BOOT2).bin $(BUILDDIR)/$(BOOT2).uf2 copyUF2

makeDir:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/$(COMPCRC).out: $(COMPCRC).cpp
	g++ -I $(UTILS) $< -o $@

$(BUILDDIR)/$(BOOT2).bin: $(BOOT2).c $(BUILDDIR)/$(COMPCRC).out
	$(TOOLCHAIN)gcc $(CFLAGS) $(BOOT2).c -c -o $(BUILDDIR)/$(BOOT2)_temp.o
	$(TOOLCHAIN)objdump -hSD $(BUILDDIR)/$(BOOT2)_temp.o > $(BUILDDIR)/$(BOOT2)_temp.objdump
	$(TOOLCHAIN)objcopy -O binary $(BUILDDIR)/$(BOOT2)_temp.o $(BUILDDIR)/$(BOOT2)_temp.bin
	./$(BUILDDIR)/$(COMPCRC).out $(BUILDDIR)/$(BOOT2)_temp.bin
	$(TOOLCHAIN)gcc $(BOOT2).c $(BUILDDIR)/$(CRCVALUE).c $(CFLAGS) $(LDFLAGS) -o $(BUILDDIR)/$(BOOT2).elf
	$(TOOLCHAIN)objdump -hSD $(BUILDDIR)/$(BOOT2).elf > $(BUILDDIR)/$(BOOT2).objdump
	$(TOOLCHAIN)objcopy -O binary $(BUILDDIR)/$(BOOT2).elf $@

$(BUILDDIR)/$(BOOT2).uf2: $(BUILDDIR)/$(BOOT2).bin
	python3 $(UTILS)/uf2/utils/uf2conv.py -b 0x10000000 -f 0xe48bff56 -c $< -o $@

copyUF2: $(BUILDDIR)/$(BOOT2).uf2
	cp $(BUILDDIR)/$(BOOT2).uf2 ./$(BOOT2).uf2

deploy:
	cp $(BOOT2).uf2 $(TARGET_DEST)

clean:
	rm -rf $(BUILDDIR) $(BOOT2).uf2