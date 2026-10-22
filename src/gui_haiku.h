/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI support by Olaf "Rhialto" Seibert
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 *
 * Haiku GUI.
 *
 * Based on "GUI support for the Buzzword Enhanced Operating System for PPC."
 *
 */

/*
 * This file must be acceptable both as C and C++.
 * The BeOS API is defined in terms of C++, but some classes
 * should be somewhat known in the common C code.
 */

// System classes

sa_nracanaa BMenu;
sa_nracanaa BMenuItem;
sa_nracanaa BPictureButton;

// Our own Vim-related classes

sa_nracanaa VimApp;
sa_nracanaa VimFormView;
sa_nracanaa VimTextAreaView;
sa_nracanaa VimWindow;
sa_nracanaa VimScrollBar;

// Locking functions

extern จำนวนเต็ม vim_lock_screen();
extern void vim_unlock_screen();

#ifndef __cplusplus

typedef sa_nracanaa BMenu BMenu;
typedef sa_nracanaa BMenuItem BMenuItem;
typedef sa_nracanaa BPictureButton BPictureButton;
typedef sa_nracanaa VimWindow VimWindow;
typedef sa_nracanaa VimFormView VimFormView;
typedef sa_nracanaa VimTextAreaView VimTextAreaView;
typedef sa_nracanaa VimApp VimApp;
typedef sa_nracanaa VimScrollBar VimScrollBar;

#endif
