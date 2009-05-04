/*
 * Copyright (c) 2003-2009 by FlashCode <flashcode@flashtux.org>
 * See README for License detail, AUTHORS for developers list.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* wee-infolist.c: manages info lists */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "weechat.h"
#include "wee-log.h"
#include "wee-string.h"
#include "wee-infolist.h"


struct t_infolist *weechat_infolists = NULL;
struct t_infolist *last_weechat_infolist = NULL;


/*
 * infolist_new: create a new weechat infolist
 */

struct t_infolist *
infolist_new ()
{
    struct t_infolist *new_infolist;

    new_infolist = malloc (sizeof (*new_infolist));
    if (new_infolist)
    {
        new_infolist->items = NULL;
        new_infolist->last_item = NULL;
        new_infolist->ptr_item = NULL;
        
        new_infolist->prev_infolist = last_weechat_infolist;
        new_infolist->next_infolist = NULL;
        if (weechat_infolists)
            last_weechat_infolist->next_infolist = new_infolist;
        else
            weechat_infolists = new_infolist;
        last_weechat_infolist = new_infolist;
    }
    
    return new_infolist;
}

/*
 * infolist_new_item: create a new item in an infolist
 */

struct t_infolist_item *
infolist_new_item (struct t_infolist *infolist)
{
    struct t_infolist_item *new_item;

    new_item = malloc (sizeof (*new_item));
    if (new_item)
    {
        new_item->vars = NULL;
        new_item->last_var = NULL;
        new_item->fields = NULL;
        
        new_item->prev_item = infolist->last_item;
        new_item->next_item = NULL;
        if (infolist->items)
            infolist->last_item->next_item = new_item;
        else
            infolist->items = new_item;
        infolist->last_item = new_item;
    }
    
    return new_item;
}

/*
 * infolist_new_var_integer: create a new integer variable in an item
 */

struct t_infolist_var *
infolist_new_var_integer (struct t_infolist_item *item,
                          const char *name, int value)
{
    struct t_infolist_var *new_var;
    
    if (!item || !name || !name[0])
        return NULL;
    
    new_var = malloc (sizeof (*new_var));
    if (new_var)
    {
        new_var->name = strdup (name);
        new_var->type = INFOLIST_INTEGER;
        new_var->value = malloc (sizeof (int));
        if (new_var->value)
            *((int *)new_var->value) = value;
        
        new_var->prev_var = item->last_var;
        new_var->next_var = NULL;
        if (item->vars)
            item->last_var->next_var = new_var;
        else
            item->vars = new_var;
        item->last_var = new_var;
    }
    
    return new_var;
}

/*
 * infolist_new_var_string: create a new string variable in an item
 */

struct t_infolist_var *
infolist_new_var_string (struct t_infolist_item *item,
                         const char *name, const char *value)
{
    struct t_infolist_var *new_var;
    
    if (!item || !name || !name[0])
        return NULL;
    
    new_var = malloc (sizeof (*new_var));
    if (new_var)
    {
        new_var->name = strdup (name);
        new_var->type = INFOLIST_STRING;
        new_var->value = (value) ? strdup (value) : NULL;
        
        new_var->prev_var = item->last_var;
        new_var->next_var = NULL;
        if (item->vars)
            item->last_var->next_var = new_var;
        else
            item->vars = new_var;
        item->last_var = new_var;
    }
    
    return new_var;
}

/*
 * infolist_new_var_pointer: create a new pointer variable in an item
 */

struct t_infolist_var *
infolist_new_var_pointer (struct t_infolist_item *item,
                          const char *name, void *pointer)
{
    struct t_infolist_var *new_var;
    
    if (!item || !name || !name[0])
        return NULL;
    
    new_var = malloc (sizeof (*new_var));
    if (new_var)
    {
        new_var->name = strdup (name);
        new_var->type = INFOLIST_POINTER;
        new_var->value = pointer;
        
        new_var->prev_var = item->last_var;
        new_var->next_var = NULL;
        if (item->vars)
            item->last_var->next_var = new_var;
        else
            item->vars = new_var;
        item->last_var = new_var;
    }
    
    return new_var;
}

/*
 * infolist_new_var_buffer: create a new buffer variable in an item
 */

