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

#شامل "vim.h"

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
#اگر defined(_UCRT) || defined(_MSC_VER)
# define USE_GET_TZNAME
#endif

/* Locale tables (English / "C" locale). */
ایستا ثابت نویسه *ثابت c_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
ایستا ثابت نویسه *ثابت c_abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
ایستا ثابت نویسه *ثابت c_mon[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
ایستا ثابت نویسه *ثابت c_abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
ایستا ثابت نویسه *ثابت c_am_pm[] = { "AM", "PM" };
ایستا ثابت نویسه c_d_t_fmt[] = "%a %b %e %H:%M:%S %Y";
ایستا ثابت نویسه c_d_fmt[] = "%m/%d/%y";
ایستا ثابت نویسه c_t_fmt[] = "%H:%M:%S";
ایستا ثابت نویسه c_t_fmt_ampm[] = "%I:%M:%S %p";

ایستا ثابت unsigned نویسه *conv_رقم(ثابت unsigned نویسه *, صحیح *,
						unsigned صحیح, unsigned صحیح);
ایستا ثابت unsigned نویسه *find_string(ثابت unsigned نویسه *, صحیح *,
			ثابت نویسه * ثابت *, ثابت نویسه * ثابت *, صحیح);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E			0x01
#define ALT_O			0x02
#define LEGAL_ALT(x)		{ اگر (alt_format & ~(x)) بازگشت پوچ; }

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
ایستا ثابت نویسه *ثابت nast[5] = {
    "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
ایستا ثابت نویسه *ثابت nadt[5] = {
    "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};

/*
 * Table to determine the ordinal date for the start of a month.
 * Ref: http://en.wikipedia.org/wiki/ISO_week_date
 */
ایستا ثابت صحیح start_of_month[2][13] = {
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
    ایستا صحیح
first_wday_of(صحیح yr)
{
    بازگشت ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
	    (isleap(yr) ? 6 : 0) + 1) % 7;
}

#define delim(p)	((p) == '\0' || isspace((unsigned نویسه)(p)))

/*
 * Stub replacing NetBSD's fromzone(): the Windows CRT does not provide
 * tzalloc() / tzgetgmtoff() and the associated IANA tzfile loader, so
 * we simply decline to recognize arbitrary timezone names.  The %Z / %z
 * numeric and RFC-822/2822 forms are still handled by the caller.
 */
    ایستا صحیح
fromzone(ثابت unsigned نویسه **bp UNUSED, ساختار tm *tm UNUSED,
						    صحیح mandatory UNUSED)
{
    بازگشت 0;
}

    نویسه *
strptime(ثابت نویسه *buf, ثابت نویسه *fmt, ساختار tm *tm)
{
    unsigned نویسه c;
    ثابت unsigned نویسه *bp, *ep, *zname;
    صحیح alt_format, i, split_year = 0, neg = 0, state = 0,
	day_offset = -1, week_offset = 0, offs, mandatory;
    ثابت نویسه *new_fmt;

    bp = (ثابت unsigned نویسه *)buf;

    وقتی (bp != پوچ && (c = *fmt++) != '\0')
    {
	/* Clear `alternate' modifier prior to new conversion. */
	alt_format = 0;
	i = 0;

	/* Eat up white-space. */
	اگر (isspace(c))
	{
	    وقتی (isspace(*bp))
		bp++;
	    ادامه;
	}

	اگر (c != '%')
	    برو literal;


again:	انتخاب (c = *fmt++)
	{
	حالت '%':	/* "%%" is converted to "%". */
literal:
	    اگر (c != *bp++)
		بازگشت پوچ;
	    LEGAL_ALT(0);
	    ادامه;

	/*
	 * "Alternative" modifiers. Just set the appropriate flag
	 * and start over again.
	 */
	حالت 'E':	/* "%E?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_E;
	    برو again;

	حالت 'O':	/* "%O?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_O;
	    برو again;

	/*
	 * "Complex" conversion rules, implemented through recursion.
	 */
	حالت 'c':	/* Date and time, using the locale's format. */
	    new_fmt = c_d_t_fmt;
	    state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
	    برو recurse;

	حالت 'D':	/* The date as "%m/%d/%y". */
	    new_fmt = "%m/%d/%y";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    برو recurse;

	حالت 'F':	/* The date as "%Y-%m-%d". */
	    new_fmt = "%Y-%m-%d";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    برو recurse;

	حالت 'R':	/* The time as "%H:%M". */
	    new_fmt = "%H:%M";
	    LEGAL_ALT(0);
	    برو recurse;

	حالت 'r':	/* The time in 12-hour clock representation. */
	    new_fmt = c_t_fmt_ampm;
	    LEGAL_ALT(0);
	    برو recurse;

	حالت 'T':	/* The time as "%H:%M:%S". */
	    new_fmt = "%H:%M:%S";
	    LEGAL_ALT(0);
	    برو recurse;

	حالت 'X':	/* The time, using the locale's format. */
	    new_fmt = c_t_fmt;
	    برو recurse;

	حالت 'x':	/* The date, using the locale's format. */
	    new_fmt = c_d_fmt;
	    state |= S_MON | S_MDAY | S_YEAR;
	recurse:
	    bp = (ثابت unsigned نویسه *)strptime((ثابت نویسه *)bp,
							new_fmt, tm);
	    LEGAL_ALT(ALT_E);
	    ادامه;

	/*
	 * "Elementary" conversion rules.
	 */
	حالت 'A':	/* The day of week, using the locale's form. */
	حالت 'a':
	    bp = find_string(bp, &tm->tm_wday, c_day, c_abday, 7);
	    LEGAL_ALT(0);
	    state |= S_WDAY;
	    ادامه;

	حالت 'B':	/* The month, using the locale's form. */
	حالت 'b':
	حالت 'h':
	    bp = find_string(bp, &tm->tm_mon, c_mon, c_abmon, 12);
	    LEGAL_ALT(0);
	    state |= S_MON;
	    ادامه;

	حالت 'C':	/* The century number. */
	    i = 20;
	    bp = conv_رقم(bp, &i, 0, 99);

	    i = i * 100 - TM_YEAR_BASE;
	    اگر (split_year)
		i += tm->tm_year % 100;
	    split_year = 1;
	    tm->tm_year = i;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    ادامه;

	حالت 'd':	/* The day of month. */
	حالت 'e':
	    bp = conv_رقم(bp, &tm->tm_mday, 1, 31);
	    LEGAL_ALT(ALT_O);
	    state |= S_MDAY;
	    ادامه;

	حالت 'k':	/* The hour (24-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	حالت 'H':
	    bp = conv_رقم(bp, &tm->tm_hour, 0, 23);
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    ادامه;

	حالت 'l':	/* The hour (12-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	حالت 'I':
	    bp = conv_رقم(bp, &tm->tm_hour, 1, 12);
	    اگر (tm->tm_hour == 12)
		tm->tm_hour = 0;
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    ادامه;

	حالت 'j':	/* The day of year. */
	    i = 1;
	    bp = conv_رقم(bp, &i, 1, 366);
	    tm->tm_yday = i - 1;
	    LEGAL_ALT(0);
	    state |= S_YDAY;
	    ادامه;

	حالت 'M':	/* The minute. */
	    bp = conv_رقم(bp, &tm->tm_min, 0, 59);
	    LEGAL_ALT(ALT_O);
	    ادامه;

	حالت 'm':	/* The month. */
	    i = 1;
	    bp = conv_رقم(bp, &i, 1, 12);
	    tm->tm_mon = i - 1;
	    LEGAL_ALT(ALT_O);
	    state |= S_MON;
	    ادامه;

	حالت 'p':	/* The locale's equivalent of AM/PM. */
	    bp = find_string(bp, &i, c_am_pm, پوچ, 2);
	    اگر (HAVE_HOUR(state) && tm->tm_hour > 11)
		بازگشت پوچ;
	    tm->tm_hour += i * 12;
	    LEGAL_ALT(0);
	    ادامه;

	حالت 'S':	/* The seconds. */
	    bp = conv_رقم(bp, &tm->tm_sec, 0, 61);
	    LEGAL_ALT(ALT_O);
	    ادامه;

	حالت 's':	/* seconds since the epoch */
	    {
		ثابت time_t TIME_MAX = (time_t)((اندازه(time_t) == 8)
					? LLONG_MAX : INT_MAX);
		time_t sse, d;

		اگر (*bp < '0' || *bp > '9')
		{
		    bp = پوچ;
		    ادامه;
		}

		sse = *bp++ - '0';
		وقتی (*bp >= '0' && *bp <= '9')
		{
		    d = *bp++ - '0';
		    اگر (sse > TIME_MAX / 10)
		    {
			bp = پوچ;
			بشکن;
		    }
		    sse *= 10;
		    اگر (sse > TIME_MAX - d)
		    {
			bp = پوچ;
			بشکن;
		    }
		    sse += d;
		}
		اگر (bp == پوچ)
		    ادامه;

		{
		    ساختار tm *lt = localtime(&sse);

		    اگر (lt == پوچ)
			bp = پوچ;
		    وگرنه
		    {
			*tm = *lt;
			state |= S_YDAY | S_WDAY |
				S_MON | S_MDAY | S_YEAR;
		    }
		}
		ادامه;
	    }

	حالت 'U':	/* The week of year, beginning on sunday. */
	حالت 'W':	/* The week of year, beginning on monday. */
	    /*
	     * This is bogus, as we can not assume any valid
	     * information present in the tm structure at this
	     * point to calculate a real value, so save the
	     * week for now in case it can be used later.
	     */
	    bp = conv_رقم(bp, &i, 0, 53);
	    LEGAL_ALT(ALT_O);
	    اگر (c == 'U')
		day_offset = TM_SUNDAY;
	    وگرنه
		day_offset = TM_MONDAY;
	    week_offset = i;
	    ادامه;

	حالت 'w':	/* The day of week, beginning on sunday. */
	    bp = conv_رقم(bp, &tm->tm_wday, 0, 6);
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    ادامه;

	حالت 'u':	/* The day of week, monday = 1. */
	    bp = conv_رقم(bp, &i, 1, 7);
	    tm->tm_wday = i % 7;
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    ادامه;

	حالت 'g':	/* ISO week year without century (parsed, ignored). */
	    bp = conv_رقم(bp, &i, 0, 99);
	    ادامه;

	حالت 'G':	/* ISO week year with century (parsed, ignored). */
	    انجامبده
		bp++;
	    وقتی (isdigit(*bp));
	    ادامه;

	حالت 'V':	/* ISO 8601:1988 week number (parsed, ignored). */
	    bp = conv_رقم(bp, &i, 1, 53);
	    ادامه;

	حالت 'Y':	/* The year. */
	    i = TM_YEAR_BASE;	/* just for data sanity... */
	    bp = conv_رقم(bp, &i, 0, 9999);
	    tm->tm_year = i - TM_YEAR_BASE;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    ادامه;

	حالت 'y':	/* The year within 100 years of the epoch. */
	    /* LEGAL_ALT(ALT_E | ALT_O); */
	    bp = conv_رقم(bp, &i, 0, 99);

	    اگر (split_year)
		/* preserve century */
		i += (tm->tm_year / 100) * 100;
	    وگرنه
	    {
		split_year = 1;
		اگر (i <= 68)
		    i = i + 2000 - TM_YEAR_BASE;
		وگرنه
		    i = i + 1900 - TM_YEAR_BASE;
	    }
	    tm->tm_year = i;
	    state |= S_YEAR;
	    ادامه;

	حالت 'Z':
	حالت 'z':
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
	    اگر (mandatory)
		وقتی (isspace(*bp))
		    bp++;

	    zname = bp;
	    انتخاب (*bp++)
	    {
	    حالت 'G':
		اگر (*bp++ != 'M')
		    برو namedzone;
		/*FALLTHROUGH*/
	    حالت 'U':
		اگر (*bp++ != 'T')
		    برو namedzone;
		وگرنه اگر (!delim(*bp) && *bp++ != 'C')
		    برو namedzone;
		/*FALLTHROUGH*/
	    حالت 'Z':
		اگر (!delim(*bp))
		    برو namedzone;
		tm->tm_isdst = 0;
		ادامه;
	    حالت '+':
		neg = 0;
		بشکن;
	    حالت '-':
		neg = 1;
		بشکن;
	    پیشفرض:
namedzone:
		bp = zname;

		/* Nautical / Military style */
		اگر (delim(bp[1]) &&
			((*bp >= 'A' && *bp <= 'I') ||
			 (*bp >= 'L' && *bp <= 'Y')))
		{
		    bp++;
		    ادامه;
		}
		/* 'J' is local time */
		اگر (delim(bp[1]) && *bp == 'J')
		{
		    bp++;
		    ادامه;
		}

		/*
		 * From our 3 letter hard-coded table.
		 */
		اگر (delim(bp[0]) || delim(bp[1]) ||
			delim(bp[2]) || !delim(bp[3]))
		    برو loadzone;
		ep = find_string(bp, &i, nast, پوچ, 4);
		اگر (ep != پوچ)
		{
		    bp = ep;
		    ادامه;
		}
		ep = find_string(bp, &i, nadt, پوچ, 4);
		اگر (ep != پوچ)
		{
		    tm->tm_isdst = 1;
		    bp = ep;
		    ادامه;
		}
		/*
		 * Our current timezone.  Prefer _get_tzname() over the
		 * tzname[] global, which UCRT marks deprecated because it
		 * may be inaccurate after locale changes.
		 */
#ifdef USE_GET_TZNAME
		{
		    نویسه tzbuf[2][32];
		    ثابت نویسه *tznames[2] = { tzbuf[0], tzbuf[1] };
		    size_t tzlen;

		    _tzset();
		    اگر (_get_tzname(&tzlen, tzbuf[0], اندازه(tzbuf[0]), 0) != 0)
			tzbuf[0][0] = NUL;
		    اگر (_get_tzname(&tzlen, tzbuf[1], اندازه(tzbuf[1]), 1) != 0)
			tzbuf[1][0] = NUL;
		    ep = find_string(bp, &i, tznames, پوچ, 2);
		}
#وگرنه
		ep = find_string(bp, &i,
			(ثابت نویسه *ثابت *)tzname, پوچ, 2);
#endif
		اگر (ep != پوچ)
		{
		    tm->tm_isdst = i;
		    bp = ep;
		    ادامه;
		}
loadzone:
		/*
		 * The hard way, load the zone!
		 */
		اگر (fromzone(&bp, tm, mandatory))
		    ادامه;
		برو out;
	    }
	    offs = 0;
	    برای (i = 0; i < 4; )
	    {
		اگر (isdigit(*bp))
		{
		    offs = offs * 10 + (*bp++ - '0');
		    i++;
		    ادامه;
		}
		اگر (i == 2 && *bp == ':')
		{
		    bp++;
		    ادامه;
		}
		بشکن;
	    }
	    اگر (isdigit(*bp))
		برو out;
	    انتخاب (i)
	    {
	    حالت 2:
		offs *= SECSPERHOUR;
		بشکن;
	    حالت 4:
		i = offs % 100;
		offs /= 100;
		اگر (i >= SECSPERMIN)
		    برو out;
		/* Convert minutes into decimal */
		offs = offs * SECSPERHOUR + i * SECSPERMIN;
		بشکن;
	    پیشفرض:
out:
		اگر (mandatory)
		    بازگشت پوچ;
		bp = zname;
		ادامه;
	    }
	    /* ISO 8601 & RFC 3339 limit to 23:59 max */
	    اگر (offs >= (HOURSPERDAY * SECSPERHOUR))
		برو out;
	    اگر (neg)
		offs = -offs;
	    tm->tm_isdst = 0;	/* XXX */
	    ادامه;

	/*
	 * Miscellaneous conversions.
	 */
	حالت 'n':	/* Any kind of white-space. */
	حالت 't':
	    وقتی (isspace(*bp))
		bp++;
	    LEGAL_ALT(0);
	    ادامه;


	پیشفرض:	/* Unknown/unsupported conversion. */
	    بازگشت پوچ;
	}
    }

    اگر (!HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	اگر (HAVE_MON(state) && HAVE_MDAY(state))
	{
	    /* calculate day of year (ordinal date) */
	    tm->tm_yday =  start_of_month[isleap_sum(tm->tm_year,
			    TM_YEAR_BASE)][tm->tm_mon] + (tm->tm_mday - 1);
	    state |= S_YDAY;
	}
	وگرنه اگر (day_offset != -1)
	{
	    /*
	     * Set the date to the first Sunday (or Monday)
	     * of the specified week of the year.
	     */
	    اگر (!HAVE_WDAY(state))
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

    اگر (HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	صحیح leap;

	اگر (!HAVE_MON(state))
	{
	    /* calculate month of day of year */
	    i = 0;
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    وقتی (tm->tm_yday >= start_of_month[leap][i])
		i++;
	    اگر (i > 12)
	    {
		i = 1;
		tm->tm_yday -= start_of_month[leap][12];
		tm->tm_year++;
	    }
	    tm->tm_mon = i - 1;
	    state |= S_MON;
	}

	اگر (!HAVE_MDAY(state))
	{
	    /* calculate day of month */
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    tm->tm_mday = tm->tm_yday -
		start_of_month[leap][tm->tm_mon] + 1;
	    state |= S_MDAY;
	}

	اگر (!HAVE_WDAY(state))
	{
	    /* calculate day of week */
	    i = 0;
	    week_offset = first_wday_of(tm->tm_year);
	    وقتی (i++ <= tm->tm_yday)
	    {
		اگر (week_offset++ >= 6)
		    week_offset = 0;
	    }
	    tm->tm_wday = week_offset;
	    state |= S_WDAY;
	}
    }

    بازگشت (نویسه *)bp;
}


    ایستا ثابت unsigned نویسه *
conv_رقم(ثابت unsigned نویسه *buf, صحیح *dest, unsigned صحیح llim,
						    unsigned صحیح ulim)
{
    unsigned صحیح result = 0;
    unsigned نویسه ch;

    /* The limit also determines the number of valid digits. */
    unsigned صحیح rulim = ulim;

    ch = *buf;
    اگر (ch < '0' || ch > '9')
	بازگشت پوچ;

    انجامبده
    {
	result *= 10;
	result += ch - '0';
	rulim /= 10;
	ch = *++buf;
    } وقتی ((result * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

    اگر (result < llim || result > ulim)
	بازگشت پوچ;

    *dest = result;
    بازگشت buf;
}

    ایستا ثابت unsigned نویسه *
find_string(ثابت unsigned نویسه *bp, صحیح *tgt, ثابت نویسه *ثابت *n1,
					    ثابت نویسه *ثابت *n2, صحیح c)
{
    صحیح		i;
    size_t	len;

    /* check full name - then abbreviated ones */
    برای (; n1 != پوچ; n1 = n2, n2 = پوچ)
    {
	برای (i = 0; i < c; i++, n1++)
	{
	    len = strlen(*n1);
	    اگر (STRNICMP(*n1, bp, len) == 0)
	    {
		*tgt = i;
		بازگشت bp + len;
	    }
	}
    }

    /* Nothing matched */
    بازگشت پوچ;
}

#endif // MSWIN
