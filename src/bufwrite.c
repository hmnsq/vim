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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
# include <utime.h>		// for struct utimbuf
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SMALLBUFSIZE	256	// size of emergency write buffer

/*
 * Structure to pass arguments from buf_write() to buf_write_bytes().
 */
ਬਣਤਰ bw_info
{
    ਪੂਰਨ		bw_fd;		// file descriptor
    char_u	*bw_buf;	// buffer with data to be written
    ਪੂਰਨ		bw_len;		// length of data
    ਪੂਰਨ		bw_flags;	// FIO_ flags
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    buf_T	*bw_ਬਫਰ;	// buffer being written
    ਪੂਰਨ		bw_ਅੰਤ;	// finish encrypting
#ਜੇ_ਅੰਤ
    char_u	bw_rest[CONV_RESTLEN]; // not converted bytes
    ਪੂਰਨ		bw_restlen;	// nr of bytes in bw_rest[]
    ਪੂਰਨ		bw_ਪਹਿਲਾ;	// first write call
    char_u	*bw_conv_buf;	// buffer for writing converted chars
    size_t	bw_conv_buflen; // size of bw_conv_buf
    ਪੂਰਨ		bw_conv_error;	// set for conversion error
    linenr_T	bw_conv_error_lnum;  // first line with error or zero
    linenr_T	bw_ਸ਼ੁਰੂ_lnum;	// line number at start of buffer
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
    iconv_t	bw_iconv_fd;	// descriptor for iconv() or -1
#ਜੇ_ਅੰਤ
};

/*
 * Convert a Unicode character to bytes.
 * Return TRUE for an error, FALSE when it's OK.
 */
    ਸਥਿਰ ਪੂਰਨ
ucs2bytes(
    ਬਿਨਾਨਿਸ਼ਾਨੀ	c,		// in: character
    char_u	**pp,		// in/out: pointer to result
    ਪੂਰਨ		flags)		// FIO_ flags
{
    char_u	*p = *pp;
    ਪੂਰਨ		error = FALSE;
    ਪੂਰਨ		cc;


    ਜੇ (flags & FIO_UCS4)
    {
	ਜੇ (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	    *p++ = (c >> 16);
	    *p++ = (c >> 24);
	}
	ਨਹੀਂਤਾਂ
	{
	    *p++ = (c >> 24);
	    *p++ = (c >> 16);
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (flags & (FIO_UCS2 | FIO_UTF16))
    {
	ਜੇ (c >= 0x10000)
	{
	    ਜੇ (flags & FIO_UTF16)
	    {
		// Make two words, ten bits of the character in each.  First
		// word is 0xd800 - 0xdbff, second one 0xdc00 - 0xdfff
		c -= 0x10000;
		ਜੇ (c >= 0x100000)
		    error = TRUE;
		cc = ((c >> 10) & 0x3ff) + 0xd800;
		ਜੇ (flags & FIO_ENDIAN_L)
		{
		    *p++ = cc;
		    *p++ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)cc >> 8);
		}
		ਨਹੀਂਤਾਂ
		{
		    *p++ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)cc >> 8);
		    *p++ = cc;
		}
		c = (c & 0x3ff) + 0xdc00;
	    }
	    ਨਹੀਂਤਾਂ
		error = TRUE;
	}
	ਜੇ (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	}
	ਨਹੀਂਤਾਂ
	{
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    ਨਹੀਂਤਾਂ    // Latin1
    {
	ਜੇ (c >= 0x100)
	{
	    error = TRUE;
	    *p++ = 0xBF;
	}
	ਨਹੀਂਤਾਂ
	    *p++ = c;
    }

    *pp = p;
    ਵਾਪਸ error;
}

/*
 * Call write() to write a number of bytes to the file.
 * Handles encryption and 'encoding' conversion.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
buf_write_ਬਾਈਟਾਂ(ਬਣਤਰ bw_info *ip)
{
    ਪੂਰਨ		wlen;
    char_u	*buf = ip->bw_buf;	// data to write
    ਪੂਰਨ		len = ip->bw_len;	// length of data
    ਪੂਰਨ		flags = ip->bw_flags;	// extra flags

    // Skip conversion when writing the crypt magic number or the BOM.
    ਜੇ (!(flags & FIO_NOCONVERT))
    {
	char_u		*p;
	ਬਿਨਾਨਿਸ਼ਾਨੀ	c;
	ਪੂਰਨ		n;

	ਜੇ (flags & FIO_UTF8)
	{
	    // Convert latin1 in the buffer to UTF-8 in the file.
	    p = ip->bw_conv_buf;	// translate to buffer
	    ਲਈ (wlen = 0; wlen < len; ++wlen)
		p += utf_char2bytes(buf[wlen], p);
	    buf = ip->bw_conv_buf;
	    len = (ਪੂਰਨ)(p - ip->bw_conv_buf);
	}
	ਨਹੀਂਤਾਂ ਜੇ (flags & (FIO_UCS4 | FIO_UTF16 | FIO_UCS2 | FIO_LATIN1))
	{
	    // Convert UTF-8 bytes in the buffer to UCS-2, UCS-4, UTF-16 or
	    // Latin1 chars in the file.
	    ਜੇ (flags & FIO_LATIN1)
		p = buf;	// translate in-place (can only get shorter)
	    ਨਹੀਂਤਾਂ
		p = ip->bw_conv_buf;	// translate to buffer
	    ਲਈ (wlen = 0; wlen < len; wlen += n)
	    {
		ਜੇ (wlen == 0 && ip->bw_restlen != 0)
		{
		    ਪੂਰਨ		l;

		    // Use remainder of previous call.  Append the start of
		    // buf[] to get a full sequence.  Might still be too
		    // short!
		    l = CONV_RESTLEN - ip->bw_restlen;
		    ਜੇ (l > len)
			l = len;
		    mch_memmove(ip->bw_rest + ip->bw_restlen, buf, (size_t)l);
		    n = utf_ptr2len_len(ip->bw_rest, ip->bw_restlen + l);
		    ਜੇ (n > ip->bw_restlen + len)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			ਜੇ (ip->bw_restlen + len > CONV_RESTLEN)
			    ਵਾਪਸ FAIL;
			ip->bw_restlen += len;
			ਰੋਕੋ;
		    }
		    ਜੇ (n > 1)
			c = utf_ptr2char(ip->bw_rest);
		    ਨਹੀਂਤਾਂ
			c = ip->bw_rest[0];
		    ਜੇ (n >= ip->bw_restlen)
		    {
			n -= ip->bw_restlen;
			ip->bw_restlen = 0;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ip->bw_restlen -= n;
			mch_memmove(ip->bw_rest, ip->bw_rest + n,
						      (size_t)ip->bw_restlen);
			n = 0;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    n = utf_ptr2len_len(buf + wlen, len - wlen);
		    ਜੇ (n > len - wlen)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			ਜੇ (len - wlen > CONV_RESTLEN)
			    ਵਾਪਸ FAIL;
			ip->bw_restlen = len - wlen;
			mch_memmove(ip->bw_rest, buf + wlen,
						      (size_t)ip->bw_restlen);
			ਰੋਕੋ;
		    }
		    ਜੇ (n > 1)
			c = utf_ptr2char(buf + wlen);
		    ਨਹੀਂਤਾਂ
			c = buf[wlen];
		}
		// Check that there is enough space
		ਜੇ (!(flags & FIO_LATIN1))
		{
		    size_t need = (flags & FIO_UCS4) ? 4 : 2;
		    ਜੇ ((flags & FIO_UTF16) && c >= 0x10000)
			need = 4;

		    ਜੇ ((size_t)(p - ip->bw_conv_buf) + need > ip->bw_conv_buflen)
			ਵਾਪਸ FAIL;
		}

		ਜੇ (ucs2bytes(c, &p, flags) && !ip->bw_conv_error)
		{
		    ip->bw_conv_error = TRUE;
		    ip->bw_conv_error_lnum = ip->bw_ਸ਼ੁਰੂ_lnum;
		}
		ਜੇ (c == NL)
		    ++ip->bw_ਸ਼ੁਰੂ_lnum;
	    }
	    ਜੇ (flags & FIO_LATIN1)
		len = (ਪੂਰਨ)(p - buf);
	    ਨਹੀਂਤਾਂ
	    {
		buf = ip->bw_conv_buf;
		len = (ਪੂਰਨ)(p - ip->bw_conv_buf);
	    }
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	ਨਹੀਂਤਾਂ ਜੇ (flags & FIO_CODEPAGE)
	{
	    // Convert UTF-8 or codepage to UCS-2 and then to MS-Windows
	    // codepage.
	    char_u	*from;
	    size_t	fromlen;
	    char_u	*ਮੰਜ਼ਿਲ;
	    ਪੂਰਨ		u8c;
	    BOOL	bad = FALSE;
	    ਪੂਰਨ		needed;

	    ਜੇ (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		from = buf;
		fromlen = len;
	    }

	    ਮੰਜ਼ਿਲ = ip->bw_conv_buf;
	    ਜੇ (enc_utf8)
	    {
		// Convert from UTF-8 to UCS-2, to the start of the buffer.
		// The buffer has been allocated to be big enough.
		ਜਦੋਂ (fromlen > 0)
		{
		    n = (ਪੂਰਨ)utf_ptr2len_len(from, (ਪੂਰਨ)fromlen);
		    ਜੇ (n > (ਪੂਰਨ)fromlen)	// incomplete byte sequence
			ਰੋਕੋ;
		    u8c = utf_ptr2char(from);
		    *ਮੰਜ਼ਿਲ++ = (u8c & 0xff);
		    *ਮੰਜ਼ਿਲ++ = (u8c >> 8);
		    fromlen -= n;
		    from += n;
		}

		// Copy remainder to ip->bw_rest[] to be used for the next
		// call.
		ਜੇ (fromlen > CONV_RESTLEN)
		{
		    // weird overlong sequence
		    ip->bw_conv_error = TRUE;
		    ਵਾਪਸ FAIL;
		}
		mch_memmove(ip->bw_rest, from, fromlen);
		ip->bw_restlen = (ਪੂਰਨ)fromlen;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Convert from enc_codepage to UCS-2, to the start of the
		// buffer.  The buffer has been allocated to be big enough.
		ip->bw_restlen = 0;
		needed = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(enc_codepage,
			     MB_ERR_INVALID_CHARS, (LPCSTR)from, (ਪੂਰਨ)fromlen,
								     ਨਿਰਾ, 0);
		ਜੇ (needed == 0)
		{
		    // When conversion fails there may be a trailing byte.
		    needed = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(enc_codepage,
			 MB_ERR_INVALID_CHARS, (LPCSTR)from, (ਪੂਰਨ)fromlen - 1,
								     ਨਿਰਾ, 0);
		    ਜੇ (needed == 0)
		    {
			// Conversion doesn't work.
			ip->bw_conv_error = TRUE;
			ਵਾਪਸ FAIL;
		    }
		    // Save the trailing byte for the next call.
		    ip->bw_rest[0] = from[fromlen - 1];
		    ip->bw_restlen = 1;
		}
		needed = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(enc_codepage, MB_ERR_INVALID_CHARS,
				(LPCSTR)from, (ਪੂਰਨ)(fromlen - ip->bw_restlen),
							  (LPWSTR)ਮੰਜ਼ਿਲ, needed);
		ਜੇ (needed == 0)
		{
		    // Safety check: Conversion doesn't work.
		    ip->bw_conv_error = TRUE;
		    ਵਾਪਸ FAIL;
		}
		ਮੰਜ਼ਿਲ += needed * 2;
	    }

	    fromlen = ਮੰਜ਼ਿਲ - ip->bw_conv_buf;
	    buf = ਮੰਜ਼ਿਲ;
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	    ਜੇ (FIO_GET_CP(flags) == CP_UTF8)
	    {
		// Convert from UCS-2 to UTF-8, using the remainder of the
		// conversion buffer.  Fails when out of space.
		ਲਈ (from = ip->bw_conv_buf; fromlen > 1; fromlen -= 2)
		{
		    u8c = *from++;
		    u8c += (*from++ << 8);
		    ਮੰਜ਼ਿਲ += utf_char2bytes(u8c, ਮੰਜ਼ਿਲ);
		    ਜੇ (ਮੰਜ਼ਿਲ + 6 >= ip->bw_conv_buf + ip->bw_conv_buflen)
		    {
			ip->bw_conv_error = TRUE;
			ਵਾਪਸ FAIL;
		    }
		}
		len = (ਪੂਰਨ)(ਮੰਜ਼ਿਲ - buf);
	    }
	    ਨਹੀਂਤਾਂ
# endif
	    {
		// Convert from UCS-2 to the codepage, using the remainder of
		// the conversion buffer.  If the conversion uses the default
		// character "0", the data doesn't fit in this encoding, so
		// fail.
		len = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(FIO_GET_CP(flags), 0,
			(LPCWSTR)ip->bw_conv_buf, (ਪੂਰਨ)fromlen / ਆਕਾਰ(WCHAR),
			(LPSTR)ਮੰਜ਼ਿਲ, (ਪੂਰਨ)(ip->bw_conv_buflen - fromlen), 0,
									&bad);
		ਜੇ (bad)
		{
		    ip->bw_conv_error = TRUE;
		    ਵਾਪਸ FAIL;
		}
	    }
	}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_CONVERT
	ਨਹੀਂਤਾਂ ਜੇ (flags & FIO_MACROMAN)
	{
	    // Convert UTF-8 or latin1 to Apple MacRoman.
	    char_u	*from;
	    size_t	fromlen;

	    ਜੇ (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		from = buf;
		fromlen = len;
	    }

	    ਜੇ (enc2macroman(from, fromlen,
			ip->bw_conv_buf, &len, ip->bw_conv_buflen,
			ip->bw_rest, &ip->bw_restlen) == FAIL)
	    {
		ip->bw_conv_error = TRUE;
		ਵਾਪਸ FAIL;
	    }
	    buf = ip->bw_conv_buf;
	}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
	ਜੇ (ip->bw_iconv_fd != (iconv_t)-1)
	{
	    ਅਚਲ ਅੱਖਰ	*from;
	    size_t	fromlen;
	    ਅੱਖਰ	*ਮੰਜ਼ਿਲ;
	    size_t	tolen;

	    // Convert with iconv().
	    ਜੇ (ip->bw_restlen > 0)
	    {
		ਅੱਖਰ *fp;

		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		fp = (ਅੱਖਰ *)ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(fp, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(fp + ip->bw_restlen, buf, (size_t)len);
		from = fp;
		tolen = ip->bw_conv_buflen - fromlen;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		from = (ਅਚਲ ਅੱਖਰ *)buf;
		fromlen = len;
		tolen = ip->bw_conv_buflen;
	    }
	    ਮੰਜ਼ਿਲ = (ਅੱਖਰ *)ip->bw_conv_buf;

	    ਜੇ (ip->bw_ਪਹਿਲਾ)
	    {
		size_t	save_len = tolen;

		// output the initial shift state sequence
		(ਖਾਲੀ)iconv(ip->bw_iconv_fd, ਨਿਰਾ, ਨਿਰਾ, &ਮੰਜ਼ਿਲ, &tolen);

		// There is a bug in iconv() on Linux (which appears to be
		// wide-spread) which sets "to" to NULL and messes up "tolen".
		ਜੇ (ਮੰਜ਼ਿਲ == ਨਿਰਾ)
		{
		    ਮੰਜ਼ਿਲ = (ਅੱਖਰ *)ip->bw_conv_buf;
		    tolen = save_len;
		}
		ip->bw_ਪਹਿਲਾ = FALSE;
	    }

	    // If iconv() has an error or there is not enough room, fail.
	    ਜੇ ((iconv(ip->bw_iconv_fd, (ਖਾਲੀ *)&from, &fromlen, &ਮੰਜ਼ਿਲ, &tolen)
			== (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						    || fromlen > CONV_RESTLEN)
	    {
		ip->bw_conv_error = TRUE;
		ਵਾਪਸ FAIL;
	    }

	    // copy remainder to ip->bw_rest[] to be used for the next call.
	    ਜੇ (fromlen > 0)
		mch_memmove(ip->bw_rest, (ਖਾਲੀ *)from, fromlen);
	    ip->bw_restlen = (ਪੂਰਨ)fromlen;

	    buf = ip->bw_conv_buf;
	    len = (ਪੂਰਨ)((char_u *)ਮੰਜ਼ਿਲ - ip->bw_conv_buf);
	}
#ਜੇ_ਅੰਤ
    }

    ਜੇ (ip->bw_fd < 0)
	// Only checking conversion, which is OK if we get here.
	ਵਾਪਸ OK;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    ਜੇ (flags & FIO_ENCRYPTED)
    {
	// Encrypt the data. Do it in-place if possible, otherwise use an
	// allocated buffer.
# ifdef CRYPT_NOT_INPLACE
	ਜੇ (crypt_ਚੱਲਦਾ_inplace(ip->bw_ਬਫਰ->b_cryptstate))
	{
# endif
	    crypt_encode_inplace(ip->bw_ਬਫਰ->b_cryptstate, buf, len,
								ip->bw_ਅੰਤ);
# ifdef CRYPT_NOT_INPLACE
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u *outbuf;

	    len = crypt_encode_alloc(curbuf->b_cryptstate, buf, len, &outbuf,
								ip->bw_ਅੰਤ);
	    ਜੇ (len == 0)
		ਵਾਪਸ OK;  // Crypt layer is buffering, will flush later.
	    wlen = write_eintr(ip->bw_fd, outbuf, len);
	    vim_free(outbuf);
	    ਵਾਪਸ (wlen < len) ? FAIL : OK;
	}
# endif
    }
#ਜੇ_ਅੰਤ

    wlen = write_eintr(ip->bw_fd, buf, len);
    ਵਾਪਸ (wlen < len) ? FAIL : OK;
}

/*
 * Check modification time of file, before writing to it.
 * The size isn't checked, because using a tool like "gzip" takes care of
 * using the same timestamp but can't set the size.
 */
    ਸਥਿਰ ਪੂਰਨ
