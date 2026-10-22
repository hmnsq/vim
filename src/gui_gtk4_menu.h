/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GUI_GTK4_MENU_H
#ವ್ಯಾಖ್ಯಾನಿಸು GUI_GTK4_MENU_H

#ಅನ್ತರ್ಭಾವ "vim.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU

# include <gtk/gtk.h>

# define VIM_TYPE_MENU_BAR_ಅಂಶ (vim_menu_bar_ಅಂಶ_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_Bar_ಅಂಶ_2, vim_menu_bar_ಅಂಶ, VIM, MENU_BAR_ಅಂಶ, GtkButton)

# define VIM_TYPE_MENU_BAR (vim_menu_bar_get_type())
G_DECLARE_FINAL_TYPE(VimMenuBar, vim_menu_bar, VIM, MENU_BAR, GtkWidget)

# define VIM_TYPE_MENU_ಅಂಶ (vim_menu_ಅಂಶ_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_ಅಂಶ, vim_menu_ಅಂಶ, VIM, MENU_ಅಂಶ, GtkButton)

# define VIM_TYPE_MENU (vim_menu_get_type())
G_DECLARE_FINAL_TYPE(VimMenu, vim_menu, VIM, MENU, GtkPopover)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    VIM_MENU_ಅಂಶ_CLICKED,
    VIM_MENU_ಅಂಶ_SELECTED
} Vim_Menu_ಅಂಶ_Event;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*Vim_Menu_ಅಂಶ_Func)(Vim_Menu_ಅಂಶ *ಅಂಶ, Vim_Menu_ಅಂಶ_Event event, ಶೂನ್ಯ *udata);

GtkWidget *vim_menu_bar_ಅಂಶ_new(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, VimMenu *menu);
ಶೂನ್ಯ vim_menu_bar_ಅಂಶ_ಗಣ_ಪಠ್ಯ(Vim_Menu_Bar_ಅಂಶ_2 *self, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ);

GtkWidget *vim_menu_bar_new(ಶೂನ್ಯ);
GtkWidget *vim_menu_bar_ಗಮ್ಯ_menu(VimMenuBar *self);
ಶೂನ್ಯ vim_menu_bar_ಸೇರಿಸು_ಅಂಶ(VimMenuBar *self, Vim_Menu_Bar_ಅಂಶ_2 *ಅಂಶ, ಪೂರ್ಣಾಂಕ idx);
ಶೂನ್ಯ vim_menu_bar_remove(VimMenuBar *self, GtkWidget *ಅಂಶ);
ಶೂನ್ಯ vim_menu_bar_show(VimMenuBar *self, Vim_Menu_Bar_ಅಂಶ_2 *ಅಂಶ);

GtkWidget *vim_menu_ಅಂಶ_new(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, Vim_Menu_ಅಂಶ_Func func, ಶೂನ್ಯ *udata);
ಶೂನ್ಯ vim_menu_ಅಂಶ_ಗಣ_ಪಠ್ಯ(Vim_Menu_ಅಂಶ *self, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ);
ಶೂನ್ಯ vim_menu_ಅಂಶ_ಗಣ_accel(Vim_Menu_ಅಂಶ *self, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *accel_ಪಠ್ಯ);
ಶೂನ್ಯ vim_menu_ಅಂಶ_ಗಣ_submenu(Vim_Menu_ಅಂಶ *self, VimMenu *submenu);

GtkWidget *vim_menu_new(ಶೂನ್ಯ);
ಶೂನ್ಯ vim_menu_ಸೇರಿಸು_ಅಂಶ(VimMenu *self, Vim_Menu_ಅಂಶ *ಅಂಶ, ಪೂರ್ಣಾಂಕ idx);
GtkWidget *vim_menu_ಸೇರಿಸು_separator(VimMenu *self, ಪೂರ್ಣಾಂಕ idx);
ಶೂನ್ಯ vim_menu_remove(VimMenu *self, GtkWidget *ಅಂಶ);
GtkWidget *vim_menu_copy(VimMenu *self);

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ
