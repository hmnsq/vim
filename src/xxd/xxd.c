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
#만약 _MSC_VER >= 1400
# define _CRT_SECURE_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#endif
#만약 !defined(CYGWIN) && defined(__CYGWIN__)
# define CYGWIN
#endif

#만약 (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__)
# define _XOPEN_SOURCE 700   /* for fdopen() */
#endif

#include <stdio.h>
#ifdef VAXC
# include <file.h>
#아니면
# include <fcntl.h>
#endif
#만약 defined(WIN32) || defined(CYGWIN)
# include <io.h>	/* for setmode() */
#endif
#ifdef WIN32
# include <windows.h>
#endif
#ifdef UNIX
# include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#만약 __MWERKS__ && !defined(BEBOX)
# include <unix.h>	/* for fdopen() on MAC */
#endif


/*  This corrects the problem of missing prototypes for certain functions
 *  in some GNU installations (e.g. SunOS 4.1.x).
 *  Darren Hiebert <darren@hmi.com> (sparc-sun-sunos4.1.3_U1/2.7.2.2)
 */
#만약 defined(__GNUC__) && defined(__STDC__)
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
# 만약 defined(sun) && defined(FILE) && !defined(__SVR4) && defined(__STDC__)
#  define __P(a) a
/* excerpt from my sun_stdlib.h */
extern 정수 fprintf __P((FILE *, char *, ...));
extern 정수 fputs   __P((char *, FILE *));
extern 정수 _flsbuf __P((unsigned char, FILE *));
extern 정수 _filbuf __P((FILE *));
extern 정수 fflush  __P((FILE *));
extern 정수 fclose  __P((FILE *));
extern 정수 fseek   __P((FILE *, long, 정수));
extern 정수 rewind  __P((FILE *));

extern void perror __P((char *));
# endif
#endif

char version[] = "xxd 2026-06-16 by Juergen Weigert et al.";
#ifdef WIN32
char osver[] = " (Win32)";
#아니면
char osver[] = "";
#endif

#만약 defined(WIN32)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "wt")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_BINARY) : O_CREAT)
# define BIN_ASSIGN(fp, yes) setmode(fileno(fp), (yes) ? O_BINARY : O_TEXT)
# define PATH_SEP '\\'
#elif defined(CYGWIN)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "w")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_BINARY) : O_CREAT)
# define BIN_ASSIGN(fp, yes) ((yes) ? (void) setmode(fileno(fp), O_BINARY) : (void) (fp))
# define PATH_SEP '/'
#아니면
# ifdef VMS
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ASSIGN(fp, dummy) fp
#  define PATH_SEP ']'
#  define FILE_SEP '.'
# 아니면
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ASSIGN(fp, dummy) fp
#  define PATH_SEP '/'
# endif
#endif

/* open has only to arguments on the Mac */
#만약 __MWERKS__
# define OPEN(name, mode, umask) open(name, mode)
#아니면
# define OPEN(name, mode, umask) open(name, mode, umask)
#endif

#ifdef AMIGA
# define STRNCMP(s1, s2, l) strncmp(s1, s2, (size_t)l)
#아니면
# define STRNCMP(s1, s2, l) strncmp(s1, s2, l)
#endif

#ifndef __P
# 만약 defined(__STDC__) || defined(WIN32)
#  define __P(a) a
# 아니면
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

char hexxa[] = "0123456789abcdef0123456789ABCDEF", *hexx = hexxa;

/* the different hextypes known by this program: */
#define HEX_NORMAL         0x00 /* no flags set */
#define HEX_POSTSCRIPT     0x01
#define HEX_CINCLUDE       0x02
#define HEX_BITS           0x04 /* not hex a dump, but bits: 01111001 */
#define HEX_LITTLEENDIAN   0x08

#define CONDITIONAL_CAPITALIZE(c) (capitalize ? toupper((unsigned char)(c)) : (c))

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

static char *pname;

  static void
exit_with_usage(void)
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
#아니면
	  "", "");
#endif
  fprintf(stderr, "    -u          use upper case hex letters.\n"
		  "    -R when     colorize the output; <when> can be 'always', 'auto' or 'never'. Default: 'auto'.\n"
		  "    -v          show version: \"%s%s\".\n", version, osver);
  exit(1);
}

  static void
