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
#אם _MSC_VER >= 1400
# define _CRT_SECURE_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#endif
#אם !defined(CYGWIN) && defined(__CYGWIN__)
# define CYGWIN
#endif

#אם (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__)
# define _XOPEN_SOURCE 700   /* for fdopen() */
#endif

#ضمن <stdio.h>
#ifdef VAXC
# include <file.h>
#אחרת
# include <fcntl.h>
#endif
#אם defined(WIN32) || defined(CYGWIN)
# include <io.h>	/* for setmode() */
#endif
#ifdef WIN32
# include <windows.h>
#endif
#ifdef UNIX
# include <unistd.h>
#endif
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <ctype.h>
#ضمن <limits.h>
#אם __MWERKS__ && !defined(BEBOX)
# include <unix.h>	/* for fdopen() on MAC */
#endif


/*  This corrects the problem of missing prototypes for certain functions
 *  in some GNU installations (e.g. SunOS 4.1.x).
 *  Darren Hiebert <darren@hmi.com> (sparc-sun-sunos4.1.3_U1/2.7.2.2)
 */
#אם defined(__GNUC__) && defined(__STDC__)
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
# אם defined(sun) && defined(FILE) && !defined(__SVR4) && defined(__STDC__)
#  define __P(a) a
/* excerpt from my sun_stdlib.h */
خارجي صحيح fprintf __P((FILE *, حرف *, ...));
خارجي صحيح fputs   __P((حرف *, FILE *));
خارجي صحيح _flsbuf __P((unsigned حرف, FILE *));
خارجي صحيح _filbuf __P((FILE *));
خارجي صحيح fflush  __P((FILE *));
خارجي صحيح fclose  __P((FILE *));
خارجي صحيح fseek   __P((FILE *, long, صحيح));
خارجي صحيح rewind  __P((FILE *));

خارجي فراغ perror __P((حرف *));
# endif
#endif

حرف version[] = "xxd 2026-06-16 by Juergen Weigert et al.";
#ifdef WIN32
حرف osver[] = " (Win32)";
#אחרת
حرف osver[] = "";
#endif

#אם defined(WIN32)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "wt")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_BINARY) : O_CREAT)
# define BIN_ASSIGN(fp, yes) setmode(fileno(fp), (yes) ? O_BINARY : O_TEXT)
# define PATH_SEP '\\'
#وإلاإذا defined(CYGWIN)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "w")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_BINARY) : O_CREAT)
# define BIN_ASSIGN(fp, yes) ((yes) ? (فراغ) setmode(fileno(fp), O_BINARY) : (فراغ) (fp))
# define PATH_SEP '/'
#אחרת
# ifdef VMS
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ASSIGN(fp, dummy) fp
#  define PATH_SEP ']'
#  define FILE_SEP '.'
# אחרת
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ASSIGN(fp, dummy) fp
#  define PATH_SEP '/'
# endif
#endif

/* open has only to arguments on the Mac */
#אם __MWERKS__
# define OPEN(name, mode, umask) open(name, mode)
#אחרת
# define OPEN(name, mode, umask) open(name, mode, umask)
#endif

#ifdef AMIGA
# define STRNCMP(s1, s2, l) strncmp(s1, s2, (size_t)l)
#אחרת
# define STRNCMP(s1, s2, l) strncmp(s1, s2, l)
#endif

#ifndef __P
# אם defined(__STDC__) || defined(WIN32)
#  define __P(a) a
# אחרת
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

حرف hexxa[] = "0123456789abcdef0123456789ABCDEF", *hexx = hexxa;

/* the different hextypes known by this program: */
#define HEX_NORMAL         0x00 /* no flags set */
#define HEX_POSTSCRIPT     0x01
#define HEX_CINCLUDE       0x02
#define HEX_BITS           0x04 /* not hex a dump, but bits: 01111001 */
#define HEX_LITTLEENDIAN   0x08

#define CONDITIONAL_CAPITALIZE(c) (capitalize ? toupper((unsigned حرف)(c)) : (c))

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

ساكن حرف *pname;

  ساكن فراغ
exit_with_usage(فراغ)
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
#אחרת
	  "", "");
#endif
  fprintf(stderr, "    -u          use upper case hex letters.\n"
		  "    -R when     colorize the output; <when> can be 'always', 'auto' or 'never'. Default: 'auto'.\n"
		  "    -v          show version: \"%s%s\".\n", version, osver);
  exit(1);
}

  ساكن فراغ
