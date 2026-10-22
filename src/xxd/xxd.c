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
#ఒకవేళ _MSC_VER >= 1400
# define _CRT_SECURE_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#అయితే_ముగింపు
#ఒకవేళ !defined(CYGWIN) && defined(__CYGWIN__)
# define CYGWIN
#అయితే_ముగింపు

#ఒకవేళ (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__)
# define XOPEN_ఆధారం 700   /* for fdopen() */
#అయితే_ముగింపు

#అన్తర్భావ <stdio.h>
#నిర్వచితమైతే VAXC
# include <file.h>
#లేకపోతే
# include <fcntl.h>
#అయితే_ముగింపు
#ఒకవేళ defined(WIN32) || defined(CYGWIN)
# include <io.h>	/* for setmode() */
#అయితే_ముగింపు
#నిర్వచితమైతే WIN32
# include <windows.h>
#అయితే_ముగింపు
#నిర్వచితమైతే UNIX
# include <unistd.h>
#అయితే_ముగింపు
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <ctype.h>
#అన్తర్భావ <limits.h>
#ఒకవేళ __MWERKS__ && !defined(BEBOX)
# include <unix.h>	/* for fdopen() on MAC */
#అయితే_ముగింపు


/*  This corrects the problem of missing prototypes for certain functions
 *  in some GNU installations (e.g. SunOS 4.1.x).
 *  Darren Hiebert <darren@hmi.com> (sparc-sun-sunos4.1.3_U1/2.7.2.2)
 */
#ఒకవేళ defined(__GNUC__) && defined(__STDC__)
# ifndef __USE_FIXED_PROTOTYPES__
#  define __USE_FIXED_PROTOTYPES__
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే __USE_FIXED_PROTOTYPES__
/*
 * This is historic and works only if the compiler really has no prototypes:
 *
 * Include prototypes for Sun OS 4.x, when using an ANSI compiler.
 * FILE is defined on OS 4.x, not on 5.x (Solaris).
 * if __SVR4 is defined (some Solaris versions), don't include this.
 */
# ఒకవేళ defined(sun) && defined(FILE) && !defined(__SVR4) && defined(__STDC__)
#  define __P(a) a
/* excerpt from my sun_stdlib.h */
బాహ్య పూర్ణాంకం fprintf __P((FILE *, అక్షరం *, ...));
బాహ్య పూర్ణాంకం fputs   __P((అక్షరం *, FILE *));
బాహ్య పూర్ణాంకం _flsbuf __P((గుర్తులేని అక్షరం, FILE *));
బాహ్య పూర్ణాంకం _filbuf __P((FILE *));
బాహ్య పూర్ణాంకం fflush  __P((FILE *));
బాహ్య పూర్ణాంకం fclose  __P((FILE *));
బాహ్య పూర్ణాంకం fseek   __P((FILE *, దీర్ఘ, పూర్ణాంకం));
బాహ్య పూర్ణాంకం rewind  __P((FILE *));

బాహ్య శూన్య perror __P((అక్షరం *));
# endif
#అయితే_ముగింపు

అక్షరం సంస్కరణ_2[] = "xxd 2026-06-16 by Juergen Weigert et al.";
#నిర్వచితమైతే WIN32
అక్షరం osver[] = " (Win32)";
#లేకపోతే
అక్షరం osver[] = "";
#అయితే_ముగింపు

#ఒకవేళ defined(WIN32)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "wt")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_ద్విమాన) : O_CREAT)
# define BIN_కేటాయించు(fp, yes) setmode(fileno(fp), (yes) ? O_ద్విమాన : O_పాఠ్యం_2)
# define మార్గం_SEP '\\'
#లేదావేళ defined(CYGWIN)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "w")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_ద్విమాన) : O_CREAT)
# define BIN_కేటాయించు(fp, yes) ((yes) ? (శూన్య) setmode(fileno(fp), O_ద్విమాన) : (శూన్య) (fp))
# define మార్గం_SEP '/'
#లేకపోతే
# ifdef VMS
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_కేటాయించు(fp, dummy) fp
#  define మార్గం_SEP ']'
#  define FILE_SEP '.'
# లేకపోతే
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_కేటాయించు(fp, dummy) fp
#  define మార్గం_SEP '/'
# endif
#అయితే_ముగింపు

/* open has only to arguments on the Mac */
#ఒకవేళ __MWERKS__
# define OPEN(name, బహుళకం, umask) open(name, బహుళకం)
#లేకపోతే
# define OPEN(name, బహుళకం, umask) open(name, బహుళకం, umask)
#అయితే_ముగింపు

#నిర్వచితమైతే AMIGA
# define STRNCMP(s1, s2, l) అక్షరశ్రేణి_పోలిక(s1, s2, (size_t)l)
#లేకపోతే
# define STRNCMP(s1, s2, l) అక్షరశ్రేణి_పోలిక(s1, s2, l)
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే __P
# ఒకవేళ defined(__STDC__) || defined(WIN32)
#  define __P(a) a
# లేకపోతే
#  define __P(a) ()
# endif
#అయితే_ముగింపు

#నిర్వచించు TRY_SEEK	/* attempt to use lseek, or skip forward by reading */
#నిర్వచించు నిలువుల_సంఖ్య_2 256	/* change here, if you ever need more columns */

/*
 * LLEN is the maximum length of a line; other than the visible characters
 * we need to consider also the escape color sequence prologue/epilogue ,
 * (11 bytes for each character).
 */