perror_exit(정수 ret)
{
  fprintf(stderr, "%s: ", pname);
  perror(NULL);
  exit(ret);
}

  static void
error_exit(정수 ret, char *msg)
{
  fprintf(stderr, "%s: %s\n", pname, msg);
  exit(ret);
}

  static 정수
getc_or_die(FILE *fpi)
{
  정수 c = getc(fpi);
  만약 (c == EOF && ferror(fpi))
    perror_exit(2);
  반환 c;
}

  static void
putc_or_die(정수 c, FILE *fpo)
{
  만약 (putc(c, fpo) == EOF)
    perror_exit(3);
}

  static void
fputs_or_die(char *s, FILE *fpo)
{
  만약 (fputs(s, fpo) == EOF)
    perror_exit(3);
}

/* Use a macro to allow for different arguments. */
#define FPRINTF_OR_DIE(args) 만약 (fprintf args < 0) perror_exit(3)

  static void
fclose_or_die(FILE *fpi, FILE *fpo)
{
  만약 (fclose(fpo) != 0)
    perror_exit(3);
  만약 (fclose(fpi) != 0)
    perror_exit(2);
}

/*
 * If "c" is a hex digit, return the value.
 * Otherwise return -1.
 */
  static 정수
parse_hex_digit(정수 c)
{
  반환 (c >= '0' && c <= '9') ? c - '0'
	: (c >= 'a' && c <= 'f') ? c - 'a' + 10
	: (c >= 'A' && c <= 'F') ? c - 'A' + 10
	: -1;
}

/*
 * If "c" is a bin digit, return the value.
 * Otherwise return -1.
 */
  static 정수
parse_bin_digit(정수 c)
{
  반환 (c >= '0' && c <= '1') ? c - '0'
	: -1;
}

/*
 * Ignore text on "fpi" until end-of-line or end-of-file.
 * Return the '\n' or EOF character.
 * When an error is encountered exit with an error message.
 */
  static 정수
skip_to_eol(FILE *fpi, 정수 c)
{
  동안 (c != '\n' && c != EOF)
    c = getc_or_die(fpi);
  반환 c;
}

/*
 * Max. cols binary characters are decoded from the input stream per line.
 * Two adjacent garbage characters after evaluated data delimit valid data.
 * Everything up to the next newline is discarded.
 *
 * The name is historic and came from 'undo type opt h'.
 */
  static 정수
