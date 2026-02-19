AS	= lwasm
ASFLAGS	= --raw
RM	= rm -f

ROMS	= bas13.rom extbas11.rom disk11.rom coco3.rom

all: $(ROMS) checksum

bas13.rom: equates.asm bas.asm
	$(AS) $(ASFLAGS) -o $@ $^

cb_equates.asm: equates.asm bas.asm
	$(AS) --symbol-dump --no-output $^ > $@

extbas11.rom: cb_equates.asm extbas.asm
	$(AS) $(ASFLAGS) -o $@ $^

ecb_equates.asm: equates.asm bas.asm extbas.asm
	$(AS) --symbol-dump --no-output $^ > $@

disk11.rom: ecb_equates.asm disk.asm
	$(AS) $(ASFLAGS) --pragma=noindex0tonone -o $@ $^

coco3.rom: equates.asm coco3.asm
	$(AS) $(ASFLAGS) -o $@ $^

checksum:
	md5 *.rom > md5sums-new.txt

clean:
	$(RM) $(ROMS) *_equates.asm md5sums-new.txt
