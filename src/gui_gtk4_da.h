/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#నిర్వచితం_కాకపోతే GUI_GTK4_DRAW_AREA_H
#నిర్వచించు GUI_GTK4_DRAW_AREA_H

#అన్తర్భావ "vim.h"

#అన్తర్భావ <gtk/gtk.h>

#నిర్వచించు VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(శూన్య);
శూన్య vim_draw_area_సమితి_కొలత(VimDrawArea *self, పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య);
పూర్ణాంకం vim_draw_area_draw_string(VimDrawArea *self, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, char_u *s, పూర్ణాంకం len, పూర్ణాంకం flags);
శూన్య vim_draw_area_clear_ఖండం(VimDrawArea *self, పూర్ణాంకం row1, పూర్ణాంకం col1, పూర్ణాంకం row2, పూర్ణాంకం col2);
శూన్య vim_draw_area_clear(VimDrawArea *self);
శూన్య vim_draw_area_move_ఖండం(VimDrawArea *self, పూర్ణాంకం గమ్యం, పూర్ణాంకం row1, పూర్ణాంకం row2, పూర్ణాంకం col1, పూర్ణాంకం col2);
శూన్య vim_draw_area_సమితి_cursor(VimDrawArea *self, పూర్ణాంకం w, పూర్ణాంకం h);
శూన్య vim_draw_area_invert_ఖండం(VimDrawArea *self, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం nrows, పూర్ణాంకం ncols);
#ఒకవేళ defined(FEAT_SIGN_ICONS)
శూన్య vim_draw_area_కూడు_sign(VimDrawArea *self, GdkTexture *sign, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
శూన్య vim_draw_area_కూడు_multisign(VimDrawArea *self, cairo_surface_t *surf, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_IMAGE_GUI
శూన్య vim_draw_area_కూడు_external(VimDrawArea *self, Gsk_Render_కణుపు *కణుపు);
శూన్య vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_కణుపు *కణుపు);
#అయితే_ముగింపు

#అయితే_ముగింపు
