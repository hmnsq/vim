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

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే MSWIN

/* Constants lifted from NetBSD's <tzfile.h>. */
#నిర్వచించు TM_YEAR_పునాది	1900
#నిర్వచించు TM_SUNDAY	0
#నిర్వచించు TM_MONDAY	1
#నిర్వచించు SECSPERHOUR	3600
#నిర్వచించు SECSPERMIN	60
#నిర్వచించు HOURSPERDAY	24

#నిర్వచించు isleap(y)	(((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)
/* isleap_sum avoids integer overflow when adding the 1900 base. */
#నిర్వచించు isleap_కూడిక(a, b) \
			isleap(((గుర్తులేని)(a) + (గుర్తులేని)(b)) % 400)

/*
 * _get_tzname() is provided by the UCRT and by the MSVC CRT.  Older MinGW
 * builds linking against msvcrt.dll fall back to the deprecated tzname[]
 * global (mingw-w64 already exposes the POSIX-named tzname[]).
 */
#ఒకవేళ defined(_UCRT) || defined(_MSC_VER)
# define USE_GET_TZNAME
#అయితే_ముగింపు

/* Locale tables (English / "C" locale). */
స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి c_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి c_abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి c_mon[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి c_abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి c_am_pm[] = { "AM", "PM" };
స్థిర స్థిరరాశి అక్షరం c_d_t_fmt[] = "%a %b %e %H:%M:%S %Y";
స్థిర స్థిరరాశి అక్షరం c_d_fmt[] = "%m/%d/%y";
స్థిర స్థిరరాశి అక్షరం c_t_fmt[] = "%H:%M:%S";
స్థిర స్థిరరాశి అక్షరం c_t_fmt_ampm[] = "%I:%M:%S %p";

స్థిర స్థిరరాశి గుర్తులేని అక్షరం *conv_అంకె(స్థిరరాశి గుర్తులేని అక్షరం *, పూర్ణాంకం *,
						గుర్తులేని పూర్ణాంకం, గుర్తులేని పూర్ణాంకం);
స్థిర స్థిరరాశి గుర్తులేని అక్షరం *కనుగొను_string(స్థిరరాశి గుర్తులేని అక్షరం *, పూర్ణాంకం *,
			స్థిరరాశి అక్షరం * స్థిరరాశి *, స్థిరరాశి అక్షరం * స్థిరరాశి *, పూర్ణాంకం);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#నిర్వచించు ALT_E			0x01
#నిర్వచించు ALT_O			0x02
#నిర్వచించు LEGAL_ALT(x)		{ ఒకవేళ (alt_format & ~(x)) తిరిగి ఖాళీ; }

#నిర్వచించు S_YEAR			(1 << 0)
#నిర్వచించు S_MON			(1 << 1)
#నిర్వచించు S_YDAY			(1 << 2)
#నిర్వచించు S_MDAY			(1 << 3)
#నిర్వచించు S_WDAY			(1 << 4)
#నిర్వచించు S_HOUR			(1 << 5)

#నిర్వచించు HAVE_MDAY(s)		(s & S_MDAY)
#నిర్వచించు HAVE_MON(s)		(s & S_MON)
#నిర్వచించు HAVE_WDAY(s)		(s & S_WDAY)
#నిర్వచించు HAVE_YDAY(s)		(s & S_YDAY)
#నిర్వచించు HAVE_YEAR(s)		(s & S_YEAR)
#నిర్వచించు HAVE_HOUR(s)		(s & S_HOUR)

/* RFC-822/RFC-2822 */
స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి nast[5] = {
    "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి nadt[5] = {
    "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};

/*
 * Table to determine the ordinal date for the start of a month.
 * Ref: http://en.wikipedia.org/wiki/ISO_week_date
 */
స్థిర స్థిరరాశి పూర్ణాంకం ప్రారంభం_యొక్క_month[2][13] = {
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
    స్థిర పూర్ణాంకం
మొదటి_wday_యొక్క(పూర్ణాంకం yr)
{
    తిరిగి ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
	    (isleap(yr) ? 6 : 0) + 1) % 7;
}

#నిర్వచించు delim(p)	((p) == '\0' || ఖాళీనా((గుర్తులేని అక్షరం)(p)))

/*
 * Stub replacing NetBSD's fromzone(): the Windows CRT does not provide
 * tzalloc() / tzgetgmtoff() and the associated IANA tzfile loader, so
 * we simply decline to recognize arbitrary timezone names.  The %Z / %z
 * numeric and RFC-822/2822 forms are still handled by the caller.
 */
    స్థిర పూర్ణాంకం
fromzone(స్థిరరాశి గుర్తులేని అక్షరం **bp వాడని, నిర్మాణం tm *tm వాడని,
						    పూర్ణాంకం mandatory వాడని)
{
    తిరిగి 0;
}

    అక్షరం *
strptime(స్థిరరాశి అక్షరం *buf, స్థిరరాశి అక్షరం *fmt, నిర్మాణం tm *tm)
{
    గుర్తులేని అక్షరం c;
    స్థిరరాశి గుర్తులేని అక్షరం *bp, *ep, *zname;
    పూర్ణాంకం alt_format, i, విభజించు_year = 0, neg = 0, state = 0,
	day_స్థానభ్రంశం = -1, week_స్థానభ్రంశం = 0, offs, mandatory;
    స్థిరరాశి అక్షరం *new_fmt;

    bp = (స్థిరరాశి గుర్తులేని అక్షరం *)buf;

    అయితే (bp != ఖాళీ && (c = *fmt++) != '\0')
    {
	/* Clear `alternate' modifier prior to new conversion. */
	alt_format = 0;
	i = 0;

	/* Eat up white-space. */
	ఒకవేళ (ఖాళీనా(c))
	{
	    అయితే (ఖాళీనా(*bp))
		bp++;
	    కొనసాగు;
	}

	ఒకవేళ (c != '%')
	    వెళ్ళు literal;


again:	ఎంపిక (c = *fmt++)
	{
	స్థితి '%':	/* "%%" is converted to "%". */
literal:
	    ఒకవేళ (c != *bp++)
		తిరిగి ఖాళీ;
	    LEGAL_ALT(0);
	    కొనసాగు;

	/*
	 * "Alternative" modifiers. Just set the appropriate flag
	 * and start over again.
	 */
	స్థితి 'E':	/* "%E?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_E;
	    వెళ్ళు again;

	స్థితి 'O':	/* "%O?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_O;
	    వెళ్ళు again;

	/*
	 * "Complex" conversion rules, implemented through recursion.
	 */
	స్థితి 'c':	/* Date and time, using the locale's format. */
	    new_fmt = c_d_t_fmt;
	    state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
	    వెళ్ళు recurse;

	స్థితి 'D':	/* The date as "%m/%d/%y". */
	    new_fmt = "%m/%d/%y";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    వెళ్ళు recurse;

	స్థితి 'F':	/* The date as "%Y-%m-%d". */
	    new_fmt = "%Y-%m-%d";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    వెళ్ళు recurse;

	స్థితి 'R':	/* The time as "%H:%M". */
	    new_fmt = "%H:%M";
	    LEGAL_ALT(0);
	    వెళ్ళు recurse;

	స్థితి 'r':	/* The time in 12-hour clock representation. */
	    new_fmt = c_t_fmt_ampm;
	    LEGAL_ALT(0);
	    వెళ్ళు recurse;

	స్థితి 'T':	/* The time as "%H:%M:%S". */
	    new_fmt = "%H:%M:%S";
	    LEGAL_ALT(0);
	    వెళ్ళు recurse;

	స్థితి 'X':	/* The time, using the locale's format. */
	    new_fmt = c_t_fmt;
	    వెళ్ళు recurse;

	స్థితి 'x':	/* The date, using the locale's format. */
	    new_fmt = c_d_fmt;
	    state |= S_MON | S_MDAY | S_YEAR;
	recurse:
	    bp = (స్థిరరాశి గుర్తులేని అక్షరం *)strptime((స్థిరరాశి అక్షరం *)bp,
							new_fmt, tm);
	    LEGAL_ALT(ALT_E);
	    కొనసాగు;

	/*
	 * "Elementary" conversion rules.
	 */
	స్థితి 'A':	/* The day of week, using the locale's form. */
	స్థితి 'a':
	    bp = కనుగొను_string(bp, &tm->tm_wday, c_day, c_abday, 7);
	    LEGAL_ALT(0);
	    state |= S_WDAY;
	    కొనసాగు;

	స్థితి 'B':	/* The month, using the locale's form. */
	స్థితి 'b':
	స్థితి 'h':
	    bp = కనుగొను_string(bp, &tm->tm_mon, c_mon, c_abmon, 12);
	    LEGAL_ALT(0);
	    state |= S_MON;
	    కొనసాగు;

	స్థితి 'C':	/* The century number. */
	    i = 20;
	    bp = conv_అంకె(bp, &i, 0, 99);

	    i = i * 100 - TM_YEAR_పునాది;
	    ఒకవేళ (విభజించు_year)
		i += tm->tm_year % 100;
	    విభజించు_year = 1;
	    tm->tm_year = i;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    కొనసాగు;

	స్థితి 'd':	/* The day of month. */
	స్థితి 'e':
	    bp = conv_అంకె(bp, &tm->tm_mday, 1, 31);
	    LEGAL_ALT(ALT_O);
	    state |= S_MDAY;
	    కొనసాగు;

	స్థితి 'k':	/* The hour (24-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	స్థితి 'H':
	    bp = conv_అంకె(bp, &tm->tm_hour, 0, 23);
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    కొనసాగు;

	స్థితి 'l':	/* The hour (12-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	స్థితి 'I':
	    bp = conv_అంకె(bp, &tm->tm_hour, 1, 12);
	    ఒకవేళ (tm->tm_hour == 12)
		tm->tm_hour = 0;
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    కొనసాగు;

	స్థితి 'j':	/* The day of year. */
	    i = 1;
	    bp = conv_అంకె(bp, &i, 1, 366);
	    tm->tm_yday = i - 1;
	    LEGAL_ALT(0);
	    state |= S_YDAY;
	    కొనసాగు;

	స్థితి 'M':	/* The minute. */
	    bp = conv_అంకె(bp, &tm->tm_min, 0, 59);
	    LEGAL_ALT(ALT_O);
	    కొనసాగు;

	స్థితి 'm':	/* The month. */
	    i = 1;
	    bp = conv_అంకె(bp, &i, 1, 12);
	    tm->tm_mon = i - 1;
	    LEGAL_ALT(ALT_O);
	    state |= S_MON;
	    కొనసాగు;

	స్థితి 'p':	/* The locale's equivalent of AM/PM. */
	    bp = కనుగొను_string(bp, &i, c_am_pm, ఖాళీ, 2);
	    ఒకవేళ (HAVE_HOUR(state) && tm->tm_hour > 11)
		తిరిగి ఖాళీ;
	    tm->tm_hour += i * 12;
	    LEGAL_ALT(0);
	    కొనసాగు;

	స్థితి 'S':	/* The seconds. */
	    bp = conv_అంకె(bp, &tm->tm_sec, 0, 61);
	    LEGAL_ALT(ALT_O);
	    కొనసాగు;

	స్థితి 's':	/* seconds since the epoch */
	    {
		స్థిరరాశి time_t సమయం_MAX = (time_t)((పరిమాణం(time_t) == 8)
					? LLONG_MAX : గరిష్ఠ_పూర్ణాంకం);
		time_t sse, d;

		ఒకవేళ (*bp < '0' || *bp > '9')
		{
		    bp = ఖాళీ;
		    కొనసాగు;
		}

		sse = *bp++ - '0';
		అయితే (*bp >= '0' && *bp <= '9')
		{
		    d = *bp++ - '0';
		    ఒకవేళ (sse > సమయం_MAX / 10)
		    {
			bp = ఖాళీ;
			ఆపు;
		    }
		    sse *= 10;
		    ఒకవేళ (sse > సమయం_MAX - d)
		    {
			bp = ఖాళీ;
			ఆపు;
		    }
		    sse += d;
		}
		ఒకవేళ (bp == ఖాళీ)
		    కొనసాగు;

		{
		    నిర్మాణం tm *lt = localtime(&sse);

		    ఒకవేళ (lt == ఖాళీ)
			bp = ఖాళీ;
		    లేకపోతే
		    {
			*tm = *lt;
			state |= S_YDAY | S_WDAY |
				S_MON | S_MDAY | S_YEAR;
		    }
		}
		కొనసాగు;
	    }

	స్థితి 'U':	/* The week of year, beginning on sunday. */
	స్థితి 'W':	/* The week of year, beginning on monday. */
	    /*
	     * This is bogus, as we can not assume any valid
	     * information present in the tm structure at this
	     * point to calculate a real value, so save the
	     * week for now in case it can be used later.
	     */
	    bp = conv_అంకె(bp, &i, 0, 53);
	    LEGAL_ALT(ALT_O);
	    ఒకవేళ (c == 'U')
		day_స్థానభ్రంశం = TM_SUNDAY;
	    లేకపోతే
		day_స్థానభ్రంశం = TM_MONDAY;
	    week_స్థానభ్రంశం = i;
	    కొనసాగు;

	స్థితి 'w':	/* The day of week, beginning on sunday. */
	    bp = conv_అంకె(bp, &tm->tm_wday, 0, 6);
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    కొనసాగు;

	స్థితి 'u':	/* The day of week, monday = 1. */
	    bp = conv_అంకె(bp, &i, 1, 7);
	    tm->tm_wday = i % 7;
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    కొనసాగు;

	స్థితి 'g':	/* ISO week year without century (parsed, ignored). */
	    bp = conv_అంకె(bp, &i, 0, 99);
	    కొనసాగు;

	స్థితి 'G':	/* ISO week year with century (parsed, ignored). */
	    చేయి
		bp++;
	    అయితే (అంకెనా(*bp));
	    కొనసాగు;

	స్థితి 'V':	/* ISO 8601:1988 week number (parsed, ignored). */
	    bp = conv_అంకె(bp, &i, 1, 53);
	    కొనసాగు;

	స్థితి 'Y':	/* The year. */
	    i = TM_YEAR_పునాది;	/* just for data sanity... */
	    bp = conv_అంకె(bp, &i, 0, 9999);
	    tm->tm_year = i - TM_YEAR_పునాది;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    కొనసాగు;

	స్థితి 'y':	/* The year within 100 years of the epoch. */
	    /* LEGAL_ALT(ALT_E | ALT_O); */
	    bp = conv_అంకె(bp, &i, 0, 99);

	    ఒకవేళ (విభజించు_year)
		/* preserve century */
		i += (tm->tm_year / 100) * 100;
	    లేకపోతే
	    {
		విభజించు_year = 1;
		ఒకవేళ (i <= 68)
		    i = i + 2000 - TM_YEAR_పునాది;
		లేకపోతే
		    i = i + 1900 - TM_YEAR_పునాది;
	    }
	    tm->tm_year = i;
	    state |= S_YEAR;
	    కొనసాగు;

	స్థితి 'Z':
	స్థితి 'z':
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
	    ఒకవేళ (mandatory)
		అయితే (ఖాళీనా(*bp))
		    bp++;

	    zname = bp;
	    ఎంపిక (*bp++)
	    {
	    స్థితి 'G':
		ఒకవేళ (*bp++ != 'M')
		    వెళ్ళు namedzone;
		/*FALLTHROUGH*/
	    స్థితి 'U':
		ఒకవేళ (*bp++ != 'T')
		    వెళ్ళు namedzone;
		లేకపోతే ఒకవేళ (!delim(*bp) && *bp++ != 'C')
		    వెళ్ళు namedzone;
		/*FALLTHROUGH*/
	    స్థితి 'Z':
		ఒకవేళ (!delim(*bp))
		    వెళ్ళు namedzone;
		tm->tm_isdst = 0;
		కొనసాగు;
	    స్థితి '+':
		neg = 0;
		ఆపు;
	    స్థితి '-':
		neg = 1;
		ఆపు;
	    అప్రమేయ:
namedzone:
		bp = zname;

		/* Nautical / Military style */
		ఒకవేళ (delim(bp[1]) &&
			((*bp >= 'A' && *bp <= 'I') ||
			 (*bp >= 'L' && *bp <= 'Y')))
		{
		    bp++;
		    కొనసాగు;
		}
		/* 'J' is local time */
		ఒకవేళ (delim(bp[1]) && *bp == 'J')
		{
		    bp++;
		    కొనసాగు;
		}

		/*
		 * From our 3 letter hard-coded table.
		 */
		ఒకవేళ (delim(bp[0]) || delim(bp[1]) ||
			delim(bp[2]) || !delim(bp[3]))
		    వెళ్ళు loadzone;
		ep = కనుగొను_string(bp, &i, nast, ఖాళీ, 4);
		ఒకవేళ (ep != ఖాళీ)
		{
		    bp = ep;
		    కొనసాగు;
		}
		ep = కనుగొను_string(bp, &i, nadt, ఖాళీ, 4);
		ఒకవేళ (ep != ఖాళీ)
		{
		    tm->tm_isdst = 1;
		    bp = ep;
		    కొనసాగు;
		}
		/*
		 * Our current timezone.  Prefer _get_tzname() over the
		 * tzname[] global, which UCRT marks deprecated because it
		 * may be inaccurate after locale changes.
		 */
#నిర్వచితమైతే USE_GET_TZNAME
		{
		    అక్షరం tzbuf[2][32];
		    స్థిరరాశి అక్షరం *tznames[2] = { tzbuf[0], tzbuf[1] };
		    size_t tzlen;

		    _tzset();
		    ఒకవేళ (_get_tzname(&tzlen, tzbuf[0], పరిమాణం(tzbuf[0]), 0) != 0)
			tzbuf[0][0] = NUL;
		    ఒకవేళ (_get_tzname(&tzlen, tzbuf[1], పరిమాణం(tzbuf[1]), 1) != 0)
			tzbuf[1][0] = NUL;
		    ep = కనుగొను_string(bp, &i, tznames, ఖాళీ, 2);
		}
#లేకపోతే
		ep = కనుగొను_string(bp, &i,
			(స్థిరరాశి అక్షరం *స్థిరరాశి *)tzname, ఖాళీ, 2);
#అయితే_ముగింపు
		ఒకవేళ (ep != ఖాళీ)
		{
		    tm->tm_isdst = i;
		    bp = ep;
		    కొనసాగు;
		}
loadzone:
		/*
		 * The hard way, load the zone!
		 */
		ఒకవేళ (fromzone(&bp, tm, mandatory))
		    కొనసాగు;
		వెళ్ళు బహిర్గతం;
	    }
	    offs = 0;
	    కొరకు (i = 0; i < 4; )
	    {
		ఒకవేళ (అంకెనా(*bp))
		{
		    offs = offs * 10 + (*bp++ - '0');
		    i++;
		    కొనసాగు;
		}
		ఒకవేళ (i == 2 && *bp == ':')
		{
		    bp++;
		    కొనసాగు;
		}
		ఆపు;
	    }
	    ఒకవేళ (అంకెనా(*bp))
		వెళ్ళు బహిర్గతం;
	    ఎంపిక (i)
	    {
	    స్థితి 2:
		offs *= SECSPERHOUR;
		ఆపు;
	    స్థితి 4:
		i = offs % 100;
		offs /= 100;
		ఒకవేళ (i >= SECSPERMIN)
		    వెళ్ళు బహిర్గతం;
		/* Convert minutes into decimal */
		offs = offs * SECSPERHOUR + i * SECSPERMIN;
		ఆపు;
	    అప్రమేయ:
బహిర్గతం:
		ఒకవేళ (mandatory)
		    తిరిగి ఖాళీ;
		bp = zname;
		కొనసాగు;
	    }
	    /* ISO 8601 & RFC 3339 limit to 23:59 max */
	    ఒకవేళ (offs >= (HOURSPERDAY * SECSPERHOUR))
		వెళ్ళు బహిర్గతం;
	    ఒకవేళ (neg)
		offs = -offs;
	    tm->tm_isdst = 0;	/* XXX */
	    కొనసాగు;

	/*
	 * Miscellaneous conversions.
	 */
	స్థితి 'n':	/* Any kind of white-space. */
	స్థితి 't':
	    అయితే (ఖాళీనా(*bp))
		bp++;
	    LEGAL_ALT(0);
	    కొనసాగు;


	అప్రమేయ:	/* Unknown/unsupported conversion. */
	    తిరిగి ఖాళీ;
	}
    }

    ఒకవేళ (!HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	ఒకవేళ (HAVE_MON(state) && HAVE_MDAY(state))
	{
	    /* calculate day of year (ordinal date) */
	    tm->tm_yday =  ప్రారంభం_యొక్క_month[isleap_కూడిక(tm->tm_year,
			    TM_YEAR_పునాది)][tm->tm_mon] + (tm->tm_mday - 1);
	    state |= S_YDAY;
	}
	లేకపోతే ఒకవేళ (day_స్థానభ్రంశం != -1)
	{
	    /*
	     * Set the date to the first Sunday (or Monday)
	     * of the specified week of the year.
	     */
	    ఒకవేళ (!HAVE_WDAY(state))
	    {
		tm->tm_wday = day_స్థానభ్రంశం;
		state |= S_WDAY;
	    }
	    tm->tm_yday = (7 -
		    మొదటి_wday_యొక్క(tm->tm_year + TM_YEAR_పునాది) +
		    day_స్థానభ్రంశం) % 7 + (week_స్థానభ్రంశం - 1) * 7 +
		    tm->tm_wday  - day_స్థానభ్రంశం;
	    state |= S_YDAY;
	}
    }

    ఒకవేళ (HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	పూర్ణాంకం leap;

	ఒకవేళ (!HAVE_MON(state))
	{
	    /* calculate month of day of year */
	    i = 0;
	    leap = isleap_కూడిక(tm->tm_year, TM_YEAR_పునాది);
	    అయితే (tm->tm_yday >= ప్రారంభం_యొక్క_month[leap][i])
		i++;
	    ఒకవేళ (i > 12)
	    {
		i = 1;
		tm->tm_yday -= ప్రారంభం_యొక్క_month[leap][12];
		tm->tm_year++;
	    }
	    tm->tm_mon = i - 1;
	    state |= S_MON;
	}

	ఒకవేళ (!HAVE_MDAY(state))
	{
	    /* calculate day of month */
	    leap = isleap_కూడిక(tm->tm_year, TM_YEAR_పునాది);
	    tm->tm_mday = tm->tm_yday -
		ప్రారంభం_యొక్క_month[leap][tm->tm_mon] + 1;
	    state |= S_MDAY;
	}

	ఒకవేళ (!HAVE_WDAY(state))
	{
	    /* calculate day of week */
	    i = 0;
	    week_స్థానభ్రంశం = మొదటి_wday_యొక్క(tm->tm_year);
	    అయితే (i++ <= tm->tm_yday)
	    {
		ఒకవేళ (week_స్థానభ్రంశం++ >= 6)
		    week_స్థానభ్రంశం = 0;
	    }
	    tm->tm_wday = week_స్థానభ్రంశం;
	    state |= S_WDAY;
	}
    }

    తిరిగి (అక్షరం *)bp;
}


    స్థిర స్థిరరాశి గుర్తులేని అక్షరం *
conv_అంకె(స్థిరరాశి గుర్తులేని అక్షరం *buf, పూర్ణాంకం *dest, గుర్తులేని పూర్ణాంకం llim,
						    గుర్తులేని పూర్ణాంకం ulim)
{
    గుర్తులేని పూర్ణాంకం ఫలితం = 0;
    గుర్తులేని అక్షరం ch;

    /* The limit also determines the number of valid digits. */
    గుర్తులేని పూర్ణాంకం rulim = ulim;

    ch = *buf;
    ఒకవేళ (ch < '0' || ch > '9')
	తిరిగి ఖాళీ;

    చేయి
    {
	ఫలితం *= 10;
	ఫలితం += ch - '0';
	rulim /= 10;
	ch = *++buf;
    } అయితే ((ఫలితం * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

    ఒకవేళ (ఫలితం < llim || ఫలితం > ulim)
	తిరిగి ఖాళీ;

    *dest = ఫలితం;
    తిరిగి buf;
}

    స్థిర స్థిరరాశి గుర్తులేని అక్షరం *
కనుగొను_string(స్థిరరాశి గుర్తులేని అక్షరం *bp, పూర్ణాంకం *tgt, స్థిరరాశి అక్షరం *స్థిరరాశి *n1,
					    స్థిరరాశి అక్షరం *స్థిరరాశి *n2, పూర్ణాంకం c)
{
    పూర్ణాంకం		i;
    size_t	len;

    /* check full name - then abbreviated ones */
    కొరకు (; n1 != ఖాళీ; n1 = n2, n2 = ఖాళీ)
    {
	కొరకు (i = 0; i < c; i++, n1++)
	{
	    len = అక్షరశ్రేణి_పొడవు(*n1);
	    ఒకవేళ (STRNICMP(*n1, bp, len) == 0)
	    {
		*tgt = i;
		తిరిగి bp + len;
	    }
	}
    }

    /* Nothing matched */
    తిరిగి ఖాళీ;
}

#అయితే_ముగింపు // MSWIN
