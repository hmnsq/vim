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

بنية BMenu;
بنية BMenu_قلم;
بنية BPictureButton;

// Our own Vim-related classes

بنية VimApp;
بنية VimFormView;
بنية Vim_متن_Area_View;
بنية Vim_پنجره;
بنية VimScrollBar;

// Locking functions

خارجي صحيح vim_قفل_screen();
خارجي فراغ vim_unlock_screen();

#اگر_تعریف_نشده __cplusplus

تعريفنوع بنية BMenu BMenu;
تعريفنوع بنية BMenu_قلم BMenu_قلم;
تعريفنوع بنية BPictureButton BPictureButton;
تعريفنوع بنية Vim_پنجره Vim_پنجره;
تعريفنوع بنية VimFormView VimFormView;
تعريفنوع بنية Vim_متن_Area_View Vim_متن_Area_View;
تعريفنوع بنية VimApp VimApp;
تعريفنوع بنية VimScrollBar VimScrollBar;

#پایان_اگر