check_mtime(buf_T *buf, stat_T *st)
{
    ਜੇ (buf->b_mtime_read != 0
		  && ਸਮਾਂ_differs(st, buf->b_mtime_read, buf->b_mtime_read_ns))
    {
	ਸੁਨੇਹਾ_scroll = TRUE;	    // don't overwrite messages here
	ਸੁਨੇਹਾ_silent = 0;		    // must give this prompt
	// don't use emsg() here, don't want to flush the buffers
	ਸੁਨੇਹਾ_attr(_("WARNING: The file has been changed since reading it!!!"),
						       HL_ATTR(HLF_E));
	ਜੇ (ask_yesno((char_u *)_("Do you really want to write to it"),
								 TRUE) == 'n')
	    ਵਾਪਸ FAIL;
	ਸੁਨੇਹਾ_scroll = FALSE;	    // always overwrite the file message now
    }
    ਵਾਪਸ OK;
}

/*
 * Generate a BOM in "buf[4]" for encoding "name".
 * Return the length of the BOM (zero when no BOM).
 */
    ਸਥਿਰ ਪੂਰਨ
make_bom(char_u *buf, char_u *name)
{
    ਪੂਰਨ		flags;
    char_u	*p;

    flags = get_fio_flags(name);

    // Can't put a BOM in a non-Unicode file.
    ਜੇ (flags == FIO_LATIN1 || flags == 0)
	ਵਾਪਸ 0;

    ਜੇ (flags == FIO_UTF8)	// UTF-8
    {
	buf[0] = 0xef;
	buf[1] = 0xbb;
	buf[2] = 0xbf;
	ਵਾਪਸ 3;
    }
    p = buf;
    (ਖਾਲੀ)ucs2bytes(0xfeff, &p, flags);
    ਵਾਪਸ (ਪੂਰਨ)(p - buf);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_file_ਸਮਾਂ(
    char_u  *fname,
    time_t  atime,	    // access time
    time_t  mtime)	    // modification time
{
# ਜੇ defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
    ਬਣਤਰ utimbuf  buf;

    buf.actime	= atime;
    buf.modtime	= mtime;
    (ਖਾਲੀ)utime((ਅੱਖਰ *)fname, &buf);
# ਨਹੀਂਤਾਂ
#  ਜੇ defined(HAVE_UTIMES)
    ਬਣਤਰ timeval  tvp[2];

    tvp[0].tv_sec   = atime;
    tvp[0].tv_usec  = 0;
    tvp[1].tv_sec   = mtime;
    tvp[1].tv_usec  = 0;
    (ਖਾਲੀ)utimes((ਅੱਖਰ *)fname, (ਅਚਲ ਬਣਤਰ timeval *)&tvp);
#  endif
# endif
}
#ਜੇ_ਅੰਤ // UNIX

    ਅੱਖਰ *
new_file_ਸੁਨੇਹਾ(ਖਾਲੀ)
{
    ਵਾਪਸ shortmess(SHM_NEW) ? _("[New]") : _("[New File]");
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
    ਪੂਰਨ
buf_write(
    buf_T	    *buf,
    char_u	    *fname,
    char_u	    *sfname,
    linenr_T	    ਸ਼ੁਰੂ,
    linenr_T	    ਸਿਰਾ,
    exarg_T	    *eap,		// for forced 'ff' and 'fenc', can be
					// NULL!
    ਪੂਰਨ		    append,		// append to the file
    ਪੂਰਨ		    forceit,
    ਪੂਰਨ		    reset_ਬਦਲਿਆ,
    ਪੂਰਨ		    filtering)
{
    ਪੂਰਨ		    fd;
    char_u	    *backup = ਨਿਰਾ;
    ਪੂਰਨ		    backup_copy = FALSE; // copy the original file?
    ਪੂਰਨ		    dobackup;
    char_u	    *ffname;
    char_u	    *wfname = ਨਿਰਾ;	// name of file to write to
    char_u	    *s;
    char_u	    *ptr;
    char_u	    c;
    ਪੂਰਨ		    len;
    linenr_T	    lnum;
    ਲੰਮਾ	    nchars;
    char_u	    *errmsg = ਨਿਰਾ;
    ਪੂਰਨ		    errmsg_allocated = FALSE;
    char_u	    *errnum = ਨਿਰਾ;
    char_u	    *ਬਫਰ;
    char_u	    smallbuf[SMALLBUFSIZE];
    char_u	    *backup_ext;
    ਪੂਰਨ		    bufsize;
    ਲੰਮਾ	    perm;		    // file permissions
    ਪੂਰਨ		    retval = OK;
    ਪੂਰਨ		    newfile = FALSE;	    // TRUE if file doesn't exist yet
    ਪੂਰਨ		    ਸੁਨੇਹਾ_save = ਸੁਨੇਹਾ_scroll;
    ਪੂਰਨ		    overwriting;	    // TRUE if writing over original
    ਪੂਰਨ		    no_eol = FALSE;	    // no end-of-line written
    ਪੂਰਨ		    device = FALSE;	    // writing to a device
    stat_T	    st_old;
    ਪੂਰਨ		    prev_got_int = got_int;
    ਪੂਰਨ		    checking_conversion;
    ਪੂਰਨ		    file_readonly = FALSE;  // overwritten file is read-only
#ਜੇ defined(UNIX)			    // XXX fix me sometime?
    ਪੂਰਨ		    made_writable = FALSE;  // 'w' bit has been set
#ਜੇ_ਅੰਤ
					// writing everything
    ਪੂਰਨ		    ਸਾਰਾ = (ਸ਼ੁਰੂ == 1 && ਸਿਰਾ == buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
    linenr_T	    old_ਸਤਰ_ਗਿਣਤੀ = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਪੂਰਨ		    attr;
    ਪੂਰਨ		    fileformat;
    ਪੂਰਨ		    write_bin;
    ਬਣਤਰ bw_info  write_info;		// info for buf_write_bytes()
    ਪੂਰਨ		    converted = FALSE;
    ਪੂਰਨ		    notconverted = FALSE;
    char_u	    *fenc;		// effective 'fileencoding'
    char_u	    *fenc_tofree = ਨਿਰਾ; // allocated "fenc"
    ਪੂਰਨ		    wb_flags = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
    vim_acl_T	    acl = ਨਿਰਾ;		// ACL copied from original file to
					// backup or new file
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
    ਪੂਰਨ		    write_undo_file = FALSE;
    context_sha256_T sha_ctx;
#ਜੇ_ਅੰਤ
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    bkc = get_bkc_flags(buf);
    pos_T	    orig_ਸ਼ੁਰੂ = buf->b_op_ਸ਼ੁਰੂ;
    pos_T	    orig_ਸਿਰਾ = buf->b_op_ਸਿਰਾ;

    ਜੇ (fname == ਨਿਰਾ || *fname == NUL)	// safety check
	ਵਾਪਸ FAIL;
    ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)
    {
	// This can happen during startup when there is a stray "w" in the
	// vimrc file.
	emsg(_(e_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ));
	ਵਾਪਸ FAIL;
    }

    // Disallow writing from .exrc and .vimrc in current directory for
    // security reasons.
    ਜੇ (check_secure())
	ਵਾਪਸ FAIL;

    // Avoid a crash for a long name.
    ਜੇ (STRLEN(fname) >= MAXPATHL)
    {
	emsg(_(e_name_too_long));
	ਵਾਪਸ FAIL;
    }

    // must init bw_conv_buf and bw_iconv_fd before jumping to "fail"
    write_info.bw_conv_buf = ਨਿਰਾ;
    write_info.bw_conv_error = FALSE;
    write_info.bw_conv_error_lnum = 0;
    write_info.bw_restlen = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
    write_info.bw_iconv_fd = (iconv_t)-1;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    write_info.bw_ਬਫਰ = buf;
    write_info.bw_ਅੰਤ = FALSE;
#ਜੇ_ਅੰਤ

    // After writing a file changedtick changes but we don't want to display
    // the line.
    ex_no_reprint = TRUE;

    // If there is no file name yet, use the one for the written file.
    // BF_NOTEDITED is set to reflect this (in case the write fails).
    // Don't do this when the write is for a filter command.
    // Don't do this when appending.
    // Only do this when 'cpoptions' contains the 'F' flag.
    ਜੇ (buf->b_ffname == ਨਿਰਾ
	    && reset_ਬਦਲਿਆ
	    && ਸਾਰਾ
	    && buf == curbuf
	    && !bt_nofilename(buf)
	    && !filtering
	    && (!append || vim_strchr(p_cpo, CPO_FNAMEAPP) != ਨਿਰਾ)
	    && vim_strchr(p_cpo, CPO_FNAMEW) != ਨਿਰਾ)
    {
	ਜੇ (ਸੈੱਟ_rw_fname(fname, sfname) == FAIL)
	    ਵਾਪਸ FAIL;
	buf = curbuf;	    // just in case autocmds made "buf" invalid
    }

    ਜੇ (sfname == ਨਿਰਾ)
	sfname = fname;
    // For Unix: Use the short file name whenever possible.
    // Avoids problems with networks and when directory names are changed.
    // Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
    // another directory, which we don't detect
    ffname = fname;			    // remember full fname
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    fname = sfname;
#ਜੇ_ਅੰਤ

    ਜੇ (buf->b_ffname != ਨਿਰਾ && fnamecmp(ffname, buf->b_ffname) == 0)
	overwriting = TRUE;
    ਨਹੀਂਤਾਂ
	overwriting = FALSE;

    ਜੇ (exiting)
	settmode(TMODE_COOK);	    // when exiting allow typeahead now

    ++no_wait_return;		    // don't wait for return yet

    // Set '[ and '] marks to the lines to be written.
    buf->b_op_ਸ਼ੁਰੂ.lnum = ਸ਼ੁਰੂ;
    buf->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ = 0;
    buf->b_op_ਸਿਰਾ.lnum = ਸਿਰਾ;
    buf->b_op_ਸਿਰਾ.ਖਾਨਾ = 0;

    {
	aco_save_T	aco;
	ਪੂਰਨ		buf_ffname = FALSE;
	ਪੂਰਨ		buf_sfname = FALSE;
	ਪੂਰਨ		buf_fname_f = FALSE;
	ਪੂਰਨ		buf_fname_s = FALSE;
	ਪੂਰਨ		did_cmd = FALSE;
	ਪੂਰਨ		nofile_err = FALSE;
	ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_memline = (buf->b_ml.ml_mfp == ਨਿਰਾ);
	bufref_T	bufref;

	// Apply PRE autocommands.
	// Set curbuf to the buffer to be written.
	// Careful: The autocommands may call buf_write() recursively!
	ਜੇ (ffname == buf->b_ffname)
	    buf_ffname = TRUE;
	ਜੇ (sfname == buf->b_sfname)
	    buf_sfname = TRUE;
	ਜੇ (fname == buf->b_ffname)
	    buf_fname_f = TRUE;
	ਜੇ (fname == buf->b_sfname)
	    buf_fname_s = TRUE;

	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, buf);
	ਜੇ (curbuf != buf)
	{
	    // Could not find a window for "buf".  Doing more might cause
	    // problems, better bail out.
	    ਵਾਪਸ FAIL;
	}

	ਸੈੱਟ_bufref(&bufref, buf);

	ਜੇ (append)
	{
	    ਜੇ (!(did_cmd = apply_autocmds_exarg(EVENT_FILEAPPENDCMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		ਜੇ (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		ਨਹੀਂਤਾਂ
		    apply_autocmds_exarg(EVENT_FILEAPPENDPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (filtering)
	{
	    apply_autocmds_exarg(EVENT_FILTERWRITEPRE,
					    ਨਿਰਾ, sfname, FALSE, curbuf, eap);
	}
	ਨਹੀਂਤਾਂ ਜੇ (reset_ਬਦਲਿਆ && ਸਾਰਾ)
	{
	    ਪੂਰਨ was_ਬਦਲਿਆ = curbuf_Is_ਬਦਲਿਆ();

	    did_cmd = apply_autocmds_exarg(EVENT_BUFWRITECMD,
					  sfname, sfname, FALSE, curbuf, eap);
	    ਜੇ (did_cmd)
	    {
		ਜੇ (was_ਬਦਲਿਆ && !curbuf_Is_ਬਦਲਿਆ())
		{
		    // Written everything correctly and BufWriteCmd has reset
		    // 'modified': Correct the undo information so that an
		    // undo now sets 'modified'.
		    u_unchanged(curbuf);
		    u_update_save_nr(curbuf);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		ਨਹੀਂਤਾਂ
		    apply_autocmds_exarg(EVENT_BUFWRITEPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (!(did_cmd = apply_autocmds_exarg(EVENT_FILEWRITECMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		ਜੇ (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		ਨਹੀਂਤਾਂ
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
	ਜੇ (!bufref_valid(&bufref))
	    buf = ਨਿਰਾ;
	ਜੇ (buf == ਨਿਰਾ || (buf->b_ml.ml_mfp == ਨਿਰਾ && !ਖਾਲੀ_ਥਾਵਾਂ_memline)
				       || did_cmd || nofile_err
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
				       || aborting()
#ਜੇ_ਅੰਤ
				       )
	{
	    ਜੇ (buf != ਨਿਰਾ && (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	    {
		// restore the original '[ and '] positions
		buf->b_op_ਸ਼ੁਰੂ = orig_ਸ਼ੁਰੂ;
		buf->b_op_ਸਿਰਾ = orig_ਸਿਰਾ;
	    }

	    --no_wait_return;
	    ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_save;
	    ਜੇ (nofile_err)
		semsg(_(e_no_matching_autocommands_for_buftype_str_ਬਫਰ),
							       curbuf->b_p_bt);

	    ਜੇ (nofile_err
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    || aborting()
#ਜੇ_ਅੰਤ
		    )
		// An aborting error, interrupt or exception in the
		// autocommands.
		ਵਾਪਸ FAIL;
	    ਜੇ (did_cmd)
	    {
		ਜੇ (buf == ਨਿਰਾ)
		    // The buffer was deleted.  We assume it was written
		    // (can't retry anyway).
		    ਵਾਪਸ OK;
		ਜੇ (overwriting)
		{
		    // Assume the buffer was written, update the timestamp.
		    ml_timestamp(buf);
		    ਜੇ (append)
			buf->b_flags &= ~BF_NEW;
		    ਨਹੀਂਤਾਂ
			buf->b_flags &= ~BF_WRITE_ਨਕਾਬ;
		}
		ਜੇ (reset_ਬਦਲਿਆ && buf->b_ਬਦਲਿਆ && !append
			&& (overwriting || vim_strchr(p_cpo, CPO_PLUS) != ਨਿਰਾ))
		    // Buffer still changed, the autocommands didn't work
		    // properly.
		    ਵਾਪਸ FAIL;
		ਵਾਪਸ OK;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਜੇ (!aborting())
#ਜੇ_ਅੰਤ
		emsg(_(e_autocommands_deleted_or_unloaded_ਬਫਰ_ਮੰਜ਼ਿਲ_be_written));
	    ਵਾਪਸ FAIL;
	}

	// The autocommands may have changed the number of lines in the file.
	// When writing the whole file, adjust the end.
	// When writing part of the file, assume that the autocommands only
	// changed the number of lines that are to be written (tricky!).
	ਜੇ (buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ != old_ਸਤਰ_ਗਿਣਤੀ)
	{
	    ਜੇ (ਸਾਰਾ)						// write all
		ਸਿਰਾ = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    ਨਹੀਂਤਾਂ ਜੇ (buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ > old_ਸਤਰ_ਗਿਣਤੀ)	// more lines
		ਸਿਰਾ += buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - old_ਸਤਰ_ਗਿਣਤੀ;
	    ਨਹੀਂਤਾਂ						// less lines
	    {
		ਸਿਰਾ -= old_ਸਤਰ_ਗਿਣਤੀ - buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
		ਜੇ (ਸਿਰਾ < ਸ਼ੁਰੂ)
		{
		    --no_wait_return;
		    ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_save;
		    emsg(_(e_autocommands_ਬਦਲਿਆ_ਸੰਖਿਆ_ਦਾ_ਸਤਰਾਂ_ਵਿੱਚ_unexpected_way));
		    ਵਾਪਸ FAIL;
		}
	    }
	}

	// The autocommands may have changed the name of the buffer, which may
	// be kept in fname, ffname and sfname.  A ":cd" may also have dropped
	// the short name, then b_fname is the full name.
	ਜੇ (buf_ffname)
	    ffname = buf->b_ffname;
	ਜੇ (buf_sfname)
	    sfname = buf->b_fname;
	ਜੇ (buf_fname_f)
	    fname = buf->b_ffname;
	ਜੇ (buf_fname_s)
	    fname = buf->b_fname;
    }

    ਜੇ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	// restore the original '[ and '] positions
	buf->b_op_ਸ਼ੁਰੂ = orig_ਸ਼ੁਰੂ;
	buf->b_op_ਸਿਰਾ = orig_ਸਿਰਾ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    ਜੇ (netbeans_ਸਰਗਰਮ() && is_Netbeans_ਬਫਰ(buf))
    {
	ਜੇ (ਸਾਰਾ)
	{
	    // b_changed can be 0 after an undo, but we still need to write
	    // the buffer to NetBeans.
	    ਜੇ (buf->b_ਬਦਲਿਆ || isNetbeansModified(buf))
	    {
		--no_wait_return;		// may wait for return now
		ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_save;
		netbeans_save_ਬਫਰ(buf);	// no error checking...
		ਵਾਪਸ retval;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		errnum = (char_u *)"E656: ";
		errmsg = (char_u *)_(e_netbeans_disallows_writes_ਦਾ_unmodified_buffers);
		ਬਫਰ = ਨਿਰਾ;
		ਜਾਓ fail;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    errnum = (char_u *)"E657: ";
	    errmsg = (char_u *)_(e_partial_writes_disallowed_for_netbeans_buffers);
	    ਬਫਰ = ਨਿਰਾ;
	    ਜਾਓ fail;
	}
    }
#ਜੇ_ਅੰਤ

    ਜੇ (shortmess(SHM_OVER) && !exiting)
	ਸੁਨੇਹਾ_scroll = FALSE;	    // overwrite previous file message
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_scroll = TRUE;	    // don't overwrite previous file message
    ਜੇ (!filtering)
	filemess(buf,
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UNIX
		sfname,
#ਨਹੀਂਤਾਂ
		fname,
#ਜੇ_ਅੰਤ
		    (char_u *)"", 0);	// show that we are busy
    ਸੁਨੇਹਾ_scroll = FALSE;		    // always overwrite the file message now

    ਬਫਰ = alloc(WRITEBUFSIZE);
    ਜੇ (ਬਫਰ == ਨਿਰਾ)		    // can't allocate big buffer, use small
				    // one (to be able to write when out of
				    // memory)
    {
	ਬਫਰ = smallbuf;
	bufsize = SMALLBUFSIZE;
    }
    ਨਹੀਂਤਾਂ
	bufsize = WRITEBUFSIZE;

    // Get information about original file (if there is one).
#ਜੇ defined(UNIX)
    st_old.st_dev = 0;
    st_old.st_ino = 0;
    perm = -1;
    ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st_old) < 0)
	newfile = TRUE;
    ਨਹੀਂਤਾਂ
    {
	perm = st_old.st_ਬਹੁਲਕ;
	ਜੇ (!S_ISREG(st_old.st_ਬਹੁਲਕ))		// not a file
	{
	    ਜੇ (S_ISDIR(st_old.st_ਬਹੁਲਕ))
	    {
		errnum = (char_u *)"E502: ";
		errmsg = (char_u *)_(e_is_a_directory);
		ਜਾਓ fail;
	    }
	    ਜੇ (mch_nodetype(fname) != ਗੰਢ_WRITABLE)
	    {
		errnum = (char_u *)"E503: ";
		errmsg = (char_u *)_(e_is_not_file_or_writable_device);
		ਜਾਓ fail;
	    }
	    // It's a device of some kind (or a fifo) which we can write to
	    // but for which we can't make a backup.
	    device = TRUE;
	    newfile = TRUE;
	    perm = -1;
	}
    }
#ਨਹੀਂਤਾਂ // !UNIX
    // Check for a writable device name.
    c = mch_nodetype(fname);
    ਜੇ (c == ਗੰਢ_OTHER)
    {
	errnum = (char_u *)"E503: ";
	errmsg = (char_u *)_(e_is_not_file_or_writable_device);
	ਜਾਓ fail;
    }
    ਜੇ (c == ਗੰਢ_WRITABLE)
    {
# ਜੇ defined(MSWIN)
	// MS-Windows allows opening a device, but we will probably get stuck
	// trying to write to it.
	ਜੇ (!p_odev)
	{
	    errnum = (char_u *)"E796: ";
	    errmsg = (char_u *)_(e_writing_ਮੰਜ਼ਿਲ_device_disabled_with_opendevice_option);
	    ਜਾਓ fail;
	}
# endif
	device = TRUE;
	newfile = TRUE;
	perm = -1;
    }
    ਨਹੀਂਤਾਂ
    {
	perm = mch_getperm(fname);
	ਜੇ (perm < 0)
	    newfile = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (mch_isdir(fname))
	{
	    errnum = (char_u *)"E502: ";
	    errmsg = (char_u *)_(e_is_a_directory);
	    ਜਾਓ fail;
	}
	ਜੇ (overwriting)
	    (ਖਾਲੀ)mch_stat((ਅੱਖਰ *)fname, &st_old);
    }
#ਜੇ_ਅੰਤ // !UNIX

    ਜੇ (!device && !newfile)
    {
	// Check if the file is really writable (when renaming the file to
	// make a backup we won't discover it later).
	file_readonly = check_file_readonly(fname, (ਪੂਰਨ)perm);

	ਜੇ (!forceit && file_readonly)
	{
	    ਜੇ (vim_strchr(p_cpo, CPO_FWRITE) != ਨਿਰਾ)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_ਵਿੱਚ_cpoptions);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		errnum = (char_u *)"E505: ";
		errmsg = (char_u *)_(e_is_read_only_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_override);
	    }
	    ਜਾਓ fail;
	}

	// Check if the timestamp hasn't changed since reading the file.
	ਜੇ (overwriting)
	{
	    retval = check_mtime(buf, &st_old);
	    ਜੇ (retval == FAIL)
		ਜਾਓ fail;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    ਜੇ (!newfile)
	acl = mch_get_acl(fname);
#ਜੇ_ਅੰਤ

    // If 'backupskip' is not empty, don't make a backup for some files.
    dobackup = (p_wb || p_bk || *p_pm != NUL);
    ਜੇ (dobackup && *p_bsk != NUL && ਮੇਲ_file_list(p_bsk, sfname, ffname))
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
    ਜੇ (!(append && *p_pm == NUL) && !filtering && perm >= 0 && dobackup)
    {
#ਜੇ defined(UNIX) || defined(MSWIN)
	stat_T	    st;
#ਜੇ_ਅੰਤ

	ਜੇ ((bkc & BKC_YES) || append)	// "yes"
	    backup_copy = TRUE;
#ਜੇ defined(UNIX) || defined(MSWIN)
	ਨਹੀਂਤਾਂ ਜੇ ((bkc & BKC_AUTO))	// "auto"
	{
# ifdef UNIX
	    // Don't rename the file when:
	    // - it's a hard link
	    // - it's a symbolic link
	    // - we don't have write permission in the directory
	    // - we can't set the owner/group of the new file
	    ਜੇ (st_old.st_nlink > 1
		    || mch_lstat((ਅੱਖਰ *)fname, &st) < 0
		    || st.st_dev != st_old.st_dev
		    || st.st_ino != st_old.st_ino
#  ifndef HAVE_FCHOWN
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid
#  endif
		    )
		backup_copy = TRUE;
	    ਨਹੀਂਤਾਂ
# ਨਹੀਂਤਾਂ
#  ifdef MSWIN
	    // On NTFS file systems hard links are possible.
	    ਜੇ (mch_is_linked(fname))
		backup_copy = TRUE;
	    ਨਹੀਂਤਾਂ
#  endif
# endif
	    {
		size_t	dirlen;
		char_u	tmp_fname[MAXPATHL];
		ਪੂਰਨ	i;

		// Check if we can create a file and set the owner/group/mode to
		// the ones from the original file.
		// First find a file name that doesn't exist yet.  Use the
		// conventional hidden swap file name so that this short-lived
		// probe is covered by standard Vim ignore patterns.
		dirlen = (size_t)(gettail(fname) - fname);
		vim_strncpy(tmp_fname, fname, dirlen);
		fd = -1;
		ਲਈ (i = 4913; ; i += 123)
		{
		    vim_snprintf((ਅੱਖਰ *)tmp_fname + dirlen,
			ਆਕਾਰ(tmp_fname) - dirlen, ".%d.swp", i);
		    ਜੇ (mch_lstat((ਅੱਖਰ *)tmp_fname, &st) < 0)
		    {
			fd = mch_open((ਅੱਖਰ *)tmp_fname,
				    O_CREAT|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
			ਜੇ (fd < 0 && errno == EEXIST)
			    // If the same file name is created by another
			    // process between lstat() and open(), find another
			    // name.
			    ਜਾਰੀ;
			ਰੋਕੋ;
		    }
		}
		ਜੇ (fd < 0)	// can't write in directory
		    backup_copy = TRUE;
		ਨਹੀਂਤਾਂ
		{
# ifdef UNIX
#  ifdef HAVE_FCHOWN
		    vim_ignored = fchown(fd, st_old.st_uid, st_old.st_gid);
#  endif
#  ifdef HAVE_FCHMOD
		    (ਖਾਲੀ)mch_fsetperm(fd, perm);
#  ਨਹੀਂਤਾਂ
		    (ਖਾਲੀ)mch_setperm(tmp_fname, perm);
#  endif
		    ਜੇ (mch_stat((ਅੱਖਰ *)tmp_fname, &st) < 0
			    || st.st_uid != st_old.st_uid
			    || st.st_gid != st_old.st_gid
			    || (ਲੰਮਾ)st.st_ਬਹੁਲਕ != perm)
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
			ਪੂਰਨ try;

			ਲਈ (try = 0; try < 10; ++try)
			{
			    ਜੇ (mch_lstat((ਅੱਖਰ *)tmp_fname, &st) < 0)
				ਰੋਕੋ;
			    ui_delay(50L, TRUE);  // wait 50 msec
			    mch_remove(tmp_fname);
			}
		    }
# endif
		}
	    }
	}

	// Break symlinks and/or hardlinks if we've been asked to.
	ਜੇ ((bkc & BKC_BREAKSYMLINK) || (bkc & BKC_BREAKHARDLINK))
	{
# ifdef UNIX
	    ਪੂਰਨ	lstat_res;

	    lstat_res = mch_lstat((ਅੱਖਰ *)fname, &st);

	    // Symlinks.
	    ਜੇ ((bkc & BKC_BREAKSYMLINK)
		    && lstat_res == 0
		    && st.st_ino != st_old.st_ino)
		backup_copy = FALSE;

	    // Hardlinks.
	    ਜੇ ((bkc & BKC_BREAKHARDLINK)
		    && st_old.st_nlink > 1
		    && (lstat_res != 0 || st.st_ino == st_old.st_ino))
		backup_copy = FALSE;
# ਨਹੀਂਤਾਂ
#  ਜੇ defined(MSWIN)
	    // Symlinks.
	    ਜੇ ((bkc & BKC_BREAKSYMLINK) && mch_is_symbolic_link(fname))
		backup_copy = FALSE;

	    // Hardlinks.
	    ਜੇ ((bkc & BKC_BREAKHARDLINK) && mch_is_hard_link(fname))
		backup_copy = FALSE;
#  endif
# endif
	}

#ਜੇ_ਅੰਤ

	// make sure we have a valid backup extension to use
	ਜੇ (*p_bex == NUL)
	    backup_ext = (char_u *)".bak";
	ਨਹੀਂਤਾਂ
	    backup_ext = p_bex;

	ਜੇ (backup_copy
		&& (fd = mch_open((ਅੱਖਰ *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
	{
	    ਪੂਰਨ		bfd;
	    char_u	*copybuf;
	    ਪੂਰਨ		some_error = FALSE;
	    stat_T	st_new;
	    char_u	*dirp;
	    char_u	*rootname;
#ਜੇ defined(UNIX)
	    ਪੂਰਨ		did_ਸੈੱਟ_shortname;
	    mode_t	umask_save;
#ਜੇ_ਅੰਤ

	    copybuf = alloc(WRITEBUFSIZE + 1);
	    ਜੇ (copybuf == ਨਿਰਾ)
	    {
		some_error = TRUE;	    // out of memory
		ਜਾਓ nobackup;
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
	    ਜਦੋਂ (*dirp)
	    {
		char_u	*p ਅਣਵਰਤਿਆ;
		ਪੂਰਨ	copybuf_len ਅਣਵਰਤਿਆ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		st_new.st_ino = 0;
		st_new.st_dev = 0;
		st_new.st_gid = 0;
#ਜੇ_ਅੰਤ

		// Isolate one directory name, using an entry in 'bdir'.
		copybuf_len = copy_option_part(&dirp, copybuf, WRITEBUFSIZE, ",");

#ਜੇ defined(UNIX) || defined(MSWIN)
		p = copybuf + copybuf_len;
		ਜੇ (after_pathsep(copybuf, p) && p[-1] == p[-2])
		    // Ends with '//', use full path
		    ਜੇ ((p = make_percent_swname(copybuf, p, fname)) != ਨਿਰਾ)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#ਜੇ_ਅੰਤ
		rootname = get_file_ਵਿੱਚ_dir(fname, copybuf);
		ਜੇ (rootname == ਨਿਰਾ)
		{
		    some_error = TRUE;	    // out of memory
		    ਜਾਓ nobackup;
		}

#ਜੇ defined(UNIX)
		did_ਸੈੱਟ_shortname = FALSE;
#ਜੇ_ਅੰਤ

		// May try twice if 'shortname' not set.
		ਲਈ (;;)
		{
		    // Make the backup file name.
		    ਜੇ (backup == ਨਿਰਾ)
			backup = buf_modname((buf->b_p_sn || buf->b_shortname),
						 rootname, backup_ext, FALSE);
		    ਜੇ (backup == ਨਿਰਾ)
		    {
			vim_free(rootname);
			some_error = TRUE;		// out of memory
			ਜਾਓ nobackup;
		    }

		    // Check if backup file already exists.
		    ਜੇ (mch_stat((ਅੱਖਰ *)backup, &st_new) >= 0)
		    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
			// Check if backup file is same as original file.
			// May happen when modname() gave the same file back.
			// E.g. silly link, or file name-length reached.
			// If we don't check here, we either ruin the file
			// when copying or erase it after writing. jw.
			ਜੇ (st_new.st_dev == st_old.st_dev
					    && st_new.st_ino == st_old.st_ino)
			{
			    VIM_CLEAR(backup);	// no backup file to delete
			    // may try again with 'shortname' set
			    ਜੇ (!(buf->b_shortname || buf->b_p_sn))
			    {
				buf->b_shortname = true;
				did_ਸੈੱਟ_shortname = TRUE;
				ਜਾਰੀ;
			    }
				// setting shortname didn't help
			    ਜੇ (did_ਸੈੱਟ_shortname)
				buf->b_shortname = false;
			    ਰੋਕੋ;
			}
#ਜੇ_ਅੰਤ

			// If we are not going to keep the backup file, don't
			// delete an existing one, try to use another name.
			// Change one character, just before the extension.
			ਜੇ (!p_bk)
			{
			    char_u	*wp;

			    wp = backup + STRLEN(backup) - 1
							 - STRLEN(backup_ext);
			    ਜੇ (wp < backup)	// empty file name ???
				wp = backup;
			    *wp = 'z';
			    ਜਦੋਂ (*wp > 'a'
				    && mch_stat((ਅੱਖਰ *)backup, &st_new) >= 0)
				--*wp;
			    // They all exist??? Must be something wrong.
			    ਜੇ (*wp == 'a')
				VIM_CLEAR(backup);
			}
		    }
		    ਰੋਕੋ;
		}
		vim_free(rootname);

		// Try to create the backup file
		ਜੇ (backup != ਨਿਰਾ)
		{
		    // remove old backup, if present
		    mch_remove(backup);
		    // Open with O_EXCL to avoid the file being created while
		    // we were sleeping (symlink hacker attack?). Reset umask
		    // if possible to avoid mch_setperm() below.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		    umask_save = umask(0);
#ਜੇ_ਅੰਤ
		    bfd = mch_open((ਅੱਖਰ *)backup,
				O_WRONLY|O_CREAT|O_EXTRA|O_EXCL|O_NOFOLLOW,
								 perm & 0777);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		    (ਖਾਲੀ)umask(umask_save);
#ਜੇ_ਅੰਤ
		    ਜੇ (bfd < 0)
			VIM_CLEAR(backup);
		    ਨਹੀਂਤਾਂ
		    {
			// Set file protection same as original file, but
			// strip s-bit.  Only needed if umask() wasn't used
			// above.
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UNIX
			(ਖਾਲੀ)mch_setperm(backup, perm & 0777);
#ਨਹੀਂਤਾਂ
			// Try to set the group of the backup same as the
			// original file. If this fails, set the protection
			// bits for the group same as the protection bits for
			// others.
			ਜੇ (st_new.st_gid != st_old.st_gid
# ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
				&& fchown(bfd, (uid_t)-1, st_old.st_gid) != 0
# endif
						)
			    mch_setperm(backup,
					  (perm & 0707) | ((perm & 07) << 3));
# ਜੇ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
# endif
# ifdef FEAT_XATTR
			mch_copy_xattr(fname, backup);
# endif
#ਜੇ_ਅੰਤ

			// copy the file.
			write_info.bw_fd = bfd;
			write_info.bw_buf = copybuf;
			write_info.bw_flags = FIO_NOCONVERT;
			ਜਦੋਂ ((write_info.bw_len = read_eintr(fd, copybuf,
							    WRITEBUFSIZE)) > 0)
			{
			    ਜੇ (buf_write_ਬਾਈਟਾਂ(&write_info) == FAIL)
			    {
				errmsg = (char_u *)_(e_cant_write_ਮੰਜ਼ਿਲ_backup_file_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_override);
				ਰੋਕੋ;
			    }
			    ui_breakcheck();
			    ਜੇ (got_int)
			    {
				errmsg = (char_u *)_(e_interrupted);
				ਰੋਕੋ;
			    }
			}

			ਜੇ (close(bfd) < 0 && errmsg == ਨਿਰਾ)
			    errmsg = (char_u *)_(e_close_error_for_backup_file_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_write_anyway);
			ਜੇ (write_info.bw_len < 0)
			    errmsg = (char_u *)_(e_cant_read_file_for_backup_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_write_anyway);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
			ਸੈੱਟ_file_ਸਮਾਂ(backup, st_old.st_atime, st_old.st_mtime);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
			mch_ਸੈੱਟ_acl(backup, acl);
#ਜੇ_ਅੰਤ
#ਜੇ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XATTR
			mch_copy_xattr(fname, backup);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
			(ਖਾਲੀ)mch_copy_file_attribute(fname, backup);
#ਜੇ_ਅੰਤ
			ਰੋਕੋ;
		    }
		}
	    }
    nobackup:
	    close(fd);		// ignore errors for closing read file
	    vim_free(copybuf);

	    ਜੇ (backup == ਨਿਰਾ && errmsg == ਨਿਰਾ)
		errmsg = (char_u *)_(e_cannot_create_backup_file_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_write_anyway);
	    // ignore errors when forceit is TRUE
	    ਜੇ ((some_error || errmsg != ਨਿਰਾ) && !forceit)
	    {
		retval = FAIL;
		ਜਾਓ fail;
	    }
	    errmsg = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u	*dirp;
	    char_u	*p;
	    char_u	*rootname;

	    // Make a backup by renaming the original file.

	    // If 'cpoptions' includes the "W" flag, we don't want to
	    // overwrite a read-only file.  But rename may be possible
	    // anyway, thus we need an extra check here.
	    ਜੇ (file_readonly && vim_strchr(p_cpo, CPO_FWRITE) != ਨਿਰਾ)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_ਵਿੱਚ_cpoptions);
		ਜਾਓ fail;
	    }

	    // Form the backup file name - change path/fo.o.h to
	    // path/fo.o.h.bak Try all directories in 'backupdir', first one
	    // that works is used.
	    dirp = p_bdir;
	    ਜਦੋਂ (*dirp)
	    {
		ਪੂਰਨ IObufflen ਅਣਵਰਤਿਆ;

		// Isolate one directory name and make the backup file name.
		IObufflen = copy_option_part(&dirp, IObuff, IOSIZE, ",");

#ਜੇ defined(UNIX) || defined(MSWIN)
		p = IObuff + IObufflen;
		ਜੇ (after_pathsep(IObuff, p) && p[-1] == p[-2])
		    // path ends with '//', use full path
		    ਜੇ ((p = make_percent_swname(IObuff, p, fname)) != ਨਿਰਾ)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#ਜੇ_ਅੰਤ
		ਜੇ (backup == ਨਿਰਾ)
		{
		    rootname = get_file_ਵਿੱਚ_dir(fname, IObuff);
		    ਜੇ (rootname == ਨਿਰਾ)
			backup = ਨਿਰਾ;
		    ਨਹੀਂਤਾਂ
		    {
			backup = buf_modname(
				(buf->b_p_sn || buf->b_shortname),
						rootname, backup_ext, FALSE);
			vim_free(rootname);
		    }
		}

		ਜੇ (backup != ਨਿਰਾ)
		{
		    // If we are not going to keep the backup file, don't
		    // delete an existing one, try to use another name.
		    // Change one character, just before the extension.
		    ਜੇ (!p_bk && mch_getperm(backup) >= 0)
		    {
			p = backup + STRLEN(backup) - 1 - STRLEN(backup_ext);
			ਜੇ (p < backup)	// empty file name ???
			    p = backup;
			*p = 'z';
			ਜਦੋਂ (*p > 'a' && mch_getperm(backup) >= 0)
			    --*p;
			// They all exist??? Must be something wrong!
			ਜੇ (*p == 'a')
			    VIM_CLEAR(backup);
		    }
		}
		ਜੇ (backup != ਨਿਰਾ)
		{
		    // Delete any existing backup and move the current version
		    // to the backup.	For safety, we don't remove the backup
		    // until the write has finished successfully. And if the
		    // 'backup' option is set, leave it around.

		    // If the renaming of the original file to the backup file
		    // works, quit here.
		    ਜੇ (vim_rename(fname, backup) == 0)
			ਰੋਕੋ;

		    VIM_CLEAR(backup);   // don't do the rename below
		}
	    }
	    ਜੇ (backup == ਨਿਰਾ && !forceit)
	    {
		errmsg = (char_u *)_(e_cant_make_backup_file_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_write_anyway);
		ਜਾਓ fail;
	    }
	}
    }

#ਜੇ defined(UNIX)
    // When using ":w!" and the file was read-only: make it writable
    ਜੇ (forceit && perm >= 0 && !(perm & 0200) && st_old.st_uid == getuid()
				     && vim_strchr(p_cpo, CPO_FWRITE) == ਨਿਰਾ)
    {
	perm |= 0200;
	(ਖਾਲੀ)mch_setperm(fname, perm);
	made_writable = TRUE;
    }
#ਜੇ_ਅੰਤ

    // When using ":w!" and writing to the current file, 'readonly' makes no
    // sense, reset it, unless 'Z' appears in 'cpoptions'.
    ਜੇ (forceit && overwriting && vim_strchr(p_cpo, CPO_KEEPRO) == ਨਿਰਾ)
    {
	buf->b_p_ro = FALSE;
	need_maketitle = TRUE;	    // set window title later
	ਸਥਿਤੀ_redraw_all();	    // redraw status lines later
    }

    ਜੇ (ਸਿਰਾ > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਸਿਰਾ = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਜੇ (buf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
	ਸ਼ੁਰੂ = ਸਿਰਾ + 1;

    // If the original file is being overwritten, there is a small chance that
    // we crash in the middle of writing. Therefore the file is preserved now.
    // This makes all block numbers positive so that recovery does not need
    // the original file.
    // Don't do this if there is a backup file and we are exiting.
    ਜੇ (reset_ਬਦਲਿਆ && !newfile && overwriting
					      && !(exiting && backup != ਨਿਰਾ))
    {
	ml_preserve(buf, FALSE);
	ਜੇ (got_int)
	{
	    errmsg = (char_u *)_(e_interrupted);
	    ਜਾਓ restore_backup;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    vms_remove_ਸੰਸਕਰਣ(fname); // remove version
#ਜੇ_ਅੰਤ
    // Default: write the file directly.  May write to a temp file for
    // multi-byte conversion.
    wfname = fname;

    // Check for forced 'fileencoding' from "++opt=val" argument.
    ਜੇ (eap != ਨਿਰਾ && eap->force_enc != 0)
    {
	fenc = eap->cmd + eap->force_enc;
	fenc = enc_canonize(fenc);
	fenc_tofree = fenc;
    }
    ਨਹੀਂਤਾਂ
	fenc = buf->b_p_fenc;

    // Check if the file needs to be converted.
    converted = need_conversion(fenc);

    // Check if UTF-8 to UCS-2/4 or Latin1 conversion needs to be done.  Or
    // Latin1 to Unicode conversion.  This is handled in buf_write_bytes().
    // Prepare the flags for it and allocate bw_conv_buf when needed.
    ਜੇ (converted && (enc_utf8 || STRCMP(p_enc, "latin1") == 0))
    {
	wb_flags = get_fio_flags(fenc);
	ਜੇ (wb_flags & (FIO_UCS2 | FIO_UCS4 | FIO_UTF16 | FIO_UTF8))
	{
	    // overallocate a bit, in case we read incomplete multi-byte chars
	    ਪੂਰਨ ਮਾਪ = bufsize + CONV_RESTLEN;
	    // Need to allocate a buffer to translate into.
	    ਜੇ (wb_flags & (FIO_UCS2 | FIO_UTF16 | FIO_UTF8))
		write_info.bw_conv_buflen = ਮਾਪ * 2;
	    ਨਹੀਂਤਾਂ // FIO_UCS4
		write_info.bw_conv_buflen = ਮਾਪ * 4;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    ਜੇ (write_info.bw_conv_buf == ਨਿਰਾ)
		ਸਿਰਾ = 0;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਜੇ (converted && wb_flags == 0 && (wb_flags = get_win_fio_flags(fenc)) != 0)
    {
	// Convert UTF-8 -> UCS-2 and UCS-2 -> DBCS.  Worst-case * 4:
	write_info.bw_conv_buflen = bufsize * 4;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	ਜੇ (write_info.bw_conv_buf == ਨਿਰਾ)
	    ਸਿਰਾ = 0;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_CONVERT
    ਜੇ (converted && wb_flags == 0 && (wb_flags = get_mac_fio_flags(fenc)) != 0)
    {
	write_info.bw_conv_buflen = bufsize * 3;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	ਜੇ (write_info.bw_conv_buf == ਨਿਰਾ)
	    ਸਿਰਾ = 0;
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL) || defined(USE_ICONV)
    ਜੇ (converted && wb_flags == 0)
    {
# ifdef USE_ICONV
	// Use iconv() conversion when conversion is needed and it's not done
	// internally.
	write_info.bw_iconv_fd = (iconv_t)my_iconv_open(fenc,
					enc_utf8 ? (char_u *)"utf-8" : p_enc);
	ਜੇ (write_info.bw_iconv_fd != (iconv_t)-1)
	{
	    // We're going to use iconv(), allocate a buffer to convert in.
	    write_info.bw_conv_buflen = bufsize * ICONV_MULT;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    ਜੇ (write_info.bw_conv_buf == ਨਿਰਾ)
		ਸਿਰਾ = 0;
	    write_info.bw_ਪਹਿਲਾ = TRUE;
	}
#  ifdef FEAT_EVAL
	ਨਹੀਂਤਾਂ
#  endif
# endif

# ifdef FEAT_EVAL
	    // When the file needs to be converted with 'charconvert' after
	    // writing, write to a temp file instead and let the conversion
	    // overwrite the original file.
	    ਜੇ (*p_ccv != NUL)
	    {
		wfname = vim_tempname('w', FALSE);
		ਜੇ (wfname == ਨਿਰਾ)	// Can't write without a tempfile!
		{
		    errmsg = (char_u *)_(e_cant_ਲੱਭੋ_ਆਰਜ਼ੀ_file_for_writing);
		    ਜਾਓ restore_backup;
		}
	    }
# endif
    }
#ਜੇ_ਅੰਤ
    ਜੇ (converted && wb_flags == 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
	    && write_info.bw_iconv_fd == (iconv_t)-1
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    && wfname == fname
#ਜੇ_ਅੰਤ
	    )
    {
	ਜੇ (!forceit)
	{
	    errmsg = (char_u *)_(e_cannot_convert_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_write_without_conversion);
	    ਜਾਓ restore_backup;
	}
	notconverted = TRUE;
    }

    // If conversion is taking place, we may first pretend to write and check
    // for conversion errors.  Then loop again to write for real.
    // When not doing conversion this writes for real right away.
    ਲਈ (checking_conversion = TRUE; ; checking_conversion = FALSE)
    {
	// There is no need to check conversion when:
	// - there is no conversion
	// - we make a backup file, that can be restored in case of conversion
	//   failure.
	ਜੇ (!converted || dobackup)
	    checking_conversion = FALSE;

	ਜੇ (checking_conversion)
	{
	    // Make sure we don't write anything.
	    fd = -1;
	    write_info.bw_fd = fd;
	}
	ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FTRUNCATE
# define TRUNC_ON_OPEN 0
#ਨਹੀਂਤਾਂ
# define TRUNC_ON_OPEN O_TRUNC
#ਜੇ_ਅੰਤ
	    // Open the file "wfname" for writing.
	    // We may try to open the file twice: If we can't write to the file
	    // and forceit is TRUE we delete the existing file and try to
	    // create a new one. If this still fails we may have lost the
	    // original file!  (this may happen when the user reached his
	    // quotum for number of files).
	    // Appending will fail if the file does not exist and forceit is
	    // FALSE.
	    ਜਦੋਂ ((fd = mch_open((ਅੱਖਰ *)wfname, O_WRONLY | O_EXTRA | (append
				? (forceit ? (O_APPEND | O_CREAT) : O_APPEND)
				: (O_CREAT | TRUNC_ON_OPEN))
				, perm < 0 ? 0666 : (perm & 0777))) < 0)
	    {
		// A forced write will try to create a new file if the old one
		// is still readonly. This may also happen when the directory
		// is read-only. In that case the mch_remove() will fail.
		ਜੇ (errmsg == ਨਿਰਾ)
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		    stat_T	st;

		    // Don't delete the file when it's a hard or symbolic link.
		    ਜੇ ((!newfile && st_old.st_nlink > 1)
			    || (mch_lstat((ਅੱਖਰ *)fname, &st) == 0
				&& (st.st_dev != st_old.st_dev
				    || st.st_ino != st_old.st_ino)))
			errmsg =
			      (char_u *)_(e_cant_open_linked_file_for_writing);
		    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    {
			errmsg = (char_u *)_(e_cant_open_file_for_writing);
			ਜੇ (forceit && vim_strchr(p_cpo, CPO_FWRITE) == ਨਿਰਾ
								  && perm >= 0)
			{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
			    // we write to the file, thus it should be marked
			    // writable after all
			    ਜੇ (!(perm & 0200))
				made_writable = TRUE;
			    perm |= 0200;
			    ਜੇ (st_old.st_uid != getuid()
						  || st_old.st_gid != getgid())
				perm &= 0777;
#ਜੇ_ਅੰਤ
			    ਜੇ (!append)  // don't remove when appending
				mch_remove(wfname);
			    ਜਾਰੀ;
			}
		    }
		}

restore_backup:
		{
		    stat_T	st;

		    // If we failed to open the file, we don't need a backup.
		    // Throw it away.  If we moved or removed the original file
		    // try to put the backup in its place.
		    ਜੇ (backup != ਨਿਰਾ && wfname == fname)
		    {
			ਜੇ (backup_copy)
			{
			    // There is a small chance that we removed the
			    // original, try to move the copy in its place.
			    // This may not work if the vim_rename() fails.
			    // In that case we leave the copy around.

			    // If file does not exist, put the copy in its
			    //ਰੱਖੋ
			    ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st) < 0)
				vim_rename(backup, fname);
			    // if original file does exist throw away the copy
			    ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st) >= 0)
				mch_remove(backup);
			}
			ਨਹੀਂਤਾਂ
			{
			    // try to put the original file back
			    vim_rename(backup, fname);
			}
		    }

		    // if original file no longer exists give an extra warning
		    ਜੇ (!newfile && mch_stat((ਅੱਖਰ *)fname, &st) < 0)
			ਸਿਰਾ = 0;
		}

		ਜੇ (wfname != fname)
		    vim_free(wfname);
		ਜਾਓ fail;
	    }
	    write_info.bw_fd = fd;

#ਜੇ defined(UNIX)
	    {
		stat_T	st;

		// Double check we are writing the intended file before making
		// any changes.
		ਜੇ (overwriting
			&& (!dobackup || backup_copy)
			&& fname == wfname
			&& perm >= 0
			&& mch_fstat(fd, &st) == 0
			&& st.st_ino != st_old.st_ino)
		{
		    close(fd);
		    errmsg = (char_u *)_(e_file_ਬਦਲਿਆ_while_writing);
		    ਜਾਓ fail;
		}
	    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FTRUNCATE
	    ਜੇ (!append)
		vim_ignored = ftruncate(fd, (off_t)0);
#ਜੇ_ਅੰਤ

#ਜੇ defined(MSWIN)
	    ਜੇ (backup != ਨਿਰਾ && overwriting && !append)
		(ਖਾਲੀ)mch_copy_file_attribute(backup, wfname);

	    ਜੇ (!overwriting && !append)
	    {
		ਜੇ (buf->b_ffname != ਨਿਰਾ)
		    (ਖਾਲੀ)mch_copy_file_attribute(buf->b_ffname, wfname);
		// Should copy resource fork
	    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	    ਜੇ (*buf->b_p_ਕੁੰਜੀ != NUL && !filtering)
	    {
		char_u		*ਸਿਰਲੇਖ;
		ਪੂਰਨ		ਸਿਰਲੇਖ_len;

		buf->b_cryptstate = crypt_create_for_writing(
						      crypt_get_ਵਿਧੀ_nr(buf),
					   buf->b_p_ਕੁੰਜੀ, &ਸਿਰਲੇਖ, &ਸਿਰਲੇਖ_len);
		ਜੇ (buf->b_cryptstate == ਨਿਰਾ || ਸਿਰਲੇਖ == ਨਿਰਾ)
		    ਸਿਰਾ = 0;
		ਨਹੀਂਤਾਂ
		{
		    // Write magic number, so that Vim knows how this file is
		    // encrypted when reading it back.
		    write_info.bw_buf = ਸਿਰਲੇਖ;
		    write_info.bw_len = ਸਿਰਲੇਖ_len;
		    write_info.bw_flags = FIO_NOCONVERT;
		    ਜੇ (buf_write_ਬਾਈਟਾਂ(&write_info) == FAIL)
			ਸਿਰਾ = 0;
		    wb_flags |= FIO_ENCRYPTED;
		    vim_free(ਸਿਰਲੇਖ);
		}
	    }
#ਜੇ_ਅੰਤ
	}
	errmsg = ਨਿਰਾ;

	write_info.bw_buf = ਬਫਰ;
	nchars = 0;

	// use "++bin", "++nobin" or 'binary'
	ਜੇ (eap != ਨਿਰਾ && eap->force_bin != 0)
	    write_bin = (eap->force_bin == FORCE_BIN);
	ਨਹੀਂਤਾਂ
	    write_bin = buf->b_p_bin;

	// The BOM is written just after the encryption magic number.
	// Skip it when appending and the file already existed, the BOM only
	// makes sense at the start of the file.
	ਜੇ (buf->b_p_bomb && !write_bin && (!append || perm < 0))
	{
	    write_info.bw_len = make_bom(ਬਫਰ, fenc);
	    ਜੇ (write_info.bw_len > 0)
	    {
		// don't convert, do encryption
		write_info.bw_flags = FIO_NOCONVERT | wb_flags;
		ਜੇ (buf_write_ਬਾਈਟਾਂ(&write_info) == FAIL)
		    ਸਿਰਾ = 0;
		ਨਹੀਂਤਾਂ
		    nchars += write_info.bw_len;
	    }
	}
	write_info.bw_ਸ਼ੁਰੂ_lnum = ਸ਼ੁਰੂ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
	write_undo_file = (buf->b_p_udf
			    && overwriting
			    && !append
			    && !filtering
# ifdef CRYPT_NOT_INPLACE
			    // writing undo file requires
			    // crypt_encode_inplace()
			    && (buf->b_cryptstate == ਨਿਰਾ
				|| crypt_ਚੱਲਦਾ_inplace(buf->b_cryptstate))
# endif
			    && reset_ਬਦਲਿਆ
			    && !checking_conversion);
# ifdef CRYPT_NOT_INPLACE
	// remove undo file if encrypting it is not possible
	ਜੇ (buf->b_p_udf
		&& overwriting
		&& !append
		&& !filtering
		&& !checking_conversion
		&& buf->b_cryptstate != ਨਿਰਾ
		&& !crypt_ਚੱਲਦਾ_inplace(buf->b_cryptstate))
	    u_undofile_reset_and_delete(buf);
# endif
	ਜੇ (write_undo_file)
	    // Prepare for computing the hash value of the text.
	    sha256_ਸ਼ੁਰੂ(&sha_ctx);
#ਜੇ_ਅੰਤ

	write_info.bw_len = bufsize;
	write_info.bw_flags = wb_flags;
	fileformat = get_fileformat_force(buf, eap);
	s = ਬਫਰ;
	len = 0;
	ਲਈ (lnum = ਸ਼ੁਰੂ; lnum <= ਸਿਰਾ; ++lnum)
	{
	    // The next while loop is done once for each character written.
	    // Keep it fast!
	    ptr = ml_get_buf(buf, lnum, FALSE) - 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
	    ਜੇ (write_undo_file)
		sha256_update(&sha_ctx, ptr + 1,
					      (UINT32_T)(STRLEN(ptr + 1) + 1));
#ਜੇ_ਅੰਤ
	    ਜਦੋਂ ((c = *++ptr) != NUL)
	    {
		ਜੇ (c == NL)
		    *s = NUL;		// replace newlines with NULs
		ਨਹੀਂਤਾਂ ਜੇ (c == CAR && fileformat == EOL_MAC)
		    *s = NL;		// Mac: replace CRs with NLs
		ਨਹੀਂਤਾਂ
		    *s = c;
		++s;
		ਜੇ (++len != bufsize)
		    ਜਾਰੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
		ਜੇ (write_info.bw_fd > 0 && lnum == ਸਿਰਾ
			&& (write_info.bw_flags & FIO_ENCRYPTED)
			&& *buf->b_p_ਕੁੰਜੀ != NUL && !filtering
			&& *ptr == NUL)
		    write_info.bw_ਅੰਤ = TRUE;
#ਜੇ_ਅੰਤ
		ਜੇ (buf_write_ਬਾਈਟਾਂ(&write_info) == FAIL)
		{
		    ਸਿਰਾ = 0;		// write error: break loop
		    ਰੋਕੋ;
		}
		nchars += bufsize;
		s = ਬਫਰ;
		len = 0;
		write_info.bw_ਸ਼ੁਰੂ_lnum = lnum;
	    }
	    // write failed or last line has no EOL: stop here
	    ਜੇ (ਸਿਰਾ == 0
		    || (lnum == ਸਿਰਾ
			&& (write_bin || !buf->b_p_fixeol)
			&& ((write_bin && lnum == buf->b_no_eol_lnum)
			    || (lnum == buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
							   && !buf->b_p_eol))))
	    {
		++lnum;			// written the line, count it
		no_eol = TRUE;
		ਰੋਕੋ;
	    }
	    ਜੇ (fileformat == EOL_UNIX)
		*s++ = NL;
	    ਨਹੀਂਤਾਂ
	    {
		*s++ = CAR;		    // EOL_MAC or EOL_DOS: write CR
		ਜੇ (fileformat == EOL_DOS)  // write CR-NL
		{
		    ਜੇ (++len == bufsize)
		    {
			ਜੇ (buf_write_ਬਾਈਟਾਂ(&write_info) == FAIL)
			{
			    ਸਿਰਾ = 0;	// write error: break loop
			    ਰੋਕੋ;
			}
			nchars += bufsize;
			s = ਬਫਰ;
			len = 0;
		    }
		    *s++ = NL;
		}
	    }
	    ਜੇ (++len == bufsize && ਸਿਰਾ)
	    {
		ਜੇ (buf_write_ਬਾਈਟਾਂ(&write_info) == FAIL)
		{
		    ਸਿਰਾ = 0;		// write error: break loop
		    ਰੋਕੋ;
		}
		nchars += bufsize;
		s = ਬਫਰ;
		len = 0;

		ui_breakcheck();
		ਜੇ (got_int)
		{
		    ਸਿਰਾ = 0;		// Interrupted, break loop
		    ਰੋਕੋ;
		}
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
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
	    ਜੇ (buf->b_fab_rfm == FAB$C_VFC
		    || ((buf->b_fab_rat & (FAB$M_FTN | FAB$M_CR)) != 0))
	    {
		ਪੂਰਨ b2write;

		buf->b_fab_mrs = (buf->b_fab_mrs == 0
			? MIN(4096, bufsize)
			: MIN(buf->b_fab_mrs, bufsize));

		b2write = len;
		ਜਦੋਂ (b2write > 0)
		{
		    write_info.bw_len = MIN(b2write, buf->b_fab_mrs);
		    ਜੇ (buf_write_ਬਾਈਟਾਂ(&write_info) == FAIL)
		    {
			ਸਿਰਾ = 0;
			ਰੋਕੋ;
		    }
		    b2write -= MIN(b2write, buf->b_fab_mrs);
		}
		write_info.bw_len = bufsize;
		nchars += len;
		s = ਬਫਰ;
		len = 0;
	    }
#ਜੇ_ਅੰਤ
	}
	ਜੇ (len > 0 && ਸਿਰਾ > 0)
	{
	    write_info.bw_len = len;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	    ਜੇ (write_info.bw_fd > 0 && lnum >= ਸਿਰਾ
		    && (write_info.bw_flags & FIO_ENCRYPTED)
		    && *buf->b_p_ਕੁੰਜੀ != NUL && !filtering)
		write_info.bw_ਅੰਤ = TRUE;
#ਜੇ_ਅੰਤ
	    ਜੇ (buf_write_ਬਾਈਟਾਂ(&write_info) == FAIL)
		ਸਿਰਾ = 0;		    // write error
	    nchars += len;
	}

	ਜੇ (!buf->b_p_fixeol && buf->b_p_eof)
	{
	    // write trailing CTRL-Z
	    (ਖਾਲੀ)write_eintr(write_info.bw_fd, "\x1a", 1);
	    nchars++;
	}

	// Stop when writing done or an error was encountered.
	ਜੇ (!checking_conversion || ਸਿਰਾ == 0)
	    ਰੋਕੋ;

	// If no error happened until now, writing should be ok, so loop to
	// really write the buffer.
    }

    // If we started writing, finish writing. Also when an error was
    // encountered.
    ਜੇ (!checking_conversion)
    {
#ਜੇ defined(UNIX) && defined(HAVE_FSYNC)
	// On many journaling file systems there is a bug that causes both the
	// original and the backup file to be lost when halting the system
	// right after writing the file.  That's because only the meta-data is
	// journalled.  Syncing the file slows down the system, but assures it
	// has been written to disk and we don't lose it.
	// For a device do try the fsync() but don't complain if it does not
	// work (could be a pipe).
	// If the 'fsync' option is FALSE, don't fsync().  Useful for laptops.
	ਜੇ ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && vim_fsync(fd) != 0
		&& !device)
	{
	    errmsg = (char_u *)_(e_fsync_failed);
	    ਸਿਰਾ = 0;
	}
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_SELINUX) || defined(HAVE_SMACK) || defined(FEAT_XATTR)
	// Probably need to set the security context.
	ਜੇ (!backup_copy)
	{
# ਜੇ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
	    mch_copy_sec(backup, wfname);
# endif
# ifdef FEAT_XATTR
	    mch_copy_xattr(backup, wfname);
# endif
	}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	// When creating a new file, set its owner/group to that of the
	// original file.  Get the new device and inode number.
	ਜੇ (backup != ਨਿਰਾ && !backup_copy)
	{
# ifdef HAVE_FCHOWN
	    stat_T	st;

	    // Don't change the owner when it's already OK, some systems remove
	    // permission or ACL stuff.
	    ਜੇ (mch_stat((ਅੱਖਰ *)wfname, &st) < 0
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid)
	    {
		// changing owner might not be possible
		vim_ignored = fchown(fd, st_old.st_uid, -1);
		// if changing group fails clear the group permissions
		ਜੇ (fchown(fd, -1, st_old.st_gid) == -1 && perm > 0)
		    perm &= ~070;
	    }
# endif
	    buf_setino(buf);
	}
	ਨਹੀਂਤਾਂ ਜੇ (!buf->b_dev_valid)
	    // Set the inode when creating a new file.
	    buf_setino(buf);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	ਜੇ (made_writable)
	    perm &= ~0200;	// reset 'w' bit for security reasons
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCHMOD
	// set permission of new file same as old file
	ਜੇ (perm >= 0)
	    (ਖਾਲੀ)mch_fsetperm(fd, perm);
#ਜੇ_ਅੰਤ
	ਜੇ (close(fd) != 0)
	{
	    errmsg = (char_u *)_(e_close_failed);
	    ਸਿਰਾ = 0;
	}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_FCHMOD
	// set permission of new file same as old file
	ਜੇ (perm >= 0)
	    (ਖਾਲੀ)mch_setperm(wfname, perm);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
	// Probably need to set the ACL before changing the user (can't set the
	// ACL on a file the user doesn't own).
	// On Solaris, with ZFS and the aclmode property set to "discard" (the
	// default), chmod() discards all part of a file's ACL that don't
	// represent the mode of the file.  It's non-trivial for us to discover
	// whether we're in that situation, so we simply always re-set the ACL.
# ifndef HAVE_SOLARIS_ZFS_ACL
	ਜੇ (!backup_copy)
# endif
	    mch_ਸੈੱਟ_acl(wfname, acl);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	ਜੇ (buf->b_cryptstate != ਨਿਰਾ)
	{
	    crypt_free_state(buf->b_cryptstate);
	    buf->b_cryptstate = ਨਿਰਾ;
	}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
	ਜੇ (wfname != fname)
	{
	    // The file was written to a temp file, now it needs to be
	    // converted with 'charconvert' to (overwrite) the output file.
	    ਜੇ (ਸਿਰਾ != 0)
	    {
		ਜੇ (eval_charconvert(enc_utf8 ? (char_u *)"utf-8" : p_enc,
						  fenc, wfname, fname) == FAIL)
		{
		    write_info.bw_conv_error = TRUE;
		    ਸਿਰਾ = 0;
		}
	    }
	    mch_remove(wfname);
	    vim_free(wfname);
	}
#ਜੇ_ਅੰਤ
    }

    ਜੇ (ਸਿਰਾ == 0)
    {
	// Error encountered.
	ਜੇ (errmsg == ਨਿਰਾ)
	{
	    ਜੇ (write_info.bw_conv_error)
	    {
		ਜੇ (write_info.bw_conv_error_lnum == 0)
		    errmsg = (char_u *)_(e_write_error_conversion_failed_make_fenc_ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_override);
		ਨਹੀਂਤਾਂ
		{
		    errmsg_allocated = TRUE;
		    errmsg = alloc(300);
		    ਜੇ (errmsg == ਨਿਰਾ)
			ਜਾਓ fail;
		    vim_snprintf((ਅੱਖਰ *)errmsg, 300, _(e_write_error_conversion_failed_ਵਿੱਚ_ਸਤਰ_nr_make_fenc_ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_override),
					 (ਲੰਮਾ)write_info.bw_conv_error_lnum);
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (got_int)
		errmsg = (char_u *)_(e_interrupted);
	    ਨਹੀਂਤਾਂ
		errmsg = (char_u *)_(e_write_error_file_system_ਭਰਿਆ);
	}

	// If we have a backup file, try to put it in place of the new file,
	// because the new file is probably corrupt.  This avoids losing the
	// original file when trying to make a backup when writing the file a
	// second time.
	// When "backup_copy" is set we need to copy the backup over the new
	// file.  Otherwise rename the backup file.
	// If this is OK, don't give the extra warning message.
	ਜੇ (backup != ਨਿਰਾ)
	{
	    ਜੇ (backup_copy)
	    {
		// This may take a while, if we were interrupted let the user
		// know we got the message.
		ਜੇ (got_int)
		{
		    ਸੁਨੇਹਾ(_(e_interrupted));
		    ਬਾਹਰਲਾ_flush();
		}
		ਜੇ ((fd = mch_open((ਅੱਖਰ *)backup, O_RDONLY | O_EXTRA, 0)) >= 0)
		{
		    ਜੇ ((write_info.bw_fd = mch_open((ਅੱਖਰ *)fname,
				    O_WRONLY | O_CREAT | O_TRUNC | O_EXTRA,
							   perm & 0777)) >= 0)
		    {
			// copy the file.
			write_info.bw_buf = smallbuf;
			write_info.bw_flags = FIO_NOCONVERT;
			ਜਦੋਂ ((write_info.bw_len = read_eintr(fd, smallbuf,
						      SMALLBUFSIZE)) > 0)
			    ਜੇ (buf_write_ਬਾਈਟਾਂ(&write_info) == FAIL)
				ਰੋਕੋ;

			ਜੇ (close(write_info.bw_fd) >= 0
						   && write_info.bw_len == 0)
			    ਸਿਰਾ = 1;		// success
		    }
		    close(fd);	// ignore errors for closing read file
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (vim_rename(backup, fname) == 0)
		    ਸਿਰਾ = 1;
	    }
	}
	ਜਾਓ fail;
    }

    lnum -= ਸ਼ੁਰੂ;	    // compute number of written lines
    --no_wait_return;	    // may wait for return now

#ਜੇ !(defined(UNIX) || defined(VMS))
    fname = sfname;	    // use shortname now, for the messages
#ਜੇ_ਅੰਤ
    ਜੇ (!filtering)
    {
	ਸੁਨੇਹਾ_ਜੋੜੋ_fname(buf, fname);	// put fname in IObuff with quotes
	c = FALSE;
	ਜੇ (write_info.bw_conv_error)
	{
	    STRCAT(IObuff, _(" CONVERSION ERROR"));
	    c = TRUE;
	    ਜੇ (write_info.bw_conv_error_lnum != 0)
		vim_snprintf_ਜੋੜੋ((ਅੱਖਰ *)IObuff, IOSIZE, _(" in line %ld;"),
			(ਲੰਮਾ)write_info.bw_conv_error_lnum);
	}
	ਨਹੀਂਤਾਂ ਜੇ (notconverted)
	{
	    STRCAT(IObuff, _("[NOT converted]"));
	    c = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (converted)
	{
	    STRCAT(IObuff, _("[converted]"));
	    c = TRUE;
	}
	ਜੇ (device)
	{
	    STRCAT(IObuff, _("[Device]"));
	    c = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (newfile)
	{
	    STRCAT(IObuff, new_file_ਸੁਨੇਹਾ());
	    c = TRUE;
	}
	ਜੇ (no_eol)
	{
	    ਸੁਨੇਹਾ_ਜੋੜੋ_eol();
	    c = TRUE;
	}
	// may add [unix/dos/mac]
	ਜੇ (ਸੁਨੇਹਾ_ਜੋੜੋ_fileformat(fileformat))
	    c = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	ਜੇ (wb_flags & FIO_ENCRYPTED)
	{
	    crypt_append_ਸੁਨੇਹਾ(buf);
	    c = TRUE;
	}
#ਜੇ_ਅੰਤ
	ਸੁਨੇਹਾ_ਜੋੜੋ_ਸਤਰਾਂ(c, (ਲੰਮਾ)lnum, nchars);	// add line/char count
	ਜੇ (!shortmess(SHM_WRITE))
	{
	    ਜੇ (append)
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [a]") : _(" appended"));
	    ਨਹੀਂਤਾਂ
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [w]") : _(" written"));
	}

	ਸੈੱਟ_keep_ਸੁਨੇਹਾ((char_u *)ਸੁਨੇਹਾ_trunc_attr((ਅੱਖਰ *)IObuff, FALSE, 0), 0);
    }

    // When written everything correctly: reset 'modified'.  Unless not
    // writing to the original file and '+' is not in 'cpoptions'.
    ਜੇ (reset_ਬਦਲਿਆ && ਸਾਰਾ && !append
	    && !write_info.bw_conv_error
	    && (overwriting || vim_strchr(p_cpo, CPO_PLUS) != ਨਿਰਾ))
    {
	unchanged(buf, TRUE, FALSE);
	// b:changedtick may be incremented in unchanged() but that should not
	// trigger a TextChanged event.
	ਜੇ (buf->b_ਆਖਰੀ_changedtick + 1 == CHANGEDTICK(buf))
	    buf->b_ਆਖਰੀ_changedtick = CHANGEDTICK(buf);
	u_unchanged(buf);
	u_update_save_nr(buf);
    }

    // If written to the current file, update the timestamp of the swap file
    // and reset the BF_WRITE_MASK flags. Also sets buf->b_mtime.
    ਜੇ (overwriting)
    {
	ml_timestamp(buf);
	ਜੇ (append)
	    buf->b_flags &= ~BF_NEW;
	ਨਹੀਂਤਾਂ
	    buf->b_flags &= ~BF_WRITE_ਨਕਾਬ;
    }

    // If we kept a backup until now, and we are in patch mode, then we make
    // the backup file our 'original' file.
    ਜੇ (*p_pm && dobackup)
    {
	ਅੱਖਰ *org = (ਅੱਖਰ *)buf_modname((buf->b_p_sn || buf->b_shortname),
							  fname, p_pm, FALSE);

	ਜੇ (backup != ਨਿਰਾ)
	{
	    stat_T	st;

	    // If the original file does not exist yet
	    // the current backup file becomes the original file
	    ਜੇ (org == ਨਿਰਾ)
		emsg(_(e_patchmode_cant_save_original_file));
	    ਨਹੀਂਤਾਂ ਜੇ (mch_stat(org, &st) < 0)
	    {
		vim_rename(backup, (char_u *)org);
		VIM_CLEAR(backup);	    // don't delete the file
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		ਸੈੱਟ_file_ਸਮਾਂ((char_u *)org, st_old.st_atime, st_old.st_mtime);
#ਜੇ_ਅੰਤ
	    }
	}
	// If there is no backup file, remember that a (new) file was
	// created.
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ ਖਾਲੀ_ਥਾਵਾਂ_fd;

	    ਜੇ (org == ਨਿਰਾ
		    || (ਖਾਲੀ_ਥਾਵਾਂ_fd = mch_open(org,
				      O_CREAT | O_EXTRA | O_EXCL | O_NOFOLLOW,
					perm < 0 ? 0666 : (perm & 0777))) < 0)
	      emsg(_(e_patchmode_cant_touch_ਖਾਲੀ_ਥਾਵਾਂ_original_file));
	    ਨਹੀਂਤਾਂ
	      close(ਖਾਲੀ_ਥਾਵਾਂ_fd);
	}
	ਜੇ (org != ਨਿਰਾ)
	{
	    mch_setperm((char_u *)org, mch_getperm(fname) & 0777);
	    vim_free(org);
	}
    }

    // Remove the backup unless 'backup' option is set or there was a
    // conversion error.
    ਜੇ (!p_bk && backup != ਨਿਰਾ && !write_info.bw_conv_error
	    && mch_remove(backup) != 0)
	emsg(_(e_cant_delete_backup_file));

    ਜਾਓ nofail;

    // Finish up.  We get here either after failure or success.
fail:
    --no_wait_return;		// may wait for return now
nofail:

    // Done saving, we accept changed buffer warnings again
    buf->b_saving = false;

    vim_free(backup);
    ਜੇ (ਬਫਰ != smallbuf)
	vim_free(ਬਫਰ);
    vim_free(fenc_tofree);
    vim_free(write_info.bw_conv_buf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
    ਜੇ (write_info.bw_iconv_fd != (iconv_t)-1)
    {
	iconv_close(write_info.bw_iconv_fd);
	write_info.bw_iconv_fd = (iconv_t)-1;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACL
    mch_free_acl(acl);
#ਜੇ_ਅੰਤ

    ਜੇ (errmsg != ਨਿਰਾ)
    {
	ਪੂਰਨ numlen = errnum != ਨਿਰਾ ? (ਪੂਰਨ)STRLEN(errnum) : 0;

	attr = HL_ATTR(HLF_E);	// set highlight for error messages
	ਸੁਨੇਹਾ_ਜੋੜੋ_fname(buf,
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UNIX
		sfname
#ਨਹੀਂਤਾਂ
		fname
#ਜੇ_ਅੰਤ
		     );		// put file name in IObuff with quotes
	ਜੇ (STRLEN(IObuff) + STRLEN(errmsg) + numlen >= IOSIZE)
	    IObuff[IOSIZE - STRLEN(errmsg) - numlen - 1] = NUL;
	// If the error message has the form "is ...", put the error number in
	// front of the file name.
	ਜੇ (errnum != ਨਿਰਾ)
	{
	    STRMOVE(IObuff + numlen, IObuff);
	    mch_memmove(IObuff, errnum, (size_t)numlen);
	}
	STRCAT(IObuff, errmsg);
	emsg((ਅੱਖਰ *)IObuff);
	ਜੇ (errmsg_allocated)
	    vim_free(errmsg);

	retval = FAIL;
	ਜੇ (ਸਿਰਾ == 0)
	{
	    ਸੁਨੇਹਾ_puts_attr(_("\nWARNING: Original file may be lost or damaged\n"),
		    attr | ਸੁਨੇਹਾ_HIST);
	    ਸੁਨੇਹਾ_puts_attr(_("don't quit the editor until the file is successfully written!"),
		    attr | ਸੁਨੇਹਾ_HIST);

	    // Update the timestamp to avoid an "overwrite changed file"
	    // prompt when writing again.
	    ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st_old) >= 0)
	    {
		buf_store_ਸਮਾਂ(buf, &st_old, fname);
		buf->b_mtime_read = buf->b_mtime;
		buf->b_mtime_read_ns = buf->b_mtime_ns;
	    }
	}
    }
    ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_save;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
    // When writing the whole file and 'undofile' is set, also write the undo
    // file.
    ਜੇ (retval == OK && write_undo_file)
    {
	char_u	    hash[UNDO_HASH_ਮਾਪ];

	sha256_ਅੰਤ(&sha_ctx, hash);
	u_write_undo(ਨਿਰਾ, FALSE, buf, hash);
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (!should_abort(retval))
#ਨਹੀਂਤਾਂ
    ਜੇ (!got_int)
#ਜੇ_ਅੰਤ
    {
	aco_save_T	aco;

	curbuf->b_no_eol_lnum = 0;  // in case it was set by the previous read

	// Apply POST autocommands.
	// Careful: The autocommands may call buf_write() recursively!
	// Only do this when a window was found for "buf".
	aucmd_prepbuf(&aco, buf);
	ਜੇ (curbuf == buf)
	{
	    ਜੇ (append)
		apply_autocmds_exarg(EVENT_FILEAPPENDPOST, fname, fname,
							   FALSE, curbuf, eap);
	    ਨਹੀਂਤਾਂ ਜੇ (filtering)
		apply_autocmds_exarg(EVENT_FILTERWRITEPOST, ਨਿਰਾ, fname,
							   FALSE, curbuf, eap);
	    ਨਹੀਂਤਾਂ ਜੇ (reset_ਬਦਲਿਆ && ਸਾਰਾ)
		apply_autocmds_exarg(EVENT_BUFWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);
	    ਨਹੀਂਤਾਂ
		apply_autocmds_exarg(EVENT_FILEWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (aborting())	    // autocmds may abort script processing
	    retval = FALSE;
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    // Make sure marks will be written out to the viminfo file later, even when
    // the file is new.
    curbuf->b_marks_read = true;
#ਜੇ_ਅੰਤ

    got_int |= prev_got_int;

    ਵਾਪਸ retval;
}
