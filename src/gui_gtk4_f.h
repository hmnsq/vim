/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#నిర్వచితం_కాకపోతే GUI_GTK4_FORM_H
#నిర్వచించు GUI_GTK4_FORM_H

#అన్తర్భావ <gtk/gtk.h>

#నిర్వచించు VIM_TYPE_FORM (vim_form_get_type())
G_DECLARE_FINAL_TYPE(VimForm, vim_form, VIM, FORM, GtkWidget)

GtkWidget *vim_form_new(శూన్య);
శూన్య vim_form_put(VimForm *self, GtkWidget *widget, పూర్ణాంకం x, పూర్ణాంకం y);
శూన్య vim_form_move(VimForm *self, GtkWidget *widget, పూర్ణాంకం x, పూర్ణాంకం y);
శూన్య vim_form_move_resize(VimForm *self, GtkWidget *widget, gint x, gint y, gint w, gint h);
శూన్య vim_form_remove(VimForm *self, GtkWidget *widget);

#అయితే_ముగింపు
