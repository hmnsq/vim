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
#ಆದರೆ _MSC_VER >= 1400
# define _CRT_SECURE_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CYGWIN) && defined(__CYGWIN__)
# define CYGWIN
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__)
# define XOPEN_ಮೂಲ 700   /* for fdopen() */
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VAXC
# include <file.h>
#ಇಲ್ಲದಿದ್ದರೆ
# include <fcntl.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(WIN32) || defined(CYGWIN)
# include <io.h>	/* for setmode() */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
# include <windows.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
# include <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಆದರೆ __MWERKS__ && !defined(BEBOX)
# include <unix.h>	/* for fdopen() on MAC */
#ಆದರೆ_ಅಂತ್ಯ


/*  This corrects the problem of missing prototypes for certain functions
 *  in some GNU installations (e.g. SunOS 4.1.x).
 *  Darren Hiebert <darren@hmi.com> (sparc-sun-sunos4.1.3_U1/2.7.2.2)
 */
#ಆದರೆ defined(__GNUC__) && defined(__STDC__)
# ifndef __USE_FIXED_PROTOTYPES__
#  define __USE_FIXED_PROTOTYPES__
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __USE_FIXED_PROTOTYPES__
/*
 * This is historic and works only if the compiler really has no prototypes:
 *
 * Include prototypes for Sun OS 4.x, when using an ANSI compiler.
 * FILE is defined on OS 4.x, not on 5.x (Solaris).
 * if __SVR4 is defined (some Solaris versions), don't include this.
 */
# ಆದರೆ defined(sun) && defined(FILE) && !defined(__SVR4) && defined(__STDC__)
#  define __P(a) a
/* excerpt from my sun_stdlib.h */
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ fprintf __P((FILE *, ಅಕ್ಷರ *, ...));
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ fputs   __P((ಅಕ್ಷರ *, FILE *));
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ _flsbuf __P((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ, FILE *));
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ _filbuf __P((FILE *));
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ fflush  __P((FILE *));
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ fclose  __P((FILE *));
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ fseek   __P((FILE *, ದೀರ್ಘ, ಪೂರ್ಣಾಂಕ));
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ rewind  __P((FILE *));

ಬಾಹ್ಯ ಶೂನ್ಯ perror __P((ಅಕ್ಷರ *));
# endif
#ಆದರೆ_ಅಂತ್ಯ

ಅಕ್ಷರ ಆವೃತ್ತಿ_2[] = "xxd 2026-06-16 by Juergen Weigert et al.";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
ಅಕ್ಷರ osver[] = " (Win32)";
#ಇಲ್ಲದಿದ್ದರೆ
ಅಕ್ಷರ osver[] = "";
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(WIN32)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "wt")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_ದ್ವಿಮಾನ) : O_CREAT)
# define BIN_ನಿಯೋಜಿಸು(fp, yes) setmode(fileno(fp), (yes) ? O_ದ್ವಿಮಾನ : O_ಪಠ್ಯ_2)
# define ಮಾರ್ಗ_SEP '\\'
#ಅಥವಾಆದರೆ defined(CYGWIN)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "w")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_ದ್ವಿಮಾನ) : O_CREAT)
# define BIN_ನಿಯೋಜಿಸು(fp, yes) ((yes) ? (ಶೂನ್ಯ) setmode(fileno(fp), O_ದ್ವಿಮಾನ) : (ಶೂನ್ಯ) (fp))
# define ಮಾರ್ಗ_SEP '/'
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef VMS
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ನಿಯೋಜಿಸು(fp, dummy) fp
#  define ಮಾರ್ಗ_SEP ']'
#  define FILE_SEP '.'
# ಇಲ್ಲದಿದ್ದರೆ
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_ನಿಯೋಜಿಸು(fp, dummy) fp
#  define ಮಾರ್ಗ_SEP '/'
# endif
#ಆದರೆ_ಅಂತ್ಯ

/* open has only to arguments on the Mac */
#ಆದರೆ __MWERKS__
# define OPEN(name, ಬಹುಲಕ, umask) open(name, ಬಹುಲಕ)
#ಇಲ್ಲದಿದ್ದರೆ
# define OPEN(name, ಬಹುಲಕ, umask) open(name, ಬಹುಲಕ, umask)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
# define STRNCMP(s1, s2, l) ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s1, s2, (size_t)l)
#ಇಲ್ಲದಿದ್ದರೆ
# define STRNCMP(s1, s2, l) ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s1, s2, l)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __P
# ಆದರೆ defined(__STDC__) || defined(WIN32)
#  define __P(a) a
# ಇಲ್ಲದಿದ್ದರೆ
#  define __P(a) ()
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು TRY_SEEK	/* attempt to use lseek, or skip forward by reading */
#ವ್ಯಾಖ್ಯಾನಿಸು ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ_2 256	/* change here, if you ever need more columns */

/*
 * LLEN is the maximum length of a line; other than the visible characters
 * we need to consider also the escape color sequence prologue/epilogue ,
 * (11 bytes for each character).
 */
#ವ್ಯಾಖ್ಯಾನಿಸು LLEN \
    (39            /* addr: ⌈log10(ULONG_MAX)⌉ if "-d" flag given. We assume ULONG_MAX = 2**128 */ \
    + 2            /* ": " */ \
    + 13 * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ_2    /* hex dump with colors */ \
    + (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ_2 - 1)   /* whitespace between groups if "-g1" option given and "-c" maxed out */ \
    + 2            /* whitespace */ \
    + 12 * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ_2    /* ASCII dump with colors */ \
    + 2)           /* "\n\0" */

/*
 * LLEN_NO_COLOR is the maximum length of a line excluding the colors.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು LLEN_NO_COLOR \
    (39            /* addr: ⌈log10(ULONG_MAX)⌉ if "-d" flag given. We assume ULONG_MAX = 2**128 */ \
    + 2            /* ": " */ \
    + 9 * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ_2     /* hex dump, worst case: bitwise output using -b */ \
    + 2            /* whitespace */ \
    + ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ_2         /* ASCII dump */ \
    + 2)           /* "\n\0" */

