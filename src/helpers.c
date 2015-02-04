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
        display_info_dialog (widget, text);
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
        dialog = gtk_message_dialog_new (GTK_WINDOW(toplevel), flags, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "%s", dialog_message);

        gtk_dialog_run (GTK_DIALOG (dialog));
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
    GtkFileChooserAction action;
    GtkFileFilter        *filter;
    gint                 result;

    // create file chooser dialog
    action = GTK_FILE_CHOOSER_ACTION_OPEN;
    window = gtk_application_get_active_window (GTK_APPLICATION(app));
    dialog = gtk_file_chooser_dialog_new ("Choose Todo.txt File", window, action, "Open", GTK_RESPONSE_ACCEPT, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT);

    
    // create filter for plain text files
    filter = gtk_file_filter_new ();
    gtk_file_filter_set_name (filter, "Todo.txt Tasklist File");
    gtk_file_filter_add_mime_type (filter, "text/plain");
    gtk_file_filter_add_pattern (filter, "*.txt");
    gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (dialog), filter);

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
 * Saves position and size of a given window
 */
void
save_window_position (GSimpleAction *simple,
                      GVariant *parameter,
                      gpointer  user_data)
{
    GtkWindow *window;
    GSettings *settings;
    int x;
    int y;
    int width;
    int height;

    // get window position and size
    window = user_data;
    gtk_window_get_position (GTK_WINDOW(window), &x, &y);
    gtk_window_get_size     (GTK_WINDOW(window), &width, &height);

    // save changes to settings 
    settings = g_settings_new ("org.y20k.ezeedo");

    g_settings_set_int (settings, "main-window-position-x", x);
    g_settings_set_int (settings, "main-window-position-y", y);
    g_settings_set_int (settings, "main-window-width", width);
    g_settings_set_int (settings, "main-window-height", height);

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

    g_settings_set_string (settings, "todo-txt-file", file);

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

    file = g_settings_get_string (settings, "todo-txt-file");

    g_object_unref (settings);

    return (file);
}


/**
 * Terminates application when hitting quit
 */
void
quit_application (GSimpleAction *simple,
                  GVariant      *parameter,
                  gpointer       user_data)
{
    GApplication *app;
    GtkWindow    *win;

    // TODO remove debugging g_print
    g_print ("Quit on quit\n");

    app = user_data;
    win = gtk_application_get_active_window (GTK_APPLICATION(app));
    save_window_position (NULL, NULL, win);

    g_application_quit (app); 

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
    GApplication *app;

    // TODO remove debugging g_print
    g_print ("Quit on close\n");

    app = user_data;
    save_window_position (NULL, NULL, GTK_WINDOW(widget));

    g_application_quit (app); 

    return;
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
 * Handles double-click on task
 */
void
task_doubleclicked (GtkTreeView       *treeview,
                    GtkTreePath       *path,
                    GtkTreeViewColumn *col, 
                    gpointer           user_data)
{
    ezeedo_wrapper_structure* ezeedo;
    GtkTreeIter               filter_iter;
    GtkTreeModel*             filter_model;
    GtkTreeIter               child_iter;
    gchar*                    tasktitle;

    // get ezeedo from user data
    ezeedo = user_data;

    // get filter model from treeview
    filter_model = gtk_tree_view_get_model (treeview);

    // get child model from filter model
    // child_model = gtk_tree_model_filter_get_model    (GTK_TREE_MODEL_FILTER (filter_model));

    // get filter iter from path
    gtk_tree_model_get_iter (filter_model, &filter_iter, path);

    // get child iter from filter iter
    gtk_tree_model_filter_convert_iter_to_child_iter (GTK_TREE_MODEL_FILTER (filter_model), &child_iter, &filter_iter);

    // get task title
    gtk_tree_model_get (filter_model, &filter_iter, TASK_DESCRIPTION, &tasktitle, -1);

    // get toplevel window
    GtkWidget *toplevel = gtk_widget_get_toplevel (GTK_WIDGET(treeview));
    if (gtk_widget_is_toplevel (toplevel))
    {

        // show selection dialog
        GtkWidget *dialog;
        GtkDialogFlags flags;
        gint result = 0;

        flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
        dialog = gtk_message_dialog_new (GTK_WINDOW(toplevel), flags, GTK_MESSAGE_INFO, GTK_BUTTONS_NONE, "Do you want to mark following taks as done?\n\"%s\"", tasktitle);
        gtk_dialog_add_button (GTK_DIALOG (dialog), "Cancel", GTK_RESPONSE_REJECT);
        gtk_dialog_add_button (GTK_DIALOG (dialog), "Mark Done", GTK_RESPONSE_APPLY);
        gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_APPLY);

        result = gtk_dialog_run (GTK_DIALOG (dialog));

        if (result == GTK_RESPONSE_APPLY)
        {
            gint id = -1;
            gtk_tree_model_get (filter_model, &filter_iter, TASK_ID, &id, -1);
            if (id >= 0)
            {
                mark_task_done(id, ezeedo->textlist, ezeedo->tasklist);
                autosave(ezeedo);
            }
            gtk_list_store_set (GTK_LIST_STORE (ezeedo->tasks_store), &child_iter, TASK_COMPLETED, TRUE, TASK_NOTCOMPLETED, FALSE, -1);
            gtk_widget_destroy (dialog);
        }
        else
        {
            gtk_widget_destroy (dialog);
        } 
    }

}


