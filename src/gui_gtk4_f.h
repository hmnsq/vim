/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GUI_GTK4_FORM_H
#ವ್ಯಾಖ್ಯಾನಿಸು GUI_GTK4_FORM_H

#ಅನ್ತರ್ಭಾವ <gtk/gtk.h>

#ವ್ಯಾಖ್ಯಾನಿಸು VIM_TYPE_FORM (vim_form_get_type())
G_DECLARE_FINAL_TYPE(VimForm, vim_form, VIM, FORM, GtkWidget)

GtkWidget *vim_form_new(ಶೂನ್ಯ);
ಶೂನ್ಯ vim_form_put(VimForm *self, GtkWidget *widget, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y);
ಶೂನ್ಯ vim_form_move(VimForm *self, GtkWidget *widget, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y);
ಶೂನ್ಯ vim_form_move_resize(VimForm *self, GtkWidget *widget, gint x, gint y, gint w, gint h);
ಶೂನ್ಯ vim_form_remove(VimForm *self, GtkWidget *widget);

#ಆದರೆ_ಅಂತ್ಯ
