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

#अन्तर्भाव "vim.h"

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
#सचे defined(_UCRT) || defined(_MSC_VER)
# define USE_GET_TZNAME
#endif

/* Locale tables (English / "C" locale). */
स्थिर ध्रुव वर्ण *ध्रुव c_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
स्थिर ध्रुव वर्ण *ध्रुव c_abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
स्थिर ध्रुव वर्ण *ध्रुव c_mon[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
स्थिर ध्रुव वर्ण *ध्रुव c_abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
स्थिर ध्रुव वर्ण *ध्रुव c_am_pm[] = { "AM", "PM" };
स्थिर ध्रुव वर्ण c_d_t_fmt[] = "%a %b %e %H:%M:%S %Y";
स्थिर ध्रुव वर्ण c_d_fmt[] = "%m/%d/%y";
स्थिर ध्रुव वर्ण c_t_fmt[] = "%H:%M:%S";
स्थिर ध्रुव वर्ण c_t_fmt_ampm[] = "%I:%M:%S %p";

स्थिर ध्रुव unsigned वर्ण *conv_अङ्कः(ध्रुव unsigned वर्ण *, पूर्णाङ्क *,
						unsigned पूर्णाङ्क, unsigned पूर्णाङ्क);
स्थिर ध्रुव unsigned वर्ण *find_string(ध्रुव unsigned वर्ण *, पूर्णाङ्क *,
			ध्रुव वर्ण * ध्रुव *, ध्रुव वर्ण * ध्रुव *, पूर्णाङ्क);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E			0x01
#define ALT_O			0x02
#define LEGAL_ALT(x)		{ सचे (alt_format & ~(x)) पटिनिवत्त रिक्त; }

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
स्थिर ध्रुव वर्ण *ध्रुव nast[5] = {
    "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
स्थिर ध्रुव वर्ण *ध्रुव nadt[5] = {
    "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};

/*
 * Table to determine the ordinal date for the start of a month.
 * Ref: http://en.wikipedia.org/wiki/ISO_week_date
 */
स्थिर ध्रुव पूर्णाङ्क start_of_month[2][13] = {
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
    स्थिर पूर्णाङ्क
first_wday_of(पूर्णाङ्क yr)
{
    पटिनिवत्त ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
	    (isleap(yr) ? 6 : 0) + 1) % 7;
}

#define delim(p)	((p) == '\0' || isspace((unsigned वर्ण)(p)))

/*
 * Stub replacing NetBSD's fromzone(): the Windows CRT does not provide
 * tzalloc() / tzgetgmtoff() and the associated IANA tzfile loader, so
 * we simply decline to recognize arbitrary timezone names.  The %Z / %z
 * numeric and RFC-822/2822 forms are still handled by the caller.
 */
    स्थिर पूर्णाङ्क
fromzone(ध्रुव unsigned वर्ण **bp UNUSED, संरचना tm *tm UNUSED,
						    पूर्णाङ्क mandatory UNUSED)
{
    पटिनिवत्त 0;
}

    वर्ण *
strptime(ध्रुव वर्ण *buf, ध्रुव वर्ण *fmt, संरचना tm *tm)
{
    unsigned वर्ण c;
    ध्रुव unsigned वर्ण *bp, *ep, *zname;
    पूर्णाङ्क alt_format, i, split_year = 0, neg = 0, state = 0,
	day_offset = -1, week_offset = 0, offs, mandatory;
    ध्रुव वर्ण *new_fmt;

    bp = (ध्रुव unsigned वर्ण *)buf;

    याव (bp != रिक्त && (c = *fmt++) != '\0')
    {
	/* Clear `alternate' modifier prior to new conversion. */
	alt_format = 0;
	i = 0;

	/* Eat up white-space. */
	सचे (isspace(c))
	{
	    याव (isspace(*bp))
		bp++;
	    अनुवर्त;
	}

	सचे (c != '%')
	    गच्छ literal;


again:	विकल्प (c = *fmt++)
	{
	पक्ष '%':	/* "%%" is converted to "%". */
literal:
	    सचे (c != *bp++)
		पटिनिवत्त रिक्त;
	    LEGAL_ALT(0);
	    अनुवर्त;

	/*
	 * "Alternative" modifiers. Just set the appropriate flag
	 * and start over again.
	 */
	पक्ष 'E':	/* "%E?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_E;
	    गच्छ again;

	पक्ष 'O':	/* "%O?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_O;
	    गच्छ again;

	/*
	 * "Complex" conversion rules, implemented through recursion.
	 */
	पक्ष 'c':	/* Date and time, using the locale's format. */
	    new_fmt = c_d_t_fmt;
	    state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
	    गच्छ recurse;

	पक्ष 'D':	/* The date as "%m/%d/%y". */
	    new_fmt = "%m/%d/%y";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    गच्छ recurse;

	पक्ष 'F':	/* The date as "%Y-%m-%d". */
	    new_fmt = "%Y-%m-%d";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    गच्छ recurse;

	पक्ष 'R':	/* The time as "%H:%M". */
	    new_fmt = "%H:%M";
	    LEGAL_ALT(0);
	    गच्छ recurse;

	पक्ष 'r':	/* The time in 12-hour clock representation. */
	    new_fmt = c_t_fmt_ampm;
	    LEGAL_ALT(0);
	    गच्छ recurse;

	पक्ष 'T':	/* The time as "%H:%M:%S". */
	    new_fmt = "%H:%M:%S";
	    LEGAL_ALT(0);
	    गच्छ recurse;

	पक्ष 'X':	/* The time, using the locale's format. */
	    new_fmt = c_t_fmt;
	    गच्छ recurse;

	पक्ष 'x':	/* The date, using the locale's format. */
	    new_fmt = c_d_fmt;
	    state |= S_MON | S_MDAY | S_YEAR;
	recurse:
	    bp = (ध्रुव unsigned वर्ण *)strptime((ध्रुव वर्ण *)bp,
							new_fmt, tm);
	    LEGAL_ALT(ALT_E);
	    अनुवर्त;

	/*
	 * "Elementary" conversion rules.
	 */
	पक्ष 'A':	/* The day of week, using the locale's form. */
	पक्ष 'a':
	    bp = find_string(bp, &tm->tm_wday, c_day, c_abday, 7);
	    LEGAL_ALT(0);
	    state |= S_WDAY;
	    अनुवर्त;

	पक्ष 'B':	/* The month, using the locale's form. */
	पक्ष 'b':
	पक्ष 'h':
	    bp = find_string(bp, &tm->tm_mon, c_mon, c_abmon, 12);
	    LEGAL_ALT(0);
	    state |= S_MON;
	    अनुवर्त;

	पक्ष 'C':	/* The century number. */
	    i = 20;
	    bp = conv_अङ्कः(bp, &i, 0, 99);

	    i = i * 100 - TM_YEAR_BASE;
	    सचे (split_year)
		i += tm->tm_year % 100;
	    split_year = 1;
	    tm->tm_year = i;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    अनुवर्त;

	पक्ष 'd':	/* The day of month. */
	पक्ष 'e':
	    bp = conv_अङ्कः(bp, &tm->tm_mday, 1, 31);
	    LEGAL_ALT(ALT_O);
	    state |= S_MDAY;
	    अनुवर्त;

	पक्ष 'k':	/* The hour (24-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	पक्ष 'H':
	    bp = conv_अङ्कः(bp, &tm->tm_hour, 0, 23);
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    अनुवर्त;

	पक्ष 'l':	/* The hour (12-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	पक्ष 'I':
	    bp = conv_अङ्कः(bp, &tm->tm_hour, 1, 12);
	    सचे (tm->tm_hour == 12)
		tm->tm_hour = 0;
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    अनुवर्त;

	पक्ष 'j':	/* The day of year. */
	    i = 1;
	    bp = conv_अङ्कः(bp, &i, 1, 366);
	    tm->tm_yday = i - 1;
	    LEGAL_ALT(0);
	    state |= S_YDAY;
	    अनुवर्त;

	पक्ष 'M':	/* The minute. */
	    bp = conv_अङ्कः(bp, &tm->tm_min, 0, 59);
	    LEGAL_ALT(ALT_O);
	    अनुवर्त;

	पक्ष 'm':	/* The month. */
	    i = 1;
	    bp = conv_अङ्कः(bp, &i, 1, 12);
	    tm->tm_mon = i - 1;
	    LEGAL_ALT(ALT_O);
	    state |= S_MON;
	    अनुवर्त;

	पक्ष 'p':	/* The locale's equivalent of AM/PM. */
	    bp = find_string(bp, &i, c_am_pm, रिक्त, 2);
	    सचे (HAVE_HOUR(state) && tm->tm_hour > 11)
		पटिनिवत्त रिक्त;
	    tm->tm_hour += i * 12;
	    LEGAL_ALT(0);
	    अनुवर्त;

	पक्ष 'S':	/* The seconds. */
	    bp = conv_अङ्कः(bp, &tm->tm_sec, 0, 61);
	    LEGAL_ALT(ALT_O);
	    अनुवर्त;

	पक्ष 's':	/* seconds since the epoch */
	    {
		ध्रुव time_t TIME_MAX = (time_t)((परिमाण(time_t) == 8)
					? LLONG_MAX : INT_MAX);
		time_t sse, d;

		सचे (*bp < '0' || *bp > '9')
		{
		    bp = रिक्त;
		    अनुवर्त;
		}

		sse = *bp++ - '0';
		याव (*bp >= '0' && *bp <= '9')
		{
		    d = *bp++ - '0';
		    सचे (sse > TIME_MAX / 10)
		    {
			bp = रिक्त;
			भङ्ग;
		    }
		    sse *= 10;
		    सचे (sse > TIME_MAX - d)
		    {
			bp = रिक्त;
			भङ्ग;
		    }
		    sse += d;
		}
		सचे (bp == रिक्त)
		    अनुवर्त;

		{
		    संरचना tm *lt = localtime(&sse);

		    सचे (lt == रिक्त)
			bp = रिक्त;
		    अञ्ञथा
		    {
			*tm = *lt;
			state |= S_YDAY | S_WDAY |
				S_MON | S_MDAY | S_YEAR;
		    }
		}
		अनुवर्त;
	    }

	पक्ष 'U':	/* The week of year, beginning on sunday. */
	पक्ष 'W':	/* The week of year, beginning on monday. */
	    /*
	     * This is bogus, as we can not assume any valid
	     * information present in the tm structure at this
	     * point to calculate a real value, so save the
	     * week for now in case it can be used later.
	     */
	    bp = conv_अङ्कः(bp, &i, 0, 53);
	    LEGAL_ALT(ALT_O);
	    सचे (c == 'U')
		day_offset = TM_SUNDAY;
	    अञ्ञथा
		day_offset = TM_MONDAY;
	    week_offset = i;
	    अनुवर्त;

	पक्ष 'w':	/* The day of week, beginning on sunday. */
	    bp = conv_अङ्कः(bp, &tm->tm_wday, 0, 6);
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    अनुवर्त;

	पक्ष 'u':	/* The day of week, monday = 1. */
	    bp = conv_अङ्कः(bp, &i, 1, 7);
	    tm->tm_wday = i % 7;
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    अनुवर्त;

	पक्ष 'g':	/* ISO week year without century (parsed, ignored). */
	    bp = conv_अङ्कः(bp, &i, 0, 99);
	    अनुवर्त;

	पक्ष 'G':	/* ISO week year with century (parsed, ignored). */
	    करोहि
		bp++;
	    याव (isdigit(*bp));
	    अनुवर्त;

	पक्ष 'V':	/* ISO 8601:1988 week number (parsed, ignored). */
	    bp = conv_अङ्कः(bp, &i, 1, 53);
	    अनुवर्त;

	पक्ष 'Y':	/* The year. */
	    i = TM_YEAR_BASE;	/* just for data sanity... */
	    bp = conv_अङ्कः(bp, &i, 0, 9999);
	    tm->tm_year = i - TM_YEAR_BASE;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    अनुवर्त;

	पक्ष 'y':	/* The year within 100 years of the epoch. */
	    /* LEGAL_ALT(ALT_E | ALT_O); */
	    bp = conv_अङ्कः(bp, &i, 0, 99);

	    सचे (split_year)
		/* preserve century */
		i += (tm->tm_year / 100) * 100;
	    अञ्ञथा
	    {
		split_year = 1;
		सचे (i <= 68)
		    i = i + 2000 - TM_YEAR_BASE;
		अञ्ञथा
		    i = i + 1900 - TM_YEAR_BASE;
	    }
	    tm->tm_year = i;
	    state |= S_YEAR;
	    अनुवर्त;

	पक्ष 'Z':
	पक्ष 'z':
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
	    सचे (mandatory)
		याव (isspace(*bp))
		    bp++;

	    zname = bp;
	    विकल्प (*bp++)
	    {
	    पक्ष 'G':
		सचे (*bp++ != 'M')
		    गच्छ namedzone;
		/*FALLTHROUGH*/
	    पक्ष 'U':
		सचे (*bp++ != 'T')
		    गच्छ namedzone;
		अञ्ञथा सचे (!delim(*bp) && *bp++ != 'C')
		    गच्छ namedzone;
		/*FALLTHROUGH*/
	    पक्ष 'Z':
		सचे (!delim(*bp))
		    गच्छ namedzone;
		tm->tm_isdst = 0;
		अनुवर्त;
	    पक्ष '+':
		neg = 0;
		भङ्ग;
	    पक्ष '-':
		neg = 1;
		भङ्ग;
	    स्वभाव:
namedzone:
		bp = zname;

		/* Nautical / Military style */
		सचे (delim(bp[1]) &&
			((*bp >= 'A' && *bp <= 'I') ||
			 (*bp >= 'L' && *bp <= 'Y')))
		{
		    bp++;
		    अनुवर्त;
		}
		/* 'J' is local time */
		सचे (delim(bp[1]) && *bp == 'J')
		{
		    bp++;
		    अनुवर्त;
		}

		/*
		 * From our 3 letter hard-coded table.
		 */
		सचे (delim(bp[0]) || delim(bp[1]) ||
			delim(bp[2]) || !delim(bp[3]))
		    गच्छ loadzone;
		ep = find_string(bp, &i, nast, रिक्त, 4);
		सचे (ep != रिक्त)
		{
		    bp = ep;
		    अनुवर्त;
		}
		ep = find_string(bp, &i, nadt, रिक्त, 4);
		सचे (ep != रिक्त)
		{
		    tm->tm_isdst = 1;
		    bp = ep;
		    अनुवर्त;
		}
		/*
		 * Our current timezone.  Prefer _get_tzname() over the
		 * tzname[] global, which UCRT marks deprecated because it
		 * may be inaccurate after locale changes.
		 */
#ifdef USE_GET_TZNAME
		{
		    वर्ण tzbuf[2][32];
		    ध्रुव वर्ण *tznames[2] = { tzbuf[0], tzbuf[1] };
		    size_t tzlen;

		    _tzset();
		    सचे (_get_tzname(&tzlen, tzbuf[0], परिमाण(tzbuf[0]), 0) != 0)
			tzbuf[0][0] = NUL;
		    सचे (_get_tzname(&tzlen, tzbuf[1], परिमाण(tzbuf[1]), 1) != 0)
			tzbuf[1][0] = NUL;
		    ep = find_string(bp, &i, tznames, रिक्त, 2);
		}
#अञ्ञथा
		ep = find_string(bp, &i,
			(ध्रुव वर्ण *ध्रुव *)tzname, रिक्त, 2);
#endif
		सचे (ep != रिक्त)
		{
		    tm->tm_isdst = i;
		    bp = ep;
		    अनुवर्त;
		}
loadzone:
		/*
		 * The hard way, load the zone!
		 */
		सचे (fromzone(&bp, tm, mandatory))
		    अनुवर्त;
		गच्छ out;
	    }
	    offs = 0;
	    अत्थाय (i = 0; i < 4; )
	    {
		सचे (isdigit(*bp))
		{
		    offs = offs * 10 + (*bp++ - '0');
		    i++;
		    अनुवर्त;
		}
		सचे (i == 2 && *bp == ':')
		{
		    bp++;
		    अनुवर्त;
		}
		भङ्ग;
	    }
	    सचे (isdigit(*bp))
		गच्छ out;
	    विकल्प (i)
	    {
	    पक्ष 2:
		offs *= SECSPERHOUR;
		भङ्ग;
	    पक्ष 4:
		i = offs % 100;
		offs /= 100;
		सचे (i >= SECSPERMIN)
		    गच्छ out;
		/* Convert minutes into decimal */
		offs = offs * SECSPERHOUR + i * SECSPERMIN;
		भङ्ग;
	    स्वभाव:
out:
		सचे (mandatory)
		    पटिनिवत्त रिक्त;
		bp = zname;
		अनुवर्त;
	    }
	    /* ISO 8601 & RFC 3339 limit to 23:59 max */
	    सचे (offs >= (HOURSPERDAY * SECSPERHOUR))
		गच्छ out;
	    सचे (neg)
		offs = -offs;
	    tm->tm_isdst = 0;	/* XXX */
	    अनुवर्त;

	/*
	 * Miscellaneous conversions.
	 */
	पक्ष 'n':	/* Any kind of white-space. */
	पक्ष 't':
	    याव (isspace(*bp))
		bp++;
	    LEGAL_ALT(0);
	    अनुवर्त;


	स्वभाव:	/* Unknown/unsupported conversion. */
	    पटिनिवत्त रिक्त;
	}
    }

    सचे (!HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	सचे (HAVE_MON(state) && HAVE_MDAY(state))
	{
	    /* calculate day of year (ordinal date) */
	    tm->tm_yday =  start_of_month[isleap_sum(tm->tm_year,
			    TM_YEAR_BASE)][tm->tm_mon] + (tm->tm_mday - 1);
	    state |= S_YDAY;
	}
	अञ्ञथा सचे (day_offset != -1)
	{
	    /*
	     * Set the date to the first Sunday (or Monday)
	     * of the specified week of the year.
	     */
	    सचे (!HAVE_WDAY(state))
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

    सचे (HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	पूर्णाङ्क leap;

	सचे (!HAVE_MON(state))
	{
	    /* calculate month of day of year */
	    i = 0;
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    याव (tm->tm_yday >= start_of_month[leap][i])
		i++;
	    सचे (i > 12)
	    {
		i = 1;
		tm->tm_yday -= start_of_month[leap][12];
		tm->tm_year++;
	    }
	    tm->tm_mon = i - 1;
	    state |= S_MON;
	}

	सचे (!HAVE_MDAY(state))
	{
	    /* calculate day of month */
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    tm->tm_mday = tm->tm_yday -
		start_of_month[leap][tm->tm_mon] + 1;
	    state |= S_MDAY;
	}

	सचे (!HAVE_WDAY(state))
	{
	    /* calculate day of week */
	    i = 0;
	    week_offset = first_wday_of(tm->tm_year);
	    याव (i++ <= tm->tm_yday)
	    {
		सचे (week_offset++ >= 6)
		    week_offset = 0;
	    }
	    tm->tm_wday = week_offset;
	    state |= S_WDAY;
	}
    }

    पटिनिवत्त (वर्ण *)bp;
}


    स्थिर ध्रुव unsigned वर्ण *
conv_अङ्कः(ध्रुव unsigned वर्ण *buf, पूर्णाङ्क *dest, unsigned पूर्णाङ्क llim,
						    unsigned पूर्णाङ्क ulim)
{
    unsigned पूर्णाङ्क result = 0;
    unsigned वर्ण ch;

    /* The limit also determines the number of valid digits. */
    unsigned पूर्णाङ्क rulim = ulim;

    ch = *buf;
    सचे (ch < '0' || ch > '9')
	पटिनिवत्त रिक्त;

    करोहि
    {
	result *= 10;
	result += ch - '0';
	rulim /= 10;
	ch = *++buf;
    } याव ((result * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

    सचे (result < llim || result > ulim)
	पटिनिवत्त रिक्त;

    *dest = result;
    पटिनिवत्त buf;
}

    स्थिर ध्रुव unsigned वर्ण *
find_string(ध्रुव unsigned वर्ण *bp, पूर्णाङ्क *tgt, ध्रुव वर्ण *ध्रुव *n1,
					    ध्रुव वर्ण *ध्रुव *n2, पूर्णाङ्क c)
{
    पूर्णाङ्क		i;
    size_t	len;

    /* check full name - then abbreviated ones */
    अत्थाय (; n1 != रिक्त; n1 = n2, n2 = रिक्त)
    {
	अत्थाय (i = 0; i < c; i++, n1++)
	{
	    len = strlen(*n1);
	    सचे (STRNICMP(*n1, bp, len) == 0)
	    {
		*tgt = i;
		पटिनिवत्त bp + len;
	    }
	}
    }

    /* Nothing matched */
    पटिनिवत्त रिक्त;
}

#endif // MSWIN
