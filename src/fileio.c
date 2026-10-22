/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * fileio.c: read from and write to a file
 */

#شامل "vim.h"

#اگر defined(__TANDEM)
# include <limits.h>		// for SSIZE_MAX
#اگر_ختم
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_EVAL)
# include <pwd.h>
# include <grp.h>
#اگر_ختم
#اگر defined(VMS) && defined(HAVE_XOS_R_H)
# include <x11/xos_r.h>
#اگر_ختم

// Is there any system that doesn't have access()?
#تعریف_کرو USE_MCH_ACCESS

// Bitmask with 0x80 set in each byte of a long_u word, used to detect
// non-ASCII bytes (high bit set) in multiple bytes at once.
#تعریف_کرو NONASCII_نقاب (((long_u)-1 / 0xFF) * 0x80)

#اگر defined(__hpux) && !defined(HAVE_DIRFD)
# define dirfd(x) ((x)->__dd_fd)
# define HAVE_DIRFD
#اگر_ختم

ساکن char_u *اگلا_fenc(char_u **pp, صحیح *alloced);
#اگر_تعریف_شدہ FEAT_EVAL
ساکن char_u *readfile_charconvert(char_u *fname, char_u *fenc, صحیح *fdp);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CRYPT
ساکن char_u *check_for_cryptkey(char_u *cryptkey, char_u *ptr, طویل *sizep, off_T *filesizep, صحیح newfile, char_u *fname, صحیح *did_ask);
#اگر_ختم
ساکن linenr_T readfile_linenr(linenr_T linecnt, char_u *p, char_u *endp);
ساکن char_u *check_for_bom(char_u *p, طویل سائز, صحیح *lenp, صحیح flags);

#اگر_تعریف_شدہ FEAT_EVAL
ساکن صحیح readdirex_ترتیب_دو;
#اگر_ختم

    خالی
filemess(
    buf_T	*buf,
    char_u	*name,
    char_u	*s,
    صحیح		attr)
{
    صحیح		پیغام_scroll_save;
    صحیح		پچھلا_پیغام_ستون = پیغام_ستون;
    size_t	len;

    اگر (پیغام_silent != 0)
	واپس;
    پیغام_جوڑو_fname(buf, name);	    // put file name in IObuff with quotes

    // If it's extremely long, truncate it.
    len = STRLEN(IObuff);
    اگر (len > IOSIZE - 100)
    {
	len = IOSIZE - 100;
	IObuff[len] = NUL;
    }

    // Avoid an over-long translation to cause trouble.
    اگر (*s != NUL)
	STRNCPY(IObuff + len, s, 99);

    /*
     * For the first message may have to start a new line.
     * For further ones overwrite the previous one, reset msg_scroll before
     * calling filemess().
     */
    پیغام_scroll_save = پیغام_scroll;
    اگر (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	پیغام_scroll = FALSE;
    اگر (!پیغام_scroll)	// wait a bit when overwriting an error msg
	check_for_delay(FALSE);
    پیغام_آغاز();
    اگر (پچھلا_پیغام_ستون != 0 && پیغام_ستون == 0)
	پیغام_putchar('\r');  // overwrite any previous message.
    پیغام_scroll = پیغام_scroll_save;
    پیغام_scrolled_ign = TRUE;
    // may truncate the message to avoid a hit-return prompt
    پیغام_outtrans_attr(پیغام_may_trunc(FALSE, IObuff), attr);
    پیغام_clr_eos();
    برآمد_flush();
    پیغام_scrolled_ign = FALSE;
}

/*
 * Read lines from file "fname" into the buffer after line "from".
 *
 * 1. We allocate blocks with lalloc, as big as possible.
 * 2. Each block is filled with characters from the file with a single read().
 * 3. The lines are inserted in the buffer with ml_append().
 *
 * (caller must check that fname != NULL, unless READ_STDIN is used)
 *
 * "lines_to_skip" is the number of lines that must be skipped
 * "lines_to_read" is the number of lines that are appended
 * When not recovering lines_to_skip is 0 and lines_to_read MAXLNUM.
 *
 * flags:
 * READ_NEW	starting to edit a new buffer
 * READ_FILTER	reading filter output
 * READ_STDIN	read from stdin instead of a file
 * READ_BUFFER	read from curbuf instead of a file (converting after reading
 *		stdin)
 * READ_NOFILE	do not read a file, only trigger BufReadCmd
 * READ_DUMMY	read into a dummy buffer (to check if file contents changed)
 * READ_KEEP_UNDO  don't clear undo info or read it from a file
 * READ_FIFO	read from fifo/socket instead of a file
 *
 * return FAIL for failure, NOTDONE for directory (failure), or OK
 */
    صحیح
readfile(
    char_u	*fname,
    char_u	*sfname,
    linenr_T	from,
    linenr_T	سطریں_مقصد_skip,
    linenr_T	سطریں_مقصد_read,
    exarg_T	*eap,			// can be NULL!
    صحیح		flags)
{
    صحیح		retval = FAIL;	// jump to "theend" instead of returning
    صحیح		fd = 0;
    صحیح		newfile = (flags & READ_NEW);
    صحیح		check_readonly;
    صحیح		filtering = (flags & READ_FILTER);
    صحیح		read_stdin = (flags & READ_STDIN);
    صحیح		read_بفر = (flags & READ_بفر);
    صحیح		read_fifo = (flags & READ_FIFO);
    صحیح		سیٹ_options = newfile || read_بفر
					   || (eap != تہی && eap->read_edit);
    linenr_T	read_buf_lnum = 1;	// next line to read from curbuf
    colnr_T	read_buf_ستون = 0;	// next char to read from this line
    char_u	c;
    linenr_T	lnum = from;
    char_u	*ptr = تہی;		// pointer into read buffer
    char_u	*بفر = تہی;		// read buffer
    char_u	*new_بفر = تہی;	// init to shut up gcc
    char_u	*سطر_آغاز = تہی;	// init to shut up gcc
    صحیح		wasempty;		// buffer was empty before reading
    colnr_T	len;
    طویل	سائز = 0;
    char_u	*p;
    off_T	filesize = 0;
    صحیح		skip_read = FALSE;
#اگر_تعریف_شدہ FEAT_CRYPT
    off_T       filesize_disk = 0;      // file size read from disk
    off_T       filesize_گنتی = 0;     //شمار
    char_u	*cryptkey = تہی;
    صحیح		did_ask_for_کلید = FALSE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
    context_sha256_T sha_ctx;
    صحیح		read_undo_file = FALSE;
#اگر_ختم
    صحیح		بانٹو = 0;		// number of split lines
#تعریف_کرو UNKNOWN	 0x0fffffff		// file size is unknown
    linenr_T	linecnt;
    صحیح		error = FALSE;		// errors encountered
    صحیح		ff_error = EOL_UNKNOWN; // file format with errors
    طویل	linerest = 0;		// remaining chars in line
#اگر_تعریف_شدہ UNIX
    صحیح		perm = 0;
    صحیح		تبادلہ_منوال = -1;		// protection bits for swap file
#ورنہ
    صحیح		perm;
#اگر_ختم
    صحیح		fileformat = 0;		// end-of-line format
    صحیح		keep_fileformat = FALSE;
    stat_T	st;
    صحیح		file_readonly;
    linenr_T	skip_گنتی = 0;
    linenr_T	read_گنتی = 0;
    صحیح		پیغام_save = پیغام_scroll;
    linenr_T	read_no_eol_lnum = 0;   // non-zero lnum when last line of
					// last read was missing the eol
    صحیح		try_mac;
    صحیح		try_dos;
    صحیح		try_unix;
    صحیح		file_rewind = FALSE;
    صحیح		can_retry;
    linenr_T	conv_error = 0;		// line nr with conversion error
    linenr_T	illegal_byte = 0;	// line nr with illegal byte
    صحیح		keep_dest_enc = FALSE;	// don't retry when char doesn't fit
					// in destination encoding
    صحیح		bad_char_behavior = BAD_REPLACE;
					// BAD_KEEP, BAD_DROP or character to
					// replace with
    char_u	*tmpname = تہی;	// name of 'charconvert' output file
    صحیح		fio_flags = 0;
    char_u	*fenc;			// fileencoding to use
    صحیح		fenc_alloced;		// fenc_next is in allocated memory
    char_u	*fenc_اگلا = تہی;	// next item in 'fencs' or NULL
    صحیح		advance_fenc = FALSE;
    طویل	real_سائز = 0;
#اگر_تعریف_شدہ USE_ICONV
    iconv_t	iconv_fd = (iconv_t)-1;	// descriptor for iconv() or -1
# ifdef FEAT_EVAL
    صحیح		did_iconv = FALSE;	// TRUE when iconv() failed and trying
					// 'charconvert' next
# endif
#اگر_ختم
    صحیح		converted = FALSE;	// TRUE if conversion done
    صحیح		notconverted = FALSE;	// TRUE if conversion wanted but it
					// wasn't possible
    char_u	conv_rest[CONV_RESTLEN];
    صحیح		conv_restlen = 0;	// nr of bytes in conv_rest[]
    pos_T	orig_آغاز;
    buf_T	*old_curbuf;
    char_u	*old_b_ffname;
    char_u	*old_b_fname;
    صحیح		using_b_ffname;
    صحیح		using_b_fname;
    ساکن حرف *پیغام_is_a_directory = N_("is a directory");
#اگر_تعریف_شدہ FEAT_CRYPT
    صحیح		eof = FALSE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SODIUM
    صحیح		may_need_lseek = FALSE;
#اگر_ختم
    size_t	fnamelen = 0;

    curbuf->b_au_did_filetype = false; // reset before triggering any autocommands

    curbuf->b_no_eol_lnum = 0;	// in case it was set by the previous read

    /*
     * If there is no file name yet, use the one for the read file.
     * BF_NOTEDITED is set to reflect this.
     * Don't do this for a read from a filter.
     * Only do this when 'cpoptions' contains the 'f' flag.
     */
    اگر (curbuf->b_ffname == تہی
	    && !filtering
	    && fname != تہی
	    && vim_strchr(p_cpo, CPO_FNAMER) != تہی
	    && !(flags & READ_DUMMY))
    {
	اگر (سیٹ_rw_fname(fname, sfname) == FAIL)
	    جاؤ theend;
    }

    // Remember the initial values of curbuf, curbuf->b_ffname and
    // curbuf->b_fname to detect whether they are altered as a result of
    // executing nasty autocommands.  Also check if "fname" and "sfname"
    // point to one of these values.
    old_curbuf = curbuf;
    old_b_ffname = curbuf->b_ffname;
    old_b_fname = curbuf->b_fname;
    using_b_ffname = (fname == curbuf->b_ffname)
					      || (sfname == curbuf->b_ffname);
    using_b_fname = (fname == curbuf->b_fname) || (sfname == curbuf->b_fname);

    // After reading a file the cursor line changes but we don't want to
    // display the line.
    ex_no_reprint = TRUE;

    // don't display the file info for another buffer now
    need_fileinfo = FALSE;

    /*
     * For Unix: Use the short file name whenever possible.
     * Avoids problems with networks and when directory names are changed.
     * Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
     * another directory, which we don't detect.
     */
    اگر (sfname == تہی)
	sfname = fname;
#اگر defined(UNIX)
    fname = sfname;
#اگر_ختم

    /*
     * The BufReadCmd and FileReadCmd events intercept the reading process by
     * executing the associated commands instead.
     */
    اگر (!filtering && !read_stdin && !read_بفر)
    {
	orig_آغاز = curbuf->b_op_آغاز;

	// Set '[ mark to the line above where the lines go (line 1 if zero).
	curbuf->b_op_آغاز.lnum = ((from == 0) ? 1 : from);
	curbuf->b_op_آغاز.ستون = 0;

	اگر (newfile)
	{
	    اگر (apply_autocmds_exarg(EVENT_BUFREADCMD, تہی, sfname,
							  FALSE, curbuf, eap))
	    {
		retval = OK;
#اگر_تعریف_شدہ FEAT_EVAL
		اگر (aborting())
		    retval = FAIL;
#اگر_ختم
		// The BufReadCmd code usually uses ":read" to get the text and
		// perhaps ":file" to change the buffer name. But we should
		// consider this to work like ":edit", thus reset the
		// BF_NOTEDITED flag.  Then ":write" will work to overwrite the
		// same file.
		اگر (retval == OK)
		    curbuf->b_flags &= ~BF_NOTEDITED;
		جاؤ theend;
	    }
	}
	ورنہ اگر (apply_autocmds_exarg(EVENT_FILEREADCMD, sfname, sfname,
							    FALSE, تہی, eap))
	{
#اگر_تعریف_شدہ FEAT_EVAL
	    retval = aborting() ? FAIL : OK;
#ورنہ
	    retval = OK;
#اگر_ختم
	    جاؤ theend;
	}

	curbuf->b_op_آغاز = orig_آغاز;

	اگر (flags & READ_NOFILE)
	{
	    // Return NOTDONE instead of FAIL so that BufEnter can be triggered
	    // and other operations don't fail.
	    retval = NOTDONE;
	    جاؤ theend;
	}
    }

    اگر ((shortmess(SHM_OVER) || curbuf->b_help) && p_verbose == 0)
	پیغام_scroll = FALSE;	// overwrite previous file message
    ورنہ
	پیغام_scroll = TRUE;	// don't overwrite previous file message

    اگر (fname != تہی && *fname != NUL)
    {
	fnamelen = STRLEN(fname);

	// If the name is too long we might crash further on, quit here.
	اگر (fnamelen >= MAXPATHL)
	{
	    filemess(curbuf, fname, (char_u *)_("Illegal file name"), 0);
	    پیغام_انتہا();
	    پیغام_scroll = پیغام_save;
	    جاؤ theend;
	}

	// If the name ends in a path separator, we can't open it.  Check here,
	// because reading the file may actually work, but then creating the
	// swap file may destroy it!  Reported on MS-DOS and Win 95.
	اگر (after_pathsep(fname, fname + fnamelen))
	{
	    filemess(curbuf, fname, (char_u *)_(پیغام_is_a_directory), 0);
	    پیغام_انتہا();
	    پیغام_scroll = پیغام_save;
	    retval = NOTDONE;
	    جاؤ theend;
	}
    }
#اگر defined(UNIX) || defined(VMS)
    اگر (!read_stdin && fname != تہی)
	perm = mch_getperm(fname);
#اگر_ختم

    اگر (!read_stdin && !read_بفر && !read_fifo)
    {
#اگر defined(UNIX) || defined(VMS)
	اگر (perm >= 0 && !S_ISREG(perm)		    // not a regular file ...
		      && !S_ISFIFO(perm)	    // ... or fifo
		      && !S_ISSOCK(perm)	    // ... or socket
# ifdef OPEN_CHR_FILES
		      && !(S_ISCHR(perm) && is_dev_fd_file(fname))
			// ... or a character special file named /dev/fd/<n>
# endif
						)
	{
	    /*
	     * On Unix it is possible to read a directory, so we have to
	     * check for it before the mch_open().
	     */
	    اگر (S_ISDIR(perm))
	    {
		filemess(curbuf, fname, (char_u *)_(پیغام_is_a_directory), 0);
		retval = NOTDONE;
	    }
	    ورنہ
		filemess(curbuf, fname, (char_u *)_("is not a file"), 0);
	    پیغام_انتہا();
	    پیغام_scroll = پیغام_save;
	    جاؤ theend;
	}
#اگر_ختم
#اگر defined(MSWIN)
	/*
	 * MS-Windows allows opening a device, but we will probably get stuck
	 * trying to read it.
	 */
	اگر (!p_odev && mch_nodetype(fname) == گرہ_WRITABLE)
	{
	    filemess(curbuf, fname, (char_u *)_("is a device (disabled with 'opendevice' option)"), 0);
	    پیغام_انتہا();
	    پیغام_scroll = پیغام_save;
	    جاؤ theend;
	}
#اگر_ختم
    }

    // Set default or forced 'fileformat' and 'binary'.
    سیٹ_file_options(سیٹ_options, eap);

    /*
     * When opening a new file we take the readonly flag from the file.
     * Default is r/w, can be set to r/o below.
     * Don't reset it when in readonly mode
     * Only set/reset b_p_ro when BF_CHECK_RO is set.
     */
    check_readonly = (newfile && (curbuf->b_flags & BF_CHECK_RO));
    اگر (check_readonly && !readonlymode)
	curbuf->b_p_ro = FALSE;

    اگر (newfile && !read_stdin && !read_بفر && !read_fifo)
    {
	// Remember time of file.
	اگر (mch_stat((حرف *)fname, &st) >= 0)
	{
	    buf_store_وقت(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
#اگر_تعریف_شدہ FEAT_CRYPT
	    filesize_disk = st.st_size;
#اگر_ختم
#اگر_تعریف_شدہ UNIX
	    /*
	     * Use the protection bits of the original file for the swap file.
	     * This makes it possible for others to read the name of the
	     * edited file from the swapfile, but only if they can read the
	     * edited file.
	     * Remove the "write" and "execute" bits for group and others
	     * (they must not write the swapfile).
	     * Add the "read" and "write" bits for the user, otherwise we may
	     * not be able to write to the file ourselves.
	     * Setting the bits is done below, after creating the swap file.
	     */
	    تبادلہ_منوال = (st.st_منوال & 0644) | 0600;
#اگر_ختم
#اگر_تعریف_شدہ VMS
	    curbuf->b_fab_rfm = st.st_fab_rfm;
	    curbuf->b_fab_rat = st.st_fab_rat;
	    curbuf->b_fab_mrs = st.st_fab_mrs;
#اگر_ختم
	}
	ورنہ
	{
	    curbuf->b_mtime = 0;
	    curbuf->b_mtime_ns = 0;
	    curbuf->b_mtime_read = 0;
	    curbuf->b_mtime_read_ns = 0;
	    curbuf->b_orig_سائز = 0;
	    curbuf->b_orig_منوال = 0;
	}

	// Reset the "new file" flag.  It will be set again below when the
	// file doesn't exist.
	curbuf->b_flags &= ~(BF_NEW | BF_NEW_W);
    }

/*
 * for UNIX: check readonly with perm and mch_access()
 * for Amiga: check readonly by trying to open the file for writing
 */
    file_readonly = FALSE;
    اگر (read_stdin)
    {
#اگر defined(MSWIN)
	// Force binary I/O on stdin to avoid CR-LF -> LF conversion.
	setmode(0, O_ثنائی);
#اگر_ختم
    }
    ورنہ اگر (!read_بفر)
    {
#اگر_تعریف_شدہ USE_MCH_ACCESS
	اگر (
# ifdef UNIX
	    !(perm & 0222) ||
# endif
				mch_access((حرف *)fname, W_OK))
	    file_readonly = TRUE;
	fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);
#ورنہ
	اگر (!newfile
		|| readonlymode
		|| (fd = mch_open((حرف *)fname, O_RDWR | O_EXTRA, 0)) < 0)
	{
	    file_readonly = TRUE;
	    // try to open ro
	    fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);
	}
#اگر_ختم
    }

    اگر (fd < 0)			    // cannot open at all
    {
#اگر_غیر_تعریف_شدہ UNIX
	صحیح	isdir_f;
#اگر_ختم
	پیغام_scroll = پیغام_save;
#اگر_غیر_تعریف_شدہ UNIX
	/*
	 * On Amiga we can't open a directory, check here.
	 */
	isdir_f = (mch_isdir(fname));
	perm = mch_getperm(fname);  // check if the file exists
	اگر (isdir_f)
	{
	    filemess(curbuf, sfname, (char_u *)_(پیغام_is_a_directory), 0);
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	}
	ورنہ
#اگر_ختم
	    اگر (newfile)
	    {
		اگر (perm < 0
#اگر_تعریف_شدہ ENOENT
			&& errno == ENOENT
#اگر_ختم
		   )
		{
		    /*
		     * Set the 'new-file' flag, so that when the file has
		     * been created by someone else, a ":w" will complain.
		     */
		    curbuf->b_flags |= BF_NEW;

		    // Create a swap file now, so that other Vims are warned
		    // that we are editing this file.  Don't do this for a
		    // "nofile" or "nowrite" buffer type.
		    اگر (!bt_dontwrite(curbuf))
		    {
			check_need_تبادلہ(newfile);
			// SwapExists autocommand may mess things up
			اگر (curbuf != old_curbuf
				|| (using_b_ffname
					&& (old_b_ffname != curbuf->b_ffname))
				|| (using_b_fname
					 && (old_b_fname != curbuf->b_fname)))
			{
			    emsg(_(e_autocommands_بدلا_بفر_or_بفر_name));
			    جاؤ theend;
			}
		    }
		    اگر (dir_کا_file_exists(fname))
			filemess(curbuf, sfname,
					      (char_u *)new_file_پیغام(), 0);
		    ورنہ
			filemess(curbuf, sfname,
					   (char_u *)_("[New DIRECTORY]"), 0);
#اگر_تعریف_شدہ FEAT_VIMINFO
		    // Even though this is a new file, it might have been
		    // edited before and deleted.  Get the old marks.
		    check_marks_read();
#اگر_ختم
		    // Set forced 'fileencoding'.
		    اگر (eap != تہی)
			سیٹ_forced_fenc(eap);
		    apply_autocmds_exarg(EVENT_BUFNEWFILE, sfname, sfname,
							  FALSE, curbuf, eap);
		    // remember the current fileformat
		    save_file_ff(curbuf);

#اگر defined(FEAT_EVAL)
		    اگر (!aborting())   // autocmds may abort script processing
#اگر_ختم
			retval = OK;	    // a new file is not an error
		    جاؤ theend;
		}
		ورنہ
		{
		    filemess(curbuf, sfname, (char_u *)(
#اگر_تعریف_شدہ EFBIG
			    (errno == EFBIG) ? _("[File too big]") :
#اگر_ختم
#اگر_تعریف_شدہ EOVERFLOW
			    (errno == EOVERFLOW) ? _("[File too big]") :
#اگر_ختم
						_("[Permission Denied]")), 0);
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
		}
	    }

	جاؤ theend;
    }

    /*
     * Only set the 'ro' flag for readonly files the first time they are
     * loaded.	Help files always get readonly mode
     */
    اگر ((check_readonly && file_readonly) || curbuf->b_help)
	curbuf->b_p_ro = TRUE;

    اگر (سیٹ_options)
    {
	// Don't change 'eol' if reading from buffer as it will already be
	// correctly set when reading stdin.
	اگر (!read_بفر)
	{
	    curbuf->b_p_eof = FALSE;
	    curbuf->b_آغاز_eof = FALSE;
	    curbuf->b_p_eol = TRUE;
	    curbuf->b_آغاز_eol = TRUE;
	}
	curbuf->b_p_bomb = FALSE;
	curbuf->b_آغاز_bomb = FALSE;
    }

    // Create a swap file now, so that other Vims are warned that we are
    // editing this file.
    // Don't do this for a "nofile" or "nowrite" buffer type.
    اگر (!bt_dontwrite(curbuf))
    {
	check_need_تبادلہ(newfile);
	اگر (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))))
	{
	    emsg(_(e_autocommands_بدلا_بفر_or_بفر_name));
	    اگر (!read_بفر)
		close(fd);
	    جاؤ theend;
	}
#اگر_تعریف_شدہ UNIX
	// Set swap file protection bits after creating it.
	اگر (تبادلہ_منوال > 0 && curbuf->b_ml.ml_mfp != تہی
			  && curbuf->b_ml.ml_mfp->mf_fname != تہی)
	{
	    char_u *تبادلہ_fname = curbuf->b_ml.ml_mfp->mf_fname;

	    /*
	     * If the group-read bit is set but not the world-read bit, then
	     * the group must be equal to the group of the original file.  If
	     * we can't make that happen then reset the group-read bit.  This
	     * avoids making the swap file readable to more users when the
	     * primary group of the user is too permissive.
	     */
	    اگر ((تبادلہ_منوال & 044) == 040)
	    {
		stat_T	تبادلہ_st;

		اگر (mch_stat((حرف *)تبادلہ_fname, &تبادلہ_st) >= 0
			&& st.st_gid != تبادلہ_st.st_gid
# ifdef HAVE_FCHOWN
			&& fchown(curbuf->b_ml.ml_mfp->mf_fd, -1, st.st_gid)
									  == -1
# endif
		   )
		    تبادلہ_منوال &= 0600;
	    }

	    (خالی)mch_setperm(تبادلہ_fname, (طویل)تبادلہ_منوال);
	}
