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

#அந்தர்பாவ "vim.h"

#எனில் defined(__TANDEM)
# include <limits.h>		// for SSIZE_MAX
#endif
#எனில் (defined(UNIX) || defined(VMS)) && defined(FEAT_EVAL)
# include <pwd.h>
# include <grp.h>
#endif
#எனில் defined(VMS) && defined(HAVE_XOS_R_H)
# include <x11/xos_r.h>
#endif

// Is there any system that doesn't have access()?
#define USE_MCH_ACCESS

// Bitmask with 0x80 set in each byte of a long_u word, used to detect
// non-ASCII bytes (high bit set) in multiple bytes at once.
#define NONASCII_MASK (((long_u)-1 / 0xFF) * 0x80)

#எனில் defined(__hpux) && !defined(HAVE_DIRFD)
# define dirfd(x) ((x)->__dd_fd)
# define HAVE_DIRFD
#endif

நிலையான char_u *next_fenc(char_u **pp, முழு *alloced);
#ifdef FEAT_EVAL
நிலையான char_u *readfile_charconvert(char_u *fname, char_u *fenc, முழு *fdp);
#endif
#ifdef FEAT_CRYPT
நிலையான char_u *check_for_cryptkey(char_u *cryptkey, char_u *ptr, long *sizep, off_T *filesizep, முழு newfile, char_u *fname, முழு *did_ask);
#endif
நிலையான linenr_T readfile_linenr(linenr_T linecnt, char_u *p, char_u *endp);
நிலையான char_u *check_for_bom(char_u *p, long size, முழு *lenp, முழு flags);

#ifdef FEAT_EVAL
நிலையான முழு readdirex_sort;
#endif

    வெறுமை
