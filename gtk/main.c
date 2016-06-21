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
  GtkWidget *button;
  GtkWidget *table;
  
  gtk_init(&argc, &argv);

  // main window setup
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "OCR");
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 480);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

  // table setup
  table = gtk_table_new(4, 1, TRUE);
  gtk_table_set_row_spacings(GTK_TABLE(table), 4);

  // buttons and alignment
  button = gtk_button_new_with_label("Load image");
  gtk_widget_set_tooltip_text(button, "Load an image in a standard format");
  gtk_table_attach_defaults(GTK_TABLE(table), button, 0, 1, 0, 1);

  button = gtk_button_new_with_label("Save text");
  gtk_widget_set_tooltip_text(button, "Save text in a file.");
  gtk_table_attach_defaults(GTK_TABLE(table), button, 1, 2, 0, 1);
  
  button = gtk_button_new_with_label("Load weight");
  gtk_widget_set_tooltip_text(button, "Load your own weight for the neural network");
  gtk_table_attach_defaults(GTK_TABLE(table), button, 2, 3, 0, 1);

  button = gtk_button_new_with_label("Train");
  gtk_widget_set_tooltip_text(button, "Train the neural network");
  gtk_table_attach_defaults(GTK_TABLE(table), button, 3, 4, 0, 1);

  gtk_container_add(GTK_CONTAINER(window), table);

  // icon setup
  icon = create_pixbuf("icon.png");
  gtk_window_set_icon(GTK_WINDOW(window), icon);

  gtk_widget_show_all(window); // show the toplevel window and childs

  g_signal_connect(G_OBJECT(window), "destroy", 
    G_CALLBACK(gtk_main_quit), NULL); // to link X to destroy

  g_object_unref(icon); // to free the icon when the count drops to 0

  gtk_main(); 
  return 0;
}
