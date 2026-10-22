/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * autocmd.c: Autocommand related functions
 */

#অন্তর্ভাব "vim.h"

/*
 * The autocommands are stored in a list for each event.
 * Autocommands for the same pattern, that are consecutive, are joined
 * together, to avoid having to match the pattern too often.
 * The result is an array of Autopat lists, which point to AutoCmd lists:
 *
 * last_autopat[0]  -----------------------------+
 *						 V
 * first_autopat[0] --> Autopat.next  -->  Autopat.next -->  NULL
 *			Autopat.cmds	   Autopat.cmds
 *			    |			 |
 *			    V			 V
 *			AutoCmd.next	   AutoCmd.next
 *			    |			 |
 *			    V			 V
 *			AutoCmd.next		NULL
 *			    |
 *			    V
 *			   NULL
 *
 * last_autopat[1]  --------+
 *			    V
 * first_autopat[1] --> Autopat.next  -->  NULL
 *			Autopat.cmds
 *			    |
 *			    V
 *			AutoCmd.next
 *			    |
 *			    V
 *			   NULL
 *   etc.
 *
 *   The order of AutoCmds is important, this is the order in which they were
 *   defined and will have to be executed.
 */
প্রকারলেখ কাঠামো AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    অক্ষর	    once;		// "One shot": removed after execution
    অক্ষর	    nested;		// If autocommands nest here.
    অক্ষর	    last;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    কাঠামো AutoCmd  *পরবর্তী;		// next AutoCmd in list
} AutoCmd;

প্রকারলেখ কাঠামো AutoPat
{
    কাঠামো AutoPat  *পরবর্তী;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    পূর্ণসংখ্যা		    group;		// group ID
    পূর্ণসংখ্যা		    patlen;		// strlen() of pat
    পূর্ণসংখ্যা		    buflocal_nr;	// !=0 for buffer-local AutoPat
    অক্ষর	    allow_dirs;		// Pattern may match whole path
    অক্ষর	    last;		// last pattern for apply_autocmds()
} AutoPat;

//
// special cases:
// BufNewFile and BufRead are searched for ALOT (especially at startup)
// so we pre-determine their index into the event_tab[] table for fast access.
// Keep these values in sync with event_tab[]!
#define BUFNEWFILE_সূচক 9
#define BUFREAD_সূচক 10

// Must be sorted by the 'value' field because it is used by bsearch()!
// Events with positive keys aren't allowed in 'eventignorewin'.
স্থির keyvalue_T event_tab[অঙ্ক_EVENTS] = {
    KEYVALUE_ENTRY(-EVENT_BUFADD, "BufAdd"),
    KEYVALUE_ENTRY(-EVENT_BUFADD, "BufCreate"),
    KEYVALUE_ENTRY(-EVENT_BUFDELETE, "BufDelete"),
    KEYVALUE_ENTRY(-EVENT_BUFENTER, "BufEnter"),
    KEYVALUE_ENTRY(-EVENT_BUFFILEPOST, "BufFilePost"),
    KEYVALUE_ENTRY(-EVENT_BUFFILEPRE, "BufFilePre"),
    KEYVALUE_ENTRY(-EVENT_BUFHIDDEN, "BufHidden"),
    KEYVALUE_ENTRY(-EVENT_BUFLEAVE, "BufLeave"),
    KEYVALUE_ENTRY(-EVENT_BUFNEW, "BufNew"),
    KEYVALUE_ENTRY(-EVENT_BUFNEWFILE, "BufNewFile"),	// BUFNEWFILE_INDEX
    KEYVALUE_ENTRY(-EVENT_BUFREADPOST, "BufRead"),	// BUFREAD_INDEX
    KEYVALUE_ENTRY(-EVENT_BUFREADCMD, "BufReadCmd"),
    KEYVALUE_ENTRY(-EVENT_BUFREADPOST, "BufReadPost"),
    KEYVALUE_ENTRY(-EVENT_BUFREADPRE, "BufReadPre"),
    KEYVALUE_ENTRY(-EVENT_BUFUNLOAD, "BufUnload"),
    KEYVALUE_ENTRY(-EVENT_BUFWINENTER, "BufWinEnter"),
    KEYVALUE_ENTRY(-EVENT_BUFWINLEAVE, "BufWinLeave"),
    KEYVALUE_ENTRY(-EVENT_BUFWIPEOUT, "BufWipeout"),
    KEYVALUE_ENTRY(-EVENT_BUFWRITEPRE, "BufWrite"),
    KEYVALUE_ENTRY(-EVENT_BUFWRITECMD, "BufWriteCmd"),
    KEYVALUE_ENTRY(-EVENT_BUFWRITEPOST, "BufWritePost"),
    KEYVALUE_ENTRY(-EVENT_BUFWRITEPRE, "BufWritePre"),
    KEYVALUE_ENTRY(EVENT_CMDLINECHANGED, "CmdlineChanged"),
    KEYVALUE_ENTRY(EVENT_CMDLINEENTER, "CmdlineEnter"),
    KEYVALUE_ENTRY(EVENT_CMDLINELEAVE, "CmdlineLeave"),
    KEYVALUE_ENTRY(EVENT_CMDLINELEAVEPRE, "CmdlineLeavePre"),
    KEYVALUE_ENTRY(EVENT_CMDUNDEFINED, "CmdUndefined"),
    KEYVALUE_ENTRY(EVENT_CMDWINENTER, "CmdwinEnter"),
    KEYVALUE_ENTRY(EVENT_CMDWINLEAVE, "CmdwinLeave"),
    KEYVALUE_ENTRY(EVENT_COLORSCHEME, "ColorScheme"),
    KEYVALUE_ENTRY(EVENT_COLORSCHEMEPRE, "ColorSchemePre"),
    KEYVALUE_ENTRY(EVENT_COMPLETECHANGED, "CompleteChanged"),
    KEYVALUE_ENTRY(EVENT_COMPLETEDONE, "CompleteDone"),
    KEYVALUE_ENTRY(EVENT_COMPLETEDONEPRE, "CompleteDonePre"),
    KEYVALUE_ENTRY(-EVENT_CURSORHOLD, "CursorHold"),
    KEYVALUE_ENTRY(-EVENT_CURSORHOLDI, "CursorHoldI"),
    KEYVALUE_ENTRY(-EVENT_CURSORMOVED, "CursorMoved"),
    KEYVALUE_ENTRY(-EVENT_CURSORMOVEDC, "CursorMovedC"),
    KEYVALUE_ENTRY(-EVENT_CURSORMOVEDI, "CursorMovedI"),
    KEYVALUE_ENTRY(EVENT_DIFFUPDATED, "DiffUpdated"),
    KEYVALUE_ENTRY(EVENT_DIRCHANGED, "DirChanged"),
    KEYVALUE_ENTRY(EVENT_DIRCHANGEDPRE, "DirChangedPre"),
    KEYVALUE_ENTRY(EVENT_ENCODINGCHANGED, "EncodingChanged"),
    KEYVALUE_ENTRY(EVENT_EXITPRE, "ExitPre"),
    KEYVALUE_ENTRY(-EVENT_FILEAPPENDCMD, "FileAppendCmd"),
    KEYVALUE_ENTRY(-EVENT_FILEAPPENDPOST, "FileAppendPost"),
    KEYVALUE_ENTRY(-EVENT_FILEAPPENDPRE, "FileAppendPre"),
    KEYVALUE_ENTRY(-EVENT_FILECHANGEDRO, "FileChangedRO"),
    KEYVALUE_ENTRY(-EVENT_FILECHANGEDSHELL, "FileChangedShell"),
    KEYVALUE_ENTRY(-EVENT_FILECHANGEDSHELLPOST, "FileChangedShellPost"),
    KEYVALUE_ENTRY(EVENT_ENCODINGCHANGED, "FileEncoding"),
    KEYVALUE_ENTRY(-EVENT_FILEREADCMD, "FileReadCmd"),
    KEYVALUE_ENTRY(-EVENT_FILEREADPOST, "FileReadPost"),
    KEYVALUE_ENTRY(-EVENT_FILEREADPRE, "FileReadPre"),
    KEYVALUE_ENTRY(-EVENT_FILETYPE, "FileType"),
    KEYVALUE_ENTRY(-EVENT_FILEWRITECMD, "FileWriteCmd"),
    KEYVALUE_ENTRY(-EVENT_FILEWRITEPOST, "FileWritePost"),
    KEYVALUE_ENTRY(-EVENT_FILEWRITEPRE, "FileWritePre"),
    KEYVALUE_ENTRY(-EVENT_FILTERREADPOST, "FilterReadPost"),
    KEYVALUE_ENTRY(-EVENT_FILTERREADPRE, "FilterReadPre"),
    KEYVALUE_ENTRY(-EVENT_FILTERWRITEPOST, "FilterWritePost"),
    KEYVALUE_ENTRY(-EVENT_FILTERWRITEPRE, "FilterWritePre"),
    KEYVALUE_ENTRY(EVENT_FOCUSGAINED, "FocusGained"),
    KEYVALUE_ENTRY(EVENT_FOCUSLOST, "FocusLost"),
    KEYVALUE_ENTRY(EVENT_FUNCUNDEFINED, "FuncUndefined"),
    KEYVALUE_ENTRY(EVENT_GUIENTER, "GUIEnter"),
    KEYVALUE_ENTRY(EVENT_GUIFAILED, "GUIFailed"),
    KEYVALUE_ENTRY(-EVENT_INSERTCHANGE, "InsertChange"),
    KEYVALUE_ENTRY(-EVENT_INSERTCHARPRE, "InsertCharPre"),
    KEYVALUE_ENTRY(-EVENT_INSERTENTER, "InsertEnter"),
    KEYVALUE_ENTRY(-EVENT_INSERTLEAVE, "InsertLeave"),
    KEYVALUE_ENTRY(-EVENT_INSERTLEAVEPRE, "InsertLeavePre"),
    KEYVALUE_ENTRY(EVENT_KEYINPUTPRE, "KeyInputPre"),
    KEYVALUE_ENTRY(EVENT_MENUPOPUP, "MenuPopup"),
    KEYVALUE_ENTRY(EVENT_MODECHANGED, "ModeChanged"),
    KEYVALUE_ENTRY(EVENT_OPTIONSET, "OptionSet"),
    KEYVALUE_ENTRY(EVENT_QUICKFIXCMDPOST, "QuickFixCmdPost"),
    KEYVALUE_ENTRY(EVENT_QUICKFIXCMDPRE, "QuickFixCmdPre"),
    KEYVALUE_ENTRY(EVENT_QUITPRE, "QuitPre"),
    KEYVALUE_ENTRY(EVENT_REMOTEREPLY, "RemoteReply"),
    KEYVALUE_ENTRY(EVENT_SAFESTATE, "SafeState"),
    KEYVALUE_ENTRY(EVENT_SAFESTATEAGAIN, "SafeStateAgain"),
    KEYVALUE_ENTRY(EVENT_SESSIONLOADPOST, "SessionLoadPost"),
    KEYVALUE_ENTRY(EVENT_SESSIONLOADPRE, "SessionLoadPre"),
    KEYVALUE_ENTRY(EVENT_SESSIONWRITEPOST, "SessionWritePost"),
    KEYVALUE_ENTRY(EVENT_SHELLCMDPOST, "ShellCmdPost"),
    KEYVALUE_ENTRY(-EVENT_SHELLFILTERPOST, "ShellFilterPost"),
    KEYVALUE_ENTRY(EVENT_SIGUSR1, "SigUSR1"),
    KEYVALUE_ENTRY(EVENT_SOURCECMD, "SourceCmd"),
    KEYVALUE_ENTRY(EVENT_SOURCEPOST, "SourcePost"),
    KEYVALUE_ENTRY(EVENT_SOURCEPRE, "SourcePre"),
    KEYVALUE_ENTRY(EVENT_SPELLFILEMISSING, "SpellFileMissing"),
    KEYVALUE_ENTRY(EVENT_STDINREADPOST, "StdinReadPost"),
    KEYVALUE_ENTRY(EVENT_STDINREADPRE, "StdinReadPre"),
    KEYVALUE_ENTRY(EVENT_SWAPEXISTS, "SwapExists"),
    KEYVALUE_ENTRY(EVENT_SYNTAX, "Syntax"),
    KEYVALUE_ENTRY(EVENT_TABCLOSED, "TabClosed"),
    KEYVALUE_ENTRY(EVENT_TABCLOSEDPRE, "TabClosedPre"),
    KEYVALUE_ENTRY(EVENT_TABENTER, "TabEnter"),
    KEYVALUE_ENTRY(EVENT_TABLEAVE, "TabLeave"),
    KEYVALUE_ENTRY(EVENT_TABNEW, "TabNew"),
    KEYVALUE_ENTRY(EVENT_TERMCHANGED, "TermChanged"),
    KEYVALUE_ENTRY(EVENT_TERMINALOPEN, "TerminalOpen"),
    KEYVALUE_ENTRY(EVENT_TERMINALWINOPEN, "TerminalWinOpen"),
    KEYVALUE_ENTRY(EVENT_TERMRESPONSE, "TermResponse"),
    KEYVALUE_ENTRY(EVENT_TERMRESPONSEALL, "TermResponseAll"),
    KEYVALUE_ENTRY(-EVENT_TEXTCHANGED, "TextChanged"),
    KEYVALUE_ENTRY(-EVENT_TEXTCHANGEDI, "TextChangedI"),
    KEYVALUE_ENTRY(-EVENT_TEXTCHANGEDP, "TextChangedP"),
    KEYVALUE_ENTRY(-EVENT_TEXTCHANGEDT, "TextChangedT"),
    KEYVALUE_ENTRY(-EVENT_TEXTPUTPOST, "TextPutPost"),
    KEYVALUE_ENTRY(-EVENT_TEXTPUTPRE, "TextPutPre"),
    KEYVALUE_ENTRY(-EVENT_TEXTYANKPOST, "TextYankPost"),
    KEYVALUE_ENTRY(EVENT_USER, "User"),
    KEYVALUE_ENTRY(EVENT_VIMENTER, "VimEnter"),
    KEYVALUE_ENTRY(EVENT_VIMLEAVE, "VimLeave"),
    KEYVALUE_ENTRY(EVENT_VIMLEAVEPRE, "VimLeavePre"),
    KEYVALUE_ENTRY(EVENT_VIMRESIZED, "VimResized"),
    KEYVALUE_ENTRY(EVENT_VIMRESUME, "VimResume"),
    KEYVALUE_ENTRY(EVENT_VIMSUSPEND, "VimSuspend"),
    KEYVALUE_ENTRY(-EVENT_WINCLOSED, "WinClosed"),
    KEYVALUE_ENTRY(-EVENT_WINENTER, "WinEnter"),
    KEYVALUE_ENTRY(-EVENT_WINLEAVE, "WinLeave"),
    KEYVALUE_ENTRY(EVENT_WINNEW, "WinNew"),
    KEYVALUE_ENTRY(EVENT_WINNEWPRE, "WinNewPre"),
    KEYVALUE_ENTRY(-EVENT_WINRESIZED, "WinResized"),
    KEYVALUE_ENTRY(-EVENT_WINSCROLLED, "WinScrolled"),
};

