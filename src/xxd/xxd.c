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
#если _MSC_VER >= 1400
# define _CRT_SECURE_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#endif
#если !defined(CYGWIN) && defined(__CYGWIN__)
# define CYGWIN
#endif

#если (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__)
# define _XOPEN_SOURCE 700   /* for fdopen() */
#endif

#include <stdio.h>
#ifdef VAXC
# include <file.h>
#иначе
# include <fcntl.h>
#endif
#если defined(WIN32) || defined(CYGWIN)
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
#если __MWERKS__ && !defined(BEBOX)
# include <unix.h>	/* for fdopen() on MAC */
#endif


/*  This corrects the problem of missing prototypes for certain functions
 *  in some GNU installations (e.g. SunOS 4.1.x).
 *  Darren Hiebert <darren@hmi.com> (sparc-sun-sunos4.1.3_U1/2.7.2.2)
 */
#если defined(__GNUC__) && defined(__STDC__)
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
# если defined(sun) && defined(FILE) && !defined(__SVR4) && defined(__STDC__)
#  define __P(a) a
/* excerpt from my sun_stdlib.h */
extern целое fprintf __P((FILE *, char *, ...));
extern целое fputs   __P((char *, FILE *));
extern целое _flsbuf __P((unsigned char, FILE *));
extern целое _filbuf __P((FILE *));
extern целое fflush  __P((FILE *));
extern целое fclose  __P((FILE *));
extern целое fseek   __P((FILE *, long, целое));
extern целое rewind  __P((FILE *));

extern void perror __P((char *));
# endif
#endif

char version[] = "xxd 2026-06-16 by Juergen Weigert et al.";
#ifdef WIN32
char osver[] = " (Win32)";
#иначе
char osver[] = "";
#endif

#если defined(WIN32)
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
#иначе
# ifdef VMS
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ASSIGN(fp, dummy) fp
#  define PATH_SEP ']'
#  define FILE_SEP '.'
# иначе
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ASSIGN(fp, dummy) fp
#  define PATH_SEP '/'
# endif
#endif

/* open has only to arguments on the Mac */
#если __MWERKS__
# define OPEN(name, mode, umask) open(name, mode)
#иначе
# define OPEN(name, mode, umask) open(name, mode, umask)
#endif

#ifdef AMIGA
# define STRNCMP(s1, s2, l) strncmp(s1, s2, (size_t)l)
#иначе
# define STRNCMP(s1, s2, l) strncmp(s1, s2, l)
#endif

#ifndef __P
# если defined(__STDC__) || defined(WIN32)
#  define __P(a) a
# иначе
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
#иначе
	  "", "");
#endif
  fprintf(stderr, "    -u          use upper case hex letters.\n"
		  "    -R when     colorize the output; <when> can be 'always', 'auto' or 'never'. Default: 'auto'.\n"
		  "    -v          show version: \"%s%s\".\n", version, osver);
  exit(1);
}

  static void
perror_exit(целое ret)
{
  fprintf(stderr, "%s: ", pname);
  perror(NULL);
  exit(ret);
}

  static void
error_exit(целое ret, char *msg)
{
  fprintf(stderr, "%s: %s\n", pname, msg);
  exit(ret);
}

  static целое
getc_or_die(FILE *fpi)
{
  целое c = getc(fpi);
  если (c == EOF && ferror(fpi))
    perror_exit(2);
  вернуть c;
}

  static void
putc_or_die(целое c, FILE *fpo)
{
  если (putc(c, fpo) == EOF)
    perror_exit(3);
}

  static void
fputs_or_die(char *s, FILE *fpo)
{
  если (fputs(s, fpo) == EOF)
    perror_exit(3);
}

/* Use a macro to allow for different arguments. */
#define FPRINTF_OR_DIE(args) если (fprintf args < 0) perror_exit(3)

  static void
fclose_or_die(FILE *fpi, FILE *fpo)
{
  если (fclose(fpo) != 0)
    perror_exit(3);
  если (fclose(fpi) != 0)
    perror_exit(2);
}

/*
 * If "c" is a hex digit, return the value.
 * Otherwise return -1.
 */
  static целое
parse_hex_digit(целое c)
{
  вернуть (c >= '0' && c <= '9') ? c - '0'
	: (c >= 'a' && c <= 'f') ? c - 'a' + 10
	: (c >= 'A' && c <= 'F') ? c - 'A' + 10
	: -1;
}

