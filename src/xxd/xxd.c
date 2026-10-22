/* xxd: my hexdump facility. jw
 *
 *  2.10.90 changed to word output
 *  3.03.93 new indent style, dumb bug inserted and fixed.
 *	    -c option, mls
 * 26.04.94 better option parser, -ps, -l, -s added.
 *  1.07.94 -r badly needs - as input file.  Per default autoskip over
 *	       consecutive lines of zeroes, as unix od does.
 *	    -a shows them too.
 *	    -i dump as c-style #include "file.h"
 *  1.11.95 if "xxd -i" knows the filename, an 'unsigned char filename_bits[]'
 *	    array is written in correct c-syntax.
 *	    -s improved, now defaults to absolute seek, relative requires a '+'.
 *	    -r improved, now -r -s -0x... is supported.
 *	       change/suppress leading '\0' bytes.
 *	    -l n improved: stops exactly after n bytes.
 *	    -r improved, better handling of partial lines with trailing garbage.
 *	    -r improved, now -r -p works again!
 *	    -r improved, less flushing, much faster now! (that was silly)
 *  3.04.96 Per repeated request of a single person: autoskip defaults to off.
 * 15.05.96 -v added. They want to know the version.
 *	    -a fixed, to show last line inf file ends in all zeros.
 *	    -u added: Print upper case hex-letters, as preferred by unix bc.
 *	    -h added to usage message. Usage message extended.
 *	    Now using outfile if specified even in normal mode, aehem.
 *	    No longer mixing of ints and longs. May help doze people.
 *	    Added binify ioctl for same reason. (Enough Doze stress for 1996!)
 * 16.05.96 -p improved, removed occasional superfluous linefeed.
 * 20.05.96 -l 0 fixed. tried to read anyway.
 * 21.05.96 -i fixed. now honours -u, and prepends __ to numeric filenames.
 *	    compile -DWIN32 for NT or W95. George V. Reilly, * -v improved :-)
 *	    support --gnuish-longhorn-options
 * 25.05.96 MAC support added: CodeWarrior already uses ``outline'' in Types.h
 *	    which is included by MacHeaders (Axel Kielhorn). Renamed to
 *	    xxdline().
 *  7.06.96 -i printed 'int' instead of 'char'. *blush*
 *	    added Bram's OS2 ifdefs...
 * 18.07.96 gcc -Wall @ SunOS4 is now silent.
 *	    Added osver for MSDOS/DJGPP/WIN32.
 * 29.08.96 Added size_t to strncmp() for Amiga.
 * 24.03.97 Windows NT support (Phil Hanna). Clean exit for Amiga WB (Bram)
 * 02.04.97 Added -E option, to have EBCDIC translation instead of ASCII
 *	    (azc10@yahoo.com)
 * 22.05.97 added -g (group octets) option (jcook@namerica.kla.com).
 * 23.09.98 nasty -p -r misfeature fixed: slightly wrong output, when -c was
 *	    missing or wrong.
 * 26.09.98 Fixed: 'xxd -i infile outfile' did not truncate outfile.
 * 27.10.98 Fixed: -g option parser required blank.
 *	    option -b added: 01000101 binary output in normal format.
 * 16.05.00 Added VAXC changes by Stephen P. Wall
 * 16.05.00 Improved MMS file and merge for VMS by Zoltan Arpadffy
 * 2011 March  Better error handling by Florian Zumbiehl.
 * 2011 April  Formatting by Bram Moolenaar
 * 08.06.2013  Little-endian hexdump (-e) and offset (-o) by Vadim Vygonets.
 * 11.01.2019  Add full 64/32 bit range to -o and output by Christer Jensen.
 * 04.02.2020  Add -d for decimal offsets by Aapo Rantalainen
 * 14.01.2022  Disable extra newlines with -c0 -p by Erik Auerswald.
 * 20.06.2022  Permit setting the variable names used by -i by David Gow
 * 31.08.2023  -R never/auto/always prints colored output
 * 06.10.2023  enable -r -b to reverse bit dumps
 * 12.01.2024  disable auto-conversion for z/OS (MVS)
 * 17.01.2024  use size_t instead of usigned int for code-generation (-i), #13876
 * 25.01.2024  revert the previous patch (size_t instead of unsigned int)
 * 10.02.2024  fix buffer-overflow when writing color output to buffer, #14003
 * 10.05.2024  fix another buffer-overflow when writing colored output to buffer, #14738
 * 10.09.2024  Support -b and -i together, #15661
 * 19.10.2024  -e did add an extra space #15899
 * 11.11.2024  improve end-of-options argument parser #9285
 * 07.12.2024  fix overflow with xxd --autoskip and large sparse files #16175
 * 15.06.2025  improve color code logic
 * 08.08.2025  fix overflow with bitwise output
 * 20.08.2025  remove external library call for autoconversion on z/OS (MVS)
 * 24.08.2025  avoid NULL dereference with autoskip colorless
 * 26.11.2025  update indent in exit_with_usage()
 * 19.03.2026  Add -t option to end output with terminating null
 * 25.03.2026  Fix color output issues
 * 26.04.2026  Use unsigned long for printing offsets
 * 31.05.2026  Colorize binary output
 * 15.06.2026  Fix UB in huntype()
 *
 * (c) 1990-1998 by Juergen Weigert (jnweiger@gmail.com)
 *
 * I hereby grant permission to distribute and use xxd
 * under X11-MIT or GPL-2.0 (at the user's choice).
 *
 * Contributions by Bram Moolenaar et al.
 */

/* Visual Studio 2005 has 'deprecated' many of the standard CRT functions */
#જો _MSC_VER >= 1400
# define _CRT_SECURE_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#endif
#જો !defined(CYGWIN) && defined(__CYGWIN__)
# define CYGWIN
#endif

#જો (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__)
# define _XOPEN_SOURCE 700   /* for fdopen() */
#endif