#నిర్వచించు LLEN \
    (39            /* addr: ⌈log10(ULONG_MAX)⌉ if "-d" flag given. We assume ULONG_MAX = 2**128 */ \
    + 2            /* ": " */ \
    + 13 * నిలువుల_సంఖ్య_2    /* hex dump with colors */ \
    + (నిలువుల_సంఖ్య_2 - 1)   /* whitespace between groups if "-g1" option given and "-c" maxed out */ \
    + 2            /* whitespace */ \
    + 12 * నిలువుల_సంఖ్య_2    /* ASCII dump with colors */ \
    + 2)           /* "\n\0" */

/*
 * LLEN_NO_COLOR is the maximum length of a line excluding the colors.
 */
#నిర్వచించు LLEN_NO_COLOR \
    (39            /* addr: ⌈log10(ULONG_MAX)⌉ if "-d" flag given. We assume ULONG_MAX = 2**128 */ \
    + 2            /* ": " */ \
    + 9 * నిలువుల_సంఖ్య_2     /* hex dump, worst case: bitwise output using -b */ \
    + 2            /* whitespace */ \
    + నిలువుల_సంఖ్య_2         /* ASCII dump */ \
    + 2)           /* "\n\0" */

అక్షరం hexxa[] = "0123456789abcdef0123456789ABCDEF", *hexx = hexxa;

/* the different hextypes known by this program: */
#నిర్వచించు HEX_సాధారణ         0x00 /* no flags set */
#నిర్వచించు HEX_POSTSCRIPT     0x01
#నిర్వచించు HEX_CINCLUDE       0x02
#నిర్వచించు HEX_బిట్లు           0x04 /* not hex a dump, but bits: 01111001 */
#నిర్వచించు HEX_LITTLEENDIAN   0x08

#నిర్వచించు CONDITIONAL_CAPITALIZE(c) (capitalize ? పెద్ద_అక్షరం((గుర్తులేని అక్షరం)(c)) : (c))

#నిర్వచించు COLOR_PROLOGUE(color) \
l_colored[c++] = '\033'; \
l_colored[c++] = '['; \
l_colored[c++] = '1'; \
l_colored[c++] = ';'; \
l_colored[c++] = '3'; \
l_colored[c++] = (color); \
l_colored[c++] = 'm';

#నిర్వచించు COLOR_EPILOGUE \
l_colored[c++] = '\033'; \
l_colored[c++] = '['; \
l_colored[c++] = '0'; \
l_colored[c++] = 'm';
#నిర్వచించు COLOR_RED '1'
#నిర్వచించు COLOR_GREEN '2'
#నిర్వచించు COLOR_YELLOW '3'
#నిర్వచించు COLOR_BLUE '4'
#నిర్వచించు COLOR_WHITE '7'

స్థిర అక్షరం *pname;

  స్థిర శూన్య
exit_with_usage(శూన్య)
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
#నిర్వచితమైతే TRY_SEEK
	  "[+][-]", "(or +: rel.) ");
#లేకపోతే
	  "", "");
#అయితే_ముగింపు
  fprintf(stderr, "    -u          use upper case hex letters.\n"
		  "    -R when     colorize the output; <when> can be 'always', 'auto' or 'never'. Default: 'auto'.\n"
		  "    -v          show version: \"%s%s\".\n", సంస్కరణ_2, osver);
  నిష్క్రమించు(1);
}

  స్థిర శూన్య
perror_exit(పూర్ణాంకం ret)
{
  fprintf(stderr, "%s: ", pname);
  perror(ఖాళీ);
  నిష్క్రమించు(ret);
}

  స్థిర శూన్య
error_exit(పూర్ణాంకం ret, అక్షరం *సందేశం)
{
  fprintf(stderr, "%s: %s\n", pname, సందేశం);
  నిష్క్రమించు(ret);
}

  స్థిర పూర్ణాంకం
getc_or_die(FILE *fpi)
{
  పూర్ణాంకం c = getc(fpi);
  ఒకవేళ (c == EOF && ferror(fpi))
    perror_exit(2);
  తిరిగి c;
}

  స్థిర శూన్య
putc_or_die(పూర్ణాంకం c, FILE *fpo)
{
  ఒకవేళ (putc(c, fpo) == EOF)
    perror_exit(3);
}

  స్థిర శూన్య
fputs_or_die(అక్షరం *s, FILE *fpo)
{
  ఒకవేళ (fputs(s, fpo) == EOF)
    perror_exit(3);
}

/* Use a macro to allow for different arguments. */
#నిర్వచించు FPRINTF_OR_DIE(వాదనలు) ఒకవేళ (fprintf వాదనలు < 0) perror_exit(3)

  స్థిర శూన్య
fclose_or_die(FILE *fpi, FILE *fpo)
{
  ఒకవేళ (fclose(fpo) != 0)
    perror_exit(3);
  ఒకవేళ (fclose(fpi) != 0)
    perror_exit(2);
}

/*
 * If "c" is a hex digit, return the value.
 * Otherwise return -1.
 */
  స్థిర పూర్ణాంకం
parse_hex_digit(పూర్ణాంకం c)
{
  తిరిగి (c >= '0' && c <= '9') ? c - '0'
	: (c >= 'a' && c <= 'f') ? c - 'a' + 10
	: (c >= 'A' && c <= 'F') ? c - 'A' + 10
	: -1;
}

/*
 * If "c" is a bin digit, return the value.
 * Otherwise return -1.
 */
  స్థిర పూర్ణాంకం
parse_bin_digit(పూర్ణాంకం c)
{
  తిరిగి (c >= '0' && c <= '1') ? c - '0'
	: -1;
}

