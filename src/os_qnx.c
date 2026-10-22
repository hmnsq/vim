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


#जर defined(FEAT_GUI_PHOTON)
पूर्णांक is_photon_available;
#endif

शून्य qnx_init(शून्य)
{
#जर defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_FIELD(parms);
    parms.flags = Ph_DYNAMIC_BUFFER;

    is_photon_available = (PhAttach(रिक्त, &parms) != रिक्त) ? TRUE : FALSE;
#endif
}

#जर defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_TEXT "TEXT"

// Turn on the clipboard for a console vim when photon is running
शून्य qnx_clip_init(शून्य)
{
    जर (is_photon_available == TRUE && !gui.in_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
पूर्णांक
clip_mch_own_selection(Clipboard_T *cbd)
{
    परत FALSE;
}

शून्य
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

शून्य
clip_mch_request_selection(Clipboard_T *cbd)
{
    पूर्णांक		    type = MLINE, clip_length = 0, is_type_set = FALSE;
    शून्य	    *cbdata;
    PhClipHeader    *clip_header;
    char_u	    *clip_text = रिक्त;

    cbdata = PhClipboardPasteStart(Ph_आदान_Group(रिक्त));
    जर (cbdata == रिक्त)
	परत;

    // Look for the vim specific clip first
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    जर (clip_header != रिक्त && clip_header->data != रिक्त)
    {
	निवड(*(अक्षर *) clip_header->data)
	{
	    पूर्वनिर्धारित: // fallthrough to line type
	    स्थिती 'L': type = MLINE; थांबा;
	    स्थिती 'C': type = MCHAR; थांबा;
	    स्थिती 'B': type = MBLOCK; थांबा;
	}
	is_type_set = TRUE;
    }

    // Try for just normal text
    clip_header = PhClipboardPasteType(cbdata, CLIP_TYPE_TEXT);
    जर (clip_header != रिक्त)
    {
	clip_text = clip_header->data;
	clip_length  = clip_header->length - 1;

	जर (clip_text != रिक्त && is_type_set == FALSE)
	    type = MAUTO;
    }

    जर ((clip_text != रिक्त) && (clip_length > 0))
	clip_yank_selection(type, clip_text, clip_length, cbd);

    PhClipboardPasteFinish(cbdata);
}

शून्य
clip_mch_set_selection(Clipboard_T *cbd)
{
    पूर्णांक type;
    long_u  len;
    char_u *text_clip, vim_clip[2], *str = रिक्त;
    PhClipHeader clip_header[2];

    // Prevent recursion from clip_get_selection()
    जर (cbd->owned == TRUE)
	परत;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    जर (type >= 0)
    {
	text_clip = alloc(len + 1); // Normal text

	जर (text_clip && vim_clip)
	{
	    CLEAR_FIELD(clip_header);

	    STRNCPY(clip_header[0].type, CLIP_TYPE_VIM, 8);
	    clip_header[0].length = आकार(vim_clip);
	    clip_header[0].data   = vim_clip;

	    STRNCPY(clip_header[1].type, CLIP_TYPE_TEXT, 8);
	    clip_header[1].length = len + 1;
	    clip_header[1].data   = text_clip;

	    निवड(type)
	    {
		पूर्वनिर्धारित: // fallthrough to MLINE
		स्थिती MLINE:	*vim_clip = 'L'; थांबा;
		स्थिती MCHAR:	*vim_clip = 'C'; थांबा;
		स्थिती MBLOCK:	*vim_clip = 'B'; थांबा;
	    }

	    vim_strncpy(text_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_आदान_Group(रिक्त), 2, clip_header);
	}
	vim_free(text_clip);
    }
    vim_free(str);
}
#endif
