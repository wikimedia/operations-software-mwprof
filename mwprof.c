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
#define GLIB_VERSION_MIN_REQUIRED GLIB_VERSION_2_32

#include <glib.h>
#include <gio/gio.h>
#include "mwprof.h"

GHashTable *table;
G_LOCK_DEFINE(table);

static void listen_stats(gpointer data);
static gboolean serve_xml();

static gint listen_port = 3811;
static GOptionEntry entries[] = {
    { "listen-port", 'p', 0, G_OPTION_ARG_INT, &listen_port,
        "UDP & TCP listen port (default: 3811)", "PORT" },
    { NULL }
};

/* Parse command-line arguments. */
static void
parse_args(int argc, char **argv) {
    GOptionContext *context = g_option_context_new(NULL);
    GError *error = NULL;

    g_option_context_set_summary(context, "Aggregate MediaWiki profiling samples");
    g_option_context_add_main_entries(context, entries, NULL);
    g_option_context_parse(context, &argc, &argv, &error);
    g_assert_no_error(error);
    g_option_context_free(context);
}

/* Answers incoming TCP connections with an XML stats dump. */
static gboolean
serve_xml(
    GThreadedSocketService  *service,
    GSocketConnection       *connection,
    GSocketListener         *listener,
    gpointer                user_data
) {
    GInputStream *in;
    GOutputStream *out;
    GString *xml;

    // Generate full dump XML into a buffer, then wrap the full buffer as a
    // GMemoryInputStream, then splice it into the socket's output stream.
    xml = generate_xml();
    in = g_memory_input_stream_new_from_data(xml->str, xml->len, g_free);
    g_slice_free(GString, xml);
    out = g_io_stream_get_output_stream(G_IO_STREAM(connection));
    g_output_stream_splice_async(out, in, G_OUTPUT_STREAM_SPLICE_CLOSE_SOURCE |
                                 G_OUTPUT_STREAM_SPLICE_CLOSE_TARGET,
                                 G_PRIORITY_DEFAULT, NULL,
                                 (GAsyncReadyCallback)g_output_stream_splice_finish,
                                 NULL);
    g_object_unref(in);
    return TRUE;
}

/* Process profiling samples coming in via UDP. */
static void
listen_stats(gpointer data) {
    GError *error = NULL;
    GSocket *stats_sock;
    GInetAddress *inet_address;
    GSocketAddress *address;
    gint port = GPOINTER_TO_INT(data);
    gchar buf[65535] = {0};
    gssize nbytes;

    stats_sock = g_socket_new(G_SOCKET_FAMILY_IPV6, G_SOCKET_TYPE_DATAGRAM,
                              G_SOCKET_PROTOCOL_UDP, &error);
    g_assert_no_error(error);

    inet_address = g_inet_address_new_any(G_SOCKET_FAMILY_IPV6);
    address = g_inet_socket_address_new(inet_address, port);
    g_object_unref(inet_address);

    g_socket_bind(stats_sock, address, TRUE, &error);
    g_assert_no_error(error);
    g_object_unref(address);

    while (TRUE) {
        nbytes = g_socket_receive(stats_sock, buf, sizeof(buf)-1, NULL, &error);
        g_assert_no_error(error);
        buf[nbytes] = '\0';
        handle_message(buf);
    }
}

int
main(int argc, char **argv) {
    GThread *listener;
    GSocketService *service;
    GError *error = NULL;

    // Initialize GLib's type and threading system.
    g_type_init();

    parse_args(argc, argv);

    // Hash table used to store stats.
    table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

    // Spawn a dedicated worker thread for processing incoming stats.
    listener = g_thread_new("stats listener", (GThreadFunc)listen_stats,
                            GINT_TO_POINTER(listen_port));
    g_thread_unref(listener);

    // Serve XML via a TCP socket service running on GLib's main event loop.
    service = g_socket_service_new();
    g_socket_listener_add_inet_port(G_SOCKET_LISTENER(service), listen_port,
                                    NULL, &error);
    g_assert_no_error(error);
    g_signal_connect(service, "incoming", G_CALLBACK(serve_xml), NULL);
    g_socket_service_start(service);

    g_main_loop_run(g_main_loop_new(NULL, FALSE));
    g_assert_not_reached();
}
