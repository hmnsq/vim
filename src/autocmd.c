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

#అన్తర్భావ "vim.h"

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
రకనిర్వచనం నిర్మాణం AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    అక్షరం	    once;		// "One shot": removed after execution
    అక్షరం	    nested;		// If autocommands nest here.
    అక్షరం	    చివరి;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    నిర్మాణం AutoCmd  *తదుపరి;		// next AutoCmd in list
} AutoCmd;

రకనిర్వచనం నిర్మాణం AutoPat
{
    నిర్మాణం AutoPat  *తదుపరి;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    పూర్ణాంకం		    సమూహం;		// group ID
    పూర్ణాంకం		    patlen;		// strlen() of pat
    పూర్ణాంకం		    buflocal_nr;	// !=0 for buffer-local AutoPat
    అక్షరం	    allow_dirs;		// Pattern may match whole path
    అక్షరం	    చివరి;		// last pattern for apply_autocmds()
} AutoPat;

//
// special cases:
// BufNewFile and BufRead are searched for ALOT (especially at startup)
// so we pre-determine their index into the event_tab[] table for fast access.
// Keep these values in sync with event_tab[]!
#నిర్వచించు BUFNEWFILE_సూచీ 9
#నిర్వచించు BUFREAD_సూచీ 10

// Must be sorted by the 'value' field because it is used by bsearch()!
// Events with positive keys aren't allowed in 'eventignorewin'.
స్థిర keyvalue_T event_tab[అంకె_EVENTS] = {
    KEYVALUE_నమోదు(-EVENT_BUFADD, "BufAdd"),
    KEYVALUE_నమోదు(-EVENT_BUFADD, "BufCreate"),
    KEYVALUE_నమోదు(-EVENT_BUFDELETE, "BufDelete"),
    KEYVALUE_నమోదు(-EVENT_BUFENTER, "BufEnter"),
    KEYVALUE_నమోదు(-EVENT_BUFFILEPOST, "BufFilePost"),
    KEYVALUE_నమోదు(-EVENT_BUFFILEPRE, "BufFilePre"),
    KEYVALUE_నమోదు(-EVENT_BUFHIDDEN, "BufHidden"),
    KEYVALUE_నమోదు(-EVENT_BUFLEAVE, "BufLeave"),
    KEYVALUE_నమోదు(-EVENT_BUFNEW, "BufNew"),
    KEYVALUE_నమోదు(-EVENT_BUFNEWFILE, "BufNewFile"),	// BUFNEWFILE_INDEX
    KEYVALUE_నమోదు(-EVENT_BUFREADPOST, "BufRead"),	// BUFREAD_INDEX
    KEYVALUE_నమోదు(-EVENT_BUFREADCMD, "BufReadCmd"),
    KEYVALUE_నమోదు(-EVENT_BUFREADPOST, "BufReadPost"),
    KEYVALUE_నమోదు(-EVENT_BUFREADPRE, "BufReadPre"),
    KEYVALUE_నమోదు(-EVENT_BUFUNLOAD, "BufUnload"),
    KEYVALUE_నమోదు(-EVENT_BUFWINENTER, "BufWinEnter"),
    KEYVALUE_నమోదు(-EVENT_BUFWINLEAVE, "BufWinLeave"),
    KEYVALUE_నమోదు(-EVENT_BUFWIPEOUT, "BufWipeout"),
    KEYVALUE_నమోదు(-EVENT_BUFWRITEPRE, "BufWrite"),
    KEYVALUE_నమోదు(-EVENT_BUFWRITECMD, "BufWriteCmd"),
    KEYVALUE_నమోదు(-EVENT_BUFWRITEPOST, "BufWritePost"),
    KEYVALUE_నమోదు(-EVENT_BUFWRITEPRE, "BufWritePre"),
    KEYVALUE_నమోదు(EVENT_CMDLINECHANGED, "CmdlineChanged"),
    KEYVALUE_నమోదు(EVENT_CMDLINEENTER, "CmdlineEnter"),
    KEYVALUE_నమోదు(EVENT_CMDLINELEAVE, "CmdlineLeave"),
    KEYVALUE_నమోదు(EVENT_CMDLINELEAVEPRE, "CmdlineLeavePre"),
    KEYVALUE_నమోదు(EVENT_CMDUNDEFINED, "CmdUndefined"),
    KEYVALUE_నమోదు(EVENT_CMDWINENTER, "CmdwinEnter"),
    KEYVALUE_నమోదు(EVENT_CMDWINLEAVE, "CmdwinLeave"),
    KEYVALUE_నమోదు(EVENT_COLORSCHEME, "ColorScheme"),
    KEYVALUE_నమోదు(EVENT_COLORSCHEMEPRE, "ColorSchemePre"),
    KEYVALUE_నమోదు(EVENT_COMPLETECHANGED, "CompleteChanged"),
    KEYVALUE_నమోదు(EVENT_COMPLETEDONE, "CompleteDone"),
    KEYVALUE_నమోదు(EVENT_COMPLETEDONEPRE, "CompleteDonePre"),
    KEYVALUE_నమోదు(-EVENT_CURSORHOLD, "CursorHold"),
    KEYVALUE_నమోదు(-EVENT_CURSORHOLDI, "CursorHoldI"),
    KEYVALUE_నమోదు(-EVENT_CURSORMOVED, "CursorMoved"),
    KEYVALUE_నమోదు(-EVENT_CURSORMOVEDC, "CursorMovedC"),
    KEYVALUE_నమోదు(-EVENT_CURSORMOVEDI, "CursorMovedI"),
    KEYVALUE_నమోదు(EVENT_DIFFUPDATED, "DiffUpdated"),
    KEYVALUE_నమోదు(EVENT_DIRCHANGED, "DirChanged"),
    KEYVALUE_నమోదు(EVENT_DIRCHANGEDPRE, "DirChangedPre"),
    KEYVALUE_నమోదు(EVENT_ENCODINGCHANGED, "EncodingChanged"),
    KEYVALUE_నమోదు(EVENT_EXITPRE, "ExitPre"),
    KEYVALUE_నమోదు(-EVENT_FILEAPPENDCMD, "FileAppendCmd"),
    KEYVALUE_నమోదు(-EVENT_FILEAPPENDPOST, "FileAppendPost"),
    KEYVALUE_నమోదు(-EVENT_FILEAPPENDPRE, "FileAppendPre"),
    KEYVALUE_నమోదు(-EVENT_FILECHANGEDRO, "FileChangedRO"),
    KEYVALUE_నమోదు(-EVENT_FILECHANGEDSHELL, "FileChangedShell"),
    KEYVALUE_నమోదు(-EVENT_FILECHANGEDSHELLPOST, "FileChangedShellPost"),
    KEYVALUE_నమోదు(EVENT_ENCODINGCHANGED, "FileEncoding"),
    KEYVALUE_నమోదు(-EVENT_FILEREADCMD, "FileReadCmd"),
    KEYVALUE_నమోదు(-EVENT_FILEREADPOST, "FileReadPost"),
    KEYVALUE_నమోదు(-EVENT_FILEREADPRE, "FileReadPre"),
    KEYVALUE_నమోదు(-EVENT_FILETYPE, "FileType"),
    KEYVALUE_నమోదు(-EVENT_FILEWRITECMD, "FileWriteCmd"),
    KEYVALUE_నమోదు(-EVENT_FILEWRITEPOST, "FileWritePost"),
    KEYVALUE_నమోదు(-EVENT_FILEWRITEPRE, "FileWritePre"),
    KEYVALUE_నమోదు(-EVENT_FILTERREADPOST, "FilterReadPost"),
    KEYVALUE_నమోదు(-EVENT_FILTERREADPRE, "FilterReadPre"),
    KEYVALUE_నమోదు(-EVENT_FILTERWRITEPOST, "FilterWritePost"),
    KEYVALUE_నమోదు(-EVENT_FILTERWRITEPRE, "FilterWritePre"),
    KEYVALUE_నమోదు(EVENT_FOCUSGAINED, "FocusGained"),
    KEYVALUE_నమోదు(EVENT_FOCUSLOST, "FocusLost"),
    KEYVALUE_నమోదు(EVENT_FUNCUNDEFINED, "FuncUndefined"),
    KEYVALUE_నమోదు(EVENT_GUIENTER, "GUIEnter"),
    KEYVALUE_నమోదు(EVENT_GUIFAILED, "GUIFailed"),
    KEYVALUE_నమోదు(-EVENT_INSERTCHANGE, "InsertChange"),
    KEYVALUE_నమోదు(-EVENT_INSERTCHARPRE, "InsertCharPre"),
    KEYVALUE_నమోదు(-EVENT_INSERTENTER, "InsertEnter"),
    KEYVALUE_నమోదు(-EVENT_INSERTLEAVE, "InsertLeave"),
    KEYVALUE_నమోదు(-EVENT_INSERTLEAVEPRE, "InsertLeavePre"),
    KEYVALUE_నమోదు(EVENT_KEYINPUTPRE, "KeyInputPre"),
    KEYVALUE_నమోదు(EVENT_MENUPOPUP, "MenuPopup"),
    KEYVALUE_నమోదు(EVENT_MODECHANGED, "ModeChanged"),
    KEYVALUE_నమోదు(EVENT_OPTIONSET, "OptionSet"),
    KEYVALUE_నమోదు(EVENT_QUICKFIXCMDPOST, "QuickFixCmdPost"),
    KEYVALUE_నమోదు(EVENT_QUICKFIXCMDPRE, "QuickFixCmdPre"),
    KEYVALUE_నమోదు(EVENT_QUITPRE, "QuitPre"),
    KEYVALUE_నమోదు(EVENT_REMOTEREPLY, "RemoteReply"),
    KEYVALUE_నమోదు(EVENT_SAFESTATE, "SafeState"),
    KEYVALUE_నమోదు(EVENT_SAFESTATEAGAIN, "SafeStateAgain"),
    KEYVALUE_నమోదు(EVENT_SESSIONLOADPOST, "SessionLoadPost"),
    KEYVALUE_నమోదు(EVENT_SESSIONLOADPRE, "SessionLoadPre"),
    KEYVALUE_నమోదు(EVENT_SESSIONWRITEPOST, "SessionWritePost"),
    KEYVALUE_నమోదు(EVENT_SHELLCMDPOST, "ShellCmdPost"),
    KEYVALUE_నమోదు(-EVENT_SHELLFILTERPOST, "ShellFilterPost"),
    KEYVALUE_నమోదు(EVENT_SIGUSR1, "SigUSR1"),
    KEYVALUE_నమోదు(EVENT_SOURCECMD, "SourceCmd"),
    KEYVALUE_నమోదు(EVENT_SOURCEPOST, "SourcePost"),
    KEYVALUE_నమోదు(EVENT_SOURCEPRE, "SourcePre"),
    KEYVALUE_నమోదు(EVENT_SPELLFILEMISSING, "SpellFileMissing"),
    KEYVALUE_నమోదు(EVENT_STDINREADPOST, "StdinReadPost"),
    KEYVALUE_నమోదు(EVENT_STDINREADPRE, "StdinReadPre"),
    KEYVALUE_నమోదు(EVENT_SWAPEXISTS, "SwapExists"),
    KEYVALUE_నమోదు(EVENT_SYNTAX, "Syntax"),
    KEYVALUE_నమోదు(EVENT_TABCLOSED, "TabClosed"),
    KEYVALUE_నమోదు(EVENT_TABCLOSEDPRE, "TabClosedPre"),
    KEYVALUE_నమోదు(EVENT_TABENTER, "TabEnter"),
    KEYVALUE_నమోదు(EVENT_TABLEAVE, "TabLeave"),
    KEYVALUE_నమోదు(EVENT_TABNEW, "TabNew"),
    KEYVALUE_నమోదు(EVENT_TERMCHANGED, "TermChanged"),
    KEYVALUE_నమోదు(EVENT_TERMINALOPEN, "TerminalOpen"),
    KEYVALUE_నమోదు(EVENT_TERMINALWINOPEN, "TerminalWinOpen"),
    KEYVALUE_నమోదు(EVENT_TERMRESPONSE, "TermResponse"),
    KEYVALUE_నమోదు(EVENT_TERMRESPONSEALL, "TermResponseAll"),
    KEYVALUE_నమోదు(-EVENT_TEXTCHANGED, "TextChanged"),
    KEYVALUE_నమోదు(-EVENT_TEXTCHANGEDI, "TextChangedI"),
    KEYVALUE_నమోదు(-EVENT_TEXTCHANGEDP, "TextChangedP"),
    KEYVALUE_నమోదు(-EVENT_TEXTCHANGEDT, "TextChangedT"),
    KEYVALUE_నమోదు(-EVENT_TEXTPUTPOST, "TextPutPost"),
    KEYVALUE_నమోదు(-EVENT_TEXTPUTPRE, "TextPutPre"),
    KEYVALUE_నమోదు(-EVENT_TEXTYANKPOST, "TextYankPost"),
    KEYVALUE_నమోదు(EVENT_USER, "User"),
    KEYVALUE_నమోదు(EVENT_VIMENTER, "VimEnter"),
    KEYVALUE_నమోదు(EVENT_VIMLEAVE, "VimLeave"),
    KEYVALUE_నమోదు(EVENT_VIMLEAVEPRE, "VimLeavePre"),
    KEYVALUE_నమోదు(EVENT_VIMRESIZED, "VimResized"),
    KEYVALUE_నమోదు(EVENT_VIMRESUME, "VimResume"),
    KEYVALUE_నమోదు(EVENT_VIMSUSPEND, "VimSuspend"),
    KEYVALUE_నమోదు(-EVENT_WINCLOSED, "WinClosed"),
    KEYVALUE_నమోదు(-EVENT_WINENTER, "WinEnter"),
    KEYVALUE_నమోదు(-EVENT_WINLEAVE, "WinLeave"),
    KEYVALUE_నమోదు(EVENT_WINNEW, "WinNew"),
    KEYVALUE_నమోదు(EVENT_WINNEWPRE, "WinNewPre"),
    KEYVALUE_నమోదు(-EVENT_WINRESIZED, "WinResized"),
    KEYVALUE_నమోదు(-EVENT_WINSCROLLED, "WinScrolled"),
};