#અન્તર્ભાવ <stdio.h>
#ifdef VAXC
# include <file.h>
#નહીંતર
# include <fcntl.h>
#endif
#જો defined(WIN32) || defined(CYGWIN)
# include <io.h>	/* for setmode() */
#endif
#ifdef WIN32
# include <windows.h>
#endif
#ifdef UNIX
# include <unistd.h>
#endif
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <ctype.h>
#અન્તર્ભાવ <limits.h>
#જો __MWERKS__ && !defined(BEBOX)
# include <unix.h>	/* for fdopen() on MAC */
#endif


/*  This corrects the problem of missing prototypes for certain functions
 *  in some GNU installations (e.g. SunOS 4.1.x).
 *  Darren Hiebert <darren@hmi.com> (sparc-sun-sunos4.1.3_U1/2.7.2.2)
 */
#જો defined(__GNUC__) && defined(__STDC__)
# ifndef __USE_FIXED_PROTOTYPES__
#  define __USE_FIXED_PROTOTYPES__
# endif
#endif

#ifndef __USE_FIXED_PROTOTYPES__
/*
 * This is historic and works only if the compiler really has no prototypes:
 *
 * Include prototypes for Sun OS 4.x, when using an ANSI compiler.
 * FILE is defined on OS 4.x, not on 5.x (Solaris).
 * if __SVR4 is defined (some Solaris versions), don't include this.
 */
# જો defined(sun) && defined(FILE) && !defined(__SVR4) && defined(__STDC__)
#  define __P(a) a
/* excerpt from my sun_stdlib.h */
બાહ્ય પૂર્ણાંક fprintf __P((FILE *, અક્ષર *, ...));
બાહ્ય પૂર્ણાંક fputs   __P((અક્ષર *, FILE *));
બાહ્ય પૂર્ણાંક _flsbuf __P((unsigned અક્ષર, FILE *));
બાહ્ય પૂર્ણાંક _filbuf __P((FILE *));
બાહ્ય પૂર્ણાંક fflush  __P((FILE *));
બાહ્ય પૂર્ણાંક fclose  __P((FILE *));
બાહ્ય પૂર્ણાંક fseek   __P((FILE *, long, પૂર્ણાંક));
બાહ્ય પૂર્ણાંક rewind  __P((FILE *));

બાહ્ય શૂન્ય perror __P((અક્ષર *));
# endif
#endif

અક્ષર version[] = "xxd 2026-06-16 by Juergen Weigert et al.";
#ifdef WIN32
અક્ષર osver[] = " (Win32)";
#નહીંતર
અક્ષર osver[] = "";
#endif

#જો defined(WIN32)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "wt")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_દ્વિઅંકી) : O_CREAT)
# define BIN_ASSIGN(fp, yes) setmode(fileno(fp), (yes) ? O_દ્વિઅંકી : O_TEXT)
# define PATH_SEP '\\'
#અથવાજો defined(CYGWIN)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "w")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_દ્વિઅંકી) : O_CREAT)
# define BIN_ASSIGN(fp, yes) ((yes) ? (શૂન્ય) setmode(fileno(fp), O_દ્વિઅંકી) : (શૂન્ય) (fp))
# define PATH_SEP '/'
#નહીંતર
# ifdef VMS
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ASSIGN(fp, dummy) fp
#  define PATH_SEP ']'
#  define FILE_SEP '.'
# નહીંતર
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ASSIGN(fp, dummy) fp
#  define PATH_SEP '/'
# endif
#endif

/* open has only to arguments on the Mac */
#જો __MWERKS__
# define OPEN(name, mode, umask) open(name, mode)
#નહીંતર
# define OPEN(name, mode, umask) open(name, mode, umask)
#endif

#ifdef AMIGA
# define STRNCMP(s1, s2, l) strncmp(s1, s2, (size_t)l)
#નહીંતર
# define STRNCMP(s1, s2, l) strncmp(s1, s2, l)
#endif

#ifndef __P
# જો defined(__STDC__) || defined(WIN32)
#  define __P(a) a
# નહીંતર
#  define __P(a) ()
# endif
#endif

#define TRY_SEEK	/* attempt to use lseek, or skip forward by reading */
#define COLS 256	/* change here, if you ever need more columns */

/*
 * LLEN is the maximum length of a line; other than the visible characters
 * we need to consider also the escape color sequence prologue/epilogue ,
 * (11 bytes for each character).
 */
#define LLEN \
    (39            /* addr: ⌈log10(ULONG_MAX)⌉ if "-d" flag given. We assume ULONG_MAX = 2**128 */ \
    + 2            /* ": " */ \
    + 13 * COLS    /* hex dump with colors */ \
    + (COLS - 1)   /* whitespace between groups if "-g1" option given and "-c" maxed out */ \
    + 2            /* whitespace */ \
    + 12 * COLS    /* ASCII dump with colors */ \
    + 2)           /* "\n\0" */

/*
 * LLEN_NO_COLOR is the maximum length of a line excluding the colors.
 */
#define LLEN_NO_COLOR \
    (39            /* addr: ⌈log10(ULONG_MAX)⌉ if "-d" flag given. We assume ULONG_MAX = 2**128 */ \
    + 2            /* ": " */ \
    + 9 * COLS     /* hex dump, worst case: bitwise output using -b */ \
    + 2            /* whitespace */ \
    + COLS         /* ASCII dump */ \
    + 2)           /* "\n\0" */

અક્ષર hexxa[] = "0123456789abcdef0123456789ABCDEF", *hexx = hexxa;

/* the different hextypes known by this program: */
#define HEX_NORMAL         0x00 /* no flags set */
#define HEX_POSTSCRIPT     0x01
#define HEX_CINCLUDE       0x02
#define HEX_BITS           0x04 /* not hex a dump, but bits: 01111001 */
#define HEX_LITTLEENDIAN   0x08

#define CONDITIONAL_CAPITALIZE(c) (capitalize ? toupper((unsigned અક્ષર)(c)) : (c))

#define COLOR_PROLOGUE(color) \
l_colored[c++] = '\033'; \
l_colored[c++] = '['; \
l_colored[c++] = '1'; \
l_colored[c++] = ';'; \
l_colored[c++] = '3'; \
l_colored[c++] = (color); \
l_colored[c++] = 'm';

