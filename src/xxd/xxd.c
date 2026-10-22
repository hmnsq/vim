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
#എങ്കിൽ _MSC_VER >= 1400
# define _CRT_SECURE_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#endif
#എങ്കിൽ !defined(CYGWIN) && defined(__CYGWIN__)
# define CYGWIN
#endif

#എങ്കിൽ (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__)
# define _XOPEN_SOURCE 700   /* for fdopen() */
#endif

#അന്തര്ഭാവ <stdio.h>
#ifdef VAXC
# include <file.h>
#അല്ലെങ്കിൽ
# include <fcntl.h>
#endif
#എങ്കിൽ defined(WIN32) || defined(CYGWIN)
# include <io.h>	/* for setmode() */
#endif
#ifdef WIN32
# include <windows.h>
#endif
#ifdef UNIX
# include <unistd.h>
#endif
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <ctype.h>
#അന്തര്ഭാവ <limits.h>
#എങ്കിൽ __MWERKS__ && !defined(BEBOX)
# include <unix.h>	/* for fdopen() on MAC */
#endif


/*  This corrects the problem of missing prototypes for certain functions
 *  in some GNU installations (e.g. SunOS 4.1.x).
 *  Darren Hiebert <darren@hmi.com> (sparc-sun-sunos4.1.3_U1/2.7.2.2)
 */
#എങ്കിൽ defined(__GNUC__) && defined(__STDC__)
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
# എങ്കിൽ defined(sun) && defined(FILE) && !defined(__SVR4) && defined(__STDC__)
#  define __P(a) a
/* excerpt from my sun_stdlib.h */
ബാഹ്യ പൂർണ്ണസംഖ്യ fprintf __P((FILE *, അക്ഷരം *, ...));
ബാഹ്യ പൂർണ്ണസംഖ്യ fputs   __P((അക്ഷരം *, FILE *));
ബാഹ്യ പൂർണ്ണസംഖ്യ _flsbuf __P((unsigned അക്ഷരം, FILE *));
ബാഹ്യ പൂർണ്ണസംഖ്യ _filbuf __P((FILE *));
ബാഹ്യ പൂർണ്ണസംഖ്യ fflush  __P((FILE *));
ബാഹ്യ പൂർണ്ണസംഖ്യ fclose  __P((FILE *));
ബാഹ്യ പൂർണ്ണസംഖ്യ fseek   __P((FILE *, long, പൂർണ്ണസംഖ്യ));
ബാഹ്യ പൂർണ്ണസംഖ്യ rewind  __P((FILE *));

ബാഹ്യ ശൂന്യം perror __P((അക്ഷരം *));
# endif
#endif

അക്ഷരം version[] = "xxd 2026-06-16 by Juergen Weigert et al.";
#ifdef WIN32
അക്ഷരം osver[] = " (Win32)";
#അല്ലെങ്കിൽ
അക്ഷരം osver[] = "";
#endif

#എങ്കിൽ defined(WIN32)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "wt")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_ദ്വിമാന) : O_CREAT)
# define BIN_ASSIGN(fp, yes) setmode(fileno(fp), (yes) ? O_ദ്വിമാന : O_TEXT)
# define PATH_SEP '\\'
#അല്ലെങ്കിലെങ്കിൽ defined(CYGWIN)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "w")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_ദ്വിമാന) : O_CREAT)
# define BIN_ASSIGN(fp, yes) ((yes) ? (ശൂന്യം) setmode(fileno(fp), O_ദ്വിമാന) : (ശൂന്യം) (fp))
# define PATH_SEP '/'
#അല്ലെങ്കിൽ
# ifdef VMS
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ASSIGN(fp, dummy) fp
#  define PATH_SEP ']'
#  define FILE_SEP '.'
# അല്ലെങ്കിൽ
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ASSIGN(fp, dummy) fp
#  define PATH_SEP '/'
# endif
#endif

/* open has only to arguments on the Mac */
#എങ്കിൽ __MWERKS__
# define OPEN(name, mode, umask) open(name, mode)
#അല്ലെങ്കിൽ
# define OPEN(name, mode, umask) open(name, mode, umask)
#endif

#ifdef AMIGA
# define STRNCMP(s1, s2, l) strncmp(s1, s2, (size_t)l)
#അല്ലെങ്കിൽ
# define STRNCMP(s1, s2, l) strncmp(s1, s2, l)
#endif

#ifndef __P
# എങ്കിൽ defined(__STDC__) || defined(WIN32)
#  define __P(a) a
# അല്ലെങ്കിൽ
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

അക്ഷരം hexxa[] = "0123456789abcdef0123456789ABCDEF", *hexx = hexxa;

/* the different hextypes known by this program: */
#define HEX_NORMAL         0x00 /* no flags set */
#define HEX_POSTSCRIPT     0x01
#define HEX_CINCLUDE       0x02
#define HEX_BITS           0x04 /* not hex a dump, but bits: 01111001 */
#define HEX_LITTLEENDIAN   0x08

#define CONDITIONAL_CAPITALIZE(c) (capitalize ? toupper((unsigned അക്ഷരം)(c)) : (c))

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

സ്ഥിതം അക്ഷരം *pname;

  സ്ഥിതം ശൂന്യം
