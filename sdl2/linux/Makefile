# makefile for arm-linux
#   gcc version 4

CC		=	gcc
STRIP	=	strip
RM		=	rm -f

NP2_PATH	= ../..

CFLAGS	=	-Wall -O3 -Wstrict-overflow=0 -Wstrict-aliasing=0 -I. -I.. \
			-I$(NP2_PATH) \
			-I$(NP2_PATH)/cbus \
			-I$(NP2_PATH)/common \
			-I$(NP2_PATH)/embed \
			-I$(NP2_PATH)/embed/menu \
			-I$(NP2_PATH)/embed/menubase \
			-I$(NP2_PATH)/generic \
			-I$(NP2_PATH)/i286c \
			-I$(NP2_PATH)/io \
			-I$(NP2_PATH)/mem \
			-I$(NP2_PATH)/sound \
			-I$(NP2_PATH)/vram

CFLAGS	+=	$(shell sdl2-config --cflags)
LFLAGS	=

TARGET	=	np2

CPPSRCS	=	./main.c \
			$(NP2_PATH)/calendar.c \
			$(NP2_PATH)/debugsub.c \
			$(NP2_PATH)/keystat.c \
			$(NP2_PATH)/nevent.c \
			$(NP2_PATH)/pccore.c \
			$(NP2_PATH)/statsave.c \
			$(NP2_PATH)/timing.c \
			$(wildcard $(NP2_PATH)/bios/*.c) \
			$(wildcard $(NP2_PATH)/cbus/*.c) \
			$(wildcard $(NP2_PATH)/codecnv/*.c) \
			$(wildcard $(NP2_PATH)/common/*.c) \
			$(wildcard $(NP2_PATH)/embed/*.c) \
			$(wildcard $(NP2_PATH)/embed/menu/*.c) \
			$(wildcard $(NP2_PATH)/embed/menubase/*.c) \
			$(wildcard $(NP2_PATH)/fdd/*.c) \
			$(wildcard $(NP2_PATH)/font/*.c) \
			$(wildcard $(NP2_PATH)/generic/*.c) \
			$(wildcard $(NP2_PATH)/i286c/*.c) \
			$(wildcard $(NP2_PATH)/io/*.c) \
			$(wildcard $(NP2_PATH)/lio/*.c) \
			$(wildcard $(NP2_PATH)/mem/*.c) \
			$(wildcard $(NP2_PATH)/sdl2/*.c) \
			$(wildcard $(NP2_PATH)/sdl2/*.cpp) \
			$(wildcard $(NP2_PATH)/sound/*.c) \
			$(wildcard $(NP2_PATH)/sound/getsnd/*.c) \
			$(wildcard $(NP2_PATH)/sound/vermouth/*.c) \
			$(wildcard $(NP2_PATH)/vram/*.c)

OBJS = $(addsuffix .o,$(basename $(CPPSRCS)))
LIBS = -lm -lz $(shell sdl2-config --libs)

.SUFFIXES: .c.o
.SUFFIXES: .cpp.o

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CC) $(LFLAGS) -g -o $@ $(OBJS) $(LIBS)
	$(STRIP) $@

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.cpp.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)
