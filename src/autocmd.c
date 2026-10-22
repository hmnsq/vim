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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    ਅੱਖਰ	    once;		// "One shot": removed after execution
    ਅੱਖਰ	    nested;		// If autocommands nest here.
    ਅੱਖਰ	    ਆਖਰੀ;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    ਬਣਤਰ AutoCmd  *ਅਗਲਾ;		// next AutoCmd in list
} AutoCmd;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ AutoPat
{
    ਬਣਤਰ AutoPat  *ਅਗਲਾ;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    ਪੂਰਨ		    ਸਮੂਹ;		// group ID
    ਪੂਰਨ		    patlen;		// strlen() of pat
    ਪੂਰਨ		    buflocal_nr;	// !=0 for buffer-local AutoPat
    ਅੱਖਰ	    allow_dirs;		// Pattern may match whole path
    ਅੱਖਰ	    ਆਖਰੀ;		// last pattern for apply_autocmds()
} AutoPat;

//
// special cases:
// BufNewFile and BufRead are searched for ALOT (especially at startup)
// so we pre-determine their index into the event_tab[] table for fast access.
// Keep these values in sync with event_tab[]!
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFNEWFILE_ਸੂਚਕਾਂਕ 9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFREAD_ਸੂਚਕਾਂਕ 10

// Must be sorted by the 'value' field because it is used by bsearch()!
// Events with positive keys aren't allowed in 'eventignorewin'.
ਸਥਿਰ keyvalue_T event_tab[ਅੰਕ_EVENTS] = {
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFADD, "BufAdd"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFADD, "BufCreate"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFDELETE, "BufDelete"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFENTER, "BufEnter"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFFILEPOST, "BufFilePost"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFFILEPRE, "BufFilePre"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFHIDDEN, "BufHidden"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFLEAVE, "BufLeave"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFNEW, "BufNew"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFNEWFILE, "BufNewFile"),	// BUFNEWFILE_INDEX
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFREADPOST, "BufRead"),	// BUFREAD_INDEX
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFREADCMD, "BufReadCmd"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFREADPOST, "BufReadPost"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFREADPRE, "BufReadPre"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFUNLOAD, "BufUnload"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFWINENTER, "BufWinEnter"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFWINLEAVE, "BufWinLeave"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFWIPEOUT, "BufWipeout"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFWRITEPRE, "BufWrite"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFWRITECMD, "BufWriteCmd"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFWRITEPOST, "BufWritePost"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_BUFWRITEPRE, "BufWritePre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_CMDLINECHANGED, "CmdlineChanged"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_CMDLINEENTER, "CmdlineEnter"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_CMDLINELEAVE, "CmdlineLeave"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_CMDLINELEAVEPRE, "CmdlineLeavePre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_CMDUNDEFINED, "CmdUndefined"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_CMDWINENTER, "CmdwinEnter"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_CMDWINLEAVE, "CmdwinLeave"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_COLORSCHEME, "ColorScheme"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_COLORSCHEMEPRE, "ColorSchemePre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_COMPLETECHANGED, "CompleteChanged"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_COMPLETEDONE, "CompleteDone"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_COMPLETEDONEPRE, "CompleteDonePre"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_CURSORHOLD, "CursorHold"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_CURSORHOLDI, "CursorHoldI"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_CURSORMOVED, "CursorMoved"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_CURSORMOVEDC, "CursorMovedC"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_CURSORMOVEDI, "CursorMovedI"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_DIFFUPDATED, "DiffUpdated"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_DIRCHANGED, "DirChanged"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_DIRCHANGEDPRE, "DirChangedPre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_ENCODINGCHANGED, "EncodingChanged"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_EXITPRE, "ExitPre"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILEAPPENDCMD, "FileAppendCmd"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILEAPPENDPOST, "FileAppendPost"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILEAPPENDPRE, "FileAppendPre"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILECHANGEDRO, "FileChangedRO"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILECHANGEDSHELL, "FileChangedShell"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILECHANGEDSHELLPOST, "FileChangedShellPost"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_ENCODINGCHANGED, "FileEncoding"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILEREADCMD, "FileReadCmd"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILEREADPOST, "FileReadPost"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILEREADPRE, "FileReadPre"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILETYPE, "FileType"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILEWRITECMD, "FileWriteCmd"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILEWRITEPOST, "FileWritePost"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILEWRITEPRE, "FileWritePre"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILTERREADPOST, "FilterReadPost"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILTERREADPRE, "FilterReadPre"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILTERWRITEPOST, "FilterWritePost"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_FILTERWRITEPRE, "FilterWritePre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_FOCUSGAINED, "FocusGained"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_FOCUSLOST, "FocusLost"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_FUNCUNDEFINED, "FuncUndefined"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_GUIENTER, "GUIEnter"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_GUIFAILED, "GUIFailed"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_INSERTCHANGE, "InsertChange"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_INSERTCHARPRE, "InsertCharPre"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_INSERTENTER, "InsertEnter"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_INSERTLEAVE, "InsertLeave"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_INSERTLEAVEPRE, "InsertLeavePre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_KEYINPUTPRE, "KeyInputPre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_MENUPOPUP, "MenuPopup"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_MODECHANGED, "ModeChanged"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_OPTIONSET, "OptionSet"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_QUICKFIXCMDPOST, "QuickFixCmdPost"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_QUICKFIXCMDPRE, "QuickFixCmdPre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_QUITPRE, "QuitPre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_REMOTEREPLY, "RemoteReply"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_SAFESTATE, "SafeState"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_SAFESTATEAGAIN, "SafeStateAgain"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_SESSIONLOADPOST, "SessionLoadPost"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_SESSIONLOADPRE, "SessionLoadPre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_SESSIONWRITEPOST, "SessionWritePost"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_SHELLCMDPOST, "ShellCmdPost"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_SHELLFILTERPOST, "ShellFilterPost"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_SIGUSR1, "SigUSR1"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_SOURCECMD, "SourceCmd"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_SOURCEPOST, "SourcePost"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_SOURCEPRE, "SourcePre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_SPELLFILEMISSING, "SpellFileMissing"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_STDINREADPOST, "StdinReadPost"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_STDINREADPRE, "StdinReadPre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_SWAPEXISTS, "SwapExists"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_SYNTAX, "Syntax"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_TABCLOSED, "TabClosed"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_TABCLOSEDPRE, "TabClosedPre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_TABENTER, "TabEnter"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_TABLEAVE, "TabLeave"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_TABNEW, "TabNew"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_TERMCHANGED, "TermChanged"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_TERMINALOPEN, "TerminalOpen"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_TERMINALWINOPEN, "TerminalWinOpen"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_TERMRESPONSE, "TermResponse"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_TERMRESPONSEALL, "TermResponseAll"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_TEXTCHANGED, "TextChanged"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_TEXTCHANGEDI, "TextChangedI"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_TEXTCHANGEDP, "TextChangedP"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_TEXTCHANGEDT, "TextChangedT"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_TEXTPUTPOST, "TextPutPost"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_TEXTPUTPRE, "TextPutPre"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_TEXTYANKPOST, "TextYankPost"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_USER, "User"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_VIMENTER, "VimEnter"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_VIMLEAVE, "VimLeave"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_VIMLEAVEPRE, "VimLeavePre"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_VIMRESIZED, "VimResized"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_VIMRESUME, "VimResume"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_VIMSUSPEND, "VimSuspend"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_WINCLOSED, "WinClosed"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_WINENTER, "WinEnter"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_WINLEAVE, "WinLeave"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_WINNEW, "WinNew"),
    KEYVALUE_ਇੰਦਰਾਜ(EVENT_WINNEWPRE, "WinNewPre"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_WINRESIZED, "WinResized"),
    KEYVALUE_ਇੰਦਰਾਜ(-EVENT_WINSCROLLED, "WinScrolled"),
};

ਸਥਿਰ AutoPat *ਪਹਿਲਾ_autopat[ਅੰਕ_EVENTS] = { ਨਿਰਾ };
ਸਥਿਰ AutoPat *ਆਖਰੀ_autopat[ਅੰਕ_EVENTS] = { ਨਿਰਾ };

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AUGROUP_DEFAULT    (-1)	    // default autocmd group
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
ਬਣਤਰ AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    ਪੂਰਨ		ਸਮੂਹ;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*ਪੂਛ;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    ਪੂਰਨ		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *ਅਗਲਾ;		// chain of active apc-s for auto-invalidation
};

ਸਥਿਰ AutoPatCmd_T *ਸਰਗਰਮ_apc_list = ਨਿਰਾ; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    ਲਈ ((ap) = ਪਹਿਲਾ_autopat[(ਪੂਰਨ)(event)]; (ap) != ਨਿਰਾ; (ap) = (ap)->ਅਗਲਾ)

/*
 * augroups stores a list of autocmd group names.
 */
ਸਥਿਰ garray_T augroups = {0, 0, ਆਕਾਰ(char_u *), 10, ਨਿਰਾ};
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AUGROUP_NAME(i) (((char_u **)augroups.ga_ਅੰਕੜੇ)[i])
// use get_deleted_augroup() to get this
ਸਥਿਰ char_u *deleted_augroup = ਨਿਰਾ;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
ਸਥਿਰ ਪੂਰਨ current_augroup = AUGROUP_DEFAULT;

ਸਥਿਰ ਪੂਰਨ au_need_clean = FALSE;   // need to delete marked patterns

