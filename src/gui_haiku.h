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

ساخت BMenu;
ساخت BMenu_شے;
ساخت BPictureButton;

// Our own Vim-related classes

ساخت VimApp;
ساخت VimFormView;
ساخت Vim_متن_Area_View;
ساخت Vim_کھڑکی;
ساخت VimScrollBar;

// Locking functions

خارجی صحیح vim_تالا_screen();
خارجی خالی vim_unlock_screen();

#اگر_غیر_تعریف_شدہ __cplusplus

قسمتعریف ساخت BMenu BMenu;
قسمتعریف ساخت BMenu_شے BMenu_شے;
قسمتعریف ساخت BPictureButton BPictureButton;
قسمتعریف ساخت Vim_کھڑکی Vim_کھڑکی;
قسمتعریف ساخت VimFormView VimFormView;
قسمتعریف ساخت Vim_متن_Area_View Vim_متن_Area_View;
قسمتعریف ساخت VimApp VimApp;
قسمتعریف ساخت VimScrollBar VimScrollBar;

#اگر_ختم
