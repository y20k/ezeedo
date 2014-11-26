/****************************************************************************
 * -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-
 *
 * core.c
 * Implements todotxt core functionalities
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


#include "core.h"
#include "helpers.h"



/**
 * Loads todo.txt file into memory.
 */
bool load_tasklist_file (const char* tasklist_file, textlist_container* textlist)
{
    if (strlen(tasklist_file) > TODOTXTFILELENGTH)
    {
        char text[INFODIALOGLENGTH];
        snprintf(text, INFODIALOGLENGTH,"Path and filename longer than %d characters", TODOTXTFILELENGTH);
        show_info(NULL, text, FALSE);
        return false;
    }

    // open todo.txt file
    FILE* fp = fopen(tasklist_file, "r");

    // check for successful open
    if (fp == NULL)
    {
        char text[INFODIALOGLENGTH];
        snprintf(text, INFODIALOGLENGTH,"Could not open %s", tasklist_file);
        show_info(NULL, text, FALSE);
        return false;
    }
    else
    {
        char text[INFODIALOGLENGTH];
        snprintf(text, INFODIALOGLENGTH,"Sucessfully opened %s", tasklist_file);
        show_info(NULL, text, FALSE);
    }

    // initialize some variables
    int  character;

    while (textlist->number_of_lines < TASKLISTLENGTH)
    {
        // create new text line
        textline* new_line = calloc(1, sizeof(textline));
        if (new_line == NULL)
        {
            return false;
        }
        new_line->id = textlist->number_of_lines;

        // until line complete
        while ((character=fgetc(fp)) !='\n')
        {
            // handle task too long
            if (new_line->line_length > TASKLENGTH)
            {
                char text[INFODIALOGLENGTH];
                snprintf(text, INFODIALOGLENGTH,"Unable to read tasks longer than %d characters.", TASKLENGTH);
                show_info(NULL, text, FALSE);
                fclose(fp);
                return false;
            }
            // handle end of file
            if (feof(fp))
            {
                free (new_line);
                fclose(fp);
                return true;
            }

            // copy current character into line
            else
            {
                new_line->raw_line[new_line->line_length] = character;
            }
            new_line->line_length++;
        }
        
        // finish line
        new_line->raw_line[new_line->line_length] = '\0';
        new_line->line_length++;

        // ignore empty lines
        if (new_line->raw_line[0] == '\0')
        {
            free (new_line);
        }
        
        // add line to textlist
        else
        {
            textlist->line[textlist->number_of_lines] = new_line;
            textlist->number_of_lines++;
        }

    }

    // close todo.txt
    fclose(fp);

    return true;
}


/**
 * Parses textlist and loads it into tasklist.
 */
bool parse_textlist(textlist_container* textlist,
                    tasklist_container* tasklist,
                    category_container* context_list,
                    category_container* project_list)
{
    // for each line within textlist
    for (int i = 0; i < textlist->number_of_lines; i++)
    {
        bool added = add_task_to_tasklist(textlist->line[i]->raw_line, tasklist, context_list, project_list);
        if (!added)
        {
            char text[INFODIALOGLENGTH];
            snprintf(text, INFODIALOGLENGTH,"Could not add line no. %d to tasklist.", i);
            show_info(NULL, text, FALSE);
            return false;
        }
    }

    return true;
}



/**
 * Adds line of text to textlist.
 */
bool add_line_to_textlist(const char* line, textlist_container* textlist)
{
    // check the length of given line
    if (strlen(line) > TASKLENGTH)
    {
        char text[INFODIALOGLENGTH];
        snprintf(text, INFODIALOGLENGTH,"Unable to write tasks longer than %d characters.", TASKLENGTH);
        show_info(NULL, text, FALSE);
        return false;
    }

    // create new text line
    textline* new_line = calloc(1, sizeof(textline));
    if (new_line == NULL)
    {
        return false;
    }
    new_line->id          = textlist->number_of_lines;
    strcpy(new_line->raw_line, line);
    new_line->line_length = strlen(line);

    // add line to textlist
    textlist->line[textlist->number_of_lines] = new_line;
    textlist->number_of_lines++;

    return true;
}


/**
 * Saves a textlist to file
 */
