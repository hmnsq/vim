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
#اگر_ختم

#تعریف_کرو SMALLBUFSIZE	256	// size of emergency write buffer

/*
 * Structure to pass arguments from buf_write() to buf_write_bytes().
 */
ساخت bw_info
{
    صحیح		bw_fd;		// file descriptor
    char_u	*bw_buf;	// buffer with data to be written
    صحیح		bw_len;		// length of data
    صحیح		bw_flags;	// FIO_ flags
#اگر_تعریف_شدہ FEAT_CRYPT
    buf_T	*bw_بفر;	// buffer being written
    صحیح		bw_اختتام;	// finish encrypting
#اگر_ختم
    char_u	bw_rest[CONV_RESTLEN]; // not converted bytes
    صحیح		bw_restlen;	// nr of bytes in bw_rest[]
    صحیح		bw_پہلا;	// first write call
    char_u	*bw_conv_buf;	// buffer for writing converted chars
    size_t	bw_conv_buflen; // size of bw_conv_buf
    صحیح		bw_conv_error;	// set for conversion error
    linenr_T	bw_conv_error_lnum;  // first line with error or zero
    linenr_T	bw_آغاز_lnum;	// line number at start of buffer
#اگر_تعریف_شدہ USE_ICONV
    iconv_t	bw_iconv_fd;	// descriptor for iconv() or -1
#اگر_ختم
};

/*
 * Convert a Unicode character to bytes.
 * Return TRUE for an error, FALSE when it's OK.
 */
    ساکن صحیح
ucs2bytes(
    غیرعلامتی	c,		// in: character
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
	ورنہ
	{
	    *p++ = (c >> 24);
	    *p++ = (c >> 16);
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    ورنہ اگر (flags & (FIO_UCS2 | FIO_UTF16))
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
		    *p++ = ((غیرعلامتی)cc >> 8);
		}
		ورنہ
		{
		    *p++ = ((غیرعلامتی)cc >> 8);
		    *p++ = cc;
		}
		c = (c & 0x3ff) + 0xdc00;
	    }
	    ورنہ
		error = TRUE;
	}
	اگر (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	}
	ورنہ
	{
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    ورنہ    // Latin1
    {
	اگر (c >= 0x100)
	{
	    error = TRUE;
	    *p++ = 0xBF;
	}
	ورنہ
	    *p++ = c;
    }

    *pp = p;
    واپس error;
}

/*
 * Call write() to write a number of bytes to the file.
 * Handles encryption and 'encoding' conversion.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ساکن صحیح
buf_write_بائٹیں(ساخت bw_info *ip)
{
    صحیح		wlen;
    char_u	*buf = ip->bw_buf;	// data to write
    صحیح		len = ip->bw_len;	// length of data
    صحیح		flags = ip->bw_flags;	// extra flags

    // Skip conversion when writing the crypt magic number or the BOM.
    اگر (!(flags & FIO_NOCONVERT))
    {
	char_u		*p;
	غیرعلامتی	c;
	صحیح		n;

	اگر (flags & FIO_UTF8)
	{
	    // Convert latin1 in the buffer to UTF-8 in the file.
	    p = ip->bw_conv_buf;	// translate to buffer
	    برائے (wlen = 0; wlen < len; ++wlen)
		p += utf_char2bytes(buf[wlen], p);
	    buf = ip->bw_conv_buf;
	    len = (صحیح)(p - ip->bw_conv_buf);
	}
	ورنہ اگر (flags & (FIO_UCS4 | FIO_UTF16 | FIO_UCS2 | FIO_LATIN1))
	{
	    // Convert UTF-8 bytes in the buffer to UCS-2, UCS-4, UTF-16 or
	    // Latin1 chars in the file.
	    اگر (flags & FIO_LATIN1)
		p = buf;	// translate in-place (can only get shorter)
	    ورنہ
		p = ip->bw_conv_buf;	// translate to buffer
	    برائے (wlen = 0; wlen < len; wlen += n)
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
			    واپس FAIL;
			ip->bw_restlen += len;
			توڑو;
		    }
		    اگر (n > 1)
			c = utf_ptr2char(ip->bw_rest);
		    ورنہ
			c = ip->bw_rest[0];
		    اگر (n >= ip->bw_restlen)
		    {
			n -= ip->bw_restlen;
			ip->bw_restlen = 0;
		    }
		    ورنہ
		    {
			ip->bw_restlen -= n;
			mch_memmove(ip->bw_rest, ip->bw_rest + n,
						      (size_t)ip->bw_restlen);
			n = 0;
		    }
		}
		ورنہ
		{
		    n = utf_ptr2len_len(buf + wlen, len - wlen);
		    اگر (n > len - wlen)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			اگر (len - wlen > CONV_RESTLEN)
			    واپس FAIL;
			ip->bw_restlen = len - wlen;
			mch_memmove(ip->bw_rest, buf + wlen,
						      (size_t)ip->bw_restlen);
			توڑو;
		    }
		    اگر (n > 1)
			c = utf_ptr2char(buf + wlen);
		    ورنہ
			c = buf[wlen];
		}
		// Check that there is enough space
		اگر (!(flags & FIO_LATIN1))
		{
		    size_t need = (flags & FIO_UCS4) ? 4 : 2;
		    اگر ((flags & FIO_UTF16) && c >= 0x10000)
			need = 4;

		    اگر ((size_t)(p - ip->bw_conv_buf) + need > ip->bw_conv_buflen)
			واپس FAIL;
		}

		اگر (ucs2bytes(c, &p, flags) && !ip->bw_conv_error)
		{
		    ip->bw_conv_error = TRUE;
		    ip->bw_conv_error_lnum = ip->bw_آغاز_lnum;
		}
		اگر (c == NL)
		    ++ip->bw_آغاز_lnum;
	    }
	    اگر (flags & FIO_LATIN1)
		len = (صحیح)(p - buf);
	    ورنہ
	    {
		buf = ip->bw_conv_buf;
		len = (صحیح)(p - ip->bw_conv_buf);
	    }
	}

#اگر_تعریف_شدہ MSWIN
	ورنہ اگر (flags & FIO_CODEPAGE)
	{
	    // Convert UTF-8 or codepage to UCS-2 and then to MS-Windows
	    // codepage.
	    char_u	*from;
	    size_t	fromlen;
	    char_u	*مقصد;
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
	    ورنہ
	    {
		from = buf;
		fromlen = len;
	    }

	    مقصد = ip->bw_conv_buf;
	    اگر (enc_utf8)
	    {
		// Convert from UTF-8 to UCS-2, to the start of the buffer.
		// The buffer has been allocated to be big enough.
		جب (fromlen > 0)
		{
		    n = (صحیح)utf_ptr2len_len(from, (صحیح)fromlen);
		    اگر (n > (صحیح)fromlen)	// incomplete byte sequence
			توڑو;
		    u8c = utf_ptr2char(from);
		    *مقصد++ = (u8c & 0xff);
		    *مقصد++ = (u8c >> 8);
		    fromlen -= n;
		    from += n;
		}

		// Copy remainder to ip->bw_rest[] to be used for the next
		// call.
		اگر (fromlen > CONV_RESTLEN)
		{
		    // weird overlong sequence
		    ip->bw_conv_error = TRUE;
		    واپس FAIL;
		}
		mch_memmove(ip->bw_rest, from, fromlen);
		ip->bw_restlen = (صحیح)fromlen;
	    }
	    ورنہ
	    {
		// Convert from enc_codepage to UCS-2, to the start of the
		// buffer.  The buffer has been allocated to be big enough.
		ip->bw_restlen = 0;
		needed = Multi_Byte_مقصد_Wide_Char(enc_codepage,
			     MB_ERR_INVALID_CHARS, (LPCSTR)from, (صحیح)fromlen,
								     تہی, 0);
		اگر (needed == 0)
		{
		    // When conversion fails there may be a trailing byte.
		    needed = Multi_Byte_مقصد_Wide_Char(enc_codepage,
			 MB_ERR_INVALID_CHARS, (LPCSTR)from, (صحیح)fromlen - 1,
								     تہی, 0);
		    اگر (needed == 0)
		    {
			// Conversion doesn't work.
			ip->bw_conv_error = TRUE;
			واپس FAIL;
		    }
		    // Save the trailing byte for the next call.
		    ip->bw_rest[0] = from[fromlen - 1];
		    ip->bw_restlen = 1;
		}
		needed = Multi_Byte_مقصد_Wide_Char(enc_codepage, MB_ERR_INVALID_CHARS,
				(LPCSTR)from, (صحیح)(fromlen - ip->bw_restlen),
							  (LPWSTR)مقصد, needed);
		اگر (needed == 0)
		{
		    // Safety check: Conversion doesn't work.
		    ip->bw_conv_error = TRUE;
		    واپس FAIL;
		}
		مقصد += needed * 2;
	    }

	    fromlen = مقصد - ip->bw_conv_buf;
	    buf = مقصد;
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	    اگر (FIO_GET_CP(flags) == CP_UTF8)
	    {
		// Convert from UCS-2 to UTF-8, using the remainder of the
		// conversion buffer.  Fails when out of space.
		برائے (from = ip->bw_conv_buf; fromlen > 1; fromlen -= 2)
		{
		    u8c = *from++;
		    u8c += (*from++ << 8);
		    مقصد += utf_char2bytes(u8c, مقصد);
		    اگر (مقصد + 6 >= ip->bw_conv_buf + ip->bw_conv_buflen)
		    {
			ip->bw_conv_error = TRUE;
			واپس FAIL;
		    }
		}
		len = (صحیح)(مقصد - buf);
	    }
	    ورنہ
# endif
	    {
		// Convert from UCS-2 to the codepage, using the remainder of
		// the conversion buffer.  If the conversion uses the default
		// character "0", the data doesn't fit in this encoding, so
		// fail.
		len = Wide_Char_مقصد_Multi_Byte(FIO_GET_CP(flags), 0,
			(LPCWSTR)ip->bw_conv_buf, (صحیح)fromlen / حجم(WCHAR),
			(LPSTR)مقصد, (صحیح)(ip->bw_conv_buflen - fromlen), 0,
									&bad);
		اگر (bad)
		{
		    ip->bw_conv_error = TRUE;
		    واپس FAIL;
		}
	    }
	}
#اگر_ختم

#اگر_تعریف_شدہ MACOS_CONVERT
	ورنہ اگر (flags & FIO_MACROMAN)
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
	    ورنہ
	    {
		from = buf;
		fromlen = len;
	    }

	    اگر (enc2macroman(from, fromlen,
			ip->bw_conv_buf, &len, ip->bw_conv_buflen,
			ip->bw_rest, &ip->bw_restlen) == FAIL)
	    {
		ip->bw_conv_error = TRUE;
		واپس FAIL;
	    }
	    buf = ip->bw_conv_buf;
	}
#اگر_ختم

#اگر_تعریف_شدہ USE_ICONV
	اگر (ip->bw_iconv_fd != (iconv_t)-1)
	{
	    ثابت حرف	*from;
	    size_t	fromlen;
	    حرف	*مقصد;
	    size_t	tolen;

	    // Convert with iconv().
	    اگر (ip->bw_restlen > 0)
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
	    ورنہ
	    {
		from = (ثابت حرف *)buf;
		fromlen = len;
		tolen = ip->bw_conv_buflen;
	    }
	    مقصد = (حرف *)ip->bw_conv_buf;

	    اگر (ip->bw_پہلا)
	    {
		size_t	save_len = tolen;

		// output the initial shift state sequence
		(خالی)iconv(ip->bw_iconv_fd, تہی, تہی, &مقصد, &tolen);

		// There is a bug in iconv() on Linux (which appears to be
		// wide-spread) which sets "to" to NULL and messes up "tolen".
		اگر (مقصد == تہی)
		{
		    مقصد = (حرف *)ip->bw_conv_buf;
		    tolen = save_len;
		}
		ip->bw_پہلا = FALSE;
	    }

	    // If iconv() has an error or there is not enough room, fail.
	    اگر ((iconv(ip->bw_iconv_fd, (خالی *)&from, &fromlen, &مقصد, &tolen)
			== (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						    || fromlen > CONV_RESTLEN)
	    {
		ip->bw_conv_error = TRUE;
		واپس FAIL;
	    }

	    // copy remainder to ip->bw_rest[] to be used for the next call.
	    اگر (fromlen > 0)
		mch_memmove(ip->bw_rest, (خالی *)from, fromlen);
	    ip->bw_restlen = (صحیح)fromlen;

	    buf = ip->bw_conv_buf;
	    len = (صحیح)((char_u *)مقصد - ip->bw_conv_buf);
	}
#اگر_ختم
    }

    اگر (ip->bw_fd < 0)
	// Only checking conversion, which is OK if we get here.
	واپس OK;

#اگر_تعریف_شدہ FEAT_CRYPT
    اگر (flags & FIO_ENCRYPTED)
    {
	// Encrypt the data. Do it in-place if possible, otherwise use an
	// allocated buffer.
# ifdef CRYPT_NOT_INPLACE
	اگر (crypt_چلتا_inplace(ip->bw_بفر->b_cryptstate))
	{
# endif
	    crypt_encode_inplace(ip->bw_بفر->b_cryptstate, buf, len,
								ip->bw_اختتام);
# ifdef CRYPT_NOT_INPLACE
	}
	ورنہ
	{
	    char_u *outbuf;

	    len = crypt_encode_alloc(curbuf->b_cryptstate, buf, len, &outbuf,
								ip->bw_اختتام);
	    اگر (len == 0)
		واپس OK;  // Crypt layer is buffering, will flush later.
	    wlen = write_eintr(ip->bw_fd, outbuf, len);
	    vim_free(outbuf);
	    واپس (wlen < len) ? FAIL : OK;
	}
# endif
    }
#اگر_ختم

    wlen = write_eintr(ip->bw_fd, buf, len);
    واپس (wlen < len) ? FAIL : OK;
}

/*
 * Check modification time of file, before writing to it.
 * The size isn't checked, because using a tool like "gzip" takes care of
 * using the same timestamp but can't set the size.
 */
    ساکن صحیح
