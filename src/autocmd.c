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
    حرف	    آخري;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    بنية AutoCmd  *ايندڙ;		// next AutoCmd in list
} AutoCmd;

تعريفنوع بنية AutoPat
{
    بنية AutoPat  *ايندڙ;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    صحيح		    گروهه;		// group ID
    صحيح		    patlen;		// strlen() of pat
    صحيح		    buflocal_nr;	// !=0 for buffer-local AutoPat
    حرف	    allow_dirs;		// Pattern may match whole path
    حرف	    آخري;		// last pattern for apply_autocmds()
} AutoPat;

//
// special cases:
// BufNewFile and BufRead are searched for ALOT (especially at startup)
// so we pre-determine their index into the event_tab[] table for fast access.
// Keep these values in sync with event_tab[]!
#وصف_ڪريو BUFNEWFILE_اشاريو 9
#وصف_ڪريو BUFREAD_اشاريو 10

// Must be sorted by the 'value' field because it is used by bsearch()!
// Events with positive keys aren't allowed in 'eventignorewin'.
ساكن keyvalue_T event_tab[انگ_EVENTS] = {
    KEYVALUE_داخلا(-EVENT_BUFADD, "BufAdd"),
    KEYVALUE_داخلا(-EVENT_BUFADD, "BufCreate"),
    KEYVALUE_داخلا(-EVENT_BUFDELETE, "BufDelete"),
    KEYVALUE_داخلا(-EVENT_BUFENTER, "BufEnter"),
    KEYVALUE_داخلا(-EVENT_BUFFILEPOST, "BufFilePost"),
    KEYVALUE_داخلا(-EVENT_BUFFILEPRE, "BufFilePre"),
    KEYVALUE_داخلا(-EVENT_BUFHIDDEN, "BufHidden"),
    KEYVALUE_داخلا(-EVENT_BUFLEAVE, "BufLeave"),
    KEYVALUE_داخلا(-EVENT_BUFNEW, "BufNew"),
    KEYVALUE_داخلا(-EVENT_BUFNEWFILE, "BufNewFile"),	// BUFNEWFILE_INDEX
    KEYVALUE_داخلا(-EVENT_BUFREADPOST, "BufRead"),	// BUFREAD_INDEX
    KEYVALUE_داخلا(-EVENT_BUFREADCMD, "BufReadCmd"),
    KEYVALUE_داخلا(-EVENT_BUFREADPOST, "BufReadPost"),
    KEYVALUE_داخلا(-EVENT_BUFREADPRE, "BufReadPre"),
    KEYVALUE_داخلا(-EVENT_BUFUNLOAD, "BufUnload"),
    KEYVALUE_داخلا(-EVENT_BUFWINENTER, "BufWinEnter"),
    KEYVALUE_داخلا(-EVENT_BUFWINLEAVE, "BufWinLeave"),
    KEYVALUE_داخلا(-EVENT_BUFWIPEOUT, "BufWipeout"),
    KEYVALUE_داخلا(-EVENT_BUFWRITEPRE, "BufWrite"),
    KEYVALUE_داخلا(-EVENT_BUFWRITECMD, "BufWriteCmd"),
    KEYVALUE_داخلا(-EVENT_BUFWRITEPOST, "BufWritePost"),
    KEYVALUE_داخلا(-EVENT_BUFWRITEPRE, "BufWritePre"),
    KEYVALUE_داخلا(EVENT_CMDLINECHANGED, "CmdlineChanged"),
    KEYVALUE_داخلا(EVENT_CMDLINEENTER, "CmdlineEnter"),
    KEYVALUE_داخلا(EVENT_CMDLINELEAVE, "CmdlineLeave"),
    KEYVALUE_داخلا(EVENT_CMDLINELEAVEPRE, "CmdlineLeavePre"),
    KEYVALUE_داخلا(EVENT_CMDUNDEFINED, "CmdUndefined"),
    KEYVALUE_داخلا(EVENT_CMDWINENTER, "CmdwinEnter"),
    KEYVALUE_داخلا(EVENT_CMDWINLEAVE, "CmdwinLeave"),
    KEYVALUE_داخلا(EVENT_COLORSCHEME, "ColorScheme"),
    KEYVALUE_داخلا(EVENT_COLORSCHEMEPRE, "ColorSchemePre"),
    KEYVALUE_داخلا(EVENT_COMPLETECHANGED, "CompleteChanged"),
    KEYVALUE_داخلا(EVENT_COMPLETEDONE, "CompleteDone"),
    KEYVALUE_داخلا(EVENT_COMPLETEDONEPRE, "CompleteDonePre"),
    KEYVALUE_داخلا(-EVENT_CURSORHOLD, "CursorHold"),
    KEYVALUE_داخلا(-EVENT_CURSORHOLDI, "CursorHoldI"),
    KEYVALUE_داخلا(-EVENT_CURSORMOVED, "CursorMoved"),
    KEYVALUE_داخلا(-EVENT_CURSORMOVEDC, "CursorMovedC"),
    KEYVALUE_داخلا(-EVENT_CURSORMOVEDI, "CursorMovedI"),
    KEYVALUE_داخلا(EVENT_DIFFUPDATED, "DiffUpdated"),
    KEYVALUE_داخلا(EVENT_DIRCHANGED, "DirChanged"),
    KEYVALUE_داخلا(EVENT_DIRCHANGEDPRE, "DirChangedPre"),
    KEYVALUE_داخلا(EVENT_ENCODINGCHANGED, "EncodingChanged"),
    KEYVALUE_داخلا(EVENT_EXITPRE, "ExitPre"),
    KEYVALUE_داخلا(-EVENT_FILEAPPENDCMD, "FileAppendCmd"),
    KEYVALUE_داخلا(-EVENT_FILEAPPENDPOST, "FileAppendPost"),
    KEYVALUE_داخلا(-EVENT_FILEAPPENDPRE, "FileAppendPre"),
    KEYVALUE_داخلا(-EVENT_FILECHANGEDRO, "FileChangedRO"),
    KEYVALUE_داخلا(-EVENT_FILECHANGEDSHELL, "FileChangedShell"),
    KEYVALUE_داخلا(-EVENT_FILECHANGEDSHELLPOST, "FileChangedShellPost"),
    KEYVALUE_داخلا(EVENT_ENCODINGCHANGED, "FileEncoding"),
    KEYVALUE_داخلا(-EVENT_FILEREADCMD, "FileReadCmd"),
    KEYVALUE_داخلا(-EVENT_FILEREADPOST, "FileReadPost"),
    KEYVALUE_داخلا(-EVENT_FILEREADPRE, "FileReadPre"),
    KEYVALUE_داخلا(-EVENT_FILETYPE, "FileType"),
    KEYVALUE_داخلا(-EVENT_FILEWRITECMD, "FileWriteCmd"),
    KEYVALUE_داخلا(-EVENT_FILEWRITEPOST, "FileWritePost"),
    KEYVALUE_داخلا(-EVENT_FILEWRITEPRE, "FileWritePre"),
    KEYVALUE_داخلا(-EVENT_FILTERREADPOST, "FilterReadPost"),
    KEYVALUE_داخلا(-EVENT_FILTERREADPRE, "FilterReadPre"),
    KEYVALUE_داخلا(-EVENT_FILTERWRITEPOST, "FilterWritePost"),
    KEYVALUE_داخلا(-EVENT_FILTERWRITEPRE, "FilterWritePre"),
    KEYVALUE_داخلا(EVENT_FOCUSGAINED, "FocusGained"),
    KEYVALUE_داخلا(EVENT_FOCUSLOST, "FocusLost"),
    KEYVALUE_داخلا(EVENT_FUNCUNDEFINED, "FuncUndefined"),
    KEYVALUE_داخلا(EVENT_GUIENTER, "GUIEnter"),
    KEYVALUE_داخلا(EVENT_GUIFAILED, "GUIFailed"),
    KEYVALUE_داخلا(-EVENT_INSERTCHANGE, "InsertChange"),
    KEYVALUE_داخلا(-EVENT_INSERTCHARPRE, "InsertCharPre"),
    KEYVALUE_داخلا(-EVENT_INSERTENTER, "InsertEnter"),
    KEYVALUE_داخلا(-EVENT_INSERTLEAVE, "InsertLeave"),
    KEYVALUE_داخلا(-EVENT_INSERTLEAVEPRE, "InsertLeavePre"),
    KEYVALUE_داخلا(EVENT_KEYINPUTPRE, "KeyInputPre"),
    KEYVALUE_داخلا(EVENT_MENUPOPUP, "MenuPopup"),
    KEYVALUE_داخلا(EVENT_MODECHANGED, "ModeChanged"),
    KEYVALUE_داخلا(EVENT_OPTIONSET, "OptionSet"),
    KEYVALUE_داخلا(EVENT_QUICKFIXCMDPOST, "QuickFixCmdPost"),
    KEYVALUE_داخلا(EVENT_QUICKFIXCMDPRE, "QuickFixCmdPre"),
    KEYVALUE_داخلا(EVENT_QUITPRE, "QuitPre"),
    KEYVALUE_داخلا(EVENT_REMOTEREPLY, "RemoteReply"),
    KEYVALUE_داخلا(EVENT_SAFESTATE, "SafeState"),
    KEYVALUE_داخلا(EVENT_SAFESTATEAGAIN, "SafeStateAgain"),
    KEYVALUE_داخلا(EVENT_SESSIONLOADPOST, "SessionLoadPost"),
    KEYVALUE_داخلا(EVENT_SESSIONLOADPRE, "SessionLoadPre"),
    KEYVALUE_داخلا(EVENT_SESSIONWRITEPOST, "SessionWritePost"),
    KEYVALUE_داخلا(EVENT_SHELLCMDPOST, "ShellCmdPost"),
    KEYVALUE_داخلا(-EVENT_SHELLFILTERPOST, "ShellFilterPost"),
    KEYVALUE_داخلا(EVENT_SIGUSR1, "SigUSR1"),
    KEYVALUE_داخلا(EVENT_SOURCECMD, "SourceCmd"),
    KEYVALUE_داخلا(EVENT_SOURCEPOST, "SourcePost"),
    KEYVALUE_داخلا(EVENT_SOURCEPRE, "SourcePre"),
    KEYVALUE_داخلا(EVENT_SPELLFILEMISSING, "SpellFileMissing"),
    KEYVALUE_داخلا(EVENT_STDINREADPOST, "StdinReadPost"),
    KEYVALUE_داخلا(EVENT_STDINREADPRE, "StdinReadPre"),
    KEYVALUE_داخلا(EVENT_SWAPEXISTS, "SwapExists"),
    KEYVALUE_داخلا(EVENT_SYNTAX, "Syntax"),
    KEYVALUE_داخلا(EVENT_TABCLOSED, "TabClosed"),
    KEYVALUE_داخلا(EVENT_TABCLOSEDPRE, "TabClosedPre"),
    KEYVALUE_داخلا(EVENT_TABENTER, "TabEnter"),
    KEYVALUE_داخلا(EVENT_TABLEAVE, "TabLeave"),
    KEYVALUE_داخلا(EVENT_TABNEW, "TabNew"),
    KEYVALUE_داخلا(EVENT_TERMCHANGED, "TermChanged"),
    KEYVALUE_داخلا(EVENT_TERMINALOPEN, "TerminalOpen"),
    KEYVALUE_داخلا(EVENT_TERMINALWINOPEN, "TerminalWinOpen"),
    KEYVALUE_داخلا(EVENT_TERMRESPONSE, "TermResponse"),
    KEYVALUE_داخلا(EVENT_TERMRESPONSEALL, "TermResponseAll"),
    KEYVALUE_داخلا(-EVENT_TEXTCHANGED, "TextChanged"),
    KEYVALUE_داخلا(-EVENT_TEXTCHANGEDI, "TextChangedI"),
    KEYVALUE_داخلا(-EVENT_TEXTCHANGEDP, "TextChangedP"),
    KEYVALUE_داخلا(-EVENT_TEXTCHANGEDT, "TextChangedT"),
    KEYVALUE_داخلا(-EVENT_TEXTPUTPOST, "TextPutPost"),
    KEYVALUE_داخلا(-EVENT_TEXTPUTPRE, "TextPutPre"),
    KEYVALUE_داخلا(-EVENT_TEXTYANKPOST, "TextYankPost"),
    KEYVALUE_داخلا(EVENT_USER, "User"),
    KEYVALUE_داخلا(EVENT_VIMENTER, "VimEnter"),
    KEYVALUE_داخلا(EVENT_VIMLEAVE, "VimLeave"),
    KEYVALUE_داخلا(EVENT_VIMLEAVEPRE, "VimLeavePre"),
    KEYVALUE_داخلا(EVENT_VIMRESIZED, "VimResized"),
    KEYVALUE_داخلا(EVENT_VIMRESUME, "VimResume"),
    KEYVALUE_داخلا(EVENT_VIMSUSPEND, "VimSuspend"),
    KEYVALUE_داخلا(-EVENT_WINCLOSED, "WinClosed"),
    KEYVALUE_داخلا(-EVENT_WINENTER, "WinEnter"),
    KEYVALUE_داخلا(-EVENT_WINLEAVE, "WinLeave"),
    KEYVALUE_داخلا(EVENT_WINNEW, "WinNew"),
    KEYVALUE_داخلا(EVENT_WINNEWPRE, "WinNewPre"),
    KEYVALUE_داخلا(-EVENT_WINRESIZED, "WinResized"),
    KEYVALUE_داخلا(-EVENT_WINSCROLLED, "WinScrolled"),
};

