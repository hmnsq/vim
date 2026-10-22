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
#ако !defined(VAX)
# include <gen64def.h>
#иначе
// based on Alpha's gen64def.h; the file is absent on VAX
typedef struct _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	unsigned цяло gen64$l_longword [2];   // ...two 32-bit values, or
	unsigned short цяло gen64$w_word [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#endif

typedef struct
{
    char	class;
    char	type;
    short	width;
    union
    {
	struct
	{
	    char	_basic[3];
	    char	length;
	}	y;
	цяло	basic;
    }	x;
    цяло		extended;
}	TT_MODE;

typedef struct
{
    short	buflen;
    short	itemcode;
    char	*bufadrs;
    цяло		*retlen;
}	ITEM;

typedef struct
{
    ITEM	equ;
    цяло		nul;
}	ITMLST1;

typedef struct
{
    ITEM	index;
    ITEM	string;
    цяло	nul;
}	ITMLST2;

static TT_MODE	orgmode;
static short	iochan;			// TTY I/O channel
static short	iosb[4];		// IO status block

static цяло vms_match_num = 0;
static цяло vms_match_free = 0;
static char_u **vms_fmatch = NULL;
static char *Fspec_Rms;		       // rms file spec, passed implicitly between routines



static TT_MODE	get_tty(void);
static void	set_tty(цяло row, цяло col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) do { докато (Str[1] == '/') ++Str; } докато (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    static void
vul_desc(DESC *des, char *str)
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
vul_item(ITEM *itm, short len, short cod, char *adr, цяло *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    void
mch_settmode(tmode_T tmode)
{
    цяло	status;

    ако ( tmode == TMODE_RAW )
	set_tty(0, 0);
    иначе
    {
	switch (orgmode.width)
	{
	    case 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	break;
	    case 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	break;
	    default:	break;
	}
	out_flush();
	status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, sizeof(TT_MODE), 0,0,0,0);
	ако (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    върни;
	(void)sys$dassgn(iochan);
	iochan = 0;
    }
}

    static void
set_tty(цяло row, цяло col)
{
    цяло		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    static short    first_time = TRUE;

    ако (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    ако (col)
	newmode.width		 = col;
    ако (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, sizeof(newmode), 0, 0, 0, 0);
    ако (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	върни;
}

    static TT_MODE
get_tty(void)
{

    static $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    цяло		status;
    TT_MODE	tt_mode;

    ако (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, sizeof(tt_mode), 0, 0, 0, 0);
    ако (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    върни(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    цяло
mch_get_shellsize(void)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    върни OK;
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
	case 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	break;
	case 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	break;
	default:	break;
    }
    out_flush();
    screen_start();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    static char		buffer[LNM$C_NAMLENGTH+1];
    char_u		*cp = NULL;
    unsigned long	attrib;
    цяло			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    char		*sbuf = NULL;

    vul_desc(&d_lognam, (char *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, sizeof(цяло), LNM$_INDEX, (char *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    ако (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, NULL,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	ако (cp = alloc(lengte + 1))
	    strcpy((char *)cp, buffer);
	върни(cp);
    }
    иначе ако ((sbuf = getenv((char *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = alloc(lengte);
	ако (cp)
	    strcpy((char *)cp, sbuf);
	върни cp;
    }
    иначе
	върни(NULL);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    цяло
mch_setenv(char *var, char *value, цяло x)
{
    цяло		res, dum;
    long	attrib = 0L;
    char	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, value ? strlen(value) : 0, value ? LNM$_STRING : 0,
	    value, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    върни((res == 1) ? 0 : -1);
}

    цяло
vms_sys(char *cmd, char *out, char *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status;

    ако (cmd)
	vul_desc(&cdsc, cmd);
    ако (out)
	vul_desc(&odsc, out);
    ако (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : NULL,		// command string
	      inp ? &idsc : NULL,		// input file
	      out ? &odsc : NULL,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    върни status;
}

/*
 * Convert string to lowercase - most often filename
 */
    char *
vms_tolower( char *name )
{
    цяло i,nlen = strlen(name);
    за (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    върни name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    цяло
vms_sys_status(цяло status)
{
    ако (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	върни status;		// Command failed.
    върни 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    цяло
vms_read(char *inbuf, size_t nbytes)
{
    цяло		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    static long trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    ако (!iochan)
	tt_mode = get_tty();

    // important: clean the inbuf
    memset(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_item(&itmlst[0], 0, TRM$_MODIFIERS,
	 (char *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_item(&itmlst[1], sizeof(trm_mask), TRM$_TERM, (char *)&trm_mask, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, sizeof(itmlst));
    len = strlen(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = strlen(inbuf); // return the total length

    върни len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    static цяло
vms_wproc(char *name, цяло val)
{
    цяло i;
    static цяло vms_match_alloced = 0;

    ако (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	върни 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    ако (vms_match_num == 0)
    {
	// first time through, setup some things
	ако (NULL == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    ако (!vms_fmatch)
		върни 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	иначе
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    за (i = 0; i<vms_match_num; i++)
    {
	ако (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    върни 1;
    }
    ако (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		sizeof(char **) * vms_match_alloced);
	ако (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    върни 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_num] = vim_strsave((char_u *)name);

    ++vms_match_num;
    върни 1;
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
    цяло
mch_expand_wildcards(
    цяло		num_pat,
    char_u	**pat,
    цяло		*num_file,
    char_u	***file,
    цяло		flags)
{
    цяло		i, cnt = 0;
    char_u	buf[MAXPATHL];
    char       *result;
    цяло		dir;
    цяло files_alloced, files_free;

    *num_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    ако (*file == NULL)
    {
	*num_file = 0;
	върни FAIL;
    }
    за (i = 0; i < num_pat; i++)
    {
	// expand environment var or home dir
	ако (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	иначе
	    STRCPY(buf,pat[i]);

	vms_match_num = 0; // reset collection counter
	result = decc$translate_vms(vms_fixfilename(buf));
	ако ( (цяло) result == 0 || (цяло) result == -1  )
	{
	    cnt = 0;
	}
	иначе
	{
	    cnt = decc$to_vms(result, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	ако (cnt > 0)
	    cnt = vms_match_num;

	ако (cnt < 1)
	    continue;

	за (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    ако (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		continue;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    ако (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		continue;

	    // Skip files that are not executable if we check for that.
	    ако (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], NULL, !(flags & EW_SHELLCMD)))
		continue;

	    // allocate memory for pointers
	    ако (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, sizeof(char_u **) * files_alloced);
		ако (*file == NULL)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *num_file = 0;
		    върни(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*num_file++] = vms_fmatch[i];
	}
    }
    върни OK;
}

    цяло
mch_expandpath(garray_T *gap, char_u *path, цяло flags)
{
    цяло		i,cnt = 0;
    char       *result;

    vms_match_num = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    result = decc$translate_vms(vms_fixfilename(path));
    ако ( (цяло) result == 0 || (цяло) result == -1  )
    {
	cnt = 0;
    }
    иначе
    {
	cnt = decc$to_vms(result, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    ако (cnt > 0)
	cnt = vms_match_num;
    за (i = 0; i < cnt; i++)
    {
	ако (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    върни cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    static void
vms_unix_mixed_filespec(char *in, char *out)
{
    char *lastcolon;
    char *end_of_dir;
    char ch;
    цяло len;
    char *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = strrchr(in, ':');   // find last colon
    ако (lastcolon != NULL)
    {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = NULL;	// default: no directory

    // start of directory portion
    ch = *in;
    ако ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    иначе ако (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    иначе
    {                                // not a special character
	докато (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    ако (strchr(in, '/') == NULL)     // any more Unix directories ?
    {
	strcpy(out, in);             // No - get rest of the spec
	върни;
    }
    иначе
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

    докато (*in != '\0')
    {
	ch = *in;
	ако ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	иначе ако (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	иначе
	{
	    докато (EQN(in, "./", 2))    // Ignore Unix "current dir"
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

    ако (end_of_dir != NULL) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    static цяло
vms_fspec_proc(char *fil, цяло val)
{
    strcpy(Fspec_Rms,fil);
    върни(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    void *
vms_fixfilename(void *instring)
{
    static char		*buf = NULL;
    static size_t	buflen = 0;
    size_t		len;

    // get a big-enough buffer
    len = strlen(instring) + 1;
    ако (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * sizeof(char));
    }

#ifdef DEBUG
    char		 *tmpbuf = NULL;
    tmpbuf = ALLOC_MULT(char, buflen);
    strcpy(tmpbuf, instring);
#endif

    Fspec_Rms = buf;				// for decc$to_vms

    ако (strchr(instring,'/') == NULL)
	// It is already a VMS file spec
	strcpy(buf, instring);
    иначе ако (strchr(instring,'"') == NULL)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	ако ( (strchr(instring,'[') == NULL) && (strchr(instring,'<') == NULL) &&
	     (strchr(instring,']') == NULL) && (strchr(instring,'>') == NULL) &&
	     (strchr(instring,':') == NULL) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	иначе
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    иначе
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    върни buf;
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
    цяло		done = 0;
    цяло		vdigits = 0;

    rp = (char_u *)fname+ strlen( (char *)fname)- 1;    // Rightmost char
    докато ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	ако (isdigit( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	иначе
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    ако (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	ако (*rp == (char_u)';')
	{
	    ако ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	иначе ако (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    ако ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		докато ((done == 0) && (dp >= (char_u *)fname))
		{
		    ако ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			ако ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    иначе ако (*dp == '.')
		    {                   // Possible dot delimiter
			ако ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
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

    върни ;
}

struct typeahead_st {
    unsigned short numchars;
    unsigned char  firstchar;
    unsigned char  reserved0;
    unsigned long  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    цяло
RealWaitForChar(
    цяло		fd UNUSED, // always read from iochan
    long	msec,
    цяло		*check_for_gpm UNUSED,
    цяло		*interrupted)
{
    цяло status;
    struct _generic_64 time_curr;
    struct _generic_64 time_diff;
    struct _generic_64 time_out;
    unsigned цяло convert_operation = LIB$K_DELTA_SECONDS_F;
    float sec =(float) msec/1000;

    // make sure the iochan is set
    ако (!iochan)
	get_tty();

    ако (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	ако (status != SS$_NORMAL)
	    върни 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#ако __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# иначе
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#иначе
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	ако (status != LIB$_NORMAL)
	    върни 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	ако (status != LIB$_NORMAL)
	    върни 0; // error
    }

    докато (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	ако (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    върни 0; // error

	ако (typeahead.numchars)
	    върни 1; // ready to read

	// there's nothing to read; what now?
	ако (msec == 0)
	{
	    // immediate time-out; return impatiently
	    върни 0;
	}
	иначе ако (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    върни 1; // fakeout to force a wait in vms_read()
	}
	иначе
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    ако (status != SS$_NORMAL)
		върни 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    ако (status != LIB$_NORMAL)
		върни 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#ако !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

typedef struct
   {
   char *name;
   цяло value;
   } decc_feat_t;

цяло vms_init_done = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (char *)NULL, 0 } };


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

    цяло feat_index;
    цяло feat_value;
    цяло feat_value_max;
    цяло feat_value_min;
    цяло i;
    цяло sts;

    // Loop through all items in the decc_feat_array[]
    за (i = 0; decc_feat_array[i].name != NULL; i++)
    {
	// Get the feature index
	feat_index = decc$feature_get_index(decc_feat_array[i].name);
	ако (feat_index >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_value = decc$feature_get_value(feat_index, 1);
	    feat_value_min = decc$feature_get_value(feat_index, 2);
	    feat_value_max = decc$feature_get_value(feat_index, 3);

	    ако ((decc_feat_array[i].value >= feat_value_min) && (decc_feat_array[i].value <= feat_value_max))
	    {
		// Valid value.  Set it if necessary
		ако (feat_value != decc_feat_array[i].value)
		    sts = decc$feature_set_value(feat_index, 1, decc_feat_array[i].value);
	    }
	    иначе
		// Invalid DECC feature value
		printf("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_value, feat_value_min, decc_feat_array[i].name, feat_value_max);
	}
	иначе
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
const цяло spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
цяло LIB$INITIALIZE(void);
# pragma extern_model strict_refdef
цяло dmy_lib$initialize = (цяло) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