struct t_infolist_var *
infolist_new_var_buffer (struct t_infolist_item *item,
                         const char *name, void *pointer, int size)
{
    struct t_infolist_var *new_var;
    
    if (!item || !name || !name[0] || (size <= 0))
        return NULL;
    
    new_var = malloc (sizeof (*new_var));
    if (new_var)
    {
        new_var->name = strdup (name);
        new_var->type = INFOLIST_BUFFER;
        new_var->value = malloc (size);
        if (new_var->value)
            memcpy (new_var->value, pointer, size);
        new_var->size = size;
        
        new_var->prev_var = item->last_var;
        new_var->next_var = NULL;
        if (item->vars)
            item->last_var->next_var = new_var;
        else
            item->vars = new_var;
        item->last_var = new_var;
    }
    
    return new_var;
}

/*
 * infolist_new_var_time: create a new time variable in an item
 */

struct t_infolist_var *
infolist_new_var_time (struct t_infolist_item *item,
                       const char *name, time_t time)
{
    struct t_infolist_var *new_var;
    
    if (!item || !name || !name[0])
        return NULL;
    
    new_var = malloc (sizeof (*new_var));
    if (new_var)
    {
        new_var->name = strdup (name);
        new_var->type = INFOLIST_TIME;
        new_var->value = malloc (sizeof (time_t));
        if (new_var->value)
            *((time_t *)new_var->value) = time;
        
        new_var->prev_var = item->last_var;
        new_var->next_var = NULL;
        if (item->vars)
            item->last_var->next_var = new_var;
        else
            item->vars = new_var;
        item->last_var = new_var;
    }
    
    return new_var;
}

/*
 * infolist_valid: check if an infolist pointer exists
 *                 return 1 if list exists
 *                        0 if list is not found
 */

int
infolist_valid (struct t_infolist *infolist)
{
    struct t_infolist *ptr_infolist;
    
    for (ptr_infolist = weechat_infolists; ptr_infolist;
         ptr_infolist = ptr_infolist->next_infolist)
    {
        if (ptr_infolist == infolist)
            return 1;
    }
    
    /* list not found */
    return 0;
}

/*
 * infolist_next: return next item for an infolist
 *                if current item pointer is NULL,
 *                then return first item of infolist
 */

struct t_infolist_item *
infolist_next (struct t_infolist *infolist)
{
    if (!infolist->ptr_item)
    {
        infolist->ptr_item = infolist->items;
        return infolist->ptr_item;
    }
    infolist->ptr_item = infolist->ptr_item->next_item;
    return infolist->ptr_item;
}

/*
 * infolist_prev: return previous item for an infolist
 *                if current item pointer is NULL,
 *                then return last item of infolist
 */

struct t_infolist_item *
infolist_prev (struct t_infolist *infolist)
{
    if (!infolist->ptr_item)
    {
        infolist->ptr_item = infolist->last_item;
        return infolist->ptr_item;
    }
    infolist->ptr_item = infolist->ptr_item->prev_item;
    return infolist->ptr_item;
}

/*
 * infolist_reset_item_cursor: reset pointer to current item in infolist
 */

void
infolist_reset_item_cursor (struct t_infolist *infolist)
{
    infolist->ptr_item = NULL;
}

/*
 * infolist_fields: get list of fields for current infolist item
 */

const char *
infolist_fields (struct t_infolist *infolist)
{
    struct t_infolist_var *ptr_var;
    int length;
    
    if (!infolist || !infolist->ptr_item)
        return NULL;

    /* list of fields already asked ? if yes, just return string */
    if (infolist->ptr_item->fields)
        return infolist->ptr_item->fields;
    
    length = 0;
    for (ptr_var = infolist->ptr_item->vars;
         ptr_var; ptr_var = ptr_var->next_var)
    {
        length += strlen (ptr_var->name) + 3;
    }
    
    infolist->ptr_item->fields = malloc (length + 1);
    if (!infolist->ptr_item->fields)
        return NULL;
    
    infolist->ptr_item->fields[0] = '\0';
    for (ptr_var = infolist->ptr_item->vars; ptr_var;
         ptr_var = ptr_var->next_var)
    {
        switch (ptr_var->type)
        {
            case INFOLIST_INTEGER:
                strcat (infolist->ptr_item->fields, "i:");
                break;
            case INFOLIST_STRING:
                strcat (infolist->ptr_item->fields, "s:");
                break;
            case INFOLIST_POINTER:
                strcat (infolist->ptr_item->fields, "p:");
                break;
            case INFOLIST_BUFFER:
                strcat (infolist->ptr_item->fields, "b:");
                break;
            case INFOLIST_TIME:
                strcat (infolist->ptr_item->fields, "t:");
                break;
        }
        strcat (infolist->ptr_item->fields, ptr_var->name);
        if (ptr_var->next_var)
            strcat (infolist->ptr_item->fields, ",");
    }
    
    return infolist->ptr_item->fields;
}

