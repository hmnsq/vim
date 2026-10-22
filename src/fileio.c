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

#यदि defined(__TANDEM)
# include <limits.h>		// for SSIZE_MAX
#endif
#यदि (defined(UNIX) || defined(VMS)) && defined(FEAT_EVAL)
# include <pwd.h>
# include <grp.h>
#endif
#यदि defined(VMS) && defined(HAVE_XOS_R_H)
# include <x11/xos_r.h>
#endif

// Is there any system that doesn't have access()?
#define USE_MCH_ACCESS

// Bitmask with 0x80 set in each byte of a long_u word, used to detect
// non-ASCII bytes (high bit set) in multiple bytes at once.
#define NONASCII_MASK (((long_u)-1 / 0xFF) * 0x80)

#यदि defined(__hpux) && !defined(HAVE_DIRFD)
# define dirfd(x) ((x)->__dd_fd)
# define HAVE_DIRFD
#endif

स्थिर char_u *next_fenc(char_u **pp, पूर्णाङ्क *alloced);
#ifdef FEAT_EVAL
स्थिर char_u *readfile_charconvert(char_u *fname, char_u *fenc, पूर्णाङ्क *fdp);
#endif
#ifdef FEAT_CRYPT
स्थिर char_u *check_for_cryptkey(char_u *cryptkey, char_u *ptr, दीर्घ *sizep, off_T *filesizep, पूर्णाङ्क newfile, char_u *fname, पूर्णाङ्क *did_ask);
#endif
स्थिर linenr_T readfile_linenr(linenr_T linecnt, char_u *p, char_u *endp);
स्थिर char_u *check_for_bom(char_u *p, दीर्घ size, पूर्णाङ्क *lenp, पूर्णाङ्क flags);

#ifdef FEAT_EVAL
स्थिर पूर्णाङ्क readdirex_sort;
#endif

    शून्य
