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
#ਜੇ _MSC_VER >= 1400
# define _CRT_SECURE_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CYGWIN) && defined(__CYGWIN__)
# define CYGWIN
#ਜੇ_ਅੰਤ

#ਜੇ (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__)
# define XOPEN_ਸਰੋਤ 700   /* for fdopen() */
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VAXC
# include <file.h>
#ਨਹੀਂਤਾਂ
# include <fcntl.h>
#ਜੇ_ਅੰਤ
#ਜੇ defined(WIN32) || defined(CYGWIN)
# include <io.h>	/* for setmode() */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
# include <windows.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
# include <unistd.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
#ਜੇ __MWERKS__ && !defined(BEBOX)
# include <unix.h>	/* for fdopen() on MAC */
#ਜੇ_ਅੰਤ


/*  This corrects the problem of missing prototypes for certain functions
 *  in some GNU installations (e.g. SunOS 4.1.x).
 *  Darren Hiebert <darren@hmi.com> (sparc-sun-sunos4.1.3_U1/2.7.2.2)
 */
#ਜੇ defined(__GNUC__) && defined(__STDC__)
# ifndef __USE_FIXED_PROTOTYPES__
#  define __USE_FIXED_PROTOTYPES__
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __USE_FIXED_PROTOTYPES__
/*
 * This is historic and works only if the compiler really has no prototypes:
 *
 * Include prototypes for Sun OS 4.x, when using an ANSI compiler.
 * FILE is defined on OS 4.x, not on 5.x (Solaris).
 * if __SVR4 is defined (some Solaris versions), don't include this.
 */
# ਜੇ defined(sun) && defined(FILE) && !defined(__SVR4) && defined(__STDC__)
#  define __P(a) a
/* excerpt from my sun_stdlib.h */
ਬਾਹਰੀ ਪੂਰਨ fprintf __P((FILE *, ਅੱਖਰ *, ...));
ਬਾਹਰੀ ਪੂਰਨ fputs   __P((ਅੱਖਰ *, FILE *));
ਬਾਹਰੀ ਪੂਰਨ _flsbuf __P((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ, FILE *));
ਬਾਹਰੀ ਪੂਰਨ _filbuf __P((FILE *));
ਬਾਹਰੀ ਪੂਰਨ fflush  __P((FILE *));
ਬਾਹਰੀ ਪੂਰਨ fclose  __P((FILE *));
ਬਾਹਰੀ ਪੂਰਨ fseek   __P((FILE *, ਲੰਮਾ, ਪੂਰਨ));
ਬਾਹਰੀ ਪੂਰਨ rewind  __P((FILE *));

ਬਾਹਰੀ ਖਾਲੀ perror __P((ਅੱਖਰ *));
# endif
#ਜੇ_ਅੰਤ

ਅੱਖਰ ਸੰਸਕਰਣ_2[] = "xxd 2026-06-16 by Juergen Weigert et al.";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
ਅੱਖਰ osver[] = " (Win32)";
#ਨਹੀਂਤਾਂ
ਅੱਖਰ osver[] = "";
#ਜੇ_ਅੰਤ

#ਜੇ defined(WIN32)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "wt")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_ਦੁਆਧਾਰੀ) : O_CREAT)
# define BIN_ਸੌਂਪੋ(fp, yes) setmode(fileno(fp), (yes) ? O_ਦੁਆਧਾਰੀ : O_ਪਾਠ_2)
# define ਰਸਤਾ_SEP '\\'
#ਜਾਂਜੇ defined(CYGWIN)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "w")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_ਦੁਆਧਾਰੀ) : O_CREAT)
# define BIN_ਸੌਂਪੋ(fp, yes) ((yes) ? (ਖਾਲੀ) setmode(fileno(fp), O_ਦੁਆਧਾਰੀ) : (ਖਾਲੀ) (fp))
# define ਰਸਤਾ_SEP '/'
#ਨਹੀਂਤਾਂ
# ifdef VMS
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ਸੌਂਪੋ(fp, dummy) fp
#  define ਰਸਤਾ_SEP ']'
#  define FILE_SEP '.'
# ਨਹੀਂਤਾਂ
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ਸੌਂਪੋ(fp, dummy) fp
#  define ਰਸਤਾ_SEP '/'
# endif
#ਜੇ_ਅੰਤ

/* open has only to arguments on the Mac */
#ਜੇ __MWERKS__
# define OPEN(name, ਬਹੁਲਕ, umask) open(name, ਬਹੁਲਕ)
#ਨਹੀਂਤਾਂ
# define OPEN(name, ਬਹੁਲਕ, umask) open(name, ਬਹੁਲਕ, umask)
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
# define STRNCMP(s1, s2, l) ਲੜੀ_ਤੁਲਨਾ(s1, s2, (size_t)l)
#ਨਹੀਂਤਾਂ
# define STRNCMP(s1, s2, l) ਲੜੀ_ਤੁਲਨਾ(s1, s2, l)
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __P
# ਜੇ defined(__STDC__) || defined(WIN32)
#  define __P(a) a
# ਨਹੀਂਤਾਂ
#  define __P(a) ()
# endif
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRY_SEEK	/* attempt to use lseek, or skip forward by reading */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖਾਨੇ_ਗਿਣਤੀ_2 256	/* change here, if you ever need more columns */

/*
 * LLEN is the maximum length of a line; other than the visible characters
 * we need to consider also the escape color sequence prologue/epilogue ,
 * (11 bytes for each character).
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LLEN \
    (39            /* addr: ⌈log10(ULONG_MAX)⌉ if "-d" flag given. We assume ULONG_MAX = 2**128 */ \
    + 2            /* ": " */ \
    + 13 * ਖਾਨੇ_ਗਿਣਤੀ_2    /* hex dump with colors */ \
    + (ਖਾਨੇ_ਗਿਣਤੀ_2 - 1)   /* whitespace between groups if "-g1" option given and "-c" maxed out */ \
    + 2            /* whitespace */ \
    + 12 * ਖਾਨੇ_ਗਿਣਤੀ_2    /* ASCII dump with colors */ \
    + 2)           /* "\n\0" */

