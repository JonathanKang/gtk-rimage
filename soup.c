#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <libsoup/soup.h>

int
main (int argc, char *argv[])
{
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    GInputStream *input_stream;
    GtkWidget *window;
    GtkWidget *image;
    guint status_code;
    SoupMessage *msg;
    SoupSession *session;

    gtk_init (&argc, &argv);

    /* HTTP request */
    msg = soup_message_new ("GET", "https://avatars2.githubusercontent.com/u/5607743?s=460&v=4");
    session = soup_session_new ();
    status_code = soup_session_send_message (session, msg);
    if (status_code != SOUP_STATUS_OK) {
        g_warning ("Failed to download the image");

        return 1;
    }

    input_stream = g_memory_input_stream_new_from_data (msg->response_body->data,
                                                        msg->response_body->length,
                                                        NULL);
    pixbuf = gdk_pixbuf_new_from_stream (input_stream, NULL, &error);
    if (error != NULL)
    {
        g_warning ("unable to create pixbuf: %s\n",
                   error->message);
        g_clear_error (&error);

        return 1;
    }

    /* Create a GtkImage based on the pixbuf */
    image = gtk_image_new_from_pixbuf (pixbuf);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_add (GTK_CONTAINER (window), image);
    gtk_widget_show_all (window);

    gtk_main ();

    g_object_unref (msg);
    g_object_unref (session);
    g_input_stream_close (input_stream, NULL, &error);
    if (error != NULL)
    {
        g_warning ("unable to close input stream: %s",
                   error->message);
        g_clear_error (&error);
    }

    return 0;
}
