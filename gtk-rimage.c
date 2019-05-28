#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <libsoup/soup.h>

static gboolean
draw_cb (GtkWidget *widget,
         cairo_t *cr,
         gpointer user_data)
{
    gint height;
    gint width;
    GdkPixbuf *temp;

    height = gtk_widget_get_allocated_height (widget);
    width = gtk_widget_get_allocated_width (widget);

    temp = gdk_pixbuf_scale_simple (GDK_PIXBUF (user_data),
                                    width, height, GDK_INTERP_BILINEAR);
    gdk_cairo_set_source_pixbuf (cr, temp, 0, 0);
    cairo_paint (cr);

    g_object_unref (temp);

    return FALSE;
}

int
main (int argc, char *argv[])
{
    GdkPixbuf *pixbuf;
    GdkPixbufLoader *loader;
    GError *error = NULL;
    GtkWidget *drawing_area;
    GtkWidget *window;
    guint status_code;
    SoupMessage *msg;
    SoupSession *session;

    gtk_init (&argc, &argv);

    drawing_area = gtk_drawing_area_new ();

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_add (GTK_CONTAINER (window), drawing_area);
    gtk_widget_show_all (window);

    /* HTTP request */
    msg = soup_message_new ("GET", "https://avatars2.githubusercontent.com/u/5607743?s=460&v=4");
    /* msg = soup_message_new (SOUP_METHOD_GET, "https://wx2.sinaimg.cn/mw690/6d3fe6d2gy1fzbzeojeh7j20u0182x6s.jpg"); */
    session = soup_session_new ();
    status_code = soup_session_send_message (session, msg);
    if (status_code != SOUP_STATUS_OK) {
        g_warning ("Failed to download the image");

        return 1;
    }

    loader = gdk_pixbuf_loader_new ();
    gdk_pixbuf_loader_write (loader,
                             (guchar *)msg->response_body->data,
                             msg->response_body->length, &error);
    if (error != NULL)
    {
        g_warning ("unable to create pixbuf: %s",
                   error->message);
        g_clear_error (&error);

        return 1;
    }

    pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
    g_signal_connect (drawing_area, "draw", G_CALLBACK (draw_cb), pixbuf);

    gtk_main ();

    g_object_unref (msg);
    g_object_unref (session);
    gdk_pixbuf_loader_close (loader, &error);

    return 0;
}
