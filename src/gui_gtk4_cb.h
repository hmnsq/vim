/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#که_نه_تعریف_شوی GUI_GTK4_CB_H
#تعریف_کړه GUI_GTK4_CB_H

#ضمن "vim.h"
#ضمن <gtk/gtk.h>

#تعریف_کړه VIM_TYPE_CONTENT_PROVIDER (vim_content_provider_get_type())
G_DECLARE_FINAL_TYPE(VimContentProvider, vim_content_provider, VIM, CONTENT_PROVIDER, GdkContentProvider)

خارجي ثابت حرف *supported_mimes[];

GdkContentProvider *vim_content_provider_new(Clipboard_T *cbd);

#د_که_پای
