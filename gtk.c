#include <gtk/gtk.h>
#include <gtk/gtkenums.h>

int main(int argc, char **argv)
{
	/* Initialize i18n support */
	gtk_set_locale();

	/* Initialize the widget set */
	gtk_init(&argc, &argv);

	/* Create the main window */
	GtkWidget *mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *but = gtk_button_new();
	mainwin->add(but);

	/* Show the application window */
	gtk_widget_show_all(mainwin);

	/* Enter the main event loop, and wait for user interaction */
	gtk_main();

	/* The user lost interest */
	return 0;
}