exit_with_usage(ശൂന്യം)
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
#അല്ലെങ്കിൽ
	  "", "");
#endif
  fprintf(stderr, "    -u          use upper case hex letters.\n"
		  "    -R when     colorize the output; <when> can be 'always', 'auto' or 'never'. Default: 'auto'.\n"
		  "    -v          show version: \"%s%s\".\n", version, osver);
  exit(1);
}

  സ്ഥിതം ശൂന്യം
perror_exit(പൂർണ്ണസംഖ്യ ret)
{
  fprintf(stderr, "%s: ", pname);
  perror(ശൂന്യ);
  exit(ret);
}

  സ്ഥിതം ശൂന്യം
error_exit(പൂർണ്ണസംഖ്യ ret, അക്ഷരം *സന്ദേശം)
{
  fprintf(stderr, "%s: %s\n", pname, സന്ദേശം);
  exit(ret);
}

  സ്ഥിതം പൂർണ്ണസംഖ്യ
getc_or_die(FILE *fpi)
{
  പൂർണ്ണസംഖ്യ c = getc(fpi);
  എങ്കിൽ (c == EOF && ferror(fpi))
    perror_exit(2);
  തിരികെ c;
}

  സ്ഥിതം ശൂന്യം
putc_or_die(പൂർണ്ണസംഖ്യ c, FILE *fpo)
{
  എങ്കിൽ (putc(c, fpo) == EOF)
    perror_exit(3);
}

  സ്ഥിതം ശൂന്യം
fputs_or_die(അക്ഷരം *s, FILE *fpo)
{
  എങ്കിൽ (fputs(s, fpo) == EOF)
    perror_exit(3);
}

/* Use a macro to allow for different arguments. */
#define FPRINTF_OR_DIE(args) എങ്കിൽ (fprintf args < 0) perror_exit(3)

  സ്ഥിതം ശൂന്യം
fclose_or_die(FILE *fpi, FILE *fpo)
{
  എങ്കിൽ (fclose(fpo) != 0)
    perror_exit(3);
  എങ്കിൽ (fclose(fpi) != 0)
    perror_exit(2);
}

/*
 * If "c" is a hex digit, return the value.
 * Otherwise return -1.
 */
  സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_hex_digit(പൂർണ്ണസംഖ്യ c)
{
  തിരികെ (c >= '0' && c <= '9') ? c - '0'
	: (c >= 'a' && c <= 'f') ? c - 'a' + 10
	: (c >= 'A' && c <= 'F') ? c - 'A' + 10
	: -1;
}

/*
 * If "c" is a bin digit, return the value.
 * Otherwise return -1.
 */
  സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_bin_digit(പൂർണ്ണസംഖ്യ c)
{
  തിരികെ (c >= '0' && c <= '1') ? c - '0'
	: -1;
}

/*
 * Ignore text on "fpi" until end-of-line or end-of-file.
 * Return the '\n' or EOF character.
 * When an error is encountered exit with an error message.
 */
  സ്ഥിതം പൂർണ്ണസംഖ്യ
skip_to_eol(FILE *fpi, പൂർണ്ണസംഖ്യ c)
{
  ആകുമ്പോൾ (c != '\n' && c != EOF)
    c = getc_or_die(fpi);
  തിരികെ c;
}

/*
 * Max. cols binary characters are decoded from the input stream per line.
 * Two adjacent garbage characters after evaluated data delimit valid data.
 * Everything up to the next newline is discarded.
 *
 * The name is historic and came from 'undo type opt h'.
 */
  സ്ഥിതം പൂർണ്ണസംഖ്യ
