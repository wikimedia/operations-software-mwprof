/**
 * mwprof -- aggregate MediaWiki profiling samples
 *
 * Written in 2005 - 2013 by Domas Mituzas <domas@mituzas.lt>,
 * Tim Starling <tstarling@wikimedia.org>, and Ori Livneh <ori@wikimedia.org>.
 *
 * To the extent possible under law, the authors have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 */
#ifndef MWPROF_H_
#define MWPROF_H_

#include <stdio.h>
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
