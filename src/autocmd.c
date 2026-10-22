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

#अन्तर्भाव "vim.h"

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
प्रकारनिर्देश संरचना AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    वर्ण	    once;		// "One shot": removed after execution
    वर्ण	    nested;		// If autocommands nest here.
    वर्ण	    last;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    संरचना AutoCmd  *next;		// next AutoCmd in list
} AutoCmd;

प्रकारनिर्देश संरचना AutoPat
{
    संरचना AutoPat  *next;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    पूर्णाङ्क		    group;		// group ID
    पूर्णाङ्क		    patlen;		// strlen() of pat
    पूर्णाङ्क		    buflocal_nr;	// !=0 for buffer-local AutoPat
    वर्ण	    allow_dirs;		// Pattern may match whole path
    वर्ण	    last;		// last pattern for apply_autocmds()
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
स्थिर keyvalue_T event_tab[अङ्कः_EVENTS] = {
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

स्थिर AutoPat *first_autopat[अङ्कः_EVENTS] = { रिक्त };
स्थिर AutoPat *last_autopat[अङ्कः_EVENTS] = { रिक्त };

#define AUGROUP_DEFAULT    (-1)	    // default autocmd group
#define AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#define AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
संरचना AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    पूर्णाङ्क		group;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*tail;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    पूर्णाङ्क		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *next;		// chain of active apc-s for auto-invalidation
};

स्थिर AutoPatCmd_T *active_apc_list = रिक्त; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#define FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    अत्थाय ((ap) = first_autopat[(पूर्णाङ्क)(event)]; (ap) != रिक्त; (ap) = (ap)->next)

/*
 * augroups stores a list of autocmd group names.
 */
स्थिर garray_T augroups = {0, 0, परिमाण(char_u *), 10, रिक्त};
#define AUGROUP_NAME(i) (((char_u **)augroups.ga_data)[i])
// use get_deleted_augroup() to get this
स्थिर char_u *deleted_augroup = रिक्त;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
स्थिर पूर्णाङ्क current_augroup = AUGROUP_DEFAULT;

स्थिर पूर्णाङ्क au_need_clean = FALSE;   // need to delete marked patterns

स्थिर event_T event_name2nr(char_u *start, char_u **end);
स्थिर string_T *event_nr2name(event_T event);
स्थिर पूर्णाङ्क au_get_grouparg(char_u **argp);
स्थिर पूर्णाङ्क do_autocmd_event(event_T event, char_u *pat, पूर्णाङ्क once, पूर्णाङ्क nested, char_u *cmd, पूर्णाङ्क forceit, पूर्णाङ्क group, पूर्णाङ्क flags);
स्थिर पूर्णाङ्क apply_autocmds_group(event_T event, char_u *fname, char_u *fname_io, पूर्णाङ्क force, पूर्णाङ्क group, buf_T *buf, exarg_T *eap);
स्थिर शून्य auto_next_pat(AutoPatCmd_T *apc, पूर्णाङ्क stop_at_last);
स्थिर पूर्णाङ्क au_find_group(char_u *name);

स्थिर event_T	last_event;
स्थिर पूर्णाङ्क	last_group;
स्थिर पूर्णाङ्क	autocmd_blocked = 0;	// block all autocmds

    स्थिर char_u *
get_deleted_augroup(शून्य)
{
    सचे (deleted_augroup == रिक्त)
	deleted_augroup = (char_u *)_("--Deleted--");
    पटिनिवत्त deleted_augroup;
}

/*
 * Show the autocommands for one AutoPat.
 */
    स्थिर शून्य
show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    // Check for "got_int" (here and at various places below), which is set
    // when "q" has been hit for the "--more--" prompt
    सचे (got_int)
	पटिनिवत्त;
    सचे (ap->pat == रिक्त)		// pattern has been removed
	पटिनिवत्त;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    सन्देशः_putchar('\n');
    सचे (got_int)
	गच्छ theend;
    सचे (event != last_event || ap->group != last_group)
    {
	string_T    *event_name;

	सचे (ap->group != AUGROUP_DEFAULT)
	{
	    सचे (AUGROUP_NAME(ap->group) == रिक्त)
		सन्देशः_puts_attr((वर्ण *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    अञ्ञथा
		सन्देशः_puts_attr((वर्ण *)AUGROUP_NAME(ap->group), HL_ATTR(HLF_T));
	    सन्देशः_puts("  ");
	}
	event_name = event_nr2name(event);
	सन्देशः_puts_attr((वर्ण *)event_name->string, HL_ATTR(HLF_T));
	last_event = event;
	last_group = ap->group;
	सन्देशः_putchar('\n');
	सचे (got_int)
	    गच्छ theend;
    }

    सचे (ap->pat == रिक्त)
	गच्छ theend;  // timer might have cleared the pattern or group

    सन्देशः_col = 4;
    सन्देशः_outtrans(ap->pat);

    अत्थाय (ac = ap->cmds; ac != रिक्त; ac = ac->next)
    {
	सचे (ac->cmd == रिक्त)		// skip removed commands
	    अनुवर्त;

	सचे (सन्देशः_col >= 14)
	    सन्देशः_putchar('\n');
	सन्देशः_col = 14;
	सचे (got_int)
	    गच्छ theend;
	सन्देशः_outtrans(ac->cmd);
#ifdef FEAT_EVAL
	सचे (p_verbose > 0)
	    last_set_सन्देशः(ac->script_ctx);
#endif
	सचे (got_int)
	    गच्छ theend;
	सचे (ac->next != रिक्त)
	{
	    सन्देशः_putchar('\n');
	    सचे (got_int)
		गच्छ theend;
	}
    }

theend:
    --autocmd_busy;
}

/*
 * Mark an autocommand pattern for deletion.
 */
    स्थिर शून्य
au_remove_pat(AutoPat *ap)
{
    VIM_CLEAR(ap->pat);
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    स्थिर शून्य
au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    अत्थाय (ac = ap->cmds; ac != रिक्त; ac = ac->next)
	VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

// Delete one command from an autocmd pattern.
स्थिर शून्य au_del_cmd(AutoCmd *ac)
{
    VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    स्थिर शून्य
au_cleanup(शून्य)
{
    AutoPat	*ap, **prev_ap;
    AutoCmd	*ac, **prev_ac;
    event_T	event;

    सचे (autocmd_busy || !au_need_clean)
	पटिनिवत्त;

    // loop over all events
    अत्थाय (event = (event_T)0; (पूर्णाङ्क)event < अङ्कः_EVENTS;
					    event = (event_T)((पूर्णाङ्क)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(first_autopat[(पूर्णाङ्क)event]);
	अत्थाय (ap = *prev_ap; ap != रिक्त; ap = *prev_ap)
	{
	    पूर्णाङ्क has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    अत्थाय (ac = *prev_ac; ac != रिक्त; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		सचे (ap->pat == रिक्त || ac->cmd == रिक्त)
		{
		    *prev_ac = ac->next;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		अञ्ञथा
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->next);
		}
	    }

	    सचे (ap->pat != रिक्त && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    सचे (ap->pat == रिक्त)
	    {
		सचे (ap->next == रिक्त)
		{
		    सचे (prev_ap == &(first_autopat[(पूर्णाङ्क)event]))
			last_autopat[(पूर्णाङ्क)event] = रिक्त;
		    अञ्ञथा
			// this depends on the "next" field being the first in
			// the struct
			last_autopat[(पूर्णाङ्क)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->next;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    अञ्ञथा
		prev_ap = &(ap->next);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Called when buffer is freed, to remove/invalidate related buffer-local
 * autocmds.
 */
    शून्य
aubuflocal_remove(buf_T *buf)
{
    AutoPat	    *ap;
    event_T	    event;
    AutoPatCmd_T    *apc;

    // invalidate currently executing autocommands
    अत्थाय (apc = active_apc_list; apc; apc = apc->next)
	सचे (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    अत्थाय (event = (event_T)0; (पूर्णाङ्क)event < अङ्कः_EVENTS;
					    event = (event_T)((पूर्णाङ्क)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    सचे (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		सचे (p_verbose >= 6)
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
    स्थिर पूर्णाङ्क
au_new_group(char_u *name)
{
    पूर्णाङ्क		i;

    i = au_find_group(name);
    सचे (i != AUGROUP_ERROR)
	पटिनिवत्त i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    अत्थाय (i = 0; i < augroups.ga_len; ++i)
	सचे (AUGROUP_NAME(i) == रिक्त)
	    भङ्ग;
    सचे (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	पटिनिवत्त AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    सचे (AUGROUP_NAME(i) == रिक्त)
	पटिनिवत्त AUGROUP_ERROR;
    सचे (i == augroups.ga_len)
	++augroups.ga_len;

    पटिनिवत्त i;
}

    स्थिर शून्य
au_del_group(char_u *name)
{
    पूर्णाङ्क		i;
    event_T	event;
    AutoPat	*ap;
    पूर्णाङ्क		in_use = FALSE;


    i = au_find_group(name);
    सचे (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_group_str), name);
	पटिनिवत्त;
    }
    सचे (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_group));
	पटिनिवत्त;
    }

    अत्थाय (event = (event_T)0; (पूर्णाङ्क)event < अङ्कः_EVENTS;
	    event = (event_T)((पूर्णाङ्क)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    सचे (ap->group == i && ap->pat != रिक्त)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		in_use = TRUE;
		event = अङ्कः_EVENTS;
		भङ्ग;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    सचे (in_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    अञ्ञथा
	AUGROUP_NAME(i) = रिक्त;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    स्थिर पूर्णाङ्क
au_find_group(char_u *name)
{
    पूर्णाङ्क	    i;

    अत्थाय (i = 0; i < augroups.ga_len; ++i)
	सचे (AUGROUP_NAME(i) != रिक्त && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    पटिनिवत्त i;
    पटिनिवत्त AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    पूर्णाङ्क
au_has_group(char_u *name)
{
    पटिनिवत्त au_find_group(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    शून्य
do_augroup(char_u *arg, पूर्णाङ्क del_group)
{
    पूर्णाङ्क	    i;

    सचे (del_group)
    {
	सचे (*arg == NUL)
	    emsg(_(e_argument_required));
	अञ्ञथा
	    au_del_group(arg);
    }
    अञ्ञथा सचे (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    अञ्ञथा सचे (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_group(arg);
	सचे (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    अञ्ञथा			    // ":aug": list the group names
    {
	सन्देशः_start();
	अत्थाय (i = 0; i < augroups.ga_len; ++i)
	{
	    सचे (AUGROUP_NAME(i) != रिक्त)
	    {
		सन्देशः_puts((वर्ण *)AUGROUP_NAME(i));
		सन्देशः_puts("  ");
	    }
	}
	सन्देशः_clr_eos();
	सन्देशः_end();
    }
}

    शून्य
autocmd_init(शून्य)
{
    CLEAR_FIELD(aucmd_win);
}

#सचे defined(EXITFREE)
    शून्य
free_all_autocmds(शून्य)
{
    char_u	*s;

    अत्थाय (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(रिक्त, (char_u *)"", TRUE);

    अत्थाय (पूर्णाङ्क i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_data))[i];
	सचे (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#endif

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    पूर्णाङ्क
is_aucmd_win(win_T *win)
{
    अत्थाय (पूर्णाङ्क i = 0; i < AUCMD_WIN_गणनम्; ++i)
	सचे (aucmd_win[i].auc_win_used && aucmd_win[i].auc_win == win)
	    पटिनिवत्त TRUE;
    पटिनिवत्त FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    स्थिर event_T
event_name2nr(char_u *start, char_u **end)
{
    char_u	*p;
    keyvalue_T target;
    keyvalue_T *entry;
    स्थिर keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_INDEX];
    स्थिर keyvalue_T *bufread = &event_tab[BUFREAD_INDEX];

    // the event name ends with end of line, '|', a blank or a comma
    अत्थाय (p = start; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    target.कुञ्चिका = 0;
    target.मानम्.string = start;
    target.मानम्.length = (size_t)(p - start);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    सचे (cmp_keyvalue_मानम्_ni(&target, bufnewfile) == 0)
	entry = bufnewfile;
    अञ्ञथा सचे (cmp_keyvalue_मानम्_ni(&target, bufread) == 0)
	entry = bufread;
    अञ्ञथा
	entry = (keyvalue_T *)bsearch(&target, &event_tab, अङ्कः_EVENTS,
				परिमाण(event_tab[0]), cmp_keyvalue_मानम्_ni);

    सचे (*p == ',')
	++p;
    *end = p;

    पटिनिवत्त (entry == रिक्त) ? अङ्कः_EVENTS : (event_T)abs(entry->कुञ्चिका);
}

/*
 * Return the name for event "event".
 */
    स्थिर string_T *
event_nr2name(event_T event)
{
    पूर्णाङ्क	    i;
    गणना {CACHE_SIZE = 12};
    स्थिर पूर्णाङ्क cache_tab[CACHE_SIZE];
    स्थिर पूर्णाङ्क cache_last_index = -1;
    स्थिर string_T unknown = STR_LITERAL_INIT("Unknown");

    सचे (cache_last_index < 0)
    {
	अत्थाय (i = 0; i < CACHE_SIZE; ++i)
	    cache_tab[i] = -1;
	cache_last_index = CACHE_SIZE - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    अत्थाय (i = cache_last_index; cache_tab[i] >= 0; )
    {
	सचे ((event_T)abs(event_tab[cache_tab[i]].कुञ्चिका) == event)
	    पटिनिवत्त &event_tab[cache_tab[i]].मानम्;

	सचे (i == 0)
	    i = CACHE_SIZE - 1;
	अञ्ञथा
	    --i;

	// are we back at the start?
	सचे (i == cache_last_index)
	    भङ्ग;
    }

    // look in the event table itself
    अत्थाय (i = 0; i < अङ्कः_EVENTS; ++i)
    {
	सचे ((event_T)abs(event_tab[i].कुञ्चिका) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    सचे (cache_last_index == CACHE_SIZE - 1)
		cache_last_index = 0;
	    अञ्ञथा
		++cache_last_index;
	    cache_tab[cache_last_index] = i;
	    पटिनिवत्त &event_tab[i].मानम्;
	}
    }

    पटिनिवत्त &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    स्थिर char_u *
find_end_event(
    char_u  *arg,
    पूर्णाङ्क	    have_group)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    सचे (*arg == '*')
    {
	सचे (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_star_str), arg);
	    पटिनिवत्त रिक्त;
	}
	pat = arg + 1;
    }
    अञ्ञथा
    {
	अत्थाय (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    सचे ((पूर्णाङ्क)event_name2nr(pat, &p) >= अङ्कः_EVENTS)
	    {
		सचे (have_group)
		    semsg(_(e_no_such_event_str), pat);
		अञ्ञथा
		    semsg(_(e_no_such_group_or_event_str), pat);
		पटिनिवत्त रिक्त;
	    }
	}
    }
    पटिनिवत्त pat;
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
    पूर्णाङ्क		group;

    सचे (*arg == '|')
	पटिनिवत्त रिक्त;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    group = au_get_grouparg(&arg);
    pat = group == AUGROUP_ERROR
			  ? रिक्त : find_end_event(arg, group != AUGROUP_ALL);
    --emsg_off;
    सचे (pat == रिक्त)
	पटिनिवत्त रिक्त;

    pat = skipwhite(pat);
    सचे (*pat == NUL || *pat == '|')
	पटिनिवत्त रिक्त;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    याव (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    अत्थाय (पूर्णाङ्क i = 0; i < 2; ++i)
    {
	सचे (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	सचे (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	सचे (!in_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    पटिनिवत्त *cmd == NUL ? रिक्त : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    पूर्णाङ्क
event_ignored(event_T event, char_u *ei)
{
    पूर्णाङ्क ignored = FALSE;
    याव (*ei != NUL)
    {
	पूर्णाङ्क unignore = *ei == '-';
	ei += unignore;
	सचे (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].कुञ्चिका <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	अञ्ञथा सचे (event_name2nr(ei, &ei) == event)
	{
	    सचे (unignore)
		पटिनिवत्त FALSE;
	    अञ्ञथा
		ignored = TRUE;
	}
    }

    पटिनिवत्त ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    पूर्णाङ्क
check_ei(char_u *ei)
{
    पूर्णाङ्क	win = ei != p_ei;

    याव (*ei)
    {
	सचे (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	अञ्ञथा
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    सचे (event == अङ्कः_EVENTS || (win && event_tab[event].कुञ्चिका > 0))
		पटिनिवत्त FAIL;
	}
    }

    पटिनिवत्त OK;
}

#सचे defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(वर्ण *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    size_t	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    सचे (save_ei == रिक्त)
	पटिनिवत्त रिक्त;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    सचे (new_ei == रिक्त)
    {
	vim_free(save_ei);
	पटिनिवत्त रिक्त;
    }

    सचे (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    अञ्ञथा
	STRCPY(new_ei + p_ei_len, what);
    set_string_option_direct((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    पटिनिवत्त save_ei;
}

    शून्य
au_event_restore(char_u *old_ei)
{
    सचे (old_ei != रिक्त)
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
    शून्य
do_autocmd(exarg_T *eap, char_u *arg_in, पूर्णाङ्क forceit)
{
    char_u	*arg = arg_in;
    char_u	*pat;
    char_u	*envpat = रिक्त;
    char_u	*cmd;
    पूर्णाङ्क		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = रिक्त;
    पूर्णाङ्क		nested = FALSE;
    पूर्णाङ्क		once = FALSE;
    पूर्णाङ्क		group;
    पूर्णाङ्क		i;
    पूर्णाङ्क		flags = 0;

    सचे (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	group = AUGROUP_ALL;	// no argument, use all groups
    }
    अञ्ञथा
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	group = au_get_grouparg(&arg);
	सचे (arg == रिक्त)	    // out of memory
	    पटिनिवत्त;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = find_end_event(arg, group != AUGROUP_ALL);
    सचे (pat == रिक्त)
	पटिनिवत्त;

    pat = skipwhite(pat);
    सचे (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    अञ्ञथा
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	याव (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	सचे (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	सचे (vim_strchr(pat, '$') != रिक्त || vim_strchr(pat, '~') != रिक्त)
	{
#ifdef BACKSLASH_IN_FILENAME
	    पूर्णाङ्क	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#endif
	    envpat = expand_env_save(pat);
#ifdef BACKSLASH_IN_FILENAME
	    p_ssl = p_ssl_save;
#endif
	    सचे (envpat != रिक्त)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	अत्थाय (i = 0; i < 2; i++)
	{
	    सचे (*cmd == NUL)
		अनुवर्त;

	    // Check for "++once" flag.
	    सचे (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		सचे (once)
		{
		    semsg(_(e_duplicate_argument_str), "++once");
		    गच्छ err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    सचे ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		सचे (nested)
		{
		    semsg(_(e_duplicate_argument_str), "++nested");
		    गच्छ err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    सचे (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		सचे (in_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_mean_plusplus_nested));
		    गच्छ err_exit;
		}
		सचे (nested)
		{
		    semsg(_(e_duplicate_argument_str), "nested");
		    गच्छ err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 6);
	    }
	}

	/*
	 * Find the start of the commands.
	 * Expand <sfile> in it.
	 */
	सचे (*cmd != NUL)
	{
	    सचे (eap != रिक्त)
		// Read a {} block if it follows.
		cmd = may_get_cmd_block(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    सचे (cmd == रिक्त)	    // some error
		गच्छ err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    सचे (!forceit && *cmd == NUL)
	// Highlight title
	सन्देशः_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    last_event = (event_T)-1;		// for listing the event name
    last_group = AUGROUP_ERROR;		// for listing the group name
    सचे (*arg == '*' || *arg == NUL || *arg == '|')
    {
	सचे (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	अञ्ञथा
	    अत्थाय (event = (event_T)0; (पूर्णाङ्क)event < अङ्कः_EVENTS;
					     event = (event_T)((पूर्णाङ्क)event + 1))
		सचे (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, group, flags) == FAIL)
		    भङ्ग;
    }
    अञ्ञथा
    {
	याव (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    सचे (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, group, flags) == FAIL)
		भङ्ग;
    }

err_exit:
    सचे (cmd_need_free)
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
    स्थिर पूर्णाङ्क
au_get_grouparg(char_u **argp)
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    पूर्णाङ्क		group = AUGROUP_ALL;

    अत्थाय (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    सचे (p <= arg)
	पटिनिवत्त AUGROUP_ALL;

    group_name = vim_strnsave(arg, p - arg);
    सचे (group_name == रिक्त)		// out of memory
	पटिनिवत्त AUGROUP_ERROR;
    group = au_find_group(group_name);
    सचे (group == AUGROUP_ERROR)
	group = AUGROUP_ALL;	// no match, use all groups
    अञ्ञथा
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(group_name);
    पटिनिवत्त group;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    स्थिर पूर्णाङ्क
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    पूर्णाङ्क		once,
    पूर्णाङ्क		nested,
    char_u	*cmd,
    पूर्णाङ्क		forceit,
    पूर्णाङ्क		group,
    पूर्णाङ्क		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    पूर्णाङ्क		brace_level;
    char_u	*endpat;
    पूर्णाङ्क		findgroup;
    पूर्णाङ्क		allgroups;
    पूर्णाङ्क		patlen;
    पूर्णाङ्क		is_buflocal;
    पूर्णाङ्क		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    सचे (group == AUGROUP_ALL)
	findgroup = current_augroup;
    अञ्ञथा
	findgroup = group;
    allgroups = (group == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    सचे (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    सचे (forceit)  // delete the AutoPat, if it's in the current group
	    {
		सचे (ap->group == findgroup)
		    au_remove_pat(ap);
	    }
	    अञ्ञथा सचे (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    अत्थाय ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	अत्थाय (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    सचे (*endpat == '{')
		brace_level++;
	    अञ्ञथा सचे (*endpat == '}')
		brace_level--;
	}
	सचे (pat == endpat)		// ignore single comma
	    अनुवर्त;
	patlen = (पूर्णाङ्क)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	सचे (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    सचे (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    अञ्ञथा सचे (patlen > 9 && pat[7] == '=')
	    {
		सचे (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		अञ्ञथा सचे (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((वर्ण *)pat + 8);
	    }
	}

	सचे (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((वर्ण *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (पूर्णाङ्क)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	सचे (!forceit && *cmd != NUL && last_autopat[(पूर्णाङ्क)event] != रिक्त)
	    prev_ap = &last_autopat[(पूर्णाङ्क)event];
	अञ्ञथा
	    prev_ap = &first_autopat[(पूर्णाङ्क)event];
	याव ((ap = *prev_ap) != रिक्त)
	{
	    सचे (ap->pat != रिक्त)
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
		सचे ((allgroups || ap->group == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    सचे (forceit)
		    {
			सचे (*cmd != NUL && ap->next == रिक्त)
			{
			    au_remove_cmds(ap);
			    भङ्ग;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    अञ्ञथा सचे (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    अञ्ञथा सचे (ap->next == रिक्त)
			भङ्ग;
		}
	    }
	    prev_ap = &ap->next;
	}

	/*
	 * Add a new command.
	 */
	सचे (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    सचे (ap == रिक्त)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		सचे (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == रिक्त))
		{
		    semsg(_(e_buffer_nr_invalid_buffer_सङ्ख्या), buflocal_nr);
		    पटिनिवत्त FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		सचे (ap == रिक्त)
		    पटिनिवत्त FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		सचे (ap->pat == रिक्त)
		{
		    vim_free(ap);
		    पटिनिवत्त FAIL;
		}

#ifdef FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		सचे (event == EVENT_MODECHANGED && !has_modechanged())
		    get_mode(last_mode);
#endif
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		सचे ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
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

		सचे (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = रिक्त;
		}
		अञ्ञथा
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_to_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    सचे (reg_pat != रिक्त)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    सचे (reg_pat == रिक्त || ap->reg_prog == रिक्त)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			पटिनिवत्त FAIL;
		    }
		}
		ap->cmds = रिक्त;
		*prev_ap = ap;
		last_autopat[(पूर्णाङ्क)event] = ap;
		ap->next = रिक्त;
		सचे (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		अञ्ञथा
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    याव ((ac = *prev_ac) != रिक्त)
		prev_ac = &ac->next;
	    ac = ALLOC_ONE(AutoCmd);
	    सचे (ac == रिक्त)
		पटिनिवत्त FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    सचे (flags & UC_VIM9)
		ac->script_ctx.sc_version = SCRIPT_VERSION_VIM9;
#ifdef FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#endif
	    सचे (ac->cmd == रिक्त)
	    {
		vim_free(ac);
		पटिनिवत्त FAIL;
	    }
	    ac->next = रिक्त;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    पटिनिवत्त OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    पूर्णाङ्क
do_doautocmd(
    char_u	*arg_start,
    पूर्णाङ्क		do_सन्देशः,	    // give message for no matching autocmds?
    पूर्णाङ्क		*did_something)
{
    char_u	*arg = arg_start;
    char_u	*fname;
    पूर्णाङ्क		nothing_done = TRUE;
    पूर्णाङ्क		group;

    सचे (did_something != रिक्त)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    सचे (arg == रिक्त)	    // out of memory
	पटिनिवत्त FAIL;

    सचे (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	पटिनिवत्त FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg, group != AUGROUP_ALL);
    सचे (fname == रिक्त)
	पटिनिवत्त FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    याव (*arg && !ends_excmd(*arg) && !VIM_ISWHITE(*arg))
	सचे (apply_autocmds_group(event_name2nr(arg, &arg),
				      fname, रिक्त, TRUE, group, curbuf, रिक्त))
	    nothing_done = FALSE;

    सचे (nothing_done && do_सन्देशः
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       )
	smsg(_("No matching autocommands: %s"), arg_start);
    सचे (did_something != रिक्त)
	*did_something = !nothing_done;

#ifdef FEAT_EVAL
    पटिनिवत्त aborting() ? FAIL : OK;
#अञ्ञथा
    पटिनिवत्त OK;
#endif
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    शून्य
ex_doautoall(exarg_T *eap)
{
    पूर्णाङ्क		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    पूर्णाङ्क		call_do_modelines = check_nomodeline(&arg);
    पूर्णाङ्क		did_aucmd;

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
	सचे (buf->b_ml.ml_mfp == रिक्त || buf == curbuf)
	    अनुवर्त;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	सचे (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    भङ्ग;
	}

	set_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	सचे (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	सचे (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    भङ्ग;
	}
    }

    // Execute autocommands for the current buffer last.
    सचे (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	सचे (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    पूर्णाङ्क
check_nomodeline(char_u **argp)
{
    सचे (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	पटिनिवत्त FALSE;
    }
    पटिनिवत्त TRUE;
}

/*
 * Prepare for executing autocommands for (hidden) buffer "buf".
 * Search for a visible window containing the current buffer.  If there isn't
 * one then use an entry in "aucmd_win[]".
 * Set "curbuf" and "curwin" to match "buf".
 * When this fails "curbuf" is not equal "buf".
 */
    शून्य
aucmd_prepbuf(
    aco_save_T	*aco,		// structure to save values in
    buf_T	*buf)		// new curbuf
{
    win_T	*win;
    पूर्णाङ्क		save_ea;
#ifdef FEAT_AUTOCHDIR
    पूर्णाङ्क		save_acd;
#endif
    पूर्णाङ्क		same_buffer = buf == curbuf;

    // Find a window that is for the new buffer
    सचे (same_buffer)		// be quick when buf is curbuf
	win = curwin;
    अञ्ञथा
	FOR_ALL_WINDOWS(win)
	    सचे (win->w_buffer == buf)
		भङ्ग;

    // Allocate a window when needed.
    win_T *auc_win = रिक्त;
    पूर्णाङ्क auc_idx = AUCMD_WIN_गणनम्;
    सचे (win == रिक्त)
    {
	अत्थाय (auc_idx = 0; auc_idx < AUCMD_WIN_गणनम्; ++auc_idx)
	    सचे (!aucmd_win[auc_idx].auc_win_used)
	    {
		सचे (aucmd_win[auc_idx].auc_win == रिक्त)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		सचे (auc_win != रिक्त)
		    aucmd_win[auc_idx].auc_win_used = TRUE;
		भङ्ग;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	सचे (auc_win == रिक्त)
	    पटिनिवत्त;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == रिक्त ? 0 : prevwin->w_id;
#ifdef FEAT_JOB_CHANNEL
    सचे (bt_prompt(curbuf))
	aco->save_prompt_निवेशय = curbuf->b_prompt_निवेशय;
#endif

    सचे (win != रिक्त)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    अञ्ञथा
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
	curtab->tp_localdir = रिक्त;
	aco->globaldir = globaldir;
	globaldir = रिक्त;

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

	(शून्य)win_split_ins(0, WSP_शिखरम् | WSP_FORCE_ROOM, auc_win, 0, रिक्त);
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
    सचे (!same_buffer)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_active = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    शून्य
aucmd_restbuf(
    aco_save_T	*aco)		// structure holding saved values
{
    पूर्णाङ्क	    dummy;
    win_T   *save_curwin;

    सचे (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	block_autocmds();
	सचे (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		सचे (wp == awp)
		{
		    सचे (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(awp);
		    गच्छ win_found;
		}
	    }
	}
win_found:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(शून्य)winframe_remove(curwin, &dummy, रिक्त, रिक्त);
	win_remove(curwin, रिक्त);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_used = FALSE;
	last_status(FALSE);	    // may need to remove last status line

	सचे (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_find_by_id(aco->save_curwin_id);
	सचे (save_curwin != रिक्त)
	    curwin = save_curwin;
	अञ्ञथा
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_window(curwin);
	सचे (bt_prompt(curbuf))
	    curbuf->b_prompt_निवेशय = aco->save_prompt_निवेशय;
#endif
	prevwin = win_find_by_id(aco->save_prevwin_id);
#ifdef FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#endif
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	सचे (awp->w_localdir != रिक्त)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_active = aco->save_VIsual_active;
	check_cursor();
	सचे (curwin->w_topline > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    curwin->w_topline = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
#ifdef FEAT_DIFF
	    curwin->w_topfill = 0;
#endif
	}
#सचे defined(FEAT_GUI)
	सचे (gui.in_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_वामः], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_दक्षिणः], FALSE);
	    gui_may_update_scrollbars();
	}
#endif
    }
    अञ्ञथा
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_find_by_id(aco->save_curwin_id);
	सचे (save_curwin != रिक्त)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    सचे (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != रिक्त)
	    {
#सचे defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		सचे (curwin->w_s == &curbuf->b_s)
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
    सचे (VIsual_active)
	check_pos(curbuf, &VIsual);
}

स्थिर पूर्णाङ्क	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    पूर्णाङ्क
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    पूर्णाङ्क		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    पटिनिवत्त apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, रिक्त);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    पूर्णाङ्क
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    पूर्णाङ्क		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    पटिनिवत्त apply_autocmds_group(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    पूर्णाङ्क
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    पूर्णाङ्क		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    पूर्णाङ्क		*retval)    // pointer to caller's retval
{
    पूर्णाङ्क		did_cmd;

#ifdef FEAT_EVAL
    सचे (should_abort(*retval))
	पटिनिवत्त FALSE;
#endif

    did_cmd = apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, रिक्त);
    सचे (did_cmd
#ifdef FEAT_EVAL
	    && aborting()
#endif
	    )
	*retval = FAIL;
    पटिनिवत्त did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    स्थिर पूर्णाङ्क
has_cursorhold(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)(get_real_state() == MODE_NORMAL_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != रिक्त);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    पूर्णाङ्क
trigger_cursorhold(शून्य)
{
    पूर्णाङ्क		state;

    सचे (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_active())
    {
	state = get_real_state();
	सचे (state == MODE_NORMAL_BUSY || (state & MODE_निवेशय) != 0)
	    पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    पूर्णाङ्क
has_winresized(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_WINRESIZED] != रिक्त);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    पूर्णाङ्क
has_winscrolled(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_WINSCROLLED] != रिक्त);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    पूर्णाङ्क
has_cursormoved(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_CURSORMOVED] != रिक्त);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    पूर्णाङ्क
has_cursormovedI(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_CURSORMOVEDI] != रिक्त);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    पूर्णाङ्क
has_textchanged(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_TEXTCHANGED] != रिक्त);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    पूर्णाङ्क
has_textchangedI(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_TEXTCHANGEDI] != रिक्त);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    पूर्णाङ्क
has_textchangedP(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_TEXTCHANGEDP] != रिक्त);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    पूर्णाङ्क
has_insertcharpre(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_INSERTCHARPRE] != रिक्त);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    पूर्णाङ्क
has_keyinputpre(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_KEYINPUTPRE] != रिक्त);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    पूर्णाङ्क
has_cmdundefined(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_CMDUNDEFINED] != रिक्त);
}

#सचे defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    पूर्णाङ्क
has_textputpost(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_TEXTPUTPOST] != रिक्त);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    पूर्णाङ्क
has_textputpre(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_TEXTPUTPRE] != रिक्त);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    पूर्णाङ्क
has_textyankpost(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_TEXTYANKPOST] != रिक्त);
}
#endif

#सचे defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    पूर्णाङ्क
has_completechanged(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_COMPLETECHANGED] != रिक्त);
}
#endif

#सचे defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    पूर्णाङ्क
has_modechanged(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_MODECHANGED] != रिक्त);
}
#endif

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    स्थिर पूर्णाङ्क
apply_autocmds_group(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    पूर्णाङ्क		force,	     // when TRUE, ignore autocmd_busy
    पूर्णाङ्क		group,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap UNUSED) // command arguments
{
    char_u	*sfname = रिक्त;	// short file name
    char_u	*tail;
    पूर्णाङ्क		save_changed;
    buf_T	*old_curbuf;
    पूर्णाङ्क		retval = FALSE;
    char_u	*save_autocmd_fname;
    पूर्णाङ्क		save_autocmd_fname_full;
    पूर्णाङ्क		save_autocmd_bufnr;
    char_u	*save_autocmd_match;
    पूर्णाङ्क		save_autocmd_busy;
    पूर्णाङ्क		save_autocmd_nested;
    स्थिर पूर्णाङ्क	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ifdef FEAT_EVAL
    funccal_entry_T funccal_entry;
    char_u	*save_cmdarg;
    long	save_cmdbang;
#endif
    स्थिर पूर्णाङ्क	filechangeshell_busy = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif
    पूर्णाङ्क		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    पूर्णाङ्क		save_कुञ्चिका_Typed = कुञ्चिका_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    सचे (event == अङ्कः_EVENTS || first_autopat[(पूर्णाङ्क)event] == रिक्त
	    || autocmd_blocked > 0)
	गच्छ BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    सचे (autocmd_busy && !(force || autocmd_nested))
	गच्छ BYPASS_AU;

#ifdef FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    सचे (aborting())
	गच्छ BYPASS_AU;
#endif

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    सचे (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	गच्छ BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    सचे (event_ignored(event, p_ei))
	गच्छ BYPASS_AU;

    पूर्णाङ्क win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    सचे (buf == curbuf && event_tab[event].कुञ्चिका <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    अञ्ञथा सचे (buf != रिक्त && event_tab[event].कुञ्चिका <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    सचे (wp->w_buffer == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		भङ्ग;
	    }
    }
    सचे (win_ignore)
	गच्छ BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    सचे (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	गच्छ BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    सचे (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	गच्छ BYPASS_AU;

    सचे (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_changed_गणनम्;

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
    सचे (fname_io == रिक्त)
    {
	सचे (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = रिक्त;
	अञ्ञथा सचे (fname != रिक्त && !ends_excmd(*fname))
	    autocmd_fname = fname;
	अञ्ञथा सचे (buf != रिक्त)
	    autocmd_fname = buf->b_ffname;
	अञ्ञथा
	    autocmd_fname = रिक्त;
    }
    अञ्ञथा
	autocmd_fname = fname_io;
    सचे (autocmd_fname != रिक्त)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_full = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    सचे (buf == रिक्त)
	autocmd_bufnr = 0;
    अञ्ञथा
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    सचे (fname == रिक्त || *fname == NUL)
    {
	सचे (buf == रिक्त)
	    fname = रिक्त;
	अञ्ञथा
	{
#ifdef FEAT_SYN_HL
	    सचे (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    अञ्ञथा
#endif
		सचे (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		अञ्ञथा
		{
		    सचे (buf->b_sfname != रिक्त)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	सचे (fname == रिक्त)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    अञ्ञथा
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	सचे (event == EVENT_FILETYPE
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
	अञ्ञथा
	    fname = FullName_save(fname, FALSE);
    }
    सचे (fname == रिक्त)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	गच्छ BYPASS_AU;
    }

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    सचे (sfname != रिक्त)
	forward_slash(sfname);
    forward_slash(fname);
#endif

#ifdef VMS
    // remove version for correct match
    सचे (sfname != रिक्त)
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
    estack_प्रक्षिप(ETYPE_AUCMD, रिक्त, 0);
    ESTACK_CHECK_SETUP;

    save_current_sctx = current_sctx;

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    सचे (do_profiling == PROF_YES)
	prof_child_enter(&wait_time); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_entry);
#endif

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    सचे (!autocmd_busy)
    {
	save_अन्वेषणम्_patterns();
	सचे (!ins_compl_active())
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
    सचे (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    tail = gettail(fname);

    // Find first autocommand that matches
    CLEAR_FIELD(patcmd);
    patcmd.curpat = first_autopat[(पूर्णाङ्क)event];
    patcmd.group = group;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.tail = tail;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_next_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    सचे (patcmd.curpat != रिक्त)
    {
	// add to active_apc_list
	patcmd.next = active_apc_list;
	active_apc_list = &patcmd;

#ifdef FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (long)get_vim_var_nr(VV_CMDBANG);
	सचे (eap != रिक्त)
	{
	    save_cmdarg = set_cmdarg(eap, रिक्त);
	    set_vim_var_nr(VV_CMDBANG, (long)eap->forceit);
	}
	अञ्ञथा
	    save_cmdarg = रिक्त;	// avoid gcc warning
#endif
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	अत्थाय (ap = patcmd.curpat; ap->next != रिक्त; ap = ap->next)
	    ap->last = FALSE;
	ap->last = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	सचे (nesting == 1)
	    check_lnums(TRUE);
	अञ्ञथा
	    check_lnums_nested(TRUE);

	पूर्णाङ्क save_did_emsg = did_emsg;
	पूर्णाङ्क save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(रिक्त, getnextac, (शून्य *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	set_pressedreturn(save_ex_pressedreturn);

	सचे (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ifdef FEAT_EVAL
	सचे (eap != रिक्त)
	{
	    (शून्य)set_cmdarg(रिक्त, save_cmdarg);
	    set_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#endif
	// delete from active_apc_list
	सचे (active_apc_list == &patcmd)	    // just in case
	    active_apc_list = patcmd.next;
    }

    सचे (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_निष्कासय();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_full = save_autocmd_fname_full;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_match = save_autocmd_match;
    current_sctx = save_current_sctx;
#ifdef FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    सचे (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif
    कुञ्चिका_Typed = save_कुञ्चिका_Typed;
    vim_free(fname);
    vim_free(sfname);
    --nesting;		// see matching increment above

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.  Free any buffers in the au_pending_free_buf list and
     * free any windows in the au_pending_free_win list.
     */
    सचे (!autocmd_busy)
    {
	restore_अन्वेषणम्_patterns();
	सचे (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	याव (au_pending_free_buf != रिक्त)
	{
	    buf_T *b = au_pending_free_buf->b_next;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	याव (au_pending_free_win != रिक्त)
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
    सचे (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	सचे (curbuf->b_changed != save_changed)
	    need_maketitle = TRUE;
	curbuf->b_changed = save_changed;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    सचे (event == EVENT_BUFWIPEOUT && buf != रिक्त)
	aubuflocal_remove(buf);

    सचे (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    पटिनिवत्त retval;
}

#ifdef FEAT_EVAL
स्थिर char_u	*old_termresponse = रिक्त;
स्थिर char_u	*old_termu7resp = रिक्त;
स्थिर char_u	*old_termblinkresp = रिक्त;
स्थिर char_u	*old_termrbgresp = रिक्त;
स्थिर char_u	*old_termrfgresp = रिक्त;
स्थिर char_u	*old_termstyleresp = रिक्त;
#endif

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    शून्य
block_autocmds(शून्य)
{
#ifdef FEAT_EVAL
    // Remember the value of v:termresponse.
    सचे (autocmd_blocked == 0)
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

    शून्य
unblock_autocmds(शून्य)
{
    --autocmd_blocked;

#ifdef FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    सचे (autocmd_blocked == 0)
    {
	सचे (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, रिक्त, रिक्त, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", रिक्त, FALSE, curbuf);
	}
	सचे (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", रिक्त, FALSE, curbuf);
	}
	सचे (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", रिक्त, FALSE, curbuf);
	}
	सचे (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", रिक्त, FALSE, curbuf);
	}
	सचे (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", रिक्त, FALSE, curbuf);
	}
	सचे (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", रिक्त, FALSE, curbuf);
	}
    }
#endif
}

    पूर्णाङ्क
is_autocmd_blocked(शून्य)
{
    पटिनिवत्त autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    स्थिर शून्य
auto_next_pat(
    AutoPatCmd_T *apc,
    पूर्णाङ्क		stop_at_last)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*entry;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(entry->es_name);
    entry->es_info.aucmd = रिक्त;

    अत्थाय (ap = apc->curpat; ap != रिक्त && !got_int; ap = ap->next)
    {
	apc->curpat = रिक्त;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	सचे (ap->pat != रिक्त && ap->cmds != रिक्त
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
	    // execution-condition
	    सचे (ap->buflocal_nr == 0
		    ? (match_file_pat(रिक्त, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->tail, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		वर्ण	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->length + ap->patlen + 1);
		सचे (namep != रिक्त)
		{
		    sprintf((वर्ण *)namep, fmt, (वर्ण *)event_name->string, (वर्ण *)ap->pat);
		    सचे (p_verbose >= 8)
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
		अत्थाय (cp = ap->cmds; cp->next != रिक्त; cp = cp->next)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    पङ्क्तिः_breakcheck();
	    सचे (apc->curpat != रिक्त)	    // found a match
		भङ्ग;
	}
	सचे (stop_at_last && ap->last)
	    भङ्ग;
    }
}

#सचे defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    पटिनिवत्त &acp->script_ctx;
}
#endif

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	पूर्णाङ्क c UNUSED,
	शून्य *cookie,
	पूर्णाङ्क indent UNUSED,
	getline_opt_T options UNUSED)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    सचे (acp->curpat == रिक्त)
	पटिनिवत्त रिक्त;

    // repeat until we find an autocommand to execute
    अत्थाय (;;)
    {
	// skip removed commands
	याव (acp->nextcmd != रिक्त && acp->nextcmd->cmd == रिक्त)
	    सचे (acp->nextcmd->last)
		acp->nextcmd = रिक्त;
	    अञ्ञथा
		acp->nextcmd = acp->nextcmd->next;

	सचे (acp->nextcmd != रिक्त)
	    भङ्ग;

	// at end of commands, find next pattern that matches
	सचे (acp->curpat->last)
	    acp->curpat = रिक्त;
	अञ्ञथा
	    acp->curpat = acp->curpat->next;
	सचे (acp->curpat != रिक्त)
	    auto_next_pat(acp, TRUE);
	सचे (acp->curpat == रिक्त)
	    पटिनिवत्त रिक्त;
    }

    ac = acp->nextcmd;

    सचे (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	सन्देशः_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    सचे (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    सचे (ac->last)
	acp->nextcmd = रिक्त;
    अञ्ञथा
	acp->nextcmd = ac->next;
    पटिनिवत्त retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    स्थिर पूर्णाङ्क
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_full)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*tail = gettail(sfname);
    पूर्णाङ्क		retval = FALSE;

    सचे (first_autopat[(पूर्णाङ्क)event] == रिक्त)
	पटिनिवत्त FALSE;

    fname = is_full ? vim_strsave(sfname) : FullName_save(sfname, FALSE);
    सचे (fname == रिक्त)
	पटिनिवत्त FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    सचे (sfname != रिक्त)
	forward_slash(sfname);
    forward_slash(fname);
#endif

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	सचे (ap->pat != रिक्त && ap->cmds != रिक्त
	      && (ap->buflocal_nr == 0
		? match_file_pat(रिक्त, &ap->reg_prog,
					  fname, sfname, tail, ap->allow_dirs)
		: buf != रिक्त && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    भङ्ग;
	}

    vim_free(fname);
#ifdef BACKSLASH_IN_FILENAME
    vim_free(sfname);
#endif

    पटिनिवत्त retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    पूर्णाङ्क
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    पटिनिवत्त has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    पूर्णाङ्क
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    पटिनिवत्त has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    सचे (idx == augroups.ga_len)		// add "END" add the end
	पटिनिवत्त (char_u *)"END";
    सचे (idx < 0 || idx >= augroups.ga_len)	// end of list
	पटिनिवत्त रिक्त;
    सचे (AUGROUP_NAME(idx) == रिक्त || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	पटिनिवत्त (char_u *)"";
    पटिनिवत्त AUGROUP_NAME(idx);		// return a name
}

स्थिर पूर्णाङ्क include_groups = FALSE;

    char_u  *
set_context_in_autocmd(
    expand_T	*xp,
    char_u	*arg,
    पूर्णाङ्क		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    पूर्णाङ्क		group;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    group = au_get_grouparg(&arg);
    सचे (group == AUGROUP_ERROR)
	पटिनिवत्त रिक्त;
    // If there only is a group name that's what we expand.
    सचे (*arg == NUL && group != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	group = AUGROUP_ALL;
    }

    // skip over event name
    अत्थाय (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	सचे (*p == ',')
	    arg = p + 1;
    सचे (*p == NUL)
    {
	सचे (group == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_pattern = arg;
	पटिनिवत्त रिक्त;
    }

    // skip over pattern
    arg = skipwhite(p);
    याव (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    सचे (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	अत्थाय (पूर्णाङ्क i = 0; i < 2; i++)
	{
	    सचे (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    सचे (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    सचे (!in_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	पटिनिवत्त arg;			    // expand (next) command
    }

    सचे (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    अञ्ञथा
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    पटिनिवत्त रिक्त;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    पूर्णाङ्क i;

    सचे (idx < augroups.ga_len)		// First list group names, if wanted
    {
	सचे (!include_groups || AUGROUP_NAME(idx) == रिक्त
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    पटिनिवत्त (char_u *)"";	// skip deleted entries
	पटिनिवत्त AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    सचे (i < 0 || i >= अङ्कः_EVENTS)
	पटिनिवत्त रिक्त;

    पटिनिवत्त event_tab[i].मानम्.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_group(expand_T *xp UNUSED, पूर्णाङ्क idx, पूर्णाङ्क win)
{
    सचे (idx < 0 || idx >= अङ्कः_EVENTS)
	पटिनिवत्त रिक्त;

    सचे (!win)
	पटिनिवत्त event_tab[idx].मानम्.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    पूर्णाङ्क j = 0;
    अत्थाय (पूर्णाङ्क i = 0; i < अङ्कः_EVENTS; ++i)
    {
	j += event_tab[i].कुञ्चिका <= 0;
	सचे (j == idx + 1)
	    पटिनिवत्त event_tab[i].मानम्.string;
    }

    पटिनिवत्त रिक्त;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    पूर्णाङ्क
has_tabclosedpre(शून्य)
{
    पटिनिवत्त (first_autopat[(पूर्णाङ्क)EVENT_TABCLOSEDPRE] != रिक्त);
}

#सचे defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    पूर्णाङ्क
autocmd_supported(char_u *name)
{
    char_u *p;

    पटिनिवत्त (event_name2nr(name, &p) != अङ्कः_EVENTS);
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
    पूर्णाङ्क
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*pattern = रिक्त;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = रिक्त;
    पूर्णाङ्क		group;
    पूर्णाङ्क		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    सचे (arg_save == रिक्त)
	पटिनिवत्त FALSE;
    p = vim_strchr(arg_save, '#');
    सचे (p != रिक्त)
	*p++ = NUL;

    // First, look for an autocmd group name
    group = au_find_group(arg_save);
    सचे (group == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	group = AUGROUP_ALL;
	event_name = arg_save;
    }
    अञ्ञथा
    {
	सचे (p == रिक्त)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    गच्छ theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	सचे (p != रिक्त)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    pattern = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    सचे (event == अङ्कः_EVENTS)
	गच्छ theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = first_autopat[(पूर्णाङ्क)event];
    सचे (ap == रिक्त)
	गच्छ theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    सचे (pattern != रिक्त && STRICMP(pattern, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    अत्थाय ( ; ap != रिक्त; ap = ap->next)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	सचे (ap->pat != रिक्त && ap->cmds != रिक्त
	    && (group == AUGROUP_ALL || ap->group == group)
	    && (pattern == रिक्त
		|| (buflocal_buf == रिक्त
		    ? fnamecmp(ap->pat, pattern) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    भङ्ग;
	}

theend:
    vim_free(arg_save);
    पटिनिवत्त retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    स्थिर शून्य
autocmd_add_or_delete(typval_T *argvars, typval_T *rettv, पूर्णाङ्क delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = रिक्त;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*group_name = रिक्त;
    पूर्णाङ्क		group;
    char_u	*pat = रिक्त;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = रिक्त;
    char_u	*end;
    पूर्णाङ्क		once;
    पूर्णाङ्क		nested;
    पूर्णाङ्क		replace;		// replace the cmd for a group/event
    पूर्णाङ्क		retval = VVAL_TRUE;
    पूर्णाङ्क		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_सङ्ख्या = VVAL_FALSE;

    सचे (check_restricted() || check_secure())
	पटिनिवत्त;

    सचे (check_for_list_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    aucmd_list = argvars[0].vval.v_list;
    सचे (aucmd_list == रिक्त)
	पटिनिवत्त;

    FOR_ALL_LIST_वस्तूनि(aucmd_list, li)
    {
	VIM_CLEAR(group_name);
	VIM_CLEAR(cmd);
	event_name = रिक्त;
	event_list = रिक्त;
	pat = रिक्त;
	pat_list = रिक्त;

	सचे (li->li_tv.v_type != VAR_DICT)
	    अनुवर्त;

	event_dict = li->li_tv.vval.v_dict;
	सचे (event_dict == रिक्त)
	    अनुवर्त;

	di = dict_find(event_dict, (char_u *)"event", -1);
	सचे (di != रिक्त)
	{
	    सचे (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		सचे (event_name == रिक्त)
		{
		    emsg(_(e_string_required));
		    अनुवर्त;
		}
	    }
	    अञ्ञथा सचे (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		सचे (event_list == रिक्त)
		{
		    emsg(_(e_list_required));
		    अनुवर्त;
		}
	    }
	    अञ्ञथा
	    {
		emsg(_(e_string_or_list_expected));
		अनुवर्त;
	    }
	}

	group_name = dict_get_string(event_dict, "group", TRUE);
	सचे (group_name == रिक्त || *group_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    group = current_augroup;
	अञ्ञथा
	{
	    group = au_find_group(group_name);
	    सचे (group == AUGROUP_ERROR)
	    {
		सचे (delete)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    भङ्ग;
		}
		// group is not found, create it now
		group = au_new_group(group_name);
		सचे (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    भङ्ग;
		}

		current_augroup = group;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	सचे (dict_has_कुञ्चिका(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_सङ्ख्या_def(event_dict, "bufnr", -1);
	    सचे (bnum == -1)
		अनुवर्त;

	    vim_snprintf((वर्ण *)IObuff, IOSIZE, "<buffer=%d>", (पूर्णाङ्क)bnum);
	    pat = IObuff;
	}
	अञ्ञथा
	{
	    di = dict_find(event_dict, (char_u *)"pattern", -1);
	    सचे (di != रिक्त)
	    {
		सचे (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    सचे (pat == रिक्त)
		    {
			emsg(_(e_string_required));
			अनुवर्त;
		    }
		}
		अञ्ञथा सचे (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    सचे (pat_list == रिक्त)
		    {
			emsg(_(e_list_required));
			अनुवर्त;
		    }
		}
		अञ्ञथा
		{
		    emsg(_(e_string_or_list_expected));
		    अनुवर्त;
		}
	    }
	    अञ्ञथा सचे (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	सचे (cmd == रिक्त)
	{
	    सचे (delete)
		cmd = vim_strsave((char_u *)"");
	    अञ्ञथा
		अनुवर्त;
	}

	सचे (delete && (event_name == रिक्त
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    अत्थाय (event = (event_T)0; (पूर्णाङ्क)event < अङ्कः_EVENTS;
		    event = (event_T)((पूर्णाङ्क)event + 1))
	    {
		सचे (do_autocmd_event(event, pat, once, nested, cmd, delete,
							group, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    भङ्ग;
		}
	    }
	}
	अञ्ञथा
	{
	    char_u *p = रिक्त;

	    eli = रिक्त;
	    end = रिक्त;
	    याव (TRUE)
	    {
		सचे (event_list != रिक्त)
		{
		    सचे (eli == रिक्त)
			eli = event_list->lv_first;
		    अञ्ञथा
			eli = eli->li_next;
		    सचे (eli == रिक्त)
			भङ्ग;
		    सचे (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == रिक्त)
		    {
			emsg(_(e_string_required));
			भङ्ग;
		    }
		}
		अञ्ञथा
		{
		    सचे (p == रिक्त)
			p = event_name;
		    सचे (p == रिक्त || *p == NUL)
			भङ्ग;
		}

		event = event_name2nr(p, &end);
		सचे (event == अङ्कः_EVENTS || *end != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    भङ्ग;
		}
		सचे (pat != रिक्त)
		{
		    सचे (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, group, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			भङ्ग;
		    }
		}
		अञ्ञथा सचे (pat_list != रिक्त)
		{
		    FOR_ALL_LIST_वस्तूनि(pat_list, pli)
		    {
			सचे (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == रिक्त)
			{
			    emsg(_(e_string_required));
			    अनुवर्त;
			}
			सचे (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, group, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    भङ्ग;
			}
		    }
		    सचे (retval == VVAL_FALSE)
			भङ्ग;
		}
		सचे (event_name != रिक्त)
		    p = end;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	सचे (delete && group_name != रिक्त &&
		(event_name == रिक्त || event_name[0] == NUL)
		&& (pat == रिक्त || pat[0] == NUL)
		&& (cmd == रिक्त || cmd[0] == NUL))
	    au_del_group(group_name);
    }

    VIM_CLEAR(group_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_सङ्ख्या = retval;
}

/*
 * autocmd_add() function
 */
    शून्य
f_autocmd_add(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    शून्य
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    शून्य
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = अङ्कः_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = रिक्त;
    char_u	*name = रिक्त;
    पूर्णाङ्क		group = AUGROUP_ALL;

    सचे (rettv_list_alloc(rettv) == FAIL)
	पटिनिवत्त;
    सचे (check_for_opt_dict_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    सचे (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	सचे (dict_has_कुञ्चिका(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    सचे (name == रिक्त)
		पटिनिवत्त;

	    सचे (*name == NUL)
		group = AUGROUP_DEFAULT;
	    अञ्ञथा
	    {
		group = au_find_group(name);
		सचे (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), name);
		    vim_free(name);
		    पटिनिवत्त;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	सचे (dict_has_कुञ्चिका(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    सचे (name == रिक्त)
		पटिनिवत्त;

	    सचे (name[0] == '*' && name[1] == NUL)
		event_arg = अङ्कः_EVENTS;
	    अञ्ञथा
	    {
		keyvalue_T target;
		keyvalue_T *entry;

		target.कुञ्चिका = 0;
		target.मानम्.string = name;
		target.मानम्.length = STRLEN(target.मानम्.string);
		entry = (keyvalue_T *)bsearch(&target, &event_tab,
		    अङ्कः_EVENTS, परिमाण(event_tab[0]), cmp_keyvalue_मानम्_ni);
		सचे (entry == रिक्त)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    पटिनिवत्त;
		}
		event_arg = (event_T)abs(entry->कुञ्चिका);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	सचे (dict_has_कुञ्चिका(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    सचे (pat == रिक्त)
		पटिनिवत्त;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    अत्थाय (event = (event_T)0; (पूर्णाङ्क)event < अङ्कः_EVENTS;
	    event = (event_T)((पूर्णाङ्क)event + 1))
    {
	string_T    *event_name;

	सचे (event_arg != अङ्कः_EVENTS && event != event_arg)
	    अनुवर्त;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	group_name;

	    सचे (ap->pat == रिक्त)		// pattern has been removed
		अनुवर्त;

	    सचे (group != AUGROUP_ALL && group != ap->group)
		अनुवर्त;

	    सचे (pat != रिक्त && STRCMP(pat, ap->pat) != 0)
		अनुवर्त;

	    group_name.string = get_augroup_name(रिक्त, ap->group);
	    सचे (group_name.string == रिक्त)
		STR_LITERAL_SET(group_name, "");
	    अञ्ञथा
		group_name.length = STRLEN(group_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    अत्थाय (ac = ap->cmds; ac != रिक्त; ac = ac->next)
	    {
		event_dict = dict_alloc();
		सचे (event_dict == रिक्त
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    पटिनिवत्त;
		}

		सचे (dict_add_string_len(event_dict, "event",
			event_name->string, (पूर्णाङ्क)event_name->length) == FAIL
			|| dict_add_string_len(event_dict, "group",
			    group_name.string, (पूर्णाङ्क)group_name.length) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_add_सङ्ख्या(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_add_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_add_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_add_bool(event_dict, "once", ac->once) == FAIL
			|| dict_add_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    पटिनिवत्त;
		}
	    }
	}
    }

    vim_free(pat);
}

#endif
