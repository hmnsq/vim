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

#ضمن "vim.h"

#אם defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
# include <utime.h>		// for struct utimbuf
#endif

#define SMALLBUFSIZE	256	// size of emergency write buffer

/*
 * Structure to pass arguments from buf_write() to buf_write_bytes().
 */
بنية bw_info
{
    صحيح		bw_fd;		// file descriptor
    char_u	*bw_buf;	// buffer with data to be written
    صحيح		bw_len;		// length of data
    صحيح		bw_flags;	// FIO_ flags
#ifdef FEAT_CRYPT
    buf_T	*bw_buffer;	// buffer being written
    صحيح		bw_finish;	// finish encrypting
#endif
    char_u	bw_rest[CONV_RESTLEN]; // not converted bytes
    صحيح		bw_restlen;	// nr of bytes in bw_rest[]
    صحيح		bw_first;	// first write call
    char_u	*bw_conv_buf;	// buffer for writing converted chars
    size_t	bw_conv_buflen; // size of bw_conv_buf
    صحيح		bw_conv_error;	// set for conversion error
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
    ساكن صحيح
ucs2bytes(
    unsigned	c,		// in: character
    char_u	**pp,		// in/out: pointer to result
    صحيح		flags)		// FIO_ flags
{
    char_u	*p = *pp;
    صحيح		error = FALSE;
    صحيح		cc;


    אם (flags & FIO_UCS4)
    {
	אם (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	    *p++ = (c >> 16);
	    *p++ = (c >> 24);
	}
	אחרת
	{
	    *p++ = (c >> 24);
	    *p++ = (c >> 16);
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    אחרת אם (flags & (FIO_UCS2 | FIO_UTF16))
    {
	אם (c >= 0x10000)
	{
	    אם (flags & FIO_UTF16)
	    {
		// Make two words, ten bits of the character in each.  First
		// word is 0xd800 - 0xdbff, second one 0xdc00 - 0xdfff
		c -= 0x10000;
		אם (c >= 0x100000)
		    error = TRUE;
		cc = ((c >> 10) & 0x3ff) + 0xd800;
		אם (flags & FIO_ENDIAN_L)
		{
		    *p++ = cc;
		    *p++ = ((unsigned)cc >> 8);
		}
		אחרת
		{
		    *p++ = ((unsigned)cc >> 8);
		    *p++ = cc;
		}
		c = (c & 0x3ff) + 0xdc00;
	    }
	    אחרת
		error = TRUE;
	}
	אם (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	}
	אחרת
	{
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    אחרת    // Latin1
    {
	אם (c >= 0x100)
	{
	    error = TRUE;
	    *p++ = 0xBF;
	}
	אחרת
	    *p++ = c;
    }

    *pp = p;
    החזר error;
}

/*
 * Call write() to write a number of bytes to the file.
 * Handles encryption and 'encoding' conversion.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ساكن صحيح
buf_write_bytes(بنية bw_info *ip)
{
    صحيح		wlen;
    char_u	*buf = ip->bw_buf;	// data to write
    صحيح		len = ip->bw_len;	// length of data
    صحيح		flags = ip->bw_flags;	// extra flags

    // Skip conversion when writing the crypt magic number or the BOM.
    אם (!(flags & FIO_NOCONVERT))
    {
	char_u		*p;
	unsigned	c;
	صحيح		n;

	אם (flags & FIO_UTF8)
	{
	    // Convert latin1 in the buffer to UTF-8 in the file.
	    p = ip->bw_conv_buf;	// translate to buffer
	    עבור (wlen = 0; wlen < len; ++wlen)
		p += utf_char2bytes(buf[wlen], p);
	    buf = ip->bw_conv_buf;
	    len = (صحيح)(p - ip->bw_conv_buf);
	}
	אחרת אם (flags & (FIO_UCS4 | FIO_UTF16 | FIO_UCS2 | FIO_LATIN1))
	{
	    // Convert UTF-8 bytes in the buffer to UCS-2, UCS-4, UTF-16 or
	    // Latin1 chars in the file.
	    אם (flags & FIO_LATIN1)
		p = buf;	// translate in-place (can only get shorter)
	    אחרת
		p = ip->bw_conv_buf;	// translate to buffer
	    עבור (wlen = 0; wlen < len; wlen += n)
	    {
		אם (wlen == 0 && ip->bw_restlen != 0)
		{
		    صحيح		l;

		    // Use remainder of previous call.  Append the start of
		    // buf[] to get a full sequence.  Might still be too
		    // short!
		    l = CONV_RESTLEN - ip->bw_restlen;
		    אם (l > len)
			l = len;
		    mch_memmove(ip->bw_rest + ip->bw_restlen, buf, (size_t)l);
		    n = utf_ptr2len_len(ip->bw_rest, ip->bw_restlen + l);
		    אם (n > ip->bw_restlen + len)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			אם (ip->bw_restlen + len > CONV_RESTLEN)
			    החזר FAIL;
			ip->bw_restlen += len;
			اكسر;
		    }
		    אם (n > 1)
			c = utf_ptr2char(ip->bw_rest);
		    אחרת
			c = ip->bw_rest[0];
		    אם (n >= ip->bw_restlen)
		    {
			n -= ip->bw_restlen;
			ip->bw_restlen = 0;
		    }
		    אחרת
		    {
			ip->bw_restlen -= n;
			mch_memmove(ip->bw_rest, ip->bw_rest + n,
						      (size_t)ip->bw_restlen);
			n = 0;
		    }
		}
		אחרת
		{
		    n = utf_ptr2len_len(buf + wlen, len - wlen);
		    אם (n > len - wlen)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			אם (len - wlen > CONV_RESTLEN)
			    החזר FAIL;
			ip->bw_restlen = len - wlen;
			mch_memmove(ip->bw_rest, buf + wlen,
						      (size_t)ip->bw_restlen);
			اكسر;
		    }
		    אם (n > 1)
			c = utf_ptr2char(buf + wlen);
		    אחרת
			c = buf[wlen];
		}
		// Check that there is enough space
		אם (!(flags & FIO_LATIN1))
		{
		    size_t need = (flags & FIO_UCS4) ? 4 : 2;
		    אם ((flags & FIO_UTF16) && c >= 0x10000)
			need = 4;

		    אם ((size_t)(p - ip->bw_conv_buf) + need > ip->bw_conv_buflen)
			החזר FAIL;
		}

		אם (ucs2bytes(c, &p, flags) && !ip->bw_conv_error)
		{
		    ip->bw_conv_error = TRUE;
		    ip->bw_conv_error_lnum = ip->bw_start_lnum;
		}
		אם (c == NL)
		    ++ip->bw_start_lnum;
	    }
	    אם (flags & FIO_LATIN1)
		len = (صحيح)(p - buf);
	    אחרת
	    {
		buf = ip->bw_conv_buf;
		len = (صحيح)(p - ip->bw_conv_buf);
	    }
	}

#ifdef MSWIN
	אחרת אם (flags & FIO_CODEPAGE)
	{
	    // Convert UTF-8 or codepage to UCS-2 and then to MS-Windows
	    // codepage.
	    char_u	*from;
	    size_t	fromlen;
	    char_u	*to;
	    صحيح		u8c;
	    BOOL	bad = FALSE;
	    صحيح		needed;

	    אם (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    אחרת
	    {
		from = buf;
		fromlen = len;
	    }

	    to = ip->bw_conv_buf;
	    אם (enc_utf8)
	    {
		// Convert from UTF-8 to UCS-2, to the start of the buffer.
		// The buffer has been allocated to be big enough.
		כלעוד (fromlen > 0)
		{
		    n = (صحيح)utf_ptr2len_len(from, (صحيح)fromlen);
		    אם (n > (صحيح)fromlen)	// incomplete byte sequence
			اكسر;
		    u8c = utf_ptr2char(from);
		    *to++ = (u8c & 0xff);
		    *to++ = (u8c >> 8);
		    fromlen -= n;
		    from += n;
		}

		// Copy remainder to ip->bw_rest[] to be used for the next
		// call.
		אם (fromlen > CONV_RESTLEN)
		{
		    // weird overlong sequence
		    ip->bw_conv_error = TRUE;
		    החזר FAIL;
		}
		mch_memmove(ip->bw_rest, from, fromlen);
		ip->bw_restlen = (صحيح)fromlen;
	    }
	    אחרת
	    {
		// Convert from enc_codepage to UCS-2, to the start of the
		// buffer.  The buffer has been allocated to be big enough.
		ip->bw_restlen = 0;
		needed = MultiByteToWideChar(enc_codepage,
			     MB_ERR_INVALID_CHARS, (LPCSTR)from, (صحيح)fromlen,
								     عدم, 0);
		אם (needed == 0)
		{
		    // When conversion fails there may be a trailing byte.
		    needed = MultiByteToWideChar(enc_codepage,
			 MB_ERR_INVALID_CHARS, (LPCSTR)from, (صحيح)fromlen - 1,
								     عدم, 0);
		    אם (needed == 0)
		    {
			// Conversion doesn't work.
			ip->bw_conv_error = TRUE;
			החזר FAIL;
		    }
		    // Save the trailing byte for the next call.
		    ip->bw_rest[0] = from[fromlen - 1];
		    ip->bw_restlen = 1;
		}
		needed = MultiByteToWideChar(enc_codepage, MB_ERR_INVALID_CHARS,
				(LPCSTR)from, (صحيح)(fromlen - ip->bw_restlen),
							  (LPWSTR)to, needed);
		אם (needed == 0)
		{
		    // Safety check: Conversion doesn't work.
		    ip->bw_conv_error = TRUE;
		    החזר FAIL;
		}
		to += needed * 2;
	    }

	    fromlen = to - ip->bw_conv_buf;
	    buf = to;
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	    אם (FIO_GET_CP(flags) == CP_UTF8)
	    {
		// Convert from UCS-2 to UTF-8, using the remainder of the
		// conversion buffer.  Fails when out of space.
		עבור (from = ip->bw_conv_buf; fromlen > 1; fromlen -= 2)
		{
		    u8c = *from++;
		    u8c += (*from++ << 8);
		    to += utf_char2bytes(u8c, to);
		    אם (to + 6 >= ip->bw_conv_buf + ip->bw_conv_buflen)
		    {
			ip->bw_conv_error = TRUE;
			החזר FAIL;
		    }
		}
		len = (صحيح)(to - buf);
	    }
	    אחרת
# endif
	    {
		// Convert from UCS-2 to the codepage, using the remainder of
		// the conversion buffer.  If the conversion uses the default
		// character "0", the data doesn't fit in this encoding, so
		// fail.
		len = WideCharToMultiByte(FIO_GET_CP(flags), 0,
			(LPCWSTR)ip->bw_conv_buf, (صحيح)fromlen / حجم(WCHAR),
			(LPSTR)to, (صحيح)(ip->bw_conv_buflen - fromlen), 0,
									&bad);
		אם (bad)
		{
		    ip->bw_conv_error = TRUE;
		    החזר FAIL;
		}
	    }
	}
#endif

#ifdef MACOS_CONVERT
	אחרת אם (flags & FIO_MACROMAN)
	{
	    // Convert UTF-8 or latin1 to Apple MacRoman.
	    char_u	*from;
	    size_t	fromlen;

	    אם (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    אחרת
	    {
		from = buf;
		fromlen = len;
	    }

	    אם (enc2macroman(from, fromlen,
			ip->bw_conv_buf, &len, ip->bw_conv_buflen,
			ip->bw_rest, &ip->bw_restlen) == FAIL)
	    {
		ip->bw_conv_error = TRUE;
		החזר FAIL;
	    }
	    buf = ip->bw_conv_buf;
	}
#endif

#ifdef USE_ICONV
	אם (ip->bw_iconv_fd != (iconv_t)-1)
	{
	    ثابت حرف	*from;
	    size_t	fromlen;
	    حرف	*to;
	    size_t	tolen;

	    // Convert with iconv().
	    אם (ip->bw_restlen > 0)
	    {
		حرف *fp;

		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		fp = (حرف *)ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(fp, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(fp + ip->bw_restlen, buf, (size_t)len);
		from = fp;
		tolen = ip->bw_conv_buflen - fromlen;
	    }
	    אחרת
	    {
		from = (ثابت حرف *)buf;
		fromlen = len;
		tolen = ip->bw_conv_buflen;
	    }
	    to = (حرف *)ip->bw_conv_buf;

	    אם (ip->bw_first)
	    {
		size_t	save_len = tolen;

		// output the initial shift state sequence
		(فراغ)iconv(ip->bw_iconv_fd, عدم, عدم, &to, &tolen);

		// There is a bug in iconv() on Linux (which appears to be
		// wide-spread) which sets "to" to NULL and messes up "tolen".
		אם (to == عدم)
		{
		    to = (حرف *)ip->bw_conv_buf;
		    tolen = save_len;
		}
		ip->bw_first = FALSE;
	    }

	    // If iconv() has an error or there is not enough room, fail.
	    אם ((iconv(ip->bw_iconv_fd, (فراغ *)&from, &fromlen, &to, &tolen)
			== (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						    || fromlen > CONV_RESTLEN)
	    {
		ip->bw_conv_error = TRUE;
		החזר FAIL;
	    }

	    // copy remainder to ip->bw_rest[] to be used for the next call.
	    אם (fromlen > 0)
		mch_memmove(ip->bw_rest, (فراغ *)from, fromlen);
	    ip->bw_restlen = (صحيح)fromlen;

	    buf = ip->bw_conv_buf;
	    len = (صحيح)((char_u *)to - ip->bw_conv_buf);
	}
#endif
    }

    אם (ip->bw_fd < 0)
	// Only checking conversion, which is OK if we get here.
	החזר OK;

#ifdef FEAT_CRYPT
    אם (flags & FIO_ENCRYPTED)
    {
	// Encrypt the data. Do it in-place if possible, otherwise use an
	// allocated buffer.
# ifdef CRYPT_NOT_INPLACE
	אם (crypt_works_inplace(ip->bw_buffer->b_cryptstate))
	{
# endif
	    crypt_encode_inplace(ip->bw_buffer->b_cryptstate, buf, len,
								ip->bw_finish);
# ifdef CRYPT_NOT_INPLACE
	}
	אחרת
	{
	    char_u *outbuf;

	    len = crypt_encode_alloc(curbuf->b_cryptstate, buf, len, &outbuf,
								ip->bw_finish);
	    אם (len == 0)
		החזר OK;  // Crypt layer is buffering, will flush later.
	    wlen = write_eintr(ip->bw_fd, outbuf, len);
	    vim_free(outbuf);
	    החזר (wlen < len) ? FAIL : OK;
	}
# endif
    }
#endif

    wlen = write_eintr(ip->bw_fd, buf, len);
    החזר (wlen < len) ? FAIL : OK;
}

/*
 * Check modification time of file, before writing to it.
 * The size isn't checked, because using a tool like "gzip" takes care of
 * using the same timestamp but can't set the size.
 */
    ساكن صحيح
check_mtime(buf_T *buf, stat_T *st)
{
    אם (buf->b_mtime_read != 0
		  && time_differs(st, buf->b_mtime_read, buf->b_mtime_read_ns))
    {
	msg_scroll = TRUE;	    // don't overwrite messages here
	msg_silent = 0;		    // must give this prompt
	// don't use emsg() here, don't want to flush the buffers
	msg_attr(_("WARNING: The file has been changed since reading it!!!"),
						       HL_ATTR(HLF_E));
	אם (ask_yesno((char_u *)_("Do you really want to write to it"),
								 TRUE) == 'n')
	    החזר FAIL;
	msg_scroll = FALSE;	    // always overwrite the file message now
    }
    החזר OK;
}

/*
 * Generate a BOM in "buf[4]" for encoding "name".
 * Return the length of the BOM (zero when no BOM).
 */
    ساكن صحيح
make_bom(char_u *buf, char_u *name)
{
    صحيح		flags;
    char_u	*p;

    flags = get_fio_flags(name);

    // Can't put a BOM in a non-Unicode file.
    אם (flags == FIO_LATIN1 || flags == 0)
	החזר 0;

    אם (flags == FIO_UTF8)	// UTF-8
    {
	buf[0] = 0xef;
	buf[1] = 0xbb;
	buf[2] = 0xbf;
	החזר 3;
    }
    p = buf;
    (فراغ)ucs2bytes(0xfeff, &p, flags);
    החזר (صحيح)(p - buf);
}

#ifdef UNIX
    ساكن فراغ
set_file_time(
    char_u  *fname,
    time_t  atime,	    // access time
    time_t  mtime)	    // modification time
{
# אם defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
    بنية utimbuf  buf;

    buf.actime	= atime;
    buf.modtime	= mtime;
    (فراغ)utime((حرف *)fname, &buf);
# אחרת
#  אם defined(HAVE_UTIMES)
    بنية timeval  tvp[2];

    tvp[0].tv_sec   = atime;
    tvp[0].tv_usec  = 0;
    tvp[1].tv_sec   = mtime;
    tvp[1].tv_usec  = 0;
    (فراغ)utimes((حرف *)fname, (ثابت بنية timeval *)&tvp);
#  endif
# endif
}
#endif // UNIX

    حرف *
new_file_message(فراغ)
{
    החזר shortmess(SHM_NEW) ? _("[New]") : _("[New File]");
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
    صحيح
buf_write(
    buf_T	    *buf,
    char_u	    *fname,
    char_u	    *sfname,
    linenr_T	    start,
    linenr_T	    end,
    exarg_T	    *eap,		// for forced 'ff' and 'fenc', can be
					// NULL!
    صحيح		    append,		// append to the file
    صحيح		    forceit,
    صحيح		    reset_changed,
    صحيح		    filtering)
{
    صحيح		    fd;
    char_u	    *backup = عدم;
    صحيح		    backup_copy = FALSE; // copy the original file?
    صحيح		    dobackup;
    char_u	    *ffname;
    char_u	    *wfname = عدم;	// name of file to write to
    char_u	    *s;
    char_u	    *ptr;
    char_u	    c;
    صحيح		    len;
    linenr_T	    lnum;
    long	    nchars;
    char_u	    *errmsg = عدم;
    صحيح		    errmsg_allocated = FALSE;
    char_u	    *errnum = عدم;
    char_u	    *buffer;
    char_u	    smallbuf[SMALLBUFSIZE];
    char_u	    *backup_ext;
    صحيح		    bufsize;
    long	    perm;		    // file permissions
    صحيح		    retval = OK;
    صحيح		    newfile = FALSE;	    // TRUE if file doesn't exist yet
    صحيح		    msg_save = msg_scroll;
    صحيح		    overwriting;	    // TRUE if writing over original
    صحيح		    no_eol = FALSE;	    // no end-of-line written
    صحيح		    device = FALSE;	    // writing to a device
    stat_T	    st_old;
    صحيح		    prev_got_int = got_int;
    صحيح		    checking_conversion;
    صحيح		    file_readonly = FALSE;  // overwritten file is read-only
#אם defined(UNIX)			    // XXX fix me sometime?
    صحيح		    made_writable = FALSE;  // 'w' bit has been set
#endif
					// writing everything
    صحيح		    whole = (start == 1 && end == buf->b_ml.ml_line_count);
    linenr_T	    old_line_count = buf->b_ml.ml_line_count;
    صحيح		    attr;
    صحيح		    fileformat;
    صحيح		    write_bin;
    بنية bw_info  write_info;		// info for buf_write_bytes()
    صحيح		    converted = FALSE;
    صحيح		    notconverted = FALSE;
    char_u	    *fenc;		// effective 'fileencoding'
    char_u	    *fenc_tofree = عدم; // allocated "fenc"
    صحيح		    wb_flags = 0;
#ifdef HAVE_ACL
    vim_acl_T	    acl = عدم;		// ACL copied from original file to
					// backup or new file
#endif
#ifdef FEAT_PERSISTENT_UNDO
    صحيح		    write_undo_file = FALSE;
    context_sha256_T sha_ctx;
#endif
    unsigned صحيح    bkc = get_bkc_flags(buf);
    pos_T	    orig_start = buf->b_op_start;
    pos_T	    orig_end = buf->b_op_end;

    אם (fname == عدم || *fname == NUL)	// safety check
	החזר FAIL;
    אם (buf->b_ml.ml_mfp == عدم)
    {
	// This can happen during startup when there is a stray "w" in the
	// vimrc file.
	emsg(_(e_empty_buffer));
	החזר FAIL;
    }

    // Disallow writing from .exrc and .vimrc in current directory for
    // security reasons.
    אם (check_secure())
	החזר FAIL;

    // Avoid a crash for a long name.
    אם (STRLEN(fname) >= MAXPATHL)
    {
	emsg(_(e_name_too_long));
	החזר FAIL;
    }

    // must init bw_conv_buf and bw_iconv_fd before jumping to "fail"
    write_info.bw_conv_buf = عدم;
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
    אם (buf->b_ffname == عدم
	    && reset_changed
	    && whole
	    && buf == curbuf
	    && !bt_nofilename(buf)
	    && !filtering
	    && (!append || vim_strchr(p_cpo, CPO_FNAMEAPP) != عدم)
	    && vim_strchr(p_cpo, CPO_FNAMEW) != عدم)
    {
	אם (set_rw_fname(fname, sfname) == FAIL)
	    החזר FAIL;
	buf = curbuf;	    // just in case autocmds made "buf" invalid
    }

    אם (sfname == عدم)
	sfname = fname;
    // For Unix: Use the short file name whenever possible.
    // Avoids problems with networks and when directory names are changed.
    // Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
    // another directory, which we don't detect
    ffname = fname;			    // remember full fname
#ifdef UNIX
    fname = sfname;
#endif

    אם (buf->b_ffname != عدم && fnamecmp(ffname, buf->b_ffname) == 0)
	overwriting = TRUE;
    אחרת
	overwriting = FALSE;

    אם (exiting)
	settmode(TMODE_COOK);	    // when exiting allow typeahead now

    ++no_wait_return;		    // don't wait for return yet

    // Set '[ and '] marks to the lines to be written.
    buf->b_op_start.lnum = start;
    buf->b_op_start.col = 0;
    buf->b_op_end.lnum = end;
    buf->b_op_end.col = 0;

    {
	aco_save_T	aco;
	صحيح		buf_ffname = FALSE;
	صحيح		buf_sfname = FALSE;
	صحيح		buf_fname_f = FALSE;
	صحيح		buf_fname_s = FALSE;
	صحيح		did_cmd = FALSE;
	صحيح		nofile_err = FALSE;
	صحيح		empty_memline = (buf->b_ml.ml_mfp == عدم);
	bufref_T	bufref;

	// Apply PRE autocommands.
	// Set curbuf to the buffer to be written.
	// Careful: The autocommands may call buf_write() recursively!
	אם (ffname == buf->b_ffname)
	    buf_ffname = TRUE;
	אם (sfname == buf->b_sfname)
	    buf_sfname = TRUE;
	אם (fname == buf->b_ffname)
	    buf_fname_f = TRUE;
	אם (fname == buf->b_sfname)
	    buf_fname_s = TRUE;

	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, buf);
	אם (curbuf != buf)
	{
	    // Could not find a window for "buf".  Doing more might cause
	    // problems, better bail out.
	    החזר FAIL;
	}

	set_bufref(&bufref, buf);

	אם (append)
	{
	    אם (!(did_cmd = apply_autocmds_exarg(EVENT_FILEAPPENDCMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		אם (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		אחרת
		    apply_autocmds_exarg(EVENT_FILEAPPENDPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	אחרת אם (filtering)
	{
	    apply_autocmds_exarg(EVENT_FILTERWRITEPRE,
					    عدم, sfname, FALSE, curbuf, eap);
	}
	אחרת אם (reset_changed && whole)
	{
	    صحيح was_changed = curbufIsChanged();

	    did_cmd = apply_autocmds_exarg(EVENT_BUFWRITECMD,
					  sfname, sfname, FALSE, curbuf, eap);
	    אם (did_cmd)
	    {
		אם (was_changed && !curbufIsChanged())
		{
		    // Written everything correctly and BufWriteCmd has reset
		    // 'modified': Correct the undo information so that an
		    // undo now sets 'modified'.
		    u_unchanged(curbuf);
		    u_update_save_nr(curbuf);
		}
	    }
	    אחרת
	    {
		אם (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		אחרת
		    apply_autocmds_exarg(EVENT_BUFWRITEPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	אחרת
	{
	    אם (!(did_cmd = apply_autocmds_exarg(EVENT_FILEWRITECMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		אם (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		אחרת
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
	אם (!bufref_valid(&bufref))
	    buf = عدم;
	אם (buf == عدم || (buf->b_ml.ml_mfp == عدم && !empty_memline)
				       || did_cmd || nofile_err
#ifdef FEAT_EVAL
				       || aborting()
#endif
				       )
	{
	    אם (buf != عدم && (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	    {
		// restore the original '[ and '] positions
		buf->b_op_start = orig_start;
		buf->b_op_end = orig_end;
	    }

	    --no_wait_return;
	    msg_scroll = msg_save;
	    אם (nofile_err)
		semsg(_(e_no_matching_autocommands_for_buftype_str_buffer),
							       curbuf->b_p_bt);

	    אם (nofile_err
#ifdef FEAT_EVAL
		    || aborting()
#endif
		    )
		// An aborting error, interrupt or exception in the
		// autocommands.
		החזר FAIL;
	    אם (did_cmd)
	    {
		אם (buf == عدم)
		    // The buffer was deleted.  We assume it was written
		    // (can't retry anyway).
		    החזר OK;
		אם (overwriting)
		{
		    // Assume the buffer was written, update the timestamp.
		    ml_timestamp(buf);
		    אם (append)
			buf->b_flags &= ~BF_NEW;
		    אחרת
			buf->b_flags &= ~BF_WRITE_MASK;
		}
		אם (reset_changed && buf->b_changed && !append
			&& (overwriting || vim_strchr(p_cpo, CPO_PLUS) != عدم))
		    // Buffer still changed, the autocommands didn't work
		    // properly.
		    החזר FAIL;
		החזר OK;
	    }
#ifdef FEAT_EVAL
	    אם (!aborting())
#endif
		emsg(_(e_autocommands_deleted_or_unloaded_buffer_to_be_written));
	    החזר FAIL;
	}

	// The autocommands may have changed the number of lines in the file.
	// When writing the whole file, adjust the end.
	// When writing part of the file, assume that the autocommands only
	// changed the number of lines that are to be written (tricky!).
	אם (buf->b_ml.ml_line_count != old_line_count)
	{
	    אם (whole)						// write all
		end = buf->b_ml.ml_line_count;
	    אחרת אם (buf->b_ml.ml_line_count > old_line_count)	// more lines
		end += buf->b_ml.ml_line_count - old_line_count;
	    אחרת						// less lines
	    {
		end -= old_line_count - buf->b_ml.ml_line_count;
		אם (end < start)
		{
		    --no_wait_return;
		    msg_scroll = msg_save;
		    emsg(_(e_autocommands_changed_number_of_lines_in_unexpected_way));
		    החזר FAIL;
		}
	    }
	}

	// The autocommands may have changed the name of the buffer, which may
	// be kept in fname, ffname and sfname.  A ":cd" may also have dropped
	// the short name, then b_fname is the full name.
	אם (buf_ffname)
	    ffname = buf->b_ffname;
	אם (buf_sfname)
	    sfname = buf->b_fname;
	אם (buf_fname_f)
	    fname = buf->b_ffname;
	אם (buf_fname_s)
	    fname = buf->b_fname;
    }

    אם (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	// restore the original '[ and '] positions
	buf->b_op_start = orig_start;
	buf->b_op_end = orig_end;
    }

#ifdef FEAT_NETBEANS_INTG
    אם (netbeans_active() && isNetbeansBuffer(buf))
    {
	אם (whole)
	{
	    // b_changed can be 0 after an undo, but we still need to write
	    // the buffer to NetBeans.
	    אם (buf->b_changed || isNetbeansModified(buf))
	    {
		--no_wait_return;		// may wait for return now
		msg_scroll = msg_save;
		netbeans_save_buffer(buf);	// no error checking...
		החזר retval;
	    }
	    אחרת
	    {
		errnum = (char_u *)"E656: ";
		errmsg = (char_u *)_(e_netbeans_disallows_writes_of_unmodified_buffers);
		buffer = عدم;
		اذهب fail;
	    }
	}
	אחרת
	{
	    errnum = (char_u *)"E657: ";
	    errmsg = (char_u *)_(e_partial_writes_disallowed_for_netbeans_buffers);
	    buffer = عدم;
	    اذهب fail;
	}
    }
#endif

    אם (shortmess(SHM_OVER) && !exiting)
	msg_scroll = FALSE;	    // overwrite previous file message
    אחרת
	msg_scroll = TRUE;	    // don't overwrite previous file message
    אם (!filtering)
	filemess(buf,
#ifndef UNIX
		sfname,
#אחרת
		fname,
#endif
		    (char_u *)"", 0);	// show that we are busy
    msg_scroll = FALSE;		    // always overwrite the file message now

    buffer = alloc(WRITEBUFSIZE);
    אם (buffer == عدم)		    // can't allocate big buffer, use small
				    // one (to be able to write when out of
				    // memory)
    {
	buffer = smallbuf;
	bufsize = SMALLBUFSIZE;
    }
    אחרת
	bufsize = WRITEBUFSIZE;

    // Get information about original file (if there is one).
#אם defined(UNIX)
    st_old.st_dev = 0;
    st_old.st_ino = 0;
    perm = -1;
    אם (mch_stat((حرف *)fname, &st_old) < 0)
	newfile = TRUE;
    אחרת
    {
	perm = st_old.st_mode;
	אם (!S_ISREG(st_old.st_mode))		// not a file
	{
	    אם (S_ISDIR(st_old.st_mode))
	    {
		errnum = (char_u *)"E502: ";
		errmsg = (char_u *)_(e_is_a_directory);
		اذهب fail;
	    }
	    אם (mch_nodetype(fname) != NODE_WRITABLE)
	    {
		errnum = (char_u *)"E503: ";
		errmsg = (char_u *)_(e_is_not_file_or_writable_device);
		اذهب fail;
	    }
	    // It's a device of some kind (or a fifo) which we can write to
	    // but for which we can't make a backup.
	    device = TRUE;
	    newfile = TRUE;
	    perm = -1;
	}
    }
#אחרת // !UNIX
    // Check for a writable device name.
    c = mch_nodetype(fname);
    אם (c == NODE_OTHER)
    {
	errnum = (char_u *)"E503: ";
	errmsg = (char_u *)_(e_is_not_file_or_writable_device);
	اذهب fail;
    }
    אם (c == NODE_WRITABLE)
    {
# אם defined(MSWIN)
	// MS-Windows allows opening a device, but we will probably get stuck
	// trying to write to it.
	אם (!p_odev)
	{
	    errnum = (char_u *)"E796: ";
	    errmsg = (char_u *)_(e_writing_to_device_disabled_with_opendevice_option);
	    اذهب fail;
	}
# endif
	device = TRUE;
	newfile = TRUE;
	perm = -1;
    }
    אחרת
    {
	perm = mch_getperm(fname);
	אם (perm < 0)
	    newfile = TRUE;
	אחרת אם (mch_isdir(fname))
	{
	    errnum = (char_u *)"E502: ";
	    errmsg = (char_u *)_(e_is_a_directory);
	    اذهب fail;
	}
	אם (overwriting)
	    (فراغ)mch_stat((حرف *)fname, &st_old);
    }
#endif // !UNIX

    אם (!device && !newfile)
    {
	// Check if the file is really writable (when renaming the file to
	// make a backup we won't discover it later).
	file_readonly = check_file_readonly(fname, (صحيح)perm);

	אם (!forceit && file_readonly)
	{
	    אם (vim_strchr(p_cpo, CPO_FWRITE) != عدم)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_in_cpoptions);
	    }
	    אחרת
	    {
		errnum = (char_u *)"E505: ";
		errmsg = (char_u *)_(e_is_read_only_add_bang_to_override);
	    }
	    اذهب fail;
	}

	// Check if the timestamp hasn't changed since reading the file.
	אם (overwriting)
	{
	    retval = check_mtime(buf, &st_old);
	    אם (retval == FAIL)
		اذهب fail;
	}
    }

#ifdef HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    אם (!newfile)
	acl = mch_get_acl(fname);
#endif

    // If 'backupskip' is not empty, don't make a backup for some files.
    dobackup = (p_wb || p_bk || *p_pm != NUL);
    אם (dobackup && *p_bsk != NUL && match_file_list(p_bsk, sfname, ffname))
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
    אם (!(append && *p_pm == NUL) && !filtering && perm >= 0 && dobackup)
    {
#אם defined(UNIX) || defined(MSWIN)
	stat_T	    st;
#endif

	אם ((bkc & BKC_YES) || append)	// "yes"
	    backup_copy = TRUE;
#אם defined(UNIX) || defined(MSWIN)
	אחרת אם ((bkc & BKC_AUTO))	// "auto"
	{
# ifdef UNIX
	    // Don't rename the file when:
	    // - it's a hard link
	    // - it's a symbolic link
	    // - we don't have write permission in the directory
	    // - we can't set the owner/group of the new file
	    אם (st_old.st_nlink > 1
		    || mch_lstat((حرف *)fname, &st) < 0
		    || st.st_dev != st_old.st_dev
		    || st.st_ino != st_old.st_ino
#  ifndef HAVE_FCHOWN
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid
#  endif
		    )
		backup_copy = TRUE;
	    אחרת
# אחרת
#  ifdef MSWIN
	    // On NTFS file systems hard links are possible.
	    אם (mch_is_linked(fname))
		backup_copy = TRUE;
	    אחרת
#  endif
# endif
	    {
		size_t	dirlen;
		char_u	tmp_fname[MAXPATHL];
		صحيح	i;

		// Check if we can create a file and set the owner/group/mode to
		// the ones from the original file.
		// First find a file name that doesn't exist yet.  Use the
		// conventional hidden swap file name so that this short-lived
		// probe is covered by standard Vim ignore patterns.
		dirlen = (size_t)(gettail(fname) - fname);
		vim_strncpy(tmp_fname, fname, dirlen);
		fd = -1;
		עבור (i = 4913; ; i += 123)
		{
		    vim_snprintf((حرف *)tmp_fname + dirlen,
			حجم(tmp_fname) - dirlen, ".%d.swp", i);
		    אם (mch_lstat((حرف *)tmp_fname, &st) < 0)
		    {
			fd = mch_open((حرف *)tmp_fname,
				    O_CREAT|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
			אם (fd < 0 && errno == EEXIST)
			    // If the same file name is created by another
			    // process between lstat() and open(), find another
			    // name.
			    استمر;
			اكسر;
		    }
		}
		אם (fd < 0)	// can't write in directory
		    backup_copy = TRUE;
		אחרת
		{
# ifdef UNIX
#  ifdef HAVE_FCHOWN
		    vim_ignored = fchown(fd, st_old.st_uid, st_old.st_gid);
#  endif
#  ifdef HAVE_FCHMOD
		    (فراغ)mch_fsetperm(fd, perm);
#  אחרת
		    (فراغ)mch_setperm(tmp_fname, perm);
#  endif
		    אם (mch_stat((حرف *)tmp_fname, &st) < 0
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
			صحيح try;

			עבור (try = 0; try < 10; ++try)
			{
			    אם (mch_lstat((حرف *)tmp_fname, &st) < 0)
				اكسر;
			    ui_delay(50L, TRUE);  // wait 50 msec
			    mch_remove(tmp_fname);
			}
		    }
# endif
		}
	    }
	}

	// Break symlinks and/or hardlinks if we've been asked to.
	אם ((bkc & BKC_BREAKSYMLINK) || (bkc & BKC_BREAKHARDLINK))
	{
# ifdef UNIX
	    صحيح	lstat_res;

	    lstat_res = mch_lstat((حرف *)fname, &st);

	    // Symlinks.
	    אם ((bkc & BKC_BREAKSYMLINK)
		    && lstat_res == 0
		    && st.st_ino != st_old.st_ino)
		backup_copy = FALSE;

	    // Hardlinks.
	    אם ((bkc & BKC_BREAKHARDLINK)
		    && st_old.st_nlink > 1
		    && (lstat_res != 0 || st.st_ino == st_old.st_ino))
		backup_copy = FALSE;
# אחרת
#  אם defined(MSWIN)
	    // Symlinks.
	    אם ((bkc & BKC_BREAKSYMLINK) && mch_is_symbolic_link(fname))
		backup_copy = FALSE;

	    // Hardlinks.
	    אם ((bkc & BKC_BREAKHARDLINK) && mch_is_hard_link(fname))
		backup_copy = FALSE;
#  endif
# endif
	}

#endif

	// make sure we have a valid backup extension to use
	אם (*p_bex == NUL)
	    backup_ext = (char_u *)".bak";
	אחרת
	    backup_ext = p_bex;

	אם (backup_copy
		&& (fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
	{
	    صحيح		bfd;
	    char_u	*copybuf;
	    صحيح		some_error = FALSE;
	    stat_T	st_new;
	    char_u	*dirp;
	    char_u	*rootname;
#אם defined(UNIX)
	    صحيح		did_set_shortname;
	    mode_t	umask_save;
#endif

	    copybuf = alloc(WRITEBUFSIZE + 1);
	    אם (copybuf == عدم)
	    {
		some_error = TRUE;	    // out of memory
		اذهب nobackup;
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
	    כלעוד (*dirp)
	    {
		char_u	*p UNUSED;
		صحيح	copybuf_len UNUSED;

#ifdef UNIX
		st_new.st_ino = 0;
		st_new.st_dev = 0;
		st_new.st_gid = 0;
#endif

		// Isolate one directory name, using an entry in 'bdir'.
		copybuf_len = copy_option_part(&dirp, copybuf, WRITEBUFSIZE, ",");

#אם defined(UNIX) || defined(MSWIN)
		p = copybuf + copybuf_len;
		אם (after_pathsep(copybuf, p) && p[-1] == p[-2])
		    // Ends with '//', use full path
		    אם ((p = make_percent_swname(copybuf, p, fname)) != عدم)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#endif
		rootname = get_file_in_dir(fname, copybuf);
		אם (rootname == عدم)
		{
		    some_error = TRUE;	    // out of memory
		    اذهب nobackup;
		}

#אם defined(UNIX)
		did_set_shortname = FALSE;
#endif

		// May try twice if 'shortname' not set.
		עבור (;;)
		{
		    // Make the backup file name.
		    אם (backup == عدم)
			backup = buf_modname((buf->b_p_sn || buf->b_shortname),
						 rootname, backup_ext, FALSE);
		    אם (backup == عدم)
		    {
			vim_free(rootname);
			some_error = TRUE;		// out of memory
			اذهب nobackup;
		    }

		    // Check if backup file already exists.
		    אם (mch_stat((حرف *)backup, &st_new) >= 0)
		    {
#ifdef UNIX
			// Check if backup file is same as original file.
			// May happen when modname() gave the same file back.
			// E.g. silly link, or file name-length reached.
			// If we don't check here, we either ruin the file
			// when copying or erase it after writing. jw.
			אם (st_new.st_dev == st_old.st_dev
					    && st_new.st_ino == st_old.st_ino)
			{
			    VIM_CLEAR(backup);	// no backup file to delete
			    // may try again with 'shortname' set
			    אם (!(buf->b_shortname || buf->b_p_sn))
			    {
				buf->b_shortname = true;
				did_set_shortname = TRUE;
				استمر;
			    }
				// setting shortname didn't help
			    אם (did_set_shortname)
				buf->b_shortname = false;
			    اكسر;
			}
#endif

			// If we are not going to keep the backup file, don't
			// delete an existing one, try to use another name.
			// Change one character, just before the extension.
			אם (!p_bk)
			{
			    char_u	*wp;

			    wp = backup + STRLEN(backup) - 1
							 - STRLEN(backup_ext);
			    אם (wp < backup)	// empty file name ???
				wp = backup;
			    *wp = 'z';
			    כלעוד (*wp > 'a'
				    && mch_stat((حرف *)backup, &st_new) >= 0)
				--*wp;
			    // They all exist??? Must be something wrong.
			    אם (*wp == 'a')
				VIM_CLEAR(backup);
			}
		    }
		    اكسر;
		}
		vim_free(rootname);

		// Try to create the backup file
		אם (backup != عدم)
		{
		    // remove old backup, if present
		    mch_remove(backup);
		    // Open with O_EXCL to avoid the file being created while
		    // we were sleeping (symlink hacker attack?). Reset umask
		    // if possible to avoid mch_setperm() below.
#ifdef UNIX
		    umask_save = umask(0);
#endif
		    bfd = mch_open((حرف *)backup,
				O_WRONLY|O_CREAT|O_EXTRA|O_EXCL|O_NOFOLLOW,
								 perm & 0777);
#ifdef UNIX
		    (فراغ)umask(umask_save);
#endif
		    אם (bfd < 0)
			VIM_CLEAR(backup);
		    אחרת
		    {
			// Set file protection same as original file, but
			// strip s-bit.  Only needed if umask() wasn't used
			// above.
#ifndef UNIX
			(فراغ)mch_setperm(backup, perm & 0777);
#אחרת
			// Try to set the group of the backup same as the
			// original file. If this fails, set the protection
			// bits for the group same as the protection bits for
			// others.
			אם (st_new.st_gid != st_old.st_gid
# ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
				&& fchown(bfd, (uid_t)-1, st_old.st_gid) != 0
# endif
						)
			    mch_setperm(backup,
					  (perm & 0707) | ((perm & 07) << 3));
# אם defined(HAVE_SELINUX) || defined(HAVE_SMACK)
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
			כלעוד ((write_info.bw_len = read_eintr(fd, copybuf,
							    WRITEBUFSIZE)) > 0)
			{
			    אם (buf_write_bytes(&write_info) == FAIL)
			    {
				errmsg = (char_u *)_(e_cant_write_to_backup_file_add_bang_to_override);
				اكسر;
			    }
			    ui_breakcheck();
			    אם (got_int)
			    {
				errmsg = (char_u *)_(e_interrupted);
				اكسر;
			    }
			}

			אם (close(bfd) < 0 && errmsg == عدم)
			    errmsg = (char_u *)_(e_close_error_for_backup_file_add_bang_to_write_anyway);
			אם (write_info.bw_len < 0)
			    errmsg = (char_u *)_(e_cant_read_file_for_backup_add_bang_to_write_anyway);
#ifdef UNIX
			set_file_time(backup, st_old.st_atime, st_old.st_mtime);
#endif
#ifdef HAVE_ACL
			mch_set_acl(backup, acl);
#endif
#אם defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
#endif
#ifdef FEAT_XATTR
			mch_copy_xattr(fname, backup);
#endif
#ifdef MSWIN
			(فراغ)mch_copy_file_attribute(fname, backup);
#endif
			اكسر;
		    }
		}
	    }
    nobackup:
	    close(fd);		// ignore errors for closing read file
	    vim_free(copybuf);

	    אם (backup == عدم && errmsg == عدم)
		errmsg = (char_u *)_(e_cannot_create_backup_file_add_bang_to_write_anyway);
	    // ignore errors when forceit is TRUE
	    אם ((some_error || errmsg != عدم) && !forceit)
	    {
		retval = FAIL;
		اذهب fail;
	    }
	    errmsg = عدم;
	}
	אחרת
	{
	    char_u	*dirp;
	    char_u	*p;
	    char_u	*rootname;

	    // Make a backup by renaming the original file.

	    // If 'cpoptions' includes the "W" flag, we don't want to
	    // overwrite a read-only file.  But rename may be possible
	    // anyway, thus we need an extra check here.
	    אם (file_readonly && vim_strchr(p_cpo, CPO_FWRITE) != عدم)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_in_cpoptions);
		اذهب fail;
	    }

	    // Form the backup file name - change path/fo.o.h to
	    // path/fo.o.h.bak Try all directories in 'backupdir', first one
	    // that works is used.
	    dirp = p_bdir;
	    כלעוד (*dirp)
	    {
		صحيح IObufflen UNUSED;

		// Isolate one directory name and make the backup file name.
		IObufflen = copy_option_part(&dirp, IObuff, IOSIZE, ",");

#אם defined(UNIX) || defined(MSWIN)
		p = IObuff + IObufflen;
		אם (after_pathsep(IObuff, p) && p[-1] == p[-2])
		    // path ends with '//', use full path
		    אם ((p = make_percent_swname(IObuff, p, fname)) != عدم)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#endif
		אם (backup == عدم)
		{
		    rootname = get_file_in_dir(fname, IObuff);
		    אם (rootname == عدم)
			backup = عدم;
		    אחרת
		    {
			backup = buf_modname(
				(buf->b_p_sn || buf->b_shortname),
						rootname, backup_ext, FALSE);
			vim_free(rootname);
		    }
		}

		אם (backup != عدم)
		{
		    // If we are not going to keep the backup file, don't
		    // delete an existing one, try to use another name.
		    // Change one character, just before the extension.
		    אם (!p_bk && mch_getperm(backup) >= 0)
		    {
			p = backup + STRLEN(backup) - 1 - STRLEN(backup_ext);
			אם (p < backup)	// empty file name ???
			    p = backup;
			*p = 'z';
			כלעוד (*p > 'a' && mch_getperm(backup) >= 0)
			    --*p;
			// They all exist??? Must be something wrong!
			אם (*p == 'a')
			    VIM_CLEAR(backup);
		    }
		}
		אם (backup != عدم)
		{
		    // Delete any existing backup and move the current version
		    // to the backup.	For safety, we don't remove the backup
		    // until the write has finished successfully. And if the
		    // 'backup' option is set, leave it around.

		    // If the renaming of the original file to the backup file
		    // works, quit here.
		    אם (vim_rename(fname, backup) == 0)
			اكسر;

		    VIM_CLEAR(backup);   // don't do the rename below
		}
	    }
	    אם (backup == عدم && !forceit)
	    {
		errmsg = (char_u *)_(e_cant_make_backup_file_add_bang_to_write_anyway);
		اذهب fail;
	    }
	}
    }

#אם defined(UNIX)
    // When using ":w!" and the file was read-only: make it writable
    אם (forceit && perm >= 0 && !(perm & 0200) && st_old.st_uid == getuid()
				     && vim_strchr(p_cpo, CPO_FWRITE) == عدم)
    {
	perm |= 0200;
	(فراغ)mch_setperm(fname, perm);
	made_writable = TRUE;
    }
#endif

    // When using ":w!" and writing to the current file, 'readonly' makes no
    // sense, reset it, unless 'Z' appears in 'cpoptions'.
    אם (forceit && overwriting && vim_strchr(p_cpo, CPO_KEEPRO) == عدم)
    {
	buf->b_p_ro = FALSE;
	need_maketitle = TRUE;	    // set window title later
	status_redraw_all();	    // redraw status lines later
    }

    אם (end > buf->b_ml.ml_line_count)
	end = buf->b_ml.ml_line_count;
    אם (buf->b_ml.ml_flags & ML_EMPTY)
	start = end + 1;

    // If the original file is being overwritten, there is a small chance that
    // we crash in the middle of writing. Therefore the file is preserved now.
    // This makes all block numbers positive so that recovery does not need
    // the original file.
    // Don't do this if there is a backup file and we are exiting.
    אם (reset_changed && !newfile && overwriting
					      && !(exiting && backup != عدم))
    {
	ml_preserve(buf, FALSE);
	אם (got_int)
	{
	    errmsg = (char_u *)_(e_interrupted);
	    اذهب restore_backup;
	}
    }

#ifdef VMS
    vms_remove_version(fname); // remove version
#endif
    // Default: write the file directly.  May write to a temp file for
    // multi-byte conversion.
    wfname = fname;

    // Check for forced 'fileencoding' from "++opt=val" argument.
    אם (eap != عدم && eap->force_enc != 0)
    {
	fenc = eap->cmd + eap->force_enc;
	fenc = enc_canonize(fenc);
	fenc_tofree = fenc;
    }
    אחרת
	fenc = buf->b_p_fenc;

    // Check if the file needs to be converted.
    converted = need_conversion(fenc);

    // Check if UTF-8 to UCS-2/4 or Latin1 conversion needs to be done.  Or
    // Latin1 to Unicode conversion.  This is handled in buf_write_bytes().
    // Prepare the flags for it and allocate bw_conv_buf when needed.
    אם (converted && (enc_utf8 || STRCMP(p_enc, "latin1") == 0))
    {
	wb_flags = get_fio_flags(fenc);
	אם (wb_flags & (FIO_UCS2 | FIO_UCS4 | FIO_UTF16 | FIO_UTF8))
	{
	    // overallocate a bit, in case we read incomplete multi-byte chars
	    صحيح size = bufsize + CONV_RESTLEN;
	    // Need to allocate a buffer to translate into.
	    אם (wb_flags & (FIO_UCS2 | FIO_UTF16 | FIO_UTF8))
		write_info.bw_conv_buflen = size * 2;
	    אחרת // FIO_UCS4
		write_info.bw_conv_buflen = size * 4;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    אם (write_info.bw_conv_buf == عدم)
		end = 0;
	}
    }

#ifdef MSWIN
    אם (converted && wb_flags == 0 && (wb_flags = get_win_fio_flags(fenc)) != 0)
    {
	// Convert UTF-8 -> UCS-2 and UCS-2 -> DBCS.  Worst-case * 4:
	write_info.bw_conv_buflen = bufsize * 4;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	אם (write_info.bw_conv_buf == عدم)
	    end = 0;
    }
#endif

#ifdef MACOS_CONVERT
    אם (converted && wb_flags == 0 && (wb_flags = get_mac_fio_flags(fenc)) != 0)
    {
	write_info.bw_conv_buflen = bufsize * 3;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	אם (write_info.bw_conv_buf == عدم)
	    end = 0;
    }
#endif

#אם defined(FEAT_EVAL) || defined(USE_ICONV)
    אם (converted && wb_flags == 0)
    {
# ifdef USE_ICONV
	// Use iconv() conversion when conversion is needed and it's not done
	// internally.
	write_info.bw_iconv_fd = (iconv_t)my_iconv_open(fenc,
					enc_utf8 ? (char_u *)"utf-8" : p_enc);
	אם (write_info.bw_iconv_fd != (iconv_t)-1)
	{
	    // We're going to use iconv(), allocate a buffer to convert in.
	    write_info.bw_conv_buflen = bufsize * ICONV_MULT;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    אם (write_info.bw_conv_buf == عدم)
		end = 0;
	    write_info.bw_first = TRUE;
	}
#  ifdef FEAT_EVAL
	אחרת
#  endif
# endif

# ifdef FEAT_EVAL
	    // When the file needs to be converted with 'charconvert' after
	    // writing, write to a temp file instead and let the conversion
	    // overwrite the original file.
	    אם (*p_ccv != NUL)
	    {
		wfname = vim_tempname('w', FALSE);
		אם (wfname == عدم)	// Can't write without a tempfile!
		{
		    errmsg = (char_u *)_(e_cant_find_temp_file_for_writing);
		    اذهب restore_backup;
		}
	    }
# endif
    }
#endif
    אם (converted && wb_flags == 0
#ifdef USE_ICONV
	    && write_info.bw_iconv_fd == (iconv_t)-1
#endif
#ifdef FEAT_EVAL
	    && wfname == fname
#endif
	    )
    {
	אם (!forceit)
	{
	    errmsg = (char_u *)_(e_cannot_convert_add_bang_to_write_without_conversion);
	    اذهب restore_backup;
	}
	notconverted = TRUE;
    }

    // If conversion is taking place, we may first pretend to write and check
    // for conversion errors.  Then loop again to write for real.
    // When not doing conversion this writes for real right away.
    עבור (checking_conversion = TRUE; ; checking_conversion = FALSE)
    {
	// There is no need to check conversion when:
	// - there is no conversion
	// - we make a backup file, that can be restored in case of conversion
	//   failure.
	אם (!converted || dobackup)
	    checking_conversion = FALSE;

	אם (checking_conversion)
	{
	    // Make sure we don't write anything.
	    fd = -1;
	    write_info.bw_fd = fd;
	}
	אחרת
	{
#ifdef HAVE_FTRUNCATE
# define TRUNC_ON_OPEN 0
#אחרת
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
	    כלעוד ((fd = mch_open((حرف *)wfname, O_WRONLY | O_EXTRA | (append
				? (forceit ? (O_APPEND | O_CREAT) : O_APPEND)
				: (O_CREAT | TRUNC_ON_OPEN))
				, perm < 0 ? 0666 : (perm & 0777))) < 0)
	    {
		// A forced write will try to create a new file if the old one
		// is still readonly. This may also happen when the directory
		// is read-only. In that case the mch_remove() will fail.
		אם (errmsg == عدم)
		{
#ifdef UNIX
		    stat_T	st;

		    // Don't delete the file when it's a hard or symbolic link.
		    אם ((!newfile && st_old.st_nlink > 1)
			    || (mch_lstat((حرف *)fname, &st) == 0
				&& (st.st_dev != st_old.st_dev
				    || st.st_ino != st_old.st_ino)))
			errmsg =
			      (char_u *)_(e_cant_open_linked_file_for_writing);
		    אחרת
#endif
		    {
			errmsg = (char_u *)_(e_cant_open_file_for_writing);
			אם (forceit && vim_strchr(p_cpo, CPO_FWRITE) == عدم
								  && perm >= 0)
			{
#ifdef UNIX
			    // we write to the file, thus it should be marked
			    // writable after all
			    אם (!(perm & 0200))
				made_writable = TRUE;
			    perm |= 0200;
			    אם (st_old.st_uid != getuid()
						  || st_old.st_gid != getgid())
				perm &= 0777;
#endif
			    אם (!append)  // don't remove when appending
				mch_remove(wfname);
			    استمر;
			}
		    }
		}

restore_backup:
		{
		    stat_T	st;

		    // If we failed to open the file, we don't need a backup.
		    // Throw it away.  If we moved or removed the original file
		    // try to put the backup in its place.
		    אם (backup != عدم && wfname == fname)
		    {
			אם (backup_copy)
			{
			    // There is a small chance that we removed the
			    // original, try to move the copy in its place.
			    // This may not work if the vim_rename() fails.
			    // In that case we leave the copy around.

			    // If file does not exist, put the copy in its
			    // place
			    אם (mch_stat((حرف *)fname, &st) < 0)
				vim_rename(backup, fname);
			    // if original file does exist throw away the copy
			    אם (mch_stat((حرف *)fname, &st) >= 0)
				mch_remove(backup);
			}
			אחרת
			{
			    // try to put the original file back
			    vim_rename(backup, fname);
			}
		    }

		    // if original file no longer exists give an extra warning
		    אם (!newfile && mch_stat((حرف *)fname, &st) < 0)
			end = 0;
		}

		אם (wfname != fname)
		    vim_free(wfname);
		اذهب fail;
	    }
	    write_info.bw_fd = fd;

#אם defined(UNIX)
	    {
		stat_T	st;

		// Double check we are writing the intended file before making
		// any changes.
		אם (overwriting
			&& (!dobackup || backup_copy)
			&& fname == wfname
			&& perm >= 0
			&& mch_fstat(fd, &st) == 0
			&& st.st_ino != st_old.st_ino)
		{
		    close(fd);
		    errmsg = (char_u *)_(e_file_changed_while_writing);
		    اذهب fail;
		}
	    }
#endif
#ifdef HAVE_FTRUNCATE
	    אם (!append)
		vim_ignored = ftruncate(fd, (off_t)0);
#endif

#אם defined(MSWIN)
	    אם (backup != عدم && overwriting && !append)
		(فراغ)mch_copy_file_attribute(backup, wfname);

	    אם (!overwriting && !append)
	    {
		אם (buf->b_ffname != عدم)
		    (فراغ)mch_copy_file_attribute(buf->b_ffname, wfname);
		// Should copy resource fork
	    }
#endif

#ifdef FEAT_CRYPT
	    אם (*buf->b_p_key != NUL && !filtering)
	    {
		char_u		*header;
		صحيح		header_len;

		buf->b_cryptstate = crypt_create_for_writing(
						      crypt_get_method_nr(buf),
					   buf->b_p_key, &header, &header_len);
		אם (buf->b_cryptstate == عدم || header == عدم)
		    end = 0;
		אחרת
		{
		    // Write magic number, so that Vim knows how this file is
		    // encrypted when reading it back.
		    write_info.bw_buf = header;
		    write_info.bw_len = header_len;
		    write_info.bw_flags = FIO_NOCONVERT;
		    אם (buf_write_bytes(&write_info) == FAIL)
			end = 0;
		    wb_flags |= FIO_ENCRYPTED;
		    vim_free(header);
		}
	    }
#endif
	}
	errmsg = عدم;

	write_info.bw_buf = buffer;
	nchars = 0;

	// use "++bin", "++nobin" or 'binary'
	אם (eap != عدم && eap->force_bin != 0)
	    write_bin = (eap->force_bin == FORCE_BIN);
	אחרת
	    write_bin = buf->b_p_bin;

	// The BOM is written just after the encryption magic number.
	// Skip it when appending and the file already existed, the BOM only
	// makes sense at the start of the file.
	אם (buf->b_p_bomb && !write_bin && (!append || perm < 0))
	{
	    write_info.bw_len = make_bom(buffer, fenc);
	    אם (write_info.bw_len > 0)
	    {
		// don't convert, do encryption
		write_info.bw_flags = FIO_NOCONVERT | wb_flags;
		אם (buf_write_bytes(&write_info) == FAIL)
		    end = 0;
		אחרת
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
			    && (buf->b_cryptstate == عدم
				|| crypt_works_inplace(buf->b_cryptstate))
# endif
			    && reset_changed
			    && !checking_conversion);
# ifdef CRYPT_NOT_INPLACE
	// remove undo file if encrypting it is not possible
	אם (buf->b_p_udf
		&& overwriting
		&& !append
		&& !filtering
		&& !checking_conversion
		&& buf->b_cryptstate != عدم
		&& !crypt_works_inplace(buf->b_cryptstate))
	    u_undofile_reset_and_delete(buf);
# endif
	אם (write_undo_file)
	    // Prepare for computing the hash value of the text.
	    sha256_start(&sha_ctx);
#endif

	write_info.bw_len = bufsize;
	write_info.bw_flags = wb_flags;
	fileformat = get_fileformat_force(buf, eap);
	s = buffer;
	len = 0;
	עבור (lnum = start; lnum <= end; ++lnum)
	{
	    // The next while loop is done once for each character written.
	    // Keep it fast!
	    ptr = ml_get_buf(buf, lnum, FALSE) - 1;
#ifdef FEAT_PERSISTENT_UNDO
	    אם (write_undo_file)
		sha256_update(&sha_ctx, ptr + 1,
					      (UINT32_T)(STRLEN(ptr + 1) + 1));
#endif
	    כלעוד ((c = *++ptr) != NUL)
	    {
		אם (c == NL)
		    *s = NUL;		// replace newlines with NULs
		אחרת אם (c == CAR && fileformat == EOL_MAC)
		    *s = NL;		// Mac: replace CRs with NLs
		אחרת
		    *s = c;
		++s;
		אם (++len != bufsize)
		    استمر;
#ifdef FEAT_CRYPT
		אם (write_info.bw_fd > 0 && lnum == end
			&& (write_info.bw_flags & FIO_ENCRYPTED)
			&& *buf->b_p_key != NUL && !filtering
			&& *ptr == NUL)
		    write_info.bw_finish = TRUE;
#endif
		אם (buf_write_bytes(&write_info) == FAIL)
		{
		    end = 0;		// write error: break loop
		    اكسر;
		}
		nchars += bufsize;
		s = buffer;
		len = 0;
		write_info.bw_start_lnum = lnum;
	    }
	    // write failed or last line has no EOL: stop here
	    אם (end == 0
		    || (lnum == end
			&& (write_bin || !buf->b_p_fixeol)
			&& ((write_bin && lnum == buf->b_no_eol_lnum)
			    || (lnum == buf->b_ml.ml_line_count
							   && !buf->b_p_eol))))
	    {
		++lnum;			// written the line, count it
		no_eol = TRUE;
		اكسر;
	    }
	    אם (fileformat == EOL_UNIX)
		*s++ = NL;
	    אחרת
	    {
		*s++ = CAR;		    // EOL_MAC or EOL_DOS: write CR
		אם (fileformat == EOL_DOS)  // write CR-NL
		{
		    אם (++len == bufsize)
		    {
			אם (buf_write_bytes(&write_info) == FAIL)
			{
			    end = 0;	// write error: break loop
			    اكسر;
			}
			nchars += bufsize;
			s = buffer;
			len = 0;
		    }
		    *s++ = NL;
		}
	    }
	    אם (++len == bufsize && end)
	    {
		אם (buf_write_bytes(&write_info) == FAIL)
		{
		    end = 0;		// write error: break loop
		    اكسر;
		}
		nchars += bufsize;
		s = buffer;
		len = 0;

		ui_breakcheck();
		אם (got_int)
		{
		    end = 0;		// Interrupted, break loop
		    اكسر;
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
	    אם (buf->b_fab_rfm == FAB$C_VFC
		    || ((buf->b_fab_rat & (FAB$M_FTN | FAB$M_CR)) != 0))
	    {
		صحيح b2write;

		buf->b_fab_mrs = (buf->b_fab_mrs == 0
			? MIN(4096, bufsize)
			: MIN(buf->b_fab_mrs, bufsize));

		b2write = len;
		כלעוד (b2write > 0)
		{
		    write_info.bw_len = MIN(b2write, buf->b_fab_mrs);
		    אם (buf_write_bytes(&write_info) == FAIL)
		    {
			end = 0;
			اكسر;
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
	אם (len > 0 && end > 0)
	{
	    write_info.bw_len = len;
#ifdef FEAT_CRYPT
	    אם (write_info.bw_fd > 0 && lnum >= end
		    && (write_info.bw_flags & FIO_ENCRYPTED)
		    && *buf->b_p_key != NUL && !filtering)
		write_info.bw_finish = TRUE;
#endif
	    אם (buf_write_bytes(&write_info) == FAIL)
		end = 0;		    // write error
	    nchars += len;
	}

	אם (!buf->b_p_fixeol && buf->b_p_eof)
	{
	    // write trailing CTRL-Z
	    (فراغ)write_eintr(write_info.bw_fd, "\x1a", 1);
	    nchars++;
	}

	// Stop when writing done or an error was encountered.
	אם (!checking_conversion || end == 0)
	    اكسر;

	// If no error happened until now, writing should be ok, so loop to
	// really write the buffer.
    }

    // If we started writing, finish writing. Also when an error was
    // encountered.
    אם (!checking_conversion)
    {
#אם defined(UNIX) && defined(HAVE_FSYNC)
	// On many journaling file systems there is a bug that causes both the
	// original and the backup file to be lost when halting the system
	// right after writing the file.  That's because only the meta-data is
	// journalled.  Syncing the file slows down the system, but assures it
	// has been written to disk and we don't lose it.
	// For a device do try the fsync() but don't complain if it does not
	// work (could be a pipe).
	// If the 'fsync' option is FALSE, don't fsync().  Useful for laptops.
	אם ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && vim_fsync(fd) != 0
		&& !device)
	{
	    errmsg = (char_u *)_(e_fsync_failed);
	    end = 0;
	}
#endif

#אם defined(HAVE_SELINUX) || defined(HAVE_SMACK) || defined(FEAT_XATTR)
	// Probably need to set the security context.
	אם (!backup_copy)
	{
# אם defined(HAVE_SELINUX) || defined(HAVE_SMACK)
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
	אם (backup != عدم && !backup_copy)
	{
# ifdef HAVE_FCHOWN
	    stat_T	st;

	    // Don't change the owner when it's already OK, some systems remove
	    // permission or ACL stuff.
	    אם (mch_stat((حرف *)wfname, &st) < 0
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid)
	    {
		// changing owner might not be possible
		vim_ignored = fchown(fd, st_old.st_uid, -1);
		// if changing group fails clear the group permissions
		אם (fchown(fd, -1, st_old.st_gid) == -1 && perm > 0)
		    perm &= ~070;
	    }
# endif
	    buf_setino(buf);
	}
	אחרת אם (!buf->b_dev_valid)
	    // Set the inode when creating a new file.
	    buf_setino(buf);
#endif

#ifdef UNIX
	אם (made_writable)
	    perm &= ~0200;	// reset 'w' bit for security reasons
#endif
#ifdef HAVE_FCHMOD
	// set permission of new file same as old file
	אם (perm >= 0)
	    (فراغ)mch_fsetperm(fd, perm);
#endif
	אם (close(fd) != 0)
	{
	    errmsg = (char_u *)_(e_close_failed);
	    end = 0;
	}

#ifndef HAVE_FCHMOD
	// set permission of new file same as old file
	אם (perm >= 0)
	    (فراغ)mch_setperm(wfname, perm);
#endif
#ifdef HAVE_ACL
	// Probably need to set the ACL before changing the user (can't set the
	// ACL on a file the user doesn't own).
	// On Solaris, with ZFS and the aclmode property set to "discard" (the
	// default), chmod() discards all part of a file's ACL that don't
	// represent the mode of the file.  It's non-trivial for us to discover
	// whether we're in that situation, so we simply always re-set the ACL.
# ifndef HAVE_SOLARIS_ZFS_ACL
	אם (!backup_copy)
# endif
	    mch_set_acl(wfname, acl);
#endif
#ifdef FEAT_CRYPT
	אם (buf->b_cryptstate != عدم)
	{
	    crypt_free_state(buf->b_cryptstate);
	    buf->b_cryptstate = عدم;
	}
#endif

#אם defined(FEAT_EVAL)
	אם (wfname != fname)
	{
	    // The file was written to a temp file, now it needs to be
	    // converted with 'charconvert' to (overwrite) the output file.
	    אם (end != 0)
	    {
		אם (eval_charconvert(enc_utf8 ? (char_u *)"utf-8" : p_enc,
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

    אם (end == 0)
    {
	// Error encountered.
	אם (errmsg == عدم)
	{
	    אם (write_info.bw_conv_error)
	    {
		אם (write_info.bw_conv_error_lnum == 0)
		    errmsg = (char_u *)_(e_write_error_conversion_failed_make_fenc_empty_to_override);
		אחרת
		{
		    errmsg_allocated = TRUE;
		    errmsg = alloc(300);
		    אם (errmsg == عدم)
			اذهب fail;
		    vim_snprintf((حرف *)errmsg, 300, _(e_write_error_conversion_failed_in_line_nr_make_fenc_empty_to_override),
					 (long)write_info.bw_conv_error_lnum);
		}
	    }
	    אחרת אם (got_int)
		errmsg = (char_u *)_(e_interrupted);
	    אחרת
		errmsg = (char_u *)_(e_write_error_file_system_full);
	}

	// If we have a backup file, try to put it in place of the new file,
	// because the new file is probably corrupt.  This avoids losing the
	// original file when trying to make a backup when writing the file a
	// second time.
	// When "backup_copy" is set we need to copy the backup over the new
	// file.  Otherwise rename the backup file.
	// If this is OK, don't give the extra warning message.
	אם (backup != عدم)
	{
	    אם (backup_copy)
	    {
		// This may take a while, if we were interrupted let the user
		// know we got the message.
		אם (got_int)
		{
		    msg(_(e_interrupted));
		    out_flush();
		}
		אם ((fd = mch_open((حرف *)backup, O_RDONLY | O_EXTRA, 0)) >= 0)
		{
		    אם ((write_info.bw_fd = mch_open((حرف *)fname,
				    O_WRONLY | O_CREAT | O_TRUNC | O_EXTRA,
							   perm & 0777)) >= 0)
		    {
			// copy the file.
			write_info.bw_buf = smallbuf;
			write_info.bw_flags = FIO_NOCONVERT;
			כלעוד ((write_info.bw_len = read_eintr(fd, smallbuf,
						      SMALLBUFSIZE)) > 0)
			    אם (buf_write_bytes(&write_info) == FAIL)
				اكسر;

			אם (close(write_info.bw_fd) >= 0
						   && write_info.bw_len == 0)
			    end = 1;		// success
		    }
		    close(fd);	// ignore errors for closing read file
		}
	    }
	    אחרת
	    {
		אם (vim_rename(backup, fname) == 0)
		    end = 1;
	    }
	}
	اذهب fail;
    }

    lnum -= start;	    // compute number of written lines
    --no_wait_return;	    // may wait for return now

#אם !(defined(UNIX) || defined(VMS))
    fname = sfname;	    // use shortname now, for the messages
#endif
    אם (!filtering)
    {
	msg_add_fname(buf, fname);	// put fname in IObuff with quotes
	c = FALSE;
	אם (write_info.bw_conv_error)
	{
	    STRCAT(IObuff, _(" CONVERSION ERROR"));
	    c = TRUE;
	    אם (write_info.bw_conv_error_lnum != 0)
		vim_snprintf_add((حرف *)IObuff, IOSIZE, _(" in line %ld;"),
			(long)write_info.bw_conv_error_lnum);
	}
	אחרת אם (notconverted)
	{
	    STRCAT(IObuff, _("[NOT converted]"));
	    c = TRUE;
	}
	אחרת אם (converted)
	{
	    STRCAT(IObuff, _("[converted]"));
	    c = TRUE;
	}
	אם (device)
	{
	    STRCAT(IObuff, _("[Device]"));
	    c = TRUE;
	}
	אחרת אם (newfile)
	{
	    STRCAT(IObuff, new_file_message());
	    c = TRUE;
	}
	אם (no_eol)
	{
	    msg_add_eol();
	    c = TRUE;
	}
	// may add [unix/dos/mac]
	אם (msg_add_fileformat(fileformat))
	    c = TRUE;
#ifdef FEAT_CRYPT
	אם (wb_flags & FIO_ENCRYPTED)
	{
	    crypt_append_msg(buf);
	    c = TRUE;
	}
#endif
	msg_add_lines(c, (long)lnum, nchars);	// add line/char count
	אם (!shortmess(SHM_WRITE))
	{
	    אם (append)
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [a]") : _(" appended"));
	    אחרת
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [w]") : _(" written"));
	}

	set_keep_msg((char_u *)msg_trunc_attr((حرف *)IObuff, FALSE, 0), 0);
    }

    // When written everything correctly: reset 'modified'.  Unless not
    // writing to the original file and '+' is not in 'cpoptions'.
    אם (reset_changed && whole && !append
	    && !write_info.bw_conv_error
	    && (overwriting || vim_strchr(p_cpo, CPO_PLUS) != عدم))
    {
	unchanged(buf, TRUE, FALSE);
	// b:changedtick may be incremented in unchanged() but that should not
	// trigger a TextChanged event.
	אם (buf->b_last_changedtick + 1 == CHANGEDTICK(buf))
	    buf->b_last_changedtick = CHANGEDTICK(buf);
	u_unchanged(buf);
	u_update_save_nr(buf);
    }

    // If written to the current file, update the timestamp of the swap file
    // and reset the BF_WRITE_MASK flags. Also sets buf->b_mtime.
    אם (overwriting)
    {
	ml_timestamp(buf);
	אם (append)
	    buf->b_flags &= ~BF_NEW;
	אחרת
	    buf->b_flags &= ~BF_WRITE_MASK;
    }

    // If we kept a backup until now, and we are in patch mode, then we make
    // the backup file our 'original' file.
    אם (*p_pm && dobackup)
    {
	حرف *org = (حرف *)buf_modname((buf->b_p_sn || buf->b_shortname),
							  fname, p_pm, FALSE);

	אם (backup != عدم)
	{
	    stat_T	st;

	    // If the original file does not exist yet
	    // the current backup file becomes the original file
	    אם (org == عدم)
		emsg(_(e_patchmode_cant_save_original_file));
	    אחרת אם (mch_stat(org, &st) < 0)
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
	אחרת
	{
	    صحيح empty_fd;

	    אם (org == عدم
		    || (empty_fd = mch_open(org,
				      O_CREAT | O_EXTRA | O_EXCL | O_NOFOLLOW,
					perm < 0 ? 0666 : (perm & 0777))) < 0)
	      emsg(_(e_patchmode_cant_touch_empty_original_file));
	    אחרת
	      close(empty_fd);
	}
	אם (org != عدم)
	{
	    mch_setperm((char_u *)org, mch_getperm(fname) & 0777);
	    vim_free(org);
	}
    }

    // Remove the backup unless 'backup' option is set or there was a
    // conversion error.
    אם (!p_bk && backup != عدم && !write_info.bw_conv_error
	    && mch_remove(backup) != 0)
	emsg(_(e_cant_delete_backup_file));

    اذهب nofail;

    // Finish up.  We get here either after failure or success.
fail:
    --no_wait_return;		// may wait for return now
nofail:

    // Done saving, we accept changed buffer warnings again
    buf->b_saving = false;

    vim_free(backup);
    אם (buffer != smallbuf)
	vim_free(buffer);
    vim_free(fenc_tofree);
    vim_free(write_info.bw_conv_buf);
#ifdef USE_ICONV
    אם (write_info.bw_iconv_fd != (iconv_t)-1)
    {
	iconv_close(write_info.bw_iconv_fd);
	write_info.bw_iconv_fd = (iconv_t)-1;
    }
#endif
#ifdef HAVE_ACL
    mch_free_acl(acl);
#endif

    אם (errmsg != عدم)
    {
	صحيح numlen = errnum != عدم ? (صحيح)STRLEN(errnum) : 0;

	attr = HL_ATTR(HLF_E);	// set highlight for error messages
	msg_add_fname(buf,
#ifndef UNIX
		sfname
#אחרת
		fname
#endif
		     );		// put file name in IObuff with quotes
	אם (STRLEN(IObuff) + STRLEN(errmsg) + numlen >= IOSIZE)
	    IObuff[IOSIZE - STRLEN(errmsg) - numlen - 1] = NUL;
	// If the error message has the form "is ...", put the error number in
	// front of the file name.
	אם (errnum != عدم)
	{
	    STRMOVE(IObuff + numlen, IObuff);
	    mch_memmove(IObuff, errnum, (size_t)numlen);
	}
	STRCAT(IObuff, errmsg);
	emsg((حرف *)IObuff);
	אם (errmsg_allocated)
	    vim_free(errmsg);

	retval = FAIL;
	אם (end == 0)
	{
	    msg_puts_attr(_("\nWARNING: Original file may be lost or damaged\n"),
		    attr | MSG_HIST);
	    msg_puts_attr(_("don't quit the editor until the file is successfully written!"),
		    attr | MSG_HIST);

	    // Update the timestamp to avoid an "overwrite changed file"
	    // prompt when writing again.
	    אם (mch_stat((حرف *)fname, &st_old) >= 0)
	    {
		buf_store_time(buf, &st_old, fname);
		buf->b_mtime_read = buf->b_mtime;
		buf->b_mtime_read_ns = buf->b_mtime_ns;
	    }
	}
    }
    msg_scroll = msg_save;

#ifdef FEAT_PERSISTENT_UNDO
    // When writing the whole file and 'undofile' is set, also write the undo
    // file.
    אם (retval == OK && write_undo_file)
    {
	char_u	    hash[UNDO_HASH_SIZE];

	sha256_finish(&sha_ctx, hash);
	u_write_undo(عدم, FALSE, buf, hash);
    }
#endif

#ifdef FEAT_EVAL
    אם (!should_abort(retval))
#אחרת
    אם (!got_int)
#endif
    {
	aco_save_T	aco;

	curbuf->b_no_eol_lnum = 0;  // in case it was set by the previous read

	// Apply POST autocommands.
	// Careful: The autocommands may call buf_write() recursively!
	// Only do this when a window was found for "buf".
	aucmd_prepbuf(&aco, buf);
	אם (curbuf == buf)
	{
	    אם (append)
		apply_autocmds_exarg(EVENT_FILEAPPENDPOST, fname, fname,
							   FALSE, curbuf, eap);
	    אחרת אם (filtering)
		apply_autocmds_exarg(EVENT_FILTERWRITEPOST, عدم, fname,
							   FALSE, curbuf, eap);
	    אחרת אם (reset_changed && whole)
		apply_autocmds_exarg(EVENT_BUFWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);
	    אחרת
		apply_autocmds_exarg(EVENT_FILEWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}

#ifdef FEAT_EVAL
	אם (aborting())	    // autocmds may abort script processing
	    retval = FALSE;
#endif
    }

#ifdef FEAT_VIMINFO
    // Make sure marks will be written out to the viminfo file later, even when
    // the file is new.
    curbuf->b_marks_read = true;
#endif

    got_int |= prev_got_int;

    החזר retval;
}
