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

#include "categorylist.h"
#include "helpers.h"


/**
 * Creates tasks store
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
                                      G_TYPE_INT,      // context id
                                      G_TYPE_STRING,   // project
                                      G_TYPE_INT,      // project id
                                      G_TYPE_BOOLEAN,  // completed
                                      G_TYPE_BOOLEAN,  // notcompleted
                                      G_TYPE_BOOLEAN); // filtered

    return (tasks_store);
} 


/**
 * Fills tasks into given task store
 */
void
fill_tasks_store (ezeedo_wrapper_structure *ezeedo)
{
    // create iterator
    GtkTreeIter iter;

    for (int i = 0; i < ezeedo->tasklist->number_of_tasks; i++)
    {
        add_task_to_taskstore (i,
                               iter,
                               ezeedo);
    }

    return;
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
    gint  context_id;
    gchar project[WORDLENGTH+1];
    gint  project_id;

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

    // set context
    if (ezeedo->tasklist->list[task_id]->number_of_contexts == 0)
    {
        strcpy (context, "\u00A0");
        context_id = -1;
    }
    else
    {
        strcpy (context, ezeedo->context_list->list[ezeedo->tasklist->list[task_id]->context[0]]->title);
        context_id = ezeedo->tasklist->list[task_id]->context[0];
    }

    // set project
    if (ezeedo->tasklist->list[task_id]->number_of_projects == 0)
    {
        strcpy (project, "\u00A0");
        project_id = -1;
    }
    else
    {
        strcpy (project, ezeedo->project_list->list[ezeedo->tasklist->list[task_id]->project[0]]->title);
        project_id = ezeedo->tasklist->list[task_id]->project[0];
    }

    // acquire an iterator 
    gtk_list_store_append (GTK_LIST_STORE(ezeedo->tasks_store),
                           &iter);

    gtk_list_store_set (GTK_LIST_STORE(ezeedo->tasks_store), &iter,
                        TASK_ID,           ezeedo->tasklist->list[task_id]->id, 
                        TASK_PRIORITY,     task_priority_symbol,
                        TASK_DESCRIPTION,  ezeedo->tasklist->list[task_id]->description,
                        TASK_CONTEXT,      context, 
                        TASK_CONTEXT_ID,   context_id, 
                        TASK_PROJECT,      project, 
                        TASK_PROJECT_ID,   project_id, 
                        TASK_COMPLETED,    ezeedo->tasklist->list[task_id]->completed,
                        TASK_NOTCOMPLETED, !(ezeedo->tasklist->list[task_id]->completed),
                        TASK_FILTERED,     false,
                        -1);
    return;
}


/**
 * Creates widget containing tasklist
 */
GtkWidget 
*build_tasklist (ezeedo_wrapper_structure *ezeedo,
                 GtkTreeModel             *model)
{
    GtkWidget         *view; 
    GtkTreeViewColumn *col;
    GtkCellRenderer   *renderer;

    view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
    gtk_widget_set_vexpand (view,
                            true);

    // Column 1: Priority
    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes ("Priority",
                                                    renderer,
                                                    "text", TASK_PRIORITY,
                                                    NULL);
    gtk_tree_view_column_set_resizable (col,
                                        true);
    gtk_tree_view_append_column (GTK_TREE_VIEW(view),
                                 col);

    // Column 2: Description
    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes ("Description",
                                                    renderer,
                                                    "text", TASK_DESCRIPTION,
                                                    NULL);
    gtk_tree_view_column_set_resizable (col,
                                        true);
    gtk_tree_view_append_column (GTK_TREE_VIEW(view),
                                 col);

    // Column 3: Project
    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes ("Project",
                                                    renderer,
                                                    "text", TASK_PROJECT,
                                                    NULL);
    gtk_tree_view_column_set_resizable (col, true);
    gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);

    // Column 4: Context
    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes ("Context",
                                                    renderer,
                                                    "text", TASK_CONTEXT,
                                                    NULL);
    gtk_tree_view_column_set_resizable (col,
                                        true);
    gtk_tree_view_append_column (GTK_TREE_VIEW(view),
                                 col);

    // dereference model with view
    g_object_unref (model); 

    gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW(view)),
                                 GTK_SELECTION_SINGLE);

    // detect double-click on todolist
    g_signal_connect (view, "row-activated",
                      G_CALLBACK(task_doubleclicked), ezeedo);

    return (view);
}


