/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ GUI_GTK4_FORM_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUI_GTK4_FORM_H

#ਅਨ੍ਤਰ੍ਭਾਵ <gtk/gtk.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_TYPE_FORM (vim_form_get_type())
G_DECLARE_FINAL_TYPE(VimForm, vim_form, VIM, FORM, GtkWidget)

GtkWidget *vim_form_new(ਖਾਲੀ);
ਖਾਲੀ vim_form_put(VimForm *self, GtkWidget *widget, ਪੂਰਨ x, ਪੂਰਨ y);
ਖਾਲੀ vim_form_move(VimForm *self, GtkWidget *widget, ਪੂਰਨ x, ਪੂਰਨ y);
ਖਾਲੀ vim_form_move_resize(VimForm *self, GtkWidget *widget, gint x, gint y, gint w, gint h);
ਖਾਲੀ vim_form_remove(VimForm *self, GtkWidget *widget);

#ਜੇ_ਅੰਤ
