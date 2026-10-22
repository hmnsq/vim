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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    ಅಕ್ಷರ	    once;		// "One shot": removed after execution
    ಅಕ್ಷರ	    nested;		// If autocommands nest here.
    ಅಕ್ಷರ	    ಕೊನೆಯ;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    ರಚನೆ AutoCmd  *ಮುಂದಿನ;		// next AutoCmd in list
} AutoCmd;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ AutoPat
{
    ರಚನೆ AutoPat  *ಮುಂದಿನ;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    ಪೂರ್ಣಾಂಕ		    ಗುಂಪು;		// group ID
    ಪೂರ್ಣಾಂಕ		    patlen;		// strlen() of pat
    ಪೂರ್ಣಾಂಕ		    buflocal_nr;	// !=0 for buffer-local AutoPat
    ಅಕ್ಷರ	    allow_dirs;		// Pattern may match whole path
    ಅಕ್ಷರ	    ಕೊನೆಯ;		// last pattern for apply_autocmds()
} AutoPat;

//
// special cases:
// BufNewFile and BufRead are searched for ALOT (especially at startup)
// so we pre-determine their index into the event_tab[] table for fast access.
// Keep these values in sync with event_tab[]!
#ವ್ಯಾಖ್ಯಾನಿಸು BUFNEWFILE_ಸೂಚ್ಯಂಕ 9
#ವ್ಯಾಖ್ಯಾನಿಸು BUFREAD_ಸೂಚ್ಯಂಕ 10

// Must be sorted by the 'value' field because it is used by bsearch()!
// Events with positive keys aren't allowed in 'eventignorewin'.
ಸ್ಥಿರ keyvalue_T event_tab[ಅಂಕೆ_EVENTS] = {
    KEYVALUE_ನಮೂದು(-EVENT_BUFADD, "BufAdd"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFADD, "BufCreate"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFDELETE, "BufDelete"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFENTER, "BufEnter"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFFILEPOST, "BufFilePost"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFFILEPRE, "BufFilePre"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFHIDDEN, "BufHidden"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFLEAVE, "BufLeave"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFNEW, "BufNew"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFNEWFILE, "BufNewFile"),	// BUFNEWFILE_INDEX
    KEYVALUE_ನಮೂದು(-EVENT_BUFREADPOST, "BufRead"),	// BUFREAD_INDEX
    KEYVALUE_ನಮೂದು(-EVENT_BUFREADCMD, "BufReadCmd"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFREADPOST, "BufReadPost"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFREADPRE, "BufReadPre"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFUNLOAD, "BufUnload"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFWINENTER, "BufWinEnter"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFWINLEAVE, "BufWinLeave"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFWIPEOUT, "BufWipeout"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFWRITEPRE, "BufWrite"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFWRITECMD, "BufWriteCmd"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFWRITEPOST, "BufWritePost"),
    KEYVALUE_ನಮೂದು(-EVENT_BUFWRITEPRE, "BufWritePre"),
    KEYVALUE_ನಮೂದು(EVENT_CMDLINECHANGED, "CmdlineChanged"),
    KEYVALUE_ನಮೂದು(EVENT_CMDLINEENTER, "CmdlineEnter"),
    KEYVALUE_ನಮೂದು(EVENT_CMDLINELEAVE, "CmdlineLeave"),
    KEYVALUE_ನಮೂದು(EVENT_CMDLINELEAVEPRE, "CmdlineLeavePre"),
    KEYVALUE_ನಮೂದು(EVENT_CMDUNDEFINED, "CmdUndefined"),
    KEYVALUE_ನಮೂದು(EVENT_CMDWINENTER, "CmdwinEnter"),
    KEYVALUE_ನಮೂದು(EVENT_CMDWINLEAVE, "CmdwinLeave"),
    KEYVALUE_ನಮೂದು(EVENT_COLORSCHEME, "ColorScheme"),
    KEYVALUE_ನಮೂದು(EVENT_COLORSCHEMEPRE, "ColorSchemePre"),
    KEYVALUE_ನಮೂದು(EVENT_COMPLETECHANGED, "CompleteChanged"),
    KEYVALUE_ನಮೂದು(EVENT_COMPLETEDONE, "CompleteDone"),
    KEYVALUE_ನಮೂದು(EVENT_COMPLETEDONEPRE, "CompleteDonePre"),
    KEYVALUE_ನಮೂದು(-EVENT_CURSORHOLD, "CursorHold"),
    KEYVALUE_ನಮೂದು(-EVENT_CURSORHOLDI, "CursorHoldI"),
    KEYVALUE_ನಮೂದು(-EVENT_CURSORMOVED, "CursorMoved"),
    KEYVALUE_ನಮೂದು(-EVENT_CURSORMOVEDC, "CursorMovedC"),
    KEYVALUE_ನಮೂದು(-EVENT_CURSORMOVEDI, "CursorMovedI"),
    KEYVALUE_ನಮೂದು(EVENT_DIFFUPDATED, "DiffUpdated"),
    KEYVALUE_ನಮೂದು(EVENT_DIRCHANGED, "DirChanged"),
    KEYVALUE_ನಮೂದು(EVENT_DIRCHANGEDPRE, "DirChangedPre"),
    KEYVALUE_ನಮೂದು(EVENT_ENCODINGCHANGED, "EncodingChanged"),
    KEYVALUE_ನಮೂದು(EVENT_EXITPRE, "ExitPre"),
    KEYVALUE_ನಮೂದು(-EVENT_FILEAPPENDCMD, "FileAppendCmd"),
    KEYVALUE_ನಮೂದು(-EVENT_FILEAPPENDPOST, "FileAppendPost"),
    KEYVALUE_ನಮೂದು(-EVENT_FILEAPPENDPRE, "FileAppendPre"),
    KEYVALUE_ನಮೂದು(-EVENT_FILECHANGEDRO, "FileChangedRO"),
    KEYVALUE_ನಮೂದು(-EVENT_FILECHANGEDSHELL, "FileChangedShell"),
    KEYVALUE_ನಮೂದು(-EVENT_FILECHANGEDSHELLPOST, "FileChangedShellPost"),
    KEYVALUE_ನಮೂದು(EVENT_ENCODINGCHANGED, "FileEncoding"),
    KEYVALUE_ನಮೂದು(-EVENT_FILEREADCMD, "FileReadCmd"),
    KEYVALUE_ನಮೂದು(-EVENT_FILEREADPOST, "FileReadPost"),
    KEYVALUE_ನಮೂದು(-EVENT_FILEREADPRE, "FileReadPre"),
    KEYVALUE_ನಮೂದು(-EVENT_FILETYPE, "FileType"),
    KEYVALUE_ನಮೂದು(-EVENT_FILEWRITECMD, "FileWriteCmd"),
    KEYVALUE_ನಮೂದು(-EVENT_FILEWRITEPOST, "FileWritePost"),
    KEYVALUE_ನಮೂದು(-EVENT_FILEWRITEPRE, "FileWritePre"),
    KEYVALUE_ನಮೂದು(-EVENT_FILTERREADPOST, "FilterReadPost"),
    KEYVALUE_ನಮೂದು(-EVENT_FILTERREADPRE, "FilterReadPre"),
    KEYVALUE_ನಮೂದು(-EVENT_FILTERWRITEPOST, "FilterWritePost"),
    KEYVALUE_ನಮೂದು(-EVENT_FILTERWRITEPRE, "FilterWritePre"),
    KEYVALUE_ನಮೂದು(EVENT_FOCUSGAINED, "FocusGained"),
    KEYVALUE_ನಮೂದು(EVENT_FOCUSLOST, "FocusLost"),
    KEYVALUE_ನಮೂದು(EVENT_FUNCUNDEFINED, "FuncUndefined"),
    KEYVALUE_ನಮೂದು(EVENT_GUIENTER, "GUIEnter"),
    KEYVALUE_ನಮೂದು(EVENT_GUIFAILED, "GUIFailed"),
    KEYVALUE_ನಮೂದು(-EVENT_INSERTCHANGE, "InsertChange"),
    KEYVALUE_ನಮೂದು(-EVENT_INSERTCHARPRE, "InsertCharPre"),
    KEYVALUE_ನಮೂದು(-EVENT_INSERTENTER, "InsertEnter"),
    KEYVALUE_ನಮೂದು(-EVENT_INSERTLEAVE, "InsertLeave"),
    KEYVALUE_ನಮೂದು(-EVENT_INSERTLEAVEPRE, "InsertLeavePre"),
    KEYVALUE_ನಮೂದು(EVENT_KEYINPUTPRE, "KeyInputPre"),
    KEYVALUE_ನಮೂದು(EVENT_MENUPOPUP, "MenuPopup"),
    KEYVALUE_ನಮೂದು(EVENT_MODECHANGED, "ModeChanged"),
    KEYVALUE_ನಮೂದು(EVENT_OPTIONSET, "OptionSet"),
    KEYVALUE_ನಮೂದು(EVENT_QUICKFIXCMDPOST, "QuickFixCmdPost"),
    KEYVALUE_ನಮೂದು(EVENT_QUICKFIXCMDPRE, "QuickFixCmdPre"),
    KEYVALUE_ನಮೂದು(EVENT_QUITPRE, "QuitPre"),
    KEYVALUE_ನಮೂದು(EVENT_REMOTEREPLY, "RemoteReply"),
    KEYVALUE_ನಮೂದು(EVENT_SAFESTATE, "SafeState"),
    KEYVALUE_ನಮೂದು(EVENT_SAFESTATEAGAIN, "SafeStateAgain"),
    KEYVALUE_ನಮೂದು(EVENT_SESSIONLOADPOST, "SessionLoadPost"),
    KEYVALUE_ನಮೂದು(EVENT_SESSIONLOADPRE, "SessionLoadPre"),
    KEYVALUE_ನಮೂದು(EVENT_SESSIONWRITEPOST, "SessionWritePost"),
    KEYVALUE_ನಮೂದು(EVENT_SHELLCMDPOST, "ShellCmdPost"),
    KEYVALUE_ನಮೂದು(-EVENT_SHELLFILTERPOST, "ShellFilterPost"),
    KEYVALUE_ನಮೂದು(EVENT_SIGUSR1, "SigUSR1"),
    KEYVALUE_ನಮೂದು(EVENT_SOURCECMD, "SourceCmd"),
    KEYVALUE_ನಮೂದು(EVENT_SOURCEPOST, "SourcePost"),
    KEYVALUE_ನಮೂದು(EVENT_SOURCEPRE, "SourcePre"),
    KEYVALUE_ನಮೂದು(EVENT_SPELLFILEMISSING, "SpellFileMissing"),
    KEYVALUE_ನಮೂದು(EVENT_STDINREADPOST, "StdinReadPost"),
    KEYVALUE_ನಮೂದು(EVENT_STDINREADPRE, "StdinReadPre"),
    KEYVALUE_ನಮೂದು(EVENT_SWAPEXISTS, "SwapExists"),
    KEYVALUE_ನಮೂದು(EVENT_SYNTAX, "Syntax"),
    KEYVALUE_ನಮೂದು(EVENT_TABCLOSED, "TabClosed"),
    KEYVALUE_ನಮೂದು(EVENT_TABCLOSEDPRE, "TabClosedPre"),
    KEYVALUE_ನಮೂದು(EVENT_TABENTER, "TabEnter"),
    KEYVALUE_ನಮೂದು(EVENT_TABLEAVE, "TabLeave"),
    KEYVALUE_ನಮೂದು(EVENT_TABNEW, "TabNew"),
    KEYVALUE_ನಮೂದು(EVENT_TERMCHANGED, "TermChanged"),
    KEYVALUE_ನಮೂದು(EVENT_TERMINALOPEN, "TerminalOpen"),
    KEYVALUE_ನಮೂದು(EVENT_TERMINALWINOPEN, "TerminalWinOpen"),
    KEYVALUE_ನಮೂದು(EVENT_TERMRESPONSE, "TermResponse"),
    KEYVALUE_ನಮೂದು(EVENT_TERMRESPONSEALL, "TermResponseAll"),
    KEYVALUE_ನಮೂದು(-EVENT_TEXTCHANGED, "TextChanged"),
    KEYVALUE_ನಮೂದು(-EVENT_TEXTCHANGEDI, "TextChangedI"),
    KEYVALUE_ನಮೂದು(-EVENT_TEXTCHANGEDP, "TextChangedP"),
    KEYVALUE_ನಮೂದು(-EVENT_TEXTCHANGEDT, "TextChangedT"),
    KEYVALUE_ನಮೂದು(-EVENT_TEXTPUTPOST, "TextPutPost"),
    KEYVALUE_ನಮೂದು(-EVENT_TEXTPUTPRE, "TextPutPre"),
    KEYVALUE_ನಮೂದು(-EVENT_TEXTYANKPOST, "TextYankPost"),
    KEYVALUE_ನಮೂದು(EVENT_USER, "User"),
    KEYVALUE_ನಮೂದು(EVENT_VIMENTER, "VimEnter"),
    KEYVALUE_ನಮೂದು(EVENT_VIMLEAVE, "VimLeave"),
    KEYVALUE_ನಮೂದು(EVENT_VIMLEAVEPRE, "VimLeavePre"),
    KEYVALUE_ನಮೂದು(EVENT_VIMRESIZED, "VimResized"),
    KEYVALUE_ನಮೂದು(EVENT_VIMRESUME, "VimResume"),
    KEYVALUE_ನಮೂದು(EVENT_VIMSUSPEND, "VimSuspend"),
    KEYVALUE_ನಮೂದು(-EVENT_WINCLOSED, "WinClosed"),
    KEYVALUE_ನಮೂದು(-EVENT_WINENTER, "WinEnter"),
    KEYVALUE_ನಮೂದು(-EVENT_WINLEAVE, "WinLeave"),
    KEYVALUE_ನಮೂದು(EVENT_WINNEW, "WinNew"),
    KEYVALUE_ನಮೂದು(EVENT_WINNEWPRE, "WinNewPre"),
    KEYVALUE_ನಮೂದು(-EVENT_WINRESIZED, "WinResized"),
    KEYVALUE_ನಮೂದು(-EVENT_WINSCROLLED, "WinScrolled"),
};

