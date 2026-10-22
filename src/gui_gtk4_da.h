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
शून्य vim_draw_area_set_size(VimDrawArea *self, पूर्णाङ्क rows, पूर्णाङ्क cols);
पूर्णाङ्क vim_draw_area_draw_string(VimDrawArea *self, पूर्णाङ्क row, पूर्णाङ्क col, char_u *s, पूर्णाङ्क len, पूर्णाङ्क flags);
शून्य vim_draw_area_clear_block(VimDrawArea *self, पूर्णाङ्क row1, पूर्णाङ्क col1, पूर्णाङ्क row2, पूर्णाङ्क col2);
शून्य vim_draw_area_clear(VimDrawArea *self);
शून्य vim_draw_area_move_block(VimDrawArea *self, पूर्णाङ्क to, पूर्णाङ्क row1, पूर्णाङ्क row2, पूर्णाङ्क col1, पूर्णाङ्क col2);
शून्य vim_draw_area_set_cursor(VimDrawArea *self, पूर्णाङ्क w, पूर्णाङ्क h);
शून्य vim_draw_area_invert_block(VimDrawArea *self, पूर्णाङ्क row, पूर्णाङ्क col, पूर्णाङ्क nrows, पूर्णाङ्क ncols);
#यदि defined(FEAT_SIGN_ICONS)
शून्य vim_draw_area_add_sign(VimDrawArea *self, GdkTexture *sign, पूर्णाङ्क row, पूर्णाङ्क col, पूर्णाङ्क width, पूर्णाङ्क height);
#endif
#ifdef FEAT_NETBEANS_INTG
शून्य vim_draw_area_add_multisign(VimDrawArea *self, cairo_surface_t *surf, पूर्णाङ्क row, पूर्णाङ्क col, पूर्णाङ्क width, पूर्णाङ्क height);
#endif
#ifdef FEAT_IMAGE_GUI
शून्य vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_गाँठो *गाँठो);
शून्य vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_गाँठो *गाँठो);
#endif

#endif
