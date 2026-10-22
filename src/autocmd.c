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

#અન્તર્ભાવ "vim.h"

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
પ્રકારવ્યાખ્યા સંરચના AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    અક્ષર	    once;		// "One shot": removed after execution
    અક્ષર	    nested;		// If autocommands nest here.
    અક્ષર	    last;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    સંરચના AutoCmd  *next;		// next AutoCmd in list
} AutoCmd;

પ્રકારવ્યાખ્યા સંરચના AutoPat
{
    સંરચના AutoPat  *next;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    પૂર્ણાંક		    group;		// group ID
    પૂર્ણાંક		    patlen;		// strlen() of pat
    પૂર્ણાંક		    buflocal_nr;	// !=0 for buffer-local AutoPat
    અક્ષર	    allow_dirs;		// Pattern may match whole path
    અક્ષર	    last;		// last pattern for apply_autocmds()
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
સ્થિર keyvalue_T event_tab[અંક_EVENTS] = {
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

સ્થિર AutoPat *first_autopat[અંક_EVENTS] = { ખાલી };
સ્થિર AutoPat *last_autopat[અંક_EVENTS] = { ખાલી };

#define AUGROUP_DEFAULT    (-1)	    // default autocmd group
#define AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#define AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
સંરચના AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    પૂર્ણાંક		group;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*tail;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    પૂર્ણાંક		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *next;		// chain of active apc-s for auto-invalidation
};

સ્થિર AutoPatCmd_T *active_apc_list = ખાલી; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#define FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    માટે ((ap) = first_autopat[(પૂર્ણાંક)(event)]; (ap) != ખાલી; (ap) = (ap)->next)

/*
 * augroups stores a list of autocmd group names.
 */
સ્થિર garray_T augroups = {0, 0, કદ(char_u *), 10, ખાલી};
#define AUGROUP_NAME(i) (((char_u **)augroups.ga_data)[i])
// use get_deleted_augroup() to get this
સ્થિર char_u *deleted_augroup = ખાલી;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
સ્થિર પૂર્ણાંક current_augroup = AUGROUP_DEFAULT;

સ્થિર પૂર્ણાંક au_need_clean = FALSE;   // need to delete marked patterns

સ્થિર event_T event_name2nr(char_u *start, char_u **end);
સ્થિર string_T *event_nr2name(event_T event);
સ્થિર પૂર્ણાંક au_get_grouparg(char_u **argp);
સ્થિર પૂર્ણાંક do_autocmd_event(event_T event, char_u *pat, પૂર્ણાંક once, પૂર્ણાંક nested, char_u *cmd, પૂર્ણાંક forceit, પૂર્ણાંક group, પૂર્ણાંક flags);
સ્થિર પૂર્ણાંક apply_autocmds_group(event_T event, char_u *fname, char_u *fname_io, પૂર્ણાંક force, પૂર્ણાંક group, buf_T *buf, exarg_T *eap);
સ્થિર શૂન્ય auto_next_pat(AutoPatCmd_T *apc, પૂર્ણાંક stop_at_last);
સ્થિર પૂર્ણાંક au_find_group(char_u *name);

સ્થિર event_T	last_event;
સ્થિર પૂર્ણાંક	last_group;
સ્થિર પૂર્ણાંક	autocmd_blocked = 0;	// block all autocmds

    સ્થિર char_u *
get_deleted_augroup(શૂન્ય)
{
    જો (deleted_augroup == ખાલી)
	deleted_augroup = (char_u *)_("--Deleted--");
    પાછા deleted_augroup;
}

/*
 * Show the autocommands for one AutoPat.
 */
    સ્થિર શૂન્ય
show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    // Check for "got_int" (here and at various places below), which is set
    // when "q" has been hit for the "--more--" prompt
    જો (got_int)
	પાછા;
    જો (ap->pat == ખાલી)		// pattern has been removed
	પાછા;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    સંદેશ_putchar('\n');
    જો (got_int)
	જાઓ theend;
    જો (event != last_event || ap->group != last_group)
    {
	string_T    *event_name;

	જો (ap->group != AUGROUP_DEFAULT)
	{
	    જો (AUGROUP_NAME(ap->group) == ખાલી)
		સંદેશ_puts_attr((અક્ષર *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    નહીંતર
		સંદેશ_puts_attr((અક્ષર *)AUGROUP_NAME(ap->group), HL_ATTR(HLF_T));
	    સંદેશ_puts("  ");
	}
	event_name = event_nr2name(event);
	સંદેશ_puts_attr((અક્ષર *)event_name->string, HL_ATTR(HLF_T));
	last_event = event;
	last_group = ap->group;
	સંદેશ_putchar('\n');
	જો (got_int)
	    જાઓ theend;
    }

    જો (ap->pat == ખાલી)
	જાઓ theend;  // timer might have cleared the pattern or group

    સંદેશ_col = 4;
    સંદેશ_outtrans(ap->pat);

    માટે (ac = ap->cmds; ac != ખાલી; ac = ac->next)
    {
	જો (ac->cmd == ખાલી)		// skip removed commands
	    ચાલુ;

	જો (સંદેશ_col >= 14)
	    સંદેશ_putchar('\n');
	સંદેશ_col = 14;
	જો (got_int)
	    જાઓ theend;
	સંદેશ_outtrans(ac->cmd);
#ifdef FEAT_EVAL
	જો (p_verbose > 0)
	    last_set_સંદેશ(ac->script_ctx);
#endif
	જો (got_int)
	    જાઓ theend;
	જો (ac->next != ખાલી)
	{
	    સંદેશ_putchar('\n');
	    જો (got_int)
		જાઓ theend;
	}
    }

theend:
    --autocmd_busy;
}

/*
 * Mark an autocommand pattern for deletion.
 */
    સ્થિર શૂન્ય
au_remove_pat(AutoPat *ap)
{
    VIM_CLEAR(ap->pat);
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    સ્થિર શૂન્ય
au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    માટે (ac = ap->cmds; ac != ખાલી; ac = ac->next)
	VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

// Delete one command from an autocmd pattern.
સ્થિર શૂન્ય au_del_cmd(AutoCmd *ac)
{
    VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    સ્થિર શૂન્ય
au_cleanup(શૂન્ય)
{
    AutoPat	*ap, **prev_ap;
    AutoCmd	*ac, **prev_ac;
    event_T	event;

    જો (autocmd_busy || !au_need_clean)
	પાછા;

    // loop over all events
    માટે (event = (event_T)0; (પૂર્ણાંક)event < અંક_EVENTS;
					    event = (event_T)((પૂર્ણાંક)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(first_autopat[(પૂર્ણાંક)event]);
	માટે (ap = *prev_ap; ap != ખાલી; ap = *prev_ap)
	{
	    પૂર્ણાંક has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    માટે (ac = *prev_ac; ac != ખાલી; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		જો (ap->pat == ખાલી || ac->cmd == ખાલી)
		{
		    *prev_ac = ac->next;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		નહીંતર
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->next);
		}
	    }

	    જો (ap->pat != ખાલી && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    જો (ap->pat == ખાલી)
	    {
		જો (ap->next == ખાલી)
		{
		    જો (prev_ap == &(first_autopat[(પૂર્ણાંક)event]))
			last_autopat[(પૂર્ણાંક)event] = ખાલી;
		    નહીંતર
			// this depends on the "next" field being the first in
			// the struct
			last_autopat[(પૂર્ણાંક)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->next;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    નહીંતર
		prev_ap = &(ap->next);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Called when buffer is freed, to remove/invalidate related buffer-local
 * autocmds.
 */
    શૂન્ય
aubuflocal_remove(buf_T *buf)
{
    AutoPat	    *ap;
    event_T	    event;
    AutoPatCmd_T    *apc;

    // invalidate currently executing autocommands
    માટે (apc = active_apc_list; apc; apc = apc->next)
	જો (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    માટે (event = (event_T)0; (પૂર્ણાંક)event < અંક_EVENTS;
					    event = (event_T)((પૂર્ણાંક)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    જો (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		જો (p_verbose >= 6)
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
    સ્થિર પૂર્ણાંક
au_new_group(char_u *name)
{
    પૂર્ણાંક		i;

    i = au_find_group(name);
    જો (i != AUGROUP_ERROR)
	પાછા i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    માટે (i = 0; i < augroups.ga_len; ++i)
	જો (AUGROUP_NAME(i) == ખાલી)
	    તોડો;
    જો (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	પાછા AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    જો (AUGROUP_NAME(i) == ખાલી)
	પાછા AUGROUP_ERROR;
    જો (i == augroups.ga_len)
	++augroups.ga_len;

    પાછા i;
}

    સ્થિર શૂન્ય
au_del_group(char_u *name)
{
    પૂર્ણાંક		i;
    event_T	event;
    AutoPat	*ap;
    પૂર્ણાંક		in_use = FALSE;


    i = au_find_group(name);
    જો (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_group_str), name);
	પાછા;
    }
    જો (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_group));
	પાછા;
    }

    માટે (event = (event_T)0; (પૂર્ણાંક)event < અંક_EVENTS;
	    event = (event_T)((પૂર્ણાંક)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    જો (ap->group == i && ap->pat != ખાલી)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		in_use = TRUE;
		event = અંક_EVENTS;
		તોડો;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    જો (in_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    નહીંતર
	AUGROUP_NAME(i) = ખાલી;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    સ્થિર પૂર્ણાંક
au_find_group(char_u *name)
{
    પૂર્ણાંક	    i;

    માટે (i = 0; i < augroups.ga_len; ++i)
	જો (AUGROUP_NAME(i) != ખાલી && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    પાછા i;
    પાછા AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    પૂર્ણાંક
au_has_group(char_u *name)
{
    પાછા au_find_group(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    શૂન્ય
do_augroup(char_u *arg, પૂર્ણાંક del_group)
{
    પૂર્ણાંક	    i;

    જો (del_group)
    {
	જો (*arg == NUL)
	    emsg(_(e_argument_required));
	નહીંતર
	    au_del_group(arg);
    }
    નહીંતર જો (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    નહીંતર જો (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_group(arg);
	જો (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    નહીંતર			    // ":aug": list the group names
    {
	સંદેશ_start();
	માટે (i = 0; i < augroups.ga_len; ++i)
	{
	    જો (AUGROUP_NAME(i) != ખાલી)
	    {
		સંદેશ_puts((અક્ષર *)AUGROUP_NAME(i));
		સંદેશ_puts("  ");
	    }
	}
	સંદેશ_clr_eos();
	સંદેશ_end();
    }
}

    શૂન્ય
autocmd_init(શૂન્ય)
{
    CLEAR_FIELD(aucmd_win);
}

#જો defined(EXITFREE)
    શૂન્ય
free_all_autocmds(શૂન્ય)
{
    char_u	*s;

    માટે (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(ખાલી, (char_u *)"", TRUE);

    માટે (પૂર્ણાંક i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_data))[i];
	જો (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#endif

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    પૂર્ણાંક
is_aucmd_win(win_T *win)
{
    માટે (પૂર્ણાંક i = 0; i < AUCMD_WIN_ગણતરી; ++i)
	જો (aucmd_win[i].auc_win_used && aucmd_win[i].auc_win == win)
	    પાછા TRUE;
    પાછા FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    સ્થિર event_T
event_name2nr(char_u *start, char_u **end)
{
    char_u	*p;
    keyvalue_T target;
    keyvalue_T *entry;
    સ્થિર keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_INDEX];
    સ્થિર keyvalue_T *bufread = &event_tab[BUFREAD_INDEX];

    // the event name ends with end of line, '|', a blank or a comma
    માટે (p = start; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    target.ચાવી = 0;
    target.કિંમત.string = start;
    target.કિંમત.length = (size_t)(p - start);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    જો (cmp_keyvalue_કિંમત_ni(&target, bufnewfile) == 0)
	entry = bufnewfile;
    નહીંતર જો (cmp_keyvalue_કિંમત_ni(&target, bufread) == 0)
	entry = bufread;
    નહીંતર
	entry = (keyvalue_T *)bsearch(&target, &event_tab, અંક_EVENTS,
				કદ(event_tab[0]), cmp_keyvalue_કિંમત_ni);

    જો (*p == ',')
	++p;
    *end = p;

    પાછા (entry == ખાલી) ? અંક_EVENTS : (event_T)abs(entry->ચાવી);
}

/*
 * Return the name for event "event".
 */
    સ્થિર string_T *
event_nr2name(event_T event)
{
    પૂર્ણાંક	    i;
    ગણના {CACHE_SIZE = 12};
    સ્થિર પૂર્ણાંક cache_tab[CACHE_SIZE];
    સ્થિર પૂર્ણાંક cache_last_index = -1;
    સ્થિર string_T unknown = STR_LITERAL_INIT("Unknown");

    જો (cache_last_index < 0)
    {
	માટે (i = 0; i < CACHE_SIZE; ++i)
	    cache_tab[i] = -1;
	cache_last_index = CACHE_SIZE - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    માટે (i = cache_last_index; cache_tab[i] >= 0; )
    {
	જો ((event_T)abs(event_tab[cache_tab[i]].ચાવી) == event)
	    પાછા &event_tab[cache_tab[i]].કિંમત;

	જો (i == 0)
	    i = CACHE_SIZE - 1;
	નહીંતર
	    --i;

	// are we back at the start?
	જો (i == cache_last_index)
	    તોડો;
    }

    // look in the event table itself
    માટે (i = 0; i < અંક_EVENTS; ++i)
    {
	જો ((event_T)abs(event_tab[i].ચાવી) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    જો (cache_last_index == CACHE_SIZE - 1)
		cache_last_index = 0;
	    નહીંતર
		++cache_last_index;
	    cache_tab[cache_last_index] = i;
	    પાછા &event_tab[i].કિંમત;
	}
    }

    પાછા &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    સ્થિર char_u *
find_end_event(
    char_u  *arg,
    પૂર્ણાંક	    have_group)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    જો (*arg == '*')
    {
	જો (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_star_str), arg);
	    પાછા ખાલી;
	}
	pat = arg + 1;
    }
    નહીંતર
    {
	માટે (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    જો ((પૂર્ણાંક)event_name2nr(pat, &p) >= અંક_EVENTS)
	    {
		જો (have_group)
		    semsg(_(e_no_such_event_str), pat);
		નહીંતર
		    semsg(_(e_no_such_group_or_event_str), pat);
		પાછા ખાલી;
	    }
	}
    }
    પાછા pat;
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
    પૂર્ણાંક		group;

    જો (*arg == '|')
	પાછા ખાલી;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    group = au_get_grouparg(&arg);
    pat = group == AUGROUP_ERROR
			  ? ખાલી : find_end_event(arg, group != AUGROUP_ALL);
    --emsg_off;
    જો (pat == ખાલી)
	પાછા ખાલી;

    pat = skipwhite(pat);
    જો (*pat == NUL || *pat == '|')
	પાછા ખાલી;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    જ્યારે (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    માટે (પૂર્ણાંક i = 0; i < 2; ++i)
    {
	જો (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	જો (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	જો (!in_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    પાછા *cmd == NUL ? ખાલી : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    પૂર્ણાંક
event_ignored(event_T event, char_u *ei)
{
    પૂર્ણાંક ignored = FALSE;
    જ્યારે (*ei != NUL)
    {
	પૂર્ણાંક unignore = *ei == '-';
	ei += unignore;
	જો (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].ચાવી <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	નહીંતર જો (event_name2nr(ei, &ei) == event)
	{
	    જો (unignore)
		પાછા FALSE;
	    નહીંતર
		ignored = TRUE;
	}
    }

    પાછા ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    પૂર્ણાંક
check_ei(char_u *ei)
{
    પૂર્ણાંક	win = ei != p_ei;

    જ્યારે (*ei)
    {
	જો (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	નહીંતર
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    જો (event == અંક_EVENTS || (win && event_tab[event].ચાવી > 0))
		પાછા FAIL;
	}
    }

    પાછા OK;
}

#જો defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(અક્ષર *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    size_t	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    જો (save_ei == ખાલી)
	પાછા ખાલી;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    જો (new_ei == ખાલી)
    {
	vim_free(save_ei);
	પાછા ખાલી;
    }

    જો (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    નહીંતર
	STRCPY(new_ei + p_ei_len, what);
    set_string_option_direct((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    પાછા save_ei;
}

    શૂન્ય
au_event_restore(char_u *old_ei)
{
    જો (old_ei != ખાલી)
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
    શૂન્ય
do_autocmd(exarg_T *eap, char_u *arg_in, પૂર્ણાંક forceit)
{
    char_u	*arg = arg_in;
    char_u	*pat;
    char_u	*envpat = ખાલી;
    char_u	*cmd;
    પૂર્ણાંક		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = ખાલી;
    પૂર્ણાંક		nested = FALSE;
    પૂર્ણાંક		once = FALSE;
    પૂર્ણાંક		group;
    પૂર્ણાંક		i;
    પૂર્ણાંક		flags = 0;

    જો (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	group = AUGROUP_ALL;	// no argument, use all groups
    }
    નહીંતર
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	group = au_get_grouparg(&arg);
	જો (arg == ખાલી)	    // out of memory
	    પાછા;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = find_end_event(arg, group != AUGROUP_ALL);
    જો (pat == ખાલી)
	પાછા;

    pat = skipwhite(pat);
    જો (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    નહીંતર
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	જ્યારે (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	જો (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	જો (vim_strchr(pat, '$') != ખાલી || vim_strchr(pat, '~') != ખાલી)
	{
#ifdef BACKSLASH_IN_FILENAME
	    પૂર્ણાંક	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#endif
	    envpat = expand_env_save(pat);
#ifdef BACKSLASH_IN_FILENAME
	    p_ssl = p_ssl_save;
#endif
	    જો (envpat != ખાલી)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	માટે (i = 0; i < 2; i++)
	{
	    જો (*cmd == NUL)
		ચાલુ;

	    // Check for "++once" flag.
	    જો (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		જો (once)
		{
		    semsg(_(e_duplicate_argument_str), "++once");
		    જાઓ err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    જો ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		જો (nested)
		{
		    semsg(_(e_duplicate_argument_str), "++nested");
		    જાઓ err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    જો (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		જો (in_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_mean_plusplus_nested));
		    જાઓ err_exit;
		}
		જો (nested)
		{
		    semsg(_(e_duplicate_argument_str), "nested");
		    જાઓ err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 6);
	    }
	}

	/*
	 * Find the start of the commands.
	 * Expand <sfile> in it.
	 */
	જો (*cmd != NUL)
	{
	    જો (eap != ખાલી)
		// Read a {} block if it follows.
		cmd = may_get_cmd_block(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    જો (cmd == ખાલી)	    // some error
		જાઓ err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    જો (!forceit && *cmd == NUL)
	// Highlight title
	સંદેશ_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    last_event = (event_T)-1;		// for listing the event name
    last_group = AUGROUP_ERROR;		// for listing the group name
    જો (*arg == '*' || *arg == NUL || *arg == '|')
    {
	જો (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	નહીંતર
	    માટે (event = (event_T)0; (પૂર્ણાંક)event < અંક_EVENTS;
					     event = (event_T)((પૂર્ણાંક)event + 1))
		જો (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, group, flags) == FAIL)
		    તોડો;
    }
    નહીંતર
    {
	જ્યારે (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    જો (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, group, flags) == FAIL)
		તોડો;
    }

err_exit:
    જો (cmd_need_free)
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
    સ્થિર પૂર્ણાંક
au_get_grouparg(char_u **argp)
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    પૂર્ણાંક		group = AUGROUP_ALL;

    માટે (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    જો (p <= arg)
	પાછા AUGROUP_ALL;

    group_name = vim_strnsave(arg, p - arg);
    જો (group_name == ખાલી)		// out of memory
	પાછા AUGROUP_ERROR;
    group = au_find_group(group_name);
    જો (group == AUGROUP_ERROR)
	group = AUGROUP_ALL;	// no match, use all groups
    નહીંતર
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(group_name);
    પાછા group;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    સ્થિર પૂર્ણાંક
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    પૂર્ણાંક		once,
    પૂર્ણાંક		nested,
    char_u	*cmd,
    પૂર્ણાંક		forceit,
    પૂર્ણાંક		group,
    પૂર્ણાંક		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    પૂર્ણાંક		brace_level;
    char_u	*endpat;
    પૂર્ણાંક		findgroup;
    પૂર્ણાંક		allgroups;
    પૂર્ણાંક		patlen;
    પૂર્ણાંક		is_buflocal;
    પૂર્ણાંક		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    જો (group == AUGROUP_ALL)
	findgroup = current_augroup;
    નહીંતર
	findgroup = group;
    allgroups = (group == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    જો (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    જો (forceit)  // delete the AutoPat, if it's in the current group
	    {
		જો (ap->group == findgroup)
		    au_remove_pat(ap);
	    }
	    નહીંતર જો (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    માટે ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	માટે (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    જો (*endpat == '{')
		brace_level++;
	    નહીંતર જો (*endpat == '}')
		brace_level--;
	}
	જો (pat == endpat)		// ignore single comma
	    ચાલુ;
	patlen = (પૂર્ણાંક)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	જો (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    જો (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    નહીંતર જો (patlen > 9 && pat[7] == '=')
	    {
		જો (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		નહીંતર જો (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((અક્ષર *)pat + 8);
	    }
	}

	જો (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((અક્ષર *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (પૂર્ણાંક)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	જો (!forceit && *cmd != NUL && last_autopat[(પૂર્ણાંક)event] != ખાલી)
	    prev_ap = &last_autopat[(પૂર્ણાંક)event];
	નહીંતર
	    prev_ap = &first_autopat[(પૂર્ણાંક)event];
	જ્યારે ((ap = *prev_ap) != ખાલી)
	{
	    જો (ap->pat != ખાલી)
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
		જો ((allgroups || ap->group == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    જો (forceit)
		    {
			જો (*cmd != NUL && ap->next == ખાલી)
			{
			    au_remove_cmds(ap);
			    તોડો;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    નહીંતર જો (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    નહીંતર જો (ap->next == ખાલી)
			તોડો;
		}
	    }
	    prev_ap = &ap->next;
	}

	/*
	 * Add a new command.
	 */
	જો (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    જો (ap == ખાલી)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		જો (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == ખાલી))
		{
		    semsg(_(e_buffer_nr_invalid_buffer_સંખ્યા), buflocal_nr);
		    પાછા FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		જો (ap == ખાલી)
		    પાછા FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		જો (ap->pat == ખાલી)
		{
		    vim_free(ap);
		    પાછા FAIL;
		}

#ifdef FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		જો (event == EVENT_MODECHANGED && !has_modechanged())
		    get_mode(last_mode);
#endif
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		જો ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
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

		જો (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = ખાલી;
		}
		નહીંતર
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_to_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    જો (reg_pat != ખાલી)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    જો (reg_pat == ખાલી || ap->reg_prog == ખાલી)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			પાછા FAIL;
		    }
		}
		ap->cmds = ખાલી;
		*prev_ap = ap;
		last_autopat[(પૂર્ણાંક)event] = ap;
		ap->next = ખાલી;
		જો (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		નહીંતર
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    જ્યારે ((ac = *prev_ac) != ખાલી)
		prev_ac = &ac->next;
	    ac = ALLOC_ONE(AutoCmd);
	    જો (ac == ખાલી)
		પાછા FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    જો (flags & UC_VIM9)
		ac->script_ctx.sc_version = SCRIPT_VERSION_VIM9;
#ifdef FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#endif
	    જો (ac->cmd == ખાલી)
	    {
		vim_free(ac);
		પાછા FAIL;
	    }
	    ac->next = ખાલી;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    પાછા OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    પૂર્ણાંક
do_doautocmd(
    char_u	*arg_start,
    પૂર્ણાંક		do_સંદેશ,	    // give message for no matching autocmds?
    પૂર્ણાંક		*did_something)
{
    char_u	*arg = arg_start;
    char_u	*fname;
    પૂર્ણાંક		nothing_done = TRUE;
    પૂર્ણાંક		group;

    જો (did_something != ખાલી)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    જો (arg == ખાલી)	    // out of memory
	પાછા FAIL;

    જો (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	પાછા FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg, group != AUGROUP_ALL);
    જો (fname == ખાલી)
	પાછા FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    જ્યારે (*arg && !ends_excmd(*arg) && !VIM_ISWHITE(*arg))
	જો (apply_autocmds_group(event_name2nr(arg, &arg),
				      fname, ખાલી, TRUE, group, curbuf, ખાલી))
	    nothing_done = FALSE;

    જો (nothing_done && do_સંદેશ
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       )
	smsg(_("No matching autocommands: %s"), arg_start);
    જો (did_something != ખાલી)
	*did_something = !nothing_done;

#ifdef FEAT_EVAL
    પાછા aborting() ? FAIL : OK;
#નહીંતર
    પાછા OK;
#endif
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    શૂન્ય
ex_doautoall(exarg_T *eap)
{
    પૂર્ણાંક		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    પૂર્ણાંક		call_do_modelines = check_nomodeline(&arg);
    પૂર્ણાંક		did_aucmd;

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
	જો (buf->b_ml.ml_mfp == ખાલી || buf == curbuf)
	    ચાલુ;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	જો (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    તોડો;
	}

	set_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	જો (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	જો (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    તોડો;
	}
    }

    // Execute autocommands for the current buffer last.
    જો (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	જો (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    પૂર્ણાંક
check_nomodeline(char_u **argp)
{
    જો (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	પાછા FALSE;
    }
    પાછા TRUE;
}

/*
 * Prepare for executing autocommands for (hidden) buffer "buf".
 * Search for a visible window containing the current buffer.  If there isn't
 * one then use an entry in "aucmd_win[]".
 * Set "curbuf" and "curwin" to match "buf".
 * When this fails "curbuf" is not equal "buf".
 */
    શૂન્ય
aucmd_prepbuf(
    aco_save_T	*aco,		// structure to save values in
    buf_T	*buf)		// new curbuf
{
    win_T	*win;
    પૂર્ણાંક		save_ea;
#ifdef FEAT_AUTOCHDIR
    પૂર્ણાંક		save_acd;
#endif
    પૂર્ણાંક		same_buffer = buf == curbuf;

    // Find a window that is for the new buffer
    જો (same_buffer)		// be quick when buf is curbuf
	win = curwin;
    નહીંતર
	FOR_ALL_WINDOWS(win)
	    જો (win->w_buffer == buf)
		તોડો;

    // Allocate a window when needed.
    win_T *auc_win = ખાલી;
    પૂર્ણાંક auc_idx = AUCMD_WIN_ગણતરી;
    જો (win == ખાલી)
    {
	માટે (auc_idx = 0; auc_idx < AUCMD_WIN_ગણતરી; ++auc_idx)
	    જો (!aucmd_win[auc_idx].auc_win_used)
	    {
		જો (aucmd_win[auc_idx].auc_win == ખાલી)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		જો (auc_win != ખાલી)
		    aucmd_win[auc_idx].auc_win_used = TRUE;
		તોડો;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	જો (auc_win == ખાલી)
	    પાછા;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == ખાલી ? 0 : prevwin->w_id;
#ifdef FEAT_JOB_CHANNEL
    જો (bt_prompt(curbuf))
	aco->save_prompt_ઉમેરો = curbuf->b_prompt_ઉમેરો;
#endif

    જો (win != ખાલી)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    નહીંતર
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
	curtab->tp_localdir = ખાલી;
	aco->globaldir = globaldir;
	globaldir = ખાલી;

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

	(શૂન્ય)win_split_ins(0, WSP_ટોચ | WSP_FORCE_ROOM, auc_win, 0, ખાલી);
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
    જો (!same_buffer)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_active = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    શૂન્ય
aucmd_restbuf(
    aco_save_T	*aco)		// structure holding saved values
{
    પૂર્ણાંક	    dummy;
    win_T   *save_curwin;

    જો (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	block_autocmds();
	જો (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		જો (wp == awp)
		{
		    જો (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(awp);
		    જાઓ win_found;
		}
	    }
	}
win_found:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(શૂન્ય)winframe_remove(curwin, &dummy, ખાલી, ખાલી);
	win_remove(curwin, ખાલી);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_used = FALSE;
	last_status(FALSE);	    // may need to remove last status line

	જો (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_find_by_id(aco->save_curwin_id);
	જો (save_curwin != ખાલી)
	    curwin = save_curwin;
	નહીંતર
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_window(curwin);
	જો (bt_prompt(curbuf))
	    curbuf->b_prompt_ઉમેરો = aco->save_prompt_ઉમેરો;
#endif
	prevwin = win_find_by_id(aco->save_prevwin_id);
#ifdef FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#endif
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	જો (awp->w_localdir != ખાલી)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_active = aco->save_VIsual_active;
	check_cursor();
	જો (curwin->w_topline > curbuf->b_ml.ml_લીટી_ગણતરી)
	{
	    curwin->w_topline = curbuf->b_ml.ml_લીટી_ગણતરી;
#ifdef FEAT_DIFF
	    curwin->w_topfill = 0;
#endif
	}
#જો defined(FEAT_GUI)
	જો (gui.in_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_ડાબું], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_જમણું], FALSE);
	    gui_may_update_scrollbars();
	}
#endif
    }
    નહીંતર
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_find_by_id(aco->save_curwin_id);
	જો (save_curwin != ખાલી)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    જો (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != ખાલી)
	    {
#જો defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		જો (curwin->w_s == &curbuf->b_s)
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
    જો (VIsual_active)
	check_pos(curbuf, &VIsual);
}

સ્થિર પૂર્ણાંક	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    પૂર્ણાંક
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    પૂર્ણાંક		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    પાછા apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ખાલી);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    પૂર્ણાંક
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    પૂર્ણાંક		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    પાછા apply_autocmds_group(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    પૂર્ણાંક
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    પૂર્ણાંક		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    પૂર્ણાંક		*retval)    // pointer to caller's retval
{
    પૂર્ણાંક		did_cmd;

#ifdef FEAT_EVAL
    જો (should_abort(*retval))
	પાછા FALSE;
#endif

    did_cmd = apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ખાલી);
    જો (did_cmd
#ifdef FEAT_EVAL
	    && aborting()
#endif
	    )
	*retval = FAIL;
    પાછા did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    સ્થિર પૂર્ણાંક
has_cursorhold(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)(get_real_state() == MODE_NORMAL_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != ખાલી);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    પૂર્ણાંક
trigger_cursorhold(શૂન્ય)
{
    પૂર્ણાંક		state;

    જો (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_active())
    {
	state = get_real_state();
	જો (state == MODE_NORMAL_BUSY || (state & MODE_ઉમેરો) != 0)
	    પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    પૂર્ણાંક
has_winresized(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_WINRESIZED] != ખાલી);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    પૂર્ણાંક
has_winscrolled(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_WINSCROLLED] != ખાલી);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    પૂર્ણાંક
has_cursormoved(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_CURSORMOVED] != ખાલી);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    પૂર્ણાંક
has_cursormovedI(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_CURSORMOVEDI] != ખાલી);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    પૂર્ણાંક
has_textchanged(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_TEXTCHANGED] != ખાલી);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    પૂર્ણાંક
has_textchangedI(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_TEXTCHANGEDI] != ખાલી);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    પૂર્ણાંક
has_textchangedP(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_TEXTCHANGEDP] != ખાલી);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    પૂર્ણાંક
has_insertcharpre(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_INSERTCHARPRE] != ખાલી);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    પૂર્ણાંક
has_keyinputpre(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_KEYINPUTPRE] != ખાલી);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    પૂર્ણાંક
has_cmdundefined(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_CMDUNDEFINED] != ખાલી);
}

#જો defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    પૂર્ણાંક
has_textputpost(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_TEXTPUTPOST] != ખાલી);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    પૂર્ણાંક
has_textputpre(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_TEXTPUTPRE] != ખાલી);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    પૂર્ણાંક
has_textyankpost(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_TEXTYANKPOST] != ખાલી);
}
#endif

#જો defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    પૂર્ણાંક
has_completechanged(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_COMPLETECHANGED] != ખાલી);
}
#endif

#જો defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    પૂર્ણાંક
has_modechanged(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_MODECHANGED] != ખાલી);
}
#endif

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    સ્થિર પૂર્ણાંક
apply_autocmds_group(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    પૂર્ણાંક		force,	     // when TRUE, ignore autocmd_busy
    પૂર્ણાંક		group,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap UNUSED) // command arguments
{
    char_u	*sfname = ખાલી;	// short file name
    char_u	*tail;
    પૂર્ણાંક		save_changed;
    buf_T	*old_curbuf;
    પૂર્ણાંક		retval = FALSE;
    char_u	*save_autocmd_fname;
    પૂર્ણાંક		save_autocmd_fname_full;
    પૂર્ણાંક		save_autocmd_bufnr;
    char_u	*save_autocmd_match;
    પૂર્ણાંક		save_autocmd_busy;
    પૂર્ણાંક		save_autocmd_nested;
    સ્થિર પૂર્ણાંક	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ifdef FEAT_EVAL
    funccal_entry_T funccal_entry;
    char_u	*save_cmdarg;
    long	save_cmdbang;
#endif
    સ્થિર પૂર્ણાંક	filechangeshell_busy = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif
    પૂર્ણાંક		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    પૂર્ણાંક		save_ચાવી_Typed = ચાવી_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    જો (event == અંક_EVENTS || first_autopat[(પૂર્ણાંક)event] == ખાલી
	    || autocmd_blocked > 0)
	જાઓ BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    જો (autocmd_busy && !(force || autocmd_nested))
	જાઓ BYPASS_AU;

#ifdef FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    જો (aborting())
	જાઓ BYPASS_AU;
#endif

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    જો (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	જાઓ BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    જો (event_ignored(event, p_ei))
	જાઓ BYPASS_AU;

    પૂર્ણાંક win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    જો (buf == curbuf && event_tab[event].ચાવી <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    નહીંતર જો (buf != ખાલી && event_tab[event].ચાવી <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    જો (wp->w_buffer == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		તોડો;
	    }
    }
    જો (win_ignore)
	જાઓ BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    જો (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	જાઓ BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    જો (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	જાઓ BYPASS_AU;

    જો (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_changed_ગણતરી;

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
    જો (fname_io == ખાલી)
    {
	જો (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = ખાલી;
	નહીંતર જો (fname != ખાલી && !ends_excmd(*fname))
	    autocmd_fname = fname;
	નહીંતર જો (buf != ખાલી)
	    autocmd_fname = buf->b_ffname;
	નહીંતર
	    autocmd_fname = ખાલી;
    }
    નહીંતર
	autocmd_fname = fname_io;
    જો (autocmd_fname != ખાલી)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_full = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    જો (buf == ખાલી)
	autocmd_bufnr = 0;
    નહીંતર
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    જો (fname == ખાલી || *fname == NUL)
    {
	જો (buf == ખાલી)
	    fname = ખાલી;
	નહીંતર
	{
#ifdef FEAT_SYN_HL
	    જો (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    નહીંતર
#endif
		જો (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		નહીંતર
		{
		    જો (buf->b_sfname != ખાલી)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	જો (fname == ખાલી)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    નહીંતર
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	જો (event == EVENT_FILETYPE
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
	નહીંતર
	    fname = FullName_save(fname, FALSE);
    }
    જો (fname == ખાલી)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	જાઓ BYPASS_AU;
    }

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    જો (sfname != ખાલી)
	forward_slash(sfname);
    forward_slash(fname);
#endif

#ifdef VMS
    // remove version for correct match
    જો (sfname != ખાલી)
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
    estack_ધકેલો(ETYPE_AUCMD, ખાલી, 0);
    ESTACK_CHECK_SETUP;

    save_current_sctx = current_sctx;

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    જો (do_profiling == PROF_YES)
	prof_child_enter(&wait_time); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_entry);
#endif

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    જો (!autocmd_busy)
    {
	save_શોધ_patterns();
	જો (!ins_compl_active())
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
    જો (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    tail = gettail(fname);

    // Find first autocommand that matches
    CLEAR_FIELD(patcmd);
    patcmd.curpat = first_autopat[(પૂર્ણાંક)event];
    patcmd.group = group;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.tail = tail;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_next_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    જો (patcmd.curpat != ખાલી)
    {
	// add to active_apc_list
	patcmd.next = active_apc_list;
	active_apc_list = &patcmd;

#ifdef FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (long)get_vim_var_nr(VV_CMDBANG);
	જો (eap != ખાલી)
	{
	    save_cmdarg = set_cmdarg(eap, ખાલી);
	    set_vim_var_nr(VV_CMDBANG, (long)eap->forceit);
	}
	નહીંતર
	    save_cmdarg = ખાલી;	// avoid gcc warning
#endif
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	માટે (ap = patcmd.curpat; ap->next != ખાલી; ap = ap->next)
	    ap->last = FALSE;
	ap->last = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	જો (nesting == 1)
	    check_lnums(TRUE);
	નહીંતર
	    check_lnums_nested(TRUE);

	પૂર્ણાંક save_did_emsg = did_emsg;
	પૂર્ણાંક save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(ખાલી, getnextac, (શૂન્ય *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	set_pressedreturn(save_ex_pressedreturn);

	જો (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ifdef FEAT_EVAL
	જો (eap != ખાલી)
	{
	    (શૂન્ય)set_cmdarg(ખાલી, save_cmdarg);
	    set_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#endif
	// delete from active_apc_list
	જો (active_apc_list == &patcmd)	    // just in case
	    active_apc_list = patcmd.next;
    }

    જો (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_કાઢો();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_full = save_autocmd_fname_full;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_match = save_autocmd_match;
    current_sctx = save_current_sctx;
#ifdef FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    જો (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif
    ચાવી_Typed = save_ચાવી_Typed;
    vim_free(fname);
    vim_free(sfname);
    --nesting;		// see matching increment above

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.  Free any buffers in the au_pending_free_buf list and
     * free any windows in the au_pending_free_win list.
     */
    જો (!autocmd_busy)
    {
	restore_શોધ_patterns();
	જો (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	જ્યારે (au_pending_free_buf != ખાલી)
	{
	    buf_T *b = au_pending_free_buf->b_next;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	જ્યારે (au_pending_free_win != ખાલી)
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
    જો (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	જો (curbuf->b_changed != save_changed)
	    need_maketitle = TRUE;
	curbuf->b_changed = save_changed;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    જો (event == EVENT_BUFWIPEOUT && buf != ખાલી)
	aubuflocal_remove(buf);

    જો (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    પાછા retval;
}

#ifdef FEAT_EVAL
સ્થિર char_u	*old_termresponse = ખાલી;
સ્થિર char_u	*old_termu7resp = ખાલી;
સ્થિર char_u	*old_termblinkresp = ખાલી;
સ્થિર char_u	*old_termrbgresp = ખાલી;
સ્થિર char_u	*old_termrfgresp = ખાલી;
સ્થિર char_u	*old_termstyleresp = ખાલી;
#endif

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    શૂન્ય
block_autocmds(શૂન્ય)
{
#ifdef FEAT_EVAL
    // Remember the value of v:termresponse.
    જો (autocmd_blocked == 0)
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

    શૂન્ય
unblock_autocmds(શૂન્ય)
{
    --autocmd_blocked;

#ifdef FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    જો (autocmd_blocked == 0)
    {
	જો (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, ખાલી, ખાલી, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", ખાલી, FALSE, curbuf);
	}
	જો (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", ખાલી, FALSE, curbuf);
	}
	જો (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", ખાલી, FALSE, curbuf);
	}
	જો (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", ખાલી, FALSE, curbuf);
	}
	જો (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", ખાલી, FALSE, curbuf);
	}
	જો (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", ખાલી, FALSE, curbuf);
	}
    }
#endif
}

    પૂર્ણાંક
is_autocmd_blocked(શૂન્ય)
{
    પાછા autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    સ્થિર શૂન્ય
auto_next_pat(
    AutoPatCmd_T *apc,
    પૂર્ણાંક		stop_at_last)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*entry;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(entry->es_name);
    entry->es_info.aucmd = ખાલી;

    માટે (ap = apc->curpat; ap != ખાલી && !got_int; ap = ap->next)
    {
	apc->curpat = ખાલી;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	જો (ap->pat != ખાલી && ap->cmds != ખાલી
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
	    // execution-condition
	    જો (ap->buflocal_nr == 0
		    ? (match_file_pat(ખાલી, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->tail, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		અક્ષર	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->length + ap->patlen + 1);
		જો (namep != ખાલી)
		{
		    sprintf((અક્ષર *)namep, fmt, (અક્ષર *)event_name->string, (અક્ષર *)ap->pat);
		    જો (p_verbose >= 8)
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
		માટે (cp = ap->cmds; cp->next != ખાલી; cp = cp->next)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    લીટી_breakcheck();
	    જો (apc->curpat != ખાલી)	    // found a match
		તોડો;
	}
	જો (stop_at_last && ap->last)
	    તોડો;
    }
}

#જો defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    પાછા &acp->script_ctx;
}
#endif

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	પૂર્ણાંક c UNUSED,
	શૂન્ય *cookie,
	પૂર્ણાંક indent UNUSED,
	getline_opt_T options UNUSED)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    જો (acp->curpat == ખાલી)
	પાછા ખાલી;

    // repeat until we find an autocommand to execute
    માટે (;;)
    {
	// skip removed commands
	જ્યારે (acp->nextcmd != ખાલી && acp->nextcmd->cmd == ખાલી)
	    જો (acp->nextcmd->last)
		acp->nextcmd = ખાલી;
	    નહીંતર
		acp->nextcmd = acp->nextcmd->next;

	જો (acp->nextcmd != ખાલી)
	    તોડો;

	// at end of commands, find next pattern that matches
	જો (acp->curpat->last)
	    acp->curpat = ખાલી;
	નહીંતર
	    acp->curpat = acp->curpat->next;
	જો (acp->curpat != ખાલી)
	    auto_next_pat(acp, TRUE);
	જો (acp->curpat == ખાલી)
	    પાછા ખાલી;
    }

    ac = acp->nextcmd;

    જો (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	સંદેશ_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    જો (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    જો (ac->last)
	acp->nextcmd = ખાલી;
    નહીંતર
	acp->nextcmd = ac->next;
    પાછા retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    સ્થિર પૂર્ણાંક
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_full)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*tail = gettail(sfname);
    પૂર્ણાંક		retval = FALSE;

    જો (first_autopat[(પૂર્ણાંક)event] == ખાલી)
	પાછા FALSE;

    fname = is_full ? vim_strsave(sfname) : FullName_save(sfname, FALSE);
    જો (fname == ખાલી)
	પાછા FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    જો (sfname != ખાલી)
	forward_slash(sfname);
    forward_slash(fname);
#endif

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	જો (ap->pat != ખાલી && ap->cmds != ખાલી
	      && (ap->buflocal_nr == 0
		? match_file_pat(ખાલી, &ap->reg_prog,
					  fname, sfname, tail, ap->allow_dirs)
		: buf != ખાલી && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    તોડો;
	}

    vim_free(fname);
#ifdef BACKSLASH_IN_FILENAME
    vim_free(sfname);
#endif

    પાછા retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    પૂર્ણાંક
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    પાછા has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    પૂર્ણાંક
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    પાછા has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx == augroups.ga_len)		// add "END" add the end
	પાછા (char_u *)"END";
    જો (idx < 0 || idx >= augroups.ga_len)	// end of list
	પાછા ખાલી;
    જો (AUGROUP_NAME(idx) == ખાલી || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	પાછા (char_u *)"";
    પાછા AUGROUP_NAME(idx);		// return a name
}

સ્થિર પૂર્ણાંક include_groups = FALSE;

    char_u  *
set_context_in_autocmd(
    expand_T	*xp,
    char_u	*arg,
    પૂર્ણાંક		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    પૂર્ણાંક		group;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    group = au_get_grouparg(&arg);
    જો (group == AUGROUP_ERROR)
	પાછા ખાલી;
    // If there only is a group name that's what we expand.
    જો (*arg == NUL && group != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	group = AUGROUP_ALL;
    }

    // skip over event name
    માટે (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	જો (*p == ',')
	    arg = p + 1;
    જો (*p == NUL)
    {
	જો (group == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_pattern = arg;
	પાછા ખાલી;
    }

    // skip over pattern
    arg = skipwhite(p);
    જ્યારે (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    જો (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	માટે (પૂર્ણાંક i = 0; i < 2; i++)
	{
	    જો (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    જો (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    જો (!in_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	પાછા arg;			    // expand (next) command
    }

    જો (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    નહીંતર
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    પાછા ખાલી;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    પૂર્ણાંક i;

    જો (idx < augroups.ga_len)		// First list group names, if wanted
    {
	જો (!include_groups || AUGROUP_NAME(idx) == ખાલી
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    પાછા (char_u *)"";	// skip deleted entries
	પાછા AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    જો (i < 0 || i >= અંક_EVENTS)
	પાછા ખાલી;

    પાછા event_tab[i].કિંમત.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_group(expand_T *xp UNUSED, પૂર્ણાંક idx, પૂર્ણાંક win)
{
    જો (idx < 0 || idx >= અંક_EVENTS)
	પાછા ખાલી;

    જો (!win)
	પાછા event_tab[idx].કિંમત.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    પૂર્ણાંક j = 0;
    માટે (પૂર્ણાંક i = 0; i < અંક_EVENTS; ++i)
    {
	j += event_tab[i].ચાવી <= 0;
	જો (j == idx + 1)
	    પાછા event_tab[i].કિંમત.string;
    }

    પાછા ખાલી;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    પૂર્ણાંક
has_tabclosedpre(શૂન્ય)
{
    પાછા (first_autopat[(પૂર્ણાંક)EVENT_TABCLOSEDPRE] != ખાલી);
}

#જો defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    પૂર્ણાંક
autocmd_supported(char_u *name)
{
    char_u *p;

    પાછા (event_name2nr(name, &p) != અંક_EVENTS);
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
    પૂર્ણાંક
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*pattern = ખાલી;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = ખાલી;
    પૂર્ણાંક		group;
    પૂર્ણાંક		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    જો (arg_save == ખાલી)
	પાછા FALSE;
    p = vim_strchr(arg_save, '#');
    જો (p != ખાલી)
	*p++ = NUL;

    // First, look for an autocmd group name
    group = au_find_group(arg_save);
    જો (group == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	group = AUGROUP_ALL;
	event_name = arg_save;
    }
    નહીંતર
    {
	જો (p == ખાલી)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    જાઓ theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	જો (p != ખાલી)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    pattern = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    જો (event == અંક_EVENTS)
	જાઓ theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = first_autopat[(પૂર્ણાંક)event];
    જો (ap == ખાલી)
	જાઓ theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    જો (pattern != ખાલી && STRICMP(pattern, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    માટે ( ; ap != ખાલી; ap = ap->next)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	જો (ap->pat != ખાલી && ap->cmds != ખાલી
	    && (group == AUGROUP_ALL || ap->group == group)
	    && (pattern == ખાલી
		|| (buflocal_buf == ખાલી
		    ? fnamecmp(ap->pat, pattern) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    તોડો;
	}

theend:
    vim_free(arg_save);
    પાછા retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    સ્થિર શૂન્ય
autocmd_add_or_delete(typval_T *argvars, typval_T *rettv, પૂર્ણાંક delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = ખાલી;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*group_name = ખાલી;
    પૂર્ણાંક		group;
    char_u	*pat = ખાલી;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = ખાલી;
    char_u	*end;
    પૂર્ણાંક		once;
    પૂર્ણાંક		nested;
    પૂર્ણાંક		replace;		// replace the cmd for a group/event
    પૂર્ણાંક		retval = VVAL_TRUE;
    પૂર્ણાંક		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_સંખ્યા = VVAL_FALSE;

    જો (check_restricted() || check_secure())
	પાછા;

    જો (check_for_list_arg(argvars, 0) == FAIL)
	પાછા;

    aucmd_list = argvars[0].vval.v_list;
    જો (aucmd_list == ખાલી)
	પાછા;

    FOR_ALL_LIST_વસ્તુઓ(aucmd_list, li)
    {
	VIM_CLEAR(group_name);
	VIM_CLEAR(cmd);
	event_name = ખાલી;
	event_list = ખાલી;
	pat = ખાલી;
	pat_list = ખાલી;

	જો (li->li_tv.v_type != VAR_DICT)
	    ચાલુ;

	event_dict = li->li_tv.vval.v_dict;
	જો (event_dict == ખાલી)
	    ચાલુ;

	di = dict_find(event_dict, (char_u *)"event", -1);
	જો (di != ખાલી)
	{
	    જો (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		જો (event_name == ખાલી)
		{
		    emsg(_(e_string_required));
		    ચાલુ;
		}
	    }
	    નહીંતર જો (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		જો (event_list == ખાલી)
		{
		    emsg(_(e_list_required));
		    ચાલુ;
		}
	    }
	    નહીંતર
	    {
		emsg(_(e_string_or_list_expected));
		ચાલુ;
	    }
	}

	group_name = dict_get_string(event_dict, "group", TRUE);
	જો (group_name == ખાલી || *group_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    group = current_augroup;
	નહીંતર
	{
	    group = au_find_group(group_name);
	    જો (group == AUGROUP_ERROR)
	    {
		જો (delete)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    તોડો;
		}
		// group is not found, create it now
		group = au_new_group(group_name);
		જો (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    તોડો;
		}

		current_augroup = group;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	જો (dict_has_ચાવી(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_સંખ્યા_def(event_dict, "bufnr", -1);
	    જો (bnum == -1)
		ચાલુ;

	    vim_snprintf((અક્ષર *)IObuff, IOSIZE, "<buffer=%d>", (પૂર્ણાંક)bnum);
	    pat = IObuff;
	}
	નહીંતર
	{
	    di = dict_find(event_dict, (char_u *)"pattern", -1);
	    જો (di != ખાલી)
	    {
		જો (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    જો (pat == ખાલી)
		    {
			emsg(_(e_string_required));
			ચાલુ;
		    }
		}
		નહીંતર જો (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    જો (pat_list == ખાલી)
		    {
			emsg(_(e_list_required));
			ચાલુ;
		    }
		}
		નહીંતર
		{
		    emsg(_(e_string_or_list_expected));
		    ચાલુ;
		}
	    }
	    નહીંતર જો (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	જો (cmd == ખાલી)
	{
	    જો (delete)
		cmd = vim_strsave((char_u *)"");
	    નહીંતર
		ચાલુ;
	}

	જો (delete && (event_name == ખાલી
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    માટે (event = (event_T)0; (પૂર્ણાંક)event < અંક_EVENTS;
		    event = (event_T)((પૂર્ણાંક)event + 1))
	    {
		જો (do_autocmd_event(event, pat, once, nested, cmd, delete,
							group, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    તોડો;
		}
	    }
	}
	નહીંતર
	{
	    char_u *p = ખાલી;

	    eli = ખાલી;
	    end = ખાલી;
	    જ્યારે (TRUE)
	    {
		જો (event_list != ખાલી)
		{
		    જો (eli == ખાલી)
			eli = event_list->lv_first;
		    નહીંતર
			eli = eli->li_next;
		    જો (eli == ખાલી)
			તોડો;
		    જો (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == ખાલી)
		    {
			emsg(_(e_string_required));
			તોડો;
		    }
		}
		નહીંતર
		{
		    જો (p == ખાલી)
			p = event_name;
		    જો (p == ખાલી || *p == NUL)
			તોડો;
		}

		event = event_name2nr(p, &end);
		જો (event == અંક_EVENTS || *end != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    તોડો;
		}
		જો (pat != ખાલી)
		{
		    જો (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, group, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			તોડો;
		    }
		}
		નહીંતર જો (pat_list != ખાલી)
		{
		    FOR_ALL_LIST_વસ્તુઓ(pat_list, pli)
		    {
			જો (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == ખાલી)
			{
			    emsg(_(e_string_required));
			    ચાલુ;
			}
			જો (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, group, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    તોડો;
			}
		    }
		    જો (retval == VVAL_FALSE)
			તોડો;
		}
		જો (event_name != ખાલી)
		    p = end;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	જો (delete && group_name != ખાલી &&
		(event_name == ખાલી || event_name[0] == NUL)
		&& (pat == ખાલી || pat[0] == NUL)
		&& (cmd == ખાલી || cmd[0] == NUL))
	    au_del_group(group_name);
    }

    VIM_CLEAR(group_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_સંખ્યા = retval;
}

/*
 * autocmd_add() function
 */
    શૂન્ય
f_autocmd_add(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    શૂન્ય
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    શૂન્ય
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = અંક_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = ખાલી;
    char_u	*name = ખાલી;
    પૂર્ણાંક		group = AUGROUP_ALL;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;
    જો (check_for_opt_dict_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	જો (dict_has_ચાવી(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    જો (name == ખાલી)
		પાછા;

	    જો (*name == NUL)
		group = AUGROUP_DEFAULT;
	    નહીંતર
	    {
		group = au_find_group(name);
		જો (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), name);
		    vim_free(name);
		    પાછા;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	જો (dict_has_ચાવી(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    જો (name == ખાલી)
		પાછા;

	    જો (name[0] == '*' && name[1] == NUL)
		event_arg = અંક_EVENTS;
	    નહીંતર
	    {
		keyvalue_T target;
		keyvalue_T *entry;

		target.ચાવી = 0;
		target.કિંમત.string = name;
		target.કિંમત.length = STRLEN(target.કિંમત.string);
		entry = (keyvalue_T *)bsearch(&target, &event_tab,
		    અંક_EVENTS, કદ(event_tab[0]), cmp_keyvalue_કિંમત_ni);
		જો (entry == ખાલી)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    પાછા;
		}
		event_arg = (event_T)abs(entry->ચાવી);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	જો (dict_has_ચાવી(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    જો (pat == ખાલી)
		પાછા;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    માટે (event = (event_T)0; (પૂર્ણાંક)event < અંક_EVENTS;
	    event = (event_T)((પૂર્ણાંક)event + 1))
    {
	string_T    *event_name;

	જો (event_arg != અંક_EVENTS && event != event_arg)
	    ચાલુ;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	group_name;

	    જો (ap->pat == ખાલી)		// pattern has been removed
		ચાલુ;

	    જો (group != AUGROUP_ALL && group != ap->group)
		ચાલુ;

	    જો (pat != ખાલી && STRCMP(pat, ap->pat) != 0)
		ચાલુ;

	    group_name.string = get_augroup_name(ખાલી, ap->group);
	    જો (group_name.string == ખાલી)
		STR_LITERAL_SET(group_name, "");
	    નહીંતર
		group_name.length = STRLEN(group_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    માટે (ac = ap->cmds; ac != ખાલી; ac = ac->next)
	    {
		event_dict = dict_alloc();
		જો (event_dict == ખાલી
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    પાછા;
		}

		જો (dict_add_string_len(event_dict, "event",
			event_name->string, (પૂર્ણાંક)event_name->length) == FAIL
			|| dict_add_string_len(event_dict, "group",
			    group_name.string, (પૂર્ણાંક)group_name.length) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_add_સંખ્યા(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_add_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_add_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_add_bool(event_dict, "once", ac->once) == FAIL
			|| dict_add_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    પાછા;
		}
	    }
	}
    }

    vim_free(pat);
}

#endif
