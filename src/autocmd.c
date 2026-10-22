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

#ಅನ್ತರ್ಭಾವ "vim.h"

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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    ವರ್ಣ	    once;		// "One shot": removed after execution
    ವರ್ಣ	    nested;		// If autocommands nest here.
    ವರ್ಣ	    last;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    ಸಂರಚನಾ AutoCmd  *next;		// next AutoCmd in list
} AutoCmd;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ AutoPat
{
    ಸಂರಚನಾ AutoPat  *next;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    ಪೂರ್ಣಾಙ್ಕ		    group;		// group ID
    ಪೂರ್ಣಾಙ್ಕ		    patlen;		// strlen() of pat
    ಪೂರ್ಣಾಙ್ಕ		    buflocal_nr;	// !=0 for buffer-local AutoPat
    ವರ್ಣ	    allow_dirs;		// Pattern may match whole path
    ವರ್ಣ	    last;		// last pattern for apply_autocmds()
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
ಸ್ಥಿರ keyvalue_T event_tab[ಅಂಕೆ_EVENTS] = {
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

ಸ್ಥಿರ AutoPat *first_autopat[ಅಂಕೆ_EVENTS] = { ರಿಕ್ತ };
ಸ್ಥಿರ AutoPat *last_autopat[ಅಂಕೆ_EVENTS] = { ರಿಕ್ತ };

#define AUGROUP_DEFAULT    (-1)	    // default autocmd group
#define AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#define AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
ಸಂರಚನಾ AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    ಪೂರ್ಣಾಙ್ಕ		group;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*tail;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    ಪೂರ್ಣಾಙ್ಕ		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *next;		// chain of active apc-s for auto-invalidation
};

ಸ್ಥಿರ AutoPatCmd_T *active_apc_list = ರಿಕ್ತ; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#define FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    ಪ್ರತಿ ((ap) = first_autopat[(ಪೂರ್ಣಾಙ್ಕ)(event)]; (ap) != ರಿಕ್ತ; (ap) = (ap)->next)

/*
 * augroups stores a list of autocmd group names.
 */
ಸ್ಥಿರ garray_T augroups = {0, 0, ಪರಿಮಾಣ(char_u *), 10, ರಿಕ್ತ};
#define AUGROUP_NAME(i) (((char_u **)augroups.ga_data)[i])
// use get_deleted_augroup() to get this
ಸ್ಥಿರ char_u *deleted_augroup = ರಿಕ್ತ;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ current_augroup = AUGROUP_DEFAULT;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ au_need_clean = FALSE;   // need to delete marked patterns

ಸ್ಥಿರ event_T event_name2nr(char_u *start, char_u **end);
ಸ್ಥಿರ string_T *event_nr2name(event_T event);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ au_get_grouparg(char_u **argp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ do_autocmd_event(event_T event, char_u *pat, ಪೂರ್ಣಾಙ್ಕ once, ಪೂರ್ಣಾಙ್ಕ nested, char_u *cmd, ಪೂರ್ಣಾಙ್ಕ forceit, ಪೂರ್ಣಾಙ್ಕ group, ಪೂರ್ಣಾಙ್ಕ flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ apply_autocmds_group(event_T event, char_u *fname, char_u *fname_io, ಪೂರ್ಣಾಙ್ಕ force, ಪೂರ್ಣಾಙ್ಕ group, buf_T *buf, exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ auto_next_pat(AutoPatCmd_T *apc, ಪೂರ್ಣಾಙ್ಕ stop_at_last);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ au_find_group(char_u *name);

ಸ್ಥಿರ event_T	last_event;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	last_group;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	autocmd_blocked = 0;	// block all autocmds

    ಸ್ಥಿರ char_u *
get_deleted_augroup(ಶೂನ್ಯ)
{
    ಯದಿ (deleted_augroup == ರಿಕ್ತ)
	deleted_augroup = (char_u *)_("--Deleted--");
    ಪ್ರತ್ಯಾಗಮ deleted_augroup;
}

/*
 * Show the autocommands for one AutoPat.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    // Check for "got_int" (here and at various places below), which is set
    // when "q" has been hit for the "--more--" prompt
    ಯದಿ (got_int)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (ap->pat == ರಿಕ್ತ)		// pattern has been removed
	ಪ್ರತ್ಯಾಗಮ;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    ಸಂದೇಶ_putchar('\n');
    ಯದಿ (got_int)
	ಗಚ್ಛ theend;
    ಯದಿ (event != last_event || ap->group != last_group)
    {
	string_T    *event_name;

	ಯದಿ (ap->group != AUGROUP_DEFAULT)
	{
	    ಯದಿ (AUGROUP_NAME(ap->group) == ರಿಕ್ತ)
		ಸಂದೇಶ_puts_attr((ವರ್ಣ *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    ಅನ್ಯಥಾ
		ಸಂದೇಶ_puts_attr((ವರ್ಣ *)AUGROUP_NAME(ap->group), HL_ATTR(HLF_T));
	    ಸಂದೇಶ_puts("  ");
	}
	event_name = event_nr2name(event);
	ಸಂದೇಶ_puts_attr((ವರ್ಣ *)event_name->string, HL_ATTR(HLF_T));
	last_event = event;
	last_group = ap->group;
	ಸಂದೇಶ_putchar('\n');
	ಯದಿ (got_int)
	    ಗಚ್ಛ theend;
    }

    ಯದಿ (ap->pat == ರಿಕ್ತ)
	ಗಚ್ಛ theend;  // timer might have cleared the pattern or group

    ಸಂದೇಶ_col = 4;
    ಸಂದೇಶ_outtrans(ap->pat);

    ಪ್ರತಿ (ac = ap->cmds; ac != ರಿಕ್ತ; ac = ac->next)
    {
	ಯದಿ (ac->cmd == ರಿಕ್ತ)		// skip removed commands
	    ಅನುವರ್ತ;

	ಯದಿ (ಸಂದೇಶ_col >= 14)
	    ಸಂದೇಶ_putchar('\n');
	ಸಂದೇಶ_col = 14;
	ಯದಿ (got_int)
	    ಗಚ್ಛ theend;
	ಸಂದೇಶ_outtrans(ac->cmd);
#ifdef FEAT_EVAL
	ಯದಿ (p_verbose > 0)
	    last_set_ಸಂದೇಶ(ac->script_ctx);
#endif
	ಯದಿ (got_int)
	    ಗಚ್ಛ theend;
	ಯದಿ (ac->next != ರಿಕ್ತ)
	{
	    ಸಂದೇಶ_putchar('\n');
	    ಯದಿ (got_int)
		ಗಚ್ಛ theend;
	}
    }

theend:
    --autocmd_busy;
}

/*
 * Mark an autocommand pattern for deletion.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
au_remove_pat(AutoPat *ap)
{
    VIM_CLEAR(ap->pat);
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    ಪ್ರತಿ (ac = ap->cmds; ac != ರಿಕ್ತ; ac = ac->next)
	VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

// Delete one command from an autocmd pattern.
ಸ್ಥಿರ ಶೂನ್ಯ au_del_cmd(AutoCmd *ac)
{
    VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
au_cleanup(ಶೂನ್ಯ)
{
    AutoPat	*ap, **prev_ap;
    AutoCmd	*ac, **prev_ac;
    event_T	event;

    ಯದಿ (autocmd_busy || !au_need_clean)
	ಪ್ರತ್ಯಾಗಮ;

    // loop over all events
    ಪ್ರತಿ (event = (event_T)0; (ಪೂರ್ಣಾಙ್ಕ)event < ಅಂಕೆ_EVENTS;
					    event = (event_T)((ಪೂರ್ಣಾಙ್ಕ)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(first_autopat[(ಪೂರ್ಣಾಙ್ಕ)event]);
	ಪ್ರತಿ (ap = *prev_ap; ap != ರಿಕ್ತ; ap = *prev_ap)
	{
	    ಪೂರ್ಣಾಙ್ಕ has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    ಪ್ರತಿ (ac = *prev_ac; ac != ರಿಕ್ತ; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		ಯದಿ (ap->pat == ರಿಕ್ತ || ac->cmd == ರಿಕ್ತ)
		{
		    *prev_ac = ac->next;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		ಅನ್ಯಥಾ
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->next);
		}
	    }

	    ಯದಿ (ap->pat != ರಿಕ್ತ && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    ಯದಿ (ap->pat == ರಿಕ್ತ)
	    {
		ಯದಿ (ap->next == ರಿಕ್ತ)
		{
		    ಯದಿ (prev_ap == &(first_autopat[(ಪೂರ್ಣಾಙ್ಕ)event]))
			last_autopat[(ಪೂರ್ಣಾಙ್ಕ)event] = ರಿಕ್ತ;
		    ಅನ್ಯಥಾ
			// this depends on the "next" field being the first in
			// the struct
			last_autopat[(ಪೂರ್ಣಾಙ್ಕ)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->next;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    ಅನ್ಯಥಾ
		prev_ap = &(ap->next);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Called when buffer is freed, to remove/invalidate related buffer-local
 * autocmds.
 */
    ಶೂನ್ಯ
aubuflocal_remove(buf_T *buf)
{
    AutoPat	    *ap;
    event_T	    event;
    AutoPatCmd_T    *apc;

    // invalidate currently executing autocommands
    ಪ್ರತಿ (apc = active_apc_list; apc; apc = apc->next)
	ಯದಿ (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    ಪ್ರತಿ (event = (event_T)0; (ಪೂರ್ಣಾಙ್ಕ)event < ಅಂಕೆ_EVENTS;
					    event = (event_T)((ಪೂರ್ಣಾಙ್ಕ)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ಯದಿ (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		ಯದಿ (p_verbose >= 6)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
au_new_group(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    i = au_find_group(name);
    ಯದಿ (i != AUGROUP_ERROR)
	ಪ್ರತ್ಯಾಗಮ i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    ಪ್ರತಿ (i = 0; i < augroups.ga_len; ++i)
	ಯದಿ (AUGROUP_NAME(i) == ರಿಕ್ತ)
	    ಭಙ್ಗ;
    ಯದಿ (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    ಯದಿ (AUGROUP_NAME(i) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ AUGROUP_ERROR;
    ಯದಿ (i == augroups.ga_len)
	++augroups.ga_len;

    ಪ್ರತ್ಯಾಗಮ i;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
au_del_group(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    event_T	event;
    AutoPat	*ap;
    ಪೂರ್ಣಾಙ್ಕ		in_use = FALSE;


    i = au_find_group(name);
    ಯದಿ (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_group_str), name);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_group));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (event = (event_T)0; (ಪೂರ್ಣಾಙ್ಕ)event < ಅಂಕೆ_EVENTS;
	    event = (event_T)((ಪೂರ್ಣಾಙ್ಕ)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ಯದಿ (ap->group == i && ap->pat != ರಿಕ್ತ)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		in_use = TRUE;
		event = ಅಂಕೆ_EVENTS;
		ಭಙ್ಗ;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    ಯದಿ (in_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    ಅನ್ಯಥಾ
	AUGROUP_NAME(i) = ರಿಕ್ತ;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
au_find_group(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = 0; i < augroups.ga_len; ++i)
	ಯದಿ (AUGROUP_NAME(i) != ರಿಕ್ತ && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    ಪ್ರತ್ಯಾಗಮ i;
    ಪ್ರತ್ಯಾಗಮ AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    ಪೂರ್ಣಾಙ್ಕ
au_has_group(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ au_find_group(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    ಶೂನ್ಯ
do_augroup(char_u *arg, ಪೂರ್ಣಾಙ್ಕ del_group)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಯದಿ (del_group)
    {
	ಯದಿ (*arg == NUL)
	    emsg(_(e_argument_required));
	ಅನ್ಯಥಾ
	    au_del_group(arg);
    }
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    ಅನ್ಯಥಾ ಯದಿ (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_group(arg);
	ಯದಿ (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    ಅನ್ಯಥಾ			    // ":aug": list the group names
    {
	ಸಂದೇಶ_start();
	ಪ್ರತಿ (i = 0; i < augroups.ga_len; ++i)
	{
	    ಯದಿ (AUGROUP_NAME(i) != ರಿಕ್ತ)
	    {
		ಸಂದೇಶ_puts((ವರ್ಣ *)AUGROUP_NAME(i));
		ಸಂದೇಶ_puts("  ");
	    }
	}
	ಸಂದೇಶ_clr_eos();
	ಸಂದೇಶ_end();
    }
}

    ಶೂನ್ಯ
autocmd_init(ಶೂನ್ಯ)
{
    CLEAR_FIELD(aucmd_win);
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_all_autocmds(ಶೂನ್ಯ)
{
    char_u	*s;

    ಪ್ರತಿ (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(ರಿಕ್ತ, (char_u *)"", TRUE);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_data))[i];
	ಯದಿ (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#endif

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    ಪೂರ್ಣಾಙ್ಕ
is_aucmd_win(win_T *win)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < AUCMD_WIN_ಎಣಿಕೆ; ++i)
	ಯದಿ (aucmd_win[i].auc_win_used && aucmd_win[i].auc_win == win)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    ಸ್ಥಿರ event_T
event_name2nr(char_u *start, char_u **end)
{
    char_u	*p;
    keyvalue_T target;
    keyvalue_T *entry;
    ಸ್ಥಿರ keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_INDEX];
    ಸ್ಥಿರ keyvalue_T *bufread = &event_tab[BUFREAD_INDEX];

    // the event name ends with end of line, '|', a blank or a comma
    ಪ್ರತಿ (p = start; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    target.ಕೀಲಿ = 0;
    target.ಮೌಲ್ಯ.string = start;
    target.ಮೌಲ್ಯ.length = (size_t)(p - start);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    ಯದಿ (cmp_keyvalue_ಮೌಲ್ಯ_ni(&target, bufnewfile) == 0)
	entry = bufnewfile;
    ಅನ್ಯಥಾ ಯದಿ (cmp_keyvalue_ಮೌಲ್ಯ_ni(&target, bufread) == 0)
	entry = bufread;
    ಅನ್ಯಥಾ
	entry = (keyvalue_T *)bsearch(&target, &event_tab, ಅಂಕೆ_EVENTS,
				ಪರಿಮಾಣ(event_tab[0]), cmp_keyvalue_ಮೌಲ್ಯ_ni);

    ಯದಿ (*p == ',')
	++p;
    *end = p;

    ಪ್ರತ್ಯಾಗಮ (entry == ರಿಕ್ತ) ? ಅಂಕೆ_EVENTS : (event_T)abs(entry->ಕೀಲಿ);
}

/*
 * Return the name for event "event".
 */
    ಸ್ಥಿರ string_T *
event_nr2name(event_T event)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಗಣನಾ {CACHE_SIZE = 12};
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cache_tab[CACHE_SIZE];
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cache_last_index = -1;
    ಸ್ಥಿರ string_T unknown = STR_LITERAL_INIT("Unknown");

    ಯದಿ (cache_last_index < 0)
    {
	ಪ್ರತಿ (i = 0; i < CACHE_SIZE; ++i)
	    cache_tab[i] = -1;
	cache_last_index = CACHE_SIZE - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    ಪ್ರತಿ (i = cache_last_index; cache_tab[i] >= 0; )
    {
	ಯದಿ ((event_T)abs(event_tab[cache_tab[i]].ಕೀಲಿ) == event)
	    ಪ್ರತ್ಯಾಗಮ &event_tab[cache_tab[i]].ಮೌಲ್ಯ;

	ಯದಿ (i == 0)
	    i = CACHE_SIZE - 1;
	ಅನ್ಯಥಾ
	    --i;

	// are we back at the start?
	ಯದಿ (i == cache_last_index)
	    ಭಙ್ಗ;
    }

    // look in the event table itself
    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_EVENTS; ++i)
    {
	ಯದಿ ((event_T)abs(event_tab[i].ಕೀಲಿ) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    ಯದಿ (cache_last_index == CACHE_SIZE - 1)
		cache_last_index = 0;
	    ಅನ್ಯಥಾ
		++cache_last_index;
	    cache_tab[cache_last_index] = i;
	    ಪ್ರತ್ಯಾಗಮ &event_tab[i].ಮೌಲ್ಯ;
	}
    }

    ಪ್ರತ್ಯಾಗಮ &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    ಸ್ಥಿರ char_u *
find_end_event(
    char_u  *arg,
    ಪೂರ್ಣಾಙ್ಕ	    have_group)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    ಯದಿ (*arg == '*')
    {
	ಯದಿ (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_star_str), arg);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	pat = arg + 1;
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)event_name2nr(pat, &p) >= ಅಂಕೆ_EVENTS)
	    {
		ಯದಿ (have_group)
		    semsg(_(e_no_such_event_str), pat);
		ಅನ್ಯಥಾ
		    semsg(_(e_no_such_group_or_event_str), pat);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ pat;
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
    ಪೂರ್ಣಾಙ್ಕ		group;

    ಯದಿ (*arg == '|')
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    group = au_get_grouparg(&arg);
    pat = group == AUGROUP_ERROR
			  ? ರಿಕ್ತ : find_end_event(arg, group != AUGROUP_ALL);
    --emsg_off;
    ಯದಿ (pat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    pat = skipwhite(pat);
    ಯದಿ (*pat == NUL || *pat == '|')
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    ಯಾವತ್ (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 2; ++i)
    {
	ಯದಿ (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	ಯದಿ (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	ಯದಿ (!in_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    ಪ್ರತ್ಯಾಗಮ *cmd == NUL ? ರಿಕ್ತ : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    ಪೂರ್ಣಾಙ್ಕ
event_ignored(event_T event, char_u *ei)
{
    ಪೂರ್ಣಾಙ್ಕ ignored = FALSE;
    ಯಾವತ್ (*ei != NUL)
    {
	ಪೂರ್ಣಾಙ್ಕ unignore = *ei == '-';
	ei += unignore;
	ಯದಿ (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].ಕೀಲಿ <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	ಅನ್ಯಥಾ ಯದಿ (event_name2nr(ei, &ei) == event)
	{
	    ಯದಿ (unignore)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    ಅನ್ಯಥಾ
		ignored = TRUE;
	}
    }

    ಪ್ರತ್ಯಾಗಮ ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_ei(char_u *ei)
{
    ಪೂರ್ಣಾಙ್ಕ	win = ei != p_ei;

    ಯಾವತ್ (*ei)
    {
	ಯದಿ (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	ಅನ್ಯಥಾ
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    ಯದಿ (event == ಅಂಕೆ_EVENTS || (win && event_tab[event].ಕೀಲಿ > 0))
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(ವರ್ಣ *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    size_t	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    ಯದಿ (save_ei == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    ಯದಿ (new_ei == ರಿಕ್ತ)
    {
	vim_free(save_ei);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    ಅನ್ಯಥಾ
	STRCPY(new_ei + p_ei_len, what);
    set_string_option_direct((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    ಪ್ರತ್ಯಾಗಮ save_ei;
}

    ಶೂನ್ಯ
au_event_restore(char_u *old_ei)
{
    ಯದಿ (old_ei != ರಿಕ್ತ)
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
    ಶೂನ್ಯ
do_autocmd(exarg_T *eap, char_u *arg_in, ಪೂರ್ಣಾಙ್ಕ forceit)
{
    char_u	*arg = arg_in;
    char_u	*pat;
    char_u	*envpat = ರಿಕ್ತ;
    char_u	*cmd;
    ಪೂರ್ಣಾಙ್ಕ		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		nested = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		once = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		group;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;

    ಯದಿ (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	group = AUGROUP_ALL;	// no argument, use all groups
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	group = au_get_grouparg(&arg);
	ಯದಿ (arg == ರಿಕ್ತ)	    // out of memory
	    ಪ್ರತ್ಯಾಗಮ;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = find_end_event(arg, group != AUGROUP_ALL);
    ಯದಿ (pat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    pat = skipwhite(pat);
    ಯದಿ (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	ಯಾವತ್ (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	ಯದಿ (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	ಯದಿ (vim_strchr(pat, '$') != ರಿಕ್ತ || vim_strchr(pat, '~') != ರಿಕ್ತ)
	{
#ifdef BACKSLASH_IN_FILENAME
	    ಪೂರ್ಣಾಙ್ಕ	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#endif
	    envpat = expand_env_save(pat);
#ifdef BACKSLASH_IN_FILENAME
	    p_ssl = p_ssl_save;
#endif
	    ಯದಿ (envpat != ರಿಕ್ತ)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	ಪ್ರತಿ (i = 0; i < 2; i++)
	{
	    ಯದಿ (*cmd == NUL)
		ಅನುವರ್ತ;

	    // Check for "++once" flag.
	    ಯದಿ (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ಯದಿ (once)
		{
		    semsg(_(e_duplicate_argument_str), "++once");
		    ಗಚ್ಛ err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    ಯದಿ ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		ಯದಿ (nested)
		{
		    semsg(_(e_duplicate_argument_str), "++nested");
		    ಗಚ್ಛ err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    ಯದಿ (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ಯದಿ (in_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_mean_plusplus_nested));
		    ಗಚ್ಛ err_exit;
		}
		ಯದಿ (nested)
		{
		    semsg(_(e_duplicate_argument_str), "nested");
		    ಗಚ್ಛ err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 6);
	    }
	}

	/*
	 * Find the start of the commands.
	 * Expand <sfile> in it.
	 */
	ಯದಿ (*cmd != NUL)
	{
	    ಯದಿ (eap != ರಿಕ್ತ)
		// Read a {} block if it follows.
		cmd = may_get_cmd_block(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    ಯದಿ (cmd == ರಿಕ್ತ)	    // some error
		ಗಚ್ಛ err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    ಯದಿ (!forceit && *cmd == NUL)
	// Highlight title
	ಸಂದೇಶ_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    last_event = (event_T)-1;		// for listing the event name
    last_group = AUGROUP_ERROR;		// for listing the group name
    ಯದಿ (*arg == '*' || *arg == NUL || *arg == '|')
    {
	ಯದಿ (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	ಅನ್ಯಥಾ
	    ಪ್ರತಿ (event = (event_T)0; (ಪೂರ್ಣಾಙ್ಕ)event < ಅಂಕೆ_EVENTS;
					     event = (event_T)((ಪೂರ್ಣಾಙ್ಕ)event + 1))
		ಯದಿ (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, group, flags) == FAIL)
		    ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ
    {
	ಯಾವತ್ (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    ಯದಿ (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, group, flags) == FAIL)
		ಭಙ್ಗ;
    }

err_exit:
    ಯದಿ (cmd_need_free)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
au_get_grouparg(char_u **argp)
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    ಪೂರ್ಣಾಙ್ಕ		group = AUGROUP_ALL;

    ಪ್ರತಿ (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    ಯದಿ (p <= arg)
	ಪ್ರತ್ಯಾಗಮ AUGROUP_ALL;

    group_name = vim_strnsave(arg, p - arg);
    ಯದಿ (group_name == ರಿಕ್ತ)		// out of memory
	ಪ್ರತ್ಯಾಗಮ AUGROUP_ERROR;
    group = au_find_group(group_name);
    ಯದಿ (group == AUGROUP_ERROR)
	group = AUGROUP_ALL;	// no match, use all groups
    ಅನ್ಯಥಾ
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(group_name);
    ಪ್ರತ್ಯಾಗಮ group;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    ಪೂರ್ಣಾಙ್ಕ		once,
    ಪೂರ್ಣಾಙ್ಕ		nested,
    char_u	*cmd,
    ಪೂರ್ಣಾಙ್ಕ		forceit,
    ಪೂರ್ಣಾಙ್ಕ		group,
    ಪೂರ್ಣಾಙ್ಕ		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    ಪೂರ್ಣಾಙ್ಕ		brace_level;
    char_u	*endpat;
    ಪೂರ್ಣಾಙ್ಕ		findgroup;
    ಪೂರ್ಣಾಙ್ಕ		allgroups;
    ಪೂರ್ಣಾಙ್ಕ		patlen;
    ಪೂರ್ಣಾಙ್ಕ		is_buflocal;
    ಪೂರ್ಣಾಙ್ಕ		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    ಯದಿ (group == AUGROUP_ALL)
	findgroup = current_augroup;
    ಅನ್ಯಥಾ
	findgroup = group;
    allgroups = (group == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    ಯದಿ (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    ಯದಿ (forceit)  // delete the AutoPat, if it's in the current group
	    {
		ಯದಿ (ap->group == findgroup)
		    au_remove_pat(ap);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    ಪ್ರತಿ ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	ಪ್ರತಿ (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    ಯದಿ (*endpat == '{')
		brace_level++;
	    ಅನ್ಯಥಾ ಯದಿ (*endpat == '}')
		brace_level--;
	}
	ಯದಿ (pat == endpat)		// ignore single comma
	    ಅನುವರ್ತ;
	patlen = (ಪೂರ್ಣಾಙ್ಕ)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	ಯದಿ (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    ಯದಿ (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    ಅನ್ಯಥಾ ಯದಿ (patlen > 9 && pat[7] == '=')
	    {
		ಯದಿ (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		ಅನ್ಯಥಾ ಯದಿ (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((ವರ್ಣ *)pat + 8);
	    }
	}

	ಯದಿ (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((ವರ್ಣ *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	ಯದಿ (!forceit && *cmd != NUL && last_autopat[(ಪೂರ್ಣಾಙ್ಕ)event] != ರಿಕ್ತ)
	    prev_ap = &last_autopat[(ಪೂರ್ಣಾಙ್ಕ)event];
	ಅನ್ಯಥಾ
	    prev_ap = &first_autopat[(ಪೂರ್ಣಾಙ್ಕ)event];
	ಯಾವತ್ ((ap = *prev_ap) != ರಿಕ್ತ)
	{
	    ಯದಿ (ap->pat != ರಿಕ್ತ)
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
		ಯದಿ ((allgroups || ap->group == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    ಯದಿ (forceit)
		    {
			ಯದಿ (*cmd != NUL && ap->next == ರಿಕ್ತ)
			{
			    au_remove_cmds(ap);
			    ಭಙ್ಗ;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    ಅನ್ಯಥಾ ಯದಿ (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    ಅನ್ಯಥಾ ಯದಿ (ap->next == ರಿಕ್ತ)
			ಭಙ್ಗ;
		}
	    }
	    prev_ap = &ap->next;
	}

	/*
	 * Add a new command.
	 */
	ಯದಿ (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    ಯದಿ (ap == ರಿಕ್ತ)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		ಯದಿ (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == ರಿಕ್ತ))
		{
		    semsg(_(e_buffer_nr_invalid_buffer_ಸಂಖ್ಯೆ), buflocal_nr);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		ಯದಿ (ap == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		ಯದಿ (ap->pat == ರಿಕ್ತ)
		{
		    vim_free(ap);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}

#ifdef FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		ಯದಿ (event == EVENT_MODECHANGED && !has_modechanged())
		    get_mode(last_mode);
#endif
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		ಯದಿ ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
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

		ಯದಿ (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = ರಿಕ್ತ;
		}
		ಅನ್ಯಥಾ
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_to_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    ಯದಿ (reg_pat != ರಿಕ್ತ)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    ಯದಿ (reg_pat == ರಿಕ್ತ || ap->reg_prog == ರಿಕ್ತ)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		}
		ap->cmds = ರಿಕ್ತ;
		*prev_ap = ap;
		last_autopat[(ಪೂರ್ಣಾಙ್ಕ)event] = ap;
		ap->next = ರಿಕ್ತ;
		ಯದಿ (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		ಅನ್ಯಥಾ
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    ಯಾವತ್ ((ac = *prev_ac) != ರಿಕ್ತ)
		prev_ac = &ac->next;
	    ac = ALLOC_ONE(AutoCmd);
	    ಯದಿ (ac == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    ಯದಿ (flags & UC_VIM9)
		ac->script_ctx.sc_version = SCRIPT_VERSION_VIM9;
#ifdef FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#endif
	    ಯದಿ (ac->cmd == ರಿಕ್ತ)
	    {
		vim_free(ac);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ac->next = ರಿಕ್ತ;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    ಪೂರ್ಣಾಙ್ಕ
do_doautocmd(
    char_u	*arg_start,
    ಪೂರ್ಣಾಙ್ಕ		do_ಸಂದೇಶ,	    // give message for no matching autocmds?
    ಪೂರ್ಣಾಙ್ಕ		*did_something)
{
    char_u	*arg = arg_start;
    char_u	*fname;
    ಪೂರ್ಣಾಙ್ಕ		nothing_done = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		group;

    ಯದಿ (did_something != ರಿಕ್ತ)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    ಯದಿ (arg == ರಿಕ್ತ)	    // out of memory
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg, group != AUGROUP_ALL);
    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    ಯಾವತ್ (*arg && !ends_excmd(*arg) && !VIM_ISWHITE(*arg))
	ಯದಿ (apply_autocmds_group(event_name2nr(arg, &arg),
				      fname, ರಿಕ್ತ, TRUE, group, curbuf, ರಿಕ್ತ))
	    nothing_done = FALSE;

    ಯದಿ (nothing_done && do_ಸಂದೇಶ
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       )
	smsg(_("No matching autocommands: %s"), arg_start);
    ಯದಿ (did_something != ರಿಕ್ತ)
	*did_something = !nothing_done;

#ifdef FEAT_EVAL
    ಪ್ರತ್ಯಾಗಮ aborting() ? FAIL : OK;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ OK;
#endif
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    ಶೂನ್ಯ
ex_doautoall(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಙ್ಕ		call_do_modelines = check_nomodeline(&arg);
    ಪೂರ್ಣಾಙ್ಕ		did_aucmd;

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
	ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ || buf == curbuf)
	    ಅನುವರ್ತ;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	ಯದಿ (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    ಭಙ್ಗ;
	}

	set_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	ಯದಿ (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	ಯದಿ (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    ಭಙ್ಗ;
	}
    }

    // Execute autocommands for the current buffer last.
    ಯದಿ (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	ಯದಿ (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_nomodeline(char_u **argp)
{
    ಯದಿ (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Prepare for executing autocommands for (hidden) buffer "buf".
 * Search for a visible window containing the current buffer.  If there isn't
 * one then use an entry in "aucmd_win[]".
 * Set "curbuf" and "curwin" to match "buf".
 * When this fails "curbuf" is not equal "buf".
 */
    ಶೂನ್ಯ
aucmd_prepbuf(
    aco_save_T	*aco,		// structure to save values in
    buf_T	*buf)		// new curbuf
{
    win_T	*win;
    ಪೂರ್ಣಾಙ್ಕ		save_ea;
#ifdef FEAT_AUTOCHDIR
    ಪೂರ್ಣಾಙ್ಕ		save_acd;
#endif
    ಪೂರ್ಣಾಙ್ಕ		same_buffer = buf == curbuf;

    // Find a window that is for the new buffer
    ಯದಿ (same_buffer)		// be quick when buf is curbuf
	win = curwin;
    ಅನ್ಯಥಾ
	FOR_ALL_WINDOWS(win)
	    ಯದಿ (win->w_buffer == buf)
		ಭಙ್ಗ;

    // Allocate a window when needed.
    win_T *auc_win = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ auc_idx = AUCMD_WIN_ಎಣಿಕೆ;
    ಯದಿ (win == ರಿಕ್ತ)
    {
	ಪ್ರತಿ (auc_idx = 0; auc_idx < AUCMD_WIN_ಎಣಿಕೆ; ++auc_idx)
	    ಯದಿ (!aucmd_win[auc_idx].auc_win_used)
	    {
		ಯದಿ (aucmd_win[auc_idx].auc_win == ರಿಕ್ತ)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		ಯದಿ (auc_win != ರಿಕ್ತ)
		    aucmd_win[auc_idx].auc_win_used = TRUE;
		ಭಙ್ಗ;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	ಯದಿ (auc_win == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == ರಿಕ್ತ ? 0 : prevwin->w_id;
#ifdef FEAT_JOB_CHANNEL
    ಯದಿ (bt_prompt(curbuf))
	aco->save_prompt_ಸೇರಿಸು = curbuf->b_prompt_ಸೇರಿಸು;
#endif

    ಯದಿ (win != ರಿಕ್ತ)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    ಅನ್ಯಥಾ
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
	curtab->tp_localdir = ರಿಕ್ತ;
	aco->globaldir = globaldir;
	globaldir = ರಿಕ್ತ;

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

	(ಶೂನ್ಯ)win_split_ins(0, WSP_ತುದಿ | WSP_FORCE_ROOM, auc_win, 0, ರಿಕ್ತ);
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
    ಯದಿ (!same_buffer)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_active = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    ಶೂನ್ಯ
aucmd_restbuf(
    aco_save_T	*aco)		// structure holding saved values
{
    ಪೂರ್ಣಾಙ್ಕ	    dummy;
    win_T   *save_curwin;

    ಯದಿ (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	block_autocmds();
	ಯದಿ (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		ಯದಿ (wp == awp)
		{
		    ಯದಿ (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(awp);
		    ಗಚ್ಛ win_found;
		}
	    }
	}
win_found:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(ಶೂನ್ಯ)winframe_remove(curwin, &dummy, ರಿಕ್ತ, ರಿಕ್ತ);
	win_remove(curwin, ರಿಕ್ತ);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_used = FALSE;
	last_status(FALSE);	    // may need to remove last status line

	ಯದಿ (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_find_by_id(aco->save_curwin_id);
	ಯದಿ (save_curwin != ರಿಕ್ತ)
	    curwin = save_curwin;
	ಅನ್ಯಥಾ
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_window(curwin);
	ಯದಿ (bt_prompt(curbuf))
	    curbuf->b_prompt_ಸೇರಿಸು = aco->save_prompt_ಸೇರಿಸು;
#endif
	prevwin = win_find_by_id(aco->save_prevwin_id);
#ifdef FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#endif
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	ಯದಿ (awp->w_localdir != ರಿಕ್ತ)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_active = aco->save_VIsual_active;
	check_cursor();
	ಯದಿ (curwin->w_topline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    curwin->w_topline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
#ifdef FEAT_DIFF
	    curwin->w_topfill = 0;
#endif
	}
#ಯದಿ defined(FEAT_GUI)
	ಯದಿ (gui.in_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_ಎಡ], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_ಬಲ], FALSE);
	    gui_may_update_scrollbars();
	}
#endif
    }
    ಅನ್ಯಥಾ
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_find_by_id(aco->save_curwin_id);
	ಯದಿ (save_curwin != ರಿಕ್ತ)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    ಯದಿ (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != ರಿಕ್ತ)
	    {
#ಯದಿ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		ಯದಿ (curwin->w_s == &curbuf->b_s)
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
    ಯದಿ (VIsual_active)
	check_pos(curbuf, &VIsual);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    ಪೂರ್ಣಾಙ್ಕ
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    ಪೂರ್ಣಾಙ್ಕ		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    ಪ್ರತ್ಯಾಗಮ apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ರಿಕ್ತ);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    ಪೂರ್ಣಾಙ್ಕ
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    ಪೂರ್ಣಾಙ್ಕ		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    ಪ್ರತ್ಯಾಗಮ apply_autocmds_group(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    ಪೂರ್ಣಾಙ್ಕ		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    ಪೂರ್ಣಾಙ್ಕ		*retval)    // pointer to caller's retval
{
    ಪೂರ್ಣಾಙ್ಕ		did_cmd;

#ifdef FEAT_EVAL
    ಯದಿ (should_abort(*retval))
	ಪ್ರತ್ಯಾಗಮ FALSE;
#endif

    did_cmd = apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ರಿಕ್ತ);
    ಯದಿ (did_cmd
#ifdef FEAT_EVAL
	    && aborting()
#endif
	    )
	*retval = FAIL;
    ಪ್ರತ್ಯಾಗಮ did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
has_cursorhold(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)(get_real_state() == MODE_NORMAL_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != ರಿಕ್ತ);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    ಪೂರ್ಣಾಙ್ಕ
trigger_cursorhold(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		state;

    ಯದಿ (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_active())
    {
	state = get_real_state();
	ಯದಿ (state == MODE_NORMAL_BUSY || (state & MODE_ಸೇರಿಸು) != 0)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_winresized(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_WINRESIZED] != ರಿಕ್ತ);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_winscrolled(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_WINSCROLLED] != ರಿಕ್ತ);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_cursormoved(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_CURSORMOVED] != ರಿಕ್ತ);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_cursormovedI(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_CURSORMOVEDI] != ರಿಕ್ತ);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_textchanged(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_TEXTCHANGED] != ರಿಕ್ತ);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_textchangedI(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_TEXTCHANGEDI] != ರಿಕ್ತ);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_textchangedP(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_TEXTCHANGEDP] != ರಿಕ್ತ);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_insertcharpre(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_INSERTCHARPRE] != ರಿಕ್ತ);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_keyinputpre(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_KEYINPUTPRE] != ರಿಕ್ತ);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_cmdundefined(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_CMDUNDEFINED] != ರಿಕ್ತ);
}

#ಯದಿ defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_textputpost(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_TEXTPUTPOST] != ರಿಕ್ತ);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_textputpre(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_TEXTPUTPRE] != ರಿಕ್ತ);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_textyankpost(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_TEXTYANKPOST] != ರಿಕ್ತ);
}
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_completechanged(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_COMPLETECHANGED] != ರಿಕ್ತ);
}
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_modechanged(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_MODECHANGED] != ರಿಕ್ತ);
}
#endif

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
apply_autocmds_group(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    ಪೂರ್ಣಾಙ್ಕ		force,	     // when TRUE, ignore autocmd_busy
    ಪೂರ್ಣಾಙ್ಕ		group,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap UNUSED) // command arguments
{
    char_u	*sfname = ರಿಕ್ತ;	// short file name
    char_u	*tail;
    ಪೂರ್ಣಾಙ್ಕ		save_changed;
    buf_T	*old_curbuf;
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;
    char_u	*save_autocmd_fname;
    ಪೂರ್ಣಾಙ್ಕ		save_autocmd_fname_full;
    ಪೂರ್ಣಾಙ್ಕ		save_autocmd_bufnr;
    char_u	*save_autocmd_match;
    ಪೂರ್ಣಾಙ್ಕ		save_autocmd_busy;
    ಪೂರ್ಣಾಙ್ಕ		save_autocmd_nested;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ifdef FEAT_EVAL
    funccal_entry_T funccal_entry;
    char_u	*save_cmdarg;
    long	save_cmdbang;
#endif
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	filechangeshell_busy = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif
    ಪೂರ್ಣಾಙ್ಕ		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    ಪೂರ್ಣಾಙ್ಕ		save_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    ಯದಿ (event == ಅಂಕೆ_EVENTS || first_autopat[(ಪೂರ್ಣಾಙ್ಕ)event] == ರಿಕ್ತ
	    || autocmd_blocked > 0)
	ಗಚ್ಛ BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    ಯದಿ (autocmd_busy && !(force || autocmd_nested))
	ಗಚ್ಛ BYPASS_AU;

#ifdef FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    ಯದಿ (aborting())
	ಗಚ್ಛ BYPASS_AU;
#endif

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    ಯದಿ (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	ಗಚ್ಛ BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    ಯದಿ (event_ignored(event, p_ei))
	ಗಚ್ಛ BYPASS_AU;

    ಪೂರ್ಣಾಙ್ಕ win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    ಯದಿ (buf == curbuf && event_tab[event].ಕೀಲಿ <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    ಅನ್ಯಥಾ ಯದಿ (buf != ರಿಕ್ತ && event_tab[event].ಕೀಲಿ <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಯದಿ (wp->w_buffer == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		ಭಙ್ಗ;
	    }
    }
    ಯದಿ (win_ignore)
	ಗಚ್ಛ BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    ಯದಿ (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	ಗಚ್ಛ BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    ಯದಿ (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	ಗಚ್ಛ BYPASS_AU;

    ಯದಿ (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_changed_ಎಣಿಕೆ;

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
    ಯದಿ (fname_io == ರಿಕ್ತ)
    {
	ಯದಿ (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = ರಿಕ್ತ;
	ಅನ್ಯಥಾ ಯದಿ (fname != ರಿಕ್ತ && !ends_excmd(*fname))
	    autocmd_fname = fname;
	ಅನ್ಯಥಾ ಯದಿ (buf != ರಿಕ್ತ)
	    autocmd_fname = buf->b_ffname;
	ಅನ್ಯಥಾ
	    autocmd_fname = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
	autocmd_fname = fname_io;
    ಯದಿ (autocmd_fname != ರಿಕ್ತ)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_full = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    ಯದಿ (buf == ರಿಕ್ತ)
	autocmd_bufnr = 0;
    ಅನ್ಯಥಾ
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    ಯದಿ (fname == ರಿಕ್ತ || *fname == NUL)
    {
	ಯದಿ (buf == ರಿಕ್ತ)
	    fname = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	{
#ifdef FEAT_SYN_HL
	    ಯದಿ (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    ಅನ್ಯಥಾ
#endif
		ಯದಿ (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (buf->b_sfname != ರಿಕ್ತ)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	ಯದಿ (fname == ರಿಕ್ತ)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    ಅನ್ಯಥಾ
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	ಯದಿ (event == EVENT_FILETYPE
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
	ಅನ್ಯಥಾ
	    fname = FullName_save(fname, FALSE);
    }
    ಯದಿ (fname == ರಿಕ್ತ)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	ಗಚ್ಛ BYPASS_AU;
    }

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    ಯದಿ (sfname != ರಿಕ್ತ)
	forward_slash(sfname);
    forward_slash(fname);
#endif

#ifdef VMS
    // remove version for correct match
    ಯದಿ (sfname != ರಿಕ್ತ)
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
    estack_ತಳ್ಳು(ETYPE_AUCMD, ರಿಕ್ತ, 0);
    ESTACK_CHECK_SETUP;

    save_current_sctx = current_sctx;

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    ಯದಿ (do_profiling == PROF_YES)
	prof_child_enter(&wait_time); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_entry);
#endif

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    ಯದಿ (!autocmd_busy)
    {
	save_ಹುಡುಕು_patterns();
	ಯದಿ (!ins_compl_active())
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
    ಯದಿ (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    tail = gettail(fname);

    // Find first autocommand that matches
    CLEAR_FIELD(patcmd);
    patcmd.curpat = first_autopat[(ಪೂರ್ಣಾಙ್ಕ)event];
    patcmd.group = group;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.tail = tail;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_next_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    ಯದಿ (patcmd.curpat != ರಿಕ್ತ)
    {
	// add to active_apc_list
	patcmd.next = active_apc_list;
	active_apc_list = &patcmd;

#ifdef FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (long)get_vim_var_nr(VV_CMDBANG);
	ಯದಿ (eap != ರಿಕ್ತ)
	{
	    save_cmdarg = set_cmdarg(eap, ರಿಕ್ತ);
	    set_vim_var_nr(VV_CMDBANG, (long)eap->forceit);
	}
	ಅನ್ಯಥಾ
	    save_cmdarg = ರಿಕ್ತ;	// avoid gcc warning
#endif
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	ಪ್ರತಿ (ap = patcmd.curpat; ap->next != ರಿಕ್ತ; ap = ap->next)
	    ap->last = FALSE;
	ap->last = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	ಯದಿ (nesting == 1)
	    check_lnums(TRUE);
	ಅನ್ಯಥಾ
	    check_lnums_nested(TRUE);

	ಪೂರ್ಣಾಙ್ಕ save_did_emsg = did_emsg;
	ಪೂರ್ಣಾಙ್ಕ save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(ರಿಕ್ತ, getnextac, (ಶೂನ್ಯ *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	set_pressedreturn(save_ex_pressedreturn);

	ಯದಿ (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ifdef FEAT_EVAL
	ಯದಿ (eap != ರಿಕ್ತ)
	{
	    (ಶೂನ್ಯ)set_cmdarg(ರಿಕ್ತ, save_cmdarg);
	    set_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#endif
	// delete from active_apc_list
	ಯದಿ (active_apc_list == &patcmd)	    // just in case
	    active_apc_list = patcmd.next;
    }

    ಯದಿ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_ತೆಗೆ();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_full = save_autocmd_fname_full;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_match = save_autocmd_match;
    current_sctx = save_current_sctx;
#ifdef FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    ಯದಿ (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif
    ಕೀಲಿ_Typed = save_ಕೀಲಿ_Typed;
    vim_free(fname);
    vim_free(sfname);
    --nesting;		// see matching increment above

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.  Free any buffers in the au_pending_free_buf list and
     * free any windows in the au_pending_free_win list.
     */
    ಯದಿ (!autocmd_busy)
    {
	restore_ಹುಡುಕು_patterns();
	ಯದಿ (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	ಯಾವತ್ (au_pending_free_buf != ರಿಕ್ತ)
	{
	    buf_T *b = au_pending_free_buf->b_next;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	ಯಾವತ್ (au_pending_free_win != ರಿಕ್ತ)
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
    ಯದಿ (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	ಯದಿ (curbuf->b_changed != save_changed)
	    need_maketitle = TRUE;
	curbuf->b_changed = save_changed;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    ಯದಿ (event == EVENT_BUFWIPEOUT && buf != ರಿಕ್ತ)
	aubuflocal_remove(buf);

    ಯದಿ (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    ಪ್ರತ್ಯಾಗಮ retval;
}

#ifdef FEAT_EVAL
ಸ್ಥಿರ char_u	*old_termresponse = ರಿಕ್ತ;
ಸ್ಥಿರ char_u	*old_termu7resp = ರಿಕ್ತ;
ಸ್ಥಿರ char_u	*old_termblinkresp = ರಿಕ್ತ;
ಸ್ಥಿರ char_u	*old_termrbgresp = ರಿಕ್ತ;
ಸ್ಥಿರ char_u	*old_termrfgresp = ರಿಕ್ತ;
ಸ್ಥಿರ char_u	*old_termstyleresp = ರಿಕ್ತ;
#endif

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    ಶೂನ್ಯ
block_autocmds(ಶೂನ್ಯ)
{
#ifdef FEAT_EVAL
    // Remember the value of v:termresponse.
    ಯದಿ (autocmd_blocked == 0)
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

    ಶೂನ್ಯ
unblock_autocmds(ಶೂನ್ಯ)
{
    --autocmd_blocked;

#ifdef FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    ಯದಿ (autocmd_blocked == 0)
    {
	ಯದಿ (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", ರಿಕ್ತ, FALSE, curbuf);
	}
	ಯದಿ (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", ರಿಕ್ತ, FALSE, curbuf);
	}
	ಯದಿ (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", ರಿಕ್ತ, FALSE, curbuf);
	}
	ಯದಿ (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", ರಿಕ್ತ, FALSE, curbuf);
	}
	ಯದಿ (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", ರಿಕ್ತ, FALSE, curbuf);
	}
	ಯದಿ (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", ರಿಕ್ತ, FALSE, curbuf);
	}
    }
#endif
}

    ಪೂರ್ಣಾಙ್ಕ
is_autocmd_blocked(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
auto_next_pat(
    AutoPatCmd_T *apc,
    ಪೂರ್ಣಾಙ್ಕ		stop_at_last)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*entry;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(entry->es_name);
    entry->es_info.aucmd = ರಿಕ್ತ;

    ಪ್ರತಿ (ap = apc->curpat; ap != ರಿಕ್ತ && !got_int; ap = ap->next)
    {
	apc->curpat = ರಿಕ್ತ;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	ಯದಿ (ap->pat != ರಿಕ್ತ && ap->cmds != ರಿಕ್ತ
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
	    // execution-condition
	    ಯದಿ (ap->buflocal_nr == 0
		    ? (match_file_pat(ರಿಕ್ತ, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->tail, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		ವರ್ಣ	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->length + ap->patlen + 1);
		ಯದಿ (namep != ರಿಕ್ತ)
		{
		    sprintf((ವರ್ಣ *)namep, fmt, (ವರ್ಣ *)event_name->string, (ವರ್ಣ *)ap->pat);
		    ಯದಿ (p_verbose >= 8)
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
		ಪ್ರತಿ (cp = ap->cmds; cp->next != ರಿಕ್ತ; cp = cp->next)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    ಸಾಲು_breakcheck();
	    ಯದಿ (apc->curpat != ರಿಕ್ತ)	    // found a match
		ಭಙ್ಗ;
	}
	ಯದಿ (stop_at_last && ap->last)
	    ಭಙ್ಗ;
    }
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    ಪ್ರತ್ಯಾಗಮ &acp->script_ctx;
}
#endif

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	ಪೂರ್ಣಾಙ್ಕ c UNUSED,
	ಶೂನ್ಯ *cookie,
	ಪೂರ್ಣಾಙ್ಕ indent UNUSED,
	getline_opt_T options UNUSED)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    ಯದಿ (acp->curpat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // repeat until we find an autocommand to execute
    ಪ್ರತಿ (;;)
    {
	// skip removed commands
	ಯಾವತ್ (acp->nextcmd != ರಿಕ್ತ && acp->nextcmd->cmd == ರಿಕ್ತ)
	    ಯದಿ (acp->nextcmd->last)
		acp->nextcmd = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
		acp->nextcmd = acp->nextcmd->next;

	ಯದಿ (acp->nextcmd != ರಿಕ್ತ)
	    ಭಙ್ಗ;

	// at end of commands, find next pattern that matches
	ಯದಿ (acp->curpat->last)
	    acp->curpat = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    acp->curpat = acp->curpat->next;
	ಯದಿ (acp->curpat != ರಿಕ್ತ)
	    auto_next_pat(acp, TRUE);
	ಯದಿ (acp->curpat == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ac = acp->nextcmd;

    ಯದಿ (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	ಸಂದೇಶ_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    ಯದಿ (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    ಯದಿ (ac->last)
	acp->nextcmd = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	acp->nextcmd = ac->next;
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_full)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*tail = gettail(sfname);
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;

    ಯದಿ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)event] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    fname = is_full ? vim_strsave(sfname) : FullName_save(sfname, FALSE);
    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    ಯದಿ (sfname != ರಿಕ್ತ)
	forward_slash(sfname);
    forward_slash(fname);
#endif

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	ಯದಿ (ap->pat != ರಿಕ್ತ && ap->cmds != ರಿಕ್ತ
	      && (ap->buflocal_nr == 0
		? match_file_pat(ರಿಕ್ತ, &ap->reg_prog,
					  fname, sfname, tail, ap->allow_dirs)
		: buf != ರಿಕ್ತ && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    ಭಙ್ಗ;
	}

    vim_free(fname);
#ifdef BACKSLASH_IN_FILENAME
    vim_free(sfname);
#endif

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx == augroups.ga_len)		// add "END" add the end
	ಪ್ರತ್ಯಾಗಮ (char_u *)"END";
    ಯದಿ (idx < 0 || idx >= augroups.ga_len)	// end of list
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (AUGROUP_NAME(idx) == ರಿಕ್ತ || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	ಪ್ರತ್ಯಾಗಮ (char_u *)"";
    ಪ್ರತ್ಯಾಗಮ AUGROUP_NAME(idx);		// return a name
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ include_groups = FALSE;

    char_u  *
set_context_in_autocmd(
    expand_T	*xp,
    char_u	*arg,
    ಪೂರ್ಣಾಙ್ಕ		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		group;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    group = au_get_grouparg(&arg);
    ಯದಿ (group == AUGROUP_ERROR)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    // If there only is a group name that's what we expand.
    ಯದಿ (*arg == NUL && group != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	group = AUGROUP_ALL;
    }

    // skip over event name
    ಪ್ರತಿ (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	ಯದಿ (*p == ',')
	    arg = p + 1;
    ಯದಿ (*p == NUL)
    {
	ಯದಿ (group == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_pattern = arg;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // skip over pattern
    arg = skipwhite(p);
    ಯಾವತ್ (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    ಯದಿ (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 2; i++)
	{
	    ಯದಿ (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    ಯದಿ (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    ಯದಿ (!in_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	ಪ್ರತ್ಯಾಗಮ arg;			    // expand (next) command
    }

    ಯದಿ (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    ಅನ್ಯಥಾ
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಯದಿ (idx < augroups.ga_len)		// First list group names, if wanted
    {
	ಯದಿ (!include_groups || AUGROUP_NAME(idx) == ರಿಕ್ತ
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    ಪ್ರತ್ಯಾಗಮ (char_u *)"";	// skip deleted entries
	ಪ್ರತ್ಯಾಗಮ AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    ಯದಿ (i < 0 || i >= ಅಂಕೆ_EVENTS)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ event_tab[i].ಮೌಲ್ಯ.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_group(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ win)
{
    ಯದಿ (idx < 0 || idx >= ಅಂಕೆ_EVENTS)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (!win)
	ಪ್ರತ್ಯಾಗಮ event_tab[idx].ಮೌಲ್ಯ.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    ಪೂರ್ಣಾಙ್ಕ j = 0;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_EVENTS; ++i)
    {
	j += event_tab[i].ಕೀಲಿ <= 0;
	ಯದಿ (j == idx + 1)
	    ಪ್ರತ್ಯಾಗಮ event_tab[i].ಮೌಲ್ಯ.string;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_tabclosedpre(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (first_autopat[(ಪೂರ್ಣಾಙ್ಕ)EVENT_TABCLOSEDPRE] != ರಿಕ್ತ);
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    ಪೂರ್ಣಾಙ್ಕ
autocmd_supported(char_u *name)
{
    char_u *p;

    ಪ್ರತ್ಯಾಗಮ (event_name2nr(name, &p) != ಅಂಕೆ_EVENTS);
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
    ಪೂರ್ಣಾಙ್ಕ
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*pattern = ರಿಕ್ತ;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		group;
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    ಯದಿ (arg_save == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    p = vim_strchr(arg_save, '#');
    ಯದಿ (p != ರಿಕ್ತ)
	*p++ = NUL;

    // First, look for an autocmd group name
    group = au_find_group(arg_save);
    ಯದಿ (group == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	group = AUGROUP_ALL;
	event_name = arg_save;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    ಗಚ್ಛ theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	ಯದಿ (p != ರಿಕ್ತ)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    pattern = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    ಯದಿ (event == ಅಂಕೆ_EVENTS)
	ಗಚ್ಛ theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = first_autopat[(ಪೂರ್ಣಾಙ್ಕ)event];
    ಯದಿ (ap == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    ಯದಿ (pattern != ರಿಕ್ತ && STRICMP(pattern, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    ಪ್ರತಿ ( ; ap != ರಿಕ್ತ; ap = ap->next)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	ಯದಿ (ap->pat != ರಿಕ್ತ && ap->cmds != ರಿಕ್ತ
	    && (group == AUGROUP_ALL || ap->group == group)
	    && (pattern == ರಿಕ್ತ
		|| (buflocal_buf == ರಿಕ್ತ
		    ? fnamecmp(ap->pat, pattern) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    ಭಙ್ಗ;
	}

theend:
    vim_free(arg_save);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
autocmd_add_or_delete(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = ರಿಕ್ತ;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*group_name = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		group;
    char_u	*pat = ರಿಕ್ತ;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = ರಿಕ್ತ;
    char_u	*end;
    ಪೂರ್ಣಾಙ್ಕ		once;
    ಪೂರ್ಣಾಙ್ಕ		nested;
    ಪೂರ್ಣಾಙ್ಕ		replace;		// replace the cmd for a group/event
    ಪೂರ್ಣಾಙ್ಕ		retval = VVAL_TRUE;
    ಪೂರ್ಣಾಙ್ಕ		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_list_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    aucmd_list = argvars[0].vval.v_list;
    ಯದಿ (aucmd_list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(aucmd_list, li)
    {
	VIM_CLEAR(group_name);
	VIM_CLEAR(cmd);
	event_name = ರಿಕ್ತ;
	event_list = ರಿಕ್ತ;
	pat = ರಿಕ್ತ;
	pat_list = ರಿಕ್ತ;

	ಯದಿ (li->li_tv.v_type != VAR_DICT)
	    ಅನುವರ್ತ;

	event_dict = li->li_tv.vval.v_dict;
	ಯದಿ (event_dict == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	di = dict_find(event_dict, (char_u *)"event", -1);
	ಯದಿ (di != ರಿಕ್ತ)
	{
	    ಯದಿ (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		ಯದಿ (event_name == ರಿಕ್ತ)
		{
		    emsg(_(e_string_required));
		    ಅನುವರ್ತ;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		ಯದಿ (event_list == ರಿಕ್ತ)
		{
		    emsg(_(e_list_required));
		    ಅನುವರ್ತ;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		emsg(_(e_string_or_list_expected));
		ಅನುವರ್ತ;
	    }
	}

	group_name = dict_get_string(event_dict, "group", TRUE);
	ಯದಿ (group_name == ರಿಕ್ತ || *group_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    group = current_augroup;
	ಅನ್ಯಥಾ
	{
	    group = au_find_group(group_name);
	    ಯದಿ (group == AUGROUP_ERROR)
	    {
		ಯದಿ (delete)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    ಭಙ್ಗ;
		}
		// group is not found, create it now
		group = au_new_group(group_name);
		ಯದಿ (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    ಭಙ್ಗ;
		}

		current_augroup = group;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	ಯದಿ (dict_has_ಕೀಲಿ(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_ಸಂಖ್ಯೆ_def(event_dict, "bufnr", -1);
	    ಯದಿ (bnum == -1)
		ಅನುವರ್ತ;

	    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "<buffer=%d>", (ಪೂರ್ಣಾಙ್ಕ)bnum);
	    pat = IObuff;
	}
	ಅನ್ಯಥಾ
	{
	    di = dict_find(event_dict, (char_u *)"pattern", -1);
	    ಯದಿ (di != ರಿಕ್ತ)
	    {
		ಯದಿ (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    ಯದಿ (pat == ರಿಕ್ತ)
		    {
			emsg(_(e_string_required));
			ಅನುವರ್ತ;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    ಯದಿ (pat_list == ರಿಕ್ತ)
		    {
			emsg(_(e_list_required));
			ಅನುವರ್ತ;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    emsg(_(e_string_or_list_expected));
		    ಅನುವರ್ತ;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	ಯದಿ (cmd == ರಿಕ್ತ)
	{
	    ಯದಿ (delete)
		cmd = vim_strsave((char_u *)"");
	    ಅನ್ಯಥಾ
		ಅನುವರ್ತ;
	}

	ಯದಿ (delete && (event_name == ರಿಕ್ತ
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    ಪ್ರತಿ (event = (event_T)0; (ಪೂರ್ಣಾಙ್ಕ)event < ಅಂಕೆ_EVENTS;
		    event = (event_T)((ಪೂರ್ಣಾಙ್ಕ)event + 1))
	    {
		ಯದಿ (do_autocmd_event(event, pat, once, nested, cmd, delete,
							group, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    ಭಙ್ಗ;
		}
	    }
	}
	ಅನ್ಯಥಾ
	{
	    char_u *p = ರಿಕ್ತ;

	    eli = ರಿಕ್ತ;
	    end = ರಿಕ್ತ;
	    ಯಾವತ್ (TRUE)
	    {
		ಯದಿ (event_list != ರಿಕ್ತ)
		{
		    ಯದಿ (eli == ರಿಕ್ತ)
			eli = event_list->lv_first;
		    ಅನ್ಯಥಾ
			eli = eli->li_next;
		    ಯದಿ (eli == ರಿಕ್ತ)
			ಭಙ್ಗ;
		    ಯದಿ (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == ರಿಕ್ತ)
		    {
			emsg(_(e_string_required));
			ಭಙ್ಗ;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (p == ರಿಕ್ತ)
			p = event_name;
		    ಯದಿ (p == ರಿಕ್ತ || *p == NUL)
			ಭಙ್ಗ;
		}

		event = event_name2nr(p, &end);
		ಯದಿ (event == ಅಂಕೆ_EVENTS || *end != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    ಭಙ್ಗ;
		}
		ಯದಿ (pat != ರಿಕ್ತ)
		{
		    ಯದಿ (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, group, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			ಭಙ್ಗ;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (pat_list != ರಿಕ್ತ)
		{
		    FOR_ALL_LIST_ವಸ್ತುಗಳು(pat_list, pli)
		    {
			ಯದಿ (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == ರಿಕ್ತ)
			{
			    emsg(_(e_string_required));
			    ಅನುವರ್ತ;
			}
			ಯದಿ (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, group, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    ಭಙ್ಗ;
			}
		    }
		    ಯದಿ (retval == VVAL_FALSE)
			ಭಙ್ಗ;
		}
		ಯದಿ (event_name != ರಿಕ್ತ)
		    p = end;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	ಯದಿ (delete && group_name != ರಿಕ್ತ &&
		(event_name == ರಿಕ್ತ || event_name[0] == NUL)
		&& (pat == ರಿಕ್ತ || pat[0] == NUL)
		&& (cmd == ರಿಕ್ತ || cmd[0] == NUL))
	    au_del_group(group_name);
    }

    VIM_CLEAR(group_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_ಸಂಖ್ಯೆ = retval;
}

/*
 * autocmd_add() function
 */
    ಶೂನ್ಯ
f_autocmd_add(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    ಶೂನ್ಯ
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    ಶೂನ್ಯ
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = ಅಂಕೆ_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = ರಿಕ್ತ;
    char_u	*name = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		group = AUGROUP_ALL;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	ಯದಿ (dict_has_ಕೀಲಿ(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    ಯದಿ (name == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;

	    ಯದಿ (*name == NUL)
		group = AUGROUP_DEFAULT;
	    ಅನ್ಯಥಾ
	    {
		group = au_find_group(name);
		ಯದಿ (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), name);
		    vim_free(name);
		    ಪ್ರತ್ಯಾಗಮ;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	ಯದಿ (dict_has_ಕೀಲಿ(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    ಯದಿ (name == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;

	    ಯದಿ (name[0] == '*' && name[1] == NUL)
		event_arg = ಅಂಕೆ_EVENTS;
	    ಅನ್ಯಥಾ
	    {
		keyvalue_T target;
		keyvalue_T *entry;

		target.ಕೀಲಿ = 0;
		target.ಮೌಲ್ಯ.string = name;
		target.ಮೌಲ್ಯ.length = STRLEN(target.ಮೌಲ್ಯ.string);
		entry = (keyvalue_T *)bsearch(&target, &event_tab,
		    ಅಂಕೆ_EVENTS, ಪರಿಮಾಣ(event_tab[0]), cmp_keyvalue_ಮೌಲ್ಯ_ni);
		ಯದಿ (entry == ರಿಕ್ತ)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    ಪ್ರತ್ಯಾಗಮ;
		}
		event_arg = (event_T)abs(entry->ಕೀಲಿ);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	ಯದಿ (dict_has_ಕೀಲಿ(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    ಯದಿ (pat == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    ಪ್ರತಿ (event = (event_T)0; (ಪೂರ್ಣಾಙ್ಕ)event < ಅಂಕೆ_EVENTS;
	    event = (event_T)((ಪೂರ್ಣಾಙ್ಕ)event + 1))
    {
	string_T    *event_name;

	ಯದಿ (event_arg != ಅಂಕೆ_EVENTS && event != event_arg)
	    ಅನುವರ್ತ;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	group_name;

	    ಯದಿ (ap->pat == ರಿಕ್ತ)		// pattern has been removed
		ಅನುವರ್ತ;

	    ಯದಿ (group != AUGROUP_ALL && group != ap->group)
		ಅನುವರ್ತ;

	    ಯದಿ (pat != ರಿಕ್ತ && STRCMP(pat, ap->pat) != 0)
		ಅನುವರ್ತ;

	    group_name.string = get_augroup_name(ರಿಕ್ತ, ap->group);
	    ಯದಿ (group_name.string == ರಿಕ್ತ)
		STR_LITERAL_SET(group_name, "");
	    ಅನ್ಯಥಾ
		group_name.length = STRLEN(group_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    ಪ್ರತಿ (ac = ap->cmds; ac != ರಿಕ್ತ; ac = ac->next)
	    {
		event_dict = dict_alloc();
		ಯದಿ (event_dict == ರಿಕ್ತ
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    ಪ್ರತ್ಯಾಗಮ;
		}

		ಯದಿ (dict_add_string_len(event_dict, "event",
			event_name->string, (ಪೂರ್ಣಾಙ್ಕ)event_name->length) == FAIL
			|| dict_add_string_len(event_dict, "group",
			    group_name.string, (ಪೂರ್ಣಾಙ್ಕ)group_name.length) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_add_ಸಂಖ್ಯೆ(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_add_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_add_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_add_bool(event_dict, "once", ac->once) == FAIL
			|| dict_add_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    ಪ್ರತ್ಯಾಗಮ;
		}
	    }
	}
    }

    vim_free(pat);
}

#endif
