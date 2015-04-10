/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * start.c
 * Implements functions used by the main loop 
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


#include "start.h"

#include "categorylist.h"
#include "helpers.h"
#include "preferences.h"
#include "taskentry.h"
#include "tasklist.h"


/**
 * Shows default first window of Ezeedo
 */
void
activate (GtkApplication *app,
          gpointer        user_data)
{
    // get ezeedo from user data
    ezeedo_wrapper_structure *ezeedo;
    ezeedo = user_data;

    // define widgets
    GtkWidget *window;
    GtkWidget *todo_showall;
    GtkWidget *todo_contexts;
    GtkWidget *todo_projects;
    GtkWidget *donelist;
    GtkWidget *todolist;

    // initialize textlist and add to ezeedo wrapper structure
    textlist_container *main_textlist = calloc (1, sizeof(textlist_container));
    ezeedo->textlist = main_textlist;

    // initialize tasklist and add to ezeedo wrapper structure
    tasklist_container *main_tasklist = calloc (1, sizeof(tasklist_container));
    ezeedo->tasklist = main_tasklist;

    // initialize context_list and add to ezeedo wrapper structure
    category_container *context_list = calloc (1, sizeof(category_container));
    ezeedo->context_list = context_list;

    // initialize project_list and add to ezeedo wrapper structure
    category_container *project_list = calloc (1, sizeof(category_container));
    ezeedo->project_list = project_list;

    // initialize tasks store and add to ezeedo wrapper structure
    GtkListStore *tasks_store = create_tasks_store ();
    ezeedo->tasks_store = tasks_store;


    // create main window for application
    window = create_mainwindow (ezeedo);


    // get todotxt file from gsettings store
    GSettings     *settings;
    gchar *todotxt_file;
    settings     = g_settings_new        ("org.y20k.ezeedo");
    todotxt_file = g_settings_get_string (settings,
                                          "todo-txt-file");
    g_object_unref (settings);

    // if not in gsettings store get todotxt file from file chooser 
    if (strlen(todotxt_file) == 0)
    {
        todotxt_file = open_file_dialog (G_APPLICATION(ezeedo->application));
        // quit application if open file dialog returns NULL
        if (todotxt_file == NULL)
        {
            char text[INFODIALOGLENGTH];
            snprintf (text, INFODIALOGLENGTH,"File chooser returned NULL.");
            show_info (GTK_WIDGET(window),
                       text,
                       true);
            terminate (ezeedo);
        }
        else
        {
            save_file_name_location (todotxt_file);
        }
    }

    // open todo.txt file and load it line by line into a raw text list
    gboolean file_loaded = load_tasklist_file (todotxt_file,
                                               main_textlist);
    if (!file_loaded)
    {
        char text[INFODIALOGLENGTH];
        snprintf (text, INFODIALOGLENGTH,"Could not load file:\n%s", todotxt_file);
        show_info (GTK_WIDGET(window),
                   text,
                   true);
        select_and_save_file (NULL,
                              GTK_APPLICATION(ezeedo->application));
    }

    // parse the raw textlist and load it into the main tasklist and sort it
    gboolean textlist_parsed = parse_textlist (main_textlist,
                                               main_tasklist,
                                               context_list,
                                               project_list);
    if (!textlist_parsed)
    {
        char text[INFODIALOGLENGTH];
        snprintf (text, INFODIALOGLENGTH,"Could not parse file:\n%s", todotxt_file);
        show_info (GTK_WIDGET(window),
                   text,
                   true);
        select_and_save_file (NULL,
                              GTK_APPLICATION(ezeedo->application));
    }
    sort_tasklist (main_tasklist);


    // fill tasks store
    fill_tasks_store (ezeedo);

    GtkTreeModel *filter_todo;
    filter_todo = gtk_tree_model_filter_new (GTK_TREE_MODEL(ezeedo->tasks_store),
                                             NULL);
    gtk_tree_model_filter_set_visible_column (GTK_TREE_MODEL_FILTER(filter_todo),
                                              TASK_NOTCOMPLETED);
 
    GtkTreeModel *filter_done;
    filter_done = gtk_tree_model_filter_new (GTK_TREE_MODEL(ezeedo->tasks_store),
                                             NULL );
    gtk_tree_model_filter_set_visible_column (GTK_TREE_MODEL_FILTER(filter_done),
                                              TASK_COMPLETED);


    // create todolist widget and add to ezeedo wrapper structure
    todolist = build_tasklist (ezeedo,
                               GTK_TREE_MODEL(filter_todo));
    gtk_container_add (GTK_CONTAINER (ezeedo->todolist_box),
                       todolist);
    ezeedo->todolist = todolist;
    /* note to self: better use show_tasklist
    show_tasklist (ezeedo,
                   CATEGORYLIST_ALL
                   -1);*/

    // create donelist and add to ezeedo wrapper structure
    donelist = build_tasklist (ezeedo,
                               GTK_TREE_MODEL(filter_done));
    gtk_container_add (GTK_CONTAINER (ezeedo->donelist_box),
                       donelist);
    ezeedo->donelist = donelist;


    // create showall and add to ezeedo wrapper structure
    todo_showall  = build_show_all (ezeedo);
    gtk_container_add (GTK_CONTAINER(ezeedo->categories_box),
                       todo_showall);
    ezeedo->todo_showall  = todo_showall;


    // create contexts and add to ezeedo wrapper structure
    GtkTreeModel *filter_contexts;
    filter_contexts = fill_category_store (ezeedo,
                                           ezeedo->context_list,
                                           CATEGORYLIST_CONTEXTS);
    todo_contexts = build_categorylist (ezeedo,
                                        GTK_TREE_MODEL(filter_contexts),
                                        "Contexts");
    gtk_container_add (GTK_CONTAINER(ezeedo->categories_box),
                       todo_contexts);
    ezeedo->todo_contexts = todo_contexts;


    // create projects and add to ezeedo wrapper structure
    GtkTreeModel *filter_projects;
    filter_projects = fill_category_store (ezeedo,
                                           ezeedo->project_list,
                                           CATEGORYLIST_PROJECTS);
    todo_projects = build_categorylist (ezeedo,
                                        GTK_TREE_MODEL(filter_projects),
                                        "Projects");
    gtk_widget_set_vexpand (todo_projects,
                            true);
    gtk_container_add (GTK_CONTAINER(ezeedo->categories_box),
                       todo_projects);
    ezeedo->todo_projects = todo_projects; 


    // show main window
    gtk_widget_show_all (window);


    return;
}


