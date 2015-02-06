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
#include "tasklist.h"
#include "taskentry.h"


/**
 * Shows default first window of Ezeedo
 */
void
activate (GtkApplication *app,
          gpointer        user_data)
{
    // create convenience wrapper structure for importent elements
    ezeedo_wrapper_structure *ezeedo = calloc (1, sizeof(ezeedo_wrapper_structure));
    // TODO free ezeedo structure

    // add application to ezeedo wrapper structure
    ezeedo->application = app;

    // initialize textlist and add to ezeedo wrapper structure
    textlist_container *main_textlist = calloc (1, sizeof(textlist_container));
    ezeedo->textlist = main_textlist;

    // initialize tasklist and add to ezeedo wrapper structure
    tasklist_container *main_tasklist = calloc (1, sizeof(tasklist_container));
    ezeedo->tasklist = main_tasklist;

    // initialize filtered_tasklist and add to ezeedo wrapper structure
    // tasklist_container* filtered_tasklist = calloc (1, sizeof(tasklist_container));

    // initialize context_list and add to ezeedo wrapper structure
    category_container *context_list = calloc (1, sizeof(category_container));
    ezeedo->context_list = context_list;

    // initialize project_list and add to ezeedo wrapper structure
    category_container *project_list = calloc (1, sizeof(category_container));
    ezeedo->project_list = project_list;

    // initialize tasks store and add to ezeedo wrapper structure
    GtkListStore *tasks_store = create_tasks_store ();
    ezeedo->tasks_store = tasks_store;

    // use CSS styling
    // GFile *cssfile;
    // GtkCssProvider *provider; 
    // GdkDisplay *display;
    // GdkScreen *screen;

    // cssfile  = g_file_new_for_path ("ezeedo.css");
    // provider = gtk_css_provider_new ();
    // display  = gdk_display_get_default ();
    // screen   = gdk_display_get_default_screen (display);
    // gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    // gtk_css_provider_load_from_file (GTK_CSS_PROVIDER(provider), cssfile, NULL);


    // define widgets
    GtkWidget *window;
    GtkWidget *headerbar;
    GtkWidget *stackswitcher;
    GtkWidget *stack;

    GtkWidget *todolist_box;
    GtkWidget *todolist_box_paned;
    GtkWidget *todolist_box_left;
    GtkWidget *todolist_box_right;
    GtkWidget *todolist_container_scrolled;
    GtkWidget *todolist;
    GtkWidget *todo_categories_container_scrolled;
    GtkWidget *todo_categories_box;
    GtkWidget *todo_contexts;
    GtkWidget *todo_projects;
    GtkWidget *showall_button;
    GtkWidget *task_entry;

    GtkWidget *donelist_box;
    GtkWidget *donelist_container_scrolled;
    GtkWidget *donelist;
    GtkWidget *archive_button;

    // define actions and settings
    GSimpleAction *about_action;
    GSettings     *settings;


    // get settings from gsettings store
    gchar *todotxt_file;
    int width;
    int height;
    int x;
    int y;

    settings     = g_settings_new        ("org.y20k.ezeedo");
    width        = g_settings_get_int    (settings,
                                          "main-window-width");
    height       = g_settings_get_int    (settings,
                                          "main-window-height");
    x            = g_settings_get_int    (settings,
                                          "main-window-position-y");
    y            = g_settings_get_int    (settings,
                                          "main-window-position-y");
    todotxt_file = g_settings_get_string (settings,
                                          "todo-txt-file");
    g_object_unref (settings);


    // create main window with title and default size
    window = gtk_application_window_new (app);
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

    // add window to ezeedo wrapper structure
    ezeedo->window = window;

    // create headerbar with centered stackswitcher
    headerbar = gtk_header_bar_new ();
    gtk_header_bar_set_show_close_button (GTK_HEADER_BAR(headerbar),
                                          true);
    stackswitcher = gtk_stack_switcher_new ();

    // create stack for todo and done
    stack = gtk_stack_new ();

    // create main box for todolist
    todolist_box = gtk_box_new (GTK_ORIENTATION_VERTICAL,
                                0);

    // create paned container for left and right columns for the todo tab
    todolist_box_paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);

    // create left column for the todo tab
    todolist_box_left = gtk_frame_new (NULL);
    gtk_widget_set_size_request (todolist_box_left,
                                 150,
                                 -1);


    // create scrolled categories container and box
    todo_categories_container_scrolled = gtk_scrolled_window_new (NULL,
                                                                  NULL);
    todo_categories_box = gtk_box_new (GTK_ORIENTATION_VERTICAL,
                                       0);
    gtk_widget_set_vexpand (todo_categories_container_scrolled,
                            true);

    // create show all button
    showall_button = gtk_button_new_with_label ("Show All");
 
    // create right column for the todo tab
    todolist_box_right = gtk_frame_new (NULL);

    // create scrolled todolist container
    todolist_container_scrolled = gtk_scrolled_window_new (NULL,
                                                           NULL);

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
 
 
    // create main box for donelist
    donelist_box = gtk_box_new (GTK_ORIENTATION_VERTICAL,
                                0);

    // create scrolled donelist container
    donelist_container_scrolled = gtk_scrolled_window_new (NULL,
                                                           NULL);
    gtk_widget_set_vexpand (donelist_container_scrolled,
                            true);

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

    // detect show all button pressed
    g_signal_connect (showall_button, "clicked",
                      G_CALLBACK (show_all), ezeedo);
 
    // detect archive button pressed
    g_signal_connect (archive_button, "clicked",
                      G_CALLBACK (display_info_dialog), "Task archival is not supported yet.");
 
    // detect window close
    g_signal_connect (G_OBJECT (window), "delete-event",
                      G_CALLBACK (close_window), app);

    // construct main window from inside to outside
    gtk_container_add    (GTK_CONTAINER(todo_categories_container_scrolled),
                          todo_categories_box);

    gtk_container_add    (GTK_CONTAINER(todolist_box_left),
                          todo_categories_container_scrolled);
    gtk_container_add    (GTK_CONTAINER(todolist_box_right),
                          todolist_container_scrolled);

    gtk_paned_pack1      (GTK_PANED(todolist_box_paned),
                          todolist_box_left,
                          false,
                          true);
    gtk_paned_pack2      (GTK_PANED(todolist_box_paned),
                          todolist_box_right,
                          true,
                          true);

    gtk_container_add    (GTK_CONTAINER(todolist_box),
                          todolist_box_paned);
    gtk_container_add    (GTK_CONTAINER(todolist_box),
                          task_entry);

    gtk_container_add    (GTK_CONTAINER(donelist_box),
                          donelist_container_scrolled);
    gtk_container_add    (GTK_CONTAINER(donelist_box),
                          archive_button);

    gtk_stack_add_titled (GTK_STACK(stack),
                          todolist_box,
                          "To do",
                          "To do");
    gtk_stack_add_titled (GTK_STACK(stack),
                          donelist_box,
                          "Done",
                          "Done");
    gtk_container_add    (GTK_CONTAINER(window),
                          stack);

    // construct headerbar
    gtk_stack_switcher_set_stack (GTK_STACK_SWITCHER(stackswitcher),
                                  GTK_STACK(stack));
    gtk_header_bar_pack_start    (GTK_HEADER_BAR(headerbar),
                                  stackswitcher);
    gtk_window_set_titlebar      (GTK_WINDOW(window),
                                  headerbar);
    // TODO Add gear menu here
 
    // show main window
    gtk_widget_show_all (window);


    // set default location and file name
    if (strlen(todotxt_file) == 0)
    {
        todotxt_file = open_file_dialog (G_APPLICATION(app));
        // quit application if open file dialog returns NULL
        if (todotxt_file == NULL)
        {
            g_application_quit (G_APPLICATION(app));
        }
        else
        {
            save_file_name_location (todotxt_file);
        }
    }


    // open todo.txt file and load it line by line into a raw text list
    bool file_loaded = load_tasklist_file (todotxt_file,
                                           main_textlist);
    if (!file_loaded)
    {
        char text[INFODIALOGLENGTH];
        snprintf (text, INFODIALOGLENGTH,"Could not load file:\n%s", todotxt_file);
        show_info (GTK_WIDGET(window),
                   text,
                   true);
        select_and_save_file (NULL,
                              GTK_APPLICATION(app));
    }

    // parse the raw textlist and load it into the main tasklist and sort it
    bool textlist_parsed = parse_textlist(main_textlist, main_tasklist, context_list, project_list);
    if (!textlist_parsed)
    {
        char text[INFODIALOGLENGTH];
        snprintf (text, INFODIALOGLENGTH,"Could not parse file:\n%s", todotxt_file);
        show_info (GTK_WIDGET(window),
                   text,
                   true);
        select_and_save_file (NULL,
                              GTK_APPLICATION(app));
    }
    sort_tasklist (main_tasklist);


    // create and display todo categories widget
    // TODO: rework Contexts and Projects as GtkSidebar / GtkStack for GTK+ 3.16 (April 2015)
    // http://fossies.org/linux/misc/gtk+-3.15.1.tar.gz/gtk+-3.15.1/docs/reference/gtk/html/GtkSidebar.html
    todo_contexts = display_category (ezeedo,
                                      ezeedo->context_list,
                                      "Contexts",
                                      CATEGORYLIST_CONTEXTS);
    todo_projects = display_category (ezeedo,
                                      ezeedo->project_list,
                                      "Projects",
                                      CATEGORYLIST_PROJECTS);
    gtk_widget_set_vexpand (todo_projects,
                            true);

    gtk_container_add (GTK_CONTAINER(todo_categories_box),
                       todo_contexts);
    gtk_container_add (GTK_CONTAINER(todo_categories_box),
                       todo_projects);
    gtk_container_add (GTK_CONTAINER(todo_categories_box),
                       showall_button);
    gtk_widget_show_all (todo_categories_box);

    // add todo_contexts and todo_projects to ezeedo wrapper structure
    ezeedo->todo_contexts = todo_contexts;
    ezeedo->todo_projects = todo_projects; 

    // fill tasks store
    fill_tasks_store (ezeedo);

    GtkTreeModel *filter_todo;
    filter_todo = gtk_tree_model_filter_new (GTK_TREE_MODEL(tasks_store),
                                             NULL);
    gtk_tree_model_filter_set_visible_column (GTK_TREE_MODEL_FILTER(filter_todo),
                                              TASK_NOTCOMPLETED);
 
    GtkTreeModel *filter_done;
    filter_done = gtk_tree_model_filter_new (GTK_TREE_MODEL(tasks_store),
                                             NULL );
    gtk_tree_model_filter_set_visible_column (GTK_TREE_MODEL_FILTER(filter_done),
                                              TASK_COMPLETED);

    // create and display todolist widget
    todolist = display_tasklist (GTK_TREE_MODEL(filter_todo));
    gtk_container_add (GTK_CONTAINER (todolist_container_scrolled),
                       todolist);
    gtk_widget_show_all (todolist_container_scrolled);

    // add todolist to ezeedo wrapper structure
    ezeedo->todolist = todolist;

    // create and display donelist widget
    donelist = display_tasklist (GTK_TREE_MODEL(filter_done));
    gtk_container_add (GTK_CONTAINER (donelist_container_scrolled),
                       donelist);
    gtk_widget_show_all (donelist_container_scrolled);

    // detect double-click on tasklist
    g_signal_connect (todolist, "row-activated",
                      G_CALLBACK(task_doubleclicked), ezeedo);
}