స్థిర AutoPat *మొదటి_autopat[అంకె_EVENTS] = { ఖాళీ };
స్థిర AutoPat *చివరి_autopat[అంకె_EVENTS] = { ఖాళీ };

#నిర్వచించు AUGROUP_DEFAULT    (-1)	    // default autocmd group
#నిర్వచించు AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#నిర్వచించు AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
నిర్మాణం AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    పూర్ణాంకం		సమూహం;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*తోక;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    పూర్ణాంకం		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *తదుపరి;		// chain of active apc-s for auto-invalidation
};

స్థిర AutoPatCmd_T *చురుకైన_apc_list = ఖాళీ; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#నిర్వచించు FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    కొరకు ((ap) = మొదటి_autopat[(పూర్ణాంకం)(event)]; (ap) != ఖాళీ; (ap) = (ap)->తదుపరి)

/*
 * augroups stores a list of autocmd group names.
 */
స్థిర garray_T augroups = {0, 0, పరిమాణం(char_u *), 10, ఖాళీ};
#నిర్వచించు AUGROUP_NAME(i) (((char_u **)augroups.ga_దత్తాంశం)[i])
// use get_deleted_augroup() to get this
స్థిర char_u *deleted_augroup = ఖాళీ;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
స్థిర పూర్ణాంకం current_augroup = AUGROUP_DEFAULT;

స్థిర పూర్ణాంకం au_need_clean = FALSE;   // need to delete marked patterns

స్థిర event_T event_name2nr(char_u *ప్రారంభం, char_u **చివర);
స్థిర string_T *event_nr2name(event_T event);
స్థిర పూర్ణాంకం au_get_grouparg(char_u **argp);
స్థిర పూర్ణాంకం do_autocmd_event(event_T event, char_u *pat, పూర్ణాంకం once, పూర్ణాంకం nested, char_u *cmd, పూర్ణాంకం forceit, పూర్ణాంకం సమూహం, పూర్ణాంకం flags);
స్థిర పూర్ణాంకం apply_autocmds_సమూహం(event_T event, char_u *fname, char_u *fname_io, పూర్ణాంకం force, పూర్ణాంకం సమూహం, buf_T *buf, exarg_T *eap);
స్థిర శూన్య auto_తదుపరి_pat(AutoPatCmd_T *apc, పూర్ణాంకం stop_at_చివరి);
స్థిర పూర్ణాంకం au_కనుగొను_సమూహం(char_u *name);

స్థిర event_T	చివరి_event;
స్థిర పూర్ణాంకం	చివరి_సమూహం;
స్థిర పూర్ణాంకం	autocmd_blocked = 0;	// block all autocmds

    స్థిర char_u *
get_deleted_augroup(శూన్య)
{
    ఒకవేళ (deleted_augroup == ఖాళీ)
	deleted_augroup = (char_u *)_("--Deleted--");
    తిరిగి deleted_augroup;
}

/*
 * Show the autocommands for one AutoPat.
 */
    స్థిర శూన్య
