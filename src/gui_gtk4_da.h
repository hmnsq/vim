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
void vim_draw_area_set_size(VimDrawArea *self, පූර්ණ_සංඛ්‍යා rows, පූර්ණ_සංඛ්‍යා cols);
පූර්ණ_සංඛ්‍යා vim_draw_area_draw_string(VimDrawArea *self, පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා col, char_u *s, පූර්ණ_සංඛ්‍යා len, පූර්ණ_සංඛ්‍යා flags);
void vim_draw_area_clear_block(VimDrawArea *self, පූර්ණ_සංඛ්‍යා row1, පූර්ණ_සංඛ්‍යා col1, පූර්ණ_සංඛ්‍යා row2, පූර්ණ_සංඛ්‍යා col2);
void vim_draw_area_clear(VimDrawArea *self);
void vim_draw_area_move_block(VimDrawArea *self, පූර්ණ_සංඛ්‍යා to, පූර්ණ_සංඛ්‍යා row1, පූර්ණ_සංඛ්‍යා row2, පූර්ණ_සංඛ්‍යා col1, පූර්ණ_සංඛ්‍යා col2);
void vim_draw_area_set_cursor(VimDrawArea *self, පූර්ණ_සංඛ්‍යා w, පූර්ණ_සංඛ්‍යා h);
void vim_draw_area_invert_block(VimDrawArea *self, පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා col, පූර්ණ_සංඛ්‍යා nrows, පූර්ණ_සංඛ්‍යා ncols);
#නම් defined(FEAT_SIGN_ICONS)
void vim_draw_area_add_sign(VimDrawArea *self, GdkTexture *sign, පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා col, පූර්ණ_සංඛ්‍යා width, පූර්ණ_සංඛ්‍යා height);
#endif
#ifdef FEAT_NETBEANS_INTG
void vim_draw_area_add_multisign(VimDrawArea *self, cairo_surface_t *surf, පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා col, පූර්ණ_සංඛ්‍යා width, පූර්ණ_සංඛ්‍යා height);
#endif
#ifdef FEAT_IMAGE_GUI
void vim_draw_area_add_external(VimDrawArea *self, GskRenderNode *node);
void vim_draw_area_remove_external(VimDrawArea *self, GskRenderNode *node);
#endif

#endif
