/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * startup.h
 * Declares functions used by the main loop 
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


#ifndef STARTUP_H
#define STARTUP_H


#include "core.h"


/**
 * Shows default first window of Ezeedo
 */
void activate(GtkApplication* app, gpointer user_data);

/**
 * Sets up the application when it first starts
 */
void startup(GApplication *app, gpointer user_data);

/**
 * Shows the about window
 */
void show_about_window (GSimpleAction *simple, GVariant *parameter, gpointer user_data);


#endif // STARTUP_H