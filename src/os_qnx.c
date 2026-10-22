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

#अन्तर्भाव "vim.h"


#यदि defined(FEAT_GUI_PHOTON)
पूर्णाङ्क is_photon_available;
#endif

शून्य qnx_init(शून्य)
{
#यदि defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_FIELD(parms);
    parms.flags = Ph_DYNAMIC_BUFFER;

    is_photon_available = (PhAttach(रिक्त, &parms) != रिक्त) ? TRUE : FALSE;
#endif
}

#यदि defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_TEXT "TEXT"

// Turn on the clipboard for a console vim when photon is running
शून्य qnx_clip_init(शून्य)
{
    यदि (is_photon_available == TRUE && !gui.in_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
पूर्णाङ्क
clip_mch_own_selection(Clipboard_T *cbd)
{
    प्रत्यागम FALSE;
}

शून्य
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

शून्य
clip_mch_request_selection(Clipboard_T *cbd)
{
    पूर्णाङ्क		    type = MLINE, clip_length = 0, is_type_set = FALSE;
    शून्य	    *cbdata;
    PhClipHeader    *clip_header;
    char_u	    *clip_text = रिक्त;

    cbdata = PhClipboardPasteStart(Ph_निवेश_Group(रिक्त));
    यदि (cbdata == रिक्त)
	प्रत्यागम;

    // Look for the vim specific clip first
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    यदि (clip_header != रिक्त && clip_header->data != रिक्त)
    {
	विकल्प(*(वर्ण *) clip_header->data)
	{
	    स्वभाव: // fallthrough to line type
	    पक्ष 'L': type = MLINE; भङ्ग;
	    पक्ष 'C': type = MCHAR; भङ्ग;
	    पक्ष 'B': type = MBLOCK; भङ्ग;
	}
	is_type_set = TRUE;
    }

    // Try for just normal text
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_TEXT);
    यदि (clip_header != रिक्त)
    {
	clip_text = clip_header->data;
	clip_length  = clip_header->length - 1;

	यदि (clip_text != रिक्त && is_type_set == FALSE)
	    type = MAUTO;
    }

    यदि ((clip_text != रिक्त) && (clip_length > 0))
	clip_yank_selection(type, clip_text, clip_length, cbd);

    PhClipboardPasteFinish(cbdata);
}

शून्य
clip_mch_set_selection(Clipboard_T *cbd)
{
    पूर्णाङ्क type;
    long_u  len;
    char_u *text_clip, vim_clip[2], *str = रिक्त;
    PhClipHeader clip_header[2];

    // Prevent recursion from clip_get_selection()
    यदि (cbd->owned == TRUE)
	प्रत्यागम;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    यदि (type >= 0)
    {
	text_clip = alloc(len + 1); // Normal text

	यदि (text_clip && vim_clip)
	{
	    CLEAR_FIELD(clip_header);

	    STRNCPY(clip_header[0].type, CLIP_TYPE_VIM, 8);
	    clip_header[0].length = परिमाण(vim_clip);
	    clip_header[0].data   = vim_clip;

	    STRNCPY(clip_header[1].type, CLIP_TYPE_TEXT, 8);
	    clip_header[1].length = len + 1;
	    clip_header[1].data   = text_clip;

	    विकल्प(type)
	    {
		स्वभाव: // fallthrough to MLINE
		पक्ष MLINE:	*vim_clip = 'L'; भङ्ग;
		पक्ष MCHAR:	*vim_clip = 'C'; भङ्ग;
		पक्ष MBLOCK:	*vim_clip = 'B'; भङ्ग;
	    }

	    vim_strncpy(text_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_निवेश_Group(रिक्त), 2, clip_header);
	}
	vim_free(text_clip);
    }
    vim_free(str);
}
#endif
