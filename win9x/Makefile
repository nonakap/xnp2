# makefile for cygwin
#   gcc version 4.3.4

AS		= nasm
CC		= gcc
RC		= windres
RM		= rm -f

OBJBASE	= ../obj
BINBASE	= ../bin

OBJ		= $(OBJBASE)/cygwin
BIN		= $(BINBASE)/cygwin

BASE		= ..
COMMON		= ../common
CODECNV		= ../codecnv
I286C		= ../i286c
MEM			= ../mem
IO			= ../io
CBUS		= ../cbus
BIOS		= ../bios
SOUND		= ../sound
VERMOUTH	= ../sound/vermouth
GETSND		= ../sound/getsnd
VRAM		= ../vram
FDD			= ../fdd
LIO			= ../lio
FONT		= ../font
GENERIC		= ../generic
ZLIB		= ../zlib
TARGET		= .
X86ASM		= ./x86
DIALOG		= ./dialog
DEBUGUTY	= ./debuguty

I286CASM	= $(I286C)/x86
IOASM		= $(IO)/x86
VRAMASM		= $(VRAM)/x86

DXSDKINC	= "$(DXSDK_DIR)include"

RESDIR		= ./resources/1252

HEADERS		=	$(TARGET)/compiler.h $(BASE)/common.h $(COMMON)/milstr.h \
				$(COMMON)/_memory.h $(COMMON)/rect.h $(COMMON)/lstarray.h \
				$(TARGET)/trace.h

CPUCORES	=	$(I286C)/cpucore.h $(I286C)/memory.h

PCCORES		=	$(BASE)/pccore.h $(BASE)/nevent.h $(BASE)/statsave.h

IOCORES		=	$(IO)/iocore.h $(IO)/artic.h $(IO)/cgrom.h $(IO)/cpuio.h \
				$(IO)/crtc.h $(IO)/dipsw.h $(IO)/dmac.h $(IO)/egc.h \
				$(IO)/epsonio.h $(IO)/emsio.h $(IO)/fdc.h $(IO)/fdd320.h \
				$(IO)/gdc.h $(IO)/gdc_cmd.h $(IO)/gdc_sub.h $(IO)/mouseif.h \
				$(IO)/necio.h $(IO)/nmiio.h $(IO)/np2sysp.h $(IO)/pic.h \
				$(IO)/pit.h $(IO)/printif.h $(IO)/serial.h $(IO)/sysport.h \
				$(IO)/upd4990.h

FMBOARDS	=	$(SOUND)/soundrom.h $(SOUND)/tms3631.h $(SOUND)/fmtimer.h \
				$(SOUND)/opngen.h $(SOUND)/psggen.h $(SOUND)/rhythm.h \
				$(SOUND)/adpcm.h $(SOUND)/pcm86.h $(SOUND)/cs4231.h

VERMOUTHS	=	$(VERMOUTH)/midiout.h $(VERMOUTH)/midimod.h \
				$(VERMOUTH)/midinst.h $(VERMOUTH)/midvoice.h \
				$(VERMOUTH)/midtable.h

ZLIBS		=	$(ZLIB)/crc32.h $(ZLIB)/inffast.h $(ZLIB)/inffixed.h \
				$(ZLIB)/inflate.h $(ZLIB)/inftrees.h $(ZLIB)/zconf.h \
				$(ZLIB)/zlib.h $(ZLIB)/zutil.h


AOPT	= -f win32
COPT	= -c -Wall -pipe -O6 -fomit-frame-pointer -fkeep-inline-functions \
			-fsigned-char -mwindows -march=pentium \
			-DWIN32 -DNDEBUG -D_WINDOWS \
			-I$(BASE) -I$(COMMON) -I$(CODECNV) -I$(I286C) -I$(MEM) -I$(IO) \
			-I$(CBUS) -I$(BIOS) -I$(LIO) -I$(VRAM) -I$(SOUND) -I$(VERMOUTH) \
			-I$(GETSND) -I$(FDD) -I$(FONT) -I$(GENERIC) -I$(TARGET) \
			-I$(DIALOG) -I$(DEBUGUTY) -I$(X86ASM) -I$(ZLIB)
LOPT	= -mwindows -lstdc++

