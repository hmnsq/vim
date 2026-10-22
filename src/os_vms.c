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

#ಅನ್ತರ್ಭಾವ "vim.h"

// define _generic_64 for use in time functions
#ಯದಿ !defined(VAX)
# include <gen64def.h>
#ಅನ್ಯಥಾ
// based on Alpha's gen64def.h; the file is absent on VAX
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	unsigned ಪೂರ್ಣಾಙ್ಕ gen64$l_longword [2];   // ...two 32-bit values, or
	unsigned short ಪೂರ್ಣಾಙ್ಕ gen64$w_word [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ವರ್ಣ	class;
    ವರ್ಣ	type;
    short	width;
    ಸಙ್ಘ
    {
	ಸಂರಚನಾ
	{
	    ವರ್ಣ	_basic[3];
	    ವರ್ಣ	length;
	}	y;
	ಪೂರ್ಣಾಙ್ಕ	basic;
    }	x;
    ಪೂರ್ಣಾಙ್ಕ		extended;
}	TT_MODE;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    short	buflen;
    short	itemcode;
    ವರ್ಣ	*bufadrs;
    ಪೂರ್ಣಾಙ್ಕ		*retlen;
}	ITEM;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ITEM	equ;
    ಪೂರ್ಣಾಙ್ಕ		nul;
}	ITMLST1;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ITEM	index;
    ITEM	string;
    ಪೂರ್ಣಾಙ್ಕ	nul;
}	ITMLST2;

ಸ್ಥಿರ TT_MODE	orgmode;
ಸ್ಥಿರ short	iochan;			// TTY I/O channel
ಸ್ಥಿರ short	iosb[4];		// IO status block

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vms_match_ಅಂಕೆ = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vms_match_free = 0;
ಸ್ಥಿರ char_u **vms_fmatch = ರಿಕ್ತ;
ಸ್ಥಿರ ವರ್ಣ *Fspec_Rms;		       // rms file spec, passed implicitly between routines



