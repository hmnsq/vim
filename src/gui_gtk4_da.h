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

#include "vim.h"

#include <gtk/gtk.h>

#define VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(void);
void vim_draw_area_set_size(VimDrawArea *self, 정수 rows, 정수 cols);
정수 vim_draw_area_draw_string(VimDrawArea *self, 정수 row, 정수 col, char_u *s, 정수 len, 정수 flags);
void vim_draw_area_clear_block(VimDrawArea *self, 정수 row1, 정수 col1, 정수 row2, 정수 col2);
void vim_draw_area_clear(VimDrawArea *self);
void vim_draw_area_move_block(VimDrawArea *self, 정수 to, 정수 row1, 정수 row2, 정수 col1, 정수 col2);
void vim_draw_area_set_cursor(VimDrawArea *self, 정수 w, 정수 h);
void vim_draw_area_invert_block(VimDrawArea *self, 정수 row, 정수 col, 정수 nrows, 정수 ncols);
#만약 defined(FEAT_SIGN_ICONS)
void vim_draw_area_add_sign(VimDrawArea *self, GdkTexture *sign, 정수 row, 정수 col, 정수 width, 정수 height);
#endif
#ifdef FEAT_NETBEANS_INTG
void vim_draw_area_add_multisign(VimDrawArea *self, cairo_surface_t *surf, 정수 row, 정수 col, 정수 width, 정수 height);
#endif
#ifdef FEAT_IMAGE_GUI
void vim_draw_area_add_external(VimDrawArea *self, GskRenderNode *node);
void vim_draw_area_remove_external(VimDrawArea *self, GskRenderNode *node);
#endif

#endif