ਸਥਿਰ event_T event_name2nr(char_u *ਸ਼ੁਰੂ, char_u **ਸਿਰਾ);
ਸਥਿਰ string_T *event_nr2name(event_T event);
ਸਥਿਰ ਪੂਰਨ au_get_grouparg(char_u **argp);
ਸਥਿਰ ਪੂਰਨ do_autocmd_event(event_T event, char_u *pat, ਪੂਰਨ once, ਪੂਰਨ nested, char_u *cmd, ਪੂਰਨ forceit, ਪੂਰਨ ਸਮੂਹ, ਪੂਰਨ flags);
ਸਥਿਰ ਪੂਰਨ apply_autocmds_ਸਮੂਹ(event_T event, char_u *fname, char_u *fname_io, ਪੂਰਨ force, ਪੂਰਨ ਸਮੂਹ, buf_T *buf, exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ auto_ਅਗਲਾ_pat(AutoPatCmd_T *apc, ਪੂਰਨ stop_at_ਆਖਰੀ);
ਸਥਿਰ ਪੂਰਨ au_ਲੱਭੋ_ਸਮੂਹ(char_u *name);

ਸਥਿਰ event_T	ਆਖਰੀ_event;
ਸਥਿਰ ਪੂਰਨ	ਆਖਰੀ_ਸਮੂਹ;
ਸਥਿਰ ਪੂਰਨ	autocmd_blocked = 0;	// block all autocmds

    ਸਥਿਰ char_u *
get_deleted_augroup(ਖਾਲੀ)
{
    ਜੇ (deleted_augroup == ਨਿਰਾ)
	deleted_augroup = (char_u *)_("--Deleted--");
    ਵਾਪਸ deleted_augroup;
}

/*
 * Show the autocommands for one AutoPat.
 */
    ਸਥਿਰ ਖਾਲੀ
show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    // Check for "got_int" (here and at various places below), which is set
    // when "q" has been hit for the "--more--" prompt
    ਜੇ (got_int)
	ਵਾਪਸ;
    ਜੇ (ap->pat == ਨਿਰਾ)		// pattern has been removed
	ਵਾਪਸ;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    ਸੁਨੇਹਾ_putchar('\n');
    ਜੇ (got_int)
	ਜਾਓ theend;
    ਜੇ (event != ਆਖਰੀ_event || ap->ਸਮੂਹ != ਆਖਰੀ_ਸਮੂਹ)
    {
	string_T    *event_name;

	ਜੇ (ap->ਸਮੂਹ != AUGROUP_DEFAULT)
	{
	    ਜੇ (AUGROUP_NAME(ap->ਸਮੂਹ) == ਨਿਰਾ)
		ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    ਨਹੀਂਤਾਂ
		ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)AUGROUP_NAME(ap->ਸਮੂਹ), HL_ATTR(HLF_T));
	    ਸੁਨੇਹਾ_puts("  ");
	}
	event_name = event_nr2name(event);
	ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)event_name->string, HL_ATTR(HLF_T));
	ਆਖਰੀ_event = event;
	ਆਖਰੀ_ਸਮੂਹ = ap->ਸਮੂਹ;
	ਸੁਨੇਹਾ_putchar('\n');
	ਜੇ (got_int)
	    ਜਾਓ theend;
    }

    ਜੇ (ap->pat == ਨਿਰਾ)
	ਜਾਓ theend;  // timer might have cleared the pattern or group

    ਸੁਨੇਹਾ_ਖਾਨਾ = 4;
    ਸੁਨੇਹਾ_outtrans(ap->pat);

    ਲਈ (ac = ap->cmds; ac != ਨਿਰਾ; ac = ac->ਅਗਲਾ)
    {
	ਜੇ (ac->cmd == ਨਿਰਾ)		// skip removed commands
	    ਜਾਰੀ;

	ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ >= 14)
	    ਸੁਨੇਹਾ_putchar('\n');
	ਸੁਨੇਹਾ_ਖਾਨਾ = 14;
	ਜੇ (got_int)
	    ਜਾਓ theend;
	ਸੁਨੇਹਾ_outtrans(ac->cmd);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (p_verbose > 0)
	    ਆਖਰੀ_ਸੈੱਟ_ਸੁਨੇਹਾ(ac->script_ctx);
#ਜੇ_ਅੰਤ
	ਜੇ (got_int)
	    ਜਾਓ theend;
	ਜੇ (ac->ਅਗਲਾ != ਨਿਰਾ)
	{
	    ਸੁਨੇਹਾ_putchar('\n');
	    ਜੇ (got_int)
		ਜਾਓ theend;
	}
    }

theend:
    --autocmd_busy;
}

/*
 * Mark an autocommand pattern for deletion.
 */
    ਸਥਿਰ ਖਾਲੀ
