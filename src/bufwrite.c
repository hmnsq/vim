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

#ಯದಿ defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
# include <utime.h>		// for struct utimbuf
#endif

#define SMALLBUFSIZE	256	// size of emergency write buffer

/*
 * Structure to pass arguments from buf_write() to buf_write_bytes().
 */
ಸಂರಚನಾ bw_info
{
    ಪೂರ್ಣಾಙ್ಕ		bw_fd;		// file descriptor
    char_u	*bw_buf;	// buffer with data to be written
    ಪೂರ್ಣಾಙ್ಕ		bw_len;		// length of data
    ಪೂರ್ಣಾಙ್ಕ		bw_flags;	// FIO_ flags
#ifdef FEAT_CRYPT
    buf_T	*bw_buffer;	// buffer being written
    ಪೂರ್ಣಾಙ್ಕ		bw_finish;	// finish encrypting
#endif
    char_u	bw_rest[CONV_RESTLEN]; // not converted bytes
    ಪೂರ್ಣಾಙ್ಕ		bw_restlen;	// nr of bytes in bw_rest[]
    ಪೂರ್ಣಾಙ್ಕ		bw_first;	// first write call
    char_u	*bw_conv_buf;	// buffer for writing converted chars
    size_t	bw_conv_buflen; // size of bw_conv_buf
    ಪೂರ್ಣಾಙ್ಕ		bw_conv_error;	// set for conversion error
    linenr_T	bw_conv_error_lnum;  // first line with error or zero
    linenr_T	bw_start_lnum;	// line number at start of buffer
#ifdef USE_ICONV
    iconv_t	bw_iconv_fd;	// descriptor for iconv() or -1
#endif
};

