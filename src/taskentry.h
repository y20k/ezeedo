/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * taskentry.h
 * Declares functions used to process task entry
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


#ifndef TASKENTRY_H
#define TASKENTRY_H


#include "core.h"


/**
 * Handles task entry
 */
void
add_task_entry (GtkEntry *entry,
                gpointer  user_data);


#endif // TASKENTRY_H