#define COLOR_EPILOGUE \
l_colored[c++] = '\033'; \
l_colored[c++] = '['; \
l_colored[c++] = '0'; \
l_colored[c++] = 'm';
#define COLOR_RED '1'
#define COLOR_GREEN '2'
#define COLOR_YELLOW '3'
#define COLOR_BLUE '4'
#define COLOR_WHITE '7'

સ્થિર અક્ષર *pname;

  સ્થિર શૂન્ય
exit_with_usage(શૂન્ય)
{
  fprintf(stderr, "Usage:\n       %s [options] [infile [outfile]]\n", pname);
  fprintf(stderr, "    or\n       %s -r [-s [-]offset] [-c cols] [-ps] [infile [outfile]]\n", pname);
  fprintf(stderr, "Options:\n"
		  "    -a          toggle autoskip: A single '*' replaces nul-lines. Default off.\n"
		  "    -b          binary digit dump (incompatible with -ps). Default hex.\n"
		  "    -C          capitalize variable names in C include file style (-i).\n"
		  "    -c cols     format <cols> octets per line. Default 16 (-i: 12, -ps: 30).\n"
		  "    -E          show characters in EBCDIC. Default ASCII.\n"
		  "    -e          little-endian dump (incompatible with -ps,-i,-r).\n"
		  "    -g bytes    number of octets per group in normal output. Default 2 (-e: 4).\n"
		  "    -h          print this summary.\n"
		  "    -i          output in C include file style.\n"
		  "    -t          append terminating zero to C include output (-i).\n"
		  "    -l len      stop after <len> octets.\n"
		  "    -n name     set the variable name used in C include output (-i).\n"
		  "    -o off      add <off> to the displayed file position.\n"
		  "    -ps         output in postscript plain hexdump style.\n"
		  "    -r          reverse operation: convert (or patch) hexdump into binary.\n"
		  "    -r -s off   revert with <off> added to file positions found in hexdump.\n"
		  "    -d          show offset in decimal instead of hex.\n");
  fprintf(stderr, "    -s %sseek  start at <seek> bytes abs. %sinfile offset.\n",
#ifdef TRY_SEEK
	  "[+][-]", "(or +: rel.) ");
#નહીંતર
	  "", "");
#endif
  fprintf(stderr, "    -u          use upper case hex letters.\n"
		  "    -R when     colorize the output; <when> can be 'always', 'auto' or 'never'. Default: 'auto'.\n"
		  "    -v          show version: \"%s%s\".\n", version, osver);
  exit(1);
}

  સ્થિર શૂન્ય
perror_exit(પૂર્ણાંક ret)
{
  fprintf(stderr, "%s: ", pname);
  perror(ખાલી);
  exit(ret);
}

  સ્થિર શૂન્ય
error_exit(પૂર્ણાંક ret, અક્ષર *સંદેશ)
{
  fprintf(stderr, "%s: %s\n", pname, સંદેશ);
  exit(ret);
}

  સ્થિર પૂર્ણાંક
getc_or_die(FILE *fpi)
{
  પૂર્ણાંક c = getc(fpi);
  જો (c == EOF && ferror(fpi))
    perror_exit(2);
  પાછા c;
}

  સ્થિર શૂન્ય
putc_or_die(પૂર્ણાંક c, FILE *fpo)
{
  જો (putc(c, fpo) == EOF)
    perror_exit(3);
}

  સ્થિર શૂન્ય
fputs_or_die(અક્ષર *s, FILE *fpo)
{
  જો (fputs(s, fpo) == EOF)
    perror_exit(3);
}

/* Use a macro to allow for different arguments. */
#define FPRINTF_OR_DIE(args) જો (fprintf args < 0) perror_exit(3)

  સ્થિર શૂન્ય
fclose_or_die(FILE *fpi, FILE *fpo)
{
  જો (fclose(fpo) != 0)
    perror_exit(3);
  જો (fclose(fpi) != 0)
    perror_exit(2);
}

/*
 * If "c" is a hex digit, return the value.
 * Otherwise return -1.
 */
  સ્થિર પૂર્ણાંક
parse_hex_digit(પૂર્ણાંક c)
{
  પાછા (c >= '0' && c <= '9') ? c - '0'
	: (c >= 'a' && c <= 'f') ? c - 'a' + 10
	: (c >= 'A' && c <= 'F') ? c - 'A' + 10
	: -1;
}

/*
 * If "c" is a bin digit, return the value.
 * Otherwise return -1.
 */
  સ્થિર પૂર્ણાંક
parse_bin_digit(પૂર્ણાંક c)
{
  પાછા (c >= '0' && c <= '1') ? c - '0'
	: -1;
}

/*
 * Ignore text on "fpi" until end-of-line or end-of-file.
 * Return the '\n' or EOF character.
 * When an error is encountered exit with an error message.
 */
  સ્થિર પૂર્ણાંક
skip_to_eol(FILE *fpi, પૂર્ણાંક c)
{
  જ્યારે (c != '\n' && c != EOF)
    c = getc_or_die(fpi);
  પાછા c;
}

/*
 * Max. cols binary characters are decoded from the input stream per line.
 * Two adjacent garbage characters after evaluated data delimit valid data.
 * Everything up to the next newline is discarded.
 *
 * The name is historic and came from 'undo type opt h'.
 */
  સ્થિર પૂર્ણાંક
