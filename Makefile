#
# Makefile to build Cimpress client
#
#

# Name of the binary
EXECUTABLE=textureatlas


# Where to install the binary
INSTALL_PATH = /usr/local


# List of source files which belongs to project
SOURCES = main.cpp \
          Atlas.cpp \



# List of needed libraries
LIBS = -lSDL -lSDL_image -largtable2 -lpng



CC=g++
CFLAGS=-O2 -c -Wall -Wextra -Wshadow -g
LDFLAGS=


# Make a list of object files from the source file list
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@


install:
	@install -v -s -m 755 $(EXECUTABLE) $(INSTALL_PATH)/bin/

uninstall:
	@rm -v $(INSTALL_PATH)/bin/$(EXECUTABLE)


# Clean up project, throw object files and executable file
clean:
	@rm -rfv *.o $(EXECUTABLE)


force_look:
	@true

.PHONY: all clean install uninstall force_look