/*
 * Ignore text on "fpi" until end-of-line or end-of-file.
 * Return the '\n' or EOF character.
 * When an error is encountered exit with an error message.
 */
  స్థిర పూర్ణాంకం
skip_గమ్యం_eol(FILE *fpi, పూర్ణాంకం c)
{
  అయితే (c != '\n' && c != EOF)
    c = getc_or_die(fpi);
  తిరిగి c;
}

/*
 * Max. cols binary characters are decoded from the input stream per line.
 * Two adjacent garbage characters after evaluated data delimit valid data.
 * Everything up to the next newline is discarded.
 *
 * The name is historic and came from 'undo type opt h'.
 */
  స్థిర పూర్ణాంకం
huntype(
  FILE *fpi,
  FILE *fpo,
  పూర్ణాంకం నిలువుల_సంఖ్య,
  పూర్ణాంకం hextype,
  దీర్ఘ పునాది_off)
{
  పూర్ణాంకం c, ign_garb = 1, n1 = -1, n2 = 0, n3 = 0, p = నిలువుల_సంఖ్య, bt = 0, b = 0, bcnt = 0;
  దీర్ఘ have_off = 0, want_off = 0;

  rewind(fpi);

  అయితే ((c = getc(fpi)) != EOF)
    {
      ఒకవేళ (c == '\r')	/* Doze style input file? */
	కొనసాగు;

      /* Allow multiple spaces.  This doesn't work when there is normal text
       * after the hex codes in the last line that looks like hex, thus only
       * use it for PostScript format. */
      ఒకవేళ (hextype == HEX_POSTSCRIPT && (c == ' ' || c == '\n' || c == '\t'))
	కొనసాగు;

      ఒకవేళ (hextype == HEX_సాధారణ || hextype == HEX_POSTSCRIPT)
	{
	  n3 = n2;
	  n2 = n1;

	  n1 = parse_hex_digit(c);
	  ఒకవేళ (n1 == -1 && ign_garb)
	    కొనసాగు;
	}
      లేకపోతే /* HEX_BITS */
	{
	  n1 = parse_hex_digit(c);
	  ఒకవేళ (n1 == -1 && ign_garb)
	    కొనసాగు;

	  bt = parse_bin_digit(c);
	  ఒకవేళ (bt != -1)
	    {
	      /* shift via unsigned to avoid signed overflow on bad input */
	      b = (పూర్ణాంకం)(((గుర్తులేని)b << 1) | (గుర్తులేని)bt);
	      ++bcnt;
	    }
	}

      ign_garb = 0;

      ఒకవేళ ((hextype != HEX_POSTSCRIPT) && (p >= నిలువుల_సంఖ్య))
	{
	  ఒకవేళ (hextype == HEX_సాధారణ)
	    {
	      ఒకవేళ (n1 < 0)
		{
		  p = 0;
		  కొనసాగు;
		}
	      want_off = (దీర్ఘ)(((గుర్తులేని దీర్ఘ)want_off << 4) | (గుర్తులేని)n1);
	    }
	  లేకపోతే /* HEX_BITS */
	    {
	      ఒకవేళ (n1 < 0)
		{
		  p = 0;
		  bcnt = 0;
		  కొనసాగు;
		}
	      want_off = (దీర్ఘ)(((గుర్తులేని దీర్ఘ)want_off << 4) | (గుర్తులేని)n1);
	    }
	  కొనసాగు;
	}

      ఒకవేళ (పునాది_off + want_off != have_off)
	{
	  ఒకవేళ (fflush(fpo) != 0)
	    perror_exit(3);
#నిర్వచితమైతే TRY_SEEK
	  ఒకవేళ (fseek(fpo, పునాది_off + want_off - have_off, SEEK_CUR) >= 0)
	    have_off = పునాది_off + want_off;
#అయితే_ముగింపు
	  ఒకవేళ (పునాది_off + want_off < have_off)
	    error_exit(5, "Sorry, cannot seek backwards.");
	  కొరకు (; have_off < పునాది_off + want_off; have_off++)
	    putc_or_die(0, fpo);
	}

      ఒకవేళ (hextype == HEX_సాధారణ || hextype == HEX_POSTSCRIPT)
	{
	  ఒకవేళ (n2 >= 0 && n1 >= 0)
	    {
	      putc_or_die((n2 << 4) | n1, fpo);
	      have_off++;
	      want_off++;
	      n1 = -1;
	      ఒకవేళ (!hextype && (++p >= నిలువుల_సంఖ్య))
	      /* skip the rest of the line as garbage */
	      c = skip_గమ్యం_eol(fpi, c);
	    }
	  లేకపోతే ఒకవేళ (n1 < 0 && n2 < 0 && n3 < 0)
	    /* already stumbled into garbage, skip line, wait and see */
	    c = skip_గమ్యం_eol(fpi, c);
	}
      లేకపోతే /* HEX_BITS */
	{
	  ఒకవేళ (bcnt == 8)
	    {
	      putc_or_die(b, fpo);
	      have_off++;
	      want_off++;
	      b = 0;
	      bcnt = 0;
	      ఒకవేళ (++p >= నిలువుల_సంఖ్య)
		/* skip the rest of the line as garbage */
		 c = skip_గమ్యం_eol(fpi, c);
	    }
	}

      ఒకవేళ (c == '\n')
	{
	  ఒకవేళ (hextype == HEX_సాధారణ || hextype == HEX_బిట్లు)
	    want_off = 0;
	  p = నిలువుల_సంఖ్య;
	  ign_garb = 1;
	}
    }
  ఒకవేళ (fflush(fpo) != 0)
    perror_exit(3);
#నిర్వచితమైతే TRY_SEEK
  fseek(fpo, 0L, SEEK_END);
#అయితే_ముగింపు
  fclose_or_die(fpi, fpo);
  తిరిగి 0;
}


