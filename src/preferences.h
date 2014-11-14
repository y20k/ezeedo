/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * preferences.h
 * Declares functions used to access and change settings
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


#ifndef PREFERENCES_H
#define PREFERENCES_H


#include "core.h"


/**
 * Shows the preferences window
 */
void show_preferences_dialog (GSimpleAction *simple, GVariant *parameter, gpointer user_data);

/**
 * Select file and save it to settings
 */
void select_and_save_file (GtkButton *button, gpointer user_data);



#endif // PREFERENCES_H