/**
 * Sets up the application when it first starts
 */
void
startup (GApplication *app,
         gpointer      user_data)
{
    // get ezeedo from user data
    ezeedo_wrapper_structure *ezeedo;
    ezeedo = user_data;

    // define widgets
    GMenu *menu;
    GMenu *section;

    // define actions
    GSimpleAction *preferences_action;
    GSimpleAction *quit_action;

    // create application menu
    menu = g_menu_new ();

    // create section
    section = g_menu_new ();

    // contruct application menu
    g_menu_append         (section,
                           "About",
                           "win.about");
    g_menu_append         (section,
                           "Quit",
                           "app.quit");
    g_menu_append         (menu,
                           "Preferences",
                           "app.preferences");
    g_menu_append_section (menu,
                           NULL,
                           G_MENU_MODEL (section));

    // create actions
    preferences_action = g_simple_action_new ("preferences",
                                              NULL);
    g_signal_connect (preferences_action, "activate",
                      G_CALLBACK(show_preferences_dialog), ezeedo);
    g_action_map_add_action (G_ACTION_MAP(app),
                             G_ACTION(preferences_action));

    // activate ctrl-q
    const gchar* quit_accels[2] = { "<Ctrl>Q", NULL };
    gtk_application_set_accels_for_action (GTK_APPLICATION(app),
                                           "app.quit",
                                           quit_accels);

    quit_action = g_simple_action_new ("quit",
                                       NULL);
    g_signal_connect        (quit_action, "activate",
                             G_CALLBACK (quit_application), ezeedo);
    g_action_map_add_action (G_ACTION_MAP(app),
                             G_ACTION(quit_action));

    // Set menu for the overall application
    gtk_application_set_app_menu (GTK_APPLICATION(app),
                                  G_MENU_MODEL(menu));

    return;
}


/**
 * Creates empty main application window
 */