bool save_textlist_to_file(textlist_container* textlist, const char* tasklist_file)
{

    // open todo.txt file
    FILE* fp = fopen(tasklist_file, "w");

    // check for successful open
    if (fp == NULL)
    {
        char text[INFODIALOGLENGTH];
        snprintf(text, INFODIALOGLENGTH,"Could not open %s.", tasklist_file);
        show_info(NULL, text, FALSE);
        return false;
    }
    else
    {
        char text[INFODIALOGLENGTH];
        snprintf(text, INFODIALOGLENGTH,"Sucessfully opened %s.", tasklist_file);
        show_info(NULL, text, FALSE);
    }

    // for each line
    for (int i = 0; i < textlist->number_of_lines; i++)
    {
        // skip empty lines
        if (textlist->line[i]->raw_line[0] != '\0')
        {
            // for (int j = 0; j < textlist->line[i]->line_length; j++)
            // TODO
            // ensure that no zeros are written
            int j = 0;
            while (textlist->line[i]->raw_line[j] != '\0' &&
                   j < textlist->line[i]->line_length)
            {
                fputc(textlist->line[i]->raw_line[j], fp);
                j++;
            }
            fputc('\n', fp);
        }
    }

    // close todo.txt
    fclose(fp);

    return true;
}


/**
 * Adds line of text as task to tasklist.
 */
bool add_task_to_tasklist(const char* line, tasklist_container* tasklist,
                          category_container* context_list,
                          category_container* project_list)
{
    // create new task
    task* new_task = calloc(1, sizeof(task));
    if (new_task == NULL)
    {
        return false;
    }
    new_task->id = tasklist->number_of_tasks;

    bool task_created = create_new_task(line, new_task, context_list, project_list);
    if (!task_created)
    {
        char text[INFODIALOGLENGTH];
        snprintf(text, INFODIALOGLENGTH,"Could not create task from given input\n%s", line);
        show_info(NULL, text, FALSE);
        return false;
    }

    // copy task into tasklist
   tasklist->list[tasklist->number_of_tasks] = new_task;
   tasklist->number_of_tasks++;

   return true;
}


/**
 * Creates new task from given line of text.
 */
bool create_new_task(const char* line, task* new_task,
                     category_container* context_list,
                     category_container* project_list)
{

    // initialize some variables
    int  line_length = (strlen(line)) + 1;
    int  line_character_counter        = 0;
    int  word_character_counter        = 0;
    int  description_character_counter = 0;
    char word[WORDLENGTH+1];

    // parse line of text
    while (line_character_counter < line_length)
    {
        if (word_character_counter >= WORDLENGTH)
        {
            char text[INFODIALOGLENGTH];
            snprintf(text, INFODIALOGLENGTH,"Word is longer than %d characters\n%s", WORDLENGTH, line);
            show_info(NULL, text, FALSE);
            return false;
        }

        // copy character from line into word
        word[word_character_counter] = line[line_character_counter];

        // found a word
        if ((line[line_character_counter]  == ' ') ||
            (line[line_character_counter]  == '\0'))
        {
            // task completion
            if (check_task_completion(word, word_character_counter))
            {
                new_task->completed = true;
                word_character_counter = 0;
            }
            // task priority
            else if (check_task_priority(word, word_character_counter, new_task))
            {
                new_task->priority = word[1];
                word_character_counter = 0;
            }

            // task context
            else if (check_task_context(word, word_character_counter,
                                        context_list, new_task))
            {
                word_character_counter = 0;
            }

            // task project
            else if (check_task_project(word, word_character_counter,
                                        project_list, new_task))
            {
                word_character_counter = 0;
            }

            // task completiondate
            // TODO

            // task creationdate
            // TODO

            // add word to task description
            else
            {
                for (int i = 0; i < word_character_counter; i++)
                {
                        new_task->description[description_character_counter] = word[i];
                        description_character_counter++;
                }
                new_task->description[description_character_counter] = ' ';
                description_character_counter++;

                word_character_counter = 0;
            }
        }
        else
        {
            word_character_counter++;
        }
        line_character_counter++;
    }

    // finish line
    new_task->description[line_length] = '\0';

    return true;
}


/**
 * Looks for the completed syntax.
 */
bool check_task_completion(const char* word, const int word_length)
{
    if (word_length == 1
        && word[0]  == 'x')
    {
        return true;
    }
    else
    {
        return false;
    }
}


