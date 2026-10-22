/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#اگر_تعریف_نشده GUI_GTK4_FORM_H
#تعریف_کن GUI_GTK4_FORM_H

#ضمن <gtk/gtk.h>

#تعریف_کن VIM_TYPE_FORM (vim_form_get_type())
G_DECLARE_FINAL_TYPE(VimForm, vim_form, VIM, FORM, GtkWidget)

GtkWidget *vim_form_new(فراغ);
فراغ vim_form_put(VimForm *self, GtkWidget *widget, صحيح x, صحيح y);
فراغ vim_form_move(VimForm *self, GtkWidget *widget, صحيح x, صحيح y);
فراغ vim_form_move_resize(VimForm *self, GtkWidget *widget, gint x, gint y, gint w, gint h);
فراغ vim_form_remove(VimForm *self, GtkWidget *widget);

#پایان_اگر
