/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * bufwrite.c: functions for writing a buffer
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
# include <utime.h>		// for struct utimbuf
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು SMALLBUFSIZE	256	// size of emergency write buffer

/*
 * Structure to pass arguments from buf_write() to buf_write_bytes().
 */
ರಚನೆ bw_info
{
    ಪೂರ್ಣಾಂಕ		bw_fd;		// file descriptor
    char_u	*bw_buf;	// buffer with data to be written
    ಪೂರ್ಣಾಂಕ		bw_len;		// length of data
    ಪೂರ್ಣಾಂಕ		bw_flags;	// FIO_ flags
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    buf_T	*bw_ಬಫರ್;	// buffer being written
    ಪೂರ್ಣಾಂಕ		bw_ಮುಕ್ತಾಯ;	// finish encrypting
#ಆದರೆ_ಅಂತ್ಯ
    char_u	bw_rest[CONV_RESTLEN]; // not converted bytes
    ಪೂರ್ಣಾಂಕ		bw_restlen;	// nr of bytes in bw_rest[]
    ಪೂರ್ಣಾಂಕ		bw_ಮೊದಲ;	// first write call
    char_u	*bw_conv_buf;	// buffer for writing converted chars
    size_t	bw_conv_buflen; // size of bw_conv_buf
    ಪೂರ್ಣಾಂಕ		bw_conv_error;	// set for conversion error
    linenr_T	bw_conv_error_lnum;  // first line with error or zero
    linenr_T	bw_ಆರಂಭ_lnum;	// line number at start of buffer
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
    iconv_t	bw_iconv_fd;	// descriptor for iconv() or -1
#ಆದರೆ_ಅಂತ್ಯ
};