ಅಕ್ಷರ hexxa[] = "0123456789abcdef0123456789ABCDEF", *hexx = hexxa;

/* the different hextypes known by this program: */
#ವ್ಯಾಖ್ಯಾನಿಸು HEX_ಸಾಮಾನ್ಯ         0x00 /* no flags set */
#ವ್ಯಾಖ್ಯಾನಿಸು HEX_POSTSCRIPT     0x01
#ವ್ಯಾಖ್ಯಾನಿಸು HEX_CINCLUDE       0x02
#ವ್ಯಾಖ್ಯಾನಿಸು HEX_ಬಿಟ್ಟುಗಳು           0x04 /* not hex a dump, but bits: 01111001 */
#ವ್ಯಾಖ್ಯಾನಿಸು HEX_LITTLEENDIAN   0x08

#ವ್ಯಾಖ್ಯಾನಿಸು CONDITIONAL_CAPITALIZE(c) (capitalize ? ದೊಡ್ಡ_ಅಕ್ಷರ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)) : (c))

#ವ್ಯಾಖ್ಯಾನಿಸು COLOR_PROLOGUE(color) \
l_colored[c++] = '\033'; \
l_colored[c++] = '['; \
l_colored[c++] = '1'; \
l_colored[c++] = ';'; \
l_colored[c++] = '3'; \
l_colored[c++] = (color); \
l_colored[c++] = 'm';

#ವ್ಯಾಖ್ಯಾನಿಸು COLOR_EPILOGUE \
l_colored[c++] = '\033'; \
l_colored[c++] = '['; \
l_colored[c++] = '0'; \
l_colored[c++] = 'm';
#ವ್ಯಾಖ್ಯಾನಿಸು COLOR_RED '1'
#ವ್ಯಾಖ್ಯಾನಿಸು COLOR_GREEN '2'
#ವ್ಯಾಖ್ಯಾನಿಸು COLOR_YELLOW '3'
#ವ್ಯಾಖ್ಯಾನಿಸು COLOR_BLUE '4'
#ವ್ಯಾಖ್ಯಾನಿಸು COLOR_WHITE '7'

ಸ್ಥಿರ ಅಕ್ಷರ *pname;

  ಸ್ಥಿರ ಶೂನ್ಯ
exit_with_usage(ಶೂನ್ಯ)
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRY_SEEK
	  "[+][-]", "(or +: rel.) ");
#ಇಲ್ಲದಿದ್ದರೆ
	  "", "");
#ಆದರೆ_ಅಂತ್ಯ
  fprintf(stderr, "    -u          use upper case hex letters.\n"
		  "    -R when     colorize the output; <when> can be 'always', 'auto' or 'never'. Default: 'auto'.\n"
		  "    -v          show version: \"%s%s\".\n", ಆವೃತ್ತಿ_2, osver);
  ನಿರ್ಗಮಿಸು(1);
}

  ಸ್ಥಿರ ಶೂನ್ಯ
perror_exit(ಪೂರ್ಣಾಂಕ ret)
{
  fprintf(stderr, "%s: ", pname);
  perror(ಖಾಲಿ);
  ನಿರ್ಗಮಿಸು(ret);
}

  ಸ್ಥಿರ ಶೂನ್ಯ
error_exit(ಪೂರ್ಣಾಂಕ ret, ಅಕ್ಷರ *ಸಂದೇಶ)
{
  fprintf(stderr, "%s: %s\n", pname, ಸಂದೇಶ);
  ನಿರ್ಗಮಿಸು(ret);
}

  ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
getc_or_die(FILE *fpi)
{
  ಪೂರ್ಣಾಂಕ c = getc(fpi);
  ಆದರೆ (c == EOF && ferror(fpi))
    perror_exit(2);
  ಹಿಂತಿರುಗಿ c;
}

  ಸ್ಥಿರ ಶೂನ್ಯ
putc_or_die(ಪೂರ್ಣಾಂಕ c, FILE *fpo)
{
  ಆದರೆ (putc(c, fpo) == EOF)
    perror_exit(3);
}

  ಸ್ಥಿರ ಶೂನ್ಯ
fputs_or_die(ಅಕ್ಷರ *s, FILE *fpo)
{
  ಆದರೆ (fputs(s, fpo) == EOF)
    perror_exit(3);
}

/* Use a macro to allow for different arguments. */
#ವ್ಯಾಖ್ಯಾನಿಸು FPRINTF_OR_DIE(ವಾದಗಳು) ಆದರೆ (fprintf ವಾದಗಳು < 0) perror_exit(3)

  ಸ್ಥಿರ ಶೂನ್ಯ
fclose_or_die(FILE *fpi, FILE *fpo)
{
  ಆದರೆ (fclose(fpo) != 0)
    perror_exit(3);
  ಆದರೆ (fclose(fpi) != 0)
    perror_exit(2);
}

/*
 * If "c" is a hex digit, return the value.
 * Otherwise return -1.
 */
  ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_hex_digit(ಪೂರ್ಣಾಂಕ c)
{
  ಹಿಂತಿರುಗಿ (c >= '0' && c <= '9') ? c - '0'
	: (c >= 'a' && c <= 'f') ? c - 'a' + 10
	: (c >= 'A' && c <= 'F') ? c - 'A' + 10
	: -1;
}

/*
 * If "c" is a bin digit, return the value.
 * Otherwise return -1.
 */
  ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_bin_digit(ಪೂರ್ಣಾಂಕ c)
{
  ಹಿಂತಿರುಗಿ (c >= '0' && c <= '1') ? c - '0'
	: -1;
}

/*
 * Ignore text on "fpi" until end-of-line or end-of-file.
 * Return the '\n' or EOF character.
 * When an error is encountered exit with an error message.
 */
  ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
