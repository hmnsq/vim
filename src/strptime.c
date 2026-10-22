/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * strptime.c: portable strptime() fallback for platforms whose C runtime
 * does not provide one (currently the MSVC / MinGW CRT on Windows).
 *
 * Ported from NetBSD's lib/libc/time/strptime.c (rev 1.67, 2024-06-07).
 * The original BSD 2-clause licence follows below and must be preserved.
 * Windows-specific adjustments:
 *   - Locale-specific weekday / month / AM-PM tables are hard-coded in
 *     English; this matches what strftime() emits in the "C" locale on
 *     Windows and is sufficient for Vim's strptime() test suite.
 *   - The NetBSD-specific fromzone()/tzalloc() path is stubbed out because
 *     the Windows CRT does not ship the IANA tzfile loader.
 *   - The tm_gmtoff / tm_zone BSD extensions are not available in the
 *     Windows CRT's struct tm, so the %Z / %z conversion parses the input
 *     without storing the offset (matching NetBSD behavior when those
 *     fields are not compiled in).
 */

/*-
 * Copyright (c) 1997, 1998, 2005, 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code was contributed to The NetBSD Foundation by Klaus Klein.
 * Heavily optimised by David Laight
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#അന്തര്ഭാവ "vim.h"

#ifdef MSWIN

/* Constants lifted from NetBSD's <tzfile.h>. */
#define TM_YEAR_BASE	1900
#define TM_SUNDAY	0
#define TM_MONDAY	1
#define SECSPERHOUR	3600
#define SECSPERMIN	60
#define HOURSPERDAY	24

#define isleap(y)	(((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)
/* isleap_sum avoids integer overflow when adding the 1900 base. */
#define isleap_sum(a, b) \
			isleap(((unsigned)(a) + (unsigned)(b)) % 400)

/*
 * _get_tzname() is provided by the UCRT and by the MSVC CRT.  Older MinGW
 * builds linking against msvcrt.dll fall back to the deprecated tzname[]
 * global (mingw-w64 already exposes the POSIX-named tzname[]).
 */
#എങ്കിൽ defined(_UCRT) || defined(_MSC_VER)
# define USE_GET_TZNAME
#endif