ಸ್ಥಿರ AutoPat *ಮೊದಲ_autopat[ಅಂಕೆ_EVENTS] = { ಖಾಲಿ };
ಸ್ಥಿರ AutoPat *ಕೊನೆಯ_autopat[ಅಂಕೆ_EVENTS] = { ಖಾಲಿ };

#ವ್ಯಾಖ್ಯಾನಿಸು AUGROUP_DEFAULT    (-1)	    // default autocmd group
#ವ್ಯಾಖ್ಯಾನಿಸು AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#ವ್ಯಾಖ್ಯಾನಿಸು AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
ರಚನೆ AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    ಪೂರ್ಣಾಂಕ		ಗುಂಪು;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*ಬಾಲ;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    ಪೂರ್ಣಾಂಕ		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *ಮುಂದಿನ;		// chain of active apc-s for auto-invalidation
};

ಸ್ಥಿರ AutoPatCmd_T *ಸಕ್ರಿಯ_apc_list = ಖಾಲಿ; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    ಗಾಗಿ ((ap) = ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)(event)]; (ap) != ಖಾಲಿ; (ap) = (ap)->ಮುಂದಿನ)

/*
 * augroups stores a list of autocmd group names.
 */
ಸ್ಥಿರ garray_T augroups = {0, 0, ಗಾತ್ರ(char_u *), 10, ಖಾಲಿ};
#ವ್ಯಾಖ್ಯಾನಿಸು AUGROUP_NAME(i) (((char_u **)augroups.ga_ದತ್ತಾಂಶ)[i])
// use get_deleted_augroup() to get this
ಸ್ಥಿರ char_u *deleted_augroup = ಖಾಲಿ;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ current_augroup = AUGROUP_DEFAULT;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ au_need_clean = FALSE;   // need to delete marked patterns

