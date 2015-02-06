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
 * Creates widget containing categorylist
 */
GtkWidget
*display_category (ezeedo_wrapper_structure *ezeedo,
                   category_container       *category_list,
                   const char               *category_name,
                   gint                      type)
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
 * Shows all tasks and resets any category selections
 */
void
show_all (GtkButton *button,
          gpointer   user_data)
{
    ezeedo_wrapper_structure *ezeedo;
    ezeedo = user_data;

    reset_category_selection (ezeedo,
                              CATEGORYLIST_CONTEXTS);
    reset_category_selection (ezeedo,
                              CATEGORYLIST_PROJECTS);

    GtkTreeModel *filter_todo;
    filter_todo = gtk_tree_model_filter_new (GTK_TREE_MODEL(ezeedo->tasks_store),
                                             NULL);
    gtk_tree_model_filter_set_visible_column (GTK_TREE_MODEL_FILTER(filter_todo),
                                              TASK_NOTCOMPLETED);
 
    // create and display todolist widget
    GtkWidget *filter_todolist;
    GtkWidget *parent;
    parent = gtk_widget_get_parent (ezeedo->todolist);
    gtk_widget_destroy (ezeedo->todolist);

    filter_todolist = display_tasklist (GTK_TREE_MODEL(filter_todo));

    g_signal_connect (filter_todolist, "row-activated",
                      G_CALLBACK(task_doubleclicked), ezeedo);
 
    // add todolist to ezeedo wrapper structure
    ezeedo->todolist = filter_todolist; 

    gtk_container_add (GTK_CONTAINER(parent),
                       filter_todolist);
    gtk_widget_show_all (parent);
 
 
}
