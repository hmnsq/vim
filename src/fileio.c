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
#endif
#اگر (defined(UNIX) || defined(VMS)) && defined(FEAT_EVAL)
# include <pwd.h>
# include <grp.h>
#endif
#اگر defined(VMS) && defined(HAVE_XOS_R_H)
# include <x11/xos_r.h>
#endif

// Is there any system that doesn't have access()?
#define USE_MCH_ACCESS

// Bitmask with 0x80 set in each byte of a long_u word, used to detect
// non-ASCII bytes (high bit set) in multiple bytes at once.
#define NONASCII_MASK (((long_u)-1 / 0xFF) * 0x80)

#اگر defined(__hpux) && !defined(HAVE_DIRFD)
# define dirfd(x) ((x)->__dd_fd)
# define HAVE_DIRFD
#endif

ایستا char_u *next_fenc(char_u **pp, صحیح *alloced);
#ifdef FEAT_EVAL
ایستا char_u *readfile_charconvert(char_u *fname, char_u *fenc, صحیح *fdp);
#endif
#ifdef FEAT_CRYPT
ایستا char_u *check_for_cryptkey(char_u *cryptkey, char_u *ptr, long *sizep, off_T *filesizep, صحیح newfile, char_u *fname, صحیح *did_ask);
#endif
ایستا linenr_T readfile_linenr(linenr_T linecnt, char_u *p, char_u *endp);
ایستا char_u *check_for_bom(char_u *p, long size, صحیح *lenp, صحیح flags);

#ifdef FEAT_EVAL
ایستا صحیح readdirex_sort;
#endif

    تهی
filemess(
    buf_T	*buf,
    char_u	*name,
    char_u	*s,
    صحیح		attr)
{
    صحیح		پیام_scroll_save;
    صحیح		prev_پیام_col = پیام_col;
    size_t	len;

    اگر (پیام_silent != 0)
	بازگشت;
    پیام_add_fname(buf, name);	    // put file name in IObuff with quotes

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
    پیام_scroll_save = پیام_scroll;
    اگر (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	پیام_scroll = FALSE;
    اگر (!پیام_scroll)	// wait a bit when overwriting an error msg
	check_for_delay(FALSE);
    پیام_start();
    اگر (prev_پیام_col != 0 && پیام_col == 0)
	پیام_putchar('\r');  // overwrite any previous message.
    پیام_scroll = پیام_scroll_save;
    پیام_scrolled_ign = TRUE;
    // may truncate the message to avoid a hit-return prompt
    پیام_outtrans_attr(پیام_may_trunc(FALSE, IObuff), attr);
    پیام_clr_eos();
    out_flush();
    پیام_scrolled_ign = FALSE;
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
    linenr_T	سطرها_to_skip,
    linenr_T	سطرها_to_read,
    exarg_T	*eap,			// can be NULL!
    صحیح		flags)
{
    صحیح		retval = FAIL;	// jump to "theend" instead of returning
    صحیح		fd = 0;
    صحیح		newfile = (flags & READ_NEW);
    صحیح		check_readonly;
    صحیح		filtering = (flags & READ_FILTER);
    صحیح		read_stdin = (flags & READ_STDIN);
    صحیح		read_buffer = (flags & READ_BUFFER);
    صحیح		read_fifo = (flags & READ_FIFO);
    صحیح		set_options = newfile || read_buffer
					   || (eap != پوچ && eap->read_edit);
    linenr_T	read_buf_lnum = 1;	// next line to read from curbuf
    colnr_T	read_buf_col = 0;	// next char to read from this line
    char_u	c;
    linenr_T	lnum = from;
    char_u	*ptr = پوچ;		// pointer into read buffer
    char_u	*buffer = پوچ;		// read buffer
    char_u	*new_buffer = پوچ;	// init to shut up gcc
    char_u	*سطر_start = پوچ;	// init to shut up gcc
    صحیح		wasempty;		// buffer was empty before reading
    colnr_T	len;
    long	size = 0;
    char_u	*p;
    off_T	filesize = 0;
    صحیح		skip_read = FALSE;
#ifdef FEAT_CRYPT
    off_T       filesize_disk = 0;      // file size read from disk
    off_T       filesize_شمار = 0;     // counter
    char_u	*cryptkey = پوچ;
    صحیح		did_ask_for_کلید = FALSE;
#endif
#ifdef FEAT_PERSISTENT_UNDO
    context_sha256_T sha_ctx;
    صحیح		read_undo_file = FALSE;
#endif
    صحیح		split = 0;		// number of split lines
#define UNKNOWN	 0x0fffffff		// file size is unknown
    linenr_T	linecnt;
    صحیح		error = FALSE;		// errors encountered
    صحیح		ff_error = EOL_UNKNOWN; // file format with errors
    long	linerest = 0;		// remaining chars in line
#ifdef UNIX
    صحیح		perm = 0;
    صحیح		swap_mode = -1;		// protection bits for swap file
#وگرنه
    صحیح		perm;
#endif
    صحیح		fileformat = 0;		// end-of-line format
    صحیح		keep_fileformat = FALSE;
    stat_T	st;
    صحیح		file_readonly;
    linenr_T	skip_شمار = 0;
    linenr_T	read_شمار = 0;
    صحیح		پیام_save = پیام_scroll;
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
    char_u	*tmpname = پوچ;	// name of 'charconvert' output file
    صحیح		fio_flags = 0;
    char_u	*fenc;			// fileencoding to use
    صحیح		fenc_alloced;		// fenc_next is in allocated memory
    char_u	*fenc_next = پوچ;	// next item in 'fencs' or NULL
    صحیح		advance_fenc = FALSE;
    long	real_size = 0;
#ifdef USE_ICONV
    iconv_t	iconv_fd = (iconv_t)-1;	// descriptor for iconv() or -1
# ifdef FEAT_EVAL
    صحیح		did_iconv = FALSE;	// TRUE when iconv() failed and trying
					// 'charconvert' next
# endif
#endif
    صحیح		converted = FALSE;	// TRUE if conversion done
    صحیح		notconverted = FALSE;	// TRUE if conversion wanted but it
					// wasn't possible
    char_u	conv_rest[CONV_RESTLEN];
    صحیح		conv_restlen = 0;	// nr of bytes in conv_rest[]
    pos_T	orig_start;
    buf_T	*old_curbuf;
    char_u	*old_b_ffname;
    char_u	*old_b_fname;
    صحیح		using_b_ffname;
    صحیح		using_b_fname;
    ایستا نویسه *پیام_is_a_directory = N_("is a directory");
#ifdef FEAT_CRYPT
    صحیح		eof = FALSE;
#endif
#ifdef FEAT_SODIUM
    صحیح		may_need_lseek = FALSE;
#endif
    size_t	fnamelen = 0;

    curbuf->b_au_did_filetype = false; // reset before triggering any autocommands

    curbuf->b_no_eol_lnum = 0;	// in case it was set by the previous read

    /*
     * If there is no file name yet, use the one for the read file.
     * BF_NOTEDITED is set to reflect this.
     * Don't do this for a read from a filter.
     * Only do this when 'cpoptions' contains the 'f' flag.
     */
    اگر (curbuf->b_ffname == پوچ
	    && !filtering
	    && fname != پوچ
	    && vim_strchr(p_cpo, CPO_FNAMER) != پوچ
	    && !(flags & READ_DUMMY))
    {
	اگر (set_rw_fname(fname, sfname) == FAIL)
	    برو theend;
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
    اگر (sfname == پوچ)
	sfname = fname;
#اگر defined(UNIX)
    fname = sfname;
#endif

    /*
     * The BufReadCmd and FileReadCmd events intercept the reading process by
     * executing the associated commands instead.
     */
    اگر (!filtering && !read_stdin && !read_buffer)
    {
	orig_start = curbuf->b_op_start;

	// Set '[ mark to the line above where the lines go (line 1 if zero).
	curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
	curbuf->b_op_start.col = 0;

	اگر (newfile)
	{
	    اگر (apply_autocmds_exarg(EVENT_BUFREADCMD, پوچ, sfname,
							  FALSE, curbuf, eap))
	    {
		retval = OK;
#ifdef FEAT_EVAL
		اگر (aborting())
		    retval = FAIL;
#endif
		// The BufReadCmd code usually uses ":read" to get the text and
		// perhaps ":file" to change the buffer name. But we should
		// consider this to work like ":edit", thus reset the
		// BF_NOTEDITED flag.  Then ":write" will work to overwrite the
		// same file.
		اگر (retval == OK)
		    curbuf->b_flags &= ~BF_NOTEDITED;
		برو theend;
	    }
	}
	وگرنه اگر (apply_autocmds_exarg(EVENT_FILEREADCMD, sfname, sfname,
							    FALSE, پوچ, eap))
	{
#ifdef FEAT_EVAL
	    retval = aborting() ? FAIL : OK;
#وگرنه
	    retval = OK;
#endif
	    برو theend;
	}

	curbuf->b_op_start = orig_start;

	اگر (flags & READ_NOFILE)
	{
	    // Return NOTDONE instead of FAIL so that BufEnter can be triggered
	    // and other operations don't fail.
	    retval = NOTDONE;
	    برو theend;
	}
    }

    اگر ((shortmess(SHM_OVER) || curbuf->b_help) && p_verbose == 0)
	پیام_scroll = FALSE;	// overwrite previous file message
    وگرنه
	پیام_scroll = TRUE;	// don't overwrite previous file message

    اگر (fname != پوچ && *fname != NUL)
    {
	fnamelen = STRLEN(fname);

	// If the name is too long we might crash further on, quit here.
	اگر (fnamelen >= MAXPATHL)
	{
	    filemess(curbuf, fname, (char_u *)_("Illegal file name"), 0);
	    پیام_end();
	    پیام_scroll = پیام_save;
	    برو theend;
	}

	// If the name ends in a path separator, we can't open it.  Check here,
	// because reading the file may actually work, but then creating the
	// swap file may destroy it!  Reported on MS-DOS and Win 95.
	اگر (after_pathsep(fname, fname + fnamelen))
	{
	    filemess(curbuf, fname, (char_u *)_(پیام_is_a_directory), 0);
	    پیام_end();
	    پیام_scroll = پیام_save;
	    retval = NOTDONE;
	    برو theend;
	}
    }
#اگر defined(UNIX) || defined(VMS)
    اگر (!read_stdin && fname != پوچ)
	perm = mch_getperm(fname);
#endif

    اگر (!read_stdin && !read_buffer && !read_fifo)
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
		filemess(curbuf, fname, (char_u *)_(پیام_is_a_directory), 0);
		retval = NOTDONE;
	    }
	    وگرنه
		filemess(curbuf, fname, (char_u *)_("is not a file"), 0);
	    پیام_end();
	    پیام_scroll = پیام_save;
	    برو theend;
	}
#endif
#اگر defined(MSWIN)
	/*
	 * MS-Windows allows opening a device, but we will probably get stuck
	 * trying to read it.
	 */
	اگر (!p_odev && mch_nodetype(fname) == گره_WRITABLE)
	{
	    filemess(curbuf, fname, (char_u *)_("is a device (disabled with 'opendevice' option)"), 0);
	    پیام_end();
	    پیام_scroll = پیام_save;
	    برو theend;
	}
#endif
    }

    // Set default or forced 'fileformat' and 'binary'.
    set_file_options(set_options, eap);

    /*
     * When opening a new file we take the readonly flag from the file.
     * Default is r/w, can be set to r/o below.
     * Don't reset it when in readonly mode
     * Only set/reset b_p_ro when BF_CHECK_RO is set.
     */
    check_readonly = (newfile && (curbuf->b_flags & BF_CHECK_RO));
    اگر (check_readonly && !readonlymode)
	curbuf->b_p_ro = FALSE;

    اگر (newfile && !read_stdin && !read_buffer && !read_fifo)
    {
	// Remember time of file.
	اگر (mch_stat((نویسه *)fname, &st) >= 0)
	{
	    buf_store_time(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
#ifdef FEAT_CRYPT
	    filesize_disk = st.st_size;
#endif
#ifdef UNIX
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
	    swap_mode = (st.st_mode & 0644) | 0600;
#endif
#ifdef VMS
	    curbuf->b_fab_rfm = st.st_fab_rfm;
	    curbuf->b_fab_rat = st.st_fab_rat;
	    curbuf->b_fab_mrs = st.st_fab_mrs;
#endif
	}
	وگرنه
	{
	    curbuf->b_mtime = 0;
	    curbuf->b_mtime_ns = 0;
	    curbuf->b_mtime_read = 0;
	    curbuf->b_mtime_read_ns = 0;
	    curbuf->b_orig_size = 0;
	    curbuf->b_orig_mode = 0;
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
	setmode(0, O_دودویی);
#endif
    }
    وگرنه اگر (!read_buffer)
    {
#ifdef USE_MCH_ACCESS
	اگر (
# ifdef UNIX
	    !(perm & 0222) ||
# endif
				mch_access((نویسه *)fname, W_OK))
	    file_readonly = TRUE;
	fd = mch_open((نویسه *)fname, O_RDONLY | O_EXTRA, 0);
#وگرنه
	اگر (!newfile
		|| readonlymode
		|| (fd = mch_open((نویسه *)fname, O_RDWR | O_EXTRA, 0)) < 0)
	{
	    file_readonly = TRUE;
	    // try to open ro
	    fd = mch_open((نویسه *)fname, O_RDONLY | O_EXTRA, 0);
	}
#endif
    }

    اگر (fd < 0)			    // cannot open at all
    {
#ifndef UNIX
	صحیح	isdir_f;
#endif
	پیام_scroll = پیام_save;
#ifndef UNIX
	/*
	 * On Amiga we can't open a directory, check here.
	 */
	isdir_f = (mch_isdir(fname));
	perm = mch_getperm(fname);  // check if the file exists
	اگر (isdir_f)
	{
	    filemess(curbuf, sfname, (char_u *)_(پیام_is_a_directory), 0);
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	}
	وگرنه
#endif
	    اگر (newfile)
	    {
		اگر (perm < 0
#ifdef ENOENT
			&& errno == ENOENT
#endif
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
			check_need_swap(newfile);
			// SwapExists autocommand may mess things up
			اگر (curbuf != old_curbuf
				|| (using_b_ffname
					&& (old_b_ffname != curbuf->b_ffname))
				|| (using_b_fname
					 && (old_b_fname != curbuf->b_fname)))
			{
			    emsg(_(e_autocommands_changed_buffer_or_buffer_name));
			    برو theend;
			}
		    }
		    اگر (dir_of_file_exists(fname))
			filemess(curbuf, sfname,
					      (char_u *)new_file_message(), 0);
		    وگرنه
			filemess(curbuf, sfname,
					   (char_u *)_("[New DIRECTORY]"), 0);
#ifdef FEAT_VIMINFO
		    // Even though this is a new file, it might have been
		    // edited before and deleted.  Get the old marks.
		    check_marks_read();
#endif
		    // Set forced 'fileencoding'.
		    اگر (eap != پوچ)
			set_forced_fenc(eap);
		    apply_autocmds_exarg(EVENT_BUFNEWFILE, sfname, sfname,
							  FALSE, curbuf, eap);
		    // remember the current fileformat
		    save_file_ff(curbuf);

#اگر defined(FEAT_EVAL)
		    اگر (!aborting())   // autocmds may abort script processing
#endif
			retval = OK;	    // a new file is not an error
		    برو theend;
		}
		وگرنه
		{
		    filemess(curbuf, sfname, (char_u *)(
#ifdef EFBIG
			    (errno == EFBIG) ? _("[File too big]") :
#endif
#ifdef EOVERFLOW
			    (errno == EOVERFLOW) ? _("[File too big]") :
#endif
						_("[Permission Denied]")), 0);
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
		}
	    }

	برو theend;
    }

    /*
     * Only set the 'ro' flag for readonly files the first time they are
     * loaded.	Help files always get readonly mode
     */
    اگر ((check_readonly && file_readonly) || curbuf->b_help)
	curbuf->b_p_ro = TRUE;

    اگر (set_options)
    {
	// Don't change 'eol' if reading from buffer as it will already be
	// correctly set when reading stdin.
	اگر (!read_buffer)
	{
	    curbuf->b_p_eof = FALSE;
	    curbuf->b_start_eof = FALSE;
	    curbuf->b_p_eol = TRUE;
	    curbuf->b_start_eol = TRUE;
	}
	curbuf->b_p_bomb = FALSE;
	curbuf->b_start_bomb = FALSE;
    }

    // Create a swap file now, so that other Vims are warned that we are
    // editing this file.
    // Don't do this for a "nofile" or "nowrite" buffer type.
    اگر (!bt_dontwrite(curbuf))
    {
	check_need_swap(newfile);
	اگر (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))))
	{
	    emsg(_(e_autocommands_changed_buffer_or_buffer_name));
	    اگر (!read_buffer)
		close(fd);
	    برو theend;
	}
#ifdef UNIX
	// Set swap file protection bits after creating it.
	اگر (swap_mode > 0 && curbuf->b_ml.ml_mfp != پوچ
			  && curbuf->b_ml.ml_mfp->mf_fname != پوچ)
	{
	    char_u *swap_fname = curbuf->b_ml.ml_mfp->mf_fname;

	    /*
	     * If the group-read bit is set but not the world-read bit, then
	     * the group must be equal to the group of the original file.  If
	     * we can't make that happen then reset the group-read bit.  This
	     * avoids making the swap file readable to more users when the
	     * primary group of the user is too permissive.
	     */
	    اگر ((swap_mode & 044) == 040)
	    {
		stat_T	swap_st;

		اگر (mch_stat((نویسه *)swap_fname, &swap_st) >= 0
			&& st.st_gid != swap_st.st_gid
# ifdef HAVE_FCHOWN
			&& fchown(curbuf->b_ml.ml_mfp->mf_fd, -1, st.st_gid)
									  == -1
# endif
		   )
		    swap_mode &= 0600;
	    }

	    (تهی)mch_setperm(swap_fname, (long)swap_mode);
	}