/**
 * Sets up the application when it first starts
 */
void
startup (GApplication *app,
         gpointer      user_data)
{
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
                      G_CALLBACK(show_preferences_dialog), app);
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
                             G_CALLBACK (quit_application), app);
    g_action_map_add_action (G_ACTION_MAP(app),
                             G_ACTION(quit_action));

    // Set menu for the overall application
    gtk_application_set_app_menu (GTK_APPLICATION(app),
                                  G_MENU_MODEL(menu));
}


/**
 * Shows the about window
 */
void
show_about_window (GSimpleAction *simple,
                   GVariant      *parameter,
                   gpointer       user_data)
{
    // define widgets
    GtkWidget *about_dialog;

    // create about dialog
    about_dialog = gtk_about_dialog_new ();

    // define widget content
    const gchar *authors[] = {"Ezeedo Team", NULL};
    const gchar *documenters[] = {"Ezeedo Team", NULL};

    gtk_about_dialog_set_program_name   (GTK_ABOUT_DIALOG (about_dialog),
                                         "Ezeedo");
    gtk_about_dialog_set_license_type   (GTK_ABOUT_DIALOG (about_dialog),
                                         GTK_LICENSE_MIT_X11);
    gtk_about_dialog_set_version        (GTK_ABOUT_DIALOG (about_dialog),
                                         "0.2 (black country rock)");
    gtk_about_dialog_set_comments       (GTK_ABOUT_DIALOG (about_dialog),
                                         "A todo.txt app for the GNOME desktop");
    gtk_about_dialog_set_logo_icon_name (GTK_ABOUT_DIALOG (about_dialog),
                                         EZEEDOICON);
    gtk_about_dialog_set_authors        (GTK_ABOUT_DIALOG (about_dialog),
                                         authors);
    gtk_about_dialog_set_documenters    (GTK_ABOUT_DIALOG (about_dialog),
                                         documenters);
    gtk_about_dialog_set_website_label  (GTK_ABOUT_DIALOG (about_dialog),
                                         "Ezeedo Website");
    gtk_about_dialog_set_website        (GTK_ABOUT_DIALOG (about_dialog),
                                         "http://www.y20k.org/ezeedo");
    gtk_window_set_title                (GTK_WINDOW (about_dialog),
                                         "");

    // connect signals and show widget 
    g_signal_connect_swapped (GTK_DIALOG(about_dialog), "response",
                              G_CALLBACK (gtk_widget_destroy), GTK_DIALOG (about_dialog));
    gtk_widget_show (about_dialog); 

    return;
}
