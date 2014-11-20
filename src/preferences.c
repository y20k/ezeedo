/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * preferences.c
 * Implements functions used to access and change settings
 *
 * Ezeedo
 * A todo.txt app for the GNOME desktop
 * 
 * Copyright (c) 2014 - Y20K.org
 * Licensed under the MIT-License
 * http://opensource.org/licenses/MIT
 *
 ***************************************************************************/


// DEBUG: valgrind --leak-check=full --track-origins=yes -v ./ezeedo


#include "preferences.h"
#include "helpers.h"
#include "start.h"


/**
 * Shows the preferences window
 */
void show_preferences_dialog (GSimpleAction *simple, GVariant *parameter, gpointer user_data)
{
    // define widgets
    GtkApplication *app;
    GtkWindow *win;
    GtkWidget *preferences_dialog;
    GtkWidget *preferences_content_area;
    GtkWidget *layoutgrid;
    GtkWidget *section_label;
    GtkWidget *select_file_label;
    GtkWidget *current_file_entry;
    GtkWidget *select_file_button;

    GtkDialogFlags flags;


    // get settings from gsettings store
    GSettings *settings;
    gchar* todotxt_file;

    settings = g_settings_new ("org.y20k.ezeedo");
    todotxt_file = g_settings_get_string (settings, "todo-txt-file");


    // get active window
    app = user_data;
    win = gtk_application_get_active_window (GTK_APPLICATION(app));

    // create preferences dialog
    flags               = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_USE_HEADER_BAR;
    preferences_dialog  = gtk_dialog_new_with_buttons ("Preferences", win, flags, NULL, NULL, NULL);
    gtk_window_set_icon_name    (GTK_WINDOW (preferences_dialog), EZEEDOICON);
    gtk_widget_set_size_request (GTK_WIDGET (preferences_dialog), 600, -1);

    // create content area
    preferences_content_area = gtk_dialog_get_content_area (GTK_DIALOG (preferences_dialog));

    // create preferences content
    section_label = gtk_label_new (NULL);
    gtk_label_set_markup (GTK_LABEL (section_label), "<b>Tasklist</b>");
    gtk_widget_set_halign (GTK_WIDGET (section_label), GTK_ALIGN_START);

    select_file_label = gtk_label_new ("Currently in use:");
    gtk_widget_set_halign (GTK_WIDGET(select_file_label), GTK_ALIGN_START);
    gtk_widget_set_margin_start (GTK_WIDGET(select_file_label), 10);

    
    current_file_entry = gtk_entry_new ();
    gtk_widget_set_sensitive (GTK_WIDGET(current_file_entry), FALSE);
	gtk_entry_set_text (GTK_ENTRY(current_file_entry), todotxt_file);
    // gtk_widget_set_halign (GTK_WIDGET(current_file_entry), GTK_ALIGN_START);
    gtk_widget_set_hexpand (GTK_WIDGET(current_file_entry), TRUE);
    
    select_file_button = gtk_button_new_with_label ("Select New File");
    gtk_widget_set_halign (GTK_WIDGET(select_file_button), GTK_ALIGN_START);

    // contruct preferences dialog
    layoutgrid = gtk_grid_new ();
    gtk_grid_set_column_spacing  (GTK_GRID (layoutgrid),20);
    gtk_widget_set_margin_start  (GTK_WIDGET(layoutgrid), 10);
    gtk_widget_set_margin_end    (GTK_WIDGET(layoutgrid), 10);
    gtk_widget_set_margin_top    (GTK_WIDGET(layoutgrid), 10);
    gtk_widget_set_margin_bottom (GTK_WIDGET(layoutgrid), 10);

    gtk_grid_attach (GTK_GRID (layoutgrid),section_label,     0,0,1,1);
    gtk_grid_attach (GTK_GRID (layoutgrid),select_file_label, 0,1,1,1);
    gtk_grid_attach (GTK_GRID (layoutgrid),current_file_entry,1,1,1,1);
    gtk_grid_attach (GTK_GRID (layoutgrid),select_file_button,2,1,1,1);

    gtk_container_add (GTK_CONTAINER (preferences_content_area), layoutgrid);

    // connect signals and show widget
    g_signal_connect (select_file_button, "clicked", G_CALLBACK (select_and_save_file), GTK_APPLICATION(app));
    g_signal_connect_swapped (preferences_dialog, "response", G_CALLBACK (gtk_widget_destroy), preferences_dialog);

    gtk_widget_show_all (preferences_dialog);

    return;
}


/**
 * Select file and save it to settings
 */
void select_and_save_file (GtkButton *button, gpointer user_data)
{
    GtkApplication *app;
    app = user_data;

    gchar *todotxt_file;
    gchar *todotxt_file_new;

    // Select new file name and location and get current ones 
    todotxt_file_new = open_file_dialog (G_APPLICATION(app));
    todotxt_file     = get_current_file_name_location ();

    
    // if changed save new file name and location
    if (todotxt_file_new != NULL && strcmp (todotxt_file,todotxt_file_new) != 0)
    {
        GtkWindow *win;

        // sav save new file name and locatione
        save_file_name_location (todotxt_file_new);

        // destroy toplevel window
        win = gtk_application_get_active_window (GTK_APPLICATION(app));
        save_window_position (NULL, NULL, win);
        gtk_widget_destroy (GTK_WIDGET(win));

        // restart app
        activate (GTK_APPLICATION(app), NULL);
    }
    else
    {
        // close the parent dailog
        GtkWidget* preferences_dialog;
        preferences_dialog = gtk_widget_get_ancestor (GTK_WIDGET(button), GTK_TYPE_DIALOG);
        gtk_widget_destroy (preferences_dialog);
    }

    return;
}
