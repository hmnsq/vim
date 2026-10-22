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
صحيح is_photon_موجود;
#جيڪڏهن_پورو

فراغ qnx_init(فراغ)
{
#إذا defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_ميدان(parms);
    parms.flags = Ph_DYNAMIC_بفر;

    is_photon_موجود = (PhAttach(عدم, &parms) != عدم) ? TRUE : FALSE;
#جيڪڏهن_پورو
}

#إذا defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_متن "TEXT"

// Turn on the clipboard for a console vim when photon is running
فراغ qnx_clip_init(فراغ)
{
    إذا (is_photon_موجود == TRUE && !gui.اندريون_use)
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
    صحيح		    type = MLINE, clip_ڊيگهائي = 0, is_type_سيٽ = FALSE;
    فراغ	    *cbdata;
    Ph_Clip_سرو    *clip_سرو;
    char_u	    *clip_متن = عدم;

    cbdata = Ph_Clipboard_Paste_شروعات(Ph_داخلا_گروهه(عدم));
    إذا (cbdata == عدم)
	أرجع;

    // Look for the vim specific clip first
    clip_سرو = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    إذا (clip_سرو != عدم && clip_سرو->معلومات != عدم)
    {
	بدل(*(حرف *) clip_سرو->معلومات)
	{
	    افتراضي: // fallthrough to line type
	    حالة 'L': type = MLINE; اكسر;
	    حالة 'C': type = MCHAR; اكسر;
	    حالة 'B': type = MBLOCK; اكسر;
	}
	is_type_سيٽ = TRUE;
    }

    // Try for just normal text
    clip_سرو = PhClipboardPasteType(cbdata, CLIP_TYPE_متن);
    إذا (clip_سرو != عدم)
    {
	clip_متن = clip_سرو->معلومات;
	clip_ڊيگهائي  = clip_سرو->ڊيگهائي - 1;

	إذا (clip_متن != عدم && is_type_سيٽ == FALSE)
	    type = MAUTO;
    }

    إذا ((clip_متن != عدم) && (clip_ڊيگهائي > 0))
	clip_yank_selection(type, clip_متن, clip_ڊيگهائي, cbd);

    Ph_Clipboard_Paste_پڄاڻي(cbdata);
}

فراغ
clip_mch_سيٽ_selection(Clipboard_T *cbd)
{
    صحيح type;
    long_u  len;
    char_u *متن_clip, vim_clip[2], *str = عدم;
    Ph_Clip_سرو clip_سرو[2];

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
	    CLEAR_ميدان(clip_سرو);

	    STRNCPY(clip_سرو[0].type, CLIP_TYPE_VIM, 8);
	    clip_سرو[0].ڊيگهائي = حجم(vim_clip);
	    clip_سرو[0].معلومات   = vim_clip;

	    STRNCPY(clip_سرو[1].type, CLIP_TYPE_متن, 8);
	    clip_سرو[1].ڊيگهائي = len + 1;
	    clip_سرو[1].معلومات   = متن_clip;

	    بدل(type)
	    {
		افتراضي: // fallthrough to MLINE
		حالة MLINE:	*vim_clip = 'L'; اكسر;
		حالة MCHAR:	*vim_clip = 'C'; اكسر;
		حالة MBLOCK:	*vim_clip = 'B'; اكسر;
	    }

	    vim_strncpy(متن_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_داخلا_گروهه(عدم), 2, clip_سرو);
	}
	vim_free(متن_clip);
    }
    vim_free(str);
}
#جيڪڏهن_پورو
