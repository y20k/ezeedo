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
GtkListStore
*fill_category_store (ezeedo_wrapper_structure *ezeedo,
                      category_container       *category_list,
                      gint                      type);

/**
 * Creates widget containing categorylist
 */
GtkWidget
*display_category (ezeedo_wrapper_structure *ezeedo,
                   GtkListStore*             categories_store,
                   const char               *category_name);

/**
 * Creates widget containing a show all option
 */
GtkWidget
*display_show_all (ezeedo_wrapper_structure *ezeedo);

/**
 * Determines if given category contains open tasks
 */
gboolean
category_contains_open_tasks (ezeedo_wrapper_structure *ezeedo,
                              gint                      id,
                              gint                      type);

/**
 * Refreshes contexts and projects display
 */
void
refresh_category_display (ezeedo_wrapper_structure *ezeedo);


#endif // CATEGORYLIST_H