GtkWidget
*create_mainwindow (ezeedo_wrapper_structure *ezeedo)
{
    // define widgets
    GtkWidget *window;
    GtkWidget *headerbar;
    GtkWidget *windowmenu_button;
    GtkWidget *stack;
    GtkWidget *stackswitcher;

    GtkWidget *todo_stack;
    GtkWidget *todo_paned;
    GtkWidget *todolist_scrollbox;
    GtkWidget *todolist_box;
    GtkWidget *categories_scrollbox;
    GtkWidget *categories_box;
    GtkWidget *task_entry;

    GtkWidget *done_stack;
    GtkWidget *donelist_scrollbox;
    GtkWidget *donelist_box;

    GtkWidget *archive_button;

    // define action
    GSimpleAction *about_action;

    // get window size and position from gsettings store
    GSettings     *settings;
    gint width;
    gint height;
    gint x;
    gint y;
    gint sidebar_size;

    settings     = g_settings_new     ("org.y20k.ezeedo");
    width        = g_settings_get_int (settings,
                                       "main-window-width");
    height       = g_settings_get_int (settings,
                                       "main-window-height");
    x            = g_settings_get_int (settings,
                                       "main-window-position-y");
    y            = g_settings_get_int (settings,
                                       "main-window-position-y");
    sidebar_size = g_settings_get_int (settings,
                                       "sidebar-size");
    g_object_unref (settings);

    // create main window with title and default size
    window = gtk_application_window_new (ezeedo->application);
    gtk_window_set_title        (GTK_WINDOW (window),
                                 "Ezeedo");
    gtk_window_set_icon_name    (GTK_WINDOW (window),
                                 EZEEDOICON);
    gtk_window_set_default_size (GTK_WINDOW (window),
                                 width,
                                 height);
    gtk_window_move             (GTK_WINDOW (window),
                                 x,
                                 y);

    // create headerbar and stack switcher
    headerbar       = gtk_header_bar_new ();
    gtk_header_bar_set_show_close_button (GTK_HEADER_BAR(headerbar),
                                          true);
    stackswitcher   = gtk_stack_switcher_new ();

    // create stack for todo and done
    stack = gtk_stack_new ();

    // create stack for todolist
    todo_stack = gtk_box_new (GTK_ORIENTATION_VERTICAL,
                              0);

    // create paned container for left and right columns of todo tab
    todo_paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_position (GTK_PANED(todo_paned),
                            sidebar_size);

    // create categories scrollbox and box for left pane
    categories_scrollbox = gtk_scrolled_window_new (NULL,
                                                    NULL);
    categories_box = gtk_box_new (GTK_ORIENTATION_VERTICAL,
                                  0);

    // create todolist scrollbox and box for right pane
    todolist_scrollbox = gtk_scrolled_window_new (NULL,
                                                  NULL);
    todolist_box = gtk_box_new (GTK_ORIENTATION_VERTICAL,
                                0);

    // create task entry
    task_entry = gtk_entry_new ();
    gtk_widget_set_margin_start       (GTK_WIDGET(task_entry),
                                       10);
    gtk_widget_set_margin_end         (GTK_WIDGET(task_entry),
                                       10);
    gtk_widget_set_margin_top         (GTK_WIDGET(task_entry),
                                       10);
    gtk_widget_set_margin_bottom      (GTK_WIDGET(task_entry),
                                       10);
    gtk_entry_set_placeholder_text    (GTK_ENTRY(task_entry),
                                       "Enter new task");
    gtk_entry_set_max_length          (GTK_ENTRY(task_entry),
                                       TASKLENGTH);
    gtk_widget_grab_focus (task_entry);
 
    // create stack for donelist
    done_stack = gtk_box_new (GTK_ORIENTATION_VERTICAL,
                                  0);

    // create donelist scrollbox and box
    donelist_scrollbox = gtk_scrolled_window_new (NULL,
                                                  NULL);
    donelist_box = gtk_box_new (GTK_ORIENTATION_VERTICAL,
                                0);

    // create archive button
    archive_button = gtk_button_new_with_label ("Archive all done tasks");
    gtk_widget_set_margin_start  (GTK_WIDGET (archive_button),
                                  10);
    gtk_widget_set_margin_end    (GTK_WIDGET (archive_button),
                                  10);
    gtk_widget_set_margin_top    (GTK_WIDGET (archive_button),
                                  10);
    gtk_widget_set_margin_bottom (GTK_WIDGET (archive_button),
                                  10);

    // add some widgets to ezeedo wrapper structure
    ezeedo->window = window;
    ezeedo->todo_paned = todo_paned;
    ezeedo->categories_box = categories_box;
    ezeedo->todolist_box = todolist_box;
    ezeedo->donelist_box = donelist_box;

    // create window menu
    windowmenu_button = create_windowmenu (ezeedo);

    // create about action and connect about action signal
    about_action = g_simple_action_new ("about",
                                        NULL); 
    g_action_map_add_action (G_ACTION_MAP (window),
                             G_ACTION (about_action));
    g_signal_connect (about_action, "activate",
                      G_CALLBACK (show_about_window), window);

    // detect entry signal
    g_signal_connect (task_entry, "activate",
                      G_CALLBACK (add_task_entry), ezeedo);

    // detect archive button pressed
    g_signal_connect (archive_button, "clicked",
                      G_CALLBACK (display_info_dialog), "Task archival is not supported yet.");
 
    // detect window close
    g_signal_connect (G_OBJECT (window), "delete-event",
                      G_CALLBACK (close_window), ezeedo);


    // construct headerbar for main window
    gtk_window_set_titlebar   (GTK_WINDOW(window),
                               headerbar);
    gtk_header_bar_pack_start (GTK_HEADER_BAR(headerbar),
                               stackswitcher);
    gtk_header_bar_pack_end   (GTK_HEADER_BAR(headerbar),
                               windowmenu_button);

    // add stack to main window
    gtk_container_add (GTK_CONTAINER(window),
                       stack);

    // set stack switcher and populate stack with todolist and donelist
    gtk_stack_switcher_set_stack (GTK_STACK_SWITCHER(stackswitcher),
                                  GTK_STACK(stack));
    gtk_stack_add_titled         (GTK_STACK(stack),
                                  todo_stack,
                                  "To do",
                                  "To do");
    gtk_stack_add_titled         (GTK_STACK(stack),
                                  done_stack,
                                  "Done",
                                  "Done");

    // construct main todo stack
    gtk_container_add (GTK_CONTAINER(todo_stack),
                       todo_paned);

    gtk_paned_add1   (GTK_PANED(todo_paned),
                      categories_scrollbox);
    gtk_paned_add2   (GTK_PANED(todo_paned),
                      todolist_scrollbox);

    // add box to scrollable box
    gtk_container_add (GTK_CONTAINER(todolist_scrollbox),
                       todolist_box);

    // add task entry to todo stack
    gtk_container_add (GTK_CONTAINER(todo_stack),
                       task_entry);

    // add box to scrollable box
    gtk_container_add (GTK_CONTAINER(categories_scrollbox),
                       categories_box);

    // construct main donelist stack
    gtk_container_add (GTK_CONTAINER(done_stack),
                       donelist_scrollbox);
    gtk_container_add (GTK_CONTAINER(donelist_scrollbox),
                       donelist_box);

    return (window); 
}


