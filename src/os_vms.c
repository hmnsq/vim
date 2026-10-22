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

#अन्तर्भाव "vim.h"

// define _generic_64 for use in time functions
#जर !defined(VAX)
# include <gen64def.h>
#अन्यथा
// based on Alpha's gen64def.h; the file is absent on VAX
प्रकारव्याख्या संरचना _generic_64 {
# pragma __nomember_alignment
    __union  {				     // You can treat me as...
	// long long is not available on VAXen
	// unsigned __int64 gen64$q_quadword; ...a single 64-bit value, or

	अचिन्हित पूर्णांक gen64$l_longword [2];   // ...two 32-bit values, or
	अचिन्हित लघु पूर्णांक gen64$w_word [4]; // ...four 16-bit values
    } gen64$r_quad_overlay;
} GENERIC_64;
#endif

प्रकारव्याख्या संरचना
{
    अक्षर	class;
    अक्षर	type;
    लघु	width;
    संघ
    {
	संरचना
	{
	    अक्षर	_basic[3];
	    अक्षर	length;
	}	y;
	पूर्णांक	basic;
    }	x;
    पूर्णांक		extended;
}	TT_MODE;

प्रकारव्याख्या संरचना
{
    लघु	buflen;
    लघु	itemcode;
    अक्षर	*bufadrs;
    पूर्णांक		*retlen;
}	ITEM;

प्रकारव्याख्या संरचना
{
    ITEM	equ;
    पूर्णांक		nul;
}	ITMLST1;

प्रकारव्याख्या संरचना
{
    ITEM	index;
    ITEM	string;
    पूर्णांक	nul;
}	ITMLST2;

स्थिर TT_MODE	orgmode;
स्थिर लघु	iochan;			// TTY I/O channel
स्थिर लघु	iosb[4];		// IO status block

स्थिर पूर्णांक vms_match_अंक = 0;
स्थिर पूर्णांक vms_match_free = 0;
स्थिर char_u **vms_fmatch = रिक्त;
स्थिर अक्षर *Fspec_Rms;		       // rms file spec, passed implicitly between routines



स्थिर TT_MODE	get_tty(शून्य);
स्थिर शून्य	set_tty(पूर्णांक row, पूर्णांक col);

#define EXPL_ALLOC_INC 64

#define EQN(S1,S2,LN) (म_भेदन(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) करा { पर्यंत (Str[1] == '/') ++Str; } पर्यंत (0)


