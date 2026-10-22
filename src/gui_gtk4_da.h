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

#અન્તર્ભાવ "vim.h"

#અન્તર્ભાવ <gtk/gtk.h>

#define VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(શૂન્ય);
શૂન્ય vim_draw_area_set_size(VimDrawArea *self, પૂર્ણાંક rows, પૂર્ણાંક cols);
પૂર્ણાંક vim_draw_area_draw_string(VimDrawArea *self, પૂર્ણાંક row, પૂર્ણાંક col, char_u *s, પૂર્ણાંક len, પૂર્ણાંક flags);
શૂન્ય vim_draw_area_clear_block(VimDrawArea *self, પૂર્ણાંક row1, પૂર્ણાંક col1, પૂર્ણાંક row2, પૂર્ણાંક col2);
શૂન્ય vim_draw_area_clear(VimDrawArea *self);
શૂન્ય vim_draw_area_move_block(VimDrawArea *self, પૂર્ણાંક to, પૂર્ણાંક row1, પૂર્ણાંક row2, પૂર્ણાંક col1, પૂર્ણાંક col2);
શૂન્ય vim_draw_area_set_cursor(VimDrawArea *self, પૂર્ણાંક w, પૂર્ણાંક h);
શૂન્ય vim_draw_area_invert_block(VimDrawArea *self, પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક nrows, પૂર્ણાંક ncols);
#જો defined(FEAT_SIGN_ICONS)
શૂન્ય vim_draw_area_add_sign(VimDrawArea *self, GdkTexture *sign, પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક width, પૂર્ણાંક height);
#endif
#ifdef FEAT_NETBEANS_INTG
શૂન્ય vim_draw_area_add_multisign(VimDrawArea *self, cairo_surface_t *surf, પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક width, પૂર્ણાંક height);
#endif
#ifdef FEAT_IMAGE_GUI
શૂન્ય vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_ગાંઠ *ગાંઠ);
શૂન્ય vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_ગાંઠ *ગાંઠ);
#endif

#endif
