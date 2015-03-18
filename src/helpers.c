/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * helpers.c
 * Implements helper functions
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


#include "helpers.h"

#include "categorylist.h"
#include "tasklist.h"


/**
 * Shows line of text within dialog window or in terminal
 */
void
show_info (GtkWidget *widget,
           gchar     *text,
           gboolean   dialog_window)
{
    if (dialog_window && widget != NULL)
    {
        display_info_dialog (widget,
                             text);
    }
    else
    {
        g_print ("Ezeedo: %s\n", text);
    }
}


/**
 * Display simple information dialog with given text
 */
void
display_info_dialog (GtkWidget *widget,
                     gpointer   user_data)
{
    // get dialog message from user data
    gchar *dialog_message;
    dialog_message = user_data;
 
    // get toplevel window
    GtkWidget *toplevel;
    toplevel = gtk_widget_get_toplevel (widget);
    if (gtk_widget_is_toplevel (toplevel))
    {
        GtkWidget *dialog;
        GtkDialogFlags flags;

        flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
        dialog = gtk_message_dialog_new (GTK_WINDOW(toplevel),
                                         flags,
                                         GTK_MESSAGE_INFO,
                                         GTK_BUTTONS_CLOSE,
                                         "%s", dialog_message);

        gtk_dialog_run (GTK_DIALOG(dialog));
        gtk_widget_destroy (dialog);
    }
    // TODO unref

    return;
}


/**
 * Ask user for location of todo.txt file
 */
gchar
*open_file_dialog (GApplication *app)
{
    GtkWindow            *window;
    GtkWidget            *dialog;
    GtkFileChooserAction  action;
    GtkFileFilter        *filter;
    gint                  result;

    // create file chooser dialog
    action = GTK_FILE_CHOOSER_ACTION_OPEN;
    window = gtk_application_get_active_window (GTK_APPLICATION(app));
    dialog = gtk_file_chooser_dialog_new ("Choose Todo.txt File",
                                          window,
                                          action,
                                          "Open",   GTK_RESPONSE_ACCEPT,
                                          "Cancel", GTK_RESPONSE_CANCEL,
                                          NULL);
    gtk_dialog_set_default_response (GTK_DIALOG (dialog),
                                     GTK_RESPONSE_ACCEPT);

 
    // create filter for plain text files
    filter = gtk_file_filter_new ();
    gtk_file_filter_set_name (filter,
                              "Todo.txt Tasklist File");
    gtk_file_filter_add_mime_type (filter,
                                   "text/plain");
    gtk_file_filter_add_pattern (filter,
                                 "*.txt");
    gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (dialog),
                                 filter);

    // display dialog
    result = gtk_dialog_run (GTK_DIALOG (dialog));

    // return filename or quit application
    if (result == GTK_RESPONSE_ACCEPT)
    {
        gchar *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename (chooser);
        gtk_widget_destroy (dialog);
        return (filename);
    }
    else
    {
        gtk_widget_destroy (dialog);
        return (NULL);
    }
}


/**
 * Toggles visibility of sidebar
 */
void
toggle_sidebar (GSimpleAction *simple,
                GVariant      *parameter,
                gpointer       user_data)
{
    // get ezeedo from user data
    ezeedo_wrapper_structure *ezeedo;
    ezeedo = user_data;

    // create variables containing sidebar sizes
    gint size_saved;
    gint size_current;
    
    // get saved sidebar size from gsettings store
    GSettings *settings;
    settings   = g_settings_new     ("org.y20k.ezeedo");
    size_saved = g_settings_get_int (settings,
                                     "sidebar-size");
    g_object_unref (settings);

    // get current sidebar size
    size_current = gtk_paned_get_position (GTK_PANED(ezeedo->todo_paned));

    // hide sidebar and show all todos
    if (size_current != 0)
    {
        gtk_paned_set_position (GTK_PANED(ezeedo->todo_paned),
                                0);
        show_tasklist (ezeedo,
                       CATEGORYLIST_ALL,
                       1);
        // TODO unref
    }
    // show sidebar
    else
    {
        gtk_paned_set_position (GTK_PANED(ezeedo->todo_paned),
                                size_saved);
    }
}


/**
 * Saves position and size of a given window
 */
void
save_window_position (GSimpleAction *simple,
                      GVariant      *parameter,
                      gpointer       user_data)
{
    GSettings *settings;
    gint x;
    gint y;
    gint width;
    gint height;
    gint sidebar_size;
    
    // get ezeedo from user data
    ezeedo_wrapper_structure *ezeedo;
    ezeedo = user_data;

    gtk_window_get_position (GTK_WINDOW(ezeedo->window),
                             &x,
                             &y);
    gtk_window_get_size (GTK_WINDOW(ezeedo->window),
                         &width,
                         &height);
    sidebar_size = gtk_paned_get_position (GTK_PANED(ezeedo->todo_paned));
    
    // save changes to settings 
    settings = g_settings_new ("org.y20k.ezeedo");
    g_settings_set_int (settings,
                        "main-window-position-x",
                        x);
    g_settings_set_int (settings,
                        "main-window-position-y",
                        y);
    g_settings_set_int (settings,
                        "main-window-width",
                        width);
    g_settings_set_int (settings,
                        "main-window-height",
                        height);
    g_settings_set_int (settings,
                        "sidebar-size",
                        sidebar_size);
    g_object_unref (settings);

    return;
}


/**
 * Saves filename and location to gsettings
 */
void
save_file_name_location (gchar *file)
{
    GSettings *settings;
    settings = g_settings_new ("org.y20k.ezeedo");

    g_settings_set_string (settings,
                           "todo-txt-file",
                           file);

    g_object_unref (settings);

    return;
}


/**
 * Get current filename and location from gsettings
 */
gchar
*get_current_file_name_location (void)
{
    GSettings *settings;
    gchar *file;
    settings = g_settings_new ("org.y20k.ezeedo");

    file = g_settings_get_string (settings,
                                  "todo-txt-file");

    g_object_unref (settings);

    return (file);
}


/**
 * Saves tasklist to file
 */
void
autosave (ezeedo_wrapper_structure* ezeedo)
{
    gchar *todotxt_file;

    todotxt_file = get_current_file_name_location();

    save_textlist_to_file(ezeedo->textlist, todotxt_file);

    return;
}


/**
 * Terminates application when closing window
 */
void
close_window (GtkWidget *widget,
              GdkEvent  *event,
              gpointer   user_data)
{
    // get ezeedo from user data
    ezeedo_wrapper_structure *ezeedo;
    ezeedo = user_data;

    save_window_position (NULL,
                          NULL,
                          ezeedo);

    g_application_quit (G_APPLICATION(ezeedo->application)); 

    return;
}


/**
 * Terminates application when hitting quit
 */
void
quit_application (GSimpleAction *simple,
                  GVariant      *parameter,
                  gpointer       user_data)
{
    // get ezeedo from user data
    ezeedo_wrapper_structure *ezeedo;
    ezeedo = user_data;

    save_window_position (NULL,
                          NULL,
                          ezeedo);

    g_application_quit (G_APPLICATION(ezeedo->application)); 

    return;
}
