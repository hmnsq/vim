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
قسمتعریف ساخت AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    حرف	    once;		// "One shot": removed after execution
    حرف	    nested;		// If autocommands nest here.
    حرف	    آخری;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    ساخت AutoCmd  *اگلا;		// next AutoCmd in list
} AutoCmd;

قسمتعریف ساخت AutoPat
{
    ساخت AutoPat  *اگلا;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    صحیح		    گروہ;		// group ID
    صحیح		    patlen;		// strlen() of pat
    صحیح		    buflocal_nr;	// !=0 for buffer-local AutoPat
    حرف	    allow_dirs;		// Pattern may match whole path
    حرف	    آخری;		// last pattern for apply_autocmds()
} AutoPat;

//
// special cases:
// BufNewFile and BufRead are searched for ALOT (especially at startup)
// so we pre-determine their index into the event_tab[] table for fast access.
// Keep these values in sync with event_tab[]!
#تعریف_کرو BUFNEWFILE_اشاریہ 9
#تعریف_کرو BUFREAD_اشاریہ 10

// Must be sorted by the 'value' field because it is used by bsearch()!
// Events with positive keys aren't allowed in 'eventignorewin'.
ساکن keyvalue_T event_tab[عدد_EVENTS] = {
    KEYVALUE_اندراج(-EVENT_BUFADD, "BufAdd"),
    KEYVALUE_اندراج(-EVENT_BUFADD, "BufCreate"),
    KEYVALUE_اندراج(-EVENT_BUFDELETE, "BufDelete"),
    KEYVALUE_اندراج(-EVENT_BUFENTER, "BufEnter"),
    KEYVALUE_اندراج(-EVENT_BUFFILEPOST, "BufFilePost"),
    KEYVALUE_اندراج(-EVENT_BUFFILEPRE, "BufFilePre"),
    KEYVALUE_اندراج(-EVENT_BUFHIDDEN, "BufHidden"),
    KEYVALUE_اندراج(-EVENT_BUFLEAVE, "BufLeave"),
    KEYVALUE_اندراج(-EVENT_BUFNEW, "BufNew"),
    KEYVALUE_اندراج(-EVENT_BUFNEWFILE, "BufNewFile"),	// BUFNEWFILE_INDEX
    KEYVALUE_اندراج(-EVENT_BUFREADPOST, "BufRead"),	// BUFREAD_INDEX
    KEYVALUE_اندراج(-EVENT_BUFREADCMD, "BufReadCmd"),
    KEYVALUE_اندراج(-EVENT_BUFREADPOST, "BufReadPost"),
    KEYVALUE_اندراج(-EVENT_BUFREADPRE, "BufReadPre"),
    KEYVALUE_اندراج(-EVENT_BUFUNLOAD, "BufUnload"),
    KEYVALUE_اندراج(-EVENT_BUFWINENTER, "BufWinEnter"),
    KEYVALUE_اندراج(-EVENT_BUFWINLEAVE, "BufWinLeave"),
    KEYVALUE_اندراج(-EVENT_BUFWIPEOUT, "BufWipeout"),
    KEYVALUE_اندراج(-EVENT_BUFWRITEPRE, "BufWrite"),
    KEYVALUE_اندراج(-EVENT_BUFWRITECMD, "BufWriteCmd"),
    KEYVALUE_اندراج(-EVENT_BUFWRITEPOST, "BufWritePost"),
    KEYVALUE_اندراج(-EVENT_BUFWRITEPRE, "BufWritePre"),
    KEYVALUE_اندراج(EVENT_CMDLINECHANGED, "CmdlineChanged"),
    KEYVALUE_اندراج(EVENT_CMDLINEENTER, "CmdlineEnter"),
    KEYVALUE_اندراج(EVENT_CMDLINELEAVE, "CmdlineLeave"),
    KEYVALUE_اندراج(EVENT_CMDLINELEAVEPRE, "CmdlineLeavePre"),
    KEYVALUE_اندراج(EVENT_CMDUNDEFINED, "CmdUndefined"),
    KEYVALUE_اندراج(EVENT_CMDWINENTER, "CmdwinEnter"),
    KEYVALUE_اندراج(EVENT_CMDWINLEAVE, "CmdwinLeave"),
    KEYVALUE_اندراج(EVENT_COLORSCHEME, "ColorScheme"),
    KEYVALUE_اندراج(EVENT_COLORSCHEMEPRE, "ColorSchemePre"),
    KEYVALUE_اندراج(EVENT_COMPLETECHANGED, "CompleteChanged"),
    KEYVALUE_اندراج(EVENT_COMPLETEDONE, "CompleteDone"),
    KEYVALUE_اندراج(EVENT_COMPLETEDONEPRE, "CompleteDonePre"),
    KEYVALUE_اندراج(-EVENT_CURSORHOLD, "CursorHold"),
    KEYVALUE_اندراج(-EVENT_CURSORHOLDI, "CursorHoldI"),
    KEYVALUE_اندراج(-EVENT_CURSORMOVED, "CursorMoved"),
    KEYVALUE_اندراج(-EVENT_CURSORMOVEDC, "CursorMovedC"),
    KEYVALUE_اندراج(-EVENT_CURSORMOVEDI, "CursorMovedI"),
    KEYVALUE_اندراج(EVENT_DIFFUPDATED, "DiffUpdated"),
    KEYVALUE_اندراج(EVENT_DIRCHANGED, "DirChanged"),
    KEYVALUE_اندراج(EVENT_DIRCHANGEDPRE, "DirChangedPre"),
    KEYVALUE_اندراج(EVENT_ENCODINGCHANGED, "EncodingChanged"),
    KEYVALUE_اندراج(EVENT_EXITPRE, "ExitPre"),
    KEYVALUE_اندراج(-EVENT_FILEAPPENDCMD, "FileAppendCmd"),
    KEYVALUE_اندراج(-EVENT_FILEAPPENDPOST, "FileAppendPost"),
    KEYVALUE_اندراج(-EVENT_FILEAPPENDPRE, "FileAppendPre"),
    KEYVALUE_اندراج(-EVENT_FILECHANGEDRO, "FileChangedRO"),
    KEYVALUE_اندراج(-EVENT_FILECHANGEDSHELL, "FileChangedShell"),
    KEYVALUE_اندراج(-EVENT_FILECHANGEDSHELLPOST, "FileChangedShellPost"),
    KEYVALUE_اندراج(EVENT_ENCODINGCHANGED, "FileEncoding"),
    KEYVALUE_اندراج(-EVENT_FILEREADCMD, "FileReadCmd"),
    KEYVALUE_اندراج(-EVENT_FILEREADPOST, "FileReadPost"),
    KEYVALUE_اندراج(-EVENT_FILEREADPRE, "FileReadPre"),
    KEYVALUE_اندراج(-EVENT_FILETYPE, "FileType"),
    KEYVALUE_اندراج(-EVENT_FILEWRITECMD, "FileWriteCmd"),
    KEYVALUE_اندراج(-EVENT_FILEWRITEPOST, "FileWritePost"),
    KEYVALUE_اندراج(-EVENT_FILEWRITEPRE, "FileWritePre"),
    KEYVALUE_اندراج(-EVENT_FILTERREADPOST, "FilterReadPost"),
    KEYVALUE_اندراج(-EVENT_FILTERREADPRE, "FilterReadPre"),
    KEYVALUE_اندراج(-EVENT_FILTERWRITEPOST, "FilterWritePost"),
    KEYVALUE_اندراج(-EVENT_FILTERWRITEPRE, "FilterWritePre"),
    KEYVALUE_اندراج(EVENT_FOCUSGAINED, "FocusGained"),
    KEYVALUE_اندراج(EVENT_FOCUSLOST, "FocusLost"),
    KEYVALUE_اندراج(EVENT_FUNCUNDEFINED, "FuncUndefined"),
    KEYVALUE_اندراج(EVENT_GUIENTER, "GUIEnter"),
    KEYVALUE_اندراج(EVENT_GUIFAILED, "GUIFailed"),
    KEYVALUE_اندراج(-EVENT_INSERTCHANGE, "InsertChange"),
    KEYVALUE_اندراج(-EVENT_INSERTCHARPRE, "InsertCharPre"),
    KEYVALUE_اندراج(-EVENT_INSERTENTER, "InsertEnter"),
    KEYVALUE_اندراج(-EVENT_INSERTLEAVE, "InsertLeave"),
    KEYVALUE_اندراج(-EVENT_INSERTLEAVEPRE, "InsertLeavePre"),
    KEYVALUE_اندراج(EVENT_KEYINPUTPRE, "KeyInputPre"),
    KEYVALUE_اندراج(EVENT_MENUPOPUP, "MenuPopup"),
    KEYVALUE_اندراج(EVENT_MODECHANGED, "ModeChanged"),
    KEYVALUE_اندراج(EVENT_OPTIONSET, "OptionSet"),
    KEYVALUE_اندراج(EVENT_QUICKFIXCMDPOST, "QuickFixCmdPost"),
    KEYVALUE_اندراج(EVENT_QUICKFIXCMDPRE, "QuickFixCmdPre"),
    KEYVALUE_اندراج(EVENT_QUITPRE, "QuitPre"),
    KEYVALUE_اندراج(EVENT_REMOTEREPLY, "RemoteReply"),
    KEYVALUE_اندراج(EVENT_SAFESTATE, "SafeState"),
    KEYVALUE_اندراج(EVENT_SAFESTATEAGAIN, "SafeStateAgain"),
    KEYVALUE_اندراج(EVENT_SESSIONLOADPOST, "SessionLoadPost"),
    KEYVALUE_اندراج(EVENT_SESSIONLOADPRE, "SessionLoadPre"),
    KEYVALUE_اندراج(EVENT_SESSIONWRITEPOST, "SessionWritePost"),
    KEYVALUE_اندراج(EVENT_SHELLCMDPOST, "ShellCmdPost"),
    KEYVALUE_اندراج(-EVENT_SHELLFILTERPOST, "ShellFilterPost"),
    KEYVALUE_اندراج(EVENT_SIGUSR1, "SigUSR1"),
    KEYVALUE_اندراج(EVENT_SOURCECMD, "SourceCmd"),
    KEYVALUE_اندراج(EVENT_SOURCEPOST, "SourcePost"),
    KEYVALUE_اندراج(EVENT_SOURCEPRE, "SourcePre"),
    KEYVALUE_اندراج(EVENT_SPELLFILEMISSING, "SpellFileMissing"),
    KEYVALUE_اندراج(EVENT_STDINREADPOST, "StdinReadPost"),
    KEYVALUE_اندراج(EVENT_STDINREADPRE, "StdinReadPre"),
    KEYVALUE_اندراج(EVENT_SWAPEXISTS, "SwapExists"),
    KEYVALUE_اندراج(EVENT_SYNTAX, "Syntax"),
    KEYVALUE_اندراج(EVENT_TABCLOSED, "TabClosed"),
    KEYVALUE_اندراج(EVENT_TABCLOSEDPRE, "TabClosedPre"),
    KEYVALUE_اندراج(EVENT_TABENTER, "TabEnter"),
    KEYVALUE_اندراج(EVENT_TABLEAVE, "TabLeave"),
    KEYVALUE_اندراج(EVENT_TABNEW, "TabNew"),
    KEYVALUE_اندراج(EVENT_TERMCHANGED, "TermChanged"),
    KEYVALUE_اندراج(EVENT_TERMINALOPEN, "TerminalOpen"),
    KEYVALUE_اندراج(EVENT_TERMINALWINOPEN, "TerminalWinOpen"),
    KEYVALUE_اندراج(EVENT_TERMRESPONSE, "TermResponse"),
    KEYVALUE_اندراج(EVENT_TERMRESPONSEALL, "TermResponseAll"),
    KEYVALUE_اندراج(-EVENT_TEXTCHANGED, "TextChanged"),
    KEYVALUE_اندراج(-EVENT_TEXTCHANGEDI, "TextChangedI"),
    KEYVALUE_اندراج(-EVENT_TEXTCHANGEDP, "TextChangedP"),
    KEYVALUE_اندراج(-EVENT_TEXTCHANGEDT, "TextChangedT"),
    KEYVALUE_اندراج(-EVENT_TEXTPUTPOST, "TextPutPost"),
    KEYVALUE_اندراج(-EVENT_TEXTPUTPRE, "TextPutPre"),
    KEYVALUE_اندراج(-EVENT_TEXTYANKPOST, "TextYankPost"),
    KEYVALUE_اندراج(EVENT_USER, "User"),
    KEYVALUE_اندراج(EVENT_VIMENTER, "VimEnter"),
    KEYVALUE_اندراج(EVENT_VIMLEAVE, "VimLeave"),
    KEYVALUE_اندراج(EVENT_VIMLEAVEPRE, "VimLeavePre"),
    KEYVALUE_اندراج(EVENT_VIMRESIZED, "VimResized"),
    KEYVALUE_اندراج(EVENT_VIMRESUME, "VimResume"),
    KEYVALUE_اندراج(EVENT_VIMSUSPEND, "VimSuspend"),
    KEYVALUE_اندراج(-EVENT_WINCLOSED, "WinClosed"),
    KEYVALUE_اندراج(-EVENT_WINENTER, "WinEnter"),
    KEYVALUE_اندراج(-EVENT_WINLEAVE, "WinLeave"),
    KEYVALUE_اندراج(EVENT_WINNEW, "WinNew"),
    KEYVALUE_اندراج(EVENT_WINNEWPRE, "WinNewPre"),
    KEYVALUE_اندراج(-EVENT_WINRESIZED, "WinResized"),
    KEYVALUE_اندراج(-EVENT_WINSCROLLED, "WinScrolled"),
};

ساکن AutoPat *پہلا_autopat[عدد_EVENTS] = { تہی };
ساکن AutoPat *آخری_autopat[عدد_EVENTS] = { تہی };

#تعریف_کرو AUGROUP_DEFAULT    (-1)	    // default autocmd group
#تعریف_کرو AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#تعریف_کرو AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
ساخت AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    صحیح		گروہ;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*دم;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    صحیح		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *اگلا;		// chain of active apc-s for auto-invalidation
};

ساکن AutoPatCmd_T *فعال_apc_list = تہی; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#تعریف_کرو FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    برائے ((ap) = پہلا_autopat[(صحیح)(event)]; (ap) != تہی; (ap) = (ap)->اگلا)

/*
 * augroups stores a list of autocmd group names.
 */
ساکن garray_T augroups = {0, 0, حجم(char_u *), 10, تہی};
#تعریف_کرو AUGROUP_NAME(i) (((char_u **)augroups.ga_ڈیٹا)[i])
// use get_deleted_augroup() to get this
ساکن char_u *deleted_augroup = تہی;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
ساکن صحیح current_augroup = AUGROUP_DEFAULT;

ساکن صحیح au_need_clean = FALSE;   // need to delete marked patterns

ساکن event_T event_name2nr(char_u *آغاز, char_u **انتہا);
ساکن string_T *event_nr2name(event_T event);
ساکن صحیح au_get_grouparg(char_u **argp);
ساکن صحیح do_autocmd_event(event_T event, char_u *pat, صحیح once, صحیح nested, char_u *cmd, صحیح forceit, صحیح گروہ, صحیح flags);
ساکن صحیح apply_autocmds_گروہ(event_T event, char_u *fname, char_u *fname_io, صحیح force, صحیح گروہ, buf_T *buf, exarg_T *eap);
ساکن خالی auto_اگلا_pat(AutoPatCmd_T *apc, صحیح stop_at_آخری);
ساکن صحیح au_ڈھونڈو_گروہ(char_u *name);

ساکن event_T	آخری_event;
ساکن صحیح	آخری_گروہ;
ساکن صحیح	autocmd_blocked = 0;	// block all autocmds

    ساکن char_u *
get_deleted_augroup(خالی)
{
    اگر (deleted_augroup == تہی)
	deleted_augroup = (char_u *)_("--Deleted--");
    واپس deleted_augroup;
}

/*
 * Show the autocommands for one AutoPat.
 */
    ساکن خالی