filemess(
    buf_T	*buf,
    char_u	*name,
    char_u	*s,
    முழு		attr)
{
    முழு		செய்தி_scroll_save;
    முழு		prev_செய்தி_col = செய்தி_col;
    size_t	len;

    எனில் (செய்தி_silent != 0)
	திரும்பு;
    செய்தி_add_fname(buf, name);	    // put file name in IObuff with quotes

    // If it's extremely long, truncate it.
    len = STRLEN(IObuff);
    எனில் (len > IOSIZE - 100)
    {
	len = IOSIZE - 100;
	IObuff[len] = NUL;
    }

    // Avoid an over-long translation to cause trouble.
    எனில் (*s != NUL)
	STRNCPY(IObuff + len, s, 99);

    /*
     * For the first message may have to start a new line.
     * For further ones overwrite the previous one, reset msg_scroll before
     * calling filemess().
     */
    செய்தி_scroll_save = செய்தி_scroll;
    எனில் (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	செய்தி_scroll = FALSE;
    எனில் (!செய்தி_scroll)	// wait a bit when overwriting an error msg
	check_for_delay(FALSE);
    செய்தி_start();
    எனில் (prev_செய்தி_col != 0 && செய்தி_col == 0)
	செய்தி_putchar('\r');  // overwrite any previous message.
    செய்தி_scroll = செய்தி_scroll_save;
    செய்தி_scrolled_ign = TRUE;
    // may truncate the message to avoid a hit-return prompt
    செய்தி_outtrans_attr(செய்தி_may_trunc(FALSE, IObuff), attr);
    செய்தி_clr_eos();
    out_flush();
    செய்தி_scrolled_ign = FALSE;
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
    முழு
readfile(
    char_u	*fname,
    char_u	*sfname,
    linenr_T	from,
    linenr_T	வரிகள்_to_skip,
    linenr_T	வரிகள்_to_read,
    exarg_T	*eap,			// can be NULL!
    முழு		flags)
{
    முழு		retval = FAIL;	// jump to "theend" instead of returning
    முழு		fd = 0;
    முழு		newfile = (flags & READ_NEW);
    முழு		check_readonly;
    முழு		filtering = (flags & READ_FILTER);
    முழு		read_stdin = (flags & READ_STDIN);
    முழு		read_buffer = (flags & READ_BUFFER);
    முழு		read_fifo = (flags & READ_FIFO);
    முழு		set_options = newfile || read_buffer
					   || (eap != வெற்று && eap->read_edit);
    linenr_T	read_buf_lnum = 1;	// next line to read from curbuf
    colnr_T	read_buf_col = 0;	// next char to read from this line
    char_u	c;
    linenr_T	lnum = from;
    char_u	*ptr = வெற்று;		// pointer into read buffer
    char_u	*buffer = வெற்று;		// read buffer
    char_u	*new_buffer = வெற்று;	// init to shut up gcc
    char_u	*வரி_start = வெற்று;	// init to shut up gcc
    முழு		wasempty;		// buffer was empty before reading
    colnr_T	len;
    long	size = 0;
    char_u	*p;
    off_T	filesize = 0;
    முழு		skip_read = FALSE;
#ifdef FEAT_CRYPT
    off_T       filesize_disk = 0;      // file size read from disk
    off_T       filesize_மொத்தம் = 0;     // counter
    char_u	*cryptkey = வெற்று;
    முழு		did_ask_for_சாவி = FALSE;
#endif
#ifdef FEAT_PERSISTENT_UNDO
    context_sha256_T sha_ctx;
    முழு		read_undo_file = FALSE;
#endif
    முழு		split = 0;		// number of split lines
#define UNKNOWN	 0x0fffffff		// file size is unknown
    linenr_T	linecnt;
    முழு		error = FALSE;		// errors encountered
    முழு		ff_error = EOL_UNKNOWN; // file format with errors
    long	linerest = 0;		// remaining chars in line
#ifdef UNIX
    முழு		perm = 0;
    முழு		swap_mode = -1;		// protection bits for swap file
#இல்லையெனில்
    முழு		perm;
#endif
    முழு		fileformat = 0;		// end-of-line format
    முழு		keep_fileformat = FALSE;
    stat_T	st;
    முழு		file_readonly;
    linenr_T	skip_மொத்தம் = 0;
    linenr_T	read_மொத்தம் = 0;
    முழு		செய்தி_save = செய்தி_scroll;
    linenr_T	read_no_eol_lnum = 0;   // non-zero lnum when last line of
					// last read was missing the eol
    முழு		try_mac;
    முழு		try_dos;
    முழு		try_unix;
    முழு		file_rewind = FALSE;
    முழு		can_retry;
    linenr_T	conv_error = 0;		// line nr with conversion error
    linenr_T	illegal_byte = 0;	// line nr with illegal byte
    முழு		keep_dest_enc = FALSE;	// don't retry when char doesn't fit
					// in destination encoding
    முழு		bad_char_behavior = BAD_REPLACE;
					// BAD_KEEP, BAD_DROP or character to
					// replace with
    char_u	*tmpname = வெற்று;	// name of 'charconvert' output file
    முழு		fio_flags = 0;
    char_u	*fenc;			// fileencoding to use
    முழு		fenc_alloced;		// fenc_next is in allocated memory
    char_u	*fenc_next = வெற்று;	// next item in 'fencs' or NULL
    முழு		advance_fenc = FALSE;
    long	real_size = 0;
#ifdef USE_ICONV
    iconv_t	iconv_fd = (iconv_t)-1;	// descriptor for iconv() or -1
# ifdef FEAT_EVAL
    முழு		did_iconv = FALSE;	// TRUE when iconv() failed and trying
					// 'charconvert' next
# endif
#endif
    முழு		converted = FALSE;	// TRUE if conversion done
    முழு		notconverted = FALSE;	// TRUE if conversion wanted but it
					// wasn't possible
    char_u	conv_rest[CONV_RESTLEN];
    முழு		conv_restlen = 0;	// nr of bytes in conv_rest[]
    pos_T	orig_start;
    buf_T	*old_curbuf;
    char_u	*old_b_ffname;
    char_u	*old_b_fname;
    முழு		using_b_ffname;
    முழு		using_b_fname;
    நிலையான எழுத்து *செய்தி_is_a_directory = N_("is a directory");
#ifdef FEAT_CRYPT
    முழு		eof = FALSE;
#endif
#ifdef FEAT_SODIUM
    முழு		may_need_lseek = FALSE;
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
    எனில் (curbuf->b_ffname == வெற்று
	    && !filtering
	    && fname != வெற்று
	    && vim_strchr(p_cpo, CPO_FNAMER) != வெற்று
	    && !(flags & READ_DUMMY))
    {
	எனில் (set_rw_fname(fname, sfname) == FAIL)
	    செல் theend;
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
    எனில் (sfname == வெற்று)
	sfname = fname;
#எனில் defined(UNIX)
    fname = sfname;
#endif

    /*
     * The BufReadCmd and FileReadCmd events intercept the reading process by
     * executing the associated commands instead.
     */
    எனில் (!filtering && !read_stdin && !read_buffer)
    {
	orig_start = curbuf->b_op_start;

	// Set '[ mark to the line above where the lines go (line 1 if zero).
	curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
	curbuf->b_op_start.col = 0;

	எனில் (newfile)
	{
	    எனில் (apply_autocmds_exarg(EVENT_BUFREADCMD, வெற்று, sfname,
							  FALSE, curbuf, eap))
	    {
		retval = OK;
#ifdef FEAT_EVAL
		எனில் (aborting())
		    retval = FAIL;
#endif
		// The BufReadCmd code usually uses ":read" to get the text and
		// perhaps ":file" to change the buffer name. But we should
		// consider this to work like ":edit", thus reset the
		// BF_NOTEDITED flag.  Then ":write" will work to overwrite the
		// same file.
		எனில் (retval == OK)
		    curbuf->b_flags &= ~BF_NOTEDITED;
		செல் theend;
	    }
	}
	இல்லையெனில் எனில் (apply_autocmds_exarg(EVENT_FILEREADCMD, sfname, sfname,
							    FALSE, வெற்று, eap))
	{
#ifdef FEAT_EVAL
	    retval = aborting() ? FAIL : OK;
#இல்லையெனில்
	    retval = OK;
#endif
	    செல் theend;
	}

	curbuf->b_op_start = orig_start;

	எனில் (flags & READ_NOFILE)
	{
	    // Return NOTDONE instead of FAIL so that BufEnter can be triggered
	    // and other operations don't fail.
	    retval = NOTDONE;
	    செல் theend;
	}
    }

    எனில் ((shortmess(SHM_OVER) || curbuf->b_help) && p_verbose == 0)
	செய்தி_scroll = FALSE;	// overwrite previous file message
    இல்லையெனில்
	செய்தி_scroll = TRUE;	// don't overwrite previous file message

    எனில் (fname != வெற்று && *fname != NUL)
    {
	fnamelen = STRLEN(fname);

	// If the name is too long we might crash further on, quit here.
	எனில் (fnamelen >= MAXPATHL)
	{
	    filemess(curbuf, fname, (char_u *)_("Illegal file name"), 0);
	    செய்தி_end();
	    செய்தி_scroll = செய்தி_save;
	    செல் theend;
	}

	// If the name ends in a path separator, we can't open it.  Check here,
	// because reading the file may actually work, but then creating the
	// swap file may destroy it!  Reported on MS-DOS and Win 95.
	எனில் (after_pathsep(fname, fname + fnamelen))
	{
	    filemess(curbuf, fname, (char_u *)_(செய்தி_is_a_directory), 0);
	    செய்தி_end();
	    செய்தி_scroll = செய்தி_save;
	    retval = NOTDONE;
	    செல் theend;
	}
    }
#எனில் defined(UNIX) || defined(VMS)
    எனில் (!read_stdin && fname != வெற்று)
	perm = mch_getperm(fname);
#endif

    எனில் (!read_stdin && !read_buffer && !read_fifo)
    {
#எனில் defined(UNIX) || defined(VMS)
	எனில் (perm >= 0 && !S_ISREG(perm)		    // not a regular file ...
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
	    எனில் (S_ISDIR(perm))
	    {
		filemess(curbuf, fname, (char_u *)_(செய்தி_is_a_directory), 0);
		retval = NOTDONE;
	    }
	    இல்லையெனில்
		filemess(curbuf, fname, (char_u *)_("is not a file"), 0);
	    செய்தி_end();
	    செய்தி_scroll = செய்தி_save;
	    செல் theend;
	}
#endif
#எனில் defined(MSWIN)
	/*
	 * MS-Windows allows opening a device, but we will probably get stuck
	 * trying to read it.
	 */
	எனில் (!p_odev && mch_nodetype(fname) == முனை_WRITABLE)
	{
	    filemess(curbuf, fname, (char_u *)_("is a device (disabled with 'opendevice' option)"), 0);
	    செய்தி_end();
	    செய்தி_scroll = செய்தி_save;
	    செல் theend;
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
    எனில் (check_readonly && !readonlymode)
	curbuf->b_p_ro = FALSE;

    எனில் (newfile && !read_stdin && !read_buffer && !read_fifo)
    {
	// Remember time of file.
	எனில் (mch_stat((எழுத்து *)fname, &st) >= 0)
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
	இல்லையெனில்
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
    எனில் (read_stdin)
    {
#எனில் defined(MSWIN)
	// Force binary I/O on stdin to avoid CR-LF -> LF conversion.
	setmode(0, O_இருமம்);
#endif
    }
    இல்லையெனில் எனில் (!read_buffer)
    {
#ifdef USE_MCH_ACCESS
	எனில் (
# ifdef UNIX
	    !(perm & 0222) ||
# endif
				mch_access((எழுத்து *)fname, W_OK))
	    file_readonly = TRUE;
	fd = mch_open((எழுத்து *)fname, O_RDONLY | O_EXTRA, 0);
#இல்லையெனில்
	எனில் (!newfile
		|| readonlymode
		|| (fd = mch_open((எழுத்து *)fname, O_RDWR | O_EXTRA, 0)) < 0)
	{
	    file_readonly = TRUE;
	    // try to open ro
	    fd = mch_open((எழுத்து *)fname, O_RDONLY | O_EXTRA, 0);
	}
#endif
    }

    எனில் (fd < 0)			    // cannot open at all
    {
#ifndef UNIX
	முழு	isdir_f;
#endif
	செய்தி_scroll = செய்தி_save;
#ifndef UNIX
	/*
	 * On Amiga we can't open a directory, check here.
	 */
	isdir_f = (mch_isdir(fname));
	perm = mch_getperm(fname);  // check if the file exists
	எனில் (isdir_f)
	{
	    filemess(curbuf, sfname, (char_u *)_(செய்தி_is_a_directory), 0);
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	}
	இல்லையெனில்
#endif
	    எனில் (newfile)
	    {
		எனில் (perm < 0
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
		    எனில் (!bt_dontwrite(curbuf))
		    {
			check_need_swap(newfile);
			// SwapExists autocommand may mess things up
			எனில் (curbuf != old_curbuf
				|| (using_b_ffname
					&& (old_b_ffname != curbuf->b_ffname))
				|| (using_b_fname
					 && (old_b_fname != curbuf->b_fname)))
			{
			    emsg(_(e_autocommands_changed_buffer_or_buffer_name));
			    செல் theend;
			}
		    }
		    எனில் (dir_of_file_exists(fname))
			filemess(curbuf, sfname,
					      (char_u *)new_file_message(), 0);
		    இல்லையெனில்
			filemess(curbuf, sfname,
					   (char_u *)_("[New DIRECTORY]"), 0);
#ifdef FEAT_VIMINFO
		    // Even though this is a new file, it might have been
		    // edited before and deleted.  Get the old marks.
		    check_marks_read();
#endif
		    // Set forced 'fileencoding'.
		    எனில் (eap != வெற்று)
			set_forced_fenc(eap);
		    apply_autocmds_exarg(EVENT_BUFNEWFILE, sfname, sfname,
							  FALSE, curbuf, eap);
		    // remember the current fileformat
		    save_file_ff(curbuf);

#எனில் defined(FEAT_EVAL)
		    எனில் (!aborting())   // autocmds may abort script processing
#endif
			retval = OK;	    // a new file is not an error
		    செல் theend;
		}
		இல்லையெனில்
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

	செல் theend;
    }

    /*
     * Only set the 'ro' flag for readonly files the first time they are
     * loaded.	Help files always get readonly mode
     */
    எனில் ((check_readonly && file_readonly) || curbuf->b_help)
	curbuf->b_p_ro = TRUE;

    எனில் (set_options)
    {
	// Don't change 'eol' if reading from buffer as it will already be
	// correctly set when reading stdin.
	எனில் (!read_buffer)
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
    எனில் (!bt_dontwrite(curbuf))
    {
	check_need_swap(newfile);
	எனில் (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))))
	{
	    emsg(_(e_autocommands_changed_buffer_or_buffer_name));
	    எனில் (!read_buffer)
		close(fd);
	    செல் theend;
	}
#ifdef UNIX
	// Set swap file protection bits after creating it.
	எனில் (swap_mode > 0 && curbuf->b_ml.ml_mfp != வெற்று
			  && curbuf->b_ml.ml_mfp->mf_fname != வெற்று)
	{
	    char_u *swap_fname = curbuf->b_ml.ml_mfp->mf_fname;

	    /*
	     * If the group-read bit is set but not the world-read bit, then
	     * the group must be equal to the group of the original file.  If
	     * we can't make that happen then reset the group-read bit.  This
	     * avoids making the swap file readable to more users when the
	     * primary group of the user is too permissive.
	     */
	    எனில் ((swap_mode & 044) == 040)
	    {
		stat_T	swap_st;

		எனில் (mch_stat((எழுத்து *)swap_fname, &swap_st) >= 0
			&& st.st_gid != swap_st.st_gid
# ifdef HAVE_FCHOWN
			&& fchown(curbuf->b_ml.ml_mfp->mf_fd, -1, st.st_gid)
									  == -1
# endif
		   )
		    swap_mode &= 0600;
	    }

	    (வெறுமை)mch_setperm(swap_fname, (long)swap_mode);
	}
#endif
    }

    // If "Quit" selected at ATTENTION dialog, don't load the file
    எனில் (swap_exists_action == SEA_QUIT)
    {
	எனில் (!read_buffer && !read_stdin)
	    close(fd);
	செல் theend;
    }

    ++no_wait_return;	    // don't wait for return yet

    /*
     * Set '[ mark to the line above where the lines go (line 1 if zero).
     */
    orig_start = curbuf->b_op_start;
    curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
    curbuf->b_op_start.col = 0;

    try_mac = (vim_strchr(p_ffs, 'm') != வெற்று);
    try_dos = (vim_strchr(p_ffs, 'd') != வெற்று);
    try_unix = (vim_strchr(p_ffs, 'x') != வெற்று);

    எனில் (!read_buffer)
    {
	முழு	m = செய்தி_scroll;
	முழு	n = செய்தி_scrolled;

	/*
	 * The file must be closed again, the autocommands may want to change
	 * the file before reading it.
	 */
	எனில் (!read_stdin)
	    close(fd);		// ignore errors

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	செய்தி_scroll = TRUE;
	எனில் (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPRE, வெற்று, sfname,
							  FALSE, curbuf, eap);
	இல்லையெனில் எனில் (read_stdin)
	    apply_autocmds_exarg(EVENT_STDINREADPRE, வெற்று, sfname,
							  FALSE, curbuf, eap);
	இல்லையெனில் எனில் (newfile)
	    apply_autocmds_exarg(EVENT_BUFREADPRE, வெற்று, sfname,
							  FALSE, curbuf, eap);
	இல்லையெனில்
	    apply_autocmds_exarg(EVENT_FILEREADPRE, sfname, sfname,
							    FALSE, வெற்று, eap);
	// autocommands may have changed it
	try_mac = (vim_strchr(p_ffs, 'm') != வெற்று);
	try_dos = (vim_strchr(p_ffs, 'd') != வெற்று);
	try_unix = (vim_strchr(p_ffs, 'x') != வெற்று);
	curbuf->b_op_start = orig_start;

	எனில் (செய்தி_scrolled == n)
	    செய்தி_scroll = m;

#ifdef FEAT_EVAL
	எனில் (aborting())	    // autocmds may abort script processing
	{
	    --no_wait_return;
	    செய்தி_scroll = செய்தி_save;
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    செல் theend;
	}
#endif
	/*
	 * Don't allow the autocommands to change the current buffer.
	 * Try to re-open the file.
	 *
	 * Don't allow the autocommands to change the buffer name either
	 * (cd for example) if it invalidates fname or sfname.
	 */
	எனில் (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))
		|| (fd = mch_open((எழுத்து *)fname, O_RDONLY | O_EXTRA, 0)) < 0))
	{
	    --no_wait_return;
	    செய்தி_scroll = செய்தி_save;
	    எனில் (fd < 0)
		emsg(_(e_readpre_autocommands_made_file_unreadable));
	    இல்லையெனில்
		emsg(_(e_readpre_autocommands_must_not_change_current_buffer));
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    செல் theend;
	}
    }

    // Autocommands may add lines to the file, need to check if it is empty
    wasempty = (curbuf->b_ml.ml_flags & ML_EMPTY);

    எனில் (!recoverymode && !filtering && !(flags & READ_DUMMY))
    {
	/*
	 * Show the user that we are busy reading the input.  Sometimes this
	 * may take a while.  When reading from stdin another program may
	 * still be running, don't move the cursor to the last line, unless
	 * always using the GUI.
	 */
	எனில் (read_stdin)
	{
	    எனில் (!is_not_a_term())
	    {
#ifndef ALWAYS_USE_GUI
# ifdef VIMDLL
		எனில் (!gui.in_use)
# endif
		    mch_செய்தி(_("Vim: Reading from stdin...\n"));
#endif
#ifdef FEAT_GUI
		// Also write a message in the GUI window, if there is one.
		எனில் (gui.in_use && !gui.dying && !gui.starting)
		{
		    size_t  plen = STRLEN(_("Reading from stdin..."));

		    // make a copy, gui_write() may try to change it
		    p = vim_strnsave((char_u *)_("Reading from stdin..."), plen);
		    எனில் (p != வெற்று)
		    {
			gui_write(p, (முழு)plen);
			vim_free(p);
		    }
		}
#endif
	    }
	}
	இல்லையெனில் எனில் (!read_buffer)
	    filemess(curbuf, sfname, (char_u *)"", 0);
    }

    செய்தி_scroll = FALSE;			// overwrite the file message

    /*
     * Set linecnt now, before the "retry" caused by a wrong guess for
     * fileformat, and after the autocommands, which may change them.
     */
    linecnt = curbuf->b_ml.ml_வரி_மொத்தம்;

    // "++bad=" argument.
    எனில் (eap != வெற்று && eap->bad_char != 0)
    {
	bad_char_behavior = eap->bad_char;
	எனில் (set_options)
	    curbuf->b_bad_char = eap->bad_char;
    }
    இல்லையெனில்
	curbuf->b_bad_char = 0;

    /*
     * Decide which 'encoding' to use or use first.
     */
    எனில் (eap != வெற்று && eap->force_enc != 0)
    {
	fenc = enc_canonize(eap->cmd + eap->force_enc);
	fenc_alloced = TRUE;
	keep_dest_enc = TRUE;
    }
    இல்லையெனில் எனில் (curbuf->b_p_bin)
    {
	fenc = (char_u *)"";		// binary: don't convert
	fenc_alloced = FALSE;
    }
    இல்லையெனில் எனில் (curbuf->b_help)
    {
	char_u	    firstline[80];
	முழு	    fc;

	// Help files are either utf-8 or latin1.  Try utf-8 first, if this
	// fails it must be latin1.
	// Always do this when 'encoding' is "utf-8".  Otherwise only do
	// this when needed to avoid [converted] remarks all the time.
	// It is needed when the first line contains non-ASCII characters.
	// That is only in *.??x files.
	fenc = (char_u *)"latin1";
	c = enc_utf8;
	எனில் (!c && !read_stdin)
	{
	    fc = fname[fnamelen - 1];
	    எனில் (TOLOWER_ASC(fc) == 'x')
	    {
		// Read the first line (and a bit more).  Immediately rewind to
		// the start of the file.  If the read() fails "len" is -1.
		len = read_eintr(fd, firstline, 80);
		vim_lseek(fd, (off_T)0L, SEEK_SET);
		க்கு (p = firstline; p < firstline + len; ++p)
		    எனில் (*p >= 0x80)
		    {
			c = TRUE;
			நிறுத்து;
		    }
	    }
	}

	எனில் (c)
	{
	    fenc_next = fenc;
	    fenc = (char_u *)"utf-8";

	    // When the file is utf-8 but a character doesn't fit in
	    // 'encoding' don't retry.  In help text editing utf-8 bytes
	    // doesn't make sense.
	    எனில் (!enc_utf8)
		keep_dest_enc = TRUE;
	}
	fenc_alloced = FALSE;
    }
    இல்லையெனில் எனில் (*p_fencs == NUL)
    {
	fenc = curbuf->b_p_fenc;	// use format from buffer
	fenc_alloced = FALSE;
    }
    இல்லையெனில்
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

    எனில் (file_rewind)
    {
	எனில் (read_buffer)
	{
	    read_buf_lnum = 1;
	    read_buf_col = 0;
	}
	இல்லையெனில் எனில் (read_stdin || vim_lseek(fd, (off_T)0L, SEEK_SET) != 0)
	{
	    // Can't rewind the file, give up.
	    error = TRUE;
	    செல் failed;
	}
	// Delete the previously read lines.
	போது (lnum > from)
	    ml_delete(lnum--);
	file_rewind = FALSE;
	எனில் (set_options)
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
    எனில் (keep_fileformat)
	keep_fileformat = FALSE;
    இல்லையெனில்
    {
	எனில் (eap != வெற்று && eap->force_ff != 0)
	{
	    fileformat = get_fileformat_force(curbuf, eap);
	    try_unix = try_dos = try_mac = FALSE;
	}
	இல்லையெனில் எனில் (curbuf->b_p_bin)
	    fileformat = EOL_UNIX;		// binary: use Unix format
	இல்லையெனில் எனில் (*p_ffs == NUL)
	    fileformat = get_fileformat(curbuf);// use format from buffer
	இல்லையெனில்
	    fileformat = EOL_UNKNOWN;		// detect from file
    }

#ifdef USE_ICONV
    எனில் (iconv_fd != (iconv_t)-1)
    {
	// aborted conversion with iconv(), close the descriptor
	iconv_close(iconv_fd);
	iconv_fd = (iconv_t)-1;
    }
#endif

    எனில் (advance_fenc)
    {
	/*
	 * Try the next entry in 'fileencodings'.
	 */
	advance_fenc = FALSE;

	எனில் (eap != வெற்று && eap->force_enc != 0)
	{
	    // Conversion given with "++cc=" wasn't possible, read
	    // without conversion.
	    notconverted = TRUE;
	    conv_error = 0;
	    எனில் (fenc_alloced)
		vim_free(fenc);
	    fenc = (char_u *)"";
	    fenc_alloced = FALSE;
	}
	இல்லையெனில்
	{
	    எனில் (fenc_alloced)
		vim_free(fenc);
	    எனில் (fenc_next != வெற்று)
	    {
		fenc = next_fenc(&fenc_next, &fenc_alloced);
	    }
	    இல்லையெனில்
	    {
		fenc = (char_u *)"";
		fenc_alloced = FALSE;
	    }
	}
	எனில் (tmpname != வெற்று)
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
    எனில் (converted)
    {
	// "ucs-bom" means we need to check the first bytes of the file
	// for a BOM.
	எனில் (STRCMP(fenc, ENC_UCSBOM) == 0)
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
	இல்லையெனில் எனில் (enc_utf8 || STRCMP(p_enc, "latin1") == 0)
	    fio_flags = get_fio_flags(fenc);

#ifdef MSWIN
	/*
	 * Conversion from an MS-Windows codepage to UTF-8 or another codepage
	 * is handled with MultiByteToWideChar().
	 */
	எனில் (fio_flags == 0)
	    fio_flags = get_win_fio_flags(fenc);
#endif

#ifdef MACOS_CONVERT
	// Conversion from Apple MacRoman to latin1 or UTF-8
	எனில் (fio_flags == 0)
	    fio_flags = get_mac_fio_flags(fenc);
#endif

#ifdef USE_ICONV
	/*
	 * Try using iconv() if we can't convert internally.
	 */
	எனில் (fio_flags == 0
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
	எனில் (fio_flags == 0 && !read_stdin && !read_buffer && *p_ccv != NUL
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
	    எனில் (tmpname == வெற்று)
	    {
		tmpname = readfile_charconvert(fname, fenc, &fd);
		எனில் (tmpname == வெற்று)
		{
		    // Conversion failed.  Try another one.
		    advance_fenc = TRUE;
		    எனில் (fd < 0)
		    {
			// Re-opening the original file failed!
			emsg(_(e_conversion_mad_file_unreadable));
			error = TRUE;
			செல் failed;
		    }
		    செல் retry;
		}
	    }
	}
	இல்லையெனில்
#endif
	{
	    எனில் (fio_flags == 0
#ifdef USE_ICONV
		    && iconv_fd == (iconv_t)-1
#endif
	       )
	    {
		// Conversion wanted but we can't.
		// Try the next conversion in 'fileencodings'
		advance_fenc = TRUE;
		செல் retry;
	    }
	}
    }

    // Set "can_retry" when it's possible to rewind the file and try with
    // another "fenc" value.  It's FALSE when no other "fenc" to try, reading
    // stdin or fixed at a specific encoding.
    can_retry = (*fenc != NUL && !read_stdin && !read_fifo && !keep_dest_enc);

    எனில் (!skip_read)
    {
	linerest = 0;
	filesize = 0;
#ifdef FEAT_CRYPT
	filesize_மொத்தம் = 0;
#endif
	skip_மொத்தம் = வரிகள்_to_skip;
	read_மொத்தம் = வரிகள்_to_read;
	conv_restlen = 0;
#ifdef FEAT_PERSISTENT_UNDO
	read_undo_file = (newfile && (flags & READ_KEEP_UNDO) == 0
				  && curbuf->b_ffname != வெற்று
				  && curbuf->b_p_udf
				  && !filtering
				  && !read_fifo
				  && !read_stdin
				  && !read_buffer);
	எனில் (read_undo_file)
	    sha256_start(&sha_ctx);
#endif
#ifdef FEAT_CRYPT
	எனில் (curbuf->b_cryptstate != வெற்று)
	{
	    // Need to free the state, but keep the key, don't want to ask for
	    // it again.
	    crypt_free_state(curbuf->b_cryptstate);
	    curbuf->b_cryptstate = வெற்று;
	}
#endif
    }

    போது (!error && !got_int)
    {
	/*
	 * We allocate as much space for the file as we can get, plus
	 * space for the old line plus room for one terminating NUL.
	 * The amount is limited by the fact that read() only can read
	 * up to max_unsigned characters (and other things).
	 */
	எனில் (!skip_read)
	{
#எனில் defined(SSIZE_MAX) && (SSIZE_MAX < 0x10000L)
		size = SSIZE_MAX;		    // use max I/O size, 52K
#இல்லையெனில்
		// Use buffer >= 64K.  Add linerest to double the size if the
		// line gets very long, to avoid a lot of copying. But don't
		// read more than 1 Mbyte at a time, so we can be interrupted.
		size = 0x10000L + linerest;
		எனில் (size > 0x100000L)
		    size = 0x100000L;
#endif
	}

	// Protect against the argument of lalloc() going negative.
	// Also split lines that are too long for colnr_T.  After this check
	// passes, we read up to 'size' more bytes.  We must ensure that even
	// after that read, the line length won't exceed MAXCOL - 1 (because
	// we add 1 for the NUL when casting to colnr_T).  If this check fires,
	// we insert a synthetic newline immediately, so linerest doesn't grow.
	எனில் (size < 0 || size + linerest + 1 < 0 || linerest >= MAXCOL - size)
	{
	    ++split;
	    *ptr = NL;		    // split line by inserting a NL
	    size = 1;
	}
	இல்லையெனில்
	{
	    எனில் (!skip_read)
	    {
		க்கு ( ; size >= 10; size = (long)((long_u)size >> 1))
		{
		    எனில் ((new_buffer = lalloc(size + linerest + 1,
							      FALSE)) != வெற்று)
			நிறுத்து;
		}
		எனில் (new_buffer == வெற்று)
		{
		    do_outofmem_செய்தி((long_u)(size * 2 + linerest + 1));
		    error = TRUE;
		    நிறுத்து;
		}
		எனில் (linerest)	// copy characters from the previous buffer
		    mch_memmove(new_buffer, ptr - linerest, (size_t)linerest);
		vim_free(buffer);
		buffer = new_buffer;
		ptr = buffer + linerest;
		வரி_start = buffer;

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
		real_size = (முழு)size;
#ifdef USE_ICONV
		எனில் (iconv_fd != (iconv_t)-1)
		    size = size / ICONV_MULT;
		இல்லையெனில்
#endif
		    எனில் (fio_flags & FIO_LATIN1)
		    size = size / 2;
		இல்லையெனில் எனில் (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    size = (size * 2 / 3) & ~1;
		இல்லையெனில் எனில் (fio_flags & FIO_UCS4)
		    size = (size * 2 / 3) & ~3;
		இல்லையெனில் எனில் (fio_flags == FIO_UCSBOM)
		    size = size / ICONV_MULT;	// worst case
#ifdef MSWIN
		இல்லையெனில் எனில் (fio_flags & FIO_CODEPAGE)
		    size = size / ICONV_MULT;	// also worst case
#endif
#ifdef MACOS_CONVERT
		இல்லையெனில் எனில் (fio_flags & FIO_MACROMAN)
		    size = size / ICONV_MULT;	// also worst case
#endif

		எனில் (conv_restlen > 0)
		{
		    // Insert unconverted bytes from previous line.
		    mch_memmove(ptr, conv_rest, conv_restlen);
		    ptr += conv_restlen;
		    size -= conv_restlen;
		}

		எனில் (read_buffer)
		{
		    /*
		     * Read bytes from curbuf.  Used for converting text read
		     * from stdin.
		     */
		    எனில் (read_buf_lnum > from)
			size = 0;
		    இல்லையெனில்
		    {
			முழு	n, ni;
			long	tlen;

			tlen = 0;
			க்கு (;;)
			{
			    p = ml_get(read_buf_lnum) + read_buf_col;
			    n = ml_get_len(read_buf_lnum) - read_buf_col;
			    எனில் ((முழு)tlen + n + 1 > size)
			    {
				// Filled up to "size", append partial line.
				// Change NL to NUL to reverse the effect done
				// below.
				n = (முழு)(size - tlen);
				க்கு (ni = 0; ni < n; ++ni)
				{
				    எனில் (p[ni] == NL)
					ptr[tlen++] = NUL;
				    இல்லையெனில்
					ptr[tlen++] = p[ni];
				}
				read_buf_col += n;
				நிறுத்து;
			    }

			    // Append whole line and new-line.  Change NL
			    // to NUL to reverse the effect done below.
			    க்கு (ni = 0; ni < n; ++ni)
			    {
				எனில் (p[ni] == NL)
				    ptr[tlen++] = NUL;
				இல்லையெனில்
				    ptr[tlen++] = p[ni];
			    }
			    ptr[tlen++] = NL;
			    read_buf_col = 0;
			    எனில் (++read_buf_lnum > from)
			    {
				// When the last line didn't have an
				// end-of-line don't add it now either.
				எனில் (!curbuf->b_p_eol)
				    --tlen;
				size = tlen;
#ifdef FEAT_CRYPT
				eof = TRUE;
#endif
				நிறுத்து;
			    }
			}
		    }
		}
		இல்லையெனில்
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
		    எனில் (filesize == 0)
		    {
			// set size to 8K + Sodium Crypt Metadata
			size = WRITEBUFSIZE + crypt_get_max_header_len()
			    + crypto_secretstream_xchacha20poly1305_HEADERBYTES
				+ crypto_secretstream_xchacha20poly1305_ABYTES;
			may_need_lseek = TRUE;
		    }

		    இல்லையெனில் எனில் (filesize > 0 && (curbuf->b_cryptstate != வெற்று
				&& crypt_method_is_sodium(
					     curbuf->b_cryptstate->method_nr)))
		    {
			size = WRITEBUFSIZE + crypto_secretstream_xchacha20poly1305_ABYTES;
			// need to rewind by - add_len from CRYPT_M_SOD2 (see
			// description above)
			எனில் (curbuf->b_cryptstate->method_nr == CRYPT_M_SOD
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
		    filesize_மொத்தம் += size;
		    eof = (size < read_size || filesize_மொத்தம் == filesize_disk);
#endif
		}

#ifdef FEAT_CRYPT
		/*
		 * At start of file: Check for magic number of encryption.
		 */
		எனில் (filesize == 0 && size > 0)
		{
		    cryptkey = check_for_cryptkey(cryptkey, ptr, &size,
						  &filesize, newfile, sfname,
						  &did_ask_for_சாவி);
# எனில் defined(CRYPT_NOT_INPLACE) && defined(FEAT_PERSISTENT_UNDO)
		    எனில் (curbuf->b_cryptstate != வெற்று
				 && !crypt_works_inplace(curbuf->b_cryptstate))
			// reading undo file requires crypt_decode_inplace()
			read_undo_file = FALSE;
# endif
		}
		/*
		 * Decrypt the read bytes.  This is done before checking for
		 * EOF because the crypt layer may be buffering.
		 */
		எனில் (cryptkey != வெற்று && curbuf->b_cryptstate != வெற்று
								   && size > 0)
		{
# ifdef CRYPT_NOT_INPLACE
		    எனில் (crypt_works_inplace(curbuf->b_cryptstate))
		    {
# endif
			crypt_decode_inplace(curbuf->b_cryptstate, ptr,
								    size, eof);
# ifdef CRYPT_NOT_INPLACE
		    }
		    இல்லையெனில்
		    {
			char_u	*newptr = வெற்று;
			முழு	decrypted_size;

			decrypted_size = crypt_decode_alloc(
				    curbuf->b_cryptstate, ptr, size,
								 &newptr, eof);

			எனில் (decrypted_size < 0)
			{
			    // error message already given
			    error = TRUE;
			    vim_free(newptr);
			    நிறுத்து;
			}
			// If the crypt layer is buffering, not producing
			// anything yet, need to read more.
			எனில் (decrypted_size == 0)
			    தொடர்;

			எனில் (linerest == 0)
			{
			    // Simple case: reuse returned buffer (may be
			    // NULL, checked later).
			    new_buffer = newptr;
			}
			இல்லையெனில்
			{
			    long_u	new_size;

			    // Need new buffer to add bytes carried over.
			    new_size = (long_u)(decrypted_size + linerest + 1);
			    new_buffer = lalloc(new_size, FALSE);
			    எனில் (new_buffer == வெற்று)
			    {
				do_outofmem_செய்தி(new_size);
				error = TRUE;
				நிறுத்து;
			    }

			    mch_memmove(new_buffer, buffer, linerest + conv_restlen);
			    எனில் (newptr != வெற்று)
				mch_memmove(new_buffer + linerest + conv_restlen,
					newptr, decrypted_size);
			    vim_free(newptr);
			}

			எனில் (new_buffer != வெற்று)
			{
			    vim_free(buffer);
			    buffer = new_buffer;
			    new_buffer = வெற்று;
			    வரி_start = buffer;
			    ptr = buffer + linerest + conv_restlen;
			    real_size = size;
			}
			size = decrypted_size;
		    }
# endif
		}
#endif

		எனில் (size <= 0)
		{
		    எனில் (size < 0)		    // read error
			error = TRUE;
		    இல்லையெனில் எனில் (conv_restlen > 0)
		    {
			/*
			 * Reached end-of-file but some trailing bytes could
			 * not be converted.  Truncated file?
			 */

			// When we did a conversion report an error.
			எனில் (fio_flags != 0
#ifdef USE_ICONV
				|| iconv_fd != (iconv_t)-1
#endif
			   )
			{
			    எனில் (can_retry)
				செல் rewind_retry;
			    எனில் (conv_error == 0)
				conv_error = curbuf->b_ml.ml_வரி_மொத்தம்
								- linecnt + 1;
			}
			// Remember the first linenr with an illegal byte
			இல்லையெனில் எனில் (illegal_byte == 0)
			    illegal_byte = curbuf->b_ml.ml_வரி_மொத்தம்
								- linecnt + 1;
			எனில் (bad_char_behavior == BAD_DROP)
			{
			    *(ptr - conv_restlen) = NUL;
			    conv_restlen = 0;
			}
			இல்லையெனில்
			{
			    // Replace the trailing bytes with the replacement
			    // character if we were converting; if we weren't,
			    // leave the UTF8 checking code to do it, as it
			    // works slightly differently.
			    எனில் (bad_char_behavior != BAD_KEEP && (fio_flags != 0
#ifdef USE_ICONV
				    || iconv_fd != (iconv_t)-1
#endif
			       ))
			    {
				போது (conv_restlen > 0)
				{
				    *(--ptr) = bad_char_behavior;
				    --conv_restlen;
				}
			    }
			    fio_flags = 0;	// don't convert this
#ifdef USE_ICONV
			    எனில் (iconv_fd != (iconv_t)-1)
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
	    எனில் ((filesize == 0
#ifdef FEAT_CRYPT
		   || (cryptkey != வெற்று
			&& filesize == crypt_get_header_len(
						 crypt_get_method_nr(curbuf)))
#endif
		       )
		    && (fio_flags == FIO_UCSBOM
			|| (!curbuf->b_p_bomb
			    && tmpname == வெற்று
			    && (*fenc == 'u' || (*fenc == NUL && enc_utf8)))))
	    {
		char_u	*ccname;
		முழு	blen;

		// no BOM detection in a short file or in binary mode
		எனில் (size < 2 || curbuf->b_p_bin)
		    ccname = வெற்று;
		இல்லையெனில்
		    ccname = check_for_bom(ptr, size, &blen,
		      fio_flags == FIO_UCSBOM ? FIO_ALL : get_fio_flags(fenc));
		எனில் (ccname != வெற்று)
		{
		    // Remove BOM from the text
		    filesize += blen;
		    size -= blen;
		    mch_memmove(ptr, ptr + blen, (size_t)size);
		    எனில் (set_options)
		    {
			curbuf->b_p_bomb = TRUE;
			curbuf->b_start_bomb = TRUE;
		    }
		}

		எனில் (fio_flags == FIO_UCSBOM)
		{
		    எனில் (ccname == வெற்று)
		    {
			// No BOM detected: retry with next encoding.
			advance_fenc = TRUE;
		    }
		    இல்லையெனில்
		    {
			// BOM detected: set "fenc" and jump back
			எனில் (fenc_alloced)
			    vim_free(fenc);
			fenc = ccname;
			fenc_alloced = FALSE;
		    }
		    // retry reading without getting new bytes or rewinding
		    skip_read = TRUE;
		    செல் retry;
		}
	    }

	    // Include not converted bytes.
	    ptr -= conv_restlen;
	    size += conv_restlen;
	    conv_restlen = 0;
	    /*
	     * Break here for a read error or end-of-file.
	     */
	    எனில் (size <= 0)
		நிறுத்து;


#ifdef USE_ICONV
	    எனில் (iconv_fd != (iconv_t)-1)
	    {
		/*
		 * Attempt conversion of the read bytes to 'encoding' using
		 * iconv().
		 */
		மாறிலி எழுத்து	*fromp;
		எழுத்து		*உச்சி;
		size_t		from_size;
		size_t		to_size;

		fromp = (எழுத்து *)ptr;
		from_size = size;
		ptr += size;
		உச்சி = (எழுத்து *)ptr;
		to_size = real_size - size;

		/*
		 * If there is conversion error or not enough room try using
		 * another conversion.  Except for when there is no
		 * alternative (help files).
		 */
		போது ((iconv(iconv_fd, (வெறுமை *)&fromp, &from_size,
							       &உச்சி, &to_size)
			    == (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						  || from_size > CONV_RESTLEN)
		{
		    எனில் (can_retry)
			செல் rewind_retry;
		    எனில் (conv_error == 0)
			conv_error = readfile_linenr(linecnt,
							  ptr, (char_u *)உச்சி);

		    // Deal with a bad byte and continue with the next.
		    ++fromp;
		    --from_size;
		    எனில் (bad_char_behavior == BAD_KEEP)
		    {
			*உச்சி++ = *(fromp - 1);
			--to_size;
		    }
		    இல்லையெனில் எனில் (bad_char_behavior != BAD_DROP)
		    {
			*உச்சி++ = bad_char_behavior;
			--to_size;
		    }
		}

		எனில் (from_size > 0)
		{
		    // Some remaining characters, keep them for the next
		    // round.
		    mch_memmove(conv_rest, (char_u *)fromp, from_size);
		    conv_restlen = (முழு)from_size;
		}

		// move the linerest to before the converted characters
		வரி_start = ptr - linerest;
		mch_memmove(வரி_start, buffer, (size_t)linerest);
		size = (long)((char_u *)உச்சி - ptr);
	    }
#endif

#ifdef MSWIN
	    எனில் (fio_flags & FIO_CODEPAGE)
	    {
		char_u	*src, *dst;
		WCHAR	ucs2buf[3];
		முழு	ucs2len;
		முழு	codepage = FIO_GET_CP(fio_flags);
		முழு	bytelen;
		முழு	found_bad;
		எழுத்து	replstr[2];

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
		எனில் (bad_char_behavior > 0)
		    replstr[0] = bad_char_behavior;
		இல்லையெனில்
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
		போது (size > 0)
		{
		    found_bad = FALSE;

# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
		    எனில் (codepage == CP_UTF8)
		    {
			// Handle CP_UTF8 input ourselves to be able to handle
			// trailing bytes properly.
			// Get one UTF-8 character from src.
			bytelen = (முழு)utf_ptr2len_len(src, size);
			எனில் (bytelen > size)
			{
			    // Only got some bytes of a character.  Normally
			    // it's put in "conv_rest", but if it's too long
			    // deal with it as if they were illegal bytes.
			    எனில் (bytelen <= CONV_RESTLEN)
				நிறுத்து;

			    // weird overlong byte sequence
			    bytelen = size;
			    found_bad = TRUE;
			}
			இல்லையெனில்
			{
			    முழு	    u8c = utf_ptr2char(src);

			    எனில் (u8c > 0xffff || (*src >= 0x80 && bytelen == 1))
				found_bad = TRUE;
			    ucs2buf[0] = u8c;
			    ucs2len = 1;
			}
		    }
		    இல்லையெனில்
# endif
		    {
			// We don't know how long the byte sequence is, try
			// from one to three bytes.
			க்கு (bytelen = 1; bytelen <= size && bytelen <= 3;
								    ++bytelen)
			{
			    ucs2len = MultiByteToWideChar(codepage,
							 MB_ERR_INVALID_CHARS,
							 (LPCSTR)src, bytelen,
								   ucs2buf, 3);
			    எனில் (ucs2len > 0)
				நிறுத்து;
			}
			எனில் (ucs2len == 0)
			{
			    // If we have only one byte then it's probably an
			    // incomplete byte sequence.  Otherwise discard
			    // one byte as a bad character.
			    எனில் (size == 1)
				நிறுத்து;
			    found_bad = TRUE;
			    bytelen = 1;
			}
		    }

		    எனில் (!found_bad)
		    {
			முழு	i;

			// Convert "ucs2buf[ucs2len]" to 'enc' in "dst".
			எனில் (enc_utf8)
			{
			    // From UCS-2 to UTF-8.  Cannot fail.
			    க்கு (i = 0; i < ucs2len; ++i)
				dst += utf_char2bytes(ucs2buf[i], dst);
			}
			இல்லையெனில்
			{
			    BOOL	bad = FALSE;
			    முழு		dstlen;

			    // From UCS-2 to "enc_codepage".  If the
			    // conversion uses the default character "?",
			    // the data doesn't fit in this encoding.
			    dstlen = WideCharToMultiByte(enc_codepage, 0,
				    (LPCWSTR)ucs2buf, ucs2len,
				    (LPSTR)dst, (முழு)(src - dst),
				    replstr, &bad);
			    எனில் (bad)
				found_bad = TRUE;
			    இல்லையெனில்
				dst += dstlen;
			}
		    }

		    எனில் (found_bad)
		    {
			// Deal with bytes we can't convert.
			எனில் (can_retry)
			    செல் rewind_retry;
			எனில் (conv_error == 0)
			    conv_error = readfile_linenr(linecnt, ptr, dst);
			எனில் (bad_char_behavior != BAD_DROP)
			{
			    எனில் (bad_char_behavior == BAD_KEEP)
			    {
				mch_memmove(dst, src, bytelen);
				dst += bytelen;
			    }
			    இல்லையெனில்
				*dst++ = bad_char_behavior;
			}
		    }

		    src += bytelen;
		    size -= bytelen;
		}

		எனில் (size > 0)
		{
		    // An incomplete byte sequence remaining.
		    mch_memmove(conv_rest, src, size);
		    conv_restlen = size;
		}

		// The new size is equal to how much "dst" was advanced.
		size = (long)(dst - ptr);
	    }
	    இல்லையெனில்
#endif
#ifdef MACOS_CONVERT
	    எனில் (fio_flags & FIO_MACROMAN)
	    {
		/*
		 * Conversion from Apple MacRoman char encoding to UTF-8 or
		 * latin1.  This is in os_mac_conv.c.
		 */
		எனில் (macroman2enc(ptr, &size, real_size) == FAIL)
		    செல் rewind_retry;
	    }
	    இல்லையெனில்
#endif
	    எனில் (fio_flags != 0)
	    {
		முழு	u8c;
		char_u	*dest;
		char_u	*tail = வெற்று;

		/*
		 * "enc_utf8" set: Convert Unicode or Latin1 to UTF-8.
		 * "enc_utf8" not set: Convert Unicode to Latin1.
		 * Go from end to start through the buffer, because the number
		 * of bytes may increase.
		 * "dest" points to after where the UTF-8 bytes go, "p" points
		 * to after the next character to convert.
		 */
		dest = ptr + real_size;
		எனில் (fio_flags == FIO_LATIN1 || fio_flags == FIO_UTF8)
		{
		    p = ptr + size;
		    எனில் (fio_flags == FIO_UTF8)
		    {
			// Check for a trailing incomplete UTF-8 sequence
			tail = ptr + size - 1;
			போது (tail > ptr && (*tail & 0xc0) == 0x80)
			    --tail;
			எனில் (tail + utf_byte2len(*tail) <= ptr + size)
			    tail = வெற்று;
			இல்லையெனில்
			    p = tail;
		    }
		}
		இல்லையெனில் எனில் (fio_flags & (FIO_UCS2 | FIO_UTF16))
		{
		    // Check for a trailing byte
		    p = ptr + (size & ~1);
		    எனில் (size & 1)
			tail = p;
		    எனில் ((fio_flags & FIO_UTF16) && p > ptr)
		    {
			// Check for a trailing leading word
			எனில் (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			இல்லையெனில்
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			எனில் (u8c >= 0xd800 && u8c <= 0xdbff)
			    tail = p;
			இல்லையெனில்
			    p += 2;
		    }
		}
		இல்லையெனில் //  FIO_UCS4
		{
		    // Check for trailing 1, 2 or 3 bytes
		    p = ptr + (size & ~3);
		    எனில் (size & 3)
			tail = p;
		}

		// If there is a trailing incomplete sequence move it to
		// conv_rest[].
		எனில் (tail != வெற்று)
		{
		    conv_restlen = (முழு)((ptr + size) - tail);
		    mch_memmove(conv_rest, (char_u *)tail, conv_restlen);
		    size -= conv_restlen;
		}


		போது (p > ptr)
		{
		    எனில் (fio_flags & FIO_LATIN1)
			u8c = *--p;
		    இல்லையெனில் எனில் (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    {
			எனில் (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			இல்லையெனில்
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			எனில் ((fio_flags & FIO_UTF16)
					    && u8c >= 0xdc00 && u8c <= 0xdfff)
			{
			    முழு u16c;

			    எனில் (p == ptr)
			    {
				// Missing leading word.
				எனில் (can_retry)
				    செல் rewind_retry;
				எனில் (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				எனில் (bad_char_behavior == BAD_DROP)
				    தொடர்;
				எனில் (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }

			    // found second word of double-word, get the first
			    // word and compute the resulting character
			    எனில் (fio_flags & FIO_ENDIAN_L)
			    {
				u16c = (*--p << 8);
				u16c += *--p;
			    }
			    இல்லையெனில்
			    {
				u16c = *--p;
				u16c += (*--p << 8);
			    }
			    u8c = 0x10000 + ((u16c & 0x3ff) << 10)
							      + (u8c & 0x3ff);

			    // Check if the word is indeed a leading word.
			    எனில் (u16c < 0xd800 || u16c > 0xdbff)
			    {
				எனில் (can_retry)
				    செல் rewind_retry;
				எனில் (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				எனில் (bad_char_behavior == BAD_DROP)
				    தொடர்;
				எனில் (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    இல்லையெனில் எனில் (fio_flags & FIO_UCS4)
		    {
			எனில் (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (unsigned)*--p << 24;
			    u8c += (unsigned)*--p << 16;
			    u8c += (unsigned)*--p << 8;
			    u8c += *--p;
			}
			இல்லையெனில்	// big endian
			{
			    u8c = *--p;
			    u8c += (unsigned)*--p << 8;
			    u8c += (unsigned)*--p << 16;
			    u8c += (unsigned)*--p << 24;
			}
		    }
		    இல்லையெனில்    // UTF-8
		    {
			எனில் (*--p < 0x80)
			    u8c = *p;
			இல்லையெனில்
			{
			    len = utf_head_off(ptr, p);
			    p -= len;
			    u8c = utf_ptr2char(p);
			    எனில் (len == 0)
			    {
				// Not a valid UTF-8 character, retry with
				// another fenc when possible, otherwise just
				// report the error.
				எனில் (can_retry)
				    செல் rewind_retry;
				எனில் (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				எனில் (bad_char_behavior == BAD_DROP)
				    தொடர்;
				எனில் (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    எனில் (enc_utf8)	// produce UTF-8
		    {
			dest -= utf_char2len(u8c);
			(வெறுமை)utf_char2bytes(u8c, dest);
		    }
		    இல்லையெனில்		// produce Latin1
		    {
			--dest;
			எனில் (u8c >= 0x100)
			{
			    // character doesn't fit in latin1, retry with
			    // another fenc when possible, otherwise just
			    // report the error.
			    எனில் (can_retry)
				செல் rewind_retry;
			    எனில் (conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
			    எனில் (bad_char_behavior == BAD_DROP)
				++dest;
			    இல்லையெனில் எனில் (bad_char_behavior == BAD_KEEP)
				*dest = u8c;
			    இல்லையெனில் எனில் (eap != வெற்று && eap->bad_char != 0)
				*dest = bad_char_behavior;
			    இல்லையெனில்
				*dest = 0xBF;
			}
			இல்லையெனில்
			    *dest = u8c;
		    }
		}

		// move the linerest to before the converted characters
		வரி_start = dest - linerest;
		mch_memmove(வரி_start, buffer, (size_t)linerest);
		size = (long)((ptr + real_size) - dest);
		ptr = dest;
	    }
	    இல்லையெனில் எனில் (enc_utf8 && !curbuf->b_p_bin)
	    {
		முழு  incomplete_tail = FALSE;

		// Reading UTF-8: Check if the bytes are valid UTF-8.
		க்கு (p = ptr; ; )
		{
		    முழு	 todo;
		    முழு	 l;

		    // Skip ASCII bytes quickly using word-at-a-time check.
		    {
			char_u *ascii_end = ptr + size;
			போது (ascii_end - p >= (long)அளவு(long_u))
			{
			    long_u word;
			    memcpy(&word, p, அளவு(long_u));
			    எனில் (word & NONASCII_MASK)
				நிறுத்து;
			    p += அளவு(long_u);
			}
			போது (p < ascii_end && *p < 0x80)
			    ++p;
		    }

		    todo = (முழு)((ptr + size) - p);
		    எனில் (todo <= 0)
			நிறுத்து;
		    எனில் (*p >= 0x80)
		    {
			// A length of 1 means it's an illegal byte.  Accept
			// an incomplete character at the end though, the next
			// read() will get the next bytes, we'll check it
			// then.
			l = utf_ptr2len_len(p, todo);
			எனில் (l > todo && !incomplete_tail)
			{
			    // Avoid retrying with a different encoding when
			    // a truncated file is more likely, or attempting
			    // to read the rest of an incomplete sequence when
			    // we have already done so.
			    எனில் (p > ptr || filesize > 0)
				incomplete_tail = TRUE;
			    // Incomplete byte sequence, move it to conv_rest[]
			    // and try to read the rest of it, unless we've
			    // already done so.
			    எனில் (p > ptr)
			    {
				conv_restlen = todo;
				mch_memmove(conv_rest, p, conv_restlen);
				size -= conv_restlen;
				நிறுத்து;
			    }
			}
			எனில் (l == 1 || l > todo)
			{
			    // Illegal byte.  If we can try another encoding
			    // do that, unless at EOF where a truncated
			    // file is more likely than a conversion error.
			    எனில் (can_retry && !incomplete_tail)
				நிறுத்து;
#ifdef USE_ICONV
			    // When we did a conversion report an error.
			    எனில் (iconv_fd != (iconv_t)-1 && conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
#endif
			    // Remember the first linenr with an illegal byte
			    எனில் (conv_error == 0 && illegal_byte == 0)
				illegal_byte = readfile_linenr(linecnt, ptr, p);

			    // Drop, keep or replace the bad byte.
			    எனில் (bad_char_behavior == BAD_DROP)
			    {
				mch_memmove(p, p + 1, todo - 1);
				--size;
			    }
			    இல்லையெனில்
			    {
				எனில் (bad_char_behavior != BAD_KEEP)
				    *p = bad_char_behavior;
				++p;
			    }
			}
			இல்லையெனில்
			    p += l;
		    }
		}
		எனில் (p < ptr + size && !incomplete_tail)
		{
		    // Detected a UTF-8 error.
rewind_retry:
		    // Retry reading with another conversion.
#எனில் defined(FEAT_EVAL) && defined(USE_ICONV)
		    எனில் (*p_ccv != NUL && iconv_fd != (iconv_t)-1)
			// iconv() failed, try 'charconvert'
			did_iconv = TRUE;
		    இல்லையெனில்
#endif
			// use next item from 'fileencodings'
			advance_fenc = TRUE;
		    file_rewind = TRUE;
		    செல் retry;
		}
	    }

	    // count the number of characters (after conversion!)
	    filesize += size;

	    /*
	     * when reading the first part of a file: guess EOL type
	     */
	    எனில் (fileformat == EOL_UNKNOWN)
	    {
		// First try finding a NL, for Dos and Unix
		எனில் (try_dos || try_unix)
		{
		    // Reset the carriage return counter.
		    எனில் (try_mac)
			try_mac = 1;

		    க்கு (p = ptr; p < ptr + size; ++p)
		    {
			எனில் (*p == NL)
			{
			    எனில் (!try_unix
				    || (try_dos && p > ptr && p[-1] == CAR))
				fileformat = EOL_DOS;
			    இல்லையெனில்
				fileformat = EOL_UNIX;
			    நிறுத்து;
			}
			இல்லையெனில் எனில் (*p == CAR && try_mac)
			    try_mac++;
		    }

		    // Don't give in to EOL_UNIX if EOL_MAC is more likely
		    எனில் (fileformat == EOL_UNIX && try_mac)
		    {
			// Need to reset the counters when retrying fenc.
			try_mac = 1;
			try_unix = 1;
			க்கு (; p >= ptr && *p != CAR; p--)
			    ;
			எனில் (p >= ptr)
			{
			    க்கு (p = ptr; p < ptr + size; ++p)
			    {
				எனில் (*p == NL)
				    try_unix++;
				இல்லையெனில் எனில் (*p == CAR)
				    try_mac++;
			    }
			    எனில் (try_mac > try_unix)
				fileformat = EOL_MAC;
			}
		    }
		    இல்லையெனில் எனில் (fileformat == EOL_UNKNOWN && try_mac == 1)
			// Looking for CR but found no end-of-line markers at
			// all: use the default format.
			fileformat = default_fileformat();
		}

		// No NL found: may use Mac format
		எனில் (fileformat == EOL_UNKNOWN && try_mac)
		    fileformat = EOL_MAC;

		// Still nothing found?  Use first format in 'ffs'
		எனில் (fileformat == EOL_UNKNOWN)
		    fileformat = default_fileformat();

		// if editing a new file: may set p_tx and p_ff
		எனில் (set_options)
		    set_fileformat(fileformat, OPT_LOCAL);
	    }
	}

	/*
	 * This loop is executed once for every character read.
	 * Keep it fast!
	 */
	எனில் (fileformat == EOL_MAC)
	{
	    --ptr;
	    போது (++ptr, --size >= 0)
	    {
		// catch most common case first
		எனில் ((c = *ptr) != NUL && c != CAR && c != NL)
		    தொடர்;
		எனில் (c == NUL)
		    *ptr = NL;	// NULs are replaced by newlines!
		இல்லையெனில் எனில் (c == NL)
		    *ptr = CAR;	// NLs are replaced by CRs!
		இல்லையெனில்
		{
		    எனில் (skip_மொத்தம் == 0)
		    {
			*ptr = NUL;	    // end of line
			len = (colnr_T) (ptr - வரி_start + 1);
			எனில் (ml_append(lnum, வரி_start, len, newfile) == FAIL)
			{
			    error = TRUE;
			    நிறுத்து;
			}
#ifdef FEAT_PERSISTENT_UNDO
			எனில் (read_undo_file)
			    sha256_update(&sha_ctx, வரி_start, len);
#endif
			++lnum;
			எனில் (--read_மொத்தம் == 0)
			{
			    error = TRUE;	// break loop
			    வரி_start = ptr;	// nothing left to write
			    நிறுத்து;
			}
		    }
		    இல்லையெனில்
			--skip_மொத்தம்;
		    வரி_start = ptr + 1;
		}
	    }
	}
	இல்லையெனில்
	{
	    // Use memchr() for SIMD-optimized newline scanning instead
	    // of scanning each byte individually.
	    char_u *end = ptr + size;

	    போது (ptr < end)
	    {
		char_u *nl = (char_u *)memchr(ptr, NL, end - ptr);
		char_u *nul_scan;

		எனில் (nl == வெற்று)
		{
		    // No more newlines in buffer.
		    // Replace any NUL bytes with NL in remaining data.
		    போது ((nul_scan = (char_u *)memchr(ptr, NUL,
						      end - ptr)) != வெற்று)
		    {
			*nul_scan = NL;
			ptr = nul_scan + 1;
		    }
		    ptr = end;
		    நிறுத்து;
		}

		// Replace NUL bytes with NL before the newline.
		{
		    char_u *scan = ptr;
		    போது ((nul_scan = (char_u *)memchr(scan, NUL,
						       nl - scan)) != வெற்று)
		    {
			*nul_scan = NL;
			scan = nul_scan + 1;
		    }
		}

		// Process the newline.
		ptr = nl;
		எனில் (skip_மொத்தம் == 0)
		{
		    *ptr = NUL;		// end of line
		    len = (colnr_T)(ptr - வரி_start + 1);
		    எனில் (fileformat == EOL_DOS)
		    {
			எனில் (ptr > வரி_start && ptr[-1] == CAR)
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
			இல்லையெனில் எனில் (ff_error != EOL_DOS)
			{
			    எனில் (   try_unix
				&& !read_stdin
				&& (read_buffer
				    || vim_lseek(fd, (off_T)0L, SEEK_SET)
								      == 0))
			    {
				fileformat = EOL_UNIX;
				எனில் (set_options)
				    set_fileformat(EOL_UNIX, OPT_LOCAL);
				file_rewind = TRUE;
				keep_fileformat = TRUE;
				செல் retry;
			    }
			    ff_error = EOL_DOS;
			}
		    }
		    எனில் (ml_append(lnum, வரி_start, len, newfile) == FAIL)
		    {
			error = TRUE;
			நிறுத்து;
		    }
#ifdef FEAT_PERSISTENT_UNDO
		    எனில் (read_undo_file)
			sha256_update(&sha_ctx, வரி_start, len);
#endif
		    ++lnum;
		    எனில் (--read_மொத்தம் == 0)
		    {
			error = TRUE;	    // break loop
			வரி_start = ptr;   // nothing left to write
			நிறுத்து;
		    }
		}
		இல்லையெனில்
		    --skip_மொத்தம்;
		வரி_start = ptr + 1;
		++ptr;
	    }
	    size = -1;
	}
	linerest = (long)(ptr - வரி_start);
	ui_breakcheck();
    }

failed:
    // not an error, max. number of lines reached
    எனில் (error && read_மொத்தம் == 0)
	error = FALSE;

    // In Dos format ignore a trailing CTRL-Z, unless 'binary' is set.
    // In old days the file length was in sector count and the CTRL-Z the
    // marker where the file really ended.  Assuming we write it to a file
    // system that keeps file length properly the CTRL-Z should be dropped.
    // Set the 'endoffile' option so the user can decide what to write later.
    // In Unix format the CTRL-Z is just another character.
    எனில் (linerest != 0
	    && !curbuf->b_p_bin
	    && fileformat == EOL_DOS
	    && ptr[-1] == Ctrl_Z)
    {
	ptr--;
	linerest--;
	எனில் (set_options)
	    curbuf->b_p_eof = TRUE;
    }

    // If we get EOF in the middle of a line, note the fact by resetting
    // 'endofline' and add the line normally.
    எனில் (!error
	    && !got_int
	    && linerest != 0)
    {
	// remember for when writing
	எனில் (set_options)
	    curbuf->b_p_eol = FALSE;
	*ptr = NUL;
	len = (colnr_T)(ptr - வரி_start + 1);
	எனில் (ml_append(lnum, வரி_start, len, newfile) == FAIL)
	    error = TRUE;
	இல்லையெனில்
	{
#ifdef FEAT_PERSISTENT_UNDO
	    எனில் (read_undo_file)
		sha256_update(&sha_ctx, வரி_start, len);
#endif
	    read_no_eol_lnum = ++lnum;
	}
    }

    எனில் (set_options)
	save_file_ff(curbuf);		// remember the current file format

#ifdef FEAT_CRYPT
    எனில் (curbuf->b_cryptstate != வெற்று)
    {
	crypt_free_state(curbuf->b_cryptstate);
	curbuf->b_cryptstate = வெற்று;
    }
    எனில் (cryptkey != வெற்று && cryptkey != curbuf->b_p_சாவி)
	crypt_free_சாவி(cryptkey);
    // Don't set cryptkey to NULL, it's used below as a flag that
    // encryption was used.
#endif

    // If editing a new file: set 'fenc' for the current buffer.
    // Also for ":read ++edit file".
    எனில் (set_options)
	set_string_option_direct((char_u *)"fenc", -1, fenc,
						       OPT_FREE|OPT_LOCAL, 0);
    எனில் (fenc_alloced)
	vim_free(fenc);
#ifdef USE_ICONV
    எனில் (iconv_fd != (iconv_t)-1)
	iconv_close(iconv_fd);
#endif

    எனில் (!read_buffer && !read_stdin)
	close(fd);				// errors are ignored
#ifdef HAVE_FD_CLOEXEC
    இல்லையெனில்
    {
	முழு fdflags = fcntl(fd, F_GETFD);

	எனில் (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (வெறுமை)fcntl(fd, F_SETFD, fdflags | FD_CLOEXEC);
    }
#endif
    vim_free(buffer);

#ifdef HAVE_DUP
    எனில் (read_stdin)
    {
	// Use stderr for stdin, makes shell commands work.
	close(0);
	vim_ignored = dup(2);
    }
#endif

    எனில் (tmpname != வெற்று)
    {
	mch_remove(tmpname);		// delete converted file
	vim_free(tmpname);
    }
    --no_wait_return;			// may wait for return now

    /*
     * In recovery mode everything but autocommands is skipped.
     */
    எனில் (!recoverymode)
    {
	// need to delete the last line, which comes from the empty buffer
	எனில் (newfile && wasempty && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	{
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 0;
#endif
	    ml_delete(curbuf->b_ml.ml_வரி_மொத்தம்);
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 1;
#endif
	    --linecnt;
	}
	linecnt = curbuf->b_ml.ml_வரி_மொத்தம் - linecnt;
	எனில் (filesize == 0)
	    linecnt = 0;
	எனில் (newfile || read_buffer)
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
	இல்லையெனில் எனில் (linecnt)		// appended at least one line
	    appended_வரிகள்_mark(from, linecnt);

#ifndef ALWAYS_USE_GUI
	/*
	 * If we were reading from the same terminal as where messages go,
	 * the screen will have been messed up.
	 * Switch on raw mode now and clear the screen.
	 */
	எனில் (read_stdin)
	{
	    settmode(TMODE_RAW);	// set to raw mode
	    starttermcap();
	    screenclear();
	}
#endif

	எனில் (got_int)
	{
	    எனில் (!(flags & READ_DUMMY))
	    {
		filemess(curbuf, sfname, (char_u *)_(e_interrupted), 0);
		எனில் (newfile)
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    }
	    செய்தி_scroll = செய்தி_save;
#ifdef FEAT_VIMINFO
	    check_marks_read();
#endif
	    retval = OK;	// an interrupt isn't really an error
	    செல் theend;
	}

	எனில் (!filtering && !(flags & READ_DUMMY))
	{
	    முழு	buflen;

	    செய்தி_add_fname(curbuf, sfname);   // fname in IObuff with quotes
	    c = FALSE;

	    buflen = (முழு)STRLEN(IObuff);
#ifdef UNIX
	    எனில் (S_ISFIFO(perm))			    // fifo
	    {
		buflen += vim_snprintf((எழுத்து *)IObuff + buflen, IOSIZE - buflen,
			_("[fifo]"));
		c = TRUE;
	    }
	    எனில் (S_ISSOCK(perm))			    // or socket
	    {
		buflen += vim_snprintf((எழுத்து *)IObuff + buflen, IOSIZE - buflen,
			_("[socket]"));
		c = TRUE;
	    }
# ifdef OPEN_CHR_FILES
	    எனில் (S_ISCHR(perm))			    // or character special
	    {
		buflen += vim_snprintf((எழுத்து *)IObuff + buflen, IOSIZE - buflen,
			_("[character special]"));
		c = TRUE;
	    }
# endif
#endif
	    எனில் (curbuf->b_p_ro)
	    {
		buflen += vim_snprintf((எழுத்து *)IObuff + buflen, IOSIZE - buflen,
			"%s", shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
		c = TRUE;
	    }
	    எனில் (read_no_eol_lnum)
	    {
		செய்தி_add_eol();
		c = TRUE;
	    }
	    எனில் (ff_error == EOL_DOS)
	    {
		buflen += vim_snprintf((எழுத்து *)IObuff + buflen, IOSIZE - buflen,
			_("[CR missing]"));
		c = TRUE;
	    }
	    எனில் (split)
	    {
		buflen += vim_snprintf((எழுத்து *)IObuff + buflen, IOSIZE - buflen,
			_("[long lines split]"));
		c = TRUE;
	    }
	    எனில் (notconverted)
	    {
		buflen += vim_snprintf((எழுத்து *)IObuff + buflen, IOSIZE - buflen,
			_("[NOT converted]"));
		c = TRUE;
	    }
	    இல்லையெனில் எனில் (converted)
	    {
		buflen += vim_snprintf((எழுத்து *)IObuff + buflen, IOSIZE - buflen,
			_("[converted]"));
		c = TRUE;
	    }
#ifdef FEAT_CRYPT
	    எனில் (cryptkey != வெற்று)
	    {
		crypt_append_செய்தி(curbuf);
		c = TRUE;
	    }
#endif
	    எனில் (conv_error != 0)
	    {
		vim_snprintf((எழுத்து *)IObuff + buflen, IOSIZE - buflen,
			_("[CONVERSION ERROR in line %ld]"), (long)conv_error);
		c = TRUE;
	    }
	    இல்லையெனில் எனில் (illegal_byte > 0)
	    {
		vim_snprintf((எழுத்து *)IObuff + buflen, IOSIZE - buflen,
			_("[ILLEGAL BYTE in line %ld]"), (long)illegal_byte);
		c = TRUE;
	    }
	    இல்லையெனில் எனில் (error)
	    {
		vim_snprintf((எழுத்து *)IObuff + buflen, IOSIZE - buflen,
			_("[READ ERRORS]"));
		c = TRUE;
	    }
	    எனில் (செய்தி_add_fileformat(fileformat))
		c = TRUE;
#ifdef FEAT_CRYPT
	    எனில் (cryptkey != வெற்று)
		செய்தி_add_வரிகள்(c, (long)linecnt, filesize
			 - crypt_get_header_len(crypt_get_method_nr(curbuf)));
	    இல்லையெனில்
#endif
		செய்தி_add_வரிகள்(c, (long)linecnt, filesize);

	    VIM_CLEAR(keep_செய்தி);
	    செய்தி_scrolled_ign = TRUE;
#ifdef ALWAYS_USE_GUI
	    // Don't show the message when reading stdin, it would end up in a
	    // message box (which might be shown when exiting!)
	    எனில் (read_stdin || read_buffer)
		p = செய்தி_may_trunc(FALSE, IObuff);
	    இல்லையெனில்
#endif
	    {
		எனில் (செய்தி_col > 0)
		    செய்தி_putchar('\r');  // overwrite previous message
		p = (char_u *)செய்தி_trunc_attr((எழுத்து *)IObuff, FALSE, 0);
	    }
	    எனில் (read_stdin || read_buffer || restart_edit != 0
		    || (செய்தி_scrolled != 0 && !need_wait_return))
		// Need to repeat the message after redrawing when:
		// - When reading from stdin (the screen will be cleared next).
		// - When restart_edit is set (otherwise there will be a delay
		//   before redrawing).
		// - When the screen was scrolled but there is no wait-return
		//   prompt.
		set_keep_செய்தி(p, 0);
	    செய்தி_scrolled_ign = FALSE;
	}

	// with errors writing the file requires ":w!"
	எனில் (newfile && (error
		    || conv_error != 0
		    || (illegal_byte > 0 && bad_char_behavior != BAD_KEEP)))
	    curbuf->b_p_ro = TRUE;

	u_clearline();	    // cannot use "U" command after adding lines

	/*
	 * In Ex mode: cursor at last new line.
	 * Otherwise: cursor at first new line.
	 */
	எனில் (exmode_active)
	    curwin->w_cursor.lnum = from + linecnt;
	இல்லையெனில்
	    curwin->w_cursor.lnum = from + 1;
	check_cursor_lnum();
	beginline(BL_WHITE | BL_FIX);	    // on first non-blank

	எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
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
	எனில் (newfile && !read_stdin && !read_buffer
					 && mch_stat((எழுத்து *)fname, &st) >= 0)
	{
	    buf_store_time(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
	}
#endif
    }
    செய்தி_scroll = செய்தி_save;

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
    எனில் (flags & READ_KEEP_UNDO)
	u_find_first_changed();

#ifdef FEAT_PERSISTENT_UNDO
    /*
     * When opening a new file locate undo info and read it.
     */
    எனில் (read_undo_file)
    {
	char_u	hash[UNDO_HASH_SIZE];

	sha256_finish(&sha_ctx, hash);
	u_read_undo(வெற்று, hash, fname);
    }
#endif

    எனில் (!read_stdin && !read_fifo && (!read_buffer || sfname != வெற்று))
    {
	முழு m = செய்தி_scroll;
	முழு n = செய்தி_scrolled;

	// Save the fileformat now, otherwise the buffer will be considered
	// modified if the format/encoding was automatically detected.
	எனில் (set_options)
	    save_file_ff(curbuf);

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	செய்தி_scroll = TRUE;
	எனில் (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPOST, வெற்று, sfname,
							  FALSE, curbuf, eap);
	இல்லையெனில் எனில் (newfile || (read_buffer && sfname != வெற்று))
	{
	    apply_autocmds_exarg(EVENT_BUFREADPOST, வெற்று, sfname,
							  FALSE, curbuf, eap);
	    எனில் (!curbuf->b_au_did_filetype && *curbuf->b_p_ft != NUL)
		/*
		 * EVENT_FILETYPE was not triggered but the buffer already has a
		 * filetype. Trigger EVENT_FILETYPE using the existing filetype.
		 */
		apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
			TRUE, curbuf);
	}
	இல்லையெனில்
	    apply_autocmds_exarg(EVENT_FILEREADPOST, sfname, sfname,
							    FALSE, வெற்று, eap);
	எனில் (செய்தி_scrolled == n)
	    செய்தி_scroll = m;
#ifdef FEAT_EVAL
	எனில் (aborting())	    // autocmds may abort script processing
	    செல் theend;
#endif
    }

    எனில் (!(recoverymode && error))
	retval = OK;

theend:
    எனில் (curbuf->b_ml.ml_mfp != வெற்று
		       && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	// OK to sync the swap file now
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    திரும்பு retval;
}

#எனில் defined(OPEN_CHR_FILES)
/*
 * Returns TRUE if the file name argument is of the form "/dev/fd/\d\+",
 * which is the name of files used for process substitution output by
 * some shells on some operating systems, e.g., bash on SunOS.
 * Do not accept "/dev/fd/[012]", opening these may hang Vim.
 */
    முழு
is_dev_fd_file(char_u *fname)
{
    திரும்பு (STRNCMP(fname, "/dev/fd/", 8) == 0
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
    நிலையான linenr_T
readfile_linenr(
    linenr_T	linecnt,	// line count before reading more bytes
    char_u	*p,		// start of more bytes read
    char_u	*endp)		// end of more bytes read
{
    char_u	*s;
    linenr_T	lnum;

    lnum = curbuf->b_ml.ml_வரி_மொத்தம் - linecnt + 1;
    க்கு (s = p; s < endp; ++s)
	எனில் (*s == '\n')
	    ++lnum;
    திரும்பு lnum;
}

/*
 * Fill "*eap" to force the 'fileencoding', 'fileformat' and 'binary' to be
 * equal to the buffer "buf".  Used for calling readfile().
 * Returns OK or FAIL.
 */
    முழு
prep_exarg(exarg_T *eap, buf_T *buf)
{
    eap->cmd = alloc(15 + (unsigned)STRLEN(buf->b_p_fenc));
    எனில் (eap->cmd == வெற்று)
	திரும்பு FAIL;

    sprintf((எழுத்து *)eap->cmd, "e ++enc=%s", buf->b_p_fenc);
    eap->force_enc = 8;
    eap->bad_char = buf->b_bad_char;
    eap->force_ff = *buf->b_p_ff;

    eap->force_bin = buf->b_p_bin ? FORCE_BIN : FORCE_NOBIN;
    eap->read_edit = FALSE;
    eap->forceit = FALSE;
    திரும்பு OK;
}

/*
 * Set default or forced 'fileformat' and 'binary'.
 */
    வெறுமை
set_file_options(முழு set_options, exarg_T *eap)
{
    // set default 'fileformat'
    எனில் (set_options)
    {
	எனில் (eap != வெற்று && eap->force_ff != 0)
	    set_fileformat(get_fileformat_force(curbuf, eap), OPT_LOCAL);
	இல்லையெனில் எனில் (*p_ffs != NUL)
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    // set or reset 'binary'
    எனில் (eap != வெற்று && eap->force_bin != 0)
    {
	முழு	oldval = curbuf->b_p_bin;

	curbuf->b_p_bin = (eap->force_bin == FORCE_BIN);
	set_options_bin(oldval, curbuf->b_p_bin, OPT_LOCAL);
    }
}

/*
 * Set forced 'fileencoding'.
 */
    வெறுமை
set_forced_fenc(exarg_T *eap)
{
    எனில் (eap->force_enc == 0)
	திரும்பு;

    char_u *fenc = enc_canonize(eap->cmd + eap->force_enc);

    எனில் (fenc != வெற்று)
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
    நிலையான char_u *
next_fenc(char_u **pp, முழு *alloced)
{
    char_u	*p;
    char_u	*r;

    *alloced = FALSE;
    எனில் (**pp == NUL)
    {
	*pp = வெற்று;
	திரும்பு (char_u *)"";
    }
    p = vim_strchr(*pp, ',');
    எனில் (p == வெற்று)
    {
	r = enc_canonize(*pp);
	*pp += STRLEN(*pp);
    }
    இல்லையெனில்
    {
	r = vim_strnsave(*pp, p - *pp);
	*pp = p + 1;
	எனில் (r != வெற்று)
	{
	    p = enc_canonize(r);
	    vim_free(r);
	    r = p;
	}
    }
    எனில் (r != வெற்று)
	*alloced = TRUE;
    இல்லையெனில்
    {
	// out of memory
	r = (char_u *)"";
	*pp = வெற்று;
    }
    திரும்பு r;
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
    நிலையான char_u *
readfile_charconvert(
    char_u	*fname,		// name of input file
    char_u	*fenc,		// converted from
    முழு		*fdp)		// in/out: file descriptor of file
{
    char_u	*tmpname;
    எழுத்து	*errmsg = வெற்று;

    tmpname = vim_tempname('r', FALSE);
    எனில் (tmpname == வெற்று)
	errmsg = _("Can't find temp file for conversion");
    இல்லையெனில்
    {
	close(*fdp);		// close the input file, ignore errors
	*fdp = -1;
	எனில் (eval_charconvert(fenc, enc_utf8 ? (char_u *)"utf-8" : p_enc,
						      fname, tmpname) == FAIL)
	    errmsg = _("Conversion with 'charconvert' failed");
	எனில் (errmsg == வெற்று && (*fdp = mch_open((எழுத்து *)tmpname,
						  O_RDONLY | O_EXTRA, 0)) < 0)
	    errmsg = _("can't read output of 'charconvert'");
    }

    எனில் (errmsg != வெற்று)
    {
	// Don't use emsg(), it breaks mappings, the retry with
	// another type of conversion might still work.
	செய்தி(errmsg);
	எனில் (tmpname != வெற்று)
	{
	    mch_remove(tmpname);	// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    // If the input file is closed, open it (caller should check for error).
    எனில் (*fdp < 0)
	*fdp = mch_open((எழுத்து *)fname, O_RDONLY | O_EXTRA, 0);

    திரும்பு tmpname;
}
#endif

#எனில் defined(FEAT_CRYPT)
/*
 * Check for magic number used for encryption.  Applies to the current buffer.
 * If found, the magic number is removed from ptr[*sizep] and *sizep and
 * *filesizep are updated.
 * Return the (new) encryption key, NULL for no encryption.
 */
    நிலையான char_u *
check_for_cryptkey(
    char_u	*cryptkey,	// previous encryption key or NULL
    char_u	*ptr,		// pointer to read bytes
    long	*sizep,		// length of read bytes
    off_T	*filesizep,	// nr of bytes used from file
    முழு		newfile,	// editing a new buffer
    char_u	*fname,		// file name to display
    முழு		*did_ask)	// flag: whether already asked for key
{
    முழு method = crypt_method_nr_from_magic((எழுத்து *)ptr, *sizep);
    முழு b_p_ro = curbuf->b_p_ro;

    எனில் (method >= 0)
    {
	// Mark the buffer as read-only until the decryption has taken place.
	// Avoids accidentally overwriting the file with garbage.
	curbuf->b_p_ro = TRUE;

	// Set the cryptmethod local to the buffer.
	crypt_set_cm_option(curbuf, method);
	எனில் (cryptkey == வெற்று && !*did_ask)
	{
	    எனில் (*curbuf->b_p_சாவி)
	    {
		cryptkey = curbuf->b_p_சாவி;
		crypt_check_swapfile_curbuf();
	    }
	    இல்லையெனில்
	    {
		// When newfile is TRUE, store the typed key in the 'key'
		// option and don't free it.  bf needs hash of the key saved.
		// Don't ask for the key again when first time Enter was hit.
		// Happens when retrying to detect encoding.
		smsg(_(need_சாவி_செய்தி), fname);
		செய்தி_scroll = TRUE;
		crypt_check_method(method);
		cryptkey = crypt_get_சாவி(newfile, FALSE);
		*did_ask = TRUE;

		// check if empty key entered
		எனில் (cryptkey != வெற்று && *cryptkey == NUL)
		{
		    எனில் (cryptkey != curbuf->b_p_சாவி)
			vim_free(cryptkey);
		    cryptkey = வெற்று;
		}
	    }
	}

	எனில் (cryptkey != வெற்று)
	{
	    முழு header_len;

	    header_len = crypt_get_header_len(method);
	    எனில் (*sizep < header_len)
	    {
		// invalid header, buffer can't be encrypted
		எனில் (cryptkey != curbuf->b_p_சாவி)
		    vim_free(cryptkey);
		திரும்பு வெற்று;
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
    இல்லையெனில் எனில் (newfile && *curbuf->b_p_சாவி != NUL && !starting)
	set_option_மதிப்பு_give_err((char_u *)"key", 0L, (char_u *)"", OPT_LOCAL);

    திரும்பு cryptkey;
}
#endif  // FEAT_CRYPT

/*
 * Return TRUE if a file appears to be read-only from the file permissions.
 */
    முழு
check_file_readonly(
    char_u	*fname,		// full path to file
    முழு		perm UNUSED)	// known permissions on file
{
#ifndef USE_MCH_ACCESS
    முழு	    fd = 0;
#endif

    திரும்பு (
#ifdef USE_MCH_ACCESS
# ifdef UNIX
	(perm & 0222) == 0 ||
# endif
	mch_access((எழுத்து *)fname, W_OK)
#இல்லையெனில்
	(fd = mch_open((எழுத்து *)fname, O_RDWR | O_EXTRA, 0)) < 0
					? TRUE : (close(fd), FALSE)
#endif
	);
}

#எனில் defined(HAVE_FSYNC)
/*
 * Call fsync() with Mac-specific exception.
 * Return fsync() result: zero for success.
 */
    முழு
vim_fsync(முழு fd)
{
    முழு r;

# ifdef MACOS_X
    r = fcntl(fd, F_FULLFSYNC);
    எனில் (r != 0)  // F_FULLFSYNC not working or not supported
# endif
	r = fsync(fd);
    திரும்பு r;
}
#endif

/*
 * Set the name of the current buffer.  Use when the buffer doesn't have a
 * name and a ":r" or ":w" command with a file name is used.
 */
    முழு
set_rw_fname(char_u *fname, char_u *sfname)
{
    buf_T	*buf = curbuf;

    // It's like the unnamed buffer is deleted....
    எனில் (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFDELETE, வெற்று, வெற்று, FALSE, curbuf);
    apply_autocmds(EVENT_BUFWIPEOUT, வெற்று, வெற்று, FALSE, curbuf);
#ifdef FEAT_EVAL
    எனில் (aborting())	    // autocmds may abort script processing
	திரும்பு FAIL;
#endif
    எனில் (curbuf != buf)
    {
	// We are in another buffer now, don't do the renaming.
	emsg(_(e_autocommands_changed_buffer_or_buffer_name));
	திரும்பு FAIL;
    }

    எனில் (setfname(curbuf, fname, sfname, FALSE) == OK)
	curbuf->b_flags |= BF_NOTEDITED;

    // ....and a new named one is created
    apply_autocmds(EVENT_BUFNEW, வெற்று, வெற்று, FALSE, curbuf);
    எனில் (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFADD, வெற்று, வெற்று, FALSE, curbuf);
#ifdef FEAT_EVAL
    எனில் (aborting())	    // autocmds may abort script processing
	திரும்பு FAIL;
#endif

    // Do filetype detection now if 'filetype' is empty.
    எனில் (*curbuf->b_p_ft == NUL)
    {
	எனில் (au_has_group((char_u *)"filetypedetect"))
	    (வெறுமை)do_doautocmd((char_u *)"filetypedetect BufRead", FALSE, வெற்று);
	do_modelines(0);
    }

    திரும்பு OK;
}

/*
 * Put file name into IObuff with quotes.
 */
    வெறுமை
செய்தி_add_fname(buf_T *buf, char_u *fname)
{
    size_t  IObufflen = 0;

    எனில் (fname == வெற்று)
	fname = (char_u *)"-stdin-";
    IObuff[IObufflen++] = '"';
    IObufflen += home_replace(buf, fname, IObuff + IObufflen, IOSIZE - 4, TRUE);
    STRCPY(IObuff + IObufflen, "\" ");
}

/*
 * Append message for text mode to IObuff.
 * Return TRUE if something appended.
 */
    முழு
செய்தி_add_fileformat(முழு eol_type)
{
#ifndef USE_CRNL
    எனில் (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[dos]") : _("[dos format]"));
	திரும்பு TRUE;
    }
#endif
    எனில் (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[mac]") : _("[mac format]"));
	திரும்பு TRUE;
    }
#ifdef USE_CRNL
    எனில் (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[unix]") : _("[unix format]"));
	திரும்பு TRUE;
    }
#endif
    திரும்பு FALSE;
}

/*
 * Append line and character count to IObuff.
 */
    வெறுமை
செய்தி_add_வரிகள்(
    முழு	    செருகு_space,
    long    lnum,
    off_T   nchars)
{
    முழு  len = (முழு)STRLEN(IObuff);

    எனில் (shortmess(SHM_வரிகள்))
	vim_snprintf((எழுத்து *)IObuff + len, IOSIZE - (size_t)len,
		// l10n: L as in line, B as in byte
		_("%s%ldL, %lldB"), செருகு_space ? " " : "", lnum, (varnumber_T)nchars);
    இல்லையெனில்
    {
	len += vim_snprintf((எழுத்து *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%s%ld line, ", "%s%ld lines, ", lnum), செருகு_space ? " " : "", lnum);
	vim_snprintf((எழுத்து *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%lld byte", "%lld bytes", nchars), (varnumber_T)nchars);
    }
}

/*
 * Append message for missing line separator to IObuff.
 */
    வெறுமை
செய்தி_add_eol(வெறுமை)
{
    STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]") : _("[Incomplete last line]"));
}

    முழு
time_differs(stat_T *st, long mtime, long mtime_ns UNUSED)
{
    திரும்பு
#ifdef ST_MTIM_NSEC
	(long)st->ST_MTIM_NSEC != mtime_ns ||
#endif
#எனில் defined(__linux__) || defined(MSWIN)
	// On a FAT filesystem, esp. under Linux, there are only 5 bits to store
	// the seconds.  Since the roundoff is done when flushing the inode, the
	// time may change unexpectedly by one second!!!
	(long)st->st_mtime - mtime > 1 || mtime - (long)st->st_mtime > 1
#இல்லையெனில்
	(long)st->st_mtime != mtime
#endif
	;
}

/*
 * Return TRUE if file encoding "fenc" requires conversion from or to
 * 'encoding'.
 */
    முழு
need_conversion(char_u *fenc)
{
    முழு		same_encoding;
    முழு		enc_flags;
    முழு		fenc_flags;

    எனில் (*fenc == NUL || STRCMP(p_enc, fenc) == 0)
    {
	same_encoding = TRUE;
	fenc_flags = 0;
    }
    இல்லையெனில்
    {
	// Ignore difference between "ansi" and "latin1", "ucs-4" and
	// "ucs-4be", etc.
	enc_flags = get_fio_flags(p_enc);
	fenc_flags = get_fio_flags(fenc);
	same_encoding = (enc_flags != 0 && fenc_flags == enc_flags);
    }
    எனில் (same_encoding)
    {
	// Specified encoding matches with 'encoding'.  This requires
	// conversion when 'encoding' is Unicode but not UTF-8.
	திரும்பு enc_unicode != 0;
    }

    // Encodings differ.  However, conversion is not needed when 'enc' is any
    // Unicode encoding and the file is UTF-8.
    திரும்பு !(enc_utf8 && fenc_flags == FIO_UTF8);
}

/*
 * Check "ptr" for a unicode encoding and return the FIO_ flags needed for the
 * internal conversion.
 * if "ptr" is an empty string, use 'encoding'.
 */
    முழு
get_fio_flags(char_u *ptr)
{
    முழு		prop;

    எனில் (*ptr == NUL)
	ptr = p_enc;

    prop = enc_canon_props(ptr);
    எனில் (prop & ENC_UNICODE)
    {
	எனில் (prop & ENC_2BYTE)
	{
	    எனில் (prop & ENC_ENDIAN_L)
		திரும்பு FIO_UCS2 | FIO_ENDIAN_L;
	    திரும்பு FIO_UCS2;
	}
	எனில் (prop & ENC_4BYTE)
	{
	    எனில் (prop & ENC_ENDIAN_L)
		திரும்பு FIO_UCS4 | FIO_ENDIAN_L;
	    திரும்பு FIO_UCS4;
	}
	எனில் (prop & ENC_2WORD)
	{
	    எனில் (prop & ENC_ENDIAN_L)
		திரும்பு FIO_UTF16 | FIO_ENDIAN_L;
	    திரும்பு FIO_UTF16;
	}
	திரும்பு FIO_UTF8;
    }
    எனில் (prop & ENC_LATIN1)
	திரும்பு FIO_LATIN1;
    // must be ENC_DBCS, requires iconv()
    திரும்பு 0;
}

#எனில் defined(MSWIN)
/*
 * Check "ptr" for a MS-Windows codepage name and return the FIO_ flags needed
 * for the conversion MS-Windows can do for us.  Also accept "utf-8".
 * Used for conversion between 'encoding' and 'fileencoding'.
 */
    முழு
get_win_fio_flags(char_u *ptr)
{
    முழு		cp;

    // Cannot do this when 'encoding' is not utf-8 and not a codepage.
    எனில் (!enc_utf8 && enc_codepage <= 0)
	திரும்பு 0;

    cp = encname2codepage(ptr);
    எனில் (cp == 0)
    {
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	எனில் (STRCMP(ptr, "utf-8") == 0)
	    cp = CP_UTF8;
	இல்லையெனில்
# endif
	    திரும்பு 0;
    }
    திரும்பு FIO_PUT_CP(cp) | FIO_CODEPAGE;
}
#endif

#எனில் defined(MACOS_CONVERT)
/*
 * Check "ptr" for a Carbon supported encoding and return the FIO_ flags
 * needed for the internal conversion to/from utf-8 or latin1.
 */
    முழு
get_mac_fio_flags(char_u *ptr)
{
    எனில் ((enc_utf8 || STRCMP(p_enc, "latin1") == 0)
				     && (enc_canon_props(ptr) & ENC_MACROMAN))
	திரும்பு FIO_MACROMAN;
    திரும்பு 0;
}
#endif

/*
 * Check for a Unicode BOM (Byte Order Mark) at the start of p[size].
 * "size" must be at least 2.
 * Return the name of the encoding and set "*lenp" to the length.
 * Returns NULL when no BOM found.
 */
    நிலையான char_u *
check_for_bom(
    char_u	*p,
    long	size,
    முழு		*lenp,
    முழு		flags)
{
    எழுத்து	*name = வெற்று;
    முழு		len = 2;

    எனில் (p[0] == 0xef && p[1] == 0xbb && size >= 3 && p[2] == 0xbf
	    && (flags == FIO_ALL || flags == FIO_UTF8 || flags == 0))
    {
	name = "utf-8";		// EF BB BF
	len = 3;
    }
    இல்லையெனில் எனில் (p[0] == 0xff && p[1] == 0xfe)
    {
	எனில் (size >= 4 && p[2] == 0 && p[3] == 0
	    && (flags == FIO_ALL || flags == (FIO_UCS4 | FIO_ENDIAN_L)))
	{
	    name = "ucs-4le";	// FF FE 00 00
	    len = 4;
	}
	இல்லையெனில் எனில் (flags == (FIO_UCS2 | FIO_ENDIAN_L))
	    name = "ucs-2le";	// FF FE
	இல்லையெனில் எனில் (flags == FIO_ALL || flags == (FIO_UTF16 | FIO_ENDIAN_L))
	    // utf-16le is preferred, it also works for ucs-2le text
	    name = "utf-16le";	// FF FE
    }
    இல்லையெனில் எனில் (p[0] == 0xfe && p[1] == 0xff
	    && (flags == FIO_ALL || flags == FIO_UCS2 || flags == FIO_UTF16))
    {
	// Default to utf-16, it works also for ucs-2 text.
	எனில் (flags == FIO_UCS2)
	    name = "ucs-2";	// FE FF
	இல்லையெனில்
	    name = "utf-16";	// FE FF
    }
    இல்லையெனில் எனில் (size >= 4 && p[0] == 0 && p[1] == 0 && p[2] == 0xfe
	    && p[3] == 0xff && (flags == FIO_ALL || flags == FIO_UCS4))
    {
	name = "ucs-4";		// 00 00 FE FF
	len = 4;
    }

    *lenp = len;
    திரும்பு (char_u *)name;
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
    எனில் (dirname == வெற்று)
	திரும்பு full_path;
    எனில் (mch_dirname(dirname, MAXPATHL) == OK)
    {
	p = shorten_fname(full_path, dirname);
	எனில் (p == வெற்று || *p == NUL)
	    p = full_path;
    }
    vim_free(dirname);
    திரும்பு p;
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
    முழு		len;
    char_u	*p;

    எனில் (full_path == வெற்று)
	திரும்பு வெற்று;
    len = (முழு)STRLEN(dir_name);
    எனில் (fnamencmp(dir_name, full_path, len) == 0)
    {
	p = full_path + len;
#எனில் defined(MSWIN)
	/*
	 * MS-Windows: when a file is in the root directory, dir_name will end
	 * in a slash, since C: by itself does not define a specific dir. In
	 * this case p may already be correct. <negri>
	 */
	எனில் (!((len > 2) && (*(p - 2) == ':')))
#endif
	{
	    எனில் (vim_ispathsep(*p))
		செய்
		    ++p;
		போது (vim_ispathsep_nocolon(*p));
#ifndef VMS   // the path separator is always part of the path
	    இல்லையெனில்
		p = வெற்று;
#endif
	}
    }
#எனில் defined(MSWIN)
    /*
     * When using a file in the current drive, remove the drive name:
     * "A:\dir\file" -> "\dir\file".  This helps when moving a session file on
     * a floppy from "A:\dir" to "B:\dir".
     */
    இல்லையெனில் எனில் (len > 3
	    && TOUPPER_LOC(full_path[0]) == TOUPPER_LOC(dir_name[0])
	    && full_path[1] == ':'
	    && vim_ispathsep(full_path[2]))
	p = full_path + 2;
#endif
    இல்லையெனில்
	p = வெற்று;
    திரும்பு p;
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
    வெறுமை
shorten_buf_fname(buf_T *buf, char_u *dirname, முழு force)
{
    char_u	*p;

    எனில் (buf->b_fname != வெற்று
	    && !bt_nofilename(buf)
	    && !path_with_url(buf->b_fname)
	    && (force
		|| buf->b_sfname == வெற்று
		|| mch_isFullName(buf->b_sfname)))
    {
	எனில் (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	p = shorten_fname(buf->b_ffname, dirname);
	எனில் (p != வெற்று)
	{
	    buf->b_sfname = vim_strsave(p);
	    buf->b_fname = buf->b_sfname;
	}
	எனில் (p == வெற்று || buf->b_fname == வெற்று)
	    buf->b_fname = buf->b_ffname;
    }
}

/*
 * Shorten filenames for all buffers.
 */
    வெறுமை
shorten_fnames(முழு force)
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
#எனில் defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    popup_update_preview_title();
#endif
}

#எனில் (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
/*
 * Shorten all filenames in "fnames[count]" by current directory.
 */
    வெறுமை
shorten_filenames(char_u **fnames, முழு மொத்தம்)
{
    முழு		i;
    char_u	dirname[MAXPATHL];
    char_u	*p;

    எனில் (fnames == வெற்று || மொத்தம் < 1)
	திரும்பு;
    mch_dirname(dirname, அளவு(dirname));
    க்கு (i = 0; i < மொத்தம்; ++i)
    {
	எனில் ((p = shorten_fname(fnames[i], dirname)) != வெற்று)
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
    முழு	    prepend_dot)	// may prepend a '.' to file name
{
    திரும்பு buf_modname((curbuf->b_p_sn || curbuf->b_shortname),
						     fname, ext, prepend_dot);
}

    char_u *
buf_modname(
    முழு	    shortname,		// use 8.3 file name
    char_u  *fname,
    char_u  *ext,
    முழு	    prepend_dot)	// may prepend a '.' to file name
{
    char_u	*retval;
    char_u	*s;
    char_u	*e;
    char_u	*ptr;
    size_t	ptrlen;
    முழு		fnamelen, extlen;

    extlen = (முழு)STRLEN(ext);

    /*
     * If there is no file name we must get the name of the current directory
     * (we need the full path in case :cd is used).
     */
    எனில் (fname == வெற்று || *fname == NUL)
    {
	retval = alloc(MAXPATHL + extlen + 3);
	எனில் (retval == வெற்று)
	    திரும்பு வெற்று;
	எனில் (mch_dirname(retval, MAXPATHL) == FAIL ||
				     (fnamelen = (முழு)STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    திரும்பு வெற்று;
	}
	எனில் (!after_pathsep(retval, retval + fnamelen))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
	prepend_dot = FALSE;	    // nothing to prepend a dot to
    }
    இல்லையெனில்
    {
	fnamelen = (முழு)STRLEN(fname);
	retval = alloc(fnamelen + extlen + 3);
	எனில் (retval == வெற்று)
	    திரும்பு வெற்று;
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
    க்கு (ptr = retval + fnamelen; ptr > retval; MB_PTR_BACK(retval, ptr))
    {
	எனில் (*ext == '.' && shortname)
	    எனில் (*ptr == '.')	// replace '.' by '_'
		*ptr = '_';
	எனில் (vim_ispathsep(*ptr))
	{
	    ++ptr;
	    நிறுத்து;
	}
    }

    // the file name has at most BASENAMELEN characters.
    ptrlen = (size_t)(fnamelen - (ptr - retval));
    எனில் (ptrlen > (unsigned)BASENAMELEN)
    {
	ptrlen = BASENAMELEN;
	ptr[ptrlen] = NUL;
    }

    s = ptr + ptrlen;

    /*
     * For 8.3 file names we may have to reduce the length.
     */
    எனில் (shortname)
    {
	/*
	 * If there is no file name, or the file name ends in '/', and the
	 * extension starts with '.', put a '_' before the dot, because just
	 * ".ext" is invalid.
	 */
	எனில் (fname == வெற்று || *fname == NUL
				   || vim_ispathsep(fname[STRLEN(fname) - 1]))
	{
	    எனில் (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	இல்லையெனில் எனில் (*ext == '.')
	{
	    எனில் ((size_t)(s - ptr) > (size_t)8)
	    {
		s = ptr + 8;
		*s = '\0';
	    }
	}
	/*
	 * If the extension doesn't start with '.', and the file name
	 * doesn't have an extension yet, append a '.'
	 */
	இல்லையெனில் எனில் ((e = vim_strchr(ptr, '.')) == வெற்று)
	    *s++ = '.';
	/*
	 * If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be truncated
	 */
	இல்லையெனில் எனில் ((முழு)(ptrlen - (e - retval)) + extlen > 4)
	    s = e + 4 - extlen;
    }
#ifdef MSWIN
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    இல்லையெனில் எனில் ((fname == வெற்று || *fname == NUL) && *ext == '.')
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
    எனில் (prepend_dot && !shortname && *(e = gettail(retval)) != '.')
    {
	mch_memmove(e + 1, e, (size_t)(((fnamelen + extlen) - (e - retval)) + 1));	// +1 for NUL
	*e = '.';
    }

    /*
     * Check that, after appending the extension, the file name is really
     * different.
     */
    எனில் (fname != வெற்று && STRCMP(fname, retval) == 0)
    {
	// we search for a character that can be replaced by '_'
	போது (--s >= ptr)
	{
	    எனில் (*s != '_')
	    {
		*s = '_';
		நிறுத்து;
	    }
	}
	எனில் (s < ptr)	// fname was "________.<ext>", how tricky!
	    *ptr = 'v';
    }
    திரும்பு retval;
}

/*
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE for end-of-file.
 * If the line is truncated then buf[size - 2] will not be NUL.
 */
    முழு
vim_fgets(char_u *buf, முழு size, FILE *fp)
{
    எழுத்து	*eof;
#define FGETS_SIZE 200
    எழுத்து	tbuf[FGETS_SIZE];

    // safety check
    எனில் (size < 2)
    {
	எனில் (size == 1)
	    buf[0] = NUL;
	திரும்பு TRUE;
    }

    buf[size - 2] = NUL;
    eof = fgets((எழுத்து *)buf, size, fp);
    எனில் (buf[size - 2] != NUL && buf[size - 2] != '\n')
    {
	buf[size - 1] = NUL;	    // Truncate the line

	// Now throw away the rest of the line:
	செய்
	{
	    tbuf[FGETS_SIZE - 2] = NUL;
	    vim_ignoredp = fgets((எழுத்து *)tbuf, FGETS_SIZE, fp);
	} போது (tbuf[FGETS_SIZE - 2] != NUL && tbuf[FGETS_SIZE - 2] != '\n');
    }
    திரும்பு (eof == வெற்று);
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 * Return -1 for failure, 0 for success.
 */
    முழு
vim_rename(char_u *from, char_u *to)
{
    முழு		n;
    முழு		ret;
#ifdef AMIGA
    BPTR	flock;
#endif
    stat_T	st;
    முழு		use_tmp_file = FALSE;

    /*
     * When the names are identical, there is nothing to do.  When they refer
     * to the same file (ignoring case and slash/backslash differences) but
     * the file name differs we need to go through a temp file.
     */
    எனில் (fnamecmp(from, to) == 0)
    {
	எனில் (p_fic && STRCMP(gettail(from), gettail(to)) != 0)
	    use_tmp_file = TRUE;
	இல்லையெனில்
	    திரும்பு 0;
    }

    /*
     * Fail if the "from" file doesn't exist.  Avoids that "to" is deleted.
     */
    எனில் (mch_stat((எழுத்து *)from, &st) < 0)
	திரும்பு -1;

#ifdef UNIX
    {
	stat_T	st_to;

	// It's possible for the source and destination to be the same file.
	// This happens when "from" and "to" differ in case and are on a FAT32
	// filesystem.  In that case go through a temp file name.
	எனில் (mch_stat((எழுத்து *)to, &st_to) >= 0
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
	எனில் (win32_fileinfo(from, &info1) == FILEINFO_OK
		&& win32_fileinfo(to, &info2) == FILEINFO_OK
		&& info1.dw_Volume_Serial_எண் == info2.dw_Volume_Serial_எண்
		&& info1.n_File_Index_உயர்வு == info2.n_File_Index_உயர்வு
		&& info1.n_File_Index_கீழ் == info2.n_File_Index_கீழ்)
	    use_tmp_file = TRUE;
    }
#endif

    எனில் (use_tmp_file)
    {
	எழுத்து	tempname[MAXPATHL + 1];

	/*
	 * Find a name that doesn't exist and is in the same directory.
	 * Rename "from" to "tempname" and then rename "tempname" to "to".
	 */
	எனில் (STRLEN(from) >= MAXPATHL - 5)
	    திரும்பு -1;
	STRCPY(tempname, from);
	க்கு (n = 123; n < 99999; ++n)
	{
	    sprintf((எழுத்து *)gettail((char_u *)tempname), "%d", n);
	    எனில் (mch_stat(tempname, &st) < 0)
	    {
		எனில் (mch_rename((எழுத்து *)from, tempname) == 0)
		{
		    எனில் (mch_rename(tempname, (எழுத்து *)to) == 0)
			திரும்பு 0;
		    // Strange, the second step failed.  Try moving the
		    // file back and return failure.
		    (வெறுமை)mch_rename(tempname, (எழுத்து *)from);
		    திரும்பு -1;
		}
		// If it fails for one temp name it will most likely fail
		// for any temp name, give up.
		திரும்பு -1;
	    }
	}
	திரும்பு -1;
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
    எனில் (flock)
	UnLock(flock);
#endif

    /*
     * First try a normal rename, return if it works.
     */
    எனில் (mch_rename((எழுத்து *)from, (எழுத்து *)to) == 0)
	திரும்பு 0;

    /*
     * Rename() failed, try copying the file.
     */
    ret = vim_copyfile(from, to);
    எனில் (ret != OK)
	திரும்பு -1;

    /*
     * Remove copied original file
     */
    எனில் (mch_stat((எழுத்து *)from, &st) >= 0)
	mch_remove(from);

    திரும்பு 0;
}


/*
 * Create the new file with same permissions as the original.
 * Return FAIL for failure, OK for success.
 */
    முழு
vim_copyfile(char_u *from, char_u *to)
{
    முழு		fd_in;
    முழு		fd_out;
    முழு		n;
    எழுத்து	*errmsg = வெற்று;
    எழுத்து	*buffer;
    long	perm;
#ifdef HAVE_ACL
    vim_acl_T	acl;		// ACL from original file
#endif

#ifdef HAVE_READLINK
    முழு		ret;
    முழு		len;
    stat_T	st;
    எழுத்து	linkbuf[MAXPATHL + 1];

    ret = mch_lstat((எழுத்து *)from, &st);
    எனில் (ret >= 0 && S_ISLNK(st.st_mode))
    {
	ret = -1;

	len = readlink((எழுத்து *)from, linkbuf, MAXPATHL);
	எனில் (len > 0)
	{
	    linkbuf[len] = NUL;

	    // Create link
	    ret = symlink(linkbuf, (எழுத்து *)to);
	}

	திரும்பு ret == 0 ? OK : FAIL;
    }
#endif

    perm = mch_getperm(from);
#ifdef HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    acl = mch_get_acl(from);
#endif
    fd_in = mch_open((எழுத்து *)from, O_RDONLY|O_EXTRA, 0);
    எனில் (fd_in == -1)
    {
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	திரும்பு FAIL;
    }

    // Create the new file with same permissions as the original.
    fd_out = mch_open((எழுத்து *)to,
		       O_CREAT|O_EXCL|O_WRONLY|O_EXTRA|O_NOFOLLOW, (முழு)perm);
    எனில் (fd_out == -1)
    {
	close(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	திரும்பு FAIL;
    }

    buffer = alloc(WRITEBUFSIZE);
    எனில் (buffer == வெற்று)
    {
	close(fd_out);
	close(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	திரும்பு FAIL;
    }

    போது ((n = read_eintr(fd_in, buffer, WRITEBUFSIZE)) > 0)
	எனில் (write_eintr(fd_out, buffer, n) != n)
	{
	    errmsg = _(e_error_writing_to_str);
	    நிறுத்து;
	}

    vim_free(buffer);
    close(fd_in);
    எனில் (close(fd_out) < 0)
	errmsg = _(e_error_closing_str);
    எனில் (n < 0)
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
#எனில் defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    mch_copy_sec(from, to);
#endif
    எனில் (errmsg != வெற்று)
    {
	semsg(errmsg, to);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

நிலையான முழு already_warned = FALSE;

/*
 * Check if any not hidden buffer has been changed.
 * Postpone the check if there are characters in the stuff buffer, a global
 * command is being executed, a mapping is being executed or an autocommand is
 * busy.
 * Returns TRUE if some message was written (screen should be redrawn and
 * cursor positioned).
 */
    முழு
check_timestamps(
    முழு		focus)		// called for GUI focus event
{
    buf_T	*buf;
    முழு		didit = 0;
    முழு		n;

    // Don't check timestamps while system() or another low-level function may
    // cause us to lose and gain focus.
    எனில் (no_check_timestamps > 0)
	திரும்பு FALSE;

    // Avoid doing a check twice.  The OK/Reload dialog can cause a focus
    // event and we would keep on checking if the file is steadily growing.
    // Do check again after typing something.
    எனில் (focus && did_check_timestamps)
    {
	need_check_timestamps = TRUE;
	திரும்பு FALSE;
    }

    எனில் (!stuff_empty() || global_busy || !typebuf_typed()
			|| autocmd_busy || curbuf_lock > 0 || allbuf_lock > 0)
	need_check_timestamps = TRUE;		// check later
    இல்லையெனில்
    {
	++no_wait_return;
	did_check_timestamps = TRUE;
	already_warned = FALSE;
	FOR_ALL_BUFFERS(buf)
	{
	    // Only check buffers in a window.
	    எனில் (buf->b_nwindows > 0)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		n = buf_check_timestamp(buf, focus);
		எனில் (didit < n)
		    didit = n;
		எனில் (n > 0 && !bufref_valid(&bufref))
		{
		    // Autocommands have removed the buffer, start at the
		    // first one again.
		    buf = firstbuf;
		    தொடர்;
		}
	    }
	}
	--no_wait_return;
	need_check_timestamps = FALSE;
	எனில் (need_wait_return && didit == 2)
	{
	    // make sure msg isn't overwritten
	    செய்தி_puts("\n");
	    out_flush();
	}
    }
    திரும்பு didit;
}

/*
 * Move all the lines from buffer "frombuf" to buffer "tobuf".
 * Return OK or FAIL.  When FAIL "tobuf" is incomplete and/or "frombuf" is not
 * empty.
 */
    நிலையான முழு
move_வரிகள்(buf_T *frombuf, buf_T *tobuf)
{
    buf_T	*tbuf = curbuf;
    முழு		retval = OK;
    linenr_T	lnum;
    char_u	*p;

    // Copy the lines in "frombuf" to "tobuf".
    curbuf = tobuf;
    க்கு (lnum = 1; lnum <= frombuf->b_ml.ml_வரி_மொத்தம்; ++lnum)
    {
	p = vim_strnsave(ml_get_buf(frombuf, lnum, FALSE), ml_get_buf_len(frombuf, lnum));
	எனில் (p == வெற்று || ml_append(lnum - 1, p, 0, FALSE) == FAIL)
	{
	    vim_free(p);
	    retval = FAIL;
	    நிறுத்து;
	}
	vim_free(p);
    }

    // Delete all the lines in "frombuf".
    எனில் (retval != FAIL)
    {
	curbuf = frombuf;
	க்கு (lnum = curbuf->b_ml.ml_வரி_மொத்தம்; lnum > 0; --lnum)
	    எனில் (ml_delete(lnum) == FAIL)
	    {
		// Oops!  We could try putting back the saved lines, but that
		// might fail again...
		retval = FAIL;
		நிறுத்து;
	    }
    }

    curbuf = tbuf;
    திரும்பு retval;
}

/*
 * Check if buffer "buf" has been changed.
 * Also check if the file for a new buffer unexpectedly appeared.
 * return 1 if a changed buffer was found.
 * return 2 if a message has been displayed.
 * return 0 otherwise.
 */
    முழு
buf_check_timestamp(
    buf_T	*buf,
    முழு		focus UNUSED)	// called for GUI focus event
{
    stat_T	st;
    முழு		stat_res;
    முழு		retval = 0;
    எழுத்து	*mesg = வெற்று;
    எழுத்து	*mesg2 = "";
    முழு		helpmesg = FALSE;
    எண்ணிக்கை {
	RELOAD_NONE,
	RELOAD_NORMAL,
	RELOAD_DETECT
    }		reload = RELOAD_NONE;
#எனில் defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
    முழு		can_reload = FALSE;
#endif
    off_T	orig_size = buf->b_orig_size;
    முழு		orig_mode = buf->b_orig_mode;
#ifdef FEAT_GUI
    முழு		save_mouse_correct = need_mouse_correct;
#endif
    நிலையான முழு	busy = FALSE;
    முழு		n;
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    // If there is no file name, the buffer is not loaded, 'buftype' is
    // set, we are in the middle of a save or being called recursively: ignore
    // this buffer.
    எனில் (buf->b_ffname == வெற்று
	    || buf->b_ml.ml_mfp == வெற்று
	    || !bt_normal(buf)
	    || buf->b_saving
	    || busy
#ifdef FEAT_NETBEANS_INTG
	    || isNetbeansBuffer(buf)
#endif
#ifdef FEAT_TERMINAL
	    || buf->b_term != வெற்று
#endif
	    )
	திரும்பு 0;

    எனில் (       !(buf->b_flags & BF_NOTEDITED)
	    && buf->b_mtime != 0
	    && ((stat_res = mch_stat((எழுத்து *)buf->b_ffname, &st)) < 0
		|| time_differs(&st, buf->b_mtime, buf->b_mtime_ns)
		|| st.st_size != buf->b_orig_size
#ifdef HAVE_ST_MODE
		|| (முழு)st.st_mode != buf->b_orig_mode
#இல்லையெனில்
		|| mch_getperm(buf->b_ffname) != buf->b_orig_mode
#endif
		))
    {
	long prev_b_mtime = buf->b_mtime;

	retval = 1;

	// set b_mtime to stop further warnings (e.g., when executing
	// FileChangedShell autocmd)
	எனில் (stat_res < 0)
	{
	    // Check the file again later to see if it re-appears.
	    buf->b_mtime = -1;
	    buf->b_orig_size = 0;
	    buf->b_orig_mode = 0;
	}
	இல்லையெனில்
	    buf_store_time(buf, &st, buf->b_ffname);

	// Don't do anything for a directory.  Might contain the file
	// explorer.
	எனில் (mch_isdir(buf->b_fname))
	    ;

	/*
	 * If 'autoread' is set, the buffer has no changes and the file still
	 * exists, reload the buffer.  Use the buffer-local option value if it
	 * was set, the global option value otherwise.
	 */
	இல்லையெனில் எனில் ((buf->b_p_ar >= 0 ? buf->b_p_ar : p_ar)
				       && !bufIsChanged(buf) && stat_res >= 0)
	    reload = RELOAD_NORMAL;
	இல்லையெனில்
	{
	    எழுத்து    *reason;
#ifdef FEAT_EVAL
	    size_t  reasonlen;
#endif

	    எனில் (stat_res < 0)
	    {
		reason = "deleted";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("deleted");
#endif
	    }
	    இல்லையெனில் எனில் (bufIsChanged(buf))
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
	    இல்லையெனில் எனில் (orig_size != buf->b_orig_size || buf_contents_changed(buf))
	    {
		reason = "changed";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("changed");
#endif
	    }
	    இல்லையெனில் எனில் (orig_mode != buf->b_orig_mode)
	    {
		reason = "mode";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("mode");
#endif
	    }
	    இல்லையெனில்
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
	    set_vim_var_string(VV_FCS_REASON, (char_u *)reason, (முழு)reasonlen);
	    set_vim_var_string(VV_FCS_CHOICE, (char_u *)"", 0);
#endif
	    ++allbuf_lock;
	    n = apply_autocmds(EVENT_FILECHANGEDSHELL,
				      buf->b_fname, buf->b_fname, FALSE, buf);
	    --allbuf_lock;
	    busy = FALSE;
	    எனில் (n)
	    {
		எனில் (!bufref_valid(&bufref))
		    emsg(_(e_filechangedshell_autocommand_deleted_buffer));
#ifdef FEAT_EVAL
		char_u	*s = get_vim_var_str(VV_FCS_CHOICE);
		எனில் (STRCMP(s, "reload") == 0 && *reason != 'd')
		    reload = RELOAD_NORMAL;
		இல்லையெனில் எனில் (STRCMP(s, "edit") == 0)
		    reload = RELOAD_DETECT;
		இல்லையெனில் எனில் (STRCMP(s, "ask") == 0)
		    n = FALSE;
		இல்லையெனில்
#endif
		    திரும்பு 2;
	    }
	    எனில் (!n)
	    {
		எனில் (*reason == 'd')
		{
		    // Only give the message once.
		    எனில் (prev_b_mtime != -1)
			mesg = _(e_file_str_no_longer_available);
		}
		இல்லையெனில்
		{
		    helpmesg = TRUE;
#எனில் defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		    can_reload = TRUE;
#endif
		    எனில் (reason[2] == 'n')
		    {
			mesg = _("W12: Warning: File \"%s\" has changed and the buffer was changed in Vim as well");
			mesg2 = _("See \":help W12\" for more info.");
		    }
		    இல்லையெனில் எனில் (reason[1] == 'h')
		    {
			mesg = _("W11: Warning: File \"%s\" has changed since editing started");
			mesg2 = _("See \":help W11\" for more info.");
		    }
		    இல்லையெனில் எனில் (*reason == 'm')
		    {
			mesg = _("W16: Warning: Mode of file \"%s\" has changed since editing started");
			mesg2 = _("See \":help W16\" for more info.");
		    }
		    இல்லையெனில்
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
    இல்லையெனில் எனில் ((buf->b_flags & BF_NEW) && !(buf->b_flags & BF_NEW_W)
						&& vim_fexists(buf->b_ffname))
    {
	retval = 1;
	mesg = _("W13: Warning: File \"%s\" has been created after editing started");
	buf->b_flags |= BF_NEW_W;
#எனில் defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	can_reload = TRUE;
#endif
    }

    எனில் (mesg != வெற்று)
    {
	char_u *path;

	path = home_replace_save(buf, buf->b_fname);
	எனில் (path != வெற்று)
	{
	    size_t  tbufsize;
	    எழுத்து    *tbuf;

	    எனில் (!helpmesg)
		mesg2 = "";
	    tbufsize = STRLEN(mesg) + STRLEN(path) + 2 + STRLEN(mesg2) + 1; // +2 for either '\n' or "; "
									    // and +1 for NUL
	    tbuf = alloc(tbufsize);
	    எனில் (tbuf != வெற்று)
	    {
		முழு tbuflen;

		tbuflen = vim_snprintf(tbuf, tbufsize, mesg, path);
#ifdef FEAT_EVAL
		// Set warningmsg here, before the unimportant and output-specific
		// mesg2 has been appended.
		set_vim_var_string(VV_WARNINGMSG, (char_u *)tbuf, tbuflen);
#endif
#எனில் defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		எனில் (can_reload)
		{
		    எனில் (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "\n%s", mesg2);
		    தேர்வு (do_dialog(VIM_WARNING, (char_u *)_("Warning"),
			    (char_u *)tbuf,
			    (char_u *)_("&OK\n&Load File\nLoad File &and Options"),
			    1, வெற்று, TRUE))
		    {
			நிலை 2:
			    reload = RELOAD_NORMAL;
			    நிறுத்து;
			நிலை 3:
			    reload = RELOAD_DETECT;
			    நிறுத்து;
		    }
		}
		இல்லையெனில்
#endif
		எனில் (State > MODE_NORMAL_BUSY || (State & MODE_CMDLINE)
								 || already_warned)
		{
		    எனில் (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "; %s", mesg2);
		    emsg(tbuf);
		    retval = 2;
		}
		இல்லையெனில்
		{
		    எனில் (!autocmd_busy)
		    {
			செய்தி_start();
			செய்தி_puts_attr(tbuf, HL_ATTR(HLF_E) + செய்தி_HIST);
			எனில் (*mesg2 != NUL)
			    செய்தி_puts_attr(mesg2, HL_ATTR(HLF_W) + செய்தி_HIST);
			செய்தி_clr_eos();
			(வெறுமை)செய்தி_end();
			எனில் (emsg_silent == 0 && !in_assert_fails)
			{
			    out_flush();
#ifdef FEAT_GUI
			    எனில் (!focus)
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

    எனில் (reload != RELOAD_NONE)
    {
	// Reload the buffer.
	buf_reload(buf, orig_mode, reload == RELOAD_DETECT);
#ifdef FEAT_PERSISTENT_UNDO
	எனில் (bufref_valid(&bufref) && buf->b_p_udf && buf->b_ffname != வெற்று)
	{
	    char_u	    hash[UNDO_HASH_SIZE];
	    buf_T	    *save_curbuf = curbuf;

	    // Any existing undo file is unusable, write it now.
	    curbuf = buf;
	    u_compute_hash(hash);
	    u_write_undo(வெற்று, FALSE, buf, hash);
	    curbuf = save_curbuf;
	}
#endif
    }

    // Trigger FileChangedShell when the file was changed in any way.
    எனில் (bufref_valid(&bufref) && retval != 0)
	(வெறுமை)apply_autocmds(EVENT_FILECHANGEDSHELLPOST,
				      buf->b_fname, buf->b_fname, FALSE, buf);
#ifdef FEAT_GUI
    // restore this in case an autocommand has set it; it would break
    // 'mousefocus'
    need_mouse_correct = save_mouse_correct;
#endif

    திரும்பு retval;
}

/*
 * Reload a buffer that is already loaded.
 * Used when the file was changed outside of Vim.
 * "orig_mode" is buf->b_orig_mode before the need for reloading was detected.
 * buf->b_orig_mode may have been reset already.
 */
    வெறுமை
buf_reload(buf_T *buf, முழு orig_mode, முழு reload_options)
{
    exarg_T	ea;
    pos_T	old_cursor;
    linenr_T	old_topline;
    முழு		old_ro = buf->b_p_ro;
    buf_T	*savebuf;
    bufref_T	bufref;
    முழு		saved = OK;
    aco_save_T	aco;
    முழு		flags = READ_NEW;
    முழு		prepped = OK;

    // Set curwin/curbuf for "buf" and save some things.
    aucmd_prepbuf(&aco, buf);
    எனில் (curbuf != buf)
    {
	// Failed to find a window for "buf", it is dangerous to continue,
	// better bail out.
	திரும்பு;
    }

    // Unless reload_options is set, we only want to read the text from the
    // file, not reset the syntax highlighting, clear marks, diff status, etc.
    // Force the fileformat and encoding to be the same.
    எனில் (reload_options)
	CLEAR_FIELD(ea);
    இல்லையெனில்
	prepped = prep_exarg(&ea, buf);

    எனில் (prepped == OK)
    {
	old_cursor = curwin->w_cursor;
	old_topline = curwin->w_topline;

	எனில் (p_ur < 0 || curbuf->b_ml.ml_வரி_மொத்தம் <= p_ur)
	{
	    // Save all the text, so that the reload can be undone.
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    saved = u_savecommon(0, curbuf->b_ml.ml_வரி_மொத்தம் + 1, 0, TRUE);
	    flags |= READ_KEEP_UNDO;
	}

	/*
	 * To behave like when a new file is edited (matters for
	 * BufReadPost autocommands) we first need to delete the current
	 * buffer contents.  But if reading the file fails we should keep
	 * the old contents.  Can't use memory only, the file might be
	 * too big.  Use a hidden buffer to move the buffer contents to.
	 */
	எனில் (BUFEMPTY() || saved == FAIL)
	    savebuf = வெற்று;
	இல்லையெனில்
	{
	    // Allocate a buffer without putting it in the buffer list.
	    savebuf = buflist_new(வெற்று, வெற்று, (linenr_T)1, BLN_DUMMY);
	    set_bufref(&bufref, savebuf);
	    எனில் (savebuf != வெற்று && buf == curbuf)
	    {
		// Open the memline.
		curbuf = savebuf;
		curwin->w_buffer = savebuf;
		saved = ml_open(curbuf);
		curbuf = buf;
		curwin->w_buffer = buf;
	    }
	    எனில் (savebuf == வெற்று || saved == FAIL || buf != curbuf
				      || move_வரிகள்(buf, savebuf) == FAIL)
	    {
		semsg(_(e_could_not_prepare_for_reloading_str), buf->b_fname);
		saved = FAIL;
	    }
	}

	எனில் (saved == OK)
	{
	    முழு old_செய்தி_silent = செய்தி_silent;

	    curbuf->b_flags |= BF_CHECK_RO;	// check for RO again
	    curbuf->b_keep_filetype = true;	// don't detect 'filetype'

	    எனில் (shortmess(SHM_FILEINFO))
		செய்தி_silent = 1;

	    எனில் (readfile(buf->b_ffname, buf->b_fname, (linenr_T)0,
			(linenr_T)0,
			(linenr_T)MAXLNUM, &ea, flags) != OK)
	    {
#எனில் defined(FEAT_EVAL)
		எனில் (!aborting())
#endif
		    semsg(_(e_could_not_reload_str), buf->b_fname);
		எனில் (savebuf != வெற்று && bufref_valid(&bufref) && buf == curbuf)
		{
		    // Put the text back from the save buffer.  First
		    // delete any lines that readfile() added.
		    போது (!BUFEMPTY())
			எனில் (ml_delete(buf->b_ml.ml_வரி_மொத்தம்) == FAIL)
			    நிறுத்து;
		    (வெறுமை)move_வரிகள்(savebuf, buf);
		}
	    }
	    இல்லையெனில் எனில் (buf == curbuf)  // "buf" still valid
	    {
		// Mark the buffer as unmodified and free undo info.
		unchanged(buf, TRUE, TRUE);
		எனில் ((flags & READ_KEEP_UNDO) == 0)
		    u_clearallandblockfree(buf);
		இல்லையெனில்
		{
		    // Mark all undo states as changed.
		    u_unchanged(curbuf);
		}
	    }

	    செய்தி_silent = old_செய்தி_silent;
	}
	vim_free(ea.cmd);

	எனில் (savebuf != வெற்று && bufref_valid(&bufref))
	    wipe_buffer(savebuf, FALSE);

#ifdef FEAT_DIFF
	// Invalidate diff info if necessary.
	diff_invalidate(curbuf);
#endif

	// Restore the topline and cursor position and check it (lines may
	// have been removed).
	எனில் (old_topline > curbuf->b_ml.ml_வரி_மொத்தம்)
	    curwin->w_topline = curbuf->b_ml.ml_வரி_மொத்தம்;
	இல்லையெனில்
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
		எனில் (wp->w_buffer == curwin->w_buffer
			&& !foldmethodIsManual(wp))
		    foldUpdateAll(wp);
	}
#endif
	// If the mode didn't change and 'readonly' was set, keep the old
	// value; the user probably used the ":view" command.  But don't
	// reset it, might have had a read error.
	எனில் (orig_mode == curbuf->b_orig_mode)
	    curbuf->b_p_ro |= old_ro;

	// Modelines must override settings done by autocommands.
	do_modelines(0);
    }

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);
    // Careful: autocommands may have made "buf" invalid!
}

    வெறுமை
buf_store_time(buf_T *buf, stat_T *st, char_u *fname UNUSED)
{
    buf->b_mtime = (long)st->st_mtime;
#ifdef ST_MTIM_NSEC
    buf->b_mtime_ns = (long)st->ST_MTIM_NSEC;
#இல்லையெனில்
    buf->b_mtime_ns = 0;
#endif
    buf->b_orig_size = st->st_size;
#ifdef HAVE_ST_MODE
    buf->b_orig_mode = (முழு)st->st_mode;
#இல்லையெனில்
    buf->b_orig_mode = mch_getperm(fname);
#endif
}

/*
 * Adjust the line with missing eol, used for the next write.
 * Used for do_filter(), when the input lines for the filter are deleted.
 */
    வெறுமை
write_lnum_adjust(linenr_T offset)
{
    எனில் (curbuf->b_no_eol_lnum != 0)	// only if there is a missing eol
	curbuf->b_no_eol_lnum += offset;
}

// Subfuncions for readdirex()
#ifdef FEAT_EVAL
# ifdef MSWIN
    நிலையான char_u *
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
    எனில் (wp != வெற்று)
    {
	எனில் (_wcsicmp(wp, L".exe") == 0 ||
		_wcsicmp(wp, L".com") == 0 ||
		_wcsicmp(wp, L".cmd") == 0 ||
		_wcsicmp(wp, L".bat") == 0)
	    st_mode |= _S_IEXEC;
    }

    // Copy user bits to group/other.
    st_mode |= (st_mode & 0700) >> 3;
    st_mode |= (st_mode & 0700) >> 6;

    st.st_mode = st_mode;
    திரும்பு getfpermst(&st, perm);
}

    நிலையான வெறுமை
getftypewfd(WIN32_FIND_DATAW *wfd, string_T *ret)
{
    DWORD flag = wfd->dwFileAttributes;
    DWORD tag = wfd->dwReserved0;

    எனில் (flag & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	எனில் (tag == IO_REPARSE_TAG_MOUNT_POINT)
	{
	    ret->string = (char_u *)"junction";
	    ret->length = STRLEN_LITERAL("junction");
	    திரும்பு;
	}
	எனில் (tag == IO_REPARSE_TAG_SYMLINK)
	{
	    எனில் (flag & FILE_ATTRIBUTE_DIRECTORY)
	    {
		ret->string = (char_u *)"linkd";
		ret->length = STRLEN_LITERAL("linkd");
		திரும்பு;
	    }

	    ret->string = (char_u *)"link";
	    ret->length = STRLEN_LITERAL("link");
	    திரும்பு;
	}

	// unknown reparse point type
	ret->string = (char_u *)"reparse";
	ret->length = STRLEN_LITERAL("reparse");
	திரும்பு;
    }

    எனில் (flag & FILE_ATTRIBUTE_DIRECTORY)
    {
	ret->string = (char_u *)"dir";
	ret->length = STRLEN_LITERAL("dir");
	திரும்பு;
    }

    ret->string = (char_u *)"file";
    ret->length = STRLEN_LITERAL("file");
}

    நிலையான dict_T *
create_readdirex_item(WIN32_FIND_DATAW *wfd)
{
    dict_T	*item;
    char_u	*p;
    varnumber_T	size, time;
    char_u	permbuf[] = "---------";
    string_T	s;

    item = dict_alloc();
    எனில் (item == வெற்று)
	திரும்பு வெற்று;
    item->dv_refcount++;

    p = utf16_to_enc(wfd->cFileName, வெற்று);
    எனில் (p == வெற்று)
	செல் theend;
    எனில் (dict_add_string(item, "name", p) == FAIL)
    {
	vim_free(p);
	செல் theend;
    }
    vim_free(p);

    size = (((varnumber_T)wfd->n_File_Size_உயர்வு) << 32) | wfd->n_File_Size_கீழ்;
    எனில் (dict_add_எண்(item, "size", size) == FAIL)
	செல் theend;

    // Convert FILETIME to unix time.
    time = (((((varnumber_T)wfd->ftLastWriteTime.dw_உயர்வு_Date_Time) << 32) |
		wfd->ftLastWriteTime.dw_கீழ்_Date_Time)
	    - 116444736000000000) / 10000000;
    எனில் (dict_add_எண்(item, "time", time) == FAIL)
	செல் theend;

    getftypewfd(wfd, &s);
    எனில் (dict_add_string_len(item, "type", s.string, (முழு)s.length) == FAIL)
	செல் theend;
    எனில் (dict_add_string(item, "perm", getfpermwfd(wfd, permbuf)) == FAIL)
	செல் theend;

    எனில் (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	செல் theend;
    எனில் (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	செல் theend;

    திரும்பு item;

theend:
    dict_unref(item);
    திரும்பு வெற்று;
}
# இல்லையெனில்
    நிலையான dict_T *
create_readdirex_item(char_u *path, char_u *name)
{
    dict_T	*item;
    எழுத்து	*p;
    size_t	pathlen, len;
    size_t	namelen;
    stat_T	st;
    முழு		ret, link = FALSE;
    varnumber_T	size;
    char_u	permbuf[] = "---------";
    string_T	q = {வெற்று, 0};
    கட்டமைப்பு passwd *pw;
    கட்டமைப்பு group  *gr;

    item = dict_alloc();
    எனில் (item == வெற்று)
	திரும்பு வெற்று;
    item->dv_refcount++;

    pathlen = STRLEN(path);
    namelen = STRLEN(name);
    len = pathlen + 1 + namelen + 1;
    p = alloc(len);
    எனில் (p == வெற்று)
	செல் theend;
    எனில் (pathlen > 0 && path[pathlen - 1] == '/')
	vim_snprintf(p, len, "%s%s", path, name);
    இல்லையெனில்
	vim_snprintf(p, len, "%s/%s", path, name);
    ret = mch_lstat(p, &st);
    எனில் (ret >= 0 && S_ISLNK(st.st_mode))
    {
	link = TRUE;
	ret = mch_stat(p, &st);
	எனில் (ret < 0)
	    STR_LITERAL_SET(q, "link");
    }
    vim_free(p);

    எனில் (dict_add_string_len(item, "name", name, (முழு)namelen) == FAIL)
	செல் theend;

    எனில் (ret >= 0)
    {
	size = (varnumber_T)st.st_size;
	எனில் (S_ISDIR(st.st_mode))
	    size = 0;
	// non-perfect check for overflow
	இல்லையெனில் எனில் ((off_T)size != (off_T)st.st_size)
	    size = -2;
	எனில் (dict_add_எண்(item, "size", size) == FAIL)
	    செல் theend;
	எனில் (dict_add_எண்(item, "time", (varnumber_T)st.st_mtime) == FAIL)
	    செல் theend;

	எனில் (link)
	{
	    எனில் (S_ISDIR(st.st_mode))
		STR_LITERAL_SET(q, "linkd");
	    இல்லையெனில்
		STR_LITERAL_SET(q, "link");
	}
	இல்லையெனில்
	{
	    q.string = getftypest(&st);
	    q.length = STRLEN(q.string);
	}
	எனில் (dict_add_string_len(item, "type", q.string, (முழு)q.length) == FAIL)
	    செல் theend;
	எனில் (dict_add_string(item, "perm", getfpermst(&st, permbuf)) == FAIL)
	    செல் theend;

	pw = getpwuid(st.st_uid);
	எனில் (pw == வெற்று)
	    STR_LITERAL_SET(q, "");
	இல்லையெனில்
	{
	    q.string = (char_u *)pw->pw_name;
	    q.length = STRLEN(q.string);
	}
	எனில் (dict_add_string_len(item, "user", q.string, (முழு)q.length) == FAIL)
	    செல் theend;
#  எனில் !defined(VMS) || (defined(VMS) && defined(HAVE_XOS_R_H))
	gr = getgrgid(st.st_gid);
	எனில் (gr == வெற்று)
	    STR_LITERAL_SET(q, "");
	இல்லையெனில்
	{
	    q.string = (char_u *)gr->gr_name;
	    q.length = STRLEN(q.string);
	}
#  endif
	எனில் (dict_add_string_len(item, "group", q.string, (முழு)q.length) == FAIL)
	    செல் theend;
    }
    இல்லையெனில்
    {
	எனில் (dict_add_எண்(item, "size", -1) == FAIL)
	    செல் theend;
	எனில் (dict_add_எண்(item, "time", -1) == FAIL)
	    செல் theend;
	எனில் (q.string == வெற்று)
	{
	    எனில் (dict_add_string_len(item, "type", (char_u *)"", 0) == FAIL)
		செல் theend;
	}
	இல்லையெனில்
	{
	    எனில் (dict_add_string_len(item, "type", q.string, (முழு)q.length) == FAIL)
		செல் theend;
	}
	எனில் (dict_add_string_len(item, "perm", (char_u *)"", 0) == FAIL)
	    செல் theend;
	எனில் (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	    செல் theend;
	எனில் (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	    செல் theend;
    }
    திரும்பு item;

theend:
    dict_unref(item);
    திரும்பு வெற்று;
}
# endif

    நிலையான முழு
compare_readdirex_item(மாறிலி வெறுமை *p1, மாறிலி வெறுமை *p2)
{
    char_u  *name1, *name2;

    name1 = dict_get_string(*(dict_T**)p1, "name", FALSE);
    name2 = dict_get_string(*(dict_T**)p2, "name", FALSE);
    எனில் (readdirex_sort == READDIR_SORT_BYTE)
	திரும்பு STRCMP(name1, name2);
    எனில் (readdirex_sort == READDIR_SORT_IC)
	திரும்பு STRICMP(name1, name2);

    திரும்பு STRCOLL(name1, name2);
}

    நிலையான முழு
compare_readdir_item(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    எனில் (readdirex_sort == READDIR_SORT_BYTE)
	திரும்பு STRCMP(*(எழுத்து **)s1, *(எழுத்து **)s2);
    எனில் (readdirex_sort == READDIR_SORT_IC)
	திரும்பு STRICMP(*(எழுத்து **)s1, *(எழுத்து **)s2);

    திரும்பு STRCOLL(*(எழுத்து **)s1, *(எழுத்து **)s2);
}
#endif

#எனில் defined(TEMPDIRNAMES) || defined(FEAT_EVAL)
/*
 * Core part of "readdir()" and "readdirex()" function.
 * Retrieve the list of files/directories of "path" into "gap".
 * If "withattr" is TRUE, retrieve the names and their attributes.
 * If "withattr" is FALSE, retrieve the names only.
 * Return OK for success, FAIL for failure.
 */
    முழு
readdir_core(
    garray_T	*gap,
    char_u	*path,
    முழு		withattr UNUSED,
    வெறுமை	*context,
    முழு		(*checkitem)(வெறுமை *context, வெறுமை *item),
    முழு		sort)
{
    முழு			failed = FALSE;
    char_u		*p;
# ifdef MSWIN
    char_u		*buf;
    முழு			ok;
    HANDLE		hFind = INVALID_HANDLE_மதிப்பு;
    WIN32_FIND_DATAW    wfd;
    WCHAR		*wn = வெற்று;	// UTF-16 name, NULL when not used.
    char_u		*p_end;
# இல்லையெனில்
    DIR			*dirp;
    கட்டமைப்பு dirent	*dp;
# endif

    ga_init2(gap, அளவு(வெறுமை *), 20);

# ifdef FEAT_EVAL
#  define FREE_ITEM(item)   செய் { \
	எனில் (withattr) \
	    dict_unref((dict_T*)(item)); \
	இல்லையெனில் \
	    vim_free(item); \
    } போது (0)

    readdirex_sort = READDIR_SORT_BYTE;
# இல்லையெனில்
#  define FREE_ITEM(item)   vim_free(item)
# endif

# ifdef MSWIN
    buf = alloc(MAXPATHL);
    எனில் (buf == வெற்று)
	திரும்பு FAIL;
    STRNCPY(buf, path, MAXPATHL-5);
    p = p_end = buf + STRLEN(buf);
    MB_PTR_BACK(buf, p);
    எனில் (*p == '\\' || *p == '/')
	p_end = p;
    STRCPY(p_end, "\\*");

    wn = enc_to_utf16(buf, வெற்று);
    எனில் (wn != வெற்று)
	hFind = FindFirstFileW(wn, &wfd);
    ok = (hFind != INVALID_HANDLE_மதிப்பு);
    எனில் (!ok)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    இல்லையெனில்
    {
	போது (ok)
	{
	    முழு	    ignore;
	    வெறுமை    *item;
	    WCHAR   *wp;

	    wp = wfd.cFileName;
	    ignore = wp[0] == L'.' &&
		    (wp[1] == NUL ||
		     (wp[1] == L'.' && wp[2] == NUL));
	    எனில் (ignore)
	    {
		ok = FindNextFileW(hFind, &wfd);
		தொடர்;
	    }
#  ifdef FEAT_EVAL
	    எனில் (withattr)
		item = (வெறுமை*)create_readdirex_item(&wfd);
	    இல்லையெனில்
#  endif
		item = (வெறுமை*)utf16_to_enc(wfd.cFileName, வெற்று);
	    எனில் (item == வெற்று)
	    {
		failed = TRUE;
		நிறுத்து;
	    }

	    எனில் (!ignore && checkitem != வெற்று)
	    {
		முழு r = checkitem(context, item);

		எனில் (r < 0)
		{
		    FREE_ITEM(item);
		    நிறுத்து;
		}
		எனில் (r == 0)
		    ignore = TRUE;
	    }

	    எனில் (!ignore)
	    {
		எனில் (ga_grow(gap, 1) == OK)
		    ((வெறுமை**)gap->ga_data)[gap->ga_len++] = item;
		இல்லையெனில்
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    நிறுத்து;
		}
	    }
	    இல்லையெனில்
		FREE_ITEM(item);

	    ok = FindNextFileW(hFind, &wfd);
	}
	FindClose(hFind);
    }

    vim_free(buf);
    vim_free(wn);
# இல்லையெனில்	// MSWIN
    dirp = opendir((எழுத்து *)path);
    எனில் (dirp == வெற்று)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    இல்லையெனில்
    {
	க்கு (;;)
	{
	    முழு	    ignore;
	    வெறுமை    *item;

	    dp = readdir(dirp);
	    எனில் (dp == வெற்று)
		நிறுத்து;
	    p = (char_u *)dp->d_name;

	    ignore = p[0] == '.' &&
		    (p[1] == NUL ||
		     (p[1] == '.' && p[2] == NUL));
	    எனில் (ignore)
		தொடர்;
#  ifdef FEAT_EVAL
	    எனில் (withattr)
		item = (வெறுமை*)create_readdirex_item(path, p);
	    இல்லையெனில்
#  endif
		item = (வெறுமை*)vim_strsave(p);
	    எனில் (item == வெற்று)
	    {
		failed = TRUE;
		நிறுத்து;
	    }

	    எனில் (checkitem != வெற்று)
	    {
		முழு r = checkitem(context, item);

		எனில் (r < 0)
		{
		    FREE_ITEM(item);
		    நிறுத்து;
		}
		எனில் (r == 0)
		    ignore = TRUE;
	    }

	    எனில் (!ignore)
	    {
		எனில் (ga_grow(gap, 1) == OK)
		    ((வெறுமை**)gap->ga_data)[gap->ga_len++] = item;
		இல்லையெனில்
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    நிறுத்து;
		}
	    }
	    இல்லையெனில்
		FREE_ITEM(item);
	}

	closedir(dirp);
    }
# endif	// MSWIN

# undef FREE_ITEM

    எனில் (!failed && gap->ga_len > 0 && sort > READDIR_SORT_NONE)
    {
# ifdef FEAT_EVAL
	readdirex_sort = sort;
	எனில் (withattr)
	    qsort((வெறுமை*)gap->ga_data, (size_t)gap->ga_len, அளவு(dict_T*),
		    compare_readdirex_item);
	இல்லையெனில்
	    qsort((வெறுமை*)gap->ga_data, (size_t)gap->ga_len, அளவு(char_u *),
		    compare_readdir_item);
# இல்லையெனில்
	    sort_strings((char_u **)gap->ga_data, gap->ga_len);
# endif
    }

    திரும்பு failed ? FAIL : OK;
}

/*
 * Delete "name" and everything in it, recursively.
 * return 0 for success, -1 if some file was not deleted.
 */
    முழு
delete_recursive(char_u *name)
{
    முழு result = 0;

    // A symbolic link to a directory itself is deleted, not the directory it
    // points to.
    எனில் (
# எனில் defined(UNIX) || defined(MSWIN)
	 mch_isrealdir(name)
# இல்லையெனில்
	 mch_isdir(name)
# endif
	    )
    {
	char_u	    *exp = vim_strsave(name);
	garray_T    ga;

	எனில் (exp == வெற்று)
	    திரும்பு -1;
	எனில் (readdir_core(&ga, exp, FALSE, வெற்று, வெற்று, READDIR_SORT_NONE) == OK)
	{
	    முழு	len = vim_snprintf((எழுத்து *)NameBuff, MAXPATHL, "%s/", exp);
	    முழு	i;

	    க்கு (i = 0; i < ga.ga_len; ++i)
	    {
		vim_snprintf((எழுத்து *)NameBuff + len, MAXPATHL - len, "%s", ((char_u **)ga.ga_data)[i]);
		எனில் (delete_recursive(NameBuff) != 0)
		    // Remember the failure but continue deleting any further
		    // entries.
		    result = -1;
	    }
	    ga_clear_strings(&ga);
	    எனில் (mch_rmdir(exp) != 0)
		result = -1;
	}
	இல்லையெனில்
	    result = -1;
	vim_free(exp);
    }
    இல்லையெனில்
	result = mch_remove(name) == 0 ? 0 : -1;

    திரும்பு result;
}
#endif

#எனில் defined(TEMPDIRNAMES)
நிலையான long	temp_மொத்தம் = 0;		// Temp filename counter.

# எனில் defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
/*
 * Open temporary directory and take file lock to prevent
 * to be auto-cleaned.
 */
   நிலையான வெறுமை
vim_opentempdir(வெறுமை)
{
    DIR *dp = வெற்று;

    எனில் (vim_tempdir_dp != வெற்று)
	திரும்பு;

    dp = opendir((மாறிலி எழுத்து*)vim_tempdir);
    எனில் (dp == வெற்று)
	திரும்பு;

    vim_tempdir_dp = dp;
    flock(dirfd(vim_tempdir_dp), LOCK_SH);
}

/*
 * Close temporary directory - it automatically release file lock.
 */
   நிலையான வெறுமை
vim_closetempdir(வெறுமை)
{
    எனில் (vim_tempdir_dp == வெற்று)
	திரும்பு;

    closedir(vim_tempdir_dp);
    vim_tempdir_dp = வெற்று;
}

/*
 * Return true if the temp directory we created is gone.
 */
    நிலையான bool
vim_tempdir_gone(வெறுமை)
{
    stat_T	st;

    எனில் (vim_tempdir_dp == வெற்று)
	திரும்பு false;
    திரும்பு fstat(dirfd(vim_tempdir_dp), &st) < 0 || st.st_nlink == 0;
}
# endif

/*
 * Delete the temp directory and all files it contains.
 */
    வெறுமை
vim_deltempdir(வெறுமை)
{
    எனில் (vim_tempdir == வெற்று)
	திரும்பு;

# எனில் defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
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
    நிலையான வெறுமை
vim_settempdir(char_u *tempdir)
{
    char_u	*buf;
    size_t	buflen;

    buf = alloc(MAXPATHL + 2);
    எனில் (buf == வெற்று)
	திரும்பு;

    எனில் (vim_FullName(tempdir, buf, MAXPATHL, FALSE) == FAIL)
	STRCPY(buf, tempdir);
    buflen = STRLEN(buf);
    எனில் (!after_pathsep(buf, buf + buflen))
    {
	STRCPY(buf + buflen, PATHSEPSTR);
	buflen += அளவு(PATHSEP);
    }
    vim_tempdir = vim_strnsave(buf, buflen);
# எனில் defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
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
    முழு	    extra_char UNUSED,  // char to use in the name instead of '?'
    முழு	    keep UNUSED)
{
#ifdef USE_TMPNAM
    char_u	itmp[L_tmpnam];	// use tmpnam()
#அல்லதெனில் defined(MSWIN)
    WCHAR	itmp[TEMPNAMELEN];
#இல்லையெனில்
    char_u	itmp[TEMPNAMELEN];
#endif

#ifdef TEMPDIRNAMES
    நிலையான எழுத்து	*(tempdirs[]) = {TEMPDIRNAMES};
    முழு		i;
# ifndef EEXIST
    stat_T	st;
# endif

# எனில் defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    // if the temp directory is gone, force re-creation of it
    எனில் (vim_tempdir != வெற்று && vim_tempdir_gone())
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
    எனில் (vim_tempdir == வெற்று)
    {
	/*
	 * Try the entries in TEMPDIRNAMES to create the temp directory.
	 */
	க்கு (i = 0; i < (முழு)ARRAY_LENGTH(tempdirs); ++i)
	{
# ifndef HAVE_MKDTEMP
	    long	nr;
	    long	off;
# endif
	    size_t	itmplen;

	    // Expand $TMP, leave room for "/v1100000/999999999".
	    // Skip the directory check if the expansion fails.
	    itmplen = expand_env((char_u *)tempdirs[i], itmp, TEMPNAMELEN - 20);
	    எனில் (itmp[0] != '$' && mch_isdir(itmp))
	    {
		// directory exists
		எனில் (!after_pathsep(itmp, itmp + itmplen))
		{
		    STRCPY(itmp + itmplen, PATHSEPSTR);
		    itmplen += அளவு(PATHSEP);
		}

# ifdef HAVE_MKDTEMP
		{
#  எனில் defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    mode_t	umask_save = umask(077);
#  endif
		    // Leave room for filename
		    STRCPY(itmp + itmplen, "vXXXXXX");
		    itmplen += STRLEN_LITERAL("vXXXXXX");
		    எனில் (mkdtemp((எழுத்து *)itmp) != வெற்று)
			vim_settempdir(itmp);
#  எனில் defined(UNIX) || defined(VMS)
		    (வெறுமை)umask(umask_save);
#  endif
		}
# இல்லையெனில்
		// Get an arbitrary number of up to 6 digits.  When it's
		// unlikely that it already exists it will be faster,
		// otherwise it doesn't matter.  The use of mkdir() avoids any
		// security problems because of the predictable number.
		nr = (mch_get_pid() + (long)time(வெற்று)) % 1000000L;

		// Try up to 10000 different values until we find a name that
		// doesn't exist.
		க்கு (off = 0; off < 10000L; ++off)
		{
		    முழு		r;
#  எனில் defined(UNIX) || defined(VMS)
		    mode_t	umask_save;
#  endif

		    vim_snprintf((எழுத்து *)itmp + itmplen, அளவு(itmp) - itmplen, "v%ld", nr + off);
#  ifndef EEXIST
		    // If mkdir() does not set errno to EEXIST, check for
		    // existing file here.  There is a race condition then,
		    // although it's fail-safe.
		    எனில் (mch_stat((எழுத்து *)itmp, &st) >= 0)
			தொடர்;
#  endif
#  எனில் defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    umask_save = umask(077);
#  endif
		    r = vim_mkdir(itmp, 0700);
#  எனில் defined(UNIX) || defined(VMS)
		    (வெறுமை)umask(umask_save);
#  endif
		    எனில் (r == 0)
		    {
			vim_settempdir(itmp);
			நிறுத்து;
		    }
#  ifdef EEXIST
		    // If the mkdir() didn't fail because the file/dir exists,
		    // we probably can't create any dir here, try another
		    // place.
		    எனில் (errno != EEXIST)
#  endif
			நிறுத்து;
		}
# endif // HAVE_MKDTEMP
		எனில் (vim_tempdir != வெற்று)
		    நிறுத்து;
	    }
	}
    }

    எனில் (vim_tempdir != வெற்று)
    {
	// There is no need to check if the file exists, because we own the
	// directory and nobody else creates a file in it.
	முழு itmplen = vim_snprintf((எழுத்து *)itmp, அளவு(itmp), "%s%ld", vim_tempdir, temp_மொத்தம்++);
	திரும்பு vim_strnsave(itmp, (size_t)itmplen);
    }

    திரும்பு வெற்று;

#இல்லையெனில் // TEMPDIRNAMES
    char_u	*p;

# ifdef MSWIN
    WCHAR	wszTempFile[_MAX_PATH + 1];
    WCHAR	buf4[4];
    WCHAR	*chartab = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char_u	*retval;
    char_u	*shname;
    long	i;

    wcscpy(itmp, L"");
    எனில் (GetTempPathW(_MAX_PATH, wszTempFile) == 0)
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
    எனில் (GetTempFileNameW(wszTempFile, buf4, 0, itmp) == 0)
	திரும்பு வெற்று;
    எனில் (!keep)
	// GetTempFileNameW() will create the file, we don't want that
	(வெறுமை)DeleteFileW(itmp);

    // Backslashes in a temp file name cause problems when filtering with
    // "sh".  NOTE: This also checks 'shellcmdflag' to help those people who
    // didn't set 'shellslash' but only if not using PowerShell.
    retval = utf16_to_enc(itmp, வெற்று);
    எனில் (retval == வெற்று)
	திரும்பு வெற்று;
    shname = gettail(p_sh);
    எனில் ((*p_shcf == '-' && !(strstr((எழுத்து *)shname, "powershell") != வெற்று
			     || strstr((எழுத்து *)shname, "pwsh") != வெற்று ))
								    || p_ssl)
	க்கு (p = retval; *p; ++p)
	    எனில் (*p == '\\')
		*p = '/';
    திரும்பு retval;

# இல்லையெனில் // MSWIN
    முழு		itmplen;

#  ifdef USE_TMPNAM
    // tmpnam() will make its own name
    p = tmpnam((எழுத்து *)itmp);
    எனில் (p == வெற்று || *p == NUL)
	திரும்பு வெற்று;
#  இல்லையெனில்

#   ifdef VMS_TEMPNAM
    // mktemp() is not working on VMS.  It seems to be
    // a do-nothing function. Therefore we use tempnam().
    vim_snprintf((எழுத்து *)itmp, அளவு(itmp), "VIM%c", extra_char);
    p = (char_u *)tempnam("tmp:", (எழுத்து *)itmp);
    எனில் (p != வெற்று)
    {
	// VMS will use '.LIS' if we don't explicitly specify an extension,
	// and VIM will then be unable to find the file later
	itmplen = vim_snprintf((எழுத்து *)itmp, அளவு(itmp), "%s.txt", p);
	விடுவி(p);
    }
    இல்லையெனில்
	திரும்பு வெற்று;
#   இல்லையெனில்
    STRCPY(itmp, TEMPNAME);
    itmplen = STRLEN_LITERAL(TEMPNAME);
    எனில் ((p = vim_strchr(itmp, '?')) != வெற்று)
	*p = extra_char;
    எனில் (mktemp((எழுத்து *)itmp) == வெற்று)
	திரும்பு வெற்று;
#   endif
#  endif

    திரும்பு vim_strnsave(itmp, (size_t)itmplen);
# endif // MSWIN
#endif // TEMPDIRNAMES
}

#எனில் defined(BACKSLASH_IN_FILENAME)
/*
 * Convert all backslashes in fname to forward slashes in-place, unless when
 * it looks like a URL.
 */
    வெறுமை
forward_slash(char_u *fname)
{
    char_u	*p;

    எனில் (path_with_url(fname))
	திரும்பு;
    க்கு (p = fname; *p != NUL; ++p)
	// The Big5 encoding can have '\' in the trail byte.
	எனில் (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
	    ++p;
	இல்லையெனில் எனில் (*p == '\\')
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
    முழு
match_file_pat(
    char_u	*pattern,		// pattern to match with
    regprog_T	**prog,			// pre-compiled regprog or NULL
    char_u	*fname,			// full path of file name
    char_u	*sfname,		// short file name or NULL
    char_u	*tail,			// tail of path
    முழு		allow_dirs)		// allow matching with dir
{
    regmatch_T	regmatch;
    முழு		result = FALSE;

    regmatch.rm_ic = p_fic; // ignore case if 'fileignorecase' is set
    எனில் (prog != வெற்று)
	regmatch.regprog = *prog;
    இல்லையெனில்
	regmatch.regprog = vim_regcomp(pattern, RE_MAGIC);

    /*
     * Try for a match with the pattern with:
     * 1. the full file name, when the pattern has a '/'.
     * 2. the short file name, when the pattern has a '/'.
     * 3. the tail of the file name, when the pattern has no '/'.
     */
    எனில் (regmatch.regprog != வெற்று
	     && ((allow_dirs
		     && (vim_regexec(&regmatch, fname, (colnr_T)0)
			 || (sfname != வெற்று
			     && vim_regexec(&regmatch, sfname, (colnr_T)0))))
		 || (!allow_dirs && vim_regexec(&regmatch, tail, (colnr_T)0))))
	result = TRUE;

    எனில் (prog != வெற்று)
	*prog = regmatch.regprog;
    இல்லையெனில்
	vim_regfree(regmatch.regprog);
    திரும்பு result;
}

/*
 * Return TRUE if a file matches with a pattern in "list".
 * "list" is a comma-separated list of patterns, like 'wildignore'.
 * "sfname" is the short file name or NULL, "ffname" the long file name.
 */
    முழு
match_file_list(char_u *list, char_u *sfname, char_u *ffname)
{
    char_u	buf[MAXPATHL];
    char_u	*tail;
    char_u	*regpat;
    எழுத்து	allow_dirs;
    முழு		match;
    char_u	*p;

    tail = gettail(sfname);

    // try all patterns in 'wildignore'
    p = list;
    போது (*p)
    {
	copy_option_part(&p, buf, MAXPATHL, ",");
	regpat = file_pat_to_reg_pat(buf, வெற்று, &allow_dirs, FALSE);
	எனில் (regpat == வெற்று)
	    நிறுத்து;
	match = match_file_pat(regpat, வெற்று, ffname, sfname,
						       tail, (முழு)allow_dirs);
	vim_free(regpat);
	எனில் (match)
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
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
    எழுத்து	*allow_dirs,	// Result passed back out in here
    முழு		no_bslash UNUSED) // Don't use a backward slash as pathsep
{
    முழு		size = 2; // '^' at start, '$' at end
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    முழு		i;
    முழு		nested = 0;
    முழு		add_dollar = TRUE;

    எனில் (allow_dirs != வெற்று)
	*allow_dirs = FALSE;
    எனில் (pat_end == வெற்று)
	pat_end = pat + STRLEN(pat);

    க்கு (p = pat; p < pat_end; p++)
    {
	தேர்வு (*p)
	{
	    நிலை '*':
	    நிலை '.':
	    நிலை ',':
	    நிலை '{':
	    நிலை '}':
	    நிலை '~':
		size += 2;	// extra backslash
		நிறுத்து;
#ifdef BACKSLASH_IN_FILENAME
	    நிலை '\\':
	    நிலை '/':
		size += 4;	// could become "[\/]"
		நிறுத்து;
#endif
	    இயல்பு:
		size++;
		எனில் (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		{
		    ++p;
		    ++size;
		}
		நிறுத்து;
	}
    }
    reg_pat = alloc(size + 1);
    எனில் (reg_pat == வெற்று)
	திரும்பு வெற்று;

    i = 0;

    எனில் (pat[0] == '*')
	போது (pat[0] == '*' && pat < pat_end - 1)
	    pat++;
    இல்லையெனில்
	reg_pat[i++] = '^';
    endp = pat_end - 1;
    எனில் (endp >= pat && *endp == '*')
    {
	போது (endp - pat > 0 && *endp == '*')
	    endp--;
	add_dollar = FALSE;
    }
    க்கு (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	தேர்வு (*p)
	{
	    நிலை '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		போது (p[1] == '*')	// "**" matches like "*"
		    ++p;
		நிறுத்து;
	    நிலை '.':
	    நிலை '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		நிறுத்து;
	    நிலை '?':
		reg_pat[i++] = '.';
		நிறுத்து;
	    நிலை '\\':
		எனில் (p[1] == NUL)
		    நிறுத்து;
#ifdef BACKSLASH_IN_FILENAME
		எனில் (!no_bslash)
		{
		    // translate:
		    // "\x" to "\\x"  e.g., "dir\file"
		    // "\*" to "\\.*" e.g., "dir\*.c"
		    // "\?" to "\\."  e.g., "dir\??.c"
		    // "\+" to "\+"   e.g., "fileX\+.c"
		    எனில் ((vim_isfilec(p[1]) || p[1] == '*' || p[1] == '?')
			    && p[1] != '+')
		    {
			reg_pat[i++] = '[';
			reg_pat[i++] = '\\';
			reg_pat[i++] = '/';
			reg_pat[i++] = ']';
			எனில் (allow_dirs != வெற்று)
			    *allow_dirs = TRUE;
			நிறுத்து;
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
		எனில் (*++p == '?'
#ifdef BACKSLASH_IN_FILENAME
			&& no_bslash
#endif
			)
		    reg_pat[i++] = '?';
		இல்லையெனில்
		    எனில் (*p == ',' || *p == '%' || *p == '#'
			       || vim_isspace(*p) || *p == '{' || *p == '}')
			reg_pat[i++] = *p;
		    இல்லையெனில் எனில் (*p == '\\' && p[1] == '\\' && p[2] == '{')
		    {
			reg_pat[i++] = '\\';
			reg_pat[i++] = '{';
			p += 2;
		    }
		    இல்லையெனில்
		    {
			எனில் (allow_dirs != வெற்று && vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
				&& (!no_bslash || *p != '\\')
#endif
				)
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			எனில் (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
			    reg_pat[i++] = *p++;
			reg_pat[i++] = *p;
		    }
		நிறுத்து;
#ifdef BACKSLASH_IN_FILENAME
	    நிலை '/':
		reg_pat[i++] = '[';
		reg_pat[i++] = '\\';
		reg_pat[i++] = '/';
		reg_pat[i++] = ']';
		எனில் (allow_dirs != வெற்று)
		    *allow_dirs = TRUE;
		நிறுத்து;
#endif
	    நிலை '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		நிறுத்து;
	    நிலை '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		நிறுத்து;
	    நிலை ',':
		எனில் (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		இல்லையெனில்
		    reg_pat[i++] = ',';
		நிறுத்து;
	    இயல்பு:
		எனில் (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		    reg_pat[i++] = *p++;
		இல்லையெனில் எனில் (allow_dirs != வெற்று && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		நிறுத்து;
	}
    }
    எனில் (add_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    எனில் (nested != 0)
    {
	எனில் (nested < 0)
	    emsg(_(e_missing_open_curly));
	இல்லையெனில்
	    emsg(_(e_missing_close_curly));
	VIM_CLEAR(reg_pat);
    }
    திரும்பு reg_pat;
}

#எனில் defined(EINTR)
/*
 * Version of read() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    long
read_eintr(முழு fd, வெறுமை *buf, size_t bufsize)
{
    long ret;

    க்கு (;;)
    {
	ret = vim_read(fd, buf, bufsize);
	எனில் (ret >= 0 || errno != EINTR)
	    நிறுத்து;
    }
    திரும்பு ret;
}

/*
 * Version of write() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    long
write_eintr(முழு fd, வெறுமை *buf, size_t bufsize)
{
    long    ret = 0;
    long    wlen;

    // Repeat the write() so long it didn't fail, other than being interrupted
    // by a signal.
    போது (ret < (long)bufsize)
    {
	wlen = vim_write(fd, (எழுத்து *)buf + ret, bufsize - ret);
	எனில் (wlen < 0)
	{
	    எனில் (errno != EINTR)
		நிறுத்து;
	}
	இல்லையெனில்
	    ret += wlen;
    }
    திரும்பு ret;
}
#endif
