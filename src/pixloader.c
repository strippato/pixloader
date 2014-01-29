/* 
 * Copyright (c) 2014 Strippato strippato@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <gtk/gtk.h>

static GdkPixbuf *pix = NULL;
static GtkImage  *img = NULL;

static void
pixbufRead_cb (GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    GError *error = NULL;

    pix = gdk_pixbuf_new_from_stream_finish (res, &error);

    if (error) {
        g_print ("upps\n");
        g_error_free (error);
        gtk_main_quit ();
    } 

    gtk_image_set_from_pixbuf (img, pix);
    
    g_object_unref (pix);
}

static void
fileRead_cb (GFile *file, GAsyncResult *res, gpointer user_data)
{
    GError *error = NULL;

    GFileInputStream *input = g_file_read_finish (file, res, &error);
    
    if (!error) {
        gdk_pixbuf_new_from_stream_async ((GInputStream*)input, NULL, (GAsyncReadyCallback)pixbufRead_cb, NULL);        
    } else {
        g_print ("upps\n");
        g_error_free (error);
        gtk_main_quit ();
    } 
}    


int 
main (int argc, char *argv[])
{

    gtk_init (NULL, NULL);

    g_assert (argc == 2);

    GtkWidget* win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    g_signal_connect (GTK_WIDGET (win), "destroy", G_CALLBACK (gtk_main_quit), NULL);

    GFile* file = g_file_new_for_path (argv[1]);

    g_file_read_async (file, G_PRIORITY_DEFAULT, FALSE, (GAsyncReadyCallback)fileRead_cb, NULL);

    img = GTK_IMAGE(gtk_image_new ());

    GtkWidget* event = gtk_event_box_new ();

    gtk_container_add (GTK_CONTAINER (event), GTK_WIDGET (img));

    gtk_container_add (GTK_CONTAINER (win), GTK_WIDGET (event));

    g_signal_connect (G_OBJECT (event), "button_press_event", G_CALLBACK (gtk_main_quit), NULL);    
    
    gtk_widget_show_all (win);

    gtk_main ();

    g_object_unref (file);

    return 0;
}
