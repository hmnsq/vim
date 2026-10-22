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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"


#ਜੇ defined(FEAT_GUI_PHOTON)
ਪੂਰਨ is_photon_ਉਪਲਬਧ;
#ਜੇ_ਅੰਤ

ਖਾਲੀ qnx_init(ਖਾਲੀ)
{
#ਜੇ defined(FEAT_GUI_PHOTON)
    PhChannelParms_t parms;

    CLEAR_ਖੇਤਰ(parms);
    parms.flags = Ph_DYNAMIC_ਬਫਰ;

    is_photon_ਉਪਲਬਧ = (PhAttach(ਨਿਰਾ, &parms) != ਨਿਰਾ) ? TRUE : FALSE;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_GUI_PHOTON) && defined(FEAT_CLIPBOARD)

# define CLIP_TYPE_VIM "VIMTYPE"
# define CLIP_TYPE_ਪਾਠ "TEXT"

// Turn on the clipboard for a console vim when photon is running
ਖਾਲੀ qnx_clip_init(ਖਾਲੀ)
{
    ਜੇ (is_photon_ਉਪਲਬਧ == TRUE && !gui.ਵਿੱਚ_use)
	clip_init(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Clipboard

// No support for owning the clipboard
ਪੂਰਨ
clip_mch_own_selection(Clipboard_T *cbd)
{
    ਵਾਪਸ FALSE;
}

ਖਾਲੀ
clip_mch_lose_selection(Clipboard_T *cbd)
{
}

ਖਾਲੀ
clip_mch_request_selection(Clipboard_T *cbd)
{
    ਪੂਰਨ		    type = MLINE, clip_ਲੰਮਾਈ = 0, is_type_ਸੈੱਟ = FALSE;
    ਖਾਲੀ	    *cbdata;
    Ph_Clip_ਸਿਰਲੇਖ    *clip_ਸਿਰਲੇਖ;
    char_u	    *clip_ਪਾਠ = ਨਿਰਾ;

    cbdata = Ph_Clipboard_Paste_ਸ਼ੁਰੂ(Ph_ਨਿਵੇਸ਼_ਸਮੂਹ(ਨਿਰਾ));
    ਜੇ (cbdata == ਨਿਰਾ)
	ਵਾਪਸ;

    // Look for the vim specific clip first
    clip_ਸਿਰਲੇਖ = PhClipboardPasteType(cbdata, CLIP_TYPE_VIM);
    ਜੇ (clip_ਸਿਰਲੇਖ != ਨਿਰਾ && clip_ਸਿਰਲੇਖ->ਅੰਕੜੇ != ਨਿਰਾ)
    {
	ਚੋਣ(*(ਅੱਖਰ *) clip_ਸਿਰਲੇਖ->ਅੰਕੜੇ)
	{
	    ਮੂਲ: // fallthrough to line type
	    ਹਾਲਤ 'L': type = MLINE; ਰੋਕੋ;
	    ਹਾਲਤ 'C': type = MCHAR; ਰੋਕੋ;
	    ਹਾਲਤ 'B': type = MBLOCK; ਰੋਕੋ;
	}
	is_type_ਸੈੱਟ = TRUE;
    }

    // Try for just normal text
    clip_ਸਿਰਲੇਖ = PhClipboardPasteType(cbdata, CLIP_TYPE_ਪਾਠ);
    ਜੇ (clip_ਸਿਰਲੇਖ != ਨਿਰਾ)
    {
	clip_ਪਾਠ = clip_ਸਿਰਲੇਖ->ਅੰਕੜੇ;
	clip_ਲੰਮਾਈ  = clip_ਸਿਰਲੇਖ->ਲੰਮਾਈ - 1;

	ਜੇ (clip_ਪਾਠ != ਨਿਰਾ && is_type_ਸੈੱਟ == FALSE)
	    type = MAUTO;
    }

    ਜੇ ((clip_ਪਾਠ != ਨਿਰਾ) && (clip_ਲੰਮਾਈ > 0))
	clip_yank_selection(type, clip_ਪਾਠ, clip_ਲੰਮਾਈ, cbd);

    Ph_Clipboard_Paste_ਅੰਤ(cbdata);
}

ਖਾਲੀ
clip_mch_ਸੈੱਟ_selection(Clipboard_T *cbd)
{
    ਪੂਰਨ type;
    long_u  len;
    char_u *ਪਾਠ_clip, vim_clip[2], *str = ਨਿਰਾ;
    Ph_Clip_ਸਿਰਲੇਖ clip_ਸਿਰਲੇਖ[2];

    // Prevent recursion from clip_get_selection()
    ਜੇ (cbd->owned == TRUE)
	ਵਾਪਸ;

    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &len, cbd);
    ਜੇ (type >= 0)
    {
	ਪਾਠ_clip = alloc(len + 1); // Normal text

	ਜੇ (ਪਾਠ_clip && vim_clip)
	{
	    CLEAR_ਖੇਤਰ(clip_ਸਿਰਲੇਖ);

	    STRNCPY(clip_ਸਿਰਲੇਖ[0].type, CLIP_TYPE_VIM, 8);
	    clip_ਸਿਰਲੇਖ[0].ਲੰਮਾਈ = ਆਕਾਰ(vim_clip);
	    clip_ਸਿਰਲੇਖ[0].ਅੰਕੜੇ   = vim_clip;

	    STRNCPY(clip_ਸਿਰਲੇਖ[1].type, CLIP_TYPE_ਪਾਠ, 8);
	    clip_ਸਿਰਲੇਖ[1].ਲੰਮਾਈ = len + 1;
	    clip_ਸਿਰਲੇਖ[1].ਅੰਕੜੇ   = ਪਾਠ_clip;

	    ਚੋਣ(type)
	    {
		ਮੂਲ: // fallthrough to MLINE
		ਹਾਲਤ MLINE:	*vim_clip = 'L'; ਰੋਕੋ;
		ਹਾਲਤ MCHAR:	*vim_clip = 'C'; ਰੋਕੋ;
		ਹਾਲਤ MBLOCK:	*vim_clip = 'B'; ਰੋਕੋ;
	    }

	    vim_strncpy(ਪਾਠ_clip, str, len);

	    vim_clip[ 1 ] = NUL;

	    PhClipboardCopy(Ph_ਨਿਵੇਸ਼_ਸਮੂਹ(ਨਿਰਾ), 2, clip_ਸਿਰਲੇਖ);
	}
	vim_free(ਪਾਠ_clip);
    }
    vim_free(str);
}
#ਜੇ_ਅੰਤ
