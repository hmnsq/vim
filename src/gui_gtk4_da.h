/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ifndef GUI_GTK4_DRAW_AREA_H
#define GUI_GTK4_DRAW_AREA_H

#অন্তর্ভাব "vim.h"

#অন্তর্ভাব <gtk/gtk.h>

#define VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(শূন্য);
শূন্য vim_draw_area_set_size(VimDrawArea *self, পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols);
পূর্ণাঙ্ক vim_draw_area_draw_string(VimDrawArea *self, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, char_u *s, পূর্ণাঙ্ক len, পূর্ণাঙ্ক flags);
শূন্য vim_draw_area_clear_block(VimDrawArea *self, পূর্ণাঙ্ক row1, পূর্ণাঙ্ক col1, পূর্ণাঙ্ক row2, পূর্ণাঙ্ক col2);
শূন্য vim_draw_area_clear(VimDrawArea *self);
শূন্য vim_draw_area_move_block(VimDrawArea *self, পূর্ণাঙ্ক to, পূর্ণাঙ্ক row1, পূর্ণাঙ্ক row2, পূর্ণাঙ্ক col1, পূর্ণাঙ্ক col2);
শূন্য vim_draw_area_set_cursor(VimDrawArea *self, পূর্ণাঙ্ক w, পূর্ণাঙ্ক h);
শূন্য vim_draw_area_invert_block(VimDrawArea *self, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, পূর্ণাঙ্ক nrows, পূর্ণাঙ্ক ncols);
#যদি defined(FEAT_SIGN_ICONS)
শূন্য vim_draw_area_add_sign(VimDrawArea *self, GdkTexture *sign, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, পূর্ণাঙ্ক width, পূর্ণাঙ্ক height);
#endif
#ifdef FEAT_NETBEANS_INTG
শূন্য vim_draw_area_add_multisign(VimDrawArea *self, cairo_surface_t *surf, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, পূর্ণাঙ্ক width, পূর্ণাঙ্ক height);
#endif
#ifdef FEAT_IMAGE_GUI
শূন্য vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_গ্রন্থি *গ্রন্থি);
শূন্য vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_গ্রন্থি *গ্রন্থি);
#endif

#endif
