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
#اگر _MSC_VER >= 1400
# define _CRT_SECURE_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#اگر_ختم
#اگر !defined(CYGWIN) && defined(__CYGWIN__)
# define CYGWIN
#اگر_ختم

#اگر (defined(__linux__) && !defined(__ANDROID__)) || defined(__CYGWIN__)
# define XOPEN_ماخذ 700   /* for fdopen() */
#اگر_ختم

#شامل <stdio.h>
#اگر_تعریف_شدہ VAXC
# include <file.h>
#ورنہ
# include <fcntl.h>
#اگر_ختم
#اگر defined(WIN32) || defined(CYGWIN)
# include <io.h>	/* for setmode() */
#اگر_ختم
#اگر_تعریف_شدہ WIN32
# include <windows.h>
#اگر_ختم
#اگر_تعریف_شدہ UNIX
# include <unistd.h>
#اگر_ختم
#شامل <stdlib.h>
#شامل <string.h>
#شامل <ctype.h>
#شامل <limits.h>
#اگر __MWERKS__ && !defined(BEBOX)
# include <unix.h>	/* for fdopen() on MAC */
#اگر_ختم


/*  This corrects the problem of missing prototypes for certain functions
 *  in some GNU installations (e.g. SunOS 4.1.x).
 *  Darren Hiebert <darren@hmi.com> (sparc-sun-sunos4.1.3_U1/2.7.2.2)
 */
#اگر defined(__GNUC__) && defined(__STDC__)
# ifndef __USE_FIXED_PROTOTYPES__
#  define __USE_FIXED_PROTOTYPES__
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ __USE_FIXED_PROTOTYPES__
/*
 * This is historic and works only if the compiler really has no prototypes:
 *
 * Include prototypes for Sun OS 4.x, when using an ANSI compiler.
 * FILE is defined on OS 4.x, not on 5.x (Solaris).
 * if __SVR4 is defined (some Solaris versions), don't include this.
 */
# اگر defined(sun) && defined(FILE) && !defined(__SVR4) && defined(__STDC__)
#  define __P(a) a
/* excerpt from my sun_stdlib.h */
خارجی صحیح fprintf __P((FILE *, حرف *, ...));
خارجی صحیح fputs   __P((حرف *, FILE *));
خارجی صحیح _flsbuf __P((غیرعلامتی حرف, FILE *));
خارجی صحیح _filbuf __P((FILE *));
خارجی صحیح fflush  __P((FILE *));
خارجی صحیح fclose  __P((FILE *));
خارجی صحیح fseek   __P((FILE *, طویل, صحیح));
خارجی صحیح rewind  __P((FILE *));

خارجی خالی perror __P((حرف *));
# endif
#اگر_ختم

حرف نسخہ_2[] = "xxd 2026-06-16 by Juergen Weigert et al.";
#اگر_تعریف_شدہ WIN32
حرف osver[] = " (Win32)";
#ورنہ
حرف osver[] = "";
#اگر_ختم

#اگر defined(WIN32)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "wt")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_ثنائی) : O_CREAT)
# define BIN_تفویض(fp, yes) setmode(fileno(fp), (yes) ? O_ثنائی : O_متن_2)
# define راستہ_SEP '\\'
#وگرنہ_اگر defined(CYGWIN)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "w")
# define BIN_CREAT(yes) ((yes) ? (O_CREAT|O_ثنائی) : O_CREAT)
# define BIN_تفویض(fp, yes) ((yes) ? (خالی) setmode(fileno(fp), O_ثنائی) : (خالی) (fp))
# define راستہ_SEP '/'
#ورنہ
# ifdef VMS
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_تفویض(fp, dummy) fp
#  define راستہ_SEP ']'
#  define FILE_SEP '.'
# ورنہ
#  define BIN_READ(dummy)  "r"
#  define BIN_WRITE(dummy) "w"
#  define BIN_CREAT(dummy) O_CREAT
#  define BIN_تفویض(fp, dummy) fp
#  define راستہ_SEP '/'
# endif
#اگر_ختم

/* open has only to arguments on the Mac */
#اگر __MWERKS__
# define OPEN(name, منوال, umask) open(name, منوال)
#ورنہ
# define OPEN(name, منوال, umask) open(name, منوال, umask)
#اگر_ختم

#اگر_تعریف_شدہ AMIGA
# define STRNCMP(s1, s2, l) لڑی_موازنہ(s1, s2, (size_t)l)
#ورنہ
# define STRNCMP(s1, s2, l) لڑی_موازنہ(s1, s2, l)
#اگر_ختم

#اگر_غیر_تعریف_شدہ __P
# اگر defined(__STDC__) || defined(WIN32)
#  define __P(a) a
# ورنہ
#  define __P(a) ()
# endif
#اگر_ختم

#تعریف_کرو TRY_SEEK	/* attempt to use lseek, or skip forward by reading */
#تعریف_کرو ستون_تعداد_2 256	/* change here, if you ever need more columns */

/*
 * LLEN is the maximum length of a line; other than the visible characters
 * we need to consider also the escape color sequence prologue/epilogue ,
 * (11 bytes for each character).
 */