/**
 * Handles single-click on category
 */
void
category_singleclicked (GtkTreeSelection *category_selection,
                        gpointer          user_data)
{
    ezeedo_wrapper_structure* ezeedo;
    GtkTreeModel*             selection_model;
    GtkTreeIter               iter;
    gint                      type;
    gint                      id;
    gint                      selected_rows;
    GtkTreeModel*             category_filter;
    
    // get ezeedo from user data
    ezeedo = user_data;

    // get nunber of selected rows
    selected_rows = gtk_tree_selection_count_selected_rows (GTK_TREE_SELECTION(category_selection));

    if (selected_rows != 0)
    {
        // get input
        gtk_tree_selection_get_selected (GTK_TREE_SELECTION (category_selection), &selection_model, &iter);

        // get category id and type
        gtk_tree_model_get (selection_model, &iter, CATEGORY_ID, &id, -1);
        gtk_tree_model_get (selection_model, &iter, CATEGORY_TYPE, &type, -1);

        // deselect all on other category list
        if (type == CATEGORYLIST_CONTEXTS)
        {
            reset_category_selection(ezeedo, CATEGORYLIST_PROJECTS);
        }
        else if (type == CATEGORYLIST_PROJECTS)
        {
            reset_category_selection(ezeedo, CATEGORYLIST_CONTEXTS);
        }
        else
        {
            return;
        }
        
        // mark tasks as visible
        change_task_visibility(ezeedo, type, id);

        // create tree model filter
        category_filter = gtk_tree_model_filter_new(GTK_TREE_MODEL(ezeedo->tasks_store), NULL );
        gtk_tree_model_filter_set_visible_column(GTK_TREE_MODEL_FILTER(category_filter), TASK_FILTERED);

        // create and display todolist widget
        GtkWidget* filter_todolist;
        GtkWidget* parent;
        parent = gtk_widget_get_parent (ezeedo->todolist);
        gtk_widget_destroy (ezeedo->todolist);

        filter_todolist = display_tasklist (GTK_TREE_MODEL(category_filter));

        g_signal_connect (filter_todolist, "row-activated", G_CALLBACK(task_doubleclicked), ezeedo);
        
        // add todolist to ezeedo wrapper structure
        ezeedo->todolist = filter_todolist;    

        gtk_container_add (GTK_CONTAINER (parent), filter_todolist);
        gtk_widget_show_all (parent);
        
    }
}


/**
 * Resets selection for given category list
 */
void
reset_category_selection (ezeedo_wrapper_structure *ezeedo,
                          gint                      type)
{
    GtkTreeView*      category_list;
    GtkTreeSelection* category_selection;
    gint               selected_rows;

    if (type == CATEGORYLIST_CONTEXTS)
    {
        category_list = GTK_TREE_VIEW(ezeedo->todo_contexts);
    }
    else if (type == CATEGORYLIST_PROJECTS) 
    {
        category_list = GTK_TREE_VIEW(ezeedo->todo_projects);
    }
    else
    {
        return;
    }

    category_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(category_list));
    selected_rows      = gtk_tree_selection_count_selected_rows (GTK_TREE_SELECTION(category_selection));

    if (selected_rows != 0)
    {
        gtk_tree_selection_unselect_all (GTK_TREE_SELECTION(category_selection));
    }
}