/**
 * Looks for the priority syntax.
 */
bool check_task_priority(const char* word, const int word_length, task* new_task)
{
    if (new_task->priority == 0
        && word_length     == 3
        && word[0]         == '('
        && word[2]         == ')')
    {
        return true;
    }
    else
    {
        return false;
    }
}


/**
 * Looks for the context syntax and puts word into given context list and given task.
 */
bool check_task_context(char* word, const int word_length,
                        category_container* context_list, task* new_task)
{
    if (word[0] == '@' && word_length   >  1)
    {
        // get context id for the given word

        int context_id = get_category_id('@', word, word_length, context_list);
        // context is a new in context list
        if (context_id == TASKLISTLENGTH+1)
        {
            // create new context
            category* new_context = calloc(1,sizeof(category));
            if (new_context == NULL)
            {
                return false;
            }

            // set context id
            new_context->id = context_list->number_of_categories;

            // copy context title
            int i = 0;
            while (word[i] != '\0')
            {
                new_context->title[i] = word[i];
                i++;
            }
            new_context->title[i] = '\0';

            // put new context into context list
            context_list->list[context_list->number_of_categories] =
            new_context;

            // put context id into task
            new_task->context[new_task->number_of_contexts] = new_context->id;
            new_task->number_of_contexts++;

            context_list->number_of_categories++;
            return true;
        }

        // context is already in context list
        else
        {
            // put context id into task
            new_task->context[new_task->number_of_contexts] = context_id;
            new_task->number_of_contexts++;
            return true;
        }
    }

    else
    {
        return false;
    }
}


/**
 * Looks for the project syntax and puts word into given project list and given task.
 */
bool check_task_project(char* word, const int word_length,
                        category_container* project_list, task* new_task)
{
    if (word[0] == '+' && word_length > 1)
    {
        // get project id for the given word
        int project_id = get_category_id('+', word, word_length, project_list);

        // project is a new in project list
        if (project_id == TASKLISTLENGTH+1)
        {
            // create new project
            category* new_project = calloc(1,sizeof(category));
            if (new_project == NULL)
            {
                return false;
            }

            // set project id
            new_project->id = project_list->number_of_categories;

            // copy project title
            int i = 0;
            while (word[i] != '\0')
            {
                new_project->title[i] = word[i];
                i++;
            }
            new_project->title[i] = '\0';

            // put new project into project list
            project_list->list[project_list->number_of_categories] =
            new_project;

            // put project id into task
            new_task->project[new_task->number_of_projects] = new_project->id;
            new_task->number_of_projects++;

            project_list->number_of_categories++;
            return true;
        }

        // project is already in context list
        else
        {
            // put project id into task
            new_task->project[new_task->number_of_projects] = project_id;
            new_task->number_of_projects++;
            return true;
        }
    }

    else
    {
        return false;
    }
}


/**
 * Lookup id of given word in category list
 */
int get_category_id (const char category_identifier, char* word, const int word_length,
                     category_container* category_list)
{
    // strip category identifier (e.g. @) and trailing space
    int i = 0;
    while (word[i+1] != ' ' && i != word_length-1)
    {
        word[i] = word[i+1];
        i++;
    }
    word[i] = '\0';

    // look for word in category list and return its id
    for (int j = 0; j < category_list->number_of_categories; j++)
    {

        if (strcmp(word, category_list->list[j]->title)==0)
        {
            return category_list->list[j]->id;
        }
    }
    return TASKLISTLENGTH+1;
}


/**
 * Filters tasklist by given category puts results into filtered tasklist
 */
