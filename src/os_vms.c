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
#གལ་ཏེ !defined(VAX)
# include <gen64def.h>
#དེ་མིན
// based on Alpha's gen64def.h; the file is absent on VAX
typedef sa_nracanaa _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	unsigned ཧྲིལ gen64$l_longword [2];   // ...two 32-bit values, or
	unsigned short ཧྲིལ gen64$w_word [4]; // ...four 16-bit values
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
	ཧྲིལ	basic;
    }	x;
    ཧྲིལ		extended;
}	TT_MODE;

typedef sa_nracanaa
{
    short	buflen;
    short	itemcode;
    var_na	*bufadrs;
    ཧྲིལ		*retlen;
}	ITEM;

typedef sa_nracanaa
{
    ITEM	equ;
    ཧྲིལ		nul;
}	ITMLST1;

typedef sa_nracanaa
{
    ITEM	index;
    ITEM	string;
    ཧྲིལ	nul;
}	ITMLST2;

static TT_MODE	orgmode;
static short	iochan;			// TTY I/O channel
static short	iosb[4];		// IO status block

static ཧྲིལ vms_match_num = 0;
static ཧྲིལ vms_match_free = 0;
static char_u **vms_fmatch = NULL;
static var_na *Fspec_Rms;		       // rms file spec, passed implicitly between routines