ساكن AutoPat *پهريون_autopat[انگ_EVENTS] = { عدم };
ساكن AutoPat *آخري_autopat[انگ_EVENTS] = { عدم };

#وصف_ڪريو AUGROUP_DEFAULT    (-1)	    // default autocmd group
#وصف_ڪريو AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#وصف_ڪريو AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
بنية AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    صحيح		گروهه;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*پڇ;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    صحيح		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *ايندڙ;		// chain of active apc-s for auto-invalidation
};

ساكن AutoPatCmd_T *سرگرم_apc_list = عدم; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#وصف_ڪريو FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    لكل ((ap) = پهريون_autopat[(صحيح)(event)]; (ap) != عدم; (ap) = (ap)->ايندڙ)

/*
 * augroups stores a list of autocmd group names.
 */
ساكن garray_T augroups = {0, 0, حجم(char_u *), 10, عدم};
#وصف_ڪريو AUGROUP_NAME(i) (((char_u **)augroups.ga_معلومات)[i])
// use get_deleted_augroup() to get this
ساكن char_u *deleted_augroup = عدم;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
ساكن صحيح current_augroup = AUGROUP_DEFAULT;

ساكن صحيح au_need_clean = FALSE;   // need to delete marked patterns

ساكن event_T event_name2nr(char_u *شروعات, char_u **پڇاڙي);
ساكن string_T *event_nr2name(event_T event);
ساكن صحيح au_get_grouparg(char_u **argp);
ساكن صحيح do_autocmd_event(event_T event, char_u *pat, صحيح once, صحيح nested, char_u *cmd, صحيح forceit, صحيح گروهه, صحيح flags);
ساكن صحيح apply_autocmds_گروهه(event_T event, char_u *fname, char_u *fname_io, صحيح force, صحيح گروهه, buf_T *buf, exarg_T *eap);
ساكن فراغ auto_ايندڙ_pat(AutoPatCmd_T *apc, صحيح stop_at_آخري);
ساكن صحيح au_ڳوليو_گروهه(char_u *name);

