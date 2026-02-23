SRCDIR = ../src
INCLDIR = ../include

WINPKGSDIR = $(HOME)/develop/win-pkgs
WINPKGINCLDIR = $(WINPKGSDIR)/include
WINPKGLIBDIR = $(WINPKGSDIR)/lib

CC = x86_64-w64-mingw32-gcc
COPTS = -O3 -g0 -Wall -I $(INCLDIR) -I $(WINPKGINCLDIR) -L . -L $(WINPKGLIBDIR)
AR = x86_64-w64-mingw32-ar
RANLIB = x86_64-w64-mingw32-ranlib

all: csv.lib test-csv.exe

csv.obj: $(SRCDIR)/csv.c $(INCLDIR)/csv.h
	$(CC) $(COPTS) -o csv.obj -c $(SRCDIR)/csv.c

test-csv.exe: test-csv.obj csv.obj
	$(CC) $(COPTS) -o test-csv.exe test-csv.obj csv.obj -lavl

test-csv.obj: $(SRCDIR)/test-csv.c $(INCLDIR)/csv.h
	$(CC) $(COPTS) -o test-csv.obj -c $(SRCDIR)/test-csv.c

libcsv.a: csv.obj
	$(AR) rcs libcsv.a csv.obj

csv.lib: libcsv.a
	@cp libcsv.a csv.lib
	$(RANLIB) csv.lib

clean:
	@rm -f *.a *.obj *.lib *.exe

install: csv.lib
	@cp csv.lib $(WINPKGLIBDIR)
	@cp ../include/csv.h $(WINPKGINCLDIR)

uninstall:
	@rm -f $(WINPKGLIBDIR)/csv.lib
	@rm -f $(WINPKGINCLDIR)/csv.h

