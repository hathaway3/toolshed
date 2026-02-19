BINARY = casm
OBJS =  as.o cmdline.o context.o cpu.o do9.o error.o eval.o export.o ffwd.o \
		input.o label.o macro.o parse.o \
		output.o output_bin.o output_listing.o output_mod.o \
		output_obj.o output_os9.o output_rof.o output_rom.o \
		output_s19.o \
		proc_branch.o proc_bitxfer.o proc_direct.o proc_extended.o \
		proc_general.o proc_immediate.o proc_indexed.o proc_inherent.o \
		proc_logicalmem.o proc_memxfer.o proc_pushpull.o proc_regtoreg.o \
		proc_util.o \
		pseudo.o struct.o symtab.o table9.o \
		util.o

include ../rules.mak
vpath %.c ../../../casm/src
vpath %.h ../../../casm/src

all: $(BINARY)$(SUFEXE)