skip_ಗಮ್ಯ_eol(FILE *fpi, ಪೂರ್ಣಾಂಕ c)
{
  ಆಗ (c != '\n' && c != EOF)
    c = getc_or_die(fpi);
  ಹಿಂತಿರುಗಿ c;
}

/*
 * Max. cols binary characters are decoded from the input stream per line.
 * Two adjacent garbage characters after evaluated data delimit valid data.
 * Everything up to the next newline is discarded.
 *
 * The name is historic and came from 'undo type opt h'.
 */
  ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
huntype(
  FILE *fpi,
  FILE *fpo,
  ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
  ಪೂರ್ಣಾಂಕ hextype,
  ದೀರ್ಘ ತಳ_off)
{
  ಪೂರ್ಣಾಂಕ c, ign_garb = 1, n1 = -1, n2 = 0, n3 = 0, p = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, bt = 0, b = 0, bcnt = 0;
  ದೀರ್ಘ have_off = 0, want_off = 0;

  rewind(fpi);

  ಆಗ ((c = getc(fpi)) != EOF)
    {
      ಆದರೆ (c == '\r')	/* Doze style input file? */
	ಮುಂದುವರಿ;

      /* Allow multiple spaces.  This doesn't work when there is normal text
       * after the hex codes in the last line that looks like hex, thus only
       * use it for PostScript format. */
      ಆದರೆ (hextype == HEX_POSTSCRIPT && (c == ' ' || c == '\n' || c == '\t'))
	ಮುಂದುವರಿ;

      ಆದರೆ (hextype == HEX_ಸಾಮಾನ್ಯ || hextype == HEX_POSTSCRIPT)
	{
	  n3 = n2;
	  n2 = n1;

	  n1 = parse_hex_digit(c);
	  ಆದರೆ (n1 == -1 && ign_garb)
	    ಮುಂದುವರಿ;
	}
      ಇಲ್ಲದಿದ್ದರೆ /* HEX_BITS */
	{
	  n1 = parse_hex_digit(c);
	  ಆದರೆ (n1 == -1 && ign_garb)
	    ಮುಂದುವರಿ;

	  bt = parse_bin_digit(c);
	  ಆದರೆ (bt != -1)
	    {
	      /* shift via unsigned to avoid signed overflow on bad input */
	      b = (ಪೂರ್ಣಾಂಕ)(((ಚಿಹ್ನೆರಹಿತ)b << 1) | (ಚಿಹ್ನೆರಹಿತ)bt);
	      ++bcnt;
	    }
	}

      ign_garb = 0;

      ಆದರೆ ((hextype != HEX_POSTSCRIPT) && (p >= ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ))
	{
	  ಆದರೆ (hextype == HEX_ಸಾಮಾನ್ಯ)
	    {
	      ಆದರೆ (n1 < 0)
		{
		  p = 0;
		  ಮುಂದುವರಿ;
		}
	      want_off = (ದೀರ್ಘ)(((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)want_off << 4) | (ಚಿಹ್ನೆರಹಿತ)n1);
	    }
	  ಇಲ್ಲದಿದ್ದರೆ /* HEX_BITS */
	    {
	      ಆದರೆ (n1 < 0)
		{
		  p = 0;
		  bcnt = 0;
		  ಮುಂದುವರಿ;
		}
	      want_off = (ದೀರ್ಘ)(((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)want_off << 4) | (ಚಿಹ್ನೆರಹಿತ)n1);
	    }
	  ಮುಂದುವರಿ;
	}

      ಆದರೆ (ತಳ_off + want_off != have_off)
	{
	  ಆದರೆ (fflush(fpo) != 0)
	    perror_exit(3);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRY_SEEK
	  ಆದರೆ (fseek(fpo, ತಳ_off + want_off - have_off, SEEK_CUR) >= 0)
	    have_off = ತಳ_off + want_off;
#ಆದರೆ_ಅಂತ್ಯ
	  ಆದರೆ (ತಳ_off + want_off < have_off)
	    error_exit(5, "Sorry, cannot seek backwards.");
	  ಗಾಗಿ (; have_off < ತಳ_off + want_off; have_off++)
	    putc_or_die(0, fpo);
	}

      ಆದರೆ (hextype == HEX_ಸಾಮಾನ್ಯ || hextype == HEX_POSTSCRIPT)
	{
	  ಆದರೆ (n2 >= 0 && n1 >= 0)
	    {
	      putc_or_die((n2 << 4) | n1, fpo);
	      have_off++;
	      want_off++;
	      n1 = -1;
	      ಆದರೆ (!hextype && (++p >= ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ))
	      /* skip the rest of the line as garbage */
	      c = skip_ಗಮ್ಯ_eol(fpi, c);
	    }
	  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n1 < 0 && n2 < 0 && n3 < 0)
	    /* already stumbled into garbage, skip line, wait and see */
	    c = skip_ಗಮ್ಯ_eol(fpi, c);
	}
      ಇಲ್ಲದಿದ್ದರೆ /* HEX_BITS */
	{
	  ಆದರೆ (bcnt == 8)
	    {
	      putc_or_die(b, fpo);
	      have_off++;
	      want_off++;
	      b = 0;
	      bcnt = 0;
	      ಆದರೆ (++p >= ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
		/* skip the rest of the line as garbage */
		 c = skip_ಗಮ್ಯ_eol(fpi, c);
	    }
	}

      ಆದರೆ (c == '\n')
	{
	  ಆದರೆ (hextype == HEX_ಸಾಮಾನ್ಯ || hextype == HEX_ಬಿಟ್ಟುಗಳು)
	    want_off = 0;
	  p = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	  ign_garb = 1;
	}
    }
  ಆದರೆ (fflush(fpo) != 0)
    perror_exit(3);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRY_SEEK
  fseek(fpo, 0L, SEEK_END);
#ಆದರೆ_ಅಂತ್ಯ
  fclose_or_die(fpi, fpo);
  ಹಿಂತಿರುಗಿ 0;
}


