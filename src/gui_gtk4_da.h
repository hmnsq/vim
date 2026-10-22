/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ GUI_GTK4_DRAW_AREA_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUI_GTK4_DRAW_AREA_H

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <gtk/gtk.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(ਖਾਲੀ);
ਖਾਲੀ vim_draw_area_ਸੈੱਟ_ਮਾਪ(VimDrawArea *self, ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ);
ਪੂਰਨ vim_draw_area_draw_string(VimDrawArea *self, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, char_u *s, ਪੂਰਨ len, ਪੂਰਨ flags);
ਖਾਲੀ vim_draw_area_clear_ਖੰਡ(VimDrawArea *self, ਪੂਰਨ row1, ਪੂਰਨ col1, ਪੂਰਨ row2, ਪੂਰਨ col2);
ਖਾਲੀ vim_draw_area_clear(VimDrawArea *self);
ਖਾਲੀ vim_draw_area_move_ਖੰਡ(VimDrawArea *self, ਪੂਰਨ ਮੰਜ਼ਿਲ, ਪੂਰਨ row1, ਪੂਰਨ row2, ਪੂਰਨ col1, ਪੂਰਨ col2);
ਖਾਲੀ vim_draw_area_ਸੈੱਟ_cursor(VimDrawArea *self, ਪੂਰਨ w, ਪੂਰਨ h);
ਖਾਲੀ vim_draw_area_invert_ਖੰਡ(VimDrawArea *self, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ nrows, ਪੂਰਨ ncols);
#ਜੇ defined(FEAT_SIGN_ICONS)
ਖਾਲੀ vim_draw_area_ਜੋੜੋ_sign(VimDrawArea *self, GdkTexture *sign, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
ਖਾਲੀ vim_draw_area_ਜੋੜੋ_multisign(VimDrawArea *self, cairo_surface_t *surf, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_GUI
ਖਾਲੀ vim_draw_area_ਜੋੜੋ_external(VimDrawArea *self, Gsk_Render_ਗੰਢ *ਗੰਢ);
ਖਾਲੀ vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_ਗੰਢ *ਗੰਢ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