#اگر_ختم
    }

    // If "Quit" selected at ATTENTION dialog, don't load the file
    اگر (تبادلہ_exists_action == SEA_QUIT)
    {
	اگر (!read_بفر && !read_stdin)
	    close(fd);
	جاؤ theend;
    }

    ++no_wait_return;	    // don't wait for return yet

    /*
     * Set '[ mark to the line above where the lines go (line 1 if zero).
     */
    orig_آغاز = curbuf->b_op_آغاز;
    curbuf->b_op_آغاز.lnum = ((from == 0) ? 1 : from);
    curbuf->b_op_آغاز.ستون = 0;

    try_mac = (vim_strchr(p_ffs, 'm') != تہی);
    try_dos = (vim_strchr(p_ffs, 'd') != تہی);
    try_unix = (vim_strchr(p_ffs, 'x') != تہی);

    اگر (!read_بفر)
    {
	صحیح	m = پیغام_scroll;
	صحیح	n = پیغام_scrolled;

	/*
	 * The file must be closed again, the autocommands may want to change
	 * the file before reading it.
	 */
	اگر (!read_stdin)
	    close(fd);		// ignore errors

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	پیغام_scroll = TRUE;
	اگر (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPRE, تہی, sfname,
							  FALSE, curbuf, eap);
	ورنہ اگر (read_stdin)
	    apply_autocmds_exarg(EVENT_STDINREADPRE, تہی, sfname,
							  FALSE, curbuf, eap);
	ورنہ اگر (newfile)
	    apply_autocmds_exarg(EVENT_BUFREADPRE, تہی, sfname,
							  FALSE, curbuf, eap);
	ورنہ
	    apply_autocmds_exarg(EVENT_FILEREADPRE, sfname, sfname,
							    FALSE, تہی, eap);
	// autocommands may have changed it
	try_mac = (vim_strchr(p_ffs, 'm') != تہی);
	try_dos = (vim_strchr(p_ffs, 'd') != تہی);
	try_unix = (vim_strchr(p_ffs, 'x') != تہی);
	curbuf->b_op_آغاز = orig_آغاز;

	اگر (پیغام_scrolled == n)
	    پیغام_scroll = m;

#اگر_تعریف_شدہ FEAT_EVAL
	اگر (aborting())	    // autocmds may abort script processing
	{
	    --no_wait_return;
	    پیغام_scroll = پیغام_save;
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    جاؤ theend;
	}
#اگر_ختم
	/*
	 * Don't allow the autocommands to change the current buffer.
	 * Try to re-open the file.
	 *
	 * Don't allow the autocommands to change the buffer name either
	 * (cd for example) if it invalidates fname or sfname.
	 */
	اگر (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))
		|| (fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0)) < 0))
	{
	    --no_wait_return;
	    پیغام_scroll = پیغام_save;
	    اگر (fd < 0)
		emsg(_(e_readpre_autocommands_made_file_unreadable));
	    ورنہ
		emsg(_(e_readpre_autocommands_must_not_change_current_بفر));
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    جاؤ theend;
	}
    }

    // Autocommands may add lines to the file, need to check if it is empty
    wasempty = (curbuf->b_ml.ml_flags & ML_خالی_خانے);

    اگر (!recoverymode && !filtering && !(flags & READ_DUMMY))
    {
	/*
	 * Show the user that we are busy reading the input.  Sometimes this
	 * may take a while.  When reading from stdin another program may
	 * still be running, don't move the cursor to the last line, unless
	 * always using the GUI.
	 */
	اگر (read_stdin)
	{
	    اگر (!is_not_a_رکن())
	    {
#اگر_غیر_تعریف_شدہ ALWAYS_USE_GUI
# ifdef VIMDLL
		اگر (!gui.اندر_use)
# endif
		    mch_پیغام(_("Vim: Reading from stdin...\n"));
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
		// Also write a message in the GUI window, if there is one.
		اگر (gui.اندر_use && !gui.dying && !gui.starting)
		{
		    size_t  plen = STRLEN(_("Reading from stdin..."));

		    // make a copy, gui_write() may try to change it
		    p = vim_strnsave((char_u *)_("Reading from stdin..."), plen);
		    اگر (p != تہی)
		    {
			gui_write(p, (صحیح)plen);
			vim_free(p);
		    }
		}
#اگر_ختم
	    }
	}
	ورنہ اگر (!read_بفر)
	    filemess(curbuf, sfname, (char_u *)"", 0);
    }

    پیغام_scroll = FALSE;			// overwrite the file message

    /*
     * Set linecnt now, before the "retry" caused by a wrong guess for
     * fileformat, and after the autocommands, which may change them.
     */
    linecnt = curbuf->b_ml.ml_سطر_گنتی;

    // "++bad=" argument.
    اگر (eap != تہی && eap->bad_char != 0)
    {
	bad_char_behavior = eap->bad_char;
	اگر (سیٹ_options)
	    curbuf->b_bad_char = eap->bad_char;
    }
    ورنہ
	curbuf->b_bad_char = 0;

    /*
     * Decide which 'encoding' to use or use first.
     */
    اگر (eap != تہی && eap->force_enc != 0)
    {
	fenc = enc_canonize(eap->cmd + eap->force_enc);
	fenc_alloced = TRUE;
	keep_dest_enc = TRUE;
    }
    ورنہ اگر (curbuf->b_p_bin)
    {
	fenc = (char_u *)"";		// binary: don't convert
	fenc_alloced = FALSE;
    }
    ورنہ اگر (curbuf->b_help)
    {
	char_u	    firstline[80];
	صحیح	    fc;

	// Help files are either utf-8 or latin1.  Try utf-8 first, if this
	// fails it must be latin1.
	// Always do this when 'encoding' is "utf-8".  Otherwise only do
	// this when needed to avoid [converted] remarks all the time.
	// It is needed when the first line contains non-ASCII characters.
	// That is only in *.??x files.
	fenc = (char_u *)"latin1";
	c = enc_utf8;
	اگر (!c && !read_stdin)
	{
	    fc = fname[fnamelen - 1];
	    اگر (TOLOWER_ASC(fc) == 'x')
	    {
		// Read the first line (and a bit more).  Immediately rewind to
		// the start of the file.  If the read() fails "len" is -1.
		len = read_eintr(fd, firstline, 80);
		vim_lseek(fd, (off_T)0L, SEEK_SET);
		برائے (p = firstline; p < firstline + len; ++p)
		    اگر (*p >= 0x80)
		    {
			c = TRUE;
			توڑو;
		    }
	    }
	}

	اگر (c)
	{
	    fenc_اگلا = fenc;
	    fenc = (char_u *)"utf-8";

	    // When the file is utf-8 but a character doesn't fit in
	    // 'encoding' don't retry.  In help text editing utf-8 bytes
	    // doesn't make sense.
	    اگر (!enc_utf8)
		keep_dest_enc = TRUE;
	}
	fenc_alloced = FALSE;
    }
    ورنہ اگر (*p_fencs == NUL)
    {
	fenc = curbuf->b_p_fenc;	// use format from buffer
	fenc_alloced = FALSE;
    }
    ورنہ
    {
	fenc_اگلا = p_fencs;		// try items in 'fileencodings'
	fenc = اگلا_fenc(&fenc_اگلا, &fenc_alloced);
    }

    /*
     * Jump back here to retry reading the file in different ways.
     * Reasons to retry:
     * - encoding conversion failed: try another one from "fenc_next"
     * - BOM detected and fenc was set, need to setup conversion
     * - "fileformat" check failed: try another
     *
     * Variables set for special retry actions:
     * "file_rewind"	Rewind the file to start reading it again.
     * "advance_fenc"	Advance "fenc" using "fenc_next".
     * "skip_read"	Re-use already read bytes (BOM detected).
     * "did_iconv"	iconv() conversion failed, try 'charconvert'.
     * "keep_fileformat" Don't reset "fileformat".
     *
     * Other status indicators:
     * "tmpname"	When != NULL did conversion with 'charconvert'.
     *			Output file has to be deleted afterwards.
     * "iconv_fd"	When != -1 did conversion with iconv().
     */
retry:

    اگر (file_rewind)
    {
	اگر (read_بفر)
	{
	    read_buf_lnum = 1;
	    read_buf_ستون = 0;
	}
	ورنہ اگر (read_stdin || vim_lseek(fd, (off_T)0L, SEEK_SET) != 0)
	{
	    // Can't rewind the file, give up.
	    error = TRUE;
	    جاؤ failed;
	}
	// Delete the previously read lines.
	جب (lnum > from)
	    ml_delete(lnum--);
	file_rewind = FALSE;
	اگر (سیٹ_options)
	{
	    curbuf->b_p_bomb = FALSE;
	    curbuf->b_آغاز_bomb = FALSE;
	}
	conv_error = 0;
    }

    /*
     * When retrying with another "fenc" and the first time "fileformat"
     * will be reset.
     */
    اگر (keep_fileformat)
	keep_fileformat = FALSE;
    ورنہ
    {
	اگر (eap != تہی && eap->force_ff != 0)
	{
	    fileformat = get_fileformat_force(curbuf, eap);
	    try_unix = try_dos = try_mac = FALSE;
	}
	ورنہ اگر (curbuf->b_p_bin)
	    fileformat = EOL_UNIX;		// binary: use Unix format
	ورنہ اگر (*p_ffs == NUL)
	    fileformat = get_fileformat(curbuf);// use format from buffer
	ورنہ
	    fileformat = EOL_UNKNOWN;		// detect from file
    }

#اگر_تعریف_شدہ USE_ICONV
    اگر (iconv_fd != (iconv_t)-1)
    {
	// aborted conversion with iconv(), close the descriptor
	iconv_close(iconv_fd);
	iconv_fd = (iconv_t)-1;
    }
