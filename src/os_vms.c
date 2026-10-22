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
#නම් !defined(VAX)
# include <gen64def.h>
#නැතහොත්
// based on Alpha's gen64def.h; the file is absent on VAX
typedef sa_nracanaa _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	unsigned පූර්ණ_සංඛ්‍යා gen64$l_longword [2];   // ...two 32-bit values, or
	unsigned short පූර්ණ_සංඛ්‍යා gen64$w_word [4]; // ...four 16-bit values
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
	පූර්ණ_සංඛ්‍යා	basic;
    }	x;
    පූර්ණ_සංඛ්‍යා		extended;
}	TT_MODE;

typedef sa_nracanaa
{
    short	buflen;
    short	itemcode;
    var_na	*bufadrs;
    පූර්ණ_සංඛ්‍යා		*retlen;
}	ITEM;

typedef sa_nracanaa
{
    ITEM	equ;
    පූර්ණ_සංඛ්‍යා		nul;
}	ITMLST1;

typedef sa_nracanaa
{
    ITEM	index;
    ITEM	string;
    පූර්ණ_සංඛ්‍යා	nul;
}	ITMLST2;

static TT_MODE	orgmode;
static short	iochan;			// TTY I/O channel
static short	iosb[4];		// IO status block

static පූර්ණ_සංඛ්‍යා vms_match_num = 0;
static පූර්ණ_සංඛ්‍යා vms_match_free = 0;
static char_u **vms_fmatch = NULL;
static var_na *Fspec_Rms;		       // rms file spec, passed implicitly between routines



