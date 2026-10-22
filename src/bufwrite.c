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

#إذا defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
# include <utime.h>		// for struct utimbuf
#جيڪڏهن_پورو

#وصف_ڪريو SMALLBUFSIZE	256	// size of emergency write buffer

/*
 * Structure to pass arguments from buf_write() to buf_write_bytes().
 */
بنية bw_info
{
    صحيح		bw_fd;		// file descriptor
    char_u	*bw_buf;	// buffer with data to be written
    صحيح		bw_len;		// length of data
    صحيح		bw_flags;	// FIO_ flags
#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
    buf_T	*bw_بفر;	// buffer being written
    صحيح		bw_پڄاڻي;	// finish encrypting
#جيڪڏهن_پورو
    char_u	bw_rest[CONV_RESTLEN]; // not converted bytes
    صحيح		bw_restlen;	// nr of bytes in bw_rest[]
    صحيح		bw_پهريون;	// first write call
    char_u	*bw_conv_buf;	// buffer for writing converted chars
    size_t	bw_conv_buflen; // size of bw_conv_buf
    صحيح		bw_conv_error;	// set for conversion error
    linenr_T	bw_conv_error_lnum;  // first line with error or zero
    linenr_T	bw_شروعات_lnum;	// line number at start of buffer
#جيڪڏهن_وصف_ٿيل USE_ICONV
    iconv_t	bw_iconv_fd;	// descriptor for iconv() or -1
#جيڪڏهن_پورو
};

/*
 * Convert a Unicode character to bytes.
 * Return TRUE for an error, FALSE when it's OK.
 */
    ساكن صحيح