/*
 * LLEN_NO_COLOR is the maximum length of a line excluding the colors.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LLEN_NO_COLOR \
    (39            /* addr: ⌈log10(ULONG_MAX)⌉ if "-d" flag given. We assume ULONG_MAX = 2**128 */ \
    + 2            /* ": " */ \
    + 9 * ਖਾਨੇ_ਗਿਣਤੀ_2     /* hex dump, worst case: bitwise output using -b */ \
    + 2            /* whitespace */ \
    + ਖਾਨੇ_ਗਿਣਤੀ_2         /* ASCII dump */ \
    + 2)           /* "\n\0" */

ਅੱਖਰ hexxa[] = "0123456789abcdef0123456789ABCDEF", *hexx = hexxa;

/* the different hextypes known by this program: */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HEX_ਸਾਧਾਰਨ         0x00 /* no flags set */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HEX_POSTSCRIPT     0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HEX_CINCLUDE       0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HEX_ਬਿੱਟ           0x04 /* not hex a dump, but bits: 01111001 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HEX_LITTLEENDIAN   0x08

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONDITIONAL_CAPITALIZE(c) (capitalize ? ਵੱਡਾ_ਅੱਖਰ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)) : (c))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COLOR_PROLOGUE(color) \
l_colored[c++] = '\033'; \
l_colored[c++] = '['; \
l_colored[c++] = '1'; \
l_colored[c++] = ';'; \
l_colored[c++] = '3'; \
l_colored[c++] = (color); \
l_colored[c++] = 'm';

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COLOR_EPILOGUE \
l_colored[c++] = '\033'; \
l_colored[c++] = '['; \
l_colored[c++] = '0'; \
l_colored[c++] = 'm';
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COLOR_RED '1'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COLOR_GREEN '2'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COLOR_YELLOW '3'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COLOR_BLUE '4'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COLOR_WHITE '7'

ਸਥਿਰ ਅੱਖਰ *pname;

  ਸਥਿਰ ਖਾਲੀ
exit_with_usage(ਖਾਲੀ)
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRY_SEEK
	  "[+][-]", "(or +: rel.) ");
#ਨਹੀਂਤਾਂ
	  "", "");
#ਜੇ_ਅੰਤ
  fprintf(stderr, "    -u          use upper case hex letters.\n"
		  "    -R when     colorize the output; <when> can be 'always', 'auto' or 'never'. Default: 'auto'.\n"
		  "    -v          show version: \"%s%s\".\n", ਸੰਸਕਰਣ_2, osver);
  ਬਾਹਰ_ਨਿਕਲੋ(1);
}

  ਸਥਿਰ ਖਾਲੀ
perror_exit(ਪੂਰਨ ret)
{
  fprintf(stderr, "%s: ", pname);
  perror(ਨਿਰਾ);
  ਬਾਹਰ_ਨਿਕਲੋ(ret);
}

  ਸਥਿਰ ਖਾਲੀ
error_exit(ਪੂਰਨ ret, ਅੱਖਰ *ਸੁਨੇਹਾ)
{
  fprintf(stderr, "%s: %s\n", pname, ਸੁਨੇਹਾ);
  ਬਾਹਰ_ਨਿਕਲੋ(ret);
}

  ਸਥਿਰ ਪੂਰਨ
getc_or_die(FILE *fpi)
{
  ਪੂਰਨ c = getc(fpi);
  ਜੇ (c == EOF && ferror(fpi))
    perror_exit(2);
  ਵਾਪਸ c;
}

  ਸਥਿਰ ਖਾਲੀ
putc_or_die(ਪੂਰਨ c, FILE *fpo)
{
  ਜੇ (putc(c, fpo) == EOF)
    perror_exit(3);
}

  ਸਥਿਰ ਖਾਲੀ
fputs_or_die(ਅੱਖਰ *s, FILE *fpo)
{
  ਜੇ (fputs(s, fpo) == EOF)
    perror_exit(3);
}

/* Use a macro to allow for different arguments. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FPRINTF_OR_DIE(ਦਲੀਲਾਂ) ਜੇ (fprintf ਦਲੀਲਾਂ < 0) perror_exit(3)

  ਸਥਿਰ ਖਾਲੀ
fclose_or_die(FILE *fpi, FILE *fpo)
{
  ਜੇ (fclose(fpo) != 0)
    perror_exit(3);
  ਜੇ (fclose(fpi) != 0)
    perror_exit(2);
}

/*
 * If "c" is a hex digit, return the value.
 * Otherwise return -1.
 */
  ਸਥਿਰ ਪੂਰਨ
parse_hex_digit(ਪੂਰਨ c)
{
  ਵਾਪਸ (c >= '0' && c <= '9') ? c - '0'
	: (c >= 'a' && c <= 'f') ? c - 'a' + 10
	: (c >= 'A' && c <= 'F') ? c - 'A' + 10
	: -1;
}

/*
 * If "c" is a bin digit, return the value.
 * Otherwise return -1.
 */
  ਸਥਿਰ ਪੂਰਨ
parse_bin_digit(ਪੂਰਨ c)
{
  ਵਾਪਸ (c >= '0' && c <= '1') ? c - '0'
	: -1;
}

/*
 * Ignore text on "fpi" until end-of-line or end-of-file.
 * Return the '\n' or EOF character.
 * When an error is encountered exit with an error message.
 */
  ਸਥਿਰ ਪੂਰਨ
skip_ਮੰਜ਼ਿਲ_eol(FILE *fpi, ਪੂਰਨ c)
{
  ਜਦੋਂ (c != '\n' && c != EOF)
    c = getc_or_die(fpi);
  ਵਾਪਸ c;
}

/*
 * Max. cols binary characters are decoded from the input stream per line.
 * Two adjacent garbage characters after evaluated data delimit valid data.
 * Everything up to the next newline is discarded.
 *
 * The name is historic and came from 'undo type opt h'.
 */
  ਸਥਿਰ ਪੂਰਨ