huntype(
  FILE *fpi,
  FILE *fpo,
  પૂર્ણાંક cols,
  પૂર્ણાંક hextype,
  long base_off)
{
  પૂર્ણાંક c, ign_garb = 1, n1 = -1, n2 = 0, n3 = 0, p = cols, bt = 0, b = 0, bcnt = 0;
  long have_off = 0, want_off = 0;

  rewind(fpi);

  જ્યારે ((c = getc(fpi)) != EOF)
    {
      જો (c == '\r')	/* Doze style input file? */
	ચાલુ;

      /* Allow multiple spaces.  This doesn't work when there is normal text
       * after the hex codes in the last line that looks like hex, thus only
       * use it for PostScript format. */
      જો (hextype == HEX_POSTSCRIPT && (c == ' ' || c == '\n' || c == '\t'))
	ચાલુ;

      જો (hextype == HEX_NORMAL || hextype == HEX_POSTSCRIPT)
	{
	  n3 = n2;
	  n2 = n1;

	  n1 = parse_hex_digit(c);
	  જો (n1 == -1 && ign_garb)
	    ચાલુ;
	}
      નહીંતર /* HEX_BITS */
	{
	  n1 = parse_hex_digit(c);
	  જો (n1 == -1 && ign_garb)
	    ચાલુ;

	  bt = parse_bin_digit(c);
	  જો (bt != -1)
	    {
	      /* shift via unsigned to avoid signed overflow on bad input */
	      b = (પૂર્ણાંક)(((unsigned)b << 1) | (unsigned)bt);
	      ++bcnt;
	    }
	}

      ign_garb = 0;

      જો ((hextype != HEX_POSTSCRIPT) && (p >= cols))
	{
	  જો (hextype == HEX_NORMAL)
	    {
	      જો (n1 < 0)
		{
		  p = 0;
		  ચાલુ;
		}
	      want_off = (long)(((unsigned long)want_off << 4) | (unsigned)n1);
	    }
	  નહીંતર /* HEX_BITS */
	    {
	      જો (n1 < 0)
		{
		  p = 0;
		  bcnt = 0;
		  ચાલુ;
		}
	      want_off = (long)(((unsigned long)want_off << 4) | (unsigned)n1);
	    }
	  ચાલુ;
	}

      જો (base_off + want_off != have_off)
	{
	  જો (fflush(fpo) != 0)
	    perror_exit(3);
#ifdef TRY_SEEK
	  જો (fseek(fpo, base_off + want_off - have_off, SEEK_CUR) >= 0)
	    have_off = base_off + want_off;
#endif
	  જો (base_off + want_off < have_off)
	    error_exit(5, "Sorry, cannot seek backwards.");
	  માટે (; have_off < base_off + want_off; have_off++)
	    putc_or_die(0, fpo);
	}

      જો (hextype == HEX_NORMAL || hextype == HEX_POSTSCRIPT)
	{
	  જો (n2 >= 0 && n1 >= 0)
	    {
	      putc_or_die((n2 << 4) | n1, fpo);
	      have_off++;
	      want_off++;
	      n1 = -1;
	      જો (!hextype && (++p >= cols))
	      /* skip the rest of the line as garbage */
	      c = skip_to_eol(fpi, c);
	    }
	  નહીંતર જો (n1 < 0 && n2 < 0 && n3 < 0)
	    /* already stumbled into garbage, skip line, wait and see */
	    c = skip_to_eol(fpi, c);
	}
      નહીંતર /* HEX_BITS */
	{
	  જો (bcnt == 8)
	    {
	      putc_or_die(b, fpo);
	      have_off++;
	      want_off++;
	      b = 0;
	      bcnt = 0;
	      જો (++p >= cols)
		/* skip the rest of the line as garbage */
		 c = skip_to_eol(fpi, c);
	    }
	}

      જો (c == '\n')
	{
	  જો (hextype == HEX_NORMAL || hextype == HEX_BITS)
	    want_off = 0;
	  p = cols;
	  ign_garb = 1;
	}
    }
  જો (fflush(fpo) != 0)
    perror_exit(3);
#ifdef TRY_SEEK
  fseek(fpo, 0L, SEEK_END);
#endif
  fclose_or_die(fpi, fpo);
  પાછા 0;
}


/*
 * Print line l with given colors.
 */
  સ્થિર શૂન્ય
print_colored_લીટી(FILE *fp, અક્ષર *l, અક્ષર *colors)
{
  સ્થિર અક્ષર l_colored[LLEN+1];

  જો (colors)
    {
      પૂર્ણાંક c = 0;
      જો (colors[0])
	{
	  COLOR_PROLOGUE(colors[0])
	}
      l_colored[c++] = l[0];
      પૂર્ણાંક i;
      માટે (i = 1; l[i]; i++)
	{
	  જો (colors[i] != colors[i-1])
	    {
	      જો (colors[i-1])
		{
		  COLOR_EPILOGUE
		}
	      જો (colors[i])
		{
		  COLOR_PROLOGUE(colors[i])
		}
	    }
	  l_colored[c++] = l[i];
	}

      જો (colors[i])
	{
	  COLOR_EPILOGUE
	}
      l_colored[c++] = '\0';

      fputs_or_die(l_colored, fp);
    }
  નહીંતર
    fputs_or_die(l, fp);
}

/*
 * Print line l with given colors. If nz is false, xxdline regards the line as a line of
 * zeroes. If there are three or more consecutive lines of zeroes,
 * they are replaced by a single '*' character.
 *
 * If the output ends with more than two lines of zeroes, you
 * should call xxdline again with l being the last line and nz
 * negative. This ensures that the last line is shown even when
 * it is all zeroes.
 *
 * If nz is always positive, lines are never suppressed.
 */
  સ્થિર શૂન્ય
xxdline(FILE *fp, અક્ષર *l, અક્ષર *colors, પૂર્ણાંક nz)
{
  સ્થિર અક્ષર z[LLEN_NO_COLOR+1];
  સ્થિર અક્ષર z_colors[LLEN_NO_COLOR+1];
  સ્થિર signed અક્ષર zero_seen = 0;

  જો (!nz && zero_seen == 1)
    {
      strcpy(z, l);
      જો (colors)
	  memcpy(z_colors, colors, strlen(z));
    }

  જો (nz || !zero_seen++)
    {
      જો (nz)
	{
	  જો (nz < 0)
	    zero_seen--;
	  જો (zero_seen == 2)
	    print_colored_લીટી(fp, z, z_colors);
	  જો (zero_seen > 2)
	    fputs_or_die("*\n", fp);
	}
      જો (nz >= 0 || zero_seen > 0)
	print_colored_લીટી(fp, l, colors);

      જો (nz)
	zero_seen = 0;
    }

  /* If zero_seen > 3, then its exact value doesn't matter, so long as it
   * remains >3 and incrementing it will not cause overflow. */
  જો (zero_seen >= 0x7F)
    zero_seen = 4;
}

