/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"
#ضمن <gtk/gtk.h>
#ضمن "gui_gtk4_cb.h"

بنية _VimContentProvider
{
    GdkContentProvider پلار;

    // Clipboard this content provider is associated with.
    Clipboard_T *cbd;
};

// Note that order is important, mime types placed first have the highest
// priority for GTK when looking what mime type to receive from.
//
// NOTE: GTK4 only supports conforming mime types, meaning formats like
// "_VIMENC_TEXT" will not work. See
// https://gitlab.gnome.org/GNOME/gtk/-/work_items/4087
// and
// https://discourse.gnome.org/t/gtk4-clipboard-does-not-provide-contents-using-custom-mime-type-without-character/6858
// To get around this, add a new VIM*_MIMETYPE_NAME conforming mime type.
ثابت حرف *supported_mimes[] = {
    VIMENC_MIMETYPE_NAME,
    VIM_MIMETYPE_NAME,
    "text/html",
    "text/plain;charset=utf-8",
    "text/plain",
    عدم // gdk_clipboard_read_async expects array to be NULL terminated.
};
#تعریف_کړه SUPPORTED_MIMES_LEN (ARRAY_اوږدتیا(supported_mimes) - 1)

G_DEFINE_TYPE(VimContentProvider, vim_content_provider, GDK_TYPE_CONTENT_PROVIDER)

ساكن GdkContentFormats *vim_content_provider_ref_formats(GdkContentProvider *cp);
ساكن فراغ vim_content_provider_write_mime_type_async(GdkContentProvider *cp, ثابت حرف *mime_type, GOutput_بهیر *بهیر, صحيح io_لومړیتوب, GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_معلومات);
ساكن gboolean vim_content_provider_write_mime_type_پای(GdkContentProvider *cp, GAsync_پایله *پایله, GError **error);

    ساكن فراغ
vim_content_provider_class_init(VimContentProviderClass *class)
{
    GdkContentProviderClass *cp_class = GDK_CONTENT_PROVIDER_CLASS(class);

    cp_class->ref_formats = vim_content_provider_ref_formats;
    cp_class->write_mime_type_async = vim_content_provider_write_mime_type_async;
    cp_class->write_mime_type_پای = vim_content_provider_write_mime_type_پای;
}

    ساكن فراغ
vim_content_provider_init(VimContentProvider *self ناکارول_شوی)
{
}

    GdkContentProvider *
vim_content_provider_new(Clipboard_T *cbd)
{
    VimContentProvider *vcp = g_object_new(VIM_TYPE_CONTENT_PROVIDER, عدم);

    vcp->cbd = cbd;

    أرجع GDK_CONTENT_PROVIDER(vcp);
}

    ساكن GdkContentFormats *
vim_content_provider_ref_formats(GdkContentProvider *cp ناکارول_شوی)
{
    // We support text formats + our own Vim specific mime types. Also expose
    // html if user specified 'html' in 'clipboard' option.
    GdkContentFormatsBuilder *builder = gdk_content_formats_builder_new();

    لكل (size_t i = 0; i < SUPPORTED_MIMES_LEN; i++)
    {
	إذا (STRCMP(supported_mimes[i], "text/html") == 0 && !clip_html)
	    استمر;
	gdk_content_formats_builder_جمع_کړه_mime_type(builder, supported_mimes[i]);
    }
    أرجع gdk_content_formats_builder_free_منزل_formats(builder);
}

ساكن فراغ
vim_content_provider_write_mime_type_بشپړ (
	GObject		*بهیر,
	GAsync_پایله	*پایله,
	GTask		*task)
{
    GError *error = عدم;

    إذا (!g_output_بهیر_write_all_پای (G_OUTPUT_بهیر (بهیر),
		پایله, عدم, &error))
	g_task_return_error (task, error);
    وإلا
	g_task_return_boolean (task, TRUE);
    g_object_unref (task);
}

    ساكن فراغ
vim_content_provider_write_mime_type_async(
	GdkContentProvider  *cp,
	ثابت حرف	    *mime_type,
	GOutput_بهیر	    *بهیر,
	صحيح		    io_لومړیتوب,
	GCancellable	    *cancellable,
	GAsyncReadyCallback callback,
	فراغ		    *udata)
{
    VimContentProvider	*self = VIM_CONTENT_PROVIDER(cp);
    Clipboard_T		*cbd = self->cbd;
    صحيح			motion_type;
    long_u		اوږدتیا;
    char_u		*string;
    صحيح			دننه_واټن = 0;
    bool		is_vim, is_vimenc;
    GTask		*task;
    gboolean		have_mime = FALSE;

    task = g_task_new (self, cancellable, callback, udata);
    g_task_سېټ_لومړیتوب (task, io_لومړیتوب);
    g_task_سېټ_سرچینه_نښان (task, vim_content_provider_write_mime_type_async);

    إذا (STRCMP(mime_type, "text/html") == 0 && !clip_html)
    {
	g_task_return_new_error(
		task, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
		"HTML not supported");
	g_object_unref(task);
	أرجع;
    }

    // Check if we actually support the mime type
    لكل (صحيح i = 0; i < (صحيح)SUPPORTED_MIMES_LEN; i++)
	إذا (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = TRUE;
	    اكسر;
	}

    إذا (!have_mime)
    {
	g_task_return_new_error(
		task, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
		"Cannot provide contents as '%s'", mime_type);
	g_object_unref(task);
	أرجع;
    }

    // Add the required stuff for our own specific formats.
    is_vimenc = STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    إذا (is_vimenc)
	دننه_واټن += 2 + STRLEN(p_enc);
    وإلا إذا (is_vim)
	دننه_واټن += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_دننه_واټن(&string, &اوږدتیا, دننه_واټن, cbd);

    إذا (motion_type < 0)
    {
	g_task_return_new_error(
		task, G_IO_ERROR, G_IO_ERROR_FAILED, "Error converting data");
	g_object_unref(task);
	أرجع;
    }

    إذا (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    وإلا إذا (is_vim)
	string[0] = (char_u)motion_type;

    // "string" is allocated using vim's allocation functions
    g_task_سېټ_task_معلومات(task, string, vim_free);

    g_output_بهیر_write_all_async(
	    بهیر, string, اوږدتیا, io_لومړیتوب, cancellable,
	    (GAsyncReadyCallback)vim_content_provider_write_mime_type_بشپړ, task);
}

    ساكن gboolean
vim_content_provider_write_mime_type_پای(
	GdkContentProvider  *cp ناکارول_شوی,
	GAsync_پایله	    *پایله,
	GError		    **error)
{
  أرجع g_task_propagate_boolean (G_TASK (پایله), error);
}