#endif
    }

    // If "Quit" selected at ATTENTION dialog, don't load the file
    اگر (swap_exists_action == SEA_QUIT)
    {
	اگر (!read_buffer && !read_stdin)
	    close(fd);
	برو theend;
    }

    ++no_wait_return;	    // don't wait for return yet

    /*
     * Set '[ mark to the line above where the lines go (line 1 if zero).
     */
    orig_start = curbuf->b_op_start;
    curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
    curbuf->b_op_start.col = 0;

    try_mac = (vim_strchr(p_ffs, 'm') != پوچ);
    try_dos = (vim_strchr(p_ffs, 'd') != پوچ);
    try_unix = (vim_strchr(p_ffs, 'x') != پوچ);

    اگر (!read_buffer)
    {
	صحیح	m = پیام_scroll;
	صحیح	n = پیام_scrolled;

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
	پیام_scroll = TRUE;
	اگر (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPRE, پوچ, sfname,
							  FALSE, curbuf, eap);
	وگرنه اگر (read_stdin)
	    apply_autocmds_exarg(EVENT_STDINREADPRE, پوچ, sfname,
							  FALSE, curbuf, eap);
	وگرنه اگر (newfile)
	    apply_autocmds_exarg(EVENT_BUFREADPRE, پوچ, sfname,
							  FALSE, curbuf, eap);
	وگرنه
	    apply_autocmds_exarg(EVENT_FILEREADPRE, sfname, sfname,
							    FALSE, پوچ, eap);
	// autocommands may have changed it
	try_mac = (vim_strchr(p_ffs, 'm') != پوچ);
	try_dos = (vim_strchr(p_ffs, 'd') != پوچ);
	try_unix = (vim_strchr(p_ffs, 'x') != پوچ);
	curbuf->b_op_start = orig_start;

	اگر (پیام_scrolled == n)
	    پیام_scroll = m;

#ifdef FEAT_EVAL
	اگر (aborting())	    // autocmds may abort script processing
	{
	    --no_wait_return;
	    پیام_scroll = پیام_save;
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    برو theend;
	}
#endif
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
		|| (fd = mch_open((نویسه *)fname, O_RDONLY | O_EXTRA, 0)) < 0))
	{
	    --no_wait_return;
	    پیام_scroll = پیام_save;
	    اگر (fd < 0)
		emsg(_(e_readpre_autocommands_made_file_unreadable));
	    وگرنه
		emsg(_(e_readpre_autocommands_must_not_change_current_buffer));
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    برو theend;
	}
    }

    // Autocommands may add lines to the file, need to check if it is empty
    wasempty = (curbuf->b_ml.ml_flags & ML_EMPTY);

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
	    اگر (!is_not_a_term())
	    {
#ifndef ALWAYS_USE_GUI
# ifdef VIMDLL
		اگر (!gui.in_use)
# endif
		    mch_پیام(_("Vim: Reading from stdin...\n"));
#endif
#ifdef FEAT_GUI
		// Also write a message in the GUI window, if there is one.
		اگر (gui.in_use && !gui.dying && !gui.starting)
		{
		    size_t  plen = STRLEN(_("Reading from stdin..."));

		    // make a copy, gui_write() may try to change it
		    p = vim_strnsave((char_u *)_("Reading from stdin..."), plen);
		    اگر (p != پوچ)
		    {
			gui_write(p, (صحیح)plen);
			vim_free(p);
		    }
		}
#endif
	    }
	}
	وگرنه اگر (!read_buffer)
	    filemess(curbuf, sfname, (char_u *)"", 0);
    }

    پیام_scroll = FALSE;			// overwrite the file message

    /*
     * Set linecnt now, before the "retry" caused by a wrong guess for
     * fileformat, and after the autocommands, which may change them.
     */
    linecnt = curbuf->b_ml.ml_سطر_شمار;

    // "++bad=" argument.
    اگر (eap != پوچ && eap->bad_char != 0)
    {
	bad_char_behavior = eap->bad_char;
	اگر (set_options)
	    curbuf->b_bad_char = eap->bad_char;
    }
    وگرنه
	curbuf->b_bad_char = 0;

    /*
     * Decide which 'encoding' to use or use first.
     */
    اگر (eap != پوچ && eap->force_enc != 0)
    {
	fenc = enc_canonize(eap->cmd + eap->force_enc);
	fenc_alloced = TRUE;
	keep_dest_enc = TRUE;
    }
    وگرنه اگر (curbuf->b_p_bin)
    {
	fenc = (char_u *)"";		// binary: don't convert
	fenc_alloced = FALSE;
    }
    وگرنه اگر (curbuf->b_help)
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
		برای (p = firstline; p < firstline + len; ++p)
		    اگر (*p >= 0x80)
		    {
			c = TRUE;
			بشکن;
		    }
	    }
	}

	اگر (c)
	{
	    fenc_next = fenc;
	    fenc = (char_u *)"utf-8";

	    // When the file is utf-8 but a character doesn't fit in
	    // 'encoding' don't retry.  In help text editing utf-8 bytes
	    // doesn't make sense.
	    اگر (!enc_utf8)
		keep_dest_enc = TRUE;
	}
	fenc_alloced = FALSE;
    }
    وگرنه اگر (*p_fencs == NUL)
    {
	fenc = curbuf->b_p_fenc;	// use format from buffer
	fenc_alloced = FALSE;
    }
    وگرنه
    {
	fenc_next = p_fencs;		// try items in 'fileencodings'
	fenc = next_fenc(&fenc_next, &fenc_alloced);
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
	اگر (read_buffer)
	{
	    read_buf_lnum = 1;
	    read_buf_col = 0;
	}
	وگرنه اگر (read_stdin || vim_lseek(fd, (off_T)0L, SEEK_SET) != 0)
	{
	    // Can't rewind the file, give up.
	    error = TRUE;
	    برو failed;
	}
	// Delete the previously read lines.
	وقتی (lnum > from)
	    ml_delete(lnum--);
	file_rewind = FALSE;
	اگر (set_options)
	{
	    curbuf->b_p_bomb = FALSE;
	    curbuf->b_start_bomb = FALSE;
	}
	conv_error = 0;
    }

    /*
     * When retrying with another "fenc" and the first time "fileformat"
     * will be reset.
     */
    اگر (keep_fileformat)
	keep_fileformat = FALSE;
    وگرنه
    {
	اگر (eap != پوچ && eap->force_ff != 0)
	{
	    fileformat = get_fileformat_force(curbuf, eap);
	    try_unix = try_dos = try_mac = FALSE;
	}
	وگرنه اگر (curbuf->b_p_bin)
	    fileformat = EOL_UNIX;		// binary: use Unix format
	وگرنه اگر (*p_ffs == NUL)
	    fileformat = get_fileformat(curbuf);// use format from buffer
	وگرنه
	    fileformat = EOL_UNKNOWN;		// detect from file
    }

#ifdef USE_ICONV
    اگر (iconv_fd != (iconv_t)-1)
    {
	// aborted conversion with iconv(), close the descriptor
	iconv_close(iconv_fd);
	iconv_fd = (iconv_t)-1;
    }
