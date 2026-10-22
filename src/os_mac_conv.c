/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * os_mac_conv.c: Code specifically for Mac string conversions.
 *
 * This code has been put in a separate file to avoid the conflicts that are
 * caused by including both the X11 and Carbon header files.
 */

#تعریف_کرو NO_X11_INCLUDES

#شامل "vim.h"

#شامل <CoreServices/CoreServices.h>


#اگر defined(MACOS_CONVERT)

ساکن char_u	    *mac_utf16_مقصد_utf8(UniChar *from, size_t fromLen, size_t *actualLen);
ساکن UniChar	    *mac_utf8_مقصد_utf16(char_u *from, size_t fromLen, size_t *actualLen);

// Converter for composing decomposed HFS+ file paths
ساکن TECObjectRef g_راستہ_Converter;
// Converter used by mac_utf16_to_utf8
ساکن TECObjectRef g_UTF16_مقصد_UTF8_Converter;

/*
 * A Mac version of string_convert_ext() for special cases.
 */
    char_u *
mac_string_convert(
    char_u		*ptr,
    صحیح			len,
    صحیح			*lenp,
    صحیح			fail_on_error,
    صحیح			from_enc,
    صحیح			مقصد_enc,
    صحیح			*unconvlenp)
{
    char_u		*retval, *d;
    CFStringRef		cfstr;
    صحیح			buflen, اندر, برآمد, l, i;
    CFStringEncoding	from;
    CFStringEncoding	مقصد;

    انتخاب (from_enc)
    {
	حالت 'l':   from = kCFStringEncodingISOLatin1; توڑو;
	حالت 'm':   from = kCFStringEncodingMacRoman; توڑو;
	حالت 'u':   from = kCFStringEncodingUTF8; توڑو;
	طےشدہ:    واپس تہی;
    }
    انتخاب (مقصد_enc)
    {
	حالت 'l':   مقصد = kCFStringEncodingISOLatin1; توڑو;
	حالت 'm':   مقصد = kCFStringEncodingMacRoman; توڑو;
	حالت 'u':   مقصد = kCFStringEncodingUTF8; توڑو;
	طےشدہ:    واپس تہی;
    }

    اگر (unconvlenp != تہی)
	*unconvlenp = 0;
    cfstr = CFString_Create_With_بائٹیں(تہی, ptr, len, from, 0);

    اگر (cfstr == تہی)
	fprintf(stderr, "Encoding failed\n");
    // When conversion failed, try excluding bytes from the end, helps when
    // there is an incomplete byte sequence.  Only do up to 6 bytes to avoid
    // looping a long time when there really is something unconvertible.
    جب (cfstr == تہی && unconvlenp != تہی && len > 1 && *unconvlenp < 6)
    {
	--len;
	++*unconvlenp;
	cfstr = CFString_Create_With_بائٹیں(تہی, ptr, len, from, 0);
    }
    اگر (cfstr == تہی)
	واپس تہی;

    اگر (مقصد == kCFStringEncodingUTF8)
	buflen = len * 6 + 1;
    ورنہ
	buflen = len + 1;
    retval = alloc(buflen);
    اگر (retval == تہی)
    {
	CFRelease(cfstr);
	واپس تہی;
    }

#اگر 0
    CFRange convertRange = CFRangeMake(0, CFString_Get_لمبائی(cfstr));
    //  Determine output buffer size
    CFString_Get_بائٹیں(cfstr, convertRange, مقصد, تہی, FALSE, تہی, 0, (CFIndex *)&buflen);
    retval = (buflen > 0) ? alloc(buflen) : تہی;
    اگر (retval == تہی)
    {
	CFRelease(cfstr);
	واپس تہی;
    }

    اگر (lenp)
	*lenp = buflen / حجم(char_u);

    اگر (!CFString_Get_بائٹیں(cfstr, convertRange, مقصد, تہی, FALSE, retval, buflen, تہی))
#اگر_ختم
    اگر (!CFStringGetCString(cfstr, (حرف *)retval, buflen, مقصد))
    {
	CFRelease(cfstr);
	اگر (fail_on_error)
	{
	    vim_free(retval);
	    واپس تہی;
	}

	fprintf(stderr, "Trying char-by-char conversion...\n");
	// conversion failed for the whole string, but maybe it will work
	// for each character
	برائے (d = retval, اندر = 0, برآمد = 0; اندر < len && برآمد < buflen - 1;)
	{
	    اگر (from == kCFStringEncodingUTF8)
		l = utf_ptr2len(ptr + اندر);
	    ورنہ
		l = 1;
	    cfstr = CFString_Create_With_بائٹیں(تہی, ptr + اندر, l, from, 0);
	    اگر (cfstr == تہی)
	    {
		*d++ = '?';
		برآمد++;
	    }
	    ورنہ
	    {
		اگر (!CFStringGetCString(cfstr, (حرف *)d, buflen - برآمد, مقصد))
		{
		    *d++ = '?';
		    برآمد++;
		}
		ورنہ
		{
		    i = STRLEN(d);
		    d += i;
		    برآمد += i;
		}
		CFRelease(cfstr);
	    }
	    اندر += l;
	}
	*d = NUL;
	اگر (lenp != تہی)
	    *lenp = برآمد;
	واپس retval;
    }
    CFRelease(cfstr);
    اگر (lenp != تہی)
	*lenp = STRLEN(retval);

    واپس retval;
}

