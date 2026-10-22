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
#پایان_اگر

فراغ qnx_init(فراغ)
{
#إذا defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_فیلد(parms);
    parms.flags = Ph_DYNAMIC_میانگیر;

    is_photon_موجود = (PhAttach(عدم, &parms) != عدم) ? TRUE : FALSE;
#پایان_اگر
}

#إذا defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_متن "TEXT"

// Turn on the clipboard for a console vim when photon is running
فراغ qnx_clip_init(فراغ)
{
    إذا (is_photon_موجود == TRUE && !gui.درون_use)
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
    صحيح		    type = MLINE, clip_درازی = 0, is_type_مجموعه = FALSE;
    فراغ	    *cbdata;
    Ph_Clip_سرآیند    *clip_سرآیند;
    char_u	    *clip_متن = عدم;

    cbdata = Ph_Clipboard_Paste_شروع(Ph_درون_داد_گروه(عدم));
    إذا (cbdata == عدم)
	أرجع;

    // Look for the vim specific clip first
    clip_سرآیند = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    إذا (clip_سرآیند != عدم && clip_سرآیند->داده != عدم)
    {
	بدل(*(حرف *) clip_سرآیند->داده)
	{
	    افتراضي: // fallthrough to line type
	    حالة 'L': type = MLINE; اكسر;
	    حالة 'C': type = MCHAR; اكسر;
	    حالة 'B': type = MBLOCK; اكسر;
	}
	is_type_مجموعه = TRUE;
    }

    // Try for just normal text
    clip_سرآیند = PhClipboardPasteType(cbdata, CLIP_TYPE_متن);
    إذا (clip_سرآیند != عدم)
    {
	clip_متن = clip_سرآیند->داده;
	clip_درازی  = clip_سرآیند->درازی - 1;

	إذا (clip_متن != عدم && is_type_مجموعه == FALSE)
	    type = MAUTO;
    }

    إذا ((clip_متن != عدم) && (clip_درازی > 0))
	clip_yank_selection(type, clip_متن, clip_درازی, cbd);

    Ph_Clipboard_Paste_پایان(cbdata);
}

فراغ
clip_mch_مجموعه_selection(Clipboard_T *cbd)
{
    صحيح type;
    long_u  len;
    char_u *متن_clip, vim_clip[2], *str = عدم;
    Ph_Clip_سرآیند clip_سرآیند[2];

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
	    CLEAR_فیلد(clip_سرآیند);

	    STRNCPY(clip_سرآیند[0].type, CLIP_TYPE_VIM, 8);
	    clip_سرآیند[0].درازی = حجم(vim_clip);
	    clip_سرآیند[0].داده   = vim_clip;

	    STRNCPY(clip_سرآیند[1].type, CLIP_TYPE_متن, 8);
	    clip_سرآیند[1].درازی = len + 1;
	    clip_سرآیند[1].داده   = متن_clip;

	    بدل(type)
	    {
		افتراضي: // fallthrough to MLINE
		حالة MLINE:	*vim_clip = 'L'; اكسر;
		حالة MCHAR:	*vim_clip = 'C'; اكسر;
		حالة MBLOCK:	*vim_clip = 'B'; اكسر;
	    }

	    vim_strncpy(متن_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_درون_داد_گروه(عدم), 2, clip_سرآیند);
	}
	vim_free(متن_clip);
    }
    vim_free(str);
}
#پایان_اگر