#endif

    اگر (advance_fenc)
    {
	/*
	 * Try the next entry in 'fileencodings'.
	 */
	advance_fenc = FALSE;

	اگر (eap != پوچ && eap->force_enc != 0)
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
	وگرنه
	{
	    اگر (fenc_alloced)
		vim_free(fenc);
	    اگر (fenc_next != پوچ)
	    {
		fenc = next_fenc(&fenc_next, &fenc_alloced);
	    }
	    وگرنه
	    {
		fenc = (char_u *)"";
		fenc_alloced = FALSE;
	    }
	}
	اگر (tmpname != پوچ)
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
	وگرنه اگر (enc_utf8 || STRCMP(p_enc, "latin1") == 0)
	    fio_flags = get_fio_flags(fenc);

#ifdef MSWIN
	/*
	 * Conversion from an MS-Windows codepage to UTF-8 or another codepage
	 * is handled with MultiByteToWideChar().
	 */
	اگر (fio_flags == 0)
	    fio_flags = get_win_fio_flags(fenc);
#endif

#ifdef MACOS_CONVERT
	// Conversion from Apple MacRoman to latin1 or UTF-8
	اگر (fio_flags == 0)
	    fio_flags = get_mac_fio_flags(fenc);
#endif

#ifdef USE_ICONV
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
#endif

#ifdef FEAT_EVAL
	/*
	 * Use the 'charconvert' expression when conversion is required
	 * and we can't do it internally or with iconv().
	 */
	اگر (fio_flags == 0 && !read_stdin && !read_buffer && *p_ccv != NUL
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
	    اگر (tmpname == پوچ)
	    {
		tmpname = readfile_charconvert(fname, fenc, &fd);
		اگر (tmpname == پوچ)
		{
		    // Conversion failed.  Try another one.
		    advance_fenc = TRUE;
		    اگر (fd < 0)
		    {
			// Re-opening the original file failed!
			emsg(_(e_conversion_mad_file_unreadable));
			error = TRUE;
			برو failed;
		    }
		    برو retry;
		}
	    }
	}
	وگرنه
#endif
	{
	    اگر (fio_flags == 0
#ifdef USE_ICONV
		    && iconv_fd == (iconv_t)-1
#endif
	       )
	    {
		// Conversion wanted but we can't.
		// Try the next conversion in 'fileencodings'
		advance_fenc = TRUE;
		برو retry;
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
#ifdef FEAT_CRYPT
	filesize_شمار = 0;
#endif
	skip_شمار = سطرها_to_skip;
	read_شمار = سطرها_to_read;
	conv_restlen = 0;
#ifdef FEAT_PERSISTENT_UNDO
	read_undo_file = (newfile && (flags & READ_KEEP_UNDO) == 0
				  && curbuf->b_ffname != پوچ
				  && curbuf->b_p_udf
				  && !filtering
				  && !read_fifo
				  && !read_stdin
				  && !read_buffer);
	اگر (read_undo_file)
	    sha256_start(&sha_ctx);
#endif
#ifdef FEAT_CRYPT
	اگر (curbuf->b_cryptstate != پوچ)
	{
	    // Need to free the state, but keep the key, don't want to ask for
	    // it again.
	    crypt_free_state(curbuf->b_cryptstate);
	    curbuf->b_cryptstate = پوچ;
	}
#endif
    }

    وقتی (!error && !got_int)
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
		size = SSIZE_MAX;		    // use max I/O size, 52K
#وگرنه
		// Use buffer >= 64K.  Add linerest to double the size if the
		// line gets very long, to avoid a lot of copying. But don't
		// read more than 1 Mbyte at a time, so we can be interrupted.
		size = 0x10000L + linerest;
		اگر (size > 0x100000L)
		    size = 0x100000L;
#endif
	}

	// Protect against the argument of lalloc() going negative.
	// Also split lines that are too long for colnr_T.  After this check
	// passes, we read up to 'size' more bytes.  We must ensure that even
	// after that read, the line length won't exceed MAXCOL - 1 (because
	// we add 1 for the NUL when casting to colnr_T).  If this check fires,
	// we insert a synthetic newline immediately, so linerest doesn't grow.
	اگر (size < 0 || size + linerest + 1 < 0 || linerest >= MAXCOL - size)
	{
	    ++split;
	    *ptr = NL;		    // split line by inserting a NL
	    size = 1;
	}
	وگرنه
	{
	    اگر (!skip_read)
	    {
		برای ( ; size >= 10; size = (long)((long_u)size >> 1))
		{
		    اگر ((new_buffer = lalloc(size + linerest + 1,
							      FALSE)) != پوچ)
			بشکن;
		}
		اگر (new_buffer == پوچ)
		{
		    do_outofmem_پیام((long_u)(size * 2 + linerest + 1));
		    error = TRUE;
		    بشکن;
		}
		اگر (linerest)	// copy characters from the previous buffer
		    mch_memmove(new_buffer, ptr - linerest, (size_t)linerest);
		vim_free(buffer);
		buffer = new_buffer;
		ptr = buffer + linerest;
		سطر_start = buffer;

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
		real_size = (صحیح)size;
#ifdef USE_ICONV
		اگر (iconv_fd != (iconv_t)-1)
		    size = size / ICONV_MULT;
		وگرنه
#endif
		    اگر (fio_flags & FIO_LATIN1)
		    size = size / 2;
		وگرنه اگر (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    size = (size * 2 / 3) & ~1;
		وگرنه اگر (fio_flags & FIO_UCS4)
		    size = (size * 2 / 3) & ~3;
		وگرنه اگر (fio_flags == FIO_UCSBOM)
		    size = size / ICONV_MULT;	// worst case
#ifdef MSWIN
		وگرنه اگر (fio_flags & FIO_CODEPAGE)
		    size = size / ICONV_MULT;	// also worst case
#endif
#ifdef MACOS_CONVERT
		وگرنه اگر (fio_flags & FIO_MACROMAN)
		    size = size / ICONV_MULT;	// also worst case
#endif

		اگر (conv_restlen > 0)
		{
		    // Insert unconverted bytes from previous line.
		    mch_memmove(ptr, conv_rest, conv_restlen);
		    ptr += conv_restlen;
		    size -= conv_restlen;
		}

		اگر (read_buffer)
		{
		    /*
		     * Read bytes from curbuf.  Used for converting text read
		     * from stdin.
		     */
		    اگر (read_buf_lnum > from)
			size = 0;
		    وگرنه
		    {
			صحیح	n, ni;
			long	tlen;

			tlen = 0;
			برای (;;)
			{
			    p = ml_get(read_buf_lnum) + read_buf_col;
			    n = ml_get_len(read_buf_lnum) - read_buf_col;
			    اگر ((صحیح)tlen + n + 1 > size)
			    {
				// Filled up to "size", append partial line.
				// Change NL to NUL to reverse the effect done
				// below.
				n = (صحیح)(size - tlen);
				برای (ni = 0; ni < n; ++ni)
				{
				    اگر (p[ni] == NL)
					ptr[tlen++] = NUL;
				    وگرنه
					ptr[tlen++] = p[ni];
				}
				read_buf_col += n;
				بشکن;
			    }

			    // Append whole line and new-line.  Change NL
			    // to NUL to reverse the effect done below.
			    برای (ni = 0; ni < n; ++ni)
			    {
				اگر (p[ni] == NL)
				    ptr[tlen++] = NUL;
				وگرنه
				    ptr[tlen++] = p[ni];
			    }
			    ptr[tlen++] = NL;
			    read_buf_col = 0;
			    اگر (++read_buf_lnum > from)
			    {
				// When the last line didn't have an
				// end-of-line don't add it now either.
				اگر (!curbuf->b_p_eol)
				    --tlen;
				size = tlen;
#ifdef FEAT_CRYPT
				eof = TRUE;
#endif
				بشکن;
			    }
			}
		    }
		}
		وگرنه
		{
		    /*
		     * Read bytes from the file.
		     */
#ifdef FEAT_SODIUM
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
			size = WRITEBUFSIZE + crypt_get_max_header_len()
			    + crypto_secretstream_xchacha20poly1305_HEADERBYTES
				+ crypto_secretstream_xchacha20poly1305_ABYTES;
			may_need_lseek = TRUE;
		    }

		    وگرنه اگر (filesize > 0 && (curbuf->b_cryptstate != پوچ
				&& crypt_method_is_sodium(
					     curbuf->b_cryptstate->method_nr)))
		    {
			size = WRITEBUFSIZE + crypto_secretstream_xchacha20poly1305_ABYTES;
			// need to rewind by - add_len from CRYPT_M_SOD2 (see
			// description above)
			اگر (curbuf->b_cryptstate->method_nr == CRYPT_M_SOD
						     && !eof && may_need_lseek)
			{
			    lseek(fd, crypt_get_header_len(
					       curbuf->b_cryptstate->method_nr)
				       - crypt_get_max_header_len(), SEEK_CUR);
			    may_need_lseek = FALSE;
			}
		    }
#endif
		    long read_size = size;
		    size = read_eintr(fd, ptr, read_size);
#ifdef FEAT_CRYPT
		    // Did we reach end of file?
		    filesize_شمار += size;
		    eof = (size < read_size || filesize_شمار == filesize_disk);
#endif
		}

#ifdef FEAT_CRYPT
		/*
		 * At start of file: Check for magic number of encryption.
		 */
		اگر (filesize == 0 && size > 0)
		{
		    cryptkey = check_for_cryptkey(cryptkey, ptr, &size,
						  &filesize, newfile, sfname,
						  &did_ask_for_کلید);
# اگر defined(CRYPT_NOT_INPLACE) && defined(FEAT_PERSISTENT_UNDO)
		    اگر (curbuf->b_cryptstate != پوچ
				 && !crypt_works_inplace(curbuf->b_cryptstate))
			// reading undo file requires crypt_decode_inplace()
			read_undo_file = FALSE;
# endif
		}
		/*
		 * Decrypt the read bytes.  This is done before checking for
		 * EOF because the crypt layer may be buffering.
		 */
		اگر (cryptkey != پوچ && curbuf->b_cryptstate != پوچ
								   && size > 0)
		{
# ifdef CRYPT_NOT_INPLACE
		    اگر (crypt_works_inplace(curbuf->b_cryptstate))
		    {
# endif
			crypt_decode_inplace(curbuf->b_cryptstate, ptr,
								    size, eof);
# ifdef CRYPT_NOT_INPLACE
		    }
		    وگرنه
		    {
			char_u	*newptr = پوچ;
			صحیح	decrypted_size;

			decrypted_size = crypt_decode_alloc(
				    curbuf->b_cryptstate, ptr, size,
								 &newptr, eof);

			اگر (decrypted_size < 0)
			{
			    // error message already given
			    error = TRUE;
			    vim_free(newptr);
			    بشکن;
			}
			// If the crypt layer is buffering, not producing
			// anything yet, need to read more.
			اگر (decrypted_size == 0)
			    ادامه;

			اگر (linerest == 0)
			{
			    // Simple case: reuse returned buffer (may be
			    // NULL, checked later).
			    new_buffer = newptr;
			}
			وگرنه
			{
			    long_u	new_size;

			    // Need new buffer to add bytes carried over.
			    new_size = (long_u)(decrypted_size + linerest + 1);
			    new_buffer = lalloc(new_size, FALSE);
			    اگر (new_buffer == پوچ)
			    {
				do_outofmem_پیام(new_size);
				error = TRUE;
				بشکن;
			    }

			    mch_memmove(new_buffer, buffer, linerest + conv_restlen);
			    اگر (newptr != پوچ)
				mch_memmove(new_buffer + linerest + conv_restlen,
					newptr, decrypted_size);
			    vim_free(newptr);
			}

			اگر (new_buffer != پوچ)
			{
			    vim_free(buffer);
			    buffer = new_buffer;
			    new_buffer = پوچ;
			    سطر_start = buffer;
			    ptr = buffer + linerest + conv_restlen;
			    real_size = size;
			}
			size = decrypted_size;
		    }
# endif
		}
#endif

		اگر (size <= 0)
		{
		    اگر (size < 0)		    // read error
			error = TRUE;
		    وگرنه اگر (conv_restlen > 0)
		    {
			/*
			 * Reached end-of-file but some trailing bytes could
			 * not be converted.  Truncated file?
			 */

			// When we did a conversion report an error.
			اگر (fio_flags != 0
#ifdef USE_ICONV
				|| iconv_fd != (iconv_t)-1
#endif
			   )
			{
			    اگر (can_retry)
				برو rewind_retry;
			    اگر (conv_error == 0)
				conv_error = curbuf->b_ml.ml_سطر_شمار
								- linecnt + 1;
			}
			// Remember the first linenr with an illegal byte
			وگرنه اگر (illegal_byte == 0)
			    illegal_byte = curbuf->b_ml.ml_سطر_شمار
								- linecnt + 1;
			اگر (bad_char_behavior == BAD_DROP)
			{
			    *(ptr - conv_restlen) = NUL;
			    conv_restlen = 0;
			}
			وگرنه
			{
			    // Replace the trailing bytes with the replacement
			    // character if we were converting; if we weren't,
			    // leave the UTF8 checking code to do it, as it
			    // works slightly differently.
			    اگر (bad_char_behavior != BAD_KEEP && (fio_flags != 0
#ifdef USE_ICONV
				    || iconv_fd != (iconv_t)-1
#endif
			       ))
			    {
				وقتی (conv_restlen > 0)
				{
				    *(--ptr) = bad_char_behavior;
				    --conv_restlen;
				}
			    }
			    fio_flags = 0;	// don't convert this
#ifdef USE_ICONV
			    اگر (iconv_fd != (iconv_t)-1)
			    {
				iconv_close(iconv_fd);
				iconv_fd = (iconv_t)-1;
			    }
#endif
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
#ifdef FEAT_CRYPT
		   || (cryptkey != پوچ
			&& filesize == crypt_get_header_len(
						 crypt_get_method_nr(curbuf)))
#endif
		       )
		    && (fio_flags == FIO_UCSBOM
			|| (!curbuf->b_p_bomb
			    && tmpname == پوچ
			    && (*fenc == 'u' || (*fenc == NUL && enc_utf8)))))
	    {
		char_u	*ccname;
		صحیح	blen;

		// no BOM detection in a short file or in binary mode
		اگر (size < 2 || curbuf->b_p_bin)
		    ccname = پوچ;
		وگرنه
		    ccname = check_for_bom(ptr, size, &blen,
		      fio_flags == FIO_UCSBOM ? FIO_ALL : get_fio_flags(fenc));
		اگر (ccname != پوچ)
		{
		    // Remove BOM from the text
		    filesize += blen;
		    size -= blen;
		    mch_memmove(ptr, ptr + blen, (size_t)size);
		    اگر (set_options)
		    {
			curbuf->b_p_bomb = TRUE;
			curbuf->b_start_bomb = TRUE;
		    }
		}

		اگر (fio_flags == FIO_UCSBOM)
		{
		    اگر (ccname == پوچ)
		    {
			// No BOM detected: retry with next encoding.
			advance_fenc = TRUE;
		    }
		    وگرنه
		    {
			// BOM detected: set "fenc" and jump back
			اگر (fenc_alloced)
			    vim_free(fenc);
			fenc = ccname;
			fenc_alloced = FALSE;
		    }
		    // retry reading without getting new bytes or rewinding
		    skip_read = TRUE;
		    برو retry;
		}
	    }

	    // Include not converted bytes.
	    ptr -= conv_restlen;
	    size += conv_restlen;
	    conv_restlen = 0;
	    /*
	     * Break here for a read error or end-of-file.
	     */
	    اگر (size <= 0)
		بشکن;


#ifdef USE_ICONV
	    اگر (iconv_fd != (iconv_t)-1)
	    {
		/*
		 * Attempt conversion of the read bytes to 'encoding' using
		 * iconv().
		 */
		ثابت نویسه	*fromp;
		نویسه		*بالاترین;
		size_t		from_size;
		size_t		to_size;

		fromp = (نویسه *)ptr;
		from_size = size;
		ptr += size;
		بالاترین = (نویسه *)ptr;
		to_size = real_size - size;

		/*
		 * If there is conversion error or not enough room try using
		 * another conversion.  Except for when there is no
		 * alternative (help files).
		 */
		وقتی ((iconv(iconv_fd, (تهی *)&fromp, &from_size,
							       &بالاترین, &to_size)
			    == (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						  || from_size > CONV_RESTLEN)
		{
		    اگر (can_retry)
			برو rewind_retry;
		    اگر (conv_error == 0)
			conv_error = readfile_linenr(linecnt,
							  ptr, (char_u *)بالاترین);

		    // Deal with a bad byte and continue with the next.
		    ++fromp;
		    --from_size;
		    اگر (bad_char_behavior == BAD_KEEP)
		    {
			*بالاترین++ = *(fromp - 1);
			--to_size;
		    }
		    وگرنه اگر (bad_char_behavior != BAD_DROP)
		    {
			*بالاترین++ = bad_char_behavior;
			--to_size;
		    }
		}

		اگر (from_size > 0)
		{
		    // Some remaining characters, keep them for the next
		    // round.
		    mch_memmove(conv_rest, (char_u *)fromp, from_size);
		    conv_restlen = (صحیح)from_size;
		}

		// move the linerest to before the converted characters
		سطر_start = ptr - linerest;
		mch_memmove(سطر_start, buffer, (size_t)linerest);
		size = (long)((char_u *)بالاترین - ptr);
	    }
#endif

#ifdef MSWIN
	    اگر (fio_flags & FIO_CODEPAGE)
	    {
		char_u	*src, *dst;
		WCHAR	ucs2buf[3];
		صحیح	ucs2len;
		صحیح	codepage = FIO_GET_CP(fio_flags);
		صحیح	bytelen;
		صحیح	found_bad;
		نویسه	replstr[2];

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
		وگرنه
		    replstr[0] = '?';
		replstr[1] = NUL;

		/*
		 * Move the bytes to the end of the buffer, so that we have
		 * room to put the result at the start.
		 */
		src = ptr + real_size - size;
		mch_memmove(src, ptr, size);

		/*
		 * Do the conversion.
		 */
		dst = ptr;
		وقتی (size > 0)
		{
		    found_bad = FALSE;

# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
		    اگر (codepage == CP_UTF8)
		    {
			// Handle CP_UTF8 input ourselves to be able to handle
			// trailing bytes properly.
			// Get one UTF-8 character from src.
			bytelen = (صحیح)utf_ptr2len_len(src, size);
			اگر (bytelen > size)
			{
			    // Only got some bytes of a character.  Normally
			    // it's put in "conv_rest", but if it's too long
			    // deal with it as if they were illegal bytes.
			    اگر (bytelen <= CONV_RESTLEN)
				بشکن;

			    // weird overlong byte sequence
			    bytelen = size;
			    found_bad = TRUE;
			}
			وگرنه
			{
			    صحیح	    u8c = utf_ptr2char(src);

			    اگر (u8c > 0xffff || (*src >= 0x80 && bytelen == 1))
				found_bad = TRUE;
			    ucs2buf[0] = u8c;
			    ucs2len = 1;
			}
		    }
		    وگرنه
# endif
		    {
			// We don't know how long the byte sequence is, try
			// from one to three bytes.
			برای (bytelen = 1; bytelen <= size && bytelen <= 3;
								    ++bytelen)
			{
			    ucs2len = MultiByteToWideChar(codepage,
							 MB_ERR_INVALID_CHARS,
							 (LPCSTR)src, bytelen,
								   ucs2buf, 3);
			    اگر (ucs2len > 0)
				بشکن;
			}
			اگر (ucs2len == 0)
			{
			    // If we have only one byte then it's probably an
			    // incomplete byte sequence.  Otherwise discard
			    // one byte as a bad character.
			    اگر (size == 1)
				بشکن;
			    found_bad = TRUE;
			    bytelen = 1;
			}
		    }

		    اگر (!found_bad)
		    {
			صحیح	i;

			// Convert "ucs2buf[ucs2len]" to 'enc' in "dst".
			اگر (enc_utf8)
			{
			    // From UCS-2 to UTF-8.  Cannot fail.
			    برای (i = 0; i < ucs2len; ++i)
				dst += utf_char2bytes(ucs2buf[i], dst);
			}
			وگرنه
			{
			    BOOL	bad = FALSE;
			    صحیح		dstlen;

			    // From UCS-2 to "enc_codepage".  If the
			    // conversion uses the default character "?",
			    // the data doesn't fit in this encoding.
			    dstlen = WideCharToMultiByte(enc_codepage, 0,
				    (LPCWSTR)ucs2buf, ucs2len,
				    (LPSTR)dst, (صحیح)(src - dst),
				    replstr, &bad);
			    اگر (bad)
				found_bad = TRUE;
			    وگرنه
				dst += dstlen;
			}
		    }

		    اگر (found_bad)
		    {
			// Deal with bytes we can't convert.
			اگر (can_retry)
			    برو rewind_retry;
			اگر (conv_error == 0)
			    conv_error = readfile_linenr(linecnt, ptr, dst);
			اگر (bad_char_behavior != BAD_DROP)
			{
			    اگر (bad_char_behavior == BAD_KEEP)
			    {
				mch_memmove(dst, src, bytelen);
				dst += bytelen;
			    }
			    وگرنه
				*dst++ = bad_char_behavior;
			}
		    }

		    src += bytelen;
		    size -= bytelen;
		}

		اگر (size > 0)
		{
		    // An incomplete byte sequence remaining.
		    mch_memmove(conv_rest, src, size);
		    conv_restlen = size;
		}

		// The new size is equal to how much "dst" was advanced.
		size = (long)(dst - ptr);
	    }
	    وگرنه
#endif
#ifdef MACOS_CONVERT
	    اگر (fio_flags & FIO_MACROMAN)
	    {
		/*
		 * Conversion from Apple MacRoman char encoding to UTF-8 or
		 * latin1.  This is in os_mac_conv.c.
		 */
		اگر (macroman2enc(ptr, &size, real_size) == FAIL)
		    برو rewind_retry;
	    }
	    وگرنه
#endif
	    اگر (fio_flags != 0)
	    {
		صحیح	u8c;
		char_u	*dest;
		char_u	*tail = پوچ;

		/*
		 * "enc_utf8" set: Convert Unicode or Latin1 to UTF-8.
		 * "enc_utf8" not set: Convert Unicode to Latin1.
		 * Go from end to start through the buffer, because the number
		 * of bytes may increase.
		 * "dest" points to after where the UTF-8 bytes go, "p" points
		 * to after the next character to convert.
		 */
		dest = ptr + real_size;
		اگر (fio_flags == FIO_LATIN1 || fio_flags == FIO_UTF8)
		{
		    p = ptr + size;
		    اگر (fio_flags == FIO_UTF8)
		    {
			// Check for a trailing incomplete UTF-8 sequence
			tail = ptr + size - 1;
			وقتی (tail > ptr && (*tail & 0xc0) == 0x80)
			    --tail;
			اگر (tail + utf_byte2len(*tail) <= ptr + size)
			    tail = پوچ;
			وگرنه
			    p = tail;
		    }
		}
		وگرنه اگر (fio_flags & (FIO_UCS2 | FIO_UTF16))
		{
		    // Check for a trailing byte
		    p = ptr + (size & ~1);
		    اگر (size & 1)
			tail = p;
		    اگر ((fio_flags & FIO_UTF16) && p > ptr)
		    {
			// Check for a trailing leading word
			اگر (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			وگرنه
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			اگر (u8c >= 0xd800 && u8c <= 0xdbff)
			    tail = p;
			وگرنه
			    p += 2;
		    }
		}
		وگرنه //  FIO_UCS4
		{
		    // Check for trailing 1, 2 or 3 bytes
		    p = ptr + (size & ~3);
		    اگر (size & 3)
			tail = p;
		}

		// If there is a trailing incomplete sequence move it to
		// conv_rest[].
		اگر (tail != پوچ)
		{
		    conv_restlen = (صحیح)((ptr + size) - tail);
		    mch_memmove(conv_rest, (char_u *)tail, conv_restlen);
		    size -= conv_restlen;
		}


		وقتی (p > ptr)
		{
		    اگر (fio_flags & FIO_LATIN1)
			u8c = *--p;
		    وگرنه اگر (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    {
			اگر (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			وگرنه
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
				    برو rewind_retry;
				اگر (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				اگر (bad_char_behavior == BAD_DROP)
				    ادامه;
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
			    وگرنه
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
				    برو rewind_retry;
				اگر (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				اگر (bad_char_behavior == BAD_DROP)
				    ادامه;
				اگر (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    وگرنه اگر (fio_flags & FIO_UCS4)
		    {
			اگر (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (unsigned)*--p << 24;
			    u8c += (unsigned)*--p << 16;
			    u8c += (unsigned)*--p << 8;
			    u8c += *--p;
			}
			وگرنه	// big endian
			{
			    u8c = *--p;
			    u8c += (unsigned)*--p << 8;
			    u8c += (unsigned)*--p << 16;
			    u8c += (unsigned)*--p << 24;
			}
		    }
		    وگرنه    // UTF-8
		    {
			اگر (*--p < 0x80)
			    u8c = *p;
			وگرنه
			{
			    len = utf_head_off(ptr, p);
			    p -= len;
			    u8c = utf_ptr2char(p);
			    اگر (len == 0)
			    {
				// Not a valid UTF-8 character, retry with
				// another fenc when possible, otherwise just
				// report the error.
				اگر (can_retry)
				    برو rewind_retry;
				اگر (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				اگر (bad_char_behavior == BAD_DROP)
				    ادامه;
				اگر (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    اگر (enc_utf8)	// produce UTF-8
		    {
			dest -= utf_char2len(u8c);
			(تهی)utf_char2bytes(u8c, dest);
		    }
		    وگرنه		// produce Latin1
		    {
			--dest;
			اگر (u8c >= 0x100)
			{
			    // character doesn't fit in latin1, retry with
			    // another fenc when possible, otherwise just
			    // report the error.
			    اگر (can_retry)
				برو rewind_retry;
			    اگر (conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
			    اگر (bad_char_behavior == BAD_DROP)
				++dest;
			    وگرنه اگر (bad_char_behavior == BAD_KEEP)
				*dest = u8c;
			    وگرنه اگر (eap != پوچ && eap->bad_char != 0)
				*dest = bad_char_behavior;
			    وگرنه
				*dest = 0xBF;
			}
			وگرنه
			    *dest = u8c;
		    }
		}

		// move the linerest to before the converted characters
		سطر_start = dest - linerest;
		mch_memmove(سطر_start, buffer, (size_t)linerest);
		size = (long)((ptr + real_size) - dest);
		ptr = dest;
	    }
	    وگرنه اگر (enc_utf8 && !curbuf->b_p_bin)
	    {
		صحیح  incomplete_tail = FALSE;

		// Reading UTF-8: Check if the bytes are valid UTF-8.
		برای (p = ptr; ; )
		{
		    صحیح	 todo;
		    صحیح	 l;

		    // Skip ASCII bytes quickly using word-at-a-time check.
		    {
			char_u *ascii_end = ptr + size;
			وقتی (ascii_end - p >= (long)اندازه(long_u))
			{
			    long_u word;
			    memcpy(&word, p, اندازه(long_u));
			    اگر (word & NONASCII_MASK)
				بشکن;
			    p += اندازه(long_u);
			}
			وقتی (p < ascii_end && *p < 0x80)
			    ++p;
		    }

		    todo = (صحیح)((ptr + size) - p);
		    اگر (todo <= 0)
			بشکن;
		    اگر (*p >= 0x80)
		    {
			// A length of 1 means it's an illegal byte.  Accept
			// an incomplete character at the end though, the next
			// read() will get the next bytes, we'll check it
			// then.
			l = utf_ptr2len_len(p, todo);
			اگر (l > todo && !incomplete_tail)
			{
			    // Avoid retrying with a different encoding when
			    // a truncated file is more likely, or attempting
			    // to read the rest of an incomplete sequence when
			    // we have already done so.
			    اگر (p > ptr || filesize > 0)
				incomplete_tail = TRUE;
			    // Incomplete byte sequence, move it to conv_rest[]
			    // and try to read the rest of it, unless we've
			    // already done so.
			    اگر (p > ptr)
			    {
				conv_restlen = todo;
				mch_memmove(conv_rest, p, conv_restlen);
				size -= conv_restlen;
				بشکن;
			    }
			}
			اگر (l == 1 || l > todo)
			{
			    // Illegal byte.  If we can try another encoding
			    // do that, unless at EOF where a truncated
			    // file is more likely than a conversion error.
			    اگر (can_retry && !incomplete_tail)
				بشکن;
#ifdef USE_ICONV
			    // When we did a conversion report an error.
			    اگر (iconv_fd != (iconv_t)-1 && conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
#endif
			    // Remember the first linenr with an illegal byte
			    اگر (conv_error == 0 && illegal_byte == 0)
				illegal_byte = readfile_linenr(linecnt, ptr, p);

			    // Drop, keep or replace the bad byte.
			    اگر (bad_char_behavior == BAD_DROP)
			    {
				mch_memmove(p, p + 1, todo - 1);
				--size;
			    }
			    وگرنه
			    {
				اگر (bad_char_behavior != BAD_KEEP)
				    *p = bad_char_behavior;
				++p;
			    }
			}
			وگرنه
			    p += l;
		    }
		}
		اگر (p < ptr + size && !incomplete_tail)
		{
		    // Detected a UTF-8 error.
rewind_retry:
		    // Retry reading with another conversion.
#اگر defined(FEAT_EVAL) && defined(USE_ICONV)
		    اگر (*p_ccv != NUL && iconv_fd != (iconv_t)-1)
			// iconv() failed, try 'charconvert'
			did_iconv = TRUE;
		    وگرنه
#endif
			// use next item from 'fileencodings'
			advance_fenc = TRUE;
		    file_rewind = TRUE;
		    برو retry;
		}
	    }

	    // count the number of characters (after conversion!)
	    filesize += size;

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

		    برای (p = ptr; p < ptr + size; ++p)
		    {
			اگر (*p == NL)
			{
			    اگر (!try_unix
				    || (try_dos && p > ptr && p[-1] == CAR))
				fileformat = EOL_DOS;
			    وگرنه
				fileformat = EOL_UNIX;
			    بشکن;
			}
			وگرنه اگر (*p == CAR && try_mac)
			    try_mac++;
		    }

		    // Don't give in to EOL_UNIX if EOL_MAC is more likely
		    اگر (fileformat == EOL_UNIX && try_mac)
		    {
			// Need to reset the counters when retrying fenc.
			try_mac = 1;
			try_unix = 1;
			برای (; p >= ptr && *p != CAR; p--)
			    ;
			اگر (p >= ptr)
			{
			    برای (p = ptr; p < ptr + size; ++p)
			    {
				اگر (*p == NL)
				    try_unix++;
				وگرنه اگر (*p == CAR)
				    try_mac++;
			    }
			    اگر (try_mac > try_unix)
				fileformat = EOL_MAC;
			}
		    }
		    وگرنه اگر (fileformat == EOL_UNKNOWN && try_mac == 1)
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
		اگر (set_options)
		    set_fileformat(fileformat, OPT_LOCAL);
	    }
	}

	/*
	 * This loop is executed once for every character read.
	 * Keep it fast!
	 */
	اگر (fileformat == EOL_MAC)
	{
	    --ptr;
	    وقتی (++ptr, --size >= 0)
	    {
		// catch most common case first
		اگر ((c = *ptr) != NUL && c != CAR && c != NL)
		    ادامه;
		اگر (c == NUL)
		    *ptr = NL;	// NULs are replaced by newlines!
		وگرنه اگر (c == NL)
		    *ptr = CAR;	// NLs are replaced by CRs!
		وگرنه
		{
		    اگر (skip_شمار == 0)
		    {
			*ptr = NUL;	    // end of line
			len = (colnr_T) (ptr - سطر_start + 1);
			اگر (ml_append(lnum, سطر_start, len, newfile) == FAIL)
			{
			    error = TRUE;
			    بشکن;
			}
#ifdef FEAT_PERSISTENT_UNDO
			اگر (read_undo_file)
			    sha256_update(&sha_ctx, سطر_start, len);
#endif
			++lnum;
			اگر (--read_شمار == 0)
			{
			    error = TRUE;	// break loop
			    سطر_start = ptr;	// nothing left to write
			    بشکن;
			}
		    }
		    وگرنه
			--skip_شمار;
		    سطر_start = ptr + 1;
		}
	    }
	}
	وگرنه
	{
	    // Use memchr() for SIMD-optimized newline scanning instead
	    // of scanning each byte individually.
	    char_u *end = ptr + size;

	    وقتی (ptr < end)
	    {
		char_u *nl = (char_u *)memchr(ptr, NL, end - ptr);
		char_u *nul_scan;

		اگر (nl == پوچ)
		{
		    // No more newlines in buffer.
		    // Replace any NUL bytes with NL in remaining data.
		    وقتی ((nul_scan = (char_u *)memchr(ptr, NUL,
						      end - ptr)) != پوچ)
		    {
			*nul_scan = NL;
			ptr = nul_scan + 1;
		    }
		    ptr = end;
		    بشکن;
		}

		// Replace NUL bytes with NL before the newline.
		{
		    char_u *scan = ptr;
		    وقتی ((nul_scan = (char_u *)memchr(scan, NUL,
						       nl - scan)) != پوچ)
		    {
			*nul_scan = NL;
			scan = nul_scan + 1;
		    }
		}

		// Process the newline.
		ptr = nl;
		اگر (skip_شمار == 0)
		{
		    *ptr = NUL;		// end of line
		    len = (colnr_T)(ptr - سطر_start + 1);
		    اگر (fileformat == EOL_DOS)
		    {
			اگر (ptr > سطر_start && ptr[-1] == CAR)
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
			وگرنه اگر (ff_error != EOL_DOS)
			{
			    اگر (   try_unix
				&& !read_stdin
				&& (read_buffer
				    || vim_lseek(fd, (off_T)0L, SEEK_SET)
								      == 0))
			    {
				fileformat = EOL_UNIX;
				اگر (set_options)
				    set_fileformat(EOL_UNIX, OPT_LOCAL);
				file_rewind = TRUE;
				keep_fileformat = TRUE;
				برو retry;
			    }
			    ff_error = EOL_DOS;
			}
		    }
		    اگر (ml_append(lnum, سطر_start, len, newfile) == FAIL)
		    {
			error = TRUE;
			بشکن;
		    }
#ifdef FEAT_PERSISTENT_UNDO
		    اگر (read_undo_file)
			sha256_update(&sha_ctx, سطر_start, len);
#endif
		    ++lnum;
		    اگر (--read_شمار == 0)
		    {
			error = TRUE;	    // break loop
			سطر_start = ptr;   // nothing left to write
			بشکن;
		    }
		}
		وگرنه
		    --skip_شمار;
		سطر_start = ptr + 1;
		++ptr;
	    }
	    size = -1;
	}
	linerest = (long)(ptr - سطر_start);
	ui_breakcheck();
    }

failed:
    // not an error, max. number of lines reached
    اگر (error && read_شمار == 0)
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
	اگر (set_options)
	    curbuf->b_p_eof = TRUE;
    }

    // If we get EOF in the middle of a line, note the fact by resetting
    // 'endofline' and add the line normally.
    اگر (!error
	    && !got_int
	    && linerest != 0)
    {
	// remember for when writing
	اگر (set_options)
	    curbuf->b_p_eol = FALSE;
	*ptr = NUL;
	len = (colnr_T)(ptr - سطر_start + 1);
	اگر (ml_append(lnum, سطر_start, len, newfile) == FAIL)
	    error = TRUE;
	وگرنه
	{
#ifdef FEAT_PERSISTENT_UNDO
	    اگر (read_undo_file)
		sha256_update(&sha_ctx, سطر_start, len);
#endif
	    read_no_eol_lnum = ++lnum;
	}
    }

    اگر (set_options)
	save_file_ff(curbuf);		// remember the current file format

#ifdef FEAT_CRYPT
    اگر (curbuf->b_cryptstate != پوچ)
    {
	crypt_free_state(curbuf->b_cryptstate);
	curbuf->b_cryptstate = پوچ;
    }
    اگر (cryptkey != پوچ && cryptkey != curbuf->b_p_کلید)
	crypt_free_کلید(cryptkey);
    // Don't set cryptkey to NULL, it's used below as a flag that
    // encryption was used.
#endif

    // If editing a new file: set 'fenc' for the current buffer.
    // Also for ":read ++edit file".
    اگر (set_options)
	set_string_option_direct((char_u *)"fenc", -1, fenc,
						       OPT_FREE|OPT_LOCAL, 0);
    اگر (fenc_alloced)
	vim_free(fenc);
#ifdef USE_ICONV
    اگر (iconv_fd != (iconv_t)-1)
	iconv_close(iconv_fd);
#endif

    اگر (!read_buffer && !read_stdin)
	close(fd);				// errors are ignored
#ifdef HAVE_FD_CLOEXEC
    وگرنه
    {
	صحیح fdflags = fcntl(fd, F_GETFD);

	اگر (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (تهی)fcntl(fd, F_SETFD, fdflags | FD_CLOEXEC);
    }
#endif
    vim_free(buffer);

#ifdef HAVE_DUP
    اگر (read_stdin)
    {
	// Use stderr for stdin, makes shell commands work.
	close(0);
	vim_ignored = dup(2);
    }
#endif

    اگر (tmpname != پوچ)
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
	اگر (newfile && wasempty && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	{
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 0;
#endif
	    ml_delete(curbuf->b_ml.ml_سطر_شمار);
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 1;
#endif
	    --linecnt;
	}
	linecnt = curbuf->b_ml.ml_سطر_شمار - linecnt;
	اگر (filesize == 0)
	    linecnt = 0;
	اگر (newfile || read_buffer)
	{
	    redraw_curbuf_later(UPD_NOT_VALID);
#ifdef FEAT_DIFF
	    // After reading the text into the buffer the diff info needs to
	    // be updated.
	    diff_invalidate(curbuf);
#endif
#ifdef FEAT_FOLDING
	    // All folds in the window are invalid now.  Mark them for update
	    // before triggering autocommands.
	    foldUpdateAll(curwin);
#endif
	}
	وگرنه اگر (linecnt)		// appended at least one line
	    appended_سطرها_mark(from, linecnt);

#ifndef ALWAYS_USE_GUI
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
#endif

	اگر (got_int)
	{
	    اگر (!(flags & READ_DUMMY))
	    {
		filemess(curbuf, sfname, (char_u *)_(e_interrupted), 0);
		اگر (newfile)
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    }
	    پیام_scroll = پیام_save;
#ifdef FEAT_VIMINFO
	    check_marks_read();
#endif
	    retval = OK;	// an interrupt isn't really an error
	    برو theend;
	}

	اگر (!filtering && !(flags & READ_DUMMY))
	{
	    صحیح	buflen;

	    پیام_add_fname(curbuf, sfname);   // fname in IObuff with quotes
	    c = FALSE;

	    buflen = (صحیح)STRLEN(IObuff);
#ifdef UNIX
	    اگر (S_ISFIFO(perm))			    // fifo
	    {
		buflen += vim_snprintf((نویسه *)IObuff + buflen, IOSIZE - buflen,
			_("[fifo]"));
		c = TRUE;
	    }
	    اگر (S_ISSOCK(perm))			    // or socket
	    {
		buflen += vim_snprintf((نویسه *)IObuff + buflen, IOSIZE - buflen,
			_("[socket]"));
		c = TRUE;
	    }
# ifdef OPEN_CHR_FILES
	    اگر (S_ISCHR(perm))			    // or character special
	    {
		buflen += vim_snprintf((نویسه *)IObuff + buflen, IOSIZE - buflen,
			_("[character special]"));
		c = TRUE;
	    }
# endif
#endif
	    اگر (curbuf->b_p_ro)
	    {
		buflen += vim_snprintf((نویسه *)IObuff + buflen, IOSIZE - buflen,
			"%s", shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
		c = TRUE;
	    }
	    اگر (read_no_eol_lnum)
	    {
		پیام_add_eol();
		c = TRUE;
	    }
	    اگر (ff_error == EOL_DOS)
	    {
		buflen += vim_snprintf((نویسه *)IObuff + buflen, IOSIZE - buflen,
			_("[CR missing]"));
		c = TRUE;
	    }
	    اگر (split)
	    {
		buflen += vim_snprintf((نویسه *)IObuff + buflen, IOSIZE - buflen,
			_("[long lines split]"));
		c = TRUE;
	    }
	    اگر (notconverted)
	    {
		buflen += vim_snprintf((نویسه *)IObuff + buflen, IOSIZE - buflen,
			_("[NOT converted]"));
		c = TRUE;
	    }
	    وگرنه اگر (converted)
	    {
		buflen += vim_snprintf((نویسه *)IObuff + buflen, IOSIZE - buflen,
			_("[converted]"));
		c = TRUE;
	    }
#ifdef FEAT_CRYPT
	    اگر (cryptkey != پوچ)
	    {
		crypt_append_پیام(curbuf);
		c = TRUE;
	    }
#endif
	    اگر (conv_error != 0)
	    {
		vim_snprintf((نویسه *)IObuff + buflen, IOSIZE - buflen,
			_("[CONVERSION ERROR in line %ld]"), (long)conv_error);
		c = TRUE;
	    }
	    وگرنه اگر (illegal_byte > 0)
	    {
		vim_snprintf((نویسه *)IObuff + buflen, IOSIZE - buflen,
			_("[ILLEGAL BYTE in line %ld]"), (long)illegal_byte);
		c = TRUE;
	    }
	    وگرنه اگر (error)
	    {
		vim_snprintf((نویسه *)IObuff + buflen, IOSIZE - buflen,
			_("[READ ERRORS]"));
		c = TRUE;
	    }
	    اگر (پیام_add_fileformat(fileformat))
		c = TRUE;
#ifdef FEAT_CRYPT
	    اگر (cryptkey != پوچ)
		پیام_add_سطرها(c, (long)linecnt, filesize
			 - crypt_get_header_len(crypt_get_method_nr(curbuf)));
	    وگرنه
#endif
		پیام_add_سطرها(c, (long)linecnt, filesize);

	    VIM_CLEAR(keep_پیام);
	    پیام_scrolled_ign = TRUE;
#ifdef ALWAYS_USE_GUI
	    // Don't show the message when reading stdin, it would end up in a
	    // message box (which might be shown when exiting!)
	    اگر (read_stdin || read_buffer)
		p = پیام_may_trunc(FALSE, IObuff);
	    وگرنه
#endif
	    {
		اگر (پیام_col > 0)
		    پیام_putchar('\r');  // overwrite previous message
		p = (char_u *)پیام_trunc_attr((نویسه *)IObuff, FALSE, 0);
	    }
	    اگر (read_stdin || read_buffer || restart_edit != 0
		    || (پیام_scrolled != 0 && !need_wait_return))
		// Need to repeat the message after redrawing when:
		// - When reading from stdin (the screen will be cleared next).
		// - When restart_edit is set (otherwise there will be a delay
		//   before redrawing).
		// - When the screen was scrolled but there is no wait-return
		//   prompt.
		set_keep_پیام(p, 0);
	    پیام_scrolled_ign = FALSE;
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
	اگر (exmode_active)
	    curwin->w_cursor.lnum = from + linecnt;
	وگرنه
	    curwin->w_cursor.lnum = from + 1;
	check_cursor_lnum();
	beginline(BL_WHITE | BL_FIX);	    // on first non-blank

	اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set '[ and '] marks to the newly read lines.
	    curbuf->b_op_start.lnum = from + 1;
	    curbuf->b_op_start.col = 0;
	    curbuf->b_op_end.lnum = from + linecnt;
	    curbuf->b_op_end.col = 0;
	}

#ifdef MSWIN
	/*
	 * Work around a weird problem: When a file has two links (only
	 * possible on NTFS) and we write through one link, then stat() it
	 * through the other link, the timestamp information may be wrong.
	 * It's correct again after reading the file, thus reset the timestamp
	 * here.
	 */
	اگر (newfile && !read_stdin && !read_buffer
					 && mch_stat((نویسه *)fname, &st) >= 0)
	{
	    buf_store_time(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
	}
#endif
    }
    پیام_scroll = پیام_save;

#ifdef FEAT_VIMINFO
    /*
     * Get the marks before executing autocommands, so they can be used there.
     */
    check_marks_read();
#endif

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
	u_find_first_changed();

#ifdef FEAT_PERSISTENT_UNDO
    /*
     * When opening a new file locate undo info and read it.
     */
    اگر (read_undo_file)
    {
	char_u	hash[UNDO_HASH_SIZE];

	sha256_finish(&sha_ctx, hash);
	u_read_undo(پوچ, hash, fname);
    }
#endif

    اگر (!read_stdin && !read_fifo && (!read_buffer || sfname != پوچ))
    {
	صحیح m = پیام_scroll;
	صحیح n = پیام_scrolled;

	// Save the fileformat now, otherwise the buffer will be considered
	// modified if the format/encoding was automatically detected.
	اگر (set_options)
	    save_file_ff(curbuf);

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	پیام_scroll = TRUE;
	اگر (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPOST, پوچ, sfname,
							  FALSE, curbuf, eap);
	وگرنه اگر (newfile || (read_buffer && sfname != پوچ))
	{
	    apply_autocmds_exarg(EVENT_BUFREADPOST, پوچ, sfname,
							  FALSE, curbuf, eap);
	    اگر (!curbuf->b_au_did_filetype && *curbuf->b_p_ft != NUL)
		/*
		 * EVENT_FILETYPE was not triggered but the buffer already has a
		 * filetype. Trigger EVENT_FILETYPE using the existing filetype.
		 */
		apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
			TRUE, curbuf);
	}
	وگرنه
	    apply_autocmds_exarg(EVENT_FILEREADPOST, sfname, sfname,
							    FALSE, پوچ, eap);
	اگر (پیام_scrolled == n)
	    پیام_scroll = m;
#ifdef FEAT_EVAL
	اگر (aborting())	    // autocmds may abort script processing
	    برو theend;
#endif
    }

    اگر (!(recoverymode && error))
	retval = OK;

theend:
    اگر (curbuf->b_ml.ml_mfp != پوچ
		       && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	// OK to sync the swap file now
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    بازگشت retval;
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
    بازگشت (STRNCMP(fname, "/dev/fd/", 8) == 0
	    && VIM_ISDIGIT(fname[8])
	    && *skipdigits(fname + 9) == NUL
	    && (fname[9] != NUL
		|| (fname[8] != '0' && fname[8] != '1' && fname[8] != '2')));
}
#endif

/*
 * From the current line count and characters read after that, estimate the
 * line number where we are now.
 * Used for error messages that include a line number.
 */
    ایستا linenr_T
readfile_linenr(
    linenr_T	linecnt,	// line count before reading more bytes
    char_u	*p,		// start of more bytes read
    char_u	*endp)		// end of more bytes read
{
    char_u	*s;
    linenr_T	lnum;

    lnum = curbuf->b_ml.ml_سطر_شمار - linecnt + 1;
    برای (s = p; s < endp; ++s)
	اگر (*s == '\n')
	    ++lnum;
    بازگشت lnum;
}

/*
 * Fill "*eap" to force the 'fileencoding', 'fileformat' and 'binary' to be
 * equal to the buffer "buf".  Used for calling readfile().
 * Returns OK or FAIL.
 */
    صحیح
prep_exarg(exarg_T *eap, buf_T *buf)
{
    eap->cmd = alloc(15 + (unsigned)STRLEN(buf->b_p_fenc));
    اگر (eap->cmd == پوچ)
	بازگشت FAIL;

    sprintf((نویسه *)eap->cmd, "e ++enc=%s", buf->b_p_fenc);
    eap->force_enc = 8;
    eap->bad_char = buf->b_bad_char;
    eap->force_ff = *buf->b_p_ff;

    eap->force_bin = buf->b_p_bin ? FORCE_BIN : FORCE_NOBIN;
    eap->read_edit = FALSE;
    eap->forceit = FALSE;
    بازگشت OK;
}

/*
 * Set default or forced 'fileformat' and 'binary'.
 */
    تهی
set_file_options(صحیح set_options, exarg_T *eap)
{
    // set default 'fileformat'
    اگر (set_options)
    {
	اگر (eap != پوچ && eap->force_ff != 0)
	    set_fileformat(get_fileformat_force(curbuf, eap), OPT_LOCAL);
	وگرنه اگر (*p_ffs != NUL)
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    // set or reset 'binary'
    اگر (eap != پوچ && eap->force_bin != 0)
    {
	صحیح	oldval = curbuf->b_p_bin;

	curbuf->b_p_bin = (eap->force_bin == FORCE_BIN);
	set_options_bin(oldval, curbuf->b_p_bin, OPT_LOCAL);
    }
}

/*
 * Set forced 'fileencoding'.
 */
    تهی
set_forced_fenc(exarg_T *eap)
{
    اگر (eap->force_enc == 0)
	بازگشت;

    char_u *fenc = enc_canonize(eap->cmd + eap->force_enc);

    اگر (fenc != پوچ)
	set_string_option_direct((char_u *)"fenc", -1,
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
    ایستا char_u *
next_fenc(char_u **pp, صحیح *alloced)
{
    char_u	*p;
    char_u	*r;

    *alloced = FALSE;
    اگر (**pp == NUL)
    {
	*pp = پوچ;
	بازگشت (char_u *)"";
    }
    p = vim_strchr(*pp, ',');
    اگر (p == پوچ)
    {
	r = enc_canonize(*pp);
	*pp += STRLEN(*pp);
    }
    وگرنه
    {
	r = vim_strnsave(*pp, p - *pp);
	*pp = p + 1;
	اگر (r != پوچ)
	{
	    p = enc_canonize(r);
	    vim_free(r);
	    r = p;
	}
    }
    اگر (r != پوچ)
	*alloced = TRUE;
    وگرنه
    {
	// out of memory
	r = (char_u *)"";
	*pp = پوچ;
    }
    بازگشت r;
}

#ifdef FEAT_EVAL
/*
 * Convert a file with the 'charconvert' expression.
 * This closes the file which is to be read, converts it and opens the
 * resulting file for reading.
 * Returns name of the resulting converted file (the caller should delete it
 * after reading it).
 * Returns NULL if the conversion failed ("*fdp" is not set) .
 */
    ایستا char_u *
readfile_charconvert(
    char_u	*fname,		// name of input file
    char_u	*fenc,		// converted from
    صحیح		*fdp)		// in/out: file descriptor of file
{
    char_u	*tmpname;
    نویسه	*errmsg = پوچ;

    tmpname = vim_tempname('r', FALSE);
    اگر (tmpname == پوچ)
	errmsg = _("Can't find temp file for conversion");
    وگرنه
    {
	close(*fdp);		// close the input file, ignore errors
	*fdp = -1;
	اگر (eval_charconvert(fenc, enc_utf8 ? (char_u *)"utf-8" : p_enc,
						      fname, tmpname) == FAIL)
	    errmsg = _("Conversion with 'charconvert' failed");
	اگر (errmsg == پوچ && (*fdp = mch_open((نویسه *)tmpname,
						  O_RDONLY | O_EXTRA, 0)) < 0)
	    errmsg = _("can't read output of 'charconvert'");
    }

    اگر (errmsg != پوچ)
    {
	// Don't use emsg(), it breaks mappings, the retry with
	// another type of conversion might still work.
	پیام(errmsg);
	اگر (tmpname != پوچ)
	{
	    mch_remove(tmpname);	// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    // If the input file is closed, open it (caller should check for error).
    اگر (*fdp < 0)
	*fdp = mch_open((نویسه *)fname, O_RDONLY | O_EXTRA, 0);

    بازگشت tmpname;
}
#endif

#اگر defined(FEAT_CRYPT)
/*
 * Check for magic number used for encryption.  Applies to the current buffer.
 * If found, the magic number is removed from ptr[*sizep] and *sizep and
 * *filesizep are updated.
 * Return the (new) encryption key, NULL for no encryption.
 */
    ایستا char_u *
check_for_cryptkey(
    char_u	*cryptkey,	// previous encryption key or NULL
    char_u	*ptr,		// pointer to read bytes
    long	*sizep,		// length of read bytes
    off_T	*filesizep,	// nr of bytes used from file
    صحیح		newfile,	// editing a new buffer
    char_u	*fname,		// file name to display
    صحیح		*did_ask)	// flag: whether already asked for key
{
    صحیح method = crypt_method_nr_from_magic((نویسه *)ptr, *sizep);
    صحیح b_p_ro = curbuf->b_p_ro;

    اگر (method >= 0)
    {
	// Mark the buffer as read-only until the decryption has taken place.
	// Avoids accidentally overwriting the file with garbage.
	curbuf->b_p_ro = TRUE;

	// Set the cryptmethod local to the buffer.
	crypt_set_cm_option(curbuf, method);
	اگر (cryptkey == پوچ && !*did_ask)
	{
	    اگر (*curbuf->b_p_کلید)
	    {
		cryptkey = curbuf->b_p_کلید;
		crypt_check_swapfile_curbuf();
	    }
	    وگرنه
	    {
		// When newfile is TRUE, store the typed key in the 'key'
		// option and don't free it.  bf needs hash of the key saved.
		// Don't ask for the key again when first time Enter was hit.
		// Happens when retrying to detect encoding.
		smsg(_(need_کلید_پیام), fname);
		پیام_scroll = TRUE;
		crypt_check_method(method);
		cryptkey = crypt_get_کلید(newfile, FALSE);
		*did_ask = TRUE;

		// check if empty key entered
		اگر (cryptkey != پوچ && *cryptkey == NUL)
		{
		    اگر (cryptkey != curbuf->b_p_کلید)
			vim_free(cryptkey);
		    cryptkey = پوچ;
		}
	    }
	}

	اگر (cryptkey != پوچ)
	{
	    صحیح header_len;

	    header_len = crypt_get_header_len(method);
	    اگر (*sizep < header_len)
	    {
		// invalid header, buffer can't be encrypted
		اگر (cryptkey != curbuf->b_p_کلید)
		    vim_free(cryptkey);
		بازگشت پوچ;
	    }

	    curbuf->b_cryptstate = crypt_create_from_header(
							method, cryptkey, ptr);
	    crypt_set_cm_option(curbuf, method);

	    // Remove cryptmethod specific header from the text.
	    *filesizep += header_len;
	    *sizep -= header_len;
	    mch_memmove(ptr, ptr + header_len, (size_t)*sizep);

	    // Restore the read-only flag.
	    curbuf->b_p_ro = b_p_ro;
	}
    }
    // When starting to edit a new file which does not have encryption, clear
    // the 'key' option, except when starting up (called with -x argument)
    وگرنه اگر (newfile && *curbuf->b_p_کلید != NUL && !starting)
	set_option_مقدار_give_err((char_u *)"key", 0L, (char_u *)"", OPT_LOCAL);

    بازگشت cryptkey;
}
#endif  // FEAT_CRYPT

/*
 * Return TRUE if a file appears to be read-only from the file permissions.
 */
    صحیح
check_file_readonly(
    char_u	*fname,		// full path to file
    صحیح		perm UNUSED)	// known permissions on file
{
#ifndef USE_MCH_ACCESS
    صحیح	    fd = 0;
#endif

    بازگشت (
#ifdef USE_MCH_ACCESS
# ifdef UNIX
	(perm & 0222) == 0 ||
# endif
	mch_access((نویسه *)fname, W_OK)
#وگرنه
	(fd = mch_open((نویسه *)fname, O_RDWR | O_EXTRA, 0)) < 0
					? TRUE : (close(fd), FALSE)
#endif
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
    بازگشت r;
}
#endif

/*
 * Set the name of the current buffer.  Use when the buffer doesn't have a
 * name and a ":r" or ":w" command with a file name is used.
 */
    صحیح
set_rw_fname(char_u *fname, char_u *sfname)
{
    buf_T	*buf = curbuf;

    // It's like the unnamed buffer is deleted....
    اگر (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFDELETE, پوچ, پوچ, FALSE, curbuf);
    apply_autocmds(EVENT_BUFWIPEOUT, پوچ, پوچ, FALSE, curbuf);
#ifdef FEAT_EVAL
    اگر (aborting())	    // autocmds may abort script processing
	بازگشت FAIL;
#endif
    اگر (curbuf != buf)
    {
	// We are in another buffer now, don't do the renaming.
	emsg(_(e_autocommands_changed_buffer_or_buffer_name));
	بازگشت FAIL;
    }

    اگر (setfname(curbuf, fname, sfname, FALSE) == OK)
	curbuf->b_flags |= BF_NOTEDITED;

    // ....and a new named one is created
    apply_autocmds(EVENT_BUFNEW, پوچ, پوچ, FALSE, curbuf);
    اگر (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFADD, پوچ, پوچ, FALSE, curbuf);
#ifdef FEAT_EVAL
    اگر (aborting())	    // autocmds may abort script processing
	بازگشت FAIL;
#endif

    // Do filetype detection now if 'filetype' is empty.
    اگر (*curbuf->b_p_ft == NUL)
    {
	اگر (au_has_group((char_u *)"filetypedetect"))
	    (تهی)do_doautocmd((char_u *)"filetypedetect BufRead", FALSE, پوچ);
	do_modelines(0);
    }

    بازگشت OK;
}

/*
 * Put file name into IObuff with quotes.
 */
    تهی
پیام_add_fname(buf_T *buf, char_u *fname)
{
    size_t  IObufflen = 0;

    اگر (fname == پوچ)
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
پیام_add_fileformat(صحیح eol_type)
{
#ifndef USE_CRNL
    اگر (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[dos]") : _("[dos format]"));
	بازگشت TRUE;
    }
#endif
    اگر (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[mac]") : _("[mac format]"));
	بازگشت TRUE;
    }
#ifdef USE_CRNL
    اگر (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[unix]") : _("[unix format]"));
	بازگشت TRUE;
    }
#endif
    بازگشت FALSE;
}

/*
 * Append line and character count to IObuff.
 */
    تهی
پیام_add_سطرها(
    صحیح	    درج_space,
    long    lnum,
    off_T   nchars)
{
    صحیح  len = (صحیح)STRLEN(IObuff);

    اگر (shortmess(SHM_سطرها))
	vim_snprintf((نویسه *)IObuff + len, IOSIZE - (size_t)len,
		// l10n: L as in line, B as in byte
		_("%s%ldL, %lldB"), درج_space ? " " : "", lnum, (varnumber_T)nchars);
    وگرنه
    {
	len += vim_snprintf((نویسه *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%s%ld line, ", "%s%ld lines, ", lnum), درج_space ? " " : "", lnum);
	vim_snprintf((نویسه *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%lld byte", "%lld bytes", nchars), (varnumber_T)nchars);
    }
}

/*
 * Append message for missing line separator to IObuff.
 */
    تهی
پیام_add_eol(تهی)
{
    STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]") : _("[Incomplete last line]"));
}

    صحیح
time_differs(stat_T *st, long mtime, long mtime_ns UNUSED)
{
    بازگشت
#ifdef ST_MTIM_NSEC
	(long)st->ST_MTIM_NSEC != mtime_ns ||
#endif
#اگر defined(__linux__) || defined(MSWIN)
	// On a FAT filesystem, esp. under Linux, there are only 5 bits to store
	// the seconds.  Since the roundoff is done when flushing the inode, the
	// time may change unexpectedly by one second!!!
	(long)st->st_mtime - mtime > 1 || mtime - (long)st->st_mtime > 1
#وگرنه
	(long)st->st_mtime != mtime
#endif
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
    وگرنه
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
	بازگشت enc_unicode != 0;
    }

    // Encodings differ.  However, conversion is not needed when 'enc' is any
    // Unicode encoding and the file is UTF-8.
    بازگشت !(enc_utf8 && fenc_flags == FIO_UTF8);
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
		بازگشت FIO_UCS2 | FIO_ENDIAN_L;
	    بازگشت FIO_UCS2;
	}
	اگر (prop & ENC_4BYTE)
	{
	    اگر (prop & ENC_ENDIAN_L)
		بازگشت FIO_UCS4 | FIO_ENDIAN_L;
	    بازگشت FIO_UCS4;
	}
	اگر (prop & ENC_2WORD)
	{
	    اگر (prop & ENC_ENDIAN_L)
		بازگشت FIO_UTF16 | FIO_ENDIAN_L;
	    بازگشت FIO_UTF16;
	}
	بازگشت FIO_UTF8;
    }
    اگر (prop & ENC_LATIN1)
	بازگشت FIO_LATIN1;
    // must be ENC_DBCS, requires iconv()
    بازگشت 0;
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
	بازگشت 0;

    cp = encname2codepage(ptr);
    اگر (cp == 0)
    {
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	اگر (STRCMP(ptr, "utf-8") == 0)
	    cp = CP_UTF8;
	وگرنه
# endif
	    بازگشت 0;
    }
    بازگشت FIO_PUT_CP(cp) | FIO_CODEPAGE;
}
#endif

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
	بازگشت FIO_MACROMAN;
    بازگشت 0;
}
#endif

/*
 * Check for a Unicode BOM (Byte Order Mark) at the start of p[size].
 * "size" must be at least 2.
 * Return the name of the encoding and set "*lenp" to the length.
 * Returns NULL when no BOM found.
 */
    ایستا char_u *
check_for_bom(
    char_u	*p,
    long	size,
    صحیح		*lenp,
    صحیح		flags)
{
    نویسه	*name = پوچ;
    صحیح		len = 2;

    اگر (p[0] == 0xef && p[1] == 0xbb && size >= 3 && p[2] == 0xbf
	    && (flags == FIO_ALL || flags == FIO_UTF8 || flags == 0))
    {
	name = "utf-8";		// EF BB BF
	len = 3;
    }
    وگرنه اگر (p[0] == 0xff && p[1] == 0xfe)
    {
	اگر (size >= 4 && p[2] == 0 && p[3] == 0
	    && (flags == FIO_ALL || flags == (FIO_UCS4 | FIO_ENDIAN_L)))
	{
	    name = "ucs-4le";	// FF FE 00 00
	    len = 4;
	}
	وگرنه اگر (flags == (FIO_UCS2 | FIO_ENDIAN_L))
	    name = "ucs-2le";	// FF FE
	وگرنه اگر (flags == FIO_ALL || flags == (FIO_UTF16 | FIO_ENDIAN_L))
	    // utf-16le is preferred, it also works for ucs-2le text
	    name = "utf-16le";	// FF FE
    }
    وگرنه اگر (p[0] == 0xfe && p[1] == 0xff
	    && (flags == FIO_ALL || flags == FIO_UCS2 || flags == FIO_UTF16))
    {
	// Default to utf-16, it works also for ucs-2 text.
	اگر (flags == FIO_UCS2)
	    name = "ucs-2";	// FE FF
	وگرنه
	    name = "utf-16";	// FE FF
    }
    وگرنه اگر (size >= 4 && p[0] == 0 && p[1] == 0 && p[2] == 0xfe
	    && p[3] == 0xff && (flags == FIO_ALL || flags == FIO_UCS4))
    {
	name = "ucs-4";		// 00 00 FE FF
	len = 4;
    }

    *lenp = len;
    بازگشت (char_u *)name;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns "full_path" or pointer into "full_path" if shortened.
 */
    char_u *
shorten_fname1(char_u *full_path)
{
    char_u	*dirname;
    char_u	*p = full_path;

    dirname = alloc(MAXPATHL);
    اگر (dirname == پوچ)
	بازگشت full_path;
    اگر (mch_dirname(dirname, MAXPATHL) == OK)
    {
	p = shorten_fname(full_path, dirname);
	اگر (p == پوچ || *p == NUL)
	    p = full_path;
    }
    vim_free(dirname);
    بازگشت p;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns NULL if not shorter name possible, pointer into "full_path"
 * otherwise.
 */
    char_u *
shorten_fname(char_u *full_path, char_u *dir_name)
{
    صحیح		len;
    char_u	*p;

    اگر (full_path == پوچ)
	بازگشت پوچ;
    len = (صحیح)STRLEN(dir_name);
    اگر (fnamencmp(dir_name, full_path, len) == 0)
    {
	p = full_path + len;
#اگر defined(MSWIN)
	/*
	 * MS-Windows: when a file is in the root directory, dir_name will end
	 * in a slash, since C: by itself does not define a specific dir. In
	 * this case p may already be correct. <negri>
	 */
	اگر (!((len > 2) && (*(p - 2) == ':')))
#endif
	{
	    اگر (vim_ispathsep(*p))
		انجامبده
		    ++p;
		وقتی (vim_ispathsep_nocolon(*p));
#ifndef VMS   // the path separator is always part of the path
	    وگرنه
		p = پوچ;
#endif
	}
    }
#اگر defined(MSWIN)
    /*
     * When using a file in the current drive, remove the drive name:
     * "A:\dir\file" -> "\dir\file".  This helps when moving a session file on
     * a floppy from "A:\dir" to "B:\dir".
     */
    وگرنه اگر (len > 3
	    && TOUPPER_LOC(full_path[0]) == TOUPPER_LOC(dir_name[0])
	    && full_path[1] == ':'
	    && vim_ispathsep(full_path[2]))
	p = full_path + 2;
#endif
    وگرنه
	p = پوچ;
    بازگشت p;
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
    تهی
shorten_buf_fname(buf_T *buf, char_u *dirname, صحیح force)
{
    char_u	*p;

    اگر (buf->b_fname != پوچ
	    && !bt_nofilename(buf)
	    && !path_with_url(buf->b_fname)
	    && (force
		|| buf->b_sfname == پوچ
		|| mch_isFullName(buf->b_sfname)))
    {
	اگر (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	p = shorten_fname(buf->b_ffname, dirname);
	اگر (p != پوچ)
	{
	    buf->b_sfname = vim_strsave(p);
	    buf->b_fname = buf->b_sfname;
	}
	اگر (p == پوچ || buf->b_fname == پوچ)
	    buf->b_fname = buf->b_ffname;
    }
}

/*
 * Shorten filenames for all buffers.
 */
    تهی
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
    status_redraw_all();
    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    popup_update_preview_title();
#endif
}

#اگر (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
/*
 * Shorten all filenames in "fnames[count]" by current directory.
 */
    تهی
shorten_filenames(char_u **fnames, صحیح شمار)
{
    صحیح		i;
    char_u	dirname[MAXPATHL];
    char_u	*p;

    اگر (fnames == پوچ || شمار < 1)
	بازگشت;
    mch_dirname(dirname, اندازه(dirname));
    برای (i = 0; i < شمار; ++i)
    {
	اگر ((p = shorten_fname(fnames[i], dirname)) != پوچ)
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
#endif

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
    بازگشت buf_modname((curbuf->b_p_sn || curbuf->b_shortname),
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
    اگر (fname == پوچ || *fname == NUL)
    {
	retval = alloc(MAXPATHL + extlen + 3);
	اگر (retval == پوچ)
	    بازگشت پوچ;
	اگر (mch_dirname(retval, MAXPATHL) == FAIL ||
				     (fnamelen = (صحیح)STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    بازگشت پوچ;
	}
	اگر (!after_pathsep(retval, retval + fnamelen))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
	prepend_dot = FALSE;	    // nothing to prepend a dot to
    }
    وگرنه
    {
	fnamelen = (صحیح)STRLEN(fname);
	retval = alloc(fnamelen + extlen + 3);
	اگر (retval == پوچ)
	    بازگشت پوچ;
	STRCPY(retval, fname);
#ifdef VMS
	vms_remove_version(retval); // we do not need versions here
#endif
    }

    /*
     * search backwards until we hit a '/', '\' or ':' replacing all '.'
     * by '_' for MSDOS or when shortname option set and ext starts with a dot.
     * Then truncate what is after the '/', '\' or ':' to 8 characters for
     * MSDOS and 26 characters for AMIGA, a lot more for UNIX.
     */
    برای (ptr = retval + fnamelen; ptr > retval; MB_PTR_BACK(retval, ptr))
    {
	اگر (*ext == '.' && shortname)
	    اگر (*ptr == '.')	// replace '.' by '_'
		*ptr = '_';
	اگر (vim_ispathsep(*ptr))
	{
	    ++ptr;
	    بشکن;
	}
    }

    // the file name has at most BASENAMELEN characters.
    ptrlen = (size_t)(fnamelen - (ptr - retval));
    اگر (ptrlen > (unsigned)BASENAMELEN)
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
	اگر (fname == پوچ || *fname == NUL
				   || vim_ispathsep(fname[STRLEN(fname) - 1]))
	{
	    اگر (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	وگرنه اگر (*ext == '.')
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
	وگرنه اگر ((e = vim_strchr(ptr, '.')) == پوچ)
	    *s++ = '.';
	/*
	 * If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be truncated
	 */
	وگرنه اگر ((صحیح)(ptrlen - (e - retval)) + extlen > 4)
	    s = e + 4 - extlen;
    }
#ifdef MSWIN
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    وگرنه اگر ((fname == پوچ || *fname == NUL) && *ext == '.')
	*s++ = '_';
#endif

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
    اگر (fname != پوچ && STRCMP(fname, retval) == 0)
    {
	// we search for a character that can be replaced by '_'
	وقتی (--s >= ptr)
	{
	    اگر (*s != '_')
	    {
		*s = '_';
		بشکن;
	    }
	}
	اگر (s < ptr)	// fname was "________.<ext>", how tricky!
	    *ptr = 'v';
    }
    بازگشت retval;
}

/*
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE for end-of-file.
 * If the line is truncated then buf[size - 2] will not be NUL.
 */
    صحیح
vim_fgets(char_u *buf, صحیح size, FILE *fp)
{
    نویسه	*eof;
#define FGETS_SIZE 200
    نویسه	tbuf[FGETS_SIZE];

    // safety check
    اگر (size < 2)
    {
	اگر (size == 1)
	    buf[0] = NUL;
	بازگشت TRUE;
    }

    buf[size - 2] = NUL;
    eof = fgets((نویسه *)buf, size, fp);
    اگر (buf[size - 2] != NUL && buf[size - 2] != '\n')
    {
	buf[size - 1] = NUL;	    // Truncate the line

	// Now throw away the rest of the line:
	انجامبده
	{
	    tbuf[FGETS_SIZE - 2] = NUL;
	    vim_ignoredp = fgets((نویسه *)tbuf, FGETS_SIZE, fp);
	} وقتی (tbuf[FGETS_SIZE - 2] != NUL && tbuf[FGETS_SIZE - 2] != '\n');
    }
    بازگشت (eof == پوچ);
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 * Return -1 for failure, 0 for success.
 */
    صحیح
vim_rename(char_u *from, char_u *to)
{
    صحیح		n;
    صحیح		ret;
#ifdef AMIGA
    BPTR	flock;
#endif
    stat_T	st;
    صحیح		use_tmp_file = FALSE;

    /*
     * When the names are identical, there is nothing to do.  When they refer
     * to the same file (ignoring case and slash/backslash differences) but
     * the file name differs we need to go through a temp file.
     */
    اگر (fnamecmp(from, to) == 0)
    {
	اگر (p_fic && STRCMP(gettail(from), gettail(to)) != 0)
	    use_tmp_file = TRUE;
	وگرنه
	    بازگشت 0;
    }

    /*
     * Fail if the "from" file doesn't exist.  Avoids that "to" is deleted.
     */
    اگر (mch_stat((نویسه *)from, &st) < 0)
	بازگشت -1;

#ifdef UNIX
    {
	stat_T	st_to;

	// It's possible for the source and destination to be the same file.
	// This happens when "from" and "to" differ in case and are on a FAT32
	// filesystem.  In that case go through a temp file name.
	اگر (mch_stat((نویسه *)to, &st_to) >= 0
		&& st.st_dev == st_to.st_dev
		&& st.st_ino == st_to.st_ino)
	    use_tmp_file = TRUE;
    }
#endif
#ifdef MSWIN
    {
	BY_HANDLE_FILE_INFORMATION info1, info2;

	// It's possible for the source and destination to be the same file.
	// In that case go through a temp file name.  This makes rename("foo",
	// "./foo") a no-op (in a complicated way).
	اگر (win32_fileinfo(from, &info1) == FILEINFO_OK
		&& win32_fileinfo(to, &info2) == FILEINFO_OK
		&& info1.dw_Volume_Serial_عدد == info2.dw_Volume_Serial_عدد
		&& info1.n_File_Index_بالا == info2.n_File_Index_بالا
		&& info1.n_File_Index_کم == info2.n_File_Index_کم)
	    use_tmp_file = TRUE;
    }
#endif

    اگر (use_tmp_file)
    {
	نویسه	tempname[MAXPATHL + 1];

	/*
	 * Find a name that doesn't exist and is in the same directory.
	 * Rename "from" to "tempname" and then rename "tempname" to "to".
	 */
	اگر (STRLEN(from) >= MAXPATHL - 5)
	    بازگشت -1;
	STRCPY(tempname, from);
	برای (n = 123; n < 99999; ++n)
	{
	    sprintf((نویسه *)gettail((char_u *)tempname), "%d", n);
	    اگر (mch_stat(tempname, &st) < 0)
	    {
		اگر (mch_rename((نویسه *)from, tempname) == 0)
		{
		    اگر (mch_rename(tempname, (نویسه *)to) == 0)
			بازگشت 0;
		    // Strange, the second step failed.  Try moving the
		    // file back and return failure.
		    (تهی)mch_rename(tempname, (نویسه *)from);
		    بازگشت -1;
		}
		// If it fails for one temp name it will most likely fail
		// for any temp name, give up.
		بازگشت -1;
	    }
	}
	بازگشت -1;
    }

    /*
     * Delete the "to" file, this is required on some systems to make the
     * mch_rename() work, on other systems it makes sure that we don't have
     * two files when the mch_rename() fails.
     */

#ifdef AMIGA
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
    flock = Lock((UBYTE *)from, (long)VIM_ACCESS_READ);
#endif
    mch_remove(to);
#ifdef AMIGA
    اگر (flock)
	UnLock(flock);
#endif

    /*
     * First try a normal rename, return if it works.
     */
    اگر (mch_rename((نویسه *)from, (نویسه *)to) == 0)
	بازگشت 0;

    /*
     * Rename() failed, try copying the file.
     */
    ret = vim_copyfile(from, to);
    اگر (ret != OK)
	بازگشت -1;

    /*
     * Remove copied original file
     */
    اگر (mch_stat((نویسه *)from, &st) >= 0)
	mch_remove(from);

    بازگشت 0;
}


/*
 * Create the new file with same permissions as the original.
 * Return FAIL for failure, OK for success.
 */
    صحیح
vim_copyfile(char_u *from, char_u *to)
{
    صحیح		fd_in;
    صحیح		fd_out;
    صحیح		n;
    نویسه	*errmsg = پوچ;
    نویسه	*buffer;
    long	perm;
#ifdef HAVE_ACL
    vim_acl_T	acl;		// ACL from original file
#endif

#ifdef HAVE_READLINK
    صحیح		ret;
    صحیح		len;
    stat_T	st;
    نویسه	linkbuf[MAXPATHL + 1];

    ret = mch_lstat((نویسه *)from, &st);
    اگر (ret >= 0 && S_ISLNK(st.st_mode))
    {
	ret = -1;

	len = readlink((نویسه *)from, linkbuf, MAXPATHL);
	اگر (len > 0)
	{
	    linkbuf[len] = NUL;

	    // Create link
	    ret = symlink(linkbuf, (نویسه *)to);
	}

	بازگشت ret == 0 ? OK : FAIL;
    }
#endif

    perm = mch_getperm(from);
#ifdef HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    acl = mch_get_acl(from);
#endif
    fd_in = mch_open((نویسه *)from, O_RDONLY|O_EXTRA, 0);
    اگر (fd_in == -1)
    {
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	بازگشت FAIL;
    }

    // Create the new file with same permissions as the original.
    fd_out = mch_open((نویسه *)to,
		       O_CREAT|O_EXCL|O_WRONLY|O_EXTRA|O_NOFOLLOW, (صحیح)perm);
    اگر (fd_out == -1)
    {
	close(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	بازگشت FAIL;
    }

    buffer = alloc(WRITEBUFSIZE);
    اگر (buffer == پوچ)
    {
	close(fd_out);
	close(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	بازگشت FAIL;
    }

    وقتی ((n = read_eintr(fd_in, buffer, WRITEBUFSIZE)) > 0)
	اگر (write_eintr(fd_out, buffer, n) != n)
	{
	    errmsg = _(e_error_writing_to_str);
	    بشکن;
	}

    vim_free(buffer);
    close(fd_in);
    اگر (close(fd_out) < 0)
	errmsg = _(e_error_closing_str);
    اگر (n < 0)
    {
	errmsg = _(e_error_reading_str);
	to = from;
    }
#ifndef UNIX	    // for Unix mch_open() already set the permission
    mch_setperm(to, perm);
#endif
#ifdef HAVE_ACL
    mch_set_acl(to, acl);
    mch_free_acl(acl);
#endif
#اگر defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    mch_copy_sec(from, to);
#endif
    اگر (errmsg != پوچ)
    {
	semsg(errmsg, to);
	بازگشت FAIL;
    }
    بازگشت OK;
}

ایستا صحیح already_warned = FALSE;

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
	بازگشت FALSE;

    // Avoid doing a check twice.  The OK/Reload dialog can cause a focus
    // event and we would keep on checking if the file is steadily growing.
    // Do check again after typing something.
    اگر (focus && did_check_timestamps)
    {
	need_check_timestamps = TRUE;
	بازگشت FALSE;
    }

    اگر (!stuff_empty() || global_busy || !typebuf_typed()
			|| autocmd_busy || curbuf_lock > 0 || allbuf_lock > 0)
	need_check_timestamps = TRUE;		// check later
    وگرنه
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

		set_bufref(&bufref, buf);
		n = buf_check_timestamp(buf, focus);
		اگر (didit < n)
		    didit = n;
		اگر (n > 0 && !bufref_valid(&bufref))
		{
		    // Autocommands have removed the buffer, start at the
		    // first one again.
		    buf = firstbuf;
		    ادامه;
		}
	    }
	}
	--no_wait_return;
	need_check_timestamps = FALSE;
	اگر (need_wait_return && didit == 2)
	{
	    // make sure msg isn't overwritten
	    پیام_puts("\n");
	    out_flush();
	}
    }
    بازگشت didit;
}

/*
 * Move all the lines from buffer "frombuf" to buffer "tobuf".
 * Return OK or FAIL.  When FAIL "tobuf" is incomplete and/or "frombuf" is not
 * empty.
 */
    ایستا صحیح
move_سطرها(buf_T *frombuf, buf_T *tobuf)
{
    buf_T	*tbuf = curbuf;
    صحیح		retval = OK;
    linenr_T	lnum;
    char_u	*p;

    // Copy the lines in "frombuf" to "tobuf".
    curbuf = tobuf;
    برای (lnum = 1; lnum <= frombuf->b_ml.ml_سطر_شمار; ++lnum)
    {
	p = vim_strnsave(ml_get_buf(frombuf, lnum, FALSE), ml_get_buf_len(frombuf, lnum));
	اگر (p == پوچ || ml_append(lnum - 1, p, 0, FALSE) == FAIL)
	{
	    vim_free(p);
	    retval = FAIL;
	    بشکن;
	}
	vim_free(p);
    }

    // Delete all the lines in "frombuf".
    اگر (retval != FAIL)
    {
	curbuf = frombuf;
	برای (lnum = curbuf->b_ml.ml_سطر_شمار; lnum > 0; --lnum)
	    اگر (ml_delete(lnum) == FAIL)
	    {
		// Oops!  We could try putting back the saved lines, but that
		// might fail again...
		retval = FAIL;
		بشکن;
	    }
    }

    curbuf = tbuf;
    بازگشت retval;
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
    صحیح		focus UNUSED)	// called for GUI focus event
{
    stat_T	st;
    صحیح		stat_res;
    صحیح		retval = 0;
    نویسه	*mesg = پوچ;
    نویسه	*mesg2 = "";
    صحیح		helpmesg = FALSE;
    شمارش {
	RELOAD_NONE,
	RELOAD_NORMAL,
	RELOAD_DETECT
    }		reload = RELOAD_NONE;
#اگر defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
    صحیح		can_reload = FALSE;
#endif
    off_T	orig_size = buf->b_orig_size;
    صحیح		orig_mode = buf->b_orig_mode;
#ifdef FEAT_GUI
    صحیح		save_mouse_correct = need_mouse_correct;
#endif
    ایستا صحیح	busy = FALSE;
    صحیح		n;
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    // If there is no file name, the buffer is not loaded, 'buftype' is
    // set, we are in the middle of a save or being called recursively: ignore
    // this buffer.
    اگر (buf->b_ffname == پوچ
	    || buf->b_ml.ml_mfp == پوچ
	    || !bt_normal(buf)
	    || buf->b_saving
	    || busy
#ifdef FEAT_NETBEANS_INTG
	    || isNetbeansBuffer(buf)
#endif
#ifdef FEAT_TERMINAL
	    || buf->b_term != پوچ
#endif
	    )
	بازگشت 0;

    اگر (       !(buf->b_flags & BF_NOTEDITED)
	    && buf->b_mtime != 0
	    && ((stat_res = mch_stat((نویسه *)buf->b_ffname, &st)) < 0
		|| time_differs(&st, buf->b_mtime, buf->b_mtime_ns)
		|| st.st_size != buf->b_orig_size
#ifdef HAVE_ST_MODE
		|| (صحیح)st.st_mode != buf->b_orig_mode
#وگرنه
		|| mch_getperm(buf->b_ffname) != buf->b_orig_mode
#endif
		))
    {
	long prev_b_mtime = buf->b_mtime;

	retval = 1;

	// set b_mtime to stop further warnings (e.g., when executing
	// FileChangedShell autocmd)
	اگر (stat_res < 0)
	{
	    // Check the file again later to see if it re-appears.
	    buf->b_mtime = -1;
	    buf->b_orig_size = 0;
	    buf->b_orig_mode = 0;
	}
	وگرنه
	    buf_store_time(buf, &st, buf->b_ffname);

	// Don't do anything for a directory.  Might contain the file
	// explorer.
	اگر (mch_isdir(buf->b_fname))
	    ;

	/*
	 * If 'autoread' is set, the buffer has no changes and the file still
	 * exists, reload the buffer.  Use the buffer-local option value if it
	 * was set, the global option value otherwise.
	 */
	وگرنه اگر ((buf->b_p_ar >= 0 ? buf->b_p_ar : p_ar)
				       && !bufIsChanged(buf) && stat_res >= 0)
	    reload = RELOAD_NORMAL;
	وگرنه
	{
	    نویسه    *reason;
#ifdef FEAT_EVAL
	    size_t  reasonlen;
#endif

	    اگر (stat_res < 0)
	    {
		reason = "deleted";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("deleted");
#endif
	    }
	    وگرنه اگر (bufIsChanged(buf))
	    {
		reason = "conflict";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("conflict");
#endif
	    }
	    /*
	     * Check if the file contents really changed to avoid giving a
	     * warning when only the timestamp was set (e.g., checked out of
	     * CVS).  Always warn when the buffer was changed.
	     */
	    وگرنه اگر (orig_size != buf->b_orig_size || buf_contents_changed(buf))
	    {
		reason = "changed";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("changed");
#endif
	    }
	    وگرنه اگر (orig_mode != buf->b_orig_mode)
	    {
		reason = "mode";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("mode");
#endif
	    }
	    وگرنه
	    {
		reason = "time";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("time");
#endif
	    }

	    /*
	     * Only give the warning if there are no FileChangedShell
	     * autocommands.
	     * Avoid being called recursively by setting "busy".
	     */
	    busy = TRUE;
#ifdef FEAT_EVAL
	    set_vim_var_string(VV_FCS_REASON, (char_u *)reason, (صحیح)reasonlen);
	    set_vim_var_string(VV_FCS_CHOICE, (char_u *)"", 0);
#endif
	    ++allbuf_lock;
	    n = apply_autocmds(EVENT_FILECHANGEDSHELL,
				      buf->b_fname, buf->b_fname, FALSE, buf);
	    --allbuf_lock;
	    busy = FALSE;
	    اگر (n)
	    {
		اگر (!bufref_valid(&bufref))
		    emsg(_(e_filechangedshell_autocommand_deleted_buffer));
#ifdef FEAT_EVAL
		char_u	*s = get_vim_var_str(VV_FCS_CHOICE);
		اگر (STRCMP(s, "reload") == 0 && *reason != 'd')
		    reload = RELOAD_NORMAL;
		وگرنه اگر (STRCMP(s, "edit") == 0)
		    reload = RELOAD_DETECT;
		وگرنه اگر (STRCMP(s, "ask") == 0)
		    n = FALSE;
		وگرنه
#endif
		    بازگشت 2;
	    }
	    اگر (!n)
	    {
		اگر (*reason == 'd')
		{
		    // Only give the message once.
		    اگر (prev_b_mtime != -1)
			mesg = _(e_file_str_no_longer_available);
		}
		وگرنه
		{
		    helpmesg = TRUE;
#اگر defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		    can_reload = TRUE;
#endif
		    اگر (reason[2] == 'n')
		    {
			mesg = _("W12: Warning: File \"%s\" has changed and the buffer was changed in Vim as well");
			mesg2 = _("See \":help W12\" for more info.");
		    }
		    وگرنه اگر (reason[1] == 'h')
		    {
			mesg = _("W11: Warning: File \"%s\" has changed since editing started");
			mesg2 = _("See \":help W11\" for more info.");
		    }
		    وگرنه اگر (*reason == 'm')
		    {
			mesg = _("W16: Warning: Mode of file \"%s\" has changed since editing started");
			mesg2 = _("See \":help W16\" for more info.");
		    }
		    وگرنه
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
    وگرنه اگر ((buf->b_flags & BF_NEW) && !(buf->b_flags & BF_NEW_W)
						&& vim_fexists(buf->b_ffname))
    {
	retval = 1;
	mesg = _("W13: Warning: File \"%s\" has been created after editing started");
	buf->b_flags |= BF_NEW_W;
#اگر defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	can_reload = TRUE;
#endif
    }

    اگر (mesg != پوچ)
    {
	char_u *path;

	path = home_replace_save(buf, buf->b_fname);
	اگر (path != پوچ)
	{
	    size_t  tbufsize;
	    نویسه    *tbuf;

	    اگر (!helpmesg)
		mesg2 = "";
	    tbufsize = STRLEN(mesg) + STRLEN(path) + 2 + STRLEN(mesg2) + 1; // +2 for either '\n' or "; "
									    // and +1 for NUL
	    tbuf = alloc(tbufsize);
	    اگر (tbuf != پوچ)
	    {
		صحیح tbuflen;

		tbuflen = vim_snprintf(tbuf, tbufsize, mesg, path);
#ifdef FEAT_EVAL
		// Set warningmsg here, before the unimportant and output-specific
		// mesg2 has been appended.
		set_vim_var_string(VV_WARNINGMSG, (char_u *)tbuf, tbuflen);
#endif
#اگر defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		اگر (can_reload)
		{
		    اگر (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "\n%s", mesg2);
		    انتخاب (do_dialog(VIM_WARNING, (char_u *)_("Warning"),
			    (char_u *)tbuf,
			    (char_u *)_("&OK\n&Load File\nLoad File &and Options"),
			    1, پوچ, TRUE))
		    {
			حالت 2:
			    reload = RELOAD_NORMAL;
			    بشکن;
			حالت 3:
			    reload = RELOAD_DETECT;
			    بشکن;
		    }
		}
		وگرنه
#endif
		اگر (State > MODE_NORMAL_BUSY || (State & MODE_CMDLINE)
								 || already_warned)
		{
		    اگر (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "; %s", mesg2);
		    emsg(tbuf);
		    retval = 2;
		}
		وگرنه
		{
		    اگر (!autocmd_busy)
		    {
			پیام_start();
			پیام_puts_attr(tbuf, HL_ATTR(HLF_E) + پیام_HIST);
			اگر (*mesg2 != NUL)
			    پیام_puts_attr(mesg2, HL_ATTR(HLF_W) + پیام_HIST);
			پیام_clr_eos();
			(تهی)پیام_end();
			اگر (emsg_silent == 0 && !in_assert_fails)
			{
			    out_flush();
#ifdef FEAT_GUI
			    اگر (!focus)
#endif
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
	    vim_free(path);
	}
    }

    اگر (reload != RELOAD_NONE)
    {
	// Reload the buffer.
	buf_reload(buf, orig_mode, reload == RELOAD_DETECT);
#ifdef FEAT_PERSISTENT_UNDO
	اگر (bufref_valid(&bufref) && buf->b_p_udf && buf->b_ffname != پوچ)
	{
	    char_u	    hash[UNDO_HASH_SIZE];
	    buf_T	    *save_curbuf = curbuf;

	    // Any existing undo file is unusable, write it now.
	    curbuf = buf;
	    u_compute_hash(hash);
	    u_write_undo(پوچ, FALSE, buf, hash);
	    curbuf = save_curbuf;
	}
#endif
    }

    // Trigger FileChangedShell when the file was changed in any way.
    اگر (bufref_valid(&bufref) && retval != 0)
	(تهی)apply_autocmds(EVENT_FILECHANGEDSHELLPOST,
				      buf->b_fname, buf->b_fname, FALSE, buf);
#ifdef FEAT_GUI
    // restore this in case an autocommand has set it; it would break
    // 'mousefocus'
    need_mouse_correct = save_mouse_correct;
#endif

    بازگشت retval;
}

/*
 * Reload a buffer that is already loaded.
 * Used when the file was changed outside of Vim.
 * "orig_mode" is buf->b_orig_mode before the need for reloading was detected.
 * buf->b_orig_mode may have been reset already.
 */
    تهی
buf_reload(buf_T *buf, صحیح orig_mode, صحیح reload_options)
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
	بازگشت;
    }

    // Unless reload_options is set, we only want to read the text from the
    // file, not reset the syntax highlighting, clear marks, diff status, etc.
    // Force the fileformat and encoding to be the same.
    اگر (reload_options)
	CLEAR_FIELD(ea);
    وگرنه
	prepped = prep_exarg(&ea, buf);

    اگر (prepped == OK)
    {
	old_cursor = curwin->w_cursor;
	old_topline = curwin->w_topline;

	اگر (p_ur < 0 || curbuf->b_ml.ml_سطر_شمار <= p_ur)
	{
	    // Save all the text, so that the reload can be undone.
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    saved = u_savecommon(0, curbuf->b_ml.ml_سطر_شمار + 1, 0, TRUE);
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
	    savebuf = پوچ;
	وگرنه
	{
	    // Allocate a buffer without putting it in the buffer list.
	    savebuf = buflist_new(پوچ, پوچ, (linenr_T)1, BLN_DUMMY);
	    set_bufref(&bufref, savebuf);
	    اگر (savebuf != پوچ && buf == curbuf)
	    {
		// Open the memline.
		curbuf = savebuf;
		curwin->w_buffer = savebuf;
		saved = ml_open(curbuf);
		curbuf = buf;
		curwin->w_buffer = buf;
	    }
	    اگر (savebuf == پوچ || saved == FAIL || buf != curbuf
				      || move_سطرها(buf, savebuf) == FAIL)
	    {
		semsg(_(e_could_not_prepare_for_reloading_str), buf->b_fname);
		saved = FAIL;
	    }
	}

	اگر (saved == OK)
	{
	    صحیح old_پیام_silent = پیام_silent;

	    curbuf->b_flags |= BF_CHECK_RO;	// check for RO again
	    curbuf->b_keep_filetype = true;	// don't detect 'filetype'

	    اگر (shortmess(SHM_FILEINFO))
		پیام_silent = 1;

	    اگر (readfile(buf->b_ffname, buf->b_fname, (linenr_T)0,
			(linenr_T)0,
			(linenr_T)MAXLNUM, &ea, flags) != OK)
	    {
#اگر defined(FEAT_EVAL)
		اگر (!aborting())
#endif
		    semsg(_(e_could_not_reload_str), buf->b_fname);
		اگر (savebuf != پوچ && bufref_valid(&bufref) && buf == curbuf)
		{
		    // Put the text back from the save buffer.  First
		    // delete any lines that readfile() added.
		    وقتی (!BUFEMPTY())
			اگر (ml_delete(buf->b_ml.ml_سطر_شمار) == FAIL)
			    بشکن;
		    (تهی)move_سطرها(savebuf, buf);
		}
	    }
	    وگرنه اگر (buf == curbuf)  // "buf" still valid
	    {
		// Mark the buffer as unmodified and free undo info.
		unchanged(buf, TRUE, TRUE);
		اگر ((flags & READ_KEEP_UNDO) == 0)
		    u_clearallandblockfree(buf);
		وگرنه
		{
		    // Mark all undo states as changed.
		    u_unchanged(curbuf);
		}
	    }

	    پیام_silent = old_پیام_silent;
	}
	vim_free(ea.cmd);

	اگر (savebuf != پوچ && bufref_valid(&bufref))
	    wipe_buffer(savebuf, FALSE);

#ifdef FEAT_DIFF
	// Invalidate diff info if necessary.
	diff_invalidate(curbuf);
#endif

	// Restore the topline and cursor position and check it (lines may
	// have been removed).
	اگر (old_topline > curbuf->b_ml.ml_سطر_شمار)
	    curwin->w_topline = curbuf->b_ml.ml_سطر_شمار;
	وگرنه
	    curwin->w_topline = old_topline;
	curwin->w_cursor = old_cursor;
	check_cursor();
	update_topline();
	curbuf->b_keep_filetype = false;
#ifdef FEAT_FOLDING
	{
	    win_T	*wp;
	    tabpage_T	*tp;

	    // Update folds unless they are defined manually.
	    FOR_ALL_TAB_WINDOWS(tp, wp)
		اگر (wp->w_buffer == curwin->w_buffer
			&& !foldmethodIsManual(wp))
		    foldUpdateAll(wp);
	}
#endif
	// If the mode didn't change and 'readonly' was set, keep the old
	// value; the user probably used the ":view" command.  But don't
	// reset it, might have had a read error.
	اگر (orig_mode == curbuf->b_orig_mode)
	    curbuf->b_p_ro |= old_ro;

	// Modelines must override settings done by autocommands.
	do_modelines(0);
    }

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);
    // Careful: autocommands may have made "buf" invalid!
}

    تهی
buf_store_time(buf_T *buf, stat_T *st, char_u *fname UNUSED)
{
    buf->b_mtime = (long)st->st_mtime;
#ifdef ST_MTIM_NSEC
    buf->b_mtime_ns = (long)st->ST_MTIM_NSEC;
#وگرنه
    buf->b_mtime_ns = 0;
#endif
    buf->b_orig_size = st->st_size;
#ifdef HAVE_ST_MODE
    buf->b_orig_mode = (صحیح)st->st_mode;
#وگرنه
    buf->b_orig_mode = mch_getperm(fname);
#endif
}

/*
 * Adjust the line with missing eol, used for the next write.
 * Used for do_filter(), when the input lines for the filter are deleted.
 */
    تهی
write_lnum_adjust(linenr_T offset)
{
    اگر (curbuf->b_no_eol_lnum != 0)	// only if there is a missing eol
	curbuf->b_no_eol_lnum += offset;
}

// Subfuncions for readdirex()
#ifdef FEAT_EVAL
# ifdef MSWIN
    ایستا char_u *
getfpermwfd(WIN32_FIND_DATAW *wfd, char_u *perm)
{
    stat_T	    st;
    unsigned short  st_mode;
    DWORD	    flag = wfd->dwFileAttributes;
    WCHAR	    *wp;

    st_mode = (flag & FILE_ATTRIBUTE_DIRECTORY)
					? (_S_IFDIR | _S_IEXEC) : _S_IFREG;
    st_mode |= (flag & FILE_ATTRIBUTE_READONLY)
					? _S_IREAD : (_S_IREAD | _S_IWRITE);

    wp = wcsrchr(wfd->cFileName, L'.');
    اگر (wp != پوچ)
    {
	اگر (_wcsicmp(wp, L".exe") == 0 ||
		_wcsicmp(wp, L".com") == 0 ||
		_wcsicmp(wp, L".cmd") == 0 ||
		_wcsicmp(wp, L".bat") == 0)
	    st_mode |= _S_IEXEC;
    }

    // Copy user bits to group/other.
    st_mode |= (st_mode & 0700) >> 3;
    st_mode |= (st_mode & 0700) >> 6;

    st.st_mode = st_mode;
    بازگشت getfpermst(&st, perm);
}

    ایستا تهی
getftypewfd(WIN32_FIND_DATAW *wfd, string_T *ret)
{
    DWORD flag = wfd->dwFileAttributes;
    DWORD tag = wfd->dwReserved0;

    اگر (flag & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	اگر (tag == IO_REPARSE_TAG_MOUNT_POINT)
	{
	    ret->string = (char_u *)"junction";
	    ret->length = STRLEN_LITERAL("junction");
	    بازگشت;
	}
	اگر (tag == IO_REPARSE_TAG_SYMLINK)
	{
	    اگر (flag & FILE_ATTRIBUTE_DIRECTORY)
	    {
		ret->string = (char_u *)"linkd";
		ret->length = STRLEN_LITERAL("linkd");
		بازگشت;
	    }

	    ret->string = (char_u *)"link";
	    ret->length = STRLEN_LITERAL("link");
	    بازگشت;
	}

	// unknown reparse point type
	ret->string = (char_u *)"reparse";
	ret->length = STRLEN_LITERAL("reparse");
	بازگشت;
    }

    اگر (flag & FILE_ATTRIBUTE_DIRECTORY)
    {
	ret->string = (char_u *)"dir";
	ret->length = STRLEN_LITERAL("dir");
	بازگشت;
    }

    ret->string = (char_u *)"file";
    ret->length = STRLEN_LITERAL("file");
}

    ایستا dict_T *
create_readdirex_item(WIN32_FIND_DATAW *wfd)
{
    dict_T	*item;
    char_u	*p;
    varnumber_T	size, time;
    char_u	permbuf[] = "---------";
    string_T	s;

    item = dict_alloc();
    اگر (item == پوچ)
	بازگشت پوچ;
    item->dv_refcount++;

    p = utf16_to_enc(wfd->cFileName, پوچ);
    اگر (p == پوچ)
	برو theend;
    اگر (dict_add_string(item, "name", p) == FAIL)
    {
	vim_free(p);
	برو theend;
    }
    vim_free(p);

    size = (((varnumber_T)wfd->n_File_Size_بالا) << 32) | wfd->n_File_Size_کم;
    اگر (dict_add_عدد(item, "size", size) == FAIL)
	برو theend;

    // Convert FILETIME to unix time.
    time = (((((varnumber_T)wfd->ftLastWriteTime.dw_بالا_Date_Time) << 32) |
		wfd->ftLastWriteTime.dw_کم_Date_Time)
	    - 116444736000000000) / 10000000;
    اگر (dict_add_عدد(item, "time", time) == FAIL)
	برو theend;

    getftypewfd(wfd, &s);
    اگر (dict_add_string_len(item, "type", s.string, (صحیح)s.length) == FAIL)
	برو theend;
    اگر (dict_add_string(item, "perm", getfpermwfd(wfd, permbuf)) == FAIL)
	برو theend;

    اگر (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	برو theend;
    اگر (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	برو theend;

    بازگشت item;

theend:
    dict_unref(item);
    بازگشت پوچ;
}
# وگرنه
    ایستا dict_T *
create_readdirex_item(char_u *path, char_u *name)
{
    dict_T	*item;
    نویسه	*p;
    size_t	pathlen, len;
    size_t	namelen;
    stat_T	st;
    صحیح		ret, link = FALSE;
    varnumber_T	size;
    char_u	permbuf[] = "---------";
    string_T	q = {پوچ, 0};
    ساختار passwd *pw;
    ساختار group  *gr;

    item = dict_alloc();
    اگر (item == پوچ)
	بازگشت پوچ;
    item->dv_refcount++;

    pathlen = STRLEN(path);
    namelen = STRLEN(name);
    len = pathlen + 1 + namelen + 1;
    p = alloc(len);
    اگر (p == پوچ)
	برو theend;
    اگر (pathlen > 0 && path[pathlen - 1] == '/')
	vim_snprintf(p, len, "%s%s", path, name);
    وگرنه
	vim_snprintf(p, len, "%s/%s", path, name);
    ret = mch_lstat(p, &st);
    اگر (ret >= 0 && S_ISLNK(st.st_mode))
    {
	link = TRUE;
	ret = mch_stat(p, &st);
	اگر (ret < 0)
	    STR_LITERAL_SET(q, "link");
    }
    vim_free(p);

    اگر (dict_add_string_len(item, "name", name, (صحیح)namelen) == FAIL)
	برو theend;

    اگر (ret >= 0)
    {
	size = (varnumber_T)st.st_size;
	اگر (S_ISDIR(st.st_mode))
	    size = 0;
	// non-perfect check for overflow
	وگرنه اگر ((off_T)size != (off_T)st.st_size)
	    size = -2;
	اگر (dict_add_عدد(item, "size", size) == FAIL)
	    برو theend;
	اگر (dict_add_عدد(item, "time", (varnumber_T)st.st_mtime) == FAIL)
	    برو theend;

	اگر (link)
	{
	    اگر (S_ISDIR(st.st_mode))
		STR_LITERAL_SET(q, "linkd");
	    وگرنه
		STR_LITERAL_SET(q, "link");
	}
	وگرنه
	{
	    q.string = getftypest(&st);
	    q.length = STRLEN(q.string);
	}
	اگر (dict_add_string_len(item, "type", q.string, (صحیح)q.length) == FAIL)
	    برو theend;
	اگر (dict_add_string(item, "perm", getfpermst(&st, permbuf)) == FAIL)
	    برو theend;

	pw = getpwuid(st.st_uid);
	اگر (pw == پوچ)
	    STR_LITERAL_SET(q, "");
	وگرنه
	{
	    q.string = (char_u *)pw->pw_name;
	    q.length = STRLEN(q.string);
	}
	اگر (dict_add_string_len(item, "user", q.string, (صحیح)q.length) == FAIL)
	    برو theend;
#  اگر !defined(VMS) || (defined(VMS) && defined(HAVE_XOS_R_H))
	gr = getgrgid(st.st_gid);
	اگر (gr == پوچ)
	    STR_LITERAL_SET(q, "");
	وگرنه
	{
	    q.string = (char_u *)gr->gr_name;
	    q.length = STRLEN(q.string);
	}
#  endif
	اگر (dict_add_string_len(item, "group", q.string, (صحیح)q.length) == FAIL)
	    برو theend;
    }
    وگرنه
    {
	اگر (dict_add_عدد(item, "size", -1) == FAIL)
	    برو theend;
	اگر (dict_add_عدد(item, "time", -1) == FAIL)
	    برو theend;
	اگر (q.string == پوچ)
	{
	    اگر (dict_add_string_len(item, "type", (char_u *)"", 0) == FAIL)
		برو theend;
	}
	وگرنه
	{
	    اگر (dict_add_string_len(item, "type", q.string, (صحیح)q.length) == FAIL)
		برو theend;
	}
	اگر (dict_add_string_len(item, "perm", (char_u *)"", 0) == FAIL)
	    برو theend;
	اگر (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	    برو theend;
	اگر (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	    برو theend;
    }
    بازگشت item;

theend:
    dict_unref(item);
    بازگشت پوچ;
}
# endif

    ایستا صحیح
compare_readdirex_item(ثابت تهی *p1, ثابت تهی *p2)
{
    char_u  *name1, *name2;

    name1 = dict_get_string(*(dict_T**)p1, "name", FALSE);
    name2 = dict_get_string(*(dict_T**)p2, "name", FALSE);
    اگر (readdirex_sort == READDIR_SORT_BYTE)
	بازگشت STRCMP(name1, name2);
    اگر (readdirex_sort == READDIR_SORT_IC)
	بازگشت STRICMP(name1, name2);

    بازگشت STRCOLL(name1, name2);
}

    ایستا صحیح
compare_readdir_item(ثابت تهی *s1, ثابت تهی *s2)
{
    اگر (readdirex_sort == READDIR_SORT_BYTE)
	بازگشت STRCMP(*(نویسه **)s1, *(نویسه **)s2);
    اگر (readdirex_sort == READDIR_SORT_IC)
	بازگشت STRICMP(*(نویسه **)s1, *(نویسه **)s2);

    بازگشت STRCOLL(*(نویسه **)s1, *(نویسه **)s2);
}
#endif

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
    char_u	*path,
    صحیح		withattr UNUSED,
    تهی	*context,
    صحیح		(*checkitem)(تهی *context, تهی *item),
    صحیح		sort)
{
    صحیح			failed = FALSE;
    char_u		*p;
# ifdef MSWIN
    char_u		*buf;
    صحیح			ok;
    HANDLE		hFind = INVALID_HANDLE_مقدار;
    WIN32_FIND_DATAW    wfd;
    WCHAR		*wn = پوچ;	// UTF-16 name, NULL when not used.
    char_u		*p_end;
# وگرنه
    DIR			*dirp;
    ساختار dirent	*dp;
# endif

    ga_init2(gap, اندازه(تهی *), 20);

# ifdef FEAT_EVAL
#  define FREE_ITEM(item)   انجامبده { \
	اگر (withattr) \
	    dict_unref((dict_T*)(item)); \
	وگرنه \
	    vim_free(item); \
    } وقتی (0)

    readdirex_sort = READDIR_SORT_BYTE;
# وگرنه
#  define FREE_ITEM(item)   vim_free(item)
# endif

# ifdef MSWIN
    buf = alloc(MAXPATHL);
    اگر (buf == پوچ)
	بازگشت FAIL;
    STRNCPY(buf, path, MAXPATHL-5);
    p = p_end = buf + STRLEN(buf);
    MB_PTR_BACK(buf, p);
    اگر (*p == '\\' || *p == '/')
	p_end = p;
    STRCPY(p_end, "\\*");

    wn = enc_to_utf16(buf, پوچ);
    اگر (wn != پوچ)
	hFind = FindFirstFileW(wn, &wfd);
    ok = (hFind != INVALID_HANDLE_مقدار);
    اگر (!ok)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    وگرنه
    {
	وقتی (ok)
	{
	    صحیح	    ignore;
	    تهی    *item;
	    WCHAR   *wp;

	    wp = wfd.cFileName;
	    ignore = wp[0] == L'.' &&
		    (wp[1] == NUL ||
		     (wp[1] == L'.' && wp[2] == NUL));
	    اگر (ignore)
	    {
		ok = FindNextFileW(hFind, &wfd);
		ادامه;
	    }
#  ifdef FEAT_EVAL
	    اگر (withattr)
		item = (تهی*)create_readdirex_item(&wfd);
	    وگرنه
#  endif
		item = (تهی*)utf16_to_enc(wfd.cFileName, پوچ);
	    اگر (item == پوچ)
	    {
		failed = TRUE;
		بشکن;
	    }

	    اگر (!ignore && checkitem != پوچ)
	    {
		صحیح r = checkitem(context, item);

		اگر (r < 0)
		{
		    FREE_ITEM(item);
		    بشکن;
		}
		اگر (r == 0)
		    ignore = TRUE;
	    }

	    اگر (!ignore)
	    {
		اگر (ga_grow(gap, 1) == OK)
		    ((تهی**)gap->ga_data)[gap->ga_len++] = item;
		وگرنه
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    بشکن;
		}
	    }
	    وگرنه
		FREE_ITEM(item);

	    ok = FindNextFileW(hFind, &wfd);
	}
	FindClose(hFind);
    }

    vim_free(buf);
    vim_free(wn);
# وگرنه	// MSWIN
    dirp = opendir((نویسه *)path);
    اگر (dirp == پوچ)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    وگرنه
    {
	برای (;;)
	{
	    صحیح	    ignore;
	    تهی    *item;

	    dp = readdir(dirp);
	    اگر (dp == پوچ)
		بشکن;
	    p = (char_u *)dp->d_name;

	    ignore = p[0] == '.' &&
		    (p[1] == NUL ||
		     (p[1] == '.' && p[2] == NUL));
	    اگر (ignore)
		ادامه;
#  ifdef FEAT_EVAL
	    اگر (withattr)
		item = (تهی*)create_readdirex_item(path, p);
	    وگرنه
#  endif
		item = (تهی*)vim_strsave(p);
	    اگر (item == پوچ)
	    {
		failed = TRUE;
		بشکن;
	    }

	    اگر (checkitem != پوچ)
	    {
		صحیح r = checkitem(context, item);

		اگر (r < 0)
		{
		    FREE_ITEM(item);
		    بشکن;
		}
		اگر (r == 0)
		    ignore = TRUE;
	    }

	    اگر (!ignore)
	    {
		اگر (ga_grow(gap, 1) == OK)
		    ((تهی**)gap->ga_data)[gap->ga_len++] = item;
		وگرنه
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    بشکن;
		}
	    }
	    وگرنه
		FREE_ITEM(item);
	}

	closedir(dirp);
    }
# endif	// MSWIN

# undef FREE_ITEM

    اگر (!failed && gap->ga_len > 0 && sort > READDIR_SORT_NONE)
    {
# ifdef FEAT_EVAL
	readdirex_sort = sort;
	اگر (withattr)
	    qsort((تهی*)gap->ga_data, (size_t)gap->ga_len, اندازه(dict_T*),
		    compare_readdirex_item);
	وگرنه
	    qsort((تهی*)gap->ga_data, (size_t)gap->ga_len, اندازه(char_u *),
		    compare_readdir_item);
# وگرنه
	    sort_strings((char_u **)gap->ga_data, gap->ga_len);
# endif
    }

    بازگشت failed ? FAIL : OK;
}

/*
 * Delete "name" and everything in it, recursively.
 * return 0 for success, -1 if some file was not deleted.
 */
    صحیح
delete_recursive(char_u *name)
{
    صحیح result = 0;

    // A symbolic link to a directory itself is deleted, not the directory it
    // points to.
    اگر (
# اگر defined(UNIX) || defined(MSWIN)
	 mch_isrealdir(name)
# وگرنه
	 mch_isdir(name)
# endif
	    )
    {
	char_u	    *exp = vim_strsave(name);
	garray_T    ga;

	اگر (exp == پوچ)
	    بازگشت -1;
	اگر (readdir_core(&ga, exp, FALSE, پوچ, پوچ, READDIR_SORT_NONE) == OK)
	{
	    صحیح	len = vim_snprintf((نویسه *)NameBuff, MAXPATHL, "%s/", exp);
	    صحیح	i;

	    برای (i = 0; i < ga.ga_len; ++i)
	    {
		vim_snprintf((نویسه *)NameBuff + len, MAXPATHL - len, "%s", ((char_u **)ga.ga_data)[i]);
		اگر (delete_recursive(NameBuff) != 0)
		    // Remember the failure but continue deleting any further
		    // entries.
		    result = -1;
	    }
	    ga_clear_strings(&ga);
	    اگر (mch_rmdir(exp) != 0)
		result = -1;
	}
	وگرنه
	    result = -1;
	vim_free(exp);
    }
    وگرنه
	result = mch_remove(name) == 0 ? 0 : -1;

    بازگشت result;
}
#endif

#اگر defined(TEMPDIRNAMES)
ایستا long	temp_شمار = 0;		// Temp filename counter.

# اگر defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
/*
 * Open temporary directory and take file lock to prevent
 * to be auto-cleaned.
 */
   ایستا تهی
vim_opentempdir(تهی)
{
    DIR *dp = پوچ;

    اگر (vim_tempdir_dp != پوچ)
	بازگشت;

    dp = opendir((ثابت نویسه*)vim_tempdir);
    اگر (dp == پوچ)
	بازگشت;

    vim_tempdir_dp = dp;
    flock(dirfd(vim_tempdir_dp), LOCK_SH);
}

/*
 * Close temporary directory - it automatically release file lock.
 */
   ایستا تهی
vim_closetempdir(تهی)
{
    اگر (vim_tempdir_dp == پوچ)
	بازگشت;

    closedir(vim_tempdir_dp);
    vim_tempdir_dp = پوچ;
}

/*
 * Return true if the temp directory we created is gone.
 */
    ایستا bool
vim_tempdir_gone(تهی)
{
    stat_T	st;

    اگر (vim_tempdir_dp == پوچ)
	بازگشت false;
    بازگشت fstat(dirfd(vim_tempdir_dp), &st) < 0 || st.st_nlink == 0;
}
# endif

/*
 * Delete the temp directory and all files it contains.
 */
    تهی
vim_deltempdir(تهی)
{
    اگر (vim_tempdir == پوچ)
	بازگشت;

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
    ایستا تهی
vim_settempdir(char_u *tempdir)
{
    char_u	*buf;
    size_t	buflen;

    buf = alloc(MAXPATHL + 2);
    اگر (buf == پوچ)
	بازگشت;

    اگر (vim_FullName(tempdir, buf, MAXPATHL, FALSE) == FAIL)
	STRCPY(buf, tempdir);
    buflen = STRLEN(buf);
    اگر (!after_pathsep(buf, buf + buflen))
    {
	STRCPY(buf + buflen, PATHSEPSTR);
	buflen += اندازه(PATHSEP);
    }
    vim_tempdir = vim_strnsave(buf, buflen);
# اگر defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    vim_opentempdir();
# endif
    vim_free(buf);
}
#endif

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
    صحیح	    extra_char UNUSED,  // char to use in the name instead of '?'
    صحیح	    keep UNUSED)
{
#ifdef USE_TMPNAM
    char_u	itmp[L_tmpnam];	// use tmpnam()
#وگرنهاگر defined(MSWIN)
    WCHAR	itmp[TEMPNAMELEN];
#وگرنه
    char_u	itmp[TEMPNAMELEN];
#endif

#ifdef TEMPDIRNAMES
    ایستا نویسه	*(tempdirs[]) = {TEMPDIRNAMES};
    صحیح		i;
# ifndef EEXIST
    stat_T	st;
# endif

# اگر defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    // if the temp directory is gone, force re-creation of it
    اگر (vim_tempdir != پوچ && vim_tempdir_gone())
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
    اگر (vim_tempdir == پوچ)
    {
	/*
	 * Try the entries in TEMPDIRNAMES to create the temp directory.
	 */
	برای (i = 0; i < (صحیح)ARRAY_LENGTH(tempdirs); ++i)
	{
# ifndef HAVE_MKDTEMP
	    long	nr;
	    long	off;
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
		    itmplen += اندازه(PATHSEP);
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
		    اگر (mkdtemp((نویسه *)itmp) != پوچ)
			vim_settempdir(itmp);
#  اگر defined(UNIX) || defined(VMS)
		    (تهی)umask(umask_save);
#  endif
		}
# وگرنه
		// Get an arbitrary number of up to 6 digits.  When it's
		// unlikely that it already exists it will be faster,
		// otherwise it doesn't matter.  The use of mkdir() avoids any
		// security problems because of the predictable number.
		nr = (mch_get_pid() + (long)time(پوچ)) % 1000000L;

		// Try up to 10000 different values until we find a name that
		// doesn't exist.
		برای (off = 0; off < 10000L; ++off)
		{
		    صحیح		r;
#  اگر defined(UNIX) || defined(VMS)
		    mode_t	umask_save;
#  endif

		    vim_snprintf((نویسه *)itmp + itmplen, اندازه(itmp) - itmplen, "v%ld", nr + off);
#  ifndef EEXIST
		    // If mkdir() does not set errno to EEXIST, check for
		    // existing file here.  There is a race condition then,
		    // although it's fail-safe.
		    اگر (mch_stat((نویسه *)itmp, &st) >= 0)
			ادامه;
#  endif
#  اگر defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    umask_save = umask(077);
#  endif
		    r = vim_mkdir(itmp, 0700);
#  اگر defined(UNIX) || defined(VMS)
		    (تهی)umask(umask_save);
#  endif
		    اگر (r == 0)
		    {
			vim_settempdir(itmp);
			بشکن;
		    }
#  ifdef EEXIST
		    // If the mkdir() didn't fail because the file/dir exists,
		    // we probably can't create any dir here, try another
		    // place.
		    اگر (errno != EEXIST)
#  endif
			بشکن;
		}
# endif // HAVE_MKDTEMP
		اگر (vim_tempdir != پوچ)
		    بشکن;
	    }
	}
    }

    اگر (vim_tempdir != پوچ)
    {
	// There is no need to check if the file exists, because we own the
	// directory and nobody else creates a file in it.
	صحیح itmplen = vim_snprintf((نویسه *)itmp, اندازه(itmp), "%s%ld", vim_tempdir, temp_شمار++);
	بازگشت vim_strnsave(itmp, (size_t)itmplen);
    }

    بازگشت پوچ;

#وگرنه // TEMPDIRNAMES
    char_u	*p;

# ifdef MSWIN
    WCHAR	wszTempFile[_MAX_PATH + 1];
    WCHAR	buf4[4];
    WCHAR	*chartab = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char_u	*retval;
    char_u	*shname;
    long	i;

    wcscpy(itmp, L"");
    اگر (GetTempPathW(_MAX_PATH, wszTempFile) == 0)
    {
	wszTempFile[0] = L'.';	// GetTempPathW() failed, use current dir
	wszTempFile[1] = L'\\';
	wszTempFile[2] = NUL;
    }
    wcscpy(buf4, L"VIM");

    // randomize the name to avoid collisions
    i = mch_get_pid() + extra_char;
    buf4[1] = chartab[i % 36];
    buf4[2] = chartab[(i / 36) % 36];
    اگر (GetTempFileNameW(wszTempFile, buf4, 0, itmp) == 0)
	بازگشت پوچ;
    اگر (!keep)
	// GetTempFileNameW() will create the file, we don't want that
	(تهی)DeleteFileW(itmp);

    // Backslashes in a temp file name cause problems when filtering with
    // "sh".  NOTE: This also checks 'shellcmdflag' to help those people who
    // didn't set 'shellslash' but only if not using PowerShell.
    retval = utf16_to_enc(itmp, پوچ);
    اگر (retval == پوچ)
	بازگشت پوچ;
    shname = gettail(p_sh);
    اگر ((*p_shcf == '-' && !(strstr((نویسه *)shname, "powershell") != پوچ
			     || strstr((نویسه *)shname, "pwsh") != پوچ ))
								    || p_ssl)
	برای (p = retval; *p; ++p)
	    اگر (*p == '\\')
		*p = '/';
    بازگشت retval;

# وگرنه // MSWIN
    صحیح		itmplen;

#  ifdef USE_TMPNAM
    // tmpnam() will make its own name
    p = tmpnam((نویسه *)itmp);
    اگر (p == پوچ || *p == NUL)
	بازگشت پوچ;
#  وگرنه

#   ifdef VMS_TEMPNAM
    // mktemp() is not working on VMS.  It seems to be
    // a do-nothing function. Therefore we use tempnam().
    vim_snprintf((نویسه *)itmp, اندازه(itmp), "VIM%c", extra_char);
    p = (char_u *)tempnam("tmp:", (نویسه *)itmp);
    اگر (p != پوچ)
    {
	// VMS will use '.LIS' if we don't explicitly specify an extension,
	// and VIM will then be unable to find the file later
	itmplen = vim_snprintf((نویسه *)itmp, اندازه(itmp), "%s.txt", p);
	آزاد(p);
    }
    وگرنه
	بازگشت پوچ;
#   وگرنه
    STRCPY(itmp, TEMPNAME);
    itmplen = STRLEN_LITERAL(TEMPNAME);
    اگر ((p = vim_strchr(itmp, '?')) != پوچ)
	*p = extra_char;
    اگر (mktemp((نویسه *)itmp) == پوچ)
	بازگشت پوچ;
#   endif
#  endif

    بازگشت vim_strnsave(itmp, (size_t)itmplen);
# endif // MSWIN
#endif // TEMPDIRNAMES
}

#اگر defined(BACKSLASH_IN_FILENAME)
/*
 * Convert all backslashes in fname to forward slashes in-place, unless when
 * it looks like a URL.
 */
    تهی
forward_slash(char_u *fname)
{
    char_u	*p;

    اگر (path_with_url(fname))
	بازگشت;
    برای (p = fname; *p != NUL; ++p)
	// The Big5 encoding can have '\' in the trail byte.
	اگر (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
	    ++p;
	وگرنه اگر (*p == '\\')
	    *p = '/';
}
#endif

/*
 * Try matching a filename with a "pattern" ("prog" is NULL), or use the
 * precompiled regprog "prog" ("pattern" is NULL).  That avoids calling
 * vim_regcomp() often.
 * Used for autocommands and 'wildignore'.
 * Returns TRUE if there is a match, FALSE otherwise.
 */
    صحیح
match_file_pat(
    char_u	*pattern,		// pattern to match with
    regprog_T	**prog,			// pre-compiled regprog or NULL
    char_u	*fname,			// full path of file name
    char_u	*sfname,		// short file name or NULL
    char_u	*tail,			// tail of path
    صحیح		allow_dirs)		// allow matching with dir
{
    regmatch_T	regmatch;
    صحیح		result = FALSE;

    regmatch.rm_ic = p_fic; // ignore case if 'fileignorecase' is set
    اگر (prog != پوچ)
	regmatch.regprog = *prog;
    وگرنه
	regmatch.regprog = vim_regcomp(pattern, RE_MAGIC);

    /*
     * Try for a match with the pattern with:
     * 1. the full file name, when the pattern has a '/'.
     * 2. the short file name, when the pattern has a '/'.
     * 3. the tail of the file name, when the pattern has no '/'.
     */
    اگر (regmatch.regprog != پوچ
	     && ((allow_dirs
		     && (vim_regexec(&regmatch, fname, (colnr_T)0)
			 || (sfname != پوچ
			     && vim_regexec(&regmatch, sfname, (colnr_T)0))))
		 || (!allow_dirs && vim_regexec(&regmatch, tail, (colnr_T)0))))
	result = TRUE;

    اگر (prog != پوچ)
	*prog = regmatch.regprog;
    وگرنه
	vim_regfree(regmatch.regprog);
    بازگشت result;
}

/*
 * Return TRUE if a file matches with a pattern in "list".
 * "list" is a comma-separated list of patterns, like 'wildignore'.
 * "sfname" is the short file name or NULL, "ffname" the long file name.
 */
    صحیح
match_file_list(char_u *list, char_u *sfname, char_u *ffname)
{
    char_u	buf[MAXPATHL];
    char_u	*tail;
    char_u	*regpat;
    نویسه	allow_dirs;
    صحیح		match;
    char_u	*p;

    tail = gettail(sfname);

    // try all patterns in 'wildignore'
    p = list;
    وقتی (*p)
    {
	copy_option_part(&p, buf, MAXPATHL, ",");
	regpat = file_pat_to_reg_pat(buf, پوچ, &allow_dirs, FALSE);
	اگر (regpat == پوچ)
	    بشکن;
	match = match_file_pat(regpat, پوچ, ffname, sfname,
						       tail, (صحیح)allow_dirs);
	vim_free(regpat);
	اگر (match)
	    بازگشت TRUE;
    }
    بازگشت FALSE;
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
file_pat_to_reg_pat(
    char_u	*pat,
    char_u	*pat_end,	// first char after pattern or NULL
    نویسه	*allow_dirs,	// Result passed back out in here
    صحیح		no_bslash UNUSED) // Don't use a backward slash as pathsep
{
    صحیح		size = 2; // '^' at start, '$' at end
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    صحیح		i;
    صحیح		nested = 0;
    صحیح		add_dollar = TRUE;

    اگر (allow_dirs != پوچ)
	*allow_dirs = FALSE;
    اگر (pat_end == پوچ)
	pat_end = pat + STRLEN(pat);

    برای (p = pat; p < pat_end; p++)
    {
	انتخاب (*p)
	{
	    حالت '*':
	    حالت '.':
	    حالت ',':
	    حالت '{':
	    حالت '}':
	    حالت '~':
		size += 2;	// extra backslash
		بشکن;
#ifdef BACKSLASH_IN_FILENAME
	    حالت '\\':
	    حالت '/':
		size += 4;	// could become "[\/]"
		بشکن;
#endif
	    پیشفرض:
		size++;
		اگر (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		{
		    ++p;
		    ++size;
		}
		بشکن;
	}
    }
    reg_pat = alloc(size + 1);
    اگر (reg_pat == پوچ)
	بازگشت پوچ;

    i = 0;

    اگر (pat[0] == '*')
	وقتی (pat[0] == '*' && pat < pat_end - 1)
	    pat++;
    وگرنه
	reg_pat[i++] = '^';
    endp = pat_end - 1;
    اگر (endp >= pat && *endp == '*')
    {
	وقتی (endp - pat > 0 && *endp == '*')
	    endp--;
	add_dollar = FALSE;
    }
    برای (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	انتخاب (*p)
	{
	    حالت '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		وقتی (p[1] == '*')	// "**" matches like "*"
		    ++p;
		بشکن;
	    حالت '.':
	    حالت '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		بشکن;
	    حالت '?':
		reg_pat[i++] = '.';
		بشکن;
	    حالت '\\':
		اگر (p[1] == NUL)
		    بشکن;
#ifdef BACKSLASH_IN_FILENAME
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
			اگر (allow_dirs != پوچ)
			    *allow_dirs = TRUE;
			بشکن;
		    }
		}
#endif
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
#ifdef BACKSLASH_IN_FILENAME
			&& no_bslash
#endif
			)
		    reg_pat[i++] = '?';
		وگرنه
		    اگر (*p == ',' || *p == '%' || *p == '#'
			       || vim_isspace(*p) || *p == '{' || *p == '}')
			reg_pat[i++] = *p;
		    وگرنه اگر (*p == '\\' && p[1] == '\\' && p[2] == '{')
		    {
			reg_pat[i++] = '\\';
			reg_pat[i++] = '{';
			p += 2;
		    }
		    وگرنه
		    {
			اگر (allow_dirs != پوچ && vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
				&& (!no_bslash || *p != '\\')
#endif
				)
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			اگر (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
			    reg_pat[i++] = *p++;
			reg_pat[i++] = *p;
		    }
		بشکن;
#ifdef BACKSLASH_IN_FILENAME
	    حالت '/':
		reg_pat[i++] = '[';
		reg_pat[i++] = '\\';
		reg_pat[i++] = '/';
		reg_pat[i++] = ']';
		اگر (allow_dirs != پوچ)
		    *allow_dirs = TRUE;
		بشکن;
#endif
	    حالت '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		بشکن;
	    حالت '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		بشکن;
	    حالت ',':
		اگر (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		وگرنه
		    reg_pat[i++] = ',';
		بشکن;
	    پیشفرض:
		اگر (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		    reg_pat[i++] = *p++;
		وگرنه اگر (allow_dirs != پوچ && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		بشکن;
	}
    }
    اگر (add_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    اگر (nested != 0)
    {
	اگر (nested < 0)
	    emsg(_(e_missing_open_curly));
	وگرنه
	    emsg(_(e_missing_close_curly));
	VIM_CLEAR(reg_pat);
    }
    بازگشت reg_pat;
}

#اگر defined(EINTR)
/*
 * Version of read() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    long
read_eintr(صحیح fd, تهی *buf, size_t bufsize)
{
    long ret;

    برای (;;)
    {
	ret = vim_read(fd, buf, bufsize);
	اگر (ret >= 0 || errno != EINTR)
	    بشکن;
    }
    بازگشت ret;
}

/*
 * Version of write() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    long
write_eintr(صحیح fd, تهی *buf, size_t bufsize)
{
    long    ret = 0;
    long    wlen;

    // Repeat the write() so long it didn't fail, other than being interrupted
    // by a signal.
    وقتی (ret < (long)bufsize)
    {
	wlen = vim_write(fd, (نویسه *)buf + ret, bufsize - ret);
	اگر (wlen < 0)
	{
	    اگر (errno != EINTR)
		بشکن;
	}
	وگرنه
	    ret += wlen;
    }
    بازگشت ret;
}
#endif
