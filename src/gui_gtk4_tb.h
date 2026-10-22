/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GUI_GTK4_TB_H
#ವ್ಯಾಖ್ಯಾನಿಸು GUI_GTK4_TB_H

#ಅನ್ತರ್ಭಾವ "vim.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR

# include <gtk/gtk.h>

# define VIM_TYPE_TOOLBAR (vim_toolbar_get_type())
G_DECLARE_FINAL_TYPE(VimToolbar, vim_toolbar, VIM, TOOLBAR, GtkWidget)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Vim_Toolbar_ಅಂಶ Vim_Toolbar_ಅಂಶ_2;

GtkWidget *vim_toolbar_new(ಶೂನ್ಯ);
GtkWidget *vim_toolbar_ಸೇರಿಸು_button(VimToolbar *self, GtkWidget *icon, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ idx);
GtkWidget *vim_toolbar_ಸೇರಿಸು_separator(VimToolbar *self, ಪೂರ್ಣಾಂಕ idx);
ಶೂನ್ಯ vim_toolbar_ಗಣ_style(VimToolbar *self, ಪೂರ್ಣಾಂಕ style, ಪೂರ್ಣಾಂಕ iconsize);
ಶೂನ್ಯ vim_toolbar_remove(VimToolbar *self, GtkWidget *ಅಂಶ);

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ
