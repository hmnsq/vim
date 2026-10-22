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

#ಅನ್ತರ್ಭಾವ "vim.h"

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
#ಯದಿ defined(_UCRT) || defined(_MSC_VER)
# define USE_GET_TZNAME
#endif

/* Locale tables (English / "C" locale). */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ c_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ c_abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ c_mon[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ c_abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ c_am_pm[] = { "AM", "PM" };
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ c_d_t_fmt[] = "%a %b %e %H:%M:%S %Y";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ c_d_fmt[] = "%m/%d/%y";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ c_t_fmt[] = "%H:%M:%S";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ c_t_fmt_ampm[] = "%I:%M:%S %p";

ಸ್ಥಿರ ಧ್ರುವ unsigned ವರ್ಣ *conv_ಅಂಕೆ(ಧ್ರುವ unsigned ವರ್ಣ *, ಪೂರ್ಣಾಙ್ಕ *,
						unsigned ಪೂರ್ಣಾಙ್ಕ, unsigned ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಧ್ರುವ unsigned ವರ್ಣ *find_string(ಧ್ರುವ unsigned ವರ್ಣ *, ಪೂರ್ಣಾಙ್ಕ *,
			ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *, ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *, ಪೂರ್ಣಾಙ್ಕ);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E			0x01
#define ALT_O			0x02
#define LEGAL_ALT(x)		{ ಯದಿ (alt_format & ~(x)) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; }

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
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ nast[5] = {
    "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ nadt[5] = {
    "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};

/*
 * Table to determine the ordinal date for the start of a month.
 * Ref: http://en.wikipedia.org/wiki/ISO_week_date
 */
ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ start_of_month[2][13] = {
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
first_wday_of(ಪೂರ್ಣಾಙ್ಕ yr)
{
    ಪ್ರತ್ಯಾಗಮ ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
	    (isleap(yr) ? 6 : 0) + 1) % 7;
}

#define delim(p)	((p) == '\0' || isspace((unsigned ವರ್ಣ)(p)))

/*
 * Stub replacing NetBSD's fromzone(): the Windows CRT does not provide
 * tzalloc() / tzgetgmtoff() and the associated IANA tzfile loader, so
 * we simply decline to recognize arbitrary timezone names.  The %Z / %z
 * numeric and RFC-822/2822 forms are still handled by the caller.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fromzone(ಧ್ರುವ unsigned ವರ್ಣ **bp UNUSED, ಸಂರಚನಾ tm *tm UNUSED,
						    ಪೂರ್ಣಾಙ್ಕ mandatory UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ವರ್ಣ *
strptime(ಧ್ರುವ ವರ್ಣ *buf, ಧ್ರುವ ವರ್ಣ *fmt, ಸಂರಚನಾ tm *tm)
{
    unsigned ವರ್ಣ c;
    ಧ್ರುವ unsigned ವರ್ಣ *bp, *ep, *zname;
    ಪೂರ್ಣಾಙ್ಕ alt_format, i, split_year = 0, neg = 0, state = 0,
	day_offset = -1, week_offset = 0, offs, mandatory;
    ಧ್ರುವ ವರ್ಣ *new_fmt;

    bp = (ಧ್ರುವ unsigned ವರ್ಣ *)buf;

    ಯಾವತ್ (bp != ರಿಕ್ತ && (c = *fmt++) != '\0')
    {
	/* Clear `alternate' modifier prior to new conversion. */
	alt_format = 0;
	i = 0;

	/* Eat up white-space. */
	ಯದಿ (isspace(c))
	{
	    ಯಾವತ್ (isspace(*bp))
		bp++;
	    ಅನುವರ್ತ;
	}

	ಯದಿ (c != '%')
	    ಗಚ್ಛ literal;


again:	ವಿಕಲ್ಪ (c = *fmt++)
	{
	ಪಕ್ಷ '%':	/* "%%" is converted to "%". */
literal:
	    ಯದಿ (c != *bp++)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    LEGAL_ALT(0);
	    ಅನುವರ್ತ;

	/*
	 * "Alternative" modifiers. Just set the appropriate flag
	 * and start over again.
	 */
	ಪಕ್ಷ 'E':	/* "%E?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_E;
	    ಗಚ್ಛ again;

	ಪಕ್ಷ 'O':	/* "%O?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_O;
	    ಗಚ್ಛ again;

	/*
	 * "Complex" conversion rules, implemented through recursion.
	 */
	ಪಕ್ಷ 'c':	/* Date and time, using the locale's format. */
	    new_fmt = c_d_t_fmt;
	    state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
	    ಗಚ್ಛ recurse;

	ಪಕ್ಷ 'D':	/* The date as "%m/%d/%y". */
	    new_fmt = "%m/%d/%y";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    ಗಚ್ಛ recurse;

	ಪಕ್ಷ 'F':	/* The date as "%Y-%m-%d". */
	    new_fmt = "%Y-%m-%d";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    ಗಚ್ಛ recurse;

	ಪಕ್ಷ 'R':	/* The time as "%H:%M". */
	    new_fmt = "%H:%M";
	    LEGAL_ALT(0);
	    ಗಚ್ಛ recurse;

	ಪಕ್ಷ 'r':	/* The time in 12-hour clock representation. */
	    new_fmt = c_t_fmt_ampm;
	    LEGAL_ALT(0);
	    ಗಚ್ಛ recurse;

	ಪಕ್ಷ 'T':	/* The time as "%H:%M:%S". */
	    new_fmt = "%H:%M:%S";
	    LEGAL_ALT(0);
	    ಗಚ್ಛ recurse;

	ಪಕ್ಷ 'X':	/* The time, using the locale's format. */
	    new_fmt = c_t_fmt;
	    ಗಚ್ಛ recurse;

	ಪಕ್ಷ 'x':	/* The date, using the locale's format. */
	    new_fmt = c_d_fmt;
	    state |= S_MON | S_MDAY | S_YEAR;
	recurse:
	    bp = (ಧ್ರುವ unsigned ವರ್ಣ *)strptime((ಧ್ರುವ ವರ್ಣ *)bp,
							new_fmt, tm);
	    LEGAL_ALT(ALT_E);
	    ಅನುವರ್ತ;

	/*
	 * "Elementary" conversion rules.
	 */
	ಪಕ್ಷ 'A':	/* The day of week, using the locale's form. */
	ಪಕ್ಷ 'a':
	    bp = find_string(bp, &tm->tm_wday, c_day, c_abday, 7);
	    LEGAL_ALT(0);
	    state |= S_WDAY;
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'B':	/* The month, using the locale's form. */
	ಪಕ್ಷ 'b':
	ಪಕ್ಷ 'h':
	    bp = find_string(bp, &tm->tm_mon, c_mon, c_abmon, 12);
	    LEGAL_ALT(0);
	    state |= S_MON;
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'C':	/* The century number. */
	    i = 20;
	    bp = conv_ಅಂಕೆ(bp, &i, 0, 99);

	    i = i * 100 - TM_YEAR_BASE;
	    ಯದಿ (split_year)
		i += tm->tm_year % 100;
	    split_year = 1;
	    tm->tm_year = i;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'd':	/* The day of month. */
	ಪಕ್ಷ 'e':
	    bp = conv_ಅಂಕೆ(bp, &tm->tm_mday, 1, 31);
	    LEGAL_ALT(ALT_O);
	    state |= S_MDAY;
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'k':	/* The hour (24-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	ಪಕ್ಷ 'H':
	    bp = conv_ಅಂಕೆ(bp, &tm->tm_hour, 0, 23);
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'l':	/* The hour (12-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	ಪಕ್ಷ 'I':
	    bp = conv_ಅಂಕೆ(bp, &tm->tm_hour, 1, 12);
	    ಯದಿ (tm->tm_hour == 12)
		tm->tm_hour = 0;
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'j':	/* The day of year. */
	    i = 1;
	    bp = conv_ಅಂಕೆ(bp, &i, 1, 366);
	    tm->tm_yday = i - 1;
	    LEGAL_ALT(0);
	    state |= S_YDAY;
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'M':	/* The minute. */
	    bp = conv_ಅಂಕೆ(bp, &tm->tm_min, 0, 59);
	    LEGAL_ALT(ALT_O);
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'm':	/* The month. */
	    i = 1;
	    bp = conv_ಅಂಕೆ(bp, &i, 1, 12);
	    tm->tm_mon = i - 1;
	    LEGAL_ALT(ALT_O);
	    state |= S_MON;
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'p':	/* The locale's equivalent of AM/PM. */
	    bp = find_string(bp, &i, c_am_pm, ರಿಕ್ತ, 2);
	    ಯದಿ (HAVE_HOUR(state) && tm->tm_hour > 11)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    tm->tm_hour += i * 12;
	    LEGAL_ALT(0);
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'S':	/* The seconds. */
	    bp = conv_ಅಂಕೆ(bp, &tm->tm_sec, 0, 61);
	    LEGAL_ALT(ALT_O);
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 's':	/* seconds since the epoch */
	    {
		ಧ್ರುವ time_t TIME_MAX = (time_t)((ಪರಿಮಾಣ(time_t) == 8)
					? LLONG_MAX : INT_MAX);
		time_t sse, d;

		ಯದಿ (*bp < '0' || *bp > '9')
		{
		    bp = ರಿಕ್ತ;
		    ಅನುವರ್ತ;
		}

		sse = *bp++ - '0';
		ಯಾವತ್ (*bp >= '0' && *bp <= '9')
		{
		    d = *bp++ - '0';
		    ಯದಿ (sse > TIME_MAX / 10)
		    {
			bp = ರಿಕ್ತ;
			ಭಙ್ಗ;
		    }
		    sse *= 10;
		    ಯದಿ (sse > TIME_MAX - d)
		    {
			bp = ರಿಕ್ತ;
			ಭಙ್ಗ;
		    }
		    sse += d;
		}
		ಯದಿ (bp == ರಿಕ್ತ)
		    ಅನುವರ್ತ;

		{
		    ಸಂರಚನಾ tm *lt = localtime(&sse);

		    ಯದಿ (lt == ರಿಕ್ತ)
			bp = ರಿಕ್ತ;
		    ಅನ್ಯಥಾ
		    {
			*tm = *lt;
			state |= S_YDAY | S_WDAY |
				S_MON | S_MDAY | S_YEAR;
		    }
		}
		ಅನುವರ್ತ;
	    }

	ಪಕ್ಷ 'U':	/* The week of year, beginning on sunday. */
	ಪಕ್ಷ 'W':	/* The week of year, beginning on monday. */
	    /*
	     * This is bogus, as we can not assume any valid
	     * information present in the tm structure at this
	     * point to calculate a real value, so save the
	     * week for now in case it can be used later.
	     */
	    bp = conv_ಅಂಕೆ(bp, &i, 0, 53);
	    LEGAL_ALT(ALT_O);
	    ಯದಿ (c == 'U')
		day_offset = TM_SUNDAY;
	    ಅನ್ಯಥಾ
		day_offset = TM_MONDAY;
	    week_offset = i;
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'w':	/* The day of week, beginning on sunday. */
	    bp = conv_ಅಂಕೆ(bp, &tm->tm_wday, 0, 6);
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'u':	/* The day of week, monday = 1. */
	    bp = conv_ಅಂಕೆ(bp, &i, 1, 7);
	    tm->tm_wday = i % 7;
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'g':	/* ISO week year without century (parsed, ignored). */
	    bp = conv_ಅಂಕೆ(bp, &i, 0, 99);
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'G':	/* ISO week year with century (parsed, ignored). */
	    ಕುರು
		bp++;
	    ಯಾವತ್ (isdigit(*bp));
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'V':	/* ISO 8601:1988 week number (parsed, ignored). */
	    bp = conv_ಅಂಕೆ(bp, &i, 1, 53);
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'Y':	/* The year. */
	    i = TM_YEAR_BASE;	/* just for data sanity... */
	    bp = conv_ಅಂಕೆ(bp, &i, 0, 9999);
	    tm->tm_year = i - TM_YEAR_BASE;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'y':	/* The year within 100 years of the epoch. */
	    /* LEGAL_ALT(ALT_E | ALT_O); */
	    bp = conv_ಅಂಕೆ(bp, &i, 0, 99);

	    ಯದಿ (split_year)
		/* preserve century */
		i += (tm->tm_year / 100) * 100;
	    ಅನ್ಯಥಾ
	    {
		split_year = 1;
		ಯದಿ (i <= 68)
		    i = i + 2000 - TM_YEAR_BASE;
		ಅನ್ಯಥಾ
		    i = i + 1900 - TM_YEAR_BASE;
	    }
	    tm->tm_year = i;
	    state |= S_YEAR;
	    ಅನುವರ್ತ;

	ಪಕ್ಷ 'Z':
	ಪಕ್ಷ 'z':
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
	    ಯದಿ (mandatory)
		ಯಾವತ್ (isspace(*bp))
		    bp++;

	    zname = bp;
	    ವಿಕಲ್ಪ (*bp++)
	    {
	    ಪಕ್ಷ 'G':
		ಯದಿ (*bp++ != 'M')
		    ಗಚ್ಛ namedzone;
		/*FALLTHROUGH*/
	    ಪಕ್ಷ 'U':
		ಯದಿ (*bp++ != 'T')
		    ಗಚ್ಛ namedzone;
		ಅನ್ಯಥಾ ಯದಿ (!delim(*bp) && *bp++ != 'C')
		    ಗಚ್ಛ namedzone;
		/*FALLTHROUGH*/
	    ಪಕ್ಷ 'Z':
		ಯದಿ (!delim(*bp))
		    ಗಚ್ಛ namedzone;
		tm->tm_isdst = 0;
		ಅನುವರ್ತ;
	    ಪಕ್ಷ '+':
		neg = 0;
		ಭಙ್ಗ;
	    ಪಕ್ಷ '-':
		neg = 1;
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
namedzone:
		bp = zname;

		/* Nautical / Military style */
		ಯದಿ (delim(bp[1]) &&
			((*bp >= 'A' && *bp <= 'I') ||
			 (*bp >= 'L' && *bp <= 'Y')))
		{
		    bp++;
		    ಅನುವರ್ತ;
		}
		/* 'J' is local time */
		ಯದಿ (delim(bp[1]) && *bp == 'J')
		{
		    bp++;
		    ಅನುವರ್ತ;
		}

		/*
		 * From our 3 letter hard-coded table.
		 */
		ಯದಿ (delim(bp[0]) || delim(bp[1]) ||
			delim(bp[2]) || !delim(bp[3]))
		    ಗಚ್ಛ loadzone;
		ep = find_string(bp, &i, nast, ರಿಕ್ತ, 4);
		ಯದಿ (ep != ರಿಕ್ತ)
		{
		    bp = ep;
		    ಅನುವರ್ತ;
		}
		ep = find_string(bp, &i, nadt, ರಿಕ್ತ, 4);
		ಯದಿ (ep != ರಿಕ್ತ)
		{
		    tm->tm_isdst = 1;
		    bp = ep;
		    ಅನುವರ್ತ;
		}
		/*
		 * Our current timezone.  Prefer _get_tzname() over the
		 * tzname[] global, which UCRT marks deprecated because it
		 * may be inaccurate after locale changes.
		 */
#ifdef USE_GET_TZNAME
		{
		    ವರ್ಣ tzbuf[2][32];
		    ಧ್ರುವ ವರ್ಣ *tznames[2] = { tzbuf[0], tzbuf[1] };
		    size_t tzlen;

		    _tzset();
		    ಯದಿ (_get_tzname(&tzlen, tzbuf[0], ಪರಿಮಾಣ(tzbuf[0]), 0) != 0)
			tzbuf[0][0] = NUL;
		    ಯದಿ (_get_tzname(&tzlen, tzbuf[1], ಪರಿಮಾಣ(tzbuf[1]), 1) != 0)
			tzbuf[1][0] = NUL;
		    ep = find_string(bp, &i, tznames, ರಿಕ್ತ, 2);
		}
#ಅನ್ಯಥಾ
		ep = find_string(bp, &i,
			(ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ *)tzname, ರಿಕ್ತ, 2);
#endif
		ಯದಿ (ep != ರಿಕ್ತ)
		{
		    tm->tm_isdst = i;
		    bp = ep;
		    ಅನುವರ್ತ;
		}
loadzone:
		/*
		 * The hard way, load the zone!
		 */
		ಯದಿ (fromzone(&bp, tm, mandatory))
		    ಅನುವರ್ತ;
		ಗಚ್ಛ out;
	    }
	    offs = 0;
	    ಪ್ರತಿ (i = 0; i < 4; )
	    {
		ಯದಿ (isdigit(*bp))
		{
		    offs = offs * 10 + (*bp++ - '0');
		    i++;
		    ಅನುವರ್ತ;
		}
		ಯದಿ (i == 2 && *bp == ':')
		{
		    bp++;
		    ಅನುವರ್ತ;
		}
		ಭಙ್ಗ;
	    }
	    ಯದಿ (isdigit(*bp))
		ಗಚ್ಛ out;
	    ವಿಕಲ್ಪ (i)
	    {
	    ಪಕ್ಷ 2:
		offs *= SECSPERHOUR;
		ಭಙ್ಗ;
	    ಪಕ್ಷ 4:
		i = offs % 100;
		offs /= 100;
		ಯದಿ (i >= SECSPERMIN)
		    ಗಚ್ಛ out;
		/* Convert minutes into decimal */
		offs = offs * SECSPERHOUR + i * SECSPERMIN;
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
out:
		ಯದಿ (mandatory)
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		bp = zname;
		ಅನುವರ್ತ;
	    }
	    /* ISO 8601 & RFC 3339 limit to 23:59 max */
	    ಯದಿ (offs >= (HOURSPERDAY * SECSPERHOUR))
		ಗಚ್ಛ out;
	    ಯದಿ (neg)
		offs = -offs;
	    tm->tm_isdst = 0;	/* XXX */
	    ಅನುವರ್ತ;

	/*
	 * Miscellaneous conversions.
	 */
	ಪಕ್ಷ 'n':	/* Any kind of white-space. */
	ಪಕ್ಷ 't':
	    ಯಾವತ್ (isspace(*bp))
		bp++;
	    LEGAL_ALT(0);
	    ಅನುವರ್ತ;


	ಸ್ವಭಾವ:	/* Unknown/unsupported conversion. */
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }

    ಯದಿ (!HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	ಯದಿ (HAVE_MON(state) && HAVE_MDAY(state))
	{
	    /* calculate day of year (ordinal date) */
	    tm->tm_yday =  start_of_month[isleap_sum(tm->tm_year,
			    TM_YEAR_BASE)][tm->tm_mon] + (tm->tm_mday - 1);
	    state |= S_YDAY;
	}
	ಅನ್ಯಥಾ ಯದಿ (day_offset != -1)
	{
	    /*
	     * Set the date to the first Sunday (or Monday)
	     * of the specified week of the year.
	     */
	    ಯದಿ (!HAVE_WDAY(state))
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

    ಯದಿ (HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	ಪೂರ್ಣಾಙ್ಕ leap;

	ಯದಿ (!HAVE_MON(state))
	{
	    /* calculate month of day of year */
	    i = 0;
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    ಯಾವತ್ (tm->tm_yday >= start_of_month[leap][i])
		i++;
	    ಯದಿ (i > 12)
	    {
		i = 1;
		tm->tm_yday -= start_of_month[leap][12];
		tm->tm_year++;
	    }
	    tm->tm_mon = i - 1;
	    state |= S_MON;
	}

	ಯದಿ (!HAVE_MDAY(state))
	{
	    /* calculate day of month */
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    tm->tm_mday = tm->tm_yday -
		start_of_month[leap][tm->tm_mon] + 1;
	    state |= S_MDAY;
	}

	ಯದಿ (!HAVE_WDAY(state))
	{
	    /* calculate day of week */
	    i = 0;
	    week_offset = first_wday_of(tm->tm_year);
	    ಯಾವತ್ (i++ <= tm->tm_yday)
	    {
		ಯದಿ (week_offset++ >= 6)
		    week_offset = 0;
	    }
	    tm->tm_wday = week_offset;
	    state |= S_WDAY;
	}
    }

    ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ *)bp;
}


    ಸ್ಥಿರ ಧ್ರುವ unsigned ವರ್ಣ *
conv_ಅಂಕೆ(ಧ್ರುವ unsigned ವರ್ಣ *buf, ಪೂರ್ಣಾಙ್ಕ *dest, unsigned ಪೂರ್ಣಾಙ್ಕ llim,
						    unsigned ಪೂರ್ಣಾಙ್ಕ ulim)
{
    unsigned ಪೂರ್ಣಾಙ್ಕ result = 0;
    unsigned ವರ್ಣ ch;

    /* The limit also determines the number of valid digits. */
    unsigned ಪೂರ್ಣಾಙ್ಕ rulim = ulim;

    ch = *buf;
    ಯದಿ (ch < '0' || ch > '9')
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಕುರು
    {
	result *= 10;
	result += ch - '0';
	rulim /= 10;
	ch = *++buf;
    } ಯಾವತ್ ((result * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

    ಯದಿ (result < llim || result > ulim)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    *dest = result;
    ಪ್ರತ್ಯಾಗಮ buf;
}

    ಸ್ಥಿರ ಧ್ರುವ unsigned ವರ್ಣ *
find_string(ಧ್ರುವ unsigned ವರ್ಣ *bp, ಪೂರ್ಣಾಙ್ಕ *tgt, ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ *n1,
					    ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ *n2, ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    size_t	len;

    /* check full name - then abbreviated ones */
    ಪ್ರತಿ (; n1 != ರಿಕ್ತ; n1 = n2, n2 = ರಿಕ್ತ)
    {
	ಪ್ರತಿ (i = 0; i < c; i++, n1++)
	{
	    len = strlen(*n1);
	    ಯದಿ (STRNICMP(*n1, bp, len) == 0)
	    {
		*tgt = i;
		ಪ್ರತ್ಯಾಗಮ bp + len;
	    }
	}
    }

    /* Nothing matched */
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#endif // MSWIN
