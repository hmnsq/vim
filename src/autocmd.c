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

#شامل "vim.h"

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
تعریفنوع ساختار AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    نویسه	    once;		// "One shot": removed after execution
    نویسه	    nested;		// If autocommands nest here.
    نویسه	    last;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    ساختار AutoCmd  *next;		// next AutoCmd in list
} AutoCmd;

تعریفنوع ساختار AutoPat
{
    ساختار AutoPat  *next;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    صحیح		    group;		// group ID
    صحیح		    patlen;		// strlen() of pat
    صحیح		    buflocal_nr;	// !=0 for buffer-local AutoPat
    نویسه	    allow_dirs;		// Pattern may match whole path
    نویسه	    last;		// last pattern for apply_autocmds()
} AutoPat;

//
// special cases:
// BufNewFile and BufRead are searched for ALOT (especially at startup)
// so we pre-determine their index into the event_tab[] table for fast access.
// Keep these values in sync with event_tab[]!
#define BUFNEWFILE_INDEX 9
#define BUFREAD_INDEX 10

// Must be sorted by the 'value' field because it is used by bsearch()!
// Events with positive keys aren't allowed in 'eventignorewin'.
ایستا keyvalue_T event_tab[رقم_EVENTS] = {
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

ایستا AutoPat *first_autopat[رقم_EVENTS] = { پوچ };
ایستا AutoPat *last_autopat[رقم_EVENTS] = { پوچ };

#define AUGROUP_DEFAULT    (-1)	    // default autocmd group
#define AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#define AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
ساختار AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    صحیح		group;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*tail;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    صحیح		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *next;		// chain of active apc-s for auto-invalidation
};

ایستا AutoPatCmd_T *active_apc_list = پوچ; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#define FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    برای ((ap) = first_autopat[(صحیح)(event)]; (ap) != پوچ; (ap) = (ap)->next)

/*
 * augroups stores a list of autocmd group names.
 */
ایستا garray_T augroups = {0, 0, اندازه(char_u *), 10, پوچ};
#define AUGROUP_NAME(i) (((char_u **)augroups.ga_data)[i])
// use get_deleted_augroup() to get this
ایستا char_u *deleted_augroup = پوچ;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
ایستا صحیح current_augroup = AUGROUP_DEFAULT;

ایستا صحیح au_need_clean = FALSE;   // need to delete marked patterns

ایستا event_T event_name2nr(char_u *start, char_u **end);
ایستا string_T *event_nr2name(event_T event);
ایستا صحیح au_get_grouparg(char_u **argp);
ایستا صحیح do_autocmd_event(event_T event, char_u *pat, صحیح once, صحیح nested, char_u *cmd, صحیح forceit, صحیح group, صحیح flags);
ایستا صحیح apply_autocmds_group(event_T event, char_u *fname, char_u *fname_io, صحیح force, صحیح group, buf_T *buf, exarg_T *eap);
ایستا تهی auto_next_pat(AutoPatCmd_T *apc, صحیح stop_at_last);
ایستا صحیح au_find_group(char_u *name);

ایستا event_T	last_event;
ایستا صحیح	last_group;
ایستا صحیح	autocmd_blocked = 0;	// block all autocmds

    ایستا char_u *
get_deleted_augroup(تهی)
{
    اگر (deleted_augroup == پوچ)
	deleted_augroup = (char_u *)_("--Deleted--");
    بازگشت deleted_augroup;
}

/*
 * Show the autocommands for one AutoPat.
 */
    ایستا تهی
