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

#അന്തര്ഭാവ "vim.h"


#എങ്കിൽ defined(FEAT_GUI_PHOTON)
പൂർണ്ണസംഖ്യ is_photon_available;
#endif

ശൂന്യം qnx_init(ശൂന്യം)
{
#എങ്കിൽ defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_FIELD(parms);
    parms.flags = Ph_DYNAMIC_BUFFER;

    is_photon_available = (PhAttach(ശൂന്യ, &parms) != ശൂന്യ) ? TRUE : FALSE;
#endif
}

#എങ്കിൽ defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_TEXT "TEXT"

// Turn on the clipboard for a console vim when photon is running
ശൂന്യം qnx_clip_init(ശൂന്യം)
{
    എങ്കിൽ (is_photon_available == TRUE && !gui.in_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
പൂർണ്ണസംഖ്യ
clip_mch_own_selection(Clipboard_T *cbd)
{
    തിരികെ FALSE;
}

ശൂന്യം
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

ശൂന്യം
clip_mch_request_selection(Clipboard_T *cbd)
{
    പൂർണ്ണസംഖ്യ		    type = MLINE, clip_length = 0, is_type_set = FALSE;
    ശൂന്യം	    *cbdata;
    PhClipHeader    *clip_header;
    char_u	    *clip_text = ശൂന്യ;

    cbdata = PhClipboardPasteStart(Ph_നിവേശം_Group(ശൂന്യ));
    എങ്കിൽ (cbdata == ശൂന്യ)
	തിരികെ;

    // Look for the vim specific clip first
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    എങ്കിൽ (clip_header != ശൂന്യ && clip_header->data != ശൂന്യ)
    {
	തിരഞ്ഞെടുപ്പ്(*(അക്ഷരം *) clip_header->data)
	{
	    സ്ഥിരസ്ഥിതി: // fallthrough to line type
	    അവസ്ഥ 'L': type = MLINE; നിർത്തുക;
	    അവസ്ഥ 'C': type = MCHAR; നിർത്തുക;
	    അവസ്ഥ 'B': type = MBLOCK; നിർത്തുക;
	}
	is_type_set = TRUE;
    }

    // Try for just normal text
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_TEXT);
    എങ്കിൽ (clip_header != ശൂന്യ)
    {
	clip_text = clip_header->data;
	clip_length  = clip_header->length - 1;

	എങ്കിൽ (clip_text != ശൂന്യ && is_type_set == FALSE)
	    type = MAUTO;
    }

    എങ്കിൽ ((clip_text != ശൂന്യ) && (clip_length > 0))
	clip_yank_selection(type, clip_text, clip_length, cbd);

    PhClipboardPasteFinish(cbdata);
}

ശൂന്യം
clip_mch_set_selection(Clipboard_T *cbd)
{
    പൂർണ്ണസംഖ്യ type;
    long_u  len;
    char_u *text_clip, vim_clip[2], *str = ശൂന്യ;
    PhClipHeader clip_header[2];

    // Prevent recursion from clip_get_selection()
    എങ്കിൽ (cbd->owned == TRUE)
	തിരികെ;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    എങ്കിൽ (type >= 0)
    {
	text_clip = alloc(len + 1); // Normal text

	എങ്കിൽ (text_clip && vim_clip)
	{
	    CLEAR_FIELD(clip_header);

	    STRNCPY(clip_header[0].type, CLIP_TYPE_VIM, 8);
	    clip_header[0].length = വലുപ്പം(vim_clip);
	    clip_header[0].data   = vim_clip;

	    STRNCPY(clip_header[1].type, CLIP_TYPE_TEXT, 8);
	    clip_header[1].length = len + 1;
	    clip_header[1].data   = text_clip;

	    തിരഞ്ഞെടുപ്പ്(type)
	    {
		സ്ഥിരസ്ഥിതി: // fallthrough to MLINE
		അവസ്ഥ MLINE:	*vim_clip = 'L'; നിർത്തുക;
		അവസ്ഥ MCHAR:	*vim_clip = 'C'; നിർത്തുക;
		അവസ്ഥ MBLOCK:	*vim_clip = 'B'; നിർത്തുക;
	    }

	    vim_strncpy(text_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_നിവേശം_Group(ശൂന്യ), 2, clip_header);
	}
	vim_free(text_clip);
    }
    vim_free(str);
}
#endif
