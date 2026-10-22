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

#include "vim.h"

#གལ་སྲིད defined(__TANDEM)
# include <limits.h>		// for SSIZE_MAX
#endif
#གལ་སྲིད (defined(UNIX) || defined(VMS)) && defined(FEAT_EVAL)
# include <pwd.h>
# include <grp.h>
#endif
#གལ་སྲིད defined(VMS) && defined(HAVE_XOS_R_H)
# include <x11/xos_r.h>
#endif

// Is there any system that doesn't have access()?
#define USE_MCH_ACCESS

// Bitmask with 0x80 set in each byte of a long_u word, used to detect
// non-ASCII bytes (high bit set) in multiple bytes at once.
#define NONASCII_MASK (((long_u)-1 / 0xFF) * 0x80)

#གལ་སྲིད defined(__hpux) && !defined(HAVE_DIRFD)
# define dirfd(x) ((x)->__dd_fd)
# define HAVE_DIRFD
#endif

static char_u *next_fenc(char_u **pp, ཧྲིལ *alloced);
#ifdef FEAT_EVAL
static char_u *readfile_charconvert(char_u *fname, char_u *fenc, ཧྲིལ *fdp);
#endif
#ifdef FEAT_CRYPT
static char_u *check_for_cryptkey(char_u *cryptkey, char_u *ptr, long *sizep, off_T *filesizep, ཧྲིལ newfile, char_u *fname, ཧྲིལ *did_ask);
#endif
static linenr_T readfile_linenr(linenr_T linecnt, char_u *p, char_u *endp);
static char_u *check_for_bom(char_u *p, long size, ཧྲིལ *lenp, ཧྲིལ flags);

#ifdef FEAT_EVAL
static ཧྲིལ readdirex_sort;
#endif

    void