/*
 * Print line l with given colors.
 */
  ಸ್ಥಿರ ಶೂನ್ಯ
print_colored_ಸಾಲು(FILE *fp, ಅಕ್ಷರ *l, ಅಕ್ಷರ *colors)
{
  ಸ್ಥಿರ ಅಕ್ಷರ l_colored[LLEN+1];

  ಆದರೆ (colors)
    {
      ಪೂರ್ಣಾಂಕ c = 0;
      ಆದರೆ (colors[0])
	{
	  COLOR_PROLOGUE(colors[0])
	}
      l_colored[c++] = l[0];
      ಪೂರ್ಣಾಂಕ i;
      ಗಾಗಿ (i = 1; l[i]; i++)
	{
	  ಆದರೆ (colors[i] != colors[i-1])
	    {
	      ಆದರೆ (colors[i-1])
		{
		  COLOR_EPILOGUE
		}
	      ಆದರೆ (colors[i])
		{
		  COLOR_PROLOGUE(colors[i])
		}
	    }
	  l_colored[c++] = l[i];
	}

      ಆದರೆ (colors[i])
	{
	  COLOR_EPILOGUE
	}
      l_colored[c++] = '\0';

      fputs_or_die(l_colored, fp);
    }
  ಇಲ್ಲದಿದ್ದರೆ
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
  ಸ್ಥಿರ ಶೂನ್ಯ
xxdline(FILE *fp, ಅಕ್ಷರ *l, ಅಕ್ಷರ *colors, ಪೂರ್ಣಾಂಕ nz)
{
  ಸ್ಥಿರ ಅಕ್ಷರ z[LLEN_NO_COLOR+1];
  ಸ್ಥಿರ ಅಕ್ಷರ z_colors[LLEN_NO_COLOR+1];
  ಸ್ಥಿರ ಚಿಹ್ನೆಯುಳ್ಳ ಅಕ್ಷರ zero_ನೋಡಿದ = 0;

  ಆದರೆ (!nz && zero_ನೋಡಿದ == 1)
    {
      strcpy(z, l);
      ಆದರೆ (colors)
	  ಸ್ಮರಣೆ_ನಕಲು(z_colors, colors, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(z));
    }

  ಆದರೆ (nz || !zero_ನೋಡಿದ++)
    {
      ಆದರೆ (nz)
	{
	  ಆದರೆ (nz < 0)
	    zero_ನೋಡಿದ--;
	  ಆದರೆ (zero_ನೋಡಿದ == 2)
	    print_colored_ಸಾಲು(fp, z, z_colors);
	  ಆದರೆ (zero_ನೋಡಿದ > 2)
	    fputs_or_die("*\n", fp);
	}
      ಆದರೆ (nz >= 0 || zero_ನೋಡಿದ > 0)
	print_colored_ಸಾಲು(fp, l, colors);

      ಆದರೆ (nz)
	zero_ನೋಡಿದ = 0;
    }

  /* If zero_seen > 3, then its exact value doesn't matter, so long as it
   * remains >3 and incrementing it will not cause overflow. */
  ಆದರೆ (zero_ನೋಡಿದ >= 0x7F)
    zero_ನೋಡಿದ = 4;
}

/* This is an EBCDIC to ASCII conversion table */
/* from a proposed BTL standard April 16, 1979 */
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ etoa64[] =
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

  ಸ್ಥಿರ ಅಕ್ಷರ
get_color_char (ಪೂರ್ಣಾಂಕ e, ಪೂರ್ಣಾಂಕ ebcdic)
{
  ಆದರೆ (ebcdic)
    {
      ಆದರೆ ((e >= 75 && e <= 80) || (e >= 90 && e <= 97) ||
	  (e >= 107 && e <= 111) || (e >= 121 && e <= 127) ||
	  (e >= 129 && e <= 137) || (e >= 145 && e <= 154) ||
	  (e >= 162 && e <= 169) || (e >= 192 && e <= 201) ||
	  (e >= 208 && e <= 217) || (e >= 226 && e <= 233) ||
	  (e >= 240 && e <= 249) || (e == 189) || (e == 64) ||
	  (e == 173) || (e == 224) )
	ಹಿಂತಿರುಗಿ COLOR_GREEN;

      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (e == 37 || e == 13 || e == 5)
	ಹಿಂತಿರುಗಿ COLOR_YELLOW;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (e == 0)
	ಹಿಂತಿರುಗಿ COLOR_WHITE;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (e == 255)
	ಹಿಂತಿರುಗಿ COLOR_BLUE;
      ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ COLOR_RED;
    }
  ಇಲ್ಲದಿದ್ದರೆ  /* ASCII */
    {
#ಆದರೆ defined(__MVS__) && __CHARSET_LIB == 0
      ಆದರೆ (e >= 64)
	ಹಿಂತಿರುಗಿ COLOR_GREEN;
#ಇಲ್ಲದಿದ್ದರೆ
      ಆದರೆ (e > 31 && e < 127)
	ಹಿಂತಿರುಗಿ COLOR_GREEN;
#ಆದರೆ_ಅಂತ್ಯ

      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (e == 9 || e == 10 || e == 13)
	ಹಿಂತಿರುಗಿ COLOR_YELLOW;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (e == 0)
	ಹಿಂತಿರುಗಿ COLOR_WHITE;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (e == 255)
	ಹಿಂತಿರುಗಿ COLOR_BLUE;
      ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ COLOR_RED;
    }
    ಹಿಂತಿರುಗಿ 0;
}

  ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
enable_color(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
  DWORD   ಬಹುಲಕ;
  HANDLE  ಹೊರಹೋಗು;

  ಆದರೆ (!isatty(1))
    ಹಿಂತಿರುಗಿ 0;

  ಹೊರಹೋಗು = GetStdHandle(STD_OUTPUT_HANDLE);
  Get_Console_ಬಹುಲಕ(ಹೊರಹೋಗು, &ಬಹುಲಕ);
  ಬಹುಲಕ |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)ಗಣ_Console_ಬಹುಲಕ(ಹೊರಹೋಗು, ಬಹುಲಕ);
#ಅಥವಾಆದರೆ defined(UNIX)
  ಅಕ್ಷರ *ಪದ;

  ಆದರೆ (!isatty(STDOUT_FILENO))
    ಹಿಂತಿರುಗಿ 0;

  ಪದ = getenv("TERM");
  ಆದರೆ (ಪದ == ಖಾಲಿ || *ಪದ == '\0' || !strcmp(ಪದ, "dumb"))
    ಹಿಂತಿರುಗಿ 0;

  ಹಿಂತಿರುಗಿ 1;
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}

  ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
  FILE *fp, *fpo;
  ಪೂರ್ಣಾಂಕ c, e, p = 0, relseek = 1, negseek = 0, revert = 0, i, x;
  ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0, colsgiven = 0, nonzero = 0, autoskip = 0, hextype = HEX_ಸಾಮಾನ್ಯ;
  ಪೂರ್ಣಾಂಕ capitalize = 0, decimal_ಸ್ಥಾನಾಂತರ = 0;
  ಪೂರ್ಣಾಂಕ ebcdic = 0;
  ಪೂರ್ಣಾಂಕ termination = 0;
  ಪೂರ್ಣಾಂಕ octspergrp = -1;	/* number of octets grouped in output */
  ಪೂರ್ಣಾಂಕ grplen;		/* total chars per octet group excluding colors */
  ದೀರ್ಘ ಉದ್ದಳತೆ = -1, n = 0, seekoff = 0;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ displayoff = 0;
  ಸ್ಥಿರ ಅಕ್ಷರ l[LLEN_NO_COLOR+1];  /* static because it may be too big for stack */
  ಸ್ಥಿರ ಅಕ್ಷರ colors[LLEN_NO_COLOR+1]; /* color array */
  ಅಕ್ಷರ *pp;
  ಅಕ್ಷರ *varname = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ addrlen = 9;
  ಪೂರ್ಣಾಂಕ color = 0;
  ಪೂರ್ಣಾಂಕ color_forced = 0;	/* set when -R always is used */
  ಅಕ್ಷರ *no_color;
  ಅಕ್ಷರ cur_color = 0;

  no_color = getenv("NO_COLOR");
  ಆದರೆ (no_color == ಖಾಲಿ || no_color[0] == '\0')
    color = enable_color();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
  /* This program doesn't work when started from the Workbench */
  ಆದರೆ (argc == 0)
    ನಿರ್ಗಮಿಸು(1);
