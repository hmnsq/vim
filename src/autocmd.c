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
    حرف	    آخرین;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    بنية AutoCmd  *بعدی;		// next AutoCmd in list
} AutoCmd;

تعريفنوع بنية AutoPat
{
    بنية AutoPat  *بعدی;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    صحيح		    گروه;		// group ID
    صحيح		    patlen;		// strlen() of pat
    صحيح		    buflocal_nr;	// !=0 for buffer-local AutoPat
    حرف	    allow_dirs;		// Pattern may match whole path
    حرف	    آخرین;		// last pattern for apply_autocmds()
} AutoPat;

//
// special cases:
// BufNewFile and BufRead are searched for ALOT (especially at startup)
// so we pre-determine their index into the event_tab[] table for fast access.
// Keep these values in sync with event_tab[]!
#تعریف_کن BUFNEWFILE_شاخص 9
#تعریف_کن BUFREAD_شاخص 10

// Must be sorted by the 'value' field because it is used by bsearch()!
// Events with positive keys aren't allowed in 'eventignorewin'.
ساكن keyvalue_T event_tab[رقم_EVENTS] = {
    KEYVALUE_مدخل(-EVENT_BUFADD, "BufAdd"),
    KEYVALUE_مدخل(-EVENT_BUFADD, "BufCreate"),
    KEYVALUE_مدخل(-EVENT_BUFDELETE, "BufDelete"),
    KEYVALUE_مدخل(-EVENT_BUFENTER, "BufEnter"),
    KEYVALUE_مدخل(-EVENT_BUFFILEPOST, "BufFilePost"),
    KEYVALUE_مدخل(-EVENT_BUFFILEPRE, "BufFilePre"),
    KEYVALUE_مدخل(-EVENT_BUFHIDDEN, "BufHidden"),
    KEYVALUE_مدخل(-EVENT_BUFLEAVE, "BufLeave"),
    KEYVALUE_مدخل(-EVENT_BUFNEW, "BufNew"),
    KEYVALUE_مدخل(-EVENT_BUFNEWFILE, "BufNewFile"),	// BUFNEWFILE_INDEX
    KEYVALUE_مدخل(-EVENT_BUFREADPOST, "BufRead"),	// BUFREAD_INDEX
    KEYVALUE_مدخل(-EVENT_BUFREADCMD, "BufReadCmd"),
    KEYVALUE_مدخل(-EVENT_BUFREADPOST, "BufReadPost"),
    KEYVALUE_مدخل(-EVENT_BUFREADPRE, "BufReadPre"),
    KEYVALUE_مدخل(-EVENT_BUFUNLOAD, "BufUnload"),
    KEYVALUE_مدخل(-EVENT_BUFWINENTER, "BufWinEnter"),
    KEYVALUE_مدخل(-EVENT_BUFWINLEAVE, "BufWinLeave"),
    KEYVALUE_مدخل(-EVENT_BUFWIPEOUT, "BufWipeout"),
    KEYVALUE_مدخل(-EVENT_BUFWRITEPRE, "BufWrite"),
    KEYVALUE_مدخل(-EVENT_BUFWRITECMD, "BufWriteCmd"),
    KEYVALUE_مدخل(-EVENT_BUFWRITEPOST, "BufWritePost"),
    KEYVALUE_مدخل(-EVENT_BUFWRITEPRE, "BufWritePre"),
    KEYVALUE_مدخل(EVENT_CMDLINECHANGED, "CmdlineChanged"),
    KEYVALUE_مدخل(EVENT_CMDLINEENTER, "CmdlineEnter"),
    KEYVALUE_مدخل(EVENT_CMDLINELEAVE, "CmdlineLeave"),
    KEYVALUE_مدخل(EVENT_CMDLINELEAVEPRE, "CmdlineLeavePre"),
    KEYVALUE_مدخل(EVENT_CMDUNDEFINED, "CmdUndefined"),
    KEYVALUE_مدخل(EVENT_CMDWINENTER, "CmdwinEnter"),
    KEYVALUE_مدخل(EVENT_CMDWINLEAVE, "CmdwinLeave"),
    KEYVALUE_مدخل(EVENT_COLORSCHEME, "ColorScheme"),
    KEYVALUE_مدخل(EVENT_COLORSCHEMEPRE, "ColorSchemePre"),
    KEYVALUE_مدخل(EVENT_COMPLETECHANGED, "CompleteChanged"),
    KEYVALUE_مدخل(EVENT_COMPLETEDONE, "CompleteDone"),
    KEYVALUE_مدخل(EVENT_COMPLETEDONEPRE, "CompleteDonePre"),
    KEYVALUE_مدخل(-EVENT_CURSORHOLD, "CursorHold"),
    KEYVALUE_مدخل(-EVENT_CURSORHOLDI, "CursorHoldI"),
    KEYVALUE_مدخل(-EVENT_CURSORMOVED, "CursorMoved"),
    KEYVALUE_مدخل(-EVENT_CURSORMOVEDC, "CursorMovedC"),
    KEYVALUE_مدخل(-EVENT_CURSORMOVEDI, "CursorMovedI"),
    KEYVALUE_مدخل(EVENT_DIFFUPDATED, "DiffUpdated"),
    KEYVALUE_مدخل(EVENT_DIRCHANGED, "DirChanged"),
    KEYVALUE_مدخل(EVENT_DIRCHANGEDPRE, "DirChangedPre"),
    KEYVALUE_مدخل(EVENT_ENCODINGCHANGED, "EncodingChanged"),
    KEYVALUE_مدخل(EVENT_EXITPRE, "ExitPre"),
    KEYVALUE_مدخل(-EVENT_FILEAPPENDCMD, "FileAppendCmd"),
    KEYVALUE_مدخل(-EVENT_FILEAPPENDPOST, "FileAppendPost"),
    KEYVALUE_مدخل(-EVENT_FILEAPPENDPRE, "FileAppendPre"),
    KEYVALUE_مدخل(-EVENT_FILECHANGEDRO, "FileChangedRO"),
    KEYVALUE_مدخل(-EVENT_FILECHANGEDSHELL, "FileChangedShell"),
    KEYVALUE_مدخل(-EVENT_FILECHANGEDSHELLPOST, "FileChangedShellPost"),
    KEYVALUE_مدخل(EVENT_ENCODINGCHANGED, "FileEncoding"),
    KEYVALUE_مدخل(-EVENT_FILEREADCMD, "FileReadCmd"),
    KEYVALUE_مدخل(-EVENT_FILEREADPOST, "FileReadPost"),
    KEYVALUE_مدخل(-EVENT_FILEREADPRE, "FileReadPre"),
    KEYVALUE_مدخل(-EVENT_FILETYPE, "FileType"),
    KEYVALUE_مدخل(-EVENT_FILEWRITECMD, "FileWriteCmd"),
    KEYVALUE_مدخل(-EVENT_FILEWRITEPOST, "FileWritePost"),
    KEYVALUE_مدخل(-EVENT_FILEWRITEPRE, "FileWritePre"),
    KEYVALUE_مدخل(-EVENT_FILTERREADPOST, "FilterReadPost"),
    KEYVALUE_مدخل(-EVENT_FILTERREADPRE, "FilterReadPre"),
    KEYVALUE_مدخل(-EVENT_FILTERWRITEPOST, "FilterWritePost"),
    KEYVALUE_مدخل(-EVENT_FILTERWRITEPRE, "FilterWritePre"),
    KEYVALUE_مدخل(EVENT_FOCUSGAINED, "FocusGained"),
    KEYVALUE_مدخل(EVENT_FOCUSLOST, "FocusLost"),
    KEYVALUE_مدخل(EVENT_FUNCUNDEFINED, "FuncUndefined"),
    KEYVALUE_مدخل(EVENT_GUIENTER, "GUIEnter"),
    KEYVALUE_مدخل(EVENT_GUIFAILED, "GUIFailed"),
    KEYVALUE_مدخل(-EVENT_INSERTCHANGE, "InsertChange"),
    KEYVALUE_مدخل(-EVENT_INSERTCHARPRE, "InsertCharPre"),
    KEYVALUE_مدخل(-EVENT_INSERTENTER, "InsertEnter"),
    KEYVALUE_مدخل(-EVENT_INSERTLEAVE, "InsertLeave"),
    KEYVALUE_مدخل(-EVENT_INSERTLEAVEPRE, "InsertLeavePre"),
    KEYVALUE_مدخل(EVENT_KEYINPUTPRE, "KeyInputPre"),
    KEYVALUE_مدخل(EVENT_MENUPOPUP, "MenuPopup"),
    KEYVALUE_مدخل(EVENT_MODECHANGED, "ModeChanged"),
    KEYVALUE_مدخل(EVENT_OPTIONSET, "OptionSet"),
    KEYVALUE_مدخل(EVENT_QUICKFIXCMDPOST, "QuickFixCmdPost"),
    KEYVALUE_مدخل(EVENT_QUICKFIXCMDPRE, "QuickFixCmdPre"),
    KEYVALUE_مدخل(EVENT_QUITPRE, "QuitPre"),
    KEYVALUE_مدخل(EVENT_REMOTEREPLY, "RemoteReply"),
    KEYVALUE_مدخل(EVENT_SAFESTATE, "SafeState"),
    KEYVALUE_مدخل(EVENT_SAFESTATEAGAIN, "SafeStateAgain"),
    KEYVALUE_مدخل(EVENT_SESSIONLOADPOST, "SessionLoadPost"),
    KEYVALUE_مدخل(EVENT_SESSIONLOADPRE, "SessionLoadPre"),
    KEYVALUE_مدخل(EVENT_SESSIONWRITEPOST, "SessionWritePost"),
    KEYVALUE_مدخل(EVENT_SHELLCMDPOST, "ShellCmdPost"),
    KEYVALUE_مدخل(-EVENT_SHELLFILTERPOST, "ShellFilterPost"),
    KEYVALUE_مدخل(EVENT_SIGUSR1, "SigUSR1"),
    KEYVALUE_مدخل(EVENT_SOURCECMD, "SourceCmd"),
    KEYVALUE_مدخل(EVENT_SOURCEPOST, "SourcePost"),
    KEYVALUE_مدخل(EVENT_SOURCEPRE, "SourcePre"),
    KEYVALUE_مدخل(EVENT_SPELLFILEMISSING, "SpellFileMissing"),
    KEYVALUE_مدخل(EVENT_STDINREADPOST, "StdinReadPost"),
    KEYVALUE_مدخل(EVENT_STDINREADPRE, "StdinReadPre"),
    KEYVALUE_مدخل(EVENT_SWAPEXISTS, "SwapExists"),
    KEYVALUE_مدخل(EVENT_SYNTAX, "Syntax"),
    KEYVALUE_مدخل(EVENT_TABCLOSED, "TabClosed"),
    KEYVALUE_مدخل(EVENT_TABCLOSEDPRE, "TabClosedPre"),
    KEYVALUE_مدخل(EVENT_TABENTER, "TabEnter"),
    KEYVALUE_مدخل(EVENT_TABLEAVE, "TabLeave"),
    KEYVALUE_مدخل(EVENT_TABNEW, "TabNew"),
    KEYVALUE_مدخل(EVENT_TERMCHANGED, "TermChanged"),
    KEYVALUE_مدخل(EVENT_TERMINALOPEN, "TerminalOpen"),
    KEYVALUE_مدخل(EVENT_TERMINALWINOPEN, "TerminalWinOpen"),
    KEYVALUE_مدخل(EVENT_TERMRESPONSE, "TermResponse"),
    KEYVALUE_مدخل(EVENT_TERMRESPONSEALL, "TermResponseAll"),
    KEYVALUE_مدخل(-EVENT_TEXTCHANGED, "TextChanged"),
    KEYVALUE_مدخل(-EVENT_TEXTCHANGEDI, "TextChangedI"),
    KEYVALUE_مدخل(-EVENT_TEXTCHANGEDP, "TextChangedP"),
    KEYVALUE_مدخل(-EVENT_TEXTCHANGEDT, "TextChangedT"),
    KEYVALUE_مدخل(-EVENT_TEXTPUTPOST, "TextPutPost"),
    KEYVALUE_مدخل(-EVENT_TEXTPUTPRE, "TextPutPre"),
    KEYVALUE_مدخل(-EVENT_TEXTYANKPOST, "TextYankPost"),
    KEYVALUE_مدخل(EVENT_USER, "User"),
    KEYVALUE_مدخل(EVENT_VIMENTER, "VimEnter"),
    KEYVALUE_مدخل(EVENT_VIMLEAVE, "VimLeave"),
    KEYVALUE_مدخل(EVENT_VIMLEAVEPRE, "VimLeavePre"),
    KEYVALUE_مدخل(EVENT_VIMRESIZED, "VimResized"),
    KEYVALUE_مدخل(EVENT_VIMRESUME, "VimResume"),
    KEYVALUE_مدخل(EVENT_VIMSUSPEND, "VimSuspend"),
    KEYVALUE_مدخل(-EVENT_WINCLOSED, "WinClosed"),
    KEYVALUE_مدخل(-EVENT_WINENTER, "WinEnter"),
    KEYVALUE_مدخل(-EVENT_WINLEAVE, "WinLeave"),
    KEYVALUE_مدخل(EVENT_WINNEW, "WinNew"),
    KEYVALUE_مدخل(EVENT_WINNEWPRE, "WinNewPre"),
    KEYVALUE_مدخل(-EVENT_WINRESIZED, "WinResized"),
    KEYVALUE_مدخل(-EVENT_WINSCROLLED, "WinScrolled"),
};

