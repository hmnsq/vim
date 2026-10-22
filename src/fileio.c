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

#अन्तर्भाव "vim.h"

#जर defined(__TANDEM)
# include <limits.h>		// for SSIZE_MAX
#endif
#जर (defined(UNIX) || defined(VMS)) && defined(FEAT_EVAL)
# include <pwd.h>
# include <grp.h>
#endif
#जर defined(VMS) && defined(HAVE_XOS_R_H)
# include <x11/xos_r.h>
#endif

// Is there any system that doesn't have access()?
#define USE_MCH_ACCESS

// Bitmask with 0x80 set in each byte of a long_u word, used to detect
// non-ASCII bytes (high bit set) in multiple bytes at once.
#define NONASCII_MASK (((long_u)-1 / 0xFF) * 0x80)

#जर defined(__hpux) && !defined(HAVE_DIRFD)
# define dirfd(x) ((x)->__dd_fd)
# define HAVE_DIRFD
#endif

स्थिर char_u *next_fenc(char_u **pp, पूर्णांक *alloced);
#ifdef FEAT_EVAL
स्थिर char_u *readfile_charconvert(char_u *fname, char_u *fenc, पूर्णांक *fdp);
#endif
#ifdef FEAT_CRYPT
स्थिर char_u *check_for_cryptkey(char_u *cryptkey, char_u *ptr, दीर्घ *sizep, off_T *filesizep, पूर्णांक newfile, char_u *fname, पूर्णांक *did_ask);
#endif
स्थिर linenr_T readfile_linenr(linenr_T linecnt, char_u *p, char_u *endp);
स्थिर char_u *check_for_bom(char_u *p, दीर्घ size, पूर्णांक *lenp, पूर्णांक flags);

#ifdef FEAT_EVAL
स्थिर पूर्णांक readdirex_sort;
#endif

    शून्य
