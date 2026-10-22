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

#অন্তর্ভাব "vim.h"

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
#যদি defined(_UCRT) || defined(_MSC_VER)
# define USE_GET_TZNAME
#endif

/* Locale tables (English / "C" locale). */
স্থির ধ্রুব বর্ণ *ধ্রুব c_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
স্থির ধ্রুব বর্ণ *ধ্রুব c_abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
স্থির ধ্রুব বর্ণ *ধ্রুব c_mon[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
স্থির ধ্রুব বর্ণ *ধ্রুব c_abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
স্থির ধ্রুব বর্ণ *ধ্রুব c_am_pm[] = { "AM", "PM" };
স্থির ধ্রুব বর্ণ c_d_t_fmt[] = "%a %b %e %H:%M:%S %Y";
স্থির ধ্রুব বর্ণ c_d_fmt[] = "%m/%d/%y";
স্থির ধ্রুব বর্ণ c_t_fmt[] = "%H:%M:%S";
স্থির ধ্রুব বর্ণ c_t_fmt_ampm[] = "%I:%M:%S %p";

স্থির ধ্রুব unsigned বর্ণ *conv_অঙ্ক(ধ্রুব unsigned বর্ণ *, পূর্ণাঙ্ক *,
						unsigned পূর্ণাঙ্ক, unsigned পূর্ণাঙ্ক);
স্থির ধ্রুব unsigned বর্ণ *find_string(ধ্রুব unsigned বর্ণ *, পূর্ণাঙ্ক *,
			ধ্রুব বর্ণ * ধ্রুব *, ধ্রুব বর্ণ * ধ্রুব *, পূর্ণাঙ্ক);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E			0x01
#define ALT_O			0x02
#define LEGAL_ALT(x)		{ যদি (alt_format & ~(x)) প্রত্যাগম রিক্ত; }

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
স্থির ধ্রুব বর্ণ *ধ্রুব nast[5] = {
    "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
স্থির ধ্রুব বর্ণ *ধ্রুব nadt[5] = {
    "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};

/*
 * Table to determine the ordinal date for the start of a month.
 * Ref: http://en.wikipedia.org/wiki/ISO_week_date
 */
স্থির ধ্রুব পূর্ণাঙ্ক start_of_month[2][13] = {
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
    স্থির পূর্ণাঙ্ক
first_wday_of(পূর্ণাঙ্ক yr)
{
    প্রত্যাগম ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
	    (isleap(yr) ? 6 : 0) + 1) % 7;
}

#define delim(p)	((p) == '\0' || isspace((unsigned বর্ণ)(p)))

/*
 * Stub replacing NetBSD's fromzone(): the Windows CRT does not provide
 * tzalloc() / tzgetgmtoff() and the associated IANA tzfile loader, so
 * we simply decline to recognize arbitrary timezone names.  The %Z / %z
 * numeric and RFC-822/2822 forms are still handled by the caller.
 */
    স্থির পূর্ণাঙ্ক
fromzone(ধ্রুব unsigned বর্ণ **bp UNUSED, সংরচনা tm *tm UNUSED,
						    পূর্ণাঙ্ক mandatory UNUSED)
{
    প্রত্যাগম 0;
}

    বর্ণ *
strptime(ধ্রুব বর্ণ *buf, ধ্রুব বর্ণ *fmt, সংরচনা tm *tm)
{
    unsigned বর্ণ c;
    ধ্রুব unsigned বর্ণ *bp, *ep, *zname;
    পূর্ণাঙ্ক alt_format, i, split_year = 0, neg = 0, state = 0,
	day_offset = -1, week_offset = 0, offs, mandatory;
    ধ্রুব বর্ণ *new_fmt;

    bp = (ধ্রুব unsigned বর্ণ *)buf;

    যাবত্ (bp != রিক্ত && (c = *fmt++) != '\0')
    {
	/* Clear `alternate' modifier prior to new conversion. */
	alt_format = 0;
	i = 0;

	/* Eat up white-space. */
	যদি (isspace(c))
	{
	    যাবত্ (isspace(*bp))
		bp++;
	    অনুবর্ত;
	}

	যদি (c != '%')
	    গচ্ছ literal;


again:	বিকল্প (c = *fmt++)
	{
	পক্ষ '%':	/* "%%" is converted to "%". */
literal:
	    যদি (c != *bp++)
		প্রত্যাগম রিক্ত;
	    LEGAL_ALT(0);
	    অনুবর্ত;

	/*
	 * "Alternative" modifiers. Just set the appropriate flag
	 * and start over again.
	 */
	পক্ষ 'E':	/* "%E?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_E;
	    গচ্ছ again;

	পক্ষ 'O':	/* "%O?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_O;
	    গচ্ছ again;

	/*
	 * "Complex" conversion rules, implemented through recursion.
	 */
	পক্ষ 'c':	/* Date and time, using the locale's format. */
	    new_fmt = c_d_t_fmt;
	    state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
	    গচ্ছ recurse;

	পক্ষ 'D':	/* The date as "%m/%d/%y". */
	    new_fmt = "%m/%d/%y";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    গচ্ছ recurse;

	পক্ষ 'F':	/* The date as "%Y-%m-%d". */
	    new_fmt = "%Y-%m-%d";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    গচ্ছ recurse;

	পক্ষ 'R':	/* The time as "%H:%M". */
	    new_fmt = "%H:%M";
	    LEGAL_ALT(0);
	    গচ্ছ recurse;

	পক্ষ 'r':	/* The time in 12-hour clock representation. */
	    new_fmt = c_t_fmt_ampm;
	    LEGAL_ALT(0);
	    গচ্ছ recurse;

	পক্ষ 'T':	/* The time as "%H:%M:%S". */
	    new_fmt = "%H:%M:%S";
	    LEGAL_ALT(0);
	    গচ্ছ recurse;

	পক্ষ 'X':	/* The time, using the locale's format. */
	    new_fmt = c_t_fmt;
	    গচ্ছ recurse;

	পক্ষ 'x':	/* The date, using the locale's format. */
	    new_fmt = c_d_fmt;
	    state |= S_MON | S_MDAY | S_YEAR;
	recurse:
	    bp = (ধ্রুব unsigned বর্ণ *)strptime((ধ্রুব বর্ণ *)bp,
							new_fmt, tm);
	    LEGAL_ALT(ALT_E);
	    অনুবর্ত;

	/*
	 * "Elementary" conversion rules.
	 */
	পক্ষ 'A':	/* The day of week, using the locale's form. */
	পক্ষ 'a':
	    bp = find_string(bp, &tm->tm_wday, c_day, c_abday, 7);
	    LEGAL_ALT(0);
	    state |= S_WDAY;
	    অনুবর্ত;

	পক্ষ 'B':	/* The month, using the locale's form. */
	পক্ষ 'b':
	পক্ষ 'h':
	    bp = find_string(bp, &tm->tm_mon, c_mon, c_abmon, 12);
	    LEGAL_ALT(0);
	    state |= S_MON;
	    অনুবর্ত;

	পক্ষ 'C':	/* The century number. */
	    i = 20;
	    bp = conv_অঙ্ক(bp, &i, 0, 99);

	    i = i * 100 - TM_YEAR_BASE;
	    যদি (split_year)
		i += tm->tm_year % 100;
	    split_year = 1;
	    tm->tm_year = i;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    অনুবর্ত;

	পক্ষ 'd':	/* The day of month. */
	পক্ষ 'e':
	    bp = conv_অঙ্ক(bp, &tm->tm_mday, 1, 31);
	    LEGAL_ALT(ALT_O);
	    state |= S_MDAY;
	    অনুবর্ত;

	পক্ষ 'k':	/* The hour (24-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	পক্ষ 'H':
	    bp = conv_অঙ্ক(bp, &tm->tm_hour, 0, 23);
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    অনুবর্ত;

	পক্ষ 'l':	/* The hour (12-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	পক্ষ 'I':
	    bp = conv_অঙ্ক(bp, &tm->tm_hour, 1, 12);
	    যদি (tm->tm_hour == 12)
		tm->tm_hour = 0;
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    অনুবর্ত;

	পক্ষ 'j':	/* The day of year. */
	    i = 1;
	    bp = conv_অঙ্ক(bp, &i, 1, 366);
	    tm->tm_yday = i - 1;
	    LEGAL_ALT(0);
	    state |= S_YDAY;
	    অনুবর্ত;

	পক্ষ 'M':	/* The minute. */
	    bp = conv_অঙ্ক(bp, &tm->tm_min, 0, 59);
	    LEGAL_ALT(ALT_O);
	    অনুবর্ত;

	পক্ষ 'm':	/* The month. */
	    i = 1;
	    bp = conv_অঙ্ক(bp, &i, 1, 12);
	    tm->tm_mon = i - 1;
	    LEGAL_ALT(ALT_O);
	    state |= S_MON;
	    অনুবর্ত;

	পক্ষ 'p':	/* The locale's equivalent of AM/PM. */
	    bp = find_string(bp, &i, c_am_pm, রিক্ত, 2);
	    যদি (HAVE_HOUR(state) && tm->tm_hour > 11)
		প্রত্যাগম রিক্ত;
	    tm->tm_hour += i * 12;
	    LEGAL_ALT(0);
	    অনুবর্ত;

	পক্ষ 'S':	/* The seconds. */
	    bp = conv_অঙ্ক(bp, &tm->tm_sec, 0, 61);
	    LEGAL_ALT(ALT_O);
	    অনুবর্ত;

	পক্ষ 's':	/* seconds since the epoch */
	    {
		ধ্রুব time_t TIME_MAX = (time_t)((পরিমাণ(time_t) == 8)
					? LLONG_MAX : INT_MAX);
		time_t sse, d;

		যদি (*bp < '0' || *bp > '9')
		{
		    bp = রিক্ত;
		    অনুবর্ত;
		}

		sse = *bp++ - '0';
		যাবত্ (*bp >= '0' && *bp <= '9')
		{
		    d = *bp++ - '0';
		    যদি (sse > TIME_MAX / 10)
		    {
			bp = রিক্ত;
			ভঙ্গ;
		    }
		    sse *= 10;
		    যদি (sse > TIME_MAX - d)
		    {
			bp = রিক্ত;
			ভঙ্গ;
		    }
		    sse += d;
		}
		যদি (bp == রিক্ত)
		    অনুবর্ত;

		{
		    সংরচনা tm *lt = localtime(&sse);

		    যদি (lt == রিক্ত)
			bp = রিক্ত;
		    অন্যথা
		    {
			*tm = *lt;
			state |= S_YDAY | S_WDAY |
				S_MON | S_MDAY | S_YEAR;
		    }
		}
		অনুবর্ত;
	    }

	পক্ষ 'U':	/* The week of year, beginning on sunday. */
	পক্ষ 'W':	/* The week of year, beginning on monday. */
	    /*
	     * This is bogus, as we can not assume any valid
	     * information present in the tm structure at this
	     * point to calculate a real value, so save the
	     * week for now in case it can be used later.
	     */
	    bp = conv_অঙ্ক(bp, &i, 0, 53);
	    LEGAL_ALT(ALT_O);
	    যদি (c == 'U')
		day_offset = TM_SUNDAY;
	    অন্যথা
		day_offset = TM_MONDAY;
	    week_offset = i;
	    অনুবর্ত;

	পক্ষ 'w':	/* The day of week, beginning on sunday. */
	    bp = conv_অঙ্ক(bp, &tm->tm_wday, 0, 6);
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    অনুবর্ত;

	পক্ষ 'u':	/* The day of week, monday = 1. */
	    bp = conv_অঙ্ক(bp, &i, 1, 7);
	    tm->tm_wday = i % 7;
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    অনুবর্ত;

	পক্ষ 'g':	/* ISO week year without century (parsed, ignored). */
	    bp = conv_অঙ্ক(bp, &i, 0, 99);
	    অনুবর্ত;

	পক্ষ 'G':	/* ISO week year with century (parsed, ignored). */
	    কুরু
		bp++;
	    যাবত্ (isdigit(*bp));
	    অনুবর্ত;

	পক্ষ 'V':	/* ISO 8601:1988 week number (parsed, ignored). */
	    bp = conv_অঙ্ক(bp, &i, 1, 53);
	    অনুবর্ত;

	পক্ষ 'Y':	/* The year. */
	    i = TM_YEAR_BASE;	/* just for data sanity... */
	    bp = conv_অঙ্ক(bp, &i, 0, 9999);
	    tm->tm_year = i - TM_YEAR_BASE;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    অনুবর্ত;

	পক্ষ 'y':	/* The year within 100 years of the epoch. */
	    /* LEGAL_ALT(ALT_E | ALT_O); */
	    bp = conv_অঙ্ক(bp, &i, 0, 99);

	    যদি (split_year)
		/* preserve century */
		i += (tm->tm_year / 100) * 100;
	    অন্যথা
	    {
		split_year = 1;
		যদি (i <= 68)
		    i = i + 2000 - TM_YEAR_BASE;
		অন্যথা
		    i = i + 1900 - TM_YEAR_BASE;
	    }
	    tm->tm_year = i;
	    state |= S_YEAR;
	    অনুবর্ত;

	পক্ষ 'Z':
	পক্ষ 'z':
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
	    যদি (mandatory)
		যাবত্ (isspace(*bp))
		    bp++;

	    zname = bp;
	    বিকল্প (*bp++)
	    {
	    পক্ষ 'G':
		যদি (*bp++ != 'M')
		    গচ্ছ namedzone;
		/*FALLTHROUGH*/
	    পক্ষ 'U':
		যদি (*bp++ != 'T')
		    গচ্ছ namedzone;
		অন্যথা যদি (!delim(*bp) && *bp++ != 'C')
		    গচ্ছ namedzone;
		/*FALLTHROUGH*/
	    পক্ষ 'Z':
		যদি (!delim(*bp))
		    গচ্ছ namedzone;
		tm->tm_isdst = 0;
		অনুবর্ত;
	    পক্ষ '+':
		neg = 0;
		ভঙ্গ;
	    পক্ষ '-':
		neg = 1;
		ভঙ্গ;
	    স্বভাব:
namedzone:
		bp = zname;

		/* Nautical / Military style */
		যদি (delim(bp[1]) &&
			((*bp >= 'A' && *bp <= 'I') ||
			 (*bp >= 'L' && *bp <= 'Y')))
		{
		    bp++;
		    অনুবর্ত;
		}
		/* 'J' is local time */
		যদি (delim(bp[1]) && *bp == 'J')
		{
		    bp++;
		    অনুবর্ত;
		}

		/*
		 * From our 3 letter hard-coded table.
		 */
		যদি (delim(bp[0]) || delim(bp[1]) ||
			delim(bp[2]) || !delim(bp[3]))
		    গচ্ছ loadzone;
		ep = find_string(bp, &i, nast, রিক্ত, 4);
		যদি (ep != রিক্ত)
		{
		    bp = ep;
		    অনুবর্ত;
		}
		ep = find_string(bp, &i, nadt, রিক্ত, 4);
		যদি (ep != রিক্ত)
		{
		    tm->tm_isdst = 1;
		    bp = ep;
		    অনুবর্ত;
		}
		/*
		 * Our current timezone.  Prefer _get_tzname() over the
		 * tzname[] global, which UCRT marks deprecated because it
		 * may be inaccurate after locale changes.
		 */
#ifdef USE_GET_TZNAME
		{
		    বর্ণ tzbuf[2][32];
		    ধ্রুব বর্ণ *tznames[2] = { tzbuf[0], tzbuf[1] };
		    size_t tzlen;

		    _tzset();
		    যদি (_get_tzname(&tzlen, tzbuf[0], পরিমাণ(tzbuf[0]), 0) != 0)
			tzbuf[0][0] = NUL;
		    যদি (_get_tzname(&tzlen, tzbuf[1], পরিমাণ(tzbuf[1]), 1) != 0)
			tzbuf[1][0] = NUL;
		    ep = find_string(bp, &i, tznames, রিক্ত, 2);
		}
#অন্যথা
		ep = find_string(bp, &i,
			(ধ্রুব বর্ণ *ধ্রুব *)tzname, রিক্ত, 2);
#endif
		যদি (ep != রিক্ত)
		{
		    tm->tm_isdst = i;
		    bp = ep;
		    অনুবর্ত;
		}
loadzone:
		/*
		 * The hard way, load the zone!
		 */
		যদি (fromzone(&bp, tm, mandatory))
		    অনুবর্ত;
		গচ্ছ out;
	    }
	    offs = 0;
	    প্রতি (i = 0; i < 4; )
	    {
		যদি (isdigit(*bp))
		{
		    offs = offs * 10 + (*bp++ - '0');
		    i++;
		    অনুবর্ত;
		}
		যদি (i == 2 && *bp == ':')
		{
		    bp++;
		    অনুবর্ত;
		}
		ভঙ্গ;
	    }
	    যদি (isdigit(*bp))
		গচ্ছ out;
	    বিকল্প (i)
	    {
	    পক্ষ 2:
		offs *= SECSPERHOUR;
		ভঙ্গ;
	    পক্ষ 4:
		i = offs % 100;
		offs /= 100;
		যদি (i >= SECSPERMIN)
		    গচ্ছ out;
		/* Convert minutes into decimal */
		offs = offs * SECSPERHOUR + i * SECSPERMIN;
		ভঙ্গ;
	    স্বভাব:
out:
		যদি (mandatory)
		    প্রত্যাগম রিক্ত;
		bp = zname;
		অনুবর্ত;
	    }
	    /* ISO 8601 & RFC 3339 limit to 23:59 max */
	    যদি (offs >= (HOURSPERDAY * SECSPERHOUR))
		গচ্ছ out;
	    যদি (neg)
		offs = -offs;
	    tm->tm_isdst = 0;	/* XXX */
	    অনুবর্ত;

	/*
	 * Miscellaneous conversions.
	 */
	পক্ষ 'n':	/* Any kind of white-space. */
	পক্ষ 't':
	    যাবত্ (isspace(*bp))
		bp++;
	    LEGAL_ALT(0);
	    অনুবর্ত;


	স্বভাব:	/* Unknown/unsupported conversion. */
	    প্রত্যাগম রিক্ত;
	}
    }

    যদি (!HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	যদি (HAVE_MON(state) && HAVE_MDAY(state))
	{
	    /* calculate day of year (ordinal date) */
	    tm->tm_yday =  start_of_month[isleap_sum(tm->tm_year,
			    TM_YEAR_BASE)][tm->tm_mon] + (tm->tm_mday - 1);
	    state |= S_YDAY;
	}
	অন্যথা যদি (day_offset != -1)
	{
	    /*
	     * Set the date to the first Sunday (or Monday)
	     * of the specified week of the year.
	     */
	    যদি (!HAVE_WDAY(state))
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

    যদি (HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	পূর্ণাঙ্ক leap;

	যদি (!HAVE_MON(state))
	{
	    /* calculate month of day of year */
	    i = 0;
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    যাবত্ (tm->tm_yday >= start_of_month[leap][i])
		i++;
	    যদি (i > 12)
	    {
		i = 1;
		tm->tm_yday -= start_of_month[leap][12];
		tm->tm_year++;
	    }
	    tm->tm_mon = i - 1;
	    state |= S_MON;
	}

	যদি (!HAVE_MDAY(state))
	{
	    /* calculate day of month */
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    tm->tm_mday = tm->tm_yday -
		start_of_month[leap][tm->tm_mon] + 1;
	    state |= S_MDAY;
	}

	যদি (!HAVE_WDAY(state))
	{
	    /* calculate day of week */
	    i = 0;
	    week_offset = first_wday_of(tm->tm_year);
	    যাবত্ (i++ <= tm->tm_yday)
	    {
		যদি (week_offset++ >= 6)
		    week_offset = 0;
	    }
	    tm->tm_wday = week_offset;
	    state |= S_WDAY;
	}
    }

    প্রত্যাগম (বর্ণ *)bp;
}


    স্থির ধ্রুব unsigned বর্ণ *
conv_অঙ্ক(ধ্রুব unsigned বর্ণ *buf, পূর্ণাঙ্ক *dest, unsigned পূর্ণাঙ্ক llim,
						    unsigned পূর্ণাঙ্ক ulim)
{
    unsigned পূর্ণাঙ্ক result = 0;
    unsigned বর্ণ ch;

    /* The limit also determines the number of valid digits. */
    unsigned পূর্ণাঙ্ক rulim = ulim;

    ch = *buf;
    যদি (ch < '0' || ch > '9')
	প্রত্যাগম রিক্ত;

    কুরু
    {
	result *= 10;
	result += ch - '0';
	rulim /= 10;
	ch = *++buf;
    } যাবত্ ((result * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

    যদি (result < llim || result > ulim)
	প্রত্যাগম রিক্ত;

    *dest = result;
    প্রত্যাগম buf;
}

    স্থির ধ্রুব unsigned বর্ণ *
find_string(ধ্রুব unsigned বর্ণ *bp, পূর্ণাঙ্ক *tgt, ধ্রুব বর্ণ *ধ্রুব *n1,
					    ধ্রুব বর্ণ *ধ্রুব *n2, পূর্ণাঙ্ক c)
{
    পূর্ণাঙ্ক		i;
    size_t	len;

    /* check full name - then abbreviated ones */
    প্রতি (; n1 != রিক্ত; n1 = n2, n2 = রিক্ত)
    {
	প্রতি (i = 0; i < c; i++, n1++)
	{
	    len = strlen(*n1);
	    যদি (STRNICMP(*n1, bp, len) == 0)
	    {
		*tgt = i;
		প্রত্যাগম bp + len;
	    }
	}
    }

    /* Nothing matched */
    প্রত্যাগম রিক্ত;
}

#endif // MSWIN