#تعریف_کرو LLEN \
    (39            /* addr: ⌈log10(ULONG_MAX)⌉ if "-d" flag given. We assume ULONG_MAX = 2**128 */ \
    + 2            /* ": " */ \
    + 13 * ستون_تعداد_2    /* hex dump with colors */ \
    + (ستون_تعداد_2 - 1)   /* whitespace between groups if "-g1" option given and "-c" maxed out */ \
    + 2            /* whitespace */ \
    + 12 * ستون_تعداد_2    /* ASCII dump with colors */ \
    + 2)           /* "\n\0" */

/*
 * LLEN_NO_COLOR is the maximum length of a line excluding the colors.
 */
#تعریف_کرو LLEN_NO_COLOR \
    (39            /* addr: ⌈log10(ULONG_MAX)⌉ if "-d" flag given. We assume ULONG_MAX = 2**128 */ \
    + 2            /* ": " */ \
    + 9 * ستون_تعداد_2     /* hex dump, worst case: bitwise output using -b */ \
    + 2            /* whitespace */ \
    + ستون_تعداد_2         /* ASCII dump */ \
    + 2)           /* "\n\0" */

حرف hexxa[] = "0123456789abcdef0123456789ABCDEF", *hexx = hexxa;

/* the different hextypes known by this program: */
#تعریف_کرو HEX_معمول         0x00 /* no flags set */
#تعریف_کرو HEX_POSTSCRIPT     0x01
#تعریف_کرو HEX_CINCLUDE       0x02
#تعریف_کرو HEX_بٹ           0x04 /* not hex a dump, but bits: 01111001 */
#تعریف_کرو HEX_LITTLEENDIAN   0x08

#تعریف_کرو CONDITIONAL_CAPITALIZE(c) (capitalize ? بڑا_حرف((غیرعلامتی حرف)(c)) : (c))

#تعریف_کرو COLOR_PROLOGUE(color) \
l_colored[c++] = '\033'; \
l_colored[c++] = '['; \
l_colored[c++] = '1'; \
l_colored[c++] = ';'; \
l_colored[c++] = '3'; \
l_colored[c++] = (color); \
l_colored[c++] = 'm';

#تعریف_کرو COLOR_EPILOGUE \
l_colored[c++] = '\033'; \
l_colored[c++] = '['; \
l_colored[c++] = '0'; \
l_colored[c++] = 'm';
#تعریف_کرو COLOR_RED '1'
#تعریف_کرو COLOR_GREEN '2'
#تعریف_کرو COLOR_YELLOW '3'
#تعریف_کرو COLOR_BLUE '4'
#تعریف_کرو COLOR_WHITE '7'

ساکن حرف *pname;

  ساکن خالی
exit_with_usage(خالی)
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
#اگر_تعریف_شدہ TRY_SEEK
	  "[+][-]", "(or +: rel.) ");
#ورنہ
	  "", "");
#اگر_ختم
  fprintf(stderr, "    -u          use upper case hex letters.\n"
		  "    -R when     colorize the output; <when> can be 'always', 'auto' or 'never'. Default: 'auto'.\n"
		  "    -v          show version: \"%s%s\".\n", نسخہ_2, osver);
  باہر_نکلو(1);
}

  ساکن خالی
perror_exit(صحیح ret)
{
  fprintf(stderr, "%s: ", pname);
  perror(تہی);
  باہر_نکلو(ret);
}

  ساکن خالی
error_exit(صحیح ret, حرف *پیغام)
{
  fprintf(stderr, "%s: %s\n", pname, پیغام);
  باہر_نکلو(ret);
}

  ساکن صحیح
getc_or_die(FILE *fpi)
{
  صحیح c = getc(fpi);
  اگر (c == EOF && ferror(fpi))
    perror_exit(2);
  واپس c;
}

  ساکن خالی
putc_or_die(صحیح c, FILE *fpo)
{
  اگر (putc(c, fpo) == EOF)
    perror_exit(3);
}

  ساکن خالی
fputs_or_die(حرف *s, FILE *fpo)
{
  اگر (fputs(s, fpo) == EOF)
    perror_exit(3);
}

/* Use a macro to allow for different arguments. */
#تعریف_کرو FPRINTF_OR_DIE(دلائل) اگر (fprintf دلائل < 0) perror_exit(3)

  ساکن خالی
fclose_or_die(FILE *fpi, FILE *fpo)
{
  اگر (fclose(fpo) != 0)
    perror_exit(3);
  اگر (fclose(fpi) != 0)
    perror_exit(2);
}

/*
 * If "c" is a hex digit, return the value.
 * Otherwise return -1.
 */
  ساکن صحیح
parse_hex_digit(صحیح c)
{
  واپس (c >= '0' && c <= '9') ? c - '0'
	: (c >= 'a' && c <= 'f') ? c - 'a' + 10
	: (c >= 'A' && c <= 'F') ? c - 'A' + 10
	: -1;
}

/*
 * If "c" is a bin digit, return the value.
 * Otherwise return -1.
 */
  ساکن صحیح
