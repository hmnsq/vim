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

#അന്തര്ഭാവ "vim.h"

// define _generic_64 for use in time functions
#എങ്കിൽ !defined(VAX)
# include <gen64def.h>
#അല്ലെങ്കിൽ
// based on Alpha's gen64def.h; the file is absent on VAX
തരനിർവചനം ഘടന _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	unsigned പൂർണ്ണസംഖ്യ gen64$l_longword [2];   // ...two 32-bit values, or
	unsigned short പൂർണ്ണസംഖ്യ gen64$w_word [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#endif

തരനിർവചനം ഘടന
{
    അക്ഷരം	class;
    അക്ഷരം	type;
    short	width;
    സംഘം
    {
	ഘടന
	{
	    അക്ഷരം	_basic[3];
	    അക്ഷരം	length;
	}	y;
	പൂർണ്ണസംഖ്യ	basic;
    }	x;
    പൂർണ്ണസംഖ്യ		extended;
}	TT_MODE;

തരനിർവചനം ഘടന
{
    short	buflen;
    short	itemcode;
    അക്ഷരം	*bufadrs;
    പൂർണ്ണസംഖ്യ		*retlen;
}	ITEM;

തരനിർവചനം ഘടന
{
    ITEM	equ;
    പൂർണ്ണസംഖ്യ		nul;
}	ITMLST1;

തരനിർവചനം ഘടന
{
    ITEM	index;
    ITEM	string;
    പൂർണ്ണസംഖ്യ	nul;
}	ITMLST2;

സ്ഥിതം TT_MODE	orgmode;
സ്ഥിതം short	iochan;			// TTY I/O channel
സ്ഥിതം short	iosb[4];		// IO status block

സ്ഥിതം പൂർണ്ണസംഖ്യ vms_match_അക്കം = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ vms_match_free = 0;
സ്ഥിതം char_u **vms_fmatch = ശൂന്യ;
സ്ഥിതം അക്ഷരം *Fspec_Rms;		       // rms file spec, passed implicitly between routines



സ്ഥിതം TT_MODE	get_tty(ശൂന്യം);
സ്ഥിതം ശൂന്യം	set_tty(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) ചെയ്യുക { ആകുമ്പോൾ (Str[1] == '/') ++Str; } ആകുമ്പോൾ (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    സ്ഥിതം ശൂന്യം
vul_desc(DESC *des, അക്ഷരം *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_pointer = str;
    des->dsc$w_length = str ? strlen(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    സ്ഥിതം ശൂന്യം
vul_item(ITEM *itm, short len, short cod, അക്ഷരം *adr, പൂർണ്ണസംഖ്യ *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    ശൂന്യം
mch_settmode(tmode_T tmode)
{
    പൂർണ്ണസംഖ്യ	status;

    എങ്കിൽ ( tmode == TMODE_RAW )
	set_tty(0, 0);
    അല്ലെങ്കിൽ
    {
	തിരഞ്ഞെടുപ്പ് (orgmode.width)
	{
	    അവസ്ഥ 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	നിർത്തുക;
	    അവസ്ഥ 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:	നിർത്തുക;
	}
	out_flush();
	status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, വലുപ്പം(TT_MODE), 0,0,0,0);
	എങ്കിൽ (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    തിരികെ;
	(ശൂന്യം)sys$dassgn(iochan);
	iochan = 0;
    }
}

    സ്ഥിതം ശൂന്യം
set_tty(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    പൂർണ്ണസംഖ്യ		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    സ്ഥിതം short    first_time = TRUE;

    എങ്കിൽ (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    എങ്കിൽ (col)
	newmode.width		 = col;
    എങ്കിൽ (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, വലുപ്പം(newmode), 0, 0, 0, 0);
    എങ്കിൽ (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	തിരികെ;
}

    സ്ഥിതം TT_MODE
get_tty(ശൂന്യം)
{

    സ്ഥിതം $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    പൂർണ്ണസംഖ്യ		status;
    TT_MODE	tt_mode;

    എങ്കിൽ (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, വലുപ്പം(tt_mode), 0, 0, 0, 0);
    എങ്കിൽ (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    തിരികെ(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    പൂർണ്ണസംഖ്യ
mch_get_shellsize(ശൂന്യം)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    തിരികെ OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    ശൂന്യം
mch_set_shellsize(ശൂന്യം)
{
    set_tty(Rows, Columns);
    തിരഞ്ഞെടുപ്പ് (Columns)
    {
	അവസ്ഥ 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	നിർത്തുക;
	അവസ്ഥ 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	നിർത്തുക;
	സ്ഥിരസ്ഥിതി:	നിർത്തുക;
    }
    out_flush();
    screen_start();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    സ്ഥിതം അക്ഷരം		buffer[LNM$C_NAMLENGTH+1];
    char_u		*cp = ശൂന്യ;
    unsigned long	attrib;
    പൂർണ്ണസംഖ്യ			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    അക്ഷരം		*sbuf = ശൂന്യ;

    vul_desc(&d_lognam, (അക്ഷരം *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, വലുപ്പം(പൂർണ്ണസംഖ്യ), LNM$_INDEX, (അക്ഷരം *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    എങ്കിൽ (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, ശൂന്യ,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	എങ്കിൽ (cp = alloc(lengte + 1))
	    strcpy((അക്ഷരം *)cp, buffer);
	തിരികെ(cp);
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((sbuf = getenv((അക്ഷരം *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = alloc(lengte);
	എങ്കിൽ (cp)
	    strcpy((അക്ഷരം *)cp, sbuf);
	തിരികെ cp;
    }
    അല്ലെങ്കിൽ
	തിരികെ(ശൂന്യ);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    പൂർണ്ണസംഖ്യ
mch_setenv(അക്ഷരം *var, അക്ഷരം *മൂല്യം, പൂർണ്ണസംഖ്യ x)
{
    പൂർണ്ണസംഖ്യ		res, dum;
    long	attrib = 0L;
    അക്ഷരം	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, മൂല്യം ? strlen(മൂല്യം) : 0, മൂല്യം ? LNM$_STRING : 0,
	    മൂല്യം, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    തിരികെ((res == 1) ? 0 : -1);
}

    പൂർണ്ണസംഖ്യ
vms_sys(അക്ഷരം *cmd, അക്ഷരം *out, അക്ഷരം *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status;

    എങ്കിൽ (cmd)
	vul_desc(&cdsc, cmd);
    എങ്കിൽ (out)
	vul_desc(&odsc, out);
    എങ്കിൽ (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : ശൂന്യ,		// command string
	      inp ? &idsc : ശൂന്യ,		// input file
	      out ? &odsc : ശൂന്യ,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    തിരികെ status;
}

/*
 * Convert string to lowercase - most often filename
 */
    അക്ഷരം *
vms_tolower( അക്ഷരം *name )
{
    പൂർണ്ണസംഖ്യ i,nlen = strlen(name);
    വേണ്ടി (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    തിരികെ name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    പൂർണ്ണസംഖ്യ
vms_sys_status(പൂർണ്ണസംഖ്യ status)
{
    എങ്കിൽ (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	തിരികെ status;		// Command failed.
    തിരികെ 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    പൂർണ്ണസംഖ്യ
vms_read(അക്ഷരം *inbuf, size_t nbytes)
{
    പൂർണ്ണസംഖ്യ		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    സ്ഥിതം long trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    എങ്കിൽ (!iochan)
	tt_mode = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_item(&itmlst[0], 0, TRM$_MODIFIERS,
	 (അക്ഷരം *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_item(&itmlst[1], വലുപ്പം(trm_mask), TRM$_TERM, (അക്ഷരം *)&trm_mask, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, വലുപ്പം(itmlst));
    len = strlen(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = strlen(inbuf); // return the total length

    തിരികെ len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vms_wproc(അക്ഷരം *name, പൂർണ്ണസംഖ്യ val)
{
    പൂർണ്ണസംഖ്യ i;
    സ്ഥിതം പൂർണ്ണസംഖ്യ vms_match_alloced = 0;

    എങ്കിൽ (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	തിരികെ 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    എങ്കിൽ (vms_match_അക്കം == 0)
    {
	// first time through, setup some things
	എങ്കിൽ (ശൂന്യ == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    എങ്കിൽ (!vms_fmatch)
		തിരികെ 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	അല്ലെങ്കിൽ
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    വേണ്ടി (i = 0; i<vms_match_അക്കം; i++)
    {
	എങ്കിൽ (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    തിരികെ 1;
    }
    എങ്കിൽ (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		വലുപ്പം(അക്ഷരം **) * vms_match_alloced);
	എങ്കിൽ (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    തിരികെ 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_അക്കം] = vim_strsave((char_u *)name);

    ++vms_match_അക്കം;
    തിരികെ 1;
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
    പൂർണ്ണസംഖ്യ
mch_expand_wildcards(
    പൂർണ്ണസംഖ്യ		അക്കം_pat,
    char_u	**pat,
    പൂർണ്ണസംഖ്യ		*അക്കം_file,
    char_u	***file,
    പൂർണ്ണസംഖ്യ		flags)
{
    പൂർണ്ണസംഖ്യ		i, cnt = 0;
    char_u	buf[MAXPATHL];
    അക്ഷരം       *result;
    പൂർണ്ണസംഖ്യ		dir;
    പൂർണ്ണസംഖ്യ files_alloced, files_free;

    *അക്കം_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    എങ്കിൽ (*file == ശൂന്യ)
    {
	*അക്കം_file = 0;
	തിരികെ FAIL;
    }
    വേണ്ടി (i = 0; i < അക്കം_pat; i++)
    {
	// expand environment var or home dir
	എങ്കിൽ (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	അല്ലെങ്കിൽ
	    STRCPY(buf,pat[i]);

	vms_match_അക്കം = 0; // reset collection counter
	result = decc$translate_vms(vms_fixfilename(buf));
	എങ്കിൽ ( (പൂർണ്ണസംഖ്യ) result == 0 || (പൂർണ്ണസംഖ്യ) result == -1  )
	{
	    cnt = 0;
	}
	അല്ലെങ്കിൽ
	{
	    cnt = decc$to_vms(result, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	എങ്കിൽ (cnt > 0)
	    cnt = vms_match_അക്കം;

	എങ്കിൽ (cnt < 1)
	    തുടരുക;

	വേണ്ടി (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    എങ്കിൽ (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		തുടരുക;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    എങ്കിൽ (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		തുടരുക;

	    // Skip files that are not executable if we check for that.
	    എങ്കിൽ (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], ശൂന്യ, !(flags & EW_SHELLCMD)))
		തുടരുക;

	    // allocate memory for pointers
	    എങ്കിൽ (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, വലുപ്പം(char_u **) * files_alloced);
		എങ്കിൽ (*file == ശൂന്യ)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *അക്കം_file = 0;
		    തിരികെ(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*അക്കം_file++] = vms_fmatch[i];
	}
    }
    തിരികെ OK;
}

    പൂർണ്ണസംഖ്യ
mch_expandpath(garray_T *gap, char_u *path, പൂർണ്ണസംഖ്യ flags)
{
    പൂർണ്ണസംഖ്യ		i,cnt = 0;
    അക്ഷരം       *result;

    vms_match_അക്കം = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    result = decc$translate_vms(vms_fixfilename(path));
    എങ്കിൽ ( (പൂർണ്ണസംഖ്യ) result == 0 || (പൂർണ്ണസംഖ്യ) result == -1  )
    {
	cnt = 0;
    }
    അല്ലെങ്കിൽ
    {
	cnt = decc$to_vms(result, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    എങ്കിൽ (cnt > 0)
	cnt = vms_match_അക്കം;
    വേണ്ടി (i = 0; i < cnt; i++)
    {
	എങ്കിൽ (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    തിരികെ cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    സ്ഥിതം ശൂന്യം
vms_unix_mixed_filespec(അക്ഷരം *in, അക്ഷരം *out)
{
    അക്ഷരം *lastcolon;
    അക്ഷരം *end_of_dir;
    അക്ഷരം ch;
    പൂർണ്ണസംഖ്യ len;
    അക്ഷരം *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(in, ':');   // find last colon
    എങ്കിൽ (lastcolon != ശൂന്യ)
    {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = ശൂന്യ;	// default: no directory

    // start of directory portion
    ch = *in;
    എങ്കിൽ ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    അല്ലെങ്കിൽ
    {                                // not a special character
	ആകുമ്പോൾ (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    എങ്കിൽ (strchr(in, '/') == ശൂന്യ)     // any more Unix directories ?
    {
	strcpy(out, in);             // No - get rest of the spec
	തിരികെ;
    }
    അല്ലെങ്കിൽ
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

    ആകുമ്പോൾ (*in != '\0')
    {
	ch = *in;
	എങ്കിൽ ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	അല്ലെങ്കിൽ എങ്കിൽ (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	അല്ലെങ്കിൽ
	{
	    ആകുമ്പോൾ (EQN(in, "./", 2))    // Ignore Unix "current dir"
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

    എങ്കിൽ (end_of_dir != ശൂന്യ) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vms_fspec_proc(അക്ഷരം *fil, പൂർണ്ണസംഖ്യ val)
{
    strcpy(Fspec_Rms,fil);
    തിരികെ(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    ശൂന്യം *
vms_fixfilename(ശൂന്യം *instring)
{
    സ്ഥിതം അക്ഷരം		*buf = ശൂന്യ;
    സ്ഥിതം size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = strlen(instring) + 1;
    എങ്കിൽ (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * വലുപ്പം(അക്ഷരം));
    }

#ifdef DEBUG
    അക്ഷരം		 *tmpbuf = ശൂന്യ;
    tmpbuf = ALLOC_MULT(അക്ഷരം, buflen);
    strcpy(tmpbuf, instring);
#endif

    Fspec_Rms = buf;				// for decc$to_vms

    എങ്കിൽ (strchr(instring,'/') == ശൂന്യ)
	// It is already a VMS file spec
	strcpy(buf, instring);
    അല്ലെങ്കിൽ എങ്കിൽ (strchr(instring,'"') == ശൂന്യ)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	എങ്കിൽ ( (strchr(instring,'[') == ശൂന്യ) && (strchr(instring,'<') == ശൂന്യ) &&
	     (strchr(instring,']') == ശൂന്യ) && (strchr(instring,'>') == ശൂന്യ) &&
	     (strchr(instring,':') == ശൂന്യ) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	അല്ലെങ്കിൽ
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    അല്ലെങ്കിൽ
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    തിരികെ buf;
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
    ശൂന്യം
vms_remove_version(ശൂന്യം * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    പൂർണ്ണസംഖ്യ		done = 0;
    പൂർണ്ണസംഖ്യ		vdigits = 0;

    rp = (char_u *)fname+ strlen( (അക്ഷരം *)fname)- 1;    // Rightmost char
    ആകുമ്പോൾ ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	എങ്കിൽ (isdigit( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	അല്ലെങ്കിൽ
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    എങ്കിൽ (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	എങ്കിൽ (*rp == (char_u)';')
	{
	    എങ്കിൽ ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    എങ്കിൽ ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		ആകുമ്പോൾ ((done == 0) && (dp >= (char_u *)fname))
		{
		    എങ്കിൽ ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			എങ്കിൽ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (*dp == '.')
		    {                   // Possible dot delimiter
			എങ്കിൽ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
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

    തിരികെ ;
}

ഘടന typeahead_st {
    unsigned short numchars;
    unsigned അക്ഷരം  firstchar;
    unsigned അക്ഷരം  reserved0;
    unsigned long  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    പൂർണ്ണസംഖ്യ
RealWaitForChar(
    പൂർണ്ണസംഖ്യ		fd UNUSED, // always read from iochan
    long	msec,
    പൂർണ്ണസംഖ്യ		*check_for_gpm UNUSED,
    പൂർണ്ണസംഖ്യ		*interrupted)
{
    പൂർണ്ണസംഖ്യ status;
    ഘടന _generic_64 time_curr;
    ഘടന _generic_64 time_diff;
    ഘടന _generic_64 time_out;
    unsigned പൂർണ്ണസംഖ്യ convert_operation = LIB$K_DELTA_SECONDS_F;
    ദശാംശം sec =(ദശാംശം) msec/1000;

    // make sure the iochan is set
    എങ്കിൽ (!iochan)
	get_tty();

    എങ്കിൽ (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	എങ്കിൽ (status != SS$_NORMAL)
	    തിരികെ 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#എങ്കിൽ __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# അല്ലെങ്കിൽ
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#അല്ലെങ്കിൽ
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	എങ്കിൽ (status != LIB$_NORMAL)
	    തിരികെ 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	എങ്കിൽ (status != LIB$_NORMAL)
	    തിരികെ 0; // error
    }

    ആകുമ്പോൾ (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	എങ്കിൽ (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    തിരികെ 0; // error

	എങ്കിൽ (typeahead.numchars)
	    തിരികെ 1; // ready to read

	// there's nothing to read; what now?
	എങ്കിൽ (msec == 0)
	{
	    // immediate time-out; return impatiently
	    തിരികെ 0;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    തിരികെ 1; // fakeout to force a wait in vms_read()
	}
	അല്ലെങ്കിൽ
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    എങ്കിൽ (status != SS$_NORMAL)
		തിരികെ 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    എങ്കിൽ (status != LIB$_NORMAL)
		തിരികെ 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#എങ്കിൽ !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

തരനിർവചനം ഘടന
   {
   അക്ഷരം *name;
   പൂർണ്ണസംഖ്യ മൂല്യം;
   } decc_feat_t;

പൂർണ്ണസംഖ്യ vms_init_done = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (അക്ഷരം *)ശൂന്യ, 0 } };


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

സ്ഥിതം ശൂന്യം
vms_init(ശൂന്യം)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_done = 1;

    പൂർണ്ണസംഖ്യ feat_index;
    പൂർണ്ണസംഖ്യ feat_മൂല്യം;
    പൂർണ്ണസംഖ്യ feat_മൂല്യം_max;
    പൂർണ്ണസംഖ്യ feat_മൂല്യം_min;
    പൂർണ്ണസംഖ്യ i;
    പൂർണ്ണസംഖ്യ sts;

    // Loop through all items in the decc_feat_array[]
    വേണ്ടി (i = 0; decc_feat_array[i].name != ശൂന്യ; i++)
    {
	// Get the feature index
	feat_index = decc$feature_get_index(decc_feat_array[i].name);
	എങ്കിൽ (feat_index >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_മൂല്യം = decc$feature_get_മൂല്യം(feat_index, 1);
	    feat_മൂല്യം_min = decc$feature_get_മൂല്യം(feat_index, 2);
	    feat_മൂല്യം_max = decc$feature_get_മൂല്യം(feat_index, 3);

	    എങ്കിൽ ((decc_feat_array[i].മൂല്യം >= feat_മൂല്യം_min) && (decc_feat_array[i].മൂല്യം <= feat_മൂല്യം_max))
	    {
		// Valid value.  Set it if necessary
		എങ്കിൽ (feat_മൂല്യം != decc_feat_array[i].മൂല്യം)
		    sts = decc$feature_set_മൂല്യം(feat_index, 1, decc_feat_array[i].മൂല്യം);
	    }
	    അല്ലെങ്കിൽ
		// Invalid DECC feature value
		printf("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_മൂല്യം, feat_മൂല്യം_min, decc_feat_array[i].name, feat_മൂല്യം_max);
	}
	അല്ലെങ്കിൽ
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
ശൂന്യം (*സ്ഥിരം x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
സ്ഥിരം പൂർണ്ണസംഖ്യ spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
പൂർണ്ണസംഖ്യ LIB$INITIALIZE(ശൂന്യം);
# pragma extern_model strict_refdef
പൂർണ്ണസംഖ്യ dmy_lib$initialize = (പൂർണ്ണസംഖ്യ) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
