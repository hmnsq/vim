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

#ضمن "vim.h"

#اگر_تعریف_شده MSWIN

/* Constants lifted from NetBSD's <tzfile.h>. */
#تعریف_کن TM_YEAR_پایه	1900
#تعریف_کن TM_SUNDAY	0
#تعریف_کن TM_MONDAY	1
#تعریف_کن SECSPERHOUR	3600
#تعریف_کن SECSPERMIN	60
#تعریف_کن HOURSPERDAY	24

#تعریف_کن isleap(y)	(((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)
/* isleap_sum avoids integer overflow when adding the 1900 base. */
#تعریف_کن isleap_جمع(a, b) \
			isleap(((بیعلامت)(a) + (بیعلامت)(b)) % 400)

/*
 * _get_tzname() is provided by the UCRT and by the MSVC CRT.  Older MinGW
 * builds linking against msvcrt.dll fall back to the deprecated tzname[]
 * global (mingw-w64 already exposes the POSIX-named tzname[]).
 */
#إذا defined(_UCRT) || defined(_MSC_VER)
# define USE_GET_TZNAME
#پایان_اگر

/* Locale tables (English / "C" locale). */
ساكن ثابت حرف *ثابت c_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
ساكن ثابت حرف *ثابت c_abday[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
ساكن ثابت حرف *ثابت c_mon[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
ساكن ثابت حرف *ثابت c_abmon[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
ساكن ثابت حرف *ثابت c_am_pm[] = { "AM", "PM" };
ساكن ثابت حرف c_d_t_fmt[] = "%a %b %e %H:%M:%S %Y";
ساكن ثابت حرف c_d_fmt[] = "%m/%d/%y";
ساكن ثابت حرف c_t_fmt[] = "%H:%M:%S";
ساكن ثابت حرف c_t_fmt_ampm[] = "%I:%M:%S %p";

ساكن ثابت بیعلامت حرف *conv_رقم(ثابت بیعلامت حرف *, صحيح *,
						بیعلامت صحيح, بیعلامت صحيح);
ساكن ثابت بیعلامت حرف *پیدا_کن_string(ثابت بیعلامت حرف *, صحيح *,
			ثابت حرف * ثابت *, ثابت حرف * ثابت *, صحيح);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#تعریف_کن ALT_E			0x01
#تعریف_کن ALT_O			0x02
#تعریف_کن LEGAL_ALT(x)		{ إذا (alt_format & ~(x)) أرجع عدم; }

#تعریف_کن S_YEAR			(1 << 0)
#تعریف_کن S_MON			(1 << 1)
#تعریف_کن S_YDAY			(1 << 2)
#تعریف_کن S_MDAY			(1 << 3)
#تعریف_کن S_WDAY			(1 << 4)
#تعریف_کن S_HOUR			(1 << 5)

#تعریف_کن HAVE_MDAY(s)		(s & S_MDAY)
#تعریف_کن HAVE_MON(s)		(s & S_MON)
#تعریف_کن HAVE_WDAY(s)		(s & S_WDAY)
#تعریف_کن HAVE_YDAY(s)		(s & S_YDAY)
#تعریف_کن HAVE_YEAR(s)		(s & S_YEAR)
#تعریف_کن HAVE_HOUR(s)		(s & S_HOUR)

/* RFC-822/RFC-2822 */
ساكن ثابت حرف *ثابت nast[5] = {
    "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
ساكن ثابت حرف *ثابت nadt[5] = {
    "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};

/*
 * Table to determine the ordinal date for the start of a month.
 * Ref: http://en.wikipedia.org/wiki/ISO_week_date
 */
ساكن ثابت صحيح شروع_از_month[2][13] = {
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
    ساكن صحيح
اولی_wday_از(صحيح yr)
{
    أرجع ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
	    (isleap(yr) ? 6 : 0) + 1) % 7;
}

#تعریف_کن delim(p)	((p) == '\0' || آیا_فاصله((بیعلامت حرف)(p)))

/*
 * Stub replacing NetBSD's fromzone(): the Windows CRT does not provide
 * tzalloc() / tzgetgmtoff() and the associated IANA tzfile loader, so
 * we simply decline to recognize arbitrary timezone names.  The %Z / %z
 * numeric and RFC-822/2822 forms are still handled by the caller.
 */
    ساكن صحيح
fromzone(ثابت بیعلامت حرف **bp استفادهنشده, بنية tm *tm استفادهنشده,
						    صحيح mandatory استفادهنشده)
{
    أرجع 0;
}

    حرف *
strptime(ثابت حرف *buf, ثابت حرف *fmt, بنية tm *tm)
{
    بیعلامت حرف c;
    ثابت بیعلامت حرف *bp, *ep, *zname;
    صحيح alt_format, i, تقسیم_year = 0, neg = 0, state = 0,
	day_جابه_جایی = -1, week_جابه_جایی = 0, offs, mandatory;
    ثابت حرف *new_fmt;

    bp = (ثابت بیعلامت حرف *)buf;

    طالما (bp != عدم && (c = *fmt++) != '\0')
    {
	/* Clear `alternate' modifier prior to new conversion. */
	alt_format = 0;
	i = 0;

	/* Eat up white-space. */
	إذا (آیا_فاصله(c))
	{
	    طالما (آیا_فاصله(*bp))
		bp++;
	    استمر;
	}

	إذا (c != '%')
	    اذهب literal;


again:	بدل (c = *fmt++)
	{
	حالة '%':	/* "%%" is converted to "%". */
literal:
	    إذا (c != *bp++)
		أرجع عدم;
	    LEGAL_ALT(0);
	    استمر;

	/*
	 * "Alternative" modifiers. Just set the appropriate flag
	 * and start over again.
	 */
	حالة 'E':	/* "%E?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_E;
	    اذهب again;

	حالة 'O':	/* "%O?" alternative conversion modifier. */
	    LEGAL_ALT(0);
	    alt_format |= ALT_O;
	    اذهب again;

	/*
	 * "Complex" conversion rules, implemented through recursion.
	 */
	حالة 'c':	/* Date and time, using the locale's format. */
	    new_fmt = c_d_t_fmt;
	    state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
	    اذهب recurse;

	حالة 'D':	/* The date as "%m/%d/%y". */
	    new_fmt = "%m/%d/%y";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    اذهب recurse;

	حالة 'F':	/* The date as "%Y-%m-%d". */
	    new_fmt = "%Y-%m-%d";
	    LEGAL_ALT(0);
	    state |= S_MON | S_MDAY | S_YEAR;
	    اذهب recurse;

	حالة 'R':	/* The time as "%H:%M". */
	    new_fmt = "%H:%M";
	    LEGAL_ALT(0);
	    اذهب recurse;

	حالة 'r':	/* The time in 12-hour clock representation. */
	    new_fmt = c_t_fmt_ampm;
	    LEGAL_ALT(0);
	    اذهب recurse;

	حالة 'T':	/* The time as "%H:%M:%S". */
	    new_fmt = "%H:%M:%S";
	    LEGAL_ALT(0);
	    اذهب recurse;

	حالة 'X':	/* The time, using the locale's format. */
	    new_fmt = c_t_fmt;
	    اذهب recurse;

	حالة 'x':	/* The date, using the locale's format. */
	    new_fmt = c_d_fmt;
	    state |= S_MON | S_MDAY | S_YEAR;
	recurse:
	    bp = (ثابت بیعلامت حرف *)strptime((ثابت حرف *)bp,
							new_fmt, tm);
	    LEGAL_ALT(ALT_E);
	    استمر;

	/*
	 * "Elementary" conversion rules.
	 */
	حالة 'A':	/* The day of week, using the locale's form. */
	حالة 'a':
	    bp = پیدا_کن_string(bp, &tm->tm_wday, c_day, c_abday, 7);
	    LEGAL_ALT(0);
	    state |= S_WDAY;
	    استمر;

	حالة 'B':	/* The month, using the locale's form. */
	حالة 'b':
	حالة 'h':
	    bp = پیدا_کن_string(bp, &tm->tm_mon, c_mon, c_abmon, 12);
	    LEGAL_ALT(0);
	    state |= S_MON;
	    استمر;

	حالة 'C':	/* The century number. */
	    i = 20;
	    bp = conv_رقم(bp, &i, 0, 99);

	    i = i * 100 - TM_YEAR_پایه;
	    إذا (تقسیم_year)
		i += tm->tm_year % 100;
	    تقسیم_year = 1;
	    tm->tm_year = i;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    استمر;

	حالة 'd':	/* The day of month. */
	حالة 'e':
	    bp = conv_رقم(bp, &tm->tm_mday, 1, 31);
	    LEGAL_ALT(ALT_O);
	    state |= S_MDAY;
	    استمر;

	حالة 'k':	/* The hour (24-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	حالة 'H':
	    bp = conv_رقم(bp, &tm->tm_hour, 0, 23);
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    استمر;

	حالة 'l':	/* The hour (12-hour clock representation). */
	    LEGAL_ALT(0);
	    /* FALLTHROUGH */
	حالة 'I':
	    bp = conv_رقم(bp, &tm->tm_hour, 1, 12);
	    إذا (tm->tm_hour == 12)
		tm->tm_hour = 0;
	    LEGAL_ALT(ALT_O);
	    state |= S_HOUR;
	    استمر;

	حالة 'j':	/* The day of year. */
	    i = 1;
	    bp = conv_رقم(bp, &i, 1, 366);
	    tm->tm_yday = i - 1;
	    LEGAL_ALT(0);
	    state |= S_YDAY;
	    استمر;

	حالة 'M':	/* The minute. */
	    bp = conv_رقم(bp, &tm->tm_min, 0, 59);
	    LEGAL_ALT(ALT_O);
	    استمر;

	حالة 'm':	/* The month. */
	    i = 1;
	    bp = conv_رقم(bp, &i, 1, 12);
	    tm->tm_mon = i - 1;
	    LEGAL_ALT(ALT_O);
	    state |= S_MON;
	    استمر;

	حالة 'p':	/* The locale's equivalent of AM/PM. */
	    bp = پیدا_کن_string(bp, &i, c_am_pm, عدم, 2);
	    إذا (HAVE_HOUR(state) && tm->tm_hour > 11)
		أرجع عدم;
	    tm->tm_hour += i * 12;
	    LEGAL_ALT(0);
	    استمر;

	حالة 'S':	/* The seconds. */
	    bp = conv_رقم(bp, &tm->tm_sec, 0, 61);
	    LEGAL_ALT(ALT_O);
	    استمر;

	حالة 's':	/* seconds since the epoch */
	    {
		ثابت time_t زمان_MAX = (time_t)((حجم(time_t) == 8)
					? LLONG_MAX : بیشترین_صحیح);
		time_t sse, d;

		إذا (*bp < '0' || *bp > '9')
		{
		    bp = عدم;
		    استمر;
		}

		sse = *bp++ - '0';
		طالما (*bp >= '0' && *bp <= '9')
		{
		    d = *bp++ - '0';
		    إذا (sse > زمان_MAX / 10)
		    {
			bp = عدم;
			اكسر;
		    }
		    sse *= 10;
		    إذا (sse > زمان_MAX - d)
		    {
			bp = عدم;
			اكسر;
		    }
		    sse += d;
		}
		إذا (bp == عدم)
		    استمر;

		{
		    بنية tm *lt = localtime(&sse);

		    إذا (lt == عدم)
			bp = عدم;
		    وإلا
		    {
			*tm = *lt;
			state |= S_YDAY | S_WDAY |
				S_MON | S_MDAY | S_YEAR;
		    }
		}
		استمر;
	    }

	حالة 'U':	/* The week of year, beginning on sunday. */
	حالة 'W':	/* The week of year, beginning on monday. */
	    /*
	     * This is bogus, as we can not assume any valid
	     * information present in the tm structure at this
	     * point to calculate a real value, so save the
	     * week for now in case it can be used later.
	     */
	    bp = conv_رقم(bp, &i, 0, 53);
	    LEGAL_ALT(ALT_O);
	    إذا (c == 'U')
		day_جابه_جایی = TM_SUNDAY;
	    وإلا
		day_جابه_جایی = TM_MONDAY;
	    week_جابه_جایی = i;
	    استمر;

	حالة 'w':	/* The day of week, beginning on sunday. */
	    bp = conv_رقم(bp, &tm->tm_wday, 0, 6);
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    استمر;

	حالة 'u':	/* The day of week, monday = 1. */
	    bp = conv_رقم(bp, &i, 1, 7);
	    tm->tm_wday = i % 7;
	    LEGAL_ALT(ALT_O);
	    state |= S_WDAY;
	    استمر;

	حالة 'g':	/* ISO week year without century (parsed, ignored). */
	    bp = conv_رقم(bp, &i, 0, 99);
	    استمر;

	حالة 'G':	/* ISO week year with century (parsed, ignored). */
	    افعل
		bp++;
	    طالما (آیا_رقم(*bp));
	    استمر;

	حالة 'V':	/* ISO 8601:1988 week number (parsed, ignored). */
	    bp = conv_رقم(bp, &i, 1, 53);
	    استمر;

	حالة 'Y':	/* The year. */
	    i = TM_YEAR_پایه;	/* just for data sanity... */
	    bp = conv_رقم(bp, &i, 0, 9999);
	    tm->tm_year = i - TM_YEAR_پایه;
	    LEGAL_ALT(ALT_E);
	    state |= S_YEAR;
	    استمر;

	حالة 'y':	/* The year within 100 years of the epoch. */
	    /* LEGAL_ALT(ALT_E | ALT_O); */
	    bp = conv_رقم(bp, &i, 0, 99);

	    إذا (تقسیم_year)
		/* preserve century */
		i += (tm->tm_year / 100) * 100;
	    وإلا
	    {
		تقسیم_year = 1;
		إذا (i <= 68)
		    i = i + 2000 - TM_YEAR_پایه;
		وإلا
		    i = i + 1900 - TM_YEAR_پایه;
	    }
	    tm->tm_year = i;
	    state |= S_YEAR;
	    استمر;

	حالة 'Z':
	حالة 'z':
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
	    إذا (mandatory)
		طالما (آیا_فاصله(*bp))
		    bp++;

	    zname = bp;
	    بدل (*bp++)
	    {
	    حالة 'G':
		إذا (*bp++ != 'M')
		    اذهب namedzone;
		/*FALLTHROUGH*/
	    حالة 'U':
		إذا (*bp++ != 'T')
		    اذهب namedzone;
		وإلا إذا (!delim(*bp) && *bp++ != 'C')
		    اذهب namedzone;
		/*FALLTHROUGH*/
	    حالة 'Z':
		إذا (!delim(*bp))
		    اذهب namedzone;
		tm->tm_isdst = 0;
		استمر;
	    حالة '+':
		neg = 0;
		اكسر;
	    حالة '-':
		neg = 1;
		اكسر;
	    افتراضي:
namedzone:
		bp = zname;

		/* Nautical / Military style */
		إذا (delim(bp[1]) &&
			((*bp >= 'A' && *bp <= 'I') ||
			 (*bp >= 'L' && *bp <= 'Y')))
		{
		    bp++;
		    استمر;
		}
		/* 'J' is local time */
		إذا (delim(bp[1]) && *bp == 'J')
		{
		    bp++;
		    استمر;
		}

		/*
		 * From our 3 letter hard-coded table.
		 */
		إذا (delim(bp[0]) || delim(bp[1]) ||
			delim(bp[2]) || !delim(bp[3]))
		    اذهب loadzone;
		ep = پیدا_کن_string(bp, &i, nast, عدم, 4);
		إذا (ep != عدم)
		{
		    bp = ep;
		    استمر;
		}
		ep = پیدا_کن_string(bp, &i, nadt, عدم, 4);
		إذا (ep != عدم)
		{
		    tm->tm_isdst = 1;
		    bp = ep;
		    استمر;
		}
		/*
		 * Our current timezone.  Prefer _get_tzname() over the
		 * tzname[] global, which UCRT marks deprecated because it
		 * may be inaccurate after locale changes.
		 */
#اگر_تعریف_شده USE_GET_TZNAME
		{
		    حرف tzbuf[2][32];
		    ثابت حرف *tznames[2] = { tzbuf[0], tzbuf[1] };
		    size_t tzlen;

		    _tzset();
		    إذا (_get_tzname(&tzlen, tzbuf[0], حجم(tzbuf[0]), 0) != 0)
			tzbuf[0][0] = NUL;
		    إذا (_get_tzname(&tzlen, tzbuf[1], حجم(tzbuf[1]), 1) != 0)
			tzbuf[1][0] = NUL;
		    ep = پیدا_کن_string(bp, &i, tznames, عدم, 2);
		}
#وإلا
		ep = پیدا_کن_string(bp, &i,
			(ثابت حرف *ثابت *)tzname, عدم, 2);
#پایان_اگر
		إذا (ep != عدم)
		{
		    tm->tm_isdst = i;
		    bp = ep;
		    استمر;
		}
loadzone:
		/*
		 * The hard way, load the zone!
		 */
		إذا (fromzone(&bp, tm, mandatory))
		    استمر;
		اذهب بیرون;
	    }
	    offs = 0;
	    لكل (i = 0; i < 4; )
	    {
		إذا (آیا_رقم(*bp))
		{
		    offs = offs * 10 + (*bp++ - '0');
		    i++;
		    استمر;
		}
		إذا (i == 2 && *bp == ':')
		{
		    bp++;
		    استمر;
		}
		اكسر;
	    }
	    إذا (آیا_رقم(*bp))
		اذهب بیرون;
	    بدل (i)
	    {
	    حالة 2:
		offs *= SECSPERHOUR;
		اكسر;
	    حالة 4:
		i = offs % 100;
		offs /= 100;
		إذا (i >= SECSPERMIN)
		    اذهب بیرون;
		/* Convert minutes into decimal */
		offs = offs * SECSPERHOUR + i * SECSPERMIN;
		اكسر;
	    افتراضي:
بیرون:
		إذا (mandatory)
		    أرجع عدم;
		bp = zname;
		استمر;
	    }
	    /* ISO 8601 & RFC 3339 limit to 23:59 max */
	    إذا (offs >= (HOURSPERDAY * SECSPERHOUR))
		اذهب بیرون;
	    إذا (neg)
		offs = -offs;
	    tm->tm_isdst = 0;	/* XXX */
	    استمر;

	/*
	 * Miscellaneous conversions.
	 */
	حالة 'n':	/* Any kind of white-space. */
	حالة 't':
	    طالما (آیا_فاصله(*bp))
		bp++;
	    LEGAL_ALT(0);
	    استمر;


	افتراضي:	/* Unknown/unsupported conversion. */
	    أرجع عدم;
	}
    }

    إذا (!HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	إذا (HAVE_MON(state) && HAVE_MDAY(state))
	{
	    /* calculate day of year (ordinal date) */
	    tm->tm_yday =  شروع_از_month[isleap_جمع(tm->tm_year,
			    TM_YEAR_پایه)][tm->tm_mon] + (tm->tm_mday - 1);
	    state |= S_YDAY;
	}
	وإلا إذا (day_جابه_جایی != -1)
	{
	    /*
	     * Set the date to the first Sunday (or Monday)
	     * of the specified week of the year.
	     */
	    إذا (!HAVE_WDAY(state))
	    {
		tm->tm_wday = day_جابه_جایی;
		state |= S_WDAY;
	    }
	    tm->tm_yday = (7 -
		    اولی_wday_از(tm->tm_year + TM_YEAR_پایه) +
		    day_جابه_جایی) % 7 + (week_جابه_جایی - 1) * 7 +
		    tm->tm_wday  - day_جابه_جایی;
	    state |= S_YDAY;
	}
    }

    إذا (HAVE_YDAY(state) && HAVE_YEAR(state))
    {
	صحيح leap;

	إذا (!HAVE_MON(state))
	{
	    /* calculate month of day of year */
	    i = 0;
	    leap = isleap_جمع(tm->tm_year, TM_YEAR_پایه);
	    طالما (tm->tm_yday >= شروع_از_month[leap][i])
		i++;
	    إذا (i > 12)
	    {
		i = 1;
		tm->tm_yday -= شروع_از_month[leap][12];
		tm->tm_year++;
	    }
	    tm->tm_mon = i - 1;
	    state |= S_MON;
	}

	إذا (!HAVE_MDAY(state))
	{
	    /* calculate day of month */
	    leap = isleap_جمع(tm->tm_year, TM_YEAR_پایه);
	    tm->tm_mday = tm->tm_yday -
		شروع_از_month[leap][tm->tm_mon] + 1;
	    state |= S_MDAY;
	}

	إذا (!HAVE_WDAY(state))
	{
	    /* calculate day of week */
	    i = 0;
	    week_جابه_جایی = اولی_wday_از(tm->tm_year);
	    طالما (i++ <= tm->tm_yday)
	    {
		إذا (week_جابه_جایی++ >= 6)
		    week_جابه_جایی = 0;
	    }
	    tm->tm_wday = week_جابه_جایی;
	    state |= S_WDAY;
	}
    }

    أرجع (حرف *)bp;
}


    ساكن ثابت بیعلامت حرف *
conv_رقم(ثابت بیعلامت حرف *buf, صحيح *dest, بیعلامت صحيح llim,
						    بیعلامت صحيح ulim)
{
    بیعلامت صحيح نتیجه = 0;
    بیعلامت حرف ch;

    /* The limit also determines the number of valid digits. */
    بیعلامت صحيح rulim = ulim;

    ch = *buf;
    إذا (ch < '0' || ch > '9')
	أرجع عدم;

    افعل
    {
	نتیجه *= 10;
	نتیجه += ch - '0';
	rulim /= 10;
	ch = *++buf;
    } طالما ((نتیجه * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');

    إذا (نتیجه < llim || نتیجه > ulim)
	أرجع عدم;

    *dest = نتیجه;
    أرجع buf;
}

    ساكن ثابت بیعلامت حرف *
پیدا_کن_string(ثابت بیعلامت حرف *bp, صحيح *tgt, ثابت حرف *ثابت *n1,
					    ثابت حرف *ثابت *n2, صحيح c)
{
    صحيح		i;
    size_t	len;

    /* check full name - then abbreviated ones */
    لكل (; n1 != عدم; n1 = n2, n2 = عدم)
    {
	لكل (i = 0; i < c; i++, n1++)
	{
	    len = طول_رشته(*n1);
	    إذا (STRNICMP(*n1, bp, len) == 0)
	    {
		*tgt = i;
		أرجع bp + len;
	    }
	}
    }

    /* Nothing matched */
    أرجع عدم;
}

#پایان_اگر // MSWIN
