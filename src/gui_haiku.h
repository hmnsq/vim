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
بنية BMenu_توکی;
بنية BPictureButton;

// Our own Vim-related classes

بنية VimApp;
بنية VimFormView;
بنية Vim_متن_Area_View;
بنية Vim_کړکۍ;
بنية VimScrollBar;

// Locking functions

خارجي صحيح vim_قلف_screen();
خارجي فراغ vim_unlock_screen();

#که_نه_تعریف_شوی __cplusplus

تعريفنوع بنية BMenu BMenu;
تعريفنوع بنية BMenu_توکی BMenu_توکی;
تعريفنوع بنية BPictureButton BPictureButton;
تعريفنوع بنية Vim_کړکۍ Vim_کړکۍ;
تعريفنوع بنية VimFormView VimFormView;
تعريفنوع بنية Vim_متن_Area_View Vim_متن_Area_View;
تعريفنوع بنية VimApp VimApp;
تعريفنوع بنية VimScrollBar VimScrollBar;

#د_که_پای