perror_exit(صحيح ret)
{
  fprintf(stderr, "%s: ", pname);
  perror(عدم);
  exit(ret);
}

  ساكن فراغ
error_exit(صحيح ret, حرف *msg)
{
  fprintf(stderr, "%s: %s\n", pname, msg);
  exit(ret);
}

  ساكن صحيح
getc_or_die(FILE *fpi)
{
  صحيح c = getc(fpi);
  אם (c == EOF && ferror(fpi))
    perror_exit(2);
  החזר c;
}

  ساكن فراغ
putc_or_die(صحيح c, FILE *fpo)
{
  אם (putc(c, fpo) == EOF)
    perror_exit(3);
}

  ساكن فراغ
fputs_or_die(حرف *s, FILE *fpo)
{
  אם (fputs(s, fpo) == EOF)
    perror_exit(3);
}

/* Use a macro to allow for different arguments. */
#define FPRINTF_OR_DIE(args) אם (fprintf args < 0) perror_exit(3)

  ساكن فراغ
fclose_or_die(FILE *fpi, FILE *fpo)
{
  אם (fclose(fpo) != 0)
    perror_exit(3);
  אם (fclose(fpi) != 0)
    perror_exit(2);
}

/*
 * If "c" is a hex digit, return the value.
 * Otherwise return -1.
 */
  ساكن صحيح
parse_hex_digit(صحيح c)
{
  החזר (c >= '0' && c <= '9') ? c - '0'
	: (c >= 'a' && c <= 'f') ? c - 'a' + 10
	: (c >= 'A' && c <= 'F') ? c - 'A' + 10
	: -1;
}

/*
 * If "c" is a bin digit, return the value.
 * Otherwise return -1.
 */
  ساكن صحيح
parse_bin_digit(صحيح c)
{
  החזר (c >= '0' && c <= '1') ? c - '0'
	: -1;
}

/*
 * Ignore text on "fpi" until end-of-line or end-of-file.
 * Return the '\n' or EOF character.
 * When an error is encountered exit with an error message.
 */
  ساكن صحيح
skip_to_eol(FILE *fpi, صحيح c)
{
  כלעוד (c != '\n' && c != EOF)
    c = getc_or_die(fpi);
  החזר c;
}

/*
 * Max. cols binary characters are decoded from the input stream per line.
 * Two adjacent garbage characters after evaluated data delimit valid data.
 * Everything up to the next newline is discarded.
 *
 * The name is historic and came from 'undo type opt h'.
 */
  ساكن صحيح
