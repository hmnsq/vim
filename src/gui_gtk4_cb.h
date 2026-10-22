/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GUI_GTK4_CB_H
#ವ್ಯಾಖ್ಯಾನಿಸು GUI_GTK4_CB_H

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ <gtk/gtk.h>

#ವ್ಯಾಖ್ಯಾನಿಸು VIM_TYPE_CONTENT_PROVIDER (vim_content_provider_get_type())
G_DECLARE_FINAL_TYPE(VimContentProvider, vim_content_provider, VIM, CONTENT_PROVIDER, GdkContentProvider)

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *supported_mimes[];

GdkContentProvider *vim_content_provider_new(Clipboard_T *cbd);

#ಆದರೆ_ಅಂತ್ಯ
