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

#અન્તર્ભાવ "vim.h"

// define _generic_64 for use in time functions
#જો !defined(VAX)
# include <gen64def.h>
#નહીંતર
// based on Alpha's gen64def.h; the file is absent on VAX
પ્રકારવ્યાખ્યા સંરચના _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	unsigned પૂર્ણાંક gen64$l_longword [2];   // ...two 32-bit values, or
	unsigned short પૂર્ણાંક gen64$w_word [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#endif

પ્રકારવ્યાખ્યા સંરચના
{
    અક્ષર	class;
    અક્ષર	type;
    short	width;
    સંઘ
    {
	સંરચના
	{
	    અક્ષર	_basic[3];
	    અક્ષર	length;
	}	y;
	પૂર્ણાંક	basic;
    }	x;
    પૂર્ણાંક		extended;
}	TT_MODE;

પ્રકારવ્યાખ્યા સંરચના
{
    short	buflen;
    short	itemcode;
    અક્ષર	*bufadrs;
    પૂર્ણાંક		*retlen;
}	ITEM;

પ્રકારવ્યાખ્યા સંરચના
{
    ITEM	equ;
    પૂર્ણાંક		nul;
}	ITMLST1;

પ્રકારવ્યાખ્યા સંરચના
{
    ITEM	index;
    ITEM	string;
    પૂર્ણાંક	nul;
}	ITMLST2;

સ્થિર TT_MODE	orgmode;
સ્થિર short	iochan;			// TTY I/O channel
સ્થિર short	iosb[4];		// IO status block

સ્થિર પૂર્ણાંક vms_match_અંક = 0;
સ્થિર પૂર્ણાંક vms_match_free = 0;
સ્થિર char_u **vms_fmatch = ખાલી;
સ્થિર અક્ષર *Fspec_Rms;		       // rms file spec, passed implicitly between routines



સ્થિર TT_MODE	get_tty(શૂન્ય);
સ્થિર શૂન્ય	set_tty(પૂર્ણાંક row, પૂર્ણાંક col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) કરો { જ્યારે (Str[1] == '/') ++Str; } જ્યારે (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    સ્થિર શૂન્ય
vul_desc(DESC *des, અક્ષર *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_pointer = str;
    des->dsc$w_length = str ? strlen(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    સ્થિર શૂન્ય
vul_item(ITEM *itm, short len, short cod, અક્ષર *adr, પૂર્ણાંક *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    શૂન્ય
mch_settmode(tmode_T tmode)
{
    પૂર્ણાંક	status;

    જો ( tmode == TMODE_RAW )
	set_tty(0, 0);
    નહીંતર
    {
	પસંદગી (orgmode.width)
	{
	    સ્થિતિ 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	તોડો;
	    સ્થિતિ 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	તોડો;
	    મૂળભૂત:	તોડો;
	}
	out_flush();
	status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, કદ(TT_MODE), 0,0,0,0);
	જો (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    પાછા;
	(શૂન્ય)sys$dassgn(iochan);
	iochan = 0;
    }
}

    સ્થિર શૂન્ય
set_tty(પૂર્ણાંક row, પૂર્ણાંક col)
{
    પૂર્ણાંક		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    સ્થિર short    first_time = TRUE;

    જો (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    જો (col)
	newmode.width		 = col;
    જો (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, કદ(newmode), 0, 0, 0, 0);
    જો (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	પાછા;
}

    સ્થિર TT_MODE
get_tty(શૂન્ય)
{

    સ્થિર $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    પૂર્ણાંક		status;
    TT_MODE	tt_mode;

    જો (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, કદ(tt_mode), 0, 0, 0, 0);
    જો (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    પાછા(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    પૂર્ણાંક
mch_get_shellsize(શૂન્ય)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    પાછા OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    શૂન્ય
mch_set_shellsize(શૂન્ય)
{
    set_tty(Rows, Columns);
    પસંદગી (Columns)
    {
	સ્થિતિ 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	તોડો;
	સ્થિતિ 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	તોડો;
	મૂળભૂત:	તોડો;
    }
    out_flush();
    screen_start();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    સ્થિર અક્ષર		buffer[LNM$C_NAMLENGTH+1];
    char_u		*cp = ખાલી;
    unsigned long	attrib;
    પૂર્ણાંક			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    અક્ષર		*sbuf = ખાલી;

    vul_desc(&d_lognam, (અક્ષર *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, કદ(પૂર્ણાંક), LNM$_INDEX, (અક્ષર *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    જો (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, ખાલી,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	જો (cp = alloc(lengte + 1))
	    strcpy((અક્ષર *)cp, buffer);
	પાછા(cp);
    }
    નહીંતર જો ((sbuf = getenv((અક્ષર *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = alloc(lengte);
	જો (cp)
	    strcpy((અક્ષર *)cp, sbuf);
	પાછા cp;
    }
    નહીંતર
	પાછા(ખાલી);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    પૂર્ણાંક
mch_setenv(અક્ષર *var, અક્ષર *કિંમત, પૂર્ણાંક x)
{
    પૂર્ણાંક		res, dum;
    long	attrib = 0L;
    અક્ષર	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, કિંમત ? strlen(કિંમત) : 0, કિંમત ? LNM$_STRING : 0,
	    કિંમત, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    પાછા((res == 1) ? 0 : -1);
}

    પૂર્ણાંક
vms_sys(અક્ષર *cmd, અક્ષર *out, અક્ષર *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status;

    જો (cmd)
	vul_desc(&cdsc, cmd);
    જો (out)
	vul_desc(&odsc, out);
    જો (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : ખાલી,		// command string
	      inp ? &idsc : ખાલી,		// input file
	      out ? &odsc : ખાલી,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    પાછા status;
}

/*
 * Convert string to lowercase - most often filename
 */
    અક્ષર *
vms_tolower( અક્ષર *name )
{
    પૂર્ણાંક i,nlen = strlen(name);
    માટે (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    પાછા name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    પૂર્ણાંક
vms_sys_status(પૂર્ણાંક status)
{
    જો (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	પાછા status;		// Command failed.
    પાછા 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    પૂર્ણાંક
vms_read(અક્ષર *inbuf, size_t nbytes)
{
    પૂર્ણાંક		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    સ્થિર long trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    જો (!iochan)
	tt_mode = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_item(&itmlst[0], 0, TRM$_MODIFIERS,
	 (અક્ષર *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_item(&itmlst[1], કદ(trm_mask), TRM$_TERM, (અક્ષર *)&trm_mask, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, કદ(itmlst));
    len = strlen(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = strlen(inbuf); // return the total length

    પાછા len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    સ્થિર પૂર્ણાંક
vms_wproc(અક્ષર *name, પૂર્ણાંક val)
{
    પૂર્ણાંક i;
    સ્થિર પૂર્ણાંક vms_match_alloced = 0;

    જો (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	પાછા 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    જો (vms_match_અંક == 0)
    {
	// first time through, setup some things
	જો (ખાલી == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    જો (!vms_fmatch)
		પાછા 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	નહીંતર
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    માટે (i = 0; i<vms_match_અંક; i++)
    {
	જો (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    પાછા 1;
    }
    જો (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		કદ(અક્ષર **) * vms_match_alloced);
	જો (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    પાછા 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_અંક] = vim_strsave((char_u *)name);

    ++vms_match_અંક;
    પાછા 1;
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
    પૂર્ણાંક
mch_expand_wildcards(
    પૂર્ણાંક		અંક_pat,
    char_u	**pat,
    પૂર્ણાંક		*અંક_file,
    char_u	***file,
    પૂર્ણાંક		flags)
{
    પૂર્ણાંક		i, cnt = 0;
    char_u	buf[MAXPATHL];
    અક્ષર       *result;
    પૂર્ણાંક		dir;
    પૂર્ણાંક files_alloced, files_free;

    *અંક_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    જો (*file == ખાલી)
    {
	*અંક_file = 0;
	પાછા FAIL;
    }
    માટે (i = 0; i < અંક_pat; i++)
    {
	// expand environment var or home dir
	જો (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	નહીંતર
	    STRCPY(buf,pat[i]);

	vms_match_અંક = 0; // reset collection counter
	result = decc$translate_vms(vms_fixfilename(buf));
	જો ( (પૂર્ણાંક) result == 0 || (પૂર્ણાંક) result == -1  )
	{
	    cnt = 0;
	}
	નહીંતર
	{
	    cnt = decc$to_vms(result, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	જો (cnt > 0)
	    cnt = vms_match_અંક;

	જો (cnt < 1)
	    ચાલુ;

	માટે (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    જો (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		ચાલુ;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    જો (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		ચાલુ;

	    // Skip files that are not executable if we check for that.
	    જો (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], ખાલી, !(flags & EW_SHELLCMD)))
		ચાલુ;

	    // allocate memory for pointers
	    જો (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, કદ(char_u **) * files_alloced);
		જો (*file == ખાલી)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *અંક_file = 0;
		    પાછા(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*અંક_file++] = vms_fmatch[i];
	}
    }
    પાછા OK;
}

    પૂર્ણાંક
mch_expandpath(garray_T *gap, char_u *path, પૂર્ણાંક flags)
{
    પૂર્ણાંક		i,cnt = 0;
    અક્ષર       *result;

    vms_match_અંક = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    result = decc$translate_vms(vms_fixfilename(path));
    જો ( (પૂર્ણાંક) result == 0 || (પૂર્ણાંક) result == -1  )
    {
	cnt = 0;
    }
    નહીંતર
    {
	cnt = decc$to_vms(result, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    જો (cnt > 0)
	cnt = vms_match_અંક;
    માટે (i = 0; i < cnt; i++)
    {
	જો (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    પાછા cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    સ્થિર શૂન્ય
vms_unix_mixed_filespec(અક્ષર *in, અક્ષર *out)
{
    અક્ષર *lastcolon;
    અક્ષર *end_of_dir;
    અક્ષર ch;
    પૂર્ણાંક len;
    અક્ષર *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(in, ':');   // find last colon
    જો (lastcolon != ખાલી)
    {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = ખાલી;	// default: no directory

    // start of directory portion
    ch = *in;
    જો ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    નહીંતર જો (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    નહીંતર
    {                                // not a special character
	જ્યારે (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    જો (strchr(in, '/') == ખાલી)     // any more Unix directories ?
    {
	strcpy(out, in);             // No - get rest of the spec
	પાછા;
    }
    નહીંતર
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

    જ્યારે (*in != '\0')
    {
	ch = *in;
	જો ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	નહીંતર જો (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	નહીંતર
	{
	    જ્યારે (EQN(in, "./", 2))    // Ignore Unix "current dir"
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

    જો (end_of_dir != ખાલી) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    સ્થિર પૂર્ણાંક
vms_fspec_proc(અક્ષર *fil, પૂર્ણાંક val)
{
    strcpy(Fspec_Rms,fil);
    પાછા(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    શૂન્ય *
vms_fixfilename(શૂન્ય *instring)
{
    સ્થિર અક્ષર		*buf = ખાલી;
    સ્થિર size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = strlen(instring) + 1;
    જો (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * કદ(અક્ષર));
    }

#ifdef DEBUG
    અક્ષર		 *tmpbuf = ખાલી;
    tmpbuf = ALLOC_MULT(અક્ષર, buflen);
    strcpy(tmpbuf, instring);
#endif

    Fspec_Rms = buf;				// for decc$to_vms

    જો (strchr(instring,'/') == ખાલી)
	// It is already a VMS file spec
	strcpy(buf, instring);
    નહીંતર જો (strchr(instring,'"') == ખાલી)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	જો ( (strchr(instring,'[') == ખાલી) && (strchr(instring,'<') == ખાલી) &&
	     (strchr(instring,']') == ખાલી) && (strchr(instring,'>') == ખાલી) &&
	     (strchr(instring,':') == ખાલી) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	નહીંતર
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    નહીંતર
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    પાછા buf;
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
    શૂન્ય
vms_remove_version(શૂન્ય * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    પૂર્ણાંક		done = 0;
    પૂર્ણાંક		vdigits = 0;

    rp = (char_u *)fname+ strlen( (અક્ષર *)fname)- 1;    // Rightmost char
    જ્યારે ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	જો (isdigit( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	નહીંતર
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    જો (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	જો (*rp == (char_u)';')
	{
	    જો ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	નહીંતર જો (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    જો ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		જ્યારે ((done == 0) && (dp >= (char_u *)fname))
		{
		    જો ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			જો ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    નહીંતર જો (*dp == '.')
		    {                   // Possible dot delimiter
			જો ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
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

    પાછા ;
}

સંરચના typeahead_st {
    unsigned short numchars;
    unsigned અક્ષર  firstchar;
    unsigned અક્ષર  reserved0;
    unsigned long  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    પૂર્ણાંક
RealWaitForChar(
    પૂર્ણાંક		fd UNUSED, // always read from iochan
    long	msec,
    પૂર્ણાંક		*check_for_gpm UNUSED,
    પૂર્ણાંક		*interrupted)
{
    પૂર્ણાંક status;
    સંરચના _generic_64 time_curr;
    સંરચના _generic_64 time_diff;
    સંરચના _generic_64 time_out;
    unsigned પૂર્ણાંક convert_operation = LIB$K_DELTA_SECONDS_F;
    દશાંશ sec =(દશાંશ) msec/1000;

    // make sure the iochan is set
    જો (!iochan)
	get_tty();

    જો (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	જો (status != SS$_NORMAL)
	    પાછા 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#જો __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# નહીંતર
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#નહીંતર
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	જો (status != LIB$_NORMAL)
	    પાછા 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	જો (status != LIB$_NORMAL)
	    પાછા 0; // error
    }

    જ્યારે (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	જો (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    પાછા 0; // error

	જો (typeahead.numchars)
	    પાછા 1; // ready to read

	// there's nothing to read; what now?
	જો (msec == 0)
	{
	    // immediate time-out; return impatiently
	    પાછા 0;
	}
	નહીંતર જો (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    પાછા 1; // fakeout to force a wait in vms_read()
	}
	નહીંતર
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    જો (status != SS$_NORMAL)
		પાછા 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    જો (status != LIB$_NORMAL)
		પાછા 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#જો !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

પ્રકારવ્યાખ્યા સંરચના
   {
   અક્ષર *name;
   પૂર્ણાંક કિંમત;
   } decc_feat_t;

પૂર્ણાંક vms_init_done = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (અક્ષર *)ખાલી, 0 } };


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

સ્થિર શૂન્ય
vms_init(શૂન્ય)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_done = 1;

    પૂર્ણાંક feat_index;
    પૂર્ણાંક feat_કિંમત;
    પૂર્ણાંક feat_કિંમત_max;
    પૂર્ણાંક feat_કિંમત_min;
    પૂર્ણાંક i;
    પૂર્ણાંક sts;

    // Loop through all items in the decc_feat_array[]
    માટે (i = 0; decc_feat_array[i].name != ખાલી; i++)
    {
	// Get the feature index
	feat_index = decc$feature_get_index(decc_feat_array[i].name);
	જો (feat_index >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_કિંમત = decc$feature_get_કિંમત(feat_index, 1);
	    feat_કિંમત_min = decc$feature_get_કિંમત(feat_index, 2);
	    feat_કિંમત_max = decc$feature_get_કિંમત(feat_index, 3);

	    જો ((decc_feat_array[i].કિંમત >= feat_કિંમત_min) && (decc_feat_array[i].કિંમત <= feat_કિંમત_max))
	    {
		// Valid value.  Set it if necessary
		જો (feat_કિંમત != decc_feat_array[i].કિંમત)
		    sts = decc$feature_set_કિંમત(feat_index, 1, decc_feat_array[i].કિંમત);
	    }
	    નહીંતર
		// Invalid DECC feature value
		printf("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_કિંમત, feat_કિંમત_min, decc_feat_array[i].name, feat_કિંમત_max);
	}
	નહીંતર
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
શૂન્ય (*અચલ x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
અચલ પૂર્ણાંક spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
પૂર્ણાંક LIB$INITIALIZE(શૂન્ય);
# pragma extern_model strict_refdef
પૂર્ણાંક dmy_lib$initialize = (પૂર્ણાંક) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
