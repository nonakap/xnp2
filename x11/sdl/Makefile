PROG?= xnp2

SDL_CONFIG?= sdl-config

CINCLUDES=	-I. -I.. -I../.. -I../../common -I../../codecnv \
		-I../../generic -I../debug \
		-I../../i286c \
		-I../../bios -I../../cbus -I../../fdd -I../../font \
		-I../../io -I../../lio -I../../mem \
		-I../../sound -I../../sound/vermouth -I../../sound/getsnd \
		-I../../vram \
		-I../../embed -I../../embed/menu -I../../embed/menubase \
		`$(SDL_CONFIG) --cflags` \
		-DUSE_SDL=1
COPTS?= 	-O2 -W -Wall -Wpointer-arith -DUSE_SDLAUDIO
SYSRESPATH?=	/usr/X11R6/lib/X11/xnp2

CFLAGS=		$(COPTS) $(CINCLUDES) -fsigned-char \
		-DSYSRESPATH=\"$(SYSRESPATH)\"
LDFLAGS=

LIBS=		`$(SDL_CONFIG) --libs` -lSDL_ttf

CPUOBJS=	../../i286c/i286c.o ../../i286c/i286c_mn.o \
		../../i286c/i286c_ea.o ../../i286c/i286c_0f.o \
		../../i286c/i286c_8x.o ../../i286c/i286c_sf.o \
		../../i286c/i286c_f6.o ../../i286c/i286c_fe.o \
		../../i286c/i286c_rp.o \
		../../i286c/memory.o \
		../../i286c/v30patch.o

CODECNVOBJS=	../../codecnv/eucsjis.o ../../codecnv/eucucs2.o \
		../../codecnv/sjiseuc.o ../../codecnv/sjisucs2.o \
		../../codecnv/ucs2utf8.o ../../codecnv/utf8ucs2.o

COMMONOBJS=	../../common/strres.o ../../common/milstr.o \
		../../common/_memory.o ../../common/textfile.o \
		../../common/profile.o ../../common/rect.o \
		../../common/lstarray.o ../../common/parts.o \
		../../common/bmpdata.o \
		../../common/mimpidef.o ../../common/wavefile.o \
		../../common/resize.o

BIOSOBJS=	../../bios/bios.o \
		../../bios/bios09.o ../../bios/bios0c.o ../../bios/bios12.o \
		../../bios/bios13.o ../../bios/bios18.o ../../bios/bios19.o \
		../../bios/bios1a.o ../../bios/bios1b.o ../../bios/bios1c.o \
                ../../bios/bios1f.o ../../bios/sxsibios.o

CBUSOBJS=	../../cbus/cbuscore.o ../../cbus/pc9861k.o \
		../../cbus/mpu98ii.o ../../cbus/amd98.o \
		../../cbus/board118.o ../../cbus/board14.o \
		../../cbus/board26k.o ../../cbus/board86.o \
		../../cbus/boardspb.o ../../cbus/boardx2.o \
		../../cbus/cs4231io.o ../../cbus/pcm86io.o \
		../../cbus/sasiio.o ../../cbus/scsicmd.o ../../cbus/scsiio.o \
		../../cbus/ideio.o ../../cbus/atapicmd.o

EMBEDOBJS=	../../embed/vramhdl.o ../../embed/vrammix.o

MENUOBJS=	../../embed/menu/dlgabout.o \
		../../embed/menu/dlgcfg.o \
		../../embed/menu/dlgscr.o \
		../../embed/menu/filesel.o \
		../../embed/menu/menustr.o

MENUBASEOBJS=	../../embed/menubase/menubase.o \
		../../embed/menubase/menudlg.o \
		../../embed/menubase/menuicon.o \
		../../embed/menubase/menumbox.o \
		../../embed/menubase/menures.o \
		../../embed/menubase/menusys.o \
		../../embed/menubase/menuvram.o

FDDOBJS=	../../fdd/diskdrv.o ../../fdd/newdisk.o ../../fdd/fddfile.o \
		../../fdd/fdd_xdf.o ../../fdd/fdd_d88.o ../../fdd/fdd_mtr.o \
		../../fdd/sxsi.o ../../fdd/sxsihdd.o ../../fdd/sxsicd.o

FONTOBJS=	../../font/font.o ../../font/fontdata.o ../../font/fontmake.o \
		../../font/fontpc88.o ../../font/fontpc98.o \
		../../font/fontv98.o ../../font/fontfm7.o ../../font/fontx1.o \
		../../font/fontx68k.o \

GENERICOBJS=	../../generic/cmjasts.o \
		../../generic/hostdrv.o ../../generic/hostdrvs.o

