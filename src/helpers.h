/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * helpers.h
 * Declares helper functions
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


#ifndef HELPERS_H
#define HELPERS_H


#include "core.h"

/**
 * Shows line of text within dialog window or in terminal
 */
void
show_info (GtkWidget *widget,
           gchar     *text,
           gboolean   dialog_window);

/**
 * Display simple information dialog with given text
 */
void
display_info_dialog (GtkWidget *widget,
                     gpointer   user_data);

/**
 * Ask user for location of todo.txt file
 */
gchar
*open_file_dialog (GApplication *app);

/**
 * Toggles visibility of sidebar
 */
void
toggle_sidebar (GSimpleAction *simple,
                GVariant      *parameter,
                gpointer       user_data);

/**
 * Saves position and size of given window to gsettings
 */
void
save_window_position (GSimpleAction *simple,
                      GVariant      *parameter,
                      gpointer       user_data);

/**
 * Saves filename and location to gsettings
 */
void
save_file_name_location (gchar *file);

/**
 * Get current filename and location from gsettings
 */
gchar
*get_current_file_name_location (void);

/**
 * Terminates application when hitting quit
 */
void
quit_application (GSimpleAction *simple,
                  GVariant      *parameter,
                  gpointer       user_data);

/**
 * Terminates application when closing window
 */
void
close_window (GtkWidget *widget,
              GdkEvent  *event,
              gpointer   user_data);

/**
 * Saves tasklist to file
 */
void
autosave (ezeedo_wrapper_structure* ezeedo);

/**
 * Handles double-click on task
 */
void
task_doubleclicked (GtkTreeView       *treeview,
                    GtkTreePath       *path,
                    GtkTreeViewColumn *col, 
                    gpointer           user_data);

/**
 * Handles single-click on category
 */
void
category_singleclicked (GtkTreeSelection *category_selection,
                        gpointer          user_data);

/**
 * Resets selection for given category list
 */
void
reset_category_selection (ezeedo_wrapper_structure *ezeedo,
                          gint                      type);


#endif // EZEEDOCALLBACKS_H
