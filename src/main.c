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

    // create convenience wrapper structure for important elements
    ezeedo_wrapper_structure *ezeedo = calloc (1, sizeof(ezeedo_wrapper_structure));
    // TODO free ezeedo structure

    // create a gtk application
    gint           status;
    GtkApplication *application;

    application = gtk_application_new ("org.y20k.ezeedo",
                                       G_APPLICATION_FLAGS_NONE);

    // add application to ezeedo wrapper structure
    ezeedo->application = application;
    
    g_signal_connect (application,
                      "activate",
                      G_CALLBACK (activate),
                      ezeedo);
    g_signal_connect (application,
                      "startup",
                      G_CALLBACK (startup),
                      ezeedo);

    // start application
    status = g_application_run (G_APPLICATION (application),
                                argc,
                                argv);

    // free memory
    g_object_unref (application);

    return status;
}
