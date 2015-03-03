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
#include "helpers.h"
#include "tasklist.h"

/**
 * Fills category into given category store
 */
GtkListStore
*fill_category_store (ezeedo_wrapper_structure *ezeedo,
                      category_container       *category_list,
                      gint                      type)
{
    GtkListStore*      categories_store;

    categories_store = gtk_list_store_new (CATEGORY_COLUMNS, // total number of colums
                                           G_TYPE_INT,       // id
                                           G_TYPE_STRING,    // category
                                           G_TYPE_INT);      // type

    // create iterator
    GtkTreeIter iter;

    for (int i = 0; i < category_list->number_of_categories; i++)
    {
        if (category_contains_open_tasks (ezeedo, 
                                          category_list->list[i]->id,
                                          type))
        {
            // acquire an iterator 
            gtk_list_store_append (categories_store,
                                   &iter);

            gtk_list_store_set (categories_store, &iter,
                                CATEGORY_ID,    category_list->list[i]->id, 
                                CATEGORY_TITLE, category_list->list[i]->title,
                                CATEGORY_TYPE,  type,
                                -1);
        }
    }
    return (categories_store);

}


/**
 * Creates widget containing categorylist
 */
GtkWidget
*display_category (ezeedo_wrapper_structure *ezeedo,
                   GtkListStore*             categories_store,
                   const char               *category_name)
{
    GtkWidget*         view;
    GtkTreeViewColumn* col;
    GtkCellRenderer*   renderer;
 
    view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (categories_store));

    // Column 1: category title
    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes (category_name,
                                                    renderer,
                                                    "text", CATEGORY_TITLE,
                                                    NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(view),
                                 col);

    // destroy model automatically with view
    // g_object_unref (tasks_store); 

    GtkTreeSelection *category_selection;
    category_selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(view));
    gtk_tree_selection_set_mode (category_selection,
                                 GTK_SELECTION_BROWSE);


    // detect single-click on contexts
    g_signal_connect (category_selection, "changed",
                      G_CALLBACK(category_singleclicked), ezeedo);
 
    return (view);
}


/**
 * Determines if given category contains open tasks
 */
gboolean
category_contains_open_tasks (ezeedo_wrapper_structure *ezeedo,
                              gint                      id,
                              gint                      type)
{
    for (int i = 0; i < ezeedo->tasklist->number_of_tasks; i++)
    {
        // task has given context and is not completed
        if (type == CATEGORYLIST_CONTEXTS && 
            !ezeedo->tasklist->list[i]->completed &&
            ezeedo->tasklist->list[i]->number_of_contexts != 0 && 
            ezeedo->tasklist->list[i]->context[0] == id)
        {
            return true;
        }
        // task has given project and is not completed
        else if (type == CATEGORYLIST_PROJECTS && 
                 !ezeedo->tasklist->list[i]->completed &&
                 ezeedo->tasklist->list[i]->number_of_projects != 0 && 
                 ezeedo->tasklist->list[i]->project[0] == id)
        {
            return true;
        }
    }
    return false;
}


/**
 * Creates widget containing a show all option
 */
GtkWidget
*display_show_all (ezeedo_wrapper_structure *ezeedo)
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

    // acquire an iterator 
    gtk_list_store_append (categories_store,
                           &iter);

    gtk_list_store_set (categories_store, &iter,
                        CATEGORY_TITLE, "Show all",
                        -1);

    view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (categories_store));

    // Column 1: category title
    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes ("Categories",
                                                    renderer,
                                                    "text", CATEGORY_TITLE,
                                                    NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(view),
                                 col);

    // destroy model automatically with view
    // g_object_unref (tasks_store); 

    GtkTreeSelection *category_selection;
    category_selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(view));
    gtk_tree_selection_set_mode (category_selection,
                                 GTK_SELECTION_BROWSE);

    // detect single-click on contexts
    g_signal_connect (category_selection, "changed",
                      G_CALLBACK(category_singleclicked), ezeedo);

    return (view);
}


/**
 * Refreshes contexts and projects display
 */
void
refresh_category_display (ezeedo_wrapper_structure *ezeedo)
{
    // rebuild contexts-store
/*    gtk_list_store_clear (ezeedo->contexts_store);
    ezeedo->contexts_store = fill_category_store (ezeedo,
                                                  ezeedo->context_list,
                                                  CATEGORYLIST_CONTEXTS);
    gtk_tree_view_set_model (GTK_TREE_VIEW(ezeedo->todo_contexts), 
                             GTK_TREE_MODEL(ezeedo->contexts_store));*/
    // g_object_unref(gtkliststore);

    // rebuild contexts-store
/*    gtk_list_store_clear (ezeedo->projects_store);
    ezeedo->projects_store = fill_category_store (ezeedo,
                                                  ezeedo->project_list,
                                                  CATEGORYLIST_PROJECTS);*/
    gtk_tree_view_set_model (GTK_TREE_VIEW(ezeedo->todo_projects), 
                             GTK_TREE_MODEL(ezeedo->projects_store));
    // g_object_unref(gtkliststore);

    return;
}