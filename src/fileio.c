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

#অন্তর্ভাব "vim.h"

#যদি defined(__TANDEM)
# include <limits.h>		// for SSIZE_MAX
#endif
#যদি (defined(UNIX) || defined(VMS)) && defined(FEAT_EVAL)
# include <pwd.h>
# include <grp.h>
#endif
#যদি defined(VMS) && defined(HAVE_XOS_R_H)
# include <x11/xos_r.h>
#endif

// Is there any system that doesn't have access()?
#define USE_MCH_ACCESS

// Bitmask with 0x80 set in each byte of a long_u word, used to detect
// non-ASCII bytes (high bit set) in multiple bytes at once.
#define NONASCII_MASK (((long_u)-1 / 0xFF) * 0x80)

#যদি defined(__hpux) && !defined(HAVE_DIRFD)
# define dirfd(x) ((x)->__dd_fd)
# define HAVE_DIRFD
#endif

স্থির char_u *পরবর্তী_fenc(char_u **pp, পূর্ণসংখ্যা *alloced);
#ifdef FEAT_EVAL
স্থির char_u *readfile_charconvert(char_u *fname, char_u *fenc, পূর্ণসংখ্যা *fdp);
#endif
#ifdef FEAT_CRYPT
স্থির char_u *check_for_cryptkey(char_u *cryptkey, char_u *ptr, long *sizep, off_T *filesizep, পূর্ণসংখ্যা newfile, char_u *fname, পূর্ণসংখ্যা *did_ask);
#endif
স্থির linenr_T readfile_linenr(linenr_T linecnt, char_u *p, char_u *endp);
স্থির char_u *check_for_bom(char_u *p, long আকার_2, পূর্ণসংখ্যা *lenp, পূর্ণসংখ্যা flags);

#ifdef FEAT_EVAL
স্থির পূর্ণসংখ্যা readdirex_sort;
#endif

    শূন্য
