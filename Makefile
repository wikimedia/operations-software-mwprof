# mwprof -- aggregate MediaWiki profiling samples
#
# Written in 2005 - 2013 by Domas Mituzas <domas@mituzas.lt>,
# Tim Starling <tstarling@wikimedia.org>, and Ori Livneh <ori@wikimedia.org>.
#
# To the extent possible under law, the authors have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

CFLAGS+=-Wall -O2 -g $(shell pkg-config --cflags --libs glib-2.0 gio-2.0 gthread-2.0 gio-unix-2.0)
LDLIBS+=$(shell pkg-config --libs glib-2.0 gio-2.0 gthread-2.0 gio-unix-2.0)

all: mwprof

mwprof: mwprof.c collector.c

clean:
	rm -f mwprof

install:
	mkdir -p /srv/deployment/mwprof/mwprof
	cp mwprof /srv/deployment/mwprof/mwprof/mwprof