#اگر_ختم

    اگر (advance_fenc)
    {
	/*
	 * Try the next entry in 'fileencodings'.
	 */
	advance_fenc = FALSE;

	اگر (eap != تہی && eap->force_enc != 0)
	{
	    // Conversion given with "++cc=" wasn't possible, read
	    // without conversion.
	    notconverted = TRUE;
	    conv_error = 0;
	    اگر (fenc_alloced)
		vim_free(fenc);
	    fenc = (char_u *)"";
	    fenc_alloced = FALSE;
	}
	ورنہ
	{
	    اگر (fenc_alloced)
		vim_free(fenc);
	    اگر (fenc_اگلا != تہی)
	    {
		fenc = اگلا_fenc(&fenc_اگلا, &fenc_alloced);
	    }
	    ورنہ
	    {
		fenc = (char_u *)"";
		fenc_alloced = FALSE;
	    }
	}
	اگر (tmpname != تہی)
	{
	    mch_remove(tmpname);		// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    /*
     * Conversion may be required when the encoding of the file is different
     * from 'encoding' or 'encoding' is UTF-16, UCS-2 or UCS-4.
     */
    fio_flags = 0;
    converted = need_conversion(fenc);
    اگر (converted)
    {
	// "ucs-bom" means we need to check the first bytes of the file
	// for a BOM.
	اگر (STRCMP(fenc, ENC_UCSBOM) == 0)
	    fio_flags = FIO_UCSBOM;

	/*
	 * Check if UCS-2/4 or Latin1 to UTF-8 conversion needs to be
	 * done.  This is handled below after read().  Prepare the
	 * fio_flags to avoid having to parse the string each time.
	 * Also check for Unicode to Latin1 conversion, because iconv()
	 * appears not to handle this correctly.  This works just like
	 * conversion to UTF-8 except how the resulting character is put in
	 * the buffer.
	 */
	ورنہ اگر (enc_utf8 || STRCMP(p_enc, "latin1") == 0)
	    fio_flags = get_fio_flags(fenc);

#اگر_تعریف_شدہ MSWIN
	/*
	 * Conversion from an MS-Windows codepage to UTF-8 or another codepage
	 * is handled with MultiByteToWideChar().
	 */
	اگر (fio_flags == 0)
	    fio_flags = get_win_fio_flags(fenc);
#اگر_ختم

#اگر_تعریف_شدہ MACOS_CONVERT
	// Conversion from Apple MacRoman to latin1 or UTF-8
	اگر (fio_flags == 0)
	    fio_flags = get_mac_fio_flags(fenc);
#اگر_ختم

#اگر_تعریف_شدہ USE_ICONV
	/*
	 * Try using iconv() if we can't convert internally.
	 */
	اگر (fio_flags == 0
# ifdef FEAT_EVAL
		&& !did_iconv
# endif
		)
	    iconv_fd = (iconv_t)my_iconv_open(
				  enc_utf8 ? (char_u *)"utf-8" : p_enc, fenc);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
	/*
	 * Use the 'charconvert' expression when conversion is required
	 * and we can't do it internally or with iconv().
	 */
	اگر (fio_flags == 0 && !read_stdin && !read_بفر && *p_ccv != NUL
						    && !read_fifo
# ifdef USE_ICONV
						    && iconv_fd == (iconv_t)-1
# endif
		)
	{
# ifdef USE_ICONV
	    did_iconv = FALSE;
# endif
	    // Skip conversion when it's already done (retry for wrong
	    // "fileformat").
	    اگر (tmpname == تہی)
	    {
		tmpname = readfile_charconvert(fname, fenc, &fd);
		اگر (tmpname == تہی)
		{
		    // Conversion failed.  Try another one.
		    advance_fenc = TRUE;
		    اگر (fd < 0)
		    {
			// Re-opening the original file failed!
			emsg(_(e_conversion_mad_file_unreadable));
			error = TRUE;
			جاؤ failed;
		    }
		    جاؤ retry;
		}
	    }
	}
	ورنہ
#اگر_ختم
	{
	    اگر (fio_flags == 0
#اگر_تعریف_شدہ USE_ICONV
		    && iconv_fd == (iconv_t)-1
#اگر_ختم
	       )
	    {
		// Conversion wanted but we can't.
		// Try the next conversion in 'fileencodings'
		advance_fenc = TRUE;
		جاؤ retry;
	    }
	}
    }

    // Set "can_retry" when it's possible to rewind the file and try with
    // another "fenc" value.  It's FALSE when no other "fenc" to try, reading
    // stdin or fixed at a specific encoding.
    can_retry = (*fenc != NUL && !read_stdin && !read_fifo && !keep_dest_enc);

    اگر (!skip_read)
    {
	linerest = 0;
	filesize = 0;
#اگر_تعریف_شدہ FEAT_CRYPT
	filesize_گنتی = 0;
#اگر_ختم
	skip_گنتی = سطریں_مقصد_skip;
	read_گنتی = سطریں_مقصد_read;
	conv_restlen = 0;
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
	read_undo_file = (newfile && (flags & READ_KEEP_UNDO) == 0
				  && curbuf->b_ffname != تہی
				  && curbuf->b_p_udf
				  && !filtering
				  && !read_fifo
				  && !read_stdin
				  && !read_بفر);
	اگر (read_undo_file)
	    sha256_آغاز(&sha_ctx);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CRYPT
	اگر (curbuf->b_cryptstate != تہی)
	{
	    // Need to free the state, but keep the key, don't want to ask for
	    // it again.
	    crypt_free_state(curbuf->b_cryptstate);
	    curbuf->b_cryptstate = تہی;
	}
#اگر_ختم
    }

    جب (!error && !got_int)
    {
	/*
	 * We allocate as much space for the file as we can get, plus
	 * space for the old line plus room for one terminating NUL.
	 * The amount is limited by the fact that read() only can read
	 * up to max_unsigned characters (and other things).
	 */
	اگر (!skip_read)
	{
#اگر defined(SSIZE_MAX) && (SSIZE_MAX < 0x10000L)
		سائز = SSIZE_MAX;		    // use max I/O size, 52K
#ورنہ
		// Use buffer >= 64K.  Add linerest to double the size if the
		// line gets very long, to avoid a lot of copying. But don't
		// read more than 1 Mbyte at a time, so we can be interrupted.
		سائز = 0x10000L + linerest;
		اگر (سائز > 0x100000L)
		    سائز = 0x100000L;
#اگر_ختم
	}

	// Protect against the argument of lalloc() going negative.
	// Also split lines that are too long for colnr_T.  After this check
	// passes, we read up to 'size' more bytes.  We must ensure that even
	// after that read, the line length won't exceed MAXCOL - 1 (because
	// we add 1 for the NUL when casting to colnr_T).  If this check fires,
	// we insert a synthetic newline immediately, so linerest doesn't grow.
	اگر (سائز < 0 || سائز + linerest + 1 < 0 || linerest >= MAXCOL - سائز)
	{
	    ++بانٹو;
	    *ptr = NL;		    // split line by inserting a NL
	    سائز = 1;
	}
	ورنہ
	{
	    اگر (!skip_read)
	    {
		برائے ( ; سائز >= 10; سائز = (طویل)((long_u)سائز >> 1))
		{
		    اگر ((new_بفر = lalloc(سائز + linerest + 1,
							      FALSE)) != تہی)
			توڑو;
		}
		اگر (new_بفر == تہی)
		{
		    do_outofmem_پیغام((long_u)(سائز * 2 + linerest + 1));
		    error = TRUE;
		    توڑو;
		}
		اگر (linerest)	// copy characters from the previous buffer
		    mch_memmove(new_بفر, ptr - linerest, (size_t)linerest);
		vim_free(بفر);
		بفر = new_بفر;
		ptr = بفر + linerest;
		سطر_آغاز = بفر;

		// May need room to translate into.
		// For iconv() we don't really know the required space, use a
		// factor ICONV_MULT.
		// latin1 to utf-8: 1 byte becomes up to 2 bytes
		// utf-16 to utf-8: 2 bytes become up to 3 bytes, 4 bytes
		// become up to 4 bytes, size must be multiple of 2
		// ucs-2 to utf-8: 2 bytes become up to 3 bytes, size must be
		// multiple of 2
		// ucs-4 to utf-8: 4 bytes become up to 6 bytes, size must be
		// multiple of 4
		real_سائز = (صحیح)سائز;
#اگر_تعریف_شدہ USE_ICONV
		اگر (iconv_fd != (iconv_t)-1)
		    سائز = سائز / ICONV_MULT;
		ورنہ
#اگر_ختم
		    اگر (fio_flags & FIO_LATIN1)
		    سائز = سائز / 2;
		ورنہ اگر (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    سائز = (سائز * 2 / 3) & ~1;
		ورنہ اگر (fio_flags & FIO_UCS4)
		    سائز = (سائز * 2 / 3) & ~3;
		ورنہ اگر (fio_flags == FIO_UCSBOM)
		    سائز = سائز / ICONV_MULT;	// worst case
#اگر_تعریف_شدہ MSWIN
		ورنہ اگر (fio_flags & FIO_CODEPAGE)
		    سائز = سائز / ICONV_MULT;	// also worst case
#اگر_ختم
#اگر_تعریف_شدہ MACOS_CONVERT
		ورنہ اگر (fio_flags & FIO_MACROMAN)
		    سائز = سائز / ICONV_MULT;	// also worst case
#اگر_ختم

		اگر (conv_restlen > 0)
		{
		    // Insert unconverted bytes from previous line.
		    mch_memmove(ptr, conv_rest, conv_restlen);
		    ptr += conv_restlen;
		    سائز -= conv_restlen;
		}

		اگر (read_بفر)
		{
		    /*
		     * Read bytes from curbuf.  Used for converting text read
		     * from stdin.
		     */
		    اگر (read_buf_lnum > from)
			سائز = 0;
		    ورنہ
		    {
			صحیح	n, ni;
			طویل	tlen;

			tlen = 0;
			برائے (;;)
			{
			    p = ml_get(read_buf_lnum) + read_buf_ستون;
			    n = ml_get_len(read_buf_lnum) - read_buf_ستون;
			    اگر ((صحیح)tlen + n + 1 > سائز)
			    {
				// Filled up to "size", append partial line.
				// Change NL to NUL to reverse the effect done
				// below.
				n = (صحیح)(سائز - tlen);
				برائے (ni = 0; ni < n; ++ni)
				{
				    اگر (p[ni] == NL)
					ptr[tlen++] = NUL;
				    ورنہ
					ptr[tlen++] = p[ni];
				}
				read_buf_ستون += n;
				توڑو;
			    }

			    // Append whole line and new-line.  Change NL
			    // to NUL to reverse the effect done below.
			    برائے (ni = 0; ni < n; ++ni)
			    {
				اگر (p[ni] == NL)
				    ptr[tlen++] = NUL;
				ورنہ
				    ptr[tlen++] = p[ni];
			    }
			    ptr[tlen++] = NL;
			    read_buf_ستون = 0;
			    اگر (++read_buf_lnum > from)
			    {
				// When the last line didn't have an
				// end-of-line don't add it now either.
				اگر (!curbuf->b_p_eol)
				    --tlen;
				سائز = tlen;
#اگر_تعریف_شدہ FEAT_CRYPT
				eof = TRUE;
#اگر_ختم
				توڑو;
			    }
			}
		    }
		}
		ورنہ
		{
		    /*
		     * Read bytes from the file.
		     */
#اگر_تعریف_شدہ FEAT_SODIUM
		    // Let the crypt layer work with a buffer size of 8192
		    //
		    // Sodium encryption requires a fixed block size to
		    // successfully decrypt. However, unfortunately the file
		    // header size changes between xchacha20 and xchacha20v2 by
		    // 'add_len' bytes.
		    // So we will now read the maximum header size + encryption
		    // metadata, but after determining to read an xchacha20
		    // encrypted file, we have to rewind the file descriptor by
		    // 'add_len' bytes in the second round.
		    //
		    // Be careful with changing it, it needs to stay the same
		    // for reading back previously encrypted files!
		    اگر (filesize == 0)
		    {
			// set size to 8K + Sodium Crypt Metadata
			سائز = WRITEBUFSIZE + crypt_get_max_سرخی_len()
			    + crypto_secretstream_xchacha20poly1305_HEADERBYTES
				+ crypto_secretstream_xchacha20poly1305_ABYTES;
			may_need_lseek = TRUE;
		    }

		    ورنہ اگر (filesize > 0 && (curbuf->b_cryptstate != تہی
				&& crypt_طریقہ_is_sodium(
					     curbuf->b_cryptstate->طریقہ_nr)))
		    {
			سائز = WRITEBUFSIZE + crypto_secretstream_xchacha20poly1305_ABYTES;
			// need to rewind by - add_len from CRYPT_M_SOD2 (see
			// description above)
			اگر (curbuf->b_cryptstate->طریقہ_nr == CRYPT_M_SOD
						     && !eof && may_need_lseek)
			{
			    lseek(fd, crypt_get_سرخی_len(
					       curbuf->b_cryptstate->طریقہ_nr)
				       - crypt_get_max_سرخی_len(), SEEK_CUR);
			    may_need_lseek = FALSE;
			}
		    }
#اگر_ختم
		    طویل read_سائز = سائز;
		    سائز = read_eintr(fd, ptr, read_سائز);
#اگر_تعریف_شدہ FEAT_CRYPT
		    // Did we reach end of file?
		    filesize_گنتی += سائز;
		    eof = (سائز < read_سائز || filesize_گنتی == filesize_disk);
#اگر_ختم
		}

#اگر_تعریف_شدہ FEAT_CRYPT
		/*
		 * At start of file: Check for magic number of encryption.
		 */
		اگر (filesize == 0 && سائز > 0)
		{
		    cryptkey = check_for_cryptkey(cryptkey, ptr, &سائز,
						  &filesize, newfile, sfname,
						  &did_ask_for_کلید);
# اگر defined(CRYPT_NOT_INPLACE) && defined(FEAT_PERSISTENT_UNDO)
		    اگر (curbuf->b_cryptstate != تہی
				 && !crypt_چلتا_inplace(curbuf->b_cryptstate))
			// reading undo file requires crypt_decode_inplace()
			read_undo_file = FALSE;
# endif
		}
		/*
		 * Decrypt the read bytes.  This is done before checking for
		 * EOF because the crypt layer may be buffering.
		 */
		اگر (cryptkey != تہی && curbuf->b_cryptstate != تہی
								   && سائز > 0)
		{
# ifdef CRYPT_NOT_INPLACE
		    اگر (crypt_چلتا_inplace(curbuf->b_cryptstate))
		    {
# endif
			crypt_decode_inplace(curbuf->b_cryptstate, ptr,
								    سائز, eof);
# ifdef CRYPT_NOT_INPLACE
		    }
		    ورنہ
		    {
			char_u	*newptr = تہی;
			صحیح	decrypted_سائز;

			decrypted_سائز = crypt_decode_alloc(
				    curbuf->b_cryptstate, ptr, سائز,
								 &newptr, eof);

			اگر (decrypted_سائز < 0)
			{
			    // error message already given
			    error = TRUE;
			    vim_free(newptr);
			    توڑو;
			}
			// If the crypt layer is buffering, not producing
			// anything yet, need to read more.
			اگر (decrypted_سائز == 0)
			    جاری;

			اگر (linerest == 0)
			{
			    // Simple case: reuse returned buffer (may be
			    // NULL, checked later).
			    new_بفر = newptr;
			}
			ورنہ
			{
			    long_u	new_سائز;

			    // Need new buffer to add bytes carried over.
			    new_سائز = (long_u)(decrypted_سائز + linerest + 1);
			    new_بفر = lalloc(new_سائز, FALSE);
			    اگر (new_بفر == تہی)
			    {
				do_outofmem_پیغام(new_سائز);
				error = TRUE;
				توڑو;
			    }

			    mch_memmove(new_بفر, بفر, linerest + conv_restlen);
			    اگر (newptr != تہی)
				mch_memmove(new_بفر + linerest + conv_restlen,
					newptr, decrypted_سائز);
			    vim_free(newptr);
			}

			اگر (new_بفر != تہی)
			{
			    vim_free(بفر);
			    بفر = new_بفر;
			    new_بفر = تہی;
			    سطر_آغاز = بفر;
			    ptr = بفر + linerest + conv_restlen;
			    real_سائز = سائز;
			}
			سائز = decrypted_سائز;
		    }
# endif
		}
#اگر_ختم

		اگر (سائز <= 0)
		{
		    اگر (سائز < 0)		    // read error
			error = TRUE;
		    ورنہ اگر (conv_restlen > 0)
		    {
			/*
			 * Reached end-of-file but some trailing bytes could
			 * not be converted.  Truncated file?
			 */

			// When we did a conversion report an error.
			اگر (fio_flags != 0
#اگر_تعریف_شدہ USE_ICONV
				|| iconv_fd != (iconv_t)-1
#اگر_ختم
			   )
			{
			    اگر (can_retry)
				جاؤ rewind_retry;
			    اگر (conv_error == 0)
				conv_error = curbuf->b_ml.ml_سطر_گنتی
								- linecnt + 1;
			}
			// Remember the first linenr with an illegal byte
			ورنہ اگر (illegal_byte == 0)
			    illegal_byte = curbuf->b_ml.ml_سطر_گنتی
								- linecnt + 1;
			اگر (bad_char_behavior == BAD_DROP)
			{
			    *(ptr - conv_restlen) = NUL;
			    conv_restlen = 0;
			}
			ورنہ
			{
			    // Replace the trailing bytes with the replacement
			    // character if we were converting; if we weren't,
			    // leave the UTF8 checking code to do it, as it
			    // works slightly differently.
			    اگر (bad_char_behavior != BAD_KEEP && (fio_flags != 0
#اگر_تعریف_شدہ USE_ICONV
				    || iconv_fd != (iconv_t)-1
#اگر_ختم
			       ))
			    {
				جب (conv_restlen > 0)
				{
				    *(--ptr) = bad_char_behavior;
				    --conv_restlen;
				}
			    }
			    fio_flags = 0;	// don't convert this
#اگر_تعریف_شدہ USE_ICONV
			    اگر (iconv_fd != (iconv_t)-1)
			    {
				iconv_close(iconv_fd);
				iconv_fd = (iconv_t)-1;
			    }
#اگر_ختم
			}
		    }
		}
	    }
	    skip_read = FALSE;

	    /*
	     * At start of file (or after crypt magic number): Check for BOM.
	     * Also check for a BOM for other Unicode encodings, but not after
	     * converting with 'charconvert' or when a BOM has already been
	     * found.
	     */
	    اگر ((filesize == 0
#اگر_تعریف_شدہ FEAT_CRYPT
		   || (cryptkey != تہی
			&& filesize == crypt_get_سرخی_len(
						 crypt_get_طریقہ_nr(curbuf)))
#اگر_ختم
		       )
		    && (fio_flags == FIO_UCSBOM
			|| (!curbuf->b_p_bomb
			    && tmpname == تہی
			    && (*fenc == 'u' || (*fenc == NUL && enc_utf8)))))
	    {
		char_u	*ccname;
		صحیح	blen;

		// no BOM detection in a short file or in binary mode
		اگر (سائز < 2 || curbuf->b_p_bin)
		    ccname = تہی;
		ورنہ
		    ccname = check_for_bom(ptr, سائز, &blen,
		      fio_flags == FIO_UCSBOM ? FIO_ALL : get_fio_flags(fenc));
		اگر (ccname != تہی)
		{
		    // Remove BOM from the text
		    filesize += blen;
		    سائز -= blen;
		    mch_memmove(ptr, ptr + blen, (size_t)سائز);
		    اگر (سیٹ_options)
		    {
			curbuf->b_p_bomb = TRUE;
			curbuf->b_آغاز_bomb = TRUE;
		    }
		}

		اگر (fio_flags == FIO_UCSBOM)
		{
		    اگر (ccname == تہی)
		    {
			// No BOM detected: retry with next encoding.
			advance_fenc = TRUE;
		    }
		    ورنہ
		    {
			// BOM detected: set "fenc" and jump back
			اگر (fenc_alloced)
			    vim_free(fenc);
			fenc = ccname;
			fenc_alloced = FALSE;
		    }
		    // retry reading without getting new bytes or rewinding
		    skip_read = TRUE;
		    جاؤ retry;
		}
	    }

	    // Include not converted bytes.
	    ptr -= conv_restlen;
	    سائز += conv_restlen;
	    conv_restlen = 0;
	    /*
	     * Break here for a read error or end-of-file.
	     */
	    اگر (سائز <= 0)
		توڑو;


#اگر_تعریف_شدہ USE_ICONV
	    اگر (iconv_fd != (iconv_t)-1)
	    {
		/*
		 * Attempt conversion of the read bytes to 'encoding' using
		 * iconv().
		 */
		ثابت حرف	*fromp;
		حرف		*چوٹی;
		size_t		from_سائز;
		size_t		مقصد_سائز;

		fromp = (حرف *)ptr;
		from_سائز = سائز;
		ptr += سائز;
		چوٹی = (حرف *)ptr;
		مقصد_سائز = real_سائز - سائز;

		/*
		 * If there is conversion error or not enough room try using
		 * another conversion.  Except for when there is no
		 * alternative (help files).
		 */
		جب ((iconv(iconv_fd, (خالی *)&fromp, &from_سائز,
							       &چوٹی, &مقصد_سائز)
			    == (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						  || from_سائز > CONV_RESTLEN)
		{
		    اگر (can_retry)
			جاؤ rewind_retry;
		    اگر (conv_error == 0)
			conv_error = readfile_linenr(linecnt,
							  ptr, (char_u *)چوٹی);

		    // Deal with a bad byte and continue with the next.
		    ++fromp;
		    --from_سائز;
		    اگر (bad_char_behavior == BAD_KEEP)
		    {
			*چوٹی++ = *(fromp - 1);
			--مقصد_سائز;
		    }
		    ورنہ اگر (bad_char_behavior != BAD_DROP)
		    {
			*چوٹی++ = bad_char_behavior;
			--مقصد_سائز;
		    }
		}

		اگر (from_سائز > 0)
		{
		    // Some remaining characters, keep them for the next
		    // round.
		    mch_memmove(conv_rest, (char_u *)fromp, from_سائز);
		    conv_restlen = (صحیح)from_سائز;
		}

		// move the linerest to before the converted characters
		سطر_آغاز = ptr - linerest;
		mch_memmove(سطر_آغاز, بفر, (size_t)linerest);
		سائز = (طویل)((char_u *)چوٹی - ptr);
	    }
#اگر_ختم

#اگر_تعریف_شدہ MSWIN
	    اگر (fio_flags & FIO_CODEPAGE)
	    {
		char_u	*src, *dst;
		WCHAR	ucs2buf[3];
		صحیح	ucs2len;
		صحیح	codepage = FIO_GET_CP(fio_flags);
		صحیح	bytelen;
		صحیح	ملا_bad;
		حرف	replstr[2];

		/*
		 * Conversion from an MS-Windows codepage or UTF-8 to UTF-8 or
		 * a codepage, using standard MS-Windows functions.  This
		 * requires two steps:
		 * 1. convert from 'fileencoding' to ucs-2
		 * 2. convert from ucs-2 to 'encoding'
		 *
		 * Because there may be illegal bytes AND an incomplete byte
		 * sequence at the end, we may have to do the conversion one
		 * character at a time to get it right.
		 */

		// Replacement string for WideCharToMultiByte().
		اگر (bad_char_behavior > 0)
		    replstr[0] = bad_char_behavior;
		ورنہ
		    replstr[0] = '?';
		replstr[1] = NUL;

		/*
		 * Move the bytes to the end of the buffer, so that we have
		 * room to put the result at the start.
		 */
		src = ptr + real_سائز - سائز;
		mch_memmove(src, ptr, سائز);

		/*
		 * Do the conversion.
		 */
		dst = ptr;
		جب (سائز > 0)
		{
		    ملا_bad = FALSE;

# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
		    اگر (codepage == CP_UTF8)
		    {
			// Handle CP_UTF8 input ourselves to be able to handle
			// trailing bytes properly.
			// Get one UTF-8 character from src.
			bytelen = (صحیح)utf_ptr2len_len(src, سائز);
			اگر (bytelen > سائز)
			{
			    // Only got some bytes of a character.  Normally
			    // it's put in "conv_rest", but if it's too long
			    // deal with it as if they were illegal bytes.
			    اگر (bytelen <= CONV_RESTLEN)
				توڑو;

			    // weird overlong byte sequence
			    bytelen = سائز;
			    ملا_bad = TRUE;
			}
			ورنہ
			{
			    صحیح	    u8c = utf_ptr2char(src);

			    اگر (u8c > 0xffff || (*src >= 0x80 && bytelen == 1))
				ملا_bad = TRUE;
			    ucs2buf[0] = u8c;
			    ucs2len = 1;
			}
		    }
		    ورنہ
# endif
		    {
			// We don't know how long the byte sequence is, try
			// from one to three bytes.
			برائے (bytelen = 1; bytelen <= سائز && bytelen <= 3;
								    ++bytelen)
			{
			    ucs2len = Multi_Byte_مقصد_Wide_Char(codepage,
							 MB_ERR_INVALID_CHARS,
							 (LPCSTR)src, bytelen,
								   ucs2buf, 3);
			    اگر (ucs2len > 0)
				توڑو;
			}
			اگر (ucs2len == 0)
			{
			    // If we have only one byte then it's probably an
			    // incomplete byte sequence.  Otherwise discard
			    // one byte as a bad character.
			    اگر (سائز == 1)
				توڑو;
			    ملا_bad = TRUE;
			    bytelen = 1;
			}
		    }

		    اگر (!ملا_bad)
		    {
			صحیح	i;

			// Convert "ucs2buf[ucs2len]" to 'enc' in "dst".
			اگر (enc_utf8)
			{
			    // From UCS-2 to UTF-8.  Cannot fail.
			    برائے (i = 0; i < ucs2len; ++i)
				dst += utf_char2bytes(ucs2buf[i], dst);
			}
			ورنہ
			{
			    BOOL	bad = FALSE;
			    صحیح		dstlen;

			    // From UCS-2 to "enc_codepage".  If the
			    // conversion uses the default character "?",
			    // the data doesn't fit in this encoding.
			    dstlen = Wide_Char_مقصد_Multi_Byte(enc_codepage, 0,
				    (LPCWSTR)ucs2buf, ucs2len,
				    (LPSTR)dst, (صحیح)(src - dst),
				    replstr, &bad);
			    اگر (bad)
				ملا_bad = TRUE;
			    ورنہ
				dst += dstlen;
			}
		    }

		    اگر (ملا_bad)
		    {
			// Deal with bytes we can't convert.
			اگر (can_retry)
			    جاؤ rewind_retry;
			اگر (conv_error == 0)
			    conv_error = readfile_linenr(linecnt, ptr, dst);
			اگر (bad_char_behavior != BAD_DROP)
			{
			    اگر (bad_char_behavior == BAD_KEEP)
			    {
				mch_memmove(dst, src, bytelen);
				dst += bytelen;
			    }
			    ورنہ
				*dst++ = bad_char_behavior;
			}
		    }

		    src += bytelen;
		    سائز -= bytelen;
		}

		اگر (سائز > 0)
		{
		    // An incomplete byte sequence remaining.
		    mch_memmove(conv_rest, src, سائز);
		    conv_restlen = سائز;
		}

		// The new size is equal to how much "dst" was advanced.
		سائز = (طویل)(dst - ptr);
	    }
	    ورنہ
#اگر_ختم
#اگر_تعریف_شدہ MACOS_CONVERT
	    اگر (fio_flags & FIO_MACROMAN)
	    {
		/*
		 * Conversion from Apple MacRoman char encoding to UTF-8 or
		 * latin1.  This is in os_mac_conv.c.
		 */
		اگر (macroman2enc(ptr, &سائز, real_سائز) == FAIL)
		    جاؤ rewind_retry;
	    }
	    ورنہ
#اگر_ختم
	    اگر (fio_flags != 0)
	    {
		صحیح	u8c;
		char_u	*dest;
		char_u	*دم = تہی;

		/*
		 * "enc_utf8" set: Convert Unicode or Latin1 to UTF-8.
		 * "enc_utf8" not set: Convert Unicode to Latin1.
		 * Go from end to start through the buffer, because the number
		 * of bytes may increase.
		 * "dest" points to after where the UTF-8 bytes go, "p" points
		 * to after the next character to convert.
		 */
		dest = ptr + real_سائز;
		اگر (fio_flags == FIO_LATIN1 || fio_flags == FIO_UTF8)
		{
		    p = ptr + سائز;
		    اگر (fio_flags == FIO_UTF8)
		    {
			// Check for a trailing incomplete UTF-8 sequence
			دم = ptr + سائز - 1;
			جب (دم > ptr && (*دم & 0xc0) == 0x80)
			    --دم;
			اگر (دم + utf_byte2len(*دم) <= ptr + سائز)
			    دم = تہی;
			ورنہ
			    p = دم;
		    }
		}
		ورنہ اگر (fio_flags & (FIO_UCS2 | FIO_UTF16))
		{
		    // Check for a trailing byte
		    p = ptr + (سائز & ~1);
		    اگر (سائز & 1)
			دم = p;
		    اگر ((fio_flags & FIO_UTF16) && p > ptr)
		    {
			// Check for a trailing leading word
			اگر (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			ورنہ
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			اگر (u8c >= 0xd800 && u8c <= 0xdbff)
			    دم = p;
			ورنہ
			    p += 2;
		    }
		}
		ورنہ //  FIO_UCS4
		{
		    // Check for trailing 1, 2 or 3 bytes
		    p = ptr + (سائز & ~3);
		    اگر (سائز & 3)
			دم = p;
		}

		// If there is a trailing incomplete sequence move it to
		// conv_rest[].
		اگر (دم != تہی)
		{
		    conv_restlen = (صحیح)((ptr + سائز) - دم);
		    mch_memmove(conv_rest, (char_u *)دم, conv_restlen);
		    سائز -= conv_restlen;
		}


		جب (p > ptr)
		{
		    اگر (fio_flags & FIO_LATIN1)
			u8c = *--p;
		    ورنہ اگر (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    {
			اگر (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			ورنہ
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			اگر ((fio_flags & FIO_UTF16)
					    && u8c >= 0xdc00 && u8c <= 0xdfff)
			{
			    صحیح u16c;

			    اگر (p == ptr)
			    {
				// Missing leading word.
				اگر (can_retry)
				    جاؤ rewind_retry;
				اگر (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				اگر (bad_char_behavior == BAD_DROP)
				    جاری;
				اگر (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }

			    // found second word of double-word, get the first
			    // word and compute the resulting character
			    اگر (fio_flags & FIO_ENDIAN_L)
			    {
				u16c = (*--p << 8);
				u16c += *--p;
			    }
			    ورنہ
			    {
				u16c = *--p;
				u16c += (*--p << 8);
			    }
			    u8c = 0x10000 + ((u16c & 0x3ff) << 10)
							      + (u8c & 0x3ff);

			    // Check if the word is indeed a leading word.
			    اگر (u16c < 0xd800 || u16c > 0xdbff)
			    {
				اگر (can_retry)
				    جاؤ rewind_retry;
				اگر (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				اگر (bad_char_behavior == BAD_DROP)
				    جاری;
				اگر (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    ورنہ اگر (fio_flags & FIO_UCS4)
		    {
			اگر (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (غیرعلامتی)*--p << 24;
			    u8c += (غیرعلامتی)*--p << 16;
			    u8c += (غیرعلامتی)*--p << 8;
			    u8c += *--p;
			}
			ورنہ	// big endian
			{
			    u8c = *--p;
			    u8c += (غیرعلامتی)*--p << 8;
			    u8c += (غیرعلامتی)*--p << 16;
			    u8c += (غیرعلامتی)*--p << 24;
			}
		    }
		    ورنہ    // UTF-8
		    {
			اگر (*--p < 0x80)
			    u8c = *p;
			ورنہ
			{
			    len = utf_سر_off(ptr, p);
			    p -= len;
			    u8c = utf_ptr2char(p);
			    اگر (len == 0)
			    {
				// Not a valid UTF-8 character, retry with
				// another fenc when possible, otherwise just
				// report the error.
				اگر (can_retry)
				    جاؤ rewind_retry;
				اگر (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				اگر (bad_char_behavior == BAD_DROP)
				    جاری;
				اگر (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    اگر (enc_utf8)	// produce UTF-8
		    {
			dest -= utf_char2len(u8c);
			(خالی)utf_char2bytes(u8c, dest);
		    }
		    ورنہ		// produce Latin1
		    {
			--dest;
			اگر (u8c >= 0x100)
			{
			    // character doesn't fit in latin1, retry with
			    // another fenc when possible, otherwise just
			    // report the error.
			    اگر (can_retry)
				جاؤ rewind_retry;
			    اگر (conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
			    اگر (bad_char_behavior == BAD_DROP)
				++dest;
			    ورنہ اگر (bad_char_behavior == BAD_KEEP)
				*dest = u8c;
			    ورنہ اگر (eap != تہی && eap->bad_char != 0)
				*dest = bad_char_behavior;
			    ورنہ
				*dest = 0xBF;
			}
			ورنہ
			    *dest = u8c;
		    }
		}

		// move the linerest to before the converted characters
		سطر_آغاز = dest - linerest;
		mch_memmove(سطر_آغاز, بفر, (size_t)linerest);
		سائز = (طویل)((ptr + real_سائز) - dest);
		ptr = dest;
	    }
	    ورنہ اگر (enc_utf8 && !curbuf->b_p_bin)
	    {
		صحیح  incomplete_دم = FALSE;

		// Reading UTF-8: Check if the bytes are valid UTF-8.
		برائے (p = ptr; ; )
		{
		    صحیح	 todo;
		    صحیح	 l;

		    // Skip ASCII bytes quickly using word-at-a-time check.
		    {
			char_u *ascii_انتہا = ptr + سائز;
			جب (ascii_انتہا - p >= (طویل)حجم(long_u))
			{
			    long_u لفظ;
			    حافظہ_نقل(&لفظ, p, حجم(long_u));
			    اگر (لفظ & NONASCII_نقاب)
				توڑو;
			    p += حجم(long_u);
			}
			جب (p < ascii_انتہا && *p < 0x80)
			    ++p;
		    }

		    todo = (صحیح)((ptr + سائز) - p);
		    اگر (todo <= 0)
			توڑو;
		    اگر (*p >= 0x80)
		    {
			// A length of 1 means it's an illegal byte.  Accept
			// an incomplete character at the end though, the next
			// read() will get the next bytes, we'll check it
			// then.
			l = utf_ptr2len_len(p, todo);
			اگر (l > todo && !incomplete_دم)
			{
			    // Avoid retrying with a different encoding when
			    // a truncated file is more likely, or attempting
			    // to read the rest of an incomplete sequence when
			    // we have already done so.
			    اگر (p > ptr || filesize > 0)
				incomplete_دم = TRUE;
			    // Incomplete byte sequence, move it to conv_rest[]
			    // and try to read the rest of it, unless we've
			    // already done so.
			    اگر (p > ptr)
			    {
				conv_restlen = todo;
				mch_memmove(conv_rest, p, conv_restlen);
				سائز -= conv_restlen;
				توڑو;
			    }
			}
			اگر (l == 1 || l > todo)
			{
			    // Illegal byte.  If we can try another encoding
			    // do that, unless at EOF where a truncated
			    // file is more likely than a conversion error.
			    اگر (can_retry && !incomplete_دم)
				توڑو;
#اگر_تعریف_شدہ USE_ICONV
			    // When we did a conversion report an error.
			    اگر (iconv_fd != (iconv_t)-1 && conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
#اگر_ختم
			    // Remember the first linenr with an illegal byte
			    اگر (conv_error == 0 && illegal_byte == 0)
				illegal_byte = readfile_linenr(linecnt, ptr, p);

			    // Drop, keep or replace the bad byte.
			    اگر (bad_char_behavior == BAD_DROP)
			    {
				mch_memmove(p, p + 1, todo - 1);
				--سائز;
			    }
			    ورنہ
			    {
				اگر (bad_char_behavior != BAD_KEEP)
				    *p = bad_char_behavior;
				++p;
			    }
			}
			ورنہ
			    p += l;
		    }
		}
		اگر (p < ptr + سائز && !incomplete_دم)
		{
		    // Detected a UTF-8 error.
rewind_retry:
		    // Retry reading with another conversion.
#اگر defined(FEAT_EVAL) && defined(USE_ICONV)
		    اگر (*p_ccv != NUL && iconv_fd != (iconv_t)-1)
			// iconv() failed, try 'charconvert'
			did_iconv = TRUE;
		    ورنہ
#اگر_ختم
			// use next item from 'fileencodings'
			advance_fenc = TRUE;
		    file_rewind = TRUE;
		    جاؤ retry;
		}
	    }

	    // count the number of characters (after conversion!)
	    filesize += سائز;

	    /*
	     * when reading the first part of a file: guess EOL type
	     */
	    اگر (fileformat == EOL_UNKNOWN)
	    {
		// First try finding a NL, for Dos and Unix
		اگر (try_dos || try_unix)
		{
		    // Reset the carriage return counter.
		    اگر (try_mac)
			try_mac = 1;

		    برائے (p = ptr; p < ptr + سائز; ++p)
		    {
			اگر (*p == NL)
			{
			    اگر (!try_unix
				    || (try_dos && p > ptr && p[-1] == CAR))
				fileformat = EOL_DOS;
			    ورنہ
				fileformat = EOL_UNIX;
			    توڑو;
			}
			ورنہ اگر (*p == CAR && try_mac)
			    try_mac++;
		    }

		    // Don't give in to EOL_UNIX if EOL_MAC is more likely
		    اگر (fileformat == EOL_UNIX && try_mac)
		    {
			// Need to reset the counters when retrying fenc.
			try_mac = 1;
			try_unix = 1;
			برائے (; p >= ptr && *p != CAR; p--)
			    ;
			اگر (p >= ptr)
			{
			    برائے (p = ptr; p < ptr + سائز; ++p)
			    {
				اگر (*p == NL)
				    try_unix++;
				ورنہ اگر (*p == CAR)
				    try_mac++;
			    }
			    اگر (try_mac > try_unix)
				fileformat = EOL_MAC;
			}
		    }
		    ورنہ اگر (fileformat == EOL_UNKNOWN && try_mac == 1)
			// Looking for CR but found no end-of-line markers at
			// all: use the default format.
			fileformat = default_fileformat();
		}

		// No NL found: may use Mac format
		اگر (fileformat == EOL_UNKNOWN && try_mac)
		    fileformat = EOL_MAC;

		// Still nothing found?  Use first format in 'ffs'
		اگر (fileformat == EOL_UNKNOWN)
		    fileformat = default_fileformat();

		// if editing a new file: may set p_tx and p_ff
		اگر (سیٹ_options)
		    سیٹ_fileformat(fileformat, OPT_LOCAL);
	    }
	}

	/*
	 * This loop is executed once for every character read.
	 * Keep it fast!
	 */
	اگر (fileformat == EOL_MAC)
	{
	    --ptr;
	    جب (++ptr, --سائز >= 0)
	    {
		// catch most common case first
		اگر ((c = *ptr) != NUL && c != CAR && c != NL)
		    جاری;
		اگر (c == NUL)
		    *ptr = NL;	// NULs are replaced by newlines!
		ورنہ اگر (c == NL)
		    *ptr = CAR;	// NLs are replaced by CRs!
		ورنہ
		{
		    اگر (skip_گنتی == 0)
		    {
			*ptr = NUL;	    // end of line
			len = (colnr_T) (ptr - سطر_آغاز + 1);
			اگر (ml_append(lnum, سطر_آغاز, len, newfile) == FAIL)
			{
			    error = TRUE;
			    توڑو;
			}
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
			اگر (read_undo_file)
			    sha256_update(&sha_ctx, سطر_آغاز, len);
#اگر_ختم
			++lnum;
			اگر (--read_گنتی == 0)
			{
			    error = TRUE;	// break loop
			    سطر_آغاز = ptr;	// nothing left to write
			    توڑو;
			}
		    }
		    ورنہ
			--skip_گنتی;
		    سطر_آغاز = ptr + 1;
		}
	    }
	}
	ورنہ
	{
	    // Use memchr() for SIMD-optimized newline scanning instead
	    // of scanning each byte individually.
	    char_u *انتہا = ptr + سائز;

	    جب (ptr < انتہا)
	    {
		char_u *nl = (char_u *)memchr(ptr, NL, انتہا - ptr);
		char_u *nul_جائزہ;

		اگر (nl == تہی)
		{
		    // No more newlines in buffer.
		    // Replace any NUL bytes with NL in remaining data.
		    جب ((nul_جائزہ = (char_u *)memchr(ptr, NUL,
						      انتہا - ptr)) != تہی)
		    {
			*nul_جائزہ = NL;
			ptr = nul_جائزہ + 1;
		    }
		    ptr = انتہا;
		    توڑو;
		}

		// Replace NUL bytes with NL before the newline.
		{
		    char_u *جائزہ = ptr;
		    جب ((nul_جائزہ = (char_u *)memchr(جائزہ, NUL,
						       nl - جائزہ)) != تہی)
		    {
			*nul_جائزہ = NL;
			جائزہ = nul_جائزہ + 1;
		    }
		}

		// Process the newline.
		ptr = nl;
		اگر (skip_گنتی == 0)
		{
		    *ptr = NUL;		// end of line
		    len = (colnr_T)(ptr - سطر_آغاز + 1);
		    اگر (fileformat == EOL_DOS)
		    {
			اگر (ptr > سطر_آغاز && ptr[-1] == CAR)
			{
			    // remove CR before NL
			    ptr[-1] = NUL;
			    --len;
			}
			/*
			 * Reading in Dos format, but no CR-LF found!
			 * When 'fileformats' includes "unix", delete all
			 * the lines read so far and start all over again.
			 * Otherwise give an error message later.
			 */
			ورنہ اگر (ff_error != EOL_DOS)
			{
			    اگر (   try_unix
				&& !read_stdin
				&& (read_بفر
				    || vim_lseek(fd, (off_T)0L, SEEK_SET)
								      == 0))
			    {
				fileformat = EOL_UNIX;
				اگر (سیٹ_options)
				    سیٹ_fileformat(EOL_UNIX, OPT_LOCAL);
				file_rewind = TRUE;
				keep_fileformat = TRUE;
				جاؤ retry;
			    }
			    ff_error = EOL_DOS;
			}
		    }
		    اگر (ml_append(lnum, سطر_آغاز, len, newfile) == FAIL)
		    {
			error = TRUE;
			توڑو;
		    }
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
		    اگر (read_undo_file)
			sha256_update(&sha_ctx, سطر_آغاز, len);
#اگر_ختم
		    ++lnum;
		    اگر (--read_گنتی == 0)
		    {
			error = TRUE;	    // break loop
			سطر_آغاز = ptr;   // nothing left to write
			توڑو;
		    }
		}
		ورنہ
		    --skip_گنتی;
		سطر_آغاز = ptr + 1;
		++ptr;
	    }
	    سائز = -1;
	}
	linerest = (طویل)(ptr - سطر_آغاز);
	ui_breakcheck();
    }

failed:
    // not an error, max. number of lines reached
    اگر (error && read_گنتی == 0)
	error = FALSE;

    // In Dos format ignore a trailing CTRL-Z, unless 'binary' is set.
    // In old days the file length was in sector count and the CTRL-Z the
    // marker where the file really ended.  Assuming we write it to a file
    // system that keeps file length properly the CTRL-Z should be dropped.
    // Set the 'endoffile' option so the user can decide what to write later.
    // In Unix format the CTRL-Z is just another character.
    اگر (linerest != 0
	    && !curbuf->b_p_bin
	    && fileformat == EOL_DOS
	    && ptr[-1] == Ctrl_Z)
    {
	ptr--;
	linerest--;
	اگر (سیٹ_options)
	    curbuf->b_p_eof = TRUE;
    }

    // If we get EOF in the middle of a line, note the fact by resetting
    // 'endofline' and add the line normally.
    اگر (!error
	    && !got_int
	    && linerest != 0)
    {
	// remember for when writing
	اگر (سیٹ_options)
	    curbuf->b_p_eol = FALSE;
	*ptr = NUL;
	len = (colnr_T)(ptr - سطر_آغاز + 1);
	اگر (ml_append(lnum, سطر_آغاز, len, newfile) == FAIL)
	    error = TRUE;
	ورنہ
	{
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
	    اگر (read_undo_file)
		sha256_update(&sha_ctx, سطر_آغاز, len);
#اگر_ختم
	    read_no_eol_lnum = ++lnum;
	}
    }

    اگر (سیٹ_options)
	save_file_ff(curbuf);		// remember the current file format

#اگر_تعریف_شدہ FEAT_CRYPT
    اگر (curbuf->b_cryptstate != تہی)
    {
	crypt_free_state(curbuf->b_cryptstate);
	curbuf->b_cryptstate = تہی;
    }
    اگر (cryptkey != تہی && cryptkey != curbuf->b_p_کلید)
	crypt_free_کلید(cryptkey);
    // Don't set cryptkey to NULL, it's used below as a flag that
    // encryption was used.
#اگر_ختم

    // If editing a new file: set 'fenc' for the current buffer.
    // Also for ":read ++edit file".
    اگر (سیٹ_options)
	سیٹ_string_option_براہ_راست((char_u *)"fenc", -1, fenc,
						       OPT_FREE|OPT_LOCAL, 0);
    اگر (fenc_alloced)
	vim_free(fenc);
#اگر_تعریف_شدہ USE_ICONV
    اگر (iconv_fd != (iconv_t)-1)
	iconv_close(iconv_fd);
#اگر_ختم

    اگر (!read_بفر && !read_stdin)
	close(fd);				// errors are ignored
#اگر_تعریف_شدہ HAVE_FD_CLOEXEC
    ورنہ
    {
	صحیح fdflags = fcntl(fd, F_GETFD);

	اگر (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (خالی)fcntl(fd, F_SETFD, fdflags | FD_CLOEXEC);
    }
#اگر_ختم
    vim_free(بفر);

#اگر_تعریف_شدہ HAVE_DUP
    اگر (read_stdin)
    {
	// Use stderr for stdin, makes shell commands work.
	close(0);
	vim_ignored = dup(2);
    }
#اگر_ختم

    اگر (tmpname != تہی)
    {
	mch_remove(tmpname);		// delete converted file
	vim_free(tmpname);
    }
    --no_wait_return;			// may wait for return now

    /*
     * In recovery mode everything but autocommands is skipped.
     */
    اگر (!recoverymode)
    {
	// need to delete the last line, which comes from the empty buffer
	اگر (newfile && wasempty && !(curbuf->b_ml.ml_flags & ML_خالی_خانے))
	{
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	    netbeansFireChanges = 0;
#اگر_ختم
	    ml_delete(curbuf->b_ml.ml_سطر_گنتی);
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	    netbeansFireChanges = 1;
#اگر_ختم
	    --linecnt;
	}
	linecnt = curbuf->b_ml.ml_سطر_گنتی - linecnt;
	اگر (filesize == 0)
	    linecnt = 0;
	اگر (newfile || read_بفر)
	{
	    redraw_curbuf_later(UPD_NOT_VALID);
#اگر_تعریف_شدہ FEAT_DIFF
	    // After reading the text into the buffer the diff info needs to
	    // be updated.
	    diff_invalidate(curbuf);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_FOLDING
	    // All folds in the window are invalid now.  Mark them for update
	    // before triggering autocommands.
	    foldUpdateAll(curwin);
#اگر_ختم
	}
	ورنہ اگر (linecnt)		// appended at least one line
	    appended_سطریں_mark(from, linecnt);

#اگر_غیر_تعریف_شدہ ALWAYS_USE_GUI
	/*
	 * If we were reading from the same terminal as where messages go,
	 * the screen will have been messed up.
	 * Switch on raw mode now and clear the screen.
	 */
	اگر (read_stdin)
	{
	    settmode(TMODE_RAW);	// set to raw mode
	    starttermcap();
	    screenclear();
	}
#اگر_ختم

	اگر (got_int)
	{
	    اگر (!(flags & READ_DUMMY))
	    {
		filemess(curbuf, sfname, (char_u *)_(e_interrupted), 0);
		اگر (newfile)
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    }
	    پیغام_scroll = پیغام_save;
#اگر_تعریف_شدہ FEAT_VIMINFO
	    check_marks_read();
#اگر_ختم
	    retval = OK;	// an interrupt isn't really an error
	    جاؤ theend;
	}

	اگر (!filtering && !(flags & READ_DUMMY))
	{
	    صحیح	buflen;

	    پیغام_جوڑو_fname(curbuf, sfname);   // fname in IObuff with quotes
	    c = FALSE;

	    buflen = (صحیح)STRLEN(IObuff);
#اگر_تعریف_شدہ UNIX
	    اگر (S_ISFIFO(perm))			    // fifo
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[fifo]"));
		c = TRUE;
	    }
	    اگر (S_ISSOCK(perm))			    // or socket
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[socket]"));
		c = TRUE;
	    }
# ifdef OPEN_CHR_FILES
	    اگر (S_ISCHR(perm))			    // or character special
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[character special]"));
		c = TRUE;
	    }
# endif
#اگر_ختم
	    اگر (curbuf->b_p_ro)
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			"%s", shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
		c = TRUE;
	    }
	    اگر (read_no_eol_lnum)
	    {
		پیغام_جوڑو_eol();
		c = TRUE;
	    }
	    اگر (ff_error == EOL_DOS)
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[CR missing]"));
		c = TRUE;
	    }
	    اگر (بانٹو)
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[long lines split]"));
		c = TRUE;
	    }
	    اگر (notconverted)
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[NOT converted]"));
		c = TRUE;
	    }
	    ورنہ اگر (converted)
	    {
		buflen += vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[converted]"));
		c = TRUE;
	    }
#اگر_تعریف_شدہ FEAT_CRYPT
	    اگر (cryptkey != تہی)
	    {
		crypt_append_پیغام(curbuf);
		c = TRUE;
	    }
#اگر_ختم
	    اگر (conv_error != 0)
	    {
		vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[CONVERSION ERROR in line %ld]"), (طویل)conv_error);
		c = TRUE;
	    }
	    ورنہ اگر (illegal_byte > 0)
	    {
		vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[ILLEGAL BYTE in line %ld]"), (طویل)illegal_byte);
		c = TRUE;
	    }
	    ورنہ اگر (error)
	    {
		vim_snprintf((حرف *)IObuff + buflen, IOSIZE - buflen,
			_("[READ ERRORS]"));
		c = TRUE;
	    }
	    اگر (پیغام_جوڑو_fileformat(fileformat))
		c = TRUE;
#اگر_تعریف_شدہ FEAT_CRYPT
	    اگر (cryptkey != تہی)
		پیغام_جوڑو_سطریں(c, (طویل)linecnt, filesize
			 - crypt_get_سرخی_len(crypt_get_طریقہ_nr(curbuf)));
	    ورنہ
#اگر_ختم
		پیغام_جوڑو_سطریں(c, (طویل)linecnt, filesize);

	    VIM_CLEAR(keep_پیغام);
	    پیغام_scrolled_ign = TRUE;
#اگر_تعریف_شدہ ALWAYS_USE_GUI
	    // Don't show the message when reading stdin, it would end up in a
	    // message box (which might be shown when exiting!)
	    اگر (read_stdin || read_بفر)
		p = پیغام_may_trunc(FALSE, IObuff);
	    ورنہ
#اگر_ختم
	    {
		اگر (پیغام_ستون > 0)
		    پیغام_putchar('\r');  // overwrite previous message
		p = (char_u *)پیغام_trunc_attr((حرف *)IObuff, FALSE, 0);
	    }
	    اگر (read_stdin || read_بفر || restart_edit != 0
		    || (پیغام_scrolled != 0 && !need_wait_return))
		// Need to repeat the message after redrawing when:
		// - When reading from stdin (the screen will be cleared next).
		// - When restart_edit is set (otherwise there will be a delay
		//   before redrawing).
		// - When the screen was scrolled but there is no wait-return
		//   prompt.
		سیٹ_keep_پیغام(p, 0);
	    پیغام_scrolled_ign = FALSE;
	}

	// with errors writing the file requires ":w!"
	اگر (newfile && (error
		    || conv_error != 0
		    || (illegal_byte > 0 && bad_char_behavior != BAD_KEEP)))
	    curbuf->b_p_ro = TRUE;

	u_clearline();	    // cannot use "U" command after adding lines

	/*
	 * In Ex mode: cursor at last new line.
	 * Otherwise: cursor at first new line.
	 */
	اگر (exmode_فعال)
	    curwin->w_cursor.lnum = from + linecnt;
	ورنہ
	    curwin->w_cursor.lnum = from + 1;
	check_cursor_lnum();
	beginline(BL_WHITE | BL_FIX);	    // on first non-blank

	اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set '[ and '] marks to the newly read lines.
	    curbuf->b_op_آغاز.lnum = from + 1;
	    curbuf->b_op_آغاز.ستون = 0;
	    curbuf->b_op_انتہا.lnum = from + linecnt;
	    curbuf->b_op_انتہا.ستون = 0;
	}

#اگر_تعریف_شدہ MSWIN
	/*
	 * Work around a weird problem: When a file has two links (only
	 * possible on NTFS) and we write through one link, then stat() it
	 * through the other link, the timestamp information may be wrong.
	 * It's correct again after reading the file, thus reset the timestamp
	 * here.
	 */
	اگر (newfile && !read_stdin && !read_بفر
					 && mch_stat((حرف *)fname, &st) >= 0)
	{
	    buf_store_وقت(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
	}
#اگر_ختم
    }
    پیغام_scroll = پیغام_save;

#اگر_تعریف_شدہ FEAT_VIMINFO
    /*
     * Get the marks before executing autocommands, so they can be used there.
     */
    check_marks_read();
#اگر_ختم

    /*
     * We remember if the last line of the read didn't have
     * an eol even when 'binary' is off, to support turning 'fixeol' off,
     * or writing the read again with 'binary' on.  The latter is required
     * for ":autocmd FileReadPost *.gz set bin|'[,']!gunzip" to work.
     */
    curbuf->b_no_eol_lnum = read_no_eol_lnum;

    // When reloading a buffer put the cursor at the first line that is
    // different.
    اگر (flags & READ_KEEP_UNDO)
	u_ڈھونڈو_پہلا_بدلا();

#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
    /*
     * When opening a new file locate undo info and read it.
     */
    اگر (read_undo_file)
    {
	char_u	hash[UNDO_HASH_سائز];

	sha256_اختتام(&sha_ctx, hash);
	u_read_undo(تہی, hash, fname);
    }
#اگر_ختم

    اگر (!read_stdin && !read_fifo && (!read_بفر || sfname != تہی))
    {
	صحیح m = پیغام_scroll;
	صحیح n = پیغام_scrolled;

	// Save the fileformat now, otherwise the buffer will be considered
	// modified if the format/encoding was automatically detected.
	اگر (سیٹ_options)
	    save_file_ff(curbuf);

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	پیغام_scroll = TRUE;
	اگر (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPOST, تہی, sfname,
							  FALSE, curbuf, eap);
	ورنہ اگر (newfile || (read_بفر && sfname != تہی))
	{
	    apply_autocmds_exarg(EVENT_BUFREADPOST, تہی, sfname,
							  FALSE, curbuf, eap);
	    اگر (!curbuf->b_au_did_filetype && *curbuf->b_p_ft != NUL)
		/*
		 * EVENT_FILETYPE was not triggered but the buffer already has a
		 * filetype. Trigger EVENT_FILETYPE using the existing filetype.
		 */
		apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
			TRUE, curbuf);
	}
	ورنہ
	    apply_autocmds_exarg(EVENT_FILEREADPOST, sfname, sfname,
							    FALSE, تہی, eap);
	اگر (پیغام_scrolled == n)
	    پیغام_scroll = m;
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (aborting())	    // autocmds may abort script processing
	    جاؤ theend;
#اگر_ختم
    }

    اگر (!(recoverymode && error))
	retval = OK;

theend:
    اگر (curbuf->b_ml.ml_mfp != تہی
		       && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	// OK to sync the swap file now
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    واپس retval;
}

#اگر defined(OPEN_CHR_FILES)
/*
 * Returns TRUE if the file name argument is of the form "/dev/fd/\d\+",
 * which is the name of files used for process substitution output by
 * some shells on some operating systems, e.g., bash on SunOS.
 * Do not accept "/dev/fd/[012]", opening these may hang Vim.
 */
    صحیح
is_dev_fd_file(char_u *fname)
{
    واپس (STRNCMP(fname, "/dev/fd/", 8) == 0
	    && VIM_ISDIGIT(fname[8])
	    && *skipdigits(fname + 9) == NUL
	    && (fname[9] != NUL
		|| (fname[8] != '0' && fname[8] != '1' && fname[8] != '2')));
}
#اگر_ختم

/*
 * From the current line count and characters read after that, estimate the
 * line number where we are now.
 * Used for error messages that include a line number.
 */
    ساکن linenr_T
readfile_linenr(
    linenr_T	linecnt,	// line count before reading more bytes
    char_u	*p,		// start of more bytes read
    char_u	*endp)		// end of more bytes read
{
    char_u	*s;
    linenr_T	lnum;

    lnum = curbuf->b_ml.ml_سطر_گنتی - linecnt + 1;
    برائے (s = p; s < endp; ++s)
	اگر (*s == '\n')
	    ++lnum;
    واپس lnum;
}

/*
 * Fill "*eap" to force the 'fileencoding', 'fileformat' and 'binary' to be
 * equal to the buffer "buf".  Used for calling readfile().
 * Returns OK or FAIL.
 */
    صحیح
prep_exarg(exarg_T *eap, buf_T *buf)
{
    eap->cmd = alloc(15 + (غیرعلامتی)STRLEN(buf->b_p_fenc));
    اگر (eap->cmd == تہی)
	واپس FAIL;

    sprintf((حرف *)eap->cmd, "e ++enc=%s", buf->b_p_fenc);
    eap->force_enc = 8;
    eap->bad_char = buf->b_bad_char;
    eap->force_ff = *buf->b_p_ff;

    eap->force_bin = buf->b_p_bin ? FORCE_BIN : FORCE_NOBIN;
    eap->read_edit = FALSE;
    eap->forceit = FALSE;
    واپس OK;
}

/*
 * Set default or forced 'fileformat' and 'binary'.
 */
    خالی
سیٹ_file_options(صحیح سیٹ_options, exarg_T *eap)
{
    // set default 'fileformat'
    اگر (سیٹ_options)
    {
	اگر (eap != تہی && eap->force_ff != 0)
	    سیٹ_fileformat(get_fileformat_force(curbuf, eap), OPT_LOCAL);
	ورنہ اگر (*p_ffs != NUL)
	    سیٹ_fileformat(default_fileformat(), OPT_LOCAL);
    }

    // set or reset 'binary'
    اگر (eap != تہی && eap->force_bin != 0)
    {
	صحیح	oldval = curbuf->b_p_bin;

	curbuf->b_p_bin = (eap->force_bin == FORCE_BIN);
	سیٹ_options_bin(oldval, curbuf->b_p_bin, OPT_LOCAL);
    }
}

/*
 * Set forced 'fileencoding'.
 */
    خالی
سیٹ_forced_fenc(exarg_T *eap)
{
    اگر (eap->force_enc == 0)
	واپس;

    char_u *fenc = enc_canonize(eap->cmd + eap->force_enc);

    اگر (fenc != تہی)
	سیٹ_string_option_براہ_راست((char_u *)"fenc", -1,
		fenc, OPT_FREE|OPT_LOCAL, 0);
    vim_free(fenc);
}

/*
 * Find next fileencoding to use from 'fileencodings'.
 * "pp" points to fenc_next.  It's advanced to the next item.
 * When there are no more items, an empty string is returned and *pp is set to
 * NULL.
 * When *pp is not set to NULL, the result is in allocated memory and "alloced"
 * is set to TRUE.
 */
    ساکن char_u *
اگلا_fenc(char_u **pp, صحیح *alloced)
{
    char_u	*p;
    char_u	*r;

    *alloced = FALSE;
    اگر (**pp == NUL)
    {
	*pp = تہی;
	واپس (char_u *)"";
    }
    p = vim_strchr(*pp, ',');
    اگر (p == تہی)
    {
	r = enc_canonize(*pp);
	*pp += STRLEN(*pp);
    }
    ورنہ
    {
	r = vim_strnsave(*pp, p - *pp);
	*pp = p + 1;
	اگر (r != تہی)
	{
	    p = enc_canonize(r);
	    vim_free(r);
	    r = p;
	}
    }
    اگر (r != تہی)
	*alloced = TRUE;
    ورنہ
    {
	// out of memory
	r = (char_u *)"";
	*pp = تہی;
    }
    واپس r;
}

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Convert a file with the 'charconvert' expression.
 * This closes the file which is to be read, converts it and opens the
 * resulting file for reading.
 * Returns name of the resulting converted file (the caller should delete it
 * after reading it).
 * Returns NULL if the conversion failed ("*fdp" is not set) .
 */
    ساکن char_u *
readfile_charconvert(
    char_u	*fname,		// name of input file
    char_u	*fenc,		// converted from
    صحیح		*fdp)		// in/out: file descriptor of file
{
    char_u	*tmpname;
    حرف	*errmsg = تہی;

    tmpname = vim_tempname('r', FALSE);
    اگر (tmpname == تہی)
	errmsg = _("Can't find temp file for conversion");
    ورنہ
    {
	close(*fdp);		// close the input file, ignore errors
	*fdp = -1;
	اگر (eval_charconvert(fenc, enc_utf8 ? (char_u *)"utf-8" : p_enc,
						      fname, tmpname) == FAIL)
	    errmsg = _("Conversion with 'charconvert' failed");
	اگر (errmsg == تہی && (*fdp = mch_open((حرف *)tmpname,
						  O_RDONLY | O_EXTRA, 0)) < 0)
	    errmsg = _("can't read output of 'charconvert'");
    }

    اگر (errmsg != تہی)
    {
	// Don't use emsg(), it breaks mappings, the retry with
	// another type of conversion might still work.
	پیغام(errmsg);
	اگر (tmpname != تہی)
	{
	    mch_remove(tmpname);	// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    // If the input file is closed, open it (caller should check for error).
    اگر (*fdp < 0)
	*fdp = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);

    واپس tmpname;
}
#اگر_ختم

#اگر defined(FEAT_CRYPT)
/*
 * Check for magic number used for encryption.  Applies to the current buffer.
 * If found, the magic number is removed from ptr[*sizep] and *sizep and
 * *filesizep are updated.
 * Return the (new) encryption key, NULL for no encryption.
 */
    ساکن char_u *
check_for_cryptkey(
    char_u	*cryptkey,	// previous encryption key or NULL
    char_u	*ptr,		// pointer to read bytes
    طویل	*sizep,		// length of read bytes
    off_T	*filesizep,	// nr of bytes used from file
    صحیح		newfile,	// editing a new buffer
    char_u	*fname,		// file name to display
    صحیح		*did_ask)	// flag: whether already asked for key
{
    صحیح طریقہ = crypt_طریقہ_nr_from_magic((حرف *)ptr, *sizep);
    صحیح b_p_ro = curbuf->b_p_ro;

    اگر (طریقہ >= 0)
    {
	// Mark the buffer as read-only until the decryption has taken place.
	// Avoids accidentally overwriting the file with garbage.
	curbuf->b_p_ro = TRUE;

	// Set the cryptmethod local to the buffer.
	crypt_سیٹ_cm_option(curbuf, طریقہ);
	اگر (cryptkey == تہی && !*did_ask)
	{
	    اگر (*curbuf->b_p_کلید)
	    {
		cryptkey = curbuf->b_p_کلید;
		crypt_check_swapfile_curbuf();
	    }
	    ورنہ
	    {
		// When newfile is TRUE, store the typed key in the 'key'
		// option and don't free it.  bf needs hash of the key saved.
		// Don't ask for the key again when first time Enter was hit.
		// Happens when retrying to detect encoding.
		smsg(_(need_کلید_پیغام), fname);
		پیغام_scroll = TRUE;
		crypt_check_طریقہ(طریقہ);
		cryptkey = crypt_get_کلید(newfile, FALSE);
		*did_ask = TRUE;

		// check if empty key entered
		اگر (cryptkey != تہی && *cryptkey == NUL)
		{
		    اگر (cryptkey != curbuf->b_p_کلید)
			vim_free(cryptkey);
		    cryptkey = تہی;
		}
	    }
	}

	اگر (cryptkey != تہی)
	{
	    صحیح سرخی_len;

	    سرخی_len = crypt_get_سرخی_len(طریقہ);
	    اگر (*sizep < سرخی_len)
	    {
		// invalid header, buffer can't be encrypted
		اگر (cryptkey != curbuf->b_p_کلید)
		    vim_free(cryptkey);
		واپس تہی;
	    }

	    curbuf->b_cryptstate = crypt_create_from_سرخی(
							طریقہ, cryptkey, ptr);
	    crypt_سیٹ_cm_option(curbuf, طریقہ);

	    // Remove cryptmethod specific header from the text.
	    *filesizep += سرخی_len;
	    *sizep -= سرخی_len;
	    mch_memmove(ptr, ptr + سرخی_len, (size_t)*sizep);

	    // Restore the read-only flag.
	    curbuf->b_p_ro = b_p_ro;
	}
    }
    // When starting to edit a new file which does not have encryption, clear
    // the 'key' option, except when starting up (called with -x argument)
    ورنہ اگر (newfile && *curbuf->b_p_کلید != NUL && !starting)
	سیٹ_option_قدر_give_err((char_u *)"key", 0L, (char_u *)"", OPT_LOCAL);

    واپس cryptkey;
}
#اگر_ختم  // FEAT_CRYPT

/*
 * Return TRUE if a file appears to be read-only from the file permissions.
 */
    صحیح
check_file_readonly(
    char_u	*fname,		// full path to file
    صحیح		perm غیر_مستعمل)	// known permissions on file
{
#اگر_غیر_تعریف_شدہ USE_MCH_ACCESS
    صحیح	    fd = 0;
#اگر_ختم

    واپس (
#اگر_تعریف_شدہ USE_MCH_ACCESS
# ifdef UNIX
	(perm & 0222) == 0 ||
# endif
	mch_access((حرف *)fname, W_OK)
#ورنہ
	(fd = mch_open((حرف *)fname, O_RDWR | O_EXTRA, 0)) < 0
					? TRUE : (close(fd), FALSE)
#اگر_ختم
	);
}

#اگر defined(HAVE_FSYNC)
/*
 * Call fsync() with Mac-specific exception.
 * Return fsync() result: zero for success.
 */
    صحیح
vim_fsync(صحیح fd)
{
    صحیح r;

# ifdef MACOS_X
    r = fcntl(fd, F_FULLFSYNC);
    اگر (r != 0)  // F_FULLFSYNC not working or not supported
# endif
	r = fsync(fd);
    واپس r;
}
#اگر_ختم

/*
 * Set the name of the current buffer.  Use when the buffer doesn't have a
 * name and a ":r" or ":w" command with a file name is used.
 */
    صحیح
سیٹ_rw_fname(char_u *fname, char_u *sfname)
{
    buf_T	*buf = curbuf;

    // It's like the unnamed buffer is deleted....
    اگر (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFDELETE, تہی, تہی, FALSE, curbuf);
    apply_autocmds(EVENT_BUFWIPEOUT, تہی, تہی, FALSE, curbuf);
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (aborting())	    // autocmds may abort script processing
	واپس FAIL;
#اگر_ختم
    اگر (curbuf != buf)
    {
	// We are in another buffer now, don't do the renaming.
	emsg(_(e_autocommands_بدلا_بفر_or_بفر_name));
	واپس FAIL;
    }

    اگر (setfname(curbuf, fname, sfname, FALSE) == OK)
	curbuf->b_flags |= BF_NOTEDITED;

    // ....and a new named one is created
    apply_autocmds(EVENT_BUFNEW, تہی, تہی, FALSE, curbuf);
    اگر (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFADD, تہی, تہی, FALSE, curbuf);
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (aborting())	    // autocmds may abort script processing
	واپس FAIL;
#اگر_ختم

    // Do filetype detection now if 'filetype' is empty.
    اگر (*curbuf->b_p_ft == NUL)
    {
	اگر (au_has_گروہ((char_u *)"filetypedetect"))
	    (خالی)do_doautocmd((char_u *)"filetypedetect BufRead", FALSE, تہی);
	do_modelines(0);
    }

    واپس OK;
}

/*
 * Put file name into IObuff with quotes.
 */
    خالی
پیغام_جوڑو_fname(buf_T *buf, char_u *fname)
{
    size_t  IObufflen = 0;

    اگر (fname == تہی)
	fname = (char_u *)"-stdin-";
    IObuff[IObufflen++] = '"';
    IObufflen += home_replace(buf, fname, IObuff + IObufflen, IOSIZE - 4, TRUE);
    STRCPY(IObuff + IObufflen, "\" ");
}

/*
 * Append message for text mode to IObuff.
 * Return TRUE if something appended.
 */
    صحیح
پیغام_جوڑو_fileformat(صحیح eol_type)
{
#اگر_غیر_تعریف_شدہ USE_CRNL
    اگر (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_متن) ? _("[dos]") : _("[dos format]"));
	واپس TRUE;
    }
#اگر_ختم
    اگر (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_متن) ? _("[mac]") : _("[mac format]"));
	واپس TRUE;
    }
#اگر_تعریف_شدہ USE_CRNL
    اگر (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_متن) ? _("[unix]") : _("[unix format]"));
	واپس TRUE;
    }
#اگر_ختم
    واپس FALSE;
}

/*
 * Append line and character count to IObuff.
 */
    خالی
پیغام_جوڑو_سطریں(
    صحیح	    داخل_کرو_space,
    طویل    lnum,
    off_T   nchars)
{
    صحیح  len = (صحیح)STRLEN(IObuff);

    اگر (shortmess(SHM_سطریں))
	vim_snprintf((حرف *)IObuff + len, IOSIZE - (size_t)len,
		// l10n: L as in line, B as in byte
		_("%s%ldL, %lldB"), داخل_کرو_space ? " " : "", lnum, (varnumber_T)nchars);
    ورنہ
    {
	len += vim_snprintf((حرف *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%s%ld line, ", "%s%ld lines, ", lnum), داخل_کرو_space ? " " : "", lnum);
	vim_snprintf((حرف *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%lld byte", "%lld bytes", nchars), (varnumber_T)nchars);
    }
}

/*
 * Append message for missing line separator to IObuff.
 */
    خالی
پیغام_جوڑو_eol(خالی)
{
    STRCAT(IObuff, shortmess(SHM_آخری) ? _("[noeol]") : _("[Incomplete last line]"));
}

    صحیح
وقت_differs(stat_T *st, طویل mtime, طویل mtime_ns غیر_مستعمل)
{
    واپس
#اگر_تعریف_شدہ ST_MTIM_NSEC
	(طویل)st->ST_MTIM_NSEC != mtime_ns ||
#اگر_ختم
#اگر defined(__linux__) || defined(MSWIN)
	// On a FAT filesystem, esp. under Linux, there are only 5 bits to store
	// the seconds.  Since the roundoff is done when flushing the inode, the
	// time may change unexpectedly by one second!!!
	(طویل)st->st_mtime - mtime > 1 || mtime - (طویل)st->st_mtime > 1
#ورنہ
	(طویل)st->st_mtime != mtime
#اگر_ختم
	;
}

/*
 * Return TRUE if file encoding "fenc" requires conversion from or to
 * 'encoding'.
 */
    صحیح
need_conversion(char_u *fenc)
{
    صحیح		same_encoding;
    صحیح		enc_flags;
    صحیح		fenc_flags;

    اگر (*fenc == NUL || STRCMP(p_enc, fenc) == 0)
    {
	same_encoding = TRUE;
	fenc_flags = 0;
    }
    ورنہ
    {
	// Ignore difference between "ansi" and "latin1", "ucs-4" and
	// "ucs-4be", etc.
	enc_flags = get_fio_flags(p_enc);
	fenc_flags = get_fio_flags(fenc);
	same_encoding = (enc_flags != 0 && fenc_flags == enc_flags);
    }
    اگر (same_encoding)
    {
	// Specified encoding matches with 'encoding'.  This requires
	// conversion when 'encoding' is Unicode but not UTF-8.
	واپس enc_unicode != 0;
    }

    // Encodings differ.  However, conversion is not needed when 'enc' is any
    // Unicode encoding and the file is UTF-8.
    واپس !(enc_utf8 && fenc_flags == FIO_UTF8);
}

/*
 * Check "ptr" for a unicode encoding and return the FIO_ flags needed for the
 * internal conversion.
 * if "ptr" is an empty string, use 'encoding'.
 */
    صحیح
get_fio_flags(char_u *ptr)
{
    صحیح		prop;

    اگر (*ptr == NUL)
	ptr = p_enc;

    prop = enc_canon_props(ptr);
    اگر (prop & ENC_UNICODE)
    {
	اگر (prop & ENC_2BYTE)
	{
	    اگر (prop & ENC_ENDIAN_L)
		واپس FIO_UCS2 | FIO_ENDIAN_L;
	    واپس FIO_UCS2;
	}
	اگر (prop & ENC_4BYTE)
	{
	    اگر (prop & ENC_ENDIAN_L)
		واپس FIO_UCS4 | FIO_ENDIAN_L;
	    واپس FIO_UCS4;
	}
	اگر (prop & ENC_2_لفظ)
	{
	    اگر (prop & ENC_ENDIAN_L)
		واپس FIO_UTF16 | FIO_ENDIAN_L;
	    واپس FIO_UTF16;
	}
	واپس FIO_UTF8;
    }
    اگر (prop & ENC_LATIN1)
	واپس FIO_LATIN1;
    // must be ENC_DBCS, requires iconv()
    واپس 0;
}

#اگر defined(MSWIN)
/*
 * Check "ptr" for a MS-Windows codepage name and return the FIO_ flags needed
 * for the conversion MS-Windows can do for us.  Also accept "utf-8".
 * Used for conversion between 'encoding' and 'fileencoding'.
 */
    صحیح
get_win_fio_flags(char_u *ptr)
{
    صحیح		cp;

    // Cannot do this when 'encoding' is not utf-8 and not a codepage.
    اگر (!enc_utf8 && enc_codepage <= 0)
	واپس 0;

    cp = encname2codepage(ptr);
    اگر (cp == 0)
    {
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	اگر (STRCMP(ptr, "utf-8") == 0)
	    cp = CP_UTF8;
	ورنہ
# endif
	    واپس 0;
    }
    واپس FIO_PUT_CP(cp) | FIO_CODEPAGE;
}
#اگر_ختم

#اگر defined(MACOS_CONVERT)
/*
 * Check "ptr" for a Carbon supported encoding and return the FIO_ flags
 * needed for the internal conversion to/from utf-8 or latin1.
 */
    صحیح
get_mac_fio_flags(char_u *ptr)
{
    اگر ((enc_utf8 || STRCMP(p_enc, "latin1") == 0)
				     && (enc_canon_props(ptr) & ENC_MACROMAN))
	واپس FIO_MACROMAN;
    واپس 0;
}
#اگر_ختم

/*
 * Check for a Unicode BOM (Byte Order Mark) at the start of p[size].
 * "size" must be at least 2.
 * Return the name of the encoding and set "*lenp" to the length.
 * Returns NULL when no BOM found.
 */
    ساکن char_u *
check_for_bom(
    char_u	*p,
    طویل	سائز,
    صحیح		*lenp,
    صحیح		flags)
{
    حرف	*name = تہی;
    صحیح		len = 2;

    اگر (p[0] == 0xef && p[1] == 0xbb && سائز >= 3 && p[2] == 0xbf
	    && (flags == FIO_ALL || flags == FIO_UTF8 || flags == 0))
    {
	name = "utf-8";		// EF BB BF
	len = 3;
    }
    ورنہ اگر (p[0] == 0xff && p[1] == 0xfe)
    {
	اگر (سائز >= 4 && p[2] == 0 && p[3] == 0
	    && (flags == FIO_ALL || flags == (FIO_UCS4 | FIO_ENDIAN_L)))
	{
	    name = "ucs-4le";	// FF FE 00 00
	    len = 4;
	}
	ورنہ اگر (flags == (FIO_UCS2 | FIO_ENDIAN_L))
	    name = "ucs-2le";	// FF FE
	ورنہ اگر (flags == FIO_ALL || flags == (FIO_UTF16 | FIO_ENDIAN_L))
	    // utf-16le is preferred, it also works for ucs-2le text
	    name = "utf-16le";	// FF FE
    }
    ورنہ اگر (p[0] == 0xfe && p[1] == 0xff
	    && (flags == FIO_ALL || flags == FIO_UCS2 || flags == FIO_UTF16))
    {
	// Default to utf-16, it works also for ucs-2 text.
	اگر (flags == FIO_UCS2)
	    name = "ucs-2";	// FE FF
	ورنہ
	    name = "utf-16";	// FE FF
    }
    ورنہ اگر (سائز >= 4 && p[0] == 0 && p[1] == 0 && p[2] == 0xfe
	    && p[3] == 0xff && (flags == FIO_ALL || flags == FIO_UCS4))
    {
	name = "ucs-4";		// 00 00 FE FF
	len = 4;
    }

    *lenp = len;
    واپس (char_u *)name;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns "full_path" or pointer into "full_path" if shortened.
 */
    char_u *
shorten_fname1(char_u *بھرا_راستہ)
{
    char_u	*dirname;
    char_u	*p = بھرا_راستہ;

    dirname = alloc(MAXPATHL);
    اگر (dirname == تہی)
	واپس بھرا_راستہ;
    اگر (mch_dirname(dirname, MAXPATHL) == OK)
    {
	p = shorten_fname(بھرا_راستہ, dirname);
	اگر (p == تہی || *p == NUL)
	    p = بھرا_راستہ;
    }
    vim_free(dirname);
    واپس p;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns NULL if not shorter name possible, pointer into "full_path"
 * otherwise.
 */
    char_u *
shorten_fname(char_u *بھرا_راستہ, char_u *dir_name)
{
    صحیح		len;
    char_u	*p;

    اگر (بھرا_راستہ == تہی)
	واپس تہی;
    len = (صحیح)STRLEN(dir_name);
    اگر (fnamencmp(dir_name, بھرا_راستہ, len) == 0)
    {
	p = بھرا_راستہ + len;
#اگر defined(MSWIN)
	/*
	 * MS-Windows: when a file is in the root directory, dir_name will end
	 * in a slash, since C: by itself does not define a specific dir. In
	 * this case p may already be correct. <negri>
	 */
	اگر (!((len > 2) && (*(p - 2) == ':')))
#اگر_ختم
	{
	    اگر (vim_ispathsep(*p))
		کرو
		    ++p;
		جب (vim_ispathsep_nocolon(*p));
#اگر_غیر_تعریف_شدہ VMS   // the path separator is always part of the path
	    ورنہ
		p = تہی;
#اگر_ختم
	}
    }
#اگر defined(MSWIN)
    /*
     * When using a file in the current drive, remove the drive name:
     * "A:\dir\file" -> "\dir\file".  This helps when moving a session file on
     * a floppy from "A:\dir" to "B:\dir".
     */
    ورنہ اگر (len > 3
	    && TOUPPER_LOC(بھرا_راستہ[0]) == TOUPPER_LOC(dir_name[0])
	    && بھرا_راستہ[1] == ':'
	    && vim_ispathsep(بھرا_راستہ[2]))
	p = بھرا_راستہ + 2;
#اگر_ختم
    ورنہ
	p = تہی;
    واپس p;
}

/*
 * Shorten filename of a buffer.
 * When "force" is TRUE: Use full path from now on for files currently being
 * edited, both for file name and swap file name.  Try to shorten the file
 * names a bit, if safe to do so.
 * When "force" is FALSE: Only try to shorten absolute file names.
 * For buffers that have buftype "nofile" or "scratch": never change the file
 * name.
 */
    خالی
shorten_buf_fname(buf_T *buf, char_u *dirname, صحیح force)
{
    char_u	*p;

    اگر (buf->b_fname != تہی
	    && !bt_nofilename(buf)
	    && !راستہ_with_url(buf->b_fname)
	    && (force
		|| buf->b_sfname == تہی
		|| mch_is_بھرا_Name(buf->b_sfname)))
    {
	اگر (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	p = shorten_fname(buf->b_ffname, dirname);
	اگر (p != تہی)
	{
	    buf->b_sfname = vim_strsave(p);
	    buf->b_fname = buf->b_sfname;
	}
	اگر (p == تہی || buf->b_fname == تہی)
	    buf->b_fname = buf->b_ffname;
    }
}

/*
 * Shorten filenames for all buffers.
 */
    خالی
shorten_fnames(صحیح force)
{
    char_u	dirname[MAXPATHL];
    buf_T	*buf;

    mch_dirname(dirname, MAXPATHL);
    FOR_ALL_BUFFERS(buf)
    {
	shorten_buf_fname(buf, dirname, force);

	// Always make the swap file name a full path, a "nofile" buffer may
	// also have a swap file.
	mf_fullname(buf->b_ml.ml_mfp);
    }
    حالت_redraw_all();
    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#اگر_ختم
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    popup_update_preview_title();
#اگر_ختم
}

#اگر (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
/*
 * Shorten all filenames in "fnames[count]" by current directory.
 */
    خالی
shorten_filenames(char_u **fnames, صحیح گنتی)
{
    صحیح		i;
    char_u	dirname[MAXPATHL];
    char_u	*p;

    اگر (fnames == تہی || گنتی < 1)
	واپس;
    mch_dirname(dirname, حجم(dirname));
    برائے (i = 0; i < گنتی; ++i)
    {
	اگر ((p = shorten_fname(fnames[i], dirname)) != تہی)
	{
	    // shorten_fname() returns pointer in given "fnames[i]".  If free
	    // "fnames[i]" first, "p" becomes invalid.  So we need to copy
	    // "p" first then free fnames[i].
	    p = vim_strsave(p);
	    vim_free(fnames[i]);
	    fnames[i] = p;
	}
    }
}
#اگر_ختم

/*
 * Add extension to file name - change path/fo.o.h to path/fo.o.h.ext or
 * fo_o_h.ext for MSDOS or when shortname option set.
 *
 * Assumed that fname is a valid name found in the filesystem we assure that
 * the return value is a different name and ends in 'ext'.
 * "ext" MUST be at most 4 characters long if it starts with a dot, 3
 * characters otherwise.
 * Space for the returned name is allocated, must be freed later.
 * Returns NULL when out of memory.
 */
    char_u *
modname(
    char_u *fname,
    char_u *ext,
    صحیح	    prepend_dot)	// may prepend a '.' to file name
{
    واپس buf_modname((curbuf->b_p_sn || curbuf->b_shortname),
						     fname, ext, prepend_dot);
}

    char_u *
buf_modname(
    صحیح	    shortname,		// use 8.3 file name
    char_u  *fname,
    char_u  *ext,
    صحیح	    prepend_dot)	// may prepend a '.' to file name
{
    char_u	*retval;
    char_u	*s;
    char_u	*e;
    char_u	*ptr;
    size_t	ptrlen;
    صحیح		fnamelen, extlen;

    extlen = (صحیح)STRLEN(ext);

    /*
     * If there is no file name we must get the name of the current directory
     * (we need the full path in case :cd is used).
     */
    اگر (fname == تہی || *fname == NUL)
    {
	retval = alloc(MAXPATHL + extlen + 3);
	اگر (retval == تہی)
	    واپس تہی;
	اگر (mch_dirname(retval, MAXPATHL) == FAIL ||
				     (fnamelen = (صحیح)STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    واپس تہی;
	}
	اگر (!after_pathsep(retval, retval + fnamelen))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
	prepend_dot = FALSE;	    // nothing to prepend a dot to
    }
    ورنہ
    {
	fnamelen = (صحیح)STRLEN(fname);
	retval = alloc(fnamelen + extlen + 3);
	اگر (retval == تہی)
	    واپس تہی;
	STRCPY(retval, fname);
#اگر_تعریف_شدہ VMS
	vms_remove_نسخہ(retval); // we do not need versions here
#اگر_ختم
    }

    /*
     * search backwards until we hit a '/', '\' or ':' replacing all '.'
     * by '_' for MSDOS or when shortname option set and ext starts with a dot.
     * Then truncate what is after the '/', '\' or ':' to 8 characters for
     * MSDOS and 26 characters for AMIGA, a lot more for UNIX.
     */
    برائے (ptr = retval + fnamelen; ptr > retval; MB_PTR_عقب(retval, ptr))
    {
	اگر (*ext == '.' && shortname)
	    اگر (*ptr == '.')	// replace '.' by '_'
		*ptr = '_';
	اگر (vim_ispathsep(*ptr))
	{
	    ++ptr;
	    توڑو;
	}
    }

    // the file name has at most BASENAMELEN characters.
    ptrlen = (size_t)(fnamelen - (ptr - retval));
    اگر (ptrlen > (غیرعلامتی)BASENAMELEN)
    {
	ptrlen = BASENAMELEN;
	ptr[ptrlen] = NUL;
    }

    s = ptr + ptrlen;

    /*
     * For 8.3 file names we may have to reduce the length.
     */
    اگر (shortname)
    {
	/*
	 * If there is no file name, or the file name ends in '/', and the
	 * extension starts with '.', put a '_' before the dot, because just
	 * ".ext" is invalid.
	 */
	اگر (fname == تہی || *fname == NUL
				   || vim_ispathsep(fname[STRLEN(fname) - 1]))
	{
	    اگر (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	ورنہ اگر (*ext == '.')
	{
	    اگر ((size_t)(s - ptr) > (size_t)8)
	    {
		s = ptr + 8;
		*s = '\0';
	    }
	}
	/*
	 * If the extension doesn't start with '.', and the file name
	 * doesn't have an extension yet, append a '.'
	 */
	ورنہ اگر ((e = vim_strchr(ptr, '.')) == تہی)
	    *s++ = '.';
	/*
	 * If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be truncated
	 */
	ورنہ اگر ((صحیح)(ptrlen - (e - retval)) + extlen > 4)
	    s = e + 4 - extlen;
    }
#اگر_تعریف_شدہ MSWIN
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    ورنہ اگر ((fname == تہی || *fname == NUL) && *ext == '.')
	*s++ = '_';
#اگر_ختم

    /*
     * Append the extension.
     * ext can start with '.' and cannot exceed 3 more characters.
     */
    STRCPY(s, ext);
    /*
     * Prepend the dot.
     */
    اگر (prepend_dot && !shortname && *(e = gettail(retval)) != '.')
    {
	mch_memmove(e + 1, e, (size_t)(((fnamelen + extlen) - (e - retval)) + 1));	// +1 for NUL
	*e = '.';
    }

    /*
     * Check that, after appending the extension, the file name is really
     * different.
     */
    اگر (fname != تہی && STRCMP(fname, retval) == 0)
    {
	// we search for a character that can be replaced by '_'
	جب (--s >= ptr)
	{
	    اگر (*s != '_')
	    {
		*s = '_';
		توڑو;
	    }
	}
	اگر (s < ptr)	// fname was "________.<ext>", how tricky!
	    *ptr = 'v';
    }
    واپس retval;
}

/*
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE for end-of-file.
 * If the line is truncated then buf[size - 2] will not be NUL.
 */
    صحیح
vim_fgets(char_u *buf, صحیح سائز, FILE *fp)
{
    حرف	*eof;
#تعریف_کرو FGETS_سائز 200
    حرف	tbuf[FGETS_سائز];

    // safety check
    اگر (سائز < 2)
    {
	اگر (سائز == 1)
	    buf[0] = NUL;
	واپس TRUE;
    }

    buf[سائز - 2] = NUL;
    eof = fgets((حرف *)buf, سائز, fp);
    اگر (buf[سائز - 2] != NUL && buf[سائز - 2] != '\n')
    {
	buf[سائز - 1] = NUL;	    // Truncate the line

	// Now throw away the rest of the line:
	کرو
	{
	    tbuf[FGETS_سائز - 2] = NUL;
	    vim_ignoredp = fgets((حرف *)tbuf, FGETS_سائز, fp);
	} جب (tbuf[FGETS_سائز - 2] != NUL && tbuf[FGETS_سائز - 2] != '\n');
    }
    واپس (eof == تہی);
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 * Return -1 for failure, 0 for success.
 */
    صحیح
vim_rename(char_u *from, char_u *مقصد)
{
    صحیح		n;
    صحیح		ret;
#اگر_تعریف_شدہ AMIGA
    BPTR	flock;
#اگر_ختم
    stat_T	st;
    صحیح		use_tmp_file = FALSE;

    /*
     * When the names are identical, there is nothing to do.  When they refer
     * to the same file (ignoring case and slash/backslash differences) but
     * the file name differs we need to go through a temp file.
     */
    اگر (fnamecmp(from, مقصد) == 0)
    {
	اگر (p_fic && STRCMP(gettail(from), gettail(مقصد)) != 0)
	    use_tmp_file = TRUE;
	ورنہ
	    واپس 0;
    }

    /*
     * Fail if the "from" file doesn't exist.  Avoids that "to" is deleted.
     */
    اگر (mch_stat((حرف *)from, &st) < 0)
	واپس -1;

#اگر_تعریف_شدہ UNIX
    {
	stat_T	st_مقصد;

	// It's possible for the source and destination to be the same file.
	// This happens when "from" and "to" differ in case and are on a FAT32
	// filesystem.  In that case go through a temp file name.
	اگر (mch_stat((حرف *)مقصد, &st_مقصد) >= 0
		&& st.st_dev == st_مقصد.st_dev
		&& st.st_ino == st_مقصد.st_ino)
	    use_tmp_file = TRUE;
    }
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
    {
	BY_HANDLE_FILE_INFORMATION info1, info2;

	// It's possible for the source and destination to be the same file.
	// In that case go through a temp file name.  This makes rename("foo",
	// "./foo") a no-op (in a complicated way).
	اگر (win32_fileinfo(from, &info1) == FILEINFO_OK
		&& win32_fileinfo(مقصد, &info2) == FILEINFO_OK
		&& info1.dw_Volume_Serial_نمبر == info2.dw_Volume_Serial_نمبر
		&& info1.n_File_اشاریہ_بلند == info2.n_File_اشاریہ_بلند
		&& info1.n_File_اشاریہ_کم == info2.n_File_اشاریہ_کم)
	    use_tmp_file = TRUE;
    }
#اگر_ختم

    اگر (use_tmp_file)
    {
	حرف	tempname[MAXPATHL + 1];

	/*
	 * Find a name that doesn't exist and is in the same directory.
	 * Rename "from" to "tempname" and then rename "tempname" to "to".
	 */
	اگر (STRLEN(from) >= MAXPATHL - 5)
	    واپس -1;
	STRCPY(tempname, from);
	برائے (n = 123; n < 99999; ++n)
	{
	    sprintf((حرف *)gettail((char_u *)tempname), "%d", n);
	    اگر (mch_stat(tempname, &st) < 0)
	    {
		اگر (mch_rename((حرف *)from, tempname) == 0)
		{
		    اگر (mch_rename(tempname, (حرف *)مقصد) == 0)
			واپس 0;
		    // Strange, the second step failed.  Try moving the
		    // file back and return failure.
		    (خالی)mch_rename(tempname, (حرف *)from);
		    واپس -1;
		}
		// If it fails for one temp name it will most likely fail
		// for any temp name, give up.
		واپس -1;
	    }
	}
	واپس -1;
    }

    /*
     * Delete the "to" file, this is required on some systems to make the
     * mch_rename() work, on other systems it makes sure that we don't have
     * two files when the mch_rename() fails.
     */

#اگر_تعریف_شدہ AMIGA
    /*
     * With MSDOS-compatible filesystems (crossdos, messydos) it is possible
     * that the name of the "to" file is the same as the "from" file, even
     * though the names are different. To avoid the chance of accidentally
     * deleting the "from" file (horror!) we lock it during the remove.
     *
     * When used for making a backup before writing the file: This should not
     * happen with ":w", because startscript() should detect this problem and
     * set buf->b_shortname, causing modname() to return a correct ".bak" file
     * name.  This problem does exist with ":w filename", but then the
     * original file will be somewhere else so the backup isn't really
     * important. If autoscripting is off the rename may fail.
     */
    flock = تالا_2((UBYTE *)from, (طویل)VIM_ACCESS_READ);
#اگر_ختم
    mch_remove(مقصد);
#اگر_تعریف_شدہ AMIGA
    اگر (flock)
	Un_تالا(flock);
#اگر_ختم

    /*
     * First try a normal rename, return if it works.
     */
    اگر (mch_rename((حرف *)from, (حرف *)مقصد) == 0)
	واپس 0;

    /*
     * Rename() failed, try copying the file.
     */
    ret = vim_copyfile(from, مقصد);
    اگر (ret != OK)
	واپس -1;

    /*
     * Remove copied original file
     */
    اگر (mch_stat((حرف *)from, &st) >= 0)
	mch_remove(from);

    واپس 0;
}


/*
 * Create the new file with same permissions as the original.
 * Return FAIL for failure, OK for success.
 */
    صحیح
vim_copyfile(char_u *from, char_u *مقصد)
{
    صحیح		fd_اندر;
    صحیح		fd_برآمد;
    صحیح		n;
    حرف	*errmsg = تہی;
    حرف	*بفر;
    طویل	perm;
#اگر_تعریف_شدہ HAVE_ACL
    vim_acl_T	acl;		// ACL from original file
#اگر_ختم

#اگر_تعریف_شدہ HAVE_READLINK
    صحیح		ret;
    صحیح		len;
    stat_T	st;
    حرف	linkbuf[MAXPATHL + 1];

    ret = mch_lstat((حرف *)from, &st);
    اگر (ret >= 0 && S_ISLNK(st.st_منوال))
    {
	ret = -1;

	len = readlink((حرف *)from, linkbuf, MAXPATHL);
	اگر (len > 0)
	{
	    linkbuf[len] = NUL;

	    // Create link
	    ret = symlink(linkbuf, (حرف *)مقصد);
	}

	واپس ret == 0 ? OK : FAIL;
    }
#اگر_ختم

    perm = mch_getperm(from);
#اگر_تعریف_شدہ HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    acl = mch_get_acl(from);
#اگر_ختم
    fd_اندر = mch_open((حرف *)from, O_RDONLY|O_EXTRA, 0);
    اگر (fd_اندر == -1)
    {
#اگر_تعریف_شدہ HAVE_ACL
	mch_free_acl(acl);
#اگر_ختم
	واپس FAIL;
    }

    // Create the new file with same permissions as the original.
    fd_برآمد = mch_open((حرف *)مقصد,
		       O_CREAT|O_EXCL|O_WRONLY|O_EXTRA|O_NOFOLLOW, (صحیح)perm);
    اگر (fd_برآمد == -1)
    {
	close(fd_اندر);
#اگر_تعریف_شدہ HAVE_ACL
	mch_free_acl(acl);
#اگر_ختم
	واپس FAIL;
    }

    بفر = alloc(WRITEBUFSIZE);
    اگر (بفر == تہی)
    {
	close(fd_برآمد);
	close(fd_اندر);
#اگر_تعریف_شدہ HAVE_ACL
	mch_free_acl(acl);
#اگر_ختم
	واپس FAIL;
    }

    جب ((n = read_eintr(fd_اندر, بفر, WRITEBUFSIZE)) > 0)
	اگر (write_eintr(fd_برآمد, بفر, n) != n)
	{
	    errmsg = _(e_error_writing_مقصد_str);
	    توڑو;
	}

    vim_free(بفر);
    close(fd_اندر);
    اگر (close(fd_برآمد) < 0)
	errmsg = _(e_error_closing_str);
    اگر (n < 0)
    {
	errmsg = _(e_error_reading_str);
	مقصد = from;
    }
#اگر_غیر_تعریف_شدہ UNIX	    // for Unix mch_open() already set the permission
    mch_setperm(مقصد, perm);
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ACL
    mch_سیٹ_acl(مقصد, acl);
    mch_free_acl(acl);
#اگر_ختم
#اگر defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    mch_copy_sec(from, مقصد);
#اگر_ختم
    اگر (errmsg != تہی)
    {
	semsg(errmsg, مقصد);
	واپس FAIL;
    }
    واپس OK;
}

ساکن صحیح already_warned = FALSE;

/*
 * Check if any not hidden buffer has been changed.
 * Postpone the check if there are characters in the stuff buffer, a global
 * command is being executed, a mapping is being executed or an autocommand is
 * busy.
 * Returns TRUE if some message was written (screen should be redrawn and
 * cursor positioned).
 */
    صحیح
check_timestamps(
    صحیح		focus)		// called for GUI focus event
{
    buf_T	*buf;
    صحیح		didit = 0;
    صحیح		n;

    // Don't check timestamps while system() or another low-level function may
    // cause us to lose and gain focus.
    اگر (no_check_timestamps > 0)
	واپس FALSE;

    // Avoid doing a check twice.  The OK/Reload dialog can cause a focus
    // event and we would keep on checking if the file is steadily growing.
    // Do check again after typing something.
    اگر (focus && did_check_timestamps)
    {
	need_check_timestamps = TRUE;
	واپس FALSE;
    }

    اگر (!stuff_خالی_خانے() || global_busy || !typebuf_typed()
			|| autocmd_busy || curbuf_تالا > 0 || allbuf_تالا > 0)
	need_check_timestamps = TRUE;		// check later
    ورنہ
    {
	++no_wait_return;
	did_check_timestamps = TRUE;
	already_warned = FALSE;
	FOR_ALL_BUFFERS(buf)
	{
	    // Only check buffers in a window.
	    اگر (buf->b_nwindows > 0)
	    {
		bufref_T bufref;

		سیٹ_bufref(&bufref, buf);
		n = buf_check_timestamp(buf, focus);
		اگر (didit < n)
		    didit = n;
		اگر (n > 0 && !bufref_valid(&bufref))
		{
		    // Autocommands have removed the buffer, start at the
		    // first one again.
		    buf = firstbuf;
		    جاری;
		}
	    }
	}
	--no_wait_return;
	need_check_timestamps = FALSE;
	اگر (need_wait_return && didit == 2)
	{
	    // make sure msg isn't overwritten
	    پیغام_puts("\n");
	    برآمد_flush();
	}
    }
    واپس didit;
}

/*
 * Move all the lines from buffer "frombuf" to buffer "tobuf".
 * Return OK or FAIL.  When FAIL "tobuf" is incomplete and/or "frombuf" is not
 * empty.
 */
    ساکن صحیح
move_سطریں(buf_T *frombuf, buf_T *tobuf)
{
    buf_T	*tbuf = curbuf;
    صحیح		retval = OK;
    linenr_T	lnum;
    char_u	*p;

    // Copy the lines in "frombuf" to "tobuf".
    curbuf = tobuf;
    برائے (lnum = 1; lnum <= frombuf->b_ml.ml_سطر_گنتی; ++lnum)
    {
	p = vim_strnsave(ml_get_buf(frombuf, lnum, FALSE), ml_get_buf_len(frombuf, lnum));
	اگر (p == تہی || ml_append(lnum - 1, p, 0, FALSE) == FAIL)
	{
	    vim_free(p);
	    retval = FAIL;
	    توڑو;
	}
	vim_free(p);
    }

    // Delete all the lines in "frombuf".
    اگر (retval != FAIL)
    {
	curbuf = frombuf;
	برائے (lnum = curbuf->b_ml.ml_سطر_گنتی; lnum > 0; --lnum)
	    اگر (ml_delete(lnum) == FAIL)
	    {
		// Oops!  We could try putting back the saved lines, but that
		// might fail again...
		retval = FAIL;
		توڑو;
	    }
    }

    curbuf = tbuf;
    واپس retval;
}

/*
 * Check if buffer "buf" has been changed.
 * Also check if the file for a new buffer unexpectedly appeared.
 * return 1 if a changed buffer was found.
 * return 2 if a message has been displayed.
 * return 0 otherwise.
 */
    صحیح
buf_check_timestamp(
    buf_T	*buf,
    صحیح		focus غیر_مستعمل)	// called for GUI focus event
{
    stat_T	st;
    صحیح		stat_res;
    صحیح		retval = 0;
    حرف	*mesg = تہی;
    حرف	*mesg2 = "";
    صحیح		helpmesg = FALSE;
    شمار {
	RELOAD_NONE,
	RELOAD_معمول,
	RELOAD_DETECT
    }		reload = RELOAD_NONE;
#اگر defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
    صحیح		can_reload = FALSE;
#اگر_ختم
    off_T	orig_سائز = buf->b_orig_سائز;
    صحیح		orig_منوال = buf->b_orig_منوال;
#اگر_تعریف_شدہ FEAT_GUI
    صحیح		save_mouse_correct = need_mouse_correct;
#اگر_ختم
    ساکن صحیح	busy = FALSE;
    صحیح		n;
    bufref_T	bufref;

    سیٹ_bufref(&bufref, buf);

    // If there is no file name, the buffer is not loaded, 'buftype' is
    // set, we are in the middle of a save or being called recursively: ignore
    // this buffer.
    اگر (buf->b_ffname == تہی
	    || buf->b_ml.ml_mfp == تہی
	    || !bt_معمول(buf)
	    || buf->b_saving
	    || busy
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	    || is_Netbeans_بفر(buf)
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
	    || buf->b_رکن != تہی
#اگر_ختم
	    )
	واپس 0;

    اگر (       !(buf->b_flags & BF_NOTEDITED)
	    && buf->b_mtime != 0
	    && ((stat_res = mch_stat((حرف *)buf->b_ffname, &st)) < 0
		|| وقت_differs(&st, buf->b_mtime, buf->b_mtime_ns)
		|| st.st_size != buf->b_orig_سائز
#اگر_تعریف_شدہ HAVE_ST_منوال
		|| (صحیح)st.st_منوال != buf->b_orig_منوال
#ورنہ
		|| mch_getperm(buf->b_ffname) != buf->b_orig_منوال
#اگر_ختم
		))
    {
	طویل پچھلا_b_mtime = buf->b_mtime;

	retval = 1;

	// set b_mtime to stop further warnings (e.g., when executing
	// FileChangedShell autocmd)
	اگر (stat_res < 0)
	{
	    // Check the file again later to see if it re-appears.
	    buf->b_mtime = -1;
	    buf->b_orig_سائز = 0;
	    buf->b_orig_منوال = 0;
	}
	ورنہ
	    buf_store_وقت(buf, &st, buf->b_ffname);

	// Don't do anything for a directory.  Might contain the file
	// explorer.
	اگر (mch_isdir(buf->b_fname))
	    ;

	/*
	 * If 'autoread' is set, the buffer has no changes and the file still
	 * exists, reload the buffer.  Use the buffer-local option value if it
	 * was set, the global option value otherwise.
	 */
	ورنہ اگر ((buf->b_p_ar >= 0 ? buf->b_p_ar : p_ar)
				       && !buf_Is_بدلا(buf) && stat_res >= 0)
	    reload = RELOAD_معمول;
	ورنہ
	{
	    حرف    *reason;
#اگر_تعریف_شدہ FEAT_EVAL
	    size_t  reasonlen;
#اگر_ختم

	    اگر (stat_res < 0)
	    {
		reason = "deleted";
#اگر_تعریف_شدہ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("deleted");
#اگر_ختم
	    }
	    ورنہ اگر (buf_Is_بدلا(buf))
	    {
		reason = "conflict";
#اگر_تعریف_شدہ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("conflict");
#اگر_ختم
	    }
	    /*
	     * Check if the file contents really changed to avoid giving a
	     * warning when only the timestamp was set (e.g., checked out of
	     * CVS).  Always warn when the buffer was changed.
	     */
	    ورنہ اگر (orig_سائز != buf->b_orig_سائز || buf_contents_بدلا(buf))
	    {
		reason = "changed";
#اگر_تعریف_شدہ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("changed");
#اگر_ختم
	    }
	    ورنہ اگر (orig_منوال != buf->b_orig_منوال)
	    {
		reason = "mode";
#اگر_تعریف_شدہ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("mode");
#اگر_ختم
	    }
	    ورنہ
	    {
		reason = "time";
#اگر_تعریف_شدہ FEAT_EVAL
		reasonlen = STRLEN_LITERAL("time");
#اگر_ختم
	    }

	    /*
	     * Only give the warning if there are no FileChangedShell
	     * autocommands.
	     * Avoid being called recursively by setting "busy".
	     */
	    busy = TRUE;
#اگر_تعریف_شدہ FEAT_EVAL
	    سیٹ_vim_var_string(VV_FCS_REASON, (char_u *)reason, (صحیح)reasonlen);
	    سیٹ_vim_var_string(VV_FCS_CHOICE, (char_u *)"", 0);
#اگر_ختم
	    ++allbuf_تالا;
	    n = apply_autocmds(EVENT_FILECHANGEDSHELL,
				      buf->b_fname, buf->b_fname, FALSE, buf);
	    --allbuf_تالا;
	    busy = FALSE;
	    اگر (n)
	    {
		اگر (!bufref_valid(&bufref))
		    emsg(_(e_filechangedshell_autocommand_deleted_بفر));
#اگر_تعریف_شدہ FEAT_EVAL
		char_u	*s = get_vim_var_str(VV_FCS_CHOICE);
		اگر (STRCMP(s, "reload") == 0 && *reason != 'd')
		    reload = RELOAD_معمول;
		ورنہ اگر (STRCMP(s, "edit") == 0)
		    reload = RELOAD_DETECT;
		ورنہ اگر (STRCMP(s, "ask") == 0)
		    n = FALSE;
		ورنہ
#اگر_ختم
		    واپس 2;
	    }
	    اگر (!n)
	    {
		اگر (*reason == 'd')
		{
		    // Only give the message once.
		    اگر (پچھلا_b_mtime != -1)
			mesg = _(e_file_str_no_longer_دستیاب);
		}
		ورنہ
		{
		    helpmesg = TRUE;
#اگر defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		    can_reload = TRUE;
#اگر_ختم
		    اگر (reason[2] == 'n')
		    {
			mesg = _("W12: Warning: File \"%s\" has changed and the buffer was changed in Vim as well");
			mesg2 = _("See \":help W12\" for more info.");
		    }
		    ورنہ اگر (reason[1] == 'h')
		    {
			mesg = _("W11: Warning: File \"%s\" has changed since editing started");
			mesg2 = _("See \":help W11\" for more info.");
		    }
		    ورنہ اگر (*reason == 'm')
		    {
			mesg = _("W16: Warning: Mode of file \"%s\" has changed since editing started");
			mesg2 = _("See \":help W16\" for more info.");
		    }
		    ورنہ
		    {
			// Only timestamp changed, store it to avoid a warning
			// in check_mtime() later.
			buf->b_mtime_read = buf->b_mtime;
			buf->b_mtime_read_ns = buf->b_mtime_ns;
		    }
		}
	    }
	}

    }
    ورنہ اگر ((buf->b_flags & BF_NEW) && !(buf->b_flags & BF_NEW_W)
						&& vim_fexists(buf->b_ffname))
    {
	retval = 1;
	mesg = _("W13: Warning: File \"%s\" has been created after editing started");
	buf->b_flags |= BF_NEW_W;
#اگر defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	can_reload = TRUE;
#اگر_ختم
    }

    اگر (mesg != تہی)
    {
	char_u *راستہ;

	راستہ = home_replace_save(buf, buf->b_fname);
	اگر (راستہ != تہی)
	{
	    size_t  tbufsize;
	    حرف    *tbuf;

	    اگر (!helpmesg)
		mesg2 = "";
	    tbufsize = STRLEN(mesg) + STRLEN(راستہ) + 2 + STRLEN(mesg2) + 1; // +2 for either '\n' or "; "
									    // and +1 for NUL
	    tbuf = alloc(tbufsize);
	    اگر (tbuf != تہی)
	    {
		صحیح tbuflen;

		tbuflen = vim_snprintf(tbuf, tbufsize, mesg, راستہ);
#اگر_تعریف_شدہ FEAT_EVAL
		// Set warningmsg here, before the unimportant and output-specific
		// mesg2 has been appended.
		سیٹ_vim_var_string(VV_WARNINGMSG, (char_u *)tbuf, tbuflen);
#اگر_ختم
#اگر defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		اگر (can_reload)
		{
		    اگر (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "\n%s", mesg2);
		    انتخاب (do_dialog(VIM_WARNING, (char_u *)_("Warning"),
			    (char_u *)tbuf,
			    (char_u *)_("&OK\n&Load File\nLoad File &and Options"),
			    1, تہی, TRUE))
		    {
			حالت 2:
			    reload = RELOAD_معمول;
			    توڑو;
			حالت 3:
			    reload = RELOAD_DETECT;
			    توڑو;
		    }
		}
		ورنہ
#اگر_ختم
		اگر (State > منوال_معمول_BUSY || (State & منوال_CMDLINE)
								 || already_warned)
		{
		    اگر (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "; %s", mesg2);
		    emsg(tbuf);
		    retval = 2;
		}
		ورنہ
		{
		    اگر (!autocmd_busy)
		    {
			پیغام_آغاز();
			پیغام_puts_attr(tbuf, HL_ATTR(HLF_E) + پیغام_HIST);
			اگر (*mesg2 != NUL)
			    پیغام_puts_attr(mesg2, HL_ATTR(HLF_W) + پیغام_HIST);
			پیغام_clr_eos();
			(خالی)پیغام_انتہا();
			اگر (emsg_silent == 0 && !اندر_assert_fails)
			{
			    برآمد_flush();
#اگر_تعریف_شدہ FEAT_GUI
			    اگر (!focus)
#اگر_ختم
				// give the user some time to think about it
				ui_delay(1004L, TRUE);

			    // don't redraw and erase the message
			    redraw_cmdline = FALSE;
			}
		    }
		    already_warned = TRUE;
		}
	    }

	    vim_free(tbuf);
	    vim_free(راستہ);
	}
    }

    اگر (reload != RELOAD_NONE)
    {
	// Reload the buffer.
	buf_reload(buf, orig_منوال, reload == RELOAD_DETECT);
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
	اگر (bufref_valid(&bufref) && buf->b_p_udf && buf->b_ffname != تہی)
	{
	    char_u	    hash[UNDO_HASH_سائز];
	    buf_T	    *save_curbuf = curbuf;

	    // Any existing undo file is unusable, write it now.
	    curbuf = buf;
	    u_compute_hash(hash);
	    u_write_undo(تہی, FALSE, buf, hash);
	    curbuf = save_curbuf;
	}
#اگر_ختم
    }

    // Trigger FileChangedShell when the file was changed in any way.
    اگر (bufref_valid(&bufref) && retval != 0)
	(خالی)apply_autocmds(EVENT_FILECHANGEDSHELLPOST,
				      buf->b_fname, buf->b_fname, FALSE, buf);
#اگر_تعریف_شدہ FEAT_GUI
    // restore this in case an autocommand has set it; it would break
    // 'mousefocus'
    need_mouse_correct = save_mouse_correct;
#اگر_ختم

    واپس retval;
}

/*
 * Reload a buffer that is already loaded.
 * Used when the file was changed outside of Vim.
 * "orig_mode" is buf->b_orig_mode before the need for reloading was detected.
 * buf->b_orig_mode may have been reset already.
 */
    خالی
buf_reload(buf_T *buf, صحیح orig_منوال, صحیح reload_options)
{
    exarg_T	ea;
    pos_T	old_cursor;
    linenr_T	old_topline;
    صحیح		old_ro = buf->b_p_ro;
    buf_T	*savebuf;
    bufref_T	bufref;
    صحیح		saved = OK;
    aco_save_T	aco;
    صحیح		flags = READ_NEW;
    صحیح		prepped = OK;

    // Set curwin/curbuf for "buf" and save some things.
    aucmd_prepbuf(&aco, buf);
    اگر (curbuf != buf)
    {
	// Failed to find a window for "buf", it is dangerous to continue,
	// better bail out.
	واپس;
    }

    // Unless reload_options is set, we only want to read the text from the
    // file, not reset the syntax highlighting, clear marks, diff status, etc.
    // Force the fileformat and encoding to be the same.
    اگر (reload_options)
	CLEAR_میدان(ea);
    ورنہ
	prepped = prep_exarg(&ea, buf);

    اگر (prepped == OK)
    {
	old_cursor = curwin->w_cursor;
	old_topline = curwin->w_topline;

	اگر (p_ur < 0 || curbuf->b_ml.ml_سطر_گنتی <= p_ur)
	{
	    // Save all the text, so that the reload can be undone.
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    saved = u_savecommon(0, curbuf->b_ml.ml_سطر_گنتی + 1, 0, TRUE);
	    flags |= READ_KEEP_UNDO;
	}

	/*
	 * To behave like when a new file is edited (matters for
	 * BufReadPost autocommands) we first need to delete the current
	 * buffer contents.  But if reading the file fails we should keep
	 * the old contents.  Can't use memory only, the file might be
	 * too big.  Use a hidden buffer to move the buffer contents to.
	 */
	اگر (BUFEMPTY() || saved == FAIL)
	    savebuf = تہی;
	ورنہ
	{
	    // Allocate a buffer without putting it in the buffer list.
	    savebuf = buflist_new(تہی, تہی, (linenr_T)1, BLN_DUMMY);
	    سیٹ_bufref(&bufref, savebuf);
	    اگر (savebuf != تہی && buf == curbuf)
	    {
		// Open the memline.
		curbuf = savebuf;
		curwin->w_بفر = savebuf;
		saved = ml_open(curbuf);
		curbuf = buf;
		curwin->w_بفر = buf;
	    }
	    اگر (savebuf == تہی || saved == FAIL || buf != curbuf
				      || move_سطریں(buf, savebuf) == FAIL)
	    {
		semsg(_(e_could_not_prepare_for_reloading_str), buf->b_fname);
		saved = FAIL;
	    }
	}

	اگر (saved == OK)
	{
	    صحیح old_پیغام_silent = پیغام_silent;

	    curbuf->b_flags |= BF_CHECK_RO;	// check for RO again
	    curbuf->b_keep_filetype = true;	// don't detect 'filetype'

	    اگر (shortmess(SHM_FILEINFO))
		پیغام_silent = 1;

	    اگر (readfile(buf->b_ffname, buf->b_fname, (linenr_T)0,
			(linenr_T)0,
			(linenr_T)MAXLNUM, &ea, flags) != OK)
	    {
#اگر defined(FEAT_EVAL)
		اگر (!aborting())
#اگر_ختم
		    semsg(_(e_could_not_reload_str), buf->b_fname);
		اگر (savebuf != تہی && bufref_valid(&bufref) && buf == curbuf)
		{
		    // Put the text back from the save buffer.  First
		    // delete any lines that readfile() added.
		    جب (!BUFEMPTY())
			اگر (ml_delete(buf->b_ml.ml_سطر_گنتی) == FAIL)
			    توڑو;
		    (خالی)move_سطریں(savebuf, buf);
		}
	    }
	    ورنہ اگر (buf == curbuf)  // "buf" still valid
	    {
		// Mark the buffer as unmodified and free undo info.
		unchanged(buf, TRUE, TRUE);
		اگر ((flags & READ_KEEP_UNDO) == 0)
		    u_clearallandblockfree(buf);
		ورنہ
		{
		    // Mark all undo states as changed.
		    u_unchanged(curbuf);
		}
	    }

	    پیغام_silent = old_پیغام_silent;
	}
	vim_free(ea.cmd);

	اگر (savebuf != تہی && bufref_valid(&bufref))
	    wipe_بفر(savebuf, FALSE);

#اگر_تعریف_شدہ FEAT_DIFF
	// Invalidate diff info if necessary.
	diff_invalidate(curbuf);
#اگر_ختم

	// Restore the topline and cursor position and check it (lines may
	// have been removed).
	اگر (old_topline > curbuf->b_ml.ml_سطر_گنتی)
	    curwin->w_topline = curbuf->b_ml.ml_سطر_گنتی;
	ورنہ
	    curwin->w_topline = old_topline;
	curwin->w_cursor = old_cursor;
	check_cursor();
	update_topline();
	curbuf->b_keep_filetype = false;
#اگر_تعریف_شدہ FEAT_FOLDING
	{
	    win_T	*wp;
	    tabpage_T	*tp;

	    // Update folds unless they are defined manually.
	    FOR_ALL_TAB_WINDOWS(tp, wp)
		اگر (wp->w_بفر == curwin->w_بفر
			&& !foldmethodIsManual(wp))
		    foldUpdateAll(wp);
	}
#اگر_ختم
	// If the mode didn't change and 'readonly' was set, keep the old
	// value; the user probably used the ":view" command.  But don't
	// reset it, might have had a read error.
	اگر (orig_منوال == curbuf->b_orig_منوال)
	    curbuf->b_p_ro |= old_ro;

	// Modelines must override settings done by autocommands.
	do_modelines(0);
    }

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);
    // Careful: autocommands may have made "buf" invalid!
}

    خالی
buf_store_وقت(buf_T *buf, stat_T *st, char_u *fname غیر_مستعمل)
{
    buf->b_mtime = (طویل)st->st_mtime;
#اگر_تعریف_شدہ ST_MTIM_NSEC
    buf->b_mtime_ns = (طویل)st->ST_MTIM_NSEC;
#ورنہ
    buf->b_mtime_ns = 0;
#اگر_ختم
    buf->b_orig_سائز = st->st_size;
#اگر_تعریف_شدہ HAVE_ST_منوال
    buf->b_orig_منوال = (صحیح)st->st_منوال;
#ورنہ
    buf->b_orig_منوال = mch_getperm(fname);
#اگر_ختم
}

/*
 * Adjust the line with missing eol, used for the next write.
 * Used for do_filter(), when the input lines for the filter are deleted.
 */
    خالی
write_lnum_adjust(linenr_T انحراف)
{
    اگر (curbuf->b_no_eol_lnum != 0)	// only if there is a missing eol
	curbuf->b_no_eol_lnum += انحراف;
}

// Subfuncions for readdirex()
#اگر_تعریف_شدہ FEAT_EVAL
# ifdef MSWIN
    ساکن char_u *
getfpermwfd(WIN32_ڈھونڈو_DATAW *wfd, char_u *perm)
{
    stat_T	    st;
    غیرعلامتی مختصر  st_منوال;
    DWORD	    flag = wfd->dw_File_اوصاف;
    WCHAR	    *wp;

    st_منوال = (flag & FILE_ATTRIBUTE_DIRECTORY)
					? (_S_IFDIR | _S_IEXEC) : _S_IFREG;
    st_منوال |= (flag & FILE_ATTRIBUTE_READONLY)
					? _S_IREAD : (_S_IREAD | _S_IWRITE);

    wp = wcsrchr(wfd->cFileName, L'.');
    اگر (wp != تہی)
    {
	اگر (_wcsicmp(wp, L".exe") == 0 ||
		_wcsicmp(wp, L".com") == 0 ||
		_wcsicmp(wp, L".cmd") == 0 ||
		_wcsicmp(wp, L".bat") == 0)
	    st_منوال |= _S_IEXEC;
    }

    // Copy user bits to group/other.
    st_منوال |= (st_منوال & 0700) >> 3;
    st_منوال |= (st_منوال & 0700) >> 6;

    st.st_منوال = st_منوال;
    واپس getfpermst(&st, perm);
}

    ساکن خالی
getftypewfd(WIN32_ڈھونڈو_DATAW *wfd, string_T *ret)
{
    DWORD flag = wfd->dw_File_اوصاف;
    DWORD لیبل = wfd->dwReserved0;

    اگر (flag & FILE_ATTRIBUTE_REPARSE_نقطہ)
    {
	اگر (لیبل == IO_REPARSE_لیبل_MOUNT_نقطہ)
	{
	    ret->string = (char_u *)"junction";
	    ret->لمبائی = STRLEN_LITERAL("junction");
	    واپس;
	}
	اگر (لیبل == IO_REPARSE_لیبل_SYMLINK)
	{
	    اگر (flag & FILE_ATTRIBUTE_DIRECTORY)
	    {
		ret->string = (char_u *)"linkd";
		ret->لمبائی = STRLEN_LITERAL("linkd");
		واپس;
	    }

	    ret->string = (char_u *)"link";
	    ret->لمبائی = STRLEN_LITERAL("link");
	    واپس;
	}

	// unknown reparse point type
	ret->string = (char_u *)"reparse";
	ret->لمبائی = STRLEN_LITERAL("reparse");
	واپس;
    }

    اگر (flag & FILE_ATTRIBUTE_DIRECTORY)
    {
	ret->string = (char_u *)"dir";
	ret->لمبائی = STRLEN_LITERAL("dir");
	واپس;
    }

    ret->string = (char_u *)"file";
    ret->لمبائی = STRLEN_LITERAL("file");
}

    ساکن dict_T *
create_readdirex_شے(WIN32_ڈھونڈو_DATAW *wfd)
{
    dict_T	*شے;
    char_u	*p;
    varnumber_T	سائز, وقت;
    char_u	permbuf[] = "---------";
    string_T	s;

    شے = dict_alloc();
    اگر (شے == تہی)
	واپس تہی;
    شے->dv_refcount++;

    p = utf16_مقصد_enc(wfd->cFileName, تہی);
    اگر (p == تہی)
	جاؤ theend;
    اگر (dict_جوڑو_string(شے, "name", p) == FAIL)
    {
	vim_free(p);
	جاؤ theend;
    }
    vim_free(p);

    سائز = (((varnumber_T)wfd->n_File_سائز_بلند) << 32) | wfd->n_File_سائز_کم;
    اگر (dict_جوڑو_نمبر(شے, "size", سائز) == FAIL)
	جاؤ theend;

    // Convert FILETIME to unix time.
    وقت = (((((varnumber_T)wfd->ft_آخری_Write_وقت.dw_بلند_Date_وقت) << 32) |
		wfd->ft_آخری_Write_وقت.dw_کم_Date_وقت)
	    - 116444736000000000) / 10000000;
    اگر (dict_جوڑو_نمبر(شے, "time", وقت) == FAIL)
	جاؤ theend;

    getftypewfd(wfd, &s);
    اگر (dict_جوڑو_string_len(شے, "type", s.string, (صحیح)s.لمبائی) == FAIL)
	جاؤ theend;
    اگر (dict_جوڑو_string(شے, "perm", getfpermwfd(wfd, permbuf)) == FAIL)
	جاؤ theend;

    اگر (dict_جوڑو_string_len(شے, "user", (char_u *)"", 0) == FAIL)
	جاؤ theend;
    اگر (dict_جوڑو_string_len(شے, "group", (char_u *)"", 0) == FAIL)
	جاؤ theend;

    واپس شے;

theend:
    dict_unref(شے);
    واپس تہی;
}
# ورنہ
    ساکن dict_T *
create_readdirex_شے(char_u *راستہ, char_u *name)
{
    dict_T	*شے;
    حرف	*p;
    size_t	pathlen, len;
    size_t	namelen;
    stat_T	st;
    صحیح		ret, link = FALSE;
    varnumber_T	سائز;
    char_u	permbuf[] = "---------";
    string_T	q = {تہی, 0};
    ساخت passwd *pw;
    ساخت گروہ  *gr;

    شے = dict_alloc();
    اگر (شے == تہی)
	واپس تہی;
    شے->dv_refcount++;

    pathlen = STRLEN(راستہ);
    namelen = STRLEN(name);
    len = pathlen + 1 + namelen + 1;
    p = alloc(len);
    اگر (p == تہی)
	جاؤ theend;
    اگر (pathlen > 0 && راستہ[pathlen - 1] == '/')
	vim_snprintf(p, len, "%s%s", راستہ, name);
    ورنہ
	vim_snprintf(p, len, "%s/%s", راستہ, name);
    ret = mch_lstat(p, &st);
    اگر (ret >= 0 && S_ISLNK(st.st_منوال))
    {
	link = TRUE;
	ret = mch_stat(p, &st);
	اگر (ret < 0)
	    STR_LITERAL_سیٹ(q, "link");
    }
    vim_free(p);

    اگر (dict_جوڑو_string_len(شے, "name", name, (صحیح)namelen) == FAIL)
	جاؤ theend;

    اگر (ret >= 0)
    {
	سائز = (varnumber_T)st.st_size;
	اگر (S_ISDIR(st.st_منوال))
	    سائز = 0;
	// non-perfect check for overflow
	ورنہ اگر ((off_T)سائز != (off_T)st.st_size)
	    سائز = -2;
	اگر (dict_جوڑو_نمبر(شے, "size", سائز) == FAIL)
	    جاؤ theend;
	اگر (dict_جوڑو_نمبر(شے, "time", (varnumber_T)st.st_mtime) == FAIL)
	    جاؤ theend;

	اگر (link)
	{
	    اگر (S_ISDIR(st.st_منوال))
		STR_LITERAL_سیٹ(q, "linkd");
	    ورنہ
		STR_LITERAL_سیٹ(q, "link");
	}
	ورنہ
	{
	    q.string = getftypest(&st);
	    q.لمبائی = STRLEN(q.string);
	}
	اگر (dict_جوڑو_string_len(شے, "type", q.string, (صحیح)q.لمبائی) == FAIL)
	    جاؤ theend;
	اگر (dict_جوڑو_string(شے, "perm", getfpermst(&st, permbuf)) == FAIL)
	    جاؤ theend;

	pw = getpwuid(st.st_uid);
	اگر (pw == تہی)
	    STR_LITERAL_سیٹ(q, "");
	ورنہ
	{
	    q.string = (char_u *)pw->pw_name;
	    q.لمبائی = STRLEN(q.string);
	}
	اگر (dict_جوڑو_string_len(شے, "user", q.string, (صحیح)q.لمبائی) == FAIL)
	    جاؤ theend;
#  اگر !defined(VMS) || (defined(VMS) && defined(HAVE_XOS_R_H))
	gr = getgrgid(st.st_gid);
	اگر (gr == تہی)
	    STR_LITERAL_سیٹ(q, "");
	ورنہ
	{
	    q.string = (char_u *)gr->gr_name;
	    q.لمبائی = STRLEN(q.string);
	}
#  endif
	اگر (dict_جوڑو_string_len(شے, "group", q.string, (صحیح)q.لمبائی) == FAIL)
	    جاؤ theend;
    }
    ورنہ
    {
	اگر (dict_جوڑو_نمبر(شے, "size", -1) == FAIL)
	    جاؤ theend;
	اگر (dict_جوڑو_نمبر(شے, "time", -1) == FAIL)
	    جاؤ theend;
	اگر (q.string == تہی)
	{
	    اگر (dict_جوڑو_string_len(شے, "type", (char_u *)"", 0) == FAIL)
		جاؤ theend;
	}
	ورنہ
	{
	    اگر (dict_جوڑو_string_len(شے, "type", q.string, (صحیح)q.لمبائی) == FAIL)
		جاؤ theend;
	}
	اگر (dict_جوڑو_string_len(شے, "perm", (char_u *)"", 0) == FAIL)
	    جاؤ theend;
	اگر (dict_جوڑو_string_len(شے, "user", (char_u *)"", 0) == FAIL)
	    جاؤ theend;
	اگر (dict_جوڑو_string_len(شے, "group", (char_u *)"", 0) == FAIL)
	    جاؤ theend;
    }
    واپس شے;

theend:
    dict_unref(شے);
    واپس تہی;
}
# endif

    ساکن صحیح
compare_readdirex_شے(ثابت خالی *p1, ثابت خالی *p2)
{
    char_u  *name1, *name2;

    name1 = dict_get_string(*(dict_T**)p1, "name", FALSE);
    name2 = dict_get_string(*(dict_T**)p2, "name", FALSE);
    اگر (readdirex_ترتیب_دو == READDIR_ترتیب_دو_BYTE)
	واپس STRCMP(name1, name2);
    اگر (readdirex_ترتیب_دو == READDIR_ترتیب_دو_IC)
	واپس STRICMP(name1, name2);

    واپس STRCOLL(name1, name2);
}

    ساکن صحیح
compare_readdir_شے(ثابت خالی *s1, ثابت خالی *s2)
{
    اگر (readdirex_ترتیب_دو == READDIR_ترتیب_دو_BYTE)
	واپس STRCMP(*(حرف **)s1, *(حرف **)s2);
    اگر (readdirex_ترتیب_دو == READDIR_ترتیب_دو_IC)
	واپس STRICMP(*(حرف **)s1, *(حرف **)s2);

    واپس STRCOLL(*(حرف **)s1, *(حرف **)s2);
}
#اگر_ختم

#اگر defined(TEMPDIRNAMES) || defined(FEAT_EVAL)
/*
 * Core part of "readdir()" and "readdirex()" function.
 * Retrieve the list of files/directories of "path" into "gap".
 * If "withattr" is TRUE, retrieve the names and their attributes.
 * If "withattr" is FALSE, retrieve the names only.
 * Return OK for success, FAIL for failure.
 */
    صحیح
readdir_core(
    garray_T	*gap,
    char_u	*راستہ,
    صحیح		withattr غیر_مستعمل,
    خالی	*context,
    صحیح		(*checkitem)(خالی *context, خالی *شے),
    صحیح		ترتیب_دو)
{
    صحیح			failed = FALSE;
    char_u		*p;
# ifdef MSWIN
    char_u		*buf;
    صحیح			ok;
    HANDLE		h_ڈھونڈو = INVALID_HANDLE_قدر;
    WIN32_ڈھونڈو_DATAW    wfd;
    WCHAR		*wn = تہی;	// UTF-16 name, NULL when not used.
    char_u		*p_انتہا;
# ورنہ
    DIR			*dirp;
    ساخت dirent	*dp;
# endif

    ga_init2(gap, حجم(خالی *), 20);

# ifdef FEAT_EVAL
#  define FREE_شے(شے)   کرو { \
	اگر (withattr) \
	    dict_unref((dict_T*)(شے)); \
	ورنہ \
	    vim_free(شے); \
    } جب (0)

    readdirex_ترتیب_دو = READDIR_ترتیب_دو_BYTE;
# ورنہ
#  define FREE_شے(شے)   vim_free(شے)
# endif

# ifdef MSWIN
    buf = alloc(MAXPATHL);
    اگر (buf == تہی)
	واپس FAIL;
    STRNCPY(buf, راستہ, MAXPATHL-5);
    p = p_انتہا = buf + STRLEN(buf);
    MB_PTR_عقب(buf, p);
    اگر (*p == '\\' || *p == '/')
	p_انتہا = p;
    STRCPY(p_انتہا, "\\*");

    wn = enc_مقصد_utf16(buf, تہی);
    اگر (wn != تہی)
	h_ڈھونڈو = ڈھونڈو_پہلا_File_W(wn, &wfd);
    ok = (h_ڈھونڈو != INVALID_HANDLE_قدر);
    اگر (!ok)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), راستہ);
    }
    ورنہ
    {
	جب (ok)
	{
	    صحیح	    ignore;
	    خالی    *شے;
	    WCHAR   *wp;

	    wp = wfd.cFileName;
	    ignore = wp[0] == L'.' &&
		    (wp[1] == NUL ||
		     (wp[1] == L'.' && wp[2] == NUL));
	    اگر (ignore)
	    {
		ok = ڈھونڈو_اگلا_File_W(h_ڈھونڈو, &wfd);
		جاری;
	    }
#  ifdef FEAT_EVAL
	    اگر (withattr)
		شے = (خالی*)create_readdirex_شے(&wfd);
	    ورنہ
#  endif
		شے = (خالی*)utf16_مقصد_enc(wfd.cFileName, تہی);
	    اگر (شے == تہی)
	    {
		failed = TRUE;
		توڑو;
	    }

	    اگر (!ignore && checkitem != تہی)
	    {
		صحیح r = checkitem(context, شے);

		اگر (r < 0)
		{
		    FREE_شے(شے);
		    توڑو;
		}
		اگر (r == 0)
		    ignore = TRUE;
	    }

	    اگر (!ignore)
	    {
		اگر (ga_grow(gap, 1) == OK)
		    ((خالی**)gap->ga_ڈیٹا)[gap->ga_len++] = شے;
		ورنہ
		{
		    failed = TRUE;
		    FREE_شے(شے);
		    توڑو;
		}
	    }
	    ورنہ
		FREE_شے(شے);

	    ok = ڈھونڈو_اگلا_File_W(h_ڈھونڈو, &wfd);
	}
	ڈھونڈو_Close(h_ڈھونڈو);
    }

    vim_free(buf);
    vim_free(wn);
# ورنہ	// MSWIN
    dirp = opendir((حرف *)راستہ);
    اگر (dirp == تہی)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), راستہ);
    }
    ورنہ
    {
	برائے (;;)
	{
	    صحیح	    ignore;
	    خالی    *شے;

	    dp = readdir(dirp);
	    اگر (dp == تہی)
		توڑو;
	    p = (char_u *)dp->d_name;

	    ignore = p[0] == '.' &&
		    (p[1] == NUL ||
		     (p[1] == '.' && p[2] == NUL));
	    اگر (ignore)
		جاری;
#  ifdef FEAT_EVAL
	    اگر (withattr)
		شے = (خالی*)create_readdirex_شے(راستہ, p);
	    ورنہ
#  endif
		شے = (خالی*)vim_strsave(p);
	    اگر (شے == تہی)
	    {
		failed = TRUE;
		توڑو;
	    }

	    اگر (checkitem != تہی)
	    {
		صحیح r = checkitem(context, شے);

		اگر (r < 0)
		{
		    FREE_شے(شے);
		    توڑو;
		}
		اگر (r == 0)
		    ignore = TRUE;
	    }

	    اگر (!ignore)
	    {
		اگر (ga_grow(gap, 1) == OK)
		    ((خالی**)gap->ga_ڈیٹا)[gap->ga_len++] = شے;
		ورنہ
		{
		    failed = TRUE;
		    FREE_شے(شے);
		    توڑو;
		}
	    }
	    ورنہ
		FREE_شے(شے);
	}

	closedir(dirp);
    }
# endif	// MSWIN

# undef FREE_شے

    اگر (!failed && gap->ga_len > 0 && ترتیب_دو > READDIR_ترتیب_دو_NONE)
    {
# ifdef FEAT_EVAL
	readdirex_ترتیب_دو = ترتیب_دو;
	اگر (withattr)
	    qsort((خالی*)gap->ga_ڈیٹا, (size_t)gap->ga_len, حجم(dict_T*),
		    compare_readdirex_شے);
	ورنہ
	    qsort((خالی*)gap->ga_ڈیٹا, (size_t)gap->ga_len, حجم(char_u *),
		    compare_readdir_شے);
# ورنہ
	    ترتیب_دو_لڑیاں((char_u **)gap->ga_ڈیٹا, gap->ga_len);
# endif
    }

    واپس failed ? FAIL : OK;
}

/*
 * Delete "name" and everything in it, recursively.
 * return 0 for success, -1 if some file was not deleted.
 */
    صحیح
delete_recursive(char_u *name)
{
    صحیح نتیجہ = 0;

    // A symbolic link to a directory itself is deleted, not the directory it
    // points to.
    اگر (
# اگر defined(UNIX) || defined(MSWIN)
	 mch_isrealdir(name)
# ورنہ
	 mch_isdir(name)
# endif
	    )
    {
	char_u	    *قوت_نما = vim_strsave(name);
	garray_T    ga;

	اگر (قوت_نما == تہی)
	    واپس -1;
	اگر (readdir_core(&ga, قوت_نما, FALSE, تہی, تہی, READDIR_ترتیب_دو_NONE) == OK)
	{
	    صحیح	len = vim_snprintf((حرف *)NameBuff, MAXPATHL, "%s/", قوت_نما);
	    صحیح	i;

	    برائے (i = 0; i < ga.ga_len; ++i)
	    {
		vim_snprintf((حرف *)NameBuff + len, MAXPATHL - len, "%s", ((char_u **)ga.ga_ڈیٹا)[i]);
		اگر (delete_recursive(NameBuff) != 0)
		    // Remember the failure but continue deleting any further
		    // entries.
		    نتیجہ = -1;
	    }
	    ga_clear_لڑیاں(&ga);
	    اگر (mch_rmdir(قوت_نما) != 0)
		نتیجہ = -1;
	}
	ورنہ
	    نتیجہ = -1;
	vim_free(قوت_نما);
    }
    ورنہ
	نتیجہ = mch_remove(name) == 0 ? 0 : -1;

    واپس نتیجہ;
}
#اگر_ختم

#اگر defined(TEMPDIRNAMES)
ساکن طویل	عارضی_گنتی = 0;		// Temp filename counter.

# اگر defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
/*
 * Open temporary directory and take file lock to prevent
 * to be auto-cleaned.
 */
   ساکن خالی
vim_opentempdir(خالی)
{
    DIR *dp = تہی;

    اگر (vim_tempdir_dp != تہی)
	واپس;

    dp = opendir((ثابت حرف*)vim_tempdir);
    اگر (dp == تہی)
	واپس;

    vim_tempdir_dp = dp;
    flock(dirfd(vim_tempdir_dp), LOCK_SH);
}

/*
 * Close temporary directory - it automatically release file lock.
 */
   ساکن خالی
vim_closetempdir(خالی)
{
    اگر (vim_tempdir_dp == تہی)
	واپس;

    closedir(vim_tempdir_dp);
    vim_tempdir_dp = تہی;
}

/*
 * Return true if the temp directory we created is gone.
 */
    ساکن bool
vim_tempdir_gone(خالی)
{
    stat_T	st;

    اگر (vim_tempdir_dp == تہی)
	واپس false;
    واپس fstat(dirfd(vim_tempdir_dp), &st) < 0 || st.st_nlink == 0;
}
# endif

/*
 * Delete the temp directory and all files it contains.
 */
    خالی
vim_deltempdir(خالی)
{
    اگر (vim_tempdir == تہی)
	واپس;

# اگر defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    vim_closetempdir();
# endif
    // remove the trailing path separator
    gettail(vim_tempdir)[-1] = NUL;
    delete_recursive(vim_tempdir);
    VIM_CLEAR(vim_tempdir);
}

/*
 * Directory "tempdir" was created.  Expand this name to a full path and put
 * it in "vim_tempdir".  This avoids that using ":cd" would confuse us.
 * "tempdir" must be no longer than MAXPATHL.
 */
    ساکن خالی
vim_settempdir(char_u *tempdir)
{
    char_u	*buf;
    size_t	buflen;

    buf = alloc(MAXPATHL + 2);
    اگر (buf == تہی)
	واپس;

    اگر (vim_بھرا_Name(tempdir, buf, MAXPATHL, FALSE) == FAIL)
	STRCPY(buf, tempdir);
    buflen = STRLEN(buf);
    اگر (!after_pathsep(buf, buf + buflen))
    {
	STRCPY(buf + buflen, PATHSEPSTR);
	buflen += حجم(PATHSEP);
    }
    vim_tempdir = vim_strnsave(buf, buflen);
# اگر defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    vim_opentempdir();
# endif
    vim_free(buf);
}
#اگر_ختم

/*
 * vim_tempname(): Return a unique name that can be used for a temp file.
 *
 * The temp file is NOT guaranteed to be created.  If "keep" is FALSE it is
 * guaranteed to NOT be created.
 *
 * The returned pointer is to allocated memory.
 * The returned pointer is NULL if no valid name was found.
 */
    char_u  *
vim_tempname(
    صحیح	    extra_char غیر_مستعمل,  // char to use in the name instead of '?'
    صحیح	    keep غیر_مستعمل)
{
#اگر_تعریف_شدہ USE_TMPNAM
    char_u	itmp[L_tmpnam];	// use tmpnam()
#وگرنہ_اگر defined(MSWIN)
    WCHAR	itmp[TEMPNAMELEN];
#ورنہ
    char_u	itmp[TEMPNAMELEN];
#اگر_ختم

#اگر_تعریف_شدہ TEMPDIRNAMES
    ساکن حرف	*(tempdirs[]) = {TEMPDIRNAMES};
    صحیح		i;
# ifndef EEXIST
    stat_T	st;
# endif

# اگر defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    // if the temp directory is gone, force re-creation of it
    اگر (vim_tempdir != تہی && vim_tempdir_gone())
    {
	vim_closetempdir();
	VIM_CLEAR(vim_tempdir);
    }
# endif

    /*
     * This will create a directory for private use by this instance of Vim.
     * This is done once, and the same directory is used for all temp files.
     * This method avoids security problems because of symlink attacks et al.
     * It's also a bit faster, because we only need to check for an existing
     * file when creating the directory and not for each temp file.
     */
    اگر (vim_tempdir == تہی)
    {
	/*
	 * Try the entries in TEMPDIRNAMES to create the temp directory.
	 */
	برائے (i = 0; i < (صحیح)ARRAY_لمبائی(tempdirs); ++i)
	{
# ifndef HAVE_MKDTEMP
	    طویل	nr;
	    طویل	off;
# endif
	    size_t	itmplen;

	    // Expand $TMP, leave room for "/v1100000/999999999".
	    // Skip the directory check if the expansion fails.
	    itmplen = expand_env((char_u *)tempdirs[i], itmp, TEMPNAMELEN - 20);
	    اگر (itmp[0] != '$' && mch_isdir(itmp))
	    {
		// directory exists
		اگر (!after_pathsep(itmp, itmp + itmplen))
		{
		    STRCPY(itmp + itmplen, PATHSEPSTR);
		    itmplen += حجم(PATHSEP);
		}

# ifdef HAVE_MKDTEMP
		{
#  اگر defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    mode_t	umask_save = umask(077);
#  endif
		    // Leave room for filename
		    STRCPY(itmp + itmplen, "vXXXXXX");
		    itmplen += STRLEN_LITERAL("vXXXXXX");
		    اگر (mkdtemp((حرف *)itmp) != تہی)
			vim_settempdir(itmp);
#  اگر defined(UNIX) || defined(VMS)
		    (خالی)umask(umask_save);
#  endif
		}
# ورنہ
		// Get an arbitrary number of up to 6 digits.  When it's
		// unlikely that it already exists it will be faster,
		// otherwise it doesn't matter.  The use of mkdir() avoids any
		// security problems because of the predictable number.
		nr = (mch_get_pid() + (طویل)وقت(تہی)) % 1000000L;

		// Try up to 10000 different values until we find a name that
		// doesn't exist.
		برائے (off = 0; off < 10000L; ++off)
		{
		    صحیح		r;
#  اگر defined(UNIX) || defined(VMS)
		    mode_t	umask_save;
#  endif

		    vim_snprintf((حرف *)itmp + itmplen, حجم(itmp) - itmplen, "v%ld", nr + off);
#  ifndef EEXIST
		    // If mkdir() does not set errno to EEXIST, check for
		    // existing file here.  There is a race condition then,
		    // although it's fail-safe.
		    اگر (mch_stat((حرف *)itmp, &st) >= 0)
			جاری;
#  endif
#  اگر defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    umask_save = umask(077);
#  endif
		    r = vim_mkdir(itmp, 0700);
#  اگر defined(UNIX) || defined(VMS)
		    (خالی)umask(umask_save);
#  endif
		    اگر (r == 0)
		    {
			vim_settempdir(itmp);
			توڑو;
		    }
#  ifdef EEXIST
		    // If the mkdir() didn't fail because the file/dir exists,
		    // we probably can't create any dir here, try another
		    // place.
		    اگر (errno != EEXIST)
#  endif
			توڑو;
		}
# endif // HAVE_MKDTEMP
		اگر (vim_tempdir != تہی)
		    توڑو;
	    }
	}
    }

    اگر (vim_tempdir != تہی)
    {
	// There is no need to check if the file exists, because we own the
	// directory and nobody else creates a file in it.
	صحیح itmplen = vim_snprintf((حرف *)itmp, حجم(itmp), "%s%ld", vim_tempdir, عارضی_گنتی++);
	واپس vim_strnsave(itmp, (size_t)itmplen);
    }

    واپس تہی;

#ورنہ // TEMPDIRNAMES
    char_u	*p;

# ifdef MSWIN
    WCHAR	wsz_عارضی_File[MAX_راستہ_2 + 1];
    WCHAR	buf4[4];
    WCHAR	*chartab = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char_u	*retval;
    char_u	*shname;
    طویل	i;

    wcscpy(itmp, L"");
    اگر (Get_عارضی_راستہ_W(MAX_راستہ_2, wsz_عارضی_File) == 0)
    {
	wsz_عارضی_File[0] = L'.';	// GetTempPathW() failed, use current dir
	wsz_عارضی_File[1] = L'\\';
	wsz_عارضی_File[2] = NUL;
    }
    wcscpy(buf4, L"VIM");

    // randomize the name to avoid collisions
    i = mch_get_pid() + extra_char;
    buf4[1] = chartab[i % 36];
    buf4[2] = chartab[(i / 36) % 36];
    اگر (Get_عارضی_File_Name_W(wsz_عارضی_File, buf4, 0, itmp) == 0)
	واپس تہی;
    اگر (!keep)
	// GetTempFileNameW() will create the file, we don't want that
	(خالی)DeleteFileW(itmp);

    // Backslashes in a temp file name cause problems when filtering with
    // "sh".  NOTE: This also checks 'shellcmdflag' to help those people who
    // didn't set 'shellslash' but only if not using PowerShell.
    retval = utf16_مقصد_enc(itmp, تہی);
    اگر (retval == تہی)
	واپس تہی;
    shname = gettail(p_sh);
    اگر ((*p_shcf == '-' && !(لڑی_تلاش((حرف *)shname, "powershell") != تہی
			     || لڑی_تلاش((حرف *)shname, "pwsh") != تہی ))
								    || p_ssl)
	برائے (p = retval; *p; ++p)
	    اگر (*p == '\\')
		*p = '/';
    واپس retval;

# ورنہ // MSWIN
    صحیح		itmplen;

#  ifdef USE_TMPNAM
    // tmpnam() will make its own name
    p = tmpnam((حرف *)itmp);
    اگر (p == تہی || *p == NUL)
	واپس تہی;
#  ورنہ

#   ifdef VMS_TEMPNAM
    // mktemp() is not working on VMS.  It seems to be
    // a do-nothing function. Therefore we use tempnam().
    vim_snprintf((حرف *)itmp, حجم(itmp), "VIM%c", extra_char);
    p = (char_u *)tempnam("tmp:", (حرف *)itmp);
    اگر (p != تہی)
    {
	// VMS will use '.LIS' if we don't explicitly specify an extension,
	// and VIM will then be unable to find the file later
	itmplen = vim_snprintf((حرف *)itmp, حجم(itmp), "%s.txt", p);
	آزاد(p);
    }
    ورنہ
	واپس تہی;
#   ورنہ
    STRCPY(itmp, TEMPNAME);
    itmplen = STRLEN_LITERAL(TEMPNAME);
    اگر ((p = vim_strchr(itmp, '?')) != تہی)
	*p = extra_char;
    اگر (mktemp((حرف *)itmp) == تہی)
	واپس تہی;
#   endif
#  endif

    واپس vim_strnsave(itmp, (size_t)itmplen);
# endif // MSWIN
#اگر_ختم // TEMPDIRNAMES
}

#اگر defined(BACKSLASH_اندر_FILENAME)
/*
 * Convert all backslashes in fname to forward slashes in-place, unless when
 * it looks like a URL.
 */
    خالی
forward_slash(char_u *fname)
{
    char_u	*p;

    اگر (راستہ_with_url(fname))
	واپس;
    برائے (p = fname; *p != NUL; ++p)
	// The Big5 encoding can have '\' in the trail byte.
	اگر (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
	    ++p;
	ورنہ اگر (*p == '\\')
	    *p = '/';
}
#اگر_ختم

/*
 * Try matching a filename with a "pattern" ("prog" is NULL), or use the
 * precompiled regprog "prog" ("pattern" is NULL).  That avoids calling
 * vim_regcomp() often.
 * Used for autocommands and 'wildignore'.
 * Returns TRUE if there is a match, FALSE otherwise.
 */
    صحیح
مطابقت_file_pat(
    char_u	*نمونہ,		// pattern to match with
    regprog_T	**prog,			// pre-compiled regprog or NULL
    char_u	*fname,			// full path of file name
    char_u	*sfname,		// short file name or NULL
    char_u	*دم,			// tail of path
    صحیح		allow_dirs)		// allow matching with dir
{
    regmatch_T	regmatch;
    صحیح		نتیجہ = FALSE;

    regmatch.rm_ic = p_fic; // ignore case if 'fileignorecase' is set
    اگر (prog != تہی)
	regmatch.regprog = *prog;
    ورنہ
	regmatch.regprog = vim_regcomp(نمونہ, RE_MAGIC);

    /*
     * Try for a match with the pattern with:
     * 1. the full file name, when the pattern has a '/'.
     * 2. the short file name, when the pattern has a '/'.
     * 3. the tail of the file name, when the pattern has no '/'.
     */
    اگر (regmatch.regprog != تہی
	     && ((allow_dirs
		     && (vim_regexec(&regmatch, fname, (colnr_T)0)
			 || (sfname != تہی
			     && vim_regexec(&regmatch, sfname, (colnr_T)0))))
		 || (!allow_dirs && vim_regexec(&regmatch, دم, (colnr_T)0))))
	نتیجہ = TRUE;

    اگر (prog != تہی)
	*prog = regmatch.regprog;
    ورنہ
	vim_regfree(regmatch.regprog);
    واپس نتیجہ;
}

/*
 * Return TRUE if a file matches with a pattern in "list".
 * "list" is a comma-separated list of patterns, like 'wildignore'.
 * "sfname" is the short file name or NULL, "ffname" the long file name.
 */
    صحیح
مطابقت_file_list(char_u *list, char_u *sfname, char_u *ffname)
{
    char_u	buf[MAXPATHL];
    char_u	*دم;
    char_u	*regpat;
    حرف	allow_dirs;
    صحیح		مطابقت;
    char_u	*p;

    دم = gettail(sfname);

    // try all patterns in 'wildignore'
    p = list;
    جب (*p)
    {
	copy_option_part(&p, buf, MAXPATHL, ",");
	regpat = file_pat_مقصد_reg_pat(buf, تہی, &allow_dirs, FALSE);
	اگر (regpat == تہی)
	    توڑو;
	مطابقت = مطابقت_file_pat(regpat, تہی, ffname, sfname,
						       دم, (صحیح)allow_dirs);
	vim_free(regpat);
	اگر (مطابقت)
	    واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Convert the given pattern "pat" which has shell style wildcards in it, into
 * a regular expression, and return the result in allocated memory.  If there
 * is a directory path separator to be matched, then TRUE is put in
 * allow_dirs, otherwise FALSE is put there -- webb.
 * Handle backslashes before special characters, like "\*" and "\ ".
 *
 * no_bslash only makes a difference, when BACKSLASH_IN_FILENAME is defined
 *
 * Returns NULL when out of memory.
 */
    char_u *
file_pat_مقصد_reg_pat(
    char_u	*pat,
    char_u	*pat_انتہا,	// first char after pattern or NULL
    حرف	*allow_dirs,	// Result passed back out in here
    صحیح		no_bslash غیر_مستعمل) // Don't use a backward slash as pathsep
{
    صحیح		سائز = 2; // '^' at start, '$' at end
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    صحیح		i;
    صحیح		nested = 0;
    صحیح		جوڑو_dollar = TRUE;

    اگر (allow_dirs != تہی)
	*allow_dirs = FALSE;
    اگر (pat_انتہا == تہی)
	pat_انتہا = pat + STRLEN(pat);

    برائے (p = pat; p < pat_انتہا; p++)
    {
	انتخاب (*p)
	{
	    حالت '*':
	    حالت '.':
	    حالت ',':
	    حالت '{':
	    حالت '}':
	    حالت '~':
		سائز += 2;	// extra backslash
		توڑو;
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	    حالت '\\':
	    حالت '/':
		سائز += 4;	// could become "[\/]"
		توڑو;
#اگر_ختم
	    طےشدہ:
		سائز++;
		اگر (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		{
		    ++p;
		    ++سائز;
		}
		توڑو;
	}
    }
    reg_pat = alloc(سائز + 1);
    اگر (reg_pat == تہی)
	واپس تہی;

    i = 0;

    اگر (pat[0] == '*')
	جب (pat[0] == '*' && pat < pat_انتہا - 1)
	    pat++;
    ورنہ
	reg_pat[i++] = '^';
    endp = pat_انتہا - 1;
    اگر (endp >= pat && *endp == '*')
    {
	جب (endp - pat > 0 && *endp == '*')
	    endp--;
	جوڑو_dollar = FALSE;
    }
    برائے (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	انتخاب (*p)
	{
	    حالت '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		جب (p[1] == '*')	// "**" matches like "*"
		    ++p;
		توڑو;
	    حالت '.':
	    حالت '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		توڑو;
	    حالت '?':
		reg_pat[i++] = '.';
		توڑو;
	    حالت '\\':
		اگر (p[1] == NUL)
		    توڑو;
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
		اگر (!no_bslash)
		{
		    // translate:
		    // "\x" to "\\x"  e.g., "dir\file"
		    // "\*" to "\\.*" e.g., "dir\*.c"
		    // "\?" to "\\."  e.g., "dir\??.c"
		    // "\+" to "\+"   e.g., "fileX\+.c"
		    اگر ((vim_isfilec(p[1]) || p[1] == '*' || p[1] == '?')
			    && p[1] != '+')
		    {
			reg_pat[i++] = '[';
			reg_pat[i++] = '\\';
			reg_pat[i++] = '/';
			reg_pat[i++] = ']';
			اگر (allow_dirs != تہی)
			    *allow_dirs = TRUE;
			توڑو;
		    }
		}
#اگر_ختم
		// Undo escaping from ExpandEscape():
		// foo\?bar -> foo?bar
		// foo\%bar -> foo%bar
		// foo\,bar -> foo,bar
		// foo\ bar -> foo bar
		// Don't unescape \, * and others that are also special in a
		// regexp.
		// An escaped { must be unescaped since we use magic not
		// verymagic.  Use "\\\{n,m\}"" to get "\{n,m}".
		اگر (*++p == '?'
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
			&& no_bslash
#اگر_ختم
			)
		    reg_pat[i++] = '?';
		ورنہ
		    اگر (*p == ',' || *p == '%' || *p == '#'
			       || vim_isspace(*p) || *p == '{' || *p == '}')
			reg_pat[i++] = *p;
		    ورنہ اگر (*p == '\\' && p[1] == '\\' && p[2] == '{')
		    {
			reg_pat[i++] = '\\';
			reg_pat[i++] = '{';
			p += 2;
		    }
		    ورنہ
		    {
			اگر (allow_dirs != تہی && vim_ispathsep(*p)
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
				&& (!no_bslash || *p != '\\')
#اگر_ختم
				)
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			اگر (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
			    reg_pat[i++] = *p++;
			reg_pat[i++] = *p;
		    }
		توڑو;
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	    حالت '/':
		reg_pat[i++] = '[';
		reg_pat[i++] = '\\';
		reg_pat[i++] = '/';
		reg_pat[i++] = ']';
		اگر (allow_dirs != تہی)
		    *allow_dirs = TRUE;
		توڑو;
#اگر_ختم
	    حالت '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		توڑو;
	    حالت '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		توڑو;
	    حالت ',':
		اگر (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		ورنہ
		    reg_pat[i++] = ',';
		توڑو;
	    طےشدہ:
		اگر (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		    reg_pat[i++] = *p++;
		ورنہ اگر (allow_dirs != تہی && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		توڑو;
	}
    }
    اگر (جوڑو_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    اگر (nested != 0)
    {
	اگر (nested < 0)
	    emsg(_(e_missing_open_curly));
	ورنہ
	    emsg(_(e_missing_close_curly));
	VIM_CLEAR(reg_pat);
    }
    واپس reg_pat;
}

#اگر defined(EINTR)
/*
 * Version of read() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    طویل
read_eintr(صحیح fd, خالی *buf, size_t bufsize)
{
    طویل ret;

    برائے (;;)
    {
	ret = vim_read(fd, buf, bufsize);
	اگر (ret >= 0 || errno != EINTR)
	    توڑو;
    }
    واپس ret;
}

/*
 * Version of write() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    طویل
write_eintr(صحیح fd, خالی *buf, size_t bufsize)
{
    طویل    ret = 0;
    طویل    wlen;

    // Repeat the write() so long it didn't fail, other than being interrupted
    // by a signal.
    جب (ret < (طویل)bufsize)
    {
	wlen = vim_write(fd, (حرف *)buf + ret, bufsize - ret);
	اگر (wlen < 0)
	{
	    اگر (errno != EINTR)
		توڑو;
	}
	ورنہ
	    ret += wlen;
    }
    واپس ret;
}
#اگر_ختم
