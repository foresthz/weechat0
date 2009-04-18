/*
 * Copyright (c) 2003-2007 by FlashCode <flashcode@flashtux.org>
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

/* This header is designed to be distributed with WeeChat plugins, if scripts
   management is needed */

#ifndef __WEECHAT_WEECHAT_SCRIPT_H
#define __WEECHAT_WEECHAT_SCRIPT_H 1

/* constants which defines return types for weechat_<lang>_exec functions */
#define SCRIPT_EXEC_INT    1
#define SCRIPT_EXEC_STRING 2

typedef struct t_plugin_script t_plugin_script;

struct t_plugin_script
{
    /* script variables */
    char *filename;                     /* name of script on disk              */
    void *interpreter;                  /* interpreter for script              */
    char *name;                         /* script name                         */
    char *description;                  /* plugin description                  */
    char *version;                      /* plugin version                      */
    char *shutdown_func;                /* function when script is unloaded    */
    char *charset;                      /* script charset                      */
    
    t_plugin_script *prev_script;       /* link to previous script             */
    t_plugin_script *next_script;       /* link to next script                 */
};

extern void weechat_script_auto_load (t_weechat_plugin *, char *,
                                      int (*)(t_weechat_plugin *, char *));
extern t_plugin_script *weechat_script_search (t_weechat_plugin *,
                                               t_plugin_script **, char *);
extern char *weechat_script_search_full_name (t_weechat_plugin *,
                                              char *, char *);
extern t_plugin_script *weechat_script_add (t_weechat_plugin *,
                                            t_plugin_script **, char *, char *,
                                            char *, char *, char *, char *);
extern void weechat_script_remove (t_weechat_plugin *,
                                   t_plugin_script **, t_plugin_script *);
extern void weechat_script_print (t_weechat_plugin *,
                                  t_plugin_script *,
                                  char *, char *, char *, ...);
extern void weechat_script_print_server (t_weechat_plugin *,
                                         t_plugin_script *,
                                         char *, ...);
extern void weechat_script_print_infobar (t_weechat_plugin *,
                                          t_plugin_script *,
                                          int, char *, ...);
extern void weechat_script_log (t_weechat_plugin *,
                                t_plugin_script *,
                                char *, char *, char *, ...);
extern void weechat_script_exec_command (t_weechat_plugin *,
                                         t_plugin_script *,
                                         char *, char *, char *);
extern void weechat_script_remove_handler (t_weechat_plugin *,
                                           t_plugin_script *,
                                           char *, char *);
extern void weechat_script_remove_timer_handler (t_weechat_plugin *,
                                                 t_plugin_script *,
                                                 char *);
extern void weechat_script_remove_keyboard_handler (t_weechat_plugin *,
                                                    t_plugin_script *,
                                                    char *);
extern void weechat_script_remove_event_handler (t_weechat_plugin *,
                                                 t_plugin_script *,
                                                 char *);
extern void weechat_script_remove_modifier (t_weechat_plugin *,
                                            t_plugin_script *,
                                            char *, char *, char *);
extern char *weechat_script_get_plugin_config (t_weechat_plugin *,
                                               t_plugin_script *,
                                               char *);
extern int weechat_script_set_plugin_config (t_weechat_plugin *,
                                             t_plugin_script *,
                                             char *, char *);
extern void weechat_script_set_charset (t_weechat_plugin *,
                                        t_plugin_script *,
                                        char *);

#endif /* weechat-script.h */