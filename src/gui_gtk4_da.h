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

#அந்தர்பாவ "vim.h"

#அந்தர்பாவ <gtk/gtk.h>

#define VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(வெறுமை);
வெறுமை vim_draw_area_set_size(VimDrawArea *self, முழு rows, முழு cols);
முழு vim_draw_area_draw_string(VimDrawArea *self, முழு row, முழு col, char_u *s, முழு len, முழு flags);
வெறுமை vim_draw_area_clear_block(VimDrawArea *self, முழு row1, முழு col1, முழு row2, முழு col2);
வெறுமை vim_draw_area_clear(VimDrawArea *self);
வெறுமை vim_draw_area_move_block(VimDrawArea *self, முழு to, முழு row1, முழு row2, முழு col1, முழு col2);
வெறுமை vim_draw_area_set_cursor(VimDrawArea *self, முழு w, முழு h);
வெறுமை vim_draw_area_invert_block(VimDrawArea *self, முழு row, முழு col, முழு nrows, முழு ncols);
#எனில் defined(FEAT_SIGN_ICONS)
வெறுமை vim_draw_area_add_sign(VimDrawArea *self, GdkTexture *sign, முழு row, முழு col, முழு width, முழு height);
#endif
#ifdef FEAT_NETBEANS_INTG
வெறுமை vim_draw_area_add_multisign(VimDrawArea *self, cairo_surface_t *surf, முழு row, முழு col, முழு width, முழு height);
#endif
#ifdef FEAT_IMAGE_GUI
வெறுமை vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_முனை *முனை);
வெறுமை vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_முனை *முனை);
#endif

#endif
