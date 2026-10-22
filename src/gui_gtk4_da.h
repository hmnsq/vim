/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#اگر_تعریف_نشده GUI_GTK4_DRAW_AREA_H
#تعریف_کن GUI_GTK4_DRAW_AREA_H

#ضمن "vim.h"

#ضمن <gtk/gtk.h>

#تعریف_کن VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(فراغ);
فراغ vim_draw_area_مجموعه_اندازه(VimDrawArea *self, صحيح ردیفها_2, صحيح تعداد_ستون);
صحيح vim_draw_area_draw_string(VimDrawArea *self, صحيح ردیف, صحيح ستون, char_u *s, صحيح len, صحيح flags);
فراغ vim_draw_area_clear_بلوک(VimDrawArea *self, صحيح row1, صحيح col1, صحيح row2, صحيح col2);
فراغ vim_draw_area_clear(VimDrawArea *self);
فراغ vim_draw_area_move_بلوک(VimDrawArea *self, صحيح مقصد, صحيح row1, صحيح row2, صحيح col1, صحيح col2);
فراغ vim_draw_area_مجموعه_cursor(VimDrawArea *self, صحيح w, صحيح h);
فراغ vim_draw_area_invert_بلوک(VimDrawArea *self, صحيح ردیف, صحيح ستون, صحيح nrows, صحيح ncols);
#إذا defined(FEAT_SIGN_ICONS)
فراغ vim_draw_area_جمع_کن_sign(VimDrawArea *self, GdkTexture *sign, صحيح ردیف, صحيح ستون, صحيح عرض, صحيح ارتفاع);
#پایان_اگر
#اگر_تعریف_شده FEAT_NETBEANS_INTG
فراغ vim_draw_area_جمع_کن_multisign(VimDrawArea *self, cairo_surface_t *surf, صحيح ردیف, صحيح ستون, صحيح عرض, صحيح ارتفاع);
#پایان_اگر
#اگر_تعریف_شده FEAT_IMAGE_GUI
فراغ vim_draw_area_جمع_کن_external(VimDrawArea *self, Gsk_Render_گره *گره);
فراغ vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_گره *گره);
#پایان_اگر

#پایان_اگر