/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    स्थिर शून्य
vul_desc(DESC *des, अक्षर *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_pointer = str;
    des->dsc$w_length = str ? म_माप(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    स्थिर शून्य
vul_item(ITEM *itm, लघु len, लघु cod, अक्षर *adr, पूर्णांक *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    शून्य
mch_settmode(tmode_T tmode)
{
    पूर्णांक	status;

    जर ( tmode == TMODE_RAW )
	set_tty(0, 0);
    अन्यथा
    {
	निवड (orgmode.width)
	{
	    स्थिती 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	थांबा;
	    स्थिती 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	थांबा;
	    पूर्वनिर्धारित:	थांबा;
	}
	out_flush();
	status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
					  &orgmode, आकार(TT_MODE), 0,0,0,0);
	जर (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    परत;
	(शून्य)sys$dassgn(iochan);
	iochan = 0;
    }
}

    स्थिर शून्य
set_tty(पूर्णांक row, पूर्णांक col)
{
    पूर्णांक		    status;
    TT_MODE	    newmode;		// New TTY mode bits
    स्थिर लघु    first_time = TRUE;

    जर (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    जर (col)
	newmode.width		 = col;
    जर (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    status = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, आकार(newmode), 0, 0, 0, 0);
    जर (status!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	परत;
}

    स्थिर TT_MODE
get_tty(शून्य)
{

    स्थिर $DESCRIPTOR(odsc,"SYS$OUTPUT");   // output descriptor

    पूर्णांक		status;
    TT_MODE	tt_mode;

    जर (!iochan)
	status = sys$assign(&odsc,&iochan,0,0);

    status = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, आकार(tt_mode), 0, 0, 0, 0);
    जर (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    परत(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    पूर्णांक
mch_get_shellsize(शून्य)
{
    TT_MODE	tmode;

    tmode = get_tty();			// get size from VMS
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    परत OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    शून्य
mch_set_shellsize(शून्य)
{
    set_tty(Rows, Columns);
    निवड (Columns)
    {
	स्थिती 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	थांबा;
	स्थिती 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	थांबा;
	पूर्वनिर्धारित:	थांबा;
    }
    out_flush();
    screen_start();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    स्थिर अक्षर		buffer[LNM$C_NAMLENGTH+1];
    char_u		*cp = रिक्त;
    अचिन्हित दीर्घ	attrib;
    पूर्णांक			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    अक्षर		*sbuf = रिक्त;

    vul_desc(&d_lognam, (अक्षर *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, आकार(पूर्णांक), LNM$_INDEX, (अक्षर *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    जर (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, रिक्त,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	जर (cp = alloc(lengte + 1))
	    म_नकल((अक्षर *)cp, buffer);
	परत(cp);
    }
    अन्यथा जर ((sbuf = दो_पर्या((अक्षर *)lognam)))
    {
	lengte = म_माप(sbuf) + 1;
	cp = alloc(lengte);
	जर (cp)
	    म_नकल((अक्षर *)cp, sbuf);
	परत cp;
    }
    अन्यथा
	परत(रिक्त);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    पूर्णांक
mch_setenv(अक्षर *var, अक्षर *मूल्य, पूर्णांक x)
{
    पूर्णांक		res, dum;
    दीर्घ	attrib = 0L;
    अक्षर	acmode = PSL$C_SUPER;	// needs SYSNAM privilege
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, मूल्य ? म_माप(मूल्य) : 0, मूल्य ? LNM$_STRING : 0,
	    मूल्य, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    परत((res == 1) ? 0 : -1);
}

    पूर्णांक
vms_sys(अक्षर *cmd, अक्षर *out, अक्षर *inp)
{
    DESC	cdsc, odsc, idsc;
    दीर्घ	status;

    जर (cmd)
	vul_desc(&cdsc, cmd);
    जर (out)
	vul_desc(&odsc, out);
    जर (inp)
	vul_desc(&idsc, inp);

    lib$spawn(cmd ? &cdsc : रिक्त,		// command string
	      inp ? &idsc : रिक्त,		// input file
	      out ? &odsc : रिक्त,		// output file
	      0, 0, 0, &status, 0, 0, 0, 0, 0, 0);
    परत status;
}

/*
 * Convert string to lowercase - most often filename
 */
    अक्षर *
vms_tolower( अक्षर *name )
{
    पूर्णांक i,nlen = म_माप(name);
    साठी (i = 0; i < nlen; i++)
	name[i] = TOLOWER_ASC(name[i]);
    परत name;
}

/*
 * Convert VMS system() or lib$spawn() return code to Unix-like exit value.
 */
    पूर्णांक
vms_sys_status(पूर्णांक status)
{
    जर (status != SS$_NORMAL && (status & STS$M_SUCCESS) == 0)
	परत status;		// Command failed.
    परत 0;
}

/*
 * vms_read()
 * function for low level char input
 *
 * Returns: input length
 */
    पूर्णांक
vms_read(अक्षर *inbuf, माप_प्रकार nbytes)
{
    पूर्णांक		status, function, len;
    TT_MODE	tt_mode;
    ITEM	itmlst[2];     // terminates on everything
    स्थिर दीर्घ trm_mask[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // whatever happened earlier we need an iochan here
    जर (!iochan)
	tt_mode = get_tty();

    // important: clean the inbuf
    स_रखो(inbuf, 0, nbytes);

    // set up the itemlist for the first read
    vul_item(&itmlst[0], 0, TRM$_MODIFIERS,
	 (अक्षर *)( TRM$M_TM_NOECHO  | TRM$M_TM_NOEDIT	 |
		   TRM$M_TM_NOFILTR | TRM$M_TM_TRMNOECHO |
		   TRM$M_TM_NORECALL) , 0);
    vul_item(&itmlst[1], आकार(trm_mask), TRM$_TERM, (अक्षर *)&trm_mask, 0);

    // wait forever for a char
    function = (IO$_READLBLK | IO$M_EXTEND);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf, nbytes-1, 0, 0, &itmlst, आकार(itmlst));
    len = म_माप(inbuf); // how many chars we got?

    // read immediately the rest in the IO queue
    function = (IO$_READLBLK | IO$M_TIMED | IO$M_ESCAPE | IO$M_NOECHO | IO$M_NOFILTR);
    status = sys$qiow(0, iochan, function, &iosb, 0, 0,
			 inbuf+len, nbytes-1-len, 0, 0, 0, 0);

    len = म_माप(inbuf); // return the total length

    परत len;
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further matches
 */
    स्थिर पूर्णांक
vms_wproc(अक्षर *name, पूर्णांक val)
{
    पूर्णांक i;
    स्थिर पूर्णांक vms_match_alloced = 0;

    जर (val == DECC$K_FOREIGN ) // foreign non VMS files are not counting
	परत 1;

    // accept all DECC$K_FILE and DECC$K_DIRECTORY
    जर (vms_match_अंक == 0)
    {
	// first time through, setup some things
	जर (रिक्त == vms_fmatch)
	{
	    vms_fmatch = ALLOC_MULT(char_u *, EXPL_ALLOC_INC);
	    जर (!vms_fmatch)
		परत 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	अन्यथा
	{
	    // re-use existing space
	    vms_match_free = vms_match_alloced;
	}
    }

    // make matches look uniform
    vms_remove_version(name);
    name=vms_tolower(name);

    // if name already exists, don't add it
    साठी (i = 0; i<vms_match_अंक; i++)
    {
	जर (0 == STRCMP((char_u *)name,vms_fmatch[i]))
	    परत 1;
    }
    जर (--vms_match_free == 0)
    {
	char_u **old_vms_fmatch = vms_fmatch;

	// add more space to store matches
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = vim_realloc(old_vms_fmatch,
		आकार(अक्षर **) * vms_match_alloced);
	जर (!vms_fmatch)
	{
	    vim_free(old_vms_fmatch);
	    परत 0;
	}
	vms_match_free = EXPL_ALLOC_INC;
    }
    vms_fmatch[vms_match_अंक] = vim_strsave((char_u *)name);

    ++vms_match_अंक;
    परत 1;
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
    पूर्णांक
mch_expand_wildcards(
    पूर्णांक		अंक_pat,
    char_u	**pat,
    पूर्णांक		*अंक_file,
    char_u	***file,
    पूर्णांक		flags)
{
    पूर्णांक		i, cnt = 0;
    char_u	buf[MAXPATHL];
    अक्षर       *result;
    पूर्णांक		dir;
    पूर्णांक files_alloced, files_free;

    *अंक_file = 0;			// default: no files found
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = ALLOC_MULT(char_u *, files_alloced);
    जर (*file == रिक्त)
    {
	*अंक_file = 0;
	परत FAIL;
    }
    साठी (i = 0; i < अंक_pat; i++)
    {
	// expand environment var or home dir
	जर (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	अन्यथा
	    STRCPY(buf,pat[i]);

	vms_match_अंक = 0; // reset collection counter
	result = decc$translate_vms(vms_fixfilename(buf));
	जर ( (पूर्णांक) result == 0 || (पूर्णांक) result == -1  )
	{
	    cnt = 0;
	}
	अन्यथा
	{
	    cnt = decc$to_vms(result, vms_wproc, 1 /*allow wild*/ , (flags & EW_DIR ? 0:1 ) /*allow directory*/) ;
	}
	जर (cnt > 0)
	    cnt = vms_match_अंक;

	जर (cnt < 1)
	    सुरूठेवा;

	साठी (i = 0; i < cnt; i++)
	{
	    // files should exist if expanding interactively
	    जर (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		सुरूठेवा;

	    // do not include directories
	    dir = (mch_isdir(vms_fmatch[i]));
	    जर (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		सुरूठेवा;

	    // Skip files that are not executable if we check for that.
	    जर (!dir && (flags & EW_EXEC)
		 && !mch_can_exe(vms_fmatch[i], रिक्त, !(flags & EW_SHELLCMD)))
		सुरूठेवा;

	    // allocate memory for pointers
	    जर (--files_free < 1)
	    {
		char_u **old_file = *file;

		files_alloced += EXPL_ALLOC_INC;
		*file = vim_realloc(old_file, आकार(char_u **) * files_alloced);
		जर (*file == रिक्त)
		{
		    vim_free(old_file);
		    *file = (char_u **)"";
		    *अंक_file = 0;
		    परत(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*अंक_file++] = vms_fmatch[i];
	}
    }
    परत OK;
}

    पूर्णांक
mch_expandpath(garray_T *gap, char_u *path, पूर्णांक flags)
{
    पूर्णांक		i,cnt = 0;
    अक्षर       *result;

    vms_match_अंक = 0;
    // the result from the decc$translate_vms needs to be handled
    // otherwise it might create ACCVIO error in decc$to_vms
    result = decc$translate_vms(vms_fixfilename(path));
    जर ( (पूर्णांक) result == 0 || (पूर्णांक) result == -1  )
    {
	cnt = 0;
    }
    अन्यथा
    {
	cnt = decc$to_vms(result, vms_wproc, 1 /*allow_wild*/, (flags & EW_DIR ? 0:1 ) /*allow directory*/);
    }
    जर (cnt > 0)
	cnt = vms_match_अंक;
    साठी (i = 0; i < cnt; i++)
    {
	जर (mch_getperm(vms_fmatch[i]) >= 0) // add existing file
	    addfile(gap, vms_fmatch[i], flags);
    }
    परत cnt;
}

/*
 * attempt to translate a mixed unix-vms file specification to pure vms
 */
    स्थिर शून्य
vms_unix_mixed_filespec(अक्षर *in, अक्षर *out)
{
    अक्षर *lastcolon;
    अक्षर *end_of_dir;
    अक्षर ch;
    पूर्णांक len;
    अक्षर *out_str=out;

    // copy vms filename portion up to last colon
    // (node and/or disk)
    lastcolon = म_खोजप(in, ':');   // find last colon
    जर (lastcolon != रिक्त)
    {
	len = lastcolon - in + 1;
	म_नकलन(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = रिक्त;	// default: no directory

    // start of directory portion
    ch = *in;
    जर ((ch == '[') || (ch == '/') || (ch == '<')) // start of directory(s) ?
    {
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    }
    अन्यथा जर (EQN(in, "../", 3))      // Unix parent directory?
    {
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    }
    अन्यथा
    {                                // not a special character
	पर्यंत (EQN(in, "./", 2))     // Ignore Unix "current dir"
	{
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    जर (म_अक्षर(in, '/') == रिक्त)     // any more Unix directories ?
    {
	म_नकल(out, in);             // No - get rest of the spec
	परत;
    }
    अन्यथा
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

    पर्यंत (*in != '\0')
    {
	ch = *in;
	जर ((ch == ']') || (ch == '/') || (ch == '>') )	// end of (sub)directory ?
	{
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	अन्यथा जर (EQN(in, "../", 3))     // Unix parent directory?
	{
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	अन्यथा
	{
	    पर्यंत (EQN(in, "./", 2))    // Ignore Unix "current dir"
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

    जर (end_of_dir != रिक्त) // Terminate directory portion
	*end_of_dir = ']';
}

/*
 * for decc$to_vms in vms_fixfilename
 */
    स्थिर पूर्णांक
vms_fspec_proc(अक्षर *fil, पूर्णांक val)
{
    म_नकल(Fspec_Rms,fil);
    परत(1);
}

/*
 * change unix and mixed filenames to VMS
 */
    शून्य *
vms_fixfilename(शून्य *instring)
{
    स्थिर अक्षर		*buf = रिक्त;
    स्थिर माप_प्रकार	buflen = 0;
    माप_प्रकार		len;

    // get a big-enough buffer
    len = म_माप(instring) + 1;
    जर (len > buflen)
    {
	buflen = len + 128;
	buf = vim_realloc(buf, buflen * आकार(अक्षर));
    }

#ifdef DEBUG
    अक्षर		 *tmpbuf = रिक्त;
    tmpbuf = ALLOC_MULT(अक्षर, buflen);
    म_नकल(tmpbuf, instring);
#endif

    Fspec_Rms = buf;				// for decc$to_vms

    जर (म_अक्षर(instring,'/') == रिक्त)
	// It is already a VMS file spec
	म_नकल(buf, instring);
    अन्यथा जर (म_अक्षर(instring,'"') == रिक्त)	// password in the path?
    {
	// Seems it is a regular file, let guess that it is pure Unix fspec
	जर ( (म_अक्षर(instring,'[') == रिक्त) && (म_अक्षर(instring,'<') == रिक्त) &&
	     (म_अक्षर(instring,']') == रिक्त) && (म_अक्षर(instring,'>') == रिक्त) &&
	     (म_अक्षर(instring,':') == रिक्त) )
	{
	    // It must be a truly unix fspec
	    decc$to_vms(instring, vms_fspec_proc, 0, 0);
	}
	अन्यथा
	{
	    // It is a mixed fspec
	    vms_unix_mixed_filespec(instring, buf);
	}
    }
    अन्यथा
	// we have a password in the path
	// decc$ functions can not handle
	// this is our only hope to resolv
	vms_unix_mixed_filespec(instring, buf);

    परत buf;
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
    शून्य
vms_remove_version(शून्य * fname)
{
    char_u	*dp;	        // Dot pointer
    char_u	*rp;            // Right pointer
    पूर्णांक		done = 0;
    पूर्णांक		vdigits = 0;

    rp = (char_u *)fname+ म_माप( (अक्षर *)fname)- 1;    // Rightmost char
    पर्यंत ((done == 0) && (rp > (char_u *)fname))       // Count digits
    {
	जर (है_अंक( *rp))
	{
	    vdigits++;
	    *rp--;
	}
	अन्यथा
	{
	    done = 1;                                   // Quit at non-digit
	}
    } // while (Count digits)

    जर (vdigits  <= 5)                  // If likely version digits, check delimiter
    {                                   // (Could check for <= 32767, not just five digits or fewer.)
	जर (*rp == (char_u)';')
	{
	    जर ((rp >= (char_u *)fname) && (*(rp- 1) != (char_u)'^'))
	    {                           // Unescaped ";"
		*rp = '\0';             // Trim off ";nnn"
	    }
	}
	अन्यथा जर (*rp == (char_u)'.')    // Last of multiple dots?
	{
	    जर ((rp >= (char_u *)fname) && (*(rp- 1) != '^'))
	    {                           // Unescaped dot.  Version requires previous one
		dp = rp- 1;             // Scan chars before "."
		done = 0;
		पर्यंत ((done == 0) && (dp >= (char_u *)fname))
		{
		    जर ((*dp == ']') || (*dp == '>') || (*dp == ':') || (*dp == '/'))
		    {                   // Possible VMS dev:[dir] delimiter (or UNIX "/")
			जर ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
			{               // Unescaped dev:[dir] (or /) delimiter
			    done = 1;   // No previous dot found in name
			}
		    }
		    अन्यथा जर (*dp == '.')
		    {                   // Possible dot delimiter
			जर ((dp >= (char_u *)fname) && (*(dp- 1) != '^'))
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

    परत ;
}

संरचना typeahead_st {
    अचिन्हित लघु numchars;
    अचिन्हित अक्षर  firstchar;
    अचिन्हित अक्षर  reserved0;
    अचिन्हित दीर्घ  reserved1;
} typeahead;

/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 */
    पूर्णांक
RealWaitForChar(
    पूर्णांक		fd UNUSED, // always read from iochan
    दीर्घ	msec,
    पूर्णांक		*check_for_gpm UNUSED,
    पूर्णांक		*interrupted)
{
    पूर्णांक status;
    संरचना _generic_64 time_curr;
    संरचना _generic_64 time_diff;
    संरचना _generic_64 time_out;
    अचिन्हित पूर्णांक convert_operation = LIB$K_DELTA_SECONDS_F;
    दशांश sec =(दशांश) msec/1000;

    // make sure the iochan is set
    जर (!iochan)
	get_tty();

    जर (sec > 0)
    {
	// time-out specified; convert it to absolute time
	// sec>0 requirement of lib$cvt[fs]_to_internal_time()

	// get current time (number of 100ns ticks since the VMS Epoch)
	status = sys$gettim(&time_curr);
	जर (status != SS$_NORMAL)
	    परत 0; // error
	// construct the delta time

/* On all non-VAX hardware architectures, the "CC /FLOAT=option"
 * determines the floating-point format.  The default format on Alpha
 * is VAX; on IA64 and x86_64 it's IEEE.  But, except on VAX, the user
 * can specify either.   What matters here is the actual floating-point
 * format being used, not the hardware architecture.  Choose the
 * appropriate time conversion function accordingly.
 */
#जर __IEEE_FLOAT
// allow fallback for older Alphas
# ifdef lib$cvts_to_internal_time
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvts_to_internal_time // IEEE
# अन्यथा
#  define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time
# endif
#अन्यथा
# define LIB_CVTX_TO_INTERNAL_TIME lib$cvtf_to_internal_time // VAX
#endif // __IEEE_FLOAT CVTS

	status = LIB_CVTX_TO_INTERNAL_TIME(
		&convert_operation, &sec, &time_diff);
	जर (status != LIB$_NORMAL)
	    परत 0; // error
	// add them up
	status = lib$add_times(
		&time_curr,
		&time_diff,
		&time_out);
	जर (status != LIB$_NORMAL)
	    परत 0; // error
    }

    पर्यंत (TRUE)
    {
	// select()
	status = sys$qiow(0, iochan, IO$_SENSEMODE | IO$M_TYPEAHDCNT, iosb,
		0, 0, &typeahead, 8, 0, 0, 0, 0);
	जर (status != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
	    परत 0; // error

	जर (typeahead.numchars)
	    परत 1; // ready to read

	// there's nothing to read; what now?
	जर (msec == 0)
	{
	    // immediate time-out; return impatiently
	    परत 0;
	}
	अन्यथा जर (msec < 0)
	{
	    // no time-out; wait on indefinitely
	    परत 1; // fakeout to force a wait in vms_read()
	}
	अन्यथा
	{
	    // time-out needs to be checked
	    status = sys$gettim(&time_curr);
	    जर (status != SS$_NORMAL)
		परत 0; // error

	    status = lib$sub_times(
		    &time_out,
		    &time_curr,
		    &time_diff);
	    जर (status != LIB$_NORMAL)
		परत 0; // error, incl. time_diff < 0 (i.e. time-out)

	    // otherwise wait some more
	}
    }
}

#जर !defined( __VAX) && (__CRTL_VER >= 70301000)

# include <stdio.h>
# include <unixlib.h>

// Structure to hold a DECC$* feature name and its desired value

प्रकारव्याख्या संरचना
   {
   अक्षर *name;
   पूर्णांक मूल्य;
   } decc_feat_t;

पूर्णांक vms_init_done = -1;

decc_feat_t decc_feat_array[] = {

   // Preserve command-line case with SET PROCESS/PARSE_STYLE=EXTENDED
 { "DECC$ARGV_PARSE_STYLE", 1 },

   // Preserve case for file names on ODS5 disks
 { "DECC$EFS_CASE_PRESERVE", 1 },

   // Enable multiple dots (and most characters) in ODS5 file names,
   // while preserving VMS-ness of ";version"
 { "DECC$EFS_CHARSET", 1 },

   // List terminator
 { (अक्षर *)रिक्त, 0 } };


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

स्थिर शून्य
vms_init(शून्य)
{
    // Set the global flag to indicate that LIB$INITIALIZE worked

    vms_init_done = 1;

    पूर्णांक feat_index;
    पूर्णांक feat_मूल्य;
    पूर्णांक feat_मूल्य_max;
    पूर्णांक feat_मूल्य_min;
    पूर्णांक i;
    पूर्णांक sts;

    // Loop through all items in the decc_feat_array[]
    साठी (i = 0; decc_feat_array[i].name != रिक्त; i++)
    {
	// Get the feature index
	feat_index = decc$feature_get_index(decc_feat_array[i].name);
	जर (feat_index >= 0)
	{
	    // Valid item.  Collect its properties
	    feat_मूल्य = decc$feature_get_मूल्य(feat_index, 1);
	    feat_मूल्य_min = decc$feature_get_मूल्य(feat_index, 2);
	    feat_मूल्य_max = decc$feature_get_मूल्य(feat_index, 3);

	    जर ((decc_feat_array[i].मूल्य >= feat_मूल्य_min) && (decc_feat_array[i].मूल्य <= feat_मूल्य_max))
	    {
		// Valid value.  Set it if necessary
		जर (feat_मूल्य != decc_feat_array[i].मूल्य)
		    sts = decc$feature_set_मूल्य(feat_index, 1, decc_feat_array[i].मूल्य);
	    }
	    अन्यथा
		// Invalid DECC feature value
		म_लिखो("INVALID DECC FEATURE VALUE, %d: %d <= %s <= %d.\n",
			feat_मूल्य, feat_मूल्य_min, decc_feat_array[i].name, feat_मूल्य_max);
	}
	अन्यथा
	    // Invalid DECC feature name
	    म_लिखो("UNKNOWN DECC FEATURE: %s.\n", decc_feat_array[i].name);
    }
}


/* Get "vms_init()" into a valid, loaded LIB$INITIALIZE PSECT. */

# pragma nostandard

/* Establish the LIB$INITIALIZE PSECTs, with proper alignment and
 * other attributes.  Note that "nopic" is significant only on VAX.
 */
# pragma extern_model save

# pragma extern_model strict_refdef "LIB$INITIALIZE" 2, nopic, nowrt
शून्य (*स्थिरांक x_vms_init)() = vms_init;

# pragma extern_model strict_refdef "LIB$INITIALIZ" 2, nopic, nowrt
स्थिरांक पूर्णांक spare[ 8] = { 0 };

# pragma extern_model restore

// Fake reference to ensure loading the LIB$INITIALIZE PSECT

# pragma extern_model save
पूर्णांक LIB$INITIALIZE(शून्य);
# pragma extern_model strict_refdef
पूर्णांक dmy_lib$initialize = (पूर्णांक) LIB$INITIALIZE;
# pragma extern_model restore

# pragma standard

#endif // !defined( __VAX) && (__CRTL_VER >= 70301000)
