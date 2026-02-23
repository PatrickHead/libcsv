AM_CPPFLAGS = -I $(srcdir)/include

AM_CFLAGS = -O3 -g0 -Wall

ARFLAGS = cr

lib_LIBRARIES = lib/libcsv.a
lib_libcsv_a_SOURCES = src/csv.c include/csv.h

bin_PROGRAMS = bin/test-csv
bin_test_csv_SOURCES = src/test-csv.c
bin_test_csv_LDADD = lib/libcsv.a

include_HEADERS = include/csv.h

EXTRA_DIST = windows extras acdoxygen.m4 amdoxygen.am doxygen.csv.cfg csv.pc .gitignore

super-clean: clean distclean
	@find . -name 'Makefile.in' -delete
	@find . -name 'config.h.in' -delete
	@find . -name '*.tar' -delete
	@find . -name '*.tar.*' -delete
	@find . -name '*~' -delete
	@find . -name '*.obj' -delete
	@find . -name '*.lib' -delete
	@find . -name '*.exe' -delete
	@rm -rf autom4te.cache
	@rm -f aclocal.m4
	@rm -f configure
	@rm -rf src/.deps
	@rm -f texput.log

include amdoxygen.am

MOSTLYCLEANFILES = $(DX_CLEANFILES)

EXTRA_DIST += $(DX_CONFIG)

install-data-hook:
	@if test -d $(DX_HTML_DIR); \
	 then \
	   echo installing $(DX_HTML_DIR) to $(prefix)/share/doc/libcsv; \
	   mkdir -p $(prefix)/share/doc/libcsv; \
	   cp -r $(DX_HTML_DIR) $(prefix)/share/doc/libcsv; \
	 fi
	@echo installing libcsv.pc
	@mkdir -p $(prefix)/share/pkgconfig
	@cp libcsv.pc $(prefix)/share/pkgconfig/libcsv.pc

uninstall-hook:
	@rm -rf $(prefix)/share/doc/libcsv
	@rm -rf $(prefix)/share/pkgconfig/libcsv.pc

clean-local:
	@(cd windows; make clean)

code-stats:
	@echo -n " total lines of source code: "; wc -l src/*.[ch] include/*.[ch] | tail -1 | sed -e 's/ total//' -e 's/^  *//'
	@echo -n "actual lines of source code: "; (gcc -fpreprocessed -dD -E src/*.[ch] include/*.[ch] | sed -e '/^$$/d' -e '/^# [0-9]/d' | wc -l)

