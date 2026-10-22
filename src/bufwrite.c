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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
# include <utime.h>		// for struct utimbuf
#అయితే_ముగింపు

#నిర్వచించు SMALLBUFSIZE	256	// size of emergency write buffer

/*
 * Structure to pass arguments from buf_write() to buf_write_bytes().
 */
నిర్మాణం bw_info
{
    పూర్ణాంకం		bw_fd;		// file descriptor
    char_u	*bw_buf;	// buffer with data to be written
    పూర్ణాంకం		bw_len;		// length of data
    పూర్ణాంకం		bw_flags;	// FIO_ flags
#నిర్వచితమైతే FEAT_CRYPT
    buf_T	*bw_బఫరు;	// buffer being written
    పూర్ణాంకం		bw_ముగింపు;	// finish encrypting
#అయితే_ముగింపు
    char_u	bw_rest[CONV_RESTLEN]; // not converted bytes
    పూర్ణాంకం		bw_restlen;	// nr of bytes in bw_rest[]
    పూర్ణాంకం		bw_మొదటి;	// first write call
    char_u	*bw_conv_buf;	// buffer for writing converted chars
    size_t	bw_conv_buflen; // size of bw_conv_buf
    పూర్ణాంకం		bw_conv_error;	// set for conversion error
    linenr_T	bw_conv_error_lnum;  // first line with error or zero
    linenr_T	bw_ప్రారంభం_lnum;	// line number at start of buffer
#నిర్వచితమైతే USE_ICONV
    iconv_t	bw_iconv_fd;	// descriptor for iconv() or -1
#అయితే_ముగింపు
};

/*
 * Convert a Unicode character to bytes.
 * Return TRUE for an error, FALSE when it's OK.
 */
    స్థిర పూర్ణాంకం
