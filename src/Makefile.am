AUTOMAKE_OPTIONS = foreign

AM_CXXFLAGS = -Wall -Werror -std=gnu++14 -fPIC
INCLUDES = -I`root-config --incdir`

lib_LTLIBRARIES = libGEM_Performance.la

include_HEADERS = 

noinst_HEADERS = GEM_Performance_LinkDef.h

dist_data_DATA =

AM_LDFLAGS = \
        -L$(libdir)

libGEM_Performance_la_LIBADD = \
        -L$(libdir) 

libGEM_Performance_la_SOURCES = \
  Gain.C \
  Rate_Capability.C \
  Read_RO.C \
  GEM_Performance_Dict.C

BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libGEM_Performance.la

testexternals.C:
	@echo '//*** this is a generated file. Do not commit, do not edit' > $@
	@echo 'int main()' >> $@
	@echo '{' >> $@
	@echo '  return 0;' >> $@
	@echo '}' >> $@

GEM_Performance_Dict.C: \
  Const_Def.h \
  Gain.h \
  Rate_Capability.h \
  Read_RO.h \
  GEM_Performance_LinkDef.h
	rootcling -f $@ -I`root-config --incdir` -c $^

pcm :
	make install
	cp GEM_Performance_Dict_rdict.pcm $(libdir)

CLEANFILES = GEM_Performance_Dict.C GEM_Performance_Dict.h GEM_Performance_Dict_rdict.pcm
