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

#അന്തര്ഭാവ "vim.h"

#അന്തര്ഭാവ <gtk/gtk.h>

#define VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(ശൂന്യം);
ശൂന്യം vim_draw_area_set_size(VimDrawArea *self, പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols);
പൂർണ്ണസംഖ്യ vim_draw_area_draw_string(VimDrawArea *self, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, char_u *s, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ flags);
ശൂന്യം vim_draw_area_clear_block(VimDrawArea *self, പൂർണ്ണസംഖ്യ row1, പൂർണ്ണസംഖ്യ col1, പൂർണ്ണസംഖ്യ row2, പൂർണ്ണസംഖ്യ col2);
ശൂന്യം vim_draw_area_clear(VimDrawArea *self);
ശൂന്യം vim_draw_area_move_block(VimDrawArea *self, പൂർണ്ണസംഖ്യ to, പൂർണ്ണസംഖ്യ row1, പൂർണ്ണസംഖ്യ row2, പൂർണ്ണസംഖ്യ col1, പൂർണ്ണസംഖ്യ col2);
ശൂന്യം vim_draw_area_set_cursor(VimDrawArea *self, പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h);
ശൂന്യം vim_draw_area_invert_block(VimDrawArea *self, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ nrows, പൂർണ്ണസംഖ്യ ncols);
#എങ്കിൽ defined(FEAT_SIGN_ICONS)
ശൂന്യം vim_draw_area_add_sign(VimDrawArea *self, GdkTexture *sign, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ height);
#endif
#ifdef FEAT_NETBEANS_INTG
ശൂന്യം vim_draw_area_add_multisign(VimDrawArea *self, cairo_surface_t *surf, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ height);
#endif
#ifdef FEAT_IMAGE_GUI
ശൂന്യം vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_കണ്ണി *കണ്ണി);
ശൂന്യം vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_കണ്ണി *കണ്ണി);
#endif

#endif
