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

#ଅନ୍ତର୍ଭାଵ "vim.h"

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
#ଯଦି defined(_UCRT) || defined(_MSC_VER)
# define USE_GET_TZNAME
#endif

/* Locale tables (English / "C" locale). */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ c_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ c_abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ c_mon[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ c_abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ c_am_pm[] = { "AM", "PM" };
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର c_d_t_fmt[] = "%a %b %e %H:%M:%S %Y";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର c_d_fmt[] = "%m/%d/%y";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର c_t_fmt[] = "%H:%M:%S";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର c_t_fmt_ampm[] = "%I:%M:%S %p";

ସ୍ଥିର ଅଚଳ unsigned ଅକ୍ଷର *conv_ଅଙ୍କ(ଅଚଳ unsigned ଅକ୍ଷର *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *,
						unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ଅଚଳ unsigned ଅକ୍ଷର *find_string(ଅଚଳ unsigned ଅକ୍ଷର *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *,
			ଅଚଳ ଅକ୍ଷର * ଅଚଳ *, ଅଚଳ ଅକ୍ଷର * ଅଚଳ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E			0x01
#define ALT_O			0x02
#define LEGAL_ALT(x)		{ ଯଦି (alt_format & ~(x)) ଫେରନ୍ତୁ ଖାଲି; }

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
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ nast[5] = {
    "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ nadt[5] = {
    "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};

/*
 * Table to determine the ordinal date for the start of a month.
 * Ref: http://en.wikipedia.org/wiki/ISO_week_date
 */
ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_of_month[2][13] = {
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
first_wday_of(ପୂର୍ଣ୍ଣସଂଖ୍ୟା yr)
{
    ଫେରନ୍ତୁ ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
	    (isleap(yr) ? 6 : 0) + 1) % 7;
}

#define delim(p)	((p) == '\0' || isspace((unsigned ଅକ୍ଷର)(p)))

/*
 * Stub replacing NetBSD's fromzone(): the Windows CRT does not provide
 * tzalloc() / tzgetgmtoff() and the associated IANA tzfile loader, so
 * we simply decline to recognize arbitrary timezone names.  The %Z / %z
 * numeric and RFC-822/2822 forms are still handled by the caller.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fromzone(ଅଚଳ unsigned ଅକ୍ଷର **bp UNUSED, ସଂରଚନା tm *tm UNUSED,
						    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mandatory UNUSED)
{
    ଫେରନ୍ତୁ 0;
}

    ଅକ୍ଷର *
strptime(ଅଚଳ ଅକ୍ଷର *buf, ଅଚଳ ଅକ୍ଷର *fmt, ସଂରଚନା tm *tm)
{
    unsigned ଅକ୍ଷର c;
    ଅଚଳ unsigned ଅକ୍ଷର *bp, *ep, *zname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା alt_format, i, split_year = 0, neg = 0, state = 0,
	day_offset = -1, week_offset = 0, offs, mandatory;
    ଅଚଳ ଅକ୍ଷର *new_fmt;

    bp = (ଅଚଳ unsigned ଅକ୍ଷର *)buf;

    ଯେତେବେଳେ (bp != ଖାଲି && (c = *fmt++) != '\0')
    {
	/* Clear `alternate' modifier prior to new conversion. */
	alt_format = 0;
	i = 0;

	/* Eat up white-space. */
	ଯଦି (isspace(c))
	{
	    ଯେତେବେଳେ (isspace(*bp))
		bp++;
	    ଜାରି;
	}

	ଯଦି (c != '%')
	    ଯାଅ literal;


again:	ବାଛ (c = *fmt++)
	{
	ସ୍ଥିତି '%':	/* "%%" is converted to "%". */
literal:
	    ଯଦି (c != *bp++)
		ଫେରନ୍ତୁ ଖାଲି;
	    LEGAL_ALT(0);
	    ଜାରି;

	/*
	 * "Alternative" modifiers. Just set the appropriate flag
	 * and start over again.
	 */
	ସ୍ଥିତି 'E':	/* "%E?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_E;
	    ଯାଅ again;

	ସ୍ଥିତି 'O':	/* "%O?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_O;
	    ଯାଅ again;

	/*
	 * "Complex" conversion rules, implemented through recursion.
	 */
	ସ୍ଥିତି 'c':	/* Date and time, using the locale's format. */
	    new_fmt = c_d_t_fmt;
	    state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
	    ଯାଅ recurse;

	ସ୍ଥିତି 'D':	/* The date as "%m/%d/%y". */
	    new_fmt = "%m/%d/%y";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    ଯାଅ recurse;

	ସ୍ଥିତି 'F':	/* The date as "%Y-%m-%d". */
	    new_fmt = "%Y-%m-%d";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    ଯାଅ recurse;

	ସ୍ଥିତି 'R':	/* The time as "%H:%M". */
	    new_fmt = "%H:%M";
	    LEGAL_ALT(0);
	    ଯାଅ recurse;

	ସ୍ଥିତି 'r':	/* The time in 12-hour clock representation. */
	    new_fmt = c_t_fmt_ampm;
	    LEGAL_ALT(0);
	    ଯାଅ recurse;

	ସ୍ଥିତି 'T':	/* The time as "%H:%M:%S". */
	    new_fmt = "%H:%M:%S";
	    LEGAL_ALT(0);
	    ଯାଅ recurse;

	ସ୍ଥିତି 'X':	/* The time, using the locale's format. */
	    new_fmt = c_t_fmt;
	    ଯାଅ recurse;

	ସ୍ଥିତି 'x':	/* The date, using the locale's format. */
	    new_fmt = c_d_fmt;
	    state |= S_MON | S_MDAY | S_YEAR;
	recurse:
	    bp = (ଅଚଳ unsigned ଅକ୍ଷର *)strptime((ଅଚଳ ଅକ୍ଷର *)bp,
							new_fmt, tm);
	    LEGAL_ALT(ALT_E);
	    ଜାରି;

	/*
	 * "Elementary" conversion rules.
	 */
	ସ୍ଥିତି 'A':	/* The day of week, using the locale's form. */
	ସ୍ଥିତି 'a':
	    bp = find_string(bp, &tm->tm_wday, c_day, c_abday, 7);
	    LEGAL_ALT(0);
	    state |= S_WDAY;
	    ଜାରି;

	ସ୍ଥିତି 'B':	/* The month, using the locale's form. */
	ସ୍ଥିତି 'b':
	ସ୍ଥିତି 'h':
	    bp = find_string(bp, &tm->tm_mon, c_mon, c_abmon, 12);
	    LEGAL_ALT(0);
	    state |= S_MON;
	    ଜାରି;

	ସ୍ଥିତି 'C':	/* The century number. */
	    i = 20;
	    bp = conv_ଅଙ୍କ(bp, &i, 0, 99);

	    i = i * 100 - TM_YEAR_BASE;
	    ଯଦି (split_year)
		i += tm->tm_year % 100;
	    split_year = 1;
	    tm->tm_year = i;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    ଜାରି;

	ସ୍ଥିତି 'd':	/* The day of month. */
	ସ୍ଥିତି 'e':
	    bp = conv_ଅଙ୍କ(bp, &tm->tm_mday, 1, 31);
	    LEGAL_ALT(ALT_O);
	    state |= S_MDAY;
	    ଜାରି;

	ସ୍ଥିତି 'k':	/* The hour (24-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	ସ୍ଥିତି 'H':
	    bp = conv_ଅଙ୍କ(bp, &tm->tm_hour, 0, 23);
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    ଜାରି;

	ସ୍ଥିତି 'l':	/* The hour (12-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	ସ୍ଥିତି 'I':
	    bp = conv_ଅଙ୍କ(bp, &tm->tm_hour, 1, 12);
	    ଯଦି (tm->tm_hour == 12)
		tm->tm_hour = 0;
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    ଜାରି;

	ସ୍ଥିତି 'j':	/* The day of year. */
	    i = 1;
	    bp = conv_ଅଙ୍କ(bp, &i, 1, 366);
	    tm->tm_yday = i - 1;
	    LEGAL_ALT(0);
	    state |= S_YDAY;
	    ଜାରି;

	ସ୍ଥିତି 'M':	/* The minute. */
	    bp = conv_ଅଙ୍କ(bp, &tm->tm_min, 0, 59);
	    LEGAL_ALT(ALT_O);
	    ଜାରି;

	ସ୍ଥିତି 'm':	/* The month. */
	    i = 1;
	    bp = conv_ଅଙ୍କ(bp, &i, 1, 12);
	    tm->tm_mon = i - 1;
	    LEGAL_ALT(ALT_O);
	    state |= S_MON;
	    ଜାରି;

	ସ୍ଥିତି 'p':	/* The locale's equivalent of AM/PM. */
	    bp = find_string(bp, &i, c_am_pm, ଖାଲି, 2);
	    ଯଦି (HAVE_HOUR(state) && tm->tm_hour > 11)
		ଫେରନ୍ତୁ ଖାଲି;
	    tm->tm_hour += i * 12;
	    LEGAL_ALT(0);
	    ଜାରି;

	ସ୍ଥିତି 'S':	/* The seconds. */
	    bp = conv_ଅଙ୍କ(bp, &tm->tm_sec, 0, 61);
	    LEGAL_ALT(ALT_O);
	    ଜାରି;

	ସ୍ଥିତି 's':	/* seconds since the epoch */
	    {
		ଅଚଳ time_t TIME_MAX = (time_t)((ଆକାର(time_t) == 8)
					? LLONG_MAX : INT_MAX);
		time_t sse, d;

		ଯଦି (*bp < '0' || *bp > '9')
		{
		    bp = ଖାଲି;
		    ଜାରି;
		}

		sse = *bp++ - '0';
		ଯେତେବେଳେ (*bp >= '0' && *bp <= '9')
		{
		    d = *bp++ - '0';
		    ଯଦି (sse > TIME_MAX / 10)
		    {
			bp = ଖାଲି;
			ଭାଙ୍ଗ;
		    }
		    sse *= 10;
		    ଯଦି (sse > TIME_MAX - d)
		    {
			bp = ଖାଲି;
			ଭାଙ୍ଗ;
		    }
		    sse += d;
		}
		ଯଦି (bp == ଖାଲି)
		    ଜାରି;

		{
		    ସଂରଚନା tm *lt = localtime(&sse);

		    ଯଦି (lt == ଖାଲି)
			bp = ଖାଲି;
		    ଅନ୍ୟଥା
		    {
			*tm = *lt;
			state |= S_YDAY | S_WDAY |
				S_MON | S_MDAY | S_YEAR;
		    }
		}
		ଜାରି;
	    }

	ସ୍ଥିତି 'U':	/* The week of year, beginning on sunday. */
	ସ୍ଥିତି 'W':	/* The week of year, beginning on monday. */
	    /*
	     * This is bogus, as we can not assume any valid
	     * information present in the tm structure at this
	     * point to calculate a real value, so save the
	     * week for now in case it can be used later.
	     */
	    bp = conv_ଅଙ୍କ(bp, &i, 0, 53);
	    LEGAL_ALT(ALT_O);
	    ଯଦି (c == 'U')
		day_offset = TM_SUNDAY;
	    ଅନ୍ୟଥା
		day_offset = TM_MONDAY;
	    week_offset = i;
	    ଜାରି;

	ସ୍ଥିତି 'w':	/* The day of week, beginning on sunday. */
	    bp = conv_ଅଙ୍କ(bp, &tm->tm_wday, 0, 6);
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    ଜାରି;

	ସ୍ଥିତି 'u':	/* The day of week, monday = 1. */
	    bp = conv_ଅଙ୍କ(bp, &i, 1, 7);
	    tm->tm_wday = i % 7;
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    ଜାରି;

	ସ୍ଥିତି 'g':	/* ISO week year without century (parsed, ignored). */
	    bp = conv_ଅଙ୍କ(bp, &i, 0, 99);
	    ଜାରି;

	ସ୍ଥିତି 'G':	/* ISO week year with century (parsed, ignored). */
	    କର
		bp++;
	    ଯେତେବେଳେ (isdigit(*bp));
	    ଜାରି;

	ସ୍ଥିତି 'V':	/* ISO 8601:1988 week number (parsed, ignored). */
	    bp = conv_ଅଙ୍କ(bp, &i, 1, 53);
	    ଜାରି;

	ସ୍ଥିତି 'Y':	/* The year. */
	    i = TM_YEAR_BASE;	/* just for data sanity... */
	    bp = conv_ଅଙ୍କ(bp, &i, 0, 9999);
	    tm->tm_year = i - TM_YEAR_BASE;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    ଜାରି;

	ସ୍ଥିତି 'y':	/* The year within 100 years of the epoch. */
	    /* LEGAL_ALT(ALT_E | ALT_O); */
	    bp = conv_ଅଙ୍କ(bp, &i, 0, 99);

	    ଯଦି (split_year)
		/* preserve century */
		i += (tm->tm_year / 100) * 100;
	    ଅନ୍ୟଥା
	    {
		split_year = 1;
		ଯଦି (i <= 68)
		    i = i + 2000 - TM_YEAR_BASE;
		ଅନ୍ୟଥା
		    i = i + 1900 - TM_YEAR_BASE;
	    }
	    tm->tm_year = i;
	    state |= S_YEAR;
	    ଜାରି;

	ସ୍ଥିତି 'Z':
	ସ୍ଥିତି 'z':
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
	    ଯଦି (mandatory)
		ଯେତେବେଳେ (isspace(*bp))
		    bp++;

	    zname = bp;
	    ବାଛ (*bp++)
	    {
	    ସ୍ଥିତି 'G':
		ଯଦି (*bp++ != 'M')
		    ଯାଅ namedzone;
		/*FALLTHROUGH*/
	    ସ୍ଥିତି 'U':
		ଯଦି (*bp++ != 'T')
		    ଯାଅ namedzone;
		ଅନ୍ୟଥା ଯଦି (!delim(*bp) && *bp++ != 'C')
		    ଯାଅ namedzone;
		/*FALLTHROUGH*/
	    ସ୍ଥିତି 'Z':
		ଯଦି (!delim(*bp))
		    ଯାଅ namedzone;
		tm->tm_isdst = 0;
		ଜାରି;
	    ସ୍ଥିତି '+':
		neg = 0;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି '-':
		neg = 1;
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
namedzone:
		bp = zname;

		/* Nautical / Military style */
		ଯଦି (delim(bp[1]) &&
			((*bp >= 'A' && *bp <= 'I') ||
			 (*bp >= 'L' && *bp <= 'Y')))
		{
		    bp++;
		    ଜାରି;
		}
		/* 'J' is local time */
		ଯଦି (delim(bp[1]) && *bp == 'J')
		{
		    bp++;
		    ଜାରି;
		}

		/*
		 * From our 3 letter hard-coded table.
		 */
		ଯଦି (delim(bp[0]) || delim(bp[1]) ||
			delim(bp[2]) || !delim(bp[3]))
		    ଯାଅ loadzone;
		ep = find_string(bp, &i, nast, ଖାଲି, 4);
		ଯଦି (ep != ଖାଲି)
		{
		    bp = ep;
		    ଜାରି;
		}
		ep = find_string(bp, &i, nadt, ଖାଲି, 4);
		ଯଦି (ep != ଖାଲି)
		{
		    tm->tm_isdst = 1;
		    bp = ep;
		    ଜାରି;
		}
		/*
		 * Our current timezone.  Prefer _get_tzname() over the
		 * tzname[] global, which UCRT marks deprecated because it
		 * may be inaccurate after locale changes.
		 */
#ifdef USE_GET_TZNAME
		{
		    ଅକ୍ଷର tzbuf[2][32];
		    ଅଚଳ ଅକ୍ଷର *tznames[2] = { tzbuf[0], tzbuf[1] };
		    size_t tzlen;

		    _tzset();
		    ଯଦି (_get_tzname(&tzlen, tzbuf[0], ଆକାର(tzbuf[0]), 0) != 0)
			tzbuf[0][0] = NUL;
		    ଯଦି (_get_tzname(&tzlen, tzbuf[1], ଆକାର(tzbuf[1]), 1) != 0)
			tzbuf[1][0] = NUL;
		    ep = find_string(bp, &i, tznames, ଖାଲି, 2);
		}
#ଅନ୍ୟଥା
		ep = find_string(bp, &i,
			(ଅଚଳ ଅକ୍ଷର *ଅଚଳ *)tzname, ଖାଲି, 2);
#endif
		ଯଦି (ep != ଖାଲି)
		{
		    tm->tm_isdst = i;
		    bp = ep;
		    ଜାରି;
		}
loadzone:
		/*
		 * The hard way, load the zone!
		 */
		ଯଦି (fromzone(&bp, tm, mandatory))
		    ଜାରି;
		ଯାଅ out;
	    }
	    offs = 0;
	    ପାଇଁ (i = 0; i < 4; )
	    {
		ଯଦି (isdigit(*bp))
		{
		    offs = offs * 10 + (*bp++ - '0');
		    i++;
		    ଜାରି;
		}
		ଯଦି (i == 2 && *bp == ':')
		{
		    bp++;
		    ଜାରି;
		}
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (isdigit(*bp))
		ଯାଅ out;
	    ବାଛ (i)
	    {
	    ସ୍ଥିତି 2:
		offs *= SECSPERHOUR;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି 4:
		i = offs % 100;
		offs /= 100;
		ଯଦି (i >= SECSPERMIN)
		    ଯାଅ out;
		/* Convert minutes into decimal */
		offs = offs * SECSPERHOUR + i * SECSPERMIN;
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
out:
		ଯଦି (mandatory)
		    ଫେରନ୍ତୁ ଖାଲି;
		bp = zname;
		ଜାରି;
	    }
	    /* ISO 8601 & RFC 3339 limit to 23:59 max */
	    ଯଦି (offs >= (HOURSPERDAY * SECSPERHOUR))
		ଯାଅ out;
	    ଯଦି (neg)
		offs = -offs;
	    tm->tm_isdst = 0;	/* XXX */
	    ଜାରି;

	/*
	 * Miscellaneous conversions.
	 */
	ସ୍ଥିତି 'n':	/* Any kind of white-space. */
	ସ୍ଥିତି 't':
	    ଯେତେବେଳେ (isspace(*bp))
		bp++;
	    LEGAL_ALT(0);
	    ଜାରି;


	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	/* Unknown/unsupported conversion. */
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }

    ଯଦି (!HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	ଯଦି (HAVE_MON(state) && HAVE_MDAY(state))
	{
	    /* calculate day of year (ordinal date) */
	    tm->tm_yday =  start_of_month[isleap_sum(tm->tm_year,
			    TM_YEAR_BASE)][tm->tm_mon] + (tm->tm_mday - 1);
	    state |= S_YDAY;
	}
	ଅନ୍ୟଥା ଯଦି (day_offset != -1)
	{
	    /*
	     * Set the date to the first Sunday (or Monday)
	     * of the specified week of the year.
	     */
	    ଯଦି (!HAVE_WDAY(state))
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

    ଯଦି (HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା leap;

	ଯଦି (!HAVE_MON(state))
	{
	    /* calculate month of day of year */
	    i = 0;
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    ଯେତେବେଳେ (tm->tm_yday >= start_of_month[leap][i])
		i++;
	    ଯଦି (i > 12)
	    {
		i = 1;
		tm->tm_yday -= start_of_month[leap][12];
		tm->tm_year++;
	    }
	    tm->tm_mon = i - 1;
	    state |= S_MON;
	}

	ଯଦି (!HAVE_MDAY(state))
	{
	    /* calculate day of month */
	    leap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
	    tm->tm_mday = tm->tm_yday -
		start_of_month[leap][tm->tm_mon] + 1;
	    state |= S_MDAY;
	}

	ଯଦି (!HAVE_WDAY(state))
	{
	    /* calculate day of week */
	    i = 0;
	    week_offset = first_wday_of(tm->tm_year);
	    ଯେତେବେଳେ (i++ <= tm->tm_yday)
	    {
		ଯଦି (week_offset++ >= 6)
		    week_offset = 0;
	    }
	    tm->tm_wday = week_offset;
	    state |= S_WDAY;
	}
    }

    ଫେରନ୍ତୁ (ଅକ୍ଷର *)bp;
}


    ସ୍ଥିର ଅଚଳ unsigned ଅକ୍ଷର *
conv_ଅଙ୍କ(ଅଚଳ unsigned ଅକ୍ଷର *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dest, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା llim,
						    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ulim)
{
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = 0;
    unsigned ଅକ୍ଷର ch;

    /* The limit also determines the number of valid digits. */
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା rulim = ulim;

    ch = *buf;
    ଯଦି (ch < '0' || ch > '9')
	ଫେରନ୍ତୁ ଖାଲି;

    କର
    {
	result *= 10;
	result += ch - '0';
	rulim /= 10;
	ch = *++buf;
    } ଯେତେବେଳେ ((result * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

    ଯଦି (result < llim || result > ulim)
	ଫେରନ୍ତୁ ଖାଲି;

    *dest = result;
    ଫେରନ୍ତୁ buf;
}

    ସ୍ଥିର ଅଚଳ unsigned ଅକ୍ଷର *
find_string(ଅଚଳ unsigned ଅକ୍ଷର *bp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *tgt, ଅଚଳ ଅକ୍ଷର *ଅଚଳ *n1,
					    ଅଚଳ ଅକ୍ଷର *ଅଚଳ *n2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    size_t	len;

    /* check full name - then abbreviated ones */
    ପାଇଁ (; n1 != ଖାଲି; n1 = n2, n2 = ଖାଲି)
    {
	ପାଇଁ (i = 0; i < c; i++, n1++)
	{
	    len = strlen(*n1);
	    ଯଦି (STRNICMP(*n1, bp, len) == 0)
	    {
		*tgt = i;
		ଫେରନ୍ତୁ bp + len;
	    }
	}
    }

    /* Nothing matched */
    ଫେରନ୍ତୁ ଖାଲି;
}

#endif // MSWIN
