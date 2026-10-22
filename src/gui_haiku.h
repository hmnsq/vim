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

ساختار BMenu;
ساختار BMenuItem;
ساختار BPictureButton;

// Our own Vim-related classes

ساختار VimApp;
ساختار VimFormView;
ساختار VimTextAreaView;
ساختار VimWindow;
ساختار VimScrollBar;

// Locking functions

بیرونی صحیح vim_lock_screen();
بیرونی تهی vim_unlock_screen();

#ifndef __cplusplus

تعریفنوع ساختار BMenu BMenu;
تعریفنوع ساختار BMenuItem BMenuItem;
تعریفنوع ساختار BPictureButton BPictureButton;
تعریفنوع ساختار VimWindow VimWindow;
تعریفنوع ساختار VimFormView VimFormView;
تعریفنوع ساختار VimTextAreaView VimTextAreaView;
تعریفنوع ساختار VimApp VimApp;
تعریفنوع ساختار VimScrollBar VimScrollBar;

#endif
