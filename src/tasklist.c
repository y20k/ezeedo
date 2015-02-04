/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * tasklist.c
 * Implements functions for the user interface of the main tasklist
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


#include "tasklist.h"


/**
 * Creates widget containing tasklist
 */
GtkWidget 
*display_tasklist (GtkTreeModel *model)
{
    GtkWidget *view; 
    // GtkListStore *tasks_store;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;

    view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));

    // Column 1: Priority
    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes ("Priority", renderer, "text", TASK_PRIORITY, NULL);
    gtk_tree_view_column_set_resizable (col, TRUE);
    gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);

    // Column 2: Description
    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes ("Description", renderer, "text", TASK_DESCRIPTION, NULL);
    gtk_tree_view_column_set_resizable (col, TRUE);
    gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);

    // Column 3: Project
    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes ("Project", renderer, "text", TASK_PROJECT, NULL);
    gtk_tree_view_column_set_resizable (col, TRUE);
    gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);

    // Column 4: Context
    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes ("Context", renderer, "text", TASK_CONTEXT, NULL);
    gtk_tree_view_column_set_resizable (col, TRUE);
    gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);

    // destroy model automatically with view
    // g_object_unref (tasks_store); 

    gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (view)), GTK_SELECTION_SINGLE);

    return (view);
}


/**
 * Creates task store
 */
GtkListStore 
*create_tasks_store (void)
{
    GtkListStore *tasks_store;
    tasks_store = gtk_list_store_new (TASK_COLUMNS,    // total number of colums
                                      G_TYPE_INT,      // id
                                      G_TYPE_STRING,   // priority
                                      G_TYPE_STRING,   // description
                                      G_TYPE_STRING,   // context
                                      G_TYPE_STRING,   // project
                                      G_TYPE_BOOLEAN,  // completed
                                      G_TYPE_BOOLEAN,  // notcompleted
                                      G_TYPE_BOOLEAN); // filtered


    return (tasks_store);
} 


/**
 * Fills tasks into given task store
 */
void
*fill_tasks_store (ezeedo_wrapper_structure *ezeedo)
{

    GtkListStore *tasks_store;
    tasks_store = create_tasks_store ();

    // create iterator
    GtkTreeIter iter;

    for (int i = 0; i < ezeedo->tasklist->number_of_tasks; i++)
    {
        add_task_to_taskstore (i, iter, ezeedo);
    }

    return (tasks_store);
}


/**
 * Adds one task to task store
 */
void
add_task_to_taskstore (gint                      task_id,
                       GtkTreeIter               iter,
                       ezeedo_wrapper_structure *ezeedo)
{
    gchar task_priority_symbol [7]; // TODO Length?
    gchar context[WORDLENGTH+1];
    gchar project[WORDLENGTH+1];

    // set symbol for task priority
    if (ezeedo->tasklist->list[task_id]->priority == 65)
    {
        // Circled Capital Letter A
        strcpy (task_priority_symbol, "\u24B6");
    }
    else if (ezeedo->tasklist->list[task_id]->priority == 66)
    {
        // Circled Capital Letter B
        strcpy (task_priority_symbol, "\u24B7");
    }
    else if (ezeedo->tasklist->list[task_id]->priority == 67)
    {
        // Circled Capital Letter C
        strcpy (task_priority_symbol, "\u24B8");
    }
    else if (ezeedo->tasklist->list[task_id]->priority == 68)
    {
        // Circled Capital Letter D
        strcpy (task_priority_symbol, "\u24B9");
    }
    else
    {
        strcpy (task_priority_symbol, "\u00A0");
    }

    // set project
    if (ezeedo->tasklist->list[task_id]->number_of_projects == 0)
    {
        strcpy (project, "\u00A0");
    }
    else
    {
        strcpy (project, ezeedo->project_list->list[ezeedo->tasklist->list[task_id]->project[0]]->title);
    }

    // set context
    if (ezeedo->tasklist->list[task_id]->number_of_contexts == 0)
    {
        strcpy (context, "\u00A0");
    }
    else
    {
        strcpy (context, ezeedo->context_list->list[ezeedo->tasklist->list[task_id]->context[0]]->title);
    }

    // acquire an iterator 
    gtk_list_store_append (GTK_LIST_STORE(ezeedo->tasks_store), &iter);

    gtk_list_store_set (GTK_LIST_STORE(ezeedo->tasks_store), &iter,
                        TASK_ID,           ezeedo->tasklist->list[task_id]->id, 
                        TASK_PRIORITY,     task_priority_symbol,
                        TASK_DESCRIPTION,  ezeedo->tasklist->list[task_id]->description,
                        TASK_CONTEXT,      context, 
                        TASK_PROJECT,      project, 
                        TASK_COMPLETED,    ezeedo->tasklist->list[task_id]->completed,
                        TASK_NOTCOMPLETED, ! (ezeedo->tasklist->list[task_id]->completed),
                        TASK_FILTERED,     FALSE,
                        -1);
    return;
}


/**
 * Changes visibility column of task list for given parameters
 */
void
change_task_visibility (ezeedo_wrapper_structure *ezeedo,
                        gint                      category_type,
                        gint                      category_id)
{
    GtkTreeIter iter;

    // get first row
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(ezeedo->tasks_store), &iter);
 
    for (int i = 0; i < ezeedo->tasklist->number_of_tasks; i++)
    {
        // exclude completed tasks and non-categorized ones
        if (((ezeedo->tasklist->list[i]->number_of_contexts != 0) || (ezeedo->tasklist->list[i]->number_of_projects != 0)) &&            (!ezeedo->tasklist->list[i]->completed))
        {
            // if given context matches set filtered flag
            if ((category_type == CATEGORYLIST_CONTEXTS) && (category_id == ezeedo->tasklist->list[i]->context[0]))
            {
                gtk_list_store_set (GTK_LIST_STORE(ezeedo->tasks_store), &iter, TASK_FILTERED, TRUE, -1);
            }
            // if given project matches set filtered flag
            else if ((category_type == CATEGORYLIST_PROJECTS) && (category_id == ezeedo->tasklist->list[i]->project[0]))
            {
                gtk_list_store_set (GTK_LIST_STORE(ezeedo->tasks_store), &iter, TASK_FILTERED, TRUE, -1);
            }
            else 
            {
                gtk_list_store_set (GTK_LIST_STORE(ezeedo->tasks_store), &iter, TASK_FILTERED, FALSE, -1);
            }
        }

        // make iter point to next row
        gtk_tree_model_iter_next (GTK_TREE_MODEL(ezeedo->tasks_store), &iter);
    }
    return;
}