/*
 * Conversion from Apple MacRoman char encoding to UTF-8 or latin1, using
 * standard Carbon framework.
 * Input: "ptr[*sizep]".
 * "real_size" is the size of the buffer that "ptr" points to.
 * output is in-place, "sizep" is adjusted.
 * Returns OK or FAIL.
 */
    صحیح
macroman2enc(
    char_u	*ptr,
    طویل	*sizep,
    طویل	real_سائز)
{
    CFStringRef		cfstr;
    CFRange		r;
    CFIndex		len = *sizep;

    // MacRoman is an 8-bit encoding, no need to move bytes to
    // conv_rest[].
    cfstr = CFString_Create_With_بائٹیں(تہی, ptr, len,
						kCFStringEncodingMacRoman, 0);
    /*
     * If there is a conversion error, try using another
     * conversion.
     */
    اگر (cfstr == تہی)
	واپس FAIL;

    r.location = 0;
    r.لمبائی = CFString_Get_لمبائی(cfstr);
    اگر (r.لمبائی != CFString_Get_بائٹیں(cfstr, r,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0, // no lossy conversion
	    0, // not external representation
	    ptr + *sizep, real_سائز - *sizep, &len))
    {
	CFRelease(cfstr);
	واپس FAIL;
    }
    CFRelease(cfstr);
    mch_memmove(ptr, ptr + *sizep, len);
    *sizep = len;

    واپس OK;
}

/*
 * Conversion from UTF-8 or latin1 to MacRoman.
 * Input: "from[fromlen]"
 * Output: "to[maxtolen]" length in "*tolenp"
 * Unconverted rest in rest[*restlenp].
 * Returns OK or FAIL.
 */
    صحیح