ಸ್ಥಿರ TT_MODE	get_tty(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	set_tty(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) ಕುರು { ಯಾವತ್ (Str[1] == '/') ++Str; } ಯಾವತ್ (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vul_desc(DESC *des, ವರ್ಣ *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_pointer = str;
    des->dsc$w_length = str ? strlen(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vul_item(ITEM *itm, short len, short cod, ವರ್ಣ *adr, ಪೂರ್ಣಾಙ್ಕ *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    ಶೂನ್ಯ
mch_settmode(tmode_T tmode)
{
    ಪೂರ್ಣಾಙ್ಕ	status;

    ಯದಿ ( tmode == TMODE_RAW )
	set_tty(0, 0);
    ಅನ್ಯಥಾ
    {
	ವಿಕಲ್ಪ (orgmode.width)
	{
	    ಪಕ್ಷ 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	ಭಙ್ಗ;
	    ಪಕ್ಷ 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	ಭಙ್ಗ;
	    ಸ್ವಭಾವ:	ಭಙ್ಗ;
	}
	out_flush();
	status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, ಪರಿಮಾಣ(TT_MODE), 0,0,0,0);
	ಯದಿ (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    ಪ್ರತ್ಯಾಗಮ;
	(ಶೂನ್ಯ)sys$dassgn(iochan);
	iochan = 0;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
set_tty(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಪೂರ್ಣಾಙ್ಕ		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    ಸ್ಥಿರ short    first_time = TRUE;

    ಯದಿ (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    ಯದಿ (col)
	newmode.width		 = col;
    ಯದಿ (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, ಪರಿಮಾಣ(newmode), 0, 0, 0, 0);
    ಯದಿ (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	ಪ್ರತ್ಯಾಗಮ;
}

    ಸ್ಥಿರ TT_MODE
get_tty(ಶೂನ್ಯ)
{

    ಸ್ಥಿರ $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    ಪೂರ್ಣಾಙ್ಕ		status;
    TT_MODE	tt_mode;

    ಯದಿ (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, ಪರಿಮಾಣ(tt_mode), 0, 0, 0, 0);
    ಯದಿ (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    ಪ್ರತ್ಯಾಗಮ(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_get_shellsize(ಶೂನ್ಯ)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    ಶೂನ್ಯ
mch_set_shellsize(ಶೂನ್ಯ)
{
    set_tty(Rows, Columns);
    ವಿಕಲ್ಪ (Columns)
    {
	ಪಕ್ಷ 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	ಭಙ್ಗ;
	ಪಕ್ಷ 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	ಭಙ್ಗ;
	ಸ್ವಭಾವ:	ಭಙ್ಗ;
    }
    out_flush();
    screen_start();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    ಸ್ಥಿರ ವರ್ಣ		buffer[LNM$C_NAMLENGTH+1];
    char_u		*cp = ರಿಕ್ತ;
    unsigned long	attrib;
    ಪೂರ್ಣಾಙ್ಕ			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    ವರ್ಣ		*sbuf = ರಿಕ್ತ;

    vul_desc(&d_lognam, (ವರ್ಣ *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ), LNM$_INDEX, (ವರ್ಣ *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    ಯದಿ (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, ರಿಕ್ತ,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	ಯದಿ (cp = alloc(lengte + 1))
	    strcpy((ವರ್ಣ *)cp, buffer);
	ಪ್ರತ್ಯಾಗಮ(cp);
    }
    ಅನ್ಯಥಾ ಯದಿ ((sbuf = getenv((ವರ್ಣ *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = alloc(lengte);
	ಯದಿ (cp)
	    strcpy((ವರ್ಣ *)cp, sbuf);
	ಪ್ರತ್ಯಾಗಮ cp;
    }
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ(ರಿಕ್ತ);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_setenv(ವರ್ಣ *var, ವರ್ಣ *ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ x)
{
    ಪೂರ್ಣಾಙ್ಕ		res, dum;
    long	attrib = 0L;
    ವರ್ಣ	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, ಮೌಲ್ಯ ? strlen(ಮೌಲ್ಯ) : 0, ಮೌಲ್ಯ ? LNM$_STRING : 0,
	    ಮೌಲ್ಯ, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    ಪ್ರತ್ಯಾಗಮ((res == 1) ? 0 : -1);
}

    ಪೂರ್ಣಾಙ್ಕ
vms_sys(ವರ್ಣ *cmd, ವರ್ಣ *out, ವರ್ಣ *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status;

    ಯದಿ (cmd)
	vul_desc(&cdsc, cmd);
    ಯದಿ (out)
	vul_desc(&odsc, out);
    ಯದಿ (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : ರಿಕ್ತ,		// command string
	      inp ? &idsc : ರಿಕ್ತ,		// input file
	      out ? &odsc : ರಿಕ್ತ,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Convert string to lowercase - most often filename
 */
    ವರ್ಣ *
vms_tolower( ವರ್ಣ *name )
{
    ಪೂರ್ಣಾಙ್ಕ i,nlen = strlen(name);
    ಪ್ರತಿ (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    ಪ್ರತ್ಯಾಗಮ name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    ಪೂರ್ಣಾಙ್ಕ
vms_sys_status(ಪೂರ್ಣಾಙ್ಕ status)
{
    ಯದಿ (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	ಪ್ರತ್ಯಾಗಮ status;		// Command failed.
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    ಪೂರ್ಣಾಙ್ಕ
vms_read(ವರ್ಣ *inbuf, size_t nbytes)
{
    ಪೂರ್ಣಾಙ್ಕ		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    ಸ್ಥಿರ long trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    ಯದಿ (!iochan)
	tt_mode = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_item(&itmlst[0], 0, TRM$_MODIFIERS,
	 (ವರ್ಣ *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_item(&itmlst[1], ಪರಿಮಾಣ(trm_mask), TRM$_TERM, (ವರ್ಣ *)&trm_mask, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, ಪರಿಮಾಣ(itmlst));
    len = strlen(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = strlen(inbuf); // return the total length

    ಪ್ರತ್ಯಾಗಮ len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vms_wproc(ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ val)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vms_match_alloced = 0;

    ಯದಿ (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	ಪ್ರತ್ಯಾಗಮ 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    ಯದಿ (vms_match_ಅಂಕೆ == 0)
    {
	// first time through, setup some things
	ಯದಿ (ರಿಕ್ತ == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    ಯದಿ (!vms_fmatch)
		ಪ್ರತ್ಯಾಗಮ 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	ಅನ್ಯಥಾ
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    ಪ್ರತಿ (i = 0; i<vms_match_ಅಂಕೆ; i++)
    {
	ಯದಿ (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಯದಿ (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		ಪರಿಮಾಣ(ವರ್ಣ **) * vms_match_alloced);
	ಯದಿ (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_ಅಂಕೆ] = vim_strsave((char_u *)name);

    ++vms_match_ಅಂಕೆ;
    ಪ್ರತ್ಯಾಗಮ 1;
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
    ಪೂರ್ಣಾಙ್ಕ
mch_expand_wildcards(
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_pat,
    char_u	**pat,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_file,
    char_u	***file,
    ಪೂರ್ಣಾಙ್ಕ		flags)
{
    ಪೂರ್ಣಾಙ್ಕ		i, cnt = 0;
    char_u	buf[MAXPATHL];
    ವರ್ಣ       *result;
    ಪೂರ್ಣಾಙ್ಕ		dir;
    ಪೂರ್ಣಾಙ್ಕ files_alloced, files_free;

    *ಅಂಕೆ_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    ಯದಿ (*file == ರಿಕ್ತ)
    {
	*ಅಂಕೆ_file = 0;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_pat; i++)
    {
	// expand environment var or home dir
	ಯದಿ (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	ಅನ್ಯಥಾ
	    STRCPY(buf,pat[i]);

	vms_match_ಅಂಕೆ = 0; // reset collection counter
	result = decc$translate_vms(vms_fixfilename(buf));
	ಯದಿ ( (ಪೂರ್ಣಾಙ್ಕ) result == 0 || (ಪೂರ್ಣಾಙ್ಕ) result == -1  )
	{
	    cnt = 0;
	}
	ಅನ್ಯಥಾ
	{
	    cnt = decc$to_vms(result, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	ಯದಿ (cnt > 0)
	    cnt = vms_match_ಅಂಕೆ;

	ಯದಿ (cnt < 1)
	    ಅನುವರ್ತ;

	ಪ್ರತಿ (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    ಯದಿ (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		ಅನುವರ್ತ;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    ಯದಿ (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		ಅನುವರ್ತ;

	    // Skip files that are not executable if we check for that.
	    ಯದಿ (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], ರಿಕ್ತ, !(flags & EW_SHELLCMD)))
		ಅನುವರ್ತ;

	    // allocate memory for pointers
	    ಯದಿ (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, ಪರಿಮಾಣ(char_u **) * files_alloced);
		ಯದಿ (*file == ರಿಕ್ತ)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *ಅಂಕೆ_file = 0;
		    ಪ್ರತ್ಯಾಗಮ(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*ಅಂಕೆ_file++] = vms_fmatch[i];
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
mch_expandpath(garray_T *gap, char_u *path, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಪೂರ್ಣಾಙ್ಕ		i,cnt = 0;
    ವರ್ಣ       *result;

    vms_match_ಅಂಕೆ = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    result = decc$translate_vms(vms_fixfilename(path));
    ಯದಿ ( (ಪೂರ್ಣಾಙ್ಕ) result == 0 || (ಪೂರ್ಣಾಙ್ಕ) result == -1  )
    {
	cnt = 0;
    }
    ಅನ್ಯಥಾ
    {
	cnt = decc$to_vms(result, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    ಯದಿ (cnt > 0)
	cnt = vms_match_ಅಂಕೆ;
    ಪ್ರತಿ (i = 0; i < cnt; i++)
    {
	ಯದಿ (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    ಪ್ರತ್ಯಾಗಮ cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vms_unix_mixed_filespec(ವರ್ಣ *in, ವರ್ಣ *out)
{
    ವರ್ಣ *lastcolon;
    ವರ್ಣ *end_of_dir;
    ವರ್ಣ ch;
    ಪೂರ್ಣಾಙ್ಕ len;
    ವರ್ಣ *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(in, ':');   // find last colon
    ಯದಿ (lastcolon != ರಿಕ್ತ)
    {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = ರಿಕ್ತ;	// default: no directory

    // start of directory portion
    ch = *in;
    ಯದಿ ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    ಅನ್ಯಥಾ ಯದಿ (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    ಅನ್ಯಥಾ
    {                                // not a special character
	ಯಾವತ್ (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    ಯದಿ (strchr(in, '/') == ರಿಕ್ತ)     // any more Unix directories ?
    {
	strcpy(out, in);             // No - get rest of the spec
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ
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

    ಯಾವತ್ (*in != '\0')
    {
	ch = *in;
	ಯದಿ ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	ಅನ್ಯಥಾ ಯದಿ (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	ಅನ್ಯಥಾ
	{
	    ಯಾವತ್ (EQN(in, "./", 2))    // Ignore Unix "current dir"
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

    ಯದಿ (end_of_dir != ರಿಕ್ತ) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vms_fspec_proc(ವರ್ಣ *fil, ಪೂರ್ಣಾಙ್ಕ val)
{
    strcpy(Fspec_Rms,fil);
    ಪ್ರತ್ಯಾಗಮ(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    ಶೂನ್ಯ *
vms_fixfilename(ಶೂನ್ಯ *instring)
{
    ಸ್ಥಿರ ವರ್ಣ		*buf = ರಿಕ್ತ;
    ಸ್ಥಿರ size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = strlen(instring) + 1;
    ಯದಿ (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * ಪರಿಮಾಣ(ವರ್ಣ));
    }

#ifdef DEBUG
    ವರ್ಣ		 *tmpbuf = ರಿಕ್ತ;
    tmpbuf = ALLOC_MULT(ವರ್ಣ, buflen);
    strcpy(tmpbuf, instring);
#endif

    Fspec_Rms = buf;				// for decc$to_vms

    ಯದಿ (strchr(instring,'/') == ರಿಕ್ತ)
	// It is already a VMS file spec
	strcpy(buf, instring);
    ಅನ್ಯಥಾ ಯದಿ (strchr(instring,'"') == ರಿಕ್ತ)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	ಯದಿ ( (strchr(instring,'[') == ರಿಕ್ತ) && (strchr(instring,'<') == ರಿಕ್ತ) &&
	     (strchr(instring,']') == ರಿಕ್ತ) && (strchr(instring,'>') == ರಿಕ್ತ) &&
	     (strchr(instring,':') == ರಿಕ್ತ) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	ಅನ್ಯಥಾ
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    ಅನ್ಯಥಾ
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    ಪ್ರತ್ಯಾಗಮ buf;
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
    ಶೂನ್ಯ
vms_remove_version(ಶೂನ್ಯ * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    ಪೂರ್ಣಾಙ್ಕ		done = 0;
    ಪೂರ್ಣಾಙ್ಕ		vdigits = 0;

    rp = (char_u *)fname+ strlen( (ವರ್ಣ *)fname)- 1;    // Rightmost char
    ಯಾವತ್ ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	ಯದಿ (isdigit( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	ಅನ್ಯಥಾ
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    ಯದಿ (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	ಯದಿ (*rp == (char_u)';')
	{
	    ಯದಿ ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    ಯದಿ ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		ಯಾವತ್ ((done == 0) && (dp >= (char_u *)fname))
		{
		    ಯದಿ ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			ಯದಿ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    ಅನ್ಯಥಾ ಯದಿ (*dp == '.')
		    {                   // Possible dot delimiter
			ಯದಿ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
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

    ಪ್ರತ್ಯಾಗಮ ;
}

ಸಂರಚನಾ typeahead_st {
    unsigned short numchars;
    unsigned ವರ್ಣ  firstchar;
    unsigned ವರ್ಣ  reserved0;
    unsigned long  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    ಪೂರ್ಣಾಙ್ಕ
RealWaitForChar(
    ಪೂರ್ಣಾಙ್ಕ		fd UNUSED, // always read from iochan
    long	msec,
    ಪೂರ್ಣಾಙ್ಕ		*check_for_gpm UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		*interrupted)
{
    ಪೂರ್ಣಾಙ್ಕ status;
    ಸಂರಚನಾ _generic_64 time_curr;
    ಸಂರಚನಾ _generic_64 time_diff;
    ಸಂರಚನಾ _generic_64 time_out;
    unsigned ಪೂರ್ಣಾಙ್ಕ convert_operation = LIB$K_DELTA_SECONDS_F;
    ಭಿನ್ನ sec =(ಭಿನ್ನ) msec/1000;

    // make sure the iochan is set
    ಯದಿ (!iochan)
	get_tty();

    ಯದಿ (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	ಯದಿ (status != SS$_NORMAL)
	    ಪ್ರತ್ಯಾಗಮ 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#ಯದಿ __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# ಅನ್ಯಥಾ
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#ಅನ್ಯಥಾ
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	ಯದಿ (status != LIB$_NORMAL)
	    ಪ್ರತ್ಯಾಗಮ 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	ಯದಿ (status != LIB$_NORMAL)
	    ಪ್ರತ್ಯಾಗಮ 0; // error
    }

    ಯಾವತ್ (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	ಯದಿ (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    ಪ್ರತ್ಯಾಗಮ 0; // error

	ಯದಿ (typeahead.numchars)
	    ಪ್ರತ್ಯಾಗಮ 1; // ready to read

	// there's nothing to read; what now?
	ಯದಿ (msec == 0)
	{
	    // immediate time-out; return impatiently
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    ಪ್ರತ್ಯಾಗಮ 1; // fakeout to force a wait in vms_read()
	}
	ಅನ್ಯಥಾ
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    ಯದಿ (status != SS$_NORMAL)
		ಪ್ರತ್ಯಾಗಮ 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    ಯದಿ (status != LIB$_NORMAL)
		ಪ್ರತ್ಯಾಗಮ 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#ಯದಿ !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
   {
   ವರ್ಣ *name;
   ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ;
   } decc_feat_t;

ಪೂರ್ಣಾಙ್ಕ vms_init_done = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (ವರ್ಣ *)ರಿಕ್ತ, 0 } };


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

ಸ್ಥಿರ ಶೂನ್ಯ
vms_init(ಶೂನ್ಯ)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_done = 1;

    ಪೂರ್ಣಾಙ್ಕ feat_index;
    ಪೂರ್ಣಾಙ್ಕ feat_ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಙ್ಕ feat_ಮೌಲ್ಯ_max;
    ಪೂರ್ಣಾಙ್ಕ feat_ಮೌಲ್ಯ_min;
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪೂರ್ಣಾಙ್ಕ sts;

    // Loop through all items in the decc_feat_array[]
    ಪ್ರತಿ (i = 0; decc_feat_array[i].name != ರಿಕ್ತ; i++)
    {
	// Get the feature index
	feat_index = decc$feature_get_index(decc_feat_array[i].name);
	ಯದಿ (feat_index >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_ಮೌಲ್ಯ = decc$feature_get_ಮೌಲ್ಯ(feat_index, 1);
	    feat_ಮೌಲ್ಯ_min = decc$feature_get_ಮೌಲ್ಯ(feat_index, 2);
	    feat_ಮೌಲ್ಯ_max = decc$feature_get_ಮೌಲ್ಯ(feat_index, 3);

	    ಯದಿ ((decc_feat_array[i].ಮೌಲ್ಯ >= feat_ಮೌಲ್ಯ_min) && (decc_feat_array[i].ಮೌಲ್ಯ <= feat_ಮೌಲ್ಯ_max))
	    {
		// Valid value.  Set it if necessary
		ಯದಿ (feat_ಮೌಲ್ಯ != decc_feat_array[i].ಮೌಲ್ಯ)
		    sts = decc$feature_set_ಮೌಲ್ಯ(feat_index, 1, decc_feat_array[i].ಮೌಲ್ಯ);
	    }
	    ಅನ್ಯಥಾ
		// Invalid DECC feature value
		printf("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_ಮೌಲ್ಯ, feat_ಮೌಲ್ಯ_min, decc_feat_array[i].name, feat_ಮೌಲ್ಯ_max);
	}
	ಅನ್ಯಥಾ
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
ಶೂನ್ಯ (*ಧ್ರುವ x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
ಪೂರ್ಣಾಙ್ಕ LIB$INITIALIZE(ಶೂನ್ಯ);
# pragma extern_model strict_refdef
ಪೂರ್ಣಾಙ್ಕ dmy_lib$initialize = (ಪೂರ್ಣಾಙ್ಕ) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
