/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#నిర్వచితం_కాకపోతే GUI_GTK4_MENU_H
#నిర్వచించు GUI_GTK4_MENU_H

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_MENU

# include <gtk/gtk.h>

# define VIM_TYPE_MENU_BAR_అంశం (vim_menu_bar_అంశం_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_Bar_అంశం_2, vim_menu_bar_అంశం, VIM, MENU_BAR_అంశం, GtkButton)

# define VIM_TYPE_MENU_BAR (vim_menu_bar_get_type())
G_DECLARE_FINAL_TYPE(VimMenuBar, vim_menu_bar, VIM, MENU_BAR, GtkWidget)

# define VIM_TYPE_MENU_అంశం (vim_menu_అంశం_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_అంశం, vim_menu_అంశం, VIM, MENU_అంశం, GtkButton)

# define VIM_TYPE_MENU (vim_menu_get_type())
G_DECLARE_FINAL_TYPE(VimMenu, vim_menu, VIM, MENU, GtkPopover)

రకనిర్వచనం గణన
{
    VIM_MENU_అంశం_CLICKED,
    VIM_MENU_అంశం_SELECTED
} Vim_Menu_అంశం_Event;

రకనిర్వచనం శూన్య (*Vim_Menu_అంశం_Func)(Vim_Menu_అంశం *అంశం, Vim_Menu_అంశం_Event event, శూన్య *udata);

GtkWidget *vim_menu_bar_అంశం_new(స్థిరరాశి అక్షరం *పాఠ్యం, VimMenu *menu);
శూన్య vim_menu_bar_అంశం_సమితి_పాఠ్యం(Vim_Menu_Bar_అంశం_2 *self, స్థిరరాశి అక్షరం *పాఠ్యం);

GtkWidget *vim_menu_bar_new(శూన్య);
GtkWidget *vim_menu_bar_గమ్యం_menu(VimMenuBar *self);
శూన్య vim_menu_bar_చొప్పించు_అంశం(VimMenuBar *self, Vim_Menu_Bar_అంశం_2 *అంశం, పూర్ణాంకం idx);
శూన్య vim_menu_bar_remove(VimMenuBar *self, GtkWidget *అంశం);
శూన్య vim_menu_bar_show(VimMenuBar *self, Vim_Menu_Bar_అంశం_2 *అంశం);

GtkWidget *vim_menu_అంశం_new(స్థిరరాశి అక్షరం *పాఠ్యం, Vim_Menu_అంశం_Func func, శూన్య *udata);
శూన్య vim_menu_అంశం_సమితి_పాఠ్యం(Vim_Menu_అంశం *self, స్థిరరాశి అక్షరం *పాఠ్యం);
శూన్య vim_menu_అంశం_సమితి_accel(Vim_Menu_అంశం *self, స్థిరరాశి అక్షరం *accel_పాఠ్యం);
శూన్య vim_menu_అంశం_సమితి_submenu(Vim_Menu_అంశం *self, VimMenu *submenu);

GtkWidget *vim_menu_new(శూన్య);
శూన్య vim_menu_చొప్పించు_అంశం(VimMenu *self, Vim_Menu_అంశం *అంశం, పూర్ణాంకం idx);
GtkWidget *vim_menu_చొప్పించు_separator(VimMenu *self, పూర్ణాంకం idx);
శూన్య vim_menu_remove(VimMenu *self, GtkWidget *అంశం);
GtkWidget *vim_menu_copy(VimMenu *self);

#అయితే_ముగింపు

#అయితే_ముగింపు