show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    // Check for "got_int" (here and at various places below), which is set
    // when "q" has been hit for the "--more--" prompt
    اگر (got_int)
	واپس;
    اگر (ap->pat == تہی)		// pattern has been removed
	واپس;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    پیغام_putchar('\n');
    اگر (got_int)
	جاؤ theend;
    اگر (event != آخری_event || ap->گروہ != آخری_گروہ)
    {
	string_T    *event_name;

	اگر (ap->گروہ != AUGROUP_DEFAULT)
	{
	    اگر (AUGROUP_NAME(ap->گروہ) == تہی)
		پیغام_puts_attr((حرف *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    ورنہ
		پیغام_puts_attr((حرف *)AUGROUP_NAME(ap->گروہ), HL_ATTR(HLF_T));
	    پیغام_puts("  ");
	}
	event_name = event_nr2name(event);
	پیغام_puts_attr((حرف *)event_name->string, HL_ATTR(HLF_T));
	آخری_event = event;
	آخری_گروہ = ap->گروہ;
	پیغام_putchar('\n');
	اگر (got_int)
	    جاؤ theend;
    }

    اگر (ap->pat == تہی)
	جاؤ theend;  // timer might have cleared the pattern or group

    پیغام_ستون = 4;
    پیغام_outtrans(ap->pat);

    برائے (ac = ap->cmds; ac != تہی; ac = ac->اگلا)
    {
	اگر (ac->cmd == تہی)		// skip removed commands
	    جاری;

	اگر (پیغام_ستون >= 14)
	    پیغام_putchar('\n');
	پیغام_ستون = 14;
	اگر (got_int)
	    جاؤ theend;
	پیغام_outtrans(ac->cmd);
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (p_verbose > 0)
	    آخری_سیٹ_پیغام(ac->script_ctx);
#اگر_ختم
	اگر (got_int)
	    جاؤ theend;
	اگر (ac->اگلا != تہی)
	{
	    پیغام_putchar('\n');
	    اگر (got_int)
		جاؤ theend;
	}
    }

theend:
    --autocmd_busy;
}

/*
 * Mark an autocommand pattern for deletion.
 */
    ساکن خالی
au_remove_pat(AutoPat *ap)
{
    VIM_CLEAR(ap->pat);
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    ساکن خالی
au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    برائے (ac = ap->cmds; ac != تہی; ac = ac->اگلا)
	VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

// Delete one command from an autocmd pattern.
ساکن خالی au_del_cmd(AutoCmd *ac)
{
    VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    ساکن خالی
au_cleanup(خالی)
{
    AutoPat	*ap, **پچھلا_ap;
    AutoCmd	*ac, **پچھلا_ac;
    event_T	event;

    اگر (autocmd_busy || !au_need_clean)
	واپس;

    // loop over all events
    برائے (event = (event_T)0; (صحیح)event < عدد_EVENTS;
					    event = (event_T)((صحیح)event + 1))
    {
	// loop over all autocommand patterns
	پچھلا_ap = &(پہلا_autopat[(صحیح)event]);
	برائے (ap = *پچھلا_ap; ap != تہی; ap = *پچھلا_ap)
	{
	    صحیح has_cmd = FALSE;

	    // loop over all commands for this pattern
	    پچھلا_ac = &(ap->cmds);
	    برائے (ac = *پچھلا_ac; ac != تہی; ac = *پچھلا_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		اگر (ap->pat == تہی || ac->cmd == تہی)
		{
		    *پچھلا_ac = ac->اگلا;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		ورنہ
		{
		    has_cmd = TRUE;
		    پچھلا_ac = &(ac->اگلا);
		}
	    }

	    اگر (ap->pat != تہی && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    اگر (ap->pat == تہی)
	    {
		اگر (ap->اگلا == تہی)
		{
		    اگر (پچھلا_ap == &(پہلا_autopat[(صحیح)event]))
			آخری_autopat[(صحیح)event] = تہی;
		    ورنہ
			// this depends on the "next" field being the first in
			// the struct
			آخری_autopat[(صحیح)event] = (AutoPat *)پچھلا_ap;
		}
		*پچھلا_ap = ap->اگلا;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    ورنہ
		پچھلا_ap = &(ap->اگلا);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Called when buffer is freed, to remove/invalidate related buffer-local
 * autocmds.
 */
    خالی
aubuflocal_remove(buf_T *buf)
{
    AutoPat	    *ap;
    event_T	    event;
    AutoPatCmd_T    *apc;

    // invalidate currently executing autocommands
    برائے (apc = فعال_apc_list; apc; apc = apc->اگلا)
	اگر (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    برائے (event = (event_T)0; (صحیح)event < عدد_EVENTS;
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
    ساکن صحیح
au_new_گروہ(char_u *name)
{
    صحیح		i;

    i = au_ڈھونڈو_گروہ(name);
    اگر (i != AUGROUP_ERROR)
	واپس i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    برائے (i = 0; i < augroups.ga_len; ++i)
	اگر (AUGROUP_NAME(i) == تہی)
	    توڑو;
    اگر (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	واپس AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    اگر (AUGROUP_NAME(i) == تہی)
	واپس AUGROUP_ERROR;
    اگر (i == augroups.ga_len)
	++augroups.ga_len;

    واپس i;
}

    ساکن خالی
au_del_گروہ(char_u *name)
{
    صحیح		i;
    event_T	event;
    AutoPat	*ap;
    صحیح		اندر_use = FALSE;


    i = au_ڈھونڈو_گروہ(name);
    اگر (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_گروہ_str), name);
	واپس;
    }
    اگر (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_گروہ));
	واپس;
    }

    برائے (event = (event_T)0; (صحیح)event < عدد_EVENTS;
	    event = (event_T)((صحیح)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    اگر (ap->گروہ == i && ap->pat != تہی)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		اندر_use = TRUE;
		event = عدد_EVENTS;
		توڑو;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    اگر (اندر_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    ورنہ
	AUGROUP_NAME(i) = تہی;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    ساکن صحیح
au_ڈھونڈو_گروہ(char_u *name)
{
    صحیح	    i;

    برائے (i = 0; i < augroups.ga_len; ++i)
	اگر (AUGROUP_NAME(i) != تہی && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    واپس i;
    واپس AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    صحیح
au_has_گروہ(char_u *name)
{
    واپس au_ڈھونڈو_گروہ(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    خالی
do_augroup(char_u *arg, صحیح del_گروہ)
{
    صحیح	    i;

    اگر (del_گروہ)
    {
	اگر (*arg == NUL)
	    emsg(_(e_دلیل_required));
	ورنہ
	    au_del_گروہ(arg);
    }
    ورنہ اگر (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    ورنہ اگر (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_گروہ(arg);
	اگر (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    ورنہ			    // ":aug": list the group names
    {
	پیغام_آغاز();
	برائے (i = 0; i < augroups.ga_len; ++i)
	{
	    اگر (AUGROUP_NAME(i) != تہی)
	    {
		پیغام_puts((حرف *)AUGROUP_NAME(i));
		پیغام_puts("  ");
	    }
	}
	پیغام_clr_eos();
	پیغام_انتہا();
    }
}

    خالی
autocmd_init(خالی)
{
    CLEAR_میدان(aucmd_win);
}

#اگر defined(EXITFREE)
    خالی
free_all_autocmds(خالی)
{
    char_u	*s;

    برائے (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(تہی, (char_u *)"", TRUE);

    برائے (صحیح i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_ڈیٹا))[i];
	اگر (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#اگر_ختم

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    صحیح
is_aucmd_win(win_T *win)
{
    برائے (صحیح i = 0; i < AUCMD_WIN_گنتی; ++i)
	اگر (aucmd_win[i].auc_win_مستعمل && aucmd_win[i].auc_win == win)
	    واپس TRUE;
    واپس FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    ساکن event_T
event_name2nr(char_u *آغاز, char_u **انتہا)
{
    char_u	*p;
    keyvalue_T ہدف;
    keyvalue_T *اندراج_2;
    ساکن keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_اشاریہ];
    ساکن keyvalue_T *bufread = &event_tab[BUFREAD_اشاریہ];

    // the event name ends with end of line, '|', a blank or a comma
    برائے (p = آغاز; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    ہدف.کلید = 0;
    ہدف.قدر.string = آغاز;
    ہدف.قدر.لمبائی = (size_t)(p - آغاز);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    اگر (cmp_keyvalue_قدر_ni(&ہدف, bufnewfile) == 0)
	اندراج_2 = bufnewfile;
    ورنہ اگر (cmp_keyvalue_قدر_ni(&ہدف, bufread) == 0)
	اندراج_2 = bufread;
    ورنہ
	اندراج_2 = (keyvalue_T *)bsearch(&ہدف, &event_tab, عدد_EVENTS,
				حجم(event_tab[0]), cmp_keyvalue_قدر_ni);

    اگر (*p == ',')
	++p;
    *انتہا = p;

    واپس (اندراج_2 == تہی) ? عدد_EVENTS : (event_T)مطلق_قدر(اندراج_2->کلید);
}

/*
 * Return the name for event "event".
 */
    ساکن string_T *
event_nr2name(event_T event)
{
    صحیح	    i;
    شمار {CACHE_سائز = 12};
    ساکن صحیح cache_tab[CACHE_سائز];
    ساکن صحیح cache_آخری_اشاریہ = -1;
    ساکن string_T unknown = STR_LITERAL_INIT("Unknown");

    اگر (cache_آخری_اشاریہ < 0)
    {
	برائے (i = 0; i < CACHE_سائز; ++i)
	    cache_tab[i] = -1;
	cache_آخری_اشاریہ = CACHE_سائز - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    برائے (i = cache_آخری_اشاریہ; cache_tab[i] >= 0; )
    {
	اگر ((event_T)مطلق_قدر(event_tab[cache_tab[i]].کلید) == event)
	    واپس &event_tab[cache_tab[i]].قدر;

	اگر (i == 0)
	    i = CACHE_سائز - 1;
	ورنہ
	    --i;

	// are we back at the start?
	اگر (i == cache_آخری_اشاریہ)
	    توڑو;
    }

    // look in the event table itself
    برائے (i = 0; i < عدد_EVENTS; ++i)
    {
	اگر ((event_T)مطلق_قدر(event_tab[i].کلید) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    اگر (cache_آخری_اشاریہ == CACHE_سائز - 1)
		cache_آخری_اشاریہ = 0;
	    ورنہ
		++cache_آخری_اشاریہ;
	    cache_tab[cache_آخری_اشاریہ] = i;
	    واپس &event_tab[i].قدر;
	}
    }

    واپس &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    ساکن char_u *
ڈھونڈو_انتہا_event(
    char_u  *arg,
    صحیح	    have_گروہ)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    اگر (*arg == '*')
    {
	اگر (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_ستارہ_str), arg);
	    واپس تہی;
	}
	pat = arg + 1;
    }
    ورنہ
    {
	برائے (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    اگر ((صحیح)event_name2nr(pat, &p) >= عدد_EVENTS)
	    {
		اگر (have_گروہ)
		    semsg(_(e_no_such_event_str), pat);
		ورنہ
		    semsg(_(e_no_such_گروہ_or_event_str), pat);
		واپس تہی;
	    }
	}
    }
    واپس pat;
}

/*
 * Find the command of an ":autocmd" with argument "arg", the part after the
 * events, the pattern and any "++once"/"++nested".  Returns NULL when there is
 * no command.  Does not modify "arg" and gives no error messages.
 */
    char_u *
au_ڈھونڈو_cmd_arg(char_u *arg)
{
    char_u	*pat;
    char_u	*cmd;
    صحیح		گروہ;

    اگر (*arg == '|')
	واپس تہی;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    گروہ = au_get_grouparg(&arg);
    pat = گروہ == AUGROUP_ERROR
			  ? تہی : ڈھونڈو_انتہا_event(arg, گروہ != AUGROUP_ALL);
    --emsg_off;
    اگر (pat == تہی)
	واپس تہی;

    pat = skipwhite(pat);
    اگر (*pat == NUL || *pat == '|')
	واپس تہی;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    جب (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    برائے (صحیح i = 0; i < 2; ++i)
    {
	اگر (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	اگر (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	اگر (!اندر_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    واپس *cmd == NUL ? تہی : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    صحیح
event_ignored(event_T event, char_u *ei)
{
    صحیح ignored = FALSE;
    جب (*ei != NUL)
    {
	صحیح unignore = *ei == '-';
	ei += unignore;
	اگر (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].کلید <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	ورنہ اگر (event_name2nr(ei, &ei) == event)
	{
	    اگر (unignore)
		واپس FALSE;
	    ورنہ
		ignored = TRUE;
	}
    }

    واپس ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    صحیح
check_ei(char_u *ei)
{
    صحیح	win = ei != p_ei;

    جب (*ei)
    {
	اگر (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	ورنہ
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    اگر (event == عدد_EVENTS || (win && event_tab[event].کلید > 0))
		واپس FAIL;
	}
    }

    واپس OK;
}

#اگر defined(FEAT_SYN_HL)

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
    اگر (save_ei == تہی)
	واپس تہی;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    اگر (new_ei == تہی)
    {
	vim_free(save_ei);
	واپس تہی;
    }

    اگر (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    ورنہ
	STRCPY(new_ei + p_ei_len, what);
    سیٹ_string_option_براہ_راست((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    واپس save_ei;
}

    خالی
au_event_restore(char_u *old_ei)
{
    اگر (old_ei != تہی)
    {
	سیٹ_string_option_براہ_راست((char_u *)"ei", -1, old_ei,
							  OPT_FREE, SID_NONE);
	vim_free(old_ei);
    }
}
#اگر_ختم  // FEAT_SYN_HL

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
    خالی
do_autocmd(exarg_T *eap, char_u *arg_اندر, صحیح forceit)
{
    char_u	*arg = arg_اندر;
    char_u	*pat;
    char_u	*envpat = تہی;
    char_u	*cmd;
    صحیح		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = تہی;
    صحیح		nested = FALSE;
    صحیح		once = FALSE;
    صحیح		گروہ;
    صحیح		i;
    صحیح		flags = 0;

    اگر (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	گروہ = AUGROUP_ALL;	// no argument, use all groups
    }
    ورنہ
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	گروہ = au_get_grouparg(&arg);
	اگر (arg == تہی)	    // out of memory
	    واپس;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = ڈھونڈو_انتہا_event(arg, گروہ != AUGROUP_ALL);
    اگر (pat == تہی)
	واپس;

    pat = skipwhite(pat);
    اگر (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    ورنہ
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	جب (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	اگر (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	اگر (vim_strchr(pat, '$') != تہی || vim_strchr(pat, '~') != تہی)
	{
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	    صحیح	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#اگر_ختم
	    envpat = expand_env_save(pat);
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	    p_ssl = p_ssl_save;
#اگر_ختم
	    اگر (envpat != تہی)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	برائے (i = 0; i < 2; i++)
	{
	    اگر (*cmd == NUL)
		جاری;

	    // Check for "++once" flag.
	    اگر (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		اگر (once)
		{
		    semsg(_(e_duplicate_دلیل_str), "++once");
		    جاؤ err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    اگر ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		اگر (nested)
		{
		    semsg(_(e_duplicate_دلیل_str), "++nested");
		    جاؤ err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    اگر (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		اگر (اندر_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_اوسط_plusplus_nested));
		    جاؤ err_exit;
		}
		اگر (nested)
		{
		    semsg(_(e_duplicate_دلیل_str), "nested");
		    جاؤ err_exit;
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
	    اگر (eap != تہی)
		// Read a {} block if it follows.
		cmd = may_get_cmd_بلاک(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    اگر (cmd == تہی)	    // some error
		جاؤ err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    اگر (!forceit && *cmd == NUL)
	// Highlight title
	پیغام_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    آخری_event = (event_T)-1;		// for listing the event name
    آخری_گروہ = AUGROUP_ERROR;		// for listing the group name
    اگر (*arg == '*' || *arg == NUL || *arg == '|')
    {
	اگر (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	ورنہ
	    برائے (event = (event_T)0; (صحیح)event < عدد_EVENTS;
					     event = (event_T)((صحیح)event + 1))
		اگر (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, گروہ, flags) == FAIL)
		    توڑو;
    }
    ورنہ
    {
	جب (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    اگر (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, گروہ, flags) == FAIL)
		توڑو;
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
    ساکن صحیح
au_get_grouparg(char_u **argp)
{
    char_u	*گروہ_name;
    char_u	*p;
    char_u	*arg = *argp;
    صحیح		گروہ = AUGROUP_ALL;

    برائے (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    اگر (p <= arg)
	واپس AUGROUP_ALL;

    گروہ_name = vim_strnsave(arg, p - arg);
    اگر (گروہ_name == تہی)		// out of memory
	واپس AUGROUP_ERROR;
    گروہ = au_ڈھونڈو_گروہ(گروہ_name);
    اگر (گروہ == AUGROUP_ERROR)
	گروہ = AUGROUP_ALL;	// no match, use all groups
    ورنہ
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(گروہ_name);
    واپس گروہ;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    ساکن صحیح
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    صحیح		once,
    صحیح		nested,
    char_u	*cmd,
    صحیح		forceit,
    صحیح		گروہ,
    صحیح		flags)
{
    AutoPat	*ap;
    AutoPat	**پچھلا_ap;
    AutoCmd	*ac;
    AutoCmd	**پچھلا_ac;
    صحیح		brace_level;
    char_u	*endpat;
    صحیح		findgroup;
    صحیح		allgroups;
    صحیح		patlen;
    صحیح		is_buflocal;
    صحیح		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    اگر (گروہ == AUGROUP_ALL)
	findgroup = current_augroup;
    ورنہ
	findgroup = گروہ;
    allgroups = (گروہ == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    اگر (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    اگر (forceit)  // delete the AutoPat, if it's in the current group
	    {
		اگر (ap->گروہ == findgroup)
		    au_remove_pat(ap);
	    }
	    ورنہ اگر (گروہ == AUGROUP_ALL || ap->گروہ == گروہ)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    برائے ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	برائے (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    اگر (*endpat == '{')
		brace_level++;
	    ورنہ اگر (*endpat == '}')
		brace_level--;
	}
	اگر (pat == endpat)		// ignore single comma
	    جاری;
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
	    ورنہ اگر (patlen > 9 && pat[7] == '=')
	    {
		اگر (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		ورنہ اگر (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((حرف *)pat + 8);
	    }
	}

	اگر (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((حرف *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (صحیح)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	اگر (!forceit && *cmd != NUL && آخری_autopat[(صحیح)event] != تہی)
	    پچھلا_ap = &آخری_autopat[(صحیح)event];
	ورنہ
	    پچھلا_ap = &پہلا_autopat[(صحیح)event];
	جب ((ap = *پچھلا_ap) != تہی)
	{
	    اگر (ap->pat != تہی)
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
		اگر ((allgroups || ap->گروہ == findgroup)
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
			اگر (*cmd != NUL && ap->اگلا == تہی)
			{
			    au_remove_cmds(ap);
			    توڑو;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    ورنہ اگر (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    ورنہ اگر (ap->اگلا == تہی)
			توڑو;
		}
	    }
	    پچھلا_ap = &ap->اگلا;
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
	    اگر (ap == تہی)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		اگر (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == تہی))
		{
		    semsg(_(e_بفر_nr_invalid_بفر_نمبر), buflocal_nr);
		    واپس FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		اگر (ap == تہی)
		    واپس FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		اگر (ap->pat == تہی)
		{
		    vim_free(ap);
		    واپس FAIL;
		}

#اگر_تعریف_شدہ FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		اگر (event == EVENT_MODECHANGED && !has_modechanged())
		    get_منوال(آخری_منوال);
#اگر_ختم
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
			snapshot_windows_scroll_سائز();
		    }
		    unuse_tabpage(curtab);
		    use_tabpage(save_curtab);
		}

		اگر (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = تہی;
		}
		ورنہ
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_مقصد_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    اگر (reg_pat != تہی)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    اگر (reg_pat == تہی || ap->reg_prog == تہی)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			واپس FAIL;
		    }
		}
		ap->cmds = تہی;
		*پچھلا_ap = ap;
		آخری_autopat[(صحیح)event] = ap;
		ap->اگلا = تہی;
		اگر (گروہ == AUGROUP_ALL)
		    ap->گروہ = current_augroup;
		ورنہ
		    ap->گروہ = گروہ;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    پچھلا_ac = &(ap->cmds);
	    جب ((ac = *پچھلا_ac) != تہی)
		پچھلا_ac = &ac->اگلا;
	    ac = ALLOC_ONE(AutoCmd);
	    اگر (ac == تہی)
		واپس FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    اگر (flags & UC_VIM9)
		ac->script_ctx.sc_نسخہ = SCRIPT_نسخہ_VIM9;
#اگر_تعریف_شدہ FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#اگر_ختم
	    اگر (ac->cmd == تہی)
	    {
		vim_free(ac);
		واپس FAIL;
	    }
	    ac->اگلا = تہی;
	    *پچھلا_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    واپس OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    صحیح
do_doautocmd(
    char_u	*arg_آغاز,
    صحیح		do_پیغام,	    // give message for no matching autocmds?
    صحیح		*did_something)
{
    char_u	*arg = arg_آغاز;
    char_u	*fname;
    صحیح		nothing_مکمل = TRUE;
    صحیح		گروہ;

    اگر (did_something != تہی)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    گروہ = au_get_grouparg(&arg);
    اگر (arg == تہی)	    // out of memory
	واپس FAIL;

    اگر (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	واپس FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = ڈھونڈو_انتہا_event(arg, گروہ != AUGROUP_ALL);
    اگر (fname == تہی)
	واپس FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    جب (*arg && !سرے_excmd(*arg) && !VIM_ISWHITE(*arg))
	اگر (apply_autocmds_گروہ(event_name2nr(arg, &arg),
				      fname, تہی, TRUE, گروہ, curbuf, تہی))
	    nothing_مکمل = FALSE;

    اگر (nothing_مکمل && do_پیغام
#اگر_تعریف_شدہ FEAT_EVAL
		&& !aborting()
#اگر_ختم
	       )
	smsg(_("No matching autocommands: %s"), arg_آغاز);
    اگر (did_something != تہی)
	*did_something = !nothing_مکمل;

#اگر_تعریف_شدہ FEAT_EVAL
    واپس aborting() ? FAIL : OK;
#ورنہ
    واپس OK;
#اگر_ختم
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    خالی
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
	اگر (buf->b_ml.ml_mfp == تہی || buf == curbuf)
	    جاری;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	اگر (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    توڑو;
	}

	سیٹ_bufref(&bufref, buf);

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
	    توڑو;
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
	واپس FALSE;
    }
    واپس TRUE;
}

/*
 * Prepare for executing autocommands for (hidden) buffer "buf".
 * Search for a visible window containing the current buffer.  If there isn't
 * one then use an entry in "aucmd_win[]".
 * Set "curbuf" and "curwin" to match "buf".
 * When this fails "curbuf" is not equal "buf".
 */
    خالی
aucmd_prepbuf(
    aco_save_T	*aco,		// structure to save values in
    buf_T	*buf)		// new curbuf
{
    win_T	*win;
    صحیح		save_ea;
#اگر_تعریف_شدہ FEAT_AUTOCHDIR
    صحیح		save_acd;
#اگر_ختم
    صحیح		same_بفر = buf == curbuf;

    // Find a window that is for the new buffer
    اگر (same_بفر)		// be quick when buf is curbuf
	win = curwin;
    ورنہ
	FOR_ALL_WINDOWS(win)
	    اگر (win->w_بفر == buf)
		توڑو;

    // Allocate a window when needed.
    win_T *auc_win = تہی;
    صحیح auc_idx = AUCMD_WIN_گنتی;
    اگر (win == تہی)
    {
	برائے (auc_idx = 0; auc_idx < AUCMD_WIN_گنتی; ++auc_idx)
	    اگر (!aucmd_win[auc_idx].auc_win_مستعمل)
	    {
		اگر (aucmd_win[auc_idx].auc_win == تہی)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		اگر (auc_win != تہی)
		    aucmd_win[auc_idx].auc_win_مستعمل = TRUE;
		توڑو;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	اگر (auc_win == تہی)
	    واپس;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == تہی ? 0 : prevwin->w_id;
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    اگر (bt_prompt(curbuf))
	aco->save_prompt_داخل_کرو = curbuf->b_prompt_داخل_کرو;
#اگر_ختم

    اگر (win != تہی)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    ورنہ
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
	curtab->tp_localdir = تہی;
	aco->globaldir = globaldir;
	globaldir = تہی;

	// Split the current window, put the auc_win in the upper half.
	// We don't want the BufEnter or WinEnter autocommands.
	بلاک_autocmds();
	make_snapshot(SNAP_AUCMD_IDX);
	save_ea = p_ea;
	p_ea = FALSE;

#اگر_تعریف_شدہ FEAT_AUTOCHDIR
	// Prevent chdir() call in win_enter_ext(), through do_autochdir().
	save_acd = p_acd;
	p_acd = FALSE;
#اگر_ختم

	(خالی)win_بانٹو_ins(0, WSP_چوٹی | WSP_FORCE_گنجائش, auc_win, 0, تہی);
	win_comp_pos();   // recompute window positions
	p_ea = save_ea;
#اگر_تعریف_شدہ FEAT_AUTOCHDIR
	p_acd = save_acd;
#اگر_ختم
	unblock_autocmds();
	curwin = auc_win;
    }
    curbuf = buf;
    aco->new_curwin_id = curwin->w_id;
    سیٹ_bufref(&aco->new_curbuf, curbuf);

    aco->save_VIsual_فعال = VIsual_فعال;
    اگر (!same_بفر)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_فعال = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    خالی
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
	بلاک_autocmds();
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
		    جاؤ win_ملا;
		}
	    }
	}
win_ملا:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(خالی)winframe_remove(curwin, &dummy, تہی, تہی);
	win_remove(curwin, تہی);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_مستعمل = FALSE;
	آخری_حالت(FALSE);	    // may need to remove last status line

	اگر (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_ڈھونڈو_by_id(aco->save_curwin_id);
	اگر (save_curwin != تہی)
	    curwin = save_curwin;
	ورنہ
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_بفر;
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_کھڑکی(curwin);
	اگر (bt_prompt(curbuf))
	    curbuf->b_prompt_داخل_کرو = aco->save_prompt_داخل_کرو;
#اگر_ختم
	prevwin = win_ڈھونڈو_by_id(aco->save_prevwin_id);
#اگر_تعریف_شدہ FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#اگر_ختم
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	اگر (awp->w_localdir != تہی)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_فعال = aco->save_VIsual_فعال;
	check_cursor();
	اگر (curwin->w_topline > curbuf->b_ml.ml_سطر_گنتی)
	{
	    curwin->w_topline = curbuf->b_ml.ml_سطر_گنتی;
#اگر_تعریف_شدہ FEAT_DIFF
	    curwin->w_topfill = 0;
#اگر_ختم
	}
#اگر defined(FEAT_GUI)
	اگر (gui.اندر_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_بایاں], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_دایاں], FALSE);
	    gui_may_update_scrollbars();
	}
#اگر_ختم
    }
    ورنہ
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_ڈھونڈو_by_id(aco->save_curwin_id);
	اگر (save_curwin != تہی)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    اگر (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != تہی)
	    {
#اگر defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		اگر (curwin->w_s == &curbuf->b_s)
		    curwin->w_s = &aco->new_curbuf.br_buf->b_s;
#اگر_ختم
		--curbuf->b_nwindows;
		curbuf = aco->new_curbuf.br_buf;
		curwin->w_بفر = curbuf;
		++curbuf->b_nwindows;
	    }

	    curwin = save_curwin;
	    curbuf = curwin->w_بفر;
	    prevwin = win_ڈھونڈو_by_id(aco->save_prevwin_id);

	    // In case the autocommand moves the cursor to a position that
	    // does not exist in curbuf.
	    VIsual_فعال = aco->save_VIsual_فعال;
	    check_cursor();
	}
    }

    VIsual_فعال = aco->save_VIsual_فعال;
    check_cursor();	    // just in case lines got deleted
    اگر (VIsual_فعال)
	check_pos(curbuf, &VIsual);
}

ساکن صحیح	autocmd_nested = FALSE;

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
    واپس apply_autocmds_گروہ(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, تہی);
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
    واپس apply_autocmds_گروہ(event, fname, fname_io, force,
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

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (should_abort(*retval))
	واپس FALSE;
#اگر_ختم

    did_cmd = apply_autocmds_گروہ(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, تہی);
    اگر (did_cmd
#اگر_تعریف_شدہ FEAT_EVAL
	    && aborting()
#اگر_ختم
	    )
	*retval = FAIL;
    واپس did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    ساکن صحیح
has_cursorhold(خالی)
{
    واپس (پہلا_autopat[(صحیح)(get_real_state() == منوال_معمول_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != تہی);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    صحیح
trigger_cursorhold(خالی)
{
    صحیح		state;

    اگر (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_فعال())
    {
	state = get_real_state();
	اگر (state == منوال_معمول_BUSY || (state & منوال_داخل_کرو) != 0)
	    واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    صحیح
has_winresized(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_WINRESIZED] != تہی);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    صحیح
has_winscrolled(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_WINSCROLLED] != تہی);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    صحیح
has_cursormoved(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_CURSORMOVED] != تہی);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    صحیح
has_cursormovedI(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_CURSORMOVEDI] != تہی);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    صحیح
has_textchanged(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_TEXTCHANGED] != تہی);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    صحیح
has_textchangedI(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_TEXTCHANGEDI] != تہی);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    صحیح
has_textchangedP(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_TEXTCHANGEDP] != تہی);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    صحیح
has_insertcharpre(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_INSERTCHARPRE] != تہی);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    صحیح
has_keyinputpre(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_KEYINPUTPRE] != تہی);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    صحیح
has_cmdundefined(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_CMDUNDEFINED] != تہی);
}

#اگر defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    صحیح
has_textputpost(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_TEXTPUTPOST] != تہی);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    صحیح
has_textputpre(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_TEXTPUTPRE] != تہی);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    صحیح
has_textyankpost(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_TEXTYANKPOST] != تہی);
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    صحیح
has_completechanged(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_COMPLETECHANGED] != تہی);
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    صحیح
has_modechanged(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_MODECHANGED] != تہی);
}
#اگر_ختم

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    ساکن صحیح
apply_autocmds_گروہ(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    صحیح		force,	     // when TRUE, ignore autocmd_busy
    صحیح		گروہ,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap غیر_مستعمل) // command arguments
{
    char_u	*sfname = تہی;	// short file name
    char_u	*دم;
    صحیح		save_بدلا;
    buf_T	*old_curbuf;
    صحیح		retval = FALSE;
    char_u	*save_autocmd_fname;
    صحیح		save_autocmd_fname_بھرا;
    صحیح		save_autocmd_bufnr;
    char_u	*save_autocmd_مطابقت;
    صحیح		save_autocmd_busy;
    صحیح		save_autocmd_nested;
    ساکن صحیح	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#اگر_تعریف_شدہ FEAT_EVAL
    funccal_اندراج_T funccal_اندراج;
    char_u	*save_cmdarg;
    طویل	save_cmdbang;
#اگر_ختم
    ساکن صحیح	filechangeshell_busy = FALSE;
#اگر_تعریف_شدہ FEAT_PROFILE
    proftime_T	wait_وقت;
#اگر_ختم
    صحیح		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    صحیح		save_کلید_Typed = کلید_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    اگر (event == عدد_EVENTS || پہلا_autopat[(صحیح)event] == تہی
	    || autocmd_blocked > 0)
	جاؤ BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    اگر (autocmd_busy && !(force || autocmd_nested))
	جاؤ BYPASS_AU;

#اگر_تعریف_شدہ FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    اگر (aborting())
	جاؤ BYPASS_AU;
#اگر_ختم

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    اگر (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	جاؤ BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    اگر (event_ignored(event, p_ei))
	جاؤ BYPASS_AU;

    صحیح win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    اگر (buf == curbuf && event_tab[event].کلید <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    ورنہ اگر (buf != تہی && event_tab[event].کلید <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    اگر (wp->w_بفر == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		توڑو;
	    }
    }
    اگر (win_ignore)
	جاؤ BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    اگر (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	جاؤ BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    اگر (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	جاؤ BYPASS_AU;

    اگر (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_بدلا_گنتی;

    /*
     * Save the autocmd_* variables and info about the current buffer.
     */
    save_autocmd_fname = autocmd_fname;
    save_autocmd_fname_بھرا = autocmd_fname_بھرا;
    save_autocmd_bufnr = autocmd_bufnr;
    save_autocmd_مطابقت = autocmd_مطابقت;
    save_autocmd_busy = autocmd_busy;
    save_autocmd_nested = autocmd_nested;
    save_بدلا = curbuf->b_بدلا;
    old_curbuf = curbuf;

    /*
     * Set the file name to be used for <afile>.
     * Make a copy to avoid that changing a buffer name or directory makes it
     * invalid.
     */
    اگر (fname_io == تہی)
    {
	اگر (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = تہی;
	ورنہ اگر (fname != تہی && !سرے_excmd(*fname))
	    autocmd_fname = fname;
	ورنہ اگر (buf != تہی)
	    autocmd_fname = buf->b_ffname;
	ورنہ
	    autocmd_fname = تہی;
    }
    ورنہ
	autocmd_fname = fname_io;
    اگر (autocmd_fname != تہی)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_بھرا = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    اگر (buf == تہی)
	autocmd_bufnr = 0;
    ورنہ
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    اگر (fname == تہی || *fname == NUL)
    {
	اگر (buf == تہی)
	    fname = تہی;
	ورنہ
	{
#اگر_تعریف_شدہ FEAT_SYN_HL
	    اگر (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    ورنہ
#اگر_ختم
		اگر (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		ورنہ
		{
		    اگر (buf->b_sfname != تہی)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	اگر (fname == تہی)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    ورنہ
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
	    autocmd_fname_بھرا = TRUE; // don't expand it later
	}
	ورنہ
	    fname = بھرا_Name_save(fname, FALSE);
    }
    اگر (fname == تہی)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	جاؤ BYPASS_AU;
    }

#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    اگر (sfname != تہی)
	forward_slash(sfname);
    forward_slash(fname);
#اگر_ختم

#اگر_تعریف_شدہ VMS
    // remove version for correct match
    اگر (sfname != تہی)
	vms_remove_نسخہ(sfname);
    vms_remove_نسخہ(fname);
#اگر_ختم

    /*
     * Set the name to be used for <amatch>.
     */
    autocmd_مطابقت = fname;


    // Don't redraw while doing autocommands.
    ++RedrawingDisabled;

    // name and lnum are filled in later
    estack_ڈالو(ETYPE_AUCMD, تہی, 0);
    ESTACK_CHECK_تیاری;

    save_current_sctx = current_sctx;

#اگر_تعریف_شدہ FEAT_EVAL
# ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	prof_بچہ_enter(&wait_وقت); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_اندراج);
#اگر_ختم

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    اگر (!autocmd_busy)
    {
	save_تلاش_patterns();
	اگر (!ins_compl_فعال())
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

    دم = gettail(fname);

    // Find first autocommand that matches
    CLEAR_میدان(patcmd);
    patcmd.curpat = پہلا_autopat[(صحیح)event];
    patcmd.گروہ = گروہ;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.دم = دم;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_اگلا_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    اگر (patcmd.curpat != تہی)
    {
	// add to active_apc_list
	patcmd.اگلا = فعال_apc_list;
	فعال_apc_list = &patcmd;

#اگر_تعریف_شدہ FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (طویل)get_vim_var_nr(VV_CMDBANG);
	اگر (eap != تہی)
	{
	    save_cmdarg = سیٹ_cmdarg(eap, تہی);
	    سیٹ_vim_var_nr(VV_CMDBANG, (طویل)eap->forceit);
	}
	ورنہ
	    save_cmdarg = تہی;	// avoid gcc warning
#اگر_ختم
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	برائے (ap = patcmd.curpat; ap->اگلا != تہی; ap = ap->اگلا)
	    ap->آخری = FALSE;
	ap->آخری = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	اگر (nesting == 1)
	    check_lnums(TRUE);
	ورنہ
	    check_lnums_nested(TRUE);

	صحیح save_did_emsg = did_emsg;
	صحیح save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(تہی, getnextac, (خالی *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	سیٹ_pressedreturn(save_ex_pressedreturn);

	اگر (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#اگر_تعریف_شدہ FEAT_EVAL
	اگر (eap != تہی)
	{
	    (خالی)سیٹ_cmdarg(تہی, save_cmdarg);
	    سیٹ_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#اگر_ختم
	// delete from active_apc_list
	اگر (فعال_apc_list == &patcmd)	    // just in case
	    فعال_apc_list = patcmd.اگلا;
    }

    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_نکالو();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_بھرا = save_autocmd_fname_بھرا;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_مطابقت = save_autocmd_مطابقت;
    current_sctx = save_current_sctx;
#اگر_تعریف_شدہ FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	prof_بچہ_exit(&wait_وقت);
# endif
#اگر_ختم
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
	restore_تلاش_patterns();
	اگر (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	جب (au_pending_free_buf != تہی)
	{
	    buf_T *b = au_pending_free_buf->b_اگلا;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	جب (au_pending_free_win != تہی)
	{
	    win_T *w = au_pending_free_win->w_اگلا;

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
	اگر (curbuf->b_بدلا != save_بدلا)
	    need_maketitle = TRUE;
	curbuf->b_بدلا = save_بدلا;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    اگر (event == EVENT_BUFWIPEOUT && buf != تہی)
	aubuflocal_remove(buf);

    اگر (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    واپس retval;
}

#اگر_تعریف_شدہ FEAT_EVAL
ساکن char_u	*old_termresponse = تہی;
ساکن char_u	*old_termu7resp = تہی;
ساکن char_u	*old_termblinkresp = تہی;
ساکن char_u	*old_termrbgresp = تہی;
ساکن char_u	*old_termrfgresp = تہی;
ساکن char_u	*old_termstyleresp = تہی;
#اگر_ختم

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    خالی
بلاک_autocmds(خالی)
{
#اگر_تعریف_شدہ FEAT_EVAL
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
#اگر_ختم
    ++autocmd_blocked;
}

    خالی
unblock_autocmds(خالی)
{
    --autocmd_blocked;

#اگر_تعریف_شدہ FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    اگر (autocmd_blocked == 0)
    {
	اگر (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, تہی, تہی, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", تہی, FALSE, curbuf);
	}
	اگر (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", تہی, FALSE, curbuf);
	}
	اگر (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", تہی, FALSE, curbuf);
	}
	اگر (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", تہی, FALSE, curbuf);
	}
	اگر (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", تہی, FALSE, curbuf);
	}
	اگر (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", تہی, FALSE, curbuf);
	}
    }
#اگر_ختم
}

    صحیح
is_autocmd_blocked(خالی)
{
    واپس autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    ساکن خالی
auto_اگلا_pat(
    AutoPatCmd_T *apc,
    صحیح		stop_at_آخری)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*اندراج_2;

    اندراج_2 = ((estack_T *)exestack.ga_ڈیٹا) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(اندراج_2->es_name);
    اندراج_2->es_info.aucmd = تہی;

    برائے (ap = apc->curpat; ap != تہی && !got_int; ap = ap->اگلا)
    {
	apc->curpat = تہی;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	اگر (ap->pat != تہی && ap->cmds != تہی
		&& (apc->گروہ == AUGROUP_ALL || apc->گروہ == ap->گروہ))
	{
	    // execution-condition
	    اگر (ap->buflocal_nr == 0
		    ? (مطابقت_file_pat(تہی, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->دم, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		حرف	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->لمبائی + ap->patlen + 1);
		اگر (namep != تہی)
		{
		    sprintf((حرف *)namep, fmt, (حرف *)event_name->string, (حرف *)ap->pat);
		    اگر (p_verbose >= 8)
		    {
			verbose_enter();
			smsg(_("Executing %s"), namep);
			verbose_leave();
		    }
		}

		// Update the exestack entry for this autocmd.
		اندراج_2->es_name = namep;
		اندراج_2->es_info.aucmd = apc;

		apc->curpat = ap;
		apc->nextcmd = ap->cmds;
		// mark last command
		برائے (cp = ap->cmds; cp->اگلا != تہی; cp = cp->اگلا)
		    cp->آخری = FALSE;
		cp->آخری = TRUE;
	    }
	    سطر_breakcheck();
	    اگر (apc->curpat != تہی)	    // found a match
		توڑو;
	}
	اگر (stop_at_آخری && ap->آخری)
	    توڑو;
    }
}

#اگر defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    واپس &acp->script_ctx;
}
#اگر_ختم

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	صحیح c غیر_مستعمل,
	خالی *cookie,
	صحیح indent غیر_مستعمل,
	getline_opt_T options غیر_مستعمل)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    اگر (acp->curpat == تہی)
	واپس تہی;

    // repeat until we find an autocommand to execute
    برائے (;;)
    {
	// skip removed commands
	جب (acp->nextcmd != تہی && acp->nextcmd->cmd == تہی)
	    اگر (acp->nextcmd->آخری)
		acp->nextcmd = تہی;
	    ورنہ
		acp->nextcmd = acp->nextcmd->اگلا;

	اگر (acp->nextcmd != تہی)
	    توڑو;

	// at end of commands, find next pattern that matches
	اگر (acp->curpat->آخری)
	    acp->curpat = تہی;
	ورنہ
	    acp->curpat = acp->curpat->اگلا;
	اگر (acp->curpat != تہی)
	    auto_اگلا_pat(acp, TRUE);
	اگر (acp->curpat == تہی)
	    واپس تہی;
    }

    ac = acp->nextcmd;

    اگر (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	پیغام_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    اگر (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    اگر (ac->آخری)
	acp->nextcmd = تہی;
    ورنہ
	acp->nextcmd = ac->اگلا;
    واپس retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    ساکن صحیح
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_بھرا)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*دم = gettail(sfname);
    صحیح		retval = FALSE;

    اگر (پہلا_autopat[(صحیح)event] == تہی)
	واپس FALSE;

    fname = is_بھرا ? vim_strsave(sfname) : بھرا_Name_save(sfname, FALSE);
    اگر (fname == تہی)
	واپس FALSE;

#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    اگر (sfname != تہی)
	forward_slash(sfname);
    forward_slash(fname);
#اگر_ختم

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	اگر (ap->pat != تہی && ap->cmds != تہی
	      && (ap->buflocal_nr == 0
		? مطابقت_file_pat(تہی, &ap->reg_prog,
					  fname, sfname, دم, ap->allow_dirs)
		: buf != تہی && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    توڑو;
	}

    vim_free(fname);
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    vim_free(sfname);
#اگر_ختم

    واپس retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    صحیح
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    واپس has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    صحیح
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    واپس has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx == augroups.ga_len)		// add "END" add the end
	واپس (char_u *)"END";
    اگر (idx < 0 || idx >= augroups.ga_len)	// end of list
	واپس تہی;
    اگر (AUGROUP_NAME(idx) == تہی || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	واپس (char_u *)"";
    واپس AUGROUP_NAME(idx);		// return a name
}

ساکن صحیح include_groups = FALSE;

    char_u  *
سیٹ_context_اندر_autocmd(
    expand_T	*xp,
    char_u	*arg,
    صحیح		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    صحیح		گروہ;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    گروہ = au_get_grouparg(&arg);
    اگر (گروہ == AUGROUP_ERROR)
	واپس تہی;
    // If there only is a group name that's what we expand.
    اگر (*arg == NUL && گروہ != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	گروہ = AUGROUP_ALL;
    }

    // skip over event name
    برائے (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	اگر (*p == ',')
	    arg = p + 1;
    اگر (*p == NUL)
    {
	اگر (گروہ == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_نمونہ = arg;
	واپس تہی;
    }

    // skip over pattern
    arg = skipwhite(p);
    جب (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    اگر (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	برائے (صحیح i = 0; i < 2; i++)
	{
	    اگر (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    اگر (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    اگر (!اندر_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	واپس arg;			    // expand (next) command
    }

    اگر (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    ورنہ
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    واپس تہی;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    صحیح i;

    اگر (idx < augroups.ga_len)		// First list group names, if wanted
    {
	اگر (!include_groups || AUGROUP_NAME(idx) == تہی
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    واپس (char_u *)"";	// skip deleted entries
	واپس AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    اگر (i < 0 || i >= عدد_EVENTS)
	واپس تہی;

    واپس event_tab[i].قدر.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_گروہ(expand_T *xp غیر_مستعمل, صحیح idx, صحیح win)
{
    اگر (idx < 0 || idx >= عدد_EVENTS)
	واپس تہی;

    اگر (!win)
	واپس event_tab[idx].قدر.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    صحیح j = 0;
    برائے (صحیح i = 0; i < عدد_EVENTS; ++i)
    {
	j += event_tab[i].کلید <= 0;
	اگر (j == idx + 1)
	    واپس event_tab[i].قدر.string;
    }

    واپس تہی;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    صحیح
has_tabclosedpre(خالی)
{
    واپس (پہلا_autopat[(صحیح)EVENT_TABCLOSEDPRE] != تہی);
}

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    صحیح
autocmd_supported(char_u *name)
{
    char_u *p;

    واپس (event_name2nr(name, &p) != عدد_EVENTS);
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
    char_u	*نمونہ = تہی;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = تہی;
    صحیح		گروہ;
    صحیح		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    اگر (arg_save == تہی)
	واپس FALSE;
    p = vim_strchr(arg_save, '#');
    اگر (p != تہی)
	*p++ = NUL;

    // First, look for an autocmd group name
    گروہ = au_ڈھونڈو_گروہ(arg_save);
    اگر (گروہ == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	گروہ = AUGROUP_ALL;
	event_name = arg_save;
    }
    ورنہ
    {
	اگر (p == تہی)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    جاؤ theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	اگر (p != تہی)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    نمونہ = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    اگر (event == عدد_EVENTS)
	جاؤ theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = پہلا_autopat[(صحیح)event];
    اگر (ap == تہی)
	جاؤ theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    اگر (نمونہ != تہی && STRICMP(نمونہ, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    برائے ( ; ap != تہی; ap = ap->اگلا)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	اگر (ap->pat != تہی && ap->cmds != تہی
	    && (گروہ == AUGROUP_ALL || ap->گروہ == گروہ)
	    && (نمونہ == تہی
		|| (buflocal_buf == تہی
		    ? fnamecmp(ap->pat, نمونہ) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    توڑو;
	}

theend:
    vim_free(arg_save);
    واپس retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    ساکن خالی
autocmd_جوڑو_or_delete(typval_T *argvars, typval_T *rettv, صحیح delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = تہی;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*گروہ_name = تہی;
    صحیح		گروہ;
    char_u	*pat = تہی;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = تہی;
    char_u	*انتہا;
    صحیح		once;
    صحیح		nested;
    صحیح		replace;		// replace the cmd for a group/event
    صحیح		retval = VVAL_TRUE;
    صحیح		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_نمبر = VVAL_FALSE;

    اگر (check_restricted() || check_secure())
	واپس;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
	واپس;

    aucmd_list = argvars[0].vval.v_list;
    اگر (aucmd_list == تہی)
	واپس;

    FOR_ALL_LIST_اشیاء(aucmd_list, li)
    {
	VIM_CLEAR(گروہ_name);
	VIM_CLEAR(cmd);
	event_name = تہی;
	event_list = تہی;
	pat = تہی;
	pat_list = تہی;

	اگر (li->li_tv.v_type != VAR_DICT)
	    جاری;

	event_dict = li->li_tv.vval.v_dict;
	اگر (event_dict == تہی)
	    جاری;

	di = dict_ڈھونڈو(event_dict, (char_u *)"event", -1);
	اگر (di != تہی)
	{
	    اگر (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		اگر (event_name == تہی)
		{
		    emsg(_(e_string_required));
		    جاری;
		}
	    }
	    ورنہ اگر (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		اگر (event_list == تہی)
		{
		    emsg(_(e_list_required));
		    جاری;
		}
	    }
	    ورنہ
	    {
		emsg(_(e_string_or_list_expected));
		جاری;
	    }
	}

	گروہ_name = dict_get_string(event_dict, "group", TRUE);
	اگر (گروہ_name == تہی || *گروہ_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    گروہ = current_augroup;
	ورنہ
	{
	    گروہ = au_ڈھونڈو_گروہ(گروہ_name);
	    اگر (گروہ == AUGROUP_ERROR)
	    {
		اگر (delete)
		{
		    semsg(_(e_no_such_گروہ_str), گروہ_name);
		    retval = VVAL_FALSE;
		    توڑو;
		}
		// group is not found, create it now
		گروہ = au_new_گروہ(گروہ_name);
		اگر (گروہ == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_گروہ_str), گروہ_name);
		    retval = VVAL_FALSE;
		    توڑو;
		}

		current_augroup = گروہ;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	اگر (dict_has_کلید(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_نمبر_def(event_dict, "bufnr", -1);
	    اگر (bnum == -1)
		جاری;

	    vim_snprintf((حرف *)IObuff, IOSIZE, "<buffer=%d>", (صحیح)bnum);
	    pat = IObuff;
	}
	ورنہ
	{
	    di = dict_ڈھونڈو(event_dict, (char_u *)"pattern", -1);
	    اگر (di != تہی)
	    {
		اگر (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    اگر (pat == تہی)
		    {
			emsg(_(e_string_required));
			جاری;
		    }
		}
		ورنہ اگر (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    اگر (pat_list == تہی)
		    {
			emsg(_(e_list_required));
			جاری;
		    }
		}
		ورنہ
		{
		    emsg(_(e_string_or_list_expected));
		    جاری;
		}
	    }
	    ورنہ اگر (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	اگر (cmd == تہی)
	{
	    اگر (delete)
		cmd = vim_strsave((char_u *)"");
	    ورنہ
		جاری;
	}

	اگر (delete && (event_name == تہی
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    برائے (event = (event_T)0; (صحیح)event < عدد_EVENTS;
		    event = (event_T)((صحیح)event + 1))
	    {
		اگر (do_autocmd_event(event, pat, once, nested, cmd, delete,
							گروہ, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    توڑو;
		}
	    }
	}
	ورنہ
	{
	    char_u *p = تہی;

	    eli = تہی;
	    انتہا = تہی;
	    جب (TRUE)
	    {
		اگر (event_list != تہی)
		{
		    اگر (eli == تہی)
			eli = event_list->lv_پہلا;
		    ورنہ
			eli = eli->li_اگلا;
		    اگر (eli == تہی)
			توڑو;
		    اگر (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == تہی)
		    {
			emsg(_(e_string_required));
			توڑو;
		    }
		}
		ورنہ
		{
		    اگر (p == تہی)
			p = event_name;
		    اگر (p == تہی || *p == NUL)
			توڑو;
		}

		event = event_name2nr(p, &انتہا);
		اگر (event == عدد_EVENTS || *انتہا != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    توڑو;
		}
		اگر (pat != تہی)
		{
		    اگر (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, گروہ, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			توڑو;
		    }
		}
		ورنہ اگر (pat_list != تہی)
		{
		    FOR_ALL_LIST_اشیاء(pat_list, pli)
		    {
			اگر (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == تہی)
			{
			    emsg(_(e_string_required));
			    جاری;
			}
			اگر (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, گروہ, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    توڑو;
			}
		    }
		    اگر (retval == VVAL_FALSE)
			توڑو;
		}
		اگر (event_name != تہی)
		    p = انتہا;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	اگر (delete && گروہ_name != تہی &&
		(event_name == تہی || event_name[0] == NUL)
		&& (pat == تہی || pat[0] == NUL)
		&& (cmd == تہی || cmd[0] == NUL))
	    au_del_گروہ(گروہ_name);
    }

    VIM_CLEAR(گروہ_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_نمبر = retval;
}

/*
 * autocmd_add() function
 */
    خالی
f_autocmd_جوڑو(typval_T *argvars, typval_T *rettv)
{
    autocmd_جوڑو_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    خالی
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_جوڑو_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    خالی
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = عدد_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = تہی;
    char_u	*name = تہی;
    صحیح		گروہ = AUGROUP_ALL;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;
    اگر (check_for_opt_dict_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	اگر (dict_has_کلید(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    اگر (name == تہی)
		واپس;

	    اگر (*name == NUL)
		گروہ = AUGROUP_DEFAULT;
	    ورنہ
	    {
		گروہ = au_ڈھونڈو_گروہ(name);
		اگر (گروہ == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_گروہ_str), name);
		    vim_free(name);
		    واپس;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	اگر (dict_has_کلید(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    اگر (name == تہی)
		واپس;

	    اگر (name[0] == '*' && name[1] == NUL)
		event_arg = عدد_EVENTS;
	    ورنہ
	    {
		keyvalue_T ہدف;
		keyvalue_T *اندراج_2;

		ہدف.کلید = 0;
		ہدف.قدر.string = name;
		ہدف.قدر.لمبائی = STRLEN(ہدف.قدر.string);
		اندراج_2 = (keyvalue_T *)bsearch(&ہدف, &event_tab,
		    عدد_EVENTS, حجم(event_tab[0]), cmp_keyvalue_قدر_ni);
		اگر (اندراج_2 == تہی)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    واپس;
		}
		event_arg = (event_T)مطلق_قدر(اندراج_2->کلید);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	اگر (dict_has_کلید(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    اگر (pat == تہی)
		واپس;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    برائے (event = (event_T)0; (صحیح)event < عدد_EVENTS;
	    event = (event_T)((صحیح)event + 1))
    {
	string_T    *event_name;

	اگر (event_arg != عدد_EVENTS && event != event_arg)
	    جاری;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	گروہ_name;

	    اگر (ap->pat == تہی)		// pattern has been removed
		جاری;

	    اگر (گروہ != AUGROUP_ALL && گروہ != ap->گروہ)
		جاری;

	    اگر (pat != تہی && STRCMP(pat, ap->pat) != 0)
		جاری;

	    گروہ_name.string = get_augroup_name(تہی, ap->گروہ);
	    اگر (گروہ_name.string == تہی)
		STR_LITERAL_سیٹ(گروہ_name, "");
	    ورنہ
		گروہ_name.لمبائی = STRLEN(گروہ_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    برائے (ac = ap->cmds; ac != تہی; ac = ac->اگلا)
	    {
		event_dict = dict_alloc();
		اگر (event_dict == تہی
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    واپس;
		}

		اگر (dict_جوڑو_string_len(event_dict, "event",
			event_name->string, (صحیح)event_name->لمبائی) == FAIL
			|| dict_جوڑو_string_len(event_dict, "group",
			    گروہ_name.string, (صحیح)گروہ_name.لمبائی) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_جوڑو_نمبر(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_جوڑو_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_جوڑو_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_جوڑو_bool(event_dict, "once", ac->once) == FAIL
			|| dict_جوڑو_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    واپس;
		}
	    }
	}
    }

    vim_free(pat);
}

#اگر_ختم
