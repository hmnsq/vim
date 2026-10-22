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
			isleap(((अचिन्हित)(a) + (अचिन्हित)(b)) % 400)

/*
 * _get_tzname() is provided by the UCRT and by the MSVC CRT.  Older MinGW
 * builds linking against msvcrt.dll fall back to the deprecated tzname[]
 * global (mingw-w64 already exposes the POSIX-named tzname[]).
 */
#जर defined(_UCRT) || defined(_MSC_VER)
# define USE_GET_TZNAME
#endif

/* Locale tables (English / "C" locale). */
स्थिर स्थिरांक अक्षर *स्थिरांक c_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
स्थिर स्थिरांक अक्षर *स्थिरांक c_abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
स्थिर स्थिरांक अक्षर *स्थिरांक c_mon[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
स्थिर स्थिरांक अक्षर *स्थिरांक c_abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
स्थिर स्थिरांक अक्षर *स्थिरांक c_am_pm[] = { "AM", "PM" };
स्थिर स्थिरांक अक्षर c_d_t_fmt[] = "%a %b %e %H:%M:%S %Y";
स्थिर स्थिरांक अक्षर c_d_fmt[] = "%m/%d/%y";
स्थिर स्थिरांक अक्षर c_t_fmt[] = "%H:%M:%S";
स्थिर स्थिरांक अक्षर c_t_fmt_ampm[] = "%I:%M:%S %p";

स्थिर स्थिरांक अचिन्हित अक्षर *conv_अंक(स्थिरांक अचिन्हित अक्षर *, पूर्णांक *,
						अचिन्हित पूर्णांक, अचिन्हित पूर्णांक);
स्थिर स्थिरांक अचिन्हित अक्षर *find_string(स्थिरांक अचिन्हित अक्षर *, पूर्णांक *,
			स्थिरांक अक्षर * स्थिरांक *, स्थिरांक अक्षर * स्थिरांक *, पूर्णांक);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E			0x01
#define ALT_O			0x02
#define LEGAL_ALT(x)		{ जर (alt_format & ~(x)) परत रिक्त; }

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
स्थिर स्थिरांक अक्षर *स्थिरांक nast[5] = {
    "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
स्थिर स्थिरांक अक्षर *स्थिरांक nadt[5] = {
    "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};

/*
 * Table to determine the ordinal date for the start of a month.
 * Ref: http://en.wikipedia.org/wiki/ISO_week_date
 */
स्थिर स्थिरांक पूर्णांक start_of_month[2][13] = {
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
    स्थिर पूर्णांक
first_wday_of(पूर्णांक yr)
{
    परत ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
	    (isleap(yr) ? 6 : 0) + 1) % 7;
}

#define delim(p)	((p) == '\0' || है_खाली((अचिन्हित अक्षर)(p)))

/*
 * Stub replacing NetBSD's fromzone(): the Windows CRT does not provide
 * tzalloc() / tzgetgmtoff() and the associated IANA tzfile loader, so
 * we simply decline to recognize arbitrary timezone names.  The %Z / %z
 * numeric and RFC-822/2822 forms are still handled by the caller.
 */
    स्थिर पूर्णांक
fromzone(स्थिरांक अचिन्हित अक्षर **bp UNUSED, संरचना पंचांग *पंचांग UNUSED,
						    पूर्णांक mandatory UNUSED)
{
    परत 0;
}

    अक्षर *
strptime(स्थिरांक अक्षर *buf, स्थिरांक अक्षर *fmt, संरचना पंचांग *पंचांग)
{
    अचिन्हित अक्षर c;
    स्थिरांक अचिन्हित अक्षर *bp, *ep, *zname;
    पूर्णांक alt_format, i, split_year = 0, neg = 0, state = 0,
	day_offset = -1, week_offset = 0, offs, mandatory;
    स्थिरांक अक्षर *new_fmt;

    bp = (स्थिरांक अचिन्हित अक्षर *)buf;

    पर्यंत (bp != रिक्त && (c = *fmt++) != '\0')
    {
	/* Clear `alternate' modifier prior to new conversion. */
	alt_format = 0;
	i = 0;

	/* Eat up white-space. */
	जर (है_खाली(c))
	{
	    पर्यंत (है_खाली(*bp))
		bp++;
	    सुरूठेवा;
	}

	जर (c != '%')
	    जा literal;


again:	निवड (c = *fmt++)
	{
	स्थिती '%':	/* "%%" is converted to "%". */
literal:
	    जर (c != *bp++)
		परत रिक्त;
	    LEGAL_ALT(0);
	    सुरूठेवा;

	/*
	 * "Alternative" modifiers. Just set the appropriate flag
	 * and start over again.
	 */
	स्थिती 'E':	/* "%E?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_E;
	    जा again;

	स्थिती 'O':	/* "%O?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_O;
	    जा again;

	/*
	 * "Complex" conversion rules, implemented through recursion.
	 */
	स्थिती 'c':	/* Date and time, using the locale's format. */
	    new_fmt = c_d_t_fmt;
	    state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
	    जा recurse;

	स्थिती 'D':	/* The date as "%m/%d/%y". */
	    new_fmt = "%m/%d/%y";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    जा recurse;

	स्थिती 'F':	/* The date as "%Y-%m-%d". */
	    new_fmt = "%Y-%m-%d";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    जा recurse;

	स्थिती 'R':	/* The time as "%H:%M". */
	    new_fmt = "%H:%M";
	    LEGAL_ALT(0);
	    जा recurse;

	स्थिती 'r':	/* The time in 12-hour clock representation. */
	    new_fmt = c_t_fmt_ampm;
	    LEGAL_ALT(0);
	    जा recurse;

	स्थिती 'T':	/* The time as "%H:%M:%S". */
	    new_fmt = "%H:%M:%S";
	    LEGAL_ALT(0);
	    जा recurse;

	स्थिती 'X':	/* The time, using the locale's format. */
	    new_fmt = c_t_fmt;
	    जा recurse;

	स्थिती 'x':	/* The date, using the locale's format. */
	    new_fmt = c_d_fmt;
	    state |= S_MON | S_MDAY | S_YEAR;
	recurse:
	    bp = (स्थिरांक अचिन्हित अक्षर *)strptime((स्थिरांक अक्षर *)bp,
							new_fmt, पंचांग);
	    LEGAL_ALT(ALT_E);
	    सुरूठेवा;

	/*
	 * "Elementary" conversion rules.
	 */
	स्थिती 'A':	/* The day of week, using the locale's form. */
	स्थिती 'a':
	    bp = find_string(bp, &पंचांग->tm_wday, c_day, c_abday, 7);
	    LEGAL_ALT(0);
	    state |= S_WDAY;
	    सुरूठेवा;

	स्थिती 'B':	/* The month, using the locale's form. */
	स्थिती 'b':
	स्थिती 'h':
	    bp = find_string(bp, &पंचांग->tm_mon, c_mon, c_abmon, 12);
	    LEGAL_ALT(0);
	    state |= S_MON;
	    सुरूठेवा;

	स्थिती 'C':	/* The century number. */
	    i = 20;
	    bp = conv_अंक(bp, &i, 0, 99);

	    i = i * 100 - TM_YEAR_BASE;
	    जर (split_year)
		i += पंचांग->tm_year % 100;
	    split_year = 1;
	    पंचांग->tm_year = i;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    सुरूठेवा;

	स्थिती 'd':	/* The day of month. */
	स्थिती 'e':
	    bp = conv_अंक(bp, &पंचांग->tm_mday, 1, 31);
	    LEGAL_ALT(ALT_O);
	    state |= S_MDAY;
	    सुरूठेवा;

	स्थिती 'k':	/* The hour (24-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	स्थिती 'H':
	    bp = conv_अंक(bp, &पंचांग->tm_hour, 0, 23);
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    सुरूठेवा;

	स्थिती 'l':	/* The hour (12-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	स्थिती 'I':
	    bp = conv_अंक(bp, &पंचांग->tm_hour, 1, 12);
	    जर (पंचांग->tm_hour == 12)
		पंचांग->tm_hour = 0;
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    सुरूठेवा;

	स्थिती 'j':	/* The day of year. */
	    i = 1;
	    bp = conv_अंक(bp, &i, 1, 366);
	    पंचांग->tm_yday = i - 1;
	    LEGAL_ALT(0);
	    state |= S_YDAY;
	    सुरूठेवा;

	स्थिती 'M':	/* The minute. */
	    bp = conv_अंक(bp, &पंचांग->tm_min, 0, 59);
	    LEGAL_ALT(ALT_O);
	    सुरूठेवा;

	स्थिती 'm':	/* The month. */
	    i = 1;
	    bp = conv_अंक(bp, &i, 1, 12);
	    पंचांग->tm_mon = i - 1;
	    LEGAL_ALT(ALT_O);
	    state |= S_MON;
	    सुरूठेवा;

	स्थिती 'p':	/* The locale's equivalent of AM/PM. */
	    bp = find_string(bp, &i, c_am_pm, रिक्त, 2);
	    जर (HAVE_HOUR(state) && पंचांग->tm_hour > 11)
		परत रिक्त;
	    पंचांग->tm_hour += i * 12;
	    LEGAL_ALT(0);
	    सुरूठेवा;

	स्थिती 'S':	/* The seconds. */
	    bp = conv_अंक(bp, &पंचांग->tm_sec, 0, 61);
	    LEGAL_ALT(ALT_O);
	    सुरूठेवा;

	स्थिती 's':	/* seconds since the epoch */
	    {
		स्थिरांक समय_प्रकार TIME_MAX = (समय_प्रकार)((आकार(समय_प्रकार) == 8)
					? LLONG_MAX : पूर्णांक_उच्च);
		समय_प्रकार sse, d;

		जर (*bp < '0' || *bp > '9')
		{
		    bp = रिक्त;
		    सुरूठेवा;
		}

		sse = *bp++ - '0';
		पर्यंत (*bp >= '0' && *bp <= '9')
		{
		    d = *bp++ - '0';
		    जर (sse > TIME_MAX / 10)
		    {
			bp = रिक्त;
			थांबा;
		    }
		    sse *= 10;
		    जर (sse > TIME_MAX - d)
		    {
			bp = रिक्त;
			थांबा;
		    }
		    sse += d;
		}
		जर (bp == रिक्त)
		    सुरूठेवा;

		{
		    संरचना पंचांग *lt = स_स्थानीय(&sse);

		    जर (lt == रिक्त)
			bp = रिक्त;
		    अन्यथा
		    {
			*पंचांग = *lt;
			state |= S_YDAY | S_WDAY |
				S_MON | S_MDAY | S_YEAR;
		    }
		}
		सुरूठेवा;
	    }

	स्थिती 'U':	/* The week of year, beginning on sunday. */
	स्थिती 'W':	/* The week of year, beginning on monday. */
	    /*
	     * This is bogus, as we can not assume any valid
	     * information present in the tm structure at this
	     * point to calculate a real value, so save the
	     * week for now in case it can be used later.
	     */
	    bp = conv_अंक(bp, &i, 0, 53);
	    LEGAL_ALT(ALT_O);
	    जर (c == 'U')
		day_offset = TM_SUNDAY;
	    अन्यथा
		day_offset = TM_MONDAY;
	    week_offset = i;
	    सुरूठेवा;

	स्थिती 'w':	/* The day of week, beginning on sunday. */
	    bp = conv_अंक(bp, &पंचांग->tm_wday, 0, 6);
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    सुरूठेवा;

	स्थिती 'u':	/* The day of week, monday = 1. */
	    bp = conv_अंक(bp, &i, 1, 7);
	    पंचांग->tm_wday = i % 7;
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    सुरूठेवा;

	स्थिती 'g':	/* ISO week year without century (parsed, ignored). */
	    bp = conv_अंक(bp, &i, 0, 99);
	    सुरूठेवा;

	स्थिती 'G':	/* ISO week year with century (parsed, ignored). */
	    करा
		bp++;
	    पर्यंत (है_अंक(*bp));
	    सुरूठेवा;

	स्थिती 'V':	/* ISO 8601:1988 week number (parsed, ignored). */
	    bp = conv_अंक(bp, &i, 1, 53);
	    सुरूठेवा;

	स्थिती 'Y':	/* The year. */
	    i = TM_YEAR_BASE;	/* just for data sanity... */
	    bp = conv_अंक(bp, &i, 0, 9999);
	    पंचांग->tm_year = i - TM_YEAR_BASE;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    सुरूठेवा;

	स्थिती 'y':	/* The year within 100 years of the epoch. */
	    /* LEGAL_ALT(ALT_E | ALT_O); */
	    bp = conv_अंक(bp, &i, 0, 99);

	    जर (split_year)
		/* preserve century */
		i += (पंचांग->tm_year / 100) * 100;
	    अन्यथा
	    {
		split_year = 1;
		जर (i <= 68)
		    i = i + 2000 - TM_YEAR_BASE;
		अन्यथा
		    i = i + 1900 - TM_YEAR_BASE;
	    }
	    पंचांग->tm_year = i;
	    state |= S_YEAR;
	    सुरूठेवा;

	स्थिती 'Z':
	स्थिती 'z':
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
	    जर (mandatory)
		पर्यंत (है_खाली(*bp))
		    bp++;

	    zname = bp;
	    निवड (*bp++)
	    {
	    स्थिती 'G':
		जर (*bp++ != 'M')
		    जा namedzone;
		/*FALLTHROUGH*/
	    स्थिती 'U':
		जर (*bp++ != 'T')
		    जा namedzone;
		अन्यथा जर (!delim(*bp) && *bp++ != 'C')
		    जा namedzone;
		/*FALLTHROUGH*/
	    स्थिती 'Z':
		जर (!delim(*bp))
		    जा namedzone;
		पंचांग->tm_isdst = 0;
		सुरूठेवा;
	    स्थिती '+':
		neg = 0;
		थांबा;
	    स्थिती '-':
		neg = 1;
		थांबा;
	    पूर्वनिर्धारित:
namedzone:
		bp = zname;

		/* Nautical / Military style */
		जर (delim(bp[1]) &&
			((*bp >= 'A' && *bp <= 'I') ||
			 (*bp >= 'L' && *bp <= 'Y')))
		{
		    bp++;
		    सुरूठेवा;
		}
		/* 'J' is local time */
		जर (delim(bp[1]) && *bp == 'J')
		{
		    bp++;
		    सुरूठेवा;
		}

		/*
		 * From our 3 letter hard-coded table.
		 */
		जर (delim(bp[0]) || delim(bp[1]) ||
			delim(bp[2]) || !delim(bp[3]))
		    जा loadzone;
		ep = find_string(bp, &i, nast, रिक्त, 4);
		जर (ep != रिक्त)
		{
		    bp = ep;
		    सुरूठेवा;
		}
		ep = find_string(bp, &i, nadt, रिक्त, 4);
		जर (ep != रिक्त)
		{
		    पंचांग->tm_isdst = 1;
		    bp = ep;
		    सुरूठेवा;
		}
		/*
		 * Our current timezone.  Prefer _get_tzname() over the
		 * tzname[] global, which UCRT marks deprecated because it
		 * may be inaccurate after locale changes.
		 */
#ifdef USE_GET_TZNAME
		{
		    अक्षर tzbuf[2][32];
		    स्थिरांक अक्षर *tznames[2] = { tzbuf[0], tzbuf[1] };
		    माप_प्रकार tzlen;

		    _tzset();
		    जर (_get_tzname(&tzlen, tzbuf[0], आकार(tzbuf[0]), 0) != 0)
			tzbuf[0][0] = NUL;
		    जर (_get_tzname(&tzlen, tzbuf[1], आकार(tzbuf[1]), 1) != 0)
			tzbuf[1][0] = NUL;
		    ep = find_string(bp, &i, tznames, रिक्त, 2);
		}
#अन्यथा
		ep = find_string(bp, &i,
			(स्थिरांक अक्षर *स्थिरांक *)tzname, रिक्त, 2);
#endif
		जर (ep != रिक्त)
		{
		    पंचांग->tm_isdst = i;
		    bp = ep;
		    सुरूठेवा;
		}
loadzone:
		/*
		 * The hard way, load the zone!
		 */
		जर (fromzone(&bp, पंचांग, mandatory))
		    सुरूठेवा;
		जा out;
	    }
	    offs = 0;
	    साठी (i = 0; i < 4; )
	    {
		जर (है_अंक(*bp))
		{
		    offs = offs * 10 + (*bp++ - '0');
		    i++;
		    सुरूठेवा;
		}
		जर (i == 2 && *bp == ':')
		{
		    bp++;
		    सुरूठेवा;
		}
		थांबा;
	    }
	    जर (है_अंक(*bp))
		जा out;
	    निवड (i)
	    {
	    स्थिती 2:
		offs *= SECSPERHOUR;
		थांबा;
	    स्थिती 4:
		i = offs % 100;
		offs /= 100;
		जर (i >= SECSPERMIN)
		    जा out;
		/* Convert minutes into decimal */
		offs = offs * SECSPERHOUR + i * SECSPERMIN;
		थांबा;
	    पूर्वनिर्धारित:
out:
		जर (mandatory)
		    परत रिक्त;
		bp = zname;
		सुरूठेवा;
	    }
	    /* ISO 8601 & RFC 3339 limit to 23:59 max */
	    जर (offs >= (HOURSPERDAY * SECSPERHOUR))
		जा out;
	    जर (neg)
		offs = -offs;
	    पंचांग->tm_isdst = 0;	/* XXX */
	    सुरूठेवा;

	/*
	 * Miscellaneous conversions.
	 */
	स्थिती 'n':	/* Any kind of white-space. */
	स्थिती 't':
	    पर्यंत (है_खाली(*bp))
		bp++;
	    LEGAL_ALT(0);
	    सुरूठेवा;


	पूर्वनिर्धारित:	/* Unknown/unsupported conversion. */
	    परत रिक्त;
	}
    }

    जर (!HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	जर (HAVE_MON(state) && HAVE_MDAY(state))
	{
	    /* calculate day of year (ordinal date) */
	    पंचांग->tm_yday =  start_of_month[isleap_sum(पंचांग->tm_year,
			    TM_YEAR_BASE)][पंचांग->tm_mon] + (पंचांग->tm_mday - 1);
	    state |= S_YDAY;
	}
	अन्यथा जर (day_offset != -1)
	{
	    /*
	     * Set the date to the first Sunday (or Monday)
	     * of the specified week of the year.
	     */
	    जर (!HAVE_WDAY(state))
	    {
		पंचांग->tm_wday = day_offset;
		state |= S_WDAY;
	    }
	    पंचांग->tm_yday = (7 -
		    first_wday_of(पंचांग->tm_year + TM_YEAR_BASE) +
		    day_offset) % 7 + (week_offset - 1) * 7 +
		    पंचांग->tm_wday  - day_offset;
	    state |= S_YDAY;
	}
    }

    जर (HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	पूर्णांक leap;

	जर (!HAVE_MON(state))
	{
	    /* calculate month of day of year */
	    i = 0;
	    leap = isleap_sum(पंचांग->tm_year, TM_YEAR_BASE);
	    पर्यंत (पंचांग->tm_yday >= start_of_month[leap][i])
		i++;
	    जर (i > 12)
	    {
		i = 1;
		पंचांग->tm_yday -= start_of_month[leap][12];
		पंचांग->tm_year++;
	    }
	    पंचांग->tm_mon = i - 1;
	    state |= S_MON;
	}

	जर (!HAVE_MDAY(state))
	{
	    /* calculate day of month */
	    leap = isleap_sum(पंचांग->tm_year, TM_YEAR_BASE);
	    पंचांग->tm_mday = पंचांग->tm_yday -
		start_of_month[leap][पंचांग->tm_mon] + 1;
	    state |= S_MDAY;
	}

	जर (!HAVE_WDAY(state))
	{
	    /* calculate day of week */
	    i = 0;
	    week_offset = first_wday_of(पंचांग->tm_year);
	    पर्यंत (i++ <= पंचांग->tm_yday)
	    {
		जर (week_offset++ >= 6)
		    week_offset = 0;
	    }
	    पंचांग->tm_wday = week_offset;
	    state |= S_WDAY;
	}
    }

    परत (अक्षर *)bp;
}


    स्थिर स्थिरांक अचिन्हित अक्षर *
conv_अंक(स्थिरांक अचिन्हित अक्षर *buf, पूर्णांक *dest, अचिन्हित पूर्णांक llim,
						    अचिन्हित पूर्णांक ulim)
{
    अचिन्हित पूर्णांक result = 0;
    अचिन्हित अक्षर ch;

    /* The limit also determines the number of valid digits. */
    अचिन्हित पूर्णांक rulim = ulim;

    ch = *buf;
    जर (ch < '0' || ch > '9')
	परत रिक्त;

    करा
    {
	result *= 10;
	result += ch - '0';
	rulim /= 10;
	ch = *++buf;
    } पर्यंत ((result * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

    जर (result < llim || result > ulim)
	परत रिक्त;

    *dest = result;
    परत buf;
}

    स्थिर स्थिरांक अचिन्हित अक्षर *
find_string(स्थिरांक अचिन्हित अक्षर *bp, पूर्णांक *tgt, स्थिरांक अक्षर *स्थिरांक *n1,
					    स्थिरांक अक्षर *स्थिरांक *n2, पूर्णांक c)
{
    पूर्णांक		i;
    माप_प्रकार	len;

    /* check full name - then abbreviated ones */
    साठी (; n1 != रिक्त; n1 = n2, n2 = रिक्त)
    {
	साठी (i = 0; i < c; i++, n1++)
	{
	    len = म_माप(*n1);
	    जर (STRNICMP(*n1, bp, len) == 0)
	    {
		*tgt = i;
		परत bp + len;
	    }
	}
    }

    /* Nothing matched */
    परत रिक्त;
}

#endif // MSWIN
