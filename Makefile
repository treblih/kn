# Generated automatically from Makefile.in by configure. 
# # Makefile for ZyOs.	-*- Indented-Text -*-
# Copyright (C) 2009 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# 19.12.09, Yang Zhang, NUIST, Nanjing, CHN.

#### Start of system configuration section. ####
ASM		=	nasm
CC		= 	gcc
LD		=	ld
ENTRYPOINT 	=	0x30400

BLFLAG		=	-I boot/include/

CADD		= 	-funroll-loops
CWARN		= 	-Wall -Wshadow -Wpointer-arith -Wunreachable-code
CFLAGS		= 	-I include/ -c -fno-builtin -std=gnu99 $(CADD) $(CWARN)

LDFLAGS		= 	-Ttext $(ENTRYPOINT) -s

BOOT		=	boot/boot.bin
LOADER		=	boot/loader.bin
KERNEL		=	kernel/kernel.bin

BOOTINCLUDE 	= 	boot/include/load.inc  boot/include/fat12hdr.inc  boot/include/pm.inc
OBJ		=	kernel/kernel.o  kernel/cstart.o  kernel/global.o  kernel/proc.o  kernel/init.o \
			kernel/clock.o  kernel/keyboard.o  kernel/tty.o  kernel/console.o  kernel/printf.o \
			kernel/ipc.o  kernel/hd.o  kernel/fs.o  lib/interrupt.o  lib/lib.o  lib/handler_int.o
CSTACTINCLUDE	= 	include/proto.h  include/type.h  include/global.h  \
			include/keyboard.h  include/keymap.h
#### End of system configuration section. ####


#to avoid trouble on systems where the SHELL variable might be inherited from the
#environment. (This is never a problem with GNU make.)
SHELL		=	/bin/sh


.PHONY: files g gvim vim
files:	$(BOOT)  $(LOADER)  $(KERNEL)  vim
g:	$(BOOT)  $(LOADER)  $(KERNEL)  gvim


$(BOOT):		boot/boot.asm  $(BOOTINCLUDE);		$(ASM) $(BLFLAG) -o $@ $<

$(LOADER):		boot/loader.asm  $(BOOTINCLUDE);	$(ASM) $(BLFLAG) -o $@ $<

$(KERNEL):		$(OBJ);					
	# It must b $(OBJ), not $<, 
	# $< just presents only 1 obj-file, namely the 1st in var-OBJ, main.o
	$(LD) $(LDFLAGS) -o $@ $(OBJ)


# gcc -Wa,-I include/ (usr comma instead of space)	pass the as args from gcc, no need to add include prefix
kernel/kernel.o:	kernel/kernel.s  include/sconst.inc;  $(CC) $(CFLAGS) -o $@ $<

lib/interrupt.o:	lib/interrupt.s  include/sconst.inc;  $(CC) $(CFLAGS) -o $@ $<

kernel/cstart.o:	kernel/cstart.c	$(CSTACTINCLUDE);     $(CC) $(CFLAGS) -o $@ $<

kernel/global.o:	kernel/global.c	$(CSTACTINCLUDE);     $(CC) $(CFLAGS) -o $@ $<

kernel/init.o:		kernel/init.c $(CSTACTINCLUDE);       $(CC) $(CFLAGS) -o $@ $<

kernel/proc.o:		kernel/proc.c $(CSTACTINCLUDE);       $(CC) $(CFLAGS) -o $@ $<

kernel/clock.o:		kernel/clock.c $(CSTACTINCLUDE);      $(CC) $(CFLAGS) -o $@ $<

kernel/keyboard.o:	kernel/keyboard.c $(CSTACTINCLUDE);   $(CC) $(CFLAGS) -o $@ $<

kernel/tty.o:		kernel/tty.c $(CSTACTINCLUDE);	      $(CC) $(CFLAGS) -o $@ $<

kernel/console.o:	kernel/console.c $(CSTACTINCLUDE);    $(CC) $(CFLAGS) -o $@ $<

kernel/printf.o:	kernel/printf.c $(CSTACTINCLUDE);     $(CC) $(CFLAGS) -o $@ $<

kernel/ipc.o:		kernel/ipc.c $(CSTACTINCLUDE);        $(CC) $(CFLAGS) -o $@ $<

kernel/hd.o:		kernel/hd.c $(CSTACTINCLUDE);        $(CC) $(CFLAGS) -o $@ $<

kernel/fs.o:		kernel/fs.c $(CSTACTINCLUDE);        $(CC) $(CFLAGS) -o $@ $<

lib/lib.o:		lib/lib.c $(CSTACTINCLUDE);	      $(CC) $(CFLAGS) -o $@ $<

lib/handler_int.o:	lib/handler_int.c $(CSTACTINCLUDE);   $(CC) $(CFLAGS) -o $@ $<


# GVIM has shell-builtin but no tty, so can't run "sudo"
gvim:
	#dd if=boot/boot.bin of=a.img bs=512 count=1 conv=notrunc
	su -c 'mount -o loop a.img /mnt/floppy'
	#sudo cp -fv boot/loader.bin /mnt/floppy
	su -c 'cp -fv kernel/kernel.bin /mnt/floppy'
	su -c 'umount /mnt/floppy'
	rem kernel/*.o lib/*.o

# /etc/sudoers  %wheel NOPASSWD
vim:
	#dd if=boot/boot.bin of=a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop a.img /mnt/floppy
	#sudo cp -fv boot/loader.bin /mnt/floppy
	sudo cp -fv kernel/kernel.bin /mnt/floppy
	sudo umount /mnt/floppy
	rem kernel/*.o lib/*.o
