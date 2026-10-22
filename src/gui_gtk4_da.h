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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଅନ୍ତର୍ଭାଵ <gtk/gtk.h>

#define VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(ଶୂନ୍ୟ);
ଶୂନ୍ୟ vim_draw_area_set_size(VimDrawArea *self, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vim_draw_area_draw_string(VimDrawArea *self, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ଶୂନ୍ୟ vim_draw_area_clear_block(VimDrawArea *self, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col2);
ଶୂନ୍ୟ vim_draw_area_clear(VimDrawArea *self);
ଶୂନ୍ୟ vim_draw_area_move_block(VimDrawArea *self, ପୂର୍ଣ୍ଣସଂଖ୍ୟା to, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col2);
ଶୂନ୍ୟ vim_draw_area_set_cursor(VimDrawArea *self, ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h);
ଶୂନ୍ୟ vim_draw_area_invert_block(VimDrawArea *self, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nrows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ncols);
#ଯଦି defined(FEAT_SIGN_ICONS)
ଶୂନ୍ୟ vim_draw_area_add_sign(VimDrawArea *self, GdkTexture *sign, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height);
#endif
#ifdef FEAT_NETBEANS_INTG
ଶୂନ୍ୟ vim_draw_area_add_multisign(VimDrawArea *self, cairo_surface_t *surf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height);
#endif
#ifdef FEAT_IMAGE_GUI
ଶୂନ୍ୟ vim_draw_area_add_external(VimDrawArea *self, Gsk_Render_ଗଣ୍ଠି *ଗଣ୍ଠି);
ଶୂନ୍ୟ vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_ଗଣ୍ଠି *ଗଣ୍ଠି);
#endif

#endif
