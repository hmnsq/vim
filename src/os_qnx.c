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

#అన్తర్భావ "vim.h"


#ఒకవేళ defined(FEAT_GUI_PHOTON)
పూర్ణాంకం is_photon_అందుబాటు;
#అయితే_ముగింపు

శూన్య qnx_init(శూన్య)
{
#ఒకవేళ defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_క్షేత్రం(parms);
    parms.flags = Ph_DYNAMIC_బఫరు;

    is_photon_అందుబాటు = (PhAttach(ఖాళీ, &parms) != ఖాళీ) ? TRUE : FALSE;
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_పాఠ్యం "TEXT"

// Turn on the clipboard for a console vim when photon is running
శూన్య qnx_clip_init(శూన్య)
{
    ఒకవేళ (is_photon_అందుబాటు == TRUE && !gui.లోపల_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
పూర్ణాంకం
clip_mch_own_selection(Clipboard_T *cbd)
{
    తిరిగి FALSE;
}

శూన్య
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

శూన్య
clip_mch_request_selection(Clipboard_T *cbd)
{
    పూర్ణాంకం		    type = MLINE, clip_నిడివి = 0, is_type_సమితి = FALSE;
    శూన్య	    *cbdata;
    Ph_Clip_శీర్షిక    *clip_శీర్షిక;
    char_u	    *clip_పాఠ్యం = ఖాళీ;

    cbdata = Ph_Clipboard_Paste_ప్రారంభం(Ph_ఆదానం_సమూహం(ఖాళీ));
    ఒకవేళ (cbdata == ఖాళీ)
	తిరిగి;

    // Look for the vim specific clip first
    clip_శీర్షిక = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    ఒకవేళ (clip_శీర్షిక != ఖాళీ && clip_శీర్షిక->దత్తాంశం != ఖాళీ)
    {
	ఎంపిక(*(అక్షరం *) clip_శీర్షిక->దత్తాంశం)
	{
	    అప్రమేయ: // fallthrough to line type
	    స్థితి 'L': type = MLINE; ఆపు;
	    స్థితి 'C': type = MCHAR; ఆపు;
	    స్థితి 'B': type = MBLOCK; ఆపు;
	}
	is_type_సమితి = TRUE;
    }

    // Try for just normal text
    clip_శీర్షిక = PhClipboardPasteType(cbdata, CLIP_TYPE_పాఠ్యం);
    ఒకవేళ (clip_శీర్షిక != ఖాళీ)
    {
	clip_పాఠ్యం = clip_శీర్షిక->దత్తాంశం;
	clip_నిడివి  = clip_శీర్షిక->నిడివి - 1;

	ఒకవేళ (clip_పాఠ్యం != ఖాళీ && is_type_సమితి == FALSE)
	    type = MAUTO;
    }

    ఒకవేళ ((clip_పాఠ్యం != ఖాళీ) && (clip_నిడివి > 0))
	clip_yank_selection(type, clip_పాఠ్యం, clip_నిడివి, cbd);

    Ph_Clipboard_Paste_ముగింపు(cbdata);
}

శూన్య
clip_mch_సమితి_selection(Clipboard_T *cbd)
{
    పూర్ణాంకం type;
    long_u  len;
    char_u *పాఠ్యం_clip, vim_clip[2], *str = ఖాళీ;
    Ph_Clip_శీర్షిక clip_శీర్షిక[2];

    // Prevent recursion from clip_get_selection()
    ఒకవేళ (cbd->owned == TRUE)
	తిరిగి;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    ఒకవేళ (type >= 0)
    {
	పాఠ్యం_clip = alloc(len + 1); // Normal text

	ఒకవేళ (పాఠ్యం_clip && vim_clip)
	{
	    CLEAR_క్షేత్రం(clip_శీర్షిక);

	    STRNCPY(clip_శీర్షిక[0].type, CLIP_TYPE_VIM, 8);
	    clip_శీర్షిక[0].నిడివి = పరిమాణం(vim_clip);
	    clip_శీర్షిక[0].దత్తాంశం   = vim_clip;

	    STRNCPY(clip_శీర్షిక[1].type, CLIP_TYPE_పాఠ్యం, 8);
	    clip_శీర్షిక[1].నిడివి = len + 1;
	    clip_శీర్షిక[1].దత్తాంశం   = పాఠ్యం_clip;

	    ఎంపిక(type)
	    {
		అప్రమేయ: // fallthrough to MLINE
		స్థితి MLINE:	*vim_clip = 'L'; ఆపు;
		స్థితి MCHAR:	*vim_clip = 'C'; ఆపు;
		స్థితి MBLOCK:	*vim_clip = 'B'; ఆపు;
	    }

	    vim_strncpy(పాఠ్యం_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_ఆదానం_సమూహం(ఖాళీ), 2, clip_శీర్షిక);
	}
	vim_free(పాఠ్యం_clip);
    }
    vim_free(str);
}
#అయితే_ముగింపు