IOOBJS=		../../io/iocore.o ../../io/artic.o ../../io/cgrom.o \
		../../io/cpuio.o ../../io/crtc.o ../../io/dipsw.o \
		../../io/dmac.o ../../io/egc.o ../../io/emsio.o \
		../../io/fdc.o ../../io/fdd320.o ../../io/gdc.o \
		../../io/gdc_sub.o ../../io/gdc_pset.o \
		../../io/mouseif.o ../../io/nmiio.o ../../io/np2sysp.o \
		../../io/pic.o ../../io/pit.o ../../io/printif.o \
		../../io/serial.o ../../io/sysport.o ../../io/upd4990.o \
		../../io/necio.o ../../io/epsonio.o

LIOOBJS=	../../lio/lio.o ../../lio/gscreen.o ../../lio/gcircle.o \
		../../lio/gpset.o ../../lio/gline.o ../../lio/gput1.o

MEMOBJS=	../../mem/dmav30.o ../../mem/dmax86.o ../../mem/memegc.o \
		../../mem/memems.o ../../mem/memepp.o ../../mem/memtram.o \
		../../mem/memvga.o ../../mem/memvram.o

NP2OBJS=	../../pccore.o ../../nevent.o ../../calendar.o ../../timing.o \
		../../debugsub.o ../../statsave.o ../../keystat.o

SOUNDOBJS=	../../sound/sound.o ../../sound/soundrom.o ../../sound/s98.o \
		../../sound/adpcmc.o ../../sound/adpcmg.o \
		../../sound/beepc.o ../../sound/beepg.o \
		../../sound/cs4231c.o ../../sound/cs4231g.o \
		../../sound/fmboard.o ../../sound/fmtimer.o \
		../../sound/opngenc.o ../../sound/opngeng.o \
		../../sound/pcm86c.o ../../sound/pcm86g.o \
		../../sound/psggenc.o ../../sound/psggeng.o \
		../../sound/rhythmc.o ../../sound/sndcsec.o \
		../../sound/tms3631c.o ../../sound/tms3631g.o

GETSNDOBJS=	../../sound/getsnd/getsnd.o ../../sound/getsnd/getsmix.o \
		../../sound/getsnd/getmp3.o ../../sound/getsnd/getogg.o \
		../../sound/getsnd/getwave.o

VERMOUTHOBJS=	../../sound/vermouth/midiout.o ../../sound/vermouth/midimod.o \
		../../sound/vermouth/midinst.o ../../sound/vermouth/midvoice.o \
		../../sound/vermouth/midtable.o

VRAMOBJS=	../../vram/vram.o ../../vram/scrndraw.o \
		../../vram/sdraw.o ../../vram/sdrawq16.o \
		../../vram/dispsync.o ../../vram/palettes.o \
		../../vram/maketext.o ../../vram/maketgrp.o \
		../../vram/makegrph.o ../../vram/scrnbmp.o

SNDDRVOBJS=	../snddrv/esd.o ../snddrv/netbsd.o ../snddrv/oss.o \
		../snddrv/sdl.o

JOYDRVOBJS=	../joydrv/sdl.o

X11OBJS=	../dosio.o ../ini.o ../trace.o ../cmmidi.o ../cmserial.o \
		../np2.o ../sysmenu.o ../toolkit.o \
		../drawmng.o ../inputmng.o ../kbdmng.o \
		../commng.o ../joymng.o ../soundmng.o ../sysmng.o ../taskmng.o \
		../timemng.o \
		../main.o

X11SDLOBJS=	sdl_font.o sdl_keyboard.o sdl_mouse.o sdl_screen.o \
		sdl_main.o

OBJS=		$(CPUOBJS) $(COMMONOBJS) $(CODECNVOBJS) \
		$(BIOSOBJS) $(CBUSOBJS) $(FDDOBJS) $(MEMOBJS) \
		$(FONTOBJS) $(IOOBJS) $(LIOOBJS) $(NP2OBJS) \
		$(SOUNDOBJS) $(GETSNDOBJS) \
		$(VERMOUTHOBJS) $(VRAMOBJS) \
		$(EMBEDOBJS) $(MENUOBJS) $(MENUBASEOBJS) \
		$(GENERICOBJS) \
		$(JOYDRVOBJS) $(SNDDRVOBJS) \
		$(X11OBJS) $(X11SDLOBJS)
SRCS=		$(OBJS:.o=.c)

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

.c.o:
	$(CC) -o $@ $(CFLAGS) -c $<

clean:
	-/bin/rm -f $(PROG) $(OBJS) *~ *.bak core.* *.core