check_mtime(buf_T *buf, stat_T *st)
{
    اگر (buf->b_mtime_read != 0
		  && وقت_differs(st, buf->b_mtime_read, buf->b_mtime_read_ns))
    {
	پیغام_scroll = TRUE;	    // don't overwrite messages here
	پیغام_silent = 0;		    // must give this prompt
	// don't use emsg() here, don't want to flush the buffers
	پیغام_attr(_("WARNING: The file has been changed since reading it!!!"),
						       HL_ATTR(HLF_E));
	اگر (ask_yesno((char_u *)_("Do you really want to write to it"),
								 TRUE) == 'n')
	    واپس FAIL;
	پیغام_scroll = FALSE;	    // always overwrite the file message now
    }
    واپس OK;
}

/*
 * Generate a BOM in "buf[4]" for encoding "name".
 * Return the length of the BOM (zero when no BOM).
 */
    ساکن صحیح
make_bom(char_u *buf, char_u *name)
{
    صحیح		flags;
    char_u	*p;

    flags = get_fio_flags(name);

    // Can't put a BOM in a non-Unicode file.
    اگر (flags == FIO_LATIN1 || flags == 0)
	واپس 0;

    اگر (flags == FIO_UTF8)	// UTF-8
    {
	buf[0] = 0xef;
	buf[1] = 0xbb;
	buf[2] = 0xbf;
	واپس 3;
    }
    p = buf;
    (خالی)ucs2bytes(0xfeff, &p, flags);
    واپس (صحیح)(p - buf);
}

#اگر_تعریف_شدہ UNIX
    ساکن خالی
سیٹ_file_وقت(
    char_u  *fname,
    time_t  atime,	    // access time
    time_t  mtime)	    // modification time
{
# اگر defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
    ساخت utimbuf  buf;

    buf.actime	= atime;
    buf.modtime	= mtime;
    (خالی)utime((حرف *)fname, &buf);
# ورنہ
#  اگر defined(HAVE_UTIMES)
    ساخت timeval  tvp[2];

    tvp[0].tv_sec   = atime;
    tvp[0].tv_usec  = 0;
    tvp[1].tv_sec   = mtime;
    tvp[1].tv_usec  = 0;
    (خالی)utimes((حرف *)fname, (ثابت ساخت timeval *)&tvp);
#  endif
# endif
}
#اگر_ختم // UNIX

    حرف *
