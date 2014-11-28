/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * tasklist.h
 * Declares functions for the user interface of the main tasklist
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


#ifndef TASKLIST_H
#define TASKLIST_H


#include "core.h"


/**
 * Creates widget containing tasklist
 */
// GtkWidget *display_tasklist (gboolean done, GtkTreeModel *model, tasklist_container* main_tasklist, category_container* context_list, category_container* project_list);
GtkWidget *display_tasklist (gboolean done, GtkTreeModel *model);

/**
 * Creates task store
 */
GtkListStore* create_tasks_store (void);

/**
 * Fills tasks into given task store
 */
void *fill_tasks_store (ezeedo_wrapper_structure* ezeedo);

/**
 * Adds one task to task store
 */
void add_task_to_taskstore (gint task_id, GtkTreeIter iter, ezeedo_wrapper_structure* ezeedo);


#endif // TASKLIST_H
