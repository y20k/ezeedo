/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * taskentry.c
 * Implements functions used to process task entry
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


#include "taskentry.h"

#include "helpers.h"
#include "tasklist.h"


/**
 * Handles task entry
 */
void add_task_entry (GtkEntry *entry, gpointer user_data)
{
    ezeedo_wrapper_structure* ezeedo;
    const gchar *input;
    
    ezeedo = user_data;
    input = gtk_entry_get_text (entry);

    // add line to textlist
    bool line_added = add_line_to_textlist(input, ezeedo->textlist);
    // TODO remove
    if (!line_added)
    {
        g_print("Could not add line to textlist:\n%s\n", input);
    }

    // add line to tasklist
    bool task_added = add_task_to_tasklist(input, ezeedo->tasklist, ezeedo->context_list, ezeedo->project_list);
    // TODO remove
    if (!task_added)
    {
        g_print("Could not add line to tasklist:\n%s\n", input);
    }

    // sort tasklist
    sort_tasklist(ezeedo->tasklist);

    // rebuild taskstore
    gtk_list_store_clear (ezeedo->tasks_store);
    fill_tasks_store (ezeedo);

    // clear text entry
    gtk_entry_set_text (entry, "");

    // save to file
    autosave(ezeedo);

    return;
}
