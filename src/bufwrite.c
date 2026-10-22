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

#অন্তর্ভাৱ "vim.h"

#যদি defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
# include <utime.h>		// for struct utimbuf
#endif

#define SMALLBUFSIZE	256	// size of emergency write buffer

/*
 * Structure to pass arguments from buf_write() to buf_write_bytes().
 */
সংরচনা bw_info
{
    পূর্ণাঙ্ক		bw_fd;		// file descriptor
    char_u	*bw_buf;	// buffer with data to be written
    পূর্ণাঙ্ক		bw_len;		// length of data
    পূর্ণাঙ্ক		bw_flags;	// FIO_ flags
#ifdef FEAT_CRYPT
    buf_T	*bw_buffer;	// buffer being written
    পূর্ণাঙ্ক		bw_finish;	// finish encrypting
#endif
    char_u	bw_rest[CONV_RESTLEN]; // not converted bytes
    পূর্ণাঙ্ক		bw_restlen;	// nr of bytes in bw_rest[]
    পূর্ণাঙ্ক		bw_first;	// first write call
    char_u	*bw_conv_buf;	// buffer for writing converted chars
    size_t	bw_conv_buflen; // size of bw_conv_buf
    পূর্ণাঙ্ক		bw_conv_error;	// set for conversion error
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
    স্থির পূর্ণাঙ্ক
ucs2bytes(
    unsigned	c,		// in: character
    char_u	**pp,		// in/out: pointer to result
    পূর্ণাঙ্ক		flags)		// FIO_ flags
{
    char_u	*p = *pp;
    পূর্ণাঙ্ক		error = FALSE;
    পূর্ণাঙ্ক		cc;


    যদি (flags & FIO_UCS4)
    {
	যদি (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	    *p++ = (c >> 16);
	    *p++ = (c >> 24);
	}
	অন্যথা
	{
	    *p++ = (c >> 24);
	    *p++ = (c >> 16);
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    অন্যথা যদি (flags & (FIO_UCS2 | FIO_UTF16))
    {
	যদি (c >= 0x10000)
	{
	    যদি (flags & FIO_UTF16)
	    {
		// Make two words, ten bits of the character in each.  First
		// word is 0xd800 - 0xdbff, second one 0xdc00 - 0xdfff
		c -= 0x10000;
		যদি (c >= 0x100000)
		    error = TRUE;
		cc = ((c >> 10) & 0x3ff) + 0xd800;
		যদি (flags & FIO_ENDIAN_L)
		{
		    *p++ = cc;
		    *p++ = ((unsigned)cc >> 8);
		}
		অন্যথা
		{
		    *p++ = ((unsigned)cc >> 8);
		    *p++ = cc;
		}
		c = (c & 0x3ff) + 0xdc00;
	    }
	    অন্যথা
		error = TRUE;
	}
	যদি (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	}
	অন্যথা
	{
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    অন্যথা    // Latin1
    {
	যদি (c >= 0x100)
	{
	    error = TRUE;
	    *p++ = 0xBF;
	}
	অন্যথা
	    *p++ = c;
    }

    *pp = p;
    ঘূৰাই error;
}

/*
 * Call write() to write a number of bytes to the file.
 * Handles encryption and 'encoding' conversion.
 *
 * Return FAIL for failure, OK otherwise.
 */
    স্থির পূর্ণাঙ্ক
buf_write_bytes(সংরচনা bw_info *ip)
{
    পূর্ণাঙ্ক		wlen;
    char_u	*buf = ip->bw_buf;	// data to write
    পূর্ণাঙ্ক		len = ip->bw_len;	// length of data
    পূর্ণাঙ্ক		flags = ip->bw_flags;	// extra flags

    // Skip conversion when writing the crypt magic number or the BOM.
    যদি (!(flags & FIO_NOCONVERT))
    {
	char_u		*p;
	unsigned	c;
	পূর্ণাঙ্ক		n;

	যদি (flags & FIO_UTF8)
	{
	    // Convert latin1 in the buffer to UTF-8 in the file.
	    p = ip->bw_conv_buf;	// translate to buffer
	    বাবে (wlen = 0; wlen < len; ++wlen)
		p += utf_char2bytes(buf[wlen], p);
	    buf = ip->bw_conv_buf;
	    len = (পূর্ণাঙ্ক)(p - ip->bw_conv_buf);
	}
	অন্যথা যদি (flags & (FIO_UCS4 | FIO_UTF16 | FIO_UCS2 | FIO_LATIN1))
	{
	    // Convert UTF-8 bytes in the buffer to UCS-2, UCS-4, UTF-16 or
	    // Latin1 chars in the file.
	    যদি (flags & FIO_LATIN1)
		p = buf;	// translate in-place (can only get shorter)
	    অন্যথা
		p = ip->bw_conv_buf;	// translate to buffer
	    বাবে (wlen = 0; wlen < len; wlen += n)
	    {
		যদি (wlen == 0 && ip->bw_restlen != 0)
		{
		    পূর্ণাঙ্ক		l;

		    // Use remainder of previous call.  Append the start of
		    // buf[] to get a full sequence.  Might still be too
		    // short!
		    l = CONV_RESTLEN - ip->bw_restlen;
		    যদি (l > len)
			l = len;
		    mch_memmove(ip->bw_rest + ip->bw_restlen, buf, (size_t)l);
		    n = utf_ptr2len_len(ip->bw_rest, ip->bw_restlen + l);
		    যদি (n > ip->bw_restlen + len)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			যদি (ip->bw_restlen + len > CONV_RESTLEN)
			    ঘূৰাই FAIL;
			ip->bw_restlen += len;
			ভঙ্গ;
		    }
		    যদি (n > 1)
			c = utf_ptr2char(ip->bw_rest);
		    অন্যথা
			c = ip->bw_rest[0];
		    যদি (n >= ip->bw_restlen)
		    {
			n -= ip->bw_restlen;
			ip->bw_restlen = 0;
		    }
		    অন্যথা
		    {
			ip->bw_restlen -= n;
			mch_memmove(ip->bw_rest, ip->bw_rest + n,
						      (size_t)ip->bw_restlen);
			n = 0;
		    }
		}
		অন্যথা
		{
		    n = utf_ptr2len_len(buf + wlen, len - wlen);
		    যদি (n > len - wlen)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			যদি (len - wlen > CONV_RESTLEN)
			    ঘূৰাই FAIL;
			ip->bw_restlen = len - wlen;
			mch_memmove(ip->bw_rest, buf + wlen,
						      (size_t)ip->bw_restlen);
			ভঙ্গ;
		    }
		    যদি (n > 1)
			c = utf_ptr2char(buf + wlen);
		    অন্যথা
			c = buf[wlen];
		}
		// Check that there is enough space
		যদি (!(flags & FIO_LATIN1))
		{
		    size_t need = (flags & FIO_UCS4) ? 4 : 2;
		    যদি ((flags & FIO_UTF16) && c >= 0x10000)
			need = 4;

		    যদি ((size_t)(p - ip->bw_conv_buf) + need > ip->bw_conv_buflen)
			ঘূৰাই FAIL;
		}

		যদি (ucs2bytes(c, &p, flags) && !ip->bw_conv_error)
		{
		    ip->bw_conv_error = TRUE;
		    ip->bw_conv_error_lnum = ip->bw_start_lnum;
		}
		যদি (c == NL)
		    ++ip->bw_start_lnum;
	    }
	    যদি (flags & FIO_LATIN1)
		len = (পূর্ণাঙ্ক)(p - buf);
	    অন্যথা
	    {
		buf = ip->bw_conv_buf;
		len = (পূর্ণাঙ্ক)(p - ip->bw_conv_buf);
	    }
	}

#ifdef MSWIN
	অন্যথা যদি (flags & FIO_CODEPAGE)
	{
	    // Convert UTF-8 or codepage to UCS-2 and then to MS-Windows
	    // codepage.
	    char_u	*from;
	    size_t	fromlen;
	    char_u	*to;
	    পূর্ণাঙ্ক		u8c;
	    BOOL	bad = FALSE;
	    পূর্ণাঙ্ক		needed;

	    যদি (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    অন্যথা
	    {
		from = buf;
		fromlen = len;
	    }

	    to = ip->bw_conv_buf;
	    যদি (enc_utf8)
	    {
		// Convert from UTF-8 to UCS-2, to the start of the buffer.
		// The buffer has been allocated to be big enough.
		যেতিয়ালৈ (fromlen > 0)
		{
		    n = (পূর্ণাঙ্ক)utf_ptr2len_len(from, (পূর্ণাঙ্ক)fromlen);
		    যদি (n > (পূর্ণাঙ্ক)fromlen)	// incomplete byte sequence
			ভঙ্গ;
		    u8c = utf_ptr2char(from);
		    *to++ = (u8c & 0xff);
		    *to++ = (u8c >> 8);
		    fromlen -= n;
		    from += n;
		}

		// Copy remainder to ip->bw_rest[] to be used for the next
		// call.
		যদি (fromlen > CONV_RESTLEN)
		{
		    // weird overlong sequence
		    ip->bw_conv_error = TRUE;
		    ঘূৰাই FAIL;
		}
		mch_memmove(ip->bw_rest, from, fromlen);
		ip->bw_restlen = (পূর্ণাঙ্ক)fromlen;
	    }
	    অন্যথা
	    {
		// Convert from enc_codepage to UCS-2, to the start of the
		// buffer.  The buffer has been allocated to be big enough.
		ip->bw_restlen = 0;
		needed = MultiByteToWideChar(enc_codepage,
			     MB_ERR_INVALID_CHARS, (LPCSTR)from, (পূর্ণাঙ্ক)fromlen,
								     রিক্ত, 0);
		যদি (needed == 0)
		{
		    // When conversion fails there may be a trailing byte.
		    needed = MultiByteToWideChar(enc_codepage,
			 MB_ERR_INVALID_CHARS, (LPCSTR)from, (পূর্ণাঙ্ক)fromlen - 1,
								     রিক্ত, 0);
		    যদি (needed == 0)
		    {
			// Conversion doesn't work.
			ip->bw_conv_error = TRUE;
			ঘূৰাই FAIL;
		    }
		    // Save the trailing byte for the next call.
		    ip->bw_rest[0] = from[fromlen - 1];
		    ip->bw_restlen = 1;
		}
		needed = MultiByteToWideChar(enc_codepage, MB_ERR_INVALID_CHARS,
				(LPCSTR)from, (পূর্ণাঙ্ক)(fromlen - ip->bw_restlen),
							  (LPWSTR)to, needed);
		যদি (needed == 0)
		{
		    // Safety check: Conversion doesn't work.
		    ip->bw_conv_error = TRUE;
		    ঘূৰাই FAIL;
		}
		to += needed * 2;
	    }

	    fromlen = to - ip->bw_conv_buf;
	    buf = to;
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	    যদি (FIO_GET_CP(flags) == CP_UTF8)
	    {
		// Convert from UCS-2 to UTF-8, using the remainder of the
		// conversion buffer.  Fails when out of space.
		বাবে (from = ip->bw_conv_buf; fromlen > 1; fromlen -= 2)
		{
		    u8c = *from++;
		    u8c += (*from++ << 8);
		    to += utf_char2bytes(u8c, to);
		    যদি (to + 6 >= ip->bw_conv_buf + ip->bw_conv_buflen)
		    {
			ip->bw_conv_error = TRUE;
			ঘূৰাই FAIL;
		    }
		}
		len = (পূর্ণাঙ্ক)(to - buf);
	    }
	    অন্যথা
# endif
	    {
		// Convert from UCS-2 to the codepage, using the remainder of
		// the conversion buffer.  If the conversion uses the default
		// character "0", the data doesn't fit in this encoding, so
		// fail.
		len = WideCharToMultiByte(FIO_GET_CP(flags), 0,
			(LPCWSTR)ip->bw_conv_buf, (পূর্ণাঙ্ক)fromlen / পরিমাণ(WCHAR),
			(LPSTR)to, (পূর্ণাঙ্ক)(ip->bw_conv_buflen - fromlen), 0,
									&bad);
		যদি (bad)
		{
		    ip->bw_conv_error = TRUE;
		    ঘূৰাই FAIL;
		}
	    }
	}
#endif

#ifdef MACOS_CONVERT
	অন্যথা যদি (flags & FIO_MACROMAN)
	{
	    // Convert UTF-8 or latin1 to Apple MacRoman.
	    char_u	*from;
	    size_t	fromlen;

	    যদি (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    অন্যথা
	    {
		from = buf;
		fromlen = len;
	    }

	    যদি (enc2macroman(from, fromlen,
			ip->bw_conv_buf, &len, ip->bw_conv_buflen,
			ip->bw_rest, &ip->bw_restlen) == FAIL)
	    {
		ip->bw_conv_error = TRUE;
		ঘূৰাই FAIL;
	    }
	    buf = ip->bw_conv_buf;
	}
#endif

#ifdef USE_ICONV
	যদি (ip->bw_iconv_fd != (iconv_t)-1)
	{
	    ধ্রুৱ ৱর্ণ	*from;
	    size_t	fromlen;
	    ৱর্ণ	*to;
	    size_t	tolen;

	    // Convert with iconv().
	    যদি (ip->bw_restlen > 0)
	    {
		ৱর্ণ *fp;

		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		fp = (ৱর্ণ *)ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(fp, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(fp + ip->bw_restlen, buf, (size_t)len);
		from = fp;
		tolen = ip->bw_conv_buflen - fromlen;
	    }
	    অন্যথা
	    {
		from = (ধ্রুৱ ৱর্ণ *)buf;
		fromlen = len;
		tolen = ip->bw_conv_buflen;
	    }
	    to = (ৱর্ণ *)ip->bw_conv_buf;

	    যদি (ip->bw_first)
	    {
		size_t	save_len = tolen;

		// output the initial shift state sequence
		(শূন্য)iconv(ip->bw_iconv_fd, রিক্ত, রিক্ত, &to, &tolen);

		// There is a bug in iconv() on Linux (which appears to be
		// wide-spread) which sets "to" to NULL and messes up "tolen".
		যদি (to == রিক্ত)
		{
		    to = (ৱর্ণ *)ip->bw_conv_buf;
		    tolen = save_len;
		}
		ip->bw_first = FALSE;
	    }

	    // If iconv() has an error or there is not enough room, fail.
	    যদি ((iconv(ip->bw_iconv_fd, (শূন্য *)&from, &fromlen, &to, &tolen)
			== (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						    || fromlen > CONV_RESTLEN)
	    {
		ip->bw_conv_error = TRUE;
		ঘূৰাই FAIL;
	    }

	    // copy remainder to ip->bw_rest[] to be used for the next call.
	    যদি (fromlen > 0)
		mch_memmove(ip->bw_rest, (শূন্য *)from, fromlen);
	    ip->bw_restlen = (পূর্ণাঙ্ক)fromlen;

	    buf = ip->bw_conv_buf;
	    len = (পূর্ণাঙ্ক)((char_u *)to - ip->bw_conv_buf);
	}
#endif
    }

    যদি (ip->bw_fd < 0)
	// Only checking conversion, which is OK if we get here.
	ঘূৰাই OK;

#ifdef FEAT_CRYPT
    যদি (flags & FIO_ENCRYPTED)
    {
	// Encrypt the data. Do it in-place if possible, otherwise use an
	// allocated buffer.
# ifdef CRYPT_NOT_INPLACE
	যদি (crypt_works_inplace(ip->bw_buffer->b_cryptstate))
	{
# endif
	    crypt_encode_inplace(ip->bw_buffer->b_cryptstate, buf, len,
								ip->bw_finish);
# ifdef CRYPT_NOT_INPLACE
	}
	অন্যথা
	{
	    char_u *outbuf;

	    len = crypt_encode_alloc(curbuf->b_cryptstate, buf, len, &outbuf,
								ip->bw_finish);
	    যদি (len == 0)
		ঘূৰাই OK;  // Crypt layer is buffering, will flush later.
	    wlen = write_eintr(ip->bw_fd, outbuf, len);
	    vim_free(outbuf);
	    ঘূৰাই (wlen < len) ? FAIL : OK;
	}
# endif
    }
#endif

    wlen = write_eintr(ip->bw_fd, buf, len);
    ঘূৰাই (wlen < len) ? FAIL : OK;
}

/*
 * Check modification time of file, before writing to it.
 * The size isn't checked, because using a tool like "gzip" takes care of
 * using the same timestamp but can't set the size.
 */
    স্থির পূর্ণাঙ্ক
check_mtime(buf_T *buf, stat_T *st)
{
    যদি (buf->b_mtime_read != 0
		  && time_differs(st, buf->b_mtime_read, buf->b_mtime_read_ns))
    {
	বাৰ্তা_scroll = TRUE;	    // don't overwrite messages here
	বাৰ্তা_silent = 0;		    // must give this prompt
	// don't use emsg() here, don't want to flush the buffers
	বাৰ্তা_attr(_("WARNING: The file has been changed since reading it!!!"),
						       HL_ATTR(HLF_E));
	যদি (ask_yesno((char_u *)_("Do you really want to write to it"),
								 TRUE) == 'n')
	    ঘূৰাই FAIL;
	বাৰ্তা_scroll = FALSE;	    // always overwrite the file message now
    }
    ঘূৰাই OK;
}

/*
 * Generate a BOM in "buf[4]" for encoding "name".
 * Return the length of the BOM (zero when no BOM).
 */
    স্থির পূর্ণাঙ্ক
make_bom(char_u *buf, char_u *name)
{
    পূর্ণাঙ্ক		flags;
    char_u	*p;

    flags = get_fio_flags(name);

    // Can't put a BOM in a non-Unicode file.
    যদি (flags == FIO_LATIN1 || flags == 0)
	ঘূৰাই 0;

    যদি (flags == FIO_UTF8)	// UTF-8
    {
	buf[0] = 0xef;
	buf[1] = 0xbb;
	buf[2] = 0xbf;
	ঘূৰাই 3;
    }
    p = buf;
    (শূন্য)ucs2bytes(0xfeff, &p, flags);
    ঘূৰাই (পূর্ণাঙ্ক)(p - buf);
}

#ifdef UNIX
    স্থির শূন্য
set_file_time(
    char_u  *fname,
    time_t  atime,	    // access time
    time_t  mtime)	    // modification time
{
# যদি defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
    সংরচনা utimbuf  buf;

    buf.actime	= atime;
    buf.modtime	= mtime;
    (শূন্য)utime((ৱর্ণ *)fname, &buf);
# অন্যথা
#  যদি defined(HAVE_UTIMES)
    সংরচনা timeval  tvp[2];

    tvp[0].tv_sec   = atime;
    tvp[0].tv_usec  = 0;
    tvp[1].tv_sec   = mtime;
    tvp[1].tv_usec  = 0;
    (শূন্য)utimes((ৱর্ণ *)fname, (ধ্রুৱ সংরচনা timeval *)&tvp);
#  endif
# endif
}
#endif // UNIX

    ৱর্ণ *
new_file_message(শূন্য)
{
    ঘূৰাই shortmess(SHM_NEW) ? _("[New]") : _("[New File]");
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
    পূর্ণাঙ্ক
buf_write(
    buf_T	    *buf,
    char_u	    *fname,
    char_u	    *sfname,
    linenr_T	    start,
    linenr_T	    end,
    exarg_T	    *eap,		// for forced 'ff' and 'fenc', can be
					// NULL!
    পূর্ণাঙ্ক		    append,		// append to the file
    পূর্ণাঙ্ক		    forceit,
    পূর্ণাঙ্ক		    reset_changed,
    পূর্ণাঙ্ক		    filtering)
{
    পূর্ণাঙ্ক		    fd;
    char_u	    *backup = রিক্ত;
    পূর্ণাঙ্ক		    backup_copy = FALSE; // copy the original file?
    পূর্ণাঙ্ক		    dobackup;
    char_u	    *ffname;
    char_u	    *wfname = রিক্ত;	// name of file to write to
    char_u	    *s;
    char_u	    *ptr;
    char_u	    c;
    পূর্ণাঙ্ক		    len;
    linenr_T	    lnum;
    long	    nchars;
    char_u	    *errmsg = রিক্ত;
    পূর্ণাঙ্ক		    errmsg_allocated = FALSE;
    char_u	    *errnum = রিক্ত;
    char_u	    *buffer;
    char_u	    smallbuf[SMALLBUFSIZE];
    char_u	    *backup_ext;
    পূর্ণাঙ্ক		    bufsize;
    long	    perm;		    // file permissions
    পূর্ণাঙ্ক		    retval = OK;
    পূর্ণাঙ্ক		    newfile = FALSE;	    // TRUE if file doesn't exist yet
    পূর্ণাঙ্ক		    বাৰ্তা_save = বাৰ্তা_scroll;
    পূর্ণাঙ্ক		    overwriting;	    // TRUE if writing over original
    পূর্ণাঙ্ক		    no_eol = FALSE;	    // no end-of-line written
    পূর্ণাঙ্ক		    device = FALSE;	    // writing to a device
    stat_T	    st_old;
    পূর্ণাঙ্ক		    prev_got_int = got_int;
    পূর্ণাঙ্ক		    checking_conversion;
    পূর্ণাঙ্ক		    file_readonly = FALSE;  // overwritten file is read-only
#যদি defined(UNIX)			    // XXX fix me sometime?
    পূর্ণাঙ্ক		    made_writable = FALSE;  // 'w' bit has been set
#endif
					// writing everything
    পূর্ণাঙ্ক		    whole = (start == 1 && end == buf->b_ml.ml_সাৰি_গুনতি);
    linenr_T	    old_সাৰি_গুনতি = buf->b_ml.ml_সাৰি_গুনতি;
    পূর্ণাঙ্ক		    attr;
    পূর্ণাঙ্ক		    fileformat;
    পূর্ণাঙ্ক		    write_bin;
    সংরচনা bw_info  write_info;		// info for buf_write_bytes()
    পূর্ণাঙ্ক		    converted = FALSE;
    পূর্ণাঙ্ক		    notconverted = FALSE;
    char_u	    *fenc;		// effective 'fileencoding'
    char_u	    *fenc_tofree = রিক্ত; // allocated "fenc"
    পূর্ণাঙ্ক		    wb_flags = 0;
#ifdef HAVE_ACL
    vim_acl_T	    acl = রিক্ত;		// ACL copied from original file to
					// backup or new file
#endif
#ifdef FEAT_PERSISTENT_UNDO
    পূর্ণাঙ্ক		    write_undo_file = FALSE;
    context_sha256_T sha_ctx;
#endif
    unsigned পূর্ণাঙ্ক    bkc = get_bkc_flags(buf);
    pos_T	    orig_start = buf->b_op_start;
    pos_T	    orig_end = buf->b_op_end;

    যদি (fname == রিক্ত || *fname == NUL)	// safety check
	ঘূৰাই FAIL;
    যদি (buf->b_ml.ml_mfp == রিক্ত)
    {
	// This can happen during startup when there is a stray "w" in the
	// vimrc file.
	emsg(_(e_empty_buffer));
	ঘূৰাই FAIL;
    }

    // Disallow writing from .exrc and .vimrc in current directory for
    // security reasons.
    যদি (check_secure())
	ঘূৰাই FAIL;

    // Avoid a crash for a long name.
    যদি (STRLEN(fname) >= MAXPATHL)
    {
	emsg(_(e_name_too_long));
	ঘূৰাই FAIL;
    }

    // must init bw_conv_buf and bw_iconv_fd before jumping to "fail"
    write_info.bw_conv_buf = রিক্ত;
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
    যদি (buf->b_ffname == রিক্ত
	    && reset_changed
	    && whole
	    && buf == curbuf
	    && !bt_nofilename(buf)
	    && !filtering
	    && (!append || vim_strchr(p_cpo, CPO_FNAMEAPP) != রিক্ত)
	    && vim_strchr(p_cpo, CPO_FNAMEW) != রিক্ত)
    {
	যদি (set_rw_fname(fname, sfname) == FAIL)
	    ঘূৰাই FAIL;
	buf = curbuf;	    // just in case autocmds made "buf" invalid
    }

    যদি (sfname == রিক্ত)
	sfname = fname;
    // For Unix: Use the short file name whenever possible.
    // Avoids problems with networks and when directory names are changed.
    // Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
    // another directory, which we don't detect
    ffname = fname;			    // remember full fname
#ifdef UNIX
    fname = sfname;
#endif

    যদি (buf->b_ffname != রিক্ত && fnamecmp(ffname, buf->b_ffname) == 0)
	overwriting = TRUE;
    অন্যথা
	overwriting = FALSE;

    যদি (exiting)
	settmode(TMODE_COOK);	    // when exiting allow typeahead now

    ++no_wait_return;		    // don't wait for return yet

    // Set '[ and '] marks to the lines to be written.
    buf->b_op_start.lnum = start;
    buf->b_op_start.col = 0;
    buf->b_op_end.lnum = end;
    buf->b_op_end.col = 0;

    {
	aco_save_T	aco;
	পূর্ণাঙ্ক		buf_ffname = FALSE;
	পূর্ণাঙ্ক		buf_sfname = FALSE;
	পূর্ণাঙ্ক		buf_fname_f = FALSE;
	পূর্ণাঙ্ক		buf_fname_s = FALSE;
	পূর্ণাঙ্ক		did_cmd = FALSE;
	পূর্ণাঙ্ক		nofile_err = FALSE;
	পূর্ণাঙ্ক		empty_memline = (buf->b_ml.ml_mfp == রিক্ত);
	bufref_T	bufref;

	// Apply PRE autocommands.
	// Set curbuf to the buffer to be written.
	// Careful: The autocommands may call buf_write() recursively!
	যদি (ffname == buf->b_ffname)
	    buf_ffname = TRUE;
	যদি (sfname == buf->b_sfname)
	    buf_sfname = TRUE;
	যদি (fname == buf->b_ffname)
	    buf_fname_f = TRUE;
	যদি (fname == buf->b_sfname)
	    buf_fname_s = TRUE;

	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, buf);
	যদি (curbuf != buf)
	{
	    // Could not find a window for "buf".  Doing more might cause
	    // problems, better bail out.
	    ঘূৰাই FAIL;
	}

	set_bufref(&bufref, buf);

	যদি (append)
	{
	    যদি (!(did_cmd = apply_autocmds_exarg(EVENT_FILEAPPENDCMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		যদি (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		অন্যথা
		    apply_autocmds_exarg(EVENT_FILEAPPENDPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	অন্যথা যদি (filtering)
	{
	    apply_autocmds_exarg(EVENT_FILTERWRITEPRE,
					    রিক্ত, sfname, FALSE, curbuf, eap);
	}
	অন্যথা যদি (reset_changed && whole)
	{
	    পূর্ণাঙ্ক was_changed = curbufIsChanged();

	    did_cmd = apply_autocmds_exarg(EVENT_BUFWRITECMD,
					  sfname, sfname, FALSE, curbuf, eap);
	    যদি (did_cmd)
	    {
		যদি (was_changed && !curbufIsChanged())
		{
		    // Written everything correctly and BufWriteCmd has reset
		    // 'modified': Correct the undo information so that an
		    // undo now sets 'modified'.
		    u_unchanged(curbuf);
		    u_update_save_nr(curbuf);
		}
	    }
	    অন্যথা
	    {
		যদি (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		অন্যথা
		    apply_autocmds_exarg(EVENT_BUFWRITEPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	অন্যথা
	{
	    যদি (!(did_cmd = apply_autocmds_exarg(EVENT_FILEWRITECMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		যদি (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		অন্যথা
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
	যদি (!bufref_valid(&bufref))
	    buf = রিক্ত;
	যদি (buf == রিক্ত || (buf->b_ml.ml_mfp == রিক্ত && !empty_memline)
				       || did_cmd || nofile_err
#ifdef FEAT_EVAL
				       || aborting()
#endif
				       )
	{
	    যদি (buf != রিক্ত && (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	    {
		// restore the original '[ and '] positions
		buf->b_op_start = orig_start;
		buf->b_op_end = orig_end;
	    }

	    --no_wait_return;
	    বাৰ্তা_scroll = বাৰ্তা_save;
	    যদি (nofile_err)
		semsg(_(e_no_matching_autocommands_for_buftype_str_buffer),
							       curbuf->b_p_bt);

	    যদি (nofile_err
#ifdef FEAT_EVAL
		    || aborting()
#endif
		    )
		// An aborting error, interrupt or exception in the
		// autocommands.
		ঘূৰাই FAIL;
	    যদি (did_cmd)
	    {
		যদি (buf == রিক্ত)
		    // The buffer was deleted.  We assume it was written
		    // (can't retry anyway).
		    ঘূৰাই OK;
		যদি (overwriting)
		{
		    // Assume the buffer was written, update the timestamp.
		    ml_timestamp(buf);
		    যদি (append)
			buf->b_flags &= ~BF_NEW;
		    অন্যথা
			buf->b_flags &= ~BF_WRITE_MASK;
		}
		যদি (reset_changed && buf->b_changed && !append
			&& (overwriting || vim_strchr(p_cpo, CPO_PLUS) != রিক্ত))
		    // Buffer still changed, the autocommands didn't work
		    // properly.
		    ঘূৰাই FAIL;
		ঘূৰাই OK;
	    }
#ifdef FEAT_EVAL
	    যদি (!aborting())
#endif
		emsg(_(e_autocommands_deleted_or_unloaded_buffer_to_be_written));
	    ঘূৰাই FAIL;
	}

	// The autocommands may have changed the number of lines in the file.
	// When writing the whole file, adjust the end.
	// When writing part of the file, assume that the autocommands only
	// changed the number of lines that are to be written (tricky!).
	যদি (buf->b_ml.ml_সাৰি_গুনতি != old_সাৰি_গুনতি)
	{
	    যদি (whole)						// write all
		end = buf->b_ml.ml_সাৰি_গুনতি;
	    অন্যথা যদি (buf->b_ml.ml_সাৰি_গুনতি > old_সাৰি_গুনতি)	// more lines
		end += buf->b_ml.ml_সাৰি_গুনতি - old_সাৰি_গুনতি;
	    অন্যথা						// less lines
	    {
		end -= old_সাৰি_গুনতি - buf->b_ml.ml_সাৰি_গুনতি;
		যদি (end < start)
		{
		    --no_wait_return;
		    বাৰ্তা_scroll = বাৰ্তা_save;
		    emsg(_(e_autocommands_changed_সংখ্যা_of_সাৰিগুলি_in_unexpected_way));
		    ঘূৰাই FAIL;
		}
	    }
	}

	// The autocommands may have changed the name of the buffer, which may
	// be kept in fname, ffname and sfname.  A ":cd" may also have dropped
	// the short name, then b_fname is the full name.
	যদি (buf_ffname)
	    ffname = buf->b_ffname;
	যদি (buf_sfname)
	    sfname = buf->b_fname;
	যদি (buf_fname_f)
	    fname = buf->b_ffname;
	যদি (buf_fname_s)
	    fname = buf->b_fname;
    }

    যদি (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	// restore the original '[ and '] positions
	buf->b_op_start = orig_start;
	buf->b_op_end = orig_end;
    }

#ifdef FEAT_NETBEANS_INTG
    যদি (netbeans_active() && isNetbeansBuffer(buf))
    {
	যদি (whole)
	{
	    // b_changed can be 0 after an undo, but we still need to write
	    // the buffer to NetBeans.
	    যদি (buf->b_changed || isNetbeansModified(buf))
	    {
		--no_wait_return;		// may wait for return now
		বাৰ্তা_scroll = বাৰ্তা_save;
		netbeans_save_buffer(buf);	// no error checking...
		ঘূৰাই retval;
	    }
	    অন্যথা
	    {
		errnum = (char_u *)"E656: ";
		errmsg = (char_u *)_(e_netbeans_disallows_writes_of_unmodified_buffers);
		buffer = রিক্ত;
		গচ্ছ fail;
	    }
	}
	অন্যথা
	{
	    errnum = (char_u *)"E657: ";
	    errmsg = (char_u *)_(e_partial_writes_disallowed_for_netbeans_buffers);
	    buffer = রিক্ত;
	    গচ্ছ fail;
	}
    }
#endif

    যদি (shortmess(SHM_OVER) && !exiting)
	বাৰ্তা_scroll = FALSE;	    // overwrite previous file message
    অন্যথা
	বাৰ্তা_scroll = TRUE;	    // don't overwrite previous file message
    যদি (!filtering)
	filemess(buf,
#ifndef UNIX
		sfname,
#অন্যথা
		fname,
#endif
		    (char_u *)"", 0);	// show that we are busy
    বাৰ্তা_scroll = FALSE;		    // always overwrite the file message now

    buffer = alloc(WRITEBUFSIZE);
    যদি (buffer == রিক্ত)		    // can't allocate big buffer, use small
				    // one (to be able to write when out of
				    // memory)
    {
	buffer = smallbuf;
	bufsize = SMALLBUFSIZE;
    }
    অন্যথা
	bufsize = WRITEBUFSIZE;

    // Get information about original file (if there is one).
#যদি defined(UNIX)
    st_old.st_dev = 0;
    st_old.st_ino = 0;
    perm = -1;
    যদি (mch_stat((ৱর্ণ *)fname, &st_old) < 0)
	newfile = TRUE;
    অন্যথা
    {
	perm = st_old.st_mode;
	যদি (!S_ISREG(st_old.st_mode))		// not a file
	{
	    যদি (S_ISDIR(st_old.st_mode))
	    {
		errnum = (char_u *)"E502: ";
		errmsg = (char_u *)_(e_is_a_directory);
		গচ্ছ fail;
	    }
	    যদি (mch_nodetype(fname) != গ্ৰন্থি_WRITABLE)
	    {
		errnum = (char_u *)"E503: ";
		errmsg = (char_u *)_(e_is_not_file_or_writable_device);
		গচ্ছ fail;
	    }
	    // It's a device of some kind (or a fifo) which we can write to
	    // but for which we can't make a backup.
	    device = TRUE;
	    newfile = TRUE;
	    perm = -1;
	}
    }
#অন্যথা // !UNIX
    // Check for a writable device name.
    c = mch_nodetype(fname);
    যদি (c == গ্ৰন্থি_OTHER)
    {
	errnum = (char_u *)"E503: ";
	errmsg = (char_u *)_(e_is_not_file_or_writable_device);
	গচ্ছ fail;
    }
    যদি (c == গ্ৰন্থি_WRITABLE)
    {
# যদি defined(MSWIN)
	// MS-Windows allows opening a device, but we will probably get stuck
	// trying to write to it.
	যদি (!p_odev)
	{
	    errnum = (char_u *)"E796: ";
	    errmsg = (char_u *)_(e_writing_to_device_disabled_with_opendevice_option);
	    গচ্ছ fail;
	}
# endif
	device = TRUE;
	newfile = TRUE;
	perm = -1;
    }
    অন্যথা
    {
	perm = mch_getperm(fname);
	যদি (perm < 0)
	    newfile = TRUE;
	অন্যথা যদি (mch_isdir(fname))
	{
	    errnum = (char_u *)"E502: ";
	    errmsg = (char_u *)_(e_is_a_directory);
	    গচ্ছ fail;
	}
	যদি (overwriting)
	    (শূন্য)mch_stat((ৱর্ণ *)fname, &st_old);
    }
#endif // !UNIX

    যদি (!device && !newfile)
    {
	// Check if the file is really writable (when renaming the file to
	// make a backup we won't discover it later).
	file_readonly = check_file_readonly(fname, (পূর্ণাঙ্ক)perm);

	যদি (!forceit && file_readonly)
	{
	    যদি (vim_strchr(p_cpo, CPO_FWRITE) != রিক্ত)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_in_cpoptions);
	    }
	    অন্যথা
	    {
		errnum = (char_u *)"E505: ";
		errmsg = (char_u *)_(e_is_read_only_add_bang_to_override);
	    }
	    গচ্ছ fail;
	}

	// Check if the timestamp hasn't changed since reading the file.
	যদি (overwriting)
	{
	    retval = check_mtime(buf, &st_old);
	    যদি (retval == FAIL)
		গচ্ছ fail;
	}
    }

#ifdef HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    যদি (!newfile)
	acl = mch_get_acl(fname);
#endif

    // If 'backupskip' is not empty, don't make a backup for some files.
    dobackup = (p_wb || p_bk || *p_pm != NUL);
    যদি (dobackup && *p_bsk != NUL && match_file_list(p_bsk, sfname, ffname))
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
    যদি (!(append && *p_pm == NUL) && !filtering && perm >= 0 && dobackup)
    {
#যদি defined(UNIX) || defined(MSWIN)
	stat_T	    st;
#endif

	যদি ((bkc & BKC_YES) || append)	// "yes"
	    backup_copy = TRUE;
#যদি defined(UNIX) || defined(MSWIN)
	অন্যথা যদি ((bkc & BKC_AUTO))	// "auto"
	{
# ifdef UNIX
	    // Don't rename the file when:
	    // - it's a hard link
	    // - it's a symbolic link
	    // - we don't have write permission in the directory
	    // - we can't set the owner/group of the new file
	    যদি (st_old.st_nlink > 1
		    || mch_lstat((ৱর্ণ *)fname, &st) < 0
		    || st.st_dev != st_old.st_dev
		    || st.st_ino != st_old.st_ino
#  ifndef HAVE_FCHOWN
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid
#  endif
		    )
		backup_copy = TRUE;
	    অন্যথা
# অন্যথা
#  ifdef MSWIN
	    // On NTFS file systems hard links are possible.
	    যদি (mch_is_linked(fname))
		backup_copy = TRUE;
	    অন্যথা
#  endif
# endif
	    {
		size_t	dirlen;
		char_u	tmp_fname[MAXPATHL];
		পূর্ণাঙ্ক	i;

		// Check if we can create a file and set the owner/group/mode to
		// the ones from the original file.
		// First find a file name that doesn't exist yet.  Use the
		// conventional hidden swap file name so that this short-lived
		// probe is covered by standard Vim ignore patterns.
		dirlen = (size_t)(gettail(fname) - fname);
		vim_strncpy(tmp_fname, fname, dirlen);
		fd = -1;
		বাবে (i = 4913; ; i += 123)
		{
		    vim_snprintf((ৱর্ণ *)tmp_fname + dirlen,
			পরিমাণ(tmp_fname) - dirlen, ".%d.swp", i);
		    যদি (mch_lstat((ৱর্ণ *)tmp_fname, &st) < 0)
		    {
			fd = mch_open((ৱর্ণ *)tmp_fname,
				    O_CREAT|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
			যদি (fd < 0 && errno == EEXIST)
			    // If the same file name is created by another
			    // process between lstat() and open(), find another
			    // name.
			    অনুৱর্ত;
			ভঙ্গ;
		    }
		}
		যদি (fd < 0)	// can't write in directory
		    backup_copy = TRUE;
		অন্যথা
		{
# ifdef UNIX
#  ifdef HAVE_FCHOWN
		    vim_ignored = fchown(fd, st_old.st_uid, st_old.st_gid);
#  endif
#  ifdef HAVE_FCHMOD
		    (শূন্য)mch_fsetperm(fd, perm);
#  অন্যথা
		    (শূন্য)mch_setperm(tmp_fname, perm);
#  endif
		    যদি (mch_stat((ৱর্ণ *)tmp_fname, &st) < 0
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
			পূর্ণাঙ্ক try;

			বাবে (try = 0; try < 10; ++try)
			{
			    যদি (mch_lstat((ৱর্ণ *)tmp_fname, &st) < 0)
				ভঙ্গ;
			    ui_delay(50L, TRUE);  // wait 50 msec
			    mch_remove(tmp_fname);
			}
		    }
# endif
		}
	    }
	}

	// Break symlinks and/or hardlinks if we've been asked to.
	যদি ((bkc & BKC_BREAKSYMLINK) || (bkc & BKC_BREAKHARDLINK))
	{
# ifdef UNIX
	    পূর্ণাঙ্ক	lstat_res;

	    lstat_res = mch_lstat((ৱর্ণ *)fname, &st);

	    // Symlinks.
	    যদি ((bkc & BKC_BREAKSYMLINK)
		    && lstat_res == 0
		    && st.st_ino != st_old.st_ino)
		backup_copy = FALSE;

	    // Hardlinks.
	    যদি ((bkc & BKC_BREAKHARDLINK)
		    && st_old.st_nlink > 1
		    && (lstat_res != 0 || st.st_ino == st_old.st_ino))
		backup_copy = FALSE;
# অন্যথা
#  যদি defined(MSWIN)
	    // Symlinks.
	    যদি ((bkc & BKC_BREAKSYMLINK) && mch_is_symbolic_link(fname))
		backup_copy = FALSE;

	    // Hardlinks.
	    যদি ((bkc & BKC_BREAKHARDLINK) && mch_is_hard_link(fname))
		backup_copy = FALSE;
#  endif
# endif
	}

#endif

	// make sure we have a valid backup extension to use
	যদি (*p_bex == NUL)
	    backup_ext = (char_u *)".bak";
	অন্যথা
	    backup_ext = p_bex;

	যদি (backup_copy
		&& (fd = mch_open((ৱর্ণ *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
	{
	    পূর্ণাঙ্ক		bfd;
	    char_u	*copybuf;
	    পূর্ণাঙ্ক		some_error = FALSE;
	    stat_T	st_new;
	    char_u	*dirp;
	    char_u	*rootname;
#যদি defined(UNIX)
	    পূর্ণাঙ্ক		did_set_shortname;
	    mode_t	umask_save;
#endif

	    copybuf = alloc(WRITEBUFSIZE + 1);
	    যদি (copybuf == রিক্ত)
	    {
		some_error = TRUE;	    // out of memory
		গচ্ছ nobackup;
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
	    যেতিয়ালৈ (*dirp)
	    {
		char_u	*p UNUSED;
		পূর্ণাঙ্ক	copybuf_len UNUSED;

#ifdef UNIX
		st_new.st_ino = 0;
		st_new.st_dev = 0;
		st_new.st_gid = 0;
#endif

		// Isolate one directory name, using an entry in 'bdir'.
		copybuf_len = copy_option_part(&dirp, copybuf, WRITEBUFSIZE, ",");

#যদি defined(UNIX) || defined(MSWIN)
		p = copybuf + copybuf_len;
		যদি (after_pathsep(copybuf, p) && p[-1] == p[-2])
		    // Ends with '//', use full path
		    যদি ((p = make_percent_swname(copybuf, p, fname)) != রিক্ত)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#endif
		rootname = get_file_in_dir(fname, copybuf);
		যদি (rootname == রিক্ত)
		{
		    some_error = TRUE;	    // out of memory
		    গচ্ছ nobackup;
		}

#যদি defined(UNIX)
		did_set_shortname = FALSE;
#endif

		// May try twice if 'shortname' not set.
		বাবে (;;)
		{
		    // Make the backup file name.
		    যদি (backup == রিক্ত)
			backup = buf_modname((buf->b_p_sn || buf->b_shortname),
						 rootname, backup_ext, FALSE);
		    যদি (backup == রিক্ত)
		    {
			vim_free(rootname);
			some_error = TRUE;		// out of memory
			গচ্ছ nobackup;
		    }

		    // Check if backup file already exists.
		    যদি (mch_stat((ৱর্ণ *)backup, &st_new) >= 0)
		    {
#ifdef UNIX
			// Check if backup file is same as original file.
			// May happen when modname() gave the same file back.
			// E.g. silly link, or file name-length reached.
			// If we don't check here, we either ruin the file
			// when copying or erase it after writing. jw.
			যদি (st_new.st_dev == st_old.st_dev
					    && st_new.st_ino == st_old.st_ino)
			{
			    VIM_CLEAR(backup);	// no backup file to delete
			    // may try again with 'shortname' set
			    যদি (!(buf->b_shortname || buf->b_p_sn))
			    {
				buf->b_shortname = true;
				did_set_shortname = TRUE;
				অনুৱর্ত;
			    }
				// setting shortname didn't help
			    যদি (did_set_shortname)
				buf->b_shortname = false;
			    ভঙ্গ;
			}
#endif

			// If we are not going to keep the backup file, don't
			// delete an existing one, try to use another name.
			// Change one character, just before the extension.
			যদি (!p_bk)
			{
			    char_u	*wp;

			    wp = backup + STRLEN(backup) - 1
							 - STRLEN(backup_ext);
			    যদি (wp < backup)	// empty file name ???
				wp = backup;
			    *wp = 'z';
			    যেতিয়ালৈ (*wp > 'a'
				    && mch_stat((ৱর্ণ *)backup, &st_new) >= 0)
				--*wp;
			    // They all exist??? Must be something wrong.
			    যদি (*wp == 'a')
				VIM_CLEAR(backup);
			}
		    }
		    ভঙ্গ;
		}
		vim_free(rootname);

		// Try to create the backup file
		যদি (backup != রিক্ত)
		{
		    // remove old backup, if present
		    mch_remove(backup);
		    // Open with O_EXCL to avoid the file being created while
		    // we were sleeping (symlink hacker attack?). Reset umask
		    // if possible to avoid mch_setperm() below.
#ifdef UNIX
		    umask_save = umask(0);
#endif
		    bfd = mch_open((ৱর্ণ *)backup,
				O_WRONLY|O_CREAT|O_EXTRA|O_EXCL|O_NOFOLLOW,
								 perm & 0777);
#ifdef UNIX
		    (শূন্য)umask(umask_save);
#endif
		    যদি (bfd < 0)
			VIM_CLEAR(backup);
		    অন্যথা
		    {
			// Set file protection same as original file, but
			// strip s-bit.  Only needed if umask() wasn't used
			// above.
#ifndef UNIX
			(শূন্য)mch_setperm(backup, perm & 0777);
#অন্যথা
			// Try to set the group of the backup same as the
			// original file. If this fails, set the protection
			// bits for the group same as the protection bits for
			// others.
			যদি (st_new.st_gid != st_old.st_gid
# ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
				&& fchown(bfd, (uid_t)-1, st_old.st_gid) != 0
# endif
						)
			    mch_setperm(backup,
					  (perm & 0707) | ((perm & 07) << 3));
# যদি defined(HAVE_SELINUX) || defined(HAVE_SMACK)
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
			যেতিয়ালৈ ((write_info.bw_len = read_eintr(fd, copybuf,
							    WRITEBUFSIZE)) > 0)
			{
			    যদি (buf_write_bytes(&write_info) == FAIL)
			    {
				errmsg = (char_u *)_(e_cant_write_to_backup_file_add_bang_to_override);
				ভঙ্গ;
			    }
			    ui_breakcheck();
			    যদি (got_int)
			    {
				errmsg = (char_u *)_(e_interrupted);
				ভঙ্গ;
			    }
			}

			যদি (close(bfd) < 0 && errmsg == রিক্ত)
			    errmsg = (char_u *)_(e_close_error_for_backup_file_add_bang_to_write_anyway);
			যদি (write_info.bw_len < 0)
			    errmsg = (char_u *)_(e_cant_read_file_for_backup_add_bang_to_write_anyway);
#ifdef UNIX
			set_file_time(backup, st_old.st_atime, st_old.st_mtime);
#endif
#ifdef HAVE_ACL
			mch_set_acl(backup, acl);
#endif
#যদি defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
#endif
#ifdef FEAT_XATTR
			mch_copy_xattr(fname, backup);
#endif
#ifdef MSWIN
			(শূন্য)mch_copy_file_attribute(fname, backup);
#endif
			ভঙ্গ;
		    }
		}
	    }
    nobackup:
	    close(fd);		// ignore errors for closing read file
	    vim_free(copybuf);

	    যদি (backup == রিক্ত && errmsg == রিক্ত)
		errmsg = (char_u *)_(e_cannot_create_backup_file_add_bang_to_write_anyway);
	    // ignore errors when forceit is TRUE
	    যদি ((some_error || errmsg != রিক্ত) && !forceit)
	    {
		retval = FAIL;
		গচ্ছ fail;
	    }
	    errmsg = রিক্ত;
	}
	অন্যথা
	{
	    char_u	*dirp;
	    char_u	*p;
	    char_u	*rootname;

	    // Make a backup by renaming the original file.

	    // If 'cpoptions' includes the "W" flag, we don't want to
	    // overwrite a read-only file.  But rename may be possible
	    // anyway, thus we need an extra check here.
	    যদি (file_readonly && vim_strchr(p_cpo, CPO_FWRITE) != রিক্ত)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_in_cpoptions);
		গচ্ছ fail;
	    }

	    // Form the backup file name - change path/fo.o.h to
	    // path/fo.o.h.bak Try all directories in 'backupdir', first one
	    // that works is used.
	    dirp = p_bdir;
	    যেতিয়ালৈ (*dirp)
	    {
		পূর্ণাঙ্ক IObufflen UNUSED;

		// Isolate one directory name and make the backup file name.
		IObufflen = copy_option_part(&dirp, IObuff, IOSIZE, ",");

#যদি defined(UNIX) || defined(MSWIN)
		p = IObuff + IObufflen;
		যদি (after_pathsep(IObuff, p) && p[-1] == p[-2])
		    // path ends with '//', use full path
		    যদি ((p = make_percent_swname(IObuff, p, fname)) != রিক্ত)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#endif
		যদি (backup == রিক্ত)
		{
		    rootname = get_file_in_dir(fname, IObuff);
		    যদি (rootname == রিক্ত)
			backup = রিক্ত;
		    অন্যথা
		    {
			backup = buf_modname(
				(buf->b_p_sn || buf->b_shortname),
						rootname, backup_ext, FALSE);
			vim_free(rootname);
		    }
		}

		যদি (backup != রিক্ত)
		{
		    // If we are not going to keep the backup file, don't
		    // delete an existing one, try to use another name.
		    // Change one character, just before the extension.
		    যদি (!p_bk && mch_getperm(backup) >= 0)
		    {
			p = backup + STRLEN(backup) - 1 - STRLEN(backup_ext);
			যদি (p < backup)	// empty file name ???
			    p = backup;
			*p = 'z';
			যেতিয়ালৈ (*p > 'a' && mch_getperm(backup) >= 0)
			    --*p;
			// They all exist??? Must be something wrong!
			যদি (*p == 'a')
			    VIM_CLEAR(backup);
		    }
		}
		যদি (backup != রিক্ত)
		{
		    // Delete any existing backup and move the current version
		    // to the backup.	For safety, we don't remove the backup
		    // until the write has finished successfully. And if the
		    // 'backup' option is set, leave it around.

		    // If the renaming of the original file to the backup file
		    // works, quit here.
		    যদি (vim_rename(fname, backup) == 0)
			ভঙ্গ;

		    VIM_CLEAR(backup);   // don't do the rename below
		}
	    }
	    যদি (backup == রিক্ত && !forceit)
	    {
		errmsg = (char_u *)_(e_cant_make_backup_file_add_bang_to_write_anyway);
		গচ্ছ fail;
	    }
	}
    }

#যদি defined(UNIX)
    // When using ":w!" and the file was read-only: make it writable
    যদি (forceit && perm >= 0 && !(perm & 0200) && st_old.st_uid == getuid()
				     && vim_strchr(p_cpo, CPO_FWRITE) == রিক্ত)
    {
	perm |= 0200;
	(শূন্য)mch_setperm(fname, perm);
	made_writable = TRUE;
    }
#endif

    // When using ":w!" and writing to the current file, 'readonly' makes no
    // sense, reset it, unless 'Z' appears in 'cpoptions'.
    যদি (forceit && overwriting && vim_strchr(p_cpo, CPO_KEEPRO) == রিক্ত)
    {
	buf->b_p_ro = FALSE;
	need_maketitle = TRUE;	    // set window title later
	status_redraw_all();	    // redraw status lines later
    }

    যদি (end > buf->b_ml.ml_সাৰি_গুনতি)
	end = buf->b_ml.ml_সাৰি_গুনতি;
    যদি (buf->b_ml.ml_flags & ML_EMPTY)
	start = end + 1;

    // If the original file is being overwritten, there is a small chance that
    // we crash in the middle of writing. Therefore the file is preserved now.
    // This makes all block numbers positive so that recovery does not need
    // the original file.
    // Don't do this if there is a backup file and we are exiting.
    যদি (reset_changed && !newfile && overwriting
					      && !(exiting && backup != রিক্ত))
    {
	ml_preserve(buf, FALSE);
	যদি (got_int)
	{
	    errmsg = (char_u *)_(e_interrupted);
	    গচ্ছ restore_backup;
	}
    }

#ifdef VMS
    vms_remove_version(fname); // remove version
#endif
    // Default: write the file directly.  May write to a temp file for
    // multi-byte conversion.
    wfname = fname;

    // Check for forced 'fileencoding' from "++opt=val" argument.
    যদি (eap != রিক্ত && eap->force_enc != 0)
    {
	fenc = eap->cmd + eap->force_enc;
	fenc = enc_canonize(fenc);
	fenc_tofree = fenc;
    }
    অন্যথা
	fenc = buf->b_p_fenc;

    // Check if the file needs to be converted.
    converted = need_conversion(fenc);

    // Check if UTF-8 to UCS-2/4 or Latin1 conversion needs to be done.  Or
    // Latin1 to Unicode conversion.  This is handled in buf_write_bytes().
    // Prepare the flags for it and allocate bw_conv_buf when needed.
    যদি (converted && (enc_utf8 || STRCMP(p_enc, "latin1") == 0))
    {
	wb_flags = get_fio_flags(fenc);
	যদি (wb_flags & (FIO_UCS2 | FIO_UCS4 | FIO_UTF16 | FIO_UTF8))
	{
	    // overallocate a bit, in case we read incomplete multi-byte chars
	    পূর্ণাঙ্ক size = bufsize + CONV_RESTLEN;
	    // Need to allocate a buffer to translate into.
	    যদি (wb_flags & (FIO_UCS2 | FIO_UTF16 | FIO_UTF8))
		write_info.bw_conv_buflen = size * 2;
	    অন্যথা // FIO_UCS4
		write_info.bw_conv_buflen = size * 4;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    যদি (write_info.bw_conv_buf == রিক্ত)
		end = 0;
	}
    }

#ifdef MSWIN
    যদি (converted && wb_flags == 0 && (wb_flags = get_win_fio_flags(fenc)) != 0)
    {
	// Convert UTF-8 -> UCS-2 and UCS-2 -> DBCS.  Worst-case * 4:
	write_info.bw_conv_buflen = bufsize * 4;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	যদি (write_info.bw_conv_buf == রিক্ত)
	    end = 0;
    }
#endif

#ifdef MACOS_CONVERT
    যদি (converted && wb_flags == 0 && (wb_flags = get_mac_fio_flags(fenc)) != 0)
    {
	write_info.bw_conv_buflen = bufsize * 3;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	যদি (write_info.bw_conv_buf == রিক্ত)
	    end = 0;
    }
#endif

#যদি defined(FEAT_EVAL) || defined(USE_ICONV)
    যদি (converted && wb_flags == 0)
    {
# ifdef USE_ICONV
	// Use iconv() conversion when conversion is needed and it's not done
	// internally.
	write_info.bw_iconv_fd = (iconv_t)my_iconv_open(fenc,
					enc_utf8 ? (char_u *)"utf-8" : p_enc);
	যদি (write_info.bw_iconv_fd != (iconv_t)-1)
	{
	    // We're going to use iconv(), allocate a buffer to convert in.
	    write_info.bw_conv_buflen = bufsize * ICONV_MULT;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    যদি (write_info.bw_conv_buf == রিক্ত)
		end = 0;
	    write_info.bw_first = TRUE;
	}
#  ifdef FEAT_EVAL
	অন্যথা
#  endif
# endif

# ifdef FEAT_EVAL
	    // When the file needs to be converted with 'charconvert' after
	    // writing, write to a temp file instead and let the conversion
	    // overwrite the original file.
	    যদি (*p_ccv != NUL)
	    {
		wfname = vim_tempname('w', FALSE);
		যদি (wfname == রিক্ত)	// Can't write without a tempfile!
		{
		    errmsg = (char_u *)_(e_cant_find_temp_file_for_writing);
		    গচ্ছ restore_backup;
		}
	    }
# endif
    }
#endif
    যদি (converted && wb_flags == 0
#ifdef USE_ICONV
	    && write_info.bw_iconv_fd == (iconv_t)-1
#endif
#ifdef FEAT_EVAL
	    && wfname == fname
#endif
	    )
    {
	যদি (!forceit)
	{
	    errmsg = (char_u *)_(e_cannot_convert_add_bang_to_write_without_conversion);
	    গচ্ছ restore_backup;
	}
	notconverted = TRUE;
    }

    // If conversion is taking place, we may first pretend to write and check
    // for conversion errors.  Then loop again to write for real.
    // When not doing conversion this writes for real right away.
    বাবে (checking_conversion = TRUE; ; checking_conversion = FALSE)
    {
	// There is no need to check conversion when:
	// - there is no conversion
	// - we make a backup file, that can be restored in case of conversion
	//   failure.
	যদি (!converted || dobackup)
	    checking_conversion = FALSE;

	যদি (checking_conversion)
	{
	    // Make sure we don't write anything.
	    fd = -1;
	    write_info.bw_fd = fd;
	}
	অন্যথা
	{
#ifdef HAVE_FTRUNCATE
# define TRUNC_ON_OPEN 0
#অন্যথা
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
	    যেতিয়ালৈ ((fd = mch_open((ৱর্ণ *)wfname, O_WRONLY | O_EXTRA | (append
				? (forceit ? (O_APPEND | O_CREAT) : O_APPEND)
				: (O_CREAT | TRUNC_ON_OPEN))
				, perm < 0 ? 0666 : (perm & 0777))) < 0)
	    {
		// A forced write will try to create a new file if the old one
		// is still readonly. This may also happen when the directory
		// is read-only. In that case the mch_remove() will fail.
		যদি (errmsg == রিক্ত)
		{
#ifdef UNIX
		    stat_T	st;

		    // Don't delete the file when it's a hard or symbolic link.
		    যদি ((!newfile && st_old.st_nlink > 1)
			    || (mch_lstat((ৱর্ণ *)fname, &st) == 0
				&& (st.st_dev != st_old.st_dev
				    || st.st_ino != st_old.st_ino)))
			errmsg =
			      (char_u *)_(e_cant_open_linked_file_for_writing);
		    অন্যথা
#endif
		    {
			errmsg = (char_u *)_(e_cant_open_file_for_writing);
			যদি (forceit && vim_strchr(p_cpo, CPO_FWRITE) == রিক্ত
								  && perm >= 0)
			{
#ifdef UNIX
			    // we write to the file, thus it should be marked
			    // writable after all
			    যদি (!(perm & 0200))
				made_writable = TRUE;
			    perm |= 0200;
			    যদি (st_old.st_uid != getuid()
						  || st_old.st_gid != getgid())
				perm &= 0777;
#endif
			    যদি (!append)  // don't remove when appending
				mch_remove(wfname);
			    অনুৱর্ত;
			}
		    }
		}

restore_backup:
		{
		    stat_T	st;

		    // If we failed to open the file, we don't need a backup.
		    // Throw it away.  If we moved or removed the original file
		    // try to put the backup in its place.
		    যদি (backup != রিক্ত && wfname == fname)
		    {
			যদি (backup_copy)
			{
			    // There is a small chance that we removed the
			    // original, try to move the copy in its place.
			    // This may not work if the vim_rename() fails.
			    // In that case we leave the copy around.

			    // If file does not exist, put the copy in its
			    // place
			    যদি (mch_stat((ৱর্ণ *)fname, &st) < 0)
				vim_rename(backup, fname);
			    // if original file does exist throw away the copy
			    যদি (mch_stat((ৱর্ণ *)fname, &st) >= 0)
				mch_remove(backup);
			}
			অন্যথা
			{
			    // try to put the original file back
			    vim_rename(backup, fname);
			}
		    }

		    // if original file no longer exists give an extra warning
		    যদি (!newfile && mch_stat((ৱর্ণ *)fname, &st) < 0)
			end = 0;
		}

		যদি (wfname != fname)
		    vim_free(wfname);
		গচ্ছ fail;
	    }
	    write_info.bw_fd = fd;

#যদি defined(UNIX)
	    {
		stat_T	st;

		// Double check we are writing the intended file before making
		// any changes.
		যদি (overwriting
			&& (!dobackup || backup_copy)
			&& fname == wfname
			&& perm >= 0
			&& mch_fstat(fd, &st) == 0
			&& st.st_ino != st_old.st_ino)
		{
		    close(fd);
		    errmsg = (char_u *)_(e_file_changed_while_writing);
		    গচ্ছ fail;
		}
	    }
#endif
#ifdef HAVE_FTRUNCATE
	    যদি (!append)
		vim_ignored = ftruncate(fd, (off_t)0);
#endif

#যদি defined(MSWIN)
	    যদি (backup != রিক্ত && overwriting && !append)
		(শূন্য)mch_copy_file_attribute(backup, wfname);

	    যদি (!overwriting && !append)
	    {
		যদি (buf->b_ffname != রিক্ত)
		    (শূন্য)mch_copy_file_attribute(buf->b_ffname, wfname);
		// Should copy resource fork
	    }
#endif

#ifdef FEAT_CRYPT
	    যদি (*buf->b_p_চাবি != NUL && !filtering)
	    {
		char_u		*header;
		পূর্ণাঙ্ক		header_len;

		buf->b_cryptstate = crypt_create_for_writing(
						      crypt_get_method_nr(buf),
					   buf->b_p_চাবি, &header, &header_len);
		যদি (buf->b_cryptstate == রিক্ত || header == রিক্ত)
		    end = 0;
		অন্যথা
		{
		    // Write magic number, so that Vim knows how this file is
		    // encrypted when reading it back.
		    write_info.bw_buf = header;
		    write_info.bw_len = header_len;
		    write_info.bw_flags = FIO_NOCONVERT;
		    যদি (buf_write_bytes(&write_info) == FAIL)
			end = 0;
		    wb_flags |= FIO_ENCRYPTED;
		    vim_free(header);
		}
	    }
#endif
	}
	errmsg = রিক্ত;

	write_info.bw_buf = buffer;
	nchars = 0;

	// use "++bin", "++nobin" or 'binary'
	যদি (eap != রিক্ত && eap->force_bin != 0)
	    write_bin = (eap->force_bin == FORCE_BIN);
	অন্যথা
	    write_bin = buf->b_p_bin;

	// The BOM is written just after the encryption magic number.
	// Skip it when appending and the file already existed, the BOM only
	// makes sense at the start of the file.
	যদি (buf->b_p_bomb && !write_bin && (!append || perm < 0))
	{
	    write_info.bw_len = make_bom(buffer, fenc);
	    যদি (write_info.bw_len > 0)
	    {
		// don't convert, do encryption
		write_info.bw_flags = FIO_NOCONVERT | wb_flags;
		যদি (buf_write_bytes(&write_info) == FAIL)
		    end = 0;
		অন্যথা
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
			    && (buf->b_cryptstate == রিক্ত
				|| crypt_works_inplace(buf->b_cryptstate))
# endif
			    && reset_changed
			    && !checking_conversion);
# ifdef CRYPT_NOT_INPLACE
	// remove undo file if encrypting it is not possible
	যদি (buf->b_p_udf
		&& overwriting
		&& !append
		&& !filtering
		&& !checking_conversion
		&& buf->b_cryptstate != রিক্ত
		&& !crypt_works_inplace(buf->b_cryptstate))
	    u_undofile_reset_and_delete(buf);
# endif
	যদি (write_undo_file)
	    // Prepare for computing the hash value of the text.
	    sha256_start(&sha_ctx);
#endif

	write_info.bw_len = bufsize;
	write_info.bw_flags = wb_flags;
	fileformat = get_fileformat_force(buf, eap);
	s = buffer;
	len = 0;
	বাবে (lnum = start; lnum <= end; ++lnum)
	{
	    // The next while loop is done once for each character written.
	    // Keep it fast!
	    ptr = ml_get_buf(buf, lnum, FALSE) - 1;
#ifdef FEAT_PERSISTENT_UNDO
	    যদি (write_undo_file)
		sha256_update(&sha_ctx, ptr + 1,
					      (UINT32_T)(STRLEN(ptr + 1) + 1));
#endif
	    যেতিয়ালৈ ((c = *++ptr) != NUL)
	    {
		যদি (c == NL)
		    *s = NUL;		// replace newlines with NULs
		অন্যথা যদি (c == CAR && fileformat == EOL_MAC)
		    *s = NL;		// Mac: replace CRs with NLs
		অন্যথা
		    *s = c;
		++s;
		যদি (++len != bufsize)
		    অনুৱর্ত;
#ifdef FEAT_CRYPT
		যদি (write_info.bw_fd > 0 && lnum == end
			&& (write_info.bw_flags & FIO_ENCRYPTED)
			&& *buf->b_p_চাবি != NUL && !filtering
			&& *ptr == NUL)
		    write_info.bw_finish = TRUE;
#endif
		যদি (buf_write_bytes(&write_info) == FAIL)
		{
		    end = 0;		// write error: break loop
		    ভঙ্গ;
		}
		nchars += bufsize;
		s = buffer;
		len = 0;
		write_info.bw_start_lnum = lnum;
	    }
	    // write failed or last line has no EOL: stop here
	    যদি (end == 0
		    || (lnum == end
			&& (write_bin || !buf->b_p_fixeol)
			&& ((write_bin && lnum == buf->b_no_eol_lnum)
			    || (lnum == buf->b_ml.ml_সাৰি_গুনতি
							   && !buf->b_p_eol))))
	    {
		++lnum;			// written the line, count it
		no_eol = TRUE;
		ভঙ্গ;
	    }
	    যদি (fileformat == EOL_UNIX)
		*s++ = NL;
	    অন্যথা
	    {
		*s++ = CAR;		    // EOL_MAC or EOL_DOS: write CR
		যদি (fileformat == EOL_DOS)  // write CR-NL
		{
		    যদি (++len == bufsize)
		    {
			যদি (buf_write_bytes(&write_info) == FAIL)
			{
			    end = 0;	// write error: break loop
			    ভঙ্গ;
			}
			nchars += bufsize;
			s = buffer;
			len = 0;
		    }
		    *s++ = NL;
		}
	    }
	    যদি (++len == bufsize && end)
	    {
		যদি (buf_write_bytes(&write_info) == FAIL)
		{
		    end = 0;		// write error: break loop
		    ভঙ্গ;
		}
		nchars += bufsize;
		s = buffer;
		len = 0;

		ui_breakcheck();
		যদি (got_int)
		{
		    end = 0;		// Interrupted, break loop
		    ভঙ্গ;
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
	    যদি (buf->b_fab_rfm == FAB$C_VFC
		    || ((buf->b_fab_rat & (FAB$M_FTN | FAB$M_CR)) != 0))
	    {
		পূর্ণাঙ্ক b2write;

		buf->b_fab_mrs = (buf->b_fab_mrs == 0
			? MIN(4096, bufsize)
			: MIN(buf->b_fab_mrs, bufsize));

		b2write = len;
		যেতিয়ালৈ (b2write > 0)
		{
		    write_info.bw_len = MIN(b2write, buf->b_fab_mrs);
		    যদি (buf_write_bytes(&write_info) == FAIL)
		    {
			end = 0;
			ভঙ্গ;
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
	যদি (len > 0 && end > 0)
	{
	    write_info.bw_len = len;
#ifdef FEAT_CRYPT
	    যদি (write_info.bw_fd > 0 && lnum >= end
		    && (write_info.bw_flags & FIO_ENCRYPTED)
		    && *buf->b_p_চাবি != NUL && !filtering)
		write_info.bw_finish = TRUE;
#endif
	    যদি (buf_write_bytes(&write_info) == FAIL)
		end = 0;		    // write error
	    nchars += len;
	}

	যদি (!buf->b_p_fixeol && buf->b_p_eof)
	{
	    // write trailing CTRL-Z
	    (শূন্য)write_eintr(write_info.bw_fd, "\x1a", 1);
	    nchars++;
	}

	// Stop when writing done or an error was encountered.
	যদি (!checking_conversion || end == 0)
	    ভঙ্গ;

	// If no error happened until now, writing should be ok, so loop to
	// really write the buffer.
    }

    // If we started writing, finish writing. Also when an error was
    // encountered.
    যদি (!checking_conversion)
    {
#যদি defined(UNIX) && defined(HAVE_FSYNC)
	// On many journaling file systems there is a bug that causes both the
	// original and the backup file to be lost when halting the system
	// right after writing the file.  That's because only the meta-data is
	// journalled.  Syncing the file slows down the system, but assures it
	// has been written to disk and we don't lose it.
	// For a device do try the fsync() but don't complain if it does not
	// work (could be a pipe).
	// If the 'fsync' option is FALSE, don't fsync().  Useful for laptops.
	যদি ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && vim_fsync(fd) != 0
		&& !device)
	{
	    errmsg = (char_u *)_(e_fsync_failed);
	    end = 0;
	}
#endif

#যদি defined(HAVE_SELINUX) || defined(HAVE_SMACK) || defined(FEAT_XATTR)
	// Probably need to set the security context.
	যদি (!backup_copy)
	{
# যদি defined(HAVE_SELINUX) || defined(HAVE_SMACK)
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
	যদি (backup != রিক্ত && !backup_copy)
	{
# ifdef HAVE_FCHOWN
	    stat_T	st;

	    // Don't change the owner when it's already OK, some systems remove
	    // permission or ACL stuff.
	    যদি (mch_stat((ৱর্ণ *)wfname, &st) < 0
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid)
	    {
		// changing owner might not be possible
		vim_ignored = fchown(fd, st_old.st_uid, -1);
		// if changing group fails clear the group permissions
		যদি (fchown(fd, -1, st_old.st_gid) == -1 && perm > 0)
		    perm &= ~070;
	    }
# endif
	    buf_setino(buf);
	}
	অন্যথা যদি (!buf->b_dev_valid)
	    // Set the inode when creating a new file.
	    buf_setino(buf);
#endif

#ifdef UNIX
	যদি (made_writable)
	    perm &= ~0200;	// reset 'w' bit for security reasons
#endif
#ifdef HAVE_FCHMOD
	// set permission of new file same as old file
	যদি (perm >= 0)
	    (শূন্য)mch_fsetperm(fd, perm);
#endif
	যদি (close(fd) != 0)
	{
	    errmsg = (char_u *)_(e_close_failed);
	    end = 0;
	}

#ifndef HAVE_FCHMOD
	// set permission of new file same as old file
	যদি (perm >= 0)
	    (শূন্য)mch_setperm(wfname, perm);
#endif
#ifdef HAVE_ACL
	// Probably need to set the ACL before changing the user (can't set the
	// ACL on a file the user doesn't own).
	// On Solaris, with ZFS and the aclmode property set to "discard" (the
	// default), chmod() discards all part of a file's ACL that don't
	// represent the mode of the file.  It's non-trivial for us to discover
	// whether we're in that situation, so we simply always re-set the ACL.
# ifndef HAVE_SOLARIS_ZFS_ACL
	যদি (!backup_copy)
# endif
	    mch_set_acl(wfname, acl);
#endif
#ifdef FEAT_CRYPT
	যদি (buf->b_cryptstate != রিক্ত)
	{
	    crypt_free_state(buf->b_cryptstate);
	    buf->b_cryptstate = রিক্ত;
	}
#endif

#যদি defined(FEAT_EVAL)
	যদি (wfname != fname)
	{
	    // The file was written to a temp file, now it needs to be
	    // converted with 'charconvert' to (overwrite) the output file.
	    যদি (end != 0)
	    {
		যদি (eval_charconvert(enc_utf8 ? (char_u *)"utf-8" : p_enc,
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

    যদি (end == 0)
    {
	// Error encountered.
	যদি (errmsg == রিক্ত)
	{
	    যদি (write_info.bw_conv_error)
	    {
		যদি (write_info.bw_conv_error_lnum == 0)
		    errmsg = (char_u *)_(e_write_error_conversion_failed_make_fenc_empty_to_override);
		অন্যথা
		{
		    errmsg_allocated = TRUE;
		    errmsg = alloc(300);
		    যদি (errmsg == রিক্ত)
			গচ্ছ fail;
		    vim_snprintf((ৱর্ণ *)errmsg, 300, _(e_write_error_conversion_failed_in_সাৰি_nr_make_fenc_empty_to_override),
					 (long)write_info.bw_conv_error_lnum);
		}
	    }
	    অন্যথা যদি (got_int)
		errmsg = (char_u *)_(e_interrupted);
	    অন্যথা
		errmsg = (char_u *)_(e_write_error_file_system_full);
	}

	// If we have a backup file, try to put it in place of the new file,
	// because the new file is probably corrupt.  This avoids losing the
	// original file when trying to make a backup when writing the file a
	// second time.
	// When "backup_copy" is set we need to copy the backup over the new
	// file.  Otherwise rename the backup file.
	// If this is OK, don't give the extra warning message.
	যদি (backup != রিক্ত)
	{
	    যদি (backup_copy)
	    {
		// This may take a while, if we were interrupted let the user
		// know we got the message.
		যদি (got_int)
		{
		    বাৰ্তা(_(e_interrupted));
		    out_flush();
		}
		যদি ((fd = mch_open((ৱর্ণ *)backup, O_RDONLY | O_EXTRA, 0)) >= 0)
		{
		    যদি ((write_info.bw_fd = mch_open((ৱর্ণ *)fname,
				    O_WRONLY | O_CREAT | O_TRUNC | O_EXTRA,
							   perm & 0777)) >= 0)
		    {
			// copy the file.
			write_info.bw_buf = smallbuf;
			write_info.bw_flags = FIO_NOCONVERT;
			যেতিয়ালৈ ((write_info.bw_len = read_eintr(fd, smallbuf,
						      SMALLBUFSIZE)) > 0)
			    যদি (buf_write_bytes(&write_info) == FAIL)
				ভঙ্গ;

			যদি (close(write_info.bw_fd) >= 0
						   && write_info.bw_len == 0)
			    end = 1;		// success
		    }
		    close(fd);	// ignore errors for closing read file
		}
	    }
	    অন্যথা
	    {
		যদি (vim_rename(backup, fname) == 0)
		    end = 1;
	    }
	}
	গচ্ছ fail;
    }

    lnum -= start;	    // compute number of written lines
    --no_wait_return;	    // may wait for return now

#যদি !(defined(UNIX) || defined(VMS))
    fname = sfname;	    // use shortname now, for the messages
#endif
    যদি (!filtering)
    {
	বাৰ্তা_add_fname(buf, fname);	// put fname in IObuff with quotes
	c = FALSE;
	যদি (write_info.bw_conv_error)
	{
	    STRCAT(IObuff, _(" CONVERSION ERROR"));
	    c = TRUE;
	    যদি (write_info.bw_conv_error_lnum != 0)
		vim_snprintf_add((ৱর্ণ *)IObuff, IOSIZE, _(" in line %ld;"),
			(long)write_info.bw_conv_error_lnum);
	}
	অন্যথা যদি (notconverted)
	{
	    STRCAT(IObuff, _("[NOT converted]"));
	    c = TRUE;
	}
	অন্যথা যদি (converted)
	{
	    STRCAT(IObuff, _("[converted]"));
	    c = TRUE;
	}
	যদি (device)
	{
	    STRCAT(IObuff, _("[Device]"));
	    c = TRUE;
	}
	অন্যথা যদি (newfile)
	{
	    STRCAT(IObuff, new_file_message());
	    c = TRUE;
	}
	যদি (no_eol)
	{
	    বাৰ্তা_add_eol();
	    c = TRUE;
	}
	// may add [unix/dos/mac]
	যদি (বাৰ্তা_add_fileformat(fileformat))
	    c = TRUE;
#ifdef FEAT_CRYPT
	যদি (wb_flags & FIO_ENCRYPTED)
	{
	    crypt_append_বাৰ্তা(buf);
	    c = TRUE;
	}
#endif
	বাৰ্তা_add_সাৰিগুলি(c, (long)lnum, nchars);	// add line/char count
	যদি (!shortmess(SHM_WRITE))
	{
	    যদি (append)
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [a]") : _(" appended"));
	    অন্যথা
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [w]") : _(" written"));
	}

	set_keep_বাৰ্তা((char_u *)বাৰ্তা_trunc_attr((ৱর্ণ *)IObuff, FALSE, 0), 0);
    }

    // When written everything correctly: reset 'modified'.  Unless not
    // writing to the original file and '+' is not in 'cpoptions'.
    যদি (reset_changed && whole && !append
	    && !write_info.bw_conv_error
	    && (overwriting || vim_strchr(p_cpo, CPO_PLUS) != রিক্ত))
    {
	unchanged(buf, TRUE, FALSE);
	// b:changedtick may be incremented in unchanged() but that should not
	// trigger a TextChanged event.
	যদি (buf->b_last_changedtick + 1 == CHANGEDTICK(buf))
	    buf->b_last_changedtick = CHANGEDTICK(buf);
	u_unchanged(buf);
	u_update_save_nr(buf);
    }

    // If written to the current file, update the timestamp of the swap file
    // and reset the BF_WRITE_MASK flags. Also sets buf->b_mtime.
    যদি (overwriting)
    {
	ml_timestamp(buf);
	যদি (append)
	    buf->b_flags &= ~BF_NEW;
	অন্যথা
	    buf->b_flags &= ~BF_WRITE_MASK;
    }

    // If we kept a backup until now, and we are in patch mode, then we make
    // the backup file our 'original' file.
    যদি (*p_pm && dobackup)
    {
	ৱর্ণ *org = (ৱর্ণ *)buf_modname((buf->b_p_sn || buf->b_shortname),
							  fname, p_pm, FALSE);

	যদি (backup != রিক্ত)
	{
	    stat_T	st;

	    // If the original file does not exist yet
	    // the current backup file becomes the original file
	    যদি (org == রিক্ত)
		emsg(_(e_patchmode_cant_save_original_file));
	    অন্যথা যদি (mch_stat(org, &st) < 0)
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
	অন্যথা
	{
	    পূর্ণাঙ্ক empty_fd;

	    যদি (org == রিক্ত
		    || (empty_fd = mch_open(org,
				      O_CREAT | O_EXTRA | O_EXCL | O_NOFOLLOW,
					perm < 0 ? 0666 : (perm & 0777))) < 0)
	      emsg(_(e_patchmode_cant_touch_empty_original_file));
	    অন্যথা
	      close(empty_fd);
	}
	যদি (org != রিক্ত)
	{
	    mch_setperm((char_u *)org, mch_getperm(fname) & 0777);
	    vim_free(org);
	}
    }

    // Remove the backup unless 'backup' option is set or there was a
    // conversion error.
    যদি (!p_bk && backup != রিক্ত && !write_info.bw_conv_error
	    && mch_remove(backup) != 0)
	emsg(_(e_cant_delete_backup_file));

    গচ্ছ nofail;

    // Finish up.  We get here either after failure or success.
fail:
    --no_wait_return;		// may wait for return now
nofail:

    // Done saving, we accept changed buffer warnings again
    buf->b_saving = false;

    vim_free(backup);
    যদি (buffer != smallbuf)
	vim_free(buffer);
    vim_free(fenc_tofree);
    vim_free(write_info.bw_conv_buf);
#ifdef USE_ICONV
    যদি (write_info.bw_iconv_fd != (iconv_t)-1)
    {
	iconv_close(write_info.bw_iconv_fd);
	write_info.bw_iconv_fd = (iconv_t)-1;
    }
#endif
#ifdef HAVE_ACL
    mch_free_acl(acl);
#endif

    যদি (errmsg != রিক্ত)
    {
	পূর্ণাঙ্ক numlen = errnum != রিক্ত ? (পূর্ণাঙ্ক)STRLEN(errnum) : 0;

	attr = HL_ATTR(HLF_E);	// set highlight for error messages
	বাৰ্তা_add_fname(buf,
#ifndef UNIX
		sfname
#অন্যথা
		fname
#endif
		     );		// put file name in IObuff with quotes
	যদি (STRLEN(IObuff) + STRLEN(errmsg) + numlen >= IOSIZE)
	    IObuff[IOSIZE - STRLEN(errmsg) - numlen - 1] = NUL;
	// If the error message has the form "is ...", put the error number in
	// front of the file name.
	যদি (errnum != রিক্ত)
	{
	    STRMOVE(IObuff + numlen, IObuff);
	    mch_memmove(IObuff, errnum, (size_t)numlen);
	}
	STRCAT(IObuff, errmsg);
	emsg((ৱর্ণ *)IObuff);
	যদি (errmsg_allocated)
	    vim_free(errmsg);

	retval = FAIL;
	যদি (end == 0)
	{
	    বাৰ্তা_puts_attr(_("\nWARNING: Original file may be lost or damaged\n"),
		    attr | বাৰ্তা_HIST);
	    বাৰ্তা_puts_attr(_("don't quit the editor until the file is successfully written!"),
		    attr | বাৰ্তা_HIST);

	    // Update the timestamp to avoid an "overwrite changed file"
	    // prompt when writing again.
	    যদি (mch_stat((ৱর্ণ *)fname, &st_old) >= 0)
	    {
		buf_store_time(buf, &st_old, fname);
		buf->b_mtime_read = buf->b_mtime;
		buf->b_mtime_read_ns = buf->b_mtime_ns;
	    }
	}
    }
    বাৰ্তা_scroll = বাৰ্তা_save;

#ifdef FEAT_PERSISTENT_UNDO
    // When writing the whole file and 'undofile' is set, also write the undo
    // file.
    যদি (retval == OK && write_undo_file)
    {
	char_u	    hash[UNDO_HASH_SIZE];

	sha256_finish(&sha_ctx, hash);
	u_write_undo(রিক্ত, FALSE, buf, hash);
    }
#endif

#ifdef FEAT_EVAL
    যদি (!should_abort(retval))
#অন্যথা
    যদি (!got_int)
#endif
    {
	aco_save_T	aco;

	curbuf->b_no_eol_lnum = 0;  // in case it was set by the previous read

	// Apply POST autocommands.
	// Careful: The autocommands may call buf_write() recursively!
	// Only do this when a window was found for "buf".
	aucmd_prepbuf(&aco, buf);
	যদি (curbuf == buf)
	{
	    যদি (append)
		apply_autocmds_exarg(EVENT_FILEAPPENDPOST, fname, fname,
							   FALSE, curbuf, eap);
	    অন্যথা যদি (filtering)
		apply_autocmds_exarg(EVENT_FILTERWRITEPOST, রিক্ত, fname,
							   FALSE, curbuf, eap);
	    অন্যথা যদি (reset_changed && whole)
		apply_autocmds_exarg(EVENT_BUFWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);
	    অন্যথা
		apply_autocmds_exarg(EVENT_FILEWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}

#ifdef FEAT_EVAL
	যদি (aborting())	    // autocmds may abort script processing
	    retval = FALSE;
#endif
    }

#ifdef FEAT_VIMINFO
    // Make sure marks will be written out to the viminfo file later, even when
    // the file is new.
    curbuf->b_marks_read = true;
#endif

    got_int |= prev_got_int;

    ঘূৰাই retval;
}