স্থির AutoPat *first_autopat[অঙ্ক_EVENTS] = { ফাঁকা };
স্থির AutoPat *last_autopat[অঙ্ক_EVENTS] = { ফাঁকা };

#define AUGROUP_DEFAULT    (-1)	    // default autocmd group
#define AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#define AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
কাঠামো AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    পূর্ণসংখ্যা		group;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*লেজ;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    পূর্ণসংখ্যা		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *পরবর্তী;		// chain of active apc-s for auto-invalidation
};

স্থির AutoPatCmd_T *active_apc_list = ফাঁকা; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#define FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    জন্য ((ap) = first_autopat[(পূর্ণসংখ্যা)(event)]; (ap) != ফাঁকা; (ap) = (ap)->পরবর্তী)

/*
 * augroups stores a list of autocmd group names.
 */
স্থির garray_T augroups = {0, 0, আকার(char_u *), 10, ফাঁকা};
#define AUGROUP_NAME(i) (((char_u **)augroups.ga_তথ্য)[i])
// use get_deleted_augroup() to get this
স্থির char_u *deleted_augroup = ফাঁকা;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
স্থির পূর্ণসংখ্যা current_augroup = AUGROUP_DEFAULT;

স্থির পূর্ণসংখ্যা au_need_clean = FALSE;   // need to delete marked patterns

স্থির event_T event_name2nr(char_u *start, char_u **end);
স্থির string_T *event_nr2name(event_T event);
স্থির পূর্ণসংখ্যা au_get_grouparg(char_u **argp);
স্থির পূর্ণসংখ্যা do_autocmd_event(event_T event, char_u *pat, পূর্ণসংখ্যা once, পূর্ণসংখ্যা nested, char_u *cmd, পূর্ণসংখ্যা forceit, পূর্ণসংখ্যা group, পূর্ণসংখ্যা flags);
স্থির পূর্ণসংখ্যা apply_autocmds_group(event_T event, char_u *fname, char_u *fname_io, পূর্ণসংখ্যা force, পূর্ণসংখ্যা group, buf_T *buf, exarg_T *eap);
স্থির শূন্য auto_পরবর্তী_pat(AutoPatCmd_T *apc, পূর্ণসংখ্যা stop_at_last);
স্থির পূর্ণসংখ্যা au_find_group(char_u *name);

স্থির event_T	last_event;
স্থির পূর্ণসংখ্যা	last_group;
স্থির পূর্ণসংখ্যা	autocmd_blocked = 0;	// block all autocmds

    স্থির char_u *
get_deleted_augroup(শূন্য)
{
    যদি (deleted_augroup == ফাঁকা)
	deleted_augroup = (char_u *)_("--Deleted--");
    ফেরত deleted_augroup;
}

/*
 * Show the autocommands for one AutoPat.
 */
    স্থির শূন্য
