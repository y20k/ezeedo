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
 * Creates widget containing categorylist
 */
GtkWidget* display_category (ezeedo_wrapper_structure* ezeedo, category_container* category_list, const char* category_name, gint type);

/**
 * Determines if given category contains open tasks
 */
gboolean category_contains_open_tasks(ezeedo_wrapper_structure* ezeedo, gint id, gint type);


#endif // CATEGORYLIST_H