/*
 * Convert a Unicode character to bytes.
 * Return TRUE for an error, FALSE when it's OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ucs2bytes(
    ಚಿಹ್ನೆರಹಿತ	c,		// in: character
    char_u	**pp,		// in/out: pointer to result
    ಪೂರ್ಣಾಂಕ		flags)		// FIO_ flags
{
    char_u	*p = *pp;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    ಪೂರ್ಣಾಂಕ		cc;


    ಆದರೆ (flags & FIO_UCS4)
    {
	ಆದರೆ (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	    *p++ = (c >> 16);
	    *p++ = (c >> 24);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    *p++ = (c >> 24);
	    *p++ = (c >> 16);
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & (FIO_UCS2 | FIO_UTF16))
    {
	ಆದರೆ (c >= 0x10000)
	{
	    ಆದರೆ (flags & FIO_UTF16)
	    {
		// Make two words, ten bits of the character in each.  First
		// word is 0xd800 - 0xdbff, second one 0xdc00 - 0xdfff
		c -= 0x10000;
		ಆದರೆ (c >= 0x100000)
		    error = TRUE;
		cc = ((c >> 10) & 0x3ff) + 0xd800;
		ಆದರೆ (flags & FIO_ENDIAN_L)
		{
		    *p++ = cc;
		    *p++ = ((ಚಿಹ್ನೆರಹಿತ)cc >> 8);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    *p++ = ((ಚಿಹ್ನೆರಹಿತ)cc >> 8);
		    *p++ = cc;
		}
		c = (c & 0x3ff) + 0xdc00;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		error = TRUE;
	}
	ಆದರೆ (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ    // Latin1
    {
	ಆದರೆ (c >= 0x100)
	{
	    error = TRUE;
	    *p++ = 0xBF;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *p++ = c;
    }

    *pp = p;
    ಹಿಂತಿರುಗಿ error;
}

/*
 * Call write() to write a number of bytes to the file.
 * Handles encryption and 'encoding' conversion.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
buf_write_ಬೈಟುಗಳು(ರಚನೆ bw_info *ip)
{
    ಪೂರ್ಣಾಂಕ		wlen;
    char_u	*buf = ip->bw_buf;	// data to write
    ಪೂರ್ಣಾಂಕ		len = ip->bw_len;	// length of data
    ಪೂರ್ಣಾಂಕ		flags = ip->bw_flags;	// extra flags

    // Skip conversion when writing the crypt magic number or the BOM.
    ಆದರೆ (!(flags & FIO_NOCONVERT))
    {
	char_u		*p;
	ಚಿಹ್ನೆರಹಿತ	c;
	ಪೂರ್ಣಾಂಕ		n;

	ಆದರೆ (flags & FIO_UTF8)
	{
	    // Convert latin1 in the buffer to UTF-8 in the file.
	    p = ip->bw_conv_buf;	// translate to buffer
	    ಗಾಗಿ (wlen = 0; wlen < len; ++wlen)
		p += utf_char2bytes(buf[wlen], p);
	    buf = ip->bw_conv_buf;
	    len = (ಪೂರ್ಣಾಂಕ)(p - ip->bw_conv_buf);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & (FIO_UCS4 | FIO_UTF16 | FIO_UCS2 | FIO_LATIN1))
	{
	    // Convert UTF-8 bytes in the buffer to UCS-2, UCS-4, UTF-16 or
	    // Latin1 chars in the file.
	    ಆದರೆ (flags & FIO_LATIN1)
		p = buf;	// translate in-place (can only get shorter)
	    ಇಲ್ಲದಿದ್ದರೆ
		p = ip->bw_conv_buf;	// translate to buffer
	    ಗಾಗಿ (wlen = 0; wlen < len; wlen += n)
	    {
		ಆದರೆ (wlen == 0 && ip->bw_restlen != 0)
		{
		    ಪೂರ್ಣಾಂಕ		l;

		    // Use remainder of previous call.  Append the start of
		    // buf[] to get a full sequence.  Might still be too
		    // short!
		    l = CONV_RESTLEN - ip->bw_restlen;
		    ಆದರೆ (l > len)
			l = len;
		    mch_memmove(ip->bw_rest + ip->bw_restlen, buf, (size_t)l);
		    n = utf_ptr2len_len(ip->bw_rest, ip->bw_restlen + l);
		    ಆದರೆ (n > ip->bw_restlen + len)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			ಆದರೆ (ip->bw_restlen + len > CONV_RESTLEN)
			    ಹಿಂತಿರುಗಿ FAIL;
			ip->bw_restlen += len;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (n > 1)
			c = utf_ptr2char(ip->bw_rest);
		    ಇಲ್ಲದಿದ್ದರೆ
			c = ip->bw_rest[0];
		    ಆದರೆ (n >= ip->bw_restlen)
		    {
			n -= ip->bw_restlen;
			ip->bw_restlen = 0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ip->bw_restlen -= n;
			mch_memmove(ip->bw_rest, ip->bw_rest + n,
						      (size_t)ip->bw_restlen);
			n = 0;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    n = utf_ptr2len_len(buf + wlen, len - wlen);
		    ಆದರೆ (n > len - wlen)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			ಆದರೆ (len - wlen > CONV_RESTLEN)
			    ಹಿಂತಿರುಗಿ FAIL;
			ip->bw_restlen = len - wlen;
			mch_memmove(ip->bw_rest, buf + wlen,
						      (size_t)ip->bw_restlen);
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (n > 1)
			c = utf_ptr2char(buf + wlen);
		    ಇಲ್ಲದಿದ್ದರೆ
			c = buf[wlen];
		}
		// Check that there is enough space
		ಆದರೆ (!(flags & FIO_LATIN1))
		{
		    size_t need = (flags & FIO_UCS4) ? 4 : 2;
		    ಆದರೆ ((flags & FIO_UTF16) && c >= 0x10000)
			need = 4;

		    ಆದರೆ ((size_t)(p - ip->bw_conv_buf) + need > ip->bw_conv_buflen)
			ಹಿಂತಿರುಗಿ FAIL;
		}

		ಆದರೆ (ucs2bytes(c, &p, flags) && !ip->bw_conv_error)
		{
		    ip->bw_conv_error = TRUE;
		    ip->bw_conv_error_lnum = ip->bw_ಆರಂಭ_lnum;
		}
		ಆದರೆ (c == NL)
		    ++ip->bw_ಆರಂಭ_lnum;
	    }
	    ಆದರೆ (flags & FIO_LATIN1)
		len = (ಪೂರ್ಣಾಂಕ)(p - buf);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		buf = ip->bw_conv_buf;
		len = (ಪೂರ್ಣಾಂಕ)(p - ip->bw_conv_buf);
	    }
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & FIO_CODEPAGE)
	{
	    // Convert UTF-8 or codepage to UCS-2 and then to MS-Windows
	    // codepage.
	    char_u	*from;
	    size_t	fromlen;
	    char_u	*ಗಮ್ಯ;
	    ಪೂರ್ಣಾಂಕ		u8c;
	    BOOL	bad = FALSE;
	    ಪೂರ್ಣಾಂಕ		needed;

	    ಆದರೆ (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		from = buf;
		fromlen = len;
	    }

	    ಗಮ್ಯ = ip->bw_conv_buf;
	    ಆದರೆ (enc_utf8)
	    {
		// Convert from UTF-8 to UCS-2, to the start of the buffer.
		// The buffer has been allocated to be big enough.
		ಆಗ (fromlen > 0)
		{
		    n = (ಪೂರ್ಣಾಂಕ)utf_ptr2len_len(from, (ಪೂರ್ಣಾಂಕ)fromlen);
		    ಆದರೆ (n > (ಪೂರ್ಣಾಂಕ)fromlen)	// incomplete byte sequence
			ನಿಲ್ಲಿಸು;
		    u8c = utf_ptr2char(from);
		    *ಗಮ್ಯ++ = (u8c & 0xff);
		    *ಗಮ್ಯ++ = (u8c >> 8);
		    fromlen -= n;
		    from += n;
		}

		// Copy remainder to ip->bw_rest[] to be used for the next
		// call.
		ಆದರೆ (fromlen > CONV_RESTLEN)
		{
		    // weird overlong sequence
		    ip->bw_conv_error = TRUE;
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		mch_memmove(ip->bw_rest, from, fromlen);
		ip->bw_restlen = (ಪೂರ್ಣಾಂಕ)fromlen;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Convert from enc_codepage to UCS-2, to the start of the
		// buffer.  The buffer has been allocated to be big enough.
		ip->bw_restlen = 0;
		needed = Multi_Byte_ಗಮ್ಯ_Wide_Char(enc_codepage,
			     MB_ERR_INVALID_CHARS, (LPCSTR)from, (ಪೂರ್ಣಾಂಕ)fromlen,
								     ಖಾಲಿ, 0);
		ಆದರೆ (needed == 0)
		{
		    // When conversion fails there may be a trailing byte.
		    needed = Multi_Byte_ಗಮ್ಯ_Wide_Char(enc_codepage,
			 MB_ERR_INVALID_CHARS, (LPCSTR)from, (ಪೂರ್ಣಾಂಕ)fromlen - 1,
								     ಖಾಲಿ, 0);
		    ಆದರೆ (needed == 0)
		    {
			// Conversion doesn't work.
			ip->bw_conv_error = TRUE;
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		    // Save the trailing byte for the next call.
		    ip->bw_rest[0] = from[fromlen - 1];
		    ip->bw_restlen = 1;
		}
		needed = Multi_Byte_ಗಮ್ಯ_Wide_Char(enc_codepage, MB_ERR_INVALID_CHARS,
				(LPCSTR)from, (ಪೂರ್ಣಾಂಕ)(fromlen - ip->bw_restlen),
							  (LPWSTR)ಗಮ್ಯ, needed);
		ಆದರೆ (needed == 0)
		{
		    // Safety check: Conversion doesn't work.
		    ip->bw_conv_error = TRUE;
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಗಮ್ಯ += needed * 2;
	    }

	    fromlen = ಗಮ್ಯ - ip->bw_conv_buf;
	    buf = ಗಮ್ಯ;
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	    ಆದರೆ (FIO_GET_CP(flags) == CP_UTF8)
	    {
		// Convert from UCS-2 to UTF-8, using the remainder of the
		// conversion buffer.  Fails when out of space.
		ಗಾಗಿ (from = ip->bw_conv_buf; fromlen > 1; fromlen -= 2)
		{
		    u8c = *from++;
		    u8c += (*from++ << 8);
		    ಗಮ್ಯ += utf_char2bytes(u8c, ಗಮ್ಯ);
		    ಆದರೆ (ಗಮ್ಯ + 6 >= ip->bw_conv_buf + ip->bw_conv_buflen)
		    {
			ip->bw_conv_error = TRUE;
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		}
		len = (ಪೂರ್ಣಾಂಕ)(ಗಮ್ಯ - buf);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
	    {
		// Convert from UCS-2 to the codepage, using the remainder of
		// the conversion buffer.  If the conversion uses the default
		// character "0", the data doesn't fit in this encoding, so
		// fail.
		len = Wide_Char_ಗಮ್ಯ_Multi_Byte(FIO_GET_CP(flags), 0,
			(LPCWSTR)ip->bw_conv_buf, (ಪೂರ್ಣಾಂಕ)fromlen / ಗಾತ್ರ(WCHAR),
			(LPSTR)ಗಮ್ಯ, (ಪೂರ್ಣಾಂಕ)(ip->bw_conv_buflen - fromlen), 0,
									&bad);
		ಆದರೆ (bad)
		{
		    ip->bw_conv_error = TRUE;
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_CONVERT
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & FIO_MACROMAN)
	{
	    // Convert UTF-8 or latin1 to Apple MacRoman.
	    char_u	*from;
	    size_t	fromlen;

	    ಆದರೆ (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		from = buf;
		fromlen = len;
	    }

	    ಆದರೆ (enc2macroman(from, fromlen,
			ip->bw_conv_buf, &len, ip->bw_conv_buflen,
			ip->bw_rest, &ip->bw_restlen) == FAIL)
	    {
		ip->bw_conv_error = TRUE;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    buf = ip->bw_conv_buf;
	}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
	ಆದರೆ (ip->bw_iconv_fd != (iconv_t)-1)
	{
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*from;
	    size_t	fromlen;
	    ಅಕ್ಷರ	*ಗಮ್ಯ;
	    size_t	tolen;

	    // Convert with iconv().
	    ಆದರೆ (ip->bw_restlen > 0)
	    {
		ಅಕ್ಷರ *fp;

		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		fp = (ಅಕ್ಷರ *)ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(fp, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(fp + ip->bw_restlen, buf, (size_t)len);
		from = fp;
		tolen = ip->bw_conv_buflen - fromlen;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		from = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf;
		fromlen = len;
		tolen = ip->bw_conv_buflen;
	    }
	    ಗಮ್ಯ = (ಅಕ್ಷರ *)ip->bw_conv_buf;

	    ಆದರೆ (ip->bw_ಮೊದಲ)
	    {
		size_t	save_len = tolen;

		// output the initial shift state sequence
		(ಶೂನ್ಯ)iconv(ip->bw_iconv_fd, ಖಾಲಿ, ಖಾಲಿ, &ಗಮ್ಯ, &tolen);

		// There is a bug in iconv() on Linux (which appears to be
		// wide-spread) which sets "to" to NULL and messes up "tolen".
		ಆದರೆ (ಗಮ್ಯ == ಖಾಲಿ)
		{
		    ಗಮ್ಯ = (ಅಕ್ಷರ *)ip->bw_conv_buf;
		    tolen = save_len;
		}
		ip->bw_ಮೊದಲ = FALSE;
	    }

	    // If iconv() has an error or there is not enough room, fail.
	    ಆದರೆ ((iconv(ip->bw_iconv_fd, (ಶೂನ್ಯ *)&from, &fromlen, &ಗಮ್ಯ, &tolen)
			== (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						    || fromlen > CONV_RESTLEN)
	    {
		ip->bw_conv_error = TRUE;
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    // copy remainder to ip->bw_rest[] to be used for the next call.
	    ಆದರೆ (fromlen > 0)
		mch_memmove(ip->bw_rest, (ಶೂನ್ಯ *)from, fromlen);
	    ip->bw_restlen = (ಪೂರ್ಣಾಂಕ)fromlen;

	    buf = ip->bw_conv_buf;
	    len = (ಪೂರ್ಣಾಂಕ)((char_u *)ಗಮ್ಯ - ip->bw_conv_buf);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (ip->bw_fd < 0)
	// Only checking conversion, which is OK if we get here.
	ಹಿಂತಿರುಗಿ OK;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    ಆದರೆ (flags & FIO_ENCRYPTED)
    {
	// Encrypt the data. Do it in-place if possible, otherwise use an
	// allocated buffer.
# ifdef CRYPT_NOT_INPLACE
	ಆದರೆ (crypt_ಕೆಲಸಮಾಡುತ್ತದೆ_inplace(ip->bw_ಬಫರ್->b_cryptstate))
	{
# endif
	    crypt_encode_inplace(ip->bw_ಬಫರ್->b_cryptstate, buf, len,
								ip->bw_ಮುಕ್ತಾಯ);
# ifdef CRYPT_NOT_INPLACE
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u *outbuf;

	    len = crypt_encode_alloc(curbuf->b_cryptstate, buf, len, &outbuf,
								ip->bw_ಮುಕ್ತಾಯ);
	    ಆದರೆ (len == 0)
		ಹಿಂತಿರುಗಿ OK;  // Crypt layer is buffering, will flush later.
	    wlen = write_eintr(ip->bw_fd, outbuf, len);
	    vim_free(outbuf);
	    ಹಿಂತಿರುಗಿ (wlen < len) ? FAIL : OK;
	}
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ

    wlen = write_eintr(ip->bw_fd, buf, len);
    ಹಿಂತಿರುಗಿ (wlen < len) ? FAIL : OK;
}

/*
 * Check modification time of file, before writing to it.
 * The size isn't checked, because using a tool like "gzip" takes care of
 * using the same timestamp but can't set the size.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_mtime(buf_T *buf, stat_T *st)
{
    ಆದರೆ (buf->b_mtime_read != 0
		  && ಸಮಯ_differs(st, buf->b_mtime_read, buf->b_mtime_read_ns))
    {
	ಸಂದೇಶ_scroll = TRUE;	    // don't overwrite messages here
	ಸಂದೇಶ_silent = 0;		    // must give this prompt
	// don't use emsg() here, don't want to flush the buffers
	ಸಂದೇಶ_attr(_("WARNING: The file has been changed since reading it!!!"),
						       HL_ATTR(HLF_E));
	ಆದರೆ (ask_yesno((char_u *)_("Do you really want to write to it"),
								 TRUE) == 'n')
	    ಹಿಂತಿರುಗಿ FAIL;
	ಸಂದೇಶ_scroll = FALSE;	    // always overwrite the file message now
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate a BOM in "buf[4]" for encoding "name".
 * Return the length of the BOM (zero when no BOM).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
make_bom(char_u *buf, char_u *name)
{
    ಪೂರ್ಣಾಂಕ		flags;
    char_u	*p;

    flags = get_fio_flags(name);

    // Can't put a BOM in a non-Unicode file.
    ಆದರೆ (flags == FIO_LATIN1 || flags == 0)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (flags == FIO_UTF8)	// UTF-8
    {
	buf[0] = 0xef;
	buf[1] = 0xbb;
	buf[2] = 0xbf;
	ಹಿಂತಿರುಗಿ 3;
    }
    p = buf;
    (ಶೂನ್ಯ)ucs2bytes(0xfeff, &p, flags);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(p - buf);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_file_ಸಮಯ(
    char_u  *fname,
    time_t  atime,	    // access time
    time_t  mtime)	    // modification time
{
# ಆದರೆ defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
    ರಚನೆ utimbuf  buf;

    buf.actime	= atime;
    buf.modtime	= mtime;
    (ಶೂನ್ಯ)utime((ಅಕ್ಷರ *)fname, &buf);
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ defined(HAVE_UTIMES)
    ರಚನೆ timeval  tvp[2];

    tvp[0].tv_sec   = atime;
    tvp[0].tv_usec  = 0;
    tvp[1].tv_sec   = mtime;
    tvp[1].tv_usec  = 0;
    (ಶೂನ್ಯ)utimes((ಅಕ್ಷರ *)fname, (ಸ್ಥಿರಾಂಕ ರಚನೆ timeval *)&tvp);
#  endif
# endif
}
#ಆದರೆ_ಅಂತ್ಯ // UNIX

    ಅಕ್ಷರ *
new_file_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ shortmess(SHM_NEW) ? _("[New]") : _("[New File]");
}

/*
 * buf_write() - write to file "fname" lines "start" through "end"
 *
 * We do our own buffering here because fwrite() is so slow.
 *
 * If "forceit" is true, we don't care for errors when attempting backups.
 * In case of an error everything possible is done to restore the original
 * file.  But when "forceit" is TRUE, we risk losing it.
 *
 * When "reset_changed" is TRUE and "append" == FALSE and "start" == 1 and
 * "end" == curbuf->b_ml.ml_line_count, reset curbuf->b_changed.
 *
 * This function must NOT use NameBuff (because it's called by autowrite()).
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಂಕ
buf_write(
    buf_T	    *buf,
    char_u	    *fname,
    char_u	    *sfname,
    linenr_T	    ಆರಂಭ,
    linenr_T	    ಕೊನೆ,
    exarg_T	    *eap,		// for forced 'ff' and 'fenc', can be
					// NULL!
    ಪೂರ್ಣಾಂಕ		    append,		// append to the file
    ಪೂರ್ಣಾಂಕ		    forceit,
    ಪೂರ್ಣಾಂಕ		    reset_ಬದಲಾಯಿತು,
    ಪೂರ್ಣಾಂಕ		    filtering)
{
    ಪೂರ್ಣಾಂಕ		    fd;
    char_u	    *backup = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    backup_copy = FALSE; // copy the original file?
    ಪೂರ್ಣಾಂಕ		    dobackup;
    char_u	    *ffname;
    char_u	    *wfname = ಖಾಲಿ;	// name of file to write to
    char_u	    *s;
    char_u	    *ptr;
    char_u	    c;
    ಪೂರ್ಣಾಂಕ		    len;
    linenr_T	    lnum;
    ದೀರ್ಘ	    nchars;
    char_u	    *errmsg = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    errmsg_allocated = FALSE;
    char_u	    *errnum = ಖಾಲಿ;
    char_u	    *ಬಫರ್;
    char_u	    smallbuf[SMALLBUFSIZE];
    char_u	    *backup_ext;
    ಪೂರ್ಣಾಂಕ		    bufsize;
    ದೀರ್ಘ	    perm;		    // file permissions
    ಪೂರ್ಣಾಂಕ		    retval = OK;
    ಪೂರ್ಣಾಂಕ		    newfile = FALSE;	    // TRUE if file doesn't exist yet
    ಪೂರ್ಣಾಂಕ		    ಸಂದೇಶ_save = ಸಂದೇಶ_scroll;
    ಪೂರ್ಣಾಂಕ		    overwriting;	    // TRUE if writing over original
    ಪೂರ್ಣಾಂಕ		    no_eol = FALSE;	    // no end-of-line written
    ಪೂರ್ಣಾಂಕ		    device = FALSE;	    // writing to a device
    stat_T	    st_old;
    ಪೂರ್ಣಾಂಕ		    prev_got_int = got_int;
    ಪೂರ್ಣಾಂಕ		    checking_conversion;
    ಪೂರ್ಣಾಂಕ		    file_readonly = FALSE;  // overwritten file is read-only
#ಆದರೆ defined(UNIX)			    // XXX fix me sometime?
    ಪೂರ್ಣಾಂಕ		    made_writable = FALSE;  // 'w' bit has been set
#ಆದರೆ_ಅಂತ್ಯ
					// writing everything
    ಪೂರ್ಣಾಂಕ		    ಸಂಪೂರ್ಣ = (ಆರಂಭ == 1 && ಕೊನೆ == buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
    linenr_T	    old_ಸಾಲು_ಎಣಿಕೆ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		    attr;
    ಪೂರ್ಣಾಂಕ		    fileformat;
    ಪೂರ್ಣಾಂಕ		    write_bin;
    ರಚನೆ bw_info  write_info;		// info for buf_write_bytes()
    ಪೂರ್ಣಾಂಕ		    converted = FALSE;
    ಪೂರ್ಣಾಂಕ		    notconverted = FALSE;
    char_u	    *fenc;		// effective 'fileencoding'
    char_u	    *fenc_tofree = ಖಾಲಿ; // allocated "fenc"
    ಪೂರ್ಣಾಂಕ		    wb_flags = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
    vim_acl_T	    acl = ಖಾಲಿ;		// ACL copied from original file to
					// backup or new file
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
    ಪೂರ್ಣಾಂಕ		    write_undo_file = FALSE;
    context_sha256_T sha_ctx;
#ಆದರೆ_ಅಂತ್ಯ
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    bkc = get_bkc_flags(buf);
    pos_T	    orig_ಆರಂಭ = buf->b_op_ಆರಂಭ;
    pos_T	    orig_ಕೊನೆ = buf->b_op_ಕೊನೆ;

    ಆದರೆ (fname == ಖಾಲಿ || *fname == NUL)	// safety check
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)
    {
	// This can happen during startup when there is a stray "w" in the
	// vimrc file.
	emsg(_(e_ಖಾಲಿಗಳು_ಬಫರ್));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Disallow writing from .exrc and .vimrc in current directory for
    // security reasons.
    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ FAIL;

    // Avoid a crash for a long name.
    ಆದರೆ (STRLEN(fname) >= MAXPATHL)
    {
	emsg(_(e_name_too_long));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // must init bw_conv_buf and bw_iconv_fd before jumping to "fail"
    write_info.bw_conv_buf = ಖಾಲಿ;
    write_info.bw_conv_error = FALSE;
    write_info.bw_conv_error_lnum = 0;
    write_info.bw_restlen = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
    write_info.bw_iconv_fd = (iconv_t)-1;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    write_info.bw_ಬಫರ್ = buf;
    write_info.bw_ಮುಕ್ತಾಯ = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // After writing a file changedtick changes but we don't want to display
    // the line.
    ex_no_reprint = TRUE;

    // If there is no file name yet, use the one for the written file.
    // BF_NOTEDITED is set to reflect this (in case the write fails).
    // Don't do this when the write is for a filter command.
    // Don't do this when appending.
    // Only do this when 'cpoptions' contains the 'F' flag.
    ಆದರೆ (buf->b_ffname == ಖಾಲಿ
	    && reset_ಬದಲಾಯಿತು
	    && ಸಂಪೂರ್ಣ
	    && buf == curbuf
	    && !bt_nofilename(buf)
	    && !filtering
	    && (!append || vim_strchr(p_cpo, CPO_FNAMEAPP) != ಖಾಲಿ)
	    && vim_strchr(p_cpo, CPO_FNAMEW) != ಖಾಲಿ)
    {
	ಆದರೆ (ಗಣ_rw_fname(fname, sfname) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	buf = curbuf;	    // just in case autocmds made "buf" invalid
    }

    ಆದರೆ (sfname == ಖಾಲಿ)
	sfname = fname;
    // For Unix: Use the short file name whenever possible.
    // Avoids problems with networks and when directory names are changed.
    // Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
    // another directory, which we don't detect
    ffname = fname;			    // remember full fname
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    fname = sfname;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (buf->b_ffname != ಖಾಲಿ && fnamecmp(ffname, buf->b_ffname) == 0)
	overwriting = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	overwriting = FALSE;

    ಆದರೆ (exiting)
	settmode(TMODE_COOK);	    // when exiting allow typeahead now

    ++no_wait_return;		    // don't wait for return yet

    // Set '[ and '] marks to the lines to be written.
    buf->b_op_ಆರಂಭ.lnum = ಆರಂಭ;
    buf->b_op_ಆರಂಭ.ಲಂಬಸಾಲು = 0;
    buf->b_op_ಕೊನೆ.lnum = ಕೊನೆ;
    buf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = 0;

    {
	aco_save_T	aco;
	ಪೂರ್ಣಾಂಕ		buf_ffname = FALSE;
	ಪೂರ್ಣಾಂಕ		buf_sfname = FALSE;
	ಪೂರ್ಣಾಂಕ		buf_fname_f = FALSE;
	ಪೂರ್ಣಾಂಕ		buf_fname_s = FALSE;
	ಪೂರ್ಣಾಂಕ		did_cmd = FALSE;
	ಪೂರ್ಣಾಂಕ		nofile_err = FALSE;
	ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_memline = (buf->b_ml.ml_mfp == ಖಾಲಿ);
	bufref_T	bufref;

	// Apply PRE autocommands.
	// Set curbuf to the buffer to be written.
	// Careful: The autocommands may call buf_write() recursively!
	ಆದರೆ (ffname == buf->b_ffname)
	    buf_ffname = TRUE;
	ಆದರೆ (sfname == buf->b_sfname)
	    buf_sfname = TRUE;
	ಆದರೆ (fname == buf->b_ffname)
	    buf_fname_f = TRUE;
	ಆದರೆ (fname == buf->b_sfname)
	    buf_fname_s = TRUE;

	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, buf);
	ಆದರೆ (curbuf != buf)
	{
	    // Could not find a window for "buf".  Doing more might cause
	    // problems, better bail out.
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಗಣ_bufref(&bufref, buf);

	ಆದರೆ (append)
	{
	    ಆದರೆ (!(did_cmd = apply_autocmds_exarg(EVENT_FILEAPPENDCMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		ಆದರೆ (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		    apply_autocmds_exarg(EVENT_FILEAPPENDPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filtering)
	{
	    apply_autocmds_exarg(EVENT_FILTERWRITEPRE,
					    ಖಾಲಿ, sfname, FALSE, curbuf, eap);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reset_ಬದಲಾಯಿತು && ಸಂಪೂರ್ಣ)
	{
	    ಪೂರ್ಣಾಂಕ was_ಬದಲಾಯಿತು = curbuf_Is_ಬದಲಾಯಿತು();

	    did_cmd = apply_autocmds_exarg(EVENT_BUFWRITECMD,
					  sfname, sfname, FALSE, curbuf, eap);
	    ಆದರೆ (did_cmd)
	    {
		ಆದರೆ (was_ಬದಲಾಯಿತು && !curbuf_Is_ಬದಲಾಯಿತು())
		{
		    // Written everything correctly and BufWriteCmd has reset
		    // 'modified': Correct the undo information so that an
		    // undo now sets 'modified'.
		    u_unchanged(curbuf);
		    u_update_save_nr(curbuf);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		    apply_autocmds_exarg(EVENT_BUFWRITEPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (!(did_cmd = apply_autocmds_exarg(EVENT_FILEWRITECMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		ಆದರೆ (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		    apply_autocmds_exarg(EVENT_FILEWRITEPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);

	// In three situations we return here and don't write the file:
	// 1. the autocommands deleted or unloaded the buffer.
	// 2. The autocommands abort script processing.
	// 3. If one of the "Cmd" autocommands was executed.
	ಆದರೆ (!bufref_valid(&bufref))
	    buf = ಖಾಲಿ;
	ಆದರೆ (buf == ಖಾಲಿ || (buf->b_ml.ml_mfp == ಖಾಲಿ && !ಖಾಲಿಗಳು_memline)
				       || did_cmd || nofile_err
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
				       || aborting()
#ಆದರೆ_ಅಂತ್ಯ
				       )
	{
	    ಆದರೆ (buf != ಖಾಲಿ && (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	    {
		// restore the original '[ and '] positions
		buf->b_op_ಆರಂಭ = orig_ಆರಂಭ;
		buf->b_op_ಕೊನೆ = orig_ಕೊನೆ;
	    }

	    --no_wait_return;
	    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
	    ಆದರೆ (nofile_err)
		semsg(_(e_no_matching_autocommands_for_buftype_str_ಬಫರ್),
							       curbuf->b_p_bt);

	    ಆದರೆ (nofile_err
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    || aborting()
#ಆದರೆ_ಅಂತ್ಯ
		    )
		// An aborting error, interrupt or exception in the
		// autocommands.
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (did_cmd)
	    {
		ಆದರೆ (buf == ಖಾಲಿ)
		    // The buffer was deleted.  We assume it was written
		    // (can't retry anyway).
		    ಹಿಂತಿರುಗಿ OK;
		ಆದರೆ (overwriting)
		{
		    // Assume the buffer was written, update the timestamp.
		    ml_timestamp(buf);
		    ಆದರೆ (append)
			buf->b_flags &= ~BF_NEW;
		    ಇಲ್ಲದಿದ್ದರೆ
			buf->b_flags &= ~BF_WRITE_ಮುಖವಾಡ;
		}
		ಆದರೆ (reset_ಬದಲಾಯಿತು && buf->b_ಬದಲಾಯಿತು && !append
			&& (overwriting || vim_strchr(p_cpo, CPO_PLUS) != ಖಾಲಿ))
		    // Buffer still changed, the autocommands didn't work
		    // properly.
		    ಹಿಂತಿರುಗಿ FAIL;
		ಹಿಂತಿರುಗಿ OK;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಆದರೆ (!aborting())
#ಆದರೆ_ಅಂತ್ಯ
		emsg(_(e_autocommands_deleted_or_unloaded_ಬಫರ್_ಗಮ್ಯ_be_written));
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// The autocommands may have changed the number of lines in the file.
	// When writing the whole file, adjust the end.
	// When writing part of the file, assume that the autocommands only
	// changed the number of lines that are to be written (tricky!).
	ಆದರೆ (buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ != old_ಸಾಲು_ಎಣಿಕೆ)
	{
	    ಆದರೆ (ಸಂಪೂರ್ಣ)						// write all
		ಕೊನೆ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ > old_ಸಾಲು_ಎಣಿಕೆ)	// more lines
		ಕೊನೆ += buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - old_ಸಾಲು_ಎಣಿಕೆ;
	    ಇಲ್ಲದಿದ್ದರೆ						// less lines
	    {
		ಕೊನೆ -= old_ಸಾಲು_ಎಣಿಕೆ - buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		ಆದರೆ (ಕೊನೆ < ಆರಂಭ)
		{
		    --no_wait_return;
		    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
		    emsg(_(e_autocommands_ಬದಲಾಯಿತು_ಸಂಖ್ಯೆ_ದ_ಸಾಲುಗಳು_ಒಳ_unexpected_way));
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	}

	// The autocommands may have changed the name of the buffer, which may
	// be kept in fname, ffname and sfname.  A ":cd" may also have dropped
	// the short name, then b_fname is the full name.
	ಆದರೆ (buf_ffname)
	    ffname = buf->b_ffname;
	ಆದರೆ (buf_sfname)
	    sfname = buf->b_fname;
	ಆದರೆ (buf_fname_f)
	    fname = buf->b_ffname;
	ಆದರೆ (buf_fname_s)
	    fname = buf->b_fname;
    }

    ಆದರೆ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	// restore the original '[ and '] positions
	buf->b_op_ಆರಂಭ = orig_ಆರಂಭ;
	buf->b_op_ಕೊನೆ = orig_ಕೊನೆ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    ಆದರೆ (netbeans_ಸಕ್ರಿಯ() && is_Netbeans_ಬಫರ್(buf))
    {
	ಆದರೆ (ಸಂಪೂರ್ಣ)
	{
	    // b_changed can be 0 after an undo, but we still need to write
	    // the buffer to NetBeans.
	    ಆದರೆ (buf->b_ಬದಲಾಯಿತು || isNetbeansModified(buf))
	    {
		--no_wait_return;		// may wait for return now
		ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
		netbeans_save_ಬಫರ್(buf);	// no error checking...
		ಹಿಂತಿರುಗಿ retval;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		errnum = (char_u *)"E656: ";
		errmsg = (char_u *)_(e_netbeans_disallows_writes_ದ_unmodified_buffers);
		ಬಫರ್ = ಖಾಲಿ;
		ಹೋಗು fail;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    errnum = (char_u *)"E657: ";
	    errmsg = (char_u *)_(e_partial_writes_disallowed_for_netbeans_buffers);
	    ಬಫರ್ = ಖಾಲಿ;
	    ಹೋಗು fail;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (shortmess(SHM_OVER) && !exiting)
	ಸಂದೇಶ_scroll = FALSE;	    // overwrite previous file message
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_scroll = TRUE;	    // don't overwrite previous file message
    ಆದರೆ (!filtering)
	filemess(buf,
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UNIX
		sfname,
#ಇಲ್ಲದಿದ್ದರೆ
		fname,
#ಆದರೆ_ಅಂತ್ಯ
		    (char_u *)"", 0);	// show that we are busy
    ಸಂದೇಶ_scroll = FALSE;		    // always overwrite the file message now

    ಬಫರ್ = alloc(WRITEBUFSIZE);
    ಆದರೆ (ಬಫರ್ == ಖಾಲಿ)		    // can't allocate big buffer, use small
				    // one (to be able to write when out of
				    // memory)
    {
	ಬಫರ್ = smallbuf;
	bufsize = SMALLBUFSIZE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	bufsize = WRITEBUFSIZE;

    // Get information about original file (if there is one).
#ಆದರೆ defined(UNIX)
    st_old.st_dev = 0;
    st_old.st_ino = 0;
    perm = -1;
    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st_old) < 0)
	newfile = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	perm = st_old.st_ಬಹುಲಕ;
	ಆದರೆ (!S_ISREG(st_old.st_ಬಹುಲಕ))		// not a file
	{
	    ಆದರೆ (S_ISDIR(st_old.st_ಬಹುಲಕ))
	    {
		errnum = (char_u *)"E502: ";
		errmsg = (char_u *)_(e_is_a_directory);
		ಹೋಗು fail;
	    }
	    ಆದರೆ (mch_nodetype(fname) != ಗಂಟು_WRITABLE)
	    {
		errnum = (char_u *)"E503: ";
		errmsg = (char_u *)_(e_is_not_file_or_writable_device);
		ಹೋಗು fail;
	    }
	    // It's a device of some kind (or a fifo) which we can write to
	    // but for which we can't make a backup.
	    device = TRUE;
	    newfile = TRUE;
	    perm = -1;
	}
    }
#ಇಲ್ಲದಿದ್ದರೆ // !UNIX
    // Check for a writable device name.
    c = mch_nodetype(fname);
    ಆದರೆ (c == ಗಂಟು_OTHER)
    {
	errnum = (char_u *)"E503: ";
	errmsg = (char_u *)_(e_is_not_file_or_writable_device);
	ಹೋಗು fail;
    }
    ಆದರೆ (c == ಗಂಟು_WRITABLE)
    {
# ಆದರೆ defined(MSWIN)
	// MS-Windows allows opening a device, but we will probably get stuck
	// trying to write to it.
	ಆದರೆ (!p_odev)
	{
	    errnum = (char_u *)"E796: ";
	    errmsg = (char_u *)_(e_writing_ಗಮ್ಯ_device_disabled_with_opendevice_option);
	    ಹೋಗು fail;
	}
# endif
	device = TRUE;
	newfile = TRUE;
	perm = -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	perm = mch_getperm(fname);
	ಆದರೆ (perm < 0)
	    newfile = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mch_isdir(fname))
	{
	    errnum = (char_u *)"E502: ";
	    errmsg = (char_u *)_(e_is_a_directory);
	    ಹೋಗು fail;
	}
	ಆದರೆ (overwriting)
	    (ಶೂನ್ಯ)mch_stat((ಅಕ್ಷರ *)fname, &st_old);
    }
#ಆದರೆ_ಅಂತ್ಯ // !UNIX

    ಆದರೆ (!device && !newfile)
    {
	// Check if the file is really writable (when renaming the file to
	// make a backup we won't discover it later).
	file_readonly = check_file_readonly(fname, (ಪೂರ್ಣಾಂಕ)perm);

	ಆದರೆ (!forceit && file_readonly)
	{
	    ಆದರೆ (vim_strchr(p_cpo, CPO_FWRITE) != ಖಾಲಿ)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_ಒಳ_cpoptions);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		errnum = (char_u *)"E505: ";
		errmsg = (char_u *)_(e_is_read_only_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override);
	    }
	    ಹೋಗು fail;
	}

	// Check if the timestamp hasn't changed since reading the file.
	ಆದರೆ (overwriting)
	{
	    retval = check_mtime(buf, &st_old);
	    ಆದರೆ (retval == FAIL)
		ಹೋಗು fail;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    ಆದರೆ (!newfile)
	acl = mch_get_acl(fname);
#ಆದರೆ_ಅಂತ್ಯ

    // If 'backupskip' is not empty, don't make a backup for some files.
    dobackup = (p_wb || p_bk || *p_pm != NUL);
    ಆದರೆ (dobackup && *p_bsk != NUL && ಹೊಂದಿಕೆ_file_list(p_bsk, sfname, ffname))
	dobackup = FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cancel writing, only hitting CTRL-C while writing should
    // abort it.
    prev_got_int = got_int;
    got_int = FALSE;

    // Mark the buffer as 'being saved' to prevent changed buffer warnings
    buf->b_saving = true;

    // If we are not appending or filtering, the file exists, and the
    // 'writebackup', 'backup' or 'patchmode' option is set, need a backup.
    // When 'patchmode' is set also make a backup when appending.
    //
    // Do not make any backup, if 'writebackup' and 'backup' are both switched
    // off.  This helps when editing large files on almost-full disks.
    ಆದರೆ (!(append && *p_pm == NUL) && !filtering && perm >= 0 && dobackup)
    {
#ಆದರೆ defined(UNIX) || defined(MSWIN)
	stat_T	    st;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ ((bkc & BKC_YES) || append)	// "yes"
	    backup_copy = TRUE;
#ಆದರೆ defined(UNIX) || defined(MSWIN)
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((bkc & BKC_AUTO))	// "auto"
	{
# ifdef UNIX
	    // Don't rename the file when:
	    // - it's a hard link
	    // - it's a symbolic link
	    // - we don't have write permission in the directory
	    // - we can't set the owner/group of the new file
	    ಆದರೆ (st_old.st_nlink > 1
		    || mch_lstat((ಅಕ್ಷರ *)fname, &st) < 0
		    || st.st_dev != st_old.st_dev
		    || st.st_ino != st_old.st_ino
#  ifndef HAVE_FCHOWN
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid
#  endif
		    )
		backup_copy = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef MSWIN
	    // On NTFS file systems hard links are possible.
	    ಆದರೆ (mch_is_linked(fname))
		backup_copy = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
#  endif
# endif
	    {
		size_t	dirlen;
		char_u	tmp_fname[MAXPATHL];
		ಪೂರ್ಣಾಂಕ	i;

		// Check if we can create a file and set the owner/group/mode to
		// the ones from the original file.
		// First find a file name that doesn't exist yet.  Use the
		// conventional hidden swap file name so that this short-lived
		// probe is covered by standard Vim ignore patterns.
		dirlen = (size_t)(gettail(fname) - fname);
		vim_strncpy(tmp_fname, fname, dirlen);
		fd = -1;
		ಗಾಗಿ (i = 4913; ; i += 123)
		{
		    vim_snprintf((ಅಕ್ಷರ *)tmp_fname + dirlen,
			ಗಾತ್ರ(tmp_fname) - dirlen, ".%d.swp", i);
		    ಆದರೆ (mch_lstat((ಅಕ್ಷರ *)tmp_fname, &st) < 0)
		    {
			fd = mch_open((ಅಕ್ಷರ *)tmp_fname,
				    O_CREAT|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
			ಆದರೆ (fd < 0 && errno == EEXIST)
			    // If the same file name is created by another
			    // process between lstat() and open(), find another
			    // name.
			    ಮುಂದುವರಿ;
			ನಿಲ್ಲಿಸು;
		    }
		}
		ಆದರೆ (fd < 0)	// can't write in directory
		    backup_copy = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		{
# ifdef UNIX
#  ifdef HAVE_FCHOWN
		    vim_ignored = fchown(fd, st_old.st_uid, st_old.st_gid);
#  endif
#  ifdef HAVE_FCHMOD
		    (ಶೂನ್ಯ)mch_fsetperm(fd, perm);
#  ಇಲ್ಲದಿದ್ದರೆ
		    (ಶೂನ್ಯ)mch_setperm(tmp_fname, perm);
#  endif
		    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)tmp_fname, &st) < 0
			    || st.st_uid != st_old.st_uid
			    || st.st_gid != st_old.st_gid
			    || (ದೀರ್ಘ)st.st_ಬಹುಲಕ != perm)
			backup_copy = TRUE;
# endif
		    // Close the file before removing it, on MS-Windows we
		    // can't delete an open file.
		    close(fd);
		    mch_remove(tmp_fname);
# ifdef MSWIN
		    // MS-Windows may trigger a virus scanner to open the
		    // file, we can't delete it then.  Keep trying for half a
		    // second.
		    {
			ಪೂರ್ಣಾಂಕ try;

			ಗಾಗಿ (try = 0; try < 10; ++try)
			{
			    ಆದರೆ (mch_lstat((ಅಕ್ಷರ *)tmp_fname, &st) < 0)
				ನಿಲ್ಲಿಸು;
			    ui_delay(50L, TRUE);  // wait 50 msec
			    mch_remove(tmp_fname);
			}
		    }
# endif
		}
	    }
	}

	// Break symlinks and/or hardlinks if we've been asked to.
	ಆದರೆ ((bkc & BKC_BREAKSYMLINK) || (bkc & BKC_BREAKHARDLINK))
	{
# ifdef UNIX
	    ಪೂರ್ಣಾಂಕ	lstat_res;

	    lstat_res = mch_lstat((ಅಕ್ಷರ *)fname, &st);

	    // Symlinks.
	    ಆದರೆ ((bkc & BKC_BREAKSYMLINK)
		    && lstat_res == 0
		    && st.st_ino != st_old.st_ino)
		backup_copy = FALSE;

	    // Hardlinks.
	    ಆದರೆ ((bkc & BKC_BREAKHARDLINK)
		    && st_old.st_nlink > 1
		    && (lstat_res != 0 || st.st_ino == st_old.st_ino))
		backup_copy = FALSE;
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ defined(MSWIN)
	    // Symlinks.
	    ಆದರೆ ((bkc & BKC_BREAKSYMLINK) && mch_is_symbolic_link(fname))
		backup_copy = FALSE;

	    // Hardlinks.
	    ಆದರೆ ((bkc & BKC_BREAKHARDLINK) && mch_is_hard_link(fname))
		backup_copy = FALSE;
#  endif
# endif
	}

#ಆದರೆ_ಅಂತ್ಯ

	// make sure we have a valid backup extension to use
	ಆದರೆ (*p_bex == NUL)
	    backup_ext = (char_u *)".bak";
	ಇಲ್ಲದಿದ್ದರೆ
	    backup_ext = p_bex;

	ಆದರೆ (backup_copy
		&& (fd = mch_open((ಅಕ್ಷರ *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
	{
	    ಪೂರ್ಣಾಂಕ		bfd;
	    char_u	*copybuf;
	    ಪೂರ್ಣಾಂಕ		some_error = FALSE;
	    stat_T	st_new;
	    char_u	*dirp;
	    char_u	*rootname;
#ಆದರೆ defined(UNIX)
	    ಪೂರ್ಣಾಂಕ		did_ಗಣ_shortname;
	    mode_t	umask_save;
#ಆದರೆ_ಅಂತ್ಯ

	    copybuf = alloc(WRITEBUFSIZE + 1);
	    ಆದರೆ (copybuf == ಖಾಲಿ)
	    {
		some_error = TRUE;	    // out of memory
		ಹೋಗು nobackup;
	    }

	    // Try to make the backup in each directory in the 'bdir' option.
	    //
	    // Unix semantics has it, that we may have a writable file,
	    // that cannot be recreated with a simple open(..., O_CREAT, ) e.g:
	    //  - the directory is not writable,
	    //  - the file may be a symbolic link,
	    //  - the file may belong to another user/group, etc.
	    //
	    // For these reasons, the existing writable file must be truncated
	    // and reused. Creation of a backup COPY will be attempted.
	    dirp = p_bdir;
	    ಆಗ (*dirp)
	    {
		char_u	*p ಬಳಸದ;
		ಪೂರ್ಣಾಂಕ	copybuf_len ಬಳಸದ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		st_new.st_ino = 0;
		st_new.st_dev = 0;
		st_new.st_gid = 0;
#ಆದರೆ_ಅಂತ್ಯ

		// Isolate one directory name, using an entry in 'bdir'.
		copybuf_len = copy_option_part(&dirp, copybuf, WRITEBUFSIZE, ",");

#ಆದರೆ defined(UNIX) || defined(MSWIN)
		p = copybuf + copybuf_len;
		ಆದರೆ (after_pathsep(copybuf, p) && p[-1] == p[-2])
		    // Ends with '//', use full path
		    ಆದರೆ ((p = make_percent_swname(copybuf, p, fname)) != ಖಾಲಿ)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#ಆದರೆ_ಅಂತ್ಯ
		rootname = get_file_ಒಳ_dir(fname, copybuf);
		ಆದರೆ (rootname == ಖಾಲಿ)
		{
		    some_error = TRUE;	    // out of memory
		    ಹೋಗು nobackup;
		}

#ಆದರೆ defined(UNIX)
		did_ಗಣ_shortname = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

		// May try twice if 'shortname' not set.
		ಗಾಗಿ (;;)
		{
		    // Make the backup file name.
		    ಆದರೆ (backup == ಖಾಲಿ)
			backup = buf_modname((buf->b_p_sn || buf->b_shortname),
						 rootname, backup_ext, FALSE);
		    ಆದರೆ (backup == ಖಾಲಿ)
		    {
			vim_free(rootname);
			some_error = TRUE;		// out of memory
			ಹೋಗು nobackup;
		    }

		    // Check if backup file already exists.
		    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)backup, &st_new) >= 0)
		    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
			// Check if backup file is same as original file.
			// May happen when modname() gave the same file back.
			// E.g. silly link, or file name-length reached.
			// If we don't check here, we either ruin the file
			// when copying or erase it after writing. jw.
			ಆದರೆ (st_new.st_dev == st_old.st_dev
					    && st_new.st_ino == st_old.st_ino)
			{
			    VIM_CLEAR(backup);	// no backup file to delete
			    // may try again with 'shortname' set
			    ಆದರೆ (!(buf->b_shortname || buf->b_p_sn))
			    {
				buf->b_shortname = true;
				did_ಗಣ_shortname = TRUE;
				ಮುಂದುವರಿ;
			    }
				// setting shortname didn't help
			    ಆದರೆ (did_ಗಣ_shortname)
				buf->b_shortname = false;
			    ನಿಲ್ಲಿಸು;
			}
#ಆದರೆ_ಅಂತ್ಯ

			// If we are not going to keep the backup file, don't
			// delete an existing one, try to use another name.
			// Change one character, just before the extension.
			ಆದರೆ (!p_bk)
			{
			    char_u	*wp;

			    wp = backup + STRLEN(backup) - 1
							 - STRLEN(backup_ext);
			    ಆದರೆ (wp < backup)	// empty file name ???
				wp = backup;
			    *wp = 'z';
			    ಆಗ (*wp > 'a'
				    && mch_stat((ಅಕ್ಷರ *)backup, &st_new) >= 0)
				--*wp;
			    // They all exist??? Must be something wrong.
			    ಆದರೆ (*wp == 'a')
				VIM_CLEAR(backup);
			}
		    }
		    ನಿಲ್ಲಿಸು;
		}
		vim_free(rootname);

		// Try to create the backup file
		ಆದರೆ (backup != ಖಾಲಿ)
		{
		    // remove old backup, if present
		    mch_remove(backup);
		    // Open with O_EXCL to avoid the file being created while
		    // we were sleeping (symlink hacker attack?). Reset umask
		    // if possible to avoid mch_setperm() below.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		    umask_save = umask(0);
#ಆದರೆ_ಅಂತ್ಯ
		    bfd = mch_open((ಅಕ್ಷರ *)backup,
				O_WRONLY|O_CREAT|O_EXTRA|O_EXCL|O_NOFOLLOW,
								 perm & 0777);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		    (ಶೂನ್ಯ)umask(umask_save);
#ಆದರೆ_ಅಂತ್ಯ
		    ಆದರೆ (bfd < 0)
			VIM_CLEAR(backup);
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// Set file protection same as original file, but
			// strip s-bit.  Only needed if umask() wasn't used
			// above.
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UNIX
			(ಶೂನ್ಯ)mch_setperm(backup, perm & 0777);
#ಇಲ್ಲದಿದ್ದರೆ
			// Try to set the group of the backup same as the
			// original file. If this fails, set the protection
			// bits for the group same as the protection bits for
			// others.
			ಆದರೆ (st_new.st_gid != st_old.st_gid
# ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
				&& fchown(bfd, (uid_t)-1, st_old.st_gid) != 0
# endif
						)
			    mch_setperm(backup,
					  (perm & 0707) | ((perm & 07) << 3));
# ಆದರೆ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
# endif
# ifdef FEAT_XATTR
			mch_copy_xattr(fname, backup);
# endif
#ಆದರೆ_ಅಂತ್ಯ

			// copy the file.
			write_info.bw_fd = bfd;
			write_info.bw_buf = copybuf;
			write_info.bw_flags = FIO_NOCONVERT;
			ಆಗ ((write_info.bw_len = read_eintr(fd, copybuf,
							    WRITEBUFSIZE)) > 0)
			{
			    ಆದರೆ (buf_write_ಬೈಟುಗಳು(&write_info) == FAIL)
			    {
				errmsg = (char_u *)_(e_cant_write_ಗಮ್ಯ_backup_file_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override);
				ನಿಲ್ಲಿಸು;
			    }
			    ui_breakcheck();
			    ಆದರೆ (got_int)
			    {
				errmsg = (char_u *)_(e_interrupted);
				ನಿಲ್ಲಿಸು;
			    }
			}

			ಆದರೆ (close(bfd) < 0 && errmsg == ಖಾಲಿ)
			    errmsg = (char_u *)_(e_close_error_for_backup_file_ಕೂಡಿಸು_bang_ಗಮ್ಯ_write_anyway);
			ಆದರೆ (write_info.bw_len < 0)
			    errmsg = (char_u *)_(e_cant_read_file_for_backup_ಕೂಡಿಸು_bang_ಗಮ್ಯ_write_anyway);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
			ಗಣ_file_ಸಮಯ(backup, st_old.st_atime, st_old.st_mtime);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
			mch_ಗಣ_acl(backup, acl);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XATTR
			mch_copy_xattr(fname, backup);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
			(ಶೂನ್ಯ)mch_copy_file_attribute(fname, backup);
#ಆದರೆ_ಅಂತ್ಯ
			ನಿಲ್ಲಿಸು;
		    }
		}
	    }
    nobackup:
	    close(fd);		// ignore errors for closing read file
	    vim_free(copybuf);

	    ಆದರೆ (backup == ಖಾಲಿ && errmsg == ಖಾಲಿ)
		errmsg = (char_u *)_(e_cannot_create_backup_file_ಕೂಡಿಸು_bang_ಗಮ್ಯ_write_anyway);
	    // ignore errors when forceit is TRUE
	    ಆದರೆ ((some_error || errmsg != ಖಾಲಿ) && !forceit)
	    {
		retval = FAIL;
		ಹೋಗು fail;
	    }
	    errmsg = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u	*dirp;
	    char_u	*p;
	    char_u	*rootname;

	    // Make a backup by renaming the original file.

	    // If 'cpoptions' includes the "W" flag, we don't want to
	    // overwrite a read-only file.  But rename may be possible
	    // anyway, thus we need an extra check here.
	    ಆದರೆ (file_readonly && vim_strchr(p_cpo, CPO_FWRITE) != ಖಾಲಿ)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_ಒಳ_cpoptions);
		ಹೋಗು fail;
	    }

	    // Form the backup file name - change path/fo.o.h to
	    // path/fo.o.h.bak Try all directories in 'backupdir', first one
	    // that works is used.
	    dirp = p_bdir;
	    ಆಗ (*dirp)
	    {
		ಪೂರ್ಣಾಂಕ IObufflen ಬಳಸದ;

		// Isolate one directory name and make the backup file name.
		IObufflen = copy_option_part(&dirp, IObuff, IOSIZE, ",");

#ಆದರೆ defined(UNIX) || defined(MSWIN)
		p = IObuff + IObufflen;
		ಆದರೆ (after_pathsep(IObuff, p) && p[-1] == p[-2])
		    // path ends with '//', use full path
		    ಆದರೆ ((p = make_percent_swname(IObuff, p, fname)) != ಖಾಲಿ)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (backup == ಖಾಲಿ)
		{
		    rootname = get_file_ಒಳ_dir(fname, IObuff);
		    ಆದರೆ (rootname == ಖಾಲಿ)
			backup = ಖಾಲಿ;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			backup = buf_modname(
				(buf->b_p_sn || buf->b_shortname),
						rootname, backup_ext, FALSE);
			vim_free(rootname);
		    }
		}

		ಆದರೆ (backup != ಖಾಲಿ)
		{
		    // If we are not going to keep the backup file, don't
		    // delete an existing one, try to use another name.
		    // Change one character, just before the extension.
		    ಆದರೆ (!p_bk && mch_getperm(backup) >= 0)
		    {
			p = backup + STRLEN(backup) - 1 - STRLEN(backup_ext);
			ಆದರೆ (p < backup)	// empty file name ???
			    p = backup;
			*p = 'z';
			ಆಗ (*p > 'a' && mch_getperm(backup) >= 0)
			    --*p;
			// They all exist??? Must be something wrong!
			ಆದರೆ (*p == 'a')
			    VIM_CLEAR(backup);
		    }
		}
		ಆದರೆ (backup != ಖಾಲಿ)
		{
		    // Delete any existing backup and move the current version
		    // to the backup.	For safety, we don't remove the backup
		    // until the write has finished successfully. And if the
		    // 'backup' option is set, leave it around.

		    // If the renaming of the original file to the backup file
		    // works, quit here.
		    ಆದರೆ (vim_rename(fname, backup) == 0)
			ನಿಲ್ಲಿಸು;

		    VIM_CLEAR(backup);   // don't do the rename below
		}
	    }
	    ಆದರೆ (backup == ಖಾಲಿ && !forceit)
	    {
		errmsg = (char_u *)_(e_cant_make_backup_file_ಕೂಡಿಸು_bang_ಗಮ್ಯ_write_anyway);
		ಹೋಗು fail;
	    }
	}
    }

#ಆದರೆ defined(UNIX)
    // When using ":w!" and the file was read-only: make it writable
    ಆದರೆ (forceit && perm >= 0 && !(perm & 0200) && st_old.st_uid == getuid()
				     && vim_strchr(p_cpo, CPO_FWRITE) == ಖಾಲಿ)
    {
	perm |= 0200;
	(ಶೂನ್ಯ)mch_setperm(fname, perm);
	made_writable = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // When using ":w!" and writing to the current file, 'readonly' makes no
    // sense, reset it, unless 'Z' appears in 'cpoptions'.
    ಆದರೆ (forceit && overwriting && vim_strchr(p_cpo, CPO_KEEPRO) == ಖಾಲಿ)
    {
	buf->b_p_ro = FALSE;
	need_maketitle = TRUE;	    // set window title later
	ಸ್ಥಿತಿ_redraw_all();	    // redraw status lines later
    }

    ಆದರೆ (ಕೊನೆ > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಕೊನೆ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಆದರೆ (buf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
	ಆರಂಭ = ಕೊನೆ + 1;

    // If the original file is being overwritten, there is a small chance that
    // we crash in the middle of writing. Therefore the file is preserved now.
    // This makes all block numbers positive so that recovery does not need
    // the original file.
    // Don't do this if there is a backup file and we are exiting.
    ಆದರೆ (reset_ಬದಲಾಯಿತು && !newfile && overwriting
					      && !(exiting && backup != ಖಾಲಿ))
    {
	ml_preserve(buf, FALSE);
	ಆದರೆ (got_int)
	{
	    errmsg = (char_u *)_(e_interrupted);
	    ಹೋಗು restore_backup;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    vms_remove_ಆವೃತ್ತಿ(fname); // remove version
#ಆದರೆ_ಅಂತ್ಯ
    // Default: write the file directly.  May write to a temp file for
    // multi-byte conversion.
    wfname = fname;

    // Check for forced 'fileencoding' from "++opt=val" argument.
    ಆದರೆ (eap != ಖಾಲಿ && eap->force_enc != 0)
    {
	fenc = eap->cmd + eap->force_enc;
	fenc = enc_canonize(fenc);
	fenc_tofree = fenc;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	fenc = buf->b_p_fenc;

    // Check if the file needs to be converted.
    converted = need_conversion(fenc);

    // Check if UTF-8 to UCS-2/4 or Latin1 conversion needs to be done.  Or
    // Latin1 to Unicode conversion.  This is handled in buf_write_bytes().
    // Prepare the flags for it and allocate bw_conv_buf when needed.
    ಆದರೆ (converted && (enc_utf8 || STRCMP(p_enc, "latin1") == 0))
    {
	wb_flags = get_fio_flags(fenc);
	ಆದರೆ (wb_flags & (FIO_UCS2 | FIO_UCS4 | FIO_UTF16 | FIO_UTF8))
	{
	    // overallocate a bit, in case we read incomplete multi-byte chars
	    ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = bufsize + CONV_RESTLEN;
	    // Need to allocate a buffer to translate into.
	    ಆದರೆ (wb_flags & (FIO_UCS2 | FIO_UTF16 | FIO_UTF8))
		write_info.bw_conv_buflen = ಗಾತ್ರ_2 * 2;
	    ಇಲ್ಲದಿದ್ದರೆ // FIO_UCS4
		write_info.bw_conv_buflen = ಗಾತ್ರ_2 * 4;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    ಆದರೆ (write_info.bw_conv_buf == ಖಾಲಿ)
		ಕೊನೆ = 0;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಆದರೆ (converted && wb_flags == 0 && (wb_flags = get_win_fio_flags(fenc)) != 0)
    {
	// Convert UTF-8 -> UCS-2 and UCS-2 -> DBCS.  Worst-case * 4:
	write_info.bw_conv_buflen = bufsize * 4;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	ಆದರೆ (write_info.bw_conv_buf == ಖಾಲಿ)
	    ಕೊನೆ = 0;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_CONVERT
    ಆದರೆ (converted && wb_flags == 0 && (wb_flags = get_mac_fio_flags(fenc)) != 0)
    {
	write_info.bw_conv_buflen = bufsize * 3;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	ಆದರೆ (write_info.bw_conv_buf == ಖಾಲಿ)
	    ಕೊನೆ = 0;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL) || defined(USE_ICONV)
    ಆದರೆ (converted && wb_flags == 0)
    {
# ifdef USE_ICONV
	// Use iconv() conversion when conversion is needed and it's not done
	// internally.
	write_info.bw_iconv_fd = (iconv_t)my_iconv_open(fenc,
					enc_utf8 ? (char_u *)"utf-8" : p_enc);
	ಆದರೆ (write_info.bw_iconv_fd != (iconv_t)-1)
	{
	    // We're going to use iconv(), allocate a buffer to convert in.
	    write_info.bw_conv_buflen = bufsize * ICONV_MULT;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    ಆದರೆ (write_info.bw_conv_buf == ಖಾಲಿ)
		ಕೊನೆ = 0;
	    write_info.bw_ಮೊದಲ = TRUE;
	}
#  ifdef FEAT_EVAL
	ಇಲ್ಲದಿದ್ದರೆ
#  endif
# endif

# ifdef FEAT_EVAL
	    // When the file needs to be converted with 'charconvert' after
	    // writing, write to a temp file instead and let the conversion
	    // overwrite the original file.
	    ಆದರೆ (*p_ccv != NUL)
	    {
		wfname = vim_tempname('w', FALSE);
		ಆದರೆ (wfname == ಖಾಲಿ)	// Can't write without a tempfile!
		{
		    errmsg = (char_u *)_(e_cant_ಕಂಡುಹಿಡಿ_ತಾತ್ಕಾಲಿಕ_file_for_writing);
		    ಹೋಗು restore_backup;
		}
	    }
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (converted && wb_flags == 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
	    && write_info.bw_iconv_fd == (iconv_t)-1
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    && wfname == fname
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	ಆದರೆ (!forceit)
	{
	    errmsg = (char_u *)_(e_cannot_convert_ಕೂಡಿಸು_bang_ಗಮ್ಯ_write_without_conversion);
	    ಹೋಗು restore_backup;
	}
	notconverted = TRUE;
    }

    // If conversion is taking place, we may first pretend to write and check
    // for conversion errors.  Then loop again to write for real.
    // When not doing conversion this writes for real right away.
    ಗಾಗಿ (checking_conversion = TRUE; ; checking_conversion = FALSE)
    {
	// There is no need to check conversion when:
	// - there is no conversion
	// - we make a backup file, that can be restored in case of conversion
	//   failure.
	ಆದರೆ (!converted || dobackup)
	    checking_conversion = FALSE;

	ಆದರೆ (checking_conversion)
	{
	    // Make sure we don't write anything.
	    fd = -1;
	    write_info.bw_fd = fd;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FTRUNCATE
# define TRUNC_ON_OPEN 0
#ಇಲ್ಲದಿದ್ದರೆ
# define TRUNC_ON_OPEN O_TRUNC
#ಆದರೆ_ಅಂತ್ಯ
	    // Open the file "wfname" for writing.
	    // We may try to open the file twice: If we can't write to the file
	    // and forceit is TRUE we delete the existing file and try to
	    // create a new one. If this still fails we may have lost the
	    // original file!  (this may happen when the user reached his
	    // quotum for number of files).
	    // Appending will fail if the file does not exist and forceit is
	    // FALSE.
	    ಆಗ ((fd = mch_open((ಅಕ್ಷರ *)wfname, O_WRONLY | O_EXTRA | (append
				? (forceit ? (O_APPEND | O_CREAT) : O_APPEND)
				: (O_CREAT | TRUNC_ON_OPEN))
				, perm < 0 ? 0666 : (perm & 0777))) < 0)
	    {
		// A forced write will try to create a new file if the old one
		// is still readonly. This may also happen when the directory
		// is read-only. In that case the mch_remove() will fail.
		ಆದರೆ (errmsg == ಖಾಲಿ)
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		    stat_T	st;

		    // Don't delete the file when it's a hard or symbolic link.
		    ಆದರೆ ((!newfile && st_old.st_nlink > 1)
			    || (mch_lstat((ಅಕ್ಷರ *)fname, &st) == 0
				&& (st.st_dev != st_old.st_dev
				    || st.st_ino != st_old.st_ino)))
			errmsg =
			      (char_u *)_(e_cant_open_linked_file_for_writing);
		    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    {
			errmsg = (char_u *)_(e_cant_open_file_for_writing);
			ಆದರೆ (forceit && vim_strchr(p_cpo, CPO_FWRITE) == ಖಾಲಿ
								  && perm >= 0)
			{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
			    // we write to the file, thus it should be marked
			    // writable after all
			    ಆದರೆ (!(perm & 0200))
				made_writable = TRUE;
			    perm |= 0200;
			    ಆದರೆ (st_old.st_uid != getuid()
						  || st_old.st_gid != getgid())
				perm &= 0777;
#ಆದರೆ_ಅಂತ್ಯ
			    ಆದರೆ (!append)  // don't remove when appending
				mch_remove(wfname);
			    ಮುಂದುವರಿ;
			}
		    }
		}

restore_backup:
		{
		    stat_T	st;

		    // If we failed to open the file, we don't need a backup.
		    // Throw it away.  If we moved or removed the original file
		    // try to put the backup in its place.
		    ಆದರೆ (backup != ಖಾಲಿ && wfname == fname)
		    {
			ಆದರೆ (backup_copy)
			{
			    // There is a small chance that we removed the
			    // original, try to move the copy in its place.
			    // This may not work if the vim_rename() fails.
			    // In that case we leave the copy around.

			    // If file does not exist, put the copy in its
			    //ಇರಿಸು
			    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st) < 0)
				vim_rename(backup, fname);
			    // if original file does exist throw away the copy
			    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st) >= 0)
				mch_remove(backup);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // try to put the original file back
			    vim_rename(backup, fname);
			}
		    }

		    // if original file no longer exists give an extra warning
		    ಆದರೆ (!newfile && mch_stat((ಅಕ್ಷರ *)fname, &st) < 0)
			ಕೊನೆ = 0;
		}

		ಆದರೆ (wfname != fname)
		    vim_free(wfname);
		ಹೋಗು fail;
	    }
	    write_info.bw_fd = fd;

#ಆದರೆ defined(UNIX)
	    {
		stat_T	st;

		// Double check we are writing the intended file before making
		// any changes.
		ಆದರೆ (overwriting
			&& (!dobackup || backup_copy)
			&& fname == wfname
			&& perm >= 0
			&& mch_fstat(fd, &st) == 0
			&& st.st_ino != st_old.st_ino)
		{
		    close(fd);
		    errmsg = (char_u *)_(e_file_ಬದಲಾಯಿತು_while_writing);
		    ಹೋಗು fail;
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FTRUNCATE
	    ಆದರೆ (!append)
		vim_ignored = ftruncate(fd, (off_t)0);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MSWIN)
	    ಆದರೆ (backup != ಖಾಲಿ && overwriting && !append)
		(ಶೂನ್ಯ)mch_copy_file_attribute(backup, wfname);

	    ಆದರೆ (!overwriting && !append)
	    {
		ಆದರೆ (buf->b_ffname != ಖಾಲಿ)
		    (ಶೂನ್ಯ)mch_copy_file_attribute(buf->b_ffname, wfname);
		// Should copy resource fork
	    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	    ಆದರೆ (*buf->b_p_ಕೀಲಿ != NUL && !filtering)
	    {
		char_u		*ಶೀರ್ಷಿಕೆ;
		ಪೂರ್ಣಾಂಕ		ಶೀರ್ಷಿಕೆ_len;

		buf->b_cryptstate = crypt_create_for_writing(
						      crypt_get_ವಿಧಾನ_nr(buf),
					   buf->b_p_ಕೀಲಿ, &ಶೀರ್ಷಿಕೆ, &ಶೀರ್ಷಿಕೆ_len);
		ಆದರೆ (buf->b_cryptstate == ಖಾಲಿ || ಶೀರ್ಷಿಕೆ == ಖಾಲಿ)
		    ಕೊನೆ = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Write magic number, so that Vim knows how this file is
		    // encrypted when reading it back.
		    write_info.bw_buf = ಶೀರ್ಷಿಕೆ;
		    write_info.bw_len = ಶೀರ್ಷಿಕೆ_len;
		    write_info.bw_flags = FIO_NOCONVERT;
		    ಆದರೆ (buf_write_ಬೈಟುಗಳು(&write_info) == FAIL)
			ಕೊನೆ = 0;
		    wb_flags |= FIO_ENCRYPTED;
		    vim_free(ಶೀರ್ಷಿಕೆ);
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
	errmsg = ಖಾಲಿ;

	write_info.bw_buf = ಬಫರ್;
	nchars = 0;

	// use "++bin", "++nobin" or 'binary'
	ಆದರೆ (eap != ಖಾಲಿ && eap->force_bin != 0)
	    write_bin = (eap->force_bin == FORCE_BIN);
	ಇಲ್ಲದಿದ್ದರೆ
	    write_bin = buf->b_p_bin;

	// The BOM is written just after the encryption magic number.
	// Skip it when appending and the file already existed, the BOM only
	// makes sense at the start of the file.
	ಆದರೆ (buf->b_p_bomb && !write_bin && (!append || perm < 0))
	{
	    write_info.bw_len = make_bom(ಬಫರ್, fenc);
	    ಆದರೆ (write_info.bw_len > 0)
	    {
		// don't convert, do encryption
		write_info.bw_flags = FIO_NOCONVERT | wb_flags;
		ಆದರೆ (buf_write_ಬೈಟುಗಳು(&write_info) == FAIL)
		    ಕೊನೆ = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		    nchars += write_info.bw_len;
	    }
	}
	write_info.bw_ಆರಂಭ_lnum = ಆರಂಭ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
	write_undo_file = (buf->b_p_udf
			    && overwriting
			    && !append
			    && !filtering
# ifdef CRYPT_NOT_INPLACE
			    // writing undo file requires
			    // crypt_encode_inplace()
			    && (buf->b_cryptstate == ಖಾಲಿ
				|| crypt_ಕೆಲಸಮಾಡುತ್ತದೆ_inplace(buf->b_cryptstate))
# endif
			    && reset_ಬದಲಾಯಿತು
			    && !checking_conversion);
# ifdef CRYPT_NOT_INPLACE
	// remove undo file if encrypting it is not possible
	ಆದರೆ (buf->b_p_udf
		&& overwriting
		&& !append
		&& !filtering
		&& !checking_conversion
		&& buf->b_cryptstate != ಖಾಲಿ
		&& !crypt_ಕೆಲಸಮಾಡುತ್ತದೆ_inplace(buf->b_cryptstate))
	    u_undofile_reset_and_delete(buf);
# endif
	ಆದರೆ (write_undo_file)
	    // Prepare for computing the hash value of the text.
	    sha256_ಆರಂಭ(&sha_ctx);
#ಆದರೆ_ಅಂತ್ಯ

	write_info.bw_len = bufsize;
	write_info.bw_flags = wb_flags;
	fileformat = get_fileformat_force(buf, eap);
	s = ಬಫರ್;
	len = 0;
	ಗಾಗಿ (lnum = ಆರಂಭ; lnum <= ಕೊನೆ; ++lnum)
	{
	    // The next while loop is done once for each character written.
	    // Keep it fast!
	    ptr = ml_get_buf(buf, lnum, FALSE) - 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
	    ಆದರೆ (write_undo_file)
		sha256_update(&sha_ctx, ptr + 1,
					      (UINT32_T)(STRLEN(ptr + 1) + 1));
#ಆದರೆ_ಅಂತ್ಯ
	    ಆಗ ((c = *++ptr) != NUL)
	    {
		ಆದರೆ (c == NL)
		    *s = NUL;		// replace newlines with NULs
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == CAR && fileformat == EOL_MAC)
		    *s = NL;		// Mac: replace CRs with NLs
		ಇಲ್ಲದಿದ್ದರೆ
		    *s = c;
		++s;
		ಆದರೆ (++len != bufsize)
		    ಮುಂದುವರಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
		ಆದರೆ (write_info.bw_fd > 0 && lnum == ಕೊನೆ
			&& (write_info.bw_flags & FIO_ENCRYPTED)
			&& *buf->b_p_ಕೀಲಿ != NUL && !filtering
			&& *ptr == NUL)
		    write_info.bw_ಮುಕ್ತಾಯ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (buf_write_ಬೈಟುಗಳು(&write_info) == FAIL)
		{
		    ಕೊನೆ = 0;		// write error: break loop
		    ನಿಲ್ಲಿಸು;
		}
		nchars += bufsize;
		s = ಬಫರ್;
		len = 0;
		write_info.bw_ಆರಂಭ_lnum = lnum;
	    }
	    // write failed or last line has no EOL: stop here
	    ಆದರೆ (ಕೊನೆ == 0
		    || (lnum == ಕೊನೆ
			&& (write_bin || !buf->b_p_fixeol)
			&& ((write_bin && lnum == buf->b_no_eol_lnum)
			    || (lnum == buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
							   && !buf->b_p_eol))))
	    {
		++lnum;			// written the line, count it
		no_eol = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (fileformat == EOL_UNIX)
		*s++ = NL;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		*s++ = CAR;		    // EOL_MAC or EOL_DOS: write CR
		ಆದರೆ (fileformat == EOL_DOS)  // write CR-NL
		{
		    ಆದರೆ (++len == bufsize)
		    {
			ಆದರೆ (buf_write_ಬೈಟುಗಳು(&write_info) == FAIL)
			{
			    ಕೊನೆ = 0;	// write error: break loop
			    ನಿಲ್ಲಿಸು;
			}
			nchars += bufsize;
			s = ಬಫರ್;
			len = 0;
		    }
		    *s++ = NL;
		}
	    }
	    ಆದರೆ (++len == bufsize && ಕೊನೆ)
	    {
		ಆದರೆ (buf_write_ಬೈಟುಗಳು(&write_info) == FAIL)
		{
		    ಕೊನೆ = 0;		// write error: break loop
		    ನಿಲ್ಲಿಸು;
		}
		nchars += bufsize;
		s = ಬಫರ್;
		len = 0;

		ui_breakcheck();
		ಆದರೆ (got_int)
		{
		    ಕೊನೆ = 0;		// Interrupted, break loop
		    ನಿಲ್ಲಿಸು;
		}
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
	    // On VMS there is a problem: newlines get added when writing
	    // blocks at a time. Fix it by writing a line at a time.
	    // This is much slower!
	    // Explanation: VAX/DECC RTL insists that records in some RMS
	    // structures end with a newline (carriage return) character, and
	    // if they don't it adds one.
	    // With other RMS structures it works perfect without this fix.
# ifndef MIN
// Older DECC compiler for VAX doesn't define MIN()
#  define MIN(a, b) ((a) < (b) ? (a) : (b))
# endif
	    ಆದರೆ (buf->b_fab_rfm == FAB$C_VFC
		    || ((buf->b_fab_rat & (FAB$M_FTN | FAB$M_CR)) != 0))
	    {
		ಪೂರ್ಣಾಂಕ b2write;

		buf->b_fab_mrs = (buf->b_fab_mrs == 0
			? MIN(4096, bufsize)
			: MIN(buf->b_fab_mrs, bufsize));

		b2write = len;
		ಆಗ (b2write > 0)
		{
		    write_info.bw_len = MIN(b2write, buf->b_fab_mrs);
		    ಆದರೆ (buf_write_ಬೈಟುಗಳು(&write_info) == FAIL)
		    {
			ಕೊನೆ = 0;
			ನಿಲ್ಲಿಸು;
		    }
		    b2write -= MIN(b2write, buf->b_fab_mrs);
		}
		write_info.bw_len = bufsize;
		nchars += len;
		s = ಬಫರ್;
		len = 0;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಆದರೆ (len > 0 && ಕೊನೆ > 0)
	{
	    write_info.bw_len = len;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	    ಆದರೆ (write_info.bw_fd > 0 && lnum >= ಕೊನೆ
		    && (write_info.bw_flags & FIO_ENCRYPTED)
		    && *buf->b_p_ಕೀಲಿ != NUL && !filtering)
		write_info.bw_ಮುಕ್ತಾಯ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (buf_write_ಬೈಟುಗಳು(&write_info) == FAIL)
		ಕೊನೆ = 0;		    // write error
	    nchars += len;
	}

	ಆದರೆ (!buf->b_p_fixeol && buf->b_p_eof)
	{
	    // write trailing CTRL-Z
	    (ಶೂನ್ಯ)write_eintr(write_info.bw_fd, "\x1a", 1);
	    nchars++;
	}

	// Stop when writing done or an error was encountered.
	ಆದರೆ (!checking_conversion || ಕೊನೆ == 0)
	    ನಿಲ್ಲಿಸು;

	// If no error happened until now, writing should be ok, so loop to
	// really write the buffer.
    }

    // If we started writing, finish writing. Also when an error was
    // encountered.
    ಆದರೆ (!checking_conversion)
    {
#ಆದರೆ defined(UNIX) && defined(HAVE_FSYNC)
	// On many journaling file systems there is a bug that causes both the
	// original and the backup file to be lost when halting the system
	// right after writing the file.  That's because only the meta-data is
	// journalled.  Syncing the file slows down the system, but assures it
	// has been written to disk and we don't lose it.
	// For a device do try the fsync() but don't complain if it does not
	// work (could be a pipe).
	// If the 'fsync' option is FALSE, don't fsync().  Useful for laptops.
	ಆದರೆ ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && vim_fsync(fd) != 0
		&& !device)
	{
	    errmsg = (char_u *)_(e_fsync_failed);
	    ಕೊನೆ = 0;
	}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_SELINUX) || defined(HAVE_SMACK) || defined(FEAT_XATTR)
	// Probably need to set the security context.
	ಆದರೆ (!backup_copy)
	{
# ಆದರೆ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
	    mch_copy_sec(backup, wfname);
# endif
# ifdef FEAT_XATTR
	    mch_copy_xattr(backup, wfname);
# endif
	}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	// When creating a new file, set its owner/group to that of the
	// original file.  Get the new device and inode number.
	ಆದರೆ (backup != ಖಾಲಿ && !backup_copy)
	{
# ifdef HAVE_FCHOWN
	    stat_T	st;

	    // Don't change the owner when it's already OK, some systems remove
	    // permission or ACL stuff.
	    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)wfname, &st) < 0
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid)
	    {
		// changing owner might not be possible
		vim_ignored = fchown(fd, st_old.st_uid, -1);
		// if changing group fails clear the group permissions
		ಆದರೆ (fchown(fd, -1, st_old.st_gid) == -1 && perm > 0)
		    perm &= ~070;
	    }
# endif
	    buf_setino(buf);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!buf->b_dev_valid)
	    // Set the inode when creating a new file.
	    buf_setino(buf);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	ಆದರೆ (made_writable)
	    perm &= ~0200;	// reset 'w' bit for security reasons
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCHMOD
	// set permission of new file same as old file
	ಆದರೆ (perm >= 0)
	    (ಶೂನ್ಯ)mch_fsetperm(fd, perm);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (close(fd) != 0)
	{
	    errmsg = (char_u *)_(e_close_failed);
	    ಕೊನೆ = 0;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_FCHMOD
	// set permission of new file same as old file
	ಆದರೆ (perm >= 0)
	    (ಶೂನ್ಯ)mch_setperm(wfname, perm);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
	// Probably need to set the ACL before changing the user (can't set the
	// ACL on a file the user doesn't own).
	// On Solaris, with ZFS and the aclmode property set to "discard" (the
	// default), chmod() discards all part of a file's ACL that don't
	// represent the mode of the file.  It's non-trivial for us to discover
	// whether we're in that situation, so we simply always re-set the ACL.
# ifndef HAVE_SOLARIS_ZFS_ACL
	ಆದರೆ (!backup_copy)
# endif
	    mch_ಗಣ_acl(wfname, acl);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	ಆದರೆ (buf->b_cryptstate != ಖಾಲಿ)
	{
	    crypt_free_state(buf->b_cryptstate);
	    buf->b_cryptstate = ಖಾಲಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
	ಆದರೆ (wfname != fname)
	{
	    // The file was written to a temp file, now it needs to be
	    // converted with 'charconvert' to (overwrite) the output file.
	    ಆದರೆ (ಕೊನೆ != 0)
	    {
		ಆದರೆ (eval_charconvert(enc_utf8 ? (char_u *)"utf-8" : p_enc,
						  fenc, wfname, fname) == FAIL)
		{
		    write_info.bw_conv_error = TRUE;
		    ಕೊನೆ = 0;
		}
	    }
	    mch_remove(wfname);
	    vim_free(wfname);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (ಕೊನೆ == 0)
    {
	// Error encountered.
	ಆದರೆ (errmsg == ಖಾಲಿ)
	{
	    ಆದರೆ (write_info.bw_conv_error)
	    {
		ಆದರೆ (write_info.bw_conv_error_lnum == 0)
		    errmsg = (char_u *)_(e_write_error_conversion_failed_make_fenc_ಖಾಲಿಗಳು_ಗಮ್ಯ_override);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    errmsg_allocated = TRUE;
		    errmsg = alloc(300);
		    ಆದರೆ (errmsg == ಖಾಲಿ)
			ಹೋಗು fail;
		    vim_snprintf((ಅಕ್ಷರ *)errmsg, 300, _(e_write_error_conversion_failed_ಒಳ_ಸಾಲು_nr_make_fenc_ಖಾಲಿಗಳು_ಗಮ್ಯ_override),
					 (ದೀರ್ಘ)write_info.bw_conv_error_lnum);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (got_int)
		errmsg = (char_u *)_(e_interrupted);
	    ಇಲ್ಲದಿದ್ದರೆ
		errmsg = (char_u *)_(e_write_error_file_system_ತುಂಬಿದ);
	}

	// If we have a backup file, try to put it in place of the new file,
	// because the new file is probably corrupt.  This avoids losing the
	// original file when trying to make a backup when writing the file a
	// second time.
	// When "backup_copy" is set we need to copy the backup over the new
	// file.  Otherwise rename the backup file.
	// If this is OK, don't give the extra warning message.
	ಆದರೆ (backup != ಖಾಲಿ)
	{
	    ಆದರೆ (backup_copy)
	    {
		// This may take a while, if we were interrupted let the user
		// know we got the message.
		ಆದರೆ (got_int)
		{
		    ಸಂದೇಶ(_(e_interrupted));
		    ಹೊರಹೋಗು_flush();
		}
		ಆದರೆ ((fd = mch_open((ಅಕ್ಷರ *)backup, O_RDONLY | O_EXTRA, 0)) >= 0)
		{
		    ಆದರೆ ((write_info.bw_fd = mch_open((ಅಕ್ಷರ *)fname,
				    O_WRONLY | O_CREAT | O_TRUNC | O_EXTRA,
							   perm & 0777)) >= 0)
		    {
			// copy the file.
			write_info.bw_buf = smallbuf;
			write_info.bw_flags = FIO_NOCONVERT;
			ಆಗ ((write_info.bw_len = read_eintr(fd, smallbuf,
						      SMALLBUFSIZE)) > 0)
			    ಆದರೆ (buf_write_ಬೈಟುಗಳು(&write_info) == FAIL)
				ನಿಲ್ಲಿಸು;

			ಆದರೆ (close(write_info.bw_fd) >= 0
						   && write_info.bw_len == 0)
			    ಕೊನೆ = 1;		// success
		    }
		    close(fd);	// ignore errors for closing read file
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (vim_rename(backup, fname) == 0)
		    ಕೊನೆ = 1;
	    }
	}
	ಹೋಗು fail;
    }

    lnum -= ಆರಂಭ;	    // compute number of written lines
    --no_wait_return;	    // may wait for return now

#ಆದರೆ !(defined(UNIX) || defined(VMS))
    fname = sfname;	    // use shortname now, for the messages
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!filtering)
    {
	ಸಂದೇಶ_ಕೂಡಿಸು_fname(buf, fname);	// put fname in IObuff with quotes
	c = FALSE;
	ಆದರೆ (write_info.bw_conv_error)
	{
	    STRCAT(IObuff, _(" CONVERSION ERROR"));
	    c = TRUE;
	    ಆದರೆ (write_info.bw_conv_error_lnum != 0)
		vim_snprintf_ಕೂಡಿಸು((ಅಕ್ಷರ *)IObuff, IOSIZE, _(" in line %ld;"),
			(ದೀರ್ಘ)write_info.bw_conv_error_lnum);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (notconverted)
	{
	    STRCAT(IObuff, _("[NOT converted]"));
	    c = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (converted)
	{
	    STRCAT(IObuff, _("[converted]"));
	    c = TRUE;
	}
	ಆದರೆ (device)
	{
	    STRCAT(IObuff, _("[Device]"));
	    c = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (newfile)
	{
	    STRCAT(IObuff, new_file_ಸಂದೇಶ());
	    c = TRUE;
	}
	ಆದರೆ (no_eol)
	{
	    ಸಂದೇಶ_ಕೂಡಿಸು_eol();
	    c = TRUE;
	}
	// may add [unix/dos/mac]
	ಆದರೆ (ಸಂದೇಶ_ಕೂಡಿಸು_fileformat(fileformat))
	    c = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	ಆದರೆ (wb_flags & FIO_ENCRYPTED)
	{
	    crypt_append_ಸಂದೇಶ(buf);
	    c = TRUE;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಸಂದೇಶ_ಕೂಡಿಸು_ಸಾಲುಗಳು(c, (ದೀರ್ಘ)lnum, nchars);	// add line/char count
	ಆದರೆ (!shortmess(SHM_WRITE))
	{
	    ಆದರೆ (append)
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [a]") : _(" appended"));
	    ಇಲ್ಲದಿದ್ದರೆ
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [w]") : _(" written"));
	}

	ಗಣ_keep_ಸಂದೇಶ((char_u *)ಸಂದೇಶ_trunc_attr((ಅಕ್ಷರ *)IObuff, FALSE, 0), 0);
    }

    // When written everything correctly: reset 'modified'.  Unless not
    // writing to the original file and '+' is not in 'cpoptions'.
    ಆದರೆ (reset_ಬದಲಾಯಿತು && ಸಂಪೂರ್ಣ && !append
	    && !write_info.bw_conv_error
	    && (overwriting || vim_strchr(p_cpo, CPO_PLUS) != ಖಾಲಿ))
    {
	unchanged(buf, TRUE, FALSE);
	// b:changedtick may be incremented in unchanged() but that should not
	// trigger a TextChanged event.
	ಆದರೆ (buf->b_ಕೊನೆಯ_changedtick + 1 == CHANGEDTICK(buf))
	    buf->b_ಕೊನೆಯ_changedtick = CHANGEDTICK(buf);
	u_unchanged(buf);
	u_update_save_nr(buf);
    }

    // If written to the current file, update the timestamp of the swap file
    // and reset the BF_WRITE_MASK flags. Also sets buf->b_mtime.
    ಆದರೆ (overwriting)
    {
	ml_timestamp(buf);
	ಆದರೆ (append)
	    buf->b_flags &= ~BF_NEW;
	ಇಲ್ಲದಿದ್ದರೆ
	    buf->b_flags &= ~BF_WRITE_ಮುಖವಾಡ;
    }

    // If we kept a backup until now, and we are in patch mode, then we make
    // the backup file our 'original' file.
    ಆದರೆ (*p_pm && dobackup)
    {
	ಅಕ್ಷರ *org = (ಅಕ್ಷರ *)buf_modname((buf->b_p_sn || buf->b_shortname),
							  fname, p_pm, FALSE);

	ಆದರೆ (backup != ಖಾಲಿ)
	{
	    stat_T	st;

	    // If the original file does not exist yet
	    // the current backup file becomes the original file
	    ಆದರೆ (org == ಖಾಲಿ)
		emsg(_(e_patchmode_cant_save_original_file));
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mch_stat(org, &st) < 0)
	    {
		vim_rename(backup, (char_u *)org);
		VIM_CLEAR(backup);	    // don't delete the file
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		ಗಣ_file_ಸಮಯ((char_u *)org, st_old.st_atime, st_old.st_mtime);
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}
	// If there is no backup file, remember that a (new) file was
	// created.
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ ಖಾಲಿಗಳು_fd;

	    ಆದರೆ (org == ಖಾಲಿ
		    || (ಖಾಲಿಗಳು_fd = mch_open(org,
				      O_CREAT | O_EXTRA | O_EXCL | O_NOFOLLOW,
					perm < 0 ? 0666 : (perm & 0777))) < 0)
	      emsg(_(e_patchmode_cant_touch_ಖಾಲಿಗಳು_original_file));
	    ಇಲ್ಲದಿದ್ದರೆ
	      close(ಖಾಲಿಗಳು_fd);
	}
	ಆದರೆ (org != ಖಾಲಿ)
	{
	    mch_setperm((char_u *)org, mch_getperm(fname) & 0777);
	    vim_free(org);
	}
    }

    // Remove the backup unless 'backup' option is set or there was a
    // conversion error.
    ಆದರೆ (!p_bk && backup != ಖಾಲಿ && !write_info.bw_conv_error
	    && mch_remove(backup) != 0)
	emsg(_(e_cant_delete_backup_file));

    ಹೋಗು nofail;

    // Finish up.  We get here either after failure or success.
fail:
    --no_wait_return;		// may wait for return now
nofail:

    // Done saving, we accept changed buffer warnings again
    buf->b_saving = false;

    vim_free(backup);
    ಆದರೆ (ಬಫರ್ != smallbuf)
	vim_free(ಬಫರ್);
    vim_free(fenc_tofree);
    vim_free(write_info.bw_conv_buf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
    ಆದರೆ (write_info.bw_iconv_fd != (iconv_t)-1)
    {
	iconv_close(write_info.bw_iconv_fd);
	write_info.bw_iconv_fd = (iconv_t)-1;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACL
    mch_free_acl(acl);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (errmsg != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ numlen = errnum != ಖಾಲಿ ? (ಪೂರ್ಣಾಂಕ)STRLEN(errnum) : 0;

	attr = HL_ATTR(HLF_E);	// set highlight for error messages
	ಸಂದೇಶ_ಕೂಡಿಸು_fname(buf,
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UNIX
		sfname
#ಇಲ್ಲದಿದ್ದರೆ
		fname
#ಆದರೆ_ಅಂತ್ಯ
		     );		// put file name in IObuff with quotes
	ಆದರೆ (STRLEN(IObuff) + STRLEN(errmsg) + numlen >= IOSIZE)
	    IObuff[IOSIZE - STRLEN(errmsg) - numlen - 1] = NUL;
	// If the error message has the form "is ...", put the error number in
	// front of the file name.
	ಆದರೆ (errnum != ಖಾಲಿ)
	{
	    STRMOVE(IObuff + numlen, IObuff);
	    mch_memmove(IObuff, errnum, (size_t)numlen);
	}
	STRCAT(IObuff, errmsg);
	emsg((ಅಕ್ಷರ *)IObuff);
	ಆದರೆ (errmsg_allocated)
	    vim_free(errmsg);

	retval = FAIL;
	ಆದರೆ (ಕೊನೆ == 0)
	{
	    ಸಂದೇಶ_puts_attr(_("\nWARNING: Original file may be lost or damaged\n"),
		    attr | ಸಂದೇಶ_HIST);
	    ಸಂದೇಶ_puts_attr(_("don't quit the editor until the file is successfully written!"),
		    attr | ಸಂದೇಶ_HIST);

	    // Update the timestamp to avoid an "overwrite changed file"
	    // prompt when writing again.
	    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st_old) >= 0)
	    {
		buf_store_ಸಮಯ(buf, &st_old, fname);
		buf->b_mtime_read = buf->b_mtime;
		buf->b_mtime_read_ns = buf->b_mtime_ns;
	    }
	}
    }
    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
    // When writing the whole file and 'undofile' is set, also write the undo
    // file.
    ಆದರೆ (retval == OK && write_undo_file)
    {
	char_u	    hash[UNDO_HASH_ಗಾತ್ರ];

	sha256_ಮುಕ್ತಾಯ(&sha_ctx, hash);
	u_write_undo(ಖಾಲಿ, FALSE, buf, hash);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (!should_abort(retval))
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (!got_int)
#ಆದರೆ_ಅಂತ್ಯ
    {
	aco_save_T	aco;

	curbuf->b_no_eol_lnum = 0;  // in case it was set by the previous read

	// Apply POST autocommands.
	// Careful: The autocommands may call buf_write() recursively!
	// Only do this when a window was found for "buf".
	aucmd_prepbuf(&aco, buf);
	ಆದರೆ (curbuf == buf)
	{
	    ಆದರೆ (append)
		apply_autocmds_exarg(EVENT_FILEAPPENDPOST, fname, fname,
							   FALSE, curbuf, eap);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filtering)
		apply_autocmds_exarg(EVENT_FILTERWRITEPOST, ಖಾಲಿ, fname,
							   FALSE, curbuf, eap);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reset_ಬದಲಾಯಿತು && ಸಂಪೂರ್ಣ)
		apply_autocmds_exarg(EVENT_BUFWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);
	    ಇಲ್ಲದಿದ್ದರೆ
		apply_autocmds_exarg(EVENT_FILEWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (aborting())	    // autocmds may abort script processing
	    retval = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    // Make sure marks will be written out to the viminfo file later, even when
    // the file is new.
    curbuf->b_marks_read = true;
#ಆದರೆ_ಅಂತ್ಯ

    got_int |= prev_got_int;

    ಹಿಂತಿರುಗಿ retval;
}
