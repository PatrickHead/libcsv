CC = gcc
COPTS = -O0 -g -Wall

all: libcsv.a test-csv doxygen

doxygen: Doxygen.libcsv csv.h csv.c README.md
	doxygen Doxygen.libcsv
	@touch doxygen

csv.o: csv.c csv.h
	$(CC) $(COPTS) -c csv.c

test-csv: test-csv.o csv.o
	$(CC) $(COPTS) -o test-csv test-csv.o csv.o

test-csv.o: test-csv.c csv.h
	$(CC) $(COPTS) -c test-csv.c

libcsv.a: csv.o
	ar rcs libcsv.a csv.o

install: libcsv.a csv.h
	@(mkdir -p /usr/local/lib)
	@(mkdir -p /usr/local/include)
	@(cp libcsv.a /usr/local/lib)
	@(cp csv.h /usr/local/include)

tar: clean
	@(cd ..; \
		tar cJf libcsv.txz libcsv; \
	)

zip: clean
	@(cd ..; \
		rm -f libcsv.zip; \
		zip -r libcsv.zip libcsv; \
	)

clean:
	@(rm -f *.a *.o *.stackdump)
	@(rm -f *.obj *.exe *.dll *.lib)
	@(rm -f test-csv)
	@(rm -f *.txz)
	@(rm -f *.zip)
	@(rm -rf doxygen)

windows: clean
	@make -f Makefile.win

