/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * categorylist.c
 * Implements user interface for categories (projects and contexts)
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


#include "categorylist.h"


/**
 * Creates widget containing categorylist
 */
GtkWidget* display_category (ezeedo_wrapper_structure* ezeedo, category_container* category_list, const char* category_name, gint type)
{
    GtkWidget*         view;
    GtkListStore*      categories_store;
    GtkTreeViewColumn* col;
    GtkCellRenderer*   renderer;
   

    categories_store = gtk_list_store_new (CATEGORY_COLUMNS, // total number of colums
                                           G_TYPE_INT,       // id
                                           G_TYPE_STRING,    // category
                                           G_TYPE_INT);      // type


    // create iterator
    GtkTreeIter iter;

    for (int i = 0; i < category_list->number_of_categories; i++)
    {
        if (category_contains_open_tasks(ezeedo, category_list->list[i]->id, type))
        //if (1)

        {
            // acquire an iterator 
            gtk_list_store_append (categories_store, &iter);

            gtk_list_store_set (categories_store, &iter,
                                CATEGORY_ID,    category_list->list[i]->id, 
                                CATEGORY_TITLE, category_list->list[i]->title,
                                CATEGORY_TYPE,  type,
                                -1);
        }
    }

    view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (categories_store));

    // Column 1: category title
    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes (category_name, renderer, "text", CATEGORY_TITLE, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);

    // destroy model automatically with view
    // g_object_unref (tasks_store); 

    gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (view)), GTK_SELECTION_SINGLE);

    return (view);
}


/**
 * Determines if given category contains open tasks
 */
gboolean category_contains_open_tasks(ezeedo_wrapper_structure* ezeedo, gint id, gint type)
{
    for (int i = 0; i < ezeedo->tasklist->number_of_tasks; i++)
    {
        // task has given context and is not completed
        if (type == CATEGORYLIST_CONTEXTS && 
            !ezeedo->tasklist->list[i]->completed &&
            ezeedo->tasklist->list[i]->number_of_contexts != 0 && 
            ezeedo->tasklist->list[i]->context[0] == id)
        {
            return TRUE;
        }
        // task has given project and is not completed
        else if (type == CATEGORYLIST_PROJECTS && 
                 !ezeedo->tasklist->list[i]->completed &&
                 ezeedo->tasklist->list[i]->number_of_projects != 0 && 
                 ezeedo->tasklist->list[i]->project[0] == id)
        {
            return TRUE;
        }
    }
    return FALSE;
}