static TT_MODE	get_tty(void);
static void	set_tty(ཧྲིལ row, ཧྲིལ col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) do { སྐབས (Str[1] == '/') ++Str; } སྐབས (0)


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
vul_item(ITEM *itm, short len, short cod, var_na *adr, ཧྲིལ *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    void
mch_settmode(tmode_T tmode)
{
    ཧྲིལ	status;

    གལ་ཏེ ( tmode == TMODE_RAW )
	set_tty(0, 0);
    དེ་མིན
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
	གལ་ཏེ (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    ཕྱིར;
	(void)sys$dassgn(iochan);
	iochan = 0;
    }
}

    static void
set_tty(ཧྲིལ row, ཧྲིལ col)
{
    ཧྲིལ		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    static short    first_time = TRUE;

    གལ་ཏེ (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    གལ་ཏེ (col)
	newmode.width		 = col;
    གལ་ཏེ (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, parimaa_na(newmode), 0, 0, 0, 0);
    གལ་ཏེ (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	ཕྱིར;
}

    static TT_MODE
get_tty(void)
{

    static $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    ཧྲིལ		status;
    TT_MODE	tt_mode;

    གལ་ཏེ (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, parimaa_na(tt_mode), 0, 0, 0, 0);
    གལ་ཏེ (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    ཕྱིར(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    ཧྲིལ
mch_get_shellsize(void)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    ཕྱིར OK;
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
    ཧྲིལ			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    var_na		*sbuf = NULL;

    vul_desc(&d_lognam, (var_na *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, parimaa_na(ཧྲིལ), LNM$_INDEX, (var_na *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    གལ་ཏེ (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, NULL,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	གལ་ཏེ (cp = alloc(lengte + 1))
	    strcpy((var_na *)cp, buffer);
	ཕྱིར(cp);
    }
    དེ་མིན གལ་ཏེ ((sbuf = getenv((var_na *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = alloc(lengte);
	གལ་ཏེ (cp)
	    strcpy((var_na *)cp, sbuf);
	ཕྱིར cp;
    }
    དེ་མིན
	ཕྱིར(NULL);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    ཧྲིལ
mch_setenv(var_na *var, var_na *value, ཧྲིལ x)
{
    ཧྲིལ		res, dum;
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
    ཕྱིར((res == 1) ? 0 : -1);
}

    ཧྲིལ
vms_sys(var_na *cmd, var_na *out, var_na *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status;

    གལ་ཏེ (cmd)
	vul_desc(&cdsc, cmd);
    གལ་ཏེ (out)
	vul_desc(&odsc, out);
    གལ་ཏེ (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : NULL,		// command string
	      inp ? &idsc : NULL,		// input file
	      out ? &odsc : NULL,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    ཕྱིར status;
}

/*
 * Convert string to lowercase - most often filename
 */
    var_na *
vms_tolower( var_na *name )
{
    ཧྲིལ i,nlen = strlen(name);
    ཆེད་དུ (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    ཕྱིར name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    ཧྲིལ
vms_sys_status(ཧྲིལ status)
{
    གལ་ཏེ (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	ཕྱིར status;		// Command failed.
    ཕྱིར 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    ཧྲིལ
vms_read(var_na *inbuf, size_t nbytes)
{
    ཧྲིལ		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    static long trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    གལ་ཏེ (!iochan)
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

    ཕྱིར len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    static ཧྲིལ
vms_wproc(var_na *name, ཧྲིལ val)
{
    ཧྲིལ i;
    static ཧྲིལ vms_match_alloced = 0;

    གལ་ཏེ (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	ཕྱིར 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    གལ་ཏེ (vms_match_num == 0)
    {
	// first time through, setup some things
	གལ་ཏེ (NULL == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    གལ་ཏེ (!vms_fmatch)
		ཕྱིར 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	དེ་མིན
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    ཆེད་དུ (i = 0; i<vms_match_num; i++)
    {
	གལ་ཏེ (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    ཕྱིར 1;
    }
    གལ་ཏེ (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		parimaa_na(var_na **) * vms_match_alloced);
	གལ་ཏེ (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    ཕྱིར 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_num] = vim_strsave((char_u *)name);

    ++vms_match_num;
    ཕྱིར 1;
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
    ཧྲིལ
mch_expand_wildcards(
    ཧྲིལ		num_pat,
    char_u	**pat,
    ཧྲིལ		*num_file,
    char_u	***file,
    ཧྲིལ		flags)
{
    ཧྲིལ		i, cnt = 0;
    char_u	buf[MAXPATHL];
    var_na       *result;
    ཧྲིལ		dir;
    ཧྲིལ files_alloced, files_free;

    *num_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    གལ་ཏེ (*file == NULL)
    {
	*num_file = 0;
	ཕྱིར FAIL;
    }
    ཆེད་དུ (i = 0; i < num_pat; i++)
    {
	// expand environment var or home dir
	གལ་ཏེ (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	དེ་མིན
	    STRCPY(buf,pat[i]);

	vms_match_num = 0; // reset collection counter
	result = decc$translate_vms(vms_fixfilename(buf));
	གལ་ཏེ ( (ཧྲིལ) result == 0 || (ཧྲིལ) result == -1  )
	{
	    cnt = 0;
	}
	དེ་མིན
	{
	    cnt = decc$to_vms(result, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	གལ་ཏེ (cnt > 0)
	    cnt = vms_match_num;

	གལ་ཏེ (cnt < 1)
	    continue;

	ཆེད་དུ (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    གལ་ཏེ (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		continue;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    གལ་ཏེ (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		continue;

	    // Skip files that are not executable if we check for that.
	    གལ་ཏེ (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], NULL, !(flags & EW_SHELLCMD)))
		continue;

	    // allocate memory for pointers
	    གལ་ཏེ (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, parimaa_na(char_u **) * files_alloced);
		གལ་ཏེ (*file == NULL)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *num_file = 0;
		    ཕྱིར(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*num_file++] = vms_fmatch[i];
	}
    }
    ཕྱིར OK;
}

    ཧྲིལ
mch_expandpath(garray_T *gap, char_u *path, ཧྲིལ flags)
{
    ཧྲིལ		i,cnt = 0;
    var_na       *result;

    vms_match_num = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    result = decc$translate_vms(vms_fixfilename(path));
    གལ་ཏེ ( (ཧྲིལ) result == 0 || (ཧྲིལ) result == -1  )
    {
	cnt = 0;
    }
    དེ་མིན
    {
	cnt = decc$to_vms(result, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    གལ་ཏེ (cnt > 0)
	cnt = vms_match_num;
    ཆེད་དུ (i = 0; i < cnt; i++)
    {
	གལ་ཏེ (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    ཕྱིར cnt;
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
    ཧྲིལ len;
    var_na *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(in, ':');   // find last colon
    གལ་ཏེ (lastcolon != NULL)
    {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = NULL;	// default: no directory

    // start of directory portion
    ch = *in;
    གལ་ཏེ ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    དེ་མིན གལ་ཏེ (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    དེ་མིན
    {                                // not a special character
	སྐབས (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    གལ་ཏེ (strchr(in, '/') == NULL)     // any more Unix directories ?
    {
	strcpy(out, in);             // No - get rest of the spec
	ཕྱིར;
    }
    དེ་མིན
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

    སྐབས (*in != '\0')
    {
	ch = *in;
	གལ་ཏེ ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	དེ་མིན གལ་ཏེ (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	དེ་མིན
	{
	    སྐབས (EQN(in, "./", 2))    // Ignore Unix "current dir"
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

    གལ་ཏེ (end_of_dir != NULL) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    static ཧྲིལ
vms_fspec_proc(var_na *fil, ཧྲིལ val)
{
    strcpy(Fspec_Rms,fil);
    ཕྱིར(1);
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
    གལ་ཏེ (len > buflen)
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

    གལ་ཏེ (strchr(instring,'/') == NULL)
	// It is already a VMS file spec
	strcpy(buf, instring);
    དེ་མིན གལ་ཏེ (strchr(instring,'"') == NULL)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	གལ་ཏེ ( (strchr(instring,'[') == NULL) && (strchr(instring,'<') == NULL) &&
	     (strchr(instring,']') == NULL) && (strchr(instring,'>') == NULL) &&
	     (strchr(instring,':') == NULL) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	དེ་མིན
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    དེ་མིན
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    ཕྱིར buf;
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
    ཧྲིལ		done = 0;
    ཧྲིལ		vdigits = 0;

    rp = (char_u *)fname+ strlen( (var_na *)fname)- 1;    // Rightmost char
    སྐབས ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	གལ་ཏེ (isdigit( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	དེ་མིན
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    གལ་ཏེ (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	གལ་ཏེ (*rp == (char_u)';')
	{
	    གལ་ཏེ ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	དེ་མིན གལ་ཏེ (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    གལ་ཏེ ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		སྐབས ((done == 0) && (dp >= (char_u *)fname))
		{
		    གལ་ཏེ ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			གལ་ཏེ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    དེ་མིན གལ་ཏེ (*dp == '.')
		    {                   // Possible dot delimiter
			གལ་ཏེ ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
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

    ཕྱིར ;
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
    ཧྲིལ
RealWaitForChar(
    ཧྲིལ		fd UNUSED, // always read from iochan
    long	msec,
    ཧྲིལ		*check_for_gpm UNUSED,
    ཧྲིལ		*interrupted)
{
    ཧྲིལ status;
    sa_nracanaa _generic_64 time_curr;
    sa_nracanaa _generic_64 time_diff;
    sa_nracanaa _generic_64 time_out;
    unsigned ཧྲིལ convert_operation = LIB$K_DELTA_SECONDS_F;
    float sec =(float) msec/1000;

    // make sure the iochan is set
    གལ་ཏེ (!iochan)
	get_tty();

    གལ་ཏེ (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	གལ་ཏེ (status != SS$_NORMAL)
	    ཕྱིར 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#གལ་ཏེ __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# དེ་མིན
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#དེ་མིན
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	གལ་ཏེ (status != LIB$_NORMAL)
	    ཕྱིར 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	གལ་ཏེ (status != LIB$_NORMAL)
	    ཕྱིར 0; // error
    }

    སྐབས (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	གལ་ཏེ (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    ཕྱིར 0; // error

	གལ་ཏེ (typeahead.numchars)
	    ཕྱིར 1; // ready to read

	// there's nothing to read; what now?
	གལ་ཏེ (msec == 0)
	{
	    // immediate time-out; return impatiently
	    ཕྱིར 0;
	}
	དེ་མིན གལ་ཏེ (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    ཕྱིར 1; // fakeout to force a wait in vms_read()
	}
	དེ་མིན
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    གལ་ཏེ (status != SS$_NORMAL)
		ཕྱིར 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    གལ་ཏེ (status != LIB$_NORMAL)
		ཕྱིར 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#གལ་ཏེ !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

typedef sa_nracanaa
   {
   var_na *name;
   ཧྲིལ value;
   } decc_feat_t;

ཧྲིལ vms_init_done = -1;

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

    ཧྲིལ feat_index;
    ཧྲིལ feat_value;
    ཧྲིལ feat_value_max;
    ཧྲིལ feat_value_min;
    ཧྲིལ i;
    ཧྲིལ sts;

    // Loop through all items in the decc_feat_array[]
    ཆེད་དུ (i = 0; decc_feat_array[i].name != NULL; i++)
    {
	// Get the feature index
	feat_index = decc$feature_get_index(decc_feat_array[i].name);
	གལ་ཏེ (feat_index >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_value = decc$feature_get_value(feat_index, 1);
	    feat_value_min = decc$feature_get_value(feat_index, 2);
	    feat_value_max = decc$feature_get_value(feat_index, 3);

	    གལ་ཏེ ((decc_feat_array[i].value >= feat_value_min) && (decc_feat_array[i].value <= feat_value_max))
	    {
		// Valid value.  Set it if necessary
		གལ་ཏེ (feat_value != decc_feat_array[i].value)
		    sts = decc$feature_set_value(feat_index, 1, decc_feat_array[i].value);
	    }
	    དེ་མིན
		// Invalid DECC feature value
		printf("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_value, feat_value_min, decc_feat_array[i].name, feat_value_max);
	}
	དེ་མིན
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
const ཧྲིལ spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
ཧྲིལ LIB$INITIALIZE(void);
# pragma extern_model strict_refdef
ཧྲིལ dmy_lib$initialize = (ཧྲིལ) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
