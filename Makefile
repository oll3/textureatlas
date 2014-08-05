# Name of the binary to build
EXECUTABLE=textureatlas

# Where to install the binary
INSTALL_PATH = /usr/local

# List of source files which belongs to project
SOURCES = main.cpp \
          Atlas.cpp \
          savepng.cpp \


BINS = res/SpriteDescriptor.h \


# List of global dependencies (rebuild upon touched)
DEPS = Makefile \
       *.h \



# List of libraries to link with
LIBS = -lSDL2 -lSDL2_image -largtable2 -lpng


CC=g++
OBJCOPY=objcopy
CFLAGS=-O2 -Wshadow -Wmaybe-uninitialized -g #-Wall 
LDFLAGS=-g


# Make a list of object files from the source file list
OBJ=$(SOURCES:.cpp=.o)
#BINOBJECTS=$(BINS:.h=.o)
BINOBJECTS:=$(addsuffix .o, $(basename $(BINS)))

all: $(EXECUTABLE)


#
# Compile to object files
#
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<


#
# Copy binaries to include in final binary
#
$(BINOBJECTS): $(BINS)
	$(OBJCOPY) --input binary --output elf64-x86-64 --binary-architecture i386 $< $@


#
# Link object files
#
$(EXECUTABLE): $(BINOBJECTS) $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)


#
# Install 
#
install:
	@install -v -s -m 755 $(EXECUTABLE) $(INSTALL_PATH)/bin/


#
# Uninstall
#
uninstall:
	@rm -v $(INSTALL_PATH)/bin/$(EXECUTABLE)


# Clean up project, throw object files and executable file
clean:
	@rm -rfv $(OBJ) $(BINOBJECTS) $(EXECUTABLE)


.PHONY: all clean install uninstall