show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    // Check for "got_int" (here and at various places below), which is set
    // when "q" has been hit for the "--more--" prompt
    ఒకవేళ (got_int)
	తిరిగి;
    ఒకవేళ (ap->pat == ఖాళీ)		// pattern has been removed
	తిరిగి;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    సందేశం_putchar('\n');
    ఒకవేళ (got_int)
	వెళ్ళు theend;
    ఒకవేళ (event != చివరి_event || ap->సమూహం != చివరి_సమూహం)
    {
	string_T    *event_name;

	ఒకవేళ (ap->సమూహం != AUGROUP_DEFAULT)
	{
	    ఒకవేళ (AUGROUP_NAME(ap->సమూహం) == ఖాళీ)
		సందేశం_puts_attr((అక్షరం *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    లేకపోతే
		సందేశం_puts_attr((అక్షరం *)AUGROUP_NAME(ap->సమూహం), HL_ATTR(HLF_T));
	    సందేశం_puts("  ");
	}
	event_name = event_nr2name(event);
	సందేశం_puts_attr((అక్షరం *)event_name->string, HL_ATTR(HLF_T));
	చివరి_event = event;
	చివరి_సమూహం = ap->సమూహం;
	సందేశం_putchar('\n');
	ఒకవేళ (got_int)
	    వెళ్ళు theend;
    }

    ఒకవేళ (ap->pat == ఖాళీ)
	వెళ్ళు theend;  // timer might have cleared the pattern or group

    సందేశం_నిలువు = 4;
    సందేశం_outtrans(ap->pat);

    కొరకు (ac = ap->cmds; ac != ఖాళీ; ac = ac->తదుపరి)
    {
	ఒకవేళ (ac->cmd == ఖాళీ)		// skip removed commands
	    కొనసాగు;

	ఒకవేళ (సందేశం_నిలువు >= 14)
	    సందేశం_putchar('\n');
	సందేశం_నిలువు = 14;
	ఒకవేళ (got_int)
	    వెళ్ళు theend;
	సందేశం_outtrans(ac->cmd);
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (p_verbose > 0)
	    చివరి_సమితి_సందేశం(ac->script_ctx);
#అయితే_ముగింపు
	ఒకవేళ (got_int)
	    వెళ్ళు theend;
	ఒకవేళ (ac->తదుపరి != ఖాళీ)
	{
	    సందేశం_putchar('\n');
	    ఒకవేళ (got_int)
		వెళ్ళు theend;
	}
    }

theend:
    --autocmd_busy;
}

/*
 * Mark an autocommand pattern for deletion.
 */
    స్థిర శూన్య
au_remove_pat(AutoPat *ap)
{
    VIM_CLEAR(ap->pat);
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    స్థిర శూన్య
au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    కొరకు (ac = ap->cmds; ac != ఖాళీ; ac = ac->తదుపరి)
	VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

// Delete one command from an autocmd pattern.
స్థిర శూన్య au_del_cmd(AutoCmd *ac)
{
    VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    స్థిర శూన్య
au_cleanup(శూన్య)
{
    AutoPat	*ap, **prev_ap;
    AutoCmd	*ac, **prev_ac;
    event_T	event;

    ఒకవేళ (autocmd_busy || !au_need_clean)
	తిరిగి;

    // loop over all events
    కొరకు (event = (event_T)0; (పూర్ణాంకం)event < అంకె_EVENTS;
					    event = (event_T)((పూర్ణాంకం)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(మొదటి_autopat[(పూర్ణాంకం)event]);
	కొరకు (ap = *prev_ap; ap != ఖాళీ; ap = *prev_ap)
	{
	    పూర్ణాంకం has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    కొరకు (ac = *prev_ac; ac != ఖాళీ; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		ఒకవేళ (ap->pat == ఖాళీ || ac->cmd == ఖాళీ)
		{
		    *prev_ac = ac->తదుపరి;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		లేకపోతే
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->తదుపరి);
		}
	    }

	    ఒకవేళ (ap->pat != ఖాళీ && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    ఒకవేళ (ap->pat == ఖాళీ)
	    {
		ఒకవేళ (ap->తదుపరి == ఖాళీ)
		{
		    ఒకవేళ (prev_ap == &(మొదటి_autopat[(పూర్ణాంకం)event]))
			చివరి_autopat[(పూర్ణాంకం)event] = ఖాళీ;
		    లేకపోతే
			// this depends on the "next" field being the first in
			// the struct
			చివరి_autopat[(పూర్ణాంకం)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->తదుపరి;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    లేకపోతే
		prev_ap = &(ap->తదుపరి);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Called when buffer is freed, to remove/invalidate related buffer-local
 * autocmds.
 */
    శూన్య
aubuflocal_remove(buf_T *buf)
{
    AutoPat	    *ap;
    event_T	    event;
    AutoPatCmd_T    *apc;

    // invalidate currently executing autocommands
    కొరకు (apc = చురుకైన_apc_list; apc; apc = apc->తదుపరి)
	ఒకవేళ (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    కొరకు (event = (event_T)0; (పూర్ణాంకం)event < అంకె_EVENTS;
					    event = (event_T)((పూర్ణాంకం)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ఒకవేళ (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		ఒకవేళ (p_verbose >= 6)
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
    స్థిర పూర్ణాంకం
au_new_సమూహం(char_u *name)
{
    పూర్ణాంకం		i;

    i = au_కనుగొను_సమూహం(name);
    ఒకవేళ (i != AUGROUP_ERROR)
	తిరిగి i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    కొరకు (i = 0; i < augroups.ga_len; ++i)
	ఒకవేళ (AUGROUP_NAME(i) == ఖాళీ)
	    ఆపు;
    ఒకవేళ (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	తిరిగి AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    ఒకవేళ (AUGROUP_NAME(i) == ఖాళీ)
	తిరిగి AUGROUP_ERROR;
    ఒకవేళ (i == augroups.ga_len)
	++augroups.ga_len;

    తిరిగి i;
}

    స్థిర శూన్య
au_del_సమూహం(char_u *name)
{
    పూర్ణాంకం		i;
    event_T	event;
    AutoPat	*ap;
    పూర్ణాంకం		లోపల_use = FALSE;


    i = au_కనుగొను_సమూహం(name);
    ఒకవేళ (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_సమూహం_str), name);
	తిరిగి;
    }
    ఒకవేళ (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_సమూహం));
	తిరిగి;
    }

    కొరకు (event = (event_T)0; (పూర్ణాంకం)event < అంకె_EVENTS;
	    event = (event_T)((పూర్ణాంకం)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ఒకవేళ (ap->సమూహం == i && ap->pat != ఖాళీ)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		లోపల_use = TRUE;
		event = అంకె_EVENTS;
		ఆపు;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    ఒకవేళ (లోపల_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    లేకపోతే
	AUGROUP_NAME(i) = ఖాళీ;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    స్థిర పూర్ణాంకం
au_కనుగొను_సమూహం(char_u *name)
{
    పూర్ణాంకం	    i;

    కొరకు (i = 0; i < augroups.ga_len; ++i)
	ఒకవేళ (AUGROUP_NAME(i) != ఖాళీ && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    తిరిగి i;
    తిరిగి AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    పూర్ణాంకం
au_has_సమూహం(char_u *name)
{
    తిరిగి au_కనుగొను_సమూహం(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    శూన్య
do_augroup(char_u *arg, పూర్ణాంకం del_సమూహం)
{
    పూర్ణాంకం	    i;

    ఒకవేళ (del_సమూహం)
    {
	ఒకవేళ (*arg == NUL)
	    emsg(_(e_వాదం_required));
	లేకపోతే
	    au_del_సమూహం(arg);
    }
    లేకపోతే ఒకవేళ (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    లేకపోతే ఒకవేళ (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_సమూహం(arg);
	ఒకవేళ (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    లేకపోతే			    // ":aug": list the group names
    {
	సందేశం_ప్రారంభం();
	కొరకు (i = 0; i < augroups.ga_len; ++i)
	{
	    ఒకవేళ (AUGROUP_NAME(i) != ఖాళీ)
	    {
		సందేశం_puts((అక్షరం *)AUGROUP_NAME(i));
		సందేశం_puts("  ");
	    }
	}
	సందేశం_clr_eos();
	సందేశం_చివర();
    }
}

    శూన్య
autocmd_init(శూన్య)
{
    CLEAR_క్షేత్రం(aucmd_win);
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_all_autocmds(శూన్య)
{
    char_u	*s;

    కొరకు (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(ఖాళీ, (char_u *)"", TRUE);

    కొరకు (పూర్ణాంకం i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_దత్తాంశం))[i];
	ఒకవేళ (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#అయితే_ముగింపు

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    పూర్ణాంకం
is_aucmd_win(win_T *win)
{
    కొరకు (పూర్ణాంకం i = 0; i < AUCMD_WIN_లెక్క; ++i)
	ఒకవేళ (aucmd_win[i].auc_win_వాడిన && aucmd_win[i].auc_win == win)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    స్థిర event_T
event_name2nr(char_u *ప్రారంభం, char_u **చివర)
{
    char_u	*p;
    keyvalue_T లక్ష్యం;
    keyvalue_T *నమోదు_2;
    స్థిర keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_సూచీ];
    స్థిర keyvalue_T *bufread = &event_tab[BUFREAD_సూచీ];

    // the event name ends with end of line, '|', a blank or a comma
    కొరకు (p = ప్రారంభం; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    లక్ష్యం.కీలకం = 0;
    లక్ష్యం.విలువ.string = ప్రారంభం;
    లక్ష్యం.విలువ.నిడివి = (size_t)(p - ప్రారంభం);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    ఒకవేళ (cmp_keyvalue_విలువ_ni(&లక్ష్యం, bufnewfile) == 0)
	నమోదు_2 = bufnewfile;
    లేకపోతే ఒకవేళ (cmp_keyvalue_విలువ_ni(&లక్ష్యం, bufread) == 0)
	నమోదు_2 = bufread;
    లేకపోతే
	నమోదు_2 = (keyvalue_T *)bsearch(&లక్ష్యం, &event_tab, అంకె_EVENTS,
				పరిమాణం(event_tab[0]), cmp_keyvalue_విలువ_ni);

    ఒకవేళ (*p == ',')
	++p;
    *చివర = p;

    తిరిగి (నమోదు_2 == ఖాళీ) ? అంకె_EVENTS : (event_T)నిరపేక్ష(నమోదు_2->కీలకం);
}

/*
 * Return the name for event "event".
 */
    స్థిర string_T *
event_nr2name(event_T event)
{
    పూర్ణాంకం	    i;
    గణన {CACHE_కొలత = 12};
    స్థిర పూర్ణాంకం cache_tab[CACHE_కొలత];
    స్థిర పూర్ణాంకం cache_చివరి_సూచీ = -1;
    స్థిర string_T unknown = STR_LITERAL_INIT("Unknown");

    ఒకవేళ (cache_చివరి_సూచీ < 0)
    {
	కొరకు (i = 0; i < CACHE_కొలత; ++i)
	    cache_tab[i] = -1;
	cache_చివరి_సూచీ = CACHE_కొలత - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    కొరకు (i = cache_చివరి_సూచీ; cache_tab[i] >= 0; )
    {
	ఒకవేళ ((event_T)నిరపేక్ష(event_tab[cache_tab[i]].కీలకం) == event)
	    తిరిగి &event_tab[cache_tab[i]].విలువ;

	ఒకవేళ (i == 0)
	    i = CACHE_కొలత - 1;
	లేకపోతే
	    --i;

	// are we back at the start?
	ఒకవేళ (i == cache_చివరి_సూచీ)
	    ఆపు;
    }

    // look in the event table itself
    కొరకు (i = 0; i < అంకె_EVENTS; ++i)
    {
	ఒకవేళ ((event_T)నిరపేక్ష(event_tab[i].కీలకం) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    ఒకవేళ (cache_చివరి_సూచీ == CACHE_కొలత - 1)
		cache_చివరి_సూచీ = 0;
	    లేకపోతే
		++cache_చివరి_సూచీ;
	    cache_tab[cache_చివరి_సూచీ] = i;
	    తిరిగి &event_tab[i].విలువ;
	}
    }

    తిరిగి &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    స్థిర char_u *
కనుగొను_చివర_event(
    char_u  *arg,
    పూర్ణాంకం	    have_సమూహం)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    ఒకవేళ (*arg == '*')
    {
	ఒకవేళ (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_నక్షత్రం_str), arg);
	    తిరిగి ఖాళీ;
	}
	pat = arg + 1;
    }
    లేకపోతే
    {
	కొరకు (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    ఒకవేళ ((పూర్ణాంకం)event_name2nr(pat, &p) >= అంకె_EVENTS)
	    {
		ఒకవేళ (have_సమూహం)
		    semsg(_(e_no_such_event_str), pat);
		లేకపోతే
		    semsg(_(e_no_such_సమూహం_or_event_str), pat);
		తిరిగి ఖాళీ;
	    }
	}
    }
    తిరిగి pat;
}

/*
 * Find the command of an ":autocmd" with argument "arg", the part after the
 * events, the pattern and any "++once"/"++nested".  Returns NULL when there is
 * no command.  Does not modify "arg" and gives no error messages.
 */
    char_u *
au_కనుగొను_cmd_arg(char_u *arg)
{
    char_u	*pat;
    char_u	*cmd;
    పూర్ణాంకం		సమూహం;

    ఒకవేళ (*arg == '|')
	తిరిగి ఖాళీ;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    సమూహం = au_get_grouparg(&arg);
    pat = సమూహం == AUGROUP_ERROR
			  ? ఖాళీ : కనుగొను_చివర_event(arg, సమూహం != AUGROUP_ALL);
    --emsg_off;
    ఒకవేళ (pat == ఖాళీ)
	తిరిగి ఖాళీ;

    pat = skipwhite(pat);
    ఒకవేళ (*pat == NUL || *pat == '|')
	తిరిగి ఖాళీ;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    అయితే (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    కొరకు (పూర్ణాంకం i = 0; i < 2; ++i)
    {
	ఒకవేళ (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	ఒకవేళ (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	ఒకవేళ (!లోపల_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    తిరిగి *cmd == NUL ? ఖాళీ : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    పూర్ణాంకం
event_ignored(event_T event, char_u *ei)
{
    పూర్ణాంకం ignored = FALSE;
    అయితే (*ei != NUL)
    {
	పూర్ణాంకం unignore = *ei == '-';
	ei += unignore;
	ఒకవేళ (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].కీలకం <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	లేకపోతే ఒకవేళ (event_name2nr(ei, &ei) == event)
	{
	    ఒకవేళ (unignore)
		తిరిగి FALSE;
	    లేకపోతే
		ignored = TRUE;
	}
    }

    తిరిగి ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    పూర్ణాంకం
check_ei(char_u *ei)
{
    పూర్ణాంకం	win = ei != p_ei;

    అయితే (*ei)
    {
	ఒకవేళ (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	లేకపోతే
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    ఒకవేళ (event == అంకె_EVENTS || (win && event_tab[event].కీలకం > 0))
		తిరిగి FAIL;
	}
    }

    తిరిగి OK;
}

#ఒకవేళ defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(అక్షరం *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    size_t	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    ఒకవేళ (save_ei == ఖాళీ)
	తిరిగి ఖాళీ;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    ఒకవేళ (new_ei == ఖాళీ)
    {
	vim_free(save_ei);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    లేకపోతే
	STRCPY(new_ei + p_ei_len, what);
    సమితి_string_option_ప్రత్యక్ష((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    తిరిగి save_ei;
}

    శూన్య
au_event_restore(char_u *old_ei)
{
    ఒకవేళ (old_ei != ఖాళీ)
    {
	సమితి_string_option_ప్రత్యక్ష((char_u *)"ei", -1, old_ei,
							  OPT_FREE, SID_NONE);
	vim_free(old_ei);
    }
}
#అయితే_ముగింపు  // FEAT_SYN_HL

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
    శూన్య
do_autocmd(exarg_T *eap, char_u *arg_లోపల, పూర్ణాంకం forceit)
{
    char_u	*arg = arg_లోపల;
    char_u	*pat;
    char_u	*envpat = ఖాళీ;
    char_u	*cmd;
    పూర్ణాంకం		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = ఖాళీ;
    పూర్ణాంకం		nested = FALSE;
    పూర్ణాంకం		once = FALSE;
    పూర్ణాంకం		సమూహం;
    పూర్ణాంకం		i;
    పూర్ణాంకం		flags = 0;

    ఒకవేళ (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	సమూహం = AUGROUP_ALL;	// no argument, use all groups
    }
    లేకపోతే
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	సమూహం = au_get_grouparg(&arg);
	ఒకవేళ (arg == ఖాళీ)	    // out of memory
	    తిరిగి;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = కనుగొను_చివర_event(arg, సమూహం != AUGROUP_ALL);
    ఒకవేళ (pat == ఖాళీ)
	తిరిగి;

    pat = skipwhite(pat);
    ఒకవేళ (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    లేకపోతే
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	అయితే (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	ఒకవేళ (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	ఒకవేళ (vim_strchr(pat, '$') != ఖాళీ || vim_strchr(pat, '~') != ఖాళీ)
	{
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	    పూర్ణాంకం	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#అయితే_ముగింపు
	    envpat = expand_env_save(pat);
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	    p_ssl = p_ssl_save;
#అయితే_ముగింపు
	    ఒకవేళ (envpat != ఖాళీ)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	కొరకు (i = 0; i < 2; i++)
	{
	    ఒకవేళ (*cmd == NUL)
		కొనసాగు;

	    // Check for "++once" flag.
	    ఒకవేళ (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ఒకవేళ (once)
		{
		    semsg(_(e_duplicate_వాదం_str), "++once");
		    వెళ్ళు err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    ఒకవేళ ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		ఒకవేళ (nested)
		{
		    semsg(_(e_duplicate_వాదం_str), "++nested");
		    వెళ్ళు err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    ఒకవేళ (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ఒకవేళ (లోపల_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_సగటు_plusplus_nested));
		    వెళ్ళు err_exit;
		}
		ఒకవేళ (nested)
		{
		    semsg(_(e_duplicate_వాదం_str), "nested");
		    వెళ్ళు err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 6);
	    }
	}

	/*
	 * Find the start of the commands.
	 * Expand <sfile> in it.
	 */
	ఒకవేళ (*cmd != NUL)
	{
	    ఒకవేళ (eap != ఖాళీ)
		// Read a {} block if it follows.
		cmd = may_get_cmd_ఖండం(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    ఒకవేళ (cmd == ఖాళీ)	    // some error
		వెళ్ళు err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    ఒకవేళ (!forceit && *cmd == NUL)
	// Highlight title
	సందేశం_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    చివరి_event = (event_T)-1;		// for listing the event name
    చివరి_సమూహం = AUGROUP_ERROR;		// for listing the group name
    ఒకవేళ (*arg == '*' || *arg == NUL || *arg == '|')
    {
	ఒకవేళ (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	లేకపోతే
	    కొరకు (event = (event_T)0; (పూర్ణాంకం)event < అంకె_EVENTS;
					     event = (event_T)((పూర్ణాంకం)event + 1))
		ఒకవేళ (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, సమూహం, flags) == FAIL)
		    ఆపు;
    }
    లేకపోతే
    {
	అయితే (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    ఒకవేళ (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, సమూహం, flags) == FAIL)
		ఆపు;
    }

err_exit:
    ఒకవేళ (cmd_need_free)
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
    స్థిర పూర్ణాంకం
au_get_grouparg(char_u **argp)
{
    char_u	*సమూహం_name;
    char_u	*p;
    char_u	*arg = *argp;
    పూర్ణాంకం		సమూహం = AUGROUP_ALL;

    కొరకు (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    ఒకవేళ (p <= arg)
	తిరిగి AUGROUP_ALL;

    సమూహం_name = vim_strnsave(arg, p - arg);
    ఒకవేళ (సమూహం_name == ఖాళీ)		// out of memory
	తిరిగి AUGROUP_ERROR;
    సమూహం = au_కనుగొను_సమూహం(సమూహం_name);
    ఒకవేళ (సమూహం == AUGROUP_ERROR)
	సమూహం = AUGROUP_ALL;	// no match, use all groups
    లేకపోతే
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(సమూహం_name);
    తిరిగి సమూహం;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    స్థిర పూర్ణాంకం
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    పూర్ణాంకం		once,
    పూర్ణాంకం		nested,
    char_u	*cmd,
    పూర్ణాంకం		forceit,
    పూర్ణాంకం		సమూహం,
    పూర్ణాంకం		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    పూర్ణాంకం		brace_level;
    char_u	*endpat;
    పూర్ణాంకం		findgroup;
    పూర్ణాంకం		allgroups;
    పూర్ణాంకం		patlen;
    పూర్ణాంకం		is_buflocal;
    పూర్ణాంకం		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    ఒకవేళ (సమూహం == AUGROUP_ALL)
	findgroup = current_augroup;
    లేకపోతే
	findgroup = సమూహం;
    allgroups = (సమూహం == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    ఒకవేళ (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    ఒకవేళ (forceit)  // delete the AutoPat, if it's in the current group
	    {
		ఒకవేళ (ap->సమూహం == findgroup)
		    au_remove_pat(ap);
	    }
	    లేకపోతే ఒకవేళ (సమూహం == AUGROUP_ALL || ap->సమూహం == సమూహం)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    కొరకు ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	కొరకు (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    ఒకవేళ (*endpat == '{')
		brace_level++;
	    లేకపోతే ఒకవేళ (*endpat == '}')
		brace_level--;
	}
	ఒకవేళ (pat == endpat)		// ignore single comma
	    కొనసాగు;
	patlen = (పూర్ణాంకం)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	ఒకవేళ (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    ఒకవేళ (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    లేకపోతే ఒకవేళ (patlen > 9 && pat[7] == '=')
	    {
		ఒకవేళ (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		లేకపోతే ఒకవేళ (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((అక్షరం *)pat + 8);
	    }
	}

	ఒకవేళ (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((అక్షరం *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (పూర్ణాంకం)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	ఒకవేళ (!forceit && *cmd != NUL && చివరి_autopat[(పూర్ణాంకం)event] != ఖాళీ)
	    prev_ap = &చివరి_autopat[(పూర్ణాంకం)event];
	లేకపోతే
	    prev_ap = &మొదటి_autopat[(పూర్ణాంకం)event];
	అయితే ((ap = *prev_ap) != ఖాళీ)
	{
	    ఒకవేళ (ap->pat != ఖాళీ)
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
		ఒకవేళ ((allgroups || ap->సమూహం == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    ఒకవేళ (forceit)
		    {
			ఒకవేళ (*cmd != NUL && ap->తదుపరి == ఖాళీ)
			{
			    au_remove_cmds(ap);
			    ఆపు;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    లేకపోతే ఒకవేళ (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    లేకపోతే ఒకవేళ (ap->తదుపరి == ఖాళీ)
			ఆపు;
		}
	    }
	    prev_ap = &ap->తదుపరి;
	}

	/*
	 * Add a new command.
	 */
	ఒకవేళ (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    ఒకవేళ (ap == ఖాళీ)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		ఒకవేళ (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == ఖాళీ))
		{
		    semsg(_(e_బఫరు_nr_invalid_బఫరు_సంఖ్య), buflocal_nr);
		    తిరిగి FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		ఒకవేళ (ap == ఖాళీ)
		    తిరిగి FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		ఒకవేళ (ap->pat == ఖాళీ)
		{
		    vim_free(ap);
		    తిరిగి FAIL;
		}

#నిర్వచితమైతే FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		ఒకవేళ (event == EVENT_MODECHANGED && !has_modechanged())
		    get_బహుళకం(చివరి_బహుళకం);
#అయితే_ముగింపు
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		ఒకవేళ ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
			&& !(has_winscrolled() || has_winresized()))
		{
		    tabpage_T *save_curtab = curtab;
		    tabpage_T *tp;
		    FOR_ALL_TABPAGES(tp)
		    {
			unuse_tabpage(curtab);
			use_tabpage(tp);
			snapshot_windows_scroll_కొలత();
		    }
		    unuse_tabpage(curtab);
		    use_tabpage(save_curtab);
		}

		ఒకవేళ (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = ఖాళీ;
		}
		లేకపోతే
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_గమ్యం_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    ఒకవేళ (reg_pat != ఖాళీ)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    ఒకవేళ (reg_pat == ఖాళీ || ap->reg_prog == ఖాళీ)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			తిరిగి FAIL;
		    }
		}
		ap->cmds = ఖాళీ;
		*prev_ap = ap;
		చివరి_autopat[(పూర్ణాంకం)event] = ap;
		ap->తదుపరి = ఖాళీ;
		ఒకవేళ (సమూహం == AUGROUP_ALL)
		    ap->సమూహం = current_augroup;
		లేకపోతే
		    ap->సమూహం = సమూహం;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    అయితే ((ac = *prev_ac) != ఖాళీ)
		prev_ac = &ac->తదుపరి;
	    ac = ALLOC_ONE(AutoCmd);
	    ఒకవేళ (ac == ఖాళీ)
		తిరిగి FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    ఒకవేళ (flags & UC_VIM9)
		ac->script_ctx.sc_సంస్కరణ = SCRIPT_సంస్కరణ_VIM9;
#నిర్వచితమైతే FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#అయితే_ముగింపు
	    ఒకవేళ (ac->cmd == ఖాళీ)
	    {
		vim_free(ac);
		తిరిగి FAIL;
	    }
	    ac->తదుపరి = ఖాళీ;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    తిరిగి OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    పూర్ణాంకం
do_doautocmd(
    char_u	*arg_ప్రారంభం,
    పూర్ణాంకం		do_సందేశం,	    // give message for no matching autocmds?
    పూర్ణాంకం		*did_something)
{
    char_u	*arg = arg_ప్రారంభం;
    char_u	*fname;
    పూర్ణాంకం		nothing_పూర్తి = TRUE;
    పూర్ణాంకం		సమూహం;

    ఒకవేళ (did_something != ఖాళీ)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    సమూహం = au_get_grouparg(&arg);
    ఒకవేళ (arg == ఖాళీ)	    // out of memory
	తిరిగి FAIL;

    ఒకవేళ (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	తిరిగి FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = కనుగొను_చివర_event(arg, సమూహం != AUGROUP_ALL);
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    అయితే (*arg && !చివర్లు_excmd(*arg) && !VIM_ISWHITE(*arg))
	ఒకవేళ (apply_autocmds_సమూహం(event_name2nr(arg, &arg),
				      fname, ఖాళీ, TRUE, సమూహం, curbuf, ఖాళీ))
	    nothing_పూర్తి = FALSE;

    ఒకవేళ (nothing_పూర్తి && do_సందేశం
#నిర్వచితమైతే FEAT_EVAL
		&& !aborting()
#అయితే_ముగింపు
	       )
	smsg(_("No matching autocommands: %s"), arg_ప్రారంభం);
    ఒకవేళ (did_something != ఖాళీ)
	*did_something = !nothing_పూర్తి;

#నిర్వచితమైతే FEAT_EVAL
    తిరిగి aborting() ? FAIL : OK;
#లేకపోతే
    తిరిగి OK;
#అయితే_ముగింపు
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    శూన్య
ex_doautoall(exarg_T *eap)
{
    పూర్ణాంకం		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    పూర్ణాంకం		call_do_modelines = check_nomodeline(&arg);
    పూర్ణాంకం		did_aucmd;

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
	ఒకవేళ (buf->b_ml.ml_mfp == ఖాళీ || buf == curbuf)
	    కొనసాగు;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	ఒకవేళ (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    ఆపు;
	}

	సమితి_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	ఒకవేళ (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	ఒకవేళ (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    ఆపు;
	}
    }

    // Execute autocommands for the current buffer last.
    ఒకవేళ (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	ఒకవేళ (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    పూర్ణాంకం
check_nomodeline(char_u **argp)
{
    ఒకవేళ (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * Prepare for executing autocommands for (hidden) buffer "buf".
 * Search for a visible window containing the current buffer.  If there isn't
 * one then use an entry in "aucmd_win[]".
 * Set "curbuf" and "curwin" to match "buf".
 * When this fails "curbuf" is not equal "buf".
 */
    శూన్య
aucmd_prepbuf(
    aco_save_T	*aco,		// structure to save values in
    buf_T	*buf)		// new curbuf
{
    win_T	*win;
    పూర్ణాంకం		save_ea;
#నిర్వచితమైతే FEAT_AUTOCHDIR
    పూర్ణాంకం		save_acd;
#అయితే_ముగింపు
    పూర్ణాంకం		same_బఫరు = buf == curbuf;

    // Find a window that is for the new buffer
    ఒకవేళ (same_బఫరు)		// be quick when buf is curbuf
	win = curwin;
    లేకపోతే
	FOR_ALL_WINDOWS(win)
	    ఒకవేళ (win->w_బఫరు == buf)
		ఆపు;

    // Allocate a window when needed.
    win_T *auc_win = ఖాళీ;
    పూర్ణాంకం auc_idx = AUCMD_WIN_లెక్క;
    ఒకవేళ (win == ఖాళీ)
    {
	కొరకు (auc_idx = 0; auc_idx < AUCMD_WIN_లెక్క; ++auc_idx)
	    ఒకవేళ (!aucmd_win[auc_idx].auc_win_వాడిన)
	    {
		ఒకవేళ (aucmd_win[auc_idx].auc_win == ఖాళీ)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		ఒకవేళ (auc_win != ఖాళీ)
		    aucmd_win[auc_idx].auc_win_వాడిన = TRUE;
		ఆపు;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	ఒకవేళ (auc_win == ఖాళీ)
	    తిరిగి;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == ఖాళీ ? 0 : prevwin->w_id;
#నిర్వచితమైతే FEAT_పని_CHANNEL
    ఒకవేళ (bt_prompt(curbuf))
	aco->save_prompt_చొప్పించు = curbuf->b_prompt_చొప్పించు;
#అయితే_ముగింపు

    ఒకవేళ (win != ఖాళీ)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    లేకపోతే
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
	curtab->tp_localdir = ఖాళీ;
	aco->globaldir = globaldir;
	globaldir = ఖాళీ;

	// Split the current window, put the auc_win in the upper half.
	// We don't want the BufEnter or WinEnter autocommands.
	ఖండం_autocmds();
	make_snapshot(SNAP_AUCMD_IDX);
	save_ea = p_ea;
	p_ea = FALSE;

#నిర్వచితమైతే FEAT_AUTOCHDIR
	// Prevent chdir() call in win_enter_ext(), through do_autochdir().
	save_acd = p_acd;
	p_acd = FALSE;
#అయితే_ముగింపు

	(శూన్య)win_విభజించు_ins(0, WSP_శిఖరం | WSP_FORCE_స్థలం, auc_win, 0, ఖాళీ);
	win_comp_pos();   // recompute window positions
	p_ea = save_ea;
#నిర్వచితమైతే FEAT_AUTOCHDIR
	p_acd = save_acd;
#అయితే_ముగింపు
	unblock_autocmds();
	curwin = auc_win;
    }
    curbuf = buf;
    aco->new_curwin_id = curwin->w_id;
    సమితి_bufref(&aco->new_curbuf, curbuf);

    aco->save_VIsual_చురుకైన = VIsual_చురుకైన;
    ఒకవేళ (!same_బఫరు)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_చురుకైన = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    శూన్య
aucmd_restbuf(
    aco_save_T	*aco)		// structure holding saved values
{
    పూర్ణాంకం	    dummy;
    win_T   *save_curwin;

    ఒకవేళ (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	ఖండం_autocmds();
	ఒకవేళ (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		ఒకవేళ (wp == awp)
		{
		    ఒకవేళ (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(awp);
		    వెళ్ళు win_దొరికింది;
		}
	    }
	}
win_దొరికింది:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(శూన్య)winframe_remove(curwin, &dummy, ఖాళీ, ఖాళీ);
	win_remove(curwin, ఖాళీ);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_వాడిన = FALSE;
	చివరి_స్థితి(FALSE);	    // may need to remove last status line

	ఒకవేళ (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_కనుగొను_by_id(aco->save_curwin_id);
	ఒకవేళ (save_curwin != ఖాళీ)
	    curwin = save_curwin;
	లేకపోతే
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_బఫరు;
#నిర్వచితమైతే FEAT_పని_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_కిటికీ(curwin);
	ఒకవేళ (bt_prompt(curbuf))
	    curbuf->b_prompt_చొప్పించు = aco->save_prompt_చొప్పించు;
#అయితే_ముగింపు
	prevwin = win_కనుగొను_by_id(aco->save_prevwin_id);
#నిర్వచితమైతే FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#అయితే_ముగింపు
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	ఒకవేళ (awp->w_localdir != ఖాళీ)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_చురుకైన = aco->save_VIsual_చురుకైన;
	check_cursor();
	ఒకవేళ (curwin->w_topline > curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    curwin->w_topline = curbuf->b_ml.ml_పంక్తి_లెక్క;
#నిర్వచితమైతే FEAT_DIFF
	    curwin->w_topfill = 0;
#అయితే_ముగింపు
	}
#ఒకవేళ defined(FEAT_GUI)
	ఒకవేళ (gui.లోపల_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_ఎడమ], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_కుడి], FALSE);
	    gui_may_update_scrollbars();
	}
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_కనుగొను_by_id(aco->save_curwin_id);
	ఒకవేళ (save_curwin != ఖాళీ)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    ఒకవేళ (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != ఖాళీ)
	    {
#ఒకవేళ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		ఒకవేళ (curwin->w_s == &curbuf->b_s)
		    curwin->w_s = &aco->new_curbuf.br_buf->b_s;
#అయితే_ముగింపు
		--curbuf->b_nwindows;
		curbuf = aco->new_curbuf.br_buf;
		curwin->w_బఫరు = curbuf;
		++curbuf->b_nwindows;
	    }

	    curwin = save_curwin;
	    curbuf = curwin->w_బఫరు;
	    prevwin = win_కనుగొను_by_id(aco->save_prevwin_id);

	    // In case the autocommand moves the cursor to a position that
	    // does not exist in curbuf.
	    VIsual_చురుకైన = aco->save_VIsual_చురుకైన;
	    check_cursor();
	}
    }

    VIsual_చురుకైన = aco->save_VIsual_చురుకైన;
    check_cursor();	    // just in case lines got deleted
    ఒకవేళ (VIsual_చురుకైన)
	check_pos(curbuf, &VIsual);
}

స్థిర పూర్ణాంకం	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    పూర్ణాంకం
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    పూర్ణాంకం		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    తిరిగి apply_autocmds_సమూహం(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ఖాళీ);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    పూర్ణాంకం
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    పూర్ణాంకం		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    తిరిగి apply_autocmds_సమూహం(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    పూర్ణాంకం
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    పూర్ణాంకం		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    పూర్ణాంకం		*retval)    // pointer to caller's retval
{
    పూర్ణాంకం		did_cmd;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (should_abort(*retval))
	తిరిగి FALSE;
#అయితే_ముగింపు

    did_cmd = apply_autocmds_సమూహం(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ఖాళీ);
    ఒకవేళ (did_cmd
#నిర్వచితమైతే FEAT_EVAL
	    && aborting()
#అయితే_ముగింపు
	    )
	*retval = FAIL;
    తిరిగి did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    స్థిర పూర్ణాంకం
has_cursorhold(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)(get_real_state() == బహుళకం_సాధారణ_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != ఖాళీ);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    పూర్ణాంకం
trigger_cursorhold(శూన్య)
{
    పూర్ణాంకం		state;

    ఒకవేళ (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_చురుకైన())
    {
	state = get_real_state();
	ఒకవేళ (state == బహుళకం_సాధారణ_BUSY || (state & బహుళకం_చొప్పించు) != 0)
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    పూర్ణాంకం
has_winresized(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_WINRESIZED] != ఖాళీ);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    పూర్ణాంకం
has_winscrolled(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_WINSCROLLED] != ఖాళీ);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    పూర్ణాంకం
has_cursormoved(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_CURSORMOVED] != ఖాళీ);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    పూర్ణాంకం
has_cursormovedI(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_CURSORMOVEDI] != ఖాళీ);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    పూర్ణాంకం
has_textchanged(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_TEXTCHANGED] != ఖాళీ);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    పూర్ణాంకం
has_textchangedI(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_TEXTCHANGEDI] != ఖాళీ);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    పూర్ణాంకం
has_textchangedP(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_TEXTCHANGEDP] != ఖాళీ);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    పూర్ణాంకం
has_insertcharpre(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_INSERTCHARPRE] != ఖాళీ);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    పూర్ణాంకం
has_keyinputpre(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_KEYINPUTPRE] != ఖాళీ);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    పూర్ణాంకం
has_cmdundefined(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_CMDUNDEFINED] != ఖాళీ);
}

#ఒకవేళ defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    పూర్ణాంకం
has_textputpost(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_TEXTPUTPOST] != ఖాళీ);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    పూర్ణాంకం
has_textputpre(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_TEXTPUTPRE] != ఖాళీ);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    పూర్ణాంకం
has_textyankpost(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_TEXTYANKPOST] != ఖాళీ);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    పూర్ణాంకం
has_completechanged(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_COMPLETECHANGED] != ఖాళీ);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    పూర్ణాంకం
has_modechanged(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_MODECHANGED] != ఖాళీ);
}
#అయితే_ముగింపు

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    స్థిర పూర్ణాంకం
apply_autocmds_సమూహం(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    పూర్ణాంకం		force,	     // when TRUE, ignore autocmd_busy
    పూర్ణాంకం		సమూహం,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap వాడని) // command arguments
{
    char_u	*sfname = ఖాళీ;	// short file name
    char_u	*తోక;
    పూర్ణాంకం		save_మారింది;
    buf_T	*old_curbuf;
    పూర్ణాంకం		retval = FALSE;
    char_u	*save_autocmd_fname;
    పూర్ణాంకం		save_autocmd_fname_నిండిన;
    పూర్ణాంకం		save_autocmd_bufnr;
    char_u	*save_autocmd_సరిపోలిక;
    పూర్ణాంకం		save_autocmd_busy;
    పూర్ణాంకం		save_autocmd_nested;
    స్థిర పూర్ణాంకం	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#నిర్వచితమైతే FEAT_EVAL
    funccal_నమోదు_T funccal_నమోదు;
    char_u	*save_cmdarg;
    దీర్ఘ	save_cmdbang;
#అయితే_ముగింపు
    స్థిర పూర్ణాంకం	filechangeshell_busy = FALSE;
#నిర్వచితమైతే FEAT_PROFILE
    proftime_T	wait_సమయం;
#అయితే_ముగింపు
    పూర్ణాంకం		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    పూర్ణాంకం		save_కీలకం_Typed = కీలకం_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    ఒకవేళ (event == అంకె_EVENTS || మొదటి_autopat[(పూర్ణాంకం)event] == ఖాళీ
	    || autocmd_blocked > 0)
	వెళ్ళు BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    ఒకవేళ (autocmd_busy && !(force || autocmd_nested))
	వెళ్ళు BYPASS_AU;

#నిర్వచితమైతే FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    ఒకవేళ (aborting())
	వెళ్ళు BYPASS_AU;
#అయితే_ముగింపు

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    ఒకవేళ (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	వెళ్ళు BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    ఒకవేళ (event_ignored(event, p_ei))
	వెళ్ళు BYPASS_AU;

    పూర్ణాంకం win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    ఒకవేళ (buf == curbuf && event_tab[event].కీలకం <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    లేకపోతే ఒకవేళ (buf != ఖాళీ && event_tab[event].కీలకం <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ఒకవేళ (wp->w_బఫరు == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		ఆపు;
	    }
    }
    ఒకవేళ (win_ignore)
	వెళ్ళు BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    ఒకవేళ (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	వెళ్ళు BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    ఒకవేళ (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	వెళ్ళు BYPASS_AU;

    ఒకవేళ (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_మారింది_లెక్క;

    /*
     * Save the autocmd_* variables and info about the current buffer.
     */
    save_autocmd_fname = autocmd_fname;
    save_autocmd_fname_నిండిన = autocmd_fname_నిండిన;
    save_autocmd_bufnr = autocmd_bufnr;
    save_autocmd_సరిపోలిక = autocmd_సరిపోలిక;
    save_autocmd_busy = autocmd_busy;
    save_autocmd_nested = autocmd_nested;
    save_మారింది = curbuf->b_మారింది;
    old_curbuf = curbuf;

    /*
     * Set the file name to be used for <afile>.
     * Make a copy to avoid that changing a buffer name or directory makes it
     * invalid.
     */
    ఒకవేళ (fname_io == ఖాళీ)
    {
	ఒకవేళ (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = ఖాళీ;
	లేకపోతే ఒకవేళ (fname != ఖాళీ && !చివర్లు_excmd(*fname))
	    autocmd_fname = fname;
	లేకపోతే ఒకవేళ (buf != ఖాళీ)
	    autocmd_fname = buf->b_ffname;
	లేకపోతే
	    autocmd_fname = ఖాళీ;
    }
    లేకపోతే
	autocmd_fname = fname_io;
    ఒకవేళ (autocmd_fname != ఖాళీ)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_నిండిన = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    ఒకవేళ (buf == ఖాళీ)
	autocmd_bufnr = 0;
    లేకపోతే
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    ఒకవేళ (fname == ఖాళీ || *fname == NUL)
    {
	ఒకవేళ (buf == ఖాళీ)
	    fname = ఖాళీ;
	లేకపోతే
	{
#నిర్వచితమైతే FEAT_SYN_HL
	    ఒకవేళ (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    లేకపోతే
#అయితే_ముగింపు
		ఒకవేళ (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		లేకపోతే
		{
		    ఒకవేళ (buf->b_sfname != ఖాళీ)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	ఒకవేళ (fname == ఖాళీ)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    లేకపోతే
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	ఒకవేళ (event == EVENT_FILETYPE
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
	    autocmd_fname_నిండిన = TRUE; // don't expand it later
	}
	లేకపోతే
	    fname = నిండిన_Name_save(fname, FALSE);
    }
    ఒకవేళ (fname == ఖాళీ)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	వెళ్ళు BYPASS_AU;
    }

#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    ఒకవేళ (sfname != ఖాళీ)
	forward_slash(sfname);
    forward_slash(fname);
#అయితే_ముగింపు

#నిర్వచితమైతే VMS
    // remove version for correct match
    ఒకవేళ (sfname != ఖాళీ)
	vms_remove_సంస్కరణ(sfname);
    vms_remove_సంస్కరణ(fname);
#అయితే_ముగింపు

    /*
     * Set the name to be used for <amatch>.
     */
    autocmd_సరిపోలిక = fname;


    // Don't redraw while doing autocommands.
    ++RedrawingDisabled;

    // name and lnum are filled in later
    estack_నెట్టు(ETYPE_AUCMD, ఖాళీ, 0);
    ESTACK_CHECK_అమరిక;

    save_current_sctx = current_sctx;

#నిర్వచితమైతే FEAT_EVAL
# ifdef FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
	prof_బిడ్డ_enter(&wait_సమయం); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_నమోదు);
#అయితే_ముగింపు

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    ఒకవేళ (!autocmd_busy)
    {
	save_శోధన_patterns();
	ఒకవేళ (!ins_compl_చురుకైన())
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
    ఒకవేళ (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    తోక = gettail(fname);

    // Find first autocommand that matches
    CLEAR_క్షేత్రం(patcmd);
    patcmd.curpat = మొదటి_autopat[(పూర్ణాంకం)event];
    patcmd.సమూహం = సమూహం;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.తోక = తోక;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_తదుపరి_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    ఒకవేళ (patcmd.curpat != ఖాళీ)
    {
	// add to active_apc_list
	patcmd.తదుపరి = చురుకైన_apc_list;
	చురుకైన_apc_list = &patcmd;

#నిర్వచితమైతే FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (దీర్ఘ)get_vim_var_nr(VV_CMDBANG);
	ఒకవేళ (eap != ఖాళీ)
	{
	    save_cmdarg = సమితి_cmdarg(eap, ఖాళీ);
	    సమితి_vim_var_nr(VV_CMDBANG, (దీర్ఘ)eap->forceit);
	}
	లేకపోతే
	    save_cmdarg = ఖాళీ;	// avoid gcc warning
#అయితే_ముగింపు
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	కొరకు (ap = patcmd.curpat; ap->తదుపరి != ఖాళీ; ap = ap->తదుపరి)
	    ap->చివరి = FALSE;
	ap->చివరి = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	ఒకవేళ (nesting == 1)
	    check_lnums(TRUE);
	లేకపోతే
	    check_lnums_nested(TRUE);

	పూర్ణాంకం save_did_emsg = did_emsg;
	పూర్ణాంకం save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(ఖాళీ, getnextac, (శూన్య *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	సమితి_pressedreturn(save_ex_pressedreturn);

	ఒకవేళ (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (eap != ఖాళీ)
	{
	    (శూన్య)సమితి_cmdarg(ఖాళీ, save_cmdarg);
	    సమితి_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#అయితే_ముగింపు
	// delete from active_apc_list
	ఒకవేళ (చురుకైన_apc_list == &patcmd)	    // just in case
	    చురుకైన_apc_list = patcmd.తదుపరి;
    }

    ఒకవేళ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_తీయి();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_నిండిన = save_autocmd_fname_నిండిన;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_సరిపోలిక = save_autocmd_సరిపోలిక;
    current_sctx = save_current_sctx;
#నిర్వచితమైతే FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
	prof_బిడ్డ_exit(&wait_సమయం);
# endif
#అయితే_ముగింపు
    కీలకం_Typed = save_కీలకం_Typed;
    vim_free(fname);
    vim_free(sfname);
    --nesting;		// see matching increment above

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.  Free any buffers in the au_pending_free_buf list and
     * free any windows in the au_pending_free_win list.
     */
    ఒకవేళ (!autocmd_busy)
    {
	restore_శోధన_patterns();
	ఒకవేళ (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	అయితే (au_pending_free_buf != ఖాళీ)
	{
	    buf_T *b = au_pending_free_buf->b_తదుపరి;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	అయితే (au_pending_free_win != ఖాళీ)
	{
	    win_T *w = au_pending_free_win->w_తదుపరి;

	    vim_free(au_pending_free_win);
	    au_pending_free_win = w;
	}
    }

    /*
     * Some events don't set or reset the Changed flag.
     * Check if still in the same buffer!
     */
    ఒకవేళ (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	ఒకవేళ (curbuf->b_మారింది != save_మారింది)
	    need_maketitle = TRUE;
	curbuf->b_మారింది = save_మారింది;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    ఒకవేళ (event == EVENT_BUFWIPEOUT && buf != ఖాళీ)
	aubuflocal_remove(buf);

    ఒకవేళ (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    తిరిగి retval;
}

#నిర్వచితమైతే FEAT_EVAL
స్థిర char_u	*old_termresponse = ఖాళీ;
స్థిర char_u	*old_termu7resp = ఖాళీ;
స్థిర char_u	*old_termblinkresp = ఖాళీ;
స్థిర char_u	*old_termrbgresp = ఖాళీ;
స్థిర char_u	*old_termrfgresp = ఖాళీ;
స్థిర char_u	*old_termstyleresp = ఖాళీ;
#అయితే_ముగింపు

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    శూన్య
ఖండం_autocmds(శూన్య)
{
#నిర్వచితమైతే FEAT_EVAL
    // Remember the value of v:termresponse.
    ఒకవేళ (autocmd_blocked == 0)
    {
	old_termresponse = get_vim_var_str(VV_TERMRESPONSE);
	old_termu7resp = get_vim_var_str(VV_TERMU7RESP);
	old_termblinkresp = get_vim_var_str(VV_TERMBLINKRESP);
	old_termrbgresp = get_vim_var_str(VV_TERMRBGRESP);
	old_termrfgresp = get_vim_var_str(VV_TERMRFGRESP);
	old_termstyleresp = get_vim_var_str(VV_TERMSTYLERESP);
    }
#అయితే_ముగింపు
    ++autocmd_blocked;
}

    శూన్య
unblock_autocmds(శూన్య)
{
    --autocmd_blocked;

#నిర్వచితమైతే FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    ఒకవేళ (autocmd_blocked == 0)
    {
	ఒకవేళ (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, ఖాళీ, ఖాళీ, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", ఖాళీ, FALSE, curbuf);
	}
	ఒకవేళ (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", ఖాళీ, FALSE, curbuf);
	}
	ఒకవేళ (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", ఖాళీ, FALSE, curbuf);
	}
	ఒకవేళ (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", ఖాళీ, FALSE, curbuf);
	}
	ఒకవేళ (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", ఖాళీ, FALSE, curbuf);
	}
	ఒకవేళ (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", ఖాళీ, FALSE, curbuf);
	}
    }
#అయితే_ముగింపు
}

    పూర్ణాంకం
is_autocmd_blocked(శూన్య)
{
    తిరిగి autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    స్థిర శూన్య
auto_తదుపరి_pat(
    AutoPatCmd_T *apc,
    పూర్ణాంకం		stop_at_చివరి)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*నమోదు_2;

    నమోదు_2 = ((estack_T *)exestack.ga_దత్తాంశం) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(నమోదు_2->es_name);
    నమోదు_2->es_info.aucmd = ఖాళీ;

    కొరకు (ap = apc->curpat; ap != ఖాళీ && !got_int; ap = ap->తదుపరి)
    {
	apc->curpat = ఖాళీ;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	ఒకవేళ (ap->pat != ఖాళీ && ap->cmds != ఖాళీ
		&& (apc->సమూహం == AUGROUP_ALL || apc->సమూహం == ap->సమూహం))
	{
	    // execution-condition
	    ఒకవేళ (ap->buflocal_nr == 0
		    ? (సరిపోలిక_file_pat(ఖాళీ, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->తోక, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		అక్షరం	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->నిడివి + ap->patlen + 1);
		ఒకవేళ (namep != ఖాళీ)
		{
		    sprintf((అక్షరం *)namep, fmt, (అక్షరం *)event_name->string, (అక్షరం *)ap->pat);
		    ఒకవేళ (p_verbose >= 8)
		    {
			verbose_enter();
			smsg(_("Executing %s"), namep);
			verbose_leave();
		    }
		}

		// Update the exestack entry for this autocmd.
		నమోదు_2->es_name = namep;
		నమోదు_2->es_info.aucmd = apc;

		apc->curpat = ap;
		apc->nextcmd = ap->cmds;
		// mark last command
		కొరకు (cp = ap->cmds; cp->తదుపరి != ఖాళీ; cp = cp->తదుపరి)
		    cp->చివరి = FALSE;
		cp->చివరి = TRUE;
	    }
	    పంక్తి_breakcheck();
	    ఒకవేళ (apc->curpat != ఖాళీ)	    // found a match
		ఆపు;
	}
	ఒకవేళ (stop_at_చివరి && ap->చివరి)
	    ఆపు;
    }
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    తిరిగి &acp->script_ctx;
}
#అయితే_ముగింపు

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	పూర్ణాంకం c వాడని,
	శూన్య *cookie,
	పూర్ణాంకం indent వాడని,
	getline_opt_T options వాడని)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    ఒకవేళ (acp->curpat == ఖాళీ)
	తిరిగి ఖాళీ;

    // repeat until we find an autocommand to execute
    కొరకు (;;)
    {
	// skip removed commands
	అయితే (acp->nextcmd != ఖాళీ && acp->nextcmd->cmd == ఖాళీ)
	    ఒకవేళ (acp->nextcmd->చివరి)
		acp->nextcmd = ఖాళీ;
	    లేకపోతే
		acp->nextcmd = acp->nextcmd->తదుపరి;

	ఒకవేళ (acp->nextcmd != ఖాళీ)
	    ఆపు;

	// at end of commands, find next pattern that matches
	ఒకవేళ (acp->curpat->చివరి)
	    acp->curpat = ఖాళీ;
	లేకపోతే
	    acp->curpat = acp->curpat->తదుపరి;
	ఒకవేళ (acp->curpat != ఖాళీ)
	    auto_తదుపరి_pat(acp, TRUE);
	ఒకవేళ (acp->curpat == ఖాళీ)
	    తిరిగి ఖాళీ;
    }

    ac = acp->nextcmd;

    ఒకవేళ (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	సందేశం_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    ఒకవేళ (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    ఒకవేళ (ac->చివరి)
	acp->nextcmd = ఖాళీ;
    లేకపోతే
	acp->nextcmd = ac->తదుపరి;
    తిరిగి retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    స్థిర పూర్ణాంకం
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_నిండిన)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*తోక = gettail(sfname);
    పూర్ణాంకం		retval = FALSE;

    ఒకవేళ (మొదటి_autopat[(పూర్ణాంకం)event] == ఖాళీ)
	తిరిగి FALSE;

    fname = is_నిండిన ? vim_strsave(sfname) : నిండిన_Name_save(sfname, FALSE);
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి FALSE;

#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    ఒకవేళ (sfname != ఖాళీ)
	forward_slash(sfname);
    forward_slash(fname);
#అయితే_ముగింపు

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	ఒకవేళ (ap->pat != ఖాళీ && ap->cmds != ఖాళీ
	      && (ap->buflocal_nr == 0
		? సరిపోలిక_file_pat(ఖాళీ, &ap->reg_prog,
					  fname, sfname, తోక, ap->allow_dirs)
		: buf != ఖాళీ && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    ఆపు;
	}

    vim_free(fname);
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    vim_free(sfname);
#అయితే_ముగింపు

    తిరిగి retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    పూర్ణాంకం
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    తిరిగి has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    పూర్ణాంకం
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    తిరిగి has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx == augroups.ga_len)		// add "END" add the end
	తిరిగి (char_u *)"END";
    ఒకవేళ (idx < 0 || idx >= augroups.ga_len)	// end of list
	తిరిగి ఖాళీ;
    ఒకవేళ (AUGROUP_NAME(idx) == ఖాళీ || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	తిరిగి (char_u *)"";
    తిరిగి AUGROUP_NAME(idx);		// return a name
}

స్థిర పూర్ణాంకం include_groups = FALSE;

    char_u  *
సమితి_context_లోపల_autocmd(
    expand_T	*xp,
    char_u	*arg,
    పూర్ణాంకం		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    పూర్ణాంకం		సమూహం;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    సమూహం = au_get_grouparg(&arg);
    ఒకవేళ (సమూహం == AUGROUP_ERROR)
	తిరిగి ఖాళీ;
    // If there only is a group name that's what we expand.
    ఒకవేళ (*arg == NUL && సమూహం != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	సమూహం = AUGROUP_ALL;
    }

    // skip over event name
    కొరకు (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	ఒకవేళ (*p == ',')
	    arg = p + 1;
    ఒకవేళ (*p == NUL)
    {
	ఒకవేళ (సమూహం == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_నమూనా = arg;
	తిరిగి ఖాళీ;
    }

    // skip over pattern
    arg = skipwhite(p);
    అయితే (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    ఒకవేళ (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	కొరకు (పూర్ణాంకం i = 0; i < 2; i++)
	{
	    ఒకవేళ (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    ఒకవేళ (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    ఒకవేళ (!లోపల_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	తిరిగి arg;			    // expand (next) command
    }

    ఒకవేళ (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    లేకపోతే
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    తిరిగి ఖాళీ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    పూర్ణాంకం i;

    ఒకవేళ (idx < augroups.ga_len)		// First list group names, if wanted
    {
	ఒకవేళ (!include_groups || AUGROUP_NAME(idx) == ఖాళీ
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    తిరిగి (char_u *)"";	// skip deleted entries
	తిరిగి AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    ఒకవేళ (i < 0 || i >= అంకె_EVENTS)
	తిరిగి ఖాళీ;

    తిరిగి event_tab[i].విలువ.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_సమూహం(expand_T *xp వాడని, పూర్ణాంకం idx, పూర్ణాంకం win)
{
    ఒకవేళ (idx < 0 || idx >= అంకె_EVENTS)
	తిరిగి ఖాళీ;

    ఒకవేళ (!win)
	తిరిగి event_tab[idx].విలువ.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    పూర్ణాంకం j = 0;
    కొరకు (పూర్ణాంకం i = 0; i < అంకె_EVENTS; ++i)
    {
	j += event_tab[i].కీలకం <= 0;
	ఒకవేళ (j == idx + 1)
	    తిరిగి event_tab[i].విలువ.string;
    }

    తిరిగి ఖాళీ;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    పూర్ణాంకం
has_tabclosedpre(శూన్య)
{
    తిరిగి (మొదటి_autopat[(పూర్ణాంకం)EVENT_TABCLOSEDPRE] != ఖాళీ);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    పూర్ణాంకం
autocmd_supported(char_u *name)
{
    char_u *p;

    తిరిగి (event_name2nr(name, &p) != అంకె_EVENTS);
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
    పూర్ణాంకం
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*నమూనా = ఖాళీ;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = ఖాళీ;
    పూర్ణాంకం		సమూహం;
    పూర్ణాంకం		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    ఒకవేళ (arg_save == ఖాళీ)
	తిరిగి FALSE;
    p = vim_strchr(arg_save, '#');
    ఒకవేళ (p != ఖాళీ)
	*p++ = NUL;

    // First, look for an autocmd group name
    సమూహం = au_కనుగొను_సమూహం(arg_save);
    ఒకవేళ (సమూహం == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	సమూహం = AUGROUP_ALL;
	event_name = arg_save;
    }
    లేకపోతే
    {
	ఒకవేళ (p == ఖాళీ)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    వెళ్ళు theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	ఒకవేళ (p != ఖాళీ)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    నమూనా = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    ఒకవేళ (event == అంకె_EVENTS)
	వెళ్ళు theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = మొదటి_autopat[(పూర్ణాంకం)event];
    ఒకవేళ (ap == ఖాళీ)
	వెళ్ళు theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    ఒకవేళ (నమూనా != ఖాళీ && STRICMP(నమూనా, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    కొరకు ( ; ap != ఖాళీ; ap = ap->తదుపరి)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	ఒకవేళ (ap->pat != ఖాళీ && ap->cmds != ఖాళీ
	    && (సమూహం == AUGROUP_ALL || ap->సమూహం == సమూహం)
	    && (నమూనా == ఖాళీ
		|| (buflocal_buf == ఖాళీ
		    ? fnamecmp(ap->pat, నమూనా) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    ఆపు;
	}

theend:
    vim_free(arg_save);
    తిరిగి retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    స్థిర శూన్య
autocmd_కూడు_or_delete(typval_T *argvars, typval_T *rettv, పూర్ణాంకం delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = ఖాళీ;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*సమూహం_name = ఖాళీ;
    పూర్ణాంకం		సమూహం;
    char_u	*pat = ఖాళీ;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = ఖాళీ;
    char_u	*చివర;
    పూర్ణాంకం		once;
    పూర్ణాంకం		nested;
    పూర్ణాంకం		replace;		// replace the cmd for a group/event
    పూర్ణాంకం		retval = VVAL_TRUE;
    పూర్ణాంకం		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_సంఖ్య = VVAL_FALSE;

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (check_for_list_arg(argvars, 0) == FAIL)
	తిరిగి;

    aucmd_list = argvars[0].vval.v_list;
    ఒకవేళ (aucmd_list == ఖాళీ)
	తిరిగి;

    FOR_ALL_LIST_వస్తువులు(aucmd_list, li)
    {
	VIM_CLEAR(సమూహం_name);
	VIM_CLEAR(cmd);
	event_name = ఖాళీ;
	event_list = ఖాళీ;
	pat = ఖాళీ;
	pat_list = ఖాళీ;

	ఒకవేళ (li->li_tv.v_type != VAR_DICT)
	    కొనసాగు;

	event_dict = li->li_tv.vval.v_dict;
	ఒకవేళ (event_dict == ఖాళీ)
	    కొనసాగు;

	di = dict_కనుగొను(event_dict, (char_u *)"event", -1);
	ఒకవేళ (di != ఖాళీ)
	{
	    ఒకవేళ (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		ఒకవేళ (event_name == ఖాళీ)
		{
		    emsg(_(e_string_required));
		    కొనసాగు;
		}
	    }
	    లేకపోతే ఒకవేళ (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		ఒకవేళ (event_list == ఖాళీ)
		{
		    emsg(_(e_list_required));
		    కొనసాగు;
		}
	    }
	    లేకపోతే
	    {
		emsg(_(e_string_or_list_expected));
		కొనసాగు;
	    }
	}

	సమూహం_name = dict_get_string(event_dict, "group", TRUE);
	ఒకవేళ (సమూహం_name == ఖాళీ || *సమూహం_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    సమూహం = current_augroup;
	లేకపోతే
	{
	    సమూహం = au_కనుగొను_సమూహం(సమూహం_name);
	    ఒకవేళ (సమూహం == AUGROUP_ERROR)
	    {
		ఒకవేళ (delete)
		{
		    semsg(_(e_no_such_సమూహం_str), సమూహం_name);
		    retval = VVAL_FALSE;
		    ఆపు;
		}
		// group is not found, create it now
		సమూహం = au_new_సమూహం(సమూహం_name);
		ఒకవేళ (సమూహం == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_సమూహం_str), సమూహం_name);
		    retval = VVAL_FALSE;
		    ఆపు;
		}

		current_augroup = సమూహం;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	ఒకవేళ (dict_has_కీలకం(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_సంఖ్య_def(event_dict, "bufnr", -1);
	    ఒకవేళ (bnum == -1)
		కొనసాగు;

	    vim_snprintf((అక్షరం *)IObuff, IOSIZE, "<buffer=%d>", (పూర్ణాంకం)bnum);
	    pat = IObuff;
	}
	లేకపోతే
	{
	    di = dict_కనుగొను(event_dict, (char_u *)"pattern", -1);
	    ఒకవేళ (di != ఖాళీ)
	    {
		ఒకవేళ (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    ఒకవేళ (pat == ఖాళీ)
		    {
			emsg(_(e_string_required));
			కొనసాగు;
		    }
		}
		లేకపోతే ఒకవేళ (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    ఒకవేళ (pat_list == ఖాళీ)
		    {
			emsg(_(e_list_required));
			కొనసాగు;
		    }
		}
		లేకపోతే
		{
		    emsg(_(e_string_or_list_expected));
		    కొనసాగు;
		}
	    }
	    లేకపోతే ఒకవేళ (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	ఒకవేళ (cmd == ఖాళీ)
	{
	    ఒకవేళ (delete)
		cmd = vim_strsave((char_u *)"");
	    లేకపోతే
		కొనసాగు;
	}

	ఒకవేళ (delete && (event_name == ఖాళీ
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    కొరకు (event = (event_T)0; (పూర్ణాంకం)event < అంకె_EVENTS;
		    event = (event_T)((పూర్ణాంకం)event + 1))
	    {
		ఒకవేళ (do_autocmd_event(event, pat, once, nested, cmd, delete,
							సమూహం, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    ఆపు;
		}
	    }
	}
	లేకపోతే
	{
	    char_u *p = ఖాళీ;

	    eli = ఖాళీ;
	    చివర = ఖాళీ;
	    అయితే (TRUE)
	    {
		ఒకవేళ (event_list != ఖాళీ)
		{
		    ఒకవేళ (eli == ఖాళీ)
			eli = event_list->lv_మొదటి;
		    లేకపోతే
			eli = eli->li_తదుపరి;
		    ఒకవేళ (eli == ఖాళీ)
			ఆపు;
		    ఒకవేళ (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == ఖాళీ)
		    {
			emsg(_(e_string_required));
			ఆపు;
		    }
		}
		లేకపోతే
		{
		    ఒకవేళ (p == ఖాళీ)
			p = event_name;
		    ఒకవేళ (p == ఖాళీ || *p == NUL)
			ఆపు;
		}

		event = event_name2nr(p, &చివర);
		ఒకవేళ (event == అంకె_EVENTS || *చివర != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    ఆపు;
		}
		ఒకవేళ (pat != ఖాళీ)
		{
		    ఒకవేళ (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, సమూహం, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			ఆపు;
		    }
		}
		లేకపోతే ఒకవేళ (pat_list != ఖాళీ)
		{
		    FOR_ALL_LIST_వస్తువులు(pat_list, pli)
		    {
			ఒకవేళ (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == ఖాళీ)
			{
			    emsg(_(e_string_required));
			    కొనసాగు;
			}
			ఒకవేళ (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, సమూహం, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    ఆపు;
			}
		    }
		    ఒకవేళ (retval == VVAL_FALSE)
			ఆపు;
		}
		ఒకవేళ (event_name != ఖాళీ)
		    p = చివర;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	ఒకవేళ (delete && సమూహం_name != ఖాళీ &&
		(event_name == ఖాళీ || event_name[0] == NUL)
		&& (pat == ఖాళీ || pat[0] == NUL)
		&& (cmd == ఖాళీ || cmd[0] == NUL))
	    au_del_సమూహం(సమూహం_name);
    }

    VIM_CLEAR(సమూహం_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_సంఖ్య = retval;
}

/*
 * autocmd_add() function
 */
    శూన్య
f_autocmd_కూడు(typval_T *argvars, typval_T *rettv)
{
    autocmd_కూడు_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    శూన్య
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_కూడు_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    శూన్య
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = అంకె_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = ఖాళీ;
    char_u	*name = ఖాళీ;
    పూర్ణాంకం		సమూహం = AUGROUP_ALL;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;
    ఒకవేళ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	ఒకవేళ (dict_has_కీలకం(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    ఒకవేళ (name == ఖాళీ)
		తిరిగి;

	    ఒకవేళ (*name == NUL)
		సమూహం = AUGROUP_DEFAULT;
	    లేకపోతే
	    {
		సమూహం = au_కనుగొను_సమూహం(name);
		ఒకవేళ (సమూహం == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_సమూహం_str), name);
		    vim_free(name);
		    తిరిగి;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	ఒకవేళ (dict_has_కీలకం(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    ఒకవేళ (name == ఖాళీ)
		తిరిగి;

	    ఒకవేళ (name[0] == '*' && name[1] == NUL)
		event_arg = అంకె_EVENTS;
	    లేకపోతే
	    {
		keyvalue_T లక్ష్యం;
		keyvalue_T *నమోదు_2;

		లక్ష్యం.కీలకం = 0;
		లక్ష్యం.విలువ.string = name;
		లక్ష్యం.విలువ.నిడివి = STRLEN(లక్ష్యం.విలువ.string);
		నమోదు_2 = (keyvalue_T *)bsearch(&లక్ష్యం, &event_tab,
		    అంకె_EVENTS, పరిమాణం(event_tab[0]), cmp_keyvalue_విలువ_ni);
		ఒకవేళ (నమోదు_2 == ఖాళీ)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    తిరిగి;
		}
		event_arg = (event_T)నిరపేక్ష(నమోదు_2->కీలకం);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	ఒకవేళ (dict_has_కీలకం(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    ఒకవేళ (pat == ఖాళీ)
		తిరిగి;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    కొరకు (event = (event_T)0; (పూర్ణాంకం)event < అంకె_EVENTS;
	    event = (event_T)((పూర్ణాంకం)event + 1))
    {
	string_T    *event_name;

	ఒకవేళ (event_arg != అంకె_EVENTS && event != event_arg)
	    కొనసాగు;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	సమూహం_name;

	    ఒకవేళ (ap->pat == ఖాళీ)		// pattern has been removed
		కొనసాగు;

	    ఒకవేళ (సమూహం != AUGROUP_ALL && సమూహం != ap->సమూహం)
		కొనసాగు;

	    ఒకవేళ (pat != ఖాళీ && STRCMP(pat, ap->pat) != 0)
		కొనసాగు;

	    సమూహం_name.string = get_augroup_name(ఖాళీ, ap->సమూహం);
	    ఒకవేళ (సమూహం_name.string == ఖాళీ)
		STR_LITERAL_సమితి(సమూహం_name, "");
	    లేకపోతే
		సమూహం_name.నిడివి = STRLEN(సమూహం_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    కొరకు (ac = ap->cmds; ac != ఖాళీ; ac = ac->తదుపరి)
	    {
		event_dict = dict_alloc();
		ఒకవేళ (event_dict == ఖాళీ
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    తిరిగి;
		}

		ఒకవేళ (dict_కూడు_string_len(event_dict, "event",
			event_name->string, (పూర్ణాంకం)event_name->నిడివి) == FAIL
			|| dict_కూడు_string_len(event_dict, "group",
			    సమూహం_name.string, (పూర్ణాంకం)సమూహం_name.నిడివి) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_కూడు_సంఖ్య(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_కూడు_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_కూడు_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_కూడు_bool(event_dict, "once", ac->once) == FAIL
			|| dict_కూడు_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    తిరిగి;
		}
	    }
	}
    }

    vim_free(pat);
}

#అయితే_ముగింపు
