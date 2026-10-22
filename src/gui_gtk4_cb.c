/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#अन्तर्भाव "vim.h"
#अन्तर्भाव <gtk/gtk.h>
#अन्तर्भाव "gui_gtk4_cb.h"

संरचना _VimContentProvider
{
    GdkContentProvider parent;

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
ध्रुव वर्ण *supported_mimes[] = {
    VIMENC_MIMETYPE_NAME,
    VIM_MIMETYPE_NAME,
    "text/html",
    "text/plain;charset=utf-8",
    "text/plain",
    रिक्त // gdk_clipboard_read_async expects array to be NULL terminated.
};
#define SUPPORTED_MIMES_LEN (ARRAY_LENGTH(supported_mimes) - 1)

G_DEFINE_TYPE(VimContentProvider, vim_content_provider, GDK_TYPE_CONTENT_PROVIDER)

स्थिर GdkContentFormats *vim_content_provider_ref_formats(GdkContentProvider *cp);
स्थिर शून्य vim_content_provider_write_mime_type_async(GdkContentProvider *cp, ध्रुव वर्ण *mime_type, GOutputStream *stream, पूर्णाङ्क io_priority, GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data);
स्थिर gboolean vim_content_provider_write_mime_type_finish(GdkContentProvider *cp, GAsyncResult *result, GError **error);

    स्थिर शून्य
vim_content_provider_class_init(VimContentProviderClass *class)
{
    GdkContentProviderClass *cp_class = GDK_CONTENT_PROVIDER_CLASS(class);

    cp_class->ref_formats = vim_content_provider_ref_formats;
    cp_class->write_mime_type_async = vim_content_provider_write_mime_type_async;
    cp_class->write_mime_type_finish = vim_content_provider_write_mime_type_finish;
}

    स्थिर शून्य
vim_content_provider_init(VimContentProvider *self UNUSED)
{
}

    GdkContentProvider *
vim_content_provider_new(Clipboard_T *cbd)
{
    VimContentProvider *vcp = g_object_new(VIM_TYPE_CONTENT_PROVIDER, रिक्त);

    vcp->cbd = cbd;

    प्रत्यागम GDK_CONTENT_PROVIDER(vcp);
}

    स्थिर GdkContentFormats *
vim_content_provider_ref_formats(GdkContentProvider *cp UNUSED)
{
    // We support text formats + our own Vim specific mime types. Also expose
    // html if user specified 'html' in 'clipboard' option.
    GdkContentFormatsBuilder *builder = gdk_content_formats_builder_new();

    प्रति (माप_प्रकार i = 0; i < SUPPORTED_MIMES_LEN; i++)
    {
	यदि (STRCMP(supported_mimes[i], "text/html") == 0 && !clip_html)
	    अनुवर्त;
	gdk_content_formats_builder_add_mime_type(builder, supported_mimes[i]);
    }
    प्रत्यागम gdk_content_formats_builder_free_to_formats(builder);
}

स्थिर शून्य
vim_content_provider_write_mime_type_done (
	GObject		*stream,
	GAsyncResult	*result,
	GTask		*task)
{
    GError *error = रिक्त;

    यदि (!g_output_stream_write_all_finish (G_OUTPUT_STREAM (stream),
		result, रिक्त, &error))
	g_task_return_error (task, error);
    अन्यथा
	g_task_return_boolean (task, TRUE);
    g_object_unref (task);
}

    स्थिर शून्य
vim_content_provider_write_mime_type_async(
	GdkContentProvider  *cp,
	ध्रुव वर्ण	    *mime_type,
	GOutputStream	    *stream,
	पूर्णाङ्क		    io_priority,
	GCancellable	    *cancellable,
	GAsyncReadyCallback callback,
	शून्य		    *udata)
{
    VimContentProvider	*self = VIM_CONTENT_PROVIDER(cp);
    Clipboard_T		*cbd = self->cbd;
    पूर्णाङ्क			motion_type;
    long_u		length;
    char_u		*string;
    पूर्णाङ्क			offset = 0;
    bool		is_vim, is_vimenc;
    GTask		*task;
    gboolean		have_mime = FALSE;

    task = g_task_new (self, cancellable, callback, udata);
    g_task_set_priority (task, io_priority);
    g_task_set_source_tag (task, vim_content_provider_write_mime_type_async);

    यदि (STRCMP(mime_type, "text/html") == 0 && !clip_html)
    {
	g_task_return_new_error(
		task, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
		"HTML not supported");
	g_object_unref(task);
	प्रत्यागम;
    }

    // Check if we actually support the mime type
    प्रति (पूर्णाङ्क i = 0; i < (पूर्णाङ्क)SUPPORTED_MIMES_LEN; i++)
	यदि (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = TRUE;
	    भङ्ग;
	}

    यदि (!have_mime)
    {
	g_task_return_new_error(
		task, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
		"Cannot provide contents as '%s'", mime_type);
	g_object_unref(task);
	प्रत्यागम;
    }

    // Add the required stuff for our own specific formats.
    is_vimenc = STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    यदि (is_vimenc)
	offset += 2 + STRLEN(p_enc);
    अन्यथा यदि (is_vim)
	offset += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_offset(&string, &length, offset, cbd);

    यदि (motion_type < 0)
    {
	g_task_return_new_error(
		task, G_IO_ERROR, G_IO_ERROR_FAILED, "Error converting data");
	g_object_unref(task);
	प्रत्यागम;
    }

    यदि (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    अन्यथा यदि (is_vim)
	string[0] = (char_u)motion_type;

    // "string" is allocated using vim's allocation functions
    g_task_set_task_data(task, string, vim_free);

    g_output_stream_write_all_async(
	    stream, string, length, io_priority, cancellable,
	    (GAsyncReadyCallback)vim_content_provider_write_mime_type_done, task);
}

    स्थिर gboolean
vim_content_provider_write_mime_type_finish(
	GdkContentProvider  *cp UNUSED,
	GAsyncResult	    *result,
	GError		    **error)
{
  प्रत्यागम g_task_propagate_boolean (G_TASK (result), error);
}