huntype(
  FILE *fpi,
  FILE *fpo,
  പൂർണ്ണസംഖ്യ cols,
  പൂർണ്ണസംഖ്യ hextype,
  long base_off)
{
  പൂർണ്ണസംഖ്യ c, ign_garb = 1, n1 = -1, n2 = 0, n3 = 0, p = cols, bt = 0, b = 0, bcnt = 0;
  long have_off = 0, want_off = 0;

  rewind(fpi);

  ആകുമ്പോൾ ((c = getc(fpi)) != EOF)
    {
      എങ്കിൽ (c == '\r')	/* Doze style input file? */
	തുടരുക;

      /* Allow multiple spaces.  This doesn't work when there is normal text
       * after the hex codes in the last line that looks like hex, thus only
       * use it for PostScript format. */
      എങ്കിൽ (hextype == HEX_POSTSCRIPT && (c == ' ' || c == '\n' || c == '\t'))
	തുടരുക;

      എങ്കിൽ (hextype == HEX_NORMAL || hextype == HEX_POSTSCRIPT)
	{
	  n3 = n2;
	  n2 = n1;

	  n1 = parse_hex_digit(c);
	  എങ്കിൽ (n1 == -1 && ign_garb)
	    തുടരുക;
	}
      അല്ലെങ്കിൽ /* HEX_BITS */
	{
	  n1 = parse_hex_digit(c);
	  എങ്കിൽ (n1 == -1 && ign_garb)
	    തുടരുക;

	  bt = parse_bin_digit(c);
	  എങ്കിൽ (bt != -1)
	    {
	      /* shift via unsigned to avoid signed overflow on bad input */
	      b = (പൂർണ്ണസംഖ്യ)(((unsigned)b << 1) | (unsigned)bt);
	      ++bcnt;
	    }
	}

      ign_garb = 0;

      എങ്കിൽ ((hextype != HEX_POSTSCRIPT) && (p >= cols))
	{
	  എങ്കിൽ (hextype == HEX_NORMAL)
	    {
	      എങ്കിൽ (n1 < 0)
		{
		  p = 0;
		  തുടരുക;
		}
	      want_off = (long)(((unsigned long)want_off << 4) | (unsigned)n1);
	    }
	  അല്ലെങ്കിൽ /* HEX_BITS */
	    {
	      എങ്കിൽ (n1 < 0)
		{
		  p = 0;
		  bcnt = 0;
		  തുടരുക;
		}
	      want_off = (long)(((unsigned long)want_off << 4) | (unsigned)n1);
	    }
	  തുടരുക;
	}

      എങ്കിൽ (base_off + want_off != have_off)
	{
	  എങ്കിൽ (fflush(fpo) != 0)
	    perror_exit(3);
#ifdef TRY_SEEK
	  എങ്കിൽ (fseek(fpo, base_off + want_off - have_off, SEEK_CUR) >= 0)
	    have_off = base_off + want_off;
#endif
	  എങ്കിൽ (base_off + want_off < have_off)
	    error_exit(5, "Sorry, cannot seek backwards.");
	  വേണ്ടി (; have_off < base_off + want_off; have_off++)
	    putc_or_die(0, fpo);
	}

      എങ്കിൽ (hextype == HEX_NORMAL || hextype == HEX_POSTSCRIPT)
	{
	  എങ്കിൽ (n2 >= 0 && n1 >= 0)
	    {
	      putc_or_die((n2 << 4) | n1, fpo);
	      have_off++;
	      want_off++;
	      n1 = -1;
	      എങ്കിൽ (!hextype && (++p >= cols))
	      /* skip the rest of the line as garbage */
	      c = skip_to_eol(fpi, c);
	    }
	  അല്ലെങ്കിൽ എങ്കിൽ (n1 < 0 && n2 < 0 && n3 < 0)
	    /* already stumbled into garbage, skip line, wait and see */
	    c = skip_to_eol(fpi, c);
	}
      അല്ലെങ്കിൽ /* HEX_BITS */
	{
	  എങ്കിൽ (bcnt == 8)
	    {
	      putc_or_die(b, fpo);
	      have_off++;
	      want_off++;
	      b = 0;
	      bcnt = 0;
	      എങ്കിൽ (++p >= cols)
		/* skip the rest of the line as garbage */
		 c = skip_to_eol(fpi, c);
	    }
	}

      എങ്കിൽ (c == '\n')
	{
	  എങ്കിൽ (hextype == HEX_NORMAL || hextype == HEX_BITS)
	    want_off = 0;
	  p = cols;
	  ign_garb = 1;
	}
    }
  എങ്കിൽ (fflush(fpo) != 0)
    perror_exit(3);
#ifdef TRY_SEEK
  fseek(fpo, 0L, SEEK_END);
#endif
  fclose_or_die(fpi, fpo);
  തിരികെ 0;
}


/*
 * Print line l with given colors.
 */
  സ്ഥിതം ശൂന്യം
