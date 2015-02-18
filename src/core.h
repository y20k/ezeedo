/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * core.h
 * Declares todotxt core functionalities
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


#ifndef CORE_H
#define CORE_H


#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>


// size of task
#define TASKLENGTH 255
// size of tasklist
#define TASKLISTLENGTH 255
// length of todo.txt filename and path
#define TODOTXTFILELENGTH 255
// maximum length for word
#define WORDLENGTH 45

// location of logo
#define EZEEDOICON "ezeedo"
// length of info dialog message
#define INFODIALOGLENGTH 80


// TODO (remove) location of toto.txt file
// #define TODOTXTFILE '/home/solaris/Documents/toto.txt'


enum
{
    TASK_ID,
    TASK_PRIORITY,
    TASK_DESCRIPTION,
    TASK_CONTEXT,
    TASK_PROJECT,
    TASK_COMPLETED,
    TASK_NOTCOMPLETED,
    TASK_FILTERED,
    TASK_COLUMNS
};


enum
{
    CATEGORY_ID,
    CATEGORY_TITLE,
    CATEGORY_TYPE,
    CATEGORY_COLUMNS
};


enum
{
    CATEGORYLIST_ALL,
    CATEGORYLIST_CONTEXTS,
    CATEGORYLIST_PROJECTS,
    NUMBER_OF_CATEGORYLISTS
};


// definition of category
typedef struct category
{
    int  id;
    char title[WORDLENGTH+1];
    int  open_tasks;
}
category;


// definition of category container
typedef struct category_container
{
    category *list[TASKLISTLENGTH];
    int       number_of_categories;
}
category_container;


// definition of task
typedef struct task
{
    int  id;
    bool completed;
    bool deleted;
    // completiondate
    int  priority;
    // creationdate
    char description[TASKLENGTH+1];
    int  context[TASKLISTLENGTH/WORDLENGTH];
    int  number_of_contexts;
    int  project[TASKLISTLENGTH/WORDLENGTH];
    int  number_of_projects;
}
task;


// definition of tasklist container
typedef struct tasklist_container
{
    task *list[TASKLISTLENGTH];
    int   number_of_tasks;
    // list of contexts
    // list of projects
}
tasklist_container;


// definition of text line
typedef struct textline
{
    int  id;
    char raw_line[TASKLENGTH+1];
    int  line_length;
}
textline;


// definition of textlist container
typedef struct textlist_container
{
    textline *line[TASKLISTLENGTH];
    int       number_of_lines;
}
textlist_container;


// definition of the ezeedo wrapper structure
typedef struct ezeedo_wrapper_structure
{
    textlist_container *textlist;
    tasklist_container *tasklist;
    category_container *context_list;
    category_container *project_list;
    GtkApplication     *application;
    GtkListStore       *tasks_store;
    GtkWidget          *window;
    GtkWidget          *todo_paned;
    GtkWidget          *categories_box;
    GtkWidget          *todolist_box;
    GtkWidget          *donelist_box;
    GtkWidget          *todo_showall;
    GtkWidget          *todo_contexts;
    GtkWidget          *todo_projects;
    GtkWidget          *todolist;
}
ezeedo_wrapper_structure;


/**
 * Loads todo.txt file into memory.
 */
bool 
load_tasklist_file (const char         *tasklist_file,
                    textlist_container *textlist);

/**
 * Parses textlist and loads it into tasklist.
 */
bool
parse_textlist (textlist_container *textlist,
                tasklist_container *tasklist,
                category_container *context_list,
                category_container *project_list);

/**
 * Adds line of text to textlist.
 */
bool
add_line_to_textlist (const char         *line,
                      textlist_container *textlist);

/**
 * Saves a textlist to file
 */
bool
save_textlist_to_file (textlist_container *textlist,
                       const char         *tasklist_file);

/**
 * Adds line of text as task to tasklist
 */
bool
add_task_to_tasklist (const char         *line,
                      tasklist_container *tasklist,
                      category_container *context_list, 
                      category_container *project_list);

/**
 * Creates new task from given line of text
 */
bool
create_new_task (const char         *line,
                 task               *new_task,
                 category_container *context_list, 
                 category_container *project_list);

/**
 * Looks for the completed syntax
 */
bool
check_task_completion (const char *word,
                       const int   word_length);

/**
 * Looks for the priority syntax
 */
bool
check_task_priority (const char *word,
                     const int   word_length,
                     task       *new_task);

/**
 * Looks for the context syntax and puts word into given context list and given task
 */
bool
check_task_context (char               *word,
                    const int           word_length,
                    category_container *context_list,
                    task               *new_task);

/**
 * Looks for the project syntax and puts word into given project list and given task.
 */
bool
check_task_project (char               *word,
                    const int           word_length,
                    category_container *project_list,
                    task               *new_task);

/**
 * Lookup id of given word in category list
 */
int
get_category_id (const char           category_identifier,
                 char                *word,
                 const int            word_length,
                 category_container  *category_list);


/**
 * Filters tasklist by given category puts results into filtered tasklist
 */
bool
filter_by_category (const char          category_identifier,
                    const int           category_id,
                    category_container *category_list,
                    tasklist_container *tasklist,
                    tasklist_container *filtered_tasklist);

/**
 * Determines which task has higher priority. Returns true if tasks are in correct order.
 */
bool
compare_current_to_next (task *current_task,
                         task *next_task);

/**
 * Sorts given tasklist
 */
void
sort_tasklist (tasklist_container* tasklist);

/**
 * Marks given task as done in textlist and tasklist
 */
bool
mark_task_done (const int           id,
                textlist_container *textlist,
                tasklist_container *tasklist);

/**
 * Deletes task from textlist marks task deleted in tasklist
 */
bool
delete_task (const int           id,
             textlist_container *textlist,
             tasklist_container *tasklist);

/**
 * Unloads given textlist
 */
bool
unload_textlist (textlist_container *textlist);

/**
 * Unloads given tasklist
 */
bool
unload_tasklist (tasklist_container *tasklist);

/**
 * Unloads given category list
 */
bool
unload_category_list (category_container *category_list);


#endif // CORE_H
