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

#ضمن "vim.h"


#إذا defined(FEAT_GUI_PHOTON)
صحيح is_photon_شته;
#د_که_پای

فراغ qnx_init(فراغ)
{
#إذا defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_ډګر(parms);
    parms.flags = Ph_DYNAMIC_بفر;

    is_photon_شته = (PhAttach(عدم, &parms) != عدم) ? TRUE : FALSE;
#د_که_پای
}

#إذا defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_متن "TEXT"

// Turn on the clipboard for a console vim when photon is running
فراغ qnx_clip_init(فراغ)
{
    إذا (is_photon_شته == TRUE && !gui.دننه_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
صحيح
clip_mch_own_selection(Clipboard_T *cbd)
{
    أرجع FALSE;
}

فراغ
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

فراغ
clip_mch_request_selection(Clipboard_T *cbd)
{
    صحيح		    type = MLINE, clip_اوږدتیا = 0, is_type_سېټ = FALSE;
    فراغ	    *cbdata;
    Ph_Clip_سرلیک    *clip_سرلیک;
    char_u	    *clip_متن = عدم;

    cbdata = Ph_Clipboard_Paste_پیل(Ph_ننوت_ډله(عدم));
    إذا (cbdata == عدم)
	أرجع;

    // Look for the vim specific clip first
    clip_سرلیک = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    إذا (clip_سرلیک != عدم && clip_سرلیک->معلومات != عدم)
    {
	بدل(*(حرف *) clip_سرلیک->معلومات)
	{
	    افتراضي: // fallthrough to line type
	    حالة 'L': type = MLINE; اكسر;
	    حالة 'C': type = MCHAR; اكسر;
	    حالة 'B': type = MBLOCK; اكسر;
	}
	is_type_سېټ = TRUE;
    }

    // Try for just normal text
    clip_سرلیک = PhClipboardPasteType(cbdata, CLIP_TYPE_متن);
    إذا (clip_سرلیک != عدم)
    {
	clip_متن = clip_سرلیک->معلومات;
	clip_اوږدتیا  = clip_سرلیک->اوږدتیا - 1;

	إذا (clip_متن != عدم && is_type_سېټ == FALSE)
	    type = MAUTO;
    }

    إذا ((clip_متن != عدم) && (clip_اوږدتیا > 0))
	clip_yank_selection(type, clip_متن, clip_اوږدتیا, cbd);

    Ph_Clipboard_Paste_پای(cbdata);
}

فراغ
clip_mch_سېټ_selection(Clipboard_T *cbd)
{
    صحيح type;
    long_u  len;
    char_u *متن_clip, vim_clip[2], *str = عدم;
    Ph_Clip_سرلیک clip_سرلیک[2];

    // Prevent recursion from clip_get_selection()
    إذا (cbd->owned == TRUE)
	أرجع;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    إذا (type >= 0)
    {
	متن_clip = alloc(len + 1); // Normal text

	إذا (متن_clip && vim_clip)
	{
	    CLEAR_ډګر(clip_سرلیک);

	    STRNCPY(clip_سرلیک[0].type, CLIP_TYPE_VIM, 8);
	    clip_سرلیک[0].اوږدتیا = حجم(vim_clip);
	    clip_سرلیک[0].معلومات   = vim_clip;

	    STRNCPY(clip_سرلیک[1].type, CLIP_TYPE_متن, 8);
	    clip_سرلیک[1].اوږدتیا = len + 1;
	    clip_سرلیک[1].معلومات   = متن_clip;

	    بدل(type)
	    {
		افتراضي: // fallthrough to MLINE
		حالة MLINE:	*vim_clip = 'L'; اكسر;
		حالة MCHAR:	*vim_clip = 'C'; اكسر;
		حالة MBLOCK:	*vim_clip = 'B'; اكسر;
	    }

	    vim_strncpy(متن_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_ننوت_ډله(عدم), 2, clip_سرلیک);
	}
	vim_free(متن_clip);
    }
    vim_free(str);
}
#د_که_پای