/* Locale tables (English / "C" locale). */
സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം c_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം c_abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം c_mon[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം c_abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം c_am_pm[] = { "AM", "PM" };
സ്ഥിതം സ്ഥിരം അക്ഷരം c_d_t_fmt[] = "%a %b %e %H:%M:%S %Y";
സ്ഥിതം സ്ഥിരം അക്ഷരം c_d_fmt[] = "%m/%d/%y";
സ്ഥിതം സ്ഥിരം അക്ഷരം c_t_fmt[] = "%H:%M:%S";
സ്ഥിതം സ്ഥിരം അക്ഷരം c_t_fmt_ampm[] = "%I:%M:%S %p";

സ്ഥിതം സ്ഥിരം unsigned അക്ഷരം *conv_അക്കം(സ്ഥിരം unsigned അക്ഷരം *, പൂർണ്ണസംഖ്യ *,
						unsigned പൂർണ്ണസംഖ്യ, unsigned പൂർണ്ണസംഖ്യ);
സ്ഥിതം സ്ഥിരം unsigned അക്ഷരം *find_string(സ്ഥിരം unsigned അക്ഷരം *, പൂർണ്ണസംഖ്യ *,
			സ്ഥിരം അക്ഷരം * സ്ഥിരം *, സ്ഥിരം അക്ഷരം * സ്ഥിരം *, പൂർണ്ണസംഖ്യ);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E			0x01
#define ALT_O			0x02
#define LEGAL_ALT(x)		{ എങ്കിൽ (alt_format & ~(x)) തിരികെ ശൂന്യ; }

#define S_YEAR			(1 << 0)
#define S_MON			(1 << 1)
#define S_YDAY			(1 << 2)
#define S_MDAY			(1 << 3)
#define S_WDAY			(1 << 4)
#define S_HOUR			(1 << 5)

#define HAVE_MDAY(s)		(s & S_MDAY)
#define HAVE_MON(s)		(s & S_MON)
#define HAVE_WDAY(s)		(s & S_WDAY)
#define HAVE_YDAY(s)		(s & S_YDAY)
#define HAVE_YEAR(s)		(s & S_YEAR)
#define HAVE_HOUR(s)		(s & S_HOUR)

/* RFC-822/RFC-2822 */
സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം nast[5] = {
    "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം nadt[5] = {
    "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};

/*
 * Table to determine the ordinal date for the start of a month.
 * Ref: http://en.wikipedia.org/wiki/ISO_week_date
 */
സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ start_of_month[2][13] = {
    /* non-leap year */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* leap year */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

/*
 * Calculate the week day of the first day of a year. Valid for
 * the Gregorian calendar, which began Sept 14, 1752 in the UK
 * and its colonies. Ref:
 * http://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
first_wday_of(പൂർണ്ണസംഖ്യ yr)
{
    തിരികെ ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
	    (isleap(yr) ? 6 : 0) + 1) % 7;
}

#define delim(p)	((p) == '\0' || isspace((unsigned അക്ഷരം)(p)))

/*
 * Stub replacing NetBSD's fromzone(): the Windows CRT does not provide
 * tzalloc() / tzgetgmtoff() and the associated IANA tzfile loader, so
 * we simply decline to recognize arbitrary timezone names.  The %Z / %z
 * numeric and RFC-822/2822 forms are still handled by the caller.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
fromzone(സ്ഥിരം unsigned അക്ഷരം **bp UNUSED, ഘടന tm *tm UNUSED,
						    പൂർണ്ണസംഖ്യ mandatory UNUSED)
{
    തിരികെ 0;
}

    അക്ഷരം *
strptime(സ്ഥിരം അക്ഷരം *buf, സ്ഥിരം അക്ഷരം *fmt, ഘടന tm *tm)
{
    unsigned അക്ഷരം c;
    സ്ഥിരം unsigned അക്ഷരം *bp, *ep, *zname;
    പൂർണ്ണസംഖ്യ alt_format, i, split_year = 0, neg = 0, state = 0,
	day_offset = -1, week_offset = 0, offs, mandatory;
    സ്ഥിരം അക്ഷരം *new_fmt;

    bp = (സ്ഥിരം unsigned അക്ഷരം *)buf;

    ആകുമ്പോൾ (bp != ശൂന്യ && (c = *fmt++) != '\0')
    {
	/* Clear `alternate' modifier prior to new conversion. */
	alt_format = 0;
	i = 0;

	/* Eat up white-space. */
	എങ്കിൽ (isspace(c))
	{
	    ആകുമ്പോൾ (isspace(*bp))
		bp++;
	    തുടരുക;
	}

	എങ്കിൽ (c != '%')
	    പോകുക literal;


again:	തിരഞ്ഞെടുപ്പ് (c = *fmt++)
	{
	അവസ്ഥ '%':	/* "%%" is converted to "%". */
literal:
	    എങ്കിൽ (c != *bp++)
		തിരികെ ശൂന്യ;
	    LEGAL_ALT(0);
	    തുടരുക;

	/*
	 * "Alternative" modifiers. Just set the appropriate flag
	 * and start over again.
	 */
	അവസ്ഥ 'E':	/* "%E?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_E;
	    പോകുക again;

	അവസ്ഥ 'O':	/* "%O?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_O;
	    പോകുക again;

	/*
	 * "Complex" conversion rules, implemented through recursion.
	 */
	അവസ്ഥ 'c':	/* Date and time, using the locale's format. */
	    new_fmt = c_d_t_fmt;
	    state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
	    പോകുക recurse;

	അവസ്ഥ 'D':	/* The date as "%m/%d/%y". */
	    new_fmt = "%m/%d/%y";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    പോകുക recurse;

	അവസ്ഥ 'F':	/* The date as "%Y-%m-%d". */
	    new_fmt = "%Y-%m-%d";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    പോകുക recurse;

	അവസ്ഥ 'R':	/* The time as "%H:%M". */
	    new_fmt = "%H:%M";
	    LEGAL_ALT(0);
	    പോകുക recurse;

	അവസ്ഥ 'r':	/* The time in 12-hour clock representation. */
	    new_fmt = c_t_fmt_ampm;
	    LEGAL_ALT(0);
	    പോകുക recurse;

	അവസ്ഥ 'T':	/* The time as "%H:%M:%S". */
	    new_fmt = "%H:%M:%S";
	    LEGAL_ALT(0);
	    പോകുക recurse;

	അവസ്ഥ 'X':	/* The time, using the locale's format. */
	    new_fmt = c_t_fmt;
	    പോകുക recurse;

	അവസ്ഥ 'x':	/* The date, using the locale's format. */
	    new_fmt = c_d_fmt;
	    state |= S_MON | S_MDAY | S_YEAR;
	recurse:
	    bp = (സ്ഥിരം unsigned അക്ഷരം *)strptime((സ്ഥിരം അക്ഷരം *)bp,
							new_fmt, tm);
	    LEGAL_ALT(ALT_E);
	    തുടരുക;

	/*
	 * "Elementary" conversion rules.
	 */
	അവസ്ഥ 'A':	/* The day of week, using the locale's form. */
	അവസ്ഥ 'a':
	    bp = find_string(bp, &tm->tm_wday, c_day, c_abday, 7);
	    LEGAL_ALT(0);
	    state |= S_WDAY;
	    തുടരുക;

	അവസ്ഥ 'B':	/* The month, using the locale's form. */
	അവസ്ഥ 'b':
	അവസ്ഥ 'h':
	    bp = find_string(bp, &tm->tm_mon, c_mon, c_abmon, 12);
	    LEGAL_ALT(0);
	    state |= S_MON;
	    തുടരുക;

	അവസ്ഥ 'C':	/* The century number. */
	    i = 20;
	    bp = conv_അക്കം(bp, &i, 0, 99);

	    i = i * 100 - TM_YEAR_BASE;
	    എങ്കിൽ (split_year)
		i += tm->tm_year % 100;
	    split_year = 1;
	    tm->tm_year = i;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    തുടരുക;

	അവസ്ഥ 'd':	/* The day of month. */
	അവസ്ഥ 'e':
	    bp = conv_അക്കം(bp, &tm->tm_mday, 1, 31);
	    LEGAL_ALT(ALT_O);
	    state |= S_MDAY;
	    തുടരുക;

	അവസ്ഥ 'k':	/* The hour (24-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	അവസ്ഥ 'H':
	    bp = conv_അക്കം(bp, &tm->tm_hour, 0, 23);
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    തുടരുക;

	അവസ്ഥ 'l':	/* The hour (12-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	അവസ്ഥ 'I':
	    bp = conv_അക്കം(bp, &tm->tm_hour, 1, 12);
	    എങ്കിൽ (tm->tm_hour == 12)
		tm->tm_hour = 0;
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    തുടരുക;

	അവസ്ഥ 'j':	/* The day of year. */
	    i = 1;
	    bp = conv_അക്കം(bp, &i, 1, 366);
	    tm->tm_yday = i - 1;
	    LEGAL_ALT(0);
	    state |= S_YDAY;
	    തുടരുക;

	അവസ്ഥ 'M':	/* The minute. */
	    bp = conv_അക്കം(bp, &tm->tm_min, 0, 59);
	    LEGAL_ALT(ALT_O);
	    തുടരുക;

	അവസ്ഥ 'm':	/* The month. */
	    i = 1;
	    bp = conv_അക്കം(bp, &i, 1, 12);
	    tm->tm_mon = i - 1;
	    LEGAL_ALT(ALT_O);
	    state |= S_MON;
	    തുടരുക;

	അവസ്ഥ 'p':	/* The locale's equivalent of AM/PM. */
	    bp = find_string(bp, &i, c_am_pm, ശൂന്യ, 2);
	    എങ്കിൽ (HAVE_HOUR(state) && tm->tm_hour > 11)
		തിരികെ ശൂന്യ;
	    tm->tm_hour += i * 12;
	    LEGAL_ALT(0);
	    തുടരുക;

	അവസ്ഥ 'S':	/* The seconds. */
	    bp = conv_അക്കം(bp, &tm->tm_sec, 0, 61);
	    LEGAL_ALT(ALT_O);
	    തുടരുക;

	അവസ്ഥ 's':	/* seconds since the epoch */
	    {
		സ്ഥിരം time_t TIME_MAX = (time_t)((വലുപ്പം(time_t) == 8)
					? LLONG_MAX : INT_MAX);
		time_t sse, d;

		എങ്കിൽ (*bp < '0' || *bp > '9')
		{
		    bp = ശൂന്യ;
		    തുടരുക;
		}

		sse = *bp++ - '0';
		ആകുമ്പോൾ (*bp >= '0' && *bp <= '9')
		{
		    d = *bp++ - '0';
		    എങ്കിൽ (sse > TIME_MAX / 10)
		    {
			bp = ശൂന്യ;
			നിർത്തുക;
		    }
		    sse *= 10;
		    എങ്കിൽ (sse > TIME_MAX - d)
		    {
			bp = ശൂന്യ;
			നിർത്തുക;
		    }
		    sse += d;
		}
		എങ്കിൽ (bp == ശൂന്യ)
		    തുടരുക;

		{
		    ഘടന tm *lt = localtime(&sse);

		    എങ്കിൽ (lt == ശൂന്യ)
			bp = ശൂന്യ;
		    അല്ലെങ്കിൽ
		    {
			*tm = *lt;
			state |= S_YDAY | S_WDAY |
				S_MON | S_MDAY | S_YEAR;
		    }
		}
		തുടരുക;
	    }

	അവസ്ഥ 'U':	/* The week of year, beginning on sunday. */
	അവസ്ഥ 'W':	/* The week of year, beginning on monday. */
	    /*
	     * This is bogus, as we can not assume any valid
	     * information present in the tm structure at this
	     * point to calculate a real value, so save the
	     * week for now in case it can be used later.
	     */
	    bp = conv_അക്കം(bp, &i, 0, 53);
	    LEGAL_ALT(ALT_O);
	    എങ്കിൽ (c == 'U')
		day_offset = TM_SUNDAY;
	    അല്ലെങ്കിൽ
		day_offset = TM_MONDAY;
	    week_offset = i;
	    തുടരുക;

	അവസ്ഥ 'w':	/* The day of week, beginning on sunday. */
	    bp = conv_അക്കം(bp, &tm->tm_wday, 0, 6);
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    തുടരുക;

	അവസ്ഥ 'u':	/* The day of week, monday = 1. */
	    bp = conv_അക്കം(bp, &i, 1, 7);
	    tm->tm_wday = i % 7;
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    തുടരുക;

	അവസ്ഥ 'g':	/* ISO week year without century (parsed, ignored). */
	    bp = conv_അക്കം(bp, &i, 0, 99);
	    തുടരുക;

	അവസ്ഥ 'G':	/* ISO week year with century (parsed, ignored). */
	    ചെയ്യുക
		bp++;
	    ആകുമ്പോൾ (isdigit(*bp));
	    തുടരുക;

	അവസ്ഥ 'V':	/* ISO 8601:1988 week number (parsed, ignored). */
	    bp = conv_അക്കം(bp, &i, 1, 53);
	    തുടരുക;

	അവസ്ഥ 'Y':	/* The year. */
	    i = TM_YEAR_BASE;	/* just for data sanity... */
	    bp = conv_അക്കം(bp, &i, 0, 9999);
	    tm->tm_year = i - TM_YEAR_BASE;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    തുടരുക;

	അവസ്ഥ 'y':	/* The year within 100 years of the epoch. */
	    /* LEGAL_ALT(ALT_E | ALT_O); */
	    bp = conv_അക്കം(bp, &i, 0, 99);

	    എങ്കിൽ (split_year)
		/* preserve century */
		i += (tm->tm_year / 100) * 100;
	    അല്ലെങ്കിൽ
	    {
		split_year = 1;
		എങ്കിൽ (i <= 68)
		    i = i + 2000 - TM_YEAR_BASE;
		അല്ലെങ്കിൽ
		    i = i + 1900 - TM_YEAR_BASE;
	    }
	    tm->tm_year = i;
	    state |= S_YEAR;
	    തുടരുക;

	അവസ്ഥ 'Z':
	അവസ്ഥ 'z':
	    tzset();
	    mandatory = c == 'z';
	    /*
	     * We recognize all ISO 8601 formats:
	     * Z	= Zulu time/UTC
	     * [+-]hhmm
	     * [+-]hh:mm
	     * [+-]hh
	     * We recognize all RFC-822/RFC-2822 formats:
	     * UT|GMT
	     *          North American : UTC offsets
	     * E[DS]T = Eastern : -4 | -5
	     * C[DS]T = Central : -5 | -6
	     * M[DS]T = Mountain: -6 | -7
	     * P[DS]T = Pacific : -7 | -8
	     *          Nautical/Military
	     * [A-IL-M] = -1 ... -9 (J not used)
	     * [N-Y]  = +1 ... +12
	     * Note: J maybe used to denote non-nautical
	     *       local time
	     */
	    എങ്കിൽ (mandatory)
		ആകുമ്പോൾ (isspace(*bp))
		    bp++;

	    zname = bp;
	    തിരഞ്ഞെടുപ്പ് (*bp++)
	    {
	    അവസ്ഥ 'G':
		എങ്കിൽ (*bp++ != 'M')
		    പോകുക namedzone;
		/*FALLTHROUGH*/
	    അവസ്ഥ 'U':
		എങ്കിൽ (*bp++ != 'T')
		    പോകുക namedzone;
		അല്ലെങ്കിൽ എങ്കിൽ (!delim(*bp) && *bp++ != 'C')
		    പോകുക namedzone;
		/*FALLTHROUGH*/
	    അവസ്ഥ 'Z':
		എങ്കിൽ (!delim(*bp))
		    പോകുക namedzone;
		tm->tm_isdst = 0;
		തുടരുക;
	    അവസ്ഥ '+':
		neg = 0;
		നിർത്തുക;
	    അവസ്ഥ '-':
		neg = 1;
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
namedzone:
		bp = zname;

		/* Nautical / Military style */
		എങ്കിൽ (delim(bp[1]) &&
			((*bp >= 'A' && *bp <= 'I') ||
			 (*bp >= 'L' && *bp <= 'Y')))
		{
		    bp++;
		    തുടരുക;
		}
		/* 'J' is local time */
		എങ്കിൽ (delim(bp[1]) && *bp == 'J')
		{
		    bp++;
		    തുടരുക;
		}

		/*
		 * From our 3 letter hard-coded table.
		 */
		എങ്കിൽ (delim(bp[0]) || delim(bp[1]) ||
			delim(bp[2]) || !delim(bp[3]))
		    പോകുക loadzone;
		ep = find_string(bp, &i, nast, ശൂന്യ, 4);
		എങ്കിൽ (ep != ശൂന്യ)
		{
		    bp = ep;
		    തുടരുക;
		}
		ep = find_string(bp, &i, nadt, ശൂന്യ, 4);
		എങ്കിൽ (ep != ശൂന്യ)
		{
		    tm->tm_isdst = 1;
		    bp = ep;
		    തുടരുക;
		}
		/*
		 * Our current timezone.  Prefer _get_tzname() over the
		 * tzname[] global, which UCRT marks deprecated because it
		 * may be inaccurate after locale changes.
		 */
#ifdef USE_GET_TZNAME
		{
		    അക്ഷരം tzbuf[2][32];
		    സ്ഥിരം അക്ഷരം *tznames[2] = { tzbuf[0], tzbuf[1] };
		    size_t tzlen;

		    _tzset();
		    എങ്കിൽ (_get_tzname(&tzlen, tzbuf[0], വലുപ്പം(tzbuf[0]), 0) != 0)
			tzbuf[0][0] = NUL;
		    എങ്കിൽ (_get_tzname(&tzlen, tzbuf[1], വലുപ്പം(tzbuf[1]), 1) != 0)
			tzbuf[1][0] = NUL;
		    ep = find_string(bp, &i, tznames, ശൂന്യ, 2);
		}
#അല്ലെങ്കിൽ
		ep = find_string(bp, &i,
			(സ്ഥിരം അക്ഷരം *സ്ഥിരം *)tzname, ശൂന്യ, 2);
#endif
		എങ്കിൽ (ep != ശൂന്യ)
		{
		    tm->tm_isdst = i;
		    bp = ep;
		    തുടരുക;
		}
loadzone:
		/*
		 * The hard way, load the zone!
		 */
		എങ്കിൽ (fromzone(&bp, tm, mandatory))
		    തുടരുക;
		പോകുക out;
	    }
	    offs = 0;
	    വേണ്ടി (i = 0; i < 4; )
	    {
		എങ്കിൽ (isdigit(*bp))
		{
		    offs = offs * 10 + (*bp++ - '0');
		    i++;
		    തുടരുക;
		}
		എങ്കിൽ (i == 2 && *bp == ':')
		{
		    bp++;
		    തുടരുക;
		}
		നിർത്തുക;
	    }
	    എങ്കിൽ (isdigit(*bp))
		പോകുക out;
	    തിരഞ്ഞെടുപ്പ് (i)
	    {
	    അവസ്ഥ 2:
		offs *= SECSPERHOUR;
		നിർത്തുക;
	    അവസ്ഥ 4:
		i = offs % 100;
		offs /= 100;
		എങ്കിൽ (i >= SECSPERMIN)
		    പോകുക out;
		/* Convert minutes into decimal */
		offs = offs * SECSPERHOUR + i * SECSPERMIN;
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
out:
		എങ്കിൽ (mandatory)
		    തിരികെ ശൂന്യ;
		bp = zname;
		തുടരുക;
	    }
	    /* ISO 8601 & RFC 3339 limit to 23:59 max */
	    എങ്കിൽ (offs >= (HOURSPERDAY * SECSPERHOUR))
		പോകുക out;
	    എങ്കിൽ (neg)
		offs = -offs;
	    tm->tm_isdst = 0;	/* XXX */
	    തുടരുക;

	/*
	 * Miscellaneous conversions.
	 */
	അവസ്ഥ 'n':	/* Any kind of white-space. */
	അവസ്ഥ 't':
	    ആകുമ്പോൾ (isspace(*bp))
		bp++;
	    LEGAL_ALT(0);
	    തുടരുക;


	സ്ഥിരസ്ഥിതി:	/* Unknown/unsupported conversion. */
	    തിരികെ ശൂന്യ;
	}
    }

    എങ്കിൽ (!HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	എങ്കിൽ (HAVE_MON(state) && HAVE_MDAY(state))
	{
	    /* calculate day of year (ordinal date) */
	    tm->tm_yday =  start_of_month[isleap_sum(tm->tm_year,
			    TM_YEAR_BASE)][tm->tm_mon] + (tm->tm_mday - 1);
	    state |= S_YDAY;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (day_offset != -1)
	{
	    /*
	     * Set the date to the first Sunday (or Monday)
	     * of the specified week of the year.
	     */
	    എങ്കിൽ (!HAVE_WDAY(state))
	    {
		tm->tm_wday = day_offset;
		state |= S_WDAY;
	    }
	    tm->tm_yday = (7 -
		    first_wday_of(tm->tm_year + TM_YEAR_BASE) +
		    day_offset) % 7 + (week_offset - 1) * 7 +
		    tm->tm_wday  - day_offset;
	    state |= S_YDAY;
	}
    }

    എങ്കിൽ (HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	പൂർണ്ണസംഖ്യ leap;

	എങ്കിൽ (!HAVE_MON(state))
	{
	    /* calculate month of day of year */
	    i = 0;
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    ആകുമ്പോൾ (tm->tm_yday >= start_of_month[leap][i])
		i++;
	    എങ്കിൽ (i > 12)
	    {
		i = 1;
		tm->tm_yday -= start_of_month[leap][12];
		tm->tm_year++;
	    }
	    tm->tm_mon = i - 1;
	    state |= S_MON;
	}

	എങ്കിൽ (!HAVE_MDAY(state))
	{
	    /* calculate day of month */
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    tm->tm_mday = tm->tm_yday -
		start_of_month[leap][tm->tm_mon] + 1;
	    state |= S_MDAY;
	}

	എങ്കിൽ (!HAVE_WDAY(state))
	{
	    /* calculate day of week */
	    i = 0;
	    week_offset = first_wday_of(tm->tm_year);
	    ആകുമ്പോൾ (i++ <= tm->tm_yday)
	    {
		എങ്കിൽ (week_offset++ >= 6)
		    week_offset = 0;
	    }
	    tm->tm_wday = week_offset;
	    state |= S_WDAY;
	}
    }

    തിരികെ (അക്ഷരം *)bp;
}


    സ്ഥിതം സ്ഥിരം unsigned അക്ഷരം *
conv_അക്കം(സ്ഥിരം unsigned അക്ഷരം *buf, പൂർണ്ണസംഖ്യ *dest, unsigned പൂർണ്ണസംഖ്യ llim,
						    unsigned പൂർണ്ണസംഖ്യ ulim)
{
    unsigned പൂർണ്ണസംഖ്യ result = 0;
    unsigned അക്ഷരം ch;

    /* The limit also determines the number of valid digits. */
    unsigned പൂർണ്ണസംഖ്യ rulim = ulim;

    ch = *buf;
    എങ്കിൽ (ch < '0' || ch > '9')
	തിരികെ ശൂന്യ;

    ചെയ്യുക
    {
	result *= 10;
	result += ch - '0';
	rulim /= 10;
	ch = *++buf;
    } ആകുമ്പോൾ ((result * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

    എങ്കിൽ (result < llim || result > ulim)
	തിരികെ ശൂന്യ;

    *dest = result;
    തിരികെ buf;
}

    സ്ഥിതം സ്ഥിരം unsigned അക്ഷരം *
find_string(സ്ഥിരം unsigned അക്ഷരം *bp, പൂർണ്ണസംഖ്യ *tgt, സ്ഥിരം അക്ഷരം *സ്ഥിരം *n1,
					    സ്ഥിരം അക്ഷരം *സ്ഥിരം *n2, പൂർണ്ണസംഖ്യ c)
{
    പൂർണ്ണസംഖ്യ		i;
    size_t	len;

    /* check full name - then abbreviated ones */
    വേണ്ടി (; n1 != ശൂന്യ; n1 = n2, n2 = ശൂന്യ)
    {
	വേണ്ടി (i = 0; i < c; i++, n1++)
	{
	    len = strlen(*n1);
	    എങ്കിൽ (STRNICMP(*n1, bp, len) == 0)
	    {
		*tgt = i;
		തിരികെ bp + len;
	    }
	}
    }

    /* Nothing matched */
    തിരികെ ശൂന്യ;
}

#endif // MSWIN
