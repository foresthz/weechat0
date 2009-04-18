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

/* gui-gtk-main.c: main loop for Gtk GUI */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "../../common/weechat.h"
#include "../gui.h"
#include "../../common/fifo.h"
#include "../../common/utf8.h"
#include "../../common/weeconfig.h"
#include "gui-gtk.h"

#ifdef PLUGINS
#include "../../plugins/plugins.h"
#endif


GtkWidget *gui_gtk_main_window;
GtkWidget *gui_gtk_vbox1;
GtkWidget *gui_gtk_entry_topic;
GtkWidget *gui_gtk_notebook1;
GtkWidget *gui_gtk_vbox2;
GtkWidget *gui_gtk_hbox1;
GtkWidget *gui_gtk_hpaned1;
GtkWidget *gui_gtk_scrolledwindow_chat;
GtkWidget *gui_gtk_scrolledwindow_nick;
GtkWidget *gui_gtk_entry_input;
GtkWidget *gui_gtk_label1;


/*
 * gui_main_loop: main loop for WeeChat with Gtk GUI
 */

void
gui_main_loop ()
{
    /* TODO: write this function for Gtk */
    gtk_main ();
}

/*
 * gui_main_pre_init: pre-initialize GUI (called before gui_init)
 */

void
gui_main_pre_init (int *argc, char **argv[])
{
    /* Initialise Gtk */
    gtk_init (argc, argv);
}

/*
 * gui_main_init: init GUI
 */

void
gui_main_init ()
{
    GdkColor color_fg, color_bg;
    
    gui_color_init ();
    
    gui_infobar = NULL;
    
    gui_ok = 1;
    
    /* init clipboard buffer */
    gui_input_clipboard = NULL;
    
    /* create Gtk widgets */

    gdk_color_parse ("white", &color_fg);
    gdk_color_parse ("black", &color_bg);
    
    gui_gtk_main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (gui_gtk_main_window), PACKAGE_STRING);
    
    g_signal_connect (G_OBJECT (gui_gtk_main_window), "destroy", gtk_main_quit, NULL);
    
    gui_gtk_vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (gui_gtk_vbox1);
    gtk_container_add (GTK_CONTAINER (gui_gtk_main_window), gui_gtk_vbox1);
    
    gui_gtk_entry_topic = gtk_entry_new ();
    gtk_widget_show (gui_gtk_entry_topic);
    gtk_box_pack_start (GTK_BOX (gui_gtk_vbox1), gui_gtk_entry_topic, FALSE, FALSE, 0);
    gtk_widget_modify_text (gui_gtk_entry_topic, GTK_STATE_NORMAL, &color_fg);
    gtk_widget_modify_base (gui_gtk_entry_topic, GTK_STATE_NORMAL, &color_bg);
    
    gui_gtk_notebook1 = gtk_notebook_new ();
    gtk_widget_show (gui_gtk_notebook1);
    gtk_box_pack_start (GTK_BOX (gui_gtk_vbox1), gui_gtk_notebook1, TRUE, TRUE, 0);
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK (gui_gtk_notebook1), GTK_POS_BOTTOM);
    
    gui_gtk_vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (gui_gtk_vbox2);
    gtk_container_add (GTK_CONTAINER (gui_gtk_notebook1), gui_gtk_vbox2);
    
    gui_gtk_hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (gui_gtk_hbox1);
    gtk_box_pack_start (GTK_BOX (gui_gtk_vbox2), gui_gtk_hbox1, TRUE, TRUE, 0);
    
    gui_gtk_hpaned1 = gtk_hpaned_new ();
    gtk_widget_show (gui_gtk_hpaned1);
    gtk_box_pack_start (GTK_BOX (gui_gtk_hbox1), gui_gtk_hpaned1, TRUE, TRUE, 0);
    gtk_paned_set_position (GTK_PANED (gui_gtk_hpaned1), 0);
    
    gui_gtk_scrolledwindow_chat = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (gui_gtk_scrolledwindow_chat);
    gtk_paned_pack1 (GTK_PANED (gui_gtk_hpaned1), gui_gtk_scrolledwindow_chat,
                     FALSE, TRUE);
    //gtk_box_pack_start (GTK_PANED (hpaned1), scrolledwindow_chat, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (gui_gtk_scrolledwindow_chat),
                                    GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_widget_modify_text (gui_gtk_scrolledwindow_chat, GTK_STATE_NORMAL, &color_fg);
    gtk_widget_modify_base (gui_gtk_scrolledwindow_chat, GTK_STATE_NORMAL, &color_bg);
    
    gui_gtk_scrolledwindow_nick = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (gui_gtk_scrolledwindow_nick);
    gtk_paned_pack2 (GTK_PANED (gui_gtk_hpaned1), gui_gtk_scrolledwindow_nick,
                     FALSE, TRUE);
    //gtk_box_pack_start (GTK_PANED (hpaned1), scrolledwindow_nick, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (gui_gtk_scrolledwindow_nick),
                                    GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_widget_modify_text (gui_gtk_scrolledwindow_nick, GTK_STATE_NORMAL, &color_fg);
    gtk_widget_modify_base (gui_gtk_scrolledwindow_nick, GTK_STATE_NORMAL, &color_bg);
    
    gui_gtk_entry_input = gtk_entry_new ();
    gtk_widget_show (gui_gtk_entry_input);
    gtk_box_pack_start (GTK_BOX (gui_gtk_vbox2), gui_gtk_entry_input, FALSE,
                        FALSE, 0);
    gtk_widget_modify_text (gui_gtk_entry_input, GTK_STATE_NORMAL, &color_fg);
    gtk_widget_modify_base (gui_gtk_entry_input, GTK_STATE_NORMAL, &color_bg);
    
    gui_gtk_label1 = gtk_label_new (_("server"));
    gtk_widget_show (gui_gtk_label1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (gui_gtk_notebook1),
                                gtk_notebook_get_nth_page (GTK_NOTEBOOK (gui_gtk_notebook1), 0),
                                gui_gtk_label1);
    gtk_label_set_justify (GTK_LABEL (gui_gtk_label1), GTK_JUSTIFY_LEFT);
    
    gtk_widget_show_all (gui_gtk_main_window);
    
    /* create new window/buffer */
    if (gui_window_new (NULL, 0, 0, 0, 0, 100, 100))
    {
        gui_current_window = gui_windows;
        gui_buffer_new (gui_windows, NULL, NULL, 0, 1);
        
        if (cfg_look_set_title)
            gui_window_set_title ();
        
        gui_init_ok = 1;
    }
}

/*
 * gui_main_end: GUI end
 */

void
gui_main_end ()
{
    t_gui_window *ptr_win;
    
    /* free clipboard buffer */
    if (gui_input_clipboard)
      free(gui_input_clipboard);

    /* delete all panels */
    while (gui_panels)
        gui_panel_free (gui_panels);
    
    /* delete all windows */
    for (ptr_win = gui_windows; ptr_win; ptr_win = ptr_win->next_window)
    {
        /* TODO: destroy Gtk widgets */
    }
    
    /* delete all buffers */
    while (gui_buffers)
        gui_buffer_free (gui_buffers, 0);
    
    /* delete all windows */
    while (gui_windows)
        gui_window_free (gui_windows);
    gui_window_tree_free (&gui_windows_tree);
    
    /* delete global history */
    history_global_free ();
    
    /* delete infobar messages */
    while (gui_infobar)
        gui_infobar_remove ();

    /* reset title */
    if (cfg_look_set_title)
	gui_window_reset_title ();
}