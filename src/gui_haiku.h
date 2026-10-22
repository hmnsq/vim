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

কাঠামো BMenu;
কাঠামো BMenuItem;
কাঠামো BPictureButton;

// Our own Vim-related classes

কাঠামো VimApp;
কাঠামো VimFormView;
কাঠামো VimTextAreaView;
কাঠামো VimWindow;
কাঠামো VimScrollBar;

// Locking functions

বাহ্যিক পূর্ণসংখ্যা vim_lock_screen();
বাহ্যিক শূন্য vim_unlock_screen();

#ifndef __cplusplus

প্রকারলেখ কাঠামো BMenu BMenu;
প্রকারলেখ কাঠামো BMenuItem BMenuItem;
প্রকারলেখ কাঠামো BPictureButton BPictureButton;
প্রকারলেখ কাঠামো VimWindow VimWindow;
প্রকারলেখ কাঠামো VimFormView VimFormView;
প্রকারলেখ কাঠামো VimTextAreaView VimTextAreaView;
প্রকারলেখ কাঠামো VimApp VimApp;
প্রকারলেখ কাঠামো VimScrollBar VimScrollBar;

#endif