filemess(
    buf_T	*buf,
    char_u	*name,
    char_u	*s,
    पूर्णाङ्क		attr)
{
    पूर्णाङ्क		संदेश_scroll_save;
    पूर्णाङ्क		prev_संदेश_col = संदेश_col;
    माप_प्रकार	len;

    यदि (संदेश_silent != 0)
	प्रत्यागम;
    संदेश_add_fname(buf, name);	    // put file name in IObuff with quotes

    // If it's extremely long, truncate it.
    len = STRLEN(IObuff);
    यदि (len > IOSIZE - 100)
    {
	len = IOSIZE - 100;
	IObuff[len] = NUL;
    }

    // Avoid an over-long translation to cause trouble.
    यदि (*s != NUL)
	STRNCPY(IObuff + len, s, 99);

    /*
     * For the first message may have to start a new line.
     * For further ones overwrite the previous one, reset msg_scroll before
     * calling filemess().
     */
    संदेश_scroll_save = संदेश_scroll;
    यदि (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	संदेश_scroll = FALSE;
    यदि (!संदेश_scroll)	// wait a bit when overwriting an error msg
	check_for_delay(FALSE);
    संदेश_start();
    यदि (prev_संदेश_col != 0 && संदेश_col == 0)
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
    पूर्णाङ्क
readfile(
    char_u	*fname,
    char_u	*sfname,
    linenr_T	from,
    linenr_T	पंक्तियाँ_to_skip,
    linenr_T	पंक्तियाँ_to_read,
    exarg_T	*eap,			// can be NULL!
    पूर्णाङ्क		flags)
{
    पूर्णाङ्क		retval = FAIL;	// jump to "theend" instead of returning
    पूर्णाङ्क		fd = 0;
    पूर्णाङ्क		newfile = (flags & READ_NEW);
    पूर्णाङ्क		check_readonly;
    पूर्णाङ्क		filtering = (flags & READ_FILTER);
    पूर्णाङ्क		read_stdin = (flags & READ_STDIN);
    पूर्णाङ्क		read_buffer = (flags & READ_BUFFER);
    पूर्णाङ्क		read_fifo = (flags & READ_FIFO);
    पूर्णाङ्क		set_options = newfile || read_buffer
					   || (eap != रिक्त && eap->read_edit);
    linenr_T	read_buf_lnum = 1;	// next line to read from curbuf
    colnr_T	read_buf_col = 0;	// next char to read from this line
    char_u	c;
    linenr_T	lnum = from;
    char_u	*ptr = रिक्त;		// pointer into read buffer
    char_u	*buffer = रिक्त;		// read buffer
    char_u	*new_buffer = रिक्त;	// init to shut up gcc
    char_u	*पंक्ति_start = रिक्त;	// init to shut up gcc
    पूर्णाङ्क		wasempty;		// buffer was empty before reading
    colnr_T	len;
    दीर्घ	size = 0;
    char_u	*p;
    off_T	filesize = 0;
    पूर्णाङ्क		skip_read = FALSE;
#ifdef FEAT_CRYPT
    off_T       filesize_disk = 0;      // file size read from disk
    off_T       filesize_गिनती = 0;     // counter
    char_u	*cryptkey = रिक्त;
    पूर्णाङ्क		did_ask_for_कुंजी = FALSE;
#endif
#ifdef FEAT_PERSISTENT_UNDO
    context_sha256_T sha_ctx;
    पूर्णाङ्क		read_undo_file = FALSE;
#endif
    पूर्णाङ्क		split = 0;		// number of split lines
#define UNKNOWN	 0x0fffffff		// file size is unknown
    linenr_T	linecnt;
    पूर्णाङ्क		error = FALSE;		// errors encountered
    पूर्णाङ्क		ff_error = EOL_UNKNOWN; // file format with errors
    दीर्घ	linerest = 0;		// remaining chars in line
#ifdef UNIX
    पूर्णाङ्क		perm = 0;
    पूर्णाङ्क		swap_mode = -1;		// protection bits for swap file
#अन्यथा
    पूर्णाङ्क		perm;
#endif
    पूर्णाङ्क		fileformat = 0;		// end-of-line format
    पूर्णाङ्क		keep_fileformat = FALSE;
    stat_T	st;
    पूर्णाङ्क		file_readonly;
    linenr_T	skip_गिनती = 0;
    linenr_T	read_गिनती = 0;
    पूर्णाङ्क		संदेश_save = संदेश_scroll;
    linenr_T	read_no_eol_lnum = 0;   // non-zero lnum when last line of
					// last read was missing the eol
    पूर्णाङ्क		try_mac;
    पूर्णाङ्क		try_dos;
    पूर्णाङ्क		try_unix;
    पूर्णाङ्क		file_rewind = FALSE;
    पूर्णाङ्क		can_retry;
    linenr_T	conv_error = 0;		// line nr with conversion error
    linenr_T	illegal_byte = 0;	// line nr with illegal byte
    पूर्णाङ्क		keep_dest_enc = FALSE;	// don't retry when char doesn't fit
					// in destination encoding
    पूर्णाङ्क		bad_char_behavior = BAD_REPLACE;
					// BAD_KEEP, BAD_DROP or character to
					// replace with
    char_u	*tmpname = रिक्त;	// name of 'charconvert' output file
    पूर्णाङ्क		fio_flags = 0;
    char_u	*fenc;			// fileencoding to use
    पूर्णाङ्क		fenc_alloced;		// fenc_next is in allocated memory
    char_u	*fenc_next = रिक्त;	// next item in 'fencs' or NULL
    पूर्णाङ्क		advance_fenc = FALSE;
    दीर्घ	real_size = 0;
#ifdef USE_ICONV
    iconv_t	iconv_fd = (iconv_t)-1;	// descriptor for iconv() or -1
# ifdef FEAT_EVAL
    पूर्णाङ्क		did_iconv = FALSE;	// TRUE when iconv() failed and trying
					// 'charconvert' next
# endif
#endif
    पूर्णाङ्क		converted = FALSE;	// TRUE if conversion done
    पूर्णाङ्क		notconverted = FALSE;	// TRUE if conversion wanted but it
					// wasn't possible
    char_u	conv_rest[CONV_RESTLEN];
    पूर्णाङ्क		conv_restlen = 0;	// nr of bytes in conv_rest[]
    pos_T	orig_start;
    buf_T	*old_curbuf;
    char_u	*old_b_ffname;
    char_u	*old_b_fname;
    पूर्णाङ्क		using_b_ffname;
    पूर्णाङ्क		using_b_fname;
    स्थिर वर्ण *संदेश_is_a_directory = N_("is a directory");
#ifdef FEAT_CRYPT
    पूर्णाङ्क		eof = FALSE;
#endif
#ifdef FEAT_SODIUM
    पूर्णाङ्क		may_need_lseek = FALSE;
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
    यदि (curbuf->b_ffname == रिक्त
	    && !filtering
	    && fname != रिक्त
	    && vim_strchr(p_cpo, CPO_FNAMER) != रिक्त
	    && !(flags & READ_DUMMY))
    {
	यदि (set_rw_fname(fname, sfname) == FAIL)
	    गच्छ theend;
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
    यदि (sfname == रिक्त)
	sfname = fname;
#यदि defined(UNIX)
    fname = sfname;
#endif

    /*
     * The BufReadCmd and FileReadCmd events intercept the reading process by
     * executing the associated commands instead.
     */
    यदि (!filtering && !read_stdin && !read_buffer)
    {
	orig_start = curbuf->b_op_start;

	// Set '[ mark to the line above where the lines go (line 1 if zero).
	curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
	curbuf->b_op_start.col = 0;

	यदि (newfile)
	{
	    यदि (apply_autocmds_exarg(EVENT_BUFREADCMD, रिक्त, sfname,
							  FALSE, curbuf, eap))
	    {
		retval = OK;
#ifdef FEAT_EVAL
		यदि (aborting())
		    retval = FAIL;
#endif
		// The BufReadCmd code usually uses ":read" to get the text and
		// perhaps ":file" to change the buffer name. But we should
		// consider this to work like ":edit", thus reset the
		// BF_NOTEDITED flag.  Then ":write" will work to overwrite the
		// same file.
		यदि (retval == OK)
		    curbuf->b_flags &= ~BF_NOTEDITED;
		गच्छ theend;
	    }
	}
	अन्यथा यदि (apply_autocmds_exarg(EVENT_FILEREADCMD, sfname, sfname,
							    FALSE, रिक्त, eap))
	{
#ifdef FEAT_EVAL
	    retval = aborting() ? FAIL : OK;
#अन्यथा
	    retval = OK;
#endif
	    गच्छ theend;
	}

	curbuf->b_op_start = orig_start;

	यदि (flags & READ_NOFILE)
	{
	    // Return NOTDONE instead of FAIL so that BufEnter can be triggered
	    // and other operations don't fail.
	    retval = NOTDONE;
	    गच्छ theend;
	}
    }

    यदि ((shortmess(SHM_OVER) || curbuf->b_help) && p_verbose == 0)
	संदेश_scroll = FALSE;	// overwrite previous file message
    अन्यथा
	संदेश_scroll = TRUE;	// don't overwrite previous file message

    यदि (fname != रिक्त && *fname != NUL)
    {
	fnamelen = STRLEN(fname);

	// If the name is too long we might crash further on, quit here.
	यदि (fnamelen >= MAXPATHL)
	{
	    filemess(curbuf, fname, (char_u *)_("Illegal file name"), 0);
	    संदेश_end();
	    संदेश_scroll = संदेश_save;
	    गच्छ theend;
	}

	// If the name ends in a path separator, we can't open it.  Check here,
	// because reading the file may actually work, but then creating the
	// swap file may destroy it!  Reported on MS-DOS and Win 95.
	यदि (after_pathsep(fname, fname + fnamelen))
	{
	    filemess(curbuf, fname, (char_u *)_(संदेश_is_a_directory), 0);
	    संदेश_end();
	    संदेश_scroll = संदेश_save;
	    retval = NOTDONE;
	    गच्छ theend;
	}
    }
#यदि defined(UNIX) || defined(VMS)
    यदि (!read_stdin && fname != रिक्त)
	perm = mch_getperm(fname);
#endif

    यदि (!read_stdin && !read_buffer && !read_fifo)
    {
#यदि defined(UNIX) || defined(VMS)
	यदि (perm >= 0 && !S_ISREG(perm)		    // not a regular file ...
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
	    यदि (S_ISDIR(perm))
	    {
		filemess(curbuf, fname, (char_u *)_(संदेश_is_a_directory), 0);
		retval = NOTDONE;
	    }
	    अन्यथा
		filemess(curbuf, fname, (char_u *)_("is not a file"), 0);
	    संदेश_end();
	    संदेश_scroll = संदेश_save;
	    गच्छ theend;
	}
#endif
#यदि defined(MSWIN)
	/*
	 * MS-Windows allows opening a device, but we will probably get stuck
	 * trying to read it.
	 */
	यदि (!p_odev && mch_nodetype(fname) == गाँठ_WRITABLE)
	{
	    filemess(curbuf, fname, (char_u *)_("is a device (disabled with 'opendevice' option)"), 0);
	    संदेश_end();
	    संदेश_scroll = संदेश_save;
	    गच्छ theend;
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
    यदि (check_readonly && !readonlymode)
	curbuf->b_p_ro = FALSE;

    यदि (newfile && !read_stdin && !read_buffer && !read_fifo)
    {
	// Remember time of file.
	यदि (mch_stat((वर्ण *)fname, &st) >= 0)
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
    यदि (read_stdin)
    {
#यदि defined(MSWIN)
	// Force binary I/O on stdin to avoid CR-LF -> LF conversion.
	setmode(0, O_द्विआधारी);
#endif
    }
    अन्यथा यदि (!read_buffer)
    {
#ifdef USE_MCH_ACCESS
	यदि (
# ifdef UNIX
	    !(perm & 0222) ||
# endif
				mch_access((वर्ण *)fname, W_OK))
	    file_readonly = TRUE;
	fd = mch_open((वर्ण *)fname, O_RDONLY | O_EXTRA, 0);
#अन्यथा
	यदि (!newfile
		|| readonlymode
		|| (fd = mch_open((वर्ण *)fname, O_RDWR | O_EXTRA, 0)) < 0)
	{
	    file_readonly = TRUE;
	    // try to open ro
	    fd = mch_open((वर्ण *)fname, O_RDONLY | O_EXTRA, 0);
	}
#endif
    }

    यदि (fd < 0)			    // cannot open at all
    {
#ifndef UNIX
	पूर्णाङ्क	isdir_f;
#endif
	संदेश_scroll = संदेश_save;
#ifndef UNIX
	/*
	 * On Amiga we can't open a directory, check here.
	 */
	isdir_f = (mch_isdir(fname));
	perm = mch_getperm(fname);  // check if the file exists
	यदि (isdir_f)
	{
	    filemess(curbuf, sfname, (char_u *)_(संदेश_is_a_directory), 0);
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	}
	अन्यथा
#endif
	    यदि (newfile)
	    {
		यदि (perm < 0
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
		    यदि (!bt_dontwrite(curbuf))
		    {
			check_need_swap(newfile);
			// SwapExists autocommand may mess things up
			यदि (curbuf != old_curbuf
				|| (using_b_ffname
					&& (old_b_ffname != curbuf->b_ffname))
				|| (using_b_fname
					 && (old_b_fname != curbuf->b_fname)))
			{
			    emsg(_(e_autocommands_changed_buffer_or_buffer_name));
			    गच्छ theend;
			}
		    }
		    यदि (dir_of_file_exists(fname))
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
		    यदि (eap != रिक्त)
			set_forced_fenc(eap);
		    apply_autocmds_exarg(EVENT_BUFNEWFILE, sfname, sfname,
							  FALSE, curbuf, eap);
		    // remember the current fileformat
		    save_file_ff(curbuf);

#यदि defined(FEAT_EVAL)
		    यदि (!aborting())   // autocmds may abort script processing
#endif
			retval = OK;	    // a new file is not an error
		    गच्छ theend;
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

	गच्छ theend;
    }

    /*
     * Only set the 'ro' flag for readonly files the first time they are
     * loaded.	Help files always get readonly mode
     */
    यदि ((check_readonly && file_readonly) || curbuf->b_help)
	curbuf->b_p_ro = TRUE;

    यदि (set_options)
    {
	// Don't change 'eol' if reading from buffer as it will already be
	// correctly set when reading stdin.
	यदि (!read_buffer)
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
    यदि (!bt_dontwrite(curbuf))
    {
	check_need_swap(newfile);
	यदि (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))))
	{
	    emsg(_(e_autocommands_changed_buffer_or_buffer_name));
	    यदि (!read_buffer)
		बंद(fd);
	    गच्छ theend;
	}
#ifdef UNIX
	// Set swap file protection bits after creating it.
	यदि (swap_mode > 0 && curbuf->b_ml.ml_mfp != रिक्त
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
	    यदि ((swap_mode & 044) == 040)
	    {
		stat_T	swap_st;

		यदि (mch_stat((वर्ण *)swap_fname, &swap_st) >= 0
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
    यदि (swap_exists_action == SEA_QUIT)
    {
	यदि (!read_buffer && !read_stdin)
	    बंद(fd);
	गच्छ theend;
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

    यदि (!read_buffer)
    {
	पूर्णाङ्क	m = संदेश_scroll;
	पूर्णाङ्क	n = संदेश_scrolled;

	/*
	 * The file must be closed again, the autocommands may want to change
	 * the file before reading it.
	 */
	यदि (!read_stdin)
	    बंद(fd);		// ignore errors

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	संदेश_scroll = TRUE;
	यदि (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPRE, रिक्त, sfname,
							  FALSE, curbuf, eap);
	अन्यथा यदि (read_stdin)
	    apply_autocmds_exarg(EVENT_STDINREADPRE, रिक्त, sfname,
							  FALSE, curbuf, eap);
	अन्यथा यदि (newfile)
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

	यदि (संदेश_scrolled == n)
	    संदेश_scroll = m;

#ifdef FEAT_EVAL
	यदि (aborting())	    // autocmds may abort script processing
	{
	    --no_wait_return;
	    संदेश_scroll = संदेश_save;
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    गच्छ theend;
	}
#endif
	/*
	 * Don't allow the autocommands to change the current buffer.
	 * Try to re-open the file.
	 *
	 * Don't allow the autocommands to change the buffer name either
	 * (cd for example) if it invalidates fname or sfname.
	 */
	यदि (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))
		|| (fd = mch_open((वर्ण *)fname, O_RDONLY | O_EXTRA, 0)) < 0))
	{
	    --no_wait_return;
	    संदेश_scroll = संदेश_save;
	    यदि (fd < 0)
		emsg(_(e_readpre_autocommands_made_file_unreadable));
	    अन्यथा
		emsg(_(e_readpre_autocommands_must_not_change_current_buffer));
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    गच्छ theend;
	}
    }

    // Autocommands may add lines to the file, need to check if it is empty
    wasempty = (curbuf->b_ml.ml_flags & ML_EMPTY);

    यदि (!recoverymode && !filtering && !(flags & READ_DUMMY))
    {
	/*
	 * Show the user that we are busy reading the input.  Sometimes this
	 * may take a while.  When reading from stdin another program may
	 * still be running, don't move the cursor to the last line, unless
	 * always using the GUI.
	 */
	यदि (read_stdin)
	{
	    यदि (!is_not_a_term())
	    {
#ifndef ALWAYS_USE_GUI
# ifdef VIMDLL
		यदि (!gui.in_use)
# endif
		    mch_संदेश(_("Vim: Reading from stdin...\n"));
#endif
#ifdef FEAT_GUI
		// Also write a message in the GUI window, if there is one.
		यदि (gui.in_use && !gui.dying && !gui.starting)
		{
		    माप_प्रकार  plen = STRLEN(_("Reading from stdin..."));

		    // make a copy, gui_write() may try to change it
		    p = vim_strnsave((char_u *)_("Reading from stdin..."), plen);
		    यदि (p != रिक्त)
		    {
			gui_write(p, (पूर्णाङ्क)plen);
			vim_free(p);
		    }
		}
#endif
	    }
	}
	अन्यथा यदि (!read_buffer)
	    filemess(curbuf, sfname, (char_u *)"", 0);
    }

    संदेश_scroll = FALSE;			// overwrite the file message

    /*
     * Set linecnt now, before the "retry" caused by a wrong guess for
     * fileformat, and after the autocommands, which may change them.
     */
    linecnt = curbuf->b_ml.ml_पंक्ति_गिनती;

    // "++bad=" argument.
    यदि (eap != रिक्त && eap->bad_char != 0)
    {
	bad_char_behavior = eap->bad_char;
	यदि (set_options)
	    curbuf->b_bad_char = eap->bad_char;
    }
    अन्यथा
	curbuf->b_bad_char = 0;

    /*
     * Decide which 'encoding' to use or use first.
     */
    यदि (eap != रिक्त && eap->force_enc != 0)
    {
	fenc = enc_canonize(eap->cmd + eap->force_enc);
	fenc_alloced = TRUE;
	keep_dest_enc = TRUE;
    }
    अन्यथा यदि (curbuf->b_p_bin)
    {
	fenc = (char_u *)"";		// binary: don't convert
	fenc_alloced = FALSE;
    }
    अन्यथा यदि (curbuf->b_help)
    {
	char_u	    firstline[80];
	पूर्णाङ्क	    fc;

	// Help files are either utf-8 or latin1.  Try utf-8 first, if this
	// fails it must be latin1.
	// Always do this when 'encoding' is "utf-8".  Otherwise only do
	// this when needed to avoid [converted] remarks all the time.
	// It is needed when the first line contains non-ASCII characters.
	// That is only in *.??x files.
	fenc = (char_u *)"latin1";
	c = enc_utf8;
	यदि (!c && !read_stdin)
	{
	    fc = fname[fnamelen - 1];
	    यदि (TOLOWER_ASC(fc) == 'x')
	    {
		// Read the first line (and a bit more).  Immediately rewind to
		// the start of the file.  If the read() fails "len" is -1.
		len = read_eintr(fd, firstline, 80);
		vim_lseek(fd, (off_T)0L, शुरू_से);
		प्रति (p = firstline; p < firstline + len; ++p)
		    यदि (*p >= 0x80)
		    {
			c = TRUE;
			भङ्ग;
		    }
	    }
	}

	यदि (c)
	{
	    fenc_next = fenc;
	    fenc = (char_u *)"utf-8";

	    // When the file is utf-8 but a character doesn't fit in
	    // 'encoding' don't retry.  In help text editing utf-8 bytes
	    // doesn't make sense.
	    यदि (!enc_utf8)
		keep_dest_enc = TRUE;
	}
	fenc_alloced = FALSE;
    }
    अन्यथा यदि (*p_fencs == NUL)
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

    यदि (file_rewind)
    {
	यदि (read_buffer)
	{
	    read_buf_lnum = 1;
	    read_buf_col = 0;
	}
	अन्यथा यदि (read_stdin || vim_lseek(fd, (off_T)0L, शुरू_से) != 0)
	{
	    // Can't rewind the file, give up.
	    error = TRUE;
	    गच्छ failed;
	}
	// Delete the previously read lines.
	यावत् (lnum > from)
	    ml_delete(lnum--);
	file_rewind = FALSE;
	यदि (set_options)
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
    यदि (keep_fileformat)
	keep_fileformat = FALSE;
    अन्यथा
    {
	यदि (eap != रिक्त && eap->force_ff != 0)
	{
	    fileformat = get_fileformat_force(curbuf, eap);
	    try_unix = try_dos = try_mac = FALSE;
	}
	अन्यथा यदि (curbuf->b_p_bin)
	    fileformat = EOL_UNIX;		// binary: use Unix format
	अन्यथा यदि (*p_ffs == NUL)
	    fileformat = get_fileformat(curbuf);// use format from buffer
	अन्यथा
	    fileformat = EOL_UNKNOWN;		// detect from file
    }

#ifdef USE_ICONV
    यदि (iconv_fd != (iconv_t)-1)
    {
	// aborted conversion with iconv(), close the descriptor
	iconv_close(iconv_fd);
	iconv_fd = (iconv_t)-1;
    }
#endif

    यदि (advance_fenc)
    {
	/*
	 * Try the next entry in 'fileencodings'.
	 */
	advance_fenc = FALSE;

	यदि (eap != रिक्त && eap->force_enc != 0)
	{
	    // Conversion given with "++cc=" wasn't possible, read
	    // without conversion.
	    notconverted = TRUE;
	    conv_error = 0;
	    यदि (fenc_alloced)
		vim_free(fenc);
	    fenc = (char_u *)"";
	    fenc_alloced = FALSE;
	}
	अन्यथा
	{
	    यदि (fenc_alloced)
		vim_free(fenc);
	    यदि (fenc_next != रिक्त)
	    {
		fenc = next_fenc(&fenc_next, &fenc_alloced);
	    }
	    अन्यथा
	    {
		fenc = (char_u *)"";
		fenc_alloced = FALSE;
	    }
	}
	यदि (tmpname != रिक्त)
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
    यदि (converted)
    {
	// "ucs-bom" means we need to check the first bytes of the file
	// for a BOM.
	यदि (STRCMP(fenc, ENC_UCSBOM) == 0)
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
	अन्यथा यदि (enc_utf8 || STRCMP(p_enc, "latin1") == 0)
	    fio_flags = get_fio_flags(fenc);

#ifdef MSWIN
	/*
	 * Conversion from an MS-Windows codepage to UTF-8 or another codepage
	 * is handled with MultiByteToWideChar().
	 */
	यदि (fio_flags == 0)
	    fio_flags = get_win_fio_flags(fenc);
#endif

#ifdef MACOS_CONVERT
	// Conversion from Apple MacRoman to latin1 or UTF-8
	यदि (fio_flags == 0)
	    fio_flags = get_mac_fio_flags(fenc);
#endif

#ifdef USE_ICONV
	/*
	 * Try using iconv() if we can't convert internally.
	 */
	यदि (fio_flags == 0
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
	यदि (fio_flags == 0 && !read_stdin && !read_buffer && *p_ccv != NUL
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
	    यदि (tmpname == रिक्त)
	    {
		tmpname = readfile_charconvert(fname, fenc, &fd);
		यदि (tmpname == रिक्त)
		{
		    // Conversion failed.  Try another one.
		    advance_fenc = TRUE;
		    यदि (fd < 0)
		    {
			// Re-opening the original file failed!
			emsg(_(e_conversion_mad_file_unreadable));
			error = TRUE;
			गच्छ failed;
		    }
		    गच्छ retry;
		}
	    }
	}
	अन्यथा
#endif
	{
	    यदि (fio_flags == 0
#ifdef USE_ICONV
		    && iconv_fd == (iconv_t)-1
#endif
	       )
	    {
		// Conversion wanted but we can't.
		// Try the next conversion in 'fileencodings'
		advance_fenc = TRUE;
		गच्छ retry;
	    }
	}
    }

    // Set "can_retry" when it's possible to rewind the file and try with
    // another "fenc" value.  It's FALSE when no other "fenc" to try, reading
    // stdin or fixed at a specific encoding.
    can_retry = (*fenc != NUL && !read_stdin && !read_fifo && !keep_dest_enc);

    यदि (!skip_read)
    {
	linerest = 0;
	filesize = 0;
#ifdef FEAT_CRYPT
	filesize_गिनती = 0;
#endif
	skip_गिनती = पंक्तियाँ_to_skip;
	read_गिनती = पंक्तियाँ_to_read;
	conv_restlen = 0;
#ifdef FEAT_PERSISTENT_UNDO
	read_undo_file = (newfile && (flags & READ_KEEP_UNDO) == 0
				  && curbuf->b_ffname != रिक्त
				  && curbuf->b_p_udf
				  && !filtering
				  && !read_fifo
				  && !read_stdin
				  && !read_buffer);
	यदि (read_undo_file)
	    sha256_start(&sha_ctx);
#endif
#ifdef FEAT_CRYPT
	यदि (curbuf->b_cryptstate != रिक्त)
	{
	    // Need to free the state, but keep the key, don't want to ask for
	    // it again.
	    crypt_free_state(curbuf->b_cryptstate);
	    curbuf->b_cryptstate = रिक्त;
	}
#endif
    }

    यावत् (!error && !got_int)
    {
	/*
	 * We allocate as much space for the file as we can get, plus
	 * space for the old line plus room for one terminating NUL.
	 * The amount is limited by the fact that read() only can read
	 * up to max_unsigned characters (and other things).
	 */
	यदि (!skip_read)
	{
#यदि defined(SSIZE_MAX) && (SSIZE_MAX < 0x10000L)
		size = SSIZE_MAX;		    // use max I/O size, 52K
#अन्यथा
		// Use buffer >= 64K.  Add linerest to double the size if the
		// line gets very long, to avoid a lot of copying. But don't
		// read more than 1 Mbyte at a time, so we can be interrupted.
		size = 0x10000L + linerest;
		यदि (size > 0x100000L)
		    size = 0x100000L;
#endif
	}

	// Protect against the argument of lalloc() going negative.
	// Also split lines that are too long for colnr_T.  After this check
	// passes, we read up to 'size' more bytes.  We must ensure that even
	// after that read, the line length won't exceed MAXCOL - 1 (because
	// we add 1 for the NUL when casting to colnr_T).  If this check fires,
	// we insert a synthetic newline immediately, so linerest doesn't grow.
	यदि (size < 0 || size + linerest + 1 < 0 || linerest >= MAXCOL - size)
	{
	    ++split;
	    *ptr = NL;		    // split line by inserting a NL
	    size = 1;
	}
	अन्यथा
	{
	    यदि (!skip_read)
	    {
		प्रति ( ; size >= 10; size = (दीर्घ)((long_u)size >> 1))
		{
		    यदि ((new_buffer = lalloc(size + linerest + 1,
							      FALSE)) != रिक्त)
			भङ्ग;
		}
		यदि (new_buffer == रिक्त)
		{
		    do_outofmem_संदेश((long_u)(size * 2 + linerest + 1));
		    error = TRUE;
		    भङ्ग;
		}
		यदि (linerest)	// copy characters from the previous buffer
		    mch_memmove(new_buffer, ptr - linerest, (माप_प्रकार)linerest);
		vim_free(buffer);
		buffer = new_buffer;
		ptr = buffer + linerest;
		पंक्ति_start = buffer;

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
		real_size = (पूर्णाङ्क)size;
#ifdef USE_ICONV
		यदि (iconv_fd != (iconv_t)-1)
		    size = size / ICONV_MULT;
		अन्यथा
#endif
		    यदि (fio_flags & FIO_LATIN1)
		    size = size / 2;
		अन्यथा यदि (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    size = (size * 2 / 3) & ~1;
		अन्यथा यदि (fio_flags & FIO_UCS4)
		    size = (size * 2 / 3) & ~3;
		अन्यथा यदि (fio_flags == FIO_UCSBOM)
		    size = size / ICONV_MULT;	// worst case
#ifdef MSWIN
		अन्यथा यदि (fio_flags & FIO_CODEPAGE)
		    size = size / ICONV_MULT;	// also worst case
#endif
#ifdef MACOS_CONVERT
		अन्यथा यदि (fio_flags & FIO_MACROMAN)
		    size = size / ICONV_MULT;	// also worst case
#endif

		यदि (conv_restlen > 0)
		{
		    // Insert unconverted bytes from previous line.
		    mch_memmove(ptr, conv_rest, conv_restlen);
		    ptr += conv_restlen;
		    size -= conv_restlen;
		}

		यदि (read_buffer)
		{
		    /*
		     * Read bytes from curbuf.  Used for converting text read
		     * from stdin.
		     */
		    यदि (read_buf_lnum > from)
			size = 0;
		    अन्यथा
		    {
			पूर्णाङ्क	n, ni;
			दीर्घ	tlen;

			tlen = 0;
			प्रति (;;)
			{
			    p = ml_get(read_buf_lnum) + read_buf_col;
			    n = ml_get_len(read_buf_lnum) - read_buf_col;
			    यदि ((पूर्णाङ्क)tlen + n + 1 > size)
			    {
				// Filled up to "size", append partial line.
				// Change NL to NUL to reverse the effect done
				// below.
				n = (पूर्णाङ्क)(size - tlen);
				प्रति (ni = 0; ni < n; ++ni)
				{
				    यदि (p[ni] == NL)
					ptr[tlen++] = NUL;
				    अन्यथा
					ptr[tlen++] = p[ni];
				}
				read_buf_col += n;
				भङ्ग;
			    }

			    // Append whole line and new-line.  Change NL
			    // to NUL to reverse the effect done below.
			    प्रति (ni = 0; ni < n; ++ni)
			    {
				यदि (p[ni] == NL)
				    ptr[tlen++] = NUL;
				अन्यथा
				    ptr[tlen++] = p[ni];
			    }
			    ptr[tlen++] = NL;
			    read_buf_col = 0;
			    यदि (++read_buf_lnum > from)
			    {
				// When the last line didn't have an
				// end-of-line don't add it now either.
				यदि (!curbuf->b_p_eol)
				    --tlen;
				size = tlen;
#ifdef FEAT_CRYPT
				eof = TRUE;
#endif
				भङ्ग;
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
		    यदि (filesize == 0)
		    {
			// set size to 8K + Sodium Crypt Metadata
			size = WRITEBUFSIZE + crypt_get_max_header_len()
			    + crypto_secretstream_xchacha20poly1305_HEADERBYTES
				+ crypto_secretstream_xchacha20poly1305_ABYTES;
			may_need_lseek = TRUE;
		    }

		    अन्यथा यदि (filesize > 0 && (curbuf->b_cryptstate != रिक्त
				&& crypt_method_is_sodium(
					     curbuf->b_cryptstate->method_nr)))
		    {
			size = WRITEBUFSIZE + crypto_secretstream_xchacha20poly1305_ABYTES;
			// need to rewind by - add_len from CRYPT_M_SOD2 (see
			// description above)
			यदि (curbuf->b_cryptstate->method_nr == CRYPT_M_SOD
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
		    filesize_गिनती += size;
		    eof = (size < read_size || filesize_गिनती == filesize_disk);
#endif
		}

#ifdef FEAT_CRYPT
		/*
		 * At start of file: Check for magic number of encryption.
		 */
		यदि (filesize == 0 && size > 0)
		{
		    cryptkey = check_for_cryptkey(cryptkey, ptr, &size,
						  &filesize, newfile, sfname,
						  &did_ask_for_कुंजी);
# यदि defined(CRYPT_NOT_INPLACE) && defined(FEAT_PERSISTENT_UNDO)
		    यदि (curbuf->b_cryptstate != रिक्त
				 && !crypt_works_inplace(curbuf->b_cryptstate))
			// reading undo file requires crypt_decode_inplace()
			read_undo_file = FALSE;
# endif
		}
		/*
		 * Decrypt the read bytes.  This is done before checking for
		 * EOF because the crypt layer may be buffering.
		 */
		यदि (cryptkey != रिक्त && curbuf->b_cryptstate != रिक्त
								   && size > 0)
		{
# ifdef CRYPT_NOT_INPLACE
		    यदि (crypt_works_inplace(curbuf->b_cryptstate))
		    {
# endif
			crypt_decode_inplace(curbuf->b_cryptstate, ptr,
								    size, eof);
# ifdef CRYPT_NOT_INPLACE
		    }
		    अन्यथा
		    {
			char_u	*newptr = रिक्त;
			पूर्णाङ्क	decrypted_size;

			decrypted_size = crypt_decode_alloc(
				    curbuf->b_cryptstate, ptr, size,
								 &newptr, eof);

			यदि (decrypted_size < 0)
			{
			    // error message already given
			    error = TRUE;
			    vim_free(newptr);
			    भङ्ग;
			}
			// If the crypt layer is buffering, not producing
			// anything yet, need to read more.
			यदि (decrypted_size == 0)
			    अनुवर्त;

			यदि (linerest == 0)
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
			    यदि (new_buffer == रिक्त)
			    {
				do_outofmem_संदेश(new_size);
				error = TRUE;
				भङ्ग;
			    }

			    mch_memmove(new_buffer, buffer, linerest + conv_restlen);
			    यदि (newptr != रिक्त)
				mch_memmove(new_buffer + linerest + conv_restlen,
					newptr, decrypted_size);
			    vim_free(newptr);
			}

			यदि (new_buffer != रिक्त)
			{
			    vim_free(buffer);
			    buffer = new_buffer;
			    new_buffer = रिक्त;
			    पंक्ति_start = buffer;
			    ptr = buffer + linerest + conv_restlen;
			    real_size = size;
			}
			size = decrypted_size;
		    }
# endif
		}
#endif

		यदि (size <= 0)
		{
		    यदि (size < 0)		    // read error
			error = TRUE;
		    अन्यथा यदि (conv_restlen > 0)
		    {
			/*
			 * Reached end-of-file but some trailing bytes could
			 * not be converted.  Truncated file?
			 */

			// When we did a conversion report an error.
			यदि (fio_flags != 0
#ifdef USE_ICONV
				|| iconv_fd != (iconv_t)-1
#endif
			   )
			{
			    यदि (can_retry)
				गच्छ rewind_retry;
			    यदि (conv_error == 0)
				conv_error = curbuf->b_ml.ml_पंक्ति_गिनती
								- linecnt + 1;
			}
			// Remember the first linenr with an illegal byte
			अन्यथा यदि (illegal_byte == 0)
			    illegal_byte = curbuf->b_ml.ml_पंक्ति_गिनती
								- linecnt + 1;
			यदि (bad_char_behavior == BAD_DROP)
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
			    यदि (bad_char_behavior != BAD_KEEP && (fio_flags != 0
#ifdef USE_ICONV
				    || iconv_fd != (iconv_t)-1
#endif
			       ))
			    {
				यावत् (conv_restlen > 0)
				{
				    *(--ptr) = bad_char_behavior;
				    --conv_restlen;
				}
			    }
			    fio_flags = 0;	// don't convert this
#ifdef USE_ICONV
			    यदि (iconv_fd != (iconv_t)-1)
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
	    यदि ((filesize == 0
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
		पूर्णाङ्क	blen;

		// no BOM detection in a short file or in binary mode
		यदि (size < 2 || curbuf->b_p_bin)
		    ccname = रिक्त;
		अन्यथा
		    ccname = check_for_bom(ptr, size, &blen,
		      fio_flags == FIO_UCSBOM ? FIO_ALL : get_fio_flags(fenc));
		यदि (ccname != रिक्त)
		{
		    // Remove BOM from the text
		    filesize += blen;
		    size -= blen;
		    mch_memmove(ptr, ptr + blen, (माप_प्रकार)size);
		    यदि (set_options)
		    {
			curbuf->b_p_bomb = TRUE;
			curbuf->b_start_bomb = TRUE;
		    }
		}

		यदि (fio_flags == FIO_UCSBOM)
		{
		    यदि (ccname == रिक्त)
		    {
			// No BOM detected: retry with next encoding.
			advance_fenc = TRUE;
		    }
		    अन्यथा
		    {
			// BOM detected: set "fenc" and jump back
			यदि (fenc_alloced)
			    vim_free(fenc);
			fenc = ccname;
			fenc_alloced = FALSE;
		    }
		    // retry reading without getting new bytes or rewinding
		    skip_read = TRUE;
		    गच्छ retry;
		}
	    }

	    // Include not converted bytes.
	    ptr -= conv_restlen;
	    size += conv_restlen;
	    conv_restlen = 0;
	    /*
	     * Break here for a read error or end-of-file.
	     */
	    यदि (size <= 0)
		भङ्ग;


#ifdef USE_ICONV
	    यदि (iconv_fd != (iconv_t)-1)
	    {
		/*
		 * Attempt conversion of the read bytes to 'encoding' using
		 * iconv().
		 */
		ध्रुव वर्ण	*fromp;
		वर्ण		*शीर्ष;
		माप_प्रकार		from_size;
		माप_प्रकार		to_size;

		fromp = (वर्ण *)ptr;
		from_size = size;
		ptr += size;
		शीर्ष = (वर्ण *)ptr;
		to_size = real_size - size;

		/*
		 * If there is conversion error or not enough room try using
		 * another conversion.  Except for when there is no
		 * alternative (help files).
		 */
		यावत् ((iconv(iconv_fd, (शून्य *)&fromp, &from_size,
							       &शीर्ष, &to_size)
			    == (माप_प्रकार)-1 && ICONV_ERRNO != ICONV_EINVAL)
						  || from_size > CONV_RESTLEN)
		{
		    यदि (can_retry)
			गच्छ rewind_retry;
		    यदि (conv_error == 0)
			conv_error = readfile_linenr(linecnt,
							  ptr, (char_u *)शीर्ष);

		    // Deal with a bad byte and continue with the next.
		    ++fromp;
		    --from_size;
		    यदि (bad_char_behavior == BAD_KEEP)
		    {
			*शीर्ष++ = *(fromp - 1);
			--to_size;
		    }
		    अन्यथा यदि (bad_char_behavior != BAD_DROP)
		    {
			*शीर्ष++ = bad_char_behavior;
			--to_size;
		    }
		}

		यदि (from_size > 0)
		{
		    // Some remaining characters, keep them for the next
		    // round.
		    mch_memmove(conv_rest, (char_u *)fromp, from_size);
		    conv_restlen = (पूर्णाङ्क)from_size;
		}

		// move the linerest to before the converted characters
		पंक्ति_start = ptr - linerest;
		mch_memmove(पंक्ति_start, buffer, (माप_प्रकार)linerest);
		size = (दीर्घ)((char_u *)शीर्ष - ptr);
	    }
#endif

#ifdef MSWIN
	    यदि (fio_flags & FIO_CODEPAGE)
	    {
		char_u	*src, *dst;
		WCHAR	ucs2buf[3];
		पूर्णाङ्क	ucs2len;
		पूर्णाङ्क	codepage = FIO_GET_CP(fio_flags);
		पूर्णाङ्क	bytelen;
		पूर्णाङ्क	found_bad;
		वर्ण	replstr[2];

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
		यदि (bad_char_behavior > 0)
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
		यावत् (size > 0)
		{
		    found_bad = FALSE;

# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
		    यदि (codepage == CP_UTF8)
		    {
			// Handle CP_UTF8 input ourselves to be able to handle
			// trailing bytes properly.
			// Get one UTF-8 character from src.
			bytelen = (पूर्णाङ्क)utf_ptr2len_len(src, size);
			यदि (bytelen > size)
			{
			    // Only got some bytes of a character.  Normally
			    // it's put in "conv_rest", but if it's too long
			    // deal with it as if they were illegal bytes.
			    यदि (bytelen <= CONV_RESTLEN)
				भङ्ग;

			    // weird overlong byte sequence
			    bytelen = size;
			    found_bad = TRUE;
			}
			अन्यथा
			{
			    पूर्णाङ्क	    u8c = utf_ptr2char(src);

			    यदि (u8c > 0xffff || (*src >= 0x80 && bytelen == 1))
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
			प्रति (bytelen = 1; bytelen <= size && bytelen <= 3;
								    ++bytelen)
			{
			    ucs2len = MultiByteToWideChar(codepage,
							 MB_ERR_INVALID_CHARS,
							 (LPCSTR)src, bytelen,
								   ucs2buf, 3);
			    यदि (ucs2len > 0)
				भङ्ग;
			}
			यदि (ucs2len == 0)
			{
			    // If we have only one byte then it's probably an
			    // incomplete byte sequence.  Otherwise discard
			    // one byte as a bad character.
			    यदि (size == 1)
				भङ्ग;
			    found_bad = TRUE;
			    bytelen = 1;
			}
		    }

		    यदि (!found_bad)
		    {
			पूर्णाङ्क	i;

			// Convert "ucs2buf[ucs2len]" to 'enc' in "dst".
			यदि (enc_utf8)
			{
			    // From UCS-2 to UTF-8.  Cannot fail.
			    प्रति (i = 0; i < ucs2len; ++i)
				dst += utf_char2bytes(ucs2buf[i], dst);
			}
			अन्यथा
			{
			    BOOL	bad = FALSE;
			    पूर्णाङ्क		dstlen;

			    // From UCS-2 to "enc_codepage".  If the
			    // conversion uses the default character "?",
			    // the data doesn't fit in this encoding.
			    dstlen = WideCharToMultiByte(enc_codepage, 0,
				    (LPCWSTR)ucs2buf, ucs2len,
				    (LPSTR)dst, (पूर्णाङ्क)(src - dst),
				    replstr, &bad);
			    यदि (bad)
				found_bad = TRUE;
			    अन्यथा
				dst += dstlen;
			}
		    }

		    यदि (found_bad)
		    {
			// Deal with bytes we can't convert.
			यदि (can_retry)
			    गच्छ rewind_retry;
			यदि (conv_error == 0)
			    conv_error = readfile_linenr(linecnt, ptr, dst);
			यदि (bad_char_behavior != BAD_DROP)
			{
			    यदि (bad_char_behavior == BAD_KEEP)
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

		यदि (size > 0)
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
	    यदि (fio_flags & FIO_MACROMAN)
	    {
		/*
		 * Conversion from Apple MacRoman char encoding to UTF-8 or
		 * latin1.  This is in os_mac_conv.c.
		 */
		यदि (macroman2enc(ptr, &size, real_size) == FAIL)
		    गच्छ rewind_retry;
	    }
	    अन्यथा
#endif
	    यदि (fio_flags != 0)
	    {
		पूर्णाङ्क	u8c;
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
		यदि (fio_flags == FIO_LATIN1 || fio_flags == FIO_UTF8)
		{
		    p = ptr + size;
		    यदि (fio_flags == FIO_UTF8)
		    {
			// Check for a trailing incomplete UTF-8 sequence
			tail = ptr + size - 1;
			यावत् (tail > ptr && (*tail & 0xc0) == 0x80)
			    --tail;
			यदि (tail + utf_byte2len(*tail) <= ptr + size)
			    tail = रिक्त;
			अन्यथा
			    p = tail;
		    }
		}
		अन्यथा यदि (fio_flags & (FIO_UCS2 | FIO_UTF16))
		{
		    // Check for a trailing byte
		    p = ptr + (size & ~1);
		    यदि (size & 1)
			tail = p;
		    यदि ((fio_flags & FIO_UTF16) && p > ptr)
		    {
			// Check for a trailing leading word
			यदि (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			अन्यथा
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			यदि (u8c >= 0xd800 && u8c <= 0xdbff)
			    tail = p;
			अन्यथा
			    p += 2;
		    }
		}
		अन्यथा //  FIO_UCS4
		{
		    // Check for trailing 1, 2 or 3 bytes
		    p = ptr + (size & ~3);
		    यदि (size & 3)
			tail = p;
		}

		// If there is a trailing incomplete sequence move it to
		// conv_rest[].
		यदि (tail != रिक्त)
		{
		    conv_restlen = (पूर्णाङ्क)((ptr + size) - tail);
		    mch_memmove(conv_rest, (char_u *)tail, conv_restlen);
		    size -= conv_restlen;
		}


		यावत् (p > ptr)
		{
		    यदि (fio_flags & FIO_LATIN1)
			u8c = *--p;
		    अन्यथा यदि (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    {
			यदि (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			अन्यथा
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			यदि ((fio_flags & FIO_UTF16)
					    && u8c >= 0xdc00 && u8c <= 0xdfff)
			{
			    पूर्णाङ्क u16c;

			    यदि (p == ptr)
			    {
				// Missing leading word.
				यदि (can_retry)
				    गच्छ rewind_retry;
				यदि (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				यदि (bad_char_behavior == BAD_DROP)
				    अनुवर्त;
				यदि (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }

			    // found second word of double-word, get the first
			    // word and compute the resulting character
			    यदि (fio_flags & FIO_ENDIAN_L)
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
			    यदि (u16c < 0xd800 || u16c > 0xdbff)
			    {
				यदि (can_retry)
				    गच्छ rewind_retry;
				यदि (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				यदि (bad_char_behavior == BAD_DROP)
				    अनुवर्त;
				यदि (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    अन्यथा यदि (fio_flags & FIO_UCS4)
		    {
			यदि (fio_flags & FIO_ENDIAN_L)
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
			यदि (*--p < 0x80)
			    u8c = *p;
			अन्यथा
			{
			    len = utf_head_off(ptr, p);
			    p -= len;
			    u8c = utf_ptr2char(p);
			    यदि (len == 0)
			    {
				// Not a valid UTF-8 character, retry with
				// another fenc when possible, otherwise just
				// report the error.
				यदि (can_retry)
				    गच्छ rewind_retry;
				यदि (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				यदि (bad_char_behavior == BAD_DROP)
				    अनुवर्त;
				यदि (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    यदि (enc_utf8)	// produce UTF-8
		    {
			dest -= utf_char2len(u8c);
			(शून्य)utf_char2bytes(u8c, dest);
		    }
		    अन्यथा		// produce Latin1
		    {
			--dest;
			यदि (u8c >= 0x100)
			{
			    // character doesn't fit in latin1, retry with
			    // another fenc when possible, otherwise just
			    // report the error.
			    यदि (can_retry)
				गच्छ rewind_retry;
			    यदि (conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
			    यदि (bad_char_behavior == BAD_DROP)
				++dest;
			    अन्यथा यदि (bad_char_behavior == BAD_KEEP)
				*dest = u8c;
			    अन्यथा यदि (eap != रिक्त && eap->bad_char != 0)
				*dest = bad_char_behavior;
			    अन्यथा
				*dest = 0xBF;
			}
			अन्यथा
			    *dest = u8c;
		    }
		}

		// move the linerest to before the converted characters
		पंक्ति_start = dest - linerest;
		mch_memmove(पंक्ति_start, buffer, (माप_प्रकार)linerest);
		size = (दीर्घ)((ptr + real_size) - dest);
		ptr = dest;
	    }
	    अन्यथा यदि (enc_utf8 && !curbuf->b_p_bin)
	    {
		पूर्णाङ्क  incomplete_tail = FALSE;

		// Reading UTF-8: Check if the bytes are valid UTF-8.
		प्रति (p = ptr; ; )
		{
		    पूर्णाङ्क	 todo;
		    पूर्णाङ्क	 l;

		    // Skip ASCII bytes quickly using word-at-a-time check.
		    {
			char_u *ascii_end = ptr + size;
			यावत् (ascii_end - p >= (दीर्घ)परिमाण(long_u))
			{
			    long_u word;
			    स_नकल(&word, p, परिमाण(long_u));
			    यदि (word & NONASCII_MASK)
				भङ्ग;
			    p += परिमाण(long_u);
			}
			यावत् (p < ascii_end && *p < 0x80)
			    ++p;
		    }

		    todo = (पूर्णाङ्क)((ptr + size) - p);
		    यदि (todo <= 0)
			भङ्ग;
		    यदि (*p >= 0x80)
		    {
			// A length of 1 means it's an illegal byte.  Accept
			// an incomplete character at the end though, the next
			// read() will get the next bytes, we'll check it
			// then.
			l = utf_ptr2len_len(p, todo);
			यदि (l > todo && !incomplete_tail)
			{
			    // Avoid retrying with a different encoding when
			    // a truncated file is more likely, or attempting
			    // to read the rest of an incomplete sequence when
			    // we have already done so.
			    यदि (p > ptr || filesize > 0)
				incomplete_tail = TRUE;
			    // Incomplete byte sequence, move it to conv_rest[]
			    // and try to read the rest of it, unless we've
			    // already done so.
			    यदि (p > ptr)
			    {
				conv_restlen = todo;
				mch_memmove(conv_rest, p, conv_restlen);
				size -= conv_restlen;
				भङ्ग;
			    }
			}
			यदि (l == 1 || l > todo)
			{
			    // Illegal byte.  If we can try another encoding
			    // do that, unless at EOF where a truncated
			    // file is more likely than a conversion error.
			    यदि (can_retry && !incomplete_tail)
				भङ्ग;
#ifdef USE_ICONV
			    // When we did a conversion report an error.
			    यदि (iconv_fd != (iconv_t)-1 && conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
#endif
			    // Remember the first linenr with an illegal byte
			    यदि (conv_error == 0 && illegal_byte == 0)
				illegal_byte = readfile_linenr(linecnt, ptr, p);

			    // Drop, keep or replace the bad byte.
			    यदि (bad_char_behavior == BAD_DROP)
			    {
				mch_memmove(p, p + 1, todo - 1);
				--size;
			    }
			    अन्यथा
			    {
				यदि (bad_char_behavior != BAD_KEEP)
				    *p = bad_char_behavior;
				++p;
			    }
			}
			अन्यथा
			    p += l;
		    }
		}
		यदि (p < ptr + size && !incomplete_tail)
		{
		    // Detected a UTF-8 error.
rewind_retry:
		    // Retry reading with another conversion.
#यदि defined(FEAT_EVAL) && defined(USE_ICONV)
		    यदि (*p_ccv != NUL && iconv_fd != (iconv_t)-1)
			// iconv() failed, try 'charconvert'
			did_iconv = TRUE;
		    अन्यथा
#endif
			// use next item from 'fileencodings'
			advance_fenc = TRUE;
		    file_rewind = TRUE;
		    गच्छ retry;
		}
	    }

	    // count the number of characters (after conversion!)
	    filesize += size;

	    /*
	     * when reading the first part of a file: guess EOL type
	     */
	    यदि (fileformat == EOL_UNKNOWN)
	    {
		// First try finding a NL, for Dos and Unix
		यदि (try_dos || try_unix)
		{
		    // Reset the carriage return counter.
		    यदि (try_mac)
			try_mac = 1;

		    प्रति (p = ptr; p < ptr + size; ++p)
		    {
			यदि (*p == NL)
			{
			    यदि (!try_unix
				    || (try_dos && p > ptr && p[-1] == CAR))
				fileformat = EOL_DOS;
			    अन्यथा
				fileformat = EOL_UNIX;
			    भङ्ग;
			}
			अन्यथा यदि (*p == CAR && try_mac)
			    try_mac++;
		    }

		    // Don't give in to EOL_UNIX if EOL_MAC is more likely
		    यदि (fileformat == EOL_UNIX && try_mac)
		    {
			// Need to reset the counters when retrying fenc.
			try_mac = 1;
			try_unix = 1;
			प्रति (; p >= ptr && *p != CAR; p--)
			    ;
			यदि (p >= ptr)
			{
			    प्रति (p = ptr; p < ptr + size; ++p)
			    {
				यदि (*p == NL)
				    try_unix++;
				अन्यथा यदि (*p == CAR)
				    try_mac++;
			    }
			    यदि (try_mac > try_unix)
				fileformat = EOL_MAC;
			}
		    }
		    अन्यथा यदि (fileformat == EOL_UNKNOWN && try_mac == 1)
			// Looking for CR but found no end-of-line markers at
			// all: use the default format.
			fileformat = default_fileformat();
		}

		// No NL found: may use Mac format
		यदि (fileformat == EOL_UNKNOWN && try_mac)
		    fileformat = EOL_MAC;

		// Still nothing found?  Use first format in 'ffs'
		यदि (fileformat == EOL_UNKNOWN)
		    fileformat = default_fileformat();

		// if editing a new file: may set p_tx and p_ff
		यदि (set_options)
		    set_fileformat(fileformat, OPT_LOCAL);
	    }
	}

	/*
	 * This loop is executed once for every character read.
	 * Keep it fast!
	 */
	यदि (fileformat == EOL_MAC)
	{
	    --ptr;
	    यावत् (++ptr, --size >= 0)
	    {
		// catch most common case first
		यदि ((c = *ptr) != NUL && c != CAR && c != NL)
		    अनुवर्त;
		यदि (c == NUL)
		    *ptr = NL;	// NULs are replaced by newlines!
		अन्यथा यदि (c == NL)
		    *ptr = CAR;	// NLs are replaced by CRs!
		अन्यथा
		{
		    यदि (skip_गिनती == 0)
		    {
			*ptr = NUL;	    // end of line
			len = (colnr_T) (ptr - पंक्ति_start + 1);
			यदि (ml_append(lnum, पंक्ति_start, len, newfile) == FAIL)
			{
			    error = TRUE;
			    भङ्ग;
			}
#ifdef FEAT_PERSISTENT_UNDO
			यदि (read_undo_file)
			    sha256_update(&sha_ctx, पंक्ति_start, len);
#endif
			++lnum;
			यदि (--read_गिनती == 0)
			{
			    error = TRUE;	// break loop
			    पंक्ति_start = ptr;	// nothing left to write
			    भङ्ग;
			}
		    }
		    अन्यथा
			--skip_गिनती;
		    पंक्ति_start = ptr + 1;
		}
	    }
	}
	अन्यथा
	{
	    // Use memchr() for SIMD-optimized newline scanning instead
	    // of scanning each byte individually.
	    char_u *end = ptr + size;

	    यावत् (ptr < end)
	    {
		char_u *nl = (char_u *)स_प्रथम(ptr, NL, end - ptr);
		char_u *nul_scan;

		यदि (nl == रिक्त)
		{
		    // No more newlines in buffer.
		    // Replace any NUL bytes with NL in remaining data.
		    यावत् ((nul_scan = (char_u *)स_प्रथम(ptr, NUL,
						      end - ptr)) != रिक्त)
		    {
			*nul_scan = NL;
			ptr = nul_scan + 1;
		    }
		    ptr = end;
		    भङ्ग;
		}

		// Replace NUL bytes with NL before the newline.
		{
		    char_u *scan = ptr;
		    यावत् ((nul_scan = (char_u *)स_प्रथम(scan, NUL,
						       nl - scan)) != रिक्त)
		    {
			*nul_scan = NL;
			scan = nul_scan + 1;
		    }
		}

		// Process the newline.
		ptr = nl;
		यदि (skip_गिनती == 0)
		{
		    *ptr = NUL;		// end of line
		    len = (colnr_T)(ptr - पंक्ति_start + 1);
		    यदि (fileformat == EOL_DOS)
		    {
			यदि (ptr > पंक्ति_start && ptr[-1] == CAR)
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
			अन्यथा यदि (ff_error != EOL_DOS)
			{
			    यदि (   try_unix
				&& !read_stdin
				&& (read_buffer
				    || vim_lseek(fd, (off_T)0L, शुरू_से)
								      == 0))
			    {
				fileformat = EOL_UNIX;
				यदि (set_options)
				    set_fileformat(EOL_UNIX, OPT_LOCAL);
				file_rewind = TRUE;
				keep_fileformat = TRUE;
				गच्छ retry;
			    }
			    ff_error = EOL_DOS;
			}
		    }
		    यदि (ml_append(lnum, पंक्ति_start, len, newfile) == FAIL)
		    {
			error = TRUE;
			भङ्ग;
		    }
#ifdef FEAT_PERSISTENT_UNDO
		    यदि (read_undo_file)
			sha256_update(&sha_ctx, पंक्ति_start, len);
#endif
		    ++lnum;
		    यदि (--read_गिनती == 0)
		    {
			error = TRUE;	    // break loop
			पंक्ति_start = ptr;   // nothing left to write
			भङ्ग;
		    }
		}
		अन्यथा
		    --skip_गिनती;
		पंक्ति_start = ptr + 1;
		++ptr;
	    }
	    size = -1;
	}
	linerest = (दीर्घ)(ptr - पंक्ति_start);
	ui_breakcheck();
    }

failed:
    // not an error, max. number of lines reached
    यदि (error && read_गिनती == 0)
	error = FALSE;

    // In Dos format ignore a trailing CTRL-Z, unless 'binary' is set.
    // In old days the file length was in sector count and the CTRL-Z the
    // marker where the file really ended.  Assuming we write it to a file
    // system that keeps file length properly the CTRL-Z should be dropped.
    // Set the 'endoffile' option so the user can decide what to write later.
    // In Unix format the CTRL-Z is just another character.
    यदि (linerest != 0
	    && !curbuf->b_p_bin
	    && fileformat == EOL_DOS
	    && ptr[-1] == Ctrl_Z)
    {
	ptr--;
	linerest--;
	यदि (set_options)
	    curbuf->b_p_eof = TRUE;
    }

    // If we get EOF in the middle of a line, note the fact by resetting
    // 'endofline' and add the line normally.
    यदि (!error
	    && !got_int
	    && linerest != 0)
    {
	// remember for when writing
	यदि (set_options)
	    curbuf->b_p_eol = FALSE;
	*ptr = NUL;
	len = (colnr_T)(ptr - पंक्ति_start + 1);
	यदि (ml_append(lnum, पंक्ति_start, len, newfile) == FAIL)
	    error = TRUE;
	अन्यथा
	{
#ifdef FEAT_PERSISTENT_UNDO
	    यदि (read_undo_file)
		sha256_update(&sha_ctx, पंक्ति_start, len);
#endif
	    read_no_eol_lnum = ++lnum;
	}
    }

    यदि (set_options)
	save_file_ff(curbuf);		// remember the current file format

#ifdef FEAT_CRYPT
    यदि (curbuf->b_cryptstate != रिक्त)
    {
	crypt_free_state(curbuf->b_cryptstate);
	curbuf->b_cryptstate = रिक्त;
    }
    यदि (cryptkey != रिक्त && cryptkey != curbuf->b_p_कुंजी)
	crypt_free_कुंजी(cryptkey);
    // Don't set cryptkey to NULL, it's used below as a flag that
    // encryption was used.
#endif

    // If editing a new file: set 'fenc' for the current buffer.
    // Also for ":read ++edit file".
    यदि (set_options)
	set_string_option_direct((char_u *)"fenc", -1, fenc,
						       OPT_FREE|OPT_LOCAL, 0);
    यदि (fenc_alloced)
	vim_free(fenc);
#ifdef USE_ICONV
    यदि (iconv_fd != (iconv_t)-1)
	iconv_close(iconv_fd);
#endif

    यदि (!read_buffer && !read_stdin)
	बंद(fd);				// errors are ignored
#ifdef HAVE_FD_CLOEXEC
    अन्यथा
    {
	पूर्णाङ्क fdflags = fcntl(fd, F_GETFD);

	यदि (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (शून्य)fcntl(fd, F_SETFD, fdflags | FD_CLOEXEC);
    }
#endif
    vim_free(buffer);

#ifdef HAVE_DUP
    यदि (read_stdin)
    {
	// Use stderr for stdin, makes shell commands work.
	बंद(0);
	vim_ignored = dup(2);
    }
#endif

    यदि (tmpname != रिक्त)
    {
	mch_remove(tmpname);		// delete converted file
	vim_free(tmpname);
    }
    --no_wait_return;			// may wait for return now

    /*
     * In recovery mode everything but autocommands is skipped.
     */
    यदि (!recoverymode)
    {
	// need to delete the last line, which comes from the empty buffer
	यदि (newfile && wasempty && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	{
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 0;
#endif
	    ml_delete(curbuf->b_ml.ml_पंक्ति_गिनती);
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 1;
#endif
	    --linecnt;
	}
	linecnt = curbuf->b_ml.ml_पंक्ति_गिनती - linecnt;
	यदि (filesize == 0)
	    linecnt = 0;
	यदि (newfile || read_buffer)
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
	अन्यथा यदि (linecnt)		// appended at least one line
	    appended_पंक्तियाँ_mark(from, linecnt);

#ifndef ALWAYS_USE_GUI
	/*
	 * If we were reading from the same terminal as where messages go,
	 * the screen will have been messed up.
	 * Switch on raw mode now and clear the screen.
	 */
	यदि (read_stdin)
	{
	    settmode(TMODE_RAW);	// set to raw mode
	    starttermcap();
	    screenclear();
	}
#endif

	यदि (got_int)
	{
	    यदि (!(flags & READ_DUMMY))
	    {
		filemess(curbuf, sfname, (char_u *)_(e_interrupted), 0);
		यदि (newfile)
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    }
	    संदेश_scroll = संदेश_save;
#ifdef FEAT_VIMINFO
	    check_marks_read();
#endif
	    retval = OK;	// an interrupt isn't really an error
	    गच्छ theend;
	}

	यदि (!filtering && !(flags & READ_DUMMY))
	{
	    पूर्णाङ्क	buflen;

	    संदेश_add_fname(curbuf, sfname);   // fname in IObuff with quotes
	    c = FALSE;

	    buflen = (पूर्णाङ्क)STRLEN(IObuff);
#ifdef UNIX
	    यदि (S_ISFIFO(perm))			    // fifo
	    {
		buflen += vim_snprintf((वर्ण *)IObuff + buflen, IOSIZE - buflen,
			_("[fifo]"));
		c = TRUE;
	    }
	    यदि (S_ISSOCK(perm))			    // or socket
	    {
		buflen += vim_snprintf((वर्ण *)IObuff + buflen, IOSIZE - buflen,
			_("[socket]"));
		c = TRUE;
	    }
# ifdef OPEN_CHR_FILES
	    यदि (S_ISCHR(perm))			    // or character special
	    {
		buflen += vim_snprintf((वर्ण *)IObuff + buflen, IOSIZE - buflen,
			_("[character special]"));
		c = TRUE;
	    }
# endif
#endif
	    यदि (curbuf->b_p_ro)
	    {
		buflen += vim_snprintf((वर्ण *)IObuff + buflen, IOSIZE - buflen,
			"%s", shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
		c = TRUE;
	    }
	    यदि (read_no_eol_lnum)
	    {
		संदेश_add_eol();
		c = TRUE;
	    }
	    यदि (ff_error == EOL_DOS)
	    {
		buflen += vim_snprintf((वर्ण *)IObuff + buflen, IOSIZE - buflen,
			_("[CR missing]"));
		c = TRUE;
	    }
	    यदि (split)
	    {
		buflen += vim_snprintf((वर्ण *)IObuff + buflen, IOSIZE - buflen,
			_("[long lines split]"));
		c = TRUE;
	    }
	    यदि (notconverted)
	    {
		buflen += vim_snprintf((वर्ण *)IObuff + buflen, IOSIZE - buflen,
			_("[NOT converted]"));
		c = TRUE;
	    }
	    अन्यथा यदि (converted)
	    {
		buflen += vim_snprintf((वर्ण *)IObuff + buflen, IOSIZE - buflen,
			_("[converted]"));
		c = TRUE;
	    }
#ifdef FEAT_CRYPT
	    यदि (cryptkey != रिक्त)
	    {
		crypt_append_संदेश(curbuf);
		c = TRUE;
	    }
#endif
	    यदि (conv_error != 0)
	    {
		vim_snprintf((वर्ण *)IObuff + buflen, IOSIZE - buflen,
			_("[CONVERSION ERROR in line %ld]"), (दीर्घ)conv_error);
		c = TRUE;
	    }
	    अन्यथा यदि (illegal_byte > 0)
	    {
		vim_snprintf((वर्ण *)IObuff + buflen, IOSIZE - buflen,
			_("[ILLEGAL BYTE in line %ld]"), (दीर्घ)illegal_byte);
		c = TRUE;
	    }
	    अन्यथा यदि (error)
	    {
		vim_snprintf((वर्ण *)IObuff + buflen, IOSIZE - buflen,
			_("[READ ERRORS]"));
		c = TRUE;
	    }
	    यदि (संदेश_add_fileformat(fileformat))
		c = TRUE;
#ifdef FEAT_CRYPT
	    यदि (cryptkey != रिक्त)
		संदेश_add_पंक्तियाँ(c, (दीर्घ)linecnt, filesize
			 - crypt_get_header_len(crypt_get_method_nr(curbuf)));
	    अन्यथा
#endif
		संदेश_add_पंक्तियाँ(c, (दीर्घ)linecnt, filesize);

	    VIM_CLEAR(keep_संदेश);
	    संदेश_scrolled_ign = TRUE;
#ifdef ALWAYS_USE_GUI
	    // Don't show the message when reading stdin, it would end up in a
	    // message box (which might be shown when exiting!)
	    यदि (read_stdin || read_buffer)
		p = संदेश_may_trunc(FALSE, IObuff);
	    अन्यथा
#endif
	    {
		यदि (संदेश_col > 0)
		    संदेश_putchar('\r');  // overwrite previous message
		p = (char_u *)संदेश_trunc_attr((वर्ण *)IObuff, FALSE, 0);
	    }
	    यदि (read_stdin || read_buffer || restart_edit != 0
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
	यदि (newfile && (error
		    || conv_error != 0
		    || (illegal_byte > 0 && bad_char_behavior != BAD_KEEP)))
	    curbuf->b_p_ro = TRUE;

	u_clearline();	    // cannot use "U" command after adding lines

	/*
	 * In Ex mode: cursor at last new line.
	 * Otherwise: cursor at first new line.
	 */
	यदि (exmode_active)
	    curwin->w_cursor.lnum = from + linecnt;
	अन्यथा
	    curwin->w_cursor.lnum = from + 1;
	check_cursor_lnum();
	beginline(BL_WHITE | BL_FIX);	    // on first non-blank

	यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
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
	यदि (newfile && !read_stdin && !read_buffer
					 && mch_stat((वर्ण *)fname, &st) >= 0)
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
    यदि (flags & READ_KEEP_UNDO)
	u_find_first_changed();

#ifdef FEAT_PERSISTENT_UNDO
    /*
     * When opening a new file locate undo info and read it.
     */
    यदि (read_undo_file)
    {
	char_u	hash[UNDO_HASH_SIZE];

	sha256_finish(&sha_ctx, hash);
	u_read_undo(रिक्त, hash, fname);
    }
#endif

    यदि (!read_stdin && !read_fifo && (!read_buffer || sfname != रिक्त))
    {
	पूर्णाङ्क m = संदेश_scroll;
	पूर्णाङ्क n = संदेश_scrolled;

	// Save the fileformat now, otherwise the buffer will be considered
	// modified if the format/encoding was automatically detected.
	यदि (set_options)
	    save_file_ff(curbuf);

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	संदेश_scroll = TRUE;
	यदि (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPOST, रिक्त, sfname,
							  FALSE, curbuf, eap);
	अन्यथा यदि (newfile || (read_buffer && sfname != रिक्त))
	{
	    apply_autocmds_exarg(EVENT_BUFREADPOST, रिक्त, sfname,
							  FALSE, curbuf, eap);
	    यदि (!curbuf->b_au_did_filetype && *curbuf->b_p_ft != NUL)
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
	यदि (संदेश_scrolled == n)
	    संदेश_scroll = m;
#ifdef FEAT_EVAL
	यदि (aborting())	    // autocmds may abort script processing
	    गच्छ theend;
#endif
    }

    यदि (!(recoverymode && error))
	retval = OK;

theend:
    यदि (curbuf->b_ml.ml_mfp != रिक्त
		       && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	// OK to sync the swap file now
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    प्रत्यागम retval;
}

#यदि defined(OPEN_CHR_FILES)
/*
 * Returns TRUE if the file name argument is of the form "/dev/fd/\d\+",
 * which is the name of files used for process substitution output by
 * some shells on some operating systems, e.g., bash on SunOS.
 * Do not accept "/dev/fd/[012]", opening these may hang Vim.
 */
    पूर्णाङ्क
is_dev_fd_file(char_u *fname)
{
    प्रत्यागम (STRNCMP(fname, "/dev/fd/", 8) == 0
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

    lnum = curbuf->b_ml.ml_पंक्ति_गिनती - linecnt + 1;
    प्रति (s = p; s < endp; ++s)
	यदि (*s == '\n')
	    ++lnum;
    प्रत्यागम lnum;
}

/*
 * Fill "*eap" to force the 'fileencoding', 'fileformat' and 'binary' to be
 * equal to the buffer "buf".  Used for calling readfile().
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
prep_exarg(exarg_T *eap, buf_T *buf)
{
    eap->cmd = alloc(15 + (अचिन्हित)STRLEN(buf->b_p_fenc));
    यदि (eap->cmd == रिक्त)
	प्रत्यागम FAIL;

    प्र_लिखो((वर्ण *)eap->cmd, "e ++enc=%s", buf->b_p_fenc);
    eap->force_enc = 8;
    eap->bad_char = buf->b_bad_char;
    eap->force_ff = *buf->b_p_ff;

    eap->force_bin = buf->b_p_bin ? FORCE_BIN : FORCE_NOBIN;
    eap->read_edit = FALSE;
    eap->forceit = FALSE;
    प्रत्यागम OK;
}

/*
 * Set default or forced 'fileformat' and 'binary'.
 */
    शून्य
set_file_options(पूर्णाङ्क set_options, exarg_T *eap)
{
    // set default 'fileformat'
    यदि (set_options)
    {
	यदि (eap != रिक्त && eap->force_ff != 0)
	    set_fileformat(get_fileformat_force(curbuf, eap), OPT_LOCAL);
	अन्यथा यदि (*p_ffs != NUL)
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    // set or reset 'binary'
    यदि (eap != रिक्त && eap->force_bin != 0)
    {
	पूर्णाङ्क	oldval = curbuf->b_p_bin;

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
    यदि (eap->force_enc == 0)
	प्रत्यागम;

    char_u *fenc = enc_canonize(eap->cmd + eap->force_enc);

    यदि (fenc != रिक्त)
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
next_fenc(char_u **pp, पूर्णाङ्क *alloced)
{
    char_u	*p;
    char_u	*r;

    *alloced = FALSE;
    यदि (**pp == NUL)
    {
	*pp = रिक्त;
	प्रत्यागम (char_u *)"";
    }
    p = vim_strchr(*pp, ',');
    यदि (p == रिक्त)
    {
	r = enc_canonize(*pp);
	*pp += STRLEN(*pp);
    }
    अन्यथा
    {
	r = vim_strnsave(*pp, p - *pp);
	*pp = p + 1;
	यदि (r != रिक्त)
	{
	    p = enc_canonize(r);
	    vim_free(r);
	    r = p;
	}
    }
    यदि (r != रिक्त)
	*alloced = TRUE;
    अन्यथा
    {
	// out of memory
	r = (char_u *)"";
	*pp = रिक्त;
    }
    प्रत्यागम r;
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
    पूर्णाङ्क		*fdp)		// in/out: file descriptor of file
{
    char_u	*tmpname;
    वर्ण	*errmsg = रिक्त;

    tmpname = vim_tempname('r', FALSE);
    यदि (tmpname == रिक्त)
	errmsg = _("Can't find temp file for conversion");
    अन्यथा
    {
	बंद(*fdp);		// close the input file, ignore errors
	*fdp = -1;
	यदि (eval_charconvert(fenc, enc_utf8 ? (char_u *)"utf-8" : p_enc,
						      fname, tmpname) == FAIL)
	    errmsg = _("Conversion with 'charconvert' failed");
	यदि (errmsg == रिक्त && (*fdp = mch_open((वर्ण *)tmpname,
						  O_RDONLY | O_EXTRA, 0)) < 0)
	    errmsg = _("can't read output of 'charconvert'");
    }

    यदि (errmsg != रिक्त)
    {
	// Don't use emsg(), it breaks mappings, the retry with
	// another type of conversion might still work.
	संदेश(errmsg);
	यदि (tmpname != रिक्त)
	{
	    mch_remove(tmpname);	// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    // If the input file is closed, open it (caller should check for error).
    यदि (*fdp < 0)
	*fdp = mch_open((वर्ण *)fname, O_RDONLY | O_EXTRA, 0);

    प्रत्यागम tmpname;
}
#endif

#यदि defined(FEAT_CRYPT)
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
    पूर्णाङ्क		newfile,	// editing a new buffer
    char_u	*fname,		// file name to display
    पूर्णाङ्क		*did_ask)	// flag: whether already asked for key
{
    पूर्णाङ्क method = crypt_method_nr_from_magic((वर्ण *)ptr, *sizep);
    पूर्णाङ्क b_p_ro = curbuf->b_p_ro;

    यदि (method >= 0)
    {
	// Mark the buffer as read-only until the decryption has taken place.
	// Avoids accidentally overwriting the file with garbage.
	curbuf->b_p_ro = TRUE;

	// Set the cryptmethod local to the buffer.
	crypt_set_cm_option(curbuf, method);
	यदि (cryptkey == रिक्त && !*did_ask)
	{
	    यदि (*curbuf->b_p_कुंजी)
	    {
		cryptkey = curbuf->b_p_कुंजी;
		crypt_check_swapfile_curbuf();
	    }
	    अन्यथा
	    {
		// When newfile is TRUE, store the typed key in the 'key'
		// option and don't free it.  bf needs hash of the key saved.
		// Don't ask for the key again when first time Enter was hit.
		// Happens when retrying to detect encoding.
		smsg(_(need_कुंजी_संदेश), fname);
		संदेश_scroll = TRUE;
		crypt_check_method(method);
		cryptkey = crypt_get_कुंजी(newfile, FALSE);
		*did_ask = TRUE;

		// check if empty key entered
		यदि (cryptkey != रिक्त && *cryptkey == NUL)
		{
		    यदि (cryptkey != curbuf->b_p_कुंजी)
			vim_free(cryptkey);
		    cryptkey = रिक्त;
		}
	    }
	}

	यदि (cryptkey != रिक्त)
	{
	    पूर्णाङ्क header_len;

	    header_len = crypt_get_header_len(method);
	    यदि (*sizep < header_len)
	    {
		// invalid header, buffer can't be encrypted
		यदि (cryptkey != curbuf->b_p_कुंजी)
		    vim_free(cryptkey);
		प्रत्यागम रिक्त;
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
    अन्यथा यदि (newfile && *curbuf->b_p_कुंजी != NUL && !starting)
	set_option_मूल्य_give_err((char_u *)"key", 0L, (char_u *)"", OPT_LOCAL);

    प्रत्यागम cryptkey;
}
#endif  // FEAT_CRYPT

/*
 * Return TRUE if a file appears to be read-only from the file permissions.
 */
    पूर्णाङ्क
check_file_readonly(
    char_u	*fname,		// full path to file
    पूर्णाङ्क		perm UNUSED)	// known permissions on file
{
#ifndef USE_MCH_ACCESS
    पूर्णाङ्क	    fd = 0;
#endif

    प्रत्यागम (
#ifdef USE_MCH_ACCESS
# ifdef UNIX
	(perm & 0222) == 0 ||
# endif
	mch_access((वर्ण *)fname, W_OK)
#अन्यथा
	(fd = mch_open((वर्ण *)fname, O_RDWR | O_EXTRA, 0)) < 0
					? TRUE : (बंद(fd), FALSE)
#endif
	);
}

#यदि defined(HAVE_FSYNC)
/*
 * Call fsync() with Mac-specific exception.
 * Return fsync() result: zero for success.
 */
    पूर्णाङ्क
vim_fsync(पूर्णाङ्क fd)
{
    पूर्णाङ्क r;

# ifdef MACOS_X
    r = fcntl(fd, F_FULLFSYNC);
    यदि (r != 0)  // F_FULLFSYNC not working or not supported
# endif
	r = fsync(fd);
    प्रत्यागम r;
}
#endif

/*
 * Set the name of the current buffer.  Use when the buffer doesn't have a
 * name and a ":r" or ":w" command with a file name is used.
 */
    पूर्णाङ्क
set_rw_fname(char_u *fname, char_u *sfname)
{
    buf_T	*buf = curbuf;

    // It's like the unnamed buffer is deleted....
    यदि (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFDELETE, रिक्त, रिक्त, FALSE, curbuf);
    apply_autocmds(EVENT_BUFWIPEOUT, रिक्त, रिक्त, FALSE, curbuf);
#ifdef FEAT_EVAL
    यदि (aborting())	    // autocmds may abort script processing
	प्रत्यागम FAIL;
#endif
    यदि (curbuf != buf)
    {
	// We are in another buffer now, don't do the renaming.
	emsg(_(e_autocommands_changed_buffer_or_buffer_name));
	प्रत्यागम FAIL;
    }

    यदि (setfname(curbuf, fname, sfname, FALSE) == OK)
	curbuf->b_flags |= BF_NOTEDITED;

    // ....and a new named one is created
    apply_autocmds(EVENT_BUFNEW, रिक्त, रिक्त, FALSE, curbuf);
    यदि (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFADD, रिक्त, रिक्त, FALSE, curbuf);
#ifdef FEAT_EVAL
    यदि (aborting())	    // autocmds may abort script processing
	प्रत्यागम FAIL;
#endif

    // Do filetype detection now if 'filetype' is empty.
    यदि (*curbuf->b_p_ft == NUL)
    {
	यदि (au_has_group((char_u *)"filetypedetect"))
	    (शून्य)do_doautocmd((char_u *)"filetypedetect BufRead", FALSE, रिक्त);
	do_modelines(0);
    }

    प्रत्यागम OK;
}

/*
 * Put file name into IObuff with quotes.
 */
    शून्य
संदेश_add_fname(buf_T *buf, char_u *fname)
{
    माप_प्रकार  IObufflen = 0;

    यदि (fname == रिक्त)
	fname = (char_u *)"-stdin-";
    IObuff[IObufflen++] = '"';
    IObufflen += home_replace(buf, fname, IObuff + IObufflen, IOSIZE - 4, TRUE);
    STRCPY(IObuff + IObufflen, "\" ");
}

/*
 * Append message for text mode to IObuff.
 * Return TRUE if something appended.
 */
    पूर्णाङ्क
संदेश_add_fileformat(पूर्णाङ्क eol_type)
{
#ifndef USE_CRNL
    यदि (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[dos]") : _("[dos format]"));
	प्रत्यागम TRUE;
    }
#endif
    यदि (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[mac]") : _("[mac format]"));
	प्रत्यागम TRUE;
    }
#ifdef USE_CRNL
    यदि (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[unix]") : _("[unix format]"));
	प्रत्यागम TRUE;
    }
#endif
    प्रत्यागम FALSE;
}

/*
 * Append line and character count to IObuff.
 */
    शून्य
संदेश_add_पंक्तियाँ(
    पूर्णाङ्क	    जोड़ो_space,
    दीर्घ    lnum,
    off_T   nchars)
{
    पूर्णाङ्क  len = (पूर्णाङ्क)STRLEN(IObuff);

    यदि (shortmess(SHM_पंक्तियाँ))
	vim_snprintf((वर्ण *)IObuff + len, IOSIZE - (माप_प्रकार)len,
		// l10n: L as in line, B as in byte
		_("%s%ldL, %lldB"), जोड़ो_space ? " " : "", lnum, (varnumber_T)nchars);
    अन्यथा
    {
	len += vim_snprintf((वर्ण *)IObuff + len, IOSIZE - (माप_प्रकार)len,
		NGETTEXT("%s%ld line, ", "%s%ld lines, ", lnum), जोड़ो_space ? " " : "", lnum);
	vim_snprintf((वर्ण *)IObuff + len, IOSIZE - (माप_प्रकार)len,
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

    पूर्णाङ्क
time_differs(stat_T *st, दीर्घ mtime, दीर्घ mtime_ns UNUSED)
{
    प्रत्यागम
#ifdef ST_MTIM_NSEC
	(दीर्घ)st->ST_MTIM_NSEC != mtime_ns ||
#endif
#यदि defined(__linux__) || defined(MSWIN)
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
    पूर्णाङ्क
need_conversion(char_u *fenc)
{
    पूर्णाङ्क		same_encoding;
    पूर्णाङ्क		enc_flags;
    पूर्णाङ्क		fenc_flags;

    यदि (*fenc == NUL || STRCMP(p_enc, fenc) == 0)
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
    यदि (same_encoding)
    {
	// Specified encoding matches with 'encoding'.  This requires
	// conversion when 'encoding' is Unicode but not UTF-8.
	प्रत्यागम enc_unicode != 0;
    }

    // Encodings differ.  However, conversion is not needed when 'enc' is any
    // Unicode encoding and the file is UTF-8.
    प्रत्यागम !(enc_utf8 && fenc_flags == FIO_UTF8);
}

/*
 * Check "ptr" for a unicode encoding and return the FIO_ flags needed for the
 * internal conversion.
 * if "ptr" is an empty string, use 'encoding'.
 */
    पूर्णाङ्क
get_fio_flags(char_u *ptr)
{
    पूर्णाङ्क		prop;

    यदि (*ptr == NUL)
	ptr = p_enc;

    prop = enc_canon_props(ptr);
    यदि (prop & ENC_UNICODE)
    {
	यदि (prop & ENC_2BYTE)
	{
	    यदि (prop & ENC_ENDIAN_L)
		प्रत्यागम FIO_UCS2 | FIO_ENDIAN_L;
	    प्रत्यागम FIO_UCS2;
	}
	यदि (prop & ENC_4BYTE)
	{
	    यदि (prop & ENC_ENDIAN_L)
		प्रत्यागम FIO_UCS4 | FIO_ENDIAN_L;
	    प्रत्यागम FIO_UCS4;
	}
	यदि (prop & ENC_2WORD)
	{
	    यदि (prop & ENC_ENDIAN_L)
		प्रत्यागम FIO_UTF16 | FIO_ENDIAN_L;
	    प्रत्यागम FIO_UTF16;
	}
	प्रत्यागम FIO_UTF8;
    }
    यदि (prop & ENC_LATIN1)
	प्रत्यागम FIO_LATIN1;
    // must be ENC_DBCS, requires iconv()
    प्रत्यागम 0;
}

#यदि defined(MSWIN)
/*
 * Check "ptr" for a MS-Windows codepage name and return the FIO_ flags needed
 * for the conversion MS-Windows can do for us.  Also accept "utf-8".
 * Used for conversion between 'encoding' and 'fileencoding'.
 */
    पूर्णाङ्क
get_win_fio_flags(char_u *ptr)
{
    पूर्णाङ्क		cp;

    // Cannot do this when 'encoding' is not utf-8 and not a codepage.
    यदि (!enc_utf8 && enc_codepage <= 0)
	प्रत्यागम 0;

    cp = encname2codepage(ptr);
    यदि (cp == 0)
    {
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	यदि (STRCMP(ptr, "utf-8") == 0)
	    cp = CP_UTF8;
	अन्यथा
# endif
	    प्रत्यागम 0;
    }
    प्रत्यागम FIO_PUT_CP(cp) | FIO_CODEPAGE;
}
#endif

#यदि defined(MACOS_CONVERT)
/*
 * Check "ptr" for a Carbon supported encoding and return the FIO_ flags
 * needed for the internal conversion to/from utf-8 or latin1.
 */
    पूर्णाङ्क
get_mac_fio_flags(char_u *ptr)
{
    यदि ((enc_utf8 || STRCMP(p_enc, "latin1") == 0)
				     && (enc_canon_props(ptr) & ENC_MACROMAN))
	प्रत्यागम FIO_MACROMAN;
    प्रत्यागम 0;
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
    पूर्णाङ्क		*lenp,
    पूर्णाङ्क		flags)
{
    वर्ण	*name = रिक्त;
    पूर्णाङ्क		len = 2;

    यदि (p[0] == 0xef && p[1] == 0xbb && size >= 3 && p[2] == 0xbf
	    && (flags == FIO_ALL || flags == FIO_UTF8 || flags == 0))
    {
	name = "utf-8";		// EF BB BF
	len = 3;
    }
    अन्यथा यदि (p[0] == 0xff && p[1] == 0xfe)
    {
	यदि (size >= 4 && p[2] == 0 && p[3] == 0
	    && (flags == FIO_ALL || flags == (FIO_UCS4 | FIO_ENDIAN_L)))
	{
	    name = "ucs-4le";	// FF FE 00 00
	    len = 4;
	}
	अन्यथा यदि (flags == (FIO_UCS2 | FIO_ENDIAN_L))
	    name = "ucs-2le";	// FF FE
	अन्यथा यदि (flags == FIO_ALL || flags == (FIO_UTF16 | FIO_ENDIAN_L))
	    // utf-16le is preferred, it also works for ucs-2le text
	    name = "utf-16le";	// FF FE
    }
    अन्यथा यदि (p[0] == 0xfe && p[1] == 0xff
	    && (flags == FIO_ALL || flags == FIO_UCS2 || flags == FIO_UTF16))
    {
	// Default to utf-16, it works also for ucs-2 text.
	यदि (flags == FIO_UCS2)
	    name = "ucs-2";	// FE FF
	अन्यथा
	    name = "utf-16";	// FE FF
    }
    अन्यथा यदि (size >= 4 && p[0] == 0 && p[1] == 0 && p[2] == 0xfe
	    && p[3] == 0xff && (flags == FIO_ALL || flags == FIO_UCS4))
    {
	name = "ucs-4";		// 00 00 FE FF
	len = 4;
    }

    *lenp = len;
    प्रत्यागम (char_u *)name;
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
    यदि (स_नाम == रिक्त)
	प्रत्यागम full_path;
    यदि (mch_dirname(स_नाम, MAXPATHL) == OK)
    {
	p = shorten_fname(full_path, स_नाम);
	यदि (p == रिक्त || *p == NUL)
	    p = full_path;
    }
    vim_free(स_नाम);
    प्रत्यागम p;
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
    पूर्णाङ्क		len;
    char_u	*p;

    यदि (full_path == रिक्त)
	प्रत्यागम रिक्त;
    len = (पूर्णाङ्क)STRLEN(dir_name);
    यदि (fnamencmp(dir_name, full_path, len) == 0)
    {
	p = full_path + len;
#यदि defined(MSWIN)
	/*
	 * MS-Windows: when a file is in the root directory, dir_name will end
	 * in a slash, since C: by itself does not define a specific dir. In
	 * this case p may already be correct. <negri>
	 */
	यदि (!((len > 2) && (*(p - 2) == ':')))
#endif
	{
	    यदि (vim_ispathsep(*p))
		कुरु
		    ++p;
		यावत् (vim_ispathsep_nocolon(*p));
#ifndef VMS   // the path separator is always part of the path
	    अन्यथा
		p = रिक्त;
#endif
	}
    }
#यदि defined(MSWIN)
    /*
     * When using a file in the current drive, remove the drive name:
     * "A:\dir\file" -> "\dir\file".  This helps when moving a session file on
     * a floppy from "A:\dir" to "B:\dir".
     */
    अन्यथा यदि (len > 3
	    && TOUPPER_LOC(full_path[0]) == TOUPPER_LOC(dir_name[0])
	    && full_path[1] == ':'
	    && vim_ispathsep(full_path[2]))
	p = full_path + 2;
#endif
    अन्यथा
	p = रिक्त;
    प्रत्यागम p;
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
shorten_buf_fname(buf_T *buf, char_u *स_नाम, पूर्णाङ्क force)
{
    char_u	*p;

    यदि (buf->b_fname != रिक्त
	    && !bt_nofilename(buf)
	    && !path_with_url(buf->b_fname)
	    && (force
		|| buf->b_sfname == रिक्त
		|| mch_isFullName(buf->b_sfname)))
    {
	यदि (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	p = shorten_fname(buf->b_ffname, स_नाम);
	यदि (p != रिक्त)
	{
	    buf->b_sfname = vim_strsave(p);
	    buf->b_fname = buf->b_sfname;
	}
	यदि (p == रिक्त || buf->b_fname == रिक्त)
	    buf->b_fname = buf->b_ffname;
    }
}

/*
 * Shorten filenames for all buffers.
 */
    शून्य
shorten_fnames(पूर्णाङ्क force)
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
#यदि defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#यदि defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    popup_update_preview_title();
#endif
}

#यदि (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
/*
 * Shorten all filenames in "fnames[count]" by current directory.
 */
    शून्य
shorten_filenames(char_u **fnames, पूर्णाङ्क गिनती)
{
    पूर्णाङ्क		i;
    char_u	स_नाम[MAXPATHL];
    char_u	*p;

    यदि (fnames == रिक्त || गिनती < 1)
	प्रत्यागम;
    mch_dirname(स_नाम, परिमाण(स_नाम));
    प्रति (i = 0; i < गिनती; ++i)
    {
	यदि ((p = shorten_fname(fnames[i], स_नाम)) != रिक्त)
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
    पूर्णाङ्क	    prepend_dot)	// may prepend a '.' to file name
{
    प्रत्यागम buf_modname((curbuf->b_p_sn || curbuf->b_shortname),
						     fname, ext, prepend_dot);
}

    char_u *
buf_modname(
    पूर्णाङ्क	    shortname,		// use 8.3 file name
    char_u  *fname,
    char_u  *ext,
    पूर्णाङ्क	    prepend_dot)	// may prepend a '.' to file name
{
    char_u	*retval;
    char_u	*s;
    char_u	*e;
    char_u	*ptr;
    माप_प्रकार	ptrlen;
    पूर्णाङ्क		fnamelen, extlen;

    extlen = (पूर्णाङ्क)STRLEN(ext);

    /*
     * If there is no file name we must get the name of the current directory
     * (we need the full path in case :cd is used).
     */
    यदि (fname == रिक्त || *fname == NUL)
    {
	retval = alloc(MAXPATHL + extlen + 3);
	यदि (retval == रिक्त)
	    प्रत्यागम रिक्त;
	यदि (mch_dirname(retval, MAXPATHL) == FAIL ||
				     (fnamelen = (पूर्णाङ्क)STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    प्रत्यागम रिक्त;
	}
	यदि (!after_pathsep(retval, retval + fnamelen))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
	prepend_dot = FALSE;	    // nothing to prepend a dot to
    }
    अन्यथा
    {
	fnamelen = (पूर्णाङ्क)STRLEN(fname);
	retval = alloc(fnamelen + extlen + 3);
	यदि (retval == रिक्त)
	    प्रत्यागम रिक्त;
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
    प्रति (ptr = retval + fnamelen; ptr > retval; MB_PTR_BACK(retval, ptr))
    {
	यदि (*ext == '.' && shortname)
	    यदि (*ptr == '.')	// replace '.' by '_'
		*ptr = '_';
	यदि (vim_ispathsep(*ptr))
	{
	    ++ptr;
	    भङ्ग;
	}
    }

    // the file name has at most BASENAMELEN characters.
    ptrlen = (माप_प्रकार)(fnamelen - (ptr - retval));
    यदि (ptrlen > (अचिन्हित)BASENAMELEN)
    {
	ptrlen = BASENAMELEN;
	ptr[ptrlen] = NUL;
    }

    s = ptr + ptrlen;

    /*
     * For 8.3 file names we may have to reduce the length.
     */
    यदि (shortname)
    {
	/*
	 * If there is no file name, or the file name ends in '/', and the
	 * extension starts with '.', put a '_' before the dot, because just
	 * ".ext" is invalid.
	 */
	यदि (fname == रिक्त || *fname == NUL
				   || vim_ispathsep(fname[STRLEN(fname) - 1]))
	{
	    यदि (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	अन्यथा यदि (*ext == '.')
	{
	    यदि ((माप_प्रकार)(s - ptr) > (माप_प्रकार)8)
	    {
		s = ptr + 8;
		*s = '\0';
	    }
	}
	/*
	 * If the extension doesn't start with '.', and the file name
	 * doesn't have an extension yet, append a '.'
	 */
	अन्यथा यदि ((e = vim_strchr(ptr, '.')) == रिक्त)
	    *s++ = '.';
	/*
	 * If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be truncated
	 */
	अन्यथा यदि ((पूर्णाङ्क)(ptrlen - (e - retval)) + extlen > 4)
	    s = e + 4 - extlen;
    }
#ifdef MSWIN
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    अन्यथा यदि ((fname == रिक्त || *fname == NUL) && *ext == '.')
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
    यदि (prepend_dot && !shortname && *(e = gettail(retval)) != '.')
    {
	mch_memmove(e + 1, e, (माप_प्रकार)(((fnamelen + extlen) - (e - retval)) + 1));	// +1 for NUL
	*e = '.';
    }

    /*
     * Check that, after appending the extension, the file name is really
     * different.
     */
    यदि (fname != रिक्त && STRCMP(fname, retval) == 0)
    {
	// we search for a character that can be replaced by '_'
	यावत् (--s >= ptr)
	{
	    यदि (*s != '_')
	    {
		*s = '_';
		भङ्ग;
	    }
	}
	यदि (s < ptr)	// fname was "________.<ext>", how tricky!
	    *ptr = 'v';
    }
    प्रत्यागम retval;
}

/*
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE for end-of-file.
 * If the line is truncated then buf[size - 2] will not be NUL.
 */
    पूर्णाङ्क
vim_fgets(char_u *buf, पूर्णाङ्क size, खाता *fp)
{
    वर्ण	*eof;
#define FGETS_SIZE 200
    वर्ण	tbuf[FGETS_SIZE];

    // safety check
    यदि (size < 2)
    {
	यदि (size == 1)
	    buf[0] = NUL;
	प्रत्यागम TRUE;
    }

    buf[size - 2] = NUL;
    eof = ख_माला_लो((वर्ण *)buf, size, fp);
    यदि (buf[size - 2] != NUL && buf[size - 2] != '\n')
    {
	buf[size - 1] = NUL;	    // Truncate the line

	// Now throw away the rest of the line:
	कुरु
	{
	    tbuf[FGETS_SIZE - 2] = NUL;
	    vim_ignoredp = ख_माला_लो((वर्ण *)tbuf, FGETS_SIZE, fp);
	} यावत् (tbuf[FGETS_SIZE - 2] != NUL && tbuf[FGETS_SIZE - 2] != '\n');
    }
    प्रत्यागम (eof == रिक्त);
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 * Return -1 for failure, 0 for success.
 */
    पूर्णाङ्क
vim_rename(char_u *from, char_u *to)
{
    पूर्णाङ्क		n;
    पूर्णाङ्क		ret;
#ifdef AMIGA
    BPTR	flock;
#endif
    stat_T	st;
    पूर्णाङ्क		use_tmp_file = FALSE;

    /*
     * When the names are identical, there is nothing to do.  When they refer
     * to the same file (ignoring case and slash/backslash differences) but
     * the file name differs we need to go through a temp file.
     */
    यदि (fnamecmp(from, to) == 0)
    {
	यदि (p_fic && STRCMP(gettail(from), gettail(to)) != 0)
	    use_tmp_file = TRUE;
	अन्यथा
	    प्रत्यागम 0;
    }

    /*
     * Fail if the "from" file doesn't exist.  Avoids that "to" is deleted.
     */
    यदि (mch_stat((वर्ण *)from, &st) < 0)
	प्रत्यागम -1;

#ifdef UNIX
    {
	stat_T	st_to;

	// It's possible for the source and destination to be the same file.
	// This happens when "from" and "to" differ in case and are on a FAT32
	// filesystem.  In that case go through a temp file name.
	यदि (mch_stat((वर्ण *)to, &st_to) >= 0
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
	यदि (win32_fileinfo(from, &info1) == FILEINFO_OK
		&& win32_fileinfo(to, &info2) == FILEINFO_OK
		&& info1.dw_Volume_Serial_संख्या == info2.dw_Volume_Serial_संख्या
		&& info1.n_File_Index_ऊपरी == info2.n_File_Index_ऊपरी
		&& info1.n_File_Index_निचला == info2.n_File_Index_निचला)
	    use_tmp_file = TRUE;
    }
#endif

    यदि (use_tmp_file)
    {
	वर्ण	tempname[MAXPATHL + 1];

	/*
	 * Find a name that doesn't exist and is in the same directory.
	 * Rename "from" to "tempname" and then rename "tempname" to "to".
	 */
	यदि (STRLEN(from) >= MAXPATHL - 5)
	    प्रत्यागम -1;
	STRCPY(tempname, from);
	प्रति (n = 123; n < 99999; ++n)
	{
	    प्र_लिखो((वर्ण *)gettail((char_u *)tempname), "%d", n);
	    यदि (mch_stat(tempname, &st) < 0)
	    {
		यदि (mch_rename((वर्ण *)from, tempname) == 0)
		{
		    यदि (mch_rename(tempname, (वर्ण *)to) == 0)
			प्रत्यागम 0;
		    // Strange, the second step failed.  Try moving the
		    // file back and return failure.
		    (शून्य)mch_rename(tempname, (वर्ण *)from);
		    प्रत्यागम -1;
		}
		// If it fails for one temp name it will most likely fail
		// for any temp name, give up.
		प्रत्यागम -1;
	    }
	}
	प्रत्यागम -1;
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
    यदि (flock)
	UnLock(flock);
#endif

    /*
     * First try a normal rename, return if it works.
     */
    यदि (mch_rename((वर्ण *)from, (वर्ण *)to) == 0)
	प्रत्यागम 0;

    /*
     * Rename() failed, try copying the file.
     */
    ret = vim_copyfile(from, to);
    यदि (ret != OK)
	प्रत्यागम -1;

    /*
     * Remove copied original file
     */
    यदि (mch_stat((वर्ण *)from, &st) >= 0)
	mch_remove(from);

    प्रत्यागम 0;
}


/*
 * Create the new file with same permissions as the original.
 * Return FAIL for failure, OK for success.
 */
    पूर्णाङ्क
vim_copyfile(char_u *from, char_u *to)
{
    पूर्णाङ्क		fd_in;
    पूर्णाङ्क		fd_out;
    पूर्णाङ्क		n;
    वर्ण	*errmsg = रिक्त;
    वर्ण	*buffer;
    दीर्घ	perm;
#ifdef HAVE_ACL
    vim_acl_T	acl;		// ACL from original file
#endif

#ifdef HAVE_READLINK
    पूर्णाङ्क		ret;
    पूर्णाङ्क		len;
    stat_T	st;
    वर्ण	linkbuf[MAXPATHL + 1];

    ret = mch_lstat((वर्ण *)from, &st);
    यदि (ret >= 0 && S_ISLNK(st.st_mode))
    {
	ret = -1;

	len = readlink((वर्ण *)from, linkbuf, MAXPATHL);
	यदि (len > 0)
	{
	    linkbuf[len] = NUL;

	    // Create link
	    ret = symlink(linkbuf, (वर्ण *)to);
	}

	प्रत्यागम ret == 0 ? OK : FAIL;
    }
#endif

    perm = mch_getperm(from);
#ifdef HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    acl = mch_get_acl(from);
#endif
    fd_in = mch_open((वर्ण *)from, O_RDONLY|O_EXTRA, 0);
    यदि (fd_in == -1)
    {
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	प्रत्यागम FAIL;
    }

    // Create the new file with same permissions as the original.
    fd_out = mch_open((वर्ण *)to,
		       O_CREAT|O_EXCL|O_WRONLY|O_EXTRA|O_NOFOLLOW, (पूर्णाङ्क)perm);
    यदि (fd_out == -1)
    {
	बंद(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	प्रत्यागम FAIL;
    }

    buffer = alloc(WRITEBUFSIZE);
    यदि (buffer == रिक्त)
    {
	बंद(fd_out);
	बंद(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	प्रत्यागम FAIL;
    }

    यावत् ((n = read_eintr(fd_in, buffer, WRITEBUFSIZE)) > 0)
	यदि (write_eintr(fd_out, buffer, n) != n)
	{
	    errmsg = _(e_error_writing_to_str);
	    भङ्ग;
	}

    vim_free(buffer);
    बंद(fd_in);
    यदि (बंद(fd_out) < 0)
	errmsg = _(e_error_closing_str);
    यदि (n < 0)
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
#यदि defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    mch_copy_sec(from, to);
#endif
    यदि (errmsg != रिक्त)
    {
	semsg(errmsg, to);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

स्थिर पूर्णाङ्क already_warned = FALSE;

/*
 * Check if any not hidden buffer has been changed.
 * Postpone the check if there are characters in the stuff buffer, a global
 * command is being executed, a mapping is being executed or an autocommand is
 * busy.
 * Returns TRUE if some message was written (screen should be redrawn and
 * cursor positioned).
 */
    पूर्णाङ्क
check_timestamps(
    पूर्णाङ्क		focus)		// called for GUI focus event
{
    buf_T	*buf;
    पूर्णाङ्क		didit = 0;
    पूर्णाङ्क		n;

    // Don't check timestamps while system() or another low-level function may
    // cause us to lose and gain focus.
    यदि (no_check_timestamps > 0)
	प्रत्यागम FALSE;

    // Avoid doing a check twice.  The OK/Reload dialog can cause a focus
    // event and we would keep on checking if the file is steadily growing.
    // Do check again after typing something.
    यदि (focus && did_check_timestamps)
    {
	need_check_timestamps = TRUE;
	प्रत्यागम FALSE;
    }

    यदि (!stuff_empty() || global_busy || !typebuf_typed()
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
	    यदि (buf->b_nwindows > 0)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		n = buf_check_timestamp(buf, focus);
		यदि (didit < n)
		    didit = n;
		यदि (n > 0 && !bufref_valid(&bufref))
		{
		    // Autocommands have removed the buffer, start at the
		    // first one again.
		    buf = firstbuf;
		    अनुवर्त;
		}
	    }
	}
	--no_wait_return;
	need_check_timestamps = FALSE;
	यदि (need_wait_return && didit == 2)
	{
	    // make sure msg isn't overwritten
	    संदेश_puts("\n");
	    out_flush();
	}
    }
    प्रत्यागम didit;
}

/*
 * Move all the lines from buffer "frombuf" to buffer "tobuf".
 * Return OK or FAIL.  When FAIL "tobuf" is incomplete and/or "frombuf" is not
 * empty.
 */
    स्थिर पूर्णाङ्क
move_पंक्तियाँ(buf_T *frombuf, buf_T *tobuf)
{
    buf_T	*tbuf = curbuf;
    पूर्णाङ्क		retval = OK;
    linenr_T	lnum;
    char_u	*p;

    // Copy the lines in "frombuf" to "tobuf".
    curbuf = tobuf;
    प्रति (lnum = 1; lnum <= frombuf->b_ml.ml_पंक्ति_गिनती; ++lnum)
    {
	p = vim_strnsave(ml_get_buf(frombuf, lnum, FALSE), ml_get_buf_len(frombuf, lnum));
	यदि (p == रिक्त || ml_append(lnum - 1, p, 0, FALSE) == FAIL)
	{
	    vim_free(p);
	    retval = FAIL;
	    भङ्ग;
	}
	vim_free(p);
    }

    // Delete all the lines in "frombuf".
    यदि (retval != FAIL)
    {
	curbuf = frombuf;
	प्रति (lnum = curbuf->b_ml.ml_पंक्ति_गिनती; lnum > 0; --lnum)
	    यदि (ml_delete(lnum) == FAIL)
	    {
		// Oops!  We could try putting back the saved lines, but that
		// might fail again...
		retval = FAIL;
		भङ्ग;
	    }
    }

    curbuf = tbuf;
    प्रत्यागम retval;
}

/*
 * Check if buffer "buf" has been changed.
 * Also check if the file for a new buffer unexpectedly appeared.
 * return 1 if a changed buffer was found.
 * return 2 if a message has been displayed.
 * return 0 otherwise.
 */
    पूर्णाङ्क
buf_check_timestamp(
    buf_T	*buf,
    पूर्णाङ्क		focus UNUSED)	// called for GUI focus event
{
    stat_T	st;
    पूर्णाङ्क		stat_res;
    पूर्णाङ्क		retval = 0;
    वर्ण	*mesg = रिक्त;
    वर्ण	*mesg2 = "";
    पूर्णाङ्क		helpmesg = FALSE;
    गणना {
	RELOAD_NONE,
	RELOAD_NORMAL,
	RELOAD_DETECT
    }		reload = RELOAD_NONE;
#यदि defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
    पूर्णाङ्क		can_reload = FALSE;
#endif
    off_T	orig_size = buf->b_orig_size;
    पूर्णाङ्क		orig_mode = buf->b_orig_mode;
#ifdef FEAT_GUI
    पूर्णाङ्क		save_mouse_correct = need_mouse_correct;
#endif
    स्थिर पूर्णाङ्क	busy = FALSE;
    पूर्णाङ्क		n;
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    // If there is no file name, the buffer is not loaded, 'buftype' is
    // set, we are in the middle of a save or being called recursively: ignore
    // this buffer.
    यदि (buf->b_ffname == रिक्त
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
	प्रत्यागम 0;

    यदि (       !(buf->b_flags & BF_NOTEDITED)
	    && buf->b_mtime != 0
	    && ((stat_res = mch_stat((वर्ण *)buf->b_ffname, &st)) < 0
		|| time_differs(&st, buf->b_mtime, buf->b_mtime_ns)
		|| st.st_size != buf->b_orig_size
#ifdef HAVE_ST_MODE
		|| (पूर्णाङ्क)st.st_mode != buf->b_orig_mode
#अन्यथा
		|| mch_getperm(buf->b_ffname) != buf->b_orig_mode
#endif
		))
    {
	दीर्घ prev_b_mtime = buf->b_mtime;

	retval = 1;

	// set b_mtime to stop further warnings (e.g., when executing
	// FileChangedShell autocmd)
	यदि (stat_res < 0)
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
	यदि (mch_isdir(buf->b_fname))
	    ;

	/*
	 * If 'autoread' is set, the buffer has no changes and the file still
	 * exists, reload the buffer.  Use the buffer-local option value if it
	 * was set, the global option value otherwise.
	 */
	अन्यथा यदि ((buf->b_p_ar >= 0 ? buf->b_p_ar : p_ar)
				       && !bufIsChanged(buf) && stat_res >= 0)
	    reload = RELOAD_NORMAL;
	अन्यथा
	{
	    वर्ण    *reason;
#ifdef FEAT_EVAL
	    माप_प्रकार  reasonlen;
#endif

	    यदि (stat_res < 0)
	    {
		reason = "deleted";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("deleted");
#endif
	    }
	    अन्यथा यदि (bufIsChanged(buf))
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
	    अन्यथा यदि (orig_size != buf->b_orig_size || buf_contents_changed(buf))
	    {
		reason = "changed";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("changed");
#endif
	    }
	    अन्यथा यदि (orig_mode != buf->b_orig_mode)
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
	    set_vim_var_string(VV_FCS_REASON, (char_u *)reason, (पूर्णाङ्क)reasonlen);
	    set_vim_var_string(VV_FCS_CHOICE, (char_u *)"", 0);
#endif
	    ++allbuf_lock;
	    n = apply_autocmds(EVENT_FILECHANGEDSHELL,
				      buf->b_fname, buf->b_fname, FALSE, buf);
	    --allbuf_lock;
	    busy = FALSE;
	    यदि (n)
	    {
		यदि (!bufref_valid(&bufref))
		    emsg(_(e_filechangedshell_autocommand_deleted_buffer));
#ifdef FEAT_EVAL
		char_u	*s = get_vim_var_str(VV_FCS_CHOICE);
		यदि (STRCMP(s, "reload") == 0 && *reason != 'd')
		    reload = RELOAD_NORMAL;
		अन्यथा यदि (STRCMP(s, "edit") == 0)
		    reload = RELOAD_DETECT;
		अन्यथा यदि (STRCMP(s, "ask") == 0)
		    n = FALSE;
		अन्यथा
#endif
		    प्रत्यागम 2;
	    }
	    यदि (!n)
	    {
		यदि (*reason == 'd')
		{
		    // Only give the message once.
		    यदि (prev_b_mtime != -1)
			mesg = _(e_file_str_no_longer_available);
		}
		अन्यथा
		{
		    helpmesg = TRUE;
#यदि defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		    can_reload = TRUE;
#endif
		    यदि (reason[2] == 'n')
		    {
			mesg = _("W12: Warning: File \"%s\" has changed and the buffer was changed in Vim as well");
			mesg2 = _("See \":help W12\" for more info.");
		    }
		    अन्यथा यदि (reason[1] == 'h')
		    {
			mesg = _("W11: Warning: File \"%s\" has changed since editing started");
			mesg2 = _("See \":help W11\" for more info.");
		    }
		    अन्यथा यदि (*reason == 'm')
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
    अन्यथा यदि ((buf->b_flags & BF_NEW) && !(buf->b_flags & BF_NEW_W)
						&& vim_fexists(buf->b_ffname))
    {
	retval = 1;
	mesg = _("W13: Warning: File \"%s\" has been created after editing started");
	buf->b_flags |= BF_NEW_W;
#यदि defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	can_reload = TRUE;
#endif
    }

    यदि (mesg != रिक्त)
    {
	char_u *path;

	path = home_replace_save(buf, buf->b_fname);
	यदि (path != रिक्त)
	{
	    माप_प्रकार  tbufsize;
	    वर्ण    *tbuf;

	    यदि (!helpmesg)
		mesg2 = "";
	    tbufsize = STRLEN(mesg) + STRLEN(path) + 2 + STRLEN(mesg2) + 1; // +2 for either '\n' or "; "
									    // and +1 for NUL
	    tbuf = alloc(tbufsize);
	    यदि (tbuf != रिक्त)
	    {
		पूर्णाङ्क tbuflen;

		tbuflen = vim_snprintf(tbuf, tbufsize, mesg, path);
#ifdef FEAT_EVAL
		// Set warningmsg here, before the unimportant and output-specific
		// mesg2 has been appended.
		set_vim_var_string(VV_WARNINGMSG, (char_u *)tbuf, tbuflen);
#endif
#यदि defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		यदि (can_reload)
		{
		    यदि (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "\n%s", mesg2);
		    विकल्प (do_dialog(VIM_WARNING, (char_u *)_("Warning"),
			    (char_u *)tbuf,
			    (char_u *)_("&OK\n&Load File\nLoad File &and Options"),
			    1, रिक्त, TRUE))
		    {
			पक्ष 2:
			    reload = RELOAD_NORMAL;
			    भङ्ग;
			पक्ष 3:
			    reload = RELOAD_DETECT;
			    भङ्ग;
		    }
		}
		अन्यथा
#endif
		यदि (State > MODE_NORMAL_BUSY || (State & MODE_CMDLINE)
								 || already_warned)
		{
		    यदि (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "; %s", mesg2);
		    emsg(tbuf);
		    retval = 2;
		}
		अन्यथा
		{
		    यदि (!autocmd_busy)
		    {
			संदेश_start();
			संदेश_puts_attr(tbuf, HL_ATTR(HLF_E) + संदेश_HIST);
			यदि (*mesg2 != NUL)
			    संदेश_puts_attr(mesg2, HL_ATTR(HLF_W) + संदेश_HIST);
			संदेश_clr_eos();
			(शून्य)संदेश_end();
			यदि (emsg_silent == 0 && !in_assert_fails)
			{
			    out_flush();
#ifdef FEAT_GUI
			    यदि (!focus)
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

    यदि (reload != RELOAD_NONE)
    {
	// Reload the buffer.
	buf_reload(buf, orig_mode, reload == RELOAD_DETECT);
#ifdef FEAT_PERSISTENT_UNDO
	यदि (bufref_valid(&bufref) && buf->b_p_udf && buf->b_ffname != रिक्त)
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
    यदि (bufref_valid(&bufref) && retval != 0)
	(शून्य)apply_autocmds(EVENT_FILECHANGEDSHELLPOST,
				      buf->b_fname, buf->b_fname, FALSE, buf);
#ifdef FEAT_GUI
    // restore this in case an autocommand has set it; it would break
    // 'mousefocus'
    need_mouse_correct = save_mouse_correct;
#endif

    प्रत्यागम retval;
}

/*
 * Reload a buffer that is already loaded.
 * Used when the file was changed outside of Vim.
 * "orig_mode" is buf->b_orig_mode before the need for reloading was detected.
 * buf->b_orig_mode may have been reset already.
 */
    शून्य
buf_reload(buf_T *buf, पूर्णाङ्क orig_mode, पूर्णाङ्क reload_options)
{
    exarg_T	ea;
    pos_T	old_cursor;
    linenr_T	old_topline;
    पूर्णाङ्क		old_ro = buf->b_p_ro;
    buf_T	*savebuf;
    bufref_T	bufref;
    पूर्णाङ्क		saved = OK;
    aco_save_T	aco;
    पूर्णाङ्क		flags = READ_NEW;
    पूर्णाङ्क		prepped = OK;

    // Set curwin/curbuf for "buf" and save some things.
    aucmd_prepbuf(&aco, buf);
    यदि (curbuf != buf)
    {
	// Failed to find a window for "buf", it is dangerous to continue,
	// better bail out.
	प्रत्यागम;
    }

    // Unless reload_options is set, we only want to read the text from the
    // file, not reset the syntax highlighting, clear marks, diff status, etc.
    // Force the fileformat and encoding to be the same.
    यदि (reload_options)
	CLEAR_FIELD(ea);
    अन्यथा
	prepped = prep_exarg(&ea, buf);

    यदि (prepped == OK)
    {
	old_cursor = curwin->w_cursor;
	old_topline = curwin->w_topline;

	यदि (p_ur < 0 || curbuf->b_ml.ml_पंक्ति_गिनती <= p_ur)
	{
	    // Save all the text, so that the reload can be undone.
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    saved = u_savecommon(0, curbuf->b_ml.ml_पंक्ति_गिनती + 1, 0, TRUE);
	    flags |= READ_KEEP_UNDO;
	}

	/*
	 * To behave like when a new file is edited (matters for
	 * BufReadPost autocommands) we first need to delete the current
	 * buffer contents.  But if reading the file fails we should keep
	 * the old contents.  Can't use memory only, the file might be
	 * too big.  Use a hidden buffer to move the buffer contents to.
	 */
	यदि (BUFEMPTY() || saved == FAIL)
	    savebuf = रिक्त;
	अन्यथा
	{
	    // Allocate a buffer without putting it in the buffer list.
	    savebuf = buflist_new(रिक्त, रिक्त, (linenr_T)1, BLN_DUMMY);
	    set_bufref(&bufref, savebuf);
	    यदि (savebuf != रिक्त && buf == curbuf)
	    {
		// Open the memline.
		curbuf = savebuf;
		curwin->w_buffer = savebuf;
		saved = ml_open(curbuf);
		curbuf = buf;
		curwin->w_buffer = buf;
	    }
	    यदि (savebuf == रिक्त || saved == FAIL || buf != curbuf
				      || move_पंक्तियाँ(buf, savebuf) == FAIL)
	    {
		semsg(_(e_could_not_prepare_for_reloading_str), buf->b_fname);
		saved = FAIL;
	    }
	}

	यदि (saved == OK)
	{
	    पूर्णाङ्क old_संदेश_silent = संदेश_silent;

	    curbuf->b_flags |= BF_CHECK_RO;	// check for RO again
	    curbuf->b_keep_filetype = true;	// don't detect 'filetype'

	    यदि (shortmess(SHM_FILEINFO))
		संदेश_silent = 1;

	    यदि (readfile(buf->b_ffname, buf->b_fname, (linenr_T)0,
			(linenr_T)0,
			(linenr_T)MAXLNUM, &ea, flags) != OK)
	    {
#यदि defined(FEAT_EVAL)
		यदि (!aborting())
#endif
		    semsg(_(e_could_not_reload_str), buf->b_fname);
		यदि (savebuf != रिक्त && bufref_valid(&bufref) && buf == curbuf)
		{
		    // Put the text back from the save buffer.  First
		    // delete any lines that readfile() added.
		    यावत् (!BUFEMPTY())
			यदि (ml_delete(buf->b_ml.ml_पंक्ति_गिनती) == FAIL)
			    भङ्ग;
		    (शून्य)move_पंक्तियाँ(savebuf, buf);
		}
	    }
	    अन्यथा यदि (buf == curbuf)  // "buf" still valid
	    {
		// Mark the buffer as unmodified and free undo info.
		unchanged(buf, TRUE, TRUE);
		यदि ((flags & READ_KEEP_UNDO) == 0)
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

	यदि (savebuf != रिक्त && bufref_valid(&bufref))
	    wipe_buffer(savebuf, FALSE);

#ifdef FEAT_DIFF
	// Invalidate diff info if necessary.
	diff_invalidate(curbuf);
#endif

	// Restore the topline and cursor position and check it (lines may
	// have been removed).
	यदि (old_topline > curbuf->b_ml.ml_पंक्ति_गिनती)
	    curwin->w_topline = curbuf->b_ml.ml_पंक्ति_गिनती;
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
		यदि (wp->w_buffer == curwin->w_buffer
			&& !foldmethodIsManual(wp))
		    foldUpdateAll(wp);
	}
#endif
	// If the mode didn't change and 'readonly' was set, keep the old
	// value; the user probably used the ":view" command.  But don't
	// reset it, might have had a read error.
	यदि (orig_mode == curbuf->b_orig_mode)
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
    buf->b_orig_mode = (पूर्णाङ्क)st->st_mode;
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
    यदि (curbuf->b_no_eol_lnum != 0)	// only if there is a missing eol
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
    यदि (wp != रिक्त)
    {
	यदि (_wcsicmp(wp, L".exe") == 0 ||
		_wcsicmp(wp, L".com") == 0 ||
		_wcsicmp(wp, L".cmd") == 0 ||
		_wcsicmp(wp, L".bat") == 0)
	    st_mode |= _S_IEXEC;
    }

    // Copy user bits to group/other.
    st_mode |= (st_mode & 0700) >> 3;
    st_mode |= (st_mode & 0700) >> 6;

    st.st_mode = st_mode;
    प्रत्यागम getfpermst(&st, perm);
}

    स्थिर शून्य
getftypewfd(WIN32_FIND_DATAW *wfd, string_T *ret)
{
    DWORD flag = wfd->dwFileAttributes;
    DWORD tag = wfd->dwReserved0;

    यदि (flag & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	यदि (tag == IO_REPARSE_TAG_MOUNT_POINT)
	{
	    ret->string = (char_u *)"junction";
	    ret->length = STRLEN_LITERAL("junction");
	    प्रत्यागम;
	}
	यदि (tag == IO_REPARSE_TAG_SYMLINK)
	{
	    यदि (flag & FILE_ATTRIBUTE_DIRECTORY)
	    {
		ret->string = (char_u *)"linkd";
		ret->length = STRLEN_LITERAL("linkd");
		प्रत्यागम;
	    }

	    ret->string = (char_u *)"link";
	    ret->length = STRLEN_LITERAL("link");
	    प्रत्यागम;
	}

	// unknown reparse point type
	ret->string = (char_u *)"reparse";
	ret->length = STRLEN_LITERAL("reparse");
	प्रत्यागम;
    }

    यदि (flag & FILE_ATTRIBUTE_DIRECTORY)
    {
	ret->string = (char_u *)"dir";
	ret->length = STRLEN_LITERAL("dir");
	प्रत्यागम;
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
    यदि (item == रिक्त)
	प्रत्यागम रिक्त;
    item->dv_refcount++;

    p = utf16_to_enc(wfd->cFileName, रिक्त);
    यदि (p == रिक्त)
	गच्छ theend;
    यदि (dict_add_string(item, "name", p) == FAIL)
    {
	vim_free(p);
	गच्छ theend;
    }
    vim_free(p);

    size = (((varnumber_T)wfd->n_File_Size_ऊपरी) << 32) | wfd->n_File_Size_निचला;
    यदि (dict_add_संख्या(item, "size", size) == FAIL)
	गच्छ theend;

    // Convert FILETIME to unix time.
    समय = (((((varnumber_T)wfd->ftLastWriteTime.dw_ऊपरी_Date_Time) << 32) |
		wfd->ftLastWriteTime.dw_निचला_Date_Time)
	    - 116444736000000000) / 10000000;
    यदि (dict_add_संख्या(item, "time", समय) == FAIL)
	गच्छ theend;

    getftypewfd(wfd, &s);
    यदि (dict_add_string_len(item, "type", s.string, (पूर्णाङ्क)s.length) == FAIL)
	गच्छ theend;
    यदि (dict_add_string(item, "perm", getfpermwfd(wfd, permbuf)) == FAIL)
	गच्छ theend;

    यदि (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	गच्छ theend;
    यदि (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	गच्छ theend;

    प्रत्यागम item;

theend:
    dict_unref(item);
    प्रत्यागम रिक्त;
}
# अन्यथा
    स्थिर dict_T *
create_readdirex_item(char_u *path, char_u *name)
{
    dict_T	*item;
    वर्ण	*p;
    माप_प्रकार	pathlen, len;
    माप_प्रकार	namelen;
    stat_T	st;
    पूर्णाङ्क		ret, link = FALSE;
    varnumber_T	size;
    char_u	permbuf[] = "---------";
    string_T	q = {रिक्त, 0};
    संरचना passwd *pw;
    संरचना group  *gr;

    item = dict_alloc();
    यदि (item == रिक्त)
	प्रत्यागम रिक्त;
    item->dv_refcount++;

    pathlen = STRLEN(path);
    namelen = STRLEN(name);
    len = pathlen + 1 + namelen + 1;
    p = alloc(len);
    यदि (p == रिक्त)
	गच्छ theend;
    यदि (pathlen > 0 && path[pathlen - 1] == '/')
	vim_snprintf(p, len, "%s%s", path, name);
    अन्यथा
	vim_snprintf(p, len, "%s/%s", path, name);
    ret = mch_lstat(p, &st);
    यदि (ret >= 0 && S_ISLNK(st.st_mode))
    {
	link = TRUE;
	ret = mch_stat(p, &st);
	यदि (ret < 0)
	    STR_LITERAL_SET(q, "link");
    }
    vim_free(p);

    यदि (dict_add_string_len(item, "name", name, (पूर्णाङ्क)namelen) == FAIL)
	गच्छ theend;

    यदि (ret >= 0)
    {
	size = (varnumber_T)st.st_size;
	यदि (S_ISDIR(st.st_mode))
	    size = 0;
	// non-perfect check for overflow
	अन्यथा यदि ((off_T)size != (off_T)st.st_size)
	    size = -2;
	यदि (dict_add_संख्या(item, "size", size) == FAIL)
	    गच्छ theend;
	यदि (dict_add_संख्या(item, "time", (varnumber_T)st.st_mtime) == FAIL)
	    गच्छ theend;

	यदि (link)
	{
	    यदि (S_ISDIR(st.st_mode))
		STR_LITERAL_SET(q, "linkd");
	    अन्यथा
		STR_LITERAL_SET(q, "link");
	}
	अन्यथा
	{
	    q.string = getftypest(&st);
	    q.length = STRLEN(q.string);
	}
	यदि (dict_add_string_len(item, "type", q.string, (पूर्णाङ्क)q.length) == FAIL)
	    गच्छ theend;
	यदि (dict_add_string(item, "perm", getfpermst(&st, permbuf)) == FAIL)
	    गच्छ theend;

	pw = getpwuid(st.st_uid);
	यदि (pw == रिक्त)
	    STR_LITERAL_SET(q, "");
	अन्यथा
	{
	    q.string = (char_u *)pw->pw_name;
	    q.length = STRLEN(q.string);
	}
	यदि (dict_add_string_len(item, "user", q.string, (पूर्णाङ्क)q.length) == FAIL)
	    गच्छ theend;
#  यदि !defined(VMS) || (defined(VMS) && defined(HAVE_XOS_R_H))
	gr = getgrgid(st.st_gid);
	यदि (gr == रिक्त)
	    STR_LITERAL_SET(q, "");
	अन्यथा
	{
	    q.string = (char_u *)gr->gr_name;
	    q.length = STRLEN(q.string);
	}
#  endif
	यदि (dict_add_string_len(item, "group", q.string, (पूर्णाङ्क)q.length) == FAIL)
	    गच्छ theend;
    }
    अन्यथा
    {
	यदि (dict_add_संख्या(item, "size", -1) == FAIL)
	    गच्छ theend;
	यदि (dict_add_संख्या(item, "time", -1) == FAIL)
	    गच्छ theend;
	यदि (q.string == रिक्त)
	{
	    यदि (dict_add_string_len(item, "type", (char_u *)"", 0) == FAIL)
		गच्छ theend;
	}
	अन्यथा
	{
	    यदि (dict_add_string_len(item, "type", q.string, (पूर्णाङ्क)q.length) == FAIL)
		गच्छ theend;
	}
	यदि (dict_add_string_len(item, "perm", (char_u *)"", 0) == FAIL)
	    गच्छ theend;
	यदि (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	    गच्छ theend;
	यदि (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	    गच्छ theend;
    }
    प्रत्यागम item;

theend:
    dict_unref(item);
    प्रत्यागम रिक्त;
}
# endif

    स्थिर पूर्णाङ्क
compare_readdirex_item(ध्रुव शून्य *p1, ध्रुव शून्य *p2)
{
    char_u  *name1, *name2;

    name1 = dict_get_string(*(dict_T**)p1, "name", FALSE);
    name2 = dict_get_string(*(dict_T**)p2, "name", FALSE);
    यदि (readdirex_sort == READDIR_SORT_BYTE)
	प्रत्यागम STRCMP(name1, name2);
    यदि (readdirex_sort == READDIR_SORT_IC)
	प्रत्यागम STRICMP(name1, name2);

    प्रत्यागम STRCOLL(name1, name2);
}

    स्थिर पूर्णाङ्क
compare_readdir_item(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    यदि (readdirex_sort == READDIR_SORT_BYTE)
	प्रत्यागम STRCMP(*(वर्ण **)s1, *(वर्ण **)s2);
    यदि (readdirex_sort == READDIR_SORT_IC)
	प्रत्यागम STRICMP(*(वर्ण **)s1, *(वर्ण **)s2);

    प्रत्यागम STRCOLL(*(वर्ण **)s1, *(वर्ण **)s2);
}
#endif

#यदि defined(TEMPDIRNAMES) || defined(FEAT_EVAL)
/*
 * Core part of "readdir()" and "readdirex()" function.
 * Retrieve the list of files/directories of "path" into "gap".
 * If "withattr" is TRUE, retrieve the names and their attributes.
 * If "withattr" is FALSE, retrieve the names only.
 * Return OK for success, FAIL for failure.
 */
    पूर्णाङ्क
readdir_core(
    garray_T	*gap,
    char_u	*path,
    पूर्णाङ्क		withattr UNUSED,
    शून्य	*context,
    पूर्णाङ्क		(*checkitem)(शून्य *context, शून्य *item),
    पूर्णाङ्क		sort)
{
    पूर्णाङ्क			failed = FALSE;
    char_u		*p;
# ifdef MSWIN
    char_u		*buf;
    पूर्णाङ्क			ok;
    HANDLE		hFind = INVALID_HANDLE_मूल्य;
    WIN32_FIND_DATAW    wfd;
    WCHAR		*wn = रिक्त;	// UTF-16 name, NULL when not used.
    char_u		*p_end;
# अन्यथा
    सूची			*dirp;
    संरचना dirent	*dp;
# endif

    ga_init2(gap, परिमाण(शून्य *), 20);

# ifdef FEAT_EVAL
#  define FREE_ITEM(item)   कुरु { \
	यदि (withattr) \
	    dict_unref((dict_T*)(item)); \
	अन्यथा \
	    vim_free(item); \
    } यावत् (0)

    readdirex_sort = READDIR_SORT_BYTE;
# अन्यथा
#  define FREE_ITEM(item)   vim_free(item)
# endif

# ifdef MSWIN
    buf = alloc(MAXPATHL);
    यदि (buf == रिक्त)
	प्रत्यागम FAIL;
    STRNCPY(buf, path, MAXPATHL-5);
    p = p_end = buf + STRLEN(buf);
    MB_PTR_BACK(buf, p);
    यदि (*p == '\\' || *p == '/')
	p_end = p;
    STRCPY(p_end, "\\*");

    wn = enc_to_utf16(buf, रिक्त);
    यदि (wn != रिक्त)
	hFind = FindFirstFileW(wn, &wfd);
    ok = (hFind != INVALID_HANDLE_मूल्य);
    यदि (!ok)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    अन्यथा
    {
	यावत् (ok)
	{
	    पूर्णाङ्क	    ignore;
	    शून्य    *item;
	    WCHAR   *wp;

	    wp = wfd.cFileName;
	    ignore = wp[0] == L'.' &&
		    (wp[1] == NUL ||
		     (wp[1] == L'.' && wp[2] == NUL));
	    यदि (ignore)
	    {
		ok = FindNextFileW(hFind, &wfd);
		अनुवर्त;
	    }
#  ifdef FEAT_EVAL
	    यदि (withattr)
		item = (शून्य*)create_readdirex_item(&wfd);
	    अन्यथा
#  endif
		item = (शून्य*)utf16_to_enc(wfd.cFileName, रिक्त);
	    यदि (item == रिक्त)
	    {
		failed = TRUE;
		भङ्ग;
	    }

	    यदि (!ignore && checkitem != रिक्त)
	    {
		पूर्णाङ्क r = checkitem(context, item);

		यदि (r < 0)
		{
		    FREE_ITEM(item);
		    भङ्ग;
		}
		यदि (r == 0)
		    ignore = TRUE;
	    }

	    यदि (!ignore)
	    {
		यदि (ga_grow(gap, 1) == OK)
		    ((शून्य**)gap->ga_data)[gap->ga_len++] = item;
		अन्यथा
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    भङ्ग;
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
    dirp = सूची_खोलो((वर्ण *)path);
    यदि (dirp == रिक्त)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    अन्यथा
    {
	प्रति (;;)
	{
	    पूर्णाङ्क	    ignore;
	    शून्य    *item;

	    dp = सूची_पढ़ो(dirp);
	    यदि (dp == रिक्त)
		भङ्ग;
	    p = (char_u *)dp->d_name;

	    ignore = p[0] == '.' &&
		    (p[1] == NUL ||
		     (p[1] == '.' && p[2] == NUL));
	    यदि (ignore)
		अनुवर्त;
#  ifdef FEAT_EVAL
	    यदि (withattr)
		item = (शून्य*)create_readdirex_item(path, p);
	    अन्यथा
#  endif
		item = (शून्य*)vim_strsave(p);
	    यदि (item == रिक्त)
	    {
		failed = TRUE;
		भङ्ग;
	    }

	    यदि (checkitem != रिक्त)
	    {
		पूर्णाङ्क r = checkitem(context, item);

		यदि (r < 0)
		{
		    FREE_ITEM(item);
		    भङ्ग;
		}
		यदि (r == 0)
		    ignore = TRUE;
	    }

	    यदि (!ignore)
	    {
		यदि (ga_grow(gap, 1) == OK)
		    ((शून्य**)gap->ga_data)[gap->ga_len++] = item;
		अन्यथा
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    भङ्ग;
		}
	    }
	    अन्यथा
		FREE_ITEM(item);
	}

	बंद_सूची(dirp);
    }
# endif	// MSWIN

# undef FREE_ITEM

    यदि (!failed && gap->ga_len > 0 && sort > READDIR_SORT_NONE)
    {
# ifdef FEAT_EVAL
	readdirex_sort = sort;
	यदि (withattr)
	    क्विक((शून्य*)gap->ga_data, (माप_प्रकार)gap->ga_len, परिमाण(dict_T*),
		    compare_readdirex_item);
	अन्यथा
	    क्विक((शून्य*)gap->ga_data, (माप_प्रकार)gap->ga_len, परिमाण(char_u *),
		    compare_readdir_item);
# अन्यथा
	    sort_strings((char_u **)gap->ga_data, gap->ga_len);
# endif
    }

    प्रत्यागम failed ? FAIL : OK;
}

/*
 * Delete "name" and everything in it, recursively.
 * return 0 for success, -1 if some file was not deleted.
 */
    पूर्णाङ्क
delete_recursive(char_u *name)
{
    पूर्णाङ्क result = 0;

    // A symbolic link to a directory itself is deleted, not the directory it
    // points to.
    यदि (
# यदि defined(UNIX) || defined(MSWIN)
	 mch_isrealdir(name)
# अन्यथा
	 mch_isdir(name)
# endif
	    )
    {
	char_u	    *exp = vim_strsave(name);
	garray_T    ga;

	यदि (exp == रिक्त)
	    प्रत्यागम -1;
	यदि (readdir_core(&ga, exp, FALSE, रिक्त, रिक्त, READDIR_SORT_NONE) == OK)
	{
	    पूर्णाङ्क	len = vim_snprintf((वर्ण *)NameBuff, MAXPATHL, "%s/", exp);
	    पूर्णाङ्क	i;

	    प्रति (i = 0; i < ga.ga_len; ++i)
	    {
		vim_snprintf((वर्ण *)NameBuff + len, MAXPATHL - len, "%s", ((char_u **)ga.ga_data)[i]);
		यदि (delete_recursive(NameBuff) != 0)
		    // Remember the failure but continue deleting any further
		    // entries.
		    result = -1;
	    }
	    ga_clear_strings(&ga);
	    यदि (mch_rmdir(exp) != 0)
		result = -1;
	}
	अन्यथा
	    result = -1;
	vim_free(exp);
    }
    अन्यथा
	result = mch_remove(name) == 0 ? 0 : -1;

    प्रत्यागम result;
}
#endif

#यदि defined(TEMPDIRNAMES)
स्थिर दीर्घ	temp_गिनती = 0;		// Temp filename counter.

# यदि defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
/*
 * Open temporary directory and take file lock to prevent
 * to be auto-cleaned.
 */
   स्थिर शून्य
vim_opentempdir(शून्य)
{
    सूची *dp = रिक्त;

    यदि (vim_tempdir_dp != रिक्त)
	प्रत्यागम;

    dp = सूची_खोलो((ध्रुव वर्ण*)vim_tempdir);
    यदि (dp == रिक्त)
	प्रत्यागम;

    vim_tempdir_dp = dp;
    flock(dirfd(vim_tempdir_dp), LOCK_SH);
}

/*
 * Close temporary directory - it automatically release file lock.
 */
   स्थिर शून्य
vim_closetempdir(शून्य)
{
    यदि (vim_tempdir_dp == रिक्त)
	प्रत्यागम;

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

    यदि (vim_tempdir_dp == रिक्त)
	प्रत्यागम false;
    प्रत्यागम ख_स्थिति(dirfd(vim_tempdir_dp), &st) < 0 || st.st_nlink == 0;
}
# endif

/*
 * Delete the temp directory and all files it contains.
 */
    शून्य
vim_deltempdir(शून्य)
{
    यदि (vim_tempdir == रिक्त)
	प्रत्यागम;

# यदि defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
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
    यदि (buf == रिक्त)
	प्रत्यागम;

    यदि (vim_FullName(tempdir, buf, MAXPATHL, FALSE) == FAIL)
	STRCPY(buf, tempdir);
    buflen = STRLEN(buf);
    यदि (!after_pathsep(buf, buf + buflen))
    {
	STRCPY(buf + buflen, PATHSEPSTR);
	buflen += परिमाण(PATHSEP);
    }
    vim_tempdir = vim_strnsave(buf, buflen);
# यदि defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
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
    पूर्णाङ्क	    extra_char UNUSED,  // char to use in the name instead of '?'
    पूर्णाङ्क	    keep UNUSED)
{
#ifdef USE_TMPNAM
    char_u	itmp[ब_क्षणिक];	// use tmpnam()
#नोचेद्यदि defined(MSWIN)
    WCHAR	itmp[TEMPNAMELEN];
#अन्यथा
    char_u	itmp[TEMPNAMELEN];
#endif

#ifdef TEMPDIRNAMES
    स्थिर वर्ण	*(tempdirs[]) = {TEMPDIRNAMES};
    पूर्णाङ्क		i;
# ifndef EEXIST
    stat_T	st;
# endif

# यदि defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    // if the temp directory is gone, force re-creation of it
    यदि (vim_tempdir != रिक्त && vim_tempdir_gone())
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
    यदि (vim_tempdir == रिक्त)
    {
	/*
	 * Try the entries in TEMPDIRNAMES to create the temp directory.
	 */
	प्रति (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(tempdirs); ++i)
	{
# ifndef HAVE_MKDTEMP
	    दीर्घ	nr;
	    दीर्घ	off;
# endif
	    माप_प्रकार	itmplen;

	    // Expand $TMP, leave room for "/v1100000/999999999".
	    // Skip the directory check if the expansion fails.
	    itmplen = expand_env((char_u *)tempdirs[i], itmp, TEMPNAMELEN - 20);
	    यदि (itmp[0] != '$' && mch_isdir(itmp))
	    {
		// directory exists
		यदि (!after_pathsep(itmp, itmp + itmplen))
		{
		    STRCPY(itmp + itmplen, PATHSEPSTR);
		    itmplen += परिमाण(PATHSEP);
		}

# ifdef HAVE_MKDTEMP
		{
#  यदि defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    mode_t	umask_save = umask(077);
#  endif
		    // Leave room for filename
		    STRCPY(itmp + itmplen, "vXXXXXX");
		    itmplen += STRLEN_LITERAL("vXXXXXX");
		    यदि (mkdtemp((वर्ण *)itmp) != रिक्त)
			vim_settempdir(itmp);
#  यदि defined(UNIX) || defined(VMS)
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
		प्रति (off = 0; off < 10000L; ++off)
		{
		    पूर्णाङ्क		r;
#  यदि defined(UNIX) || defined(VMS)
		    mode_t	umask_save;
#  endif

		    vim_snprintf((वर्ण *)itmp + itmplen, परिमाण(itmp) - itmplen, "v%ld", nr + off);
#  ifndef EEXIST
		    // If mkdir() does not set errno to EEXIST, check for
		    // existing file here.  There is a race condition then,
		    // although it's fail-safe.
		    यदि (mch_stat((वर्ण *)itmp, &st) >= 0)
			अनुवर्त;
#  endif
#  यदि defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    umask_save = umask(077);
#  endif
		    r = vim_mkdir(itmp, 0700);
#  यदि defined(UNIX) || defined(VMS)
		    (शून्य)umask(umask_save);
#  endif
		    यदि (r == 0)
		    {
			vim_settempdir(itmp);
			भङ्ग;
		    }
#  ifdef EEXIST
		    // If the mkdir() didn't fail because the file/dir exists,
		    // we probably can't create any dir here, try another
		    // place.
		    यदि (त्रुटि_सं != EEXIST)
#  endif
			भङ्ग;
		}
# endif // HAVE_MKDTEMP
		यदि (vim_tempdir != रिक्त)
		    भङ्ग;
	    }
	}
    }

    यदि (vim_tempdir != रिक्त)
    {
	// There is no need to check if the file exists, because we own the
	// directory and nobody else creates a file in it.
	पूर्णाङ्क itmplen = vim_snprintf((वर्ण *)itmp, परिमाण(itmp), "%s%ld", vim_tempdir, temp_गिनती++);
	प्रत्यागम vim_strnsave(itmp, (माप_प्रकार)itmplen);
    }

    प्रत्यागम रिक्त;

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
    यदि (GetTempPathW(_MAX_PATH, wszTempFile) == 0)
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
    यदि (GetTempFileNameW(wszTempFile, buf4, 0, itmp) == 0)
	प्रत्यागम रिक्त;
    यदि (!keep)
	// GetTempFileNameW() will create the file, we don't want that
	(शून्य)DeleteFileW(itmp);

    // Backslashes in a temp file name cause problems when filtering with
    // "sh".  NOTE: This also checks 'shellcmdflag' to help those people who
    // didn't set 'shellslash' but only if not using PowerShell.
    retval = utf16_to_enc(itmp, रिक्त);
    यदि (retval == रिक्त)
	प्रत्यागम रिक्त;
    shname = gettail(p_sh);
    यदि ((*p_shcf == '-' && !(म_माला((वर्ण *)shname, "powershell") != रिक्त
			     || म_माला((वर्ण *)shname, "pwsh") != रिक्त ))
								    || p_ssl)
	प्रति (p = retval; *p; ++p)
	    यदि (*p == '\\')
		*p = '/';
    प्रत्यागम retval;

# अन्यथा // MSWIN
    पूर्णाङ्क		itmplen;

#  ifdef USE_TMPNAM
    // tmpnam() will make its own name
    p = क्षणिक((वर्ण *)itmp);
    यदि (p == रिक्त || *p == NUL)
	प्रत्यागम रिक्त;
#  अन्यथा

#   ifdef VMS_TEMPNAM
    // mktemp() is not working on VMS.  It seems to be
    // a do-nothing function. Therefore we use tempnam().
    vim_snprintf((वर्ण *)itmp, परिमाण(itmp), "VIM%c", extra_char);
    p = (char_u *)tempnam("tmp:", (वर्ण *)itmp);
    यदि (p != रिक्त)
    {
	// VMS will use '.LIS' if we don't explicitly specify an extension,
	// and VIM will then be unable to find the file later
	itmplen = vim_snprintf((वर्ण *)itmp, परिमाण(itmp), "%s.txt", p);
	मुक्त(p);
    }
    अन्यथा
	प्रत्यागम रिक्त;
#   अन्यथा
    STRCPY(itmp, TEMPNAME);
    itmplen = STRLEN_LITERAL(TEMPNAME);
    यदि ((p = vim_strchr(itmp, '?')) != रिक्त)
	*p = extra_char;
    यदि (mktemp((वर्ण *)itmp) == रिक्त)
	प्रत्यागम रिक्त;
#   endif
#  endif

    प्रत्यागम vim_strnsave(itmp, (माप_प्रकार)itmplen);
# endif // MSWIN
#endif // TEMPDIRNAMES
}

#यदि defined(BACKSLASH_IN_FILENAME)
/*
 * Convert all backslashes in fname to forward slashes in-place, unless when
 * it looks like a URL.
 */
    शून्य
forward_slash(char_u *fname)
{
    char_u	*p;

    यदि (path_with_url(fname))
	प्रत्यागम;
    प्रति (p = fname; *p != NUL; ++p)
	// The Big5 encoding can have '\' in the trail byte.
	यदि (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
	    ++p;
	अन्यथा यदि (*p == '\\')
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
    पूर्णाङ्क
match_file_pat(
    char_u	*pattern,		// pattern to match with
    regprog_T	**prog,			// pre-compiled regprog or NULL
    char_u	*fname,			// full path of file name
    char_u	*sfname,		// short file name or NULL
    char_u	*tail,			// tail of path
    पूर्णाङ्क		allow_dirs)		// allow matching with dir
{
    regmatch_T	regmatch;
    पूर्णाङ्क		result = FALSE;

    regmatch.rm_ic = p_fic; // ignore case if 'fileignorecase' is set
    यदि (prog != रिक्त)
	regmatch.regprog = *prog;
    अन्यथा
	regmatch.regprog = vim_regcomp(pattern, RE_MAGIC);

    /*
     * Try for a match with the pattern with:
     * 1. the full file name, when the pattern has a '/'.
     * 2. the short file name, when the pattern has a '/'.
     * 3. the tail of the file name, when the pattern has no '/'.
     */
    यदि (regmatch.regprog != रिक्त
	     && ((allow_dirs
		     && (vim_regexec(&regmatch, fname, (colnr_T)0)
			 || (sfname != रिक्त
			     && vim_regexec(&regmatch, sfname, (colnr_T)0))))
		 || (!allow_dirs && vim_regexec(&regmatch, tail, (colnr_T)0))))
	result = TRUE;

    यदि (prog != रिक्त)
	*prog = regmatch.regprog;
    अन्यथा
	vim_regfree(regmatch.regprog);
    प्रत्यागम result;
}

/*
 * Return TRUE if a file matches with a pattern in "list".
 * "list" is a comma-separated list of patterns, like 'wildignore'.
 * "sfname" is the short file name or NULL, "ffname" the long file name.
 */
    पूर्णाङ्क
match_file_list(char_u *list, char_u *sfname, char_u *ffname)
{
    char_u	buf[MAXPATHL];
    char_u	*tail;
    char_u	*regpat;
    वर्ण	allow_dirs;
    पूर्णाङ्क		match;
    char_u	*p;

    tail = gettail(sfname);

    // try all patterns in 'wildignore'
    p = list;
    यावत् (*p)
    {
	copy_option_part(&p, buf, MAXPATHL, ",");
	regpat = file_pat_to_reg_pat(buf, रिक्त, &allow_dirs, FALSE);
	यदि (regpat == रिक्त)
	    भङ्ग;
	match = match_file_pat(regpat, रिक्त, ffname, sfname,
						       tail, (पूर्णाङ्क)allow_dirs);
	vim_free(regpat);
	यदि (match)
	    प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
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
    वर्ण	*allow_dirs,	// Result passed back out in here
    पूर्णाङ्क		no_bslash UNUSED) // Don't use a backward slash as pathsep
{
    पूर्णाङ्क		size = 2; // '^' at start, '$' at end
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    पूर्णाङ्क		i;
    पूर्णाङ्क		nested = 0;
    पूर्णाङ्क		add_dollar = TRUE;

    यदि (allow_dirs != रिक्त)
	*allow_dirs = FALSE;
    यदि (pat_end == रिक्त)
	pat_end = pat + STRLEN(pat);

    प्रति (p = pat; p < pat_end; p++)
    {
	विकल्प (*p)
	{
	    पक्ष '*':
	    पक्ष '.':
	    पक्ष ',':
	    पक्ष '{':
	    पक्ष '}':
	    पक्ष '~':
		size += 2;	// extra backslash
		भङ्ग;
#ifdef BACKSLASH_IN_FILENAME
	    पक्ष '\\':
	    पक्ष '/':
		size += 4;	// could become "[\/]"
		भङ्ग;
#endif
	    स्वभाव:
		size++;
		यदि (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		{
		    ++p;
		    ++size;
		}
		भङ्ग;
	}
    }
    reg_pat = alloc(size + 1);
    यदि (reg_pat == रिक्त)
	प्रत्यागम रिक्त;

    i = 0;

    यदि (pat[0] == '*')
	यावत् (pat[0] == '*' && pat < pat_end - 1)
	    pat++;
    अन्यथा
	reg_pat[i++] = '^';
    endp = pat_end - 1;
    यदि (endp >= pat && *endp == '*')
    {
	यावत् (endp - pat > 0 && *endp == '*')
	    endp--;
	add_dollar = FALSE;
    }
    प्रति (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	विकल्प (*p)
	{
	    पक्ष '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		यावत् (p[1] == '*')	// "**" matches like "*"
		    ++p;
		भङ्ग;
	    पक्ष '.':
	    पक्ष '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		भङ्ग;
	    पक्ष '?':
		reg_pat[i++] = '.';
		भङ्ग;
	    पक्ष '\\':
		यदि (p[1] == NUL)
		    भङ्ग;
#ifdef BACKSLASH_IN_FILENAME
		यदि (!no_bslash)
		{
		    // translate:
		    // "\x" to "\\x"  e.g., "dir\file"
		    // "\*" to "\\.*" e.g., "dir\*.c"
		    // "\?" to "\\."  e.g., "dir\??.c"
		    // "\+" to "\+"   e.g., "fileX\+.c"
		    यदि ((vim_isfilec(p[1]) || p[1] == '*' || p[1] == '?')
			    && p[1] != '+')
		    {
			reg_pat[i++] = '[';
			reg_pat[i++] = '\\';
			reg_pat[i++] = '/';
			reg_pat[i++] = ']';
			यदि (allow_dirs != रिक्त)
			    *allow_dirs = TRUE;
			भङ्ग;
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
		यदि (*++p == '?'
#ifdef BACKSLASH_IN_FILENAME
			&& no_bslash
#endif
			)
		    reg_pat[i++] = '?';
		अन्यथा
		    यदि (*p == ',' || *p == '%' || *p == '#'
			       || vim_isspace(*p) || *p == '{' || *p == '}')
			reg_pat[i++] = *p;
		    अन्यथा यदि (*p == '\\' && p[1] == '\\' && p[2] == '{')
		    {
			reg_pat[i++] = '\\';
			reg_pat[i++] = '{';
			p += 2;
		    }
		    अन्यथा
		    {
			यदि (allow_dirs != रिक्त && vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
				&& (!no_bslash || *p != '\\')
#endif
				)
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			यदि (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
			    reg_pat[i++] = *p++;
			reg_pat[i++] = *p;
		    }
		भङ्ग;
#ifdef BACKSLASH_IN_FILENAME
	    पक्ष '/':
		reg_pat[i++] = '[';
		reg_pat[i++] = '\\';
		reg_pat[i++] = '/';
		reg_pat[i++] = ']';
		यदि (allow_dirs != रिक्त)
		    *allow_dirs = TRUE;
		भङ्ग;
#endif
	    पक्ष '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		भङ्ग;
	    पक्ष '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		भङ्ग;
	    पक्ष ',':
		यदि (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		अन्यथा
		    reg_pat[i++] = ',';
		भङ्ग;
	    स्वभाव:
		यदि (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		    reg_pat[i++] = *p++;
		अन्यथा यदि (allow_dirs != रिक्त && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		भङ्ग;
	}
    }
    यदि (add_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    यदि (nested != 0)
    {
	यदि (nested < 0)
	    emsg(_(e_missing_open_curly));
	अन्यथा
	    emsg(_(e_missing_close_curly));
	VIM_CLEAR(reg_pat);
    }
    प्रत्यागम reg_pat;
}

#यदि defined(EINTR)
/*
 * Version of read() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    दीर्घ
read_eintr(पूर्णाङ्क fd, शून्य *buf, माप_प्रकार bufsize)
{
    दीर्घ ret;

    प्रति (;;)
    {
	ret = vim_read(fd, buf, bufsize);
	यदि (ret >= 0 || त्रुटि_सं != EINTR)
	    भङ्ग;
    }
    प्रत्यागम ret;
}

/*
 * Version of write() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    दीर्घ
write_eintr(पूर्णाङ्क fd, शून्य *buf, माप_प्रकार bufsize)
{
    दीर्घ    ret = 0;
    दीर्घ    wlen;

    // Repeat the write() so long it didn't fail, other than being interrupted
    // by a signal.
    यावत् (ret < (दीर्घ)bufsize)
    {
	wlen = vim_write(fd, (वर्ण *)buf + ret, bufsize - ret);
	यदि (wlen < 0)
	{
	    यदि (त्रुटि_सं != EINTR)
		भङ्ग;
	}
	अन्यथा
	    ret += wlen;
    }
    प्रत्यागम ret;
}
#endif