filemess(
    buf_T	*buf,
    char_u	*name,
    char_u	*s,
    ཧྲིལ		attr)
{
    ཧྲིལ		msg_scroll_save;
    ཧྲིལ		prev_msg_col = msg_col;
    size_t	len;

    གལ་སྲིད (msg_silent != 0)
	ལོག;
    msg_add_fname(buf, name);	    // put file name in IObuff with quotes

    // If it's extremely long, truncate it.
    len = STRLEN(IObuff);
    གལ་སྲིད (len > IOSIZE - 100)
    {
	len = IOSIZE - 100;
	IObuff[len] = NUL;
    }

    // Avoid an over-long translation to cause trouble.
    གལ་སྲིད (*s != NUL)
	STRNCPY(IObuff + len, s, 99);

    /*
     * For the first message may have to start a new line.
     * For further ones overwrite the previous one, reset msg_scroll before
     * calling filemess().
     */
    msg_scroll_save = msg_scroll;
    གལ་སྲིད (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	msg_scroll = FALSE;
    གལ་སྲིད (!msg_scroll)	// wait a bit when overwriting an error msg
	check_for_delay(FALSE);
    msg_start();
    གལ་སྲིད (prev_msg_col != 0 && msg_col == 0)
	msg_putchar('\r');  // overwrite any previous message.
    msg_scroll = msg_scroll_save;
    msg_scrolled_ign = TRUE;
    // may truncate the message to avoid a hit-return prompt
    msg_outtrans_attr(msg_may_trunc(FALSE, IObuff), attr);
    msg_clr_eos();
    out_flush();
    msg_scrolled_ign = FALSE;
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
    ཧྲིལ
readfile(
    char_u	*fname,
    char_u	*sfname,
    linenr_T	from,
    linenr_T	lines_to_skip,
    linenr_T	lines_to_read,
    exarg_T	*eap,			// can be NULL!
    ཧྲིལ		flags)
{
    ཧྲིལ		retval = FAIL;	// jump to "theend" instead of returning
    ཧྲིལ		fd = 0;
    ཧྲིལ		newfile = (flags & READ_NEW);
    ཧྲིལ		check_readonly;
    ཧྲིལ		filtering = (flags & READ_FILTER);
    ཧྲིལ		read_stdin = (flags & READ_STDIN);
    ཧྲིལ		read_buffer = (flags & READ_BUFFER);
    ཧྲིལ		read_fifo = (flags & READ_FIFO);
    ཧྲིལ		set_options = newfile || read_buffer
					   || (eap != NULL && eap->read_edit);
    linenr_T	read_buf_lnum = 1;	// next line to read from curbuf
    colnr_T	read_buf_col = 0;	// next char to read from this line
    char_u	c;
    linenr_T	lnum = from;
    char_u	*ptr = NULL;		// pointer into read buffer
    char_u	*buffer = NULL;		// read buffer
    char_u	*new_buffer = NULL;	// init to shut up gcc
    char_u	*line_start = NULL;	// init to shut up gcc
    ཧྲིལ		wasempty;		// buffer was empty before reading
    colnr_T	len;
    long	size = 0;
    char_u	*p;
    off_T	filesize = 0;
    ཧྲིལ		skip_read = FALSE;
#ifdef FEAT_CRYPT
    off_T       filesize_disk = 0;      // file size read from disk
    off_T       filesize_count = 0;     // counter
    char_u	*cryptkey = NULL;
    ཧྲིལ		did_ask_for_key = FALSE;
#endif
#ifdef FEAT_PERSISTENT_UNDO
    context_sha256_T sha_ctx;
    ཧྲིལ		read_undo_file = FALSE;
#endif
    ཧྲིལ		split = 0;		// number of split lines
#define UNKNOWN	 0x0fffffff		// file size is unknown
    linenr_T	linecnt;
    ཧྲིལ		error = FALSE;		// errors encountered
    ཧྲིལ		ff_error = EOL_UNKNOWN; // file format with errors
    long	linerest = 0;		// remaining chars in line
#ifdef UNIX
    ཧྲིལ		perm = 0;
    ཧྲིལ		swap_mode = -1;		// protection bits for swap file
#དེ་མིན
    ཧྲིལ		perm;
#endif
    ཧྲིལ		fileformat = 0;		// end-of-line format
    ཧྲིལ		keep_fileformat = FALSE;
    stat_T	st;
    ཧྲིལ		file_readonly;
    linenr_T	skip_count = 0;
    linenr_T	read_count = 0;
    ཧྲིལ		msg_save = msg_scroll;
    linenr_T	read_no_eol_lnum = 0;   // non-zero lnum when last line of
					// last read was missing the eol
    ཧྲིལ		try_mac;
    ཧྲིལ		try_dos;
    ཧྲིལ		try_unix;
    ཧྲིལ		file_rewind = FALSE;
    ཧྲིལ		can_retry;
    linenr_T	conv_error = 0;		// line nr with conversion error
    linenr_T	illegal_byte = 0;	// line nr with illegal byte
    ཧྲིལ		keep_dest_enc = FALSE;	// don't retry when char doesn't fit
					// in destination encoding
    ཧྲིལ		bad_char_behavior = BAD_REPLACE;
					// BAD_KEEP, BAD_DROP or character to
					// replace with
    char_u	*tmpname = NULL;	// name of 'charconvert' output file
    ཧྲིལ		fio_flags = 0;
    char_u	*fenc;			// fileencoding to use
    ཧྲིལ		fenc_alloced;		// fenc_next is in allocated memory
    char_u	*fenc_next = NULL;	// next item in 'fencs' or NULL
    ཧྲིལ		advance_fenc = FALSE;
    long	real_size = 0;
#ifdef USE_ICONV
    iconv_t	iconv_fd = (iconv_t)-1;	// descriptor for iconv() or -1
# ifdef FEAT_EVAL
    ཧྲིལ		did_iconv = FALSE;	// TRUE when iconv() failed and trying
					// 'charconvert' next
# endif
#endif
    ཧྲིལ		converted = FALSE;	// TRUE if conversion done
    ཧྲིལ		notconverted = FALSE;	// TRUE if conversion wanted but it
					// wasn't possible
    char_u	conv_rest[CONV_RESTLEN];
    ཧྲིལ		conv_restlen = 0;	// nr of bytes in conv_rest[]
    pos_T	orig_start;
    buf_T	*old_curbuf;
    char_u	*old_b_ffname;
    char_u	*old_b_fname;
    ཧྲིལ		using_b_ffname;
    ཧྲིལ		using_b_fname;
    static var_na *msg_is_a_directory = N_("is a directory");
#ifdef FEAT_CRYPT
    ཧྲིལ		eof = FALSE;
#endif
#ifdef FEAT_SODIUM
    ཧྲིལ		may_need_lseek = FALSE;
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
    གལ་སྲིད (curbuf->b_ffname == NULL
	    && !filtering
	    && fname != NULL
	    && vim_strchr(p_cpo, CPO_FNAMER) != NULL
	    && !(flags & READ_DUMMY))
    {
	གལ་སྲིད (set_rw_fname(fname, sfname) == FAIL)
	    goto theend;
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
    གལ་སྲིད (sfname == NULL)
	sfname = fname;
#གལ་སྲིད defined(UNIX)
    fname = sfname;
#endif

    /*
     * The BufReadCmd and FileReadCmd events intercept the reading process by
     * executing the associated commands instead.
     */
    གལ་སྲིད (!filtering && !read_stdin && !read_buffer)
    {
	orig_start = curbuf->b_op_start;

	// Set '[ mark to the line above where the lines go (line 1 if zero).
	curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
	curbuf->b_op_start.col = 0;

	གལ་སྲིད (newfile)
	{
	    གལ་སྲིད (apply_autocmds_exarg(EVENT_BUFREADCMD, NULL, sfname,
							  FALSE, curbuf, eap))
	    {
		retval = OK;
#ifdef FEAT_EVAL
		གལ་སྲིད (aborting())
		    retval = FAIL;
#endif
		// The BufReadCmd code usually uses ":read" to get the text and
		// perhaps ":file" to change the buffer name. But we should
		// consider this to work like ":edit", thus reset the
		// BF_NOTEDITED flag.  Then ":write" will work to overwrite the
		// same file.
		གལ་སྲིད (retval == OK)
		    curbuf->b_flags &= ~BF_NOTEDITED;
		goto theend;
	    }
	}
	དེ་མིན གལ་སྲིད (apply_autocmds_exarg(EVENT_FILEREADCMD, sfname, sfname,
							    FALSE, NULL, eap))
	{
#ifdef FEAT_EVAL
	    retval = aborting() ? FAIL : OK;
#དེ་མིན
	    retval = OK;
#endif
	    goto theend;
	}

	curbuf->b_op_start = orig_start;

	གལ་སྲིད (flags & READ_NOFILE)
	{
	    // Return NOTDONE instead of FAIL so that BufEnter can be triggered
	    // and other operations don't fail.
	    retval = NOTDONE;
	    goto theend;
	}
    }

    གལ་སྲིད ((shortmess(SHM_OVER) || curbuf->b_help) && p_verbose == 0)
	msg_scroll = FALSE;	// overwrite previous file message
    དེ་མིན
	msg_scroll = TRUE;	// don't overwrite previous file message

    གལ་སྲིད (fname != NULL && *fname != NUL)
    {
	fnamelen = STRLEN(fname);

	// If the name is too long we might crash further on, quit here.
	གལ་སྲིད (fnamelen >= MAXPATHL)
	{
	    filemess(curbuf, fname, (char_u *)_("Illegal file name"), 0);
	    msg_end();
	    msg_scroll = msg_save;
	    goto theend;
	}

	// If the name ends in a path separator, we can't open it.  Check here,
	// because reading the file may actually work, but then creating the
	// swap file may destroy it!  Reported on MS-DOS and Win 95.
	གལ་སྲིད (after_pathsep(fname, fname + fnamelen))
	{
	    filemess(curbuf, fname, (char_u *)_(msg_is_a_directory), 0);
	    msg_end();
	    msg_scroll = msg_save;
	    retval = NOTDONE;
	    goto theend;
	}
    }
#གལ་སྲིད defined(UNIX) || defined(VMS)
    གལ་སྲིད (!read_stdin && fname != NULL)
	perm = mch_getperm(fname);
#endif

    གལ་སྲིད (!read_stdin && !read_buffer && !read_fifo)
    {
#གལ་སྲིད defined(UNIX) || defined(VMS)
	གལ་སྲིད (perm >= 0 && !S_ISREG(perm)		    // not a regular file ...
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
	    གལ་སྲིད (S_ISDIR(perm))
	    {
		filemess(curbuf, fname, (char_u *)_(msg_is_a_directory), 0);
		retval = NOTDONE;
	    }
	    དེ་མིན
		filemess(curbuf, fname, (char_u *)_("is not a file"), 0);
	    msg_end();
	    msg_scroll = msg_save;
	    goto theend;
	}
#endif
#གལ་སྲིད defined(MSWIN)
	/*
	 * MS-Windows allows opening a device, but we will probably get stuck
	 * trying to read it.
	 */
	གལ་སྲིད (!p_odev && mch_nodetype(fname) == NODE_WRITABLE)
	{
	    filemess(curbuf, fname, (char_u *)_("is a device (disabled with 'opendevice' option)"), 0);
	    msg_end();
	    msg_scroll = msg_save;
	    goto theend;
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
    གལ་སྲིད (check_readonly && !readonlymode)
	curbuf->b_p_ro = FALSE;

    གལ་སྲིད (newfile && !read_stdin && !read_buffer && !read_fifo)
    {
	// Remember time of file.
	གལ་སྲིད (mch_stat((var_na *)fname, &st) >= 0)
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
	དེ་མིན
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
    གལ་སྲིད (read_stdin)
    {
#གལ་སྲིད defined(MSWIN)
	// Force binary I/O on stdin to avoid CR-LF -> LF conversion.
	setmode(0, O_BINARY);
#endif
    }
    དེ་མིན གལ་སྲིད (!read_buffer)
    {
#ifdef USE_MCH_ACCESS
	གལ་སྲིད (
# ifdef UNIX
	    !(perm & 0222) ||
# endif
				mch_access((var_na *)fname, W_OK))
	    file_readonly = TRUE;
	fd = mch_open((var_na *)fname, O_RDONLY | O_EXTRA, 0);
#དེ་མིན
	གལ་སྲིད (!newfile
		|| readonlymode
		|| (fd = mch_open((var_na *)fname, O_RDWR | O_EXTRA, 0)) < 0)
	{
	    file_readonly = TRUE;
	    // try to open ro
	    fd = mch_open((var_na *)fname, O_RDONLY | O_EXTRA, 0);
	}
#endif
    }

    གལ་སྲིད (fd < 0)			    // cannot open at all
    {
#ifndef UNIX
	ཧྲིལ	isdir_f;
#endif
	msg_scroll = msg_save;
#ifndef UNIX
	/*
	 * On Amiga we can't open a directory, check here.
	 */
	isdir_f = (mch_isdir(fname));
	perm = mch_getperm(fname);  // check if the file exists
	གལ་སྲིད (isdir_f)
	{
	    filemess(curbuf, sfname, (char_u *)_(msg_is_a_directory), 0);
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	}
	དེ་མིན
#endif
	    གལ་སྲིད (newfile)
	    {
		གལ་སྲིད (perm < 0
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
		    གལ་སྲིད (!bt_dontwrite(curbuf))
		    {
			check_need_swap(newfile);
			// SwapExists autocommand may mess things up
			གལ་སྲིད (curbuf != old_curbuf
				|| (using_b_ffname
					&& (old_b_ffname != curbuf->b_ffname))
				|| (using_b_fname
					 && (old_b_fname != curbuf->b_fname)))
			{
			    emsg(_(e_autocommands_changed_buffer_or_buffer_name));
			    goto theend;
			}
		    }
		    གལ་སྲིད (dir_of_file_exists(fname))
			filemess(curbuf, sfname,
					      (char_u *)new_file_message(), 0);
		    དེ་མིན
			filemess(curbuf, sfname,
					   (char_u *)_("[New DIRECTORY]"), 0);
#ifdef FEAT_VIMINFO
		    // Even though this is a new file, it might have been
		    // edited before and deleted.  Get the old marks.
		    check_marks_read();
#endif
		    // Set forced 'fileencoding'.
		    གལ་སྲིད (eap != NULL)
			set_forced_fenc(eap);
		    apply_autocmds_exarg(EVENT_BUFNEWFILE, sfname, sfname,
							  FALSE, curbuf, eap);
		    // remember the current fileformat
		    save_file_ff(curbuf);

#གལ་སྲིད defined(FEAT_EVAL)
		    གལ་སྲིད (!aborting())   // autocmds may abort script processing
#endif
			retval = OK;	    // a new file is not an error
		    goto theend;
		}
		དེ་མིན
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

	goto theend;
    }

    /*
     * Only set the 'ro' flag for readonly files the first time they are
     * loaded.	Help files always get readonly mode
     */
    གལ་སྲིད ((check_readonly && file_readonly) || curbuf->b_help)
	curbuf->b_p_ro = TRUE;

    གལ་སྲིད (set_options)
    {
	// Don't change 'eol' if reading from buffer as it will already be
	// correctly set when reading stdin.
	གལ་སྲིད (!read_buffer)
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
    གལ་སྲིད (!bt_dontwrite(curbuf))
    {
	check_need_swap(newfile);
	གལ་སྲིད (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))))
	{
	    emsg(_(e_autocommands_changed_buffer_or_buffer_name));
	    གལ་སྲིད (!read_buffer)
		close(fd);
	    goto theend;
	}
#ifdef UNIX
	// Set swap file protection bits after creating it.
	གལ་སྲིད (swap_mode > 0 && curbuf->b_ml.ml_mfp != NULL
			  && curbuf->b_ml.ml_mfp->mf_fname != NULL)
	{
	    char_u *swap_fname = curbuf->b_ml.ml_mfp->mf_fname;

	    /*
	     * If the group-read bit is set but not the world-read bit, then
	     * the group must be equal to the group of the original file.  If
	     * we can't make that happen then reset the group-read bit.  This
	     * avoids making the swap file readable to more users when the
	     * primary group of the user is too permissive.
	     */
	    གལ་སྲིད ((swap_mode & 044) == 040)
	    {
		stat_T	swap_st;

		གལ་སྲིད (mch_stat((var_na *)swap_fname, &swap_st) >= 0
			&& st.st_gid != swap_st.st_gid
# ifdef HAVE_FCHOWN
			&& fchown(curbuf->b_ml.ml_mfp->mf_fd, -1, st.st_gid)
									  == -1
# endif
		   )
		    swap_mode &= 0600;
	    }

	    (void)mch_setperm(swap_fname, (long)swap_mode);
	}
#endif
    }

    // If "Quit" selected at ATTENTION dialog, don't load the file
    གལ་སྲིད (swap_exists_action == SEA_QUIT)
    {
	གལ་སྲིད (!read_buffer && !read_stdin)
	    close(fd);
	goto theend;
    }

    ++no_wait_return;	    // don't wait for return yet

    /*
     * Set '[ mark to the line above where the lines go (line 1 if zero).
     */
    orig_start = curbuf->b_op_start;
    curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
    curbuf->b_op_start.col = 0;

    try_mac = (vim_strchr(p_ffs, 'm') != NULL);
    try_dos = (vim_strchr(p_ffs, 'd') != NULL);
    try_unix = (vim_strchr(p_ffs, 'x') != NULL);

    གལ་སྲིད (!read_buffer)
    {
	ཧྲིལ	m = msg_scroll;
	ཧྲིལ	n = msg_scrolled;

	/*
	 * The file must be closed again, the autocommands may want to change
	 * the file before reading it.
	 */
	གལ་སྲིད (!read_stdin)
	    close(fd);		// ignore errors

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	msg_scroll = TRUE;
	གལ་སྲིད (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPRE, NULL, sfname,
							  FALSE, curbuf, eap);
	དེ་མིན གལ་སྲིད (read_stdin)
	    apply_autocmds_exarg(EVENT_STDINREADPRE, NULL, sfname,
							  FALSE, curbuf, eap);
	དེ་མིན གལ་སྲིད (newfile)
	    apply_autocmds_exarg(EVENT_BUFREADPRE, NULL, sfname,
							  FALSE, curbuf, eap);
	དེ་མིན
	    apply_autocmds_exarg(EVENT_FILEREADPRE, sfname, sfname,
							    FALSE, NULL, eap);
	// autocommands may have changed it
	try_mac = (vim_strchr(p_ffs, 'm') != NULL);
	try_dos = (vim_strchr(p_ffs, 'd') != NULL);
	try_unix = (vim_strchr(p_ffs, 'x') != NULL);
	curbuf->b_op_start = orig_start;

	གལ་སྲིད (msg_scrolled == n)
	    msg_scroll = m;

#ifdef FEAT_EVAL
	གལ་སྲིད (aborting())	    // autocmds may abort script processing
	{
	    --no_wait_return;
	    msg_scroll = msg_save;
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    goto theend;
	}
#endif
	/*
	 * Don't allow the autocommands to change the current buffer.
	 * Try to re-open the file.
	 *
	 * Don't allow the autocommands to change the buffer name either
	 * (cd for example) if it invalidates fname or sfname.
	 */
	གལ་སྲིད (!read_stdin && (curbuf != old_curbuf
		|| (using_b_ffname && (old_b_ffname != curbuf->b_ffname))
		|| (using_b_fname && (old_b_fname != curbuf->b_fname))
		|| (fd = mch_open((var_na *)fname, O_RDONLY | O_EXTRA, 0)) < 0))
	{
	    --no_wait_return;
	    msg_scroll = msg_save;
	    གལ་སྲིད (fd < 0)
		emsg(_(e_readpre_autocommands_made_file_unreadable));
	    དེ་མིན
		emsg(_(e_readpre_autocommands_must_not_change_current_buffer));
	    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    goto theend;
	}
    }

    // Autocommands may add lines to the file, need to check if it is empty
    wasempty = (curbuf->b_ml.ml_flags & ML_EMPTY);

    གལ་སྲིད (!recoverymode && !filtering && !(flags & READ_DUMMY))
    {
	/*
	 * Show the user that we are busy reading the input.  Sometimes this
	 * may take a while.  When reading from stdin another program may
	 * still be running, don't move the cursor to the last line, unless
	 * always using the GUI.
	 */
	གལ་སྲིད (read_stdin)
	{
	    གལ་སྲིད (!is_not_a_term())
	    {
#ifndef ALWAYS_USE_GUI
# ifdef VIMDLL
		གལ་སྲིད (!gui.in_use)
# endif
		    mch_msg(_("Vim: Reading from stdin...\n"));
#endif
#ifdef FEAT_GUI
		// Also write a message in the GUI window, if there is one.
		གལ་སྲིད (gui.in_use && !gui.dying && !gui.starting)
		{
		    size_t  plen = STRLEN(_("Reading from stdin..."));

		    // make a copy, gui_write() may try to change it
		    p = vim_strnsave((char_u *)_("Reading from stdin..."), plen);
		    གལ་སྲིད (p != NULL)
		    {
			gui_write(p, (ཧྲིལ)plen);
			vim_free(p);
		    }
		}
#endif
	    }
	}
	དེ་མིན གལ་སྲིད (!read_buffer)
	    filemess(curbuf, sfname, (char_u *)"", 0);
    }

    msg_scroll = FALSE;			// overwrite the file message

    /*
     * Set linecnt now, before the "retry" caused by a wrong guess for
     * fileformat, and after the autocommands, which may change them.
     */
    linecnt = curbuf->b_ml.ml_line_count;

    // "++bad=" argument.
    གལ་སྲིད (eap != NULL && eap->bad_char != 0)
    {
	bad_char_behavior = eap->bad_char;
	གལ་སྲིད (set_options)
	    curbuf->b_bad_char = eap->bad_char;
    }
    དེ་མིན
	curbuf->b_bad_char = 0;

    /*
     * Decide which 'encoding' to use or use first.
     */
    གལ་སྲིད (eap != NULL && eap->force_enc != 0)
    {
	fenc = enc_canonize(eap->cmd + eap->force_enc);
	fenc_alloced = TRUE;
	keep_dest_enc = TRUE;
    }
    དེ་མིན གལ་སྲིད (curbuf->b_p_bin)
    {
	fenc = (char_u *)"";		// binary: don't convert
	fenc_alloced = FALSE;
    }
    དེ་མིན གལ་སྲིད (curbuf->b_help)
    {
	char_u	    firstline[80];
	ཧྲིལ	    fc;

	// Help files are either utf-8 or latin1.  Try utf-8 first, if this
	// fails it must be latin1.
	// Always do this when 'encoding' is "utf-8".  Otherwise only do
	// this when needed to avoid [converted] remarks all the time.
	// It is needed when the first line contains non-ASCII characters.
	// That is only in *.??x files.
	fenc = (char_u *)"latin1";
	c = enc_utf8;
	གལ་སྲིད (!c && !read_stdin)
	{
	    fc = fname[fnamelen - 1];
	    གལ་སྲིད (TOLOWER_ASC(fc) == 'x')
	    {
		// Read the first line (and a bit more).  Immediately rewind to
		// the start of the file.  If the read() fails "len" is -1.
		len = read_eintr(fd, firstline, 80);
		vim_lseek(fd, (off_T)0L, SEEK_SET);
		དོན་ལུ (p = firstline; p < firstline + len; ++p)
		    གལ་སྲིད (*p >= 0x80)
		    {
			c = TRUE;
			bha_nga;
		    }
	    }
	}

	གལ་སྲིད (c)
	{
	    fenc_next = fenc;
	    fenc = (char_u *)"utf-8";

	    // When the file is utf-8 but a character doesn't fit in
	    // 'encoding' don't retry.  In help text editing utf-8 bytes
	    // doesn't make sense.
	    གལ་སྲིད (!enc_utf8)
		keep_dest_enc = TRUE;
	}
	fenc_alloced = FALSE;
    }
    དེ་མིན གལ་སྲིད (*p_fencs == NUL)
    {
	fenc = curbuf->b_p_fenc;	// use format from buffer
	fenc_alloced = FALSE;
    }
    དེ་མིན
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

    གལ་སྲིད (file_rewind)
    {
	གལ་སྲིད (read_buffer)
	{
	    read_buf_lnum = 1;
	    read_buf_col = 0;
	}
	དེ་མིན གལ་སྲིད (read_stdin || vim_lseek(fd, (off_T)0L, SEEK_SET) != 0)
	{
	    // Can't rewind the file, give up.
	    error = TRUE;
	    goto failed;
	}
	// Delete the previously read lines.
	སྐབས (lnum > from)
	    ml_delete(lnum--);
	file_rewind = FALSE;
	གལ་སྲིད (set_options)
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
    གལ་སྲིད (keep_fileformat)
	keep_fileformat = FALSE;
    དེ་མིན
    {
	གལ་སྲིད (eap != NULL && eap->force_ff != 0)
	{
	    fileformat = get_fileformat_force(curbuf, eap);
	    try_unix = try_dos = try_mac = FALSE;
	}
	དེ་མིན གལ་སྲིད (curbuf->b_p_bin)
	    fileformat = EOL_UNIX;		// binary: use Unix format
	དེ་མིན གལ་སྲིད (*p_ffs == NUL)
	    fileformat = get_fileformat(curbuf);// use format from buffer
	དེ་མིན
	    fileformat = EOL_UNKNOWN;		// detect from file
    }

#ifdef USE_ICONV
    གལ་སྲིད (iconv_fd != (iconv_t)-1)
    {
	// aborted conversion with iconv(), close the descriptor
	iconv_close(iconv_fd);
	iconv_fd = (iconv_t)-1;
    }
#endif

    གལ་སྲིད (advance_fenc)
    {
	/*
	 * Try the next entry in 'fileencodings'.
	 */
	advance_fenc = FALSE;

	གལ་སྲིད (eap != NULL && eap->force_enc != 0)
	{
	    // Conversion given with "++cc=" wasn't possible, read
	    // without conversion.
	    notconverted = TRUE;
	    conv_error = 0;
	    གལ་སྲིད (fenc_alloced)
		vim_free(fenc);
	    fenc = (char_u *)"";
	    fenc_alloced = FALSE;
	}
	དེ་མིན
	{
	    གལ་སྲིད (fenc_alloced)
		vim_free(fenc);
	    གལ་སྲིད (fenc_next != NULL)
	    {
		fenc = next_fenc(&fenc_next, &fenc_alloced);
	    }
	    དེ་མིན
	    {
		fenc = (char_u *)"";
		fenc_alloced = FALSE;
	    }
	}
	གལ་སྲིད (tmpname != NULL)
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
    གལ་སྲིད (converted)
    {
	// "ucs-bom" means we need to check the first bytes of the file
	// for a BOM.
	གལ་སྲིད (STRCMP(fenc, ENC_UCSBOM) == 0)
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
	དེ་མིན གལ་སྲིད (enc_utf8 || STRCMP(p_enc, "latin1") == 0)
	    fio_flags = get_fio_flags(fenc);

#ifdef MSWIN
	/*
	 * Conversion from an MS-Windows codepage to UTF-8 or another codepage
	 * is handled with MultiByteToWideChar().
	 */
	གལ་སྲིད (fio_flags == 0)
	    fio_flags = get_win_fio_flags(fenc);
#endif

#ifdef MACOS_CONVERT
	// Conversion from Apple MacRoman to latin1 or UTF-8
	གལ་སྲིད (fio_flags == 0)
	    fio_flags = get_mac_fio_flags(fenc);
#endif

#ifdef USE_ICONV
	/*
	 * Try using iconv() if we can't convert internally.
	 */
	གལ་སྲིད (fio_flags == 0
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
	གལ་སྲིད (fio_flags == 0 && !read_stdin && !read_buffer && *p_ccv != NUL
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
	    གལ་སྲིད (tmpname == NULL)
	    {
		tmpname = readfile_charconvert(fname, fenc, &fd);
		གལ་སྲིད (tmpname == NULL)
		{
		    // Conversion failed.  Try another one.
		    advance_fenc = TRUE;
		    གལ་སྲིད (fd < 0)
		    {
			// Re-opening the original file failed!
			emsg(_(e_conversion_mad_file_unreadable));
			error = TRUE;
			goto failed;
		    }
		    goto retry;
		}
	    }
	}
	དེ་མིན
#endif
	{
	    གལ་སྲིད (fio_flags == 0
#ifdef USE_ICONV
		    && iconv_fd == (iconv_t)-1
#endif
	       )
	    {
		// Conversion wanted but we can't.
		// Try the next conversion in 'fileencodings'
		advance_fenc = TRUE;
		goto retry;
	    }
	}
    }

    // Set "can_retry" when it's possible to rewind the file and try with
    // another "fenc" value.  It's FALSE when no other "fenc" to try, reading
    // stdin or fixed at a specific encoding.
    can_retry = (*fenc != NUL && !read_stdin && !read_fifo && !keep_dest_enc);

    གལ་སྲིད (!skip_read)
    {
	linerest = 0;
	filesize = 0;
#ifdef FEAT_CRYPT
	filesize_count = 0;
#endif
	skip_count = lines_to_skip;
	read_count = lines_to_read;
	conv_restlen = 0;
#ifdef FEAT_PERSISTENT_UNDO
	read_undo_file = (newfile && (flags & READ_KEEP_UNDO) == 0
				  && curbuf->b_ffname != NULL
				  && curbuf->b_p_udf
				  && !filtering
				  && !read_fifo
				  && !read_stdin
				  && !read_buffer);
	གལ་སྲིད (read_undo_file)
	    sha256_start(&sha_ctx);
#endif
#ifdef FEAT_CRYPT
	གལ་སྲིད (curbuf->b_cryptstate != NULL)
	{
	    // Need to free the state, but keep the key, don't want to ask for
	    // it again.
	    crypt_free_state(curbuf->b_cryptstate);
	    curbuf->b_cryptstate = NULL;
	}
#endif
    }

    སྐབས (!error && !got_int)
    {
	/*
	 * We allocate as much space for the file as we can get, plus
	 * space for the old line plus room for one terminating NUL.
	 * The amount is limited by the fact that read() only can read
	 * up to max_unsigned characters (and other things).
	 */
	གལ་སྲིད (!skip_read)
	{
#གལ་སྲིད defined(SSIZE_MAX) && (SSIZE_MAX < 0x10000L)
		size = SSIZE_MAX;		    // use max I/O size, 52K
#དེ་མིན
		// Use buffer >= 64K.  Add linerest to double the size if the
		// line gets very long, to avoid a lot of copying. But don't
		// read more than 1 Mbyte at a time, so we can be interrupted.
		size = 0x10000L + linerest;
		གལ་སྲིད (size > 0x100000L)
		    size = 0x100000L;
#endif
	}

	// Protect against the argument of lalloc() going negative.
	// Also split lines that are too long for colnr_T.  After this check
	// passes, we read up to 'size' more bytes.  We must ensure that even
	// after that read, the line length won't exceed MAXCOL - 1 (because
	// we add 1 for the NUL when casting to colnr_T).  If this check fires,
	// we insert a synthetic newline immediately, so linerest doesn't grow.
	གལ་སྲིད (size < 0 || size + linerest + 1 < 0 || linerest >= MAXCOL - size)
	{
	    ++split;
	    *ptr = NL;		    // split line by inserting a NL
	    size = 1;
	}
	དེ་མིན
	{
	    གལ་སྲིད (!skip_read)
	    {
		དོན་ལུ ( ; size >= 10; size = (long)((long_u)size >> 1))
		{
		    གལ་སྲིད ((new_buffer = lalloc(size + linerest + 1,
							      FALSE)) != NULL)
			bha_nga;
		}
		གལ་སྲིད (new_buffer == NULL)
		{
		    do_outofmem_msg((long_u)(size * 2 + linerest + 1));
		    error = TRUE;
		    bha_nga;
		}
		གལ་སྲིད (linerest)	// copy characters from the previous buffer
		    mch_memmove(new_buffer, ptr - linerest, (size_t)linerest);
		vim_free(buffer);
		buffer = new_buffer;
		ptr = buffer + linerest;
		line_start = buffer;

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
		real_size = (ཧྲིལ)size;
#ifdef USE_ICONV
		གལ་སྲིད (iconv_fd != (iconv_t)-1)
		    size = size / ICONV_MULT;
		དེ་མིན
#endif
		    གལ་སྲིད (fio_flags & FIO_LATIN1)
		    size = size / 2;
		དེ་མིན གལ་སྲིད (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    size = (size * 2 / 3) & ~1;
		དེ་མིན གལ་སྲིད (fio_flags & FIO_UCS4)
		    size = (size * 2 / 3) & ~3;
		དེ་མིན གལ་སྲིད (fio_flags == FIO_UCSBOM)
		    size = size / ICONV_MULT;	// worst case
#ifdef MSWIN
		དེ་མིན གལ་སྲིད (fio_flags & FIO_CODEPAGE)
		    size = size / ICONV_MULT;	// also worst case
#endif
#ifdef MACOS_CONVERT
		དེ་མིན གལ་སྲིད (fio_flags & FIO_MACROMAN)
		    size = size / ICONV_MULT;	// also worst case
#endif

		གལ་སྲིད (conv_restlen > 0)
		{
		    // Insert unconverted bytes from previous line.
		    mch_memmove(ptr, conv_rest, conv_restlen);
		    ptr += conv_restlen;
		    size -= conv_restlen;
		}

		གལ་སྲིད (read_buffer)
		{
		    /*
		     * Read bytes from curbuf.  Used for converting text read
		     * from stdin.
		     */
		    གལ་སྲིད (read_buf_lnum > from)
			size = 0;
		    དེ་མིན
		    {
			ཧྲིལ	n, ni;
			long	tlen;

			tlen = 0;
			དོན་ལུ (;;)
			{
			    p = ml_get(read_buf_lnum) + read_buf_col;
			    n = ml_get_len(read_buf_lnum) - read_buf_col;
			    གལ་སྲིད ((ཧྲིལ)tlen + n + 1 > size)
			    {
				// Filled up to "size", append partial line.
				// Change NL to NUL to reverse the effect done
				// below.
				n = (ཧྲིལ)(size - tlen);
				དོན་ལུ (ni = 0; ni < n; ++ni)
				{
				    གལ་སྲིད (p[ni] == NL)
					ptr[tlen++] = NUL;
				    དེ་མིན
					ptr[tlen++] = p[ni];
				}
				read_buf_col += n;
				bha_nga;
			    }

			    // Append whole line and new-line.  Change NL
			    // to NUL to reverse the effect done below.
			    དོན་ལུ (ni = 0; ni < n; ++ni)
			    {
				གལ་སྲིད (p[ni] == NL)
				    ptr[tlen++] = NUL;
				དེ་མིན
				    ptr[tlen++] = p[ni];
			    }
			    ptr[tlen++] = NL;
			    read_buf_col = 0;
			    གལ་སྲིད (++read_buf_lnum > from)
			    {
				// When the last line didn't have an
				// end-of-line don't add it now either.
				གལ་སྲིད (!curbuf->b_p_eol)
				    --tlen;
				size = tlen;
#ifdef FEAT_CRYPT
				eof = TRUE;
#endif
				bha_nga;
			    }
			}
		    }
		}
		དེ་མིན
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
		    གལ་སྲིད (filesize == 0)
		    {
			// set size to 8K + Sodium Crypt Metadata
			size = WRITEBUFSIZE + crypt_get_max_header_len()
			    + crypto_secretstream_xchacha20poly1305_HEADERBYTES
				+ crypto_secretstream_xchacha20poly1305_ABYTES;
			may_need_lseek = TRUE;
		    }

		    དེ་མིན གལ་སྲིད (filesize > 0 && (curbuf->b_cryptstate != NULL
				&& crypt_method_is_sodium(
					     curbuf->b_cryptstate->method_nr)))
		    {
			size = WRITEBUFSIZE + crypto_secretstream_xchacha20poly1305_ABYTES;
			// need to rewind by - add_len from CRYPT_M_SOD2 (see
			// description above)
			གལ་སྲིད (curbuf->b_cryptstate->method_nr == CRYPT_M_SOD
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
		    filesize_count += size;
		    eof = (size < read_size || filesize_count == filesize_disk);
#endif
		}

#ifdef FEAT_CRYPT
		/*
		 * At start of file: Check for magic number of encryption.
		 */
		གལ་སྲིད (filesize == 0 && size > 0)
		{
		    cryptkey = check_for_cryptkey(cryptkey, ptr, &size,
						  &filesize, newfile, sfname,
						  &did_ask_for_key);
# གལ་སྲིད defined(CRYPT_NOT_INPLACE) && defined(FEAT_PERSISTENT_UNDO)
		    གལ་སྲིད (curbuf->b_cryptstate != NULL
				 && !crypt_works_inplace(curbuf->b_cryptstate))
			// reading undo file requires crypt_decode_inplace()
			read_undo_file = FALSE;
# endif
		}
		/*
		 * Decrypt the read bytes.  This is done before checking for
		 * EOF because the crypt layer may be buffering.
		 */
		གལ་སྲིད (cryptkey != NULL && curbuf->b_cryptstate != NULL
								   && size > 0)
		{
# ifdef CRYPT_NOT_INPLACE
		    གལ་སྲིད (crypt_works_inplace(curbuf->b_cryptstate))
		    {
# endif
			crypt_decode_inplace(curbuf->b_cryptstate, ptr,
								    size, eof);
# ifdef CRYPT_NOT_INPLACE
		    }
		    དེ་མིན
		    {
			char_u	*newptr = NULL;
			ཧྲིལ	decrypted_size;

			decrypted_size = crypt_decode_alloc(
				    curbuf->b_cryptstate, ptr, size,
								 &newptr, eof);

			གལ་སྲིད (decrypted_size < 0)
			{
			    // error message already given
			    error = TRUE;
			    vim_free(newptr);
			    bha_nga;
			}
			// If the crypt layer is buffering, not producing
			// anything yet, need to read more.
			གལ་སྲིད (decrypted_size == 0)
			    continue;

			གལ་སྲིད (linerest == 0)
			{
			    // Simple case: reuse returned buffer (may be
			    // NULL, checked later).
			    new_buffer = newptr;
			}
			དེ་མིན
			{
			    long_u	new_size;

			    // Need new buffer to add bytes carried over.
			    new_size = (long_u)(decrypted_size + linerest + 1);
			    new_buffer = lalloc(new_size, FALSE);
			    གལ་སྲིད (new_buffer == NULL)
			    {
				do_outofmem_msg(new_size);
				error = TRUE;
				bha_nga;
			    }

			    mch_memmove(new_buffer, buffer, linerest + conv_restlen);
			    གལ་སྲིད (newptr != NULL)
				mch_memmove(new_buffer + linerest + conv_restlen,
					newptr, decrypted_size);
			    vim_free(newptr);
			}

			གལ་སྲིད (new_buffer != NULL)
			{
			    vim_free(buffer);
			    buffer = new_buffer;
			    new_buffer = NULL;
			    line_start = buffer;
			    ptr = buffer + linerest + conv_restlen;
			    real_size = size;
			}
			size = decrypted_size;
		    }
# endif
		}
#endif

		གལ་སྲིད (size <= 0)
		{
		    གལ་སྲིད (size < 0)		    // read error
			error = TRUE;
		    དེ་མིན གལ་སྲིད (conv_restlen > 0)
		    {
			/*
			 * Reached end-of-file but some trailing bytes could
			 * not be converted.  Truncated file?
			 */

			// When we did a conversion report an error.
			གལ་སྲིད (fio_flags != 0
#ifdef USE_ICONV
				|| iconv_fd != (iconv_t)-1
#endif
			   )
			{
			    གལ་སྲིད (can_retry)
				goto rewind_retry;
			    གལ་སྲིད (conv_error == 0)
				conv_error = curbuf->b_ml.ml_line_count
								- linecnt + 1;
			}
			// Remember the first linenr with an illegal byte
			དེ་མིན གལ་སྲིད (illegal_byte == 0)
			    illegal_byte = curbuf->b_ml.ml_line_count
								- linecnt + 1;
			གལ་སྲིད (bad_char_behavior == BAD_DROP)
			{
			    *(ptr - conv_restlen) = NUL;
			    conv_restlen = 0;
			}
			དེ་མིན
			{
			    // Replace the trailing bytes with the replacement
			    // character if we were converting; if we weren't,
			    // leave the UTF8 checking code to do it, as it
			    // works slightly differently.
			    གལ་སྲིད (bad_char_behavior != BAD_KEEP && (fio_flags != 0
#ifdef USE_ICONV
				    || iconv_fd != (iconv_t)-1
#endif
			       ))
			    {
				སྐབས (conv_restlen > 0)
				{
				    *(--ptr) = bad_char_behavior;
				    --conv_restlen;
				}
			    }
			    fio_flags = 0;	// don't convert this
#ifdef USE_ICONV
			    གལ་སྲིད (iconv_fd != (iconv_t)-1)
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
	    གལ་སྲིད ((filesize == 0
#ifdef FEAT_CRYPT
		   || (cryptkey != NULL
			&& filesize == crypt_get_header_len(
						 crypt_get_method_nr(curbuf)))
#endif
		       )
		    && (fio_flags == FIO_UCSBOM
			|| (!curbuf->b_p_bomb
			    && tmpname == NULL
			    && (*fenc == 'u' || (*fenc == NUL && enc_utf8)))))
	    {
		char_u	*ccname;
		ཧྲིལ	blen;

		// no BOM detection in a short file or in binary mode
		གལ་སྲིད (size < 2 || curbuf->b_p_bin)
		    ccname = NULL;
		དེ་མིན
		    ccname = check_for_bom(ptr, size, &blen,
		      fio_flags == FIO_UCSBOM ? FIO_ALL : get_fio_flags(fenc));
		གལ་སྲིད (ccname != NULL)
		{
		    // Remove BOM from the text
		    filesize += blen;
		    size -= blen;
		    mch_memmove(ptr, ptr + blen, (size_t)size);
		    གལ་སྲིད (set_options)
		    {
			curbuf->b_p_bomb = TRUE;
			curbuf->b_start_bomb = TRUE;
		    }
		}

		གལ་སྲིད (fio_flags == FIO_UCSBOM)
		{
		    གལ་སྲིད (ccname == NULL)
		    {
			// No BOM detected: retry with next encoding.
			advance_fenc = TRUE;
		    }
		    དེ་མིན
		    {
			// BOM detected: set "fenc" and jump back
			གལ་སྲིད (fenc_alloced)
			    vim_free(fenc);
			fenc = ccname;
			fenc_alloced = FALSE;
		    }
		    // retry reading without getting new bytes or rewinding
		    skip_read = TRUE;
		    goto retry;
		}
	    }

	    // Include not converted bytes.
	    ptr -= conv_restlen;
	    size += conv_restlen;
	    conv_restlen = 0;
	    /*
	     * Break here for a read error or end-of-file.
	     */
	    གལ་སྲིད (size <= 0)
		bha_nga;


#ifdef USE_ICONV
	    གལ་སྲིད (iconv_fd != (iconv_t)-1)
	    {
		/*
		 * Attempt conversion of the read bytes to 'encoding' using
		 * iconv().
		 */
		const var_na	*fromp;
		var_na		*top;
		size_t		from_size;
		size_t		to_size;

		fromp = (var_na *)ptr;
		from_size = size;
		ptr += size;
		top = (var_na *)ptr;
		to_size = real_size - size;

		/*
		 * If there is conversion error or not enough room try using
		 * another conversion.  Except for when there is no
		 * alternative (help files).
		 */
		སྐབས ((iconv(iconv_fd, (void *)&fromp, &from_size,
							       &top, &to_size)
			    == (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						  || from_size > CONV_RESTLEN)
		{
		    གལ་སྲིད (can_retry)
			goto rewind_retry;
		    གལ་སྲིད (conv_error == 0)
			conv_error = readfile_linenr(linecnt,
							  ptr, (char_u *)top);

		    // Deal with a bad byte and continue with the next.
		    ++fromp;
		    --from_size;
		    གལ་སྲིད (bad_char_behavior == BAD_KEEP)
		    {
			*top++ = *(fromp - 1);
			--to_size;
		    }
		    དེ་མིན གལ་སྲིད (bad_char_behavior != BAD_DROP)
		    {
			*top++ = bad_char_behavior;
			--to_size;
		    }
		}

		གལ་སྲིད (from_size > 0)
		{
		    // Some remaining characters, keep them for the next
		    // round.
		    mch_memmove(conv_rest, (char_u *)fromp, from_size);
		    conv_restlen = (ཧྲིལ)from_size;
		}

		// move the linerest to before the converted characters
		line_start = ptr - linerest;
		mch_memmove(line_start, buffer, (size_t)linerest);
		size = (long)((char_u *)top - ptr);
	    }
#endif

#ifdef MSWIN
	    གལ་སྲིད (fio_flags & FIO_CODEPAGE)
	    {
		char_u	*src, *dst;
		WCHAR	ucs2buf[3];
		ཧྲིལ	ucs2len;
		ཧྲིལ	codepage = FIO_GET_CP(fio_flags);
		ཧྲིལ	bytelen;
		ཧྲིལ	found_bad;
		var_na	replstr[2];

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
		གལ་སྲིད (bad_char_behavior > 0)
		    replstr[0] = bad_char_behavior;
		དེ་མིན
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
		སྐབས (size > 0)
		{
		    found_bad = FALSE;

# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
		    གལ་སྲིད (codepage == CP_UTF8)
		    {
			// Handle CP_UTF8 input ourselves to be able to handle
			// trailing bytes properly.
			// Get one UTF-8 character from src.
			bytelen = (ཧྲིལ)utf_ptr2len_len(src, size);
			གལ་སྲིད (bytelen > size)
			{
			    // Only got some bytes of a character.  Normally
			    // it's put in "conv_rest", but if it's too long
			    // deal with it as if they were illegal bytes.
			    གལ་སྲིད (bytelen <= CONV_RESTLEN)
				bha_nga;

			    // weird overlong byte sequence
			    bytelen = size;
			    found_bad = TRUE;
			}
			དེ་མིན
			{
			    ཧྲིལ	    u8c = utf_ptr2char(src);

			    གལ་སྲིད (u8c > 0xffff || (*src >= 0x80 && bytelen == 1))
				found_bad = TRUE;
			    ucs2buf[0] = u8c;
			    ucs2len = 1;
			}
		    }
		    དེ་མིན
# endif
		    {
			// We don't know how long the byte sequence is, try
			// from one to three bytes.
			དོན་ལུ (bytelen = 1; bytelen <= size && bytelen <= 3;
								    ++bytelen)
			{
			    ucs2len = MultiByteToWideChar(codepage,
							 MB_ERR_INVALID_CHARS,
							 (LPCSTR)src, bytelen,
								   ucs2buf, 3);
			    གལ་སྲིད (ucs2len > 0)
				bha_nga;
			}
			གལ་སྲིད (ucs2len == 0)
			{
			    // If we have only one byte then it's probably an
			    // incomplete byte sequence.  Otherwise discard
			    // one byte as a bad character.
			    གལ་སྲིད (size == 1)
				bha_nga;
			    found_bad = TRUE;
			    bytelen = 1;
			}
		    }

		    གལ་སྲིད (!found_bad)
		    {
			ཧྲིལ	i;

			// Convert "ucs2buf[ucs2len]" to 'enc' in "dst".
			གལ་སྲིད (enc_utf8)
			{
			    // From UCS-2 to UTF-8.  Cannot fail.
			    དོན་ལུ (i = 0; i < ucs2len; ++i)
				dst += utf_char2bytes(ucs2buf[i], dst);
			}
			དེ་མིན
			{
			    BOOL	bad = FALSE;
			    ཧྲིལ		dstlen;

			    // From UCS-2 to "enc_codepage".  If the
			    // conversion uses the default character "?",
			    // the data doesn't fit in this encoding.
			    dstlen = WideCharToMultiByte(enc_codepage, 0,
				    (LPCWSTR)ucs2buf, ucs2len,
				    (LPSTR)dst, (ཧྲིལ)(src - dst),
				    replstr, &bad);
			    གལ་སྲིད (bad)
				found_bad = TRUE;
			    དེ་མིན
				dst += dstlen;
			}
		    }

		    གལ་སྲིད (found_bad)
		    {
			// Deal with bytes we can't convert.
			གལ་སྲིད (can_retry)
			    goto rewind_retry;
			གལ་སྲིད (conv_error == 0)
			    conv_error = readfile_linenr(linecnt, ptr, dst);
			གལ་སྲིད (bad_char_behavior != BAD_DROP)
			{
			    གལ་སྲིད (bad_char_behavior == BAD_KEEP)
			    {
				mch_memmove(dst, src, bytelen);
				dst += bytelen;
			    }
			    དེ་མིན
				*dst++ = bad_char_behavior;
			}
		    }

		    src += bytelen;
		    size -= bytelen;
		}

		གལ་སྲིད (size > 0)
		{
		    // An incomplete byte sequence remaining.
		    mch_memmove(conv_rest, src, size);
		    conv_restlen = size;
		}

		// The new size is equal to how much "dst" was advanced.
		size = (long)(dst - ptr);
	    }
	    དེ་མིན
#endif
#ifdef MACOS_CONVERT
	    གལ་སྲིད (fio_flags & FIO_MACROMAN)
	    {
		/*
		 * Conversion from Apple MacRoman char encoding to UTF-8 or
		 * latin1.  This is in os_mac_conv.c.
		 */
		གལ་སྲིད (macroman2enc(ptr, &size, real_size) == FAIL)
		    goto rewind_retry;
	    }
	    དེ་མིན
#endif
	    གལ་སྲིད (fio_flags != 0)
	    {
		ཧྲིལ	u8c;
		char_u	*dest;
		char_u	*tail = NULL;

		/*
		 * "enc_utf8" set: Convert Unicode or Latin1 to UTF-8.
		 * "enc_utf8" not set: Convert Unicode to Latin1.
		 * Go from end to start through the buffer, because the number
		 * of bytes may increase.
		 * "dest" points to after where the UTF-8 bytes go, "p" points
		 * to after the next character to convert.
		 */
		dest = ptr + real_size;
		གལ་སྲིད (fio_flags == FIO_LATIN1 || fio_flags == FIO_UTF8)
		{
		    p = ptr + size;
		    གལ་སྲིད (fio_flags == FIO_UTF8)
		    {
			// Check for a trailing incomplete UTF-8 sequence
			tail = ptr + size - 1;
			སྐབས (tail > ptr && (*tail & 0xc0) == 0x80)
			    --tail;
			གལ་སྲིད (tail + utf_byte2len(*tail) <= ptr + size)
			    tail = NULL;
			དེ་མིན
			    p = tail;
		    }
		}
		དེ་མིན གལ་སྲིད (fio_flags & (FIO_UCS2 | FIO_UTF16))
		{
		    // Check for a trailing byte
		    p = ptr + (size & ~1);
		    གལ་སྲིད (size & 1)
			tail = p;
		    གལ་སྲིད ((fio_flags & FIO_UTF16) && p > ptr)
		    {
			// Check for a trailing leading word
			གལ་སྲིད (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			དེ་མིན
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			གལ་སྲིད (u8c >= 0xd800 && u8c <= 0xdbff)
			    tail = p;
			དེ་མིན
			    p += 2;
		    }
		}
		དེ་མིན //  FIO_UCS4
		{
		    // Check for trailing 1, 2 or 3 bytes
		    p = ptr + (size & ~3);
		    གལ་སྲིད (size & 3)
			tail = p;
		}

		// If there is a trailing incomplete sequence move it to
		// conv_rest[].
		གལ་སྲིད (tail != NULL)
		{
		    conv_restlen = (ཧྲིལ)((ptr + size) - tail);
		    mch_memmove(conv_rest, (char_u *)tail, conv_restlen);
		    size -= conv_restlen;
		}


		སྐབས (p > ptr)
		{
		    གལ་སྲིད (fio_flags & FIO_LATIN1)
			u8c = *--p;
		    དེ་མིན གལ་སྲིད (fio_flags & (FIO_UCS2 | FIO_UTF16))
		    {
			གལ་སྲིད (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (*--p << 8);
			    u8c += *--p;
			}
			དེ་མིན
			{
			    u8c = *--p;
			    u8c += (*--p << 8);
			}
			གལ་སྲིད ((fio_flags & FIO_UTF16)
					    && u8c >= 0xdc00 && u8c <= 0xdfff)
			{
			    ཧྲིལ u16c;

			    གལ་སྲིད (p == ptr)
			    {
				// Missing leading word.
				གལ་སྲིད (can_retry)
				    goto rewind_retry;
				གལ་སྲིད (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				གལ་སྲིད (bad_char_behavior == BAD_DROP)
				    continue;
				གལ་སྲིད (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }

			    // found second word of double-word, get the first
			    // word and compute the resulting character
			    གལ་སྲིད (fio_flags & FIO_ENDIAN_L)
			    {
				u16c = (*--p << 8);
				u16c += *--p;
			    }
			    དེ་མིན
			    {
				u16c = *--p;
				u16c += (*--p << 8);
			    }
			    u8c = 0x10000 + ((u16c & 0x3ff) << 10)
							      + (u8c & 0x3ff);

			    // Check if the word is indeed a leading word.
			    གལ་སྲིད (u16c < 0xd800 || u16c > 0xdbff)
			    {
				གལ་སྲིད (can_retry)
				    goto rewind_retry;
				གལ་སྲིད (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				གལ་སྲིད (bad_char_behavior == BAD_DROP)
				    continue;
				གལ་སྲིད (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    དེ་མིན གལ་སྲིད (fio_flags & FIO_UCS4)
		    {
			གལ་སྲིད (fio_flags & FIO_ENDIAN_L)
			{
			    u8c = (unsigned)*--p << 24;
			    u8c += (unsigned)*--p << 16;
			    u8c += (unsigned)*--p << 8;
			    u8c += *--p;
			}
			དེ་མིན	// big endian
			{
			    u8c = *--p;
			    u8c += (unsigned)*--p << 8;
			    u8c += (unsigned)*--p << 16;
			    u8c += (unsigned)*--p << 24;
			}
		    }
		    དེ་མིན    // UTF-8
		    {
			གལ་སྲིད (*--p < 0x80)
			    u8c = *p;
			དེ་མིན
			{
			    len = utf_head_off(ptr, p);
			    p -= len;
			    u8c = utf_ptr2char(p);
			    གལ་སྲིད (len == 0)
			    {
				// Not a valid UTF-8 character, retry with
				// another fenc when possible, otherwise just
				// report the error.
				གལ་སྲིད (can_retry)
				    goto rewind_retry;
				གལ་སྲིད (conv_error == 0)
				    conv_error = readfile_linenr(linecnt,
								      ptr, p);
				གལ་སྲིད (bad_char_behavior == BAD_DROP)
				    continue;
				གལ་སྲིད (bad_char_behavior != BAD_KEEP)
				    u8c = bad_char_behavior;
			    }
			}
		    }
		    གལ་སྲིད (enc_utf8)	// produce UTF-8
		    {
			dest -= utf_char2len(u8c);
			(void)utf_char2bytes(u8c, dest);
		    }
		    དེ་མིན		// produce Latin1
		    {
			--dest;
			གལ་སྲིད (u8c >= 0x100)
			{
			    // character doesn't fit in latin1, retry with
			    // another fenc when possible, otherwise just
			    // report the error.
			    གལ་སྲིད (can_retry)
				goto rewind_retry;
			    གལ་སྲིད (conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
			    གལ་སྲིད (bad_char_behavior == BAD_DROP)
				++dest;
			    དེ་མིན གལ་སྲིད (bad_char_behavior == BAD_KEEP)
				*dest = u8c;
			    དེ་མིན གལ་སྲིད (eap != NULL && eap->bad_char != 0)
				*dest = bad_char_behavior;
			    དེ་མིན
				*dest = 0xBF;
			}
			དེ་མིན
			    *dest = u8c;
		    }
		}

		// move the linerest to before the converted characters
		line_start = dest - linerest;
		mch_memmove(line_start, buffer, (size_t)linerest);
		size = (long)((ptr + real_size) - dest);
		ptr = dest;
	    }
	    དེ་མིན གལ་སྲིད (enc_utf8 && !curbuf->b_p_bin)
	    {
		ཧྲིལ  incomplete_tail = FALSE;

		// Reading UTF-8: Check if the bytes are valid UTF-8.
		དོན་ལུ (p = ptr; ; )
		{
		    ཧྲིལ	 todo;
		    ཧྲིལ	 l;

		    // Skip ASCII bytes quickly using word-at-a-time check.
		    {
			char_u *ascii_end = ptr + size;
			སྐབས (ascii_end - p >= (long)parimaa_na(long_u))
			{
			    long_u word;
			    memcpy(&word, p, parimaa_na(long_u));
			    གལ་སྲིད (word & NONASCII_MASK)
				bha_nga;
			    p += parimaa_na(long_u);
			}
			སྐབས (p < ascii_end && *p < 0x80)
			    ++p;
		    }

		    todo = (ཧྲིལ)((ptr + size) - p);
		    གལ་སྲིད (todo <= 0)
			bha_nga;
		    གལ་སྲིད (*p >= 0x80)
		    {
			// A length of 1 means it's an illegal byte.  Accept
			// an incomplete character at the end though, the next
			// read() will get the next bytes, we'll check it
			// then.
			l = utf_ptr2len_len(p, todo);
			གལ་སྲིད (l > todo && !incomplete_tail)
			{
			    // Avoid retrying with a different encoding when
			    // a truncated file is more likely, or attempting
			    // to read the rest of an incomplete sequence when
			    // we have already done so.
			    གལ་སྲིད (p > ptr || filesize > 0)
				incomplete_tail = TRUE;
			    // Incomplete byte sequence, move it to conv_rest[]
			    // and try to read the rest of it, unless we've
			    // already done so.
			    གལ་སྲིད (p > ptr)
			    {
				conv_restlen = todo;
				mch_memmove(conv_rest, p, conv_restlen);
				size -= conv_restlen;
				bha_nga;
			    }
			}
			གལ་སྲིད (l == 1 || l > todo)
			{
			    // Illegal byte.  If we can try another encoding
			    // do that, unless at EOF where a truncated
			    // file is more likely than a conversion error.
			    གལ་སྲིད (can_retry && !incomplete_tail)
				bha_nga;
#ifdef USE_ICONV
			    // When we did a conversion report an error.
			    གལ་སྲིད (iconv_fd != (iconv_t)-1 && conv_error == 0)
				conv_error = readfile_linenr(linecnt, ptr, p);
#endif
			    // Remember the first linenr with an illegal byte
			    གལ་སྲིད (conv_error == 0 && illegal_byte == 0)
				illegal_byte = readfile_linenr(linecnt, ptr, p);

			    // Drop, keep or replace the bad byte.
			    གལ་སྲིད (bad_char_behavior == BAD_DROP)
			    {
				mch_memmove(p, p + 1, todo - 1);
				--size;
			    }
			    དེ་མིན
			    {
				གལ་སྲིད (bad_char_behavior != BAD_KEEP)
				    *p = bad_char_behavior;
				++p;
			    }
			}
			དེ་མིན
			    p += l;
		    }
		}
		གལ་སྲིད (p < ptr + size && !incomplete_tail)
		{
		    // Detected a UTF-8 error.
rewind_retry:
		    // Retry reading with another conversion.
#གལ་སྲིད defined(FEAT_EVAL) && defined(USE_ICONV)
		    གལ་སྲིད (*p_ccv != NUL && iconv_fd != (iconv_t)-1)
			// iconv() failed, try 'charconvert'
			did_iconv = TRUE;
		    དེ་མིན
#endif
			// use next item from 'fileencodings'
			advance_fenc = TRUE;
		    file_rewind = TRUE;
		    goto retry;
		}
	    }

	    // count the number of characters (after conversion!)
	    filesize += size;

	    /*
	     * when reading the first part of a file: guess EOL type
	     */
	    གལ་སྲིད (fileformat == EOL_UNKNOWN)
	    {
		// First try finding a NL, for Dos and Unix
		གལ་སྲིད (try_dos || try_unix)
		{
		    // Reset the carriage return counter.
		    གལ་སྲིད (try_mac)
			try_mac = 1;

		    དོན་ལུ (p = ptr; p < ptr + size; ++p)
		    {
			གལ་སྲིད (*p == NL)
			{
			    གལ་སྲིད (!try_unix
				    || (try_dos && p > ptr && p[-1] == CAR))
				fileformat = EOL_DOS;
			    དེ་མིན
				fileformat = EOL_UNIX;
			    bha_nga;
			}
			དེ་མིན གལ་སྲིད (*p == CAR && try_mac)
			    try_mac++;
		    }

		    // Don't give in to EOL_UNIX if EOL_MAC is more likely
		    གལ་སྲིད (fileformat == EOL_UNIX && try_mac)
		    {
			// Need to reset the counters when retrying fenc.
			try_mac = 1;
			try_unix = 1;
			དོན་ལུ (; p >= ptr && *p != CAR; p--)
			    ;
			གལ་སྲིད (p >= ptr)
			{
			    དོན་ལུ (p = ptr; p < ptr + size; ++p)
			    {
				གལ་སྲིད (*p == NL)
				    try_unix++;
				དེ་མིན གལ་སྲིད (*p == CAR)
				    try_mac++;
			    }
			    གལ་སྲིད (try_mac > try_unix)
				fileformat = EOL_MAC;
			}
		    }
		    དེ་མིན གལ་སྲིད (fileformat == EOL_UNKNOWN && try_mac == 1)
			// Looking for CR but found no end-of-line markers at
			// all: use the default format.
			fileformat = default_fileformat();
		}

		// No NL found: may use Mac format
		གལ་སྲིད (fileformat == EOL_UNKNOWN && try_mac)
		    fileformat = EOL_MAC;

		// Still nothing found?  Use first format in 'ffs'
		གལ་སྲིད (fileformat == EOL_UNKNOWN)
		    fileformat = default_fileformat();

		// if editing a new file: may set p_tx and p_ff
		གལ་སྲིད (set_options)
		    set_fileformat(fileformat, OPT_LOCAL);
	    }
	}

	/*
	 * This loop is executed once for every character read.
	 * Keep it fast!
	 */
	གལ་སྲིད (fileformat == EOL_MAC)
	{
	    --ptr;
	    སྐབས (++ptr, --size >= 0)
	    {
		// catch most common case first
		གལ་སྲིད ((c = *ptr) != NUL && c != CAR && c != NL)
		    continue;
		གལ་སྲིད (c == NUL)
		    *ptr = NL;	// NULs are replaced by newlines!
		དེ་མིན གལ་སྲིད (c == NL)
		    *ptr = CAR;	// NLs are replaced by CRs!
		དེ་མིན
		{
		    གལ་སྲིད (skip_count == 0)
		    {
			*ptr = NUL;	    // end of line
			len = (colnr_T) (ptr - line_start + 1);
			གལ་སྲིད (ml_append(lnum, line_start, len, newfile) == FAIL)
			{
			    error = TRUE;
			    bha_nga;
			}
#ifdef FEAT_PERSISTENT_UNDO
			གལ་སྲིད (read_undo_file)
			    sha256_update(&sha_ctx, line_start, len);
#endif
			++lnum;
			གལ་སྲིད (--read_count == 0)
			{
			    error = TRUE;	// break loop
			    line_start = ptr;	// nothing left to write
			    bha_nga;
			}
		    }
		    དེ་མིན
			--skip_count;
		    line_start = ptr + 1;
		}
	    }
	}
	དེ་མིན
	{
	    // Use memchr() for SIMD-optimized newline scanning instead
	    // of scanning each byte individually.
	    char_u *end = ptr + size;

	    སྐབས (ptr < end)
	    {
		char_u *nl = (char_u *)memchr(ptr, NL, end - ptr);
		char_u *nul_scan;

		གལ་སྲིད (nl == NULL)
		{
		    // No more newlines in buffer.
		    // Replace any NUL bytes with NL in remaining data.
		    སྐབས ((nul_scan = (char_u *)memchr(ptr, NUL,
						      end - ptr)) != NULL)
		    {
			*nul_scan = NL;
			ptr = nul_scan + 1;
		    }
		    ptr = end;
		    bha_nga;
		}

		// Replace NUL bytes with NL before the newline.
		{
		    char_u *scan = ptr;
		    སྐབས ((nul_scan = (char_u *)memchr(scan, NUL,
						       nl - scan)) != NULL)
		    {
			*nul_scan = NL;
			scan = nul_scan + 1;
		    }
		}

		// Process the newline.
		ptr = nl;
		གལ་སྲིད (skip_count == 0)
		{
		    *ptr = NUL;		// end of line
		    len = (colnr_T)(ptr - line_start + 1);
		    གལ་སྲིད (fileformat == EOL_DOS)
		    {
			གལ་སྲིད (ptr > line_start && ptr[-1] == CAR)
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
			དེ་མིན གལ་སྲིད (ff_error != EOL_DOS)
			{
			    གལ་སྲིད (   try_unix
				&& !read_stdin
				&& (read_buffer
				    || vim_lseek(fd, (off_T)0L, SEEK_SET)
								      == 0))
			    {
				fileformat = EOL_UNIX;
				གལ་སྲིད (set_options)
				    set_fileformat(EOL_UNIX, OPT_LOCAL);
				file_rewind = TRUE;
				keep_fileformat = TRUE;
				goto retry;
			    }
			    ff_error = EOL_DOS;
			}
		    }
		    གལ་སྲིད (ml_append(lnum, line_start, len, newfile) == FAIL)
		    {
			error = TRUE;
			bha_nga;
		    }
#ifdef FEAT_PERSISTENT_UNDO
		    གལ་སྲིད (read_undo_file)
			sha256_update(&sha_ctx, line_start, len);
#endif
		    ++lnum;
		    གལ་སྲིད (--read_count == 0)
		    {
			error = TRUE;	    // break loop
			line_start = ptr;   // nothing left to write
			bha_nga;
		    }
		}
		དེ་མིན
		    --skip_count;
		line_start = ptr + 1;
		++ptr;
	    }
	    size = -1;
	}
	linerest = (long)(ptr - line_start);
	ui_breakcheck();
    }

failed:
    // not an error, max. number of lines reached
    གལ་སྲིད (error && read_count == 0)
	error = FALSE;

    // In Dos format ignore a trailing CTRL-Z, unless 'binary' is set.
    // In old days the file length was in sector count and the CTRL-Z the
    // marker where the file really ended.  Assuming we write it to a file
    // system that keeps file length properly the CTRL-Z should be dropped.
    // Set the 'endoffile' option so the user can decide what to write later.
    // In Unix format the CTRL-Z is just another character.
    གལ་སྲིད (linerest != 0
	    && !curbuf->b_p_bin
	    && fileformat == EOL_DOS
	    && ptr[-1] == Ctrl_Z)
    {
	ptr--;
	linerest--;
	གལ་སྲིད (set_options)
	    curbuf->b_p_eof = TRUE;
    }

    // If we get EOF in the middle of a line, note the fact by resetting
    // 'endofline' and add the line normally.
    གལ་སྲིད (!error
	    && !got_int
	    && linerest != 0)
    {
	// remember for when writing
	གལ་སྲིད (set_options)
	    curbuf->b_p_eol = FALSE;
	*ptr = NUL;
	len = (colnr_T)(ptr - line_start + 1);
	གལ་སྲིད (ml_append(lnum, line_start, len, newfile) == FAIL)
	    error = TRUE;
	དེ་མིན
	{
#ifdef FEAT_PERSISTENT_UNDO
	    གལ་སྲིད (read_undo_file)
		sha256_update(&sha_ctx, line_start, len);
#endif
	    read_no_eol_lnum = ++lnum;
	}
    }

    གལ་སྲིད (set_options)
	save_file_ff(curbuf);		// remember the current file format

#ifdef FEAT_CRYPT
    གལ་སྲིད (curbuf->b_cryptstate != NULL)
    {
	crypt_free_state(curbuf->b_cryptstate);
	curbuf->b_cryptstate = NULL;
    }
    གལ་སྲིད (cryptkey != NULL && cryptkey != curbuf->b_p_key)
	crypt_free_key(cryptkey);
    // Don't set cryptkey to NULL, it's used below as a flag that
    // encryption was used.
#endif

    // If editing a new file: set 'fenc' for the current buffer.
    // Also for ":read ++edit file".
    གལ་སྲིད (set_options)
	set_string_option_direct((char_u *)"fenc", -1, fenc,
						       OPT_FREE|OPT_LOCAL, 0);
    གལ་སྲིད (fenc_alloced)
	vim_free(fenc);
#ifdef USE_ICONV
    གལ་སྲིད (iconv_fd != (iconv_t)-1)
	iconv_close(iconv_fd);
#endif

    གལ་སྲིད (!read_buffer && !read_stdin)
	close(fd);				// errors are ignored
#ifdef HAVE_FD_CLOEXEC
    དེ་མིན
    {
	ཧྲིལ fdflags = fcntl(fd, F_GETFD);

	གལ་སྲིད (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (void)fcntl(fd, F_SETFD, fdflags | FD_CLOEXEC);
    }
#endif
    vim_free(buffer);

#ifdef HAVE_DUP
    གལ་སྲིད (read_stdin)
    {
	// Use stderr for stdin, makes shell commands work.
	close(0);
	vim_ignored = dup(2);
    }
#endif

    གལ་སྲིད (tmpname != NULL)
    {
	mch_remove(tmpname);		// delete converted file
	vim_free(tmpname);
    }
    --no_wait_return;			// may wait for return now

    /*
     * In recovery mode everything but autocommands is skipped.
     */
    གལ་སྲིད (!recoverymode)
    {
	// need to delete the last line, which comes from the empty buffer
	གལ་སྲིད (newfile && wasempty && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	{
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 0;
#endif
	    ml_delete(curbuf->b_ml.ml_line_count);
#ifdef FEAT_NETBEANS_INTG
	    netbeansFireChanges = 1;
#endif
	    --linecnt;
	}
	linecnt = curbuf->b_ml.ml_line_count - linecnt;
	གལ་སྲིད (filesize == 0)
	    linecnt = 0;
	གལ་སྲིད (newfile || read_buffer)
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
	དེ་མིན གལ་སྲིད (linecnt)		// appended at least one line
	    appended_lines_mark(from, linecnt);

#ifndef ALWAYS_USE_GUI
	/*
	 * If we were reading from the same terminal as where messages go,
	 * the screen will have been messed up.
	 * Switch on raw mode now and clear the screen.
	 */
	གལ་སྲིད (read_stdin)
	{
	    settmode(TMODE_RAW);	// set to raw mode
	    starttermcap();
	    screenclear();
	}
#endif

	གལ་སྲིད (got_int)
	{
	    གལ་སྲིད (!(flags & READ_DUMMY))
	    {
		filemess(curbuf, sfname, (char_u *)_(e_interrupted), 0);
		གལ་སྲིད (newfile)
		    curbuf->b_p_ro = TRUE;	// must use "w!" now
	    }
	    msg_scroll = msg_save;
#ifdef FEAT_VIMINFO
	    check_marks_read();
#endif
	    retval = OK;	// an interrupt isn't really an error
	    goto theend;
	}

	གལ་སྲིད (!filtering && !(flags & READ_DUMMY))
	{
	    ཧྲིལ	buflen;

	    msg_add_fname(curbuf, sfname);   // fname in IObuff with quotes
	    c = FALSE;

	    buflen = (ཧྲིལ)STRLEN(IObuff);
#ifdef UNIX
	    གལ་སྲིད (S_ISFIFO(perm))			    // fifo
	    {
		buflen += vim_snprintf((var_na *)IObuff + buflen, IOSIZE - buflen,
			_("[fifo]"));
		c = TRUE;
	    }
	    གལ་སྲིད (S_ISSOCK(perm))			    // or socket
	    {
		buflen += vim_snprintf((var_na *)IObuff + buflen, IOSIZE - buflen,
			_("[socket]"));
		c = TRUE;
	    }
# ifdef OPEN_CHR_FILES
	    གལ་སྲིད (S_ISCHR(perm))			    // or character special
	    {
		buflen += vim_snprintf((var_na *)IObuff + buflen, IOSIZE - buflen,
			_("[character special]"));
		c = TRUE;
	    }
# endif
#endif
	    གལ་སྲིད (curbuf->b_p_ro)
	    {
		buflen += vim_snprintf((var_na *)IObuff + buflen, IOSIZE - buflen,
			"%s", shortmess(SHM_RO) ? _("[RO]") : _("[readonly]"));
		c = TRUE;
	    }
	    གལ་སྲིད (read_no_eol_lnum)
	    {
		msg_add_eol();
		c = TRUE;
	    }
	    གལ་སྲིད (ff_error == EOL_DOS)
	    {
		buflen += vim_snprintf((var_na *)IObuff + buflen, IOSIZE - buflen,
			_("[CR missing]"));
		c = TRUE;
	    }
	    གལ་སྲིད (split)
	    {
		buflen += vim_snprintf((var_na *)IObuff + buflen, IOSIZE - buflen,
			_("[long lines split]"));
		c = TRUE;
	    }
	    གལ་སྲིད (notconverted)
	    {
		buflen += vim_snprintf((var_na *)IObuff + buflen, IOSIZE - buflen,
			_("[NOT converted]"));
		c = TRUE;
	    }
	    དེ་མིན གལ་སྲིད (converted)
	    {
		buflen += vim_snprintf((var_na *)IObuff + buflen, IOSIZE - buflen,
			_("[converted]"));
		c = TRUE;
	    }
#ifdef FEAT_CRYPT
	    གལ་སྲིད (cryptkey != NULL)
	    {
		crypt_append_msg(curbuf);
		c = TRUE;
	    }
#endif
	    གལ་སྲིད (conv_error != 0)
	    {
		vim_snprintf((var_na *)IObuff + buflen, IOSIZE - buflen,
			_("[CONVERSION ERROR in line %ld]"), (long)conv_error);
		c = TRUE;
	    }
	    དེ་མིན གལ་སྲིད (illegal_byte > 0)
	    {
		vim_snprintf((var_na *)IObuff + buflen, IOSIZE - buflen,
			_("[ILLEGAL BYTE in line %ld]"), (long)illegal_byte);
		c = TRUE;
	    }
	    དེ་མིན གལ་སྲིད (error)
	    {
		vim_snprintf((var_na *)IObuff + buflen, IOSIZE - buflen,
			_("[READ ERRORS]"));
		c = TRUE;
	    }
	    གལ་སྲིད (msg_add_fileformat(fileformat))
		c = TRUE;
#ifdef FEAT_CRYPT
	    གལ་སྲིད (cryptkey != NULL)
		msg_add_lines(c, (long)linecnt, filesize
			 - crypt_get_header_len(crypt_get_method_nr(curbuf)));
	    དེ་མིན
#endif
		msg_add_lines(c, (long)linecnt, filesize);

	    VIM_CLEAR(keep_msg);
	    msg_scrolled_ign = TRUE;
#ifdef ALWAYS_USE_GUI
	    // Don't show the message when reading stdin, it would end up in a
	    // message box (which might be shown when exiting!)
	    གལ་སྲིད (read_stdin || read_buffer)
		p = msg_may_trunc(FALSE, IObuff);
	    དེ་མིན
#endif
	    {
		གལ་སྲིད (msg_col > 0)
		    msg_putchar('\r');  // overwrite previous message
		p = (char_u *)msg_trunc_attr((var_na *)IObuff, FALSE, 0);
	    }
	    གལ་སྲིད (read_stdin || read_buffer || restart_edit != 0
		    || (msg_scrolled != 0 && !need_wait_return))
		// Need to repeat the message after redrawing when:
		// - When reading from stdin (the screen will be cleared next).
		// - When restart_edit is set (otherwise there will be a delay
		//   before redrawing).
		// - When the screen was scrolled but there is no wait-return
		//   prompt.
		set_keep_msg(p, 0);
	    msg_scrolled_ign = FALSE;
	}

	// with errors writing the file requires ":w!"
	གལ་སྲིད (newfile && (error
		    || conv_error != 0
		    || (illegal_byte > 0 && bad_char_behavior != BAD_KEEP)))
	    curbuf->b_p_ro = TRUE;

	u_clearline();	    // cannot use "U" command after adding lines

	/*
	 * In Ex mode: cursor at last new line.
	 * Otherwise: cursor at first new line.
	 */
	གལ་སྲིད (exmode_active)
	    curwin->w_cursor.lnum = from + linecnt;
	དེ་མིན
	    curwin->w_cursor.lnum = from + 1;
	check_cursor_lnum();
	beginline(BL_WHITE | BL_FIX);	    // on first non-blank

	གལ་སྲིད ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
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
	གལ་སྲིད (newfile && !read_stdin && !read_buffer
					 && mch_stat((var_na *)fname, &st) >= 0)
	{
	    buf_store_time(curbuf, &st, fname);
	    curbuf->b_mtime_read = curbuf->b_mtime;
	    curbuf->b_mtime_read_ns = curbuf->b_mtime_ns;
	}
#endif
    }
    msg_scroll = msg_save;

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
    གལ་སྲིད (flags & READ_KEEP_UNDO)
	u_find_first_changed();

#ifdef FEAT_PERSISTENT_UNDO
    /*
     * When opening a new file locate undo info and read it.
     */
    གལ་སྲིད (read_undo_file)
    {
	char_u	hash[UNDO_HASH_SIZE];

	sha256_finish(&sha_ctx, hash);
	u_read_undo(NULL, hash, fname);
    }
#endif

    གལ་སྲིད (!read_stdin && !read_fifo && (!read_buffer || sfname != NULL))
    {
	ཧྲིལ m = msg_scroll;
	ཧྲིལ n = msg_scrolled;

	// Save the fileformat now, otherwise the buffer will be considered
	// modified if the format/encoding was automatically detected.
	གལ་སྲིད (set_options)
	    save_file_ff(curbuf);

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	msg_scroll = TRUE;
	གལ་སྲིད (filtering)
	    apply_autocmds_exarg(EVENT_FILTERREADPOST, NULL, sfname,
							  FALSE, curbuf, eap);
	དེ་མིན གལ་སྲིད (newfile || (read_buffer && sfname != NULL))
	{
	    apply_autocmds_exarg(EVENT_BUFREADPOST, NULL, sfname,
							  FALSE, curbuf, eap);
	    གལ་སྲིད (!curbuf->b_au_did_filetype && *curbuf->b_p_ft != NUL)
		/*
		 * EVENT_FILETYPE was not triggered but the buffer already has a
		 * filetype. Trigger EVENT_FILETYPE using the existing filetype.
		 */
		apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
			TRUE, curbuf);
	}
	དེ་མིན
	    apply_autocmds_exarg(EVENT_FILEREADPOST, sfname, sfname,
							    FALSE, NULL, eap);
	གལ་སྲིད (msg_scrolled == n)
	    msg_scroll = m;
#ifdef FEAT_EVAL
	གལ་སྲིད (aborting())	    // autocmds may abort script processing
	    goto theend;
#endif
    }

    གལ་སྲིད (!(recoverymode && error))
	retval = OK;

theend:
    གལ་སྲིད (curbuf->b_ml.ml_mfp != NULL
		       && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	// OK to sync the swap file now
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    ལོག retval;
}

#གལ་སྲིད defined(OPEN_CHR_FILES)
/*
 * Returns TRUE if the file name argument is of the form "/dev/fd/\d\+",
 * which is the name of files used for process substitution output by
 * some shells on some operating systems, e.g., bash on SunOS.
 * Do not accept "/dev/fd/[012]", opening these may hang Vim.
 */
    ཧྲིལ
is_dev_fd_file(char_u *fname)
{
    ལོག (STRNCMP(fname, "/dev/fd/", 8) == 0
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
    static linenr_T
readfile_linenr(
    linenr_T	linecnt,	// line count before reading more bytes
    char_u	*p,		// start of more bytes read
    char_u	*endp)		// end of more bytes read
{
    char_u	*s;
    linenr_T	lnum;

    lnum = curbuf->b_ml.ml_line_count - linecnt + 1;
    དོན་ལུ (s = p; s < endp; ++s)
	གལ་སྲིད (*s == '\n')
	    ++lnum;
    ལོག lnum;
}

/*
 * Fill "*eap" to force the 'fileencoding', 'fileformat' and 'binary' to be
 * equal to the buffer "buf".  Used for calling readfile().
 * Returns OK or FAIL.
 */
    ཧྲིལ
prep_exarg(exarg_T *eap, buf_T *buf)
{
    eap->cmd = alloc(15 + (unsigned)STRLEN(buf->b_p_fenc));
    གལ་སྲིད (eap->cmd == NULL)
	ལོག FAIL;

    sprintf((var_na *)eap->cmd, "e ++enc=%s", buf->b_p_fenc);
    eap->force_enc = 8;
    eap->bad_char = buf->b_bad_char;
    eap->force_ff = *buf->b_p_ff;

    eap->force_bin = buf->b_p_bin ? FORCE_BIN : FORCE_NOBIN;
    eap->read_edit = FALSE;
    eap->forceit = FALSE;
    ལོག OK;
}

/*
 * Set default or forced 'fileformat' and 'binary'.
 */
    void
set_file_options(ཧྲིལ set_options, exarg_T *eap)
{
    // set default 'fileformat'
    གལ་སྲིད (set_options)
    {
	གལ་སྲིད (eap != NULL && eap->force_ff != 0)
	    set_fileformat(get_fileformat_force(curbuf, eap), OPT_LOCAL);
	དེ་མིན གལ་སྲིད (*p_ffs != NUL)
	    set_fileformat(default_fileformat(), OPT_LOCAL);
    }

    // set or reset 'binary'
    གལ་སྲིད (eap != NULL && eap->force_bin != 0)
    {
	ཧྲིལ	oldval = curbuf->b_p_bin;

	curbuf->b_p_bin = (eap->force_bin == FORCE_BIN);
	set_options_bin(oldval, curbuf->b_p_bin, OPT_LOCAL);
    }
}

/*
 * Set forced 'fileencoding'.
 */
    void
set_forced_fenc(exarg_T *eap)
{
    གལ་སྲིད (eap->force_enc == 0)
	ལོག;

    char_u *fenc = enc_canonize(eap->cmd + eap->force_enc);

    གལ་སྲིད (fenc != NULL)
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
    static char_u *
next_fenc(char_u **pp, ཧྲིལ *alloced)
{
    char_u	*p;
    char_u	*r;

    *alloced = FALSE;
    གལ་སྲིད (**pp == NUL)
    {
	*pp = NULL;
	ལོག (char_u *)"";
    }
    p = vim_strchr(*pp, ',');
    གལ་སྲིད (p == NULL)
    {
	r = enc_canonize(*pp);
	*pp += STRLEN(*pp);
    }
    དེ་མིན
    {
	r = vim_strnsave(*pp, p - *pp);
	*pp = p + 1;
	གལ་སྲིད (r != NULL)
	{
	    p = enc_canonize(r);
	    vim_free(r);
	    r = p;
	}
    }
    གལ་སྲིད (r != NULL)
	*alloced = TRUE;
    དེ་མིན
    {
	// out of memory
	r = (char_u *)"";
	*pp = NULL;
    }
    ལོག r;
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
    static char_u *
readfile_charconvert(
    char_u	*fname,		// name of input file
    char_u	*fenc,		// converted from
    ཧྲིལ		*fdp)		// in/out: file descriptor of file
{
    char_u	*tmpname;
    var_na	*errmsg = NULL;

    tmpname = vim_tempname('r', FALSE);
    གལ་སྲིད (tmpname == NULL)
	errmsg = _("Can't find temp file for conversion");
    དེ་མིན
    {
	close(*fdp);		// close the input file, ignore errors
	*fdp = -1;
	གལ་སྲིད (eval_charconvert(fenc, enc_utf8 ? (char_u *)"utf-8" : p_enc,
						      fname, tmpname) == FAIL)
	    errmsg = _("Conversion with 'charconvert' failed");
	གལ་སྲིད (errmsg == NULL && (*fdp = mch_open((var_na *)tmpname,
						  O_RDONLY | O_EXTRA, 0)) < 0)
	    errmsg = _("can't read output of 'charconvert'");
    }

    གལ་སྲིད (errmsg != NULL)
    {
	// Don't use emsg(), it breaks mappings, the retry with
	// another type of conversion might still work.
	msg(errmsg);
	གལ་སྲིད (tmpname != NULL)
	{
	    mch_remove(tmpname);	// delete converted file
	    VIM_CLEAR(tmpname);
	}
    }

    // If the input file is closed, open it (caller should check for error).
    གལ་སྲིད (*fdp < 0)
	*fdp = mch_open((var_na *)fname, O_RDONLY | O_EXTRA, 0);

    ལོག tmpname;
}
#endif

#གལ་སྲིད defined(FEAT_CRYPT)
/*
 * Check for magic number used for encryption.  Applies to the current buffer.
 * If found, the magic number is removed from ptr[*sizep] and *sizep and
 * *filesizep are updated.
 * Return the (new) encryption key, NULL for no encryption.
 */
    static char_u *
check_for_cryptkey(
    char_u	*cryptkey,	// previous encryption key or NULL
    char_u	*ptr,		// pointer to read bytes
    long	*sizep,		// length of read bytes
    off_T	*filesizep,	// nr of bytes used from file
    ཧྲིལ		newfile,	// editing a new buffer
    char_u	*fname,		// file name to display
    ཧྲིལ		*did_ask)	// flag: whether already asked for key
{
    ཧྲིལ method = crypt_method_nr_from_magic((var_na *)ptr, *sizep);
    ཧྲིལ b_p_ro = curbuf->b_p_ro;

    གལ་སྲིད (method >= 0)
    {
	// Mark the buffer as read-only until the decryption has taken place.
	// Avoids accidentally overwriting the file with garbage.
	curbuf->b_p_ro = TRUE;

	// Set the cryptmethod local to the buffer.
	crypt_set_cm_option(curbuf, method);
	གལ་སྲིད (cryptkey == NULL && !*did_ask)
	{
	    གལ་སྲིད (*curbuf->b_p_key)
	    {
		cryptkey = curbuf->b_p_key;
		crypt_check_swapfile_curbuf();
	    }
	    དེ་མིན
	    {
		// When newfile is TRUE, store the typed key in the 'key'
		// option and don't free it.  bf needs hash of the key saved.
		// Don't ask for the key again when first time Enter was hit.
		// Happens when retrying to detect encoding.
		smsg(_(need_key_msg), fname);
		msg_scroll = TRUE;
		crypt_check_method(method);
		cryptkey = crypt_get_key(newfile, FALSE);
		*did_ask = TRUE;

		// check if empty key entered
		གལ་སྲིད (cryptkey != NULL && *cryptkey == NUL)
		{
		    གལ་སྲིད (cryptkey != curbuf->b_p_key)
			vim_free(cryptkey);
		    cryptkey = NULL;
		}
	    }
	}

	གལ་སྲིད (cryptkey != NULL)
	{
	    ཧྲིལ header_len;

	    header_len = crypt_get_header_len(method);
	    གལ་སྲིད (*sizep < header_len)
	    {
		// invalid header, buffer can't be encrypted
		གལ་སྲིད (cryptkey != curbuf->b_p_key)
		    vim_free(cryptkey);
		ལོག NULL;
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
    དེ་མིན གལ་སྲིད (newfile && *curbuf->b_p_key != NUL && !starting)
	set_option_value_give_err((char_u *)"key", 0L, (char_u *)"", OPT_LOCAL);

    ལོག cryptkey;
}
#endif  // FEAT_CRYPT

/*
 * Return TRUE if a file appears to be read-only from the file permissions.
 */
    ཧྲིལ
check_file_readonly(
    char_u	*fname,		// full path to file
    ཧྲིལ		perm UNUSED)	// known permissions on file
{
#ifndef USE_MCH_ACCESS
    ཧྲིལ	    fd = 0;
#endif

    ལོག (
#ifdef USE_MCH_ACCESS
# ifdef UNIX
	(perm & 0222) == 0 ||
# endif
	mch_access((var_na *)fname, W_OK)
#དེ་མིན
	(fd = mch_open((var_na *)fname, O_RDWR | O_EXTRA, 0)) < 0
					? TRUE : (close(fd), FALSE)
#endif
	);
}

#གལ་སྲིད defined(HAVE_FSYNC)
/*
 * Call fsync() with Mac-specific exception.
 * Return fsync() result: zero for success.
 */
    ཧྲིལ
vim_fsync(ཧྲིལ fd)
{
    ཧྲིལ r;

# ifdef MACOS_X
    r = fcntl(fd, F_FULLFSYNC);
    གལ་སྲིད (r != 0)  // F_FULLFSYNC not working or not supported
# endif
	r = fsync(fd);
    ལོག r;
}
#endif

/*
 * Set the name of the current buffer.  Use when the buffer doesn't have a
 * name and a ":r" or ":w" command with a file name is used.
 */
    ཧྲིལ
set_rw_fname(char_u *fname, char_u *sfname)
{
    buf_T	*buf = curbuf;

    // It's like the unnamed buffer is deleted....
    གལ་སྲིད (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFDELETE, NULL, NULL, FALSE, curbuf);
    apply_autocmds(EVENT_BUFWIPEOUT, NULL, NULL, FALSE, curbuf);
#ifdef FEAT_EVAL
    གལ་སྲིད (aborting())	    // autocmds may abort script processing
	ལོག FAIL;
#endif
    གལ་སྲིད (curbuf != buf)
    {
	// We are in another buffer now, don't do the renaming.
	emsg(_(e_autocommands_changed_buffer_or_buffer_name));
	ལོག FAIL;
    }

    གལ་སྲིད (setfname(curbuf, fname, sfname, FALSE) == OK)
	curbuf->b_flags |= BF_NOTEDITED;

    // ....and a new named one is created
    apply_autocmds(EVENT_BUFNEW, NULL, NULL, FALSE, curbuf);
    གལ་སྲིད (curbuf->b_p_bl)
	apply_autocmds(EVENT_BUFADD, NULL, NULL, FALSE, curbuf);
#ifdef FEAT_EVAL
    གལ་སྲིད (aborting())	    // autocmds may abort script processing
	ལོག FAIL;
#endif

    // Do filetype detection now if 'filetype' is empty.
    གལ་སྲིད (*curbuf->b_p_ft == NUL)
    {
	གལ་སྲིད (au_has_group((char_u *)"filetypedetect"))
	    (void)do_doautocmd((char_u *)"filetypedetect BufRead", FALSE, NULL);
	do_modelines(0);
    }

    ལོག OK;
}

/*
 * Put file name into IObuff with quotes.
 */
    void
msg_add_fname(buf_T *buf, char_u *fname)
{
    size_t  IObufflen = 0;

    གལ་སྲིད (fname == NULL)
	fname = (char_u *)"-stdin-";
    IObuff[IObufflen++] = '"';
    IObufflen += home_replace(buf, fname, IObuff + IObufflen, IOSIZE - 4, TRUE);
    STRCPY(IObuff + IObufflen, "\" ");
}

/*
 * Append message for text mode to IObuff.
 * Return TRUE if something appended.
 */
    ཧྲིལ
msg_add_fileformat(ཧྲིལ eol_type)
{
#ifndef USE_CRNL
    གལ་སྲིད (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[dos]") : _("[dos format]"));
	ལོག TRUE;
    }
#endif
    གལ་སྲིད (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[mac]") : _("[mac format]"));
	ལོག TRUE;
    }
#ifdef USE_CRNL
    གལ་སྲིད (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[unix]") : _("[unix format]"));
	ལོག TRUE;
    }
#endif
    ལོག FALSE;
}

/*
 * Append line and character count to IObuff.
 */
    void
msg_add_lines(
    ཧྲིལ	    insert_space,
    long    lnum,
    off_T   nchars)
{
    ཧྲིལ  len = (ཧྲིལ)STRLEN(IObuff);

    གལ་སྲིད (shortmess(SHM_LINES))
	vim_snprintf((var_na *)IObuff + len, IOSIZE - (size_t)len,
		// l10n: L as in line, B as in byte
		_("%s%ldL, %lldB"), insert_space ? " " : "", lnum, (varnumber_T)nchars);
    དེ་མིན
    {
	len += vim_snprintf((var_na *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%s%ld line, ", "%s%ld lines, ", lnum), insert_space ? " " : "", lnum);
	vim_snprintf((var_na *)IObuff + len, IOSIZE - (size_t)len,
		NGETTEXT("%lld byte", "%lld bytes", nchars), (varnumber_T)nchars);
    }
}

/*
 * Append message for missing line separator to IObuff.
 */
    void
msg_add_eol(void)
{
    STRCAT(IObuff, shortmess(SHM_LAST) ? _("[noeol]") : _("[Incomplete last line]"));
}

    ཧྲིལ
time_differs(stat_T *st, long mtime, long mtime_ns UNUSED)
{
    ལོག
#ifdef ST_MTIM_NSEC
	(long)st->ST_MTIM_NSEC != mtime_ns ||
#endif
#གལ་སྲིད defined(__linux__) || defined(MSWIN)
	// On a FAT filesystem, esp. under Linux, there are only 5 bits to store
	// the seconds.  Since the roundoff is done when flushing the inode, the
	// time may change unexpectedly by one second!!!
	(long)st->st_mtime - mtime > 1 || mtime - (long)st->st_mtime > 1
#དེ་མིན
	(long)st->st_mtime != mtime
#endif
	;
}

/*
 * Return TRUE if file encoding "fenc" requires conversion from or to
 * 'encoding'.
 */
    ཧྲིལ
need_conversion(char_u *fenc)
{
    ཧྲིལ		same_encoding;
    ཧྲིལ		enc_flags;
    ཧྲིལ		fenc_flags;

    གལ་སྲིད (*fenc == NUL || STRCMP(p_enc, fenc) == 0)
    {
	same_encoding = TRUE;
	fenc_flags = 0;
    }
    དེ་མིན
    {
	// Ignore difference between "ansi" and "latin1", "ucs-4" and
	// "ucs-4be", etc.
	enc_flags = get_fio_flags(p_enc);
	fenc_flags = get_fio_flags(fenc);
	same_encoding = (enc_flags != 0 && fenc_flags == enc_flags);
    }
    གལ་སྲིད (same_encoding)
    {
	// Specified encoding matches with 'encoding'.  This requires
	// conversion when 'encoding' is Unicode but not UTF-8.
	ལོག enc_unicode != 0;
    }

    // Encodings differ.  However, conversion is not needed when 'enc' is any
    // Unicode encoding and the file is UTF-8.
    ལོག !(enc_utf8 && fenc_flags == FIO_UTF8);
}

/*
 * Check "ptr" for a unicode encoding and return the FIO_ flags needed for the
 * internal conversion.
 * if "ptr" is an empty string, use 'encoding'.
 */
    ཧྲིལ
get_fio_flags(char_u *ptr)
{
    ཧྲིལ		prop;

    གལ་སྲིད (*ptr == NUL)
	ptr = p_enc;

    prop = enc_canon_props(ptr);
    གལ་སྲིད (prop & ENC_UNICODE)
    {
	གལ་སྲིད (prop & ENC_2BYTE)
	{
	    གལ་སྲིད (prop & ENC_ENDIAN_L)
		ལོག FIO_UCS2 | FIO_ENDIAN_L;
	    ལོག FIO_UCS2;
	}
	གལ་སྲིད (prop & ENC_4BYTE)
	{
	    གལ་སྲིད (prop & ENC_ENDIAN_L)
		ལོག FIO_UCS4 | FIO_ENDIAN_L;
	    ལོག FIO_UCS4;
	}
	གལ་སྲིད (prop & ENC_2WORD)
	{
	    གལ་སྲིད (prop & ENC_ENDIAN_L)
		ལོག FIO_UTF16 | FIO_ENDIAN_L;
	    ལོག FIO_UTF16;
	}
	ལོག FIO_UTF8;
    }
    གལ་སྲིད (prop & ENC_LATIN1)
	ལོག FIO_LATIN1;
    // must be ENC_DBCS, requires iconv()
    ལོག 0;
}

#གལ་སྲིད defined(MSWIN)
/*
 * Check "ptr" for a MS-Windows codepage name and return the FIO_ flags needed
 * for the conversion MS-Windows can do for us.  Also accept "utf-8".
 * Used for conversion between 'encoding' and 'fileencoding'.
 */
    ཧྲིལ
get_win_fio_flags(char_u *ptr)
{
    ཧྲིལ		cp;

    // Cannot do this when 'encoding' is not utf-8 and not a codepage.
    གལ་སྲིད (!enc_utf8 && enc_codepage <= 0)
	ལོག 0;

    cp = encname2codepage(ptr);
    གལ་སྲིད (cp == 0)
    {
# ifdef CP_UTF8	// VC 4.1 doesn't define CP_UTF8
	གལ་སྲིད (STRCMP(ptr, "utf-8") == 0)
	    cp = CP_UTF8;
	དེ་མིན
# endif
	    ལོག 0;
    }
    ལོག FIO_PUT_CP(cp) | FIO_CODEPAGE;
}
#endif

#གལ་སྲིད defined(MACOS_CONVERT)
/*
 * Check "ptr" for a Carbon supported encoding and return the FIO_ flags
 * needed for the internal conversion to/from utf-8 or latin1.
 */
    ཧྲིལ
get_mac_fio_flags(char_u *ptr)
{
    གལ་སྲིད ((enc_utf8 || STRCMP(p_enc, "latin1") == 0)
				     && (enc_canon_props(ptr) & ENC_MACROMAN))
	ལོག FIO_MACROMAN;
    ལོག 0;
}
#endif

/*
 * Check for a Unicode BOM (Byte Order Mark) at the start of p[size].
 * "size" must be at least 2.
 * Return the name of the encoding and set "*lenp" to the length.
 * Returns NULL when no BOM found.
 */
    static char_u *
check_for_bom(
    char_u	*p,
    long	size,
    ཧྲིལ		*lenp,
    ཧྲིལ		flags)
{
    var_na	*name = NULL;
    ཧྲིལ		len = 2;

    གལ་སྲིད (p[0] == 0xef && p[1] == 0xbb && size >= 3 && p[2] == 0xbf
	    && (flags == FIO_ALL || flags == FIO_UTF8 || flags == 0))
    {
	name = "utf-8";		// EF BB BF
	len = 3;
    }
    དེ་མིན གལ་སྲིད (p[0] == 0xff && p[1] == 0xfe)
    {
	གལ་སྲིད (size >= 4 && p[2] == 0 && p[3] == 0
	    && (flags == FIO_ALL || flags == (FIO_UCS4 | FIO_ENDIAN_L)))
	{
	    name = "ucs-4le";	// FF FE 00 00
	    len = 4;
	}
	དེ་མིན གལ་སྲིད (flags == (FIO_UCS2 | FIO_ENDIAN_L))
	    name = "ucs-2le";	// FF FE
	དེ་མིན གལ་སྲིད (flags == FIO_ALL || flags == (FIO_UTF16 | FIO_ENDIAN_L))
	    // utf-16le is preferred, it also works for ucs-2le text
	    name = "utf-16le";	// FF FE
    }
    དེ་མིན གལ་སྲིད (p[0] == 0xfe && p[1] == 0xff
	    && (flags == FIO_ALL || flags == FIO_UCS2 || flags == FIO_UTF16))
    {
	// Default to utf-16, it works also for ucs-2 text.
	གལ་སྲིད (flags == FIO_UCS2)
	    name = "ucs-2";	// FE FF
	དེ་མིན
	    name = "utf-16";	// FE FF
    }
    དེ་མིན གལ་སྲིད (size >= 4 && p[0] == 0 && p[1] == 0 && p[2] == 0xfe
	    && p[3] == 0xff && (flags == FIO_ALL || flags == FIO_UCS4))
    {
	name = "ucs-4";		// 00 00 FE FF
	len = 4;
    }

    *lenp = len;
    ལོག (char_u *)name;
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
    གལ་སྲིད (dirname == NULL)
	ལོག full_path;
    གལ་སྲིད (mch_dirname(dirname, MAXPATHL) == OK)
    {
	p = shorten_fname(full_path, dirname);
	གལ་སྲིད (p == NULL || *p == NUL)
	    p = full_path;
    }
    vim_free(dirname);
    ལོག p;
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
    ཧྲིལ		len;
    char_u	*p;

    གལ་སྲིད (full_path == NULL)
	ལོག NULL;
    len = (ཧྲིལ)STRLEN(dir_name);
    གལ་སྲིད (fnamencmp(dir_name, full_path, len) == 0)
    {
	p = full_path + len;
#གལ་སྲིད defined(MSWIN)
	/*
	 * MS-Windows: when a file is in the root directory, dir_name will end
	 * in a slash, since C: by itself does not define a specific dir. In
	 * this case p may already be correct. <negri>
	 */
	གལ་སྲིད (!((len > 2) && (*(p - 2) == ':')))
#endif
	{
	    གལ་སྲིད (vim_ispathsep(*p))
		do
		    ++p;
		སྐབས (vim_ispathsep_nocolon(*p));
#ifndef VMS   // the path separator is always part of the path
	    དེ་མིན
		p = NULL;
#endif
	}
    }
#གལ་སྲིད defined(MSWIN)
    /*
     * When using a file in the current drive, remove the drive name:
     * "A:\dir\file" -> "\dir\file".  This helps when moving a session file on
     * a floppy from "A:\dir" to "B:\dir".
     */
    དེ་མིན གལ་སྲིད (len > 3
	    && TOUPPER_LOC(full_path[0]) == TOUPPER_LOC(dir_name[0])
	    && full_path[1] == ':'
	    && vim_ispathsep(full_path[2]))
	p = full_path + 2;
#endif
    དེ་མིན
	p = NULL;
    ལོག p;
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
    void
shorten_buf_fname(buf_T *buf, char_u *dirname, ཧྲིལ force)
{
    char_u	*p;

    གལ་སྲིད (buf->b_fname != NULL
	    && !bt_nofilename(buf)
	    && !path_with_url(buf->b_fname)
	    && (force
		|| buf->b_sfname == NULL
		|| mch_isFullName(buf->b_sfname)))
    {
	གལ་སྲིད (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	p = shorten_fname(buf->b_ffname, dirname);
	གལ་སྲིད (p != NULL)
	{
	    buf->b_sfname = vim_strsave(p);
	    buf->b_fname = buf->b_sfname;
	}
	གལ་སྲིད (p == NULL || buf->b_fname == NULL)
	    buf->b_fname = buf->b_ffname;
    }
}

/*
 * Shorten filenames for all buffers.
 */
    void
shorten_fnames(ཧྲིལ force)
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
#གལ་སྲིད defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#གལ་སྲིད defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
    popup_update_preview_title();
#endif
}

#གལ་སྲིད (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
/*
 * Shorten all filenames in "fnames[count]" by current directory.
 */
    void
shorten_filenames(char_u **fnames, ཧྲིལ count)
{
    ཧྲིལ		i;
    char_u	dirname[MAXPATHL];
    char_u	*p;

    གལ་སྲིད (fnames == NULL || count < 1)
	ལོག;
    mch_dirname(dirname, parimaa_na(dirname));
    དོན་ལུ (i = 0; i < count; ++i)
    {
	གལ་སྲིད ((p = shorten_fname(fnames[i], dirname)) != NULL)
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
    ཧྲིལ	    prepend_dot)	// may prepend a '.' to file name
{
    ལོག buf_modname((curbuf->b_p_sn || curbuf->b_shortname),
						     fname, ext, prepend_dot);
}

    char_u *
buf_modname(
    ཧྲིལ	    shortname,		// use 8.3 file name
    char_u  *fname,
    char_u  *ext,
    ཧྲིལ	    prepend_dot)	// may prepend a '.' to file name
{
    char_u	*retval;
    char_u	*s;
    char_u	*e;
    char_u	*ptr;
    size_t	ptrlen;
    ཧྲིལ		fnamelen, extlen;

    extlen = (ཧྲིལ)STRLEN(ext);

    /*
     * If there is no file name we must get the name of the current directory
     * (we need the full path in case :cd is used).
     */
    གལ་སྲིད (fname == NULL || *fname == NUL)
    {
	retval = alloc(MAXPATHL + extlen + 3);
	གལ་སྲིད (retval == NULL)
	    ལོག NULL;
	གལ་སྲིད (mch_dirname(retval, MAXPATHL) == FAIL ||
				     (fnamelen = (ཧྲིལ)STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    ལོག NULL;
	}
	གལ་སྲིད (!after_pathsep(retval, retval + fnamelen))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
	prepend_dot = FALSE;	    // nothing to prepend a dot to
    }
    དེ་མིན
    {
	fnamelen = (ཧྲིལ)STRLEN(fname);
	retval = alloc(fnamelen + extlen + 3);
	གལ་སྲིད (retval == NULL)
	    ལོག NULL;
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
    དོན་ལུ (ptr = retval + fnamelen; ptr > retval; MB_PTR_BACK(retval, ptr))
    {
	གལ་སྲིད (*ext == '.' && shortname)
	    གལ་སྲིད (*ptr == '.')	// replace '.' by '_'
		*ptr = '_';
	གལ་སྲིད (vim_ispathsep(*ptr))
	{
	    ++ptr;
	    bha_nga;
	}
    }

    // the file name has at most BASENAMELEN characters.
    ptrlen = (size_t)(fnamelen - (ptr - retval));
    གལ་སྲིད (ptrlen > (unsigned)BASENAMELEN)
    {
	ptrlen = BASENAMELEN;
	ptr[ptrlen] = NUL;
    }

    s = ptr + ptrlen;

    /*
     * For 8.3 file names we may have to reduce the length.
     */
    གལ་སྲིད (shortname)
    {
	/*
	 * If there is no file name, or the file name ends in '/', and the
	 * extension starts with '.', put a '_' before the dot, because just
	 * ".ext" is invalid.
	 */
	གལ་སྲིད (fname == NULL || *fname == NUL
				   || vim_ispathsep(fname[STRLEN(fname) - 1]))
	{
	    གལ་སྲིད (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	དེ་མིན གལ་སྲིད (*ext == '.')
	{
	    གལ་སྲིད ((size_t)(s - ptr) > (size_t)8)
	    {
		s = ptr + 8;
		*s = '\0';
	    }
	}
	/*
	 * If the extension doesn't start with '.', and the file name
	 * doesn't have an extension yet, append a '.'
	 */
	དེ་མིན གལ་སྲིད ((e = vim_strchr(ptr, '.')) == NULL)
	    *s++ = '.';
	/*
	 * If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be truncated
	 */
	དེ་མིན གལ་སྲིད ((ཧྲིལ)(ptrlen - (e - retval)) + extlen > 4)
	    s = e + 4 - extlen;
    }
#ifdef MSWIN
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    དེ་མིན གལ་སྲིད ((fname == NULL || *fname == NUL) && *ext == '.')
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
    གལ་སྲིད (prepend_dot && !shortname && *(e = gettail(retval)) != '.')
    {
	mch_memmove(e + 1, e, (size_t)(((fnamelen + extlen) - (e - retval)) + 1));	// +1 for NUL
	*e = '.';
    }

    /*
     * Check that, after appending the extension, the file name is really
     * different.
     */
    གལ་སྲིད (fname != NULL && STRCMP(fname, retval) == 0)
    {
	// we search for a character that can be replaced by '_'
	སྐབས (--s >= ptr)
	{
	    གལ་སྲིད (*s != '_')
	    {
		*s = '_';
		bha_nga;
	    }
	}
	གལ་སྲིད (s < ptr)	// fname was "________.<ext>", how tricky!
	    *ptr = 'v';
    }
    ལོག retval;
}

/*
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE for end-of-file.
 * If the line is truncated then buf[size - 2] will not be NUL.
 */
    ཧྲིལ
vim_fgets(char_u *buf, ཧྲིལ size, FILE *fp)
{
    var_na	*eof;
#define FGETS_SIZE 200
    var_na	tbuf[FGETS_SIZE];

    // safety check
    གལ་སྲིད (size < 2)
    {
	གལ་སྲིད (size == 1)
	    buf[0] = NUL;
	ལོག TRUE;
    }

    buf[size - 2] = NUL;
    eof = fgets((var_na *)buf, size, fp);
    གལ་སྲིད (buf[size - 2] != NUL && buf[size - 2] != '\n')
    {
	buf[size - 1] = NUL;	    // Truncate the line

	// Now throw away the rest of the line:
	do
	{
	    tbuf[FGETS_SIZE - 2] = NUL;
	    vim_ignoredp = fgets((var_na *)tbuf, FGETS_SIZE, fp);
	} སྐབས (tbuf[FGETS_SIZE - 2] != NUL && tbuf[FGETS_SIZE - 2] != '\n');
    }
    ལོག (eof == NULL);
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 * Return -1 for failure, 0 for success.
 */
    ཧྲིལ
vim_rename(char_u *from, char_u *to)
{
    ཧྲིལ		n;
    ཧྲིལ		ret;
#ifdef AMIGA
    BPTR	flock;
#endif
    stat_T	st;
    ཧྲིལ		use_tmp_file = FALSE;

    /*
     * When the names are identical, there is nothing to do.  When they refer
     * to the same file (ignoring case and slash/backslash differences) but
     * the file name differs we need to go through a temp file.
     */
    གལ་སྲིད (fnamecmp(from, to) == 0)
    {
	གལ་སྲིད (p_fic && STRCMP(gettail(from), gettail(to)) != 0)
	    use_tmp_file = TRUE;
	དེ་མིན
	    ལོག 0;
    }

    /*
     * Fail if the "from" file doesn't exist.  Avoids that "to" is deleted.
     */
    གལ་སྲིད (mch_stat((var_na *)from, &st) < 0)
	ལོག -1;

#ifdef UNIX
    {
	stat_T	st_to;

	// It's possible for the source and destination to be the same file.
	// This happens when "from" and "to" differ in case and are on a FAT32
	// filesystem.  In that case go through a temp file name.
	གལ་སྲིད (mch_stat((var_na *)to, &st_to) >= 0
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
	གལ་སྲིད (win32_fileinfo(from, &info1) == FILEINFO_OK
		&& win32_fileinfo(to, &info2) == FILEINFO_OK
		&& info1.dwVolumeSerialNumber == info2.dwVolumeSerialNumber
		&& info1.nFileIndexHigh == info2.nFileIndexHigh
		&& info1.nFileIndexLow == info2.nFileIndexLow)
	    use_tmp_file = TRUE;
    }
#endif

    གལ་སྲིད (use_tmp_file)
    {
	var_na	tempname[MAXPATHL + 1];

	/*
	 * Find a name that doesn't exist and is in the same directory.
	 * Rename "from" to "tempname" and then rename "tempname" to "to".
	 */
	གལ་སྲིད (STRLEN(from) >= MAXPATHL - 5)
	    ལོག -1;
	STRCPY(tempname, from);
	དོན་ལུ (n = 123; n < 99999; ++n)
	{
	    sprintf((var_na *)gettail((char_u *)tempname), "%d", n);
	    གལ་སྲིད (mch_stat(tempname, &st) < 0)
	    {
		གལ་སྲིད (mch_rename((var_na *)from, tempname) == 0)
		{
		    གལ་སྲིད (mch_rename(tempname, (var_na *)to) == 0)
			ལོག 0;
		    // Strange, the second step failed.  Try moving the
		    // file back and return failure.
		    (void)mch_rename(tempname, (var_na *)from);
		    ལོག -1;
		}
		// If it fails for one temp name it will most likely fail
		// for any temp name, give up.
		ལོག -1;
	    }
	}
	ལོག -1;
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
    གལ་སྲིད (flock)
	UnLock(flock);
#endif

    /*
     * First try a normal rename, return if it works.
     */
    གལ་སྲིད (mch_rename((var_na *)from, (var_na *)to) == 0)
	ལོག 0;

    /*
     * Rename() failed, try copying the file.
     */
    ret = vim_copyfile(from, to);
    གལ་སྲིད (ret != OK)
	ལོག -1;

    /*
     * Remove copied original file
     */
    གལ་སྲིད (mch_stat((var_na *)from, &st) >= 0)
	mch_remove(from);

    ལོག 0;
}


/*
 * Create the new file with same permissions as the original.
 * Return FAIL for failure, OK for success.
 */
    ཧྲིལ
vim_copyfile(char_u *from, char_u *to)
{
    ཧྲིལ		fd_in;
    ཧྲིལ		fd_out;
    ཧྲིལ		n;
    var_na	*errmsg = NULL;
    var_na	*buffer;
    long	perm;
#ifdef HAVE_ACL
    vim_acl_T	acl;		// ACL from original file
#endif

#ifdef HAVE_READLINK
    ཧྲིལ		ret;
    ཧྲིལ		len;
    stat_T	st;
    var_na	linkbuf[MAXPATHL + 1];

    ret = mch_lstat((var_na *)from, &st);
    གལ་སྲིད (ret >= 0 && S_ISLNK(st.st_mode))
    {
	ret = -1;

	len = readlink((var_na *)from, linkbuf, MAXPATHL);
	གལ་སྲིད (len > 0)
	{
	    linkbuf[len] = NUL;

	    // Create link
	    ret = symlink(linkbuf, (var_na *)to);
	}

	ལོག ret == 0 ? OK : FAIL;
    }
#endif

    perm = mch_getperm(from);
#ifdef HAVE_ACL
    // For systems that support ACL: get the ACL from the original file.
    acl = mch_get_acl(from);
#endif
    fd_in = mch_open((var_na *)from, O_RDONLY|O_EXTRA, 0);
    གལ་སྲིད (fd_in == -1)
    {
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	ལོག FAIL;
    }

    // Create the new file with same permissions as the original.
    fd_out = mch_open((var_na *)to,
		       O_CREAT|O_EXCL|O_WRONLY|O_EXTRA|O_NOFOLLOW, (ཧྲིལ)perm);
    གལ་སྲིད (fd_out == -1)
    {
	close(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	ལོག FAIL;
    }

    buffer = alloc(WRITEBUFSIZE);
    གལ་སྲིད (buffer == NULL)
    {
	close(fd_out);
	close(fd_in);
#ifdef HAVE_ACL
	mch_free_acl(acl);
#endif
	ལོག FAIL;
    }

    སྐབས ((n = read_eintr(fd_in, buffer, WRITEBUFSIZE)) > 0)
	གལ་སྲིད (write_eintr(fd_out, buffer, n) != n)
	{
	    errmsg = _(e_error_writing_to_str);
	    bha_nga;
	}

    vim_free(buffer);
    close(fd_in);
    གལ་སྲིད (close(fd_out) < 0)
	errmsg = _(e_error_closing_str);
    གལ་སྲིད (n < 0)
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
#གལ་སྲིད defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    mch_copy_sec(from, to);
#endif
    གལ་སྲིད (errmsg != NULL)
    {
	semsg(errmsg, to);
	ལོག FAIL;
    }
    ལོག OK;
}

static ཧྲིལ already_warned = FALSE;

/*
 * Check if any not hidden buffer has been changed.
 * Postpone the check if there are characters in the stuff buffer, a global
 * command is being executed, a mapping is being executed or an autocommand is
 * busy.
 * Returns TRUE if some message was written (screen should be redrawn and
 * cursor positioned).
 */
    ཧྲིལ
check_timestamps(
    ཧྲིལ		focus)		// called for GUI focus event
{
    buf_T	*buf;
    ཧྲིལ		didit = 0;
    ཧྲིལ		n;

    // Don't check timestamps while system() or another low-level function may
    // cause us to lose and gain focus.
    གལ་སྲིད (no_check_timestamps > 0)
	ལོག FALSE;

    // Avoid doing a check twice.  The OK/Reload dialog can cause a focus
    // event and we would keep on checking if the file is steadily growing.
    // Do check again after typing something.
    གལ་སྲིད (focus && did_check_timestamps)
    {
	need_check_timestamps = TRUE;
	ལོག FALSE;
    }

    གལ་སྲིད (!stuff_empty() || global_busy || !typebuf_typed()
			|| autocmd_busy || curbuf_lock > 0 || allbuf_lock > 0)
	need_check_timestamps = TRUE;		// check later
    དེ་མིན
    {
	++no_wait_return;
	did_check_timestamps = TRUE;
	already_warned = FALSE;
	FOR_ALL_BUFFERS(buf)
	{
	    // Only check buffers in a window.
	    གལ་སྲིད (buf->b_nwindows > 0)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		n = buf_check_timestamp(buf, focus);
		གལ་སྲིད (didit < n)
		    didit = n;
		གལ་སྲིད (n > 0 && !bufref_valid(&bufref))
		{
		    // Autocommands have removed the buffer, start at the
		    // first one again.
		    buf = firstbuf;
		    continue;
		}
	    }
	}
	--no_wait_return;
	need_check_timestamps = FALSE;
	གལ་སྲིད (need_wait_return && didit == 2)
	{
	    // make sure msg isn't overwritten
	    msg_puts("\n");
	    out_flush();
	}
    }
    ལོག didit;
}

/*
 * Move all the lines from buffer "frombuf" to buffer "tobuf".
 * Return OK or FAIL.  When FAIL "tobuf" is incomplete and/or "frombuf" is not
 * empty.
 */
    static ཧྲིལ
move_lines(buf_T *frombuf, buf_T *tobuf)
{
    buf_T	*tbuf = curbuf;
    ཧྲིལ		retval = OK;
    linenr_T	lnum;
    char_u	*p;

    // Copy the lines in "frombuf" to "tobuf".
    curbuf = tobuf;
    དོན་ལུ (lnum = 1; lnum <= frombuf->b_ml.ml_line_count; ++lnum)
    {
	p = vim_strnsave(ml_get_buf(frombuf, lnum, FALSE), ml_get_buf_len(frombuf, lnum));
	གལ་སྲིད (p == NULL || ml_append(lnum - 1, p, 0, FALSE) == FAIL)
	{
	    vim_free(p);
	    retval = FAIL;
	    bha_nga;
	}
	vim_free(p);
    }

    // Delete all the lines in "frombuf".
    གལ་སྲིད (retval != FAIL)
    {
	curbuf = frombuf;
	དོན་ལུ (lnum = curbuf->b_ml.ml_line_count; lnum > 0; --lnum)
	    གལ་སྲིད (ml_delete(lnum) == FAIL)
	    {
		// Oops!  We could try putting back the saved lines, but that
		// might fail again...
		retval = FAIL;
		bha_nga;
	    }
    }

    curbuf = tbuf;
    ལོག retval;
}

/*
 * Check if buffer "buf" has been changed.
 * Also check if the file for a new buffer unexpectedly appeared.
 * return 1 if a changed buffer was found.
 * return 2 if a message has been displayed.
 * return 0 otherwise.
 */
    ཧྲིལ
buf_check_timestamp(
    buf_T	*buf,
    ཧྲིལ		focus UNUSED)	// called for GUI focus event
{
    stat_T	st;
    ཧྲིལ		stat_res;
    ཧྲིལ		retval = 0;
    var_na	*mesg = NULL;
    var_na	*mesg2 = "";
    ཧྲིལ		helpmesg = FALSE;
    ga_nanaa {
	RELOAD_NONE,
	RELOAD_NORMAL,
	RELOAD_DETECT
    }		reload = RELOAD_NONE;
#གལ་སྲིད defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
    ཧྲིལ		can_reload = FALSE;
#endif
    off_T	orig_size = buf->b_orig_size;
    ཧྲིལ		orig_mode = buf->b_orig_mode;
#ifdef FEAT_GUI
    ཧྲིལ		save_mouse_correct = need_mouse_correct;
#endif
    static ཧྲིལ	busy = FALSE;
    ཧྲིལ		n;
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    // If there is no file name, the buffer is not loaded, 'buftype' is
    // set, we are in the middle of a save or being called recursively: ignore
    // this buffer.
    གལ་སྲིད (buf->b_ffname == NULL
	    || buf->b_ml.ml_mfp == NULL
	    || !bt_normal(buf)
	    || buf->b_saving
	    || busy
#ifdef FEAT_NETBEANS_INTG
	    || isNetbeansBuffer(buf)
#endif
#ifdef FEAT_TERMINAL
	    || buf->b_term != NULL
#endif
	    )
	ལོག 0;

    གལ་སྲིད (       !(buf->b_flags & BF_NOTEDITED)
	    && buf->b_mtime != 0
	    && ((stat_res = mch_stat((var_na *)buf->b_ffname, &st)) < 0
		|| time_differs(&st, buf->b_mtime, buf->b_mtime_ns)
		|| st.st_size != buf->b_orig_size
#ifdef HAVE_ST_MODE
		|| (ཧྲིལ)st.st_mode != buf->b_orig_mode
#དེ་མིན
		|| mch_getperm(buf->b_ffname) != buf->b_orig_mode
#endif
		))
    {
	long prev_b_mtime = buf->b_mtime;

	retval = 1;

	// set b_mtime to stop further warnings (e.g., when executing
	// FileChangedShell autocmd)
	གལ་སྲིད (stat_res < 0)
	{
	    // Check the file again later to see if it re-appears.
	    buf->b_mtime = -1;
	    buf->b_orig_size = 0;
	    buf->b_orig_mode = 0;
	}
	དེ་མིན
	    buf_store_time(buf, &st, buf->b_ffname);

	// Don't do anything for a directory.  Might contain the file
	// explorer.
	གལ་སྲིད (mch_isdir(buf->b_fname))
	    ;

	/*
	 * If 'autoread' is set, the buffer has no changes and the file still
	 * exists, reload the buffer.  Use the buffer-local option value if it
	 * was set, the global option value otherwise.
	 */
	དེ་མིན གལ་སྲིད ((buf->b_p_ar >= 0 ? buf->b_p_ar : p_ar)
				       && !bufIsChanged(buf) && stat_res >= 0)
	    reload = RELOAD_NORMAL;
	དེ་མིན
	{
	    var_na    *reason;
#ifdef FEAT_EVAL
	    size_t  reasonlen;
#endif

	    གལ་སྲིད (stat_res < 0)
	    {
		reason = "deleted";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("deleted");
#endif
	    }
	    དེ་མིན གལ་སྲིད (bufIsChanged(buf))
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
	    དེ་མིན གལ་སྲིད (orig_size != buf->b_orig_size || buf_contents_changed(buf))
	    {
		reason = "changed";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("changed");
#endif
	    }
	    དེ་མིན གལ་སྲིད (orig_mode != buf->b_orig_mode)
	    {
		reason = "mode";
#ifdef FEAT_EVAL
		reasonlen = STRLEN_LITERAL("mode");
#endif
	    }
	    དེ་མིན
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
	    set_vim_var_string(VV_FCS_REASON, (char_u *)reason, (ཧྲིལ)reasonlen);
	    set_vim_var_string(VV_FCS_CHOICE, (char_u *)"", 0);
#endif
	    ++allbuf_lock;
	    n = apply_autocmds(EVENT_FILECHANGEDSHELL,
				      buf->b_fname, buf->b_fname, FALSE, buf);
	    --allbuf_lock;
	    busy = FALSE;
	    གལ་སྲིད (n)
	    {
		གལ་སྲིད (!bufref_valid(&bufref))
		    emsg(_(e_filechangedshell_autocommand_deleted_buffer));
#ifdef FEAT_EVAL
		char_u	*s = get_vim_var_str(VV_FCS_CHOICE);
		གལ་སྲིད (STRCMP(s, "reload") == 0 && *reason != 'd')
		    reload = RELOAD_NORMAL;
		དེ་མིན གལ་སྲིད (STRCMP(s, "edit") == 0)
		    reload = RELOAD_DETECT;
		དེ་མིན གལ་སྲིད (STRCMP(s, "ask") == 0)
		    n = FALSE;
		དེ་མིན
#endif
		    ལོག 2;
	    }
	    གལ་སྲིད (!n)
	    {
		གལ་སྲིད (*reason == 'd')
		{
		    // Only give the message once.
		    གལ་སྲིད (prev_b_mtime != -1)
			mesg = _(e_file_str_no_longer_available);
		}
		དེ་མིན
		{
		    helpmesg = TRUE;
#གལ་སྲིད defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		    can_reload = TRUE;
#endif
		    གལ་སྲིད (reason[2] == 'n')
		    {
			mesg = _("W12: Warning: File \"%s\" has changed and the buffer was changed in Vim as well");
			mesg2 = _("See \":help W12\" for more info.");
		    }
		    དེ་མིན གལ་སྲིད (reason[1] == 'h')
		    {
			mesg = _("W11: Warning: File \"%s\" has changed since editing started");
			mesg2 = _("See \":help W11\" for more info.");
		    }
		    དེ་མིན གལ་སྲིད (*reason == 'm')
		    {
			mesg = _("W16: Warning: Mode of file \"%s\" has changed since editing started");
			mesg2 = _("See \":help W16\" for more info.");
		    }
		    དེ་མིན
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
    དེ་མིན གལ་སྲིད ((buf->b_flags & BF_NEW) && !(buf->b_flags & BF_NEW_W)
						&& vim_fexists(buf->b_ffname))
    {
	retval = 1;
	mesg = _("W13: Warning: File \"%s\" has been created after editing started");
	buf->b_flags |= BF_NEW_W;
#གལ་སྲིད defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
	can_reload = TRUE;
#endif
    }

    གལ་སྲིད (mesg != NULL)
    {
	char_u *path;

	path = home_replace_save(buf, buf->b_fname);
	གལ་སྲིད (path != NULL)
	{
	    size_t  tbufsize;
	    var_na    *tbuf;

	    གལ་སྲིད (!helpmesg)
		mesg2 = "";
	    tbufsize = STRLEN(mesg) + STRLEN(path) + 2 + STRLEN(mesg2) + 1; // +2 for either '\n' or "; "
									    // and +1 for NUL
	    tbuf = alloc(tbufsize);
	    གལ་སྲིད (tbuf != NULL)
	    {
		ཧྲིལ tbuflen;

		tbuflen = vim_snprintf(tbuf, tbufsize, mesg, path);
#ifdef FEAT_EVAL
		// Set warningmsg here, before the unimportant and output-specific
		// mesg2 has been appended.
		set_vim_var_string(VV_WARNINGMSG, (char_u *)tbuf, tbuflen);
#endif
#གལ་སྲིད defined(FEAT_CON_DIALOG) || defined(FEAT_GUI_DIALOG)
		གལ་སྲིད (can_reload)
		{
		    གལ་སྲིད (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "\n%s", mesg2);
		    switch (do_dialog(VIM_WARNING, (char_u *)_("Warning"),
			    (char_u *)tbuf,
			    (char_u *)_("&OK\n&Load File\nLoad File &and Options"),
			    1, NULL, TRUE))
		    {
			pak_sa 2:
			    reload = RELOAD_NORMAL;
			    bha_nga;
			pak_sa 3:
			    reload = RELOAD_DETECT;
			    bha_nga;
		    }
		}
		དེ་མིན
#endif
		གལ་སྲིད (State > MODE_NORMAL_BUSY || (State & MODE_CMDLINE)
								 || already_warned)
		{
		    གལ་སྲིད (*mesg2 != NUL)
			vim_snprintf(tbuf + tbuflen, tbufsize - tbuflen, "; %s", mesg2);
		    emsg(tbuf);
		    retval = 2;
		}
		དེ་མིན
		{
		    གལ་སྲིད (!autocmd_busy)
		    {
			msg_start();
			msg_puts_attr(tbuf, HL_ATTR(HLF_E) + MSG_HIST);
			གལ་སྲིད (*mesg2 != NUL)
			    msg_puts_attr(mesg2, HL_ATTR(HLF_W) + MSG_HIST);
			msg_clr_eos();
			(void)msg_end();
			གལ་སྲིད (emsg_silent == 0 && !in_assert_fails)
			{
			    out_flush();
#ifdef FEAT_GUI
			    གལ་སྲིད (!focus)
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

    གལ་སྲིད (reload != RELOAD_NONE)
    {
	// Reload the buffer.
	buf_reload(buf, orig_mode, reload == RELOAD_DETECT);
#ifdef FEAT_PERSISTENT_UNDO
	གལ་སྲིད (bufref_valid(&bufref) && buf->b_p_udf && buf->b_ffname != NULL)
	{
	    char_u	    hash[UNDO_HASH_SIZE];
	    buf_T	    *save_curbuf = curbuf;

	    // Any existing undo file is unusable, write it now.
	    curbuf = buf;
	    u_compute_hash(hash);
	    u_write_undo(NULL, FALSE, buf, hash);
	    curbuf = save_curbuf;
	}
#endif
    }

    // Trigger FileChangedShell when the file was changed in any way.
    གལ་སྲིད (bufref_valid(&bufref) && retval != 0)
	(void)apply_autocmds(EVENT_FILECHANGEDSHELLPOST,
				      buf->b_fname, buf->b_fname, FALSE, buf);
#ifdef FEAT_GUI
    // restore this in case an autocommand has set it; it would break
    // 'mousefocus'
    need_mouse_correct = save_mouse_correct;
#endif

    ལོག retval;
}

/*
 * Reload a buffer that is already loaded.
 * Used when the file was changed outside of Vim.
 * "orig_mode" is buf->b_orig_mode before the need for reloading was detected.
 * buf->b_orig_mode may have been reset already.
 */
    void
buf_reload(buf_T *buf, ཧྲིལ orig_mode, ཧྲིལ reload_options)
{
    exarg_T	ea;
    pos_T	old_cursor;
    linenr_T	old_topline;
    ཧྲིལ		old_ro = buf->b_p_ro;
    buf_T	*savebuf;
    bufref_T	bufref;
    ཧྲིལ		saved = OK;
    aco_save_T	aco;
    ཧྲིལ		flags = READ_NEW;
    ཧྲིལ		prepped = OK;

    // Set curwin/curbuf for "buf" and save some things.
    aucmd_prepbuf(&aco, buf);
    གལ་སྲིད (curbuf != buf)
    {
	// Failed to find a window for "buf", it is dangerous to continue,
	// better bail out.
	ལོག;
    }

    // Unless reload_options is set, we only want to read the text from the
    // file, not reset the syntax highlighting, clear marks, diff status, etc.
    // Force the fileformat and encoding to be the same.
    གལ་སྲིད (reload_options)
	CLEAR_FIELD(ea);
    དེ་མིན
	prepped = prep_exarg(&ea, buf);

    གལ་སྲིད (prepped == OK)
    {
	old_cursor = curwin->w_cursor;
	old_topline = curwin->w_topline;

	གལ་སྲིད (p_ur < 0 || curbuf->b_ml.ml_line_count <= p_ur)
	{
	    // Save all the text, so that the reload can be undone.
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    saved = u_savecommon(0, curbuf->b_ml.ml_line_count + 1, 0, TRUE);
	    flags |= READ_KEEP_UNDO;
	}

	/*
	 * To behave like when a new file is edited (matters for
	 * BufReadPost autocommands) we first need to delete the current
	 * buffer contents.  But if reading the file fails we should keep
	 * the old contents.  Can't use memory only, the file might be
	 * too big.  Use a hidden buffer to move the buffer contents to.
	 */
	གལ་སྲིད (BUFEMPTY() || saved == FAIL)
	    savebuf = NULL;
	དེ་མིན
	{
	    // Allocate a buffer without putting it in the buffer list.
	    savebuf = buflist_new(NULL, NULL, (linenr_T)1, BLN_DUMMY);
	    set_bufref(&bufref, savebuf);
	    གལ་སྲིད (savebuf != NULL && buf == curbuf)
	    {
		// Open the memline.
		curbuf = savebuf;
		curwin->w_buffer = savebuf;
		saved = ml_open(curbuf);
		curbuf = buf;
		curwin->w_buffer = buf;
	    }
	    གལ་སྲིད (savebuf == NULL || saved == FAIL || buf != curbuf
				      || move_lines(buf, savebuf) == FAIL)
	    {
		semsg(_(e_could_not_prepare_for_reloading_str), buf->b_fname);
		saved = FAIL;
	    }
	}

	གལ་སྲིད (saved == OK)
	{
	    ཧྲིལ old_msg_silent = msg_silent;

	    curbuf->b_flags |= BF_CHECK_RO;	// check for RO again
	    curbuf->b_keep_filetype = true;	// don't detect 'filetype'

	    གལ་སྲིད (shortmess(SHM_FILEINFO))
		msg_silent = 1;

	    གལ་སྲིད (readfile(buf->b_ffname, buf->b_fname, (linenr_T)0,
			(linenr_T)0,
			(linenr_T)MAXLNUM, &ea, flags) != OK)
	    {
#གལ་སྲིད defined(FEAT_EVAL)
		གལ་སྲིད (!aborting())
#endif
		    semsg(_(e_could_not_reload_str), buf->b_fname);
		གལ་སྲིད (savebuf != NULL && bufref_valid(&bufref) && buf == curbuf)
		{
		    // Put the text back from the save buffer.  First
		    // delete any lines that readfile() added.
		    སྐབས (!BUFEMPTY())
			གལ་སྲིད (ml_delete(buf->b_ml.ml_line_count) == FAIL)
			    bha_nga;
		    (void)move_lines(savebuf, buf);
		}
	    }
	    དེ་མིན གལ་སྲིད (buf == curbuf)  // "buf" still valid
	    {
		// Mark the buffer as unmodified and free undo info.
		unchanged(buf, TRUE, TRUE);
		གལ་སྲིད ((flags & READ_KEEP_UNDO) == 0)
		    u_clearallandblockfree(buf);
		དེ་མིན
		{
		    // Mark all undo states as changed.
		    u_unchanged(curbuf);
		}
	    }

	    msg_silent = old_msg_silent;
	}
	vim_free(ea.cmd);

	གལ་སྲིད (savebuf != NULL && bufref_valid(&bufref))
	    wipe_buffer(savebuf, FALSE);

#ifdef FEAT_DIFF
	// Invalidate diff info if necessary.
	diff_invalidate(curbuf);
#endif

	// Restore the topline and cursor position and check it (lines may
	// have been removed).
	གལ་སྲིད (old_topline > curbuf->b_ml.ml_line_count)
	    curwin->w_topline = curbuf->b_ml.ml_line_count;
	དེ་མིན
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
		གལ་སྲིད (wp->w_buffer == curwin->w_buffer
			&& !foldmethodIsManual(wp))
		    foldUpdateAll(wp);
	}
#endif
	// If the mode didn't change and 'readonly' was set, keep the old
	// value; the user probably used the ":view" command.  But don't
	// reset it, might have had a read error.
	གལ་སྲིད (orig_mode == curbuf->b_orig_mode)
	    curbuf->b_p_ro |= old_ro;

	// Modelines must override settings done by autocommands.
	do_modelines(0);
    }

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);
    // Careful: autocommands may have made "buf" invalid!
}

    void
buf_store_time(buf_T *buf, stat_T *st, char_u *fname UNUSED)
{
    buf->b_mtime = (long)st->st_mtime;
#ifdef ST_MTIM_NSEC
    buf->b_mtime_ns = (long)st->ST_MTIM_NSEC;
#དེ་མིན
    buf->b_mtime_ns = 0;
#endif
    buf->b_orig_size = st->st_size;
#ifdef HAVE_ST_MODE
    buf->b_orig_mode = (ཧྲིལ)st->st_mode;
#དེ་མིན
    buf->b_orig_mode = mch_getperm(fname);
#endif
}

/*
 * Adjust the line with missing eol, used for the next write.
 * Used for do_filter(), when the input lines for the filter are deleted.
 */
    void
write_lnum_adjust(linenr_T offset)
{
    གལ་སྲིད (curbuf->b_no_eol_lnum != 0)	// only if there is a missing eol
	curbuf->b_no_eol_lnum += offset;
}

// Subfuncions for readdirex()
#ifdef FEAT_EVAL
# ifdef MSWIN
    static char_u *
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
    གལ་སྲིད (wp != NULL)
    {
	གལ་སྲིད (_wcsicmp(wp, L".exe") == 0 ||
		_wcsicmp(wp, L".com") == 0 ||
		_wcsicmp(wp, L".cmd") == 0 ||
		_wcsicmp(wp, L".bat") == 0)
	    st_mode |= _S_IEXEC;
    }

    // Copy user bits to group/other.
    st_mode |= (st_mode & 0700) >> 3;
    st_mode |= (st_mode & 0700) >> 6;

    st.st_mode = st_mode;
    ལོག getfpermst(&st, perm);
}

    static void
getftypewfd(WIN32_FIND_DATAW *wfd, string_T *ret)
{
    DWORD flag = wfd->dwFileAttributes;
    DWORD tag = wfd->dwReserved0;

    གལ་སྲིད (flag & FILE_ATTRIBUTE_REPARSE_POINT)
    {
	གལ་སྲིད (tag == IO_REPARSE_TAG_MOUNT_POINT)
	{
	    ret->string = (char_u *)"junction";
	    ret->length = STRLEN_LITERAL("junction");
	    ལོག;
	}
	གལ་སྲིད (tag == IO_REPARSE_TAG_SYMLINK)
	{
	    གལ་སྲིད (flag & FILE_ATTRIBUTE_DIRECTORY)
	    {
		ret->string = (char_u *)"linkd";
		ret->length = STRLEN_LITERAL("linkd");
		ལོག;
	    }

	    ret->string = (char_u *)"link";
	    ret->length = STRLEN_LITERAL("link");
	    ལོག;
	}

	// unknown reparse point type
	ret->string = (char_u *)"reparse";
	ret->length = STRLEN_LITERAL("reparse");
	ལོག;
    }

    གལ་སྲིད (flag & FILE_ATTRIBUTE_DIRECTORY)
    {
	ret->string = (char_u *)"dir";
	ret->length = STRLEN_LITERAL("dir");
	ལོག;
    }

    ret->string = (char_u *)"file";
    ret->length = STRLEN_LITERAL("file");
}

    static dict_T *
create_readdirex_item(WIN32_FIND_DATAW *wfd)
{
    dict_T	*item;
    char_u	*p;
    varnumber_T	size, time;
    char_u	permbuf[] = "---------";
    string_T	s;

    item = dict_alloc();
    གལ་སྲིད (item == NULL)
	ལོག NULL;
    item->dv_refcount++;

    p = utf16_to_enc(wfd->cFileName, NULL);
    གལ་སྲིད (p == NULL)
	goto theend;
    གལ་སྲིད (dict_add_string(item, "name", p) == FAIL)
    {
	vim_free(p);
	goto theend;
    }
    vim_free(p);

    size = (((varnumber_T)wfd->nFileSizeHigh) << 32) | wfd->nFileSizeLow;
    གལ་སྲིད (dict_add_number(item, "size", size) == FAIL)
	goto theend;

    // Convert FILETIME to unix time.
    time = (((((varnumber_T)wfd->ftLastWriteTime.dwHighDateTime) << 32) |
		wfd->ftLastWriteTime.dwLowDateTime)
	    - 116444736000000000) / 10000000;
    གལ་སྲིད (dict_add_number(item, "time", time) == FAIL)
	goto theend;

    getftypewfd(wfd, &s);
    གལ་སྲིད (dict_add_string_len(item, "type", s.string, (ཧྲིལ)s.length) == FAIL)
	goto theend;
    གལ་སྲིད (dict_add_string(item, "perm", getfpermwfd(wfd, permbuf)) == FAIL)
	goto theend;

    གལ་སྲིད (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	goto theend;
    གལ་སྲིད (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	goto theend;

    ལོག item;

theend:
    dict_unref(item);
    ལོག NULL;
}
# དེ་མིན
    static dict_T *
create_readdirex_item(char_u *path, char_u *name)
{
    dict_T	*item;
    var_na	*p;
    size_t	pathlen, len;
    size_t	namelen;
    stat_T	st;
    ཧྲིལ		ret, link = FALSE;
    varnumber_T	size;
    char_u	permbuf[] = "---------";
    string_T	q = {NULL, 0};
    sa_nracanaa passwd *pw;
    sa_nracanaa group  *gr;

    item = dict_alloc();
    གལ་སྲིད (item == NULL)
	ལོག NULL;
    item->dv_refcount++;

    pathlen = STRLEN(path);
    namelen = STRLEN(name);
    len = pathlen + 1 + namelen + 1;
    p = alloc(len);
    གལ་སྲིད (p == NULL)
	goto theend;
    གལ་སྲིད (pathlen > 0 && path[pathlen - 1] == '/')
	vim_snprintf(p, len, "%s%s", path, name);
    དེ་མིན
	vim_snprintf(p, len, "%s/%s", path, name);
    ret = mch_lstat(p, &st);
    གལ་སྲིད (ret >= 0 && S_ISLNK(st.st_mode))
    {
	link = TRUE;
	ret = mch_stat(p, &st);
	གལ་སྲིད (ret < 0)
	    STR_LITERAL_SET(q, "link");
    }
    vim_free(p);

    གལ་སྲིད (dict_add_string_len(item, "name", name, (ཧྲིལ)namelen) == FAIL)
	goto theend;

    གལ་སྲིད (ret >= 0)
    {
	size = (varnumber_T)st.st_size;
	གལ་སྲིད (S_ISDIR(st.st_mode))
	    size = 0;
	// non-perfect check for overflow
	དེ་མིན གལ་སྲིད ((off_T)size != (off_T)st.st_size)
	    size = -2;
	གལ་སྲིད (dict_add_number(item, "size", size) == FAIL)
	    goto theend;
	གལ་སྲིད (dict_add_number(item, "time", (varnumber_T)st.st_mtime) == FAIL)
	    goto theend;

	གལ་སྲིད (link)
	{
	    གལ་སྲིད (S_ISDIR(st.st_mode))
		STR_LITERAL_SET(q, "linkd");
	    དེ་མིན
		STR_LITERAL_SET(q, "link");
	}
	དེ་མིན
	{
	    q.string = getftypest(&st);
	    q.length = STRLEN(q.string);
	}
	གལ་སྲིད (dict_add_string_len(item, "type", q.string, (ཧྲིལ)q.length) == FAIL)
	    goto theend;
	གལ་སྲིད (dict_add_string(item, "perm", getfpermst(&st, permbuf)) == FAIL)
	    goto theend;

	pw = getpwuid(st.st_uid);
	གལ་སྲིད (pw == NULL)
	    STR_LITERAL_SET(q, "");
	དེ་མིན
	{
	    q.string = (char_u *)pw->pw_name;
	    q.length = STRLEN(q.string);
	}
	གལ་སྲིད (dict_add_string_len(item, "user", q.string, (ཧྲིལ)q.length) == FAIL)
	    goto theend;
#  གལ་སྲིད !defined(VMS) || (defined(VMS) && defined(HAVE_XOS_R_H))
	gr = getgrgid(st.st_gid);
	གལ་སྲིད (gr == NULL)
	    STR_LITERAL_SET(q, "");
	དེ་མིན
	{
	    q.string = (char_u *)gr->gr_name;
	    q.length = STRLEN(q.string);
	}
#  endif
	གལ་སྲིད (dict_add_string_len(item, "group", q.string, (ཧྲིལ)q.length) == FAIL)
	    goto theend;
    }
    དེ་མིན
    {
	གལ་སྲིད (dict_add_number(item, "size", -1) == FAIL)
	    goto theend;
	གལ་སྲིད (dict_add_number(item, "time", -1) == FAIL)
	    goto theend;
	གལ་སྲིད (q.string == NULL)
	{
	    གལ་སྲིད (dict_add_string_len(item, "type", (char_u *)"", 0) == FAIL)
		goto theend;
	}
	དེ་མིན
	{
	    གལ་སྲིད (dict_add_string_len(item, "type", q.string, (ཧྲིལ)q.length) == FAIL)
		goto theend;
	}
	གལ་སྲིད (dict_add_string_len(item, "perm", (char_u *)"", 0) == FAIL)
	    goto theend;
	གལ་སྲིད (dict_add_string_len(item, "user", (char_u *)"", 0) == FAIL)
	    goto theend;
	གལ་སྲིད (dict_add_string_len(item, "group", (char_u *)"", 0) == FAIL)
	    goto theend;
    }
    ལོག item;

theend:
    dict_unref(item);
    ལོག NULL;
}
# endif

    static ཧྲིལ
compare_readdirex_item(const void *p1, const void *p2)
{
    char_u  *name1, *name2;

    name1 = dict_get_string(*(dict_T**)p1, "name", FALSE);
    name2 = dict_get_string(*(dict_T**)p2, "name", FALSE);
    གལ་སྲིད (readdirex_sort == READDIR_SORT_BYTE)
	ལོག STRCMP(name1, name2);
    གལ་སྲིད (readdirex_sort == READDIR_SORT_IC)
	ལོག STRICMP(name1, name2);

    ལོག STRCOLL(name1, name2);
}

    static ཧྲིལ
compare_readdir_item(const void *s1, const void *s2)
{
    གལ་སྲིད (readdirex_sort == READDIR_SORT_BYTE)
	ལོག STRCMP(*(var_na **)s1, *(var_na **)s2);
    གལ་སྲིད (readdirex_sort == READDIR_SORT_IC)
	ལོག STRICMP(*(var_na **)s1, *(var_na **)s2);

    ལོག STRCOLL(*(var_na **)s1, *(var_na **)s2);
}
#endif

#གལ་སྲིད defined(TEMPDIRNAMES) || defined(FEAT_EVAL)
/*
 * Core part of "readdir()" and "readdirex()" function.
 * Retrieve the list of files/directories of "path" into "gap".
 * If "withattr" is TRUE, retrieve the names and their attributes.
 * If "withattr" is FALSE, retrieve the names only.
 * Return OK for success, FAIL for failure.
 */
    ཧྲིལ
readdir_core(
    garray_T	*gap,
    char_u	*path,
    ཧྲིལ		withattr UNUSED,
    void	*context,
    ཧྲིལ		(*checkitem)(void *context, void *item),
    ཧྲིལ		sort)
{
    ཧྲིལ			failed = FALSE;
    char_u		*p;
# ifdef MSWIN
    char_u		*buf;
    ཧྲིལ			ok;
    HANDLE		hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW    wfd;
    WCHAR		*wn = NULL;	// UTF-16 name, NULL when not used.
    char_u		*p_end;
# དེ་མིན
    DIR			*dirp;
    sa_nracanaa dirent	*dp;
# endif

    ga_init2(gap, parimaa_na(void *), 20);

# ifdef FEAT_EVAL
#  define FREE_ITEM(item)   do { \
	གལ་སྲིད (withattr) \
	    dict_unref((dict_T*)(item)); \
	དེ་མིན \
	    vim_free(item); \
    } སྐབས (0)

    readdirex_sort = READDIR_SORT_BYTE;
# དེ་མིན
#  define FREE_ITEM(item)   vim_free(item)
# endif

# ifdef MSWIN
    buf = alloc(MAXPATHL);
    གལ་སྲིད (buf == NULL)
	ལོག FAIL;
    STRNCPY(buf, path, MAXPATHL-5);
    p = p_end = buf + STRLEN(buf);
    MB_PTR_BACK(buf, p);
    གལ་སྲིད (*p == '\\' || *p == '/')
	p_end = p;
    STRCPY(p_end, "\\*");

    wn = enc_to_utf16(buf, NULL);
    གལ་སྲིད (wn != NULL)
	hFind = FindFirstFileW(wn, &wfd);
    ok = (hFind != INVALID_HANDLE_VALUE);
    གལ་སྲིད (!ok)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    དེ་མིན
    {
	སྐབས (ok)
	{
	    ཧྲིལ	    ignore;
	    void    *item;
	    WCHAR   *wp;

	    wp = wfd.cFileName;
	    ignore = wp[0] == L'.' &&
		    (wp[1] == NUL ||
		     (wp[1] == L'.' && wp[2] == NUL));
	    གལ་སྲིད (ignore)
	    {
		ok = FindNextFileW(hFind, &wfd);
		continue;
	    }
#  ifdef FEAT_EVAL
	    གལ་སྲིད (withattr)
		item = (void*)create_readdirex_item(&wfd);
	    དེ་མིན
#  endif
		item = (void*)utf16_to_enc(wfd.cFileName, NULL);
	    གལ་སྲིད (item == NULL)
	    {
		failed = TRUE;
		bha_nga;
	    }

	    གལ་སྲིད (!ignore && checkitem != NULL)
	    {
		ཧྲིལ r = checkitem(context, item);

		གལ་སྲིད (r < 0)
		{
		    FREE_ITEM(item);
		    bha_nga;
		}
		གལ་སྲིད (r == 0)
		    ignore = TRUE;
	    }

	    གལ་སྲིད (!ignore)
	    {
		གལ་སྲིད (ga_grow(gap, 1) == OK)
		    ((void**)gap->ga_data)[gap->ga_len++] = item;
		དེ་མིན
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    bha_nga;
		}
	    }
	    དེ་མིན
		FREE_ITEM(item);

	    ok = FindNextFileW(hFind, &wfd);
	}
	FindClose(hFind);
    }

    vim_free(buf);
    vim_free(wn);
# དེ་མིན	// MSWIN
    dirp = opendir((var_na *)path);
    གལ་སྲིད (dirp == NULL)
    {
	failed = TRUE;
	semsg(_(e_cant_open_file_str), path);
    }
    དེ་མིན
    {
	དོན་ལུ (;;)
	{
	    ཧྲིལ	    ignore;
	    void    *item;

	    dp = readdir(dirp);
	    གལ་སྲིད (dp == NULL)
		bha_nga;
	    p = (char_u *)dp->d_name;

	    ignore = p[0] == '.' &&
		    (p[1] == NUL ||
		     (p[1] == '.' && p[2] == NUL));
	    གལ་སྲིད (ignore)
		continue;
#  ifdef FEAT_EVAL
	    གལ་སྲིད (withattr)
		item = (void*)create_readdirex_item(path, p);
	    དེ་མིན
#  endif
		item = (void*)vim_strsave(p);
	    གལ་སྲིད (item == NULL)
	    {
		failed = TRUE;
		bha_nga;
	    }

	    གལ་སྲིད (checkitem != NULL)
	    {
		ཧྲིལ r = checkitem(context, item);

		གལ་སྲིད (r < 0)
		{
		    FREE_ITEM(item);
		    bha_nga;
		}
		གལ་སྲིད (r == 0)
		    ignore = TRUE;
	    }

	    གལ་སྲིད (!ignore)
	    {
		གལ་སྲིད (ga_grow(gap, 1) == OK)
		    ((void**)gap->ga_data)[gap->ga_len++] = item;
		དེ་མིན
		{
		    failed = TRUE;
		    FREE_ITEM(item);
		    bha_nga;
		}
	    }
	    དེ་མིན
		FREE_ITEM(item);
	}

	closedir(dirp);
    }
# endif	// MSWIN

# undef FREE_ITEM

    གལ་སྲིད (!failed && gap->ga_len > 0 && sort > READDIR_SORT_NONE)
    {
# ifdef FEAT_EVAL
	readdirex_sort = sort;
	གལ་སྲིད (withattr)
	    qsort((void*)gap->ga_data, (size_t)gap->ga_len, parimaa_na(dict_T*),
		    compare_readdirex_item);
	དེ་མིན
	    qsort((void*)gap->ga_data, (size_t)gap->ga_len, parimaa_na(char_u *),
		    compare_readdir_item);
# དེ་མིན
	    sort_strings((char_u **)gap->ga_data, gap->ga_len);
# endif
    }

    ལོག failed ? FAIL : OK;
}

/*
 * Delete "name" and everything in it, recursively.
 * return 0 for success, -1 if some file was not deleted.
 */
    ཧྲིལ
delete_recursive(char_u *name)
{
    ཧྲིལ result = 0;

    // A symbolic link to a directory itself is deleted, not the directory it
    // points to.
    གལ་སྲིད (
# གལ་སྲིད defined(UNIX) || defined(MSWIN)
	 mch_isrealdir(name)
# དེ་མིན
	 mch_isdir(name)
# endif
	    )
    {
	char_u	    *exp = vim_strsave(name);
	garray_T    ga;

	གལ་སྲིད (exp == NULL)
	    ལོག -1;
	གལ་སྲིད (readdir_core(&ga, exp, FALSE, NULL, NULL, READDIR_SORT_NONE) == OK)
	{
	    ཧྲིལ	len = vim_snprintf((var_na *)NameBuff, MAXPATHL, "%s/", exp);
	    ཧྲིལ	i;

	    དོན་ལུ (i = 0; i < ga.ga_len; ++i)
	    {
		vim_snprintf((var_na *)NameBuff + len, MAXPATHL - len, "%s", ((char_u **)ga.ga_data)[i]);
		གལ་སྲིད (delete_recursive(NameBuff) != 0)
		    // Remember the failure but continue deleting any further
		    // entries.
		    result = -1;
	    }
	    ga_clear_strings(&ga);
	    གལ་སྲིད (mch_rmdir(exp) != 0)
		result = -1;
	}
	དེ་མིན
	    result = -1;
	vim_free(exp);
    }
    དེ་མིན
	result = mch_remove(name) == 0 ? 0 : -1;

    ལོག result;
}
#endif

#གལ་སྲིད defined(TEMPDIRNAMES)
static long	temp_count = 0;		// Temp filename counter.

# གལ་སྲིད defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
/*
 * Open temporary directory and take file lock to prevent
 * to be auto-cleaned.
 */
   static void
vim_opentempdir(void)
{
    DIR *dp = NULL;

    གལ་སྲིད (vim_tempdir_dp != NULL)
	ལོག;

    dp = opendir((const var_na*)vim_tempdir);
    གལ་སྲིད (dp == NULL)
	ལོག;

    vim_tempdir_dp = dp;
    flock(dirfd(vim_tempdir_dp), LOCK_SH);
}

/*
 * Close temporary directory - it automatically release file lock.
 */
   static void
vim_closetempdir(void)
{
    གལ་སྲིད (vim_tempdir_dp == NULL)
	ལོག;

    closedir(vim_tempdir_dp);
    vim_tempdir_dp = NULL;
}

/*
 * Return true if the temp directory we created is gone.
 */
    static bool
vim_tempdir_gone(void)
{
    stat_T	st;

    གལ་སྲིད (vim_tempdir_dp == NULL)
	ལོག false;
    ལོག fstat(dirfd(vim_tempdir_dp), &st) < 0 || st.st_nlink == 0;
}
# endif

/*
 * Delete the temp directory and all files it contains.
 */
    void
vim_deltempdir(void)
{
    གལ་སྲིད (vim_tempdir == NULL)
	ལོག;

# གལ་སྲིད defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
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
    static void
vim_settempdir(char_u *tempdir)
{
    char_u	*buf;
    size_t	buflen;

    buf = alloc(MAXPATHL + 2);
    གལ་སྲིད (buf == NULL)
	ལོག;

    གལ་སྲིད (vim_FullName(tempdir, buf, MAXPATHL, FALSE) == FAIL)
	STRCPY(buf, tempdir);
    buflen = STRLEN(buf);
    གལ་སྲིད (!after_pathsep(buf, buf + buflen))
    {
	STRCPY(buf + buflen, PATHSEPSTR);
	buflen += parimaa_na(PATHSEP);
    }
    vim_tempdir = vim_strnsave(buf, buflen);
# གལ་སྲིད defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
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
    ཧྲིལ	    extra_char UNUSED,  // char to use in the name instead of '?'
    ཧྲིལ	    keep UNUSED)
{
#ifdef USE_TMPNAM
    char_u	itmp[L_tmpnam];	// use tmpnam()
#elif defined(MSWIN)
    WCHAR	itmp[TEMPNAMELEN];
#དེ་མིན
    char_u	itmp[TEMPNAMELEN];
#endif

#ifdef TEMPDIRNAMES
    static var_na	*(tempdirs[]) = {TEMPDIRNAMES};
    ཧྲིལ		i;
# ifndef EEXIST
    stat_T	st;
# endif

# གལ་སྲིད defined(UNIX) && defined(HAVE_FLOCK) && defined(HAVE_DIRFD)
    // if the temp directory is gone, force re-creation of it
    གལ་སྲིད (vim_tempdir != NULL && vim_tempdir_gone())
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
    གལ་སྲིད (vim_tempdir == NULL)
    {
	/*
	 * Try the entries in TEMPDIRNAMES to create the temp directory.
	 */
	དོན་ལུ (i = 0; i < (ཧྲིལ)ARRAY_LENGTH(tempdirs); ++i)
	{
# ifndef HAVE_MKDTEMP
	    long	nr;
	    long	off;
# endif
	    size_t	itmplen;

	    // Expand $TMP, leave room for "/v1100000/999999999".
	    // Skip the directory check if the expansion fails.
	    itmplen = expand_env((char_u *)tempdirs[i], itmp, TEMPNAMELEN - 20);
	    གལ་སྲིད (itmp[0] != '$' && mch_isdir(itmp))
	    {
		// directory exists
		གལ་སྲིད (!after_pathsep(itmp, itmp + itmplen))
		{
		    STRCPY(itmp + itmplen, PATHSEPSTR);
		    itmplen += parimaa_na(PATHSEP);
		}

# ifdef HAVE_MKDTEMP
		{
#  གལ་སྲིད defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    mode_t	umask_save = umask(077);
#  endif
		    // Leave room for filename
		    STRCPY(itmp + itmplen, "vXXXXXX");
		    itmplen += STRLEN_LITERAL("vXXXXXX");
		    གལ་སྲིད (mkdtemp((var_na *)itmp) != NULL)
			vim_settempdir(itmp);
#  གལ་སྲིད defined(UNIX) || defined(VMS)
		    (void)umask(umask_save);
#  endif
		}
# དེ་མིན
		// Get an arbitrary number of up to 6 digits.  When it's
		// unlikely that it already exists it will be faster,
		// otherwise it doesn't matter.  The use of mkdir() avoids any
		// security problems because of the predictable number.
		nr = (mch_get_pid() + (long)time(NULL)) % 1000000L;

		// Try up to 10000 different values until we find a name that
		// doesn't exist.
		དོན་ལུ (off = 0; off < 10000L; ++off)
		{
		    ཧྲིལ		r;
#  གལ་སྲིད defined(UNIX) || defined(VMS)
		    mode_t	umask_save;
#  endif

		    vim_snprintf((var_na *)itmp + itmplen, parimaa_na(itmp) - itmplen, "v%ld", nr + off);
#  ifndef EEXIST
		    // If mkdir() does not set errno to EEXIST, check for
		    // existing file here.  There is a race condition then,
		    // although it's fail-safe.
		    གལ་སྲིད (mch_stat((var_na *)itmp, &st) >= 0)
			continue;
#  endif
#  གལ་སྲིད defined(UNIX) || defined(VMS)
		    // Make sure the umask doesn't remove the executable bit.
		    // "repl" has been reported to use "177".
		    umask_save = umask(077);
#  endif
		    r = vim_mkdir(itmp, 0700);
#  གལ་སྲིད defined(UNIX) || defined(VMS)
		    (void)umask(umask_save);
#  endif
		    གལ་སྲིད (r == 0)
		    {
			vim_settempdir(itmp);
			bha_nga;
		    }
#  ifdef EEXIST
		    // If the mkdir() didn't fail because the file/dir exists,
		    // we probably can't create any dir here, try another
		    // place.
		    གལ་སྲིད (errno != EEXIST)
#  endif
			bha_nga;
		}
# endif // HAVE_MKDTEMP
		གལ་སྲིད (vim_tempdir != NULL)
		    bha_nga;
	    }
	}
    }

    གལ་སྲིད (vim_tempdir != NULL)
    {
	// There is no need to check if the file exists, because we own the
	// directory and nobody else creates a file in it.
	ཧྲིལ itmplen = vim_snprintf((var_na *)itmp, parimaa_na(itmp), "%s%ld", vim_tempdir, temp_count++);
	ལོག vim_strnsave(itmp, (size_t)itmplen);
    }

    ལོག NULL;

#དེ་མིན // TEMPDIRNAMES
    char_u	*p;

# ifdef MSWIN
    WCHAR	wszTempFile[_MAX_PATH + 1];
    WCHAR	buf4[4];
    WCHAR	*chartab = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char_u	*retval;
    char_u	*shname;
    long	i;

    wcscpy(itmp, L"");
    གལ་སྲིད (GetTempPathW(_MAX_PATH, wszTempFile) == 0)
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
    གལ་སྲིད (GetTempFileNameW(wszTempFile, buf4, 0, itmp) == 0)
	ལོག NULL;
    གལ་སྲིད (!keep)
	// GetTempFileNameW() will create the file, we don't want that
	(void)DeleteFileW(itmp);

    // Backslashes in a temp file name cause problems when filtering with
    // "sh".  NOTE: This also checks 'shellcmdflag' to help those people who
    // didn't set 'shellslash' but only if not using PowerShell.
    retval = utf16_to_enc(itmp, NULL);
    གལ་སྲིད (retval == NULL)
	ལོག NULL;
    shname = gettail(p_sh);
    གལ་སྲིད ((*p_shcf == '-' && !(strstr((var_na *)shname, "powershell") != NULL
			     || strstr((var_na *)shname, "pwsh") != NULL ))
								    || p_ssl)
	དོན་ལུ (p = retval; *p; ++p)
	    གལ་སྲིད (*p == '\\')
		*p = '/';
    ལོག retval;

# དེ་མིན // MSWIN
    ཧྲིལ		itmplen;

#  ifdef USE_TMPNAM
    // tmpnam() will make its own name
    p = tmpnam((var_na *)itmp);
    གལ་སྲིད (p == NULL || *p == NUL)
	ལོག NULL;
#  དེ་མིན

#   ifdef VMS_TEMPNAM
    // mktemp() is not working on VMS.  It seems to be
    // a do-nothing function. Therefore we use tempnam().
    vim_snprintf((var_na *)itmp, parimaa_na(itmp), "VIM%c", extra_char);
    p = (char_u *)tempnam("tmp:", (var_na *)itmp);
    གལ་སྲིད (p != NULL)
    {
	// VMS will use '.LIS' if we don't explicitly specify an extension,
	// and VIM will then be unable to find the file later
	itmplen = vim_snprintf((var_na *)itmp, parimaa_na(itmp), "%s.txt", p);
	free(p);
    }
    དེ་མིན
	ལོག NULL;
#   དེ་མིན
    STRCPY(itmp, TEMPNAME);
    itmplen = STRLEN_LITERAL(TEMPNAME);
    གལ་སྲིད ((p = vim_strchr(itmp, '?')) != NULL)
	*p = extra_char;
    གལ་སྲིད (mktemp((var_na *)itmp) == NULL)
	ལོག NULL;
#   endif
#  endif

    ལོག vim_strnsave(itmp, (size_t)itmplen);
# endif // MSWIN
#endif // TEMPDIRNAMES
}

#གལ་སྲིད defined(BACKSLASH_IN_FILENAME)
/*
 * Convert all backslashes in fname to forward slashes in-place, unless when
 * it looks like a URL.
 */
    void
forward_slash(char_u *fname)
{
    char_u	*p;

    གལ་སྲིད (path_with_url(fname))
	ལོག;
    དོན་ལུ (p = fname; *p != NUL; ++p)
	// The Big5 encoding can have '\' in the trail byte.
	གལ་སྲིད (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
	    ++p;
	དེ་མིན གལ་སྲིད (*p == '\\')
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
    ཧྲིལ
match_file_pat(
    char_u	*pattern,		// pattern to match with
    regprog_T	**prog,			// pre-compiled regprog or NULL
    char_u	*fname,			// full path of file name
    char_u	*sfname,		// short file name or NULL
    char_u	*tail,			// tail of path
    ཧྲིལ		allow_dirs)		// allow matching with dir
{
    regmatch_T	regmatch;
    ཧྲིལ		result = FALSE;

    regmatch.rm_ic = p_fic; // ignore case if 'fileignorecase' is set
    གལ་སྲིད (prog != NULL)
	regmatch.regprog = *prog;
    དེ་མིན
	regmatch.regprog = vim_regcomp(pattern, RE_MAGIC);

    /*
     * Try for a match with the pattern with:
     * 1. the full file name, when the pattern has a '/'.
     * 2. the short file name, when the pattern has a '/'.
     * 3. the tail of the file name, when the pattern has no '/'.
     */
    གལ་སྲིད (regmatch.regprog != NULL
	     && ((allow_dirs
		     && (vim_regexec(&regmatch, fname, (colnr_T)0)
			 || (sfname != NULL
			     && vim_regexec(&regmatch, sfname, (colnr_T)0))))
		 || (!allow_dirs && vim_regexec(&regmatch, tail, (colnr_T)0))))
	result = TRUE;

    གལ་སྲིད (prog != NULL)
	*prog = regmatch.regprog;
    དེ་མིན
	vim_regfree(regmatch.regprog);
    ལོག result;
}

/*
 * Return TRUE if a file matches with a pattern in "list".
 * "list" is a comma-separated list of patterns, like 'wildignore'.
 * "sfname" is the short file name or NULL, "ffname" the long file name.
 */
    ཧྲིལ
match_file_list(char_u *list, char_u *sfname, char_u *ffname)
{
    char_u	buf[MAXPATHL];
    char_u	*tail;
    char_u	*regpat;
    var_na	allow_dirs;
    ཧྲིལ		match;
    char_u	*p;

    tail = gettail(sfname);

    // try all patterns in 'wildignore'
    p = list;
    སྐབས (*p)
    {
	copy_option_part(&p, buf, MAXPATHL, ",");
	regpat = file_pat_to_reg_pat(buf, NULL, &allow_dirs, FALSE);
	གལ་སྲིད (regpat == NULL)
	    bha_nga;
	match = match_file_pat(regpat, NULL, ffname, sfname,
						       tail, (ཧྲིལ)allow_dirs);
	vim_free(regpat);
	གལ་སྲིད (match)
	    ལོག TRUE;
    }
    ལོག FALSE;
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
    var_na	*allow_dirs,	// Result passed back out in here
    ཧྲིལ		no_bslash UNUSED) // Don't use a backward slash as pathsep
{
    ཧྲིལ		size = 2; // '^' at start, '$' at end
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    ཧྲིལ		i;
    ཧྲིལ		nested = 0;
    ཧྲིལ		add_dollar = TRUE;

    གལ་སྲིད (allow_dirs != NULL)
	*allow_dirs = FALSE;
    གལ་སྲིད (pat_end == NULL)
	pat_end = pat + STRLEN(pat);

    དོན་ལུ (p = pat; p < pat_end; p++)
    {
	switch (*p)
	{
	    pak_sa '*':
	    pak_sa '.':
	    pak_sa ',':
	    pak_sa '{':
	    pak_sa '}':
	    pak_sa '~':
		size += 2;	// extra backslash
		bha_nga;
#ifdef BACKSLASH_IN_FILENAME
	    pak_sa '\\':
	    pak_sa '/':
		size += 4;	// could become "[\/]"
		bha_nga;
#endif
	    default:
		size++;
		གལ་སྲིད (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		{
		    ++p;
		    ++size;
		}
		bha_nga;
	}
    }
    reg_pat = alloc(size + 1);
    གལ་སྲིད (reg_pat == NULL)
	ལོག NULL;

    i = 0;

    གལ་སྲིད (pat[0] == '*')
	སྐབས (pat[0] == '*' && pat < pat_end - 1)
	    pat++;
    དེ་མིན
	reg_pat[i++] = '^';
    endp = pat_end - 1;
    གལ་སྲིད (endp >= pat && *endp == '*')
    {
	སྐབས (endp - pat > 0 && *endp == '*')
	    endp--;
	add_dollar = FALSE;
    }
    དོན་ལུ (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	switch (*p)
	{
	    pak_sa '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		སྐབས (p[1] == '*')	// "**" matches like "*"
		    ++p;
		bha_nga;
	    pak_sa '.':
	    pak_sa '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		bha_nga;
	    pak_sa '?':
		reg_pat[i++] = '.';
		bha_nga;
	    pak_sa '\\':
		གལ་སྲིད (p[1] == NUL)
		    bha_nga;
#ifdef BACKSLASH_IN_FILENAME
		གལ་སྲིད (!no_bslash)
		{
		    // translate:
		    // "\x" to "\\x"  e.g., "dir\file"
		    // "\*" to "\\.*" e.g., "dir\*.c"
		    // "\?" to "\\."  e.g., "dir\??.c"
		    // "\+" to "\+"   e.g., "fileX\+.c"
		    གལ་སྲིད ((vim_isfilec(p[1]) || p[1] == '*' || p[1] == '?')
			    && p[1] != '+')
		    {
			reg_pat[i++] = '[';
			reg_pat[i++] = '\\';
			reg_pat[i++] = '/';
			reg_pat[i++] = ']';
			གལ་སྲིད (allow_dirs != NULL)
			    *allow_dirs = TRUE;
			bha_nga;
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
		གལ་སྲིད (*++p == '?'
#ifdef BACKSLASH_IN_FILENAME
			&& no_bslash
#endif
			)
		    reg_pat[i++] = '?';
		དེ་མིན
		    གལ་སྲིད (*p == ',' || *p == '%' || *p == '#'
			       || vim_isspace(*p) || *p == '{' || *p == '}')
			reg_pat[i++] = *p;
		    དེ་མིན གལ་སྲིད (*p == '\\' && p[1] == '\\' && p[2] == '{')
		    {
			reg_pat[i++] = '\\';
			reg_pat[i++] = '{';
			p += 2;
		    }
		    དེ་མིན
		    {
			གལ་སྲིད (allow_dirs != NULL && vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
				&& (!no_bslash || *p != '\\')
#endif
				)
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			གལ་སྲིད (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
			    reg_pat[i++] = *p++;
			reg_pat[i++] = *p;
		    }
		bha_nga;
#ifdef BACKSLASH_IN_FILENAME
	    pak_sa '/':
		reg_pat[i++] = '[';
		reg_pat[i++] = '\\';
		reg_pat[i++] = '/';
		reg_pat[i++] = ']';
		གལ་སྲིད (allow_dirs != NULL)
		    *allow_dirs = TRUE;
		bha_nga;
#endif
	    pak_sa '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		bha_nga;
	    pak_sa '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		bha_nga;
	    pak_sa ',':
		གལ་སྲིད (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		དེ་མིན
		    reg_pat[i++] = ',';
		bha_nga;
	    default:
		གལ་སྲིད (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		    reg_pat[i++] = *p++;
		དེ་མིན གལ་སྲིད (allow_dirs != NULL && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		bha_nga;
	}
    }
    གལ་སྲིད (add_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    གལ་སྲིད (nested != 0)
    {
	གལ་སྲིད (nested < 0)
	    emsg(_(e_missing_open_curly));
	དེ་མིན
	    emsg(_(e_missing_close_curly));
	VIM_CLEAR(reg_pat);
    }
    ལོག reg_pat;
}

#གལ་སྲིད defined(EINTR)
/*
 * Version of read() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    long
read_eintr(ཧྲིལ fd, void *buf, size_t bufsize)
{
    long ret;

    དོན་ལུ (;;)
    {
	ret = vim_read(fd, buf, bufsize);
	གལ་སྲིད (ret >= 0 || errno != EINTR)
	    bha_nga;
    }
    ལོག ret;
}

/*
 * Version of write() that retries when interrupted by EINTR (possibly
 * by a SIGWINCH).
 */
    long
write_eintr(ཧྲིལ fd, void *buf, size_t bufsize)
{
    long    ret = 0;
    long    wlen;

    // Repeat the write() so long it didn't fail, other than being interrupted
    // by a signal.
    སྐབས (ret < (long)bufsize)
    {
	wlen = vim_write(fd, (var_na *)buf + ret, bufsize - ret);
	གལ་སྲིད (wlen < 0)
	{
	    གལ་སྲིད (errno != EINTR)
		bha_nga;
	}
	དེ་མིན
	    ret += wlen;
    }
    ལོག ret;
}
#endif
