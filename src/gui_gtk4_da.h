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

#شامل "vim.h"

#شامل <gtk/gtk.h>

#define VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(تهی);
تهی vim_draw_area_set_size(VimDrawArea *self, صحیح rows, صحیح cols);
صحیح vim_draw_area_draw_string(VimDrawArea *self, صحیح row, صحیح col, char_u *s, صحیح len, صحیح flags);
تهی vim_draw_area_clear_block(VimDrawArea *self, صحیح row1, صحیح col1, صحیح row2, صحیح col2);
تهی vim_draw_area_clear(VimDrawArea *self);
تهی vim_draw_area_move_block(VimDrawArea *self, صحیح to, صحیح row1, صحیح row2, صحیح col1, صحیح col2);
تهی vim_draw_area_set_cursor(VimDrawArea *self, صحیح w, صحیح h);
تهی vim_draw_area_invert_block(VimDrawArea *self, صحیح row, صحیح col, صحیح nrows, صحیح ncols);
#اگر defined(FEAT_SIGN_ICONS)
تهی vim_draw_area_add_sign(VimDrawArea *self, GdkTexture *sign, صحیح row, صحیح col, صحیح width, صحیح height);
#endif
#ifdef FEAT_NETBEANS_INTG
تهی vim_draw_area_add_multisign(VimDrawArea *self, cairo_surface_t *surf, صحیح row, صحیح col, صحیح width, صحیح height);
#endif
#ifdef FEAT_IMAGE_GUI
تهی vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_گره *گره);
تهی vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_گره *گره);
#endif

#endif