enc2macroman(
    char_u	*from,
    size_t	fromlen,
    char_u	*مقصد,
    صحیح		*tolenp,
    صحیح		maxtolen,
    char_u	*rest,
    صحیح		*restlenp)
{
    CFStringRef	cfstr;
    CFRange	r;
    CFIndex	l;

    *restlenp = 0;
    cfstr = CFString_Create_With_بائٹیں(تہی, from, fromlen,
	    (enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
	    0);
    جب (cfstr == تہی && *restlenp < 3 && fromlen > 1)
    {
	rest[*restlenp++] = from[--fromlen];
	cfstr = CFString_Create_With_بائٹیں(تہی, from, fromlen,
		(enc_utf8) ? kCFStringEncodingUTF8 : kCFStringEncodingISOLatin1,
		0);
    }
    اگر (cfstr == تہی)
	واپس FAIL;

    r.location = 0;
    r.لمبائی = CFString_Get_لمبائی(cfstr);
    اگر (r.لمبائی != CFString_Get_بائٹیں(cfstr, r,
		kCFStringEncodingMacRoman,
		0, // no lossy conversion
		0, // not external representation (since vim
		   // handles this internally)
		مقصد, maxtolen, &l))
    {
	CFRelease(cfstr);
	واپس FAIL;
    }
    CFRelease(cfstr);
    *tolenp = l;
    واپس OK;
}

/*
 * Initializes text converters
 */
    خالی
mac_conv_init(خالی)
{
    متن_Encoding    utf8_encoding;
    متن_Encoding    utf8_hfsplus_encoding;
    متن_Encoding    utf8_canon_encoding;
    متن_Encoding    utf16_encoding;

    utf8_encoding = Create_متن_Encoding(k_متن_Encoding_Unicode_Default,
	    k_متن_Encoding_Default_Variant, kUnicodeUTF8Format);
    utf8_hfsplus_encoding = Create_متن_Encoding(k_متن_Encoding_Unicode_Default,
	    kUnicodeHFSPlusCompVariant, kUnicodeUTF8Format);
    utf8_canon_encoding = Create_متن_Encoding(k_متن_Encoding_Unicode_Default,
	    kUnicodeCanonicalCompVariant, kUnicodeUTF8Format);
    utf16_encoding = Create_متن_Encoding(k_متن_Encoding_Unicode_Default,
	    k_متن_Encoding_Default_Variant, k_Unicode16_ثنائی_ہندسہ_Format);

    اگر (TECCreateConverter(&g_راستہ_Converter, utf8_encoding,
		utf8_hfsplus_encoding) != noErr)
	g_راستہ_Converter = تہی;

    اگر (TECCreateConverter(&g_UTF16_مقصد_UTF8_Converter, utf16_encoding,
		utf8_canon_encoding) != noErr)
    {
	// On pre-10.3, Unicode normalization is not available so
	// fall back to non-normalizing converter
	اگر (TECCreateConverter(&g_UTF16_مقصد_UTF8_Converter, utf16_encoding,
		    utf8_encoding) != noErr)
	    g_UTF16_مقصد_UTF8_Converter = تہی;
    }
}

/*
 * Destroys text converters
 */
    خالی
mac_conv_cleanup(خالی)
{
    اگر (g_UTF16_مقصد_UTF8_Converter)
    {
	TECDisposeConverter(g_UTF16_مقصد_UTF8_Converter);
	g_UTF16_مقصد_UTF8_Converter = تہی;
    }

    اگر (g_راستہ_Converter)
    {
	TECDisposeConverter(g_راستہ_Converter);
	g_راستہ_Converter = تہی;
    }
}

/*
 * Conversion from UTF-16 UniChars to 'encoding'
 * The function signature uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    char_u *
mac_utf16_مقصد_enc(
    غیرعلامتی مختصر *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    char_u      *نتیجہ = تہی;

    // Convert to utf-8 first, works better with iconv
    utf8_len = 0;
    utf8_str = mac_utf16_مقصد_utf8(from, fromLen, &utf8_len);

    اگر (utf8_str)
    {
	// We might be called before we have p_enc set up.
	conv.vc_type = CONV_NONE;

	// If encoding (p_enc) is any unicode, it is actually in utf-8 (vim
	// internal unicode is always utf-8) so don't convert in such cases

	اگر ((enc_canon_props(p_enc) & ENC_UNICODE) == 0)
	    convert_تیاری(&conv, (char_u *)"utf-8",
		    p_enc? p_enc: (char_u *)"macroman");
	اگر (conv.vc_type == CONV_NONE)
	{
	    // p_enc is utf-8, so we're done.
	    نتیجہ = utf8_str;
	}
	ورنہ
	{
	    صحیح len = utf8_len;
	    نتیجہ = string_convert(&conv, utf8_str, &len);
	    utf8_len = len;
	    vim_free(utf8_str);
	}

	convert_تیاری(&conv, تہی, تہی);

	اگر (actualLen)
	    *actualLen = utf8_len;
    }
    ورنہ اگر (actualLen)
	*actualLen = 0;

    واپس نتیجہ;
}

/*
 * Conversion from 'encoding' to UTF-16 UniChars
 * The function return uses the real type of UniChar (as typedef'ed in
 * CFBase.h) to avoid clashes with X11 header files in the .pro file
 */
    غیرعلامتی مختصر *
mac_enc_مقصد_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    // Following code borrows somewhat from os_mswin.c
    vimconv_T	conv;
    size_t      utf8_len;
    char_u      *utf8_str;
    UniChar     *نتیجہ = تہی;
    Boolean     should_free_utf8 = FALSE;

    کرو
    {
	// Use MacRoman by default, we might be called before we have p_enc
	// set up.  Convert to utf-8 first, works better with iconv().  Does
	// nothing if 'encoding' is "utf-8".
	conv.vc_type = CONV_NONE;
	اگر ((enc_canon_props(p_enc) & ENC_UNICODE) == 0 &&
		convert_تیاری(&conv, p_enc ? p_enc : (char_u *)"macroman",
		    (char_u *)"utf-8") == FAIL)
	    توڑو;

	اگر (conv.vc_type != CONV_NONE)
	{
	    utf8_len = fromLen;
	    utf8_str = string_convert(&conv, from, (صحیح *)&utf8_len);
	    should_free_utf8 = TRUE;
	}
	ورنہ
	{
	    utf8_str = from;
	    utf8_len = fromLen;
	}

	اگر (utf8_str == تہی)
	    توڑو;

	convert_تیاری(&conv, تہی, تہی);

	نتیجہ = mac_utf8_مقصد_utf16(utf8_str, utf8_len, actualLen);

	اگر (should_free_utf8)
	    vim_free(utf8_str);
	واپس نتیجہ;
    }
    جب (0);

    اگر (actualLen)
	*actualLen = 0;

    واپس نتیجہ;
}

/*
 * Converts from UTF-16 UniChars to CFString
 * The void * return type is actually a CFStringRef
 */
    خالی *