huntype(
  FILE *fpi,
  FILE *fpo,
  صحيح cols,
  صحيح hextype,
  long base_off)
{
  صحيح c, ign_garb = 1, n1 = -1, n2 = 0, n3 = 0, p = cols, bt = 0, b = 0, bcnt = 0;
  long have_off = 0, want_off = 0;

  rewind(fpi);

  כלעוד ((c = getc(fpi)) != EOF)
    {
      אם (c == '\r')	/* Doze style input file? */
	استمر;

      /* Allow multiple spaces.  This doesn't work when there is normal text
       * after the hex codes in the last line that looks like hex, thus only
       * use it for PostScript format. */
      אם (hextype == HEX_POSTSCRIPT && (c == ' ' || c == '\n' || c == '\t'))
	استمر;

      אם (hextype == HEX_NORMAL || hextype == HEX_POSTSCRIPT)
	{
	  n3 = n2;
	  n2 = n1;

	  n1 = parse_hex_digit(c);
	  אם (n1 == -1 && ign_garb)
	    استمر;
	}
      אחרת /* HEX_BITS */
	{
	  n1 = parse_hex_digit(c);
	  אם (n1 == -1 && ign_garb)
	    استمر;

	  bt = parse_bin_digit(c);
	  אם (bt != -1)
	    {
	      /* shift via unsigned to avoid signed overflow on bad input */
	      b = (صحيح)(((unsigned)b << 1) | (unsigned)bt);
	      ++bcnt;
	    }
	}

      ign_garb = 0;

      אם ((hextype != HEX_POSTSCRIPT) && (p >= cols))
	{
	  אם (hextype == HEX_NORMAL)
	    {
	      אם (n1 < 0)
		{
		  p = 0;
		  استمر;
		}
	      want_off = (long)(((unsigned long)want_off << 4) | (unsigned)n1);
	    }
	  אחרת /* HEX_BITS */
	    {
	      אם (n1 < 0)
		{
		  p = 0;
		  bcnt = 0;
		  استمر;
		}
	      want_off = (long)(((unsigned long)want_off << 4) | (unsigned)n1);
	    }
	  استمر;
	}

      אם (base_off + want_off != have_off)
	{
	  אם (fflush(fpo) != 0)
	    perror_exit(3);
#ifdef TRY_SEEK
	  אם (fseek(fpo, base_off + want_off - have_off, SEEK_CUR) >= 0)
	    have_off = base_off + want_off;
#endif
	  אם (base_off + want_off < have_off)
	    error_exit(5, "Sorry, cannot seek backwards.");
	  עבור (; have_off < base_off + want_off; have_off++)
	    putc_or_die(0, fpo);
	}

      אם (hextype == HEX_NORMAL || hextype == HEX_POSTSCRIPT)
	{
	  אם (n2 >= 0 && n1 >= 0)
	    {
	      putc_or_die((n2 << 4) | n1, fpo);
	      have_off++;
	      want_off++;
	      n1 = -1;
	      אם (!hextype && (++p >= cols))
	      /* skip the rest of the line as garbage */
	      c = skip_to_eol(fpi, c);
	    }
	  אחרת אם (n1 < 0 && n2 < 0 && n3 < 0)
	    /* already stumbled into garbage, skip line, wait and see */
	    c = skip_to_eol(fpi, c);
	}
      אחרת /* HEX_BITS */
	{
	  אם (bcnt == 8)
	    {
	      putc_or_die(b, fpo);
	      have_off++;
	      want_off++;
	      b = 0;
	      bcnt = 0;
	      אם (++p >= cols)
		/* skip the rest of the line as garbage */
		 c = skip_to_eol(fpi, c);
	    }
	}

      אם (c == '\n')
	{
	  אם (hextype == HEX_NORMAL || hextype == HEX_BITS)
	    want_off = 0;
	  p = cols;
	  ign_garb = 1;
	}
    }
  אם (fflush(fpo) != 0)
    perror_exit(3);
#ifdef TRY_SEEK
  fseek(fpo, 0L, SEEK_END);
#endif
  fclose_or_die(fpi, fpo);
  החזר 0;
}


/*
 * Print line l with given colors.
 */
  ساكن فراغ
