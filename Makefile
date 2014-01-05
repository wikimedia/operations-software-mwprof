# mwprof -- aggregate MediaWiki profiling samples
#
# Copyright 2005 Domas Mituzas <domas@mituzas.lt>
# Copyright 2011 Tim Starling <tstarling@wikimedia.org>
# Copyright 2013 Ori Livneh <ori@wikimedia.org>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
CFLAGS+=-Wall -O2 -g $(shell pkg-config --cflags --libs glib-2.0 gio-2.0 gthread-2.0 gio-unix-2.0)
LDLIBS+=$(shell pkg-config --libs glib-2.0 gio-2.0 gthread-2.0 gio-unix-2.0)

all: mwprof

mwprof: mwprof.c collector.c

clean:
	rm -f mwprof

install:
	mkdir -p /srv/deployment/mwprof/mwprof
	cp mwprof /srv/deployment/mwprof/mwprof/mwprof