/*
 * infolist_integer: get an integer variable value in current infolist item
 */

int
infolist_integer (struct t_infolist *infolist, const char *var)
{
    struct t_infolist_var *ptr_var;
    
    if (!infolist || !infolist->ptr_item || !var || !var[0])
        return 0;
    
    for (ptr_var = infolist->ptr_item->vars; ptr_var;
         ptr_var = ptr_var->next_var)
    {
        if (string_strcasecmp (ptr_var->name, var) == 0)
        {
            if (ptr_var->type == INFOLIST_INTEGER)
                return *((int *)ptr_var->value);
            else
                return 0;
        }
    }
    
    /* variable not found */
    return 0;
}

/*
 * infolist_string: get a string variable value in current list item
 */

const char *
infolist_string (struct t_infolist *infolist, const char *var)
{
    struct t_infolist_var *ptr_var;
    
    if (!infolist || !infolist->ptr_item || !var || !var[0])
        return NULL;
    
    for (ptr_var = infolist->ptr_item->vars; ptr_var;
         ptr_var = ptr_var->next_var)
    {
        if (string_strcasecmp (ptr_var->name, var) == 0)
        {
            if (ptr_var->type == INFOLIST_STRING)
                return (char *)ptr_var->value;
            else
                return NULL;
        }
    }
    
    /* variable not found */
    return NULL;
}

/*
 * infolist_pointer: get a pointer variable value in current infolist item
 */

void *
infolist_pointer (struct t_infolist *infolist, const char *var)
{
    struct t_infolist_var *ptr_var;
    
    if (!infolist || !infolist->ptr_item || !var || !var[0])
        return NULL;
    
    for (ptr_var = infolist->ptr_item->vars; ptr_var;
         ptr_var = ptr_var->next_var)
    {
        if (string_strcasecmp (ptr_var->name, var) == 0)
        {
            if (ptr_var->type == INFOLIST_POINTER)
                return ptr_var->value;
            else
                return NULL;
        }
    }
    
    /* variable not found */
    return NULL;
}

/*
 * infolist_buffer: get a buffer variable value in current infolist item
 *                  size is used to return size of buffer
 */

void *
infolist_buffer (struct t_infolist *infolist, const char *var,
                 int *size)
{
    struct t_infolist_var *ptr_var;
    
    if (!infolist || !infolist->ptr_item || !var || !var[0])
        return NULL;
    
    for (ptr_var = infolist->ptr_item->vars; ptr_var;
         ptr_var = ptr_var->next_var)
    {
        if (string_strcasecmp (ptr_var->name, var) == 0)
        {
            if (ptr_var->type == INFOLIST_BUFFER)
            {
                *size = ptr_var->size;
                return ptr_var->value;
            }
            else
                return NULL;
        }
    }
    
    /* variable not found */
    return NULL;
}

/*
 * infolist_time: get a time variable value in current infolist item
 */

time_t
infolist_time (struct t_infolist *infolist, const char *var)
{
    struct t_infolist_var *ptr_var;
    
    if (!infolist || !infolist->ptr_item || !var || !var[0])
        return 0;
    
    for (ptr_var = infolist->ptr_item->vars; ptr_var;
         ptr_var = ptr_var->next_var)
    {
        if (string_strcasecmp (ptr_var->name, var) == 0)
        {
            if (ptr_var->type == INFOLIST_TIME)
                return *((time_t *)ptr_var->value);
            else
                return 0;
        }
    }
    
    /* variable not found */
    return 0;
}

/*
 * infolist_var_free: free an infolist variable
 */

void
infolist_var_free (struct t_infolist_item *item,
                   struct t_infolist_var *var)
{
    struct t_infolist_var *new_vars;
    
    /* remove var */
    if (item->last_var == var)
        item->last_var = var->prev_var;
    if (var->prev_var)
    {
        (var->prev_var)->next_var = var->next_var;
        new_vars = item->vars;
    }
    else
        new_vars = var->next_var;
    
    if (var->next_var)
        (var->next_var)->prev_var = var->prev_var;
    
    /* free data */
    if (var->name)
        free (var->name);
    if (((var->type == INFOLIST_INTEGER)
         || (var->type == INFOLIST_STRING)
         || (var->type == INFOLIST_BUFFER)
         || (var->type == INFOLIST_TIME))
        && var->value)
    {
        free (var->value);
    }
    
    free (var);
    
    item->vars = new_vars;
}

/*
 * infolist_item_free: free an infolist item
 */

