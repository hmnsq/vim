/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <gtk/gtk.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "gui_gtk4_cb.h"

ਬਣਤਰ _VimContentProvider
{
    GdkContentProvider ਮਾਪਾ;

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
ਅਚਲ ਅੱਖਰ *supported_mimes[] = {
    VIMENC_MIMETYPE_NAME,
    VIM_MIMETYPE_NAME,
    "text/html",
    "text/plain;charset=utf-8",
    "text/plain",
    ਨਿਰਾ // gdk_clipboard_read_async expects array to be NULL terminated.
};
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUPPORTED_MIMES_LEN (ARRAY_ਲੰਮਾਈ(supported_mimes) - 1)

G_DEFINE_TYPE(VimContentProvider, vim_content_provider, GDK_TYPE_CONTENT_PROVIDER)

ਸਥਿਰ GdkContentFormats *vim_content_provider_ref_formats(GdkContentProvider *cp);
ਸਥਿਰ ਖਾਲੀ vim_content_provider_write_mime_type_async(GdkContentProvider *cp, ਅਚਲ ਅੱਖਰ *mime_type, GOutput_ਧਾਰਾ *ਧਾਰਾ, ਪੂਰਨ io_ਤਰਜੀਹ, GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_ਅੰਕੜੇ);
ਸਥਿਰ gboolean vim_content_provider_write_mime_type_ਅੰਤ(GdkContentProvider *cp, GAsync_ਨਤੀਜਾ *ਨਤੀਜਾ, GError **error);

    ਸਥਿਰ ਖਾਲੀ
vim_content_provider_class_init(VimContentProviderClass *class)
{
    GdkContentProviderClass *cp_class = GDK_CONTENT_PROVIDER_CLASS(class);

    cp_class->ref_formats = vim_content_provider_ref_formats;
    cp_class->write_mime_type_async = vim_content_provider_write_mime_type_async;
    cp_class->write_mime_type_ਅੰਤ = vim_content_provider_write_mime_type_ਅੰਤ;
}

    ਸਥਿਰ ਖਾਲੀ
vim_content_provider_init(VimContentProvider *self ਅਣਵਰਤਿਆ)
{
}

    GdkContentProvider *
vim_content_provider_new(Clipboard_T *cbd)
{
    VimContentProvider *vcp = g_object_new(VIM_TYPE_CONTENT_PROVIDER, ਨਿਰਾ);

    vcp->cbd = cbd;

    ਵਾਪਸ GDK_CONTENT_PROVIDER(vcp);
}

    ਸਥਿਰ GdkContentFormats *
vim_content_provider_ref_formats(GdkContentProvider *cp ਅਣਵਰਤਿਆ)
{
    // We support text formats + our own Vim specific mime types. Also expose
    // html if user specified 'html' in 'clipboard' option.
    GdkContentFormatsBuilder *builder = gdk_content_formats_builder_new();

    ਲਈ (size_t i = 0; i < SUPPORTED_MIMES_LEN; i++)
    {
	ਜੇ (STRCMP(supported_mimes[i], "text/html") == 0 && !clip_html)
	    ਜਾਰੀ;
	gdk_content_formats_builder_ਜੋੜੋ_mime_type(builder, supported_mimes[i]);
    }
    ਵਾਪਸ gdk_content_formats_builder_free_ਮੰਜ਼ਿਲ_formats(builder);
}

ਸਥਿਰ ਖਾਲੀ
vim_content_provider_write_mime_type_ਪੂਰਾ (
	GObject		*ਧਾਰਾ,
	GAsync_ਨਤੀਜਾ	*ਨਤੀਜਾ,
	GTask		*task)
{
    GError *error = ਨਿਰਾ;

    ਜੇ (!g_output_ਧਾਰਾ_write_all_ਅੰਤ (G_OUTPUT_ਧਾਰਾ (ਧਾਰਾ),
		ਨਤੀਜਾ, ਨਿਰਾ, &error))
	g_task_return_error (task, error);
    ਨਹੀਂਤਾਂ
	g_task_return_boolean (task, TRUE);
    g_object_unref (task);
}

    ਸਥਿਰ ਖਾਲੀ
vim_content_provider_write_mime_type_async(
	GdkContentProvider  *cp,
	ਅਚਲ ਅੱਖਰ	    *mime_type,
	GOutput_ਧਾਰਾ	    *ਧਾਰਾ,
	ਪੂਰਨ		    io_ਤਰਜੀਹ,
	GCancellable	    *cancellable,
	GAsyncReadyCallback callback,
	ਖਾਲੀ		    *udata)
{
    VimContentProvider	*self = VIM_CONTENT_PROVIDER(cp);
    Clipboard_T		*cbd = self->cbd;
    ਪੂਰਨ			motion_type;
    long_u		ਲੰਮਾਈ;
    char_u		*string;
    ਪੂਰਨ			ਵਿੱਥ = 0;
    bool		is_vim, is_vimenc;
    GTask		*task;
    gboolean		have_mime = FALSE;

    task = g_task_new (self, cancellable, callback, udata);
    g_task_ਸੈੱਟ_ਤਰਜੀਹ (task, io_ਤਰਜੀਹ);
    g_task_ਸੈੱਟ_ਸਰੋਤ_ਟੈਗ (task, vim_content_provider_write_mime_type_async);

    ਜੇ (STRCMP(mime_type, "text/html") == 0 && !clip_html)
    {
	g_task_return_new_error(
		task, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
		"HTML not supported");
	g_object_unref(task);
	ਵਾਪਸ;
    }

    // Check if we actually support the mime type
    ਲਈ (ਪੂਰਨ i = 0; i < (ਪੂਰਨ)SUPPORTED_MIMES_LEN; i++)
	ਜੇ (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = TRUE;
	    ਰੋਕੋ;
	}

    ਜੇ (!have_mime)
    {
	g_task_return_new_error(
		task, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
		"Cannot provide contents as '%s'", mime_type);
	g_object_unref(task);
	ਵਾਪਸ;
    }

    // Add the required stuff for our own specific formats.
    is_vimenc = STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    ਜੇ (is_vimenc)
	ਵਿੱਥ += 2 + STRLEN(p_enc);
    ਨਹੀਂਤਾਂ ਜੇ (is_vim)
	ਵਿੱਥ += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_ਵਿੱਥ(&string, &ਲੰਮਾਈ, ਵਿੱਥ, cbd);

    ਜੇ (motion_type < 0)
    {
	g_task_return_new_error(
		task, G_IO_ERROR, G_IO_ERROR_FAILED, "Error converting data");
	g_object_unref(task);
	ਵਾਪਸ;
    }

    ਜੇ (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    ਨਹੀਂਤਾਂ ਜੇ (is_vim)
	string[0] = (char_u)motion_type;

    // "string" is allocated using vim's allocation functions
    g_task_ਸੈੱਟ_task_ਅੰਕੜੇ(task, string, vim_free);

    g_output_ਧਾਰਾ_write_all_async(
	    ਧਾਰਾ, string, ਲੰਮਾਈ, io_ਤਰਜੀਹ, cancellable,
	    (GAsyncReadyCallback)vim_content_provider_write_mime_type_ਪੂਰਾ, task);
}

    ਸਥਿਰ gboolean
vim_content_provider_write_mime_type_ਅੰਤ(
	GdkContentProvider  *cp ਅਣਵਰਤਿਆ,
	GAsync_ਨਤੀਜਾ	    *ਨਤੀਜਾ,
	GError		    **error)
{
  ਵਾਪਸ g_task_propagate_boolean (G_TASK (ਨਤੀਜਾ), error);
}
