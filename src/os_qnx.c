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

#অন্তর্ভাৱ "vim.h"


#যদি defined(FEAT_GUI_PHOTON)
পূর্ণাঙ্ক is_photon_available;
#endif

শূন্য qnx_init(শূন্য)
{
#যদি defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_FIELD(parms);
    parms.flags = Ph_DYNAMIC_BUFFER;

    is_photon_available = (PhAttach(রিক্ত, &parms) != রিক্ত) ? TRUE : FALSE;
#endif
}

#যদি defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_TEXT "TEXT"

// Turn on the clipboard for a console vim when photon is running
শূন্য qnx_clip_init(শূন্য)
{
    যদি (is_photon_available == TRUE && !gui.in_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
পূর্ণাঙ্ক
clip_mch_own_selection(Clipboard_T *cbd)
{
    ঘূৰাই FALSE;
}

শূন্য
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

শূন্য
clip_mch_request_selection(Clipboard_T *cbd)
{
    পূর্ণাঙ্ক		    type = MLINE, clip_length = 0, is_type_set = FALSE;
    শূন্য	    *cbdata;
    PhClipHeader    *clip_header;
    char_u	    *clip_text = রিক্ত;

    cbdata = PhClipboardPasteStart(Ph_প্ৰবেশ_Group(রিক্ত));
    যদি (cbdata == রিক্ত)
	ঘূৰাই;

    // Look for the vim specific clip first
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    যদি (clip_header != রিক্ত && clip_header->data != রিক্ত)
    {
	ৱিকল্প(*(ৱর্ণ *) clip_header->data)
	{
	    স্ৱভাৱ: // fallthrough to line type
	    পক্ষ 'L': type = MLINE; ভঙ্গ;
	    পক্ষ 'C': type = MCHAR; ভঙ্গ;
	    পক্ষ 'B': type = MBLOCK; ভঙ্গ;
	}
	is_type_set = TRUE;
    }

    // Try for just normal text
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_TEXT);
    যদি (clip_header != রিক্ত)
    {
	clip_text = clip_header->data;
	clip_length  = clip_header->length - 1;

	যদি (clip_text != রিক্ত && is_type_set == FALSE)
	    type = MAUTO;
    }

    যদি ((clip_text != রিক্ত) && (clip_length > 0))
	clip_yank_selection(type, clip_text, clip_length, cbd);

    PhClipboardPasteFinish(cbdata);
}

শূন্য
clip_mch_set_selection(Clipboard_T *cbd)
{
    পূর্ণাঙ্ক type;
    long_u  len;
    char_u *text_clip, vim_clip[2], *str = রিক্ত;
    PhClipHeader clip_header[2];

    // Prevent recursion from clip_get_selection()
    যদি (cbd->owned == TRUE)
	ঘূৰাই;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    যদি (type >= 0)
    {
	text_clip = alloc(len + 1); // Normal text

	যদি (text_clip && vim_clip)
	{
	    CLEAR_FIELD(clip_header);

	    STRNCPY(clip_header[0].type, CLIP_TYPE_VIM, 8);
	    clip_header[0].length = পরিমাণ(vim_clip);
	    clip_header[0].data   = vim_clip;

	    STRNCPY(clip_header[1].type, CLIP_TYPE_TEXT, 8);
	    clip_header[1].length = len + 1;
	    clip_header[1].data   = text_clip;

	    ৱিকল্প(type)
	    {
		স্ৱভাৱ: // fallthrough to MLINE
		পক্ষ MLINE:	*vim_clip = 'L'; ভঙ্গ;
		পক্ষ MCHAR:	*vim_clip = 'C'; ভঙ্গ;
		পক্ষ MBLOCK:	*vim_clip = 'B'; ভঙ্গ;
	    }

	    vim_strncpy(text_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_প্ৰবেশ_Group(রিক্ত), 2, clip_header);
	}
	vim_free(text_clip);
    }
    vim_free(str);
}
#endif