parse_bin_digit(صحیح c)
{
  واپس (c >= '0' && c <= '1') ? c - '0'
	: -1;
}

/*
 * Ignore text on "fpi" until end-of-line or end-of-file.
 * Return the '\n' or EOF character.
 * When an error is encountered exit with an error message.
 */
  ساکن صحیح
skip_مقصد_eol(FILE *fpi, صحیح c)
{
  جب (c != '\n' && c != EOF)
    c = getc_or_die(fpi);
  واپس c;
}

/*
 * Max. cols binary characters are decoded from the input stream per line.
 * Two adjacent garbage characters after evaluated data delimit valid data.
 * Everything up to the next newline is discarded.
 *
 * The name is historic and came from 'undo type opt h'.
 */
  ساکن صحیح
huntype(
  FILE *fpi,
  FILE *fpo,
  صحیح ستون_تعداد,
  صحیح hextype,
  طویل بنیاد_off)
{
  صحیح c, ign_garb = 1, n1 = -1, n2 = 0, n3 = 0, p = ستون_تعداد, bt = 0, b = 0, bcnt = 0;
  طویل have_off = 0, want_off = 0;

  rewind(fpi);

  جب ((c = getc(fpi)) != EOF)
    {
      اگر (c == '\r')	/* Doze style input file? */
	جاری;

      /* Allow multiple spaces.  This doesn't work when there is normal text
       * after the hex codes in the last line that looks like hex, thus only
       * use it for PostScript format. */
      اگر (hextype == HEX_POSTSCRIPT && (c == ' ' || c == '\n' || c == '\t'))
	جاری;

      اگر (hextype == HEX_معمول || hextype == HEX_POSTSCRIPT)
	{
	  n3 = n2;
	  n2 = n1;

	  n1 = parse_hex_digit(c);
	  اگر (n1 == -1 && ign_garb)
	    جاری;
	}
      ورنہ /* HEX_BITS */
	{
	  n1 = parse_hex_digit(c);
	  اگر (n1 == -1 && ign_garb)
	    جاری;

	  bt = parse_bin_digit(c);
	  اگر (bt != -1)
	    {
	      /* shift via unsigned to avoid signed overflow on bad input */
	      b = (صحیح)(((غیرعلامتی)b << 1) | (غیرعلامتی)bt);
	      ++bcnt;
	    }
	}

      ign_garb = 0;

      اگر ((hextype != HEX_POSTSCRIPT) && (p >= ستون_تعداد))
	{
	  اگر (hextype == HEX_معمول)
	    {
	      اگر (n1 < 0)
		{
		  p = 0;
		  جاری;
		}
	      want_off = (طویل)(((غیرعلامتی طویل)want_off << 4) | (غیرعلامتی)n1);
	    }
	  ورنہ /* HEX_BITS */
	    {
	      اگر (n1 < 0)
		{
		  p = 0;
		  bcnt = 0;
		  جاری;
		}
	      want_off = (طویل)(((غیرعلامتی طویل)want_off << 4) | (غیرعلامتی)n1);
	    }
	  جاری;
	}

      اگر (بنیاد_off + want_off != have_off)
	{
	  اگر (fflush(fpo) != 0)
	    perror_exit(3);
#اگر_تعریف_شدہ TRY_SEEK
	  اگر (fseek(fpo, بنیاد_off + want_off - have_off, SEEK_CUR) >= 0)
	    have_off = بنیاد_off + want_off;
#اگر_ختم
	  اگر (بنیاد_off + want_off < have_off)
	    error_exit(5, "Sorry, cannot seek backwards.");
	  برائے (; have_off < بنیاد_off + want_off; have_off++)
	    putc_or_die(0, fpo);
	}

      اگر (hextype == HEX_معمول || hextype == HEX_POSTSCRIPT)
	{
	  اگر (n2 >= 0 && n1 >= 0)
	    {
	      putc_or_die((n2 << 4) | n1, fpo);
	      have_off++;
	      want_off++;
	      n1 = -1;
	      اگر (!hextype && (++p >= ستون_تعداد))
	      /* skip the rest of the line as garbage */
	      c = skip_مقصد_eol(fpi, c);
	    }
	  ورنہ اگر (n1 < 0 && n2 < 0 && n3 < 0)
	    /* already stumbled into garbage, skip line, wait and see */
	    c = skip_مقصد_eol(fpi, c);
	}
      ورنہ /* HEX_BITS */
	{
	  اگر (bcnt == 8)
	    {
	      putc_or_die(b, fpo);
	      have_off++;
	      want_off++;
	      b = 0;
	      bcnt = 0;
	      اگر (++p >= ستون_تعداد)
		/* skip the rest of the line as garbage */
		 c = skip_مقصد_eol(fpi, c);
	    }
	}

      اگر (c == '\n')
	{
	  اگر (hextype == HEX_معمول || hextype == HEX_بٹ)
	    want_off = 0;
	  p = ستون_تعداد;
	  ign_garb = 1;
	}
    }
  اگر (fflush(fpo) != 0)
    perror_exit(3);
#اگر_تعریف_شدہ TRY_SEEK
  fseek(fpo, 0L, SEEK_END);
#اگر_ختم
  fclose_or_die(fpi, fpo);
  واپس 0;
}


