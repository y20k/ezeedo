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

#include "categorylist.h"
#include "core.h"
#include "helpers.h"
#include "tasklist.h"


/**
 * Handles task entry
 */
void
add_task_entry (GtkEntry *entry,
                gpointer  user_data)
{
    ezeedo_wrapper_structure *ezeedo;
    const gchar              *input;

    // get ezeedo from user data
    ezeedo = user_data;

    // get input from form
    input = gtk_entry_get_text (entry);

    // add line to textlist
    bool line_added = add_line_to_textlist (input,
                                            ezeedo->textlist);
    if (!line_added)
    {
        char text[INFODIALOGLENGTH];
        snprintf (text, INFODIALOGLENGTH,"Could not add input to textlist.\n%s", input);
        show_info (NULL,
                   text,
                   false);
    }

    // add line to tasklist
    bool task_added = add_task_to_tasklist (input,
                                            ezeedo->tasklist,
                                            ezeedo->context_list,
                                            ezeedo->project_list);
    if (!task_added)
    {
        char text[INFODIALOGLENGTH];
        snprintf (text, INFODIALOGLENGTH,"Could not add input to tasklist.\n%s", input);
        show_info (NULL,
                   text,
                   false);
    }

    // sort tasklist
    sort_tasklist (ezeedo->tasklist);

    // rebuild taskstore
    gtk_list_store_clear (ezeedo->tasks_store);
    fill_tasks_store (ezeedo);

/*    // rebuild contexts-store
    ezeedo->contexts_store = fill_category_store (ezeedo,
                                                  ezeedo->context_list,
                                                  CATEGORYLIST_CONTEXTS);
    gtk_tree_view_set_model (GTK_TREE_VIEW(ezeedo->todo_contexts), 
                             GTK_TREE_MODEL(ezeedo->contexts_store));
    // g_object_unref(gtkliststore);

    // rebuild contexts-store
    ezeedo->projects_store = fill_category_store (ezeedo,
                                                  ezeedo->project_list,
                                                  CATEGORYLIST_PROJECTS);
    gtk_tree_view_set_model (GTK_TREE_VIEW(ezeedo->todo_projects), 
                             GTK_TREE_MODEL(ezeedo->projects_store));
    // g_object_unref(gtkliststore);*/

    // refresh display of contexts and projects
    refresh_category_display (ezeedo);

    // clear text entry
    gtk_entry_set_text (entry,
                        "");

    // save to file
    autosave (ezeedo);

    return;
}