/**
 * Creates window menu
 */
GtkWidget
*create_windowmenu (ezeedo_wrapper_structure *ezeedo)
{
    // define widgets
    GtkWidget *windowmenu_button;
    GtkWidget *win;
    GMenu     *windowmenu;

    win = ezeedo->window;

    // define actions
    GSimpleAction *toggle_action;

    // create gear menu
    windowmenu = g_menu_new ();
    g_menu_append (windowmenu,
                   "Toggle sidebar",
                   "win.toggle_sidebar");

    // create actions
    toggle_action = g_simple_action_new ("toggle_sidebar",
                                         NULL);
    g_signal_connect (toggle_action, "activate",
                      G_CALLBACK(toggle_sidebar), ezeedo);
    g_action_map_add_action (G_ACTION_MAP(win),
                             G_ACTION(toggle_action));

    // activate ctrl-h
    const gchar* toggle_accels[2] = { "<Ctrl>H", NULL };
    gtk_application_set_accels_for_action (GTK_APPLICATION(ezeedo->application),
                                           "win.toggle_sidebar",
                                            toggle_accels);

    // create window menu 
    windowmenu_button  = gtk_menu_button_new ();
    gtk_menu_button_set_direction (GTK_MENU_BUTTON(windowmenu_button),
                                   GTK_ARROW_NONE);

    // attach window menu to button
    gtk_menu_button_set_use_popover (GTK_MENU_BUTTON(windowmenu_button),
                                     true);
    gtk_menu_button_set_menu_model (GTK_MENU_BUTTON(windowmenu_button),
                                    G_MENU_MODEL(windowmenu)); 

    // return button with window menu
    return (windowmenu_button);
}


/**
 * Shows the about window
 */
void
show_about_window (GSimpleAction *simple,
                   GVariant      *parameter,
                   gpointer       user_data)
{
    GtkWidget *window;
    window = user_data;

    // define authors and documenters
    const gchar *authors[] = {"Ezeedo Team", NULL};
    const gchar *documenters[] = {"Ezeedo Team", NULL};

    gtk_show_about_dialog (GTK_WINDOW (window),
                           "program-name",   "Ezeedo",
                           "comments",       "A todo.txt app for the GNOME desktop",
                           "logo-icon-name", EZEEDOICON,
                           "license-type",   GTK_LICENSE_MIT_X11,
                           "version",        EZEEDOVERSION,
                           "authors",        authors,
                           "documenters",    documenters,
                           "website",        "http://www.y20k.org/ezeedo/",
                           "website_label",  "Ezeedo Website",
                           NULL);

    return;
}