static TT_MODE	get_tty(void);
static void	set_tty(පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) do { අතරතුර (Str[1] == '/') ++Str; } අතරතුර (0)


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
vul_item(ITEM *itm, short len, short cod, var_na *adr, පූර්ණ_සංඛ්‍යා *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    void
mch_settmode(tmode_T tmode)
{
    පූර්ණ_සංඛ්‍යා	status;

    නම් ( tmode == TMODE_RAW )
	set_tty(0, 0);
    නැතහොත්
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
	නම් (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    ආපසු;
	(void)sys$dassgn(iochan);
	iochan = 0;
    }
}

    static void
set_tty(පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා col)
{
    පූර්ණ_සංඛ්‍යා		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    static short    first_time = TRUE;

    නම් (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    නම් (col)
	newmode.width		 = col;
    නම් (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, parimaa_na(newmode), 0, 0, 0, 0);
    නම් (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	ආපසු;
}

    static TT_MODE
get_tty(void)
{

    static $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    පූර්ණ_සංඛ්‍යා		status;
    TT_MODE	tt_mode;

    නම් (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, parimaa_na(tt_mode), 0, 0, 0, 0);
    නම් (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    ආපසු(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    පූර්ණ_සංඛ්‍යා
mch_get_shellsize(void)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    ආපසු OK;
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
    පූර්ණ_සංඛ්‍යා			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    var_na		*sbuf = NULL;

    vul_desc(&d_lognam, (var_na *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, parimaa_na(පූර්ණ_සංඛ්‍යා), LNM$_INDEX, (var_na *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    නම් (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, NULL,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	නම් (cp = alloc(lengte + 1))
	    strcpy((var_na *)cp, buffer);
	ආපසු(cp);
    }
    නැතහොත් නම් ((sbuf = getenv((var_na *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = alloc(lengte);
	නම් (cp)
	    strcpy((var_na *)cp, sbuf);
	ආපසු cp;
    }
    නැතහොත්
	ආපසු(NULL);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    පූර්ණ_සංඛ්‍යා
mch_setenv(var_na *var, var_na *value, පූර්ණ_සංඛ්‍යා x)
{
    පූර්ණ_සංඛ්‍යා		res, dum;
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
    ආපසු((res == 1) ? 0 : -1);
}

    පූර්ණ_සංඛ්‍යා
vms_sys(var_na *cmd, var_na *out, var_na *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status;

    නම් (cmd)
	vul_desc(&cdsc, cmd);
    නම් (out)
	vul_desc(&odsc, out);
    නම් (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : NULL,		// command string
	      inp ? &idsc : NULL,		// input file
	      out ? &odsc : NULL,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    ආපසු status;
}

/*
 * Convert string to lowercase - most often filename
 */
    var_na *
vms_tolower( var_na *name )
{
    පූර්ණ_සංඛ්‍යා i,nlen = strlen(name);
    සඳහා (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    ආපසු name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    පූර්ණ_සංඛ්‍යා
vms_sys_status(පූර්ණ_සංඛ්‍යා status)
{
    නම් (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	ආපසු status;		// Command failed.
    ආපසු 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    පූර්ණ_සංඛ්‍යා
vms_read(var_na *inbuf, size_t nbytes)
{
    පූර්ණ_සංඛ්‍යා		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    static long trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    නම් (!iochan)
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

    ආපසු len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    static පූර්ණ_සංඛ්‍යා
vms_wproc(var_na *name, පූර්ණ_සංඛ්‍යා val)
{
    පූර්ණ_සංඛ්‍යා i;
    static පූර්ණ_සංඛ්‍යා vms_match_alloced = 0;

    නම් (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	ආපසු 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    නම් (vms_match_num == 0)
    {
	// first time through, setup some things
	නම් (NULL == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    නම් (!vms_fmatch)
		ආපසු 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	නැතහොත්
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    සඳහා (i = 0; i<vms_match_num; i++)
    {
	නම් (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    ආපසු 1;
    }
    නම් (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		parimaa_na(var_na **) * vms_match_alloced);
	නම් (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    ආපසු 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_num] = vim_strsave((char_u *)name);

    ++vms_match_num;
    ආපසු 1;
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
    පූර්ණ_සංඛ්‍යා
mch_expand_wildcards(
    පූර්ණ_සංඛ්‍යා		num_pat,
    char_u	**pat,
    පූර්ණ_සංඛ්‍යා		*num_file,
    char_u	***file,
    පූර්ණ_සංඛ්‍යා		flags)
{
    පූර්ණ_සංඛ්‍යා		i, cnt = 0;
    char_u	buf[MAXPATHL];
    var_na       *result;
    පූර්ණ_සංඛ්‍යා		dir;
    පූර්ණ_සංඛ්‍යා files_alloced, files_free;

    *num_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    නම් (*file == NULL)
    {
	*num_file = 0;
	ආපසු FAIL;
    }
    සඳහා (i = 0; i < num_pat; i++)
    {
	// expand environment var or home dir
	නම් (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	නැතහොත්
	    STRCPY(buf,pat[i]);

	vms_match_num = 0; // reset collection counter
	result = decc$translate_vms(vms_fixfilename(buf));
	නම් ( (පූර්ණ_සංඛ්‍යා) result == 0 || (පූර්ණ_සංඛ්‍යා) result == -1  )
	{
	    cnt = 0;
	}
	නැතහොත්
	{
	    cnt = decc$to_vms(result, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	නම් (cnt > 0)
	    cnt = vms_match_num;

	නම් (cnt < 1)
	    continue;

	සඳහා (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    නම් (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		continue;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    නම් (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		continue;

	    // Skip files that are not executable if we check for that.
	    නම් (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], NULL, !(flags & EW_SHELLCMD)))
		continue;

	    // allocate memory for pointers
	    නම් (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, parimaa_na(char_u **) * files_alloced);
		නම් (*file == NULL)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *num_file = 0;
		    ආපසු(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*num_file++] = vms_fmatch[i];
	}
    }
    ආපසු OK;
}

    පූර්ණ_සංඛ්‍යා
mch_expandpath(garray_T *gap, char_u *path, පූර්ණ_සංඛ්‍යා flags)
{
    පූර්ණ_සංඛ්‍යා		i,cnt = 0;
    var_na       *result;

    vms_match_num = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    result = decc$translate_vms(vms_fixfilename(path));
    නම් ( (පූර්ණ_සංඛ්‍යා) result == 0 || (පූර්ණ_සංඛ්‍යා) result == -1  )
    {
	cnt = 0;
    }
    නැතහොත්
    {
	cnt = decc$to_vms(result, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    නම් (cnt > 0)
	cnt = vms_match_num;
    සඳහා (i = 0; i < cnt; i++)
    {
	නම් (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    ආපසු cnt;
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
    පූර්ණ_සංඛ්‍යා len;
    var_na *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(in, ':');   // find last colon
    නම් (lastcolon != NULL)
    {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = NULL;	// default: no directory

    // start of directory portion
    ch = *in;
    නම් ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    නැතහොත් නම් (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    නැතහොත්
    {                                // not a special character
	අතරතුර (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    නම් (strchr(in, '/') == NULL)     // any more Unix directories ?
    {
	strcpy(out, in);             // No - get rest of the spec
	ආපසු;
    }
    නැතහොත්
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

    අතරතුර (*in != '\0')
    {
	ch = *in;
	නම් ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	නැතහොත් නම් (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	නැතහොත්
	{
	    අතරතුර (EQN(in, "./", 2))    // Ignore Unix "current dir"
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

    නම් (end_of_dir != NULL) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    static පූර්ණ_සංඛ්‍යා
vms_fspec_proc(var_na *fil, පූර්ණ_සංඛ්‍යා val)
{
    strcpy(Fspec_Rms,fil);
    ආපසු(1);
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
    නම් (len > buflen)
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

    නම් (strchr(instring,'/') == NULL)
	// It is already a VMS file spec
	strcpy(buf, instring);
    නැතහොත් නම් (strchr(instring,'"') == NULL)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	නම් ( (strchr(instring,'[') == NULL) && (strchr(instring,'<') == NULL) &&
	     (strchr(instring,']') == NULL) && (strchr(instring,'>') == NULL) &&
	     (strchr(instring,':') == NULL) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	නැතහොත්
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    නැතහොත්
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    ආපසු buf;
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
    පූර්ණ_සංඛ්‍යා		done = 0;
    පූර්ණ_සංඛ්‍යා		vdigits = 0;

    rp = (char_u *)fname+ strlen( (var_na *)fname)- 1;    // Rightmost char
    අතරතුර ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	නම් (isdigit( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	නැතහොත්
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    නම් (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	නම් (*rp == (char_u)';')
	{
	    නම් ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	නැතහොත් නම් (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    නම් ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		අතරතුර ((done == 0) && (dp >= (char_u *)fname))
		{
		    නම් ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			නම් ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    නැතහොත් නම් (*dp == '.')
		    {                   // Possible dot delimiter
			නම් ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
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

    ආපසු ;
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
    පූර්ණ_සංඛ්‍යා
RealWaitForChar(
    පූර්ණ_සංඛ්‍යා		fd UNUSED, // always read from iochan
    long	msec,
    පූර්ණ_සංඛ්‍යා		*check_for_gpm UNUSED,
    පූර්ණ_සංඛ්‍යා		*interrupted)
{
    පූර්ණ_සංඛ්‍යා status;
    sa_nracanaa _generic_64 time_curr;
    sa_nracanaa _generic_64 time_diff;
    sa_nracanaa _generic_64 time_out;
    unsigned පූර්ණ_සංඛ්‍යා convert_operation = LIB$K_DELTA_SECONDS_F;
    float sec =(float) msec/1000;

    // make sure the iochan is set
    නම් (!iochan)
	get_tty();

    නම් (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	නම් (status != SS$_NORMAL)
	    ආපසු 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#නම් __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# නැතහොත්
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#නැතහොත්
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	නම් (status != LIB$_NORMAL)
	    ආපසු 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	නම් (status != LIB$_NORMAL)
	    ආපසු 0; // error
    }

    අතරතුර (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	නම් (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    ආපසු 0; // error

	නම් (typeahead.numchars)
	    ආපසු 1; // ready to read

	// there's nothing to read; what now?
	නම් (msec == 0)
	{
	    // immediate time-out; return impatiently
	    ආපසු 0;
	}
	නැතහොත් නම් (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    ආපසු 1; // fakeout to force a wait in vms_read()
	}
	නැතහොත්
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    නම් (status != SS$_NORMAL)
		ආපසු 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    නම් (status != LIB$_NORMAL)
		ආපසු 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#නම් !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

typedef sa_nracanaa
   {
   var_na *name;
   පූර්ණ_සංඛ්‍යා value;
   } decc_feat_t;

පූර්ණ_සංඛ්‍යා vms_init_done = -1;

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

    පූර්ණ_සංඛ්‍යා feat_index;
    පූර්ණ_සංඛ්‍යා feat_value;
    පූර්ණ_සංඛ්‍යා feat_value_max;
    පූර්ණ_සංඛ්‍යා feat_value_min;
    පූර්ණ_සංඛ්‍යා i;
    පූර්ණ_සංඛ්‍යා sts;

    // Loop through all items in the decc_feat_array[]
    සඳහා (i = 0; decc_feat_array[i].name != NULL; i++)
    {
	// Get the feature index
	feat_index = decc$feature_get_index(decc_feat_array[i].name);
	නම් (feat_index >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_value = decc$feature_get_value(feat_index, 1);
	    feat_value_min = decc$feature_get_value(feat_index, 2);
	    feat_value_max = decc$feature_get_value(feat_index, 3);

	    නම් ((decc_feat_array[i].value >= feat_value_min) && (decc_feat_array[i].value <= feat_value_max))
	    {
		// Valid value.  Set it if necessary
		නම් (feat_value != decc_feat_array[i].value)
		    sts = decc$feature_set_value(feat_index, 1, decc_feat_array[i].value);
	    }
	    නැතහොත්
		// Invalid DECC feature value
		printf("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_value, feat_value_min, decc_feat_array[i].name, feat_value_max);
	}
	නැතහොත්
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
const පූර්ණ_සංඛ්‍යා spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
පූර්ණ_සංඛ්‍යා LIB$INITIALIZE(void);
# pragma extern_model strict_refdef
පූර්ණ_සංඛ්‍යා dmy_lib$initialize = (පූර්ණ_සංඛ්‍යා) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