ساكن AutoPat *اولی_autopat[رقم_EVENTS] = { عدم };
ساكن AutoPat *آخرین_autopat[رقم_EVENTS] = { عدم };

#تعریف_کن AUGROUP_DEFAULT    (-1)	    // default autocmd group
#تعریف_کن AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#تعریف_کن AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
بنية AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    صحيح		گروه;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*دم;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    صحيح		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *بعدی;		// chain of active apc-s for auto-invalidation
};

ساكن AutoPatCmd_T *فعال_apc_list = عدم; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#تعریف_کن FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    لكل ((ap) = اولی_autopat[(صحيح)(event)]; (ap) != عدم; (ap) = (ap)->بعدی)

/*
 * augroups stores a list of autocmd group names.
 */
ساكن garray_T augroups = {0, 0, حجم(char_u *), 10, عدم};
#تعریف_کن AUGROUP_NAME(i) (((char_u **)augroups.ga_داده)[i])
// use get_deleted_augroup() to get this
ساكن char_u *deleted_augroup = عدم;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
ساكن صحيح current_augroup = AUGROUP_DEFAULT;

ساكن صحيح au_need_clean = FALSE;   // need to delete marked patterns

ساكن event_T event_name2nr(char_u *شروع, char_u **انتها);
ساكن string_T *event_nr2name(event_T event);
ساكن صحيح au_get_grouparg(char_u **argp);
ساكن صحيح do_autocmd_event(event_T event, char_u *pat, صحيح once, صحيح nested, char_u *cmd, صحيح forceit, صحيح گروه, صحيح flags);
ساكن صحيح apply_autocmds_گروه(event_T event, char_u *fname, char_u *fname_io, صحيح force, صحيح گروه, buf_T *buf, exarg_T *eap);
ساكن فراغ auto_بعدی_pat(AutoPatCmd_T *apc, صحيح stop_at_آخرین);
ساكن صحيح au_پیدا_کن_گروه(char_u *name);

