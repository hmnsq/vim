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

ਬਣਤਰ BMenu;
ਬਣਤਰ BMenu_ਵਸਤੂ;
ਬਣਤਰ BPictureButton;

// Our own Vim-related classes

ਬਣਤਰ VimApp;
ਬਣਤਰ VimFormView;
ਬਣਤਰ Vim_ਪਾਠ_Area_View;
ਬਣਤਰ Vim_ਖਿੜਕੀ;
ਬਣਤਰ VimScrollBar;

// Locking functions

ਬਾਹਰੀ ਪੂਰਨ vim_ਤਾਲਾ_screen();
ਬਾਹਰੀ ਖਾਲੀ vim_unlock_screen();

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __cplusplus

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ BMenu BMenu;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ BMenu_ਵਸਤੂ BMenu_ਵਸਤੂ;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ BPictureButton BPictureButton;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Vim_ਖਿੜਕੀ Vim_ਖਿੜਕੀ;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ VimFormView VimFormView;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Vim_ਪਾਠ_Area_View Vim_ਪਾਠ_Area_View;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ VimApp VimApp;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ VimScrollBar VimScrollBar;

#ਜੇ_ਅੰਤ
