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

#அந்தர்பாவ "vim.h"


#எனில் defined(FEAT_GUI_PHOTON)
முழு is_photon_available;
#endif

வெறுமை qnx_init(வெறுமை)
{
#எனில் defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_FIELD(parms);
    parms.flags = Ph_DYNAMIC_BUFFER;

    is_photon_available = (PhAttach(வெற்று, &parms) != வெற்று) ? TRUE : FALSE;
#endif
}

#எனில் defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_TEXT "TEXT"

// Turn on the clipboard for a console vim when photon is running
வெறுமை qnx_clip_init(வெறுமை)
{
    எனில் (is_photon_available == TRUE && !gui.in_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
முழு
clip_mch_own_selection(Clipboard_T *cbd)
{
    திரும்பு FALSE;
}

வெறுமை
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

வெறுமை
clip_mch_request_selection(Clipboard_T *cbd)
{
    முழு		    type = MLINE, clip_length = 0, is_type_set = FALSE;
    வெறுமை	    *cbdata;
    PhClipHeader    *clip_header;
    char_u	    *clip_text = வெற்று;

    cbdata = PhClipboardPasteStart(Ph_உள்ளீடு_Group(வெற்று));
    எனில் (cbdata == வெற்று)
	திரும்பு;

    // Look for the vim specific clip first
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    எனில் (clip_header != வெற்று && clip_header->data != வெற்று)
    {
	தேர்வு(*(எழுத்து *) clip_header->data)
	{
	    இயல்பு: // fallthrough to line type
	    நிலை 'L': type = MLINE; நிறுத்து;
	    நிலை 'C': type = MCHAR; நிறுத்து;
	    நிலை 'B': type = MBLOCK; நிறுத்து;
	}
	is_type_set = TRUE;
    }

    // Try for just normal text
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_TEXT);
    எனில் (clip_header != வெற்று)
    {
	clip_text = clip_header->data;
	clip_length  = clip_header->length - 1;

	எனில் (clip_text != வெற்று && is_type_set == FALSE)
	    type = MAUTO;
    }

    எனில் ((clip_text != வெற்று) && (clip_length > 0))
	clip_yank_selection(type, clip_text, clip_length, cbd);

    PhClipboardPasteFinish(cbdata);
}

வெறுமை
clip_mch_set_selection(Clipboard_T *cbd)
{
    முழு type;
    long_u  len;
    char_u *text_clip, vim_clip[2], *str = வெற்று;
    PhClipHeader clip_header[2];

    // Prevent recursion from clip_get_selection()
    எனில் (cbd->owned == TRUE)
	திரும்பு;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    எனில் (type >= 0)
    {
	text_clip = alloc(len + 1); // Normal text

	எனில் (text_clip && vim_clip)
	{
	    CLEAR_FIELD(clip_header);

	    STRNCPY(clip_header[0].type, CLIP_TYPE_VIM, 8);
	    clip_header[0].length = அளவு(vim_clip);
	    clip_header[0].data   = vim_clip;

	    STRNCPY(clip_header[1].type, CLIP_TYPE_TEXT, 8);
	    clip_header[1].length = len + 1;
	    clip_header[1].data   = text_clip;

	    தேர்வு(type)
	    {
		இயல்பு: // fallthrough to MLINE
		நிலை MLINE:	*vim_clip = 'L'; நிறுத்து;
		நிலை MCHAR:	*vim_clip = 'C'; நிறுத்து;
		நிலை MBLOCK:	*vim_clip = 'B'; நிறுத்து;
	    }

	    vim_strncpy(text_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_உள்ளீடு_Group(வெற்று), 2, clip_header);
	}
	vim_free(text_clip);
    }
    vim_free(str);
}
#endif