NP2OBJ	=	$(OBJ)/strres.o		$(OBJ)/milstr.o		$(OBJ)/_memory.o	\
			$(OBJ)/textfile.o	$(OBJ)/profile.o	$(OBJ)/rect.o		\
			$(OBJ)/lstarray.o	$(OBJ)/bmpdata.o	$(OBJ)/mimpidef.o	\
			$(OBJ)/parts.oa		$(OBJ)/arc.o		$(OBJ)/arcunzip.o	\
			\
			$(OBJ)/adler32.o	$(OBJ)/crc32.o		$(OBJ)/inffast.o	\
			$(OBJ)/inflate.o	$(OBJ)/inftrees.o	$(OBJ)/zutil.o		\
			\
			$(OBJ)/utf8ucs2.o	$(OBJ)/ucs2utf8.o	$(OBJ)/tcswap16.o	\
			\
			$(OBJ)/i286c.o		$(OBJ)/i286c_mn.o	$(OBJ)/i286c_ea.o	\
			$(OBJ)/i286c_0f.o	$(OBJ)/i286c_8x.o	$(OBJ)/i286c_sf.o	\
			$(OBJ)/i286c_f6.o	$(OBJ)/i286c_fe.o	$(OBJ)/i286c_rp.o	\
			$(OBJ)/v30patch.o	$(OBJ)/memory.o							\
			\
			$(OBJ)/dmax86.o		$(OBJ)/dmav30.o		$(OBJ)/memtram.o	\
			$(OBJ)/memvram.o	$(OBJ)/memegc.o		$(OBJ)/memems.o		\
			$(OBJ)/memepp.o												\
			\
			$(OBJ)/iocore.o		$(OBJ)/artic.o		$(OBJ)/cgrom.o		\
			$(OBJ)/cpuio.o		$(OBJ)/crtc.o		$(OBJ)/dipsw.o		\
			$(OBJ)/dmac.o		$(OBJ)/egc.o		$(OBJ)/emsio.o		\
			$(OBJ)/epsonio.o	$(OBJ)/fdc.o		$(OBJ)/fdd320.o		\
			$(OBJ)/gdc.o		$(OBJ)/gdc_sub.o	$(OBJ)/gdc_pset.o	\
			$(OBJ)/mouseif.o	$(OBJ)/necio.o		$(OBJ)/nmiio.o		\
			$(OBJ)/np2sysp.o	$(OBJ)/pic.o		$(OBJ)/pit.o		\
			$(OBJ)/printif.o	$(OBJ)/serial.o		$(OBJ)/sysport.o	\
			$(OBJ)/upd4990.o											\
			\
			$(OBJ)/cbuscore.o	$(OBJ)/sasiio.o		$(OBJ)/scsiio.o		\
			$(OBJ)/scsicmd.o	$(OBJ)/pc9861k.o	$(OBJ)/mpu98ii.o	\
			$(OBJ)/board14.o	$(OBJ)/board26k.o	$(OBJ)/board86.o	\
			$(OBJ)/boardx2.o	$(OBJ)/board118.o	$(OBJ)/boardspb.o	\
			$(OBJ)/amd98.o		$(OBJ)/pcm86io.o	$(OBJ)/cs4231io.o	\
			\
			$(OBJ)/bios.o		$(OBJ)/bios09.o		$(OBJ)/bios0c.o		\
			$(OBJ)/bios12.o		$(OBJ)/bios13.o		$(OBJ)/bios18.o		\
			$(OBJ)/bios19.o		$(OBJ)/bios1a.o		$(OBJ)/bios1b.o		\
			$(OBJ)/bios1c.o		$(OBJ)/bios1f.o		$(OBJ)/sxsibios.o	\
			$(OBJ)/lio.o		$(OBJ)/gscreen.o	$(OBJ)/gpset.o		\
			$(OBJ)/gline.o		$(OBJ)/gcircle.o	$(OBJ)/gput1.o		\
			$(OBJ)/vram.o		$(OBJ)/scrndraw.o	$(OBJ)/sdraw.o		\
			$(OBJ)/dispsync.o	$(OBJ)/palettes.o	$(OBJ)/maketext.o	\
			$(OBJ)/maketgrp.o	$(OBJ)/makegrph.oa	$(OBJ)/scrnsave.o	\
			\
			$(OBJ)/sound.o		$(OBJ)/sndcsec.o	$(OBJ)/fmboard.o	\
			$(OBJ)/fmtimer.o	$(OBJ)/beepc.o		$(OBJ)/beepg.o		\
			$(OBJ)/tms3631c.o	$(OBJ)/tms3631g.o	$(OBJ)/opngenc.o	\
			$(OBJ)/opngeng.oa	$(OBJ)/psggenc.o	$(OBJ)/psggeng.o	\
			$(OBJ)/rhythmc.o	$(OBJ)/adpcmc.o		$(OBJ)/adpcmg.o		\
			$(OBJ)/pcm86c.o		$(OBJ)/pcm86g.o		$(OBJ)/cs4231c.o	\
			$(OBJ)/cs4231g.o	$(OBJ)/soundrom.o	$(OBJ)/s98.o		\
			\
			$(OBJ)/getsnd.o		$(OBJ)/getwave.o	$(OBJ)/getsmix.o	\
			\
			$(OBJ)/midiout.o	$(OBJ)/midimod.o	$(OBJ)/midinst.o	\
			$(OBJ)/midvoice.o	$(OBJ)/midtable.o						\
			\
			$(OBJ)/diskdrv.o	$(OBJ)/newdisk.o	$(OBJ)/fddfile.o	\
			$(OBJ)/fdd_xdf.o	$(OBJ)/fdd_d88.o	$(OBJ)/fdd_mtr.o	\
			$(OBJ)/sxsi.o		$(OBJ)/sxsihdd.o	$(OBJ)/sxsicd.o		\
			\
			$(OBJ)/font.o		$(OBJ)/fontdata.o	$(OBJ)/fontpc88.o	\
			$(OBJ)/fontpc98.o	$(OBJ)/fontv98.o	$(OBJ)/fontfm7.o	\
			$(OBJ)/fontx1.o		$(OBJ)/fontx68k.o	$(OBJ)/fontmake.o	\
			\
			$(OBJ)/cmndraw.o	$(OBJ)/keydisp.o	$(OBJ)/softkbd.o	\
			$(OBJ)/dipswbmp.o	$(OBJ)/cmjasts.o	$(OBJ)/hostdrv.o	\
			$(OBJ)/hostdrvs.o	$(OBJ)/unasm.o		$(OBJ)/np2info.o	\
			\
			$(OBJ)/np2.o		$(OBJ)/np2arg.o		$(OBJ)/cputype.oa	\
			$(OBJ)/oemtext.o	$(OBJ)/dosio.o		$(OBJ)/extromio.o	\
			$(OBJ)/trace.o		$(OBJ)/winloc.o		$(OBJ)/commng.o		\
			$(OBJ)/cmserial.o	$(OBJ)/cmmidi.o		$(OBJ)/cmpara.o		\
			$(OBJ)/fontmng.o	$(OBJ)/joymng.o		$(OBJ)/mousemng.o	\
			$(OBJ)/scrnmng.o	$(OBJ)/soundmng.o	$(OBJ)/sysmng.o		\
			$(OBJ)/taskmng.o	$(OBJ)/timemng.o	$(OBJ)/juliet.o		\
			$(OBJ)/sstp.o		$(OBJ)/sstpmsg.o	$(OBJ)/winkbd.o		\
			$(OBJ)/dclock.o		$(OBJ)/dclockd.o	$(OBJ)/menu.o		\
			$(OBJ)/ini.o		$(OBJ)/dd2.o		$(OBJ)/toolwin.o	\
			$(OBJ)/subwind.o	$(OBJ)/mt32snd.o	$(OBJ)/win32sub.o	\
			$(OBJ)/np2res.o		\
			\
			$(OBJ)/np2class.o	$(OBJ)/d_about.o	$(OBJ)/d_config.o	\
			$(OBJ)/d_mpu98.o	$(OBJ)/d_screen.o	$(OBJ)/d_serial.o	\
			$(OBJ)/d_sound.o	$(OBJ)/d_clnd.o		$(OBJ)/d_disk.o		\
			$(OBJ)/d_bmp.o		$(OBJ)/dialogs.o						\
			\
			$(OBJ)/viewer.o		$(OBJ)/viewcmn.o	$(OBJ)/viewmenu.o	\
			$(OBJ)/viewmem.o	$(OBJ)/viewreg.o	$(OBJ)/viewseg.o	\
			$(OBJ)/view1mb.o	$(OBJ)/viewasm.o	$(OBJ)/viewsnd.o	\
			\
			$(OBJ)/pccore.o		$(OBJ)/nevent.o		$(OBJ)/calendar.o	\
			$(OBJ)/timing.o		$(OBJ)/keystat.o	$(OBJ)/statsave.o	\
			$(OBJ)/debugsub.o


NP2LIB	= -lddraw -ldxguid -ldsound -lwinmm -lcomctl32 -lwsock32 -mwindows
NP2BIN	= $(BIN)/np2.exe


all: $(OBJBASE) $(BINBASE) $(OBJ) $(BIN) $(NP2BIN)


$(OBJBASE):
	mkdir $(OBJBASE)

$(BINBASE):
	mkdir $(BINBASE)

$(OBJ):
	mkdir $(OBJ)

$(BIN):
	mkdir $(BIN)


$(NP2BIN): $(NP2OBJ)
	$(CC) $(NP2OBJ) $(LOPT) $(NP2LIB) -o $@



$(OBJ)/strres.o		:	$(COMMON)/strres.c $(COMMON)/strres.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/milstr.o		:	$(COMMON)/milstr.c $(HEADERS)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/_memory.o	:	$(COMMON)/_memory.c $(HEADERS) $(TARGET)/dosio.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/textfile.o	:	$(COMMON)/textfile.c $(HEADERS) $(TARGET)/dosio.h \
						$(COMMON)/textfile.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/profile.o	:	$(COMMON)/profile.c $(HEADERS) $(TARGET)/dosio.h \
						$(COMMON)/textfile.h $(COMMON)/profile.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/rect.o		:	$(COMMON)/rect.c $(HEADERS)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/lstarray.o	:	$(COMMON)/lstarray.c $(HEADERS)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/bmpdata.o	:	$(COMMON)/bmpdata.c $(HEADERS) $(COMMON)/bmpdata.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/mimpidef.o	:	$(COMMON)/mimpidef.c $(HEADERS) $(TARGET)/dosio.h \
						$(COMMON)/textfile.h $(COMMON)/mimpidef.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/parts.o		:	$(COMMON)/parts.c $(HEADERS)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/parts.oa		:	$(X86ASM)/parts.x86
	$(AS) $(AOPT) $< -o $@

$(OBJ)/arc.o		:	$(COMMON)/arc.c $(HEADERS) $(TARGET)/dosio.h \
						$(COMMON)/arc.h $(COMMON)/arcunzip.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/arcunzip.o	:	$(COMMON)/arcunzip.c $(HEADERS) $(TARGET)/dosio.h \
						$(COMMON)/arc.h $(COMMON)/arcunzip.h $(ZLIBS)
	$(CC) $(COPT) -o $@ $<



$(OBJ)/adler32.o	:	$(ZLIB)/adler32.c $(ZLIBS)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/crc32.o		:	$(ZLIB)/crc32.c $(ZLIBS)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/inffast.o	:	$(ZLIB)/inffast.c $(ZLIBS)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/inflate.o	:	$(ZLIB)/inflate.c $(ZLIBS)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/inftrees.o	:	$(ZLIB)/inftrees.c $(ZLIBS)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/zutil.o		:	$(ZLIB)/zutil.c $(ZLIBS)
	$(CC) $(COPT) -o $@ $<



