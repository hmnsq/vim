/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#جے_بنا_تعریف GUI_GTK4_DRAW_AREA_H
#تعریف_کرو GUI_GTK4_DRAW_AREA_H

#ضمن "vim.h"

#ضمن <gtk/gtk.h>

#تعریف_کرو VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(فراغ);
فراغ vim_draw_area_سیٹ_ناپ(VimDrawArea *self, صحيح صفاں_2, صحيح خانے_گنتی);
صحيح vim_draw_area_draw_string(VimDrawArea *self, صحيح صف, صحيح خانہ, char_u *s, صحيح len, صحيح flags);
فراغ vim_draw_area_clear_بلاک(VimDrawArea *self, صحيح row1, صحيح col1, صحيح row2, صحيح col2);
فراغ vim_draw_area_clear(VimDrawArea *self);
فراغ vim_draw_area_move_بلاک(VimDrawArea *self, صحيح منزل, صحيح row1, صحيح row2, صحيح col1, صحيح col2);
فراغ vim_draw_area_سیٹ_cursor(VimDrawArea *self, صحيح w, صحيح h);
فراغ vim_draw_area_invert_بلاک(VimDrawArea *self, صحيح صف, صحيح خانہ, صحيح nrows, صحيح ncols);
#إذا defined(FEAT_SIGN_ICONS)
فراغ vim_draw_area_جوڑو_sign(VimDrawArea *self, GdkTexture *sign, صحيح صف, صحيح خانہ, صحيح چوڑائی, صحيح اچائی);
#جے_انت
#جے_تعریف_شدہ FEAT_NETBEANS_INTG
فراغ vim_draw_area_جوڑو_multisign(VimDrawArea *self, cairo_surface_t *surf, صحيح صف, صحيح خانہ, صحيح چوڑائی, صحيح اچائی);
#جے_انت
#جے_تعریف_شدہ FEAT_IMAGE_GUI
فراغ vim_draw_area_جوڑو_external(VimDrawArea *self, Gsk_Render_گرہ *گرہ);
فراغ vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_گرہ *گرہ);
#جے_انت

#جے_انت
