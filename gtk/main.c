#include <gtk/gtk.h>

// to send error if filename is wrong
GdkPixbuf *create_pixbuf(const gchar * filename) {
    
   GdkPixbuf *pixbuf;
   GError *error = NULL;
   pixbuf = gdk_pixbuf_new_from_file(filename, &error);
   
   if (!pixbuf) {
       
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
   }

   return pixbuf;
}

int main(int argc, char *argv[])
{
  // var declarations
  GtkWidget *window;
  GdkPixbuf *icon;
  
  gtk_init(&argc, &argv);

  // main window setup
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "OCR");
  gtk_window_set_default_size(GTK_WINDOW(window), 100, 50);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

  // icon setup
  icon = create_pixbuf("icon.png");
  gtk_window_set_icon(GTK_WINDOW(window), icon);

  gtk_widget_show(window); // show the toplevel window

  g_signal_connect(G_OBJECT(window), "destroy", 
      G_CALLBACK(gtk_main_quit), NULL); // to link X to destroy

  g_object_unref(icon); // to free the icon when the count drops to 0

  gtk_main(); 
  return 0;
}
