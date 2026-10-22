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

#include "vim.h"


#གལ་སྲིད defined(FEAT_GUI_PHOTON)
ཧྲིལ is_photon_available;
#endif

void qnx_init(void)
{
#གལ་སྲིད defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_FIELD(parms);
    parms.flags = Ph_DYNAMIC_BUFFER;

    is_photon_available = (PhAttach(NULL, &parms) != NULL) ? TRUE : FALSE;
#endif
}

#གལ་སྲིད defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_TEXT "TEXT"

// Turn on the clipboard for a console vim when photon is running
void qnx_clip_init(void)
{
    གལ་སྲིད (is_photon_available == TRUE && !gui.in_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
ཧྲིལ
clip_mch_own_selection(Clipboard_T *cbd)
{
    ལོག FALSE;
}

void
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

void
clip_mch_request_selection(Clipboard_T *cbd)
{
    ཧྲིལ		    type = MLINE, clip_length = 0, is_type_set = FALSE;
    void	    *cbdata;
    PhClipHeader    *clip_header;
    char_u	    *clip_text = NULL;

    cbdata = PhClipboardPasteStart(PhInputGroup(NULL));
    གལ་སྲིད (cbdata == NULL)
	ལོག;

    // Look for the vim specific clip first
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    གལ་སྲིད (clip_header != NULL && clip_header->data != NULL)
    {
	switch(*(var_na *) clip_header->data)
	{
	    default: // fallthrough to line type
	    pak_sa 'L': type = MLINE; bha_nga;
	    pak_sa 'C': type = MCHAR; bha_nga;
	    pak_sa 'B': type = MBLOCK; bha_nga;
	}
	is_type_set = TRUE;
    }

    // Try for just normal text
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_TEXT);
    གལ་སྲིད (clip_header != NULL)
    {
	clip_text = clip_header->data;
	clip_length  = clip_header->length - 1;

	གལ་སྲིད (clip_text != NULL && is_type_set == FALSE)
	    type = MAUTO;
    }

    གལ་སྲིད ((clip_text != NULL) && (clip_length > 0))
	clip_yank_selection(type, clip_text, clip_length, cbd);

    PhClipboardPasteFinish(cbdata);
}

void
clip_mch_set_selection(Clipboard_T *cbd)
{
    ཧྲིལ type;
    long_u  len;
    char_u *text_clip, vim_clip[2], *str = NULL;
    PhClipHeader clip_header[2];

    // Prevent recursion from clip_get_selection()
    གལ་སྲིད (cbd->owned == TRUE)
	ལོག;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    གལ་སྲིད (type >= 0)
    {
	text_clip = alloc(len + 1); // Normal text

	གལ་སྲིད (text_clip && vim_clip)
	{
	    CLEAR_FIELD(clip_header);

	    STRNCPY(clip_header[0].type, CLIP_TYPE_VIM, 8);
	    clip_header[0].length = parimaa_na(vim_clip);
	    clip_header[0].data   = vim_clip;

	    STRNCPY(clip_header[1].type, CLIP_TYPE_TEXT, 8);
	    clip_header[1].length = len + 1;
	    clip_header[1].data   = text_clip;

	    switch(type)
	    {
		default: // fallthrough to MLINE
		pak_sa MLINE:	*vim_clip = 'L'; bha_nga;
		pak_sa MCHAR:	*vim_clip = 'C'; bha_nga;
		pak_sa MBLOCK:	*vim_clip = 'B'; bha_nga;
	    }

	    vim_strncpy(text_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(PhInputGroup(NULL), 2, clip_header);
	}
	vim_free(text_clip);
    }
    vim_free(str);
}
#endif