$(OBJ)/utf8ucs2.o	:	$(CODECNV)/utf8ucs2.c $(HEADERS) $(CODECNV)/textcnv.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/ucs2utf8.o	:	$(CODECNV)/ucs2utf8.c $(HEADERS) $(CODECNV)/textcnv.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/tcswap16.o	:	$(CODECNV)/tcswap16.c $(HEADERS) $(CODECNV)/textcnv.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/i286c.o		:	$(I286C)/i286c.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(I286C)/i286c.h $(I286C)/v30patch.h \
						$(I286C)/i286c.mcr $(MEM)/dmax86.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/i286c_mn.o	:	$(I286C)/i286c_mn.c $(HEADERS) $(CPUCORES) \
						$(PCCORES) $(IOCORES) $(I286C)/i286c.h \
						$(I286C)/i286c.mcr $(BIOS)/bios.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/i286c_ea.o	:	$(I286C)/i286c_ea.c $(HEADERS) $(CPUCORES) \
						$(I286C)/i286c.h $(I286C)/i286c.mcr
	$(CC) $(COPT) -o $@ $<

$(OBJ)/i286c_0f.o	:	$(I286C)/i286c_0f.c $(HEADERS) $(CPUCORES) \
						$(I286C)/i286c.h $(I286C)/i286c.mcr
	$(CC) $(COPT) -o $@ $<

$(OBJ)/i286c_8x.o	:	$(I286C)/i286c_8x.c $(HEADERS) $(CPUCORES) \
						$(I286C)/i286c.h $(I286C)/i286c.mcr
	$(CC) $(COPT) -o $@ $<

$(OBJ)/i286c_sf.o	:	$(I286C)/i286c_sf.c $(HEADERS) $(CPUCORES) \
						$(I286C)/i286c.h $(I286C)/i286c.mcr \
						$(I286C)/i286c_sf.mcr
	$(CC) $(COPT) -o $@ $<

$(OBJ)/i286c_f6.o	:	$(I286C)/i286c_f6.c $(HEADERS) $(CPUCORES) \
						$(I286C)/i286c.mcr
	$(CC) $(COPT) -o $@ $<

$(OBJ)/i286c_fe.o	:	$(I286C)/i286c_fe.c $(HEADERS) $(CPUCORES) \
						$(I286C)/i286c.h $(I286C)/i286c.mcr
	$(CC) $(COPT) -o $@ $<

$(OBJ)/i286c_rp.o	:	$(I286C)/i286c_rp.c $(HEADERS) $(CPUCORES) \
						$(PCCORES) $(IOCORES) $(I286C)/i286c.h \
						$(I286C)/i286c.mcr
	$(CC) $(COPT) -o $@ $<

$(OBJ)/v30patch.o	:	$(I286C)/v30patch.c $(HEADERS) $(CPUCORES) \
						$(PCCORES) $(IOCORES) $(I286C)/i286c.h \
						$(I286C)/v30patch.h $(I286C)/i286c.mcr \
						$(MEM)/dmav30.h $(BIOS)/bios.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/memory.o		:	$(I286C)/memory.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(MEM)/memegc.h $(VRAM)/vram.h \
						$(FONT)/font.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/dmax86.o		:	$(MEM)/dmax86.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(MEM)/dmax86.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/dmav30.o		:	$(MEM)/dmav30.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(MEM)/dmav30.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/memtram.o	:	$(MEM)/memtram.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(MEM)/memtram.h $(VRAM)/vram.h \
						$(FONT)/font.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/memvram.o	:	$(MEM)/memvram.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(MEM)/memtram.h $(VRAM)/vram.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/memegc.o		:	$(MEM)/memegc.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(MEM)/memegc.h $(VRAM)/vram.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/memems.o		:	$(MEM)/memems.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(MEM)/memems.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/memepp.o		:	$(MEM)/memepp.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(MEM)/memepp.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/iocore.o		:	$(IO)/iocore.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(FMBOARDS) $(IO)/iocore16.tbl \
						$(SOUND)/sound.h $(CBUS)/cs4231io.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/artic.o		:	$(IO)/artic.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/cgrom.o		:	$(IO)/cgrom.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(FONT)/font.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/cpuio.o		:	$(IO)/cpuio.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/crtc.o		:	$(IO)/crtc.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(VRAM)/vram.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/dipsw.o		:	$(IO)/dipsw.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(TARGET)/sysmng.h \
						$(SOUND)/sound.h $(SOUND)/beep.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/dmac.o		:	$(IO)/dmac.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(SOUND)/sound.h $(SOUND)/cs4231.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/egc.o		:	$(IO)/egc.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(MEM)/memegc.h $(VRAM)/vram.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/emsio.o		:	$(IO)/emsio.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/epsonio.o	:	$(IO)/epsonio.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fdc.o		:	$(IO)/fdc.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FDD)/fddfile.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fdd320.o		:	$(IO)/fdd320.c $(HEADERS) $(PCCORES) $(IOCORES)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/gdc.o		:	$(IO)/gdc.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(TARGET)/scrnmng.h $(IO)/gdc_cmd.tbl \
						$(VRAM)/vram.h $(VRAM)/palettes.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/gdc_sub.o	:	$(IO)/gdc_sub.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(IO)/gdc_pset.h $(VRAM)/vram.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/gdc_pset.o	:	$(IO)/gdc_pset.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(IO)/gdc_pset.h $(VRAM)/vram.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/mouseif.o	:	$(IO)/mouseif.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(TARGET)/mousemng.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/necio.o		:	$(IO)/necio.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/nmiio.o		:	$(IO)/nmiio.c $(HEADERS) $(PCCORES) $(IOCORES)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/np2sysp.o	:	$(IO)/np2sysp.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(COMMON)/strres.h $(TARGET)/taskmng.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/pic.o		:	$(IO)/pic.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/pit.o		:	$(IO)/pit.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(SOUND)/sound.h $(SOUND)/beep.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/printif.o	:	$(IO)/printif.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(TARGET)/commng.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/serial.o		:	$(IO)/serial.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(TARGET)/commng.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/sysport.o	:	$(IO)/sysport.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(SOUND)/sound.h $(SOUND)/beep.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/upd4990.o	:	$(IO)/upd4990.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(TARGET)/timemng.h $(BASE)/calendar.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/cbuscore.o	:	$(CBUS)/cbuscore.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(CBUS)/cbuscore.h $(CBUS)/pc9861k.h $(CBUS)/mpu98ii.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/sasiio.o		:	$(CBUS)/sasiio.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(TARGET)/dosio.h $(CBUS)/cbuscore.h \
						$(CBUS)/sasiio.h $(FDD)/sxsi.h $(CBUS)/sasibios.res
	$(CC) $(COPT) -o $@ $<

$(OBJ)/scsiio.o		:	$(CBUS)/scsiio.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(TARGET)/dosio.h $(CBUS)/cbuscore.h \
						$(CBUS)/scsiio.h $(CBUS)/scsiio.tbl \
						$(CBUS)/scsicmd.h $(CBUS)/scsibios.res
	$(CC) $(COPT) -o $@ $<