huntype(
  FILE *fpi,
  FILE *fpo,
  정수 cols,
  정수 hextype,
  long base_off)
{
  정수 c, ign_garb = 1, n1 = -1, n2 = 0, n3 = 0, p = cols, bt = 0, b = 0, bcnt = 0;
  long have_off = 0, want_off = 0;

  rewind(fpi);

  동안 ((c = getc(fpi)) != EOF)
    {
      만약 (c == '\r')	/* Doze style input file? */
	continue;

      /* Allow multiple spaces.  This doesn't work when there is normal text
       * after the hex codes in the last line that looks like hex, thus only
       * use it for PostScript format. */
      만약 (hextype == HEX_POSTSCRIPT && (c == ' ' || c == '\n' || c == '\t'))
	continue;

      만약 (hextype == HEX_NORMAL || hextype == HEX_POSTSCRIPT)
	{
	  n3 = n2;
	  n2 = n1;

	  n1 = parse_hex_digit(c);
	  만약 (n1 == -1 && ign_garb)
	    continue;
	}
      아니면 /* HEX_BITS */
	{
	  n1 = parse_hex_digit(c);
	  만약 (n1 == -1 && ign_garb)
	    continue;

	  bt = parse_bin_digit(c);
	  만약 (bt != -1)
	    {
	      /* shift via unsigned to avoid signed overflow on bad input */
	      b = (정수)(((unsigned)b << 1) | (unsigned)bt);
	      ++bcnt;
	    }
	}

      ign_garb = 0;

      만약 ((hextype != HEX_POSTSCRIPT) && (p >= cols))
	{
	  만약 (hextype == HEX_NORMAL)
	    {
	      만약 (n1 < 0)
		{
		  p = 0;
		  continue;
		}
	      want_off = (long)(((unsigned long)want_off << 4) | (unsigned)n1);
	    }
	  아니면 /* HEX_BITS */
	    {
	      만약 (n1 < 0)
		{
		  p = 0;
		  bcnt = 0;
		  continue;
		}
	      want_off = (long)(((unsigned long)want_off << 4) | (unsigned)n1);
	    }
	  continue;
	}

      만약 (base_off + want_off != have_off)
	{
	  만약 (fflush(fpo) != 0)
	    perror_exit(3);
#ifdef TRY_SEEK
	  만약 (fseek(fpo, base_off + want_off - have_off, SEEK_CUR) >= 0)
	    have_off = base_off + want_off;
#endif
	  만약 (base_off + want_off < have_off)
	    error_exit(5, "Sorry, cannot seek backwards.");
	  위해 (; have_off < base_off + want_off; have_off++)
	    putc_or_die(0, fpo);
	}

      만약 (hextype == HEX_NORMAL || hextype == HEX_POSTSCRIPT)
	{
	  만약 (n2 >= 0 && n1 >= 0)
	    {
	      putc_or_die((n2 << 4) | n1, fpo);
	      have_off++;
	      want_off++;
	      n1 = -1;
	      만약 (!hextype && (++p >= cols))
	      /* skip the rest of the line as garbage */
	      c = skip_to_eol(fpi, c);
	    }
	  아니면 만약 (n1 < 0 && n2 < 0 && n3 < 0)
	    /* already stumbled into garbage, skip line, wait and see */
	    c = skip_to_eol(fpi, c);
	}
      아니면 /* HEX_BITS */
	{
	  만약 (bcnt == 8)
	    {
	      putc_or_die(b, fpo);
	      have_off++;
	      want_off++;
	      b = 0;
	      bcnt = 0;
	      만약 (++p >= cols)
		/* skip the rest of the line as garbage */
		 c = skip_to_eol(fpi, c);
	    }
	}

      만약 (c == '\n')
	{
	  만약 (hextype == HEX_NORMAL || hextype == HEX_BITS)
	    want_off = 0;
	  p = cols;
	  ign_garb = 1;
	}
    }
  만약 (fflush(fpo) != 0)
    perror_exit(3);
#ifdef TRY_SEEK
  fseek(fpo, 0L, SEEK_END);
#endif
  fclose_or_die(fpi, fpo);
  반환 0;
}


/*
 * Print line l with given colors.
 */
  static void
