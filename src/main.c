/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * main.c
 * the main programm
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


#include "core.h"
#include "start.h"


/**
 * Kick it off
 */
int
main (int   argc,
      char *argv[])
{

    // create a gtk application
    int status;
    GtkApplication *app;
    app = gtk_application_new ("org.y20k.ezeedo", G_APPLICATION_FLAGS_NONE);

    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    g_signal_connect (app, "startup", G_CALLBACK (startup), NULL);

    // start application
    status = g_application_run (G_APPLICATION (app), argc, argv);

    // free memory
    g_object_unref (app);

    return status;
}
