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

#include "vim.h"

// define _generic_64 for use in time functions
#ꦪꦼꦤ꧀ !defined(VAX)
# include <gen64def.h>
#ꦭꦶꦪ
// based on Alpha's gen64def.h; the file is absent on VAX
typedef sa_nracanaa _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	unsigned puur_naa_nka gen64$l_longword [2];   // ...two 32-bit values, or
	unsigned short puur_naa_nka gen64$w_word [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#endif

typedef sa_nracanaa
{
    var_na	class;
    var_na	type;
    short	width;
    sa_ngha
    {
	sa_nracanaa
	{
	    var_na	_basic[3];
	    var_na	length;
	}	y;
	puur_naa_nka	basic;
    }	x;
    puur_naa_nka		extended;
}	TT_MODE;

typedef sa_nracanaa
{
    short	buflen;
    short	itemcode;
    var_na	*bufadrs;
    puur_naa_nka		*retlen;
}	ITEM;

typedef sa_nracanaa
{
    ITEM	equ;
    puur_naa_nka		nul;
}	ITMLST1;

typedef sa_nracanaa
{
    ITEM	index;
    ITEM	string;
    puur_naa_nka	nul;
}	ITMLST2;

static TT_MODE	orgmode;
static short	iochan;			// TTY I/O channel
static short	iosb[4];		// IO status block

static puur_naa_nka vms_match_num = 0;
static puur_naa_nka vms_match_free = 0;
static char_u **vms_fmatch = NULL;
static var_na *Fspec_Rms;		       // rms file spec, passed implicitly between routines



static TT_MODE	get_tty(void);
static void	set_tty(puur_naa_nka row, puur_naa_nka col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) do { ꦱꦼꦢꦼꦭ (Str[1] == '/') ++Str; } ꦱꦼꦢꦼꦭ (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    static void
vul_desc(DESC *des, var_na *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_pointer = str;
    des->dsc$w_length = str ? strlen(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    static void
vul_item(ITEM *itm, short len, short cod, var_na *adr, puur_naa_nka *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    void
mch_settmode(tmode_T tmode)
{
    puur_naa_nka	status;

    ꦪꦼꦤ꧀ ( tmode == TMODE_RAW )
	set_tty(0, 0);
    ꦭꦶꦪ
    {
	switch (orgmode.width)
	{
	    pak_sa 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	bha_nga;
	    pak_sa 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	bha_nga;
	    default:	bha_nga;
	}
	out_flush();
	status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, parimaa_na(TT_MODE), 0,0,0,0);
	ꦪꦼꦤ꧀ (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    ꦮꦭꦶ;
	(void)sys$dassgn(iochan);
	iochan = 0;
    }
}

    static void
set_tty(puur_naa_nka row, puur_naa_nka col)
{
    puur_naa_nka		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    static short    first_time = TRUE;

    ꦪꦼꦤ꧀ (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    ꦪꦼꦤ꧀ (col)
	newmode.width		 = col;
    ꦪꦼꦤ꧀ (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, parimaa_na(newmode), 0, 0, 0, 0);
    ꦪꦼꦤ꧀ (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	ꦮꦭꦶ;
}

    static TT_MODE
get_tty(void)
{

    static $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    puur_naa_nka		status;
    TT_MODE	tt_mode;

    ꦪꦼꦤ꧀ (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, parimaa_na(tt_mode), 0, 0, 0, 0);
    ꦪꦼꦤ꧀ (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    ꦮꦭꦶ(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    puur_naa_nka
mch_get_shellsize(void)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    ꦮꦭꦶ OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    void
mch_set_shellsize(void)
{
    set_tty(Rows, Columns);
    switch (Columns)
    {
	pak_sa 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	bha_nga;
	pak_sa 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	bha_nga;
	default:	bha_nga;
    }
    out_flush();
    screen_start();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    static var_na		buffer[LNM$C_NAMLENGTH+1];
    char_u		*cp = NULL;
    unsigned long	attrib;
    puur_naa_nka			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    var_na		*sbuf = NULL;

    vul_desc(&d_lognam, (var_na *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, parimaa_na(puur_naa_nka), LNM$_INDEX, (var_na *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    ꦪꦼꦤ꧀ (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, NULL,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	ꦪꦼꦤ꧀ (cp = alloc(lengte + 1))
	    strcpy((var_na *)cp, buffer);
	ꦮꦭꦶ(cp);
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ ((sbuf = getenv((var_na *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = alloc(lengte);
	ꦪꦼꦤ꧀ (cp)
	    strcpy((var_na *)cp, sbuf);
	ꦮꦭꦶ cp;
    }
    ꦭꦶꦪ
	ꦮꦭꦶ(NULL);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    puur_naa_nka
mch_setenv(var_na *var, var_na *value, puur_naa_nka x)
{
    puur_naa_nka		res, dum;
    long	attrib = 0L;
    var_na	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, value ? strlen(value) : 0, value ? LNM$_STRING : 0,
	    value, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    ꦮꦭꦶ((res == 1) ? 0 : -1);
}

    puur_naa_nka
vms_sys(var_na *cmd, var_na *out, var_na *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status;

    ꦪꦼꦤ꧀ (cmd)
	vul_desc(&cdsc, cmd);
    ꦪꦼꦤ꧀ (out)
	vul_desc(&odsc, out);
    ꦪꦼꦤ꧀ (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : NULL,		// command string
	      inp ? &idsc : NULL,		// input file
	      out ? &odsc : NULL,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    ꦮꦭꦶ status;
}

/*
 * Convert string to lowercase - most often filename
 */
    var_na *
vms_tolower( var_na *name )
{
    puur_naa_nka i,nlen = strlen(name);
    ꦏꦁꦒꦺꦴ (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    ꦮꦭꦶ name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    puur_naa_nka
vms_sys_status(puur_naa_nka status)
{
    ꦪꦼꦤ꧀ (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	ꦮꦭꦶ status;		// Command failed.
    ꦮꦭꦶ 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    puur_naa_nka
vms_read(var_na *inbuf, size_t nbytes)
{
    puur_naa_nka		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    static long trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    ꦪꦼꦤ꧀ (!iochan)
	tt_mode = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_item(&itmlst[0], 0, TRM$_MODIFIERS,
	 (var_na *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_item(&itmlst[1], parimaa_na(trm_mask), TRM$_TERM, (var_na *)&trm_mask, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, parimaa_na(itmlst));
    len = strlen(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = strlen(inbuf); // return the total length

    ꦮꦭꦶ len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    static puur_naa_nka
vms_wproc(var_na *name, puur_naa_nka val)
{
    puur_naa_nka i;
    static puur_naa_nka vms_match_alloced = 0;

    ꦪꦼꦤ꧀ (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	ꦮꦭꦶ 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    ꦪꦼꦤ꧀ (vms_match_num == 0)
    {
	// first time through, setup some things
	ꦪꦼꦤ꧀ (NULL == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    ꦪꦼꦤ꧀ (!vms_fmatch)
		ꦮꦭꦶ 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	ꦭꦶꦪ
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    ꦏꦁꦒꦺꦴ (i = 0; i<vms_match_num; i++)
    {
	ꦪꦼꦤ꧀ (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    ꦮꦭꦶ 1;
    }
    ꦪꦼꦤ꧀ (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		parimaa_na(var_na **) * vms_match_alloced);
	ꦪꦼꦤ꧀ (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    ꦮꦭꦶ 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_num] = vim_strsave((char_u *)name);

    ++vms_match_num;
    ꦮꦭꦶ 1;
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
    puur_naa_nka
mch_expand_wildcards(
    puur_naa_nka		num_pat,
    char_u	**pat,
    puur_naa_nka		*num_file,
    char_u	***file,
    puur_naa_nka		flags)
{
    puur_naa_nka		i, cnt = 0;
    char_u	buf[MAXPATHL];
    var_na       *result;
    puur_naa_nka		dir;
    puur_naa_nka files_alloced, files_free;

    *num_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    ꦪꦼꦤ꧀ (*file == NULL)
    {
	*num_file = 0;
	ꦮꦭꦶ FAIL;
    }
    ꦏꦁꦒꦺꦴ (i = 0; i < num_pat; i++)
    {
	// expand environment var or home dir
	ꦪꦼꦤ꧀ (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	ꦭꦶꦪ
	    STRCPY(buf,pat[i]);

	vms_match_num = 0; // reset collection counter
	result = decc$translate_vms(vms_fixfilename(buf));
	ꦪꦼꦤ꧀ ( (puur_naa_nka) result == 0 || (puur_naa_nka) result == -1  )
	{
	    cnt = 0;
	}
	ꦭꦶꦪ
	{
	    cnt = decc$to_vms(result, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	ꦪꦼꦤ꧀ (cnt > 0)
	    cnt = vms_match_num;

	ꦪꦼꦤ꧀ (cnt < 1)
	    continue;

	ꦏꦁꦒꦺꦴ (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    ꦪꦼꦤ꧀ (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		continue;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    ꦪꦼꦤ꧀ (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		continue;

	    // Skip files that are not executable if we check for that.
	    ꦪꦼꦤ꧀ (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], NULL, !(flags & EW_SHELLCMD)))
		continue;

	    // allocate memory for pointers
	    ꦪꦼꦤ꧀ (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, parimaa_na(char_u **) * files_alloced);
		ꦪꦼꦤ꧀ (*file == NULL)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *num_file = 0;
		    ꦮꦭꦶ(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*num_file++] = vms_fmatch[i];
	}
    }
    ꦮꦭꦶ OK;
}

    puur_naa_nka
mch_expandpath(garray_T *gap, char_u *path, puur_naa_nka flags)
{
    puur_naa_nka		i,cnt = 0;
    var_na       *result;

    vms_match_num = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    result = decc$translate_vms(vms_fixfilename(path));
    ꦪꦼꦤ꧀ ( (puur_naa_nka) result == 0 || (puur_naa_nka) result == -1  )
    {
	cnt = 0;
    }
    ꦭꦶꦪ
    {
	cnt = decc$to_vms(result, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    ꦪꦼꦤ꧀ (cnt > 0)
	cnt = vms_match_num;
    ꦏꦁꦒꦺꦴ (i = 0; i < cnt; i++)
    {
	ꦪꦼꦤ꧀ (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    ꦮꦭꦶ cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    static void
vms_unix_mixed_filespec(var_na *in, var_na *out)
{
    var_na *lastcolon;
    var_na *end_of_dir;
    var_na ch;
    puur_naa_nka len;
    var_na *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(in, ':');   // find last colon
    ꦪꦼꦤ꧀ (lastcolon != NULL)
    {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = NULL;	// default: no directory

    // start of directory portion
    ch = *in;
    ꦪꦼꦤ꧀ ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    ꦭꦶꦪ
    {                                // not a special character
	ꦱꦼꦢꦼꦭ (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    ꦪꦼꦤ꧀ (strchr(in, '/') == NULL)     // any more Unix directories ?
    {
	strcpy(out, in);             // No - get rest of the spec
	ꦮꦭꦶ;
    }
    ꦭꦶꦪ
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

    ꦱꦼꦢꦼꦭ (*in != '\0')
    {
	ch = *in;
	ꦪꦼꦤ꧀ ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	ꦭꦶꦪ ꦪꦼꦤ꧀ (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	ꦭꦶꦪ
	{
	    ꦱꦼꦢꦼꦭ (EQN(in, "./", 2))    // Ignore Unix "current dir"
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

    ꦪꦼꦤ꧀ (end_of_dir != NULL) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    static puur_naa_nka
vms_fspec_proc(var_na *fil, puur_naa_nka val)
{
    strcpy(Fspec_Rms,fil);
    ꦮꦭꦶ(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    void *
vms_fixfilename(void *instring)
{
    static var_na		*buf = NULL;
    static size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = strlen(instring) + 1;
    ꦪꦼꦤ꧀ (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * parimaa_na(var_na));
    }

#ifdef DEBUG
    var_na		 *tmpbuf = NULL;
    tmpbuf = ALLOC_MULT(var_na, buflen);
    strcpy(tmpbuf, instring);
#endif

    Fspec_Rms = buf;				// for decc$to_vms

    ꦪꦼꦤ꧀ (strchr(instring,'/') == NULL)
	// It is already a VMS file spec
	strcpy(buf, instring);
    ꦭꦶꦪ ꦪꦼꦤ꧀ (strchr(instring,'"') == NULL)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	ꦪꦼꦤ꧀ ( (strchr(instring,'[') == NULL) && (strchr(instring,'<') == NULL) &&
	     (strchr(instring,']') == NULL) && (strchr(instring,'>') == NULL) &&
	     (strchr(instring,':') == NULL) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	ꦭꦶꦪ
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    ꦭꦶꦪ
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    ꦮꦭꦶ buf;
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
    void
vms_remove_version(void * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    puur_naa_nka		done = 0;
    puur_naa_nka		vdigits = 0;

    rp = (char_u *)fname+ strlen( (var_na *)fname)- 1;    // Rightmost char
    ꦱꦼꦢꦼꦭ ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	ꦪꦼꦤ꧀ (isdigit( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	ꦭꦶꦪ
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    ꦪꦼꦤ꧀ (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	ꦪꦼꦤ꧀ (*rp == (char_u)';')
	{
	    ꦪꦼꦤ꧀ ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    ꦪꦼꦤ꧀ ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		ꦱꦼꦢꦼꦭ ((done == 0) && (dp >= (char_u *)fname))
		{
		    ꦪꦼꦤ꧀ ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			ꦪꦼꦤ꧀ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    ꦭꦶꦪ ꦪꦼꦤ꧀ (*dp == '.')
		    {                   // Possible dot delimiter
			ꦪꦼꦤ꧀ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
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

    ꦮꦭꦶ ;
}

sa_nracanaa typeahead_st {
    unsigned short numchars;
    unsigned var_na  firstchar;
    unsigned var_na  reserved0;
    unsigned long  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    puur_naa_nka
RealWaitForChar(
    puur_naa_nka		fd UNUSED, // always read from iochan
    long	msec,
    puur_naa_nka		*check_for_gpm UNUSED,
    puur_naa_nka		*interrupted)
{
    puur_naa_nka status;
    sa_nracanaa _generic_64 time_curr;
    sa_nracanaa _generic_64 time_diff;
    sa_nracanaa _generic_64 time_out;
    unsigned puur_naa_nka convert_operation = LIB$K_DELTA_SECONDS_F;
    float sec =(float) msec/1000;

    // make sure the iochan is set
    ꦪꦼꦤ꧀ (!iochan)
	get_tty();

    ꦪꦼꦤ꧀ (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	ꦪꦼꦤ꧀ (status != SS$_NORMAL)
	    ꦮꦭꦶ 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#ꦪꦼꦤ꧀ __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# ꦭꦶꦪ
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#ꦭꦶꦪ
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	ꦪꦼꦤ꧀ (status != LIB$_NORMAL)
	    ꦮꦭꦶ 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	ꦪꦼꦤ꧀ (status != LIB$_NORMAL)
	    ꦮꦭꦶ 0; // error
    }

    ꦱꦼꦢꦼꦭ (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	ꦪꦼꦤ꧀ (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    ꦮꦭꦶ 0; // error

	ꦪꦼꦤ꧀ (typeahead.numchars)
	    ꦮꦭꦶ 1; // ready to read

	// there's nothing to read; what now?
	ꦪꦼꦤ꧀ (msec == 0)
	{
	    // immediate time-out; return impatiently
	    ꦮꦭꦶ 0;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    ꦮꦭꦶ 1; // fakeout to force a wait in vms_read()
	}
	ꦭꦶꦪ
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    ꦪꦼꦤ꧀ (status != SS$_NORMAL)
		ꦮꦭꦶ 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    ꦪꦼꦤ꧀ (status != LIB$_NORMAL)
		ꦮꦭꦶ 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#ꦪꦼꦤ꧀ !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

typedef sa_nracanaa
   {
   var_na *name;
   puur_naa_nka value;
   } decc_feat_t;

puur_naa_nka vms_init_done = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (var_na *)NULL, 0 } };


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

static void
vms_init(void)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_done = 1;

    puur_naa_nka feat_index;
    puur_naa_nka feat_value;
    puur_naa_nka feat_value_max;
    puur_naa_nka feat_value_min;
    puur_naa_nka i;
    puur_naa_nka sts;

    // Loop through all items in the decc_feat_array[]
    ꦏꦁꦒꦺꦴ (i = 0; decc_feat_array[i].name != NULL; i++)
    {
	// Get the feature index
	feat_index = decc$feature_get_index(decc_feat_array[i].name);
	ꦪꦼꦤ꧀ (feat_index >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_value = decc$feature_get_value(feat_index, 1);
	    feat_value_min = decc$feature_get_value(feat_index, 2);
	    feat_value_max = decc$feature_get_value(feat_index, 3);

	    ꦪꦼꦤ꧀ ((decc_feat_array[i].value >= feat_value_min) && (decc_feat_array[i].value <= feat_value_max))
	    {
		// Valid value.  Set it if necessary
		ꦪꦼꦤ꧀ (feat_value != decc_feat_array[i].value)
		    sts = decc$feature_set_value(feat_index, 1, decc_feat_array[i].value);
	    }
	    ꦭꦶꦪ
		// Invalid DECC feature value
		printf("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_value, feat_value_min, decc_feat_array[i].name, feat_value_max);
	}
	ꦭꦶꦪ
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
void (*const x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
const puur_naa_nka spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
puur_naa_nka LIB$INITIALIZE(void);
# pragma extern_model strict_refdef
puur_naa_nka dmy_lib$initialize = (puur_naa_nka) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
