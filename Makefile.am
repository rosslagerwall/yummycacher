bin_PROGRAMS=yummycacher
yummycacher_SOURCES=src/main.c src/pserv.c src/pserv.h src/dns.c src/dns.h \
	src/protoshttp.c src/protoshttp.h src/pclient.c src/pclient.h \
	src/protochttp.c src/protochttp.h src/cxmap.h src/cxmap.c \
	src/options.c src/options.h src/log.c src/log.h

AM_CPPFLAGS = $(DEPS_CFLAGS) $(DEPS2_CFLAGS)
yummycacher_LDADD = -levent_core -levent_extra $(DEPS2_LIBS)
