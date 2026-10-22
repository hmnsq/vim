/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#اگر_غیر_تعریف_شدہ GUI_GTK4_DRAW_AREA_H
#تعریف_کرو GUI_GTK4_DRAW_AREA_H

#شامل "vim.h"

#شامل <gtk/gtk.h>

#تعریف_کرو VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(خالی);
خالی vim_draw_area_سیٹ_سائز(VimDrawArea *self, صحیح صفیں_2, صحیح ستون_تعداد);
صحیح vim_draw_area_draw_string(VimDrawArea *self, صحیح صف, صحیح ستون, char_u *s, صحیح len, صحیح flags);
خالی vim_draw_area_clear_بلاک(VimDrawArea *self, صحیح row1, صحیح col1, صحیح row2, صحیح col2);
خالی vim_draw_area_clear(VimDrawArea *self);
خالی vim_draw_area_move_بلاک(VimDrawArea *self, صحیح مقصد, صحیح row1, صحیح row2, صحیح col1, صحیح col2);
خالی vim_draw_area_سیٹ_cursor(VimDrawArea *self, صحیح w, صحیح h);
خالی vim_draw_area_invert_بلاک(VimDrawArea *self, صحیح صف, صحیح ستون, صحیح nrows, صحیح ncols);
#اگر defined(FEAT_SIGN_ICONS)
خالی vim_draw_area_جوڑو_sign(VimDrawArea *self, GdkTexture *sign, صحیح صف, صحیح ستون, صحیح چوڑائی, صحیح اونچائی);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
خالی vim_draw_area_جوڑو_multisign(VimDrawArea *self, cairo_surface_t *surf, صحیح صف, صحیح ستون, صحیح چوڑائی, صحیح اونچائی);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_IMAGE_GUI
خالی vim_draw_area_جوڑو_external(VimDrawArea *self, Gsk_Render_گرہ *گرہ);
خالی vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_گرہ *گرہ);
#اگر_ختم

#اگر_ختم
