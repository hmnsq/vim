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

#شامل "vim.h"


#اگر defined(FEAT_GUI_PHOTON)
صحیح is_photon_available;
#endif

تهی qnx_init(تهی)
{
#اگر defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_FIELD(parms);
    parms.flags = Ph_DYNAMIC_BUFFER;

    is_photon_available = (PhAttach(پوچ, &parms) != پوچ) ? TRUE : FALSE;
#endif
}

#اگر defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_TEXT "TEXT"

// Turn on the clipboard for a console vim when photon is running
تهی qnx_clip_init(تهی)
{
    اگر (is_photon_available == TRUE && !gui.in_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
صحیح
clip_mch_own_selection(Clipboard_T *cbd)
{
    بازگشت FALSE;
}

تهی
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

تهی
clip_mch_request_selection(Clipboard_T *cbd)
{
    صحیح		    type = MLINE, clip_length = 0, is_type_set = FALSE;
    تهی	    *cbdata;
    PhClipHeader    *clip_header;
    char_u	    *clip_text = پوچ;

    cbdata = PhClipboardPasteStart(Ph_درون_داد_Group(پوچ));
    اگر (cbdata == پوچ)
	بازگشت;

    // Look for the vim specific clip first
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    اگر (clip_header != پوچ && clip_header->data != پوچ)
    {
	انتخاب(*(نویسه *) clip_header->data)
	{
	    پیشفرض: // fallthrough to line type
	    حالت 'L': type = MLINE; بشکن;
	    حالت 'C': type = MCHAR; بشکن;
	    حالت 'B': type = MBLOCK; بشکن;
	}
	is_type_set = TRUE;
    }

    // Try for just normal text
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_TEXT);
    اگر (clip_header != پوچ)
    {
	clip_text = clip_header->data;
	clip_length  = clip_header->length - 1;

	اگر (clip_text != پوچ && is_type_set == FALSE)
	    type = MAUTO;
    }

    اگر ((clip_text != پوچ) && (clip_length > 0))
	clip_yank_selection(type, clip_text, clip_length, cbd);

    PhClipboardPasteFinish(cbdata);
}

تهی
clip_mch_set_selection(Clipboard_T *cbd)
{
    صحیح type;
    long_u  len;
    char_u *text_clip, vim_clip[2], *str = پوچ;
    PhClipHeader clip_header[2];

    // Prevent recursion from clip_get_selection()
    اگر (cbd->owned == TRUE)
	بازگشت;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    اگر (type >= 0)
    {
	text_clip = alloc(len + 1); // Normal text

	اگر (text_clip && vim_clip)
	{
	    CLEAR_FIELD(clip_header);

	    STRNCPY(clip_header[0].type, CLIP_TYPE_VIM, 8);
	    clip_header[0].length = اندازه(vim_clip);
	    clip_header[0].data   = vim_clip;

	    STRNCPY(clip_header[1].type, CLIP_TYPE_TEXT, 8);
	    clip_header[1].length = len + 1;
	    clip_header[1].data   = text_clip;

	    انتخاب(type)
	    {
		پیشفرض: // fallthrough to MLINE
		حالت MLINE:	*vim_clip = 'L'; بشکن;
		حالت MCHAR:	*vim_clip = 'C'; بشکن;
		حالت MBLOCK:	*vim_clip = 'B'; بشکن;
	    }

	    vim_strncpy(text_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_درون_داد_Group(پوچ), 2, clip_header);
	}
	vim_free(text_clip);
    }
    vim_free(str);
}
#endif
