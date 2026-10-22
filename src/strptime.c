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

#include "vim.h"

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
#ᤀᤡ᤺ defined(_UCRT) || defined(_MSC_VER)
# define USE_GET_TZNAME
#endif

/* Locale tables (English / "C" locale). */
static const var_na *const c_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
static const var_na *const c_abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static const var_na *const c_mon[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
static const var_na *const c_abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static const var_na *const c_am_pm[] = { "AM", "PM" };
static const var_na c_d_t_fmt[] = "%a %b %e %H:%M:%S %Y";
static const var_na c_d_fmt[] = "%m/%d/%y";
static const var_na c_t_fmt[] = "%H:%M:%S";
static const var_na c_t_fmt_ampm[] = "%I:%M:%S %p";

static const unsigned var_na *conv_num(const unsigned var_na *, puur_naa_nka *,
						unsigned puur_naa_nka, unsigned puur_naa_nka);
static const unsigned var_na *find_string(const unsigned var_na *, puur_naa_nka *,
			const var_na * const *, const var_na * const *, puur_naa_nka);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E			0x01
#define ALT_O			0x02
#define LEGAL_ALT(x)		{ ᤀᤡ᤺ (alt_format & ~(x)) ᤀᤡᤴ NULL; }

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
static const var_na *const nast[5] = {
    "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
static const var_na *const nadt[5] = {
    "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};

/*
 * Table to determine the ordinal date for the start of a month.
 * Ref: http://en.wikipedia.org/wiki/ISO_week_date
 */
static const puur_naa_nka start_of_month[2][13] = {
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
    static puur_naa_nka
first_wday_of(puur_naa_nka yr)
{
    ᤀᤡᤴ ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
	    (isleap(yr) ? 6 : 0) + 1) % 7;
}

#define delim(p)	((p) == '\0' || isspace((unsigned var_na)(p)))

/*
 * Stub replacing NetBSD's fromzone(): the Windows CRT does not provide
 * tzalloc() / tzgetgmtoff() and the associated IANA tzfile loader, so
 * we simply decline to recognize arbitrary timezone names.  The %Z / %z
 * numeric and RFC-822/2822 forms are still handled by the caller.
 */
    static puur_naa_nka
fromzone(const unsigned var_na **bp UNUSED, sa_nracanaa tm *tm UNUSED,
						    puur_naa_nka mandatory UNUSED)
{
    ᤀᤡᤴ 0;
}

    var_na *
strptime(const var_na *buf, const var_na *fmt, sa_nracanaa tm *tm)
{
    unsigned var_na c;
    const unsigned var_na *bp, *ep, *zname;
    puur_naa_nka alt_format, i, split_year = 0, neg = 0, state = 0,
	day_offset = -1, week_offset = 0, offs, mandatory;
    const var_na *new_fmt;

    bp = (const unsigned var_na *)buf;

    ᤀᤠ᤺ (bp != NULL && (c = *fmt++) != '\0')
    {
	/* Clear `alternate' modifier prior to new conversion. */
	alt_format = 0;
	i = 0;

	/* Eat up white-space. */
	ᤀᤡ᤺ (isspace(c))
	{
	    ᤀᤠ᤺ (isspace(*bp))
		bp++;
	    continue;
	}

	ᤀᤡ᤺ (c != '%')
	    goto literal;


again:	switch (c = *fmt++)
	{
	pak_sa '%':	/* "%%" is converted to "%". */
literal:
	    ᤀᤡ᤺ (c != *bp++)
		ᤀᤡᤴ NULL;
	    LEGAL_ALT(0);
	    continue;

	/*
	 * "Alternative" modifiers. Just set the appropriate flag
	 * and start over again.
	 */
	pak_sa 'E':	/* "%E?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_E;
	    goto again;

	pak_sa 'O':	/* "%O?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_O;
	    goto again;

	/*
	 * "Complex" conversion rules, implemented through recursion.
	 */
	pak_sa 'c':	/* Date and time, using the locale's format. */
	    new_fmt = c_d_t_fmt;
	    state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
	    goto recurse;

	pak_sa 'D':	/* The date as "%m/%d/%y". */
	    new_fmt = "%m/%d/%y";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    goto recurse;

	pak_sa 'F':	/* The date as "%Y-%m-%d". */
	    new_fmt = "%Y-%m-%d";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    goto recurse;

	pak_sa 'R':	/* The time as "%H:%M". */
	    new_fmt = "%H:%M";
	    LEGAL_ALT(0);
	    goto recurse;

	pak_sa 'r':	/* The time in 12-hour clock representation. */
	    new_fmt = c_t_fmt_ampm;
	    LEGAL_ALT(0);
	    goto recurse;

	pak_sa 'T':	/* The time as "%H:%M:%S". */
	    new_fmt = "%H:%M:%S";
	    LEGAL_ALT(0);
	    goto recurse;

	pak_sa 'X':	/* The time, using the locale's format. */
	    new_fmt = c_t_fmt;
	    goto recurse;

	pak_sa 'x':	/* The date, using the locale's format. */
	    new_fmt = c_d_fmt;
	    state |= S_MON | S_MDAY | S_YEAR;
	recurse:
	    bp = (const unsigned var_na *)strptime((const var_na *)bp,
							new_fmt, tm);
	    LEGAL_ALT(ALT_E);
	    continue;

	/*
	 * "Elementary" conversion rules.
	 */
	pak_sa 'A':	/* The day of week, using the locale's form. */
	pak_sa 'a':
	    bp = find_string(bp, &tm->tm_wday, c_day, c_abday, 7);
	    LEGAL_ALT(0);
	    state |= S_WDAY;
	    continue;

	pak_sa 'B':	/* The month, using the locale's form. */
	pak_sa 'b':
	pak_sa 'h':
	    bp = find_string(bp, &tm->tm_mon, c_mon, c_abmon, 12);
	    LEGAL_ALT(0);
	    state |= S_MON;
	    continue;

	pak_sa 'C':	/* The century number. */
	    i = 20;
	    bp = conv_num(bp, &i, 0, 99);

	    i = i * 100 - TM_YEAR_BASE;
	    ᤀᤡ᤺ (split_year)
		i += tm->tm_year % 100;
	    split_year = 1;
	    tm->tm_year = i;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    continue;

	pak_sa 'd':	/* The day of month. */
	pak_sa 'e':
	    bp = conv_num(bp, &tm->tm_mday, 1, 31);
	    LEGAL_ALT(ALT_O);
	    state |= S_MDAY;
	    continue;

	pak_sa 'k':	/* The hour (24-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	pak_sa 'H':
	    bp = conv_num(bp, &tm->tm_hour, 0, 23);
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    continue;

	pak_sa 'l':	/* The hour (12-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	pak_sa 'I':
	    bp = conv_num(bp, &tm->tm_hour, 1, 12);
	    ᤀᤡ᤺ (tm->tm_hour == 12)
		tm->tm_hour = 0;
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    continue;

	pak_sa 'j':	/* The day of year. */
	    i = 1;
	    bp = conv_num(bp, &i, 1, 366);
	    tm->tm_yday = i - 1;
	    LEGAL_ALT(0);
	    state |= S_YDAY;
	    continue;

	pak_sa 'M':	/* The minute. */
	    bp = conv_num(bp, &tm->tm_min, 0, 59);
	    LEGAL_ALT(ALT_O);
	    continue;

	pak_sa 'm':	/* The month. */
	    i = 1;
	    bp = conv_num(bp, &i, 1, 12);
	    tm->tm_mon = i - 1;
	    LEGAL_ALT(ALT_O);
	    state |= S_MON;
	    continue;

	pak_sa 'p':	/* The locale's equivalent of AM/PM. */
	    bp = find_string(bp, &i, c_am_pm, NULL, 2);
	    ᤀᤡ᤺ (HAVE_HOUR(state) && tm->tm_hour > 11)
		ᤀᤡᤴ NULL;
	    tm->tm_hour += i * 12;
	    LEGAL_ALT(0);
	    continue;

	pak_sa 'S':	/* The seconds. */
	    bp = conv_num(bp, &tm->tm_sec, 0, 61);
	    LEGAL_ALT(ALT_O);
	    continue;

	pak_sa 's':	/* seconds since the epoch */
	    {
		const time_t TIME_MAX = (time_t)((parimaa_na(time_t) == 8)
					? LLONG_MAX : INT_MAX);
		time_t sse, d;

		ᤀᤡ᤺ (*bp < '0' || *bp > '9')
		{
		    bp = NULL;
		    continue;
		}

		sse = *bp++ - '0';
		ᤀᤠ᤺ (*bp >= '0' && *bp <= '9')
		{
		    d = *bp++ - '0';
		    ᤀᤡ᤺ (sse > TIME_MAX / 10)
		    {
			bp = NULL;
			bha_nga;
		    }
		    sse *= 10;
		    ᤀᤡ᤺ (sse > TIME_MAX - d)
		    {
			bp = NULL;
			bha_nga;
		    }
		    sse += d;
		}
		ᤀᤡ᤺ (bp == NULL)
		    continue;

		{
		    sa_nracanaa tm *lt = localtime(&sse);

		    ᤀᤡ᤺ (lt == NULL)
			bp = NULL;
		    ᤀᤡ᤺ᤆᤡ
		    {
			*tm = *lt;
			state |= S_YDAY | S_WDAY |
				S_MON | S_MDAY | S_YEAR;
		    }
		}
		continue;
	    }

	pak_sa 'U':	/* The week of year, beginning on sunday. */
	pak_sa 'W':	/* The week of year, beginning on monday. */
	    /*
	     * This is bogus, as we can not assume any valid
	     * information present in the tm structure at this
	     * point to calculate a real value, so save the
	     * week for now in case it can be used later.
	     */
	    bp = conv_num(bp, &i, 0, 53);
	    LEGAL_ALT(ALT_O);
	    ᤀᤡ᤺ (c == 'U')
		day_offset = TM_SUNDAY;
	    ᤀᤡ᤺ᤆᤡ
		day_offset = TM_MONDAY;
	    week_offset = i;
	    continue;

	pak_sa 'w':	/* The day of week, beginning on sunday. */
	    bp = conv_num(bp, &tm->tm_wday, 0, 6);
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    continue;

	pak_sa 'u':	/* The day of week, monday = 1. */
	    bp = conv_num(bp, &i, 1, 7);
	    tm->tm_wday = i % 7;
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    continue;

	pak_sa 'g':	/* ISO week year without century (parsed, ignored). */
	    bp = conv_num(bp, &i, 0, 99);
	    continue;

	pak_sa 'G':	/* ISO week year with century (parsed, ignored). */
	    do
		bp++;
	    ᤀᤠ᤺ (isdigit(*bp));
	    continue;

	pak_sa 'V':	/* ISO 8601:1988 week number (parsed, ignored). */
	    bp = conv_num(bp, &i, 1, 53);
	    continue;

	pak_sa 'Y':	/* The year. */
	    i = TM_YEAR_BASE;	/* just for data sanity... */
	    bp = conv_num(bp, &i, 0, 9999);
	    tm->tm_year = i - TM_YEAR_BASE;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    continue;

	pak_sa 'y':	/* The year within 100 years of the epoch. */
	    /* LEGAL_ALT(ALT_E | ALT_O); */
	    bp = conv_num(bp, &i, 0, 99);

	    ᤀᤡ᤺ (split_year)
		/* preserve century */
		i += (tm->tm_year / 100) * 100;
	    ᤀᤡ᤺ᤆᤡ
	    {
		split_year = 1;
		ᤀᤡ᤺ (i <= 68)
		    i = i + 2000 - TM_YEAR_BASE;
		ᤀᤡ᤺ᤆᤡ
		    i = i + 1900 - TM_YEAR_BASE;
	    }
	    tm->tm_year = i;
	    state |= S_YEAR;
	    continue;

	pak_sa 'Z':
	pak_sa 'z':
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
	    ᤀᤡ᤺ (mandatory)
		ᤀᤠ᤺ (isspace(*bp))
		    bp++;

	    zname = bp;
	    switch (*bp++)
	    {
	    pak_sa 'G':
		ᤀᤡ᤺ (*bp++ != 'M')
		    goto namedzone;
		/*FALLTHROUGH*/
	    pak_sa 'U':
		ᤀᤡ᤺ (*bp++ != 'T')
		    goto namedzone;
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!delim(*bp) && *bp++ != 'C')
		    goto namedzone;
		/*FALLTHROUGH*/
	    pak_sa 'Z':
		ᤀᤡ᤺ (!delim(*bp))
		    goto namedzone;
		tm->tm_isdst = 0;
		continue;
	    pak_sa '+':
		neg = 0;
		bha_nga;
	    pak_sa '-':
		neg = 1;
		bha_nga;
	    default:
namedzone:
		bp = zname;

		/* Nautical / Military style */
		ᤀᤡ᤺ (delim(bp[1]) &&
			((*bp >= 'A' && *bp <= 'I') ||
			 (*bp >= 'L' && *bp <= 'Y')))
		{
		    bp++;
		    continue;
		}
		/* 'J' is local time */
		ᤀᤡ᤺ (delim(bp[1]) && *bp == 'J')
		{
		    bp++;
		    continue;
		}

		/*
		 * From our 3 letter hard-coded table.
		 */
		ᤀᤡ᤺ (delim(bp[0]) || delim(bp[1]) ||
			delim(bp[2]) || !delim(bp[3]))
		    goto loadzone;
		ep = find_string(bp, &i, nast, NULL, 4);
		ᤀᤡ᤺ (ep != NULL)
		{
		    bp = ep;
		    continue;
		}
		ep = find_string(bp, &i, nadt, NULL, 4);
		ᤀᤡ᤺ (ep != NULL)
		{
		    tm->tm_isdst = 1;
		    bp = ep;
		    continue;
		}
		/*
		 * Our current timezone.  Prefer _get_tzname() over the
		 * tzname[] global, which UCRT marks deprecated because it
		 * may be inaccurate after locale changes.
		 */
#ifdef USE_GET_TZNAME
		{
		    var_na tzbuf[2][32];
		    const var_na *tznames[2] = { tzbuf[0], tzbuf[1] };
		    size_t tzlen;

		    _tzset();
		    ᤀᤡ᤺ (_get_tzname(&tzlen, tzbuf[0], parimaa_na(tzbuf[0]), 0) != 0)
			tzbuf[0][0] = NUL;
		    ᤀᤡ᤺ (_get_tzname(&tzlen, tzbuf[1], parimaa_na(tzbuf[1]), 1) != 0)
			tzbuf[1][0] = NUL;
		    ep = find_string(bp, &i, tznames, NULL, 2);
		}
#ᤀᤡ᤺ᤆᤡ
		ep = find_string(bp, &i,
			(const var_na *const *)tzname, NULL, 2);
#endif
		ᤀᤡ᤺ (ep != NULL)
		{
		    tm->tm_isdst = i;
		    bp = ep;
		    continue;
		}
loadzone:
		/*
		 * The hard way, load the zone!
		 */
		ᤀᤡ᤺ (fromzone(&bp, tm, mandatory))
		    continue;
		goto out;
	    }
	    offs = 0;
	    for (i = 0; i < 4; )
	    {
		ᤀᤡ᤺ (isdigit(*bp))
		{
		    offs = offs * 10 + (*bp++ - '0');
		    i++;
		    continue;
		}
		ᤀᤡ᤺ (i == 2 && *bp == ':')
		{
		    bp++;
		    continue;
		}
		bha_nga;
	    }
	    ᤀᤡ᤺ (isdigit(*bp))
		goto out;
	    switch (i)
	    {
	    pak_sa 2:
		offs *= SECSPERHOUR;
		bha_nga;
	    pak_sa 4:
		i = offs % 100;
		offs /= 100;
		ᤀᤡ᤺ (i >= SECSPERMIN)
		    goto out;
		/* Convert minutes into decimal */
		offs = offs * SECSPERHOUR + i * SECSPERMIN;
		bha_nga;
	    default:
out:
		ᤀᤡ᤺ (mandatory)
		    ᤀᤡᤴ NULL;
		bp = zname;
		continue;
	    }
	    /* ISO 8601 & RFC 3339 limit to 23:59 max */
	    ᤀᤡ᤺ (offs >= (HOURSPERDAY * SECSPERHOUR))
		goto out;
	    ᤀᤡ᤺ (neg)
		offs = -offs;
	    tm->tm_isdst = 0;	/* XXX */
	    continue;

	/*
	 * Miscellaneous conversions.
	 */
	pak_sa 'n':	/* Any kind of white-space. */
	pak_sa 't':
	    ᤀᤠ᤺ (isspace(*bp))
		bp++;
	    LEGAL_ALT(0);
	    continue;


	default:	/* Unknown/unsupported conversion. */
	    ᤀᤡᤴ NULL;
	}
    }

    ᤀᤡ᤺ (!HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	ᤀᤡ᤺ (HAVE_MON(state) && HAVE_MDAY(state))
	{
	    /* calculate day of year (ordinal date) */
	    tm->tm_yday =  start_of_month[isleap_sum(tm->tm_year,
			    TM_YEAR_BASE)][tm->tm_mon] + (tm->tm_mday - 1);
	    state |= S_YDAY;
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (day_offset != -1)
	{
	    /*
	     * Set the date to the first Sunday (or Monday)
	     * of the specified week of the year.
	     */
	    ᤀᤡ᤺ (!HAVE_WDAY(state))
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

    ᤀᤡ᤺ (HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	puur_naa_nka leap;

	ᤀᤡ᤺ (!HAVE_MON(state))
	{
	    /* calculate month of day of year */
	    i = 0;
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    ᤀᤠ᤺ (tm->tm_yday >= start_of_month[leap][i])
		i++;
	    ᤀᤡ᤺ (i > 12)
	    {
		i = 1;
		tm->tm_yday -= start_of_month[leap][12];
		tm->tm_year++;
	    }
	    tm->tm_mon = i - 1;
	    state |= S_MON;
	}

	ᤀᤡ᤺ (!HAVE_MDAY(state))
	{
	    /* calculate day of month */
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    tm->tm_mday = tm->tm_yday -
		start_of_month[leap][tm->tm_mon] + 1;
	    state |= S_MDAY;
	}

	ᤀᤡ᤺ (!HAVE_WDAY(state))
	{
	    /* calculate day of week */
	    i = 0;
	    week_offset = first_wday_of(tm->tm_year);
	    ᤀᤠ᤺ (i++ <= tm->tm_yday)
	    {
		ᤀᤡ᤺ (week_offset++ >= 6)
		    week_offset = 0;
	    }
	    tm->tm_wday = week_offset;
	    state |= S_WDAY;
	}
    }

    ᤀᤡᤴ (var_na *)bp;
}


    static const unsigned var_na *
conv_num(const unsigned var_na *buf, puur_naa_nka *dest, unsigned puur_naa_nka llim,
						    unsigned puur_naa_nka ulim)
{
    unsigned puur_naa_nka result = 0;
    unsigned var_na ch;

    /* The limit also determines the number of valid digits. */
    unsigned puur_naa_nka rulim = ulim;

    ch = *buf;
    ᤀᤡ᤺ (ch < '0' || ch > '9')
	ᤀᤡᤴ NULL;

    do
    {
	result *= 10;
	result += ch - '0';
	rulim /= 10;
	ch = *++buf;
    } ᤀᤠ᤺ ((result * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

    ᤀᤡ᤺ (result < llim || result > ulim)
	ᤀᤡᤴ NULL;

    *dest = result;
    ᤀᤡᤴ buf;
}

    static const unsigned var_na *
find_string(const unsigned var_na *bp, puur_naa_nka *tgt, const var_na *const *n1,
					    const var_na *const *n2, puur_naa_nka c)
{
    puur_naa_nka		i;
    size_t	len;

    /* check full name - then abbreviated ones */
    for (; n1 != NULL; n1 = n2, n2 = NULL)
    {
	for (i = 0; i < c; i++, n1++)
	{
	    len = strlen(*n1);
	    ᤀᤡ᤺ (STRNICMP(*n1, bp, len) == 0)
	    {
		*tgt = i;
		ᤀᤡᤴ bp + len;
	    }
	}
    }

    /* Nothing matched */
    ᤀᤡᤴ NULL;
}

#endif // MSWIN