/*
 * Print line l with given colors.
 */
  ساکن خالی
print_colored_سطر(FILE *fp, حرف *l, حرف *colors)
{
  ساکن حرف l_colored[LLEN+1];

  اگر (colors)
    {
      صحیح c = 0;
      اگر (colors[0])
	{
	  COLOR_PROLOGUE(colors[0])
	}
      l_colored[c++] = l[0];
      صحیح i;
      برائے (i = 1; l[i]; i++)
	{
	  اگر (colors[i] != colors[i-1])
	    {
	      اگر (colors[i-1])
		{
		  COLOR_EPILOGUE
		}
	      اگر (colors[i])
		{
		  COLOR_PROLOGUE(colors[i])
		}
	    }
	  l_colored[c++] = l[i];
	}

      اگر (colors[i])
	{
	  COLOR_EPILOGUE
	}
      l_colored[c++] = '\0';

      fputs_or_die(l_colored, fp);
    }
  ورنہ
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
  ساکن خالی
xxdline(FILE *fp, حرف *l, حرف *colors, صحیح nz)
{
  ساکن حرف z[LLEN_NO_COLOR+1];
  ساکن حرف z_colors[LLEN_NO_COLOR+1];
  ساکن علامتی حرف zero_نظر_آیا = 0;

  اگر (!nz && zero_نظر_آیا == 1)
    {
      strcpy(z, l);
      اگر (colors)
	  حافظہ_نقل(z_colors, colors, لڑی_لمبائی(z));
    }

  اگر (nz || !zero_نظر_آیا++)
    {
      اگر (nz)
	{
	  اگر (nz < 0)
	    zero_نظر_آیا--;
	  اگر (zero_نظر_آیا == 2)
	    print_colored_سطر(fp, z, z_colors);
	  اگر (zero_نظر_آیا > 2)
	    fputs_or_die("*\n", fp);
	}
      اگر (nz >= 0 || zero_نظر_آیا > 0)
	print_colored_سطر(fp, l, colors);

      اگر (nz)
	zero_نظر_آیا = 0;
    }

  /* If zero_seen > 3, then its exact value doesn't matter, so long as it
   * remains >3 and incrementing it will not cause overflow. */
  اگر (zero_نظر_آیا >= 0x7F)
    zero_نظر_آیا = 4;
}

/* This is an EBCDIC to ASCII conversion table */
/* from a proposed BTL standard April 16, 1979 */
ساکن غیرعلامتی حرف etoa64[] =
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

  ساکن حرف
get_color_char (صحیح e, صحیح ebcdic)
{
  اگر (ebcdic)
    {
      اگر ((e >= 75 && e <= 80) || (e >= 90 && e <= 97) ||
	  (e >= 107 && e <= 111) || (e >= 121 && e <= 127) ||
	  (e >= 129 && e <= 137) || (e >= 145 && e <= 154) ||
	  (e >= 162 && e <= 169) || (e >= 192 && e <= 201) ||
	  (e >= 208 && e <= 217) || (e >= 226 && e <= 233) ||
	  (e >= 240 && e <= 249) || (e == 189) || (e == 64) ||
	  (e == 173) || (e == 224) )
	واپس COLOR_GREEN;

      ورنہ اگر (e == 37 || e == 13 || e == 5)
	واپس COLOR_YELLOW;
      ورنہ اگر (e == 0)
	واپس COLOR_WHITE;
      ورنہ اگر (e == 255)
	واپس COLOR_BLUE;
      ورنہ
	واپس COLOR_RED;
    }
  ورنہ  /* ASCII */
    {
#اگر defined(__MVS__) && __CHARSET_LIB == 0
      اگر (e >= 64)
	واپس COLOR_GREEN;
#ورنہ
      اگر (e > 31 && e < 127)
	واپس COLOR_GREEN;
#اگر_ختم

      ورنہ اگر (e == 9 || e == 10 || e == 13)
	واپس COLOR_YELLOW;
      ورنہ اگر (e == 0)
	واپس COLOR_WHITE;
      ورنہ اگر (e == 255)
	واپس COLOR_BLUE;
      ورنہ
	واپس COLOR_RED;
    }
    واپس 0;
}

  ساکن صحیح
enable_color(خالی)
{
#اگر_تعریف_شدہ WIN32
  DWORD   منوال;
  HANDLE  برآمد;

  اگر (!isatty(1))
    واپس 0;

  برآمد = GetStdHandle(STD_OUTPUT_HANDLE);
  Get_Console_منوال(برآمد, &منوال);
  منوال |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  واپس (صحیح)سیٹ_Console_منوال(برآمد, منوال);
#وگرنہ_اگر defined(UNIX)
  حرف *رکن;

  اگر (!isatty(STDOUT_FILENO))
    واپس 0;

  رکن = getenv("TERM");
  اگر (رکن == تہی || *رکن == '\0' || !strcmp(رکن, "dumb"))
    واپس 0;

  واپس 1;
#ورنہ
  واپس 0;
#اگر_ختم
}

  صحیح
مین(صحیح argc, حرف *argv[])
{
  FILE *fp, *fpo;
  صحیح c, e, p = 0, relseek = 1, negseek = 0, revert = 0, i, x;
  صحیح ستون_تعداد = 0, colsgiven = 0, nonzero = 0, autoskip = 0, hextype = HEX_معمول;
  صحیح capitalize = 0, decimal_انحراف = 0;
  صحیح ebcdic = 0;
  صحیح termination = 0;
  صحیح octspergrp = -1;	/* number of octets grouped in output */
  صحیح grplen;		/* total chars per octet group excluding colors */
  طویل لمبائی = -1, n = 0, seekoff = 0;
  غیرعلامتی طویل displayoff = 0;
  ساکن حرف l[LLEN_NO_COLOR+1];  /* static because it may be too big for stack */
  ساکن حرف colors[LLEN_NO_COLOR+1]; /* color array */
  حرف *pp;
  حرف *varname = تہی;
  صحیح addrlen = 9;
  صحیح color = 0;
  صحیح color_forced = 0;	/* set when -R always is used */
  حرف *no_color;
  حرف cur_color = 0;

  no_color = getenv("NO_COLOR");
  اگر (no_color == تہی || no_color[0] == '\0')
    color = enable_color();

#اگر_تعریف_شدہ AMIGA
  /* This program doesn't work when started from the Workbench */
  اگر (argc == 0)
    باہر_نکلو(1);
#اگر_ختم

  pname = argv[0];
  برائے (pp = pname; *pp; )
    اگر (*pp++ == راستہ_SEP)
      pname = pp;
#اگر_تعریف_شدہ FILE_SEP
  برائے (pp = pname; *pp; pp++)
    اگر (*pp == FILE_SEP)
      {
	*pp = '\0';
	توڑو;
      }
#اگر_ختم

  جب (argc >= 2)
    {
      pp = argv[1] + (!STRNCMP(argv[1], "--", 2) && argv[1][2]);
	   اگر (!STRNCMP(pp, "-a", 2)) autoskip = 1 - autoskip;
      ورنہ اگر (!STRNCMP(pp, "-b", 2)) hextype |= HEX_بٹ;
      ورنہ اگر (!STRNCMP(pp, "-e", 2)) hextype |= HEX_LITTLEENDIAN;
      ورنہ اگر (!STRNCMP(pp, "-u", 2)) hexx = hexxa + 16;
      ورنہ اگر (!STRNCMP(pp, "-p", 2)) hextype |= HEX_POSTSCRIPT;
      ورنہ اگر (!STRNCMP(pp, "-i", 2)) hextype |= HEX_CINCLUDE;
      ورنہ اگر (!STRNCMP(pp, "-C", 2)) capitalize = 1;
      ورنہ اگر (!STRNCMP(pp, "-d", 2)) decimal_انحراف = 1;
      ورنہ اگر (!STRNCMP(pp, "-r", 2)) revert++;
      ورنہ اگر (!STRNCMP(pp, "-E", 2)) ebcdic++;
      ورنہ اگر (!STRNCMP(pp, "-t", 2)) termination++;
      ورنہ اگر (!STRNCMP(pp, "-v", 2))
	{
	  fprintf(stderr, "%s%s\n", نسخہ_2, osver);
	  باہر_نکلو(0);
	}
      ورنہ اگر (!STRNCMP(pp, "-c", 2))
	{
	  اگر (pp[2] && !STRNCMP("apitalize", pp + 2, 9))
	    capitalize = 1;
	  ورنہ اگر (pp[2] && STRNCMP("ols", pp + 2, 3))
	    {
	      colsgiven = 1;
	      ستون_تعداد = (صحیح)strtol(pp + 2, تہی, 0);
	    }
	  ورنہ
	    {
	      اگر (!argv[2])
		exit_with_usage();
	      colsgiven = 1;
	      ستون_تعداد = (صحیح)strtol(argv[2], تہی, 0);
	      argv++;
	      argc--;
	    }
	}
      ورنہ اگر (!STRNCMP(pp, "-g", 2))
	{
	  اگر (pp[2] && STRNCMP("roup", pp + 2, 4))
	    octspergrp = (صحیح)strtol(pp + 2, تہی, 0);
	  ورنہ
	    {
	      اگر (!argv[2])
		exit_with_usage();
	      octspergrp = (صحیح)strtol(argv[2], تہی, 0);
	      argv++;
	      argc--;
	    }
	}
      ورنہ اگر (!STRNCMP(pp, "-o", 2))
	{
	  صحیح reloffset = 0;
	  صحیح negoffset = 0;
	  اگر (pp[2] && STRNCMP("ffset", pp + 2, 5))
	    displayoff = strtoul(pp + 2, تہی, 0);
	  ورنہ
	    {
	      اگر (!argv[2])
		exit_with_usage();

	      اگر (argv[2][0] == '+')
	       reloffset++;
	     اگر (argv[2][reloffset] == '-')
	       negoffset++;

	     اگر (negoffset)
	       displayoff = ULONG_MAX - strtoul(argv[2] + reloffset+negoffset, تہی, 0) + 1;
	     ورنہ
	       displayoff = strtoul(argv[2] + reloffset+negoffset, تہی, 0);

	      argv++;
	      argc--;
	    }
	}
      ورنہ اگر (!STRNCMP(pp, "-s", 2))
	{
	  relseek = 0;
	  negseek = 0;
	  اگر (pp[2] && STRNCMP("kip", pp+2, 3) && STRNCMP("eek", pp+2, 3))
	    {
#اگر_تعریف_شدہ TRY_SEEK
	      اگر (pp[2] == '+')
		relseek++;
	      اگر (pp[2+relseek] == '-')
		negseek++;
#اگر_ختم
	      seekoff = strtol(pp + 2+relseek+negseek, (حرف **)تہی, 0);
	    }
	  ورنہ
	    {
	      اگر (!argv[2])
		exit_with_usage();
#اگر_تعریف_شدہ TRY_SEEK
	      اگر (argv[2][0] == '+')
		relseek++;
	      اگر (argv[2][relseek] == '-')
		negseek++;
#اگر_ختم
	      seekoff = strtol(argv[2] + relseek+negseek, (حرف **)تہی, 0);
	      argv++;
	      argc--;
	    }
	}
      ورنہ اگر (!STRNCMP(pp, "-l", 2))
	{
	  اگر (pp[2] && STRNCMP("en", pp + 2, 2))
	    لمبائی = strtol(pp + 2, (حرف **)تہی, 0);
	  ورنہ
	    {
	      اگر (!argv[2])
		exit_with_usage();
	      لمبائی = strtol(argv[2], (حرف **)تہی, 0);
	      argv++;
	      argc--;
	    }
	}
      ورنہ اگر (!STRNCMP(pp, "-n", 2))
	{
	  اگر (pp[2] && STRNCMP("ame", pp + 2, 3))
	    varname = pp + 2;
	  ورنہ
	    {
	      اگر (!argv[2])
		exit_with_usage();
	      varname = argv[2];
	      argv++;
	      argc--;
	    }
	}
      ورنہ اگر (!STRNCMP(pp, "-R", 2))
	{
	  حرف *pw = pp + 2;
	  اگر (!pw[0])
	    {
	      pw = argv[2];
	      argv++;
	      argc--;
	    }
	  اگر (!pw)
	    exit_with_usage();
	  اگر (!STRNCMP(pw, "always", 6))
	    {
	      (خالی)enable_color();
	      color = 1;
	      color_forced = 1;
	    }
	  ورنہ اگر (!STRNCMP(pw, "never", 5))
	    color = 0;
	  ورنہ اگر (!STRNCMP(pw, "auto", 4))
	    color = enable_color();
	  ورنہ
	    exit_with_usage();
	}
      ورنہ اگر (!strcmp(argv[1], "--"))	/* end of options */
	{
	  argv++;
	  argc--;
	  توڑو;
	}
      ورنہ اگر (pp[0] == '-' && pp[1])	/* unknown option */
	exit_with_usage();
      ورنہ
	توڑو;				/* not an option */

      argv++;				/* advance to next argument */
      argc--;
    }

  اگر (hextype != (HEX_CINCLUDE | HEX_بٹ))
    {
	/* Allow at most one bit to be set in hextype */
	اگر (hextype & (hextype - 1))
	    error_exit(1, "only one of -b, -e, -u, -p, -i can be used");
    }

  اگر (!colsgiven || (!ستون_تعداد && hextype != HEX_POSTSCRIPT))
    انتخاب (hextype)
      {
      حالت HEX_POSTSCRIPT:	ستون_تعداد = 30; توڑو;
      حالت HEX_CINCLUDE:	ستون_تعداد = 12; توڑو;
      حالت HEX_CINCLUDE | HEX_بٹ:
      حالت HEX_بٹ:		ستون_تعداد = 6; توڑو;
      حالت HEX_معمول:
      حالت HEX_LITTLEENDIAN:
      طےشدہ:			ستون_تعداد = 16; توڑو;
      }

  اگر (octspergrp < 0)
    انتخاب (hextype)
      {
      حالت HEX_CINCLUDE | HEX_بٹ:
      حالت HEX_بٹ:		octspergrp = 1; توڑو;
      حالت HEX_معمول:		octspergrp = 2; توڑو;
      حالت HEX_LITTLEENDIAN:	octspergrp = 4; توڑو;
      حالت HEX_POSTSCRIPT:
      حالت HEX_CINCLUDE:
      طےشدہ:			octspergrp = 0; توڑو;
      }

  اگر ((hextype == HEX_POSTSCRIPT && ستون_تعداد < 0) ||
      (hextype != HEX_POSTSCRIPT && ستون_تعداد < 1) ||
      ((hextype == HEX_معمول || hextype == HEX_بٹ || hextype == HEX_LITTLEENDIAN)
							    && (ستون_تعداد > ستون_تعداد_2)))
    {
      fprintf(stderr, "%s: invalid number of columns (max. %d).\n", pname, ستون_تعداد_2);
      باہر_نکلو(1);
    }

  اگر (octspergrp < 1 || octspergrp > ستون_تعداد)
    octspergrp = ستون_تعداد;
  ورنہ اگر (hextype == HEX_LITTLEENDIAN && (octspergrp & (octspergrp-1)))
    error_exit(1, "number of octets per group must be a power of 2 with -e.");

  اگر (argc > 3)
    exit_with_usage();

  اگر (argc == 1 || (argv[1][0] == '-' && !argv[1][1]))
    BIN_تفویض(fp = stdin, !revert);
  ورنہ
    {
      اگر ((fp = fopen(argv[1], BIN_READ(!revert))) == تہی)
	{
	  fprintf(stderr,"%s: ", pname);
	  perror(argv[1]);
	  واپس 2;
	}
    }

  اگر (argc < 3 || (argv[2][0] == '-' && !argv[2][1]))
    BIN_تفویض(fpo = stdout, revert);
  ورنہ
    {
      صحیح fd;
      صحیح منوال = revert ? O_WRONLY : (O_TRUNC|O_WRONLY);

      اگر (((fd = OPEN(argv[2], منوال | BIN_CREAT(revert), 0666)) < 0) ||
	  (fpo = fdopen(fd, BIN_WRITE(revert))) == تہی)
	{
	  fprintf(stderr, "%s: ", pname);
	  perror(argv[2]);
	  واپس 3;
	}
      rewind(fpo);

      /* Disable auto color when writing to a file. */
      اگر (!color_forced)
	color = 0;
    }

  اگر (revert)
    انتخاب (hextype)
      {
      حالت HEX_معمول:
      حالت HEX_POSTSCRIPT:
      حالت HEX_بٹ:
	واپس huntype(fp, fpo, ستون_تعداد, hextype,
	  negseek ? -seekoff : seekoff);
	توڑو;
      طےشدہ:
	error_exit(-1, "Sorry, cannot revert this type of hexdump");
      }

  اگر (seekoff || negseek || !relseek)
    {
#اگر_تعریف_شدہ TRY_SEEK
      اگر (relseek)
	e = fseek(fp, negseek ? -seekoff : seekoff, SEEK_CUR);
      ورنہ
	e = fseek(fp, negseek ? -seekoff : seekoff,
						negseek ? SEEK_END : SEEK_SET);
      اگر (e < 0 && negseek)
	error_exit(4, "Sorry, cannot seek.");
      اگر (e >= 0)
	seekoff = ftell(fp);
      ورنہ
#اگر_ختم
	{
	  طویل s = seekoff;

	  جب (s--)
	    اگر (getc_or_die(fp) == EOF)
	    {
	      error_exit(4, "Sorry, cannot seek.");
	    }
	}
    }

  اگر (hextype & HEX_CINCLUDE)
    {
      /* A user-set variable name overrides fp == stdin */
      اگر (varname == تہی && fp != stdin)
	varname = argv[1];

      اگر (varname != تہی)
	{
	  FPRINTF_OR_DIE((fpo, "unsigned char %s", ہندسہ_ہے((غیرعلامتی حرف)varname[0]) ? "__" : ""));
	  برائے (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(حرف_یا_ہندسہ((غیرعلامتی حرف)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  fputs_or_die("[] = {\n", fpo);
	}

      p = 0;
      جب ((لمبائی < 0 || p < لمبائی) && (((c = getc_or_die(fp)) != EOF) || termination))
	{
	  اگر (c == EOF)
	    {
	      c = 0;
	      termination = -1;
	    }
	  اگر (hextype & HEX_بٹ)
	    {
	      اگر (p == 0)
		fputs_or_die("  ", fpo);
	      ورنہ اگر (p % ستون_تعداد == 0)
		fputs_or_die(",\n  ", fpo);
	      ورنہ
		fputs_or_die(", ", fpo);

	      FPRINTF_OR_DIE((fpo, "0b"));
	      برائے (صحیح j = 7; j >= 0; j--)
		putc_or_die((c & (1 << j)) ? '1' : '0', fpo);
	      p++;
	    }
	  ورنہ
	    {
	      FPRINTF_OR_DIE((fpo, (hexx == hexxa) ? "%s0x%02x" : "%s0X%02X",
		(p % ستون_تعداد) ? ", " : (!p ? "  " : ",\n  "), c));
	      p++;
	    }
	  اگر (termination == -1)
	    {
	      --p;
	      توڑو ;
	    }
	}

      اگر (p)
	fputs_or_die("\n", fpo);

      اگر (varname != تہی)
	{
	  fputs_or_die("};\n", fpo);
	  FPRINTF_OR_DIE((fpo, "unsigned int %s", ہندسہ_ہے((غیرعلامتی حرف)varname[0]) ? "__" : ""));
	  برائے (e = 0; (c = varname[e]) != 0; e++)
	    putc_or_die(حرف_یا_ہندسہ((غیرعلامتی حرف)c) ? CONDITIONAL_CAPITALIZE(c) : '_', fpo);
	  FPRINTF_OR_DIE((fpo, "_%s = %d;\n", capitalize ? "LEN" : "len", p));
	}

      fclose_or_die(fp, fpo);
      واپس 0;
    }

  اگر (hextype == HEX_POSTSCRIPT)
    {
      p = ستون_تعداد;
      جب ((لمبائی < 0 || n < لمبائی) && (e = getc_or_die(fp)) != EOF)
	{
	  putc_or_die(hexx[(e >> 4) & 0xf], fpo);
	  putc_or_die(hexx[e & 0xf], fpo);
	  n++;
	  اگر (ستون_تعداد > 0 && !--p)
	    {
	      putc_or_die('\n', fpo);
	      p = ستون_تعداد;
	    }
	}
      اگر (ستون_تعداد == 0 || p < ستون_تعداد)
	putc_or_die('\n', fpo);
      fclose_or_die(fp, fpo);
      واپس 0;
    }

  /* hextype: HEX_NORMAL or HEX_BITS or HEX_LITTLEENDIAN */

  اگر (hextype != HEX_بٹ)
    {
      grplen = octspergrp + octspergrp + 1;	/* chars per octet group */
    }
  ورنہ	/* hextype == HEX_BITS */
    grplen = 8 * octspergrp + 1;

  جب ((لمبائی < 0 || n < لمبائی) && (e = getc_or_die(fp)) != EOF)
    {
      اگر (p == 0)
	{
	  addrlen = sprintf(l, decimal_انحراف ? "%08lu:" : "%08lx:",
				  ((غیرعلامتی طویل)(n + seekoff + displayoff)));
	  برائے (c = addrlen; c < LLEN_NO_COLOR; l[c++] = ' ')
	    ;
	}
      x = hextype == HEX_LITTLEENDIAN ? p ^ (octspergrp-1) : p;
      c = addrlen + 1 + (grplen * x) / octspergrp;
      اگر (hextype == HEX_معمول || hextype == HEX_LITTLEENDIAN)
	{
	  اگر (color)
	    {
	      cur_color = get_color_char(e, ebcdic);
	      colors[c] = cur_color;
	      colors[c+1] = cur_color;
	    }

	  l[c]   = hexx[(e >> 4) & 0xf];
	  l[++c] = hexx[e & 0xf];
	}
      ورنہ /* hextype == HEX_BITS */
	{
	  اگر (color)
	    cur_color = get_color_char(e, ebcdic);

	  برائے (i = 7; i >= 0; i--)
	    {
	      اگر (color)
		colors[c] = cur_color;
	      l[c++] = (e & (1 << i)) ? '1' : '0';
	    }
	}
      اگر (e)
	nonzero++;
      /* When changing this update definition of LLEN and LLEN_NO_COLOR above. */
      اگر (hextype == HEX_LITTLEENDIAN)
	/* last group will be fully used, round up */
	c = grplen * ((ستون_تعداد + octspergrp - 1) / octspergrp);
      ورنہ
	c = (grplen * ستون_تعداد - 1) / octspergrp;


      اگر (ebcdic)
	e = (e < 64) ? '.' : etoa64[e-64];

      اگر (hextype == HEX_LITTLEENDIAN)
	c -= 1;

      c += addrlen + 3 + p;
      اگر (color)
	  colors[c] = cur_color;
      l[c++] =
#اگر defined(__MVS__) && __CHARSET_LIB == 0
	  (e >= 64)
#ورنہ
	  (e > 31 && e < 127)
#اگر_ختم
	  ? e : '.';
      n++;
      اگر (++p == ستون_تعداد)
	{
	  l[c++] = '\n';
	  l[c] = '\0';

	  xxdline(fpo, l, color ? colors : تہی, autoskip ? nonzero : 1);
	  memset(colors, 0, c);
	  nonzero = 0;
	  p = 0;
	}
    }
  اگر (p)
    {
      l[c++] = '\n';
      l[c] = '\0';
      اگر (color)
	{
	  x = p;
	  اگر (hextype == HEX_LITTLEENDIAN)
	    {
	      صحیح fill = octspergrp - (p % octspergrp);
	      اگر (fill == octspergrp) fill = 0;

	      c = addrlen + 1 + (grplen * (x - (octspergrp-fill))) / octspergrp;

	      برائے (i = 0; i < fill;i++)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		  x++;
		  p++;
		}
	    }

	  اگر (hextype != HEX_بٹ)
	    {
	      c = addrlen + 1 + (grplen * x) / octspergrp;
	      c += ستون_تعداد - p;
	      c += (ستون_تعداد - p) / octspergrp;

	      برائے (i = ستون_تعداد - p; i > 0;i--)
		{
		  colors[c] = COLOR_RED;
		  l[c++] = ' '; /* empty space */
		}
	    }
	  xxdline(fpo, l, colors, 1);
	}
      ورنہ
	xxdline(fpo, l, تہی, 1);
    }
  ورنہ اگر (autoskip)
    xxdline(fpo, l, color ? colors : تہی, -1);	/* last chance to flush out suppressed lines */

  fclose_or_die(fp, fpo);
  واپس 0;
}

/* vi:set ts=8 sw=4 sts=2 cino+={2 cino+=n-2 : */
