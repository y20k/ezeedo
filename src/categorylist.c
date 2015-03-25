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
GtkTreeModel
*fill_category_store (ezeedo_wrapper_structure *ezeedo,
                      category_container       *category_list,
                      gint                      type)
{
    GtkListStore *categories_store;
    GtkTreeIter   iter;
    GtkTreeModel *filter;

    categories_store = gtk_list_store_new (CATEGORY_COLUMNS, // total number of colums
                                           G_TYPE_INT,       // id
                                           G_TYPE_STRING,    // category
                                           G_TYPE_INT,       // type
                                           G_TYPE_INT,       // open tasks
                                           G_TYPE_BOOLEAN);  // filtered

    // traverse category list
    for (int i = 0; i < category_list->number_of_categories; i++)
    {
        // set filter for categories containing open tasks to true
        if (category_list->list[i]->open_tasks > 0)
        {
            // acquire an iterator 
            gtk_list_store_append (categories_store,
                                   &iter);
            // add category to store and mark filtered
            gtk_list_store_set (categories_store,
                                &iter,
                                CATEGORY_ID,       category_list->list[i]->id, 
                                CATEGORY_TITLE,    category_list->list[i]->title,
                                CATEGORY_TYPE,     type,
                                CATEGORY_OPEN,     category_list->list[i]->open_tasks,
                                CATEGORY_FILTERED, true,
                                -1);
        }
        // set filter for categories containing only done tasks to false
        else
        {
            // acquire an iterator 
            gtk_list_store_append (categories_store,
                                   &iter);
            // add category to store and mark not filtered
            gtk_list_store_set (categories_store,
                                &iter,
                                CATEGORY_ID,       category_list->list[i]->id, 
                                CATEGORY_TITLE,    category_list->list[i]->title,
                                CATEGORY_TYPE,     type,
                                CATEGORY_OPEN,     category_list->list[i]->open_tasks,
                                CATEGORY_FILTERED, false,
                                -1);
        }
    }

    // set filter column
    filter = gtk_tree_model_filter_new (GTK_TREE_MODEL(categories_store),
                                        NULL);
    gtk_tree_model_filter_set_visible_column (GTK_TREE_MODEL_FILTER(filter),
                                              CATEGORY_FILTERED);

    // put categories store into ezeedo wrapper structure
    if (type == CATEGORYLIST_CONTEXTS)
    {
        ezeedo->contexts_store = categories_store;
    }
    else
    {
        ezeedo->projects_store = categories_store;
    }

    return (filter);
}


/**
 * Creates widget containing a show all option
 */
GtkWidget
*build_show_all (ezeedo_wrapper_structure *ezeedo)
{
    GtkWidget*         view;
    GtkListStore*      categories_store;
    GtkTreeViewColumn* col;
    GtkCellRenderer*   renderer;
 
    categories_store = gtk_list_store_new (CATEGORY_COLUMNS, // total number of colums
                                           G_TYPE_INT,       // id
                                           G_TYPE_STRING,    // category
                                           G_TYPE_INT,       // type
                                           G_TYPE_INT,       // open tasks
                                           G_TYPE_BOOLEAN);  // filtered

    // create iterator
    GtkTreeIter iter;

    // appends new row to categories store and acquire iterator 
    gtk_list_store_append (categories_store,
                           &iter);

    gtk_list_store_set (categories_store,
                        &iter,
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

    // dereference model with view
    g_object_unref (categories_store); 

    gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW(view)),
                                 GTK_SELECTION_SINGLE);

    // detect single-click on contexts
    g_signal_connect (gtk_tree_view_get_selection (GTK_TREE_VIEW(view)), "changed",
                      G_CALLBACK(category_singleclicked), ezeedo);

    return (view);
}


/**
 * Creates widget containing categorylist
 */
GtkWidget
*build_categorylist (ezeedo_wrapper_structure *ezeedo,
                     GtkTreeModel             *model,
                     const char               *name)
{
    GtkWidget         *view;
    GtkTreeViewColumn *col;
    GtkCellRenderer   *renderer;
    GtkTreeSelection  *category_selection;

    view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));

    // Column 1: category title
    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes (name,
                                                    renderer,
                                                    "text", CATEGORY_TITLE,
                                                    NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(view),
                                 col);

    // dereference model with view
    g_object_unref (model); 

    category_selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(view));
    gtk_tree_selection_set_mode (category_selection,
                                 GTK_SELECTION_SINGLE);

    // detect single-click on category
    g_signal_connect (category_selection, "changed",
                      G_CALLBACK(category_singleclicked), ezeedo);
    
    return (view);
}


