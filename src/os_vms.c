/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 * VMS port			by Henk Elbers
 * VMS deport			by Zoltan Arpadffy
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#அந்தர்பாவ "vim.h"

// define _generic_64 for use in time functions
#எனில் !defined(VAX)
# include <gen64def.h>
#இல்லையெனில்
// based on Alpha's gen64def.h; the file is absent on VAX
வகைவரை கட்டமைப்பு _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	unsigned முழு gen64$l_longword [2];   // ...two 32-bit values, or
	unsigned short முழு gen64$w_word [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#endif

வகைவரை கட்டமைப்பு
{
    எழுத்து	class;
    எழுத்து	type;
    short	width;
    ஒன்றிப்பு
    {
	கட்டமைப்பு
	{
	    எழுத்து	_basic[3];
	    எழுத்து	length;
	}	y;
	முழு	basic;
    }	x;
    முழு		extended;
}	TT_MODE;

வகைவரை கட்டமைப்பு
{
    short	buflen;
    short	itemcode;
    எழுத்து	*bufadrs;
    முழு		*retlen;
}	ITEM;

வகைவரை கட்டமைப்பு
{
    ITEM	equ;
    முழு		nul;
}	ITMLST1;

வகைவரை கட்டமைப்பு
{
    ITEM	index;
    ITEM	string;
    முழு	nul;
}	ITMLST2;

நிலையான TT_MODE	orgmode;
நிலையான short	iochan;			// TTY I/O channel
நிலையான short	iosb[4];		// IO status block

நிலையான முழு vms_match_இலக்கம் = 0;
நிலையான முழு vms_match_free = 0;
நிலையான char_u **vms_fmatch = வெற்று;
நிலையான எழுத்து *Fspec_Rms;		       // rms file spec, passed implicitly between routines



நிலையான TT_MODE	get_tty(வெறுமை);
நிலையான வெறுமை	set_tty(முழு row, முழு col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) செய் { போது (Str[1] == '/') ++Str; } போது (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    நிலையான வெறுமை
vul_desc(DESC *des, எழுத்து *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_pointer = str;
    des->dsc$w_length = str ? strlen(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    நிலையான வெறுமை
vul_item(ITEM *itm, short len, short cod, எழுத்து *adr, முழு *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    வெறுமை
mch_settmode(tmode_T tmode)
{
    முழு	status;

    எனில் ( tmode == TMODE_RAW )
	set_tty(0, 0);
    இல்லையெனில்
    {
	தேர்வு (orgmode.width)
	{
	    நிலை 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	நிறுத்து;
	    நிலை 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	நிறுத்து;
	    இயல்பு:	நிறுத்து;
	}
	out_flush();
	status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, அளவு(TT_MODE), 0,0,0,0);
	எனில் (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    திரும்பு;
	(வெறுமை)sys$dassgn(iochan);
	iochan = 0;
    }
}

    நிலையான வெறுமை
set_tty(முழு row, முழு col)
{
    முழு		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    நிலையான short    first_time = TRUE;

    எனில் (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    எனில் (col)
	newmode.width		 = col;
    எனில் (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, அளவு(newmode), 0, 0, 0, 0);
    எனில் (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	திரும்பு;
}

    நிலையான TT_MODE
get_tty(வெறுமை)
{

    நிலையான $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    முழு		status;
    TT_MODE	tt_mode;

    எனில் (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, அளவு(tt_mode), 0, 0, 0, 0);
    எனில் (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    திரும்பு(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    முழு
mch_get_shellsize(வெறுமை)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    திரும்பு OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    வெறுமை
mch_set_shellsize(வெறுமை)
{
    set_tty(Rows, Columns);
    தேர்வு (Columns)
    {
	நிலை 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	நிறுத்து;
	நிலை 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	நிறுத்து;
	இயல்பு:	நிறுத்து;
    }
    out_flush();
    screen_start();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    நிலையான எழுத்து		buffer[LNM$C_NAMLENGTH+1];
    char_u		*cp = வெற்று;
    unsigned long	attrib;
    முழு			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    எழுத்து		*sbuf = வெற்று;

    vul_desc(&d_lognam, (எழுத்து *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, அளவு(முழு), LNM$_INDEX, (எழுத்து *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    எனில் (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, வெற்று,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	எனில் (cp = alloc(lengte + 1))
	    strcpy((எழுத்து *)cp, buffer);
	திரும்பு(cp);
    }
    இல்லையெனில் எனில் ((sbuf = getenv((எழுத்து *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = alloc(lengte);
	எனில் (cp)
	    strcpy((எழுத்து *)cp, sbuf);
	திரும்பு cp;
    }
    இல்லையெனில்
	திரும்பு(வெற்று);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    முழு
mch_setenv(எழுத்து *var, எழுத்து *மதிப்பு, முழு x)
{
    முழு		res, dum;
    long	attrib = 0L;
    எழுத்து	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, மதிப்பு ? strlen(மதிப்பு) : 0, மதிப்பு ? LNM$_STRING : 0,
	    மதிப்பு, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    திரும்பு((res == 1) ? 0 : -1);
}

    முழு
vms_sys(எழுத்து *cmd, எழுத்து *out, எழுத்து *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status;

    எனில் (cmd)
	vul_desc(&cdsc, cmd);
    எனில் (out)
	vul_desc(&odsc, out);
    எனில் (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : வெற்று,		// command string
	      inp ? &idsc : வெற்று,		// input file
	      out ? &odsc : வெற்று,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    திரும்பு status;
}

/*
 * Convert string to lowercase - most often filename
 */
    எழுத்து *
vms_tolower( எழுத்து *name )
{
    முழு i,nlen = strlen(name);
    க்கு (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    திரும்பு name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    முழு
vms_sys_status(முழு status)
{
    எனில் (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	திரும்பு status;		// Command failed.
    திரும்பு 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    முழு
vms_read(எழுத்து *inbuf, size_t nbytes)
{
    முழு		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    நிலையான long trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    எனில் (!iochan)
	tt_mode = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_item(&itmlst[0], 0, TRM$_MODIFIERS,
	 (எழுத்து *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_item(&itmlst[1], அளவு(trm_mask), TRM$_TERM, (எழுத்து *)&trm_mask, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, அளவு(itmlst));
    len = strlen(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = strlen(inbuf); // return the total length

    திரும்பு len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    நிலையான முழு
vms_wproc(எழுத்து *name, முழு val)
{
    முழு i;
    நிலையான முழு vms_match_alloced = 0;

    எனில் (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	திரும்பு 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    எனில் (vms_match_இலக்கம் == 0)
    {
	// first time through, setup some things
	எனில் (வெற்று == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    எனில் (!vms_fmatch)
		திரும்பு 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	இல்லையெனில்
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    க்கு (i = 0; i<vms_match_இலக்கம்; i++)
    {
	எனில் (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    திரும்பு 1;
    }
    எனில் (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		அளவு(எழுத்து **) * vms_match_alloced);
	எனில் (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    திரும்பு 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_இலக்கம்] = vim_strsave((char_u *)name);

    ++vms_match_இலக்கம்;
    திரும்பு 1;
}

/*
 *	mch_expand_wildcards	this code does wild-card pattern
 *				matching NOT using the shell
 *
 *	return OK for success, FAIL for error (you may lose some
 *	memory) and put an error message in *file.
 *
 *	num_pat	   number of input patterns
 *	pat	   array of pointers to input patterns
 *	num_file   pointer to number of matched file names
 *	file	   pointer to array of pointers to matched file names
 *
 */
    முழு
mch_expand_wildcards(
    முழு		இலக்கம்_pat,
    char_u	**pat,
    முழு		*இலக்கம்_file,
    char_u	***file,
    முழு		flags)
{
    முழு		i, cnt = 0;
    char_u	buf[MAXPATHL];
    எழுத்து       *result;
    முழு		dir;
    முழு files_alloced, files_free;

    *இலக்கம்_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    எனில் (*file == வெற்று)
    {
	*இலக்கம்_file = 0;
	திரும்பு FAIL;
    }
    க்கு (i = 0; i < இலக்கம்_pat; i++)
    {
	// expand environment var or home dir
	எனில் (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	இல்லையெனில்
	    STRCPY(buf,pat[i]);

	vms_match_இலக்கம் = 0; // reset collection counter
	result = decc$translate_vms(vms_fixfilename(buf));
	எனில் ( (முழு) result == 0 || (முழு) result == -1  )
	{
	    cnt = 0;
	}
	இல்லையெனில்
	{
	    cnt = decc$to_vms(result, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	எனில் (cnt > 0)
	    cnt = vms_match_இலக்கம்;

	எனில் (cnt < 1)
	    தொடர்;

	க்கு (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    எனில் (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		தொடர்;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    எனில் (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		தொடர்;

	    // Skip files that are not executable if we check for that.
	    எனில் (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], வெற்று, !(flags & EW_SHELLCMD)))
		தொடர்;

	    // allocate memory for pointers
	    எனில் (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, அளவு(char_u **) * files_alloced);
		எனில் (*file == வெற்று)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *இலக்கம்_file = 0;
		    திரும்பு(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*இலக்கம்_file++] = vms_fmatch[i];
	}
    }
    திரும்பு OK;
}

    முழு
mch_expandpath(garray_T *gap, char_u *path, முழு flags)
{
    முழு		i,cnt = 0;
    எழுத்து       *result;

    vms_match_இலக்கம் = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    result = decc$translate_vms(vms_fixfilename(path));
    எனில் ( (முழு) result == 0 || (முழு) result == -1  )
    {
	cnt = 0;
    }
    இல்லையெனில்
    {
	cnt = decc$to_vms(result, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    எனில் (cnt > 0)
	cnt = vms_match_இலக்கம்;
    க்கு (i = 0; i < cnt; i++)
    {
	எனில் (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    திரும்பு cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    நிலையான வெறுமை
vms_unix_mixed_filespec(எழுத்து *in, எழுத்து *out)
{
    எழுத்து *lastcolon;
    எழுத்து *end_of_dir;
    எழுத்து ch;
    முழு len;
    எழுத்து *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(in, ':');   // find last colon
    எனில் (lastcolon != வெற்று)
    {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = வெற்று;	// default: no directory

    // start of directory portion
    ch = *in;
    எனில் ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    இல்லையெனில் எனில் (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    இல்லையெனில்
    {                                // not a special character
	போது (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    எனில் (strchr(in, '/') == வெற்று)     // any more Unix directories ?
    {
	strcpy(out, in);             // No - get rest of the spec
	திரும்பு;
    }
    இல்லையெனில்
    {
	*out++ = '[';                // Yes, denote a Vms subdirectory
	ch = '.';
	--in;
	}
    }

    // if we get here, there is a directory part of the filename

    // initialize output file spec
    *out++ = ch;
    ++in;

    போது (*in != '\0')
    {
	ch = *in;
	எனில் ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	இல்லையெனில் எனில் (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	இல்லையெனில்
	{
	    போது (EQN(in, "./", 2))    // Ignore Unix "current dir"
	    {
		end_of_dir = out;
		in += 2;
		SKIP_FOLLOWING_SLASHES(in);
		ch = *in;
	    }
	}

    // Place next character into output file spec
	*out++ = ch;
	++in;
    }

    *out = '\0';            // Terminate output file spec

    எனில் (end_of_dir != வெற்று) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    நிலையான முழு
vms_fspec_proc(எழுத்து *fil, முழு val)
{
    strcpy(Fspec_Rms,fil);
    திரும்பு(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    வெறுமை *
vms_fixfilename(வெறுமை *instring)
{
    நிலையான எழுத்து		*buf = வெற்று;
    நிலையான size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = strlen(instring) + 1;
    எனில் (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * அளவு(எழுத்து));
    }

#ifdef DEBUG
    எழுத்து		 *tmpbuf = வெற்று;
    tmpbuf = ALLOC_MULT(எழுத்து, buflen);
    strcpy(tmpbuf, instring);
#endif

    Fspec_Rms = buf;				// for decc$to_vms

    எனில் (strchr(instring,'/') == வெற்று)
	// It is already a VMS file spec
	strcpy(buf, instring);
    இல்லையெனில் எனில் (strchr(instring,'"') == வெற்று)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	எனில் ( (strchr(instring,'[') == வெற்று) && (strchr(instring,'<') == வெற்று) &&
	     (strchr(instring,']') == வெற்று) && (strchr(instring,'>') == வெற்று) &&
	     (strchr(instring,':') == வெற்று) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	இல்லையெனில்
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    இல்லையெனில்
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    திரும்பு buf;
}

/*
 * Remove version number from file name
 * we need it in some special cases as:
 * creating swap file name and writing new file
 */

/*
 * 2025-05-13 SMS.
 * Using $PARSE would be simpler and more accurate, if all-VMS (not
 * mixed UNIX+VMS) path were ensured.  Meanwhile, to improve (imperfect)
 * handling of extended name syntax:
 *     o All characters (up to five (32767)) after semi-colon (or last
 *       of multiple dots) must be numeric.
 *     o Caret-escaped semi-colon (^;) or dot (^.) does not delimit
 *       version.
 * Whether it makes sense to detect syntax errors here is not entirely
 * clear.  Currently, many invalid version strings are not treated as
 * version strings.  (More could be.)
 */
    வெறுமை
vms_remove_version(வெறுமை * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    முழு		done = 0;
    முழு		vdigits = 0;

    rp = (char_u *)fname+ strlen( (எழுத்து *)fname)- 1;    // Rightmost char
    போது ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	எனில் (isdigit( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	இல்லையெனில்
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    எனில் (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	எனில் (*rp == (char_u)';')
	{
	    எனில் ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	இல்லையெனில் எனில் (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    எனில் ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		போது ((done == 0) && (dp >= (char_u *)fname))
		{
		    எனில் ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			எனில் ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    இல்லையெனில் எனில் (*dp == '.')
		    {                   // Possible dot delimiter
			எனில் ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dot delimiter
			    done = 1;   // Previous dot found in name
			    *rp = '\0'; // Trim off ".nnn"
			}
		    }
		    dp--;               // Next char to right
		} // while
	    }
	}
	// Else no version found to remove
    } // if (vdigits  <= 5)

    திரும்பு ;
}

கட்டமைப்பு typeahead_st {
    unsigned short numchars;
    unsigned எழுத்து  firstchar;
    unsigned எழுத்து  reserved0;
    unsigned long  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    முழு
RealWaitForChar(
    முழு		fd UNUSED, // always read from iochan
    long	msec,
    முழு		*check_for_gpm UNUSED,
    முழு		*interrupted)
{
    முழு status;
    கட்டமைப்பு _generic_64 time_curr;
    கட்டமைப்பு _generic_64 time_diff;
    கட்டமைப்பு _generic_64 time_out;
    unsigned முழு convert_operation = LIB$K_DELTA_SECONDS_F;
    தசம sec =(தசம) msec/1000;

    // make sure the iochan is set
    எனில் (!iochan)
	get_tty();

    எனில் (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	எனில் (status != SS$_NORMAL)
	    திரும்பு 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#எனில் __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# இல்லையெனில்
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#இல்லையெனில்
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	எனில் (status != LIB$_NORMAL)
	    திரும்பு 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	எனில் (status != LIB$_NORMAL)
	    திரும்பு 0; // error
    }

    போது (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	எனில் (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    திரும்பு 0; // error

	எனில் (typeahead.numchars)
	    திரும்பு 1; // ready to read

	// there's nothing to read; what now?
	எனில் (msec == 0)
	{
	    // immediate time-out; return impatiently
	    திரும்பு 0;
	}
	இல்லையெனில் எனில் (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    திரும்பு 1; // fakeout to force a wait in vms_read()
	}
	இல்லையெனில்
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    எனில் (status != SS$_NORMAL)
		திரும்பு 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    எனில் (status != LIB$_NORMAL)
		திரும்பு 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#எனில் !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

வகைவரை கட்டமைப்பு
   {
   எழுத்து *name;
   முழு மதிப்பு;
   } decc_feat_t;

முழு vms_init_done = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (எழுத்து *)வெற்று, 0 } };


/* LIB$INITIALIZE initialization.
 *
 * On sufficiently recent non-VAX systems, set a collection of C RTL
 * features without using the DECC$* logical name method.
 *
 * Note: Old VAX VMS versions may suffer from a linker complaint like
 * this:
 *
 * %LINK-W-MULPSC, conflicting attributes for psect LIB$INITIALIZE
 * in module LIB$INITIALIZE file SYS$COMMON:[SYSLIB]STARLET.OLB;1
 *
 * Using a LINK options file which includes a line like this one should
 * stop this complaint:
 *
 * PSECT_ATTR=LIB$INITIALIZE,NOPIC
 */

/* vms_init()
 *
 * Uses LIB$INITIALIZE to set a collection of C RTL features without
 * requiring the user to define the corresponding logical names.
 *
 * LIB$INITIALIZE initialization function
 */

நிலையான வெறுமை
vms_init(வெறுமை)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_done = 1;

    முழு feat_index;
    முழு feat_மதிப்பு;
    முழு feat_மதிப்பு_max;
    முழு feat_மதிப்பு_min;
    முழு i;
    முழு sts;

    // Loop through all items in the decc_feat_array[]
    க்கு (i = 0; decc_feat_array[i].name != வெற்று; i++)
    {
	// Get the feature index
	feat_index = decc$feature_get_index(decc_feat_array[i].name);
	எனில் (feat_index >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_மதிப்பு = decc$feature_get_மதிப்பு(feat_index, 1);
	    feat_மதிப்பு_min = decc$feature_get_மதிப்பு(feat_index, 2);
	    feat_மதிப்பு_max = decc$feature_get_மதிப்பு(feat_index, 3);

	    எனில் ((decc_feat_array[i].மதிப்பு >= feat_மதிப்பு_min) && (decc_feat_array[i].மதிப்பு <= feat_மதிப்பு_max))
	    {
		// Valid value.  Set it if necessary
		எனில் (feat_மதிப்பு != decc_feat_array[i].மதிப்பு)
		    sts = decc$feature_set_மதிப்பு(feat_index, 1, decc_feat_array[i].மதிப்பு);
	    }
	    இல்லையெனில்
		// Invalid DECC feature value
		printf("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_மதிப்பு, feat_மதிப்பு_min, decc_feat_array[i].name, feat_மதிப்பு_max);
	}
	இல்லையெனில்
	    // Invalid DECC feature name
	    printf("UNKNOWN DECC FEATURE: %s.\n", decc_feat_array[i].name);
    }
}


/* Get "vms_init()" into a valid, loaded LIB$INITIALIZE PSECT. */

# pragma nostandard

/* Establish the LIB$INITIALIZE PSECTs, with proper alignment and
 * other attributes.  Note that "nopic" is significant only on VAX.
 */
# pragma extern_model save

# pragma extern_model strict_refdef "LIB$INITIALIZE" 2, nopic, nowrt
வெறுமை (*மாறிலி x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
மாறிலி முழு spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
முழு LIB$INITIALIZE(வெறுமை);
# pragma extern_model strict_refdef
முழு dmy_lib$initialize = (முழு) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
