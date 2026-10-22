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

ಸಂರಚನಾ BMenu;
ಸಂರಚನಾ BMenuItem;
ಸಂರಚನಾ BPictureButton;

// Our own Vim-related classes

ಸಂರಚನಾ VimApp;
ಸಂರಚನಾ VimFormView;
ಸಂರಚನಾ VimTextAreaView;
ಸಂರಚನಾ VimWindow;
ಸಂರಚನಾ VimScrollBar;

// Locking functions

ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ vim_lock_screen();
ಬಾಹ್ಯ ಶೂನ್ಯ vim_unlock_screen();

#ifndef __cplusplus

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ BMenu BMenu;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ BMenuItem BMenuItem;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ BPictureButton BPictureButton;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ VimWindow VimWindow;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ VimFormView VimFormView;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ VimTextAreaView VimTextAreaView;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ VimApp VimApp;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ VimScrollBar VimScrollBar;

#endif