ساكن event_T	آخرین_event;
ساكن صحيح	آخرین_گروه;
ساكن صحيح	autocmd_blocked = 0;	// block all autocmds

    ساكن char_u *
get_deleted_augroup(فراغ)
{
    إذا (deleted_augroup == عدم)
	deleted_augroup = (char_u *)_("--Deleted--");
    أرجع deleted_augroup;
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
	أرجع;
    إذا (ap->pat == عدم)		// pattern has been removed
	أرجع;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    پیام_putchar('\n');
    إذا (got_int)
	اذهب theend;
    إذا (event != آخرین_event || ap->گروه != آخرین_گروه)
    {
	string_T    *event_name;

	إذا (ap->گروه != AUGROUP_DEFAULT)
	{
	    إذا (AUGROUP_NAME(ap->گروه) == عدم)
		پیام_puts_attr((حرف *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    وإلا
		پیام_puts_attr((حرف *)AUGROUP_NAME(ap->گروه), HL_ATTR(HLF_T));
	    پیام_puts("  ");
	}
	event_name = event_nr2name(event);
	پیام_puts_attr((حرف *)event_name->string, HL_ATTR(HLF_T));
	آخرین_event = event;
	آخرین_گروه = ap->گروه;
	پیام_putchar('\n');
	إذا (got_int)
	    اذهب theend;
    }

    إذا (ap->pat == عدم)
	اذهب theend;  // timer might have cleared the pattern or group

    پیام_ستون = 4;
    پیام_outtrans(ap->pat);

    لكل (ac = ap->cmds; ac != عدم; ac = ac->بعدی)
    {
	إذا (ac->cmd == عدم)		// skip removed commands
	    استمر;

	إذا (پیام_ستون >= 14)
	    پیام_putchar('\n');
	پیام_ستون = 14;
	إذا (got_int)
	    اذهب theend;
	پیام_outtrans(ac->cmd);
#اگر_تعریف_شده FEAT_EVAL
	إذا (p_verbose > 0)
	    آخرین_مجموعه_پیام(ac->script_ctx);
#پایان_اگر
	إذا (got_int)
	    اذهب theend;
	إذا (ac->بعدی != عدم)
	{
	    پیام_putchar('\n');
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

    لكل (ac = ap->cmds; ac != عدم; ac = ac->بعدی)
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
	أرجع;

    // loop over all events
    لكل (event = (event_T)0; (صحيح)event < رقم_EVENTS;
					    event = (event_T)((صحيح)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(اولی_autopat[(صحيح)event]);
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
		    *prev_ac = ac->بعدی;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		وإلا
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->بعدی);
		}
	    }

	    إذا (ap->pat != عدم && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    إذا (ap->pat == عدم)
	    {
		إذا (ap->بعدی == عدم)
		{
		    إذا (prev_ap == &(اولی_autopat[(صحيح)event]))
			آخرین_autopat[(صحيح)event] = عدم;
		    وإلا
			// this depends on the "next" field being the first in
			// the struct
			آخرین_autopat[(صحيح)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->بعدی;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    وإلا
		prev_ap = &(ap->بعدی);
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
    لكل (apc = فعال_apc_list; apc; apc = apc->بعدی)
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
au_new_گروه(char_u *name)
{
    صحيح		i;

    i = au_پیدا_کن_گروه(name);
    إذا (i != AUGROUP_ERROR)
	أرجع i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    لكل (i = 0; i < augroups.ga_len; ++i)
	إذا (AUGROUP_NAME(i) == عدم)
	    اكسر;
    إذا (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	أرجع AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    إذا (AUGROUP_NAME(i) == عدم)
	أرجع AUGROUP_ERROR;
    إذا (i == augroups.ga_len)
	++augroups.ga_len;

    أرجع i;
}

    ساكن فراغ
au_del_گروه(char_u *name)
{
    صحيح		i;
    event_T	event;
    AutoPat	*ap;
    صحيح		درون_use = FALSE;


    i = au_پیدا_کن_گروه(name);
    إذا (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_گروه_str), name);
	أرجع;
    }
    إذا (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_گروه));
	أرجع;
    }

    لكل (event = (event_T)0; (صحيح)event < رقم_EVENTS;
	    event = (event_T)((صحيح)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    إذا (ap->گروه == i && ap->pat != عدم)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		درون_use = TRUE;
		event = رقم_EVENTS;
		اكسر;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    إذا (درون_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    وإلا
	AUGROUP_NAME(i) = عدم;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    ساكن صحيح
au_پیدا_کن_گروه(char_u *name)
{
    صحيح	    i;

    لكل (i = 0; i < augroups.ga_len; ++i)
	إذا (AUGROUP_NAME(i) != عدم && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    أرجع i;
    أرجع AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    صحيح
au_has_گروه(char_u *name)
{
    أرجع au_پیدا_کن_گروه(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    فراغ
do_augroup(char_u *arg, صحيح del_گروه)
{
    صحيح	    i;

    إذا (del_گروه)
    {
	إذا (*arg == NUL)
	    emsg(_(e_آرگومان_required));
	وإلا
	    au_del_گروه(arg);
    }
    وإلا إذا (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    وإلا إذا (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_گروه(arg);
	إذا (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    وإلا			    // ":aug": list the group names
    {
	پیام_شروع();
	لكل (i = 0; i < augroups.ga_len; ++i)
	{
	    إذا (AUGROUP_NAME(i) != عدم)
	    {
		پیام_puts((حرف *)AUGROUP_NAME(i));
		پیام_puts("  ");
	    }
	}
	پیام_clr_eos();
	پیام_انتها();
    }
}

    فراغ
autocmd_init(فراغ)
{
    CLEAR_فیلد(aucmd_win);
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
	s = ((char_u **)(augroups.ga_داده))[i];
	إذا (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#پایان_اگر

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    صحيح
is_aucmd_win(win_T *win)
{
    لكل (صحيح i = 0; i < AUCMD_WIN_شمار; ++i)
	إذا (aucmd_win[i].auc_win_استفاده_شده && aucmd_win[i].auc_win == win)
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    ساكن event_T
event_name2nr(char_u *شروع, char_u **انتها)
{
    char_u	*p;
    keyvalue_T هدف;
    keyvalue_T *مدخل;
    ساكن keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_شاخص];
    ساكن keyvalue_T *bufread = &event_tab[BUFREAD_شاخص];

    // the event name ends with end of line, '|', a blank or a comma
    لكل (p = شروع; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    هدف.کلید = 0;
    هدف.مقدار.string = شروع;
    هدف.مقدار.درازی = (size_t)(p - شروع);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    إذا (cmp_keyvalue_مقدار_ni(&هدف, bufnewfile) == 0)
	مدخل = bufnewfile;
    وإلا إذا (cmp_keyvalue_مقدار_ni(&هدف, bufread) == 0)
	مدخل = bufread;
    وإلا
	مدخل = (keyvalue_T *)bsearch(&هدف, &event_tab, رقم_EVENTS,
				حجم(event_tab[0]), cmp_keyvalue_مقدار_ni);

    إذا (*p == ',')
	++p;
    *انتها = p;

    أرجع (مدخل == عدم) ? رقم_EVENTS : (event_T)قدر_مطلق(مدخل->کلید);
}

/*
 * Return the name for event "event".
 */
    ساكن string_T *
event_nr2name(event_T event)
{
    صحيح	    i;
    تعداد {CACHE_اندازه = 12};
    ساكن صحيح cache_tab[CACHE_اندازه];
    ساكن صحيح cache_آخرین_شاخص = -1;
    ساكن string_T unknown = STR_LITERAL_INIT("Unknown");

    إذا (cache_آخرین_شاخص < 0)
    {
	لكل (i = 0; i < CACHE_اندازه; ++i)
	    cache_tab[i] = -1;
	cache_آخرین_شاخص = CACHE_اندازه - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    لكل (i = cache_آخرین_شاخص; cache_tab[i] >= 0; )
    {
	إذا ((event_T)قدر_مطلق(event_tab[cache_tab[i]].کلید) == event)
	    أرجع &event_tab[cache_tab[i]].مقدار;

	إذا (i == 0)
	    i = CACHE_اندازه - 1;
	وإلا
	    --i;

	// are we back at the start?
	إذا (i == cache_آخرین_شاخص)
	    اكسر;
    }

    // look in the event table itself
    لكل (i = 0; i < رقم_EVENTS; ++i)
    {
	إذا ((event_T)قدر_مطلق(event_tab[i].کلید) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    إذا (cache_آخرین_شاخص == CACHE_اندازه - 1)
		cache_آخرین_شاخص = 0;
	    وإلا
		++cache_آخرین_شاخص;
	    cache_tab[cache_آخرین_شاخص] = i;
	    أرجع &event_tab[i].مقدار;
	}
    }

    أرجع &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    ساكن char_u *
پیدا_کن_انتها_event(
    char_u  *arg,
    صحيح	    have_گروه)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    إذا (*arg == '*')
    {
	إذا (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_ستاره_str), arg);
	    أرجع عدم;
	}
	pat = arg + 1;
    }
    وإلا
    {
	لكل (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    إذا ((صحيح)event_name2nr(pat, &p) >= رقم_EVENTS)
	    {
		إذا (have_گروه)
		    semsg(_(e_no_such_event_str), pat);
		وإلا
		    semsg(_(e_no_such_گروه_or_event_str), pat);
		أرجع عدم;
	    }
	}
    }
    أرجع pat;
}

/*
 * Find the command of an ":autocmd" with argument "arg", the part after the
 * events, the pattern and any "++once"/"++nested".  Returns NULL when there is
 * no command.  Does not modify "arg" and gives no error messages.
 */
    char_u *
au_پیدا_کن_cmd_arg(char_u *arg)
{
    char_u	*pat;
    char_u	*cmd;
    صحيح		گروه;

    إذا (*arg == '|')
	أرجع عدم;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    گروه = au_get_grouparg(&arg);
    pat = گروه == AUGROUP_ERROR
			  ? عدم : پیدا_کن_انتها_event(arg, گروه != AUGROUP_ALL);
    --emsg_off;
    إذا (pat == عدم)
	أرجع عدم;

    pat = skipwhite(pat);
    إذا (*pat == NUL || *pat == '|')
	أرجع عدم;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    طالما (*cmd != NUL
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
	إذا (!درون_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    أرجع *cmd == NUL ? عدم : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    صحيح
event_ignored(event_T event, char_u *ei)
{
    صحيح ignored = FALSE;
    طالما (*ei != NUL)
    {
	صحيح unignore = *ei == '-';
	ei += unignore;
	إذا (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].کلید <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	وإلا إذا (event_name2nr(ei, &ei) == event)
	{
	    إذا (unignore)
		أرجع FALSE;
	    وإلا
		ignored = TRUE;
	}
    }

    أرجع ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    صحيح
check_ei(char_u *ei)
{
    صحيح	win = ei != p_ei;

    طالما (*ei)
    {
	إذا (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	وإلا
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    إذا (event == رقم_EVENTS || (win && event_tab[event].کلید > 0))
		أرجع FAIL;
	}
    }

    أرجع OK;
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
	أرجع عدم;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    إذا (new_ei == عدم)
    {
	vim_free(save_ei);
	أرجع عدم;
    }

    إذا (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    وإلا
	STRCPY(new_ei + p_ei_len, what);
    مجموعه_string_option_مستقیم((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    أرجع save_ei;
}

    فراغ
au_event_restore(char_u *old_ei)
{
    إذا (old_ei != عدم)
    {
	مجموعه_string_option_مستقیم((char_u *)"ei", -1, old_ei,
							  OPT_FREE, SID_NONE);
	vim_free(old_ei);
    }
}
#پایان_اگر  // FEAT_SYN_HL

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
do_autocmd(exarg_T *eap, char_u *arg_درون, صحيح forceit)
{
    char_u	*arg = arg_درون;
    char_u	*pat;
    char_u	*envpat = عدم;
    char_u	*cmd;
    صحيح		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = عدم;
    صحيح		nested = FALSE;
    صحيح		once = FALSE;
    صحيح		گروه;
    صحيح		i;
    صحيح		flags = 0;

    إذا (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	گروه = AUGROUP_ALL;	// no argument, use all groups
    }
    وإلا
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	گروه = au_get_grouparg(&arg);
	إذا (arg == عدم)	    // out of memory
	    أرجع;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = پیدا_کن_انتها_event(arg, گروه != AUGROUP_ALL);
    إذا (pat == عدم)
	أرجع;

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
	طالما (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	إذا (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	إذا (vim_strchr(pat, '$') != عدم || vim_strchr(pat, '~') != عدم)
	{
#اگر_تعریف_شده BACKSLASH_درون_FILENAME
	    صحيح	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#پایان_اگر
	    envpat = expand_env_save(pat);
#اگر_تعریف_شده BACKSLASH_درون_FILENAME
	    p_ssl = p_ssl_save;
#پایان_اگر
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
		    semsg(_(e_duplicate_آرگومان_str), "++once");
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
		    semsg(_(e_duplicate_آرگومان_str), "++nested");
		    اذهب err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    إذا (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		إذا (درون_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_میانگین_plusplus_nested));
		    اذهب err_exit;
		}
		إذا (nested)
		{
		    semsg(_(e_duplicate_آرگومان_str), "nested");
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
		cmd = may_get_cmd_بلوک(eap, cmd, &tofree, &flags);

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
	پیام_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    آخرین_event = (event_T)-1;		// for listing the event name
    آخرین_گروه = AUGROUP_ERROR;		// for listing the group name
    إذا (*arg == '*' || *arg == NUL || *arg == '|')
    {
	إذا (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	وإلا
	    لكل (event = (event_T)0; (صحيح)event < رقم_EVENTS;
					     event = (event_T)((صحيح)event + 1))
		إذا (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, گروه, flags) == FAIL)
		    اكسر;
    }
    وإلا
    {
	طالما (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    إذا (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, گروه, flags) == FAIL)
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
    char_u	*گروه_name;
    char_u	*p;
    char_u	*arg = *argp;
    صحيح		گروه = AUGROUP_ALL;

    لكل (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    إذا (p <= arg)
	أرجع AUGROUP_ALL;

    گروه_name = vim_strnsave(arg, p - arg);
    إذا (گروه_name == عدم)		// out of memory
	أرجع AUGROUP_ERROR;
    گروه = au_پیدا_کن_گروه(گروه_name);
    إذا (گروه == AUGROUP_ERROR)
	گروه = AUGROUP_ALL;	// no match, use all groups
    وإلا
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(گروه_name);
    أرجع گروه;
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
    صحيح		گروه,
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

    إذا (گروه == AUGROUP_ALL)
	findgroup = current_augroup;
    وإلا
	findgroup = گروه;
    allgroups = (گروه == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    إذا (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    إذا (forceit)  // delete the AutoPat, if it's in the current group
	    {
		إذا (ap->گروه == findgroup)
		    au_remove_pat(ap);
	    }
	    وإلا إذا (گروه == AUGROUP_ALL || ap->گروه == گروه)
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
	إذا (!forceit && *cmd != NUL && آخرین_autopat[(صحيح)event] != عدم)
	    prev_ap = &آخرین_autopat[(صحيح)event];
	وإلا
	    prev_ap = &اولی_autopat[(صحيح)event];
	طالما ((ap = *prev_ap) != عدم)
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
		إذا ((allgroups || ap->گروه == findgroup)
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
			إذا (*cmd != NUL && ap->بعدی == عدم)
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
		    وإلا إذا (ap->بعدی == عدم)
			اكسر;
		}
	    }
	    prev_ap = &ap->بعدی;
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
		    semsg(_(e_میانگیر_nr_invalid_میانگیر_عدد), buflocal_nr);
		    أرجع FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		إذا (ap == عدم)
		    أرجع FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		إذا (ap->pat == عدم)
		{
		    vim_free(ap);
		    أرجع FAIL;
		}

#اگر_تعریف_شده FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		إذا (event == EVENT_MODECHANGED && !has_modechanged())
		    get_نما(آخرین_نما);
#پایان_اگر
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
			snapshot_windows_scroll_اندازه();
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
		    reg_pat = file_pat_مقصد_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    إذا (reg_pat != عدم)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    إذا (reg_pat == عدم || ap->reg_prog == عدم)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			أرجع FAIL;
		    }
		}
		ap->cmds = عدم;
		*prev_ap = ap;
		آخرین_autopat[(صحيح)event] = ap;
		ap->بعدی = عدم;
		إذا (گروه == AUGROUP_ALL)
		    ap->گروه = current_augroup;
		وإلا
		    ap->گروه = گروه;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    طالما ((ac = *prev_ac) != عدم)
		prev_ac = &ac->بعدی;
	    ac = ALLOC_ONE(AutoCmd);
	    إذا (ac == عدم)
		أرجع FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    إذا (flags & UC_VIM9)
		ac->script_ctx.sc_نسخه = SCRIPT_نسخه_VIM9;
#اگر_تعریف_شده FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#پایان_اگر
	    إذا (ac->cmd == عدم)
	    {
		vim_free(ac);
		أرجع FAIL;
	    }
	    ac->بعدی = عدم;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    أرجع OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    صحيح
do_doautocmd(
    char_u	*arg_شروع,
    صحيح		do_پیام,	    // give message for no matching autocmds?
    صحيح		*did_something)
{
    char_u	*arg = arg_شروع;
    char_u	*fname;
    صحيح		nothing_انجامشده = TRUE;
    صحيح		گروه;

    إذا (did_something != عدم)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    گروه = au_get_grouparg(&arg);
    إذا (arg == عدم)	    // out of memory
	أرجع FAIL;

    إذا (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	أرجع FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = پیدا_کن_انتها_event(arg, گروه != AUGROUP_ALL);
    إذا (fname == عدم)
	أرجع FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    طالما (*arg && !دوسر_excmd(*arg) && !VIM_ISWHITE(*arg))
	إذا (apply_autocmds_گروه(event_name2nr(arg, &arg),
				      fname, عدم, TRUE, گروه, curbuf, عدم))
	    nothing_انجامشده = FALSE;

    إذا (nothing_انجامشده && do_پیام
#اگر_تعریف_شده FEAT_EVAL
		&& !aborting()
#پایان_اگر
	       )
	smsg(_("No matching autocommands: %s"), arg_شروع);
    إذا (did_something != عدم)
	*did_something = !nothing_انجامشده;

#اگر_تعریف_شده FEAT_EVAL
    أرجع aborting() ? FAIL : OK;
#وإلا
    أرجع OK;
#پایان_اگر
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

	مجموعه_bufref(&bufref, buf);

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
	أرجع FALSE;
    }
    أرجع TRUE;
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
#اگر_تعریف_شده FEAT_AUTOCHDIR
    صحيح		save_acd;
#پایان_اگر
    صحيح		same_میانگیر = buf == curbuf;

    // Find a window that is for the new buffer
    إذا (same_میانگیر)		// be quick when buf is curbuf
	win = curwin;
    وإلا
	FOR_ALL_WINDOWS(win)
	    إذا (win->w_میانگیر == buf)
		اكسر;

    // Allocate a window when needed.
    win_T *auc_win = عدم;
    صحيح auc_idx = AUCMD_WIN_شمار;
    إذا (win == عدم)
    {
	لكل (auc_idx = 0; auc_idx < AUCMD_WIN_شمار; ++auc_idx)
	    إذا (!aucmd_win[auc_idx].auc_win_استفاده_شده)
	    {
		إذا (aucmd_win[auc_idx].auc_win == عدم)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		إذا (auc_win != عدم)
		    aucmd_win[auc_idx].auc_win_استفاده_شده = TRUE;
		اكسر;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	إذا (auc_win == عدم)
	    أرجع;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == عدم ? 0 : prevwin->w_id;
#اگر_تعریف_شده FEAT_کار_CHANNEL
    إذا (bt_prompt(curbuf))
	aco->save_prompt_درج = curbuf->b_prompt_درج;
#پایان_اگر

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
	بلوک_autocmds();
	make_snapshot(SNAP_AUCMD_IDX);
	save_ea = p_ea;
	p_ea = FALSE;

#اگر_تعریف_شده FEAT_AUTOCHDIR
	// Prevent chdir() call in win_enter_ext(), through do_autochdir().
	save_acd = p_acd;
	p_acd = FALSE;
#پایان_اگر

	(فراغ)win_تقسیم_ins(0, WSP_بالاترین | WSP_FORCE_جا, auc_win, 0, عدم);
	win_comp_pos();   // recompute window positions
	p_ea = save_ea;
#اگر_تعریف_شده FEAT_AUTOCHDIR
	p_acd = save_acd;
#پایان_اگر
	unblock_autocmds();
	curwin = auc_win;
    }
    curbuf = buf;
    aco->new_curwin_id = curwin->w_id;
    مجموعه_bufref(&aco->new_curbuf, curbuf);

    aco->save_VIsual_فعال = VIsual_فعال;
    إذا (!same_میانگیر)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_فعال = FALSE;
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
	بلوک_autocmds();
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
		    اذهب win_یافت;
		}
	    }
	}
win_یافت:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(فراغ)winframe_remove(curwin, &dummy, عدم, عدم);
	win_remove(curwin, عدم);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_استفاده_شده = FALSE;
	آخرین_وضعیت(FALSE);	    // may need to remove last status line

	إذا (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_پیدا_کن_by_id(aco->save_curwin_id);
	إذا (save_curwin != عدم)
	    curwin = save_curwin;
	وإلا
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_میانگیر;
#اگر_تعریف_شده FEAT_کار_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_پنجره(curwin);
	إذا (bt_prompt(curbuf))
	    curbuf->b_prompt_درج = aco->save_prompt_درج;
#پایان_اگر
	prevwin = win_پیدا_کن_by_id(aco->save_prevwin_id);
#اگر_تعریف_شده FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#پایان_اگر
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	إذا (awp->w_localdir != عدم)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_فعال = aco->save_VIsual_فعال;
	check_cursor();
	إذا (curwin->w_topline > curbuf->b_ml.ml_سطر_شمار)
	{
	    curwin->w_topline = curbuf->b_ml.ml_سطر_شمار;
#اگر_تعریف_شده FEAT_DIFF
	    curwin->w_topfill = 0;
#پایان_اگر
	}
#إذا defined(FEAT_GUI)
	إذا (gui.درون_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_چپ], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_راست], FALSE);
	    gui_may_update_scrollbars();
	}
#پایان_اگر
    }
    وإلا
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_پیدا_کن_by_id(aco->save_curwin_id);
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
#پایان_اگر
		--curbuf->b_nwindows;
		curbuf = aco->new_curbuf.br_buf;
		curwin->w_میانگیر = curbuf;
		++curbuf->b_nwindows;
	    }

	    curwin = save_curwin;
	    curbuf = curwin->w_میانگیر;
	    prevwin = win_پیدا_کن_by_id(aco->save_prevwin_id);

	    // In case the autocommand moves the cursor to a position that
	    // does not exist in curbuf.
	    VIsual_فعال = aco->save_VIsual_فعال;
	    check_cursor();
	}
    }

    VIsual_فعال = aco->save_VIsual_فعال;
    check_cursor();	    // just in case lines got deleted
    إذا (VIsual_فعال)
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
    أرجع apply_autocmds_گروه(event, fname, fname_io, force,
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
    أرجع apply_autocmds_گروه(event, fname, fname_io, force,
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

#اگر_تعریف_شده FEAT_EVAL
    إذا (should_abort(*retval))
	أرجع FALSE;
#پایان_اگر

    did_cmd = apply_autocmds_گروه(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, عدم);
    إذا (did_cmd
#اگر_تعریف_شده FEAT_EVAL
	    && aborting()
#پایان_اگر
	    )
	*retval = FAIL;
    أرجع did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    ساكن صحيح
has_cursorhold(فراغ)
{
    أرجع (اولی_autopat[(صحيح)(get_real_state() == نما_نرمال_BUSY
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
	    && !ins_compl_فعال())
    {
	state = get_real_state();
	إذا (state == نما_نرمال_BUSY || (state & نما_درج) != 0)
	    أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    صحيح
has_winresized(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_WINRESIZED] != عدم);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    صحيح
has_winscrolled(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_WINSCROLLED] != عدم);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    صحيح
has_cursormoved(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_CURSORMOVED] != عدم);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    صحيح
has_cursormovedI(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_CURSORMOVEDI] != عدم);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    صحيح
has_textchanged(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_TEXTCHANGED] != عدم);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    صحيح
has_textchangedI(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_TEXTCHANGEDI] != عدم);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    صحيح
has_textchangedP(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_TEXTCHANGEDP] != عدم);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    صحيح
has_insertcharpre(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_INSERTCHARPRE] != عدم);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    صحيح
has_keyinputpre(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_KEYINPUTPRE] != عدم);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    صحيح
has_cmdundefined(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_CMDUNDEFINED] != عدم);
}

#إذا defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    صحيح
has_textputpost(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_TEXTPUTPOST] != عدم);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    صحيح
has_textputpre(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_TEXTPUTPRE] != عدم);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    صحيح
has_textyankpost(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_TEXTYANKPOST] != عدم);
}
#پایان_اگر

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    صحيح
has_completechanged(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_COMPLETECHANGED] != عدم);
}
#پایان_اگر

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    صحيح
has_modechanged(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_MODECHANGED] != عدم);
}
#پایان_اگر

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    ساكن صحيح
apply_autocmds_گروه(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    صحيح		force,	     // when TRUE, ignore autocmd_busy
    صحيح		گروه,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap استفادهنشده) // command arguments
{
    char_u	*sfname = عدم;	// short file name
    char_u	*دم;
    صحيح		save_تغییریافته;
    buf_T	*old_curbuf;
    صحيح		retval = FALSE;
    char_u	*save_autocmd_fname;
    صحيح		save_autocmd_fname_پر;
    صحيح		save_autocmd_bufnr;
    char_u	*save_autocmd_تطابق;
    صحيح		save_autocmd_busy;
    صحيح		save_autocmd_nested;
    ساكن صحيح	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#اگر_تعریف_شده FEAT_EVAL
    funccal_مدخل_T funccal_مدخل;
    char_u	*save_cmdarg;
    دراز	save_cmdbang;
#پایان_اگر
    ساكن صحيح	filechangeshell_busy = FALSE;
#اگر_تعریف_شده FEAT_PROFILE
    proftime_T	wait_زمان;
#پایان_اگر
    صحيح		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    صحيح		save_کلید_Typed = کلید_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    إذا (event == رقم_EVENTS || اولی_autopat[(صحيح)event] == عدم
	    || autocmd_blocked > 0)
	اذهب BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    إذا (autocmd_busy && !(force || autocmd_nested))
	اذهب BYPASS_AU;

#اگر_تعریف_شده FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    إذا (aborting())
	اذهب BYPASS_AU;
#پایان_اگر

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
    إذا (buf == curbuf && event_tab[event].کلید <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    وإلا إذا (buf != عدم && event_tab[event].کلید <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    إذا (wp->w_میانگیر == buf && !event_ignored(event, wp->w_p_eiw))
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
	++aucmd_cmdline_تغییریافته_شمار;

    /*
     * Save the autocmd_* variables and info about the current buffer.
     */
    save_autocmd_fname = autocmd_fname;
    save_autocmd_fname_پر = autocmd_fname_پر;
    save_autocmd_bufnr = autocmd_bufnr;
    save_autocmd_تطابق = autocmd_تطابق;
    save_autocmd_busy = autocmd_busy;
    save_autocmd_nested = autocmd_nested;
    save_تغییریافته = curbuf->b_تغییریافته;
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
	وإلا إذا (fname != عدم && !دوسر_excmd(*fname))
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
    autocmd_fname_پر = FALSE; // call FullName_save() later

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
#اگر_تعریف_شده FEAT_SYN_HL
	    إذا (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    وإلا
#پایان_اگر
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
	    autocmd_fname_پر = TRUE; // don't expand it later
	}
	وإلا
	    fname = پر_Name_save(fname, FALSE);
    }
    إذا (fname == عدم)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	اذهب BYPASS_AU;
    }

#اگر_تعریف_شده BACKSLASH_درون_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    إذا (sfname != عدم)
	forward_slash(sfname);
    forward_slash(fname);
#پایان_اگر

#اگر_تعریف_شده VMS
    // remove version for correct match
    إذا (sfname != عدم)
	vms_remove_نسخه(sfname);
    vms_remove_نسخه(fname);
#پایان_اگر

    /*
     * Set the name to be used for <amatch>.
     */
    autocmd_تطابق = fname;


    // Don't redraw while doing autocommands.
    ++RedrawingDisabled;

    // name and lnum are filled in later
    estack_افزودن(ETYPE_AUCMD, عدم, 0);
    ESTACK_CHECK_راهاندازی;

    save_current_sctx = current_sctx;

#اگر_تعریف_شده FEAT_EVAL
# ifdef FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	prof_فرزند_enter(&wait_زمان); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_مدخل);
#پایان_اگر

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    إذا (!autocmd_busy)
    {
	save_جستجو_patterns();
	إذا (!ins_compl_فعال())
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

    دم = gettail(fname);

    // Find first autocommand that matches
    CLEAR_فیلد(patcmd);
    patcmd.curpat = اولی_autopat[(صحيح)event];
    patcmd.گروه = گروه;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.دم = دم;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_بعدی_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    إذا (patcmd.curpat != عدم)
    {
	// add to active_apc_list
	patcmd.بعدی = فعال_apc_list;
	فعال_apc_list = &patcmd;

#اگر_تعریف_شده FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (دراز)get_vim_var_nr(VV_CMDBANG);
	إذا (eap != عدم)
	{
	    save_cmdarg = مجموعه_cmdarg(eap, عدم);
	    مجموعه_vim_var_nr(VV_CMDBANG, (دراز)eap->forceit);
	}
	وإلا
	    save_cmdarg = عدم;	// avoid gcc warning
#پایان_اگر
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	لكل (ap = patcmd.curpat; ap->بعدی != عدم; ap = ap->بعدی)
	    ap->آخرین = FALSE;
	ap->آخرین = TRUE;

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
	مجموعه_pressedreturn(save_ex_pressedreturn);

	إذا (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#اگر_تعریف_شده FEAT_EVAL
	إذا (eap != عدم)
	{
	    (فراغ)مجموعه_cmdarg(عدم, save_cmdarg);
	    مجموعه_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#پایان_اگر
	// delete from active_apc_list
	إذا (فعال_apc_list == &patcmd)	    // just in case
	    فعال_apc_list = patcmd.بعدی;
    }

    إذا (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_برداشت();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_پر = save_autocmd_fname_پر;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_تطابق = save_autocmd_تطابق;
    current_sctx = save_current_sctx;
#اگر_تعریف_شده FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	prof_فرزند_exit(&wait_زمان);
# endif
#پایان_اگر
    کلید_Typed = save_کلید_Typed;
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
	restore_جستجو_patterns();
	إذا (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	طالما (au_pending_free_buf != عدم)
	{
	    buf_T *b = au_pending_free_buf->b_بعدی;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	طالما (au_pending_free_win != عدم)
	{
	    win_T *w = au_pending_free_win->w_بعدی;

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
	إذا (curbuf->b_تغییریافته != save_تغییریافته)
	    need_maketitle = TRUE;
	curbuf->b_تغییریافته = save_تغییریافته;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    إذا (event == EVENT_BUFWIPEOUT && buf != عدم)
	aubuflocal_remove(buf);

    إذا (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    أرجع retval;
}

#اگر_تعریف_شده FEAT_EVAL
ساكن char_u	*old_termresponse = عدم;
ساكن char_u	*old_termu7resp = عدم;
ساكن char_u	*old_termblinkresp = عدم;
ساكن char_u	*old_termrbgresp = عدم;
ساكن char_u	*old_termrfgresp = عدم;
ساكن char_u	*old_termstyleresp = عدم;
#پایان_اگر

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    فراغ
بلوک_autocmds(فراغ)
{
#اگر_تعریف_شده FEAT_EVAL
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
#پایان_اگر
    ++autocmd_blocked;
}

    فراغ
unblock_autocmds(فراغ)
{
    --autocmd_blocked;

#اگر_تعریف_شده FEAT_EVAL
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
#پایان_اگر
}

    صحيح
is_autocmd_blocked(فراغ)
{
    أرجع autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    ساكن فراغ
auto_بعدی_pat(
    AutoPatCmd_T *apc,
    صحيح		stop_at_آخرین)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*مدخل;

    مدخل = ((estack_T *)exestack.ga_داده) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(مدخل->es_name);
    مدخل->es_info.aucmd = عدم;

    لكل (ap = apc->curpat; ap != عدم && !got_int; ap = ap->بعدی)
    {
	apc->curpat = عدم;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	إذا (ap->pat != عدم && ap->cmds != عدم
		&& (apc->گروه == AUGROUP_ALL || apc->گروه == ap->گروه))
	{
	    // execution-condition
	    إذا (ap->buflocal_nr == 0
		    ? (تطابق_file_pat(عدم, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->دم, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		حرف	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->درازی + ap->patlen + 1);
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
		مدخل->es_name = namep;
		مدخل->es_info.aucmd = apc;

		apc->curpat = ap;
		apc->nextcmd = ap->cmds;
		// mark last command
		لكل (cp = ap->cmds; cp->بعدی != عدم; cp = cp->بعدی)
		    cp->آخرین = FALSE;
		cp->آخرین = TRUE;
	    }
	    سطر_breakcheck();
	    إذا (apc->curpat != عدم)	    // found a match
		اكسر;
	}
	إذا (stop_at_آخرین && ap->آخرین)
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
    أرجع &acp->script_ctx;
}
#پایان_اگر

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	صحيح c استفادهنشده,
	فراغ *cookie,
	صحيح indent استفادهنشده,
	getline_opt_T options استفادهنشده)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    إذا (acp->curpat == عدم)
	أرجع عدم;

    // repeat until we find an autocommand to execute
    لكل (;;)
    {
	// skip removed commands
	طالما (acp->nextcmd != عدم && acp->nextcmd->cmd == عدم)
	    إذا (acp->nextcmd->آخرین)
		acp->nextcmd = عدم;
	    وإلا
		acp->nextcmd = acp->nextcmd->بعدی;

	إذا (acp->nextcmd != عدم)
	    اكسر;

	// at end of commands, find next pattern that matches
	إذا (acp->curpat->آخرین)
	    acp->curpat = عدم;
	وإلا
	    acp->curpat = acp->curpat->بعدی;
	إذا (acp->curpat != عدم)
	    auto_بعدی_pat(acp, TRUE);
	إذا (acp->curpat == عدم)
	    أرجع عدم;
    }

    ac = acp->nextcmd;

    إذا (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	پیام_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    إذا (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    إذا (ac->آخرین)
	acp->nextcmd = عدم;
    وإلا
	acp->nextcmd = ac->بعدی;
    أرجع retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    ساكن صحيح
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_پر)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*دم = gettail(sfname);
    صحيح		retval = FALSE;

    إذا (اولی_autopat[(صحيح)event] == عدم)
	أرجع FALSE;

    fname = is_پر ? vim_strsave(sfname) : پر_Name_save(sfname, FALSE);
    إذا (fname == عدم)
	أرجع FALSE;

#اگر_تعریف_شده BACKSLASH_درون_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    إذا (sfname != عدم)
	forward_slash(sfname);
    forward_slash(fname);
#پایان_اگر

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	إذا (ap->pat != عدم && ap->cmds != عدم
	      && (ap->buflocal_nr == 0
		? تطابق_file_pat(عدم, &ap->reg_prog,
					  fname, sfname, دم, ap->allow_dirs)
		: buf != عدم && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    اكسر;
	}

    vim_free(fname);
#اگر_تعریف_شده BACKSLASH_درون_FILENAME
    vim_free(sfname);
#پایان_اگر

    أرجع retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    صحيح
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    أرجع has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    صحيح
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    أرجع has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp استفادهنشده, صحيح idx)
{
    إذا (idx == augroups.ga_len)		// add "END" add the end
	أرجع (char_u *)"END";
    إذا (idx < 0 || idx >= augroups.ga_len)	// end of list
	أرجع عدم;
    إذا (AUGROUP_NAME(idx) == عدم || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	أرجع (char_u *)"";
    أرجع AUGROUP_NAME(idx);		// return a name
}

ساكن صحيح include_groups = FALSE;

    char_u  *
مجموعه_context_درون_autocmd(
    expand_T	*xp,
    char_u	*arg,
    صحيح		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    صحيح		گروه;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    گروه = au_get_grouparg(&arg);
    إذا (گروه == AUGROUP_ERROR)
	أرجع عدم;
    // If there only is a group name that's what we expand.
    إذا (*arg == NUL && گروه != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	گروه = AUGROUP_ALL;
    }

    // skip over event name
    لكل (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	إذا (*p == ',')
	    arg = p + 1;
    إذا (*p == NUL)
    {
	إذا (گروه == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_الگو = arg;
	أرجع عدم;
    }

    // skip over pattern
    arg = skipwhite(p);
    طالما (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
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
	    إذا (!درون_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	أرجع arg;			    // expand (next) command
    }

    إذا (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    وإلا
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    أرجع عدم;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp استفادهنشده, صحيح idx)
{
    صحيح i;

    إذا (idx < augroups.ga_len)		// First list group names, if wanted
    {
	إذا (!include_groups || AUGROUP_NAME(idx) == عدم
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    أرجع (char_u *)"";	// skip deleted entries
	أرجع AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    إذا (i < 0 || i >= رقم_EVENTS)
	أرجع عدم;

    أرجع event_tab[i].مقدار.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_گروه(expand_T *xp استفادهنشده, صحيح idx, صحيح win)
{
    إذا (idx < 0 || idx >= رقم_EVENTS)
	أرجع عدم;

    إذا (!win)
	أرجع event_tab[idx].مقدار.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    صحيح j = 0;
    لكل (صحيح i = 0; i < رقم_EVENTS; ++i)
    {
	j += event_tab[i].کلید <= 0;
	إذا (j == idx + 1)
	    أرجع event_tab[i].مقدار.string;
    }

    أرجع عدم;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    صحيح
has_tabclosedpre(فراغ)
{
    أرجع (اولی_autopat[(صحيح)EVENT_TABCLOSEDPRE] != عدم);
}

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    صحيح
autocmd_supported(char_u *name)
{
    char_u *p;

    أرجع (event_name2nr(name, &p) != رقم_EVENTS);
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
    char_u	*الگو = عدم;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = عدم;
    صحيح		گروه;
    صحيح		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    إذا (arg_save == عدم)
	أرجع FALSE;
    p = vim_strchr(arg_save, '#');
    إذا (p != عدم)
	*p++ = NUL;

    // First, look for an autocmd group name
    گروه = au_پیدا_کن_گروه(arg_save);
    إذا (گروه == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	گروه = AUGROUP_ALL;
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

    الگو = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    إذا (event == رقم_EVENTS)
	اذهب theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = اولی_autopat[(صحيح)event];
    إذا (ap == عدم)
	اذهب theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    إذا (الگو != عدم && STRICMP(الگو, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    لكل ( ; ap != عدم; ap = ap->بعدی)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	إذا (ap->pat != عدم && ap->cmds != عدم
	    && (گروه == AUGROUP_ALL || ap->گروه == گروه)
	    && (الگو == عدم
		|| (buflocal_buf == عدم
		    ? fnamecmp(ap->pat, الگو) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    اكسر;
	}

theend:
    vim_free(arg_save);
    أرجع retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    ساكن فراغ
autocmd_جمع_کن_or_delete(typval_T *argvars, typval_T *rettv, صحيح delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = عدم;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*گروه_name = عدم;
    صحيح		گروه;
    char_u	*pat = عدم;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = عدم;
    char_u	*انتها;
    صحيح		once;
    صحيح		nested;
    صحيح		replace;		// replace the cmd for a group/event
    صحيح		retval = VVAL_TRUE;
    صحيح		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = VVAL_FALSE;

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (check_for_list_arg(argvars, 0) == FAIL)
	أرجع;

    aucmd_list = argvars[0].vval.v_list;
    إذا (aucmd_list == عدم)
	أرجع;

    FOR_ALL_LIST_اقلام(aucmd_list, li)
    {
	VIM_CLEAR(گروه_name);
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

	di = dict_پیدا_کن(event_dict, (char_u *)"event", -1);
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

	گروه_name = dict_get_string(event_dict, "group", TRUE);
	إذا (گروه_name == عدم || *گروه_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    گروه = current_augroup;
	وإلا
	{
	    گروه = au_پیدا_کن_گروه(گروه_name);
	    إذا (گروه == AUGROUP_ERROR)
	    {
		إذا (delete)
		{
		    semsg(_(e_no_such_گروه_str), گروه_name);
		    retval = VVAL_FALSE;
		    اكسر;
		}
		// group is not found, create it now
		گروه = au_new_گروه(گروه_name);
		إذا (گروه == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_گروه_str), گروه_name);
		    retval = VVAL_FALSE;
		    اكسر;
		}

		current_augroup = گروه;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	إذا (dict_has_کلید(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_عدد_def(event_dict, "bufnr", -1);
	    إذا (bnum == -1)
		استمر;

	    vim_snprintf((حرف *)IObuff, IOSIZE, "<buffer=%d>", (صحيح)bnum);
	    pat = IObuff;
	}
	وإلا
	{
	    di = dict_پیدا_کن(event_dict, (char_u *)"pattern", -1);
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
							گروه, 0) == FAIL)
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
	    انتها = عدم;
	    طالما (TRUE)
	    {
		إذا (event_list != عدم)
		{
		    إذا (eli == عدم)
			eli = event_list->lv_اولی;
		    وإلا
			eli = eli->li_بعدی;
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

		event = event_name2nr(p, &انتها);
		إذا (event == رقم_EVENTS || *انتها != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    اكسر;
		}
		إذا (pat != عدم)
		{
		    إذا (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, گروه, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			اكسر;
		    }
		}
		وإلا إذا (pat_list != عدم)
		{
		    FOR_ALL_LIST_اقلام(pat_list, pli)
		    {
			إذا (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == عدم)
			{
			    emsg(_(e_string_required));
			    استمر;
			}
			إذا (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, گروه, 0) ==
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
		    p = انتها;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	إذا (delete && گروه_name != عدم &&
		(event_name == عدم || event_name[0] == NUL)
		&& (pat == عدم || pat[0] == NUL)
		&& (cmd == عدم || cmd[0] == NUL))
	    au_del_گروه(گروه_name);
    }

    VIM_CLEAR(گروه_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_عدد = retval;
}

/*
 * autocmd_add() function
 */
    فراغ
f_autocmd_جمع_کن(typval_T *argvars, typval_T *rettv)
{
    autocmd_جمع_کن_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    فراغ
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_جمع_کن_or_delete(argvars, rettv, TRUE);
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
    صحيح		گروه = AUGROUP_ALL;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;
    إذا (check_for_opt_dict_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	إذا (dict_has_کلید(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    إذا (name == عدم)
		أرجع;

	    إذا (*name == NUL)
		گروه = AUGROUP_DEFAULT;
	    وإلا
	    {
		گروه = au_پیدا_کن_گروه(name);
		إذا (گروه == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_گروه_str), name);
		    vim_free(name);
		    أرجع;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	إذا (dict_has_کلید(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    إذا (name == عدم)
		أرجع;

	    إذا (name[0] == '*' && name[1] == NUL)
		event_arg = رقم_EVENTS;
	    وإلا
	    {
		keyvalue_T هدف;
		keyvalue_T *مدخل;

		هدف.کلید = 0;
		هدف.مقدار.string = name;
		هدف.مقدار.درازی = STRLEN(هدف.مقدار.string);
		مدخل = (keyvalue_T *)bsearch(&هدف, &event_tab,
		    رقم_EVENTS, حجم(event_tab[0]), cmp_keyvalue_مقدار_ni);
		إذا (مدخل == عدم)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    أرجع;
		}
		event_arg = (event_T)قدر_مطلق(مدخل->کلید);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	إذا (dict_has_کلید(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    إذا (pat == عدم)
		أرجع;
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
	    string_T	گروه_name;

	    إذا (ap->pat == عدم)		// pattern has been removed
		استمر;

	    إذا (گروه != AUGROUP_ALL && گروه != ap->گروه)
		استمر;

	    إذا (pat != عدم && STRCMP(pat, ap->pat) != 0)
		استمر;

	    گروه_name.string = get_augroup_name(عدم, ap->گروه);
	    إذا (گروه_name.string == عدم)
		STR_LITERAL_مجموعه(گروه_name, "");
	    وإلا
		گروه_name.درازی = STRLEN(گروه_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    لكل (ac = ap->cmds; ac != عدم; ac = ac->بعدی)
	    {
		event_dict = dict_alloc();
		إذا (event_dict == عدم
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    أرجع;
		}

		إذا (dict_جمع_کن_string_len(event_dict, "event",
			event_name->string, (صحيح)event_name->درازی) == FAIL
			|| dict_جمع_کن_string_len(event_dict, "group",
			    گروه_name.string, (صحيح)گروه_name.درازی) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_جمع_کن_عدد(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_جمع_کن_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_جمع_کن_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_جمع_کن_bool(event_dict, "once", ac->once) == FAIL
			|| dict_جمع_کن_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    أرجع;
		}
	    }
	}
    }

    vim_free(pat);
}

#پایان_اگر