filemess(
    buf_T	*buf,
    char_u	*name,
    char_u	*s,
    पूर्णांक		attr)
{
    पूर्णांक		संदेश_scroll_save;
    पूर्णांक		prev_संदेश_col = संदेश_col;
    माप_प्रकार	len;

    जर (संदेश_silent != 0)
	परत;
    संदेश_add_fname(buf, name);	    // put file name in IObuff with quotes

    // If it's extremely long, truncate it.
    len = STRLEN(IObuff);
    जर (len > IOSIZE - 100)
    {
	len = IOSIZE - 100;
	IObuff[len] = NUL;
    }

    // Avoid an over-long translation to cause trouble.
    जर (*s != NUL)
	STRNCPY(IObuff + len, s, 99);

    /*
     * For the first message may have to start a new line.
     * For further ones overwrite the previous one, reset msg_scroll before
     * calling filemess().
     */
    संदेश_scroll_save = संदेश_scroll;
    जर (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	संदेश_scroll = FALSE;
    जर (!संदेश_scroll)	// wait a bit when overwriting an error msg
	check_for_delay(FALSE);
    संदेश_start();
    जर (prev_संदेश_col != 0 && संदेश_col == 0)
	संदेश_putchar('\r');  // overwrite any previous message.
    संदेश_scroll = संदेश_scroll_save;
    संदेश_scrolled_ign = TRUE;
    // may truncate the message to avoid a hit-return prompt
    संदेश_outtrans_attr(संदेश_may_trunc(FALSE, IObuff), attr);
    संदेश_clr_eos();
    out_flush();
    संदेश_scrolled_ign = FALSE;
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
    पूर्णांक
readfile(
    char_u	*fname,
    char_u	*sfname,
    linenr_T	from,
    linenr_T	ओळी_to_skip,
    linenr_T	ओळी_to_read,
    exarg_T	*eap,			// can be NULL!
    पूर्णांक		flags)
{
    पूर्णांक		retval = FAIL;	// jump to "theend" instead of returning
    पूर्णांक		fd = 0;
    पूर्णांक		newfile = (flags & READ_NEW);
    पूर्णांक		check_readonly;
    पूर्णांक		filtering = (flags & READ_FILTER);
    पूर्णांक		read_stdin = (flags & READ_STDIN);
    पूर्णांक		read_buffer = (flags & READ_BUFFER);
    पूर्णांक		read_fifo = (flags & READ_FIFO);
    पूर्णांक		set_options = newfile || read_buffer
					   || (eap != रिक्त && eap->read_edit);
    linenr_T	read_buf_lnum = 1;	// next line to read from curbuf
    colnr_T	read_buf_col = 0;	// next char to read from this line
    char_u	c;
    linenr_T	lnum = from;
    char_u	*ptr = रिक्त;		// pointer into read buffer
    char_u	*buffer = रिक्त;		// read buffer
    char_u	*new_buffer = रिक्त;	// init to shut up gcc
    char_u	*ओळ_start = रिक्त;	// init to shut up gcc
    पूर्णांक		wasempty;		// buffer was empty before reading
    colnr_T	len;
    दीर्घ	size = 0;
    char_u	*p;
    off_T	filesize = 0;
    पूर्णांक		skip_read = FALSE;
#ifdef FEAT_CRYPT
    off_T       filesize_disk = 0;      // file size read from disk
    off_T       filesize_मोजणी = 0;     // counter
    char_u	*cryptkey = रिक्त;
    पूर्णांक		did_ask_for_किल्ली = FALSE;
#endif
#ifdef FEAT_PERSISTENT_UNDO
    context_sha256_T sha_ctx;
    पूर्णांक		read_undo_file = FALSE;
#endif
    पूर्णांक		split = 0;		// number of split lines
#define UNKNOWN	 0x0fffffff		// file size is unknown
    linenr_T	linecnt;
    पूर्णांक		error = FALSE;		// errors encountered
    पूर्णांक		ff_error = EOL_UNKNOWN; // file format with errors
    दीर्घ	linerest = 0;		// remaining chars in line
#ifdef UNIX
    पूर्णांक		perm = 0;
    पूर्णांक		swap_mode = -1;		// protection bits for swap file
#अन्यथा
    पूर्णांक		perm;
#endif
    पूर्णांक		fileformat = 0;		// end-of-line format
    पूर्णांक		keep_fileformat = FALSE;
    stat_T	st;
    पूर्णांक		file_readonly;
    linenr_T	skip_मोजणी = 0;
    linenr_T	read_मोजणी = 0;
    पूर्णांक		संदेश_save = संदेश_scroll;
    linenr_T	read_no_eol_lnum = 0;   // non-zero lnum when last line of
					// last read was missing the eol
    पूर्णांक		try_mac;
    पूर्णांक		try_dos;
    पूर्णांक		try_unix;
    पूर्णांक		file_rewind = FALSE;
    पूर्णांक		can_retry;
    linenr_T	conv_error = 0;		// line nr with conversion error
    linenr_T	illegal_byte = 0;	// line nr with illegal byte
    पूर्णांक		keep_dest_enc = FALSE;	// don't retry when char doesn't fit
					// in destination encoding
    पूर्णांक		bad_char_behavior = BAD_REPLACE;
					// BAD_KEEP, BAD_DROP or character to
					// replace with
    char_u	*tmpname = रिक्त;	// name of 'charconvert' output file
    पूर्णांक		fio_flags = 0;
    char_u	*fenc;			// fileencoding to use
    पूर्णांक		fenc_alloced;		// fenc_next is in allocated memory
    char_u	*fenc_next = रिक्त;	// next item in 'fencs' or NULL
    पूर्णांक		advance_fenc = FALSE;
    दीर्घ	real_size = 0;
#ifdef USE_ICONV
    iconv_t	iconv_fd = (iconv_t)-1;	// descriptor for iconv() or -1
# ifdef FEAT_EVAL
    पूर्णांक		did_iconv = FALSE;	// TRUE when iconv() failed and trying
					// 'charconvert' next
# endif
#endif
    पूर्णांक		converted = FALSE;	// TRUE if conversion done
    पूर्णांक		notconverted = FALSE;	// TRUE if conversion wanted but it
					// wasn't possible
    char_u	conv_rest[CONV_RESTLEN];
    पूर्णांक		conv_restlen = 0;	// nr of bytes in conv_rest[]
    pos_T	orig_start;
    buf_T	*old_curbuf;
    char_u	*old_b_ffname;
    char_u	*old_b_fname;
    पूर्णांक		using_b_ffname;
    पूर्णांक		using_b_fname;
    स्थिर अक्षर *संदेश_is_a_directory = N_("is a directory");
#ifdef FEAT_CRYPT
    पूर्णांक		eof = FALSE;
#endif
#ifdef FEAT_SODIUM
    पूर्णांक		may_need_lseek = FALSE;
#endif
    माप_प्रकार	fnamelen = 0;

    curbuf->b_au_did_filetype = false; // reset before triggering any autocommands

    curbuf->b_no_eol_lnum = 0;	// in case it was set by the previous read

    /*
     * If there is no file name yet, use the one for the read file.
     * BF_NOTEDITED is set to reflect this.
     * Don't do this for a read from a filter.
     * Only do this when 'cpoptions' contains the 'f' flag.
     */
    जर (curbuf->b_ffname == रिक्त
	    && !filtering
	    && fname != रिक्त
	    && vim_strchr(p_cpo, CPO_FNAMER) != रिक्त
	    && !(flags & READ_DUMMY))
    {
	जर (set_rw_fname(fname, sfname) == FAIL)
	    जा theend;
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
    जर (sfname == रिक्त)
	sfname = fname;
#जर defined(UNIX)
    fname = sfname;
#endif

    /*
     * The BufReadCmd and FileReadCmd events intercept the reading process by
     * executing the associated commands instead.
     */
    जर (!filtering && !read_stdin && !read_buffer)
    {
	orig_start = curbuf->b_op_start;

	// Set '[ mark to the line above where the lines go (line 1 if zero).
	curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
	curbuf->b_op_start.col = 0;

	जर (newfile)
	{
	    जर (apply_autocmds_exarg(EVENT_BUFREADCMD, रिक्त, sfname,
							  FALSE, curbuf, eap))
	    {
		retval = OK;
#ifdef FEAT_EVAL
		जर (aborting())
		    retval = FAIL;
#endif
		// The BufReadCmd code usually uses ":read" to get the text and
		// perhaps ":file" to change the buffer name. But we should
		// consider this to work like ":edit", thus reset the
		// BF_NOTEDITED flag.  Then ":write" will work to overwrite the
		// same file.
		जर (retval == OK)
		    curbuf->b_flags &= ~BF_NOTEDITED;
		जा theend;
	    }
	}
	अन्यथा जर (apply_autocmds_exarg(EVENT_FILEREADCMD, sfname, sfname,
							    FALSE, रिक्त, eap))
	{
#ifdef FEAT_EVAL
	    retval = aborting() ? FAIL : OK;
#अन्यथा
	    retval = OK;
#endif
	    जा theend;
	}

	curbuf->b_op_start = orig_start;

	जर (flags & READ_NOFILE)
	{
	    // Return NOTDONE instead of FAIL so that BufEnter can be triggered
	    // and other operations don't fail.
	    retval = NOTDONE;
	    जा theend;
	}
    }

    जर ((shortmess(SHM_OVER) || curbuf->b_help) && p_verbose == 0)
	संदेश_scroll = FALSE;	// overwrite previous file message
    अन्यथा
	संदेश_scroll = TRUE;	// don't overwrite previous file message

    जर (fname != रिक्त && *fname != NUL)
    {
	fnamelen = STRLEN(fname);

	// If the name is too long we might crash further on, quit here.
	जर (fnamelen >= MAXPATHL)
	{
	    filemess(curbuf, fname, (char_u *)_("Illegal file name"), 0);
	    संदेश_end();
	    संदेश_scroll = संदेश_save;
	    जा theend;
	}

	// If the name ends in a path separator, we can't open it.  Check here,
	// because reading the file may actually work, but then creating the
	// swap file may destroy it!  Reported on MS-DOS and Win 95.
	जर (after_pathsep(fname, fname + fnamelen))
	{
	    filemess(curbuf, fname, (char_u *)_(संदेश_is_a_directory), 0);
	    संदेश_end();
	    संदेश_scroll = संदेश_save;
	    retval = NOTDONE;
	    जा theend;
	}
    }
#जर defined(UNIX) || defined(VMS)
    जर (!read_stdin && fname != रिक्त)
	perm = mch_getperm(fname);
#endif

    जर (!read_stdin && !read_buffer && !read_fifo)
    {
#जर defined(UNIX) || defined(VMS)
	जर (perm >= 0 && !S_ISREG(perm)		    // not a regular file ...
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
	    जर (S_ISDIR(perm))
	    {
		filemess(curbuf, fname, (char_u *)_(संदेश_is_a_directory), 0);
		retval = NOTDONE;
	    }
	    अन्यथा
		filemess(curbuf, fname, (char_u *)_("is not a file"), 0);
	    संदेश_end();
	    संदेश_scroll = संदेश_save;
	    जा theend;
	}
#endif
#जर defined(MSWIN)
	/*
	 * MS-Windows allows opening a device, but we will probably get stuck
	 * trying to read it.
	 */
	जर (!p_odev && mch_nodetype(fname) == गाठ_WRITABLE)
	{
	    filemess(curbuf, fname, (char_u *)_("is a device (disabled with 'opendevice' option)"), 0);
	    संदेश_end();
	    संदेश_scroll = संदेश_save;
	    जा theend;
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
    जर (check_readonly && !readonlymode)
	curbuf->b_p_ro = FALSE;

    जर (newfile && !read_stdin && !read_buffer && !read_fifo)
    {
	// Remember time of file.
	जर (mch_stat((अक्षर *)fname, &st) >= 0)
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
	अन्यथा
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
    जर (read_stdin)
    {
#जर defined(MSWIN)
	// Force binary I/O on stdin to avoid CR-LF -> LF conversion.
	setmode(0, O_द्विमान);
#endif
    }
    अन्यथा जर (!read_buffer)
    {
#ifdef USE_MCH_ACCESS
	जर (
# ifdef UNIX
	    !(perm & 0222) ||
# endif
				mch_access((अक्षर *)fname, W_OK))
	    file_readonly = TRUE;
	fd = mch_open((अक्षर *)fname, O_RDONLY | O_EXTRA, 0);
#अन्यथा
	जर (!newfile
		|| readonlymode
		|| (fd = mch_open((अक्षर *)fname, O_RDWR | O_EXTRA, 0)) < 0)
	{
	    file_readonly = TRUE;
	    // try to open ro
	    fd = mch_open((अक्षर *)fname, O_RDONLY | O_EXTRA, 0);
	}
#endif
    }

    जर (fd < 0)			    // cannot open at all
    {
#ifndef UNIX
	पूर्णांक	isdir_f;
#endif
	संदेश_scroll = संदेश_save;
#ifndef UNIX
	/*
	 * On Amiga we can't open a directory, check here.
	 */
	isdir_f = (mch_isdir(fname));
	perm = mch_getperm(fname);  // check if the file exists
	जर (isdir_f)
	{
	    filemess(curbuf, sfname, (char_u *)_(संदेश_is_a_directory), 0);
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	}
	अन्यथा
#endif
	    जर (newfile)
	    {
		जर (perm < 0
#ifdef ENOENT
			&& त्रुटि_सं == ENOENT
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
		    जर (!bt_dontwrite(curbuf))
		    {
			check_need_swap(newfile);
			// SwapExists autocommand may mess things up
			जर (curbuf != old_curbuf
				|| (using_b_ffname
					&& (old_b_ffname != curbuf->b_ffname))
				|| (using_b_fname
					 && (old_b_fname != curbuf->b_fname)))
			{
			    emsg(_(e_autocommands_changed_buffer_or_buffer_name));
			    जा theend;
			}
		    }
		    जर (dir_of_file_exists(fname))
			filemess(curbuf, sfname,
					      (char_u *)new_file_message(), 0);
		    अन्यथा
			filemess(curbuf, sfname,
					   (char_u *)_("[New DIRECTORY]"), 0);
#ifdef FEAT_VIMINFO
		    // Even though this is a new file, it might have been
		    // edited before and deleted.  Get the old marks.
		    check_marks_read();
#endif
		    // Set forced 'fileencoding'.
		    जर (eap != रिक्त)
			set_forced_fenc(eap);
		    apply_autocmds_exarg(EVENT_BUFNEWFILE, sfname, sfname,
							  FALSE, curbuf, eap);
		    // remember the current fileformat
		    save_file_ff(curbuf);

#जर defined(FEAT_EVAL)
		    जर (!aborting())   // autocmds may abort script processing
#endif
			retval = OK;	    // a new file is not an error
		    जा theend;
		}
		अन्यथा
		{
		    filemess(curbuf, sfname, (char_u *)(
#ifdef EFBIG
			    (त्रुटि_सं == EFBIG) ? _("[File too big]") :
#endif
#ifdef EOVERFLOW
			    (त्रुटि_सं == EOVERFLOW) ? _("[File too big]") :
#endif
						_("[Permission Denied]")), 0);
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
		}
	    }

	जा theend;
    }

    /*
     * Only set the 'ro' flag for readonly files the first time they are
     * loaded.	Help files always get readonly mode
     */
    जर ((check_readonly && file_readonly) || curbuf->b_help)
	curbuf->b_p_ro = TRUE;

    जर (set_options)
    {
	// Don't change 'eol' if reading from buffer as it will already be
	// correctly set when reading stdin.
	जर (!read_buffer)
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
    जर (!bt_dontwrite(curbuf))
    {
	check_need_swap(newfile);
	जर (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))))
	{
	    emsg(_(e_autocommands_changed_buffer_or_buffer_name));
	    जर (!read_buffer)
		बंद(fd);
	    जा theend;
	}
#ifdef UNIX
	// Set swap file protection bits after creating it.
	जर (swap_mode > 0 && curbuf->b_ml.ml_mfp != रिक्त
			  && curbuf->b_ml.ml_mfp->mf_fname != रिक्त)
	{
	    char_u *swap_fname = curbuf->b_ml.ml_mfp->mf_fname;

	    /*
	     * If the group-read bit is set but not the world-read bit, then
	     * the group must be equal to the group of the original file.  If
	     * we can't make that happen then reset the group-read bit.  This
	     * avoids making the swap file readable to more users when the
	     * primary group of the user is too permissive.
	     */
	    जर ((swap_mode & 044) == 040)
	    {
		stat_T	swap_st;

		जर (mch_stat((अक्षर *)swap_fname, &swap_st) >= 0
			&& st.st_gid != swap_st.st_gid
# ifdef HAVE_FCHOWN
			&& fchown(curbuf->b_ml.ml_mfp->mf_fd, -1, st.st_gid)
									  == -1
# endif
		   )
		    swap_mode &= 0600;
	    }

	    (शून्य)mch_setperm(swap_fname, (दीर्घ)swap_mode);
	}
#endif
    }

    // If "Quit" selected at ATTENTION dialog, don't load the file
    जर (swap_exists_action == SEA_QUIT)
    {
	जर (!read_buffer && !read_stdin)
	    बंद(fd);
	जा theend;
    }

    ++no_wait_return;	    // don't wait for return yet

    /*
     * Set '[ mark to the line above where the lines go (line 1 if zero).
     */
    orig_start = curbuf->b_op_start;
    curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
    curbuf->b_op_start.col = 0;

    try_mac = (vim_strchr(p_ffs, 'm') != रिक्त);
    try_dos = (vim_strchr(p_ffs, 'd') != रिक्त);
    try_unix = (vim_strchr(p_ffs, 'x') != रिक्त);

    जर (!read_buffer)
    {
	पूर्णांक	m = संदेश_scroll;
	पूर्णांक	n = संदेश_scrolled;

	/*
	 * The file must be closed again, the autocommands may want to change
	 * the file before reading it.
	 */
	जर (!read_stdin)
	    बंद(fd);		// ignore errors

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	संदेश_scroll = TRUE;
	जर (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPRE, रिक्त, sfname,
							  FALSE, curbuf, eap);
	अन्यथा जर (read_stdin)
	    apply_autocmds_exarg(EVENT_STDINREADPRE, रिक्त, sfname,
							  FALSE, curbuf, eap);
	अन्यथा जर (newfile)
	    apply_autocmds_exarg(EVENT_BUFREADPRE, रिक्त, sfname,
							  FALSE, curbuf, eap);
	अन्यथा
	    apply_autocmds_exarg(EVENT_FILEREADPRE, sfname, sfname,
							    FALSE, रिक्त, eap);
	// autocommands may have changed it
	try_mac = (vim_strchr(p_ffs, 'm') != रिक्त);
	try_dos = (vim_strchr(p_ffs, 'd') != रिक्त);
	try_unix = (vim_strchr(p_ffs, 'x') != रिक्त);
	curbuf->b_op_start = orig_start;

	जर (संदेश_scrolled == n)
	    संदेश_scroll = m;

#ifdef FEAT_EVAL
	जर (aborting())	    // autocmds may abort script processing
	{
	    --no_wait_return;
	    संदेश_scroll = संदेश_save;
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    जा theend;
	}
#endif
	/*
	 * Don't allow the autocommands to change the current buffer.
	 * Try to re-open the file.
	 *
	 * Don't allow the autocommands to change the buffer name either
	 * (cd for example) if it invalidates fname or sfname.
	 */
	जर (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))
		|| (fd = mch_open((अक्षर *)fname, O_RDONLY | O_EXTRA, 0)) < 0))
	{
	    --no_wait_return;
	    संदेश_scroll = संदेश_save;
	    जर (fd < 0)
		emsg(_(e_readpre_autocommands_made_file_unreadable));
	    अन्यथा
		emsg(_(e_readpre_autocommands_must_not_change_current_buffer));
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    जा theend;
	}
    }

    // Autocommands may add lines to the file, need to check if it is empty
    wasempty = (curbuf->b_ml.ml_flags & ML_EMPTY);

    जर (!recoverymode && !filtering && !(flags & READ_DUMMY))
    {
	/*
	 * Show the user that we are busy reading the input.  Sometimes this
	 * may take a while.  When reading from stdin another program may
	 * still be running, don't move the cursor to the last line, unless
	 * always using the GUI.
	 */
	जर (read_stdin)
	{
	    जर (!is_not_a_term())
	    {
#ifndef ALWAYS_USE_GUI
# ifdef VIMDLL
		जर (!gui.in_use)
# endif
		    mch_संदेश(_("Vim: Reading from stdin...\n"));
#endif
#ifdef FEAT_GUI
		// Also write a message in the GUI window, if there is one.
		जर (gui.in_use && !gui.dying && !gui.starting)
		{
		    माप_प्रकार  plen = STRLEN(_("Reading from stdin..."));

		    // make a copy, gui_write() may try to change it
		    p = vim_strnsave((char_u *)_("Reading from stdin..."), plen);
		    जर (p != रिक्त)
		    {
			gui_write(p, (पूर्णांक)plen);
			vim_free(p);
		    }
		}
#endif
	    }
	}
	अन्यथा जर (!read_buffer)
	    filemess(curbuf, sfname, (char_u *)"", 0);
    }

    संदेश_scroll = FALSE;			// overwrite the file message

    /*
     * Set linecnt now, before the "retry" caused by a wrong guess for
     * fileformat, and after the autocommands, which may change them.
     */
    linecnt = curbuf->b_ml.ml_ओळ_मोजणी;

    // "++bad=" argument.
    जर (eap != रिक्त && eap->bad_char != 0)
    {
	bad_char_behavior = eap->bad_char;
	जर (set_options)
	    curbuf->b_bad_char = eap->bad_char;
    }
    अन्यथा
	curbuf->b_bad_char = 0;

    /*
     * Decide which 'encoding' to use or use first.
     */
    जर (eap != रिक्त && eap->force_enc != 0)
    {
	fenc = enc_canonize(eap->cmd + eap->force_enc);
	fenc_alloced = TRUE;
	keep_dest_enc = TRUE;
    }
    अन्यथा जर (curbuf->b_p_bin)
    {
	fenc = (char_u *)"";		// binary: don't convert
	fenc_alloced = FALSE;
    }
    अन्यथा जर (curbuf->b_help)
    {
	char_u	    firstline[80];
	पूर्णांक	    fc;

	// Help files are either utf-8 or latin1.  Try utf-8 first, if this
	// fails it must be latin1.
	// Always do this when 'encoding' is "utf-8".  Otherwise only do
	// this when needed to avoid [converted] remarks all the time.
	// It is needed when the first line contains non-ASCII characters.
	// That is only in *.??x files.
	fenc = (char_u *)"latin1";
	c = enc_utf8;
	जर (!c && !read_stdin)
	{
	    fc = fname[fnamelen - 1];
	    जर (TOLOWER_ASC(fc) == 'x')
	    {
		// Read the first line (and a bit more).  Immediately rewind to
		// the start of the file.  If the read() fails "len" is -1.
		len = read_eintr(fd, firstline, 80);
		vim_lseek(fd, (off_T)0L, शुरू_से);
		साठी (p = firstline; p < firstline + len; ++p)
		    जर (*p >= 0x80)
		    {
			c = TRUE;
			थांबा;
		    }
	    }
	}

	जर (c)
	{
	    fenc_next = fenc;
	    fenc = (char_u *)"utf-8";

	    // When the file is utf-8 but a character doesn't fit in
	    // 'encoding' don't retry.  In help text editing utf-8 bytes
	    // doesn't make sense.
	    जर (!enc_utf8)
		keep_dest_enc = TRUE;
	}
	fenc_alloced = FALSE;
    }
    अन्यथा जर (*p_fencs == NUL)
    {
	fenc = curbuf->b_p_fenc;	// use format from buffer
	fenc_alloced = FALSE;
    }
    अन्यथा
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

    जर (file_rewind)
    {
	जर (read_buffer)
	{
	    read_buf_lnum = 1;
	    read_buf_col = 0;
	}
	अन्यथा जर (read_stdin || vim_lseek(fd, (off_T)0L, शुरू_से) != 0)
	{
	    // Can't rewind the file, give up.
	    error = TRUE;
	    जा failed;
	}
	// Delete the previously read lines.
	पर्यंत (lnum > from)
	    ml_delete(lnum--);
	file_rewind = FALSE;
	जर (set_options)
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
    जर (keep_fileformat)
	keep_fileformat = FALSE;
    अन्यथा
    {
	जर (eap != रिक्त && eap->force_ff != 0)
	{
	    fileformat = get_fileformat_force(curbuf, eap);
	    try_unix = try_dos = try_mac = FALSE;
	}
	अन्यथा जर (curbuf->b_p_bin)
	    fileformat = EOL_UNIX;		// binary: use Unix format
	अन्यथा जर (*p_ffs == NUL)
	    fileformat = get_fileformat(curbuf);// use format from buffer
	अन्यथा
	    fileformat = EOL_UNKNOWN;		// detect from file
    }

#ifdef USE_ICONV
    जर (iconv_fd != (iconv_t)-1)
    {
	// aborted conversion with iconv(), close the descriptor
	iconv_close(iconv_fd);
	iconv_fd = (iconv_t)-1;
    }
#endif

    जर (advance_fenc)
    {
	/*
	 * Try the next entry in 'fileencodings'.
	 */
	advance_fenc = FALSE;

	जर (eap != रिक्त && eap->force_enc != 0)
	{
	    // Conversion given with "++cc=" wasn't possible, read
	    // without conversion.
	    notconverted = TRUE;
	    conv_error = 0;
	    जर (fenc_alloced)
		vim_free(fenc);
	    fenc = (char_u *)"";
	    fenc_alloced = FALSE;
	}
	अन्यथा
	{
	    जर (fenc_alloced)
		vim_free(fenc);
	    जर (fenc_next != रिक्त)
	    {
		fenc = next_fenc(&fenc_next, &fenc_alloced);
	    }
	    अन्यथा
	    {
		fenc = (char_u *)"";
		fenc_alloced = FALSE;
	    }
	}
	जर (tmpname != रिक्त)
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
    जर (converted)
    {
	// "ucs-bom" means we need to check the first bytes of the file
	// for a BOM.
	जर (STRCMP(fenc, ENC_UCSBOM) == 0)
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
	अन्यथा जर (enc_utf8 || STRCMP(p_enc, "latin1") == 0)
	    fio_flags = get_fio_flags(fenc);

#ifdef MSWIN
	/*
	 * Conversion from an MS-Windows codepage to UTF-8 or another codepage
	 * is handled with MultiByteToWideChar().
	 */
	जर (fio_flags == 0)
	    fio_flags = get_win_fio_flags(fenc);
#endif

#ifdef MACOS_CONVERT
	// Conversion from Apple MacRoman to latin1 or UTF-8
	जर (fio_flags == 0)
	    fio_flags = get_mac_fio_flags(fenc);
#endif

#ifdef USE_ICONV
	/*
	 * Try using iconv() if we can't convert internally.
	 */
	जर (fio_flags == 0
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
	जर (fio_flags == 0 && !read_stdin && !read_buffer && *p_ccv != NUL
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
	    जर (tmpname == रिक्त)
	    {
		tmpname = readfile_charconvert(fname, fenc, &fd);
		जर (tmpname == रिक्त)
		{
		    // Conversion failed.  Try another one.
		    advance_fenc = TRUE;
		    जर (fd < 0)
		    {
			// Re-opening the original file failed!
			emsg(_(e_conversion_mad_file_unreadable));
			error = TRUE;
			जा failed;
		    }
		    जा retry;
		}
	    }
	}
	अन्यथा
#endif
	{
	    जर (fio_flags == 0
#ifdef USE_ICONV
		    && iconv_fd == (iconv_t)-1
#endif
	       )
	    {
		// Conversion wanted but we can't.
		// Try the next conversion in 'fileencodings'
		advance_fenc = TRUE;
		जा retry;
	    }
	}
    }

    // Set "can_retry" when it's possible to rewind the file and try with
    // another "fenc" value.  It's FALSE when no other "fenc" to try, reading
    // stdin or fixed at a specific encoding.
    can_retry = (*fenc != NUL && !read_stdin && !read_fifo && !keep_dest_enc);

    जर (!skip_read)
    {
	linerest = 0;
	filesize = 0;
#ifdef FEAT_CRYPT
	filesize_मोजणी = 0;
#endif
	skip_मोजणी = ओळी_to_skip;
	read_मोजणी = ओळी_to_read;
	conv_restlen = 0;
#ifdef FEAT_PERSISTENT_UNDO
	read_undo_file = (newfile && (flags & READ_KEEP_UNDO) == 0
				  && curbuf->b_ffname != रिक्त
				  && curbuf->b_p_udf
				  && !filtering
				  && !read_fifo
				  && !read_stdin
				  && !read_buffer);
	जर (read_undo_file)
	    sha256_start(&sha_ctx);
#endif
#ifdef FEAT_CRYPT
	जर (curbuf->b_cryptstate != रिक्त)
	{
	    // Need to free the state, but keep the key, don't want to ask for
	    // it again.
	    crypt_free_state(curbuf->b_cryptstate);
	    curbuf->b_cryptstate = रिक्त;
	}
#endif
    }

    पर्यंत (!error && !got_int)
    {
	/*
	 * We allocate as much space for the file as we can get, plus
	 * space for the old line plus room for one terminating NUL.
	 * The amount is limited by the fact that read() only can read
	 * up to max_unsigned characters (and other things).
	 */
	जर (!skip_read)
	{
#जर defined(SSIZE_MAX) && (SSIZE_MAX < 0x10000L)
		size = SSIZE_MAX;		    // use max I/O size, 52K
#अन्यथा
		// Use buffer >= 64K.  Add linerest to double the size if the
		// line gets very long, to avoid a lot of copying. But don't
		// read more than 1 Mbyte at a time, so we can be interrupted.
		size = 0x10000L + linerest;
		जर (size > 0x100000L)
		    size = 0x100000L;
#endif
	}

	// Protect against the argument of lalloc() going negative.
	// Also split lines that are too long for colnr_T.  After this check
	// passes, we read up to 'size' more bytes.  We must ensure that even
	// after that read, the line length won't exceed MAXCOL - 1 (because
	// we add 1 for the NUL when casting to colnr_T).  If this check fires,
	// we insert a synthetic newline immediately, so linerest doesn't grow.
	जर (size < 0 || size + linerest + 1 < 0 || linerest >= MAXCOL - size)
	{
	    ++split;
	    *ptr = NL;		    // split line by inserting a NL
	    size = 1;
	}
	अन्यथा
	{
	    जर (!skip_read)
	    {
		साठी ( ; size >= 10; size = (दीर्घ)((long_u)size >> 1))
		{
		    जर ((new_buffer = lalloc(size + linerest + 1,
							      FALSE)) != रिक्त)
			थांबा;
		}
		जर (new_buffer == रिक्त)
		{
		    do_outofmem_संदेश((long_u)(size * 2 + linerest + 1));
		    error = TRUE;
		    थांबा;
		}
		जर (linerest)	// copy characters from the previous buffer
		    mch_memmove(new_buffer, ptr - linerest, (माप_प्रकार)linerest);
		vim_free(buffer);
		buffer = new_buffer;
		ptr = buffer + linerest;
		ओळ_start = buffer;

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
		real_size = (पूर्णांक)size;
#ifdef USE_ICONV
		जर (iconv_fd != (iconv_t)-1)
		    size = size / ICONV_MULT;
		अन्यथा
#endif
		    जर (fio_flags & FIO_LATIN1)
		    size = size / 2;
		अन्यथा जर (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    size = (size * 2 / 3) & ~1;
		अन्यथा जर (fio_flags & FIO_UCS4)
		    size = (size * 2 / 3) & ~3;
		अन्यथा जर (fio_flags == FIO_UCSBOM)
		    size = size / ICONV_MULT;	// worst case
#ifdef MSWIN
		अन्यथा जर (fio_flags & FIO_CODEPAGE)
		    size = size / ICONV_MULT;	// also worst case
#endif
#ifdef MACOS_CONVERT
		अन्यथा जर (fio_flags & FIO_MACROMAN)
		    size = size / ICONV_MULT;	// also worst case
#endif

		जर (conv_restlen > 0)
		{
		    // Insert unconverted bytes from previous line.
		    mch_memmove(ptr, conv_rest, conv_restlen);
		    ptr += conv_restlen;
		    size -= conv_restlen;
		}

		जर (read_buffer)
		{
		    /*
		     * Read bytes from curbuf.  Used for converting text read
		     * from stdin.
		     */
		    जर (read_buf_lnum > from)
			size = 0;
		    अन्यथा
		    {
			पूर्णांक	n, ni;
			दीर्घ	tlen;

			tlen = 0;
			साठी (;;)
			{
			    p = ml_get(read_buf_lnum) + read_buf_col;
			    n = ml_get_len(read_buf_lnum) - read_buf_col;
			    जर ((पूर्णांक)tlen + n + 1 > size)
			    {
				// Filled up to "size", append partial line.
				// Change NL to NUL to reverse the effect done
				// below.
				n = (पूर्णांक)(size - tlen);
				साठी (ni = 0; ni < n; ++ni)
				{
				    जर (p[ni] == NL)
					ptr[tlen++] = NUL;
				    अन्यथा
					ptr[tlen++] = p[ni];
				}
				read_buf_col += n;
				थांबा;
			    }

			    // Append whole line and new-line.  Change NL
			    // to NUL to reverse the effect done below.
			    साठी (ni = 0; ni < n; ++ni)
			    {
				जर (p[ni] == NL)
				    ptr[tlen++] = NUL;
				अन्यथा
				    ptr[tlen++] = p[ni];
			    }
			    ptr[tlen++] = NL;
			    read_buf_col = 0;
			    जर (++read_buf_lnum > from)
			    {
				// When the last line didn't have an
				// end-of-line don't add it now either.
				जर (!curbuf->b_p_eol)
				    --tlen;
				size = tlen;
#ifdef FEAT_CRYPT
				eof = TRUE;
#endif
				थांबा;
			    }
			}
		    }
		}
		अन्यथा
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
		    जर (filesize == 0)
		    {
			// set size to 8K + Sodium Crypt Metadata
			size = WRITEBUFSIZE + crypt_get_max_header_len()
			    + crypto_secretstream_xchacha20poly1305_HEADERBYTES
				+ crypto_secretstream_xchacha20poly1305_ABYTES;
			may_need_lseek = TRUE;
		    }

		    अन्यथा जर (filesize > 0 && (curbuf->b_cryptstate != रिक्त
				&& crypt_method_is_sodium(
					     curbuf->b_cryptstate->method_nr)))
		    {
			size = WRITEBUFSIZE + crypto_secretstream_xchacha20poly1305_ABYTES;
			// need to rewind by - add_len from CRYPT_M_SOD2 (see
			// description above)
			जर (curbuf->b_cryptstate->method_nr == CRYPT_M_SOD
						     && !eof && may_need_lseek)
			{
			    lseek(fd, crypt_get_header_len(
					       curbuf->b_cryptstate->method_nr)
				       - crypt_get_max_header_len(), प्रस्तुत_से);
			    may_need_lseek = FALSE;
			}
		    }
#endif
		    दीर्घ read_size = size;
		    size = read_eintr(fd, ptr, read_size);
#ifdef FEAT_CRYPT
		    // Did we reach end of file?
		    filesize_मोजणी += size;
		    eof = (size < read_size || filesize_मोजणी == filesize_disk);
#endif
		}

#ifdef FEAT_CRYPT
		/*
		 * At start of file: Check for magic number of encryption.
		 */
		जर (filesize == 0 && size > 0)
		{
		    cryptkey = check_for_cryptkey(cryptkey, ptr, &size,
						  &filesize, newfile, sfname,
						  &did_ask_for_किल्ली);
# जर defined(CRYPT_NOT_INPLACE) && defined(FEAT_PERSISTENT_UNDO)
		    जर (curbuf->b_cryptstate != रिक्त
				 && !crypt_works_inplace(curbuf->b_cryptstate))
			// reading undo file requires crypt_decode_inplace()
			read_undo_file = FALSE;
# endif
		}
		/*
		 * Decrypt the read bytes.  This is done before checking for
		 * EOF because the crypt layer may be buffering.
		 */
		जर (cryptkey != रिक्त && curbuf->b_cryptstate != रिक्त
								   && size > 0)
		{
# ifdef CRYPT_NOT_INPLACE
		    जर (crypt_works_inplace(curbuf->b_cryptstate))
		    {
# endif
			crypt_decode_inplace(curbuf->b_cryptstate, ptr,
								    size, eof);
# ifdef CRYPT_NOT_INPLACE
		    }
		    अन्यथा
		    {
			char_u	*newptr = रिक्त;
			पूर्णांक	decrypted_size;

			decrypted_size = crypt_decode_alloc(
				    curbuf->b_cryptstate, ptr, size,
								 &newptr, eof);

			जर (decrypted_size < 0)
			{
			    // error message already given
			    error = TRUE;
			    vim_free(newptr);
			    थांबा;
			}
			// If the crypt layer is buffering, not producing
			// anything yet, need to read more.
			जर (decrypted_size == 0)
			    सुरूठेवा;

			जर (linerest == 0)
			{
			    // Simple case: reuse returned buffer (may be
			    // NULL, checked later).
			    new_buffer = newptr;
			}
			अन्यथा
			{
			    long_u	new_size;

			    // Need new buffer to add bytes carried over.
			    new_size = (long_u)(decrypted_size + linerest + 1);
			    new_buffer = lalloc(new_size, FALSE);
			    जर (new_buffer == रिक्त)
			    {
				do_outofmem_संदेश(new_size);
				error = TRUE;
				थांबा;
			    }

			    mch_memmove(new_buffer, buffer, linerest + conv_restlen);
			    जर (newptr != रिक्त)
				mch_memmove(new_buffer + linerest + conv_restlen,
					newptr, decrypted_size);
			    vim_free(newptr);
			}

			जर (new_buffer != रिक्त)
			{
			    vim_free(buffer);
			    buffer = new_buffer;
			    new_buffer = रिक्त;
			    ओळ_start = buffer;
			    ptr = buffer + linerest + conv_restlen;
			    real_size = size;
			}
			size = decrypted_size;
		    }
# endif
		}
#endif

		जर (size <= 0)
		{
		    जर (size < 0)		    // read error
			error = TRUE;
		    अन्यथा जर (conv_restlen > 0)
		    {
			/*
			 * Reached end-of-file but some trailing bytes could
			 * not be converted.  Truncated file?
			 */

			// When we did a conversion report an error.
			जर (fio_flags != 0
#ifdef USE_ICONV
				|| iconv_fd != (iconv_t)-1
#endif
			   )
			{
			    जर (can_retry)
				जा rewind_retry;
			    जर (conv_error == 0)
				conv_error = curbuf->b_ml.ml_ओळ_मोजणी
								- linecnt + 1;
			}
			// Remember the first linenr with an illegal byte
			अन्यथा जर (illegal_byte == 0)
			    illegal_byte = curbuf->b_ml.ml_ओळ_मोजणी
								- linecnt + 1;
			जर (bad_char_behavior == BAD_DROP)
			{
			    *(ptr - conv_restlen) = NUL;
			    conv_restlen = 0;
			}
			अन्यथा
			{
			    // Replace the trailing bytes with the replacement
			    // character if we were converting; if we weren't,
			    // leave the UTF8 checking code to do it, as it
			    // works slightly differently.
			    जर (bad_char_behavior != BAD_KEEP && (fio_flags != 0
#ifdef USE_ICONV
				    || iconv_fd != (iconv_t)-1
#endif
			       ))
			    {
				पर्यंत (conv_restlen > 0)
				{
				    *(--ptr) = bad_char_behavior;
				    --conv_restlen;
				}
			    }
			    fio_flags = 0;	// don't convert this
#ifdef USE_ICONV
			    जर (iconv_fd != (iconv_t)-1)
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
	    जर ((filesize == 0
#ifdef FEAT_CRYPT
		   || (cryptkey != रिक्त
			&& filesize == crypt_get_header_len(
						 crypt_get_method_nr(curbuf)))
#endif
		       )
		    && (fio_flags == FIO_UCSBOM
			|| (!curbuf->b_p_bomb
			    && tmpname == रिक्त
			    && (*fenc == 'u' || (*fenc == NUL && enc_utf8)))))
	    {
		char_u	*ccname;
		पूर्णांक	blen;

		// no BOM detection in a short file or in binary mode
		जर (size < 2 || curbuf->b_p_bin)
		    ccname = रिक्त;
		अन्यथा
		    ccname = check_for_bom(ptr, size, &blen,
		      fio_flags == FIO_UCSBOM ? FIO_ALL : get_fio_flags(fenc));
		जर (ccname != रिक्त)
		{
		    // Remove BOM from the text
		    filesize += blen;
		    size -= blen;
		    mch_memmove(ptr, ptr + blen, (माप_प्रकार)size);
		    जर (set_options)
		    {
			curbuf->b_p_bomb = TRUE;
			curbuf->b_start_bomb = TRUE;
		    }
		}

		जर (fio_flags == FIO_UCSBOM)
		{
		    जर (ccname == रिक्त)
		    {
			// No BOM detected: retry with next encoding.
			advance_fenc = TRUE;
		    }
		    अन्यथा
		    {
			// BOM detected: set "fenc" and jump back
			जर (fenc_alloced)
			    vim_free(fenc);
			fenc = ccname;
			fenc_alloced = FALSE;
		    }
		    // retry reading without getting new bytes or rewinding
		    skip_read = TRUE;
		    जा retry;
		}
	    }

	    // Include not converted bytes.
	    ptr -= conv_restlen;
	    size += conv_restlen;
	    conv_restlen = 0;
	    /*
	     * Break here for a read error or end-of-file.
	     */
	    जर (size <= 0)
		थांबा;


#ifdef USE_ICONV
	    जर (iconv_fd != (iconv_t)-1)
	    {
		/*
		 * Attempt conversion of the read bytes to 'encoding' using
		 * iconv().
		 */
		स्थिरांक अक्षर	*fromp;
		अक्षर		*शीर्ष;
		माप_प्रकार		from_size;
		माप_प्रकार		to_size;

		fromp = (अक्षर *)ptr;
		from_size = size;
		ptr += size;
		शीर्ष = (अक्षर *)ptr;
		to_size = real_size - size;

		/*
		 * If there is conversion error or not enough room try using
		 * another conversion.  Except for when there is no
		 * alternative (help files).
		 */
		पर्यंत ((iconv(iconv_fd, (शून्य *)&fromp, &from_size,
							       &शीर्ष, &to_size)
			    == (माप_प्रकार)-1 && ICONV_ERRNO != ICONV_EINVAL)
						  || from_size > CONV_RESTLEN)
		{
		    जर (can_retry)
			जा rewind_retry;
		    जर (conv_error == 0)
			conv_error = readfile_linenr(linecnt,
							  ptr, (char_u *)शीर्ष);

		    // Deal with a bad byte and continue with the next.
		    ++fromp;
		    --from_size;
		    जर (bad_char_behavior == BAD_KEEP)
		    {
			*शीर्ष++ = *(fromp - 1);
			--to_size;
		    }
		    अन्यथा जर (bad_char_behavior != BAD_DROP)
		    {
			*शीर्ष++ = bad_char_behavior;
			--to_size;
		    }
		}

		जर (from_size > 0)
		{
		    // Some remaining characters, keep them for the next
		    // round.
		    mch_memmove(conv_rest, (char_u *)fromp, from_size);
		    conv_restlen = (पूर्णांक)from_size;
		}

		// move the linerest to before the converted characters
		ओळ_start = ptr - linerest;
		mch_memmove(ओळ_start, buffer, (माप_प्रकार)linerest);
		size = (दीर्घ)((char_u *)शीर्ष - ptr);
	    }
#endif

#ifdef MSWIN
	    जर (fio_flags & FIO_CODEPAGE)
	    {
		char_u	*src, *dst;
		WCHAR	ucs2buf[3];
		पूर्णांक	ucs2len;
		पूर्णांक	codepage = FIO_GET_CP(fio_flags);
		पूर्णांक	bytelen;
		पूर्णांक	found_bad;
		अक्षर	replstr[2];

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
		जर (bad_char_behavior > 0)
		    replstr[0] = bad_char_behavior;
		अन्यथा
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
		पर्यंत (size > 0)
		{
		    found_bad = FALSE;

# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
		    जर (codepage == CP_UTF8)
		    {
			// Handle CP_UTF8 input ourselves to be able to handle
			// trailing bytes properly.
			// Get one UTF-8 character from src.
			bytelen = (पूर्णांक)utf_ptr2len_len(src, size);
			जर (bytelen > size)
			{
			    // Only got some bytes of a character.  Normally
			    // it's put in "conv_rest", but if it's too long
			    // deal with it as if they were illegal bytes.
			    जर (bytelen <= CONV_RESTLEN)
				थांबा;

			    // weird overlong byte sequence
			    bytelen = size;
			    found_bad = TRUE;
			}
			अन्यथा
			{
			    पूर्णांक	    u8c = utf_ptr2char(src);

			    जर (u8c > 0xffff || (*src >= 0x80 && bytelen == 1))
				found_bad = TRUE;
			    ucs2buf[0] = u8c;
			    ucs2len = 1;
			}
		    }
		    अन्यथा
# endif
		    {
			// We don't know how long the byte sequence is, try
			// from one to three bytes.
			साठी (bytelen = 1; bytelen <= size && bytelen <= 3;
								    ++bytelen)
			{
			    ucs2len = MultiByteToWideChar(codepage,
							 MB_ERR_INVALID_CHARS,
							 (LPCSTR)src, bytelen,
								   ucs2buf, 3);
			    जर (ucs2len > 0)
				थांबा;
			}
			जर (ucs2len == 0)
			{
			    // If we have only one byte then it's probably an
			    // incomplete byte sequence.  Otherwise discard
			    // one byte as a bad character.
			    जर (size == 1)
				थांबा;
			    found_bad = TRUE;
			    bytelen = 1;
			}
		    }

		    जर (!found_bad)
		    {
			पूर्णांक	i;

			// Convert "ucs2buf[ucs2len]" to 'enc' in "dst".
			जर (enc_utf8)
			{
			    // From UCS-2 to UTF-8.  Cannot fail.
			    साठी (i = 0; i < ucs2len; ++i)
				dst += utf_char2bytes(ucs2buf[i], dst);
			}
			अन्यथा
			{
			    BOOL	bad = FALSE;
			    पूर्णांक		dstlen;

			    // From UCS-2 to "enc_codepage".  If the
			    // conversion uses the default character "?",
			    // the data doesn't fit in this encoding.
			    dstlen = WideCharToMultiByte(enc_codepage, 0,
				    (LPCWSTR)ucs2buf, ucs2len,
				    (LPSTR)dst, (पूर्णांक)(src - dst),
				    replstr, &bad);
			    जर (bad)
				found_bad = TRUE;
			    अन्यथा
				dst += dstlen;
			}
		    }

		    जर (found_bad)
		    {
			// Deal with bytes we can't convert.
			जर (can_retry)
			    जा rewind_retry;
			जर (conv_error == 0)
			    conv_error = readfile_linenr(linecnt, ptr, dst);
			जर (bad_char_behavior != BAD_DROP)
			{
			    जर (bad_char_behavior == BAD_KEEP)
			    {
				mch_memmove(dst, src, bytelen);
				dst += bytelen;
			    }
			    अन्यथा
				*dst++ = bad_char_behavior;
			}
		    }

		    src += bytelen;
		    size -= bytelen;
		}

		जर (size > 0)
		{
		    // An incomplete byte sequence remaining.
		    mch_memmove(conv_rest, src, size);
		    conv_restlen = size;
		}

		// The new size is equal to how much "dst" was advanced.
		size = (दीर्घ)(dst - ptr);
	    }
	    अन्यथा
#endif
#ifdef MACOS_CONVERT
	    जर (fio_flags & FIO_MACROMAN)
	    {
		/*
		 * Conversion from Apple MacRoman char encoding to UTF-8 or
		 * latin1.  This is in os_mac_conv.c.
		 */
		जर (macroman2enc(ptr, &size, real_size) == FAIL)
		    जा rewind_retry;
	    }
	    अन्यथा
#endif
	    जर (fio_flags != 0)
	    {
		पूर्णांक	u8c;
		char_u	*dest;
		char_u	*tail = रिक्त;

		/*
		 * "enc_utf8" set: Convert Unicode or Latin1 to UTF-8.
		 * "enc_utf8" not set: Convert Unicode to Latin1.
		 * Go from end to start through the buffer, because the number
		 * of bytes may increase.
		 * "dest" points to after where the UTF-8 bytes go, "p" points
		 * to after the next character to convert.
		 */
		dest = ptr + real_size;
		जर (fio_flags == FIO_LATIN1 || fio_flags == FIO_UTF8)
		{
		    p = ptr + size;
		    जर (fio_flags == FIO_UTF8)
		    {
			// Check for a trailing incomplete UTF-8 sequence
			tail = ptr + size - 1;
			पर्यंत (tail > ptr && (*tail & 0xc0) == 0x80)
			    --tail;
			जर (tail + utf_byte2len(*tail) <= ptr + size)
			    tail = रिक्त;
			अन्यथा
			    p = tail;
		    }
		}
		अन्यथा जर (fio_flags & (FIO_UCS2 | FIO_UTF16))
		{
		    // Check for a trailing byte
		    p = ptr + (size & ~1);
		    जर (size & 1)
			tail = p;
		    जर ((fio_flags & FIO_UTF16) && p > ptr)
		    {
			// Check for a trailing leading word
			जर (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			अन्यथा
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			जर (u8c >= 0xd800 && u8c <= 0xdbff)
			    tail = p;
			अन्यथा
			    p += 2;
		    }
		}
		अन्यथा //  FIO_UCS4
		{
		    // Check for trailing 1, 2 or 3 bytes
		    p = ptr + (size & ~3);
		    जर (size & 3)
			tail = p;
		}

		// If there is a trailing incomplete sequence move it to
		// conv_rest[].
		जर (tail != रिक्त)
		{
		    conv_restlen = (पूर्णांक)((ptr + size) - tail);
		    mch_memmove(conv_rest, (char_u *)tail, conv_restlen);
		    size -= conv_restlen;
		}


		पर्यंत (p > ptr)
		{
		    जर (fio_flags & FIO_LATIN1)
			u8c = *--p;
		    अन्यथा जर (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    {
			जर (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			अन्यथा
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			जर ((fio_flags & FIO_UTF16)
					    && u8c >= 0xdc00 && u8c <= 0xdfff)
			{
			    पूर्णांक u16c;

			    जर (p == ptr)
			    {
				// Missing leading word.
				जर (can_retry)
				    जा rewind_retry;
				जर (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				जर (bad_char_behavior == BAD_DROP)
				    सुरूठेवा;
				जर (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }

			    // found second word of double-word, get the first
			    // word and compute the resulting character
			    जर (fio_flags & FIO_ENDIAN_L)
			    {
				u16c = (*--p << 8);
				u16c += *--p;
			    }
			    अन्यथा
			    {
				u16c = *--p;
				u16c += (*--p << 8);
			    }
			    u8c = 0x10000 + ((u16c & 0x3ff) << 10)
							      + (u8c & 0x3ff);

			    // Check if the word is indeed a leading word.
			    जर (u16c < 0xd800 || u16c > 0xdbff)
			    {
				जर (can_retry)
				    जा rewind_retry;
				जर (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				जर (bad_char_behavior == BAD_DROP)
				    सुरूठेवा;
				जर (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    अन्यथा जर (fio_flags & FIO_UCS4)
		    {
			जर (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (अचिन्हित)*--p << 24;
			    u8c += (अचिन्हित)*--p << 16;
			    u8c += (अचिन्हित)*--p << 8;
			    u8c += *--p;
			}
			अन्यथा	// big endian
			{
			    u8c = *--p;
			    u8c += (अचिन्हित)*--p << 8;
			    u8c += (अचिन्हित)*--p << 16;
			    u8c += (अचिन्हित)*--p << 24;
			}
		    }
		    अन्यथा    // UTF-8
		    {
			जर (*--p < 0x80)
			    u8c = *p;
			अन्यथा
			{
			    len = utf_head_off(ptr, p);
			    p -= len;
			    u8c = utf_ptr2char(p);
			    जर (len == 0)
			    {
				// Not a valid UTF-8 character, retry with
				// another fenc when possible, otherwise just
				// report the error.
				जर (can_retry)
				    जा rewind_retry;
				जर (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				जर (bad_char_behavior == BAD_DROP)
				    सुरूठेवा;
				जर (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    जर (enc_utf8)	// produce UTF-8
		    {
			dest -= utf_char2len(u8c);
			(शून्य)utf_char2bytes(u8c, dest);
		    }
		    अन्यथा		// produce Latin1
		    {
			--dest;
			जर (u8c >= 0x100)
			{
			    // character doesn't fit in latin1, retry with
			    // another fenc when possible, otherwise just
			    // report the error.
			    जर (can_retry)
				जा rewind_retry;
			    जर (conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
			    जर (bad_char_behavior == BAD_DROP)
				++dest;
			    अन्यथा जर (bad_char_behavior == BAD_KEEP)
				*dest = u8c;
			    अन्यथा जर (eap != रिक्त && eap->bad_char != 0)
				*dest = bad_char_behavior;
			    अन्यथा
				*dest = 0xBF;
			}
			अन्यथा
			    *dest = u8c;
		    }
		}

		// move the linerest to before the converted characters
		ओळ_start = dest - linerest;
		mch_memmove(ओळ_start, buffer, (माप_प्रकार)linerest);
		size = (दीर्घ)((ptr + real_size) - dest);
		ptr = dest;
	    }
	    अन्यथा जर (enc_utf8 && !curbuf->b_p_bin)
	    {
		पूर्णांक  incomplete_tail = FALSE;

		// Reading UTF-8: Check if the bytes are valid UTF-8.
		साठी (p = ptr; ; )
		{
		    पूर्णांक	 todo;
		    पूर्णांक	 l;

		    // Skip ASCII bytes quickly using word-at-a-time check.
		    {
			char_u *ascii_end = ptr + size;
			पर्यंत (ascii_end - p >= (दीर्घ)आकार(long_u))
			{
			    long_u word;
			    स_नकल(&word, p, आकार(long_u));
			    जर (word & NONASCII_MASK)
				थांबा;
			    p += आकार(long_u);
			}
			पर्यंत (p < ascii_end && *p < 0x80)
			    ++p;
		    }

		    todo = (पूर्णांक)((ptr + size) - p);
		    जर (todo <= 0)
			थांबा;
		    जर (*p >= 0x80)
		    {
			// A length of 1 means it's an illegal byte.  Accept
			// an incomplete character at the end though, the next
			// read() will get the next bytes, we'll check it
			// then.
			l = utf_ptr2len_len(p, todo);
			जर (l > todo && !incomplete_tail)
			{
			    // Avoid retrying with a different encoding when
			    // a truncated file is more likely, or attempting
			    // to read the rest of an incomplete sequence when
			    // we have already done so.
			    जर (p > ptr || filesize > 0)
				incomplete_tail = TRUE;
			    // Incomplete byte sequence, move it to conv_rest[]
			    // and try to read the rest of it, unless we've
			    // already done so.
			    जर (p > ptr)
			    {
				conv_restlen = todo;
				mch_memmove(conv_rest, p, conv_restlen);
				size -= conv_restlen;
				थांबा;
			    }
			}
			जर (l == 1 || l > todo)
			{
			    // Illegal byte.  If we can try another encoding
			    // do that, unless at EOF where a truncated
			    // file is more likely than a conversion error.
			    जर (can_retry && !incomplete_tail)
				थांबा;
#ifdef USE_ICONV
			    // When we did a conversion report an error.
			    जर (iconv_fd != (iconv_t)-1 && conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
#endif
			    // Remember the first linenr with an illegal byte
			    जर (conv_error == 0 && illegal_byte == 0)
				illegal_byte = readfile_linenr(linecnt, ptr, p);

			    // Drop, keep or replace the bad byte.
			    जर (bad_char_behavior == BAD_DROP)
			    {
				mch_memmove(p, p + 1, todo - 1);
				--size;
			    }
			    अन्यथा
			    {
				जर (bad_char_behavior != BAD_KEEP)
				    *p = bad_char_behavior;
				++p;
			    }
			}
			अन्यथा
			    p += l;
		    }
		}
		जर (p < ptr + size && !incomplete_tail)
		{
		    // Detected a UTF-8 error.
rewind_retry:
		    // Retry reading with another conversion.
#जर defined(FEAT_EVAL) && defined(USE_ICONV)
		    जर (*p_ccv != NUL && iconv_fd != (iconv_t)-1)
			// iconv() failed, try 'charconvert'
			did_iconv = TRUE;
		    अन्यथा
#endif
			// use next item from 'fileencodings'
			advance_fenc = TRUE;
		    file_rewind = TRUE;
		    जा retry;
		}
	    }

	    // count the number of characters (after conversion!)
	    filesize += size;

	    /*
	     * when reading the first part of a file: guess EOL type
	     */
	    जर (fileformat == EOL_UNKNOWN)
	    {
		// First try finding a NL, for Dos and Unix
		जर (try_dos || try_unix)
		{
		    // Reset the carriage return counter.
		    जर (try_mac)
			try_mac = 1;

		    साठी (p = ptr; p < ptr + size; ++p)
		    {
			जर (*p == NL)
			{
			    जर (!try_unix
				    || (try_dos && p > ptr && p[-1] == CAR))
				fileformat = EOL_DOS;
			    अन्यथा
				fileformat = EOL_UNIX;
			    थांबा;
			}
			अन्यथा जर (*p == CAR && try_mac)
			    try_mac++;
		    }

		    // Don't give in to EOL_UNIX if EOL_MAC is more likely
		    जर (fileformat == EOL_UNIX && try_mac)
		    {
			// Need to reset the counters when retrying fenc.
			try_mac = 1;
			try_unix = 1;
			साठी (; p >= ptr && *p != CAR; p--)
			    ;
			जर (p >= ptr)
			{
			    साठी (p = ptr; p < ptr + size; ++p)
			    {
				जर (*p == NL)
				    try_unix++;
				अन्यथा जर (*p == CAR)
				    try_mac++;
			    }
			    जर (try_mac > try_unix)
				fileformat = EOL_MAC;
			}
		    }
		    अन्यथा जर (fileformat == EOL_UNKNOWN && try_mac == 1)
			// Looking for CR but found no end-of-line markers at
			// all: use the default format.
			fileformat = default_fileformat();
		}

		// No NL found: may use Mac format
		जर (fileformat == EOL_UNKNOWN && try_mac)
		    fileformat = EOL_MAC;

		// Still nothing found?  Use first format in 'ffs'
		जर (fileformat == EOL_UNKNOWN)
		    fileformat = default_fileformat();

		// if editing a new file: may set p_tx and p_ff
		जर (set_options)
		    set_fileformat(fileformat, OPT_LOCAL);
	    }
	}

	/*
	 * This loop is executed once for every character read.
	 * Keep it fast!
	 */
	जर (fileformat == EOL_MAC)
	{
	    --ptr;
	    पर्यंत (++ptr, --size >= 0)
	    {
		// catch most common case first
		जर ((c = *ptr) != NUL && c != CAR && c != NL)
		    सुरूठेवा;
		जर (c == NUL)
		    *ptr = NL;	// NULs are replaced by newlines!
		अन्यथा जर (c == NL)
		    *ptr = CAR;	// NLs are replaced by CRs!
		अन्यथा
		{
		    जर (skip_मोजणी == 0)
		    {
			*ptr = NUL;	    // end of line
			len = (colnr_T) (ptr - ओळ_start + 1);
			जर (ml_append(lnum, ओळ_start, len, newfile) == FAIL)
			{
			    error = TRUE;
			    थांबा;
			}
#ifdef FEAT_PERSISTENT_UNDO
			जर (read_undo_file)
			    sha256_update(&sha_ctx, ओळ_start, len);
#endif
			++lnum;
			जर (--read_मोजणी == 0)
			{
			    error = TRUE;	// break loop
			    ओळ_start = ptr;	// nothing left to write
			    थांबा;
			}
		    }
		    अन्यथा
			--skip_मोजणी;
		    ओळ_start = ptr + 1;
		}
	    }
	}
	अन्यथा
	{
	    // Use memchr() for SIMD-optimized newline scanning instead
	    // of scanning each byte individually.
	    char_u *end = ptr + size;

	    पर्यंत (ptr < end)
	    {
		char_u *nl = (char_u *)स_प्रथम(ptr, NL, end - ptr);
		char_u *nul_scan;

		जर (nl == रिक्त)
		{
		    // No more newlines in buffer.
		    // Replace any NUL bytes with NL in remaining data.
		    पर्यंत ((nul_scan = (char_u *)स_प्रथम(ptr, NUL,
						      end - ptr)) != रिक्त)
		    {
			*nul_scan = NL;
			ptr = nul_scan + 1;
		    }
		    ptr = end;
		    थांबा;
		}

		// Replace NUL bytes with NL before the newline.
		{
		    char_u *scan = ptr;
		    पर्यंत ((nul_scan = (char_u *)स_प्रथम(scan, NUL,
						       nl - scan)) != रिक्त)
		    {
			*nul_scan = NL;
			scan = nul_scan + 1;
		    }
		}

		// Process the newline.
		ptr = nl;
		जर (skip_मोजणी == 0)
		{
		    *ptr = NUL;		// end of line
		    len = (colnr_T)(ptr - ओळ_start + 1);
		    जर (fileformat == EOL_DOS)
		    {
			जर (ptr > ओळ_start && ptr[-1] == CAR)
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
			अन्यथा जर (ff_error != EOL_DOS)
			{
			    जर (   try_unix
				&& !read_stdin
				&& (read_buffer
				    || vim_lseek(fd, (off_T)0L, शुरू_से)
								      == 0))
			    {
				fileformat = EOL_UNIX;
				जर (set_options)
				    set_fileformat(EOL_UNIX, OPT_LOCAL);
				file_rewind = TRUE;
				keep_fileformat = TRUE;
				जा retry;
			    }
			    ff_error = EOL_DOS;
			}
		    }
		    जर (ml_append(lnum, ओळ_start, len, newfile) == FAIL)
		    {
			error = TRUE;
			थांबा;
		    }
#ifdef FEAT_PERSISTENT_UNDO
		    जर (read_undo_file)
			sha256_update(&sha_ctx, ओळ_start, len);
#endif
		    ++lnum;
		    जर (--read_मोजणी == 0)
		    {
			error = TRUE;	    // break loop
			ओळ_start = ptr;   // nothing left to write
			थांबा;
		    }
		}
		अन्यथा
		    --skip_मोजणी;
		ओळ_start = ptr + 1;
		++ptr;
	    }
	    size = -1;
	}
	linerest = (दीर्घ)(ptr - ओळ_start);
	ui_breakcheck();
    }

failed:
    // not an error, max. number of lines reached
    जर (error && read_मोजणी == 0)
	error = FALSE;

    // In Dos format ignore a trailing CTRL-Z, unless 'binary' is set.
    // In old days the file length was in sector count and the CTRL-Z the
    // marker where the file really ended.  Assuming we write it to a file
    // system that keeps file length properly the CTRL-Z should be dropped.
    // Set the 'endoffile' option so the user can decide what to write later.
    // In Unix format the CTRL-Z is just another character.
    जर (linerest != 0
	    && !curbuf->b_p_bin
	    && fileformat == EOL_DOS
	    && ptr[-1] == Ctrl_Z)
    {
	ptr--;
	linerest--;
	जर (set_options)
	    curbuf->b_p_eof = TRUE;
    }

    // If we get EOF in the middle of a line, note the fact by resetting
    // 'endofline' and add the line normally.
    जर (!error
	    && !got_int
	    && linerest != 0)
    {
	// remember for when writing
	जर (set_options)
	    curbuf->b_p_eol = FALSE;
	*ptr = NUL;
	len = (colnr_T)(ptr - ओळ_start + 1);
	जर (ml_append(lnum, ओळ_start, len, newfile) == FAIL)
	    error = TRUE;
	अन्यथा
	{
#ifdef FEAT_PERSISTENT_UNDO
	    जर (read_undo_file)
		sha256_update(&sha_ctx, ओळ_start, len);
#endif
	    read_no_eol_lnum = ++lnum;
	}
    }

    जर (set_options)
	save_file_ff(curbuf);		// remember the current file format

#ifdef FEAT_CRYPT
    जर (curbuf->b_cryptstate != रिक्त)
    {
	crypt_free_state(curbuf->b_cryptstate);
	curbuf->b_cryptstate = रिक्त;
    }
    जर (cryptkey != रिक्त && cryptkey != curbuf->b_p_किल्ली)
	crypt_free_किल्ली(cryptkey);
    // Don't set cryptkey to NULL, it's used below as a flag that
    // encryption was used.
#endif

    // If editing a new file: set 'fenc' for the current buffer.
    // Also for ":read ++edit file".
    जर (set_options)
	set_string_option_direct((char_u *)"fenc", -1, fenc,
						       OPT_FREE|OPT_LOCAL, 0);
    जर (fenc_alloced)
	vim_free(fenc);
#ifdef USE_ICONV
    जर (iconv_fd != (iconv_t)-1)
	iconv_close(iconv_fd);
#endif

    जर (!read_buffer && !read_stdin)
	बंद(fd);				// errors are ignored
#ifdef HAVE_FD_CLOEXEC
    अन्यथा
    {
	पूर्णांक fdflags = fcntl(fd, F_GETFD);

	जर (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (शून्य)fcntl(fd, F_SETFD, fdflags | FD_CLOEXEC);
    }
#endif
    vim_free(buffer);

#ifdef HAVE_DUP
    जर (read_stdin)
    {
	// Use stderr for stdin, makes shell commands work.
	बंद(0);
	vim_ignored = dup(2);
    }
#endif

    जर (tmpname != रिक्त)
    {
	mch_remove(tmpname);		// delete converted file
	vim_free(tmpname);
    }
    --no_wait_return;			// may wait for return now

    /*
     * In recovery mode everything but autocommands is skipped.
     */
    जर (!recoverymode)
    {
	// need to delete the last line, which comes from the empty buffer
	जर (newfile && wasempty && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	{
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 0;
#endif
	    ml_delete(curbuf->b_ml.ml_ओळ_मोजणी);
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 1;
#endif
	    --linecnt;
	}
	linecnt = curbuf->b_ml.ml_ओळ_मोजणी - linecnt;
	जर (filesize == 0)
	    linecnt = 0;
	जर (newfile || read_buffer)
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
	अन्यथा जर (linecnt)		// appended at least one line
	    appended_ओळी_mark(from, linecnt);

#ifndef ALWAYS_USE_GUI
	/*
	 * If we were reading from the same terminal as where messages go,
	 * the screen will have been messed up.
	 * Switch on raw mode now and clear the screen.
	 */
	जर (read_stdin)
	{
	    settmode(TMODE_RAW);	// set to raw mode
	    starttermcap();
	    screenclear();
	}
#endif

	जर (got_int)
	{
	    जर (!(flags & READ_DUMMY))
	    {
		filemess(curbuf, sfname, (char_u *)_(e_interrupted), 0);
		जर (newfile)
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    }
	    संदेश_scroll = संदेश_save;
#ifdef FEAT_VIMINFO
	    check_marks_read();
#endif
	    retval = OK;	// an interrupt isn't really an error
	    जा theend;
	}

	जर (!filtering && !(flags & READ_DUMMY))
	{
	    पूर्णांक	buflen;

	    संदेश_add_fname(curbuf, sfname);   // fname in IObuff with quotes
	    c = FALSE;

	    buflen = (पूर्णांक)STRLEN(IObuff);
#ifdef UNIX
	    जर (S_ISFIFO(perm))			    // fifo
	    {
		buflen += vim_snprintf((अक्षर *)IObuff + buflen, IOSIZE - buflen,
			_("[fifo]"));
		c = TRUE;
	    }
	    जर (S_ISSOCK(perm))			    // or socket
	    {
		buflen += vim_snprintf((अक्षर *)IObuff + buflen, IOSIZE - buflen,
			_("[socket]"));
		c = TRUE;
	    }
# ifdef OPEN_CHR_FILES
	    जर (S_ISCHR(perm))			    // or character special
	    {
		buflen += vim_snprintf((अक्षर *)IObuff + buflen, IOSIZE - buflen,
			_("[character special]"));
		c = TRUE;
	    }
# endif
#endif
	    जर (curbuf->b_p_ro)
	    {
		buflen += vim_snprintf((अक्षर *)IObuff + buflen, IOSIZE - buflen,
			"%s", shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
		c = TRUE;
	    }
	    जर (read_no_eol_lnum)
	    {
		संदेश_add_eol();
		c = TRUE;
	    }
	    जर (ff_error == EOL_DOS)
	    {
		buflen += vim_snprintf((अक्षर *)IObuff + buflen, IOSIZE - buflen,
			_("[CR missing]"));
		c = TRUE;
	    }
	    जर (split)
	    {
		buflen += vim_snprintf((अक्षर *)IObuff + buflen, IOSIZE - buflen,
			_("[long lines split]"));
		c = TRUE;
	    }
	    जर (notconverted)
	    {
		buflen += vim_snprintf((अक्षर *)IObuff + buflen, IOSIZE - buflen,
			_("[NOT converted]"));
		c = TRUE;
	    }
	    अन्यथा जर (converted)
	    {
		buflen += vim_snprintf((अक्षर *)IObuff + buflen, IOSIZE - buflen,
			_("[converted]"));
		c = TRUE;
	    }
#ifdef FEAT_CRYPT
	    जर (cryptkey != रिक्त)
	    {
		crypt_append_संदेश(curbuf);
		c = TRUE;
	    }
#endif
	    जर (conv_error != 0)
	    {
		vim_snprintf((अक्षर *)IObuff + buflen, IOSIZE - buflen,
			_("[CONVERSION ERROR in line %ld]"), (दीर्घ)conv_error);
		c = TRUE;
	    }
	    अन्यथा जर (illegal_byte > 0)
	    {
		vim_snprintf((अक्षर *)IObuff + buflen, IOSIZE - buflen,
			_("[ILLEGAL BYTE in line %ld]"), (दीर्घ)illegal_byte);
		c = TRUE;
	    }
	    अन्यथा जर (error)
	    {
		vim_snprintf((अक्षर *)IObuff + buflen, IOSIZE - buflen,
			_("[READ ERRORS]"));
		c = TRUE;
	    }
	    जर (संदेश_add_fileformat(fileformat))
		c = TRUE;
#ifdef FEAT_CRYPT
	    जर (cryptkey != रिक्त)
		संदेश_add_ओळी(c, (दीर्घ)linecnt, filesize
			 - crypt_get_header_len(crypt_get_method_nr(curbuf)));
	    अन्यथा
#endif
		संदेश_add_ओळी(c, (दीर्घ)linecnt, filesize);

	    VIM_CLEAR(keep_संदेश);
	    संदेश_scrolled_ign = TRUE;
#ifdef ALWAYS_USE_GUI
	    // Don't show the message when reading stdin, it would end up in a
	    // message box (which might be shown when exiting!)
	    जर (read_stdin || read_buffer)
		p = संदेश_may_trunc(FALSE, IObuff);
	    अन्यथा
#endif
	    {
		जर (संदेश_col > 0)
		    संदेश_putchar('\r');  // overwrite previous message
		p = (char_u *)संदेश_trunc_attr((अक्षर *)IObuff, FALSE, 0);
	    }
	    जर (read_stdin || read_buffer || restart_edit != 0
		    || (संदेश_scrolled != 0 && !need_wait_return))
		// Need to repeat the message after redrawing when:
		// - When reading from stdin (the screen will be cleared next).
		// - When restart_edit is set (otherwise there will be a delay
		//   before redrawing).
		// - When the screen was scrolled but there is no wait-return
		//   prompt.
		set_keep_संदेश(p, 0);
	    संदेश_scrolled_ign = FALSE;
	}

	// with errors writing the file requires ":w!"
	जर (newfile && (error
		    || conv_error != 0
		    || (illegal_byte > 0 && bad_char_behavior != BAD_KEEP)))
	    curbuf->b_p_ro = TRUE;

	u_clearline();	    // cannot use "U" command after adding lines

	/*
	 * In Ex mode: cursor at last new line.
	 * Otherwise: cursor at first new line.
	 */
	जर (exmode_active)
	    curwin->w_cursor.lnum = from + linecnt;
	अन्यथा
	    curwin->w_cursor.lnum = from + 1;
	check_cursor_lnum();
	beginline(BL_WHITE | BL_FIX);	    // on first non-blank

	जर ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
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
	जर (newfile && !read_stdin && !read_buffer
					 && mch_stat((अक्षर *)fname, &st) >= 0)
	{
	    buf_store_time(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
	}
#endif
    }
    संदेश_scroll = संदेश_save;

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
    जर (flags & READ_KEEP_UNDO)
	u_find_first_changed();

#ifdef FEAT_PERSISTENT_UNDO
    /*
     * When opening a new file locate undo info and read it.
     */
    जर (read_undo_file)
    {
	char_u	hash[UNDO_HASH_SIZE];

	sha256_finish(&sha_ctx, hash);
	u_read_undo(रिक्त, hash, fname);
    }
#endif

    जर (!read_stdin && !read_fifo && (!read_buffer || sfname != रिक्त))
    {
	पूर्णांक m = संदेश_scroll;
	पूर्णांक n = संदेश_scrolled;

	// Save the fileformat now, otherwise the buffer will be considered
	// modified if the format/encoding was automatically detected.
	जर (set_options)
	    save_file_ff(curbuf);

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	संदेश_scroll = TRUE;
	जर (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPOST, रिक्त, sfname,
							  FALSE, curbuf, eap);
	अन्यथा जर (newfile || (read_buffer && sfname != रिक्त))
	{
	    apply_autocmds_exarg(EVENT_BUFREADPOST, रिक्त, sfname,
							  FALSE, curbuf, eap);
	    जर (!curbuf->b_au_did_filetype && *curbuf->b_p_ft != NUL)
		/*
		 * EVENT_FILETYPE was not triggered but the buffer already has a
		 * filetype. Trigger EVENT_FILETYPE using the existing filetype.
		 */
		apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
			TRUE, curbuf);
	}
	अन्यथा
	    apply_autocmds_exarg(EVENT_FILEREADPOST, sfname, sfname,
							    FALSE, रिक्त, eap);
	जर (संदेश_scrolled == n)
	    संदेश_scroll = m;
#ifdef FEAT_EVAL
	जर (aborting())	    // autocmds may abort script processing
	    जा theend;
#endif
    }

    जर (!(recoverymode && error))
	retval = OK;

theend:
    जर (curbuf->b_ml.ml_mfp != रिक्त
		       && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	// OK to sync the swap file now
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    परत retval;
}

#जर defined(OPEN_CHR_FILES)
/*
 * Returns TRUE if the file name argument is of the form "/dev/fd/\d\+",
 * which is the name of files used for process substitution output by
 * some shells on some operating systems, e.g., bash on SunOS.
 * Do not accept "/dev/fd/[012]", opening these may hang Vim.
 */
    पूर्णांक
is_dev_fd_file(char_u *fname)
{
    परत (STRNCMP(fname, "/dev/fd/", 8) == 0
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
    स्थिर linenr_T
readfile_linenr(
    linenr_T	linecnt,	// line count before reading more bytes
    char_u	*p,		// start of more bytes read
    char_u	*endp)		// end of more bytes read
{
    char_u	*s;
    linenr_T	lnum;

    lnum = curbuf->b_ml.ml_ओळ_मोजणी - linecnt + 1;
    साठी (s = p; s < endp; ++s)
	जर (*s == '\n')
	    ++lnum;
    परत lnum;
}

/*
 * Fill "*eap" to force the 'fileencoding', 'fileformat' and 'binary' to be
 * equal to the buffer "buf".  Used for calling readfile().
 * Returns OK or FAIL.
 */
    पूर्णांक
prep_exarg(exarg_T *eap, buf_T *buf)
{
    eap->cmd = alloc(15 + (अचिन्हित)STRLEN(buf->b_p_fenc));
    जर (eap->cmd == रिक्त)
	परत FAIL;

    प्र_लिखो((अक्षर *)eap->cmd, "e ++enc=%s", buf->b_p_fenc);
    eap->force_enc = 8;
    eap->bad_char = buf->b_bad_char;
    eap->force_ff = *buf->b_p_ff;

    eap->force_bin = buf->b_p_bin ? FORCE_BIN : FORCE_NOBIN;
    eap->read_edit = FALSE;
    eap->forceit = FALSE;
    परत OK;
}

/*
 * Set default or forced 'fileformat' and 'binary'.
 */
    शून्य
set_file_options(पूर्णांक set_options, exarg_T *eap)
{
    // set default 'fileformat'
    जर (set_options)
    {
	जर (eap != रिक्त && eap->force_ff != 0)
	    set_fileformat(get_fileformat_force(curbuf, eap), OPT_LOCAL);
	अन्यथा जर (*p_ffs != NUL)
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    // set or reset 'binary'
    जर (eap != रिक्त && eap->force_bin != 0)
    {
	पूर्णांक	oldval = curbuf->b_p_bin;

	curbuf->b_p_bin = (eap->force_bin == FORCE_BIN);
	set_options_bin(oldval, curbuf->b_p_bin, OPT_LOCAL);
    }
}

/*
 * Set forced 'fileencoding'.
 */
    शून्य
set_forced_fenc(exarg_T *eap)
{
    जर (eap->force_enc == 0)
	परत;

    char_u *fenc = enc_canonize(eap->cmd + eap->force_enc);

    जर (fenc != रिक्त)
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
    स्थिर char_u *
next_fenc(char_u **pp, पूर्णांक *alloced)
{
    char_u	*p;
    char_u	*r;

    *alloced = FALSE;
    जर (**pp == NUL)
    {
	*pp = रिक्त;
	परत (char_u *)"";
    }
    p = vim_strchr(*pp, ',');
    जर (p == रिक्त)
    {
	r = enc_canonize(*pp);
	*pp += STRLEN(*pp);
    }
    अन्यथा
    {
	r = vim_strnsave(*pp, p - *pp);
	*pp = p + 1;
	जर (r != रिक्त)
	{
	    p = enc_canonize(r);
	    vim_free(r);
	    r = p;
	}
    }
    जर (r != रिक्त)
	*alloced = TRUE;
    अन्यथा
    {
	// out of memory
	r = (char_u *)"";
	*pp = रिक्त;
    }
    परत r;
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
    स्थिर char_u *
readfile_charconvert(
    char_u	*fname,		// name of input file
    char_u	*fenc,		// converted from
    पूर्णांक		*fdp)		// in/out: file descriptor of file
{
    char_u	*tmpname;
    अक्षर	*errmsg = रिक्त;

    tmpname = vim_tempname('r', FALSE);
    जर (tmpname == रिक्त)
	errmsg = _("Can't find temp file for conversion");
    अन्यथा
    {
	बंद(*fdp);		// close the input file, ignore errors
	*fdp = -1;
	जर (eval_charconvert(fenc, enc_utf8 ? (char_u *)"utf-8" : p_enc,
						      fname, tmpname) == FAIL)
	    errmsg = _("Conversion with 'charconvert' failed");
	जर (errmsg == रिक्त && (*fdp = mch_open((अक्षर *)tmpname,
						  O_RDONLY | O_EXTRA, 0)) < 0)
	    errmsg = _("can't read output of 'charconvert'");
    }

    जर (errmsg != रिक्त)
    {
	// Don't use emsg(), it breaks mappings, the retry with
	// another type of conversion might still work.
	संदेश(errmsg);
	जर (tmpname != रिक्त)
	{
	    mch_remove(tmpname);	// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    // If the input file is closed, open it (caller should check for error).
    जर (*fdp < 0)
	*fdp = mch_open((अक्षर *)fname, O_RDONLY | O_EXTRA, 0);

    परत tmpname;
}
#endif

#जर defined(FEAT_CRYPT)
/*
 * Check for magic number used for encryption.  Applies to the current buffer.
 * If found, the magic number is removed from ptr[*sizep] and *sizep and
 * *filesizep are updated.
 * Return the (new) encryption key, NULL for no encryption.
 */
    स्थिर char_u *
check_for_cryptkey(
    char_u	*cryptkey,	// previous encryption key or NULL
    char_u	*ptr,		// pointer to read bytes
    दीर्घ	*sizep,		// length of read bytes
    off_T	*filesizep,	// nr of bytes used from file
    पूर्णांक		newfile,	// editing a new buffer
    char_u	*fname,		// file name to display
    पूर्णांक		*did_ask)	// flag: whether already asked for key
{
    पूर्णांक method = crypt_method_nr_from_magic((अक्षर *)ptr, *sizep);
    पूर्णांक b_p_ro = curbuf->b_p_ro;

    जर (method >= 0)
    {
	// Mark the buffer as read-only until the decryption has taken place.
	// Avoids accidentally overwriting the file with garbage.
	curbuf->b_p_ro = TRUE;

	// Set the cryptmethod local to the buffer.
	crypt_set_cm_option(curbuf, method);
	जर (cryptkey == रिक्त && !*did_ask)
	{
	    जर (*curbuf->b_p_किल्ली)
	    {
		cryptkey = curbuf->b_p_किल्ली;
		crypt_check_swapfile_curbuf();
	    }
	    अन्यथा
	    {
		// When newfile is TRUE, store the typed key in the 'key'
		// option and don't free it.  bf needs hash of the key saved.
		// Don't ask for the key again when first time Enter was hit.
		// Happens when retrying to detect encoding.
		smsg(_(need_किल्ली_संदेश), fname);
		संदेश_scroll = TRUE;
		crypt_check_method(method);
		cryptkey = crypt_get_किल्ली(newfile, FALSE);
		*did_ask = TRUE;

		// check if empty key entered
		जर (cryptkey != रिक्त && *cryptkey == NUL)
		{
		    जर (cryptkey != curbuf->b_p_किल्ली)
			vim_free(cryptkey);
		    cryptkey = रिक्त;
		}
	    }
	}

	जर (cryptkey != रिक्त)
	{
	    पूर्णांक header_len;

	    header_len = crypt_get_header_len(method);
	    जर (*sizep < header_len)
	    {
		// invalid header, buffer can't be encrypted
		जर (cryptkey != curbuf->b_p_किल्ली)
		    vim_free(cryptkey);
		परत रिक्त;
	    }

	    curbuf->b_cryptstate = crypt_create_from_header(
							method, cryptkey, ptr);
	    crypt_set_cm_option(curbuf, method);

	    // Remove cryptmethod specific header from the text.
	    *filesizep += header_len;
	    *sizep -= header_len;
	    mch_memmove(ptr, ptr + header_len, (माप_प्रकार)*sizep);

	    // Restore the read-only flag.
	    curbuf->b_p_ro = b_p_ro;
	}
    }
    // When starting to edit a new file which does not have encryption, clear
    // the 'key' option, except when starting up (called with -x argument)
    अन्यथा जर (newfile && *curbuf->b_p_किल्ली != NUL && !starting)
	set_option_मूल्य_give_err((char_u *)"key", 0L, (char_u *)"", OPT_LOCAL);

    परत cryptkey;
}
#endif  // FEAT_CRYPT

/*
 * Return TRUE if a file appears to be read-only from the file permissions.
 */
    पूर्णांक
check_file_readonly(
    char_u	*fname,		// full path to file
    पूर्णांक		perm UNUSED)	// known permissions on file
{
#ifndef USE_MCH_ACCESS
    पूर्णांक	    fd = 0;
#endif

    परत (
#ifdef USE_MCH_ACCESS
# ifdef UNIX
	(perm & 0222) == 0 ||
# endif
	mch_access((अक्षर *)fname, W_OK)
#अन्यथा
	(fd = mch_open((अक्षर *)fname, O_RDWR | O_EXTRA, 0)) < 0
					? TRUE : (बंद(fd), FALSE)
#endif
	);
}

#जर defined(HAVE_FSYNC)
/*
 * Call fsync() with Mac-specific exception.
 * Return fsync() result: zero for success.
 */
    पूर्णांक
vim_fsync(पूर्णांक fd)
{
    पूर्णांक r;

# ifdef MACOS_X
    r = fcntl(fd, F_FULLFSYNC);
    जर (r != 0)  // F_FULLFSYNC not working or not supported
# endif
	r = fsync(fd);
    परत r;
}
#endif

/*
 * Set the name of the current buffer.  Use when the buffer doesn't have a
 * name and a ":r" or ":w" command with a file name is used.
 */
    पूर्णांक
set_rw_fname(char_u *fname, char_u *sfname)
{
    buf_T	*buf = curbuf;

    // It's like the unnamed buffer is deleted....
    जर (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFDELETE, रिक्त, रिक्त, FALSE, curbuf);
    apply_autocmds(EVENT_BUFWIPEOUT, रिक्त, रिक्त, FALSE, curbuf);
#ifdef FEAT_EVAL
    जर (aborting())	    // autocmds may abort script processing
	परत FAIL;
#endif
    जर (curbuf != buf)
    {
	// We are in another buffer now, don't do the renaming.
	emsg(_(e_autocommands_changed_buffer_or_buffer_name));
	परत FAIL;
    }

    जर (setfname(curbuf, fname, sfname, FALSE) == OK)
	curbuf->b_flags |= BF_NOTEDITED;

    // ....and a new named one is created
    apply_autocmds(EVENT_BUFNEW, रिक्त, रिक्त, FALSE, curbuf);
    जर (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFADD, रिक्त, रिक्त, FALSE, curbuf);
#ifdef FEAT_EVAL
    जर (aborting())	    // autocmds may abort script processing
	परत FAIL;
#endif

    // Do filetype detection now if 'filetype' is empty.
    जर (*curbuf->b_p_ft == NUL)
    {
	जर (au_has_group((char_u *)"filetypedetect"))
	    (शून्य)do_doautocmd((char_u *)"filetypedetect BufRead", FALSE, रिक्त);
	do_modelines(0);
    }

    परत OK;
}

/*
 * Put file name into IObuff with quotes.
 */
    शून्य
संदेश_add_fname(buf_T *buf, char_u *fname)
{
    माप_प्रकार  IObufflen = 0;

    जर (fname == रिक्त)
	fname = (char_u *)"-stdin-";
    IObuff[IObufflen++] = '"';
    IObufflen += home_replace(buf, fname, IObuff + IObufflen, IOSIZE - 4, TRUE);
    STRCPY(IObuff + IObufflen, "\" ");
}

/*
 * Append message for text mode to IObuff.
 * Return TRUE if something appended.
 */
    पूर्णांक
संदेश_add_fileformat(पूर्णांक eol_type)
{
#ifndef USE_CRNL
    जर (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[dos]") : _("[dos format]"));
	परत TRUE;
    }
#endif
    जर (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[mac]") : _("[mac format]"));
	परत TRUE;
    }
#ifdef USE_CRNL
    जर (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[unix]") : _("[unix format]"));
	परत TRUE;
    }
#endif
    परत FALSE;
}

/*
 * Append line and character count to IObuff.
 */
    शून्य
संदेश_add_ओळी(
    पूर्णांक	    घाला_space,
    दीर्घ    lnum,
    off_T   nchars)
{
    पूर्णांक  len = (पूर्णांक)STRLEN(IObuff);

    जर (shortmess(SHM_ओळी))
	vim_snprintf((अक्षर *)IObuff + len, IOSIZE - (माप_प्रकार)len,
		// l10n: L as in line, B as in byte
		_("%s%ldL, %lldB"), घाला_space ? " " : "", lnum, (varnumber_T)nchars);
    अन्यथा
    {
	len += vim_snprintf((अक्षर *)IObuff + len, IOSIZE - (माप_प्रकार)len,
		NGETTEXT("%s%ld line, ", "%s%ld lines, ", lnum), घाला_space ? " " : "", lnum);
	vim_snprintf((अक्षर *)IObuff + len, IOSIZE - (माप_प्रकार)len,
		NGETTEXT("%lld byte", "%lld bytes", nchars), (varnumber_T)nchars);
    }
}

/*
 * Append message for missing line separator to IObuff.
 */
    शून्य
संदेश_add_eol(शून्य)
{
    STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]") : _("[Incomplete last line]"));
}

    पूर्णांक
time_differs(stat_T *st, दीर्घ mtime, दीर्घ mtime_ns UNUSED)
{
    परत
#ifdef ST_MTIM_NSEC
	(दीर्घ)st->ST_MTIM_NSEC != mtime_ns ||
#endif
#जर defined(__linux__) || defined(MSWIN)
	// On a FAT filesystem, esp. under Linux, there are only 5 bits to store
	// the seconds.  Since the roundoff is done when flushing the inode, the
	// time may change unexpectedly by one second!!!
	(दीर्घ)st->st_mtime - mtime > 1 || mtime - (दीर्घ)st->st_mtime > 1
#अन्यथा
	(दीर्घ)st->st_mtime != mtime
#endif
	;
}

/*
 * Return TRUE if file encoding "fenc" requires conversion from or to
 * 'encoding'.
 */
    पूर्णांक
need_conversion(char_u *fenc)
{
    पूर्णांक		same_encoding;
    पूर्णांक		enc_flags;
    पूर्णांक		fenc_flags;

    जर (*fenc == NUL || STRCMP(p_enc, fenc) == 0)
    {
	same_encoding = TRUE;
	fenc_flags = 0;
    }
    अन्यथा
    {
	// Ignore difference between "ansi" and "latin1", "ucs-4" and
	// "ucs-4be", etc.
	enc_flags = get_fio_flags(p_enc);
	fenc_flags = get_fio_flags(fenc);
	same_encoding = (enc_flags != 0 && fenc_flags == enc_flags);
    }
    जर (same_encoding)
    {
	// Specified encoding matches with 'encoding'.  This requires
	// conversion when 'encoding' is Unicode but not UTF-8.
	परत enc_unicode != 0;
    }

    // Encodings differ.  However, conversion is not needed when 'enc' is any
    // Unicode encoding and the file is UTF-8.
    परत !(enc_utf8 && fenc_flags == FIO_UTF8);
}

/*
 * Check "ptr" for a unicode encoding and return the FIO_ flags needed for the
 * internal conversion.
 * if "ptr" is an empty string, use 'encoding'.
 */
    पूर्णांक
get_fio_flags(char_u *ptr)
{
    पूर्णांक		prop;

    जर (*ptr == NUL)
	ptr = p_enc;

    prop = enc_canon_props(ptr);
    जर (prop & ENC_UNICODE)
    {
	जर (prop & ENC_2BYTE)
	{
	    जर (prop & ENC_ENDIAN_L)
		परत FIO_UCS2 | FIO_ENDIAN_L;
	    परत FIO_UCS2;
	}
	जर (prop & ENC_4BYTE)
	{
	    जर (prop & ENC_ENDIAN_L)
		परत FIO_UCS4 | FIO_ENDIAN_L;
	    परत FIO_UCS4;
	}
	जर (prop & ENC_2WORD)
	{
	    जर (prop & ENC_ENDIAN_L)
		परत FIO_UTF16 | FIO_ENDIAN_L;
	    परत FIO_UTF16;
	}
	परत FIO_UTF8;
    }
    जर (prop & ENC_LATIN1)
	परत FIO_LATIN1;
    // must be ENC_DBCS, requires iconv()
    परत 0;
}

#जर defined(MSWIN)
/*
 * Check "ptr" for a MS-Windows codepage name and return the FIO_ flags needed
 * for the conversion MS-Windows can do for us.  Also accept "utf-8".
 * Used for conversion between 'encoding' and 'fileencoding'.
 */
    पूर्णांक
get_win_fio_flags(char_u *ptr)
{
    पूर्णांक		cp;

    // Cannot do this when 'encoding' is not utf-8 and not a codepage.
    जर (!enc_utf8 && enc_codepage <= 0)
	परत 0;

    cp = encname2codepage(ptr);
    जर (cp == 0)
    {
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	जर (STRCMP(ptr, "utf-8") == 0)
	    cp = CP_UTF8;
	अन्यथा
# endif
	    परत 0;
    }
    परत FIO_PUT_CP(cp) | FIO_CODEPAGE;
}
#endif

#जर defined(MACOS_CONVERT)
/*
 * Check "ptr" for a Carbon supported encoding and return the FIO_ flags
 * needed for the internal conversion to/from utf-8 or latin1.
 */
    पूर्णांक
get_mac_fio_flags(char_u *ptr)
{
    जर ((enc_utf8 || STRCMP(p_enc, "latin1") == 0)
				     && (enc_canon_props(ptr) & ENC_MACROMAN))
	परत FIO_MACROMAN;
    परत 0;
}
#endif

/*
 * Check for a Unicode BOM (Byte Order Mark) at the start of p[size].
 * "size" must be at least 2.
 * Return the name of the encoding and set "*lenp" to the length.
 * Returns NULL when no BOM found.
 */
    स्थिर char_u *
check_for_bom(
    char_u	*p,
    दीर्घ	size,
    पूर्णांक		*lenp,
    पूर्णांक		flags)
{
    अक्षर	*name = रिक्त;
    पूर्णांक		len = 2;

    जर (p[0] == 0xef && p[1] == 0xbb && size >= 3 && p[2] == 0xbf
	    && (flags == FIO_ALL || flags == FIO_UTF8 || flags == 0))
    {
	name = "utf-8";		// EF BB BF
	len = 3;
    }
    अन्यथा जर (p[0] == 0xff && p[1] == 0xfe)
    {
	जर (size >= 4 && p[2] == 0 && p[3] == 0
	    && (flags == FIO_ALL || flags == (FIO_UCS4 | FIO_ENDIAN_L)))
	{
	    name = "ucs-4le";	// FF FE 00 00
	    len = 4;
	}
	अन्यथा जर (flags == (FIO_UCS2 | FIO_ENDIAN_L))
	    name = "ucs-2le";	// FF FE
	अन्यथा जर (flags == FIO_ALL || flags == (FIO_UTF16 | FIO_ENDIAN_L))
	    // utf-16le is preferred, it also works for ucs-2le text
	    name = "utf-16le";	// FF FE
    }
    अन्यथा जर (p[0] == 0xfe && p[1] == 0xff
	    && (flags == FIO_ALL || flags == FIO_UCS2 || flags == FIO_UTF16))
    {
	// Default to utf-16, it works also for ucs-2 text.
	जर (flags == FIO_UCS2)
	    name = "ucs-2";	// FE FF
	अन्यथा
	    name = "utf-16";	// FE FF
    }
    अन्यथा जर (size >= 4 && p[0] == 0 && p[1] == 0 && p[2] == 0xfe
	    && p[3] == 0xff && (flags == FIO_ALL || flags == FIO_UCS4))
    {
	name = "ucs-4";		// 00 00 FE FF
	len = 4;
    }

    *lenp = len;
    परत (char_u *)name;
}

/*
 * Try to find a shortname by comparing the fullname with the current
 * directory.
 * Returns "full_path" or pointer into "full_path" if shortened.
 */
    char_u *
shorten_fname1(char_u *full_path)
{
    char_u	*स_नाम;
    char_u	*p = full_path;

    स_नाम = alloc(MAXPATHL);
    जर (स_नाम == रिक्त)
	परत full_path;
    जर (mch_dirname(स_नाम, MAXPATHL) == OK)
    {
	p = shorten_fname(full_path, स_नाम);
	जर (p == रिक्त || *p == NUL)
	    p = full_path;
    }
    vim_free(स_नाम);
    परत p;
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
    पूर्णांक		len;
    char_u	*p;

    जर (full_path == रिक्त)
	परत रिक्त;
    len = (पूर्णांक)STRLEN(dir_name);
    जर (fnamencmp(dir_name, full_path, len) == 0)
    {
	p = full_path + len;
#जर defined(MSWIN)
	/*
	 * MS-Windows: when a file is in the root directory, dir_name will end
	 * in a slash, since C: by itself does not define a specific dir. In
	 * this case p may already be correct. <negri>
	 */
	जर (!((len > 2) && (*(p - 2) == ':')))
#endif
	{
	    जर (vim_ispathsep(*p))
		करा
		    ++p;
		पर्यंत (vim_ispathsep_nocolon(*p));
#ifndef VMS   // the path separator is always part of the path
	    अन्यथा
		p = रिक्त;
#endif
	}
    }
#जर defined(MSWIN)
    /*
     * When using a file in the current drive, remove the drive name:
     * "A:\dir\file" -> "\dir\file".  This helps when moving a session file on
     * a floppy from "A:\dir" to "B:\dir".
     */
    अन्यथा जर (len > 3
	    && TOUPPER_LOC(full_path[0]) == TOUPPER_LOC(dir_name[0])
	    && full_path[1] == ':'
	    && vim_ispathsep(full_path[2]))
	p = full_path + 2;
#endif
    अन्यथा
	p = रिक्त;
    परत p;
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
    शून्य
shorten_buf_fname(buf_T *buf, char_u *स_नाम, पूर्णांक force)
{
    char_u	*p;

    जर (buf->b_fname != रिक्त
	    && !bt_nofilename(buf)
	    && !path_with_url(buf->b_fname)
	    && (force
		|| buf->b_sfname == रिक्त
		|| mch_isFullName(buf->b_sfname)))
    {
	जर (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	p = shorten_fname(buf->b_ffname, स_नाम);
	जर (p != रिक्त)
	{
	    buf->b_sfname = vim_strsave(p);
	    buf->b_fname = buf->b_sfname;
	}
	जर (p == रिक्त || buf->b_fname == रिक्त)
	    buf->b_fname = buf->b_ffname;
    }
}

/*
 * Shorten filenames for all buffers.
 */
    शून्य
shorten_fnames(पूर्णांक force)
{
    char_u	स_नाम[MAXPATHL];
    buf_T	*buf;

    mch_dirname(स_नाम, MAXPATHL);
    FOR_ALL_BUFFERS(buf)
    {
	shorten_buf_fname(buf, स_नाम, force);

	// Always make the swap file name a full path, a "nofile" buffer may
	// also have a swap file.
	mf_fullname(buf->b_ml.ml_mfp);
    }
    status_redraw_all();
    redraw_tabline = TRUE;
#जर defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#जर defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    popup_update_preview_title();
#endif
}

#जर (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
/*
 * Shorten all filenames in "fnames[count]" by current directory.
 */
    शून्य
shorten_filenames(char_u **fnames, पूर्णांक मोजणी)
{
    पूर्णांक		i;
    char_u	स_नाम[MAXPATHL];
    char_u	*p;

    जर (fnames == रिक्त || मोजणी < 1)
	परत;
    mch_dirname(स_नाम, आकार(स_नाम));
    साठी (i = 0; i < मोजणी; ++i)
    {
	जर ((p = shorten_fname(fnames[i], स_नाम)) != रिक्त)
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
    पूर्णांक	    prepend_dot)	// may prepend a '.' to file name
{
    परत buf_modname((curbuf->b_p_sn || curbuf->b_shortname),
						     fname, ext, prepend_dot);
}

    char_u *
buf_modname(
    पूर्णांक	    shortname,		// use 8.3 file name
    char_u  *fname,
    char_u  *ext,
    पूर्णांक	    prepend_dot)	// may prepend a '.' to file name
{
    char_u	*retval;
    char_u	*s;
    char_u	*e;
    char_u	*ptr;
    माप_प्रकार	ptrlen;
    पूर्णांक		fnamelen, extlen;

    extlen = (पूर्णांक)STRLEN(ext);

    /*
     * If there is no file name we must get the name of the current directory
     * (we need the full path in case :cd is used).
     */
    जर (fname == रिक्त || *fname == NUL)
    {
	retval = alloc(MAXPATHL + extlen + 3);
	जर (retval == रिक्त)
	    परत रिक्त;
	जर (mch_dirname(retval, MAXPATHL) == FAIL ||
				     (fnamelen = (पूर्णांक)STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    परत रिक्त;
	}
	जर (!after_pathsep(retval, retval + fnamelen))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
	prepend_dot = FALSE;	    // nothing to prepend a dot to
    }
    अन्यथा
    {
	fnamelen = (पूर्णांक)STRLEN(fname);
	retval = alloc(fnamelen + extlen + 3);
	जर (retval == रिक्त)
	    परत रिक्त;
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
    साठी (ptr = retval + fnamelen; ptr > retval; MB_PTR_BACK(retval, ptr))
    {
	जर (*ext == '.' && shortname)
	    जर (*ptr == '.')	// replace '.' by '_'
		*ptr = '_';
	जर (vim_ispathsep(*ptr))
	{
	    ++ptr;
	    थांबा;
	}
    }

    // the file name has at most BASENAMELEN characters.
    ptrlen = (माप_प्रकार)(fnamelen - (ptr - retval));
    जर (ptrlen > (अचिन्हित)BASENAMELEN)
    {
	ptrlen = BASENAMELEN;
	ptr[ptrlen] = NUL;
    }

    s = ptr + ptrlen;

    /*
     * For 8.3 file names we may have to reduce the length.
     */
    जर (shortname)
    {
	/*
	 * If there is no file name, or the file name ends in '/', and the
	 * extension starts with '.', put a '_' before the dot, because just
	 * ".ext" is invalid.
	 */
	जर (fname == रिक्त || *fname == NUL
				   || vim_ispathsep(fname[STRLEN(fname) - 1]))
	{
	    जर (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	अन्यथा जर (*ext == '.')
	{
	    जर ((माप_प्रकार)(s - ptr) > (माप_प्रकार)8)
	    {
		s = ptr + 8;
		*s = '\0';
	    }
	}
	/*
	 * If the extension doesn't start with '.', and the file name
	 * doesn't have an extension yet, append a '.'
	 */
	अन्यथा जर ((e = vim_strchr(ptr, '.')) == रिक्त)
	    *s++ = '.';
	/*
	 * If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be truncated
	 */
	अन्यथा जर ((पूर्णांक)(ptrlen - (e - retval)) + extlen > 4)
	    s = e + 4 - extlen;
    }
#ifdef MSWIN
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    अन्यथा जर ((fname == रिक्त || *fname == NUL) && *ext == '.')
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
    जर (prepend_dot && !shortname && *(e = gettail(retval)) != '.')
    {
	mch_memmove(e + 1, e, (माप_प्रकार)(((fnamelen + extlen) - (e - retval)) + 1));	// +1 for NUL
	*e = '.';
    }

    /*
     * Check that, after appending the extension, the file name is really
     * different.
     */
    जर (fname != रिक्त && STRCMP(fname, retval) == 0)
    {
	// we search for a character that can be replaced by '_'
	पर्यंत (--s >= ptr)
	{
	    जर (*s != '_')
	    {
		*s = '_';
		थांबा;
	    }
	}
	जर (s < ptr)	// fname was "________.<ext>", how tricky!
	    *ptr = 'v';
    }
    परत retval;
}

/*
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE for end-of-file.
 * If the line is truncated then buf[size - 2] will not be NUL.
 */
    पूर्णांक
vim_fgets(char_u *buf, पूर्णांक size, खाता *fp)
{
    अक्षर	*eof;
#define FGETS_SIZE 200
    अक्षर	tbuf[FGETS_SIZE];

    // safety check
    जर (size < 2)
    {
	जर (size == 1)
	    buf[0] = NUL;
	परत TRUE;
    }

    buf[size - 2] = NUL;
    eof = ख_माला_लो((अक्षर *)buf, size, fp);
    जर (buf[size - 2] != NUL && buf[size - 2] != '\n')
    {
	buf[size - 1] = NUL;	    // Truncate the line

	// Now throw away the rest of the line:
	करा
	{
	    tbuf[FGETS_SIZE - 2] = NUL;
	    vim_ignoredp = ख_माला_लो((अक्षर *)tbuf, FGETS_SIZE, fp);
	} पर्यंत (tbuf[FGETS_SIZE - 2] != NUL && tbuf[FGETS_SIZE - 2] != '\n');
    }
    परत (eof == रिक्त);
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 * Return -1 for failure, 0 for success.
 */
    पूर्णांक
vim_rename(char_u *from, char_u *to)
{
    पूर्णांक		n;
    पूर्णांक		ret;
#ifdef AMIGA
    BPTR	flock;
#endif
    stat_T	st;
    पूर्णांक		use_tmp_file = FALSE;

    /*
     * When the names are identical, there is nothing to do.  When they refer
     * to the same file (ignoring case and slash/backslash differences) but
     * the file name differs we need to go through a temp file.
     */
    जर (fnamecmp(from, to) == 0)
    {
	जर (p_fic && STRCMP(gettail(from), gettail(to)) != 0)
	    use_tmp_file = TRUE;
	अन्यथा
	    परत 0;
    }

    /*
     * Fail if the "from" file doesn't exist.  Avoids that "to" is deleted.
     */
    जर (mch_stat((अक्षर *)from, &st) < 0)
	परत -1;

#ifdef UNIX
    {
	stat_T	st_to;

	// It's possible for the source and destination to be the same file.
	// This happens when "from" and "to" differ in case and are on a FAT32
	// filesystem.  In that case go through a temp file name.
	जर (mch_stat((अक्षर *)to, &st_to) >= 0
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
	जर (win32_fileinfo(from, &info1) == FILEINFO_OK
		&& win32_fileinfo(to, &info2) == FILEINFO_OK
		&& info1.dw_Volume_Serial_संख्या == info2.dw_Volume_Serial_संख्या
		&& info1.n_File_Index_वरचा == info2.n_File_Index_वरचा
		&& info1.n_File_Index_खालचा == info2.n_File_Index_खालचा)
	    use_tmp_file = TRUE;
    }
#endif

    जर (use_tmp_file)
    {
	अक्षर	tempname[MAXPATHL + 1];

	/*
	 * Find a name that doesn't exist and is in the same directory.
	 * Rename "from" to "tempname" and then rename "tempname" to "to".
	 */
	जर (STRLEN(from) >= MAXPATHL - 5)
	    परत -1;
	STRCPY(tempname, from);
	साठी (n = 123; n < 99999; ++n)
	{
	    प्र_लिखो((अक्षर *)gettail((char_u *)tempname), "%d", n);
	    जर (mch_stat(tempname, &st) < 0)
	    {
		जर (mch_rename((अक्षर *)from, tempname) == 0)
		{
		    जर (mch_rename(tempname, (अक्षर *)to) == 0)
			परत 0;
		    // Strange, the second step failed.  Try moving the
		    // file back and return failure.
		    (शून्य)mch_rename(tempname, (अक्षर *)from);
		    परत -1;
		}
		// If it fails for one temp name it will most likely fail
		// for any temp name, give up.
		परत -1;
	    }
	}
	परत -1;
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
    flock = Lock((UBYTE *)from, (दीर्घ)VIM_ACCESS_READ);
#endif
    mch_remove(to);
#ifdef AMIGA
    जर (flock)
	UnLock(flock);
#endif

    /*
     * First try a normal rename, return if it works.
     */
    जर (mch_rename((अक्षर *)from, (अक्षर *)to) == 0)
	परत 0;

    /*
     * Rename() failed, try copying the file.
     */
    ret = vim_copyfile(from, to);
    जर (ret != OK)
	परत -1;

    /*
     * Remove copied original file
     */
    जर (mch_stat((अक्षर *)from, &st) >= 0)
	mch_remove(from);

    परत 0;
}


/*
 * Create the new file with same permissions as the original.
 * Return FAIL for failure, OK for success.
 */
    पूर्णांक
vim_copyfile(char_u *from, char_u *to)
{
    पूर्णांक		fd_in;
    पूर्णांक		fd_out;
    पूर्णांक		n;
    अक्षर	*errmsg = रिक्त;
    अक्षर	*buffer;
    दीर्घ	perm;
#ifdef HAVE_ACL
    vim_acl_T	acl;		// ACL from original file
#endif

#ifdef HAVE_READLINK
    पूर्णांक		ret;
    पूर्णांक		len;
    stat_T	st;
    अक्षर	linkbuf[MAXPATHL + 1];

    ret = mch_lstat((अक्षर *)from, &st);
    जर (ret >= 0 && S_ISLNK(st.st_mode))
    {
	ret = -1;

	len = readlink((अक्षर *)from, linkbuf, MAXPATHL);
	जर (len > 0)
	{
	    linkbuf[len] = NUL;

	    // Create link
	    ret = symlink(linkbuf, (अक्षर *)to);
	}

	परत ret == 0 ? OK : FAIL;
    }
#endif

    perm = mch_getperm(from);
#ifdef HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    acl = mch_get_acl(from);
#endif
    fd_in = mch_open((अक्षर *)from, O_RDONLY|O_EXTRA, 0);
    जर (fd_in == -1)
    {
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	परत FAIL;
    }

    // Create the new file with same permissions as the original.
    fd_out = mch_open((अक्षर *)to,
		       O_CREAT|O_EXCL|O_WRONLY|O_EXTRA|O_NOFOLLOW, (पूर्णांक)perm);
    जर (fd_out == -1)
    {
	बंद(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	परत FAIL;
    }

    buffer = alloc(WRITEBUFSIZE);
    जर (buffer == रिक्त)
    {
	बंद(fd_out);
	बंद(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	परत FAIL;
    }

    पर्यंत ((n = read_eintr(fd_in, buffer, WRITEBUFSIZE)) > 0)
	जर (write_eintr(fd_out, buffer, n) != n)
	{
	    errmsg = _(e_error_writing_to_str);
	    थांबा;
	}

    vim_free(buffer);
    बंद(fd_in);
    जर (बंद(fd_out) < 0)
	errmsg = _(e_error_closing_str);
    जर (n < 0)
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
#जर defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    mch_copy_sec(from, to);
#endif
    जर (errmsg != रिक्त)
    {
	semsg(errmsg, to);
	परत FAIL;
    }
    परत OK;
}

स्थिर पूर्णांक already_warned = FALSE;

/*
 * Check if any not hidden buffer has been changed.
 * Postpone the check if there are characters in the stuff buffer, a global
 * command is being executed, a mapping is being executed or an autocommand is
 * busy.
 * Returns TRUE if some message was written (screen should be redrawn and
 * cursor positioned).
 */
    पूर्णांक
check_timestamps(
    पूर्णांक		focus)		// called for GUI focus event
{
    buf_T	*buf;
    पूर्णांक		didit = 0;
    पूर्णांक		n;

    // Don't check timestamps while system() or another low-level function may
    // cause us to lose and gain focus.
    जर (no_check_timestamps > 0)
	परत FALSE;

    // Avoid doing a check twice.  The OK/Reload dialog can cause a focus
    // event and we would keep on checking if the file is steadily growing.
    // Do check again after typing something.
    जर (focus && did_check_timestamps)
    {
	need_check_timestamps = TRUE;
	परत FALSE;
    }

    जर (!stuff_empty() || global_busy || !typebuf_typed()
			|| autocmd_busy || curbuf_lock > 0 || allbuf_lock > 0)
	need_check_timestamps = TRUE;		// check later
    अन्यथा
    {
	++no_wait_return;
	did_check_timestamps = TRUE;
	already_warned = FALSE;
	FOR_ALL_BUFFERS(buf)
	{
	    // Only check buffers in a window.
	    जर (buf->b_nwindows > 0)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		n = buf_check_timestamp(buf, focus);
		जर (didit < n)
		    didit = n;
		जर (n > 0 && !bufref_valid(&bufref))
		{
		    // Autocommands have removed the buffer, start at the
		    // first one again.
		    buf = firstbuf;
		    सुरूठेवा;
		}
	    }
	}
	--no_wait_return;
	need_check_timestamps = FALSE;
	जर (need_wait_return && didit == 2)
	{
	    // make sure msg isn't overwritten
	    संदेश_puts("\n");
	    out_flush();
	}
    }
    परत didit;
}

/*
 * Move all the lines from buffer "frombuf" to buffer "tobuf".
 * Return OK or FAIL.  When FAIL "tobuf" is incomplete and/or "frombuf" is not
 * empty.
 */
    स्थिर पूर्णांक
move_ओळी(buf_T *frombuf, buf_T *tobuf)
{
    buf_T	*tbuf = curbuf;
    पूर्णांक		retval = OK;
    linenr_T	lnum;
    char_u	*p;

    // Copy the lines in "frombuf" to "tobuf".
    curbuf = tobuf;
    साठी (lnum = 1; lnum <= frombuf->b_ml.ml_ओळ_मोजणी; ++lnum)
    {
	p = vim_strnsave(ml_get_buf(frombuf, lnum, FALSE), ml_get_buf_len(frombuf, lnum));
	जर (p == रिक्त || ml_append(lnum - 1, p, 0, FALSE) == FAIL)
	{
	    vim_free(p);
	    retval = FAIL;
	    थांबा;
	}
	vim_free(p);
    }

    // Delete all the lines in "frombuf".
    जर (retval != FAIL)
    {
	curbuf = frombuf;
	साठी (lnum = curbuf->b_ml.ml_ओळ_मोजणी; lnum > 0; --lnum)
	    जर (ml_delete(lnum) == FAIL)
	    {
		// Oops!  We could try putting back the saved lines, but that
		// might fail again...
		retval = FAIL;
		थांबा;
	    }
    }

    curbuf = tbuf;
    परत retval;
}

/*
 * Check if buffer "buf" has been changed.
 * Also check if the file for a new buffer unexpectedly appeared.
 * return 1 if a changed buffer was found.
 * return 2 if a message has been displayed.
 * return 0 otherwise.
 */
    पूर्णांक
buf_check_timestamp(
    buf_T	*buf,
    पूर्णांक		focus UNUSED)	// called for GUI focus event
{
    stat_T	st;
    पूर्णांक		stat_res;
    पूर्णांक		retval = 0;
    अक्षर	*mesg = रिक्त;
    अक्षर	*mesg2 = "";
    पूर्णांक		helpmesg = FALSE;
    गणना {
	RELOAD_NONE,
	RELOAD_NORMAL,
	RELOAD_DETECT
    }		reload = RELOAD_NONE;
#जर defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
    पूर्णांक		can_reload = FALSE;
#endif
    off_T	orig_size = buf->b_orig_size;
    पूर्णांक		orig_mode = buf->b_orig_mode;
#ifdef FEAT_GUI
    पूर्णांक		save_mouse_correct = need_mouse_correct;
#endif
    स्थिर पूर्णांक	busy = FALSE;
    पूर्णांक		n;
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    // If there is no file name, the buffer is not loaded, 'buftype' is
    // set, we are in the middle of a save or being called recursively: ignore
    // this buffer.
    जर (buf->b_ffname == रिक्त
	    || buf->b_ml.ml_mfp == रिक्त
	    || !bt_normal(buf)
	    || buf->b_saving
	    || busy
#ifdef FEAT_NETBEANS_INTG
	    || isNetbeansBuffer(buf)
#endif
#ifdef FEAT_TERMINAL
	    || buf->b_term != रिक्त
#endif
	    )
	परत 0;

    जर (       !(buf->b_flags & BF_NOTEDITED)
	    && buf->b_mtime != 0
	    && ((stat_res = mch_stat((अक्षर *)buf->b_ffname, &st)) < 0
		|| time_differs(&st, buf->b_mtime, buf->b_mtime_ns)
		|| st.st_size != buf->b_orig_size
#ifdef HAVE_ST_MODE
		|| (पूर्णांक)st.st_mode != buf->b_orig_mode
#अन्यथा
		|| mch_getperm(buf->b_ffname) != buf->b_orig_mode
#endif
		))
    {
	दीर्घ prev_b_mtime = buf->b_mtime;

	retval = 1;

	// set b_mtime to stop further warnings (e.g., when executing
	// FileChangedShell autocmd)
	जर (stat_res < 0)
	{
	    // Check the file again later to see if it re-appears.
	    buf->b_mtime = -1;
	    buf->b_orig_size = 0;
	    buf->b_orig_mode = 0;
	}
	अन्यथा
	    buf_store_time(buf, &st, buf->b_ffname);

	// Don't do anything for a directory.  Might contain the file
	// explorer.
	जर (mch_isdir(buf->b_fname))
	    ;

	/*
	 * If 'autoread' is set, the buffer has no changes and the file still
	 * exists, reload the buffer.  Use the buffer-local option value if it
	 * was set, the global option value otherwise.
	 */
	अन्यथा जर ((buf->b_p_ar >= 0 ? buf->b_p_ar : p_ar)
				       && !bufIsChanged(buf) && stat_res >= 0)
	    reload = RELOAD_NORMAL;
	अन्यथा
	{
	    अक्षर    *reason;
#ifdef FEAT_EVAL
	    माप_प्रकार  reasonlen;
#endif

	    जर (stat_res < 0)
	    {
		reason = "deleted";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("deleted");
#endif
	    }
	    अन्यथा जर (bufIsChanged(buf))
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
	    अन्यथा जर (orig_size != buf->b_orig_size || buf_contents_changed(buf))
	    {
		reason = "changed";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("changed");
#endif
	    }
	    अन्यथा जर (orig_mode != buf->b_orig_mode)
	    {
		reason = "mode";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("mode");
#endif
	    }
	    अन्यथा
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
	    set_vim_var_string(VV_FCS_REASON, (char_u *)reason, (पूर्णांक)reasonlen);
	    set_vim_var_string(VV_FCS_CHOICE, (char_u *)"", 0);
#endif
	    ++allbuf_lock;
	    n = apply_autocmds(EVENT_FILECHANGEDSHELL,
				      buf->b_fname, buf->b_fname, FALSE, buf);
	    --allbuf_lock;
	    busy = FALSE;
	    जर (n)
	    {
		जर (!bufref_valid(&bufref))
		    emsg(_(e_filechangedshell_autocommand_deleted_buffer));
#ifdef FEAT_EVAL
		char_u	*s = get_vim_var_str(VV_FCS_CHOICE);
		जर (STRCMP(s, "reload") == 0 && *reason != 'd')
		    reload = RELOAD_NORMAL;
		अन्यथा जर (STRCMP(s, "edit") == 0)
		    reload = RELOAD_DETECT;
		अन्यथा जर (STRCMP(s, "ask") == 0)
		    n = FALSE;
		अन्यथा
#endif
		    परत 2;
	    }
	    जर (!n)
	    {
		जर (*reason == 'd')
		{
		    // Only give the message once.
		    जर (prev_b_mtime != -1)
			mesg = _(e_file_str_no_longer_available);
		}
		अन्यथा
		{
		    helpmesg = TRUE;
#जर defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		    can_reload = TRUE;
#endif
		    जर (reason[2] == 'n')
		    {
			mesg = _("W12: Warning: File \"%s\" has changed and the buffer was changed in Vim as well");
			mesg2 = _("See \":help W12\" for more info.");
		    }
		    अन्यथा जर (reason[1] == 'h')
		    {
			mesg = _("W11: Warning: File \"%s\" has changed since editing started");
			mesg2 = _("See \":help W11\" for more info.");
		    }
		    अन्यथा जर (*reason == 'm')
		    {
			mesg = _("W16: Warning: Mode of file \"%s\" has changed since editing started");
			mesg2 = _("See \":help W16\" for more info.");
		    }
		    अन्यथा
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
    अन्यथा जर ((buf->b_flags & BF_NEW) && !(buf->b_flags & BF_NEW_W)
						&& vim_fexists(buf->b_ffname))
    {
	retval = 1;
	mesg = _("W13: Warning: File \"%s\" has been created after editing started");
	buf->b_flags |= BF_NEW_W;
#जर defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	can_reload = TRUE;
#endif
    }

    जर (mesg != रिक्त)
    {
	char_u *path;

	path = home_replace_save(buf, buf->b_fname);
	जर (path != रिक्त)
	{
	    माप_प्रकार  tbufsize;
	    अक्षर    *tbuf;

	    जर (!helpmesg)
		mesg2 = "";
	    tbufsize = STRLEN(mesg) + STRLEN(path) + 2 + STRLEN(mesg2) + 1; // +2 for either '\n' or "; "
									    // and +1 for NUL
	    tbuf = alloc(tbufsize);
	    जर (tbuf != रिक्त)
	    {
		पूर्णांक tbuflen;

		tbuflen = vim_snprintf(tbuf, tbufsize, mesg, path);
#ifdef FEAT_EVAL
		// Set warningmsg here, before the unimportant and output-specific
		// mesg2 has been appended.
		set_vim_var_string(VV_WARNINGMSG, (char_u *)tbuf, tbuflen);
#endif
#जर defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		जर (can_reload)
		{
		    जर (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "\n%s", mesg2);
		    निवड (do_dialog(VIM_WARNING, (char_u *)_("Warning"),
			    (char_u *)tbuf,
			    (char_u *)_("&OK\n&Load File\nLoad File &and Options"),
			    1, रिक्त, TRUE))
		    {
			स्थिती 2:
			    reload = RELOAD_NORMAL;
			    थांबा;
			स्थिती 3:
			    reload = RELOAD_DETECT;
			    थांबा;
		    }
		}
		अन्यथा
#endif
		जर (State > MODE_NORMAL_BUSY || (State & MODE_CMDLINE)
								 || already_warned)
		{
		    जर (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "; %s", mesg2);
		    emsg(tbuf);
		    retval = 2;
		}
		अन्यथा
		{
		    जर (!autocmd_busy)
		    {
			संदेश_start();
			संदेश_puts_attr(tbuf, HL_ATTR(HLF_E) + संदेश_HIST);
			जर (*mesg2 != NUL)
			    संदेश_puts_attr(mesg2, HL_ATTR(HLF_W) + संदेश_HIST);
			संदेश_clr_eos();
			(शून्य)संदेश_end();
			जर (emsg_silent == 0 && !in_assert_fails)
			{
			    out_flush();
#ifdef FEAT_GUI
			    जर (!focus)
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

    जर (reload != RELOAD_NONE)
    {
	// Reload the buffer.
	buf_reload(buf, orig_mode, reload == RELOAD_DETECT);
#ifdef FEAT_PERSISTENT_UNDO
	जर (bufref_valid(&bufref) && buf->b_p_udf && buf->b_ffname != रिक्त)
	{
	    char_u	    hash[UNDO_HASH_SIZE];
	    buf_T	    *save_curbuf = curbuf;

	    // Any existing undo file is unusable, write it now.
	    curbuf = buf;
	    u_compute_hash(hash);
	    u_write_undo(रिक्त, FALSE, buf, hash);
	    curbuf = save_curbuf;
	}
#endif
    }

    // Trigger FileChangedShell when the file was changed in any way.
    जर (bufref_valid(&bufref) && retval != 0)
	(शून्य)apply_autocmds(EVENT_FILECHANGEDSHELLPOST,
				      buf->b_fname, buf->b_fname, FALSE, buf);
#ifdef FEAT_GUI
    // restore this in case an autocommand has set it; it would break
    // 'mousefocus'
    need_mouse_correct = save_mouse_correct;
#endif

    परत retval;
}

/*
 * Reload a buffer that is already loaded.
 * Used when the file was changed outside of Vim.
 * "orig_mode" is buf->b_orig_mode before the need for reloading was detected.
 * buf->b_orig_mode may have been reset already.
 */
    शून्य
buf_reload(buf_T *buf, पूर्णांक orig_mode, पूर्णांक reload_options)
{
    exarg_T	ea;
    pos_T	old_cursor;
    linenr_T	old_topline;
    पूर्णांक		old_ro = buf->b_p_ro;
    buf_T	*savebuf;
    bufref_T	bufref;
    पूर्णांक		saved = OK;
    aco_save_T	aco;
    पूर्णांक		flags = READ_NEW;
    पूर्णांक		prepped = OK;

    // Set curwin/curbuf for "buf" and save some things.
    aucmd_prepbuf(&aco, buf);
    जर (curbuf != buf)
    {
	// Failed to find a window for "buf", it is dangerous to continue,
	// better bail out.
	परत;
    }

    // Unless reload_options is set, we only want to read the text from the
    // file, not reset the syntax highlighting, clear marks, diff status, etc.
    // Force the fileformat and encoding to be the same.
    जर (reload_options)
	CLEAR_FIELD(ea);
    अन्यथा
	prepped = prep_exarg(&ea, buf);

    जर (prepped == OK)
    {
	old_cursor = curwin->w_cursor;
	old_topline = curwin->w_topline;

	जर (p_ur < 0 || curbuf->b_ml.ml_ओळ_मोजणी <= p_ur)
	{
	    // Save all the text, so that the reload can be undone.
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    saved = u_savecommon(0, curbuf->b_ml.ml_ओळ_मोजणी + 1, 0, TRUE);
	    flags |= READ_KEEP_UNDO;
	}

	/*
	 * To behave like when a new file is edited (matters for
	 * BufReadPost autocommands) we first need to delete the current
	 * buffer contents.  But if reading the file fails we should keep
	 * the old contents.  Can't use memory only, the file might be
	 * too big.  Use a hidden buffer to move the buffer contents to.
	 */
	जर (BUFEMPTY() || saved == FAIL)
	    savebuf = रिक्त;
	अन्यथा
	{
	    // Allocate a buffer without putting it in the buffer list.
	    savebuf = buflist_new(रिक्त, रिक्त, (linenr_T)1, BLN_DUMMY);
	    set_bufref(&bufref, savebuf);
	    जर (savebuf != रिक्त && buf == curbuf)
	    {
		// Open the memline.
		curbuf = savebuf;
		curwin->w_buffer = savebuf;
		saved = ml_open(curbuf);
		curbuf = buf;
		curwin->w_buffer = buf;
	    }
	    जर (savebuf == रिक्त || saved == FAIL || buf != curbuf
				      || move_ओळी(buf, savebuf) == FAIL)
	    {
		semsg(_(e_could_not_prepare_for_reloading_str), buf->b_fname);
		saved = FAIL;
	    }
	}

	जर (saved == OK)
	{
	    पूर्णांक old_संदेश_silent = संदेश_silent;

	    curbuf->b_flags |= BF_CHECK_RO;	// check for RO again
	    curbuf->b_keep_filetype = true;	// don't detect 'filetype'

	    जर (shortmess(SHM_FILEINFO))
		संदेश_silent = 1;

	    जर (readfile(buf->b_ffname, buf->b_fname, (linenr_T)0,
			(linenr_T)0,
			(linenr_T)MAXLNUM, &ea, flags) != OK)
	    {
#जर defined(FEAT_EVAL)
		जर (!aborting())
#endif
		    semsg(_(e_could_not_reload_str), buf->b_fname);
		जर (savebuf != रिक्त && bufref_valid(&bufref) && buf == curbuf)
		{
		    // Put the text back from the save buffer.  First
		    // delete any lines that readfile() added.
		    पर्यंत (!BUFEMPTY())
			जर (ml_delete(buf->b_ml.ml_ओळ_मोजणी) == FAIL)
			    थांबा;
		    (शून्य)move_ओळी(savebuf, buf);
		}
	    }
	    अन्यथा जर (buf == curbuf)  // "buf" still valid
	    {
		// Mark the buffer as unmodified and free undo info.
		unchanged(buf, TRUE, TRUE);
		जर ((flags & READ_KEEP_UNDO) == 0)
		    u_clearallandblockfree(buf);
		अन्यथा
		{
		    // Mark all undo states as changed.
		    u_unchanged(curbuf);
		}
	    }

	    संदेश_silent = old_संदेश_silent;
	}
	vim_free(ea.cmd);

	जर (savebuf != रिक्त && bufref_valid(&bufref))
	    wipe_buffer(savebuf, FALSE);

#ifdef FEAT_DIFF
	// Invalidate diff info if necessary.
	diff_invalidate(curbuf);
#endif

	// Restore the topline and cursor position and check it (lines may
	// have been removed).
	जर (old_topline > curbuf->b_ml.ml_ओळ_मोजणी)
	    curwin->w_topline = curbuf->b_ml.ml_ओळ_मोजणी;
	अन्यथा
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
		जर (wp->w_buffer == curwin->w_buffer
			&& !foldmethodIsManual(wp))
		    foldUpdateAll(wp);
	}
#endif
	// If the mode didn't change and 'readonly' was set, keep the old
	// value; the user probably used the ":view" command.  But don't
	// reset it, might have had a read error.
	जर (orig_mode == curbuf->b_orig_mode)
	    curbuf->b_p_ro |= old_ro;

	// Modelines must override settings done by autocommands.
	do_modelines(0);
    }

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);
    // Careful: autocommands may have made "buf" invalid!
}

    शून्य
buf_store_time(buf_T *buf, stat_T *st, char_u *fname UNUSED)
{
    buf->b_mtime = (दीर्घ)st->st_mtime;
#ifdef ST_MTIM_NSEC
    buf->b_mtime_ns = (दीर्घ)st->ST_MTIM_NSEC;
#अन्यथा
    buf->b_mtime_ns = 0;
#endif
    buf->b_orig_size = st->st_size;
#ifdef HAVE_ST_MODE
    buf->b_orig_mode = (पूर्णांक)st->st_mode;
#अन्यथा
    buf->b_orig_mode = mch_getperm(fname);
#endif
}

/*
 * Adjust the line with missing eol, used for the next write.
 * Used for do_filter(), when the input lines for the filter are deleted.
 */
    शून्य
write_lnum_adjust(linenr_T offset)
{
    जर (curbuf->b_no_eol_lnum != 0)	// only if there is a missing eol
	curbuf->b_no_eol_lnum += offset;
}

// Subfuncions for readdirex()
#ifdef FEAT_EVAL
# ifdef MSWIN
    स्थिर char_u *
getfpermwfd(WIN32_FIND_DATAW *wfd, char_u *perm)
{
    stat_T	    st;
    अचिन्हित लघु  st_mode;
    DWORD	    flag = wfd->dwFileAttributes;
    WCHAR	    *wp;

    st_mode = (flag & FILE_ATTRIBUTE_DIRECTORY)
					? (_S_IFDIR | _S_IEXEC) : _S_IFREG;
    st_mode |= (flag & FILE_ATTRIBUTE_READONLY)
					? _S_IREAD : (_S_IREAD | _S_IWRITE);

    wp = wcsrchr(wfd->cFileName, L'.');
    जर (wp != रिक्त)
    {
	जर (_wcsicmp(wp, L".exe") == 0 ||
		_wcsicmp(wp, L".com") == 0 ||
		_wcsicmp(wp, L".cmd") == 0 ||
		_wcsicmp(wp, L".bat") == 0)
	    st_mode |= _S_IEXEC;
    }

    // Copy user bits to group/other.
    st_mode |= (st_mode & 0700) >> 3;
    st_mode |= (st_mode & 0700) >> 6;

    st.st_mode = st_mode;
    परत getfpermst(&st, perm);
}

    स्थिर शून्य
getftypewfd(WIN32_FIND_DATAW *wfd, string_T *ret)
{
    DWORD flag = wfd->dwFileAttributes;
    DWORD tag = wfd->dwReserved0;

    जर (flag & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	जर (tag == IO_REPARSE_TAG_MOUNT_POINT)
	{
	    ret->string = (char_u *)"junction";
	    ret->length = STRLEN_LITERAL("junction");
	    परत;
	}
	जर (tag == IO_REPARSE_TAG_SYMLINK)
	{
	    जर (flag & FILE_ATTRIBUTE_DIRECTORY)
	    {
		ret->string = (char_u *)"linkd";
		ret->length = STRLEN_LITERAL("linkd");
		परत;
	    }

	    ret->string = (char_u *)"link";
	    ret->length = STRLEN_LITERAL("link");
	    परत;
	}

	// unknown reparse point type
	ret->string = (char_u *)"reparse";
	ret->length = STRLEN_LITERAL("reparse");
	परत;
    }

    जर (flag & FILE_ATTRIBUTE_DIRECTORY)
    {
	ret->string = (char_u *)"dir";
	ret->length = STRLEN_LITERAL("dir");
	परत;
    }

    ret->string = (char_u *)"file";
    ret->length = STRLEN_LITERAL("file");
}

    स्थिर dict_T *
create_readdirex_item(WIN32_FIND_DATAW *wfd)
{
    dict_T	*item;
    char_u	*p;
    varnumber_T	size, समय;
    char_u	permbuf[] = "---------";
    string_T	s;

    item = dict_alloc();
    जर (item == रिक्त)
	परत रिक्त;
    item->dv_refcount++;

    p = utf16_to_enc(wfd->cFileName, रिक्त);
    जर (p == रिक्त)
	जा theend;
    जर (dict_add_string(item, "name", p) == FAIL)
    {
	vim_free(p);
	जा theend;
    }
    vim_free(p);

    size = (((varnumber_T)wfd->n_File_Size_वरचा) << 32) | wfd->n_File_Size_खालचा;
    जर (dict_add_संख्या(item, "size", size) == FAIL)
	जा theend;

    // Convert FILETIME to unix time.
    समय = (((((varnumber_T)wfd->ftLastWriteTime.dw_वरचा_Date_Time) << 32) |
		wfd->ftLastWriteTime.dw_खालचा_Date_Time)
	    - 116444736000000000) / 10000000;
    जर (dict_add_संख्या(item, "time", समय) == FAIL)
	जा theend;

    getftypewfd(wfd, &s);
    जर (dict_add_string_len(item, "type", s.string, (पूर्णांक)s.length) == FAIL)
	जा theend;
    जर (dict_add_string(item, "perm", getfpermwfd(wfd, permbuf)) == FAIL)
	जा theend;

    जर (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	जा theend;
    जर (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	जा theend;

    परत item;

theend:
    dict_unref(item);
    परत रिक्त;
}
# अन्यथा
    स्थिर dict_T *
create_readdirex_item(char_u *path, char_u *name)
{
    dict_T	*item;
    अक्षर	*p;
    माप_प्रकार	pathlen, len;
    माप_प्रकार	namelen;
    stat_T	st;
    पूर्णांक		ret, link = FALSE;
    varnumber_T	size;
    char_u	permbuf[] = "---------";
    string_T	q = {रिक्त, 0};
    संरचना passwd *pw;
    संरचना group  *gr;

    item = dict_alloc();
    जर (item == रिक्त)
	परत रिक्त;
    item->dv_refcount++;

    pathlen = STRLEN(path);
    namelen = STRLEN(name);
    len = pathlen + 1 + namelen + 1;
    p = alloc(len);
    जर (p == रिक्त)
	जा theend;
    जर (pathlen > 0 && path[pathlen - 1] == '/')
	vim_snprintf(p, len, "%s%s", path, name);
    अन्यथा
	vim_snprintf(p, len, "%s/%s", path, name);
    ret = mch_lstat(p, &st);
    जर (ret >= 0 && S_ISLNK(st.st_mode))
    {
	link = TRUE;
	ret = mch_stat(p, &st);
	जर (ret < 0)
	    STR_LITERAL_SET(q, "link");
    }
    vim_free(p);

    जर (dict_add_string_len(item, "name", name, (पूर्णांक)namelen) == FAIL)
	जा theend;

    जर (ret >= 0)
    {
	size = (varnumber_T)st.st_size;
	जर (S_ISDIR(st.st_mode))
	    size = 0;
	// non-perfect check for overflow
	अन्यथा जर ((off_T)size != (off_T)st.st_size)
	    size = -2;
	जर (dict_add_संख्या(item, "size", size) == FAIL)
	    जा theend;
	जर (dict_add_संख्या(item, "time", (varnumber_T)st.st_mtime) == FAIL)
	    जा theend;

	जर (link)
	{
	    जर (S_ISDIR(st.st_mode))
		STR_LITERAL_SET(q, "linkd");
	    अन्यथा
		STR_LITERAL_SET(q, "link");
	}
	अन्यथा
	{
	    q.string = getftypest(&st);
	    q.length = STRLEN(q.string);
	}
	जर (dict_add_string_len(item, "type", q.string, (पूर्णांक)q.length) == FAIL)
	    जा theend;
	जर (dict_add_string(item, "perm", getfpermst(&st, permbuf)) == FAIL)
	    जा theend;

	pw = getpwuid(st.st_uid);
	जर (pw == रिक्त)
	    STR_LITERAL_SET(q, "");
	अन्यथा
	{
	    q.string = (char_u *)pw->pw_name;
	    q.length = STRLEN(q.string);
	}
	जर (dict_add_string_len(item, "user", q.string, (पूर्णांक)q.length) == FAIL)
	    जा theend;
#  जर !defined(VMS) || (defined(VMS) && defined(HAVE_XOS_R_H))
	gr = getgrgid(st.st_gid);
	जर (gr == रिक्त)
	    STR_LITERAL_SET(q, "");
	अन्यथा
	{
	    q.string = (char_u *)gr->gr_name;
	    q.length = STRLEN(q.string);
	}
#  endif
	जर (dict_add_string_len(item, "group", q.string, (पूर्णांक)q.length) == FAIL)
	    जा theend;
    }
    अन्यथा
    {
	जर (dict_add_संख्या(item, "size", -1) == FAIL)
	    जा theend;
	जर (dict_add_संख्या(item, "time", -1) == FAIL)
	    जा theend;
	जर (q.string == रिक्त)
	{
	    जर (dict_add_string_len(item, "type", (char_u *)"", 0) == FAIL)
		जा theend;
	}
	अन्यथा
	{
	    जर (dict_add_string_len(item, "type", q.string, (पूर्णांक)q.length) == FAIL)
		जा theend;
	}
	जर (dict_add_string_len(item, "perm", (char_u *)"", 0) == FAIL)
	    जा theend;
	जर (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	    जा theend;
	जर (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	    जा theend;
    }
    परत item;

theend:
    dict_unref(item);
    परत रिक्त;
}
# endif

    स्थिर पूर्णांक
compare_readdirex_item(स्थिरांक शून्य *p1, स्थिरांक शून्य *p2)
{
    char_u  *name1, *name2;

    name1 = dict_get_string(*(dict_T**)p1, "name", FALSE);
    name2 = dict_get_string(*(dict_T**)p2, "name", FALSE);
    जर (readdirex_sort == READDIR_SORT_BYTE)
	परत STRCMP(name1, name2);
    जर (readdirex_sort == READDIR_SORT_IC)
	परत STRICMP(name1, name2);

    परत STRCOLL(name1, name2);
}

    स्थिर पूर्णांक
compare_readdir_item(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2)
{
    जर (readdirex_sort == READDIR_SORT_BYTE)
	परत STRCMP(*(अक्षर **)s1, *(अक्षर **)s2);
    जर (readdirex_sort == READDIR_SORT_IC)
	परत STRICMP(*(अक्षर **)s1, *(अक्षर **)s2);

    परत STRCOLL(*(अक्षर **)s1, *(अक्षर **)s2);
}
#endif

#जर defined(TEMPDIRNAMES) || defined(FEAT_EVAL)
/*
 * Core part of "readdir()" and "readdirex()" function.
 * Retrieve the list of files/directories of "path" into "gap".
 * If "withattr" is TRUE, retrieve the names and their attributes.
 * If "withattr" is FALSE, retrieve the names only.
 * Return OK for success, FAIL for failure.
 */
    पूर्णांक
readdir_core(
    garray_T	*gap,
    char_u	*path,
    पूर्णांक		withattr UNUSED,
    शून्य	*context,
    पूर्णांक		(*checkitem)(शून्य *context, शून्य *item),
    पूर्णांक		sort)
{
    पूर्णांक			failed = FALSE;
    char_u		*p;
# ifdef MSWIN
    char_u		*buf;
    पूर्णांक			ok;
    HANDLE		hFind = INVALID_HANDLE_मूल्य;
    WIN32_FIND_DATAW    wfd;
    WCHAR		*wn = रिक्त;	// UTF-16 name, NULL when not used.
    char_u		*p_end;
# अन्यथा
    सूची			*dirp;
    संरचना dirent	*dp;
# endif

    ga_init2(gap, आकार(शून्य *), 20);

# ifdef FEAT_EVAL
#  define FREE_ITEM(item)   करा { \
	जर (withattr) \
	    dict_unref((dict_T*)(item)); \
	अन्यथा \
	    vim_free(item); \
    } पर्यंत (0)

    readdirex_sort = READDIR_SORT_BYTE;
# अन्यथा
#  define FREE_ITEM(item)   vim_free(item)
# endif

# ifdef MSWIN
    buf = alloc(MAXPATHL);
    जर (buf == रिक्त)
	परत FAIL;
    STRNCPY(buf, path, MAXPATHL-5);
    p = p_end = buf + STRLEN(buf);
    MB_PTR_BACK(buf, p);
    जर (*p == '\\' || *p == '/')
	p_end = p;
    STRCPY(p_end, "\\*");

    wn = enc_to_utf16(buf, रिक्त);
    जर (wn != रिक्त)
	hFind = FindFirstFileW(wn, &wfd);
    ok = (hFind != INVALID_HANDLE_मूल्य);
    जर (!ok)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    अन्यथा
    {
	पर्यंत (ok)
	{
	    पूर्णांक	    ignore;
	    शून्य    *item;
	    WCHAR   *wp;

	    wp = wfd.cFileName;
	    ignore = wp[0] == L'.' &&
		    (wp[1] == NUL ||
		     (wp[1] == L'.' && wp[2] == NUL));
	    जर (ignore)
	    {
		ok = FindNextFileW(hFind, &wfd);
		सुरूठेवा;
	    }
#  ifdef FEAT_EVAL
	    जर (withattr)
		item = (शून्य*)create_readdirex_item(&wfd);
	    अन्यथा
#  endif
		item = (शून्य*)utf16_to_enc(wfd.cFileName, रिक्त);
	    जर (item == रिक्त)
	    {
		failed = TRUE;
		थांबा;
	    }

	    जर (!ignore && checkitem != रिक्त)
	    {
		पूर्णांक r = checkitem(context, item);

		जर (r < 0)
		{
		    FREE_ITEM(item);
		    थांबा;
		}
		जर (r == 0)
		    ignore = TRUE;
	    }

	    जर (!ignore)
	    {
		जर (ga_grow(gap, 1) == OK)
		    ((शून्य**)gap->ga_data)[gap->ga_len++] = item;
		अन्यथा
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    थांबा;
		}
	    }
	    अन्यथा
		FREE_ITEM(item);

	    ok = FindNextFileW(hFind, &wfd);
	}
	FindClose(hFind);
    }

    vim_free(buf);
    vim_free(wn);
# अन्यथा	// MSWIN
    dirp = सूची_खोलो((अक्षर *)path);
    जर (dirp == रिक्त)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    अन्यथा
    {
	साठी (;;)
	{
	    पूर्णांक	    ignore;
	    शून्य    *item;

	    dp = सूची_पढ़ो(dirp);
	    जर (dp == रिक्त)
		थांबा;
	    p = (char_u *)dp->d_name;

	    ignore = p[0] == '.' &&
		    (p[1] == NUL ||
		     (p[1] == '.' && p[2] == NUL));
	    जर (ignore)
		सुरूठेवा;
#  ifdef FEAT_EVAL
	    जर (withattr)
		item = (शून्य*)create_readdirex_item(path, p);
	    अन्यथा
#  endif
		item = (शून्य*)vim_strsave(p);
	    जर (item == रिक्त)
	    {
		failed = TRUE;
		थांबा;
	    }

	    जर (checkitem != रिक्त)
	    {
		पूर्णांक r = checkitem(context, item);

		जर (r < 0)
		{
		    FREE_ITEM(item);
		    थांबा;
		}
		जर (r == 0)
		    ignore = TRUE;
	    }

	    जर (!ignore)
	    {
		जर (ga_grow(gap, 1) == OK)
		    ((शून्य**)gap->ga_data)[gap->ga_len++] = item;
		अन्यथा
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    थांबा;
		}
	    }
	    अन्यथा
		FREE_ITEM(item);
	}

	बंद_सूची(dirp);
    }
# endif	// MSWIN

# undef FREE_ITEM

    जर (!failed && gap->ga_len > 0 && sort > READDIR_SORT_NONE)
    {
# ifdef FEAT_EVAL
	readdirex_sort = sort;
	जर (withattr)
	    क्विक((शून्य*)gap->ga_data, (माप_प्रकार)gap->ga_len, आकार(dict_T*),
		    compare_readdirex_item);
	अन्यथा
	    क्विक((शून्य*)gap->ga_data, (माप_प्रकार)gap->ga_len, आकार(char_u *),
		    compare_readdir_item);
# अन्यथा
	    sort_strings((char_u **)gap->ga_data, gap->ga_len);
# endif
    }

    परत failed ? FAIL : OK;
}

/*
 * Delete "name" and everything in it, recursively.
 * return 0 for success, -1 if some file was not deleted.
 */
    पूर्णांक
delete_recursive(char_u *name)
{
    पूर्णांक result = 0;

    // A symbolic link to a directory itself is deleted, not the directory it
    // points to.
    जर (
# जर defined(UNIX) || defined(MSWIN)
	 mch_isrealdir(name)
# अन्यथा
	 mch_isdir(name)
# endif
	    )
    {
	char_u	    *exp = vim_strsave(name);
	garray_T    ga;

	जर (exp == रिक्त)
	    परत -1;
	जर (readdir_core(&ga, exp, FALSE, रिक्त, रिक्त, READDIR_SORT_NONE) == OK)
	{
	    पूर्णांक	len = vim_snprintf((अक्षर *)NameBuff, MAXPATHL, "%s/", exp);
	    पूर्णांक	i;

	    साठी (i = 0; i < ga.ga_len; ++i)
	    {
		vim_snprintf((अक्षर *)NameBuff + len, MAXPATHL - len, "%s", ((char_u **)ga.ga_data)[i]);
		जर (delete_recursive(NameBuff) != 0)
		    // Remember the failure but continue deleting any further
		    // entries.
		    result = -1;
	    }
	    ga_clear_strings(&ga);
	    जर (mch_rmdir(exp) != 0)
		result = -1;
	}
	अन्यथा
	    result = -1;
	vim_free(exp);
    }
    अन्यथा
	result = mch_remove(name) == 0 ? 0 : -1;

    परत result;
}
#endif

#जर defined(TEMPDIRNAMES)
स्थिर दीर्घ	temp_मोजणी = 0;		// Temp filename counter.

# जर defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
/*
 * Open temporary directory and take file lock to prevent
 * to be auto-cleaned.
 */
   स्थिर शून्य
vim_opentempdir(शून्य)
{
    सूची *dp = रिक्त;

    जर (vim_tempdir_dp != रिक्त)
	परत;

    dp = सूची_खोलो((स्थिरांक अक्षर*)vim_tempdir);
    जर (dp == रिक्त)
	परत;

    vim_tempdir_dp = dp;
    flock(dirfd(vim_tempdir_dp), LOCK_SH);
}

/*
 * Close temporary directory - it automatically release file lock.
 */
   स्थिर शून्य
vim_closetempdir(शून्य)
{
    जर (vim_tempdir_dp == रिक्त)
	परत;

    बंद_सूची(vim_tempdir_dp);
    vim_tempdir_dp = रिक्त;
}

/*
 * Return true if the temp directory we created is gone.
 */
    स्थिर bool
vim_tempdir_gone(शून्य)
{
    stat_T	st;

    जर (vim_tempdir_dp == रिक्त)
	परत false;
    परत ख_स्थिति(dirfd(vim_tempdir_dp), &st) < 0 || st.st_nlink == 0;
}
# endif

/*
 * Delete the temp directory and all files it contains.
 */
    शून्य
vim_deltempdir(शून्य)
{
    जर (vim_tempdir == रिक्त)
	परत;

# जर defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
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
    स्थिर शून्य
vim_settempdir(char_u *tempdir)
{
    char_u	*buf;
    माप_प्रकार	buflen;

    buf = alloc(MAXPATHL + 2);
    जर (buf == रिक्त)
	परत;

    जर (vim_FullName(tempdir, buf, MAXPATHL, FALSE) == FAIL)
	STRCPY(buf, tempdir);
    buflen = STRLEN(buf);
    जर (!after_pathsep(buf, buf + buflen))
    {
	STRCPY(buf + buflen, PATHSEPSTR);
	buflen += आकार(PATHSEP);
    }
    vim_tempdir = vim_strnsave(buf, buflen);
# जर defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
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
    पूर्णांक	    extra_char UNUSED,  // char to use in the name instead of '?'
    पूर्णांक	    keep UNUSED)
{
#ifdef USE_TMPNAM
    char_u	itmp[ब_क्षणिक];	// use tmpnam()
#किंवाजर defined(MSWIN)
    WCHAR	itmp[TEMPNAMELEN];
#अन्यथा
    char_u	itmp[TEMPNAMELEN];
#endif

#ifdef TEMPDIRNAMES
    स्थिर अक्षर	*(tempdirs[]) = {TEMPDIRNAMES};
    पूर्णांक		i;
# ifndef EEXIST
    stat_T	st;
# endif

# जर defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    // if the temp directory is gone, force re-creation of it
    जर (vim_tempdir != रिक्त && vim_tempdir_gone())
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
    जर (vim_tempdir == रिक्त)
    {
	/*
	 * Try the entries in TEMPDIRNAMES to create the temp directory.
	 */
	साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(tempdirs); ++i)
	{
# ifndef HAVE_MKDTEMP
	    दीर्घ	nr;
	    दीर्घ	off;
# endif
	    माप_प्रकार	itmplen;

	    // Expand $TMP, leave room for "/v1100000/999999999".
	    // Skip the directory check if the expansion fails.
	    itmplen = expand_env((char_u *)tempdirs[i], itmp, TEMPNAMELEN - 20);
	    जर (itmp[0] != '$' && mch_isdir(itmp))
	    {
		// directory exists
		जर (!after_pathsep(itmp, itmp + itmplen))
		{
		    STRCPY(itmp + itmplen, PATHSEPSTR);
		    itmplen += आकार(PATHSEP);
		}

# ifdef HAVE_MKDTEMP
		{
#  जर defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    mode_t	umask_save = umask(077);
#  endif
		    // Leave room for filename
		    STRCPY(itmp + itmplen, "vXXXXXX");
		    itmplen += STRLEN_LITERAL("vXXXXXX");
		    जर (mkdtemp((अक्षर *)itmp) != रिक्त)
			vim_settempdir(itmp);
#  जर defined(UNIX) || defined(VMS)
		    (शून्य)umask(umask_save);
#  endif
		}
# अन्यथा
		// Get an arbitrary number of up to 6 digits.  When it's
		// unlikely that it already exists it will be faster,
		// otherwise it doesn't matter.  The use of mkdir() avoids any
		// security problems because of the predictable number.
		nr = (mch_get_pid() + (दीर्घ)समय(रिक्त)) % 1000000L;

		// Try up to 10000 different values until we find a name that
		// doesn't exist.
		साठी (off = 0; off < 10000L; ++off)
		{
		    पूर्णांक		r;
#  जर defined(UNIX) || defined(VMS)
		    mode_t	umask_save;
#  endif

		    vim_snprintf((अक्षर *)itmp + itmplen, आकार(itmp) - itmplen, "v%ld", nr + off);
#  ifndef EEXIST
		    // If mkdir() does not set errno to EEXIST, check for
		    // existing file here.  There is a race condition then,
		    // although it's fail-safe.
		    जर (mch_stat((अक्षर *)itmp, &st) >= 0)
			सुरूठेवा;
#  endif
#  जर defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    umask_save = umask(077);
#  endif
		    r = vim_mkdir(itmp, 0700);
#  जर defined(UNIX) || defined(VMS)
		    (शून्य)umask(umask_save);
#  endif
		    जर (r == 0)
		    {
			vim_settempdir(itmp);
			थांबा;
		    }
#  ifdef EEXIST
		    // If the mkdir() didn't fail because the file/dir exists,
		    // we probably can't create any dir here, try another
		    // place.
		    जर (त्रुटि_सं != EEXIST)
#  endif
			थांबा;
		}
# endif // HAVE_MKDTEMP
		जर (vim_tempdir != रिक्त)
		    थांबा;
	    }
	}
    }

    जर (vim_tempdir != रिक्त)
    {
	// There is no need to check if the file exists, because we own the
	// directory and nobody else creates a file in it.
	पूर्णांक itmplen = vim_snprintf((अक्षर *)itmp, आकार(itmp), "%s%ld", vim_tempdir, temp_मोजणी++);
	परत vim_strnsave(itmp, (माप_प्रकार)itmplen);
    }

    परत रिक्त;

#अन्यथा // TEMPDIRNAMES
    char_u	*p;

# ifdef MSWIN
    WCHAR	wszTempFile[_MAX_PATH + 1];
    WCHAR	buf4[4];
    WCHAR	*chartab = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char_u	*retval;
    char_u	*shname;
    दीर्घ	i;

    wcscpy(itmp, L"");
    जर (GetTempPathW(_MAX_PATH, wszTempFile) == 0)
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
    जर (GetTempFileNameW(wszTempFile, buf4, 0, itmp) == 0)
	परत रिक्त;
    जर (!keep)
	// GetTempFileNameW() will create the file, we don't want that
	(शून्य)DeleteFileW(itmp);

    // Backslashes in a temp file name cause problems when filtering with
    // "sh".  NOTE: This also checks 'shellcmdflag' to help those people who
    // didn't set 'shellslash' but only if not using PowerShell.
    retval = utf16_to_enc(itmp, रिक्त);
    जर (retval == रिक्त)
	परत रिक्त;
    shname = gettail(p_sh);
    जर ((*p_shcf == '-' && !(म_माला((अक्षर *)shname, "powershell") != रिक्त
			     || म_माला((अक्षर *)shname, "pwsh") != रिक्त ))
								    || p_ssl)
	साठी (p = retval; *p; ++p)
	    जर (*p == '\\')
		*p = '/';
    परत retval;

# अन्यथा // MSWIN
    पूर्णांक		itmplen;

#  ifdef USE_TMPNAM
    // tmpnam() will make its own name
    p = क्षणिक((अक्षर *)itmp);
    जर (p == रिक्त || *p == NUL)
	परत रिक्त;
#  अन्यथा

#   ifdef VMS_TEMPNAM
    // mktemp() is not working on VMS.  It seems to be
    // a do-nothing function. Therefore we use tempnam().
    vim_snprintf((अक्षर *)itmp, आकार(itmp), "VIM%c", extra_char);
    p = (char_u *)tempnam("tmp:", (अक्षर *)itmp);
    जर (p != रिक्त)
    {
	// VMS will use '.LIS' if we don't explicitly specify an extension,
	// and VIM will then be unable to find the file later
	itmplen = vim_snprintf((अक्षर *)itmp, आकार(itmp), "%s.txt", p);
	मुक्त(p);
    }
    अन्यथा
	परत रिक्त;
#   अन्यथा
    STRCPY(itmp, TEMPNAME);
    itmplen = STRLEN_LITERAL(TEMPNAME);
    जर ((p = vim_strchr(itmp, '?')) != रिक्त)
	*p = extra_char;
    जर (mktemp((अक्षर *)itmp) == रिक्त)
	परत रिक्त;
#   endif
#  endif

    परत vim_strnsave(itmp, (माप_प्रकार)itmplen);
# endif // MSWIN
#endif // TEMPDIRNAMES
}

#जर defined(BACKSLASH_IN_FILENAME)
/*
 * Convert all backslashes in fname to forward slashes in-place, unless when
 * it looks like a URL.
 */
    शून्य
forward_slash(char_u *fname)
{
    char_u	*p;

    जर (path_with_url(fname))
	परत;
    साठी (p = fname; *p != NUL; ++p)
	// The Big5 encoding can have '\' in the trail byte.
	जर (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
	    ++p;
	अन्यथा जर (*p == '\\')
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
    पूर्णांक
match_file_pat(
    char_u	*pattern,		// pattern to match with
    regprog_T	**prog,			// pre-compiled regprog or NULL
    char_u	*fname,			// full path of file name
    char_u	*sfname,		// short file name or NULL
    char_u	*tail,			// tail of path
    पूर्णांक		allow_dirs)		// allow matching with dir
{
    regmatch_T	regmatch;
    पूर्णांक		result = FALSE;

    regmatch.rm_ic = p_fic; // ignore case if 'fileignorecase' is set
    जर (prog != रिक्त)
	regmatch.regprog = *prog;
    अन्यथा
	regmatch.regprog = vim_regcomp(pattern, RE_MAGIC);

    /*
     * Try for a match with the pattern with:
     * 1. the full file name, when the pattern has a '/'.
     * 2. the short file name, when the pattern has a '/'.
     * 3. the tail of the file name, when the pattern has no '/'.
     */
    जर (regmatch.regprog != रिक्त
	     && ((allow_dirs
		     && (vim_regexec(&regmatch, fname, (colnr_T)0)
			 || (sfname != रिक्त
			     && vim_regexec(&regmatch, sfname, (colnr_T)0))))
		 || (!allow_dirs && vim_regexec(&regmatch, tail, (colnr_T)0))))
	result = TRUE;

    जर (prog != रिक्त)
	*prog = regmatch.regprog;
    अन्यथा
	vim_regfree(regmatch.regprog);
    परत result;
}

/*
 * Return TRUE if a file matches with a pattern in "list".
 * "list" is a comma-separated list of patterns, like 'wildignore'.
 * "sfname" is the short file name or NULL, "ffname" the long file name.
 */
    पूर्णांक
match_file_list(char_u *list, char_u *sfname, char_u *ffname)
{
    char_u	buf[MAXPATHL];
    char_u	*tail;
    char_u	*regpat;
    अक्षर	allow_dirs;
    पूर्णांक		match;
    char_u	*p;

    tail = gettail(sfname);

    // try all patterns in 'wildignore'
    p = list;
    पर्यंत (*p)
    {
	copy_option_part(&p, buf, MAXPATHL, ",");
	regpat = file_pat_to_reg_pat(buf, रिक्त, &allow_dirs, FALSE);
	जर (regpat == रिक्त)
	    थांबा;
	match = match_file_pat(regpat, रिक्त, ffname, sfname,
						       tail, (पूर्णांक)allow_dirs);
	vim_free(regpat);
	जर (match)
	    परत TRUE;
    }
    परत FALSE;
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
    अक्षर	*allow_dirs,	// Result passed back out in here
    पूर्णांक		no_bslash UNUSED) // Don't use a backward slash as pathsep
{
    पूर्णांक		size = 2; // '^' at start, '$' at end
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    पूर्णांक		i;
    पूर्णांक		nested = 0;
    पूर्णांक		add_dollar = TRUE;

    जर (allow_dirs != रिक्त)
	*allow_dirs = FALSE;
    जर (pat_end == रिक्त)
	pat_end = pat + STRLEN(pat);

    साठी (p = pat; p < pat_end; p++)
    {
	निवड (*p)
	{
	    स्थिती '*':
	    स्थिती '.':
	    स्थिती ',':
	    स्थिती '{':
	    स्थिती '}':
	    स्थिती '~':
		size += 2;	// extra backslash
		थांबा;
#ifdef BACKSLASH_IN_FILENAME
	    स्थिती '\\':
	    स्थिती '/':
		size += 4;	// could become "[\/]"
		थांबा;
#endif
	    पूर्वनिर्धारित:
		size++;
		जर (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		{
		    ++p;
		    ++size;
		}
		थांबा;
	}
    }
    reg_pat = alloc(size + 1);
    जर (reg_pat == रिक्त)
	परत रिक्त;

    i = 0;

    जर (pat[0] == '*')
	पर्यंत (pat[0] == '*' && pat < pat_end - 1)
	    pat++;
    अन्यथा
	reg_pat[i++] = '^';
    endp = pat_end - 1;
    जर (endp >= pat && *endp == '*')
    {
	पर्यंत (endp - pat > 0 && *endp == '*')
	    endp--;
	add_dollar = FALSE;
    }
    साठी (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	निवड (*p)
	{
	    स्थिती '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		पर्यंत (p[1] == '*')	// "**" matches like "*"
		    ++p;
		थांबा;
	    स्थिती '.':
	    स्थिती '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		थांबा;
	    स्थिती '?':
		reg_pat[i++] = '.';
		थांबा;
	    स्थिती '\\':
		जर (p[1] == NUL)
		    थांबा;
#ifdef BACKSLASH_IN_FILENAME
		जर (!no_bslash)
		{
		    // translate:
		    // "\x" to "\\x"  e.g., "dir\file"
		    // "\*" to "\\.*" e.g., "dir\*.c"
		    // "\?" to "\\."  e.g., "dir\??.c"
		    // "\+" to "\+"   e.g., "fileX\+.c"
		    जर ((vim_isfilec(p[1]) || p[1] == '*' || p[1] == '?')
			    && p[1] != '+')
		    {
			reg_pat[i++] = '[';
			reg_pat[i++] = '\\';
			reg_pat[i++] = '/';
			reg_pat[i++] = ']';
			जर (allow_dirs != रिक्त)
			    *allow_dirs = TRUE;
			थांबा;
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
		जर (*++p == '?'
#ifdef BACKSLASH_IN_FILENAME
			&& no_bslash
#endif
			)
		    reg_pat[i++] = '?';
		अन्यथा
		    जर (*p == ',' || *p == '%' || *p == '#'
			       || vim_isspace(*p) || *p == '{' || *p == '}')
			reg_pat[i++] = *p;
		    अन्यथा जर (*p == '\\' && p[1] == '\\' && p[2] == '{')
		    {
			reg_pat[i++] = '\\';
			reg_pat[i++] = '{';
			p += 2;
		    }
		    अन्यथा
		    {
			जर (allow_dirs != रिक्त && vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
				&& (!no_bslash || *p != '\\')
#endif
				)
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			जर (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
			    reg_pat[i++] = *p++;
			reg_pat[i++] = *p;
		    }
		थांबा;
#ifdef BACKSLASH_IN_FILENAME
	    स्थिती '/':
		reg_pat[i++] = '[';
		reg_pat[i++] = '\\';
		reg_pat[i++] = '/';
		reg_pat[i++] = ']';
		जर (allow_dirs != रिक्त)
		    *allow_dirs = TRUE;
		थांबा;
#endif
	    स्थिती '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		थांबा;
	    स्थिती '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		थांबा;
	    स्थिती ',':
		जर (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		अन्यथा
		    reg_pat[i++] = ',';
		थांबा;
	    पूर्वनिर्धारित:
		जर (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		    reg_pat[i++] = *p++;
		अन्यथा जर (allow_dirs != रिक्त && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		थांबा;
	}
    }
    जर (add_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    जर (nested != 0)
    {
	जर (nested < 0)
	    emsg(_(e_missing_open_curly));
	अन्यथा
	    emsg(_(e_missing_close_curly));
	VIM_CLEAR(reg_pat);
    }
    परत reg_pat;
}

#जर defined(EINTR)
/*
 * Version of read() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    दीर्घ
read_eintr(पूर्णांक fd, शून्य *buf, माप_प्रकार bufsize)
{
    दीर्घ ret;

    साठी (;;)
    {
	ret = vim_read(fd, buf, bufsize);
	जर (ret >= 0 || त्रुटि_सं != EINTR)
	    थांबा;
    }
    परत ret;
}

/*
 * Version of write() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    दीर्घ
write_eintr(पूर्णांक fd, शून्य *buf, माप_प्रकार bufsize)
{
    दीर्घ    ret = 0;
    दीर्घ    wlen;

    // Repeat the write() so long it didn't fail, other than being interrupted
    // by a signal.
    पर्यंत (ret < (दीर्घ)bufsize)
    {
	wlen = vim_write(fd, (अक्षर *)buf + ret, bufsize - ret);
	जर (wlen < 0)
	{
	    जर (त्रुटि_सं != EINTR)
		थांबा;
	}
	अन्यथा
	    ret += wlen;
    }
    परत ret;
}
#endif
