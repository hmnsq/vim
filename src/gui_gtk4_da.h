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
void vim_draw_area_set_size(VimDrawArea *self, puur_naa_nka rows, puur_naa_nka cols);
puur_naa_nka vim_draw_area_draw_string(VimDrawArea *self, puur_naa_nka row, puur_naa_nka col, char_u *s, puur_naa_nka len, puur_naa_nka flags);
void vim_draw_area_clear_block(VimDrawArea *self, puur_naa_nka row1, puur_naa_nka col1, puur_naa_nka row2, puur_naa_nka col2);
void vim_draw_area_clear(VimDrawArea *self);
void vim_draw_area_move_block(VimDrawArea *self, puur_naa_nka to, puur_naa_nka row1, puur_naa_nka row2, puur_naa_nka col1, puur_naa_nka col2);
void vim_draw_area_set_cursor(VimDrawArea *self, puur_naa_nka w, puur_naa_nka h);
void vim_draw_area_invert_block(VimDrawArea *self, puur_naa_nka row, puur_naa_nka col, puur_naa_nka nrows, puur_naa_nka ncols);
#ᮚᮦᮔ᮪ defined(FEAT_SIGN_ICONS)
void vim_draw_area_add_sign(VimDrawArea *self, GdkTexture *sign, puur_naa_nka row, puur_naa_nka col, puur_naa_nka width, puur_naa_nka height);
#endif
#ifdef FEAT_NETBEANS_INTG
void vim_draw_area_add_multisign(VimDrawArea *self, cairo_surface_t *surf, puur_naa_nka row, puur_naa_nka col, puur_naa_nka width, puur_naa_nka height);
#endif
#ifdef FEAT_IMAGE_GUI
void vim_draw_area_add_external(VimDrawArea *self, GskRenderNode *node);
void vim_draw_area_remove_external(VimDrawArea *self, GskRenderNode *node);
#endif

#endif
