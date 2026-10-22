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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <gtk/gtk.h>

#define VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(ಶೂನ್ಯ);
ಶೂನ್ಯ vim_draw_area_set_size(VimDrawArea *self, ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols);
ಪೂರ್ಣಾಙ್ಕ vim_draw_area_draw_string(VimDrawArea *self, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, char_u *s, ಪೂರ್ಣಾಙ್ಕ len, ಪೂರ್ಣಾಙ್ಕ flags);
ಶೂನ್ಯ vim_draw_area_clear_block(VimDrawArea *self, ಪೂರ್ಣಾಙ್ಕ row1, ಪೂರ್ಣಾಙ್ಕ col1, ಪೂರ್ಣಾಙ್ಕ row2, ಪೂರ್ಣಾಙ್ಕ col2);
ಶೂನ್ಯ vim_draw_area_clear(VimDrawArea *self);
ಶೂನ್ಯ vim_draw_area_move_block(VimDrawArea *self, ಪೂರ್ಣಾಙ್ಕ to, ಪೂರ್ಣಾಙ್ಕ row1, ಪೂರ್ಣಾಙ್ಕ row2, ಪೂರ್ಣಾಙ್ಕ col1, ಪೂರ್ಣಾಙ್ಕ col2);
ಶೂನ್ಯ vim_draw_area_set_cursor(VimDrawArea *self, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h);
ಶೂನ್ಯ vim_draw_area_invert_block(VimDrawArea *self, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ nrows, ಪೂರ್ಣಾಙ್ಕ ncols);
#ಯದಿ defined(FEAT_SIGN_ICONS)
ಶೂನ್ಯ vim_draw_area_add_sign(VimDrawArea *self, GdkTexture *sign, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height);
#endif
#ifdef FEAT_NETBEANS_INTG
ಶೂನ್ಯ vim_draw_area_add_multisign(VimDrawArea *self, cairo_surface_t *surf, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height);
#endif
#ifdef FEAT_IMAGE_GUI
ಶೂನ್ಯ vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_ಗಂಟು *ಗಂಟು);
ಶೂನ್ಯ vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_ಗಂಟು *ಗಂಟು);
#endif

#endif