mac_enc_مقصد_cfstring(
    char_u  *from,
    size_t  fromLen)
{
    UniChar	*utf16_str;
    size_t	utf16_len;
    CFStringRef	نتیجہ = تہی;

    utf16_str = mac_enc_مقصد_utf16(from, fromLen, &utf16_len);
    اگر (utf16_str)
    {
	نتیجہ = CFStringCreateWithCharacters(تہی, utf16_str, utf16_len/حجم(UniChar));
	vim_free(utf16_str);
    }

    واپس (خالی *)نتیجہ;
}

/*
 * Converts a decomposed HFS+ UTF-8 path to precomposed UTF-8
 */
    char_u *
mac_precompose_راستہ(
    char_u  *decomp_راستہ,
    size_t  decompLen,
    size_t  *precompLen)
{
    char_u  *نتیجہ = تہی;
    size_t  actualLen = 0;

    اگر (g_راستہ_Converter)
    {
	نتیجہ = alloc(decompLen);
	اگر (نتیجہ)
	{
	    اگر (TECConvert_متن(g_راستہ_Converter, decomp_راستہ,
			decompLen, &decompLen, نتیجہ,
			decompLen, &actualLen) != noErr)
		VIM_CLEAR(نتیجہ);
	}
    }

    اگر (precompLen)
	*precompLen = actualLen;

    واپس نتیجہ;
}

/*
 * Converts from UTF-16 UniChars to precomposed UTF-8
 */
    ساکن char_u *
mac_utf16_مقصد_utf8(
    UniChar *from,
    size_t fromLen,
    size_t *actualLen)
{
    Byte_گنتی		utf8_len;
    Byte_گنتی		داخلہ_Read;
    char_u		*نتیجہ;

    اگر (g_UTF16_مقصد_UTF8_Converter)
    {
	نتیجہ = alloc(fromLen * 6 + 1);
	اگر (نتیجہ && TECConvert_متن(g_UTF16_مقصد_UTF8_Converter, (Const_متن_Ptr)from,
		    fromLen, &داخلہ_Read, نتیجہ,
		    (fromLen*6+1)*حجم(char_u), &utf8_len) == noErr)
	{
	    TECFlush_متن(g_UTF16_مقصد_UTF8_Converter, نتیجہ, (fromLen*6+1)*حجم(char_u), &داخلہ_Read);
	    utf8_len += داخلہ_Read;
	}
	ورنہ
	    VIM_CLEAR(نتیجہ);
    }
    ورنہ
    {
	نتیجہ = تہی;
    }

    اگر (actualLen)
	*actualLen = نتیجہ ? utf8_len : 0;

    واپس نتیجہ;
}

/*
 * Converts from UTF-8 to UTF-16 UniChars
 */
    ساکن UniChar *
mac_utf8_مقصد_utf16(
    char_u *from,
    size_t fromLen,
    size_t *actualLen)
{
    CFStringRef  utf8_str;
    CFRange      convertRange;
    UniChar      *نتیجہ = تہی;

    utf8_str = CFString_Create_With_بائٹیں(تہی, from, fromLen,
	    kCFStringEncodingUTF8, FALSE);

    اگر (utf8_str == تہی)
    {
	اگر (actualLen)
	    *actualLen = 0;
	واپس تہی;
    }

    convertRange = CFRangeMake(0, CFString_Get_لمبائی(utf8_str));
    نتیجہ = ALLOC_MULT(UniChar, convertRange.لمبائی);

    CFStringGetCharacters(utf8_str, convertRange, نتیجہ);

    CFRelease(utf8_str);

    اگر (actualLen)
	*actualLen = convertRange.لمبائی * حجم(UniChar);

    واپس نتیجہ;
}

/*
 * Sets LANG environment variable in Vim from Mac locale
 */
    خالی
mac_lang_init(خالی)
{
    اگر (mch_getenv((char_u *)"LANG") != تہی)
	واپس;

    حرف	buf[50];

    // $LANG is not set, either because it was unset or Vim was started
    // from the Dock.  Query the system locale.
    اگر (LocaleRefGetPartString(تہی,
		k_Locale_Language_نقاب | k_Locale_Language_Variant_نقاب |
		k_Locale_Region_نقاب | k_Locale_Region_Variant_نقاب,
		حجم(buf) - 10, buf) == noErr && *buf)
    {
	اگر (strcasestr(buf, "utf-8") == تہی)
	    strcat(buf, ".UTF-8");
	vim_setenv((char_u *)"LANG", (char_u *)buf);
#اگر_تعریف_شدہ HAVE_LOCALE_H
	setlocale(LC_ALL, "");
#اگر_ختم
#اگر defined(LC_NUMERIC)
	// Make sure strtod() uses a decimal point, not a comma.
	setlocale(LC_NUMERIC, "C");
#اگر_ختم
    }
}
#اگر_ختم // MACOS_CONVERT
