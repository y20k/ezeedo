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
GtkWidget* display_category (category_container* category_list, const char* category_name, gint type)
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
        // acquire an iterator 
        gtk_list_store_append (categories_store, &iter);

        gtk_list_store_set (categories_store, &iter,
                            CATEGORY_ID,    category_list->list[i]->id, 
                            CATEGORY_TITLE, category_list->list[i]->title,
                            CATEGORY_TYPE,  type,
                            -1);
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