$(OBJ)/scsicmd.o	:	$(CBUS)/scsicmd.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(TARGET)/dosio.h $(CBUS)/cbuscore.h \
						$(CBUS)/scsiio.h $(CBUS)/scsiio.tbl \
						$(CBUS)/scsicmd.h $(FDD)/sxsi.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/pc9861k.o	:	$(CBUS)/pc9861k.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(TARGET)/commng.h $(CBUS)/cbuscore.h \
						$(CBUS)/pc9861k.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/mpu98ii.o	:	$(CBUS)/mpu98ii.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(TARGET)/commng.h $(CBUS)/cbuscore.h \
						$(CBUS)/mpu98ii.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/board14.o	:	$(CBUS)/board14.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FMBOARDS) $(CBUS)/cbuscore.h $(CBUS)/board14.h \
						$(SOUND)/sound.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/board26k.o	:	$(CBUS)/board26k.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FMBOARDS) $(CBUS)/cbuscore.h $(CBUS)/board26k.h \
						$(SOUND)/sound.h $(SOUND)/s98.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/board86.o	:	$(CBUS)/board86.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FMBOARDS) $(CBUS)/cbuscore.h $(CBUS)/board86.h \
						$(CBUS)/pcm86io.h $(SOUND)/sound.h $(SOUND)/s98.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/boardx2.o	:	$(CBUS)/boardx2.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FMBOARDS) $(CBUS)/cbuscore.h $(CBUS)/boardx2.h \
						$(CBUS)/pcm86io.h $(SOUND)/sound.h $(SOUND)/s98.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/board118.o	:	$(CBUS)/board118.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FMBOARDS) $(CBUS)/cbuscore.h $(CBUS)/board118.h \
						$(CBUS)/cs4231io.h $(SOUND)/sound.h $(SOUND)/s98.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/boardspb.o	:	$(CBUS)/boardspb.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FMBOARDS) $(CBUS)/cbuscore.h $(CBUS)/boardspb.h \
						$(SOUND)/sound.h $(SOUND)/s98.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/amd98.o		:	$(CBUS)/amd98.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FMBOARDS) $(CBUS)/cbuscore.h $(CBUS)/amd98.h \
						$(SOUND)/sound.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/pcm86io.o	:	$(CBUS)/pcm86io.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(FMBOARDS) $(CBUS)/pcm86io.h \
						$(SOUND)/sound.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/cs4231io.o	:	$(CBUS)/cs4231io.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(FMBOARDS) $(CBUS)/cs4231io.h \
						$(SOUND)/sound.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/bios.o		:	$(BIOS)/bios.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(TARGET)/dosio.h $(BIOS)/bios.h \
						$(BIOS)/biosmem.h $(BIOS)/fdfmt.h \
						$(BIOS)/keytable.res $(BIOS)/itfrom.res \
						$(BIOS)/startup.res $(BIOS)/biosfd80.res \
						$(LIO)/lio.h $(FDD)/fddfile.h $(FDD)/fdd_mtr.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/bios09.o		:	$(BIOS)/bios09.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(BIOS)/bios.h $(BIOS)/biosmem.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/bios0c.o		:	$(BIOS)/bios0c.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(BIOS)/bios.h $(BIOS)/biosmem.h \
						$(BIOS)/rsbios.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/bios12.o		:	$(BIOS)/bios12.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(BIOS)/bios.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/bios13.o		:	$(BIOS)/bios13.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(BIOS)/bios.h $(BIOS)/biosmem.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/bios18.o		:	$(BIOS)/bios18.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(BIOS)/bios.h $(BIOS)/biosmem.h \
						$(FONT)/font.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/bios19.o		:	$(BIOS)/bios19.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(BIOS)/bios.h $(BIOS)/biosmem.h \
						$(BIOS)/rsbios.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/bios1a.o		:	$(BIOS)/bios1a.c $(HEADERS) $(CPUCORE) $(PCCORES) \
						$(IOCORES) $(BIOS)/bios.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/bios1b.o		:	$(BIOS)/bios1b.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(CBUS)/scsicmd.h $(BIOS)/bios.h \
						$(BIOS)/biosmem.h $(BIOS)/sxsibios.h \
						$(FDD)/fddfile.h $(FDD)/fdd_mtr.h $(FDD)/sxsi.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/bios1c.o		:	$(BIOS)/bios1c.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(TARGET)/timemng.h $(BASE)/calendar.h \
						$(BIOS)/bios.h $(BIOS)/biosmem.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/bios1f.o		:	$(BIOS)/bios1f.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(BIOS)/bios.h $(BIOS)/biosmem.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/sxsibios.o	:	$(BIOS)/sxsibios.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(BIOS)/bios.h $(BIOS)/biosmem.h $(BIOS)/sxsibios.h \
						$(CBUS)/scsicmd.h $(FDD)/sxsi.h $(BASE)/timing.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/lio.o		:	$(LIO)/lio.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(BIOS)/bios.h $(BIOS)/biosmem.h \
						$(VRAM)/vram.h $(LIO)/lio.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/gscreen.o	:	$(LIO)/gscreen.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(LIO)/lio.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/gpset.o		:	$(LIO)/gpset.c $(HEADERS) $(CPUCORES) $(LIO)/lio.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/gline.o		:	$(LIO)/gline.c $(HEADERS) $(CPUCORES) $(LIO)/lio.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/gcircle.o	:	$(LIO)/gcircle.c $(HEADERS) $(CPUCORES) $(LIO)/lio.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/gput1.o		:	$(LIO)/gput1.c $(HEADERS) $(CPUCORES) $(LIO)/lio.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/vram.o		:	$(VRAM)/vram.c $(HEADERS) $(CPUCORES) $(VRAM)/vram.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/scrndraw.o	:	$(VRAM)/scrndraw.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(TARGET)/scrnmng.h $(VRAM)/scrndraw.h \
						$(VRAM)/sdraw.h $(VRAM)/dispsync.h $(VRAM)/palettes.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/sdraw.o		:	$(VRAM)/sdraw.c $(HEADERS) $(TARGET)/scrnmng.h \
						$(VRAM)/scrndraw.h $(VRAM)/sdraw.h \
						$(VRAM)/palettes.h $(VRAM)/sdraw.mcr
	$(CC) $(COPT) -o $@ $<

$(OBJ)/sdrawq16.o	:	$(VRAM)/sdrawq16.c $(HEADERS) $(TARGET)/scrnmng.h \
						$(VRAM)/scrndraw.h $(VRAM)/sdraw.h \
						$(VRAM)/palettes.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/dispsync.o	:	$(VRAM)/dispsync.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(TARGET)/scrnmng.h $(VRAM)/scrndraw.h \
						$(VRAM)/dispsync.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/palettes.o	:	$(VRAM)/palettes.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(TARGET)/scrnmng.h $(VRAM)/scrndraw.h \
						$(VRAM)/palettes.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/maketext.o	:	$(VRAM)/maketext.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(VRAM)/vram.h $(VRAM)/scrndraw.h \
						$(VRAM)/dispsync.h $(VRAM)/maketext.h $(FONT)/font.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/maketgrp.o	:	$(VRAM)/maketgrp.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(VRAM)/vram.h $(VRAM)/scrndraw.h \
						$(VRAM)/dispsync.h $(VRAM)/palettes.h \
						$(VRAM)/maketext.h $(VRAM)/maketgrp.h \
						$(VRAM)/makegrph.h $(VRAM)/makegrph.mcr $(FONT)/font.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/makegrph.o	:	$(VRAM)/makegrph.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(VRAM)/vram.h $(VRAM)/scrndraw.h \
						$(VRAM)/dispsync.h $(VRAM)/makegrph.h \
						$(VRAM)/makegrph.mcr
	$(CC) $(COPT) -o $@ $<

$(OBJ)/makegrph.oa	:	$(X86ASM)/makegrph.x86 $(X86ASM)/pccore.inc \
						$(I286CASM)/cpucore.inc $(IOASM)/iocore.inc \
						$(X86ASM)/vram.inc
	$(AS) $(AOPT) -i$(X86ASM)/ -i$(I286CASM)/ -i$(IOASM)/ $< -o $@