show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    // Check for "got_int" (here and at various places below), which is set
    // when "q" has been hit for the "--more--" prompt
    اگر (got_int)
	بازگشت;
    اگر (ap->pat == پوچ)		// pattern has been removed
	بازگشت;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    پیام_putchar('\n');
    اگر (got_int)
	برو theend;
    اگر (event != last_event || ap->group != last_group)
    {
	string_T    *event_name;

	اگر (ap->group != AUGROUP_DEFAULT)
	{
	    اگر (AUGROUP_NAME(ap->group) == پوچ)
		پیام_puts_attr((نویسه *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    وگرنه
		پیام_puts_attr((نویسه *)AUGROUP_NAME(ap->group), HL_ATTR(HLF_T));
	    پیام_puts("  ");
	}
	event_name = event_nr2name(event);
	پیام_puts_attr((نویسه *)event_name->string, HL_ATTR(HLF_T));
	last_event = event;
	last_group = ap->group;
	پیام_putchar('\n');
	اگر (got_int)
	    برو theend;
    }

    اگر (ap->pat == پوچ)
	برو theend;  // timer might have cleared the pattern or group

    پیام_col = 4;
    پیام_outtrans(ap->pat);

    برای (ac = ap->cmds; ac != پوچ; ac = ac->next)
    {
	اگر (ac->cmd == پوچ)		// skip removed commands
	    ادامه;

	اگر (پیام_col >= 14)
	    پیام_putchar('\n');
	پیام_col = 14;
	اگر (got_int)
	    برو theend;
	پیام_outtrans(ac->cmd);
#ifdef FEAT_EVAL
	اگر (p_verbose > 0)
	    last_set_پیام(ac->script_ctx);
#endif
	اگر (got_int)
	    برو theend;
	اگر (ac->next != پوچ)
	{
	    پیام_putchar('\n');
	    اگر (got_int)
		برو theend;
	}
    }

theend:
    --autocmd_busy;
}

/*
 * Mark an autocommand pattern for deletion.
 */
    ایستا تهی
au_remove_pat(AutoPat *ap)
{
    VIM_CLEAR(ap->pat);
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    ایستا تهی
au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    برای (ac = ap->cmds; ac != پوچ; ac = ac->next)
	VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

// Delete one command from an autocmd pattern.
ایستا تهی au_del_cmd(AutoCmd *ac)
{
    VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    ایستا تهی
au_cleanup(تهی)
{
    AutoPat	*ap, **prev_ap;
    AutoCmd	*ac, **prev_ac;
    event_T	event;

    اگر (autocmd_busy || !au_need_clean)
	بازگشت;

    // loop over all events
    برای (event = (event_T)0; (صحیح)event < رقم_EVENTS;
					    event = (event_T)((صحیح)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(first_autopat[(صحیح)event]);
	برای (ap = *prev_ap; ap != پوچ; ap = *prev_ap)
	{
	    صحیح has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    برای (ac = *prev_ac; ac != پوچ; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		اگر (ap->pat == پوچ || ac->cmd == پوچ)
		{
		    *prev_ac = ac->next;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		وگرنه
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->next);
		}
	    }

	    اگر (ap->pat != پوچ && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    اگر (ap->pat == پوچ)
	    {
		اگر (ap->next == پوچ)
		{
		    اگر (prev_ap == &(first_autopat[(صحیح)event]))
			last_autopat[(صحیح)event] = پوچ;
		    وگرنه
			// this depends on the "next" field being the first in
			// the struct
			last_autopat[(صحیح)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->next;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    وگرنه
		prev_ap = &(ap->next);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Called when buffer is freed, to remove/invalidate related buffer-local
 * autocmds.
 */
    تهی
aubuflocal_remove(buf_T *buf)
{
    AutoPat	    *ap;
    event_T	    event;
    AutoPatCmd_T    *apc;

    // invalidate currently executing autocommands
    برای (apc = active_apc_list; apc; apc = apc->next)
	اگر (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    برای (event = (event_T)0; (صحیح)event < رقم_EVENTS;
					    event = (event_T)((صحیح)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    اگر (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		اگر (p_verbose >= 6)
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
    ایستا صحیح
au_new_group(char_u *name)
{
    صحیح		i;

    i = au_find_group(name);
    اگر (i != AUGROUP_ERROR)
	بازگشت i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    برای (i = 0; i < augroups.ga_len; ++i)
	اگر (AUGROUP_NAME(i) == پوچ)
	    بشکن;
    اگر (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	بازگشت AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    اگر (AUGROUP_NAME(i) == پوچ)
	بازگشت AUGROUP_ERROR;
    اگر (i == augroups.ga_len)
	++augroups.ga_len;

    بازگشت i;
}

    ایستا تهی
au_del_group(char_u *name)
{
    صحیح		i;
    event_T	event;
    AutoPat	*ap;
    صحیح		in_use = FALSE;


    i = au_find_group(name);
    اگر (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_group_str), name);
	بازگشت;
    }
    اگر (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_group));
	بازگشت;
    }

    برای (event = (event_T)0; (صحیح)event < رقم_EVENTS;
	    event = (event_T)((صحیح)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    اگر (ap->group == i && ap->pat != پوچ)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		in_use = TRUE;
		event = رقم_EVENTS;
		بشکن;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    اگر (in_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    وگرنه
	AUGROUP_NAME(i) = پوچ;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    ایستا صحیح
au_find_group(char_u *name)
{
    صحیح	    i;

    برای (i = 0; i < augroups.ga_len; ++i)
	اگر (AUGROUP_NAME(i) != پوچ && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    بازگشت i;
    بازگشت AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    صحیح
au_has_group(char_u *name)
{
    بازگشت au_find_group(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    تهی
do_augroup(char_u *arg, صحیح del_group)
{
    صحیح	    i;

    اگر (del_group)
    {
	اگر (*arg == NUL)
	    emsg(_(e_argument_required));
	وگرنه
	    au_del_group(arg);
    }
    وگرنه اگر (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    وگرنه اگر (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_group(arg);
	اگر (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    وگرنه			    // ":aug": list the group names
    {
	پیام_start();
	برای (i = 0; i < augroups.ga_len; ++i)
	{
	    اگر (AUGROUP_NAME(i) != پوچ)
	    {
		پیام_puts((نویسه *)AUGROUP_NAME(i));
		پیام_puts("  ");
	    }
	}
	پیام_clr_eos();
	پیام_end();
    }
}

    تهی
autocmd_init(تهی)
{
    CLEAR_FIELD(aucmd_win);
}

#اگر defined(EXITFREE)
    تهی
free_all_autocmds(تهی)
{
    char_u	*s;

    برای (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(پوچ, (char_u *)"", TRUE);

    برای (صحیح i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_data))[i];
	اگر (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#endif

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    صحیح
is_aucmd_win(win_T *win)
{
    برای (صحیح i = 0; i < AUCMD_WIN_شمار; ++i)
	اگر (aucmd_win[i].auc_win_used && aucmd_win[i].auc_win == win)
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    ایستا event_T
event_name2nr(char_u *start, char_u **end)
{
    char_u	*p;
    keyvalue_T target;
    keyvalue_T *entry;
    ایستا keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_INDEX];
    ایستا keyvalue_T *bufread = &event_tab[BUFREAD_INDEX];

    // the event name ends with end of line, '|', a blank or a comma
    برای (p = start; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    target.کلید = 0;
    target.مقدار.string = start;
    target.مقدار.length = (size_t)(p - start);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    اگر (cmp_keyvalue_مقدار_ni(&target, bufnewfile) == 0)
	entry = bufnewfile;
    وگرنه اگر (cmp_keyvalue_مقدار_ni(&target, bufread) == 0)
	entry = bufread;
    وگرنه
	entry = (keyvalue_T *)bsearch(&target, &event_tab, رقم_EVENTS,
				اندازه(event_tab[0]), cmp_keyvalue_مقدار_ni);

    اگر (*p == ',')
	++p;
    *end = p;

    بازگشت (entry == پوچ) ? رقم_EVENTS : (event_T)abs(entry->کلید);
}

/*
 * Return the name for event "event".
 */
    ایستا string_T *
event_nr2name(event_T event)
{
    صحیح	    i;
    شمارش {CACHE_SIZE = 12};
    ایستا صحیح cache_tab[CACHE_SIZE];
    ایستا صحیح cache_last_index = -1;
    ایستا string_T unknown = STR_LITERAL_INIT("Unknown");

    اگر (cache_last_index < 0)
    {
	برای (i = 0; i < CACHE_SIZE; ++i)
	    cache_tab[i] = -1;
	cache_last_index = CACHE_SIZE - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    برای (i = cache_last_index; cache_tab[i] >= 0; )
    {
	اگر ((event_T)abs(event_tab[cache_tab[i]].کلید) == event)
	    بازگشت &event_tab[cache_tab[i]].مقدار;

	اگر (i == 0)
	    i = CACHE_SIZE - 1;
	وگرنه
	    --i;

	// are we back at the start?
	اگر (i == cache_last_index)
	    بشکن;
    }

    // look in the event table itself
    برای (i = 0; i < رقم_EVENTS; ++i)
    {
	اگر ((event_T)abs(event_tab[i].کلید) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    اگر (cache_last_index == CACHE_SIZE - 1)
		cache_last_index = 0;
	    وگرنه
		++cache_last_index;
	    cache_tab[cache_last_index] = i;
	    بازگشت &event_tab[i].مقدار;
	}
    }

    بازگشت &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    ایستا char_u *
find_end_event(
    char_u  *arg,
    صحیح	    have_group)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    اگر (*arg == '*')
    {
	اگر (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_star_str), arg);
	    بازگشت پوچ;
	}
	pat = arg + 1;
    }
    وگرنه
    {
	برای (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    اگر ((صحیح)event_name2nr(pat, &p) >= رقم_EVENTS)
	    {
		اگر (have_group)
		    semsg(_(e_no_such_event_str), pat);
		وگرنه
		    semsg(_(e_no_such_group_or_event_str), pat);
		بازگشت پوچ;
	    }
	}
    }
    بازگشت pat;
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
    صحیح		group;

    اگر (*arg == '|')
	بازگشت پوچ;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    group = au_get_grouparg(&arg);
    pat = group == AUGROUP_ERROR
			  ? پوچ : find_end_event(arg, group != AUGROUP_ALL);
    --emsg_off;
    اگر (pat == پوچ)
	بازگشت پوچ;

    pat = skipwhite(pat);
    اگر (*pat == NUL || *pat == '|')
	بازگشت پوچ;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    وقتی (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    برای (صحیح i = 0; i < 2; ++i)
    {
	اگر (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	اگر (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	اگر (!in_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    بازگشت *cmd == NUL ? پوچ : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    صحیح
event_ignored(event_T event, char_u *ei)
{
    صحیح ignored = FALSE;
    وقتی (*ei != NUL)
    {
	صحیح unignore = *ei == '-';
	ei += unignore;
	اگر (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].کلید <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	وگرنه اگر (event_name2nr(ei, &ei) == event)
	{
	    اگر (unignore)
		بازگشت FALSE;
	    وگرنه
		ignored = TRUE;
	}
    }

    بازگشت ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    صحیح
check_ei(char_u *ei)
{
    صحیح	win = ei != p_ei;

    وقتی (*ei)
    {
	اگر (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	وگرنه
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    اگر (event == رقم_EVENTS || (win && event_tab[event].کلید > 0))
		بازگشت FAIL;
	}
    }

    بازگشت OK;
}

#اگر defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(نویسه *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    size_t	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    اگر (save_ei == پوچ)
	بازگشت پوچ;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    اگر (new_ei == پوچ)
    {
	vim_free(save_ei);
	بازگشت پوچ;
    }

    اگر (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    وگرنه
	STRCPY(new_ei + p_ei_len, what);
    set_string_option_direct((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    بازگشت save_ei;
}

    تهی
au_event_restore(char_u *old_ei)
{
    اگر (old_ei != پوچ)
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
    تهی
do_autocmd(exarg_T *eap, char_u *arg_in, صحیح forceit)
{
    char_u	*arg = arg_in;
    char_u	*pat;
    char_u	*envpat = پوچ;
    char_u	*cmd;
    صحیح		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = پوچ;
    صحیح		nested = FALSE;
    صحیح		once = FALSE;
    صحیح		group;
    صحیح		i;
    صحیح		flags = 0;

    اگر (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	group = AUGROUP_ALL;	// no argument, use all groups
    }
    وگرنه
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	group = au_get_grouparg(&arg);
	اگر (arg == پوچ)	    // out of memory
	    بازگشت;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = find_end_event(arg, group != AUGROUP_ALL);
    اگر (pat == پوچ)
	بازگشت;

    pat = skipwhite(pat);
    اگر (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    وگرنه
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	وقتی (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	اگر (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	اگر (vim_strchr(pat, '$') != پوچ || vim_strchr(pat, '~') != پوچ)
	{
#ifdef BACKSLASH_IN_FILENAME
	    صحیح	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#endif
	    envpat = expand_env_save(pat);
#ifdef BACKSLASH_IN_FILENAME
	    p_ssl = p_ssl_save;
#endif
	    اگر (envpat != پوچ)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	برای (i = 0; i < 2; i++)
	{
	    اگر (*cmd == NUL)
		ادامه;

	    // Check for "++once" flag.
	    اگر (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		اگر (once)
		{
		    semsg(_(e_duplicate_argument_str), "++once");
		    برو err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    اگر ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		اگر (nested)
		{
		    semsg(_(e_duplicate_argument_str), "++nested");
		    برو err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    اگر (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		اگر (in_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_mean_plusplus_nested));
		    برو err_exit;
		}
		اگر (nested)
		{
		    semsg(_(e_duplicate_argument_str), "nested");
		    برو err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 6);
	    }
	}

	/*
	 * Find the start of the commands.
	 * Expand <sfile> in it.
	 */
	اگر (*cmd != NUL)
	{
	    اگر (eap != پوچ)
		// Read a {} block if it follows.
		cmd = may_get_cmd_block(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    اگر (cmd == پوچ)	    // some error
		برو err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    اگر (!forceit && *cmd == NUL)
	// Highlight title
	پیام_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    last_event = (event_T)-1;		// for listing the event name
    last_group = AUGROUP_ERROR;		// for listing the group name
    اگر (*arg == '*' || *arg == NUL || *arg == '|')
    {
	اگر (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	وگرنه
	    برای (event = (event_T)0; (صحیح)event < رقم_EVENTS;
					     event = (event_T)((صحیح)event + 1))
		اگر (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, group, flags) == FAIL)
		    بشکن;
    }
    وگرنه
    {
	وقتی (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    اگر (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, group, flags) == FAIL)
		بشکن;
    }

err_exit:
    اگر (cmd_need_free)
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
    ایستا صحیح
au_get_grouparg(char_u **argp)
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    صحیح		group = AUGROUP_ALL;

    برای (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    اگر (p <= arg)
	بازگشت AUGROUP_ALL;

    group_name = vim_strnsave(arg, p - arg);
    اگر (group_name == پوچ)		// out of memory
	بازگشت AUGROUP_ERROR;
    group = au_find_group(group_name);
    اگر (group == AUGROUP_ERROR)
	group = AUGROUP_ALL;	// no match, use all groups
    وگرنه
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(group_name);
    بازگشت group;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    ایستا صحیح
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    صحیح		once,
    صحیح		nested,
    char_u	*cmd,
    صحیح		forceit,
    صحیح		group,
    صحیح		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    صحیح		brace_level;
    char_u	*endpat;
    صحیح		findgroup;
    صحیح		allgroups;
    صحیح		patlen;
    صحیح		is_buflocal;
    صحیح		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    اگر (group == AUGROUP_ALL)
	findgroup = current_augroup;
    وگرنه
	findgroup = group;
    allgroups = (group == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    اگر (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    اگر (forceit)  // delete the AutoPat, if it's in the current group
	    {
		اگر (ap->group == findgroup)
		    au_remove_pat(ap);
	    }
	    وگرنه اگر (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    برای ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	برای (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    اگر (*endpat == '{')
		brace_level++;
	    وگرنه اگر (*endpat == '}')
		brace_level--;
	}
	اگر (pat == endpat)		// ignore single comma
	    ادامه;
	patlen = (صحیح)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	اگر (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    اگر (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    وگرنه اگر (patlen > 9 && pat[7] == '=')
	    {
		اگر (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		وگرنه اگر (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((نویسه *)pat + 8);
	    }
	}

	اگر (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((نویسه *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (صحیح)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	اگر (!forceit && *cmd != NUL && last_autopat[(صحیح)event] != پوچ)
	    prev_ap = &last_autopat[(صحیح)event];
	وگرنه
	    prev_ap = &first_autopat[(صحیح)event];
	وقتی ((ap = *prev_ap) != پوچ)
	{
	    اگر (ap->pat != پوچ)
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
		اگر ((allgroups || ap->group == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    اگر (forceit)
		    {
			اگر (*cmd != NUL && ap->next == پوچ)
			{
			    au_remove_cmds(ap);
			    بشکن;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    وگرنه اگر (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    وگرنه اگر (ap->next == پوچ)
			بشکن;
		}
	    }
	    prev_ap = &ap->next;
	}

	/*
	 * Add a new command.
	 */
	اگر (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    اگر (ap == پوچ)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		اگر (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == پوچ))
		{
		    semsg(_(e_buffer_nr_invalid_buffer_عدد), buflocal_nr);
		    بازگشت FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		اگر (ap == پوچ)
		    بازگشت FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		اگر (ap->pat == پوچ)
		{
		    vim_free(ap);
		    بازگشت FAIL;
		}

#ifdef FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		اگر (event == EVENT_MODECHANGED && !has_modechanged())
		    get_mode(last_mode);
#endif
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		اگر ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
			&& !(has_winscrolled() || has_winresized()))
		{
		    tabpage_T *save_curtab = curtab;
		    tabpage_T *tp;
		    FOR_ALL_TABPAGES(tp)
		    {
			unuse_tabpage(curtab);
			use_tabpage(tp);
			snapshot_windows_scroll_size();
		    }
		    unuse_tabpage(curtab);
		    use_tabpage(save_curtab);
		}

		اگر (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = پوچ;
		}
		وگرنه
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_to_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    اگر (reg_pat != پوچ)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    اگر (reg_pat == پوچ || ap->reg_prog == پوچ)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			بازگشت FAIL;
		    }
		}
		ap->cmds = پوچ;
		*prev_ap = ap;
		last_autopat[(صحیح)event] = ap;
		ap->next = پوچ;
		اگر (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		وگرنه
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    وقتی ((ac = *prev_ac) != پوچ)
		prev_ac = &ac->next;
	    ac = ALLOC_ONE(AutoCmd);
	    اگر (ac == پوچ)
		بازگشت FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    اگر (flags & UC_VIM9)
		ac->script_ctx.sc_version = SCRIPT_VERSION_VIM9;
#ifdef FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#endif
	    اگر (ac->cmd == پوچ)
	    {
		vim_free(ac);
		بازگشت FAIL;
	    }
	    ac->next = پوچ;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    بازگشت OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    صحیح
do_doautocmd(
    char_u	*arg_start,
    صحیح		do_پیام,	    // give message for no matching autocmds?
    صحیح		*did_something)
{
    char_u	*arg = arg_start;
    char_u	*fname;
    صحیح		nothing_done = TRUE;
    صحیح		group;

    اگر (did_something != پوچ)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    اگر (arg == پوچ)	    // out of memory
	بازگشت FAIL;

    اگر (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	بازگشت FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg, group != AUGROUP_ALL);
    اگر (fname == پوچ)
	بازگشت FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    وقتی (*arg && !ends_excmd(*arg) && !VIM_ISWHITE(*arg))
	اگر (apply_autocmds_group(event_name2nr(arg, &arg),
				      fname, پوچ, TRUE, group, curbuf, پوچ))
	    nothing_done = FALSE;

    اگر (nothing_done && do_پیام
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       )
	smsg(_("No matching autocommands: %s"), arg_start);
    اگر (did_something != پوچ)
	*did_something = !nothing_done;

#ifdef FEAT_EVAL
    بازگشت aborting() ? FAIL : OK;
#وگرنه
    بازگشت OK;
#endif
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    تهی
ex_doautoall(exarg_T *eap)
{
    صحیح		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    صحیح		call_do_modelines = check_nomodeline(&arg);
    صحیح		did_aucmd;

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
	اگر (buf->b_ml.ml_mfp == پوچ || buf == curbuf)
	    ادامه;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	اگر (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    بشکن;
	}

	set_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	اگر (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	اگر (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    بشکن;
	}
    }

    // Execute autocommands for the current buffer last.
    اگر (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	اگر (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    صحیح
check_nomodeline(char_u **argp)
{
    اگر (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	بازگشت FALSE;
    }
    بازگشت TRUE;
}

/*
 * Prepare for executing autocommands for (hidden) buffer "buf".
 * Search for a visible window containing the current buffer.  If there isn't
 * one then use an entry in "aucmd_win[]".
 * Set "curbuf" and "curwin" to match "buf".
 * When this fails "curbuf" is not equal "buf".
 */
    تهی
aucmd_prepbuf(
    aco_save_T	*aco,		// structure to save values in
    buf_T	*buf)		// new curbuf
{
    win_T	*win;
    صحیح		save_ea;
#ifdef FEAT_AUTOCHDIR
    صحیح		save_acd;
#endif
    صحیح		same_buffer = buf == curbuf;

    // Find a window that is for the new buffer
    اگر (same_buffer)		// be quick when buf is curbuf
	win = curwin;
    وگرنه
	FOR_ALL_WINDOWS(win)
	    اگر (win->w_buffer == buf)
		بشکن;

    // Allocate a window when needed.
    win_T *auc_win = پوچ;
    صحیح auc_idx = AUCMD_WIN_شمار;
    اگر (win == پوچ)
    {
	برای (auc_idx = 0; auc_idx < AUCMD_WIN_شمار; ++auc_idx)
	    اگر (!aucmd_win[auc_idx].auc_win_used)
	    {
		اگر (aucmd_win[auc_idx].auc_win == پوچ)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		اگر (auc_win != پوچ)
		    aucmd_win[auc_idx].auc_win_used = TRUE;
		بشکن;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	اگر (auc_win == پوچ)
	    بازگشت;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == پوچ ? 0 : prevwin->w_id;
#ifdef FEAT_JOB_CHANNEL
    اگر (bt_prompt(curbuf))
	aco->save_prompt_درج = curbuf->b_prompt_درج;
#endif

    اگر (win != پوچ)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    وگرنه
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
	curtab->tp_localdir = پوچ;
	aco->globaldir = globaldir;
	globaldir = پوچ;

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

	(تهی)win_split_ins(0, WSP_بالاترین | WSP_FORCE_ROOM, auc_win, 0, پوچ);
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
    اگر (!same_buffer)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_active = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    تهی
aucmd_restbuf(
    aco_save_T	*aco)		// structure holding saved values
{
    صحیح	    dummy;
    win_T   *save_curwin;

    اگر (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	block_autocmds();
	اگر (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		اگر (wp == awp)
		{
		    اگر (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(awp);
		    برو win_found;
		}
	    }
	}
win_found:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(تهی)winframe_remove(curwin, &dummy, پوچ, پوچ);
	win_remove(curwin, پوچ);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_used = FALSE;
	last_status(FALSE);	    // may need to remove last status line

	اگر (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_find_by_id(aco->save_curwin_id);
	اگر (save_curwin != پوچ)
	    curwin = save_curwin;
	وگرنه
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_window(curwin);
	اگر (bt_prompt(curbuf))
	    curbuf->b_prompt_درج = aco->save_prompt_درج;
#endif
	prevwin = win_find_by_id(aco->save_prevwin_id);
#ifdef FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#endif
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	اگر (awp->w_localdir != پوچ)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_active = aco->save_VIsual_active;
	check_cursor();
	اگر (curwin->w_topline > curbuf->b_ml.ml_سطر_شمار)
	{
	    curwin->w_topline = curbuf->b_ml.ml_سطر_شمار;
#ifdef FEAT_DIFF
	    curwin->w_topfill = 0;
#endif
	}
#اگر defined(FEAT_GUI)
	اگر (gui.in_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_چپ], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_راست], FALSE);
	    gui_may_update_scrollbars();
	}
#endif
    }
    وگرنه
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_find_by_id(aco->save_curwin_id);
	اگر (save_curwin != پوچ)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    اگر (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != پوچ)
	    {
#اگر defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		اگر (curwin->w_s == &curbuf->b_s)
		    curwin->w_s = &aco->new_curbuf.br_buf->b_s;
#endif
		--curbuf->b_nwindows;
		curbuf = aco->new_curbuf.br_buf;
		curwin->w_buffer = curbuf;
		++curbuf->b_nwindows;
	    }

	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	    prevwin = win_find_by_id(aco->save_prevwin_id);

	    // In case the autocommand moves the cursor to a position that
	    // does not exist in curbuf.
	    VIsual_active = aco->save_VIsual_active;
	    check_cursor();
	}
    }

    VIsual_active = aco->save_VIsual_active;
    check_cursor();	    // just in case lines got deleted
    اگر (VIsual_active)
	check_pos(curbuf, &VIsual);
}

ایستا صحیح	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    صحیح
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    صحیح		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    بازگشت apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, پوچ);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    صحیح
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    صحیح		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    بازگشت apply_autocmds_group(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    صحیح
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    صحیح		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    صحیح		*retval)    // pointer to caller's retval
{
    صحیح		did_cmd;

#ifdef FEAT_EVAL
    اگر (should_abort(*retval))
	بازگشت FALSE;
#endif

    did_cmd = apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, پوچ);
    اگر (did_cmd
#ifdef FEAT_EVAL
	    && aborting()
#endif
	    )
	*retval = FAIL;
    بازگشت did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    ایستا صحیح
has_cursorhold(تهی)
{
    بازگشت (first_autopat[(صحیح)(get_real_state() == MODE_NORMAL_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != پوچ);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    صحیح
trigger_cursorhold(تهی)
{
    صحیح		state;

    اگر (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_active())
    {
	state = get_real_state();
	اگر (state == MODE_NORMAL_BUSY || (state & MODE_درج) != 0)
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    صحیح
has_winresized(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_WINRESIZED] != پوچ);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    صحیح
has_winscrolled(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_WINSCROLLED] != پوچ);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    صحیح
has_cursormoved(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_CURSORMOVED] != پوچ);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    صحیح
has_cursormovedI(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_CURSORMOVEDI] != پوچ);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    صحیح
has_textchanged(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_TEXTCHANGED] != پوچ);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    صحیح
has_textchangedI(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_TEXTCHANGEDI] != پوچ);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    صحیح
has_textchangedP(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_TEXTCHANGEDP] != پوچ);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    صحیح
has_insertcharpre(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_INSERTCHARPRE] != پوچ);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    صحیح
has_keyinputpre(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_KEYINPUTPRE] != پوچ);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    صحیح
has_cmdundefined(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_CMDUNDEFINED] != پوچ);
}

#اگر defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    صحیح
has_textputpost(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_TEXTPUTPOST] != پوچ);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    صحیح
has_textputpre(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_TEXTPUTPRE] != پوچ);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    صحیح
has_textyankpost(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_TEXTYANKPOST] != پوچ);
}
#endif

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    صحیح
has_completechanged(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_COMPLETECHANGED] != پوچ);
}
#endif

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    صحیح
has_modechanged(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_MODECHANGED] != پوچ);
}
#endif

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    ایستا صحیح
apply_autocmds_group(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    صحیح		force,	     // when TRUE, ignore autocmd_busy
    صحیح		group,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap UNUSED) // command arguments
{
    char_u	*sfname = پوچ;	// short file name
    char_u	*tail;
    صحیح		save_changed;
    buf_T	*old_curbuf;
    صحیح		retval = FALSE;
    char_u	*save_autocmd_fname;
    صحیح		save_autocmd_fname_full;
    صحیح		save_autocmd_bufnr;
    char_u	*save_autocmd_match;
    صحیح		save_autocmd_busy;
    صحیح		save_autocmd_nested;
    ایستا صحیح	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ifdef FEAT_EVAL
    funccal_entry_T funccal_entry;
    char_u	*save_cmdarg;
    long	save_cmdbang;
#endif
    ایستا صحیح	filechangeshell_busy = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif
    صحیح		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    صحیح		save_کلید_Typed = کلید_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    اگر (event == رقم_EVENTS || first_autopat[(صحیح)event] == پوچ
	    || autocmd_blocked > 0)
	برو BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    اگر (autocmd_busy && !(force || autocmd_nested))
	برو BYPASS_AU;

#ifdef FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    اگر (aborting())
	برو BYPASS_AU;
#endif

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    اگر (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	برو BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    اگر (event_ignored(event, p_ei))
	برو BYPASS_AU;

    صحیح win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    اگر (buf == curbuf && event_tab[event].کلید <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    وگرنه اگر (buf != پوچ && event_tab[event].کلید <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    اگر (wp->w_buffer == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		بشکن;
	    }
    }
    اگر (win_ignore)
	برو BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    اگر (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	برو BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    اگر (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	برو BYPASS_AU;

    اگر (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_changed_شمار;

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
    اگر (fname_io == پوچ)
    {
	اگر (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = پوچ;
	وگرنه اگر (fname != پوچ && !ends_excmd(*fname))
	    autocmd_fname = fname;
	وگرنه اگر (buf != پوچ)
	    autocmd_fname = buf->b_ffname;
	وگرنه
	    autocmd_fname = پوچ;
    }
    وگرنه
	autocmd_fname = fname_io;
    اگر (autocmd_fname != پوچ)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_full = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    اگر (buf == پوچ)
	autocmd_bufnr = 0;
    وگرنه
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    اگر (fname == پوچ || *fname == NUL)
    {
	اگر (buf == پوچ)
	    fname = پوچ;
	وگرنه
	{
#ifdef FEAT_SYN_HL
	    اگر (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    وگرنه
#endif
		اگر (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		وگرنه
		{
		    اگر (buf->b_sfname != پوچ)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	اگر (fname == پوچ)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    وگرنه
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	اگر (event == EVENT_FILETYPE
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
	وگرنه
	    fname = FullName_save(fname, FALSE);
    }
    اگر (fname == پوچ)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	برو BYPASS_AU;
    }

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    اگر (sfname != پوچ)
	forward_slash(sfname);
    forward_slash(fname);
#endif

#ifdef VMS
    // remove version for correct match
    اگر (sfname != پوچ)
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
    estack_افزودن(ETYPE_AUCMD, پوچ, 0);
    ESTACK_CHECK_SETUP;

    save_current_sctx = current_sctx;

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	prof_child_enter(&wait_time); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_entry);
#endif

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    اگر (!autocmd_busy)
    {
	save_جستجو_patterns();
	اگر (!ins_compl_active())
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
    اگر (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    tail = gettail(fname);

    // Find first autocommand that matches
    CLEAR_FIELD(patcmd);
    patcmd.curpat = first_autopat[(صحیح)event];
    patcmd.group = group;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.tail = tail;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_next_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    اگر (patcmd.curpat != پوچ)
    {
	// add to active_apc_list
	patcmd.next = active_apc_list;
	active_apc_list = &patcmd;

#ifdef FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (long)get_vim_var_nr(VV_CMDBANG);
	اگر (eap != پوچ)
	{
	    save_cmdarg = set_cmdarg(eap, پوچ);
	    set_vim_var_nr(VV_CMDBANG, (long)eap->forceit);
	}
	وگرنه
	    save_cmdarg = پوچ;	// avoid gcc warning
#endif
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	برای (ap = patcmd.curpat; ap->next != پوچ; ap = ap->next)
	    ap->last = FALSE;
	ap->last = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	اگر (nesting == 1)
	    check_lnums(TRUE);
	وگرنه
	    check_lnums_nested(TRUE);

	صحیح save_did_emsg = did_emsg;
	صحیح save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(پوچ, getnextac, (تهی *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	set_pressedreturn(save_ex_pressedreturn);

	اگر (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ifdef FEAT_EVAL
	اگر (eap != پوچ)
	{
	    (تهی)set_cmdarg(پوچ, save_cmdarg);
	    set_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#endif
	// delete from active_apc_list
	اگر (active_apc_list == &patcmd)	    // just in case
	    active_apc_list = patcmd.next;
    }

    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_برداشت();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_full = save_autocmd_fname_full;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_match = save_autocmd_match;
    current_sctx = save_current_sctx;
#ifdef FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif
    کلید_Typed = save_کلید_Typed;
    vim_free(fname);
    vim_free(sfname);
    --nesting;		// see matching increment above

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.  Free any buffers in the au_pending_free_buf list and
     * free any windows in the au_pending_free_win list.
     */
    اگر (!autocmd_busy)
    {
	restore_جستجو_patterns();
	اگر (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	وقتی (au_pending_free_buf != پوچ)
	{
	    buf_T *b = au_pending_free_buf->b_next;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	وقتی (au_pending_free_win != پوچ)
	{
	    win_T *w = au_pending_free_win->w_next;

	    vim_free(au_pending_free_win);
	    au_pending_free_win = w;
	}
    }

    /*
     * Some events don't set or reset the Changed flag.
     * Check if still in the same buffer!
     */
    اگر (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	اگر (curbuf->b_changed != save_changed)
	    need_maketitle = TRUE;
	curbuf->b_changed = save_changed;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    اگر (event == EVENT_BUFWIPEOUT && buf != پوچ)
	aubuflocal_remove(buf);

    اگر (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    بازگشت retval;
}

#ifdef FEAT_EVAL
ایستا char_u	*old_termresponse = پوچ;
ایستا char_u	*old_termu7resp = پوچ;
ایستا char_u	*old_termblinkresp = پوچ;
ایستا char_u	*old_termrbgresp = پوچ;
ایستا char_u	*old_termrfgresp = پوچ;
ایستا char_u	*old_termstyleresp = پوچ;
#endif

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    تهی
block_autocmds(تهی)
{
#ifdef FEAT_EVAL
    // Remember the value of v:termresponse.
    اگر (autocmd_blocked == 0)
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

    تهی
unblock_autocmds(تهی)
{
    --autocmd_blocked;

#ifdef FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    اگر (autocmd_blocked == 0)
    {
	اگر (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, پوچ, پوچ, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", پوچ, FALSE, curbuf);
	}
	اگر (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", پوچ, FALSE, curbuf);
	}
	اگر (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", پوچ, FALSE, curbuf);
	}
	اگر (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", پوچ, FALSE, curbuf);
	}
	اگر (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", پوچ, FALSE, curbuf);
	}
	اگر (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", پوچ, FALSE, curbuf);
	}
    }
#endif
}

    صحیح
is_autocmd_blocked(تهی)
{
    بازگشت autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    ایستا تهی
auto_next_pat(
    AutoPatCmd_T *apc,
    صحیح		stop_at_last)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*entry;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(entry->es_name);
    entry->es_info.aucmd = پوچ;

    برای (ap = apc->curpat; ap != پوچ && !got_int; ap = ap->next)
    {
	apc->curpat = پوچ;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	اگر (ap->pat != پوچ && ap->cmds != پوچ
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
	    // execution-condition
	    اگر (ap->buflocal_nr == 0
		    ? (match_file_pat(پوچ, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->tail, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		نویسه	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->length + ap->patlen + 1);
		اگر (namep != پوچ)
		{
		    sprintf((نویسه *)namep, fmt, (نویسه *)event_name->string, (نویسه *)ap->pat);
		    اگر (p_verbose >= 8)
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
		برای (cp = ap->cmds; cp->next != پوچ; cp = cp->next)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    سطر_breakcheck();
	    اگر (apc->curpat != پوچ)	    // found a match
		بشکن;
	}
	اگر (stop_at_last && ap->last)
	    بشکن;
    }
}

#اگر defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    بازگشت &acp->script_ctx;
}
#endif

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	صحیح c UNUSED,
	تهی *cookie,
	صحیح indent UNUSED,
	getline_opt_T options UNUSED)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    اگر (acp->curpat == پوچ)
	بازگشت پوچ;

    // repeat until we find an autocommand to execute
    برای (;;)
    {
	// skip removed commands
	وقتی (acp->nextcmd != پوچ && acp->nextcmd->cmd == پوچ)
	    اگر (acp->nextcmd->last)
		acp->nextcmd = پوچ;
	    وگرنه
		acp->nextcmd = acp->nextcmd->next;

	اگر (acp->nextcmd != پوچ)
	    بشکن;

	// at end of commands, find next pattern that matches
	اگر (acp->curpat->last)
	    acp->curpat = پوچ;
	وگرنه
	    acp->curpat = acp->curpat->next;
	اگر (acp->curpat != پوچ)
	    auto_next_pat(acp, TRUE);
	اگر (acp->curpat == پوچ)
	    بازگشت پوچ;
    }

    ac = acp->nextcmd;

    اگر (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	پیام_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    اگر (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    اگر (ac->last)
	acp->nextcmd = پوچ;
    وگرنه
	acp->nextcmd = ac->next;
    بازگشت retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    ایستا صحیح
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_full)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*tail = gettail(sfname);
    صحیح		retval = FALSE;

    اگر (first_autopat[(صحیح)event] == پوچ)
	بازگشت FALSE;

    fname = is_full ? vim_strsave(sfname) : FullName_save(sfname, FALSE);
    اگر (fname == پوچ)
	بازگشت FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    اگر (sfname != پوچ)
	forward_slash(sfname);
    forward_slash(fname);
#endif

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	اگر (ap->pat != پوچ && ap->cmds != پوچ
	      && (ap->buflocal_nr == 0
		? match_file_pat(پوچ, &ap->reg_prog,
					  fname, sfname, tail, ap->allow_dirs)
		: buf != پوچ && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    بشکن;
	}

    vim_free(fname);
#ifdef BACKSLASH_IN_FILENAME
    vim_free(sfname);
#endif

    بازگشت retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    صحیح
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    بازگشت has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    صحیح
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    بازگشت has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx == augroups.ga_len)		// add "END" add the end
	بازگشت (char_u *)"END";
    اگر (idx < 0 || idx >= augroups.ga_len)	// end of list
	بازگشت پوچ;
    اگر (AUGROUP_NAME(idx) == پوچ || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	بازگشت (char_u *)"";
    بازگشت AUGROUP_NAME(idx);		// return a name
}

ایستا صحیح include_groups = FALSE;

    char_u  *
set_context_in_autocmd(
    expand_T	*xp,
    char_u	*arg,
    صحیح		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    صحیح		group;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    group = au_get_grouparg(&arg);
    اگر (group == AUGROUP_ERROR)
	بازگشت پوچ;
    // If there only is a group name that's what we expand.
    اگر (*arg == NUL && group != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	group = AUGROUP_ALL;
    }

    // skip over event name
    برای (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	اگر (*p == ',')
	    arg = p + 1;
    اگر (*p == NUL)
    {
	اگر (group == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_pattern = arg;
	بازگشت پوچ;
    }

    // skip over pattern
    arg = skipwhite(p);
    وقتی (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    اگر (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	برای (صحیح i = 0; i < 2; i++)
	{
	    اگر (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    اگر (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    اگر (!in_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	بازگشت arg;			    // expand (next) command
    }

    اگر (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    وگرنه
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    بازگشت پوچ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp UNUSED, صحیح idx)
{
    صحیح i;

    اگر (idx < augroups.ga_len)		// First list group names, if wanted
    {
	اگر (!include_groups || AUGROUP_NAME(idx) == پوچ
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    بازگشت (char_u *)"";	// skip deleted entries
	بازگشت AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    اگر (i < 0 || i >= رقم_EVENTS)
	بازگشت پوچ;

    بازگشت event_tab[i].مقدار.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_group(expand_T *xp UNUSED, صحیح idx, صحیح win)
{
    اگر (idx < 0 || idx >= رقم_EVENTS)
	بازگشت پوچ;

    اگر (!win)
	بازگشت event_tab[idx].مقدار.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    صحیح j = 0;
    برای (صحیح i = 0; i < رقم_EVENTS; ++i)
    {
	j += event_tab[i].کلید <= 0;
	اگر (j == idx + 1)
	    بازگشت event_tab[i].مقدار.string;
    }

    بازگشت پوچ;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    صحیح
has_tabclosedpre(تهی)
{
    بازگشت (first_autopat[(صحیح)EVENT_TABCLOSEDPRE] != پوچ);
}

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    صحیح
autocmd_supported(char_u *name)
{
    char_u *p;

    بازگشت (event_name2nr(name, &p) != رقم_EVENTS);
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
    صحیح
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*pattern = پوچ;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = پوچ;
    صحیح		group;
    صحیح		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    اگر (arg_save == پوچ)
	بازگشت FALSE;
    p = vim_strchr(arg_save, '#');
    اگر (p != پوچ)
	*p++ = NUL;

    // First, look for an autocmd group name
    group = au_find_group(arg_save);
    اگر (group == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	group = AUGROUP_ALL;
	event_name = arg_save;
    }
    وگرنه
    {
	اگر (p == پوچ)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    برو theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	اگر (p != پوچ)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    pattern = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    اگر (event == رقم_EVENTS)
	برو theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = first_autopat[(صحیح)event];
    اگر (ap == پوچ)
	برو theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    اگر (pattern != پوچ && STRICMP(pattern, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    برای ( ; ap != پوچ; ap = ap->next)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	اگر (ap->pat != پوچ && ap->cmds != پوچ
	    && (group == AUGROUP_ALL || ap->group == group)
	    && (pattern == پوچ
		|| (buflocal_buf == پوچ
		    ? fnamecmp(ap->pat, pattern) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    بشکن;
	}

theend:
    vim_free(arg_save);
    بازگشت retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    ایستا تهی
autocmd_add_or_delete(typval_T *argvars, typval_T *rettv, صحیح delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = پوچ;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*group_name = پوچ;
    صحیح		group;
    char_u	*pat = پوچ;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = پوچ;
    char_u	*end;
    صحیح		once;
    صحیح		nested;
    صحیح		replace;		// replace the cmd for a group/event
    صحیح		retval = VVAL_TRUE;
    صحیح		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = VVAL_FALSE;

    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
	بازگشت;

    aucmd_list = argvars[0].vval.v_list;
    اگر (aucmd_list == پوچ)
	بازگشت;

    FOR_ALL_LIST_اقلام(aucmd_list, li)
    {
	VIM_CLEAR(group_name);
	VIM_CLEAR(cmd);
	event_name = پوچ;
	event_list = پوچ;
	pat = پوچ;
	pat_list = پوچ;

	اگر (li->li_tv.v_type != VAR_DICT)
	    ادامه;

	event_dict = li->li_tv.vval.v_dict;
	اگر (event_dict == پوچ)
	    ادامه;

	di = dict_find(event_dict, (char_u *)"event", -1);
	اگر (di != پوچ)
	{
	    اگر (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		اگر (event_name == پوچ)
		{
		    emsg(_(e_string_required));
		    ادامه;
		}
	    }
	    وگرنه اگر (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		اگر (event_list == پوچ)
		{
		    emsg(_(e_list_required));
		    ادامه;
		}
	    }
	    وگرنه
	    {
		emsg(_(e_string_or_list_expected));
		ادامه;
	    }
	}

	group_name = dict_get_string(event_dict, "group", TRUE);
	اگر (group_name == پوچ || *group_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    group = current_augroup;
	وگرنه
	{
	    group = au_find_group(group_name);
	    اگر (group == AUGROUP_ERROR)
	    {
		اگر (delete)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    بشکن;
		}
		// group is not found, create it now
		group = au_new_group(group_name);
		اگر (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    بشکن;
		}

		current_augroup = group;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	اگر (dict_has_کلید(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_عدد_def(event_dict, "bufnr", -1);
	    اگر (bnum == -1)
		ادامه;

	    vim_snprintf((نویسه *)IObuff, IOSIZE, "<buffer=%d>", (صحیح)bnum);
	    pat = IObuff;
	}
	وگرنه
	{
	    di = dict_find(event_dict, (char_u *)"pattern", -1);
	    اگر (di != پوچ)
	    {
		اگر (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    اگر (pat == پوچ)
		    {
			emsg(_(e_string_required));
			ادامه;
		    }
		}
		وگرنه اگر (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    اگر (pat_list == پوچ)
		    {
			emsg(_(e_list_required));
			ادامه;
		    }
		}
		وگرنه
		{
		    emsg(_(e_string_or_list_expected));
		    ادامه;
		}
	    }
	    وگرنه اگر (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	اگر (cmd == پوچ)
	{
	    اگر (delete)
		cmd = vim_strsave((char_u *)"");
	    وگرنه
		ادامه;
	}

	اگر (delete && (event_name == پوچ
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    برای (event = (event_T)0; (صحیح)event < رقم_EVENTS;
		    event = (event_T)((صحیح)event + 1))
	    {
		اگر (do_autocmd_event(event, pat, once, nested, cmd, delete,
							group, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    بشکن;
		}
	    }
	}
	وگرنه
	{
	    char_u *p = پوچ;

	    eli = پوچ;
	    end = پوچ;
	    وقتی (TRUE)
	    {
		اگر (event_list != پوچ)
		{
		    اگر (eli == پوچ)
			eli = event_list->lv_first;
		    وگرنه
			eli = eli->li_next;
		    اگر (eli == پوچ)
			بشکن;
		    اگر (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == پوچ)
		    {
			emsg(_(e_string_required));
			بشکن;
		    }
		}
		وگرنه
		{
		    اگر (p == پوچ)
			p = event_name;
		    اگر (p == پوچ || *p == NUL)
			بشکن;
		}

		event = event_name2nr(p, &end);
		اگر (event == رقم_EVENTS || *end != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    بشکن;
		}
		اگر (pat != پوچ)
		{
		    اگر (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, group, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			بشکن;
		    }
		}
		وگرنه اگر (pat_list != پوچ)
		{
		    FOR_ALL_LIST_اقلام(pat_list, pli)
		    {
			اگر (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == پوچ)
			{
			    emsg(_(e_string_required));
			    ادامه;
			}
			اگر (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, group, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    بشکن;
			}
		    }
		    اگر (retval == VVAL_FALSE)
			بشکن;
		}
		اگر (event_name != پوچ)
		    p = end;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	اگر (delete && group_name != پوچ &&
		(event_name == پوچ || event_name[0] == NUL)
		&& (pat == پوچ || pat[0] == NUL)
		&& (cmd == پوچ || cmd[0] == NUL))
	    au_del_group(group_name);
    }

    VIM_CLEAR(group_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_عدد = retval;
}

/*
 * autocmd_add() function
 */
    تهی
f_autocmd_add(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    تهی
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    تهی
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = رقم_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = پوچ;
    char_u	*name = پوچ;
    صحیح		group = AUGROUP_ALL;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;
    اگر (check_for_opt_dict_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	اگر (dict_has_کلید(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    اگر (name == پوچ)
		بازگشت;

	    اگر (*name == NUL)
		group = AUGROUP_DEFAULT;
	    وگرنه
	    {
		group = au_find_group(name);
		اگر (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), name);
		    vim_free(name);
		    بازگشت;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	اگر (dict_has_کلید(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    اگر (name == پوچ)
		بازگشت;

	    اگر (name[0] == '*' && name[1] == NUL)
		event_arg = رقم_EVENTS;
	    وگرنه
	    {
		keyvalue_T target;
		keyvalue_T *entry;

		target.کلید = 0;
		target.مقدار.string = name;
		target.مقدار.length = STRLEN(target.مقدار.string);
		entry = (keyvalue_T *)bsearch(&target, &event_tab,
		    رقم_EVENTS, اندازه(event_tab[0]), cmp_keyvalue_مقدار_ni);
		اگر (entry == پوچ)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    بازگشت;
		}
		event_arg = (event_T)abs(entry->کلید);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	اگر (dict_has_کلید(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    اگر (pat == پوچ)
		بازگشت;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    برای (event = (event_T)0; (صحیح)event < رقم_EVENTS;
	    event = (event_T)((صحیح)event + 1))
    {
	string_T    *event_name;

	اگر (event_arg != رقم_EVENTS && event != event_arg)
	    ادامه;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	group_name;

	    اگر (ap->pat == پوچ)		// pattern has been removed
		ادامه;

	    اگر (group != AUGROUP_ALL && group != ap->group)
		ادامه;

	    اگر (pat != پوچ && STRCMP(pat, ap->pat) != 0)
		ادامه;

	    group_name.string = get_augroup_name(پوچ, ap->group);
	    اگر (group_name.string == پوچ)
		STR_LITERAL_SET(group_name, "");
	    وگرنه
		group_name.length = STRLEN(group_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    برای (ac = ap->cmds; ac != پوچ; ac = ac->next)
	    {
		event_dict = dict_alloc();
		اگر (event_dict == پوچ
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    بازگشت;
		}

		اگر (dict_add_string_len(event_dict, "event",
			event_name->string, (صحیح)event_name->length) == FAIL
			|| dict_add_string_len(event_dict, "group",
			    group_name.string, (صحیح)group_name.length) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_add_عدد(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_add_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_add_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_add_bool(event_dict, "once", ac->once) == FAIL
			|| dict_add_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    بازگشت;
		}
	    }
	}
    }

    vim_free(pat);
}

#endif