/**
 * Handles single-click on category
 */
void
category_singleclicked (GtkTreeSelection *category_selection,
                        gpointer          user_data)
{
    ezeedo_wrapper_structure *ezeedo;
    GtkTreeModel             *selection_model;
    GtkTreeIter               iter;
    gint                      type;
    gint                      id;
    gint                      selected_rows;
    
    // get ezeedo from user data
    ezeedo = user_data;

    // get nunber of selected rows
    selected_rows = gtk_tree_selection_count_selected_rows (GTK_TREE_SELECTION(category_selection));

    // check if any row selected
    if (selected_rows != 0)
    {
        // get input
        gtk_tree_selection_get_selected (GTK_TREE_SELECTION(category_selection),
                                         &selection_model,
                                         &iter);

        // get category id and type
        gtk_tree_model_get (selection_model,
                            &iter,
                            CATEGORY_ID, &id,
                            CATEGORY_TYPE, &type,
                            -1);

        // show tasklist
        show_tasklist (ezeedo,
                       type,
                       id);
    }
}


/**
 * Resets selection for given category list
 */
void
reset_category_selection (ezeedo_wrapper_structure *ezeedo,
                          gint                      type)
{
    GtkTreeView      *category_list;
    GtkTreeSelection *category_selection;
    gint              selected_rows;

    if (type == CATEGORYLIST_CONTEXTS)
    {
        category_list = GTK_TREE_VIEW(ezeedo->todo_contexts);
    }
    else if (type == CATEGORYLIST_PROJECTS) 
    {
        category_list = GTK_TREE_VIEW(ezeedo->todo_projects);
    }
    else if (type == CATEGORYLIST_ALL) 
    {
        category_list = GTK_TREE_VIEW(ezeedo->todo_showall);
    }
    else
    {
        return;
    }

    category_selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(category_list));
    selected_rows = gtk_tree_selection_count_selected_rows (GTK_TREE_SELECTION(category_selection));

    if (selected_rows != 0)
    {
        gtk_tree_selection_unselect_all (GTK_TREE_SELECTION(category_selection));
    }
}


/**
 * Refreshes contexts and projects display
 */
void
refresh_category_display (ezeedo_wrapper_structure *ezeedo)
{
    GtkTreeModel *filter_contexts;
    GtkTreeModel *filter_projects;

    gtk_list_store_clear (ezeedo->contexts_store);
    filter_contexts = fill_category_store (ezeedo,
                                           ezeedo->context_list,
                                           CATEGORYLIST_CONTEXTS);

    gtk_list_store_clear (ezeedo->projects_store);
    filter_projects = fill_category_store (ezeedo,
                                           ezeedo->project_list,
                                           CATEGORYLIST_PROJECTS);

    // rebuild contexts-store
    gtk_tree_view_set_model (GTK_TREE_VIEW(ezeedo->todo_contexts), 
                             GTK_TREE_MODEL(filter_contexts));

    // rebuild contexts-store
    gtk_tree_view_set_model (GTK_TREE_VIEW(ezeedo->todo_projects), 
                             GTK_TREE_MODEL(filter_projects));

    return;
}


/**
 * Decreases open task counter for contexts and projects
 */