print_colored_line(FILE *fp, char *l, char *colors)
{
  static char l_colored[LLEN+1];

  만약 (colors)
    {
      정수 c = 0;
      만약 (colors[0])
	{
	  COLOR_PROLOGUE(colors[0])
	}
      l_colored[c++] = l[0];
      정수 i;
      위해 (i = 1; l[i]; i++)
	{
	  만약 (colors[i] != colors[i-1])
	    {
	      만약 (colors[i-1])
		{
		  COLOR_EPILOGUE
		}
	      만약 (colors[i])
		{
		  COLOR_PROLOGUE(colors[i])
		}
	    }
	  l_colored[c++] = l[i];
	}

      만약 (colors[i])
	{
	  COLOR_EPILOGUE
	}
      l_colored[c++] = '\0';

      fputs_or_die(l_colored, fp);
    }
  아니면
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
  static void
xxdline(FILE *fp, char *l, char *colors, 정수 nz)
{
  static char z[LLEN_NO_COLOR+1];
  static char z_colors[LLEN_NO_COLOR+1];
  static signed char zero_seen = 0;

  만약 (!nz && zero_seen == 1)
    {
      strcpy(z, l);
      만약 (colors)
	  memcpy(z_colors, colors, strlen(z));
    }

  만약 (nz || !zero_seen++)
    {
      만약 (nz)
	{
	  만약 (nz < 0)
	    zero_seen--;
	  만약 (zero_seen == 2)
	    print_colored_line(fp, z, z_colors);
	  만약 (zero_seen > 2)
	    fputs_or_die("*\n", fp);
	}
      만약 (nz >= 0 || zero_seen > 0)
	print_colored_line(fp, l, colors);

      만약 (nz)
	zero_seen = 0;
    }

  /* If zero_seen > 3, then its exact value doesn't matter, so long as it
   * remains >3 and incrementing it will not cause overflow. */
  만약 (zero_seen >= 0x7F)
    zero_seen = 4;
}

/* This is an EBCDIC to ASCII conversion table */
/* from a proposed BTL standard April 16, 1979 */
static unsigned char etoa64[] =
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

  static char
get_color_char (정수 e, 정수 ebcdic)
{
  만약 (ebcdic)
    {
      만약 ((e >= 75 && e <= 80) || (e >= 90 && e <= 97) ||
	  (e >= 107 && e <= 111) || (e >= 121 && e <= 127) ||
	  (e >= 129 && e <= 137) || (e >= 145 && e <= 154) ||
	  (e >= 162 && e <= 169) || (e >= 192 && e <= 201) ||
	  (e >= 208 && e <= 217) || (e >= 226 && e <= 233) ||
	  (e >= 240 && e <= 249) || (e == 189) || (e == 64) ||
	  (e == 173) || (e == 224) )
	반환 COLOR_GREEN;

      아니면 만약 (e == 37 || e == 13 || e == 5)
	반환 COLOR_YELLOW;
      아니면 만약 (e == 0)
	반환 COLOR_WHITE;
      아니면 만약 (e == 255)
	반환 COLOR_BLUE;
      아니면
	반환 COLOR_RED;
    }
  아니면  /* ASCII */
    {
#만약 defined(__MVS__) && __CHARSET_LIB == 0
      만약 (e >= 64)
	반환 COLOR_GREEN;
#아니면
      만약 (e > 31 && e < 127)
	반환 COLOR_GREEN;
#endif

      아니면 만약 (e == 9 || e == 10 || e == 13)
	반환 COLOR_YELLOW;
      아니면 만약 (e == 0)
	반환 COLOR_WHITE;
      아니면 만약 (e == 255)
	반환 COLOR_BLUE;
      아니면
	반환 COLOR_RED;
    }
    반환 0;
}

  static 정수
enable_color(void)
{
#ifdef WIN32
  DWORD   mode;
  HANDLE  out;

  만약 (!isatty(1))
    반환 0;

  out = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleMode(out, &mode);
  mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  반환 (정수)SetConsoleMode(out, mode);
#elif defined(UNIX)
  char *term;

  만약 (!isatty(STDOUT_FILENO))
    반환 0;

  term = getenv("TERM");
  만약 (term == NULL || *term == '\0' || !strcmp(term, "dumb"))
    반환 0;

  반환 1;
#아니면
  반환 0;
#endif
}

  정수
main(정수 argc, char *argv[])
{
  FILE *fp, *fpo;
  정수 c, e, p = 0, relseek = 1, negseek = 0, revert = 0, i, x;
  정수 cols = 0, colsgiven = 0, nonzero = 0, autoskip = 0, hextype = HEX_NORMAL;
  정수 capitalize = 0, decimal_offset = 0;
  정수 ebcdic = 0;
  정수 termination = 0;
  정수 octspergrp = -1;	/* number of octets grouped in output */
  정수 grplen;		/* total chars per octet group excluding colors */
  long length = -1, n = 0, seekoff = 0;
  unsigned long displayoff = 0;
  static char l[LLEN_NO_COLOR+1];  /* static because it may be too big for stack */
  static char colors[LLEN_NO_COLOR+1]; /* color array */
  char *pp;
  char *varname = NULL;
  정수 addrlen = 9;
  정수 color = 0;
  정수 color_forced = 0;	/* set when -R always is used */
  char *no_color;
  char cur_color = 0;

  no_color = getenv("NO_COLOR");
  만약 (no_color == NULL || no_color[0] == '\0')
    color = enable_color();

#ifdef AMIGA
  /* This program doesn't work when started from the Workbench */
  만약 (argc == 0)
    exit(1);
#endif

  pname = argv[0];
  위해 (pp = pname; *pp; )
    만약 (*pp++ == PATH_SEP)
      pname = pp;
#ifdef FILE_SEP
  위해 (pp = pname; *pp; pp++)
    만약 (*pp == FILE_SEP)
      {
	*pp = '\0';
	break;
      }
#endif

  동안 (argc >= 2)
    {
      pp = argv[1] + (!STRNCMP(argv[1], "--", 2) && argv[1][2]);
	   만약 (!STRNCMP(pp, "-a", 2)) autoskip = 1 - autoskip;
      아니면 만약 (!STRNCMP(pp, "-b", 2)) hextype |= HEX_BITS;
      아니면 만약 (!STRNCMP(pp, "-e", 2)) hextype |= HEX_LITTLEENDIAN;
      아니면 만약 (!STRNCMP(pp, "-u", 2)) hexx = hexxa + 16;
      아니면 만약 (!STRNCMP(pp, "-p", 2)) hextype |= HEX_POSTSCRIPT;
      아니면 만약 (!STRNCMP(pp, "-i", 2)) hextype |= HEX_CINCLUDE;
      아니면 만약 (!STRNCMP(pp, "-C", 2)) capitalize = 1;
      아니면 만약 (!STRNCMP(pp, "-d", 2)) decimal_offset = 1;
      아니면 만약 (!STRNCMP(pp, "-r", 2)) revert++;
      아니면 만약 (!STRNCMP(pp, "-E", 2)) ebcdic++;
      아니면 만약 (!STRNCMP(pp, "-t", 2)) termination++;
      아니면 만약 (!STRNCMP(pp, "-v", 2))
	{
	  fprintf(stderr, "%s%s\n", version, osver);
	  exit(0);
	}
      아니면 만약 (!STRNCMP(pp, "-c", 2))
	{
	  만약 (pp[2] && !STRNCMP("apitalize", pp + 2, 9))
	    capitalize = 1;
	  아니면 만약 (pp[2] && STRNCMP("ols", pp + 2, 3))
	    {
	      colsgiven = 1;
	      cols = (정수)strtol(pp + 2, NULL, 0);
	    }
	  아니면
	    {
	      만약 (!argv[2])
		exit_with_usage();
	      colsgiven = 1;
	      cols = (정수)strtol(argv[2], NULL, 0);
	      argv++;
	      argc--;
	    }
	}
      아니면 만약 (!STRNCMP(pp, "-g", 2))
	{
	  만약 (pp[2] && STRNCMP("roup", pp + 2, 4))
	    octspergrp = (정수)strtol(pp + 2, NULL, 0);
	  아니면
	    {
	      만약 (!argv[2])
		exit_with_usage();
	      octspergrp = (정수)strtol(argv[2], NULL, 0);
	      argv++;
	      argc--;
	    }
	}
      아니면 만약 (!STRNCMP(pp, "-o", 2))
	{
	  정수 reloffset = 0;
	  정수 negoffset = 0;
	  만약 (pp[2] && STRNCMP("ffset", pp + 2, 5))
	    displayoff = strtoul(pp + 2, NULL, 0);
	  아니면
	    {
	      만약 (!argv[2])
		exit_with_usage();

	      만약 (argv[2][0] == '+')
	       reloffset++;
	     만약 (argv[2][reloffset] == '-')
	       negoffset++;

	     만약 (negoffset)
	       displayoff = ULONG_MAX - strtoul(argv[2] + reloffset+negoffset, NULL, 0) + 1;
	     아니면
	       displayoff = strtoul(argv[2] + reloffset+negoffset, NULL, 0);

	      argv++;
	      argc--;
	    }
	}
      아니면 만약 (!STRNCMP(pp, "-s", 2))
	{
	  relseek = 0;
	  negseek = 0;
	  만약 (pp[2] && STRNCMP("kip", pp+2, 3) && STRNCMP("eek", pp+2, 3))
	    {
#ifdef TRY_SEEK
	      만약 (pp[2] == '+')
		relseek++;
	      만약 (pp[2+relseek] == '-')
		negseek++;
#endif
	      seekoff = strtol(pp + 2+relseek+negseek, (char **)NULL, 0);
	    }
	  아니면
	    {
	      만약 (!argv[2])
		exit_with_usage();
#ifdef TRY_SEEK
	      만약 (argv[2][0] == '+')
		relseek++;
	      만약 (argv[2][relseek] == '-')
		negseek++;
#endif
	      seekoff = strtol(argv[2] + relseek+negseek, (char **)NULL, 0);
	      argv++;
	      argc--;
	    }
	}
      아니면 만약 (!STRNCMP(pp, "-l", 2))
	{
	  만약 (pp[2] && STRNCMP("en", pp + 2, 2))
	    length = strtol(pp + 2, (char **)NULL, 0);
	  아니면
	    {
	      만약 (!argv[2])
		exit_with_usage();
	      length = strtol(argv[2], (char **)NULL, 0);
	      argv++;
	      argc--;
	    }
	}
      아니면 만약 (!STRNCMP(pp, "-n", 2))
	{
	  만약 (pp[2] && STRNCMP("ame", pp + 2, 3))
	    varname = pp + 2;
	  아니면
	    {
	      만약 (!argv[2])
		exit_with_usage();
	      varname = argv[2];
	      argv++;
	      argc--;
	    }
	}
      아니면 만약 (!STRNCMP(pp, "-R", 2))
	{
	  char *pw = pp + 2;
	  만약 (!pw[0])
	    {
	      pw = argv[2];
	      argv++;
	      argc--;
	    }
	  만약 (!pw)
	    exit_with_usage();
	  만약 (!STRNCMP(pw, "always", 6))
	    {
	      (void)enable_color();
	      color = 1;
	      color_forced = 1;
	    }
	  아니면 만약 (!STRNCMP(pw, "never", 5))
	    color = 0;
	  아니면 만약 (!STRNCMP(pw, "auto", 4))
	    color = enable_color();
	  아니면
	    exit_with_usage();
	}
      아니면 만약 (!strcmp(argv[1], "--"))	/* end of options */
	{
	  argv++;
	  argc--;
	  break;
	}
      아니면 만약 (pp[0] == '-' && pp[1])	/* unknown option */
	exit_with_usage();
      아니면
	break;				/* not an option */

      argv++;				/* advance to next argument */
      argc--;
    }

  만약 (hextype != (HEX_CINCLUDE | HEX_BITS))
    {
	/* Allow at most one bit to be set in hextype */
	만약 (hextype & (hextype - 1))
	    error_exit(1, "only one of -b, -e, -u, -p, -i can be used");
    }

  만약 (!colsgiven || (!cols && hextype != HEX_POSTSCRIPT))
    switch (hextype)
      {
      case HEX_POSTSCRIPT:	cols = 30; break;
      case HEX_CINCLUDE:	cols = 12; break;
      case HEX_CINCLUDE | HEX_BITS:
      case HEX_BITS:		cols = 6; break;
      case HEX_NORMAL:
      case HEX_LITTLEENDIAN:
      default:			cols = 16; break;
      }

  만약 (octspergrp < 0)
    switch (hextype)
      {
      case HEX_CINCLUDE | HEX_BITS:
      case HEX_BITS:		octspergrp = 1; break;
      case HEX_NORMAL:		octspergrp = 2; break;
      case HEX_LITTLEENDIAN:	octspergrp = 4; break;
      case HEX_POSTSCRIPT:
      case HEX_CINCLUDE:
      default:			octspergrp = 0; break;
      }

  만약 ((hextype == HEX_POSTSCRIPT && cols < 0) ||
      (hextype != HEX_POSTSCRIPT && cols < 1) ||
      ((hextype == HEX_NORMAL || hextype == HEX_BITS || hextype == HEX_LITTLEENDIAN)
							    && (cols > COLS)))
    {
      fprintf(stderr, "%s: invalid number of columns (max. %d).\n", pname, COLS);
      exit(1);
    }

  만약 (octspergrp < 1 || octspergrp > cols)
    octspergrp = cols;
  아니면 만약 (hextype == HEX_LITTLEENDIAN && (octspergrp & (octspergrp-1)))
    error_exit(1, "number of octets per group must be a power of 2 with -e.");

  만약 (argc > 3)
    exit_with_usage();

  만약 (argc == 1 || (argv[1][0] == '-' && !argv[1][1]))
    BIN_ASSIGN(fp = stdin, !revert);
  아니면
    {
      만약 ((fp = fopen(argv[1], BIN_READ(!revert))) == NULL)
	{
	  fprintf(stderr,"%s: ", pname);
	  perror(argv[1]);
	  반환 2;
	}
    }

  만약 (argc < 3 || (argv[2][0] == '-' && !argv[2][1]))
    BIN_ASSIGN(fpo = stdout, revert);
  아니면
    {
      정수 fd;
      정수 mode = revert ? O_WRONLY : (O_TRUNC|O_WRONLY);

      만약 (((fd = OPEN(argv[2], mode | BIN_CREAT(revert), 0666)) < 0) ||
	  (fpo = fdopen(fd, BIN_WRITE(revert))) == NULL)
	{
	  fprintf(stderr, "%s: ", pname);
	  perror(argv[2]);
	  반환 3;
	}
      rewind(fpo);

      /* Disable auto color when writing to a file. */
      만약 (!color_forced)
	color = 0;
    }

  만약 (revert)
    switch (hextype)
      {
      case HEX_NORMAL:
      case HEX_POSTSCRIPT:
      case HEX_BITS:
	반환 huntype(fp, fpo, cols, hextype,
	  negseek ? -seekoff : seekoff);
	break;
      default:
	error_exit(-1, "Sorry, cannot revert this type of hexdump");
      }

  만약 (seekoff || negseek || !relseek)
    {
#ifdef TRY_SEEK
      만약 (relseek)
	e = fseek(fp, negseek ? -seekoff : seekoff, SEEK_CUR);
      아니면
	e = fseek(fp, negseek ? -seekoff : seekoff,
						negseek ? SEEK_END : SEEK_SET);
      만약 (e < 0 && negseek)
	error_exit(4, "Sorry, cannot seek.");
      만약 (e >= 0)
	seekoff = ftell(fp);
      아니면
#endif
	{
	  long s = seekoff;

	  동안 (s--)
	    만약 (getc_or_die(fp) == EOF)
	    {
	      error_exit(4, "Sorry, cannot seek.");
	    }
	}
    }

  만약 (hextype & HEX_CINCLUDE)
    {
      /* A user-set variable name overrides fp == stdin */
      만약 (varname == NULL && fp != stdin)
	varname = argv[1];

      만약 (varname != NULL)
	{
	  FPRINTF_OR_DIE((fpo, "unsigned char %s", isdigit((unsigned char)varname[0]) ? "__" : ""));
	  위해 (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(isalnum((unsigned char)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  fputs_or_die("[] = {\n", fpo);
	}

      p = 0;
      동안 ((length < 0 || p < length) && (((c = getc_or_die(fp)) != EOF) || termination))
	{
	  만약 (c == EOF)
	    {
	      c = 0;
	      termination = -1;
	    }
	  만약 (hextype & HEX_BITS)
	    {
	      만약 (p == 0)
		fputs_or_die("  ", fpo);
	      아니면 만약 (p % cols == 0)
		fputs_or_die(",\n  ", fpo);
	      아니면
		fputs_or_die(", ", fpo);

	      FPRINTF_OR_DIE((fpo, "0b"));
	      위해 (정수 j = 7; j >= 0; j--)
		putc_or_die((c & (1 << j)) ? '1' : '0', fpo);
	      p++;
	    }
	  아니면
	    {
	      FPRINTF_OR_DIE((fpo, (hexx == hexxa) ? "%s0x%02x" : "%s0X%02X",
		(p % cols) ? ", " : (!p ? "  " : ",\n  "), c));
	      p++;
	    }
	  만약 (termination == -1)
	    {
	      --p;
	      break ;
	    }
	}

      만약 (p)
	fputs_or_die("\n", fpo);

      만약 (varname != NULL)
	{
	  fputs_or_die("};\n", fpo);
	  FPRINTF_OR_DIE((fpo, "unsigned int %s", isdigit((unsigned char)varname[0]) ? "__" : ""));
	  위해 (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(isalnum((unsigned char)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  FPRINTF_OR_DIE((fpo, "_%s = %d;\n", capitalize ? "LEN" : "len", p));
	}

      fclose_or_die(fp, fpo);
      반환 0;
    }

  만약 (hextype == HEX_POSTSCRIPT)
    {
      p = cols;
      동안 ((length < 0 || n < length) && (e = getc_or_die(fp)) != EOF)
	{
	  putc_or_die(hexx[(e >> 4) & 0xf], fpo);
	  putc_or_die(hexx[e & 0xf], fpo);
	  n++;
	  만약 (cols > 0 && !--p)
	    {
	      putc_or_die('\n', fpo);
	      p = cols;
	    }
	}
      만약 (cols == 0 || p < cols)
	putc_or_die('\n', fpo);
      fclose_or_die(fp, fpo);
      반환 0;
    }

  /* hextype: HEX_NORMAL or HEX_BITS or HEX_LITTLEENDIAN */

  만약 (hextype != HEX_BITS)
    {
      grplen = octspergrp + octspergrp + 1;	/* chars per octet group */
    }
  아니면	/* hextype == HEX_BITS */
    grplen = 8 * octspergrp + 1;

  동안 ((length < 0 || n < length) && (e = getc_or_die(fp)) != EOF)
    {
      만약 (p == 0)
	{
	  addrlen = sprintf(l, decimal_offset ? "%08lu:" : "%08lx:",
				  ((unsigned long)(n + seekoff + displayoff)));
	  위해 (c = addrlen; c < LLEN_NO_COLOR; l[c++] = ' ')
	    ;
	}
      x = hextype == HEX_LITTLEENDIAN ? p ^ (octspergrp-1) : p;
      c = addrlen + 1 + (grplen * x) / octspergrp;
      만약 (hextype == HEX_NORMAL || hextype == HEX_LITTLEENDIAN)
	{
	  만약 (color)
	    {
	      cur_color = get_color_char(e, ebcdic);
	      colors[c] = cur_color;
	      colors[c+1] = cur_color;
	    }

	  l[c]   = hexx[(e >> 4) & 0xf];
	  l[++c] = hexx[e & 0xf];
	}
      아니면 /* hextype == HEX_BITS */
	{
	  만약 (color)
	    cur_color = get_color_char(e, ebcdic);

	  위해 (i = 7; i >= 0; i--)
	    {
	      만약 (color)
		colors[c] = cur_color;
	      l[c++] = (e & (1 << i)) ? '1' : '0';
	    }
	}
      만약 (e)
	nonzero++;
      /* When changing this update definition of LLEN and LLEN_NO_COLOR above. */
      만약 (hextype == HEX_LITTLEENDIAN)
	/* last group will be fully used, round up */
	c = grplen * ((cols + octspergrp - 1) / octspergrp);
      아니면
	c = (grplen * cols - 1) / octspergrp;


      만약 (ebcdic)
	e = (e < 64) ? '.' : etoa64[e-64];

      만약 (hextype == HEX_LITTLEENDIAN)
	c -= 1;

      c += addrlen + 3 + p;
      만약 (color)
	  colors[c] = cur_color;
      l[c++] =
#만약 defined(__MVS__) && __CHARSET_LIB == 0
	  (e >= 64)
#아니면
	  (e > 31 && e < 127)
#endif
	  ? e : '.';
      n++;
      만약 (++p == cols)
	{
	  l[c++] = '\n';
	  l[c] = '\0';

	  xxdline(fpo, l, color ? colors : NULL, autoskip ? nonzero : 1);
	  memset(colors, 0, c);
	  nonzero = 0;
	  p = 0;
	}
    }
  만약 (p)
    {
      l[c++] = '\n';
      l[c] = '\0';
      만약 (color)
	{
	  x = p;
	  만약 (hextype == HEX_LITTLEENDIAN)
	    {
	      정수 fill = octspergrp - (p % octspergrp);
	      만약 (fill == octspergrp) fill = 0;

	      c = addrlen + 1 + (grplen * (x - (octspergrp-fill))) / octspergrp;

	      위해 (i = 0; i < fill;i++)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		  x++;
		  p++;
		}
	    }

	  만약 (hextype != HEX_BITS)
	    {
	      c = addrlen + 1 + (grplen * x) / octspergrp;
	      c += cols - p;
	      c += (cols - p) / octspergrp;

	      위해 (i = cols - p; i > 0;i--)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		}
	    }
	  xxdline(fpo, l, colors, 1);
	}
      아니면
	xxdline(fpo, l, NULL, 1);
    }
  아니면 만약 (autoskip)
    xxdline(fpo, l, color ? colors : NULL, -1);	/* last chance to flush out suppressed lines */

  fclose_or_die(fp, fpo);
  반환 0;
}

/* vi:set ts=8 sw=4 sts=2 cino+={2 cino+=n-2 : */
