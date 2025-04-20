#include <gtk/gtk.h>

static void
print_entry (GtkWidget* widget,
             gpointer   data)
{
    GtkEntryBuffer* buffer = (GtkEntryBuffer*) data;

    g_print(gtk_entry_buffer_get_text(buffer));
    g_print("\n");
}

static void
activate (GtkApplication* app,
          gpointer        user_data)
{
    GtkWidget* window;
    GtkWidget* button;
    GtkWidget* box;
    GtkWidget* entry;
    GtkEntryBuffer* entry_buffer;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "This is the Title");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    gtk_window_set_child(GTK_WINDOW(window), box);

    button = gtk_button_new_with_label("Click me");

    gtk_box_append(GTK_BOX(box), button);

    entry_buffer = gtk_entry_buffer_new("Enter text to display", 21);
    entry = gtk_entry_new_with_buffer(entry_buffer);

    gtk_box_append(GTK_BOX(box), entry);

    g_signal_connect(button, "clicked", G_CALLBACK(print_entry), (gpointer)entry_buffer);

    gtk_window_present(GTK_WINDOW(window));
}

int
main (int    argc,
      char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.BasicGui", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