ساكن event_T	آخري_event;
ساكن صحيح	آخري_گروهه;
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

    نياپو_putchar('\n');
    إذا (got_int)
	اذهب theend;
    إذا (event != آخري_event || ap->گروهه != آخري_گروهه)
    {
	string_T    *event_name;

	إذا (ap->گروهه != AUGROUP_DEFAULT)
	{
	    إذا (AUGROUP_NAME(ap->گروهه) == عدم)
		نياپو_puts_attr((حرف *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    وإلا
		نياپو_puts_attr((حرف *)AUGROUP_NAME(ap->گروهه), HL_ATTR(HLF_T));
	    نياپو_puts("  ");
	}
	event_name = event_nr2name(event);
	نياپو_puts_attr((حرف *)event_name->string, HL_ATTR(HLF_T));
	آخري_event = event;
	آخري_گروهه = ap->گروهه;
	نياپو_putchar('\n');
	إذا (got_int)
	    اذهب theend;
    }

    إذا (ap->pat == عدم)
	اذهب theend;  // timer might have cleared the pattern or group

    نياپو_کانو = 4;
    نياپو_outtrans(ap->pat);

    لكل (ac = ap->cmds; ac != عدم; ac = ac->ايندڙ)
    {
	إذا (ac->cmd == عدم)		// skip removed commands
	    استمر;

	إذا (نياپو_کانو >= 14)
	    نياپو_putchar('\n');
	نياپو_کانو = 14;
	إذا (got_int)
	    اذهب theend;
	نياپو_outtrans(ac->cmd);
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (p_verbose > 0)
	    آخري_سيٽ_نياپو(ac->script_ctx);
#جيڪڏهن_پورو
	إذا (got_int)
	    اذهب theend;
	إذا (ac->ايندڙ != عدم)
	{
	    نياپو_putchar('\n');
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

    لكل (ac = ap->cmds; ac != عدم; ac = ac->ايندڙ)
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
    لكل (event = (event_T)0; (صحيح)event < انگ_EVENTS;
					    event = (event_T)((صحيح)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(پهريون_autopat[(صحيح)event]);
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
		    *prev_ac = ac->ايندڙ;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		وإلا
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->ايندڙ);
		}
	    }

	    إذا (ap->pat != عدم && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    إذا (ap->pat == عدم)
	    {
		إذا (ap->ايندڙ == عدم)
		{
		    إذا (prev_ap == &(پهريون_autopat[(صحيح)event]))
			آخري_autopat[(صحيح)event] = عدم;
		    وإلا
			// this depends on the "next" field being the first in
			// the struct
			آخري_autopat[(صحيح)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->ايندڙ;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    وإلا
		prev_ap = &(ap->ايندڙ);
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
    لكل (apc = سرگرم_apc_list; apc; apc = apc->ايندڙ)
	إذا (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    لكل (event = (event_T)0; (صحيح)event < انگ_EVENTS;
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
au_new_گروهه(char_u *name)
{
    صحيح		i;

    i = au_ڳوليو_گروهه(name);
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
au_del_گروهه(char_u *name)
{
    صحيح		i;
    event_T	event;
    AutoPat	*ap;
    صحيح		اندريون_use = FALSE;


    i = au_ڳوليو_گروهه(name);
    إذا (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_گروهه_str), name);
	أرجع;
    }
    إذا (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_گروهه));
	أرجع;
    }

    لكل (event = (event_T)0; (صحيح)event < انگ_EVENTS;
	    event = (event_T)((صحيح)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    إذا (ap->گروهه == i && ap->pat != عدم)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		اندريون_use = TRUE;
		event = انگ_EVENTS;
		اكسر;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    إذا (اندريون_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    وإلا
	AUGROUP_NAME(i) = عدم;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    ساكن صحيح
au_ڳوليو_گروهه(char_u *name)
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
au_has_گروهه(char_u *name)
{
    أرجع au_ڳوليو_گروهه(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    فراغ
do_augroup(char_u *arg, صحيح del_گروهه)
{
    صحيح	    i;

    إذا (del_گروهه)
    {
	إذا (*arg == NUL)
	    emsg(_(e_دليل_required));
	وإلا
	    au_del_گروهه(arg);
    }
    وإلا إذا (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    وإلا إذا (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_گروهه(arg);
	إذا (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    وإلا			    // ":aug": list the group names
    {
	نياپو_شروعات();
	لكل (i = 0; i < augroups.ga_len; ++i)
	{
	    إذا (AUGROUP_NAME(i) != عدم)
	    {
		نياپو_puts((حرف *)AUGROUP_NAME(i));
		نياپو_puts("  ");
	    }
	}
	نياپو_clr_eos();
	نياپو_پڇاڙي();
    }
}

    فراغ
autocmd_init(فراغ)
{
    CLEAR_ميدان(aucmd_win);
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
	s = ((char_u **)(augroups.ga_معلومات))[i];
	إذا (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#جيڪڏهن_پورو

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    صحيح
is_aucmd_win(win_T *win)
{
    لكل (صحيح i = 0; i < AUCMD_WIN_ڳڻپ; ++i)
	إذا (aucmd_win[i].auc_win_استعمال_ٿيل && aucmd_win[i].auc_win == win)
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    ساكن event_T
event_name2nr(char_u *شروعات, char_u **پڇاڙي)
{
    char_u	*p;
    keyvalue_T هدف;
    keyvalue_T *داخلا;
    ساكن keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_اشاريو];
    ساكن keyvalue_T *bufread = &event_tab[BUFREAD_اشاريو];

    // the event name ends with end of line, '|', a blank or a comma
    لكل (p = شروعات; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    هدف.ڪنجي = 0;
    هدف.قدر.string = شروعات;
    هدف.قدر.ڊيگهائي = (size_t)(p - شروعات);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    إذا (cmp_keyvalue_قدر_ni(&هدف, bufnewfile) == 0)
	داخلا = bufnewfile;
    وإلا إذا (cmp_keyvalue_قدر_ni(&هدف, bufread) == 0)
	داخلا = bufread;
    وإلا
	داخلا = (keyvalue_T *)bsearch(&هدف, &event_tab, انگ_EVENTS,
				حجم(event_tab[0]), cmp_keyvalue_قدر_ni);

    إذا (*p == ',')
	++p;
    *پڇاڙي = p;

    أرجع (داخلا == عدم) ? انگ_EVENTS : (event_T)مطلق_قدر(داخلا->ڪنجي);
}

/*
 * Return the name for event "event".
 */
    ساكن string_T *
event_nr2name(event_T event)
{
    صحيح	    i;
    تعداد {CACHE_سائيز = 12};
    ساكن صحيح cache_tab[CACHE_سائيز];
    ساكن صحيح cache_آخري_اشاريو = -1;
    ساكن string_T unknown = STR_LITERAL_INIT("Unknown");

    إذا (cache_آخري_اشاريو < 0)
    {
	لكل (i = 0; i < CACHE_سائيز; ++i)
	    cache_tab[i] = -1;
	cache_آخري_اشاريو = CACHE_سائيز - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    لكل (i = cache_آخري_اشاريو; cache_tab[i] >= 0; )
    {
	إذا ((event_T)مطلق_قدر(event_tab[cache_tab[i]].ڪنجي) == event)
	    أرجع &event_tab[cache_tab[i]].قدر;

	إذا (i == 0)
	    i = CACHE_سائيز - 1;
	وإلا
	    --i;

	// are we back at the start?
	إذا (i == cache_آخري_اشاريو)
	    اكسر;
    }

    // look in the event table itself
    لكل (i = 0; i < انگ_EVENTS; ++i)
    {
	إذا ((event_T)مطلق_قدر(event_tab[i].ڪنجي) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    إذا (cache_آخري_اشاريو == CACHE_سائيز - 1)
		cache_آخري_اشاريو = 0;
	    وإلا
		++cache_آخري_اشاريو;
	    cache_tab[cache_آخري_اشاريو] = i;
	    أرجع &event_tab[i].قدر;
	}
    }

    أرجع &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    ساكن char_u *
ڳوليو_پڇاڙي_event(
    char_u  *arg,
    صحيح	    have_گروهه)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    إذا (*arg == '*')
    {
	إذا (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_تارو_str), arg);
	    أرجع عدم;
	}
	pat = arg + 1;
    }
    وإلا
    {
	لكل (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    إذا ((صحيح)event_name2nr(pat, &p) >= انگ_EVENTS)
	    {
		إذا (have_گروهه)
		    semsg(_(e_no_such_event_str), pat);
		وإلا
		    semsg(_(e_no_such_گروهه_or_event_str), pat);
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
au_ڳوليو_cmd_arg(char_u *arg)
{
    char_u	*pat;
    char_u	*cmd;
    صحيح		گروهه;

    إذا (*arg == '|')
	أرجع عدم;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    گروهه = au_get_grouparg(&arg);
    pat = گروهه == AUGROUP_ERROR
			  ? عدم : ڳوليو_پڇاڙي_event(arg, گروهه != AUGROUP_ALL);
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
	إذا (!اندريون_vim9script() && STRNCMP(cmd, "nested", 6) == 0
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
	    ignored = ei == p_ei || (event_tab[event].ڪنجي <= 0);
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
	    إذا (event == انگ_EVENTS || (win && event_tab[event].ڪنجي > 0))
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
    سيٽ_string_option_سڌو((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    أرجع save_ei;
}

    فراغ
au_event_restore(char_u *old_ei)
{
    إذا (old_ei != عدم)
    {
	سيٽ_string_option_سڌو((char_u *)"ei", -1, old_ei,
							  OPT_FREE, SID_NONE);
	vim_free(old_ei);
    }
}
#جيڪڏهن_پورو  // FEAT_SYN_HL

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
do_autocmd(exarg_T *eap, char_u *arg_اندريون, صحيح forceit)
{
    char_u	*arg = arg_اندريون;
    char_u	*pat;
    char_u	*envpat = عدم;
    char_u	*cmd;
    صحيح		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = عدم;
    صحيح		nested = FALSE;
    صحيح		once = FALSE;
    صحيح		گروهه;
    صحيح		i;
    صحيح		flags = 0;

    إذا (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	گروهه = AUGROUP_ALL;	// no argument, use all groups
    }
    وإلا
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	گروهه = au_get_grouparg(&arg);
	إذا (arg == عدم)	    // out of memory
	    أرجع;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = ڳوليو_پڇاڙي_event(arg, گروهه != AUGROUP_ALL);
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
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
	    صحيح	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#جيڪڏهن_پورو
	    envpat = expand_env_save(pat);
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
	    p_ssl = p_ssl_save;
#جيڪڏهن_پورو
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
		    semsg(_(e_duplicate_دليل_str), "++once");
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
		    semsg(_(e_duplicate_دليل_str), "++nested");
		    اذهب err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    إذا (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		إذا (اندريون_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_سراسري_plusplus_nested));
		    اذهب err_exit;
		}
		إذا (nested)
		{
		    semsg(_(e_duplicate_دليل_str), "nested");
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
		cmd = may_get_cmd_بلاڪ(eap, cmd, &tofree, &flags);

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
	نياپو_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    آخري_event = (event_T)-1;		// for listing the event name
    آخري_گروهه = AUGROUP_ERROR;		// for listing the group name
    إذا (*arg == '*' || *arg == NUL || *arg == '|')
    {
	إذا (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	وإلا
	    لكل (event = (event_T)0; (صحيح)event < انگ_EVENTS;
					     event = (event_T)((صحيح)event + 1))
		إذا (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, گروهه, flags) == FAIL)
		    اكسر;
    }
    وإلا
    {
	طالما (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    إذا (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, گروهه, flags) == FAIL)
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
    char_u	*گروهه_name;
    char_u	*p;
    char_u	*arg = *argp;
    صحيح		گروهه = AUGROUP_ALL;

    لكل (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    إذا (p <= arg)
	أرجع AUGROUP_ALL;

    گروهه_name = vim_strnsave(arg, p - arg);
    إذا (گروهه_name == عدم)		// out of memory
	أرجع AUGROUP_ERROR;
    گروهه = au_ڳوليو_گروهه(گروهه_name);
    إذا (گروهه == AUGROUP_ERROR)
	گروهه = AUGROUP_ALL;	// no match, use all groups
    وإلا
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(گروهه_name);
    أرجع گروهه;
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
    صحيح		گروهه,
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

    إذا (گروهه == AUGROUP_ALL)
	findgroup = current_augroup;
    وإلا
	findgroup = گروهه;
    allgroups = (گروهه == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    إذا (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    إذا (forceit)  // delete the AutoPat, if it's in the current group
	    {
		إذا (ap->گروهه == findgroup)
		    au_remove_pat(ap);
	    }
	    وإلا إذا (گروهه == AUGROUP_ALL || ap->گروهه == گروهه)
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
	إذا (!forceit && *cmd != NUL && آخري_autopat[(صحيح)event] != عدم)
	    prev_ap = &آخري_autopat[(صحيح)event];
	وإلا
	    prev_ap = &پهريون_autopat[(صحيح)event];
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
		إذا ((allgroups || ap->گروهه == findgroup)
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
			إذا (*cmd != NUL && ap->ايندڙ == عدم)
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
		    وإلا إذا (ap->ايندڙ == عدم)
			اكسر;
		}
	    }
	    prev_ap = &ap->ايندڙ;
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
		    semsg(_(e_بفر_nr_invalid_بفر_عدد), buflocal_nr);
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

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		إذا (event == EVENT_MODECHANGED && !has_modechanged())
		    get_گهڻائي(آخري_گهڻائي);
#جيڪڏهن_پورو
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
			snapshot_windows_scroll_سائيز();
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
		    reg_pat = file_pat_منزل_reg_pat(pat, endpat,
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
		آخري_autopat[(صحيح)event] = ap;
		ap->ايندڙ = عدم;
		إذا (گروهه == AUGROUP_ALL)
		    ap->گروهه = current_augroup;
		وإلا
		    ap->گروهه = گروهه;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    طالما ((ac = *prev_ac) != عدم)
		prev_ac = &ac->ايندڙ;
	    ac = ALLOC_ONE(AutoCmd);
	    إذا (ac == عدم)
		أرجع FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    إذا (flags & UC_VIM9)
		ac->script_ctx.sc_نسخو = SCRIPT_نسخو_VIM9;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#جيڪڏهن_پورو
	    إذا (ac->cmd == عدم)
	    {
		vim_free(ac);
		أرجع FAIL;
	    }
	    ac->ايندڙ = عدم;
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
    char_u	*arg_شروعات,
    صحيح		do_نياپو,	    // give message for no matching autocmds?
    صحيح		*did_something)
{
    char_u	*arg = arg_شروعات;
    char_u	*fname;
    صحيح		nothing_پورو = TRUE;
    صحيح		گروهه;

    إذا (did_something != عدم)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    گروهه = au_get_grouparg(&arg);
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
    fname = ڳوليو_پڇاڙي_event(arg, گروهه != AUGROUP_ALL);
    إذا (fname == عدم)
	أرجع FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    طالما (*arg && !پڇاڙيون_excmd(*arg) && !VIM_ISWHITE(*arg))
	إذا (apply_autocmds_گروهه(event_name2nr(arg, &arg),
				      fname, عدم, TRUE, گروهه, curbuf, عدم))
	    nothing_پورو = FALSE;

    إذا (nothing_پورو && do_نياپو
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		&& !aborting()
#جيڪڏهن_پورو
	       )
	smsg(_("No matching autocommands: %s"), arg_شروعات);
    إذا (did_something != عدم)
	*did_something = !nothing_پورو;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    أرجع aborting() ? FAIL : OK;
#وإلا
    أرجع OK;
#جيڪڏهن_پورو
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

	سيٽ_bufref(&bufref, buf);

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
#جيڪڏهن_وصف_ٿيل FEAT_AUTOCHDIR
    صحيح		save_acd;
#جيڪڏهن_پورو
    صحيح		same_بفر = buf == curbuf;

    // Find a window that is for the new buffer
    إذا (same_بفر)		// be quick when buf is curbuf
	win = curwin;
    وإلا
	FOR_ALL_WINDOWS(win)
	    إذا (win->w_بفر == buf)
		اكسر;

    // Allocate a window when needed.
    win_T *auc_win = عدم;
    صحيح auc_idx = AUCMD_WIN_ڳڻپ;
    إذا (win == عدم)
    {
	لكل (auc_idx = 0; auc_idx < AUCMD_WIN_ڳڻپ; ++auc_idx)
	    إذا (!aucmd_win[auc_idx].auc_win_استعمال_ٿيل)
	    {
		إذا (aucmd_win[auc_idx].auc_win == عدم)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		إذا (auc_win != عدم)
		    aucmd_win[auc_idx].auc_win_استعمال_ٿيل = TRUE;
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
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    إذا (bt_prompt(curbuf))
	aco->save_prompt_وجهو = curbuf->b_prompt_وجهو;
#جيڪڏهن_پورو

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
	بلاڪ_autocmds();
	make_snapshot(SNAP_AUCMD_IDX);
	save_ea = p_ea;
	p_ea = FALSE;

#جيڪڏهن_وصف_ٿيل FEAT_AUTOCHDIR
	// Prevent chdir() call in win_enter_ext(), through do_autochdir().
	save_acd = p_acd;
	p_acd = FALSE;
#جيڪڏهن_پورو

	(فراغ)win_ورهايو_ins(0, WSP_چوٽي | WSP_FORCE_گنجائش, auc_win, 0, عدم);
	win_comp_pos();   // recompute window positions
	p_ea = save_ea;
#جيڪڏهن_وصف_ٿيل FEAT_AUTOCHDIR
	p_acd = save_acd;
#جيڪڏهن_پورو
	unblock_autocmds();
	curwin = auc_win;
    }
    curbuf = buf;
    aco->new_curwin_id = curwin->w_id;
    سيٽ_bufref(&aco->new_curbuf, curbuf);

    aco->save_VIsual_سرگرم = VIsual_سرگرم;
    إذا (!same_بفر)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_سرگرم = FALSE;
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
	بلاڪ_autocmds();
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
		    اذهب win_مليو;
		}
	    }
	}
win_مليو:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(فراغ)winframe_remove(curwin, &dummy, عدم, عدم);
	win_remove(curwin, عدم);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_استعمال_ٿيل = FALSE;
	آخري_حالت(FALSE);	    // may need to remove last status line

	إذا (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_ڳوليو_by_id(aco->save_curwin_id);
	إذا (save_curwin != عدم)
	    curwin = save_curwin;
	وإلا
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_بفر;
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_دري(curwin);
	إذا (bt_prompt(curbuf))
	    curbuf->b_prompt_وجهو = aco->save_prompt_وجهو;
#جيڪڏهن_پورو
	prevwin = win_ڳوليو_by_id(aco->save_prevwin_id);
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#جيڪڏهن_پورو
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	إذا (awp->w_localdir != عدم)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_سرگرم = aco->save_VIsual_سرگرم;
	check_cursor();
	إذا (curwin->w_topline > curbuf->b_ml.ml_سٽ_ڳڻپ)
	{
	    curwin->w_topline = curbuf->b_ml.ml_سٽ_ڳڻپ;
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
	    curwin->w_topfill = 0;
#جيڪڏهن_پورو
	}
#إذا defined(FEAT_GUI)
	إذا (gui.اندريون_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_کاٻو], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_ساڄو], FALSE);
	    gui_may_update_scrollbars();
	}
#جيڪڏهن_پورو
    }
    وإلا
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_ڳوليو_by_id(aco->save_curwin_id);
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
#جيڪڏهن_پورو
		--curbuf->b_nwindows;
		curbuf = aco->new_curbuf.br_buf;
		curwin->w_بفر = curbuf;
		++curbuf->b_nwindows;
	    }

	    curwin = save_curwin;
	    curbuf = curwin->w_بفر;
	    prevwin = win_ڳوليو_by_id(aco->save_prevwin_id);

	    // In case the autocommand moves the cursor to a position that
	    // does not exist in curbuf.
	    VIsual_سرگرم = aco->save_VIsual_سرگرم;
	    check_cursor();
	}
    }

    VIsual_سرگرم = aco->save_VIsual_سرگرم;
    check_cursor();	    // just in case lines got deleted
    إذا (VIsual_سرگرم)
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
    أرجع apply_autocmds_گروهه(event, fname, fname_io, force,
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
    أرجع apply_autocmds_گروهه(event, fname, fname_io, force,
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

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (should_abort(*retval))
	أرجع FALSE;
#جيڪڏهن_پورو

    did_cmd = apply_autocmds_گروهه(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, عدم);
    إذا (did_cmd
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    && aborting()
#جيڪڏهن_پورو
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
    أرجع (پهريون_autopat[(صحيح)(get_real_state() == گهڻائي_عام_BUSY
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
	    && !ins_compl_سرگرم())
    {
	state = get_real_state();
	إذا (state == گهڻائي_عام_BUSY || (state & گهڻائي_وجهو) != 0)
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
    أرجع (پهريون_autopat[(صحيح)EVENT_WINRESIZED] != عدم);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    صحيح
has_winscrolled(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_WINSCROLLED] != عدم);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    صحيح
has_cursormoved(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_CURSORMOVED] != عدم);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    صحيح
has_cursormovedI(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_CURSORMOVEDI] != عدم);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    صحيح
has_textchanged(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_TEXTCHANGED] != عدم);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    صحيح
has_textchangedI(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_TEXTCHANGEDI] != عدم);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    صحيح
has_textchangedP(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_TEXTCHANGEDP] != عدم);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    صحيح
has_insertcharpre(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_INSERTCHARPRE] != عدم);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    صحيح
has_keyinputpre(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_KEYINPUTPRE] != عدم);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    صحيح
has_cmdundefined(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_CMDUNDEFINED] != عدم);
}

#إذا defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    صحيح
has_textputpost(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_TEXTPUTPOST] != عدم);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    صحيح
has_textputpre(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_TEXTPUTPRE] != عدم);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    صحيح
has_textyankpost(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_TEXTYANKPOST] != عدم);
}
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    صحيح
has_completechanged(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_COMPLETECHANGED] != عدم);
}
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    صحيح
has_modechanged(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_MODECHANGED] != عدم);
}
#جيڪڏهن_پورو

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    ساكن صحيح
apply_autocmds_گروهه(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    صحيح		force,	     // when TRUE, ignore autocmd_busy
    صحيح		گروهه,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap اڻ_استعمال_ٿيل) // command arguments
{
    char_u	*sfname = عدم;	// short file name
    char_u	*پڇ;
    صحيح		save_بدليو;
    buf_T	*old_curbuf;
    صحيح		retval = FALSE;
    char_u	*save_autocmd_fname;
    صحيح		save_autocmd_fname_ڀريل;
    صحيح		save_autocmd_bufnr;
    char_u	*save_autocmd_ميلاپ;
    صحيح		save_autocmd_busy;
    صحيح		save_autocmd_nested;
    ساكن صحيح	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    funccal_داخلا_T funccal_داخلا;
    char_u	*save_cmdarg;
    ڊگهو	save_cmdbang;
#جيڪڏهن_پورو
    ساكن صحيح	filechangeshell_busy = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    proftime_T	wait_وقت;
#جيڪڏهن_پورو
    صحيح		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    صحيح		save_ڪنجي_Typed = ڪنجي_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    إذا (event == انگ_EVENTS || پهريون_autopat[(صحيح)event] == عدم
	    || autocmd_blocked > 0)
	اذهب BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    إذا (autocmd_busy && !(force || autocmd_nested))
	اذهب BYPASS_AU;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    إذا (aborting())
	اذهب BYPASS_AU;
#جيڪڏهن_پورو

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
    إذا (buf == curbuf && event_tab[event].ڪنجي <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    وإلا إذا (buf != عدم && event_tab[event].ڪنجي <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    إذا (wp->w_بفر == buf && !event_ignored(event, wp->w_p_eiw))
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
	++aucmd_cmdline_بدليو_ڳڻپ;

    /*
     * Save the autocmd_* variables and info about the current buffer.
     */
    save_autocmd_fname = autocmd_fname;
    save_autocmd_fname_ڀريل = autocmd_fname_ڀريل;
    save_autocmd_bufnr = autocmd_bufnr;
    save_autocmd_ميلاپ = autocmd_ميلاپ;
    save_autocmd_busy = autocmd_busy;
    save_autocmd_nested = autocmd_nested;
    save_بدليو = curbuf->b_بدليو;
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
	وإلا إذا (fname != عدم && !پڇاڙيون_excmd(*fname))
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
    autocmd_fname_ڀريل = FALSE; // call FullName_save() later

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
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
	    إذا (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    وإلا
#جيڪڏهن_پورو
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
	    autocmd_fname_ڀريل = TRUE; // don't expand it later
	}
	وإلا
	    fname = ڀريل_Name_save(fname, FALSE);
    }
    إذا (fname == عدم)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	اذهب BYPASS_AU;
    }

#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    إذا (sfname != عدم)
	forward_slash(sfname);
    forward_slash(fname);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل VMS
    // remove version for correct match
    إذا (sfname != عدم)
	vms_remove_نسخو(sfname);
    vms_remove_نسخو(fname);
#جيڪڏهن_پورو

    /*
     * Set the name to be used for <amatch>.
     */
    autocmd_ميلاپ = fname;


    // Don't redraw while doing autocommands.
    ++RedrawingDisabled;

    // name and lnum are filled in later
    estack_ڌڪو(ETYPE_AUCMD, عدم, 0);
    ESTACK_CHECK_تياري;

    save_current_sctx = current_sctx;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
# ifdef FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	prof_ٻار_enter(&wait_وقت); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_داخلا);
#جيڪڏهن_پورو

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    إذا (!autocmd_busy)
    {
	save_ڳولا_patterns();
	إذا (!ins_compl_سرگرم())
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

    پڇ = gettail(fname);

    // Find first autocommand that matches
    CLEAR_ميدان(patcmd);
    patcmd.curpat = پهريون_autopat[(صحيح)event];
    patcmd.گروهه = گروهه;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.پڇ = پڇ;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_ايندڙ_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    إذا (patcmd.curpat != عدم)
    {
	// add to active_apc_list
	patcmd.ايندڙ = سرگرم_apc_list;
	سرگرم_apc_list = &patcmd;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (ڊگهو)get_vim_var_nr(VV_CMDBANG);
	إذا (eap != عدم)
	{
	    save_cmdarg = سيٽ_cmdarg(eap, عدم);
	    سيٽ_vim_var_nr(VV_CMDBANG, (ڊگهو)eap->forceit);
	}
	وإلا
	    save_cmdarg = عدم;	// avoid gcc warning
#جيڪڏهن_پورو
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	لكل (ap = patcmd.curpat; ap->ايندڙ != عدم; ap = ap->ايندڙ)
	    ap->آخري = FALSE;
	ap->آخري = TRUE;

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
	سيٽ_pressedreturn(save_ex_pressedreturn);

	إذا (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	إذا (eap != عدم)
	{
	    (فراغ)سيٽ_cmdarg(عدم, save_cmdarg);
	    سيٽ_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#جيڪڏهن_پورو
	// delete from active_apc_list
	إذا (سرگرم_apc_list == &patcmd)	    // just in case
	    سرگرم_apc_list = patcmd.ايندڙ;
    }

    إذا (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_ڪڍو();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_ڀريل = save_autocmd_fname_ڀريل;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_ميلاپ = save_autocmd_ميلاپ;
    current_sctx = save_current_sctx;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	prof_ٻار_exit(&wait_وقت);
# endif
#جيڪڏهن_پورو
    ڪنجي_Typed = save_ڪنجي_Typed;
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
	restore_ڳولا_patterns();
	إذا (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	طالما (au_pending_free_buf != عدم)
	{
	    buf_T *b = au_pending_free_buf->b_ايندڙ;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	طالما (au_pending_free_win != عدم)
	{
	    win_T *w = au_pending_free_win->w_ايندڙ;

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
	إذا (curbuf->b_بدليو != save_بدليو)
	    need_maketitle = TRUE;
	curbuf->b_بدليو = save_بدليو;
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

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
ساكن char_u	*old_termresponse = عدم;
ساكن char_u	*old_termu7resp = عدم;
ساكن char_u	*old_termblinkresp = عدم;
ساكن char_u	*old_termrbgresp = عدم;
ساكن char_u	*old_termrfgresp = عدم;
ساكن char_u	*old_termstyleresp = عدم;
#جيڪڏهن_پورو

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    فراغ
بلاڪ_autocmds(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
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
#جيڪڏهن_پورو
    ++autocmd_blocked;
}

    فراغ
unblock_autocmds(فراغ)
{
    --autocmd_blocked;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
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
#جيڪڏهن_پورو
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
auto_ايندڙ_pat(
    AutoPatCmd_T *apc,
    صحيح		stop_at_آخري)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*داخلا;

    داخلا = ((estack_T *)exestack.ga_معلومات) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(داخلا->es_name);
    داخلا->es_info.aucmd = عدم;

    لكل (ap = apc->curpat; ap != عدم && !got_int; ap = ap->ايندڙ)
    {
	apc->curpat = عدم;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	إذا (ap->pat != عدم && ap->cmds != عدم
		&& (apc->گروهه == AUGROUP_ALL || apc->گروهه == ap->گروهه))
	{
	    // execution-condition
	    إذا (ap->buflocal_nr == 0
		    ? (ميلاپ_file_pat(عدم, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->پڇ, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		حرف	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->ڊيگهائي + ap->patlen + 1);
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
		داخلا->es_name = namep;
		داخلا->es_info.aucmd = apc;

		apc->curpat = ap;
		apc->nextcmd = ap->cmds;
		// mark last command
		لكل (cp = ap->cmds; cp->ايندڙ != عدم; cp = cp->ايندڙ)
		    cp->آخري = FALSE;
		cp->آخري = TRUE;
	    }
	    سٽ_breakcheck();
	    إذا (apc->curpat != عدم)	    // found a match
		اكسر;
	}
	إذا (stop_at_آخري && ap->آخري)
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
#جيڪڏهن_پورو

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	صحيح c اڻ_استعمال_ٿيل,
	فراغ *cookie,
	صحيح indent اڻ_استعمال_ٿيل,
	getline_opt_T options اڻ_استعمال_ٿيل)
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
	    إذا (acp->nextcmd->آخري)
		acp->nextcmd = عدم;
	    وإلا
		acp->nextcmd = acp->nextcmd->ايندڙ;

	إذا (acp->nextcmd != عدم)
	    اكسر;

	// at end of commands, find next pattern that matches
	إذا (acp->curpat->آخري)
	    acp->curpat = عدم;
	وإلا
	    acp->curpat = acp->curpat->ايندڙ;
	إذا (acp->curpat != عدم)
	    auto_ايندڙ_pat(acp, TRUE);
	إذا (acp->curpat == عدم)
	    أرجع عدم;
    }

    ac = acp->nextcmd;

    إذا (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	نياپو_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    إذا (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    إذا (ac->آخري)
	acp->nextcmd = عدم;
    وإلا
	acp->nextcmd = ac->ايندڙ;
    أرجع retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    ساكن صحيح
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_ڀريل)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*پڇ = gettail(sfname);
    صحيح		retval = FALSE;

    إذا (پهريون_autopat[(صحيح)event] == عدم)
	أرجع FALSE;

    fname = is_ڀريل ? vim_strsave(sfname) : ڀريل_Name_save(sfname, FALSE);
    إذا (fname == عدم)
	أرجع FALSE;

#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    إذا (sfname != عدم)
	forward_slash(sfname);
    forward_slash(fname);
#جيڪڏهن_پورو

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	إذا (ap->pat != عدم && ap->cmds != عدم
	      && (ap->buflocal_nr == 0
		? ميلاپ_file_pat(عدم, &ap->reg_prog,
					  fname, sfname, پڇ, ap->allow_dirs)
		: buf != عدم && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    اكسر;
	}

    vim_free(fname);
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
    vim_free(sfname);
#جيڪڏهن_پورو

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
get_augroup_name(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
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
سيٽ_context_اندريون_autocmd(
    expand_T	*xp,
    char_u	*arg,
    صحيح		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    صحيح		گروهه;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    گروهه = au_get_grouparg(&arg);
    إذا (گروهه == AUGROUP_ERROR)
	أرجع عدم;
    // If there only is a group name that's what we expand.
    إذا (*arg == NUL && گروهه != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	گروهه = AUGROUP_ALL;
    }

    // skip over event name
    لكل (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	إذا (*p == ',')
	    arg = p + 1;
    إذا (*p == NUL)
    {
	إذا (گروهه == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_نمونو = arg;
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
	    إذا (!اندريون_vim9script() && STRNCMP(arg, "nested", 6) == 0
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
get_event_name(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
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
    إذا (i < 0 || i >= انگ_EVENTS)
	أرجع عدم;

    أرجع event_tab[i].قدر.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_گروهه(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx, صحيح win)
{
    إذا (idx < 0 || idx >= انگ_EVENTS)
	أرجع عدم;

    إذا (!win)
	أرجع event_tab[idx].قدر.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    صحيح j = 0;
    لكل (صحيح i = 0; i < انگ_EVENTS; ++i)
    {
	j += event_tab[i].ڪنجي <= 0;
	إذا (j == idx + 1)
	    أرجع event_tab[i].قدر.string;
    }

    أرجع عدم;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    صحيح
has_tabclosedpre(فراغ)
{
    أرجع (پهريون_autopat[(صحيح)EVENT_TABCLOSEDPRE] != عدم);
}

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    صحيح
autocmd_supported(char_u *name)
{
    char_u *p;

    أرجع (event_name2nr(name, &p) != انگ_EVENTS);
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
    char_u	*نمونو = عدم;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = عدم;
    صحيح		گروهه;
    صحيح		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    إذا (arg_save == عدم)
	أرجع FALSE;
    p = vim_strchr(arg_save, '#');
    إذا (p != عدم)
	*p++ = NUL;

    // First, look for an autocmd group name
    گروهه = au_ڳوليو_گروهه(arg_save);
    إذا (گروهه == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	گروهه = AUGROUP_ALL;
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

    نمونو = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    إذا (event == انگ_EVENTS)
	اذهب theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = پهريون_autopat[(صحيح)event];
    إذا (ap == عدم)
	اذهب theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    إذا (نمونو != عدم && STRICMP(نمونو, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    لكل ( ; ap != عدم; ap = ap->ايندڙ)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	إذا (ap->pat != عدم && ap->cmds != عدم
	    && (گروهه == AUGROUP_ALL || ap->گروهه == گروهه)
	    && (نمونو == عدم
		|| (buflocal_buf == عدم
		    ? fnamecmp(ap->pat, نمونو) == 0
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
autocmd_جوڙيو_or_delete(typval_T *argvars, typval_T *rettv, صحيح delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = عدم;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*گروهه_name = عدم;
    صحيح		گروهه;
    char_u	*pat = عدم;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = عدم;
    char_u	*پڇاڙي;
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

    FOR_ALL_LIST_شيون(aucmd_list, li)
    {
	VIM_CLEAR(گروهه_name);
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

	di = dict_ڳوليو(event_dict, (char_u *)"event", -1);
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

	گروهه_name = dict_get_string(event_dict, "group", TRUE);
	إذا (گروهه_name == عدم || *گروهه_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    گروهه = current_augroup;
	وإلا
	{
	    گروهه = au_ڳوليو_گروهه(گروهه_name);
	    إذا (گروهه == AUGROUP_ERROR)
	    {
		إذا (delete)
		{
		    semsg(_(e_no_such_گروهه_str), گروهه_name);
		    retval = VVAL_FALSE;
		    اكسر;
		}
		// group is not found, create it now
		گروهه = au_new_گروهه(گروهه_name);
		إذا (گروهه == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_گروهه_str), گروهه_name);
		    retval = VVAL_FALSE;
		    اكسر;
		}

		current_augroup = گروهه;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	إذا (dict_has_ڪنجي(event_dict, "bufnr"))
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
	    di = dict_ڳوليو(event_dict, (char_u *)"pattern", -1);
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
	    لكل (event = (event_T)0; (صحيح)event < انگ_EVENTS;
		    event = (event_T)((صحيح)event + 1))
	    {
		إذا (do_autocmd_event(event, pat, once, nested, cmd, delete,
							گروهه, 0) == FAIL)
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
	    پڇاڙي = عدم;
	    طالما (TRUE)
	    {
		إذا (event_list != عدم)
		{
		    إذا (eli == عدم)
			eli = event_list->lv_پهريون;
		    وإلا
			eli = eli->li_ايندڙ;
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

		event = event_name2nr(p, &پڇاڙي);
		إذا (event == انگ_EVENTS || *پڇاڙي != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    اكسر;
		}
		إذا (pat != عدم)
		{
		    إذا (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, گروهه, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			اكسر;
		    }
		}
		وإلا إذا (pat_list != عدم)
		{
		    FOR_ALL_LIST_شيون(pat_list, pli)
		    {
			إذا (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == عدم)
			{
			    emsg(_(e_string_required));
			    استمر;
			}
			إذا (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, گروهه, 0) ==
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
		    p = پڇاڙي;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	إذا (delete && گروهه_name != عدم &&
		(event_name == عدم || event_name[0] == NUL)
		&& (pat == عدم || pat[0] == NUL)
		&& (cmd == عدم || cmd[0] == NUL))
	    au_del_گروهه(گروهه_name);
    }

    VIM_CLEAR(گروهه_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_عدد = retval;
}

/*
 * autocmd_add() function
 */
    فراغ
f_autocmd_جوڙيو(typval_T *argvars, typval_T *rettv)
{
    autocmd_جوڙيو_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    فراغ
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_جوڙيو_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    فراغ
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = انگ_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = عدم;
    char_u	*name = عدم;
    صحيح		گروهه = AUGROUP_ALL;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;
    إذا (check_for_opt_dict_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	إذا (dict_has_ڪنجي(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    إذا (name == عدم)
		أرجع;

	    إذا (*name == NUL)
		گروهه = AUGROUP_DEFAULT;
	    وإلا
	    {
		گروهه = au_ڳوليو_گروهه(name);
		إذا (گروهه == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_گروهه_str), name);
		    vim_free(name);
		    أرجع;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	إذا (dict_has_ڪنجي(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    إذا (name == عدم)
		أرجع;

	    إذا (name[0] == '*' && name[1] == NUL)
		event_arg = انگ_EVENTS;
	    وإلا
	    {
		keyvalue_T هدف;
		keyvalue_T *داخلا;

		هدف.ڪنجي = 0;
		هدف.قدر.string = name;
		هدف.قدر.ڊيگهائي = STRLEN(هدف.قدر.string);
		داخلا = (keyvalue_T *)bsearch(&هدف, &event_tab,
		    انگ_EVENTS, حجم(event_tab[0]), cmp_keyvalue_قدر_ni);
		إذا (داخلا == عدم)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    أرجع;
		}
		event_arg = (event_T)مطلق_قدر(داخلا->ڪنجي);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	إذا (dict_has_ڪنجي(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    إذا (pat == عدم)
		أرجع;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    لكل (event = (event_T)0; (صحيح)event < انگ_EVENTS;
	    event = (event_T)((صحيح)event + 1))
    {
	string_T    *event_name;

	إذا (event_arg != انگ_EVENTS && event != event_arg)
	    استمر;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	گروهه_name;

	    إذا (ap->pat == عدم)		// pattern has been removed
		استمر;

	    إذا (گروهه != AUGROUP_ALL && گروهه != ap->گروهه)
		استمر;

	    إذا (pat != عدم && STRCMP(pat, ap->pat) != 0)
		استمر;

	    گروهه_name.string = get_augroup_name(عدم, ap->گروهه);
	    إذا (گروهه_name.string == عدم)
		STR_LITERAL_سيٽ(گروهه_name, "");
	    وإلا
		گروهه_name.ڊيگهائي = STRLEN(گروهه_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    لكل (ac = ap->cmds; ac != عدم; ac = ac->ايندڙ)
	    {
		event_dict = dict_alloc();
		إذا (event_dict == عدم
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    أرجع;
		}

		إذا (dict_جوڙيو_string_len(event_dict, "event",
			event_name->string, (صحيح)event_name->ڊيگهائي) == FAIL
			|| dict_جوڙيو_string_len(event_dict, "group",
			    گروهه_name.string, (صحيح)گروهه_name.ڊيگهائي) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_جوڙيو_عدد(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_جوڙيو_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_جوڙيو_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_جوڙيو_bool(event_dict, "once", ac->once) == FAIL
			|| dict_جوڙيو_bool(event_dict, "nested",
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

#جيڪڏهن_پورو
