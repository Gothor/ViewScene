SHELL       = /bin/sh

# default config:
#    release
#    x86
#    shared
#    
#    to build 64 bit:                make -e M64=-m64
#    to build static:                make -e STATIC_LINK=1
#    to build 64 bit static debug:   make -e M64=-m64 STATIC_LINK=1 VARIANT=debug
#

VARIANT      = release
ifeq "$(VARIANT)" "debug"
    GCC_DEBUG_FLAGS += -g -D_DEBUG
endif

M64 = -m32
ARCH = x86
S64  =
EXTRA_LIBS=-lGL4Dummies $(SDL_LDFLAGS)

ifeq "$(M64)" "-m64"
    ARCH=x64
    S64 = 64
    EXTRA_LIBS=-lGL4Dummies $(SDL_LDFLAGS)
endif
  
EXAMPLE     = ViewScene
BINDIR      = ./build
OBJDIR      = ./build
LIBDIR      = /mnt/d/Rodolphe/Ubuntu/FBX/lib/gcc4/$(ARCH)/$(VARIANT)
INCDIR      = /mnt/d/Rodolphe/Ubuntu/FBX/include

TARGET      = $(BINDIR)/$(EXAMPLE)
OBJS  = \
	./Common.o\
	./DrawScene.o\
	./DrawText.o\
	./GetPosition.o\
	./SceneCache.o\
	./SceneContext.o\
	./GlFunctions.o\
	./SetCamera.o\
	./targa.o\
	./main.o

CC          = gcc 
LD          = gcc 
RM          = /bin/rm -rf

SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs)

CXXFLAGS    = $(M64) $(GCC_DEBUG_FLAGS) $(SDL_CFLAGS)
LDFLAGS     = $(M64) $(GCC_LINK_FLAGS)

RPATH_DEF    =
STATIC_LINK  =
FBXSDK_LIB   = -lfbxsdk
ifeq "$(STATIC_LINK)" ""
    CXXFLAGS += -DFBXSDK_SHARED
    RPATH_DEF = -Wl,-rpath $(LIBDIR)
else
    FBXSDK_LIB = $(LIBDIR)/libfbxsdk.a
endif

LIBS = $(FBXSDK_LIB) -L/home/rpeccatte/local/lib -lm -lrt -lstdc++ -lpthread -ldl $(EXTRA_LIBS) -lGL

.SUFFIXES: .cxx

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(BINDIR)
#	$(LD) $(LDFLAGS) -o $@ $(OBJSFINAL) -L$(LIBDIR) -L. -L/usr/lib$(S64) -L/usr/X11R6/lib$(S64) $(LIBS) $(RPATH_DEF)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) -L$(LIBDIR) -L. -L/usr/lib$(S64) -L/usr/X11R6/lib$(S64) $(LIBS)
	cp *.fbx $(BINDIR)
	cp FBX_ASCII_TEXTURE $(BINDIR)

.c.o:
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $*.o 
	# mv $*.o $(OBJDIR)

.cxx.o:
	mkdir -p $(OBJDIR)
	$(CC) $(CXXFLAGS) -I$(INCDIR) -c $< -o $*.o
	# mv $*.o $(OBJDIR)

clean:
	rm -rf $(BINDIR)
	rm -rf $(OBJS)