show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    // Check for "got_int" (here and at various places below), which is set
    // when "q" has been hit for the "--more--" prompt
    যদি (got_int)
	ফেরত;
    যদি (ap->pat == ফাঁকা)		// pattern has been removed
	ফেরত;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    বার্তা_putchar('\n');
    যদি (got_int)
	যাও theend;
    যদি (event != last_event || ap->group != last_group)
    {
	string_T    *event_name;

	যদি (ap->group != AUGROUP_DEFAULT)
	{
	    যদি (AUGROUP_NAME(ap->group) == ফাঁকা)
		বার্তা_puts_attr((অক্ষর *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    অন্যথা
		বার্তা_puts_attr((অক্ষর *)AUGROUP_NAME(ap->group), HL_ATTR(HLF_T));
	    বার্তা_puts("  ");
	}
	event_name = event_nr2name(event);
	বার্তা_puts_attr((অক্ষর *)event_name->string, HL_ATTR(HLF_T));
	last_event = event;
	last_group = ap->group;
	বার্তা_putchar('\n');
	যদি (got_int)
	    যাও theend;
    }

    যদি (ap->pat == ফাঁকা)
	যাও theend;  // timer might have cleared the pattern or group

    বার্তা_col = 4;
    বার্তা_outtrans(ap->pat);

    জন্য (ac = ap->cmds; ac != ফাঁকা; ac = ac->পরবর্তী)
    {
	যদি (ac->cmd == ফাঁকা)		// skip removed commands
	    চালু;

	যদি (বার্তা_col >= 14)
	    বার্তা_putchar('\n');
	বার্তা_col = 14;
	যদি (got_int)
	    যাও theend;
	বার্তা_outtrans(ac->cmd);
#ifdef FEAT_EVAL
	যদি (p_verbose > 0)
	    last_set_বার্তা(ac->script_ctx);
#endif
	যদি (got_int)
	    যাও theend;
	যদি (ac->পরবর্তী != ফাঁকা)
	{
	    বার্তা_putchar('\n');
	    যদি (got_int)
		যাও theend;
	}
    }

theend:
    --autocmd_busy;
}

/*
 * Mark an autocommand pattern for deletion.
 */
    স্থির শূন্য
au_remove_pat(AutoPat *ap)
{
    VIM_CLEAR(ap->pat);
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    স্থির শূন্য
au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    জন্য (ac = ap->cmds; ac != ফাঁকা; ac = ac->পরবর্তী)
	VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

// Delete one command from an autocmd pattern.
স্থির শূন্য au_del_cmd(AutoCmd *ac)
{
    VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    স্থির শূন্য
au_cleanup(শূন্য)
{
    AutoPat	*ap, **পূর্ব_ap;
    AutoCmd	*ac, **পূর্ব_ac;
    event_T	event;

    যদি (autocmd_busy || !au_need_clean)
	ফেরত;

    // loop over all events
    জন্য (event = (event_T)0; (পূর্ণসংখ্যা)event < অঙ্ক_EVENTS;
					    event = (event_T)((পূর্ণসংখ্যা)event + 1))
    {
	// loop over all autocommand patterns
	পূর্ব_ap = &(first_autopat[(পূর্ণসংখ্যা)event]);
	জন্য (ap = *পূর্ব_ap; ap != ফাঁকা; ap = *পূর্ব_ap)
	{
	    পূর্ণসংখ্যা has_cmd = FALSE;

	    // loop over all commands for this pattern
	    পূর্ব_ac = &(ap->cmds);
	    জন্য (ac = *পূর্ব_ac; ac != ফাঁকা; ac = *পূর্ব_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		যদি (ap->pat == ফাঁকা || ac->cmd == ফাঁকা)
		{
		    *পূর্ব_ac = ac->পরবর্তী;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		অন্যথা
		{
		    has_cmd = TRUE;
		    পূর্ব_ac = &(ac->পরবর্তী);
		}
	    }

	    যদি (ap->pat != ফাঁকা && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    যদি (ap->pat == ফাঁকা)
	    {
		যদি (ap->পরবর্তী == ফাঁকা)
		{
		    যদি (পূর্ব_ap == &(first_autopat[(পূর্ণসংখ্যা)event]))
			last_autopat[(পূর্ণসংখ্যা)event] = ফাঁকা;
		    অন্যথা
			// this depends on the "next" field being the first in
			// the struct
			last_autopat[(পূর্ণসংখ্যা)event] = (AutoPat *)পূর্ব_ap;
		}
		*পূর্ব_ap = ap->পরবর্তী;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    অন্যথা
		পূর্ব_ap = &(ap->পরবর্তী);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Called when buffer is freed, to remove/invalidate related buffer-local
 * autocmds.
 */
    শূন্য
aubuflocal_remove(buf_T *buf)
{
    AutoPat	    *ap;
    event_T	    event;
    AutoPatCmd_T    *apc;

    // invalidate currently executing autocommands
    জন্য (apc = active_apc_list; apc; apc = apc->পরবর্তী)
	যদি (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    জন্য (event = (event_T)0; (পূর্ণসংখ্যা)event < অঙ্ক_EVENTS;
					    event = (event_T)((পূর্ণসংখ্যা)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    যদি (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		যদি (p_verbose >= 6)
		{
		    string_T	*event_name;

		    verbose_enter();
		    event_name = event_nr2name(event);
		    smsg(_("auto-removing autocommand: %s <buffer=%d>"),
				       event_name->string, buf->b_fnum);
		    verbose_leave();
		}
	    }
    au_cleanup();
}

/*
 * Add an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    স্থির পূর্ণসংখ্যা
au_new_group(char_u *name)
{
    পূর্ণসংখ্যা		i;

    i = au_find_group(name);
    যদি (i != AUGROUP_ERROR)
	ফেরত i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    জন্য (i = 0; i < augroups.ga_len; ++i)
	যদি (AUGROUP_NAME(i) == ফাঁকা)
	    বিরতি;
    যদি (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	ফেরত AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    যদি (AUGROUP_NAME(i) == ফাঁকা)
	ফেরত AUGROUP_ERROR;
    যদি (i == augroups.ga_len)
	++augroups.ga_len;

    ফেরত i;
}

    স্থির শূন্য
au_del_group(char_u *name)
{
    পূর্ণসংখ্যা		i;
    event_T	event;
    AutoPat	*ap;
    পূর্ণসংখ্যা		in_use = FALSE;


    i = au_find_group(name);
    যদি (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_group_str), name);
	ফেরত;
    }
    যদি (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_group));
	ফেরত;
    }

    জন্য (event = (event_T)0; (পূর্ণসংখ্যা)event < অঙ্ক_EVENTS;
	    event = (event_T)((পূর্ণসংখ্যা)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    যদি (ap->group == i && ap->pat != ফাঁকা)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		in_use = TRUE;
		event = অঙ্ক_EVENTS;
		বিরতি;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    যদি (in_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    অন্যথা
	AUGROUP_NAME(i) = ফাঁকা;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    স্থির পূর্ণসংখ্যা
au_find_group(char_u *name)
{
    পূর্ণসংখ্যা	    i;

    জন্য (i = 0; i < augroups.ga_len; ++i)
	যদি (AUGROUP_NAME(i) != ফাঁকা && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    ফেরত i;
    ফেরত AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    পূর্ণসংখ্যা
au_has_group(char_u *name)
{
    ফেরত au_find_group(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    শূন্য
do_augroup(char_u *arg, পূর্ণসংখ্যা del_group)
{
    পূর্ণসংখ্যা	    i;

    যদি (del_group)
    {
	যদি (*arg == NUL)
	    emsg(_(e_argument_required));
	অন্যথা
	    au_del_group(arg);
    }
    অন্যথা যদি (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    অন্যথা যদি (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_group(arg);
	যদি (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    অন্যথা			    // ":aug": list the group names
    {
	বার্তা_start();
	জন্য (i = 0; i < augroups.ga_len; ++i)
	{
	    যদি (AUGROUP_NAME(i) != ফাঁকা)
	    {
		বার্তা_puts((অক্ষর *)AUGROUP_NAME(i));
		বার্তা_puts("  ");
	    }
	}
	বার্তা_clr_eos();
	বার্তা_end();
    }
}

    শূন্য
autocmd_init(শূন্য)
{
    CLEAR_FIELD(aucmd_win);
}

#যদি defined(EXITFREE)
    শূন্য
free_all_autocmds(শূন্য)
{
    char_u	*s;

    জন্য (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(ফাঁকা, (char_u *)"", TRUE);

    জন্য (পূর্ণসংখ্যা i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_তথ্য))[i];
	যদি (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#endif

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    পূর্ণসংখ্যা
is_aucmd_win(win_T *win)
{
    জন্য (পূর্ণসংখ্যা i = 0; i < AUCMD_WIN_গুনতি; ++i)
	যদি (aucmd_win[i].auc_win_used && aucmd_win[i].auc_win == win)
	    ফেরত TRUE;
    ফেরত FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    স্থির event_T
event_name2nr(char_u *start, char_u **end)
{
    char_u	*p;
    keyvalue_T target;
    keyvalue_T *entry;
    স্থির keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_সূচক];
    স্থির keyvalue_T *bufread = &event_tab[BUFREAD_সূচক];

    // the event name ends with end of line, '|', a blank or a comma
    জন্য (p = start; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    target.চাবি = 0;
    target.মান.string = start;
    target.মান.দৈর্ঘ্য = (size_t)(p - start);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    যদি (cmp_keyvalue_মান_ni(&target, bufnewfile) == 0)
	entry = bufnewfile;
    অন্যথা যদি (cmp_keyvalue_মান_ni(&target, bufread) == 0)
	entry = bufread;
    অন্যথা
	entry = (keyvalue_T *)bsearch(&target, &event_tab, অঙ্ক_EVENTS,
				আকার(event_tab[0]), cmp_keyvalue_মান_ni);

    যদি (*p == ',')
	++p;
    *end = p;

    ফেরত (entry == ফাঁকা) ? অঙ্ক_EVENTS : (event_T)abs(entry->চাবি);
}

/*
 * Return the name for event "event".
 */
    স্থির string_T *
event_nr2name(event_T event)
{
    পূর্ণসংখ্যা	    i;
    গণনা {CACHE_আকার = 12};
    স্থির পূর্ণসংখ্যা cache_tab[CACHE_আকার];
    স্থির পূর্ণসংখ্যা cache_last_সূচক = -1;
    স্থির string_T unknown = STR_LITERAL_INIT("Unknown");

    যদি (cache_last_সূচক < 0)
    {
	জন্য (i = 0; i < CACHE_আকার; ++i)
	    cache_tab[i] = -1;
	cache_last_সূচক = CACHE_আকার - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    জন্য (i = cache_last_সূচক; cache_tab[i] >= 0; )
    {
	যদি ((event_T)abs(event_tab[cache_tab[i]].চাবি) == event)
	    ফেরত &event_tab[cache_tab[i]].মান;

	যদি (i == 0)
	    i = CACHE_আকার - 1;
	অন্যথা
	    --i;

	// are we back at the start?
	যদি (i == cache_last_সূচক)
	    বিরতি;
    }

    // look in the event table itself
    জন্য (i = 0; i < অঙ্ক_EVENTS; ++i)
    {
	যদি ((event_T)abs(event_tab[i].চাবি) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    যদি (cache_last_সূচক == CACHE_আকার - 1)
		cache_last_সূচক = 0;
	    অন্যথা
		++cache_last_সূচক;
	    cache_tab[cache_last_সূচক] = i;
	    ফেরত &event_tab[i].মান;
	}
    }

    ফেরত &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    স্থির char_u *
find_end_event(
    char_u  *arg,
    পূর্ণসংখ্যা	    have_group)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    যদি (*arg == '*')
    {
	যদি (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_star_str), arg);
	    ফেরত ফাঁকা;
	}
	pat = arg + 1;
    }
    অন্যথা
    {
	জন্য (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    যদি ((পূর্ণসংখ্যা)event_name2nr(pat, &p) >= অঙ্ক_EVENTS)
	    {
		যদি (have_group)
		    semsg(_(e_no_such_event_str), pat);
		অন্যথা
		    semsg(_(e_no_such_group_or_event_str), pat);
		ফেরত ফাঁকা;
	    }
	}
    }
    ফেরত pat;
}

/*
 * Find the command of an ":autocmd" with argument "arg", the part after the
 * events, the pattern and any "++once"/"++nested".  Returns NULL when there is
 * no command.  Does not modify "arg" and gives no error messages.
 */
    char_u *
au_find_cmd_arg(char_u *arg)
{
    char_u	*pat;
    char_u	*cmd;
    পূর্ণসংখ্যা		group;

    যদি (*arg == '|')
	ফেরত ফাঁকা;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    group = au_get_grouparg(&arg);
    pat = group == AUGROUP_ERROR
			  ? ফাঁকা : find_end_event(arg, group != AUGROUP_ALL);
    --emsg_off;
    যদি (pat == ফাঁকা)
	ফেরত ফাঁকা;

    pat = skipwhite(pat);
    যদি (*pat == NUL || *pat == '|')
	ফেরত ফাঁকা;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    যখন (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    জন্য (পূর্ণসংখ্যা i = 0; i < 2; ++i)
    {
	যদি (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	যদি (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	যদি (!in_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    ফেরত *cmd == NUL ? ফাঁকা : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    পূর্ণসংখ্যা
event_ignored(event_T event, char_u *ei)
{
    পূর্ণসংখ্যা ignored = FALSE;
    যখন (*ei != NUL)
    {
	পূর্ণসংখ্যা unignore = *ei == '-';
	ei += unignore;
	যদি (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].চাবি <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	অন্যথা যদি (event_name2nr(ei, &ei) == event)
	{
	    যদি (unignore)
		ফেরত FALSE;
	    অন্যথা
		ignored = TRUE;
	}
    }

    ফেরত ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    পূর্ণসংখ্যা
check_ei(char_u *ei)
{
    পূর্ণসংখ্যা	win = ei != p_ei;

    যখন (*ei)
    {
	যদি (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	অন্যথা
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    যদি (event == অঙ্ক_EVENTS || (win && event_tab[event].চাবি > 0))
		ফেরত FAIL;
	}
    }

    ফেরত OK;
}

#যদি defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(অক্ষর *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    size_t	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    যদি (save_ei == ফাঁকা)
	ফেরত ফাঁকা;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    যদি (new_ei == ফাঁকা)
    {
	vim_free(save_ei);
	ফেরত ফাঁকা;
    }

    যদি (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    অন্যথা
	STRCPY(new_ei + p_ei_len, what);
    set_string_option_direct((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    ফেরত save_ei;
}

    শূন্য
au_event_restore(char_u *old_ei)
{
    যদি (old_ei != ফাঁকা)
    {
	set_string_option_direct((char_u *)"ei", -1, old_ei,
							  OPT_FREE, SID_NONE);
	vim_free(old_ei);
    }
}
#endif  // FEAT_SYN_HL

/*
 * do_autocmd() -- implements the :autocmd command.  Can be used in the
 *  following ways:
 *
 * :autocmd <event> <pat> <cmd>	    Add <cmd> to the list of commands that
 *				    will be automatically executed for <event>
 *				    when editing a file matching <pat>, in
 *				    the current group.
 * :autocmd <event> <pat>	    Show the autocommands associated with
 *				    <event> and <pat>.
 * :autocmd <event>		    Show the autocommands associated with
 *				    <event>.
 * :autocmd			    Show all autocommands.
 * :autocmd! <event> <pat> <cmd>    Remove all autocommands associated with
 *				    <event> and <pat>, and add the command
 *				    <cmd>, for the current group.
 * :autocmd! <event> <pat>	    Remove all autocommands associated with
 *				    <event> and <pat> for the current group.
 * :autocmd! <event>		    Remove all autocommands associated with
 *				    <event> for the current group.
 * :autocmd!			    Remove ALL autocommands for the current
 *				    group.
 *
 *  Multiple events and patterns may be given separated by commas.  Here are
 *  some examples:
 * :autocmd bufread,bufenter *.c,*.h	set tw=0 smartindent noic
 * :autocmd bufleave	     *		set tw=79 nosmartindent ic infercase
 *
 * :autocmd * *.c		show all autocommands for *.c files.
 *
 * Mostly a {group} argument can optionally appear before <event>.
 * "eap" can be NULL.
 */
    শূন্য
do_autocmd(exarg_T *eap, char_u *arg_in, পূর্ণসংখ্যা forceit)
{
    char_u	*arg = arg_in;
    char_u	*pat;
    char_u	*envpat = ফাঁকা;
    char_u	*cmd;
    পূর্ণসংখ্যা		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = ফাঁকা;
    পূর্ণসংখ্যা		nested = FALSE;
    পূর্ণসংখ্যা		once = FALSE;
    পূর্ণসংখ্যা		group;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		flags = 0;

    যদি (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	group = AUGROUP_ALL;	// no argument, use all groups
    }
    অন্যথা
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	group = au_get_grouparg(&arg);
	যদি (arg == ফাঁকা)	    // out of memory
	    ফেরত;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = find_end_event(arg, group != AUGROUP_ALL);
    যদি (pat == ফাঁকা)
	ফেরত;

    pat = skipwhite(pat);
    যদি (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    অন্যথা
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	যখন (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	যদি (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	যদি (vim_strchr(pat, '$') != ফাঁকা || vim_strchr(pat, '~') != ফাঁকা)
	{
#ifdef BACKSLASH_IN_FILENAME
	    পূর্ণসংখ্যা	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#endif
	    envpat = expand_env_save(pat);
#ifdef BACKSLASH_IN_FILENAME
	    p_ssl = p_ssl_save;
#endif
	    যদি (envpat != ফাঁকা)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	জন্য (i = 0; i < 2; i++)
	{
	    যদি (*cmd == NUL)
		চালু;

	    // Check for "++once" flag.
	    যদি (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		যদি (once)
		{
		    semsg(_(e_duplicate_argument_str), "++once");
		    যাও err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    যদি ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		যদি (nested)
		{
		    semsg(_(e_duplicate_argument_str), "++nested");
		    যাও err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    যদি (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		যদি (in_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_mean_plusplus_nested));
		    যাও err_exit;
		}
		যদি (nested)
		{
		    semsg(_(e_duplicate_argument_str), "nested");
		    যাও err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 6);
	    }
	}

	/*
	 * Find the start of the commands.
	 * Expand <sfile> in it.
	 */
	যদি (*cmd != NUL)
	{
	    যদি (eap != ফাঁকা)
		// Read a {} block if it follows.
		cmd = may_get_cmd_block(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    যদি (cmd == ফাঁকা)	    // some error
		যাও err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    যদি (!forceit && *cmd == NUL)
	// Highlight title
	বার্তা_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    last_event = (event_T)-1;		// for listing the event name
    last_group = AUGROUP_ERROR;		// for listing the group name
    যদি (*arg == '*' || *arg == NUL || *arg == '|')
    {
	যদি (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	অন্যথা
	    জন্য (event = (event_T)0; (পূর্ণসংখ্যা)event < অঙ্ক_EVENTS;
					     event = (event_T)((পূর্ণসংখ্যা)event + 1))
		যদি (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, group, flags) == FAIL)
		    বিরতি;
    }
    অন্যথা
    {
	যখন (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    যদি (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, group, flags) == FAIL)
		বিরতি;
    }

err_exit:
    যদি (cmd_need_free)
	vim_free(cmd);
    vim_free(tofree);
    vim_free(envpat);
}

/*
 * Find the group ID in a ":autocmd" or ":doautocmd" argument.
 * The "argp" argument is advanced to the following argument.
 *
 * Returns the group ID, AUGROUP_ERROR for error (out of memory).
 */
    স্থির পূর্ণসংখ্যা
au_get_grouparg(char_u **argp)
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    পূর্ণসংখ্যা		group = AUGROUP_ALL;

    জন্য (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    যদি (p <= arg)
	ফেরত AUGROUP_ALL;

    group_name = vim_strnsave(arg, p - arg);
    যদি (group_name == ফাঁকা)		// out of memory
	ফেরত AUGROUP_ERROR;
    group = au_find_group(group_name);
    যদি (group == AUGROUP_ERROR)
	group = AUGROUP_ALL;	// no match, use all groups
    অন্যথা
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(group_name);
    ফেরত group;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    স্থির পূর্ণসংখ্যা
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    পূর্ণসংখ্যা		once,
    পূর্ণসংখ্যা		nested,
    char_u	*cmd,
    পূর্ণসংখ্যা		forceit,
    পূর্ণসংখ্যা		group,
    পূর্ণসংখ্যা		flags)
{
    AutoPat	*ap;
    AutoPat	**পূর্ব_ap;
    AutoCmd	*ac;
    AutoCmd	**পূর্ব_ac;
    পূর্ণসংখ্যা		brace_level;
    char_u	*endpat;
    পূর্ণসংখ্যা		findgroup;
    পূর্ণসংখ্যা		allgroups;
    পূর্ণসংখ্যা		patlen;
    পূর্ণসংখ্যা		is_buflocal;
    পূর্ণসংখ্যা		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    যদি (group == AUGROUP_ALL)
	findgroup = current_augroup;
    অন্যথা
	findgroup = group;
    allgroups = (group == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    যদি (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    যদি (forceit)  // delete the AutoPat, if it's in the current group
	    {
		যদি (ap->group == findgroup)
		    au_remove_pat(ap);
	    }
	    অন্যথা যদি (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    জন্য ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	জন্য (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    যদি (*endpat == '{')
		brace_level++;
	    অন্যথা যদি (*endpat == '}')
		brace_level--;
	}
	যদি (pat == endpat)		// ignore single comma
	    চালু;
	patlen = (পূর্ণসংখ্যা)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	যদি (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    যদি (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    অন্যথা যদি (patlen > 9 && pat[7] == '=')
	    {
		যদি (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		অন্যথা যদি (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((অক্ষর *)pat + 8);
	    }
	}

	যদি (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((অক্ষর *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (পূর্ণসংখ্যা)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	যদি (!forceit && *cmd != NUL && last_autopat[(পূর্ণসংখ্যা)event] != ফাঁকা)
	    পূর্ব_ap = &last_autopat[(পূর্ণসংখ্যা)event];
	অন্যথা
	    পূর্ব_ap = &first_autopat[(পূর্ণসংখ্যা)event];
	যখন ((ap = *পূর্ব_ap) != ফাঁকা)
	{
	    যদি (ap->pat != ফাঁকা)
	    {
		/*
		 * Accept a pattern when:
		 * - a group was specified and it's that group, or a group was
		 *   not specified and it's the current group, or a group was
		 *   not specified and we are listing
		 * - the length of the pattern matches
		 * - the pattern matches.
		 * For <buffer[=X]>, this condition works because we normalize
		 * all buffer-local patterns.
		 */
		যদি ((allgroups || ap->group == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    যদি (forceit)
		    {
			যদি (*cmd != NUL && ap->পরবর্তী == ফাঁকা)
			{
			    au_remove_cmds(ap);
			    বিরতি;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    অন্যথা যদি (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    অন্যথা যদি (ap->পরবর্তী == ফাঁকা)
			বিরতি;
		}
	    }
	    পূর্ব_ap = &ap->পরবর্তী;
	}

	/*
	 * Add a new command.
	 */
	যদি (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    যদি (ap == ফাঁকা)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		যদি (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == ফাঁকা))
		{
		    semsg(_(e_বাফার_nr_invalid_বাফার_সংখ্যা), buflocal_nr);
		    ফেরত FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		যদি (ap == ফাঁকা)
		    ফেরত FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		যদি (ap->pat == ফাঁকা)
		{
		    vim_free(ap);
		    ফেরত FAIL;
		}

#ifdef FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		যদি (event == EVENT_MODECHANGED && !has_modechanged())
		    get_mode(last_mode);
#endif
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		যদি ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
			&& !(has_winscrolled() || has_winresized()))
		{
		    tabpage_T *save_curtab = curtab;
		    tabpage_T *tp;
		    FOR_ALL_TABPAGES(tp)
		    {
			unuse_tabpage(curtab);
			use_tabpage(tp);
			snapshot_windows_scroll_আকার();
		    }
		    unuse_tabpage(curtab);
		    use_tabpage(save_curtab);
		}

		যদি (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = ফাঁকা;
		}
		অন্যথা
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_to_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    যদি (reg_pat != ফাঁকা)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    যদি (reg_pat == ফাঁকা || ap->reg_prog == ফাঁকা)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			ফেরত FAIL;
		    }
		}
		ap->cmds = ফাঁকা;
		*পূর্ব_ap = ap;
		last_autopat[(পূর্ণসংখ্যা)event] = ap;
		ap->পরবর্তী = ফাঁকা;
		যদি (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		অন্যথা
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    পূর্ব_ac = &(ap->cmds);
	    যখন ((ac = *পূর্ব_ac) != ফাঁকা)
		পূর্ব_ac = &ac->পরবর্তী;
	    ac = ALLOC_ONE(AutoCmd);
	    যদি (ac == ফাঁকা)
		ফেরত FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    যদি (flags & UC_VIM9)
		ac->script_ctx.sc_version = SCRIPT_VERSION_VIM9;
#ifdef FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#endif
	    যদি (ac->cmd == ফাঁকা)
	    {
		vim_free(ac);
		ফেরত FAIL;
	    }
	    ac->পরবর্তী = ফাঁকা;
	    *পূর্ব_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    ফেরত OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    পূর্ণসংখ্যা
do_doautocmd(
    char_u	*arg_start,
    পূর্ণসংখ্যা		do_বার্তা,	    // give message for no matching autocmds?
    পূর্ণসংখ্যা		*did_something)
{
    char_u	*arg = arg_start;
    char_u	*fname;
    পূর্ণসংখ্যা		nothing_done = TRUE;
    পূর্ণসংখ্যা		group;

    যদি (did_something != ফাঁকা)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    যদি (arg == ফাঁকা)	    // out of memory
	ফেরত FAIL;

    যদি (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	ফেরত FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg, group != AUGROUP_ALL);
    যদি (fname == ফাঁকা)
	ফেরত FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    যখন (*arg && !ends_excmd(*arg) && !VIM_ISWHITE(*arg))
	যদি (apply_autocmds_group(event_name2nr(arg, &arg),
				      fname, ফাঁকা, TRUE, group, curbuf, ফাঁকা))
	    nothing_done = FALSE;

    যদি (nothing_done && do_বার্তা
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       )
	smsg(_("No matching autocommands: %s"), arg_start);
    যদি (did_something != ফাঁকা)
	*did_something = !nothing_done;

#ifdef FEAT_EVAL
    ফেরত aborting() ? FAIL : OK;
#অন্যথা
    ফেরত OK;
#endif
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    শূন্য
ex_doautoall(exarg_T *eap)
{
    পূর্ণসংখ্যা		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    পূর্ণসংখ্যা		call_do_modelines = check_nomodeline(&arg);
    পূর্ণসংখ্যা		did_aucmd;

    /*
     * This is a bit tricky: For some commands curwin->w_buffer needs to be
     * equal to curbuf, but for some buffers there may not be a window.
     * So we change the buffer for the current window for a moment.  This
     * gives problems when the autocommands make changes to the list of
     * buffers or windows...
     */
    FOR_ALL_BUFFERS(buf)
    {
	// Only do loaded buffers and skip the current buffer, it's done last.
	যদি (buf->b_ml.ml_mfp == ফাঁকা || buf == curbuf)
	    চালু;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	যদি (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    বিরতি;
	}

	set_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	যদি (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	যদি (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    বিরতি;
	}
    }

    // Execute autocommands for the current buffer last.
    যদি (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	যদি (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    পূর্ণসংখ্যা
check_nomodeline(char_u **argp)
{
    যদি (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	ফেরত FALSE;
    }
    ফেরত TRUE;
}

/*
 * Prepare for executing autocommands for (hidden) buffer "buf".
 * Search for a visible window containing the current buffer.  If there isn't
 * one then use an entry in "aucmd_win[]".
 * Set "curbuf" and "curwin" to match "buf".
 * When this fails "curbuf" is not equal "buf".
 */
    শূন্য
aucmd_prepbuf(
    aco_save_T	*aco,		// structure to save values in
    buf_T	*buf)		// new curbuf
{
    win_T	*win;
    পূর্ণসংখ্যা		save_ea;
#ifdef FEAT_AUTOCHDIR
    পূর্ণসংখ্যা		save_acd;
#endif
    পূর্ণসংখ্যা		same_বাফার = buf == curbuf;

    // Find a window that is for the new buffer
    যদি (same_বাফার)		// be quick when buf is curbuf
	win = curwin;
    অন্যথা
	FOR_ALL_WINDOWS(win)
	    যদি (win->w_বাফার == buf)
		বিরতি;

    // Allocate a window when needed.
    win_T *auc_win = ফাঁকা;
    পূর্ণসংখ্যা auc_idx = AUCMD_WIN_গুনতি;
    যদি (win == ফাঁকা)
    {
	জন্য (auc_idx = 0; auc_idx < AUCMD_WIN_গুনতি; ++auc_idx)
	    যদি (!aucmd_win[auc_idx].auc_win_used)
	    {
		যদি (aucmd_win[auc_idx].auc_win == ফাঁকা)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		যদি (auc_win != ফাঁকা)
		    aucmd_win[auc_idx].auc_win_used = TRUE;
		বিরতি;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	যদি (auc_win == ফাঁকা)
	    ফেরত;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == ফাঁকা ? 0 : prevwin->w_id;
#ifdef FEAT_JOB_CHANNEL
    যদি (bt_prompt(curbuf))
	aco->save_prompt_ঢোকাও = curbuf->b_prompt_ঢোকাও;
#endif

    যদি (win != ফাঁকা)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    অন্যথা
    {
	// There is no window for "buf", use "auc_win".  To minimize the side
	// effects, insert it in the current tab page.
	// Anything related to a window (e.g., setting folds) may have
	// unexpected results.
	aco->use_aucmd_win_idx = auc_idx;

	win_init_popup_win(auc_win, buf);

	// Make sure tp_localdir and globaldir are NULL to avoid a
	// chdir() in win_enter_ext().
	// win_init_popup_win() has already set w_localdir to NULL.
	aco->tp_localdir = curtab->tp_localdir;
	curtab->tp_localdir = ফাঁকা;
	aco->globaldir = globaldir;
	globaldir = ফাঁকা;

	// Split the current window, put the auc_win in the upper half.
	// We don't want the BufEnter or WinEnter autocommands.
	block_autocmds();
	make_snapshot(SNAP_AUCMD_IDX);
	save_ea = p_ea;
	p_ea = FALSE;

#ifdef FEAT_AUTOCHDIR
	// Prevent chdir() call in win_enter_ext(), through do_autochdir().
	save_acd = p_acd;
	p_acd = FALSE;
#endif

	(শূন্য)win_split_ins(0, WSP_শীর্ষ | WSP_FORCE_ROOM, auc_win, 0, ফাঁকা);
	win_comp_pos();   // recompute window positions
	p_ea = save_ea;
#ifdef FEAT_AUTOCHDIR
	p_acd = save_acd;
#endif
	unblock_autocmds();
	curwin = auc_win;
    }
    curbuf = buf;
    aco->new_curwin_id = curwin->w_id;
    set_bufref(&aco->new_curbuf, curbuf);

    aco->save_VIsual_active = VIsual_active;
    যদি (!same_বাফার)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_active = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    শূন্য
aucmd_restbuf(
    aco_save_T	*aco)		// structure holding saved values
{
    পূর্ণসংখ্যা	    dummy;
    win_T   *save_curwin;

    যদি (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	block_autocmds();
	যদি (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		যদি (wp == awp)
		{
		    যদি (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(awp);
		    যাও win_found;
		}
	    }
	}
win_found:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(শূন্য)winframe_remove(curwin, &dummy, ফাঁকা, ফাঁকা);
	win_remove(curwin, ফাঁকা);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_used = FALSE;
	last_status(FALSE);	    // may need to remove last status line

	যদি (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_find_by_id(aco->save_curwin_id);
	যদি (save_curwin != ফাঁকা)
	    curwin = save_curwin;
	অন্যথা
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_বাফার;
#ifdef FEAT_JOB_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_window(curwin);
	যদি (bt_prompt(curbuf))
	    curbuf->b_prompt_ঢোকাও = aco->save_prompt_ঢোকাও;
#endif
	prevwin = win_find_by_id(aco->save_prevwin_id);
#ifdef FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#endif
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	যদি (awp->w_localdir != ফাঁকা)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_active = aco->save_VIsual_active;
	check_cursor();
	যদি (curwin->w_topline > curbuf->b_ml.ml_সারি_গুনতি)
	{
	    curwin->w_topline = curbuf->b_ml.ml_সারি_গুনতি;
#ifdef FEAT_DIFF
	    curwin->w_topfill = 0;
#endif
	}
#যদি defined(FEAT_GUI)
	যদি (gui.in_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_বাম], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_ডান], FALSE);
	    gui_may_update_scrollbars();
	}
#endif
    }
    অন্যথা
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_find_by_id(aco->save_curwin_id);
	যদি (save_curwin != ফাঁকা)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    যদি (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != ফাঁকা)
	    {
#যদি defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		যদি (curwin->w_s == &curbuf->b_s)
		    curwin->w_s = &aco->new_curbuf.br_buf->b_s;
#endif
		--curbuf->b_nwindows;
		curbuf = aco->new_curbuf.br_buf;
		curwin->w_বাফার = curbuf;
		++curbuf->b_nwindows;
	    }

	    curwin = save_curwin;
	    curbuf = curwin->w_বাফার;
	    prevwin = win_find_by_id(aco->save_prevwin_id);

	    // In case the autocommand moves the cursor to a position that
	    // does not exist in curbuf.
	    VIsual_active = aco->save_VIsual_active;
	    check_cursor();
	}
    }

    VIsual_active = aco->save_VIsual_active;
    check_cursor();	    // just in case lines got deleted
    যদি (VIsual_active)
	check_pos(curbuf, &VIsual);
}

স্থির পূর্ণসংখ্যা	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    পূর্ণসংখ্যা
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    পূর্ণসংখ্যা		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    ফেরত apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ফাঁকা);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    পূর্ণসংখ্যা
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    পূর্ণসংখ্যা		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    ফেরত apply_autocmds_group(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    পূর্ণসংখ্যা
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    পূর্ণসংখ্যা		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    পূর্ণসংখ্যা		*retval)    // pointer to caller's retval
{
    পূর্ণসংখ্যা		did_cmd;

#ifdef FEAT_EVAL
    যদি (should_abort(*retval))
	ফেরত FALSE;
#endif

    did_cmd = apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ফাঁকা);
    যদি (did_cmd
#ifdef FEAT_EVAL
	    && aborting()
#endif
	    )
	*retval = FAIL;
    ফেরত did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    স্থির পূর্ণসংখ্যা
has_cursorhold(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)(get_real_state() == MODE_NORMAL_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != ফাঁকা);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    পূর্ণসংখ্যা
trigger_cursorhold(শূন্য)
{
    পূর্ণসংখ্যা		state;

    যদি (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_active())
    {
	state = get_real_state();
	যদি (state == MODE_NORMAL_BUSY || (state & MODE_ঢোকাও) != 0)
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    পূর্ণসংখ্যা
has_winresized(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_WINRESIZED] != ফাঁকা);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    পূর্ণসংখ্যা
has_winscrolled(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_WINSCROLLED] != ফাঁকা);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    পূর্ণসংখ্যা
has_cursormoved(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_CURSORMOVED] != ফাঁকা);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    পূর্ণসংখ্যা
has_cursormovedI(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_CURSORMOVEDI] != ফাঁকা);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    পূর্ণসংখ্যা
has_textchanged(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_TEXTCHANGED] != ফাঁকা);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    পূর্ণসংখ্যা
has_textchangedI(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_TEXTCHANGEDI] != ফাঁকা);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    পূর্ণসংখ্যা
has_textchangedP(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_TEXTCHANGEDP] != ফাঁকা);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    পূর্ণসংখ্যা
has_insertcharpre(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_INSERTCHARPRE] != ফাঁকা);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    পূর্ণসংখ্যা
has_keyinputpre(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_KEYINPUTPRE] != ফাঁকা);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    পূর্ণসংখ্যা
has_cmdundefined(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_CMDUNDEFINED] != ফাঁকা);
}

#যদি defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    পূর্ণসংখ্যা
has_textputpost(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_TEXTPUTPOST] != ফাঁকা);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    পূর্ণসংখ্যা
has_textputpre(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_TEXTPUTPRE] != ফাঁকা);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    পূর্ণসংখ্যা
has_textyankpost(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_TEXTYANKPOST] != ফাঁকা);
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    পূর্ণসংখ্যা
has_completechanged(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_COMPLETECHANGED] != ফাঁকা);
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    পূর্ণসংখ্যা
has_modechanged(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_MODECHANGED] != ফাঁকা);
}
#endif

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    স্থির পূর্ণসংখ্যা
apply_autocmds_group(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    পূর্ণসংখ্যা		force,	     // when TRUE, ignore autocmd_busy
    পূর্ণসংখ্যা		group,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap UNUSED) // command arguments
{
    char_u	*sfname = ফাঁকা;	// short file name
    char_u	*লেজ;
    পূর্ণসংখ্যা		save_changed;
    buf_T	*old_curbuf;
    পূর্ণসংখ্যা		retval = FALSE;
    char_u	*save_autocmd_fname;
    পূর্ণসংখ্যা		save_autocmd_fname_full;
    পূর্ণসংখ্যা		save_autocmd_bufnr;
    char_u	*save_autocmd_match;
    পূর্ণসংখ্যা		save_autocmd_busy;
    পূর্ণসংখ্যা		save_autocmd_nested;
    স্থির পূর্ণসংখ্যা	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ifdef FEAT_EVAL
    funccal_entry_T funccal_entry;
    char_u	*save_cmdarg;
    long	save_cmdbang;
#endif
    স্থির পূর্ণসংখ্যা	filechangeshell_busy = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif
    পূর্ণসংখ্যা		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    পূর্ণসংখ্যা		save_চাবি_Typed = চাবি_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    যদি (event == অঙ্ক_EVENTS || first_autopat[(পূর্ণসংখ্যা)event] == ফাঁকা
	    || autocmd_blocked > 0)
	যাও BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    যদি (autocmd_busy && !(force || autocmd_nested))
	যাও BYPASS_AU;

#ifdef FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    যদি (aborting())
	যাও BYPASS_AU;
#endif

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    যদি (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	যাও BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    যদি (event_ignored(event, p_ei))
	যাও BYPASS_AU;

    পূর্ণসংখ্যা win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    যদি (buf == curbuf && event_tab[event].চাবি <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    অন্যথা যদি (buf != ফাঁকা && event_tab[event].চাবি <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    যদি (wp->w_বাফার == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		বিরতি;
	    }
    }
    যদি (win_ignore)
	যাও BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    যদি (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	যাও BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    যদি (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	যাও BYPASS_AU;

    যদি (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_changed_গুনতি;

    /*
     * Save the autocmd_* variables and info about the current buffer.
     */
    save_autocmd_fname = autocmd_fname;
    save_autocmd_fname_full = autocmd_fname_full;
    save_autocmd_bufnr = autocmd_bufnr;
    save_autocmd_match = autocmd_match;
    save_autocmd_busy = autocmd_busy;
    save_autocmd_nested = autocmd_nested;
    save_changed = curbuf->b_changed;
    old_curbuf = curbuf;

    /*
     * Set the file name to be used for <afile>.
     * Make a copy to avoid that changing a buffer name or directory makes it
     * invalid.
     */
    যদি (fname_io == ফাঁকা)
    {
	যদি (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = ফাঁকা;
	অন্যথা যদি (fname != ফাঁকা && !ends_excmd(*fname))
	    autocmd_fname = fname;
	অন্যথা যদি (buf != ফাঁকা)
	    autocmd_fname = buf->b_ffname;
	অন্যথা
	    autocmd_fname = ফাঁকা;
    }
    অন্যথা
	autocmd_fname = fname_io;
    যদি (autocmd_fname != ফাঁকা)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_full = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    যদি (buf == ফাঁকা)
	autocmd_bufnr = 0;
    অন্যথা
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    যদি (fname == ফাঁকা || *fname == NUL)
    {
	যদি (buf == ফাঁকা)
	    fname = ফাঁকা;
	অন্যথা
	{
#ifdef FEAT_SYN_HL
	    যদি (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    অন্যথা
#endif
		যদি (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		অন্যথা
		{
		    যদি (buf->b_sfname != ফাঁকা)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	যদি (fname == ফাঁকা)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    অন্যথা
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	যদি (event == EVENT_FILETYPE
		|| event == EVENT_SYNTAX
		|| event == EVENT_CMDLINECHANGED
		|| event == EVENT_CMDLINEENTER
		|| event == EVENT_CMDLINELEAVEPRE
		|| event == EVENT_CMDLINELEAVE
		|| event == EVENT_CURSORMOVEDC
		|| event == EVENT_CMDWINENTER
		|| event == EVENT_CMDWINLEAVE
		|| event == EVENT_CMDUNDEFINED
		|| event == EVENT_FUNCUNDEFINED
		|| event == EVENT_KEYINPUTPRE
		|| event == EVENT_REMOTEREPLY
		|| event == EVENT_SPELLFILEMISSING
		|| event == EVENT_QUICKFIXCMDPRE
		|| event == EVENT_COLORSCHEME
		|| event == EVENT_COLORSCHEMEPRE
		|| event == EVENT_OPTIONSET
		|| event == EVENT_QUICKFIXCMDPOST
		|| event == EVENT_DIRCHANGED
		|| event == EVENT_DIRCHANGEDPRE
		|| event == EVENT_MODECHANGED
		|| event == EVENT_MENUPOPUP
		|| event == EVENT_USER
		|| event == EVENT_WINCLOSED
		|| event == EVENT_WINRESIZED
		|| event == EVENT_WINSCROLLED
		|| event == EVENT_TERMRESPONSEALL)
	{
	    fname = vim_strsave(fname);
	    autocmd_fname_full = TRUE; // don't expand it later
	}
	অন্যথা
	    fname = FullName_save(fname, FALSE);
    }
    যদি (fname == ফাঁকা)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	যাও BYPASS_AU;
    }

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    যদি (sfname != ফাঁকা)
	forward_slash(sfname);
    forward_slash(fname);
#endif

#ifdef VMS
    // remove version for correct match
    যদি (sfname != ফাঁকা)
	vms_remove_version(sfname);
    vms_remove_version(fname);
#endif

    /*
     * Set the name to be used for <amatch>.
     */
    autocmd_match = fname;


    // Don't redraw while doing autocommands.
    ++RedrawingDisabled;

    // name and lnum are filled in later
    estack_ঠেলো(ETYPE_AUCMD, ফাঁকা, 0);
    ESTACK_CHECK_SETUP;

    save_current_sctx = current_sctx;

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
	prof_child_enter(&wait_time); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_entry);
#endif

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    যদি (!autocmd_busy)
    {
	save_অনুসন্ধান_patterns();
	যদি (!ins_compl_active())
	{
	    saveRedobuff(&save_redo);
	    did_save_redobuff = TRUE;
	}
	curbuf->b_did_filetype = curbuf->b_keep_filetype;
    }

    /*
     * Note that we are applying autocmds.  Some commands need to know.
     */
    autocmd_busy = TRUE;
    filechangeshell_busy = (event == EVENT_FILECHANGEDSHELL);
    ++nesting;		// see matching decrement below

    // Remember that FileType was triggered.  Used for did_filetype().
    যদি (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    লেজ = gettail(fname);

    // Find first autocommand that matches
    CLEAR_FIELD(patcmd);
    patcmd.curpat = first_autopat[(পূর্ণসংখ্যা)event];
    patcmd.group = group;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.লেজ = লেজ;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_পরবর্তী_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    যদি (patcmd.curpat != ফাঁকা)
    {
	// add to active_apc_list
	patcmd.পরবর্তী = active_apc_list;
	active_apc_list = &patcmd;

#ifdef FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (long)get_vim_var_nr(VV_CMDBANG);
	যদি (eap != ফাঁকা)
	{
	    save_cmdarg = set_cmdarg(eap, ফাঁকা);
	    set_vim_var_nr(VV_CMDBANG, (long)eap->forceit);
	}
	অন্যথা
	    save_cmdarg = ফাঁকা;	// avoid gcc warning
#endif
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	জন্য (ap = patcmd.curpat; ap->পরবর্তী != ফাঁকা; ap = ap->পরবর্তী)
	    ap->last = FALSE;
	ap->last = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	যদি (nesting == 1)
	    check_lnums(TRUE);
	অন্যথা
	    check_lnums_nested(TRUE);

	পূর্ণসংখ্যা save_did_emsg = did_emsg;
	পূর্ণসংখ্যা save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(ফাঁকা, getnextac, (শূন্য *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	set_pressedreturn(save_ex_pressedreturn);

	যদি (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ifdef FEAT_EVAL
	যদি (eap != ফাঁকা)
	{
	    (শূন্য)set_cmdarg(ফাঁকা, save_cmdarg);
	    set_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#endif
	// delete from active_apc_list
	যদি (active_apc_list == &patcmd)	    // just in case
	    active_apc_list = patcmd.পরবর্তী;
    }

    যদি (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_বের_করো();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_full = save_autocmd_fname_full;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_match = save_autocmd_match;
    current_sctx = save_current_sctx;
#ifdef FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif
    চাবি_Typed = save_চাবি_Typed;
    vim_free(fname);
    vim_free(sfname);
    --nesting;		// see matching increment above

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.  Free any buffers in the au_pending_free_buf list and
     * free any windows in the au_pending_free_win list.
     */
    যদি (!autocmd_busy)
    {
	restore_অনুসন্ধান_patterns();
	যদি (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	যখন (au_pending_free_buf != ফাঁকা)
	{
	    buf_T *b = au_pending_free_buf->b_পরবর্তী;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	যখন (au_pending_free_win != ফাঁকা)
	{
	    win_T *w = au_pending_free_win->w_পরবর্তী;

	    vim_free(au_pending_free_win);
	    au_pending_free_win = w;
	}
    }

    /*
     * Some events don't set or reset the Changed flag.
     * Check if still in the same buffer!
     */
    যদি (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	যদি (curbuf->b_changed != save_changed)
	    need_maketitle = TRUE;
	curbuf->b_changed = save_changed;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    যদি (event == EVENT_BUFWIPEOUT && buf != ফাঁকা)
	aubuflocal_remove(buf);

    যদি (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    ফেরত retval;
}

#ifdef FEAT_EVAL
স্থির char_u	*old_termresponse = ফাঁকা;
স্থির char_u	*old_termu7resp = ফাঁকা;
স্থির char_u	*old_termblinkresp = ফাঁকা;
স্থির char_u	*old_termrbgresp = ফাঁকা;
স্থির char_u	*old_termrfgresp = ফাঁকা;
স্থির char_u	*old_termstyleresp = ফাঁকা;
#endif

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    শূন্য
block_autocmds(শূন্য)
{
#ifdef FEAT_EVAL
    // Remember the value of v:termresponse.
    যদি (autocmd_blocked == 0)
    {
	old_termresponse = get_vim_var_str(VV_TERMRESPONSE);
	old_termu7resp = get_vim_var_str(VV_TERMU7RESP);
	old_termblinkresp = get_vim_var_str(VV_TERMBLINKRESP);
	old_termrbgresp = get_vim_var_str(VV_TERMRBGRESP);
	old_termrfgresp = get_vim_var_str(VV_TERMRFGRESP);
	old_termstyleresp = get_vim_var_str(VV_TERMSTYLERESP);
    }
#endif
    ++autocmd_blocked;
}

    শূন্য
unblock_autocmds(শূন্য)
{
    --autocmd_blocked;

#ifdef FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    যদি (autocmd_blocked == 0)
    {
	যদি (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, ফাঁকা, ফাঁকা, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", ফাঁকা, FALSE, curbuf);
	}
	যদি (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", ফাঁকা, FALSE, curbuf);
	}
	যদি (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", ফাঁকা, FALSE, curbuf);
	}
	যদি (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", ফাঁকা, FALSE, curbuf);
	}
	যদি (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", ফাঁকা, FALSE, curbuf);
	}
	যদি (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", ফাঁকা, FALSE, curbuf);
	}
    }
#endif
}

    পূর্ণসংখ্যা
is_autocmd_blocked(শূন্য)
{
    ফেরত autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    স্থির শূন্য
auto_পরবর্তী_pat(
    AutoPatCmd_T *apc,
    পূর্ণসংখ্যা		stop_at_last)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*entry;

    entry = ((estack_T *)exestack.ga_তথ্য) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(entry->es_name);
    entry->es_info.aucmd = ফাঁকা;

    জন্য (ap = apc->curpat; ap != ফাঁকা && !got_int; ap = ap->পরবর্তী)
    {
	apc->curpat = ফাঁকা;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	যদি (ap->pat != ফাঁকা && ap->cmds != ফাঁকা
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
	    // execution-condition
	    যদি (ap->buflocal_nr == 0
		    ? (match_file_pat(ফাঁকা, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->লেজ, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		অক্ষর	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->দৈর্ঘ্য + ap->patlen + 1);
		যদি (namep != ফাঁকা)
		{
		    sprintf((অক্ষর *)namep, fmt, (অক্ষর *)event_name->string, (অক্ষর *)ap->pat);
		    যদি (p_verbose >= 8)
		    {
			verbose_enter();
			smsg(_("Executing %s"), namep);
			verbose_leave();
		    }
		}

		// Update the exestack entry for this autocmd.
		entry->es_name = namep;
		entry->es_info.aucmd = apc;

		apc->curpat = ap;
		apc->nextcmd = ap->cmds;
		// mark last command
		জন্য (cp = ap->cmds; cp->পরবর্তী != ফাঁকা; cp = cp->পরবর্তী)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    সারি_breakcheck();
	    যদি (apc->curpat != ফাঁকা)	    // found a match
		বিরতি;
	}
	যদি (stop_at_last && ap->last)
	    বিরতি;
    }
}

#যদি defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    ফেরত &acp->script_ctx;
}
#endif

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	পূর্ণসংখ্যা c UNUSED,
	শূন্য *cookie,
	পূর্ণসংখ্যা indent UNUSED,
	getline_opt_T options UNUSED)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    যদি (acp->curpat == ফাঁকা)
	ফেরত ফাঁকা;

    // repeat until we find an autocommand to execute
    জন্য (;;)
    {
	// skip removed commands
	যখন (acp->nextcmd != ফাঁকা && acp->nextcmd->cmd == ফাঁকা)
	    যদি (acp->nextcmd->last)
		acp->nextcmd = ফাঁকা;
	    অন্যথা
		acp->nextcmd = acp->nextcmd->পরবর্তী;

	যদি (acp->nextcmd != ফাঁকা)
	    বিরতি;

	// at end of commands, find next pattern that matches
	যদি (acp->curpat->last)
	    acp->curpat = ফাঁকা;
	অন্যথা
	    acp->curpat = acp->curpat->পরবর্তী;
	যদি (acp->curpat != ফাঁকা)
	    auto_পরবর্তী_pat(acp, TRUE);
	যদি (acp->curpat == ফাঁকা)
	    ফেরত ফাঁকা;
    }

    ac = acp->nextcmd;

    যদি (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	বার্তা_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    যদি (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    যদি (ac->last)
	acp->nextcmd = ফাঁকা;
    অন্যথা
	acp->nextcmd = ac->পরবর্তী;
    ফেরত retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    স্থির পূর্ণসংখ্যা
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_full)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*লেজ = gettail(sfname);
    পূর্ণসংখ্যা		retval = FALSE;

    যদি (first_autopat[(পূর্ণসংখ্যা)event] == ফাঁকা)
	ফেরত FALSE;

    fname = is_full ? vim_strsave(sfname) : FullName_save(sfname, FALSE);
    যদি (fname == ফাঁকা)
	ফেরত FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    যদি (sfname != ফাঁকা)
	forward_slash(sfname);
    forward_slash(fname);
#endif

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	যদি (ap->pat != ফাঁকা && ap->cmds != ফাঁকা
	      && (ap->buflocal_nr == 0
		? match_file_pat(ফাঁকা, &ap->reg_prog,
					  fname, sfname, লেজ, ap->allow_dirs)
		: buf != ফাঁকা && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    বিরতি;
	}

    vim_free(fname);
#ifdef BACKSLASH_IN_FILENAME
    vim_free(sfname);
#endif

    ফেরত retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    পূর্ণসংখ্যা
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    ফেরত has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    পূর্ণসংখ্যা
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    ফেরত has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    যদি (idx == augroups.ga_len)		// add "END" add the end
	ফেরত (char_u *)"END";
    যদি (idx < 0 || idx >= augroups.ga_len)	// end of list
	ফেরত ফাঁকা;
    যদি (AUGROUP_NAME(idx) == ফাঁকা || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	ফেরত (char_u *)"";
    ফেরত AUGROUP_NAME(idx);		// return a name
}

স্থির পূর্ণসংখ্যা include_groups = FALSE;

    char_u  *
set_context_in_autocmd(
    expand_T	*xp,
    char_u	*arg,
    পূর্ণসংখ্যা		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    পূর্ণসংখ্যা		group;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    group = au_get_grouparg(&arg);
    যদি (group == AUGROUP_ERROR)
	ফেরত ফাঁকা;
    // If there only is a group name that's what we expand.
    যদি (*arg == NUL && group != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	group = AUGROUP_ALL;
    }

    // skip over event name
    জন্য (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	যদি (*p == ',')
	    arg = p + 1;
    যদি (*p == NUL)
    {
	যদি (group == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_pattern = arg;
	ফেরত ফাঁকা;
    }

    // skip over pattern
    arg = skipwhite(p);
    যখন (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    যদি (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	জন্য (পূর্ণসংখ্যা i = 0; i < 2; i++)
	{
	    যদি (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    যদি (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    যদি (!in_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	ফেরত arg;			    // expand (next) command
    }

    যদি (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    অন্যথা
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    ফেরত ফাঁকা;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    পূর্ণসংখ্যা i;

    যদি (idx < augroups.ga_len)		// First list group names, if wanted
    {
	যদি (!include_groups || AUGROUP_NAME(idx) == ফাঁকা
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    ফেরত (char_u *)"";	// skip deleted entries
	ফেরত AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    যদি (i < 0 || i >= অঙ্ক_EVENTS)
	ফেরত ফাঁকা;

    ফেরত event_tab[i].মান.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_group(expand_T *xp UNUSED, পূর্ণসংখ্যা idx, পূর্ণসংখ্যা win)
{
    যদি (idx < 0 || idx >= অঙ্ক_EVENTS)
	ফেরত ফাঁকা;

    যদি (!win)
	ফেরত event_tab[idx].মান.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    পূর্ণসংখ্যা j = 0;
    জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_EVENTS; ++i)
    {
	j += event_tab[i].চাবি <= 0;
	যদি (j == idx + 1)
	    ফেরত event_tab[i].মান.string;
    }

    ফেরত ফাঁকা;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    পূর্ণসংখ্যা
has_tabclosedpre(শূন্য)
{
    ফেরত (first_autopat[(পূর্ণসংখ্যা)EVENT_TABCLOSEDPRE] != ফাঁকা);
}

#যদি defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    পূর্ণসংখ্যা
autocmd_supported(char_u *name)
{
    char_u *p;

    ফেরত (event_name2nr(name, &p) != অঙ্ক_EVENTS);
}

/*
 * Return TRUE if an autocommand is defined for a group, event and
 * pattern:  The group can be omitted to accept any group. "event" and "pattern"
 * can be NULL to accept any event and pattern. "pattern" can be NULL to accept
 * any pattern. Buffer-local patterns <buffer> or <buffer=N> are accepted.
 * Used for:
 *	exists("#Group") or
 *	exists("#Group#Event") or
 *	exists("#Group#Event#pat") or
 *	exists("#Event") or
 *	exists("#Event#pat")
 */
    পূর্ণসংখ্যা
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*pattern = ফাঁকা;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = ফাঁকা;
    পূর্ণসংখ্যা		group;
    পূর্ণসংখ্যা		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    যদি (arg_save == ফাঁকা)
	ফেরত FALSE;
    p = vim_strchr(arg_save, '#');
    যদি (p != ফাঁকা)
	*p++ = NUL;

    // First, look for an autocmd group name
    group = au_find_group(arg_save);
    যদি (group == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	group = AUGROUP_ALL;
	event_name = arg_save;
    }
    অন্যথা
    {
	যদি (p == ফাঁকা)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    যাও theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	যদি (p != ফাঁকা)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    pattern = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    যদি (event == অঙ্ক_EVENTS)
	যাও theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = first_autopat[(পূর্ণসংখ্যা)event];
    যদি (ap == ফাঁকা)
	যাও theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    যদি (pattern != ফাঁকা && STRICMP(pattern, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    জন্য ( ; ap != ফাঁকা; ap = ap->পরবর্তী)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	যদি (ap->pat != ফাঁকা && ap->cmds != ফাঁকা
	    && (group == AUGROUP_ALL || ap->group == group)
	    && (pattern == ফাঁকা
		|| (buflocal_buf == ফাঁকা
		    ? fnamecmp(ap->pat, pattern) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    বিরতি;
	}

theend:
    vim_free(arg_save);
    ফেরত retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    স্থির শূন্য
autocmd_add_or_delete(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = ফাঁকা;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*group_name = ফাঁকা;
    পূর্ণসংখ্যা		group;
    char_u	*pat = ফাঁকা;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = ফাঁকা;
    char_u	*end;
    পূর্ণসংখ্যা		once;
    পূর্ণসংখ্যা		nested;
    পূর্ণসংখ্যা		replace;		// replace the cmd for a group/event
    পূর্ণসংখ্যা		retval = VVAL_TRUE;
    পূর্ণসংখ্যা		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_সংখ্যা = VVAL_FALSE;

    যদি (check_restricted() || check_secure())
	ফেরত;

    যদি (check_for_list_arg(argvars, 0) == FAIL)
	ফেরত;

    aucmd_list = argvars[0].vval.v_list;
    যদি (aucmd_list == ফাঁকা)
	ফেরত;

    FOR_ALL_LIST_বস্তুসমূহ(aucmd_list, li)
    {
	VIM_CLEAR(group_name);
	VIM_CLEAR(cmd);
	event_name = ফাঁকা;
	event_list = ফাঁকা;
	pat = ফাঁকা;
	pat_list = ফাঁকা;

	যদি (li->li_tv.v_type != VAR_DICT)
	    চালু;

	event_dict = li->li_tv.vval.v_dict;
	যদি (event_dict == ফাঁকা)
	    চালু;

	di = dict_find(event_dict, (char_u *)"event", -1);
	যদি (di != ফাঁকা)
	{
	    যদি (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		যদি (event_name == ফাঁকা)
		{
		    emsg(_(e_string_required));
		    চালু;
		}
	    }
	    অন্যথা যদি (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		যদি (event_list == ফাঁকা)
		{
		    emsg(_(e_list_required));
		    চালু;
		}
	    }
	    অন্যথা
	    {
		emsg(_(e_string_or_list_expected));
		চালু;
	    }
	}

	group_name = dict_get_string(event_dict, "group", TRUE);
	যদি (group_name == ফাঁকা || *group_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    group = current_augroup;
	অন্যথা
	{
	    group = au_find_group(group_name);
	    যদি (group == AUGROUP_ERROR)
	    {
		যদি (delete)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    বিরতি;
		}
		// group is not found, create it now
		group = au_new_group(group_name);
		যদি (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    বিরতি;
		}

		current_augroup = group;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	যদি (dict_has_চাবি(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_সংখ্যা_def(event_dict, "bufnr", -1);
	    যদি (bnum == -1)
		চালু;

	    vim_snprintf((অক্ষর *)IObuff, IOSIZE, "<buffer=%d>", (পূর্ণসংখ্যা)bnum);
	    pat = IObuff;
	}
	অন্যথা
	{
	    di = dict_find(event_dict, (char_u *)"pattern", -1);
	    যদি (di != ফাঁকা)
	    {
		যদি (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    যদি (pat == ফাঁকা)
		    {
			emsg(_(e_string_required));
			চালু;
		    }
		}
		অন্যথা যদি (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    যদি (pat_list == ফাঁকা)
		    {
			emsg(_(e_list_required));
			চালু;
		    }
		}
		অন্যথা
		{
		    emsg(_(e_string_or_list_expected));
		    চালু;
		}
	    }
	    অন্যথা যদি (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	যদি (cmd == ফাঁকা)
	{
	    যদি (delete)
		cmd = vim_strsave((char_u *)"");
	    অন্যথা
		চালু;
	}

	যদি (delete && (event_name == ফাঁকা
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    জন্য (event = (event_T)0; (পূর্ণসংখ্যা)event < অঙ্ক_EVENTS;
		    event = (event_T)((পূর্ণসংখ্যা)event + 1))
	    {
		যদি (do_autocmd_event(event, pat, once, nested, cmd, delete,
							group, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    বিরতি;
		}
	    }
	}
	অন্যথা
	{
	    char_u *p = ফাঁকা;

	    eli = ফাঁকা;
	    end = ফাঁকা;
	    যখন (TRUE)
	    {
		যদি (event_list != ফাঁকা)
		{
		    যদি (eli == ফাঁকা)
			eli = event_list->lv_first;
		    অন্যথা
			eli = eli->li_পরবর্তী;
		    যদি (eli == ফাঁকা)
			বিরতি;
		    যদি (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == ফাঁকা)
		    {
			emsg(_(e_string_required));
			বিরতি;
		    }
		}
		অন্যথা
		{
		    যদি (p == ফাঁকা)
			p = event_name;
		    যদি (p == ফাঁকা || *p == NUL)
			বিরতি;
		}

		event = event_name2nr(p, &end);
		যদি (event == অঙ্ক_EVENTS || *end != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    বিরতি;
		}
		যদি (pat != ফাঁকা)
		{
		    যদি (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, group, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			বিরতি;
		    }
		}
		অন্যথা যদি (pat_list != ফাঁকা)
		{
		    FOR_ALL_LIST_বস্তুসমূহ(pat_list, pli)
		    {
			যদি (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == ফাঁকা)
			{
			    emsg(_(e_string_required));
			    চালু;
			}
			যদি (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, group, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    বিরতি;
			}
		    }
		    যদি (retval == VVAL_FALSE)
			বিরতি;
		}
		যদি (event_name != ফাঁকা)
		    p = end;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	যদি (delete && group_name != ফাঁকা &&
		(event_name == ফাঁকা || event_name[0] == NUL)
		&& (pat == ফাঁকা || pat[0] == NUL)
		&& (cmd == ফাঁকা || cmd[0] == NUL))
	    au_del_group(group_name);
    }

    VIM_CLEAR(group_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_সংখ্যা = retval;
}

/*
 * autocmd_add() function
 */
    শূন্য
f_autocmd_add(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    শূন্য
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    শূন্য
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = অঙ্ক_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = ফাঁকা;
    char_u	*name = ফাঁকা;
    পূর্ণসংখ্যা		group = AUGROUP_ALL;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;
    যদি (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	যদি (dict_has_চাবি(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    যদি (name == ফাঁকা)
		ফেরত;

	    যদি (*name == NUL)
		group = AUGROUP_DEFAULT;
	    অন্যথা
	    {
		group = au_find_group(name);
		যদি (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), name);
		    vim_free(name);
		    ফেরত;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	যদি (dict_has_চাবি(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    যদি (name == ফাঁকা)
		ফেরত;

	    যদি (name[0] == '*' && name[1] == NUL)
		event_arg = অঙ্ক_EVENTS;
	    অন্যথা
	    {
		keyvalue_T target;
		keyvalue_T *entry;

		target.চাবি = 0;
		target.মান.string = name;
		target.মান.দৈর্ঘ্য = STRLEN(target.মান.string);
		entry = (keyvalue_T *)bsearch(&target, &event_tab,
		    অঙ্ক_EVENTS, আকার(event_tab[0]), cmp_keyvalue_মান_ni);
		যদি (entry == ফাঁকা)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    ফেরত;
		}
		event_arg = (event_T)abs(entry->চাবি);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	যদি (dict_has_চাবি(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    যদি (pat == ফাঁকা)
		ফেরত;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    জন্য (event = (event_T)0; (পূর্ণসংখ্যা)event < অঙ্ক_EVENTS;
	    event = (event_T)((পূর্ণসংখ্যা)event + 1))
    {
	string_T    *event_name;

	যদি (event_arg != অঙ্ক_EVENTS && event != event_arg)
	    চালু;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	group_name;

	    যদি (ap->pat == ফাঁকা)		// pattern has been removed
		চালু;

	    যদি (group != AUGROUP_ALL && group != ap->group)
		চালু;

	    যদি (pat != ফাঁকা && STRCMP(pat, ap->pat) != 0)
		চালু;

	    group_name.string = get_augroup_name(ফাঁকা, ap->group);
	    যদি (group_name.string == ফাঁকা)
		STR_LITERAL_SET(group_name, "");
	    অন্যথা
		group_name.দৈর্ঘ্য = STRLEN(group_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    জন্য (ac = ap->cmds; ac != ফাঁকা; ac = ac->পরবর্তী)
	    {
		event_dict = dict_alloc();
		যদি (event_dict == ফাঁকা
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    ফেরত;
		}

		যদি (dict_add_string_len(event_dict, "event",
			event_name->string, (পূর্ণসংখ্যা)event_name->দৈর্ঘ্য) == FAIL
			|| dict_add_string_len(event_dict, "group",
			    group_name.string, (পূর্ণসংখ্যা)group_name.দৈর্ঘ্য) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_add_সংখ্যা(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_add_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_add_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_add_bool(event_dict, "once", ac->once) == FAIL
			|| dict_add_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    ফেরত;
		}
	    }
	}
    }

    vim_free(pat);
}

#endif
