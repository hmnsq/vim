/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#جيڪڏهن_وصف_نه_ٿيل GUI_GTK4_DRAW_AREA_H
#وصف_ڪريو GUI_GTK4_DRAW_AREA_H

#ضمن "vim.h"

#ضمن <gtk/gtk.h>

#وصف_ڪريو VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(فراغ);
فراغ vim_draw_area_سيٽ_سائيز(VimDrawArea *self, صحيح صفون_2, صحيح کانن_ڳڻپ);
صحيح vim_draw_area_draw_string(VimDrawArea *self, صحيح صف, صحيح کانو, char_u *s, صحيح len, صحيح flags);
فراغ vim_draw_area_clear_بلاڪ(VimDrawArea *self, صحيح row1, صحيح col1, صحيح row2, صحيح col2);
فراغ vim_draw_area_clear(VimDrawArea *self);
فراغ vim_draw_area_move_بلاڪ(VimDrawArea *self, صحيح منزل, صحيح row1, صحيح row2, صحيح col1, صحيح col2);
فراغ vim_draw_area_سيٽ_cursor(VimDrawArea *self, صحيح w, صحيح h);
فراغ vim_draw_area_invert_بلاڪ(VimDrawArea *self, صحيح صف, صحيح کانو, صحيح nrows, صحيح ncols);
#إذا defined(FEAT_SIGN_ICONS)
فراغ vim_draw_area_جوڙيو_sign(VimDrawArea *self, GdkTexture *sign, صحيح صف, صحيح کانو, صحيح ويڪر, صحيح اوچائي);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
فراغ vim_draw_area_جوڙيو_multisign(VimDrawArea *self, cairo_surface_t *surf, صحيح صف, صحيح کانو, صحيح ويڪر, صحيح اوچائي);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_IMAGE_GUI
فراغ vim_draw_area_جوڙيو_external(VimDrawArea *self, Gsk_Render_ڳنڍ *ڳنڍ);
فراغ vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_ڳنڍ *ڳنڍ);
#جيڪڏهن_پورو

#جيڪڏهن_پورو
