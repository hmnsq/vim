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

#અન્તર્ભાવ "vim.h"


#જો defined(FEAT_GUI_PHOTON)
પૂર્ણાંક is_photon_available;
#endif

શૂન્ય qnx_init(શૂન્ય)
{
#જો defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_FIELD(parms);
    parms.flags = Ph_DYNAMIC_BUFFER;

    is_photon_available = (PhAttach(ખાલી, &parms) != ખાલી) ? TRUE : FALSE;
#endif
}

#જો defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_TEXT "TEXT"

// Turn on the clipboard for a console vim when photon is running
શૂન્ય qnx_clip_init(શૂન્ય)
{
    જો (is_photon_available == TRUE && !gui.in_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
પૂર્ણાંક
clip_mch_own_selection(Clipboard_T *cbd)
{
    પાછા FALSE;
}

શૂન્ય
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

શૂન્ય
clip_mch_request_selection(Clipboard_T *cbd)
{
    પૂર્ણાંક		    type = MLINE, clip_length = 0, is_type_set = FALSE;
    શૂન્ય	    *cbdata;
    PhClipHeader    *clip_header;
    char_u	    *clip_text = ખાલી;

    cbdata = PhClipboardPasteStart(Ph_આગત_Group(ખાલી));
    જો (cbdata == ખાલી)
	પાછા;

    // Look for the vim specific clip first
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    જો (clip_header != ખાલી && clip_header->data != ખાલી)
    {
	પસંદગી(*(અક્ષર *) clip_header->data)
	{
	    મૂળભૂત: // fallthrough to line type
	    સ્થિતિ 'L': type = MLINE; તોડો;
	    સ્થિતિ 'C': type = MCHAR; તોડો;
	    સ્થિતિ 'B': type = MBLOCK; તોડો;
	}
	is_type_set = TRUE;
    }

    // Try for just normal text
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_TEXT);
    જો (clip_header != ખાલી)
    {
	clip_text = clip_header->data;
	clip_length  = clip_header->length - 1;

	જો (clip_text != ખાલી && is_type_set == FALSE)
	    type = MAUTO;
    }

    જો ((clip_text != ખાલી) && (clip_length > 0))
	clip_yank_selection(type, clip_text, clip_length, cbd);

    PhClipboardPasteFinish(cbdata);
}

શૂન્ય
clip_mch_set_selection(Clipboard_T *cbd)
{
    પૂર્ણાંક type;
    long_u  len;
    char_u *text_clip, vim_clip[2], *str = ખાલી;
    PhClipHeader clip_header[2];

    // Prevent recursion from clip_get_selection()
    જો (cbd->owned == TRUE)
	પાછા;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    જો (type >= 0)
    {
	text_clip = alloc(len + 1); // Normal text

	જો (text_clip && vim_clip)
	{
	    CLEAR_FIELD(clip_header);

	    STRNCPY(clip_header[0].type, CLIP_TYPE_VIM, 8);
	    clip_header[0].length = કદ(vim_clip);
	    clip_header[0].data   = vim_clip;

	    STRNCPY(clip_header[1].type, CLIP_TYPE_TEXT, 8);
	    clip_header[1].length = len + 1;
	    clip_header[1].data   = text_clip;

	    પસંદગી(type)
	    {
		મૂળભૂત: // fallthrough to MLINE
		સ્થિતિ MLINE:	*vim_clip = 'L'; તોડો;
		સ્થિતિ MCHAR:	*vim_clip = 'C'; તોડો;
		સ્થિતિ MBLOCK:	*vim_clip = 'B'; તોડો;
	    }

	    vim_strncpy(text_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_આગત_Group(ખાલી), 2, clip_header);
	}
	vim_free(text_clip);
    }
    vim_free(str);
}
#endif