/**
 * Handles double-click on task
 */
void
task_doubleclicked (GtkTreeView       *treeview,
                    GtkTreePath       *path,
                    GtkTreeViewColumn *col, 
                    gpointer           user_data)
{
    // get ezeedo from user data
    ezeedo_wrapper_structure *ezeedo;
    ezeedo = user_data;

    // do nothing if donelist
    if (treeview == GTK_TREE_VIEW(ezeedo->donelist))
    {
        return;
    }

    GtkTreeIter   filter_iter;
    GtkTreeModel *filter_model;
    GtkTreeIter   child_iter;
    gchar        *tasktitle;

    GtkWidget *dialog;
    GtkDialogFlags flags;
    gint result;


    // get filter model from treeview
    filter_model = gtk_tree_view_get_model (treeview);

    // get filter iter from path
    gtk_tree_model_get_iter (filter_model,
                             &filter_iter,
                             path);

    // get child iter from filter iter
    gtk_tree_model_filter_convert_iter_to_child_iter (GTK_TREE_MODEL_FILTER (filter_model),
                                                      &child_iter,
                                                      &filter_iter);

    // get task title
    gtk_tree_model_get (filter_model,
                        &filter_iter,
                        TASK_DESCRIPTION, &tasktitle,
                        -1);


    // show selection dialog
    flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_message_dialog_new (GTK_WINDOW(ezeedo->window),
                                     flags,
                                     GTK_MESSAGE_INFO,
                                     GTK_BUTTONS_NONE,
                                     "Do you want to mark following taks as done?\n\"%s\"", tasktitle);
    gtk_dialog_add_button (GTK_DIALOG (dialog),
                           "Cancel", GTK_RESPONSE_REJECT);
    gtk_dialog_add_button (GTK_DIALOG (dialog),
                           "Mark Done", GTK_RESPONSE_APPLY);
    gtk_dialog_set_default_response (GTK_DIALOG (dialog),
                                     GTK_RESPONSE_APPLY);
    result = gtk_dialog_run (GTK_DIALOG (dialog));

    if (result == GTK_RESPONSE_APPLY)
    {
        // get task id
        gint task_id;
        gtk_tree_model_get (filter_model,
                            &filter_iter,
                            TASK_ID,         &task_id,
                            -1);

        if (task_id >= 0)
        {
            // get ids for context and project
            gint context_id;
            gint project_id;
            gtk_tree_model_get (filter_model,
                                &filter_iter,
                                TASK_CONTEXT_ID, &context_id, 
                                TASK_PROJECT_ID, &project_id, 
                                -1);

            // mark task done 
            mark_task_done (task_id,
                            ezeedo->textlist,
                            ezeedo->tasklist);
            gtk_list_store_set (GTK_LIST_STORE(ezeedo->tasks_store),
                                &child_iter,
                                TASK_COMPLETED, true,
                                TASK_NOTCOMPLETED, false,
                                -1);

            // decrease open task counters
            decrease_open_tasks (ezeedo,
                                 context_id,
                                 project_id);
            
            // refresh display of contexts and projects
            refresh_category_display (ezeedo);

            // save to file
            autosave (ezeedo);
        }

        gtk_widget_destroy (dialog);
    }
    else
    {
        gtk_widget_destroy (dialog);
    }

    return;
}


/**
 * Changes visibility column of task list for given parameters
 */