filemess(
    buf_T	*buf,
    char_u	*name,
    char_u	*s,
    পূর্ণসংখ্যা		attr)
{
    পূর্ণসংখ্যা		বার্তা_scroll_save;
    পূর্ণসংখ্যা		পূর্ব_বার্তা_col = বার্তা_col;
    size_t	len;

    যদি (বার্তা_silent != 0)
	ফেরত;
    বার্তা_add_fname(buf, name);	    // put file name in IObuff with quotes

    // If it's extremely long, truncate it.
    len = STRLEN(IObuff);
    যদি (len > IOSIZE - 100)
    {
	len = IOSIZE - 100;
	IObuff[len] = NUL;
    }

    // Avoid an over-long translation to cause trouble.
    যদি (*s != NUL)
	STRNCPY(IObuff + len, s, 99);

    /*
     * For the first message may have to start a new line.
     * For further ones overwrite the previous one, reset msg_scroll before
     * calling filemess().
     */
    বার্তা_scroll_save = বার্তা_scroll;
    যদি (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	বার্তা_scroll = FALSE;
    যদি (!বার্তা_scroll)	// wait a bit when overwriting an error msg
	check_for_delay(FALSE);
    বার্তা_start();
    যদি (পূর্ব_বার্তা_col != 0 && বার্তা_col == 0)
	বার্তা_putchar('\r');  // overwrite any previous message.
    বার্তা_scroll = বার্তা_scroll_save;
    বার্তা_scrolled_ign = TRUE;
    // may truncate the message to avoid a hit-return prompt
    বার্তা_outtrans_attr(বার্তা_may_trunc(FALSE, IObuff), attr);
    বার্তা_clr_eos();
    out_flush();
    বার্তা_scrolled_ign = FALSE;
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
    পূর্ণসংখ্যা
readfile(
    char_u	*fname,
    char_u	*sfname,
    linenr_T	from,
    linenr_T	সারিগুলি_to_skip,
    linenr_T	সারিগুলি_to_read,
    exarg_T	*eap,			// can be NULL!
    পূর্ণসংখ্যা		flags)
{
    পূর্ণসংখ্যা		retval = FAIL;	// jump to "theend" instead of returning
    পূর্ণসংখ্যা		fd = 0;
    পূর্ণসংখ্যা		newfile = (flags & READ_NEW);
    পূর্ণসংখ্যা		check_readonly;
    পূর্ণসংখ্যা		filtering = (flags & READ_FILTER);
    পূর্ণসংখ্যা		read_stdin = (flags & READ_STDIN);
    পূর্ণসংখ্যা		read_বাফার = (flags & READ_বাফার);
    পূর্ণসংখ্যা		read_fifo = (flags & READ_FIFO);
    পূর্ণসংখ্যা		set_options = newfile || read_বাফার
					   || (eap != ফাঁকা && eap->read_edit);
    linenr_T	read_buf_lnum = 1;	// next line to read from curbuf
    colnr_T	read_buf_col = 0;	// next char to read from this line
    char_u	c;
    linenr_T	lnum = from;
    char_u	*ptr = ফাঁকা;		// pointer into read buffer
    char_u	*বাফার = ফাঁকা;		// read buffer
    char_u	*new_বাফার = ফাঁকা;	// init to shut up gcc
    char_u	*সারি_start = ফাঁকা;	// init to shut up gcc
    পূর্ণসংখ্যা		wasempty;		// buffer was empty before reading
    colnr_T	len;
    long	আকার_2 = 0;
    char_u	*p;
    off_T	filesize = 0;
    পূর্ণসংখ্যা		skip_read = FALSE;
#ifdef FEAT_CRYPT
    off_T       filesize_disk = 0;      // file size read from disk
    off_T       filesize_গুনতি = 0;     //গণক
    char_u	*cryptkey = ফাঁকা;
    পূর্ণসংখ্যা		did_ask_for_চাবি = FALSE;
#endif
#ifdef FEAT_PERSISTENT_UNDO
    context_sha256_T sha_ctx;
    পূর্ণসংখ্যা		read_undo_file = FALSE;
#endif
    পূর্ণসংখ্যা		split = 0;		// number of split lines
#define UNKNOWN	 0x0fffffff		// file size is unknown
    linenr_T	linecnt;
    পূর্ণসংখ্যা		error = FALSE;		// errors encountered
    পূর্ণসংখ্যা		ff_error = EOL_UNKNOWN; // file format with errors
    long	linerest = 0;		// remaining chars in line
#ifdef UNIX
    পূর্ণসংখ্যা		perm = 0;
    পূর্ণসংখ্যা		swap_mode = -1;		// protection bits for swap file
#অন্যথা
    পূর্ণসংখ্যা		perm;
#endif
    পূর্ণসংখ্যা		fileformat = 0;		// end-of-line format
    পূর্ণসংখ্যা		keep_fileformat = FALSE;
    stat_T	st;
    পূর্ণসংখ্যা		file_readonly;
    linenr_T	skip_গুনতি = 0;
    linenr_T	read_গুনতি = 0;
    পূর্ণসংখ্যা		বার্তা_save = বার্তা_scroll;
    linenr_T	read_no_eol_lnum = 0;   // non-zero lnum when last line of
					// last read was missing the eol
    পূর্ণসংখ্যা		try_mac;
    পূর্ণসংখ্যা		try_dos;
    পূর্ণসংখ্যা		try_unix;
    পূর্ণসংখ্যা		file_rewind = FALSE;
    পূর্ণসংখ্যা		can_retry;
    linenr_T	conv_error = 0;		// line nr with conversion error
    linenr_T	illegal_byte = 0;	// line nr with illegal byte
    পূর্ণসংখ্যা		keep_dest_enc = FALSE;	// don't retry when char doesn't fit
					// in destination encoding
    পূর্ণসংখ্যা		bad_char_behavior = BAD_REPLACE;
					// BAD_KEEP, BAD_DROP or character to
					// replace with
    char_u	*tmpname = ফাঁকা;	// name of 'charconvert' output file
    পূর্ণসংখ্যা		fio_flags = 0;
    char_u	*fenc;			// fileencoding to use
    পূর্ণসংখ্যা		fenc_alloced;		// fenc_next is in allocated memory
    char_u	*fenc_পরবর্তী = ফাঁকা;	// next item in 'fencs' or NULL
    পূর্ণসংখ্যা		advance_fenc = FALSE;
    long	real_আকার = 0;
#ifdef USE_ICONV
    iconv_t	iconv_fd = (iconv_t)-1;	// descriptor for iconv() or -1
# ifdef FEAT_EVAL
    পূর্ণসংখ্যা		did_iconv = FALSE;	// TRUE when iconv() failed and trying
					// 'charconvert' next
# endif
#endif
    পূর্ণসংখ্যা		converted = FALSE;	// TRUE if conversion done
    পূর্ণসংখ্যা		notconverted = FALSE;	// TRUE if conversion wanted but it
					// wasn't possible
    char_u	conv_rest[CONV_RESTLEN];
    পূর্ণসংখ্যা		conv_restlen = 0;	// nr of bytes in conv_rest[]
    pos_T	orig_start;
    buf_T	*old_curbuf;
    char_u	*old_b_ffname;
    char_u	*old_b_fname;
    পূর্ণসংখ্যা		using_b_ffname;
    পূর্ণসংখ্যা		using_b_fname;
    স্থির অক্ষর *বার্তা_is_a_directory = N_("is a directory");
#ifdef FEAT_CRYPT
    পূর্ণসংখ্যা		eof = FALSE;
#endif
#ifdef FEAT_SODIUM
    পূর্ণসংখ্যা		may_need_lseek = FALSE;
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
    যদি (curbuf->b_ffname == ফাঁকা
	    && !filtering
	    && fname != ফাঁকা
	    && vim_strchr(p_cpo, CPO_FNAMER) != ফাঁকা
	    && !(flags & READ_DUMMY))
    {
	যদি (set_rw_fname(fname, sfname) == FAIL)
	    যাও theend;
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
    যদি (sfname == ফাঁকা)
	sfname = fname;
#যদি defined(UNIX)
    fname = sfname;
#endif

    /*
     * The BufReadCmd and FileReadCmd events intercept the reading process by
     * executing the associated commands instead.
     */
    যদি (!filtering && !read_stdin && !read_বাফার)
    {
	orig_start = curbuf->b_op_start;

	// Set '[ mark to the line above where the lines go (line 1 if zero).
	curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
	curbuf->b_op_start.col = 0;

	যদি (newfile)
	{
	    যদি (apply_autocmds_exarg(EVENT_BUFREADCMD, ফাঁকা, sfname,
							  FALSE, curbuf, eap))
	    {
		retval = OK;
#ifdef FEAT_EVAL
		যদি (aborting())
		    retval = FAIL;
#endif
		// The BufReadCmd code usually uses ":read" to get the text and
		// perhaps ":file" to change the buffer name. But we should
		// consider this to work like ":edit", thus reset the
		// BF_NOTEDITED flag.  Then ":write" will work to overwrite the
		// same file.
		যদি (retval == OK)
		    curbuf->b_flags &= ~BF_NOTEDITED;
		যাও theend;
	    }
	}
	অন্যথা যদি (apply_autocmds_exarg(EVENT_FILEREADCMD, sfname, sfname,
							    FALSE, ফাঁকা, eap))
	{
#ifdef FEAT_EVAL
	    retval = aborting() ? FAIL : OK;
#অন্যথা
	    retval = OK;
#endif
	    যাও theend;
	}

	curbuf->b_op_start = orig_start;

	যদি (flags & READ_NOFILE)
	{
	    // Return NOTDONE instead of FAIL so that BufEnter can be triggered
	    // and other operations don't fail.
	    retval = NOTDONE;
	    যাও theend;
	}
    }

    যদি ((shortmess(SHM_OVER) || curbuf->b_help) && p_verbose == 0)
	বার্তা_scroll = FALSE;	// overwrite previous file message
    অন্যথা
	বার্তা_scroll = TRUE;	// don't overwrite previous file message

    যদি (fname != ফাঁকা && *fname != NUL)
    {
	fnamelen = STRLEN(fname);

	// If the name is too long we might crash further on, quit here.
	যদি (fnamelen >= MAXPATHL)
	{
	    filemess(curbuf, fname, (char_u *)_("Illegal file name"), 0);
	    বার্তা_end();
	    বার্তা_scroll = বার্তা_save;
	    যাও theend;
	}

	// If the name ends in a path separator, we can't open it.  Check here,
	// because reading the file may actually work, but then creating the
	// swap file may destroy it!  Reported on MS-DOS and Win 95.
	যদি (after_pathsep(fname, fname + fnamelen))
	{
	    filemess(curbuf, fname, (char_u *)_(বার্তা_is_a_directory), 0);
	    বার্তা_end();
	    বার্তা_scroll = বার্তা_save;
	    retval = NOTDONE;
	    যাও theend;
	}
    }
#যদি defined(UNIX) || defined(VMS)
    যদি (!read_stdin && fname != ফাঁকা)
	perm = mch_getperm(fname);
#endif

    যদি (!read_stdin && !read_বাফার && !read_fifo)
    {
#যদি defined(UNIX) || defined(VMS)
	যদি (perm >= 0 && !S_ISREG(perm)		    // not a regular file ...
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
	    যদি (S_ISDIR(perm))
	    {
		filemess(curbuf, fname, (char_u *)_(বার্তা_is_a_directory), 0);
		retval = NOTDONE;
	    }
	    অন্যথা
		filemess(curbuf, fname, (char_u *)_("is not a file"), 0);
	    বার্তা_end();
	    বার্তা_scroll = বার্তা_save;
	    যাও theend;
	}
#endif
#যদি defined(MSWIN)
	/*
	 * MS-Windows allows opening a device, but we will probably get stuck
	 * trying to read it.
	 */
	যদি (!p_odev && mch_nodetype(fname) == গ্রন্থি_WRITABLE)
	{
	    filemess(curbuf, fname, (char_u *)_("is a device (disabled with 'opendevice' option)"), 0);
	    বার্তা_end();
	    বার্তা_scroll = বার্তা_save;
	    যাও theend;
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
    যদি (check_readonly && !readonlymode)
	curbuf->b_p_ro = FALSE;

    যদি (newfile && !read_stdin && !read_বাফার && !read_fifo)
    {
	// Remember time of file.
	যদি (mch_stat((অক্ষর *)fname, &st) >= 0)
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
	অন্যথা
	{
	    curbuf->b_mtime = 0;
	    curbuf->b_mtime_ns = 0;
	    curbuf->b_mtime_read = 0;
	    curbuf->b_mtime_read_ns = 0;
	    curbuf->b_orig_আকার = 0;
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
    যদি (read_stdin)
    {
#যদি defined(MSWIN)
	// Force binary I/O on stdin to avoid CR-LF -> LF conversion.
	setmode(0, O_দ্বিমিক);
#endif
    }
    অন্যথা যদি (!read_বাফার)
    {
#ifdef USE_MCH_ACCESS
	যদি (
# ifdef UNIX
	    !(perm & 0222) ||
# endif
				mch_access((অক্ষর *)fname, W_OK))
	    file_readonly = TRUE;
	fd = mch_open((অক্ষর *)fname, O_RDONLY | O_EXTRA, 0);
#অন্যথা
	যদি (!newfile
		|| readonlymode
		|| (fd = mch_open((অক্ষর *)fname, O_RDWR | O_EXTRA, 0)) < 0)
	{
	    file_readonly = TRUE;
	    // try to open ro
	    fd = mch_open((অক্ষর *)fname, O_RDONLY | O_EXTRA, 0);
	}
#endif
    }

    যদি (fd < 0)			    // cannot open at all
    {
#ifndef UNIX
	পূর্ণসংখ্যা	isdir_f;
#endif
	বার্তা_scroll = বার্তা_save;
#ifndef UNIX
	/*
	 * On Amiga we can't open a directory, check here.
	 */
	isdir_f = (mch_isdir(fname));
	perm = mch_getperm(fname);  // check if the file exists
	যদি (isdir_f)
	{
	    filemess(curbuf, sfname, (char_u *)_(বার্তা_is_a_directory), 0);
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	}
	অন্যথা
#endif
	    যদি (newfile)
	    {
		যদি (perm < 0
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
		    যদি (!bt_dontwrite(curbuf))
		    {
			check_need_swap(newfile);
			// SwapExists autocommand may mess things up
			যদি (curbuf != old_curbuf
				|| (using_b_ffname
					&& (old_b_ffname != curbuf->b_ffname))
				|| (using_b_fname
					 && (old_b_fname != curbuf->b_fname)))
			{
			    emsg(_(e_autocommands_changed_বাফার_or_বাফার_name));
			    যাও theend;
			}
		    }
		    যদি (dir_of_file_exists(fname))
			filemess(curbuf, sfname,
					      (char_u *)new_file_message(), 0);
		    অন্যথা
			filemess(curbuf, sfname,
					   (char_u *)_("[New DIRECTORY]"), 0);
#ifdef FEAT_VIMINFO
		    // Even though this is a new file, it might have been
		    // edited before and deleted.  Get the old marks.
		    check_marks_read();
#endif
		    // Set forced 'fileencoding'.
		    যদি (eap != ফাঁকা)
			set_forced_fenc(eap);
		    apply_autocmds_exarg(EVENT_BUFNEWFILE, sfname, sfname,
							  FALSE, curbuf, eap);
		    // remember the current fileformat
		    save_file_ff(curbuf);

#যদি defined(FEAT_EVAL)
		    যদি (!aborting())   // autocmds may abort script processing
#endif
			retval = OK;	    // a new file is not an error
		    যাও theend;
		}
		অন্যথা
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

	যাও theend;
    }

    /*
     * Only set the 'ro' flag for readonly files the first time they are
     * loaded.	Help files always get readonly mode
     */
    যদি ((check_readonly && file_readonly) || curbuf->b_help)
	curbuf->b_p_ro = TRUE;

    যদি (set_options)
    {
	// Don't change 'eol' if reading from buffer as it will already be
	// correctly set when reading stdin.
	যদি (!read_বাফার)
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
    যদি (!bt_dontwrite(curbuf))
    {
	check_need_swap(newfile);
	যদি (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))))
	{
	    emsg(_(e_autocommands_changed_বাফার_or_বাফার_name));
	    যদি (!read_বাফার)
		close(fd);
	    যাও theend;
	}
#ifdef UNIX
	// Set swap file protection bits after creating it.
	যদি (swap_mode > 0 && curbuf->b_ml.ml_mfp != ফাঁকা
			  && curbuf->b_ml.ml_mfp->mf_fname != ফাঁকা)
	{
	    char_u *swap_fname = curbuf->b_ml.ml_mfp->mf_fname;

	    /*
	     * If the group-read bit is set but not the world-read bit, then
	     * the group must be equal to the group of the original file.  If
	     * we can't make that happen then reset the group-read bit.  This
	     * avoids making the swap file readable to more users when the
	     * primary group of the user is too permissive.
	     */
	    যদি ((swap_mode & 044) == 040)
	    {
		stat_T	swap_st;

		যদি (mch_stat((অক্ষর *)swap_fname, &swap_st) >= 0
			&& st.st_gid != swap_st.st_gid
# ifdef HAVE_FCHOWN
			&& fchown(curbuf->b_ml.ml_mfp->mf_fd, -1, st.st_gid)
									  == -1
# endif
		   )
		    swap_mode &= 0600;
	    }

	    (শূন্য)mch_setperm(swap_fname, (long)swap_mode);
	}
#endif
    }

    // If "Quit" selected at ATTENTION dialog, don't load the file
    যদি (swap_exists_action == SEA_QUIT)
    {
	যদি (!read_বাফার && !read_stdin)
	    close(fd);
	যাও theend;
    }

    ++no_wait_return;	    // don't wait for return yet

    /*
     * Set '[ mark to the line above where the lines go (line 1 if zero).
     */
    orig_start = curbuf->b_op_start;
    curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
    curbuf->b_op_start.col = 0;

    try_mac = (vim_strchr(p_ffs, 'm') != ফাঁকা);
    try_dos = (vim_strchr(p_ffs, 'd') != ফাঁকা);
    try_unix = (vim_strchr(p_ffs, 'x') != ফাঁকা);

    যদি (!read_বাফার)
    {
	পূর্ণসংখ্যা	m = বার্তা_scroll;
	পূর্ণসংখ্যা	n = বার্তা_scrolled;

	/*
	 * The file must be closed again, the autocommands may want to change
	 * the file before reading it.
	 */
	যদি (!read_stdin)
	    close(fd);		// ignore errors

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	বার্তা_scroll = TRUE;
	যদি (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPRE, ফাঁকা, sfname,
							  FALSE, curbuf, eap);
	অন্যথা যদি (read_stdin)
	    apply_autocmds_exarg(EVENT_STDINREADPRE, ফাঁকা, sfname,
							  FALSE, curbuf, eap);
	অন্যথা যদি (newfile)
	    apply_autocmds_exarg(EVENT_BUFREADPRE, ফাঁকা, sfname,
							  FALSE, curbuf, eap);
	অন্যথা
	    apply_autocmds_exarg(EVENT_FILEREADPRE, sfname, sfname,
							    FALSE, ফাঁকা, eap);
	// autocommands may have changed it
	try_mac = (vim_strchr(p_ffs, 'm') != ফাঁকা);
	try_dos = (vim_strchr(p_ffs, 'd') != ফাঁকা);
	try_unix = (vim_strchr(p_ffs, 'x') != ফাঁকা);
	curbuf->b_op_start = orig_start;

	যদি (বার্তা_scrolled == n)
	    বার্তা_scroll = m;

#ifdef FEAT_EVAL
	যদি (aborting())	    // autocmds may abort script processing
	{
	    --no_wait_return;
	    বার্তা_scroll = বার্তা_save;
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    যাও theend;
	}
#endif
	/*
	 * Don't allow the autocommands to change the current buffer.
	 * Try to re-open the file.
	 *
	 * Don't allow the autocommands to change the buffer name either
	 * (cd for example) if it invalidates fname or sfname.
	 */
	যদি (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))
		|| (fd = mch_open((অক্ষর *)fname, O_RDONLY | O_EXTRA, 0)) < 0))
	{
	    --no_wait_return;
	    বার্তা_scroll = বার্তা_save;
	    যদি (fd < 0)
		emsg(_(e_readpre_autocommands_made_file_unreadable));
	    অন্যথা
		emsg(_(e_readpre_autocommands_must_not_change_current_বাফার));
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    যাও theend;
	}
    }

    // Autocommands may add lines to the file, need to check if it is empty
    wasempty = (curbuf->b_ml.ml_flags & ML_EMPTY);

    যদি (!recoverymode && !filtering && !(flags & READ_DUMMY))
    {
	/*
	 * Show the user that we are busy reading the input.  Sometimes this
	 * may take a while.  When reading from stdin another program may
	 * still be running, don't move the cursor to the last line, unless
	 * always using the GUI.
	 */
	যদি (read_stdin)
	{
	    যদি (!is_not_a_term())
	    {
#ifndef ALWAYS_USE_GUI
# ifdef VIMDLL
		যদি (!gui.in_use)
# endif
		    mch_বার্তা(_("Vim: Reading from stdin...\n"));
#endif
#ifdef FEAT_GUI
		// Also write a message in the GUI window, if there is one.
		যদি (gui.in_use && !gui.dying && !gui.starting)
		{
		    size_t  plen = STRLEN(_("Reading from stdin..."));

		    // make a copy, gui_write() may try to change it
		    p = vim_strnsave((char_u *)_("Reading from stdin..."), plen);
		    যদি (p != ফাঁকা)
		    {
			gui_write(p, (পূর্ণসংখ্যা)plen);
			vim_free(p);
		    }
		}
#endif
	    }
	}
	অন্যথা যদি (!read_বাফার)
	    filemess(curbuf, sfname, (char_u *)"", 0);
    }

    বার্তা_scroll = FALSE;			// overwrite the file message

    /*
     * Set linecnt now, before the "retry" caused by a wrong guess for
     * fileformat, and after the autocommands, which may change them.
     */
    linecnt = curbuf->b_ml.ml_সারি_গুনতি;

    // "++bad=" argument.
    যদি (eap != ফাঁকা && eap->bad_char != 0)
    {
	bad_char_behavior = eap->bad_char;
	যদি (set_options)
	    curbuf->b_bad_char = eap->bad_char;
    }
    অন্যথা
	curbuf->b_bad_char = 0;

    /*
     * Decide which 'encoding' to use or use first.
     */
    যদি (eap != ফাঁকা && eap->force_enc != 0)
    {
	fenc = enc_canonize(eap->cmd + eap->force_enc);
	fenc_alloced = TRUE;
	keep_dest_enc = TRUE;
    }
    অন্যথা যদি (curbuf->b_p_bin)
    {
	fenc = (char_u *)"";		// binary: don't convert
	fenc_alloced = FALSE;
    }
    অন্যথা যদি (curbuf->b_help)
    {
	char_u	    firstline[80];
	পূর্ণসংখ্যা	    fc;

	// Help files are either utf-8 or latin1.  Try utf-8 first, if this
	// fails it must be latin1.
	// Always do this when 'encoding' is "utf-8".  Otherwise only do
	// this when needed to avoid [converted] remarks all the time.
	// It is needed when the first line contains non-ASCII characters.
	// That is only in *.??x files.
	fenc = (char_u *)"latin1";
	c = enc_utf8;
	যদি (!c && !read_stdin)
	{
	    fc = fname[fnamelen - 1];
	    যদি (TOLOWER_ASC(fc) == 'x')
	    {
		// Read the first line (and a bit more).  Immediately rewind to
		// the start of the file.  If the read() fails "len" is -1.
		len = read_eintr(fd, firstline, 80);
		vim_lseek(fd, (off_T)0L, SEEK_SET);
		জন্য (p = firstline; p < firstline + len; ++p)
		    যদি (*p >= 0x80)
		    {
			c = TRUE;
			বিরতি;
		    }
	    }
	}

	যদি (c)
	{
	    fenc_পরবর্তী = fenc;
	    fenc = (char_u *)"utf-8";

	    // When the file is utf-8 but a character doesn't fit in
	    // 'encoding' don't retry.  In help text editing utf-8 bytes
	    // doesn't make sense.
	    যদি (!enc_utf8)
		keep_dest_enc = TRUE;
	}
	fenc_alloced = FALSE;
    }
    অন্যথা যদি (*p_fencs == NUL)
    {
	fenc = curbuf->b_p_fenc;	// use format from buffer
	fenc_alloced = FALSE;
    }
    অন্যথা
    {
	fenc_পরবর্তী = p_fencs;		// try items in 'fileencodings'
	fenc = পরবর্তী_fenc(&fenc_পরবর্তী, &fenc_alloced);
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

    যদি (file_rewind)
    {
	যদি (read_বাফার)
	{
	    read_buf_lnum = 1;
	    read_buf_col = 0;
	}
	অন্যথা যদি (read_stdin || vim_lseek(fd, (off_T)0L, SEEK_SET) != 0)
	{
	    // Can't rewind the file, give up.
	    error = TRUE;
	    যাও failed;
	}
	// Delete the previously read lines.
	যখন (lnum > from)
	    ml_delete(lnum--);
	file_rewind = FALSE;
	যদি (set_options)
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
    যদি (keep_fileformat)
	keep_fileformat = FALSE;
    অন্যথা
    {
	যদি (eap != ফাঁকা && eap->force_ff != 0)
	{
	    fileformat = get_fileformat_force(curbuf, eap);
	    try_unix = try_dos = try_mac = FALSE;
	}
	অন্যথা যদি (curbuf->b_p_bin)
	    fileformat = EOL_UNIX;		// binary: use Unix format
	অন্যথা যদি (*p_ffs == NUL)
	    fileformat = get_fileformat(curbuf);// use format from buffer
	অন্যথা
	    fileformat = EOL_UNKNOWN;		// detect from file
    }

#ifdef USE_ICONV
    যদি (iconv_fd != (iconv_t)-1)
    {
	// aborted conversion with iconv(), close the descriptor
	iconv_close(iconv_fd);
	iconv_fd = (iconv_t)-1;
    }
#endif

    যদি (advance_fenc)
    {
	/*
	 * Try the next entry in 'fileencodings'.
	 */
	advance_fenc = FALSE;

	যদি (eap != ফাঁকা && eap->force_enc != 0)
	{
	    // Conversion given with "++cc=" wasn't possible, read
	    // without conversion.
	    notconverted = TRUE;
	    conv_error = 0;
	    যদি (fenc_alloced)
		vim_free(fenc);
	    fenc = (char_u *)"";
	    fenc_alloced = FALSE;
	}
	অন্যথা
	{
	    যদি (fenc_alloced)
		vim_free(fenc);
	    যদি (fenc_পরবর্তী != ফাঁকা)
	    {
		fenc = পরবর্তী_fenc(&fenc_পরবর্তী, &fenc_alloced);
	    }
	    অন্যথা
	    {
		fenc = (char_u *)"";
		fenc_alloced = FALSE;
	    }
	}
	যদি (tmpname != ফাঁকা)
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
    যদি (converted)
    {
	// "ucs-bom" means we need to check the first bytes of the file
	// for a BOM.
	যদি (STRCMP(fenc, ENC_UCSBOM) == 0)
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
	অন্যথা যদি (enc_utf8 || STRCMP(p_enc, "latin1") == 0)
	    fio_flags = get_fio_flags(fenc);

#ifdef MSWIN
	/*
	 * Conversion from an MS-Windows codepage to UTF-8 or another codepage
	 * is handled with MultiByteToWideChar().
	 */
	যদি (fio_flags == 0)
	    fio_flags = get_win_fio_flags(fenc);
#endif

#ifdef MACOS_CONVERT
	// Conversion from Apple MacRoman to latin1 or UTF-8
	যদি (fio_flags == 0)
	    fio_flags = get_mac_fio_flags(fenc);
#endif

#ifdef USE_ICONV
	/*
	 * Try using iconv() if we can't convert internally.
	 */
	যদি (fio_flags == 0
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
	যদি (fio_flags == 0 && !read_stdin && !read_বাফার && *p_ccv != NUL
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
	    যদি (tmpname == ফাঁকা)
	    {
		tmpname = readfile_charconvert(fname, fenc, &fd);
		যদি (tmpname == ফাঁকা)
		{
		    // Conversion failed.  Try another one.
		    advance_fenc = TRUE;
		    যদি (fd < 0)
		    {
			// Re-opening the original file failed!
			emsg(_(e_conversion_mad_file_unreadable));
			error = TRUE;
			যাও failed;
		    }
		    যাও retry;
		}
	    }
	}
	অন্যথা
#endif
	{
	    যদি (fio_flags == 0
#ifdef USE_ICONV
		    && iconv_fd == (iconv_t)-1
#endif
	       )
	    {
		// Conversion wanted but we can't.
		// Try the next conversion in 'fileencodings'
		advance_fenc = TRUE;
		যাও retry;
	    }
	}
    }

    // Set "can_retry" when it's possible to rewind the file and try with
    // another "fenc" value.  It's FALSE when no other "fenc" to try, reading
    // stdin or fixed at a specific encoding.
    can_retry = (*fenc != NUL && !read_stdin && !read_fifo && !keep_dest_enc);

    যদি (!skip_read)
    {
	linerest = 0;
	filesize = 0;
#ifdef FEAT_CRYPT
	filesize_গুনতি = 0;
#endif
	skip_গুনতি = সারিগুলি_to_skip;
	read_গুনতি = সারিগুলি_to_read;
	conv_restlen = 0;
#ifdef FEAT_PERSISTENT_UNDO
	read_undo_file = (newfile && (flags & READ_KEEP_UNDO) == 0
				  && curbuf->b_ffname != ফাঁকা
				  && curbuf->b_p_udf
				  && !filtering
				  && !read_fifo
				  && !read_stdin
				  && !read_বাফার);
	যদি (read_undo_file)
	    sha256_start(&sha_ctx);
#endif
#ifdef FEAT_CRYPT
	যদি (curbuf->b_cryptstate != ফাঁকা)
	{
	    // Need to free the state, but keep the key, don't want to ask for
	    // it again.
	    crypt_free_state(curbuf->b_cryptstate);
	    curbuf->b_cryptstate = ফাঁকা;
	}
#endif
    }

    যখন (!error && !got_int)
    {
	/*
	 * We allocate as much space for the file as we can get, plus
	 * space for the old line plus room for one terminating NUL.
	 * The amount is limited by the fact that read() only can read
	 * up to max_unsigned characters (and other things).
	 */
	যদি (!skip_read)
	{
#যদি defined(SSIZE_MAX) && (SSIZE_MAX < 0x10000L)
		আকার_2 = SSIZE_MAX;		    // use max I/O size, 52K
#অন্যথা
		// Use buffer >= 64K.  Add linerest to double the size if the
		// line gets very long, to avoid a lot of copying. But don't
		// read more than 1 Mbyte at a time, so we can be interrupted.
		আকার_2 = 0x10000L + linerest;
		যদি (আকার_2 > 0x100000L)
		    আকার_2 = 0x100000L;
#endif
	}

	// Protect against the argument of lalloc() going negative.
	// Also split lines that are too long for colnr_T.  After this check
	// passes, we read up to 'size' more bytes.  We must ensure that even
	// after that read, the line length won't exceed MAXCOL - 1 (because
	// we add 1 for the NUL when casting to colnr_T).  If this check fires,
	// we insert a synthetic newline immediately, so linerest doesn't grow.
	যদি (আকার_2 < 0 || আকার_2 + linerest + 1 < 0 || linerest >= MAXCOL - আকার_2)
	{
	    ++split;
	    *ptr = NL;		    // split line by inserting a NL
	    আকার_2 = 1;
	}
	অন্যথা
	{
	    যদি (!skip_read)
	    {
		জন্য ( ; আকার_2 >= 10; আকার_2 = (long)((long_u)আকার_2 >> 1))
		{
		    যদি ((new_বাফার = lalloc(আকার_2 + linerest + 1,
							      FALSE)) != ফাঁকা)
			বিরতি;
		}
		যদি (new_বাফার == ফাঁকা)
		{
		    do_outofmem_বার্তা((long_u)(আকার_2 * 2 + linerest + 1));
		    error = TRUE;
		    বিরতি;
		}
		যদি (linerest)	// copy characters from the previous buffer
		    mch_memmove(new_বাফার, ptr - linerest, (size_t)linerest);
		vim_free(বাফার);
		বাফার = new_বাফার;
		ptr = বাফার + linerest;
		সারি_start = বাফার;

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
		real_আকার = (পূর্ণসংখ্যা)আকার_2;
#ifdef USE_ICONV
		যদি (iconv_fd != (iconv_t)-1)
		    আকার_2 = আকার_2 / ICONV_MULT;
		অন্যথা
#endif
		    যদি (fio_flags & FIO_LATIN1)
		    আকার_2 = আকার_2 / 2;
		অন্যথা যদি (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    আকার_2 = (আকার_2 * 2 / 3) & ~1;
		অন্যথা যদি (fio_flags & FIO_UCS4)
		    আকার_2 = (আকার_2 * 2 / 3) & ~3;
		অন্যথা যদি (fio_flags == FIO_UCSBOM)
		    আকার_2 = আকার_2 / ICONV_MULT;	// worst case
#ifdef MSWIN
		অন্যথা যদি (fio_flags & FIO_CODEPAGE)
		    আকার_2 = আকার_2 / ICONV_MULT;	// also worst case
#endif
#ifdef MACOS_CONVERT
		অন্যথা যদি (fio_flags & FIO_MACROMAN)
		    আকার_2 = আকার_2 / ICONV_MULT;	// also worst case
#endif

		যদি (conv_restlen > 0)
		{
		    // Insert unconverted bytes from previous line.
		    mch_memmove(ptr, conv_rest, conv_restlen);
		    ptr += conv_restlen;
		    আকার_2 -= conv_restlen;
		}

		যদি (read_বাফার)
		{
		    /*
		     * Read bytes from curbuf.  Used for converting text read
		     * from stdin.
		     */
		    যদি (read_buf_lnum > from)
			আকার_2 = 0;
		    অন্যথা
		    {
			পূর্ণসংখ্যা	n, ni;
			long	tlen;

			tlen = 0;
			জন্য (;;)
			{
			    p = ml_get(read_buf_lnum) + read_buf_col;
			    n = ml_get_len(read_buf_lnum) - read_buf_col;
			    যদি ((পূর্ণসংখ্যা)tlen + n + 1 > আকার_2)
			    {
				// Filled up to "size", append partial line.
				// Change NL to NUL to reverse the effect done
				// below.
				n = (পূর্ণসংখ্যা)(আকার_2 - tlen);
				জন্য (ni = 0; ni < n; ++ni)
				{
				    যদি (p[ni] == NL)
					ptr[tlen++] = NUL;
				    অন্যথা
					ptr[tlen++] = p[ni];
				}
				read_buf_col += n;
				বিরতি;
			    }

			    // Append whole line and new-line.  Change NL
			    // to NUL to reverse the effect done below.
			    জন্য (ni = 0; ni < n; ++ni)
			    {
				যদি (p[ni] == NL)
				    ptr[tlen++] = NUL;
				অন্যথা
				    ptr[tlen++] = p[ni];
			    }
			    ptr[tlen++] = NL;
			    read_buf_col = 0;
			    যদি (++read_buf_lnum > from)
			    {
				// When the last line didn't have an
				// end-of-line don't add it now either.
				যদি (!curbuf->b_p_eol)
				    --tlen;
				আকার_2 = tlen;
#ifdef FEAT_CRYPT
				eof = TRUE;
#endif
				বিরতি;
			    }
			}
		    }
		}
		অন্যথা
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
		    যদি (filesize == 0)
		    {
			// set size to 8K + Sodium Crypt Metadata
			আকার_2 = WRITEBUFSIZE + crypt_get_max_header_len()
			    + crypto_secretstream_xchacha20poly1305_HEADERBYTES
				+ crypto_secretstream_xchacha20poly1305_ABYTES;
			may_need_lseek = TRUE;
		    }

		    অন্যথা যদি (filesize > 0 && (curbuf->b_cryptstate != ফাঁকা
				&& crypt_method_is_sodium(
					     curbuf->b_cryptstate->method_nr)))
		    {
			আকার_2 = WRITEBUFSIZE + crypto_secretstream_xchacha20poly1305_ABYTES;
			// need to rewind by - add_len from CRYPT_M_SOD2 (see
			// description above)
			যদি (curbuf->b_cryptstate->method_nr == CRYPT_M_SOD
						     && !eof && may_need_lseek)
			{
			    lseek(fd, crypt_get_header_len(
					       curbuf->b_cryptstate->method_nr)
				       - crypt_get_max_header_len(), SEEK_CUR);
			    may_need_lseek = FALSE;
			}
		    }
#endif
		    long read_আকার = আকার_2;
		    আকার_2 = read_eintr(fd, ptr, read_আকার);
#ifdef FEAT_CRYPT
		    // Did we reach end of file?
		    filesize_গুনতি += আকার_2;
		    eof = (আকার_2 < read_আকার || filesize_গুনতি == filesize_disk);
#endif
		}

#ifdef FEAT_CRYPT
		/*
		 * At start of file: Check for magic number of encryption.
		 */
		যদি (filesize == 0 && আকার_2 > 0)
		{
		    cryptkey = check_for_cryptkey(cryptkey, ptr, &আকার_2,
						  &filesize, newfile, sfname,
						  &did_ask_for_চাবি);
# যদি defined(CRYPT_NOT_INPLACE) && defined(FEAT_PERSISTENT_UNDO)
		    যদি (curbuf->b_cryptstate != ফাঁকা
				 && !crypt_works_inplace(curbuf->b_cryptstate))
			// reading undo file requires crypt_decode_inplace()
			read_undo_file = FALSE;
# endif
		}
		/*
		 * Decrypt the read bytes.  This is done before checking for
		 * EOF because the crypt layer may be buffering.
		 */
		যদি (cryptkey != ফাঁকা && curbuf->b_cryptstate != ফাঁকা
								   && আকার_2 > 0)
		{
# ifdef CRYPT_NOT_INPLACE
		    যদি (crypt_works_inplace(curbuf->b_cryptstate))
		    {
# endif
			crypt_decode_inplace(curbuf->b_cryptstate, ptr,
								    আকার_2, eof);
# ifdef CRYPT_NOT_INPLACE
		    }
		    অন্যথা
		    {
			char_u	*newptr = ফাঁকা;
			পূর্ণসংখ্যা	decrypted_আকার;

			decrypted_আকার = crypt_decode_alloc(
				    curbuf->b_cryptstate, ptr, আকার_2,
								 &newptr, eof);

			যদি (decrypted_আকার < 0)
			{
			    // error message already given
			    error = TRUE;
			    vim_free(newptr);
			    বিরতি;
			}
			// If the crypt layer is buffering, not producing
			// anything yet, need to read more.
			যদি (decrypted_আকার == 0)
			    চালু;

			যদি (linerest == 0)
			{
			    // Simple case: reuse returned buffer (may be
			    // NULL, checked later).
			    new_বাফার = newptr;
			}
			অন্যথা
			{
			    long_u	new_আকার;

			    // Need new buffer to add bytes carried over.
			    new_আকার = (long_u)(decrypted_আকার + linerest + 1);
			    new_বাফার = lalloc(new_আকার, FALSE);
			    যদি (new_বাফার == ফাঁকা)
			    {
				do_outofmem_বার্তা(new_আকার);
				error = TRUE;
				বিরতি;
			    }

			    mch_memmove(new_বাফার, বাফার, linerest + conv_restlen);
			    যদি (newptr != ফাঁকা)
				mch_memmove(new_বাফার + linerest + conv_restlen,
					newptr, decrypted_আকার);
			    vim_free(newptr);
			}

			যদি (new_বাফার != ফাঁকা)
			{
			    vim_free(বাফার);
			    বাফার = new_বাফার;
			    new_বাফার = ফাঁকা;
			    সারি_start = বাফার;
			    ptr = বাফার + linerest + conv_restlen;
			    real_আকার = আকার_2;
			}
			আকার_2 = decrypted_আকার;
		    }
# endif
		}
#endif

		যদি (আকার_2 <= 0)
		{
		    যদি (আকার_2 < 0)		    // read error
			error = TRUE;
		    অন্যথা যদি (conv_restlen > 0)
		    {
			/*
			 * Reached end-of-file but some trailing bytes could
			 * not be converted.  Truncated file?
			 */

			// When we did a conversion report an error.
			যদি (fio_flags != 0
#ifdef USE_ICONV
				|| iconv_fd != (iconv_t)-1
#endif
			   )
			{
			    যদি (can_retry)
				যাও rewind_retry;
			    যদি (conv_error == 0)
				conv_error = curbuf->b_ml.ml_সারি_গুনতি
								- linecnt + 1;
			}
			// Remember the first linenr with an illegal byte
			অন্যথা যদি (illegal_byte == 0)
			    illegal_byte = curbuf->b_ml.ml_সারি_গুনতি
								- linecnt + 1;
			যদি (bad_char_behavior == BAD_DROP)
			{
			    *(ptr - conv_restlen) = NUL;
			    conv_restlen = 0;
			}
			অন্যথা
			{
			    // Replace the trailing bytes with the replacement
			    // character if we were converting; if we weren't,
			    // leave the UTF8 checking code to do it, as it
			    // works slightly differently.
			    যদি (bad_char_behavior != BAD_KEEP && (fio_flags != 0
#ifdef USE_ICONV
				    || iconv_fd != (iconv_t)-1
#endif
			       ))
			    {
				যখন (conv_restlen > 0)
				{
				    *(--ptr) = bad_char_behavior;
				    --conv_restlen;
				}
			    }
			    fio_flags = 0;	// don't convert this
#ifdef USE_ICONV
			    যদি (iconv_fd != (iconv_t)-1)
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
	    যদি ((filesize == 0
#ifdef FEAT_CRYPT
		   || (cryptkey != ফাঁকা
			&& filesize == crypt_get_header_len(
						 crypt_get_method_nr(curbuf)))
#endif
		       )
		    && (fio_flags == FIO_UCSBOM
			|| (!curbuf->b_p_bomb
			    && tmpname == ফাঁকা
			    && (*fenc == 'u' || (*fenc == NUL && enc_utf8)))))
	    {
		char_u	*ccname;
		পূর্ণসংখ্যা	blen;

		// no BOM detection in a short file or in binary mode
		যদি (আকার_2 < 2 || curbuf->b_p_bin)
		    ccname = ফাঁকা;
		অন্যথা
		    ccname = check_for_bom(ptr, আকার_2, &blen,
		      fio_flags == FIO_UCSBOM ? FIO_ALL : get_fio_flags(fenc));
		যদি (ccname != ফাঁকা)
		{
		    // Remove BOM from the text
		    filesize += blen;
		    আকার_2 -= blen;
		    mch_memmove(ptr, ptr + blen, (size_t)আকার_2);
		    যদি (set_options)
		    {
			curbuf->b_p_bomb = TRUE;
			curbuf->b_start_bomb = TRUE;
		    }
		}

		যদি (fio_flags == FIO_UCSBOM)
		{
		    যদি (ccname == ফাঁকা)
		    {
			// No BOM detected: retry with next encoding.
			advance_fenc = TRUE;
		    }
		    অন্যথা
		    {
			// BOM detected: set "fenc" and jump back
			যদি (fenc_alloced)
			    vim_free(fenc);
			fenc = ccname;
			fenc_alloced = FALSE;
		    }
		    // retry reading without getting new bytes or rewinding
		    skip_read = TRUE;
		    যাও retry;
		}
	    }

	    // Include not converted bytes.
	    ptr -= conv_restlen;
	    আকার_2 += conv_restlen;
	    conv_restlen = 0;
	    /*
	     * Break here for a read error or end-of-file.
	     */
	    যদি (আকার_2 <= 0)
		বিরতি;


#ifdef USE_ICONV
	    যদি (iconv_fd != (iconv_t)-1)
	    {
		/*
		 * Attempt conversion of the read bytes to 'encoding' using
		 * iconv().
		 */
		ধ্রুবক অক্ষর	*fromp;
		অক্ষর		*শীর্ষ_2;
		size_t		from_আকার;
		size_t		to_আকার;

		fromp = (অক্ষর *)ptr;
		from_আকার = আকার_2;
		ptr += আকার_2;
		শীর্ষ_2 = (অক্ষর *)ptr;
		to_আকার = real_আকার - আকার_2;

		/*
		 * If there is conversion error or not enough room try using
		 * another conversion.  Except for when there is no
		 * alternative (help files).
		 */
		যখন ((iconv(iconv_fd, (শূন্য *)&fromp, &from_আকার,
							       &শীর্ষ_2, &to_আকার)
			    == (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						  || from_আকার > CONV_RESTLEN)
		{
		    যদি (can_retry)
			যাও rewind_retry;
		    যদি (conv_error == 0)
			conv_error = readfile_linenr(linecnt,
							  ptr, (char_u *)শীর্ষ_2);

		    // Deal with a bad byte and continue with the next.
		    ++fromp;
		    --from_আকার;
		    যদি (bad_char_behavior == BAD_KEEP)
		    {
			*শীর্ষ_2++ = *(fromp - 1);
			--to_আকার;
		    }
		    অন্যথা যদি (bad_char_behavior != BAD_DROP)
		    {
			*শীর্ষ_2++ = bad_char_behavior;
			--to_আকার;
		    }
		}

		যদি (from_আকার > 0)
		{
		    // Some remaining characters, keep them for the next
		    // round.
		    mch_memmove(conv_rest, (char_u *)fromp, from_আকার);
		    conv_restlen = (পূর্ণসংখ্যা)from_আকার;
		}

		// move the linerest to before the converted characters
		সারি_start = ptr - linerest;
		mch_memmove(সারি_start, বাফার, (size_t)linerest);
		আকার_2 = (long)((char_u *)শীর্ষ_2 - ptr);
	    }
#endif

#ifdef MSWIN
	    যদি (fio_flags & FIO_CODEPAGE)
	    {
		char_u	*src, *dst;
		WCHAR	ucs2buf[3];
		পূর্ণসংখ্যা	ucs2len;
		পূর্ণসংখ্যা	codepage = FIO_GET_CP(fio_flags);
		পূর্ণসংখ্যা	bytelen;
		পূর্ণসংখ্যা	found_bad;
		অক্ষর	replstr[2];

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
		যদি (bad_char_behavior > 0)
		    replstr[0] = bad_char_behavior;
		অন্যথা
		    replstr[0] = '?';
		replstr[1] = NUL;

		/*
		 * Move the bytes to the end of the buffer, so that we have
		 * room to put the result at the start.
		 */
		src = ptr + real_আকার - আকার_2;
		mch_memmove(src, ptr, আকার_2);

		/*
		 * Do the conversion.
		 */
		dst = ptr;
		যখন (আকার_2 > 0)
		{
		    found_bad = FALSE;

# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
		    যদি (codepage == CP_UTF8)
		    {
			// Handle CP_UTF8 input ourselves to be able to handle
			// trailing bytes properly.
			// Get one UTF-8 character from src.
			bytelen = (পূর্ণসংখ্যা)utf_ptr2len_len(src, আকার_2);
			যদি (bytelen > আকার_2)
			{
			    // Only got some bytes of a character.  Normally
			    // it's put in "conv_rest", but if it's too long
			    // deal with it as if they were illegal bytes.
			    যদি (bytelen <= CONV_RESTLEN)
				বিরতি;

			    // weird overlong byte sequence
			    bytelen = আকার_2;
			    found_bad = TRUE;
			}
			অন্যথা
			{
			    পূর্ণসংখ্যা	    u8c = utf_ptr2char(src);

			    যদি (u8c > 0xffff || (*src >= 0x80 && bytelen == 1))
				found_bad = TRUE;
			    ucs2buf[0] = u8c;
			    ucs2len = 1;
			}
		    }
		    অন্যথা
# endif
		    {
			// We don't know how long the byte sequence is, try
			// from one to three bytes.
			জন্য (bytelen = 1; bytelen <= আকার_2 && bytelen <= 3;
								    ++bytelen)
			{
			    ucs2len = MultiByteToWideChar(codepage,
							 MB_ERR_INVALID_CHARS,
							 (LPCSTR)src, bytelen,
								   ucs2buf, 3);
			    যদি (ucs2len > 0)
				বিরতি;
			}
			যদি (ucs2len == 0)
			{
			    // If we have only one byte then it's probably an
			    // incomplete byte sequence.  Otherwise discard
			    // one byte as a bad character.
			    যদি (আকার_2 == 1)
				বিরতি;
			    found_bad = TRUE;
			    bytelen = 1;
			}
		    }

		    যদি (!found_bad)
		    {
			পূর্ণসংখ্যা	i;

			// Convert "ucs2buf[ucs2len]" to 'enc' in "dst".
			যদি (enc_utf8)
			{
			    // From UCS-2 to UTF-8.  Cannot fail.
			    জন্য (i = 0; i < ucs2len; ++i)
				dst += utf_char2bytes(ucs2buf[i], dst);
			}
			অন্যথা
			{
			    BOOL	bad = FALSE;
			    পূর্ণসংখ্যা		dstlen;

			    // From UCS-2 to "enc_codepage".  If the
			    // conversion uses the default character "?",
			    // the data doesn't fit in this encoding.
			    dstlen = WideCharToMultiByte(enc_codepage, 0,
				    (LPCWSTR)ucs2buf, ucs2len,
				    (LPSTR)dst, (পূর্ণসংখ্যা)(src - dst),
				    replstr, &bad);
			    যদি (bad)
				found_bad = TRUE;
			    অন্যথা
				dst += dstlen;
			}
		    }

		    যদি (found_bad)
		    {
			// Deal with bytes we can't convert.
			যদি (can_retry)
			    যাও rewind_retry;
			যদি (conv_error == 0)
			    conv_error = readfile_linenr(linecnt, ptr, dst);
			যদি (bad_char_behavior != BAD_DROP)
			{
			    যদি (bad_char_behavior == BAD_KEEP)
			    {
				mch_memmove(dst, src, bytelen);
				dst += bytelen;
			    }
			    অন্যথা
				*dst++ = bad_char_behavior;
			}
		    }

		    src += bytelen;
		    আকার_2 -= bytelen;
		}

		যদি (আকার_2 > 0)
		{
		    // An incomplete byte sequence remaining.
		    mch_memmove(conv_rest, src, আকার_2);
		    conv_restlen = আকার_2;
		}

		// The new size is equal to how much "dst" was advanced.
		আকার_2 = (long)(dst - ptr);
	    }
	    অন্যথা
#endif
#ifdef MACOS_CONVERT
	    যদি (fio_flags & FIO_MACROMAN)
	    {
		/*
		 * Conversion from Apple MacRoman char encoding to UTF-8 or
		 * latin1.  This is in os_mac_conv.c.
		 */
		যদি (macroman2enc(ptr, &আকার_2, real_আকার) == FAIL)
		    যাও rewind_retry;
	    }
	    অন্যথা
#endif
	    যদি (fio_flags != 0)
	    {
		পূর্ণসংখ্যা	u8c;
		char_u	*dest;
		char_u	*লেজ = ফাঁকা;

		/*
		 * "enc_utf8" set: Convert Unicode or Latin1 to UTF-8.
		 * "enc_utf8" not set: Convert Unicode to Latin1.
		 * Go from end to start through the buffer, because the number
		 * of bytes may increase.
		 * "dest" points to after where the UTF-8 bytes go, "p" points
		 * to after the next character to convert.
		 */
		dest = ptr + real_আকার;
		যদি (fio_flags == FIO_LATIN1 || fio_flags == FIO_UTF8)
		{
		    p = ptr + আকার_2;
		    যদি (fio_flags == FIO_UTF8)
		    {
			// Check for a trailing incomplete UTF-8 sequence
			লেজ = ptr + আকার_2 - 1;
			যখন (লেজ > ptr && (*লেজ & 0xc0) == 0x80)
			    --লেজ;
			যদি (লেজ + utf_byte2len(*লেজ) <= ptr + আকার_2)
			    লেজ = ফাঁকা;
			অন্যথা
			    p = লেজ;
		    }
		}
		অন্যথা যদি (fio_flags & (FIO_UCS2 | FIO_UTF16))
		{
		    // Check for a trailing byte
		    p = ptr + (আকার_2 & ~1);
		    যদি (আকার_2 & 1)
			লেজ = p;
		    যদি ((fio_flags & FIO_UTF16) && p > ptr)
		    {
			// Check for a trailing leading word
			যদি (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			অন্যথা
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			যদি (u8c >= 0xd800 && u8c <= 0xdbff)
			    লেজ = p;
			অন্যথা
			    p += 2;
		    }
		}
		অন্যথা //  FIO_UCS4
		{
		    // Check for trailing 1, 2 or 3 bytes
		    p = ptr + (আকার_2 & ~3);
		    যদি (আকার_2 & 3)
			লেজ = p;
		}

		// If there is a trailing incomplete sequence move it to
		// conv_rest[].
		যদি (লেজ != ফাঁকা)
		{
		    conv_restlen = (পূর্ণসংখ্যা)((ptr + আকার_2) - লেজ);
		    mch_memmove(conv_rest, (char_u *)লেজ, conv_restlen);
		    আকার_2 -= conv_restlen;
		}


		যখন (p > ptr)
		{
		    যদি (fio_flags & FIO_LATIN1)
			u8c = *--p;
		    অন্যথা যদি (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    {
			যদি (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			অন্যথা
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			যদি ((fio_flags & FIO_UTF16)
					    && u8c >= 0xdc00 && u8c <= 0xdfff)
			{
			    পূর্ণসংখ্যা u16c;

			    যদি (p == ptr)
			    {
				// Missing leading word.
				যদি (can_retry)
				    যাও rewind_retry;
				যদি (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				যদি (bad_char_behavior == BAD_DROP)
				    চালু;
				যদি (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }

			    // found second word of double-word, get the first
			    // word and compute the resulting character
			    যদি (fio_flags & FIO_ENDIAN_L)
			    {
				u16c = (*--p << 8);
				u16c += *--p;
			    }
			    অন্যথা
			    {
				u16c = *--p;
				u16c += (*--p << 8);
			    }
			    u8c = 0x10000 + ((u16c & 0x3ff) << 10)
							      + (u8c & 0x3ff);

			    // Check if the word is indeed a leading word.
			    যদি (u16c < 0xd800 || u16c > 0xdbff)
			    {
				যদি (can_retry)
				    যাও rewind_retry;
				যদি (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				যদি (bad_char_behavior == BAD_DROP)
				    চালু;
				যদি (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    অন্যথা যদি (fio_flags & FIO_UCS4)
		    {
			যদি (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (unsigned)*--p << 24;
			    u8c += (unsigned)*--p << 16;
			    u8c += (unsigned)*--p << 8;
			    u8c += *--p;
			}
			অন্যথা	// big endian
			{
			    u8c = *--p;
			    u8c += (unsigned)*--p << 8;
			    u8c += (unsigned)*--p << 16;
			    u8c += (unsigned)*--p << 24;
			}
		    }
		    অন্যথা    // UTF-8
		    {
			যদি (*--p < 0x80)
			    u8c = *p;
			অন্যথা
			{
			    len = utf_শীর্ষ_off(ptr, p);
			    p -= len;
			    u8c = utf_ptr2char(p);
			    যদি (len == 0)
			    {
				// Not a valid UTF-8 character, retry with
				// another fenc when possible, otherwise just
				// report the error.
				যদি (can_retry)
				    যাও rewind_retry;
				যদি (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				যদি (bad_char_behavior == BAD_DROP)
				    চালু;
				যদি (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    যদি (enc_utf8)	// produce UTF-8
		    {
			dest -= utf_char2len(u8c);
			(শূন্য)utf_char2bytes(u8c, dest);
		    }
		    অন্যথা		// produce Latin1
		    {
			--dest;
			যদি (u8c >= 0x100)
			{
			    // character doesn't fit in latin1, retry with
			    // another fenc when possible, otherwise just
			    // report the error.
			    যদি (can_retry)
				যাও rewind_retry;
			    যদি (conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
			    যদি (bad_char_behavior == BAD_DROP)
				++dest;
			    অন্যথা যদি (bad_char_behavior == BAD_KEEP)
				*dest = u8c;
			    অন্যথা যদি (eap != ফাঁকা && eap->bad_char != 0)
				*dest = bad_char_behavior;
			    অন্যথা
				*dest = 0xBF;
			}
			অন্যথা
			    *dest = u8c;
		    }
		}

		// move the linerest to before the converted characters
		সারি_start = dest - linerest;
		mch_memmove(সারি_start, বাফার, (size_t)linerest);
		আকার_2 = (long)((ptr + real_আকার) - dest);
		ptr = dest;
	    }
	    অন্যথা যদি (enc_utf8 && !curbuf->b_p_bin)
	    {
		পূর্ণসংখ্যা  incomplete_লেজ = FALSE;

		// Reading UTF-8: Check if the bytes are valid UTF-8.
		জন্য (p = ptr; ; )
		{
		    পূর্ণসংখ্যা	 todo;
		    পূর্ণসংখ্যা	 l;

		    // Skip ASCII bytes quickly using word-at-a-time check.
		    {
			char_u *ascii_end = ptr + আকার_2;
			যখন (ascii_end - p >= (long)আকার(long_u))
			{
			    long_u word;
			    memcpy(&word, p, আকার(long_u));
			    যদি (word & NONASCII_MASK)
				বিরতি;
			    p += আকার(long_u);
			}
			যখন (p < ascii_end && *p < 0x80)
			    ++p;
		    }

		    todo = (পূর্ণসংখ্যা)((ptr + আকার_2) - p);
		    যদি (todo <= 0)
			বিরতি;
		    যদি (*p >= 0x80)
		    {
			// A length of 1 means it's an illegal byte.  Accept
			// an incomplete character at the end though, the next
			// read() will get the next bytes, we'll check it
			// then.
			l = utf_ptr2len_len(p, todo);
			যদি (l > todo && !incomplete_লেজ)
			{
			    // Avoid retrying with a different encoding when
			    // a truncated file is more likely, or attempting
			    // to read the rest of an incomplete sequence when
			    // we have already done so.
			    যদি (p > ptr || filesize > 0)
				incomplete_লেজ = TRUE;
			    // Incomplete byte sequence, move it to conv_rest[]
			    // and try to read the rest of it, unless we've
			    // already done so.
			    যদি (p > ptr)
			    {
				conv_restlen = todo;
				mch_memmove(conv_rest, p, conv_restlen);
				আকার_2 -= conv_restlen;
				বিরতি;
			    }
			}
			যদি (l == 1 || l > todo)
			{
			    // Illegal byte.  If we can try another encoding
			    // do that, unless at EOF where a truncated
			    // file is more likely than a conversion error.
			    যদি (can_retry && !incomplete_লেজ)
				বিরতি;
#ifdef USE_ICONV
			    // When we did a conversion report an error.
			    যদি (iconv_fd != (iconv_t)-1 && conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
#endif
			    // Remember the first linenr with an illegal byte
			    যদি (conv_error == 0 && illegal_byte == 0)
				illegal_byte = readfile_linenr(linecnt, ptr, p);

			    // Drop, keep or replace the bad byte.
			    যদি (bad_char_behavior == BAD_DROP)
			    {
				mch_memmove(p, p + 1, todo - 1);
				--আকার_2;
			    }
			    অন্যথা
			    {
				যদি (bad_char_behavior != BAD_KEEP)
				    *p = bad_char_behavior;
				++p;
			    }
			}
			অন্যথা
			    p += l;
		    }
		}
		যদি (p < ptr + আকার_2 && !incomplete_লেজ)
		{
		    // Detected a UTF-8 error.
rewind_retry:
		    // Retry reading with another conversion.
#যদি defined(FEAT_EVAL) && defined(USE_ICONV)
		    যদি (*p_ccv != NUL && iconv_fd != (iconv_t)-1)
			// iconv() failed, try 'charconvert'
			did_iconv = TRUE;
		    অন্যথা
#endif
			// use next item from 'fileencodings'
			advance_fenc = TRUE;
		    file_rewind = TRUE;
		    যাও retry;
		}
	    }

	    // count the number of characters (after conversion!)
	    filesize += আকার_2;

	    /*
	     * when reading the first part of a file: guess EOL type
	     */
	    যদি (fileformat == EOL_UNKNOWN)
	    {
		// First try finding a NL, for Dos and Unix
		যদি (try_dos || try_unix)
		{
		    // Reset the carriage return counter.
		    যদি (try_mac)
			try_mac = 1;

		    জন্য (p = ptr; p < ptr + আকার_2; ++p)
		    {
			যদি (*p == NL)
			{
			    যদি (!try_unix
				    || (try_dos && p > ptr && p[-1] == CAR))
				fileformat = EOL_DOS;
			    অন্যথা
				fileformat = EOL_UNIX;
			    বিরতি;
			}
			অন্যথা যদি (*p == CAR && try_mac)
			    try_mac++;
		    }

		    // Don't give in to EOL_UNIX if EOL_MAC is more likely
		    যদি (fileformat == EOL_UNIX && try_mac)
		    {
			// Need to reset the counters when retrying fenc.
			try_mac = 1;
			try_unix = 1;
			জন্য (; p >= ptr && *p != CAR; p--)
			    ;
			যদি (p >= ptr)
			{
			    জন্য (p = ptr; p < ptr + আকার_2; ++p)
			    {
				যদি (*p == NL)
				    try_unix++;
				অন্যথা যদি (*p == CAR)
				    try_mac++;
			    }
			    যদি (try_mac > try_unix)
				fileformat = EOL_MAC;
			}
		    }
		    অন্যথা যদি (fileformat == EOL_UNKNOWN && try_mac == 1)
			// Looking for CR but found no end-of-line markers at
			// all: use the default format.
			fileformat = default_fileformat();
		}

		// No NL found: may use Mac format
		যদি (fileformat == EOL_UNKNOWN && try_mac)
		    fileformat = EOL_MAC;

		// Still nothing found?  Use first format in 'ffs'
		যদি (fileformat == EOL_UNKNOWN)
		    fileformat = default_fileformat();

		// if editing a new file: may set p_tx and p_ff
		যদি (set_options)
		    set_fileformat(fileformat, OPT_LOCAL);
	    }
	}

	/*
	 * This loop is executed once for every character read.
	 * Keep it fast!
	 */
	যদি (fileformat == EOL_MAC)
	{
	    --ptr;
	    যখন (++ptr, --আকার_2 >= 0)
	    {
		// catch most common case first
		যদি ((c = *ptr) != NUL && c != CAR && c != NL)
		    চালু;
		যদি (c == NUL)
		    *ptr = NL;	// NULs are replaced by newlines!
		অন্যথা যদি (c == NL)
		    *ptr = CAR;	// NLs are replaced by CRs!
		অন্যথা
		{
		    যদি (skip_গুনতি == 0)
		    {
			*ptr = NUL;	    // end of line
			len = (colnr_T) (ptr - সারি_start + 1);
			যদি (ml_append(lnum, সারি_start, len, newfile) == FAIL)
			{
			    error = TRUE;
			    বিরতি;
			}
#ifdef FEAT_PERSISTENT_UNDO
			যদি (read_undo_file)
			    sha256_update(&sha_ctx, সারি_start, len);
#endif
			++lnum;
			যদি (--read_গুনতি == 0)
			{
			    error = TRUE;	// break loop
			    সারি_start = ptr;	// nothing left to write
			    বিরতি;
			}
		    }
		    অন্যথা
			--skip_গুনতি;
		    সারি_start = ptr + 1;
		}
	    }
	}
	অন্যথা
	{
	    // Use memchr() for SIMD-optimized newline scanning instead
	    // of scanning each byte individually.
	    char_u *end = ptr + আকার_2;

	    যখন (ptr < end)
	    {
		char_u *nl = (char_u *)memchr(ptr, NL, end - ptr);
		char_u *nul_scan;

		যদি (nl == ফাঁকা)
		{
		    // No more newlines in buffer.
		    // Replace any NUL bytes with NL in remaining data.
		    যখন ((nul_scan = (char_u *)memchr(ptr, NUL,
						      end - ptr)) != ফাঁকা)
		    {
			*nul_scan = NL;
			ptr = nul_scan + 1;
		    }
		    ptr = end;
		    বিরতি;
		}

		// Replace NUL bytes with NL before the newline.
		{
		    char_u *scan = ptr;
		    যখন ((nul_scan = (char_u *)memchr(scan, NUL,
						       nl - scan)) != ফাঁকা)
		    {
			*nul_scan = NL;
			scan = nul_scan + 1;
		    }
		}

		// Process the newline.
		ptr = nl;
		যদি (skip_গুনতি == 0)
		{
		    *ptr = NUL;		// end of line
		    len = (colnr_T)(ptr - সারি_start + 1);
		    যদি (fileformat == EOL_DOS)
		    {
			যদি (ptr > সারি_start && ptr[-1] == CAR)
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
			অন্যথা যদি (ff_error != EOL_DOS)
			{
			    যদি (   try_unix
				&& !read_stdin
				&& (read_বাফার
				    || vim_lseek(fd, (off_T)0L, SEEK_SET)
								      == 0))
			    {
				fileformat = EOL_UNIX;
				যদি (set_options)
				    set_fileformat(EOL_UNIX, OPT_LOCAL);
				file_rewind = TRUE;
				keep_fileformat = TRUE;
				যাও retry;
			    }
			    ff_error = EOL_DOS;
			}
		    }
		    যদি (ml_append(lnum, সারি_start, len, newfile) == FAIL)
		    {
			error = TRUE;
			বিরতি;
		    }
#ifdef FEAT_PERSISTENT_UNDO
		    যদি (read_undo_file)
			sha256_update(&sha_ctx, সারি_start, len);
#endif
		    ++lnum;
		    যদি (--read_গুনতি == 0)
		    {
			error = TRUE;	    // break loop
			সারি_start = ptr;   // nothing left to write
			বিরতি;
		    }
		}
		অন্যথা
		    --skip_গুনতি;
		সারি_start = ptr + 1;
		++ptr;
	    }
	    আকার_2 = -1;
	}
	linerest = (long)(ptr - সারি_start);
	ui_breakcheck();
    }

failed:
    // not an error, max. number of lines reached
    যদি (error && read_গুনতি == 0)
	error = FALSE;

    // In Dos format ignore a trailing CTRL-Z, unless 'binary' is set.
    // In old days the file length was in sector count and the CTRL-Z the
    // marker where the file really ended.  Assuming we write it to a file
    // system that keeps file length properly the CTRL-Z should be dropped.
    // Set the 'endoffile' option so the user can decide what to write later.
    // In Unix format the CTRL-Z is just another character.
    যদি (linerest != 0
	    && !curbuf->b_p_bin
	    && fileformat == EOL_DOS
	    && ptr[-1] == Ctrl_Z)
    {
	ptr--;
	linerest--;
	যদি (set_options)
	    curbuf->b_p_eof = TRUE;
    }

    // If we get EOF in the middle of a line, note the fact by resetting
    // 'endofline' and add the line normally.
    যদি (!error
	    && !got_int
	    && linerest != 0)
    {
	// remember for when writing
	যদি (set_options)
	    curbuf->b_p_eol = FALSE;
	*ptr = NUL;
	len = (colnr_T)(ptr - সারি_start + 1);
	যদি (ml_append(lnum, সারি_start, len, newfile) == FAIL)
	    error = TRUE;
	অন্যথা
	{
#ifdef FEAT_PERSISTENT_UNDO
	    যদি (read_undo_file)
		sha256_update(&sha_ctx, সারি_start, len);
#endif
	    read_no_eol_lnum = ++lnum;
	}
    }

    যদি (set_options)
	save_file_ff(curbuf);		// remember the current file format

#ifdef FEAT_CRYPT
    যদি (curbuf->b_cryptstate != ফাঁকা)
    {
	crypt_free_state(curbuf->b_cryptstate);
	curbuf->b_cryptstate = ফাঁকা;
    }
    যদি (cryptkey != ফাঁকা && cryptkey != curbuf->b_p_চাবি)
	crypt_free_চাবি(cryptkey);
    // Don't set cryptkey to NULL, it's used below as a flag that
    // encryption was used.
#endif

    // If editing a new file: set 'fenc' for the current buffer.
    // Also for ":read ++edit file".
    যদি (set_options)
	set_string_option_direct((char_u *)"fenc", -1, fenc,
						       OPT_FREE|OPT_LOCAL, 0);
    যদি (fenc_alloced)
	vim_free(fenc);
#ifdef USE_ICONV
    যদি (iconv_fd != (iconv_t)-1)
	iconv_close(iconv_fd);
#endif

    যদি (!read_বাফার && !read_stdin)
	close(fd);				// errors are ignored
#ifdef HAVE_FD_CLOEXEC
    অন্যথা
    {
	পূর্ণসংখ্যা fdflags = fcntl(fd, F_GETFD);

	যদি (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (শূন্য)fcntl(fd, F_SETFD, fdflags | FD_CLOEXEC);
    }
#endif
    vim_free(বাফার);

#ifdef HAVE_DUP
    যদি (read_stdin)
    {
	// Use stderr for stdin, makes shell commands work.
	close(0);
	vim_ignored = dup(2);
    }
#endif

    যদি (tmpname != ফাঁকা)
    {
	mch_remove(tmpname);		// delete converted file
	vim_free(tmpname);
    }
    --no_wait_return;			// may wait for return now

    /*
     * In recovery mode everything but autocommands is skipped.
     */
    যদি (!recoverymode)
    {
	// need to delete the last line, which comes from the empty buffer
	যদি (newfile && wasempty && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	{
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 0;
#endif
	    ml_delete(curbuf->b_ml.ml_সারি_গুনতি);
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 1;
#endif
	    --linecnt;
	}
	linecnt = curbuf->b_ml.ml_সারি_গুনতি - linecnt;
	যদি (filesize == 0)
	    linecnt = 0;
	যদি (newfile || read_বাফার)
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
	অন্যথা যদি (linecnt)		// appended at least one line
	    appended_সারিগুলি_mark(from, linecnt);

#ifndef ALWAYS_USE_GUI
	/*
	 * If we were reading from the same terminal as where messages go,
	 * the screen will have been messed up.
	 * Switch on raw mode now and clear the screen.
	 */
	যদি (read_stdin)
	{
	    settmode(TMODE_RAW);	// set to raw mode
	    starttermcap();
	    screenclear();
	}
#endif

	যদি (got_int)
	{
	    যদি (!(flags & READ_DUMMY))
	    {
		filemess(curbuf, sfname, (char_u *)_(e_interrupted), 0);
		যদি (newfile)
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    }
	    বার্তা_scroll = বার্তা_save;
#ifdef FEAT_VIMINFO
	    check_marks_read();
#endif
	    retval = OK;	// an interrupt isn't really an error
	    যাও theend;
	}

	যদি (!filtering && !(flags & READ_DUMMY))
	{
	    পূর্ণসংখ্যা	buflen;

	    বার্তা_add_fname(curbuf, sfname);   // fname in IObuff with quotes
	    c = FALSE;

	    buflen = (পূর্ণসংখ্যা)STRLEN(IObuff);
#ifdef UNIX
	    যদি (S_ISFIFO(perm))			    // fifo
	    {
		buflen += vim_snprintf((অক্ষর *)IObuff + buflen, IOSIZE - buflen,
			_("[fifo]"));
		c = TRUE;
	    }
	    যদি (S_ISSOCK(perm))			    // or socket
	    {
		buflen += vim_snprintf((অক্ষর *)IObuff + buflen, IOSIZE - buflen,
			_("[socket]"));
		c = TRUE;
	    }
# ifdef OPEN_CHR_FILES
	    যদি (S_ISCHR(perm))			    // or character special
	    {
		buflen += vim_snprintf((অক্ষর *)IObuff + buflen, IOSIZE - buflen,
			_("[character special]"));
		c = TRUE;
	    }
# endif
#endif
	    যদি (curbuf->b_p_ro)
	    {
		buflen += vim_snprintf((অক্ষর *)IObuff + buflen, IOSIZE - buflen,
			"%s", shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
		c = TRUE;
	    }
	    যদি (read_no_eol_lnum)
	    {
		বার্তা_add_eol();
		c = TRUE;
	    }
	    যদি (ff_error == EOL_DOS)
	    {
		buflen += vim_snprintf((অক্ষর *)IObuff + buflen, IOSIZE - buflen,
			_("[CR missing]"));
		c = TRUE;
	    }
	    যদি (split)
	    {
		buflen += vim_snprintf((অক্ষর *)IObuff + buflen, IOSIZE - buflen,
			_("[long lines split]"));
		c = TRUE;
	    }
	    যদি (notconverted)
	    {
		buflen += vim_snprintf((অক্ষর *)IObuff + buflen, IOSIZE - buflen,
			_("[NOT converted]"));
		c = TRUE;
	    }
	    অন্যথা যদি (converted)
	    {
		buflen += vim_snprintf((অক্ষর *)IObuff + buflen, IOSIZE - buflen,
			_("[converted]"));
		c = TRUE;
	    }
#ifdef FEAT_CRYPT
	    যদি (cryptkey != ফাঁকা)
	    {
		crypt_append_বার্তা(curbuf);
		c = TRUE;
	    }
#endif
	    যদি (conv_error != 0)
	    {
		vim_snprintf((অক্ষর *)IObuff + buflen, IOSIZE - buflen,
			_("[CONVERSION ERROR in line %ld]"), (long)conv_error);
		c = TRUE;
	    }
	    অন্যথা যদি (illegal_byte > 0)
	    {
		vim_snprintf((অক্ষর *)IObuff + buflen, IOSIZE - buflen,
			_("[ILLEGAL BYTE in line %ld]"), (long)illegal_byte);
		c = TRUE;
	    }
	    অন্যথা যদি (error)
	    {
		vim_snprintf((অক্ষর *)IObuff + buflen, IOSIZE - buflen,
			_("[READ ERRORS]"));
		c = TRUE;
	    }
	    যদি (বার্তা_add_fileformat(fileformat))
		c = TRUE;
#ifdef FEAT_CRYPT
	    যদি (cryptkey != ফাঁকা)
		বার্তা_add_সারিগুলি(c, (long)linecnt, filesize
			 - crypt_get_header_len(crypt_get_method_nr(curbuf)));
	    অন্যথা
#endif
		বার্তা_add_সারিগুলি(c, (long)linecnt, filesize);

	    VIM_CLEAR(keep_বার্তা);
	    বার্তা_scrolled_ign = TRUE;
#ifdef ALWAYS_USE_GUI
	    // Don't show the message when reading stdin, it would end up in a
	    // message box (which might be shown when exiting!)
	    যদি (read_stdin || read_বাফার)
		p = বার্তা_may_trunc(FALSE, IObuff);
	    অন্যথা
#endif
	    {
		যদি (বার্তা_col > 0)
		    বার্তা_putchar('\r');  // overwrite previous message
		p = (char_u *)বার্তা_trunc_attr((অক্ষর *)IObuff, FALSE, 0);
	    }
	    যদি (read_stdin || read_বাফার || restart_edit != 0
		    || (বার্তা_scrolled != 0 && !need_wait_return))
		// Need to repeat the message after redrawing when:
		// - When reading from stdin (the screen will be cleared next).
		// - When restart_edit is set (otherwise there will be a delay
		//   before redrawing).
		// - When the screen was scrolled but there is no wait-return
		//   prompt.
		set_keep_বার্তা(p, 0);
	    বার্তা_scrolled_ign = FALSE;
	}

	// with errors writing the file requires ":w!"
	যদি (newfile && (error
		    || conv_error != 0
		    || (illegal_byte > 0 && bad_char_behavior != BAD_KEEP)))
	    curbuf->b_p_ro = TRUE;

	u_clearline();	    // cannot use "U" command after adding lines

	/*
	 * In Ex mode: cursor at last new line.
	 * Otherwise: cursor at first new line.
	 */
	যদি (exmode_active)
	    curwin->w_cursor.lnum = from + linecnt;
	অন্যথা
	    curwin->w_cursor.lnum = from + 1;
	check_cursor_lnum();
	beginline(BL_WHITE | BL_FIX);	    // on first non-blank

	যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
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
	যদি (newfile && !read_stdin && !read_বাফার
					 && mch_stat((অক্ষর *)fname, &st) >= 0)
	{
	    buf_store_time(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
	}
#endif
    }
    বার্তা_scroll = বার্তা_save;

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
    যদি (flags & READ_KEEP_UNDO)
	u_find_first_changed();

#ifdef FEAT_PERSISTENT_UNDO
    /*
     * When opening a new file locate undo info and read it.
     */
    যদি (read_undo_file)
    {
	char_u	hash[UNDO_HASH_আকার];

	sha256_finish(&sha_ctx, hash);
	u_read_undo(ফাঁকা, hash, fname);
    }
#endif

    যদি (!read_stdin && !read_fifo && (!read_বাফার || sfname != ফাঁকা))
    {
	পূর্ণসংখ্যা m = বার্তা_scroll;
	পূর্ণসংখ্যা n = বার্তা_scrolled;

	// Save the fileformat now, otherwise the buffer will be considered
	// modified if the format/encoding was automatically detected.
	যদি (set_options)
	    save_file_ff(curbuf);

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	বার্তা_scroll = TRUE;
	যদি (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPOST, ফাঁকা, sfname,
							  FALSE, curbuf, eap);
	অন্যথা যদি (newfile || (read_বাফার && sfname != ফাঁকা))
	{
	    apply_autocmds_exarg(EVENT_BUFREADPOST, ফাঁকা, sfname,
							  FALSE, curbuf, eap);
	    যদি (!curbuf->b_au_did_filetype && *curbuf->b_p_ft != NUL)
		/*
		 * EVENT_FILETYPE was not triggered but the buffer already has a
		 * filetype. Trigger EVENT_FILETYPE using the existing filetype.
		 */
		apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
			TRUE, curbuf);
	}
	অন্যথা
	    apply_autocmds_exarg(EVENT_FILEREADPOST, sfname, sfname,
							    FALSE, ফাঁকা, eap);
	যদি (বার্তা_scrolled == n)
	    বার্তা_scroll = m;
#ifdef FEAT_EVAL
	যদি (aborting())	    // autocmds may abort script processing
	    যাও theend;
#endif
    }

    যদি (!(recoverymode && error))
	retval = OK;

theend:
    যদি (curbuf->b_ml.ml_mfp != ফাঁকা
		       && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	// OK to sync the swap file now
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    ফেরত retval;
}

#যদি defined(OPEN_CHR_FILES)
/*
 * Returns TRUE if the file name argument is of the form "/dev/fd/\d\+",
 * which is the name of files used for process substitution output by
 * some shells on some operating systems, e.g., bash on SunOS.
 * Do not accept "/dev/fd/[012]", opening these may hang Vim.
 */
    পূর্ণসংখ্যা
is_dev_fd_file(char_u *fname)
{
    ফেরত (STRNCMP(fname, "/dev/fd/", 8) == 0
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
    স্থির linenr_T
readfile_linenr(
    linenr_T	linecnt,	// line count before reading more bytes
    char_u	*p,		// start of more bytes read
    char_u	*endp)		// end of more bytes read
{
    char_u	*s;
    linenr_T	lnum;

    lnum = curbuf->b_ml.ml_সারি_গুনতি - linecnt + 1;
    জন্য (s = p; s < endp; ++s)
	যদি (*s == '\n')
	    ++lnum;
    ফেরত lnum;
}

/*
 * Fill "*eap" to force the 'fileencoding', 'fileformat' and 'binary' to be
 * equal to the buffer "buf".  Used for calling readfile().
 * Returns OK or FAIL.
 */
    পূর্ণসংখ্যা
prep_exarg(exarg_T *eap, buf_T *buf)
{
    eap->cmd = alloc(15 + (unsigned)STRLEN(buf->b_p_fenc));
    যদি (eap->cmd == ফাঁকা)
	ফেরত FAIL;

    sprintf((অক্ষর *)eap->cmd, "e ++enc=%s", buf->b_p_fenc);
    eap->force_enc = 8;
    eap->bad_char = buf->b_bad_char;
    eap->force_ff = *buf->b_p_ff;

    eap->force_bin = buf->b_p_bin ? FORCE_BIN : FORCE_NOBIN;
    eap->read_edit = FALSE;
    eap->forceit = FALSE;
    ফেরত OK;
}

/*
 * Set default or forced 'fileformat' and 'binary'.
 */
    শূন্য
set_file_options(পূর্ণসংখ্যা set_options, exarg_T *eap)
{
    // set default 'fileformat'
    যদি (set_options)
    {
	যদি (eap != ফাঁকা && eap->force_ff != 0)
	    set_fileformat(get_fileformat_force(curbuf, eap), OPT_LOCAL);
	অন্যথা যদি (*p_ffs != NUL)
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    // set or reset 'binary'
    যদি (eap != ফাঁকা && eap->force_bin != 0)
    {
	পূর্ণসংখ্যা	oldval = curbuf->b_p_bin;

	curbuf->b_p_bin = (eap->force_bin == FORCE_BIN);
	set_options_bin(oldval, curbuf->b_p_bin, OPT_LOCAL);
    }
}

/*
 * Set forced 'fileencoding'.
 */
    শূন্য
set_forced_fenc(exarg_T *eap)
{
    যদি (eap->force_enc == 0)
	ফেরত;

    char_u *fenc = enc_canonize(eap->cmd + eap->force_enc);

    যদি (fenc != ফাঁকা)
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
    স্থির char_u *
পরবর্তী_fenc(char_u **pp, পূর্ণসংখ্যা *alloced)
{
    char_u	*p;
    char_u	*r;

    *alloced = FALSE;
    যদি (**pp == NUL)
    {
	*pp = ফাঁকা;
	ফেরত (char_u *)"";
    }
    p = vim_strchr(*pp, ',');
    যদি (p == ফাঁকা)
    {
	r = enc_canonize(*pp);
	*pp += STRLEN(*pp);
    }
    অন্যথা
    {
	r = vim_strnsave(*pp, p - *pp);
	*pp = p + 1;
	যদি (r != ফাঁকা)
	{
	    p = enc_canonize(r);
	    vim_free(r);
	    r = p;
	}
    }
    যদি (r != ফাঁকা)
	*alloced = TRUE;
    অন্যথা
    {
	// out of memory
	r = (char_u *)"";
	*pp = ফাঁকা;
    }
    ফেরত r;
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
    স্থির char_u *
readfile_charconvert(
    char_u	*fname,		// name of input file
    char_u	*fenc,		// converted from
    পূর্ণসংখ্যা		*fdp)		// in/out: file descriptor of file
{
    char_u	*tmpname;
    অক্ষর	*errmsg = ফাঁকা;

    tmpname = vim_tempname('r', FALSE);
    যদি (tmpname == ফাঁকা)
	errmsg = _("Can't find temp file for conversion");
    অন্যথা
    {
	close(*fdp);		// close the input file, ignore errors
	*fdp = -1;
	যদি (eval_charconvert(fenc, enc_utf8 ? (char_u *)"utf-8" : p_enc,
						      fname, tmpname) == FAIL)
	    errmsg = _("Conversion with 'charconvert' failed");
	যদি (errmsg == ফাঁকা && (*fdp = mch_open((অক্ষর *)tmpname,
						  O_RDONLY | O_EXTRA, 0)) < 0)
	    errmsg = _("can't read output of 'charconvert'");
    }

    যদি (errmsg != ফাঁকা)
    {
	// Don't use emsg(), it breaks mappings, the retry with
	// another type of conversion might still work.
	বার্তা(errmsg);
	যদি (tmpname != ফাঁকা)
	{
	    mch_remove(tmpname);	// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    // If the input file is closed, open it (caller should check for error).
    যদি (*fdp < 0)
	*fdp = mch_open((অক্ষর *)fname, O_RDONLY | O_EXTRA, 0);

    ফেরত tmpname;
}
#endif

#যদি defined(FEAT_CRYPT)
/*
 * Check for magic number used for encryption.  Applies to the current buffer.
 * If found, the magic number is removed from ptr[*sizep] and *sizep and
 * *filesizep are updated.
 * Return the (new) encryption key, NULL for no encryption.
 */
    স্থির char_u *
check_for_cryptkey(
    char_u	*cryptkey,	// previous encryption key or NULL
    char_u	*ptr,		// pointer to read bytes
    long	*sizep,		// length of read bytes
    off_T	*filesizep,	// nr of bytes used from file
    পূর্ণসংখ্যা		newfile,	// editing a new buffer
    char_u	*fname,		// file name to display
    পূর্ণসংখ্যা		*did_ask)	// flag: whether already asked for key
{
    পূর্ণসংখ্যা method = crypt_method_nr_from_magic((অক্ষর *)ptr, *sizep);
    পূর্ণসংখ্যা b_p_ro = curbuf->b_p_ro;

    যদি (method >= 0)
    {
	// Mark the buffer as read-only until the decryption has taken place.
	// Avoids accidentally overwriting the file with garbage.
	curbuf->b_p_ro = TRUE;

	// Set the cryptmethod local to the buffer.
	crypt_set_cm_option(curbuf, method);
	যদি (cryptkey == ফাঁকা && !*did_ask)
	{
	    যদি (*curbuf->b_p_চাবি)
	    {
		cryptkey = curbuf->b_p_চাবি;
		crypt_check_swapfile_curbuf();
	    }
	    অন্যথা
	    {
		// When newfile is TRUE, store the typed key in the 'key'
		// option and don't free it.  bf needs hash of the key saved.
		// Don't ask for the key again when first time Enter was hit.
		// Happens when retrying to detect encoding.
		smsg(_(need_চাবি_বার্তা), fname);
		বার্তা_scroll = TRUE;
		crypt_check_method(method);
		cryptkey = crypt_get_চাবি(newfile, FALSE);
		*did_ask = TRUE;

		// check if empty key entered
		যদি (cryptkey != ফাঁকা && *cryptkey == NUL)
		{
		    যদি (cryptkey != curbuf->b_p_চাবি)
			vim_free(cryptkey);
		    cryptkey = ফাঁকা;
		}
	    }
	}

	যদি (cryptkey != ফাঁকা)
	{
	    পূর্ণসংখ্যা header_len;

	    header_len = crypt_get_header_len(method);
	    যদি (*sizep < header_len)
	    {
		// invalid header, buffer can't be encrypted
		যদি (cryptkey != curbuf->b_p_চাবি)
		    vim_free(cryptkey);
		ফেরত ফাঁকা;
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
    অন্যথা যদি (newfile && *curbuf->b_p_চাবি != NUL && !starting)
	set_option_মান_give_err((char_u *)"key", 0L, (char_u *)"", OPT_LOCAL);

    ফেরত cryptkey;
}
#endif  // FEAT_CRYPT

/*
 * Return TRUE if a file appears to be read-only from the file permissions.
 */
    পূর্ণসংখ্যা
check_file_readonly(
    char_u	*fname,		// full path to file
    পূর্ণসংখ্যা		perm UNUSED)	// known permissions on file
{
#ifndef USE_MCH_ACCESS
    পূর্ণসংখ্যা	    fd = 0;
#endif

    ফেরত (
#ifdef USE_MCH_ACCESS
# ifdef UNIX
	(perm & 0222) == 0 ||
# endif
	mch_access((অক্ষর *)fname, W_OK)
#অন্যথা
	(fd = mch_open((অক্ষর *)fname, O_RDWR | O_EXTRA, 0)) < 0
					? TRUE : (close(fd), FALSE)
#endif
	);
}

#যদি defined(HAVE_FSYNC)
/*
 * Call fsync() with Mac-specific exception.
 * Return fsync() result: zero for success.
 */
    পূর্ণসংখ্যা
vim_fsync(পূর্ণসংখ্যা fd)
{
    পূর্ণসংখ্যা r;

# ifdef MACOS_X
    r = fcntl(fd, F_FULLFSYNC);
    যদি (r != 0)  // F_FULLFSYNC not working or not supported
# endif
	r = fsync(fd);
    ফেরত r;
}
#endif

/*
 * Set the name of the current buffer.  Use when the buffer doesn't have a
 * name and a ":r" or ":w" command with a file name is used.
 */
    পূর্ণসংখ্যা
set_rw_fname(char_u *fname, char_u *sfname)
{
    buf_T	*buf = curbuf;

    // It's like the unnamed buffer is deleted....
    যদি (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFDELETE, ফাঁকা, ফাঁকা, FALSE, curbuf);
    apply_autocmds(EVENT_BUFWIPEOUT, ফাঁকা, ফাঁকা, FALSE, curbuf);
#ifdef FEAT_EVAL
    যদি (aborting())	    // autocmds may abort script processing
	ফেরত FAIL;
#endif
    যদি (curbuf != buf)
    {
	// We are in another buffer now, don't do the renaming.
	emsg(_(e_autocommands_changed_বাফার_or_বাফার_name));
	ফেরত FAIL;
    }

    যদি (setfname(curbuf, fname, sfname, FALSE) == OK)
	curbuf->b_flags |= BF_NOTEDITED;

    // ....and a new named one is created
    apply_autocmds(EVENT_BUFNEW, ফাঁকা, ফাঁকা, FALSE, curbuf);
    যদি (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFADD, ফাঁকা, ফাঁকা, FALSE, curbuf);
#ifdef FEAT_EVAL
    যদি (aborting())	    // autocmds may abort script processing
	ফেরত FAIL;
#endif

    // Do filetype detection now if 'filetype' is empty.
    যদি (*curbuf->b_p_ft == NUL)
    {
	যদি (au_has_group((char_u *)"filetypedetect"))
	    (শূন্য)do_doautocmd((char_u *)"filetypedetect BufRead", FALSE, ফাঁকা);
	do_modelines(0);
    }

    ফেরত OK;
}

/*
 * Put file name into IObuff with quotes.
 */
    শূন্য
বার্তা_add_fname(buf_T *buf, char_u *fname)
{
    size_t  IObufflen = 0;

    যদি (fname == ফাঁকা)
	fname = (char_u *)"-stdin-";
    IObuff[IObufflen++] = '"';
    IObufflen += home_replace(buf, fname, IObuff + IObufflen, IOSIZE - 4, TRUE);
    STRCPY(IObuff + IObufflen, "\" ");
}

/*
 * Append message for text mode to IObuff.
 * Return TRUE if something appended.
 */
    পূর্ণসংখ্যা
বার্তা_add_fileformat(পূর্ণসংখ্যা eol_type)
{
#ifndef USE_CRNL
    যদি (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[dos]") : _("[dos format]"));
	ফেরত TRUE;
    }
#endif
    যদি (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[mac]") : _("[mac format]"));
	ফেরত TRUE;
    }
#ifdef USE_CRNL
    যদি (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[unix]") : _("[unix format]"));
	ফেরত TRUE;
    }
#endif
    ফেরত FALSE;
}

/*
 * Append line and character count to IObuff.
 */
    শূন্য
বার্তা_add_সারিগুলি(
    পূর্ণসংখ্যা	    ঢোকাও_space,
    long    lnum,
    off_T   nchars)
{
    পূর্ণসংখ্যা  len = (পূর্ণসংখ্যা)STRLEN(IObuff);

    যদি (shortmess(SHM_সারিগুলি))
	vim_snprintf((অক্ষর *)IObuff + len, IOSIZE - (size_t)len,
		// l10n: L as in line, B as in byte
		_("%s%ldL, %lldB"), ঢোকাও_space ? " " : "", lnum, (varnumber_T)nchars);
    অন্যথা
    {
	len += vim_snprintf((অক্ষর *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%s%ld line, ", "%s%ld lines, ", lnum), ঢোকাও_space ? " " : "", lnum);
	vim_snprintf((অক্ষর *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%lld byte", "%lld bytes", nchars), (varnumber_T)nchars);
    }
}

/*
 * Append message for missing line separator to IObuff.
 */
    শূন্য
বার্তা_add_eol(শূন্য)
{
    STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]") : _("[Incomplete last line]"));
}

    পূর্ণসংখ্যা
time_differs(stat_T *st, long mtime, long mtime_ns UNUSED)
{
    ফেরত
#ifdef ST_MTIM_NSEC
	(long)st->ST_MTIM_NSEC != mtime_ns ||
#endif
#যদি defined(__linux__) || defined(MSWIN)
	// On a FAT filesystem, esp. under Linux, there are only 5 bits to store
	// the seconds.  Since the roundoff is done when flushing the inode, the
	// time may change unexpectedly by one second!!!
	(long)st->st_mtime - mtime > 1 || mtime - (long)st->st_mtime > 1
#অন্যথা
	(long)st->st_mtime != mtime
#endif
	;
}

/*
 * Return TRUE if file encoding "fenc" requires conversion from or to
 * 'encoding'.
 */
    পূর্ণসংখ্যা
need_conversion(char_u *fenc)
{
    পূর্ণসংখ্যা		same_encoding;
    পূর্ণসংখ্যা		enc_flags;
    পূর্ণসংখ্যা		fenc_flags;

    যদি (*fenc == NUL || STRCMP(p_enc, fenc) == 0)
    {
	same_encoding = TRUE;
	fenc_flags = 0;
    }
    অন্যথা
    {
	// Ignore difference between "ansi" and "latin1", "ucs-4" and
	// "ucs-4be", etc.
	enc_flags = get_fio_flags(p_enc);
	fenc_flags = get_fio_flags(fenc);
	same_encoding = (enc_flags != 0 && fenc_flags == enc_flags);
    }
    যদি (same_encoding)
    {
	// Specified encoding matches with 'encoding'.  This requires
	// conversion when 'encoding' is Unicode but not UTF-8.
	ফেরত enc_unicode != 0;
    }

    // Encodings differ.  However, conversion is not needed when 'enc' is any
    // Unicode encoding and the file is UTF-8.
    ফেরত !(enc_utf8 && fenc_flags == FIO_UTF8);
}

/*
 * Check "ptr" for a unicode encoding and return the FIO_ flags needed for the
 * internal conversion.
 * if "ptr" is an empty string, use 'encoding'.
 */
    পূর্ণসংখ্যা
get_fio_flags(char_u *ptr)
{
    পূর্ণসংখ্যা		prop;

    যদি (*ptr == NUL)
	ptr = p_enc;

    prop = enc_canon_props(ptr);
    যদি (prop & ENC_UNICODE)
    {
	যদি (prop & ENC_2BYTE)
	{
	    যদি (prop & ENC_ENDIAN_L)
		ফেরত FIO_UCS2 | FIO_ENDIAN_L;
	    ফেরত FIO_UCS2;
	}
	যদি (prop & ENC_4BYTE)
	{
	    যদি (prop & ENC_ENDIAN_L)
		ফেরত FIO_UCS4 | FIO_ENDIAN_L;
	    ফেরত FIO_UCS4;
	}
	যদি (prop & ENC_2WORD)
	{
	    যদি (prop & ENC_ENDIAN_L)
		ফেরত FIO_UTF16 | FIO_ENDIAN_L;
	    ফেরত FIO_UTF16;
	}
	ফেরত FIO_UTF8;
    }
    যদি (prop & ENC_LATIN1)
	ফেরত FIO_LATIN1;
    // must be ENC_DBCS, requires iconv()
    ফেরত 0;
}

#যদি defined(MSWIN)
/*
 * Check "ptr" for a MS-Windows codepage name and return the FIO_ flags needed
 * for the conversion MS-Windows can do for us.  Also accept "utf-8".
 * Used for conversion between 'encoding' and 'fileencoding'.
 */
    পূর্ণসংখ্যা
get_win_fio_flags(char_u *ptr)
{
    পূর্ণসংখ্যা		cp;

    // Cannot do this when 'encoding' is not utf-8 and not a codepage.
    যদি (!enc_utf8 && enc_codepage <= 0)
	ফেরত 0;

    cp = encname2codepage(ptr);
    যদি (cp == 0)
    {
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	যদি (STRCMP(ptr, "utf-8") == 0)
	    cp = CP_UTF8;
	অন্যথা
# endif
	    ফেরত 0;
    }
    ফেরত FIO_PUT_CP(cp) | FIO_CODEPAGE;
}
#endif

#যদি defined(MACOS_CONVERT)
/*
 * Check "ptr" for a Carbon supported encoding and return the FIO_ flags
 * needed for the internal conversion to/from utf-8 or latin1.
 */
    পূর্ণসংখ্যা
get_mac_fio_flags(char_u *ptr)
{
    যদি ((enc_utf8 || STRCMP(p_enc, "latin1") == 0)
				     && (enc_canon_props(ptr) & ENC_MACROMAN))
	ফেরত FIO_MACROMAN;
    ফেরত 0;
}
#endif

/*
 * Check for a Unicode BOM (Byte Order Mark) at the start of p[size].
 * "size" must be at least 2.
 * Return the name of the encoding and set "*lenp" to the length.
 * Returns NULL when no BOM found.
 */
    স্থির char_u *
check_for_bom(
    char_u	*p,
    long	আকার_2,
    পূর্ণসংখ্যা		*lenp,
    পূর্ণসংখ্যা		flags)
{
    অক্ষর	*name = ফাঁকা;
    পূর্ণসংখ্যা		len = 2;

    যদি (p[0] == 0xef && p[1] == 0xbb && আকার_2 >= 3 && p[2] == 0xbf
	    && (flags == FIO_ALL || flags == FIO_UTF8 || flags == 0))
    {
	name = "utf-8";		// EF BB BF
	len = 3;
    }
    অন্যথা যদি (p[0] == 0xff && p[1] == 0xfe)
    {
	যদি (আকার_2 >= 4 && p[2] == 0 && p[3] == 0
	    && (flags == FIO_ALL || flags == (FIO_UCS4 | FIO_ENDIAN_L)))
	{
	    name = "ucs-4le";	// FF FE 00 00
	    len = 4;
	}
	অন্যথা যদি (flags == (FIO_UCS2 | FIO_ENDIAN_L))
	    name = "ucs-2le";	// FF FE
	অন্যথা যদি (flags == FIO_ALL || flags == (FIO_UTF16 | FIO_ENDIAN_L))
	    // utf-16le is preferred, it also works for ucs-2le text
	    name = "utf-16le";	// FF FE
    }
    অন্যথা যদি (p[0] == 0xfe && p[1] == 0xff
	    && (flags == FIO_ALL || flags == FIO_UCS2 || flags == FIO_UTF16))
    {
	// Default to utf-16, it works also for ucs-2 text.
	যদি (flags == FIO_UCS2)
	    name = "ucs-2";	// FE FF
	অন্যথা
	    name = "utf-16";	// FE FF
    }
    অন্যথা যদি (আকার_2 >= 4 && p[0] == 0 && p[1] == 0 && p[2] == 0xfe
	    && p[3] == 0xff && (flags == FIO_ALL || flags == FIO_UCS4))
    {
	name = "ucs-4";		// 00 00 FE FF
	len = 4;
    }

    *lenp = len;
    ফেরত (char_u *)name;
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
    যদি (dirname == ফাঁকা)
	ফেরত full_path;
    যদি (mch_dirname(dirname, MAXPATHL) == OK)
    {
	p = shorten_fname(full_path, dirname);
	যদি (p == ফাঁকা || *p == NUL)
	    p = full_path;
    }
    vim_free(dirname);
    ফেরত p;
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
    পূর্ণসংখ্যা		len;
    char_u	*p;

    যদি (full_path == ফাঁকা)
	ফেরত ফাঁকা;
    len = (পূর্ণসংখ্যা)STRLEN(dir_name);
    যদি (fnamencmp(dir_name, full_path, len) == 0)
    {
	p = full_path + len;
#যদি defined(MSWIN)
	/*
	 * MS-Windows: when a file is in the root directory, dir_name will end
	 * in a slash, since C: by itself does not define a specific dir. In
	 * this case p may already be correct. <negri>
	 */
	যদি (!((len > 2) && (*(p - 2) == ':')))
#endif
	{
	    যদি (vim_ispathsep(*p))
		করো
		    ++p;
		যখন (vim_ispathsep_nocolon(*p));
#ifndef VMS   // the path separator is always part of the path
	    অন্যথা
		p = ফাঁকা;
#endif
	}
    }
#যদি defined(MSWIN)
    /*
     * When using a file in the current drive, remove the drive name:
     * "A:\dir\file" -> "\dir\file".  This helps when moving a session file on
     * a floppy from "A:\dir" to "B:\dir".
     */
    অন্যথা যদি (len > 3
	    && TOUPPER_LOC(full_path[0]) == TOUPPER_LOC(dir_name[0])
	    && full_path[1] == ':'
	    && vim_ispathsep(full_path[2]))
	p = full_path + 2;
#endif
    অন্যথা
	p = ফাঁকা;
    ফেরত p;
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
    শূন্য
shorten_buf_fname(buf_T *buf, char_u *dirname, পূর্ণসংখ্যা force)
{
    char_u	*p;

    যদি (buf->b_fname != ফাঁকা
	    && !bt_nofilename(buf)
	    && !path_with_url(buf->b_fname)
	    && (force
		|| buf->b_sfname == ফাঁকা
		|| mch_isFullName(buf->b_sfname)))
    {
	যদি (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	p = shorten_fname(buf->b_ffname, dirname);
	যদি (p != ফাঁকা)
	{
	    buf->b_sfname = vim_strsave(p);
	    buf->b_fname = buf->b_sfname;
	}
	যদি (p == ফাঁকা || buf->b_fname == ফাঁকা)
	    buf->b_fname = buf->b_ffname;
    }
}

/*
 * Shorten filenames for all buffers.
 */
    শূন্য
shorten_fnames(পূর্ণসংখ্যা force)
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
#যদি defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    popup_update_preview_title();
#endif
}

#যদি (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
/*
 * Shorten all filenames in "fnames[count]" by current directory.
 */
    শূন্য
shorten_filenames(char_u **fnames, পূর্ণসংখ্যা গুনতি)
{
    পূর্ণসংখ্যা		i;
    char_u	dirname[MAXPATHL];
    char_u	*p;

    যদি (fnames == ফাঁকা || গুনতি < 1)
	ফেরত;
    mch_dirname(dirname, আকার(dirname));
    জন্য (i = 0; i < গুনতি; ++i)
    {
	যদি ((p = shorten_fname(fnames[i], dirname)) != ফাঁকা)
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
    পূর্ণসংখ্যা	    prepend_dot)	// may prepend a '.' to file name
{
    ফেরত buf_modname((curbuf->b_p_sn || curbuf->b_shortname),
						     fname, ext, prepend_dot);
}

    char_u *
buf_modname(
    পূর্ণসংখ্যা	    shortname,		// use 8.3 file name
    char_u  *fname,
    char_u  *ext,
    পূর্ণসংখ্যা	    prepend_dot)	// may prepend a '.' to file name
{
    char_u	*retval;
    char_u	*s;
    char_u	*e;
    char_u	*ptr;
    size_t	ptrlen;
    পূর্ণসংখ্যা		fnamelen, extlen;

    extlen = (পূর্ণসংখ্যা)STRLEN(ext);

    /*
     * If there is no file name we must get the name of the current directory
     * (we need the full path in case :cd is used).
     */
    যদি (fname == ফাঁকা || *fname == NUL)
    {
	retval = alloc(MAXPATHL + extlen + 3);
	যদি (retval == ফাঁকা)
	    ফেরত ফাঁকা;
	যদি (mch_dirname(retval, MAXPATHL) == FAIL ||
				     (fnamelen = (পূর্ণসংখ্যা)STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    ফেরত ফাঁকা;
	}
	যদি (!after_pathsep(retval, retval + fnamelen))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
	prepend_dot = FALSE;	    // nothing to prepend a dot to
    }
    অন্যথা
    {
	fnamelen = (পূর্ণসংখ্যা)STRLEN(fname);
	retval = alloc(fnamelen + extlen + 3);
	যদি (retval == ফাঁকা)
	    ফেরত ফাঁকা;
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
    জন্য (ptr = retval + fnamelen; ptr > retval; MB_PTR_BACK(retval, ptr))
    {
	যদি (*ext == '.' && shortname)
	    যদি (*ptr == '.')	// replace '.' by '_'
		*ptr = '_';
	যদি (vim_ispathsep(*ptr))
	{
	    ++ptr;
	    বিরতি;
	}
    }

    // the file name has at most BASENAMELEN characters.
    ptrlen = (size_t)(fnamelen - (ptr - retval));
    যদি (ptrlen > (unsigned)BASENAMELEN)
    {
	ptrlen = BASENAMELEN;
	ptr[ptrlen] = NUL;
    }

    s = ptr + ptrlen;

    /*
     * For 8.3 file names we may have to reduce the length.
     */
    যদি (shortname)
    {
	/*
	 * If there is no file name, or the file name ends in '/', and the
	 * extension starts with '.', put a '_' before the dot, because just
	 * ".ext" is invalid.
	 */
	যদি (fname == ফাঁকা || *fname == NUL
				   || vim_ispathsep(fname[STRLEN(fname) - 1]))
	{
	    যদি (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	অন্যথা যদি (*ext == '.')
	{
	    যদি ((size_t)(s - ptr) > (size_t)8)
	    {
		s = ptr + 8;
		*s = '\0';
	    }
	}
	/*
	 * If the extension doesn't start with '.', and the file name
	 * doesn't have an extension yet, append a '.'
	 */
	অন্যথা যদি ((e = vim_strchr(ptr, '.')) == ফাঁকা)
	    *s++ = '.';
	/*
	 * If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be truncated
	 */
	অন্যথা যদি ((পূর্ণসংখ্যা)(ptrlen - (e - retval)) + extlen > 4)
	    s = e + 4 - extlen;
    }
#ifdef MSWIN
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    অন্যথা যদি ((fname == ফাঁকা || *fname == NUL) && *ext == '.')
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
    যদি (prepend_dot && !shortname && *(e = gettail(retval)) != '.')
    {
	mch_memmove(e + 1, e, (size_t)(((fnamelen + extlen) - (e - retval)) + 1));	// +1 for NUL
	*e = '.';
    }

    /*
     * Check that, after appending the extension, the file name is really
     * different.
     */
    যদি (fname != ফাঁকা && STRCMP(fname, retval) == 0)
    {
	// we search for a character that can be replaced by '_'
	যখন (--s >= ptr)
	{
	    যদি (*s != '_')
	    {
		*s = '_';
		বিরতি;
	    }
	}
	যদি (s < ptr)	// fname was "________.<ext>", how tricky!
	    *ptr = 'v';
    }
    ফেরত retval;
}

/*
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE for end-of-file.
 * If the line is truncated then buf[size - 2] will not be NUL.
 */
    পূর্ণসংখ্যা
vim_fgets(char_u *buf, পূর্ণসংখ্যা আকার_2, FILE *fp)
{
    অক্ষর	*eof;
#define FGETS_আকার 200
    অক্ষর	tbuf[FGETS_আকার];

    // safety check
    যদি (আকার_2 < 2)
    {
	যদি (আকার_2 == 1)
	    buf[0] = NUL;
	ফেরত TRUE;
    }

    buf[আকার_2 - 2] = NUL;
    eof = fgets((অক্ষর *)buf, আকার_2, fp);
    যদি (buf[আকার_2 - 2] != NUL && buf[আকার_2 - 2] != '\n')
    {
	buf[আকার_2 - 1] = NUL;	    // Truncate the line

	// Now throw away the rest of the line:
	করো
	{
	    tbuf[FGETS_আকার - 2] = NUL;
	    vim_ignoredp = fgets((অক্ষর *)tbuf, FGETS_আকার, fp);
	} যখন (tbuf[FGETS_আকার - 2] != NUL && tbuf[FGETS_আকার - 2] != '\n');
    }
    ফেরত (eof == ফাঁকা);
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 * Return -1 for failure, 0 for success.
 */
    পূর্ণসংখ্যা
vim_rename(char_u *from, char_u *to)
{
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		ret;
#ifdef AMIGA
    BPTR	flock;
#endif
    stat_T	st;
    পূর্ণসংখ্যা		use_tmp_file = FALSE;

    /*
     * When the names are identical, there is nothing to do.  When they refer
     * to the same file (ignoring case and slash/backslash differences) but
     * the file name differs we need to go through a temp file.
     */
    যদি (fnamecmp(from, to) == 0)
    {
	যদি (p_fic && STRCMP(gettail(from), gettail(to)) != 0)
	    use_tmp_file = TRUE;
	অন্যথা
	    ফেরত 0;
    }

    /*
     * Fail if the "from" file doesn't exist.  Avoids that "to" is deleted.
     */
    যদি (mch_stat((অক্ষর *)from, &st) < 0)
	ফেরত -1;

#ifdef UNIX
    {
	stat_T	st_to;

	// It's possible for the source and destination to be the same file.
	// This happens when "from" and "to" differ in case and are on a FAT32
	// filesystem.  In that case go through a temp file name.
	যদি (mch_stat((অক্ষর *)to, &st_to) >= 0
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
	যদি (win32_fileinfo(from, &info1) == FILEINFO_OK
		&& win32_fileinfo(to, &info2) == FILEINFO_OK
		&& info1.dw_Volume_Serial_সংখ্যা == info2.dw_Volume_Serial_সংখ্যা
		&& info1.n_File_সূচক_উপরের == info2.n_File_সূচক_উপরের
		&& info1.n_File_সূচক_নিচের == info2.n_File_সূচক_নিচের)
	    use_tmp_file = TRUE;
    }
#endif

    যদি (use_tmp_file)
    {
	অক্ষর	tempname[MAXPATHL + 1];

	/*
	 * Find a name that doesn't exist and is in the same directory.
	 * Rename "from" to "tempname" and then rename "tempname" to "to".
	 */
	যদি (STRLEN(from) >= MAXPATHL - 5)
	    ফেরত -1;
	STRCPY(tempname, from);
	জন্য (n = 123; n < 99999; ++n)
	{
	    sprintf((অক্ষর *)gettail((char_u *)tempname), "%d", n);
	    যদি (mch_stat(tempname, &st) < 0)
	    {
		যদি (mch_rename((অক্ষর *)from, tempname) == 0)
		{
		    যদি (mch_rename(tempname, (অক্ষর *)to) == 0)
			ফেরত 0;
		    // Strange, the second step failed.  Try moving the
		    // file back and return failure.
		    (শূন্য)mch_rename(tempname, (অক্ষর *)from);
		    ফেরত -1;
		}
		// If it fails for one temp name it will most likely fail
		// for any temp name, give up.
		ফেরত -1;
	    }
	}
	ফেরত -1;
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
    যদি (flock)
	UnLock(flock);
#endif

    /*
     * First try a normal rename, return if it works.
     */
    যদি (mch_rename((অক্ষর *)from, (অক্ষর *)to) == 0)
	ফেরত 0;

    /*
     * Rename() failed, try copying the file.
     */
    ret = vim_copyfile(from, to);
    যদি (ret != OK)
	ফেরত -1;

    /*
     * Remove copied original file
     */
    যদি (mch_stat((অক্ষর *)from, &st) >= 0)
	mch_remove(from);

    ফেরত 0;
}


/*
 * Create the new file with same permissions as the original.
 * Return FAIL for failure, OK for success.
 */
    পূর্ণসংখ্যা
vim_copyfile(char_u *from, char_u *to)
{
    পূর্ণসংখ্যা		fd_in;
    পূর্ণসংখ্যা		fd_out;
    পূর্ণসংখ্যা		n;
    অক্ষর	*errmsg = ফাঁকা;
    অক্ষর	*বাফার;
    long	perm;
#ifdef HAVE_ACL
    vim_acl_T	acl;		// ACL from original file
#endif

#ifdef HAVE_READLINK
    পূর্ণসংখ্যা		ret;
    পূর্ণসংখ্যা		len;
    stat_T	st;
    অক্ষর	linkbuf[MAXPATHL + 1];

    ret = mch_lstat((অক্ষর *)from, &st);
    যদি (ret >= 0 && S_ISLNK(st.st_mode))
    {
	ret = -1;

	len = readlink((অক্ষর *)from, linkbuf, MAXPATHL);
	যদি (len > 0)
	{
	    linkbuf[len] = NUL;

	    // Create link
	    ret = symlink(linkbuf, (অক্ষর *)to);
	}

	ফেরত ret == 0 ? OK : FAIL;
    }
#endif

    perm = mch_getperm(from);
#ifdef HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    acl = mch_get_acl(from);
#endif
    fd_in = mch_open((অক্ষর *)from, O_RDONLY|O_EXTRA, 0);
    যদি (fd_in == -1)
    {
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	ফেরত FAIL;
    }

    // Create the new file with same permissions as the original.
    fd_out = mch_open((অক্ষর *)to,
		       O_CREAT|O_EXCL|O_WRONLY|O_EXTRA|O_NOFOLLOW, (পূর্ণসংখ্যা)perm);
    যদি (fd_out == -1)
    {
	close(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	ফেরত FAIL;
    }

    বাফার = alloc(WRITEBUFSIZE);
    যদি (বাফার == ফাঁকা)
    {
	close(fd_out);
	close(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	ফেরত FAIL;
    }

    যখন ((n = read_eintr(fd_in, বাফার, WRITEBUFSIZE)) > 0)
	যদি (write_eintr(fd_out, বাফার, n) != n)
	{
	    errmsg = _(e_error_writing_to_str);
	    বিরতি;
	}

    vim_free(বাফার);
    close(fd_in);
    যদি (close(fd_out) < 0)
	errmsg = _(e_error_closing_str);
    যদি (n < 0)
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
#যদি defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    mch_copy_sec(from, to);
#endif
    যদি (errmsg != ফাঁকা)
    {
	semsg(errmsg, to);
	ফেরত FAIL;
    }
    ফেরত OK;
}

স্থির পূর্ণসংখ্যা already_warned = FALSE;

/*
 * Check if any not hidden buffer has been changed.
 * Postpone the check if there are characters in the stuff buffer, a global
 * command is being executed, a mapping is being executed or an autocommand is
 * busy.
 * Returns TRUE if some message was written (screen should be redrawn and
 * cursor positioned).
 */
    পূর্ণসংখ্যা
check_timestamps(
    পূর্ণসংখ্যা		focus)		// called for GUI focus event
{
    buf_T	*buf;
    পূর্ণসংখ্যা		didit = 0;
    পূর্ণসংখ্যা		n;

    // Don't check timestamps while system() or another low-level function may
    // cause us to lose and gain focus.
    যদি (no_check_timestamps > 0)
	ফেরত FALSE;

    // Avoid doing a check twice.  The OK/Reload dialog can cause a focus
    // event and we would keep on checking if the file is steadily growing.
    // Do check again after typing something.
    যদি (focus && did_check_timestamps)
    {
	need_check_timestamps = TRUE;
	ফেরত FALSE;
    }

    যদি (!stuff_empty() || global_busy || !typebuf_typed()
			|| autocmd_busy || curbuf_lock > 0 || allbuf_lock > 0)
	need_check_timestamps = TRUE;		// check later
    অন্যথা
    {
	++no_wait_return;
	did_check_timestamps = TRUE;
	already_warned = FALSE;
	FOR_ALL_BUFFERS(buf)
	{
	    // Only check buffers in a window.
	    যদি (buf->b_nwindows > 0)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		n = buf_check_timestamp(buf, focus);
		যদি (didit < n)
		    didit = n;
		যদি (n > 0 && !bufref_valid(&bufref))
		{
		    // Autocommands have removed the buffer, start at the
		    // first one again.
		    buf = firstbuf;
		    চালু;
		}
	    }
	}
	--no_wait_return;
	need_check_timestamps = FALSE;
	যদি (need_wait_return && didit == 2)
	{
	    // make sure msg isn't overwritten
	    বার্তা_puts("\n");
	    out_flush();
	}
    }
    ফেরত didit;
}

/*
 * Move all the lines from buffer "frombuf" to buffer "tobuf".
 * Return OK or FAIL.  When FAIL "tobuf" is incomplete and/or "frombuf" is not
 * empty.
 */
    স্থির পূর্ণসংখ্যা
move_সারিগুলি(buf_T *frombuf, buf_T *tobuf)
{
    buf_T	*tbuf = curbuf;
    পূর্ণসংখ্যা		retval = OK;
    linenr_T	lnum;
    char_u	*p;

    // Copy the lines in "frombuf" to "tobuf".
    curbuf = tobuf;
    জন্য (lnum = 1; lnum <= frombuf->b_ml.ml_সারি_গুনতি; ++lnum)
    {
	p = vim_strnsave(ml_get_buf(frombuf, lnum, FALSE), ml_get_buf_len(frombuf, lnum));
	যদি (p == ফাঁকা || ml_append(lnum - 1, p, 0, FALSE) == FAIL)
	{
	    vim_free(p);
	    retval = FAIL;
	    বিরতি;
	}
	vim_free(p);
    }

    // Delete all the lines in "frombuf".
    যদি (retval != FAIL)
    {
	curbuf = frombuf;
	জন্য (lnum = curbuf->b_ml.ml_সারি_গুনতি; lnum > 0; --lnum)
	    যদি (ml_delete(lnum) == FAIL)
	    {
		// Oops!  We could try putting back the saved lines, but that
		// might fail again...
		retval = FAIL;
		বিরতি;
	    }
    }

    curbuf = tbuf;
    ফেরত retval;
}

/*
 * Check if buffer "buf" has been changed.
 * Also check if the file for a new buffer unexpectedly appeared.
 * return 1 if a changed buffer was found.
 * return 2 if a message has been displayed.
 * return 0 otherwise.
 */
    পূর্ণসংখ্যা
buf_check_timestamp(
    buf_T	*buf,
    পূর্ণসংখ্যা		focus UNUSED)	// called for GUI focus event
{
    stat_T	st;
    পূর্ণসংখ্যা		stat_res;
    পূর্ণসংখ্যা		retval = 0;
    অক্ষর	*mesg = ফাঁকা;
    অক্ষর	*mesg2 = "";
    পূর্ণসংখ্যা		helpmesg = FALSE;
    গণনা {
	RELOAD_NONE,
	RELOAD_NORMAL,
	RELOAD_DETECT
    }		reload = RELOAD_NONE;
#যদি defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
    পূর্ণসংখ্যা		can_reload = FALSE;
#endif
    off_T	orig_আকার = buf->b_orig_আকার;
    পূর্ণসংখ্যা		orig_mode = buf->b_orig_mode;
#ifdef FEAT_GUI
    পূর্ণসংখ্যা		save_mouse_correct = need_mouse_correct;
#endif
    স্থির পূর্ণসংখ্যা	busy = FALSE;
    পূর্ণসংখ্যা		n;
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    // If there is no file name, the buffer is not loaded, 'buftype' is
    // set, we are in the middle of a save or being called recursively: ignore
    // this buffer.
    যদি (buf->b_ffname == ফাঁকা
	    || buf->b_ml.ml_mfp == ফাঁকা
	    || !bt_normal(buf)
	    || buf->b_saving
	    || busy
#ifdef FEAT_NETBEANS_INTG
	    || is_Netbeans_বাফার(buf)
#endif
#ifdef FEAT_TERMINAL
	    || buf->b_term != ফাঁকা
#endif
	    )
	ফেরত 0;

    যদি (       !(buf->b_flags & BF_NOTEDITED)
	    && buf->b_mtime != 0
	    && ((stat_res = mch_stat((অক্ষর *)buf->b_ffname, &st)) < 0
		|| time_differs(&st, buf->b_mtime, buf->b_mtime_ns)
		|| st.st_size != buf->b_orig_আকার
#ifdef HAVE_ST_MODE
		|| (পূর্ণসংখ্যা)st.st_mode != buf->b_orig_mode
#অন্যথা
		|| mch_getperm(buf->b_ffname) != buf->b_orig_mode
#endif
		))
    {
	long পূর্ব_b_mtime = buf->b_mtime;

	retval = 1;

	// set b_mtime to stop further warnings (e.g., when executing
	// FileChangedShell autocmd)
	যদি (stat_res < 0)
	{
	    // Check the file again later to see if it re-appears.
	    buf->b_mtime = -1;
	    buf->b_orig_আকার = 0;
	    buf->b_orig_mode = 0;
	}
	অন্যথা
	    buf_store_time(buf, &st, buf->b_ffname);

	// Don't do anything for a directory.  Might contain the file
	// explorer.
	যদি (mch_isdir(buf->b_fname))
	    ;

	/*
	 * If 'autoread' is set, the buffer has no changes and the file still
	 * exists, reload the buffer.  Use the buffer-local option value if it
	 * was set, the global option value otherwise.
	 */
	অন্যথা যদি ((buf->b_p_ar >= 0 ? buf->b_p_ar : p_ar)
				       && !bufIsChanged(buf) && stat_res >= 0)
	    reload = RELOAD_NORMAL;
	অন্যথা
	{
	    অক্ষর    *reason;
#ifdef FEAT_EVAL
	    size_t  reasonlen;
#endif

	    যদি (stat_res < 0)
	    {
		reason = "deleted";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("deleted");
#endif
	    }
	    অন্যথা যদি (bufIsChanged(buf))
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
	    অন্যথা যদি (orig_আকার != buf->b_orig_আকার || buf_contents_changed(buf))
	    {
		reason = "changed";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("changed");
#endif
	    }
	    অন্যথা যদি (orig_mode != buf->b_orig_mode)
	    {
		reason = "mode";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("mode");
#endif
	    }
	    অন্যথা
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
	    set_vim_var_string(VV_FCS_REASON, (char_u *)reason, (পূর্ণসংখ্যা)reasonlen);
	    set_vim_var_string(VV_FCS_CHOICE, (char_u *)"", 0);
#endif
	    ++allbuf_lock;
	    n = apply_autocmds(EVENT_FILECHANGEDSHELL,
				      buf->b_fname, buf->b_fname, FALSE, buf);
	    --allbuf_lock;
	    busy = FALSE;
	    যদি (n)
	    {
		যদি (!bufref_valid(&bufref))
		    emsg(_(e_filechangedshell_autocommand_deleted_বাফার));
#ifdef FEAT_EVAL
		char_u	*s = get_vim_var_str(VV_FCS_CHOICE);
		যদি (STRCMP(s, "reload") == 0 && *reason != 'd')
		    reload = RELOAD_NORMAL;
		অন্যথা যদি (STRCMP(s, "edit") == 0)
		    reload = RELOAD_DETECT;
		অন্যথা যদি (STRCMP(s, "ask") == 0)
		    n = FALSE;
		অন্যথা
#endif
		    ফেরত 2;
	    }
	    যদি (!n)
	    {
		যদি (*reason == 'd')
		{
		    // Only give the message once.
		    যদি (পূর্ব_b_mtime != -1)
			mesg = _(e_file_str_no_longer_available);
		}
		অন্যথা
		{
		    helpmesg = TRUE;
#যদি defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		    can_reload = TRUE;
#endif
		    যদি (reason[2] == 'n')
		    {
			mesg = _("W12: Warning: File \"%s\" has changed and the buffer was changed in Vim as well");
			mesg2 = _("See \":help W12\" for more info.");
		    }
		    অন্যথা যদি (reason[1] == 'h')
		    {
			mesg = _("W11: Warning: File \"%s\" has changed since editing started");
			mesg2 = _("See \":help W11\" for more info.");
		    }
		    অন্যথা যদি (*reason == 'm')
		    {
			mesg = _("W16: Warning: Mode of file \"%s\" has changed since editing started");
			mesg2 = _("See \":help W16\" for more info.");
		    }
		    অন্যথা
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
    অন্যথা যদি ((buf->b_flags & BF_NEW) && !(buf->b_flags & BF_NEW_W)
						&& vim_fexists(buf->b_ffname))
    {
	retval = 1;
	mesg = _("W13: Warning: File \"%s\" has been created after editing started");
	buf->b_flags |= BF_NEW_W;
#যদি defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	can_reload = TRUE;
#endif
    }

    যদি (mesg != ফাঁকা)
    {
	char_u *path;

	path = home_replace_save(buf, buf->b_fname);
	যদি (path != ফাঁকা)
	{
	    size_t  tbufsize;
	    অক্ষর    *tbuf;

	    যদি (!helpmesg)
		mesg2 = "";
	    tbufsize = STRLEN(mesg) + STRLEN(path) + 2 + STRLEN(mesg2) + 1; // +2 for either '\n' or "; "
									    // and +1 for NUL
	    tbuf = alloc(tbufsize);
	    যদি (tbuf != ফাঁকা)
	    {
		পূর্ণসংখ্যা tbuflen;

		tbuflen = vim_snprintf(tbuf, tbufsize, mesg, path);
#ifdef FEAT_EVAL
		// Set warningmsg here, before the unimportant and output-specific
		// mesg2 has been appended.
		set_vim_var_string(VV_WARNINGMSG, (char_u *)tbuf, tbuflen);
#endif
#যদি defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		যদি (can_reload)
		{
		    যদি (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "\n%s", mesg2);
		    নির্বাচন (do_dialog(VIM_WARNING, (char_u *)_("Warning"),
			    (char_u *)tbuf,
			    (char_u *)_("&OK\n&Load File\nLoad File &and Options"),
			    1, ফাঁকা, TRUE))
		    {
			ক্ষেত্র 2:
			    reload = RELOAD_NORMAL;
			    বিরতি;
			ক্ষেত্র 3:
			    reload = RELOAD_DETECT;
			    বিরতি;
		    }
		}
		অন্যথা
#endif
		যদি (State > MODE_NORMAL_BUSY || (State & MODE_CMDLINE)
								 || already_warned)
		{
		    যদি (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "; %s", mesg2);
		    emsg(tbuf);
		    retval = 2;
		}
		অন্যথা
		{
		    যদি (!autocmd_busy)
		    {
			বার্তা_start();
			বার্তা_puts_attr(tbuf, HL_ATTR(HLF_E) + বার্তা_HIST);
			যদি (*mesg2 != NUL)
			    বার্তা_puts_attr(mesg2, HL_ATTR(HLF_W) + বার্তা_HIST);
			বার্তা_clr_eos();
			(শূন্য)বার্তা_end();
			যদি (emsg_silent == 0 && !in_assert_fails)
			{
			    out_flush();
#ifdef FEAT_GUI
			    যদি (!focus)
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

    যদি (reload != RELOAD_NONE)
    {
	// Reload the buffer.
	buf_reload(buf, orig_mode, reload == RELOAD_DETECT);
#ifdef FEAT_PERSISTENT_UNDO
	যদি (bufref_valid(&bufref) && buf->b_p_udf && buf->b_ffname != ফাঁকা)
	{
	    char_u	    hash[UNDO_HASH_আকার];
	    buf_T	    *save_curbuf = curbuf;

	    // Any existing undo file is unusable, write it now.
	    curbuf = buf;
	    u_compute_hash(hash);
	    u_write_undo(ফাঁকা, FALSE, buf, hash);
	    curbuf = save_curbuf;
	}
#endif
    }

    // Trigger FileChangedShell when the file was changed in any way.
    যদি (bufref_valid(&bufref) && retval != 0)
	(শূন্য)apply_autocmds(EVENT_FILECHANGEDSHELLPOST,
				      buf->b_fname, buf->b_fname, FALSE, buf);
#ifdef FEAT_GUI
    // restore this in case an autocommand has set it; it would break
    // 'mousefocus'
    need_mouse_correct = save_mouse_correct;
#endif

    ফেরত retval;
}

/*
 * Reload a buffer that is already loaded.
 * Used when the file was changed outside of Vim.
 * "orig_mode" is buf->b_orig_mode before the need for reloading was detected.
 * buf->b_orig_mode may have been reset already.
 */
    শূন্য
buf_reload(buf_T *buf, পূর্ণসংখ্যা orig_mode, পূর্ণসংখ্যা reload_options)
{
    exarg_T	ea;
    pos_T	old_cursor;
    linenr_T	old_topline;
    পূর্ণসংখ্যা		old_ro = buf->b_p_ro;
    buf_T	*savebuf;
    bufref_T	bufref;
    পূর্ণসংখ্যা		saved = OK;
    aco_save_T	aco;
    পূর্ণসংখ্যা		flags = READ_NEW;
    পূর্ণসংখ্যা		prepped = OK;

    // Set curwin/curbuf for "buf" and save some things.
    aucmd_prepbuf(&aco, buf);
    যদি (curbuf != buf)
    {
	// Failed to find a window for "buf", it is dangerous to continue,
	// better bail out.
	ফেরত;
    }

    // Unless reload_options is set, we only want to read the text from the
    // file, not reset the syntax highlighting, clear marks, diff status, etc.
    // Force the fileformat and encoding to be the same.
    যদি (reload_options)
	CLEAR_FIELD(ea);
    অন্যথা
	prepped = prep_exarg(&ea, buf);

    যদি (prepped == OK)
    {
	old_cursor = curwin->w_cursor;
	old_topline = curwin->w_topline;

	যদি (p_ur < 0 || curbuf->b_ml.ml_সারি_গুনতি <= p_ur)
	{
	    // Save all the text, so that the reload can be undone.
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    saved = u_savecommon(0, curbuf->b_ml.ml_সারি_গুনতি + 1, 0, TRUE);
	    flags |= READ_KEEP_UNDO;
	}

	/*
	 * To behave like when a new file is edited (matters for
	 * BufReadPost autocommands) we first need to delete the current
	 * buffer contents.  But if reading the file fails we should keep
	 * the old contents.  Can't use memory only, the file might be
	 * too big.  Use a hidden buffer to move the buffer contents to.
	 */
	যদি (BUFEMPTY() || saved == FAIL)
	    savebuf = ফাঁকা;
	অন্যথা
	{
	    // Allocate a buffer without putting it in the buffer list.
	    savebuf = buflist_new(ফাঁকা, ফাঁকা, (linenr_T)1, BLN_DUMMY);
	    set_bufref(&bufref, savebuf);
	    যদি (savebuf != ফাঁকা && buf == curbuf)
	    {
		// Open the memline.
		curbuf = savebuf;
		curwin->w_বাফার = savebuf;
		saved = ml_open(curbuf);
		curbuf = buf;
		curwin->w_বাফার = buf;
	    }
	    যদি (savebuf == ফাঁকা || saved == FAIL || buf != curbuf
				      || move_সারিগুলি(buf, savebuf) == FAIL)
	    {
		semsg(_(e_could_not_prepare_for_reloading_str), buf->b_fname);
		saved = FAIL;
	    }
	}

	যদি (saved == OK)
	{
	    পূর্ণসংখ্যা old_বার্তা_silent = বার্তা_silent;

	    curbuf->b_flags |= BF_CHECK_RO;	// check for RO again
	    curbuf->b_keep_filetype = true;	// don't detect 'filetype'

	    যদি (shortmess(SHM_FILEINFO))
		বার্তা_silent = 1;

	    যদি (readfile(buf->b_ffname, buf->b_fname, (linenr_T)0,
			(linenr_T)0,
			(linenr_T)MAXLNUM, &ea, flags) != OK)
	    {
#যদি defined(FEAT_EVAL)
		যদি (!aborting())
#endif
		    semsg(_(e_could_not_reload_str), buf->b_fname);
		যদি (savebuf != ফাঁকা && bufref_valid(&bufref) && buf == curbuf)
		{
		    // Put the text back from the save buffer.  First
		    // delete any lines that readfile() added.
		    যখন (!BUFEMPTY())
			যদি (ml_delete(buf->b_ml.ml_সারি_গুনতি) == FAIL)
			    বিরতি;
		    (শূন্য)move_সারিগুলি(savebuf, buf);
		}
	    }
	    অন্যথা যদি (buf == curbuf)  // "buf" still valid
	    {
		// Mark the buffer as unmodified and free undo info.
		unchanged(buf, TRUE, TRUE);
		যদি ((flags & READ_KEEP_UNDO) == 0)
		    u_clearallandblockfree(buf);
		অন্যথা
		{
		    // Mark all undo states as changed.
		    u_unchanged(curbuf);
		}
	    }

	    বার্তা_silent = old_বার্তা_silent;
	}
	vim_free(ea.cmd);

	যদি (savebuf != ফাঁকা && bufref_valid(&bufref))
	    wipe_বাফার(savebuf, FALSE);

#ifdef FEAT_DIFF
	// Invalidate diff info if necessary.
	diff_invalidate(curbuf);
#endif

	// Restore the topline and cursor position and check it (lines may
	// have been removed).
	যদি (old_topline > curbuf->b_ml.ml_সারি_গুনতি)
	    curwin->w_topline = curbuf->b_ml.ml_সারি_গুনতি;
	অন্যথা
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
		যদি (wp->w_বাফার == curwin->w_বাফার
			&& !foldmethodIsManual(wp))
		    foldUpdateAll(wp);
	}
#endif
	// If the mode didn't change and 'readonly' was set, keep the old
	// value; the user probably used the ":view" command.  But don't
	// reset it, might have had a read error.
	যদি (orig_mode == curbuf->b_orig_mode)
	    curbuf->b_p_ro |= old_ro;

	// Modelines must override settings done by autocommands.
	do_modelines(0);
    }

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);
    // Careful: autocommands may have made "buf" invalid!
}

    শূন্য
buf_store_time(buf_T *buf, stat_T *st, char_u *fname UNUSED)
{
    buf->b_mtime = (long)st->st_mtime;
#ifdef ST_MTIM_NSEC
    buf->b_mtime_ns = (long)st->ST_MTIM_NSEC;
#অন্যথা
    buf->b_mtime_ns = 0;
#endif
    buf->b_orig_আকার = st->st_size;
#ifdef HAVE_ST_MODE
    buf->b_orig_mode = (পূর্ণসংখ্যা)st->st_mode;
#অন্যথা
    buf->b_orig_mode = mch_getperm(fname);
#endif
}

/*
 * Adjust the line with missing eol, used for the next write.
 * Used for do_filter(), when the input lines for the filter are deleted.
 */
    শূন্য
write_lnum_adjust(linenr_T offset)
{
    যদি (curbuf->b_no_eol_lnum != 0)	// only if there is a missing eol
	curbuf->b_no_eol_lnum += offset;
}

// Subfuncions for readdirex()
#ifdef FEAT_EVAL
# ifdef MSWIN
    স্থির char_u *
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
    যদি (wp != ফাঁকা)
    {
	যদি (_wcsicmp(wp, L".exe") == 0 ||
		_wcsicmp(wp, L".com") == 0 ||
		_wcsicmp(wp, L".cmd") == 0 ||
		_wcsicmp(wp, L".bat") == 0)
	    st_mode |= _S_IEXEC;
    }

    // Copy user bits to group/other.
    st_mode |= (st_mode & 0700) >> 3;
    st_mode |= (st_mode & 0700) >> 6;

    st.st_mode = st_mode;
    ফেরত getfpermst(&st, perm);
}

    স্থির শূন্য
getftypewfd(WIN32_FIND_DATAW *wfd, string_T *ret)
{
    DWORD flag = wfd->dwFileAttributes;
    DWORD tag = wfd->dwReserved0;

    যদি (flag & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	যদি (tag == IO_REPARSE_TAG_MOUNT_POINT)
	{
	    ret->string = (char_u *)"junction";
	    ret->দৈর্ঘ্য = STRLEN_LITERAL("junction");
	    ফেরত;
	}
	যদি (tag == IO_REPARSE_TAG_SYMLINK)
	{
	    যদি (flag & FILE_ATTRIBUTE_DIRECTORY)
	    {
		ret->string = (char_u *)"linkd";
		ret->দৈর্ঘ্য = STRLEN_LITERAL("linkd");
		ফেরত;
	    }

	    ret->string = (char_u *)"link";
	    ret->দৈর্ঘ্য = STRLEN_LITERAL("link");
	    ফেরত;
	}

	// unknown reparse point type
	ret->string = (char_u *)"reparse";
	ret->দৈর্ঘ্য = STRLEN_LITERAL("reparse");
	ফেরত;
    }

    যদি (flag & FILE_ATTRIBUTE_DIRECTORY)
    {
	ret->string = (char_u *)"dir";
	ret->দৈর্ঘ্য = STRLEN_LITERAL("dir");
	ফেরত;
    }

    ret->string = (char_u *)"file";
    ret->দৈর্ঘ্য = STRLEN_LITERAL("file");
}

    স্থির dict_T *
create_readdirex_item(WIN32_FIND_DATAW *wfd)
{
    dict_T	*item;
    char_u	*p;
    varnumber_T	আকার_2, time;
    char_u	permbuf[] = "---------";
    string_T	s;

    item = dict_alloc();
    যদি (item == ফাঁকা)
	ফেরত ফাঁকা;
    item->dv_refcount++;

    p = utf16_to_enc(wfd->cFileName, ফাঁকা);
    যদি (p == ফাঁকা)
	যাও theend;
    যদি (dict_add_string(item, "name", p) == FAIL)
    {
	vim_free(p);
	যাও theend;
    }
    vim_free(p);

    আকার_2 = (((varnumber_T)wfd->n_File_আকার_উপরের) << 32) | wfd->n_File_আকার_নিচের;
    যদি (dict_add_সংখ্যা(item, "size", আকার_2) == FAIL)
	যাও theend;

    // Convert FILETIME to unix time.
    time = (((((varnumber_T)wfd->ftLastWriteTime.dw_উপরের_Date_Time) << 32) |
		wfd->ftLastWriteTime.dw_নিচের_Date_Time)
	    - 116444736000000000) / 10000000;
    যদি (dict_add_সংখ্যা(item, "time", time) == FAIL)
	যাও theend;

    getftypewfd(wfd, &s);
    যদি (dict_add_string_len(item, "type", s.string, (পূর্ণসংখ্যা)s.দৈর্ঘ্য) == FAIL)
	যাও theend;
    যদি (dict_add_string(item, "perm", getfpermwfd(wfd, permbuf)) == FAIL)
	যাও theend;

    যদি (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	যাও theend;
    যদি (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	যাও theend;

    ফেরত item;

theend:
    dict_unref(item);
    ফেরত ফাঁকা;
}
# অন্যথা
    স্থির dict_T *
create_readdirex_item(char_u *path, char_u *name)
{
    dict_T	*item;
    অক্ষর	*p;
    size_t	pathlen, len;
    size_t	namelen;
    stat_T	st;
    পূর্ণসংখ্যা		ret, link = FALSE;
    varnumber_T	আকার_2;
    char_u	permbuf[] = "---------";
    string_T	q = {ফাঁকা, 0};
    কাঠামো passwd *pw;
    কাঠামো group  *gr;

    item = dict_alloc();
    যদি (item == ফাঁকা)
	ফেরত ফাঁকা;
    item->dv_refcount++;

    pathlen = STRLEN(path);
    namelen = STRLEN(name);
    len = pathlen + 1 + namelen + 1;
    p = alloc(len);
    যদি (p == ফাঁকা)
	যাও theend;
    যদি (pathlen > 0 && path[pathlen - 1] == '/')
	vim_snprintf(p, len, "%s%s", path, name);
    অন্যথা
	vim_snprintf(p, len, "%s/%s", path, name);
    ret = mch_lstat(p, &st);
    যদি (ret >= 0 && S_ISLNK(st.st_mode))
    {
	link = TRUE;
	ret = mch_stat(p, &st);
	যদি (ret < 0)
	    STR_LITERAL_SET(q, "link");
    }
    vim_free(p);

    যদি (dict_add_string_len(item, "name", name, (পূর্ণসংখ্যা)namelen) == FAIL)
	যাও theend;

    যদি (ret >= 0)
    {
	আকার_2 = (varnumber_T)st.st_size;
	যদি (S_ISDIR(st.st_mode))
	    আকার_2 = 0;
	// non-perfect check for overflow
	অন্যথা যদি ((off_T)আকার_2 != (off_T)st.st_size)
	    আকার_2 = -2;
	যদি (dict_add_সংখ্যা(item, "size", আকার_2) == FAIL)
	    যাও theend;
	যদি (dict_add_সংখ্যা(item, "time", (varnumber_T)st.st_mtime) == FAIL)
	    যাও theend;

	যদি (link)
	{
	    যদি (S_ISDIR(st.st_mode))
		STR_LITERAL_SET(q, "linkd");
	    অন্যথা
		STR_LITERAL_SET(q, "link");
	}
	অন্যথা
	{
	    q.string = getftypest(&st);
	    q.দৈর্ঘ্য = STRLEN(q.string);
	}
	যদি (dict_add_string_len(item, "type", q.string, (পূর্ণসংখ্যা)q.দৈর্ঘ্য) == FAIL)
	    যাও theend;
	যদি (dict_add_string(item, "perm", getfpermst(&st, permbuf)) == FAIL)
	    যাও theend;

	pw = getpwuid(st.st_uid);
	যদি (pw == ফাঁকা)
	    STR_LITERAL_SET(q, "");
	অন্যথা
	{
	    q.string = (char_u *)pw->pw_name;
	    q.দৈর্ঘ্য = STRLEN(q.string);
	}
	যদি (dict_add_string_len(item, "user", q.string, (পূর্ণসংখ্যা)q.দৈর্ঘ্য) == FAIL)
	    যাও theend;
#  যদি !defined(VMS) || (defined(VMS) && defined(HAVE_XOS_R_H))
	gr = getgrgid(st.st_gid);
	যদি (gr == ফাঁকা)
	    STR_LITERAL_SET(q, "");
	অন্যথা
	{
	    q.string = (char_u *)gr->gr_name;
	    q.দৈর্ঘ্য = STRLEN(q.string);
	}
#  endif
	যদি (dict_add_string_len(item, "group", q.string, (পূর্ণসংখ্যা)q.দৈর্ঘ্য) == FAIL)
	    যাও theend;
    }
    অন্যথা
    {
	যদি (dict_add_সংখ্যা(item, "size", -1) == FAIL)
	    যাও theend;
	যদি (dict_add_সংখ্যা(item, "time", -1) == FAIL)
	    যাও theend;
	যদি (q.string == ফাঁকা)
	{
	    যদি (dict_add_string_len(item, "type", (char_u *)"", 0) == FAIL)
		যাও theend;
	}
	অন্যথা
	{
	    যদি (dict_add_string_len(item, "type", q.string, (পূর্ণসংখ্যা)q.দৈর্ঘ্য) == FAIL)
		যাও theend;
	}
	যদি (dict_add_string_len(item, "perm", (char_u *)"", 0) == FAIL)
	    যাও theend;
	যদি (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	    যাও theend;
	যদি (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	    যাও theend;
    }
    ফেরত item;

theend:
    dict_unref(item);
    ফেরত ফাঁকা;
}
# endif

    স্থির পূর্ণসংখ্যা
compare_readdirex_item(ধ্রুবক শূন্য *p1, ধ্রুবক শূন্য *p2)
{
    char_u  *name1, *name2;

    name1 = dict_get_string(*(dict_T**)p1, "name", FALSE);
    name2 = dict_get_string(*(dict_T**)p2, "name", FALSE);
    যদি (readdirex_sort == READDIR_SORT_BYTE)
	ফেরত STRCMP(name1, name2);
    যদি (readdirex_sort == READDIR_SORT_IC)
	ফেরত STRICMP(name1, name2);

    ফেরত STRCOLL(name1, name2);
}

    স্থির পূর্ণসংখ্যা
compare_readdir_item(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2)
{
    যদি (readdirex_sort == READDIR_SORT_BYTE)
	ফেরত STRCMP(*(অক্ষর **)s1, *(অক্ষর **)s2);
    যদি (readdirex_sort == READDIR_SORT_IC)
	ফেরত STRICMP(*(অক্ষর **)s1, *(অক্ষর **)s2);

    ফেরত STRCOLL(*(অক্ষর **)s1, *(অক্ষর **)s2);
}
#endif

#যদি defined(TEMPDIRNAMES) || defined(FEAT_EVAL)
/*
 * Core part of "readdir()" and "readdirex()" function.
 * Retrieve the list of files/directories of "path" into "gap".
 * If "withattr" is TRUE, retrieve the names and their attributes.
 * If "withattr" is FALSE, retrieve the names only.
 * Return OK for success, FAIL for failure.
 */
    পূর্ণসংখ্যা
readdir_core(
    garray_T	*gap,
    char_u	*path,
    পূর্ণসংখ্যা		withattr UNUSED,
    শূন্য	*context,
    পূর্ণসংখ্যা		(*checkitem)(শূন্য *context, শূন্য *item),
    পূর্ণসংখ্যা		sort)
{
    পূর্ণসংখ্যা			failed = FALSE;
    char_u		*p;
# ifdef MSWIN
    char_u		*buf;
    পূর্ণসংখ্যা			ok;
    HANDLE		hFind = INVALID_HANDLE_মান;
    WIN32_FIND_DATAW    wfd;
    WCHAR		*wn = ফাঁকা;	// UTF-16 name, NULL when not used.
    char_u		*p_end;
# অন্যথা
    DIR			*dirp;
    কাঠামো dirent	*dp;
# endif

    ga_init2(gap, আকার(শূন্য *), 20);

# ifdef FEAT_EVAL
#  define FREE_ITEM(item)   করো { \
	যদি (withattr) \
	    dict_unref((dict_T*)(item)); \
	অন্যথা \
	    vim_free(item); \
    } যখন (0)

    readdirex_sort = READDIR_SORT_BYTE;
# অন্যথা
#  define FREE_ITEM(item)   vim_free(item)
# endif

# ifdef MSWIN
    buf = alloc(MAXPATHL);
    যদি (buf == ফাঁকা)
	ফেরত FAIL;
    STRNCPY(buf, path, MAXPATHL-5);
    p = p_end = buf + STRLEN(buf);
    MB_PTR_BACK(buf, p);
    যদি (*p == '\\' || *p == '/')
	p_end = p;
    STRCPY(p_end, "\\*");

    wn = enc_to_utf16(buf, ফাঁকা);
    যদি (wn != ফাঁকা)
	hFind = FindFirstFileW(wn, &wfd);
    ok = (hFind != INVALID_HANDLE_মান);
    যদি (!ok)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    অন্যথা
    {
	যখন (ok)
	{
	    পূর্ণসংখ্যা	    ignore;
	    শূন্য    *item;
	    WCHAR   *wp;

	    wp = wfd.cFileName;
	    ignore = wp[0] == L'.' &&
		    (wp[1] == NUL ||
		     (wp[1] == L'.' && wp[2] == NUL));
	    যদি (ignore)
	    {
		ok = Find_পরবর্তী_File_W(hFind, &wfd);
		চালু;
	    }
#  ifdef FEAT_EVAL
	    যদি (withattr)
		item = (শূন্য*)create_readdirex_item(&wfd);
	    অন্যথা
#  endif
		item = (শূন্য*)utf16_to_enc(wfd.cFileName, ফাঁকা);
	    যদি (item == ফাঁকা)
	    {
		failed = TRUE;
		বিরতি;
	    }

	    যদি (!ignore && checkitem != ফাঁকা)
	    {
		পূর্ণসংখ্যা r = checkitem(context, item);

		যদি (r < 0)
		{
		    FREE_ITEM(item);
		    বিরতি;
		}
		যদি (r == 0)
		    ignore = TRUE;
	    }

	    যদি (!ignore)
	    {
		যদি (ga_grow(gap, 1) == OK)
		    ((শূন্য**)gap->ga_তথ্য)[gap->ga_len++] = item;
		অন্যথা
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    বিরতি;
		}
	    }
	    অন্যথা
		FREE_ITEM(item);

	    ok = Find_পরবর্তী_File_W(hFind, &wfd);
	}
	FindClose(hFind);
    }

    vim_free(buf);
    vim_free(wn);
# অন্যথা	// MSWIN
    dirp = opendir((অক্ষর *)path);
    যদি (dirp == ফাঁকা)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    অন্যথা
    {
	জন্য (;;)
	{
	    পূর্ণসংখ্যা	    ignore;
	    শূন্য    *item;

	    dp = readdir(dirp);
	    যদি (dp == ফাঁকা)
		বিরতি;
	    p = (char_u *)dp->d_name;

	    ignore = p[0] == '.' &&
		    (p[1] == NUL ||
		     (p[1] == '.' && p[2] == NUL));
	    যদি (ignore)
		চালু;
#  ifdef FEAT_EVAL
	    যদি (withattr)
		item = (শূন্য*)create_readdirex_item(path, p);
	    অন্যথা
#  endif
		item = (শূন্য*)vim_strsave(p);
	    যদি (item == ফাঁকা)
	    {
		failed = TRUE;
		বিরতি;
	    }

	    যদি (checkitem != ফাঁকা)
	    {
		পূর্ণসংখ্যা r = checkitem(context, item);

		যদি (r < 0)
		{
		    FREE_ITEM(item);
		    বিরতি;
		}
		যদি (r == 0)
		    ignore = TRUE;
	    }

	    যদি (!ignore)
	    {
		যদি (ga_grow(gap, 1) == OK)
		    ((শূন্য**)gap->ga_তথ্য)[gap->ga_len++] = item;
		অন্যথা
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    বিরতি;
		}
	    }
	    অন্যথা
		FREE_ITEM(item);
	}

	closedir(dirp);
    }
# endif	// MSWIN

# undef FREE_ITEM

    যদি (!failed && gap->ga_len > 0 && sort > READDIR_SORT_NONE)
    {
# ifdef FEAT_EVAL
	readdirex_sort = sort;
	যদি (withattr)
	    qsort((শূন্য*)gap->ga_তথ্য, (size_t)gap->ga_len, আকার(dict_T*),
		    compare_readdirex_item);
	অন্যথা
	    qsort((শূন্য*)gap->ga_তথ্য, (size_t)gap->ga_len, আকার(char_u *),
		    compare_readdir_item);
# অন্যথা
	    sort_strings((char_u **)gap->ga_তথ্য, gap->ga_len);
# endif
    }

    ফেরত failed ? FAIL : OK;
}

/*
 * Delete "name" and everything in it, recursively.
 * return 0 for success, -1 if some file was not deleted.
 */
    পূর্ণসংখ্যা
delete_recursive(char_u *name)
{
    পূর্ণসংখ্যা ফলাফল = 0;

    // A symbolic link to a directory itself is deleted, not the directory it
    // points to.
    যদি (
# যদি defined(UNIX) || defined(MSWIN)
	 mch_isrealdir(name)
# অন্যথা
	 mch_isdir(name)
# endif
	    )
    {
	char_u	    *exp = vim_strsave(name);
	garray_T    ga;

	যদি (exp == ফাঁকা)
	    ফেরত -1;
	যদি (readdir_core(&ga, exp, FALSE, ফাঁকা, ফাঁকা, READDIR_SORT_NONE) == OK)
	{
	    পূর্ণসংখ্যা	len = vim_snprintf((অক্ষর *)NameBuff, MAXPATHL, "%s/", exp);
	    পূর্ণসংখ্যা	i;

	    জন্য (i = 0; i < ga.ga_len; ++i)
	    {
		vim_snprintf((অক্ষর *)NameBuff + len, MAXPATHL - len, "%s", ((char_u **)ga.ga_তথ্য)[i]);
		যদি (delete_recursive(NameBuff) != 0)
		    // Remember the failure but continue deleting any further
		    // entries.
		    ফলাফল = -1;
	    }
	    ga_clear_strings(&ga);
	    যদি (mch_rmdir(exp) != 0)
		ফলাফল = -1;
	}
	অন্যথা
	    ফলাফল = -1;
	vim_free(exp);
    }
    অন্যথা
	ফলাফল = mch_remove(name) == 0 ? 0 : -1;

    ফেরত ফলাফল;
}
#endif

#যদি defined(TEMPDIRNAMES)
স্থির long	অস্থায়ী_গুনতি = 0;		// Temp filename counter.

# যদি defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
/*
 * Open temporary directory and take file lock to prevent
 * to be auto-cleaned.
 */
   স্থির শূন্য
vim_opentempdir(শূন্য)
{
    DIR *dp = ফাঁকা;

    যদি (vim_tempdir_dp != ফাঁকা)
	ফেরত;

    dp = opendir((ধ্রুবক অক্ষর*)vim_tempdir);
    যদি (dp == ফাঁকা)
	ফেরত;

    vim_tempdir_dp = dp;
    flock(dirfd(vim_tempdir_dp), LOCK_SH);
}

/*
 * Close temporary directory - it automatically release file lock.
 */
   স্থির শূন্য
vim_closetempdir(শূন্য)
{
    যদি (vim_tempdir_dp == ফাঁকা)
	ফেরত;

    closedir(vim_tempdir_dp);
    vim_tempdir_dp = ফাঁকা;
}

/*
 * Return true if the temp directory we created is gone.
 */
    স্থির bool
vim_tempdir_gone(শূন্য)
{
    stat_T	st;

    যদি (vim_tempdir_dp == ফাঁকা)
	ফেরত false;
    ফেরত fstat(dirfd(vim_tempdir_dp), &st) < 0 || st.st_nlink == 0;
}
# endif

/*
 * Delete the temp directory and all files it contains.
 */
    শূন্য
vim_deltempdir(শূন্য)
{
    যদি (vim_tempdir == ফাঁকা)
	ফেরত;

# যদি defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
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
    স্থির শূন্য
vim_settempdir(char_u *tempdir)
{
    char_u	*buf;
    size_t	buflen;

    buf = alloc(MAXPATHL + 2);
    যদি (buf == ফাঁকা)
	ফেরত;

    যদি (vim_FullName(tempdir, buf, MAXPATHL, FALSE) == FAIL)
	STRCPY(buf, tempdir);
    buflen = STRLEN(buf);
    যদি (!after_pathsep(buf, buf + buflen))
    {
	STRCPY(buf + buflen, PATHSEPSTR);
	buflen += আকার(PATHSEP);
    }
    vim_tempdir = vim_strnsave(buf, buflen);
# যদি defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
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
    পূর্ণসংখ্যা	    extra_char UNUSED,  // char to use in the name instead of '?'
    পূর্ণসংখ্যা	    keep UNUSED)
{
#ifdef USE_TMPNAM
    char_u	itmp[L_tmpnam];	// use tmpnam()
#নতুবা_যদি defined(MSWIN)
    WCHAR	itmp[TEMPNAMELEN];
#অন্যথা
    char_u	itmp[TEMPNAMELEN];
#endif

#ifdef TEMPDIRNAMES
    স্থির অক্ষর	*(tempdirs[]) = {TEMPDIRNAMES};
    পূর্ণসংখ্যা		i;
# ifndef EEXIST
    stat_T	st;
# endif

# যদি defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    // if the temp directory is gone, force re-creation of it
    যদি (vim_tempdir != ফাঁকা && vim_tempdir_gone())
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
    যদি (vim_tempdir == ফাঁকা)
    {
	/*
	 * Try the entries in TEMPDIRNAMES to create the temp directory.
	 */
	জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(tempdirs); ++i)
	{
# ifndef HAVE_MKDTEMP
	    long	nr;
	    long	off;
# endif
	    size_t	itmplen;

	    // Expand $TMP, leave room for "/v1100000/999999999".
	    // Skip the directory check if the expansion fails.
	    itmplen = expand_env((char_u *)tempdirs[i], itmp, TEMPNAMELEN - 20);
	    যদি (itmp[0] != '$' && mch_isdir(itmp))
	    {
		// directory exists
		যদি (!after_pathsep(itmp, itmp + itmplen))
		{
		    STRCPY(itmp + itmplen, PATHSEPSTR);
		    itmplen += আকার(PATHSEP);
		}

# ifdef HAVE_MKDTEMP
		{
#  যদি defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    mode_t	umask_save = umask(077);
#  endif
		    // Leave room for filename
		    STRCPY(itmp + itmplen, "vXXXXXX");
		    itmplen += STRLEN_LITERAL("vXXXXXX");
		    যদি (mkdtemp((অক্ষর *)itmp) != ফাঁকা)
			vim_settempdir(itmp);
#  যদি defined(UNIX) || defined(VMS)
		    (শূন্য)umask(umask_save);
#  endif
		}
# অন্যথা
		// Get an arbitrary number of up to 6 digits.  When it's
		// unlikely that it already exists it will be faster,
		// otherwise it doesn't matter.  The use of mkdir() avoids any
		// security problems because of the predictable number.
		nr = (mch_get_pid() + (long)time(ফাঁকা)) % 1000000L;

		// Try up to 10000 different values until we find a name that
		// doesn't exist.
		জন্য (off = 0; off < 10000L; ++off)
		{
		    পূর্ণসংখ্যা		r;
#  যদি defined(UNIX) || defined(VMS)
		    mode_t	umask_save;
#  endif

		    vim_snprintf((অক্ষর *)itmp + itmplen, আকার(itmp) - itmplen, "v%ld", nr + off);
#  ifndef EEXIST
		    // If mkdir() does not set errno to EEXIST, check for
		    // existing file here.  There is a race condition then,
		    // although it's fail-safe.
		    যদি (mch_stat((অক্ষর *)itmp, &st) >= 0)
			চালু;
#  endif
#  যদি defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    umask_save = umask(077);
#  endif
		    r = vim_mkdir(itmp, 0700);
#  যদি defined(UNIX) || defined(VMS)
		    (শূন্য)umask(umask_save);
#  endif
		    যদি (r == 0)
		    {
			vim_settempdir(itmp);
			বিরতি;
		    }
#  ifdef EEXIST
		    // If the mkdir() didn't fail because the file/dir exists,
		    // we probably can't create any dir here, try another
		    // place.
		    যদি (errno != EEXIST)
#  endif
			বিরতি;
		}
# endif // HAVE_MKDTEMP
		যদি (vim_tempdir != ফাঁকা)
		    বিরতি;
	    }
	}
    }

    যদি (vim_tempdir != ফাঁকা)
    {
	// There is no need to check if the file exists, because we own the
	// directory and nobody else creates a file in it.
	পূর্ণসংখ্যা itmplen = vim_snprintf((অক্ষর *)itmp, আকার(itmp), "%s%ld", vim_tempdir, অস্থায়ী_গুনতি++);
	ফেরত vim_strnsave(itmp, (size_t)itmplen);
    }

    ফেরত ফাঁকা;

#অন্যথা // TEMPDIRNAMES
    char_u	*p;

# ifdef MSWIN
    WCHAR	wsz_অস্থায়ী_File[_MAX_PATH + 1];
    WCHAR	buf4[4];
    WCHAR	*chartab = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char_u	*retval;
    char_u	*shname;
    long	i;

    wcscpy(itmp, L"");
    যদি (Get_অস্থায়ী_Path_W(_MAX_PATH, wsz_অস্থায়ী_File) == 0)
    {
	wsz_অস্থায়ী_File[0] = L'.';	// GetTempPathW() failed, use current dir
	wsz_অস্থায়ী_File[1] = L'\\';
	wsz_অস্থায়ী_File[2] = NUL;
    }
    wcscpy(buf4, L"VIM");

    // randomize the name to avoid collisions
    i = mch_get_pid() + extra_char;
    buf4[1] = chartab[i % 36];
    buf4[2] = chartab[(i / 36) % 36];
    যদি (Get_অস্থায়ী_File_Name_W(wsz_অস্থায়ী_File, buf4, 0, itmp) == 0)
	ফেরত ফাঁকা;
    যদি (!keep)
	// GetTempFileNameW() will create the file, we don't want that
	(শূন্য)DeleteFileW(itmp);

    // Backslashes in a temp file name cause problems when filtering with
    // "sh".  NOTE: This also checks 'shellcmdflag' to help those people who
    // didn't set 'shellslash' but only if not using PowerShell.
    retval = utf16_to_enc(itmp, ফাঁকা);
    যদি (retval == ফাঁকা)
	ফেরত ফাঁকা;
    shname = gettail(p_sh);
    যদি ((*p_shcf == '-' && !(strstr((অক্ষর *)shname, "powershell") != ফাঁকা
			     || strstr((অক্ষর *)shname, "pwsh") != ফাঁকা ))
								    || p_ssl)
	জন্য (p = retval; *p; ++p)
	    যদি (*p == '\\')
		*p = '/';
    ফেরত retval;

# অন্যথা // MSWIN
    পূর্ণসংখ্যা		itmplen;

#  ifdef USE_TMPNAM
    // tmpnam() will make its own name
    p = tmpnam((অক্ষর *)itmp);
    যদি (p == ফাঁকা || *p == NUL)
	ফেরত ফাঁকা;
#  অন্যথা

#   ifdef VMS_TEMPNAM
    // mktemp() is not working on VMS.  It seems to be
    // a do-nothing function. Therefore we use tempnam().
    vim_snprintf((অক্ষর *)itmp, আকার(itmp), "VIM%c", extra_char);
    p = (char_u *)tempnam("tmp:", (অক্ষর *)itmp);
    যদি (p != ফাঁকা)
    {
	// VMS will use '.LIS' if we don't explicitly specify an extension,
	// and VIM will then be unable to find the file later
	itmplen = vim_snprintf((অক্ষর *)itmp, আকার(itmp), "%s.txt", p);
	মুক্ত(p);
    }
    অন্যথা
	ফেরত ফাঁকা;
#   অন্যথা
    STRCPY(itmp, TEMPNAME);
    itmplen = STRLEN_LITERAL(TEMPNAME);
    যদি ((p = vim_strchr(itmp, '?')) != ফাঁকা)
	*p = extra_char;
    যদি (mktemp((অক্ষর *)itmp) == ফাঁকা)
	ফেরত ফাঁকা;
#   endif
#  endif

    ফেরত vim_strnsave(itmp, (size_t)itmplen);
# endif // MSWIN
#endif // TEMPDIRNAMES
}

#যদি defined(BACKSLASH_IN_FILENAME)
/*
 * Convert all backslashes in fname to forward slashes in-place, unless when
 * it looks like a URL.
 */
    শূন্য
forward_slash(char_u *fname)
{
    char_u	*p;

    যদি (path_with_url(fname))
	ফেরত;
    জন্য (p = fname; *p != NUL; ++p)
	// The Big5 encoding can have '\' in the trail byte.
	যদি (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
	    ++p;
	অন্যথা যদি (*p == '\\')
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
    পূর্ণসংখ্যা
match_file_pat(
    char_u	*pattern,		// pattern to match with
    regprog_T	**prog,			// pre-compiled regprog or NULL
    char_u	*fname,			// full path of file name
    char_u	*sfname,		// short file name or NULL
    char_u	*লেজ,			// tail of path
    পূর্ণসংখ্যা		allow_dirs)		// allow matching with dir
{
    regmatch_T	regmatch;
    পূর্ণসংখ্যা		ফলাফল = FALSE;

    regmatch.rm_ic = p_fic; // ignore case if 'fileignorecase' is set
    যদি (prog != ফাঁকা)
	regmatch.regprog = *prog;
    অন্যথা
	regmatch.regprog = vim_regcomp(pattern, RE_MAGIC);

    /*
     * Try for a match with the pattern with:
     * 1. the full file name, when the pattern has a '/'.
     * 2. the short file name, when the pattern has a '/'.
     * 3. the tail of the file name, when the pattern has no '/'.
     */
    যদি (regmatch.regprog != ফাঁকা
	     && ((allow_dirs
		     && (vim_regexec(&regmatch, fname, (colnr_T)0)
			 || (sfname != ফাঁকা
			     && vim_regexec(&regmatch, sfname, (colnr_T)0))))
		 || (!allow_dirs && vim_regexec(&regmatch, লেজ, (colnr_T)0))))
	ফলাফল = TRUE;

    যদি (prog != ফাঁকা)
	*prog = regmatch.regprog;
    অন্যথা
	vim_regfree(regmatch.regprog);
    ফেরত ফলাফল;
}

/*
 * Return TRUE if a file matches with a pattern in "list".
 * "list" is a comma-separated list of patterns, like 'wildignore'.
 * "sfname" is the short file name or NULL, "ffname" the long file name.
 */
    পূর্ণসংখ্যা
match_file_list(char_u *list, char_u *sfname, char_u *ffname)
{
    char_u	buf[MAXPATHL];
    char_u	*লেজ;
    char_u	*regpat;
    অক্ষর	allow_dirs;
    পূর্ণসংখ্যা		match;
    char_u	*p;

    লেজ = gettail(sfname);

    // try all patterns in 'wildignore'
    p = list;
    যখন (*p)
    {
	copy_option_part(&p, buf, MAXPATHL, ",");
	regpat = file_pat_to_reg_pat(buf, ফাঁকা, &allow_dirs, FALSE);
	যদি (regpat == ফাঁকা)
	    বিরতি;
	match = match_file_pat(regpat, ফাঁকা, ffname, sfname,
						       লেজ, (পূর্ণসংখ্যা)allow_dirs);
	vim_free(regpat);
	যদি (match)
	    ফেরত TRUE;
    }
    ফেরত FALSE;
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
    অক্ষর	*allow_dirs,	// Result passed back out in here
    পূর্ণসংখ্যা		no_bslash UNUSED) // Don't use a backward slash as pathsep
{
    পূর্ণসংখ্যা		আকার_2 = 2; // '^' at start, '$' at end
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		nested = 0;
    পূর্ণসংখ্যা		add_dollar = TRUE;

    যদি (allow_dirs != ফাঁকা)
	*allow_dirs = FALSE;
    যদি (pat_end == ফাঁকা)
	pat_end = pat + STRLEN(pat);

    জন্য (p = pat; p < pat_end; p++)
    {
	নির্বাচন (*p)
	{
	    ক্ষেত্র '*':
	    ক্ষেত্র '.':
	    ক্ষেত্র ',':
	    ক্ষেত্র '{':
	    ক্ষেত্র '}':
	    ক্ষেত্র '~':
		আকার_2 += 2;	// extra backslash
		বিরতি;
#ifdef BACKSLASH_IN_FILENAME
	    ক্ষেত্র '\\':
	    ক্ষেত্র '/':
		আকার_2 += 4;	// could become "[\/]"
		বিরতি;
#endif
	    পূর্বনির্ধারিত:
		আকার_2++;
		যদি (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		{
		    ++p;
		    ++আকার_2;
		}
		বিরতি;
	}
    }
    reg_pat = alloc(আকার_2 + 1);
    যদি (reg_pat == ফাঁকা)
	ফেরত ফাঁকা;

    i = 0;

    যদি (pat[0] == '*')
	যখন (pat[0] == '*' && pat < pat_end - 1)
	    pat++;
    অন্যথা
	reg_pat[i++] = '^';
    endp = pat_end - 1;
    যদি (endp >= pat && *endp == '*')
    {
	যখন (endp - pat > 0 && *endp == '*')
	    endp--;
	add_dollar = FALSE;
    }
    জন্য (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	নির্বাচন (*p)
	{
	    ক্ষেত্র '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		যখন (p[1] == '*')	// "**" matches like "*"
		    ++p;
		বিরতি;
	    ক্ষেত্র '.':
	    ক্ষেত্র '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		বিরতি;
	    ক্ষেত্র '?':
		reg_pat[i++] = '.';
		বিরতি;
	    ক্ষেত্র '\\':
		যদি (p[1] == NUL)
		    বিরতি;
#ifdef BACKSLASH_IN_FILENAME
		যদি (!no_bslash)
		{
		    // translate:
		    // "\x" to "\\x"  e.g., "dir\file"
		    // "\*" to "\\.*" e.g., "dir\*.c"
		    // "\?" to "\\."  e.g., "dir\??.c"
		    // "\+" to "\+"   e.g., "fileX\+.c"
		    যদি ((vim_isfilec(p[1]) || p[1] == '*' || p[1] == '?')
			    && p[1] != '+')
		    {
			reg_pat[i++] = '[';
			reg_pat[i++] = '\\';
			reg_pat[i++] = '/';
			reg_pat[i++] = ']';
			যদি (allow_dirs != ফাঁকা)
			    *allow_dirs = TRUE;
			বিরতি;
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
		যদি (*++p == '?'
#ifdef BACKSLASH_IN_FILENAME
			&& no_bslash
#endif
			)
		    reg_pat[i++] = '?';
		অন্যথা
		    যদি (*p == ',' || *p == '%' || *p == '#'
			       || vim_isspace(*p) || *p == '{' || *p == '}')
			reg_pat[i++] = *p;
		    অন্যথা যদি (*p == '\\' && p[1] == '\\' && p[2] == '{')
		    {
			reg_pat[i++] = '\\';
			reg_pat[i++] = '{';
			p += 2;
		    }
		    অন্যথা
		    {
			যদি (allow_dirs != ফাঁকা && vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
				&& (!no_bslash || *p != '\\')
#endif
				)
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			যদি (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
			    reg_pat[i++] = *p++;
			reg_pat[i++] = *p;
		    }
		বিরতি;
#ifdef BACKSLASH_IN_FILENAME
	    ক্ষেত্র '/':
		reg_pat[i++] = '[';
		reg_pat[i++] = '\\';
		reg_pat[i++] = '/';
		reg_pat[i++] = ']';
		যদি (allow_dirs != ফাঁকা)
		    *allow_dirs = TRUE;
		বিরতি;
#endif
	    ক্ষেত্র '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		বিরতি;
	    ক্ষেত্র '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		বিরতি;
	    ক্ষেত্র ',':
		যদি (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		অন্যথা
		    reg_pat[i++] = ',';
		বিরতি;
	    পূর্বনির্ধারিত:
		যদি (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		    reg_pat[i++] = *p++;
		অন্যথা যদি (allow_dirs != ফাঁকা && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		বিরতি;
	}
    }
    যদি (add_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    যদি (nested != 0)
    {
	যদি (nested < 0)
	    emsg(_(e_missing_open_curly));
	অন্যথা
	    emsg(_(e_missing_close_curly));
	VIM_CLEAR(reg_pat);
    }
    ফেরত reg_pat;
}

#যদি defined(EINTR)
/*
 * Version of read() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    long
read_eintr(পূর্ণসংখ্যা fd, শূন্য *buf, size_t bufsize)
{
    long ret;

    জন্য (;;)
    {
	ret = vim_read(fd, buf, bufsize);
	যদি (ret >= 0 || errno != EINTR)
	    বিরতি;
    }
    ফেরত ret;
}

/*
 * Version of write() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    long
write_eintr(পূর্ণসংখ্যা fd, শূন্য *buf, size_t bufsize)
{
    long    ret = 0;
    long    wlen;

    // Repeat the write() so long it didn't fail, other than being interrupted
    // by a signal.
    যখন (ret < (long)bufsize)
    {
	wlen = vim_write(fd, (অক্ষর *)buf + ret, bufsize - ret);
	যদি (wlen < 0)
	{
	    যদি (errno != EINTR)
		বিরতি;
	}
	অন্যথা
	    ret += wlen;
    }
    ফেরত ret;
}
#endif