/*
 * Print line l with given colors.
 */
  స్థిర శూన్య
print_colored_పంక్తి(FILE *fp, అక్షరం *l, అక్షరం *colors)
{
  స్థిర అక్షరం l_colored[LLEN+1];

  ఒకవేళ (colors)
    {
      పూర్ణాంకం c = 0;
      ఒకవేళ (colors[0])
	{
	  COLOR_PROLOGUE(colors[0])
	}
      l_colored[c++] = l[0];
      పూర్ణాంకం i;
      కొరకు (i = 1; l[i]; i++)
	{
	  ఒకవేళ (colors[i] != colors[i-1])
	    {
	      ఒకవేళ (colors[i-1])
		{
		  COLOR_EPILOGUE
		}
	      ఒకవేళ (colors[i])
		{
		  COLOR_PROLOGUE(colors[i])
		}
	    }
	  l_colored[c++] = l[i];
	}

      ఒకవేళ (colors[i])
	{
	  COLOR_EPILOGUE
	}
      l_colored[c++] = '\0';

      fputs_or_die(l_colored, fp);
    }
  లేకపోతే
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
  స్థిర శూన్య
xxdline(FILE *fp, అక్షరం *l, అక్షరం *colors, పూర్ణాంకం nz)
{
  స్థిర అక్షరం z[LLEN_NO_COLOR+1];
  స్థిర అక్షరం z_colors[LLEN_NO_COLOR+1];
  స్థిర గుర్తుగల అక్షరం zero_చూసిన = 0;

  ఒకవేళ (!nz && zero_చూసిన == 1)
    {
      strcpy(z, l);
      ఒకవేళ (colors)
	  జ్ఞాపకం_నకలు(z_colors, colors, అక్షరశ్రేణి_పొడవు(z));
    }

  ఒకవేళ (nz || !zero_చూసిన++)
    {
      ఒకవేళ (nz)
	{
	  ఒకవేళ (nz < 0)
	    zero_చూసిన--;
	  ఒకవేళ (zero_చూసిన == 2)
	    print_colored_పంక్తి(fp, z, z_colors);
	  ఒకవేళ (zero_చూసిన > 2)
	    fputs_or_die("*\n", fp);
	}
      ఒకవేళ (nz >= 0 || zero_చూసిన > 0)
	print_colored_పంక్తి(fp, l, colors);

      ఒకవేళ (nz)
	zero_చూసిన = 0;
    }

  /* If zero_seen > 3, then its exact value doesn't matter, so long as it
   * remains >3 and incrementing it will not cause overflow. */
  ఒకవేళ (zero_చూసిన >= 0x7F)
    zero_చూసిన = 4;
}

/* This is an EBCDIC to ASCII conversion table */
/* from a proposed BTL standard April 16, 1979 */
స్థిర గుర్తులేని అక్షరం etoa64[] =
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

  స్థిర అక్షరం
get_color_char (పూర్ణాంకం e, పూర్ణాంకం ebcdic)
{
  ఒకవేళ (ebcdic)
    {
      ఒకవేళ ((e >= 75 && e <= 80) || (e >= 90 && e <= 97) ||
	  (e >= 107 && e <= 111) || (e >= 121 && e <= 127) ||
	  (e >= 129 && e <= 137) || (e >= 145 && e <= 154) ||
	  (e >= 162 && e <= 169) || (e >= 192 && e <= 201) ||
	  (e >= 208 && e <= 217) || (e >= 226 && e <= 233) ||
	  (e >= 240 && e <= 249) || (e == 189) || (e == 64) ||
	  (e == 173) || (e == 224) )
	తిరిగి COLOR_GREEN;

      లేకపోతే ఒకవేళ (e == 37 || e == 13 || e == 5)
	తిరిగి COLOR_YELLOW;
      లేకపోతే ఒకవేళ (e == 0)
	తిరిగి COLOR_WHITE;
      లేకపోతే ఒకవేళ (e == 255)
	తిరిగి COLOR_BLUE;
      లేకపోతే
	తిరిగి COLOR_RED;
    }
  లేకపోతే  /* ASCII */
    {
#ఒకవేళ defined(__MVS__) && __CHARSET_LIB == 0
      ఒకవేళ (e >= 64)
	తిరిగి COLOR_GREEN;
#లేకపోతే
      ఒకవేళ (e > 31 && e < 127)
	తిరిగి COLOR_GREEN;
#అయితే_ముగింపు

      లేకపోతే ఒకవేళ (e == 9 || e == 10 || e == 13)
	తిరిగి COLOR_YELLOW;
      లేకపోతే ఒకవేళ (e == 0)
	తిరిగి COLOR_WHITE;
      లేకపోతే ఒకవేళ (e == 255)
	తిరిగి COLOR_BLUE;
      లేకపోతే
	తిరిగి COLOR_RED;
    }
    తిరిగి 0;
}

  స్థిర పూర్ణాంకం
enable_color(శూన్య)
{
#నిర్వచితమైతే WIN32
  DWORD   బహుళకం;
  HANDLE  బహిర్గతం;

  ఒకవేళ (!isatty(1))
    తిరిగి 0;

  బహిర్గతం = GetStdHandle(STD_OUTPUT_HANDLE);
  Get_Console_బహుళకం(బహిర్గతం, &బహుళకం);
  బహుళకం |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  తిరిగి (పూర్ణాంకం)సమితి_Console_బహుళకం(బహిర్గతం, బహుళకం);
#లేదావేళ defined(UNIX)
  అక్షరం *పదం;

  ఒకవేళ (!isatty(STDOUT_FILENO))
    తిరిగి 0;

  పదం = getenv("TERM");
  ఒకవేళ (పదం == ఖాళీ || *పదం == '\0' || !strcmp(పదం, "dumb"))
    తిరిగి 0;

  తిరిగి 1;
#లేకపోతే
  తిరిగి 0;
#అయితే_ముగింపు
}

  పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
  FILE *fp, *fpo;
  పూర్ణాంకం c, e, p = 0, relseek = 1, negseek = 0, revert = 0, i, x;
  పూర్ణాంకం నిలువుల_సంఖ్య = 0, colsgiven = 0, nonzero = 0, autoskip = 0, hextype = HEX_సాధారణ;
  పూర్ణాంకం capitalize = 0, decimal_స్థానభ్రంశం = 0;
  పూర్ణాంకం ebcdic = 0;
  పూర్ణాంకం termination = 0;
  పూర్ణాంకం octspergrp = -1;	/* number of octets grouped in output */
  పూర్ణాంకం grplen;		/* total chars per octet group excluding colors */
  దీర్ఘ నిడివి = -1, n = 0, seekoff = 0;
  గుర్తులేని దీర్ఘ displayoff = 0;
  స్థిర అక్షరం l[LLEN_NO_COLOR+1];  /* static because it may be too big for stack */
  స్థిర అక్షరం colors[LLEN_NO_COLOR+1]; /* color array */
  అక్షరం *pp;
  అక్షరం *varname = ఖాళీ;
  పూర్ణాంకం addrlen = 9;
  పూర్ణాంకం color = 0;
  పూర్ణాంకం color_forced = 0;	/* set when -R always is used */
  అక్షరం *no_color;
  అక్షరం cur_color = 0;

  no_color = getenv("NO_COLOR");
  ఒకవేళ (no_color == ఖాళీ || no_color[0] == '\0')
    color = enable_color();

#నిర్వచితమైతే AMIGA
  /* This program doesn't work when started from the Workbench */
  ఒకవేళ (argc == 0)
    నిష్క్రమించు(1);
#అయితే_ముగింపు

  pname = argv[0];
  కొరకు (pp = pname; *pp; )
    ఒకవేళ (*pp++ == మార్గం_SEP)
      pname = pp;
#నిర్వచితమైతే FILE_SEP
  కొరకు (pp = pname; *pp; pp++)
    ఒకవేళ (*pp == FILE_SEP)
      {
	*pp = '\0';
	ఆపు;
      }
#అయితే_ముగింపు

  అయితే (argc >= 2)
    {
      pp = argv[1] + (!STRNCMP(argv[1], "--", 2) && argv[1][2]);
	   ఒకవేళ (!STRNCMP(pp, "-a", 2)) autoskip = 1 - autoskip;
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-b", 2)) hextype |= HEX_బిట్లు;
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-e", 2)) hextype |= HEX_LITTLEENDIAN;
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-u", 2)) hexx = hexxa + 16;
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-p", 2)) hextype |= HEX_POSTSCRIPT;
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-i", 2)) hextype |= HEX_CINCLUDE;
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-C", 2)) capitalize = 1;
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-d", 2)) decimal_స్థానభ్రంశం = 1;
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-r", 2)) revert++;
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-E", 2)) ebcdic++;
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-t", 2)) termination++;
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-v", 2))
	{
	  fprintf(stderr, "%s%s\n", సంస్కరణ_2, osver);
	  నిష్క్రమించు(0);
	}
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-c", 2))
	{
	  ఒకవేళ (pp[2] && !STRNCMP("apitalize", pp + 2, 9))
	    capitalize = 1;
	  లేకపోతే ఒకవేళ (pp[2] && STRNCMP("ols", pp + 2, 3))
	    {
	      colsgiven = 1;
	      నిలువుల_సంఖ్య = (పూర్ణాంకం)strtol(pp + 2, ఖాళీ, 0);
	    }
	  లేకపోతే
	    {
	      ఒకవేళ (!argv[2])
		exit_with_usage();
	      colsgiven = 1;
	      నిలువుల_సంఖ్య = (పూర్ణాంకం)strtol(argv[2], ఖాళీ, 0);
	      argv++;
	      argc--;
	    }
	}
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-g", 2))
	{
	  ఒకవేళ (pp[2] && STRNCMP("roup", pp + 2, 4))
	    octspergrp = (పూర్ణాంకం)strtol(pp + 2, ఖాళీ, 0);
	  లేకపోతే
	    {
	      ఒకవేళ (!argv[2])
		exit_with_usage();
	      octspergrp = (పూర్ణాంకం)strtol(argv[2], ఖాళీ, 0);
	      argv++;
	      argc--;
	    }
	}
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-o", 2))
	{
	  పూర్ణాంకం reloffset = 0;
	  పూర్ణాంకం negoffset = 0;
	  ఒకవేళ (pp[2] && STRNCMP("ffset", pp + 2, 5))
	    displayoff = strtoul(pp + 2, ఖాళీ, 0);
	  లేకపోతే
	    {
	      ఒకవేళ (!argv[2])
		exit_with_usage();

	      ఒకవేళ (argv[2][0] == '+')
	       reloffset++;
	     ఒకవేళ (argv[2][reloffset] == '-')
	       negoffset++;

	     ఒకవేళ (negoffset)
	       displayoff = ULONG_MAX - strtoul(argv[2] + reloffset+negoffset, ఖాళీ, 0) + 1;
	     లేకపోతే
	       displayoff = strtoul(argv[2] + reloffset+negoffset, ఖాళీ, 0);

	      argv++;
	      argc--;
	    }
	}
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-s", 2))
	{
	  relseek = 0;
	  negseek = 0;
	  ఒకవేళ (pp[2] && STRNCMP("kip", pp+2, 3) && STRNCMP("eek", pp+2, 3))
	    {
#నిర్వచితమైతే TRY_SEEK
	      ఒకవేళ (pp[2] == '+')
		relseek++;
	      ఒకవేళ (pp[2+relseek] == '-')
		negseek++;
#అయితే_ముగింపు
	      seekoff = strtol(pp + 2+relseek+negseek, (అక్షరం **)ఖాళీ, 0);
	    }
	  లేకపోతే
	    {
	      ఒకవేళ (!argv[2])
		exit_with_usage();
#నిర్వచితమైతే TRY_SEEK
	      ఒకవేళ (argv[2][0] == '+')
		relseek++;
	      ఒకవేళ (argv[2][relseek] == '-')
		negseek++;
#అయితే_ముగింపు
	      seekoff = strtol(argv[2] + relseek+negseek, (అక్షరం **)ఖాళీ, 0);
	      argv++;
	      argc--;
	    }
	}
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-l", 2))
	{
	  ఒకవేళ (pp[2] && STRNCMP("en", pp + 2, 2))
	    నిడివి = strtol(pp + 2, (అక్షరం **)ఖాళీ, 0);
	  లేకపోతే
	    {
	      ఒకవేళ (!argv[2])
		exit_with_usage();
	      నిడివి = strtol(argv[2], (అక్షరం **)ఖాళీ, 0);
	      argv++;
	      argc--;
	    }
	}
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-n", 2))
	{
	  ఒకవేళ (pp[2] && STRNCMP("ame", pp + 2, 3))
	    varname = pp + 2;
	  లేకపోతే
	    {
	      ఒకవేళ (!argv[2])
		exit_with_usage();
	      varname = argv[2];
	      argv++;
	      argc--;
	    }
	}
      లేకపోతే ఒకవేళ (!STRNCMP(pp, "-R", 2))
	{
	  అక్షరం *pw = pp + 2;
	  ఒకవేళ (!pw[0])
	    {
	      pw = argv[2];
	      argv++;
	      argc--;
	    }
	  ఒకవేళ (!pw)
	    exit_with_usage();
	  ఒకవేళ (!STRNCMP(pw, "always", 6))
	    {
	      (శూన్య)enable_color();
	      color = 1;
	      color_forced = 1;
	    }
	  లేకపోతే ఒకవేళ (!STRNCMP(pw, "never", 5))
	    color = 0;
	  లేకపోతే ఒకవేళ (!STRNCMP(pw, "auto", 4))
	    color = enable_color();
	  లేకపోతే
	    exit_with_usage();
	}
      లేకపోతే ఒకవేళ (!strcmp(argv[1], "--"))	/* end of options */
	{
	  argv++;
	  argc--;
	  ఆపు;
	}
      లేకపోతే ఒకవేళ (pp[0] == '-' && pp[1])	/* unknown option */
	exit_with_usage();
      లేకపోతే
	ఆపు;				/* not an option */

      argv++;				/* advance to next argument */
      argc--;
    }

  ఒకవేళ (hextype != (HEX_CINCLUDE | HEX_బిట్లు))
    {
	/* Allow at most one bit to be set in hextype */
	ఒకవేళ (hextype & (hextype - 1))
	    error_exit(1, "only one of -b, -e, -u, -p, -i can be used");
    }

  ఒకవేళ (!colsgiven || (!నిలువుల_సంఖ్య && hextype != HEX_POSTSCRIPT))
    ఎంపిక (hextype)
      {
      స్థితి HEX_POSTSCRIPT:	నిలువుల_సంఖ్య = 30; ఆపు;
      స్థితి HEX_CINCLUDE:	నిలువుల_సంఖ్య = 12; ఆపు;
      స్థితి HEX_CINCLUDE | HEX_బిట్లు:
      స్థితి HEX_బిట్లు:		నిలువుల_సంఖ్య = 6; ఆపు;
      స్థితి HEX_సాధారణ:
      స్థితి HEX_LITTLEENDIAN:
      అప్రమేయ:			నిలువుల_సంఖ్య = 16; ఆపు;
      }

  ఒకవేళ (octspergrp < 0)
    ఎంపిక (hextype)
      {
      స్థితి HEX_CINCLUDE | HEX_బిట్లు:
      స్థితి HEX_బిట్లు:		octspergrp = 1; ఆపు;
      స్థితి HEX_సాధారణ:		octspergrp = 2; ఆపు;
      స్థితి HEX_LITTLEENDIAN:	octspergrp = 4; ఆపు;
      స్థితి HEX_POSTSCRIPT:
      స్థితి HEX_CINCLUDE:
      అప్రమేయ:			octspergrp = 0; ఆపు;
      }

  ఒకవేళ ((hextype == HEX_POSTSCRIPT && నిలువుల_సంఖ్య < 0) ||
      (hextype != HEX_POSTSCRIPT && నిలువుల_సంఖ్య < 1) ||
      ((hextype == HEX_సాధారణ || hextype == HEX_బిట్లు || hextype == HEX_LITTLEENDIAN)
							    && (నిలువుల_సంఖ్య > నిలువుల_సంఖ్య_2)))
    {
      fprintf(stderr, "%s: invalid number of columns (max. %d).\n", pname, నిలువుల_సంఖ్య_2);
      నిష్క్రమించు(1);
    }

  ఒకవేళ (octspergrp < 1 || octspergrp > నిలువుల_సంఖ్య)
    octspergrp = నిలువుల_సంఖ్య;
  లేకపోతే ఒకవేళ (hextype == HEX_LITTLEENDIAN && (octspergrp & (octspergrp-1)))
    error_exit(1, "number of octets per group must be a power of 2 with -e.");

  ఒకవేళ (argc > 3)
    exit_with_usage();

  ఒకవేళ (argc == 1 || (argv[1][0] == '-' && !argv[1][1]))
    BIN_కేటాయించు(fp = stdin, !revert);
  లేకపోతే
    {
      ఒకవేళ ((fp = fopen(argv[1], BIN_READ(!revert))) == ఖాళీ)
	{
	  fprintf(stderr,"%s: ", pname);
	  perror(argv[1]);
	  తిరిగి 2;
	}
    }

  ఒకవేళ (argc < 3 || (argv[2][0] == '-' && !argv[2][1]))
    BIN_కేటాయించు(fpo = stdout, revert);
  లేకపోతే
    {
      పూర్ణాంకం fd;
      పూర్ణాంకం బహుళకం = revert ? O_WRONLY : (O_TRUNC|O_WRONLY);

      ఒకవేళ (((fd = OPEN(argv[2], బహుళకం | BIN_CREAT(revert), 0666)) < 0) ||
	  (fpo = fdopen(fd, BIN_WRITE(revert))) == ఖాళీ)
	{
	  fprintf(stderr, "%s: ", pname);
	  perror(argv[2]);
	  తిరిగి 3;
	}
      rewind(fpo);

      /* Disable auto color when writing to a file. */
      ఒకవేళ (!color_forced)
	color = 0;
    }

  ఒకవేళ (revert)
    ఎంపిక (hextype)
      {
      స్థితి HEX_సాధారణ:
      స్థితి HEX_POSTSCRIPT:
      స్థితి HEX_బిట్లు:
	తిరిగి huntype(fp, fpo, నిలువుల_సంఖ్య, hextype,
	  negseek ? -seekoff : seekoff);
	ఆపు;
      అప్రమేయ:
	error_exit(-1, "Sorry, cannot revert this type of hexdump");
      }

  ఒకవేళ (seekoff || negseek || !relseek)
    {
#నిర్వచితమైతే TRY_SEEK
      ఒకవేళ (relseek)
	e = fseek(fp, negseek ? -seekoff : seekoff, SEEK_CUR);
      లేకపోతే
	e = fseek(fp, negseek ? -seekoff : seekoff,
						negseek ? SEEK_END : SEEK_SET);
      ఒకవేళ (e < 0 && negseek)
	error_exit(4, "Sorry, cannot seek.");
      ఒకవేళ (e >= 0)
	seekoff = ftell(fp);
      లేకపోతే
#అయితే_ముగింపు
	{
	  దీర్ఘ s = seekoff;

	  అయితే (s--)
	    ఒకవేళ (getc_or_die(fp) == EOF)
	    {
	      error_exit(4, "Sorry, cannot seek.");
	    }
	}
    }

  ఒకవేళ (hextype & HEX_CINCLUDE)
    {
      /* A user-set variable name overrides fp == stdin */
      ఒకవేళ (varname == ఖాళీ && fp != stdin)
	varname = argv[1];

      ఒకవేళ (varname != ఖాళీ)
	{
	  FPRINTF_OR_DIE((fpo, "unsigned char %s", అంకెనా((గుర్తులేని అక్షరం)varname[0]) ? "__" : ""));
	  కొరకు (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(అక్షరమా_అంకెనా((గుర్తులేని అక్షరం)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  fputs_or_die("[] = {\n", fpo);
	}

      p = 0;
      అయితే ((నిడివి < 0 || p < నిడివి) && (((c = getc_or_die(fp)) != EOF) || termination))
	{
	  ఒకవేళ (c == EOF)
	    {
	      c = 0;
	      termination = -1;
	    }
	  ఒకవేళ (hextype & HEX_బిట్లు)
	    {
	      ఒకవేళ (p == 0)
		fputs_or_die("  ", fpo);
	      లేకపోతే ఒకవేళ (p % నిలువుల_సంఖ్య == 0)
		fputs_or_die(",\n  ", fpo);
	      లేకపోతే
		fputs_or_die(", ", fpo);

	      FPRINTF_OR_DIE((fpo, "0b"));
	      కొరకు (పూర్ణాంకం j = 7; j >= 0; j--)
		putc_or_die((c & (1 << j)) ? '1' : '0', fpo);
	      p++;
	    }
	  లేకపోతే
	    {
	      FPRINTF_OR_DIE((fpo, (hexx == hexxa) ? "%s0x%02x" : "%s0X%02X",
		(p % నిలువుల_సంఖ్య) ? ", " : (!p ? "  " : ",\n  "), c));
	      p++;
	    }
	  ఒకవేళ (termination == -1)
	    {
	      --p;
	      ఆపు ;
	    }
	}

      ఒకవేళ (p)
	fputs_or_die("\n", fpo);

      ఒకవేళ (varname != ఖాళీ)
	{
	  fputs_or_die("};\n", fpo);
	  FPRINTF_OR_DIE((fpo, "unsigned int %s", అంకెనా((గుర్తులేని అక్షరం)varname[0]) ? "__" : ""));
	  కొరకు (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(అక్షరమా_అంకెనా((గుర్తులేని అక్షరం)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  FPRINTF_OR_DIE((fpo, "_%s = %d;\n", capitalize ? "LEN" : "len", p));
	}

      fclose_or_die(fp, fpo);
      తిరిగి 0;
    }

  ఒకవేళ (hextype == HEX_POSTSCRIPT)
    {
      p = నిలువుల_సంఖ్య;
      అయితే ((నిడివి < 0 || n < నిడివి) && (e = getc_or_die(fp)) != EOF)
	{
	  putc_or_die(hexx[(e >> 4) & 0xf], fpo);
	  putc_or_die(hexx[e & 0xf], fpo);
	  n++;
	  ఒకవేళ (నిలువుల_సంఖ్య > 0 && !--p)
	    {
	      putc_or_die('\n', fpo);
	      p = నిలువుల_సంఖ్య;
	    }
	}
      ఒకవేళ (నిలువుల_సంఖ్య == 0 || p < నిలువుల_సంఖ్య)
	putc_or_die('\n', fpo);
      fclose_or_die(fp, fpo);
      తిరిగి 0;
    }

  /* hextype: HEX_NORMAL or HEX_BITS or HEX_LITTLEENDIAN */

  ఒకవేళ (hextype != HEX_బిట్లు)
    {
      grplen = octspergrp + octspergrp + 1;	/* chars per octet group */
    }
  లేకపోతే	/* hextype == HEX_BITS */
    grplen = 8 * octspergrp + 1;

  అయితే ((నిడివి < 0 || n < నిడివి) && (e = getc_or_die(fp)) != EOF)
    {
      ఒకవేళ (p == 0)
	{
	  addrlen = sprintf(l, decimal_స్థానభ్రంశం ? "%08lu:" : "%08lx:",
				  ((గుర్తులేని దీర్ఘ)(n + seekoff + displayoff)));
	  కొరకు (c = addrlen; c < LLEN_NO_COLOR; l[c++] = ' ')
	    ;
	}
      x = hextype == HEX_LITTLEENDIAN ? p ^ (octspergrp-1) : p;
      c = addrlen + 1 + (grplen * x) / octspergrp;
      ఒకవేళ (hextype == HEX_సాధారణ || hextype == HEX_LITTLEENDIAN)
	{
	  ఒకవేళ (color)
	    {
	      cur_color = get_color_char(e, ebcdic);
	      colors[c] = cur_color;
	      colors[c+1] = cur_color;
	    }

	  l[c]   = hexx[(e >> 4) & 0xf];
	  l[++c] = hexx[e & 0xf];
	}
      లేకపోతే /* hextype == HEX_BITS */
	{
	  ఒకవేళ (color)
	    cur_color = get_color_char(e, ebcdic);

	  కొరకు (i = 7; i >= 0; i--)
	    {
	      ఒకవేళ (color)
		colors[c] = cur_color;
	      l[c++] = (e & (1 << i)) ? '1' : '0';
	    }
	}
      ఒకవేళ (e)
	nonzero++;
      /* When changing this update definition of LLEN and LLEN_NO_COLOR above. */
      ఒకవేళ (hextype == HEX_LITTLEENDIAN)
	/* last group will be fully used, round up */
	c = grplen * ((నిలువుల_సంఖ్య + octspergrp - 1) / octspergrp);
      లేకపోతే
	c = (grplen * నిలువుల_సంఖ్య - 1) / octspergrp;


      ఒకవేళ (ebcdic)
	e = (e < 64) ? '.' : etoa64[e-64];

      ఒకవేళ (hextype == HEX_LITTLEENDIAN)
	c -= 1;

      c += addrlen + 3 + p;
      ఒకవేళ (color)
	  colors[c] = cur_color;
      l[c++] =
#ఒకవేళ defined(__MVS__) && __CHARSET_LIB == 0
	  (e >= 64)
#లేకపోతే
	  (e > 31 && e < 127)
#అయితే_ముగింపు
	  ? e : '.';
      n++;
      ఒకవేళ (++p == నిలువుల_సంఖ్య)
	{
	  l[c++] = '\n';
	  l[c] = '\0';

	  xxdline(fpo, l, color ? colors : ఖాళీ, autoskip ? nonzero : 1);
	  memset(colors, 0, c);
	  nonzero = 0;
	  p = 0;
	}
    }
  ఒకవేళ (p)
    {
      l[c++] = '\n';
      l[c] = '\0';
      ఒకవేళ (color)
	{
	  x = p;
	  ఒకవేళ (hextype == HEX_LITTLEENDIAN)
	    {
	      పూర్ణాంకం fill = octspergrp - (p % octspergrp);
	      ఒకవేళ (fill == octspergrp) fill = 0;

	      c = addrlen + 1 + (grplen * (x - (octspergrp-fill))) / octspergrp;

	      కొరకు (i = 0; i < fill;i++)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		  x++;
		  p++;
		}
	    }

	  ఒకవేళ (hextype != HEX_బిట్లు)
	    {
	      c = addrlen + 1 + (grplen * x) / octspergrp;
	      c += నిలువుల_సంఖ్య - p;
	      c += (నిలువుల_సంఖ్య - p) / octspergrp;

	      కొరకు (i = నిలువుల_సంఖ్య - p; i > 0;i--)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		}
	    }
	  xxdline(fpo, l, colors, 1);
	}
      లేకపోతే
	xxdline(fpo, l, ఖాళీ, 1);
    }
  లేకపోతే ఒకవేళ (autoskip)
    xxdline(fpo, l, color ? colors : ఖాళీ, -1);	/* last chance to flush out suppressed lines */

  fclose_or_die(fp, fpo);
  తిరిగి 0;
}

/* vi:set ts=8 sw=4 sts=2 cino+={2 cino+=n-2 : */