void
change_task_visibility (ezeedo_wrapper_structure *ezeedo,
                        gint                      type,
                        gint                      id)
{
    GtkTreeIter iter;
    gboolean    valid;
    gint        i = 0;

    // get first row
    valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL(ezeedo->tasks_store),
                                           &iter);
    while (valid)
    {

        // if given context matches set filtered flag
        if (type == CATEGORYLIST_CONTEXTS &&
            id   == ezeedo->tasklist->list[i]->context[0] &&
            ezeedo->tasklist->list[i]->number_of_contexts != 0 &&
            !ezeedo->tasklist->list[i]->completed)
        {
            gtk_list_store_set (GTK_LIST_STORE(ezeedo->tasks_store),
                                &iter,
                                TASK_FILTERED, true,
                                -1);
        }

        // if given project matches set filtered flag
        else if (type == CATEGORYLIST_PROJECTS &&
                 id   == ezeedo->tasklist->list[i]->project[0] &&
                 ezeedo->tasklist->list[i]->number_of_projects != 0 &&
                 !ezeedo->tasklist->list[i]->completed)
        {
            gtk_list_store_set (GTK_LIST_STORE(ezeedo->tasks_store),
                                &iter,
                                TASK_FILTERED, true,
                                -1);
        }
        else 
        {
            gtk_list_store_set (GTK_LIST_STORE(ezeedo->tasks_store),
                                &iter,
                                TASK_FILTERED, false,
                                -1);
        }

        // increment i and make iter point to next row 
        i++;
        valid = gtk_tree_model_iter_next (GTK_TREE_MODEL(ezeedo->tasks_store),
                                          &iter);
    }

    return;
}


/**
 * Shows list of tasks filtered by category type and category id
 */
void
show_tasklist (ezeedo_wrapper_structure *ezeedo,
               gint                      type,
               gint                      id)
{
    GtkTreeModel *category_filter;
    GtkWidget    *new_todolist;

    // set task visibility for given id and type
    change_task_visibility (ezeedo,
                            type,
                            id);

    // if show all is selected, deselect contexts and projects
    if (type == CATEGORYLIST_ALL)
    {
        reset_category_selection (ezeedo,
                                  CATEGORYLIST_PROJECTS);
        reset_category_selection (ezeedo,
                                  CATEGORYLIST_CONTEXTS);
    }
    // if a context is selected, deselect projects and show all
    else if (type == CATEGORYLIST_CONTEXTS)
    {
        reset_category_selection (ezeedo,
                                  CATEGORYLIST_PROJECTS);
        reset_category_selection (ezeedo,
                                  CATEGORYLIST_ALL);
    }
    // if a projects is selected, deselect contexts and show all
    else if (type == CATEGORYLIST_PROJECTS)
    {
        reset_category_selection (ezeedo,
                                  CATEGORYLIST_CONTEXTS);
        reset_category_selection (ezeedo,
                                  CATEGORYLIST_ALL);
    }
    else
    {
        return;
    }

    // create tree model filter
    category_filter = gtk_tree_model_filter_new (GTK_TREE_MODEL(ezeedo->tasks_store),
                                                 NULL);
    // use unfinished tasks for filter
    if (type == CATEGORYLIST_ALL)
    {
        gtk_tree_model_filter_set_visible_column (GTK_TREE_MODEL_FILTER(category_filter),
                                                  TASK_NOTCOMPLETED);
    }
    // use visible tasks for filter
    else
    {
        gtk_tree_model_filter_set_visible_column (GTK_TREE_MODEL_FILTER(category_filter),
                                                  TASK_FILTERED);
    }

    // destroy current todolist
    gtk_widget_destroy (ezeedo->todolist);

    // create new todolist widget
    new_todolist = build_tasklist (ezeedo,
                                   GTK_TREE_MODEL(category_filter));

    // add todolist to ezeedo wrapper structure
    ezeedo->todolist = new_todolist; 

    // display new todolist widget
    gtk_container_add (GTK_CONTAINER(ezeedo->todolist_box),
                       new_todolist);
    gtk_widget_show_all (ezeedo->todolist_box);

    return;
}