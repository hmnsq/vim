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

#अन्तर्भाव "vim.h"

#अन्तर्भाव <gtk/gtk.h>

#define VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(शून्य);
शून्य vim_draw_area_set_size(VimDrawArea *self, पूर्णांक rows, पूर्णांक cols);
पूर्णांक vim_draw_area_draw_string(VimDrawArea *self, पूर्णांक row, पूर्णांक col, char_u *s, पूर्णांक len, पूर्णांक flags);
शून्य vim_draw_area_clear_block(VimDrawArea *self, पूर्णांक row1, पूर्णांक col1, पूर्णांक row2, पूर्णांक col2);
शून्य vim_draw_area_clear(VimDrawArea *self);
शून्य vim_draw_area_move_block(VimDrawArea *self, पूर्णांक to, पूर्णांक row1, पूर्णांक row2, पूर्णांक col1, पूर्णांक col2);
शून्य vim_draw_area_set_cursor(VimDrawArea *self, पूर्णांक w, पूर्णांक h);
शून्य vim_draw_area_invert_block(VimDrawArea *self, पूर्णांक row, पूर्णांक col, पूर्णांक nrows, पूर्णांक ncols);
#जर defined(FEAT_SIGN_ICONS)
शून्य vim_draw_area_add_sign(VimDrawArea *self, GdkTexture *sign, पूर्णांक row, पूर्णांक col, पूर्णांक width, पूर्णांक height);
#endif
#ifdef FEAT_NETBEANS_INTG
शून्य vim_draw_area_add_multisign(VimDrawArea *self, cairo_surface_t *surf, पूर्णांक row, पूर्णांक col, पूर्णांक width, पूर्णांक height);
#endif
#ifdef FEAT_IMAGE_GUI
शून्य vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_गाठ *गाठ);
शून्य vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_गाठ *गाठ);
#endif

#endif
