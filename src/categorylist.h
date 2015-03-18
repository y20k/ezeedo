/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * categorylist.h
 * Declares user interface for categories (projects and contexts)
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


#ifndef CATEGORYLIST_H
#define CATEGORYLIST_H


#include "core.h"


/**
 * Fills category into given category store
 */
GtkTreeModel
*fill_category_store (ezeedo_wrapper_structure *ezeedo,
                      category_container       *category_list,
                      gint                      type);

/**
 * Creates widget containing a show all option
 */
GtkWidget
*build_show_all (ezeedo_wrapper_structure *ezeedo);

/**
 * Creates widget containing categorylist
 */
GtkWidget
*build_categorylist (ezeedo_wrapper_structure *ezeedo,
                     GtkTreeModel             *model,
                     const char               *name);

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

/**
 * Refreshes contexts and projects display
 */
void
refresh_category_display (ezeedo_wrapper_structure *ezeedo);

/**
 * Decreases open task counter for contexts and projects
 */
void
decrease_open_tasks (ezeedo_wrapper_structure *ezeedo,
                     gint                      context_id,
                     gint                      project_id);

/**
 * Debug
 */
void
debug_category_lists (ezeedo_wrapper_structure *ezeedo);


#endif // CATEGORYLIST_H
