/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#నిర్వచితం_కాకపోతే GUI_GTK4_TB_H
#నిర్వచించు GUI_GTK4_TB_H

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_TOOLBAR

# include <gtk/gtk.h>

# define VIM_TYPE_TOOLBAR (vim_toolbar_get_type())
G_DECLARE_FINAL_TYPE(VimToolbar, vim_toolbar, VIM, TOOLBAR, GtkWidget)

రకనిర్వచనం నిర్మాణం Vim_Toolbar_అంశం Vim_Toolbar_అంశం_2;

GtkWidget *vim_toolbar_new(శూన్య);
GtkWidget *vim_toolbar_చొప్పించు_button(VimToolbar *self, GtkWidget *icon, స్థిరరాశి అక్షరం *పాఠ్యం, పూర్ణాంకం idx);
GtkWidget *vim_toolbar_చొప్పించు_separator(VimToolbar *self, పూర్ణాంకం idx);
శూన్య vim_toolbar_సమితి_style(VimToolbar *self, పూర్ణాంకం style, పూర్ణాంకం iconsize);
శూన్య vim_toolbar_remove(VimToolbar *self, GtkWidget *అంశం);

#అయితే_ముగింపు

#అయితే_ముగింపు
