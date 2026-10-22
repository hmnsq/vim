/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#నిర్వచితం_కాకపోతే GUI_GTK4_CB_H
#నిర్వచించు GUI_GTK4_CB_H

#అన్తర్భావ "vim.h"
#అన్తర్భావ <gtk/gtk.h>

#నిర్వచించు VIM_TYPE_CONTENT_PROVIDER (vim_content_provider_get_type())
G_DECLARE_FINAL_TYPE(VimContentProvider, vim_content_provider, VIM, CONTENT_PROVIDER, GdkContentProvider)

బాహ్య స్థిరరాశి అక్షరం *supported_mimes[];

GdkContentProvider *vim_content_provider_new(Clipboard_T *cbd);

#అయితే_ముగింపు