void
decrease_open_tasks (ezeedo_wrapper_structure *ezeedo,
                     gint context_id,
                     gint project_id)
{
    GtkTreeIter iter;
    gboolean    valid;

    // decrease open tasks count in project list
    if (context_id != -1 &&
        ezeedo->context_list->list[context_id]->open_tasks > 0)
    {
        printf ("Context -> Open Tasks: %d\n", ezeedo->context_list->list[context_id]->open_tasks);
        ezeedo->context_list->list[context_id]->open_tasks--;
    }

    // decrease open tasks count in project list
    if (project_id != -1 &&
        ezeedo->project_list->list[project_id]->open_tasks > 0)
    {
        printf ("Project -> Open Tasks: %d\n", ezeedo->context_list->list[project_id]->open_tasks);
        ezeedo->project_list->list[project_id]->open_tasks--;
    }

   // traverse trough context store
    valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(ezeedo->contexts_store), &iter);
    while (valid)
    {
        gint id;
        // get category id
        gtk_tree_model_get (GTK_TREE_MODEL(ezeedo->contexts_store),
                            &iter,
                            CATEGORY_ID, &id,
                            -1);

        // found matching id in contexts store
        if (context_id == id)
        {
            gtk_list_store_set (GTK_LIST_STORE(ezeedo->contexts_store),
                                &iter,
                                CATEGORY_OPEN, ezeedo->context_list->list[context_id]->open_tasks,
                                -1);
        }

        // look for valid context with zero open taks
        if (context_id != -1 &&
            ezeedo->context_list->list[context_id]->open_tasks == 0)
        {
            gtk_list_store_set (GTK_LIST_STORE(ezeedo->contexts_store),
                                &iter,
                                CATEGORY_FILTERED, false,
                                -1);
        }

        // increment iter
        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(ezeedo->contexts_store), &iter);
    }

    // traverse trough projects store
    valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(ezeedo->projects_store), &iter);
    while (valid)
    {
        gint id;
        // get category id
        gtk_tree_model_get (GTK_TREE_MODEL(ezeedo->projects_store),
                            &iter,
                            CATEGORY_ID, &id,
                            -1);

        // found matching id in contexts store
        if (project_id == id)
        {
            gtk_list_store_set (GTK_LIST_STORE(ezeedo->projects_store),
                                &iter,
                                CATEGORY_OPEN, ezeedo->project_list->list[project_id]->open_tasks,
                                -1);
        }

        // look for valid project with zero open taks
        if (project_id != -1 &&
            ezeedo->project_list->list[project_id]->open_tasks == 0)
        {
            gtk_list_store_set (GTK_LIST_STORE(ezeedo->projects_store),
                                &iter,
                                CATEGORY_FILTERED, false,
                                -1);
        }

        // increment iter
        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(ezeedo->projects_store), &iter);
    }

    return;
}


/**
 * Debug
 */
void
debug_category_lists (ezeedo_wrapper_structure *ezeedo)
{
    GtkTreeIter iter;
    gboolean    valid;

    for (int i = 0; i < ezeedo->context_list->number_of_categories; i++)
    {
        char text[INFODIALOGLENGTH];
        snprintf (text, INFODIALOGLENGTH,"struct-context: %s open tasks: %d",
                  ezeedo->context_list->list[i]->title,
                  ezeedo->context_list->list[i]->open_tasks);
        show_info (NULL,
                   text,
                   false);
    }
    for (int i = 0; i < ezeedo->project_list->number_of_categories; i++)
    {
        char text[INFODIALOGLENGTH];
        snprintf (text, INFODIALOGLENGTH,"stuct-project: %s open tasks: %d",
                  ezeedo->project_list->list[i]->title,
                  ezeedo->project_list->list[i]->open_tasks);
        show_info (NULL,
                   text,
                   false);
    }

    // traverse trough context store
    valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(ezeedo->contexts_store),
                                          &iter);
    while (valid)
    {
        gint  open;
        char *title;
        gtk_tree_model_get (GTK_TREE_MODEL(ezeedo->contexts_store),
                            &iter,
                            CATEGORY_TITLE, &title,
                            CATEGORY_OPEN,  &open,
                            -1);

        char text[INFODIALOGLENGTH];
        snprintf (text, INFODIALOGLENGTH,"store-context: %s open tasks: %d", title, open);
        show_info (NULL,
                   text,
                   false);

        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(ezeedo->contexts_store), &iter);
    }

    // traverse trough projects store
    valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(ezeedo->projects_store),
                                          &iter);
    while (valid)
    {
        gint  open;
        char *title;
        // get category id and title
        gtk_tree_model_get (GTK_TREE_MODEL(ezeedo->projects_store),
                            &iter,
                            CATEGORY_TITLE, &title,
                            CATEGORY_OPEN,  &open,
                            -1);

        char text[INFODIALOGLENGTH];
        snprintf (text, INFODIALOGLENGTH,"store-project: %s open tasks: %d", title, open);
        show_info (NULL,
                   text,
                   false);

        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(ezeedo->projects_store), &iter);
    }

    return;
}