ಸ್ಥಿರ event_T event_name2nr(char_u *ಆರಂಭ, char_u **ಕೊನೆ);
ಸ್ಥಿರ string_T *event_nr2name(event_T event);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ au_get_grouparg(char_u **argp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ do_autocmd_event(event_T event, char_u *pat, ಪೂರ್ಣಾಂಕ once, ಪೂರ್ಣಾಂಕ nested, char_u *cmd, ಪೂರ್ಣಾಂಕ forceit, ಪೂರ್ಣಾಂಕ ಗುಂಪು, ಪೂರ್ಣಾಂಕ flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ apply_autocmds_ಗುಂಪು(event_T event, char_u *fname, char_u *fname_io, ಪೂರ್ಣಾಂಕ force, ಪೂರ್ಣಾಂಕ ಗುಂಪು, buf_T *buf, exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ auto_ಮುಂದಿನ_pat(AutoPatCmd_T *apc, ಪೂರ್ಣಾಂಕ stop_at_ಕೊನೆಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ au_ಕಂಡುಹಿಡಿ_ಗುಂಪು(char_u *name);

ಸ್ಥಿರ event_T	ಕೊನೆಯ_event;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಕೊನೆಯ_ಗುಂಪು;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	autocmd_blocked = 0;	// block all autocmds

    ಸ್ಥಿರ char_u *
get_deleted_augroup(ಶೂನ್ಯ)
{
    ಆದರೆ (deleted_augroup == ಖಾಲಿ)
	deleted_augroup = (char_u *)_("--Deleted--");
    ಹಿಂತಿರುಗಿ deleted_augroup;
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
    ಆದರೆ (got_int)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (ap->pat == ಖಾಲಿ)		// pattern has been removed
	ಹಿಂತಿರುಗಿ;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    ಸಂದೇಶ_putchar('\n');
    ಆದರೆ (got_int)
	ಹೋಗು theend;
    ಆದರೆ (event != ಕೊನೆಯ_event || ap->ಗುಂಪು != ಕೊನೆಯ_ಗುಂಪು)
    {
	string_T    *event_name;

	ಆದರೆ (ap->ಗುಂಪು != AUGROUP_DEFAULT)
	{
	    ಆದರೆ (AUGROUP_NAME(ap->ಗುಂಪು) == ಖಾಲಿ)
		ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)AUGROUP_NAME(ap->ಗುಂಪು), HL_ATTR(HLF_T));
	    ಸಂದೇಶ_puts("  ");
	}
	event_name = event_nr2name(event);
	ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)event_name->string, HL_ATTR(HLF_T));
	ಕೊನೆಯ_event = event;
	ಕೊನೆಯ_ಗುಂಪು = ap->ಗುಂಪು;
	ಸಂದೇಶ_putchar('\n');
	ಆದರೆ (got_int)
	    ಹೋಗು theend;
    }

    ಆದರೆ (ap->pat == ಖಾಲಿ)
	ಹೋಗು theend;  // timer might have cleared the pattern or group

    ಸಂದೇಶ_ಲಂಬಸಾಲು = 4;
    ಸಂದೇಶ_outtrans(ap->pat);

    ಗಾಗಿ (ac = ap->cmds; ac != ಖಾಲಿ; ac = ac->ಮುಂದಿನ)
    {
	ಆದರೆ (ac->cmd == ಖಾಲಿ)		// skip removed commands
	    ಮುಂದುವರಿ;

	ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು >= 14)
	    ಸಂದೇಶ_putchar('\n');
	ಸಂದೇಶ_ಲಂಬಸಾಲು = 14;
	ಆದರೆ (got_int)
	    ಹೋಗು theend;
	ಸಂದೇಶ_outtrans(ac->cmd);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (p_verbose > 0)
	    ಕೊನೆಯ_ಗಣ_ಸಂದೇಶ(ac->script_ctx);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (got_int)
	    ಹೋಗು theend;
	ಆದರೆ (ac->ಮುಂದಿನ != ಖಾಲಿ)
	{
	    ಸಂದೇಶ_putchar('\n');
	    ಆದರೆ (got_int)
		ಹೋಗು theend;
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

    ಗಾಗಿ (ac = ap->cmds; ac != ಖಾಲಿ; ac = ac->ಮುಂದಿನ)
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

    ಆದರೆ (autocmd_busy || !au_need_clean)
	ಹಿಂತಿರುಗಿ;

    // loop over all events
    ಗಾಗಿ (event = (event_T)0; (ಪೂರ್ಣಾಂಕ)event < ಅಂಕೆ_EVENTS;
					    event = (event_T)((ಪೂರ್ಣಾಂಕ)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)event]);
	ಗಾಗಿ (ap = *prev_ap; ap != ಖಾಲಿ; ap = *prev_ap)
	{
	    ಪೂರ್ಣಾಂಕ has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    ಗಾಗಿ (ac = *prev_ac; ac != ಖಾಲಿ; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		ಆದರೆ (ap->pat == ಖಾಲಿ || ac->cmd == ಖಾಲಿ)
		{
		    *prev_ac = ac->ಮುಂದಿನ;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->ಮುಂದಿನ);
		}
	    }

	    ಆದರೆ (ap->pat != ಖಾಲಿ && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    ಆದರೆ (ap->pat == ಖಾಲಿ)
	    {
		ಆದರೆ (ap->ಮುಂದಿನ == ಖಾಲಿ)
		{
		    ಆದರೆ (prev_ap == &(ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)event]))
			ಕೊನೆಯ_autopat[(ಪೂರ್ಣಾಂಕ)event] = ಖಾಲಿ;
		    ಇಲ್ಲದಿದ್ದರೆ
			// this depends on the "next" field being the first in
			// the struct
			ಕೊನೆಯ_autopat[(ಪೂರ್ಣಾಂಕ)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->ಮುಂದಿನ;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		prev_ap = &(ap->ಮುಂದಿನ);
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
    ಗಾಗಿ (apc = ಸಕ್ರಿಯ_apc_list; apc; apc = apc->ಮುಂದಿನ)
	ಆದರೆ (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    ಗಾಗಿ (event = (event_T)0; (ಪೂರ್ಣಾಂಕ)event < ಅಂಕೆ_EVENTS;
					    event = (event_T)((ಪೂರ್ಣಾಂಕ)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ಆದರೆ (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		ಆದರೆ (p_verbose >= 6)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
au_new_ಗುಂಪು(char_u *name)
{
    ಪೂರ್ಣಾಂಕ		i;

    i = au_ಕಂಡುಹಿಡಿ_ಗುಂಪು(name);
    ಆದರೆ (i != AUGROUP_ERROR)
	ಹಿಂತಿರುಗಿ i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    ಗಾಗಿ (i = 0; i < augroups.ga_len; ++i)
	ಆದರೆ (AUGROUP_NAME(i) == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	ಹಿಂತಿರುಗಿ AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    ಆದರೆ (AUGROUP_NAME(i) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ AUGROUP_ERROR;
    ಆದರೆ (i == augroups.ga_len)
	++augroups.ga_len;

    ಹಿಂತಿರುಗಿ i;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
au_del_ಗುಂಪು(char_u *name)
{
    ಪೂರ್ಣಾಂಕ		i;
    event_T	event;
    AutoPat	*ap;
    ಪೂರ್ಣಾಂಕ		ಒಳ_use = FALSE;


    i = au_ಕಂಡುಹಿಡಿ_ಗುಂಪು(name);
    ಆದರೆ (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_ಗುಂಪು_str), name);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_ಗುಂಪು));
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (event = (event_T)0; (ಪೂರ್ಣಾಂಕ)event < ಅಂಕೆ_EVENTS;
	    event = (event_T)((ಪೂರ್ಣಾಂಕ)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ಆದರೆ (ap->ಗುಂಪು == i && ap->pat != ಖಾಲಿ)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		ಒಳ_use = TRUE;
		event = ಅಂಕೆ_EVENTS;
		ನಿಲ್ಲಿಸು;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    ಆದರೆ (ಒಳ_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    ಇಲ್ಲದಿದ್ದರೆ
	AUGROUP_NAME(i) = ಖಾಲಿ;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
au_ಕಂಡುಹಿಡಿ_ಗುಂಪು(char_u *name)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = 0; i < augroups.ga_len; ++i)
	ಆದರೆ (AUGROUP_NAME(i) != ಖಾಲಿ && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    ಹಿಂತಿರುಗಿ i;
    ಹಿಂತಿರುಗಿ AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    ಪೂರ್ಣಾಂಕ
au_has_ಗುಂಪು(char_u *name)
{
    ಹಿಂತಿರುಗಿ au_ಕಂಡುಹಿಡಿ_ಗುಂಪು(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    ಶೂನ್ಯ
do_augroup(char_u *arg, ಪೂರ್ಣಾಂಕ del_ಗುಂಪು)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಆದರೆ (del_ಗುಂಪು)
    {
	ಆದರೆ (*arg == NUL)
	    emsg(_(e_ವಾದ_required));
	ಇಲ್ಲದಿದ್ದರೆ
	    au_del_ಗುಂಪು(arg);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_ಗುಂಪು(arg);
	ಆದರೆ (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    ಇಲ್ಲದಿದ್ದರೆ			    // ":aug": list the group names
    {
	ಸಂದೇಶ_ಆರಂಭ();
	ಗಾಗಿ (i = 0; i < augroups.ga_len; ++i)
	{
	    ಆದರೆ (AUGROUP_NAME(i) != ಖಾಲಿ)
	    {
		ಸಂದೇಶ_puts((ಅಕ್ಷರ *)AUGROUP_NAME(i));
		ಸಂದೇಶ_puts("  ");
	    }
	}
	ಸಂದೇಶ_clr_eos();
	ಸಂದೇಶ_ಕೊನೆ();
    }
}

    ಶೂನ್ಯ
autocmd_init(ಶೂನ್ಯ)
{
    CLEAR_ಕ್ಷೇತ್ರ(aucmd_win);
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_all_autocmds(ಶೂನ್ಯ)
{
    char_u	*s;

    ಗಾಗಿ (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(ಖಾಲಿ, (char_u *)"", TRUE);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_ದತ್ತಾಂಶ))[i];
	ಆದರೆ (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    ಪೂರ್ಣಾಂಕ
is_aucmd_win(win_T *win)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < AUCMD_WIN_ಎಣಿಕೆ; ++i)
	ಆದರೆ (aucmd_win[i].auc_win_ಬಳಸಿದ && aucmd_win[i].auc_win == win)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    ಸ್ಥಿರ event_T
event_name2nr(char_u *ಆರಂಭ, char_u **ಕೊನೆ)
{
    char_u	*p;
    keyvalue_T ಗುರಿ;
    keyvalue_T *ನಮೂದು;
    ಸ್ಥಿರ keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_ಸೂಚ್ಯಂಕ];
    ಸ್ಥಿರ keyvalue_T *bufread = &event_tab[BUFREAD_ಸೂಚ್ಯಂಕ];

    // the event name ends with end of line, '|', a blank or a comma
    ಗಾಗಿ (p = ಆರಂಭ; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    ಗುರಿ.ಕೀಲಿ = 0;
    ಗುರಿ.ಮೌಲ್ಯ.string = ಆರಂಭ;
    ಗುರಿ.ಮೌಲ್ಯ.ಉದ್ದಳತೆ = (size_t)(p - ಆರಂಭ);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    ಆದರೆ (cmp_keyvalue_ಮೌಲ್ಯ_ni(&ಗುರಿ, bufnewfile) == 0)
	ನಮೂದು = bufnewfile;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmp_keyvalue_ಮೌಲ್ಯ_ni(&ಗುರಿ, bufread) == 0)
	ನಮೂದು = bufread;
    ಇಲ್ಲದಿದ್ದರೆ
	ನಮೂದು = (keyvalue_T *)bsearch(&ಗುರಿ, &event_tab, ಅಂಕೆ_EVENTS,
				ಗಾತ್ರ(event_tab[0]), cmp_keyvalue_ಮೌಲ್ಯ_ni);

    ಆದರೆ (*p == ',')
	++p;
    *ಕೊನೆ = p;

    ಹಿಂತಿರುಗಿ (ನಮೂದು == ಖಾಲಿ) ? ಅಂಕೆ_EVENTS : (event_T)ನಿರಪೇಕ್ಷ(ನಮೂದು->ಕೀಲಿ);
}

/*
 * Return the name for event "event".
 */
    ಸ್ಥಿರ string_T *
event_nr2name(event_T event)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಎಣಿಕೆ {CACHE_ಗಾತ್ರ = 12};
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cache_tab[CACHE_ಗಾತ್ರ];
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cache_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ = -1;
    ಸ್ಥಿರ string_T unknown = STR_LITERAL_INIT("Unknown");

    ಆದರೆ (cache_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ < 0)
    {
	ಗಾಗಿ (i = 0; i < CACHE_ಗಾತ್ರ; ++i)
	    cache_tab[i] = -1;
	cache_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ = CACHE_ಗಾತ್ರ - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    ಗಾಗಿ (i = cache_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ; cache_tab[i] >= 0; )
    {
	ಆದರೆ ((event_T)ನಿರಪೇಕ್ಷ(event_tab[cache_tab[i]].ಕೀಲಿ) == event)
	    ಹಿಂತಿರುಗಿ &event_tab[cache_tab[i]].ಮೌಲ್ಯ;

	ಆದರೆ (i == 0)
	    i = CACHE_ಗಾತ್ರ - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    --i;

	// are we back at the start?
	ಆದರೆ (i == cache_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ)
	    ನಿಲ್ಲಿಸು;
    }

    // look in the event table itself
    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_EVENTS; ++i)
    {
	ಆದರೆ ((event_T)ನಿರಪೇಕ್ಷ(event_tab[i].ಕೀಲಿ) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    ಆದರೆ (cache_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ == CACHE_ಗಾತ್ರ - 1)
		cache_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		++cache_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ;
	    cache_tab[cache_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ] = i;
	    ಹಿಂತಿರುಗಿ &event_tab[i].ಮೌಲ್ಯ;
	}
    }

    ಹಿಂತಿರುಗಿ &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    ಸ್ಥಿರ char_u *
ಕಂಡುಹಿಡಿ_ಕೊನೆ_event(
    char_u  *arg,
    ಪೂರ್ಣಾಂಕ	    have_ಗುಂಪು)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    ಆದರೆ (*arg == '*')
    {
	ಆದರೆ (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_ನಕ್ಷತ್ರ_str), arg);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	pat = arg + 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)event_name2nr(pat, &p) >= ಅಂಕೆ_EVENTS)
	    {
		ಆದರೆ (have_ಗುಂಪು)
		    semsg(_(e_no_such_event_str), pat);
		ಇಲ್ಲದಿದ್ದರೆ
		    semsg(_(e_no_such_ಗುಂಪು_or_event_str), pat);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}
    }
    ಹಿಂತಿರುಗಿ pat;
}

/*
 * Find the command of an ":autocmd" with argument "arg", the part after the
 * events, the pattern and any "++once"/"++nested".  Returns NULL when there is
 * no command.  Does not modify "arg" and gives no error messages.
 */
    char_u *
au_ಕಂಡುಹಿಡಿ_cmd_arg(char_u *arg)
{
    char_u	*pat;
    char_u	*cmd;
    ಪೂರ್ಣಾಂಕ		ಗುಂಪು;

    ಆದರೆ (*arg == '|')
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    ಗುಂಪು = au_get_grouparg(&arg);
    pat = ಗುಂಪು == AUGROUP_ERROR
			  ? ಖಾಲಿ : ಕಂಡುಹಿಡಿ_ಕೊನೆ_event(arg, ಗುಂಪು != AUGROUP_ALL);
    --emsg_off;
    ಆದರೆ (pat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    pat = skipwhite(pat);
    ಆದರೆ (*pat == NUL || *pat == '|')
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    ಆಗ (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 2; ++i)
    {
	ಆದರೆ (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	ಆದರೆ (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	ಆದರೆ (!ಒಳ_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    ಹಿಂತಿರುಗಿ *cmd == NUL ? ಖಾಲಿ : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    ಪೂರ್ಣಾಂಕ
event_ignored(event_T event, char_u *ei)
{
    ಪೂರ್ಣಾಂಕ ignored = FALSE;
    ಆಗ (*ei != NUL)
    {
	ಪೂರ್ಣಾಂಕ unignore = *ei == '-';
	ei += unignore;
	ಆದರೆ (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].ಕೀಲಿ <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (event_name2nr(ei, &ei) == event)
	{
	    ಆದರೆ (unignore)
		ಹಿಂತಿರುಗಿ FALSE;
	    ಇಲ್ಲದಿದ್ದರೆ
		ignored = TRUE;
	}
    }

    ಹಿಂತಿರುಗಿ ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
check_ei(char_u *ei)
{
    ಪೂರ್ಣಾಂಕ	win = ei != p_ei;

    ಆಗ (*ei)
    {
	ಆದರೆ (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    ಆದರೆ (event == ಅಂಕೆ_EVENTS || (win && event_tab[event].ಕೀಲಿ > 0))
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(ಅಕ್ಷರ *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    size_t	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    ಆದರೆ (save_ei == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    ಆದರೆ (new_ei == ಖಾಲಿ)
    {
	vim_free(save_ei);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    ಇಲ್ಲದಿದ್ದರೆ
	STRCPY(new_ei + p_ei_len, what);
    ಗಣ_string_option_ನೇರ((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    ಹಿಂತಿರುಗಿ save_ei;
}

    ಶೂನ್ಯ
au_event_restore(char_u *old_ei)
{
    ಆದರೆ (old_ei != ಖಾಲಿ)
    {
	ಗಣ_string_option_ನೇರ((char_u *)"ei", -1, old_ei,
							  OPT_FREE, SID_NONE);
	vim_free(old_ei);
    }
}
#ಆದರೆ_ಅಂತ್ಯ  // FEAT_SYN_HL

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
do_autocmd(exarg_T *eap, char_u *arg_ಒಳ, ಪೂರ್ಣಾಂಕ forceit)
{
    char_u	*arg = arg_ಒಳ;
    char_u	*pat;
    char_u	*envpat = ಖಾಲಿ;
    char_u	*cmd;
    ಪೂರ್ಣಾಂಕ		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		nested = FALSE;
    ಪೂರ್ಣಾಂಕ		once = FALSE;
    ಪೂರ್ಣಾಂಕ		ಗುಂಪು;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		flags = 0;

    ಆದರೆ (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	ಗುಂಪು = AUGROUP_ALL;	// no argument, use all groups
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	ಗುಂಪು = au_get_grouparg(&arg);
	ಆದರೆ (arg == ಖಾಲಿ)	    // out of memory
	    ಹಿಂತಿರುಗಿ;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = ಕಂಡುಹಿಡಿ_ಕೊನೆ_event(arg, ಗುಂಪು != AUGROUP_ALL);
    ಆದರೆ (pat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    pat = skipwhite(pat);
    ಆದರೆ (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	ಆಗ (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	ಆದರೆ (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	ಆದರೆ (vim_strchr(pat, '$') != ಖಾಲಿ || vim_strchr(pat, '~') != ಖಾಲಿ)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	    ಪೂರ್ಣಾಂಕ	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    envpat = expand_env_save(pat);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	    p_ssl = p_ssl_save;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (envpat != ಖಾಲಿ)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	ಗಾಗಿ (i = 0; i < 2; i++)
	{
	    ಆದರೆ (*cmd == NUL)
		ಮುಂದುವರಿ;

	    // Check for "++once" flag.
	    ಆದರೆ (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ಆದರೆ (once)
		{
		    semsg(_(e_duplicate_ವಾದ_str), "++once");
		    ಹೋಗು err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    ಆದರೆ ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		ಆದರೆ (nested)
		{
		    semsg(_(e_duplicate_ವಾದ_str), "++nested");
		    ಹೋಗು err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    ಆದರೆ (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ಆದರೆ (ಒಳ_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_ಸರಾಸರಿ_plusplus_nested));
		    ಹೋಗು err_exit;
		}
		ಆದರೆ (nested)
		{
		    semsg(_(e_duplicate_ವಾದ_str), "nested");
		    ಹೋಗು err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 6);
	    }
	}

	/*
	 * Find the start of the commands.
	 * Expand <sfile> in it.
	 */
	ಆದರೆ (*cmd != NUL)
	{
	    ಆದರೆ (eap != ಖಾಲಿ)
		// Read a {} block if it follows.
		cmd = may_get_cmd_ಖಂಡ(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    ಆದರೆ (cmd == ಖಾಲಿ)	    // some error
		ಹೋಗು err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    ಆದರೆ (!forceit && *cmd == NUL)
	// Highlight title
	ಸಂದೇಶ_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    ಕೊನೆಯ_event = (event_T)-1;		// for listing the event name
    ಕೊನೆಯ_ಗುಂಪು = AUGROUP_ERROR;		// for listing the group name
    ಆದರೆ (*arg == '*' || *arg == NUL || *arg == '|')
    {
	ಆದರೆ (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗಾಗಿ (event = (event_T)0; (ಪೂರ್ಣಾಂಕ)event < ಅಂಕೆ_EVENTS;
					     event = (event_T)((ಪೂರ್ಣಾಂಕ)event + 1))
		ಆದರೆ (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, ಗುಂಪು, flags) == FAIL)
		    ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಗ (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    ಆದರೆ (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, ಗುಂಪು, flags) == FAIL)
		ನಿಲ್ಲಿಸು;
    }

err_exit:
    ಆದರೆ (cmd_need_free)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
au_get_grouparg(char_u **argp)
{
    char_u	*ಗುಂಪು_name;
    char_u	*p;
    char_u	*arg = *argp;
    ಪೂರ್ಣಾಂಕ		ಗುಂಪು = AUGROUP_ALL;

    ಗಾಗಿ (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    ಆದರೆ (p <= arg)
	ಹಿಂತಿರುಗಿ AUGROUP_ALL;

    ಗುಂಪು_name = vim_strnsave(arg, p - arg);
    ಆದರೆ (ಗುಂಪು_name == ಖಾಲಿ)		// out of memory
	ಹಿಂತಿರುಗಿ AUGROUP_ERROR;
    ಗುಂಪು = au_ಕಂಡುಹಿಡಿ_ಗುಂಪು(ಗುಂಪು_name);
    ಆದರೆ (ಗುಂಪು == AUGROUP_ERROR)
	ಗುಂಪು = AUGROUP_ALL;	// no match, use all groups
    ಇಲ್ಲದಿದ್ದರೆ
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(ಗುಂಪು_name);
    ಹಿಂತಿರುಗಿ ಗುಂಪು;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    ಪೂರ್ಣಾಂಕ		once,
    ಪೂರ್ಣಾಂಕ		nested,
    char_u	*cmd,
    ಪೂರ್ಣಾಂಕ		forceit,
    ಪೂರ್ಣಾಂಕ		ಗುಂಪು,
    ಪೂರ್ಣಾಂಕ		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    ಪೂರ್ಣಾಂಕ		brace_level;
    char_u	*endpat;
    ಪೂರ್ಣಾಂಕ		findgroup;
    ಪೂರ್ಣಾಂಕ		allgroups;
    ಪೂರ್ಣಾಂಕ		patlen;
    ಪೂರ್ಣಾಂಕ		is_buflocal;
    ಪೂರ್ಣಾಂಕ		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    ಆದರೆ (ಗುಂಪು == AUGROUP_ALL)
	findgroup = current_augroup;
    ಇಲ್ಲದಿದ್ದರೆ
	findgroup = ಗುಂಪು;
    allgroups = (ಗುಂಪು == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    ಆದರೆ (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    ಆದರೆ (forceit)  // delete the AutoPat, if it's in the current group
	    {
		ಆದರೆ (ap->ಗುಂಪು == findgroup)
		    au_remove_pat(ap);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಗುಂಪು == AUGROUP_ALL || ap->ಗುಂಪು == ಗುಂಪು)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    ಗಾಗಿ ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	ಗಾಗಿ (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    ಆದರೆ (*endpat == '{')
		brace_level++;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*endpat == '}')
		brace_level--;
	}
	ಆದರೆ (pat == endpat)		// ignore single comma
	    ಮುಂದುವರಿ;
	patlen = (ಪೂರ್ಣಾಂಕ)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	ಆದರೆ (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    ಆದರೆ (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (patlen > 9 && pat[7] == '=')
	    {
		ಆದರೆ (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((ಅಕ್ಷರ *)pat + 8);
	    }
	}

	ಆದರೆ (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((ಅಕ್ಷರ *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (ಪೂರ್ಣಾಂಕ)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	ಆದರೆ (!forceit && *cmd != NUL && ಕೊನೆಯ_autopat[(ಪೂರ್ಣಾಂಕ)event] != ಖಾಲಿ)
	    prev_ap = &ಕೊನೆಯ_autopat[(ಪೂರ್ಣಾಂಕ)event];
	ಇಲ್ಲದಿದ್ದರೆ
	    prev_ap = &ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)event];
	ಆಗ ((ap = *prev_ap) != ಖಾಲಿ)
	{
	    ಆದರೆ (ap->pat != ಖಾಲಿ)
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
		ಆದರೆ ((allgroups || ap->ಗುಂಪು == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    ಆದರೆ (forceit)
		    {
			ಆದರೆ (*cmd != NUL && ap->ಮುಂದಿನ == ಖಾಲಿ)
			{
			    au_remove_cmds(ap);
			    ನಿಲ್ಲಿಸು;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ap->ಮುಂದಿನ == ಖಾಲಿ)
			ನಿಲ್ಲಿಸು;
		}
	    }
	    prev_ap = &ap->ಮುಂದಿನ;
	}

	/*
	 * Add a new command.
	 */
	ಆದರೆ (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    ಆದರೆ (ap == ಖಾಲಿ)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		ಆದರೆ (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == ಖಾಲಿ))
		{
		    semsg(_(e_ಬಫರ್_nr_invalid_ಬಫರ್_ಸಂಖ್ಯೆ), buflocal_nr);
		    ಹಿಂತಿರುಗಿ FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		ಆದರೆ (ap == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		ಆದರೆ (ap->pat == ಖಾಲಿ)
		{
		    vim_free(ap);
		    ಹಿಂತಿರುಗಿ FAIL;
		}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		ಆದರೆ (event == EVENT_MODECHANGED && !has_modechanged())
		    get_ಬಹುಲಕ(ಕೊನೆಯ_ಬಹುಲಕ);
#ಆದರೆ_ಅಂತ್ಯ
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		ಆದರೆ ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
			&& !(has_winscrolled() || has_winresized()))
		{
		    tabpage_T *save_curtab = curtab;
		    tabpage_T *tp;
		    FOR_ALL_TABPAGES(tp)
		    {
			unuse_tabpage(curtab);
			use_tabpage(tp);
			snapshot_windows_scroll_ಗಾತ್ರ();
		    }
		    unuse_tabpage(curtab);
		    use_tabpage(save_curtab);
		}

		ಆದರೆ (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = ಖಾಲಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_ಗಮ್ಯ_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    ಆದರೆ (reg_pat != ಖಾಲಿ)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    ಆದರೆ (reg_pat == ಖಾಲಿ || ap->reg_prog == ಖಾಲಿ)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		}
		ap->cmds = ಖಾಲಿ;
		*prev_ap = ap;
		ಕೊನೆಯ_autopat[(ಪೂರ್ಣಾಂಕ)event] = ap;
		ap->ಮುಂದಿನ = ಖಾಲಿ;
		ಆದರೆ (ಗುಂಪು == AUGROUP_ALL)
		    ap->ಗುಂಪು = current_augroup;
		ಇಲ್ಲದಿದ್ದರೆ
		    ap->ಗುಂಪು = ಗುಂಪು;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    ಆಗ ((ac = *prev_ac) != ಖಾಲಿ)
		prev_ac = &ac->ಮುಂದಿನ;
	    ac = ALLOC_ONE(AutoCmd);
	    ಆದರೆ (ac == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    ಆದರೆ (flags & UC_VIM9)
		ac->script_ctx.sc_ಆವೃತ್ತಿ = SCRIPT_ಆವೃತ್ತಿ_VIM9;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (ac->cmd == ಖಾಲಿ)
	    {
		vim_free(ac);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ac->ಮುಂದಿನ = ಖಾಲಿ;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    ಪೂರ್ಣಾಂಕ
do_doautocmd(
    char_u	*arg_ಆರಂಭ,
    ಪೂರ್ಣಾಂಕ		do_ಸಂದೇಶ,	    // give message for no matching autocmds?
    ಪೂರ್ಣಾಂಕ		*did_something)
{
    char_u	*arg = arg_ಆರಂಭ;
    char_u	*fname;
    ಪೂರ್ಣಾಂಕ		nothing_ಮುಗಿದ = TRUE;
    ಪೂರ್ಣಾಂಕ		ಗುಂಪು;

    ಆದರೆ (did_something != ಖಾಲಿ)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    ಗುಂಪು = au_get_grouparg(&arg);
    ಆದರೆ (arg == ಖಾಲಿ)	    // out of memory
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = ಕಂಡುಹಿಡಿ_ಕೊನೆ_event(arg, ಗುಂಪು != AUGROUP_ALL);
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    ಆಗ (*arg && !ತುದಿಗಳು_excmd(*arg) && !VIM_ISWHITE(*arg))
	ಆದರೆ (apply_autocmds_ಗುಂಪು(event_name2nr(arg, &arg),
				      fname, ಖಾಲಿ, TRUE, ಗುಂಪು, curbuf, ಖಾಲಿ))
	    nothing_ಮುಗಿದ = FALSE;

    ಆದರೆ (nothing_ಮುಗಿದ && do_ಸಂದೇಶ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		&& !aborting()
#ಆದರೆ_ಅಂತ್ಯ
	       )
	smsg(_("No matching autocommands: %s"), arg_ಆರಂಭ);
    ಆದರೆ (did_something != ಖಾಲಿ)
	*did_something = !nothing_ಮುಗಿದ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಹಿಂತಿರುಗಿ aborting() ? FAIL : OK;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ OK;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    ಶೂನ್ಯ
ex_doautoall(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಂಕ		call_do_modelines = check_nomodeline(&arg);
    ಪೂರ್ಣಾಂಕ		did_aucmd;

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
	ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ || buf == curbuf)
	    ಮುಂದುವರಿ;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	ಆದರೆ (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    ನಿಲ್ಲಿಸು;
	}

	ಗಣ_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	ಆದರೆ (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	ಆದರೆ (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    ನಿಲ್ಲಿಸು;
	}
    }

    // Execute autocommands for the current buffer last.
    ಆದರೆ (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	ಆದರೆ (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    ಪೂರ್ಣಾಂಕ
check_nomodeline(char_u **argp)
{
    ಆದರೆ (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
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
    ಪೂರ್ಣಾಂಕ		save_ea;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_AUTOCHDIR
    ಪೂರ್ಣಾಂಕ		save_acd;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		same_ಬಫರ್ = buf == curbuf;

    // Find a window that is for the new buffer
    ಆದರೆ (same_ಬಫರ್)		// be quick when buf is curbuf
	win = curwin;
    ಇಲ್ಲದಿದ್ದರೆ
	FOR_ALL_WINDOWS(win)
	    ಆದರೆ (win->w_ಬಫರ್ == buf)
		ನಿಲ್ಲಿಸು;

    // Allocate a window when needed.
    win_T *auc_win = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ auc_idx = AUCMD_WIN_ಎಣಿಕೆ;
    ಆದರೆ (win == ಖಾಲಿ)
    {
	ಗಾಗಿ (auc_idx = 0; auc_idx < AUCMD_WIN_ಎಣಿಕೆ; ++auc_idx)
	    ಆದರೆ (!aucmd_win[auc_idx].auc_win_ಬಳಸಿದ)
	    {
		ಆದರೆ (aucmd_win[auc_idx].auc_win == ಖಾಲಿ)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		ಆದರೆ (auc_win != ಖಾಲಿ)
		    aucmd_win[auc_idx].auc_win_ಬಳಸಿದ = TRUE;
		ನಿಲ್ಲಿಸು;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	ಆದರೆ (auc_win == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == ಖಾಲಿ ? 0 : prevwin->w_id;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (bt_prompt(curbuf))
	aco->save_prompt_ಸೇರಿಸು = curbuf->b_prompt_ಸೇರಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (win != ಖಾಲಿ)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
	curtab->tp_localdir = ಖಾಲಿ;
	aco->globaldir = globaldir;
	globaldir = ಖಾಲಿ;

	// Split the current window, put the auc_win in the upper half.
	// We don't want the BufEnter or WinEnter autocommands.
	ಖಂಡ_autocmds();
	make_snapshot(SNAP_AUCMD_IDX);
	save_ea = p_ea;
	p_ea = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_AUTOCHDIR
	// Prevent chdir() call in win_enter_ext(), through do_autochdir().
	save_acd = p_acd;
	p_acd = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

	(ಶೂನ್ಯ)win_ವಿಭಜಿಸು_ins(0, WSP_ತುದಿ | WSP_FORCE_ಸ್ಥಳ, auc_win, 0, ಖಾಲಿ);
	win_comp_pos();   // recompute window positions
	p_ea = save_ea;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_AUTOCHDIR
	p_acd = save_acd;
#ಆದರೆ_ಅಂತ್ಯ
	unblock_autocmds();
	curwin = auc_win;
    }
    curbuf = buf;
    aco->new_curwin_id = curwin->w_id;
    ಗಣ_bufref(&aco->new_curbuf, curbuf);

    aco->save_VIsual_ಸಕ್ರಿಯ = VIsual_ಸಕ್ರಿಯ;
    ಆದರೆ (!same_ಬಫರ್)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_ಸಕ್ರಿಯ = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    ಶೂನ್ಯ
aucmd_restbuf(
    aco_save_T	*aco)		// structure holding saved values
{
    ಪೂರ್ಣಾಂಕ	    dummy;
    win_T   *save_curwin;

    ಆದರೆ (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	ಖಂಡ_autocmds();
	ಆದರೆ (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		ಆದರೆ (wp == awp)
		{
		    ಆದರೆ (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(awp);
		    ಹೋಗು win_ಸಿಕ್ಕಿತು;
		}
	    }
	}
win_ಸಿಕ್ಕಿತು:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(ಶೂನ್ಯ)winframe_remove(curwin, &dummy, ಖಾಲಿ, ಖಾಲಿ);
	win_remove(curwin, ಖಾಲಿ);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_ಬಳಸಿದ = FALSE;
	ಕೊನೆಯ_ಸ್ಥಿತಿ(FALSE);	    // may need to remove last status line

	ಆದರೆ (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_ಕಂಡುಹಿಡಿ_by_id(aco->save_curwin_id);
	ಆದರೆ (save_curwin != ಖಾಲಿ)
	    curwin = save_curwin;
	ಇಲ್ಲದಿದ್ದರೆ
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_ಬಫರ್;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_ಕಿಟಕಿ(curwin);
	ಆದರೆ (bt_prompt(curbuf))
	    curbuf->b_prompt_ಸೇರಿಸು = aco->save_prompt_ಸೇರಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	prevwin = win_ಕಂಡುಹಿಡಿ_by_id(aco->save_prevwin_id);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#ಆದರೆ_ಅಂತ್ಯ
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	ಆದರೆ (awp->w_localdir != ಖಾಲಿ)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_ಸಕ್ರಿಯ = aco->save_VIsual_ಸಕ್ರಿಯ;
	check_cursor();
	ಆದರೆ (curwin->w_topline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    curwin->w_topline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    curwin->w_topfill = 0;
#ಆದರೆ_ಅಂತ್ಯ
	}
#ಆದರೆ defined(FEAT_GUI)
	ಆದರೆ (gui.ಒಳ_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_ಎಡ], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_ಬಲ], FALSE);
	    gui_may_update_scrollbars();
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_ಕಂಡುಹಿಡಿ_by_id(aco->save_curwin_id);
	ಆದರೆ (save_curwin != ಖಾಲಿ)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    ಆದರೆ (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != ಖಾಲಿ)
	    {
#ಆದರೆ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		ಆದರೆ (curwin->w_s == &curbuf->b_s)
		    curwin->w_s = &aco->new_curbuf.br_buf->b_s;
#ಆದರೆ_ಅಂತ್ಯ
		--curbuf->b_nwindows;
		curbuf = aco->new_curbuf.br_buf;
		curwin->w_ಬಫರ್ = curbuf;
		++curbuf->b_nwindows;
	    }

	    curwin = save_curwin;
	    curbuf = curwin->w_ಬಫರ್;
	    prevwin = win_ಕಂಡುಹಿಡಿ_by_id(aco->save_prevwin_id);

	    // In case the autocommand moves the cursor to a position that
	    // does not exist in curbuf.
	    VIsual_ಸಕ್ರಿಯ = aco->save_VIsual_ಸಕ್ರಿಯ;
	    check_cursor();
	}
    }

    VIsual_ಸಕ್ರಿಯ = aco->save_VIsual_ಸಕ್ರಿಯ;
    check_cursor();	    // just in case lines got deleted
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	check_pos(curbuf, &VIsual);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    ಪೂರ್ಣಾಂಕ
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    ಪೂರ್ಣಾಂಕ		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    ಹಿಂತಿರುಗಿ apply_autocmds_ಗುಂಪು(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ಖಾಲಿ);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    ಪೂರ್ಣಾಂಕ
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    ಪೂರ್ಣಾಂಕ		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    ಹಿಂತಿರುಗಿ apply_autocmds_ಗುಂಪು(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    ಪೂರ್ಣಾಂಕ
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    ಪೂರ್ಣಾಂಕ		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    ಪೂರ್ಣಾಂಕ		*retval)    // pointer to caller's retval
{
    ಪೂರ್ಣಾಂಕ		did_cmd;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (should_abort(*retval))
	ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    did_cmd = apply_autocmds_ಗುಂಪು(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ಖಾಲಿ);
    ಆದರೆ (did_cmd
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    && aborting()
#ಆದರೆ_ಅಂತ್ಯ
	    )
	*retval = FAIL;
    ಹಿಂತಿರುಗಿ did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
has_cursorhold(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)(get_real_state() == ಬಹುಲಕ_ಸಾಮಾನ್ಯ_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != ಖಾಲಿ);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    ಪೂರ್ಣಾಂಕ
trigger_cursorhold(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		state;

    ಆದರೆ (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_ಸಕ್ರಿಯ())
    {
	state = get_real_state();
	ಆದರೆ (state == ಬಹುಲಕ_ಸಾಮಾನ್ಯ_BUSY || (state & ಬಹುಲಕ_ಸೇರಿಸು) != 0)
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_winresized(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_WINRESIZED] != ಖಾಲಿ);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_winscrolled(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_WINSCROLLED] != ಖಾಲಿ);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_cursormoved(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_CURSORMOVED] != ಖಾಲಿ);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_cursormovedI(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_CURSORMOVEDI] != ಖಾಲಿ);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_textchanged(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_TEXTCHANGED] != ಖಾಲಿ);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_textchangedI(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_TEXTCHANGEDI] != ಖಾಲಿ);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_textchangedP(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_TEXTCHANGEDP] != ಖಾಲಿ);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_insertcharpre(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_INSERTCHARPRE] != ಖಾಲಿ);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_keyinputpre(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_KEYINPUTPRE] != ಖಾಲಿ);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_cmdundefined(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_CMDUNDEFINED] != ಖಾಲಿ);
}

#ಆದರೆ defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_textputpost(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_TEXTPUTPOST] != ಖಾಲಿ);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_textputpre(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_TEXTPUTPRE] != ಖಾಲಿ);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_textyankpost(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_TEXTYANKPOST] != ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_completechanged(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_COMPLETECHANGED] != ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_modechanged(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_MODECHANGED] != ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
apply_autocmds_ಗುಂಪು(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    ಪೂರ್ಣಾಂಕ		force,	     // when TRUE, ignore autocmd_busy
    ಪೂರ್ಣಾಂಕ		ಗುಂಪು,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap ಬಳಸದ) // command arguments
{
    char_u	*sfname = ಖಾಲಿ;	// short file name
    char_u	*ಬಾಲ;
    ಪೂರ್ಣಾಂಕ		save_ಬದಲಾಯಿತು;
    buf_T	*old_curbuf;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;
    char_u	*save_autocmd_fname;
    ಪೂರ್ಣಾಂಕ		save_autocmd_fname_ತುಂಬಿದ;
    ಪೂರ್ಣಾಂಕ		save_autocmd_bufnr;
    char_u	*save_autocmd_ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ		save_autocmd_busy;
    ಪೂರ್ಣಾಂಕ		save_autocmd_nested;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    funccal_ನಮೂದು_T funccal_ನಮೂದು;
    char_u	*save_cmdarg;
    ದೀರ್ಘ	save_cmdbang;
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	filechangeshell_busy = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    proftime_T	wait_ಸಮಯ;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    ಪೂರ್ಣಾಂಕ		save_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    ಆದರೆ (event == ಅಂಕೆ_EVENTS || ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)event] == ಖಾಲಿ
	    || autocmd_blocked > 0)
	ಹೋಗು BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    ಆದರೆ (autocmd_busy && !(force || autocmd_nested))
	ಹೋಗು BYPASS_AU;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    ಆದರೆ (aborting())
	ಹೋಗು BYPASS_AU;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    ಆದರೆ (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	ಹೋಗು BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    ಆದರೆ (event_ignored(event, p_ei))
	ಹೋಗು BYPASS_AU;

    ಪೂರ್ಣಾಂಕ win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    ಆದರೆ (buf == curbuf && event_tab[event].ಕೀಲಿ <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf != ಖಾಲಿ && event_tab[event].ಕೀಲಿ <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಆದರೆ (wp->w_ಬಫರ್ == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		ನಿಲ್ಲಿಸು;
	    }
    }
    ಆದರೆ (win_ignore)
	ಹೋಗು BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    ಆದರೆ (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	ಹೋಗು BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    ಆದರೆ (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	ಹೋಗು BYPASS_AU;

    ಆದರೆ (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_ಬದಲಾಯಿತು_ಎಣಿಕೆ;

    /*
     * Save the autocmd_* variables and info about the current buffer.
     */
    save_autocmd_fname = autocmd_fname;
    save_autocmd_fname_ತುಂಬಿದ = autocmd_fname_ತುಂಬಿದ;
    save_autocmd_bufnr = autocmd_bufnr;
    save_autocmd_ಹೊಂದಿಕೆ = autocmd_ಹೊಂದಿಕೆ;
    save_autocmd_busy = autocmd_busy;
    save_autocmd_nested = autocmd_nested;
    save_ಬದಲಾಯಿತು = curbuf->b_ಬದಲಾಯಿತು;
    old_curbuf = curbuf;

    /*
     * Set the file name to be used for <afile>.
     * Make a copy to avoid that changing a buffer name or directory makes it
     * invalid.
     */
    ಆದರೆ (fname_io == ಖಾಲಿ)
    {
	ಆದರೆ (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fname != ಖಾಲಿ && !ತುದಿಗಳು_excmd(*fname))
	    autocmd_fname = fname;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf != ಖಾಲಿ)
	    autocmd_fname = buf->b_ffname;
	ಇಲ್ಲದಿದ್ದರೆ
	    autocmd_fname = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	autocmd_fname = fname_io;
    ಆದರೆ (autocmd_fname != ಖಾಲಿ)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_ತುಂಬಿದ = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    ಆದರೆ (buf == ಖಾಲಿ)
	autocmd_bufnr = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    ಆದರೆ (fname == ಖಾಲಿ || *fname == NUL)
    {
	ಆದರೆ (buf == ಖಾಲಿ)
	    fname = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಆದರೆ (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (buf->b_sfname != ಖಾಲಿ)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	ಆದರೆ (fname == ಖಾಲಿ)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	ಆದರೆ (event == EVENT_FILETYPE
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
	    autocmd_fname_ತುಂಬಿದ = TRUE; // don't expand it later
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    fname = ತುಂಬಿದ_Name_save(fname, FALSE);
    }
    ಆದರೆ (fname == ಖಾಲಿ)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	ಹೋಗು BYPASS_AU;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    ಆದರೆ (sfname != ಖಾಲಿ)
	forward_slash(sfname);
    forward_slash(fname);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    // remove version for correct match
    ಆದರೆ (sfname != ಖಾಲಿ)
	vms_remove_ಆವೃತ್ತಿ(sfname);
    vms_remove_ಆವೃತ್ತಿ(fname);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Set the name to be used for <amatch>.
     */
    autocmd_ಹೊಂದಿಕೆ = fname;


    // Don't redraw while doing autocommands.
    ++RedrawingDisabled;

    // name and lnum are filled in later
    estack_ತಳ್ಳು(ETYPE_AUCMD, ಖಾಲಿ, 0);
    ESTACK_CHECK_ಸಿದ್ಧತೆ;

    save_current_sctx = current_sctx;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
# ifdef FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
	prof_ಮಗು_enter(&wait_ಸಮಯ); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_ನಮೂದು);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    ಆದರೆ (!autocmd_busy)
    {
	save_ಹುಡುಕು_patterns();
	ಆದರೆ (!ins_compl_ಸಕ್ರಿಯ())
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
    ಆದರೆ (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    ಬಾಲ = gettail(fname);

    // Find first autocommand that matches
    CLEAR_ಕ್ಷೇತ್ರ(patcmd);
    patcmd.curpat = ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)event];
    patcmd.ಗುಂಪು = ಗುಂಪು;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.ಬಾಲ = ಬಾಲ;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_ಮುಂದಿನ_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    ಆದರೆ (patcmd.curpat != ಖಾಲಿ)
    {
	// add to active_apc_list
	patcmd.ಮುಂದಿನ = ಸಕ್ರಿಯ_apc_list;
	ಸಕ್ರಿಯ_apc_list = &patcmd;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (ದೀರ್ಘ)get_vim_var_nr(VV_CMDBANG);
	ಆದರೆ (eap != ಖಾಲಿ)
	{
	    save_cmdarg = ಗಣ_cmdarg(eap, ಖಾಲಿ);
	    ಗಣ_vim_var_nr(VV_CMDBANG, (ದೀರ್ಘ)eap->forceit);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    save_cmdarg = ಖಾಲಿ;	// avoid gcc warning
#ಆದರೆ_ಅಂತ್ಯ
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	ಗಾಗಿ (ap = patcmd.curpat; ap->ಮುಂದಿನ != ಖಾಲಿ; ap = ap->ಮುಂದಿನ)
	    ap->ಕೊನೆಯ = FALSE;
	ap->ಕೊನೆಯ = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	ಆದರೆ (nesting == 1)
	    check_lnums(TRUE);
	ಇಲ್ಲದಿದ್ದರೆ
	    check_lnums_nested(TRUE);

	ಪೂರ್ಣಾಂಕ save_did_emsg = did_emsg;
	ಪೂರ್ಣಾಂಕ save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(ಖಾಲಿ, getnextac, (ಶೂನ್ಯ *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	ಗಣ_pressedreturn(save_ex_pressedreturn);

	ಆದರೆ (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (eap != ಖಾಲಿ)
	{
	    (ಶೂನ್ಯ)ಗಣ_cmdarg(ಖಾಲಿ, save_cmdarg);
	    ಗಣ_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#ಆದರೆ_ಅಂತ್ಯ
	// delete from active_apc_list
	ಆದರೆ (ಸಕ್ರಿಯ_apc_list == &patcmd)	    // just in case
	    ಸಕ್ರಿಯ_apc_list = patcmd.ಮುಂದಿನ;
    }

    ಆದರೆ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_ತೆಗೆ();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_ತುಂಬಿದ = save_autocmd_fname_ತುಂಬಿದ;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_ಹೊಂದಿಕೆ = save_autocmd_ಹೊಂದಿಕೆ;
    current_sctx = save_current_sctx;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
	prof_ಮಗು_exit(&wait_ಸಮಯ);
# endif
#ಆದರೆ_ಅಂತ್ಯ
    ಕೀಲಿ_Typed = save_ಕೀಲಿ_Typed;
    vim_free(fname);
    vim_free(sfname);
    --nesting;		// see matching increment above

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.  Free any buffers in the au_pending_free_buf list and
     * free any windows in the au_pending_free_win list.
     */
    ಆದರೆ (!autocmd_busy)
    {
	restore_ಹುಡುಕು_patterns();
	ಆದರೆ (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	ಆಗ (au_pending_free_buf != ಖಾಲಿ)
	{
	    buf_T *b = au_pending_free_buf->b_ಮುಂದಿನ;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	ಆಗ (au_pending_free_win != ಖಾಲಿ)
	{
	    win_T *w = au_pending_free_win->w_ಮುಂದಿನ;

	    vim_free(au_pending_free_win);
	    au_pending_free_win = w;
	}
    }

    /*
     * Some events don't set or reset the Changed flag.
     * Check if still in the same buffer!
     */
    ಆದರೆ (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	ಆದರೆ (curbuf->b_ಬದಲಾಯಿತು != save_ಬದಲಾಯಿತು)
	    need_maketitle = TRUE;
	curbuf->b_ಬದಲಾಯಿತು = save_ಬದಲಾಯಿತು;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    ಆದರೆ (event == EVENT_BUFWIPEOUT && buf != ಖಾಲಿ)
	aubuflocal_remove(buf);

    ಆದರೆ (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    ಹಿಂತಿರುಗಿ retval;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ char_u	*old_termresponse = ಖಾಲಿ;
ಸ್ಥಿರ char_u	*old_termu7resp = ಖಾಲಿ;
ಸ್ಥಿರ char_u	*old_termblinkresp = ಖಾಲಿ;
ಸ್ಥಿರ char_u	*old_termrbgresp = ಖಾಲಿ;
ಸ್ಥಿರ char_u	*old_termrfgresp = ಖಾಲಿ;
ಸ್ಥಿರ char_u	*old_termstyleresp = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    ಶೂನ್ಯ
ಖಂಡ_autocmds(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // Remember the value of v:termresponse.
    ಆದರೆ (autocmd_blocked == 0)
    {
	old_termresponse = get_vim_var_str(VV_TERMRESPONSE);
	old_termu7resp = get_vim_var_str(VV_TERMU7RESP);
	old_termblinkresp = get_vim_var_str(VV_TERMBLINKRESP);
	old_termrbgresp = get_vim_var_str(VV_TERMRBGRESP);
	old_termrfgresp = get_vim_var_str(VV_TERMRFGRESP);
	old_termstyleresp = get_vim_var_str(VV_TERMSTYLERESP);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ++autocmd_blocked;
}

    ಶೂನ್ಯ
unblock_autocmds(ಶೂನ್ಯ)
{
    --autocmd_blocked;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    ಆದರೆ (autocmd_blocked == 0)
    {
	ಆದರೆ (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", ಖಾಲಿ, FALSE, curbuf);
	}
	ಆದರೆ (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", ಖಾಲಿ, FALSE, curbuf);
	}
	ಆದರೆ (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", ಖಾಲಿ, FALSE, curbuf);
	}
	ಆದರೆ (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", ಖಾಲಿ, FALSE, curbuf);
	}
	ಆದರೆ (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", ಖಾಲಿ, FALSE, curbuf);
	}
	ಆದರೆ (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", ಖಾಲಿ, FALSE, curbuf);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
}

    ಪೂರ್ಣಾಂಕ
is_autocmd_blocked(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
auto_ಮುಂದಿನ_pat(
    AutoPatCmd_T *apc,
    ಪೂರ್ಣಾಂಕ		stop_at_ಕೊನೆಯ)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*ನಮೂದು;

    ನಮೂದು = ((estack_T *)exestack.ga_ದತ್ತಾಂಶ) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(ನಮೂದು->es_name);
    ನಮೂದು->es_info.aucmd = ಖಾಲಿ;

    ಗಾಗಿ (ap = apc->curpat; ap != ಖಾಲಿ && !got_int; ap = ap->ಮುಂದಿನ)
    {
	apc->curpat = ಖಾಲಿ;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	ಆದರೆ (ap->pat != ಖಾಲಿ && ap->cmds != ಖಾಲಿ
		&& (apc->ಗುಂಪು == AUGROUP_ALL || apc->ಗುಂಪು == ap->ಗುಂಪು))
	{
	    // execution-condition
	    ಆದರೆ (ap->buflocal_nr == 0
		    ? (ಹೊಂದಿಕೆ_file_pat(ಖಾಲಿ, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->ಬಾಲ, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		ಅಕ್ಷರ	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->ಉದ್ದಳತೆ + ap->patlen + 1);
		ಆದರೆ (namep != ಖಾಲಿ)
		{
		    sprintf((ಅಕ್ಷರ *)namep, fmt, (ಅಕ್ಷರ *)event_name->string, (ಅಕ್ಷರ *)ap->pat);
		    ಆದರೆ (p_verbose >= 8)
		    {
			verbose_enter();
			smsg(_("Executing %s"), namep);
			verbose_leave();
		    }
		}

		// Update the exestack entry for this autocmd.
		ನಮೂದು->es_name = namep;
		ನಮೂದು->es_info.aucmd = apc;

		apc->curpat = ap;
		apc->nextcmd = ap->cmds;
		// mark last command
		ಗಾಗಿ (cp = ap->cmds; cp->ಮುಂದಿನ != ಖಾಲಿ; cp = cp->ಮುಂದಿನ)
		    cp->ಕೊನೆಯ = FALSE;
		cp->ಕೊನೆಯ = TRUE;
	    }
	    ಸಾಲು_breakcheck();
	    ಆದರೆ (apc->curpat != ಖಾಲಿ)	    // found a match
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (stop_at_ಕೊನೆಯ && ap->ಕೊನೆಯ)
	    ನಿಲ್ಲಿಸು;
    }
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    ಹಿಂತಿರುಗಿ &acp->script_ctx;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	ಪೂರ್ಣಾಂಕ c ಬಳಸದ,
	ಶೂನ್ಯ *cookie,
	ಪೂರ್ಣಾಂಕ indent ಬಳಸದ,
	getline_opt_T options ಬಳಸದ)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    ಆದರೆ (acp->curpat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // repeat until we find an autocommand to execute
    ಗಾಗಿ (;;)
    {
	// skip removed commands
	ಆಗ (acp->nextcmd != ಖಾಲಿ && acp->nextcmd->cmd == ಖಾಲಿ)
	    ಆದರೆ (acp->nextcmd->ಕೊನೆಯ)
		acp->nextcmd = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
		acp->nextcmd = acp->nextcmd->ಮುಂದಿನ;

	ಆದರೆ (acp->nextcmd != ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	// at end of commands, find next pattern that matches
	ಆದರೆ (acp->curpat->ಕೊನೆಯ)
	    acp->curpat = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    acp->curpat = acp->curpat->ಮುಂದಿನ;
	ಆದರೆ (acp->curpat != ಖಾಲಿ)
	    auto_ಮುಂದಿನ_pat(acp, TRUE);
	ಆದರೆ (acp->curpat == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ac = acp->nextcmd;

    ಆದರೆ (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	ಸಂದೇಶ_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    ಆದರೆ (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    ಆದರೆ (ac->ಕೊನೆಯ)
	acp->nextcmd = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	acp->nextcmd = ac->ಮುಂದಿನ;
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_ತುಂಬಿದ)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*ಬಾಲ = gettail(sfname);
    ಪೂರ್ಣಾಂಕ		retval = FALSE;

    ಆದರೆ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)event] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    fname = is_ತುಂಬಿದ ? vim_strsave(sfname) : ತುಂಬಿದ_Name_save(sfname, FALSE);
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    ಆದರೆ (sfname != ಖಾಲಿ)
	forward_slash(sfname);
    forward_slash(fname);
#ಆದರೆ_ಅಂತ್ಯ

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	ಆದರೆ (ap->pat != ಖಾಲಿ && ap->cmds != ಖಾಲಿ
	      && (ap->buflocal_nr == 0
		? ಹೊಂದಿಕೆ_file_pat(ಖಾಲಿ, &ap->reg_prog,
					  fname, sfname, ಬಾಲ, ap->allow_dirs)
		: buf != ಖಾಲಿ && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    ನಿಲ್ಲಿಸು;
	}

    vim_free(fname);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    vim_free(sfname);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    ಪೂರ್ಣಾಂಕ
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    ಹಿಂತಿರುಗಿ has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    ಪೂರ್ಣಾಂಕ
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    ಹಿಂತಿರುಗಿ has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx == augroups.ga_len)		// add "END" add the end
	ಹಿಂತಿರುಗಿ (char_u *)"END";
    ಆದರೆ (idx < 0 || idx >= augroups.ga_len)	// end of list
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (AUGROUP_NAME(idx) == ಖಾಲಿ || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	ಹಿಂತಿರುಗಿ (char_u *)"";
    ಹಿಂತಿರುಗಿ AUGROUP_NAME(idx);		// return a name
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ include_groups = FALSE;

    char_u  *
ಗಣ_context_ಒಳ_autocmd(
    expand_T	*xp,
    char_u	*arg,
    ಪೂರ್ಣಾಂಕ		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		ಗುಂಪು;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    ಗುಂಪು = au_get_grouparg(&arg);
    ಆದರೆ (ಗುಂಪು == AUGROUP_ERROR)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    // If there only is a group name that's what we expand.
    ಆದರೆ (*arg == NUL && ಗುಂಪು != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	ಗುಂಪು = AUGROUP_ALL;
    }

    // skip over event name
    ಗಾಗಿ (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	ಆದರೆ (*p == ',')
	    arg = p + 1;
    ಆದರೆ (*p == NUL)
    {
	ಆದರೆ (ಗುಂಪು == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_ನಮೂನೆ = arg;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // skip over pattern
    arg = skipwhite(p);
    ಆಗ (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    ಆದರೆ (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 2; i++)
	{
	    ಆದರೆ (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    ಆದರೆ (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    ಆದರೆ (!ಒಳ_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	ಹಿಂತಿರುಗಿ arg;			    // expand (next) command
    }

    ಆದರೆ (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    ಇಲ್ಲದಿದ್ದರೆ
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (idx < augroups.ga_len)		// First list group names, if wanted
    {
	ಆದರೆ (!include_groups || AUGROUP_NAME(idx) == ಖಾಲಿ
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    ಹಿಂತಿರುಗಿ (char_u *)"";	// skip deleted entries
	ಹಿಂತಿರುಗಿ AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    ಆದರೆ (i < 0 || i >= ಅಂಕೆ_EVENTS)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ event_tab[i].ಮೌಲ್ಯ.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_ಗುಂಪು(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ win)
{
    ಆದರೆ (idx < 0 || idx >= ಅಂಕೆ_EVENTS)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (!win)
	ಹಿಂತಿರುಗಿ event_tab[idx].ಮೌಲ್ಯ.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    ಪೂರ್ಣಾಂಕ j = 0;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_EVENTS; ++i)
    {
	j += event_tab[i].ಕೀಲಿ <= 0;
	ಆದರೆ (j == idx + 1)
	    ಹಿಂತಿರುಗಿ event_tab[i].ಮೌಲ್ಯ.string;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    ಪೂರ್ಣಾಂಕ
has_tabclosedpre(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)EVENT_TABCLOSEDPRE] != ಖಾಲಿ);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    ಪೂರ್ಣಾಂಕ
autocmd_supported(char_u *name)
{
    char_u *p;

    ಹಿಂತಿರುಗಿ (event_name2nr(name, &p) != ಅಂಕೆ_EVENTS);
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
    ಪೂರ್ಣಾಂಕ
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*ನಮೂನೆ = ಖಾಲಿ;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಗುಂಪು;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    ಆದರೆ (arg_save == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    p = vim_strchr(arg_save, '#');
    ಆದರೆ (p != ಖಾಲಿ)
	*p++ = NUL;

    // First, look for an autocmd group name
    ಗುಂಪು = au_ಕಂಡುಹಿಡಿ_ಗುಂಪು(arg_save);
    ಆದರೆ (ಗುಂಪು == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	ಗುಂಪು = AUGROUP_ALL;
	event_name = arg_save;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    ಹೋಗು theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	ಆದರೆ (p != ಖಾಲಿ)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    ನಮೂನೆ = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    ಆದರೆ (event == ಅಂಕೆ_EVENTS)
	ಹೋಗು theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = ಮೊದಲ_autopat[(ಪೂರ್ಣಾಂಕ)event];
    ಆದರೆ (ap == ಖಾಲಿ)
	ಹೋಗು theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    ಆದರೆ (ನಮೂನೆ != ಖಾಲಿ && STRICMP(ನಮೂನೆ, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    ಗಾಗಿ ( ; ap != ಖಾಲಿ; ap = ap->ಮುಂದಿನ)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	ಆದರೆ (ap->pat != ಖಾಲಿ && ap->cmds != ಖಾಲಿ
	    && (ಗುಂಪು == AUGROUP_ALL || ap->ಗುಂಪು == ಗುಂಪು)
	    && (ನಮೂನೆ == ಖಾಲಿ
		|| (buflocal_buf == ಖಾಲಿ
		    ? fnamecmp(ap->pat, ನಮೂನೆ) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    ನಿಲ್ಲಿಸು;
	}

theend:
    vim_free(arg_save);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
autocmd_ಕೂಡಿಸು_or_delete(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = ಖಾಲಿ;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*ಗುಂಪು_name = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಗುಂಪು;
    char_u	*pat = ಖಾಲಿ;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = ಖಾಲಿ;
    char_u	*ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		once;
    ಪೂರ್ಣಾಂಕ		nested;
    ಪೂರ್ಣಾಂಕ		replace;		// replace the cmd for a group/event
    ಪೂರ್ಣಾಂಕ		retval = VVAL_TRUE;
    ಪೂರ್ಣಾಂಕ		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_list_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    aucmd_list = argvars[0].vval.v_list;
    ಆದರೆ (aucmd_list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(aucmd_list, li)
    {
	VIM_CLEAR(ಗುಂಪು_name);
	VIM_CLEAR(cmd);
	event_name = ಖಾಲಿ;
	event_list = ಖಾಲಿ;
	pat = ಖಾಲಿ;
	pat_list = ಖಾಲಿ;

	ಆದರೆ (li->li_tv.v_type != VAR_DICT)
	    ಮುಂದುವರಿ;

	event_dict = li->li_tv.vval.v_dict;
	ಆದರೆ (event_dict == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	di = dict_ಕಂಡುಹಿಡಿ(event_dict, (char_u *)"event", -1);
	ಆದರೆ (di != ಖಾಲಿ)
	{
	    ಆದರೆ (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		ಆದರೆ (event_name == ಖಾಲಿ)
		{
		    emsg(_(e_string_required));
		    ಮುಂದುವರಿ;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		ಆದರೆ (event_list == ಖಾಲಿ)
		{
		    emsg(_(e_list_required));
		    ಮುಂದುವರಿ;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		emsg(_(e_string_or_list_expected));
		ಮುಂದುವರಿ;
	    }
	}

	ಗುಂಪು_name = dict_get_string(event_dict, "group", TRUE);
	ಆದರೆ (ಗುಂಪು_name == ಖಾಲಿ || *ಗುಂಪು_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    ಗುಂಪು = current_augroup;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗುಂಪು = au_ಕಂಡುಹಿಡಿ_ಗುಂಪು(ಗುಂಪು_name);
	    ಆದರೆ (ಗುಂಪು == AUGROUP_ERROR)
	    {
		ಆದರೆ (delete)
		{
		    semsg(_(e_no_such_ಗುಂಪು_str), ಗುಂಪು_name);
		    retval = VVAL_FALSE;
		    ನಿಲ್ಲಿಸು;
		}
		// group is not found, create it now
		ಗುಂಪು = au_new_ಗುಂಪು(ಗುಂಪು_name);
		ಆದರೆ (ಗುಂಪು == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_ಗುಂಪು_str), ಗುಂಪು_name);
		    retval = VVAL_FALSE;
		    ನಿಲ್ಲಿಸು;
		}

		current_augroup = ಗುಂಪು;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	ಆದರೆ (dict_has_ಕೀಲಿ(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_ಸಂಖ್ಯೆ_def(event_dict, "bufnr", -1);
	    ಆದರೆ (bnum == -1)
		ಮುಂದುವರಿ;

	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "<buffer=%d>", (ಪೂರ್ಣಾಂಕ)bnum);
	    pat = IObuff;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    di = dict_ಕಂಡುಹಿಡಿ(event_dict, (char_u *)"pattern", -1);
	    ಆದರೆ (di != ಖಾಲಿ)
	    {
		ಆದರೆ (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    ಆದರೆ (pat == ಖಾಲಿ)
		    {
			emsg(_(e_string_required));
			ಮುಂದುವರಿ;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    ಆದರೆ (pat_list == ಖಾಲಿ)
		    {
			emsg(_(e_list_required));
			ಮುಂದುವರಿ;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    emsg(_(e_string_or_list_expected));
		    ಮುಂದುವರಿ;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	ಆದರೆ (cmd == ಖಾಲಿ)
	{
	    ಆದರೆ (delete)
		cmd = vim_strsave((char_u *)"");
	    ಇಲ್ಲದಿದ್ದರೆ
		ಮುಂದುವರಿ;
	}

	ಆದರೆ (delete && (event_name == ಖಾಲಿ
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    ಗಾಗಿ (event = (event_T)0; (ಪೂರ್ಣಾಂಕ)event < ಅಂಕೆ_EVENTS;
		    event = (event_T)((ಪೂರ್ಣಾಂಕ)event + 1))
	    {
		ಆದರೆ (do_autocmd_event(event, pat, once, nested, cmd, delete,
							ಗುಂಪು, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u *p = ಖಾಲಿ;

	    eli = ಖಾಲಿ;
	    ಕೊನೆ = ಖಾಲಿ;
	    ಆಗ (TRUE)
	    {
		ಆದರೆ (event_list != ಖಾಲಿ)
		{
		    ಆದರೆ (eli == ಖಾಲಿ)
			eli = event_list->lv_ಮೊದಲ;
		    ಇಲ್ಲದಿದ್ದರೆ
			eli = eli->li_ಮುಂದಿನ;
		    ಆದರೆ (eli == ಖಾಲಿ)
			ನಿಲ್ಲಿಸು;
		    ಆದರೆ (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == ಖಾಲಿ)
		    {
			emsg(_(e_string_required));
			ನಿಲ್ಲಿಸು;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (p == ಖಾಲಿ)
			p = event_name;
		    ಆದರೆ (p == ಖಾಲಿ || *p == NUL)
			ನಿಲ್ಲಿಸು;
		}

		event = event_name2nr(p, &ಕೊನೆ);
		ಆದರೆ (event == ಅಂಕೆ_EVENTS || *ಕೊನೆ != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (pat != ಖಾಲಿ)
		{
		    ಆದರೆ (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, ಗುಂಪು, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			ನಿಲ್ಲಿಸು;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pat_list != ಖಾಲಿ)
		{
		    FOR_ALL_LIST_ವಸ್ತುಗಳು(pat_list, pli)
		    {
			ಆದರೆ (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == ಖಾಲಿ)
			{
			    emsg(_(e_string_required));
			    ಮುಂದುವರಿ;
			}
			ಆದರೆ (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, ಗುಂಪು, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    ನಿಲ್ಲಿಸು;
			}
		    }
		    ಆದರೆ (retval == VVAL_FALSE)
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (event_name != ಖಾಲಿ)
		    p = ಕೊನೆ;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	ಆದರೆ (delete && ಗುಂಪು_name != ಖಾಲಿ &&
		(event_name == ಖಾಲಿ || event_name[0] == NUL)
		&& (pat == ಖಾಲಿ || pat[0] == NUL)
		&& (cmd == ಖಾಲಿ || cmd[0] == NUL))
	    au_del_ಗುಂಪು(ಗುಂಪು_name);
    }

    VIM_CLEAR(ಗುಂಪು_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_ಸಂಖ್ಯೆ = retval;
}

/*
 * autocmd_add() function
 */
    ಶೂನ್ಯ
f_autocmd_ಕೂಡಿಸು(typval_T *argvars, typval_T *rettv)
{
    autocmd_ಕೂಡಿಸು_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    ಶೂನ್ಯ
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_ಕೂಡಿಸು_or_delete(argvars, rettv, TRUE);
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
    char_u	*pat = ಖಾಲಿ;
    char_u	*name = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಗುಂಪು = AUGROUP_ALL;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	ಆದರೆ (dict_has_ಕೀಲಿ(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    ಆದರೆ (name == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;

	    ಆದರೆ (*name == NUL)
		ಗುಂಪು = AUGROUP_DEFAULT;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗುಂಪು = au_ಕಂಡುಹಿಡಿ_ಗುಂಪು(name);
		ಆದರೆ (ಗುಂಪು == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_ಗುಂಪು_str), name);
		    vim_free(name);
		    ಹಿಂತಿರುಗಿ;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	ಆದರೆ (dict_has_ಕೀಲಿ(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    ಆದರೆ (name == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;

	    ಆದರೆ (name[0] == '*' && name[1] == NUL)
		event_arg = ಅಂಕೆ_EVENTS;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		keyvalue_T ಗುರಿ;
		keyvalue_T *ನಮೂದು;

		ಗುರಿ.ಕೀಲಿ = 0;
		ಗುರಿ.ಮೌಲ್ಯ.string = name;
		ಗುರಿ.ಮೌಲ್ಯ.ಉದ್ದಳತೆ = STRLEN(ಗುರಿ.ಮೌಲ್ಯ.string);
		ನಮೂದು = (keyvalue_T *)bsearch(&ಗುರಿ, &event_tab,
		    ಅಂಕೆ_EVENTS, ಗಾತ್ರ(event_tab[0]), cmp_keyvalue_ಮೌಲ್ಯ_ni);
		ಆದರೆ (ನಮೂದು == ಖಾಲಿ)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    ಹಿಂತಿರುಗಿ;
		}
		event_arg = (event_T)ನಿರಪೇಕ್ಷ(ನಮೂದು->ಕೀಲಿ);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	ಆದರೆ (dict_has_ಕೀಲಿ(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    ಆದರೆ (pat == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    ಗಾಗಿ (event = (event_T)0; (ಪೂರ್ಣಾಂಕ)event < ಅಂಕೆ_EVENTS;
	    event = (event_T)((ಪೂರ್ಣಾಂಕ)event + 1))
    {
	string_T    *event_name;

	ಆದರೆ (event_arg != ಅಂಕೆ_EVENTS && event != event_arg)
	    ಮುಂದುವರಿ;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	ಗುಂಪು_name;

	    ಆದರೆ (ap->pat == ಖಾಲಿ)		// pattern has been removed
		ಮುಂದುವರಿ;

	    ಆದರೆ (ಗುಂಪು != AUGROUP_ALL && ಗುಂಪು != ap->ಗುಂಪು)
		ಮುಂದುವರಿ;

	    ಆದರೆ (pat != ಖಾಲಿ && STRCMP(pat, ap->pat) != 0)
		ಮುಂದುವರಿ;

	    ಗುಂಪು_name.string = get_augroup_name(ಖಾಲಿ, ap->ಗುಂಪು);
	    ಆದರೆ (ಗುಂಪು_name.string == ಖಾಲಿ)
		STR_LITERAL_ಗಣ(ಗುಂಪು_name, "");
	    ಇಲ್ಲದಿದ್ದರೆ
		ಗುಂಪು_name.ಉದ್ದಳತೆ = STRLEN(ಗುಂಪು_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    ಗಾಗಿ (ac = ap->cmds; ac != ಖಾಲಿ; ac = ac->ಮುಂದಿನ)
	    {
		event_dict = dict_alloc();
		ಆದರೆ (event_dict == ಖಾಲಿ
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    ಹಿಂತಿರುಗಿ;
		}

		ಆದರೆ (dict_ಕೂಡಿಸು_string_len(event_dict, "event",
			event_name->string, (ಪೂರ್ಣಾಂಕ)event_name->ಉದ್ದಳತೆ) == FAIL
			|| dict_ಕೂಡಿಸು_string_len(event_dict, "group",
			    ಗುಂಪು_name.string, (ಪೂರ್ಣಾಂಕ)ಗುಂಪು_name.ಉದ್ದಳತೆ) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_ಕೂಡಿಸು_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_ಕೂಡಿಸು_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_ಕೂಡಿಸು_bool(event_dict, "once", ac->once) == FAIL
			|| dict_ಕೂಡಿಸು_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    ಹಿಂತಿರುಗಿ;
		}
	    }
	}
    }

    vim_free(pat);
}

#ಆದರೆ_ಅಂತ್ಯ