huntype(
  FILE *fpi,
  FILE *fpo,
  ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ,
  ਪੂਰਨ hextype,
  ਲੰਮਾ ਅਧਾਰ_off)
{
  ਪੂਰਨ c, ign_garb = 1, n1 = -1, n2 = 0, n3 = 0, p = ਖਾਨੇ_ਗਿਣਤੀ, bt = 0, b = 0, bcnt = 0;
  ਲੰਮਾ have_off = 0, want_off = 0;

  rewind(fpi);

  ਜਦੋਂ ((c = getc(fpi)) != EOF)
    {
      ਜੇ (c == '\r')	/* Doze style input file? */
	ਜਾਰੀ;

      /* Allow multiple spaces.  This doesn't work when there is normal text
       * after the hex codes in the last line that looks like hex, thus only
       * use it for PostScript format. */
      ਜੇ (hextype == HEX_POSTSCRIPT && (c == ' ' || c == '\n' || c == '\t'))
	ਜਾਰੀ;

      ਜੇ (hextype == HEX_ਸਾਧਾਰਨ || hextype == HEX_POSTSCRIPT)
	{
	  n3 = n2;
	  n2 = n1;

	  n1 = parse_hex_digit(c);
	  ਜੇ (n1 == -1 && ign_garb)
	    ਜਾਰੀ;
	}
      ਨਹੀਂਤਾਂ /* HEX_BITS */
	{
	  n1 = parse_hex_digit(c);
	  ਜੇ (n1 == -1 && ign_garb)
	    ਜਾਰੀ;

	  bt = parse_bin_digit(c);
	  ਜੇ (bt != -1)
	    {
	      /* shift via unsigned to avoid signed overflow on bad input */
	      b = (ਪੂਰਨ)(((ਬਿਨਾਨਿਸ਼ਾਨੀ)b << 1) | (ਬਿਨਾਨਿਸ਼ਾਨੀ)bt);
	      ++bcnt;
	    }
	}

      ign_garb = 0;

      ਜੇ ((hextype != HEX_POSTSCRIPT) && (p >= ਖਾਨੇ_ਗਿਣਤੀ))
	{
	  ਜੇ (hextype == HEX_ਸਾਧਾਰਨ)
	    {
	      ਜੇ (n1 < 0)
		{
		  p = 0;
		  ਜਾਰੀ;
		}
	      want_off = (ਲੰਮਾ)(((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)want_off << 4) | (ਬਿਨਾਨਿਸ਼ਾਨੀ)n1);
	    }
	  ਨਹੀਂਤਾਂ /* HEX_BITS */
	    {
	      ਜੇ (n1 < 0)
		{
		  p = 0;
		  bcnt = 0;
		  ਜਾਰੀ;
		}
	      want_off = (ਲੰਮਾ)(((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)want_off << 4) | (ਬਿਨਾਨਿਸ਼ਾਨੀ)n1);
	    }
	  ਜਾਰੀ;
	}

      ਜੇ (ਅਧਾਰ_off + want_off != have_off)
	{
	  ਜੇ (fflush(fpo) != 0)
	    perror_exit(3);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRY_SEEK
	  ਜੇ (fseek(fpo, ਅਧਾਰ_off + want_off - have_off, SEEK_CUR) >= 0)
	    have_off = ਅਧਾਰ_off + want_off;
#ਜੇ_ਅੰਤ
	  ਜੇ (ਅਧਾਰ_off + want_off < have_off)
	    error_exit(5, "Sorry, cannot seek backwards.");
	  ਲਈ (; have_off < ਅਧਾਰ_off + want_off; have_off++)
	    putc_or_die(0, fpo);
	}

      ਜੇ (hextype == HEX_ਸਾਧਾਰਨ || hextype == HEX_POSTSCRIPT)
	{
	  ਜੇ (n2 >= 0 && n1 >= 0)
	    {
	      putc_or_die((n2 << 4) | n1, fpo);
	      have_off++;
	      want_off++;
	      n1 = -1;
	      ਜੇ (!hextype && (++p >= ਖਾਨੇ_ਗਿਣਤੀ))
	      /* skip the rest of the line as garbage */
	      c = skip_ਮੰਜ਼ਿਲ_eol(fpi, c);
	    }
	  ਨਹੀਂਤਾਂ ਜੇ (n1 < 0 && n2 < 0 && n3 < 0)
	    /* already stumbled into garbage, skip line, wait and see */
	    c = skip_ਮੰਜ਼ਿਲ_eol(fpi, c);
	}
      ਨਹੀਂਤਾਂ /* HEX_BITS */
	{
	  ਜੇ (bcnt == 8)
	    {
	      putc_or_die(b, fpo);
	      have_off++;
	      want_off++;
	      b = 0;
	      bcnt = 0;
	      ਜੇ (++p >= ਖਾਨੇ_ਗਿਣਤੀ)
		/* skip the rest of the line as garbage */
		 c = skip_ਮੰਜ਼ਿਲ_eol(fpi, c);
	    }
	}

      ਜੇ (c == '\n')
	{
	  ਜੇ (hextype == HEX_ਸਾਧਾਰਨ || hextype == HEX_ਬਿੱਟ)
	    want_off = 0;
	  p = ਖਾਨੇ_ਗਿਣਤੀ;
	  ign_garb = 1;
	}
    }
  ਜੇ (fflush(fpo) != 0)
    perror_exit(3);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRY_SEEK
  fseek(fpo, 0L, SEEK_END);
#ਜੇ_ਅੰਤ
  fclose_or_die(fpi, fpo);
  ਵਾਪਸ 0;
}


/*
 * Print line l with given colors.
 */
  ਸਥਿਰ ਖਾਲੀ