$(OBJ)/scrnsave.o	:	$(VRAM)/scrnsave.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(COMMON)/bmpdata.h $(VRAM)/scrndraw.h \
						$(VRAM)/dispsync.h $(VRAM)/palettes.h \
						$(VRAM)/scrnsave.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/sound.o		:	$(SOUND)/sound.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(TARGET)/soundmng.h $(SOUND)/sound.h \
						$(SOUND)/sndcsec.h $(SOUND)/beep.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/sndcsec.o	:	$(SOUND)/sndcsec.c $(HEADERS) $(SOUND)/sound.h \
						$(SOUND)/sndcsec.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fmboard.o	:	$(SOUND)/fmboard.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FMBOARDS) $(TARGET)/joymng.h $(TARGET)/soundmng.h \
						$(CBUS)/cbuscore.h $(CBUS)/board14.h \
						$(CBUS)/board26k.h $(CBUS)/board86.h \
						$(CBUS)/boardx2.h $(CBUS)/board118.h \
						$(CBUS)/boardspb.h $(CBUS)/amd98.h $(CBUS)/pcm86io.h \
						$(CBUS)/cs4231io.h $(SOUND)/sound.h $(SOUND)/beep.h \
						$(GENERIC)/keydisp.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fmtimer.o	:	$(SOUND)/fmtimer.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(FMBOARDS) $(SOUND)/sound.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/beepc.o		:	$(SOUND)/beepc.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(FMBOARDS) $(SOUND)/sound.h \
						$(SOUND)/beep.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/beepg.o		:	$(SOUND)/beepg.c $(HEADERS) $(SOUND)/sound.h \
						$(SOUND)/beep.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/tms3631c.o	:	$(SOUND)/tms3631c.c $(HEADERS) $(SOUND)/sound.h \
						$(SOUND)/tms3631.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/tms3631g.o	:	$(SOUND)/tms3631g.c $(HEADERS) $(SOUND)/sound.h \
						$(SOUND)/tms3631.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/opngenc.o	:	$(SOUND)/opngenc.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FMBOARDS) $(SOUND)/sound.h $(GENERIC)/keydisp.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/opngeng.o	:	$(SOUND)/opngeng.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FMBOARDS) $(SOUND)/sound.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/opngeng.oa	:	$(X86ASM)/opngeng.x86
	$(AS) $(AOPT) $< -o $@

$(OBJ)/psggenc.o	:	$(SOUND)/psggenc.c $(HEADERS) $(SOUND)/sound.h \
						$(SOUND)/psggen.h $(GENERIC)/keydisp.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/psggeng.o	:	$(SOUND)/psggeng.c $(HEADERS) $(COMMON)/parts.h \
						$(SOUND)/sound.h $(SOUND)/psggen.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/rhythmc.o	:	$(SOUND)/rhythmc.c $(HEADERS) $(PCCORES) \
						$(COMMON)/wavefile.h $(TARGET)/dosio.h \
						$(SOUND)/sound.h $(SOUND)/rhythm.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/adpcmc.o		:	$(SOUND)/adpcmc.c $(HEADERS) $(SOUND)/sound.h \
						$(SOUND)/opngen.h $(SOUND)/adpcm.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/adpcmg.o		:	$(SOUND)/adpcmg.c $(HEADERS) $(SOUND)/sound.h \
						$(SOUND)/adpcm.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/pcm86c.o		:	$(SOUND)/pcm86c.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(FMBOARDS) $(SOUND)/sound.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/pcm86g.o		:	$(SOUND)/pcm86g.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FMBOARDS) $(SOUND)/sound.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/cs4231c.o	:	$(SOUND)/cs4231c.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FMBOARDS) $(SOUND)/sound.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/cs4231g.o	:	$(SOUND)/cs4231g.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(FMBOARDS) $(SOUND)/sound.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/soundrom.o	:	$(SOUND)/soundrom.c $(HEADERS) $(CPUCORES) \
						$(TARGET)/dosio.h $(SOUND)/soundrom.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/s98.o		:	$(SOUND)/s98.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(FMBOARDS) $(TARGET)/dosio.h $(SOUND)/sound.h \
						$(SOUND)/s98.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/getsnd.o		:	$(GETSND)/getsnd.c $(HEADERS) $(GETSND)/getsnd.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/getwave.o	:	$(GETSND)/getwave.c $(HEADERS) $(GETSND)/getsnd.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/getmp3.o		:	$(GETSND)/getmp3.c $(HEADERS) $(GETSND)/getsnd.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/getogg.o		:	$(GETSND)/getogg.c $(HEADERS) $(GETSND)/getsnd.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/getsmix.o	:	$(GETSND)/getsmix.c $(HEADERS) $(GETSND)/getsnd.h \
						$(GETSND)/getsndmn.mcr $(GETSND)/getsndst.mcr
	$(CC) $(COPT) -o $@ $<



$(OBJ)/midiout.o	:	$(VERMOUTH)/midiout.c $(HEADERS) $(VERMOUTHS)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/midimod.o	:	$(VERMOUTH)/midimod.c $(HEADERS) $(VERMOUTHS) \
						$(COMMON)/textfile.h $(TARGET)/dosio.h 
	$(CC) $(COPT) -o $@ $<

$(OBJ)/midinst.o	:	$(VERMOUTH)/midinst.c $(HEADERS) $(VERMOUTHS) \
						$(TARGET)/dosio.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/midvoice.o	:	$(VERMOUTH)/midvoice.c $(HEADERS) $(VERMOUTHS)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/midtable.o	:	$(VERMOUTH)/midtable.c $(HEADERS) $(VERMOUTHS)
	$(CC) $(COPT) -o $@ $<



$(OBJ)/diskdrv.o	:	$(FDD)/diskdrv.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(TARGET)/dosio.h $(TARGET)/sysmng.h \
						$(FDD)/diskdrv.h $(FDD)/fddfile.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/newdisk.o	:	$(FDD)/newdisk.c $(HEADERS) $(TARGET)/dosio.h \
						$(FDD)/newdisk.h $(FDD)/fddfile.h $(FDD)/sxsi.h \
						$(FDD)/hddboot.res
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fddfile.o	:	$(FDD)/fddfile.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(COMMON)/strres.h $(TARGET)/dosio.h \
						$(TARGET)/sysmng.h $(FDD)/fddfile.h \
						$(FDD)/fdd_xdf.h $(FDD)/fdd_d88.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fdd_xdf.o	:	$(FDD)/fdd_xdf.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(TARGET)/dosio.h $(FDD)/fddfile.h $(FDD)/fdd_xdf.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fdd_d88.o	:	$(FDD)/fdd_d88.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(TARGET)/dosio.h $(FDD)/fddfile.h $(FDD)/fdd_d88.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fdd_mtr.o	:	$(FDD)/fdd_mtr.c $(HEADERS) $(PCCORES) \
						$(TARGET)/soundmng.h $(FDD)/fdd_mtr.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/sxsi.o		:	$(FDD)/sxsi.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(COMMON)/strres.h $(TARGET)/dosio.h \
						$(TARGET)/sysmng.h $(FDD)/sxsi.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/sxsihdd.o	:	$(FDD)/sxsihdd.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(COMMON)/strres.h $(TARGET)/dosio.h \
						$(TARGET)/sysmng.h $(FDD)/sxsi.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/sxsicd.o		:	$(FDD)/sxsicd.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(COMMON)/strres.h $(TARGET)/dosio.h \
						$(TARGET)/sysmng.h $(FDD)/sxsi.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/font.o		:	$(FONT)/font.c $(HEADERS) $(CPUCORES) \
						$(COMMON)/strres.h $(TARGET)/dosio.h $(FONT)/font.h \
						$(FONT)/fontdata.h $(FONT)/fontmake.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fontdata.o	:	$(FONT)/fontdata.c $(HEADERS) $(CPUCORES) \
						$(FONT)/font.h $(FONT)/fontdata.h $(FONT)/fontdata.res
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fontpc88.o	:	$(FONT)/fontpc88.c $(HEADERS) $(CPUCORES) \
						$(TARGET)/dosio.h $(FONT)/font.h $(FONT)/fontdata.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fontpc98.o	:	$(FONT)/fontpc98.c $(HEADERS) $(CPUCORES) \
						$(COMMON)/bmpdata.h $(TARGET)/dosio.h \
						$(FONT)/font.h $(FONT)/fontdata.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fontv98.o	:	$(FONT)/fontv98.c $(HEADERS) $(CPUCORES) \
						$(TARGET)/dosio.h $(FONT)/font.h $(FONT)/fontdata.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fontfm7.o	:	$(FONT)/fontfm7.c $(HEADERS) $(CPUCORES) \
						$(TARGET)/dosio.h $(FONT)/font.h $(FONT)/fontdata.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fontx1.o		:	$(FONT)/fontx1.c $(HEADERS) $(CPUCORES) \
						$(COMMON)/parts.h $(TARGET)/dosio.h $(FONT)/font.h \
						$(FONT)/fontdata.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fontx68k.o	:	$(FONT)/fontx68k.c $(HEADERS) $(CPUCORES) \
						$(TARGET)/dosio.h $(FONT)/font.h $(FONT)/fontdata.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fontmake.o	:	$(FONT)/fontmake.c $(HEADERS) $(COMMON)/bmpdata.h \
						$(COMMON)/parts.h $(TARGET)/dosio.h \
						$(TARGET)/fontmng.h $(FONT)/font.h \
						$(FONT)/fontdata.h $(FONT)/fontmake.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/cmndraw.o	:	$(GENERIC)/cmndraw.c $(HEADERS) \
						$(TARGET)/scrnmng.h $(GENERIC)/cmndraw.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/keydisp.o	:	$(GENERIC)/keydisp.c $(HEADERS) $(PCCORES) \
						$(IOCORES) $(FMBOARDS) $(SOUND)/sound.h \
						$(GENERIC)/keydisp.h $(GENERIC)/keydisp.res
	$(CC) $(COPT) -o $@ $<