#ಆದರೆ_ಅಂತ್ಯ

  pname = argv[0];
  ಗಾಗಿ (pp = pname; *pp; )
    ಆದರೆ (*pp++ == ಮಾರ್ಗ_SEP)
      pname = pp;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FILE_SEP
  ಗಾಗಿ (pp = pname; *pp; pp++)
    ಆದರೆ (*pp == FILE_SEP)
      {
	*pp = '\0';
	ನಿಲ್ಲಿಸು;
      }
#ಆದರೆ_ಅಂತ್ಯ

  ಆಗ (argc >= 2)
    {
      pp = argv[1] + (!STRNCMP(argv[1], "--", 2) && argv[1][2]);
	   ಆದರೆ (!STRNCMP(pp, "-a", 2)) autoskip = 1 - autoskip;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-b", 2)) hextype |= HEX_ಬಿಟ್ಟುಗಳು;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-e", 2)) hextype |= HEX_LITTLEENDIAN;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-u", 2)) hexx = hexxa + 16;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-p", 2)) hextype |= HEX_POSTSCRIPT;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-i", 2)) hextype |= HEX_CINCLUDE;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-C", 2)) capitalize = 1;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-d", 2)) decimal_ಸ್ಥಾನಾಂತರ = 1;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-r", 2)) revert++;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-E", 2)) ebcdic++;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-t", 2)) termination++;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-v", 2))
	{
	  fprintf(stderr, "%s%s\n", ಆವೃತ್ತಿ_2, osver);
	  ನಿರ್ಗಮಿಸು(0);
	}
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-c", 2))
	{
	  ಆದರೆ (pp[2] && !STRNCMP("apitalize", pp + 2, 9))
	    capitalize = 1;
	  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pp[2] && STRNCMP("ols", pp + 2, 3))
	    {
	      colsgiven = 1;
	      ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = (ಪೂರ್ಣಾಂಕ)strtol(pp + 2, ಖಾಲಿ, 0);
	    }
	  ಇಲ್ಲದಿದ್ದರೆ
	    {
	      ಆದರೆ (!argv[2])
		exit_with_usage();
	      colsgiven = 1;
	      ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = (ಪೂರ್ಣಾಂಕ)strtol(argv[2], ಖಾಲಿ, 0);
	      argv++;
	      argc--;
	    }
	}
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-g", 2))
	{
	  ಆದರೆ (pp[2] && STRNCMP("roup", pp + 2, 4))
	    octspergrp = (ಪೂರ್ಣಾಂಕ)strtol(pp + 2, ಖಾಲಿ, 0);
	  ಇಲ್ಲದಿದ್ದರೆ
	    {
	      ಆದರೆ (!argv[2])
		exit_with_usage();
	      octspergrp = (ಪೂರ್ಣಾಂಕ)strtol(argv[2], ಖಾಲಿ, 0);
	      argv++;
	      argc--;
	    }
	}
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-o", 2))
	{
	  ಪೂರ್ಣಾಂಕ reloffset = 0;
	  ಪೂರ್ಣಾಂಕ negoffset = 0;
	  ಆದರೆ (pp[2] && STRNCMP("ffset", pp + 2, 5))
	    displayoff = strtoul(pp + 2, ಖಾಲಿ, 0);
	  ಇಲ್ಲದಿದ್ದರೆ
	    {
	      ಆದರೆ (!argv[2])
		exit_with_usage();

	      ಆದರೆ (argv[2][0] == '+')
	       reloffset++;
	     ಆದರೆ (argv[2][reloffset] == '-')
	       negoffset++;

	     ಆದರೆ (negoffset)
	       displayoff = ULONG_MAX - strtoul(argv[2] + reloffset+negoffset, ಖಾಲಿ, 0) + 1;
	     ಇಲ್ಲದಿದ್ದರೆ
	       displayoff = strtoul(argv[2] + reloffset+negoffset, ಖಾಲಿ, 0);

	      argv++;
	      argc--;
	    }
	}
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-s", 2))
	{
	  relseek = 0;
	  negseek = 0;
	  ಆದರೆ (pp[2] && STRNCMP("kip", pp+2, 3) && STRNCMP("eek", pp+2, 3))
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRY_SEEK
	      ಆದರೆ (pp[2] == '+')
		relseek++;
	      ಆದರೆ (pp[2+relseek] == '-')
		negseek++;
#ಆದರೆ_ಅಂತ್ಯ
	      seekoff = strtol(pp + 2+relseek+negseek, (ಅಕ್ಷರ **)ಖಾಲಿ, 0);
	    }
	  ಇಲ್ಲದಿದ್ದರೆ
	    {
	      ಆದರೆ (!argv[2])
		exit_with_usage();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRY_SEEK
	      ಆದರೆ (argv[2][0] == '+')
		relseek++;
	      ಆದರೆ (argv[2][relseek] == '-')
		negseek++;
#ಆದರೆ_ಅಂತ್ಯ
	      seekoff = strtol(argv[2] + relseek+negseek, (ಅಕ್ಷರ **)ಖಾಲಿ, 0);
	      argv++;
	      argc--;
	    }
	}
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-l", 2))
	{
	  ಆದರೆ (pp[2] && STRNCMP("en", pp + 2, 2))
	    ಉದ್ದಳತೆ = strtol(pp + 2, (ಅಕ್ಷರ **)ಖಾಲಿ, 0);
	  ಇಲ್ಲದಿದ್ದರೆ
	    {
	      ಆದರೆ (!argv[2])
		exit_with_usage();
	      ಉದ್ದಳತೆ = strtol(argv[2], (ಅಕ್ಷರ **)ಖಾಲಿ, 0);
	      argv++;
	      argc--;
	    }
	}
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-n", 2))
	{
	  ಆದರೆ (pp[2] && STRNCMP("ame", pp + 2, 3))
	    varname = pp + 2;
	  ಇಲ್ಲದಿದ್ದರೆ
	    {
	      ಆದರೆ (!argv[2])
		exit_with_usage();
	      varname = argv[2];
	      argv++;
	      argc--;
	    }
	}
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pp, "-R", 2))
	{
	  ಅಕ್ಷರ *pw = pp + 2;
	  ಆದರೆ (!pw[0])
	    {
	      pw = argv[2];
	      argv++;
	      argc--;
	    }
	  ಆದರೆ (!pw)
	    exit_with_usage();
	  ಆದರೆ (!STRNCMP(pw, "always", 6))
	    {
	      (ಶೂನ್ಯ)enable_color();
	      color = 1;
	      color_forced = 1;
	    }
	  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pw, "never", 5))
	    color = 0;
	  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!STRNCMP(pw, "auto", 4))
	    color = enable_color();
	  ಇಲ್ಲದಿದ್ದರೆ
	    exit_with_usage();
	}
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!strcmp(argv[1], "--"))	/* end of options */
	{
	  argv++;
	  argc--;
	  ನಿಲ್ಲಿಸು;
	}
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pp[0] == '-' && pp[1])	/* unknown option */
	exit_with_usage();
      ಇಲ್ಲದಿದ್ದರೆ
	ನಿಲ್ಲಿಸು;				/* not an option */

      argv++;				/* advance to next argument */
      argc--;
    }

  ಆದರೆ (hextype != (HEX_CINCLUDE | HEX_ಬಿಟ್ಟುಗಳು))
    {
	/* Allow at most one bit to be set in hextype */
	ಆದರೆ (hextype & (hextype - 1))
	    error_exit(1, "only one of -b, -e, -u, -p, -i can be used");
    }

  ಆದರೆ (!colsgiven || (!ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ && hextype != HEX_POSTSCRIPT))
    ಆಯ್ಕೆ (hextype)
      {
      ಸ್ಥಿತಿ HEX_POSTSCRIPT:	ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 30; ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ HEX_CINCLUDE:	ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 12; ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ HEX_CINCLUDE | HEX_ಬಿಟ್ಟುಗಳು:
      ಸ್ಥಿತಿ HEX_ಬಿಟ್ಟುಗಳು:		ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 6; ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ HEX_ಸಾಮಾನ್ಯ:
      ಸ್ಥಿತಿ HEX_LITTLEENDIAN:
      ಪೂರ್ವನಿಯೋಜಿತ:			ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 16; ನಿಲ್ಲಿಸು;
      }

  ಆದರೆ (octspergrp < 0)
    ಆಯ್ಕೆ (hextype)
      {
      ಸ್ಥಿತಿ HEX_CINCLUDE | HEX_ಬಿಟ್ಟುಗಳು:
      ಸ್ಥಿತಿ HEX_ಬಿಟ್ಟುಗಳು:		octspergrp = 1; ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ HEX_ಸಾಮಾನ್ಯ:		octspergrp = 2; ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ HEX_LITTLEENDIAN:	octspergrp = 4; ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ HEX_POSTSCRIPT:
      ಸ್ಥಿತಿ HEX_CINCLUDE:
      ಪೂರ್ವನಿಯೋಜಿತ:			octspergrp = 0; ನಿಲ್ಲಿಸು;
      }

  ಆದರೆ ((hextype == HEX_POSTSCRIPT && ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < 0) ||
      (hextype != HEX_POSTSCRIPT && ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < 1) ||
      ((hextype == HEX_ಸಾಮಾನ್ಯ || hextype == HEX_ಬಿಟ್ಟುಗಳು || hextype == HEX_LITTLEENDIAN)
							    && (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ_2)))
    {
      fprintf(stderr, "%s: invalid number of columns (max. %d).\n", pname, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ_2);
      ನಿರ್ಗಮಿಸು(1);
    }

  ಆದರೆ (octspergrp < 1 || octspergrp > ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
    octspergrp = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hextype == HEX_LITTLEENDIAN && (octspergrp & (octspergrp-1)))
    error_exit(1, "number of octets per group must be a power of 2 with -e.");

  ಆದರೆ (argc > 3)
    exit_with_usage();

  ಆದರೆ (argc == 1 || (argv[1][0] == '-' && !argv[1][1]))
    BIN_ನಿಯೋಜಿಸು(fp = stdin, !revert);
  ಇಲ್ಲದಿದ್ದರೆ
    {
      ಆದರೆ ((fp = fopen(argv[1], BIN_READ(!revert))) == ಖಾಲಿ)
	{
	  fprintf(stderr,"%s: ", pname);
	  perror(argv[1]);
	  ಹಿಂತಿರುಗಿ 2;
	}
    }

  ಆದರೆ (argc < 3 || (argv[2][0] == '-' && !argv[2][1]))
    BIN_ನಿಯೋಜಿಸು(fpo = stdout, revert);
  ಇಲ್ಲದಿದ್ದರೆ
    {
      ಪೂರ್ಣಾಂಕ fd;
      ಪೂರ್ಣಾಂಕ ಬಹುಲಕ = revert ? O_WRONLY : (O_TRUNC|O_WRONLY);

      ಆದರೆ (((fd = OPEN(argv[2], ಬಹುಲಕ | BIN_CREAT(revert), 0666)) < 0) ||
	  (fpo = fdopen(fd, BIN_WRITE(revert))) == ಖಾಲಿ)
	{
	  fprintf(stderr, "%s: ", pname);
	  perror(argv[2]);
	  ಹಿಂತಿರುಗಿ 3;
	}
      rewind(fpo);

      /* Disable auto color when writing to a file. */
      ಆದರೆ (!color_forced)
	color = 0;
    }

  ಆದರೆ (revert)
    ಆಯ್ಕೆ (hextype)
      {
      ಸ್ಥಿತಿ HEX_ಸಾಮಾನ್ಯ:
      ಸ್ಥಿತಿ HEX_POSTSCRIPT:
      ಸ್ಥಿತಿ HEX_ಬಿಟ್ಟುಗಳು:
	ಹಿಂತಿರುಗಿ huntype(fp, fpo, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, hextype,
	  negseek ? -seekoff : seekoff);
	ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
	error_exit(-1, "Sorry, cannot revert this type of hexdump");
      }

  ಆದರೆ (seekoff || negseek || !relseek)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRY_SEEK
      ಆದರೆ (relseek)
	e = fseek(fp, negseek ? -seekoff : seekoff, SEEK_CUR);
      ಇಲ್ಲದಿದ್ದರೆ
	e = fseek(fp, negseek ? -seekoff : seekoff,
						negseek ? SEEK_END : SEEK_SET);
      ಆದರೆ (e < 0 && negseek)
	error_exit(4, "Sorry, cannot seek.");
      ಆದರೆ (e >= 0)
	seekoff = ftell(fp);
      ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	  ದೀರ್ಘ s = seekoff;

	  ಆಗ (s--)
	    ಆದರೆ (getc_or_die(fp) == EOF)
	    {
	      error_exit(4, "Sorry, cannot seek.");
	    }
	}
    }

  ಆದರೆ (hextype & HEX_CINCLUDE)
    {
      /* A user-set variable name overrides fp == stdin */
      ಆದರೆ (varname == ಖಾಲಿ && fp != stdin)
	varname = argv[1];

      ಆದರೆ (varname != ಖಾಲಿ)
	{
	  FPRINTF_OR_DIE((fpo, "unsigned char %s", ಅಂಕಿಯೇ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)varname[0]) ? "__" : ""));
	  ಗಾಗಿ (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(ಅಕ್ಷರವೇ_ಅಂಕಿಯೇ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  fputs_or_die("[] = {\n", fpo);
	}

      p = 0;
      ಆಗ ((ಉದ್ದಳತೆ < 0 || p < ಉದ್ದಳತೆ) && (((c = getc_or_die(fp)) != EOF) || termination))
	{
	  ಆದರೆ (c == EOF)
	    {
	      c = 0;
	      termination = -1;
	    }
	  ಆದರೆ (hextype & HEX_ಬಿಟ್ಟುಗಳು)
	    {
	      ಆದರೆ (p == 0)
		fputs_or_die("  ", fpo);
	      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p % ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ == 0)
		fputs_or_die(",\n  ", fpo);
	      ಇಲ್ಲದಿದ್ದರೆ
		fputs_or_die(", ", fpo);

	      FPRINTF_OR_DIE((fpo, "0b"));
	      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 7; j >= 0; j--)
		putc_or_die((c & (1 << j)) ? '1' : '0', fpo);
	      p++;
	    }
	  ಇಲ್ಲದಿದ್ದರೆ
	    {
	      FPRINTF_OR_DIE((fpo, (hexx == hexxa) ? "%s0x%02x" : "%s0X%02X",
		(p % ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) ? ", " : (!p ? "  " : ",\n  "), c));
	      p++;
	    }
	  ಆದರೆ (termination == -1)
	    {
	      --p;
	      ನಿಲ್ಲಿಸು ;
	    }
	}

      ಆದರೆ (p)
	fputs_or_die("\n", fpo);

      ಆದರೆ (varname != ಖಾಲಿ)
	{
	  fputs_or_die("};\n", fpo);
	  FPRINTF_OR_DIE((fpo, "unsigned int %s", ಅಂಕಿಯೇ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)varname[0]) ? "__" : ""));
	  ಗಾಗಿ (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(ಅಕ್ಷರವೇ_ಅಂಕಿಯೇ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  FPRINTF_OR_DIE((fpo, "_%s = %d;\n", capitalize ? "LEN" : "len", p));
	}

      fclose_or_die(fp, fpo);
      ಹಿಂತಿರುಗಿ 0;
    }

  ಆದರೆ (hextype == HEX_POSTSCRIPT)
    {
      p = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
      ಆಗ ((ಉದ್ದಳತೆ < 0 || n < ಉದ್ದಳತೆ) && (e = getc_or_die(fp)) != EOF)
	{
	  putc_or_die(hexx[(e >> 4) & 0xf], fpo);
	  putc_or_die(hexx[e & 0xf], fpo);
	  n++;
	  ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > 0 && !--p)
	    {
	      putc_or_die('\n', fpo);
	      p = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	    }
	}
      ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ == 0 || p < ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	putc_or_die('\n', fpo);
      fclose_or_die(fp, fpo);
      ಹಿಂತಿರುಗಿ 0;
    }

  /* hextype: HEX_NORMAL or HEX_BITS or HEX_LITTLEENDIAN */

  ಆದರೆ (hextype != HEX_ಬಿಟ್ಟುಗಳು)
    {
      grplen = octspergrp + octspergrp + 1;	/* chars per octet group */
    }
  ಇಲ್ಲದಿದ್ದರೆ	/* hextype == HEX_BITS */
    grplen = 8 * octspergrp + 1;

  ಆಗ ((ಉದ್ದಳತೆ < 0 || n < ಉದ್ದಳತೆ) && (e = getc_or_die(fp)) != EOF)
    {
      ಆದರೆ (p == 0)
	{
	  addrlen = sprintf(l, decimal_ಸ್ಥಾನಾಂತರ ? "%08lu:" : "%08lx:",
				  ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(n + seekoff + displayoff)));
	  ಗಾಗಿ (c = addrlen; c < LLEN_NO_COLOR; l[c++] = ' ')
	    ;
	}
      x = hextype == HEX_LITTLEENDIAN ? p ^ (octspergrp-1) : p;
      c = addrlen + 1 + (grplen * x) / octspergrp;
      ಆದರೆ (hextype == HEX_ಸಾಮಾನ್ಯ || hextype == HEX_LITTLEENDIAN)
	{
	  ಆದರೆ (color)
	    {
	      cur_color = get_color_char(e, ebcdic);
	      colors[c] = cur_color;
	      colors[c+1] = cur_color;
	    }

	  l[c]   = hexx[(e >> 4) & 0xf];
	  l[++c] = hexx[e & 0xf];
	}
      ಇಲ್ಲದಿದ್ದರೆ /* hextype == HEX_BITS */
	{
	  ಆದರೆ (color)
	    cur_color = get_color_char(e, ebcdic);

	  ಗಾಗಿ (i = 7; i >= 0; i--)
	    {
	      ಆದರೆ (color)
		colors[c] = cur_color;
	      l[c++] = (e & (1 << i)) ? '1' : '0';
	    }
	}
      ಆದರೆ (e)
	nonzero++;
      /* When changing this update definition of LLEN and LLEN_NO_COLOR above. */
      ಆದರೆ (hextype == HEX_LITTLEENDIAN)
	/* last group will be fully used, round up */
	c = grplen * ((ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + octspergrp - 1) / octspergrp);
      ಇಲ್ಲದಿದ್ದರೆ
	c = (grplen * ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - 1) / octspergrp;


      ಆದರೆ (ebcdic)
	e = (e < 64) ? '.' : etoa64[e-64];

      ಆದರೆ (hextype == HEX_LITTLEENDIAN)
	c -= 1;

      c += addrlen + 3 + p;
      ಆದರೆ (color)
	  colors[c] = cur_color;
      l[c++] =
