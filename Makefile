.PHONY:  purge clean setup-static setup-shared setup-executable executable shared static install uninstall sudo-check help

default: executable

# overwrittable variables
OUTDIR?=out
BUILDDIR?=build
DEBUG?=0
CMAKE_BUILD_JOBS?=2
PREFIX?=/usr/local

PROJECT=brainfreak
TARGET=all
CMAKE_BUILD_FLAGS=--build ${BUILDDIR} -j ${CMAKE_BUILD_JOBS} --target ${TARGET}
CMAKE_SETUP_FLAGS=-B${BUILDDIR} -H. -DCMAKE_EXPORT_COMPILE_COMMANDS=1

ifeq ($(DEBUG),1)
	CMAKE_SETUP_FLAGS+= -DCMAKE_BUILD_DEBUG=true
endif

# https://stackoverflow.com/a/14777895
ifeq ($(OS),Windows_NT)
  detected_OS := Windows
else
  detected_OS := $(shell uname)
endif

# out name
ifeq ($(detected_OS),Windows)
	EXE_NAME=${PROJECT}.exe
	SHARED_NAME=lib${PROJECT}.dll
	STATIC_NAME=lib${PROJECT}.lib
else ifeq ($(detected_OS),Darwin)
	EXE_NAME=${PROJECT}.exe
	SHARED_NAME=lib${PROJECT}.dylib
	STATIC_NAME=lib${PROJECT}.a
else
	EXE_NAME=${PROJECT}
	SHARED_NAME=lib${PROJECT}.so
	STATIC_NAME=lib${PROJECT}.a
endif

# does $PREFIX has write permissions
HAS_WRITE_PERM=$(shell test -w ${PREFIX} && echo 1 || echo 0)

# setups
# default target
setup-executable:
	cmake ${CMAKE_SETUP_FLAGS} -DCMAKE_BUILD_TYPE=executable

setup-shared:
	cmake ${CMAKE_SETUP_FLAGS} -DCMAKE_BUILD_TYPE=shared

setup-static:
	cmake ${CMAKE_SETUP_FLAGS} -DCMAKE_BUILD_TYPE=static

#build
executable: ${OUTDIR} setup-executable
	cmake ${CMAKE_BUILD_FLAGS} --
	mv ${BUILDDIR}/${EXE_NAME} ${OUTDIR}

shared: ${OUTDIR} setup-shared
	cmake ${CMAKE_BUILD_FLAGS} --
	mv ${BUILDDIR}/${SHARED_NAME} ${OUTDIR}

static: ${OUTDIR} setup-static
	cmake ${CMAKE_BUILD_FLAGS} --
	mv ${BUILDDIR}/${STATIC_NAME} ${OUTDIR}

all: static shared executable

${OUTDIR}:
	mkdir ${OUTDIR}

sudo-check: 
ifeq ($(HAS_WRITE_PERM),0)
	$(error "current user does not have write permissions to ${PREFIX}. re-running with sudo may solve the issue.")
endif

install: sudo-check
	-@install -v -m 644 -D -t $(PREFIX)/lib  $(OUTDIR)/$(STATIC_NAME)
	-@install -v -m 777 -D -t $(PREFIX)/lib $(OUTDIR)/$(SHARED_NAME)
	-@install -v -m 755 -D -t $(PREFIX)/bin $(OUTDIR)/$(EXE_NAME)
	-@install -v -m 644 -D -t $(PREFIX)/include/$(PROJECT) src/include/$(PROJECT)/*

uninstall: sudo-check
	-@rm -v $(PREFIX)/lib/$(SHARED_NAME) $(PREFIX)/lib/$(STATIC_NAME) $(PREFIX)/bin/$(EXE_NAME)
	-@rm -vr $(PREFIX)/include/$(PROJECT)
# cleaning
clean:
	cmake --build ${BUILDDIR} --target clean --
	rm -rf ${OUTDIR}
purge: clean
	rm -rf ${BUILDDIR}

help:
	@echo "available targets for ${PROJECT}:"
	@echo "  build targets: (OUTDIR=${OUTDIR}; BUILDDIR=${BUILDDIR}; DEBUG=${DEBUG}; CMAKE_BUILD_JOBS=${CMAKE_BUILD_JOBS};)"
	@echo "    .. executable (default) \t -- build the interpreter with main function, enabling the repl  (${EXE_NAME})"
	@echo "    .. shared \t\t\t -- build as a shared library  (${SHARED_NAME})"
	@echo "    .. static \t\t\t -- build as a satic library  (${STATIC_NAME})"
	@echo "    .. all \t\t\t -- build the three above"
	@echo ""
	@echo "  cleaning targets:"
	@echo "    .. clean \t\t\t -- remove compiled files"
	@echo "    .. purge \t\t\t -- remove every file generated"
	@echo ""
	@echo "  install targets: (PREFIX=${PREFIX};)"
	@echo "    .. install"
	@echo "    .. uninstall"
