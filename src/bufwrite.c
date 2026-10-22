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

#شامل "vim.h"

#اگر defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
# include <utime.h>		// for struct utimbuf
#endif

#define SMALLBUFSIZE	256	// size of emergency write buffer

/*
 * Structure to pass arguments from buf_write() to buf_write_bytes().
 */
ساختار bw_info
{
    صحیح		bw_fd;		// file descriptor
    char_u	*bw_buf;	// buffer with data to be written
    صحیح		bw_len;		// length of data
    صحیح		bw_flags;	// FIO_ flags
#ifdef FEAT_CRYPT
    buf_T	*bw_buffer;	// buffer being written
    صحیح		bw_finish;	// finish encrypting
#endif
    char_u	bw_rest[CONV_RESTLEN]; // not converted bytes
    صحیح		bw_restlen;	// nr of bytes in bw_rest[]
    صحیح		bw_first;	// first write call
    char_u	*bw_conv_buf;	// buffer for writing converted chars
    size_t	bw_conv_buflen; // size of bw_conv_buf
    صحیح		bw_conv_error;	// set for conversion error
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
    ایستا صحیح
ucs2bytes(
    unsigned	c,		// in: character
    char_u	**pp,		// in/out: pointer to result
    صحیح		flags)		// FIO_ flags
{
    char_u	*p = *pp;
    صحیح		error = FALSE;
    صحیح		cc;


    اگر (flags & FIO_UCS4)
    {
	اگر (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	    *p++ = (c >> 16);
	    *p++ = (c >> 24);
	}
	وگرنه
	{
	    *p++ = (c >> 24);
	    *p++ = (c >> 16);
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    وگرنه اگر (flags & (FIO_UCS2 | FIO_UTF16))
    {
	اگر (c >= 0x10000)
	{
	    اگر (flags & FIO_UTF16)
	    {
		// Make two words, ten bits of the character in each.  First
		// word is 0xd800 - 0xdbff, second one 0xdc00 - 0xdfff
		c -= 0x10000;
		اگر (c >= 0x100000)
		    error = TRUE;
		cc = ((c >> 10) & 0x3ff) + 0xd800;
		اگر (flags & FIO_ENDIAN_L)
		{
		    *p++ = cc;
		    *p++ = ((unsigned)cc >> 8);
		}
		وگرنه
		{
		    *p++ = ((unsigned)cc >> 8);
		    *p++ = cc;
		}
		c = (c & 0x3ff) + 0xdc00;
	    }
	    وگرنه
		error = TRUE;
	}
	اگر (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	}
	وگرنه
	{
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    وگرنه    // Latin1
    {
	اگر (c >= 0x100)
	{
	    error = TRUE;
	    *p++ = 0xBF;
	}
	وگرنه
	    *p++ = c;
    }

    *pp = p;
    بازگشت error;
}

/*
 * Call write() to write a number of bytes to the file.
 * Handles encryption and 'encoding' conversion.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ایستا صحیح
buf_write_bytes(ساختار bw_info *ip)
{
    صحیح		wlen;
    char_u	*buf = ip->bw_buf;	// data to write
    صحیح		len = ip->bw_len;	// length of data
    صحیح		flags = ip->bw_flags;	// extra flags

    // Skip conversion when writing the crypt magic number or the BOM.
    اگر (!(flags & FIO_NOCONVERT))
    {
	char_u		*p;
	unsigned	c;
	صحیح		n;

	اگر (flags & FIO_UTF8)
	{
	    // Convert latin1 in the buffer to UTF-8 in the file.
	    p = ip->bw_conv_buf;	// translate to buffer
	    برای (wlen = 0; wlen < len; ++wlen)
		p += utf_char2bytes(buf[wlen], p);
	    buf = ip->bw_conv_buf;
	    len = (صحیح)(p - ip->bw_conv_buf);
	}
	وگرنه اگر (flags & (FIO_UCS4 | FIO_UTF16 | FIO_UCS2 | FIO_LATIN1))
	{
	    // Convert UTF-8 bytes in the buffer to UCS-2, UCS-4, UTF-16 or
	    // Latin1 chars in the file.
	    اگر (flags & FIO_LATIN1)
		p = buf;	// translate in-place (can only get shorter)
	    وگرنه
		p = ip->bw_conv_buf;	// translate to buffer
	    برای (wlen = 0; wlen < len; wlen += n)
	    {
		اگر (wlen == 0 && ip->bw_restlen != 0)
		{
		    صحیح		l;

		    // Use remainder of previous call.  Append the start of
		    // buf[] to get a full sequence.  Might still be too
		    // short!
		    l = CONV_RESTLEN - ip->bw_restlen;
		    اگر (l > len)
			l = len;
		    mch_memmove(ip->bw_rest + ip->bw_restlen, buf, (size_t)l);
		    n = utf_ptr2len_len(ip->bw_rest, ip->bw_restlen + l);
		    اگر (n > ip->bw_restlen + len)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			اگر (ip->bw_restlen + len > CONV_RESTLEN)
			    بازگشت FAIL;
			ip->bw_restlen += len;
			بشکن;
		    }
		    اگر (n > 1)
			c = utf_ptr2char(ip->bw_rest);
		    وگرنه
			c = ip->bw_rest[0];
		    اگر (n >= ip->bw_restlen)
		    {
			n -= ip->bw_restlen;
			ip->bw_restlen = 0;
		    }
		    وگرنه
		    {
			ip->bw_restlen -= n;
			mch_memmove(ip->bw_rest, ip->bw_rest + n,
						      (size_t)ip->bw_restlen);
			n = 0;
		    }
		}
		وگرنه
		{
		    n = utf_ptr2len_len(buf + wlen, len - wlen);
		    اگر (n > len - wlen)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			اگر (len - wlen > CONV_RESTLEN)
			    بازگشت FAIL;
			ip->bw_restlen = len - wlen;
			mch_memmove(ip->bw_rest, buf + wlen,
						      (size_t)ip->bw_restlen);
			بشکن;
		    }
		    اگر (n > 1)
			c = utf_ptr2char(buf + wlen);
		    وگرنه
			c = buf[wlen];
		}
		// Check that there is enough space
		اگر (!(flags & FIO_LATIN1))
		{
		    size_t need = (flags & FIO_UCS4) ? 4 : 2;
		    اگر ((flags & FIO_UTF16) && c >= 0x10000)
			need = 4;

		    اگر ((size_t)(p - ip->bw_conv_buf) + need > ip->bw_conv_buflen)
			بازگشت FAIL;
		}

		اگر (ucs2bytes(c, &p, flags) && !ip->bw_conv_error)
		{
		    ip->bw_conv_error = TRUE;
		    ip->bw_conv_error_lnum = ip->bw_start_lnum;
		}
		اگر (c == NL)
		    ++ip->bw_start_lnum;
	    }
	    اگر (flags & FIO_LATIN1)
		len = (صحیح)(p - buf);
	    وگرنه
	    {
		buf = ip->bw_conv_buf;
		len = (صحیح)(p - ip->bw_conv_buf);
	    }
	}

#ifdef MSWIN
	وگرنه اگر (flags & FIO_CODEPAGE)
	{
	    // Convert UTF-8 or codepage to UCS-2 and then to MS-Windows
	    // codepage.
	    char_u	*from;
	    size_t	fromlen;
	    char_u	*to;
	    صحیح		u8c;
	    BOOL	bad = FALSE;
	    صحیح		needed;

	    اگر (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    وگرنه
	    {
		from = buf;
		fromlen = len;
	    }

	    to = ip->bw_conv_buf;
	    اگر (enc_utf8)
	    {
		// Convert from UTF-8 to UCS-2, to the start of the buffer.
		// The buffer has been allocated to be big enough.
		وقتی (fromlen > 0)
		{
		    n = (صحیح)utf_ptr2len_len(from, (صحیح)fromlen);
		    اگر (n > (صحیح)fromlen)	// incomplete byte sequence
			بشکن;
		    u8c = utf_ptr2char(from);
		    *to++ = (u8c & 0xff);
		    *to++ = (u8c >> 8);
		    fromlen -= n;
		    from += n;
		}

		// Copy remainder to ip->bw_rest[] to be used for the next
		// call.
		اگر (fromlen > CONV_RESTLEN)
		{
		    // weird overlong sequence
		    ip->bw_conv_error = TRUE;
		    بازگشت FAIL;
		}
		mch_memmove(ip->bw_rest, from, fromlen);
		ip->bw_restlen = (صحیح)fromlen;
	    }
	    وگرنه
	    {
		// Convert from enc_codepage to UCS-2, to the start of the
		// buffer.  The buffer has been allocated to be big enough.
		ip->bw_restlen = 0;
		needed = MultiByteToWideChar(enc_codepage,
			     MB_ERR_INVALID_CHARS, (LPCSTR)from, (صحیح)fromlen,
								     پوچ, 0);
		اگر (needed == 0)
		{
		    // When conversion fails there may be a trailing byte.
		    needed = MultiByteToWideChar(enc_codepage,
			 MB_ERR_INVALID_CHARS, (LPCSTR)from, (صحیح)fromlen - 1,
								     پوچ, 0);
		    اگر (needed == 0)
		    {
			// Conversion doesn't work.
			ip->bw_conv_error = TRUE;
			بازگشت FAIL;
		    }
		    // Save the trailing byte for the next call.
		    ip->bw_rest[0] = from[fromlen - 1];
		    ip->bw_restlen = 1;
		}
		needed = MultiByteToWideChar(enc_codepage, MB_ERR_INVALID_CHARS,
				(LPCSTR)from, (صحیح)(fromlen - ip->bw_restlen),
							  (LPWSTR)to, needed);
		اگر (needed == 0)
		{
		    // Safety check: Conversion doesn't work.
		    ip->bw_conv_error = TRUE;
		    بازگشت FAIL;
		}
		to += needed * 2;
	    }

	    fromlen = to - ip->bw_conv_buf;
	    buf = to;
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	    اگر (FIO_GET_CP(flags) == CP_UTF8)
	    {
		// Convert from UCS-2 to UTF-8, using the remainder of the
		// conversion buffer.  Fails when out of space.
		برای (from = ip->bw_conv_buf; fromlen > 1; fromlen -= 2)
		{
		    u8c = *from++;
		    u8c += (*from++ << 8);
		    to += utf_char2bytes(u8c, to);
		    اگر (to + 6 >= ip->bw_conv_buf + ip->bw_conv_buflen)
		    {
			ip->bw_conv_error = TRUE;
			بازگشت FAIL;
		    }
		}
		len = (صحیح)(to - buf);
	    }
	    وگرنه
# endif
	    {
		// Convert from UCS-2 to the codepage, using the remainder of
		// the conversion buffer.  If the conversion uses the default
		// character "0", the data doesn't fit in this encoding, so
		// fail.
		len = WideCharToMultiByte(FIO_GET_CP(flags), 0,
			(LPCWSTR)ip->bw_conv_buf, (صحیح)fromlen / اندازه(WCHAR),
			(LPSTR)to, (صحیح)(ip->bw_conv_buflen - fromlen), 0,
									&bad);
		اگر (bad)
		{
		    ip->bw_conv_error = TRUE;
		    بازگشت FAIL;
		}
	    }
	}
#endif

#ifdef MACOS_CONVERT
	وگرنه اگر (flags & FIO_MACROMAN)
	{
	    // Convert UTF-8 or latin1 to Apple MacRoman.
	    char_u	*from;
	    size_t	fromlen;

	    اگر (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    وگرنه
	    {
		from = buf;
		fromlen = len;
	    }

	    اگر (enc2macroman(from, fromlen,
			ip->bw_conv_buf, &len, ip->bw_conv_buflen,
			ip->bw_rest, &ip->bw_restlen) == FAIL)
	    {
		ip->bw_conv_error = TRUE;
		بازگشت FAIL;
	    }
	    buf = ip->bw_conv_buf;
	}
#endif

#ifdef USE_ICONV
	اگر (ip->bw_iconv_fd != (iconv_t)-1)
	{
	    ثابت نویسه	*from;
	    size_t	fromlen;
	    نویسه	*to;
	    size_t	tolen;

	    // Convert with iconv().
	    اگر (ip->bw_restlen > 0)
	    {
		نویسه *fp;

		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		fp = (نویسه *)ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(fp, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(fp + ip->bw_restlen, buf, (size_t)len);
		from = fp;
		tolen = ip->bw_conv_buflen - fromlen;
	    }
	    وگرنه
	    {
		from = (ثابت نویسه *)buf;
		fromlen = len;
		tolen = ip->bw_conv_buflen;
	    }
	    to = (نویسه *)ip->bw_conv_buf;

	    اگر (ip->bw_first)
	    {
		size_t	save_len = tolen;

		// output the initial shift state sequence
		(تهی)iconv(ip->bw_iconv_fd, پوچ, پوچ, &to, &tolen);

		// There is a bug in iconv() on Linux (which appears to be
		// wide-spread) which sets "to" to NULL and messes up "tolen".
		اگر (to == پوچ)
		{
		    to = (نویسه *)ip->bw_conv_buf;
		    tolen = save_len;
		}
		ip->bw_first = FALSE;
	    }

	    // If iconv() has an error or there is not enough room, fail.
	    اگر ((iconv(ip->bw_iconv_fd, (تهی *)&from, &fromlen, &to, &tolen)
			== (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						    || fromlen > CONV_RESTLEN)
	    {
		ip->bw_conv_error = TRUE;
		بازگشت FAIL;
	    }

	    // copy remainder to ip->bw_rest[] to be used for the next call.
	    اگر (fromlen > 0)
		mch_memmove(ip->bw_rest, (تهی *)from, fromlen);
	    ip->bw_restlen = (صحیح)fromlen;

	    buf = ip->bw_conv_buf;
	    len = (صحیح)((char_u *)to - ip->bw_conv_buf);
	}
#endif
    }

    اگر (ip->bw_fd < 0)
	// Only checking conversion, which is OK if we get here.
	بازگشت OK;

#ifdef FEAT_CRYPT
    اگر (flags & FIO_ENCRYPTED)
    {
	// Encrypt the data. Do it in-place if possible, otherwise use an
	// allocated buffer.
# ifdef CRYPT_NOT_INPLACE
	اگر (crypt_works_inplace(ip->bw_buffer->b_cryptstate))
	{
# endif
	    crypt_encode_inplace(ip->bw_buffer->b_cryptstate, buf, len,
								ip->bw_finish);
# ifdef CRYPT_NOT_INPLACE
	}
	وگرنه
	{
	    char_u *outbuf;

	    len = crypt_encode_alloc(curbuf->b_cryptstate, buf, len, &outbuf,
								ip->bw_finish);
	    اگر (len == 0)
		بازگشت OK;  // Crypt layer is buffering, will flush later.
	    wlen = write_eintr(ip->bw_fd, outbuf, len);
	    vim_free(outbuf);
	    بازگشت (wlen < len) ? FAIL : OK;
	}
# endif
    }
#endif

    wlen = write_eintr(ip->bw_fd, buf, len);
    بازگشت (wlen < len) ? FAIL : OK;
}

/*
 * Check modification time of file, before writing to it.
 * The size isn't checked, because using a tool like "gzip" takes care of
 * using the same timestamp but can't set the size.
 */
    ایستا صحیح
check_mtime(buf_T *buf, stat_T *st)
{
    اگر (buf->b_mtime_read != 0
		  && time_differs(st, buf->b_mtime_read, buf->b_mtime_read_ns))
    {
	پیام_scroll = TRUE;	    // don't overwrite messages here
	پیام_silent = 0;		    // must give this prompt
	// don't use emsg() here, don't want to flush the buffers
	پیام_attr(_("WARNING: The file has been changed since reading it!!!"),
						       HL_ATTR(HLF_E));
	اگر (ask_yesno((char_u *)_("Do you really want to write to it"),
								 TRUE) == 'n')
	    بازگشت FAIL;
	پیام_scroll = FALSE;	    // always overwrite the file message now
    }
    بازگشت OK;
}

/*
 * Generate a BOM in "buf[4]" for encoding "name".
 * Return the length of the BOM (zero when no BOM).
 */
    ایستا صحیح
make_bom(char_u *buf, char_u *name)
{
    صحیح		flags;
    char_u	*p;

    flags = get_fio_flags(name);

    // Can't put a BOM in a non-Unicode file.
    اگر (flags == FIO_LATIN1 || flags == 0)
	بازگشت 0;

    اگر (flags == FIO_UTF8)	// UTF-8
    {
	buf[0] = 0xef;
	buf[1] = 0xbb;
	buf[2] = 0xbf;
	بازگشت 3;
    }
    p = buf;
    (تهی)ucs2bytes(0xfeff, &p, flags);
    بازگشت (صحیح)(p - buf);
}

#ifdef UNIX
    ایستا تهی
set_file_time(
    char_u  *fname,
    time_t  atime,	    // access time
    time_t  mtime)	    // modification time
{
# اگر defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
    ساختار utimbuf  buf;

    buf.actime	= atime;
    buf.modtime	= mtime;
    (تهی)utime((نویسه *)fname, &buf);
# وگرنه
#  اگر defined(HAVE_UTIMES)
    ساختار timeval  tvp[2];

    tvp[0].tv_sec   = atime;
    tvp[0].tv_usec  = 0;
    tvp[1].tv_sec   = mtime;
    tvp[1].tv_usec  = 0;
    (تهی)utimes((نویسه *)fname, (ثابت ساختار timeval *)&tvp);
#  endif
# endif
}
#endif // UNIX

    نویسه *
new_file_message(تهی)
{
    بازگشت shortmess(SHM_NEW) ? _("[New]") : _("[New File]");
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
    صحیح
buf_write(
    buf_T	    *buf,
    char_u	    *fname,
    char_u	    *sfname,
    linenr_T	    start,
    linenr_T	    end,
    exarg_T	    *eap,		// for forced 'ff' and 'fenc', can be
					// NULL!
    صحیح		    append,		// append to the file
    صحیح		    forceit,
    صحیح		    reset_changed,
    صحیح		    filtering)
{
    صحیح		    fd;
    char_u	    *backup = پوچ;
    صحیح		    backup_copy = FALSE; // copy the original file?
    صحیح		    dobackup;
    char_u	    *ffname;
    char_u	    *wfname = پوچ;	// name of file to write to
    char_u	    *s;
    char_u	    *ptr;
    char_u	    c;
    صحیح		    len;
    linenr_T	    lnum;
    long	    nchars;
    char_u	    *errmsg = پوچ;
    صحیح		    errmsg_allocated = FALSE;
    char_u	    *errnum = پوچ;
    char_u	    *buffer;
    char_u	    smallbuf[SMALLBUFSIZE];
    char_u	    *backup_ext;
    صحیح		    bufsize;
    long	    perm;		    // file permissions
    صحیح		    retval = OK;
    صحیح		    newfile = FALSE;	    // TRUE if file doesn't exist yet
    صحیح		    پیام_save = پیام_scroll;
    صحیح		    overwriting;	    // TRUE if writing over original
    صحیح		    no_eol = FALSE;	    // no end-of-line written
    صحیح		    device = FALSE;	    // writing to a device
    stat_T	    st_old;
    صحیح		    prev_got_int = got_int;
    صحیح		    checking_conversion;
    صحیح		    file_readonly = FALSE;  // overwritten file is read-only
#اگر defined(UNIX)			    // XXX fix me sometime?
    صحیح		    made_writable = FALSE;  // 'w' bit has been set
#endif
					// writing everything
    صحیح		    whole = (start == 1 && end == buf->b_ml.ml_سطر_شمار);
    linenr_T	    old_سطر_شمار = buf->b_ml.ml_سطر_شمار;
    صحیح		    attr;
    صحیح		    fileformat;
    صحیح		    write_bin;
    ساختار bw_info  write_info;		// info for buf_write_bytes()
    صحیح		    converted = FALSE;
    صحیح		    notconverted = FALSE;
    char_u	    *fenc;		// effective 'fileencoding'
    char_u	    *fenc_tofree = پوچ; // allocated "fenc"
    صحیح		    wb_flags = 0;
#ifdef HAVE_ACL
    vim_acl_T	    acl = پوچ;		// ACL copied from original file to
					// backup or new file
#endif
#ifdef FEAT_PERSISTENT_UNDO
    صحیح		    write_undo_file = FALSE;
    context_sha256_T sha_ctx;
#endif
    unsigned صحیح    bkc = get_bkc_flags(buf);
    pos_T	    orig_start = buf->b_op_start;
    pos_T	    orig_end = buf->b_op_end;

    اگر (fname == پوچ || *fname == NUL)	// safety check
	بازگشت FAIL;
    اگر (buf->b_ml.ml_mfp == پوچ)
    {
	// This can happen during startup when there is a stray "w" in the
	// vimrc file.
	emsg(_(e_empty_buffer));
	بازگشت FAIL;
    }

    // Disallow writing from .exrc and .vimrc in current directory for
    // security reasons.
    اگر (check_secure())
	بازگشت FAIL;

    // Avoid a crash for a long name.
    اگر (STRLEN(fname) >= MAXPATHL)
    {
	emsg(_(e_name_too_long));
	بازگشت FAIL;
    }

    // must init bw_conv_buf and bw_iconv_fd before jumping to "fail"
    write_info.bw_conv_buf = پوچ;
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
    اگر (buf->b_ffname == پوچ
	    && reset_changed
	    && whole
	    && buf == curbuf
	    && !bt_nofilename(buf)
	    && !filtering
	    && (!append || vim_strchr(p_cpo, CPO_FNAMEAPP) != پوچ)
	    && vim_strchr(p_cpo, CPO_FNAMEW) != پوچ)
    {
	اگر (set_rw_fname(fname, sfname) == FAIL)
	    بازگشت FAIL;
	buf = curbuf;	    // just in case autocmds made "buf" invalid
    }

    اگر (sfname == پوچ)
	sfname = fname;
    // For Unix: Use the short file name whenever possible.
    // Avoids problems with networks and when directory names are changed.
    // Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
    // another directory, which we don't detect
    ffname = fname;			    // remember full fname
#ifdef UNIX
    fname = sfname;
#endif

    اگر (buf->b_ffname != پوچ && fnamecmp(ffname, buf->b_ffname) == 0)
	overwriting = TRUE;
    وگرنه
	overwriting = FALSE;

    اگر (exiting)
	settmode(TMODE_COOK);	    // when exiting allow typeahead now

    ++no_wait_return;		    // don't wait for return yet

    // Set '[ and '] marks to the lines to be written.
    buf->b_op_start.lnum = start;
    buf->b_op_start.col = 0;
    buf->b_op_end.lnum = end;
    buf->b_op_end.col = 0;

    {
	aco_save_T	aco;
	صحیح		buf_ffname = FALSE;
	صحیح		buf_sfname = FALSE;
	صحیح		buf_fname_f = FALSE;
	صحیح		buf_fname_s = FALSE;
	صحیح		did_cmd = FALSE;
	صحیح		nofile_err = FALSE;
	صحیح		empty_memline = (buf->b_ml.ml_mfp == پوچ);
	bufref_T	bufref;

	// Apply PRE autocommands.
	// Set curbuf to the buffer to be written.
	// Careful: The autocommands may call buf_write() recursively!
	اگر (ffname == buf->b_ffname)
	    buf_ffname = TRUE;
	اگر (sfname == buf->b_sfname)
	    buf_sfname = TRUE;
	اگر (fname == buf->b_ffname)
	    buf_fname_f = TRUE;
	اگر (fname == buf->b_sfname)
	    buf_fname_s = TRUE;

	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, buf);
	اگر (curbuf != buf)
	{
	    // Could not find a window for "buf".  Doing more might cause
	    // problems, better bail out.
	    بازگشت FAIL;
	}

	set_bufref(&bufref, buf);

	اگر (append)
	{
	    اگر (!(did_cmd = apply_autocmds_exarg(EVENT_FILEAPPENDCMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		اگر (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		وگرنه
		    apply_autocmds_exarg(EVENT_FILEAPPENDPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	وگرنه اگر (filtering)
	{
	    apply_autocmds_exarg(EVENT_FILTERWRITEPRE,
					    پوچ, sfname, FALSE, curbuf, eap);
	}
	وگرنه اگر (reset_changed && whole)
	{
	    صحیح was_changed = curbufIsChanged();

	    did_cmd = apply_autocmds_exarg(EVENT_BUFWRITECMD,
					  sfname, sfname, FALSE, curbuf, eap);
	    اگر (did_cmd)
	    {
		اگر (was_changed && !curbufIsChanged())
		{
		    // Written everything correctly and BufWriteCmd has reset
		    // 'modified': Correct the undo information so that an
		    // undo now sets 'modified'.
		    u_unchanged(curbuf);
		    u_update_save_nr(curbuf);
		}
	    }
	    وگرنه
	    {
		اگر (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		وگرنه
		    apply_autocmds_exarg(EVENT_BUFWRITEPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	وگرنه
	{
	    اگر (!(did_cmd = apply_autocmds_exarg(EVENT_FILEWRITECMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		اگر (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		وگرنه
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
	اگر (!bufref_valid(&bufref))
	    buf = پوچ;
	اگر (buf == پوچ || (buf->b_ml.ml_mfp == پوچ && !empty_memline)
				       || did_cmd || nofile_err
#ifdef FEAT_EVAL
				       || aborting()
#endif
				       )
	{
	    اگر (buf != پوچ && (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	    {
		// restore the original '[ and '] positions
		buf->b_op_start = orig_start;
		buf->b_op_end = orig_end;
	    }

	    --no_wait_return;
	    پیام_scroll = پیام_save;
	    اگر (nofile_err)
		semsg(_(e_no_matching_autocommands_for_buftype_str_buffer),
							       curbuf->b_p_bt);

	    اگر (nofile_err
#ifdef FEAT_EVAL
		    || aborting()
#endif
		    )
		// An aborting error, interrupt or exception in the
		// autocommands.
		بازگشت FAIL;
	    اگر (did_cmd)
	    {
		اگر (buf == پوچ)
		    // The buffer was deleted.  We assume it was written
		    // (can't retry anyway).
		    بازگشت OK;
		اگر (overwriting)
		{
		    // Assume the buffer was written, update the timestamp.
		    ml_timestamp(buf);
		    اگر (append)
			buf->b_flags &= ~BF_NEW;
		    وگرنه
			buf->b_flags &= ~BF_WRITE_MASK;
		}
		اگر (reset_changed && buf->b_changed && !append
			&& (overwriting || vim_strchr(p_cpo, CPO_PLUS) != پوچ))
		    // Buffer still changed, the autocommands didn't work
		    // properly.
		    بازگشت FAIL;
		بازگشت OK;
	    }
#ifdef FEAT_EVAL
	    اگر (!aborting())
#endif
		emsg(_(e_autocommands_deleted_or_unloaded_buffer_to_be_written));
	    بازگشت FAIL;
	}

	// The autocommands may have changed the number of lines in the file.
	// When writing the whole file, adjust the end.
	// When writing part of the file, assume that the autocommands only
	// changed the number of lines that are to be written (tricky!).
	اگر (buf->b_ml.ml_سطر_شمار != old_سطر_شمار)
	{
	    اگر (whole)						// write all
		end = buf->b_ml.ml_سطر_شمار;
	    وگرنه اگر (buf->b_ml.ml_سطر_شمار > old_سطر_شمار)	// more lines
		end += buf->b_ml.ml_سطر_شمار - old_سطر_شمار;
	    وگرنه						// less lines
	    {
		end -= old_سطر_شمار - buf->b_ml.ml_سطر_شمار;
		اگر (end < start)
		{
		    --no_wait_return;
		    پیام_scroll = پیام_save;
		    emsg(_(e_autocommands_changed_عدد_of_سطرها_in_unexpected_way));
		    بازگشت FAIL;
		}
	    }
	}

	// The autocommands may have changed the name of the buffer, which may
	// be kept in fname, ffname and sfname.  A ":cd" may also have dropped
	// the short name, then b_fname is the full name.
	اگر (buf_ffname)
	    ffname = buf->b_ffname;
	اگر (buf_sfname)
	    sfname = buf->b_fname;
	اگر (buf_fname_f)
	    fname = buf->b_ffname;
	اگر (buf_fname_s)
	    fname = buf->b_fname;
    }

    اگر (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	// restore the original '[ and '] positions
	buf->b_op_start = orig_start;
	buf->b_op_end = orig_end;
    }

#ifdef FEAT_NETBEANS_INTG
    اگر (netbeans_active() && isNetbeansBuffer(buf))
    {
	اگر (whole)
	{
	    // b_changed can be 0 after an undo, but we still need to write
	    // the buffer to NetBeans.
	    اگر (buf->b_changed || isNetbeansModified(buf))
	    {
		--no_wait_return;		// may wait for return now
		پیام_scroll = پیام_save;
		netbeans_save_buffer(buf);	// no error checking...
		بازگشت retval;
	    }
	    وگرنه
	    {
		errnum = (char_u *)"E656: ";
		errmsg = (char_u *)_(e_netbeans_disallows_writes_of_unmodified_buffers);
		buffer = پوچ;
		برو fail;
	    }
	}
	وگرنه
	{
	    errnum = (char_u *)"E657: ";
	    errmsg = (char_u *)_(e_partial_writes_disallowed_for_netbeans_buffers);
	    buffer = پوچ;
	    برو fail;
	}
    }
#endif

    اگر (shortmess(SHM_OVER) && !exiting)
	پیام_scroll = FALSE;	    // overwrite previous file message
    وگرنه
	پیام_scroll = TRUE;	    // don't overwrite previous file message
    اگر (!filtering)
	filemess(buf,
#ifndef UNIX
		sfname,
#وگرنه
		fname,
#endif
		    (char_u *)"", 0);	// show that we are busy
    پیام_scroll = FALSE;		    // always overwrite the file message now

    buffer = alloc(WRITEBUFSIZE);
    اگر (buffer == پوچ)		    // can't allocate big buffer, use small
				    // one (to be able to write when out of
				    // memory)
    {
	buffer = smallbuf;
	bufsize = SMALLBUFSIZE;
    }
    وگرنه
	bufsize = WRITEBUFSIZE;

    // Get information about original file (if there is one).
#اگر defined(UNIX)
    st_old.st_dev = 0;
    st_old.st_ino = 0;
    perm = -1;
    اگر (mch_stat((نویسه *)fname, &st_old) < 0)
	newfile = TRUE;
    وگرنه
    {
	perm = st_old.st_mode;
	اگر (!S_ISREG(st_old.st_mode))		// not a file
	{
	    اگر (S_ISDIR(st_old.st_mode))
	    {
		errnum = (char_u *)"E502: ";
		errmsg = (char_u *)_(e_is_a_directory);
		برو fail;
	    }
	    اگر (mch_nodetype(fname) != گره_WRITABLE)
	    {
		errnum = (char_u *)"E503: ";
		errmsg = (char_u *)_(e_is_not_file_or_writable_device);
		برو fail;
	    }
	    // It's a device of some kind (or a fifo) which we can write to
	    // but for which we can't make a backup.
	    device = TRUE;
	    newfile = TRUE;
	    perm = -1;
	}
    }
#وگرنه // !UNIX
    // Check for a writable device name.
    c = mch_nodetype(fname);
    اگر (c == گره_OTHER)
    {
	errnum = (char_u *)"E503: ";
	errmsg = (char_u *)_(e_is_not_file_or_writable_device);
	برو fail;
    }
    اگر (c == گره_WRITABLE)
    {
# اگر defined(MSWIN)
	// MS-Windows allows opening a device, but we will probably get stuck
	// trying to write to it.
	اگر (!p_odev)
	{
	    errnum = (char_u *)"E796: ";
	    errmsg = (char_u *)_(e_writing_to_device_disabled_with_opendevice_option);
	    برو fail;
	}
# endif
	device = TRUE;
	newfile = TRUE;
	perm = -1;
    }
    وگرنه
    {
	perm = mch_getperm(fname);
	اگر (perm < 0)
	    newfile = TRUE;
	وگرنه اگر (mch_isdir(fname))
	{
	    errnum = (char_u *)"E502: ";
	    errmsg = (char_u *)_(e_is_a_directory);
	    برو fail;
	}
	اگر (overwriting)
	    (تهی)mch_stat((نویسه *)fname, &st_old);
    }
#endif // !UNIX

    اگر (!device && !newfile)
    {
	// Check if the file is really writable (when renaming the file to
	// make a backup we won't discover it later).
	file_readonly = check_file_readonly(fname, (صحیح)perm);

	اگر (!forceit && file_readonly)
	{
	    اگر (vim_strchr(p_cpo, CPO_FWRITE) != پوچ)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_in_cpoptions);
	    }
	    وگرنه
	    {
		errnum = (char_u *)"E505: ";
		errmsg = (char_u *)_(e_is_read_only_add_bang_to_override);
	    }
	    برو fail;
	}

	// Check if the timestamp hasn't changed since reading the file.
	اگر (overwriting)
	{
	    retval = check_mtime(buf, &st_old);
	    اگر (retval == FAIL)
		برو fail;
	}
    }

#ifdef HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    اگر (!newfile)
	acl = mch_get_acl(fname);
#endif

    // If 'backupskip' is not empty, don't make a backup for some files.
    dobackup = (p_wb || p_bk || *p_pm != NUL);
    اگر (dobackup && *p_bsk != NUL && match_file_list(p_bsk, sfname, ffname))
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
    اگر (!(append && *p_pm == NUL) && !filtering && perm >= 0 && dobackup)
    {
#اگر defined(UNIX) || defined(MSWIN)
	stat_T	    st;
#endif

	اگر ((bkc & BKC_YES) || append)	// "yes"
	    backup_copy = TRUE;
#اگر defined(UNIX) || defined(MSWIN)
	وگرنه اگر ((bkc & BKC_AUTO))	// "auto"
	{
# ifdef UNIX
	    // Don't rename the file when:
	    // - it's a hard link
	    // - it's a symbolic link
	    // - we don't have write permission in the directory
	    // - we can't set the owner/group of the new file
	    اگر (st_old.st_nlink > 1
		    || mch_lstat((نویسه *)fname, &st) < 0
		    || st.st_dev != st_old.st_dev
		    || st.st_ino != st_old.st_ino
#  ifndef HAVE_FCHOWN
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid
#  endif
		    )
		backup_copy = TRUE;
	    وگرنه
# وگرنه
#  ifdef MSWIN
	    // On NTFS file systems hard links are possible.
	    اگر (mch_is_linked(fname))
		backup_copy = TRUE;
	    وگرنه
#  endif
# endif
	    {
		size_t	dirlen;
		char_u	tmp_fname[MAXPATHL];
		صحیح	i;

		// Check if we can create a file and set the owner/group/mode to
		// the ones from the original file.
		// First find a file name that doesn't exist yet.  Use the
		// conventional hidden swap file name so that this short-lived
		// probe is covered by standard Vim ignore patterns.
		dirlen = (size_t)(gettail(fname) - fname);
		vim_strncpy(tmp_fname, fname, dirlen);
		fd = -1;
		برای (i = 4913; ; i += 123)
		{
		    vim_snprintf((نویسه *)tmp_fname + dirlen,
			اندازه(tmp_fname) - dirlen, ".%d.swp", i);
		    اگر (mch_lstat((نویسه *)tmp_fname, &st) < 0)
		    {
			fd = mch_open((نویسه *)tmp_fname,
				    O_CREAT|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
			اگر (fd < 0 && errno == EEXIST)
			    // If the same file name is created by another
			    // process between lstat() and open(), find another
			    // name.
			    ادامه;
			بشکن;
		    }
		}
		اگر (fd < 0)	// can't write in directory
		    backup_copy = TRUE;
		وگرنه
		{
# ifdef UNIX
#  ifdef HAVE_FCHOWN
		    vim_ignored = fchown(fd, st_old.st_uid, st_old.st_gid);
#  endif
#  ifdef HAVE_FCHMOD
		    (تهی)mch_fsetperm(fd, perm);
#  وگرنه
		    (تهی)mch_setperm(tmp_fname, perm);
#  endif
		    اگر (mch_stat((نویسه *)tmp_fname, &st) < 0
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
			صحیح try;

			برای (try = 0; try < 10; ++try)
			{
			    اگر (mch_lstat((نویسه *)tmp_fname, &st) < 0)
				بشکن;
			    ui_delay(50L, TRUE);  // wait 50 msec
			    mch_remove(tmp_fname);
			}
		    }
# endif
		}
	    }
	}

	// Break symlinks and/or hardlinks if we've been asked to.
	اگر ((bkc & BKC_BREAKSYMLINK) || (bkc & BKC_BREAKHARDLINK))
	{
# ifdef UNIX
	    صحیح	lstat_res;

	    lstat_res = mch_lstat((نویسه *)fname, &st);

	    // Symlinks.
	    اگر ((bkc & BKC_BREAKSYMLINK)
		    && lstat_res == 0
		    && st.st_ino != st_old.st_ino)
		backup_copy = FALSE;

	    // Hardlinks.
	    اگر ((bkc & BKC_BREAKHARDLINK)
		    && st_old.st_nlink > 1
		    && (lstat_res != 0 || st.st_ino == st_old.st_ino))
		backup_copy = FALSE;
# وگرنه
#  اگر defined(MSWIN)
	    // Symlinks.
	    اگر ((bkc & BKC_BREAKSYMLINK) && mch_is_symbolic_link(fname))
		backup_copy = FALSE;

	    // Hardlinks.
	    اگر ((bkc & BKC_BREAKHARDLINK) && mch_is_hard_link(fname))
		backup_copy = FALSE;
#  endif
# endif
	}

#endif

	// make sure we have a valid backup extension to use
	اگر (*p_bex == NUL)
	    backup_ext = (char_u *)".bak";
	وگرنه
	    backup_ext = p_bex;

	اگر (backup_copy
		&& (fd = mch_open((نویسه *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
	{
	    صحیح		bfd;
	    char_u	*copybuf;
	    صحیح		some_error = FALSE;
	    stat_T	st_new;
	    char_u	*dirp;
	    char_u	*rootname;
#اگر defined(UNIX)
	    صحیح		did_set_shortname;
	    mode_t	umask_save;
#endif

	    copybuf = alloc(WRITEBUFSIZE + 1);
	    اگر (copybuf == پوچ)
	    {
		some_error = TRUE;	    // out of memory
		برو nobackup;
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
	    وقتی (*dirp)
	    {
		char_u	*p UNUSED;
		صحیح	copybuf_len UNUSED;

#ifdef UNIX
		st_new.st_ino = 0;
		st_new.st_dev = 0;
		st_new.st_gid = 0;
#endif

		// Isolate one directory name, using an entry in 'bdir'.
		copybuf_len = copy_option_part(&dirp, copybuf, WRITEBUFSIZE, ",");

#اگر defined(UNIX) || defined(MSWIN)
		p = copybuf + copybuf_len;
		اگر (after_pathsep(copybuf, p) && p[-1] == p[-2])
		    // Ends with '//', use full path
		    اگر ((p = make_percent_swname(copybuf, p, fname)) != پوچ)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#endif
		rootname = get_file_in_dir(fname, copybuf);
		اگر (rootname == پوچ)
		{
		    some_error = TRUE;	    // out of memory
		    برو nobackup;
		}

#اگر defined(UNIX)
		did_set_shortname = FALSE;
#endif

		// May try twice if 'shortname' not set.
		برای (;;)
		{
		    // Make the backup file name.
		    اگر (backup == پوچ)
			backup = buf_modname((buf->b_p_sn || buf->b_shortname),
						 rootname, backup_ext, FALSE);
		    اگر (backup == پوچ)
		    {
			vim_free(rootname);
			some_error = TRUE;		// out of memory
			برو nobackup;
		    }

		    // Check if backup file already exists.
		    اگر (mch_stat((نویسه *)backup, &st_new) >= 0)
		    {
#ifdef UNIX
			// Check if backup file is same as original file.
			// May happen when modname() gave the same file back.
			// E.g. silly link, or file name-length reached.
			// If we don't check here, we either ruin the file
			// when copying or erase it after writing. jw.
			اگر (st_new.st_dev == st_old.st_dev
					    && st_new.st_ino == st_old.st_ino)
			{
			    VIM_CLEAR(backup);	// no backup file to delete
			    // may try again with 'shortname' set
			    اگر (!(buf->b_shortname || buf->b_p_sn))
			    {
				buf->b_shortname = true;
				did_set_shortname = TRUE;
				ادامه;
			    }
				// setting shortname didn't help
			    اگر (did_set_shortname)
				buf->b_shortname = false;
			    بشکن;
			}
#endif

			// If we are not going to keep the backup file, don't
			// delete an existing one, try to use another name.
			// Change one character, just before the extension.
			اگر (!p_bk)
			{
			    char_u	*wp;

			    wp = backup + STRLEN(backup) - 1
							 - STRLEN(backup_ext);
			    اگر (wp < backup)	// empty file name ???
				wp = backup;
			    *wp = 'z';
			    وقتی (*wp > 'a'
				    && mch_stat((نویسه *)backup, &st_new) >= 0)
				--*wp;
			    // They all exist??? Must be something wrong.
			    اگر (*wp == 'a')
				VIM_CLEAR(backup);
			}
		    }
		    بشکن;
		}
		vim_free(rootname);

		// Try to create the backup file
		اگر (backup != پوچ)
		{
		    // remove old backup, if present
		    mch_remove(backup);
		    // Open with O_EXCL to avoid the file being created while
		    // we were sleeping (symlink hacker attack?). Reset umask
		    // if possible to avoid mch_setperm() below.
#ifdef UNIX
		    umask_save = umask(0);
#endif
		    bfd = mch_open((نویسه *)backup,
				O_WRONLY|O_CREAT|O_EXTRA|O_EXCL|O_NOFOLLOW,
								 perm & 0777);
#ifdef UNIX
		    (تهی)umask(umask_save);
#endif
		    اگر (bfd < 0)
			VIM_CLEAR(backup);
		    وگرنه
		    {
			// Set file protection same as original file, but
			// strip s-bit.  Only needed if umask() wasn't used
			// above.
#ifndef UNIX
			(تهی)mch_setperm(backup, perm & 0777);
#وگرنه
			// Try to set the group of the backup same as the
			// original file. If this fails, set the protection
			// bits for the group same as the protection bits for
			// others.
			اگر (st_new.st_gid != st_old.st_gid
# ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
				&& fchown(bfd, (uid_t)-1, st_old.st_gid) != 0
# endif
						)
			    mch_setperm(backup,
					  (perm & 0707) | ((perm & 07) << 3));
# اگر defined(HAVE_SELINUX) || defined(HAVE_SMACK)
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
			وقتی ((write_info.bw_len = read_eintr(fd, copybuf,
							    WRITEBUFSIZE)) > 0)
			{
			    اگر (buf_write_bytes(&write_info) == FAIL)
			    {
				errmsg = (char_u *)_(e_cant_write_to_backup_file_add_bang_to_override);
				بشکن;
			    }
			    ui_breakcheck();
			    اگر (got_int)
			    {
				errmsg = (char_u *)_(e_interrupted);
				بشکن;
			    }
			}

			اگر (close(bfd) < 0 && errmsg == پوچ)
			    errmsg = (char_u *)_(e_close_error_for_backup_file_add_bang_to_write_anyway);
			اگر (write_info.bw_len < 0)
			    errmsg = (char_u *)_(e_cant_read_file_for_backup_add_bang_to_write_anyway);
#ifdef UNIX
			set_file_time(backup, st_old.st_atime, st_old.st_mtime);
#endif
#ifdef HAVE_ACL
			mch_set_acl(backup, acl);
#endif
#اگر defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
#endif
#ifdef FEAT_XATTR
			mch_copy_xattr(fname, backup);
#endif
#ifdef MSWIN
			(تهی)mch_copy_file_attribute(fname, backup);
#endif
			بشکن;
		    }
		}
	    }
    nobackup:
	    close(fd);		// ignore errors for closing read file
	    vim_free(copybuf);

	    اگر (backup == پوچ && errmsg == پوچ)
		errmsg = (char_u *)_(e_cannot_create_backup_file_add_bang_to_write_anyway);
	    // ignore errors when forceit is TRUE
	    اگر ((some_error || errmsg != پوچ) && !forceit)
	    {
		retval = FAIL;
		برو fail;
	    }
	    errmsg = پوچ;
	}
	وگرنه
	{
	    char_u	*dirp;
	    char_u	*p;
	    char_u	*rootname;

	    // Make a backup by renaming the original file.

	    // If 'cpoptions' includes the "W" flag, we don't want to
	    // overwrite a read-only file.  But rename may be possible
	    // anyway, thus we need an extra check here.
	    اگر (file_readonly && vim_strchr(p_cpo, CPO_FWRITE) != پوچ)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_in_cpoptions);
		برو fail;
	    }

	    // Form the backup file name - change path/fo.o.h to
	    // path/fo.o.h.bak Try all directories in 'backupdir', first one
	    // that works is used.
	    dirp = p_bdir;
	    وقتی (*dirp)
	    {
		صحیح IObufflen UNUSED;

		// Isolate one directory name and make the backup file name.
		IObufflen = copy_option_part(&dirp, IObuff, IOSIZE, ",");

#اگر defined(UNIX) || defined(MSWIN)
		p = IObuff + IObufflen;
		اگر (after_pathsep(IObuff, p) && p[-1] == p[-2])
		    // path ends with '//', use full path
		    اگر ((p = make_percent_swname(IObuff, p, fname)) != پوچ)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#endif
		اگر (backup == پوچ)
		{
		    rootname = get_file_in_dir(fname, IObuff);
		    اگر (rootname == پوچ)
			backup = پوچ;
		    وگرنه
		    {
			backup = buf_modname(
				(buf->b_p_sn || buf->b_shortname),
						rootname, backup_ext, FALSE);
			vim_free(rootname);
		    }
		}

		اگر (backup != پوچ)
		{
		    // If we are not going to keep the backup file, don't
		    // delete an existing one, try to use another name.
		    // Change one character, just before the extension.
		    اگر (!p_bk && mch_getperm(backup) >= 0)
		    {
			p = backup + STRLEN(backup) - 1 - STRLEN(backup_ext);
			اگر (p < backup)	// empty file name ???
			    p = backup;
			*p = 'z';
			وقتی (*p > 'a' && mch_getperm(backup) >= 0)
			    --*p;
			// They all exist??? Must be something wrong!
			اگر (*p == 'a')
			    VIM_CLEAR(backup);
		    }
		}
		اگر (backup != پوچ)
		{
		    // Delete any existing backup and move the current version
		    // to the backup.	For safety, we don't remove the backup
		    // until the write has finished successfully. And if the
		    // 'backup' option is set, leave it around.

		    // If the renaming of the original file to the backup file
		    // works, quit here.
		    اگر (vim_rename(fname, backup) == 0)
			بشکن;

		    VIM_CLEAR(backup);   // don't do the rename below
		}
	    }
	    اگر (backup == پوچ && !forceit)
	    {
		errmsg = (char_u *)_(e_cant_make_backup_file_add_bang_to_write_anyway);
		برو fail;
	    }
	}
    }

#اگر defined(UNIX)
    // When using ":w!" and the file was read-only: make it writable
    اگر (forceit && perm >= 0 && !(perm & 0200) && st_old.st_uid == getuid()
				     && vim_strchr(p_cpo, CPO_FWRITE) == پوچ)
    {
	perm |= 0200;
	(تهی)mch_setperm(fname, perm);
	made_writable = TRUE;
    }
#endif

    // When using ":w!" and writing to the current file, 'readonly' makes no
    // sense, reset it, unless 'Z' appears in 'cpoptions'.
    اگر (forceit && overwriting && vim_strchr(p_cpo, CPO_KEEPRO) == پوچ)
    {
	buf->b_p_ro = FALSE;
	need_maketitle = TRUE;	    // set window title later
	status_redraw_all();	    // redraw status lines later
    }

    اگر (end > buf->b_ml.ml_سطر_شمار)
	end = buf->b_ml.ml_سطر_شمار;
    اگر (buf->b_ml.ml_flags & ML_EMPTY)
	start = end + 1;

    // If the original file is being overwritten, there is a small chance that
    // we crash in the middle of writing. Therefore the file is preserved now.
    // This makes all block numbers positive so that recovery does not need
    // the original file.
    // Don't do this if there is a backup file and we are exiting.
    اگر (reset_changed && !newfile && overwriting
					      && !(exiting && backup != پوچ))
    {
	ml_preserve(buf, FALSE);
	اگر (got_int)
	{
	    errmsg = (char_u *)_(e_interrupted);
	    برو restore_backup;
	}
    }

#ifdef VMS
    vms_remove_version(fname); // remove version
#endif
    // Default: write the file directly.  May write to a temp file for
    // multi-byte conversion.
    wfname = fname;

    // Check for forced 'fileencoding' from "++opt=val" argument.
    اگر (eap != پوچ && eap->force_enc != 0)
    {
	fenc = eap->cmd + eap->force_enc;
	fenc = enc_canonize(fenc);
	fenc_tofree = fenc;
    }
    وگرنه
	fenc = buf->b_p_fenc;

    // Check if the file needs to be converted.
    converted = need_conversion(fenc);

    // Check if UTF-8 to UCS-2/4 or Latin1 conversion needs to be done.  Or
    // Latin1 to Unicode conversion.  This is handled in buf_write_bytes().
    // Prepare the flags for it and allocate bw_conv_buf when needed.
    اگر (converted && (enc_utf8 || STRCMP(p_enc, "latin1") == 0))
    {
	wb_flags = get_fio_flags(fenc);
	اگر (wb_flags & (FIO_UCS2 | FIO_UCS4 | FIO_UTF16 | FIO_UTF8))
	{
	    // overallocate a bit, in case we read incomplete multi-byte chars
	    صحیح size = bufsize + CONV_RESTLEN;
	    // Need to allocate a buffer to translate into.
	    اگر (wb_flags & (FIO_UCS2 | FIO_UTF16 | FIO_UTF8))
		write_info.bw_conv_buflen = size * 2;
	    وگرنه // FIO_UCS4
		write_info.bw_conv_buflen = size * 4;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    اگر (write_info.bw_conv_buf == پوچ)
		end = 0;
	}
    }

#ifdef MSWIN
    اگر (converted && wb_flags == 0 && (wb_flags = get_win_fio_flags(fenc)) != 0)
    {
	// Convert UTF-8 -> UCS-2 and UCS-2 -> DBCS.  Worst-case * 4:
	write_info.bw_conv_buflen = bufsize * 4;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	اگر (write_info.bw_conv_buf == پوچ)
	    end = 0;
    }
#endif

#ifdef MACOS_CONVERT
    اگر (converted && wb_flags == 0 && (wb_flags = get_mac_fio_flags(fenc)) != 0)
    {
	write_info.bw_conv_buflen = bufsize * 3;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	اگر (write_info.bw_conv_buf == پوچ)
	    end = 0;
    }
#endif

#اگر defined(FEAT_EVAL) || defined(USE_ICONV)
    اگر (converted && wb_flags == 0)
    {
# ifdef USE_ICONV
	// Use iconv() conversion when conversion is needed and it's not done
	// internally.
	write_info.bw_iconv_fd = (iconv_t)my_iconv_open(fenc,
					enc_utf8 ? (char_u *)"utf-8" : p_enc);
	اگر (write_info.bw_iconv_fd != (iconv_t)-1)
	{
	    // We're going to use iconv(), allocate a buffer to convert in.
	    write_info.bw_conv_buflen = bufsize * ICONV_MULT;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    اگر (write_info.bw_conv_buf == پوچ)
		end = 0;
	    write_info.bw_first = TRUE;
	}
#  ifdef FEAT_EVAL
	وگرنه
#  endif
# endif

# ifdef FEAT_EVAL
	    // When the file needs to be converted with 'charconvert' after
	    // writing, write to a temp file instead and let the conversion
	    // overwrite the original file.
	    اگر (*p_ccv != NUL)
	    {
		wfname = vim_tempname('w', FALSE);
		اگر (wfname == پوچ)	// Can't write without a tempfile!
		{
		    errmsg = (char_u *)_(e_cant_find_temp_file_for_writing);
		    برو restore_backup;
		}
	    }
# endif
    }
#endif
    اگر (converted && wb_flags == 0
#ifdef USE_ICONV
	    && write_info.bw_iconv_fd == (iconv_t)-1
#endif
#ifdef FEAT_EVAL
	    && wfname == fname
#endif
	    )
    {
	اگر (!forceit)
	{
	    errmsg = (char_u *)_(e_cannot_convert_add_bang_to_write_without_conversion);
	    برو restore_backup;
	}
	notconverted = TRUE;
    }

    // If conversion is taking place, we may first pretend to write and check
    // for conversion errors.  Then loop again to write for real.
    // When not doing conversion this writes for real right away.
    برای (checking_conversion = TRUE; ; checking_conversion = FALSE)
    {
	// There is no need to check conversion when:
	// - there is no conversion
	// - we make a backup file, that can be restored in case of conversion
	//   failure.
	اگر (!converted || dobackup)
	    checking_conversion = FALSE;

	اگر (checking_conversion)
	{
	    // Make sure we don't write anything.
	    fd = -1;
	    write_info.bw_fd = fd;
	}
	وگرنه
	{
#ifdef HAVE_FTRUNCATE
# define TRUNC_ON_OPEN 0
#وگرنه
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
	    وقتی ((fd = mch_open((نویسه *)wfname, O_WRONLY | O_EXTRA | (append
				? (forceit ? (O_APPEND | O_CREAT) : O_APPEND)
				: (O_CREAT | TRUNC_ON_OPEN))
				, perm < 0 ? 0666 : (perm & 0777))) < 0)
	    {
		// A forced write will try to create a new file if the old one
		// is still readonly. This may also happen when the directory
		// is read-only. In that case the mch_remove() will fail.
		اگر (errmsg == پوچ)
		{
#ifdef UNIX
		    stat_T	st;

		    // Don't delete the file when it's a hard or symbolic link.
		    اگر ((!newfile && st_old.st_nlink > 1)
			    || (mch_lstat((نویسه *)fname, &st) == 0
				&& (st.st_dev != st_old.st_dev
				    || st.st_ino != st_old.st_ino)))
			errmsg =
			      (char_u *)_(e_cant_open_linked_file_for_writing);
		    وگرنه
#endif
		    {
			errmsg = (char_u *)_(e_cant_open_file_for_writing);
			اگر (forceit && vim_strchr(p_cpo, CPO_FWRITE) == پوچ
								  && perm >= 0)
			{
#ifdef UNIX
			    // we write to the file, thus it should be marked
			    // writable after all
			    اگر (!(perm & 0200))
				made_writable = TRUE;
			    perm |= 0200;
			    اگر (st_old.st_uid != getuid()
						  || st_old.st_gid != getgid())
				perm &= 0777;
#endif
			    اگر (!append)  // don't remove when appending
				mch_remove(wfname);
			    ادامه;
			}
		    }
		}

restore_backup:
		{
		    stat_T	st;

		    // If we failed to open the file, we don't need a backup.
		    // Throw it away.  If we moved or removed the original file
		    // try to put the backup in its place.
		    اگر (backup != پوچ && wfname == fname)
		    {
			اگر (backup_copy)
			{
			    // There is a small chance that we removed the
			    // original, try to move the copy in its place.
			    // This may not work if the vim_rename() fails.
			    // In that case we leave the copy around.

			    // If file does not exist, put the copy in its
			    // place
			    اگر (mch_stat((نویسه *)fname, &st) < 0)
				vim_rename(backup, fname);
			    // if original file does exist throw away the copy
			    اگر (mch_stat((نویسه *)fname, &st) >= 0)
				mch_remove(backup);
			}
			وگرنه
			{
			    // try to put the original file back
			    vim_rename(backup, fname);
			}
		    }

		    // if original file no longer exists give an extra warning
		    اگر (!newfile && mch_stat((نویسه *)fname, &st) < 0)
			end = 0;
		}

		اگر (wfname != fname)
		    vim_free(wfname);
		برو fail;
	    }
	    write_info.bw_fd = fd;

#اگر defined(UNIX)
	    {
		stat_T	st;

		// Double check we are writing the intended file before making
		// any changes.
		اگر (overwriting
			&& (!dobackup || backup_copy)
			&& fname == wfname
			&& perm >= 0
			&& mch_fstat(fd, &st) == 0
			&& st.st_ino != st_old.st_ino)
		{
		    close(fd);
		    errmsg = (char_u *)_(e_file_changed_while_writing);
		    برو fail;
		}
	    }
#endif
#ifdef HAVE_FTRUNCATE
	    اگر (!append)
		vim_ignored = ftruncate(fd, (off_t)0);
#endif

#اگر defined(MSWIN)
	    اگر (backup != پوچ && overwriting && !append)
		(تهی)mch_copy_file_attribute(backup, wfname);

	    اگر (!overwriting && !append)
	    {
		اگر (buf->b_ffname != پوچ)
		    (تهی)mch_copy_file_attribute(buf->b_ffname, wfname);
		// Should copy resource fork
	    }
#endif

#ifdef FEAT_CRYPT
	    اگر (*buf->b_p_کلید != NUL && !filtering)
	    {
		char_u		*header;
		صحیح		header_len;

		buf->b_cryptstate = crypt_create_for_writing(
						      crypt_get_method_nr(buf),
					   buf->b_p_کلید, &header, &header_len);
		اگر (buf->b_cryptstate == پوچ || header == پوچ)
		    end = 0;
		وگرنه
		{
		    // Write magic number, so that Vim knows how this file is
		    // encrypted when reading it back.
		    write_info.bw_buf = header;
		    write_info.bw_len = header_len;
		    write_info.bw_flags = FIO_NOCONVERT;
		    اگر (buf_write_bytes(&write_info) == FAIL)
			end = 0;
		    wb_flags |= FIO_ENCRYPTED;
		    vim_free(header);
		}
	    }
#endif
	}
	errmsg = پوچ;

	write_info.bw_buf = buffer;
	nchars = 0;

	// use "++bin", "++nobin" or 'binary'
	اگر (eap != پوچ && eap->force_bin != 0)
	    write_bin = (eap->force_bin == FORCE_BIN);
	وگرنه
	    write_bin = buf->b_p_bin;

	// The BOM is written just after the encryption magic number.
	// Skip it when appending and the file already existed, the BOM only
	// makes sense at the start of the file.
	اگر (buf->b_p_bomb && !write_bin && (!append || perm < 0))
	{
	    write_info.bw_len = make_bom(buffer, fenc);
	    اگر (write_info.bw_len > 0)
	    {
		// don't convert, do encryption
		write_info.bw_flags = FIO_NOCONVERT | wb_flags;
		اگر (buf_write_bytes(&write_info) == FAIL)
		    end = 0;
		وگرنه
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
			    && (buf->b_cryptstate == پوچ
				|| crypt_works_inplace(buf->b_cryptstate))
# endif
			    && reset_changed
			    && !checking_conversion);
# ifdef CRYPT_NOT_INPLACE
	// remove undo file if encrypting it is not possible
	اگر (buf->b_p_udf
		&& overwriting
		&& !append
		&& !filtering
		&& !checking_conversion
		&& buf->b_cryptstate != پوچ
		&& !crypt_works_inplace(buf->b_cryptstate))
	    u_undofile_reset_and_delete(buf);
# endif
	اگر (write_undo_file)
	    // Prepare for computing the hash value of the text.
	    sha256_start(&sha_ctx);
#endif

	write_info.bw_len = bufsize;
	write_info.bw_flags = wb_flags;
	fileformat = get_fileformat_force(buf, eap);
	s = buffer;
	len = 0;
	برای (lnum = start; lnum <= end; ++lnum)
	{
	    // The next while loop is done once for each character written.
	    // Keep it fast!
	    ptr = ml_get_buf(buf, lnum, FALSE) - 1;
#ifdef FEAT_PERSISTENT_UNDO
	    اگر (write_undo_file)
		sha256_update(&sha_ctx, ptr + 1,
					      (UINT32_T)(STRLEN(ptr + 1) + 1));
#endif
	    وقتی ((c = *++ptr) != NUL)
	    {
		اگر (c == NL)
		    *s = NUL;		// replace newlines with NULs
		وگرنه اگر (c == CAR && fileformat == EOL_MAC)
		    *s = NL;		// Mac: replace CRs with NLs
		وگرنه
		    *s = c;
		++s;
		اگر (++len != bufsize)
		    ادامه;
#ifdef FEAT_CRYPT
		اگر (write_info.bw_fd > 0 && lnum == end
			&& (write_info.bw_flags & FIO_ENCRYPTED)
			&& *buf->b_p_کلید != NUL && !filtering
			&& *ptr == NUL)
		    write_info.bw_finish = TRUE;
#endif
		اگر (buf_write_bytes(&write_info) == FAIL)
		{
		    end = 0;		// write error: break loop
		    بشکن;
		}
		nchars += bufsize;
		s = buffer;
		len = 0;
		write_info.bw_start_lnum = lnum;
	    }
	    // write failed or last line has no EOL: stop here
	    اگر (end == 0
		    || (lnum == end
			&& (write_bin || !buf->b_p_fixeol)
			&& ((write_bin && lnum == buf->b_no_eol_lnum)
			    || (lnum == buf->b_ml.ml_سطر_شمار
							   && !buf->b_p_eol))))
	    {
		++lnum;			// written the line, count it
		no_eol = TRUE;
		بشکن;
	    }
	    اگر (fileformat == EOL_UNIX)
		*s++ = NL;
	    وگرنه
	    {
		*s++ = CAR;		    // EOL_MAC or EOL_DOS: write CR
		اگر (fileformat == EOL_DOS)  // write CR-NL
		{
		    اگر (++len == bufsize)
		    {
			اگر (buf_write_bytes(&write_info) == FAIL)
			{
			    end = 0;	// write error: break loop
			    بشکن;
			}
			nchars += bufsize;
			s = buffer;
			len = 0;
		    }
		    *s++ = NL;
		}
	    }
	    اگر (++len == bufsize && end)
	    {
		اگر (buf_write_bytes(&write_info) == FAIL)
		{
		    end = 0;		// write error: break loop
		    بشکن;
		}
		nchars += bufsize;
		s = buffer;
		len = 0;

		ui_breakcheck();
		اگر (got_int)
		{
		    end = 0;		// Interrupted, break loop
		    بشکن;
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
	    اگر (buf->b_fab_rfm == FAB$C_VFC
		    || ((buf->b_fab_rat & (FAB$M_FTN | FAB$M_CR)) != 0))
	    {
		صحیح b2write;

		buf->b_fab_mrs = (buf->b_fab_mrs == 0
			? MIN(4096, bufsize)
			: MIN(buf->b_fab_mrs, bufsize));

		b2write = len;
		وقتی (b2write > 0)
		{
		    write_info.bw_len = MIN(b2write, buf->b_fab_mrs);
		    اگر (buf_write_bytes(&write_info) == FAIL)
		    {
			end = 0;
			بشکن;
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
	اگر (len > 0 && end > 0)
	{
	    write_info.bw_len = len;
#ifdef FEAT_CRYPT
	    اگر (write_info.bw_fd > 0 && lnum >= end
		    && (write_info.bw_flags & FIO_ENCRYPTED)
		    && *buf->b_p_کلید != NUL && !filtering)
		write_info.bw_finish = TRUE;
#endif
	    اگر (buf_write_bytes(&write_info) == FAIL)
		end = 0;		    // write error
	    nchars += len;
	}

	اگر (!buf->b_p_fixeol && buf->b_p_eof)
	{
	    // write trailing CTRL-Z
	    (تهی)write_eintr(write_info.bw_fd, "\x1a", 1);
	    nchars++;
	}

	// Stop when writing done or an error was encountered.
	اگر (!checking_conversion || end == 0)
	    بشکن;

	// If no error happened until now, writing should be ok, so loop to
	// really write the buffer.
    }

    // If we started writing, finish writing. Also when an error was
    // encountered.
    اگر (!checking_conversion)
    {
#اگر defined(UNIX) && defined(HAVE_FSYNC)
	// On many journaling file systems there is a bug that causes both the
	// original and the backup file to be lost when halting the system
	// right after writing the file.  That's because only the meta-data is
	// journalled.  Syncing the file slows down the system, but assures it
	// has been written to disk and we don't lose it.
	// For a device do try the fsync() but don't complain if it does not
	// work (could be a pipe).
	// If the 'fsync' option is FALSE, don't fsync().  Useful for laptops.
	اگر ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && vim_fsync(fd) != 0
		&& !device)
	{
	    errmsg = (char_u *)_(e_fsync_failed);
	    end = 0;
	}
#endif

#اگر defined(HAVE_SELINUX) || defined(HAVE_SMACK) || defined(FEAT_XATTR)
	// Probably need to set the security context.
	اگر (!backup_copy)
	{
# اگر defined(HAVE_SELINUX) || defined(HAVE_SMACK)
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
	اگر (backup != پوچ && !backup_copy)
	{
# ifdef HAVE_FCHOWN
	    stat_T	st;

	    // Don't change the owner when it's already OK, some systems remove
	    // permission or ACL stuff.
	    اگر (mch_stat((نویسه *)wfname, &st) < 0
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid)
	    {
		// changing owner might not be possible
		vim_ignored = fchown(fd, st_old.st_uid, -1);
		// if changing group fails clear the group permissions
		اگر (fchown(fd, -1, st_old.st_gid) == -1 && perm > 0)
		    perm &= ~070;
	    }
# endif
	    buf_setino(buf);
	}
	وگرنه اگر (!buf->b_dev_valid)
	    // Set the inode when creating a new file.
	    buf_setino(buf);
#endif

#ifdef UNIX
	اگر (made_writable)
	    perm &= ~0200;	// reset 'w' bit for security reasons
#endif
#ifdef HAVE_FCHMOD
	// set permission of new file same as old file
	اگر (perm >= 0)
	    (تهی)mch_fsetperm(fd, perm);
#endif
	اگر (close(fd) != 0)
	{
	    errmsg = (char_u *)_(e_close_failed);
	    end = 0;
	}

#ifndef HAVE_FCHMOD
	// set permission of new file same as old file
	اگر (perm >= 0)
	    (تهی)mch_setperm(wfname, perm);
#endif
#ifdef HAVE_ACL
	// Probably need to set the ACL before changing the user (can't set the
	// ACL on a file the user doesn't own).
	// On Solaris, with ZFS and the aclmode property set to "discard" (the
	// default), chmod() discards all part of a file's ACL that don't
	// represent the mode of the file.  It's non-trivial for us to discover
	// whether we're in that situation, so we simply always re-set the ACL.
# ifndef HAVE_SOLARIS_ZFS_ACL
	اگر (!backup_copy)
# endif
	    mch_set_acl(wfname, acl);
#endif
#ifdef FEAT_CRYPT
	اگر (buf->b_cryptstate != پوچ)
	{
	    crypt_free_state(buf->b_cryptstate);
	    buf->b_cryptstate = پوچ;
	}
#endif

#اگر defined(FEAT_EVAL)
	اگر (wfname != fname)
	{
	    // The file was written to a temp file, now it needs to be
	    // converted with 'charconvert' to (overwrite) the output file.
	    اگر (end != 0)
	    {
		اگر (eval_charconvert(enc_utf8 ? (char_u *)"utf-8" : p_enc,
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

    اگر (end == 0)
    {
	// Error encountered.
	اگر (errmsg == پوچ)
	{
	    اگر (write_info.bw_conv_error)
	    {
		اگر (write_info.bw_conv_error_lnum == 0)
		    errmsg = (char_u *)_(e_write_error_conversion_failed_make_fenc_empty_to_override);
		وگرنه
		{
		    errmsg_allocated = TRUE;
		    errmsg = alloc(300);
		    اگر (errmsg == پوچ)
			برو fail;
		    vim_snprintf((نویسه *)errmsg, 300, _(e_write_error_conversion_failed_in_سطر_nr_make_fenc_empty_to_override),
					 (long)write_info.bw_conv_error_lnum);
		}
	    }
	    وگرنه اگر (got_int)
		errmsg = (char_u *)_(e_interrupted);
	    وگرنه
		errmsg = (char_u *)_(e_write_error_file_system_full);
	}

	// If we have a backup file, try to put it in place of the new file,
	// because the new file is probably corrupt.  This avoids losing the
	// original file when trying to make a backup when writing the file a
	// second time.
	// When "backup_copy" is set we need to copy the backup over the new
	// file.  Otherwise rename the backup file.
	// If this is OK, don't give the extra warning message.
	اگر (backup != پوچ)
	{
	    اگر (backup_copy)
	    {
		// This may take a while, if we were interrupted let the user
		// know we got the message.
		اگر (got_int)
		{
		    پیام(_(e_interrupted));
		    out_flush();
		}
		اگر ((fd = mch_open((نویسه *)backup, O_RDONLY | O_EXTRA, 0)) >= 0)
		{
		    اگر ((write_info.bw_fd = mch_open((نویسه *)fname,
				    O_WRONLY | O_CREAT | O_TRUNC | O_EXTRA,
							   perm & 0777)) >= 0)
		    {
			// copy the file.
			write_info.bw_buf = smallbuf;
			write_info.bw_flags = FIO_NOCONVERT;
			وقتی ((write_info.bw_len = read_eintr(fd, smallbuf,
						      SMALLBUFSIZE)) > 0)
			    اگر (buf_write_bytes(&write_info) == FAIL)
				بشکن;

			اگر (close(write_info.bw_fd) >= 0
						   && write_info.bw_len == 0)
			    end = 1;		// success
		    }
		    close(fd);	// ignore errors for closing read file
		}
	    }
	    وگرنه
	    {
		اگر (vim_rename(backup, fname) == 0)
		    end = 1;
	    }
	}
	برو fail;
    }

    lnum -= start;	    // compute number of written lines
    --no_wait_return;	    // may wait for return now

#اگر !(defined(UNIX) || defined(VMS))
    fname = sfname;	    // use shortname now, for the messages
#endif
    اگر (!filtering)
    {
	پیام_add_fname(buf, fname);	// put fname in IObuff with quotes
	c = FALSE;
	اگر (write_info.bw_conv_error)
	{
	    STRCAT(IObuff, _(" CONVERSION ERROR"));
	    c = TRUE;
	    اگر (write_info.bw_conv_error_lnum != 0)
		vim_snprintf_add((نویسه *)IObuff, IOSIZE, _(" in line %ld;"),
			(long)write_info.bw_conv_error_lnum);
	}
	وگرنه اگر (notconverted)
	{
	    STRCAT(IObuff, _("[NOT converted]"));
	    c = TRUE;
	}
	وگرنه اگر (converted)
	{
	    STRCAT(IObuff, _("[converted]"));
	    c = TRUE;
	}
	اگر (device)
	{
	    STRCAT(IObuff, _("[Device]"));
	    c = TRUE;
	}
	وگرنه اگر (newfile)
	{
	    STRCAT(IObuff, new_file_message());
	    c = TRUE;
	}
	اگر (no_eol)
	{
	    پیام_add_eol();
	    c = TRUE;
	}
	// may add [unix/dos/mac]
	اگر (پیام_add_fileformat(fileformat))
	    c = TRUE;
#ifdef FEAT_CRYPT
	اگر (wb_flags & FIO_ENCRYPTED)
	{
	    crypt_append_پیام(buf);
	    c = TRUE;
	}
#endif
	پیام_add_سطرها(c, (long)lnum, nchars);	// add line/char count
	اگر (!shortmess(SHM_WRITE))
	{
	    اگر (append)
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [a]") : _(" appended"));
	    وگرنه
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [w]") : _(" written"));
	}

	set_keep_پیام((char_u *)پیام_trunc_attr((نویسه *)IObuff, FALSE, 0), 0);
    }

    // When written everything correctly: reset 'modified'.  Unless not
    // writing to the original file and '+' is not in 'cpoptions'.
    اگر (reset_changed && whole && !append
	    && !write_info.bw_conv_error
	    && (overwriting || vim_strchr(p_cpo, CPO_PLUS) != پوچ))
    {
	unchanged(buf, TRUE, FALSE);
	// b:changedtick may be incremented in unchanged() but that should not
	// trigger a TextChanged event.
	اگر (buf->b_last_changedtick + 1 == CHANGEDTICK(buf))
	    buf->b_last_changedtick = CHANGEDTICK(buf);
	u_unchanged(buf);
	u_update_save_nr(buf);
    }

    // If written to the current file, update the timestamp of the swap file
    // and reset the BF_WRITE_MASK flags. Also sets buf->b_mtime.
    اگر (overwriting)
    {
	ml_timestamp(buf);
	اگر (append)
	    buf->b_flags &= ~BF_NEW;
	وگرنه
	    buf->b_flags &= ~BF_WRITE_MASK;
    }

    // If we kept a backup until now, and we are in patch mode, then we make
    // the backup file our 'original' file.
    اگر (*p_pm && dobackup)
    {
	نویسه *org = (نویسه *)buf_modname((buf->b_p_sn || buf->b_shortname),
							  fname, p_pm, FALSE);

	اگر (backup != پوچ)
	{
	    stat_T	st;

	    // If the original file does not exist yet
	    // the current backup file becomes the original file
	    اگر (org == پوچ)
		emsg(_(e_patchmode_cant_save_original_file));
	    وگرنه اگر (mch_stat(org, &st) < 0)
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
	وگرنه
	{
	    صحیح empty_fd;

	    اگر (org == پوچ
		    || (empty_fd = mch_open(org,
				      O_CREAT | O_EXTRA | O_EXCL | O_NOFOLLOW,
					perm < 0 ? 0666 : (perm & 0777))) < 0)
	      emsg(_(e_patchmode_cant_touch_empty_original_file));
	    وگرنه
	      close(empty_fd);
	}
	اگر (org != پوچ)
	{
	    mch_setperm((char_u *)org, mch_getperm(fname) & 0777);
	    vim_free(org);
	}
    }

    // Remove the backup unless 'backup' option is set or there was a
    // conversion error.
    اگر (!p_bk && backup != پوچ && !write_info.bw_conv_error
	    && mch_remove(backup) != 0)
	emsg(_(e_cant_delete_backup_file));

    برو nofail;

    // Finish up.  We get here either after failure or success.
fail:
    --no_wait_return;		// may wait for return now
nofail:

    // Done saving, we accept changed buffer warnings again
    buf->b_saving = false;

    vim_free(backup);
    اگر (buffer != smallbuf)
	vim_free(buffer);
    vim_free(fenc_tofree);
    vim_free(write_info.bw_conv_buf);
#ifdef USE_ICONV
    اگر (write_info.bw_iconv_fd != (iconv_t)-1)
    {
	iconv_close(write_info.bw_iconv_fd);
	write_info.bw_iconv_fd = (iconv_t)-1;
    }
#endif
#ifdef HAVE_ACL
    mch_free_acl(acl);
#endif

    اگر (errmsg != پوچ)
    {
	صحیح numlen = errnum != پوچ ? (صحیح)STRLEN(errnum) : 0;

	attr = HL_ATTR(HLF_E);	// set highlight for error messages
	پیام_add_fname(buf,
#ifndef UNIX
		sfname
#وگرنه
		fname
#endif
		     );		// put file name in IObuff with quotes
	اگر (STRLEN(IObuff) + STRLEN(errmsg) + numlen >= IOSIZE)
	    IObuff[IOSIZE - STRLEN(errmsg) - numlen - 1] = NUL;
	// If the error message has the form "is ...", put the error number in
	// front of the file name.
	اگر (errnum != پوچ)
	{
	    STRMOVE(IObuff + numlen, IObuff);
	    mch_memmove(IObuff, errnum, (size_t)numlen);
	}
	STRCAT(IObuff, errmsg);
	emsg((نویسه *)IObuff);
	اگر (errmsg_allocated)
	    vim_free(errmsg);

	retval = FAIL;
	اگر (end == 0)
	{
	    پیام_puts_attr(_("\nWARNING: Original file may be lost or damaged\n"),
		    attr | پیام_HIST);
	    پیام_puts_attr(_("don't quit the editor until the file is successfully written!"),
		    attr | پیام_HIST);

	    // Update the timestamp to avoid an "overwrite changed file"
	    // prompt when writing again.
	    اگر (mch_stat((نویسه *)fname, &st_old) >= 0)
	    {
		buf_store_time(buf, &st_old, fname);
		buf->b_mtime_read = buf->b_mtime;
		buf->b_mtime_read_ns = buf->b_mtime_ns;
	    }
	}
    }
    پیام_scroll = پیام_save;

#ifdef FEAT_PERSISTENT_UNDO
    // When writing the whole file and 'undofile' is set, also write the undo
    // file.
    اگر (retval == OK && write_undo_file)
    {
	char_u	    hash[UNDO_HASH_SIZE];

	sha256_finish(&sha_ctx, hash);
	u_write_undo(پوچ, FALSE, buf, hash);
    }
#endif

#ifdef FEAT_EVAL
    اگر (!should_abort(retval))
#وگرنه
    اگر (!got_int)
#endif
    {
	aco_save_T	aco;

	curbuf->b_no_eol_lnum = 0;  // in case it was set by the previous read

	// Apply POST autocommands.
	// Careful: The autocommands may call buf_write() recursively!
	// Only do this when a window was found for "buf".
	aucmd_prepbuf(&aco, buf);
	اگر (curbuf == buf)
	{
	    اگر (append)
		apply_autocmds_exarg(EVENT_FILEAPPENDPOST, fname, fname,
							   FALSE, curbuf, eap);
	    وگرنه اگر (filtering)
		apply_autocmds_exarg(EVENT_FILTERWRITEPOST, پوچ, fname,
							   FALSE, curbuf, eap);
	    وگرنه اگر (reset_changed && whole)
		apply_autocmds_exarg(EVENT_BUFWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);
	    وگرنه
		apply_autocmds_exarg(EVENT_FILEWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}

#ifdef FEAT_EVAL
	اگر (aborting())	    // autocmds may abort script processing
	    retval = FALSE;
#endif
    }

#ifdef FEAT_VIMINFO
    // Make sure marks will be written out to the viminfo file later, even when
    // the file is new.
    curbuf->b_marks_read = true;
#endif

    got_int |= prev_got_int;

    بازگشت retval;
}