print_colored_line(FILE *fp, حرف *l, حرف *colors)
{
  ساكن حرف l_colored[LLEN+1];

  אם (colors)
    {
      صحيح c = 0;
      אם (colors[0])
	{
	  COLOR_PROLOGUE(colors[0])
	}
      l_colored[c++] = l[0];
      صحيح i;
      עבור (i = 1; l[i]; i++)
	{
	  אם (colors[i] != colors[i-1])
	    {
	      אם (colors[i-1])
		{
		  COLOR_EPILOGUE
		}
	      אם (colors[i])
		{
		  COLOR_PROLOGUE(colors[i])
		}
	    }
	  l_colored[c++] = l[i];
	}

      אם (colors[i])
	{
	  COLOR_EPILOGUE
	}
      l_colored[c++] = '\0';

      fputs_or_die(l_colored, fp);
    }
  אחרת
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
  ساكن فراغ
xxdline(FILE *fp, حرف *l, حرف *colors, صحيح nz)
{
  ساكن حرف z[LLEN_NO_COLOR+1];
  ساكن حرف z_colors[LLEN_NO_COLOR+1];
  ساكن signed حرف zero_seen = 0;

  אם (!nz && zero_seen == 1)
    {
      strcpy(z, l);
      אם (colors)
	  memcpy(z_colors, colors, strlen(z));
    }

  אם (nz || !zero_seen++)
    {
      אם (nz)
	{
	  אם (nz < 0)
	    zero_seen--;
	  אם (zero_seen == 2)
	    print_colored_line(fp, z, z_colors);
	  אם (zero_seen > 2)
	    fputs_or_die("*\n", fp);
	}
      אם (nz >= 0 || zero_seen > 0)
	print_colored_line(fp, l, colors);

      אם (nz)
	zero_seen = 0;
    }

  /* If zero_seen > 3, then its exact value doesn't matter, so long as it
   * remains >3 and incrementing it will not cause overflow. */
  אם (zero_seen >= 0x7F)
    zero_seen = 4;
}

/* This is an EBCDIC to ASCII conversion table */
/* from a proposed BTL standard April 16, 1979 */
ساكن unsigned حرف etoa64[] =
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

  ساكن حرف
get_color_char (صحيح e, صحيح ebcdic)
{
  אם (ebcdic)
    {
      אם ((e >= 75 && e <= 80) || (e >= 90 && e <= 97) ||
	  (e >= 107 && e <= 111) || (e >= 121 && e <= 127) ||
	  (e >= 129 && e <= 137) || (e >= 145 && e <= 154) ||
	  (e >= 162 && e <= 169) || (e >= 192 && e <= 201) ||
	  (e >= 208 && e <= 217) || (e >= 226 && e <= 233) ||
	  (e >= 240 && e <= 249) || (e == 189) || (e == 64) ||
	  (e == 173) || (e == 224) )
	החזר COLOR_GREEN;

      אחרת אם (e == 37 || e == 13 || e == 5)
	החזר COLOR_YELLOW;
      אחרת אם (e == 0)
	החזר COLOR_WHITE;
      אחרת אם (e == 255)
	החזר COLOR_BLUE;
      אחרת
	החזר COLOR_RED;
    }
  אחרת  /* ASCII */
    {
#אם defined(__MVS__) && __CHARSET_LIB == 0
      אם (e >= 64)
	החזר COLOR_GREEN;
#אחרת
      אם (e > 31 && e < 127)
	החזר COLOR_GREEN;
#endif

      אחרת אם (e == 9 || e == 10 || e == 13)
	החזר COLOR_YELLOW;
      אחרת אם (e == 0)
	החזר COLOR_WHITE;
      אחרת אם (e == 255)
	החזר COLOR_BLUE;
      אחרת
	החזר COLOR_RED;
    }
    החזר 0;
}

  ساكن صحيح
enable_color(فراغ)
{
#ifdef WIN32
  DWORD   mode;
  HANDLE  out;

  אם (!isatty(1))
    החזר 0;

  out = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleMode(out, &mode);
  mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  החזר (صحيح)SetConsoleMode(out, mode);
#وإلاإذا defined(UNIX)
  حرف *term;

  אם (!isatty(STDOUT_FILENO))
    החזר 0;

  term = getenv("TERM");
  אם (term == عدم || *term == '\0' || !strcmp(term, "dumb"))
    החזר 0;

  החזר 1;
#אחרת
  החזר 0;
#endif
}

  صحيح
main(صحيح argc, حرف *argv[])
{
  FILE *fp, *fpo;
  صحيح c, e, p = 0, relseek = 1, negseek = 0, revert = 0, i, x;
  صحيح cols = 0, colsgiven = 0, nonzero = 0, autoskip = 0, hextype = HEX_NORMAL;
  صحيح capitalize = 0, decimal_offset = 0;
  صحيح ebcdic = 0;
  صحيح termination = 0;
  صحيح octspergrp = -1;	/* number of octets grouped in output */
  صحيح grplen;		/* total chars per octet group excluding colors */
  long length = -1, n = 0, seekoff = 0;
  unsigned long displayoff = 0;
  ساكن حرف l[LLEN_NO_COLOR+1];  /* static because it may be too big for stack */
  ساكن حرف colors[LLEN_NO_COLOR+1]; /* color array */
  حرف *pp;
  حرف *varname = عدم;
  صحيح addrlen = 9;
  صحيح color = 0;
  صحيح color_forced = 0;	/* set when -R always is used */
  حرف *no_color;
  حرف cur_color = 0;

  no_color = getenv("NO_COLOR");
  אם (no_color == عدم || no_color[0] == '\0')
    color = enable_color();

#ifdef AMIGA
  /* This program doesn't work when started from the Workbench */
  אם (argc == 0)
    exit(1);
#endif

  pname = argv[0];
  עבור (pp = pname; *pp; )
    אם (*pp++ == PATH_SEP)
      pname = pp;
#ifdef FILE_SEP
  עבור (pp = pname; *pp; pp++)
    אם (*pp == FILE_SEP)
      {
	*pp = '\0';
	اكسر;
      }
#endif

  כלעוד (argc >= 2)
    {
      pp = argv[1] + (!STRNCMP(argv[1], "--", 2) && argv[1][2]);
	   אם (!STRNCMP(pp, "-a", 2)) autoskip = 1 - autoskip;
      אחרת אם (!STRNCMP(pp, "-b", 2)) hextype |= HEX_BITS;
      אחרת אם (!STRNCMP(pp, "-e", 2)) hextype |= HEX_LITTLEENDIAN;
      אחרת אם (!STRNCMP(pp, "-u", 2)) hexx = hexxa + 16;
      אחרת אם (!STRNCMP(pp, "-p", 2)) hextype |= HEX_POSTSCRIPT;
      אחרת אם (!STRNCMP(pp, "-i", 2)) hextype |= HEX_CINCLUDE;
      אחרת אם (!STRNCMP(pp, "-C", 2)) capitalize = 1;
      אחרת אם (!STRNCMP(pp, "-d", 2)) decimal_offset = 1;
      אחרת אם (!STRNCMP(pp, "-r", 2)) revert++;
      אחרת אם (!STRNCMP(pp, "-E", 2)) ebcdic++;
      אחרת אם (!STRNCMP(pp, "-t", 2)) termination++;
      אחרת אם (!STRNCMP(pp, "-v", 2))
	{
	  fprintf(stderr, "%s%s\n", version, osver);
	  exit(0);
	}
      אחרת אם (!STRNCMP(pp, "-c", 2))
	{
	  אם (pp[2] && !STRNCMP("apitalize", pp + 2, 9))
	    capitalize = 1;
	  אחרת אם (pp[2] && STRNCMP("ols", pp + 2, 3))
	    {
	      colsgiven = 1;
	      cols = (صحيح)strtol(pp + 2, عدم, 0);
	    }
	  אחרת
	    {
	      אם (!argv[2])
		exit_with_usage();
	      colsgiven = 1;
	      cols = (صحيح)strtol(argv[2], عدم, 0);
	      argv++;
	      argc--;
	    }
	}
      אחרת אם (!STRNCMP(pp, "-g", 2))
	{
	  אם (pp[2] && STRNCMP("roup", pp + 2, 4))
	    octspergrp = (صحيح)strtol(pp + 2, عدم, 0);
	  אחרת
	    {
	      אם (!argv[2])
		exit_with_usage();
	      octspergrp = (صحيح)strtol(argv[2], عدم, 0);
	      argv++;
	      argc--;
	    }
	}
      אחרת אם (!STRNCMP(pp, "-o", 2))
	{
	  صحيح reloffset = 0;
	  صحيح negoffset = 0;
	  אם (pp[2] && STRNCMP("ffset", pp + 2, 5))
	    displayoff = strtoul(pp + 2, عدم, 0);
	  אחרת
	    {
	      אם (!argv[2])
		exit_with_usage();

	      אם (argv[2][0] == '+')
	       reloffset++;
	     אם (argv[2][reloffset] == '-')
	       negoffset++;

	     אם (negoffset)
	       displayoff = ULONG_MAX - strtoul(argv[2] + reloffset+negoffset, عدم, 0) + 1;
	     אחרת
	       displayoff = strtoul(argv[2] + reloffset+negoffset, عدم, 0);

	      argv++;
	      argc--;
	    }
	}
      אחרת אם (!STRNCMP(pp, "-s", 2))
	{
	  relseek = 0;
	  negseek = 0;
	  אם (pp[2] && STRNCMP("kip", pp+2, 3) && STRNCMP("eek", pp+2, 3))
	    {
#ifdef TRY_SEEK
	      אם (pp[2] == '+')
		relseek++;
	      אם (pp[2+relseek] == '-')
		negseek++;
#endif
	      seekoff = strtol(pp + 2+relseek+negseek, (حرف **)عدم, 0);
	    }
	  אחרת
	    {
	      אם (!argv[2])
		exit_with_usage();
#ifdef TRY_SEEK
	      אם (argv[2][0] == '+')
		relseek++;
	      אם (argv[2][relseek] == '-')
		negseek++;
#endif
	      seekoff = strtol(argv[2] + relseek+negseek, (حرف **)عدم, 0);
	      argv++;
	      argc--;
	    }
	}
      אחרת אם (!STRNCMP(pp, "-l", 2))
	{
	  אם (pp[2] && STRNCMP("en", pp + 2, 2))
	    length = strtol(pp + 2, (حرف **)عدم, 0);
	  אחרת
	    {
	      אם (!argv[2])
		exit_with_usage();
	      length = strtol(argv[2], (حرف **)عدم, 0);
	      argv++;
	      argc--;
	    }
	}
      אחרת אם (!STRNCMP(pp, "-n", 2))
	{
	  אם (pp[2] && STRNCMP("ame", pp + 2, 3))
	    varname = pp + 2;
	  אחרת
	    {
	      אם (!argv[2])
		exit_with_usage();
	      varname = argv[2];
	      argv++;
	      argc--;
	    }
	}
      אחרת אם (!STRNCMP(pp, "-R", 2))
	{
	  حرف *pw = pp + 2;
	  אם (!pw[0])
	    {
	      pw = argv[2];
	      argv++;
	      argc--;
	    }
	  אם (!pw)
	    exit_with_usage();
	  אם (!STRNCMP(pw, "always", 6))
	    {
	      (فراغ)enable_color();
	      color = 1;
	      color_forced = 1;
	    }
	  אחרת אם (!STRNCMP(pw, "never", 5))
	    color = 0;
	  אחרת אם (!STRNCMP(pw, "auto", 4))
	    color = enable_color();
	  אחרת
	    exit_with_usage();
	}
      אחרת אם (!strcmp(argv[1], "--"))	/* end of options */
	{
	  argv++;
	  argc--;
	  اكسر;
	}
      אחרת אם (pp[0] == '-' && pp[1])	/* unknown option */
	exit_with_usage();
      אחרת
	اكسر;				/* not an option */

      argv++;				/* advance to next argument */
      argc--;
    }

  אם (hextype != (HEX_CINCLUDE | HEX_BITS))
    {
	/* Allow at most one bit to be set in hextype */
	אם (hextype & (hextype - 1))
	    error_exit(1, "only one of -b, -e, -u, -p, -i can be used");
    }

  אם (!colsgiven || (!cols && hextype != HEX_POSTSCRIPT))
    بدل (hextype)
      {
      حالة HEX_POSTSCRIPT:	cols = 30; اكسر;
      حالة HEX_CINCLUDE:	cols = 12; اكسر;
      حالة HEX_CINCLUDE | HEX_BITS:
      حالة HEX_BITS:		cols = 6; اكسر;
      حالة HEX_NORMAL:
      حالة HEX_LITTLEENDIAN:
      افتراضي:			cols = 16; اكسر;
      }

  אם (octspergrp < 0)
    بدل (hextype)
      {
      حالة HEX_CINCLUDE | HEX_BITS:
      حالة HEX_BITS:		octspergrp = 1; اكسر;
      حالة HEX_NORMAL:		octspergrp = 2; اكسر;
      حالة HEX_LITTLEENDIAN:	octspergrp = 4; اكسر;
      حالة HEX_POSTSCRIPT:
      حالة HEX_CINCLUDE:
      افتراضي:			octspergrp = 0; اكسر;
      }

  אם ((hextype == HEX_POSTSCRIPT && cols < 0) ||
      (hextype != HEX_POSTSCRIPT && cols < 1) ||
      ((hextype == HEX_NORMAL || hextype == HEX_BITS || hextype == HEX_LITTLEENDIAN)
							    && (cols > COLS)))
    {
      fprintf(stderr, "%s: invalid number of columns (max. %d).\n", pname, COLS);
      exit(1);
    }

  אם (octspergrp < 1 || octspergrp > cols)
    octspergrp = cols;
  אחרת אם (hextype == HEX_LITTLEENDIAN && (octspergrp & (octspergrp-1)))
    error_exit(1, "number of octets per group must be a power of 2 with -e.");

  אם (argc > 3)
    exit_with_usage();

  אם (argc == 1 || (argv[1][0] == '-' && !argv[1][1]))
    BIN_ASSIGN(fp = stdin, !revert);
  אחרת
    {
      אם ((fp = fopen(argv[1], BIN_READ(!revert))) == عدم)
	{
	  fprintf(stderr,"%s: ", pname);
	  perror(argv[1]);
	  החזר 2;
	}
    }

  אם (argc < 3 || (argv[2][0] == '-' && !argv[2][1]))
    BIN_ASSIGN(fpo = stdout, revert);
  אחרת
    {
      صحيح fd;
      صحيح mode = revert ? O_WRONLY : (O_TRUNC|O_WRONLY);

      אם (((fd = OPEN(argv[2], mode | BIN_CREAT(revert), 0666)) < 0) ||
	  (fpo = fdopen(fd, BIN_WRITE(revert))) == عدم)
	{
	  fprintf(stderr, "%s: ", pname);
	  perror(argv[2]);
	  החזר 3;
	}
      rewind(fpo);

      /* Disable auto color when writing to a file. */
      אם (!color_forced)
	color = 0;
    }

  אם (revert)
    بدل (hextype)
      {
      حالة HEX_NORMAL:
      حالة HEX_POSTSCRIPT:
      حالة HEX_BITS:
	החזר huntype(fp, fpo, cols, hextype,
	  negseek ? -seekoff : seekoff);
	اكسر;
      افتراضي:
	error_exit(-1, "Sorry, cannot revert this type of hexdump");
      }

  אם (seekoff || negseek || !relseek)
    {
#ifdef TRY_SEEK
      אם (relseek)
	e = fseek(fp, negseek ? -seekoff : seekoff, SEEK_CUR);
      אחרת
	e = fseek(fp, negseek ? -seekoff : seekoff,
						negseek ? SEEK_END : SEEK_SET);
      אם (e < 0 && negseek)
	error_exit(4, "Sorry, cannot seek.");
      אם (e >= 0)
	seekoff = ftell(fp);
      אחרת
#endif
	{
	  long s = seekoff;

	  כלעוד (s--)
	    אם (getc_or_die(fp) == EOF)
	    {
	      error_exit(4, "Sorry, cannot seek.");
	    }
	}
    }

  אם (hextype & HEX_CINCLUDE)
    {
      /* A user-set variable name overrides fp == stdin */
      אם (varname == عدم && fp != stdin)
	varname = argv[1];

      אם (varname != عدم)
	{
	  FPRINTF_OR_DIE((fpo, "unsigned char %s", isdigit((unsigned حرف)varname[0]) ? "__" : ""));
	  עבור (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(isalnum((unsigned حرف)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  fputs_or_die("[] = {\n", fpo);
	}

      p = 0;
      כלעוד ((length < 0 || p < length) && (((c = getc_or_die(fp)) != EOF) || termination))
	{
	  אם (c == EOF)
	    {
	      c = 0;
	      termination = -1;
	    }
	  אם (hextype & HEX_BITS)
	    {
	      אם (p == 0)
		fputs_or_die("  ", fpo);
	      אחרת אם (p % cols == 0)
		fputs_or_die(",\n  ", fpo);
	      אחרת
		fputs_or_die(", ", fpo);

	      FPRINTF_OR_DIE((fpo, "0b"));
	      עבור (صحيح j = 7; j >= 0; j--)
		putc_or_die((c & (1 << j)) ? '1' : '0', fpo);
	      p++;
	    }
	  אחרת
	    {
	      FPRINTF_OR_DIE((fpo, (hexx == hexxa) ? "%s0x%02x" : "%s0X%02X",
		(p % cols) ? ", " : (!p ? "  " : ",\n  "), c));
	      p++;
	    }
	  אם (termination == -1)
	    {
	      --p;
	      اكسر ;
	    }
	}

      אם (p)
	fputs_or_die("\n", fpo);

      אם (varname != عدم)
	{
	  fputs_or_die("};\n", fpo);
	  FPRINTF_OR_DIE((fpo, "unsigned int %s", isdigit((unsigned حرف)varname[0]) ? "__" : ""));
	  עבור (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(isalnum((unsigned حرف)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  FPRINTF_OR_DIE((fpo, "_%s = %d;\n", capitalize ? "LEN" : "len", p));
	}

      fclose_or_die(fp, fpo);
      החזר 0;
    }

  אם (hextype == HEX_POSTSCRIPT)
    {
      p = cols;
      כלעוד ((length < 0 || n < length) && (e = getc_or_die(fp)) != EOF)
	{
	  putc_or_die(hexx[(e >> 4) & 0xf], fpo);
	  putc_or_die(hexx[e & 0xf], fpo);
	  n++;
	  אם (cols > 0 && !--p)
	    {
	      putc_or_die('\n', fpo);
	      p = cols;
	    }
	}
      אם (cols == 0 || p < cols)
	putc_or_die('\n', fpo);
      fclose_or_die(fp, fpo);
      החזר 0;
    }

  /* hextype: HEX_NORMAL or HEX_BITS or HEX_LITTLEENDIAN */

  אם (hextype != HEX_BITS)
    {
      grplen = octspergrp + octspergrp + 1;	/* chars per octet group */
    }
  אחרת	/* hextype == HEX_BITS */
    grplen = 8 * octspergrp + 1;

  כלעוד ((length < 0 || n < length) && (e = getc_or_die(fp)) != EOF)
    {
      אם (p == 0)
	{
	  addrlen = sprintf(l, decimal_offset ? "%08lu:" : "%08lx:",
				  ((unsigned long)(n + seekoff + displayoff)));
	  עבור (c = addrlen; c < LLEN_NO_COLOR; l[c++] = ' ')
	    ;
	}
      x = hextype == HEX_LITTLEENDIAN ? p ^ (octspergrp-1) : p;
      c = addrlen + 1 + (grplen * x) / octspergrp;
      אם (hextype == HEX_NORMAL || hextype == HEX_LITTLEENDIAN)
	{
	  אם (color)
	    {
	      cur_color = get_color_char(e, ebcdic);
	      colors[c] = cur_color;
	      colors[c+1] = cur_color;
	    }

	  l[c]   = hexx[(e >> 4) & 0xf];
	  l[++c] = hexx[e & 0xf];
	}
      אחרת /* hextype == HEX_BITS */
	{
	  אם (color)
	    cur_color = get_color_char(e, ebcdic);

	  עבור (i = 7; i >= 0; i--)
	    {
	      אם (color)
		colors[c] = cur_color;
	      l[c++] = (e & (1 << i)) ? '1' : '0';
	    }
	}
      אם (e)
	nonzero++;
      /* When changing this update definition of LLEN and LLEN_NO_COLOR above. */
      אם (hextype == HEX_LITTLEENDIAN)
	/* last group will be fully used, round up */
	c = grplen * ((cols + octspergrp - 1) / octspergrp);
      אחרת
	c = (grplen * cols - 1) / octspergrp;


      אם (ebcdic)
	e = (e < 64) ? '.' : etoa64[e-64];

      אם (hextype == HEX_LITTLEENDIAN)
	c -= 1;

      c += addrlen + 3 + p;
      אם (color)
	  colors[c] = cur_color;
      l[c++] =
#אם defined(__MVS__) && __CHARSET_LIB == 0
	  (e >= 64)
#אחרת
	  (e > 31 && e < 127)
#endif
	  ? e : '.';
      n++;
      אם (++p == cols)
	{
	  l[c++] = '\n';
	  l[c] = '\0';

	  xxdline(fpo, l, color ? colors : عدم, autoskip ? nonzero : 1);
	  memset(colors, 0, c);
	  nonzero = 0;
	  p = 0;
	}
    }
  אם (p)
    {
      l[c++] = '\n';
      l[c] = '\0';
      אם (color)
	{
	  x = p;
	  אם (hextype == HEX_LITTLEENDIAN)
	    {
	      صحيح fill = octspergrp - (p % octspergrp);
	      אם (fill == octspergrp) fill = 0;

	      c = addrlen + 1 + (grplen * (x - (octspergrp-fill))) / octspergrp;

	      עבור (i = 0; i < fill;i++)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		  x++;
		  p++;
		}
	    }

	  אם (hextype != HEX_BITS)
	    {
	      c = addrlen + 1 + (grplen * x) / octspergrp;
	      c += cols - p;
	      c += (cols - p) / octspergrp;

	      עבור (i = cols - p; i > 0;i--)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		}
	    }
	  xxdline(fpo, l, colors, 1);
	}
      אחרת
	xxdline(fpo, l, عدم, 1);
    }
  אחרת אם (autoskip)
    xxdline(fpo, l, color ? colors : عدم, -1);	/* last chance to flush out suppressed lines */

  fclose_or_die(fp, fpo);
  החזר 0;
}

/* vi:set ts=8 sw=4 sts=2 cino+={2 cino+=n-2 : */