/* This is an EBCDIC to ASCII conversion table */
/* from a proposed BTL standard April 16, 1979 */
સ્થિર unsigned અક્ષર etoa64[] =
{
    0040,0240,0241,0242,0243,0244,0245,0246,
    0247,0250,0325,0056,0074,0050,0053,0174,
    0046,0251,0252,0253,0254,0255,0256,0257,
    0260,0261,0041,0044,0052,0051,0073,0176,
    0055,0057,0262,0263,0264,0265,0266,0267,
    0270,0271,0313,0054,0045,0137,0076,0077,
    0272,0273,0274,0275,0276,0277,0300,0301,
    0302,0140,0072,0043,0100,0047,0075,0042,
    0303,0141,0142,0143,0144,0145,0146,0147,
    0150,0151,0304,0305,0306,0307,0310,0311,
    0312,0152,0153,0154,0155,0156,0157,0160,
    0161,0162,0136,0314,0315,0316,0317,0320,
    0321,0345,0163,0164,0165,0166,0167,0170,
    0171,0172,0322,0323,0324,0133,0326,0327,
    0330,0331,0332,0333,0334,0335,0336,0337,
    0340,0341,0342,0343,0344,0135,0346,0347,
    0173,0101,0102,0103,0104,0105,0106,0107,
    0110,0111,0350,0351,0352,0353,0354,0355,
    0175,0112,0113,0114,0115,0116,0117,0120,
    0121,0122,0356,0357,0360,0361,0362,0363,
    0134,0237,0123,0124,0125,0126,0127,0130,
    0131,0132,0364,0365,0366,0367,0370,0371,
    0060,0061,0062,0063,0064,0065,0066,0067,
    0070,0071,0372,0373,0374,0375,0376,0377
};

  સ્થિર અક્ષર
get_color_char (પૂર્ણાંક e, પૂર્ણાંક ebcdic)
{
  જો (ebcdic)
    {
      જો ((e >= 75 && e <= 80) || (e >= 90 && e <= 97) ||
	  (e >= 107 && e <= 111) || (e >= 121 && e <= 127) ||
	  (e >= 129 && e <= 137) || (e >= 145 && e <= 154) ||
	  (e >= 162 && e <= 169) || (e >= 192 && e <= 201) ||
	  (e >= 208 && e <= 217) || (e >= 226 && e <= 233) ||
	  (e >= 240 && e <= 249) || (e == 189) || (e == 64) ||
	  (e == 173) || (e == 224) )
	પાછા COLOR_GREEN;

      નહીંતર જો (e == 37 || e == 13 || e == 5)
	પાછા COLOR_YELLOW;
      નહીંતર જો (e == 0)
	પાછા COLOR_WHITE;
      નહીંતર જો (e == 255)
	પાછા COLOR_BLUE;
      નહીંતર
	પાછા COLOR_RED;
    }
  નહીંતર  /* ASCII */
    {
#જો defined(__MVS__) && __CHARSET_LIB == 0
      જો (e >= 64)
	પાછા COLOR_GREEN;
#નહીંતર
      જો (e > 31 && e < 127)
	પાછા COLOR_GREEN;
#endif

      નહીંતર જો (e == 9 || e == 10 || e == 13)
	પાછા COLOR_YELLOW;
      નહીંતર જો (e == 0)
	પાછા COLOR_WHITE;
      નહીંતર જો (e == 255)
	પાછા COLOR_BLUE;
      નહીંતર
	પાછા COLOR_RED;
    }
    પાછા 0;
}

  સ્થિર પૂર્ણાંક