new_file_پیغام(خالی)
{
    واپس shortmess(SHM_NEW) ? _("[New]") : _("[New File]");
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
    linenr_T	    آغاز,
    linenr_T	    انتہا,
    exarg_T	    *eap,		// for forced 'ff' and 'fenc', can be
					// NULL!
    صحیح		    append,		// append to the file
    صحیح		    forceit,
    صحیح		    reset_بدلا,
    صحیح		    filtering)
{
    صحیح		    fd;
    char_u	    *backup = تہی;
    صحیح		    backup_copy = FALSE; // copy the original file?
    صحیح		    dobackup;
    char_u	    *ffname;
    char_u	    *wfname = تہی;	// name of file to write to
    char_u	    *s;
    char_u	    *ptr;
    char_u	    c;
    صحیح		    len;
    linenr_T	    lnum;
    طویل	    nchars;
    char_u	    *errmsg = تہی;
    صحیح		    errmsg_allocated = FALSE;
    char_u	    *errnum = تہی;
    char_u	    *بفر;
    char_u	    smallbuf[SMALLBUFSIZE];
    char_u	    *backup_ext;
    صحیح		    bufsize;
    طویل	    perm;		    // file permissions
    صحیح		    retval = OK;
    صحیح		    newfile = FALSE;	    // TRUE if file doesn't exist yet
    صحیح		    پیغام_save = پیغام_scroll;
    صحیح		    overwriting;	    // TRUE if writing over original
    صحیح		    no_eol = FALSE;	    // no end-of-line written
    صحیح		    device = FALSE;	    // writing to a device
    stat_T	    st_old;
    صحیح		    پچھلا_got_int = got_int;
    صحیح		    checking_conversion;
    صحیح		    file_readonly = FALSE;  // overwritten file is read-only
#اگر defined(UNIX)			    // XXX fix me sometime?
    صحیح		    made_writable = FALSE;  // 'w' bit has been set
#اگر_ختم
					// writing everything
    صحیح		    پورا = (آغاز == 1 && انتہا == buf->b_ml.ml_سطر_گنتی);
    linenr_T	    old_سطر_گنتی = buf->b_ml.ml_سطر_گنتی;
    صحیح		    attr;
    صحیح		    fileformat;
    صحیح		    write_bin;
    ساخت bw_info  write_info;		// info for buf_write_bytes()
    صحیح		    converted = FALSE;
    صحیح		    notconverted = FALSE;
    char_u	    *fenc;		// effective 'fileencoding'
    char_u	    *fenc_tofree = تہی; // allocated "fenc"
    صحیح		    wb_flags = 0;
#اگر_تعریف_شدہ HAVE_ACL
    vim_acl_T	    acl = تہی;		// ACL copied from original file to
					// backup or new file
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
    صحیح		    write_undo_file = FALSE;
    context_sha256_T sha_ctx;
#اگر_ختم
    غیرعلامتی صحیح    bkc = get_bkc_flags(buf);
    pos_T	    orig_آغاز = buf->b_op_آغاز;
    pos_T	    orig_انتہا = buf->b_op_انتہا;

    اگر (fname == تہی || *fname == NUL)	// safety check
	واپس FAIL;
    اگر (buf->b_ml.ml_mfp == تہی)
    {
	// This can happen during startup when there is a stray "w" in the
	// vimrc file.
	emsg(_(e_خالی_خانے_بفر));
	واپس FAIL;
    }

    // Disallow writing from .exrc and .vimrc in current directory for
    // security reasons.
    اگر (check_secure())
	واپس FAIL;

    // Avoid a crash for a long name.
    اگر (STRLEN(fname) >= MAXPATHL)
    {
	emsg(_(e_name_too_long));
	واپس FAIL;
    }

    // must init bw_conv_buf and bw_iconv_fd before jumping to "fail"
    write_info.bw_conv_buf = تہی;
    write_info.bw_conv_error = FALSE;
    write_info.bw_conv_error_lnum = 0;
    write_info.bw_restlen = 0;
#اگر_تعریف_شدہ USE_ICONV
    write_info.bw_iconv_fd = (iconv_t)-1;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CRYPT
    write_info.bw_بفر = buf;
    write_info.bw_اختتام = FALSE;
#اگر_ختم

    // After writing a file changedtick changes but we don't want to display
    // the line.
    ex_no_reprint = TRUE;

    // If there is no file name yet, use the one for the written file.
    // BF_NOTEDITED is set to reflect this (in case the write fails).
    // Don't do this when the write is for a filter command.
    // Don't do this when appending.
    // Only do this when 'cpoptions' contains the 'F' flag.
    اگر (buf->b_ffname == تہی
	    && reset_بدلا
	    && پورا
	    && buf == curbuf
	    && !bt_nofilename(buf)
	    && !filtering
	    && (!append || vim_strchr(p_cpo, CPO_FNAMEAPP) != تہی)
	    && vim_strchr(p_cpo, CPO_FNAMEW) != تہی)
    {
	اگر (سیٹ_rw_fname(fname, sfname) == FAIL)
	    واپس FAIL;
	buf = curbuf;	    // just in case autocmds made "buf" invalid
    }

    اگر (sfname == تہی)
	sfname = fname;
    // For Unix: Use the short file name whenever possible.
    // Avoids problems with networks and when directory names are changed.
    // Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
    // another directory, which we don't detect
    ffname = fname;			    // remember full fname
#اگر_تعریف_شدہ UNIX
    fname = sfname;
#اگر_ختم

    اگر (buf->b_ffname != تہی && fnamecmp(ffname, buf->b_ffname) == 0)
	overwriting = TRUE;
    ورنہ
	overwriting = FALSE;

    اگر (exiting)
	settmode(TMODE_COOK);	    // when exiting allow typeahead now

    ++no_wait_return;		    // don't wait for return yet

    // Set '[ and '] marks to the lines to be written.
    buf->b_op_آغاز.lnum = آغاز;
    buf->b_op_آغاز.ستون = 0;
    buf->b_op_انتہا.lnum = انتہا;
    buf->b_op_انتہا.ستون = 0;

    {
	aco_save_T	aco;
	صحیح		buf_ffname = FALSE;
	صحیح		buf_sfname = FALSE;
	صحیح		buf_fname_f = FALSE;
	صحیح		buf_fname_s = FALSE;
	صحیح		did_cmd = FALSE;
	صحیح		nofile_err = FALSE;
	صحیح		خالی_خانے_memline = (buf->b_ml.ml_mfp == تہی);
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
	    واپس FAIL;
	}

	سیٹ_bufref(&bufref, buf);

	اگر (append)
	{
	    اگر (!(did_cmd = apply_autocmds_exarg(EVENT_FILEAPPENDCMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		اگر (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		ورنہ
		    apply_autocmds_exarg(EVENT_FILEAPPENDPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	ورنہ اگر (filtering)
	{
	    apply_autocmds_exarg(EVENT_FILTERWRITEPRE,
					    تہی, sfname, FALSE, curbuf, eap);
	}
	ورنہ اگر (reset_بدلا && پورا)
	{
	    صحیح was_بدلا = curbuf_Is_بدلا();

	    did_cmd = apply_autocmds_exarg(EVENT_BUFWRITECMD,
					  sfname, sfname, FALSE, curbuf, eap);
	    اگر (did_cmd)
	    {
		اگر (was_بدلا && !curbuf_Is_بدلا())
		{
		    // Written everything correctly and BufWriteCmd has reset
		    // 'modified': Correct the undo information so that an
		    // undo now sets 'modified'.
		    u_unchanged(curbuf);
		    u_update_save_nr(curbuf);
		}
	    }
	    ورنہ
	    {
		اگر (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		ورنہ
		    apply_autocmds_exarg(EVENT_BUFWRITEPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	ورنہ
	{
	    اگر (!(did_cmd = apply_autocmds_exarg(EVENT_FILEWRITECMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		اگر (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		ورنہ
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
	    buf = تہی;
	اگر (buf == تہی || (buf->b_ml.ml_mfp == تہی && !خالی_خانے_memline)
				       || did_cmd || nofile_err
#اگر_تعریف_شدہ FEAT_EVAL
				       || aborting()
#اگر_ختم
				       )
	{
	    اگر (buf != تہی && (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	    {
		// restore the original '[ and '] positions
		buf->b_op_آغاز = orig_آغاز;
		buf->b_op_انتہا = orig_انتہا;
	    }

	    --no_wait_return;
	    پیغام_scroll = پیغام_save;
	    اگر (nofile_err)
		semsg(_(e_no_matching_autocommands_for_buftype_str_بفر),
							       curbuf->b_p_bt);

	    اگر (nofile_err
#اگر_تعریف_شدہ FEAT_EVAL
		    || aborting()
#اگر_ختم
		    )
		// An aborting error, interrupt or exception in the
		// autocommands.
		واپس FAIL;
	    اگر (did_cmd)
	    {
		اگر (buf == تہی)
		    // The buffer was deleted.  We assume it was written
		    // (can't retry anyway).
		    واپس OK;
		اگر (overwriting)
		{
		    // Assume the buffer was written, update the timestamp.
		    ml_timestamp(buf);
		    اگر (append)
			buf->b_flags &= ~BF_NEW;
		    ورنہ
			buf->b_flags &= ~BF_WRITE_نقاب;
		}
		اگر (reset_بدلا && buf->b_بدلا && !append
			&& (overwriting || vim_strchr(p_cpo, CPO_PLUS) != تہی))
		    // Buffer still changed, the autocommands didn't work
		    // properly.
		    واپس FAIL;
		واپس OK;
	    }
#اگر_تعریف_شدہ FEAT_EVAL
	    اگر (!aborting())
#اگر_ختم
		emsg(_(e_autocommands_deleted_or_unloaded_بفر_مقصد_be_written));
	    واپس FAIL;
	}

	// The autocommands may have changed the number of lines in the file.
	// When writing the whole file, adjust the end.
	// When writing part of the file, assume that the autocommands only
	// changed the number of lines that are to be written (tricky!).
	اگر (buf->b_ml.ml_سطر_گنتی != old_سطر_گنتی)
	{
	    اگر (پورا)						// write all
		انتہا = buf->b_ml.ml_سطر_گنتی;
	    ورنہ اگر (buf->b_ml.ml_سطر_گنتی > old_سطر_گنتی)	// more lines
		انتہا += buf->b_ml.ml_سطر_گنتی - old_سطر_گنتی;
	    ورنہ						// less lines
	    {
		انتہا -= old_سطر_گنتی - buf->b_ml.ml_سطر_گنتی;
		اگر (انتہا < آغاز)
		{
		    --no_wait_return;
		    پیغام_scroll = پیغام_save;
		    emsg(_(e_autocommands_بدلا_نمبر_کا_سطریں_اندر_unexpected_way));
		    واپس FAIL;
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
	buf->b_op_آغاز = orig_آغاز;
	buf->b_op_انتہا = orig_انتہا;
    }

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    اگر (netbeans_فعال() && is_Netbeans_بفر(buf))
    {
	اگر (پورا)
	{
	    // b_changed can be 0 after an undo, but we still need to write
	    // the buffer to NetBeans.
	    اگر (buf->b_بدلا || isNetbeansModified(buf))
	    {
		--no_wait_return;		// may wait for return now
		پیغام_scroll = پیغام_save;
		netbeans_save_بفر(buf);	// no error checking...
		واپس retval;
	    }
	    ورنہ
	    {
		errnum = (char_u *)"E656: ";
		errmsg = (char_u *)_(e_netbeans_disallows_writes_کا_unmodified_buffers);
		بفر = تہی;
		جاؤ fail;
	    }
	}
	ورنہ
	{
	    errnum = (char_u *)"E657: ";
	    errmsg = (char_u *)_(e_partial_writes_disallowed_for_netbeans_buffers);
	    بفر = تہی;
	    جاؤ fail;
	}
    }
#اگر_ختم

    اگر (shortmess(SHM_OVER) && !exiting)
	پیغام_scroll = FALSE;	    // overwrite previous file message
    ورنہ
	پیغام_scroll = TRUE;	    // don't overwrite previous file message
    اگر (!filtering)
	filemess(buf,
#اگر_غیر_تعریف_شدہ UNIX
		sfname,
#ورنہ
		fname,
#اگر_ختم
		    (char_u *)"", 0);	// show that we are busy
    پیغام_scroll = FALSE;		    // always overwrite the file message now

    بفر = alloc(WRITEBUFSIZE);
    اگر (بفر == تہی)		    // can't allocate big buffer, use small
				    // one (to be able to write when out of
				    // memory)
    {
	بفر = smallbuf;
	bufsize = SMALLBUFSIZE;
    }
    ورنہ
	bufsize = WRITEBUFSIZE;

    // Get information about original file (if there is one).
#اگر defined(UNIX)
    st_old.st_dev = 0;
    st_old.st_ino = 0;
    perm = -1;
    اگر (mch_stat((حرف *)fname, &st_old) < 0)
	newfile = TRUE;
    ورنہ
    {
	perm = st_old.st_منوال;
	اگر (!S_ISREG(st_old.st_منوال))		// not a file
	{
	    اگر (S_ISDIR(st_old.st_منوال))
	    {
		errnum = (char_u *)"E502: ";
		errmsg = (char_u *)_(e_is_a_directory);
		جاؤ fail;
	    }
	    اگر (mch_nodetype(fname) != گرہ_WRITABLE)
	    {
		errnum = (char_u *)"E503: ";
		errmsg = (char_u *)_(e_is_not_file_or_writable_device);
		جاؤ fail;
	    }
	    // It's a device of some kind (or a fifo) which we can write to
	    // but for which we can't make a backup.
	    device = TRUE;
	    newfile = TRUE;
	    perm = -1;
	}
    }
#ورنہ // !UNIX
    // Check for a writable device name.
    c = mch_nodetype(fname);
    اگر (c == گرہ_OTHER)
    {
	errnum = (char_u *)"E503: ";
	errmsg = (char_u *)_(e_is_not_file_or_writable_device);
	جاؤ fail;
    }
    اگر (c == گرہ_WRITABLE)
    {
# اگر defined(MSWIN)
	// MS-Windows allows opening a device, but we will probably get stuck
	// trying to write to it.
	اگر (!p_odev)
	{
	    errnum = (char_u *)"E796: ";
	    errmsg = (char_u *)_(e_writing_مقصد_device_disabled_with_opendevice_option);
	    جاؤ fail;
	}
# endif
	device = TRUE;
	newfile = TRUE;
	perm = -1;
    }
    ورنہ
    {
	perm = mch_getperm(fname);
	اگر (perm < 0)
	    newfile = TRUE;
	ورنہ اگر (mch_isdir(fname))
	{
	    errnum = (char_u *)"E502: ";
	    errmsg = (char_u *)_(e_is_a_directory);
	    جاؤ fail;
	}
	اگر (overwriting)
	    (خالی)mch_stat((حرف *)fname, &st_old);
    }
#اگر_ختم // !UNIX

    اگر (!device && !newfile)
    {
	// Check if the file is really writable (when renaming the file to
	// make a backup we won't discover it later).
	file_readonly = check_file_readonly(fname, (صحیح)perm);

	اگر (!forceit && file_readonly)
	{
	    اگر (vim_strchr(p_cpo, CPO_FWRITE) != تہی)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_اندر_cpoptions);
	    }
	    ورنہ
	    {
		errnum = (char_u *)"E505: ";
		errmsg = (char_u *)_(e_is_read_only_جوڑو_bang_مقصد_override);
	    }
	    جاؤ fail;
	}

	// Check if the timestamp hasn't changed since reading the file.
	اگر (overwriting)
	{
	    retval = check_mtime(buf, &st_old);
	    اگر (retval == FAIL)
		جاؤ fail;
	}
    }

#اگر_تعریف_شدہ HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    اگر (!newfile)
	acl = mch_get_acl(fname);
#اگر_ختم

    // If 'backupskip' is not empty, don't make a backup for some files.
    dobackup = (p_wb || p_bk || *p_pm != NUL);
    اگر (dobackup && *p_bsk != NUL && مطابقت_file_list(p_bsk, sfname, ffname))
	dobackup = FALSE;

    // Save the value of got_int and reset it.  We don't want a previous
    // interruption cancel writing, only hitting CTRL-C while writing should
    // abort it.
    پچھلا_got_int = got_int;
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
#اگر_ختم

	اگر ((bkc & BKC_YES) || append)	// "yes"
	    backup_copy = TRUE;
#اگر defined(UNIX) || defined(MSWIN)
	ورنہ اگر ((bkc & BKC_AUTO))	// "auto"
	{
# ifdef UNIX
	    // Don't rename the file when:
	    // - it's a hard link
	    // - it's a symbolic link
	    // - we don't have write permission in the directory
	    // - we can't set the owner/group of the new file
	    اگر (st_old.st_nlink > 1
		    || mch_lstat((حرف *)fname, &st) < 0
		    || st.st_dev != st_old.st_dev
		    || st.st_ino != st_old.st_ino
#  ifndef HAVE_FCHOWN
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid
#  endif
		    )
		backup_copy = TRUE;
	    ورنہ
# ورنہ
#  ifdef MSWIN
	    // On NTFS file systems hard links are possible.
	    اگر (mch_is_linked(fname))
		backup_copy = TRUE;
	    ورنہ
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
		برائے (i = 4913; ; i += 123)
		{
		    vim_snprintf((حرف *)tmp_fname + dirlen,
			حجم(tmp_fname) - dirlen, ".%d.swp", i);
		    اگر (mch_lstat((حرف *)tmp_fname, &st) < 0)
		    {
			fd = mch_open((حرف *)tmp_fname,
				    O_CREAT|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
			اگر (fd < 0 && errno == EEXIST)
			    // If the same file name is created by another
			    // process between lstat() and open(), find another
			    // name.
			    جاری;
			توڑو;
		    }
		}
		اگر (fd < 0)	// can't write in directory
		    backup_copy = TRUE;
		ورنہ
		{
# ifdef UNIX
#  ifdef HAVE_FCHOWN
		    vim_ignored = fchown(fd, st_old.st_uid, st_old.st_gid);
#  endif
#  ifdef HAVE_FCHMOD
		    (خالی)mch_fsetperm(fd, perm);
#  ورنہ
		    (خالی)mch_setperm(tmp_fname, perm);
#  endif
		    اگر (mch_stat((حرف *)tmp_fname, &st) < 0
			    || st.st_uid != st_old.st_uid
			    || st.st_gid != st_old.st_gid
			    || (طویل)st.st_منوال != perm)
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

			برائے (try = 0; try < 10; ++try)
			{
			    اگر (mch_lstat((حرف *)tmp_fname, &st) < 0)
				توڑو;
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

	    lstat_res = mch_lstat((حرف *)fname, &st);

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
# ورنہ
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

#اگر_ختم

	// make sure we have a valid backup extension to use
	اگر (*p_bex == NUL)
	    backup_ext = (char_u *)".bak";
	ورنہ
	    backup_ext = p_bex;

	اگر (backup_copy
		&& (fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
	{
	    صحیح		bfd;
	    char_u	*copybuf;
	    صحیح		some_error = FALSE;
	    stat_T	st_new;
	    char_u	*dirp;
	    char_u	*rootname;
#اگر defined(UNIX)
	    صحیح		did_سیٹ_shortname;
	    mode_t	umask_save;
#اگر_ختم

	    copybuf = alloc(WRITEBUFSIZE + 1);
	    اگر (copybuf == تہی)
	    {
		some_error = TRUE;	    // out of memory
		جاؤ nobackup;
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
	    جب (*dirp)
	    {
		char_u	*p غیر_مستعمل;
		صحیح	copybuf_len غیر_مستعمل;

#اگر_تعریف_شدہ UNIX
		st_new.st_ino = 0;
		st_new.st_dev = 0;
		st_new.st_gid = 0;
#اگر_ختم

		// Isolate one directory name, using an entry in 'bdir'.
		copybuf_len = copy_option_part(&dirp, copybuf, WRITEBUFSIZE, ",");

#اگر defined(UNIX) || defined(MSWIN)
		p = copybuf + copybuf_len;
		اگر (after_pathsep(copybuf, p) && p[-1] == p[-2])
		    // Ends with '//', use full path
		    اگر ((p = make_percent_swname(copybuf, p, fname)) != تہی)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#اگر_ختم
		rootname = get_file_اندر_dir(fname, copybuf);
		اگر (rootname == تہی)
		{
		    some_error = TRUE;	    // out of memory
		    جاؤ nobackup;
		}

#اگر defined(UNIX)
		did_سیٹ_shortname = FALSE;
#اگر_ختم

		// May try twice if 'shortname' not set.
		برائے (;;)
		{
		    // Make the backup file name.
		    اگر (backup == تہی)
			backup = buf_modname((buf->b_p_sn || buf->b_shortname),
						 rootname, backup_ext, FALSE);
		    اگر (backup == تہی)
		    {
			vim_free(rootname);
			some_error = TRUE;		// out of memory
			جاؤ nobackup;
		    }

		    // Check if backup file already exists.
		    اگر (mch_stat((حرف *)backup, &st_new) >= 0)
		    {
#اگر_تعریف_شدہ UNIX
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
				did_سیٹ_shortname = TRUE;
				جاری;
			    }
				// setting shortname didn't help
			    اگر (did_سیٹ_shortname)
				buf->b_shortname = false;
			    توڑو;
			}
#اگر_ختم

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
			    جب (*wp > 'a'
				    && mch_stat((حرف *)backup, &st_new) >= 0)
				--*wp;
			    // They all exist??? Must be something wrong.
			    اگر (*wp == 'a')
				VIM_CLEAR(backup);
			}
		    }
		    توڑو;
		}
		vim_free(rootname);

		// Try to create the backup file
		اگر (backup != تہی)
		{
		    // remove old backup, if present
		    mch_remove(backup);
		    // Open with O_EXCL to avoid the file being created while
		    // we were sleeping (symlink hacker attack?). Reset umask
		    // if possible to avoid mch_setperm() below.
#اگر_تعریف_شدہ UNIX
		    umask_save = umask(0);
#اگر_ختم
		    bfd = mch_open((حرف *)backup,
				O_WRONLY|O_CREAT|O_EXTRA|O_EXCL|O_NOFOLLOW,
								 perm & 0777);
#اگر_تعریف_شدہ UNIX
		    (خالی)umask(umask_save);
#اگر_ختم
		    اگر (bfd < 0)
			VIM_CLEAR(backup);
		    ورنہ
		    {
			// Set file protection same as original file, but
			// strip s-bit.  Only needed if umask() wasn't used
			// above.
#اگر_غیر_تعریف_شدہ UNIX
			(خالی)mch_setperm(backup, perm & 0777);
#ورنہ
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
#اگر_ختم

			// copy the file.
			write_info.bw_fd = bfd;
			write_info.bw_buf = copybuf;
			write_info.bw_flags = FIO_NOCONVERT;
			جب ((write_info.bw_len = read_eintr(fd, copybuf,
							    WRITEBUFSIZE)) > 0)
			{
			    اگر (buf_write_بائٹیں(&write_info) == FAIL)
			    {
				errmsg = (char_u *)_(e_cant_write_مقصد_backup_file_جوڑو_bang_مقصد_override);
				توڑو;
			    }
			    ui_breakcheck();
			    اگر (got_int)
			    {
				errmsg = (char_u *)_(e_interrupted);
				توڑو;
			    }
			}

			اگر (close(bfd) < 0 && errmsg == تہی)
			    errmsg = (char_u *)_(e_close_error_for_backup_file_جوڑو_bang_مقصد_write_anyway);
			اگر (write_info.bw_len < 0)
			    errmsg = (char_u *)_(e_cant_read_file_for_backup_جوڑو_bang_مقصد_write_anyway);
#اگر_تعریف_شدہ UNIX
			سیٹ_file_وقت(backup, st_old.st_atime, st_old.st_mtime);
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ACL
			mch_سیٹ_acl(backup, acl);
#اگر_ختم
#اگر defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_XATTR
			mch_copy_xattr(fname, backup);
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
			(خالی)mch_copy_file_attribute(fname, backup);
#اگر_ختم
			توڑو;
		    }
		}
	    }
    nobackup:
	    close(fd);		// ignore errors for closing read file
	    vim_free(copybuf);

	    اگر (backup == تہی && errmsg == تہی)
		errmsg = (char_u *)_(e_cannot_create_backup_file_جوڑو_bang_مقصد_write_anyway);
	    // ignore errors when forceit is TRUE
	    اگر ((some_error || errmsg != تہی) && !forceit)
	    {
		retval = FAIL;
		جاؤ fail;
	    }
	    errmsg = تہی;
	}
	ورنہ
	{
	    char_u	*dirp;
	    char_u	*p;
	    char_u	*rootname;

	    // Make a backup by renaming the original file.

	    // If 'cpoptions' includes the "W" flag, we don't want to
	    // overwrite a read-only file.  But rename may be possible
	    // anyway, thus we need an extra check here.
	    اگر (file_readonly && vim_strchr(p_cpo, CPO_FWRITE) != تہی)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_اندر_cpoptions);
		جاؤ fail;
	    }

	    // Form the backup file name - change path/fo.o.h to
	    // path/fo.o.h.bak Try all directories in 'backupdir', first one
	    // that works is used.
	    dirp = p_bdir;
	    جب (*dirp)
	    {
		صحیح IObufflen غیر_مستعمل;

		// Isolate one directory name and make the backup file name.
		IObufflen = copy_option_part(&dirp, IObuff, IOSIZE, ",");

#اگر defined(UNIX) || defined(MSWIN)
		p = IObuff + IObufflen;
		اگر (after_pathsep(IObuff, p) && p[-1] == p[-2])
		    // path ends with '//', use full path
		    اگر ((p = make_percent_swname(IObuff, p, fname)) != تہی)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#اگر_ختم
		اگر (backup == تہی)
		{
		    rootname = get_file_اندر_dir(fname, IObuff);
		    اگر (rootname == تہی)
			backup = تہی;
		    ورنہ
		    {
			backup = buf_modname(
				(buf->b_p_sn || buf->b_shortname),
						rootname, backup_ext, FALSE);
			vim_free(rootname);
		    }
		}

		اگر (backup != تہی)
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
			جب (*p > 'a' && mch_getperm(backup) >= 0)
			    --*p;
			// They all exist??? Must be something wrong!
			اگر (*p == 'a')
			    VIM_CLEAR(backup);
		    }
		}
		اگر (backup != تہی)
		{
		    // Delete any existing backup and move the current version
		    // to the backup.	For safety, we don't remove the backup
		    // until the write has finished successfully. And if the
		    // 'backup' option is set, leave it around.

		    // If the renaming of the original file to the backup file
		    // works, quit here.
		    اگر (vim_rename(fname, backup) == 0)
			توڑو;

		    VIM_CLEAR(backup);   // don't do the rename below
		}
	    }
	    اگر (backup == تہی && !forceit)
	    {
		errmsg = (char_u *)_(e_cant_make_backup_file_جوڑو_bang_مقصد_write_anyway);
		جاؤ fail;
	    }
	}
    }

#اگر defined(UNIX)
    // When using ":w!" and the file was read-only: make it writable
    اگر (forceit && perm >= 0 && !(perm & 0200) && st_old.st_uid == getuid()
				     && vim_strchr(p_cpo, CPO_FWRITE) == تہی)
    {
	perm |= 0200;
	(خالی)mch_setperm(fname, perm);
	made_writable = TRUE;
    }
#اگر_ختم

    // When using ":w!" and writing to the current file, 'readonly' makes no
    // sense, reset it, unless 'Z' appears in 'cpoptions'.
    اگر (forceit && overwriting && vim_strchr(p_cpo, CPO_KEEPRO) == تہی)
    {
	buf->b_p_ro = FALSE;
	need_maketitle = TRUE;	    // set window title later
	حالت_redraw_all();	    // redraw status lines later
    }

    اگر (انتہا > buf->b_ml.ml_سطر_گنتی)
	انتہا = buf->b_ml.ml_سطر_گنتی;
    اگر (buf->b_ml.ml_flags & ML_خالی_خانے)
	آغاز = انتہا + 1;

    // If the original file is being overwritten, there is a small chance that
    // we crash in the middle of writing. Therefore the file is preserved now.
    // This makes all block numbers positive so that recovery does not need
    // the original file.
    // Don't do this if there is a backup file and we are exiting.
    اگر (reset_بدلا && !newfile && overwriting
					      && !(exiting && backup != تہی))
    {
	ml_preserve(buf, FALSE);
	اگر (got_int)
	{
	    errmsg = (char_u *)_(e_interrupted);
	    جاؤ restore_backup;
	}
    }

#اگر_تعریف_شدہ VMS
    vms_remove_نسخہ(fname); // remove version
#اگر_ختم
    // Default: write the file directly.  May write to a temp file for
    // multi-byte conversion.
    wfname = fname;

    // Check for forced 'fileencoding' from "++opt=val" argument.
    اگر (eap != تہی && eap->force_enc != 0)
    {
	fenc = eap->cmd + eap->force_enc;
	fenc = enc_canonize(fenc);
	fenc_tofree = fenc;
    }
    ورنہ
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
	    صحیح سائز = bufsize + CONV_RESTLEN;
	    // Need to allocate a buffer to translate into.
	    اگر (wb_flags & (FIO_UCS2 | FIO_UTF16 | FIO_UTF8))
		write_info.bw_conv_buflen = سائز * 2;
	    ورنہ // FIO_UCS4
		write_info.bw_conv_buflen = سائز * 4;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    اگر (write_info.bw_conv_buf == تہی)
		انتہا = 0;
	}
    }

#اگر_تعریف_شدہ MSWIN
    اگر (converted && wb_flags == 0 && (wb_flags = get_win_fio_flags(fenc)) != 0)
    {
	// Convert UTF-8 -> UCS-2 and UCS-2 -> DBCS.  Worst-case * 4:
	write_info.bw_conv_buflen = bufsize * 4;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	اگر (write_info.bw_conv_buf == تہی)
	    انتہا = 0;
    }
#اگر_ختم

#اگر_تعریف_شدہ MACOS_CONVERT
    اگر (converted && wb_flags == 0 && (wb_flags = get_mac_fio_flags(fenc)) != 0)
    {
	write_info.bw_conv_buflen = bufsize * 3;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	اگر (write_info.bw_conv_buf == تہی)
	    انتہا = 0;
    }
#اگر_ختم

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
	    اگر (write_info.bw_conv_buf == تہی)
		انتہا = 0;
	    write_info.bw_پہلا = TRUE;
	}
#  ifdef FEAT_EVAL
	ورنہ
#  endif
# endif

# ifdef FEAT_EVAL
	    // When the file needs to be converted with 'charconvert' after
	    // writing, write to a temp file instead and let the conversion
	    // overwrite the original file.
	    اگر (*p_ccv != NUL)
	    {
		wfname = vim_tempname('w', FALSE);
		اگر (wfname == تہی)	// Can't write without a tempfile!
		{
		    errmsg = (char_u *)_(e_cant_ڈھونڈو_عارضی_file_for_writing);
		    جاؤ restore_backup;
		}
	    }
# endif
    }
#اگر_ختم
    اگر (converted && wb_flags == 0
#اگر_تعریف_شدہ USE_ICONV
	    && write_info.bw_iconv_fd == (iconv_t)-1
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
	    && wfname == fname
#اگر_ختم
	    )
    {
	اگر (!forceit)
	{
	    errmsg = (char_u *)_(e_cannot_convert_جوڑو_bang_مقصد_write_without_conversion);
	    جاؤ restore_backup;
	}
	notconverted = TRUE;
    }

    // If conversion is taking place, we may first pretend to write and check
    // for conversion errors.  Then loop again to write for real.
    // When not doing conversion this writes for real right away.
    برائے (checking_conversion = TRUE; ; checking_conversion = FALSE)
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
	ورنہ
	{
#اگر_تعریف_شدہ HAVE_FTRUNCATE
# define TRUNC_ON_OPEN 0
#ورنہ
# define TRUNC_ON_OPEN O_TRUNC
#اگر_ختم
	    // Open the file "wfname" for writing.
	    // We may try to open the file twice: If we can't write to the file
	    // and forceit is TRUE we delete the existing file and try to
	    // create a new one. If this still fails we may have lost the
	    // original file!  (this may happen when the user reached his
	    // quotum for number of files).
	    // Appending will fail if the file does not exist and forceit is
	    // FALSE.
	    جب ((fd = mch_open((حرف *)wfname, O_WRONLY | O_EXTRA | (append
				? (forceit ? (O_APPEND | O_CREAT) : O_APPEND)
				: (O_CREAT | TRUNC_ON_OPEN))
				, perm < 0 ? 0666 : (perm & 0777))) < 0)
	    {
		// A forced write will try to create a new file if the old one
		// is still readonly. This may also happen when the directory
		// is read-only. In that case the mch_remove() will fail.
		اگر (errmsg == تہی)
		{
#اگر_تعریف_شدہ UNIX
		    stat_T	st;

		    // Don't delete the file when it's a hard or symbolic link.
		    اگر ((!newfile && st_old.st_nlink > 1)
			    || (mch_lstat((حرف *)fname, &st) == 0
				&& (st.st_dev != st_old.st_dev
				    || st.st_ino != st_old.st_ino)))
			errmsg =
			      (char_u *)_(e_cant_open_linked_file_for_writing);
		    ورنہ
#اگر_ختم
		    {
			errmsg = (char_u *)_(e_cant_open_file_for_writing);
			اگر (forceit && vim_strchr(p_cpo, CPO_FWRITE) == تہی
								  && perm >= 0)
			{
#اگر_تعریف_شدہ UNIX
			    // we write to the file, thus it should be marked
			    // writable after all
			    اگر (!(perm & 0200))
				made_writable = TRUE;
			    perm |= 0200;
			    اگر (st_old.st_uid != getuid()
						  || st_old.st_gid != getgid())
				perm &= 0777;
#اگر_ختم
			    اگر (!append)  // don't remove when appending
				mch_remove(wfname);
			    جاری;
			}
		    }
		}

restore_backup:
		{
		    stat_T	st;

		    // If we failed to open the file, we don't need a backup.
		    // Throw it away.  If we moved or removed the original file
		    // try to put the backup in its place.
		    اگر (backup != تہی && wfname == fname)
		    {
			اگر (backup_copy)
			{
			    // There is a small chance that we removed the
			    // original, try to move the copy in its place.
			    // This may not work if the vim_rename() fails.
			    // In that case we leave the copy around.

			    // If file does not exist, put the copy in its
			    //رکھو
			    اگر (mch_stat((حرف *)fname, &st) < 0)
				vim_rename(backup, fname);
			    // if original file does exist throw away the copy
			    اگر (mch_stat((حرف *)fname, &st) >= 0)
				mch_remove(backup);
			}
			ورنہ
			{
			    // try to put the original file back
			    vim_rename(backup, fname);
			}
		    }

		    // if original file no longer exists give an extra warning
		    اگر (!newfile && mch_stat((حرف *)fname, &st) < 0)
			انتہا = 0;
		}

		اگر (wfname != fname)
		    vim_free(wfname);
		جاؤ fail;
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
		    errmsg = (char_u *)_(e_file_بدلا_while_writing);
		    جاؤ fail;
		}
	    }
#اگر_ختم
#اگر_تعریف_شدہ HAVE_FTRUNCATE
	    اگر (!append)
		vim_ignored = ftruncate(fd, (off_t)0);
#اگر_ختم

#اگر defined(MSWIN)
	    اگر (backup != تہی && overwriting && !append)
		(خالی)mch_copy_file_attribute(backup, wfname);

	    اگر (!overwriting && !append)
	    {
		اگر (buf->b_ffname != تہی)
		    (خالی)mch_copy_file_attribute(buf->b_ffname, wfname);
		// Should copy resource fork
	    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CRYPT
	    اگر (*buf->b_p_کلید != NUL && !filtering)
	    {
		char_u		*سرخی;
		صحیح		سرخی_len;

		buf->b_cryptstate = crypt_create_for_writing(
						      crypt_get_طریقہ_nr(buf),
					   buf->b_p_کلید, &سرخی, &سرخی_len);
		اگر (buf->b_cryptstate == تہی || سرخی == تہی)
		    انتہا = 0;
		ورنہ
		{
		    // Write magic number, so that Vim knows how this file is
		    // encrypted when reading it back.
		    write_info.bw_buf = سرخی;
		    write_info.bw_len = سرخی_len;
		    write_info.bw_flags = FIO_NOCONVERT;
		    اگر (buf_write_بائٹیں(&write_info) == FAIL)
			انتہا = 0;
		    wb_flags |= FIO_ENCRYPTED;
		    vim_free(سرخی);
		}
	    }
#اگر_ختم
	}
	errmsg = تہی;

	write_info.bw_buf = بفر;
	nchars = 0;

	// use "++bin", "++nobin" or 'binary'
	اگر (eap != تہی && eap->force_bin != 0)
	    write_bin = (eap->force_bin == FORCE_BIN);
	ورنہ
	    write_bin = buf->b_p_bin;

	// The BOM is written just after the encryption magic number.
	// Skip it when appending and the file already existed, the BOM only
	// makes sense at the start of the file.
	اگر (buf->b_p_bomb && !write_bin && (!append || perm < 0))
	{
	    write_info.bw_len = make_bom(بفر, fenc);
	    اگر (write_info.bw_len > 0)
	    {
		// don't convert, do encryption
		write_info.bw_flags = FIO_NOCONVERT | wb_flags;
		اگر (buf_write_بائٹیں(&write_info) == FAIL)
		    انتہا = 0;
		ورنہ
		    nchars += write_info.bw_len;
	    }
	}
	write_info.bw_آغاز_lnum = آغاز;

#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
	write_undo_file = (buf->b_p_udf
			    && overwriting
			    && !append
			    && !filtering
# ifdef CRYPT_NOT_INPLACE
			    // writing undo file requires
			    // crypt_encode_inplace()
			    && (buf->b_cryptstate == تہی
				|| crypt_چلتا_inplace(buf->b_cryptstate))
# endif
			    && reset_بدلا
			    && !checking_conversion);
# ifdef CRYPT_NOT_INPLACE
	// remove undo file if encrypting it is not possible
	اگر (buf->b_p_udf
		&& overwriting
		&& !append
		&& !filtering
		&& !checking_conversion
		&& buf->b_cryptstate != تہی
		&& !crypt_چلتا_inplace(buf->b_cryptstate))
	    u_undofile_reset_and_delete(buf);
# endif
	اگر (write_undo_file)
	    // Prepare for computing the hash value of the text.
	    sha256_آغاز(&sha_ctx);
#اگر_ختم

	write_info.bw_len = bufsize;
	write_info.bw_flags = wb_flags;
	fileformat = get_fileformat_force(buf, eap);
	s = بفر;
	len = 0;
	برائے (lnum = آغاز; lnum <= انتہا; ++lnum)
	{
	    // The next while loop is done once for each character written.
	    // Keep it fast!
	    ptr = ml_get_buf(buf, lnum, FALSE) - 1;
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
	    اگر (write_undo_file)
		sha256_update(&sha_ctx, ptr + 1,
					      (UINT32_T)(STRLEN(ptr + 1) + 1));
#اگر_ختم
	    جب ((c = *++ptr) != NUL)
	    {
		اگر (c == NL)
		    *s = NUL;		// replace newlines with NULs
		ورنہ اگر (c == CAR && fileformat == EOL_MAC)
		    *s = NL;		// Mac: replace CRs with NLs
		ورنہ
		    *s = c;
		++s;
		اگر (++len != bufsize)
		    جاری;
#اگر_تعریف_شدہ FEAT_CRYPT
		اگر (write_info.bw_fd > 0 && lnum == انتہا
			&& (write_info.bw_flags & FIO_ENCRYPTED)
			&& *buf->b_p_کلید != NUL && !filtering
			&& *ptr == NUL)
		    write_info.bw_اختتام = TRUE;
#اگر_ختم
		اگر (buf_write_بائٹیں(&write_info) == FAIL)
		{
		    انتہا = 0;		// write error: break loop
		    توڑو;
		}
		nchars += bufsize;
		s = بفر;
		len = 0;
		write_info.bw_آغاز_lnum = lnum;
	    }
	    // write failed or last line has no EOL: stop here
	    اگر (انتہا == 0
		    || (lnum == انتہا
			&& (write_bin || !buf->b_p_fixeol)
			&& ((write_bin && lnum == buf->b_no_eol_lnum)
			    || (lnum == buf->b_ml.ml_سطر_گنتی
							   && !buf->b_p_eol))))
	    {
		++lnum;			// written the line, count it
		no_eol = TRUE;
		توڑو;
	    }
	    اگر (fileformat == EOL_UNIX)
		*s++ = NL;
	    ورنہ
	    {
		*s++ = CAR;		    // EOL_MAC or EOL_DOS: write CR
		اگر (fileformat == EOL_DOS)  // write CR-NL
		{
		    اگر (++len == bufsize)
		    {
			اگر (buf_write_بائٹیں(&write_info) == FAIL)
			{
			    انتہا = 0;	// write error: break loop
			    توڑو;
			}
			nchars += bufsize;
			s = بفر;
			len = 0;
		    }
		    *s++ = NL;
		}
	    }
	    اگر (++len == bufsize && انتہا)
	    {
		اگر (buf_write_بائٹیں(&write_info) == FAIL)
		{
		    انتہا = 0;		// write error: break loop
		    توڑو;
		}
		nchars += bufsize;
		s = بفر;
		len = 0;

		ui_breakcheck();
		اگر (got_int)
		{
		    انتہا = 0;		// Interrupted, break loop
		    توڑو;
		}
	    }
#اگر_تعریف_شدہ VMS
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
		جب (b2write > 0)
		{
		    write_info.bw_len = MIN(b2write, buf->b_fab_mrs);
		    اگر (buf_write_بائٹیں(&write_info) == FAIL)
		    {
			انتہا = 0;
			توڑو;
		    }
		    b2write -= MIN(b2write, buf->b_fab_mrs);
		}
		write_info.bw_len = bufsize;
		nchars += len;
		s = بفر;
		len = 0;
	    }
#اگر_ختم
	}
	اگر (len > 0 && انتہا > 0)
	{
	    write_info.bw_len = len;
#اگر_تعریف_شدہ FEAT_CRYPT
	    اگر (write_info.bw_fd > 0 && lnum >= انتہا
		    && (write_info.bw_flags & FIO_ENCRYPTED)
		    && *buf->b_p_کلید != NUL && !filtering)
		write_info.bw_اختتام = TRUE;
#اگر_ختم
	    اگر (buf_write_بائٹیں(&write_info) == FAIL)
		انتہا = 0;		    // write error
	    nchars += len;
	}

	اگر (!buf->b_p_fixeol && buf->b_p_eof)
	{
	    // write trailing CTRL-Z
	    (خالی)write_eintr(write_info.bw_fd, "\x1a", 1);
	    nchars++;
	}

	// Stop when writing done or an error was encountered.
	اگر (!checking_conversion || انتہا == 0)
	    توڑو;

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
	    انتہا = 0;
	}
#اگر_ختم

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
#اگر_ختم

#اگر_تعریف_شدہ UNIX
	// When creating a new file, set its owner/group to that of the
	// original file.  Get the new device and inode number.
	اگر (backup != تہی && !backup_copy)
	{
# ifdef HAVE_FCHOWN
	    stat_T	st;

	    // Don't change the owner when it's already OK, some systems remove
	    // permission or ACL stuff.
	    اگر (mch_stat((حرف *)wfname, &st) < 0
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
	ورنہ اگر (!buf->b_dev_valid)
	    // Set the inode when creating a new file.
	    buf_setino(buf);
#اگر_ختم

#اگر_تعریف_شدہ UNIX
	اگر (made_writable)
	    perm &= ~0200;	// reset 'w' bit for security reasons
#اگر_ختم
#اگر_تعریف_شدہ HAVE_FCHMOD
	// set permission of new file same as old file
	اگر (perm >= 0)
	    (خالی)mch_fsetperm(fd, perm);
#اگر_ختم
	اگر (close(fd) != 0)
	{
	    errmsg = (char_u *)_(e_close_failed);
	    انتہا = 0;
	}

#اگر_غیر_تعریف_شدہ HAVE_FCHMOD
	// set permission of new file same as old file
	اگر (perm >= 0)
	    (خالی)mch_setperm(wfname, perm);
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ACL
	// Probably need to set the ACL before changing the user (can't set the
	// ACL on a file the user doesn't own).
	// On Solaris, with ZFS and the aclmode property set to "discard" (the
	// default), chmod() discards all part of a file's ACL that don't
	// represent the mode of the file.  It's non-trivial for us to discover
	// whether we're in that situation, so we simply always re-set the ACL.
# ifndef HAVE_SOLARIS_ZFS_ACL
	اگر (!backup_copy)
# endif
	    mch_سیٹ_acl(wfname, acl);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CRYPT
	اگر (buf->b_cryptstate != تہی)
	{
	    crypt_free_state(buf->b_cryptstate);
	    buf->b_cryptstate = تہی;
	}
#اگر_ختم

#اگر defined(FEAT_EVAL)
	اگر (wfname != fname)
	{
	    // The file was written to a temp file, now it needs to be
	    // converted with 'charconvert' to (overwrite) the output file.
	    اگر (انتہا != 0)
	    {
		اگر (eval_charconvert(enc_utf8 ? (char_u *)"utf-8" : p_enc,
						  fenc, wfname, fname) == FAIL)
		{
		    write_info.bw_conv_error = TRUE;
		    انتہا = 0;
		}
	    }
	    mch_remove(wfname);
	    vim_free(wfname);
	}
#اگر_ختم
    }

    اگر (انتہا == 0)
    {
	// Error encountered.
	اگر (errmsg == تہی)
	{
	    اگر (write_info.bw_conv_error)
	    {
		اگر (write_info.bw_conv_error_lnum == 0)
		    errmsg = (char_u *)_(e_write_error_conversion_failed_make_fenc_خالی_خانے_مقصد_override);
		ورنہ
		{
		    errmsg_allocated = TRUE;
		    errmsg = alloc(300);
		    اگر (errmsg == تہی)
			جاؤ fail;
		    vim_snprintf((حرف *)errmsg, 300, _(e_write_error_conversion_failed_اندر_سطر_nr_make_fenc_خالی_خانے_مقصد_override),
					 (طویل)write_info.bw_conv_error_lnum);
		}
	    }
	    ورنہ اگر (got_int)
		errmsg = (char_u *)_(e_interrupted);
	    ورنہ
		errmsg = (char_u *)_(e_write_error_file_system_بھرا);
	}

	// If we have a backup file, try to put it in place of the new file,
	// because the new file is probably corrupt.  This avoids losing the
	// original file when trying to make a backup when writing the file a
	// second time.
	// When "backup_copy" is set we need to copy the backup over the new
	// file.  Otherwise rename the backup file.
	// If this is OK, don't give the extra warning message.
	اگر (backup != تہی)
	{
	    اگر (backup_copy)
	    {
		// This may take a while, if we were interrupted let the user
		// know we got the message.
		اگر (got_int)
		{
		    پیغام(_(e_interrupted));
		    برآمد_flush();
		}
		اگر ((fd = mch_open((حرف *)backup, O_RDONLY | O_EXTRA, 0)) >= 0)
		{
		    اگر ((write_info.bw_fd = mch_open((حرف *)fname,
				    O_WRONLY | O_CREAT | O_TRUNC | O_EXTRA,
							   perm & 0777)) >= 0)
		    {
			// copy the file.
			write_info.bw_buf = smallbuf;
			write_info.bw_flags = FIO_NOCONVERT;
			جب ((write_info.bw_len = read_eintr(fd, smallbuf,
						      SMALLBUFSIZE)) > 0)
			    اگر (buf_write_بائٹیں(&write_info) == FAIL)
				توڑو;

			اگر (close(write_info.bw_fd) >= 0
						   && write_info.bw_len == 0)
			    انتہا = 1;		// success
		    }
		    close(fd);	// ignore errors for closing read file
		}
	    }
	    ورنہ
	    {
		اگر (vim_rename(backup, fname) == 0)
		    انتہا = 1;
	    }
	}
	جاؤ fail;
    }

    lnum -= آغاز;	    // compute number of written lines
    --no_wait_return;	    // may wait for return now

#اگر !(defined(UNIX) || defined(VMS))
    fname = sfname;	    // use shortname now, for the messages
#اگر_ختم
    اگر (!filtering)
    {
	پیغام_جوڑو_fname(buf, fname);	// put fname in IObuff with quotes
	c = FALSE;
	اگر (write_info.bw_conv_error)
	{
	    STRCAT(IObuff, _(" CONVERSION ERROR"));
	    c = TRUE;
	    اگر (write_info.bw_conv_error_lnum != 0)
		vim_snprintf_جوڑو((حرف *)IObuff, IOSIZE, _(" in line %ld;"),
			(طویل)write_info.bw_conv_error_lnum);
	}
	ورنہ اگر (notconverted)
	{
	    STRCAT(IObuff, _("[NOT converted]"));
	    c = TRUE;
	}
	ورنہ اگر (converted)
	{
	    STRCAT(IObuff, _("[converted]"));
	    c = TRUE;
	}
	اگر (device)
	{
	    STRCAT(IObuff, _("[Device]"));
	    c = TRUE;
	}
	ورنہ اگر (newfile)
	{
	    STRCAT(IObuff, new_file_پیغام());
	    c = TRUE;
	}
	اگر (no_eol)
	{
	    پیغام_جوڑو_eol();
	    c = TRUE;
	}
	// may add [unix/dos/mac]
	اگر (پیغام_جوڑو_fileformat(fileformat))
	    c = TRUE;
#اگر_تعریف_شدہ FEAT_CRYPT
	اگر (wb_flags & FIO_ENCRYPTED)
	{
	    crypt_append_پیغام(buf);
	    c = TRUE;
	}
#اگر_ختم
	پیغام_جوڑو_سطریں(c, (طویل)lnum, nchars);	// add line/char count
	اگر (!shortmess(SHM_WRITE))
	{
	    اگر (append)
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [a]") : _(" appended"));
	    ورنہ
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [w]") : _(" written"));
	}

	سیٹ_keep_پیغام((char_u *)پیغام_trunc_attr((حرف *)IObuff, FALSE, 0), 0);
    }

    // When written everything correctly: reset 'modified'.  Unless not
    // writing to the original file and '+' is not in 'cpoptions'.
    اگر (reset_بدلا && پورا && !append
	    && !write_info.bw_conv_error
	    && (overwriting || vim_strchr(p_cpo, CPO_PLUS) != تہی))
    {
	unchanged(buf, TRUE, FALSE);
	// b:changedtick may be incremented in unchanged() but that should not
	// trigger a TextChanged event.
	اگر (buf->b_آخری_changedtick + 1 == CHANGEDTICK(buf))
	    buf->b_آخری_changedtick = CHANGEDTICK(buf);
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
	ورنہ
	    buf->b_flags &= ~BF_WRITE_نقاب;
    }

    // If we kept a backup until now, and we are in patch mode, then we make
    // the backup file our 'original' file.
    اگر (*p_pm && dobackup)
    {
	حرف *org = (حرف *)buf_modname((buf->b_p_sn || buf->b_shortname),
							  fname, p_pm, FALSE);

	اگر (backup != تہی)
	{
	    stat_T	st;

	    // If the original file does not exist yet
	    // the current backup file becomes the original file
	    اگر (org == تہی)
		emsg(_(e_patchmode_cant_save_original_file));
	    ورنہ اگر (mch_stat(org, &st) < 0)
	    {
		vim_rename(backup, (char_u *)org);
		VIM_CLEAR(backup);	    // don't delete the file
#اگر_تعریف_شدہ UNIX
		سیٹ_file_وقت((char_u *)org, st_old.st_atime, st_old.st_mtime);
#اگر_ختم
	    }
	}
	// If there is no backup file, remember that a (new) file was
	// created.
	ورنہ
	{
	    صحیح خالی_خانے_fd;

	    اگر (org == تہی
		    || (خالی_خانے_fd = mch_open(org,
				      O_CREAT | O_EXTRA | O_EXCL | O_NOFOLLOW,
					perm < 0 ? 0666 : (perm & 0777))) < 0)
	      emsg(_(e_patchmode_cant_touch_خالی_خانے_original_file));
	    ورنہ
	      close(خالی_خانے_fd);
	}
	اگر (org != تہی)
	{
	    mch_setperm((char_u *)org, mch_getperm(fname) & 0777);
	    vim_free(org);
	}
    }

    // Remove the backup unless 'backup' option is set or there was a
    // conversion error.
    اگر (!p_bk && backup != تہی && !write_info.bw_conv_error
	    && mch_remove(backup) != 0)
	emsg(_(e_cant_delete_backup_file));

    جاؤ nofail;

    // Finish up.  We get here either after failure or success.
fail:
    --no_wait_return;		// may wait for return now
nofail:

    // Done saving, we accept changed buffer warnings again
    buf->b_saving = false;

    vim_free(backup);
    اگر (بفر != smallbuf)
	vim_free(بفر);
    vim_free(fenc_tofree);
    vim_free(write_info.bw_conv_buf);
#اگر_تعریف_شدہ USE_ICONV
    اگر (write_info.bw_iconv_fd != (iconv_t)-1)
    {
	iconv_close(write_info.bw_iconv_fd);
	write_info.bw_iconv_fd = (iconv_t)-1;
    }
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ACL
    mch_free_acl(acl);
#اگر_ختم

    اگر (errmsg != تہی)
    {
	صحیح numlen = errnum != تہی ? (صحیح)STRLEN(errnum) : 0;

	attr = HL_ATTR(HLF_E);	// set highlight for error messages
	پیغام_جوڑو_fname(buf,
#اگر_غیر_تعریف_شدہ UNIX
		sfname
#ورنہ
		fname
#اگر_ختم
		     );		// put file name in IObuff with quotes
	اگر (STRLEN(IObuff) + STRLEN(errmsg) + numlen >= IOSIZE)
	    IObuff[IOSIZE - STRLEN(errmsg) - numlen - 1] = NUL;
	// If the error message has the form "is ...", put the error number in
	// front of the file name.
	اگر (errnum != تہی)
	{
	    STRMOVE(IObuff + numlen, IObuff);
	    mch_memmove(IObuff, errnum, (size_t)numlen);
	}
	STRCAT(IObuff, errmsg);
	emsg((حرف *)IObuff);
	اگر (errmsg_allocated)
	    vim_free(errmsg);

	retval = FAIL;
	اگر (انتہا == 0)
	{
	    پیغام_puts_attr(_("\nWARNING: Original file may be lost or damaged\n"),
		    attr | پیغام_HIST);
	    پیغام_puts_attr(_("don't quit the editor until the file is successfully written!"),
		    attr | پیغام_HIST);

	    // Update the timestamp to avoid an "overwrite changed file"
	    // prompt when writing again.
	    اگر (mch_stat((حرف *)fname, &st_old) >= 0)
	    {
		buf_store_وقت(buf, &st_old, fname);
		buf->b_mtime_read = buf->b_mtime;
		buf->b_mtime_read_ns = buf->b_mtime_ns;
	    }
	}
    }
    پیغام_scroll = پیغام_save;

#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
    // When writing the whole file and 'undofile' is set, also write the undo
    // file.
    اگر (retval == OK && write_undo_file)
    {
	char_u	    hash[UNDO_HASH_سائز];

	sha256_اختتام(&sha_ctx, hash);
	u_write_undo(تہی, FALSE, buf, hash);
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (!should_abort(retval))
#ورنہ
    اگر (!got_int)
#اگر_ختم
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
	    ورنہ اگر (filtering)
		apply_autocmds_exarg(EVENT_FILTERWRITEPOST, تہی, fname,
							   FALSE, curbuf, eap);
	    ورنہ اگر (reset_بدلا && پورا)
		apply_autocmds_exarg(EVENT_BUFWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);
	    ورنہ
		apply_autocmds_exarg(EVENT_FILEWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}

#اگر_تعریف_شدہ FEAT_EVAL
	اگر (aborting())	    // autocmds may abort script processing
	    retval = FALSE;
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_VIMINFO
    // Make sure marks will be written out to the viminfo file later, even when
    // the file is new.
    curbuf->b_marks_read = true;
#اگر_ختم

    got_int |= پچھلا_got_int;

    واپس retval;
}