$(OBJ)/softkbd.o	:	$(GENERIC)/softkbd.c $(HEADERS) \
						$(COMMON)/bmpdata.h $(BASE)/keystat.h \
						$(GENERIC)/softkbd.h $(GENERIC)/softkbd.res
	$(CC) $(COPT) -o $@ $<

$(OBJ)/dipswbmp.o	:	$(GENERIC)/dipswbmp.c $(HEADERS) $(COMMON)/bmpdata.h \
						$(GENERIC)/dipswbmp.h $(GENERIC)/dipswbmp.res
	$(CC) $(COPT) -o $@ $<

$(OBJ)/cmver.o		:	$(GENERIC)/cmver.c $(HEADERS) $(TARGET)/commng.h \
						$(SOUND)/sound.h $(VERMOUTH)/vermouth.h \
						$(GENERIC)/keydisp.h $(GENERIC)/cmver.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/cmjasts.o	:	$(GENERIC)/cmjasts.c $(HEADERS) $(CPUCORES) \
						$(TARGET)/commng.h $(SOUND)/sound.h \
						$(GENERIC)/cmjasts.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/hostdrv.o	:	$(GENERIC)/hostdrv.c $(HEADERS) $(CPUCORES) \
						$(PCCORES) $(IOCORES) $(GENERIC)/hostdrv.h \
						$(GENERIC)/hostdrv.tbl $(GENERIC)/hostdrvs.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/hostdrvs.o	:	$(GENERIC)/hostdrvs.c $(HEADERS) $(PCCORES) \
						$(TARGET)/dosio.h $(GENERIC)/hostdrv.h \
						$(GENERIC)/hostdrvs.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/unasm.o		:	$(GENERIC)/unasm.c $(HEADER) $(GENERIC)/unasm.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/np2info.o	:	$(GENERIC)/np2info.c $(HEADERS) $(CPUCORES) \
						$(PCCORES) $(IOCORES) $(FMBOARDS) $(COMMON)/strres.h \
						$(TARGET)/scrnmng.h $(BIOS)/bios.h \
						$(GENERIC)/np2info.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/np2.o		:	$(TARGET)/np2.cpp $(HEADERS) $(PCCORES) $(IOCORES) \
						$(COMMON)/strres.h $(COMMON)/parts.h \
						$(TARGET)/resource.h $(TARGET)/np2.h \
						$(TARGET)/np2arg.h $(X86ASM)/cputype.h \
						$(TARGET)/dosio.h $(TARGET)/extromio.h \
						$(TARGET)/commng.h $(TARGET)/joymng.h \
						$(TARGET)/mousemng.h $(TARGET)/scrnmng.h \
						$(TARGET)/soundmng.h $(TARGET)/sysmng.h \
						$(TARGET)/winkbd.h $(TARGET)/ini.h $(TARGET)/menu.h \
						$(TARGET)/winloc.h $(TARGET)/sstp.h \
						$(TARGET)/sstpmsg.h $(TARGET)/dclock.h \
						$(TARGET)/subwind.h \
						$(TARGET)/juliet.h $(DIALOG)/dialog.h \
						$(I286C)/memory.h $(CBUS)/pc9861k.h \
						$(CBUS)/mpu98ii.h $(BIOS)/bios.h $(VRAM)/scrndraw.h \
						$(SOUND)/sound.h $(SOUND)/beep.h $(SOUND)/s98.h \
						$(FDD)/diskdrv.h $(FDD)/fddfile.h $(BASE)/timing.h \
						$(BASE)/debugsub.h $(DEBUGUTY)/viewer.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/np2arg.o		:	$(TARGET)/np2arg.cpp $(HEADERS) $(TARGET)/np2arg.h \
						$(TARGET)/dosio.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/cputype.oa	:	$(X86ASM)/cputype.x86
	$(AS) $(AOPT) $< -o $@

$(OBJ)/oemtext.o	:	$(TARGET)/oemtext.cpp $(HEADERS) $(TARGET)/oemtext.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/dosio.o		:	$(TARGET)/dosio.cpp $(HEADERS) $(TARGET)/dosio.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/extromio.o	:	$(TARGET)/extromio.cpp $(HEADERS) $(TARGET)/dosio.h \
						$(TARGET)/extromio.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/trace.o		:	$(TARGET)/trace.cpp $(HEADERS) $(COMMON)/strres.h \
						$(TARGET)/dosio.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/winloc.o		:	$(TARGET)/winloc.cpp
	$(CC) $(COPT) -o $@ $<

$(OBJ)/commng.o		:	$(TARGET)/commng.cpp $(HEADERS) $(TARGET)/np2.h \
						$(TARGET)/commng.h $(TARGET)/cmserial.h \
						$(TARGET)/cmmidi.h $(TARGET)/cmpara.h \
						$(TARGET)/commng.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/cmserial.o	:	$(TARGET)/cmserial.cpp $(HEADERS) $(TARGET)/commng.h \
						$(TARGET)/cmserial.h $(TARGET)/cmmidi.h \
						$(TARGET)/cmpara.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/cmmidi.o		:	$(TARGET)/cmmidi.cpp $(HEADERS) $(COMMON)/mimpidef.h \
						$(TARGET)/commng.h $(TARGET)/cmserial.h \
						$(TARGET)/cmmidi.h $(TARGET)/cmpara.h \
						$(SOUND)/sound.h $(VERMOUTH)/vermouth.h \
						$(GENERIC)/keydisp.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/cmpara.o		:	$(TARGET)/cmpara.cpp
	$(CC) $(COPT) -o $@ $<

$(OBJ)/fontmng.o	:	$(TARGET)/fontmng.cpp $(HEADERS) $(TARGET)/fontmng.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/joymng.o		:	$(TARGET)/joymng.cpp $(HEADERS) $(TARGET)/np2.h \
						$(TARGET)/joymng.h $(TARGET)/menu.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/mousemng.o	:	$(TARGET)/mousemng.cpp $(HEADERS) $(TARGET)/np2.h \
						$(TARGET)/mousemng.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/scrnmng.o	:	$(TARGET)/scrnmng.cpp $(HEADERS) \
						$(TARGET)/resource.h $(TARGET)/np2.h \
						$(TARGET)/mousemng.h $(TARGET)/scrnmng.h \
						$(TARGET)/sysmng.h $(TARGET)/dclock.h \
						$(TARGET)/menu.h $(VRAM)/scrndraw.h $(VRAM)/palettes.h
	$(CC) $(COPT) -I$(DXSDKINC) -o $@ $<