print_colored_ਸਤਰ(FILE *fp, ਅੱਖਰ *l, ਅੱਖਰ *colors)
{
  ਸਥਿਰ ਅੱਖਰ l_colored[LLEN+1];

  ਜੇ (colors)
    {
      ਪੂਰਨ c = 0;
      ਜੇ (colors[0])
	{
	  COLOR_PROLOGUE(colors[0])
	}
      l_colored[c++] = l[0];
      ਪੂਰਨ i;
      ਲਈ (i = 1; l[i]; i++)
	{
	  ਜੇ (colors[i] != colors[i-1])
	    {
	      ਜੇ (colors[i-1])
		{
		  COLOR_EPILOGUE
		}
	      ਜੇ (colors[i])
		{
		  COLOR_PROLOGUE(colors[i])
		}
	    }
	  l_colored[c++] = l[i];
	}

      ਜੇ (colors[i])
	{
	  COLOR_EPILOGUE
	}
      l_colored[c++] = '\0';

      fputs_or_die(l_colored, fp);
    }
  ਨਹੀਂਤਾਂ
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
  ਸਥਿਰ ਖਾਲੀ
xxdline(FILE *fp, ਅੱਖਰ *l, ਅੱਖਰ *colors, ਪੂਰਨ nz)
{
  ਸਥਿਰ ਅੱਖਰ z[LLEN_NO_COLOR+1];
  ਸਥਿਰ ਅੱਖਰ z_colors[LLEN_NO_COLOR+1];
  ਸਥਿਰ ਨਿਸ਼ਾਨੀਵਾਲਾ ਅੱਖਰ zero_ਵੇਖਿਆ = 0;

  ਜੇ (!nz && zero_ਵੇਖਿਆ == 1)
    {
      strcpy(z, l);
      ਜੇ (colors)
	  ਸਮ੍ਰਿਤੀ_ਨਕਲ(z_colors, colors, ਲੜੀ_ਲੰਬਾਈ(z));
    }

  ਜੇ (nz || !zero_ਵੇਖਿਆ++)
    {
      ਜੇ (nz)
	{
	  ਜੇ (nz < 0)
	    zero_ਵੇਖਿਆ--;
	  ਜੇ (zero_ਵੇਖਿਆ == 2)
	    print_colored_ਸਤਰ(fp, z, z_colors);
	  ਜੇ (zero_ਵੇਖਿਆ > 2)
	    fputs_or_die("*\n", fp);
	}
      ਜੇ (nz >= 0 || zero_ਵੇਖਿਆ > 0)
	print_colored_ਸਤਰ(fp, l, colors);

      ਜੇ (nz)
	zero_ਵੇਖਿਆ = 0;
    }

  /* If zero_seen > 3, then its exact value doesn't matter, so long as it
   * remains >3 and incrementing it will not cause overflow. */
  ਜੇ (zero_ਵੇਖਿਆ >= 0x7F)
    zero_ਵੇਖਿਆ = 4;
}

/* This is an EBCDIC to ASCII conversion table */
/* from a proposed BTL standard April 16, 1979 */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ etoa64[] =
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

  ਸਥਿਰ ਅੱਖਰ
get_color_char (ਪੂਰਨ e, ਪੂਰਨ ebcdic)
{
  ਜੇ (ebcdic)
    {
      ਜੇ ((e >= 75 && e <= 80) || (e >= 90 && e <= 97) ||
	  (e >= 107 && e <= 111) || (e >= 121 && e <= 127) ||
	  (e >= 129 && e <= 137) || (e >= 145 && e <= 154) ||
	  (e >= 162 && e <= 169) || (e >= 192 && e <= 201) ||
	  (e >= 208 && e <= 217) || (e >= 226 && e <= 233) ||
	  (e >= 240 && e <= 249) || (e == 189) || (e == 64) ||
	  (e == 173) || (e == 224) )
	ਵਾਪਸ COLOR_GREEN;

      ਨਹੀਂਤਾਂ ਜੇ (e == 37 || e == 13 || e == 5)
	ਵਾਪਸ COLOR_YELLOW;
      ਨਹੀਂਤਾਂ ਜੇ (e == 0)
	ਵਾਪਸ COLOR_WHITE;
      ਨਹੀਂਤਾਂ ਜੇ (e == 255)
	ਵਾਪਸ COLOR_BLUE;
      ਨਹੀਂਤਾਂ
	ਵਾਪਸ COLOR_RED;
    }
  ਨਹੀਂਤਾਂ  /* ASCII */
    {
#ਜੇ defined(__MVS__) && __CHARSET_LIB == 0
      ਜੇ (e >= 64)
	ਵਾਪਸ COLOR_GREEN;
#ਨਹੀਂਤਾਂ
      ਜੇ (e > 31 && e < 127)
	ਵਾਪਸ COLOR_GREEN;
#ਜੇ_ਅੰਤ

      ਨਹੀਂਤਾਂ ਜੇ (e == 9 || e == 10 || e == 13)
	ਵਾਪਸ COLOR_YELLOW;
      ਨਹੀਂਤਾਂ ਜੇ (e == 0)
	ਵਾਪਸ COLOR_WHITE;
      ਨਹੀਂਤਾਂ ਜੇ (e == 255)
	ਵਾਪਸ COLOR_BLUE;
      ਨਹੀਂਤਾਂ
	ਵਾਪਸ COLOR_RED;
    }
    ਵਾਪਸ 0;
}

  ਸਥਿਰ ਪੂਰਨ
enable_color(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
  DWORD   ਬਹੁਲਕ;
  HANDLE  ਬਾਹਰਲਾ;

  ਜੇ (!isatty(1))
    ਵਾਪਸ 0;

  ਬਾਹਰਲਾ = GetStdHandle(STD_OUTPUT_HANDLE);
  Get_Console_ਬਹੁਲਕ(ਬਾਹਰਲਾ, &ਬਹੁਲਕ);
  ਬਹੁਲਕ |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  ਵਾਪਸ (ਪੂਰਨ)ਸੈੱਟ_Console_ਬਹੁਲਕ(ਬਾਹਰਲਾ, ਬਹੁਲਕ);
#ਜਾਂਜੇ defined(UNIX)
  ਅੱਖਰ *ਪਦ;

  ਜੇ (!isatty(STDOUT_FILENO))
    ਵਾਪਸ 0;

  ਪਦ = getenv("TERM");
  ਜੇ (ਪਦ == ਨਿਰਾ || *ਪਦ == '\0' || !strcmp(ਪਦ, "dumb"))
    ਵਾਪਸ 0;

  ਵਾਪਸ 1;
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}

  ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
  FILE *fp, *fpo;
  ਪੂਰਨ c, e, p = 0, relseek = 1, negseek = 0, revert = 0, i, x;
  ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ = 0, colsgiven = 0, nonzero = 0, autoskip = 0, hextype = HEX_ਸਾਧਾਰਨ;
  ਪੂਰਨ capitalize = 0, decimal_ਵਿੱਥ = 0;
  ਪੂਰਨ ebcdic = 0;
  ਪੂਰਨ termination = 0;
  ਪੂਰਨ octspergrp = -1;	/* number of octets grouped in output */
  ਪੂਰਨ grplen;		/* total chars per octet group excluding colors */
  ਲੰਮਾ ਲੰਮਾਈ = -1, n = 0, seekoff = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ displayoff = 0;
  ਸਥਿਰ ਅੱਖਰ l[LLEN_NO_COLOR+1];  /* static because it may be too big for stack */
  ਸਥਿਰ ਅੱਖਰ colors[LLEN_NO_COLOR+1]; /* color array */
  ਅੱਖਰ *pp;
  ਅੱਖਰ *varname = ਨਿਰਾ;
  ਪੂਰਨ addrlen = 9;
  ਪੂਰਨ color = 0;
  ਪੂਰਨ color_forced = 0;	/* set when -R always is used */
  ਅੱਖਰ *no_color;
  ਅੱਖਰ cur_color = 0;

  no_color = getenv("NO_COLOR");
  ਜੇ (no_color == ਨਿਰਾ || no_color[0] == '\0')
    color = enable_color();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
  /* This program doesn't work when started from the Workbench */
  ਜੇ (argc == 0)
    ਬਾਹਰ_ਨਿਕਲੋ(1);
#ਜੇ_ਅੰਤ

  pname = argv[0];
  ਲਈ (pp = pname; *pp; )
    ਜੇ (*pp++ == ਰਸਤਾ_SEP)
      pname = pp;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FILE_SEP
  ਲਈ (pp = pname; *pp; pp++)
    ਜੇ (*pp == FILE_SEP)
      {
	*pp = '\0';
	ਰੋਕੋ;
      }
#ਜੇ_ਅੰਤ

  ਜਦੋਂ (argc >= 2)
    {
      pp = argv[1] + (!STRNCMP(argv[1], "--", 2) && argv[1][2]);
	   ਜੇ (!STRNCMP(pp, "-a", 2)) autoskip = 1 - autoskip;
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-b", 2)) hextype |= HEX_ਬਿੱਟ;
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-e", 2)) hextype |= HEX_LITTLEENDIAN;
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-u", 2)) hexx = hexxa + 16;
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-p", 2)) hextype |= HEX_POSTSCRIPT;
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-i", 2)) hextype |= HEX_CINCLUDE;
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-C", 2)) capitalize = 1;
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-d", 2)) decimal_ਵਿੱਥ = 1;
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-r", 2)) revert++;
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-E", 2)) ebcdic++;
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-t", 2)) termination++;
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-v", 2))
	{
	  fprintf(stderr, "%s%s\n", ਸੰਸਕਰਣ_2, osver);
	  ਬਾਹਰ_ਨਿਕਲੋ(0);
	}
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-c", 2))
	{
	  ਜੇ (pp[2] && !STRNCMP("apitalize", pp + 2, 9))
	    capitalize = 1;
	  ਨਹੀਂਤਾਂ ਜੇ (pp[2] && STRNCMP("ols", pp + 2, 3))
	    {
	      colsgiven = 1;
	      ਖਾਨੇ_ਗਿਣਤੀ = (ਪੂਰਨ)strtol(pp + 2, ਨਿਰਾ, 0);
	    }
	  ਨਹੀਂਤਾਂ
	    {
	      ਜੇ (!argv[2])
		exit_with_usage();
	      colsgiven = 1;
	      ਖਾਨੇ_ਗਿਣਤੀ = (ਪੂਰਨ)strtol(argv[2], ਨਿਰਾ, 0);
	      argv++;
	      argc--;
	    }
	}
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-g", 2))
	{
	  ਜੇ (pp[2] && STRNCMP("roup", pp + 2, 4))
	    octspergrp = (ਪੂਰਨ)strtol(pp + 2, ਨਿਰਾ, 0);
	  ਨਹੀਂਤਾਂ
	    {
	      ਜੇ (!argv[2])
		exit_with_usage();
	      octspergrp = (ਪੂਰਨ)strtol(argv[2], ਨਿਰਾ, 0);
	      argv++;
	      argc--;
	    }
	}
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-o", 2))
	{
	  ਪੂਰਨ reloffset = 0;
	  ਪੂਰਨ negoffset = 0;
	  ਜੇ (pp[2] && STRNCMP("ffset", pp + 2, 5))
	    displayoff = strtoul(pp + 2, ਨਿਰਾ, 0);
	  ਨਹੀਂਤਾਂ
	    {
	      ਜੇ (!argv[2])
		exit_with_usage();

	      ਜੇ (argv[2][0] == '+')
	       reloffset++;
	     ਜੇ (argv[2][reloffset] == '-')
	       negoffset++;

	     ਜੇ (negoffset)
	       displayoff = ULONG_MAX - strtoul(argv[2] + reloffset+negoffset, ਨਿਰਾ, 0) + 1;
	     ਨਹੀਂਤਾਂ
	       displayoff = strtoul(argv[2] + reloffset+negoffset, ਨਿਰਾ, 0);

	      argv++;
	      argc--;
	    }
	}
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-s", 2))
	{
	  relseek = 0;
	  negseek = 0;
	  ਜੇ (pp[2] && STRNCMP("kip", pp+2, 3) && STRNCMP("eek", pp+2, 3))
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRY_SEEK
	      ਜੇ (pp[2] == '+')
		relseek++;
	      ਜੇ (pp[2+relseek] == '-')
		negseek++;
#ਜੇ_ਅੰਤ
	      seekoff = strtol(pp + 2+relseek+negseek, (ਅੱਖਰ **)ਨਿਰਾ, 0);
	    }
	  ਨਹੀਂਤਾਂ
	    {
	      ਜੇ (!argv[2])
		exit_with_usage();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRY_SEEK
	      ਜੇ (argv[2][0] == '+')
		relseek++;
	      ਜੇ (argv[2][relseek] == '-')
		negseek++;
#ਜੇ_ਅੰਤ
	      seekoff = strtol(argv[2] + relseek+negseek, (ਅੱਖਰ **)ਨਿਰਾ, 0);
	      argv++;
	      argc--;
	    }
	}
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-l", 2))
	{
	  ਜੇ (pp[2] && STRNCMP("en", pp + 2, 2))
	    ਲੰਮਾਈ = strtol(pp + 2, (ਅੱਖਰ **)ਨਿਰਾ, 0);
	  ਨਹੀਂਤਾਂ
	    {
	      ਜੇ (!argv[2])
		exit_with_usage();
	      ਲੰਮਾਈ = strtol(argv[2], (ਅੱਖਰ **)ਨਿਰਾ, 0);
	      argv++;
	      argc--;
	    }
	}
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-n", 2))
	{
	  ਜੇ (pp[2] && STRNCMP("ame", pp + 2, 3))
	    varname = pp + 2;
	  ਨਹੀਂਤਾਂ
	    {
	      ਜੇ (!argv[2])
		exit_with_usage();
	      varname = argv[2];
	      argv++;
	      argc--;
	    }
	}
      ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pp, "-R", 2))
	{
	  ਅੱਖਰ *pw = pp + 2;
	  ਜੇ (!pw[0])
	    {
	      pw = argv[2];
	      argv++;
	      argc--;
	    }
	  ਜੇ (!pw)
	    exit_with_usage();
	  ਜੇ (!STRNCMP(pw, "always", 6))
	    {
	      (ਖਾਲੀ)enable_color();
	      color = 1;
	      color_forced = 1;
	    }
	  ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pw, "never", 5))
	    color = 0;
	  ਨਹੀਂਤਾਂ ਜੇ (!STRNCMP(pw, "auto", 4))
	    color = enable_color();
	  ਨਹੀਂਤਾਂ
	    exit_with_usage();
	}
      ਨਹੀਂਤਾਂ ਜੇ (!strcmp(argv[1], "--"))	/* end of options */
	{
	  argv++;
	  argc--;
	  ਰੋਕੋ;
	}
      ਨਹੀਂਤਾਂ ਜੇ (pp[0] == '-' && pp[1])	/* unknown option */
	exit_with_usage();
      ਨਹੀਂਤਾਂ
	ਰੋਕੋ;				/* not an option */

      argv++;				/* advance to next argument */
      argc--;
    }

  ਜੇ (hextype != (HEX_CINCLUDE | HEX_ਬਿੱਟ))
    {
	/* Allow at most one bit to be set in hextype */
	ਜੇ (hextype & (hextype - 1))
	    error_exit(1, "only one of -b, -e, -u, -p, -i can be used");
    }

  ਜੇ (!colsgiven || (!ਖਾਨੇ_ਗਿਣਤੀ && hextype != HEX_POSTSCRIPT))
    ਚੋਣ (hextype)
      {
      ਹਾਲਤ HEX_POSTSCRIPT:	ਖਾਨੇ_ਗਿਣਤੀ = 30; ਰੋਕੋ;
      ਹਾਲਤ HEX_CINCLUDE:	ਖਾਨੇ_ਗਿਣਤੀ = 12; ਰੋਕੋ;
      ਹਾਲਤ HEX_CINCLUDE | HEX_ਬਿੱਟ:
      ਹਾਲਤ HEX_ਬਿੱਟ:		ਖਾਨੇ_ਗਿਣਤੀ = 6; ਰੋਕੋ;
      ਹਾਲਤ HEX_ਸਾਧਾਰਨ:
      ਹਾਲਤ HEX_LITTLEENDIAN:
      ਮੂਲ:			ਖਾਨੇ_ਗਿਣਤੀ = 16; ਰੋਕੋ;
      }

  ਜੇ (octspergrp < 0)
    ਚੋਣ (hextype)
      {
      ਹਾਲਤ HEX_CINCLUDE | HEX_ਬਿੱਟ:
      ਹਾਲਤ HEX_ਬਿੱਟ:		octspergrp = 1; ਰੋਕੋ;
      ਹਾਲਤ HEX_ਸਾਧਾਰਨ:		octspergrp = 2; ਰੋਕੋ;
      ਹਾਲਤ HEX_LITTLEENDIAN:	octspergrp = 4; ਰੋਕੋ;
      ਹਾਲਤ HEX_POSTSCRIPT:
      ਹਾਲਤ HEX_CINCLUDE:
      ਮੂਲ:			octspergrp = 0; ਰੋਕੋ;
      }

  ਜੇ ((hextype == HEX_POSTSCRIPT && ਖਾਨੇ_ਗਿਣਤੀ < 0) ||
      (hextype != HEX_POSTSCRIPT && ਖਾਨੇ_ਗਿਣਤੀ < 1) ||
      ((hextype == HEX_ਸਾਧਾਰਨ || hextype == HEX_ਬਿੱਟ || hextype == HEX_LITTLEENDIAN)
							    && (ਖਾਨੇ_ਗਿਣਤੀ > ਖਾਨੇ_ਗਿਣਤੀ_2)))
    {
      fprintf(stderr, "%s: invalid number of columns (max. %d).\n", pname, ਖਾਨੇ_ਗਿਣਤੀ_2);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

  ਜੇ (octspergrp < 1 || octspergrp > ਖਾਨੇ_ਗਿਣਤੀ)
    octspergrp = ਖਾਨੇ_ਗਿਣਤੀ;
  ਨਹੀਂਤਾਂ ਜੇ (hextype == HEX_LITTLEENDIAN && (octspergrp & (octspergrp-1)))
    error_exit(1, "number of octets per group must be a power of 2 with -e.");

  ਜੇ (argc > 3)
    exit_with_usage();

  ਜੇ (argc == 1 || (argv[1][0] == '-' && !argv[1][1]))
    BIN_ਸੌਂਪੋ(fp = stdin, !revert);
  ਨਹੀਂਤਾਂ
    {
      ਜੇ ((fp = fopen(argv[1], BIN_READ(!revert))) == ਨਿਰਾ)
	{
	  fprintf(stderr,"%s: ", pname);
	  perror(argv[1]);
	  ਵਾਪਸ 2;
	}
    }

  ਜੇ (argc < 3 || (argv[2][0] == '-' && !argv[2][1]))
    BIN_ਸੌਂਪੋ(fpo = stdout, revert);
  ਨਹੀਂਤਾਂ
    {
      ਪੂਰਨ fd;
      ਪੂਰਨ ਬਹੁਲਕ = revert ? O_WRONLY : (O_TRUNC|O_WRONLY);

      ਜੇ (((fd = OPEN(argv[2], ਬਹੁਲਕ | BIN_CREAT(revert), 0666)) < 0) ||
	  (fpo = fdopen(fd, BIN_WRITE(revert))) == ਨਿਰਾ)
	{
	  fprintf(stderr, "%s: ", pname);
	  perror(argv[2]);
	  ਵਾਪਸ 3;
	}
      rewind(fpo);

      /* Disable auto color when writing to a file. */
      ਜੇ (!color_forced)
	color = 0;
    }

  ਜੇ (revert)
    ਚੋਣ (hextype)
      {
      ਹਾਲਤ HEX_ਸਾਧਾਰਨ:
      ਹਾਲਤ HEX_POSTSCRIPT:
      ਹਾਲਤ HEX_ਬਿੱਟ:
	ਵਾਪਸ huntype(fp, fpo, ਖਾਨੇ_ਗਿਣਤੀ, hextype,
	  negseek ? -seekoff : seekoff);
	ਰੋਕੋ;
      ਮੂਲ:
	error_exit(-1, "Sorry, cannot revert this type of hexdump");
      }

  ਜੇ (seekoff || negseek || !relseek)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRY_SEEK
      ਜੇ (relseek)
	e = fseek(fp, negseek ? -seekoff : seekoff, SEEK_CUR);
      ਨਹੀਂਤਾਂ
	e = fseek(fp, negseek ? -seekoff : seekoff,
						negseek ? SEEK_END : SEEK_SET);
      ਜੇ (e < 0 && negseek)
	error_exit(4, "Sorry, cannot seek.");
      ਜੇ (e >= 0)
	seekoff = ftell(fp);
      ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	  ਲੰਮਾ s = seekoff;

	  ਜਦੋਂ (s--)
	    ਜੇ (getc_or_die(fp) == EOF)
	    {
	      error_exit(4, "Sorry, cannot seek.");
	    }
	}
    }

  ਜੇ (hextype & HEX_CINCLUDE)
    {
      /* A user-set variable name overrides fp == stdin */
      ਜੇ (varname == ਨਿਰਾ && fp != stdin)
	varname = argv[1];

      ਜੇ (varname != ਨਿਰਾ)
	{
	  FPRINTF_OR_DIE((fpo, "unsigned char %s", ਅੰਕ_ਹੈ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)varname[0]) ? "__" : ""));
	  ਲਈ (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(ਅੱਖਰ_ਜਾਂ_ਅੰਕ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  fputs_or_die("[] = {\n", fpo);
	}

      p = 0;
      ਜਦੋਂ ((ਲੰਮਾਈ < 0 || p < ਲੰਮਾਈ) && (((c = getc_or_die(fp)) != EOF) || termination))
	{
	  ਜੇ (c == EOF)
	    {
	      c = 0;
	      termination = -1;
	    }
	  ਜੇ (hextype & HEX_ਬਿੱਟ)
	    {
	      ਜੇ (p == 0)
		fputs_or_die("  ", fpo);
	      ਨਹੀਂਤਾਂ ਜੇ (p % ਖਾਨੇ_ਗਿਣਤੀ == 0)
		fputs_or_die(",\n  ", fpo);
	      ਨਹੀਂਤਾਂ
		fputs_or_die(", ", fpo);

	      FPRINTF_OR_DIE((fpo, "0b"));
	      ਲਈ (ਪੂਰਨ j = 7; j >= 0; j--)
		putc_or_die((c & (1 << j)) ? '1' : '0', fpo);
	      p++;
	    }
	  ਨਹੀਂਤਾਂ
	    {
	      FPRINTF_OR_DIE((fpo, (hexx == hexxa) ? "%s0x%02x" : "%s0X%02X",
		(p % ਖਾਨੇ_ਗਿਣਤੀ) ? ", " : (!p ? "  " : ",\n  "), c));
	      p++;
	    }
	  ਜੇ (termination == -1)
	    {
	      --p;
	      ਰੋਕੋ ;
	    }
	}

      ਜੇ (p)
	fputs_or_die("\n", fpo);

      ਜੇ (varname != ਨਿਰਾ)
	{
	  fputs_or_die("};\n", fpo);
	  FPRINTF_OR_DIE((fpo, "unsigned int %s", ਅੰਕ_ਹੈ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)varname[0]) ? "__" : ""));
	  ਲਈ (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(ਅੱਖਰ_ਜਾਂ_ਅੰਕ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  FPRINTF_OR_DIE((fpo, "_%s = %d;\n", capitalize ? "LEN" : "len", p));
	}

      fclose_or_die(fp, fpo);
      ਵਾਪਸ 0;
    }

  ਜੇ (hextype == HEX_POSTSCRIPT)
    {
      p = ਖਾਨੇ_ਗਿਣਤੀ;
      ਜਦੋਂ ((ਲੰਮਾਈ < 0 || n < ਲੰਮਾਈ) && (e = getc_or_die(fp)) != EOF)
	{
	  putc_or_die(hexx[(e >> 4) & 0xf], fpo);
	  putc_or_die(hexx[e & 0xf], fpo);
	  n++;
	  ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ > 0 && !--p)
	    {
	      putc_or_die('\n', fpo);
	      p = ਖਾਨੇ_ਗਿਣਤੀ;
	    }
	}
      ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ == 0 || p < ਖਾਨੇ_ਗਿਣਤੀ)
	putc_or_die('\n', fpo);
      fclose_or_die(fp, fpo);
      ਵਾਪਸ 0;
    }

  /* hextype: HEX_NORMAL or HEX_BITS or HEX_LITTLEENDIAN */

  ਜੇ (hextype != HEX_ਬਿੱਟ)
    {
      grplen = octspergrp + octspergrp + 1;	/* chars per octet group */
    }
  ਨਹੀਂਤਾਂ	/* hextype == HEX_BITS */
    grplen = 8 * octspergrp + 1;

  ਜਦੋਂ ((ਲੰਮਾਈ < 0 || n < ਲੰਮਾਈ) && (e = getc_or_die(fp)) != EOF)
    {
      ਜੇ (p == 0)
	{
	  addrlen = sprintf(l, decimal_ਵਿੱਥ ? "%08lu:" : "%08lx:",
				  ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(n + seekoff + displayoff)));
	  ਲਈ (c = addrlen; c < LLEN_NO_COLOR; l[c++] = ' ')
	    ;
	}
      x = hextype == HEX_LITTLEENDIAN ? p ^ (octspergrp-1) : p;
      c = addrlen + 1 + (grplen * x) / octspergrp;
      ਜੇ (hextype == HEX_ਸਾਧਾਰਨ || hextype == HEX_LITTLEENDIAN)
	{
	  ਜੇ (color)
	    {
	      cur_color = get_color_char(e, ebcdic);
	      colors[c] = cur_color;
	      colors[c+1] = cur_color;
	    }

	  l[c]   = hexx[(e >> 4) & 0xf];
	  l[++c] = hexx[e & 0xf];
	}
      ਨਹੀਂਤਾਂ /* hextype == HEX_BITS */
	{
	  ਜੇ (color)
	    cur_color = get_color_char(e, ebcdic);

	  ਲਈ (i = 7; i >= 0; i--)
	    {
	      ਜੇ (color)
		colors[c] = cur_color;
	      l[c++] = (e & (1 << i)) ? '1' : '0';
	    }
	}
      ਜੇ (e)
	nonzero++;
      /* When changing this update definition of LLEN and LLEN_NO_COLOR above. */
      ਜੇ (hextype == HEX_LITTLEENDIAN)
	/* last group will be fully used, round up */
	c = grplen * ((ਖਾਨੇ_ਗਿਣਤੀ + octspergrp - 1) / octspergrp);
      ਨਹੀਂਤਾਂ
	c = (grplen * ਖਾਨੇ_ਗਿਣਤੀ - 1) / octspergrp;


      ਜੇ (ebcdic)
	e = (e < 64) ? '.' : etoa64[e-64];

      ਜੇ (hextype == HEX_LITTLEENDIAN)
	c -= 1;

      c += addrlen + 3 + p;
      ਜੇ (color)
	  colors[c] = cur_color;
      l[c++] =