au_remove_pat(AutoPat *ap)
{
    VIM_CLEAR(ap->pat);
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    ਸਥਿਰ ਖਾਲੀ
au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    ਲਈ (ac = ap->cmds; ac != ਨਿਰਾ; ac = ac->ਅਗਲਾ)
	VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

// Delete one command from an autocmd pattern.
ਸਥਿਰ ਖਾਲੀ au_del_cmd(AutoCmd *ac)
{
    VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    ਸਥਿਰ ਖਾਲੀ
au_cleanup(ਖਾਲੀ)
{
    AutoPat	*ap, **prev_ap;
    AutoCmd	*ac, **prev_ac;
    event_T	event;

    ਜੇ (autocmd_busy || !au_need_clean)
	ਵਾਪਸ;

    // loop over all events
    ਲਈ (event = (event_T)0; (ਪੂਰਨ)event < ਅੰਕ_EVENTS;
					    event = (event_T)((ਪੂਰਨ)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(ਪਹਿਲਾ_autopat[(ਪੂਰਨ)event]);
	ਲਈ (ap = *prev_ap; ap != ਨਿਰਾ; ap = *prev_ap)
	{
	    ਪੂਰਨ has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    ਲਈ (ac = *prev_ac; ac != ਨਿਰਾ; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		ਜੇ (ap->pat == ਨਿਰਾ || ac->cmd == ਨਿਰਾ)
		{
		    *prev_ac = ac->ਅਗਲਾ;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		ਨਹੀਂਤਾਂ
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->ਅਗਲਾ);
		}
	    }

	    ਜੇ (ap->pat != ਨਿਰਾ && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    ਜੇ (ap->pat == ਨਿਰਾ)
	    {
		ਜੇ (ap->ਅਗਲਾ == ਨਿਰਾ)
		{
		    ਜੇ (prev_ap == &(ਪਹਿਲਾ_autopat[(ਪੂਰਨ)event]))
			ਆਖਰੀ_autopat[(ਪੂਰਨ)event] = ਨਿਰਾ;
		    ਨਹੀਂਤਾਂ
			// this depends on the "next" field being the first in
			// the struct
			ਆਖਰੀ_autopat[(ਪੂਰਨ)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->ਅਗਲਾ;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    ਨਹੀਂਤਾਂ
		prev_ap = &(ap->ਅਗਲਾ);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Called when buffer is freed, to remove/invalidate related buffer-local
 * autocmds.
 */
    ਖਾਲੀ
aubuflocal_remove(buf_T *buf)
{
    AutoPat	    *ap;
    event_T	    event;
    AutoPatCmd_T    *apc;

    // invalidate currently executing autocommands
    ਲਈ (apc = ਸਰਗਰਮ_apc_list; apc; apc = apc->ਅਗਲਾ)
	ਜੇ (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    ਲਈ (event = (event_T)0; (ਪੂਰਨ)event < ਅੰਕ_EVENTS;
					    event = (event_T)((ਪੂਰਨ)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ਜੇ (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		ਜੇ (p_verbose >= 6)
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
    ਸਥਿਰ ਪੂਰਨ
au_new_ਸਮੂਹ(char_u *name)
{
    ਪੂਰਨ		i;

    i = au_ਲੱਭੋ_ਸਮੂਹ(name);
    ਜੇ (i != AUGROUP_ERROR)
	ਵਾਪਸ i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    ਲਈ (i = 0; i < augroups.ga_len; ++i)
	ਜੇ (AUGROUP_NAME(i) == ਨਿਰਾ)
	    ਰੋਕੋ;
    ਜੇ (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	ਵਾਪਸ AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    ਜੇ (AUGROUP_NAME(i) == ਨਿਰਾ)
	ਵਾਪਸ AUGROUP_ERROR;
    ਜੇ (i == augroups.ga_len)
	++augroups.ga_len;

    ਵਾਪਸ i;
}

    ਸਥਿਰ ਖਾਲੀ
au_del_ਸਮੂਹ(char_u *name)
{
    ਪੂਰਨ		i;
    event_T	event;
    AutoPat	*ap;
    ਪੂਰਨ		ਵਿੱਚ_use = FALSE;


    i = au_ਲੱਭੋ_ਸਮੂਹ(name);
    ਜੇ (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_ਸਮੂਹ_str), name);
	ਵਾਪਸ;
    }
    ਜੇ (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_ਸਮੂਹ));
	ਵਾਪਸ;
    }

    ਲਈ (event = (event_T)0; (ਪੂਰਨ)event < ਅੰਕ_EVENTS;
	    event = (event_T)((ਪੂਰਨ)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ਜੇ (ap->ਸਮੂਹ == i && ap->pat != ਨਿਰਾ)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		ਵਿੱਚ_use = TRUE;
		event = ਅੰਕ_EVENTS;
		ਰੋਕੋ;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    ਜੇ (ਵਿੱਚ_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    ਨਹੀਂਤਾਂ
	AUGROUP_NAME(i) = ਨਿਰਾ;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    ਸਥਿਰ ਪੂਰਨ
au_ਲੱਭੋ_ਸਮੂਹ(char_u *name)
{
    ਪੂਰਨ	    i;

    ਲਈ (i = 0; i < augroups.ga_len; ++i)
	ਜੇ (AUGROUP_NAME(i) != ਨਿਰਾ && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    ਵਾਪਸ i;
    ਵਾਪਸ AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    ਪੂਰਨ
au_has_ਸਮੂਹ(char_u *name)
{
    ਵਾਪਸ au_ਲੱਭੋ_ਸਮੂਹ(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    ਖਾਲੀ
do_augroup(char_u *arg, ਪੂਰਨ del_ਸਮੂਹ)
{
    ਪੂਰਨ	    i;

    ਜੇ (del_ਸਮੂਹ)
    {
	ਜੇ (*arg == NUL)
	    emsg(_(e_ਦਲੀਲ_required));
	ਨਹੀਂਤਾਂ
	    au_del_ਸਮੂਹ(arg);
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    ਨਹੀਂਤਾਂ ਜੇ (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_ਸਮੂਹ(arg);
	ਜੇ (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    ਨਹੀਂਤਾਂ			    // ":aug": list the group names
    {
	ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	ਲਈ (i = 0; i < augroups.ga_len; ++i)
	{
	    ਜੇ (AUGROUP_NAME(i) != ਨਿਰਾ)
	    {
		ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)AUGROUP_NAME(i));
		ਸੁਨੇਹਾ_puts("  ");
	    }
	}
	ਸੁਨੇਹਾ_clr_eos();
	ਸੁਨੇਹਾ_ਸਿਰਾ();
    }
}

    ਖਾਲੀ
autocmd_init(ਖਾਲੀ)
{
    CLEAR_ਖੇਤਰ(aucmd_win);
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_all_autocmds(ਖਾਲੀ)
{
    char_u	*s;

    ਲਈ (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(ਨਿਰਾ, (char_u *)"", TRUE);

    ਲਈ (ਪੂਰਨ i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_ਅੰਕੜੇ))[i];
	ਜੇ (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    ਪੂਰਨ
is_aucmd_win(win_T *win)
{
    ਲਈ (ਪੂਰਨ i = 0; i < AUCMD_WIN_ਗਿਣਤੀ; ++i)
	ਜੇ (aucmd_win[i].auc_win_ਵਰਤਿਆ && aucmd_win[i].auc_win == win)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    ਸਥਿਰ event_T
event_name2nr(char_u *ਸ਼ੁਰੂ, char_u **ਸਿਰਾ)
{
    char_u	*p;
    keyvalue_T ਨਿਸ਼ਾਨਾ;
    keyvalue_T *ਇੰਦਰਾਜ;
    ਸਥਿਰ keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_ਸੂਚਕਾਂਕ];
    ਸਥਿਰ keyvalue_T *bufread = &event_tab[BUFREAD_ਸੂਚਕਾਂਕ];

    // the event name ends with end of line, '|', a blank or a comma
    ਲਈ (p = ਸ਼ੁਰੂ; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    ਨਿਸ਼ਾਨਾ.ਕੁੰਜੀ = 0;
    ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.string = ਸ਼ੁਰੂ;
    ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.ਲੰਮਾਈ = (size_t)(p - ਸ਼ੁਰੂ);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    ਜੇ (cmp_keyvalue_ਮੁੱਲ_ni(&ਨਿਸ਼ਾਨਾ, bufnewfile) == 0)
	ਇੰਦਰਾਜ = bufnewfile;
    ਨਹੀਂਤਾਂ ਜੇ (cmp_keyvalue_ਮੁੱਲ_ni(&ਨਿਸ਼ਾਨਾ, bufread) == 0)
	ਇੰਦਰਾਜ = bufread;
    ਨਹੀਂਤਾਂ
	ਇੰਦਰਾਜ = (keyvalue_T *)bsearch(&ਨਿਸ਼ਾਨਾ, &event_tab, ਅੰਕ_EVENTS,
				ਆਕਾਰ(event_tab[0]), cmp_keyvalue_ਮੁੱਲ_ni);

    ਜੇ (*p == ',')
	++p;
    *ਸਿਰਾ = p;

    ਵਾਪਸ (ਇੰਦਰਾਜ == ਨਿਰਾ) ? ਅੰਕ_EVENTS : (event_T)ਨਿਰਪੇਖ(ਇੰਦਰਾਜ->ਕੁੰਜੀ);
}

/*
 * Return the name for event "event".
 */
    ਸਥਿਰ string_T *
event_nr2name(event_T event)
{
    ਪੂਰਨ	    i;
    ਗਿਣਤੀ {CACHE_ਮਾਪ = 12};
    ਸਥਿਰ ਪੂਰਨ cache_tab[CACHE_ਮਾਪ];
    ਸਥਿਰ ਪੂਰਨ cache_ਆਖਰੀ_ਸੂਚਕਾਂਕ = -1;
    ਸਥਿਰ string_T unknown = STR_LITERAL_INIT("Unknown");

    ਜੇ (cache_ਆਖਰੀ_ਸੂਚਕਾਂਕ < 0)
    {
	ਲਈ (i = 0; i < CACHE_ਮਾਪ; ++i)
	    cache_tab[i] = -1;
	cache_ਆਖਰੀ_ਸੂਚਕਾਂਕ = CACHE_ਮਾਪ - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    ਲਈ (i = cache_ਆਖਰੀ_ਸੂਚਕਾਂਕ; cache_tab[i] >= 0; )
    {
	ਜੇ ((event_T)ਨਿਰਪੇਖ(event_tab[cache_tab[i]].ਕੁੰਜੀ) == event)
	    ਵਾਪਸ &event_tab[cache_tab[i]].ਮੁੱਲ;

	ਜੇ (i == 0)
	    i = CACHE_ਮਾਪ - 1;
	ਨਹੀਂਤਾਂ
	    --i;

	// are we back at the start?
	ਜੇ (i == cache_ਆਖਰੀ_ਸੂਚਕਾਂਕ)
	    ਰੋਕੋ;
    }

    // look in the event table itself
    ਲਈ (i = 0; i < ਅੰਕ_EVENTS; ++i)
    {
	ਜੇ ((event_T)ਨਿਰਪੇਖ(event_tab[i].ਕੁੰਜੀ) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    ਜੇ (cache_ਆਖਰੀ_ਸੂਚਕਾਂਕ == CACHE_ਮਾਪ - 1)
		cache_ਆਖਰੀ_ਸੂਚਕਾਂਕ = 0;
	    ਨਹੀਂਤਾਂ
		++cache_ਆਖਰੀ_ਸੂਚਕਾਂਕ;
	    cache_tab[cache_ਆਖਰੀ_ਸੂਚਕਾਂਕ] = i;
	    ਵਾਪਸ &event_tab[i].ਮੁੱਲ;
	}
    }

    ਵਾਪਸ &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    ਸਥਿਰ char_u *
ਲੱਭੋ_ਸਿਰਾ_event(
    char_u  *arg,
    ਪੂਰਨ	    have_ਸਮੂਹ)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    ਜੇ (*arg == '*')
    {
	ਜੇ (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_ਤਾਰਾ_str), arg);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	pat = arg + 1;
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    ਜੇ ((ਪੂਰਨ)event_name2nr(pat, &p) >= ਅੰਕ_EVENTS)
	    {
		ਜੇ (have_ਸਮੂਹ)
		    semsg(_(e_no_such_event_str), pat);
		ਨਹੀਂਤਾਂ
		    semsg(_(e_no_such_ਸਮੂਹ_or_event_str), pat);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	}
    }
    ਵਾਪਸ pat;
}

/*
 * Find the command of an ":autocmd" with argument "arg", the part after the
 * events, the pattern and any "++once"/"++nested".  Returns NULL when there is
 * no command.  Does not modify "arg" and gives no error messages.
 */
    char_u *
au_ਲੱਭੋ_cmd_arg(char_u *arg)
{
    char_u	*pat;
    char_u	*cmd;
    ਪੂਰਨ		ਸਮੂਹ;

    ਜੇ (*arg == '|')
	ਵਾਪਸ ਨਿਰਾ;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    ਸਮੂਹ = au_get_grouparg(&arg);
    pat = ਸਮੂਹ == AUGROUP_ERROR
			  ? ਨਿਰਾ : ਲੱਭੋ_ਸਿਰਾ_event(arg, ਸਮੂਹ != AUGROUP_ALL);
    --emsg_off;
    ਜੇ (pat == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    pat = skipwhite(pat);
    ਜੇ (*pat == NUL || *pat == '|')
	ਵਾਪਸ ਨਿਰਾ;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    ਜਦੋਂ (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    ਲਈ (ਪੂਰਨ i = 0; i < 2; ++i)
    {
	ਜੇ (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	ਜੇ (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	ਜੇ (!ਵਿੱਚ_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    ਵਾਪਸ *cmd == NUL ? ਨਿਰਾ : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    ਪੂਰਨ
event_ignored(event_T event, char_u *ei)
{
    ਪੂਰਨ ignored = FALSE;
    ਜਦੋਂ (*ei != NUL)
    {
	ਪੂਰਨ unignore = *ei == '-';
	ei += unignore;
	ਜੇ (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].ਕੁੰਜੀ <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	ਨਹੀਂਤਾਂ ਜੇ (event_name2nr(ei, &ei) == event)
	{
	    ਜੇ (unignore)
		ਵਾਪਸ FALSE;
	    ਨਹੀਂਤਾਂ
		ignored = TRUE;
	}
    }

    ਵਾਪਸ ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    ਪੂਰਨ
check_ei(char_u *ei)
{
    ਪੂਰਨ	win = ei != p_ei;

    ਜਦੋਂ (*ei)
    {
	ਜੇ (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	ਨਹੀਂਤਾਂ
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    ਜੇ (event == ਅੰਕ_EVENTS || (win && event_tab[event].ਕੁੰਜੀ > 0))
		ਵਾਪਸ FAIL;
	}
    }

    ਵਾਪਸ OK;
}

#ਜੇ defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(ਅੱਖਰ *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    size_t	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    ਜੇ (save_ei == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    ਜੇ (new_ei == ਨਿਰਾ)
    {
	vim_free(save_ei);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    ਨਹੀਂਤਾਂ
	STRCPY(new_ei + p_ei_len, what);
    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    ਵਾਪਸ save_ei;
}

    ਖਾਲੀ
au_event_restore(char_u *old_ei)
{
    ਜੇ (old_ei != ਨਿਰਾ)
    {
	ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"ei", -1, old_ei,
							  OPT_FREE, SID_NONE);
	vim_free(old_ei);
    }
}
#ਜੇ_ਅੰਤ  // FEAT_SYN_HL

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
    ਖਾਲੀ
do_autocmd(exarg_T *eap, char_u *arg_ਵਿੱਚ, ਪੂਰਨ forceit)
{
    char_u	*arg = arg_ਵਿੱਚ;
    char_u	*pat;
    char_u	*envpat = ਨਿਰਾ;
    char_u	*cmd;
    ਪੂਰਨ		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = ਨਿਰਾ;
    ਪੂਰਨ		nested = FALSE;
    ਪੂਰਨ		once = FALSE;
    ਪੂਰਨ		ਸਮੂਹ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		flags = 0;

    ਜੇ (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	ਸਮੂਹ = AUGROUP_ALL;	// no argument, use all groups
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	ਸਮੂਹ = au_get_grouparg(&arg);
	ਜੇ (arg == ਨਿਰਾ)	    // out of memory
	    ਵਾਪਸ;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = ਲੱਭੋ_ਸਿਰਾ_event(arg, ਸਮੂਹ != AUGROUP_ALL);
    ਜੇ (pat == ਨਿਰਾ)
	ਵਾਪਸ;

    pat = skipwhite(pat);
    ਜੇ (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	ਜਦੋਂ (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	ਜੇ (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	ਜੇ (vim_strchr(pat, '$') != ਨਿਰਾ || vim_strchr(pat, '~') != ਨਿਰਾ)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	    ਪੂਰਨ	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#ਜੇ_ਅੰਤ
	    envpat = expand_env_save(pat);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	    p_ssl = p_ssl_save;
#ਜੇ_ਅੰਤ
	    ਜੇ (envpat != ਨਿਰਾ)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	ਲਈ (i = 0; i < 2; i++)
	{
	    ਜੇ (*cmd == NUL)
		ਜਾਰੀ;

	    // Check for "++once" flag.
	    ਜੇ (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ਜੇ (once)
		{
		    semsg(_(e_duplicate_ਦਲੀਲ_str), "++once");
		    ਜਾਓ err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    ਜੇ ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		ਜੇ (nested)
		{
		    semsg(_(e_duplicate_ਦਲੀਲ_str), "++nested");
		    ਜਾਓ err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    ਜੇ (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ਜੇ (ਵਿੱਚ_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_ਔਸਤ_plusplus_nested));
		    ਜਾਓ err_exit;
		}
		ਜੇ (nested)
		{
		    semsg(_(e_duplicate_ਦਲੀਲ_str), "nested");
		    ਜਾਓ err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 6);
	    }
	}

	/*
	 * Find the start of the commands.
	 * Expand <sfile> in it.
	 */
	ਜੇ (*cmd != NUL)
	{
	    ਜੇ (eap != ਨਿਰਾ)
		// Read a {} block if it follows.
		cmd = may_get_cmd_ਖੰਡ(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    ਜੇ (cmd == ਨਿਰਾ)	    // some error
		ਜਾਓ err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    ਜੇ (!forceit && *cmd == NUL)
	// Highlight title
	ਸੁਨੇਹਾ_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    ਆਖਰੀ_event = (event_T)-1;		// for listing the event name
    ਆਖਰੀ_ਸਮੂਹ = AUGROUP_ERROR;		// for listing the group name
    ਜੇ (*arg == '*' || *arg == NUL || *arg == '|')
    {
	ਜੇ (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	ਨਹੀਂਤਾਂ
	    ਲਈ (event = (event_T)0; (ਪੂਰਨ)event < ਅੰਕ_EVENTS;
					     event = (event_T)((ਪੂਰਨ)event + 1))
		ਜੇ (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, ਸਮੂਹ, flags) == FAIL)
		    ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜਦੋਂ (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    ਜੇ (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, ਸਮੂਹ, flags) == FAIL)
		ਰੋਕੋ;
    }

err_exit:
    ਜੇ (cmd_need_free)
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
    ਸਥਿਰ ਪੂਰਨ
au_get_grouparg(char_u **argp)
{
    char_u	*ਸਮੂਹ_name;
    char_u	*p;
    char_u	*arg = *argp;
    ਪੂਰਨ		ਸਮੂਹ = AUGROUP_ALL;

    ਲਈ (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    ਜੇ (p <= arg)
	ਵਾਪਸ AUGROUP_ALL;

    ਸਮੂਹ_name = vim_strnsave(arg, p - arg);
    ਜੇ (ਸਮੂਹ_name == ਨਿਰਾ)		// out of memory
	ਵਾਪਸ AUGROUP_ERROR;
    ਸਮੂਹ = au_ਲੱਭੋ_ਸਮੂਹ(ਸਮੂਹ_name);
    ਜੇ (ਸਮੂਹ == AUGROUP_ERROR)
	ਸਮੂਹ = AUGROUP_ALL;	// no match, use all groups
    ਨਹੀਂਤਾਂ
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(ਸਮੂਹ_name);
    ਵਾਪਸ ਸਮੂਹ;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    ਸਥਿਰ ਪੂਰਨ
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    ਪੂਰਨ		once,
    ਪੂਰਨ		nested,
    char_u	*cmd,
    ਪੂਰਨ		forceit,
    ਪੂਰਨ		ਸਮੂਹ,
    ਪੂਰਨ		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    ਪੂਰਨ		brace_level;
    char_u	*endpat;
    ਪੂਰਨ		findgroup;
    ਪੂਰਨ		allgroups;
    ਪੂਰਨ		patlen;
    ਪੂਰਨ		is_buflocal;
    ਪੂਰਨ		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    ਜੇ (ਸਮੂਹ == AUGROUP_ALL)
	findgroup = current_augroup;
    ਨਹੀਂਤਾਂ
	findgroup = ਸਮੂਹ;
    allgroups = (ਸਮੂਹ == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    ਜੇ (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    ਜੇ (forceit)  // delete the AutoPat, if it's in the current group
	    {
		ਜੇ (ap->ਸਮੂਹ == findgroup)
		    au_remove_pat(ap);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਸਮੂਹ == AUGROUP_ALL || ap->ਸਮੂਹ == ਸਮੂਹ)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    ਲਈ ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	ਲਈ (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    ਜੇ (*endpat == '{')
		brace_level++;
	    ਨਹੀਂਤਾਂ ਜੇ (*endpat == '}')
		brace_level--;
	}
	ਜੇ (pat == endpat)		// ignore single comma
	    ਜਾਰੀ;
	patlen = (ਪੂਰਨ)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	ਜੇ (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    ਜੇ (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    ਨਹੀਂਤਾਂ ਜੇ (patlen > 9 && pat[7] == '=')
	    {
		ਜੇ (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		ਨਹੀਂਤਾਂ ਜੇ (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((ਅੱਖਰ *)pat + 8);
	    }
	}

	ਜੇ (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((ਅੱਖਰ *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (ਪੂਰਨ)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	ਜੇ (!forceit && *cmd != NUL && ਆਖਰੀ_autopat[(ਪੂਰਨ)event] != ਨਿਰਾ)
	    prev_ap = &ਆਖਰੀ_autopat[(ਪੂਰਨ)event];
	ਨਹੀਂਤਾਂ
	    prev_ap = &ਪਹਿਲਾ_autopat[(ਪੂਰਨ)event];
	ਜਦੋਂ ((ap = *prev_ap) != ਨਿਰਾ)
	{
	    ਜੇ (ap->pat != ਨਿਰਾ)
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
		ਜੇ ((allgroups || ap->ਸਮੂਹ == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    ਜੇ (forceit)
		    {
			ਜੇ (*cmd != NUL && ap->ਅਗਲਾ == ਨਿਰਾ)
			{
			    au_remove_cmds(ap);
			    ਰੋਕੋ;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    ਨਹੀਂਤਾਂ ਜੇ (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    ਨਹੀਂਤਾਂ ਜੇ (ap->ਅਗਲਾ == ਨਿਰਾ)
			ਰੋਕੋ;
		}
	    }
	    prev_ap = &ap->ਅਗਲਾ;
	}

	/*
	 * Add a new command.
	 */
	ਜੇ (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    ਜੇ (ap == ਨਿਰਾ)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		ਜੇ (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == ਨਿਰਾ))
		{
		    semsg(_(e_ਬਫਰ_nr_invalid_ਬਫਰ_ਸੰਖਿਆ), buflocal_nr);
		    ਵਾਪਸ FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		ਜੇ (ap == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		ਜੇ (ap->pat == ਨਿਰਾ)
		{
		    vim_free(ap);
		    ਵਾਪਸ FAIL;
		}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		ਜੇ (event == EVENT_MODECHANGED && !has_modechanged())
		    get_ਬਹੁਲਕ(ਆਖਰੀ_ਬਹੁਲਕ);
#ਜੇ_ਅੰਤ
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		ਜੇ ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
			&& !(has_winscrolled() || has_winresized()))
		{
		    tabpage_T *save_curtab = curtab;
		    tabpage_T *tp;
		    FOR_ALL_TABPAGES(tp)
		    {
			unuse_tabpage(curtab);
			use_tabpage(tp);
			snapshot_windows_scroll_ਮਾਪ();
		    }
		    unuse_tabpage(curtab);
		    use_tabpage(save_curtab);
		}

		ਜੇ (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = ਨਿਰਾ;
		}
		ਨਹੀਂਤਾਂ
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_ਮੰਜ਼ਿਲ_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    ਜੇ (reg_pat != ਨਿਰਾ)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    ਜੇ (reg_pat == ਨਿਰਾ || ap->reg_prog == ਨਿਰਾ)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			ਵਾਪਸ FAIL;
		    }
		}
		ap->cmds = ਨਿਰਾ;
		*prev_ap = ap;
		ਆਖਰੀ_autopat[(ਪੂਰਨ)event] = ap;
		ap->ਅਗਲਾ = ਨਿਰਾ;
		ਜੇ (ਸਮੂਹ == AUGROUP_ALL)
		    ap->ਸਮੂਹ = current_augroup;
		ਨਹੀਂਤਾਂ
		    ap->ਸਮੂਹ = ਸਮੂਹ;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    ਜਦੋਂ ((ac = *prev_ac) != ਨਿਰਾ)
		prev_ac = &ac->ਅਗਲਾ;
	    ac = ALLOC_ONE(AutoCmd);
	    ਜੇ (ac == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    ਜੇ (flags & UC_VIM9)
		ac->script_ctx.sc_ਸੰਸਕਰਣ = SCRIPT_ਸੰਸਕਰਣ_VIM9;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#ਜੇ_ਅੰਤ
	    ਜੇ (ac->cmd == ਨਿਰਾ)
	    {
		vim_free(ac);
		ਵਾਪਸ FAIL;
	    }
	    ac->ਅਗਲਾ = ਨਿਰਾ;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    ਵਾਪਸ OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    ਪੂਰਨ
do_doautocmd(
    char_u	*arg_ਸ਼ੁਰੂ,
    ਪੂਰਨ		do_ਸੁਨੇਹਾ,	    // give message for no matching autocmds?
    ਪੂਰਨ		*did_something)
{
    char_u	*arg = arg_ਸ਼ੁਰੂ;
    char_u	*fname;
    ਪੂਰਨ		nothing_ਪੂਰਾ = TRUE;
    ਪੂਰਨ		ਸਮੂਹ;

    ਜੇ (did_something != ਨਿਰਾ)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    ਸਮੂਹ = au_get_grouparg(&arg);
    ਜੇ (arg == ਨਿਰਾ)	    // out of memory
	ਵਾਪਸ FAIL;

    ਜੇ (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	ਵਾਪਸ FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = ਲੱਭੋ_ਸਿਰਾ_event(arg, ਸਮੂਹ != AUGROUP_ALL);
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    ਜਦੋਂ (*arg && !ਸਿਰੇ_excmd(*arg) && !VIM_ISWHITE(*arg))
	ਜੇ (apply_autocmds_ਸਮੂਹ(event_name2nr(arg, &arg),
				      fname, ਨਿਰਾ, TRUE, ਸਮੂਹ, curbuf, ਨਿਰਾ))
	    nothing_ਪੂਰਾ = FALSE;

    ਜੇ (nothing_ਪੂਰਾ && do_ਸੁਨੇਹਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		&& !aborting()
#ਜੇ_ਅੰਤ
	       )
	smsg(_("No matching autocommands: %s"), arg_ਸ਼ੁਰੂ);
    ਜੇ (did_something != ਨਿਰਾ)
	*did_something = !nothing_ਪੂਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਵਾਪਸ aborting() ? FAIL : OK;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ OK;
#ਜੇ_ਅੰਤ
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    ਖਾਲੀ
ex_doautoall(exarg_T *eap)
{
    ਪੂਰਨ		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    ਪੂਰਨ		call_do_modelines = check_nomodeline(&arg);
    ਪੂਰਨ		did_aucmd;

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
	ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ || buf == curbuf)
	    ਜਾਰੀ;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	ਜੇ (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    ਰੋਕੋ;
	}

	ਸੈੱਟ_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	ਜੇ (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	ਜੇ (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    ਰੋਕੋ;
	}
    }

    // Execute autocommands for the current buffer last.
    ਜੇ (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	ਜੇ (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    ਪੂਰਨ
check_nomodeline(char_u **argp)
{
    ਜੇ (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Prepare for executing autocommands for (hidden) buffer "buf".
 * Search for a visible window containing the current buffer.  If there isn't
 * one then use an entry in "aucmd_win[]".
 * Set "curbuf" and "curwin" to match "buf".
 * When this fails "curbuf" is not equal "buf".
 */
    ਖਾਲੀ
aucmd_prepbuf(
    aco_save_T	*aco,		// structure to save values in
    buf_T	*buf)		// new curbuf
{
    win_T	*win;
    ਪੂਰਨ		save_ea;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_AUTOCHDIR
    ਪੂਰਨ		save_acd;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		same_ਬਫਰ = buf == curbuf;

    // Find a window that is for the new buffer
    ਜੇ (same_ਬਫਰ)		// be quick when buf is curbuf
	win = curwin;
    ਨਹੀਂਤਾਂ
	FOR_ALL_WINDOWS(win)
	    ਜੇ (win->w_ਬਫਰ == buf)
		ਰੋਕੋ;

    // Allocate a window when needed.
    win_T *auc_win = ਨਿਰਾ;
    ਪੂਰਨ auc_idx = AUCMD_WIN_ਗਿਣਤੀ;
    ਜੇ (win == ਨਿਰਾ)
    {
	ਲਈ (auc_idx = 0; auc_idx < AUCMD_WIN_ਗਿਣਤੀ; ++auc_idx)
	    ਜੇ (!aucmd_win[auc_idx].auc_win_ਵਰਤਿਆ)
	    {
		ਜੇ (aucmd_win[auc_idx].auc_win == ਨਿਰਾ)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		ਜੇ (auc_win != ਨਿਰਾ)
		    aucmd_win[auc_idx].auc_win_ਵਰਤਿਆ = TRUE;
		ਰੋਕੋ;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	ਜੇ (auc_win == ਨਿਰਾ)
	    ਵਾਪਸ;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == ਨਿਰਾ ? 0 : prevwin->w_id;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਜੇ (bt_prompt(curbuf))
	aco->save_prompt_ਪਾਓ = curbuf->b_prompt_ਪਾਓ;
#ਜੇ_ਅੰਤ

    ਜੇ (win != ਨਿਰਾ)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    ਨਹੀਂਤਾਂ
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
	curtab->tp_localdir = ਨਿਰਾ;
	aco->globaldir = globaldir;
	globaldir = ਨਿਰਾ;

	// Split the current window, put the auc_win in the upper half.
	// We don't want the BufEnter or WinEnter autocommands.
	ਖੰਡ_autocmds();
	make_snapshot(SNAP_AUCMD_IDX);
	save_ea = p_ea;
	p_ea = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_AUTOCHDIR
	// Prevent chdir() call in win_enter_ext(), through do_autochdir().
	save_acd = p_acd;
	p_acd = FALSE;
#ਜੇ_ਅੰਤ

	(ਖਾਲੀ)win_ਵੰਡੋ_ins(0, WSP_ਸਿਖਰ | WSP_FORCE_ਗੁੰਜਾਇਸ਼, auc_win, 0, ਨਿਰਾ);
	win_comp_pos();   // recompute window positions
	p_ea = save_ea;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_AUTOCHDIR
	p_acd = save_acd;
#ਜੇ_ਅੰਤ
	unblock_autocmds();
	curwin = auc_win;
    }
    curbuf = buf;
    aco->new_curwin_id = curwin->w_id;
    ਸੈੱਟ_bufref(&aco->new_curbuf, curbuf);

    aco->save_VIsual_ਸਰਗਰਮ = VIsual_ਸਰਗਰਮ;
    ਜੇ (!same_ਬਫਰ)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_ਸਰਗਰਮ = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    ਖਾਲੀ
aucmd_restbuf(
    aco_save_T	*aco)		// structure holding saved values
{
    ਪੂਰਨ	    dummy;
    win_T   *save_curwin;

    ਜੇ (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	ਖੰਡ_autocmds();
	ਜੇ (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		ਜੇ (wp == awp)
		{
		    ਜੇ (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(awp);
		    ਜਾਓ win_ਮਿਲਿਆ;
		}
	    }
	}
win_ਮਿਲਿਆ:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(ਖਾਲੀ)winframe_remove(curwin, &dummy, ਨਿਰਾ, ਨਿਰਾ);
	win_remove(curwin, ਨਿਰਾ);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_ਵਰਤਿਆ = FALSE;
	ਆਖਰੀ_ਸਥਿਤੀ(FALSE);	    // may need to remove last status line

	ਜੇ (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_ਲੱਭੋ_by_id(aco->save_curwin_id);
	ਜੇ (save_curwin != ਨਿਰਾ)
	    curwin = save_curwin;
	ਨਹੀਂਤਾਂ
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_ਬਫਰ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_ਖਿੜਕੀ(curwin);
	ਜੇ (bt_prompt(curbuf))
	    curbuf->b_prompt_ਪਾਓ = aco->save_prompt_ਪਾਓ;
#ਜੇ_ਅੰਤ
	prevwin = win_ਲੱਭੋ_by_id(aco->save_prevwin_id);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#ਜੇ_ਅੰਤ
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	ਜੇ (awp->w_localdir != ਨਿਰਾ)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_ਸਰਗਰਮ = aco->save_VIsual_ਸਰਗਰਮ;
	check_cursor();
	ਜੇ (curwin->w_topline > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    curwin->w_topline = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    curwin->w_topfill = 0;
#ਜੇ_ਅੰਤ
	}
#ਜੇ defined(FEAT_GUI)
	ਜੇ (gui.ਵਿੱਚ_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_ਖੱਬਾ], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_ਸੱਜਾ], FALSE);
	    gui_may_update_scrollbars();
	}
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_ਲੱਭੋ_by_id(aco->save_curwin_id);
	ਜੇ (save_curwin != ਨਿਰਾ)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    ਜੇ (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != ਨਿਰਾ)
	    {
#ਜੇ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		ਜੇ (curwin->w_s == &curbuf->b_s)
		    curwin->w_s = &aco->new_curbuf.br_buf->b_s;
#ਜੇ_ਅੰਤ
		--curbuf->b_nwindows;
		curbuf = aco->new_curbuf.br_buf;
		curwin->w_ਬਫਰ = curbuf;
		++curbuf->b_nwindows;
	    }

	    curwin = save_curwin;
	    curbuf = curwin->w_ਬਫਰ;
	    prevwin = win_ਲੱਭੋ_by_id(aco->save_prevwin_id);

	    // In case the autocommand moves the cursor to a position that
	    // does not exist in curbuf.
	    VIsual_ਸਰਗਰਮ = aco->save_VIsual_ਸਰਗਰਮ;
	    check_cursor();
	}
    }

    VIsual_ਸਰਗਰਮ = aco->save_VIsual_ਸਰਗਰਮ;
    check_cursor();	    // just in case lines got deleted
    ਜੇ (VIsual_ਸਰਗਰਮ)
	check_pos(curbuf, &VIsual);
}

ਸਥਿਰ ਪੂਰਨ	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    ਪੂਰਨ
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    ਪੂਰਨ		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    ਵਾਪਸ apply_autocmds_ਸਮੂਹ(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ਨਿਰਾ);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    ਪੂਰਨ
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    ਪੂਰਨ		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    ਵਾਪਸ apply_autocmds_ਸਮੂਹ(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    ਪੂਰਨ
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    ਪੂਰਨ		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    ਪੂਰਨ		*retval)    // pointer to caller's retval
{
    ਪੂਰਨ		did_cmd;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (should_abort(*retval))
	ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ

    did_cmd = apply_autocmds_ਸਮੂਹ(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ਨਿਰਾ);
    ਜੇ (did_cmd
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    && aborting()
#ਜੇ_ਅੰਤ
	    )
	*retval = FAIL;
    ਵਾਪਸ did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    ਸਥਿਰ ਪੂਰਨ
has_cursorhold(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)(get_real_state() == ਬਹੁਲਕ_ਸਾਧਾਰਨ_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != ਨਿਰਾ);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    ਪੂਰਨ
trigger_cursorhold(ਖਾਲੀ)
{
    ਪੂਰਨ		state;

    ਜੇ (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_ਸਰਗਰਮ())
    {
	state = get_real_state();
	ਜੇ (state == ਬਹੁਲਕ_ਸਾਧਾਰਨ_BUSY || (state & ਬਹੁਲਕ_ਪਾਓ) != 0)
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    ਪੂਰਨ
has_winresized(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_WINRESIZED] != ਨਿਰਾ);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    ਪੂਰਨ
has_winscrolled(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_WINSCROLLED] != ਨਿਰਾ);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    ਪੂਰਨ
has_cursormoved(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_CURSORMOVED] != ਨਿਰਾ);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    ਪੂਰਨ
has_cursormovedI(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_CURSORMOVEDI] != ਨਿਰਾ);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    ਪੂਰਨ
has_textchanged(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_TEXTCHANGED] != ਨਿਰਾ);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    ਪੂਰਨ
has_textchangedI(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_TEXTCHANGEDI] != ਨਿਰਾ);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    ਪੂਰਨ
has_textchangedP(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_TEXTCHANGEDP] != ਨਿਰਾ);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    ਪੂਰਨ
has_insertcharpre(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_INSERTCHARPRE] != ਨਿਰਾ);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    ਪੂਰਨ
has_keyinputpre(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_KEYINPUTPRE] != ਨਿਰਾ);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    ਪੂਰਨ
has_cmdundefined(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_CMDUNDEFINED] != ਨਿਰਾ);
}

#ਜੇ defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    ਪੂਰਨ
has_textputpost(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_TEXTPUTPOST] != ਨਿਰਾ);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    ਪੂਰਨ
has_textputpre(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_TEXTPUTPRE] != ਨਿਰਾ);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    ਪੂਰਨ
has_textyankpost(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_TEXTYANKPOST] != ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    ਪੂਰਨ
has_completechanged(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_COMPLETECHANGED] != ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    ਪੂਰਨ
has_modechanged(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_MODECHANGED] != ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    ਸਥਿਰ ਪੂਰਨ
apply_autocmds_ਸਮੂਹ(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    ਪੂਰਨ		force,	     // when TRUE, ignore autocmd_busy
    ਪੂਰਨ		ਸਮੂਹ,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap ਅਣਵਰਤਿਆ) // command arguments
{
    char_u	*sfname = ਨਿਰਾ;	// short file name
    char_u	*ਪੂਛ;
    ਪੂਰਨ		save_ਬਦਲਿਆ;
    buf_T	*old_curbuf;
    ਪੂਰਨ		retval = FALSE;
    char_u	*save_autocmd_fname;
    ਪੂਰਨ		save_autocmd_fname_ਭਰਿਆ;
    ਪੂਰਨ		save_autocmd_bufnr;
    char_u	*save_autocmd_ਮੇਲ;
    ਪੂਰਨ		save_autocmd_busy;
    ਪੂਰਨ		save_autocmd_nested;
    ਸਥਿਰ ਪੂਰਨ	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    funccal_ਇੰਦਰਾਜ_T funccal_ਇੰਦਰਾਜ;
    char_u	*save_cmdarg;
    ਲੰਮਾ	save_cmdbang;
#ਜੇ_ਅੰਤ
    ਸਥਿਰ ਪੂਰਨ	filechangeshell_busy = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    proftime_T	wait_ਸਮਾਂ;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    ਪੂਰਨ		save_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    ਜੇ (event == ਅੰਕ_EVENTS || ਪਹਿਲਾ_autopat[(ਪੂਰਨ)event] == ਨਿਰਾ
	    || autocmd_blocked > 0)
	ਜਾਓ BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    ਜੇ (autocmd_busy && !(force || autocmd_nested))
	ਜਾਓ BYPASS_AU;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    ਜੇ (aborting())
	ਜਾਓ BYPASS_AU;
#ਜੇ_ਅੰਤ

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    ਜੇ (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	ਜਾਓ BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    ਜੇ (event_ignored(event, p_ei))
	ਜਾਓ BYPASS_AU;

    ਪੂਰਨ win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    ਜੇ (buf == curbuf && event_tab[event].ਕੁੰਜੀ <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    ਨਹੀਂਤਾਂ ਜੇ (buf != ਨਿਰਾ && event_tab[event].ਕੁੰਜੀ <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ਜੇ (wp->w_ਬਫਰ == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		ਰੋਕੋ;
	    }
    }
    ਜੇ (win_ignore)
	ਜਾਓ BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    ਜੇ (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	ਜਾਓ BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    ਜੇ (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	ਜਾਓ BYPASS_AU;

    ਜੇ (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_ਬਦਲਿਆ_ਗਿਣਤੀ;

    /*
     * Save the autocmd_* variables and info about the current buffer.
     */
    save_autocmd_fname = autocmd_fname;
    save_autocmd_fname_ਭਰਿਆ = autocmd_fname_ਭਰਿਆ;
    save_autocmd_bufnr = autocmd_bufnr;
    save_autocmd_ਮੇਲ = autocmd_ਮੇਲ;
    save_autocmd_busy = autocmd_busy;
    save_autocmd_nested = autocmd_nested;
    save_ਬਦਲਿਆ = curbuf->b_ਬਦਲਿਆ;
    old_curbuf = curbuf;

    /*
     * Set the file name to be used for <afile>.
     * Make a copy to avoid that changing a buffer name or directory makes it
     * invalid.
     */
    ਜੇ (fname_io == ਨਿਰਾ)
    {
	ਜੇ (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ ਜੇ (fname != ਨਿਰਾ && !ਸਿਰੇ_excmd(*fname))
	    autocmd_fname = fname;
	ਨਹੀਂਤਾਂ ਜੇ (buf != ਨਿਰਾ)
	    autocmd_fname = buf->b_ffname;
	ਨਹੀਂਤਾਂ
	    autocmd_fname = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
	autocmd_fname = fname_io;
    ਜੇ (autocmd_fname != ਨਿਰਾ)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_ਭਰਿਆ = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    ਜੇ (buf == ਨਿਰਾ)
	autocmd_bufnr = 0;
    ਨਹੀਂਤਾਂ
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    ਜੇ (fname == ਨਿਰਾ || *fname == NUL)
    {
	ਜੇ (buf == ਨਿਰਾ)
	    fname = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਜੇ (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਜੇ (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (buf->b_sfname != ਨਿਰਾ)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	ਜੇ (fname == ਨਿਰਾ)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    ਨਹੀਂਤਾਂ
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	ਜੇ (event == EVENT_FILETYPE
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
	    autocmd_fname_ਭਰਿਆ = TRUE; // don't expand it later
	}
	ਨਹੀਂਤਾਂ
	    fname = ਭਰਿਆ_Name_save(fname, FALSE);
    }
    ਜੇ (fname == ਨਿਰਾ)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	ਜਾਓ BYPASS_AU;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    ਜੇ (sfname != ਨਿਰਾ)
	forward_slash(sfname);
    forward_slash(fname);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    // remove version for correct match
    ਜੇ (sfname != ਨਿਰਾ)
	vms_remove_ਸੰਸਕਰਣ(sfname);
    vms_remove_ਸੰਸਕਰਣ(fname);
#ਜੇ_ਅੰਤ

    /*
     * Set the name to be used for <amatch>.
     */
    autocmd_ਮੇਲ = fname;


    // Don't redraw while doing autocommands.
    ++RedrawingDisabled;

    // name and lnum are filled in later
    estack_ਧੱਕੋ(ETYPE_AUCMD, ਨਿਰਾ, 0);
    ESTACK_CHECK_ਤਿਆਰੀ;

    save_current_sctx = current_sctx;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
# ifdef FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
	prof_ਬੱਚਾ_enter(&wait_ਸਮਾਂ); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_ਇੰਦਰਾਜ);
#ਜੇ_ਅੰਤ

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    ਜੇ (!autocmd_busy)
    {
	save_ਖੋਜ_patterns();
	ਜੇ (!ins_compl_ਸਰਗਰਮ())
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
    ਜੇ (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    ਪੂਛ = gettail(fname);

    // Find first autocommand that matches
    CLEAR_ਖੇਤਰ(patcmd);
    patcmd.curpat = ਪਹਿਲਾ_autopat[(ਪੂਰਨ)event];
    patcmd.ਸਮੂਹ = ਸਮੂਹ;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.ਪੂਛ = ਪੂਛ;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_ਅਗਲਾ_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    ਜੇ (patcmd.curpat != ਨਿਰਾ)
    {
	// add to active_apc_list
	patcmd.ਅਗਲਾ = ਸਰਗਰਮ_apc_list;
	ਸਰਗਰਮ_apc_list = &patcmd;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (ਲੰਮਾ)get_vim_var_nr(VV_CMDBANG);
	ਜੇ (eap != ਨਿਰਾ)
	{
	    save_cmdarg = ਸੈੱਟ_cmdarg(eap, ਨਿਰਾ);
	    ਸੈੱਟ_vim_var_nr(VV_CMDBANG, (ਲੰਮਾ)eap->forceit);
	}
	ਨਹੀਂਤਾਂ
	    save_cmdarg = ਨਿਰਾ;	// avoid gcc warning
#ਜੇ_ਅੰਤ
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	ਲਈ (ap = patcmd.curpat; ap->ਅਗਲਾ != ਨਿਰਾ; ap = ap->ਅਗਲਾ)
	    ap->ਆਖਰੀ = FALSE;
	ap->ਆਖਰੀ = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	ਜੇ (nesting == 1)
	    check_lnums(TRUE);
	ਨਹੀਂਤਾਂ
	    check_lnums_nested(TRUE);

	ਪੂਰਨ save_did_emsg = did_emsg;
	ਪੂਰਨ save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(ਨਿਰਾ, getnextac, (ਖਾਲੀ *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	ਸੈੱਟ_pressedreturn(save_ex_pressedreturn);

	ਜੇ (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (eap != ਨਿਰਾ)
	{
	    (ਖਾਲੀ)ਸੈੱਟ_cmdarg(ਨਿਰਾ, save_cmdarg);
	    ਸੈੱਟ_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#ਜੇ_ਅੰਤ
	// delete from active_apc_list
	ਜੇ (ਸਰਗਰਮ_apc_list == &patcmd)	    // just in case
	    ਸਰਗਰਮ_apc_list = patcmd.ਅਗਲਾ;
    }

    ਜੇ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_ਕੱਢੋ();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_ਭਰਿਆ = save_autocmd_fname_ਭਰਿਆ;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_ਮੇਲ = save_autocmd_ਮੇਲ;
    current_sctx = save_current_sctx;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
	prof_ਬੱਚਾ_exit(&wait_ਸਮਾਂ);
# endif
#ਜੇ_ਅੰਤ
    ਕੁੰਜੀ_Typed = save_ਕੁੰਜੀ_Typed;
    vim_free(fname);
    vim_free(sfname);
    --nesting;		// see matching increment above

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.  Free any buffers in the au_pending_free_buf list and
     * free any windows in the au_pending_free_win list.
     */
    ਜੇ (!autocmd_busy)
    {
	restore_ਖੋਜ_patterns();
	ਜੇ (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	ਜਦੋਂ (au_pending_free_buf != ਨਿਰਾ)
	{
	    buf_T *b = au_pending_free_buf->b_ਅਗਲਾ;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	ਜਦੋਂ (au_pending_free_win != ਨਿਰਾ)
	{
	    win_T *w = au_pending_free_win->w_ਅਗਲਾ;

	    vim_free(au_pending_free_win);
	    au_pending_free_win = w;
	}
    }

    /*
     * Some events don't set or reset the Changed flag.
     * Check if still in the same buffer!
     */
    ਜੇ (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	ਜੇ (curbuf->b_ਬਦਲਿਆ != save_ਬਦਲਿਆ)
	    need_maketitle = TRUE;
	curbuf->b_ਬਦਲਿਆ = save_ਬਦਲਿਆ;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    ਜੇ (event == EVENT_BUFWIPEOUT && buf != ਨਿਰਾ)
	aubuflocal_remove(buf);

    ਜੇ (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    ਵਾਪਸ retval;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ char_u	*old_termresponse = ਨਿਰਾ;
ਸਥਿਰ char_u	*old_termu7resp = ਨਿਰਾ;
ਸਥਿਰ char_u	*old_termblinkresp = ਨਿਰਾ;
ਸਥਿਰ char_u	*old_termrbgresp = ਨਿਰਾ;
ਸਥਿਰ char_u	*old_termrfgresp = ਨਿਰਾ;
ਸਥਿਰ char_u	*old_termstyleresp = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    ਖਾਲੀ
ਖੰਡ_autocmds(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // Remember the value of v:termresponse.
    ਜੇ (autocmd_blocked == 0)
    {
	old_termresponse = get_vim_var_str(VV_TERMRESPONSE);
	old_termu7resp = get_vim_var_str(VV_TERMU7RESP);
	old_termblinkresp = get_vim_var_str(VV_TERMBLINKRESP);
	old_termrbgresp = get_vim_var_str(VV_TERMRBGRESP);
	old_termrfgresp = get_vim_var_str(VV_TERMRFGRESP);
	old_termstyleresp = get_vim_var_str(VV_TERMSTYLERESP);
    }
#ਜੇ_ਅੰਤ
    ++autocmd_blocked;
}

    ਖਾਲੀ
unblock_autocmds(ਖਾਲੀ)
{
    --autocmd_blocked;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    ਜੇ (autocmd_blocked == 0)
    {
	ਜੇ (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", ਨਿਰਾ, FALSE, curbuf);
	}
	ਜੇ (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", ਨਿਰਾ, FALSE, curbuf);
	}
	ਜੇ (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", ਨਿਰਾ, FALSE, curbuf);
	}
	ਜੇ (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", ਨਿਰਾ, FALSE, curbuf);
	}
	ਜੇ (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", ਨਿਰਾ, FALSE, curbuf);
	}
	ਜੇ (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", ਨਿਰਾ, FALSE, curbuf);
	}
    }
#ਜੇ_ਅੰਤ
}

    ਪੂਰਨ
is_autocmd_blocked(ਖਾਲੀ)
{
    ਵਾਪਸ autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    ਸਥਿਰ ਖਾਲੀ
auto_ਅਗਲਾ_pat(
    AutoPatCmd_T *apc,
    ਪੂਰਨ		stop_at_ਆਖਰੀ)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*ਇੰਦਰਾਜ;

    ਇੰਦਰਾਜ = ((estack_T *)exestack.ga_ਅੰਕੜੇ) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(ਇੰਦਰਾਜ->es_name);
    ਇੰਦਰਾਜ->es_info.aucmd = ਨਿਰਾ;

    ਲਈ (ap = apc->curpat; ap != ਨਿਰਾ && !got_int; ap = ap->ਅਗਲਾ)
    {
	apc->curpat = ਨਿਰਾ;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	ਜੇ (ap->pat != ਨਿਰਾ && ap->cmds != ਨਿਰਾ
		&& (apc->ਸਮੂਹ == AUGROUP_ALL || apc->ਸਮੂਹ == ap->ਸਮੂਹ))
	{
	    // execution-condition
	    ਜੇ (ap->buflocal_nr == 0
		    ? (ਮੇਲ_file_pat(ਨਿਰਾ, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->ਪੂਛ, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		ਅੱਖਰ	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->ਲੰਮਾਈ + ap->patlen + 1);
		ਜੇ (namep != ਨਿਰਾ)
		{
		    sprintf((ਅੱਖਰ *)namep, fmt, (ਅੱਖਰ *)event_name->string, (ਅੱਖਰ *)ap->pat);
		    ਜੇ (p_verbose >= 8)
		    {
			verbose_enter();
			smsg(_("Executing %s"), namep);
			verbose_leave();
		    }
		}

		// Update the exestack entry for this autocmd.
		ਇੰਦਰਾਜ->es_name = namep;
		ਇੰਦਰਾਜ->es_info.aucmd = apc;

		apc->curpat = ap;
		apc->nextcmd = ap->cmds;
		// mark last command
		ਲਈ (cp = ap->cmds; cp->ਅਗਲਾ != ਨਿਰਾ; cp = cp->ਅਗਲਾ)
		    cp->ਆਖਰੀ = FALSE;
		cp->ਆਖਰੀ = TRUE;
	    }
	    ਸਤਰ_breakcheck();
	    ਜੇ (apc->curpat != ਨਿਰਾ)	    // found a match
		ਰੋਕੋ;
	}
	ਜੇ (stop_at_ਆਖਰੀ && ap->ਆਖਰੀ)
	    ਰੋਕੋ;
    }
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    ਵਾਪਸ &acp->script_ctx;
}
#ਜੇ_ਅੰਤ

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	ਪੂਰਨ c ਅਣਵਰਤਿਆ,
	ਖਾਲੀ *cookie,
	ਪੂਰਨ indent ਅਣਵਰਤਿਆ,
	getline_opt_T options ਅਣਵਰਤਿਆ)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    ਜੇ (acp->curpat == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // repeat until we find an autocommand to execute
    ਲਈ (;;)
    {
	// skip removed commands
	ਜਦੋਂ (acp->nextcmd != ਨਿਰਾ && acp->nextcmd->cmd == ਨਿਰਾ)
	    ਜੇ (acp->nextcmd->ਆਖਰੀ)
		acp->nextcmd = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
		acp->nextcmd = acp->nextcmd->ਅਗਲਾ;

	ਜੇ (acp->nextcmd != ਨਿਰਾ)
	    ਰੋਕੋ;

	// at end of commands, find next pattern that matches
	ਜੇ (acp->curpat->ਆਖਰੀ)
	    acp->curpat = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    acp->curpat = acp->curpat->ਅਗਲਾ;
	ਜੇ (acp->curpat != ਨਿਰਾ)
	    auto_ਅਗਲਾ_pat(acp, TRUE);
	ਜੇ (acp->curpat == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
    }

    ac = acp->nextcmd;

    ਜੇ (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	ਸੁਨੇਹਾ_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    ਜੇ (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    ਜੇ (ac->ਆਖਰੀ)
	acp->nextcmd = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	acp->nextcmd = ac->ਅਗਲਾ;
    ਵਾਪਸ retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    ਸਥਿਰ ਪੂਰਨ
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_ਭਰਿਆ)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*ਪੂਛ = gettail(sfname);
    ਪੂਰਨ		retval = FALSE;

    ਜੇ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)event] == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    fname = is_ਭਰਿਆ ? vim_strsave(sfname) : ਭਰਿਆ_Name_save(sfname, FALSE);
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    ਜੇ (sfname != ਨਿਰਾ)
	forward_slash(sfname);
    forward_slash(fname);
#ਜੇ_ਅੰਤ

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	ਜੇ (ap->pat != ਨਿਰਾ && ap->cmds != ਨਿਰਾ
	      && (ap->buflocal_nr == 0
		? ਮੇਲ_file_pat(ਨਿਰਾ, &ap->reg_prog,
					  fname, sfname, ਪੂਛ, ap->allow_dirs)
		: buf != ਨਿਰਾ && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    ਰੋਕੋ;
	}

    vim_free(fname);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    vim_free(sfname);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    ਪੂਰਨ
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    ਵਾਪਸ has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    ਪੂਰਨ
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    ਵਾਪਸ has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx == augroups.ga_len)		// add "END" add the end
	ਵਾਪਸ (char_u *)"END";
    ਜੇ (idx < 0 || idx >= augroups.ga_len)	// end of list
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (AUGROUP_NAME(idx) == ਨਿਰਾ || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	ਵਾਪਸ (char_u *)"";
    ਵਾਪਸ AUGROUP_NAME(idx);		// return a name
}

ਸਥਿਰ ਪੂਰਨ include_groups = FALSE;

    char_u  *
ਸੈੱਟ_context_ਵਿੱਚ_autocmd(
    expand_T	*xp,
    char_u	*arg,
    ਪੂਰਨ		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    ਪੂਰਨ		ਸਮੂਹ;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    ਸਮੂਹ = au_get_grouparg(&arg);
    ਜੇ (ਸਮੂਹ == AUGROUP_ERROR)
	ਵਾਪਸ ਨਿਰਾ;
    // If there only is a group name that's what we expand.
    ਜੇ (*arg == NUL && ਸਮੂਹ != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	ਸਮੂਹ = AUGROUP_ALL;
    }

    // skip over event name
    ਲਈ (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	ਜੇ (*p == ',')
	    arg = p + 1;
    ਜੇ (*p == NUL)
    {
	ਜੇ (ਸਮੂਹ == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_ਨਮੂਨਾ = arg;
	ਵਾਪਸ ਨਿਰਾ;
    }

    // skip over pattern
    arg = skipwhite(p);
    ਜਦੋਂ (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    ਜੇ (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	ਲਈ (ਪੂਰਨ i = 0; i < 2; i++)
	{
	    ਜੇ (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    ਜੇ (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    ਜੇ (!ਵਿੱਚ_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	ਵਾਪਸ arg;			    // expand (next) command
    }

    ਜੇ (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    ਨਹੀਂਤਾਂ
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਪੂਰਨ i;

    ਜੇ (idx < augroups.ga_len)		// First list group names, if wanted
    {
	ਜੇ (!include_groups || AUGROUP_NAME(idx) == ਨਿਰਾ
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    ਵਾਪਸ (char_u *)"";	// skip deleted entries
	ਵਾਪਸ AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    ਜੇ (i < 0 || i >= ਅੰਕ_EVENTS)
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ event_tab[i].ਮੁੱਲ.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_ਸਮੂਹ(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx, ਪੂਰਨ win)
{
    ਜੇ (idx < 0 || idx >= ਅੰਕ_EVENTS)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (!win)
	ਵਾਪਸ event_tab[idx].ਮੁੱਲ.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    ਪੂਰਨ j = 0;
    ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_EVENTS; ++i)
    {
	j += event_tab[i].ਕੁੰਜੀ <= 0;
	ਜੇ (j == idx + 1)
	    ਵਾਪਸ event_tab[i].ਮੁੱਲ.string;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    ਪੂਰਨ
has_tabclosedpre(ਖਾਲੀ)
{
    ਵਾਪਸ (ਪਹਿਲਾ_autopat[(ਪੂਰਨ)EVENT_TABCLOSEDPRE] != ਨਿਰਾ);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    ਪੂਰਨ
autocmd_supported(char_u *name)
{
    char_u *p;

    ਵਾਪਸ (event_name2nr(name, &p) != ਅੰਕ_EVENTS);
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
    ਪੂਰਨ
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*ਨਮੂਨਾ = ਨਿਰਾ;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = ਨਿਰਾ;
    ਪੂਰਨ		ਸਮੂਹ;
    ਪੂਰਨ		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    ਜੇ (arg_save == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    p = vim_strchr(arg_save, '#');
    ਜੇ (p != ਨਿਰਾ)
	*p++ = NUL;

    // First, look for an autocmd group name
    ਸਮੂਹ = au_ਲੱਭੋ_ਸਮੂਹ(arg_save);
    ਜੇ (ਸਮੂਹ == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	ਸਮੂਹ = AUGROUP_ALL;
	event_name = arg_save;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (p == ਨਿਰਾ)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    ਜਾਓ theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	ਜੇ (p != ਨਿਰਾ)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    ਨਮੂਨਾ = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    ਜੇ (event == ਅੰਕ_EVENTS)
	ਜਾਓ theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = ਪਹਿਲਾ_autopat[(ਪੂਰਨ)event];
    ਜੇ (ap == ਨਿਰਾ)
	ਜਾਓ theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    ਜੇ (ਨਮੂਨਾ != ਨਿਰਾ && STRICMP(ਨਮੂਨਾ, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    ਲਈ ( ; ap != ਨਿਰਾ; ap = ap->ਅਗਲਾ)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	ਜੇ (ap->pat != ਨਿਰਾ && ap->cmds != ਨਿਰਾ
	    && (ਸਮੂਹ == AUGROUP_ALL || ap->ਸਮੂਹ == ਸਮੂਹ)
	    && (ਨਮੂਨਾ == ਨਿਰਾ
		|| (buflocal_buf == ਨਿਰਾ
		    ? fnamecmp(ap->pat, ਨਮੂਨਾ) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    ਰੋਕੋ;
	}

theend:
    vim_free(arg_save);
    ਵਾਪਸ retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    ਸਥਿਰ ਖਾਲੀ
autocmd_ਜੋੜੋ_or_delete(typval_T *argvars, typval_T *rettv, ਪੂਰਨ delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = ਨਿਰਾ;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*ਸਮੂਹ_name = ਨਿਰਾ;
    ਪੂਰਨ		ਸਮੂਹ;
    char_u	*pat = ਨਿਰਾ;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = ਨਿਰਾ;
    char_u	*ਸਿਰਾ;
    ਪੂਰਨ		once;
    ਪੂਰਨ		nested;
    ਪੂਰਨ		replace;		// replace the cmd for a group/event
    ਪੂਰਨ		retval = VVAL_TRUE;
    ਪੂਰਨ		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ਸੰਖਿਆ = VVAL_FALSE;

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (check_for_list_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    aucmd_list = argvars[0].vval.v_list;
    ਜੇ (aucmd_list == ਨਿਰਾ)
	ਵਾਪਸ;

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(aucmd_list, li)
    {
	VIM_CLEAR(ਸਮੂਹ_name);
	VIM_CLEAR(cmd);
	event_name = ਨਿਰਾ;
	event_list = ਨਿਰਾ;
	pat = ਨਿਰਾ;
	pat_list = ਨਿਰਾ;

	ਜੇ (li->li_tv.v_type != VAR_DICT)
	    ਜਾਰੀ;

	event_dict = li->li_tv.vval.v_dict;
	ਜੇ (event_dict == ਨਿਰਾ)
	    ਜਾਰੀ;

	di = dict_ਲੱਭੋ(event_dict, (char_u *)"event", -1);
	ਜੇ (di != ਨਿਰਾ)
	{
	    ਜੇ (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		ਜੇ (event_name == ਨਿਰਾ)
		{
		    emsg(_(e_string_required));
		    ਜਾਰੀ;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		ਜੇ (event_list == ਨਿਰਾ)
		{
		    emsg(_(e_list_required));
		    ਜਾਰੀ;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		emsg(_(e_string_or_list_expected));
		ਜਾਰੀ;
	    }
	}

	ਸਮੂਹ_name = dict_get_string(event_dict, "group", TRUE);
	ਜੇ (ਸਮੂਹ_name == ਨਿਰਾ || *ਸਮੂਹ_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    ਸਮੂਹ = current_augroup;
	ਨਹੀਂਤਾਂ
	{
	    ਸਮੂਹ = au_ਲੱਭੋ_ਸਮੂਹ(ਸਮੂਹ_name);
	    ਜੇ (ਸਮੂਹ == AUGROUP_ERROR)
	    {
		ਜੇ (delete)
		{
		    semsg(_(e_no_such_ਸਮੂਹ_str), ਸਮੂਹ_name);
		    retval = VVAL_FALSE;
		    ਰੋਕੋ;
		}
		// group is not found, create it now
		ਸਮੂਹ = au_new_ਸਮੂਹ(ਸਮੂਹ_name);
		ਜੇ (ਸਮੂਹ == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_ਸਮੂਹ_str), ਸਮੂਹ_name);
		    retval = VVAL_FALSE;
		    ਰੋਕੋ;
		}

		current_augroup = ਸਮੂਹ;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	ਜੇ (dict_has_ਕੁੰਜੀ(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_ਸੰਖਿਆ_def(event_dict, "bufnr", -1);
	    ਜੇ (bnum == -1)
		ਜਾਰੀ;

	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "<buffer=%d>", (ਪੂਰਨ)bnum);
	    pat = IObuff;
	}
	ਨਹੀਂਤਾਂ
	{
	    di = dict_ਲੱਭੋ(event_dict, (char_u *)"pattern", -1);
	    ਜੇ (di != ਨਿਰਾ)
	    {
		ਜੇ (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    ਜੇ (pat == ਨਿਰਾ)
		    {
			emsg(_(e_string_required));
			ਜਾਰੀ;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    ਜੇ (pat_list == ਨਿਰਾ)
		    {
			emsg(_(e_list_required));
			ਜਾਰੀ;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    emsg(_(e_string_or_list_expected));
		    ਜਾਰੀ;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	ਜੇ (cmd == ਨਿਰਾ)
	{
	    ਜੇ (delete)
		cmd = vim_strsave((char_u *)"");
	    ਨਹੀਂਤਾਂ
		ਜਾਰੀ;
	}

	ਜੇ (delete && (event_name == ਨਿਰਾ
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    ਲਈ (event = (event_T)0; (ਪੂਰਨ)event < ਅੰਕ_EVENTS;
		    event = (event_T)((ਪੂਰਨ)event + 1))
	    {
		ਜੇ (do_autocmd_event(event, pat, once, nested, cmd, delete,
							ਸਮੂਹ, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    ਰੋਕੋ;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u *p = ਨਿਰਾ;

	    eli = ਨਿਰਾ;
	    ਸਿਰਾ = ਨਿਰਾ;
	    ਜਦੋਂ (TRUE)
	    {
		ਜੇ (event_list != ਨਿਰਾ)
		{
		    ਜੇ (eli == ਨਿਰਾ)
			eli = event_list->lv_ਪਹਿਲਾ;
		    ਨਹੀਂਤਾਂ
			eli = eli->li_ਅਗਲਾ;
		    ਜੇ (eli == ਨਿਰਾ)
			ਰੋਕੋ;
		    ਜੇ (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == ਨਿਰਾ)
		    {
			emsg(_(e_string_required));
			ਰੋਕੋ;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (p == ਨਿਰਾ)
			p = event_name;
		    ਜੇ (p == ਨਿਰਾ || *p == NUL)
			ਰੋਕੋ;
		}

		event = event_name2nr(p, &ਸਿਰਾ);
		ਜੇ (event == ਅੰਕ_EVENTS || *ਸਿਰਾ != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    ਰੋਕੋ;
		}
		ਜੇ (pat != ਨਿਰਾ)
		{
		    ਜੇ (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, ਸਮੂਹ, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			ਰੋਕੋ;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (pat_list != ਨਿਰਾ)
		{
		    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(pat_list, pli)
		    {
			ਜੇ (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == ਨਿਰਾ)
			{
			    emsg(_(e_string_required));
			    ਜਾਰੀ;
			}
			ਜੇ (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, ਸਮੂਹ, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    ਰੋਕੋ;
			}
		    }
		    ਜੇ (retval == VVAL_FALSE)
			ਰੋਕੋ;
		}
		ਜੇ (event_name != ਨਿਰਾ)
		    p = ਸਿਰਾ;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	ਜੇ (delete && ਸਮੂਹ_name != ਨਿਰਾ &&
		(event_name == ਨਿਰਾ || event_name[0] == NUL)
		&& (pat == ਨਿਰਾ || pat[0] == NUL)
		&& (cmd == ਨਿਰਾ || cmd[0] == NUL))
	    au_del_ਸਮੂਹ(ਸਮੂਹ_name);
    }

    VIM_CLEAR(ਸਮੂਹ_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_ਸੰਖਿਆ = retval;
}

/*
 * autocmd_add() function
 */
    ਖਾਲੀ
f_autocmd_ਜੋੜੋ(typval_T *argvars, typval_T *rettv)
{
    autocmd_ਜੋੜੋ_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    ਖਾਲੀ
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_ਜੋੜੋ_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    ਖਾਲੀ
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = ਅੰਕ_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = ਨਿਰਾ;
    char_u	*name = ਨਿਰਾ;
    ਪੂਰਨ		ਸਮੂਹ = AUGROUP_ALL;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    ਜੇ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	ਜੇ (dict_has_ਕੁੰਜੀ(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    ਜੇ (name == ਨਿਰਾ)
		ਵਾਪਸ;

	    ਜੇ (*name == NUL)
		ਸਮੂਹ = AUGROUP_DEFAULT;
	    ਨਹੀਂਤਾਂ
	    {
		ਸਮੂਹ = au_ਲੱਭੋ_ਸਮੂਹ(name);
		ਜੇ (ਸਮੂਹ == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_ਸਮੂਹ_str), name);
		    vim_free(name);
		    ਵਾਪਸ;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	ਜੇ (dict_has_ਕੁੰਜੀ(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    ਜੇ (name == ਨਿਰਾ)
		ਵਾਪਸ;

	    ਜੇ (name[0] == '*' && name[1] == NUL)
		event_arg = ਅੰਕ_EVENTS;
	    ਨਹੀਂਤਾਂ
	    {
		keyvalue_T ਨਿਸ਼ਾਨਾ;
		keyvalue_T *ਇੰਦਰਾਜ;

		ਨਿਸ਼ਾਨਾ.ਕੁੰਜੀ = 0;
		ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.string = name;
		ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.ਲੰਮਾਈ = STRLEN(ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.string);
		ਇੰਦਰਾਜ = (keyvalue_T *)bsearch(&ਨਿਸ਼ਾਨਾ, &event_tab,
		    ਅੰਕ_EVENTS, ਆਕਾਰ(event_tab[0]), cmp_keyvalue_ਮੁੱਲ_ni);
		ਜੇ (ਇੰਦਰਾਜ == ਨਿਰਾ)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    ਵਾਪਸ;
		}
		event_arg = (event_T)ਨਿਰਪੇਖ(ਇੰਦਰਾਜ->ਕੁੰਜੀ);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	ਜੇ (dict_has_ਕੁੰਜੀ(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    ਜੇ (pat == ਨਿਰਾ)
		ਵਾਪਸ;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    ਲਈ (event = (event_T)0; (ਪੂਰਨ)event < ਅੰਕ_EVENTS;
	    event = (event_T)((ਪੂਰਨ)event + 1))
    {
	string_T    *event_name;

	ਜੇ (event_arg != ਅੰਕ_EVENTS && event != event_arg)
	    ਜਾਰੀ;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	ਸਮੂਹ_name;

	    ਜੇ (ap->pat == ਨਿਰਾ)		// pattern has been removed
		ਜਾਰੀ;

	    ਜੇ (ਸਮੂਹ != AUGROUP_ALL && ਸਮੂਹ != ap->ਸਮੂਹ)
		ਜਾਰੀ;

	    ਜੇ (pat != ਨਿਰਾ && STRCMP(pat, ap->pat) != 0)
		ਜਾਰੀ;

	    ਸਮੂਹ_name.string = get_augroup_name(ਨਿਰਾ, ap->ਸਮੂਹ);
	    ਜੇ (ਸਮੂਹ_name.string == ਨਿਰਾ)
		STR_LITERAL_ਸੈੱਟ(ਸਮੂਹ_name, "");
	    ਨਹੀਂਤਾਂ
		ਸਮੂਹ_name.ਲੰਮਾਈ = STRLEN(ਸਮੂਹ_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    ਲਈ (ac = ap->cmds; ac != ਨਿਰਾ; ac = ac->ਅਗਲਾ)
	    {
		event_dict = dict_alloc();
		ਜੇ (event_dict == ਨਿਰਾ
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    ਵਾਪਸ;
		}

		ਜੇ (dict_ਜੋੜੋ_string_len(event_dict, "event",
			event_name->string, (ਪੂਰਨ)event_name->ਲੰਮਾਈ) == FAIL
			|| dict_ਜੋੜੋ_string_len(event_dict, "group",
			    ਸਮੂਹ_name.string, (ਪੂਰਨ)ਸਮੂਹ_name.ਲੰਮਾਈ) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_ਜੋੜੋ_ਸੰਖਿਆ(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_ਜੋੜੋ_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_ਜੋੜੋ_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_ਜੋੜੋ_bool(event_dict, "once", ac->once) == FAIL
			|| dict_ਜੋੜੋ_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    ਵਾਪਸ;
		}
	    }
	}
    }

    vim_free(pat);
}

#ਜੇ_ਅੰਤ