#ಆದರೆ defined(__MVS__) && __CHARSET_LIB == 0
	  (e >= 64)
#ಇಲ್ಲದಿದ್ದರೆ
	  (e > 31 && e < 127)
#ಆದರೆ_ಅಂತ್ಯ
	  ? e : '.';
      n++;
      ಆದರೆ (++p == ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	{
	  l[c++] = '\n';
	  l[c] = '\0';

	  xxdline(fpo, l, color ? colors : ಖಾಲಿ, autoskip ? nonzero : 1);
	  memset(colors, 0, c);
	  nonzero = 0;
	  p = 0;
	}
    }
  ಆದರೆ (p)
    {
      l[c++] = '\n';
      l[c] = '\0';
      ಆದರೆ (color)
	{
	  x = p;
	  ಆದರೆ (hextype == HEX_LITTLEENDIAN)
	    {
	      ಪೂರ್ಣಾಂಕ fill = octspergrp - (p % octspergrp);
	      ಆದರೆ (fill == octspergrp) fill = 0;

	      c = addrlen + 1 + (grplen * (x - (octspergrp-fill))) / octspergrp;

	      ಗಾಗಿ (i = 0; i < fill;i++)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		  x++;
		  p++;
		}
	    }

	  ಆದರೆ (hextype != HEX_ಬಿಟ್ಟುಗಳು)
	    {
	      c = addrlen + 1 + (grplen * x) / octspergrp;
	      c += ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - p;
	      c += (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - p) / octspergrp;

	      ಗಾಗಿ (i = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - p; i > 0;i--)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		}
	    }
	  xxdline(fpo, l, colors, 1);
	}
      ಇಲ್ಲದಿದ್ದರೆ
	xxdline(fpo, l, ಖಾಲಿ, 1);
    }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (autoskip)
    xxdline(fpo, l, color ? colors : ಖಾಲಿ, -1);	/* last chance to flush out suppressed lines */

  fclose_or_die(fp, fpo);
  ಹಿಂತಿರುಗಿ 0;
}

/* vi:set ts=8 sw=4 sts=2 cino+={2 cino+=n-2 : */