#ਜੇ defined(__MVS__) && __CHARSET_LIB == 0
	  (e >= 64)
#ਨਹੀਂਤਾਂ
	  (e > 31 && e < 127)
#ਜੇ_ਅੰਤ
	  ? e : '.';
      n++;
      ਜੇ (++p == ਖਾਨੇ_ਗਿਣਤੀ)
	{
	  l[c++] = '\n';
	  l[c] = '\0';

	  xxdline(fpo, l, color ? colors : ਨਿਰਾ, autoskip ? nonzero : 1);
	  memset(colors, 0, c);
	  nonzero = 0;
	  p = 0;
	}
    }
  ਜੇ (p)
    {
      l[c++] = '\n';
      l[c] = '\0';
      ਜੇ (color)
	{
	  x = p;
	  ਜੇ (hextype == HEX_LITTLEENDIAN)
	    {
	      ਪੂਰਨ fill = octspergrp - (p % octspergrp);
	      ਜੇ (fill == octspergrp) fill = 0;

	      c = addrlen + 1 + (grplen * (x - (octspergrp-fill))) / octspergrp;

	      ਲਈ (i = 0; i < fill;i++)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		  x++;
		  p++;
		}
	    }

	  ਜੇ (hextype != HEX_ਬਿੱਟ)
	    {
	      c = addrlen + 1 + (grplen * x) / octspergrp;
	      c += ਖਾਨੇ_ਗਿਣਤੀ - p;
	      c += (ਖਾਨੇ_ਗਿਣਤੀ - p) / octspergrp;

	      ਲਈ (i = ਖਾਨੇ_ਗਿਣਤੀ - p; i > 0;i--)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		}
	    }
	  xxdline(fpo, l, colors, 1);
	}
      ਨਹੀਂਤਾਂ
	xxdline(fpo, l, ਨਿਰਾ, 1);
    }
  ਨਹੀਂਤਾਂ ਜੇ (autoskip)
    xxdline(fpo, l, color ? colors : ਨਿਰਾ, -1);	/* last chance to flush out suppressed lines */

  fclose_or_die(fp, fpo);
  ਵਾਪਸ 0;
}

/* vi:set ts=8 sw=4 sts=2 cino+={2 cino+=n-2 : */
