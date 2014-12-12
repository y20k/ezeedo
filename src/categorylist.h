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
GtkWidget* display_category (category_container* category_list, const char* category_name, gint type);


#endif // CATEGORYLIST_H