$(OBJ)/soundmng.o	:	$(TARGET)/soundmng.cpp $(HEADERS) $(COMMON)/parts.h \
						$(COMMON)/wavefile.h $(TARGET)/np2.h \
						$(TARGET)/soundmng.h $(TARGET)/extromio.h \
						$(SOUND)/sound.h $(VERMOUTH)/vermouth.h
	$(CC) $(COPT) -I$(DXSDKINC) -o $@ $<

$(OBJ)/sysmng.o		:	$(TARGET)/sysmng.cpp $(HEADERS) $(PCCORES) \
						$(TARGET)/np2.h $(TARGET)/dosio.h $(TARGET)/sysmng.h \
						$(I286C)/cpucore.h $(FDD)/fddfile.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/taskmng.o	:	$(TARGET)/taskmng.cpp $(HEADERS) $(TARGET)/taskmng.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/timemng.o	:	$(TARGET)/timemng.cpp $(HEADERS) $(TARGET)/timemng.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/juliet.o		:	$(TARGET)/juliet.cpp $(HEADERS) $(TARGET)/romeo.h \
						$(TARGET)/juliet.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/sstp.o		:	$(TARGET)/sstp.cpp $(HEADERS) $(TARGET)/np2.h \
						$(TARGET)/scrnmng.h $(TARGET)/sstp.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/sstpmsg.o	:	$(TARGET)/sstpmsg.cpp $(HEADERS) $(PCCORES) \
						$(TARGET)/np2.h $(TARGET)/sysmng.h $(TARGET)/sstp.h \
						$(TARGET)/sstpres.h $(GENERIC)/np2info.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/winkbd.o		:	$(TARGET)/winkbd.cpp $(HEADERS) $(PCCORES) \
						$(IOCORES) $(TARGET)/np2.h $(TARGET)/winkbd.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/dclock.o		:	$(TARGET)/dclock.cpp $(HEADERS) $(COMMON)/parts.h \
						$(TARGET)/np2.h $(TARGET)/scrnmng.h \
						$(TARGET)/timemng.h $(TARGET)/dclock.h \
						$(VRAM)/scrndraw.h $(VRAM)/palettes.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/dclockd.o	:	$(TARGET)/dclockd.x86
	$(AS) $(AOPT) $< -o $@

$(OBJ)/menu.o		:	$(TARGET)/menu.cpp $(HEADERS) $(PCCORES) \
						$(TARGET)/resource.h $(TARGET)/np2.h \
						$(TARGET)/sysmng.h $(TARGET)/menu.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/ini.o		:	$(TARGET)/ini.cpp $(HEADERS) $(PCCORES) \
						$(TARGET)/np2.h $(TARGET)/np2arg.h $(TARGET)/dosio.h \
						$(TARGET)/ini.h $(TARGET)/winkbd.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/dd2.o		:	$(TARGET)/dd2.cpp $(HEADERS) $(TARGET)/dd2.h
	$(CC) $(COPT) -I$(DXSDKINC) -o $@ $<

$(OBJ)/toolwin.o	:	$(TARGET)/toolwin.cpp $(HEADERS) $(PCCORES) \
						$(COMMON)/strres.h $(TARGET)/resource.h \
						$(TARGET)/np2.h $(TARGET)/dosio.h $(TARGET)/sysmng.h \
						$(TARGET)/toolwin.h $(TARGET)/ini.h \
						$(DIALOG)/dialog.h $(FDD)/diskdrv.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/subwind.o	:	$(TARGET)/subwind.cpp $(HEADERS) \
						$(TARGET)/resource.h $(TARGET)/np2.h \
						$(TARGET)/winloc.h $(TARGET)/soundmng.h \
						$(TARGET)/sysmng.h $(TARGET)/menu.h $(TARGET)/ini.h \
						$(TARGET)/dd2.h $(TARGET)/subwind.h \
						$(DIALOG)/np2class.h $(GENERIC)/keydisp.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/mt32snd.o	:	$(TARGET)/mt32snd.cpp $(HEADERS) \
						$(TARGET)/mt32snd.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/win32sub.o	:	$(TARGET)/win32sub.cpp $(HEADERS) \
						$(TARGET)/win32sub.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/np2res.o		:	$(RESDIR)/np2.rc $(TARGET)/resource.h
	$(RC) -I$(RESDIR) -i $< -o $@



