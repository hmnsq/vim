/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * QNX port by Julian Kinraid
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * os_qnx.c
 */

#ضمن "vim.h"


#إذا defined(FEAT_GUI_PHOTON)
صحيح is_photon_available;
#endif

فراغ qnx_init(فراغ)
{
#إذا defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_FIELD(parms);
    parms.flags = Ph_DYNAMIC_BUFFER;

    is_photon_available = (PhAttach(عدم, &parms) != عدم) ? TRUE : FALSE;
#endif
}

#إذا defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_TEXT "TEXT"

// Turn on the clipboard for a console vim when photon is running
فراغ qnx_clip_init(فراغ)
{
    إذا (is_photon_available == TRUE && !gui.in_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
صحيح
clip_mch_own_selection(Clipboard_T *cbd)
{
    أرجع FALSE;
}

فراغ
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

فراغ
clip_mch_request_selection(Clipboard_T *cbd)
{
    صحيح		    type = MLINE, clip_length = 0, is_type_set = FALSE;
    فراغ	    *cbdata;
    PhClipHeader    *clip_header;
    char_u	    *clip_text = عدم;

    cbdata = PhClipboardPasteStart(Ph_كىرگۈزۈش_Group(عدم));
    إذا (cbdata == عدم)
	أرجع;

    // Look for the vim specific clip first
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    إذا (clip_header != عدم && clip_header->data != عدم)
    {
	بدل(*(حرف *) clip_header->data)
	{
	    افتراضي: // fallthrough to line type
	    حالة 'L': type = MLINE; اكسر;
	    حالة 'C': type = MCHAR; اكسر;
	    حالة 'B': type = MBLOCK; اكسر;
	}
	is_type_set = TRUE;
    }

    // Try for just normal text
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_TEXT);
    إذا (clip_header != عدم)
    {
	clip_text = clip_header->data;
	clip_length  = clip_header->length - 1;

	إذا (clip_text != عدم && is_type_set == FALSE)
	    type = MAUTO;
    }

    إذا ((clip_text != عدم) && (clip_length > 0))
	clip_yank_selection(type, clip_text, clip_length, cbd);

    PhClipboardPasteFinish(cbdata);
}

فراغ
clip_mch_set_selection(Clipboard_T *cbd)
{
    صحيح type;
    long_u  len;
    char_u *text_clip, vim_clip[2], *str = عدم;
    PhClipHeader clip_header[2];

    // Prevent recursion from clip_get_selection()
    إذا (cbd->owned == TRUE)
	أرجع;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    إذا (type >= 0)
    {
	text_clip = alloc(len + 1); // Normal text

	إذا (text_clip && vim_clip)
	{
	    CLEAR_FIELD(clip_header);

	    STRNCPY(clip_header[0].type, CLIP_TYPE_VIM, 8);
	    clip_header[0].length = حجم(vim_clip);
	    clip_header[0].data   = vim_clip;

	    STRNCPY(clip_header[1].type, CLIP_TYPE_TEXT, 8);
	    clip_header[1].length = len + 1;
	    clip_header[1].data   = text_clip;

	    بدل(type)
	    {
		افتراضي: // fallthrough to MLINE
		حالة MLINE:	*vim_clip = 'L'; اكسر;
		حالة MCHAR:	*vim_clip = 'C'; اكسر;
		حالة MBLOCK:	*vim_clip = 'B'; اكسر;
	    }

	    vim_strncpy(text_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_كىرگۈزۈش_Group(عدم), 2, clip_header);
	}
	vim_free(text_clip);
    }
    vim_free(str);
}
#endif
