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
void vim_draw_area_set_size(VimDrawArea *self, ចំនួនគត់ rows, ចំនួនគត់ cols);
ចំនួនគត់ vim_draw_area_draw_string(VimDrawArea *self, ចំនួនគត់ row, ចំនួនគត់ col, char_u *s, ចំនួនគត់ len, ចំនួនគត់ flags);
void vim_draw_area_clear_block(VimDrawArea *self, ចំនួនគត់ row1, ចំនួនគត់ col1, ចំនួនគត់ row2, ចំនួនគត់ col2);
void vim_draw_area_clear(VimDrawArea *self);
void vim_draw_area_move_block(VimDrawArea *self, ចំនួនគត់ to, ចំនួនគត់ row1, ចំនួនគត់ row2, ចំនួនគត់ col1, ចំនួនគត់ col2);
void vim_draw_area_set_cursor(VimDrawArea *self, ចំនួនគត់ w, ចំនួនគត់ h);
void vim_draw_area_invert_block(VimDrawArea *self, ចំនួនគត់ row, ចំនួនគត់ col, ចំនួនគត់ nrows, ចំនួនគត់ ncols);
#ប្រសិនបើ defined(FEAT_SIGN_ICONS)
void vim_draw_area_add_sign(VimDrawArea *self, GdkTexture *sign, ចំនួនគត់ row, ចំនួនគត់ col, ចំនួនគត់ width, ចំនួនគត់ height);
#endif
#ifdef FEAT_NETBEANS_INTG
void vim_draw_area_add_multisign(VimDrawArea *self, cairo_surface_t *surf, ចំនួនគត់ row, ចំនួនគត់ col, ចំនួនគត់ width, ចំនួនគត់ height);
#endif
#ifdef FEAT_IMAGE_GUI
void vim_draw_area_add_external(VimDrawArea *self, GskRenderNode *node);
void vim_draw_area_remove_external(VimDrawArea *self, GskRenderNode *node);
#endif

#endif