/*
 * Convert a Unicode character to bytes.
 * Return TRUE for an error, FALSE when it's OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ucs2bytes(
    unsigned	c,		// in: character
    char_u	**pp,		// in/out: pointer to result
    ಪೂರ್ಣಾಙ್ಕ		flags)		// FIO_ flags
{
    char_u	*p = *pp;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		cc;


    ಯದಿ (flags & FIO_UCS4)
    {
	ಯದಿ (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	    *p++ = (c >> 16);
	    *p++ = (c >> 24);
	}
	ಅನ್ಯಥಾ
	{
	    *p++ = (c >> 24);
	    *p++ = (c >> 16);
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (flags & (FIO_UCS2 | FIO_UTF16))
    {
	ಯದಿ (c >= 0x10000)
	{
	    ಯದಿ (flags & FIO_UTF16)
	    {
		// Make two words, ten bits of the character in each.  First
		// word is 0xd800 - 0xdbff, second one 0xdc00 - 0xdfff
		c -= 0x10000;
		ಯದಿ (c >= 0x100000)
		    error = TRUE;
		cc = ((c >> 10) & 0x3ff) + 0xd800;
		ಯದಿ (flags & FIO_ENDIAN_L)
		{
		    *p++ = cc;
		    *p++ = ((unsigned)cc >> 8);
		}
		ಅನ್ಯಥಾ
		{
		    *p++ = ((unsigned)cc >> 8);
		    *p++ = cc;
		}
		c = (c & 0x3ff) + 0xdc00;
	    }
	    ಅನ್ಯಥಾ
		error = TRUE;
	}
	ಯದಿ (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	}
	ಅನ್ಯಥಾ
	{
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    ಅನ್ಯಥಾ    // Latin1
    {
	ಯದಿ (c >= 0x100)
	{
	    error = TRUE;
	    *p++ = 0xBF;
	}
	ಅನ್ಯಥಾ
	    *p++ = c;
    }

    *pp = p;
    ಪ್ರತ್ಯಾಗಮ error;
}

/*
 * Call write() to write a number of bytes to the file.
 * Handles encryption and 'encoding' conversion.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
buf_write_bytes(ಸಂರಚನಾ bw_info *ip)
{
    ಪೂರ್ಣಾಙ್ಕ		wlen;
    char_u	*buf = ip->bw_buf;	// data to write
    ಪೂರ್ಣಾಙ್ಕ		len = ip->bw_len;	// length of data
    ಪೂರ್ಣಾಙ್ಕ		flags = ip->bw_flags;	// extra flags

    // Skip conversion when writing the crypt magic number or the BOM.
    ಯದಿ (!(flags & FIO_NOCONVERT))
    {
	char_u		*p;
	unsigned	c;
	ಪೂರ್ಣಾಙ್ಕ		n;

	ಯದಿ (flags & FIO_UTF8)
	{
	    // Convert latin1 in the buffer to UTF-8 in the file.
	    p = ip->bw_conv_buf;	// translate to buffer
	    ಪ್ರತಿ (wlen = 0; wlen < len; ++wlen)
		p += utf_char2bytes(buf[wlen], p);
	    buf = ip->bw_conv_buf;
	    len = (ಪೂರ್ಣಾಙ್ಕ)(p - ip->bw_conv_buf);
	}
	ಅನ್ಯಥಾ ಯದಿ (flags & (FIO_UCS4 | FIO_UTF16 | FIO_UCS2 | FIO_LATIN1))
	{
	    // Convert UTF-8 bytes in the buffer to UCS-2, UCS-4, UTF-16 or
	    // Latin1 chars in the file.
	    ಯದಿ (flags & FIO_LATIN1)
		p = buf;	// translate in-place (can only get shorter)
	    ಅನ್ಯಥಾ
		p = ip->bw_conv_buf;	// translate to buffer
	    ಪ್ರತಿ (wlen = 0; wlen < len; wlen += n)
	    {
		ಯದಿ (wlen == 0 && ip->bw_restlen != 0)
		{
		    ಪೂರ್ಣಾಙ್ಕ		l;

		    // Use remainder of previous call.  Append the start of
		    // buf[] to get a full sequence.  Might still be too
		    // short!
		    l = CONV_RESTLEN - ip->bw_restlen;
		    ಯದಿ (l > len)
			l = len;
		    mch_memmove(ip->bw_rest + ip->bw_restlen, buf, (size_t)l);
		    n = utf_ptr2len_len(ip->bw_rest, ip->bw_restlen + l);
		    ಯದಿ (n > ip->bw_restlen + len)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			ಯದಿ (ip->bw_restlen + len > CONV_RESTLEN)
			    ಪ್ರತ್ಯಾಗಮ FAIL;
			ip->bw_restlen += len;
			ಭಙ್ಗ;
		    }
		    ಯದಿ (n > 1)
			c = utf_ptr2char(ip->bw_rest);
		    ಅನ್ಯಥಾ
			c = ip->bw_rest[0];
		    ಯದಿ (n >= ip->bw_restlen)
		    {
			n -= ip->bw_restlen;
			ip->bw_restlen = 0;
		    }
		    ಅನ್ಯಥಾ
		    {
			ip->bw_restlen -= n;
			mch_memmove(ip->bw_rest, ip->bw_rest + n,
						      (size_t)ip->bw_restlen);
			n = 0;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    n = utf_ptr2len_len(buf + wlen, len - wlen);
		    ಯದಿ (n > len - wlen)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			ಯದಿ (len - wlen > CONV_RESTLEN)
			    ಪ್ರತ್ಯಾಗಮ FAIL;
			ip->bw_restlen = len - wlen;
			mch_memmove(ip->bw_rest, buf + wlen,
						      (size_t)ip->bw_restlen);
			ಭಙ್ಗ;
		    }
		    ಯದಿ (n > 1)
			c = utf_ptr2char(buf + wlen);
		    ಅನ್ಯಥಾ
			c = buf[wlen];
		}
		// Check that there is enough space
		ಯದಿ (!(flags & FIO_LATIN1))
		{
		    size_t need = (flags & FIO_UCS4) ? 4 : 2;
		    ಯದಿ ((flags & FIO_UTF16) && c >= 0x10000)
			need = 4;

		    ಯದಿ ((size_t)(p - ip->bw_conv_buf) + need > ip->bw_conv_buflen)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}

		ಯದಿ (ucs2bytes(c, &p, flags) && !ip->bw_conv_error)
		{
		    ip->bw_conv_error = TRUE;
		    ip->bw_conv_error_lnum = ip->bw_start_lnum;
		}
		ಯದಿ (c == NL)
		    ++ip->bw_start_lnum;
	    }
	    ಯದಿ (flags & FIO_LATIN1)
		len = (ಪೂರ್ಣಾಙ್ಕ)(p - buf);
	    ಅನ್ಯಥಾ
	    {
		buf = ip->bw_conv_buf;
		len = (ಪೂರ್ಣಾಙ್ಕ)(p - ip->bw_conv_buf);
	    }
	}

#ifdef MSWIN
	ಅನ್ಯಥಾ ಯದಿ (flags & FIO_CODEPAGE)
	{
	    // Convert UTF-8 or codepage to UCS-2 and then to MS-Windows
	    // codepage.
	    char_u	*from;
	    size_t	fromlen;
	    char_u	*to;
	    ಪೂರ್ಣಾಙ್ಕ		u8c;
	    BOOL	bad = FALSE;
	    ಪೂರ್ಣಾಙ್ಕ		needed;

	    ಯದಿ (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    ಅನ್ಯಥಾ
	    {
		from = buf;
		fromlen = len;
	    }

	    to = ip->bw_conv_buf;
	    ಯದಿ (enc_utf8)
	    {
		// Convert from UTF-8 to UCS-2, to the start of the buffer.
		// The buffer has been allocated to be big enough.
		ಯಾವತ್ (fromlen > 0)
		{
		    n = (ಪೂರ್ಣಾಙ್ಕ)utf_ptr2len_len(from, (ಪೂರ್ಣಾಙ್ಕ)fromlen);
		    ಯದಿ (n > (ಪೂರ್ಣಾಙ್ಕ)fromlen)	// incomplete byte sequence
			ಭಙ್ಗ;
		    u8c = utf_ptr2char(from);
		    *to++ = (u8c & 0xff);
		    *to++ = (u8c >> 8);
		    fromlen -= n;
		    from += n;
		}

		// Copy remainder to ip->bw_rest[] to be used for the next
		// call.
		ಯದಿ (fromlen > CONV_RESTLEN)
		{
		    // weird overlong sequence
		    ip->bw_conv_error = TRUE;
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		mch_memmove(ip->bw_rest, from, fromlen);
		ip->bw_restlen = (ಪೂರ್ಣಾಙ್ಕ)fromlen;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Convert from enc_codepage to UCS-2, to the start of the
		// buffer.  The buffer has been allocated to be big enough.
		ip->bw_restlen = 0;
		needed = MultiByteToWideChar(enc_codepage,
			     MB_ERR_INVALID_CHARS, (LPCSTR)from, (ಪೂರ್ಣಾಙ್ಕ)fromlen,
								     ರಿಕ್ತ, 0);
		ಯದಿ (needed == 0)
		{
		    // When conversion fails there may be a trailing byte.
		    needed = MultiByteToWideChar(enc_codepage,
			 MB_ERR_INVALID_CHARS, (LPCSTR)from, (ಪೂರ್ಣಾಙ್ಕ)fromlen - 1,
								     ರಿಕ್ತ, 0);
		    ಯದಿ (needed == 0)
		    {
			// Conversion doesn't work.
			ip->bw_conv_error = TRUE;
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		    // Save the trailing byte for the next call.
		    ip->bw_rest[0] = from[fromlen - 1];
		    ip->bw_restlen = 1;
		}
		needed = MultiByteToWideChar(enc_codepage, MB_ERR_INVALID_CHARS,
				(LPCSTR)from, (ಪೂರ್ಣಾಙ್ಕ)(fromlen - ip->bw_restlen),
							  (LPWSTR)to, needed);
		ಯದಿ (needed == 0)
		{
		    // Safety check: Conversion doesn't work.
		    ip->bw_conv_error = TRUE;
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		to += needed * 2;
	    }

	    fromlen = to - ip->bw_conv_buf;
	    buf = to;
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	    ಯದಿ (FIO_GET_CP(flags) == CP_UTF8)
	    {
		// Convert from UCS-2 to UTF-8, using the remainder of the
		// conversion buffer.  Fails when out of space.
		ಪ್ರತಿ (from = ip->bw_conv_buf; fromlen > 1; fromlen -= 2)
		{
		    u8c = *from++;
		    u8c += (*from++ << 8);
		    to += utf_char2bytes(u8c, to);
		    ಯದಿ (to + 6 >= ip->bw_conv_buf + ip->bw_conv_buflen)
		    {
			ip->bw_conv_error = TRUE;
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		}
		len = (ಪೂರ್ಣಾಙ್ಕ)(to - buf);
	    }
	    ಅನ್ಯಥಾ
# endif
	    {
		// Convert from UCS-2 to the codepage, using the remainder of
		// the conversion buffer.  If the conversion uses the default
		// character "0", the data doesn't fit in this encoding, so
		// fail.
		len = WideCharToMultiByte(FIO_GET_CP(flags), 0,
			(LPCWSTR)ip->bw_conv_buf, (ಪೂರ್ಣಾಙ್ಕ)fromlen / ಪರಿಮಾಣ(WCHAR),
			(LPSTR)to, (ಪೂರ್ಣಾಙ್ಕ)(ip->bw_conv_buflen - fromlen), 0,
									&bad);
		ಯದಿ (bad)
		{
		    ip->bw_conv_error = TRUE;
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	}
#endif

#ifdef MACOS_CONVERT
	ಅನ್ಯಥಾ ಯದಿ (flags & FIO_MACROMAN)
	{
	    // Convert UTF-8 or latin1 to Apple MacRoman.
	    char_u	*from;
	    size_t	fromlen;

	    ಯದಿ (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    ಅನ್ಯಥಾ
	    {
		from = buf;
		fromlen = len;
	    }

	    ಯದಿ (enc2macroman(from, fromlen,
			ip->bw_conv_buf, &len, ip->bw_conv_buflen,
			ip->bw_rest, &ip->bw_restlen) == FAIL)
	    {
		ip->bw_conv_error = TRUE;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    buf = ip->bw_conv_buf;
	}
#endif

#ifdef USE_ICONV
	ಯದಿ (ip->bw_iconv_fd != (iconv_t)-1)
	{
	    ಧ್ರುವ ವರ್ಣ	*from;
	    size_t	fromlen;
	    ವರ್ಣ	*to;
	    size_t	tolen;

	    // Convert with iconv().
	    ಯದಿ (ip->bw_restlen > 0)
	    {
		ವರ್ಣ *fp;

		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		fp = (ವರ್ಣ *)ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(fp, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(fp + ip->bw_restlen, buf, (size_t)len);
		from = fp;
		tolen = ip->bw_conv_buflen - fromlen;
	    }
	    ಅನ್ಯಥಾ
	    {
		from = (ಧ್ರುವ ವರ್ಣ *)buf;
		fromlen = len;
		tolen = ip->bw_conv_buflen;
	    }
	    to = (ವರ್ಣ *)ip->bw_conv_buf;

	    ಯದಿ (ip->bw_first)
	    {
		size_t	save_len = tolen;

		// output the initial shift state sequence
		(ಶೂನ್ಯ)iconv(ip->bw_iconv_fd, ರಿಕ್ತ, ರಿಕ್ತ, &to, &tolen);

		// There is a bug in iconv() on Linux (which appears to be
		// wide-spread) which sets "to" to NULL and messes up "tolen".
		ಯದಿ (to == ರಿಕ್ತ)
		{
		    to = (ವರ್ಣ *)ip->bw_conv_buf;
		    tolen = save_len;
		}
		ip->bw_first = FALSE;
	    }

	    // If iconv() has an error or there is not enough room, fail.
	    ಯದಿ ((iconv(ip->bw_iconv_fd, (ಶೂನ್ಯ *)&from, &fromlen, &to, &tolen)
			== (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						    || fromlen > CONV_RESTLEN)
	    {
		ip->bw_conv_error = TRUE;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    // copy remainder to ip->bw_rest[] to be used for the next call.
	    ಯದಿ (fromlen > 0)
		mch_memmove(ip->bw_rest, (ಶೂನ್ಯ *)from, fromlen);
	    ip->bw_restlen = (ಪೂರ್ಣಾಙ್ಕ)fromlen;

	    buf = ip->bw_conv_buf;
	    len = (ಪೂರ್ಣಾಙ್ಕ)((char_u *)to - ip->bw_conv_buf);
	}
#endif
    }

    ಯದಿ (ip->bw_fd < 0)
	// Only checking conversion, which is OK if we get here.
	ಪ್ರತ್ಯಾಗಮ OK;

#ifdef FEAT_CRYPT
    ಯದಿ (flags & FIO_ENCRYPTED)
    {
	// Encrypt the data. Do it in-place if possible, otherwise use an
	// allocated buffer.
# ifdef CRYPT_NOT_INPLACE
	ಯದಿ (crypt_works_inplace(ip->bw_buffer->b_cryptstate))
	{
# endif
	    crypt_encode_inplace(ip->bw_buffer->b_cryptstate, buf, len,
								ip->bw_finish);
# ifdef CRYPT_NOT_INPLACE
	}
	ಅನ್ಯಥಾ
	{
	    char_u *outbuf;

	    len = crypt_encode_alloc(curbuf->b_cryptstate, buf, len, &outbuf,
								ip->bw_finish);
	    ಯದಿ (len == 0)
		ಪ್ರತ್ಯಾಗಮ OK;  // Crypt layer is buffering, will flush later.
	    wlen = write_eintr(ip->bw_fd, outbuf, len);
	    vim_free(outbuf);
	    ಪ್ರತ್ಯಾಗಮ (wlen < len) ? FAIL : OK;
	}
# endif
    }
#endif

    wlen = write_eintr(ip->bw_fd, buf, len);
    ಪ್ರತ್ಯಾಗಮ (wlen < len) ? FAIL : OK;
}

/*
 * Check modification time of file, before writing to it.
 * The size isn't checked, because using a tool like "gzip" takes care of
 * using the same timestamp but can't set the size.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_mtime(buf_T *buf, stat_T *st)
{
    ಯದಿ (buf->b_mtime_read != 0
		  && time_differs(st, buf->b_mtime_read, buf->b_mtime_read_ns))
    {
	ಸಂದೇಶ_scroll = TRUE;	    // don't overwrite messages here
	ಸಂದೇಶ_silent = 0;		    // must give this prompt
	// don't use emsg() here, don't want to flush the buffers
	ಸಂದೇಶ_attr(_("WARNING: The file has been changed since reading it!!!"),
						       HL_ATTR(HLF_E));
	ಯದಿ (ask_yesno((char_u *)_("Do you really want to write to it"),
								 TRUE) == 'n')
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಸಂದೇಶ_scroll = FALSE;	    // always overwrite the file message now
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate a BOM in "buf[4]" for encoding "name".
 * Return the length of the BOM (zero when no BOM).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
make_bom(char_u *buf, char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		flags;
    char_u	*p;

    flags = get_fio_flags(name);

    // Can't put a BOM in a non-Unicode file.
    ಯದಿ (flags == FIO_LATIN1 || flags == 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ (flags == FIO_UTF8)	// UTF-8
    {
	buf[0] = 0xef;
	buf[1] = 0xbb;
	buf[2] = 0xbf;
	ಪ್ರತ್ಯಾಗಮ 3;
    }
    p = buf;
    (ಶೂನ್ಯ)ucs2bytes(0xfeff, &p, flags);
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(p - buf);
}

#ifdef UNIX
    ಸ್ಥಿರ ಶೂನ್ಯ
set_file_time(
    char_u  *fname,
    time_t  atime,	    // access time
    time_t  mtime)	    // modification time
{
# ಯದಿ defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
    ಸಂರಚನಾ utimbuf  buf;

    buf.actime	= atime;
    buf.modtime	= mtime;
    (ಶೂನ್ಯ)utime((ವರ್ಣ *)fname, &buf);
# ಅನ್ಯಥಾ
#  ಯದಿ defined(HAVE_UTIMES)
    ಸಂರಚನಾ timeval  tvp[2];

    tvp[0].tv_sec   = atime;
    tvp[0].tv_usec  = 0;
    tvp[1].tv_sec   = mtime;
    tvp[1].tv_usec  = 0;
    (ಶೂನ್ಯ)utimes((ವರ್ಣ *)fname, (ಧ್ರುವ ಸಂರಚನಾ timeval *)&tvp);
#  endif
# endif
}
#endif // UNIX

    ವರ್ಣ *
new_file_message(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ shortmess(SHM_NEW) ? _("[New]") : _("[New File]");
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
    ಪೂರ್ಣಾಙ್ಕ
buf_write(
    buf_T	    *buf,
    char_u	    *fname,
    char_u	    *sfname,
    linenr_T	    start,
    linenr_T	    end,
    exarg_T	    *eap,		// for forced 'ff' and 'fenc', can be
					// NULL!
    ಪೂರ್ಣಾಙ್ಕ		    append,		// append to the file
    ಪೂರ್ಣಾಙ್ಕ		    forceit,
    ಪೂರ್ಣಾಙ್ಕ		    reset_changed,
    ಪೂರ್ಣಾಙ್ಕ		    filtering)
{
    ಪೂರ್ಣಾಙ್ಕ		    fd;
    char_u	    *backup = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		    backup_copy = FALSE; // copy the original file?
    ಪೂರ್ಣಾಙ್ಕ		    dobackup;
    char_u	    *ffname;
    char_u	    *wfname = ರಿಕ್ತ;	// name of file to write to
    char_u	    *s;
    char_u	    *ptr;
    char_u	    c;
    ಪೂರ್ಣಾಙ್ಕ		    len;
    linenr_T	    lnum;
    long	    nchars;
    char_u	    *errmsg = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		    errmsg_allocated = FALSE;
    char_u	    *errnum = ರಿಕ್ತ;
    char_u	    *buffer;
    char_u	    smallbuf[SMALLBUFSIZE];
    char_u	    *backup_ext;
    ಪೂರ್ಣಾಙ್ಕ		    bufsize;
    long	    perm;		    // file permissions
    ಪೂರ್ಣಾಙ್ಕ		    retval = OK;
    ಪೂರ್ಣಾಙ್ಕ		    newfile = FALSE;	    // TRUE if file doesn't exist yet
    ಪೂರ್ಣಾಙ್ಕ		    ಸಂದೇಶ_save = ಸಂದೇಶ_scroll;
    ಪೂರ್ಣಾಙ್ಕ		    overwriting;	    // TRUE if writing over original
    ಪೂರ್ಣಾಙ್ಕ		    no_eol = FALSE;	    // no end-of-line written
    ಪೂರ್ಣಾಙ್ಕ		    device = FALSE;	    // writing to a device
    stat_T	    st_old;
    ಪೂರ್ಣಾಙ್ಕ		    prev_got_int = got_int;
    ಪೂರ್ಣಾಙ್ಕ		    checking_conversion;
    ಪೂರ್ಣಾಙ್ಕ		    file_readonly = FALSE;  // overwritten file is read-only
#ಯದಿ defined(UNIX)			    // XXX fix me sometime?
    ಪೂರ್ಣಾಙ್ಕ		    made_writable = FALSE;  // 'w' bit has been set
#endif
					// writing everything
    ಪೂರ್ಣಾಙ್ಕ		    whole = (start == 1 && end == buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
    linenr_T	    old_ಸಾಲು_ಎಣಿಕೆ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		    attr;
    ಪೂರ್ಣಾಙ್ಕ		    fileformat;
    ಪೂರ್ಣಾಙ್ಕ		    write_bin;
    ಸಂರಚನಾ bw_info  write_info;		// info for buf_write_bytes()
    ಪೂರ್ಣಾಙ್ಕ		    converted = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		    notconverted = FALSE;
    char_u	    *fenc;		// effective 'fileencoding'
    char_u	    *fenc_tofree = ರಿಕ್ತ; // allocated "fenc"
    ಪೂರ್ಣಾಙ್ಕ		    wb_flags = 0;
#ifdef HAVE_ACL
    vim_acl_T	    acl = ರಿಕ್ತ;		// ACL copied from original file to
					// backup or new file
#endif
#ifdef FEAT_PERSISTENT_UNDO
    ಪೂರ್ಣಾಙ್ಕ		    write_undo_file = FALSE;
    context_sha256_T sha_ctx;
#endif
    unsigned ಪೂರ್ಣಾಙ್ಕ    bkc = get_bkc_flags(buf);
    pos_T	    orig_start = buf->b_op_start;
    pos_T	    orig_end = buf->b_op_end;

    ಯದಿ (fname == ರಿಕ್ತ || *fname == NUL)	// safety check
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
    {
	// This can happen during startup when there is a stray "w" in the
	// vimrc file.
	emsg(_(e_empty_buffer));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Disallow writing from .exrc and .vimrc in current directory for
    // security reasons.
    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Avoid a crash for a long name.
    ಯದಿ (STRLEN(fname) >= MAXPATHL)
    {
	emsg(_(e_name_too_long));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // must init bw_conv_buf and bw_iconv_fd before jumping to "fail"
    write_info.bw_conv_buf = ರಿಕ್ತ;
    write_info.bw_conv_error = FALSE;
    write_info.bw_conv_error_lnum = 0;
    write_info.bw_restlen = 0;
#ifdef USE_ICONV
    write_info.bw_iconv_fd = (iconv_t)-1;
#endif
#ifdef FEAT_CRYPT
    write_info.bw_buffer = buf;
    write_info.bw_finish = FALSE;
#endif

    // After writing a file changedtick changes but we don't want to display
    // the line.
    ex_no_reprint = TRUE;

    // If there is no file name yet, use the one for the written file.
    // BF_NOTEDITED is set to reflect this (in case the write fails).
    // Don't do this when the write is for a filter command.
    // Don't do this when appending.
    // Only do this when 'cpoptions' contains the 'F' flag.
    ಯದಿ (buf->b_ffname == ರಿಕ್ತ
	    && reset_changed
	    && whole
	    && buf == curbuf
	    && !bt_nofilename(buf)
	    && !filtering
	    && (!append || vim_strchr(p_cpo, CPO_FNAMEAPP) != ರಿಕ್ತ)
	    && vim_strchr(p_cpo, CPO_FNAMEW) != ರಿಕ್ತ)
    {
	ಯದಿ (set_rw_fname(fname, sfname) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	buf = curbuf;	    // just in case autocmds made "buf" invalid
    }

    ಯದಿ (sfname == ರಿಕ್ತ)
	sfname = fname;
    // For Unix: Use the short file name whenever possible.
    // Avoids problems with networks and when directory names are changed.
    // Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
    // another directory, which we don't detect
    ffname = fname;			    // remember full fname
#ifdef UNIX
    fname = sfname;
#endif

    ಯದಿ (buf->b_ffname != ರಿಕ್ತ && fnamecmp(ffname, buf->b_ffname) == 0)
	overwriting = TRUE;
    ಅನ್ಯಥಾ
	overwriting = FALSE;

    ಯದಿ (exiting)
	settmode(TMODE_COOK);	    // when exiting allow typeahead now

    ++no_wait_return;		    // don't wait for return yet

    // Set '[ and '] marks to the lines to be written.
    buf->b_op_start.lnum = start;
    buf->b_op_start.col = 0;
    buf->b_op_end.lnum = end;
    buf->b_op_end.col = 0;

    {
	aco_save_T	aco;
	ಪೂರ್ಣಾಙ್ಕ		buf_ffname = FALSE;
	ಪೂರ್ಣಾಙ್ಕ		buf_sfname = FALSE;
	ಪೂರ್ಣಾಙ್ಕ		buf_fname_f = FALSE;
	ಪೂರ್ಣಾಙ್ಕ		buf_fname_s = FALSE;
	ಪೂರ್ಣಾಙ್ಕ		did_cmd = FALSE;
	ಪೂರ್ಣಾಙ್ಕ		nofile_err = FALSE;
	ಪೂರ್ಣಾಙ್ಕ		empty_memline = (buf->b_ml.ml_mfp == ರಿಕ್ತ);
	bufref_T	bufref;

	// Apply PRE autocommands.
	// Set curbuf to the buffer to be written.
	// Careful: The autocommands may call buf_write() recursively!
	ಯದಿ (ffname == buf->b_ffname)
	    buf_ffname = TRUE;
	ಯದಿ (sfname == buf->b_sfname)
	    buf_sfname = TRUE;
	ಯದಿ (fname == buf->b_ffname)
	    buf_fname_f = TRUE;
	ಯದಿ (fname == buf->b_sfname)
	    buf_fname_s = TRUE;

	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, buf);
	ಯದಿ (curbuf != buf)
	{
	    // Could not find a window for "buf".  Doing more might cause
	    // problems, better bail out.
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	set_bufref(&bufref, buf);

	ಯದಿ (append)
	{
	    ಯದಿ (!(did_cmd = apply_autocmds_exarg(EVENT_FILEAPPENDCMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		ಯದಿ (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		ಅನ್ಯಥಾ
		    apply_autocmds_exarg(EVENT_FILEAPPENDPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (filtering)
	{
	    apply_autocmds_exarg(EVENT_FILTERWRITEPRE,
					    ರಿಕ್ತ, sfname, FALSE, curbuf, eap);
	}
	ಅನ್ಯಥಾ ಯದಿ (reset_changed && whole)
	{
	    ಪೂರ್ಣಾಙ್ಕ was_changed = curbufIsChanged();

	    did_cmd = apply_autocmds_exarg(EVENT_BUFWRITECMD,
					  sfname, sfname, FALSE, curbuf, eap);
	    ಯದಿ (did_cmd)
	    {
		ಯದಿ (was_changed && !curbufIsChanged())
		{
		    // Written everything correctly and BufWriteCmd has reset
		    // 'modified': Correct the undo information so that an
		    // undo now sets 'modified'.
		    u_unchanged(curbuf);
		    u_update_save_nr(curbuf);
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		ಅನ್ಯಥಾ
		    apply_autocmds_exarg(EVENT_BUFWRITEPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (!(did_cmd = apply_autocmds_exarg(EVENT_FILEWRITECMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		ಯದಿ (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		ಅನ್ಯಥಾ
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
	ಯದಿ (!bufref_valid(&bufref))
	    buf = ರಿಕ್ತ;
	ಯದಿ (buf == ರಿಕ್ತ || (buf->b_ml.ml_mfp == ರಿಕ್ತ && !empty_memline)
				       || did_cmd || nofile_err
#ifdef FEAT_EVAL
				       || aborting()
#endif
				       )
	{
	    ಯದಿ (buf != ರಿಕ್ತ && (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	    {
		// restore the original '[ and '] positions
		buf->b_op_start = orig_start;
		buf->b_op_end = orig_end;
	    }

	    --no_wait_return;
	    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
	    ಯದಿ (nofile_err)
		semsg(_(e_no_matching_autocommands_for_buftype_str_buffer),
							       curbuf->b_p_bt);

	    ಯದಿ (nofile_err
#ifdef FEAT_EVAL
		    || aborting()
#endif
		    )
		// An aborting error, interrupt or exception in the
		// autocommands.
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (did_cmd)
	    {
		ಯದಿ (buf == ರಿಕ್ತ)
		    // The buffer was deleted.  We assume it was written
		    // (can't retry anyway).
		    ಪ್ರತ್ಯಾಗಮ OK;
		ಯದಿ (overwriting)
		{
		    // Assume the buffer was written, update the timestamp.
		    ml_timestamp(buf);
		    ಯದಿ (append)
			buf->b_flags &= ~BF_NEW;
		    ಅನ್ಯಥಾ
			buf->b_flags &= ~BF_WRITE_MASK;
		}
		ಯದಿ (reset_changed && buf->b_changed && !append
			&& (overwriting || vim_strchr(p_cpo, CPO_PLUS) != ರಿಕ್ತ))
		    // Buffer still changed, the autocommands didn't work
		    // properly.
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಪ್ರತ್ಯಾಗಮ OK;
	    }
#ifdef FEAT_EVAL
	    ಯದಿ (!aborting())
#endif
		emsg(_(e_autocommands_deleted_or_unloaded_buffer_to_be_written));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	// The autocommands may have changed the number of lines in the file.
	// When writing the whole file, adjust the end.
	// When writing part of the file, assume that the autocommands only
	// changed the number of lines that are to be written (tricky!).
	ಯದಿ (buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ != old_ಸಾಲು_ಎಣಿಕೆ)
	{
	    ಯದಿ (whole)						// write all
		end = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಅನ್ಯಥಾ ಯದಿ (buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ > old_ಸಾಲು_ಎಣಿಕೆ)	// more lines
		end += buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - old_ಸಾಲು_ಎಣಿಕೆ;
	    ಅನ್ಯಥಾ						// less lines
	    {
		end -= old_ಸಾಲು_ಎಣಿಕೆ - buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		ಯದಿ (end < start)
		{
		    --no_wait_return;
		    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
		    emsg(_(e_autocommands_changed_ಸಂಖ್ಯೆ_of_ಸಾಲುಗಳು_in_unexpected_way));
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	}

	// The autocommands may have changed the name of the buffer, which may
	// be kept in fname, ffname and sfname.  A ":cd" may also have dropped
	// the short name, then b_fname is the full name.
	ಯದಿ (buf_ffname)
	    ffname = buf->b_ffname;
	ಯದಿ (buf_sfname)
	    sfname = buf->b_fname;
	ಯದಿ (buf_fname_f)
	    fname = buf->b_ffname;
	ಯದಿ (buf_fname_s)
	    fname = buf->b_fname;
    }

    ಯದಿ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	// restore the original '[ and '] positions
	buf->b_op_start = orig_start;
	buf->b_op_end = orig_end;
    }

#ifdef FEAT_NETBEANS_INTG
    ಯದಿ (netbeans_active() && isNetbeansBuffer(buf))
    {
	ಯದಿ (whole)
	{
	    // b_changed can be 0 after an undo, but we still need to write
	    // the buffer to NetBeans.
	    ಯದಿ (buf->b_changed || isNetbeansModified(buf))
	    {
		--no_wait_return;		// may wait for return now
		ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;
		netbeans_save_buffer(buf);	// no error checking...
		ಪ್ರತ್ಯಾಗಮ retval;
	    }
	    ಅನ್ಯಥಾ
	    {
		errnum = (char_u *)"E656: ";
		errmsg = (char_u *)_(e_netbeans_disallows_writes_of_unmodified_buffers);
		buffer = ರಿಕ್ತ;
		ಗಚ್ಛ fail;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    errnum = (char_u *)"E657: ";
	    errmsg = (char_u *)_(e_partial_writes_disallowed_for_netbeans_buffers);
	    buffer = ರಿಕ್ತ;
	    ಗಚ್ಛ fail;
	}
    }
#endif

    ಯದಿ (shortmess(SHM_OVER) && !exiting)
	ಸಂದೇಶ_scroll = FALSE;	    // overwrite previous file message
    ಅನ್ಯಥಾ
	ಸಂದೇಶ_scroll = TRUE;	    // don't overwrite previous file message
    ಯದಿ (!filtering)
	filemess(buf,
#ifndef UNIX
		sfname,
#ಅನ್ಯಥಾ
		fname,
#endif
		    (char_u *)"", 0);	// show that we are busy
    ಸಂದೇಶ_scroll = FALSE;		    // always overwrite the file message now

    buffer = alloc(WRITEBUFSIZE);
    ಯದಿ (buffer == ರಿಕ್ತ)		    // can't allocate big buffer, use small
				    // one (to be able to write when out of
				    // memory)
    {
	buffer = smallbuf;
	bufsize = SMALLBUFSIZE;
    }
    ಅನ್ಯಥಾ
	bufsize = WRITEBUFSIZE;

    // Get information about original file (if there is one).
#ಯದಿ defined(UNIX)
    st_old.st_dev = 0;
    st_old.st_ino = 0;
    perm = -1;
    ಯದಿ (mch_stat((ವರ್ಣ *)fname, &st_old) < 0)
	newfile = TRUE;
    ಅನ್ಯಥಾ
    {
	perm = st_old.st_mode;
	ಯದಿ (!S_ISREG(st_old.st_mode))		// not a file
	{
	    ಯದಿ (S_ISDIR(st_old.st_mode))
	    {
		errnum = (char_u *)"E502: ";
		errmsg = (char_u *)_(e_is_a_directory);
		ಗಚ್ಛ fail;
	    }
	    ಯದಿ (mch_nodetype(fname) != ಗಂಟು_WRITABLE)
	    {
		errnum = (char_u *)"E503: ";
		errmsg = (char_u *)_(e_is_not_file_or_writable_device);
		ಗಚ್ಛ fail;
	    }
	    // It's a device of some kind (or a fifo) which we can write to
	    // but for which we can't make a backup.
	    device = TRUE;
	    newfile = TRUE;
	    perm = -1;
	}
    }
#ಅನ್ಯಥಾ // !UNIX
    // Check for a writable device name.
    c = mch_nodetype(fname);
    ಯದಿ (c == ಗಂಟು_OTHER)
    {
	errnum = (char_u *)"E503: ";
	errmsg = (char_u *)_(e_is_not_file_or_writable_device);
	ಗಚ್ಛ fail;
    }
    ಯದಿ (c == ಗಂಟು_WRITABLE)
    {
# ಯದಿ defined(MSWIN)
	// MS-Windows allows opening a device, but we will probably get stuck
	// trying to write to it.
	ಯದಿ (!p_odev)
	{
	    errnum = (char_u *)"E796: ";
	    errmsg = (char_u *)_(e_writing_to_device_disabled_with_opendevice_option);
	    ಗಚ್ಛ fail;
	}
# endif
	device = TRUE;
	newfile = TRUE;
	perm = -1;
    }
    ಅನ್ಯಥಾ
    {
	perm = mch_getperm(fname);
	ಯದಿ (perm < 0)
	    newfile = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (mch_isdir(fname))
	{
	    errnum = (char_u *)"E502: ";
	    errmsg = (char_u *)_(e_is_a_directory);
	    ಗಚ್ಛ fail;
	}
	ಯದಿ (overwriting)
	    (ಶೂನ್ಯ)mch_stat((ವರ್ಣ *)fname, &st_old);
    }
#endif // !UNIX

    ಯದಿ (!device && !newfile)
    {
	// Check if the file is really writable (when renaming the file to
	// make a backup we won't discover it later).
	file_readonly = check_file_readonly(fname, (ಪೂರ್ಣಾಙ್ಕ)perm);

	ಯದಿ (!forceit && file_readonly)
	{
	    ಯದಿ (vim_strchr(p_cpo, CPO_FWRITE) != ರಿಕ್ತ)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_in_cpoptions);
	    }
	    ಅನ್ಯಥಾ
	    {
		errnum = (char_u *)"E505: ";
		errmsg = (char_u *)_(e_is_read_only_add_bang_to_override);
	    }
	    ಗಚ್ಛ fail;
	}

	// Check if the timestamp hasn't changed since reading the file.
	ಯದಿ (overwriting)
	{
	    retval = check_mtime(buf, &st_old);
	    ಯದಿ (retval == FAIL)
		ಗಚ್ಛ fail;
	}
    }

#ifdef HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    ಯದಿ (!newfile)
	acl = mch_get_acl(fname);
#endif

    // If 'backupskip' is not empty, don't make a backup for some files.
    dobackup = (p_wb || p_bk || *p_pm != NUL);
    ಯದಿ (dobackup && *p_bsk != NUL && match_file_list(p_bsk, sfname, ffname))
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
    ಯದಿ (!(append && *p_pm == NUL) && !filtering && perm >= 0 && dobackup)
    {
#ಯದಿ defined(UNIX) || defined(MSWIN)
	stat_T	    st;
#endif

	ಯದಿ ((bkc & BKC_YES) || append)	// "yes"
	    backup_copy = TRUE;
#ಯದಿ defined(UNIX) || defined(MSWIN)
	ಅನ್ಯಥಾ ಯದಿ ((bkc & BKC_AUTO))	// "auto"
	{
# ifdef UNIX
	    // Don't rename the file when:
	    // - it's a hard link
	    // - it's a symbolic link
	    // - we don't have write permission in the directory
	    // - we can't set the owner/group of the new file
	    ಯದಿ (st_old.st_nlink > 1
		    || mch_lstat((ವರ್ಣ *)fname, &st) < 0
		    || st.st_dev != st_old.st_dev
		    || st.st_ino != st_old.st_ino
#  ifndef HAVE_FCHOWN
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid
#  endif
		    )
		backup_copy = TRUE;
	    ಅನ್ಯಥಾ
# ಅನ್ಯಥಾ
#  ifdef MSWIN
	    // On NTFS file systems hard links are possible.
	    ಯದಿ (mch_is_linked(fname))
		backup_copy = TRUE;
	    ಅನ್ಯಥಾ
#  endif
# endif
	    {
		size_t	dirlen;
		char_u	tmp_fname[MAXPATHL];
		ಪೂರ್ಣಾಙ್ಕ	i;

		// Check if we can create a file and set the owner/group/mode to
		// the ones from the original file.
		// First find a file name that doesn't exist yet.  Use the
		// conventional hidden swap file name so that this short-lived
		// probe is covered by standard Vim ignore patterns.
		dirlen = (size_t)(gettail(fname) - fname);
		vim_strncpy(tmp_fname, fname, dirlen);
		fd = -1;
		ಪ್ರತಿ (i = 4913; ; i += 123)
		{
		    vim_snprintf((ವರ್ಣ *)tmp_fname + dirlen,
			ಪರಿಮಾಣ(tmp_fname) - dirlen, ".%d.swp", i);
		    ಯದಿ (mch_lstat((ವರ್ಣ *)tmp_fname, &st) < 0)
		    {
			fd = mch_open((ವರ್ಣ *)tmp_fname,
				    O_CREAT|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
			ಯದಿ (fd < 0 && errno == EEXIST)
			    // If the same file name is created by another
			    // process between lstat() and open(), find another
			    // name.
			    ಅನುವರ್ತ;
			ಭಙ್ಗ;
		    }
		}
		ಯದಿ (fd < 0)	// can't write in directory
		    backup_copy = TRUE;
		ಅನ್ಯಥಾ
		{
# ifdef UNIX
#  ifdef HAVE_FCHOWN
		    vim_ignored = fchown(fd, st_old.st_uid, st_old.st_gid);
#  endif
#  ifdef HAVE_FCHMOD
		    (ಶೂನ್ಯ)mch_fsetperm(fd, perm);
#  ಅನ್ಯಥಾ
		    (ಶೂನ್ಯ)mch_setperm(tmp_fname, perm);
#  endif
		    ಯದಿ (mch_stat((ವರ್ಣ *)tmp_fname, &st) < 0
			    || st.st_uid != st_old.st_uid
			    || st.st_gid != st_old.st_gid
			    || (long)st.st_mode != perm)
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
			ಪೂರ್ಣಾಙ್ಕ try;

			ಪ್ರತಿ (try = 0; try < 10; ++try)
			{
			    ಯದಿ (mch_lstat((ವರ್ಣ *)tmp_fname, &st) < 0)
				ಭಙ್ಗ;
			    ui_delay(50L, TRUE);  // wait 50 msec
			    mch_remove(tmp_fname);
			}
		    }
# endif
		}
	    }
	}

	// Break symlinks and/or hardlinks if we've been asked to.
	ಯದಿ ((bkc & BKC_BREAKSYMLINK) || (bkc & BKC_BREAKHARDLINK))
	{
# ifdef UNIX
	    ಪೂರ್ಣಾಙ್ಕ	lstat_res;

	    lstat_res = mch_lstat((ವರ್ಣ *)fname, &st);

	    // Symlinks.
	    ಯದಿ ((bkc & BKC_BREAKSYMLINK)
		    && lstat_res == 0
		    && st.st_ino != st_old.st_ino)
		backup_copy = FALSE;

	    // Hardlinks.
	    ಯದಿ ((bkc & BKC_BREAKHARDLINK)
		    && st_old.st_nlink > 1
		    && (lstat_res != 0 || st.st_ino == st_old.st_ino))
		backup_copy = FALSE;
# ಅನ್ಯಥಾ
#  ಯದಿ defined(MSWIN)
	    // Symlinks.
	    ಯದಿ ((bkc & BKC_BREAKSYMLINK) && mch_is_symbolic_link(fname))
		backup_copy = FALSE;

	    // Hardlinks.
	    ಯದಿ ((bkc & BKC_BREAKHARDLINK) && mch_is_hard_link(fname))
		backup_copy = FALSE;
#  endif
# endif
	}

#endif

	// make sure we have a valid backup extension to use
	ಯದಿ (*p_bex == NUL)
	    backup_ext = (char_u *)".bak";
	ಅನ್ಯಥಾ
	    backup_ext = p_bex;

	ಯದಿ (backup_copy
		&& (fd = mch_open((ವರ್ಣ *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ		bfd;
	    char_u	*copybuf;
	    ಪೂರ್ಣಾಙ್ಕ		some_error = FALSE;
	    stat_T	st_new;
	    char_u	*dirp;
	    char_u	*rootname;
#ಯದಿ defined(UNIX)
	    ಪೂರ್ಣಾಙ್ಕ		did_set_shortname;
	    mode_t	umask_save;
#endif

	    copybuf = alloc(WRITEBUFSIZE + 1);
	    ಯದಿ (copybuf == ರಿಕ್ತ)
	    {
		some_error = TRUE;	    // out of memory
		ಗಚ್ಛ nobackup;
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
	    ಯಾವತ್ (*dirp)
	    {
		char_u	*p UNUSED;
		ಪೂರ್ಣಾಙ್ಕ	copybuf_len UNUSED;

#ifdef UNIX
		st_new.st_ino = 0;
		st_new.st_dev = 0;
		st_new.st_gid = 0;
#endif

		// Isolate one directory name, using an entry in 'bdir'.
		copybuf_len = copy_option_part(&dirp, copybuf, WRITEBUFSIZE, ",");

#ಯದಿ defined(UNIX) || defined(MSWIN)
		p = copybuf + copybuf_len;
		ಯದಿ (after_pathsep(copybuf, p) && p[-1] == p[-2])
		    // Ends with '//', use full path
		    ಯದಿ ((p = make_percent_swname(copybuf, p, fname)) != ರಿಕ್ತ)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#endif
		rootname = get_file_in_dir(fname, copybuf);
		ಯದಿ (rootname == ರಿಕ್ತ)
		{
		    some_error = TRUE;	    // out of memory
		    ಗಚ್ಛ nobackup;
		}

#ಯದಿ defined(UNIX)
		did_set_shortname = FALSE;
#endif

		// May try twice if 'shortname' not set.
		ಪ್ರತಿ (;;)
		{
		    // Make the backup file name.
		    ಯದಿ (backup == ರಿಕ್ತ)
			backup = buf_modname((buf->b_p_sn || buf->b_shortname),
						 rootname, backup_ext, FALSE);
		    ಯದಿ (backup == ರಿಕ್ತ)
		    {
			vim_free(rootname);
			some_error = TRUE;		// out of memory
			ಗಚ್ಛ nobackup;
		    }

		    // Check if backup file already exists.
		    ಯದಿ (mch_stat((ವರ್ಣ *)backup, &st_new) >= 0)
		    {
#ifdef UNIX
			// Check if backup file is same as original file.
			// May happen when modname() gave the same file back.
			// E.g. silly link, or file name-length reached.
			// If we don't check here, we either ruin the file
			// when copying or erase it after writing. jw.
			ಯದಿ (st_new.st_dev == st_old.st_dev
					    && st_new.st_ino == st_old.st_ino)
			{
			    VIM_CLEAR(backup);	// no backup file to delete
			    // may try again with 'shortname' set
			    ಯದಿ (!(buf->b_shortname || buf->b_p_sn))
			    {
				buf->b_shortname = true;
				did_set_shortname = TRUE;
				ಅನುವರ್ತ;
			    }
				// setting shortname didn't help
			    ಯದಿ (did_set_shortname)
				buf->b_shortname = false;
			    ಭಙ್ಗ;
			}
#endif

			// If we are not going to keep the backup file, don't
			// delete an existing one, try to use another name.
			// Change one character, just before the extension.
			ಯದಿ (!p_bk)
			{
			    char_u	*wp;

			    wp = backup + STRLEN(backup) - 1
							 - STRLEN(backup_ext);
			    ಯದಿ (wp < backup)	// empty file name ???
				wp = backup;
			    *wp = 'z';
			    ಯಾವತ್ (*wp > 'a'
				    && mch_stat((ವರ್ಣ *)backup, &st_new) >= 0)
				--*wp;
			    // They all exist??? Must be something wrong.
			    ಯದಿ (*wp == 'a')
				VIM_CLEAR(backup);
			}
		    }
		    ಭಙ್ಗ;
		}
		vim_free(rootname);

		// Try to create the backup file
		ಯದಿ (backup != ರಿಕ್ತ)
		{
		    // remove old backup, if present
		    mch_remove(backup);
		    // Open with O_EXCL to avoid the file being created while
		    // we were sleeping (symlink hacker attack?). Reset umask
		    // if possible to avoid mch_setperm() below.
#ifdef UNIX
		    umask_save = umask(0);
#endif
		    bfd = mch_open((ವರ್ಣ *)backup,
				O_WRONLY|O_CREAT|O_EXTRA|O_EXCL|O_NOFOLLOW,
								 perm & 0777);
#ifdef UNIX
		    (ಶೂನ್ಯ)umask(umask_save);
#endif
		    ಯದಿ (bfd < 0)
			VIM_CLEAR(backup);
		    ಅನ್ಯಥಾ
		    {
			// Set file protection same as original file, but
			// strip s-bit.  Only needed if umask() wasn't used
			// above.
#ifndef UNIX
			(ಶೂನ್ಯ)mch_setperm(backup, perm & 0777);
#ಅನ್ಯಥಾ
			// Try to set the group of the backup same as the
			// original file. If this fails, set the protection
			// bits for the group same as the protection bits for
			// others.
			ಯದಿ (st_new.st_gid != st_old.st_gid
# ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
				&& fchown(bfd, (uid_t)-1, st_old.st_gid) != 0
# endif
						)
			    mch_setperm(backup,
					  (perm & 0707) | ((perm & 07) << 3));
# ಯದಿ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
# endif
# ifdef FEAT_XATTR
			mch_copy_xattr(fname, backup);
# endif
#endif

			// copy the file.
			write_info.bw_fd = bfd;
			write_info.bw_buf = copybuf;
			write_info.bw_flags = FIO_NOCONVERT;
			ಯಾವತ್ ((write_info.bw_len = read_eintr(fd, copybuf,
							    WRITEBUFSIZE)) > 0)
			{
			    ಯದಿ (buf_write_bytes(&write_info) == FAIL)
			    {
				errmsg = (char_u *)_(e_cant_write_to_backup_file_add_bang_to_override);
				ಭಙ್ಗ;
			    }
			    ui_breakcheck();
			    ಯದಿ (got_int)
			    {
				errmsg = (char_u *)_(e_interrupted);
				ಭಙ್ಗ;
			    }
			}

			ಯದಿ (close(bfd) < 0 && errmsg == ರಿಕ್ತ)
			    errmsg = (char_u *)_(e_close_error_for_backup_file_add_bang_to_write_anyway);
			ಯದಿ (write_info.bw_len < 0)
			    errmsg = (char_u *)_(e_cant_read_file_for_backup_add_bang_to_write_anyway);
#ifdef UNIX
			set_file_time(backup, st_old.st_atime, st_old.st_mtime);
#endif
#ifdef HAVE_ACL
			mch_set_acl(backup, acl);
#endif
#ಯದಿ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
#endif
#ifdef FEAT_XATTR
			mch_copy_xattr(fname, backup);
#endif
#ifdef MSWIN
			(ಶೂನ್ಯ)mch_copy_file_attribute(fname, backup);
#endif
			ಭಙ್ಗ;
		    }
		}
	    }
    nobackup:
	    close(fd);		// ignore errors for closing read file
	    vim_free(copybuf);

	    ಯದಿ (backup == ರಿಕ್ತ && errmsg == ರಿಕ್ತ)
		errmsg = (char_u *)_(e_cannot_create_backup_file_add_bang_to_write_anyway);
	    // ignore errors when forceit is TRUE
	    ಯದಿ ((some_error || errmsg != ರಿಕ್ತ) && !forceit)
	    {
		retval = FAIL;
		ಗಚ್ಛ fail;
	    }
	    errmsg = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    char_u	*dirp;
	    char_u	*p;
	    char_u	*rootname;

	    // Make a backup by renaming the original file.

	    // If 'cpoptions' includes the "W" flag, we don't want to
	    // overwrite a read-only file.  But rename may be possible
	    // anyway, thus we need an extra check here.
	    ಯದಿ (file_readonly && vim_strchr(p_cpo, CPO_FWRITE) != ರಿಕ್ತ)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_in_cpoptions);
		ಗಚ್ಛ fail;
	    }

	    // Form the backup file name - change path/fo.o.h to
	    // path/fo.o.h.bak Try all directories in 'backupdir', first one
	    // that works is used.
	    dirp = p_bdir;
	    ಯಾವತ್ (*dirp)
	    {
		ಪೂರ್ಣಾಙ್ಕ IObufflen UNUSED;

		// Isolate one directory name and make the backup file name.
		IObufflen = copy_option_part(&dirp, IObuff, IOSIZE, ",");

#ಯದಿ defined(UNIX) || defined(MSWIN)
		p = IObuff + IObufflen;
		ಯದಿ (after_pathsep(IObuff, p) && p[-1] == p[-2])
		    // path ends with '//', use full path
		    ಯದಿ ((p = make_percent_swname(IObuff, p, fname)) != ರಿಕ್ತ)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#endif
		ಯದಿ (backup == ರಿಕ್ತ)
		{
		    rootname = get_file_in_dir(fname, IObuff);
		    ಯದಿ (rootname == ರಿಕ್ತ)
			backup = ರಿಕ್ತ;
		    ಅನ್ಯಥಾ
		    {
			backup = buf_modname(
				(buf->b_p_sn || buf->b_shortname),
						rootname, backup_ext, FALSE);
			vim_free(rootname);
		    }
		}

		ಯದಿ (backup != ರಿಕ್ತ)
		{
		    // If we are not going to keep the backup file, don't
		    // delete an existing one, try to use another name.
		    // Change one character, just before the extension.
		    ಯದಿ (!p_bk && mch_getperm(backup) >= 0)
		    {
			p = backup + STRLEN(backup) - 1 - STRLEN(backup_ext);
			ಯದಿ (p < backup)	// empty file name ???
			    p = backup;
			*p = 'z';
			ಯಾವತ್ (*p > 'a' && mch_getperm(backup) >= 0)
			    --*p;
			// They all exist??? Must be something wrong!
			ಯದಿ (*p == 'a')
			    VIM_CLEAR(backup);
		    }
		}
		ಯದಿ (backup != ರಿಕ್ತ)
		{
		    // Delete any existing backup and move the current version
		    // to the backup.	For safety, we don't remove the backup
		    // until the write has finished successfully. And if the
		    // 'backup' option is set, leave it around.

		    // If the renaming of the original file to the backup file
		    // works, quit here.
		    ಯದಿ (vim_rename(fname, backup) == 0)
			ಭಙ್ಗ;

		    VIM_CLEAR(backup);   // don't do the rename below
		}
	    }
	    ಯದಿ (backup == ರಿಕ್ತ && !forceit)
	    {
		errmsg = (char_u *)_(e_cant_make_backup_file_add_bang_to_write_anyway);
		ಗಚ್ಛ fail;
	    }
	}
    }

#ಯದಿ defined(UNIX)
    // When using ":w!" and the file was read-only: make it writable
    ಯದಿ (forceit && perm >= 0 && !(perm & 0200) && st_old.st_uid == getuid()
				     && vim_strchr(p_cpo, CPO_FWRITE) == ರಿಕ್ತ)
    {
	perm |= 0200;
	(ಶೂನ್ಯ)mch_setperm(fname, perm);
	made_writable = TRUE;
    }
#endif

    // When using ":w!" and writing to the current file, 'readonly' makes no
    // sense, reset it, unless 'Z' appears in 'cpoptions'.
    ಯದಿ (forceit && overwriting && vim_strchr(p_cpo, CPO_KEEPRO) == ರಿಕ್ತ)
    {
	buf->b_p_ro = FALSE;
	need_maketitle = TRUE;	    // set window title later
	status_redraw_all();	    // redraw status lines later
    }

    ಯದಿ (end > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	end = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಯದಿ (buf->b_ml.ml_flags & ML_EMPTY)
	start = end + 1;

    // If the original file is being overwritten, there is a small chance that
    // we crash in the middle of writing. Therefore the file is preserved now.
    // This makes all block numbers positive so that recovery does not need
    // the original file.
    // Don't do this if there is a backup file and we are exiting.
    ಯದಿ (reset_changed && !newfile && overwriting
					      && !(exiting && backup != ರಿಕ್ತ))
    {
	ml_preserve(buf, FALSE);
	ಯದಿ (got_int)
	{
	    errmsg = (char_u *)_(e_interrupted);
	    ಗಚ್ಛ restore_backup;
	}
    }

#ifdef VMS
    vms_remove_version(fname); // remove version
#endif
    // Default: write the file directly.  May write to a temp file for
    // multi-byte conversion.
    wfname = fname;

    // Check for forced 'fileencoding' from "++opt=val" argument.
    ಯದಿ (eap != ರಿಕ್ತ && eap->force_enc != 0)
    {
	fenc = eap->cmd + eap->force_enc;
	fenc = enc_canonize(fenc);
	fenc_tofree = fenc;
    }
    ಅನ್ಯಥಾ
	fenc = buf->b_p_fenc;

    // Check if the file needs to be converted.
    converted = need_conversion(fenc);

    // Check if UTF-8 to UCS-2/4 or Latin1 conversion needs to be done.  Or
    // Latin1 to Unicode conversion.  This is handled in buf_write_bytes().
    // Prepare the flags for it and allocate bw_conv_buf when needed.
    ಯದಿ (converted && (enc_utf8 || STRCMP(p_enc, "latin1") == 0))
    {
	wb_flags = get_fio_flags(fenc);
	ಯದಿ (wb_flags & (FIO_UCS2 | FIO_UCS4 | FIO_UTF16 | FIO_UTF8))
	{
	    // overallocate a bit, in case we read incomplete multi-byte chars
	    ಪೂರ್ಣಾಙ್ಕ size = bufsize + CONV_RESTLEN;
	    // Need to allocate a buffer to translate into.
	    ಯದಿ (wb_flags & (FIO_UCS2 | FIO_UTF16 | FIO_UTF8))
		write_info.bw_conv_buflen = size * 2;
	    ಅನ್ಯಥಾ // FIO_UCS4
		write_info.bw_conv_buflen = size * 4;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    ಯದಿ (write_info.bw_conv_buf == ರಿಕ್ತ)
		end = 0;
	}
    }

#ifdef MSWIN
    ಯದಿ (converted && wb_flags == 0 && (wb_flags = get_win_fio_flags(fenc)) != 0)
    {
	// Convert UTF-8 -> UCS-2 and UCS-2 -> DBCS.  Worst-case * 4:
	write_info.bw_conv_buflen = bufsize * 4;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	ಯದಿ (write_info.bw_conv_buf == ರಿಕ್ತ)
	    end = 0;
    }
#endif

#ifdef MACOS_CONVERT
    ಯದಿ (converted && wb_flags == 0 && (wb_flags = get_mac_fio_flags(fenc)) != 0)
    {
	write_info.bw_conv_buflen = bufsize * 3;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	ಯದಿ (write_info.bw_conv_buf == ರಿಕ್ತ)
	    end = 0;
    }
#endif

#ಯದಿ defined(FEAT_EVAL) || defined(USE_ICONV)
    ಯದಿ (converted && wb_flags == 0)
    {
# ifdef USE_ICONV
	// Use iconv() conversion when conversion is needed and it's not done
	// internally.
	write_info.bw_iconv_fd = (iconv_t)my_iconv_open(fenc,
					enc_utf8 ? (char_u *)"utf-8" : p_enc);
	ಯದಿ (write_info.bw_iconv_fd != (iconv_t)-1)
	{
	    // We're going to use iconv(), allocate a buffer to convert in.
	    write_info.bw_conv_buflen = bufsize * ICONV_MULT;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    ಯದಿ (write_info.bw_conv_buf == ರಿಕ್ತ)
		end = 0;
	    write_info.bw_first = TRUE;
	}
#  ifdef FEAT_EVAL
	ಅನ್ಯಥಾ
#  endif
# endif

# ifdef FEAT_EVAL
	    // When the file needs to be converted with 'charconvert' after
	    // writing, write to a temp file instead and let the conversion
	    // overwrite the original file.
	    ಯದಿ (*p_ccv != NUL)
	    {
		wfname = vim_tempname('w', FALSE);
		ಯದಿ (wfname == ರಿಕ್ತ)	// Can't write without a tempfile!
		{
		    errmsg = (char_u *)_(e_cant_find_temp_file_for_writing);
		    ಗಚ್ಛ restore_backup;
		}
	    }
# endif
    }
#endif
    ಯದಿ (converted && wb_flags == 0
#ifdef USE_ICONV
	    && write_info.bw_iconv_fd == (iconv_t)-1
#endif
#ifdef FEAT_EVAL
	    && wfname == fname
#endif
	    )
    {
	ಯದಿ (!forceit)
	{
	    errmsg = (char_u *)_(e_cannot_convert_add_bang_to_write_without_conversion);
	    ಗಚ್ಛ restore_backup;
	}
	notconverted = TRUE;
    }

    // If conversion is taking place, we may first pretend to write and check
    // for conversion errors.  Then loop again to write for real.
    // When not doing conversion this writes for real right away.
    ಪ್ರತಿ (checking_conversion = TRUE; ; checking_conversion = FALSE)
    {
	// There is no need to check conversion when:
	// - there is no conversion
	// - we make a backup file, that can be restored in case of conversion
	//   failure.
	ಯದಿ (!converted || dobackup)
	    checking_conversion = FALSE;

	ಯದಿ (checking_conversion)
	{
	    // Make sure we don't write anything.
	    fd = -1;
	    write_info.bw_fd = fd;
	}
	ಅನ್ಯಥಾ
	{
#ifdef HAVE_FTRUNCATE
# define TRUNC_ON_OPEN 0
#ಅನ್ಯಥಾ
# define TRUNC_ON_OPEN O_TRUNC
#endif
	    // Open the file "wfname" for writing.
	    // We may try to open the file twice: If we can't write to the file
	    // and forceit is TRUE we delete the existing file and try to
	    // create a new one. If this still fails we may have lost the
	    // original file!  (this may happen when the user reached his
	    // quotum for number of files).
	    // Appending will fail if the file does not exist and forceit is
	    // FALSE.
	    ಯಾವತ್ ((fd = mch_open((ವರ್ಣ *)wfname, O_WRONLY | O_EXTRA | (append
				? (forceit ? (O_APPEND | O_CREAT) : O_APPEND)
				: (O_CREAT | TRUNC_ON_OPEN))
				, perm < 0 ? 0666 : (perm & 0777))) < 0)
	    {
		// A forced write will try to create a new file if the old one
		// is still readonly. This may also happen when the directory
		// is read-only. In that case the mch_remove() will fail.
		ಯದಿ (errmsg == ರಿಕ್ತ)
		{
#ifdef UNIX
		    stat_T	st;

		    // Don't delete the file when it's a hard or symbolic link.
		    ಯದಿ ((!newfile && st_old.st_nlink > 1)
			    || (mch_lstat((ವರ್ಣ *)fname, &st) == 0
				&& (st.st_dev != st_old.st_dev
				    || st.st_ino != st_old.st_ino)))
			errmsg =
			      (char_u *)_(e_cant_open_linked_file_for_writing);
		    ಅನ್ಯಥಾ
#endif
		    {
			errmsg = (char_u *)_(e_cant_open_file_for_writing);
			ಯದಿ (forceit && vim_strchr(p_cpo, CPO_FWRITE) == ರಿಕ್ತ
								  && perm >= 0)
			{
#ifdef UNIX
			    // we write to the file, thus it should be marked
			    // writable after all
			    ಯದಿ (!(perm & 0200))
				made_writable = TRUE;
			    perm |= 0200;
			    ಯದಿ (st_old.st_uid != getuid()
						  || st_old.st_gid != getgid())
				perm &= 0777;
#endif
			    ಯದಿ (!append)  // don't remove when appending
				mch_remove(wfname);
			    ಅನುವರ್ತ;
			}
		    }
		}

restore_backup:
		{
		    stat_T	st;

		    // If we failed to open the file, we don't need a backup.
		    // Throw it away.  If we moved or removed the original file
		    // try to put the backup in its place.
		    ಯದಿ (backup != ರಿಕ್ತ && wfname == fname)
		    {
			ಯದಿ (backup_copy)
			{
			    // There is a small chance that we removed the
			    // original, try to move the copy in its place.
			    // This may not work if the vim_rename() fails.
			    // In that case we leave the copy around.

			    // If file does not exist, put the copy in its
			    // place
			    ಯದಿ (mch_stat((ವರ್ಣ *)fname, &st) < 0)
				vim_rename(backup, fname);
			    // if original file does exist throw away the copy
			    ಯದಿ (mch_stat((ವರ್ಣ *)fname, &st) >= 0)
				mch_remove(backup);
			}
			ಅನ್ಯಥಾ
			{
			    // try to put the original file back
			    vim_rename(backup, fname);
			}
		    }

		    // if original file no longer exists give an extra warning
		    ಯದಿ (!newfile && mch_stat((ವರ್ಣ *)fname, &st) < 0)
			end = 0;
		}

		ಯದಿ (wfname != fname)
		    vim_free(wfname);
		ಗಚ್ಛ fail;
	    }
	    write_info.bw_fd = fd;

#ಯದಿ defined(UNIX)
	    {
		stat_T	st;

		// Double check we are writing the intended file before making
		// any changes.
		ಯದಿ (overwriting
			&& (!dobackup || backup_copy)
			&& fname == wfname
			&& perm >= 0
			&& mch_fstat(fd, &st) == 0
			&& st.st_ino != st_old.st_ino)
		{
		    close(fd);
		    errmsg = (char_u *)_(e_file_changed_while_writing);
		    ಗಚ್ಛ fail;
		}
	    }
#endif
#ifdef HAVE_FTRUNCATE
	    ಯದಿ (!append)
		vim_ignored = ftruncate(fd, (off_t)0);
#endif

#ಯದಿ defined(MSWIN)
	    ಯದಿ (backup != ರಿಕ್ತ && overwriting && !append)
		(ಶೂನ್ಯ)mch_copy_file_attribute(backup, wfname);

	    ಯದಿ (!overwriting && !append)
	    {
		ಯದಿ (buf->b_ffname != ರಿಕ್ತ)
		    (ಶೂನ್ಯ)mch_copy_file_attribute(buf->b_ffname, wfname);
		// Should copy resource fork
	    }
#endif

#ifdef FEAT_CRYPT
	    ಯದಿ (*buf->b_p_ಕೀಲಿ != NUL && !filtering)
	    {
		char_u		*header;
		ಪೂರ್ಣಾಙ್ಕ		header_len;

		buf->b_cryptstate = crypt_create_for_writing(
						      crypt_get_method_nr(buf),
					   buf->b_p_ಕೀಲಿ, &header, &header_len);
		ಯದಿ (buf->b_cryptstate == ರಿಕ್ತ || header == ರಿಕ್ತ)
		    end = 0;
		ಅನ್ಯಥಾ
		{
		    // Write magic number, so that Vim knows how this file is
		    // encrypted when reading it back.
		    write_info.bw_buf = header;
		    write_info.bw_len = header_len;
		    write_info.bw_flags = FIO_NOCONVERT;
		    ಯದಿ (buf_write_bytes(&write_info) == FAIL)
			end = 0;
		    wb_flags |= FIO_ENCRYPTED;
		    vim_free(header);
		}
	    }
#endif
	}
	errmsg = ರಿಕ್ತ;

	write_info.bw_buf = buffer;
	nchars = 0;

	// use "++bin", "++nobin" or 'binary'
	ಯದಿ (eap != ರಿಕ್ತ && eap->force_bin != 0)
	    write_bin = (eap->force_bin == FORCE_BIN);
	ಅನ್ಯಥಾ
	    write_bin = buf->b_p_bin;

	// The BOM is written just after the encryption magic number.
	// Skip it when appending and the file already existed, the BOM only
	// makes sense at the start of the file.
	ಯದಿ (buf->b_p_bomb && !write_bin && (!append || perm < 0))
	{
	    write_info.bw_len = make_bom(buffer, fenc);
	    ಯದಿ (write_info.bw_len > 0)
	    {
		// don't convert, do encryption
		write_info.bw_flags = FIO_NOCONVERT | wb_flags;
		ಯದಿ (buf_write_bytes(&write_info) == FAIL)
		    end = 0;
		ಅನ್ಯಥಾ
		    nchars += write_info.bw_len;
	    }
	}
	write_info.bw_start_lnum = start;

#ifdef FEAT_PERSISTENT_UNDO
	write_undo_file = (buf->b_p_udf
			    && overwriting
			    && !append
			    && !filtering
# ifdef CRYPT_NOT_INPLACE
			    // writing undo file requires
			    // crypt_encode_inplace()
			    && (buf->b_cryptstate == ರಿಕ್ತ
				|| crypt_works_inplace(buf->b_cryptstate))
# endif
			    && reset_changed
			    && !checking_conversion);
# ifdef CRYPT_NOT_INPLACE
	// remove undo file if encrypting it is not possible
	ಯದಿ (buf->b_p_udf
		&& overwriting
		&& !append
		&& !filtering
		&& !checking_conversion
		&& buf->b_cryptstate != ರಿಕ್ತ
		&& !crypt_works_inplace(buf->b_cryptstate))
	    u_undofile_reset_and_delete(buf);
# endif
	ಯದಿ (write_undo_file)
	    // Prepare for computing the hash value of the text.
	    sha256_start(&sha_ctx);
#endif

	write_info.bw_len = bufsize;
	write_info.bw_flags = wb_flags;
	fileformat = get_fileformat_force(buf, eap);
	s = buffer;
	len = 0;
	ಪ್ರತಿ (lnum = start; lnum <= end; ++lnum)
	{
	    // The next while loop is done once for each character written.
	    // Keep it fast!
	    ptr = ml_get_buf(buf, lnum, FALSE) - 1;
#ifdef FEAT_PERSISTENT_UNDO
	    ಯದಿ (write_undo_file)
		sha256_update(&sha_ctx, ptr + 1,
					      (UINT32_T)(STRLEN(ptr + 1) + 1));
#endif
	    ಯಾವತ್ ((c = *++ptr) != NUL)
	    {
		ಯದಿ (c == NL)
		    *s = NUL;		// replace newlines with NULs
		ಅನ್ಯಥಾ ಯದಿ (c == CAR && fileformat == EOL_MAC)
		    *s = NL;		// Mac: replace CRs with NLs
		ಅನ್ಯಥಾ
		    *s = c;
		++s;
		ಯದಿ (++len != bufsize)
		    ಅನುವರ್ತ;
#ifdef FEAT_CRYPT
		ಯದಿ (write_info.bw_fd > 0 && lnum == end
			&& (write_info.bw_flags & FIO_ENCRYPTED)
			&& *buf->b_p_ಕೀಲಿ != NUL && !filtering
			&& *ptr == NUL)
		    write_info.bw_finish = TRUE;
#endif
		ಯದಿ (buf_write_bytes(&write_info) == FAIL)
		{
		    end = 0;		// write error: break loop
		    ಭಙ್ಗ;
		}
		nchars += bufsize;
		s = buffer;
		len = 0;
		write_info.bw_start_lnum = lnum;
	    }
	    // write failed or last line has no EOL: stop here
	    ಯದಿ (end == 0
		    || (lnum == end
			&& (write_bin || !buf->b_p_fixeol)
			&& ((write_bin && lnum == buf->b_no_eol_lnum)
			    || (lnum == buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
							   && !buf->b_p_eol))))
	    {
		++lnum;			// written the line, count it
		no_eol = TRUE;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (fileformat == EOL_UNIX)
		*s++ = NL;
	    ಅನ್ಯಥಾ
	    {
		*s++ = CAR;		    // EOL_MAC or EOL_DOS: write CR
		ಯದಿ (fileformat == EOL_DOS)  // write CR-NL
		{
		    ಯದಿ (++len == bufsize)
		    {
			ಯದಿ (buf_write_bytes(&write_info) == FAIL)
			{
			    end = 0;	// write error: break loop
			    ಭಙ್ಗ;
			}
			nchars += bufsize;
			s = buffer;
			len = 0;
		    }
		    *s++ = NL;
		}
	    }
	    ಯದಿ (++len == bufsize && end)
	    {
		ಯದಿ (buf_write_bytes(&write_info) == FAIL)
		{
		    end = 0;		// write error: break loop
		    ಭಙ್ಗ;
		}
		nchars += bufsize;
		s = buffer;
		len = 0;

		ui_breakcheck();
		ಯದಿ (got_int)
		{
		    end = 0;		// Interrupted, break loop
		    ಭಙ್ಗ;
		}
	    }
#ifdef VMS
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
	    ಯದಿ (buf->b_fab_rfm == FAB$C_VFC
		    || ((buf->b_fab_rat & (FAB$M_FTN | FAB$M_CR)) != 0))
	    {
		ಪೂರ್ಣಾಙ್ಕ b2write;

		buf->b_fab_mrs = (buf->b_fab_mrs == 0
			? MIN(4096, bufsize)
			: MIN(buf->b_fab_mrs, bufsize));

		b2write = len;
		ಯಾವತ್ (b2write > 0)
		{
		    write_info.bw_len = MIN(b2write, buf->b_fab_mrs);
		    ಯದಿ (buf_write_bytes(&write_info) == FAIL)
		    {
			end = 0;
			ಭಙ್ಗ;
		    }
		    b2write -= MIN(b2write, buf->b_fab_mrs);
		}
		write_info.bw_len = bufsize;
		nchars += len;
		s = buffer;
		len = 0;
	    }
#endif
	}
	ಯದಿ (len > 0 && end > 0)
	{
	    write_info.bw_len = len;
#ifdef FEAT_CRYPT
	    ಯದಿ (write_info.bw_fd > 0 && lnum >= end
		    && (write_info.bw_flags & FIO_ENCRYPTED)
		    && *buf->b_p_ಕೀಲಿ != NUL && !filtering)
		write_info.bw_finish = TRUE;
#endif
	    ಯದಿ (buf_write_bytes(&write_info) == FAIL)
		end = 0;		    // write error
	    nchars += len;
	}

	ಯದಿ (!buf->b_p_fixeol && buf->b_p_eof)
	{
	    // write trailing CTRL-Z
	    (ಶೂನ್ಯ)write_eintr(write_info.bw_fd, "\x1a", 1);
	    nchars++;
	}

	// Stop when writing done or an error was encountered.
	ಯದಿ (!checking_conversion || end == 0)
	    ಭಙ್ಗ;

	// If no error happened until now, writing should be ok, so loop to
	// really write the buffer.
    }

    // If we started writing, finish writing. Also when an error was
    // encountered.
    ಯದಿ (!checking_conversion)
    {
#ಯದಿ defined(UNIX) && defined(HAVE_FSYNC)
	// On many journaling file systems there is a bug that causes both the
	// original and the backup file to be lost when halting the system
	// right after writing the file.  That's because only the meta-data is
	// journalled.  Syncing the file slows down the system, but assures it
	// has been written to disk and we don't lose it.
	// For a device do try the fsync() but don't complain if it does not
	// work (could be a pipe).
	// If the 'fsync' option is FALSE, don't fsync().  Useful for laptops.
	ಯದಿ ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && vim_fsync(fd) != 0
		&& !device)
	{
	    errmsg = (char_u *)_(e_fsync_failed);
	    end = 0;
	}
#endif

#ಯದಿ defined(HAVE_SELINUX) || defined(HAVE_SMACK) || defined(FEAT_XATTR)
	// Probably need to set the security context.
	ಯದಿ (!backup_copy)
	{
# ಯದಿ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
	    mch_copy_sec(backup, wfname);
# endif
# ifdef FEAT_XATTR
	    mch_copy_xattr(backup, wfname);
# endif
	}
#endif

#ifdef UNIX
	// When creating a new file, set its owner/group to that of the
	// original file.  Get the new device and inode number.
	ಯದಿ (backup != ರಿಕ್ತ && !backup_copy)
	{
# ifdef HAVE_FCHOWN
	    stat_T	st;

	    // Don't change the owner when it's already OK, some systems remove
	    // permission or ACL stuff.
	    ಯದಿ (mch_stat((ವರ್ಣ *)wfname, &st) < 0
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid)
	    {
		// changing owner might not be possible
		vim_ignored = fchown(fd, st_old.st_uid, -1);
		// if changing group fails clear the group permissions
		ಯದಿ (fchown(fd, -1, st_old.st_gid) == -1 && perm > 0)
		    perm &= ~070;
	    }
# endif
	    buf_setino(buf);
	}
	ಅನ್ಯಥಾ ಯದಿ (!buf->b_dev_valid)
	    // Set the inode when creating a new file.
	    buf_setino(buf);
#endif

#ifdef UNIX
	ಯದಿ (made_writable)
	    perm &= ~0200;	// reset 'w' bit for security reasons
#endif
#ifdef HAVE_FCHMOD
	// set permission of new file same as old file
	ಯದಿ (perm >= 0)
	    (ಶೂನ್ಯ)mch_fsetperm(fd, perm);
#endif
	ಯದಿ (close(fd) != 0)
	{
	    errmsg = (char_u *)_(e_close_failed);
	    end = 0;
	}

#ifndef HAVE_FCHMOD
	// set permission of new file same as old file
	ಯದಿ (perm >= 0)
	    (ಶೂನ್ಯ)mch_setperm(wfname, perm);
#endif
#ifdef HAVE_ACL
	// Probably need to set the ACL before changing the user (can't set the
	// ACL on a file the user doesn't own).
	// On Solaris, with ZFS and the aclmode property set to "discard" (the
	// default), chmod() discards all part of a file's ACL that don't
	// represent the mode of the file.  It's non-trivial for us to discover
	// whether we're in that situation, so we simply always re-set the ACL.
# ifndef HAVE_SOLARIS_ZFS_ACL
	ಯದಿ (!backup_copy)
# endif
	    mch_set_acl(wfname, acl);
#endif
#ifdef FEAT_CRYPT
	ಯದಿ (buf->b_cryptstate != ರಿಕ್ತ)
	{
	    crypt_free_state(buf->b_cryptstate);
	    buf->b_cryptstate = ರಿಕ್ತ;
	}
#endif

#ಯದಿ defined(FEAT_EVAL)
	ಯದಿ (wfname != fname)
	{
	    // The file was written to a temp file, now it needs to be
	    // converted with 'charconvert' to (overwrite) the output file.
	    ಯದಿ (end != 0)
	    {
		ಯದಿ (eval_charconvert(enc_utf8 ? (char_u *)"utf-8" : p_enc,
						  fenc, wfname, fname) == FAIL)
		{
		    write_info.bw_conv_error = TRUE;
		    end = 0;
		}
	    }
	    mch_remove(wfname);
	    vim_free(wfname);
	}
#endif
    }

    ಯದಿ (end == 0)
    {
	// Error encountered.
	ಯದಿ (errmsg == ರಿಕ್ತ)
	{
	    ಯದಿ (write_info.bw_conv_error)
	    {
		ಯದಿ (write_info.bw_conv_error_lnum == 0)
		    errmsg = (char_u *)_(e_write_error_conversion_failed_make_fenc_empty_to_override);
		ಅನ್ಯಥಾ
		{
		    errmsg_allocated = TRUE;
		    errmsg = alloc(300);
		    ಯದಿ (errmsg == ರಿಕ್ತ)
			ಗಚ್ಛ fail;
		    vim_snprintf((ವರ್ಣ *)errmsg, 300, _(e_write_error_conversion_failed_in_ಸಾಲು_nr_make_fenc_empty_to_override),
					 (long)write_info.bw_conv_error_lnum);
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (got_int)
		errmsg = (char_u *)_(e_interrupted);
	    ಅನ್ಯಥಾ
		errmsg = (char_u *)_(e_write_error_file_system_full);
	}

	// If we have a backup file, try to put it in place of the new file,
	// because the new file is probably corrupt.  This avoids losing the
	// original file when trying to make a backup when writing the file a
	// second time.
	// When "backup_copy" is set we need to copy the backup over the new
	// file.  Otherwise rename the backup file.
	// If this is OK, don't give the extra warning message.
	ಯದಿ (backup != ರಿಕ್ತ)
	{
	    ಯದಿ (backup_copy)
	    {
		// This may take a while, if we were interrupted let the user
		// know we got the message.
		ಯದಿ (got_int)
		{
		    ಸಂದೇಶ(_(e_interrupted));
		    out_flush();
		}
		ಯದಿ ((fd = mch_open((ವರ್ಣ *)backup, O_RDONLY | O_EXTRA, 0)) >= 0)
		{
		    ಯದಿ ((write_info.bw_fd = mch_open((ವರ್ಣ *)fname,
				    O_WRONLY | O_CREAT | O_TRUNC | O_EXTRA,
							   perm & 0777)) >= 0)
		    {
			// copy the file.
			write_info.bw_buf = smallbuf;
			write_info.bw_flags = FIO_NOCONVERT;
			ಯಾವತ್ ((write_info.bw_len = read_eintr(fd, smallbuf,
						      SMALLBUFSIZE)) > 0)
			    ಯದಿ (buf_write_bytes(&write_info) == FAIL)
				ಭಙ್ಗ;

			ಯದಿ (close(write_info.bw_fd) >= 0
						   && write_info.bw_len == 0)
			    end = 1;		// success
		    }
		    close(fd);	// ignore errors for closing read file
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (vim_rename(backup, fname) == 0)
		    end = 1;
	    }
	}
	ಗಚ್ಛ fail;
    }

    lnum -= start;	    // compute number of written lines
    --no_wait_return;	    // may wait for return now

#ಯದಿ !(defined(UNIX) || defined(VMS))
    fname = sfname;	    // use shortname now, for the messages
#endif
    ಯದಿ (!filtering)
    {
	ಸಂದೇಶ_add_fname(buf, fname);	// put fname in IObuff with quotes
	c = FALSE;
	ಯದಿ (write_info.bw_conv_error)
	{
	    STRCAT(IObuff, _(" CONVERSION ERROR"));
	    c = TRUE;
	    ಯದಿ (write_info.bw_conv_error_lnum != 0)
		vim_snprintf_add((ವರ್ಣ *)IObuff, IOSIZE, _(" in line %ld;"),
			(long)write_info.bw_conv_error_lnum);
	}
	ಅನ್ಯಥಾ ಯದಿ (notconverted)
	{
	    STRCAT(IObuff, _("[NOT converted]"));
	    c = TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (converted)
	{
	    STRCAT(IObuff, _("[converted]"));
	    c = TRUE;
	}
	ಯದಿ (device)
	{
	    STRCAT(IObuff, _("[Device]"));
	    c = TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (newfile)
	{
	    STRCAT(IObuff, new_file_message());
	    c = TRUE;
	}
	ಯದಿ (no_eol)
	{
	    ಸಂದೇಶ_add_eol();
	    c = TRUE;
	}
	// may add [unix/dos/mac]
	ಯದಿ (ಸಂದೇಶ_add_fileformat(fileformat))
	    c = TRUE;
#ifdef FEAT_CRYPT
	ಯದಿ (wb_flags & FIO_ENCRYPTED)
	{
	    crypt_append_ಸಂದೇಶ(buf);
	    c = TRUE;
	}
#endif
	ಸಂದೇಶ_add_ಸಾಲುಗಳು(c, (long)lnum, nchars);	// add line/char count
	ಯದಿ (!shortmess(SHM_WRITE))
	{
	    ಯದಿ (append)
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [a]") : _(" appended"));
	    ಅನ್ಯಥಾ
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [w]") : _(" written"));
	}

	set_keep_ಸಂದೇಶ((char_u *)ಸಂದೇಶ_trunc_attr((ವರ್ಣ *)IObuff, FALSE, 0), 0);
    }

    // When written everything correctly: reset 'modified'.  Unless not
    // writing to the original file and '+' is not in 'cpoptions'.
    ಯದಿ (reset_changed && whole && !append
	    && !write_info.bw_conv_error
	    && (overwriting || vim_strchr(p_cpo, CPO_PLUS) != ರಿಕ್ತ))
    {
	unchanged(buf, TRUE, FALSE);
	// b:changedtick may be incremented in unchanged() but that should not
	// trigger a TextChanged event.
	ಯದಿ (buf->b_last_changedtick + 1 == CHANGEDTICK(buf))
	    buf->b_last_changedtick = CHANGEDTICK(buf);
	u_unchanged(buf);
	u_update_save_nr(buf);
    }

    // If written to the current file, update the timestamp of the swap file
    // and reset the BF_WRITE_MASK flags. Also sets buf->b_mtime.
    ಯದಿ (overwriting)
    {
	ml_timestamp(buf);
	ಯದಿ (append)
	    buf->b_flags &= ~BF_NEW;
	ಅನ್ಯಥಾ
	    buf->b_flags &= ~BF_WRITE_MASK;
    }

    // If we kept a backup until now, and we are in patch mode, then we make
    // the backup file our 'original' file.
    ಯದಿ (*p_pm && dobackup)
    {
	ವರ್ಣ *org = (ವರ್ಣ *)buf_modname((buf->b_p_sn || buf->b_shortname),
							  fname, p_pm, FALSE);

	ಯದಿ (backup != ರಿಕ್ತ)
	{
	    stat_T	st;

	    // If the original file does not exist yet
	    // the current backup file becomes the original file
	    ಯದಿ (org == ರಿಕ್ತ)
		emsg(_(e_patchmode_cant_save_original_file));
	    ಅನ್ಯಥಾ ಯದಿ (mch_stat(org, &st) < 0)
	    {
		vim_rename(backup, (char_u *)org);
		VIM_CLEAR(backup);	    // don't delete the file
#ifdef UNIX
		set_file_time((char_u *)org, st_old.st_atime, st_old.st_mtime);
#endif
	    }
	}
	// If there is no backup file, remember that a (new) file was
	// created.
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ empty_fd;

	    ಯದಿ (org == ರಿಕ್ತ
		    || (empty_fd = mch_open(org,
				      O_CREAT | O_EXTRA | O_EXCL | O_NOFOLLOW,
					perm < 0 ? 0666 : (perm & 0777))) < 0)
	      emsg(_(e_patchmode_cant_touch_empty_original_file));
	    ಅನ್ಯಥಾ
	      close(empty_fd);
	}
	ಯದಿ (org != ರಿಕ್ತ)
	{
	    mch_setperm((char_u *)org, mch_getperm(fname) & 0777);
	    vim_free(org);
	}
    }

    // Remove the backup unless 'backup' option is set or there was a
    // conversion error.
    ಯದಿ (!p_bk && backup != ರಿಕ್ತ && !write_info.bw_conv_error
	    && mch_remove(backup) != 0)
	emsg(_(e_cant_delete_backup_file));

    ಗಚ್ಛ nofail;

    // Finish up.  We get here either after failure or success.
fail:
    --no_wait_return;		// may wait for return now
nofail:

    // Done saving, we accept changed buffer warnings again
    buf->b_saving = false;

    vim_free(backup);
    ಯದಿ (buffer != smallbuf)
	vim_free(buffer);
    vim_free(fenc_tofree);
    vim_free(write_info.bw_conv_buf);
#ifdef USE_ICONV
    ಯದಿ (write_info.bw_iconv_fd != (iconv_t)-1)
    {
	iconv_close(write_info.bw_iconv_fd);
	write_info.bw_iconv_fd = (iconv_t)-1;
    }
#endif
#ifdef HAVE_ACL
    mch_free_acl(acl);
#endif

    ಯದಿ (errmsg != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ numlen = errnum != ರಿಕ್ತ ? (ಪೂರ್ಣಾಙ್ಕ)STRLEN(errnum) : 0;

	attr = HL_ATTR(HLF_E);	// set highlight for error messages
	ಸಂದೇಶ_add_fname(buf,
#ifndef UNIX
		sfname
#ಅನ್ಯಥಾ
		fname
#endif
		     );		// put file name in IObuff with quotes
	ಯದಿ (STRLEN(IObuff) + STRLEN(errmsg) + numlen >= IOSIZE)
	    IObuff[IOSIZE - STRLEN(errmsg) - numlen - 1] = NUL;
	// If the error message has the form "is ...", put the error number in
	// front of the file name.
	ಯದಿ (errnum != ರಿಕ್ತ)
	{
	    STRMOVE(IObuff + numlen, IObuff);
	    mch_memmove(IObuff, errnum, (size_t)numlen);
	}
	STRCAT(IObuff, errmsg);
	emsg((ವರ್ಣ *)IObuff);
	ಯದಿ (errmsg_allocated)
	    vim_free(errmsg);

	retval = FAIL;
	ಯದಿ (end == 0)
	{
	    ಸಂದೇಶ_puts_attr(_("\nWARNING: Original file may be lost or damaged\n"),
		    attr | ಸಂದೇಶ_HIST);
	    ಸಂದೇಶ_puts_attr(_("don't quit the editor until the file is successfully written!"),
		    attr | ಸಂದೇಶ_HIST);

	    // Update the timestamp to avoid an "overwrite changed file"
	    // prompt when writing again.
	    ಯದಿ (mch_stat((ವರ್ಣ *)fname, &st_old) >= 0)
	    {
		buf_store_time(buf, &st_old, fname);
		buf->b_mtime_read = buf->b_mtime;
		buf->b_mtime_read_ns = buf->b_mtime_ns;
	    }
	}
    }
    ಸಂದೇಶ_scroll = ಸಂದೇಶ_save;

#ifdef FEAT_PERSISTENT_UNDO
    // When writing the whole file and 'undofile' is set, also write the undo
    // file.
    ಯದಿ (retval == OK && write_undo_file)
    {
	char_u	    hash[UNDO_HASH_SIZE];

	sha256_finish(&sha_ctx, hash);
	u_write_undo(ರಿಕ್ತ, FALSE, buf, hash);
    }
#endif

#ifdef FEAT_EVAL
    ಯದಿ (!should_abort(retval))
#ಅನ್ಯಥಾ
    ಯದಿ (!got_int)
#endif
    {
	aco_save_T	aco;

	curbuf->b_no_eol_lnum = 0;  // in case it was set by the previous read

	// Apply POST autocommands.
	// Careful: The autocommands may call buf_write() recursively!
	// Only do this when a window was found for "buf".
	aucmd_prepbuf(&aco, buf);
	ಯದಿ (curbuf == buf)
	{
	    ಯದಿ (append)
		apply_autocmds_exarg(EVENT_FILEAPPENDPOST, fname, fname,
							   FALSE, curbuf, eap);
	    ಅನ್ಯಥಾ ಯದಿ (filtering)
		apply_autocmds_exarg(EVENT_FILTERWRITEPOST, ರಿಕ್ತ, fname,
							   FALSE, curbuf, eap);
	    ಅನ್ಯಥಾ ಯದಿ (reset_changed && whole)
		apply_autocmds_exarg(EVENT_BUFWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);
	    ಅನ್ಯಥಾ
		apply_autocmds_exarg(EVENT_FILEWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}

#ifdef FEAT_EVAL
	ಯದಿ (aborting())	    // autocmds may abort script processing
	    retval = FALSE;
#endif
    }

#ifdef FEAT_VIMINFO
    // Make sure marks will be written out to the viminfo file later, even when
    // the file is new.
    curbuf->b_marks_read = true;
#endif

    got_int |= prev_got_int;

    ಪ್ರತ್ಯಾಗಮ retval;
}