enable_color(શૂન્ય)
{
#ifdef WIN32
  DWORD   mode;
  HANDLE  out;

  જો (!isatty(1))
    પાછા 0;

  out = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleMode(out, &mode);
  mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  પાછા (પૂર્ણાંક)SetConsoleMode(out, mode);
#અથવાજો defined(UNIX)
  અક્ષર *term;

  જો (!isatty(STDOUT_FILENO))
    પાછા 0;

  term = getenv("TERM");
  જો (term == ખાલી || *term == '\0' || !strcmp(term, "dumb"))
    પાછા 0;

  પાછા 1;
#નહીંતર
  પાછા 0;
#endif
}

  પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
{
  FILE *fp, *fpo;
  પૂર્ણાંક c, e, p = 0, relseek = 1, negseek = 0, revert = 0, i, x;
  પૂર્ણાંક cols = 0, colsgiven = 0, nonzero = 0, autoskip = 0, hextype = HEX_NORMAL;
  પૂર્ણાંક capitalize = 0, decimal_offset = 0;
  પૂર્ણાંક ebcdic = 0;
  પૂર્ણાંક termination = 0;
  પૂર્ણાંક octspergrp = -1;	/* number of octets grouped in output */
  પૂર્ણાંક grplen;		/* total chars per octet group excluding colors */
  long length = -1, n = 0, seekoff = 0;
  unsigned long displayoff = 0;
  સ્થિર અક્ષર l[LLEN_NO_COLOR+1];  /* static because it may be too big for stack */
  સ્થિર અક્ષર colors[LLEN_NO_COLOR+1]; /* color array */
  અક્ષર *pp;
  અક્ષર *varname = ખાલી;
  પૂર્ણાંક addrlen = 9;
  પૂર્ણાંક color = 0;
  પૂર્ણાંક color_forced = 0;	/* set when -R always is used */
  અક્ષર *no_color;
  અક્ષર cur_color = 0;

  no_color = getenv("NO_COLOR");
  જો (no_color == ખાલી || no_color[0] == '\0')
    color = enable_color();

#ifdef AMIGA
  /* This program doesn't work when started from the Workbench */
  જો (argc == 0)
    exit(1);
#endif

  pname = argv[0];
  માટે (pp = pname; *pp; )
    જો (*pp++ == PATH_SEP)
      pname = pp;
#ifdef FILE_SEP
  માટે (pp = pname; *pp; pp++)
    જો (*pp == FILE_SEP)
      {
	*pp = '\0';
	તોડો;
      }
#endif

  જ્યારે (argc >= 2)
    {
      pp = argv[1] + (!STRNCMP(argv[1], "--", 2) && argv[1][2]);
	   જો (!STRNCMP(pp, "-a", 2)) autoskip = 1 - autoskip;
      નહીંતર જો (!STRNCMP(pp, "-b", 2)) hextype |= HEX_BITS;
      નહીંતર જો (!STRNCMP(pp, "-e", 2)) hextype |= HEX_LITTLEENDIAN;
      નહીંતર જો (!STRNCMP(pp, "-u", 2)) hexx = hexxa + 16;
      નહીંતર જો (!STRNCMP(pp, "-p", 2)) hextype |= HEX_POSTSCRIPT;
      નહીંતર જો (!STRNCMP(pp, "-i", 2)) hextype |= HEX_CINCLUDE;
      નહીંતર જો (!STRNCMP(pp, "-C", 2)) capitalize = 1;
      નહીંતર જો (!STRNCMP(pp, "-d", 2)) decimal_offset = 1;
      નહીંતર જો (!STRNCMP(pp, "-r", 2)) revert++;
      નહીંતર જો (!STRNCMP(pp, "-E", 2)) ebcdic++;
      નહીંતર જો (!STRNCMP(pp, "-t", 2)) termination++;
      નહીંતર જો (!STRNCMP(pp, "-v", 2))
	{
	  fprintf(stderr, "%s%s\n", version, osver);
	  exit(0);
	}
      નહીંતર જો (!STRNCMP(pp, "-c", 2))
	{
	  જો (pp[2] && !STRNCMP("apitalize", pp + 2, 9))
	    capitalize = 1;
	  નહીંતર જો (pp[2] && STRNCMP("ols", pp + 2, 3))
	    {
	      colsgiven = 1;
	      cols = (પૂર્ણાંક)strtol(pp + 2, ખાલી, 0);
	    }
	  નહીંતર
	    {
	      જો (!argv[2])
		exit_with_usage();
	      colsgiven = 1;
	      cols = (પૂર્ણાંક)strtol(argv[2], ખાલી, 0);
	      argv++;
	      argc--;
	    }
	}
      નહીંતર જો (!STRNCMP(pp, "-g", 2))
	{
	  જો (pp[2] && STRNCMP("roup", pp + 2, 4))
	    octspergrp = (પૂર્ણાંક)strtol(pp + 2, ખાલી, 0);
	  નહીંતર
	    {
	      જો (!argv[2])
		exit_with_usage();
	      octspergrp = (પૂર્ણાંક)strtol(argv[2], ખાલી, 0);
	      argv++;
	      argc--;
	    }
	}
      નહીંતર જો (!STRNCMP(pp, "-o", 2))
	{
	  પૂર્ણાંક reloffset = 0;
	  પૂર્ણાંક negoffset = 0;
	  જો (pp[2] && STRNCMP("ffset", pp + 2, 5))
	    displayoff = strtoul(pp + 2, ખાલી, 0);
	  નહીંતર
	    {
	      જો (!argv[2])
		exit_with_usage();

	      જો (argv[2][0] == '+')
	       reloffset++;
	     જો (argv[2][reloffset] == '-')
	       negoffset++;

	     જો (negoffset)
	       displayoff = ULONG_MAX - strtoul(argv[2] + reloffset+negoffset, ખાલી, 0) + 1;
	     નહીંતર
	       displayoff = strtoul(argv[2] + reloffset+negoffset, ખાલી, 0);

	      argv++;
	      argc--;
	    }
	}
      નહીંતર જો (!STRNCMP(pp, "-s", 2))
	{
	  relseek = 0;
	  negseek = 0;
	  જો (pp[2] && STRNCMP("kip", pp+2, 3) && STRNCMP("eek", pp+2, 3))
	    {
#ifdef TRY_SEEK
	      જો (pp[2] == '+')
		relseek++;
	      જો (pp[2+relseek] == '-')
		negseek++;
#endif
	      seekoff = strtol(pp + 2+relseek+negseek, (અક્ષર **)ખાલી, 0);
	    }
	  નહીંતર
	    {
	      જો (!argv[2])
		exit_with_usage();
#ifdef TRY_SEEK
	      જો (argv[2][0] == '+')
		relseek++;
	      જો (argv[2][relseek] == '-')
		negseek++;
#endif
	      seekoff = strtol(argv[2] + relseek+negseek, (અક્ષર **)ખાલી, 0);
	      argv++;
	      argc--;
	    }
	}
      નહીંતર જો (!STRNCMP(pp, "-l", 2))
	{
	  જો (pp[2] && STRNCMP("en", pp + 2, 2))
	    length = strtol(pp + 2, (અક્ષર **)ખાલી, 0);
	  નહીંતર
	    {
	      જો (!argv[2])
		exit_with_usage();
	      length = strtol(argv[2], (અક્ષર **)ખાલી, 0);
	      argv++;
	      argc--;
	    }
	}
      નહીંતર જો (!STRNCMP(pp, "-n", 2))
	{
	  જો (pp[2] && STRNCMP("ame", pp + 2, 3))
	    varname = pp + 2;
	  નહીંતર
	    {
	      જો (!argv[2])
		exit_with_usage();
	      varname = argv[2];
	      argv++;
	      argc--;
	    }
	}
      નહીંતર જો (!STRNCMP(pp, "-R", 2))
	{
	  અક્ષર *pw = pp + 2;
	  જો (!pw[0])
	    {
	      pw = argv[2];
	      argv++;
	      argc--;
	    }
	  જો (!pw)
	    exit_with_usage();
	  જો (!STRNCMP(pw, "always", 6))
	    {
	      (શૂન્ય)enable_color();
	      color = 1;
	      color_forced = 1;
	    }
	  નહીંતર જો (!STRNCMP(pw, "never", 5))
	    color = 0;
	  નહીંતર જો (!STRNCMP(pw, "auto", 4))
	    color = enable_color();
	  નહીંતર
	    exit_with_usage();
	}
      નહીંતર જો (!strcmp(argv[1], "--"))	/* end of options */
	{
	  argv++;
	  argc--;
	  તોડો;
	}
      નહીંતર જો (pp[0] == '-' && pp[1])	/* unknown option */
	exit_with_usage();
      નહીંતર
	તોડો;				/* not an option */

      argv++;				/* advance to next argument */
      argc--;
    }

  જો (hextype != (HEX_CINCLUDE | HEX_BITS))
    {
	/* Allow at most one bit to be set in hextype */
	જો (hextype & (hextype - 1))
	    error_exit(1, "only one of -b, -e, -u, -p, -i can be used");
    }

  જો (!colsgiven || (!cols && hextype != HEX_POSTSCRIPT))
    પસંદગી (hextype)
      {
      સ્થિતિ HEX_POSTSCRIPT:	cols = 30; તોડો;
      સ્થિતિ HEX_CINCLUDE:	cols = 12; તોડો;
      સ્થિતિ HEX_CINCLUDE | HEX_BITS:
      સ્થિતિ HEX_BITS:		cols = 6; તોડો;
      સ્થિતિ HEX_NORMAL:
      સ્થિતિ HEX_LITTLEENDIAN:
      મૂળભૂત:			cols = 16; તોડો;
      }

  જો (octspergrp < 0)
    પસંદગી (hextype)
      {
      સ્થિતિ HEX_CINCLUDE | HEX_BITS:
      સ્થિતિ HEX_BITS:		octspergrp = 1; તોડો;
      સ્થિતિ HEX_NORMAL:		octspergrp = 2; તોડો;
      સ્થિતિ HEX_LITTLEENDIAN:	octspergrp = 4; તોડો;
      સ્થિતિ HEX_POSTSCRIPT:
      સ્થિતિ HEX_CINCLUDE:
      મૂળભૂત:			octspergrp = 0; તોડો;
      }

  જો ((hextype == HEX_POSTSCRIPT && cols < 0) ||
      (hextype != HEX_POSTSCRIPT && cols < 1) ||
      ((hextype == HEX_NORMAL || hextype == HEX_BITS || hextype == HEX_LITTLEENDIAN)
							    && (cols > COLS)))
    {
      fprintf(stderr, "%s: invalid number of columns (max. %d).\n", pname, COLS);
      exit(1);
    }

  જો (octspergrp < 1 || octspergrp > cols)
    octspergrp = cols;
  નહીંતર જો (hextype == HEX_LITTLEENDIAN && (octspergrp & (octspergrp-1)))
    error_exit(1, "number of octets per group must be a power of 2 with -e.");

  જો (argc > 3)
    exit_with_usage();

  જો (argc == 1 || (argv[1][0] == '-' && !argv[1][1]))
    BIN_ASSIGN(fp = stdin, !revert);
  નહીંતર
    {
      જો ((fp = fopen(argv[1], BIN_READ(!revert))) == ખાલી)
	{
	  fprintf(stderr,"%s: ", pname);
	  perror(argv[1]);
	  પાછા 2;
	}
    }

  જો (argc < 3 || (argv[2][0] == '-' && !argv[2][1]))
    BIN_ASSIGN(fpo = stdout, revert);
  નહીંતર
    {
      પૂર્ણાંક fd;
      પૂર્ણાંક mode = revert ? O_WRONLY : (O_TRUNC|O_WRONLY);

      જો (((fd = OPEN(argv[2], mode | BIN_CREAT(revert), 0666)) < 0) ||
	  (fpo = fdopen(fd, BIN_WRITE(revert))) == ખાલી)
	{
	  fprintf(stderr, "%s: ", pname);
	  perror(argv[2]);
	  પાછા 3;
	}
      rewind(fpo);

      /* Disable auto color when writing to a file. */
      જો (!color_forced)
	color = 0;
    }

  જો (revert)
    પસંદગી (hextype)
      {
      સ્થિતિ HEX_NORMAL:
      સ્થિતિ HEX_POSTSCRIPT:
      સ્થિતિ HEX_BITS:
	પાછા huntype(fp, fpo, cols, hextype,
	  negseek ? -seekoff : seekoff);
	તોડો;
      મૂળભૂત:
	error_exit(-1, "Sorry, cannot revert this type of hexdump");
      }

  જો (seekoff || negseek || !relseek)
    {
#ifdef TRY_SEEK
      જો (relseek)
	e = fseek(fp, negseek ? -seekoff : seekoff, SEEK_CUR);
      નહીંતર
	e = fseek(fp, negseek ? -seekoff : seekoff,
						negseek ? SEEK_END : SEEK_SET);
      જો (e < 0 && negseek)
	error_exit(4, "Sorry, cannot seek.");
      જો (e >= 0)
	seekoff = ftell(fp);
      નહીંતર
#endif
	{
	  long s = seekoff;

	  જ્યારે (s--)
	    જો (getc_or_die(fp) == EOF)
	    {
	      error_exit(4, "Sorry, cannot seek.");
	    }
	}
    }

  જો (hextype & HEX_CINCLUDE)
    {
      /* A user-set variable name overrides fp == stdin */
      જો (varname == ખાલી && fp != stdin)
	varname = argv[1];

      જો (varname != ખાલી)
	{
	  FPRINTF_OR_DIE((fpo, "unsigned char %s", isdigit((unsigned અક્ષર)varname[0]) ? "__" : ""));
	  માટે (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(isalnum((unsigned અક્ષર)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  fputs_or_die("[] = {\n", fpo);
	}

      p = 0;
      જ્યારે ((length < 0 || p < length) && (((c = getc_or_die(fp)) != EOF) || termination))
	{
	  જો (c == EOF)
	    {
	      c = 0;
	      termination = -1;
	    }
	  જો (hextype & HEX_BITS)
	    {
	      જો (p == 0)
		fputs_or_die("  ", fpo);
	      નહીંતર જો (p % cols == 0)
		fputs_or_die(",\n  ", fpo);
	      નહીંતર
		fputs_or_die(", ", fpo);

	      FPRINTF_OR_DIE((fpo, "0b"));
	      માટે (પૂર્ણાંક j = 7; j >= 0; j--)
		putc_or_die((c & (1 << j)) ? '1' : '0', fpo);
	      p++;
	    }
	  નહીંતર
	    {
	      FPRINTF_OR_DIE((fpo, (hexx == hexxa) ? "%s0x%02x" : "%s0X%02X",
		(p % cols) ? ", " : (!p ? "  " : ",\n  "), c));
	      p++;
	    }
	  જો (termination == -1)
	    {
	      --p;
	      તોડો ;
	    }
	}

      જો (p)
	fputs_or_die("\n", fpo);

      જો (varname != ખાલી)
	{
	  fputs_or_die("};\n", fpo);
	  FPRINTF_OR_DIE((fpo, "unsigned int %s", isdigit((unsigned અક્ષર)varname[0]) ? "__" : ""));
	  માટે (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(isalnum((unsigned અક્ષર)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  FPRINTF_OR_DIE((fpo, "_%s = %d;\n", capitalize ? "LEN" : "len", p));
	}

      fclose_or_die(fp, fpo);
      પાછા 0;
    }

  જો (hextype == HEX_POSTSCRIPT)
    {
      p = cols;
      જ્યારે ((length < 0 || n < length) && (e = getc_or_die(fp)) != EOF)
	{
	  putc_or_die(hexx[(e >> 4) & 0xf], fpo);
	  putc_or_die(hexx[e & 0xf], fpo);
	  n++;
	  જો (cols > 0 && !--p)
	    {
	      putc_or_die('\n', fpo);
	      p = cols;
	    }
	}
      જો (cols == 0 || p < cols)
	putc_or_die('\n', fpo);
      fclose_or_die(fp, fpo);
      પાછા 0;
    }

  /* hextype: HEX_NORMAL or HEX_BITS or HEX_LITTLEENDIAN */

  જો (hextype != HEX_BITS)
    {
      grplen = octspergrp + octspergrp + 1;	/* chars per octet group */
    }
  નહીંતર	/* hextype == HEX_BITS */
    grplen = 8 * octspergrp + 1;

  જ્યારે ((length < 0 || n < length) && (e = getc_or_die(fp)) != EOF)
    {
      જો (p == 0)
	{
	  addrlen = sprintf(l, decimal_offset ? "%08lu:" : "%08lx:",
				  ((unsigned long)(n + seekoff + displayoff)));
	  માટે (c = addrlen; c < LLEN_NO_COLOR; l[c++] = ' ')
	    ;
	}
      x = hextype == HEX_LITTLEENDIAN ? p ^ (octspergrp-1) : p;
      c = addrlen + 1 + (grplen * x) / octspergrp;
      જો (hextype == HEX_NORMAL || hextype == HEX_LITTLEENDIAN)
	{
	  જો (color)
	    {
	      cur_color = get_color_char(e, ebcdic);
	      colors[c] = cur_color;
	      colors[c+1] = cur_color;
	    }

	  l[c]   = hexx[(e >> 4) & 0xf];
	  l[++c] = hexx[e & 0xf];
	}
      નહીંતર /* hextype == HEX_BITS */
	{
	  જો (color)
	    cur_color = get_color_char(e, ebcdic);

	  માટે (i = 7; i >= 0; i--)
	    {
	      જો (color)
		colors[c] = cur_color;
	      l[c++] = (e & (1 << i)) ? '1' : '0';
	    }
	}
      જો (e)
	nonzero++;
      /* When changing this update definition of LLEN and LLEN_NO_COLOR above. */
      જો (hextype == HEX_LITTLEENDIAN)
	/* last group will be fully used, round up */
	c = grplen * ((cols + octspergrp - 1) / octspergrp);
      નહીંતર
	c = (grplen * cols - 1) / octspergrp;


      જો (ebcdic)
	e = (e < 64) ? '.' : etoa64[e-64];

      જો (hextype == HEX_LITTLEENDIAN)
	c -= 1;

      c += addrlen + 3 + p;
      જો (color)
	  colors[c] = cur_color;
      l[c++] =
#જો defined(__MVS__) && __CHARSET_LIB == 0
	  (e >= 64)
#નહીંતર
	  (e > 31 && e < 127)
#endif
	  ? e : '.';
      n++;
      જો (++p == cols)
	{
	  l[c++] = '\n';
	  l[c] = '\0';

	  xxdline(fpo, l, color ? colors : ખાલી, autoskip ? nonzero : 1);
	  memset(colors, 0, c);
	  nonzero = 0;
	  p = 0;
	}
    }
  જો (p)
    {
      l[c++] = '\n';
      l[c] = '\0';
      જો (color)
	{
	  x = p;
	  જો (hextype == HEX_LITTLEENDIAN)
	    {
	      પૂર્ણાંક fill = octspergrp - (p % octspergrp);
	      જો (fill == octspergrp) fill = 0;

	      c = addrlen + 1 + (grplen * (x - (octspergrp-fill))) / octspergrp;

	      માટે (i = 0; i < fill;i++)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		  x++;
		  p++;
		}
	    }

	  જો (hextype != HEX_BITS)
	    {
	      c = addrlen + 1 + (grplen * x) / octspergrp;
	      c += cols - p;
	      c += (cols - p) / octspergrp;

	      માટે (i = cols - p; i > 0;i--)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		}
	    }
	  xxdline(fpo, l, colors, 1);
	}
      નહીંતર
	xxdline(fpo, l, ખાલી, 1);
    }
  નહીંતર જો (autoskip)
    xxdline(fpo, l, color ? colors : ખાલી, -1);	/* last chance to flush out suppressed lines */

  fclose_or_die(fp, fpo);
  પાછા 0;
}

/* vi:set ts=8 sw=4 sts=2 cino+={2 cino+=n-2 : */
