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

ഘടന BMenu;
ഘടന BMenuItem;
ഘടന BPictureButton;

// Our own Vim-related classes

ഘടന VimApp;
ഘടന VimFormView;
ഘടന VimTextAreaView;
ഘടന VimWindow;
ഘടന VimScrollBar;

// Locking functions

ബാഹ്യ പൂർണ്ണസംഖ്യ vim_lock_screen();
ബാഹ്യ ശൂന്യം vim_unlock_screen();

#ifndef __cplusplus

തരനിർവചനം ഘടന BMenu BMenu;
തരനിർവചനം ഘടന BMenuItem BMenuItem;
തരനിർവചനം ഘടന BPictureButton BPictureButton;
തരനിർവചനം ഘടന VimWindow VimWindow;
തരനിർവചനം ഘടന VimFormView VimFormView;
തരനിർവചനം ഘടന VimTextAreaView VimTextAreaView;
തരനിർവചനം ഘടന VimApp VimApp;
തരനിർവചനം ഘടന VimScrollBar VimScrollBar;

#endif