/*
 * If "c" is a bin digit, return the value.
 * Otherwise return -1.
 */
  static целое
parse_bin_digit(целое c)
{
  вернуть (c >= '0' && c <= '1') ? c - '0'
	: -1;
}

/*
 * Ignore text on "fpi" until end-of-line or end-of-file.
 * Return the '\n' or EOF character.
 * When an error is encountered exit with an error message.
 */
  static целое
skip_to_eol(FILE *fpi, целое c)
{
  пока (c != '\n' && c != EOF)
    c = getc_or_die(fpi);
  вернуть c;
}

/*
 * Max. cols binary characters are decoded from the input stream per line.
 * Two adjacent garbage characters after evaluated data delimit valid data.
 * Everything up to the next newline is discarded.
 *
 * The name is historic and came from 'undo type opt h'.
 */
  static целое
huntype(
  FILE *fpi,
  FILE *fpo,
  целое cols,
  целое hextype,
  long base_off)
{
  целое c, ign_garb = 1, n1 = -1, n2 = 0, n3 = 0, p = cols, bt = 0, b = 0, bcnt = 0;
  long have_off = 0, want_off = 0;

  rewind(fpi);

  пока ((c = getc(fpi)) != EOF)
    {
      если (c == '\r')	/* Doze style input file? */
	continue;

      /* Allow multiple spaces.  This doesn't work when there is normal text
       * after the hex codes in the last line that looks like hex, thus only
       * use it for PostScript format. */
      если (hextype == HEX_POSTSCRIPT && (c == ' ' || c == '\n' || c == '\t'))
	continue;

      если (hextype == HEX_NORMAL || hextype == HEX_POSTSCRIPT)
	{
	  n3 = n2;
	  n2 = n1;

	  n1 = parse_hex_digit(c);
	  если (n1 == -1 && ign_garb)
	    continue;
	}
      иначе /* HEX_BITS */
	{
	  n1 = parse_hex_digit(c);
	  если (n1 == -1 && ign_garb)
	    continue;

	  bt = parse_bin_digit(c);
	  если (bt != -1)
	    {
	      /* shift via unsigned to avoid signed overflow on bad input */
	      b = (целое)(((unsigned)b << 1) | (unsigned)bt);
	      ++bcnt;
	    }
	}

      ign_garb = 0;

      если ((hextype != HEX_POSTSCRIPT) && (p >= cols))
	{
	  если (hextype == HEX_NORMAL)
	    {
	      если (n1 < 0)
		{
		  p = 0;
		  continue;
		}
	      want_off = (long)(((unsigned long)want_off << 4) | (unsigned)n1);
	    }
	  иначе /* HEX_BITS */
	    {
	      если (n1 < 0)
		{
		  p = 0;
		  bcnt = 0;
		  continue;
		}
	      want_off = (long)(((unsigned long)want_off << 4) | (unsigned)n1);
	    }
	  continue;
	}

      если (base_off + want_off != have_off)
	{
	  если (fflush(fpo) != 0)
	    perror_exit(3);
#ifdef TRY_SEEK
	  если (fseek(fpo, base_off + want_off - have_off, SEEK_CUR) >= 0)
	    have_off = base_off + want_off;
#endif
	  если (base_off + want_off < have_off)
	    error_exit(5, "Sorry, cannot seek backwards.");
	  для (; have_off < base_off + want_off; have_off++)
	    putc_or_die(0, fpo);
	}

      если (hextype == HEX_NORMAL || hextype == HEX_POSTSCRIPT)
	{
	  если (n2 >= 0 && n1 >= 0)
	    {
	      putc_or_die((n2 << 4) | n1, fpo);
	      have_off++;
	      want_off++;
	      n1 = -1;
	      если (!hextype && (++p >= cols))
	      /* skip the rest of the line as garbage */
	      c = skip_to_eol(fpi, c);
	    }
	  иначе если (n1 < 0 && n2 < 0 && n3 < 0)
	    /* already stumbled into garbage, skip line, wait and see */
	    c = skip_to_eol(fpi, c);
	}
      иначе /* HEX_BITS */
	{
	  если (bcnt == 8)
	    {
	      putc_or_die(b, fpo);
	      have_off++;
	      want_off++;
	      b = 0;
	      bcnt = 0;
	      если (++p >= cols)
		/* skip the rest of the line as garbage */
		 c = skip_to_eol(fpi, c);
	    }
	}

      если (c == '\n')
	{
	  если (hextype == HEX_NORMAL || hextype == HEX_BITS)
	    want_off = 0;
	  p = cols;
	  ign_garb = 1;
	}
    }
  если (fflush(fpo) != 0)
    perror_exit(3);
#ifdef TRY_SEEK
  fseek(fpo, 0L, SEEK_END);
#endif
  fclose_or_die(fpi, fpo);
  вернуть 0;
}


