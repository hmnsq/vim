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

#ଅନ୍ତର୍ଭାଵ "vim.h"


#ଯଦି defined(FEAT_GUI_PHOTON)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_photon_available;
#endif

ଶୂନ୍ୟ qnx_init(ଶୂନ୍ୟ)
{
#ଯଦି defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_FIELD(parms);
    parms.flags = Ph_DYNAMIC_BUFFER;

    is_photon_available = (PhAttach(ଖାଲି, &parms) != ଖାଲି) ? TRUE : FALSE;
#endif
}

#ଯଦି defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_TEXT "TEXT"

// Turn on the clipboard for a console vim when photon is running
ଶୂନ୍ୟ qnx_clip_init(ଶୂନ୍ୟ)
{
    ଯଦି (is_photon_available == TRUE && !gui.in_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_mch_own_selection(Clipboard_T *cbd)
{
    ଫେରନ୍ତୁ FALSE;
}

ଶୂନ୍ୟ
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

ଶୂନ୍ୟ
clip_mch_request_selection(Clipboard_T *cbd)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    type = MLINE, clip_length = 0, is_type_set = FALSE;
    ଶୂନ୍ୟ	    *cbdata;
    PhClipHeader    *clip_header;
    char_u	    *clip_text = ଖାଲି;

    cbdata = PhClipboardPasteStart(Ph_ନିବେଶ_Group(ଖାଲି));
    ଯଦି (cbdata == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Look for the vim specific clip first
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    ଯଦି (clip_header != ଖାଲି && clip_header->data != ଖାଲି)
    {
	ବାଛ(*(ଅକ୍ଷର *) clip_header->data)
	{
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: // fallthrough to line type
	    ସ୍ଥିତି 'L': type = MLINE; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'C': type = MCHAR; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'B': type = MBLOCK; ଭାଙ୍ଗ;
	}
	is_type_set = TRUE;
    }

    // Try for just normal text
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_TEXT);
    ଯଦି (clip_header != ଖାଲି)
    {
	clip_text = clip_header->data;
	clip_length  = clip_header->length - 1;

	ଯଦି (clip_text != ଖାଲି && is_type_set == FALSE)
	    type = MAUTO;
    }

    ଯଦି ((clip_text != ଖାଲି) && (clip_length > 0))
	clip_yank_selection(type, clip_text, clip_length, cbd);

    PhClipboardPasteFinish(cbdata);
}

ଶୂନ୍ୟ
clip_mch_set_selection(Clipboard_T *cbd)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
    long_u  len;
    char_u *text_clip, vim_clip[2], *str = ଖାଲି;
    PhClipHeader clip_header[2];

    // Prevent recursion from clip_get_selection()
    ଯଦି (cbd->owned == TRUE)
	ଫେରନ୍ତୁ;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    ଯଦି (type >= 0)
    {
	text_clip = alloc(len + 1); // Normal text

	ଯଦି (text_clip && vim_clip)
	{
	    CLEAR_FIELD(clip_header);

	    STRNCPY(clip_header[0].type, CLIP_TYPE_VIM, 8);
	    clip_header[0].length = ଆକାର(vim_clip);
	    clip_header[0].data   = vim_clip;

	    STRNCPY(clip_header[1].type, CLIP_TYPE_TEXT, 8);
	    clip_header[1].length = len + 1;
	    clip_header[1].data   = text_clip;

	    ବାଛ(type)
	    {
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ: // fallthrough to MLINE
		ସ୍ଥିତି MLINE:	*vim_clip = 'L'; ଭାଙ୍ଗ;
		ସ୍ଥିତି MCHAR:	*vim_clip = 'C'; ଭାଙ୍ଗ;
		ସ୍ଥିତି MBLOCK:	*vim_clip = 'B'; ଭାଙ୍ଗ;
	    }

	    vim_strncpy(text_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_ନିବେଶ_Group(ଖାଲି), 2, clip_header);
	}
	vim_free(text_clip);
    }
    vim_free(str);
}
#endif
