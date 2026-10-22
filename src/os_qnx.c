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

#অন্তর্ভাব "vim.h"


#যদি defined(FEAT_GUI_PHOTON)
পূর্ণসংখ্যা is_photon_available;
#endif

শূন্য qnx_init(শূন্য)
{
#যদি defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_FIELD(parms);
    parms.flags = Ph_DYNAMIC_বাফার;

    is_photon_available = (PhAttach(ফাঁকা, &parms) != ফাঁকা) ? TRUE : FALSE;
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
পূর্ণসংখ্যা
clip_mch_own_selection(Clipboard_T *cbd)
{
    ফেরত FALSE;
}

শূন্য
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

শূন্য
clip_mch_request_selection(Clipboard_T *cbd)
{
    পূর্ণসংখ্যা		    type = MLINE, clip_দৈর্ঘ্য = 0, is_type_set = FALSE;
    শূন্য	    *cbdata;
    PhClipHeader    *clip_header;
    char_u	    *clip_text = ফাঁকা;

    cbdata = PhClipboardPasteStart(Ph_প্রবেশ_Group(ফাঁকা));
    যদি (cbdata == ফাঁকা)
	ফেরত;

    // Look for the vim specific clip first
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    যদি (clip_header != ফাঁকা && clip_header->তথ্য != ফাঁকা)
    {
	নির্বাচন(*(অক্ষর *) clip_header->তথ্য)
	{
	    পূর্বনির্ধারিত: // fallthrough to line type
	    ক্ষেত্র 'L': type = MLINE; বিরতি;
	    ক্ষেত্র 'C': type = MCHAR; বিরতি;
	    ক্ষেত্র 'B': type = MBLOCK; বিরতি;
	}
	is_type_set = TRUE;
    }

    // Try for just normal text
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_TEXT);
    যদি (clip_header != ফাঁকা)
    {
	clip_text = clip_header->তথ্য;
	clip_দৈর্ঘ্য  = clip_header->দৈর্ঘ্য - 1;

	যদি (clip_text != ফাঁকা && is_type_set == FALSE)
	    type = MAUTO;
    }

    যদি ((clip_text != ফাঁকা) && (clip_দৈর্ঘ্য > 0))
	clip_yank_selection(type, clip_text, clip_দৈর্ঘ্য, cbd);

    PhClipboardPasteFinish(cbdata);
}

শূন্য
clip_mch_set_selection(Clipboard_T *cbd)
{
    পূর্ণসংখ্যা type;
    long_u  len;
    char_u *text_clip, vim_clip[2], *str = ফাঁকা;
    PhClipHeader clip_header[2];

    // Prevent recursion from clip_get_selection()
    যদি (cbd->owned == TRUE)
	ফেরত;

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
	    clip_header[0].দৈর্ঘ্য = আকার(vim_clip);
	    clip_header[0].তথ্য   = vim_clip;

	    STRNCPY(clip_header[1].type, CLIP_TYPE_TEXT, 8);
	    clip_header[1].দৈর্ঘ্য = len + 1;
	    clip_header[1].তথ্য   = text_clip;

	    নির্বাচন(type)
	    {
		পূর্বনির্ধারিত: // fallthrough to MLINE
		ক্ষেত্র MLINE:	*vim_clip = 'L'; বিরতি;
		ক্ষেত্র MCHAR:	*vim_clip = 'C'; বিরতি;
		ক্ষেত্র MBLOCK:	*vim_clip = 'B'; বিরতি;
	    }

	    vim_strncpy(text_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_প্রবেশ_Group(ফাঁকা), 2, clip_header);
	}
	vim_free(text_clip);
    }
    vim_free(str);
}
#endif
