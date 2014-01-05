/**
 * mwprof -- aggregate MediaWiki profiling samples
 *
 * Copyright 2005 Domas Mituzas <domas@mituzas.lt>
 * Copyright 2011 Tim Starling <tstarling@wikimedia.org>
 * Copyright 2013 Ori Livneh <ori@wikimedia.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MWPROF_H_
#define MWPROF_H_

#include <glib.h>
#define POINTS 300

extern GHashTable *table;
G_LOCK_EXTERN(table);

typedef struct CallStats {
    GMutex mutex;
    gulong count;
    gdouble cpu;
    gdouble cpu_sq;
    gdouble real;
    gdouble real_sq;
    gint real_pointer;
    gdouble reals[POINTS];
} CallStats;

GString * generate_xml();
void truncate_data();
void handle_message(gchar *buffer);
void update_entry(char *db, char *host, char *task, CallStats *sample);

#endif  // MWPROF_H_
