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

#شامل "vim.h"


#اگر defined(FEAT_GUI_PHOTON)
صحیح is_photon_دستیاب;
#اگر_ختم

خالی qnx_init(خالی)
{
#اگر defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_میدان(parms);
    parms.flags = Ph_DYNAMIC_بفر;

    is_photon_دستیاب = (PhAttach(تہی, &parms) != تہی) ? TRUE : FALSE;
#اگر_ختم
}

#اگر defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_متن "TEXT"

// Turn on the clipboard for a console vim when photon is running
خالی qnx_clip_init(خالی)
{
    اگر (is_photon_دستیاب == TRUE && !gui.اندر_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
صحیح
clip_mch_own_selection(Clipboard_T *cbd)
{
    واپس FALSE;
}

خالی
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

خالی
clip_mch_request_selection(Clipboard_T *cbd)
{
    صحیح		    type = MLINE, clip_لمبائی = 0, is_type_سیٹ = FALSE;
    خالی	    *cbdata;
    Ph_Clip_سرخی    *clip_سرخی;
    char_u	    *clip_متن = تہی;

    cbdata = Ph_Clipboard_Paste_آغاز(Ph_داخلہ_گروہ(تہی));
    اگر (cbdata == تہی)
	واپس;

    // Look for the vim specific clip first
    clip_سرخی = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    اگر (clip_سرخی != تہی && clip_سرخی->ڈیٹا != تہی)
    {
	انتخاب(*(حرف *) clip_سرخی->ڈیٹا)
	{
	    طےشدہ: // fallthrough to line type
	    حالت 'L': type = MLINE; توڑو;
	    حالت 'C': type = MCHAR; توڑو;
	    حالت 'B': type = MBLOCK; توڑو;
	}
	is_type_سیٹ = TRUE;
    }

    // Try for just normal text
    clip_سرخی = PhClipboardPasteType(cbdata, CLIP_TYPE_متن);
    اگر (clip_سرخی != تہی)
    {
	clip_متن = clip_سرخی->ڈیٹا;
	clip_لمبائی  = clip_سرخی->لمبائی - 1;

	اگر (clip_متن != تہی && is_type_سیٹ == FALSE)
	    type = MAUTO;
    }

    اگر ((clip_متن != تہی) && (clip_لمبائی > 0))
	clip_yank_selection(type, clip_متن, clip_لمبائی, cbd);

    Ph_Clipboard_Paste_اختتام(cbdata);
}

خالی
clip_mch_سیٹ_selection(Clipboard_T *cbd)
{
    صحیح type;
    long_u  len;
    char_u *متن_clip, vim_clip[2], *str = تہی;
    Ph_Clip_سرخی clip_سرخی[2];

    // Prevent recursion from clip_get_selection()
    اگر (cbd->owned == TRUE)
	واپس;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    اگر (type >= 0)
    {
	متن_clip = alloc(len + 1); // Normal text

	اگر (متن_clip && vim_clip)
	{
	    CLEAR_میدان(clip_سرخی);

	    STRNCPY(clip_سرخی[0].type, CLIP_TYPE_VIM, 8);
	    clip_سرخی[0].لمبائی = حجم(vim_clip);
	    clip_سرخی[0].ڈیٹا   = vim_clip;

	    STRNCPY(clip_سرخی[1].type, CLIP_TYPE_متن, 8);
	    clip_سرخی[1].لمبائی = len + 1;
	    clip_سرخی[1].ڈیٹا   = متن_clip;

	    انتخاب(type)
	    {
		طےشدہ: // fallthrough to MLINE
		حالت MLINE:	*vim_clip = 'L'; توڑو;
		حالت MCHAR:	*vim_clip = 'C'; توڑو;
		حالت MBLOCK:	*vim_clip = 'B'; توڑو;
	    }

	    vim_strncpy(متن_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_داخلہ_گروہ(تہی), 2, clip_سرخی);
	}
	vim_free(متن_clip);
    }
    vim_free(str);
}
#اگر_ختم