bool filter_by_category(const char category_identifier,
                        const int category_id,
                        category_container* category_list,
                        tasklist_container* tasklist,
                        tasklist_container* filtered_tasklist)
{
    // inititalize a counter
    int counter = 0;

    for (int i = 0; i < category_list->number_of_categories; i++)
    {
        // found given category in categorylist
        if (category_list->list[i]->id == category_id)
        {
            for (int j = 0; j < tasklist->number_of_tasks; j++)
            {
                // filter by context
                if (category_identifier == '@')
                {
                    for (int k = 0; k < tasklist->list[j]->number_of_contexts; k++)
                    {
                        // populate filtered list
                        if (category_id == tasklist->list[j]->context[k])
                        {
                            filtered_tasklist->list[counter] = tasklist->list[j];
                            counter++;
                            filtered_tasklist->number_of_tasks = counter;

                        }
                    }
                }

                // filter by project
                if (category_identifier == '+')
                {
                    for (int k = 0; k < tasklist->list[j]->number_of_projects; k++)
                    {
                        // populate filtered list
                        if (category_id == tasklist->list[j]->project[k])
                        {
                            filtered_tasklist->list[counter] = tasklist->list[j];
                            counter++;
                            filtered_tasklist->number_of_tasks = counter;
                        }
                    }
                }
            }
        }
    }

    // check if any category was found
    if (counter == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}


/**
 * Determines which task has higher priority. Returns true if tasks are in correct order.
 */
bool compare_current_to_next (task* current_task, task* next_task)
{
    // determine maximum length for comparison of descriptions
    int maximum_length;
    int current_task_length = strlen(current_task->description);
    int next_task_length    = strlen(next_task->description);
    if (current_task_length < next_task_length)
    {
        maximum_length = current_task_length;
    }
    else
    {
         maximum_length = next_task_length;
    }

    // current is not completed and next is completd
    if (current_task->completed == false && next_task->completed == true)
    {
        return true;
    }

    // current is completed and next is not completd
    else if (current_task->completed == true && next_task->completed == false)
    {
        return false;
    }

    // current has a higher priority
    else if ((current_task->priority != 0 && next_task->priority == 0) ||
             (current_task->priority != 0 && current_task->priority < next_task->priority))
    {
        return true;
    }

    // current's description is lower within the alphabet closer to a)
    else if (current_task->priority == next_task->priority)
    {
        for (int i = 0; i < maximum_length; i ++)
        {
            int current_task_character = (tolower(current_task->description[i]));
            int next_task_character    = (tolower(next_task->description[i]));
            if (current_task_character < next_task_character)
            {
                return true;
            }
            else if (next_task_character < current_task_character)
            {
                return false;
            }
        }
    }

    return false;
}


/**
 * Sorts global tasklist
 */
void sort_tasklist(tasklist_container* tasklist)
{
    // create a temporary task
    task* tmp = malloc(sizeof(task));

    for (int i = 0; i < tasklist->number_of_tasks; i++)
    {
        for (int j = 0; j < tasklist->number_of_tasks-1; j++)
        {
            int current = j;
            int next = j + 1;
            // if the current task is larger than the next
            if (!compare_current_to_next(tasklist->list[current], tasklist->list[next]))
            {
                // swap positions of tasks within tasklist
                tmp                     = tasklist->list[current];
                tasklist->list[current] = tasklist->list[next];
                tasklist->list[next]    = tmp;
            }
        }
    }
    return;
}


/**
 * Deletes task from textlist marks task deleted in tasklist
 */
bool delete_task(const int id, textlist_container* textlist, tasklist_container* tasklist)
{
    bool task_deleted;
    bool line_deleted;

    for (int i = 0; i < textlist->number_of_lines; i++)
    {
        // found the right task
        if (tasklist->list[i]->id == id)
        {
            tasklist->list[i]->deleted = true;
            task_deleted = true;
        }

        // found the right line
        if (textlist->line[i]->id == id)
        {
            textlist->line[i]->raw_line[0] ='\0';
            textlist->line[i]->line_length = 0;
            line_deleted = true;
        }
    }

    tasklist->number_of_tasks--;

    if (task_deleted && line_deleted)
    {
        return true;
    }
    else
    {
        return false;
    }
}


/**
 * Unloads given textlist
 */
bool unload_textlist(textlist_container* textlist)
{
    // free textlist
    for (int i = 0; i < textlist->number_of_lines; i++)
    {
        free (textlist->line[i]);
    }

    free (textlist);

    return true;
}


/**
 * Unloads given tasklist
 */
bool unload_tasklist(tasklist_container* tasklist)
{
    // free tasklist
    for (int i = 0; i < tasklist->number_of_tasks; i++)
    {
        free (tasklist->list[i]);
    }

    free (tasklist);

    return true;
}


/**
 * Unloads given category list
 */
bool unload_category_list(category_container* category_list)
{
    // free category list
    for (int i = 0; i < category_list->number_of_categories; i++)
    {
        free (category_list->list[i]);
    }

    free (category_list);	

    return true;
}
