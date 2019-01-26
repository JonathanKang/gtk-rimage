#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <libsoup/soup.h>

static void
on_area_prepared (GdkPixbufLoader *loader,
                  gpointer user_data)
{
    GdkPixbuf *pixbuf;
    GtkWidget *image;

    image = GTK_WIDGET (user_data);

    pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
    gdk_pixbuf_fill (pixbuf, 0xaaaaaaff);

    gtk_image_set_from_pixbuf (GTK_IMAGE (image), pixbuf);
}

static void
on_area_updated (GdkPixbufLoader *loader,
                 gint             x,
                 gint             y,
                 gint             width,
                 gint             height,
                 gpointer         user_data)
{
    GdkPixbuf *pixbuf;
    GtkWidget *image;

    image = GTK_WIDGET (user_data);

    pixbuf = gtk_image_get_pixbuf (GTK_IMAGE (image));
    g_object_ref (pixbuf);
    gtk_image_set_from_pixbuf (GTK_IMAGE (image), pixbuf);
    g_object_unref (pixbuf);
}

int
main (int argc, char *argv[])
{
    GdkPixbufLoader *loader;
    GError *error = NULL;
    GtkWidget *window;
    GtkWidget *image;
    guint status_code;
    SoupMessage *msg;
    SoupSession *session;

    gtk_init (&argc, &argv);

    image = gtk_image_new_from_pixbuf (NULL);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_add (GTK_CONTAINER (window), image);
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
    g_signal_connect (loader, "area-prepared",
                      G_CALLBACK (on_area_prepared), image);
    g_signal_connect (loader, "area-updated",
                      G_CALLBACK (on_area_updated), image);
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

    gtk_main ();

    g_object_unref (msg);
    g_object_unref (session);
    gdk_pixbuf_loader_close (loader, &error);

    return 0;
}