void
infolist_item_free (struct t_infolist *infolist,
                    struct t_infolist_item *item)
{
    struct t_infolist_item *new_items;
    
    /* remove var */
    if (infolist->last_item == item)
        infolist->last_item = item->prev_item;
    if (item->prev_item)
    {
        (item->prev_item)->next_item = item->next_item;
        new_items = infolist->items;
    }
    else
        new_items = item->next_item;
    
    if (item->next_item)
        (item->next_item)->prev_item = item->prev_item;
    
    /* free data */
    while (item->vars)
    {
        infolist_var_free (item, item->vars);
    }
    if (item->fields)
        free (item->fields);
    
    free (item);
    
    infolist->items = new_items;
}

/*
 * infolist_free: free an infolist
 */

void
infolist_free (struct t_infolist *infolist)
{
    struct t_infolist *new_weechat_infolists;
    
    /* remove list */
    if (last_weechat_infolist == infolist)
        last_weechat_infolist = infolist->prev_infolist;
    if (infolist->prev_infolist)
    {
        (infolist->prev_infolist)->next_infolist = infolist->next_infolist;
        new_weechat_infolists = weechat_infolists;
    }
    else
        new_weechat_infolists = infolist->next_infolist;
    
    if (infolist->next_infolist)
        (infolist->next_infolist)->prev_infolist = infolist->prev_infolist;
    
    /* free data */
    while (infolist->items)
    {
        infolist_item_free (infolist, infolist->items);
    }
    
    free (infolist);
    
    weechat_infolists = new_weechat_infolists;
}

/*
 * infolist_print_log: print infolists infos in log (usually for crash dump)
 */

void
infolist_print_log ()
{
    struct t_infolist *ptr_infolist;
    struct t_infolist_item *ptr_item;
    struct t_infolist_var *ptr_var;
    
    for (ptr_infolist = weechat_infolists; ptr_infolist;
         ptr_infolist = ptr_infolist->next_infolist)
    {
        log_printf ("");
        log_printf ("[infolist (addr:0x%lx)]", ptr_infolist);
        log_printf ("  items. . . . . . . . . : 0x%lx", ptr_infolist->items);
        log_printf ("  last_item. . . . . . . : 0x%lx", ptr_infolist->last_item);
        log_printf ("  ptr_item . . . . . . . : 0x%lx", ptr_infolist->ptr_item);
        log_printf ("  prev_infolist. . . . . : 0x%lx", ptr_infolist->prev_infolist);
        log_printf ("  next_infolist. . . . . : 0x%lx", ptr_infolist->next_infolist);
        
        for (ptr_item = ptr_infolist->items; ptr_item;
             ptr_item = ptr_item->next_item)
        {
            log_printf ("");
            log_printf ("    [item (addr:0x%lx)]", ptr_item);
            log_printf ("      vars . . . . . . . . . : 0x%lx", ptr_item->vars);
            log_printf ("      last_var . . . . . . . : 0x%lx", ptr_item->last_var);
            log_printf ("      prev_item. . . . . . . : 0x%lx", ptr_item->prev_item);
            log_printf ("      next_item. . . . . . . : 0x%lx", ptr_item->next_item);
            
            for (ptr_var = ptr_item->vars; ptr_var;
                 ptr_var = ptr_var->next_var)
            {
                log_printf ("");
                log_printf ("      [var (addr:0x%lx)]", ptr_var);
                log_printf ("        name . . . . . . . . : '%s'", ptr_var->name);
                log_printf ("        type . . . . . . . . : %d",   ptr_var->type);
                switch (ptr_var->type)
                {
                    case INFOLIST_INTEGER:
                        log_printf ("        value (integer). . . : %d",    *((int *)ptr_var->value));
                        break;
                    case INFOLIST_STRING:
                        log_printf ("        value (string) . . . : '%s'",  (char *)ptr_var->value);
                        break;
                    case INFOLIST_POINTER:
                        log_printf ("        value (pointer). . . : 0x%lx", ptr_var->value);
                        break;
                    case INFOLIST_BUFFER:
                        log_printf ("        value (buffer) . . . : 0x%lx", ptr_var->value);
                        log_printf ("        size of buffer . . . : %d",    ptr_var->size);
                        break;
                    case INFOLIST_TIME:
                        log_printf ("        value (time) . . . . : %ld", *((time_t *)ptr_var->value));
                        break;
                }
                log_printf ("        prev_var . . . . . . : 0x%lx", ptr_var->prev_var);
                log_printf ("        next_var . . . . . . : 0x%lx", ptr_var->next_var);
            }
        }
    }
}