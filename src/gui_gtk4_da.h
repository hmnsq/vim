/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GUI_GTK4_DRAW_AREA_H
#ವ್ಯಾಖ್ಯಾನಿಸು GUI_GTK4_DRAW_AREA_H

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <gtk/gtk.h>

#ವ್ಯಾಖ್ಯಾನಿಸು VIM_TYPE_DRAW_AREA (vim_draw_area_get_type())
G_DECLARE_FINAL_TYPE(VimDrawArea, vim_draw_area, VIM, DRAW_AREA, GtkWidget)

GtkWidget *vim_draw_area_new(ಶೂನ್ಯ);
ಶೂನ್ಯ vim_draw_area_ಗಣ_ಗಾತ್ರ(VimDrawArea *self, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
ಪೂರ್ಣಾಂಕ vim_draw_area_draw_string(VimDrawArea *self, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, char_u *s, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ flags);
ಶೂನ್ಯ vim_draw_area_clear_ಖಂಡ(VimDrawArea *self, ಪೂರ್ಣಾಂಕ row1, ಪೂರ್ಣಾಂಕ col1, ಪೂರ್ಣಾಂಕ row2, ಪೂರ್ಣಾಂಕ col2);
ಶೂನ್ಯ vim_draw_area_clear(VimDrawArea *self);
ಶೂನ್ಯ vim_draw_area_move_ಖಂಡ(VimDrawArea *self, ಪೂರ್ಣಾಂಕ ಗಮ್ಯ, ಪೂರ್ಣಾಂಕ row1, ಪೂರ್ಣಾಂಕ row2, ಪೂರ್ಣಾಂಕ col1, ಪೂರ್ಣಾಂಕ col2);
ಶೂನ್ಯ vim_draw_area_ಗಣ_cursor(VimDrawArea *self, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h);
ಶೂನ್ಯ vim_draw_area_invert_ಖಂಡ(VimDrawArea *self, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ nrows, ಪೂರ್ಣಾಂಕ ncols);
#ಆದರೆ defined(FEAT_SIGN_ICONS)
ಶೂನ್ಯ vim_draw_area_ಕೂಡಿಸು_sign(VimDrawArea *self, GdkTexture *sign, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
ಶೂನ್ಯ vim_draw_area_ಕೂಡಿಸು_multisign(VimDrawArea *self, cairo_surface_t *surf, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_GUI
ಶೂನ್ಯ vim_draw_area_ಕೂಡಿಸು_external(VimDrawArea *self, Gsk_Render_ಗಂಟು *ಗಂಟು);
ಶೂನ್ಯ vim_draw_area_remove_external(VimDrawArea *self, Gsk_Render_ಗಂಟು *ಗಂಟು);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ
