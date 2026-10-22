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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN

/* Constants lifted from NetBSD's <tzfile.h>. */
#ವ್ಯಾಖ್ಯಾನಿಸು TM_YEAR_ತಳ	1900
#ವ್ಯಾಖ್ಯಾನಿಸು TM_SUNDAY	0
#ವ್ಯಾಖ್ಯಾನಿಸು TM_MONDAY	1
#ವ್ಯಾಖ್ಯಾನಿಸು SECSPERHOUR	3600
#ವ್ಯಾಖ್ಯಾನಿಸು SECSPERMIN	60
#ವ್ಯಾಖ್ಯಾನಿಸು HOURSPERDAY	24

#ವ್ಯಾಖ್ಯಾನಿಸು isleap(y)	(((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)
/* isleap_sum avoids integer overflow when adding the 1900 base. */
#ವ್ಯಾಖ್ಯಾನಿಸು isleap_ಮೊತ್ತ(a, b) \
			isleap(((ಚಿಹ್ನೆರಹಿತ)(a) + (ಚಿಹ್ನೆರಹಿತ)(b)) % 400)

/*
 * _get_tzname() is provided by the UCRT and by the MSVC CRT.  Older MinGW
 * builds linking against msvcrt.dll fall back to the deprecated tzname[]
 * global (mingw-w64 already exposes the POSIX-named tzname[]).
 */
#ಆದರೆ defined(_UCRT) || defined(_MSC_VER)
# define USE_GET_TZNAME
#ಆದರೆ_ಅಂತ್ಯ

/* Locale tables (English / "C" locale). */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ c_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ c_abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ c_mon[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ c_abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ c_am_pm[] = { "AM", "PM" };
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c_d_t_fmt[] = "%a %b %e %H:%M:%S %Y";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c_d_fmt[] = "%m/%d/%y";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c_t_fmt[] = "%H:%M:%S";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c_t_fmt_ampm[] = "%I:%M:%S %p";

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *conv_ಅಂಕೆ(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *, ಪೂರ್ಣಾಂಕ *,
						ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕಂಡುಹಿಡಿ_string(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *, ಪೂರ್ಣಾಂಕ *,
			ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *, ಪೂರ್ಣಾಂಕ);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ALT_E			0x01
#ವ್ಯಾಖ್ಯಾನಿಸು ALT_O			0x02
#ವ್ಯಾಖ್ಯಾನಿಸು LEGAL_ALT(x)		{ ಆದರೆ (alt_format & ~(x)) ಹಿಂತಿರುಗಿ ಖಾಲಿ; }

#ವ್ಯಾಖ್ಯಾನಿಸು S_YEAR			(1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು S_MON			(1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು S_YDAY			(1 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು S_MDAY			(1 << 3)
#ವ್ಯಾಖ್ಯಾನಿಸು S_WDAY			(1 << 4)
#ವ್ಯಾಖ್ಯಾನಿಸು S_HOUR			(1 << 5)

#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_MDAY(s)		(s & S_MDAY)
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_MON(s)		(s & S_MON)
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_WDAY(s)		(s & S_WDAY)
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_YDAY(s)		(s & S_YDAY)
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_YEAR(s)		(s & S_YEAR)
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_HOUR(s)		(s & S_HOUR)

/* RFC-822/RFC-2822 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ nast[5] = {
    "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ nadt[5] = {
    "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};

/*
 * Table to determine the ordinal date for the start of a month.
 * Ref: http://en.wikipedia.org/wiki/ISO_week_date
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಆರಂಭ_ದ_month[2][13] = {
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಮೊದಲ_wday_ದ(ಪೂರ್ಣಾಂಕ yr)
{
    ಹಿಂತಿರುಗಿ ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
	    (isleap(yr) ? 6 : 0) + 1) % 7;
}

#ವ್ಯಾಖ್ಯಾನಿಸು delim(p)	((p) == '\0' || ಖಾಲಿಯೇ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(p)))

/*
 * Stub replacing NetBSD's fromzone(): the Windows CRT does not provide
 * tzalloc() / tzgetgmtoff() and the associated IANA tzfile loader, so
 * we simply decline to recognize arbitrary timezone names.  The %Z / %z
 * numeric and RFC-822/2822 forms are still handled by the caller.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fromzone(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **bp ಬಳಸದ, ರಚನೆ tm *tm ಬಳಸದ,
						    ಪೂರ್ಣಾಂಕ mandatory ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ 0;
}

    ಅಕ್ಷರ *
strptime(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ರಚನೆ tm *tm)
{
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *bp, *ep, *zname;
    ಪೂರ್ಣಾಂಕ alt_format, i, ವಿಭಜಿಸು_year = 0, neg = 0, state = 0,
	day_ಸ್ಥಾನಾಂತರ = -1, week_ಸ್ಥಾನಾಂತರ = 0, offs, mandatory;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *new_fmt;

    bp = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)buf;

    ಆಗ (bp != ಖಾಲಿ && (c = *fmt++) != '\0')
    {
	/* Clear `alternate' modifier prior to new conversion. */
	alt_format = 0;
	i = 0;

	/* Eat up white-space. */
	ಆದರೆ (ಖಾಲಿಯೇ(c))
	{
	    ಆಗ (ಖಾಲಿಯೇ(*bp))
		bp++;
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ (c != '%')
	    ಹೋಗು literal;


again:	ಆಯ್ಕೆ (c = *fmt++)
	{
	ಸ್ಥಿತಿ '%':	/* "%%" is converted to "%". */
literal:
	    ಆದರೆ (c != *bp++)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    LEGAL_ALT(0);
	    ಮುಂದುವರಿ;

	/*
	 * "Alternative" modifiers. Just set the appropriate flag
	 * and start over again.
	 */
	ಸ್ಥಿತಿ 'E':	/* "%E?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_E;
	    ಹೋಗು again;

	ಸ್ಥಿತಿ 'O':	/* "%O?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_O;
	    ಹೋಗು again;

	/*
	 * "Complex" conversion rules, implemented through recursion.
	 */
	ಸ್ಥಿತಿ 'c':	/* Date and time, using the locale's format. */
	    new_fmt = c_d_t_fmt;
	    state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
	    ಹೋಗು recurse;

	ಸ್ಥಿತಿ 'D':	/* The date as "%m/%d/%y". */
	    new_fmt = "%m/%d/%y";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    ಹೋಗು recurse;

	ಸ್ಥಿತಿ 'F':	/* The date as "%Y-%m-%d". */
	    new_fmt = "%Y-%m-%d";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    ಹೋಗು recurse;

	ಸ್ಥಿತಿ 'R':	/* The time as "%H:%M". */
	    new_fmt = "%H:%M";
	    LEGAL_ALT(0);
	    ಹೋಗು recurse;

	ಸ್ಥಿತಿ 'r':	/* The time in 12-hour clock representation. */
	    new_fmt = c_t_fmt_ampm;
	    LEGAL_ALT(0);
	    ಹೋಗು recurse;

	ಸ್ಥಿತಿ 'T':	/* The time as "%H:%M:%S". */
	    new_fmt = "%H:%M:%S";
	    LEGAL_ALT(0);
	    ಹೋಗು recurse;

	ಸ್ಥಿತಿ 'X':	/* The time, using the locale's format. */
	    new_fmt = c_t_fmt;
	    ಹೋಗು recurse;

	ಸ್ಥಿತಿ 'x':	/* The date, using the locale's format. */
	    new_fmt = c_d_fmt;
	    state |= S_MON | S_MDAY | S_YEAR;
	recurse:
	    bp = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)strptime((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)bp,
							new_fmt, tm);
	    LEGAL_ALT(ALT_E);
	    ಮುಂದುವರಿ;

	/*
	 * "Elementary" conversion rules.
	 */
	ಸ್ಥಿತಿ 'A':	/* The day of week, using the locale's form. */
	ಸ್ಥಿತಿ 'a':
	    bp = ಕಂಡುಹಿಡಿ_string(bp, &tm->tm_wday, c_day, c_abday, 7);
	    LEGAL_ALT(0);
	    state |= S_WDAY;
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'B':	/* The month, using the locale's form. */
	ಸ್ಥಿತಿ 'b':
	ಸ್ಥಿತಿ 'h':
	    bp = ಕಂಡುಹಿಡಿ_string(bp, &tm->tm_mon, c_mon, c_abmon, 12);
	    LEGAL_ALT(0);
	    state |= S_MON;
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'C':	/* The century number. */
	    i = 20;
	    bp = conv_ಅಂಕೆ(bp, &i, 0, 99);

	    i = i * 100 - TM_YEAR_ತಳ;
	    ಆದರೆ (ವಿಭಜಿಸು_year)
		i += tm->tm_year % 100;
	    ವಿಭಜಿಸು_year = 1;
	    tm->tm_year = i;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'd':	/* The day of month. */
	ಸ್ಥಿತಿ 'e':
	    bp = conv_ಅಂಕೆ(bp, &tm->tm_mday, 1, 31);
	    LEGAL_ALT(ALT_O);
	    state |= S_MDAY;
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'k':	/* The hour (24-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	ಸ್ಥಿತಿ 'H':
	    bp = conv_ಅಂಕೆ(bp, &tm->tm_hour, 0, 23);
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'l':	/* The hour (12-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	ಸ್ಥಿತಿ 'I':
	    bp = conv_ಅಂಕೆ(bp, &tm->tm_hour, 1, 12);
	    ಆದರೆ (tm->tm_hour == 12)
		tm->tm_hour = 0;
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'j':	/* The day of year. */
	    i = 1;
	    bp = conv_ಅಂಕೆ(bp, &i, 1, 366);
	    tm->tm_yday = i - 1;
	    LEGAL_ALT(0);
	    state |= S_YDAY;
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'M':	/* The minute. */
	    bp = conv_ಅಂಕೆ(bp, &tm->tm_min, 0, 59);
	    LEGAL_ALT(ALT_O);
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'm':	/* The month. */
	    i = 1;
	    bp = conv_ಅಂಕೆ(bp, &i, 1, 12);
	    tm->tm_mon = i - 1;
	    LEGAL_ALT(ALT_O);
	    state |= S_MON;
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'p':	/* The locale's equivalent of AM/PM. */
	    bp = ಕಂಡುಹಿಡಿ_string(bp, &i, c_am_pm, ಖಾಲಿ, 2);
	    ಆದರೆ (HAVE_HOUR(state) && tm->tm_hour > 11)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    tm->tm_hour += i * 12;
	    LEGAL_ALT(0);
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'S':	/* The seconds. */
	    bp = conv_ಅಂಕೆ(bp, &tm->tm_sec, 0, 61);
	    LEGAL_ALT(ALT_O);
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 's':	/* seconds since the epoch */
	    {
		ಸ್ಥಿರಾಂಕ time_t ಸಮಯ_MAX = (time_t)((ಗಾತ್ರ(time_t) == 8)
					? LLONG_MAX : ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
		time_t sse, d;

		ಆದರೆ (*bp < '0' || *bp > '9')
		{
		    bp = ಖಾಲಿ;
		    ಮುಂದುವರಿ;
		}

		sse = *bp++ - '0';
		ಆಗ (*bp >= '0' && *bp <= '9')
		{
		    d = *bp++ - '0';
		    ಆದರೆ (sse > ಸಮಯ_MAX / 10)
		    {
			bp = ಖಾಲಿ;
			ನಿಲ್ಲಿಸು;
		    }
		    sse *= 10;
		    ಆದರೆ (sse > ಸಮಯ_MAX - d)
		    {
			bp = ಖಾಲಿ;
			ನಿಲ್ಲಿಸು;
		    }
		    sse += d;
		}
		ಆದರೆ (bp == ಖಾಲಿ)
		    ಮುಂದುವರಿ;

		{
		    ರಚನೆ tm *lt = localtime(&sse);

		    ಆದರೆ (lt == ಖಾಲಿ)
			bp = ಖಾಲಿ;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			*tm = *lt;
			state |= S_YDAY | S_WDAY |
				S_MON | S_MDAY | S_YEAR;
		    }
		}
		ಮುಂದುವರಿ;
	    }

	ಸ್ಥಿತಿ 'U':	/* The week of year, beginning on sunday. */
	ಸ್ಥಿತಿ 'W':	/* The week of year, beginning on monday. */
	    /*
	     * This is bogus, as we can not assume any valid
	     * information present in the tm structure at this
	     * point to calculate a real value, so save the
	     * week for now in case it can be used later.
	     */
	    bp = conv_ಅಂಕೆ(bp, &i, 0, 53);
	    LEGAL_ALT(ALT_O);
	    ಆದರೆ (c == 'U')
		day_ಸ್ಥಾನಾಂತರ = TM_SUNDAY;
	    ಇಲ್ಲದಿದ್ದರೆ
		day_ಸ್ಥಾನಾಂತರ = TM_MONDAY;
	    week_ಸ್ಥಾನಾಂತರ = i;
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'w':	/* The day of week, beginning on sunday. */
	    bp = conv_ಅಂಕೆ(bp, &tm->tm_wday, 0, 6);
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'u':	/* The day of week, monday = 1. */
	    bp = conv_ಅಂಕೆ(bp, &i, 1, 7);
	    tm->tm_wday = i % 7;
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'g':	/* ISO week year without century (parsed, ignored). */
	    bp = conv_ಅಂಕೆ(bp, &i, 0, 99);
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'G':	/* ISO week year with century (parsed, ignored). */
	    ಮಾಡು
		bp++;
	    ಆಗ (ಅಂಕಿಯೇ(*bp));
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'V':	/* ISO 8601:1988 week number (parsed, ignored). */
	    bp = conv_ಅಂಕೆ(bp, &i, 1, 53);
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'Y':	/* The year. */
	    i = TM_YEAR_ತಳ;	/* just for data sanity... */
	    bp = conv_ಅಂಕೆ(bp, &i, 0, 9999);
	    tm->tm_year = i - TM_YEAR_ತಳ;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'y':	/* The year within 100 years of the epoch. */
	    /* LEGAL_ALT(ALT_E | ALT_O); */
	    bp = conv_ಅಂಕೆ(bp, &i, 0, 99);

	    ಆದರೆ (ವಿಭಜಿಸು_year)
		/* preserve century */
		i += (tm->tm_year / 100) * 100;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ವಿಭಜಿಸು_year = 1;
		ಆದರೆ (i <= 68)
		    i = i + 2000 - TM_YEAR_ತಳ;
		ಇಲ್ಲದಿದ್ದರೆ
		    i = i + 1900 - TM_YEAR_ತಳ;
	    }
	    tm->tm_year = i;
	    state |= S_YEAR;
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ 'Z':
	ಸ್ಥಿತಿ 'z':
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
	    ಆದರೆ (mandatory)
		ಆಗ (ಖಾಲಿಯೇ(*bp))
		    bp++;

	    zname = bp;
	    ಆಯ್ಕೆ (*bp++)
	    {
	    ಸ್ಥಿತಿ 'G':
		ಆದರೆ (*bp++ != 'M')
		    ಹೋಗು namedzone;
		/*FALLTHROUGH*/
	    ಸ್ಥಿತಿ 'U':
		ಆದರೆ (*bp++ != 'T')
		    ಹೋಗು namedzone;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!delim(*bp) && *bp++ != 'C')
		    ಹೋಗು namedzone;
		/*FALLTHROUGH*/
	    ಸ್ಥಿತಿ 'Z':
		ಆದರೆ (!delim(*bp))
		    ಹೋಗು namedzone;
		tm->tm_isdst = 0;
		ಮುಂದುವರಿ;
	    ಸ್ಥಿತಿ '+':
		neg = 0;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '-':
		neg = 1;
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
namedzone:
		bp = zname;

		/* Nautical / Military style */
		ಆದರೆ (delim(bp[1]) &&
			((*bp >= 'A' && *bp <= 'I') ||
			 (*bp >= 'L' && *bp <= 'Y')))
		{
		    bp++;
		    ಮುಂದುವರಿ;
		}
		/* 'J' is local time */
		ಆದರೆ (delim(bp[1]) && *bp == 'J')
		{
		    bp++;
		    ಮುಂದುವರಿ;
		}

		/*
		 * From our 3 letter hard-coded table.
		 */
		ಆದರೆ (delim(bp[0]) || delim(bp[1]) ||
			delim(bp[2]) || !delim(bp[3]))
		    ಹೋಗು loadzone;
		ep = ಕಂಡುಹಿಡಿ_string(bp, &i, nast, ಖಾಲಿ, 4);
		ಆದರೆ (ep != ಖಾಲಿ)
		{
		    bp = ep;
		    ಮುಂದುವರಿ;
		}
		ep = ಕಂಡುಹಿಡಿ_string(bp, &i, nadt, ಖಾಲಿ, 4);
		ಆದರೆ (ep != ಖಾಲಿ)
		{
		    tm->tm_isdst = 1;
		    bp = ep;
		    ಮುಂದುವರಿ;
		}
		/*
		 * Our current timezone.  Prefer _get_tzname() over the
		 * tzname[] global, which UCRT marks deprecated because it
		 * may be inaccurate after locale changes.
		 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GET_TZNAME
		{
		    ಅಕ್ಷರ tzbuf[2][32];
		    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tznames[2] = { tzbuf[0], tzbuf[1] };
		    size_t tzlen;

		    _tzset();
		    ಆದರೆ (_get_tzname(&tzlen, tzbuf[0], ಗಾತ್ರ(tzbuf[0]), 0) != 0)
			tzbuf[0][0] = NUL;
		    ಆದರೆ (_get_tzname(&tzlen, tzbuf[1], ಗಾತ್ರ(tzbuf[1]), 1) != 0)
			tzbuf[1][0] = NUL;
		    ep = ಕಂಡುಹಿಡಿ_string(bp, &i, tznames, ಖಾಲಿ, 2);
		}
#ಇಲ್ಲದಿದ್ದರೆ
		ep = ಕಂಡುಹಿಡಿ_string(bp, &i,
			(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ *)tzname, ಖಾಲಿ, 2);
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (ep != ಖಾಲಿ)
		{
		    tm->tm_isdst = i;
		    bp = ep;
		    ಮುಂದುವರಿ;
		}
loadzone:
		/*
		 * The hard way, load the zone!
		 */
		ಆದರೆ (fromzone(&bp, tm, mandatory))
		    ಮುಂದುವರಿ;
		ಹೋಗು ಹೊರಹೋಗು;
	    }
	    offs = 0;
	    ಗಾಗಿ (i = 0; i < 4; )
	    {
		ಆದರೆ (ಅಂಕಿಯೇ(*bp))
		{
		    offs = offs * 10 + (*bp++ - '0');
		    i++;
		    ಮುಂದುವರಿ;
		}
		ಆದರೆ (i == 2 && *bp == ':')
		{
		    bp++;
		    ಮುಂದುವರಿ;
		}
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (ಅಂಕಿಯೇ(*bp))
		ಹೋಗು ಹೊರಹೋಗು;
	    ಆಯ್ಕೆ (i)
	    {
	    ಸ್ಥಿತಿ 2:
		offs *= SECSPERHOUR;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 4:
		i = offs % 100;
		offs /= 100;
		ಆದರೆ (i >= SECSPERMIN)
		    ಹೋಗು ಹೊರಹೋಗು;
		/* Convert minutes into decimal */
		offs = offs * SECSPERHOUR + i * SECSPERMIN;
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
ಹೊರಹೋಗು:
		ಆದರೆ (mandatory)
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		bp = zname;
		ಮುಂದುವರಿ;
	    }
	    /* ISO 8601 & RFC 3339 limit to 23:59 max */
	    ಆದರೆ (offs >= (HOURSPERDAY * SECSPERHOUR))
		ಹೋಗು ಹೊರಹೋಗು;
	    ಆದರೆ (neg)
		offs = -offs;
	    tm->tm_isdst = 0;	/* XXX */
	    ಮುಂದುವರಿ;

	/*
	 * Miscellaneous conversions.
	 */
	ಸ್ಥಿತಿ 'n':	/* Any kind of white-space. */
	ಸ್ಥಿತಿ 't':
	    ಆಗ (ಖಾಲಿಯೇ(*bp))
		bp++;
	    LEGAL_ALT(0);
	    ಮುಂದುವರಿ;


	ಪೂರ್ವನಿಯೋಜಿತ:	/* Unknown/unsupported conversion. */
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }

    ಆದರೆ (!HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	ಆದರೆ (HAVE_MON(state) && HAVE_MDAY(state))
	{
	    /* calculate day of year (ordinal date) */
	    tm->tm_yday =  ಆರಂಭ_ದ_month[isleap_ಮೊತ್ತ(tm->tm_year,
			    TM_YEAR_ತಳ)][tm->tm_mon] + (tm->tm_mday - 1);
	    state |= S_YDAY;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (day_ಸ್ಥಾನಾಂತರ != -1)
	{
	    /*
	     * Set the date to the first Sunday (or Monday)
	     * of the specified week of the year.
	     */
	    ಆದರೆ (!HAVE_WDAY(state))
	    {
		tm->tm_wday = day_ಸ್ಥಾನಾಂತರ;
		state |= S_WDAY;
	    }
	    tm->tm_yday = (7 -
		    ಮೊದಲ_wday_ದ(tm->tm_year + TM_YEAR_ತಳ) +
		    day_ಸ್ಥಾನಾಂತರ) % 7 + (week_ಸ್ಥಾನಾಂತರ - 1) * 7 +
		    tm->tm_wday  - day_ಸ್ಥಾನಾಂತರ;
	    state |= S_YDAY;
	}
    }

    ಆದರೆ (HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	ಪೂರ್ಣಾಂಕ leap;

	ಆದರೆ (!HAVE_MON(state))
	{
	    /* calculate month of day of year */
	    i = 0;
	    leap = isleap_ಮೊತ್ತ(tm->tm_year, TM_YEAR_ತಳ);
	    ಆಗ (tm->tm_yday >= ಆರಂಭ_ದ_month[leap][i])
		i++;
	    ಆದರೆ (i > 12)
	    {
		i = 1;
		tm->tm_yday -= ಆರಂಭ_ದ_month[leap][12];
		tm->tm_year++;
	    }
	    tm->tm_mon = i - 1;
	    state |= S_MON;
	}

	ಆದರೆ (!HAVE_MDAY(state))
	{
	    /* calculate day of month */
	    leap = isleap_ಮೊತ್ತ(tm->tm_year, TM_YEAR_ತಳ);
	    tm->tm_mday = tm->tm_yday -
		ಆರಂಭ_ದ_month[leap][tm->tm_mon] + 1;
	    state |= S_MDAY;
	}

	ಆದರೆ (!HAVE_WDAY(state))
	{
	    /* calculate day of week */
	    i = 0;
	    week_ಸ್ಥಾನಾಂತರ = ಮೊದಲ_wday_ದ(tm->tm_year);
	    ಆಗ (i++ <= tm->tm_yday)
	    {
		ಆದರೆ (week_ಸ್ಥಾನಾಂತರ++ >= 6)
		    week_ಸ್ಥಾನಾಂತರ = 0;
	    }
	    tm->tm_wday = week_ಸ್ಥಾನಾಂತರ;
	    state |= S_WDAY;
	}
    }

    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)bp;
}


    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *
conv_ಅಂಕೆ(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, ಪೂರ್ಣಾಂಕ *dest, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ llim,
						    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ulim)
{
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = 0;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ch;

    /* The limit also determines the number of valid digits. */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ rulim = ulim;

    ch = *buf;
    ಆದರೆ (ch < '0' || ch > '9')
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಮಾಡು
    {
	ಫಲಿತಾಂಶ *= 10;
	ಫಲಿತಾಂಶ += ch - '0';
	rulim /= 10;
	ch = *++buf;
    } ಆಗ ((ಫಲಿತಾಂಶ * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

    ಆದರೆ (ಫಲಿತಾಂಶ < llim || ಫಲಿತಾಂಶ > ulim)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    *dest = ಫಲಿತಾಂಶ;
    ಹಿಂತಿರುಗಿ buf;
}

    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *
ಕಂಡುಹಿಡಿ_string(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *bp, ಪೂರ್ಣಾಂಕ *tgt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ *n1,
					    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ *n2, ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ		i;
    size_t	len;

    /* check full name - then abbreviated ones */
    ಗಾಗಿ (; n1 != ಖಾಲಿ; n1 = n2, n2 = ಖಾಲಿ)
    {
	ಗಾಗಿ (i = 0; i < c; i++, n1++)
	{
	    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*n1);
	    ಆದರೆ (STRNICMP(*n1, bp, len) == 0)
	    {
		*tgt = i;
		ಹಿಂತಿರುಗಿ bp + len;
	    }
	}
    }

    /* Nothing matched */
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ_ಅಂತ್ಯ // MSWIN
