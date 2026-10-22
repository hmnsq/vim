/* vi:set ts=8 sw=8 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * NetBeans Debugging Tools. What are these tools and why are they important?
 * There are two main tools here. The first tool is a tool for delaying or
 * stopping gvim during startup.  The second tool is a protocol log tool.
 *
 * The startup delay tool is called nbdebug_wait(). This is very important for
 * debugging startup problems because gvim will be started automatically from
 * netbeans and cannot be run directly from a debugger. The only way to debug
 * a gvim started by netbeans is by attaching a debugger to it. Without this
 * tool all startup code will have completed before you can get the pid and
 * attach.
 *
 * The second tool is a log tool.
 *
 * This code must have NBDEBUG defined for it to be compiled into vim/gvim.
 */

#جيڪڏهن_وصف_ٿيل NBDEBUG

#ضمن "vim.h"

FILE		*nb_debug = عدم;
u_int		 nb_dlevel = 0;		// nb_debug verbosity level

فراغ		 nbdb(حرف *, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

ساكن صحيح	 lookup(حرف *);
#جيڪڏهن_وصف_ٿيل USE_NB_ERRORHANDLER
ساكن صحيح	 errorHandler(Display *, XErrorEvent *);
#جيڪڏهن_پورو

/*
 * nbdebug_wait	-   This function can be used to delay or stop execution of vim.
 *		    It's normally used to delay startup while attaching a
 *		    debugger to a running process. Since NetBeans starts gvim
 *		    from a background process this is the only way to debug
 *		    startup problems.
 */
	فراغ
nbdebug_wait(
	u_int		 wait_flags,	// tells what to do
	حرف		*wait_var,	// wait environment variable
	u_int		 wait_secs)	// how many seconds to wait
{

	init_homedir();			// not inited yet
#جيڪڏهن_وصف_ٿيل USE_WDDUMP
	WDDump(0, 0, 0);
#جيڪڏهن_پورو

	// for debugging purposes only
	إذا (wait_flags & WT_ENV && wait_var && getenv(wait_var) != عدم)
	{
		sleep(atoi(getenv(wait_var)));
	}
	وإلا إذا (wait_flags & WT_WAIT && lookup("~/.gvimwait"))
	{
		sleep(wait_secs > 0 && wait_secs < 120 ? wait_secs : 20);
	}
	وإلا إذا (wait_flags & WT_STOP && lookup("~/.gvimstop"))
	{
		صحيح w = 1;
		طالما (w)
		{
			;
		}
	}
}

	فراغ
nbdebug_log_init(
	حرف		*log_var,	// env var with log file
	حرف		*level_var)	// env var with nb_debug level
{
	حرف		*file;		// possible nb_debug output file
	حرف		*cp;		// nb_dlevel pointer

	إذا (log_var && (file = getenv(log_var)) != عدم)
	{
		time_t now;

		nb_debug = fopen(file, "a");
		وقت(&now);
		fprintf(nb_debug, "%s", get_ctime(now, TRUE));
		إذا (level_var && (cp = getenv(level_var)) != عدم)
		{
			nb_dlevel = strtoul(cp, عدم, 0);
		}
		وإلا
		{
			nb_dlevel = NB_TRACE;	// default level
		}
#جيڪڏهن_وصف_ٿيل USE_NB_ERRORHANDLER
		XSetErrorHandler(errorHandler);
#جيڪڏهن_پورو
	}

}

	فراغ
nbdbg(حرف *fmt, ...)
{
	va_list		 ap;

	إذا (nb_debug != عدم && nb_dlevel & NB_TRACE)
	{
		va_start(ap, fmt);
		vfprintf(nb_debug, fmt, ap);
		va_end(ap);
		fflush(nb_debug);
	}

}

	ساكن صحيح
lookup(حرف *file)
{
	حرف		 buf[BUFSIZ];

	expand_env((char_u *) file, (char_u *) buf, BUFSIZ);
	أرجع
#جيڪڏهن_وصف_نه_ٿيل FEAT_GUI_MSWIN
		(access(buf, F_OK) == 0);
#وإلا
		(access(buf, 0) == 0);
#جيڪڏهن_پورو

}

#جيڪڏهن_وصف_ٿيل USE_NB_ERRORHANDLER
	ساكن صحيح
errorHandler(
	Display		*dpy,
	XErrorEvent	*err)
{
	حرف		 نياپو[256];
	حرف		 buf[256];

	XGet_Error_متن(dpy, err->error_ڪوڊ, نياپو, حجم(نياپو));
	nbdbg("\n\nNBDEBUG Vim: X Error of failed request: %s\n", نياپو);

	sprintf(buf, "%d", err->request_ڪوڊ);
	XGet_Error_Database_متن(dpy,
	    "XRequest", buf, "Unknown", نياپو, حجم(نياپو));
	nbdbg("\tMajor opcode of failed request: %d (%s)\n",
	    err->request_ڪوڊ, نياپو);
	إذا (err->request_ڪوڊ > 128)
	{
		nbdbg("\tMinor opcode of failed request: %d\n",
		    err->minor_ڪوڊ);
	}

	أرجع 0;
}
#جيڪڏهن_پورو


#جيڪڏهن_پورو // NBDEBUG