ucs2bytes(
    بنانشان	c,		// in: character
    char_u	**pp,		// in/out: pointer to result
    صحيح		flags)		// FIO_ flags
{
    char_u	*p = *pp;
    صحيح		error = FALSE;
    صحيح		cc;


    إذا (flags & FIO_UCS4)
    {
	إذا (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	    *p++ = (c >> 16);
	    *p++ = (c >> 24);
	}
	وإلا
	{
	    *p++ = (c >> 24);
	    *p++ = (c >> 16);
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    وإلا إذا (flags & (FIO_UCS2 | FIO_UTF16))
    {
	إذا (c >= 0x10000)
	{
	    إذا (flags & FIO_UTF16)
	    {
		// Make two words, ten bits of the character in each.  First
		// word is 0xd800 - 0xdbff, second one 0xdc00 - 0xdfff
		c -= 0x10000;
		إذا (c >= 0x100000)
		    error = TRUE;
		cc = ((c >> 10) & 0x3ff) + 0xd800;
		إذا (flags & FIO_ENDIAN_L)
		{
		    *p++ = cc;
		    *p++ = ((بنانشان)cc >> 8);
		}
		وإلا
		{
		    *p++ = ((بنانشان)cc >> 8);
		    *p++ = cc;
		}
		c = (c & 0x3ff) + 0xdc00;
	    }
	    وإلا
		error = TRUE;
	}
	إذا (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	}
	وإلا
	{
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    وإلا    // Latin1
    {
	إذا (c >= 0x100)
	{
	    error = TRUE;
	    *p++ = 0xBF;
	}
	وإلا
	    *p++ = c;
    }

    *pp = p;
    أرجع error;
}

/*
 * Call write() to write a number of bytes to the file.
 * Handles encryption and 'encoding' conversion.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ساكن صحيح
buf_write_بائيٽون(بنية bw_info *ip)
{
    صحيح		wlen;
    char_u	*buf = ip->bw_buf;	// data to write
    صحيح		len = ip->bw_len;	// length of data
    صحيح		flags = ip->bw_flags;	// extra flags

    // Skip conversion when writing the crypt magic number or the BOM.
    إذا (!(flags & FIO_NOCONVERT))
    {
	char_u		*p;
	بنانشان	c;
	صحيح		n;

	إذا (flags & FIO_UTF8)
	{
	    // Convert latin1 in the buffer to UTF-8 in the file.
	    p = ip->bw_conv_buf;	// translate to buffer
	    لكل (wlen = 0; wlen < len; ++wlen)
		p += utf_char2bytes(buf[wlen], p);
	    buf = ip->bw_conv_buf;
	    len = (صحيح)(p - ip->bw_conv_buf);
	}
	وإلا إذا (flags & (FIO_UCS4 | FIO_UTF16 | FIO_UCS2 | FIO_LATIN1))
	{
	    // Convert UTF-8 bytes in the buffer to UCS-2, UCS-4, UTF-16 or
	    // Latin1 chars in the file.
	    إذا (flags & FIO_LATIN1)
		p = buf;	// translate in-place (can only get shorter)
	    وإلا
		p = ip->bw_conv_buf;	// translate to buffer
	    لكل (wlen = 0; wlen < len; wlen += n)
	    {
		إذا (wlen == 0 && ip->bw_restlen != 0)
		{
		    صحيح		l;

		    // Use remainder of previous call.  Append the start of
		    // buf[] to get a full sequence.  Might still be too
		    // short!
		    l = CONV_RESTLEN - ip->bw_restlen;
		    إذا (l > len)
			l = len;
		    mch_memmove(ip->bw_rest + ip->bw_restlen, buf, (size_t)l);
		    n = utf_ptr2len_len(ip->bw_rest, ip->bw_restlen + l);
		    إذا (n > ip->bw_restlen + len)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			إذا (ip->bw_restlen + len > CONV_RESTLEN)
			    أرجع FAIL;
			ip->bw_restlen += len;
			اكسر;
		    }
		    إذا (n > 1)
			c = utf_ptr2char(ip->bw_rest);
		    وإلا
			c = ip->bw_rest[0];
		    إذا (n >= ip->bw_restlen)
		    {
			n -= ip->bw_restlen;
			ip->bw_restlen = 0;
		    }
		    وإلا
		    {
			ip->bw_restlen -= n;
			mch_memmove(ip->bw_rest, ip->bw_rest + n,
						      (size_t)ip->bw_restlen);
			n = 0;
		    }
		}
		وإلا
		{
		    n = utf_ptr2len_len(buf + wlen, len - wlen);
		    إذا (n > len - wlen)
		    {
			// We have an incomplete byte sequence at the end to
			// be written.  We can't convert it without the
			// remaining bytes.  Keep them for the next call.
			إذا (len - wlen > CONV_RESTLEN)
			    أرجع FAIL;
			ip->bw_restlen = len - wlen;
			mch_memmove(ip->bw_rest, buf + wlen,
						      (size_t)ip->bw_restlen);
			اكسر;
		    }
		    إذا (n > 1)
			c = utf_ptr2char(buf + wlen);
		    وإلا
			c = buf[wlen];
		}
		// Check that there is enough space
		إذا (!(flags & FIO_LATIN1))
		{
		    size_t need = (flags & FIO_UCS4) ? 4 : 2;
		    إذا ((flags & FIO_UTF16) && c >= 0x10000)
			need = 4;

		    إذا ((size_t)(p - ip->bw_conv_buf) + need > ip->bw_conv_buflen)
			أرجع FAIL;
		}

		إذا (ucs2bytes(c, &p, flags) && !ip->bw_conv_error)
		{
		    ip->bw_conv_error = TRUE;
		    ip->bw_conv_error_lnum = ip->bw_شروعات_lnum;
		}
		إذا (c == NL)
		    ++ip->bw_شروعات_lnum;
	    }
	    إذا (flags & FIO_LATIN1)
		len = (صحيح)(p - buf);
	    وإلا
	    {
		buf = ip->bw_conv_buf;
		len = (صحيح)(p - ip->bw_conv_buf);
	    }
	}

#جيڪڏهن_وصف_ٿيل MSWIN
	وإلا إذا (flags & FIO_CODEPAGE)
	{
	    // Convert UTF-8 or codepage to UCS-2 and then to MS-Windows
	    // codepage.
	    char_u	*from;
	    size_t	fromlen;
	    char_u	*منزل;
	    صحيح		u8c;
	    BOOL	bad = FALSE;
	    صحيح		needed;

	    إذا (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    وإلا
	    {
		from = buf;
		fromlen = len;
	    }

	    منزل = ip->bw_conv_buf;
	    إذا (enc_utf8)
	    {
		// Convert from UTF-8 to UCS-2, to the start of the buffer.
		// The buffer has been allocated to be big enough.
		طالما (fromlen > 0)
		{
		    n = (صحيح)utf_ptr2len_len(from, (صحيح)fromlen);
		    إذا (n > (صحيح)fromlen)	// incomplete byte sequence
			اكسر;
		    u8c = utf_ptr2char(from);
		    *منزل++ = (u8c & 0xff);
		    *منزل++ = (u8c >> 8);
		    fromlen -= n;
		    from += n;
		}

		// Copy remainder to ip->bw_rest[] to be used for the next
		// call.
		إذا (fromlen > CONV_RESTLEN)
		{
		    // weird overlong sequence
		    ip->bw_conv_error = TRUE;
		    أرجع FAIL;
		}
		mch_memmove(ip->bw_rest, from, fromlen);
		ip->bw_restlen = (صحيح)fromlen;
	    }
	    وإلا
	    {
		// Convert from enc_codepage to UCS-2, to the start of the
		// buffer.  The buffer has been allocated to be big enough.
		ip->bw_restlen = 0;
		needed = Multi_Byte_منزل_Wide_Char(enc_codepage,
			     MB_ERR_INVALID_CHARS, (LPCSTR)from, (صحيح)fromlen,
								     عدم, 0);
		إذا (needed == 0)
		{
		    // When conversion fails there may be a trailing byte.
		    needed = Multi_Byte_منزل_Wide_Char(enc_codepage,
			 MB_ERR_INVALID_CHARS, (LPCSTR)from, (صحيح)fromlen - 1,
								     عدم, 0);
		    إذا (needed == 0)
		    {
			// Conversion doesn't work.
			ip->bw_conv_error = TRUE;
			أرجع FAIL;
		    }
		    // Save the trailing byte for the next call.
		    ip->bw_rest[0] = from[fromlen - 1];
		    ip->bw_restlen = 1;
		}
		needed = Multi_Byte_منزل_Wide_Char(enc_codepage, MB_ERR_INVALID_CHARS,
				(LPCSTR)from, (صحيح)(fromlen - ip->bw_restlen),
							  (LPWSTR)منزل, needed);
		إذا (needed == 0)
		{
		    // Safety check: Conversion doesn't work.
		    ip->bw_conv_error = TRUE;
		    أرجع FAIL;
		}
		منزل += needed * 2;
	    }

	    fromlen = منزل - ip->bw_conv_buf;
	    buf = منزل;
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	    إذا (FIO_GET_CP(flags) == CP_UTF8)
	    {
		// Convert from UCS-2 to UTF-8, using the remainder of the
		// conversion buffer.  Fails when out of space.
		لكل (from = ip->bw_conv_buf; fromlen > 1; fromlen -= 2)
		{
		    u8c = *from++;
		    u8c += (*from++ << 8);
		    منزل += utf_char2bytes(u8c, منزل);
		    إذا (منزل + 6 >= ip->bw_conv_buf + ip->bw_conv_buflen)
		    {
			ip->bw_conv_error = TRUE;
			أرجع FAIL;
		    }
		}
		len = (صحيح)(منزل - buf);
	    }
	    وإلا
# endif
	    {
		// Convert from UCS-2 to the codepage, using the remainder of
		// the conversion buffer.  If the conversion uses the default
		// character "0", the data doesn't fit in this encoding, so
		// fail.
		len = Wide_Char_منزل_Multi_Byte(FIO_GET_CP(flags), 0,
			(LPCWSTR)ip->bw_conv_buf, (صحيح)fromlen / حجم(WCHAR),
			(LPSTR)منزل, (صحيح)(ip->bw_conv_buflen - fromlen), 0,
									&bad);
		إذا (bad)
		{
		    ip->bw_conv_error = TRUE;
		    أرجع FAIL;
		}
	    }
	}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل MACOS_CONVERT
	وإلا إذا (flags & FIO_MACROMAN)
	{
	    // Convert UTF-8 or latin1 to Apple MacRoman.
	    char_u	*from;
	    size_t	fromlen;

	    إذا (ip->bw_restlen > 0)
	    {
		// Need to concatenate the remainder of the previous call and
		// the bytes of the current call.  Use the end of the
		// conversion buffer for this.
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    وإلا
	    {
		from = buf;
		fromlen = len;
	    }

	    إذا (enc2macroman(from, fromlen,
			ip->bw_conv_buf, &len, ip->bw_conv_buflen,
			ip->bw_rest, &ip->bw_restlen) == FAIL)
	    {
		ip->bw_conv_error = TRUE;
		أرجع FAIL;
	    }
	    buf = ip->bw_conv_buf;
	}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_ICONV
	إذا (ip->bw_iconv_fd != (iconv_t)-1)
	{
	    ثابت حرف	*from;
	    size_t	fromlen;
	    حرف	*منزل;
	    size_t	tolen;

	    // Convert with iconv().
	    إذا (ip->bw_restlen > 0)
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
	    وإلا
	    {
		from = (ثابت حرف *)buf;
		fromlen = len;
		tolen = ip->bw_conv_buflen;
	    }
	    منزل = (حرف *)ip->bw_conv_buf;

	    إذا (ip->bw_پهريون)
	    {
		size_t	save_len = tolen;

		// output the initial shift state sequence
		(فراغ)iconv(ip->bw_iconv_fd, عدم, عدم, &منزل, &tolen);

		// There is a bug in iconv() on Linux (which appears to be
		// wide-spread) which sets "to" to NULL and messes up "tolen".
		إذا (منزل == عدم)
		{
		    منزل = (حرف *)ip->bw_conv_buf;
		    tolen = save_len;
		}
		ip->bw_پهريون = FALSE;
	    }

	    // If iconv() has an error or there is not enough room, fail.
	    إذا ((iconv(ip->bw_iconv_fd, (فراغ *)&from, &fromlen, &منزل, &tolen)
			== (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						    || fromlen > CONV_RESTLEN)
	    {
		ip->bw_conv_error = TRUE;
		أرجع FAIL;
	    }

	    // copy remainder to ip->bw_rest[] to be used for the next call.
	    إذا (fromlen > 0)
		mch_memmove(ip->bw_rest, (فراغ *)from, fromlen);
	    ip->bw_restlen = (صحيح)fromlen;

	    buf = ip->bw_conv_buf;
	    len = (صحيح)((char_u *)منزل - ip->bw_conv_buf);
	}
#جيڪڏهن_پورو
    }

    إذا (ip->bw_fd < 0)
	// Only checking conversion, which is OK if we get here.
	أرجع OK;

#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
    إذا (flags & FIO_ENCRYPTED)
    {
	// Encrypt the data. Do it in-place if possible, otherwise use an
	// allocated buffer.
# ifdef CRYPT_NOT_INPLACE
	إذا (crypt_هلي_inplace(ip->bw_بفر->b_cryptstate))
	{
# endif
	    crypt_encode_inplace(ip->bw_بفر->b_cryptstate, buf, len,
								ip->bw_پڄاڻي);
# ifdef CRYPT_NOT_INPLACE
	}
	وإلا
	{
	    char_u *outbuf;

	    len = crypt_encode_alloc(curbuf->b_cryptstate, buf, len, &outbuf,
								ip->bw_پڄاڻي);
	    إذا (len == 0)
		أرجع OK;  // Crypt layer is buffering, will flush later.
	    wlen = write_eintr(ip->bw_fd, outbuf, len);
	    vim_free(outbuf);
	    أرجع (wlen < len) ? FAIL : OK;
	}
# endif
    }
#جيڪڏهن_پورو

    wlen = write_eintr(ip->bw_fd, buf, len);
    أرجع (wlen < len) ? FAIL : OK;
}

/*
 * Check modification time of file, before writing to it.
 * The size isn't checked, because using a tool like "gzip" takes care of
 * using the same timestamp but can't set the size.
 */
    ساكن صحيح
check_mtime(buf_T *buf, stat_T *st)
{
    إذا (buf->b_mtime_read != 0
		  && وقت_differs(st, buf->b_mtime_read, buf->b_mtime_read_ns))
    {
	نياپو_scroll = TRUE;	    // don't overwrite messages here
	نياپو_silent = 0;		    // must give this prompt
	// don't use emsg() here, don't want to flush the buffers
	نياپو_attr(_("WARNING: The file has been changed since reading it!!!"),
						       HL_ATTR(HLF_E));
	إذا (ask_yesno((char_u *)_("Do you really want to write to it"),
								 TRUE) == 'n')
	    أرجع FAIL;
	نياپو_scroll = FALSE;	    // always overwrite the file message now
    }
    أرجع OK;
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
    إذا (flags == FIO_LATIN1 || flags == 0)
	أرجع 0;

    إذا (flags == FIO_UTF8)	// UTF-8
    {
	buf[0] = 0xef;
	buf[1] = 0xbb;
	buf[2] = 0xbf;
	أرجع 3;
    }
    p = buf;
    (فراغ)ucs2bytes(0xfeff, &p, flags);
    أرجع (صحيح)(p - buf);
}

#جيڪڏهن_وصف_ٿيل UNIX
    ساكن فراغ
سيٽ_file_وقت(
    char_u  *fname,
    time_t  atime,	    // access time
    time_t  mtime)	    // modification time
{
# إذا defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
    بنية utimbuf  buf;

    buf.actime	= atime;
    buf.modtime	= mtime;
    (فراغ)utime((حرف *)fname, &buf);
# وإلا
#  إذا defined(HAVE_UTIMES)
    بنية timeval  tvp[2];

    tvp[0].tv_sec   = atime;
    tvp[0].tv_usec  = 0;
    tvp[1].tv_sec   = mtime;
    tvp[1].tv_usec  = 0;
    (فراغ)utimes((حرف *)fname, (ثابت بنية timeval *)&tvp);
#  endif
# endif
}
#جيڪڏهن_پورو // UNIX

    حرف *
new_file_پيغام(فراغ)
{
    أرجع shortmess(SHM_NEW) ? _("[New]") : _("[New File]");
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
    linenr_T	    شروعات,
    linenr_T	    پڇاڙي,
    exarg_T	    *eap,		// for forced 'ff' and 'fenc', can be
					// NULL!
    صحيح		    append,		// append to the file
    صحيح		    forceit,
    صحيح		    reset_بدليو,
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
    ڊگهو	    nchars;
    char_u	    *errmsg = عدم;
    صحيح		    errmsg_allocated = FALSE;
    char_u	    *errnum = عدم;
    char_u	    *بفر;
    char_u	    smallbuf[SMALLBUFSIZE];
    char_u	    *backup_ext;
    صحيح		    bufsize;
    ڊگهو	    perm;		    // file permissions
    صحيح		    retval = OK;
    صحيح		    newfile = FALSE;	    // TRUE if file doesn't exist yet
    صحيح		    نياپو_save = نياپو_scroll;
    صحيح		    overwriting;	    // TRUE if writing over original
    صحيح		    no_eol = FALSE;	    // no end-of-line written
    صحيح		    device = FALSE;	    // writing to a device
    stat_T	    st_old;
    صحيح		    prev_got_int = got_int;
    صحيح		    checking_conversion;
    صحيح		    file_readonly = FALSE;  // overwritten file is read-only
#إذا defined(UNIX)			    // XXX fix me sometime?
    صحيح		    made_writable = FALSE;  // 'w' bit has been set
#جيڪڏهن_پورو
					// writing everything
    صحيح		    سڄو = (شروعات == 1 && پڇاڙي == buf->b_ml.ml_سٽ_ڳڻپ);
    linenr_T	    old_سٽ_ڳڻپ = buf->b_ml.ml_سٽ_ڳڻپ;
    صحيح		    attr;
    صحيح		    fileformat;
    صحيح		    write_bin;
    بنية bw_info  write_info;		// info for buf_write_bytes()
    صحيح		    converted = FALSE;
    صحيح		    notconverted = FALSE;
    char_u	    *fenc;		// effective 'fileencoding'
    char_u	    *fenc_tofree = عدم; // allocated "fenc"
    صحيح		    wb_flags = 0;
#جيڪڏهن_وصف_ٿيل HAVE_ACL
    vim_acl_T	    acl = عدم;		// ACL copied from original file to
					// backup or new file
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_PERSISTENT_UNDO
    صحيح		    write_undo_file = FALSE;
    context_sha256_T sha_ctx;
#جيڪڏهن_پورو
    بنانشان صحيح    bkc = get_bkc_flags(buf);
    pos_T	    orig_شروعات = buf->b_op_شروعات;
    pos_T	    orig_پڇاڙي = buf->b_op_پڇاڙي;

    إذا (fname == عدم || *fname == NUL)	// safety check
	أرجع FAIL;
    إذا (buf->b_ml.ml_mfp == عدم)
    {
	// This can happen during startup when there is a stray "w" in the
	// vimrc file.
	emsg(_(e_خالي_جايون_بفر));
	أرجع FAIL;
    }

    // Disallow writing from .exrc and .vimrc in current directory for
    // security reasons.
    إذا (check_secure())
	أرجع FAIL;

    // Avoid a crash for a long name.
    إذا (STRLEN(fname) >= MAXPATHL)
    {
	emsg(_(e_name_too_long));
	أرجع FAIL;
    }

    // must init bw_conv_buf and bw_iconv_fd before jumping to "fail"
    write_info.bw_conv_buf = عدم;
    write_info.bw_conv_error = FALSE;
    write_info.bw_conv_error_lnum = 0;
    write_info.bw_restlen = 0;
#جيڪڏهن_وصف_ٿيل USE_ICONV
    write_info.bw_iconv_fd = (iconv_t)-1;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
    write_info.bw_بفر = buf;
    write_info.bw_پڄاڻي = FALSE;
#جيڪڏهن_پورو

    // After writing a file changedtick changes but we don't want to display
    // the line.
    ex_no_reprint = TRUE;

    // If there is no file name yet, use the one for the written file.
    // BF_NOTEDITED is set to reflect this (in case the write fails).
    // Don't do this when the write is for a filter command.
    // Don't do this when appending.
    // Only do this when 'cpoptions' contains the 'F' flag.
    إذا (buf->b_ffname == عدم
	    && reset_بدليو
	    && سڄو
	    && buf == curbuf
	    && !bt_nofilename(buf)
	    && !filtering
	    && (!append || vim_strchr(p_cpo, CPO_FNAMEAPP) != عدم)
	    && vim_strchr(p_cpo, CPO_FNAMEW) != عدم)
    {
	إذا (سيٽ_rw_fname(fname, sfname) == FAIL)
	    أرجع FAIL;
	buf = curbuf;	    // just in case autocmds made "buf" invalid
    }

    إذا (sfname == عدم)
	sfname = fname;
    // For Unix: Use the short file name whenever possible.
    // Avoids problems with networks and when directory names are changed.
    // Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
    // another directory, which we don't detect
    ffname = fname;			    // remember full fname
#جيڪڏهن_وصف_ٿيل UNIX
    fname = sfname;
#جيڪڏهن_پورو

    إذا (buf->b_ffname != عدم && fnamecmp(ffname, buf->b_ffname) == 0)
	overwriting = TRUE;
    وإلا
	overwriting = FALSE;

    إذا (exiting)
	settmode(TMODE_COOK);	    // when exiting allow typeahead now

    ++no_wait_return;		    // don't wait for return yet

    // Set '[ and '] marks to the lines to be written.
    buf->b_op_شروعات.lnum = شروعات;
    buf->b_op_شروعات.کانو = 0;
    buf->b_op_پڇاڙي.lnum = پڇاڙي;
    buf->b_op_پڇاڙي.کانو = 0;

    {
	aco_save_T	aco;
	صحيح		buf_ffname = FALSE;
	صحيح		buf_sfname = FALSE;
	صحيح		buf_fname_f = FALSE;
	صحيح		buf_fname_s = FALSE;
	صحيح		did_cmd = FALSE;
	صحيح		nofile_err = FALSE;
	صحيح		خالي_جايون_memline = (buf->b_ml.ml_mfp == عدم);
	bufref_T	bufref;

	// Apply PRE autocommands.
	// Set curbuf to the buffer to be written.
	// Careful: The autocommands may call buf_write() recursively!
	إذا (ffname == buf->b_ffname)
	    buf_ffname = TRUE;
	إذا (sfname == buf->b_sfname)
	    buf_sfname = TRUE;
	إذا (fname == buf->b_ffname)
	    buf_fname_f = TRUE;
	إذا (fname == buf->b_sfname)
	    buf_fname_s = TRUE;

	// Set curwin/curbuf to buf and save a few things.
	aucmd_prepbuf(&aco, buf);
	إذا (curbuf != buf)
	{
	    // Could not find a window for "buf".  Doing more might cause
	    // problems, better bail out.
	    أرجع FAIL;
	}

	سيٽ_bufref(&bufref, buf);

	إذا (append)
	{
	    إذا (!(did_cmd = apply_autocmds_exarg(EVENT_FILEAPPENDCMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		إذا (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		وإلا
		    apply_autocmds_exarg(EVENT_FILEAPPENDPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	وإلا إذا (filtering)
	{
	    apply_autocmds_exarg(EVENT_FILTERWRITEPRE,
					    عدم, sfname, FALSE, curbuf, eap);
	}
	وإلا إذا (reset_بدليو && سڄو)
	{
	    صحيح was_بدليو = curbuf_Is_بدليو();

	    did_cmd = apply_autocmds_exarg(EVENT_BUFWRITECMD,
					  sfname, sfname, FALSE, curbuf, eap);
	    إذا (did_cmd)
	    {
		إذا (was_بدليو && !curbuf_Is_بدليو())
		{
		    // Written everything correctly and BufWriteCmd has reset
		    // 'modified': Correct the undo information so that an
		    // undo now sets 'modified'.
		    u_unchanged(curbuf);
		    u_update_save_nr(curbuf);
		}
	    }
	    وإلا
	    {
		إذا (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		وإلا
		    apply_autocmds_exarg(EVENT_BUFWRITEPRE,
					  sfname, sfname, FALSE, curbuf, eap);
	    }
	}
	وإلا
	{
	    إذا (!(did_cmd = apply_autocmds_exarg(EVENT_FILEWRITECMD,
					 sfname, sfname, FALSE, curbuf, eap)))
	    {
		إذا (overwriting && bt_nofilename(curbuf))
		    nofile_err = TRUE;
		وإلا
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
	إذا (!bufref_valid(&bufref))
	    buf = عدم;
	إذا (buf == عدم || (buf->b_ml.ml_mfp == عدم && !خالي_جايون_memline)
				       || did_cmd || nofile_err
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
				       || aborting()
#جيڪڏهن_پورو
				       )
	{
	    إذا (buf != عدم && (cmdmod.cmod_flags & CMOD_LOCKMARKS))
	    {
		// restore the original '[ and '] positions
		buf->b_op_شروعات = orig_شروعات;
		buf->b_op_پڇاڙي = orig_پڇاڙي;
	    }

	    --no_wait_return;
	    نياپو_scroll = نياپو_save;
	    إذا (nofile_err)
		semsg(_(e_no_matching_autocommands_for_buftype_str_بفر),
							       curbuf->b_p_bt);

	    إذا (nofile_err
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		    || aborting()
#جيڪڏهن_پورو
		    )
		// An aborting error, interrupt or exception in the
		// autocommands.
		أرجع FAIL;
	    إذا (did_cmd)
	    {
		إذا (buf == عدم)
		    // The buffer was deleted.  We assume it was written
		    // (can't retry anyway).
		    أرجع OK;
		إذا (overwriting)
		{
		    // Assume the buffer was written, update the timestamp.
		    ml_timestamp(buf);
		    إذا (append)
			buf->b_flags &= ~BF_NEW;
		    وإلا
			buf->b_flags &= ~BF_WRITE_نقاب;
		}
		إذا (reset_بدليو && buf->b_بدليو && !append
			&& (overwriting || vim_strchr(p_cpo, CPO_PLUS) != عدم))
		    // Buffer still changed, the autocommands didn't work
		    // properly.
		    أرجع FAIL;
		أرجع OK;
	    }
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    إذا (!aborting())
#جيڪڏهن_پورو
		emsg(_(e_autocommands_deleted_or_unloaded_بفر_منزل_be_written));
	    أرجع FAIL;
	}

	// The autocommands may have changed the number of lines in the file.
	// When writing the whole file, adjust the end.
	// When writing part of the file, assume that the autocommands only
	// changed the number of lines that are to be written (tricky!).
	إذا (buf->b_ml.ml_سٽ_ڳڻپ != old_سٽ_ڳڻپ)
	{
	    إذا (سڄو)						// write all
		پڇاڙي = buf->b_ml.ml_سٽ_ڳڻپ;
	    وإلا إذا (buf->b_ml.ml_سٽ_ڳڻپ > old_سٽ_ڳڻپ)	// more lines
		پڇاڙي += buf->b_ml.ml_سٽ_ڳڻپ - old_سٽ_ڳڻپ;
	    وإلا						// less lines
	    {
		پڇاڙي -= old_سٽ_ڳڻپ - buf->b_ml.ml_سٽ_ڳڻپ;
		إذا (پڇاڙي < شروعات)
		{
		    --no_wait_return;
		    نياپو_scroll = نياپو_save;
		    emsg(_(e_autocommands_بدليو_عدد_جو_سٽون_اندريون_unexpected_way));
		    أرجع FAIL;
		}
	    }
	}

	// The autocommands may have changed the name of the buffer, which may
	// be kept in fname, ffname and sfname.  A ":cd" may also have dropped
	// the short name, then b_fname is the full name.
	إذا (buf_ffname)
	    ffname = buf->b_ffname;
	إذا (buf_sfname)
	    sfname = buf->b_fname;
	إذا (buf_fname_f)
	    fname = buf->b_ffname;
	إذا (buf_fname_s)
	    fname = buf->b_fname;
    }

    إذا (cmdmod.cmod_flags & CMOD_LOCKMARKS)
    {
	// restore the original '[ and '] positions
	buf->b_op_شروعات = orig_شروعات;
	buf->b_op_پڇاڙي = orig_پڇاڙي;
    }

#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
    إذا (netbeans_سرگرم() && is_Netbeans_بفر(buf))
    {
	إذا (سڄو)
	{
	    // b_changed can be 0 after an undo, but we still need to write
	    // the buffer to NetBeans.
	    إذا (buf->b_بدليو || isNetbeansModified(buf))
	    {
		--no_wait_return;		// may wait for return now
		نياپو_scroll = نياپو_save;
		netbeans_save_بفر(buf);	// no error checking...
		أرجع retval;
	    }
	    وإلا
	    {
		errnum = (char_u *)"E656: ";
		errmsg = (char_u *)_(e_netbeans_disallows_writes_جو_unmodified_buffers);
		بفر = عدم;
		اذهب fail;
	    }
	}
	وإلا
	{
	    errnum = (char_u *)"E657: ";
	    errmsg = (char_u *)_(e_partial_writes_disallowed_for_netbeans_buffers);
	    بفر = عدم;
	    اذهب fail;
	}
    }
#جيڪڏهن_پورو

    إذا (shortmess(SHM_OVER) && !exiting)
	نياپو_scroll = FALSE;	    // overwrite previous file message
    وإلا
	نياپو_scroll = TRUE;	    // don't overwrite previous file message
    إذا (!filtering)
	filemess(buf,
#جيڪڏهن_وصف_نه_ٿيل UNIX
		sfname,
#وإلا
		fname,
#جيڪڏهن_پورو
		    (char_u *)"", 0);	// show that we are busy
    نياپو_scroll = FALSE;		    // always overwrite the file message now

    بفر = alloc(WRITEBUFSIZE);
    إذا (بفر == عدم)		    // can't allocate big buffer, use small
				    // one (to be able to write when out of
				    // memory)
    {
	بفر = smallbuf;
	bufsize = SMALLBUFSIZE;
    }
    وإلا
	bufsize = WRITEBUFSIZE;

    // Get information about original file (if there is one).
#إذا defined(UNIX)
    st_old.st_dev = 0;
    st_old.st_ino = 0;
    perm = -1;
    إذا (mch_stat((حرف *)fname, &st_old) < 0)
	newfile = TRUE;
    وإلا
    {
	perm = st_old.st_گهڻائي;
	إذا (!S_ISREG(st_old.st_گهڻائي))		// not a file
	{
	    إذا (S_ISDIR(st_old.st_گهڻائي))
	    {
		errnum = (char_u *)"E502: ";
		errmsg = (char_u *)_(e_is_a_directory);
		اذهب fail;
	    }
	    إذا (mch_nodetype(fname) != ڳنڍ_WRITABLE)
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
#وإلا // !UNIX
    // Check for a writable device name.
    c = mch_nodetype(fname);
    إذا (c == ڳنڍ_OTHER)
    {
	errnum = (char_u *)"E503: ";
	errmsg = (char_u *)_(e_is_not_file_or_writable_device);
	اذهب fail;
    }
    إذا (c == ڳنڍ_WRITABLE)
    {
# إذا defined(MSWIN)
	// MS-Windows allows opening a device, but we will probably get stuck
	// trying to write to it.
	إذا (!p_odev)
	{
	    errnum = (char_u *)"E796: ";
	    errmsg = (char_u *)_(e_writing_منزل_device_disabled_with_opendevice_option);
	    اذهب fail;
	}
# endif
	device = TRUE;
	newfile = TRUE;
	perm = -1;
    }
    وإلا
    {
	perm = mch_getperm(fname);
	إذا (perm < 0)
	    newfile = TRUE;
	وإلا إذا (mch_isdir(fname))
	{
	    errnum = (char_u *)"E502: ";
	    errmsg = (char_u *)_(e_is_a_directory);
	    اذهب fail;
	}
	إذا (overwriting)
	    (فراغ)mch_stat((حرف *)fname, &st_old);
    }
#جيڪڏهن_پورو // !UNIX

    إذا (!device && !newfile)
    {
	// Check if the file is really writable (when renaming the file to
	// make a backup we won't discover it later).
	file_readonly = check_file_readonly(fname, (صحيح)perm);

	إذا (!forceit && file_readonly)
	{
	    إذا (vim_strchr(p_cpo, CPO_FWRITE) != عدم)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_اندريون_cpoptions);
	    }
	    وإلا
	    {
		errnum = (char_u *)"E505: ";
		errmsg = (char_u *)_(e_is_read_only_جوڙيو_bang_منزل_override);
	    }
	    اذهب fail;
	}

	// Check if the timestamp hasn't changed since reading the file.
	إذا (overwriting)
	{
	    retval = check_mtime(buf, &st_old);
	    إذا (retval == FAIL)
		اذهب fail;
	}
    }

#جيڪڏهن_وصف_ٿيل HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    إذا (!newfile)
	acl = mch_get_acl(fname);
#جيڪڏهن_پورو

    // If 'backupskip' is not empty, don't make a backup for some files.
    dobackup = (p_wb || p_bk || *p_pm != NUL);
    إذا (dobackup && *p_bsk != NUL && ميلاپ_file_list(p_bsk, sfname, ffname))
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
    إذا (!(append && *p_pm == NUL) && !filtering && perm >= 0 && dobackup)
    {
#إذا defined(UNIX) || defined(MSWIN)
	stat_T	    st;
#جيڪڏهن_پورو

	إذا ((bkc & BKC_YES) || append)	// "yes"
	    backup_copy = TRUE;
#إذا defined(UNIX) || defined(MSWIN)
	وإلا إذا ((bkc & BKC_AUTO))	// "auto"
	{
# ifdef UNIX
	    // Don't rename the file when:
	    // - it's a hard link
	    // - it's a symbolic link
	    // - we don't have write permission in the directory
	    // - we can't set the owner/group of the new file
	    إذا (st_old.st_nlink > 1
		    || mch_lstat((حرف *)fname, &st) < 0
		    || st.st_dev != st_old.st_dev
		    || st.st_ino != st_old.st_ino
#  ifndef HAVE_FCHOWN
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid
#  endif
		    )
		backup_copy = TRUE;
	    وإلا
# وإلا
#  ifdef MSWIN
	    // On NTFS file systems hard links are possible.
	    إذا (mch_is_linked(fname))
		backup_copy = TRUE;
	    وإلا
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
		لكل (i = 4913; ; i += 123)
		{
		    vim_snprintf((حرف *)tmp_fname + dirlen,
			حجم(tmp_fname) - dirlen, ".%d.swp", i);
		    إذا (mch_lstat((حرف *)tmp_fname, &st) < 0)
		    {
			fd = mch_open((حرف *)tmp_fname,
				    O_CREAT|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
			إذا (fd < 0 && errno == EEXIST)
			    // If the same file name is created by another
			    // process between lstat() and open(), find another
			    // name.
			    استمر;
			اكسر;
		    }
		}
		إذا (fd < 0)	// can't write in directory
		    backup_copy = TRUE;
		وإلا
		{
# ifdef UNIX
#  ifdef HAVE_FCHOWN
		    vim_ignored = fchown(fd, st_old.st_uid, st_old.st_gid);
#  endif
#  ifdef HAVE_FCHMOD
		    (فراغ)mch_fsetperm(fd, perm);
#  وإلا
		    (فراغ)mch_setperm(tmp_fname, perm);
#  endif
		    إذا (mch_stat((حرف *)tmp_fname, &st) < 0
			    || st.st_uid != st_old.st_uid
			    || st.st_gid != st_old.st_gid
			    || (ڊگهو)st.st_گهڻائي != perm)
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

			لكل (try = 0; try < 10; ++try)
			{
			    إذا (mch_lstat((حرف *)tmp_fname, &st) < 0)
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
	إذا ((bkc & BKC_BREAKSYMLINK) || (bkc & BKC_BREAKHARDLINK))
	{
# ifdef UNIX
	    صحيح	lstat_res;

	    lstat_res = mch_lstat((حرف *)fname, &st);

	    // Symlinks.
	    إذا ((bkc & BKC_BREAKSYMLINK)
		    && lstat_res == 0
		    && st.st_ino != st_old.st_ino)
		backup_copy = FALSE;

	    // Hardlinks.
	    إذا ((bkc & BKC_BREAKHARDLINK)
		    && st_old.st_nlink > 1
		    && (lstat_res != 0 || st.st_ino == st_old.st_ino))
		backup_copy = FALSE;
# وإلا
#  إذا defined(MSWIN)
	    // Symlinks.
	    إذا ((bkc & BKC_BREAKSYMLINK) && mch_is_symbolic_link(fname))
		backup_copy = FALSE;

	    // Hardlinks.
	    إذا ((bkc & BKC_BREAKHARDLINK) && mch_is_hard_link(fname))
		backup_copy = FALSE;
#  endif
# endif
	}

#جيڪڏهن_پورو

	// make sure we have a valid backup extension to use
	إذا (*p_bex == NUL)
	    backup_ext = (char_u *)".bak";
	وإلا
	    backup_ext = p_bex;

	إذا (backup_copy
		&& (fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
	{
	    صحيح		bfd;
	    char_u	*copybuf;
	    صحيح		some_error = FALSE;
	    stat_T	st_new;
	    char_u	*dirp;
	    char_u	*rootname;
#إذا defined(UNIX)
	    صحيح		did_سيٽ_shortname;
	    mode_t	umask_save;
#جيڪڏهن_پورو

	    copybuf = alloc(WRITEBUFSIZE + 1);
	    إذا (copybuf == عدم)
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
	    طالما (*dirp)
	    {
		char_u	*p اڻ_استعمال_ٿيل;
		صحيح	copybuf_len اڻ_استعمال_ٿيل;

#جيڪڏهن_وصف_ٿيل UNIX
		st_new.st_ino = 0;
		st_new.st_dev = 0;
		st_new.st_gid = 0;
#جيڪڏهن_پورو

		// Isolate one directory name, using an entry in 'bdir'.
		copybuf_len = copy_option_part(&dirp, copybuf, WRITEBUFSIZE, ",");

#إذا defined(UNIX) || defined(MSWIN)
		p = copybuf + copybuf_len;
		إذا (after_pathsep(copybuf, p) && p[-1] == p[-2])
		    // Ends with '//', use full path
		    إذا ((p = make_percent_swname(copybuf, p, fname)) != عدم)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#جيڪڏهن_پورو
		rootname = get_file_اندريون_dir(fname, copybuf);
		إذا (rootname == عدم)
		{
		    some_error = TRUE;	    // out of memory
		    اذهب nobackup;
		}

#إذا defined(UNIX)
		did_سيٽ_shortname = FALSE;
#جيڪڏهن_پورو

		// May try twice if 'shortname' not set.
		لكل (;;)
		{
		    // Make the backup file name.
		    إذا (backup == عدم)
			backup = buf_modname((buf->b_p_sn || buf->b_shortname),
						 rootname, backup_ext, FALSE);
		    إذا (backup == عدم)
		    {
			vim_free(rootname);
			some_error = TRUE;		// out of memory
			اذهب nobackup;
		    }

		    // Check if backup file already exists.
		    إذا (mch_stat((حرف *)backup, &st_new) >= 0)
		    {
#جيڪڏهن_وصف_ٿيل UNIX
			// Check if backup file is same as original file.
			// May happen when modname() gave the same file back.
			// E.g. silly link, or file name-length reached.
			// If we don't check here, we either ruin the file
			// when copying or erase it after writing. jw.
			إذا (st_new.st_dev == st_old.st_dev
					    && st_new.st_ino == st_old.st_ino)
			{
			    VIM_CLEAR(backup);	// no backup file to delete
			    // may try again with 'shortname' set
			    إذا (!(buf->b_shortname || buf->b_p_sn))
			    {
				buf->b_shortname = true;
				did_سيٽ_shortname = TRUE;
				استمر;
			    }
				// setting shortname didn't help
			    إذا (did_سيٽ_shortname)
				buf->b_shortname = false;
			    اكسر;
			}
#جيڪڏهن_پورو

			// If we are not going to keep the backup file, don't
			// delete an existing one, try to use another name.
			// Change one character, just before the extension.
			إذا (!p_bk)
			{
			    char_u	*wp;

			    wp = backup + STRLEN(backup) - 1
							 - STRLEN(backup_ext);
			    إذا (wp < backup)	// empty file name ???
				wp = backup;
			    *wp = 'z';
			    طالما (*wp > 'a'
				    && mch_stat((حرف *)backup, &st_new) >= 0)
				--*wp;
			    // They all exist??? Must be something wrong.
			    إذا (*wp == 'a')
				VIM_CLEAR(backup);
			}
		    }
		    اكسر;
		}
		vim_free(rootname);

		// Try to create the backup file
		إذا (backup != عدم)
		{
		    // remove old backup, if present
		    mch_remove(backup);
		    // Open with O_EXCL to avoid the file being created while
		    // we were sleeping (symlink hacker attack?). Reset umask
		    // if possible to avoid mch_setperm() below.
#جيڪڏهن_وصف_ٿيل UNIX
		    umask_save = umask(0);
#جيڪڏهن_پورو
		    bfd = mch_open((حرف *)backup,
				O_WRONLY|O_CREAT|O_EXTRA|O_EXCL|O_NOFOLLOW,
								 perm & 0777);
#جيڪڏهن_وصف_ٿيل UNIX
		    (فراغ)umask(umask_save);
#جيڪڏهن_پورو
		    إذا (bfd < 0)
			VIM_CLEAR(backup);
		    وإلا
		    {
			// Set file protection same as original file, but
			// strip s-bit.  Only needed if umask() wasn't used
			// above.
#جيڪڏهن_وصف_نه_ٿيل UNIX
			(فراغ)mch_setperm(backup, perm & 0777);
#وإلا
			// Try to set the group of the backup same as the
			// original file. If this fails, set the protection
			// bits for the group same as the protection bits for
			// others.
			إذا (st_new.st_gid != st_old.st_gid
# ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
				&& fchown(bfd, (uid_t)-1, st_old.st_gid) != 0
# endif
						)
			    mch_setperm(backup,
					  (perm & 0707) | ((perm & 07) << 3));
# إذا defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
# endif
# ifdef FEAT_XATTR
			mch_copy_xattr(fname, backup);
# endif
#جيڪڏهن_پورو

			// copy the file.
			write_info.bw_fd = bfd;
			write_info.bw_buf = copybuf;
			write_info.bw_flags = FIO_NOCONVERT;
			طالما ((write_info.bw_len = read_eintr(fd, copybuf,
							    WRITEBUFSIZE)) > 0)
			{
			    إذا (buf_write_بائيٽون(&write_info) == FAIL)
			    {
				errmsg = (char_u *)_(e_cant_write_منزل_backup_file_جوڙيو_bang_منزل_override);
				اكسر;
			    }
			    ui_breakcheck();
			    إذا (got_int)
			    {
				errmsg = (char_u *)_(e_interrupted);
				اكسر;
			    }
			}

			إذا (close(bfd) < 0 && errmsg == عدم)
			    errmsg = (char_u *)_(e_close_error_for_backup_file_جوڙيو_bang_منزل_write_anyway);
			إذا (write_info.bw_len < 0)
			    errmsg = (char_u *)_(e_cant_read_file_for_backup_جوڙيو_bang_منزل_write_anyway);
#جيڪڏهن_وصف_ٿيل UNIX
			سيٽ_file_وقت(backup, st_old.st_atime, st_old.st_mtime);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ACL
			mch_سيٽ_acl(backup, acl);
#جيڪڏهن_پورو
#إذا defined(HAVE_SELINUX) || defined(HAVE_SMACK)
			mch_copy_sec(fname, backup);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_XATTR
			mch_copy_xattr(fname, backup);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MSWIN
			(فراغ)mch_copy_file_attribute(fname, backup);
#جيڪڏهن_پورو
			اكسر;
		    }
		}
	    }
    nobackup:
	    close(fd);		// ignore errors for closing read file
	    vim_free(copybuf);

	    إذا (backup == عدم && errmsg == عدم)
		errmsg = (char_u *)_(e_cannot_create_backup_file_جوڙيو_bang_منزل_write_anyway);
	    // ignore errors when forceit is TRUE
	    إذا ((some_error || errmsg != عدم) && !forceit)
	    {
		retval = FAIL;
		اذهب fail;
	    }
	    errmsg = عدم;
	}
	وإلا
	{
	    char_u	*dirp;
	    char_u	*p;
	    char_u	*rootname;

	    // Make a backup by renaming the original file.

	    // If 'cpoptions' includes the "W" flag, we don't want to
	    // overwrite a read-only file.  But rename may be possible
	    // anyway, thus we need an extra check here.
	    إذا (file_readonly && vim_strchr(p_cpo, CPO_FWRITE) != عدم)
	    {
		errnum = (char_u *)"E504: ";
		errmsg = (char_u *)_(e_is_read_only_cannot_override_W_اندريون_cpoptions);
		اذهب fail;
	    }

	    // Form the backup file name - change path/fo.o.h to
	    // path/fo.o.h.bak Try all directories in 'backupdir', first one
	    // that works is used.
	    dirp = p_bdir;
	    طالما (*dirp)
	    {
		صحيح IObufflen اڻ_استعمال_ٿيل;

		// Isolate one directory name and make the backup file name.
		IObufflen = copy_option_part(&dirp, IObuff, IOSIZE, ",");

#إذا defined(UNIX) || defined(MSWIN)
		p = IObuff + IObufflen;
		إذا (after_pathsep(IObuff, p) && p[-1] == p[-2])
		    // path ends with '//', use full path
		    إذا ((p = make_percent_swname(IObuff, p, fname)) != عدم)
		    {
			backup = modname(p, backup_ext, FALSE);
			vim_free(p);
		    }
#جيڪڏهن_پورو
		إذا (backup == عدم)
		{
		    rootname = get_file_اندريون_dir(fname, IObuff);
		    إذا (rootname == عدم)
			backup = عدم;
		    وإلا
		    {
			backup = buf_modname(
				(buf->b_p_sn || buf->b_shortname),
						rootname, backup_ext, FALSE);
			vim_free(rootname);
		    }
		}

		إذا (backup != عدم)
		{
		    // If we are not going to keep the backup file, don't
		    // delete an existing one, try to use another name.
		    // Change one character, just before the extension.
		    إذا (!p_bk && mch_getperm(backup) >= 0)
		    {
			p = backup + STRLEN(backup) - 1 - STRLEN(backup_ext);
			إذا (p < backup)	// empty file name ???
			    p = backup;
			*p = 'z';
			طالما (*p > 'a' && mch_getperm(backup) >= 0)
			    --*p;
			// They all exist??? Must be something wrong!
			إذا (*p == 'a')
			    VIM_CLEAR(backup);
		    }
		}
		إذا (backup != عدم)
		{
		    // Delete any existing backup and move the current version
		    // to the backup.	For safety, we don't remove the backup
		    // until the write has finished successfully. And if the
		    // 'backup' option is set, leave it around.

		    // If the renaming of the original file to the backup file
		    // works, quit here.
		    إذا (vim_rename(fname, backup) == 0)
			اكسر;

		    VIM_CLEAR(backup);   // don't do the rename below
		}
	    }
	    إذا (backup == عدم && !forceit)
	    {
		errmsg = (char_u *)_(e_cant_make_backup_file_جوڙيو_bang_منزل_write_anyway);
		اذهب fail;
	    }
	}
    }

#إذا defined(UNIX)
    // When using ":w!" and the file was read-only: make it writable
    إذا (forceit && perm >= 0 && !(perm & 0200) && st_old.st_uid == getuid()
				     && vim_strchr(p_cpo, CPO_FWRITE) == عدم)
    {
	perm |= 0200;
	(فراغ)mch_setperm(fname, perm);
	made_writable = TRUE;
    }
#جيڪڏهن_پورو

    // When using ":w!" and writing to the current file, 'readonly' makes no
    // sense, reset it, unless 'Z' appears in 'cpoptions'.
    إذا (forceit && overwriting && vim_strchr(p_cpo, CPO_KEEPRO) == عدم)
    {
	buf->b_p_ro = FALSE;
	need_maketitle = TRUE;	    // set window title later
	حالت_redraw_all();	    // redraw status lines later
    }

    إذا (پڇاڙي > buf->b_ml.ml_سٽ_ڳڻپ)
	پڇاڙي = buf->b_ml.ml_سٽ_ڳڻپ;
    إذا (buf->b_ml.ml_flags & ML_خالي_جايون)
	شروعات = پڇاڙي + 1;

    // If the original file is being overwritten, there is a small chance that
    // we crash in the middle of writing. Therefore the file is preserved now.
    // This makes all block numbers positive so that recovery does not need
    // the original file.
    // Don't do this if there is a backup file and we are exiting.
    إذا (reset_بدليو && !newfile && overwriting
					      && !(exiting && backup != عدم))
    {
	ml_preserve(buf, FALSE);
	إذا (got_int)
	{
	    errmsg = (char_u *)_(e_interrupted);
	    اذهب restore_backup;
	}
    }

#جيڪڏهن_وصف_ٿيل VMS
    vms_remove_نسخو(fname); // remove version
#جيڪڏهن_پورو
    // Default: write the file directly.  May write to a temp file for
    // multi-byte conversion.
    wfname = fname;

    // Check for forced 'fileencoding' from "++opt=val" argument.
    إذا (eap != عدم && eap->force_enc != 0)
    {
	fenc = eap->cmd + eap->force_enc;
	fenc = enc_canonize(fenc);
	fenc_tofree = fenc;
    }
    وإلا
	fenc = buf->b_p_fenc;

    // Check if the file needs to be converted.
    converted = need_conversion(fenc);

    // Check if UTF-8 to UCS-2/4 or Latin1 conversion needs to be done.  Or
    // Latin1 to Unicode conversion.  This is handled in buf_write_bytes().
    // Prepare the flags for it and allocate bw_conv_buf when needed.
    إذا (converted && (enc_utf8 || STRCMP(p_enc, "latin1") == 0))
    {
	wb_flags = get_fio_flags(fenc);
	إذا (wb_flags & (FIO_UCS2 | FIO_UCS4 | FIO_UTF16 | FIO_UTF8))
	{
	    // overallocate a bit, in case we read incomplete multi-byte chars
	    صحيح سائيز = bufsize + CONV_RESTLEN;
	    // Need to allocate a buffer to translate into.
	    إذا (wb_flags & (FIO_UCS2 | FIO_UTF16 | FIO_UTF8))
		write_info.bw_conv_buflen = سائيز * 2;
	    وإلا // FIO_UCS4
		write_info.bw_conv_buflen = سائيز * 4;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    إذا (write_info.bw_conv_buf == عدم)
		پڇاڙي = 0;
	}
    }

#جيڪڏهن_وصف_ٿيل MSWIN
    إذا (converted && wb_flags == 0 && (wb_flags = get_win_fio_flags(fenc)) != 0)
    {
	// Convert UTF-8 -> UCS-2 and UCS-2 -> DBCS.  Worst-case * 4:
	write_info.bw_conv_buflen = bufsize * 4;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	إذا (write_info.bw_conv_buf == عدم)
	    پڇاڙي = 0;
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل MACOS_CONVERT
    إذا (converted && wb_flags == 0 && (wb_flags = get_mac_fio_flags(fenc)) != 0)
    {
	write_info.bw_conv_buflen = bufsize * 3;
	write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	إذا (write_info.bw_conv_buf == عدم)
	    پڇاڙي = 0;
    }
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL) || defined(USE_ICONV)
    إذا (converted && wb_flags == 0)
    {
# ifdef USE_ICONV
	// Use iconv() conversion when conversion is needed and it's not done
	// internally.
	write_info.bw_iconv_fd = (iconv_t)my_iconv_open(fenc,
					enc_utf8 ? (char_u *)"utf-8" : p_enc);
	إذا (write_info.bw_iconv_fd != (iconv_t)-1)
	{
	    // We're going to use iconv(), allocate a buffer to convert in.
	    write_info.bw_conv_buflen = bufsize * ICONV_MULT;
	    write_info.bw_conv_buf = alloc(write_info.bw_conv_buflen);
	    إذا (write_info.bw_conv_buf == عدم)
		پڇاڙي = 0;
	    write_info.bw_پهريون = TRUE;
	}
#  ifdef FEAT_EVAL
	وإلا
#  endif
# endif

# ifdef FEAT_EVAL
	    // When the file needs to be converted with 'charconvert' after
	    // writing, write to a temp file instead and let the conversion
	    // overwrite the original file.
	    إذا (*p_ccv != NUL)
	    {
		wfname = vim_tempname('w', FALSE);
		إذا (wfname == عدم)	// Can't write without a tempfile!
		{
		    errmsg = (char_u *)_(e_cant_ڳوليو_عارضي_file_for_writing);
		    اذهب restore_backup;
		}
	    }
# endif
    }
#جيڪڏهن_پورو
    إذا (converted && wb_flags == 0
#جيڪڏهن_وصف_ٿيل USE_ICONV
	    && write_info.bw_iconv_fd == (iconv_t)-1
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    && wfname == fname
#جيڪڏهن_پورو
	    )
    {
	إذا (!forceit)
	{
	    errmsg = (char_u *)_(e_cannot_convert_جوڙيو_bang_منزل_write_without_conversion);
	    اذهب restore_backup;
	}
	notconverted = TRUE;
    }

    // If conversion is taking place, we may first pretend to write and check
    // for conversion errors.  Then loop again to write for real.
    // When not doing conversion this writes for real right away.
    لكل (checking_conversion = TRUE; ; checking_conversion = FALSE)
    {
	// There is no need to check conversion when:
	// - there is no conversion
	// - we make a backup file, that can be restored in case of conversion
	//   failure.
	إذا (!converted || dobackup)
	    checking_conversion = FALSE;

	إذا (checking_conversion)
	{
	    // Make sure we don't write anything.
	    fd = -1;
	    write_info.bw_fd = fd;
	}
	وإلا
	{
#جيڪڏهن_وصف_ٿيل HAVE_FTRUNCATE
# define TRUNC_ON_OPEN 0
#وإلا
# define TRUNC_ON_OPEN O_TRUNC
#جيڪڏهن_پورو
	    // Open the file "wfname" for writing.
	    // We may try to open the file twice: If we can't write to the file
	    // and forceit is TRUE we delete the existing file and try to
	    // create a new one. If this still fails we may have lost the
	    // original file!  (this may happen when the user reached his
	    // quotum for number of files).
	    // Appending will fail if the file does not exist and forceit is
	    // FALSE.
	    طالما ((fd = mch_open((حرف *)wfname, O_WRONLY | O_EXTRA | (append
				? (forceit ? (O_APPEND | O_CREAT) : O_APPEND)
				: (O_CREAT | TRUNC_ON_OPEN))
				, perm < 0 ? 0666 : (perm & 0777))) < 0)
	    {
		// A forced write will try to create a new file if the old one
		// is still readonly. This may also happen when the directory
		// is read-only. In that case the mch_remove() will fail.
		إذا (errmsg == عدم)
		{
#جيڪڏهن_وصف_ٿيل UNIX
		    stat_T	st;

		    // Don't delete the file when it's a hard or symbolic link.
		    إذا ((!newfile && st_old.st_nlink > 1)
			    || (mch_lstat((حرف *)fname, &st) == 0
				&& (st.st_dev != st_old.st_dev
				    || st.st_ino != st_old.st_ino)))
			errmsg =
			      (char_u *)_(e_cant_open_linked_file_for_writing);
		    وإلا
#جيڪڏهن_پورو
		    {
			errmsg = (char_u *)_(e_cant_open_file_for_writing);
			إذا (forceit && vim_strchr(p_cpo, CPO_FWRITE) == عدم
								  && perm >= 0)
			{
#جيڪڏهن_وصف_ٿيل UNIX
			    // we write to the file, thus it should be marked
			    // writable after all
			    إذا (!(perm & 0200))
				made_writable = TRUE;
			    perm |= 0200;
			    إذا (st_old.st_uid != getuid()
						  || st_old.st_gid != getgid())
				perm &= 0777;
#جيڪڏهن_پورو
			    إذا (!append)  // don't remove when appending
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
		    إذا (backup != عدم && wfname == fname)
		    {
			إذا (backup_copy)
			{
			    // There is a small chance that we removed the
			    // original, try to move the copy in its place.
			    // This may not work if the vim_rename() fails.
			    // In that case we leave the copy around.

			    // If file does not exist, put the copy in its
			    //رکو
			    إذا (mch_stat((حرف *)fname, &st) < 0)
				vim_rename(backup, fname);
			    // if original file does exist throw away the copy
			    إذا (mch_stat((حرف *)fname, &st) >= 0)
				mch_remove(backup);
			}
			وإلا
			{
			    // try to put the original file back
			    vim_rename(backup, fname);
			}
		    }

		    // if original file no longer exists give an extra warning
		    إذا (!newfile && mch_stat((حرف *)fname, &st) < 0)
			پڇاڙي = 0;
		}

		إذا (wfname != fname)
		    vim_free(wfname);
		اذهب fail;
	    }
	    write_info.bw_fd = fd;

#إذا defined(UNIX)
	    {
		stat_T	st;

		// Double check we are writing the intended file before making
		// any changes.
		إذا (overwriting
			&& (!dobackup || backup_copy)
			&& fname == wfname
			&& perm >= 0
			&& mch_fstat(fd, &st) == 0
			&& st.st_ino != st_old.st_ino)
		{
		    close(fd);
		    errmsg = (char_u *)_(e_file_بدليو_while_writing);
		    اذهب fail;
		}
	    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_FTRUNCATE
	    إذا (!append)
		vim_ignored = ftruncate(fd, (off_t)0);
#جيڪڏهن_پورو

#إذا defined(MSWIN)
	    إذا (backup != عدم && overwriting && !append)
		(فراغ)mch_copy_file_attribute(backup, wfname);

	    إذا (!overwriting && !append)
	    {
		إذا (buf->b_ffname != عدم)
		    (فراغ)mch_copy_file_attribute(buf->b_ffname, wfname);
		// Should copy resource fork
	    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
	    إذا (*buf->b_p_ڪنجي != NUL && !filtering)
	    {
		char_u		*سرو;
		صحيح		سرو_len;

		buf->b_cryptstate = crypt_create_for_writing(
						      crypt_get_طريقو_nr(buf),
					   buf->b_p_ڪنجي, &سرو, &سرو_len);
		إذا (buf->b_cryptstate == عدم || سرو == عدم)
		    پڇاڙي = 0;
		وإلا
		{
		    // Write magic number, so that Vim knows how this file is
		    // encrypted when reading it back.
		    write_info.bw_buf = سرو;
		    write_info.bw_len = سرو_len;
		    write_info.bw_flags = FIO_NOCONVERT;
		    إذا (buf_write_بائيٽون(&write_info) == FAIL)
			پڇاڙي = 0;
		    wb_flags |= FIO_ENCRYPTED;
		    vim_free(سرو);
		}
	    }
#جيڪڏهن_پورو
	}
	errmsg = عدم;

	write_info.bw_buf = بفر;
	nchars = 0;

	// use "++bin", "++nobin" or 'binary'
	إذا (eap != عدم && eap->force_bin != 0)
	    write_bin = (eap->force_bin == FORCE_BIN);
	وإلا
	    write_bin = buf->b_p_bin;

	// The BOM is written just after the encryption magic number.
	// Skip it when appending and the file already existed, the BOM only
	// makes sense at the start of the file.
	إذا (buf->b_p_bomb && !write_bin && (!append || perm < 0))
	{
	    write_info.bw_len = make_bom(بفر, fenc);
	    إذا (write_info.bw_len > 0)
	    {
		// don't convert, do encryption
		write_info.bw_flags = FIO_NOCONVERT | wb_flags;
		إذا (buf_write_بائيٽون(&write_info) == FAIL)
		    پڇاڙي = 0;
		وإلا
		    nchars += write_info.bw_len;
	    }
	}
	write_info.bw_شروعات_lnum = شروعات;

#جيڪڏهن_وصف_ٿيل FEAT_PERSISTENT_UNDO
	write_undo_file = (buf->b_p_udf
			    && overwriting
			    && !append
			    && !filtering
# ifdef CRYPT_NOT_INPLACE
			    // writing undo file requires
			    // crypt_encode_inplace()
			    && (buf->b_cryptstate == عدم
				|| crypt_هلي_inplace(buf->b_cryptstate))
# endif
			    && reset_بدليو
			    && !checking_conversion);
# ifdef CRYPT_NOT_INPLACE
	// remove undo file if encrypting it is not possible
	إذا (buf->b_p_udf
		&& overwriting
		&& !append
		&& !filtering
		&& !checking_conversion
		&& buf->b_cryptstate != عدم
		&& !crypt_هلي_inplace(buf->b_cryptstate))
	    u_undofile_reset_and_delete(buf);
# endif
	إذا (write_undo_file)
	    // Prepare for computing the hash value of the text.
	    sha256_شروعات(&sha_ctx);
#جيڪڏهن_پورو

	write_info.bw_len = bufsize;
	write_info.bw_flags = wb_flags;
	fileformat = get_fileformat_force(buf, eap);
	s = بفر;
	len = 0;
	لكل (lnum = شروعات; lnum <= پڇاڙي; ++lnum)
	{
	    // The next while loop is done once for each character written.
	    // Keep it fast!
	    ptr = ml_get_buf(buf, lnum, FALSE) - 1;
#جيڪڏهن_وصف_ٿيل FEAT_PERSISTENT_UNDO
	    إذا (write_undo_file)
		sha256_update(&sha_ctx, ptr + 1,
					      (UINT32_T)(STRLEN(ptr + 1) + 1));
#جيڪڏهن_پورو
	    طالما ((c = *++ptr) != NUL)
	    {
		إذا (c == NL)
		    *s = NUL;		// replace newlines with NULs
		وإلا إذا (c == CAR && fileformat == EOL_MAC)
		    *s = NL;		// Mac: replace CRs with NLs
		وإلا
		    *s = c;
		++s;
		إذا (++len != bufsize)
		    استمر;
#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
		إذا (write_info.bw_fd > 0 && lnum == پڇاڙي
			&& (write_info.bw_flags & FIO_ENCRYPTED)
			&& *buf->b_p_ڪنجي != NUL && !filtering
			&& *ptr == NUL)
		    write_info.bw_پڄاڻي = TRUE;
#جيڪڏهن_پورو
		إذا (buf_write_بائيٽون(&write_info) == FAIL)
		{
		    پڇاڙي = 0;		// write error: break loop
		    اكسر;
		}
		nchars += bufsize;
		s = بفر;
		len = 0;
		write_info.bw_شروعات_lnum = lnum;
	    }
	    // write failed or last line has no EOL: stop here
	    إذا (پڇاڙي == 0
		    || (lnum == پڇاڙي
			&& (write_bin || !buf->b_p_fixeol)
			&& ((write_bin && lnum == buf->b_no_eol_lnum)
			    || (lnum == buf->b_ml.ml_سٽ_ڳڻپ
							   && !buf->b_p_eol))))
	    {
		++lnum;			// written the line, count it
		no_eol = TRUE;
		اكسر;
	    }
	    إذا (fileformat == EOL_UNIX)
		*s++ = NL;
	    وإلا
	    {
		*s++ = CAR;		    // EOL_MAC or EOL_DOS: write CR
		إذا (fileformat == EOL_DOS)  // write CR-NL
		{
		    إذا (++len == bufsize)
		    {
			إذا (buf_write_بائيٽون(&write_info) == FAIL)
			{
			    پڇاڙي = 0;	// write error: break loop
			    اكسر;
			}
			nchars += bufsize;
			s = بفر;
			len = 0;
		    }
		    *s++ = NL;
		}
	    }
	    إذا (++len == bufsize && پڇاڙي)
	    {
		إذا (buf_write_بائيٽون(&write_info) == FAIL)
		{
		    پڇاڙي = 0;		// write error: break loop
		    اكسر;
		}
		nchars += bufsize;
		s = بفر;
		len = 0;

		ui_breakcheck();
		إذا (got_int)
		{
		    پڇاڙي = 0;		// Interrupted, break loop
		    اكسر;
		}
	    }
#جيڪڏهن_وصف_ٿيل VMS
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
	    إذا (buf->b_fab_rfm == FAB$C_VFC
		    || ((buf->b_fab_rat & (FAB$M_FTN | FAB$M_CR)) != 0))
	    {
		صحيح b2write;

		buf->b_fab_mrs = (buf->b_fab_mrs == 0
			? MIN(4096, bufsize)
			: MIN(buf->b_fab_mrs, bufsize));

		b2write = len;
		طالما (b2write > 0)
		{
		    write_info.bw_len = MIN(b2write, buf->b_fab_mrs);
		    إذا (buf_write_بائيٽون(&write_info) == FAIL)
		    {
			پڇاڙي = 0;
			اكسر;
		    }
		    b2write -= MIN(b2write, buf->b_fab_mrs);
		}
		write_info.bw_len = bufsize;
		nchars += len;
		s = بفر;
		len = 0;
	    }
#جيڪڏهن_پورو
	}
	إذا (len > 0 && پڇاڙي > 0)
	{
	    write_info.bw_len = len;
#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
	    إذا (write_info.bw_fd > 0 && lnum >= پڇاڙي
		    && (write_info.bw_flags & FIO_ENCRYPTED)
		    && *buf->b_p_ڪنجي != NUL && !filtering)
		write_info.bw_پڄاڻي = TRUE;
#جيڪڏهن_پورو
	    إذا (buf_write_بائيٽون(&write_info) == FAIL)
		پڇاڙي = 0;		    // write error
	    nchars += len;
	}

	إذا (!buf->b_p_fixeol && buf->b_p_eof)
	{
	    // write trailing CTRL-Z
	    (فراغ)write_eintr(write_info.bw_fd, "\x1a", 1);
	    nchars++;
	}

	// Stop when writing done or an error was encountered.
	إذا (!checking_conversion || پڇاڙي == 0)
	    اكسر;

	// If no error happened until now, writing should be ok, so loop to
	// really write the buffer.
    }

    // If we started writing, finish writing. Also when an error was
    // encountered.
    إذا (!checking_conversion)
    {
#إذا defined(UNIX) && defined(HAVE_FSYNC)
	// On many journaling file systems there is a bug that causes both the
	// original and the backup file to be lost when halting the system
	// right after writing the file.  That's because only the meta-data is
	// journalled.  Syncing the file slows down the system, but assures it
	// has been written to disk and we don't lose it.
	// For a device do try the fsync() but don't complain if it does not
	// work (could be a pipe).
	// If the 'fsync' option is FALSE, don't fsync().  Useful for laptops.
	إذا ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && vim_fsync(fd) != 0
		&& !device)
	{
	    errmsg = (char_u *)_(e_fsync_failed);
	    پڇاڙي = 0;
	}
#جيڪڏهن_پورو

#إذا defined(HAVE_SELINUX) || defined(HAVE_SMACK) || defined(FEAT_XATTR)
	// Probably need to set the security context.
	إذا (!backup_copy)
	{
# إذا defined(HAVE_SELINUX) || defined(HAVE_SMACK)
	    mch_copy_sec(backup, wfname);
# endif
# ifdef FEAT_XATTR
	    mch_copy_xattr(backup, wfname);
# endif
	}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل UNIX
	// When creating a new file, set its owner/group to that of the
	// original file.  Get the new device and inode number.
	إذا (backup != عدم && !backup_copy)
	{
# ifdef HAVE_FCHOWN
	    stat_T	st;

	    // Don't change the owner when it's already OK, some systems remove
	    // permission or ACL stuff.
	    إذا (mch_stat((حرف *)wfname, &st) < 0
		    || st.st_uid != st_old.st_uid
		    || st.st_gid != st_old.st_gid)
	    {
		// changing owner might not be possible
		vim_ignored = fchown(fd, st_old.st_uid, -1);
		// if changing group fails clear the group permissions
		إذا (fchown(fd, -1, st_old.st_gid) == -1 && perm > 0)
		    perm &= ~070;
	    }
# endif
	    buf_setino(buf);
	}
	وإلا إذا (!buf->b_dev_valid)
	    // Set the inode when creating a new file.
	    buf_setino(buf);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل UNIX
	إذا (made_writable)
	    perm &= ~0200;	// reset 'w' bit for security reasons
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_FCHMOD
	// set permission of new file same as old file
	إذا (perm >= 0)
	    (فراغ)mch_fsetperm(fd, perm);
#جيڪڏهن_پورو
	إذا (close(fd) != 0)
	{
	    errmsg = (char_u *)_(e_close_failed);
	    پڇاڙي = 0;
	}

#جيڪڏهن_وصف_نه_ٿيل HAVE_FCHMOD
	// set permission of new file same as old file
	إذا (perm >= 0)
	    (فراغ)mch_setperm(wfname, perm);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ACL
	// Probably need to set the ACL before changing the user (can't set the
	// ACL on a file the user doesn't own).
	// On Solaris, with ZFS and the aclmode property set to "discard" (the
	// default), chmod() discards all part of a file's ACL that don't
	// represent the mode of the file.  It's non-trivial for us to discover
	// whether we're in that situation, so we simply always re-set the ACL.
# ifndef HAVE_SOLARIS_ZFS_ACL
	إذا (!backup_copy)
# endif
	    mch_سيٽ_acl(wfname, acl);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
	إذا (buf->b_cryptstate != عدم)
	{
	    crypt_free_state(buf->b_cryptstate);
	    buf->b_cryptstate = عدم;
	}
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL)
	إذا (wfname != fname)
	{
	    // The file was written to a temp file, now it needs to be
	    // converted with 'charconvert' to (overwrite) the output file.
	    إذا (پڇاڙي != 0)
	    {
		إذا (eval_charconvert(enc_utf8 ? (char_u *)"utf-8" : p_enc,
						  fenc, wfname, fname) == FAIL)
		{
		    write_info.bw_conv_error = TRUE;
		    پڇاڙي = 0;
		}
	    }
	    mch_remove(wfname);
	    vim_free(wfname);
	}
#جيڪڏهن_پورو
    }

    إذا (پڇاڙي == 0)
    {
	// Error encountered.
	إذا (errmsg == عدم)
	{
	    إذا (write_info.bw_conv_error)
	    {
		إذا (write_info.bw_conv_error_lnum == 0)
		    errmsg = (char_u *)_(e_write_error_conversion_failed_make_fenc_خالي_جايون_منزل_override);
		وإلا
		{
		    errmsg_allocated = TRUE;
		    errmsg = alloc(300);
		    إذا (errmsg == عدم)
			اذهب fail;
		    vim_snprintf((حرف *)errmsg, 300, _(e_write_error_conversion_failed_اندريون_سٽ_nr_make_fenc_خالي_جايون_منزل_override),
					 (ڊگهو)write_info.bw_conv_error_lnum);
		}
	    }
	    وإلا إذا (got_int)
		errmsg = (char_u *)_(e_interrupted);
	    وإلا
		errmsg = (char_u *)_(e_write_error_file_system_ڀريل);
	}

	// If we have a backup file, try to put it in place of the new file,
	// because the new file is probably corrupt.  This avoids losing the
	// original file when trying to make a backup when writing the file a
	// second time.
	// When "backup_copy" is set we need to copy the backup over the new
	// file.  Otherwise rename the backup file.
	// If this is OK, don't give the extra warning message.
	إذا (backup != عدم)
	{
	    إذا (backup_copy)
	    {
		// This may take a while, if we were interrupted let the user
		// know we got the message.
		إذا (got_int)
		{
		    نياپو(_(e_interrupted));
		    ٻاهريون_flush();
		}
		إذا ((fd = mch_open((حرف *)backup, O_RDONLY | O_EXTRA, 0)) >= 0)
		{
		    إذا ((write_info.bw_fd = mch_open((حرف *)fname,
				    O_WRONLY | O_CREAT | O_TRUNC | O_EXTRA,
							   perm & 0777)) >= 0)
		    {
			// copy the file.
			write_info.bw_buf = smallbuf;
			write_info.bw_flags = FIO_NOCONVERT;
			طالما ((write_info.bw_len = read_eintr(fd, smallbuf,
						      SMALLBUFSIZE)) > 0)
			    إذا (buf_write_بائيٽون(&write_info) == FAIL)
				اكسر;

			إذا (close(write_info.bw_fd) >= 0
						   && write_info.bw_len == 0)
			    پڇاڙي = 1;		// success
		    }
		    close(fd);	// ignore errors for closing read file
		}
	    }
	    وإلا
	    {
		إذا (vim_rename(backup, fname) == 0)
		    پڇاڙي = 1;
	    }
	}
	اذهب fail;
    }

    lnum -= شروعات;	    // compute number of written lines
    --no_wait_return;	    // may wait for return now

#إذا !(defined(UNIX) || defined(VMS))
    fname = sfname;	    // use shortname now, for the messages
#جيڪڏهن_پورو
    إذا (!filtering)
    {
	نياپو_جوڙيو_fname(buf, fname);	// put fname in IObuff with quotes
	c = FALSE;
	إذا (write_info.bw_conv_error)
	{
	    STRCAT(IObuff, _(" CONVERSION ERROR"));
	    c = TRUE;
	    إذا (write_info.bw_conv_error_lnum != 0)
		vim_snprintf_جوڙيو((حرف *)IObuff, IOSIZE, _(" in line %ld;"),
			(ڊگهو)write_info.bw_conv_error_lnum);
	}
	وإلا إذا (notconverted)
	{
	    STRCAT(IObuff, _("[NOT converted]"));
	    c = TRUE;
	}
	وإلا إذا (converted)
	{
	    STRCAT(IObuff, _("[converted]"));
	    c = TRUE;
	}
	إذا (device)
	{
	    STRCAT(IObuff, _("[Device]"));
	    c = TRUE;
	}
	وإلا إذا (newfile)
	{
	    STRCAT(IObuff, new_file_پيغام());
	    c = TRUE;
	}
	إذا (no_eol)
	{
	    نياپو_جوڙيو_eol();
	    c = TRUE;
	}
	// may add [unix/dos/mac]
	إذا (نياپو_جوڙيو_fileformat(fileformat))
	    c = TRUE;
#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
	إذا (wb_flags & FIO_ENCRYPTED)
	{
	    crypt_append_نياپو(buf);
	    c = TRUE;
	}
#جيڪڏهن_پورو
	نياپو_جوڙيو_سٽون(c, (ڊگهو)lnum, nchars);	// add line/char count
	إذا (!shortmess(SHM_WRITE))
	{
	    إذا (append)
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [a]") : _(" appended"));
	    وإلا
		STRCAT(IObuff, shortmess(SHM_WRI) ? _(" [w]") : _(" written"));
	}

	سيٽ_keep_نياپو((char_u *)نياپو_trunc_attr((حرف *)IObuff, FALSE, 0), 0);
    }

    // When written everything correctly: reset 'modified'.  Unless not
    // writing to the original file and '+' is not in 'cpoptions'.
    إذا (reset_بدليو && سڄو && !append
	    && !write_info.bw_conv_error
	    && (overwriting || vim_strchr(p_cpo, CPO_PLUS) != عدم))
    {
	unchanged(buf, TRUE, FALSE);
	// b:changedtick may be incremented in unchanged() but that should not
	// trigger a TextChanged event.
	إذا (buf->b_آخري_changedtick + 1 == CHANGEDTICK(buf))
	    buf->b_آخري_changedtick = CHANGEDTICK(buf);
	u_unchanged(buf);
	u_update_save_nr(buf);
    }

    // If written to the current file, update the timestamp of the swap file
    // and reset the BF_WRITE_MASK flags. Also sets buf->b_mtime.
    إذا (overwriting)
    {
	ml_timestamp(buf);
	إذا (append)
	    buf->b_flags &= ~BF_NEW;
	وإلا
	    buf->b_flags &= ~BF_WRITE_نقاب;
    }

    // If we kept a backup until now, and we are in patch mode, then we make
    // the backup file our 'original' file.
    إذا (*p_pm && dobackup)
    {
	حرف *org = (حرف *)buf_modname((buf->b_p_sn || buf->b_shortname),
							  fname, p_pm, FALSE);

	إذا (backup != عدم)
	{
	    stat_T	st;

	    // If the original file does not exist yet
	    // the current backup file becomes the original file
	    إذا (org == عدم)
		emsg(_(e_patchmode_cant_save_original_file));
	    وإلا إذا (mch_stat(org, &st) < 0)
	    {
		vim_rename(backup, (char_u *)org);
		VIM_CLEAR(backup);	    // don't delete the file
#جيڪڏهن_وصف_ٿيل UNIX
		سيٽ_file_وقت((char_u *)org, st_old.st_atime, st_old.st_mtime);
#جيڪڏهن_پورو
	    }
	}
	// If there is no backup file, remember that a (new) file was
	// created.
	وإلا
	{
	    صحيح خالي_جايون_fd;

	    إذا (org == عدم
		    || (خالي_جايون_fd = mch_open(org,
				      O_CREAT | O_EXTRA | O_EXCL | O_NOFOLLOW,
					perm < 0 ? 0666 : (perm & 0777))) < 0)
	      emsg(_(e_patchmode_cant_touch_خالي_جايون_original_file));
	    وإلا
	      close(خالي_جايون_fd);
	}
	إذا (org != عدم)
	{
	    mch_setperm((char_u *)org, mch_getperm(fname) & 0777);
	    vim_free(org);
	}
    }

    // Remove the backup unless 'backup' option is set or there was a
    // conversion error.
    إذا (!p_bk && backup != عدم && !write_info.bw_conv_error
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
    إذا (بفر != smallbuf)
	vim_free(بفر);
    vim_free(fenc_tofree);
    vim_free(write_info.bw_conv_buf);
#جيڪڏهن_وصف_ٿيل USE_ICONV
    إذا (write_info.bw_iconv_fd != (iconv_t)-1)
    {
	iconv_close(write_info.bw_iconv_fd);
	write_info.bw_iconv_fd = (iconv_t)-1;
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ACL
    mch_free_acl(acl);
#جيڪڏهن_پورو

    إذا (errmsg != عدم)
    {
	صحيح numlen = errnum != عدم ? (صحيح)STRLEN(errnum) : 0;

	attr = HL_ATTR(HLF_E);	// set highlight for error messages
	نياپو_جوڙيو_fname(buf,
#جيڪڏهن_وصف_نه_ٿيل UNIX
		sfname
#وإلا
		fname
#جيڪڏهن_پورو
		     );		// put file name in IObuff with quotes
	إذا (STRLEN(IObuff) + STRLEN(errmsg) + numlen >= IOSIZE)
	    IObuff[IOSIZE - STRLEN(errmsg) - numlen - 1] = NUL;
	// If the error message has the form "is ...", put the error number in
	// front of the file name.
	إذا (errnum != عدم)
	{
	    STRMOVE(IObuff + numlen, IObuff);
	    mch_memmove(IObuff, errnum, (size_t)numlen);
	}
	STRCAT(IObuff, errmsg);
	emsg((حرف *)IObuff);
	إذا (errmsg_allocated)
	    vim_free(errmsg);

	retval = FAIL;
	إذا (پڇاڙي == 0)
	{
	    نياپو_puts_attr(_("\nWARNING: Original file may be lost or damaged\n"),
		    attr | نياپو_HIST);
	    نياپو_puts_attr(_("don't quit the editor until the file is successfully written!"),
		    attr | نياپو_HIST);

	    // Update the timestamp to avoid an "overwrite changed file"
	    // prompt when writing again.
	    إذا (mch_stat((حرف *)fname, &st_old) >= 0)
	    {
		buf_store_وقت(buf, &st_old, fname);
		buf->b_mtime_read = buf->b_mtime;
		buf->b_mtime_read_ns = buf->b_mtime_ns;
	    }
	}
    }
    نياپو_scroll = نياپو_save;

#جيڪڏهن_وصف_ٿيل FEAT_PERSISTENT_UNDO
    // When writing the whole file and 'undofile' is set, also write the undo
    // file.
    إذا (retval == OK && write_undo_file)
    {
	char_u	    hash[UNDO_HASH_سائيز];

	sha256_پڄاڻي(&sha_ctx, hash);
	u_write_undo(عدم, FALSE, buf, hash);
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (!should_abort(retval))
#وإلا
    إذا (!got_int)
#جيڪڏهن_پورو
    {
	aco_save_T	aco;

	curbuf->b_no_eol_lnum = 0;  // in case it was set by the previous read

	// Apply POST autocommands.
	// Careful: The autocommands may call buf_write() recursively!
	// Only do this when a window was found for "buf".
	aucmd_prepbuf(&aco, buf);
	إذا (curbuf == buf)
	{
	    إذا (append)
		apply_autocmds_exarg(EVENT_FILEAPPENDPOST, fname, fname,
							   FALSE, curbuf, eap);
	    وإلا إذا (filtering)
		apply_autocmds_exarg(EVENT_FILTERWRITEPOST, عدم, fname,
							   FALSE, curbuf, eap);
	    وإلا إذا (reset_بدليو && سڄو)
		apply_autocmds_exarg(EVENT_BUFWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);
	    وإلا
		apply_autocmds_exarg(EVENT_FILEWRITEPOST, fname, fname,
							   FALSE, curbuf, eap);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (aborting())	    // autocmds may abort script processing
	    retval = FALSE;
#جيڪڏهن_پورو
    }

#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
    // Make sure marks will be written out to the viminfo file later, even when
    // the file is new.
    curbuf->b_marks_read = true;
#جيڪڏهن_پورو

    got_int |= prev_got_int;

    أرجع retval;
}