$(OBJ)/np2class.o	:	$(DIALOG)/np2class.cpp $(HEADERS) $(TARGET)/resource.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/d_about.o	:	$(DIALOG)/d_about.cpp $(HEADERS) $(PCCORES) \
						$(TARGET)/resource.h $(TARGET)/np2.h \
						$(DIALOG)/dialog.h $(DIALOG)/dialogs.h \
						$(GENERIC)/np2info.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/d_config.o	:	$(DIALOG)/d_config.cpp $(HEADERS) $(PCCORES) \
						$(COMMON)/strres.h $(TARGET)/resource.h \
						$(TARGET)/np2.h $(TARGET)/dosio.h $(TARGET)/sysmng.h \
						$(DIALOG)/dialog.h $(DIALOG)/dialogs.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/d_mpu98.o	:	$(DIALOG)/d_mpu98.cpp $(HEADERS) $(PCCORES) \
						$(COMMON)/strres.h $(TARGET)/resource.h \
						$(TARGET)/np2.h $(TARGET)/commng.h \
						$(TARGET)/sysmng.h $(DIALOG)/dialog.h \
						$(DIALOG)/dialogs.h $(GENERIC)/dipswbmp.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/d_screen.o	:	$(DIALOG)/d_screen.cpp $(HEADERS) $(PCCORES) \
						$(IOCORES) $(COMMON)/strres.h $(TARGET)/resource.h \
						$(TARGET)/np2.h $(TARGET)/scrnmng.h \
						$(TARGET)/sysmng.h $(DIALOG)/dialog.h \
						$(DIALOG)/dialogs.h $(VRAM)/scrndraw.h \
						$(VRAM)/palettes.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/d_serial.o	:	$(DIALOG)/d_serial.cpp $(HEADERS) $(PCCORES) \
						$(IOCORES) $(COMMON)/strres.h $(TARGET)/resource.h \
						$(TARGET)/np2.h $(TARGET)/dosio.h $(TARGET)/commng.h \
						$(TARGET)/sysmng.h $(DIALOG)/dialog.h \
						$(DIALOG)/dialogs.h $(VRAM)/scrndraw.h \
						$(VRAM)/palettes.h $(GENERIC)/dipswbmp.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/d_sound.o	:	$(DIALOG)/d_sound.cpp $(HEADERS) $(PCCORES) \
						$(IOCORES) $(FMBOARDS) $(COMMON)/strres.h \
						$(TARGET)/resource.h $(TARGET)/np2.h \
						$(TARGET)/joymng.h $(TARGET)/sysmng.h \
						$(TARGET)/menu.h $(DIALOG)/dialog.h \
						$(DIALOG)/dialogs.h $(SOUND)/sound.h $(SOUND)/s98.h \
						$(GENERIC)/dipswbmp.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/d_clnd.o		:	$(DIALOG)/d_clnd.cpp $(HEADERS) $(PCCORES) \
						$(COMMON)/strres.h $(TARGET)/resource.h \
						$(TARGET)/np2.h $(TARGET)/sysmng.h \
						$(TARGET)/timemng.h \
						$(DIALOG)/dialog.h $(DIALOG)/dialogs.h \
						$(BASE)/calendar.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/d_disk.o		:	$(DIALOG)/d_disk.cpp $(HEADERS) $(PCCORES) \
						$(COMMON)/strres.h $(TARGET)/resource.h \
						$(TARGET)/np2.h $(TARGET)/dosio.h \
						$(DIALOG)/dialog.h $(DIALOG)/dialogs.h \
						$(FDD)/diskdrv.h $(FDD)/fddfile.h $(FDD)/newdisk.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/d_bmp.o		:	$(DIALOG)/d_bmp.cpp $(HEADERS) $(PCCORES) $(IOCORES) \
						$(COMMON)/strres.h $(TARGET)/resource.h \
						$(TARGET)/np2.h $(TARGET)/dosio.h \
						$(TARGET)/sysmng.h $(DIALOG)/dialog.h \
						$(DIALOG)/dialogs.h $(VRAM)/scrnsave.h $(FONT)/font.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/dialogs.o	:	$(DIALOG)/dialogs.cpp $(HEADERS) $(COMMON)/strres.h \
						$(TARGET)/dosio.h $(TARGET)/commng.h \
						$(TARGET)/sysmng.h $(DIALOG)/dialogs.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/viewer.o		:	$(DEBUGUTY)/viewer.cpp $(HEADERS) \
						$(TARGET)/resource.h $(TARGET)/np2.h \
						$(DEBUGUTY)/viewer.h $(DEBUGUTY)/viewcmn.h \
						$(I286C)/cpucore.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/viewcmn.o	:	$(DEBUGUTY)/viewcmn.cpp $(HEADERS) $(PCCORES) \
						$(IOCORES) $(TARGET)/resource.h $(DEBUGUTY)/viewer.h \
						$(DEBUGUTY)/viewcmn.h $(DEBUGUTY)/viewmenu.h \
						$(DEBUGUTY)/viewmem.h $(DEBUGUTY)/viewreg.h \
						$(DEBUGUTY)/viewseg.h $(DEBUGUTY)/view1mb.h \
						$(DEBUGUTY)/viewasm.h $(DEBUGUTY)/viewsnd.h \
						$(I286C)/cpucore.h $(I286C)/memory.h $(BIOS)/bios.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/viewmenu.o	:	$(DEBUGUTY)/viewmenu.cpp $(HEADERS) \
						$(TARGET)/resource.h $(DEBUGUTY)/viewer.h \
						$(DEBUGUTY)/viewcmn.h $(DEBUGUTY)/viewmenu.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/viewmem.o	:	$(DEBUGUTY)/viewmem.cpp $(HEADERS) \
						$(TARGET)/resource.h $(TARGET)/np2.h \
						$(DEBUGUTY)/viewer.h $(DEBUGUTY)/viewmem.h \
						$(I286C)/memory.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/viewreg.o	:	$(DEBUGUTY)/viewreg.cpp $(HEADERS) \
						$(TARGET)/resource.h $(TARGET)/np2.h \
						$(DEBUGUTY)/viewer.h $(DEBUGUTY)/viewcmn.h \
						$(DEBUGUTY)/viewmenu.h $(DEBUGUTY)/viewmem.h \
						$(DEBUGUTY)/viewreg.h $(I286C)/cpucore.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/viewseg.o	:	$(DEBUGUTY)/viewseg.cpp $(HEADERS) \
						$(TARGET)/resource.h $(TARGET)/np2.h \
						$(DEBUGUTY)/viewer.h $(DEBUGUTY)/viewcmn.h \
						$(DEBUGUTY)/viewmenu.h $(DEBUGUTY)/viewmem.h \
						$(DEBUGUTY)/viewseg.h $(I286C)/cpucore.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/view1mb.o	:	$(DEBUGUTY)/view1mb.cpp $(HEADERS) \
						$(TARGET)/resource.h $(TARGET)/np2.h \
						$(DEBUGUTY)/viewer.h $(DEBUGUTY)/viewcmn.h \
						$(DEBUGUTY)/viewmenu.h $(DEBUGUTY)/viewmem.h \
						$(DEBUGUTY)/view1mb.h $(I286C)/cpucore.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/viewasm.o	:	$(DEBUGUTY)/viewasm.cpp $(HEADERS) \
						$(TARGET)/resource.h $(TARGET)/np2.h \
						$(DEBUGUTY)/viewer.h $(DEBUGUTY)/viewcmn.h \
						$(DEBUGUTY)/viewmenu.h $(DEBUGUTY)/viewmem.h \
						$(DEBUGUTY)/viewsnd.h $(I286C)/cpucore.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/viewsnd.o	:	$(DEBUGUTY)/viewsnd.cpp $(HEADERS) $(PCCORES) \
						$(IOCORES) $(FMBOARDS) $(COMMON)/strres.h \
						$(TARGET)/resource.h $(TARGET)/np2.h \
						$(DEBUGUTY)/viewer.h $(DEBUGUTY)/viewcmn.h \
						$(DEBUGUTY)/viewmenu.h $(DEBUGUTY)/viewsnd.h \
						$(SOUND)/sound.h
	$(CC) $(COPT) -o $@ $<



$(OBJ)/pccore.o		:	$(BASE)/pccore.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(FMBOARDS) $(COMMON)/strres.h \
						$(TARGET)/dosio.h $(TARGET)/soundmng.h \
						$(TARGET)/sysmng.h $(TARGET)/timemng.h \
						$(CBUS)/cbuscore.h $(CBUS)/pc9861k.h \
						$(CBUS)/mpu98ii.h $(BIOS)/bios.h $(BIOS)/biosmem.h \
						$(VRAM)/vram.h $(VRAM)/scrndraw.h $(VRAM)/dispsync.h \
						$(VRAM)/palettes.h $(VRAM)/maketext.h \
						$(VRAM)/maketgrp.h $(VRAM)/makegrph.h \
						$(SOUND)/sound.h $(SOUND)/beep.h $(SOUND)/s98.h \
						$(FONT)/font.h $(FDD)/diskdrv.h $(FDD)/fddfile.h \
						$(FDD)/fdd_mtr.h $(FDD)/sxsi.h $(GENERIC)/hostdrv.h \
						$(BASE)/np2ver.h $(BASE)/calendar.h $(BASE)/timing.h \
						$(BASE)/debugsub.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/nevent.o		:	$(BASE)/nevent.c $(HEADERS) $(CPUCORES) $(PCCORES)
	$(CC) $(COPT) -o $@ $<

$(OBJ)/calendar.o	:	$(BASE)/calendar.c $(HEADERS) $(PCCORES) \
						$(COMMON)/parts.h $(TARGET)/timemng.h \
						$(BASE)/calendar.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/timing.o		:	$(BASE)/timing.c $(HEADERS) $(PCCORES) \
						$(FDD)/diskdrv.h $(FDD)/fdd_mtr.h $(BASE)/timing.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/keystat.o	:	$(BASE)/keystat.c $(HEADERS) $(PCCORES) $(IOCORES) \
						$(BASE)/keystat.h
	$(CC) $(COPT) -o $@ $<

$(OBJ)/statsave.o	:	$(BASE)/statsave.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(FMBOARDS) $(COMMON)/strres.h \
						$(TARGET)/dosio.h $(TARGET)/commng.h \
						$(TARGET)/scrnmng.h $(TARGET)/soundmng.h \
						$(TARGET)/timemng.h $(CBUS)/cbuscore.h \
						$(CBUS)/sasiio.h $(CBUS)/scsiio.h $(CBUS)/pc9861k.h \
						$(CBUS)/mpu98ii.h $(CBUS)/board14.h $(CBUS)/amd98.h \
						$(BIOS)/bios.h $(VRAM)/vram.h $(VRAM)/palettes.h \
						$(VRAM)/maketext.h $(SOUND)/sound.h $(SOUND)/beep.h \
						$(FONT)/font.h $(FDD)/fddfile.h $(FDD)/fdd_mtr.h \
						$(FDD)/sxsi.h $(GENERIC)/keydisp.h \
						$(GENERIC)/hostdrv.h $(BASE)/calendar.h \
						$(BASE)/statsave.tbl
	$(CC) $(COPT) -o $@ $<

$(OBJ)/debugsub.o	:	$(BASE)/debugsub.c $(HEADERS) $(CPUCORES) $(PCCORES) \
						$(IOCORES) $(COMMON)/strres.h $(TARGET)/dosio.h \
						$(BASE)/debugsub.h
	$(CC) $(COPT) -o $@ $<



clean:
	$(RM) $(NP2OBJ)
	$(RM) $(NP2BIN)

