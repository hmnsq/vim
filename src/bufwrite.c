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

#अन्तर्भाव "vim.h"

#सचे defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
# include <utime.h>		// for struct utimbuf
#endif

#define SMALLBUFSIZE	256	// size of emergency write buffer

/*
 * Structure to pass arguments from buf_write() to buf_write_bytes().
 */
संरचना bw_info
{
    पूर्णाङ्क		bw_fd;		// file descriptor
    char_u	*bw_buf;	// buffer with data to be written
    पूर्णाङ्क		bw_len;		// length of data
    पूर्णाङ्क		bw_flags;	// FIO_ flags
#ifdef FEAT_CRYPT
    buf_T	*bw_buffer;	// buffer being written
    पूर्णाङ्क		bw_finish;	// finish encrypting
#endif
    char_u	bw_rest[CONV_RESTLEN]; // not converted bytes
    पूर्णाङ्क		bw_restlen;	// nr of bytes in bw_rest[]
    पूर्णाङ्क		bw_first;	// first write call
    char_u	*bw_conv_buf;	// buffer for writing converted chars
    size_t	bw_conv_buflen; // size of bw_conv_buf
    पूर्णाङ्क		bw_conv_error;	// set for conversion error
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
    स्थिर पूर्णाङ्क
ucs2bytes(
    unsigned	c,		// in: character
    char_u	**pp,		// in/out: pointer to result
    पूर्णाङ्क		flags)		// FIO_ flags
{
    char_u	*p = *pp;
    पूर्णाङ्क		error = FALSE;
    पूर्णाङ्क		cc;


    सचे (flags & FIO_UCS4)
    {
	सचे (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	    *p++ = (c >> 16);
	    *p++ = (c >> 24);
	}
	अञ्ञथा
	{
	    *p++ = (c >> 24);
	    *p++ = (c >> 16);
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    अञ्ञथा सचे (flags & (FIO_UCS2 | FIO_UTF16))
    {
	सचे (c >= 0x10000)
	{
	    सचे (flags & FIO_UTF16)
	    {
		// Make two words, ten bits of the character in each.  First
		// word is 0xd800 - 0xdbff, second one 0xdc00 - 0xdfff
		c -= 0x10000;
		सचे (c >= 0x100000)
		    error = TRUE;
		cc = ((c >> 10) & 0x3ff) + 0xd800;
		सचे (flags & FIO_ENDIAN_L)
		{
		    *p++ = cc;
		    *p++ = ((unsigned)cc >> 8);
		}
		अञ्ञथा
		{
		    *p++ = ((unsigned)cc >> 8);
		    *p++ = cc;
		}
		c = (c & 0x3ff) + 0xdc00;
	    }
	    अञ्ञथा
		error = TRUE;
	}
	सचे (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	}
	अञ्ञथा
	{
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    अञ्ञथा    // Latin1
    {
	सचे (c >= 0x100)
	{
	    error = TRUE;
	    *p++ = 0xBF;
	}
	अञ्ञथा
	    *p++ = c;
    }

    *pp = p;
    पटिनिवत्त error;
}

/*
 * Call write() to write a number of bytes to the file.
 * Handles encryption and 'encoding' conversion.
 *
 * Return FAIL for failure, OK otherwise.
 */
    स्थिर पूर्णाङ्क
buf_write_bytes(संरचना bw_info *ip)
{
    पूर्णाङ्क		wlen;
    char_u	*buf = ip->bw_buf;	// data to write
    पूर्णाङ्क		len = ip->bw_len;	// length of data
    पूर्णाङ्क		flags = ip->bw_flags;	// extra flags

    // Skip conversion when writing the crypt magic number or the BOM.
    सचे (!(flags & FIO_NOCONVERT))
    {
	char_u		*p;
	unsigned	c;
	पूर्णाङ्क		n;

	सचे (flags & FIO_UTF8)
	{
	    // Convert latin1 in the buffer to UTF-8 in the file.
	    p = ip->bw_conv_buf;	// translate to buffer
	    अत्थाय (wlen = 0; wlen < len; ++wlen)
		p += utf_char2bytes(buf[wlen], p);
	    buf = ip->bw_conv_buf;
	    len = (पूर्णाङ्क)(p - ip->bw_conv_buf);
	}
	अञ्ञथा सचे (flags & (FIO_UCS4 | FIO_UTF16 | FIO_UCS2 | FIO_LATIN1))
	{
	    // Convert UTF-8 bytes in the buffer to UCS-2, UCS-4, UTF-16 or
	    // Latin1 chars in the file.
	    सचे (flags & FIO_LATIN1)
		p = buf;	// translate in-place (can only get shorter)
	    अञ्ञथा
		p = ip->bw_conv_buf;	// translate to buffer
	    अत्थाय (wlen = 0; wlen < len; wlen += n)
	    {
		सचे (wlen == 0 && ip->bw_restlen != 0)
		{
		    पूर्णाङ्क		l;

		    // Use remainder of previous call.  Append the start of
		    // buf[] to get a full sequence.  Might still be too
		    // short!
		    l = CONV_RESTLEN - ip->bw_restlen;
		    सचे (l > len)
			l = len;
		    mch_memmove(ip->bw_rest + ip->bw_restlen, buf, (size_t)l);
		    n = utf_ptr2len_len(ip->bw_rest, ip->bw_restlen + l);
		    सचे (n > ip->bw_restlen + len)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			सचे (ip->bw_restlen + len > CONV_RESTLEN)
			    पटिनिवत्त FAIL;
			ip->bw_restlen += len;
			भङ्ग;
		    }
		    सचे (n > 1)
			c = utf_ptr2char(ip->bw_rest);
		    अञ्ञथा
			c = ip->bw_rest[0];
		    सचे (n >= ip->bw_restlen)
		    {
			n -= ip->bw_restlen;
			ip->bw_restlen = 0;
		    }
		    अञ्ञथा
		    {
			ip->bw_restlen -= n;
			mch_memmove(ip->bw_rest, ip->bw_rest + n,
						      (size_t)ip->bw_restlen);
			n = 0;
		    }
		}
		अञ्ञथा
		{
		    n = utf_ptr2len_len(buf + wlen, len - wlen);
		    सचे (n > len - wlen)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			सचे (len - wlen > CONV_RESTLEN)
			    पटिनिवत्त FAIL;
			ip->bw_restlen = len - wlen;
			mch_memmove(ip->bw_rest, buf + wlen,
						      (size_t)ip->bw_restlen);
			भङ्ग;
		    }
		    सचे (n > 1)
			c = utf_ptr2char(buf + wlen);
		    अञ्ञथा
			c = buf[wlen];
		}
		// Check that there is enough space
		सचे (!(flags & FIO_LATIN1))
		{
		    size_t need = (flags & FIO_UCS4) ? 4 : 2;
		    सचे ((flags & FIO_UTF16) && c >= 0x10000)
			need = 4;

		    सचे ((size_t)(p - ip->bw_conv_buf) + need > ip->bw_conv_buflen)
			पटिनिवत्त FAIL;
		}

		सचे (ucs2bytes(c, &p, flags) && !ip->bw_conv_error)
		{
		    ip->bw_conv_error = TRUE;
		    ip->bw_conv_error_lnum = ip->bw_start_lnum;
		}
		सचे (c == NL)
		    ++ip->bw_start_lnum;
	    }
	    सचे (flags & FIO_LATIN1)
		len = (पूर्णाङ्क)(p - buf);
	    अञ्ञथा
	    {
		buf = ip->bw_conv_buf;
		len = (पूर्णाङ्क)(p - ip->bw_conv_buf);
	    }
	}

#ifdef MSWIN
	अञ्ञथा सचे (flags & FIO_CODEPAGE)
	{
	    // Convert UTF-8 or codepage to UCS-2 and then to MS-Windows
	    // codepage.
	    char_u	*from;
	    size_t	fromlen;
	    char_u	*to;
	    पूर्णाङ्क		u8c;
	    BOOL	bad = FALSE;
	    पूर्णाङ्क		needed;

	    सचे (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    अञ्ञथा
	    {
		from = buf;
		fromlen = len;
	    }

	    to = ip->bw_conv_buf;
	    सचे (enc_utf8)
	    {
		// Convert from UTF-8 to UCS-2, to the start of the buffer.
		// The buffer has been allocated to be big enough.
		याव (fromlen > 0)
		{
		    n = (पूर्णाङ्क)utf_ptr2len_len(from, (पूर्णाङ्क)fromlen);
		    सचे (n > (पूर्णाङ्क)fromlen)	// incomplete byte sequence
			भङ्ग;
		    u8c = utf_ptr2char(from);
		    *to++ = (u8c & 0xff);
		    *to++ = (u8c >> 8);
		    fromlen -= n;
		    from += n;
		}

		// Copy remainder to ip->bw_rest[] to be used for the next
		// call.
		सचे (fromlen > CONV_RESTLEN)
		{
		    // weird overlong sequence
		    ip->bw_conv_error = TRUE;
		    पटिनिवत्त FAIL;
		}
		mch_memmove(ip->bw_rest, from, fromlen);
		ip->bw_restlen = (पूर्णाङ्क)fromlen;
	    }
	    अञ्ञथा
	    {
		// Convert from enc_codepage to UCS-2, to the start of the
		// buffer.  The buffer has been allocated to be big enough.
		ip->bw_restlen = 0;
		needed = MultiByteToWideChar(enc_codepage,
			     MB_ERR_INVALID_CHARS, (LPCSTR)from, (पूर्णाङ्क)fromlen,
								     रिक्त, 0);
		सचे (needed == 0)
		{
		    // When conversion fails there may be a trailing byte.
		    needed = MultiByteToWideChar(enc_codepage,
			 MB_ERR_INVALID_CHARS, (LPCSTR)from, (पूर्णाङ्क)fromlen - 1,
								     रिक्त, 0);
		    सचे (needed == 0)
		    {
			// Conversion doesn't work.
			ip->bw_conv_error = TRUE;
			पटिनिवत्त FAIL;
		    }
		    // Save the trailing byte for the next call.
		    ip->bw_rest[0] = from[fromlen - 1];
		    ip->bw_restlen = 1;
		}
		needed = MultiByteToWideChar(enc_codepage, MB_ERR_INVALID_CHARS,
				(LPCSTR)from, (पूर्णाङ्क)(fromlen - ip->bw_restlen),
							  (LPWSTR)to, needed);
		सचे (needed == 0)
		{
		    // Safety check: Conversion doesn't work.
		    ip->bw_conv_error = TRUE;
		    पटिनिवत्त FAIL;
		}
		to += needed * 2;
	    }

	    fromlen = to - ip->bw_conv_buf;
	    buf = to;
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	    सचे (FIO_GET_CP(flags) == CP_UTF8)
	    {
		// Convert from UCS-2 to UTF-8, using the remainder of the
		// conversion buffer.  Fails when out of space.
		अत्थाय (from = ip->bw_conv_buf; fromlen > 1; fromlen -= 2)
		{
		    u8c = *from++;
		    u8c += (*from++ << 8);
		    to += utf_char2bytes(u8c, to);
		    सचे (to + 6 >= ip->bw_conv_buf + ip->bw_conv_buflen)
		    {
			ip->bw_conv_error = TRUE;
			पटिनिवत्त FAIL;
		    }
		}
		len = (पूर्णाङ्क)(to - buf);
	    }
	    अञ्ञथा
# endif
	    {
		// Convert from UCS-2 to the codepage, using the remainder of
		// the conversion buffer.  If the conversion uses the default
		// character "0", the data doesn't fit in this encoding, so
		// fail.
		len = WideCharToMultiByte(FIO_GET_CP(flags), 0,
			(LPCWSTR)ip->bw_conv_buf, (पूर्णाङ्क)fromlen / परिमाण(WCHAR),
			(LPSTR)to, (पूर्णाङ्क)(ip->bw_conv_buflen - fromlen), 0,
									&bad);
		सचे (bad)
		{
		    ip->bw_conv_error = TRUE;
		    पटिनिवत्त FAIL;
		}
	    }
	}
#endif

#ifdef MACOS_CONVERT
	अञ्ञथा सचे (flags & FIO_MACROMAN)
	{
	    // Convert UTF-8 or latin1 to Apple MacRoman.
	    char_u	*from;
	    size_t	fromlen;

	    सचे (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    अञ्ञथा
	    {
		from = buf;
		fromlen = len;
	    }

	    सचे (enc2macroman(from, fromlen,
			ip->bw_conv_buf, &len, ip->bw_conv_buflen,
			ip->bw_rest, &ip->bw_restlen) == FAIL)
	    {
		ip->bw_conv_error = TRUE;
		पटिनिवत्त FAIL;
	    }
	    buf = ip->bw_conv_buf;
	}
#endif

#ifdef USE_ICONV
	सचे (ip->bw_iconv_fd != (iconv_t)-1)
	{
	    ध्रुव वर्ण	*from;
	    size_t	fromlen;
	    वर्ण	*to;
	    size_t	tolen;

	    // Convert with iconv().
	    सचे (ip->bw_restlen > 0)
	    {
		वर्ण *fp;

		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		fp = (वर्ण *)ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(fp, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(fp + ip->bw_restlen, buf, (size_t)len);
		from = fp;
		tolen = ip->bw_conv_buflen - fromlen;
	    }
	    अञ्ञथा
	    {
		from = (ध्रुव वर्ण *)buf;
		fromlen = len;
		tolen = ip->bw_conv_buflen;
	    }
	    to = (वर्ण *)ip->bw_conv_buf;

	    सचे (ip->bw_first)
	    {
		size_t	save_len = tolen;

		// output the initial shift state sequence
		(शून्य)iconv(ip->bw_iconv_fd, रिक्त, रिक्त, &to, &tolen);

		// There is a bug in iconv() on Linux (which appears to be
		// wide-spread) which sets "to" to NULL and messes up "tolen".
		सचे (to == रिक्त)
		{
		    to = (वर्ण *)ip->bw_conv_buf;
		    tolen = save_len;
		}
		ip->bw_first = FALSE;
	    }

	    // If iconv() has an error or there is not enough room, fail.
	    सचे ((iconv(ip->bw_iconv_fd, (शून्य *)&from, &fromlen, &to, &tolen)
			== (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						    || fromlen > CONV_RESTLEN)
	    {
		ip->bw_conv_error = TRUE;
		पटिनिवत्त FAIL;
	    }

	    // copy remainder to ip->bw_rest[] to be used for the next call.
	    सचे (fromlen > 0)
		mch_memmove(ip->bw_rest, (शून्य *)from, fromlen);
	    ip->bw_restlen = (पूर्णाङ्क)fromlen;

	    buf = ip->bw_conv_buf;
	    len = (पूर्णाङ्क)((char_u *)to - ip->bw_conv_buf);
	}
#endif
    }

    सचे (ip->bw_fd < 0)
	// Only checking conversion, which is OK if we get here.
	पटिनिवत्त OK;

#ifdef FEAT_CRYPT
    सचे (flags & FIO_ENCRYPTED)
    {
	// Encrypt the data. Do it in-place if possible, otherwise use an
	// allocated buffer.
# ifdef CRYPT_NOT_INPLACE
	सचे (crypt_works_inplace(ip->bw_buffer->b_cryptstate))
	{
# endif
	    crypt_encode_inplace(ip->bw_buffer->b_cryptstate, buf, len,
								ip->bw_finish);
# ifdef CRYPT_NOT_INPLACE
	}
	अञ्ञथा
	{
	    char_u *outbuf;

	    len = crypt_encode_alloc(curbuf->b_cryptstate, buf, len, &outbuf,
								ip->bw_finish);
	    सचे (len == 0)
		पटिनिवत्त OK;  // Crypt layer is buffering, will flush later.
	    wlen = write_eintr(ip->bw_fd, outbuf, len);
	    vim_free(outbuf);
	    पटिनिवत्त (wlen < len) ? FAIL : OK;
	}
# endif
    }
#endif

    wlen = write_eintr(ip->bw_fd, buf, len);
    पटिनिवत्त (wlen < len) ? FAIL : OK;
}

/*
 * Check modification time of file, before writing to it.
 * The size isn't checked, because using a tool like "gzip" takes care of
 * using the same timestamp but can't set the size.
 */
    स्थिर पूर्णाङ्क
check_mtime(buf_T *buf, stat_T *st)
{
    सचे (buf->b_mtime_read != 0
		  && time_differs(st, buf->b_mtime_read, buf->b_mtime_read_ns))
    {
	सन्देशः_scroll = TRUE;	    // don't overwrite messages here
	सन्देशः_silent = 0;		    // must give this prompt
	// don't use emsg() here, don't want to flush the buffers
	सन्देशः_attr(_("WARNING: The file has been changed since reading it!!!"),
						       HL_ATTR(HLF_E));
	सचे (ask_yesno((char_u *)_("Do you really want to write to it"),
								 TRUE) == 'n')
	    पटिनिवत्त FAIL;
	सन्देशः_scroll = FALSE;	    // always overwrite the file message now
    }
    पटिनिवत्त OK;
}

/*
 * Generate a BOM in "buf[4]" for encoding "name".
 * Return the length of the BOM (zero when no BOM).
 */
    स्थिर पूर्णाङ्क
make_bom(char_u *buf, char_u *name)
{
    पूर्णाङ्क		flags;
    char_u	*p;

    flags = get_fio_flags(name);

    // Can't put a BOM in a non-Unicode file.
    सचे (flags == FIO_LATIN1 || flags == 0)
	पटिनिवत्त 0;

    सचे (flags == FIO_UTF8)	// UTF-8
    {
	buf[0] = 0xef;
	buf[1] = 0xbb;
	buf[2] = 0xbf;
	पटिनिवत्त 3;
    }
    p = buf;
    (शून्य)ucs2bytes(0xfeff, &p, flags);
    पटिनिवत्त (पूर्णाङ्क)(p - buf);
}

#ifdef UNIX
    स्थिर शून्य
set_file_time(
    char_u  *fname,
    time_t  atime,	    // access time
    time_t  mtime)	    // modification time
{
# सचे defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
    संरचना utimbuf  buf;

    buf.actime	= atime;
    buf.modtime	= mtime;
    (शून्य)utime((वर्ण *)fname, &buf);
# अञ्ञथा
#  सचे defined(HAVE_UTIMES)
    संरचना timeval  tvp[2];

    tvp[0].tv_sec   = atime;
    tvp[0].tv_usec  = 0;
    tvp[1].tv_sec   = mtime;
    tvp[1].tv_usec  = 0;
    (शून्य)utimes((वर्ण *)fname, (ध्रुव संरचना timeval *)&tvp);
#  endif
# endif
}
#endif // UNIX

    वर्ण *
new_file_message(शून्य)
{
    पटिनिवत्त shortmess(SHM_NEW) ? _("[New]") : _("[New File]");
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
    पूर्णाङ्क
buf_write(
    buf_T	    *buf,
    char_u	    *fname,
    char_u	    *sfname,
    linenr_T	    start,
    linenr_T	    end,
    exarg_T	    *eap,		// for forced 'ff' and 'fenc', can be
					// NULL!
    पूर्णाङ्क		    append,		// append to the file
    पूर्णाङ्क		    forceit,
    पूर्णाङ्क		    reset_changed,
    पूर्णाङ्क		    filtering)
{
    पूर्णाङ्क		    fd;
    char_u	    *backup = रिक्त;
    पूर्णाङ्क		    backup_copy = FALSE; // copy the original file?
    पूर्णाङ्क		    dobackup;
    char_u	    *ffname;
    char_u	    *wfname = रिक्त;	// name of file to write to
    char_u	    *s;
    char_u	    *ptr;
    char_u	    c;
    पूर्णाङ्क		    len;
    linenr_T	    lnum;
    long	    nchars;
    char_u	    *errmsg = रिक्त;
    पूर्णाङ्क		    errmsg_allocated = FALSE;
    char_u	    *errnum = रिक्त;
    char_u	    *buffer;
    char_u	    smallbuf[SMALLBUFSIZE];
    char_u	    *backup_ext;
    पूर्णाङ्क		    bufsize;
    long	    perm;		    // file permissions
    पूर्णाङ्क		    retval = OK;
    पूर्णाङ्क		    newfile = FALSE;	    // TRUE if file doesn't exist yet
    पूर्णाङ्क		    सन्देशः_save = सन्देशः_scroll;
    पूर्णाङ्क		    overwriting;	    // TRUE if writing over original
    पूर्णाङ्क		    no_eol = FALSE;	    // no end-of-line written
    पूर्णाङ्क		    device = FALSE;	    // writing to a device
    stat_T	    st_old;
    पूर्णाङ्क		    prev_got_int = got_int;
    पूर्णाङ्क		    checking_conversion;
    पूर्णाङ्क		    file_readonly = FALSE;  // overwritten file is read-only
#सचे defined(UNIX)			    // XXX fix me sometime?
    पूर्णाङ्क		    made_writable = FALSE;  // 'w' bit has been set
#endif
					// writing everything
    पूर्णाङ्क		    whole = (start == 1 && end == buf->b_ml.ml_पङ्क्तिः_गणनम्);
    linenr_T	    old_पङ्क्तिः_गणनम् = buf->b_ml.ml_पङ्क्तिः_गणनम्;
    पूर्णाङ्क		    attr;
    पूर्णाङ्क		    fileformat;
    पूर्णाङ्क		    write_bin;
    संरचना bw_info  write_info;		// info for buf_write_bytes()
    पूर्णाङ्क		    converted = FALSE;
    पूर्णाङ्क		    notconverted = FALSE;
    char_u	    *fenc;		// effective 'fileencoding'
    char_u	    *fenc_tofree = रिक्त; // allocated "fenc"
    पूर्णाङ्क		    wb_flags = 0;
#ifdef HAVE_ACL
    vim_acl_T	    acl = रिक्त;		// ACL copied from original file to
					// backup or new file
#endif
#ifdef FEAT_PERSISTENT_UNDO
    पूर्णाङ्क		    write_undo_file = FALSE;
    context_sha256_T sha_ctx;
#endif
    unsigned पूर्णाङ्क    bkc = get_bkc_flags(buf);
    pos_T	    orig_start = buf->b_op_start;
    pos_T	    orig_end = buf->b_op_end;

    सचे (fname == रिक्त || *fname == NUL)	// safety check
	पटिनिवत्त FAIL;
    सचे (buf->b_ml.ml_mfp == रिक्त)
    {
	// This can happen during startup when there is a stray "w" in the
	// vimrc file.
	emsg(_(e_empty_buffer));
	पटिनिवत्त FAIL;
    }

    // Disallow writing from .exrc and .vimrc in current directory for
    // security reasons.
    सचे (check_secure())
	पटिनिवत्त FAIL;

    // Avoid a crash for a long name.
    सचे (STRLEN(fname) >= MAXPATHL)
    {
	emsg(_(e_name_too_long));
	पटिनिवत्त FAIL;
    }

    // must init bw_conv_buf and bw_iconv_fd before jumping to "fail"
    write_info.bw_conv_buf = रिक्त;
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
    सचे (buf->b_ffname == रिक्त
	    && reset_changed
	    && whole
	    && buf == curbuf
	    && !bt_nofilename(buf)
	    && !filtering
	    && (!append || vim_strchr(p_cpo, CPO_FNAMEAPP) != रिक्त)
	    && vim_strchr(p_cpo, CPO_FNAMEW) != रिक्त)
    {
	सचे (set_rw_fname(fname, sfname) == FAIL)
	    पटिनिवत्त FAIL;
	buf = curbuf;	    // just in case autocmds made "buf" invalid
    }

    सचे (sfname == रिक्त)
	sfname = fname;
    // For Unix: Use the short file name whenever possible.
    // Avoids problems with networks and when directory names are changed.
    // Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
    // another directory, which we don't detect
    ffname = fname;			    // remember full fname
#ifdef UNIX
    fname = sfname;
#endif

    सचे (buf->b_ffname != रिक्त && fnamecmp(ffname, buf->b_ffname) == 0)
	overwriting = TRUE;
    अञ्ञथा
	overwriting = FALSE;

    सचे (exiting)
	settmode(TMODE_COOK);	    // when exiting allow typeahead now

    ++no_wait_return;		    // don't wait for return yet

    // Set '[ and '] marks to the lines to be written.
    buf->b_op_start.lnum = start;
    buf->b_op_start.col = 0;
    buf->b_op_end.lnum = end;
    buf->b_op_end.col = 0;

    {
	aco_save_T	aco;
	पूर्णाङ्क		buf_ffname = FALSE;
	पूर्णाङ्क		buf_sfname = FALSE;
	पूर्णाङ्क		buf_fname_f = FALSE;
	पूर्णाङ्क		buf_fname_s = FALSE;
	पूर्णाङ्क		did_cmd = FALSE;
	पूर्णाङ्क		nofile_err = FALSE;
	पूर्णाङ्क		empty_memline = (buf->b_ml.ml_mfp == रिक्त);
	bufref_T	bufref;

	// Apply PRE autocommands.
	// Set curbuf to the buffer to be written.
	// Careful: The autocommands may call buf_write() recursively!
	सचे (ffname == buf->b_ffname)
	    buf_ffname = TRUE;
	सचे (sfname == buf->b_sfname)
	    buf_sfname = TRUE;
	सचे (fname == buf->b_ffname)
	    buf_fname_f = TRUE;
	सचे (fname == buf->b_sfname)
	    buf_fname_s = TRUE;

	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, buf);
	सचे (curbuf != buf)
	{
	    // Could not find a window for "buf".  Doing more might cause
	    // problems, better bail out.
	    पटिनिवत्त FAIL;
	}

	set_bufref(&bufref, buf);

	सचे (append)
	{
	    सचे (!(did_cmd = apply_autocmds_exarg(EVENT_FILEAPPENDCMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		सचे (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		अञ्ञथा
		    apply_autocmds_exarg(EVENT_FILEAPPENDPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	अञ्ञथा सचे (filtering)
	{
	    apply_autocmds_exarg(EVENT_FILTERWRITEPRE,
					    रिक्त, sfname, FALSE, curbuf, eap);
	}
	अञ्ञथा सचे (reset_changed && whole)
	{
	    पूर्णाङ्क was_changed = curbufIsChanged();

	    did_cmd = apply_autocmds_exarg(EVENT_BUFWRITECMD,
					  sfname, sfname, FALSE, curbuf, eap);
	    सचे (did_cmd)
	    {
		सचे (was_changed && !curbufIsChanged())
		{
		    // Written everything correctly and BufWriteCmd has reset
		    // 'modified': Correct the undo information so that an
		    // undo now sets 'modified'.
		    u_unchanged(curbuf);
		    u_update_save_nr(curbuf);
		}
	    }
	    अञ्ञथा
	    {
		सचे (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		अञ्ञथा
		    apply_autocmds_exarg(EVENT_BUFWRITEPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	अञ्ञथा
	{
	    सचे (!(did_cmd = apply_autocmds_exarg(EVENT_FILEWRITECMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		सचे (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		अञ्ञथा
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
	सचे (!bufref_valid(&bufref))
	    buf = रिक्त;
	सचे (buf == रिक्त || (buf->b_ml.ml_mfp == रिक्त && !empty_memline)
				       || did_cmd || nofile_err
#ifdef FEAT_EVAL
				       || aborting()
#endif
				       )
	{
	    सचे (buf != रिक्त && (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	    {
		// restore the original '[ and '] positions
		buf->b_op_start = orig_start;
		buf->b_op_end = orig_end;
	    }

	    --no_wait_return;
	    सन्देशः_scroll = सन्देशः_save;
	    सचे (nofile_err)
		semsg(_(e_no_matching_autocommands_for_buftype_str_buffer),
							       curbuf->b_p_bt);

	    सचे (nofile_err
#ifdef FEAT_EVAL
		    || aborting()
#endif
		    )
		// An aborting error, interrupt or exception in the
		// autocommands.
		पटिनिवत्त FAIL;
	    सचे (did_cmd)
	    {
		सचे (buf == रिक्त)
		    // The buffer was deleted.  We assume it was written
		    // (can't retry anyway).
		    पटिनिवत्त OK;
		सचे (overwriting)
		{
		    // Assume the buffer was written, update the timestamp.
		    ml_timestamp(buf);
		    सचे (append)
			buf->b_flags &= ~BF_NEW;
		    अञ्ञथा
			buf->b_flags &= ~BF_WRITE_MASK;
		}
		सचे (reset_changed && buf->b_changed && !append
			&& (overwriting || vim_strchr(p_cpo, CPO_PLUS) != रिक्त))
		    // Buffer still changed, the autocommands didn't work
		    // properly.
		    पटिनिवत्त FAIL;
		पटिनिवत्त OK;
	    }
#ifdef FEAT_EVAL
	    सचे (!aborting())
#endif
		emsg(_(e_autocommands_deleted_or_unloaded_buffer_to_be_written));
	    पटिनिवत्त FAIL;
	}

	// The autocommands may have changed the number of lines in the file.
	// When writing the whole file, adjust the end.
	// When writing part of the file, assume that the autocommands only
	// changed the number of lines that are to be written (tricky!).
	सचे (buf->b_ml.ml_पङ्क्तिः_गणनम् != old_पङ्क्तिः_गणनम्)
	{
	    सचे (whole)						// write all
		end = buf->b_ml.ml_पङ्क्तिः_गणनम्;
	    अञ्ञथा सचे (buf->b_ml.ml_पङ्क्तिः_गणनम् > old_पङ्क्तिः_गणनम्)	// more lines
		end += buf->b_ml.ml_पङ्क्तिः_गणनम् - old_पङ्क्तिः_गणनम्;
	    अञ्ञथा						// less lines
	    {
		end -= old_पङ्क्तिः_गणनम् - buf->b_ml.ml_पङ्क्तिः_गणनम्;
		सचे (end < start)
		{
		    --no_wait_return;
		    सन्देशः_scroll = सन्देशः_save;
		    emsg(_(e_autocommands_changed_सङ्ख्या_of_पङ्क्तयः_in_unexpected_way));
		    पटिनिवत्त FAIL;
		}
	    }
	}

	// The autocommands may have changed the name of the buffer, which may
	// be kept in fname, ffname and sfname.  A ":cd" may also have dropped
	// the short name, then b_fname is the full name.
	सचे (buf_ffname)
	    ffname = buf->b_ffname;
	सचे (buf_sfname)
	    sfname = buf->b_fname;
	सचे (buf_fname_f)
	    fname = buf->b_ffname;
	सचे (buf_fname_s)
	    fname = buf->b_fname;
    }

    सचे (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	// restore the original '[ and '] positions
	buf->b_op_start = orig_start;
	buf->b_op_end = orig_end;
    }

#ifdef FEAT_NETBEANS_INTG
    सचे (netbeans_active() && isNetbeansBuffer(buf))
    {
	सचे (whole)
	{
	    // b_changed can be 0 after an undo, but we still need to write
	    // the buffer to NetBeans.
	    सचे (buf->b_changed || isNetbeansModified(buf))
	    {
		--no_wait_return;		// may wait for return now
		सन्देशः_scroll = सन्देशः_save;
		netbeans_save_buffer(buf);	// no error checking...
		पटिनिवत्त retval;
	    }
	    अञ्ञथा
	    {
		errnum = (char_u *)"E656: ";
		errmsg = (char_u *)_(e_netbeans_disallows_writes_of_unmodified_buffers);
		buffer = रिक्त;
		गच्छ fail;
	    }
	}
	अञ्ञथा
	{
	    errnum = (char_u *)"E657: ";
	    errmsg = (char_u *)_(e_partial_writes_disallowed_for_netbeans_buffers);
	    buffer = रिक्त;
	    गच्छ fail;
	}
    }
#endif

    सचे (shortmess(SHM_OVER) && !exiting)
	सन्देशः_scroll = FALSE;	    // overwrite previous file message
    अञ्ञथा
	सन्देशः_scroll = TRUE;	    // don't overwrite previous file message
    सचे (!filtering)
	filemess(buf,
#ifndef UNIX
		sfname,
#अञ्ञथा
		fname,
#endif
		    (char_u *)"", 0);	// show that we are busy
    सन्देशः_scroll = FALSE;		    // always overwrite the file message now

    buffer = alloc(WRITEBUFSIZE);
    सचे (buffer == रिक्त)		    // can't allocate big buffer, use small
				    // one (to be able to write when out of
				    // memory)
    {
	buffer = smallbuf;
	bufsize = SMALLBUFSIZE;
    }
    अञ्ञथा
	bufsize = WRITEBUFSIZE;

    // Get information about original file (if there is one).
#सचे defined(UNIX)
    st_old.st_dev = 0;
    st_old.st_ino = 0;
    perm = -1;
    सचे (mch_stat((वर्ण *)fname, &st_old) < 0)
	newfile = TRUE;
    अञ्ञथा
    {
	perm = st_old.st_mode;
	सचे (!S_ISREG(st_old.st_mode))		// not a file
	{
	    सचे (S_ISDIR(st_old.st_mode))
	    {
		errnum = (char_u *)"E502: ";
		errmsg = (char_u *)_(e_is_a_directory);
		गच्छ fail;
	    }
	    सचे (mch_nodetype(fname) != ग्रन्थिः_WRITABLE)
	    {
		errnum = (char_u *)"E503: ";
		errmsg = (char_u *)_(e_is_not_file_or_writable_device);
		गच्छ fail;
	    }
	    // It's a device of some kind (or a fifo) which we can write to
	    // but for which we can't make a backup.
	    device = TRUE;
	    newfile = TRUE;
	    perm = -1;
	}
    }
#अञ्ञथा // !UNIX
    // Check for a writable device name.
    c = mch_nodetype(fname);
    सचे (c == ग्रन्थिः_OTHER)
    {
	errnum = (char_u *)"E503: ";
	errmsg = (char_u *)_(e_is_not_file_or_writable_device);
	गच्छ fail;
    }
    सचे (c == ग्रन्थिः_WRITABLE)
    {
# सचे defined(MSWIN)
	// MS-Windows allows opening a device, but we will probably get stuck
	// trying to write to it.
	सचे (!p_odev)
	{
	    errnum = (char_u *)"E796: ";
	    errmsg = (char_u *)_(e_writing_to_device_disabled_with_opendevice_option);
	    गच्छ fail;
	}
# endif
	device = TRUE;
	newfile = TRUE;
	perm = -1;
    }
    अञ्ञथा
    {
	perm = mch_getperm(fname);
	सचे (perm < 0)
	    newfile = TRUE;
	अञ्ञथा सचे (mch_isdir(fname))
	{
	    errnum = (char_u *)"E502: ";
	    errmsg = (char_u *)_(e_is_a_directory);
	    गच्छ fail;
	}
	सचे (overwriting)
	    (शून्य)mch_stat((वर्ण *)fname, &st_old);
    }
#endif // !UNIX

    सचे (!device && !newfile)
    {
	// Check if the file is really writable (when renaming the file to
	// make a backup we won't discover it later).
	file_readonly = check_file_readonly(fname, (पूर्णाङ्क)perm);

	सचे (!forceit && file_readonly)
	{
	    सचे (vim_strchr(p_cpo, CPO_FWRITE) != रिक्त)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_in_cpoptions);
	    }
	    अञ्ञथा
	    {
		errnum = (char_u *)"E505: ";
		errmsg = (char_u *)_(e_is_read_only_add_bang_to_override);
	    }
	    गच्छ fail;
	}

	// Check if the timestamp hasn't changed since reading the file.
	सचे (overwriting)
	{
	    retval = check_mtime(buf, &st_old);
	    सचे (retval == FAIL)
		गच्छ fail;
	}
    }

#ifdef HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    सचे (!newfile)
	acl = mch_get_acl(fname);
#endif

    // If 'backupskip' is not empty, don't make a backup for some files.
    dobackup = (p_wb || p_bk || *p_pm != NUL);
    सचे (dobackup && *p_bsk != NUL && match_file_list(p_bsk, sfname, ffname))
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
    सचे (!(append && *p_pm == NUL) && !filtering && perm >= 0 && dobackup)
    {
#सचे defined(UNIX) || defined(MSWIN)
	stat_T	    st;
#endif

	सचे ((bkc & BKC_YES) || append)	// "yes"
	    backup_copy = TRUE;
#सचे defined(UNIX) || defined(MSWIN)
	अञ्ञथा सचे ((bkc & BKC_AUTO))	// "auto"
	{
# ifdef UNIX
	    // Don't rename the file when:
	    // - it's a hard link
	    // - it's a symbolic link
	    // - we don't have write permission in the directory
	    // - we can't set the owner/group of the new file
	    सचे (st_old.st_nlink > 1
		    || mch_lstat((वर्ण *)fname, &st) < 0
		    || st.st_dev != st_old.st_dev
		    || st.st_ino != st_old.st_ino
#  ifndef HAVE_FCHOWN
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid
#  endif
		    )
		backup_copy = TRUE;
	    अञ्ञथा
# अञ्ञथा
#  ifdef MSWIN
	    // On NTFS file systems hard links are possible.
	    सचे (mch_is_linked(fname))
		backup_copy = TRUE;
	    अञ्ञथा
#  endif
# endif
	    {
		size_t	dirlen;
		char_u	tmp_fname[MAXPATHL];
		पूर्णाङ्क	i;

		// Check if we can create a file and set the owner/group/mode to
		// the ones from the original file.
		// First find a file name that doesn't exist yet.  Use the
		// conventional hidden swap file name so that this short-lived
		// probe is covered by standard Vim ignore patterns.
		dirlen = (size_t)(gettail(fname) - fname);
		vim_strncpy(tmp_fname, fname, dirlen);
		fd = -1;
		अत्थाय (i = 4913; ; i += 123)
		{
		    vim_snprintf((वर्ण *)tmp_fname + dirlen,
			परिमाण(tmp_fname) - dirlen, ".%d.swp", i);
		    सचे (mch_lstat((वर्ण *)tmp_fname, &st) < 0)
		    {
			fd = mch_open((वर्ण *)tmp_fname,
				    O_CREAT|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
			सचे (fd < 0 && errno == EEXIST)
			    // If the same file name is created by another
			    // process between lstat() and open(), find another
			    // name.
			    अनुवर्त;
			भङ्ग;
		    }
		}
		सचे (fd < 0)	// can't write in directory
		    backup_copy = TRUE;
		अञ्ञथा
		{
# ifdef UNIX
#  ifdef HAVE_FCHOWN
		    vim_ignored = fchown(fd, st_old.st_uid, st_old.st_gid);
#  endif
#  ifdef HAVE_FCHMOD
		    (शून्य)mch_fsetperm(fd, perm);
#  अञ्ञथा
		    (शून्य)mch_setperm(tmp_fname, perm);
#  endif
		    सचे (mch_stat((वर्ण *)tmp_fname, &st) < 0
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
			पूर्णाङ्क try;

			अत्थाय (try = 0; try < 10; ++try)
			{
			    सचे (mch_lstat((वर्ण *)tmp_fname, &st) < 0)
				भङ्ग;
			    ui_delay(50L, TRUE);  // wait 50 msec
			    mch_remove(tmp_fname);
			}
		    }
# endif
		}
	    }
	}

	// Break symlinks and/or hardlinks if we've been asked to.
	सचे ((bkc & BKC_BREAKSYMLINK) || (bkc & BKC_BREAKHARDLINK))
	{
# ifdef UNIX
	    पूर्णाङ्क	lstat_res;

	    lstat_res = mch_lstat((वर्ण *)fname, &st);

	    // Symlinks.
	    सचे ((bkc & BKC_BREAKSYMLINK)
		    && lstat_res == 0
		    && st.st_ino != st_old.st_ino)
		backup_copy = FALSE;

	    // Hardlinks.
	    सचे ((bkc & BKC_BREAKHARDLINK)
		    && st_old.st_nlink > 1
		    && (lstat_res != 0 || st.st_ino == st_old.st_ino))
		backup_copy = FALSE;
# अञ्ञथा
#  सचे defined(MSWIN)
	    // Symlinks.
	    सचे ((bkc & BKC_BREAKSYMLINK) && mch_is_symbolic_link(fname))
		backup_copy = FALSE;

	    // Hardlinks.
	    सचे ((bkc & BKC_BREAKHARDLINK) && mch_is_hard_link(fname))
		backup_copy = FALSE;
#  endif
# endif
	}

#endif

	// make sure we have a valid backup extension to use
	सचे (*p_bex == NUL)
	    backup_ext = (char_u *)".bak";
	अञ्ञथा
	    backup_ext = p_bex;

	सचे (backup_copy
		&& (fd = mch_open((वर्ण *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
	{
	    पूर्णाङ्क		bfd;
	    char_u	*copybuf;
	    पूर्णाङ्क		some_error = FALSE;
	    stat_T	st_new;
	    char_u	*dirp;
	    char_u	*rootname;
#सचे defined(UNIX)
	    पूर्णाङ्क		did_set_shortname;
	    mode_t	umask_save;
#endif

	    copybuf = alloc(WRITEBUFSIZE + 1);
	    सचे (copybuf == रिक्त)
	    {
		some_error = TRUE;	    // out of memory
		गच्छ nobackup;
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
	    याव (*dirp)
	    {
		char_u	*p UNUSED;
		पूर्णाङ्क	copybuf_len UNUSED;

#ifdef UNIX
		st_new.st_ino = 0;
		st_new.st_dev = 0;
		st_new.st_gid = 0;
#endif

		// Isolate one directory name, using an entry in 'bdir'.
		copybuf_len = copy_option_part(&dirp, copybuf, WRITEBUFSIZE, ",");

#सचे defined(UNIX) || defined(MSWIN)
		p = copybuf + copybuf_len;
		सचे (after_pathsep(copybuf, p) && p[-1] == p[-2])
		    // Ends with '//', use full path
		    सचे ((p = make_percent_swname(copybuf, p, fname)) != रिक्त)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#endif
		rootname = get_file_in_dir(fname, copybuf);
		सचे (rootname == रिक्त)
		{
		    some_error = TRUE;	    // out of memory
		    गच्छ nobackup;
		}

#सचे defined(UNIX)
		did_set_shortname = FALSE;
#endif

		// May try twice if 'shortname' not set.
		अत्थाय (;;)
		{
		    // Make the backup file name.
		    सचे (backup == रिक्त)
			backup = buf_modname((buf->b_p_sn || buf->b_shortname),
						 rootname, backup_ext, FALSE);
		    सचे (backup == रिक्त)
		    {
			vim_free(rootname);
			some_error = TRUE;		// out of memory
			गच्छ nobackup;
		    }

		    // Check if backup file already exists.
		    सचे (mch_stat((वर्ण *)backup, &st_new) >= 0)
		    {
#ifdef UNIX
			// Check if backup file is same as original file.
			// May happen when modname() gave the same file back.
			// E.g. silly link, or file name-length reached.
			// If we don't check here, we either ruin the file
			// when copying or erase it after writing. jw.
			सचे (st_new.st_dev == st_old.st_dev
					    && st_new.st_ino == st_old.st_ino)
			{
			    VIM_CLEAR(backup);	// no backup file to delete
			    // may try again with 'shortname' set
			    सचे (!(buf->b_shortname || buf->b_p_sn))
			    {
				buf->b_shortname = true;
				did_set_shortname = TRUE;
				अनुवर्त;
			    }
				// setting shortname didn't help
			    सचे (did_set_shortname)
				buf->b_shortname = false;
			    भङ्ग;
			}
#endif

			// If we are not going to keep the backup file, don't
			// delete an existing one, try to use another name.
			// Change one character, just before the extension.
			सचे (!p_bk)
			{
			    char_u	*wp;

			    wp = backup + STRLEN(backup) - 1
							 - STRLEN(backup_ext);
			    सचे (wp < backup)	// empty file name ???
				wp = backup;
			    *wp = 'z';
			    याव (*wp > 'a'
				    && mch_stat((वर्ण *)backup, &st_new) >= 0)
				--*wp;
			    // They all exist??? Must be something wrong.
			    सचे (*wp == 'a')
				VIM_CLEAR(backup);
			}
		    }
		    भङ्ग;
		}
		vim_free(rootname);

		// Try to create the backup file
		सचे (backup != रिक्त)
		{
		    // remove old backup, if present
		    mch_remove(backup);
		    // Open with O_EXCL to avoid the file being created while
		    // we were sleeping (symlink hacker attack?). Reset umask
		    // if possible to avoid mch_setperm() below.
#ifdef UNIX
		    umask_save = umask(0);
#endif
		    bfd = mch_open((वर्ण *)backup,
				O_WRONLY|O_CREAT|O_EXTRA|O_EXCL|O_NOFOLLOW,
								 perm & 0777);
#ifdef UNIX
		    (शून्य)umask(umask_save);
#endif
		    सचे (bfd < 0)
			VIM_CLEAR(backup);
		    अञ्ञथा
		    {
			// Set file protection same as original file, but
			// strip s-bit.  Only needed if umask() wasn't used
			// above.
#ifndef UNIX
			(शून्य)mch_setperm(backup, perm & 0777);
#अञ्ञथा
			// Try to set the group of the backup same as the
			// original file. If this fails, set the protection
			// bits for the group same as the protection bits for
			// others.
			सचे (st_new.st_gid != st_old.st_gid
# ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
				&& fchown(bfd, (uid_t)-1, st_old.st_gid) != 0
# endif
						)
			    mch_setperm(backup,
					  (perm & 0707) | ((perm & 07) << 3));
# सचे defined(HAVE_SELINUX) || defined(HAVE_SMACK)
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
			याव ((write_info.bw_len = read_eintr(fd, copybuf,
							    WRITEBUFSIZE)) > 0)
			{
			    सचे (buf_write_bytes(&write_info) == FAIL)
			    {
				errmsg = (char_u *)_(e_cant_write_to_backup_file_add_bang_to_override);
				भङ्ग;
			    }
			    ui_breakcheck();
			    सचे (got_int)
			    {
				errmsg = (char_u *)_(e_interrupted);
				भङ्ग;
			    }
			}

			सचे (close(bfd) < 0 && errmsg == रिक्त)
			    errmsg = (char_u *)_(e_close_error_for_backup_file_add_bang_to_write_anyway);
			सचे (write_info.bw_len < 0)
			    errmsg = (char_u *)_(e_cant_read_file_for_backup_add_bang_to_write_anyway);
#ifdef UNIX
			set_file_time(backup, st_old.st_atime, st_old.st_mtime);
#endif
#ifdef HAVE_ACL
			mch_set_acl(backup, acl);
#endif
#सचे defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
#endif
#ifdef FEAT_XATTR
			mch_copy_xattr(fname, backup);
#endif
#ifdef MSWIN
			(शून्य)mch_copy_file_attribute(fname, backup);
#endif
			भङ्ग;
		    }
		}
	    }
    nobackup:
	    close(fd);		// ignore errors for closing read file
	    vim_free(copybuf);

	    सचे (backup == रिक्त && errmsg == रिक्त)
		errmsg = (char_u *)_(e_cannot_create_backup_file_add_bang_to_write_anyway);
	    // ignore errors when forceit is TRUE
	    सचे ((some_error || errmsg != रिक्त) && !forceit)
	    {
		retval = FAIL;
		गच्छ fail;
	    }
	    errmsg = रिक्त;
	}
	अञ्ञथा
	{
	    char_u	*dirp;
	    char_u	*p;
	    char_u	*rootname;

	    // Make a backup by renaming the original file.

	    // If 'cpoptions' includes the "W" flag, we don't want to
	    // overwrite a read-only file.  But rename may be possible
	    // anyway, thus we need an extra check here.
	    सचे (file_readonly && vim_strchr(p_cpo, CPO_FWRITE) != रिक्त)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_in_cpoptions);
		गच्छ fail;
	    }

	    // Form the backup file name - change path/fo.o.h to
	    // path/fo.o.h.bak Try all directories in 'backupdir', first one
	    // that works is used.
	    dirp = p_bdir;
	    याव (*dirp)
	    {
		पूर्णाङ्क IObufflen UNUSED;

		// Isolate one directory name and make the backup file name.
		IObufflen = copy_option_part(&dirp, IObuff, IOSIZE, ",");

#सचे defined(UNIX) || defined(MSWIN)
		p = IObuff + IObufflen;
		सचे (after_pathsep(IObuff, p) && p[-1] == p[-2])
		    // path ends with '//', use full path
		    सचे ((p = make_percent_swname(IObuff, p, fname)) != रिक्त)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#endif
		सचे (backup == रिक्त)
		{
		    rootname = get_file_in_dir(fname, IObuff);
		    सचे (rootname == रिक्त)
			backup = रिक्त;
		    अञ्ञथा
		    {
			backup = buf_modname(
				(buf->b_p_sn || buf->b_shortname),
						rootname, backup_ext, FALSE);
			vim_free(rootname);
		    }
		}

		सचे (backup != रिक्त)
		{
		    // If we are not going to keep the backup file, don't
		    // delete an existing one, try to use another name.
		    // Change one character, just before the extension.
		    सचे (!p_bk && mch_getperm(backup) >= 0)
		    {
			p = backup + STRLEN(backup) - 1 - STRLEN(backup_ext);
			सचे (p < backup)	// empty file name ???
			    p = backup;
			*p = 'z';
			याव (*p > 'a' && mch_getperm(backup) >= 0)
			    --*p;
			// They all exist??? Must be something wrong!
			सचे (*p == 'a')
			    VIM_CLEAR(backup);
		    }
		}
		सचे (backup != रिक्त)
		{
		    // Delete any existing backup and move the current version
		    // to the backup.	For safety, we don't remove the backup
		    // until the write has finished successfully. And if the
		    // 'backup' option is set, leave it around.

		    // If the renaming of the original file to the backup file
		    // works, quit here.
		    सचे (vim_rename(fname, backup) == 0)
			भङ्ग;

		    VIM_CLEAR(backup);   // don't do the rename below
		}
	    }
	    सचे (backup == रिक्त && !forceit)
	    {
		errmsg = (char_u *)_(e_cant_make_backup_file_add_bang_to_write_anyway);
		गच्छ fail;
	    }
	}
    }

#सचे defined(UNIX)
    // When using ":w!" and the file was read-only: make it writable
    सचे (forceit && perm >= 0 && !(perm & 0200) && st_old.st_uid == getuid()
				     && vim_strchr(p_cpo, CPO_FWRITE) == रिक्त)
    {
	perm |= 0200;
	(शून्य)mch_setperm(fname, perm);
	made_writable = TRUE;
    }
#endif

    // When using ":w!" and writing to the current file, 'readonly' makes no
    // sense, reset it, unless 'Z' appears in 'cpoptions'.
    सचे (forceit && overwriting && vim_strchr(p_cpo, CPO_KEEPRO) == रिक्त)
    {
	buf->b_p_ro = FALSE;
	need_maketitle = TRUE;	    // set window title later
	status_redraw_all();	    // redraw status lines later
    }

    सचे (end > buf->b_ml.ml_पङ्क्तिः_गणनम्)
	end = buf->b_ml.ml_पङ्क्तिः_गणनम्;
    सचे (buf->b_ml.ml_flags & ML_EMPTY)
	start = end + 1;

    // If the original file is being overwritten, there is a small chance that
    // we crash in the middle of writing. Therefore the file is preserved now.
    // This makes all block numbers positive so that recovery does not need
    // the original file.
    // Don't do this if there is a backup file and we are exiting.
    सचे (reset_changed && !newfile && overwriting
					      && !(exiting && backup != रिक्त))
    {
	ml_preserve(buf, FALSE);
	सचे (got_int)
	{
	    errmsg = (char_u *)_(e_interrupted);
	    गच्छ restore_backup;
	}
    }

#ifdef VMS
    vms_remove_version(fname); // remove version
#endif
    // Default: write the file directly.  May write to a temp file for
    // multi-byte conversion.
    wfname = fname;

    // Check for forced 'fileencoding' from "++opt=val" argument.
    सचे (eap != रिक्त && eap->force_enc != 0)
    {
	fenc = eap->cmd + eap->force_enc;
	fenc = enc_canonize(fenc);
	fenc_tofree = fenc;
    }
    अञ्ञथा
	fenc = buf->b_p_fenc;

    // Check if the file needs to be converted.
    converted = need_conversion(fenc);

    // Check if UTF-8 to UCS-2/4 or Latin1 conversion needs to be done.  Or
    // Latin1 to Unicode conversion.  This is handled in buf_write_bytes().
    // Prepare the flags for it and allocate bw_conv_buf when needed.
    सचे (converted && (enc_utf8 || STRCMP(p_enc, "latin1") == 0))
    {
	wb_flags = get_fio_flags(fenc);
	सचे (wb_flags & (FIO_UCS2 | FIO_UCS4 | FIO_UTF16 | FIO_UTF8))
	{
	    // overallocate a bit, in case we read incomplete multi-byte chars
	    पूर्णाङ्क size = bufsize + CONV_RESTLEN;
	    // Need to allocate a buffer to translate into.
	    सचे (wb_flags & (FIO_UCS2 | FIO_UTF16 | FIO_UTF8))
		write_info.bw_conv_buflen = size * 2;
	    अञ्ञथा // FIO_UCS4
		write_info.bw_conv_buflen = size * 4;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    सचे (write_info.bw_conv_buf == रिक्त)
		end = 0;
	}
    }

#ifdef MSWIN
    सचे (converted && wb_flags == 0 && (wb_flags = get_win_fio_flags(fenc)) != 0)
    {
	// Convert UTF-8 -> UCS-2 and UCS-2 -> DBCS.  Worst-case * 4:
	write_info.bw_conv_buflen = bufsize * 4;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	सचे (write_info.bw_conv_buf == रिक्त)
	    end = 0;
    }
#endif

#ifdef MACOS_CONVERT
    सचे (converted && wb_flags == 0 && (wb_flags = get_mac_fio_flags(fenc)) != 0)
    {
	write_info.bw_conv_buflen = bufsize * 3;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	सचे (write_info.bw_conv_buf == रिक्त)
	    end = 0;
    }
#endif

#सचे defined(FEAT_EVAL) || defined(USE_ICONV)
    सचे (converted && wb_flags == 0)
    {
# ifdef USE_ICONV
	// Use iconv() conversion when conversion is needed and it's not done
	// internally.
	write_info.bw_iconv_fd = (iconv_t)my_iconv_open(fenc,
					enc_utf8 ? (char_u *)"utf-8" : p_enc);
	सचे (write_info.bw_iconv_fd != (iconv_t)-1)
	{
	    // We're going to use iconv(), allocate a buffer to convert in.
	    write_info.bw_conv_buflen = bufsize * ICONV_MULT;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    सचे (write_info.bw_conv_buf == रिक्त)
		end = 0;
	    write_info.bw_first = TRUE;
	}
#  ifdef FEAT_EVAL
	अञ्ञथा
#  endif
# endif

# ifdef FEAT_EVAL
	    // When the file needs to be converted with 'charconvert' after
	    // writing, write to a temp file instead and let the conversion
	    // overwrite the original file.
	    सचे (*p_ccv != NUL)
	    {
		wfname = vim_tempname('w', FALSE);
		सचे (wfname == रिक्त)	// Can't write without a tempfile!
		{
		    errmsg = (char_u *)_(e_cant_find_temp_file_for_writing);
		    गच्छ restore_backup;
		}
	    }
# endif
    }
#endif
    सचे (converted && wb_flags == 0
#ifdef USE_ICONV
	    && write_info.bw_iconv_fd == (iconv_t)-1
#endif
#ifdef FEAT_EVAL
	    && wfname == fname
#endif
	    )
    {
	सचे (!forceit)
	{
	    errmsg = (char_u *)_(e_cannot_convert_add_bang_to_write_without_conversion);
	    गच्छ restore_backup;
	}
	notconverted = TRUE;
    }

    // If conversion is taking place, we may first pretend to write and check
    // for conversion errors.  Then loop again to write for real.
    // When not doing conversion this writes for real right away.
    अत्थाय (checking_conversion = TRUE; ; checking_conversion = FALSE)
    {
	// There is no need to check conversion when:
	// - there is no conversion
	// - we make a backup file, that can be restored in case of conversion
	//   failure.
	सचे (!converted || dobackup)
	    checking_conversion = FALSE;

	सचे (checking_conversion)
	{
	    // Make sure we don't write anything.
	    fd = -1;
	    write_info.bw_fd = fd;
	}
	अञ्ञथा
	{
#ifdef HAVE_FTRUNCATE
# define TRUNC_ON_OPEN 0
#अञ्ञथा
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
	    याव ((fd = mch_open((वर्ण *)wfname, O_WRONLY | O_EXTRA | (append
				? (forceit ? (O_APPEND | O_CREAT) : O_APPEND)
				: (O_CREAT | TRUNC_ON_OPEN))
				, perm < 0 ? 0666 : (perm & 0777))) < 0)
	    {
		// A forced write will try to create a new file if the old one
		// is still readonly. This may also happen when the directory
		// is read-only. In that case the mch_remove() will fail.
		सचे (errmsg == रिक्त)
		{
#ifdef UNIX
		    stat_T	st;

		    // Don't delete the file when it's a hard or symbolic link.
		    सचे ((!newfile && st_old.st_nlink > 1)
			    || (mch_lstat((वर्ण *)fname, &st) == 0
				&& (st.st_dev != st_old.st_dev
				    || st.st_ino != st_old.st_ino)))
			errmsg =
			      (char_u *)_(e_cant_open_linked_file_for_writing);
		    अञ्ञथा
#endif
		    {
			errmsg = (char_u *)_(e_cant_open_file_for_writing);
			सचे (forceit && vim_strchr(p_cpo, CPO_FWRITE) == रिक्त
								  && perm >= 0)
			{
#ifdef UNIX
			    // we write to the file, thus it should be marked
			    // writable after all
			    सचे (!(perm & 0200))
				made_writable = TRUE;
			    perm |= 0200;
			    सचे (st_old.st_uid != getuid()
						  || st_old.st_gid != getgid())
				perm &= 0777;
#endif
			    सचे (!append)  // don't remove when appending
				mch_remove(wfname);
			    अनुवर्त;
			}
		    }
		}

restore_backup:
		{
		    stat_T	st;

		    // If we failed to open the file, we don't need a backup.
		    // Throw it away.  If we moved or removed the original file
		    // try to put the backup in its place.
		    सचे (backup != रिक्त && wfname == fname)
		    {
			सचे (backup_copy)
			{
			    // There is a small chance that we removed the
			    // original, try to move the copy in its place.
			    // This may not work if the vim_rename() fails.
			    // In that case we leave the copy around.

			    // If file does not exist, put the copy in its
			    // place
			    सचे (mch_stat((वर्ण *)fname, &st) < 0)
				vim_rename(backup, fname);
			    // if original file does exist throw away the copy
			    सचे (mch_stat((वर्ण *)fname, &st) >= 0)
				mch_remove(backup);
			}
			अञ्ञथा
			{
			    // try to put the original file back
			    vim_rename(backup, fname);
			}
		    }

		    // if original file no longer exists give an extra warning
		    सचे (!newfile && mch_stat((वर्ण *)fname, &st) < 0)
			end = 0;
		}

		सचे (wfname != fname)
		    vim_free(wfname);
		गच्छ fail;
	    }
	    write_info.bw_fd = fd;

#सचे defined(UNIX)
	    {
		stat_T	st;

		// Double check we are writing the intended file before making
		// any changes.
		सचे (overwriting
			&& (!dobackup || backup_copy)
			&& fname == wfname
			&& perm >= 0
			&& mch_fstat(fd, &st) == 0
			&& st.st_ino != st_old.st_ino)
		{
		    close(fd);
		    errmsg = (char_u *)_(e_file_changed_while_writing);
		    गच्छ fail;
		}
	    }
#endif
#ifdef HAVE_FTRUNCATE
	    सचे (!append)
		vim_ignored = ftruncate(fd, (off_t)0);
#endif

#सचे defined(MSWIN)
	    सचे (backup != रिक्त && overwriting && !append)
		(शून्य)mch_copy_file_attribute(backup, wfname);

	    सचे (!overwriting && !append)
	    {
		सचे (buf->b_ffname != रिक्त)
		    (शून्य)mch_copy_file_attribute(buf->b_ffname, wfname);
		// Should copy resource fork
	    }
#endif

#ifdef FEAT_CRYPT
	    सचे (*buf->b_p_कुञ्चिका != NUL && !filtering)
	    {
		char_u		*header;
		पूर्णाङ्क		header_len;

		buf->b_cryptstate = crypt_create_for_writing(
						      crypt_get_method_nr(buf),
					   buf->b_p_कुञ्चिका, &header, &header_len);
		सचे (buf->b_cryptstate == रिक्त || header == रिक्त)
		    end = 0;
		अञ्ञथा
		{
		    // Write magic number, so that Vim knows how this file is
		    // encrypted when reading it back.
		    write_info.bw_buf = header;
		    write_info.bw_len = header_len;
		    write_info.bw_flags = FIO_NOCONVERT;
		    सचे (buf_write_bytes(&write_info) == FAIL)
			end = 0;
		    wb_flags |= FIO_ENCRYPTED;
		    vim_free(header);
		}
	    }
#endif
	}
	errmsg = रिक्त;

	write_info.bw_buf = buffer;
	nchars = 0;

	// use "++bin", "++nobin" or 'binary'
	सचे (eap != रिक्त && eap->force_bin != 0)
	    write_bin = (eap->force_bin == FORCE_BIN);
	अञ्ञथा
	    write_bin = buf->b_p_bin;

	// The BOM is written just after the encryption magic number.
	// Skip it when appending and the file already existed, the BOM only
	// makes sense at the start of the file.
	सचे (buf->b_p_bomb && !write_bin && (!append || perm < 0))
	{
	    write_info.bw_len = make_bom(buffer, fenc);
	    सचे (write_info.bw_len > 0)
	    {
		// don't convert, do encryption
		write_info.bw_flags = FIO_NOCONVERT | wb_flags;
		सचे (buf_write_bytes(&write_info) == FAIL)
		    end = 0;
		अञ्ञथा
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
			    && (buf->b_cryptstate == रिक्त
				|| crypt_works_inplace(buf->b_cryptstate))
# endif
			    && reset_changed
			    && !checking_conversion);
# ifdef CRYPT_NOT_INPLACE
	// remove undo file if encrypting it is not possible
	सचे (buf->b_p_udf
		&& overwriting
		&& !append
		&& !filtering
		&& !checking_conversion
		&& buf->b_cryptstate != रिक्त
		&& !crypt_works_inplace(buf->b_cryptstate))
	    u_undofile_reset_and_delete(buf);
# endif
	सचे (write_undo_file)
	    // Prepare for computing the hash value of the text.
	    sha256_start(&sha_ctx);
#endif

	write_info.bw_len = bufsize;
	write_info.bw_flags = wb_flags;
	fileformat = get_fileformat_force(buf, eap);
	s = buffer;
	len = 0;
	अत्थाय (lnum = start; lnum <= end; ++lnum)
	{
	    // The next while loop is done once for each character written.
	    // Keep it fast!
	    ptr = ml_get_buf(buf, lnum, FALSE) - 1;
#ifdef FEAT_PERSISTENT_UNDO
	    सचे (write_undo_file)
		sha256_update(&sha_ctx, ptr + 1,
					      (UINT32_T)(STRLEN(ptr + 1) + 1));
#endif
	    याव ((c = *++ptr) != NUL)
	    {
		सचे (c == NL)
		    *s = NUL;		// replace newlines with NULs
		अञ्ञथा सचे (c == CAR && fileformat == EOL_MAC)
		    *s = NL;		// Mac: replace CRs with NLs
		अञ्ञथा
		    *s = c;
		++s;
		सचे (++len != bufsize)
		    अनुवर्त;
#ifdef FEAT_CRYPT
		सचे (write_info.bw_fd > 0 && lnum == end
			&& (write_info.bw_flags & FIO_ENCRYPTED)
			&& *buf->b_p_कुञ्चिका != NUL && !filtering
			&& *ptr == NUL)
		    write_info.bw_finish = TRUE;
#endif
		सचे (buf_write_bytes(&write_info) == FAIL)
		{
		    end = 0;		// write error: break loop
		    भङ्ग;
		}
		nchars += bufsize;
		s = buffer;
		len = 0;
		write_info.bw_start_lnum = lnum;
	    }
	    // write failed or last line has no EOL: stop here
	    सचे (end == 0
		    || (lnum == end
			&& (write_bin || !buf->b_p_fixeol)
			&& ((write_bin && lnum == buf->b_no_eol_lnum)
			    || (lnum == buf->b_ml.ml_पङ्क्तिः_गणनम्
							   && !buf->b_p_eol))))
	    {
		++lnum;			// written the line, count it
		no_eol = TRUE;
		भङ्ग;
	    }
	    सचे (fileformat == EOL_UNIX)
		*s++ = NL;
	    अञ्ञथा
	    {
		*s++ = CAR;		    // EOL_MAC or EOL_DOS: write CR
		सचे (fileformat == EOL_DOS)  // write CR-NL
		{
		    सचे (++len == bufsize)
		    {
			सचे (buf_write_bytes(&write_info) == FAIL)
			{
			    end = 0;	// write error: break loop
			    भङ्ग;
			}
			nchars += bufsize;
			s = buffer;
			len = 0;
		    }
		    *s++ = NL;
		}
	    }
	    सचे (++len == bufsize && end)
	    {
		सचे (buf_write_bytes(&write_info) == FAIL)
		{
		    end = 0;		// write error: break loop
		    भङ्ग;
		}
		nchars += bufsize;
		s = buffer;
		len = 0;

		ui_breakcheck();
		सचे (got_int)
		{
		    end = 0;		// Interrupted, break loop
		    भङ्ग;
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
	    सचे (buf->b_fab_rfm == FAB$C_VFC
		    || ((buf->b_fab_rat & (FAB$M_FTN | FAB$M_CR)) != 0))
	    {
		पूर्णाङ्क b2write;

		buf->b_fab_mrs = (buf->b_fab_mrs == 0
			? MIN(4096, bufsize)
			: MIN(buf->b_fab_mrs, bufsize));

		b2write = len;
		याव (b2write > 0)
		{
		    write_info.bw_len = MIN(b2write, buf->b_fab_mrs);
		    सचे (buf_write_bytes(&write_info) == FAIL)
		    {
			end = 0;
			भङ्ग;
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
	सचे (len > 0 && end > 0)
	{
	    write_info.bw_len = len;
#ifdef FEAT_CRYPT
	    सचे (write_info.bw_fd > 0 && lnum >= end
		    && (write_info.bw_flags & FIO_ENCRYPTED)
		    && *buf->b_p_कुञ्चिका != NUL && !filtering)
		write_info.bw_finish = TRUE;
#endif
	    सचे (buf_write_bytes(&write_info) == FAIL)
		end = 0;		    // write error
	    nchars += len;
	}

	सचे (!buf->b_p_fixeol && buf->b_p_eof)
	{
	    // write trailing CTRL-Z
	    (शून्य)write_eintr(write_info.bw_fd, "\x1a", 1);
	    nchars++;
	}

	// Stop when writing done or an error was encountered.
	सचे (!checking_conversion || end == 0)
	    भङ्ग;

	// If no error happened until now, writing should be ok, so loop to
	// really write the buffer.
    }

    // If we started writing, finish writing. Also when an error was
    // encountered.
    सचे (!checking_conversion)
    {
#सचे defined(UNIX) && defined(HAVE_FSYNC)
	// On many journaling file systems there is a bug that causes both the
	// original and the backup file to be lost when halting the system
	// right after writing the file.  That's because only the meta-data is
	// journalled.  Syncing the file slows down the system, but assures it
	// has been written to disk and we don't lose it.
	// For a device do try the fsync() but don't complain if it does not
	// work (could be a pipe).
	// If the 'fsync' option is FALSE, don't fsync().  Useful for laptops.
	सचे ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && vim_fsync(fd) != 0
		&& !device)
	{
	    errmsg = (char_u *)_(e_fsync_failed);
	    end = 0;
	}
#endif

#सचे defined(HAVE_SELINUX) || defined(HAVE_SMACK) || defined(FEAT_XATTR)
	// Probably need to set the security context.
	सचे (!backup_copy)
	{
# सचे defined(HAVE_SELINUX) || defined(HAVE_SMACK)
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
	सचे (backup != रिक्त && !backup_copy)
	{
# ifdef HAVE_FCHOWN
	    stat_T	st;

	    // Don't change the owner when it's already OK, some systems remove
	    // permission or ACL stuff.
	    सचे (mch_stat((वर्ण *)wfname, &st) < 0
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid)
	    {
		// changing owner might not be possible
		vim_ignored = fchown(fd, st_old.st_uid, -1);
		// if changing group fails clear the group permissions
		सचे (fchown(fd, -1, st_old.st_gid) == -1 && perm > 0)
		    perm &= ~070;
	    }
# endif
	    buf_setino(buf);
	}
	अञ्ञथा सचे (!buf->b_dev_valid)
	    // Set the inode when creating a new file.
	    buf_setino(buf);
#endif

#ifdef UNIX
	सचे (made_writable)
	    perm &= ~0200;	// reset 'w' bit for security reasons
#endif
#ifdef HAVE_FCHMOD
	// set permission of new file same as old file
	सचे (perm >= 0)
	    (शून्य)mch_fsetperm(fd, perm);
#endif
	सचे (close(fd) != 0)
	{
	    errmsg = (char_u *)_(e_close_failed);
	    end = 0;
	}

#ifndef HAVE_FCHMOD
	// set permission of new file same as old file
	सचे (perm >= 0)
	    (शून्य)mch_setperm(wfname, perm);
#endif
#ifdef HAVE_ACL
	// Probably need to set the ACL before changing the user (can't set the
	// ACL on a file the user doesn't own).
	// On Solaris, with ZFS and the aclmode property set to "discard" (the
	// default), chmod() discards all part of a file's ACL that don't
	// represent the mode of the file.  It's non-trivial for us to discover
	// whether we're in that situation, so we simply always re-set the ACL.
# ifndef HAVE_SOLARIS_ZFS_ACL
	सचे (!backup_copy)
# endif
	    mch_set_acl(wfname, acl);
#endif
#ifdef FEAT_CRYPT
	सचे (buf->b_cryptstate != रिक्त)
	{
	    crypt_free_state(buf->b_cryptstate);
	    buf->b_cryptstate = रिक्त;
	}
#endif

#सचे defined(FEAT_EVAL)
	सचे (wfname != fname)
	{
	    // The file was written to a temp file, now it needs to be
	    // converted with 'charconvert' to (overwrite) the output file.
	    सचे (end != 0)
	    {
		सचे (eval_charconvert(enc_utf8 ? (char_u *)"utf-8" : p_enc,
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

    सचे (end == 0)
    {
	// Error encountered.
	सचे (errmsg == रिक्त)
	{
	    सचे (write_info.bw_conv_error)
	    {
		सचे (write_info.bw_conv_error_lnum == 0)
		    errmsg = (char_u *)_(e_write_error_conversion_failed_make_fenc_empty_to_override);
		अञ्ञथा
		{
		    errmsg_allocated = TRUE;
		    errmsg = alloc(300);
		    सचे (errmsg == रिक्त)
			गच्छ fail;
		    vim_snprintf((वर्ण *)errmsg, 300, _(e_write_error_conversion_failed_in_पङ्क्तिः_nr_make_fenc_empty_to_override),
					 (long)write_info.bw_conv_error_lnum);
		}
	    }
	    अञ्ञथा सचे (got_int)
		errmsg = (char_u *)_(e_interrupted);
	    अञ्ञथा
		errmsg = (char_u *)_(e_write_error_file_system_full);
	}

	// If we have a backup file, try to put it in place of the new file,
	// because the new file is probably corrupt.  This avoids losing the
	// original file when trying to make a backup when writing the file a
	// second time.
	// When "backup_copy" is set we need to copy the backup over the new
	// file.  Otherwise rename the backup file.
	// If this is OK, don't give the extra warning message.
	सचे (backup != रिक्त)
	{
	    सचे (backup_copy)
	    {
		// This may take a while, if we were interrupted let the user
		// know we got the message.
		सचे (got_int)
		{
		    सन्देशः(_(e_interrupted));
		    out_flush();
		}
		सचे ((fd = mch_open((वर्ण *)backup, O_RDONLY | O_EXTRA, 0)) >= 0)
		{
		    सचे ((write_info.bw_fd = mch_open((वर्ण *)fname,
				    O_WRONLY | O_CREAT | O_TRUNC | O_EXTRA,
							   perm & 0777)) >= 0)
		    {
			// copy the file.
			write_info.bw_buf = smallbuf;
			write_info.bw_flags = FIO_NOCONVERT;
			याव ((write_info.bw_len = read_eintr(fd, smallbuf,
						      SMALLBUFSIZE)) > 0)
			    सचे (buf_write_bytes(&write_info) == FAIL)
				भङ्ग;

			सचे (close(write_info.bw_fd) >= 0
						   && write_info.bw_len == 0)
			    end = 1;		// success
		    }
		    close(fd);	// ignore errors for closing read file
		}
	    }
	    अञ्ञथा
	    {
		सचे (vim_rename(backup, fname) == 0)
		    end = 1;
	    }
	}
	गच्छ fail;
    }

    lnum -= start;	    // compute number of written lines
    --no_wait_return;	    // may wait for return now

#सचे !(defined(UNIX) || defined(VMS))
    fname = sfname;	    // use shortname now, for the messages
#endif
    सचे (!filtering)
    {
	सन्देशः_add_fname(buf, fname);	// put fname in IObuff with quotes
	c = FALSE;
	सचे (write_info.bw_conv_error)
	{
	    STRCAT(IObuff, _(" CONVERSION ERROR"));
	    c = TRUE;
	    सचे (write_info.bw_conv_error_lnum != 0)
		vim_snprintf_add((वर्ण *)IObuff, IOSIZE, _(" in line %ld;"),
			(long)write_info.bw_conv_error_lnum);
	}
	अञ्ञथा सचे (notconverted)
	{
	    STRCAT(IObuff, _("[NOT converted]"));
	    c = TRUE;
	}
	अञ्ञथा सचे (converted)
	{
	    STRCAT(IObuff, _("[converted]"));
	    c = TRUE;
	}
	सचे (device)
	{
	    STRCAT(IObuff, _("[Device]"));
	    c = TRUE;
	}
	अञ्ञथा सचे (newfile)
	{
	    STRCAT(IObuff, new_file_message());
	    c = TRUE;
	}
	सचे (no_eol)
	{
	    सन्देशः_add_eol();
	    c = TRUE;
	}
	// may add [unix/dos/mac]
	सचे (सन्देशः_add_fileformat(fileformat))
	    c = TRUE;
#ifdef FEAT_CRYPT
	सचे (wb_flags & FIO_ENCRYPTED)
	{
	    crypt_append_सन्देशः(buf);
	    c = TRUE;
	}
#endif
	सन्देशः_add_पङ्क्तयः(c, (long)lnum, nchars);	// add line/char count
	सचे (!shortmess(SHM_WRITE))
	{
	    सचे (append)
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [a]") : _(" appended"));
	    अञ्ञथा
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [w]") : _(" written"));
	}

	set_keep_सन्देशः((char_u *)सन्देशः_trunc_attr((वर्ण *)IObuff, FALSE, 0), 0);
    }

    // When written everything correctly: reset 'modified'.  Unless not
    // writing to the original file and '+' is not in 'cpoptions'.
    सचे (reset_changed && whole && !append
	    && !write_info.bw_conv_error
	    && (overwriting || vim_strchr(p_cpo, CPO_PLUS) != रिक्त))
    {
	unchanged(buf, TRUE, FALSE);
	// b:changedtick may be incremented in unchanged() but that should not
	// trigger a TextChanged event.
	सचे (buf->b_last_changedtick + 1 == CHANGEDTICK(buf))
	    buf->b_last_changedtick = CHANGEDTICK(buf);
	u_unchanged(buf);
	u_update_save_nr(buf);
    }

    // If written to the current file, update the timestamp of the swap file
    // and reset the BF_WRITE_MASK flags. Also sets buf->b_mtime.
    सचे (overwriting)
    {
	ml_timestamp(buf);
	सचे (append)
	    buf->b_flags &= ~BF_NEW;
	अञ्ञथा
	    buf->b_flags &= ~BF_WRITE_MASK;
    }

    // If we kept a backup until now, and we are in patch mode, then we make
    // the backup file our 'original' file.
    सचे (*p_pm && dobackup)
    {
	वर्ण *org = (वर्ण *)buf_modname((buf->b_p_sn || buf->b_shortname),
							  fname, p_pm, FALSE);

	सचे (backup != रिक्त)
	{
	    stat_T	st;

	    // If the original file does not exist yet
	    // the current backup file becomes the original file
	    सचे (org == रिक्त)
		emsg(_(e_patchmode_cant_save_original_file));
	    अञ्ञथा सचे (mch_stat(org, &st) < 0)
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
	अञ्ञथा
	{
	    पूर्णाङ्क empty_fd;

	    सचे (org == रिक्त
		    || (empty_fd = mch_open(org,
				      O_CREAT | O_EXTRA | O_EXCL | O_NOFOLLOW,
					perm < 0 ? 0666 : (perm & 0777))) < 0)
	      emsg(_(e_patchmode_cant_touch_empty_original_file));
	    अञ्ञथा
	      close(empty_fd);
	}
	सचे (org != रिक्त)
	{
	    mch_setperm((char_u *)org, mch_getperm(fname) & 0777);
	    vim_free(org);
	}
    }

    // Remove the backup unless 'backup' option is set or there was a
    // conversion error.
    सचे (!p_bk && backup != रिक्त && !write_info.bw_conv_error
	    && mch_remove(backup) != 0)
	emsg(_(e_cant_delete_backup_file));

    गच्छ nofail;

    // Finish up.  We get here either after failure or success.
fail:
    --no_wait_return;		// may wait for return now
nofail:

    // Done saving, we accept changed buffer warnings again
    buf->b_saving = false;

    vim_free(backup);
    सचे (buffer != smallbuf)
	vim_free(buffer);
    vim_free(fenc_tofree);
    vim_free(write_info.bw_conv_buf);
#ifdef USE_ICONV
    सचे (write_info.bw_iconv_fd != (iconv_t)-1)
    {
	iconv_close(write_info.bw_iconv_fd);
	write_info.bw_iconv_fd = (iconv_t)-1;
    }
#endif
#ifdef HAVE_ACL
    mch_free_acl(acl);
#endif

    सचे (errmsg != रिक्त)
    {
	पूर्णाङ्क numlen = errnum != रिक्त ? (पूर्णाङ्क)STRLEN(errnum) : 0;

	attr = HL_ATTR(HLF_E);	// set highlight for error messages
	सन्देशः_add_fname(buf,
#ifndef UNIX
		sfname
#अञ्ञथा
		fname
#endif
		     );		// put file name in IObuff with quotes
	सचे (STRLEN(IObuff) + STRLEN(errmsg) + numlen >= IOSIZE)
	    IObuff[IOSIZE - STRLEN(errmsg) - numlen - 1] = NUL;
	// If the error message has the form "is ...", put the error number in
	// front of the file name.
	सचे (errnum != रिक्त)
	{
	    STRMOVE(IObuff + numlen, IObuff);
	    mch_memmove(IObuff, errnum, (size_t)numlen);
	}
	STRCAT(IObuff, errmsg);
	emsg((वर्ण *)IObuff);
	सचे (errmsg_allocated)
	    vim_free(errmsg);

	retval = FAIL;
	सचे (end == 0)
	{
	    सन्देशः_puts_attr(_("\nWARNING: Original file may be lost or damaged\n"),
		    attr | सन्देशः_HIST);
	    सन्देशः_puts_attr(_("don't quit the editor until the file is successfully written!"),
		    attr | सन्देशः_HIST);

	    // Update the timestamp to avoid an "overwrite changed file"
	    // prompt when writing again.
	    सचे (mch_stat((वर्ण *)fname, &st_old) >= 0)
	    {
		buf_store_time(buf, &st_old, fname);
		buf->b_mtime_read = buf->b_mtime;
		buf->b_mtime_read_ns = buf->b_mtime_ns;
	    }
	}
    }
    सन्देशः_scroll = सन्देशः_save;

#ifdef FEAT_PERSISTENT_UNDO
    // When writing the whole file and 'undofile' is set, also write the undo
    // file.
    सचे (retval == OK && write_undo_file)
    {
	char_u	    hash[UNDO_HASH_SIZE];

	sha256_finish(&sha_ctx, hash);
	u_write_undo(रिक्त, FALSE, buf, hash);
    }
#endif

#ifdef FEAT_EVAL
    सचे (!should_abort(retval))
#अञ्ञथा
    सचे (!got_int)
#endif
    {
	aco_save_T	aco;

	curbuf->b_no_eol_lnum = 0;  // in case it was set by the previous read

	// Apply POST autocommands.
	// Careful: The autocommands may call buf_write() recursively!
	// Only do this when a window was found for "buf".
	aucmd_prepbuf(&aco, buf);
	सचे (curbuf == buf)
	{
	    सचे (append)
		apply_autocmds_exarg(EVENT_FILEAPPENDPOST, fname, fname,
							   FALSE, curbuf, eap);
	    अञ्ञथा सचे (filtering)
		apply_autocmds_exarg(EVENT_FILTERWRITEPOST, रिक्त, fname,
							   FALSE, curbuf, eap);
	    अञ्ञथा सचे (reset_changed && whole)
		apply_autocmds_exarg(EVENT_BUFWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);
	    अञ्ञथा
		apply_autocmds_exarg(EVENT_FILEWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}

#ifdef FEAT_EVAL
	सचे (aborting())	    // autocmds may abort script processing
	    retval = FALSE;
#endif
    }

#ifdef FEAT_VIMINFO
    // Make sure marks will be written out to the viminfo file later, even when
    // the file is new.
    curbuf->b_marks_read = true;
#endif

    got_int |= prev_got_int;

    पटिनिवत्त retval;
}
