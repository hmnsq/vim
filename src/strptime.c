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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN

/* Constants lifted from NetBSD's <tzfile.h>. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TM_YEAR_ਅਧਾਰ	1900
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TM_SUNDAY	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TM_MONDAY	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SECSPERHOUR	3600
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SECSPERMIN	60
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HOURSPERDAY	24

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isleap(y)	(((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)
/* isleap_sum avoids integer overflow when adding the 1900 base. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isleap_ਜੋੜ(a, b) \
			isleap(((ਬਿਨਾਨਿਸ਼ਾਨੀ)(a) + (ਬਿਨਾਨਿਸ਼ਾਨੀ)(b)) % 400)

/*
 * _get_tzname() is provided by the UCRT and by the MSVC CRT.  Older MinGW
 * builds linking against msvcrt.dll fall back to the deprecated tzname[]
 * global (mingw-w64 already exposes the POSIX-named tzname[]).
 */
#ਜੇ defined(_UCRT) || defined(_MSC_VER)
# define USE_GET_TZNAME
#ਜੇ_ਅੰਤ

/* Locale tables (English / "C" locale). */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ c_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ c_abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ c_mon[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ c_abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ c_am_pm[] = { "AM", "PM" };
ਸਥਿਰ ਅਚਲ ਅੱਖਰ c_d_t_fmt[] = "%a %b %e %H:%M:%S %Y";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ c_d_fmt[] = "%m/%d/%y";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ c_t_fmt[] = "%H:%M:%S";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ c_t_fmt_ampm[] = "%I:%M:%S %p";

ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *conv_ਅੰਕ(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *, ਪੂਰਨ *,
						ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ);
ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਲੱਭੋ_string(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *, ਪੂਰਨ *,
			ਅਚਲ ਅੱਖਰ * ਅਚਲ *, ਅਚਲ ਅੱਖਰ * ਅਚਲ *, ਪੂਰਨ);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALT_E			0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALT_O			0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LEGAL_ALT(x)		{ ਜੇ (alt_format & ~(x)) ਵਾਪਸ ਨਿਰਾ; }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ S_YEAR			(1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ S_MON			(1 << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ S_YDAY			(1 << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ S_MDAY			(1 << 3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ S_WDAY			(1 << 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ S_HOUR			(1 << 5)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_MDAY(s)		(s & S_MDAY)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_MON(s)		(s & S_MON)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_WDAY(s)		(s & S_WDAY)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_YDAY(s)		(s & S_YDAY)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_YEAR(s)		(s & S_YEAR)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_HOUR(s)		(s & S_HOUR)

/* RFC-822/RFC-2822 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ nast[5] = {
    "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ nadt[5] = {
    "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};

/*
 * Table to determine the ordinal date for the start of a month.
 * Ref: http://en.wikipedia.org/wiki/ISO_week_date
 */
ਸਥਿਰ ਅਚਲ ਪੂਰਨ ਸ਼ੁਰੂ_ਦਾ_month[2][13] = {
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
    ਸਥਿਰ ਪੂਰਨ
ਪਹਿਲਾ_wday_ਦਾ(ਪੂਰਨ yr)
{
    ਵਾਪਸ ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
	    (isleap(yr) ? 6 : 0) + 1) % 7;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ delim(p)	((p) == '\0' || ਖਾਲੀ_ਥਾਂ_ਹੈ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(p)))

/*
 * Stub replacing NetBSD's fromzone(): the Windows CRT does not provide
 * tzalloc() / tzgetgmtoff() and the associated IANA tzfile loader, so
 * we simply decline to recognize arbitrary timezone names.  The %Z / %z
 * numeric and RFC-822/2822 forms are still handled by the caller.
 */
    ਸਥਿਰ ਪੂਰਨ
fromzone(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **bp ਅਣਵਰਤਿਆ, ਬਣਤਰ tm *tm ਅਣਵਰਤਿਆ,
						    ਪੂਰਨ mandatory ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ 0;
}

    ਅੱਖਰ *
strptime(ਅਚਲ ਅੱਖਰ *buf, ਅਚਲ ਅੱਖਰ *fmt, ਬਣਤਰ tm *tm)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *bp, *ep, *zname;
    ਪੂਰਨ alt_format, i, ਵੰਡੋ_year = 0, neg = 0, state = 0,
	day_ਵਿੱਥ = -1, week_ਵਿੱਥ = 0, offs, mandatory;
    ਅਚਲ ਅੱਖਰ *new_fmt;

    bp = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)buf;

    ਜਦੋਂ (bp != ਨਿਰਾ && (c = *fmt++) != '\0')
    {
	/* Clear `alternate' modifier prior to new conversion. */
	alt_format = 0;
	i = 0;

	/* Eat up white-space. */
	ਜੇ (ਖਾਲੀ_ਥਾਂ_ਹੈ(c))
	{
	    ਜਦੋਂ (ਖਾਲੀ_ਥਾਂ_ਹੈ(*bp))
		bp++;
	    ਜਾਰੀ;
	}

	ਜੇ (c != '%')
	    ਜਾਓ literal;


again:	ਚੋਣ (c = *fmt++)
	{
	ਹਾਲਤ '%':	/* "%%" is converted to "%". */
literal:
	    ਜੇ (c != *bp++)
		ਵਾਪਸ ਨਿਰਾ;
	    LEGAL_ALT(0);
	    ਜਾਰੀ;

	/*
	 * "Alternative" modifiers. Just set the appropriate flag
	 * and start over again.
	 */
	ਹਾਲਤ 'E':	/* "%E?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_E;
	    ਜਾਓ again;

	ਹਾਲਤ 'O':	/* "%O?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_O;
	    ਜਾਓ again;

	/*
	 * "Complex" conversion rules, implemented through recursion.
	 */
	ਹਾਲਤ 'c':	/* Date and time, using the locale's format. */
	    new_fmt = c_d_t_fmt;
	    state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
	    ਜਾਓ recurse;

	ਹਾਲਤ 'D':	/* The date as "%m/%d/%y". */
	    new_fmt = "%m/%d/%y";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    ਜਾਓ recurse;

	ਹਾਲਤ 'F':	/* The date as "%Y-%m-%d". */
	    new_fmt = "%Y-%m-%d";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    ਜਾਓ recurse;

	ਹਾਲਤ 'R':	/* The time as "%H:%M". */
	    new_fmt = "%H:%M";
	    LEGAL_ALT(0);
	    ਜਾਓ recurse;

	ਹਾਲਤ 'r':	/* The time in 12-hour clock representation. */
	    new_fmt = c_t_fmt_ampm;
	    LEGAL_ALT(0);
	    ਜਾਓ recurse;

	ਹਾਲਤ 'T':	/* The time as "%H:%M:%S". */
	    new_fmt = "%H:%M:%S";
	    LEGAL_ALT(0);
	    ਜਾਓ recurse;

	ਹਾਲਤ 'X':	/* The time, using the locale's format. */
	    new_fmt = c_t_fmt;
	    ਜਾਓ recurse;

	ਹਾਲਤ 'x':	/* The date, using the locale's format. */
	    new_fmt = c_d_fmt;
	    state |= S_MON | S_MDAY | S_YEAR;
	recurse:
	    bp = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)strptime((ਅਚਲ ਅੱਖਰ *)bp,
							new_fmt, tm);
	    LEGAL_ALT(ALT_E);
	    ਜਾਰੀ;

	/*
	 * "Elementary" conversion rules.
	 */
	ਹਾਲਤ 'A':	/* The day of week, using the locale's form. */
	ਹਾਲਤ 'a':
	    bp = ਲੱਭੋ_string(bp, &tm->tm_wday, c_day, c_abday, 7);
	    LEGAL_ALT(0);
	    state |= S_WDAY;
	    ਜਾਰੀ;

	ਹਾਲਤ 'B':	/* The month, using the locale's form. */
	ਹਾਲਤ 'b':
	ਹਾਲਤ 'h':
	    bp = ਲੱਭੋ_string(bp, &tm->tm_mon, c_mon, c_abmon, 12);
	    LEGAL_ALT(0);
	    state |= S_MON;
	    ਜਾਰੀ;

	ਹਾਲਤ 'C':	/* The century number. */
	    i = 20;
	    bp = conv_ਅੰਕ(bp, &i, 0, 99);

	    i = i * 100 - TM_YEAR_ਅਧਾਰ;
	    ਜੇ (ਵੰਡੋ_year)
		i += tm->tm_year % 100;
	    ਵੰਡੋ_year = 1;
	    tm->tm_year = i;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    ਜਾਰੀ;

	ਹਾਲਤ 'd':	/* The day of month. */
	ਹਾਲਤ 'e':
	    bp = conv_ਅੰਕ(bp, &tm->tm_mday, 1, 31);
	    LEGAL_ALT(ALT_O);
	    state |= S_MDAY;
	    ਜਾਰੀ;

	ਹਾਲਤ 'k':	/* The hour (24-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	ਹਾਲਤ 'H':
	    bp = conv_ਅੰਕ(bp, &tm->tm_hour, 0, 23);
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    ਜਾਰੀ;

	ਹਾਲਤ 'l':	/* The hour (12-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	ਹਾਲਤ 'I':
	    bp = conv_ਅੰਕ(bp, &tm->tm_hour, 1, 12);
	    ਜੇ (tm->tm_hour == 12)
		tm->tm_hour = 0;
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    ਜਾਰੀ;

	ਹਾਲਤ 'j':	/* The day of year. */
	    i = 1;
	    bp = conv_ਅੰਕ(bp, &i, 1, 366);
	    tm->tm_yday = i - 1;
	    LEGAL_ALT(0);
	    state |= S_YDAY;
	    ਜਾਰੀ;

	ਹਾਲਤ 'M':	/* The minute. */
	    bp = conv_ਅੰਕ(bp, &tm->tm_min, 0, 59);
	    LEGAL_ALT(ALT_O);
	    ਜਾਰੀ;

	ਹਾਲਤ 'm':	/* The month. */
	    i = 1;
	    bp = conv_ਅੰਕ(bp, &i, 1, 12);
	    tm->tm_mon = i - 1;
	    LEGAL_ALT(ALT_O);
	    state |= S_MON;
	    ਜਾਰੀ;

	ਹਾਲਤ 'p':	/* The locale's equivalent of AM/PM. */
	    bp = ਲੱਭੋ_string(bp, &i, c_am_pm, ਨਿਰਾ, 2);
	    ਜੇ (HAVE_HOUR(state) && tm->tm_hour > 11)
		ਵਾਪਸ ਨਿਰਾ;
	    tm->tm_hour += i * 12;
	    LEGAL_ALT(0);
	    ਜਾਰੀ;

	ਹਾਲਤ 'S':	/* The seconds. */
	    bp = conv_ਅੰਕ(bp, &tm->tm_sec, 0, 61);
	    LEGAL_ALT(ALT_O);
	    ਜਾਰੀ;

	ਹਾਲਤ 's':	/* seconds since the epoch */
	    {
		ਅਚਲ time_t ਸਮਾਂ_MAX = (time_t)((ਆਕਾਰ(time_t) == 8)
					? LLONG_MAX : ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
		time_t sse, d;

		ਜੇ (*bp < '0' || *bp > '9')
		{
		    bp = ਨਿਰਾ;
		    ਜਾਰੀ;
		}

		sse = *bp++ - '0';
		ਜਦੋਂ (*bp >= '0' && *bp <= '9')
		{
		    d = *bp++ - '0';
		    ਜੇ (sse > ਸਮਾਂ_MAX / 10)
		    {
			bp = ਨਿਰਾ;
			ਰੋਕੋ;
		    }
		    sse *= 10;
		    ਜੇ (sse > ਸਮਾਂ_MAX - d)
		    {
			bp = ਨਿਰਾ;
			ਰੋਕੋ;
		    }
		    sse += d;
		}
		ਜੇ (bp == ਨਿਰਾ)
		    ਜਾਰੀ;

		{
		    ਬਣਤਰ tm *lt = localtime(&sse);

		    ਜੇ (lt == ਨਿਰਾ)
			bp = ਨਿਰਾ;
		    ਨਹੀਂਤਾਂ
		    {
			*tm = *lt;
			state |= S_YDAY | S_WDAY |
				S_MON | S_MDAY | S_YEAR;
		    }
		}
		ਜਾਰੀ;
	    }

	ਹਾਲਤ 'U':	/* The week of year, beginning on sunday. */
	ਹਾਲਤ 'W':	/* The week of year, beginning on monday. */
	    /*
	     * This is bogus, as we can not assume any valid
	     * information present in the tm structure at this
	     * point to calculate a real value, so save the
	     * week for now in case it can be used later.
	     */
	    bp = conv_ਅੰਕ(bp, &i, 0, 53);
	    LEGAL_ALT(ALT_O);
	    ਜੇ (c == 'U')
		day_ਵਿੱਥ = TM_SUNDAY;
	    ਨਹੀਂਤਾਂ
		day_ਵਿੱਥ = TM_MONDAY;
	    week_ਵਿੱਥ = i;
	    ਜਾਰੀ;

	ਹਾਲਤ 'w':	/* The day of week, beginning on sunday. */
	    bp = conv_ਅੰਕ(bp, &tm->tm_wday, 0, 6);
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    ਜਾਰੀ;

	ਹਾਲਤ 'u':	/* The day of week, monday = 1. */
	    bp = conv_ਅੰਕ(bp, &i, 1, 7);
	    tm->tm_wday = i % 7;
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    ਜਾਰੀ;

	ਹਾਲਤ 'g':	/* ISO week year without century (parsed, ignored). */
	    bp = conv_ਅੰਕ(bp, &i, 0, 99);
	    ਜਾਰੀ;

	ਹਾਲਤ 'G':	/* ISO week year with century (parsed, ignored). */
	    ਕਰੋ
		bp++;
	    ਜਦੋਂ (ਅੰਕ_ਹੈ(*bp));
	    ਜਾਰੀ;

	ਹਾਲਤ 'V':	/* ISO 8601:1988 week number (parsed, ignored). */
	    bp = conv_ਅੰਕ(bp, &i, 1, 53);
	    ਜਾਰੀ;

	ਹਾਲਤ 'Y':	/* The year. */
	    i = TM_YEAR_ਅਧਾਰ;	/* just for data sanity... */
	    bp = conv_ਅੰਕ(bp, &i, 0, 9999);
	    tm->tm_year = i - TM_YEAR_ਅਧਾਰ;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    ਜਾਰੀ;

	ਹਾਲਤ 'y':	/* The year within 100 years of the epoch. */
	    /* LEGAL_ALT(ALT_E | ALT_O); */
	    bp = conv_ਅੰਕ(bp, &i, 0, 99);

	    ਜੇ (ਵੰਡੋ_year)
		/* preserve century */
		i += (tm->tm_year / 100) * 100;
	    ਨਹੀਂਤਾਂ
	    {
		ਵੰਡੋ_year = 1;
		ਜੇ (i <= 68)
		    i = i + 2000 - TM_YEAR_ਅਧਾਰ;
		ਨਹੀਂਤਾਂ
		    i = i + 1900 - TM_YEAR_ਅਧਾਰ;
	    }
	    tm->tm_year = i;
	    state |= S_YEAR;
	    ਜਾਰੀ;

	ਹਾਲਤ 'Z':
	ਹਾਲਤ 'z':
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
	    ਜੇ (mandatory)
		ਜਦੋਂ (ਖਾਲੀ_ਥਾਂ_ਹੈ(*bp))
		    bp++;

	    zname = bp;
	    ਚੋਣ (*bp++)
	    {
	    ਹਾਲਤ 'G':
		ਜੇ (*bp++ != 'M')
		    ਜਾਓ namedzone;
		/*FALLTHROUGH*/
	    ਹਾਲਤ 'U':
		ਜੇ (*bp++ != 'T')
		    ਜਾਓ namedzone;
		ਨਹੀਂਤਾਂ ਜੇ (!delim(*bp) && *bp++ != 'C')
		    ਜਾਓ namedzone;
		/*FALLTHROUGH*/
	    ਹਾਲਤ 'Z':
		ਜੇ (!delim(*bp))
		    ਜਾਓ namedzone;
		tm->tm_isdst = 0;
		ਜਾਰੀ;
	    ਹਾਲਤ '+':
		neg = 0;
		ਰੋਕੋ;
	    ਹਾਲਤ '-':
		neg = 1;
		ਰੋਕੋ;
	    ਮੂਲ:
namedzone:
		bp = zname;

		/* Nautical / Military style */
		ਜੇ (delim(bp[1]) &&
			((*bp >= 'A' && *bp <= 'I') ||
			 (*bp >= 'L' && *bp <= 'Y')))
		{
		    bp++;
		    ਜਾਰੀ;
		}
		/* 'J' is local time */
		ਜੇ (delim(bp[1]) && *bp == 'J')
		{
		    bp++;
		    ਜਾਰੀ;
		}

		/*
		 * From our 3 letter hard-coded table.
		 */
		ਜੇ (delim(bp[0]) || delim(bp[1]) ||
			delim(bp[2]) || !delim(bp[3]))
		    ਜਾਓ loadzone;
		ep = ਲੱਭੋ_string(bp, &i, nast, ਨਿਰਾ, 4);
		ਜੇ (ep != ਨਿਰਾ)
		{
		    bp = ep;
		    ਜਾਰੀ;
		}
		ep = ਲੱਭੋ_string(bp, &i, nadt, ਨਿਰਾ, 4);
		ਜੇ (ep != ਨਿਰਾ)
		{
		    tm->tm_isdst = 1;
		    bp = ep;
		    ਜਾਰੀ;
		}
		/*
		 * Our current timezone.  Prefer _get_tzname() over the
		 * tzname[] global, which UCRT marks deprecated because it
		 * may be inaccurate after locale changes.
		 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GET_TZNAME
		{
		    ਅੱਖਰ tzbuf[2][32];
		    ਅਚਲ ਅੱਖਰ *tznames[2] = { tzbuf[0], tzbuf[1] };
		    size_t tzlen;

		    _tzset();
		    ਜੇ (_get_tzname(&tzlen, tzbuf[0], ਆਕਾਰ(tzbuf[0]), 0) != 0)
			tzbuf[0][0] = NUL;
		    ਜੇ (_get_tzname(&tzlen, tzbuf[1], ਆਕਾਰ(tzbuf[1]), 1) != 0)
			tzbuf[1][0] = NUL;
		    ep = ਲੱਭੋ_string(bp, &i, tznames, ਨਿਰਾ, 2);
		}
#ਨਹੀਂਤਾਂ
		ep = ਲੱਭੋ_string(bp, &i,
			(ਅਚਲ ਅੱਖਰ *ਅਚਲ *)tzname, ਨਿਰਾ, 2);
#ਜੇ_ਅੰਤ
		ਜੇ (ep != ਨਿਰਾ)
		{
		    tm->tm_isdst = i;
		    bp = ep;
		    ਜਾਰੀ;
		}
loadzone:
		/*
		 * The hard way, load the zone!
		 */
		ਜੇ (fromzone(&bp, tm, mandatory))
		    ਜਾਰੀ;
		ਜਾਓ ਬਾਹਰਲਾ;
	    }
	    offs = 0;
	    ਲਈ (i = 0; i < 4; )
	    {
		ਜੇ (ਅੰਕ_ਹੈ(*bp))
		{
		    offs = offs * 10 + (*bp++ - '0');
		    i++;
		    ਜਾਰੀ;
		}
		ਜੇ (i == 2 && *bp == ':')
		{
		    bp++;
		    ਜਾਰੀ;
		}
		ਰੋਕੋ;
	    }
	    ਜੇ (ਅੰਕ_ਹੈ(*bp))
		ਜਾਓ ਬਾਹਰਲਾ;
	    ਚੋਣ (i)
	    {
	    ਹਾਲਤ 2:
		offs *= SECSPERHOUR;
		ਰੋਕੋ;
	    ਹਾਲਤ 4:
		i = offs % 100;
		offs /= 100;
		ਜੇ (i >= SECSPERMIN)
		    ਜਾਓ ਬਾਹਰਲਾ;
		/* Convert minutes into decimal */
		offs = offs * SECSPERHOUR + i * SECSPERMIN;
		ਰੋਕੋ;
	    ਮੂਲ:
ਬਾਹਰਲਾ:
		ਜੇ (mandatory)
		    ਵਾਪਸ ਨਿਰਾ;
		bp = zname;
		ਜਾਰੀ;
	    }
	    /* ISO 8601 & RFC 3339 limit to 23:59 max */
	    ਜੇ (offs >= (HOURSPERDAY * SECSPERHOUR))
		ਜਾਓ ਬਾਹਰਲਾ;
	    ਜੇ (neg)
		offs = -offs;
	    tm->tm_isdst = 0;	/* XXX */
	    ਜਾਰੀ;

	/*
	 * Miscellaneous conversions.
	 */
	ਹਾਲਤ 'n':	/* Any kind of white-space. */
	ਹਾਲਤ 't':
	    ਜਦੋਂ (ਖਾਲੀ_ਥਾਂ_ਹੈ(*bp))
		bp++;
	    LEGAL_ALT(0);
	    ਜਾਰੀ;


	ਮੂਲ:	/* Unknown/unsupported conversion. */
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }

    ਜੇ (!HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	ਜੇ (HAVE_MON(state) && HAVE_MDAY(state))
	{
	    /* calculate day of year (ordinal date) */
	    tm->tm_yday =  ਸ਼ੁਰੂ_ਦਾ_month[isleap_ਜੋੜ(tm->tm_year,
			    TM_YEAR_ਅਧਾਰ)][tm->tm_mon] + (tm->tm_mday - 1);
	    state |= S_YDAY;
	}
	ਨਹੀਂਤਾਂ ਜੇ (day_ਵਿੱਥ != -1)
	{
	    /*
	     * Set the date to the first Sunday (or Monday)
	     * of the specified week of the year.
	     */
	    ਜੇ (!HAVE_WDAY(state))
	    {
		tm->tm_wday = day_ਵਿੱਥ;
		state |= S_WDAY;
	    }
	    tm->tm_yday = (7 -
		    ਪਹਿਲਾ_wday_ਦਾ(tm->tm_year + TM_YEAR_ਅਧਾਰ) +
		    day_ਵਿੱਥ) % 7 + (week_ਵਿੱਥ - 1) * 7 +
		    tm->tm_wday  - day_ਵਿੱਥ;
	    state |= S_YDAY;
	}
    }

    ਜੇ (HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	ਪੂਰਨ leap;

	ਜੇ (!HAVE_MON(state))
	{
	    /* calculate month of day of year */
	    i = 0;
	    leap = isleap_ਜੋੜ(tm->tm_year, TM_YEAR_ਅਧਾਰ);
	    ਜਦੋਂ (tm->tm_yday >= ਸ਼ੁਰੂ_ਦਾ_month[leap][i])
		i++;
	    ਜੇ (i > 12)
	    {
		i = 1;
		tm->tm_yday -= ਸ਼ੁਰੂ_ਦਾ_month[leap][12];
		tm->tm_year++;
	    }
	    tm->tm_mon = i - 1;
	    state |= S_MON;
	}

	ਜੇ (!HAVE_MDAY(state))
	{
	    /* calculate day of month */
	    leap = isleap_ਜੋੜ(tm->tm_year, TM_YEAR_ਅਧਾਰ);
	    tm->tm_mday = tm->tm_yday -
		ਸ਼ੁਰੂ_ਦਾ_month[leap][tm->tm_mon] + 1;
	    state |= S_MDAY;
	}

	ਜੇ (!HAVE_WDAY(state))
	{
	    /* calculate day of week */
	    i = 0;
	    week_ਵਿੱਥ = ਪਹਿਲਾ_wday_ਦਾ(tm->tm_year);
	    ਜਦੋਂ (i++ <= tm->tm_yday)
	    {
		ਜੇ (week_ਵਿੱਥ++ >= 6)
		    week_ਵਿੱਥ = 0;
	    }
	    tm->tm_wday = week_ਵਿੱਥ;
	    state |= S_WDAY;
	}
    }

    ਵਾਪਸ (ਅੱਖਰ *)bp;
}


    ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *
conv_ਅੰਕ(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, ਪੂਰਨ *dest, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ llim,
						    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ulim)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਨਤੀਜਾ = 0;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ch;

    /* The limit also determines the number of valid digits. */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ rulim = ulim;

    ch = *buf;
    ਜੇ (ch < '0' || ch > '9')
	ਵਾਪਸ ਨਿਰਾ;

    ਕਰੋ
    {
	ਨਤੀਜਾ *= 10;
	ਨਤੀਜਾ += ch - '0';
	rulim /= 10;
	ch = *++buf;
    } ਜਦੋਂ ((ਨਤੀਜਾ * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

    ਜੇ (ਨਤੀਜਾ < llim || ਨਤੀਜਾ > ulim)
	ਵਾਪਸ ਨਿਰਾ;

    *dest = ਨਤੀਜਾ;
    ਵਾਪਸ buf;
}

    ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *
ਲੱਭੋ_string(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *bp, ਪੂਰਨ *tgt, ਅਚਲ ਅੱਖਰ *ਅਚਲ *n1,
					    ਅਚਲ ਅੱਖਰ *ਅਚਲ *n2, ਪੂਰਨ c)
{
    ਪੂਰਨ		i;
    size_t	len;

    /* check full name - then abbreviated ones */
    ਲਈ (; n1 != ਨਿਰਾ; n1 = n2, n2 = ਨਿਰਾ)
    {
	ਲਈ (i = 0; i < c; i++, n1++)
	{
	    len = ਲੜੀ_ਲੰਬਾਈ(*n1);
	    ਜੇ (STRNICMP(*n1, bp, len) == 0)
	    {
		*tgt = i;
		ਵਾਪਸ bp + len;
	    }
	}
    }

    /* Nothing matched */
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਅੰਤ // MSWIN