print_colored_വരി(FILE *fp, അക്ഷരം *l, അക്ഷരം *colors)
{
  സ്ഥിതം അക്ഷരം l_colored[LLEN+1];

  എങ്കിൽ (colors)
    {
      പൂർണ്ണസംഖ്യ c = 0;
      എങ്കിൽ (colors[0])
	{
	  COLOR_PROLOGUE(colors[0])
	}
      l_colored[c++] = l[0];
      പൂർണ്ണസംഖ്യ i;
      വേണ്ടി (i = 1; l[i]; i++)
	{
	  എങ്കിൽ (colors[i] != colors[i-1])
	    {
	      എങ്കിൽ (colors[i-1])
		{
		  COLOR_EPILOGUE
		}
	      എങ്കിൽ (colors[i])
		{
		  COLOR_PROLOGUE(colors[i])
		}
	    }
	  l_colored[c++] = l[i];
	}

      എങ്കിൽ (colors[i])
	{
	  COLOR_EPILOGUE
	}
      l_colored[c++] = '\0';

      fputs_or_die(l_colored, fp);
    }
  അല്ലെങ്കിൽ
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
  സ്ഥിതം ശൂന്യം
xxdline(FILE *fp, അക്ഷരം *l, അക്ഷരം *colors, പൂർണ്ണസംഖ്യ nz)
{
  സ്ഥിതം അക്ഷരം z[LLEN_NO_COLOR+1];
  സ്ഥിതം അക്ഷരം z_colors[LLEN_NO_COLOR+1];
  സ്ഥിതം signed അക്ഷരം zero_seen = 0;

  എങ്കിൽ (!nz && zero_seen == 1)
    {
      strcpy(z, l);
      എങ്കിൽ (colors)
	  memcpy(z_colors, colors, strlen(z));
    }

  എങ്കിൽ (nz || !zero_seen++)
    {
      എങ്കിൽ (nz)
	{
	  എങ്കിൽ (nz < 0)
	    zero_seen--;
	  എങ്കിൽ (zero_seen == 2)
	    print_colored_വരി(fp, z, z_colors);
	  എങ്കിൽ (zero_seen > 2)
	    fputs_or_die("*\n", fp);
	}
      എങ്കിൽ (nz >= 0 || zero_seen > 0)
	print_colored_വരി(fp, l, colors);

      എങ്കിൽ (nz)
	zero_seen = 0;
    }

  /* If zero_seen > 3, then its exact value doesn't matter, so long as it
   * remains >3 and incrementing it will not cause overflow. */
  എങ്കിൽ (zero_seen >= 0x7F)
    zero_seen = 4;
}

/* This is an EBCDIC to ASCII conversion table */
/* from a proposed BTL standard April 16, 1979 */
സ്ഥിതം unsigned അക്ഷരം etoa64[] =
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

  സ്ഥിതം അക്ഷരം
get_color_char (പൂർണ്ണസംഖ്യ e, പൂർണ്ണസംഖ്യ ebcdic)
{
  എങ്കിൽ (ebcdic)
    {
      എങ്കിൽ ((e >= 75 && e <= 80) || (e >= 90 && e <= 97) ||
	  (e >= 107 && e <= 111) || (e >= 121 && e <= 127) ||
	  (e >= 129 && e <= 137) || (e >= 145 && e <= 154) ||
	  (e >= 162 && e <= 169) || (e >= 192 && e <= 201) ||
	  (e >= 208 && e <= 217) || (e >= 226 && e <= 233) ||
	  (e >= 240 && e <= 249) || (e == 189) || (e == 64) ||
	  (e == 173) || (e == 224) )
	തിരികെ COLOR_GREEN;

      അല്ലെങ്കിൽ എങ്കിൽ (e == 37 || e == 13 || e == 5)
	തിരികെ COLOR_YELLOW;
      അല്ലെങ്കിൽ എങ്കിൽ (e == 0)
	തിരികെ COLOR_WHITE;
      അല്ലെങ്കിൽ എങ്കിൽ (e == 255)
	തിരികെ COLOR_BLUE;
      അല്ലെങ്കിൽ
	തിരികെ COLOR_RED;
    }
  അല്ലെങ്കിൽ  /* ASCII */
    {
#എങ്കിൽ defined(__MVS__) && __CHARSET_LIB == 0
      എങ്കിൽ (e >= 64)
	തിരികെ COLOR_GREEN;
#അല്ലെങ്കിൽ
      എങ്കിൽ (e > 31 && e < 127)
	തിരികെ COLOR_GREEN;
#endif

      അല്ലെങ്കിൽ എങ്കിൽ (e == 9 || e == 10 || e == 13)
	തിരികെ COLOR_YELLOW;
      അല്ലെങ്കിൽ എങ്കിൽ (e == 0)
	തിരികെ COLOR_WHITE;
      അല്ലെങ്കിൽ എങ്കിൽ (e == 255)
	തിരികെ COLOR_BLUE;
      അല്ലെങ്കിൽ
	തിരികെ COLOR_RED;
    }
    തിരികെ 0;
}

  സ്ഥിതം പൂർണ്ണസംഖ്യ
enable_color(ശൂന്യം)
{
#ifdef WIN32
  DWORD   mode;
  HANDLE  out;

  എങ്കിൽ (!isatty(1))
    തിരികെ 0;

  out = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleMode(out, &mode);
  mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  തിരികെ (പൂർണ്ണസംഖ്യ)SetConsoleMode(out, mode);
#അല്ലെങ്കിലെങ്കിൽ defined(UNIX)
  അക്ഷരം *term;

  എങ്കിൽ (!isatty(STDOUT_FILENO))
    തിരികെ 0;

  term = getenv("TERM");
  എങ്കിൽ (term == ശൂന്യ || *term == '\0' || !strcmp(term, "dumb"))
    തിരികെ 0;

  തിരികെ 1;
#അല്ലെങ്കിൽ
  തിരികെ 0;
#endif
}

  പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
{
  FILE *fp, *fpo;
  പൂർണ്ണസംഖ്യ c, e, p = 0, relseek = 1, negseek = 0, revert = 0, i, x;
  പൂർണ്ണസംഖ്യ cols = 0, colsgiven = 0, nonzero = 0, autoskip = 0, hextype = HEX_NORMAL;
  പൂർണ്ണസംഖ്യ capitalize = 0, decimal_offset = 0;
  പൂർണ്ണസംഖ്യ ebcdic = 0;
  പൂർണ്ണസംഖ്യ termination = 0;
  പൂർണ്ണസംഖ്യ octspergrp = -1;	/* number of octets grouped in output */
  പൂർണ്ണസംഖ്യ grplen;		/* total chars per octet group excluding colors */
  long length = -1, n = 0, seekoff = 0;
  unsigned long displayoff = 0;
  സ്ഥിതം അക്ഷരം l[LLEN_NO_COLOR+1];  /* static because it may be too big for stack */
  സ്ഥിതം അക്ഷരം colors[LLEN_NO_COLOR+1]; /* color array */
  അക്ഷരം *pp;
  അക്ഷരം *varname = ശൂന്യ;
  പൂർണ്ണസംഖ്യ addrlen = 9;
  പൂർണ്ണസംഖ്യ color = 0;
  പൂർണ്ണസംഖ്യ color_forced = 0;	/* set when -R always is used */
  അക്ഷരം *no_color;
  അക്ഷരം cur_color = 0;

  no_color = getenv("NO_COLOR");
  എങ്കിൽ (no_color == ശൂന്യ || no_color[0] == '\0')
    color = enable_color();

#ifdef AMIGA
  /* This program doesn't work when started from the Workbench */
  എങ്കിൽ (argc == 0)
    exit(1);
#endif

  pname = argv[0];
  വേണ്ടി (pp = pname; *pp; )
    എങ്കിൽ (*pp++ == PATH_SEP)
      pname = pp;
#ifdef FILE_SEP
  വേണ്ടി (pp = pname; *pp; pp++)
    എങ്കിൽ (*pp == FILE_SEP)
      {
	*pp = '\0';
	നിർത്തുക;
      }
#endif

  ആകുമ്പോൾ (argc >= 2)
    {
      pp = argv[1] + (!STRNCMP(argv[1], "--", 2) && argv[1][2]);
	   എങ്കിൽ (!STRNCMP(pp, "-a", 2)) autoskip = 1 - autoskip;
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-b", 2)) hextype |= HEX_BITS;
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-e", 2)) hextype |= HEX_LITTLEENDIAN;
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-u", 2)) hexx = hexxa + 16;
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-p", 2)) hextype |= HEX_POSTSCRIPT;
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-i", 2)) hextype |= HEX_CINCLUDE;
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-C", 2)) capitalize = 1;
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-d", 2)) decimal_offset = 1;
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-r", 2)) revert++;
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-E", 2)) ebcdic++;
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-t", 2)) termination++;
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-v", 2))
	{
	  fprintf(stderr, "%s%s\n", version, osver);
	  exit(0);
	}
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-c", 2))
	{
	  എങ്കിൽ (pp[2] && !STRNCMP("apitalize", pp + 2, 9))
	    capitalize = 1;
	  അല്ലെങ്കിൽ എങ്കിൽ (pp[2] && STRNCMP("ols", pp + 2, 3))
	    {
	      colsgiven = 1;
	      cols = (പൂർണ്ണസംഖ്യ)strtol(pp + 2, ശൂന്യ, 0);
	    }
	  അല്ലെങ്കിൽ
	    {
	      എങ്കിൽ (!argv[2])
		exit_with_usage();
	      colsgiven = 1;
	      cols = (പൂർണ്ണസംഖ്യ)strtol(argv[2], ശൂന്യ, 0);
	      argv++;
	      argc--;
	    }
	}
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-g", 2))
	{
	  എങ്കിൽ (pp[2] && STRNCMP("roup", pp + 2, 4))
	    octspergrp = (പൂർണ്ണസംഖ്യ)strtol(pp + 2, ശൂന്യ, 0);
	  അല്ലെങ്കിൽ
	    {
	      എങ്കിൽ (!argv[2])
		exit_with_usage();
	      octspergrp = (പൂർണ്ണസംഖ്യ)strtol(argv[2], ശൂന്യ, 0);
	      argv++;
	      argc--;
	    }
	}
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-o", 2))
	{
	  പൂർണ്ണസംഖ്യ reloffset = 0;
	  പൂർണ്ണസംഖ്യ negoffset = 0;
	  എങ്കിൽ (pp[2] && STRNCMP("ffset", pp + 2, 5))
	    displayoff = strtoul(pp + 2, ശൂന്യ, 0);
	  അല്ലെങ്കിൽ
	    {
	      എങ്കിൽ (!argv[2])
		exit_with_usage();

	      എങ്കിൽ (argv[2][0] == '+')
	       reloffset++;
	     എങ്കിൽ (argv[2][reloffset] == '-')
	       negoffset++;

	     എങ്കിൽ (negoffset)
	       displayoff = ULONG_MAX - strtoul(argv[2] + reloffset+negoffset, ശൂന്യ, 0) + 1;
	     അല്ലെങ്കിൽ
	       displayoff = strtoul(argv[2] + reloffset+negoffset, ശൂന്യ, 0);

	      argv++;
	      argc--;
	    }
	}
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-s", 2))
	{
	  relseek = 0;
	  negseek = 0;
	  എങ്കിൽ (pp[2] && STRNCMP("kip", pp+2, 3) && STRNCMP("eek", pp+2, 3))
	    {
#ifdef TRY_SEEK
	      എങ്കിൽ (pp[2] == '+')
		relseek++;
	      എങ്കിൽ (pp[2+relseek] == '-')
		negseek++;
#endif
	      seekoff = strtol(pp + 2+relseek+negseek, (അക്ഷരം **)ശൂന്യ, 0);
	    }
	  അല്ലെങ്കിൽ
	    {
	      എങ്കിൽ (!argv[2])
		exit_with_usage();
#ifdef TRY_SEEK
	      എങ്കിൽ (argv[2][0] == '+')
		relseek++;
	      എങ്കിൽ (argv[2][relseek] == '-')
		negseek++;
#endif
	      seekoff = strtol(argv[2] + relseek+negseek, (അക്ഷരം **)ശൂന്യ, 0);
	      argv++;
	      argc--;
	    }
	}
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-l", 2))
	{
	  എങ്കിൽ (pp[2] && STRNCMP("en", pp + 2, 2))
	    length = strtol(pp + 2, (അക്ഷരം **)ശൂന്യ, 0);
	  അല്ലെങ്കിൽ
	    {
	      എങ്കിൽ (!argv[2])
		exit_with_usage();
	      length = strtol(argv[2], (അക്ഷരം **)ശൂന്യ, 0);
	      argv++;
	      argc--;
	    }
	}
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-n", 2))
	{
	  എങ്കിൽ (pp[2] && STRNCMP("ame", pp + 2, 3))
	    varname = pp + 2;
	  അല്ലെങ്കിൽ
	    {
	      എങ്കിൽ (!argv[2])
		exit_with_usage();
	      varname = argv[2];
	      argv++;
	      argc--;
	    }
	}
      അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pp, "-R", 2))
	{
	  അക്ഷരം *pw = pp + 2;
	  എങ്കിൽ (!pw[0])
	    {
	      pw = argv[2];
	      argv++;
	      argc--;
	    }
	  എങ്കിൽ (!pw)
	    exit_with_usage();
	  എങ്കിൽ (!STRNCMP(pw, "always", 6))
	    {
	      (ശൂന്യം)enable_color();
	      color = 1;
	      color_forced = 1;
	    }
	  അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pw, "never", 5))
	    color = 0;
	  അല്ലെങ്കിൽ എങ്കിൽ (!STRNCMP(pw, "auto", 4))
	    color = enable_color();
	  അല്ലെങ്കിൽ
	    exit_with_usage();
	}
      അല്ലെങ്കിൽ എങ്കിൽ (!strcmp(argv[1], "--"))	/* end of options */
	{
	  argv++;
	  argc--;
	  നിർത്തുക;
	}
      അല്ലെങ്കിൽ എങ്കിൽ (pp[0] == '-' && pp[1])	/* unknown option */
	exit_with_usage();
      അല്ലെങ്കിൽ
	നിർത്തുക;				/* not an option */

      argv++;				/* advance to next argument */
      argc--;
    }

  എങ്കിൽ (hextype != (HEX_CINCLUDE | HEX_BITS))
    {
	/* Allow at most one bit to be set in hextype */
	എങ്കിൽ (hextype & (hextype - 1))
	    error_exit(1, "only one of -b, -e, -u, -p, -i can be used");
    }

  എങ്കിൽ (!colsgiven || (!cols && hextype != HEX_POSTSCRIPT))
    തിരഞ്ഞെടുപ്പ് (hextype)
      {
      അവസ്ഥ HEX_POSTSCRIPT:	cols = 30; നിർത്തുക;
      അവസ്ഥ HEX_CINCLUDE:	cols = 12; നിർത്തുക;
      അവസ്ഥ HEX_CINCLUDE | HEX_BITS:
      അവസ്ഥ HEX_BITS:		cols = 6; നിർത്തുക;
      അവസ്ഥ HEX_NORMAL:
      അവസ്ഥ HEX_LITTLEENDIAN:
      സ്ഥിരസ്ഥിതി:			cols = 16; നിർത്തുക;
      }

  എങ്കിൽ (octspergrp < 0)
    തിരഞ്ഞെടുപ്പ് (hextype)
      {
      അവസ്ഥ HEX_CINCLUDE | HEX_BITS:
      അവസ്ഥ HEX_BITS:		octspergrp = 1; നിർത്തുക;
      അവസ്ഥ HEX_NORMAL:		octspergrp = 2; നിർത്തുക;
      അവസ്ഥ HEX_LITTLEENDIAN:	octspergrp = 4; നിർത്തുക;
      അവസ്ഥ HEX_POSTSCRIPT:
      അവസ്ഥ HEX_CINCLUDE:
      സ്ഥിരസ്ഥിതി:			octspergrp = 0; നിർത്തുക;
      }

  എങ്കിൽ ((hextype == HEX_POSTSCRIPT && cols < 0) ||
      (hextype != HEX_POSTSCRIPT && cols < 1) ||
      ((hextype == HEX_NORMAL || hextype == HEX_BITS || hextype == HEX_LITTLEENDIAN)
							    && (cols > COLS)))
    {
      fprintf(stderr, "%s: invalid number of columns (max. %d).\n", pname, COLS);
      exit(1);
    }

  എങ്കിൽ (octspergrp < 1 || octspergrp > cols)
    octspergrp = cols;
  അല്ലെങ്കിൽ എങ്കിൽ (hextype == HEX_LITTLEENDIAN && (octspergrp & (octspergrp-1)))
    error_exit(1, "number of octets per group must be a power of 2 with -e.");

  എങ്കിൽ (argc > 3)
    exit_with_usage();

  എങ്കിൽ (argc == 1 || (argv[1][0] == '-' && !argv[1][1]))
    BIN_ASSIGN(fp = stdin, !revert);
  അല്ലെങ്കിൽ
    {
      എങ്കിൽ ((fp = fopen(argv[1], BIN_READ(!revert))) == ശൂന്യ)
	{
	  fprintf(stderr,"%s: ", pname);
	  perror(argv[1]);
	  തിരികെ 2;
	}
    }

  എങ്കിൽ (argc < 3 || (argv[2][0] == '-' && !argv[2][1]))
    BIN_ASSIGN(fpo = stdout, revert);
  അല്ലെങ്കിൽ
    {
      പൂർണ്ണസംഖ്യ fd;
      പൂർണ്ണസംഖ്യ mode = revert ? O_WRONLY : (O_TRUNC|O_WRONLY);

      എങ്കിൽ (((fd = OPEN(argv[2], mode | BIN_CREAT(revert), 0666)) < 0) ||
	  (fpo = fdopen(fd, BIN_WRITE(revert))) == ശൂന്യ)
	{
	  fprintf(stderr, "%s: ", pname);
	  perror(argv[2]);
	  തിരികെ 3;
	}
      rewind(fpo);

      /* Disable auto color when writing to a file. */
      എങ്കിൽ (!color_forced)
	color = 0;
    }

  എങ്കിൽ (revert)
    തിരഞ്ഞെടുപ്പ് (hextype)
      {
      അവസ്ഥ HEX_NORMAL:
      അവസ്ഥ HEX_POSTSCRIPT:
      അവസ്ഥ HEX_BITS:
	തിരികെ huntype(fp, fpo, cols, hextype,
	  negseek ? -seekoff : seekoff);
	നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
	error_exit(-1, "Sorry, cannot revert this type of hexdump");
      }

  എങ്കിൽ (seekoff || negseek || !relseek)
    {
#ifdef TRY_SEEK
      എങ്കിൽ (relseek)
	e = fseek(fp, negseek ? -seekoff : seekoff, SEEK_CUR);
      അല്ലെങ്കിൽ
	e = fseek(fp, negseek ? -seekoff : seekoff,
						negseek ? SEEK_END : SEEK_SET);
      എങ്കിൽ (e < 0 && negseek)
	error_exit(4, "Sorry, cannot seek.");
      എങ്കിൽ (e >= 0)
	seekoff = ftell(fp);
      അല്ലെങ്കിൽ
#endif
	{
	  long s = seekoff;

	  ആകുമ്പോൾ (s--)
	    എങ്കിൽ (getc_or_die(fp) == EOF)
	    {
	      error_exit(4, "Sorry, cannot seek.");
	    }
	}
    }

  എങ്കിൽ (hextype & HEX_CINCLUDE)
    {
      /* A user-set variable name overrides fp == stdin */
      എങ്കിൽ (varname == ശൂന്യ && fp != stdin)
	varname = argv[1];

      എങ്കിൽ (varname != ശൂന്യ)
	{
	  FPRINTF_OR_DIE((fpo, "unsigned char %s", isdigit((unsigned അക്ഷരം)varname[0]) ? "__" : ""));
	  വേണ്ടി (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(isalnum((unsigned അക്ഷരം)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  fputs_or_die("[] = {\n", fpo);
	}

      p = 0;
      ആകുമ്പോൾ ((length < 0 || p < length) && (((c = getc_or_die(fp)) != EOF) || termination))
	{
	  എങ്കിൽ (c == EOF)
	    {
	      c = 0;
	      termination = -1;
	    }
	  എങ്കിൽ (hextype & HEX_BITS)
	    {
	      എങ്കിൽ (p == 0)
		fputs_or_die("  ", fpo);
	      അല്ലെങ്കിൽ എങ്കിൽ (p % cols == 0)
		fputs_or_die(",\n  ", fpo);
	      അല്ലെങ്കിൽ
		fputs_or_die(", ", fpo);

	      FPRINTF_OR_DIE((fpo, "0b"));
	      വേണ്ടി (പൂർണ്ണസംഖ്യ j = 7; j >= 0; j--)
		putc_or_die((c & (1 << j)) ? '1' : '0', fpo);
	      p++;
	    }
	  അല്ലെങ്കിൽ
	    {
	      FPRINTF_OR_DIE((fpo, (hexx == hexxa) ? "%s0x%02x" : "%s0X%02X",
		(p % cols) ? ", " : (!p ? "  " : ",\n  "), c));
	      p++;
	    }
	  എങ്കിൽ (termination == -1)
	    {
	      --p;
	      നിർത്തുക ;
	    }
	}

      എങ്കിൽ (p)
	fputs_or_die("\n", fpo);

      എങ്കിൽ (varname != ശൂന്യ)
	{
	  fputs_or_die("};\n", fpo);
	  FPRINTF_OR_DIE((fpo, "unsigned int %s", isdigit((unsigned അക്ഷരം)varname[0]) ? "__" : ""));
	  വേണ്ടി (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(isalnum((unsigned അക്ഷരം)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  FPRINTF_OR_DIE((fpo, "_%s = %d;\n", capitalize ? "LEN" : "len", p));
	}

      fclose_or_die(fp, fpo);
      തിരികെ 0;
    }

  എങ്കിൽ (hextype == HEX_POSTSCRIPT)
    {
      p = cols;
      ആകുമ്പോൾ ((length < 0 || n < length) && (e = getc_or_die(fp)) != EOF)
	{
	  putc_or_die(hexx[(e >> 4) & 0xf], fpo);
	  putc_or_die(hexx[e & 0xf], fpo);
	  n++;
	  എങ്കിൽ (cols > 0 && !--p)
	    {
	      putc_or_die('\n', fpo);
	      p = cols;
	    }
	}
      എങ്കിൽ (cols == 0 || p < cols)
	putc_or_die('\n', fpo);
      fclose_or_die(fp, fpo);
      തിരികെ 0;
    }

  /* hextype: HEX_NORMAL or HEX_BITS or HEX_LITTLEENDIAN */

  എങ്കിൽ (hextype != HEX_BITS)
    {
      grplen = octspergrp + octspergrp + 1;	/* chars per octet group */
    }
  അല്ലെങ്കിൽ	/* hextype == HEX_BITS */
    grplen = 8 * octspergrp + 1;

  ആകുമ്പോൾ ((length < 0 || n < length) && (e = getc_or_die(fp)) != EOF)
    {
      എങ്കിൽ (p == 0)
	{
	  addrlen = sprintf(l, decimal_offset ? "%08lu:" : "%08lx:",
				  ((unsigned long)(n + seekoff + displayoff)));
	  വേണ്ടി (c = addrlen; c < LLEN_NO_COLOR; l[c++] = ' ')
	    ;
	}
      x = hextype == HEX_LITTLEENDIAN ? p ^ (octspergrp-1) : p;
      c = addrlen + 1 + (grplen * x) / octspergrp;
      എങ്കിൽ (hextype == HEX_NORMAL || hextype == HEX_LITTLEENDIAN)
	{
	  എങ്കിൽ (color)
	    {
	      cur_color = get_color_char(e, ebcdic);
	      colors[c] = cur_color;
	      colors[c+1] = cur_color;
	    }

	  l[c]   = hexx[(e >> 4) & 0xf];
	  l[++c] = hexx[e & 0xf];
	}
      അല്ലെങ്കിൽ /* hextype == HEX_BITS */
	{
	  എങ്കിൽ (color)
	    cur_color = get_color_char(e, ebcdic);

	  വേണ്ടി (i = 7; i >= 0; i--)
	    {
	      എങ്കിൽ (color)
		colors[c] = cur_color;
	      l[c++] = (e & (1 << i)) ? '1' : '0';
	    }
	}
      എങ്കിൽ (e)
	nonzero++;
      /* When changing this update definition of LLEN and LLEN_NO_COLOR above. */
      എങ്കിൽ (hextype == HEX_LITTLEENDIAN)
	/* last group will be fully used, round up */
	c = grplen * ((cols + octspergrp - 1) / octspergrp);
      അല്ലെങ്കിൽ
	c = (grplen * cols - 1) / octspergrp;


      എങ്കിൽ (ebcdic)
	e = (e < 64) ? '.' : etoa64[e-64];

      എങ്കിൽ (hextype == HEX_LITTLEENDIAN)
	c -= 1;

      c += addrlen + 3 + p;
      എങ്കിൽ (color)
	  colors[c] = cur_color;
      l[c++] =
#എങ്കിൽ defined(__MVS__) && __CHARSET_LIB == 0
	  (e >= 64)
#അല്ലെങ്കിൽ
	  (e > 31 && e < 127)
#endif
	  ? e : '.';
      n++;
      എങ്കിൽ (++p == cols)
	{
	  l[c++] = '\n';
	  l[c] = '\0';

	  xxdline(fpo, l, color ? colors : ശൂന്യ, autoskip ? nonzero : 1);
	  memset(colors, 0, c);
	  nonzero = 0;
	  p = 0;
	}
    }
  എങ്കിൽ (p)
    {
      l[c++] = '\n';
      l[c] = '\0';
      എങ്കിൽ (color)
	{
	  x = p;
	  എങ്കിൽ (hextype == HEX_LITTLEENDIAN)
	    {
	      പൂർണ്ണസംഖ്യ fill = octspergrp - (p % octspergrp);
	      എങ്കിൽ (fill == octspergrp) fill = 0;

	      c = addrlen + 1 + (grplen * (x - (octspergrp-fill))) / octspergrp;

	      വേണ്ടി (i = 0; i < fill;i++)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		  x++;
		  p++;
		}
	    }

	  എങ്കിൽ (hextype != HEX_BITS)
	    {
	      c = addrlen + 1 + (grplen * x) / octspergrp;
	      c += cols - p;
	      c += (cols - p) / octspergrp;

	      വേണ്ടി (i = cols - p; i > 0;i--)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		}
	    }
	  xxdline(fpo, l, colors, 1);
	}
      അല്ലെങ്കിൽ
	xxdline(fpo, l, ശൂന്യ, 1);
    }
  അല്ലെങ്കിൽ എങ്കിൽ (autoskip)
    xxdline(fpo, l, color ? colors : ശൂന്യ, -1);	/* last chance to flush out suppressed lines */

  fclose_or_die(fp, fpo);
  തിരികെ 0;
}

/* vi:set ts=8 sw=4 sts=2 cino+={2 cino+=n-2 : */