/*
 * Print line l with given colors.
 */
  static void
print_colored_line(FILE *fp, char *l, char *colors)
{
  static char l_colored[LLEN+1];

  если (colors)
    {
      целое c = 0;
      если (colors[0])
	{
	  COLOR_PROLOGUE(colors[0])
	}
      l_colored[c++] = l[0];
      целое i;
      для (i = 1; l[i]; i++)
	{
	  если (colors[i] != colors[i-1])
	    {
	      если (colors[i-1])
		{
		  COLOR_EPILOGUE
		}
	      если (colors[i])
		{
		  COLOR_PROLOGUE(colors[i])
		}
	    }
	  l_colored[c++] = l[i];
	}

      если (colors[i])
	{
	  COLOR_EPILOGUE
	}
      l_colored[c++] = '\0';

      fputs_or_die(l_colored, fp);
    }
  иначе
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
xxdline(FILE *fp, char *l, char *colors, целое nz)
{
  static char z[LLEN_NO_COLOR+1];
  static char z_colors[LLEN_NO_COLOR+1];
  static signed char zero_seen = 0;

  если (!nz && zero_seen == 1)
    {
      strcpy(z, l);
      если (colors)
	  memcpy(z_colors, colors, strlen(z));
    }

  если (nz || !zero_seen++)
    {
      если (nz)
	{
	  если (nz < 0)
	    zero_seen--;
	  если (zero_seen == 2)
	    print_colored_line(fp, z, z_colors);
	  если (zero_seen > 2)
	    fputs_or_die("*\n", fp);
	}
      если (nz >= 0 || zero_seen > 0)
	print_colored_line(fp, l, colors);

      если (nz)
	zero_seen = 0;
    }

  /* If zero_seen > 3, then its exact value doesn't matter, so long as it
   * remains >3 and incrementing it will not cause overflow. */
  если (zero_seen >= 0x7F)
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
get_color_char (целое e, целое ebcdic)
{
  если (ebcdic)
    {
      если ((e >= 75 && e <= 80) || (e >= 90 && e <= 97) ||
	  (e >= 107 && e <= 111) || (e >= 121 && e <= 127) ||
	  (e >= 129 && e <= 137) || (e >= 145 && e <= 154) ||
	  (e >= 162 && e <= 169) || (e >= 192 && e <= 201) ||
	  (e >= 208 && e <= 217) || (e >= 226 && e <= 233) ||
	  (e >= 240 && e <= 249) || (e == 189) || (e == 64) ||
	  (e == 173) || (e == 224) )
	вернуть COLOR_GREEN;

      иначе если (e == 37 || e == 13 || e == 5)
	вернуть COLOR_YELLOW;
      иначе если (e == 0)
	вернуть COLOR_WHITE;
      иначе если (e == 255)
	вернуть COLOR_BLUE;
      иначе
	вернуть COLOR_RED;
    }
  иначе  /* ASCII */
    {
#если defined(__MVS__) && __CHARSET_LIB == 0
      если (e >= 64)
	вернуть COLOR_GREEN;
#иначе
      если (e > 31 && e < 127)
	вернуть COLOR_GREEN;
#endif

      иначе если (e == 9 || e == 10 || e == 13)
	вернуть COLOR_YELLOW;
      иначе если (e == 0)
	вернуть COLOR_WHITE;
      иначе если (e == 255)
	вернуть COLOR_BLUE;
      иначе
	вернуть COLOR_RED;
    }
    вернуть 0;
}

  static целое
enable_color(void)
{
#ifdef WIN32
  DWORD   mode;
  HANDLE  out;

  если (!isatty(1))
    вернуть 0;

  out = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleMode(out, &mode);
  mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  вернуть (целое)SetConsoleMode(out, mode);
#elif defined(UNIX)
  char *term;

  если (!isatty(STDOUT_FILENO))
    вернуть 0;

  term = getenv("TERM");
  если (term == NULL || *term == '\0' || !strcmp(term, "dumb"))
    вернуть 0;

  вернуть 1;
#иначе
  вернуть 0;
#endif
}

  целое
основной(целое argc, char *argv[])
{
  FILE *fp, *fpo;
  целое c, e, p = 0, relseek = 1, negseek = 0, revert = 0, i, x;
  целое cols = 0, colsgiven = 0, nonzero = 0, autoskip = 0, hextype = HEX_NORMAL;
  целое capitalize = 0, decimal_offset = 0;
  целое ebcdic = 0;
  целое termination = 0;
  целое octspergrp = -1;	/* number of octets grouped in output */
  целое grplen;		/* total chars per octet group excluding colors */
  long length = -1, n = 0, seekoff = 0;
  unsigned long displayoff = 0;
  static char l[LLEN_NO_COLOR+1];  /* static because it may be too big for stack */
  static char colors[LLEN_NO_COLOR+1]; /* color array */
  char *pp;
  char *varname = NULL;
  целое addrlen = 9;
  целое color = 0;
  целое color_forced = 0;	/* set when -R always is used */
  char *no_color;
  char cur_color = 0;

  no_color = getenv("NO_COLOR");
  если (no_color == NULL || no_color[0] == '\0')
    color = enable_color();

#ifdef AMIGA
  /* This program doesn't work when started from the Workbench */
  если (argc == 0)
    exit(1);
#endif

  pname = argv[0];
  для (pp = pname; *pp; )
    если (*pp++ == PATH_SEP)
      pname = pp;
#ifdef FILE_SEP
  для (pp = pname; *pp; pp++)
    если (*pp == FILE_SEP)
      {
	*pp = '\0';
	break;
      }
#endif

  пока (argc >= 2)
    {
      pp = argv[1] + (!STRNCMP(argv[1], "--", 2) && argv[1][2]);
	   если (!STRNCMP(pp, "-a", 2)) autoskip = 1 - autoskip;
      иначе если (!STRNCMP(pp, "-b", 2)) hextype |= HEX_BITS;
      иначе если (!STRNCMP(pp, "-e", 2)) hextype |= HEX_LITTLEENDIAN;
      иначе если (!STRNCMP(pp, "-u", 2)) hexx = hexxa + 16;
      иначе если (!STRNCMP(pp, "-p", 2)) hextype |= HEX_POSTSCRIPT;
      иначе если (!STRNCMP(pp, "-i", 2)) hextype |= HEX_CINCLUDE;
      иначе если (!STRNCMP(pp, "-C", 2)) capitalize = 1;
      иначе если (!STRNCMP(pp, "-d", 2)) decimal_offset = 1;
      иначе если (!STRNCMP(pp, "-r", 2)) revert++;
      иначе если (!STRNCMP(pp, "-E", 2)) ebcdic++;
      иначе если (!STRNCMP(pp, "-t", 2)) termination++;
      иначе если (!STRNCMP(pp, "-v", 2))
	{
	  fprintf(stderr, "%s%s\n", version, osver);
	  exit(0);
	}
      иначе если (!STRNCMP(pp, "-c", 2))
	{
	  если (pp[2] && !STRNCMP("apitalize", pp + 2, 9))
	    capitalize = 1;
	  иначе если (pp[2] && STRNCMP("ols", pp + 2, 3))
	    {
	      colsgiven = 1;
	      cols = (целое)strtol(pp + 2, NULL, 0);
	    }
	  иначе
	    {
	      если (!argv[2])
		exit_with_usage();
	      colsgiven = 1;
	      cols = (целое)strtol(argv[2], NULL, 0);
	      argv++;
	      argc--;
	    }
	}
      иначе если (!STRNCMP(pp, "-g", 2))
	{
	  если (pp[2] && STRNCMP("roup", pp + 2, 4))
	    octspergrp = (целое)strtol(pp + 2, NULL, 0);
	  иначе
	    {
	      если (!argv[2])
		exit_with_usage();
	      octspergrp = (целое)strtol(argv[2], NULL, 0);
	      argv++;
	      argc--;
	    }
	}
      иначе если (!STRNCMP(pp, "-o", 2))
	{
	  целое reloffset = 0;
	  целое negoffset = 0;
	  если (pp[2] && STRNCMP("ffset", pp + 2, 5))
	    displayoff = strtoul(pp + 2, NULL, 0);
	  иначе
	    {
	      если (!argv[2])
		exit_with_usage();

	      если (argv[2][0] == '+')
	       reloffset++;
	     если (argv[2][reloffset] == '-')
	       negoffset++;

	     если (negoffset)
	       displayoff = ULONG_MAX - strtoul(argv[2] + reloffset+negoffset, NULL, 0) + 1;
	     иначе
	       displayoff = strtoul(argv[2] + reloffset+negoffset, NULL, 0);

	      argv++;
	      argc--;
	    }
	}
      иначе если (!STRNCMP(pp, "-s", 2))
	{
	  relseek = 0;
	  negseek = 0;
	  если (pp[2] && STRNCMP("kip", pp+2, 3) && STRNCMP("eek", pp+2, 3))
	    {
#ifdef TRY_SEEK
	      если (pp[2] == '+')
		relseek++;
	      если (pp[2+relseek] == '-')
		negseek++;
#endif
	      seekoff = strtol(pp + 2+relseek+negseek, (char **)NULL, 0);
	    }
	  иначе
	    {
	      если (!argv[2])
		exit_with_usage();
#ifdef TRY_SEEK
	      если (argv[2][0] == '+')
		relseek++;
	      если (argv[2][relseek] == '-')
		negseek++;
#endif
	      seekoff = strtol(argv[2] + relseek+negseek, (char **)NULL, 0);
	      argv++;
	      argc--;
	    }
	}
      иначе если (!STRNCMP(pp, "-l", 2))
	{
	  если (pp[2] && STRNCMP("en", pp + 2, 2))
	    length = strtol(pp + 2, (char **)NULL, 0);
	  иначе
	    {
	      если (!argv[2])
		exit_with_usage();
	      length = strtol(argv[2], (char **)NULL, 0);
	      argv++;
	      argc--;
	    }
	}
      иначе если (!STRNCMP(pp, "-n", 2))
	{
	  если (pp[2] && STRNCMP("ame", pp + 2, 3))
	    varname = pp + 2;
	  иначе
	    {
	      если (!argv[2])
		exit_with_usage();
	      varname = argv[2];
	      argv++;
	      argc--;
	    }
	}
      иначе если (!STRNCMP(pp, "-R", 2))
	{
	  char *pw = pp + 2;
	  если (!pw[0])
	    {
	      pw = argv[2];
	      argv++;
	      argc--;
	    }
	  если (!pw)
	    exit_with_usage();
	  если (!STRNCMP(pw, "always", 6))
	    {
	      (void)enable_color();
	      color = 1;
	      color_forced = 1;
	    }
	  иначе если (!STRNCMP(pw, "never", 5))
	    color = 0;
	  иначе если (!STRNCMP(pw, "auto", 4))
	    color = enable_color();
	  иначе
	    exit_with_usage();
	}
      иначе если (!strcmp(argv[1], "--"))	/* end of options */
	{
	  argv++;
	  argc--;
	  break;
	}
      иначе если (pp[0] == '-' && pp[1])	/* unknown option */
	exit_with_usage();
      иначе
	break;				/* not an option */

      argv++;				/* advance to next argument */
      argc--;
    }

  если (hextype != (HEX_CINCLUDE | HEX_BITS))
    {
	/* Allow at most one bit to be set in hextype */
	если (hextype & (hextype - 1))
	    error_exit(1, "only one of -b, -e, -u, -p, -i can be used");
    }

  если (!colsgiven || (!cols && hextype != HEX_POSTSCRIPT))
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

  если (octspergrp < 0)
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

  если ((hextype == HEX_POSTSCRIPT && cols < 0) ||
      (hextype != HEX_POSTSCRIPT && cols < 1) ||
      ((hextype == HEX_NORMAL || hextype == HEX_BITS || hextype == HEX_LITTLEENDIAN)
							    && (cols > COLS)))
    {
      fprintf(stderr, "%s: invalid number of columns (max. %d).\n", pname, COLS);
      exit(1);
    }

  если (octspergrp < 1 || octspergrp > cols)
    octspergrp = cols;
  иначе если (hextype == HEX_LITTLEENDIAN && (octspergrp & (octspergrp-1)))
    error_exit(1, "number of octets per group must be a power of 2 with -e.");

  если (argc > 3)
    exit_with_usage();

  если (argc == 1 || (argv[1][0] == '-' && !argv[1][1]))
    BIN_ASSIGN(fp = stdin, !revert);
  иначе
    {
      если ((fp = fopen(argv[1], BIN_READ(!revert))) == NULL)
	{
	  fprintf(stderr,"%s: ", pname);
	  perror(argv[1]);
	  вернуть 2;
	}
    }

  если (argc < 3 || (argv[2][0] == '-' && !argv[2][1]))
    BIN_ASSIGN(fpo = stdout, revert);
  иначе
    {
      целое fd;
      целое mode = revert ? O_WRONLY : (O_TRUNC|O_WRONLY);

      если (((fd = OPEN(argv[2], mode | BIN_CREAT(revert), 0666)) < 0) ||
	  (fpo = fdopen(fd, BIN_WRITE(revert))) == NULL)
	{
	  fprintf(stderr, "%s: ", pname);
	  perror(argv[2]);
	  вернуть 3;
	}
      rewind(fpo);

      /* Disable auto color when writing to a file. */
      если (!color_forced)
	color = 0;
    }

  если (revert)
    switch (hextype)
      {
      case HEX_NORMAL:
      case HEX_POSTSCRIPT:
      case HEX_BITS:
	вернуть huntype(fp, fpo, cols, hextype,
	  negseek ? -seekoff : seekoff);
	break;
      default:
	error_exit(-1, "Sorry, cannot revert this type of hexdump");
      }

  если (seekoff || negseek || !relseek)
    {
#ifdef TRY_SEEK
      если (relseek)
	e = fseek(fp, negseek ? -seekoff : seekoff, SEEK_CUR);
      иначе
	e = fseek(fp, negseek ? -seekoff : seekoff,
						negseek ? SEEK_END : SEEK_SET);
      если (e < 0 && negseek)
	error_exit(4, "Sorry, cannot seek.");
      если (e >= 0)
	seekoff = ftell(fp);
      иначе
#endif
	{
	  long s = seekoff;

	  пока (s--)
	    если (getc_or_die(fp) == EOF)
	    {
	      error_exit(4, "Sorry, cannot seek.");
	    }
	}
    }

  если (hextype & HEX_CINCLUDE)
    {
      /* A user-set variable name overrides fp == stdin */
      если (varname == NULL && fp != stdin)
	varname = argv[1];

      если (varname != NULL)
	{
	  FPRINTF_OR_DIE((fpo, "unsigned char %s", isdigit((unsigned char)varname[0]) ? "__" : ""));
	  для (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(isalnum((unsigned char)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  fputs_or_die("[] = {\n", fpo);
	}

      p = 0;
      пока ((length < 0 || p < length) && (((c = getc_or_die(fp)) != EOF) || termination))
	{
	  если (c == EOF)
	    {
	      c = 0;
	      termination = -1;
	    }
	  если (hextype & HEX_BITS)
	    {
	      если (p == 0)
		fputs_or_die("  ", fpo);
	      иначе если (p % cols == 0)
		fputs_or_die(",\n  ", fpo);
	      иначе
		fputs_or_die(", ", fpo);

	      FPRINTF_OR_DIE((fpo, "0b"));
	      для (целое j = 7; j >= 0; j--)
		putc_or_die((c & (1 << j)) ? '1' : '0', fpo);
	      p++;
	    }
	  иначе
	    {
	      FPRINTF_OR_DIE((fpo, (hexx == hexxa) ? "%s0x%02x" : "%s0X%02X",
		(p % cols) ? ", " : (!p ? "  " : ",\n  "), c));
	      p++;
	    }
	  если (termination == -1)
	    {
	      --p;
	      break ;
	    }
	}

      если (p)
	fputs_or_die("\n", fpo);

      если (varname != NULL)
	{
	  fputs_or_die("};\n", fpo);
	  FPRINTF_OR_DIE((fpo, "unsigned int %s", isdigit((unsigned char)varname[0]) ? "__" : ""));
	  для (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(isalnum((unsigned char)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  FPRINTF_OR_DIE((fpo, "_%s = %d;\n", capitalize ? "LEN" : "len", p));
	}

      fclose_or_die(fp, fpo);
      вернуть 0;
    }

  если (hextype == HEX_POSTSCRIPT)
    {
      p = cols;
      пока ((length < 0 || n < length) && (e = getc_or_die(fp)) != EOF)
	{
	  putc_or_die(hexx[(e >> 4) & 0xf], fpo);
	  putc_or_die(hexx[e & 0xf], fpo);
	  n++;
	  если (cols > 0 && !--p)
	    {
	      putc_or_die('\n', fpo);
	      p = cols;
	    }
	}
      если (cols == 0 || p < cols)
	putc_or_die('\n', fpo);
      fclose_or_die(fp, fpo);
      вернуть 0;
    }

  /* hextype: HEX_NORMAL or HEX_BITS or HEX_LITTLEENDIAN */

  если (hextype != HEX_BITS)
    {
      grplen = octspergrp + octspergrp + 1;	/* chars per octet group */
    }
  иначе	/* hextype == HEX_BITS */
    grplen = 8 * octspergrp + 1;

  пока ((length < 0 || n < length) && (e = getc_or_die(fp)) != EOF)
    {
      если (p == 0)
	{
	  addrlen = sprintf(l, decimal_offset ? "%08lu:" : "%08lx:",
				  ((unsigned long)(n + seekoff + displayoff)));
	  для (c = addrlen; c < LLEN_NO_COLOR; l[c++] = ' ')
	    ;
	}
      x = hextype == HEX_LITTLEENDIAN ? p ^ (octspergrp-1) : p;
      c = addrlen + 1 + (grplen * x) / octspergrp;
      если (hextype == HEX_NORMAL || hextype == HEX_LITTLEENDIAN)
	{
	  если (color)
	    {
	      cur_color = get_color_char(e, ebcdic);
	      colors[c] = cur_color;
	      colors[c+1] = cur_color;
	    }

	  l[c]   = hexx[(e >> 4) & 0xf];
	  l[++c] = hexx[e & 0xf];
	}
      иначе /* hextype == HEX_BITS */
	{
	  если (color)
	    cur_color = get_color_char(e, ebcdic);

	  для (i = 7; i >= 0; i--)
	    {
	      если (color)
		colors[c] = cur_color;
	      l[c++] = (e & (1 << i)) ? '1' : '0';
	    }
	}
      если (e)
	nonzero++;
      /* When changing this update definition of LLEN and LLEN_NO_COLOR above. */
      если (hextype == HEX_LITTLEENDIAN)
	/* last group will be fully used, round up */
	c = grplen * ((cols + octspergrp - 1) / octspergrp);
      иначе
	c = (grplen * cols - 1) / octspergrp;


      если (ebcdic)
	e = (e < 64) ? '.' : etoa64[e-64];

      если (hextype == HEX_LITTLEENDIAN)
	c -= 1;

      c += addrlen + 3 + p;
      если (color)
	  colors[c] = cur_color;
      l[c++] =
#если defined(__MVS__) && __CHARSET_LIB == 0
	  (e >= 64)
#иначе
	  (e > 31 && e < 127)
#endif
	  ? e : '.';
      n++;
      если (++p == cols)
	{
	  l[c++] = '\n';
	  l[c] = '\0';

	  xxdline(fpo, l, color ? colors : NULL, autoskip ? nonzero : 1);
	  memset(colors, 0, c);
	  nonzero = 0;
	  p = 0;
	}
    }
  если (p)
    {
      l[c++] = '\n';
      l[c] = '\0';
      если (color)
	{
	  x = p;
	  если (hextype == HEX_LITTLEENDIAN)
	    {
	      целое fill = octspergrp - (p % octspergrp);
	      если (fill == octspergrp) fill = 0;

	      c = addrlen + 1 + (grplen * (x - (octspergrp-fill))) / octspergrp;

	      для (i = 0; i < fill;i++)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		  x++;
		  p++;
		}
	    }

	  если (hextype != HEX_BITS)
	    {
	      c = addrlen + 1 + (grplen * x) / octspergrp;
	      c += cols - p;
	      c += (cols - p) / octspergrp;

	      для (i = cols - p; i > 0;i--)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		}
	    }
	  xxdline(fpo, l, colors, 1);
	}
      иначе
	xxdline(fpo, l, NULL, 1);
    }
  иначе если (autoskip)
    xxdline(fpo, l, color ? colors : NULL, -1);	/* last chance to flush out suppressed lines */

  fclose_or_die(fp, fpo);
  вернуть 0;
}

/* vi:set ts=8 sw=4 sts=2 cino+={2 cino+=n-2 : */
