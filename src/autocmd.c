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

#ضمن "vim.h"

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
تعريفنوع بنية AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    حرف	    once;		// "One shot": removed after execution
    حرف	    nested;		// If autocommands nest here.
    حرف	    last;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    بنية AutoCmd  *next;		// next AutoCmd in list
} AutoCmd;

تعريفنوع بنية AutoPat
{
    بنية AutoPat  *next;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    صحيح		    group;		// group ID
    صحيح		    patlen;		// strlen() of pat
    صحيح		    buflocal_nr;	// !=0 for buffer-local AutoPat
    حرف	    allow_dirs;		// Pattern may match whole path
    حرف	    last;		// last pattern for apply_autocmds()
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
ساكن keyvalue_T event_tab[رقم_EVENTS] = {
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

ساكن AutoPat *first_autopat[رقم_EVENTS] = { عدم };
ساكن AutoPat *last_autopat[رقم_EVENTS] = { عدم };

#define AUGROUP_DEFAULT    (-1)	    // default autocmd group
#define AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#define AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
بنية AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    صحيح		group;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*tail;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    صحيح		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *next;		// chain of active apc-s for auto-invalidation
};

ساكن AutoPatCmd_T *active_apc_list = عدم; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#define FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    لكل ((ap) = first_autopat[(صحيح)(event)]; (ap) != عدم; (ap) = (ap)->next)

/*
 * augroups stores a list of autocmd group names.
 */
ساكن garray_T augroups = {0, 0, حجم(char_u *), 10, عدم};
#define AUGROUP_NAME(i) (((char_u **)augroups.ga_data)[i])
// use get_deleted_augroup() to get this
ساكن char_u *deleted_augroup = عدم;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
ساكن صحيح current_augroup = AUGROUP_DEFAULT;

ساكن صحيح au_need_clean = FALSE;   // need to delete marked patterns

ساكن event_T event_name2nr(char_u *start, char_u **end);
ساكن string_T *event_nr2name(event_T event);
ساكن صحيح au_get_grouparg(char_u **argp);
ساكن صحيح do_autocmd_event(event_T event, char_u *pat, صحيح once, صحيح nested, char_u *cmd, صحيح forceit, صحيح group, صحيح flags);
ساكن صحيح apply_autocmds_group(event_T event, char_u *fname, char_u *fname_io, صحيح force, صحيح group, buf_T *buf, exarg_T *eap);
ساكن فراغ auto_next_pat(AutoPatCmd_T *apc, صحيح stop_at_last);
ساكن صحيح au_find_group(char_u *name);

ساكن event_T	last_event;
ساكن صحيح	last_group;
ساكن صحيح	autocmd_blocked = 0;	// block all autocmds

    ساكن char_u *
get_deleted_augroup(فراغ)
{
    إذا (deleted_augroup == عدم)
	deleted_augroup = (char_u *)_("--Deleted--");
    إرجاع deleted_augroup;
}

/*
 * Show the autocommands for one AutoPat.
 */
    ساكن فراغ
show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    // Check for "got_int" (here and at various places below), which is set
    // when "q" has been hit for the "--more--" prompt
    إذا (got_int)
	إرجاع;
    إذا (ap->pat == عدم)		// pattern has been removed
	إرجاع;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    رسالة_putchar('\n');
    إذا (got_int)
	اذهب theend;
    إذا (event != last_event || ap->group != last_group)
    {
	string_T    *event_name;

	إذا (ap->group != AUGROUP_DEFAULT)
	{
	    إذا (AUGROUP_NAME(ap->group) == عدم)
		رسالة_puts_attr((حرف *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    وإلا
		رسالة_puts_attr((حرف *)AUGROUP_NAME(ap->group), HL_ATTR(HLF_T));
	    رسالة_puts("  ");
	}
	event_name = event_nr2name(event);
	رسالة_puts_attr((حرف *)event_name->string, HL_ATTR(HLF_T));
	last_event = event;
	last_group = ap->group;
	رسالة_putchar('\n');
	إذا (got_int)
	    اذهب theend;
    }

    إذا (ap->pat == عدم)
	اذهب theend;  // timer might have cleared the pattern or group

    رسالة_col = 4;
    رسالة_outtrans(ap->pat);

    لكل (ac = ap->cmds; ac != عدم; ac = ac->next)
    {
	إذا (ac->cmd == عدم)		// skip removed commands
	    استمر;

	إذا (رسالة_col >= 14)
	    رسالة_putchar('\n');
	رسالة_col = 14;
	إذا (got_int)
	    اذهب theend;
	رسالة_outtrans(ac->cmd);
#ifdef FEAT_EVAL
	إذا (p_verbose > 0)
	    last_set_رسالة(ac->script_ctx);
#endif
	إذا (got_int)
	    اذهب theend;
	إذا (ac->next != عدم)
	{
	    رسالة_putchar('\n');
	    إذا (got_int)
		اذهب theend;
	}
    }

theend:
    --autocmd_busy;
}

/*
 * Mark an autocommand pattern for deletion.
 */
    ساكن فراغ
au_remove_pat(AutoPat *ap)
{
    VIM_CLEAR(ap->pat);
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    ساكن فراغ
au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    لكل (ac = ap->cmds; ac != عدم; ac = ac->next)
	VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

// Delete one command from an autocmd pattern.
ساكن فراغ au_del_cmd(AutoCmd *ac)
{
    VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    ساكن فراغ
au_cleanup(فراغ)
{
    AutoPat	*ap, **prev_ap;
    AutoCmd	*ac, **prev_ac;
    event_T	event;

    إذا (autocmd_busy || !au_need_clean)
	إرجاع;

    // loop over all events
    لكل (event = (event_T)0; (صحيح)event < رقم_EVENTS;
					    event = (event_T)((صحيح)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(first_autopat[(صحيح)event]);
	لكل (ap = *prev_ap; ap != عدم; ap = *prev_ap)
	{
	    صحيح has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    لكل (ac = *prev_ac; ac != عدم; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		إذا (ap->pat == عدم || ac->cmd == عدم)
		{
		    *prev_ac = ac->next;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		وإلا
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->next);
		}
	    }

	    إذا (ap->pat != عدم && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    إذا (ap->pat == عدم)
	    {
		إذا (ap->next == عدم)
		{
		    إذا (prev_ap == &(first_autopat[(صحيح)event]))
			last_autopat[(صحيح)event] = عدم;
		    وإلا
			// this depends on the "next" field being the first in
			// the struct
			last_autopat[(صحيح)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->next;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    وإلا
		prev_ap = &(ap->next);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Called when buffer is freed, to remove/invalidate related buffer-local
 * autocmds.
 */
    فراغ
aubuflocal_remove(buf_T *buf)
{
    AutoPat	    *ap;
    event_T	    event;
    AutoPatCmd_T    *apc;

    // invalidate currently executing autocommands
    لكل (apc = active_apc_list; apc; apc = apc->next)
	إذا (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    لكل (event = (event_T)0; (صحيح)event < رقم_EVENTS;
					    event = (event_T)((صحيح)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    إذا (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		إذا (p_verbose >= 6)
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
    ساكن صحيح
au_new_group(char_u *name)
{
    صحيح		i;

    i = au_find_group(name);
    إذا (i != AUGROUP_ERROR)
	إرجاع i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    لكل (i = 0; i < augroups.ga_len; ++i)
	إذا (AUGROUP_NAME(i) == عدم)
	    اكسر;
    إذا (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	إرجاع AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    إذا (AUGROUP_NAME(i) == عدم)
	إرجاع AUGROUP_ERROR;
    إذا (i == augroups.ga_len)
	++augroups.ga_len;

    إرجاع i;
}

    ساكن فراغ
au_del_group(char_u *name)
{
    صحيح		i;
    event_T	event;
    AutoPat	*ap;
    صحيح		in_use = FALSE;


    i = au_find_group(name);
    إذا (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_group_str), name);
	إرجاع;
    }
    إذا (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_group));
	إرجاع;
    }

    لكل (event = (event_T)0; (صحيح)event < رقم_EVENTS;
	    event = (event_T)((صحيح)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    إذا (ap->group == i && ap->pat != عدم)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		in_use = TRUE;
		event = رقم_EVENTS;
		اكسر;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    إذا (in_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    وإلا
	AUGROUP_NAME(i) = عدم;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    ساكن صحيح
au_find_group(char_u *name)
{
    صحيح	    i;

    لكل (i = 0; i < augroups.ga_len; ++i)
	إذا (AUGROUP_NAME(i) != عدم && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    إرجاع i;
    إرجاع AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    صحيح
au_has_group(char_u *name)
{
    إرجاع au_find_group(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    فراغ
do_augroup(char_u *arg, صحيح del_group)
{
    صحيح	    i;

    إذا (del_group)
    {
	إذا (*arg == NUL)
	    emsg(_(e_argument_required));
	وإلا
	    au_del_group(arg);
    }
    وإلا إذا (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    وإلا إذا (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_group(arg);
	إذا (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    وإلا			    // ":aug": list the group names
    {
	رسالة_start();
	لكل (i = 0; i < augroups.ga_len; ++i)
	{
	    إذا (AUGROUP_NAME(i) != عدم)
	    {
		رسالة_puts((حرف *)AUGROUP_NAME(i));
		رسالة_puts("  ");
	    }
	}
	رسالة_clr_eos();
	رسالة_end();
    }
}

    فراغ
autocmd_init(فراغ)
{
    CLEAR_FIELD(aucmd_win);
}

#إذا defined(EXITFREE)
    فراغ
free_all_autocmds(فراغ)
{
    char_u	*s;

    لكل (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(عدم, (char_u *)"", TRUE);

    لكل (صحيح i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_data))[i];
	إذا (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#endif

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    صحيح
is_aucmd_win(win_T *win)
{
    لكل (صحيح i = 0; i < AUCMD_WIN_تعداد; ++i)
	إذا (aucmd_win[i].auc_win_used && aucmd_win[i].auc_win == win)
	    إرجاع TRUE;
    إرجاع FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    ساكن event_T
event_name2nr(char_u *start, char_u **end)
{
    char_u	*p;
    keyvalue_T target;
    keyvalue_T *entry;
    ساكن keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_INDEX];
    ساكن keyvalue_T *bufread = &event_tab[BUFREAD_INDEX];

    // the event name ends with end of line, '|', a blank or a comma
    لكل (p = start; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    target.مفتاح = 0;
    target.قيمة.string = start;
    target.قيمة.length = (size_t)(p - start);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    إذا (cmp_keyvalue_قيمة_ni(&target, bufnewfile) == 0)
	entry = bufnewfile;
    وإلا إذا (cmp_keyvalue_قيمة_ni(&target, bufread) == 0)
	entry = bufread;
    وإلا
	entry = (keyvalue_T *)bsearch(&target, &event_tab, رقم_EVENTS,
				حجم(event_tab[0]), cmp_keyvalue_قيمة_ni);

    إذا (*p == ',')
	++p;
    *end = p;

    إرجاع (entry == عدم) ? رقم_EVENTS : (event_T)abs(entry->مفتاح);
}

/*
 * Return the name for event "event".
 */
    ساكن string_T *
event_nr2name(event_T event)
{
    صحيح	    i;
    تعداد {CACHE_SIZE = 12};
    ساكن صحيح cache_tab[CACHE_SIZE];
    ساكن صحيح cache_last_index = -1;
    ساكن string_T unknown = STR_LITERAL_INIT("Unknown");

    إذا (cache_last_index < 0)
    {
	لكل (i = 0; i < CACHE_SIZE; ++i)
	    cache_tab[i] = -1;
	cache_last_index = CACHE_SIZE - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    لكل (i = cache_last_index; cache_tab[i] >= 0; )
    {
	إذا ((event_T)abs(event_tab[cache_tab[i]].مفتاح) == event)
	    إرجاع &event_tab[cache_tab[i]].قيمة;

	إذا (i == 0)
	    i = CACHE_SIZE - 1;
	وإلا
	    --i;

	// are we back at the start?
	إذا (i == cache_last_index)
	    اكسر;
    }

    // look in the event table itself
    لكل (i = 0; i < رقم_EVENTS; ++i)
    {
	إذا ((event_T)abs(event_tab[i].مفتاح) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    إذا (cache_last_index == CACHE_SIZE - 1)
		cache_last_index = 0;
	    وإلا
		++cache_last_index;
	    cache_tab[cache_last_index] = i;
	    إرجاع &event_tab[i].قيمة;
	}
    }

    إرجاع &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    ساكن char_u *
find_end_event(
    char_u  *arg,
    صحيح	    have_group)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    إذا (*arg == '*')
    {
	إذا (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_star_str), arg);
	    إرجاع عدم;
	}
	pat = arg + 1;
    }
    وإلا
    {
	لكل (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    إذا ((صحيح)event_name2nr(pat, &p) >= رقم_EVENTS)
	    {
		إذا (have_group)
		    semsg(_(e_no_such_event_str), pat);
		وإلا
		    semsg(_(e_no_such_group_or_event_str), pat);
		إرجاع عدم;
	    }
	}
    }
    إرجاع pat;
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
    صحيح		group;

    إذا (*arg == '|')
	إرجاع عدم;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    group = au_get_grouparg(&arg);
    pat = group == AUGROUP_ERROR
			  ? عدم : find_end_event(arg, group != AUGROUP_ALL);
    --emsg_off;
    إذا (pat == عدم)
	إرجاع عدم;

    pat = skipwhite(pat);
    إذا (*pat == NUL || *pat == '|')
	إرجاع عدم;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    بينما (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    لكل (صحيح i = 0; i < 2; ++i)
    {
	إذا (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	إذا (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	إذا (!in_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    إرجاع *cmd == NUL ? عدم : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    صحيح
event_ignored(event_T event, char_u *ei)
{
    صحيح ignored = FALSE;
    بينما (*ei != NUL)
    {
	صحيح unignore = *ei == '-';
	ei += unignore;
	إذا (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].مفتاح <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	وإلا إذا (event_name2nr(ei, &ei) == event)
	{
	    إذا (unignore)
		إرجاع FALSE;
	    وإلا
		ignored = TRUE;
	}
    }

    إرجاع ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    صحيح
check_ei(char_u *ei)
{
    صحيح	win = ei != p_ei;

    بينما (*ei)
    {
	إذا (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	وإلا
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    إذا (event == رقم_EVENTS || (win && event_tab[event].مفتاح > 0))
		إرجاع FAIL;
	}
    }

    إرجاع OK;
}

#إذا defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(حرف *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    size_t	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    إذا (save_ei == عدم)
	إرجاع عدم;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    إذا (new_ei == عدم)
    {
	vim_free(save_ei);
	إرجاع عدم;
    }

    إذا (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    وإلا
	STRCPY(new_ei + p_ei_len, what);
    set_string_option_direct((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    إرجاع save_ei;
}

    فراغ
au_event_restore(char_u *old_ei)
{
    إذا (old_ei != عدم)
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
    فراغ
do_autocmd(exarg_T *eap, char_u *arg_in, صحيح forceit)
{
    char_u	*arg = arg_in;
    char_u	*pat;
    char_u	*envpat = عدم;
    char_u	*cmd;
    صحيح		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = عدم;
    صحيح		nested = FALSE;
    صحيح		once = FALSE;
    صحيح		group;
    صحيح		i;
    صحيح		flags = 0;

    إذا (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	group = AUGROUP_ALL;	// no argument, use all groups
    }
    وإلا
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	group = au_get_grouparg(&arg);
	إذا (arg == عدم)	    // out of memory
	    إرجاع;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = find_end_event(arg, group != AUGROUP_ALL);
    إذا (pat == عدم)
	إرجاع;

    pat = skipwhite(pat);
    إذا (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    وإلا
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	بينما (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	إذا (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	إذا (vim_strchr(pat, '$') != عدم || vim_strchr(pat, '~') != عدم)
	{
#ifdef BACKSLASH_IN_FILENAME
	    صحيح	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#endif
	    envpat = expand_env_save(pat);
#ifdef BACKSLASH_IN_FILENAME
	    p_ssl = p_ssl_save;
#endif
	    إذا (envpat != عدم)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	لكل (i = 0; i < 2; i++)
	{
	    إذا (*cmd == NUL)
		استمر;

	    // Check for "++once" flag.
	    إذا (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		إذا (once)
		{
		    semsg(_(e_duplicate_argument_str), "++once");
		    اذهب err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    إذا ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		إذا (nested)
		{
		    semsg(_(e_duplicate_argument_str), "++nested");
		    اذهب err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    إذا (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		إذا (in_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_mean_plusplus_nested));
		    اذهب err_exit;
		}
		إذا (nested)
		{
		    semsg(_(e_duplicate_argument_str), "nested");
		    اذهب err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 6);
	    }
	}

	/*
	 * Find the start of the commands.
	 * Expand <sfile> in it.
	 */
	إذا (*cmd != NUL)
	{
	    إذا (eap != عدم)
		// Read a {} block if it follows.
		cmd = may_get_cmd_block(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    إذا (cmd == عدم)	    // some error
		اذهب err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    إذا (!forceit && *cmd == NUL)
	// Highlight title
	رسالة_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    last_event = (event_T)-1;		// for listing the event name
    last_group = AUGROUP_ERROR;		// for listing the group name
    إذا (*arg == '*' || *arg == NUL || *arg == '|')
    {
	إذا (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	وإلا
	    لكل (event = (event_T)0; (صحيح)event < رقم_EVENTS;
					     event = (event_T)((صحيح)event + 1))
		إذا (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, group, flags) == FAIL)
		    اكسر;
    }
    وإلا
    {
	بينما (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    إذا (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, group, flags) == FAIL)
		اكسر;
    }

err_exit:
    إذا (cmd_need_free)
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
    ساكن صحيح
au_get_grouparg(char_u **argp)
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    صحيح		group = AUGROUP_ALL;

    لكل (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    إذا (p <= arg)
	إرجاع AUGROUP_ALL;

    group_name = vim_strnsave(arg, p - arg);
    إذا (group_name == عدم)		// out of memory
	إرجاع AUGROUP_ERROR;
    group = au_find_group(group_name);
    إذا (group == AUGROUP_ERROR)
	group = AUGROUP_ALL;	// no match, use all groups
    وإلا
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(group_name);
    إرجاع group;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    ساكن صحيح
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    صحيح		once,
    صحيح		nested,
    char_u	*cmd,
    صحيح		forceit,
    صحيح		group,
    صحيح		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    صحيح		brace_level;
    char_u	*endpat;
    صحيح		findgroup;
    صحيح		allgroups;
    صحيح		patlen;
    صحيح		is_buflocal;
    صحيح		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    إذا (group == AUGROUP_ALL)
	findgroup = current_augroup;
    وإلا
	findgroup = group;
    allgroups = (group == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    إذا (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    إذا (forceit)  // delete the AutoPat, if it's in the current group
	    {
		إذا (ap->group == findgroup)
		    au_remove_pat(ap);
	    }
	    وإلا إذا (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    لكل ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	لكل (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    إذا (*endpat == '{')
		brace_level++;
	    وإلا إذا (*endpat == '}')
		brace_level--;
	}
	إذا (pat == endpat)		// ignore single comma
	    استمر;
	patlen = (صحيح)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	إذا (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    إذا (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    وإلا إذا (patlen > 9 && pat[7] == '=')
	    {
		إذا (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		وإلا إذا (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((حرف *)pat + 8);
	    }
	}

	إذا (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((حرف *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (صحيح)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	إذا (!forceit && *cmd != NUL && last_autopat[(صحيح)event] != عدم)
	    prev_ap = &last_autopat[(صحيح)event];
	وإلا
	    prev_ap = &first_autopat[(صحيح)event];
	بينما ((ap = *prev_ap) != عدم)
	{
	    إذا (ap->pat != عدم)
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
		إذا ((allgroups || ap->group == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    إذا (forceit)
		    {
			إذا (*cmd != NUL && ap->next == عدم)
			{
			    au_remove_cmds(ap);
			    اكسر;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    وإلا إذا (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    وإلا إذا (ap->next == عدم)
			اكسر;
		}
	    }
	    prev_ap = &ap->next;
	}

	/*
	 * Add a new command.
	 */
	إذا (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    إذا (ap == عدم)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		إذا (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == عدم))
		{
		    semsg(_(e_buffer_nr_invalid_buffer_العدد), buflocal_nr);
		    إرجاع FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		إذا (ap == عدم)
		    إرجاع FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		إذا (ap->pat == عدم)
		{
		    vim_free(ap);
		    إرجاع FAIL;
		}

#ifdef FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		إذا (event == EVENT_MODECHANGED && !has_modechanged())
		    get_mode(last_mode);
#endif
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		إذا ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
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

		إذا (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = عدم;
		}
		وإلا
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_to_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    إذا (reg_pat != عدم)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    إذا (reg_pat == عدم || ap->reg_prog == عدم)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			إرجاع FAIL;
		    }
		}
		ap->cmds = عدم;
		*prev_ap = ap;
		last_autopat[(صحيح)event] = ap;
		ap->next = عدم;
		إذا (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		وإلا
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    بينما ((ac = *prev_ac) != عدم)
		prev_ac = &ac->next;
	    ac = ALLOC_ONE(AutoCmd);
	    إذا (ac == عدم)
		إرجاع FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    إذا (flags & UC_VIM9)
		ac->script_ctx.sc_version = SCRIPT_VERSION_VIM9;
#ifdef FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#endif
	    إذا (ac->cmd == عدم)
	    {
		vim_free(ac);
		إرجاع FAIL;
	    }
	    ac->next = عدم;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    إرجاع OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    صحيح
do_doautocmd(
    char_u	*arg_start,
    صحيح		do_رسالة,	    // give message for no matching autocmds?
    صحيح		*did_something)
{
    char_u	*arg = arg_start;
    char_u	*fname;
    صحيح		nothing_done = TRUE;
    صحيح		group;

    إذا (did_something != عدم)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    إذا (arg == عدم)	    // out of memory
	إرجاع FAIL;

    إذا (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	إرجاع FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg, group != AUGROUP_ALL);
    إذا (fname == عدم)
	إرجاع FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    بينما (*arg && !ends_excmd(*arg) && !VIM_ISWHITE(*arg))
	إذا (apply_autocmds_group(event_name2nr(arg, &arg),
				      fname, عدم, TRUE, group, curbuf, عدم))
	    nothing_done = FALSE;

    إذا (nothing_done && do_رسالة
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       )
	smsg(_("No matching autocommands: %s"), arg_start);
    إذا (did_something != عدم)
	*did_something = !nothing_done;

#ifdef FEAT_EVAL
    إرجاع aborting() ? FAIL : OK;
#وإلا
    إرجاع OK;
#endif
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    فراغ
ex_doautoall(exarg_T *eap)
{
    صحيح		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    صحيح		call_do_modelines = check_nomodeline(&arg);
    صحيح		did_aucmd;

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
	إذا (buf->b_ml.ml_mfp == عدم || buf == curbuf)
	    استمر;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	إذا (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    اكسر;
	}

	set_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	إذا (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	إذا (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    اكسر;
	}
    }

    // Execute autocommands for the current buffer last.
    إذا (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	إذا (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    صحيح
check_nomodeline(char_u **argp)
{
    إذا (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	إرجاع FALSE;
    }
    إرجاع TRUE;
}

/*
 * Prepare for executing autocommands for (hidden) buffer "buf".
 * Search for a visible window containing the current buffer.  If there isn't
 * one then use an entry in "aucmd_win[]".
 * Set "curbuf" and "curwin" to match "buf".
 * When this fails "curbuf" is not equal "buf".
 */
    فراغ
aucmd_prepbuf(
    aco_save_T	*aco,		// structure to save values in
    buf_T	*buf)		// new curbuf
{
    win_T	*win;
    صحيح		save_ea;
#ifdef FEAT_AUTOCHDIR
    صحيح		save_acd;
#endif
    صحيح		same_buffer = buf == curbuf;

    // Find a window that is for the new buffer
    إذا (same_buffer)		// be quick when buf is curbuf
	win = curwin;
    وإلا
	FOR_ALL_WINDOWS(win)
	    إذا (win->w_buffer == buf)
		اكسر;

    // Allocate a window when needed.
    win_T *auc_win = عدم;
    صحيح auc_idx = AUCMD_WIN_تعداد;
    إذا (win == عدم)
    {
	لكل (auc_idx = 0; auc_idx < AUCMD_WIN_تعداد; ++auc_idx)
	    إذا (!aucmd_win[auc_idx].auc_win_used)
	    {
		إذا (aucmd_win[auc_idx].auc_win == عدم)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		إذا (auc_win != عدم)
		    aucmd_win[auc_idx].auc_win_used = TRUE;
		اكسر;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	إذا (auc_win == عدم)
	    إرجاع;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == عدم ? 0 : prevwin->w_id;
#ifdef FEAT_JOB_CHANNEL
    إذا (bt_prompt(curbuf))
	aco->save_prompt_أدرج = curbuf->b_prompt_أدرج;
#endif

    إذا (win != عدم)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    وإلا
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
	curtab->tp_localdir = عدم;
	aco->globaldir = globaldir;
	globaldir = عدم;

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

	(فراغ)win_split_ins(0, WSP_قمة | WSP_FORCE_ROOM, auc_win, 0, عدم);
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
    إذا (!same_buffer)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_active = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    فراغ
aucmd_restbuf(
    aco_save_T	*aco)		// structure holding saved values
{
    صحيح	    dummy;
    win_T   *save_curwin;

    إذا (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	block_autocmds();
	إذا (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		إذا (wp == awp)
		{
		    إذا (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(awp);
		    اذهب win_found;
		}
	    }
	}
win_found:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(فراغ)winframe_remove(curwin, &dummy, عدم, عدم);
	win_remove(curwin, عدم);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_used = FALSE;
	last_status(FALSE);	    // may need to remove last status line

	إذا (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_find_by_id(aco->save_curwin_id);
	إذا (save_curwin != عدم)
	    curwin = save_curwin;
	وإلا
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_window(curwin);
	إذا (bt_prompt(curbuf))
	    curbuf->b_prompt_أدرج = aco->save_prompt_أدرج;
#endif
	prevwin = win_find_by_id(aco->save_prevwin_id);
#ifdef FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#endif
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	إذا (awp->w_localdir != عدم)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_active = aco->save_VIsual_active;
	check_cursor();
	إذا (curwin->w_topline > curbuf->b_ml.ml_سطر_تعداد)
	{
	    curwin->w_topline = curbuf->b_ml.ml_سطر_تعداد;
#ifdef FEAT_DIFF
	    curwin->w_topfill = 0;
#endif
	}
#إذا defined(FEAT_GUI)
	إذا (gui.in_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_يسار], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_يمين], FALSE);
	    gui_may_update_scrollbars();
	}
#endif
    }
    وإلا
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_find_by_id(aco->save_curwin_id);
	إذا (save_curwin != عدم)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    إذا (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != عدم)
	    {
#إذا defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		إذا (curwin->w_s == &curbuf->b_s)
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
    إذا (VIsual_active)
	check_pos(curbuf, &VIsual);
}

ساكن صحيح	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    صحيح
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    صحيح		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    إرجاع apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, عدم);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    صحيح
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    صحيح		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    إرجاع apply_autocmds_group(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    صحيح
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    صحيح		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    صحيح		*retval)    // pointer to caller's retval
{
    صحيح		did_cmd;

#ifdef FEAT_EVAL
    إذا (should_abort(*retval))
	إرجاع FALSE;
#endif

    did_cmd = apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, عدم);
    إذا (did_cmd
#ifdef FEAT_EVAL
	    && aborting()
#endif
	    )
	*retval = FAIL;
    إرجاع did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    ساكن صحيح
has_cursorhold(فراغ)
{
    إرجاع (first_autopat[(صحيح)(get_real_state() == MODE_NORMAL_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != عدم);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    صحيح
trigger_cursorhold(فراغ)
{
    صحيح		state;

    إذا (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_active())
    {
	state = get_real_state();
	إذا (state == MODE_NORMAL_BUSY || (state & MODE_أدرج) != 0)
	    إرجاع TRUE;
    }
    إرجاع FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    صحيح
has_winresized(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_WINRESIZED] != عدم);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    صحيح
has_winscrolled(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_WINSCROLLED] != عدم);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    صحيح
has_cursormoved(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_CURSORMOVED] != عدم);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    صحيح
has_cursormovedI(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_CURSORMOVEDI] != عدم);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    صحيح
has_textchanged(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_TEXTCHANGED] != عدم);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    صحيح
has_textchangedI(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_TEXTCHANGEDI] != عدم);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    صحيح
has_textchangedP(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_TEXTCHANGEDP] != عدم);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    صحيح
has_insertcharpre(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_INSERTCHARPRE] != عدم);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    صحيح
has_keyinputpre(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_KEYINPUTPRE] != عدم);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    صحيح
has_cmdundefined(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_CMDUNDEFINED] != عدم);
}

#إذا defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    صحيح
has_textputpost(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_TEXTPUTPOST] != عدم);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    صحيح
has_textputpre(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_TEXTPUTPRE] != عدم);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    صحيح
has_textyankpost(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_TEXTYANKPOST] != عدم);
}
#endif

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    صحيح
has_completechanged(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_COMPLETECHANGED] != عدم);
}
#endif

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    صحيح
has_modechanged(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_MODECHANGED] != عدم);
}
#endif

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    ساكن صحيح
apply_autocmds_group(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    صحيح		force,	     // when TRUE, ignore autocmd_busy
    صحيح		group,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap UNUSED) // command arguments
{
    char_u	*sfname = عدم;	// short file name
    char_u	*tail;
    صحيح		save_changed;
    buf_T	*old_curbuf;
    صحيح		retval = FALSE;
    char_u	*save_autocmd_fname;
    صحيح		save_autocmd_fname_full;
    صحيح		save_autocmd_bufnr;
    char_u	*save_autocmd_match;
    صحيح		save_autocmd_busy;
    صحيح		save_autocmd_nested;
    ساكن صحيح	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ifdef FEAT_EVAL
    funccal_entry_T funccal_entry;
    char_u	*save_cmdarg;
    long	save_cmdbang;
#endif
    ساكن صحيح	filechangeshell_busy = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif
    صحيح		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    صحيح		save_مفتاح_Typed = مفتاح_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    إذا (event == رقم_EVENTS || first_autopat[(صحيح)event] == عدم
	    || autocmd_blocked > 0)
	اذهب BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    إذا (autocmd_busy && !(force || autocmd_nested))
	اذهب BYPASS_AU;

#ifdef FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    إذا (aborting())
	اذهب BYPASS_AU;
#endif

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    إذا (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	اذهب BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    إذا (event_ignored(event, p_ei))
	اذهب BYPASS_AU;

    صحيح win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    إذا (buf == curbuf && event_tab[event].مفتاح <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    وإلا إذا (buf != عدم && event_tab[event].مفتاح <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    إذا (wp->w_buffer == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		اكسر;
	    }
    }
    إذا (win_ignore)
	اذهب BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    إذا (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	اذهب BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    إذا (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	اذهب BYPASS_AU;

    إذا (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_changed_تعداد;

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
    إذا (fname_io == عدم)
    {
	إذا (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = عدم;
	وإلا إذا (fname != عدم && !ends_excmd(*fname))
	    autocmd_fname = fname;
	وإلا إذا (buf != عدم)
	    autocmd_fname = buf->b_ffname;
	وإلا
	    autocmd_fname = عدم;
    }
    وإلا
	autocmd_fname = fname_io;
    إذا (autocmd_fname != عدم)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_full = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    إذا (buf == عدم)
	autocmd_bufnr = 0;
    وإلا
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    إذا (fname == عدم || *fname == NUL)
    {
	إذا (buf == عدم)
	    fname = عدم;
	وإلا
	{
#ifdef FEAT_SYN_HL
	    إذا (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    وإلا
#endif
		إذا (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		وإلا
		{
		    إذا (buf->b_sfname != عدم)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	إذا (fname == عدم)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    وإلا
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	إذا (event == EVENT_FILETYPE
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
	وإلا
	    fname = FullName_save(fname, FALSE);
    }
    إذا (fname == عدم)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	اذهب BYPASS_AU;
    }

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    إذا (sfname != عدم)
	forward_slash(sfname);
    forward_slash(fname);
#endif

#ifdef VMS
    // remove version for correct match
    إذا (sfname != عدم)
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
    estack_ادفع(ETYPE_AUCMD, عدم, 0);
    ESTACK_CHECK_SETUP;

    save_current_sctx = current_sctx;

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	prof_child_enter(&wait_time); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_entry);
#endif

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    إذا (!autocmd_busy)
    {
	save_بحث_patterns();
	إذا (!ins_compl_active())
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
    إذا (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    tail = gettail(fname);

    // Find first autocommand that matches
    CLEAR_FIELD(patcmd);
    patcmd.curpat = first_autopat[(صحيح)event];
    patcmd.group = group;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.tail = tail;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_next_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    إذا (patcmd.curpat != عدم)
    {
	// add to active_apc_list
	patcmd.next = active_apc_list;
	active_apc_list = &patcmd;

#ifdef FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (long)get_vim_var_nr(VV_CMDBANG);
	إذا (eap != عدم)
	{
	    save_cmdarg = set_cmdarg(eap, عدم);
	    set_vim_var_nr(VV_CMDBANG, (long)eap->forceit);
	}
	وإلا
	    save_cmdarg = عدم;	// avoid gcc warning
#endif
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	لكل (ap = patcmd.curpat; ap->next != عدم; ap = ap->next)
	    ap->last = FALSE;
	ap->last = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	إذا (nesting == 1)
	    check_lnums(TRUE);
	وإلا
	    check_lnums_nested(TRUE);

	صحيح save_did_emsg = did_emsg;
	صحيح save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(عدم, getnextac, (فراغ *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	set_pressedreturn(save_ex_pressedreturn);

	إذا (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ifdef FEAT_EVAL
	إذا (eap != عدم)
	{
	    (فراغ)set_cmdarg(عدم, save_cmdarg);
	    set_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#endif
	// delete from active_apc_list
	إذا (active_apc_list == &patcmd)	    // just in case
	    active_apc_list = patcmd.next;
    }

    إذا (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_اسحب();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_full = save_autocmd_fname_full;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_match = save_autocmd_match;
    current_sctx = save_current_sctx;
#ifdef FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif
    مفتاح_Typed = save_مفتاح_Typed;
    vim_free(fname);
    vim_free(sfname);
    --nesting;		// see matching increment above

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.  Free any buffers in the au_pending_free_buf list and
     * free any windows in the au_pending_free_win list.
     */
    إذا (!autocmd_busy)
    {
	restore_بحث_patterns();
	إذا (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	بينما (au_pending_free_buf != عدم)
	{
	    buf_T *b = au_pending_free_buf->b_next;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	بينما (au_pending_free_win != عدم)
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
    إذا (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	إذا (curbuf->b_changed != save_changed)
	    need_maketitle = TRUE;
	curbuf->b_changed = save_changed;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    إذا (event == EVENT_BUFWIPEOUT && buf != عدم)
	aubuflocal_remove(buf);

    إذا (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    إرجاع retval;
}

#ifdef FEAT_EVAL
ساكن char_u	*old_termresponse = عدم;
ساكن char_u	*old_termu7resp = عدم;
ساكن char_u	*old_termblinkresp = عدم;
ساكن char_u	*old_termrbgresp = عدم;
ساكن char_u	*old_termrfgresp = عدم;
ساكن char_u	*old_termstyleresp = عدم;
#endif

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    فراغ
block_autocmds(فراغ)
{
#ifdef FEAT_EVAL
    // Remember the value of v:termresponse.
    إذا (autocmd_blocked == 0)
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

    فراغ
unblock_autocmds(فراغ)
{
    --autocmd_blocked;

#ifdef FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    إذا (autocmd_blocked == 0)
    {
	إذا (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, عدم, عدم, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", عدم, FALSE, curbuf);
	}
	إذا (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", عدم, FALSE, curbuf);
	}
	إذا (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", عدم, FALSE, curbuf);
	}
	إذا (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", عدم, FALSE, curbuf);
	}
	إذا (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", عدم, FALSE, curbuf);
	}
	إذا (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", عدم, FALSE, curbuf);
	}
    }
#endif
}

    صحيح
is_autocmd_blocked(فراغ)
{
    إرجاع autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    ساكن فراغ
auto_next_pat(
    AutoPatCmd_T *apc,
    صحيح		stop_at_last)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*entry;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(entry->es_name);
    entry->es_info.aucmd = عدم;

    لكل (ap = apc->curpat; ap != عدم && !got_int; ap = ap->next)
    {
	apc->curpat = عدم;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	إذا (ap->pat != عدم && ap->cmds != عدم
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
	    // execution-condition
	    إذا (ap->buflocal_nr == 0
		    ? (match_file_pat(عدم, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->tail, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		حرف	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->length + ap->patlen + 1);
		إذا (namep != عدم)
		{
		    sprintf((حرف *)namep, fmt, (حرف *)event_name->string, (حرف *)ap->pat);
		    إذا (p_verbose >= 8)
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
		لكل (cp = ap->cmds; cp->next != عدم; cp = cp->next)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    سطر_breakcheck();
	    إذا (apc->curpat != عدم)	    // found a match
		اكسر;
	}
	إذا (stop_at_last && ap->last)
	    اكسر;
    }
}

#إذا defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    إرجاع &acp->script_ctx;
}
#endif

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	صحيح c UNUSED,
	فراغ *cookie,
	صحيح indent UNUSED,
	getline_opt_T options UNUSED)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    إذا (acp->curpat == عدم)
	إرجاع عدم;

    // repeat until we find an autocommand to execute
    لكل (;;)
    {
	// skip removed commands
	بينما (acp->nextcmd != عدم && acp->nextcmd->cmd == عدم)
	    إذا (acp->nextcmd->last)
		acp->nextcmd = عدم;
	    وإلا
		acp->nextcmd = acp->nextcmd->next;

	إذا (acp->nextcmd != عدم)
	    اكسر;

	// at end of commands, find next pattern that matches
	إذا (acp->curpat->last)
	    acp->curpat = عدم;
	وإلا
	    acp->curpat = acp->curpat->next;
	إذا (acp->curpat != عدم)
	    auto_next_pat(acp, TRUE);
	إذا (acp->curpat == عدم)
	    إرجاع عدم;
    }

    ac = acp->nextcmd;

    إذا (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	رسالة_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    إذا (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    إذا (ac->last)
	acp->nextcmd = عدم;
    وإلا
	acp->nextcmd = ac->next;
    إرجاع retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    ساكن صحيح
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_full)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*tail = gettail(sfname);
    صحيح		retval = FALSE;

    إذا (first_autopat[(صحيح)event] == عدم)
	إرجاع FALSE;

    fname = is_full ? vim_strsave(sfname) : FullName_save(sfname, FALSE);
    إذا (fname == عدم)
	إرجاع FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    إذا (sfname != عدم)
	forward_slash(sfname);
    forward_slash(fname);
#endif

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	إذا (ap->pat != عدم && ap->cmds != عدم
	      && (ap->buflocal_nr == 0
		? match_file_pat(عدم, &ap->reg_prog,
					  fname, sfname, tail, ap->allow_dirs)
		: buf != عدم && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    اكسر;
	}

    vim_free(fname);
#ifdef BACKSLASH_IN_FILENAME
    vim_free(sfname);
#endif

    إرجاع retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    صحيح
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    إرجاع has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    صحيح
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    إرجاع has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp UNUSED, صحيح idx)
{
    إذا (idx == augroups.ga_len)		// add "END" add the end
	إرجاع (char_u *)"END";
    إذا (idx < 0 || idx >= augroups.ga_len)	// end of list
	إرجاع عدم;
    إذا (AUGROUP_NAME(idx) == عدم || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	إرجاع (char_u *)"";
    إرجاع AUGROUP_NAME(idx);		// return a name
}

ساكن صحيح include_groups = FALSE;

    char_u  *
set_context_in_autocmd(
    expand_T	*xp,
    char_u	*arg,
    صحيح		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    صحيح		group;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    group = au_get_grouparg(&arg);
    إذا (group == AUGROUP_ERROR)
	إرجاع عدم;
    // If there only is a group name that's what we expand.
    إذا (*arg == NUL && group != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	group = AUGROUP_ALL;
    }

    // skip over event name
    لكل (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	إذا (*p == ',')
	    arg = p + 1;
    إذا (*p == NUL)
    {
	إذا (group == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_pattern = arg;
	إرجاع عدم;
    }

    // skip over pattern
    arg = skipwhite(p);
    بينما (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    إذا (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	لكل (صحيح i = 0; i < 2; i++)
	{
	    إذا (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    إذا (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    إذا (!in_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	إرجاع arg;			    // expand (next) command
    }

    إذا (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    وإلا
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    إرجاع عدم;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp UNUSED, صحيح idx)
{
    صحيح i;

    إذا (idx < augroups.ga_len)		// First list group names, if wanted
    {
	إذا (!include_groups || AUGROUP_NAME(idx) == عدم
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    إرجاع (char_u *)"";	// skip deleted entries
	إرجاع AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    إذا (i < 0 || i >= رقم_EVENTS)
	إرجاع عدم;

    إرجاع event_tab[i].قيمة.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_group(expand_T *xp UNUSED, صحيح idx, صحيح win)
{
    إذا (idx < 0 || idx >= رقم_EVENTS)
	إرجاع عدم;

    إذا (!win)
	إرجاع event_tab[idx].قيمة.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    صحيح j = 0;
    لكل (صحيح i = 0; i < رقم_EVENTS; ++i)
    {
	j += event_tab[i].مفتاح <= 0;
	إذا (j == idx + 1)
	    إرجاع event_tab[i].قيمة.string;
    }

    إرجاع عدم;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    صحيح
has_tabclosedpre(فراغ)
{
    إرجاع (first_autopat[(صحيح)EVENT_TABCLOSEDPRE] != عدم);
}

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    صحيح
autocmd_supported(char_u *name)
{
    char_u *p;

    إرجاع (event_name2nr(name, &p) != رقم_EVENTS);
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
    صحيح
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*pattern = عدم;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = عدم;
    صحيح		group;
    صحيح		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    إذا (arg_save == عدم)
	إرجاع FALSE;
    p = vim_strchr(arg_save, '#');
    إذا (p != عدم)
	*p++ = NUL;

    // First, look for an autocmd group name
    group = au_find_group(arg_save);
    إذا (group == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	group = AUGROUP_ALL;
	event_name = arg_save;
    }
    وإلا
    {
	إذا (p == عدم)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    اذهب theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	إذا (p != عدم)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    pattern = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    إذا (event == رقم_EVENTS)
	اذهب theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = first_autopat[(صحيح)event];
    إذا (ap == عدم)
	اذهب theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    إذا (pattern != عدم && STRICMP(pattern, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    لكل ( ; ap != عدم; ap = ap->next)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	إذا (ap->pat != عدم && ap->cmds != عدم
	    && (group == AUGROUP_ALL || ap->group == group)
	    && (pattern == عدم
		|| (buflocal_buf == عدم
		    ? fnamecmp(ap->pat, pattern) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    اكسر;
	}

theend:
    vim_free(arg_save);
    إرجاع retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    ساكن فراغ
autocmd_add_or_delete(typval_T *argvars, typval_T *rettv, صحيح delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = عدم;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*group_name = عدم;
    صحيح		group;
    char_u	*pat = عدم;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = عدم;
    char_u	*end;
    صحيح		once;
    صحيح		nested;
    صحيح		replace;		// replace the cmd for a group/event
    صحيح		retval = VVAL_TRUE;
    صحيح		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_العدد = VVAL_FALSE;

    إذا (check_restricted() || check_secure())
	إرجاع;

    إذا (check_for_list_arg(argvars, 0) == FAIL)
	إرجاع;

    aucmd_list = argvars[0].vval.v_list;
    إذا (aucmd_list == عدم)
	إرجاع;

    FOR_ALL_LIST_عناصر(aucmd_list, li)
    {
	VIM_CLEAR(group_name);
	VIM_CLEAR(cmd);
	event_name = عدم;
	event_list = عدم;
	pat = عدم;
	pat_list = عدم;

	إذا (li->li_tv.v_type != VAR_DICT)
	    استمر;

	event_dict = li->li_tv.vval.v_dict;
	إذا (event_dict == عدم)
	    استمر;

	di = dict_find(event_dict, (char_u *)"event", -1);
	إذا (di != عدم)
	{
	    إذا (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		إذا (event_name == عدم)
		{
		    emsg(_(e_string_required));
		    استمر;
		}
	    }
	    وإلا إذا (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		إذا (event_list == عدم)
		{
		    emsg(_(e_list_required));
		    استمر;
		}
	    }
	    وإلا
	    {
		emsg(_(e_string_or_list_expected));
		استمر;
	    }
	}

	group_name = dict_get_string(event_dict, "group", TRUE);
	إذا (group_name == عدم || *group_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    group = current_augroup;
	وإلا
	{
	    group = au_find_group(group_name);
	    إذا (group == AUGROUP_ERROR)
	    {
		إذا (delete)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    اكسر;
		}
		// group is not found, create it now
		group = au_new_group(group_name);
		إذا (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    اكسر;
		}

		current_augroup = group;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	إذا (dict_has_مفتاح(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_العدد_def(event_dict, "bufnr", -1);
	    إذا (bnum == -1)
		استمر;

	    vim_snprintf((حرف *)IObuff, IOSIZE, "<buffer=%d>", (صحيح)bnum);
	    pat = IObuff;
	}
	وإلا
	{
	    di = dict_find(event_dict, (char_u *)"pattern", -1);
	    إذا (di != عدم)
	    {
		إذا (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    إذا (pat == عدم)
		    {
			emsg(_(e_string_required));
			استمر;
		    }
		}
		وإلا إذا (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    إذا (pat_list == عدم)
		    {
			emsg(_(e_list_required));
			استمر;
		    }
		}
		وإلا
		{
		    emsg(_(e_string_or_list_expected));
		    استمر;
		}
	    }
	    وإلا إذا (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	إذا (cmd == عدم)
	{
	    إذا (delete)
		cmd = vim_strsave((char_u *)"");
	    وإلا
		استمر;
	}

	إذا (delete && (event_name == عدم
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    لكل (event = (event_T)0; (صحيح)event < رقم_EVENTS;
		    event = (event_T)((صحيح)event + 1))
	    {
		إذا (do_autocmd_event(event, pat, once, nested, cmd, delete,
							group, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    اكسر;
		}
	    }
	}
	وإلا
	{
	    char_u *p = عدم;

	    eli = عدم;
	    end = عدم;
	    بينما (TRUE)
	    {
		إذا (event_list != عدم)
		{
		    إذا (eli == عدم)
			eli = event_list->lv_first;
		    وإلا
			eli = eli->li_next;
		    إذا (eli == عدم)
			اكسر;
		    إذا (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == عدم)
		    {
			emsg(_(e_string_required));
			اكسر;
		    }
		}
		وإلا
		{
		    إذا (p == عدم)
			p = event_name;
		    إذا (p == عدم || *p == NUL)
			اكسر;
		}

		event = event_name2nr(p, &end);
		إذا (event == رقم_EVENTS || *end != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    اكسر;
		}
		إذا (pat != عدم)
		{
		    إذا (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, group, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			اكسر;
		    }
		}
		وإلا إذا (pat_list != عدم)
		{
		    FOR_ALL_LIST_عناصر(pat_list, pli)
		    {
			إذا (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == عدم)
			{
			    emsg(_(e_string_required));
			    استمر;
			}
			إذا (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, group, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    اكسر;
			}
		    }
		    إذا (retval == VVAL_FALSE)
			اكسر;
		}
		إذا (event_name != عدم)
		    p = end;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	إذا (delete && group_name != عدم &&
		(event_name == عدم || event_name[0] == NUL)
		&& (pat == عدم || pat[0] == NUL)
		&& (cmd == عدم || cmd[0] == NUL))
	    au_del_group(group_name);
    }

    VIM_CLEAR(group_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_العدد = retval;
}

/*
 * autocmd_add() function
 */
    فراغ
f_autocmd_add(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    فراغ
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    فراغ
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = رقم_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = عدم;
    char_u	*name = عدم;
    صحيح		group = AUGROUP_ALL;

    إذا (rettv_list_alloc(rettv) == FAIL)
	إرجاع;
    إذا (check_for_opt_dict_arg(argvars, 0) == FAIL)
	إرجاع;

    إذا (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	إذا (dict_has_مفتاح(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    إذا (name == عدم)
		إرجاع;

	    إذا (*name == NUL)
		group = AUGROUP_DEFAULT;
	    وإلا
	    {
		group = au_find_group(name);
		إذا (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), name);
		    vim_free(name);
		    إرجاع;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	إذا (dict_has_مفتاح(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    إذا (name == عدم)
		إرجاع;

	    إذا (name[0] == '*' && name[1] == NUL)
		event_arg = رقم_EVENTS;
	    وإلا
	    {
		keyvalue_T target;
		keyvalue_T *entry;

		target.مفتاح = 0;
		target.قيمة.string = name;
		target.قيمة.length = STRLEN(target.قيمة.string);
		entry = (keyvalue_T *)bsearch(&target, &event_tab,
		    رقم_EVENTS, حجم(event_tab[0]), cmp_keyvalue_قيمة_ni);
		إذا (entry == عدم)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    إرجاع;
		}
		event_arg = (event_T)abs(entry->مفتاح);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	إذا (dict_has_مفتاح(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    إذا (pat == عدم)
		إرجاع;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    لكل (event = (event_T)0; (صحيح)event < رقم_EVENTS;
	    event = (event_T)((صحيح)event + 1))
    {
	string_T    *event_name;

	إذا (event_arg != رقم_EVENTS && event != event_arg)
	    استمر;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	group_name;

	    إذا (ap->pat == عدم)		// pattern has been removed
		استمر;

	    إذا (group != AUGROUP_ALL && group != ap->group)
		استمر;

	    إذا (pat != عدم && STRCMP(pat, ap->pat) != 0)
		استمر;

	    group_name.string = get_augroup_name(عدم, ap->group);
	    إذا (group_name.string == عدم)
		STR_LITERAL_SET(group_name, "");
	    وإلا
		group_name.length = STRLEN(group_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    لكل (ac = ap->cmds; ac != عدم; ac = ac->next)
	    {
		event_dict = dict_alloc();
		إذا (event_dict == عدم
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    إرجاع;
		}

		إذا (dict_add_string_len(event_dict, "event",
			event_name->string, (صحيح)event_name->length) == FAIL
			|| dict_add_string_len(event_dict, "group",
			    group_name.string, (صحيح)group_name.length) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_add_العدد(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_add_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_add_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_add_bool(event_dict, "once", ac->once) == FAIL
			|| dict_add_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    إرجاع;
		}
	    }
	}
    }

    vim_free(pat);
}

#endif