ucs2bytes(
    గుర్తులేని	c,		// in: character
    char_u	**pp,		// in/out: pointer to result
    పూర్ణాంకం		flags)		// FIO_ flags
{
    char_u	*p = *pp;
    పూర్ణాంకం		error = FALSE;
    పూర్ణాంకం		cc;


    ఒకవేళ (flags & FIO_UCS4)
    {
	ఒకవేళ (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	    *p++ = (c >> 16);
	    *p++ = (c >> 24);
	}
	లేకపోతే
	{
	    *p++ = (c >> 24);
	    *p++ = (c >> 16);
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    లేకపోతే ఒకవేళ (flags & (FIO_UCS2 | FIO_UTF16))
    {
	ఒకవేళ (c >= 0x10000)
	{
	    ఒకవేళ (flags & FIO_UTF16)
	    {
		// Make two words, ten bits of the character in each.  First
		// word is 0xd800 - 0xdbff, second one 0xdc00 - 0xdfff
		c -= 0x10000;
		ఒకవేళ (c >= 0x100000)
		    error = TRUE;
		cc = ((c >> 10) & 0x3ff) + 0xd800;
		ఒకవేళ (flags & FIO_ENDIAN_L)
		{
		    *p++ = cc;
		    *p++ = ((గుర్తులేని)cc >> 8);
		}
		లేకపోతే
		{
		    *p++ = ((గుర్తులేని)cc >> 8);
		    *p++ = cc;
		}
		c = (c & 0x3ff) + 0xdc00;
	    }
	    లేకపోతే
		error = TRUE;
	}
	ఒకవేళ (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	}
	లేకపోతే
	{
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    లేకపోతే    // Latin1
    {
	ఒకవేళ (c >= 0x100)
	{
	    error = TRUE;
	    *p++ = 0xBF;
	}
	లేకపోతే
	    *p++ = c;
    }

    *pp = p;
    తిరిగి error;
}

/*
 * Call write() to write a number of bytes to the file.
 * Handles encryption and 'encoding' conversion.
 *
 * Return FAIL for failure, OK otherwise.
 */
    స్థిర పూర్ణాంకం
buf_write_బైట్లు(నిర్మాణం bw_info *ip)
{
    పూర్ణాంకం		wlen;
    char_u	*buf = ip->bw_buf;	// data to write
    పూర్ణాంకం		len = ip->bw_len;	// length of data
    పూర్ణాంకం		flags = ip->bw_flags;	// extra flags

    // Skip conversion when writing the crypt magic number or the BOM.
    ఒకవేళ (!(flags & FIO_NOCONVERT))
    {
	char_u		*p;
	గుర్తులేని	c;
	పూర్ణాంకం		n;

	ఒకవేళ (flags & FIO_UTF8)
	{
	    // Convert latin1 in the buffer to UTF-8 in the file.
	    p = ip->bw_conv_buf;	// translate to buffer
	    కొరకు (wlen = 0; wlen < len; ++wlen)
		p += utf_char2bytes(buf[wlen], p);
	    buf = ip->bw_conv_buf;
	    len = (పూర్ణాంకం)(p - ip->bw_conv_buf);
	}
	లేకపోతే ఒకవేళ (flags & (FIO_UCS4 | FIO_UTF16 | FIO_UCS2 | FIO_LATIN1))
	{
	    // Convert UTF-8 bytes in the buffer to UCS-2, UCS-4, UTF-16 or
	    // Latin1 chars in the file.
	    ఒకవేళ (flags & FIO_LATIN1)
		p = buf;	// translate in-place (can only get shorter)
	    లేకపోతే
		p = ip->bw_conv_buf;	// translate to buffer
	    కొరకు (wlen = 0; wlen < len; wlen += n)
	    {
		ఒకవేళ (wlen == 0 && ip->bw_restlen != 0)
		{
		    పూర్ణాంకం		l;

		    // Use remainder of previous call.  Append the start of
		    // buf[] to get a full sequence.  Might still be too
		    // short!
		    l = CONV_RESTLEN - ip->bw_restlen;
		    ఒకవేళ (l > len)
			l = len;
		    mch_memmove(ip->bw_rest + ip->bw_restlen, buf, (size_t)l);
		    n = utf_ptr2len_len(ip->bw_rest, ip->bw_restlen + l);
		    ఒకవేళ (n > ip->bw_restlen + len)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			ఒకవేళ (ip->bw_restlen + len > CONV_RESTLEN)
			    తిరిగి FAIL;
			ip->bw_restlen += len;
			ఆపు;
		    }
		    ఒకవేళ (n > 1)
			c = utf_ptr2char(ip->bw_rest);
		    లేకపోతే
			c = ip->bw_rest[0];
		    ఒకవేళ (n >= ip->bw_restlen)
		    {
			n -= ip->bw_restlen;
			ip->bw_restlen = 0;
		    }
		    లేకపోతే
		    {
			ip->bw_restlen -= n;
			mch_memmove(ip->bw_rest, ip->bw_rest + n,
						      (size_t)ip->bw_restlen);
			n = 0;
		    }
		}
		లేకపోతే
		{
		    n = utf_ptr2len_len(buf + wlen, len - wlen);
		    ఒకవేళ (n > len - wlen)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			ఒకవేళ (len - wlen > CONV_RESTLEN)
			    తిరిగి FAIL;
			ip->bw_restlen = len - wlen;
			mch_memmove(ip->bw_rest, buf + wlen,
						      (size_t)ip->bw_restlen);
			ఆపు;
		    }
		    ఒకవేళ (n > 1)
			c = utf_ptr2char(buf + wlen);
		    లేకపోతే
			c = buf[wlen];
		}
		// Check that there is enough space
		ఒకవేళ (!(flags & FIO_LATIN1))
		{
		    size_t need = (flags & FIO_UCS4) ? 4 : 2;
		    ఒకవేళ ((flags & FIO_UTF16) && c >= 0x10000)
			need = 4;

		    ఒకవేళ ((size_t)(p - ip->bw_conv_buf) + need > ip->bw_conv_buflen)
			తిరిగి FAIL;
		}

		ఒకవేళ (ucs2bytes(c, &p, flags) && !ip->bw_conv_error)
		{
		    ip->bw_conv_error = TRUE;
		    ip->bw_conv_error_lnum = ip->bw_ప్రారంభం_lnum;
		}
		ఒకవేళ (c == NL)
		    ++ip->bw_ప్రారంభం_lnum;
	    }
	    ఒకవేళ (flags & FIO_LATIN1)
		len = (పూర్ణాంకం)(p - buf);
	    లేకపోతే
	    {
		buf = ip->bw_conv_buf;
		len = (పూర్ణాంకం)(p - ip->bw_conv_buf);
	    }
	}

#నిర్వచితమైతే MSWIN
	లేకపోతే ఒకవేళ (flags & FIO_CODEPAGE)
	{
	    // Convert UTF-8 or codepage to UCS-2 and then to MS-Windows
	    // codepage.
	    char_u	*from;
	    size_t	fromlen;
	    char_u	*గమ్యం;
	    పూర్ణాంకం		u8c;
	    BOOL	bad = FALSE;
	    పూర్ణాంకం		needed;

	    ఒకవేళ (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    లేకపోతే
	    {
		from = buf;
		fromlen = len;
	    }

	    గమ్యం = ip->bw_conv_buf;
	    ఒకవేళ (enc_utf8)
	    {
		// Convert from UTF-8 to UCS-2, to the start of the buffer.
		// The buffer has been allocated to be big enough.
		అయితే (fromlen > 0)
		{
		    n = (పూర్ణాంకం)utf_ptr2len_len(from, (పూర్ణాంకం)fromlen);
		    ఒకవేళ (n > (పూర్ణాంకం)fromlen)	// incomplete byte sequence
			ఆపు;
		    u8c = utf_ptr2char(from);
		    *గమ్యం++ = (u8c & 0xff);
		    *గమ్యం++ = (u8c >> 8);
		    fromlen -= n;
		    from += n;
		}

		// Copy remainder to ip->bw_rest[] to be used for the next
		// call.
		ఒకవేళ (fromlen > CONV_RESTLEN)
		{
		    // weird overlong sequence
		    ip->bw_conv_error = TRUE;
		    తిరిగి FAIL;
		}
		mch_memmove(ip->bw_rest, from, fromlen);
		ip->bw_restlen = (పూర్ణాంకం)fromlen;
	    }
	    లేకపోతే
	    {
		// Convert from enc_codepage to UCS-2, to the start of the
		// buffer.  The buffer has been allocated to be big enough.
		ip->bw_restlen = 0;
		needed = Multi_Byte_గమ్యం_Wide_Char(enc_codepage,
			     MB_ERR_INVALID_CHARS, (LPCSTR)from, (పూర్ణాంకం)fromlen,
								     ఖాళీ, 0);
		ఒకవేళ (needed == 0)
		{
		    // When conversion fails there may be a trailing byte.
		    needed = Multi_Byte_గమ్యం_Wide_Char(enc_codepage,
			 MB_ERR_INVALID_CHARS, (LPCSTR)from, (పూర్ణాంకం)fromlen - 1,
								     ఖాళీ, 0);
		    ఒకవేళ (needed == 0)
		    {
			// Conversion doesn't work.
			ip->bw_conv_error = TRUE;
			తిరిగి FAIL;
		    }
		    // Save the trailing byte for the next call.
		    ip->bw_rest[0] = from[fromlen - 1];
		    ip->bw_restlen = 1;
		}
		needed = Multi_Byte_గమ్యం_Wide_Char(enc_codepage, MB_ERR_INVALID_CHARS,
				(LPCSTR)from, (పూర్ణాంకం)(fromlen - ip->bw_restlen),
							  (LPWSTR)గమ్యం, needed);
		ఒకవేళ (needed == 0)
		{
		    // Safety check: Conversion doesn't work.
		    ip->bw_conv_error = TRUE;
		    తిరిగి FAIL;
		}
		గమ్యం += needed * 2;
	    }

	    fromlen = గమ్యం - ip->bw_conv_buf;
	    buf = గమ్యం;
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	    ఒకవేళ (FIO_GET_CP(flags) == CP_UTF8)
	    {
		// Convert from UCS-2 to UTF-8, using the remainder of the
		// conversion buffer.  Fails when out of space.
		కొరకు (from = ip->bw_conv_buf; fromlen > 1; fromlen -= 2)
		{
		    u8c = *from++;
		    u8c += (*from++ << 8);
		    గమ్యం += utf_char2bytes(u8c, గమ్యం);
		    ఒకవేళ (గమ్యం + 6 >= ip->bw_conv_buf + ip->bw_conv_buflen)
		    {
			ip->bw_conv_error = TRUE;
			తిరిగి FAIL;
		    }
		}
		len = (పూర్ణాంకం)(గమ్యం - buf);
	    }
	    లేకపోతే
# endif
	    {
		// Convert from UCS-2 to the codepage, using the remainder of
		// the conversion buffer.  If the conversion uses the default
		// character "0", the data doesn't fit in this encoding, so
		// fail.
		len = Wide_Char_గమ్యం_Multi_Byte(FIO_GET_CP(flags), 0,
			(LPCWSTR)ip->bw_conv_buf, (పూర్ణాంకం)fromlen / పరిమాణం(WCHAR),
			(LPSTR)గమ్యం, (పూర్ణాంకం)(ip->bw_conv_buflen - fromlen), 0,
									&bad);
		ఒకవేళ (bad)
		{
		    ip->bw_conv_error = TRUE;
		    తిరిగి FAIL;
		}
	    }
	}
#అయితే_ముగింపు

#నిర్వచితమైతే MACOS_CONVERT
	లేకపోతే ఒకవేళ (flags & FIO_MACROMAN)
	{
	    // Convert UTF-8 or latin1 to Apple MacRoman.
	    char_u	*from;
	    size_t	fromlen;

	    ఒకవేళ (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    లేకపోతే
	    {
		from = buf;
		fromlen = len;
	    }

	    ఒకవేళ (enc2macroman(from, fromlen,
			ip->bw_conv_buf, &len, ip->bw_conv_buflen,
			ip->bw_rest, &ip->bw_restlen) == FAIL)
	    {
		ip->bw_conv_error = TRUE;
		తిరిగి FAIL;
	    }
	    buf = ip->bw_conv_buf;
	}
#అయితే_ముగింపు

#నిర్వచితమైతే USE_ICONV
	ఒకవేళ (ip->bw_iconv_fd != (iconv_t)-1)
	{
	    స్థిరరాశి అక్షరం	*from;
	    size_t	fromlen;
	    అక్షరం	*గమ్యం;
	    size_t	tolen;

	    // Convert with iconv().
	    ఒకవేళ (ip->bw_restlen > 0)
	    {
		అక్షరం *fp;

		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		fp = (అక్షరం *)ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(fp, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(fp + ip->bw_restlen, buf, (size_t)len);
		from = fp;
		tolen = ip->bw_conv_buflen - fromlen;
	    }
	    లేకపోతే
	    {
		from = (స్థిరరాశి అక్షరం *)buf;
		fromlen = len;
		tolen = ip->bw_conv_buflen;
	    }
	    గమ్యం = (అక్షరం *)ip->bw_conv_buf;

	    ఒకవేళ (ip->bw_మొదటి)
	    {
		size_t	save_len = tolen;

		// output the initial shift state sequence
		(శూన్య)iconv(ip->bw_iconv_fd, ఖాళీ, ఖాళీ, &గమ్యం, &tolen);

		// There is a bug in iconv() on Linux (which appears to be
		// wide-spread) which sets "to" to NULL and messes up "tolen".
		ఒకవేళ (గమ్యం == ఖాళీ)
		{
		    గమ్యం = (అక్షరం *)ip->bw_conv_buf;
		    tolen = save_len;
		}
		ip->bw_మొదటి = FALSE;
	    }

	    // If iconv() has an error or there is not enough room, fail.
	    ఒకవేళ ((iconv(ip->bw_iconv_fd, (శూన్య *)&from, &fromlen, &గమ్యం, &tolen)
			== (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						    || fromlen > CONV_RESTLEN)
	    {
		ip->bw_conv_error = TRUE;
		తిరిగి FAIL;
	    }

	    // copy remainder to ip->bw_rest[] to be used for the next call.
	    ఒకవేళ (fromlen > 0)
		mch_memmove(ip->bw_rest, (శూన్య *)from, fromlen);
	    ip->bw_restlen = (పూర్ణాంకం)fromlen;

	    buf = ip->bw_conv_buf;
	    len = (పూర్ణాంకం)((char_u *)గమ్యం - ip->bw_conv_buf);
	}
#అయితే_ముగింపు
    }

    ఒకవేళ (ip->bw_fd < 0)
	// Only checking conversion, which is OK if we get here.
	తిరిగి OK;

#నిర్వచితమైతే FEAT_CRYPT
    ఒకవేళ (flags & FIO_ENCRYPTED)
    {
	// Encrypt the data. Do it in-place if possible, otherwise use an
	// allocated buffer.
# ifdef CRYPT_NOT_INPLACE
	ఒకవేళ (crypt_పనిచేస్తుంది_inplace(ip->bw_బఫరు->b_cryptstate))
	{
# endif
	    crypt_encode_inplace(ip->bw_బఫరు->b_cryptstate, buf, len,
								ip->bw_ముగింపు);
# ifdef CRYPT_NOT_INPLACE
	}
	లేకపోతే
	{
	    char_u *outbuf;

	    len = crypt_encode_alloc(curbuf->b_cryptstate, buf, len, &outbuf,
								ip->bw_ముగింపు);
	    ఒకవేళ (len == 0)
		తిరిగి OK;  // Crypt layer is buffering, will flush later.
	    wlen = write_eintr(ip->bw_fd, outbuf, len);
	    vim_free(outbuf);
	    తిరిగి (wlen < len) ? FAIL : OK;
	}
# endif
    }
#అయితే_ముగింపు

    wlen = write_eintr(ip->bw_fd, buf, len);
    తిరిగి (wlen < len) ? FAIL : OK;
}

/*
 * Check modification time of file, before writing to it.
 * The size isn't checked, because using a tool like "gzip" takes care of
 * using the same timestamp but can't set the size.
 */
    స్థిర పూర్ణాంకం
check_mtime(buf_T *buf, stat_T *st)
{
    ఒకవేళ (buf->b_mtime_read != 0
		  && సమయం_differs(st, buf->b_mtime_read, buf->b_mtime_read_ns))
    {
	సందేశం_scroll = TRUE;	    // don't overwrite messages here
	సందేశం_silent = 0;		    // must give this prompt
	// don't use emsg() here, don't want to flush the buffers
	సందేశం_attr(_("WARNING: The file has been changed since reading it!!!"),
						       HL_ATTR(HLF_E));
	ఒకవేళ (ask_yesno((char_u *)_("Do you really want to write to it"),
								 TRUE) == 'n')
	    తిరిగి FAIL;
	సందేశం_scroll = FALSE;	    // always overwrite the file message now
    }
    తిరిగి OK;
}

/*
 * Generate a BOM in "buf[4]" for encoding "name".
 * Return the length of the BOM (zero when no BOM).
 */
    స్థిర పూర్ణాంకం
make_bom(char_u *buf, char_u *name)
{
    పూర్ణాంకం		flags;
    char_u	*p;

    flags = get_fio_flags(name);

    // Can't put a BOM in a non-Unicode file.
    ఒకవేళ (flags == FIO_LATIN1 || flags == 0)
	తిరిగి 0;

    ఒకవేళ (flags == FIO_UTF8)	// UTF-8
    {
	buf[0] = 0xef;
	buf[1] = 0xbb;
	buf[2] = 0xbf;
	తిరిగి 3;
    }
    p = buf;
    (శూన్య)ucs2bytes(0xfeff, &p, flags);
    తిరిగి (పూర్ణాంకం)(p - buf);
}

#నిర్వచితమైతే UNIX
    స్థిర శూన్య
సమితి_file_సమయం(
    char_u  *fname,
    time_t  atime,	    // access time
    time_t  mtime)	    // modification time
{
# ఒకవేళ defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
    నిర్మాణం utimbuf  buf;

    buf.actime	= atime;
    buf.modtime	= mtime;
    (శూన్య)utime((అక్షరం *)fname, &buf);
# లేకపోతే
#  ఒకవేళ defined(HAVE_UTIMES)
    నిర్మాణం timeval  tvp[2];

    tvp[0].tv_sec   = atime;
    tvp[0].tv_usec  = 0;
    tvp[1].tv_sec   = mtime;
    tvp[1].tv_usec  = 0;
    (శూన్య)utimes((అక్షరం *)fname, (స్థిరరాశి నిర్మాణం timeval *)&tvp);
#  endif
# endif
}
#అయితే_ముగింపు // UNIX

    అక్షరం *
new_file_సందేశం(శూన్య)
{
    తిరిగి shortmess(SHM_NEW) ? _("[New]") : _("[New File]");
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
    పూర్ణాంకం
buf_write(
    buf_T	    *buf,
    char_u	    *fname,
    char_u	    *sfname,
    linenr_T	    ప్రారంభం,
    linenr_T	    చివర,
    exarg_T	    *eap,		// for forced 'ff' and 'fenc', can be
					// NULL!
    పూర్ణాంకం		    append,		// append to the file
    పూర్ణాంకం		    forceit,
    పూర్ణాంకం		    reset_మారింది,
    పూర్ణాంకం		    filtering)
{
    పూర్ణాంకం		    fd;
    char_u	    *backup = ఖాళీ;
    పూర్ణాంకం		    backup_copy = FALSE; // copy the original file?
    పూర్ణాంకం		    dobackup;
    char_u	    *ffname;
    char_u	    *wfname = ఖాళీ;	// name of file to write to
    char_u	    *s;
    char_u	    *ptr;
    char_u	    c;
    పూర్ణాంకం		    len;
    linenr_T	    lnum;
    దీర్ఘ	    nchars;
    char_u	    *errmsg = ఖాళీ;
    పూర్ణాంకం		    errmsg_allocated = FALSE;
    char_u	    *errnum = ఖాళీ;
    char_u	    *బఫరు;
    char_u	    smallbuf[SMALLBUFSIZE];
    char_u	    *backup_ext;
    పూర్ణాంకం		    bufsize;
    దీర్ఘ	    perm;		    // file permissions
    పూర్ణాంకం		    retval = OK;
    పూర్ణాంకం		    newfile = FALSE;	    // TRUE if file doesn't exist yet
    పూర్ణాంకం		    సందేశం_save = సందేశం_scroll;
    పూర్ణాంకం		    overwriting;	    // TRUE if writing over original
    పూర్ణాంకం		    no_eol = FALSE;	    // no end-of-line written
    పూర్ణాంకం		    device = FALSE;	    // writing to a device
    stat_T	    st_old;
    పూర్ణాంకం		    prev_got_int = got_int;
    పూర్ణాంకం		    checking_conversion;
    పూర్ణాంకం		    file_readonly = FALSE;  // overwritten file is read-only
#ఒకవేళ defined(UNIX)			    // XXX fix me sometime?
    పూర్ణాంకం		    made_writable = FALSE;  // 'w' bit has been set
#అయితే_ముగింపు
					// writing everything
    పూర్ణాంకం		    సంపూర్ణ = (ప్రారంభం == 1 && చివర == buf->b_ml.ml_పంక్తి_లెక్క);
    linenr_T	    old_పంక్తి_లెక్క = buf->b_ml.ml_పంక్తి_లెక్క;
    పూర్ణాంకం		    attr;
    పూర్ణాంకం		    fileformat;
    పూర్ణాంకం		    write_bin;
    నిర్మాణం bw_info  write_info;		// info for buf_write_bytes()
    పూర్ణాంకం		    converted = FALSE;
    పూర్ణాంకం		    notconverted = FALSE;
    char_u	    *fenc;		// effective 'fileencoding'
    char_u	    *fenc_tofree = ఖాళీ; // allocated "fenc"
    పూర్ణాంకం		    wb_flags = 0;
#నిర్వచితమైతే HAVE_ACL
    vim_acl_T	    acl = ఖాళీ;		// ACL copied from original file to
					// backup or new file
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
    పూర్ణాంకం		    write_undo_file = FALSE;
    context_sha256_T sha_ctx;
#అయితే_ముగింపు
    గుర్తులేని పూర్ణాంకం    bkc = get_bkc_flags(buf);
    pos_T	    orig_ప్రారంభం = buf->b_op_ప్రారంభం;
    pos_T	    orig_చివర = buf->b_op_చివర;

    ఒకవేళ (fname == ఖాళీ || *fname == NUL)	// safety check
	తిరిగి FAIL;
    ఒకవేళ (buf->b_ml.ml_mfp == ఖాళీ)
    {
	// This can happen during startup when there is a stray "w" in the
	// vimrc file.
	emsg(_(e_ఖాళీలు_బఫరు));
	తిరిగి FAIL;
    }

    // Disallow writing from .exrc and .vimrc in current directory for
    // security reasons.
    ఒకవేళ (check_secure())
	తిరిగి FAIL;

    // Avoid a crash for a long name.
    ఒకవేళ (STRLEN(fname) >= MAXPATHL)
    {
	emsg(_(e_name_too_long));
	తిరిగి FAIL;
    }

    // must init bw_conv_buf and bw_iconv_fd before jumping to "fail"
    write_info.bw_conv_buf = ఖాళీ;
    write_info.bw_conv_error = FALSE;
    write_info.bw_conv_error_lnum = 0;
    write_info.bw_restlen = 0;
#నిర్వచితమైతే USE_ICONV
    write_info.bw_iconv_fd = (iconv_t)-1;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
    write_info.bw_బఫరు = buf;
    write_info.bw_ముగింపు = FALSE;
#అయితే_ముగింపు

    // After writing a file changedtick changes but we don't want to display
    // the line.
    ex_no_reprint = TRUE;

    // If there is no file name yet, use the one for the written file.
    // BF_NOTEDITED is set to reflect this (in case the write fails).
    // Don't do this when the write is for a filter command.
    // Don't do this when appending.
    // Only do this when 'cpoptions' contains the 'F' flag.
    ఒకవేళ (buf->b_ffname == ఖాళీ
	    && reset_మారింది
	    && సంపూర్ణ
	    && buf == curbuf
	    && !bt_nofilename(buf)
	    && !filtering
	    && (!append || vim_strchr(p_cpo, CPO_FNAMEAPP) != ఖాళీ)
	    && vim_strchr(p_cpo, CPO_FNAMEW) != ఖాళీ)
    {
	ఒకవేళ (సమితి_rw_fname(fname, sfname) == FAIL)
	    తిరిగి FAIL;
	buf = curbuf;	    // just in case autocmds made "buf" invalid
    }

    ఒకవేళ (sfname == ఖాళీ)
	sfname = fname;
    // For Unix: Use the short file name whenever possible.
    // Avoids problems with networks and when directory names are changed.
    // Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
    // another directory, which we don't detect
    ffname = fname;			    // remember full fname
#నిర్వచితమైతే UNIX
    fname = sfname;
#అయితే_ముగింపు

    ఒకవేళ (buf->b_ffname != ఖాళీ && fnamecmp(ffname, buf->b_ffname) == 0)
	overwriting = TRUE;
    లేకపోతే
	overwriting = FALSE;

    ఒకవేళ (exiting)
	settmode(TMODE_COOK);	    // when exiting allow typeahead now

    ++no_wait_return;		    // don't wait for return yet

    // Set '[ and '] marks to the lines to be written.
    buf->b_op_ప్రారంభం.lnum = ప్రారంభం;
    buf->b_op_ప్రారంభం.నిలువు = 0;
    buf->b_op_చివర.lnum = చివర;
    buf->b_op_చివర.నిలువు = 0;

    {
	aco_save_T	aco;
	పూర్ణాంకం		buf_ffname = FALSE;
	పూర్ణాంకం		buf_sfname = FALSE;
	పూర్ణాంకం		buf_fname_f = FALSE;
	పూర్ణాంకం		buf_fname_s = FALSE;
	పూర్ణాంకం		did_cmd = FALSE;
	పూర్ణాంకం		nofile_err = FALSE;
	పూర్ణాంకం		ఖాళీలు_memline = (buf->b_ml.ml_mfp == ఖాళీ);
	bufref_T	bufref;

	// Apply PRE autocommands.
	// Set curbuf to the buffer to be written.
	// Careful: The autocommands may call buf_write() recursively!
	ఒకవేళ (ffname == buf->b_ffname)
	    buf_ffname = TRUE;
	ఒకవేళ (sfname == buf->b_sfname)
	    buf_sfname = TRUE;
	ఒకవేళ (fname == buf->b_ffname)
	    buf_fname_f = TRUE;
	ఒకవేళ (fname == buf->b_sfname)
	    buf_fname_s = TRUE;

	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, buf);
	ఒకవేళ (curbuf != buf)
	{
	    // Could not find a window for "buf".  Doing more might cause
	    // problems, better bail out.
	    తిరిగి FAIL;
	}

	సమితి_bufref(&bufref, buf);

	ఒకవేళ (append)
	{
	    ఒకవేళ (!(did_cmd = apply_autocmds_exarg(EVENT_FILEAPPENDCMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		ఒకవేళ (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		లేకపోతే
		    apply_autocmds_exarg(EVENT_FILEAPPENDPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	లేకపోతే ఒకవేళ (filtering)
	{
	    apply_autocmds_exarg(EVENT_FILTERWRITEPRE,
					    ఖాళీ, sfname, FALSE, curbuf, eap);
	}
	లేకపోతే ఒకవేళ (reset_మారింది && సంపూర్ణ)
	{
	    పూర్ణాంకం was_మారింది = curbuf_Is_మారింది();

	    did_cmd = apply_autocmds_exarg(EVENT_BUFWRITECMD,
					  sfname, sfname, FALSE, curbuf, eap);
	    ఒకవేళ (did_cmd)
	    {
		ఒకవేళ (was_మారింది && !curbuf_Is_మారింది())
		{
		    // Written everything correctly and BufWriteCmd has reset
		    // 'modified': Correct the undo information so that an
		    // undo now sets 'modified'.
		    u_unchanged(curbuf);
		    u_update_save_nr(curbuf);
		}
	    }
	    లేకపోతే
	    {
		ఒకవేళ (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		లేకపోతే
		    apply_autocmds_exarg(EVENT_BUFWRITEPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (!(did_cmd = apply_autocmds_exarg(EVENT_FILEWRITECMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		ఒకవేళ (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		లేకపోతే
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
	ఒకవేళ (!bufref_valid(&bufref))
	    buf = ఖాళీ;
	ఒకవేళ (buf == ఖాళీ || (buf->b_ml.ml_mfp == ఖాళీ && !ఖాళీలు_memline)
				       || did_cmd || nofile_err
#నిర్వచితమైతే FEAT_EVAL
				       || aborting()
#అయితే_ముగింపు
				       )
	{
	    ఒకవేళ (buf != ఖాళీ && (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	    {
		// restore the original '[ and '] positions
		buf->b_op_ప్రారంభం = orig_ప్రారంభం;
		buf->b_op_చివర = orig_చివర;
	    }

	    --no_wait_return;
	    సందేశం_scroll = సందేశం_save;
	    ఒకవేళ (nofile_err)
		semsg(_(e_no_matching_autocommands_for_buftype_str_బఫరు),
							       curbuf->b_p_bt);

	    ఒకవేళ (nofile_err
#నిర్వచితమైతే FEAT_EVAL
		    || aborting()
#అయితే_ముగింపు
		    )
		// An aborting error, interrupt or exception in the
		// autocommands.
		తిరిగి FAIL;
	    ఒకవేళ (did_cmd)
	    {
		ఒకవేళ (buf == ఖాళీ)
		    // The buffer was deleted.  We assume it was written
		    // (can't retry anyway).
		    తిరిగి OK;
		ఒకవేళ (overwriting)
		{
		    // Assume the buffer was written, update the timestamp.
		    ml_timestamp(buf);
		    ఒకవేళ (append)
			buf->b_flags &= ~BF_NEW;
		    లేకపోతే
			buf->b_flags &= ~BF_WRITE_ముసుగు;
		}
		ఒకవేళ (reset_మారింది && buf->b_మారింది && !append
			&& (overwriting || vim_strchr(p_cpo, CPO_PLUS) != ఖాళీ))
		    // Buffer still changed, the autocommands didn't work
		    // properly.
		    తిరిగి FAIL;
		తిరిగి OK;
	    }
#నిర్వచితమైతే FEAT_EVAL
	    ఒకవేళ (!aborting())
#అయితే_ముగింపు
		emsg(_(e_autocommands_deleted_or_unloaded_బఫరు_గమ్యం_be_written));
	    తిరిగి FAIL;
	}

	// The autocommands may have changed the number of lines in the file.
	// When writing the whole file, adjust the end.
	// When writing part of the file, assume that the autocommands only
	// changed the number of lines that are to be written (tricky!).
	ఒకవేళ (buf->b_ml.ml_పంక్తి_లెక్క != old_పంక్తి_లెక్క)
	{
	    ఒకవేళ (సంపూర్ణ)						// write all
		చివర = buf->b_ml.ml_పంక్తి_లెక్క;
	    లేకపోతే ఒకవేళ (buf->b_ml.ml_పంక్తి_లెక్క > old_పంక్తి_లెక్క)	// more lines
		చివర += buf->b_ml.ml_పంక్తి_లెక్క - old_పంక్తి_లెక్క;
	    లేకపోతే						// less lines
	    {
		చివర -= old_పంక్తి_లెక్క - buf->b_ml.ml_పంక్తి_లెక్క;
		ఒకవేళ (చివర < ప్రారంభం)
		{
		    --no_wait_return;
		    సందేశం_scroll = సందేశం_save;
		    emsg(_(e_autocommands_మారింది_సంఖ్య_యొక్క_పంక్తులు_లోపల_unexpected_way));
		    తిరిగి FAIL;
		}
	    }
	}

	// The autocommands may have changed the name of the buffer, which may
	// be kept in fname, ffname and sfname.  A ":cd" may also have dropped
	// the short name, then b_fname is the full name.
	ఒకవేళ (buf_ffname)
	    ffname = buf->b_ffname;
	ఒకవేళ (buf_sfname)
	    sfname = buf->b_fname;
	ఒకవేళ (buf_fname_f)
	    fname = buf->b_ffname;
	ఒకవేళ (buf_fname_s)
	    fname = buf->b_fname;
    }

    ఒకవేళ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	// restore the original '[ and '] positions
	buf->b_op_ప్రారంభం = orig_ప్రారంభం;
	buf->b_op_చివర = orig_చివర;
    }

#నిర్వచితమైతే FEAT_NETBEANS_INTG
    ఒకవేళ (netbeans_చురుకైన() && is_Netbeans_బఫరు(buf))
    {
	ఒకవేళ (సంపూర్ణ)
	{
	    // b_changed can be 0 after an undo, but we still need to write
	    // the buffer to NetBeans.
	    ఒకవేళ (buf->b_మారింది || isNetbeansModified(buf))
	    {
		--no_wait_return;		// may wait for return now
		సందేశం_scroll = సందేశం_save;
		netbeans_save_బఫరు(buf);	// no error checking...
		తిరిగి retval;
	    }
	    లేకపోతే
	    {
		errnum = (char_u *)"E656: ";
		errmsg = (char_u *)_(e_netbeans_disallows_writes_యొక్క_unmodified_buffers);
		బఫరు = ఖాళీ;
		వెళ్ళు fail;
	    }
	}
	లేకపోతే
	{
	    errnum = (char_u *)"E657: ";
	    errmsg = (char_u *)_(e_partial_writes_disallowed_for_netbeans_buffers);
	    బఫరు = ఖాళీ;
	    వెళ్ళు fail;
	}
    }
#అయితే_ముగింపు

    ఒకవేళ (shortmess(SHM_OVER) && !exiting)
	సందేశం_scroll = FALSE;	    // overwrite previous file message
    లేకపోతే
	సందేశం_scroll = TRUE;	    // don't overwrite previous file message
    ఒకవేళ (!filtering)
	filemess(buf,
#నిర్వచితం_కాకపోతే UNIX
		sfname,
#లేకపోతే
		fname,
#అయితే_ముగింపు
		    (char_u *)"", 0);	// show that we are busy
    సందేశం_scroll = FALSE;		    // always overwrite the file message now

    బఫరు = alloc(WRITEBUFSIZE);
    ఒకవేళ (బఫరు == ఖాళీ)		    // can't allocate big buffer, use small
				    // one (to be able to write when out of
				    // memory)
    {
	బఫరు = smallbuf;
	bufsize = SMALLBUFSIZE;
    }
    లేకపోతే
	bufsize = WRITEBUFSIZE;

    // Get information about original file (if there is one).
#ఒకవేళ defined(UNIX)
    st_old.st_dev = 0;
    st_old.st_ino = 0;
    perm = -1;
    ఒకవేళ (mch_stat((అక్షరం *)fname, &st_old) < 0)
	newfile = TRUE;
    లేకపోతే
    {
	perm = st_old.st_బహుళకం;
	ఒకవేళ (!S_ISREG(st_old.st_బహుళకం))		// not a file
	{
	    ఒకవేళ (S_ISDIR(st_old.st_బహుళకం))
	    {
		errnum = (char_u *)"E502: ";
		errmsg = (char_u *)_(e_is_a_directory);
		వెళ్ళు fail;
	    }
	    ఒకవేళ (mch_nodetype(fname) != కణుపు_WRITABLE)
	    {
		errnum = (char_u *)"E503: ";
		errmsg = (char_u *)_(e_is_not_file_or_writable_device);
		వెళ్ళు fail;
	    }
	    // It's a device of some kind (or a fifo) which we can write to
	    // but for which we can't make a backup.
	    device = TRUE;
	    newfile = TRUE;
	    perm = -1;
	}
    }
#లేకపోతే // !UNIX
    // Check for a writable device name.
    c = mch_nodetype(fname);
    ఒకవేళ (c == కణుపు_OTHER)
    {
	errnum = (char_u *)"E503: ";
	errmsg = (char_u *)_(e_is_not_file_or_writable_device);
	వెళ్ళు fail;
    }
    ఒకవేళ (c == కణుపు_WRITABLE)
    {
# ఒకవేళ defined(MSWIN)
	// MS-Windows allows opening a device, but we will probably get stuck
	// trying to write to it.
	ఒకవేళ (!p_odev)
	{
	    errnum = (char_u *)"E796: ";
	    errmsg = (char_u *)_(e_writing_గమ్యం_device_disabled_with_opendevice_option);
	    వెళ్ళు fail;
	}
# endif
	device = TRUE;
	newfile = TRUE;
	perm = -1;
    }
    లేకపోతే
    {
	perm = mch_getperm(fname);
	ఒకవేళ (perm < 0)
	    newfile = TRUE;
	లేకపోతే ఒకవేళ (mch_isdir(fname))
	{
	    errnum = (char_u *)"E502: ";
	    errmsg = (char_u *)_(e_is_a_directory);
	    వెళ్ళు fail;
	}
	ఒకవేళ (overwriting)
	    (శూన్య)mch_stat((అక్షరం *)fname, &st_old);
    }
#అయితే_ముగింపు // !UNIX

    ఒకవేళ (!device && !newfile)
    {
	// Check if the file is really writable (when renaming the file to
	// make a backup we won't discover it later).
	file_readonly = check_file_readonly(fname, (పూర్ణాంకం)perm);

	ఒకవేళ (!forceit && file_readonly)
	{
	    ఒకవేళ (vim_strchr(p_cpo, CPO_FWRITE) != ఖాళీ)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_లోపల_cpoptions);
	    }
	    లేకపోతే
	    {
		errnum = (char_u *)"E505: ";
		errmsg = (char_u *)_(e_is_read_only_కూడు_bang_గమ్యం_override);
	    }
	    వెళ్ళు fail;
	}

	// Check if the timestamp hasn't changed since reading the file.
	ఒకవేళ (overwriting)
	{
	    retval = check_mtime(buf, &st_old);
	    ఒకవేళ (retval == FAIL)
		వెళ్ళు fail;
	}
    }

#నిర్వచితమైతే HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    ఒకవేళ (!newfile)
	acl = mch_get_acl(fname);
#అయితే_ముగింపు

    // If 'backupskip' is not empty, don't make a backup for some files.
    dobackup = (p_wb || p_bk || *p_pm != NUL);
    ఒకవేళ (dobackup && *p_bsk != NUL && సరిపోలిక_file_list(p_bsk, sfname, ffname))
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
    ఒకవేళ (!(append && *p_pm == NUL) && !filtering && perm >= 0 && dobackup)
    {
#ఒకవేళ defined(UNIX) || defined(MSWIN)
	stat_T	    st;
#అయితే_ముగింపు

	ఒకవేళ ((bkc & BKC_YES) || append)	// "yes"
	    backup_copy = TRUE;
#ఒకవేళ defined(UNIX) || defined(MSWIN)
	లేకపోతే ఒకవేళ ((bkc & BKC_AUTO))	// "auto"
	{
# ifdef UNIX
	    // Don't rename the file when:
	    // - it's a hard link
	    // - it's a symbolic link
	    // - we don't have write permission in the directory
	    // - we can't set the owner/group of the new file
	    ఒకవేళ (st_old.st_nlink > 1
		    || mch_lstat((అక్షరం *)fname, &st) < 0
		    || st.st_dev != st_old.st_dev
		    || st.st_ino != st_old.st_ino
#  ifndef HAVE_FCHOWN
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid
#  endif
		    )
		backup_copy = TRUE;
	    లేకపోతే
# లేకపోతే
#  ifdef MSWIN
	    // On NTFS file systems hard links are possible.
	    ఒకవేళ (mch_is_linked(fname))
		backup_copy = TRUE;
	    లేకపోతే
#  endif
# endif
	    {
		size_t	dirlen;
		char_u	tmp_fname[MAXPATHL];
		పూర్ణాంకం	i;

		// Check if we can create a file and set the owner/group/mode to
		// the ones from the original file.
		// First find a file name that doesn't exist yet.  Use the
		// conventional hidden swap file name so that this short-lived
		// probe is covered by standard Vim ignore patterns.
		dirlen = (size_t)(gettail(fname) - fname);
		vim_strncpy(tmp_fname, fname, dirlen);
		fd = -1;
		కొరకు (i = 4913; ; i += 123)
		{
		    vim_snprintf((అక్షరం *)tmp_fname + dirlen,
			పరిమాణం(tmp_fname) - dirlen, ".%d.swp", i);
		    ఒకవేళ (mch_lstat((అక్షరం *)tmp_fname, &st) < 0)
		    {
			fd = mch_open((అక్షరం *)tmp_fname,
				    O_CREAT|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
			ఒకవేళ (fd < 0 && errno == EEXIST)
			    // If the same file name is created by another
			    // process between lstat() and open(), find another
			    // name.
			    కొనసాగు;
			ఆపు;
		    }
		}
		ఒకవేళ (fd < 0)	// can't write in directory
		    backup_copy = TRUE;
		లేకపోతే
		{
# ifdef UNIX
#  ifdef HAVE_FCHOWN
		    vim_ignored = fchown(fd, st_old.st_uid, st_old.st_gid);
#  endif
#  ifdef HAVE_FCHMOD
		    (శూన్య)mch_fsetperm(fd, perm);
#  లేకపోతే
		    (శూన్య)mch_setperm(tmp_fname, perm);
#  endif
		    ఒకవేళ (mch_stat((అక్షరం *)tmp_fname, &st) < 0
			    || st.st_uid != st_old.st_uid
			    || st.st_gid != st_old.st_gid
			    || (దీర్ఘ)st.st_బహుళకం != perm)
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
			పూర్ణాంకం try;

			కొరకు (try = 0; try < 10; ++try)
			{
			    ఒకవేళ (mch_lstat((అక్షరం *)tmp_fname, &st) < 0)
				ఆపు;
			    ui_delay(50L, TRUE);  // wait 50 msec
			    mch_remove(tmp_fname);
			}
		    }
# endif
		}
	    }
	}

	// Break symlinks and/or hardlinks if we've been asked to.
	ఒకవేళ ((bkc & BKC_BREAKSYMLINK) || (bkc & BKC_BREAKHARDLINK))
	{
# ifdef UNIX
	    పూర్ణాంకం	lstat_res;

	    lstat_res = mch_lstat((అక్షరం *)fname, &st);

	    // Symlinks.
	    ఒకవేళ ((bkc & BKC_BREAKSYMLINK)
		    && lstat_res == 0
		    && st.st_ino != st_old.st_ino)
		backup_copy = FALSE;

	    // Hardlinks.
	    ఒకవేళ ((bkc & BKC_BREAKHARDLINK)
		    && st_old.st_nlink > 1
		    && (lstat_res != 0 || st.st_ino == st_old.st_ino))
		backup_copy = FALSE;
# లేకపోతే
#  ఒకవేళ defined(MSWIN)
	    // Symlinks.
	    ఒకవేళ ((bkc & BKC_BREAKSYMLINK) && mch_is_symbolic_link(fname))
		backup_copy = FALSE;

	    // Hardlinks.
	    ఒకవేళ ((bkc & BKC_BREAKHARDLINK) && mch_is_hard_link(fname))
		backup_copy = FALSE;
#  endif
# endif
	}

#అయితే_ముగింపు

	// make sure we have a valid backup extension to use
	ఒకవేళ (*p_bex == NUL)
	    backup_ext = (char_u *)".bak";
	లేకపోతే
	    backup_ext = p_bex;

	ఒకవేళ (backup_copy
		&& (fd = mch_open((అక్షరం *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
	{
	    పూర్ణాంకం		bfd;
	    char_u	*copybuf;
	    పూర్ణాంకం		some_error = FALSE;
	    stat_T	st_new;
	    char_u	*dirp;
	    char_u	*rootname;
#ఒకవేళ defined(UNIX)
	    పూర్ణాంకం		did_సమితి_shortname;
	    mode_t	umask_save;
#అయితే_ముగింపు

	    copybuf = alloc(WRITEBUFSIZE + 1);
	    ఒకవేళ (copybuf == ఖాళీ)
	    {
		some_error = TRUE;	    // out of memory
		వెళ్ళు nobackup;
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
	    అయితే (*dirp)
	    {
		char_u	*p వాడని;
		పూర్ణాంకం	copybuf_len వాడని;

#నిర్వచితమైతే UNIX
		st_new.st_ino = 0;
		st_new.st_dev = 0;
		st_new.st_gid = 0;
#అయితే_ముగింపు

		// Isolate one directory name, using an entry in 'bdir'.
		copybuf_len = copy_option_part(&dirp, copybuf, WRITEBUFSIZE, ",");

#ఒకవేళ defined(UNIX) || defined(MSWIN)
		p = copybuf + copybuf_len;
		ఒకవేళ (after_pathsep(copybuf, p) && p[-1] == p[-2])
		    // Ends with '//', use full path
		    ఒకవేళ ((p = make_percent_swname(copybuf, p, fname)) != ఖాళీ)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#అయితే_ముగింపు
		rootname = get_file_లోపల_dir(fname, copybuf);
		ఒకవేళ (rootname == ఖాళీ)
		{
		    some_error = TRUE;	    // out of memory
		    వెళ్ళు nobackup;
		}

#ఒకవేళ defined(UNIX)
		did_సమితి_shortname = FALSE;
#అయితే_ముగింపు

		// May try twice if 'shortname' not set.
		కొరకు (;;)
		{
		    // Make the backup file name.
		    ఒకవేళ (backup == ఖాళీ)
			backup = buf_modname((buf->b_p_sn || buf->b_shortname),
						 rootname, backup_ext, FALSE);
		    ఒకవేళ (backup == ఖాళీ)
		    {
			vim_free(rootname);
			some_error = TRUE;		// out of memory
			వెళ్ళు nobackup;
		    }

		    // Check if backup file already exists.
		    ఒకవేళ (mch_stat((అక్షరం *)backup, &st_new) >= 0)
		    {
#నిర్వచితమైతే UNIX
			// Check if backup file is same as original file.
			// May happen when modname() gave the same file back.
			// E.g. silly link, or file name-length reached.
			// If we don't check here, we either ruin the file
			// when copying or erase it after writing. jw.
			ఒకవేళ (st_new.st_dev == st_old.st_dev
					    && st_new.st_ino == st_old.st_ino)
			{
			    VIM_CLEAR(backup);	// no backup file to delete
			    // may try again with 'shortname' set
			    ఒకవేళ (!(buf->b_shortname || buf->b_p_sn))
			    {
				buf->b_shortname = true;
				did_సమితి_shortname = TRUE;
				కొనసాగు;
			    }
				// setting shortname didn't help
			    ఒకవేళ (did_సమితి_shortname)
				buf->b_shortname = false;
			    ఆపు;
			}
#అయితే_ముగింపు

			// If we are not going to keep the backup file, don't
			// delete an existing one, try to use another name.
			// Change one character, just before the extension.
			ఒకవేళ (!p_bk)
			{
			    char_u	*wp;

			    wp = backup + STRLEN(backup) - 1
							 - STRLEN(backup_ext);
			    ఒకవేళ (wp < backup)	// empty file name ???
				wp = backup;
			    *wp = 'z';
			    అయితే (*wp > 'a'
				    && mch_stat((అక్షరం *)backup, &st_new) >= 0)
				--*wp;
			    // They all exist??? Must be something wrong.
			    ఒకవేళ (*wp == 'a')
				VIM_CLEAR(backup);
			}
		    }
		    ఆపు;
		}
		vim_free(rootname);

		// Try to create the backup file
		ఒకవేళ (backup != ఖాళీ)
		{
		    // remove old backup, if present
		    mch_remove(backup);
		    // Open with O_EXCL to avoid the file being created while
		    // we were sleeping (symlink hacker attack?). Reset umask
		    // if possible to avoid mch_setperm() below.
#నిర్వచితమైతే UNIX
		    umask_save = umask(0);
#అయితే_ముగింపు
		    bfd = mch_open((అక్షరం *)backup,
				O_WRONLY|O_CREAT|O_EXTRA|O_EXCL|O_NOFOLLOW,
								 perm & 0777);
#నిర్వచితమైతే UNIX
		    (శూన్య)umask(umask_save);
#అయితే_ముగింపు
		    ఒకవేళ (bfd < 0)
			VIM_CLEAR(backup);
		    లేకపోతే
		    {
			// Set file protection same as original file, but
			// strip s-bit.  Only needed if umask() wasn't used
			// above.
#నిర్వచితం_కాకపోతే UNIX
			(శూన్య)mch_setperm(backup, perm & 0777);
#లేకపోతే
			// Try to set the group of the backup same as the
			// original file. If this fails, set the protection
			// bits for the group same as the protection bits for
			// others.
			ఒకవేళ (st_new.st_gid != st_old.st_gid
# ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
				&& fchown(bfd, (uid_t)-1, st_old.st_gid) != 0
# endif
						)
			    mch_setperm(backup,
					  (perm & 0707) | ((perm & 07) << 3));
# ఒకవేళ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
# endif
# ifdef FEAT_XATTR
			mch_copy_xattr(fname, backup);
# endif
#అయితే_ముగింపు

			// copy the file.
			write_info.bw_fd = bfd;
			write_info.bw_buf = copybuf;
			write_info.bw_flags = FIO_NOCONVERT;
			అయితే ((write_info.bw_len = read_eintr(fd, copybuf,
							    WRITEBUFSIZE)) > 0)
			{
			    ఒకవేళ (buf_write_బైట్లు(&write_info) == FAIL)
			    {
				errmsg = (char_u *)_(e_cant_write_గమ్యం_backup_file_కూడు_bang_గమ్యం_override);
				ఆపు;
			    }
			    ui_breakcheck();
			    ఒకవేళ (got_int)
			    {
				errmsg = (char_u *)_(e_interrupted);
				ఆపు;
			    }
			}

			ఒకవేళ (close(bfd) < 0 && errmsg == ఖాళీ)
			    errmsg = (char_u *)_(e_close_error_for_backup_file_కూడు_bang_గమ్యం_write_anyway);
			ఒకవేళ (write_info.bw_len < 0)
			    errmsg = (char_u *)_(e_cant_read_file_for_backup_కూడు_bang_గమ్యం_write_anyway);
#నిర్వచితమైతే UNIX
			సమితి_file_సమయం(backup, st_old.st_atime, st_old.st_mtime);
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ACL
			mch_సమితి_acl(backup, acl);
#అయితే_ముగింపు
#ఒకవేళ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_XATTR
			mch_copy_xattr(fname, backup);
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
			(శూన్య)mch_copy_file_attribute(fname, backup);
#అయితే_ముగింపు
			ఆపు;
		    }
		}
	    }
    nobackup:
	    close(fd);		// ignore errors for closing read file
	    vim_free(copybuf);

	    ఒకవేళ (backup == ఖాళీ && errmsg == ఖాళీ)
		errmsg = (char_u *)_(e_cannot_create_backup_file_కూడు_bang_గమ్యం_write_anyway);
	    // ignore errors when forceit is TRUE
	    ఒకవేళ ((some_error || errmsg != ఖాళీ) && !forceit)
	    {
		retval = FAIL;
		వెళ్ళు fail;
	    }
	    errmsg = ఖాళీ;
	}
	లేకపోతే
	{
	    char_u	*dirp;
	    char_u	*p;
	    char_u	*rootname;

	    // Make a backup by renaming the original file.

	    // If 'cpoptions' includes the "W" flag, we don't want to
	    // overwrite a read-only file.  But rename may be possible
	    // anyway, thus we need an extra check here.
	    ఒకవేళ (file_readonly && vim_strchr(p_cpo, CPO_FWRITE) != ఖాళీ)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_లోపల_cpoptions);
		వెళ్ళు fail;
	    }

	    // Form the backup file name - change path/fo.o.h to
	    // path/fo.o.h.bak Try all directories in 'backupdir', first one
	    // that works is used.
	    dirp = p_bdir;
	    అయితే (*dirp)
	    {
		పూర్ణాంకం IObufflen వాడని;

		// Isolate one directory name and make the backup file name.
		IObufflen = copy_option_part(&dirp, IObuff, IOSIZE, ",");

#ఒకవేళ defined(UNIX) || defined(MSWIN)
		p = IObuff + IObufflen;
		ఒకవేళ (after_pathsep(IObuff, p) && p[-1] == p[-2])
		    // path ends with '//', use full path
		    ఒకవేళ ((p = make_percent_swname(IObuff, p, fname)) != ఖాళీ)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#అయితే_ముగింపు
		ఒకవేళ (backup == ఖాళీ)
		{
		    rootname = get_file_లోపల_dir(fname, IObuff);
		    ఒకవేళ (rootname == ఖాళీ)
			backup = ఖాళీ;
		    లేకపోతే
		    {
			backup = buf_modname(
				(buf->b_p_sn || buf->b_shortname),
						rootname, backup_ext, FALSE);
			vim_free(rootname);
		    }
		}

		ఒకవేళ (backup != ఖాళీ)
		{
		    // If we are not going to keep the backup file, don't
		    // delete an existing one, try to use another name.
		    // Change one character, just before the extension.
		    ఒకవేళ (!p_bk && mch_getperm(backup) >= 0)
		    {
			p = backup + STRLEN(backup) - 1 - STRLEN(backup_ext);
			ఒకవేళ (p < backup)	// empty file name ???
			    p = backup;
			*p = 'z';
			అయితే (*p > 'a' && mch_getperm(backup) >= 0)
			    --*p;
			// They all exist??? Must be something wrong!
			ఒకవేళ (*p == 'a')
			    VIM_CLEAR(backup);
		    }
		}
		ఒకవేళ (backup != ఖాళీ)
		{
		    // Delete any existing backup and move the current version
		    // to the backup.	For safety, we don't remove the backup
		    // until the write has finished successfully. And if the
		    // 'backup' option is set, leave it around.

		    // If the renaming of the original file to the backup file
		    // works, quit here.
		    ఒకవేళ (vim_rename(fname, backup) == 0)
			ఆపు;

		    VIM_CLEAR(backup);   // don't do the rename below
		}
	    }
	    ఒకవేళ (backup == ఖాళీ && !forceit)
	    {
		errmsg = (char_u *)_(e_cant_make_backup_file_కూడు_bang_గమ్యం_write_anyway);
		వెళ్ళు fail;
	    }
	}
    }

#ఒకవేళ defined(UNIX)
    // When using ":w!" and the file was read-only: make it writable
    ఒకవేళ (forceit && perm >= 0 && !(perm & 0200) && st_old.st_uid == getuid()
				     && vim_strchr(p_cpo, CPO_FWRITE) == ఖాళీ)
    {
	perm |= 0200;
	(శూన్య)mch_setperm(fname, perm);
	made_writable = TRUE;
    }
#అయితే_ముగింపు

    // When using ":w!" and writing to the current file, 'readonly' makes no
    // sense, reset it, unless 'Z' appears in 'cpoptions'.
    ఒకవేళ (forceit && overwriting && vim_strchr(p_cpo, CPO_KEEPRO) == ఖాళీ)
    {
	buf->b_p_ro = FALSE;
	need_maketitle = TRUE;	    // set window title later
	స్థితి_redraw_all();	    // redraw status lines later
    }

    ఒకవేళ (చివర > buf->b_ml.ml_పంక్తి_లెక్క)
	చివర = buf->b_ml.ml_పంక్తి_లెక్క;
    ఒకవేళ (buf->b_ml.ml_flags & ML_ఖాళీలు)
	ప్రారంభం = చివర + 1;

    // If the original file is being overwritten, there is a small chance that
    // we crash in the middle of writing. Therefore the file is preserved now.
    // This makes all block numbers positive so that recovery does not need
    // the original file.
    // Don't do this if there is a backup file and we are exiting.
    ఒకవేళ (reset_మారింది && !newfile && overwriting
					      && !(exiting && backup != ఖాళీ))
    {
	ml_preserve(buf, FALSE);
	ఒకవేళ (got_int)
	{
	    errmsg = (char_u *)_(e_interrupted);
	    వెళ్ళు restore_backup;
	}
    }

#నిర్వచితమైతే VMS
    vms_remove_సంస్కరణ(fname); // remove version
#అయితే_ముగింపు
    // Default: write the file directly.  May write to a temp file for
    // multi-byte conversion.
    wfname = fname;

    // Check for forced 'fileencoding' from "++opt=val" argument.
    ఒకవేళ (eap != ఖాళీ && eap->force_enc != 0)
    {
	fenc = eap->cmd + eap->force_enc;
	fenc = enc_canonize(fenc);
	fenc_tofree = fenc;
    }
    లేకపోతే
	fenc = buf->b_p_fenc;

    // Check if the file needs to be converted.
    converted = need_conversion(fenc);

    // Check if UTF-8 to UCS-2/4 or Latin1 conversion needs to be done.  Or
    // Latin1 to Unicode conversion.  This is handled in buf_write_bytes().
    // Prepare the flags for it and allocate bw_conv_buf when needed.
    ఒకవేళ (converted && (enc_utf8 || STRCMP(p_enc, "latin1") == 0))
    {
	wb_flags = get_fio_flags(fenc);
	ఒకవేళ (wb_flags & (FIO_UCS2 | FIO_UCS4 | FIO_UTF16 | FIO_UTF8))
	{
	    // overallocate a bit, in case we read incomplete multi-byte chars
	    పూర్ణాంకం కొలత = bufsize + CONV_RESTLEN;
	    // Need to allocate a buffer to translate into.
	    ఒకవేళ (wb_flags & (FIO_UCS2 | FIO_UTF16 | FIO_UTF8))
		write_info.bw_conv_buflen = కొలత * 2;
	    లేకపోతే // FIO_UCS4
		write_info.bw_conv_buflen = కొలత * 4;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    ఒకవేళ (write_info.bw_conv_buf == ఖాళీ)
		చివర = 0;
	}
    }

#నిర్వచితమైతే MSWIN
    ఒకవేళ (converted && wb_flags == 0 && (wb_flags = get_win_fio_flags(fenc)) != 0)
    {
	// Convert UTF-8 -> UCS-2 and UCS-2 -> DBCS.  Worst-case * 4:
	write_info.bw_conv_buflen = bufsize * 4;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	ఒకవేళ (write_info.bw_conv_buf == ఖాళీ)
	    చివర = 0;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే MACOS_CONVERT
    ఒకవేళ (converted && wb_flags == 0 && (wb_flags = get_mac_fio_flags(fenc)) != 0)
    {
	write_info.bw_conv_buflen = bufsize * 3;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	ఒకవేళ (write_info.bw_conv_buf == ఖాళీ)
	    చివర = 0;
    }
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL) || defined(USE_ICONV)
    ఒకవేళ (converted && wb_flags == 0)
    {
# ifdef USE_ICONV
	// Use iconv() conversion when conversion is needed and it's not done
	// internally.
	write_info.bw_iconv_fd = (iconv_t)my_iconv_open(fenc,
					enc_utf8 ? (char_u *)"utf-8" : p_enc);
	ఒకవేళ (write_info.bw_iconv_fd != (iconv_t)-1)
	{
	    // We're going to use iconv(), allocate a buffer to convert in.
	    write_info.bw_conv_buflen = bufsize * ICONV_MULT;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    ఒకవేళ (write_info.bw_conv_buf == ఖాళీ)
		చివర = 0;
	    write_info.bw_మొదటి = TRUE;
	}
#  ifdef FEAT_EVAL
	లేకపోతే
#  endif
# endif

# ifdef FEAT_EVAL
	    // When the file needs to be converted with 'charconvert' after
	    // writing, write to a temp file instead and let the conversion
	    // overwrite the original file.
	    ఒకవేళ (*p_ccv != NUL)
	    {
		wfname = vim_tempname('w', FALSE);
		ఒకవేళ (wfname == ఖాళీ)	// Can't write without a tempfile!
		{
		    errmsg = (char_u *)_(e_cant_కనుగొను_తాత్కాలిక_file_for_writing);
		    వెళ్ళు restore_backup;
		}
	    }
# endif
    }
#అయితే_ముగింపు
    ఒకవేళ (converted && wb_flags == 0
#నిర్వచితమైతే USE_ICONV
	    && write_info.bw_iconv_fd == (iconv_t)-1
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
	    && wfname == fname
#అయితే_ముగింపు
	    )
    {
	ఒకవేళ (!forceit)
	{
	    errmsg = (char_u *)_(e_cannot_convert_కూడు_bang_గమ్యం_write_without_conversion);
	    వెళ్ళు restore_backup;
	}
	notconverted = TRUE;
    }

    // If conversion is taking place, we may first pretend to write and check
    // for conversion errors.  Then loop again to write for real.
    // When not doing conversion this writes for real right away.
    కొరకు (checking_conversion = TRUE; ; checking_conversion = FALSE)
    {
	// There is no need to check conversion when:
	// - there is no conversion
	// - we make a backup file, that can be restored in case of conversion
	//   failure.
	ఒకవేళ (!converted || dobackup)
	    checking_conversion = FALSE;

	ఒకవేళ (checking_conversion)
	{
	    // Make sure we don't write anything.
	    fd = -1;
	    write_info.bw_fd = fd;
	}
	లేకపోతే
	{
#నిర్వచితమైతే HAVE_FTRUNCATE
# define TRUNC_ON_OPEN 0
#లేకపోతే
# define TRUNC_ON_OPEN O_TRUNC
#అయితే_ముగింపు
	    // Open the file "wfname" for writing.
	    // We may try to open the file twice: If we can't write to the file
	    // and forceit is TRUE we delete the existing file and try to
	    // create a new one. If this still fails we may have lost the
	    // original file!  (this may happen when the user reached his
	    // quotum for number of files).
	    // Appending will fail if the file does not exist and forceit is
	    // FALSE.
	    అయితే ((fd = mch_open((అక్షరం *)wfname, O_WRONLY | O_EXTRA | (append
				? (forceit ? (O_APPEND | O_CREAT) : O_APPEND)
				: (O_CREAT | TRUNC_ON_OPEN))
				, perm < 0 ? 0666 : (perm & 0777))) < 0)
	    {
		// A forced write will try to create a new file if the old one
		// is still readonly. This may also happen when the directory
		// is read-only. In that case the mch_remove() will fail.
		ఒకవేళ (errmsg == ఖాళీ)
		{
#నిర్వచితమైతే UNIX
		    stat_T	st;

		    // Don't delete the file when it's a hard or symbolic link.
		    ఒకవేళ ((!newfile && st_old.st_nlink > 1)
			    || (mch_lstat((అక్షరం *)fname, &st) == 0
				&& (st.st_dev != st_old.st_dev
				    || st.st_ino != st_old.st_ino)))
			errmsg =
			      (char_u *)_(e_cant_open_linked_file_for_writing);
		    లేకపోతే
#అయితే_ముగింపు
		    {
			errmsg = (char_u *)_(e_cant_open_file_for_writing);
			ఒకవేళ (forceit && vim_strchr(p_cpo, CPO_FWRITE) == ఖాళీ
								  && perm >= 0)
			{
#నిర్వచితమైతే UNIX
			    // we write to the file, thus it should be marked
			    // writable after all
			    ఒకవేళ (!(perm & 0200))
				made_writable = TRUE;
			    perm |= 0200;
			    ఒకవేళ (st_old.st_uid != getuid()
						  || st_old.st_gid != getgid())
				perm &= 0777;
#అయితే_ముగింపు
			    ఒకవేళ (!append)  // don't remove when appending
				mch_remove(wfname);
			    కొనసాగు;
			}
		    }
		}

restore_backup:
		{
		    stat_T	st;

		    // If we failed to open the file, we don't need a backup.
		    // Throw it away.  If we moved or removed the original file
		    // try to put the backup in its place.
		    ఒకవేళ (backup != ఖాళీ && wfname == fname)
		    {
			ఒకవేళ (backup_copy)
			{
			    // There is a small chance that we removed the
			    // original, try to move the copy in its place.
			    // This may not work if the vim_rename() fails.
			    // In that case we leave the copy around.

			    // If file does not exist, put the copy in its
			    //ఉంచు
			    ఒకవేళ (mch_stat((అక్షరం *)fname, &st) < 0)
				vim_rename(backup, fname);
			    // if original file does exist throw away the copy
			    ఒకవేళ (mch_stat((అక్షరం *)fname, &st) >= 0)
				mch_remove(backup);
			}
			లేకపోతే
			{
			    // try to put the original file back
			    vim_rename(backup, fname);
			}
		    }

		    // if original file no longer exists give an extra warning
		    ఒకవేళ (!newfile && mch_stat((అక్షరం *)fname, &st) < 0)
			చివర = 0;
		}

		ఒకవేళ (wfname != fname)
		    vim_free(wfname);
		వెళ్ళు fail;
	    }
	    write_info.bw_fd = fd;

#ఒకవేళ defined(UNIX)
	    {
		stat_T	st;

		// Double check we are writing the intended file before making
		// any changes.
		ఒకవేళ (overwriting
			&& (!dobackup || backup_copy)
			&& fname == wfname
			&& perm >= 0
			&& mch_fstat(fd, &st) == 0
			&& st.st_ino != st_old.st_ino)
		{
		    close(fd);
		    errmsg = (char_u *)_(e_file_మారింది_while_writing);
		    వెళ్ళు fail;
		}
	    }
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_FTRUNCATE
	    ఒకవేళ (!append)
		vim_ignored = ftruncate(fd, (off_t)0);
#అయితే_ముగింపు

#ఒకవేళ defined(MSWIN)
	    ఒకవేళ (backup != ఖాళీ && overwriting && !append)
		(శూన్య)mch_copy_file_attribute(backup, wfname);

	    ఒకవేళ (!overwriting && !append)
	    {
		ఒకవేళ (buf->b_ffname != ఖాళీ)
		    (శూన్య)mch_copy_file_attribute(buf->b_ffname, wfname);
		// Should copy resource fork
	    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CRYPT
	    ఒకవేళ (*buf->b_p_కీలకం != NUL && !filtering)
	    {
		char_u		*శీర్షిక;
		పూర్ణాంకం		శీర్షిక_len;

		buf->b_cryptstate = crypt_create_for_writing(
						      crypt_get_పద్ధతి_nr(buf),
					   buf->b_p_కీలకం, &శీర్షిక, &శీర్షిక_len);
		ఒకవేళ (buf->b_cryptstate == ఖాళీ || శీర్షిక == ఖాళీ)
		    చివర = 0;
		లేకపోతే
		{
		    // Write magic number, so that Vim knows how this file is
		    // encrypted when reading it back.
		    write_info.bw_buf = శీర్షిక;
		    write_info.bw_len = శీర్షిక_len;
		    write_info.bw_flags = FIO_NOCONVERT;
		    ఒకవేళ (buf_write_బైట్లు(&write_info) == FAIL)
			చివర = 0;
		    wb_flags |= FIO_ENCRYPTED;
		    vim_free(శీర్షిక);
		}
	    }
#అయితే_ముగింపు
	}
	errmsg = ఖాళీ;

	write_info.bw_buf = బఫరు;
	nchars = 0;

	// use "++bin", "++nobin" or 'binary'
	ఒకవేళ (eap != ఖాళీ && eap->force_bin != 0)
	    write_bin = (eap->force_bin == FORCE_BIN);
	లేకపోతే
	    write_bin = buf->b_p_bin;

	// The BOM is written just after the encryption magic number.
	// Skip it when appending and the file already existed, the BOM only
	// makes sense at the start of the file.
	ఒకవేళ (buf->b_p_bomb && !write_bin && (!append || perm < 0))
	{
	    write_info.bw_len = make_bom(బఫరు, fenc);
	    ఒకవేళ (write_info.bw_len > 0)
	    {
		// don't convert, do encryption
		write_info.bw_flags = FIO_NOCONVERT | wb_flags;
		ఒకవేళ (buf_write_బైట్లు(&write_info) == FAIL)
		    చివర = 0;
		లేకపోతే
		    nchars += write_info.bw_len;
	    }
	}
	write_info.bw_ప్రారంభం_lnum = ప్రారంభం;

#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
	write_undo_file = (buf->b_p_udf
			    && overwriting
			    && !append
			    && !filtering
# ifdef CRYPT_NOT_INPLACE
			    // writing undo file requires
			    // crypt_encode_inplace()
			    && (buf->b_cryptstate == ఖాళీ
				|| crypt_పనిచేస్తుంది_inplace(buf->b_cryptstate))
# endif
			    && reset_మారింది
			    && !checking_conversion);
# ifdef CRYPT_NOT_INPLACE
	// remove undo file if encrypting it is not possible
	ఒకవేళ (buf->b_p_udf
		&& overwriting
		&& !append
		&& !filtering
		&& !checking_conversion
		&& buf->b_cryptstate != ఖాళీ
		&& !crypt_పనిచేస్తుంది_inplace(buf->b_cryptstate))
	    u_undofile_reset_and_delete(buf);
# endif
	ఒకవేళ (write_undo_file)
	    // Prepare for computing the hash value of the text.
	    sha256_ప్రారంభం(&sha_ctx);
#అయితే_ముగింపు

	write_info.bw_len = bufsize;
	write_info.bw_flags = wb_flags;
	fileformat = get_fileformat_force(buf, eap);
	s = బఫరు;
	len = 0;
	కొరకు (lnum = ప్రారంభం; lnum <= చివర; ++lnum)
	{
	    // The next while loop is done once for each character written.
	    // Keep it fast!
	    ptr = ml_get_buf(buf, lnum, FALSE) - 1;
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
	    ఒకవేళ (write_undo_file)
		sha256_update(&sha_ctx, ptr + 1,
					      (UINT32_T)(STRLEN(ptr + 1) + 1));
#అయితే_ముగింపు
	    అయితే ((c = *++ptr) != NUL)
	    {
		ఒకవేళ (c == NL)
		    *s = NUL;		// replace newlines with NULs
		లేకపోతే ఒకవేళ (c == CAR && fileformat == EOL_MAC)
		    *s = NL;		// Mac: replace CRs with NLs
		లేకపోతే
		    *s = c;
		++s;
		ఒకవేళ (++len != bufsize)
		    కొనసాగు;
#నిర్వచితమైతే FEAT_CRYPT
		ఒకవేళ (write_info.bw_fd > 0 && lnum == చివర
			&& (write_info.bw_flags & FIO_ENCRYPTED)
			&& *buf->b_p_కీలకం != NUL && !filtering
			&& *ptr == NUL)
		    write_info.bw_ముగింపు = TRUE;
#అయితే_ముగింపు
		ఒకవేళ (buf_write_బైట్లు(&write_info) == FAIL)
		{
		    చివర = 0;		// write error: break loop
		    ఆపు;
		}
		nchars += bufsize;
		s = బఫరు;
		len = 0;
		write_info.bw_ప్రారంభం_lnum = lnum;
	    }
	    // write failed or last line has no EOL: stop here
	    ఒకవేళ (చివర == 0
		    || (lnum == చివర
			&& (write_bin || !buf->b_p_fixeol)
			&& ((write_bin && lnum == buf->b_no_eol_lnum)
			    || (lnum == buf->b_ml.ml_పంక్తి_లెక్క
							   && !buf->b_p_eol))))
	    {
		++lnum;			// written the line, count it
		no_eol = TRUE;
		ఆపు;
	    }
	    ఒకవేళ (fileformat == EOL_UNIX)
		*s++ = NL;
	    లేకపోతే
	    {
		*s++ = CAR;		    // EOL_MAC or EOL_DOS: write CR
		ఒకవేళ (fileformat == EOL_DOS)  // write CR-NL
		{
		    ఒకవేళ (++len == bufsize)
		    {
			ఒకవేళ (buf_write_బైట్లు(&write_info) == FAIL)
			{
			    చివర = 0;	// write error: break loop
			    ఆపు;
			}
			nchars += bufsize;
			s = బఫరు;
			len = 0;
		    }
		    *s++ = NL;
		}
	    }
	    ఒకవేళ (++len == bufsize && చివర)
	    {
		ఒకవేళ (buf_write_బైట్లు(&write_info) == FAIL)
		{
		    చివర = 0;		// write error: break loop
		    ఆపు;
		}
		nchars += bufsize;
		s = బఫరు;
		len = 0;

		ui_breakcheck();
		ఒకవేళ (got_int)
		{
		    చివర = 0;		// Interrupted, break loop
		    ఆపు;
		}
	    }
#నిర్వచితమైతే VMS
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
	    ఒకవేళ (buf->b_fab_rfm == FAB$C_VFC
		    || ((buf->b_fab_rat & (FAB$M_FTN | FAB$M_CR)) != 0))
	    {
		పూర్ణాంకం b2write;

		buf->b_fab_mrs = (buf->b_fab_mrs == 0
			? MIN(4096, bufsize)
			: MIN(buf->b_fab_mrs, bufsize));

		b2write = len;
		అయితే (b2write > 0)
		{
		    write_info.bw_len = MIN(b2write, buf->b_fab_mrs);
		    ఒకవేళ (buf_write_బైట్లు(&write_info) == FAIL)
		    {
			చివర = 0;
			ఆపు;
		    }
		    b2write -= MIN(b2write, buf->b_fab_mrs);
		}
		write_info.bw_len = bufsize;
		nchars += len;
		s = బఫరు;
		len = 0;
	    }
#అయితే_ముగింపు
	}
	ఒకవేళ (len > 0 && చివర > 0)
	{
	    write_info.bw_len = len;
#నిర్వచితమైతే FEAT_CRYPT
	    ఒకవేళ (write_info.bw_fd > 0 && lnum >= చివర
		    && (write_info.bw_flags & FIO_ENCRYPTED)
		    && *buf->b_p_కీలకం != NUL && !filtering)
		write_info.bw_ముగింపు = TRUE;
#అయితే_ముగింపు
	    ఒకవేళ (buf_write_బైట్లు(&write_info) == FAIL)
		చివర = 0;		    // write error
	    nchars += len;
	}

	ఒకవేళ (!buf->b_p_fixeol && buf->b_p_eof)
	{
	    // write trailing CTRL-Z
	    (శూన్య)write_eintr(write_info.bw_fd, "\x1a", 1);
	    nchars++;
	}

	// Stop when writing done or an error was encountered.
	ఒకవేళ (!checking_conversion || చివర == 0)
	    ఆపు;

	// If no error happened until now, writing should be ok, so loop to
	// really write the buffer.
    }

    // If we started writing, finish writing. Also when an error was
    // encountered.
    ఒకవేళ (!checking_conversion)
    {
#ఒకవేళ defined(UNIX) && defined(HAVE_FSYNC)
	// On many journaling file systems there is a bug that causes both the
	// original and the backup file to be lost when halting the system
	// right after writing the file.  That's because only the meta-data is
	// journalled.  Syncing the file slows down the system, but assures it
	// has been written to disk and we don't lose it.
	// For a device do try the fsync() but don't complain if it does not
	// work (could be a pipe).
	// If the 'fsync' option is FALSE, don't fsync().  Useful for laptops.
	ఒకవేళ ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && vim_fsync(fd) != 0
		&& !device)
	{
	    errmsg = (char_u *)_(e_fsync_failed);
	    చివర = 0;
	}
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_SELINUX) || defined(HAVE_SMACK) || defined(FEAT_XATTR)
	// Probably need to set the security context.
	ఒకవేళ (!backup_copy)
	{
# ఒకవేళ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
	    mch_copy_sec(backup, wfname);
# endif
# ifdef FEAT_XATTR
	    mch_copy_xattr(backup, wfname);
# endif
	}
#అయితే_ముగింపు

#నిర్వచితమైతే UNIX
	// When creating a new file, set its owner/group to that of the
	// original file.  Get the new device and inode number.
	ఒకవేళ (backup != ఖాళీ && !backup_copy)
	{
# ifdef HAVE_FCHOWN
	    stat_T	st;

	    // Don't change the owner when it's already OK, some systems remove
	    // permission or ACL stuff.
	    ఒకవేళ (mch_stat((అక్షరం *)wfname, &st) < 0
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid)
	    {
		// changing owner might not be possible
		vim_ignored = fchown(fd, st_old.st_uid, -1);
		// if changing group fails clear the group permissions
		ఒకవేళ (fchown(fd, -1, st_old.st_gid) == -1 && perm > 0)
		    perm &= ~070;
	    }
# endif
	    buf_setino(buf);
	}
	లేకపోతే ఒకవేళ (!buf->b_dev_valid)
	    // Set the inode when creating a new file.
	    buf_setino(buf);
#అయితే_ముగింపు

#నిర్వచితమైతే UNIX
	ఒకవేళ (made_writable)
	    perm &= ~0200;	// reset 'w' bit for security reasons
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_FCHMOD
	// set permission of new file same as old file
	ఒకవేళ (perm >= 0)
	    (శూన్య)mch_fsetperm(fd, perm);
#అయితే_ముగింపు
	ఒకవేళ (close(fd) != 0)
	{
	    errmsg = (char_u *)_(e_close_failed);
	    చివర = 0;
	}

#నిర్వచితం_కాకపోతే HAVE_FCHMOD
	// set permission of new file same as old file
	ఒకవేళ (perm >= 0)
	    (శూన్య)mch_setperm(wfname, perm);
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ACL
	// Probably need to set the ACL before changing the user (can't set the
	// ACL on a file the user doesn't own).
	// On Solaris, with ZFS and the aclmode property set to "discard" (the
	// default), chmod() discards all part of a file's ACL that don't
	// represent the mode of the file.  It's non-trivial for us to discover
	// whether we're in that situation, so we simply always re-set the ACL.
# ifndef HAVE_SOLARIS_ZFS_ACL
	ఒకవేళ (!backup_copy)
# endif
	    mch_సమితి_acl(wfname, acl);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
	ఒకవేళ (buf->b_cryptstate != ఖాళీ)
	{
	    crypt_free_state(buf->b_cryptstate);
	    buf->b_cryptstate = ఖాళీ;
	}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
	ఒకవేళ (wfname != fname)
	{
	    // The file was written to a temp file, now it needs to be
	    // converted with 'charconvert' to (overwrite) the output file.
	    ఒకవేళ (చివర != 0)
	    {
		ఒకవేళ (eval_charconvert(enc_utf8 ? (char_u *)"utf-8" : p_enc,
						  fenc, wfname, fname) == FAIL)
		{
		    write_info.bw_conv_error = TRUE;
		    చివర = 0;
		}
	    }
	    mch_remove(wfname);
	    vim_free(wfname);
	}
#అయితే_ముగింపు
    }

    ఒకవేళ (చివర == 0)
    {
	// Error encountered.
	ఒకవేళ (errmsg == ఖాళీ)
	{
	    ఒకవేళ (write_info.bw_conv_error)
	    {
		ఒకవేళ (write_info.bw_conv_error_lnum == 0)
		    errmsg = (char_u *)_(e_write_error_conversion_failed_make_fenc_ఖాళీలు_గమ్యం_override);
		లేకపోతే
		{
		    errmsg_allocated = TRUE;
		    errmsg = alloc(300);
		    ఒకవేళ (errmsg == ఖాళీ)
			వెళ్ళు fail;
		    vim_snprintf((అక్షరం *)errmsg, 300, _(e_write_error_conversion_failed_లోపల_పంక్తి_nr_make_fenc_ఖాళీలు_గమ్యం_override),
					 (దీర్ఘ)write_info.bw_conv_error_lnum);
		}
	    }
	    లేకపోతే ఒకవేళ (got_int)
		errmsg = (char_u *)_(e_interrupted);
	    లేకపోతే
		errmsg = (char_u *)_(e_write_error_file_system_నిండిన);
	}

	// If we have a backup file, try to put it in place of the new file,
	// because the new file is probably corrupt.  This avoids losing the
	// original file when trying to make a backup when writing the file a
	// second time.
	// When "backup_copy" is set we need to copy the backup over the new
	// file.  Otherwise rename the backup file.
	// If this is OK, don't give the extra warning message.
	ఒకవేళ (backup != ఖాళీ)
	{
	    ఒకవేళ (backup_copy)
	    {
		// This may take a while, if we were interrupted let the user
		// know we got the message.
		ఒకవేళ (got_int)
		{
		    సందేశం(_(e_interrupted));
		    బహిర్గతం_flush();
		}
		ఒకవేళ ((fd = mch_open((అక్షరం *)backup, O_RDONLY | O_EXTRA, 0)) >= 0)
		{
		    ఒకవేళ ((write_info.bw_fd = mch_open((అక్షరం *)fname,
				    O_WRONLY | O_CREAT | O_TRUNC | O_EXTRA,
							   perm & 0777)) >= 0)
		    {
			// copy the file.
			write_info.bw_buf = smallbuf;
			write_info.bw_flags = FIO_NOCONVERT;
			అయితే ((write_info.bw_len = read_eintr(fd, smallbuf,
						      SMALLBUFSIZE)) > 0)
			    ఒకవేళ (buf_write_బైట్లు(&write_info) == FAIL)
				ఆపు;

			ఒకవేళ (close(write_info.bw_fd) >= 0
						   && write_info.bw_len == 0)
			    చివర = 1;		// success
		    }
		    close(fd);	// ignore errors for closing read file
		}
	    }
	    లేకపోతే
	    {
		ఒకవేళ (vim_rename(backup, fname) == 0)
		    చివర = 1;
	    }
	}
	వెళ్ళు fail;
    }

    lnum -= ప్రారంభం;	    // compute number of written lines
    --no_wait_return;	    // may wait for return now

#ఒకవేళ !(defined(UNIX) || defined(VMS))
    fname = sfname;	    // use shortname now, for the messages
#అయితే_ముగింపు
    ఒకవేళ (!filtering)
    {
	సందేశం_కూడు_fname(buf, fname);	// put fname in IObuff with quotes
	c = FALSE;
	ఒకవేళ (write_info.bw_conv_error)
	{
	    STRCAT(IObuff, _(" CONVERSION ERROR"));
	    c = TRUE;
	    ఒకవేళ (write_info.bw_conv_error_lnum != 0)
		vim_snprintf_కూడు((అక్షరం *)IObuff, IOSIZE, _(" in line %ld;"),
			(దీర్ఘ)write_info.bw_conv_error_lnum);
	}
	లేకపోతే ఒకవేళ (notconverted)
	{
	    STRCAT(IObuff, _("[NOT converted]"));
	    c = TRUE;
	}
	లేకపోతే ఒకవేళ (converted)
	{
	    STRCAT(IObuff, _("[converted]"));
	    c = TRUE;
	}
	ఒకవేళ (device)
	{
	    STRCAT(IObuff, _("[Device]"));
	    c = TRUE;
	}
	లేకపోతే ఒకవేళ (newfile)
	{
	    STRCAT(IObuff, new_file_సందేశం());
	    c = TRUE;
	}
	ఒకవేళ (no_eol)
	{
	    సందేశం_కూడు_eol();
	    c = TRUE;
	}
	// may add [unix/dos/mac]
	ఒకవేళ (సందేశం_కూడు_fileformat(fileformat))
	    c = TRUE;
#నిర్వచితమైతే FEAT_CRYPT
	ఒకవేళ (wb_flags & FIO_ENCRYPTED)
	{
	    crypt_append_సందేశం(buf);
	    c = TRUE;
	}
#అయితే_ముగింపు
	సందేశం_కూడు_పంక్తులు(c, (దీర్ఘ)lnum, nchars);	// add line/char count
	ఒకవేళ (!shortmess(SHM_WRITE))
	{
	    ఒకవేళ (append)
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [a]") : _(" appended"));
	    లేకపోతే
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [w]") : _(" written"));
	}

	సమితి_keep_సందేశం((char_u *)సందేశం_trunc_attr((అక్షరం *)IObuff, FALSE, 0), 0);
    }

    // When written everything correctly: reset 'modified'.  Unless not
    // writing to the original file and '+' is not in 'cpoptions'.
    ఒకవేళ (reset_మారింది && సంపూర్ణ && !append
	    && !write_info.bw_conv_error
	    && (overwriting || vim_strchr(p_cpo, CPO_PLUS) != ఖాళీ))
    {
	unchanged(buf, TRUE, FALSE);
	// b:changedtick may be incremented in unchanged() but that should not
	// trigger a TextChanged event.
	ఒకవేళ (buf->b_చివరి_changedtick + 1 == CHANGEDTICK(buf))
	    buf->b_చివరి_changedtick = CHANGEDTICK(buf);
	u_unchanged(buf);
	u_update_save_nr(buf);
    }

    // If written to the current file, update the timestamp of the swap file
    // and reset the BF_WRITE_MASK flags. Also sets buf->b_mtime.
    ఒకవేళ (overwriting)
    {
	ml_timestamp(buf);
	ఒకవేళ (append)
	    buf->b_flags &= ~BF_NEW;
	లేకపోతే
	    buf->b_flags &= ~BF_WRITE_ముసుగు;
    }

    // If we kept a backup until now, and we are in patch mode, then we make
    // the backup file our 'original' file.
    ఒకవేళ (*p_pm && dobackup)
    {
	అక్షరం *org = (అక్షరం *)buf_modname((buf->b_p_sn || buf->b_shortname),
							  fname, p_pm, FALSE);

	ఒకవేళ (backup != ఖాళీ)
	{
	    stat_T	st;

	    // If the original file does not exist yet
	    // the current backup file becomes the original file
	    ఒకవేళ (org == ఖాళీ)
		emsg(_(e_patchmode_cant_save_original_file));
	    లేకపోతే ఒకవేళ (mch_stat(org, &st) < 0)
	    {
		vim_rename(backup, (char_u *)org);
		VIM_CLEAR(backup);	    // don't delete the file
#నిర్వచితమైతే UNIX
		సమితి_file_సమయం((char_u *)org, st_old.st_atime, st_old.st_mtime);
#అయితే_ముగింపు
	    }
	}
	// If there is no backup file, remember that a (new) file was
	// created.
	లేకపోతే
	{
	    పూర్ణాంకం ఖాళీలు_fd;

	    ఒకవేళ (org == ఖాళీ
		    || (ఖాళీలు_fd = mch_open(org,
				      O_CREAT | O_EXTRA | O_EXCL | O_NOFOLLOW,
					perm < 0 ? 0666 : (perm & 0777))) < 0)
	      emsg(_(e_patchmode_cant_touch_ఖాళీలు_original_file));
	    లేకపోతే
	      close(ఖాళీలు_fd);
	}
	ఒకవేళ (org != ఖాళీ)
	{
	    mch_setperm((char_u *)org, mch_getperm(fname) & 0777);
	    vim_free(org);
	}
    }

    // Remove the backup unless 'backup' option is set or there was a
    // conversion error.
    ఒకవేళ (!p_bk && backup != ఖాళీ && !write_info.bw_conv_error
	    && mch_remove(backup) != 0)
	emsg(_(e_cant_delete_backup_file));

    వెళ్ళు nofail;

    // Finish up.  We get here either after failure or success.
fail:
    --no_wait_return;		// may wait for return now
nofail:

    // Done saving, we accept changed buffer warnings again
    buf->b_saving = false;

    vim_free(backup);
    ఒకవేళ (బఫరు != smallbuf)
	vim_free(బఫరు);
    vim_free(fenc_tofree);
    vim_free(write_info.bw_conv_buf);
#నిర్వచితమైతే USE_ICONV
    ఒకవేళ (write_info.bw_iconv_fd != (iconv_t)-1)
    {
	iconv_close(write_info.bw_iconv_fd);
	write_info.bw_iconv_fd = (iconv_t)-1;
    }
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ACL
    mch_free_acl(acl);
#అయితే_ముగింపు

    ఒకవేళ (errmsg != ఖాళీ)
    {
	పూర్ణాంకం numlen = errnum != ఖాళీ ? (పూర్ణాంకం)STRLEN(errnum) : 0;

	attr = HL_ATTR(HLF_E);	// set highlight for error messages
	సందేశం_కూడు_fname(buf,
#నిర్వచితం_కాకపోతే UNIX
		sfname
#లేకపోతే
		fname
#అయితే_ముగింపు
		     );		// put file name in IObuff with quotes
	ఒకవేళ (STRLEN(IObuff) + STRLEN(errmsg) + numlen >= IOSIZE)
	    IObuff[IOSIZE - STRLEN(errmsg) - numlen - 1] = NUL;
	// If the error message has the form "is ...", put the error number in
	// front of the file name.
	ఒకవేళ (errnum != ఖాళీ)
	{
	    STRMOVE(IObuff + numlen, IObuff);
	    mch_memmove(IObuff, errnum, (size_t)numlen);
	}
	STRCAT(IObuff, errmsg);
	emsg((అక్షరం *)IObuff);
	ఒకవేళ (errmsg_allocated)
	    vim_free(errmsg);

	retval = FAIL;
	ఒకవేళ (చివర == 0)
	{
	    సందేశం_puts_attr(_("\nWARNING: Original file may be lost or damaged\n"),
		    attr | సందేశం_HIST);
	    సందేశం_puts_attr(_("don't quit the editor until the file is successfully written!"),
		    attr | సందేశం_HIST);

	    // Update the timestamp to avoid an "overwrite changed file"
	    // prompt when writing again.
	    ఒకవేళ (mch_stat((అక్షరం *)fname, &st_old) >= 0)
	    {
		buf_store_సమయం(buf, &st_old, fname);
		buf->b_mtime_read = buf->b_mtime;
		buf->b_mtime_read_ns = buf->b_mtime_ns;
	    }
	}
    }
    సందేశం_scroll = సందేశం_save;

#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
    // When writing the whole file and 'undofile' is set, also write the undo
    // file.
    ఒకవేళ (retval == OK && write_undo_file)
    {
	char_u	    hash[UNDO_HASH_కొలత];

	sha256_ముగింపు(&sha_ctx, hash);
	u_write_undo(ఖాళీ, FALSE, buf, hash);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (!should_abort(retval))
#లేకపోతే
    ఒకవేళ (!got_int)
#అయితే_ముగింపు
    {
	aco_save_T	aco;

	curbuf->b_no_eol_lnum = 0;  // in case it was set by the previous read

	// Apply POST autocommands.
	// Careful: The autocommands may call buf_write() recursively!
	// Only do this when a window was found for "buf".
	aucmd_prepbuf(&aco, buf);
	ఒకవేళ (curbuf == buf)
	{
	    ఒకవేళ (append)
		apply_autocmds_exarg(EVENT_FILEAPPENDPOST, fname, fname,
							   FALSE, curbuf, eap);
	    లేకపోతే ఒకవేళ (filtering)
		apply_autocmds_exarg(EVENT_FILTERWRITEPOST, ఖాళీ, fname,
							   FALSE, curbuf, eap);
	    లేకపోతే ఒకవేళ (reset_మారింది && సంపూర్ణ)
		apply_autocmds_exarg(EVENT_BUFWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);
	    లేకపోతే
		apply_autocmds_exarg(EVENT_FILEWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}

#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (aborting())	    // autocmds may abort script processing
	    retval = FALSE;
#అయితే_ముగింపు
    }

#నిర్వచితమైతే FEAT_VIMINFO
    // Make sure marks will be written out to the viminfo file later, even when
    // the file is new.
    curbuf->b_marks_read = true;
#అయితే_ముగింపు

    got_int |= prev_got_int;

    తిరిగి retval;
}
