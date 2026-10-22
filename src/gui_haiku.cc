/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *    BeBox GUI support Copyright 1998 by Olaf Seibert.
 *		    All Rights Reserved.
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 *
 * Based on "GUI support for the Buzzword Enhanced Operating System."
 *
 * Ported to R4 by Richard Offer <richard@whitequeen.com> Jul 99
 *
 * Haiku support by Siarzhuk Zharski <imker@gmx.li> Apr-Mai 2009
 *
 */

/*
 * Structure of the Haiku GUI code:
 *
 * There are 3 threads.
 * 1. The initial thread. In gui_mch_prepare() this gets to run the
 *    BApplication message loop. But before it starts doing that,
 *    it creates thread 2
 * 2. The main() thread. This thread is created in gui_mch_prepare()
 *    and its purpose in life is to call main(argc, argv) again.
 *    This thread is doing the bulk of the work.
 * 3. Sooner or later, a window is opened by the main() thread. This
 *    causes a second message loop to be created: the window thread.
 *
 * == alternatively ===
 *
 * #if RUN_BAPPLICATION_IN_NEW_THREAD...
 *
 * 1. The initial thread. In gui_mch_prepare() this gets to spawn
 *    thread 2. After doing that, it returns to main() to do the
 *    bulk of the work, being the main() thread.
 * 2. Runs the BApplication.
 * 3. The window thread, just like in the first case.
 *
 * This second alternative is cleaner from Vim's viewpoint. However,
 * the BeBook seems to assume everywhere that the BApplication *must*
 * run in the initial thread. So perhaps doing otherwise is very wrong.
 *
 * However, from a B_SINGLE_LAUNCH viewpoint, the first is better.
 * If Vim is marked "Single Launch" in its application resources,
 * and a file is dropped on the Vim icon, and another Vim is already
 * running, the file is passed on to the earlier Vim. This happens
 * in BApplication::Run(). So we want Vim to terminate if
 * BApplication::Run() terminates. (See the BeBook, on BApplication.
 * However, it seems that the second copy of Vim isn't even started
 * in this case... which is for the better since I wouldn't know how
 * to detect this case.)
 *
 * Communication between these threads occurs mostly by translating
 * BMessages that come in and posting an appropriate translation on
 * the VDCMP (Vim Direct Communication Message Port). Therefore the
 * actions required for keypresses and window resizes, etc, are mostly
 * performed in the main() thread.
 *
 * A notable exception to this is the Draw() event. The redrawing of
 * the window contents is performed asynchronously from the window
 * thread. To make this work correctly, a locking protocol is used when
 * any thread is accessing the essential variables that are used by
 * the window thread.
 *
 * This locking protocol consists of locking Vim's window. This is both
 * convenient and necessary.
 */

ಬಾಹ್ಯ "C" {

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <float.h>
#ಅನ್ತರ್ಭಾವ <syslog.h>

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#ಅನ್ತರ್ಭಾವ <Application.h>
#ಅನ್ತರ್ಭಾವ <Beep.h>
#ಅನ್ತರ್ಭಾವ <Bitmap.h>
#ಅನ್ತರ್ಭಾವ <Box.h>
#ಅನ್ತರ್ಭಾವ <Button.h>
#ಅನ್ತರ್ಭಾವ <Clipboard.h>
#ಅನ್ತರ್ಭಾವ <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#ಅನ್ತರ್ಭಾವ <File.h>
#ಅನ್ತರ್ಭಾವ <FilePanel.h>
#ಅನ್ತರ್ಭಾವ <FindDirectory.h>
//#include <Font.h>
#ಅನ್ತರ್ಭಾವ <IconUtils.h>
#ಅನ್ತರ್ಭಾವ <Input.h>
#ಅನ್ತರ್ಭಾವ <ListView.h>
#ಅನ್ತರ್ಭಾವ <MenuBar.h>
#ಅನ್ತರ್ಭಾವ <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#ಅನ್ತರ್ಭಾವ <Path.h>
#ಅನ್ತರ್ಭಾವ <PictureButton.h>
#ಅನ್ತರ್ಭಾವ <PopUpMenu.h>
//#include <Region.h>
#ಅನ್ತರ್ಭಾವ <Resources.h>
//#include <Roster.h>
#ಅನ್ತರ್ಭಾವ <Screen.h>
#ಅನ್ತರ್ಭಾವ <ScrollBar.h>
#ಅನ್ತರ್ಭಾವ <ScrollView.h>
#ಅನ್ತರ್ಭಾವ <String.h>
#ಅನ್ತರ್ಭಾವ <StringView.h>
//#include <SupportDefs.h>
#ಅನ್ತರ್ಭಾವ <TabView.h>
#ಅನ್ತರ್ಭಾವ <TextControl.h>
#ಅನ್ತರ್ಭಾವ <TextView.h>
#ಅನ್ತರ್ಭಾವ <TranslationUtils.h>
#ಅನ್ತರ್ಭಾವ <TranslatorFormats.h>
#ಅನ್ತರ್ಭಾವ <View.h>
#ಅನ್ತರ್ಭಾವ <Window.h>

ವರ್ಗ VimApp;
ವರ್ಗ VimFormView;
ವರ್ಗ Vim_ಪಠ್ಯ_Area_View;
ವರ್ಗ Vim_ಕಿಟಕಿ;
ವರ್ಗ VimToolbar;
ವರ್ಗ Vim_Tab_ಸಾಲು;

ಬಾಹ್ಯ ಕೀಲಿ_map *ಕೀಲಿ_Map;
ಬಾಹ್ಯ ಅಕ್ಷರ *ಕೀಲಿ_Map_Chars;

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ B_MAX_ಪೋರ್ಟ್_ಎಣಿಕೆ
#ವ್ಯಾಖ್ಯಾನಿಸು B_MAX_ಪೋರ್ಟ್_ಎಣಿಕೆ    255
#ಆದರೆ_ಅಂತ್ಯ

// VimApp seems comparable to the X "vimShell"
ವರ್ಗ VimApp: ಸಾರ್ವಜನಿಕ BApplication
{
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ BApplication Inherited;
    ಸಾರ್ವಜನಿಕ:
    VimApp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *appsig);
    ~VimApp();

    // callbacks:
#ಆದರೆ 0
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ Dispatch_ಸಂದೇಶ(BMessage *m, BHandler *h)
    {
	m->Print_ಗಮ್ಯ_ಹರಿವು();
	Inherited::Dispatch_ಸಂದೇಶ(m, h);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ Ready_ಗಮ್ಯ_ಸತತ();
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ ArgvReceived(int32 argc, ಅಕ್ಷರ **argv);
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ RefsReceived(BMessage *m);
    ವಾಸ್ತವಿಕ ತಾರ್ಕಿಕ QuitRequested();
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸಂದೇಶ_Received(BMessage *m);

    ಸ್ಥಿರ ಶೂನ್ಯ SendRefs(BMessage *m, ತಾರ್ಕಿಕ changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	f_Browsed_ಮಾರ್ಗ;
    ಖಾಸಗಿ:
};

ವರ್ಗ Vim_ಕಿಟಕಿ: ಸಾರ್ವಜನಿಕ BWindow
{
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ BWindow Inherited;
    ಸಾರ್ವಜನಿಕ:
    Vim_ಕಿಟಕಿ();
    ~Vim_ಕಿಟಕಿ();

    ವಾಸ್ತವಿಕ ಶೂನ್ಯ Dispatch_ಸಂದೇಶ(BMessage *m, BHandler *h);
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಕಿಟಕಿ_Activated(ತಾರ್ಕಿಕ ಸಕ್ರಿಯ);
    ವಾಸ್ತವಿಕ ತಾರ್ಕಿಕ QuitRequested();

    VimFormView	    *formView;

    ಖಾಸಗಿ:
    ಶೂನ್ಯ init();

    ತಾರ್ಕಿಕ is_fullscreen = ಸುಳ್ಳು;
    BRect saved_ಚೌಕಟ್ಟು;
    ಕಿಟಕಿ_look saved_look;
};

ವರ್ಗ VimFormView: ಸಾರ್ವಜನಿಕ BView
{
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ BView Inherited;
    ಸಾರ್ವಜನಿಕ:
    VimFormView(BRect ಚೌಕಟ್ಟು);
    ~VimFormView();

    // callbacks:
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ AllAttached();
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಚೌಕಟ್ಟು_Resized(ದಶಮಾಂಶ new_ಅಗಲ, ದಶಮಾಂಶ new_ಎತ್ತರ);

#ವ್ಯಾಖ್ಯಾನಿಸು MENUBAR_MARGIN	1
    ದಶಮಾಂಶ Menu_ಎತ್ತರ() ಸ್ಥಿರಾಂಕ
    { ಹಿಂತಿರುಗಿ menuBar ? menuBar->ಚೌಕಟ್ಟು_2().ಎತ್ತರ_2() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() ಸ್ಥಿರಾಂಕ
    { ಹಿಂತಿರುಗಿ menuBar; }

    ಖಾಸಗಿ:
    ಶೂನ್ಯ init(BRect);

    BMenuBar	    *menuBar;
    Vim_ಪಠ್ಯ_Area_View *ಪಠ್ಯ_Area;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    ಸಾರ್ವಜನಿಕ:
    ದಶಮಾಂಶ Toolbar_ಎತ್ತರ() ಸ್ಥಿರಾಂಕ;
    VimToolbar *ToolBar() ಸ್ಥಿರಾಂಕ
    { ಹಿಂತಿರುಗಿ toolBar; }
    ಖಾಸಗಿ:
    VimToolbar	    *toolBar;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    ಸಾರ್ವಜನಿಕ:
    Vim_Tab_ಸಾಲು *Tab_ಸಾಲು() ಸ್ಥಿರಾಂಕ	{ ಹಿಂತಿರುಗಿ tab_ಸಾಲು; }
    ತಾರ್ಕಿಕ Is_Showing_Tab_ಸಾಲು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ showing_Tab_ಸಾಲು; }
    ಶೂನ್ಯ ಗಣ_Showing_Tab_ಸಾಲು(ತಾರ್ಕಿಕ showing) { showing_Tab_ಸಾಲು = showing;	}
    ದಶಮಾಂಶ Tabline_ಎತ್ತರ() ಸ್ಥಿರಾಂಕ;
    ಖಾಸಗಿ:
    Vim_Tab_ಸಾಲು	*tab_ಸಾಲು;
    ಪೂರ್ಣಾಂಕ	showing_Tab_ಸಾಲು;
#ಆದರೆ_ಅಂತ್ಯ
};

ವರ್ಗ Vim_ಪಠ್ಯ_Area_View: ಸಾರ್ವಜನಿಕ BView
{
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ BView Inherited;
    ಸಾರ್ವಜನಿಕ:
    Vim_ಪಠ್ಯ_Area_View(BRect ಚೌಕಟ್ಟು);
    ~Vim_ಪಠ್ಯ_Area_View();

    // callbacks:
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ Draw(BRect updateRect);
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಕೀಲಿ_ಕೆಳಕ್ಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, int32 ಅಂಕೆ_ಬೈಟುಗಳು_2);
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ Mouse_ಕೆಳಕ್ಕೆ(BPoint ಬಿಂದು);
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ Mouse_ಮೇಲೆ(BPoint ಬಿಂದು);
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ MouseMoved(BPoint ಬಿಂದು, uint32 transit, ಸ್ಥಿರಾಂಕ BMessage *ಸಂದೇಶ_2);
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸಂದೇಶ_Received(BMessage *m);

    // own functions:
    ಪೂರ್ಣಾಂಕ mchInitFont(char_u *name);
    ಶೂನ್ಯ mchDrawString(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, char_u *s, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ flags);
    ಶೂನ್ಯ mch_Clear_ಖಂಡ(ಪೂರ್ಣಾಂಕ row1, ಪೂರ್ಣಾಂಕ col1, ಪೂರ್ಣಾಂಕ row2, ಪೂರ್ಣಾಂಕ col2);
    ಶೂನ್ಯ mchClearAll();
    ಶೂನ್ಯ mch_Delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಾಲುಗಳು);
    ಶೂನ್ಯ mch_ಸೇರಿಸು_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಾಲುಗಳು);

    ಸ್ಥಿರ ಶೂನ್ಯ guiSendMouseEvent(ಪೂರ್ಣಾಂಕ button, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ repeated_click, int_u modifiers);
    ಸ್ಥಿರ ಶೂನ್ಯ guiMouseMoved(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y);
    ಸ್ಥಿರ ಶೂನ್ಯ guiBlankMouse(ತಾರ್ಕಿಕ should_hide);
    ಸ್ಥಿರ int_u mouse_Modifiers_ಗಮ್ಯ_Vim(int32 beModifiers);

    int32 mouse_Drag_Event_ಎಣಿಕೆ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
    ಶೂನ್ಯ DrawIMString(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

    ಖಾಸಗಿ:
    ಶೂನ್ಯ init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
    ರಚನೆ {
	BMessenger* messenger;
	BMessage* ಸಂದೇಶ_2;
	BPoint location;
	ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು;
	ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2;
    } IMData;
#ಆದರೆ_ಅಂತ್ಯ
};

ವರ್ಗ VimScrollBar: ಸಾರ್ವಜನಿಕ BScrollBar
{
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ BScrollBar Inherited;
    ಸಾರ್ವಜನಿಕ:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಮೌಲ್ಯ_ಬದಲಾಯಿತು_2(ದಶಮಾಂಶ new_ಮೌಲ್ಯ);
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ Mouse_ಮೇಲೆ(BPoint where);
    ಶೂನ್ಯ ಗಣ_ಮೌಲ್ಯ(ದಶಮಾಂಶ newval);
    scrollbar_T *getGsb()
    { ಹಿಂತಿರುಗಿ gsb; }

    int32	scroll_Event_ಎಣಿಕೆ;

    ಖಾಸಗಿ:
    scrollbar_T *gsb;
    ದಶಮಾಂಶ   ignore_ಮೌಲ್ಯ;
};


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR

ವರ್ಗ VimToolbar : ಸಾರ್ವಜನಿಕ BBox
{
    ಸ್ಥಿರ BBitmap *ಸಾಮಾನ್ಯ_Buttons_Bitmap;
    ಸ್ಥಿರ BBitmap *grayedButtonsBitmap;

    BBitmap *ತುಂಬು_Vim_Bitmap(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* fileName);
    ತಾರ್ಕಿಕ GetPictureFromBitmap(BPicture *picture_ಗಮ್ಯ, int32 ಸೂಚ್ಯಂಕ, BBitmap *bitmapFrom, ತಾರ್ಕಿಕ pressed);
    ತಾರ್ಕಿಕ Modify_Bitmap_ಗಮ್ಯ_Grayed(BBitmap *bitmap);

    BList fButtonsList;
    ಶೂನ್ಯ InvalidateLayout();

    ಸಾರ್ವಜನಿಕ:
    VimToolbar(BRect ಚೌಕಟ್ಟು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * name);
    ~VimToolbar();

    ತಾರ್ಕಿಕ PrepareButtonBitmaps();

    ತಾರ್ಕಿಕ ಕೂಡಿಸು_Button(int32 ಸೂಚ್ಯಂಕ, vimmenu_T *menu);
    ತಾರ್ಕಿಕ RemoveButton(vimmenu_T *menu);
    ತಾರ್ಕಿಕ ಬೂದು_Button(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ grey);

    ದಶಮಾಂಶ Toolbar_ಎತ್ತರ() ಸ್ಥಿರಾಂಕ;
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ Attached_ಗಮ್ಯ_ಕಿಟಕಿ();
};

BBitmap *VimToolbar::ಸಾಮಾನ್ಯ_Buttons_Bitmap  = ಖಾಲಿ;
BBitmap *VimToolbar::grayedButtonsBitmap  = ಖಾಲಿ;

ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ToolbarMargin = 3.;
ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ButtonMargin  = 3.;

#ಆದರೆ_ಅಂತ್ಯ //FEAT_TOOLBAR

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE

ವರ್ಗ Vim_Tab_ಸಾಲು : ಸಾರ್ವಜನಿಕ BTabView
{
    ಸಾರ್ವಜನಿಕ:
	ವರ್ಗ VimTab : ಸಾರ್ವಜನಿಕ BTab {
	    ಸಾರ್ವಜನಿಕ:
		VimTab() : BTab(ಹೊಸ BView(BRect(), "-Empty-", 0, 0)) {}

	    ವಾಸ್ತವಿಕ ಶೂನ್ಯ Select(BView* owner);
	};

	Vim_Tab_ಸಾಲು(BRect r) : BTabView(r, "vimTabLine", B_ಅಗಲ_FROM_LABEL,
	       B_FOLLOW_ಎಡ | B_FOLLOW_ತುದಿ | B_FOLLOW_ಬಲ, B_WILL_DRAW | B_ಚೌಕಟ್ಟು_EVENTS) {}

    ದಶಮಾಂಶ Tabline_ಎತ್ತರ() ಸ್ಥಿರಾಂಕ;
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ Mouse_ಕೆಳಕ್ಕೆ(BPoint ಬಿಂದು);
};

#ಆದರೆ_ಅಂತ್ಯ //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
ವರ್ಗ VimFont: ಸಾರ್ವಜನಿಕ BFont
{
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ BFont Inherited;
    ಸಾರ್ವಜನಿಕ:
    VimFont();
    VimFont(ಸ್ಥಿರಾಂಕ VimFont *rhs);
    VimFont(ಸ್ಥಿರಾಂಕ BFont *rhs);
    VimFont(ಸ್ಥಿರಾಂಕ VimFont &rhs);
    ~VimFont();

    VimFont *ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ refcount;
    char_u *name;

    ಖಾಸಗಿ:
    ಶೂನ್ಯ init();
};

#ಆದರೆ defined(FEAT_GUI_DIALOG)

ವರ್ಗ VimDialog : ಸಾರ್ವಜನಿಕ BWindow
{
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ BWindow Inherited;

    BButton* _CreateButton(int32 which, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* label);

    ಸಾರ್ವಜನಿಕ:

    ವರ್ಗ View : ಸಾರ್ವಜನಿಕ BView {
	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ BView Inherited;

	ಸಾರ್ವಜನಿಕ:
	View(BRect ಚೌಕಟ್ಟು);
	~View();

	ವಾಸ್ತವಿಕ ಶೂನ್ಯ Draw(BRect updateRect);
	ಶೂನ್ಯ InitIcon(int32 type);

	ಖಾಸಗಿ:
	BBitmap*    fIconBitmap;
    };

    VimDialog(ಪೂರ್ಣಾಂಕ type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *title, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ_2,
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buttons, ಪೂರ್ಣಾಂಕ dfltbutton, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *textfield,
	    ಪೂರ್ಣಾಂಕ ex_cmd);
    ~VimDialog();

    ಪೂರ್ಣಾಂಕ Go();

    ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸಂದೇಶ_Received(BMessage *ಸಂದೇಶ);

    ಖಾಸಗಿ:
    sem_id	    fDialogSem;
    ಪೂರ್ಣಾಂಕ		    f_Dialog_ಮೌಲ್ಯ;
    BList	    fButtonsList;
    BTextView*	    f_ಸಂದೇಶ_View;
    BTextControl*   f_ಒಳಹರಿವು_Control;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*	    f_ಒಳಹರಿವು_ಮೌಲ್ಯ;
};

ವರ್ಗ VimSelectFontDialog : ಸಾರ್ವಜನಿಕ BWindow
{
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ BWindow Inherited;

    ಶೂನ್ಯ _CleanList(BListView* list);
    ಶೂನ್ಯ _UpdateFontStyles();
    ಶೂನ್ಯ Update_ಗಾತ್ರ_ಒಳಹರಿವು_Preview();
    ಶೂನ್ಯ _UpdateFontPreview();
    ತಾರ್ಕಿಕ Update_From_List_ಅಂಶ(BListView* list, ಅಕ್ಷರ* ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ ಪಠ್ಯ_ಗಾತ್ರ);
    ಸಾರ್ವಜನಿಕ:

    VimSelectFontDialog(font_family* family, font_style* style, ದಶಮಾಂಶ* ಗಾತ್ರ_2);
    ~VimSelectFontDialog();

    ತಾರ್ಕಿಕ Go();

    ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸಂದೇಶ_Received(BMessage *ಸಂದೇಶ);

    ಖಾಸಗಿ:
    ಸ್ಥಿತಿ_t	    f_ಸ್ಥಿತಿ;
    sem_id	    fDialogSem;
    ತಾರ್ಕಿಕ	    f_Dialog_ಮೌಲ್ಯ;
    font_family*    fFamily;
    font_style*	    fStyle;
    ದಶಮಾಂಶ*	    f_ಗಾತ್ರ;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    ದಶಮಾಂಶ	    f_Font_ಗಾತ್ರ;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_ಒಳಹರಿವು;
};

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

ರಚನೆ Main_ವಾದಗಳು {
    ಪೂರ್ಣಾಂಕ	     argc;
    ಅಕ್ಷರ    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#ವ್ಯಾಖ್ಯಾನಿಸು	ಕೀಲಿ_ಸಂದೇಶ_BUFSIZ	7
#ಆದರೆ ಕೀಲಿ_ಸಂದೇಶ_BUFSIZ < MAX_ಕೀಲಿ_ಸಂಕೇತ_LEN
#ದೋಷ Increase ಕೀಲಿ_ಸಂದೇಶ_BUFSIZ!
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ Vim_ಕೀಲಿ_ಸಂದೇಶ {
    char_u  ಉದ್ದಳತೆ;
    char_u  chars[ಕೀಲಿ_ಸಂದೇಶ_BUFSIZ];  // contains Vim encoding
    ತಾರ್ಕಿಕ    csi_escape;
};

ರಚನೆ Vim_Resize_ಸಂದೇಶ {
    ಪೂರ್ಣಾಂಕ	    ಅಗಲ;
    ಪೂರ್ಣಾಂಕ	    ಎತ್ತರ;
};

ರಚನೆ Vim_Scroll_Bar_ಸಂದೇಶ {
    VimScrollBar *sb;
    ದೀರ್ಘ    ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಂಕ	    stillDragging;
};

ರಚನೆ Vim_Menu_ಸಂದೇಶ {
    vimmenu_T	*guiMenu;
};

ರಚನೆ Vim_Mouse_ಸಂದೇಶ {
    ಪೂರ್ಣಾಂಕ	    button;
    ಪೂರ್ಣಾಂಕ	    x;
    ಪೂರ್ಣಾಂಕ	    y;
    ಪೂರ್ಣಾಂಕ	    repeated_click;
    int_u   modifiers;
};

ರಚನೆ Vim_Mouse_Moved_ಸಂದೇಶ {
    ಪೂರ್ಣಾಂಕ	    x;
    ಪೂರ್ಣಾಂಕ	    y;
};

ರಚನೆ Vim_Focus_ಸಂದೇಶ {
    ತಾರ್ಕಿಕ    ಸಕ್ರಿಯ;
};

ರಚನೆ Vim_Refs_ಸಂದೇಶ {
    BMessage   *ಸಂದೇಶ_2;
    ತಾರ್ಕಿಕ    changedir;
};

ರಚನೆ Vim_Tabline_ಸಂದೇಶ {
    ಪೂರ್ಣಾಂಕ	    ಸೂಚ್ಯಂಕ;
};

ರಚನೆ Vim_Tabline_Menu_ಸಂದೇಶ {
    ಪೂರ್ಣಾಂಕ	    ಸೂಚ್ಯಂಕ;
    ಪೂರ್ಣಾಂಕ	    event;
};

ರಚನೆ Vim_ಸಂದೇಶ {
    ಎಣಿಕೆ Vim_ಸಂದೇಶ_Type {
	ಕೀಲಿ_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    ಒಕ್ಕೂಟ {
	ರಚನೆ Vim_ಕೀಲಿ_ಸಂದೇಶ    ಕೀಲಿ_2;
	ರಚನೆ Vim_Resize_ಸಂದೇಶ New_ಗಾತ್ರ;
	ರಚನೆ Vim_Scroll_Bar_ಸಂದೇಶ	Scroll;
	ರಚನೆ Vim_Menu_ಸಂದೇಶ   Menu;
	ರಚನೆ Vim_Mouse_ಸಂದೇಶ  Mouse;
	ರಚನೆ Vim_Mouse_Moved_ಸಂದೇಶ	MouseMoved;
	ರಚನೆ Vim_Focus_ಸಂದೇಶ  Focus;
	ರಚನೆ Vim_Refs_ಸಂದೇಶ   Refs;
	ರಚನೆ Vim_Tabline_ಸಂದೇಶ	Tabline;
	ರಚನೆ Vim_Tabline_Menu_ಸಂದೇಶ    TablineMenu;
    } u;
};

#ವ್ಯಾಖ್ಯಾನಿಸು RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು GUI_ಗಮ್ಯ_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

ಸ್ಥಿರ ರಚನೆ specialkey
{
    uint16  Be_ಕೀಲಿಗಳು;
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೀಲಿ_3(a,b)    ((a)<<8|(b))
#ವ್ಯಾಖ್ಯಾನಿಸು K(a)	    ಕೀಲಿ_3(0,a)		// for ASCII codes
#ವ್ಯಾಖ್ಯಾನಿಸು F(b)	    ಕೀಲಿ_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_ಕೀಲಿಗಳು[] =
{
    {K(B_ಮೇಲೆ_ARROW),	'k', 'u'},
    {K(B_ಕೆಳಕ್ಕೆ_ARROW),	    'k', 'd'},
    {K(B_ಎಡ_ARROW),	    'k', 'l'},
    {K(B_ಬಲ_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_ಸೇರಿಸು),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_ಕೊನೆ),		'@', '7'},
    {K(B_ಪುಟ_ಮೇಲೆ),	'k', 'P'},	// XK_Prior
    {K(B_ಪುಟ_ಕೆಳಕ್ಕೆ),	    'k', 'N'},	    // XK_Next,

#ವ್ಯಾಖ್ಯಾನಿಸು ಮೊದಲ_FUNCTION_ಕೀಲಿ  11
    {F(B_F1_ಕೀಲಿ),	'k', '1'},
    {F(B_F2_ಕೀಲಿ),	'k', '2'},
    {F(B_F3_ಕೀಲಿ),	'k', '3'},
    {F(B_F4_ಕೀಲಿ),	'k', '4'},
    {F(B_F5_ಕೀಲಿ),	'k', '5'},
    {F(B_F6_ಕೀಲಿ),	'k', '6'},
    {F(B_F7_ಕೀಲಿ),	'k', '7'},
    {F(B_F8_ಕೀಲಿ),	'k', '8'},
    {F(B_F9_ಕೀಲಿ),	'k', '9'},
    {F(B_F10_ಕೀಲಿ),	'k', ';'},

    {F(B_F11_ಕೀಲಿ),	'F', '1'},
    {F(B_F12_ಕೀಲಿ),	'F', '2'},
    //	{XK_F13,	    'F', '3'},	// would be print screen
    // sysreq
    {F(0x0F),		'F', '4'},	// scroll lock
    {F(0x10),		'F', '5'},	// pause/break
    //	{XK_F16,	'F', '6'},
    //	{XK_F17,	'F', '7'},
    //	{XK_F18,	'F', '8'},
    //	{XK_F19,	'F', '9'},
    //	 {XK_F20,	'F', 'A'},
    //	{XK_F21,	'F', 'B'},
    //	{XK_F22,	'F', 'C'},
    //	{XK_F23,	'F', 'D'},
    //	{XK_F24,	'F', 'E'},
    //	{XK_F25,	'F', 'F'},
    //	{XK_F26,	'F', 'G'},
    //	{XK_F27,	'F', 'H'},
    //	{XK_F28,	'F', 'I'},
    //	{XK_F29,	'F', 'J'},
    //	{XK_F30,	'F', 'K'},
    //	{XK_F31,	'F', 'L'},
    //	{XK_F32,	'F', 'M'},
    //	{XK_F33,	'F', 'N'},
    //	{XK_F34,	'F', 'O'},
    //	{XK_F35,	'F', 'P'},	// keysymdef.h defines up to F35

    //	{XK_Help,	'%', '1'},	// XK_Help
    {F(B_PRINT_ಕೀಲಿ),	    '%', '9'},

#ಆದರೆ 0
    // Keypad keys:
    {F(0x48),	    'k', 'l'},	    // XK_KP_Left
    {F(0x4A),	    'k', 'r'},	    // XK_KP_Right
    {F(0x38),	    'k', 'u'},	    // XK_KP_Up
    {F(0x59),	    'k', 'd'},	    // XK_KP_Down
    {F(0x64),	    'k', 'I'},	    // XK_KP_Insert
    {F(0x65),	    'k', 'D'},	    // XK_KP_Delete
    {F(0x37),	    'k', 'h'},	    // XK_KP_Home
    {F(0x58),	    '@', '7'},	    // XK_KP_End
    {F(0x39),	    'k', 'P'},	    // XK_KP_Prior
    {F(0x60),	    'k', 'N'},	    // XK_KP_Next
    {F(0x49),	    '&', '8'},	    // XK_Undo, keypad 5
#ಆದರೆ_ಅಂತ್ಯ

    // End of list marker:
    {0,		    0, 0}
};

#ವ್ಯಾಖ್ಯಾನಿಸು ಅಂಕೆ_SPECIAL_ಕೀಲಿಗಳು    ARRAY_ಉದ್ದಳತೆ(special_ಕೀಲಿಗಳು)

// ---------------- VimApp ----------------

    ಸ್ಥಿರ ಶೂನ್ಯ
docd(BPath &ಮಾರ್ಗ)
{
    mch_chdir((ಅಕ್ಷರ *)ಮಾರ್ಗ.ಮಾರ್ಗ_2());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	ಸ್ಥಿರ ಶೂನ್ಯ
drop_callback(ಶೂನ್ಯ *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	ಸ್ಥಿರ ಶೂನ್ಯ
RefsReceived(BMessage *m, ತಾರ್ಕಿಕ changedir)
{
    uint32 type;
    int32 ಎಣಿಕೆ_2;

    m->Print_ಗಮ್ಯ_ಹರಿವು();
    ಆಯ್ಕೆ (m->what) {
	ಸ್ಥಿತಿ B_REFS_RECEIVED:
	ಸ್ಥಿತಿ B_SIMPLE_ದತ್ತಾಂಶ:
	    m->GetInfo("refs", &type, &ಎಣಿಕೆ_2);
	    ಆದರೆ (type != B_REF_TYPE)
		ಹೋಗು bad;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &ಎಣಿಕೆ_2);
	    ಆದರೆ (type != B_STRING_TYPE)
		ಹೋಗು bad;
	    ಆದರೆ (changedir) {
		ಅಕ್ಷರ *dirname;
		ಆದರೆ (m->ಕಂಡುಹಿಡಿ_String("cwd", (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    ಅಳಿಸು m;
	    ಹಿಂತಿರುಗಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VISUAL
    reset_VIsual();
#ಆದರೆ_ಅಂತ್ಯ

    char_u  **fnames;
    fnames = (char_u **) alloc(ಎಣಿಕೆ_2 * ಗಾತ್ರ(char_u *));
    ಪೂರ್ಣಾಂಕ fname_ಸೂಚ್ಯಂಕ = 0;

    ಆಯ್ಕೆ (m->what) {
	ಸ್ಥಿತಿ B_REFS_RECEIVED:
	ಸ್ಥಿತಿ B_SIMPLE_ದತ್ತಾಂಶ:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಎಣಿಕೆ_2; ++i)
	    {
		ನಮೂದು_ref ref;
		ಆದರೆ (m->ಕಂಡುಹಿಡಿ_Ref("refs", i, &ref) == B_OK) {
		    BEntry ನಮೂದು(&ref, ಸುಳ್ಳು);
		    BPath ಮಾರ್ಗ;
		    ನಮೂದು.Get_ಮಾರ್ಗ(&ಮಾರ್ಗ);

		    // Change to parent directory?
		    ಆದರೆ (changedir) {
			BPath parentpath;
			ಮಾರ್ಗ.Get_ಪೋಷಕ(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    ಆದರೆ (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			ಆದರೆ (!changedir)
			    docd(ಮಾರ್ಗ);
		    } ಇಲ್ಲದಿದ್ದರೆ {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)ಮಾರ್ಗ.ಮಾರ್ಗ_2(), IObuff);
			ಆದರೆ (fname == ಖಾಲಿ)
			    fname = (char_u *)ಮಾರ್ಗ.ಮಾರ್ಗ_2();
			fnames[fname_ಸೂಚ್ಯಂಕ++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = ಸುಳ್ಳು;
		}
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < ಎಣಿಕೆ_2; ++i)
	    {
		ಅಕ್ಷರ *fname;

		ಆದರೆ (m->ಕಂಡುಹಿಡಿ_String("argv", i, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **) &fname) == B_OK) {
		    fnames[fname_ಸೂಚ್ಯಂಕ++] = vim_strsave((char_u *)fname);
		}
	    }
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    // fprintf(stderr, "case default\n");
	    ನಿಲ್ಲಿಸು;
    }

    ಅಳಿಸು m;

    // Handle the drop, :edit to get to the file
    ಆದರೆ (fname_ಸೂಚ್ಯಂಕ > 0) {
	handle_drop(fname_ಸೂಚ್ಯಂಕ, fnames, FALSE, drop_callback, ಖಾಲಿ);

	setcursor();
	ಹೊರಹೋಗು_flush();
    } ಇಲ್ಲದಿದ್ದರೆ {
	vim_free(fnames);
    }
}

VimApp::VimApp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(ಖಾಲಿ)
{
}

VimApp::~VimApp()
{
}

    ಶೂನ್ಯ
VimApp::Ready_ಗಮ್ಯ_ಸತತ()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    ಶೂನ್ಯ
VimApp::ArgvReceived(int32 arg_argc, ಅಕ್ಷರ **arg_argv)
{
    ಆದರೆ (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	ಆದರೆ (gui.vim_ಕಿಟಕಿ)
	    gui.vim_ಕಿಟಕಿ->Minimize(ಸುಳ್ಳು);
	BMessage *m = Current_ಸಂದೇಶ();
	Detach_Current_ಸಂದೇಶ();
	SendRefs(m, ನಿಜ);
    }
}

    ಶೂನ್ಯ
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    ಪೂರ್ಣಾಂಕ ಎಲ್ಲೆ = 15;
    ಆಗ (--ಎಲ್ಲೆ >= 0 && (curbuf == ಖಾಲಿ || curbuf->b_ಆರಂಭ_ffc == 0))
	snooze(100000);    //  0.1 s
    ಆದರೆ (gui.vim_ಕಿಟಕಿ)
	gui.vim_ಕಿಟಕಿ->Minimize(ಸುಳ್ಳು);
    Detach_Current_ಸಂದೇಶ();
    SendRefs(m, ನಿಜ);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    ಶೂನ್ಯ
VimApp::SendRefs(BMessage *m, ತಾರ್ಕಿಕ changedir)
{
    Vim_Refs_ಸಂದೇಶ rm;
    rm.ಸಂದೇಶ_2 = m;
    rm.changedir = changedir;

    write_ಪೋರ್ಟ್(gui.vdcmp, Vim_ಸಂದೇಶ::Refs, &rm, ಗಾತ್ರ(rm));
    //	calls ::RefsReceived
}

    ಶೂನ್ಯ
VimApp::ಸಂದೇಶ_Received(BMessage *m)
{
    ಆಯ್ಕೆ (m->what) {
	ಸ್ಥಿತಿ 'save':
	    {
		ನಮೂದು_ref refDirectory;
		m->ಕಂಡುಹಿಡಿ_Ref("directory", &refDirectory);
		f_Browsed_ಮಾರ್ಗ.ಗಣ_ಗಮ್ಯ(&refDirectory);
		BString strName;
		m->ಕಂಡುಹಿಡಿ_String("name", &strName);
		f_Browsed_ಮಾರ್ಗ.Append(strName.String());
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'open':
	    {
		ನಮೂದು_ref ref;
		m->ಕಂಡುಹಿಡಿ_Ref("refs", &ref);
		f_Browsed_ಮಾರ್ಗ.ಗಣ_ಗಮ್ಯ(&ref);
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ B_CANCEL:
	    {
		BFilePanel *panel;
		m->ಕಂಡುಹಿಡಿ_ಸೂಚಕ("source", (ಶೂನ್ಯ**)&panel);
		ಆದರೆ (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    Inherited::ಸಂದೇಶ_Received(m);
	    ನಿಲ್ಲಿಸು;
    }
}

    ತಾರ್ಕಿಕ
VimApp::QuitRequested()
{
    (ಶೂನ್ಯ)Inherited::QuitRequested();
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

// ---------------- VimWindow ----------------

Vim_ಕಿಟಕಿ::Vim_ಕಿಟಕಿ():
    BWindow(BRect(40, 40, 150, 150),
	    "Vim",
	    B_TITLED_ಕಿಟಕಿ,
	    0,
	    B_CURRENT_WORKSPACE)

{
    init();
}

Vim_ಕಿಟಕಿ::~Vim_ಕಿಟಕಿ()
{
    ಆದರೆ (formView) {
	Remove_ಮಗು(formView);
	ಅಳಿಸು formView;
    }
    gui.vim_ಕಿಟಕಿ = ಖಾಲಿ;
}

    ಶೂನ್ಯ
Vim_ಕಿಟಕಿ::init()
{
    // Attach the VimFormView
    formView = ಹೊಸ VimFormView(Bounds());
    ಆದರೆ (formView != ಖಾಲಿ) {
	ಕೂಡಿಸು_ಮಗು(formView);
    }
}

#ಆದರೆ 0  //  disabled in zeta patch
    ಶೂನ್ಯ
Vim_ಕಿಟಕಿ::Dispatch_ಸಂದೇಶ(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    ಆಯ್ಕೆ (m->what) {
	ಸ್ಥಿತಿ B_MOUSE_ಮೇಲೆ:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		ಆದರೆ (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->ಕಂಡುಹಿಡಿ_ಬಿಂದು("where", &where);
		    v->Mouse_ಮೇಲೆ(where);
		} ಇಲ್ಲದಿದ್ದರೆ {
		    Inherited::Dispatch_ಸಂದೇಶ(m, h);
		}
	    }
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    Inherited::Dispatch_ಸಂದೇಶ(m, h);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
Vim_ಕಿಟಕಿ::ಕಿಟಕಿ_Activated(ತಾರ್ಕಿಕ ಸಕ್ರಿಯ)
{
    Inherited::ಕಿಟಕಿ_Activated(ಸಕ್ರಿಯ);
    // the textArea gets the keyboard action
    ಆದರೆ (ಸಕ್ರಿಯ && gui.vim_ಪಠ್ಯ_Area)
	gui.vim_ಪಠ್ಯ_Area->MakeFocus(ನಿಜ);

    ರಚನೆ Vim_Focus_ಸಂದೇಶ fm;
    fm.ಸಕ್ರಿಯ = ಸಕ್ರಿಯ;

    write_ಪೋರ್ಟ್(gui.vdcmp, Vim_ಸಂದೇಶ::Focus, &fm, ಗಾತ್ರ(fm));
}

    ತಾರ್ಕಿಕ
Vim_ಕಿಟಕಿ::QuitRequested()
{
    ರಚನೆ Vim_ಕೀಲಿ_ಸಂದೇಶ km;
    km.ಉದ್ದಳತೆ = 5;
    ಸ್ಮರಣೆ_ನಕಲು((ಅಕ್ಷರ *)km.chars, "\033:qa\r", km.ಉದ್ದಳತೆ);
    km.csi_escape = ಸುಳ್ಳು;
    write_ಪೋರ್ಟ್(gui.vdcmp, Vim_ಸಂದೇಶ::ಕೀಲಿ_2, &km, ಗಾತ್ರ(km));
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}
    ಶೂನ್ಯ
Vim_ಕಿಟಕಿ::Dispatch_ಸಂದೇಶ(BMessage *m, BHandler *h)
{
    ತಾರ್ಕಿಕ should_propagate = ನಿಜ;

    ಆಯ್ಕೆ (m->what)
    {
	ಸ್ಥಿತಿ B_ಕೀಲಿ_ಕೆಳಕ್ಕೆ:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->ಕಂಡುಹಿಡಿ_Int32("raw_char", &scancode);
		m->ಕಂಡುಹಿಡಿ_Int32("modifiers", &beModifiers);

		ಆದರೆ (scancode == B_ENTER && (beModifiers & B_ಎಡ_COMMAND_ಕೀಲಿ))
		    {
			should_propagate = ಸುಳ್ಳು;
			ಆದರೆ (ಇದು->is_fullscreen)
			    {
				ಇದು->is_fullscreen = ಸುಳ್ಳು;
				Resize_ಗಮ್ಯ(ಇದು->saved_ಚೌಕಟ್ಟು.ಅಗಲ_2(), ಇದು->saved_ಚೌಕಟ್ಟು.ಎತ್ತರ_2());
				Move_ಗಮ್ಯ(ಇದು->saved_ಚೌಕಟ್ಟು.ಎಡ, ಇದು->saved_ಚೌಕಟ್ಟು.ತುದಿ);
				ಗಣ_Look(ಇದು->saved_look);
				ಗಣ_Flags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } ಇಲ್ಲದಿದ್ದರೆ {
				ಇದು->saved_ಚೌಕಟ್ಟು = ಚೌಕಟ್ಟು_2();
				ಇದು->saved_look = Look();
				ಇದು->is_fullscreen = ನಿಜ;
				BScreen s(ಇದು);
				ಗಣ_Look(B_NO_BORDER_ಕಿಟಕಿ_LOOK);
				Resize_ಗಮ್ಯ(s.ಚೌಕಟ್ಟು_2().ಅಗಲ_2() + 1, s.ಚೌಕಟ್ಟು_2().ಎತ್ತರ_2() + 1);
				Move_ಗಮ್ಯ(s.ಚೌಕಟ್ಟು_2().ಎಡ, s.ಚೌಕಟ್ಟು_2().ತುದಿ);
				ಗಣ_Flags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    ಆದರೆ (should_propagate)
	Inherited::Dispatch_ಸಂದೇಶ(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect ಚೌಕಟ್ಟು):
    BView(ಚೌಕಟ್ಟು, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_ಚೌಕಟ್ಟು_EVENTS),
    menuBar(ಖಾಲಿ),
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    toolBar(ಖಾಲಿ),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_ಸಾಲು(ಖಾಲಿ),
#ಆದರೆ_ಅಂತ್ಯ
    ಪಠ್ಯ_Area(ಖಾಲಿ)
{
    init(ಚೌಕಟ್ಟು);
}

VimFormView::~VimFormView()
{
    ಆದರೆ (menuBar) {
	Remove_ಮಗು(menuBar);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ never
	//  deleting the menuBar leads to SEGV on exit
	//  richard@whitequeen.com Jul 99
	ಅಳಿಸು menuBar;
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    ಅಳಿಸು toolBar;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    ಅಳಿಸು tab_ಸಾಲು;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ಪಠ್ಯ_Area) {
	Remove_ಮಗು(ಪಠ್ಯ_Area);
	ಅಳಿಸು ಪಠ್ಯ_Area;
    }
    gui.vimForm = ಖಾಲಿ;
}

    ಶೂನ್ಯ
VimFormView::init(BRect ಚೌಕಟ್ಟು)
{
    menuBar = ಹೊಸ BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    ಕೂಡಿಸು_ಮಗು(menuBar);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    toolBar = ಹೊಸ VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    ಕೂಡಿಸು_ಮಗು(toolBar);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    tab_ಸಾಲು = ಹೊಸ Vim_Tab_ಸಾಲು(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    ಕೂಡಿಸು_ಮಗು(tab_ಸಾಲು);
#ಆದರೆ_ಅಂತ್ಯ

    BRect ಉಳಿದ = ಚೌಕಟ್ಟು;
    ಪಠ್ಯ_Area = ಹೊಸ Vim_ಪಠ್ಯ_Area_View(ಉಳಿದ);
    ಕೂಡಿಸು_ಮಗು(ಪಠ್ಯ_Area);
    // The textArea will be resized later when menus are added

    gui.vimForm = ಇದು;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    ದಶಮಾಂಶ
VimFormView::Toolbar_ಎತ್ತರ() ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ toolBar ? toolBar->Toolbar_ಎತ್ತರ() : 0.;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    ದಶಮಾಂಶ
VimFormView::Tabline_ಎತ್ತರ() ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ (tab_ಸಾಲು && Is_Showing_Tab_ಸಾಲು()) ? tab_ಸಾಲು->Tabline_ಎತ್ತರ() : 0.;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    ಆದರೆ (menuBar && ಪಠ್ಯ_Area) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect ಉಳಿದ = Bounds();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	ಉಳಿದ.ತುದಿ += Menu_ಎತ್ತರ();
	menuBar->Resize_ಗಮ್ಯ(ಉಳಿದ.ಬಲ, ಉಳಿದ.ತುದಿ);
	gui.menu_ಎತ್ತರ = (ಪೂರ್ಣಾಂಕ) Menu_ಎತ್ತರ();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
	toolBar->Move_ಗಮ್ಯ(ಉಳಿದ.ಎಡ, ಉಳಿದ.ತುದಿ);
	toolBar->Resize_ಗಮ್ಯ(ಉಳಿದ.ಬಲ, Toolbar_ಎತ್ತರ());
	ಉಳಿದ.ತುದಿ += Toolbar_ಎತ್ತರ();
	gui.toolbar_ಎತ್ತರ = Toolbar_ಎತ್ತರ();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
	tab_ಸಾಲು->Move_ಗಮ್ಯ(ಉಳಿದ.ಎಡ, ಉಳಿದ.ತುದಿ);
	tab_ಸಾಲು->Resize_ಗಮ್ಯ(ಉಳಿದ.ಬಲ + 1, Tabline_ಎತ್ತರ());
	ಉಳಿದ.ತುದಿ += Tabline_ಎತ್ತರ();
	gui.tabline_ಎತ್ತರ = Tabline_ಎತ್ತರ();
#ಆದರೆ_ಅಂತ್ಯ

	ಪಠ್ಯ_Area->Resize_ಗಮ್ಯ(ಉಳಿದ.ಅಗಲ_2(), ಉಳಿದ.ಎತ್ತರ_2());
	ಪಠ್ಯ_Area->Move_ಗಮ್ಯ(ಉಳಿದ.ಎಡ, ಉಳಿದ.ತುದಿ);
    }


    Inherited::AllAttached();
}

    ಶೂನ್ಯ
VimFormView::ಚೌಕಟ್ಟು_Resized(ದಶಮಾಂಶ new_ಅಗಲ, ದಶಮಾಂಶ new_ಎತ್ತರ)
{
    ರಚನೆ Vim_Resize_ಸಂದೇಶ sm;
    ಪೂರ್ಣಾಂಕ adjust_h, adjust_w;

    new_ಅಗಲ += 1;	//  adjust from width to number of pixels occupied
    new_ಎತ್ತರ += 1;

    sm.ಅಗಲ = (ಪೂರ್ಣಾಂಕ) new_ಅಗಲ;
    sm.ಎತ್ತರ = (ಪೂರ್ಣಾಂಕ) new_ಎತ್ತರ;
    adjust_w = ((ಪೂರ್ಣಾಂಕ)new_ಅಗಲ - gui_get_ತಳ_ಅಗಲ()) % gui.char_ಅಗಲ;
    adjust_h = ((ಪೂರ್ಣಾಂಕ)new_ಎತ್ತರ - gui_get_ತಳ_ಎತ್ತರ()) % gui.char_ಎತ್ತರ;

    ಆದರೆ (adjust_w > 0 || adjust_h > 0) {
	sm.ಅಗಲ  -= adjust_w;
	sm.ಎತ್ತರ -= adjust_h;
    }

    write_ಪೋರ್ಟ್(gui.vdcmp, Vim_ಸಂದೇಶ::Resize, &sm, ಗಾತ್ರ(sm));
    //	calls gui_resize_shell(new_width, new_height);

    ಹಿಂತಿರುಗಿ;

    /*
     * The area below the vertical scrollbar is erased to the colour
     * set with SetViewColor() automatically, because we had set
     * B_WILL_DRAW. Resizing the window tight around the vertical
     * scroll bar also helps to avoid debris.
     */
}

// ---------------- VimTextAreaView ----------------

Vim_ಪಠ್ಯ_Area_View::Vim_ಪಠ್ಯ_Area_View(BRect ಚೌಕಟ್ಟು):
    BView(ಚೌಕಟ್ಟು, "VimTextAreaView", B_FOLLOW_ALL_SIDES,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
	B_WILL_DRAW | B_ತುಂಬಿದ_UPDATE_ON_RESIZE | B_ಒಳಹರಿವು_ವಿಧಾನ_AWARE
#ಇಲ್ಲದಿದ್ದರೆ
	B_WILL_DRAW | B_ತುಂಬಿದ_UPDATE_ON_RESIZE
#ಆದರೆ_ಅಂತ್ಯ
	),
    mouse_Drag_Event_ಎಣಿಕೆ(0)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
    IMData.messenger = ಖಾಲಿ;
    IMData.ಸಂದೇಶ_2 = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    init(ಚೌಕಟ್ಟು);
}

Vim_ಪಠ್ಯ_Area_View::~Vim_ಪಠ್ಯ_Area_View()
{
    gui.vim_ಪಠ್ಯ_Area = ಖಾಲಿ;
}

    ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::init(BRect ಚೌಕಟ್ಟು)
{
    // set up global var for fast access
    gui.vim_ಪಠ್ಯ_Area = ಇದು;

    /*
     * Tell the app server not to erase the view: we will
     * fill it in completely by ourselves.
     * (Does this really work? Even if not, it won't harm either.)
     */
    ಗಣ_View_Color(B_TRANSPARENT_32_ಬಿಟ್);
#ವ್ಯಾಖ್ಯಾನಿಸು PEN_ಅಗಲ   1
    ಗಣ_Pen_ಗಾತ್ರ(PEN_ಅಗಲ);
#ವ್ಯಾಖ್ಯಾನಿಸು W_ಅಗಲ(curwin)   0
}

    ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::Draw(BRect updateRect)
{
    /*
     * XXX Other ports call here:
     * out_flush();	 * make sure all output has been processed *
     * but we can't do that, since it involves too much information
     * that is owned by other threads...
     */

    /*
     *	No need to use gui.vimWindow->Lock(): we are locked already.
     *	However, it would not hurt.
     */
    rgb_color rgb = GUI_ಗಮ್ಯ_RGB(gui.ಹಿಂಭಾಗ_pixel);
    ಗಣ_ಕೆಳಗಿನ_Color(rgb);
    FillRect(updateRect, B_SOLID_ಕೆಳಗಿನ);
    gui_redraw((ಪೂರ್ಣಾಂಕ) updateRect.ಎಡ, (ಪೂರ್ಣಾಂಕ) updateRect.ತುದಿ,
	    (ಪೂರ್ಣಾಂಕ) (updateRect.ಅಗಲ_2() + PEN_ಅಗಲ), (ಪೂರ್ಣಾಂಕ) (updateRect.ಎತ್ತರ_2() + PEN_ಅಗಲ));

    // Clear the border areas if needed
    ಗಣ_ಕೆಳಗಿನ_Color(rgb);

    ಆದರೆ (updateRect.ಎಡ < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.ಎಡ, updateRect.ತುದಿ,
		    FILL_X(0)-PEN_ಅಗಲ, updateRect.bottom), B_SOLID_ಕೆಳಗಿನ);
    ಆದರೆ (updateRect.ತುದಿ < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.ಎಡ, updateRect.ತುದಿ,
		    updateRect.ಬಲ, FILL_Y(0)-PEN_ಅಗಲ), B_SOLID_ಕೆಳಗಿನ);
    ಆದರೆ (updateRect.ಬಲ >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((ಪೂರ್ಣಾಂಕ)Columns), updateRect.ತುದಿ,
		    updateRect.ಬಲ, updateRect.bottom), B_SOLID_ಕೆಳಗಿನ);
    ಆದರೆ (updateRect.bottom >= FILL_Y(ಅಡ್ಡಸಾಲುಗಳು))   //  bottom border
	FillRect(BRect(updateRect.ಎಡ, FILL_Y((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು),
		    updateRect.ಬಲ, updateRect.bottom), B_SOLID_ಕೆಳಗಿನ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
    DrawIMString();
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::ಕೀಲಿ_ಕೆಳಕ್ಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, int32 ಅಂಕೆ_ಬೈಟುಗಳು_2)
{
    ರಚನೆ Vim_ಕೀಲಿ_ಸಂದೇಶ km;
    char_u *dest = km.chars;

    ತಾರ್ಕಿಕ canHaveVimModifiers = ಸುಳ್ಳು;

    BMessage *ಸಂದೇಶ = ಕಿಟಕಿ()->Current_ಸಂದೇಶ();
    ಪ್ರತಿಜ್ಞಾ(ಸಂದೇಶ);
    // msg->PrintToStream();

    /*
     * Convert special keys to Vim codes.
     * I think it is better to do it in the window thread
     * so we use at least a little bit of the potential
     * of our 2 CPUs. Besides, due to the fantastic mapping
     * of special keys to UTF-8, we have quite some work to
     * do...
     * TODO: I'm not quite happy with detection of special
     * keys. Perhaps I should use scan codes after all...
     */
    ಆದರೆ (ಅಂಕೆ_ಬೈಟುಗಳು_2 > 1) {
	// This cannot be a special key
	ಆದರೆ (ಅಂಕೆ_ಬೈಟುಗಳು_2 > ಕೀಲಿ_ಸಂದೇಶ_BUFSIZ)
	    ಅಂಕೆ_ಬೈಟುಗಳು_2 = ಕೀಲಿ_ಸಂದೇಶ_BUFSIZ;	    //	should never happen... ???
	km.ಉದ್ದಳತೆ = ಅಂಕೆ_ಬೈಟುಗಳು_2;
	ಸ್ಮರಣೆ_ನಕಲು((ಅಕ್ಷರ *)dest, ಬೈಟುಗಳು, ಅಂಕೆ_ಬೈಟುಗಳು_2);
	km.csi_escape = ನಿಜ;
    } ಇಲ್ಲದಿದ್ದರೆ {
	int32 scancode = 0;
	ಸಂದೇಶ->ಕಂಡುಹಿಡಿ_Int32("key", &scancode);

	int32 beModifiers = 0;
	ಸಂದೇಶ->ಕಂಡುಹಿಡಿ_Int32("modifiers", &beModifiers);

	char_u string[3];
	ಪೂರ್ಣಾಂಕ len = 0;
	km.ಉದ್ದಳತೆ = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	ಪ್ರತಿಜ್ಞಾ(B_BACKSPACE <= 0x20);
	ಪ್ರತಿಜ್ಞಾ(B_DELETE == 0x7F);
	ಆದರೆ (((char_u)ಬೈಟುಗಳು[0] <= 0x20 || (char_u)ಬೈಟುಗಳು[0] == 0x7F) &&
		ಅಂಕೆ_ಬೈಟುಗಳು_2 == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    ಆದರೆ (beModifiers & B_CONTROL_ಕೀಲಿ) {
		ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = ಕೀಲಿ_Map->ಸಾಮಾನ್ಯ_map[scancode];
		ಪೂರ್ಣಾಂಕ new_ಅಂಕೆ_ಬೈಟುಗಳು = ಕೀಲಿ_Map_Chars[ಸೂಚ್ಯಂಕ];
		char_u *new_ಬೈಟುಗಳು = (char_u *)&ಕೀಲಿ_Map_Chars[ಸೂಚ್ಯಂಕ + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		ಆದರೆ (new_ಅಂಕೆ_ಬೈಟುಗಳು != 1 || (*new_ಬೈಟುಗಳು > 0x20 &&
			    *new_ಬೈಟುಗಳು != 0x7F )) {
		    ಹೋಗು notspecial;
		}
		ಬೈಟುಗಳು = (ಅಕ್ಷರ *)new_ಬೈಟುಗಳು;
	    }
	    canHaveVimModifiers = ನಿಜ;

	    uint16 beoskey;
	    ಪೂರ್ಣಾಂಕ ಮೊದಲ, ಕೊನೆಯ;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    ಆದರೆ (ಅಂಕೆ_ಬೈಟುಗಳು_2 == 0 || ಬೈಟುಗಳು[0] == B_FUNCTION_ಕೀಲಿ) {
		beoskey = F(scancode);
		ಮೊದಲ = ಮೊದಲ_FUNCTION_ಕೀಲಿ;
		ಕೊನೆಯ = ಅಂಕೆ_SPECIAL_ಕೀಲಿಗಳು;
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಬೈಟುಗಳು == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		ಮೊದಲ = ಕೊನೆಯ = 0;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		beoskey = K(ಬೈಟುಗಳು[0]);
		ಮೊದಲ = 0;
		ಕೊನೆಯ = ಮೊದಲ_FUNCTION_ಕೀಲಿ;
	    }

	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = ಮೊದಲ; i < ಕೊನೆಯ; i++) {
		ಆದರೆ (special_ಕೀಲಿಗಳು[i].Be_ಕೀಲಿಗಳು == beoskey) {
		    string[0] = CSI;
		    string[1] = special_ಕೀಲಿಗಳು[i].vim_code0;
		    string[2] = special_ಕೀಲಿಗಳು[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	ಆದರೆ (len == 0) {
	    string[0] = ಬೈಟುಗಳು[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#ಆದರೆ 0
	ಆದರೆ (len == 3 ||
		ಬೈಟುಗಳು[0] == B_SPACE || ಬೈಟುಗಳು[0] == B_TAB ||
		ಬೈಟುಗಳು[0] == B_RETURN || ಬೈಟುಗಳು[0] == '\r' ||
		ಬೈಟುಗಳು[0] == B_ESCAPE)
#ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (canHaveVimModifiers)
#ಆದರೆ_ಅಂತ್ಯ
	    {
		ಪೂರ್ಣಾಂಕ modifiers;
		modifiers = 0;
		ಆದರೆ (beModifiers & B_ಸರಿಸು_ಕೀಲಿ)
		    modifiers |= MOD_ಮುಖವಾಡ_ಸರಿಸು;
		ಆದರೆ (beModifiers & B_CONTROL_ಕೀಲಿ)
		    modifiers |= MOD_ಮುಖವಾಡ_CTRL;
		ಆದರೆ (beModifiers & B_OPTION_ಕೀಲಿ)
		    modifiers |= MOD_ಮುಖವಾಡ_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		ಪೂರ್ಣಾಂಕ ಕೀಲಿ;
		ಆದರೆ (string[0] == CSI && len == 3)
		    ಕೀಲಿ = ಗಮ್ಯ_SPECIAL(string[1], string[2]);
		ಇಲ್ಲದಿದ್ದರೆ
		    ಕೀಲಿ = string[0];
		ಕೀಲಿ = simplify_ಕೀಲಿ(ಕೀಲಿ, &modifiers);
		ಆದರೆ (IS_SPECIAL(ಕೀಲಿ))
		{
		    string[0] = CSI;
		    string[1] = K_ಎರಡನೇ(ಕೀಲಿ);
		    string[2] = K_ಮೂರನೇ(ಕೀಲಿ);
		    len = 3;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    string[0] = ಕೀಲಿ;
		    len = 1;
		}

		ಆದರೆ (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.ಉದ್ದಳತೆ = 3;
		}
	    }
	ಸ್ಮರಣೆ_ನಕಲು((ಅಕ್ಷರ *)dest, string, len);
	km.ಉದ್ದಳತೆ += len;
	km.csi_escape = ಸುಳ್ಳು;
    }

    write_ಪೋರ್ಟ್(gui.vdcmp, Vim_ಸಂದೇಶ::ಕೀಲಿ_2, &km, ಗಾತ್ರ(km));

    /*
     * blank out the pointer if necessary
     */
    ಆದರೆ (p_mh && !gui.ಸೂಚಕ_hidden)
    {
	guiBlankMouse(ನಿಜ);
	gui.ಸೂಚಕ_hidden = TRUE;
    }
}
ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::guiSendMouseEvent(
	ಪೂರ್ಣಾಂಕ	button,
	ಪೂರ್ಣಾಂಕ	x,
	ಪೂರ್ಣಾಂಕ	y,
	ಪೂರ್ಣಾಂಕ	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_ಸಂದೇಶ mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_ಪೋರ್ಟ್(gui.vdcmp, Vim_ಸಂದೇಶ::Mouse, &mm, ಗಾತ್ರ(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    ಆದರೆ (gui.ಸೂಚಕ_hidden)
    {
	guiBlankMouse(ಸುಳ್ಳು);
	gui.ಸೂಚಕ_hidden = FALSE;
    }
}

ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::guiMouseMoved(
	ಪೂರ್ಣಾಂಕ	x,
	ಪೂರ್ಣಾಂಕ	y)
{
    Vim_Mouse_Moved_ಸಂದೇಶ mm;

    mm.x = x;
    mm.y = y;

    write_ಪೋರ್ಟ್(gui.vdcmp, Vim_ಸಂದೇಶ::MouseMoved, &mm, ಗಾತ್ರ(mm));

    ಆದರೆ (gui.ಸೂಚಕ_hidden)
    {
	guiBlankMouse(ಸುಳ್ಳು);
	gui.ಸೂಚಕ_hidden = FALSE;
    }
}

    ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::guiBlankMouse(ತಾರ್ಕಿಕ should_hide)
{
    ಆದರೆ (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } ಇಲ್ಲದಿದ್ದರೆ {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
Vim_ಪಠ್ಯ_Area_View::mouse_Modifiers_ಗಮ್ಯ_Vim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    ಆದರೆ (beModifiers & B_ಸರಿಸು_ಕೀಲಿ)
	vim_modifiers |= MOUSE_ಸರಿಸು;
    ಆದರೆ (beModifiers & B_CONTROL_ಕೀಲಿ)
	vim_modifiers |= MOUSE_CTRL;
    ಆದರೆ (beModifiers & B_OPTION_ಕೀಲಿ)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    ಹಿಂತಿರುಗಿ vim_modifiers;
}

    ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::Mouse_ಕೆಳಕ್ಕೆ(BPoint ಬಿಂದು)
{
    BMessage *m = ಕಿಟಕಿ()->Current_ಸಂದೇಶ();
    ಪ್ರತಿಜ್ಞಾ(m);

    int32 buttons = 0;
    m->ಕಂಡುಹಿಡಿ_Int32("buttons", &buttons);

    ಪೂರ್ಣಾಂಕ vimButton;

    ಆದರೆ (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_ಎಡ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_ಬಲ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_ಮಧ್ಯ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ;		// Unknown button

    vimMouseButton = 1;	    // don't care which one

    // Handle multiple clicks
    int32 clicks = 0;
    m->ಕಂಡುಹಿಡಿ_Int32("clicks", &clicks);

    int32 modifiers = 0;
    m->ಕಂಡುಹಿಡಿ_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_ಗಮ್ಯ_Vim(modifiers);

    guiSendMouseEvent(vimButton, ಬಿಂದು.x, ಬಿಂದು.y,
	    clicks > 1 /* = repeated_click*/, vimMouseModifiers);
}

    ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::Mouse_ಮೇಲೆ(BPoint ಬಿಂದು)
{
    vimMouseButton = 0;

    BMessage *m = ಕಿಟಕಿ()->Current_ಸಂದೇಶ();
    ಪ್ರತಿಜ್ಞಾ(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->ಕಂಡುಹಿಡಿ_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_ಗಮ್ಯ_Vim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, ಬಿಂದು.x, ಬಿಂದು.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::Mouse_ಮೇಲೆ(ಬಿಂದು);
}

    ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::MouseMoved(BPoint ಬಿಂದು, uint32 transit, ಸ್ಥಿರಾಂಕ BMessage *ಸಂದೇಶ_2)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    ಆದರೆ (gui.ಸೂಚಕ_hidden)
    {
	guiBlankMouse(ಸುಳ್ಳು);
	gui.ಸೂಚಕ_hidden = FALSE;
    }

    ಆದರೆ (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(ಬಿಂದು.x, ಬಿಂದು.y);
	ಹಿಂತಿರುಗಿ;
    }

    atomic_ಕೂಡಿಸು(&mouse_Drag_Event_ಎಣಿಕೆ, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, ಬಿಂದು.x, ಬಿಂದು.y, 0, vimMouseModifiers);
}

    ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::ಸಂದೇಶ_Received(BMessage *m)
{
    ಆಯ್ಕೆ (m->what) {
	ಸ್ಥಿತಿ 'menu':
	    {
		Vim_Menu_ಸಂದೇಶ mm;
		mm.guiMenu = ಖಾಲಿ;  // in case no pointer in msg
		m->ಕಂಡುಹಿಡಿ_ಸೂಚಕ("VimMenu", (ಶೂನ್ಯ **)&mm.guiMenu);
		write_ಪೋರ್ಟ್(gui.vdcmp, Vim_ಸಂದೇಶ::Menu, &mm, ಗಾತ್ರ(mm));
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ B_MOUSE_WHEEL_ಬದಲಾಯಿತು:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		ದಶಮಾಂಶ small=0, big=0, dy=0;
		m->ಕಂಡುಹಿಡಿ_Float("be:wheel_delta_y", &dy);
		scb->Get_ಹೆಜ್ಜೆಗಳು(&small, &big);
		scb->ಗಣ_ಮೌಲ್ಯ(scb->ಮೌಲ್ಯ_2()+small*dy*3);
		scb->ಮೌಲ್ಯ_ಬದಲಾಯಿತು_2(scb->ಮೌಲ್ಯ_2());
#ಆದರೆ 0
		scb = curwin->w_scrollbars[0].id;
		scb->Get_ಹೆಜ್ಜೆಗಳು(&small, &big);
		scb->ಗಣ_ಮೌಲ್ಯ(scb->ಮೌಲ್ಯ_2()+small*dy);
		scb->ಮೌಲ್ಯ_ಬದಲಾಯಿತು_2(scb->ಮೌಲ್ಯ_2());
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
	ಸ್ಥಿತಿ B_ಒಳಹರಿವು_ವಿಧಾನ_EVENT:
	    {
		int32 opcode;
		m->ಕಂಡುಹಿಡಿ_Int32("be:opcode", &opcode);
		ಆಯ್ಕೆ(opcode)
		{
		    ಸ್ಥಿತಿ B_ಒಳಹರಿವು_ವಿಧಾನ_STARTED:
			ಆದರೆ (!IMData.messenger) ಅಳಿಸು IMData.messenger;
			IMData.messenger = ಹೊಸ BMessenger();
			m->ಕಂಡುಹಿಡಿ_Messenger("be:reply_to", IMData.messenger);
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ B_ಒಳಹರಿವು_ವಿಧಾನ_ಬದಲಾಯಿತು:
			{
			    BString str;
			    ತಾರ್ಕಿಕ confirmed;
			    ಆದರೆ (IMData.ಸಂದೇಶ_2) *(IMData.ಸಂದೇಶ_2) = *m;
			    ಇಲ್ಲದಿದ್ದರೆ	       IMData.ಸಂದೇಶ_2 = ಹೊಸ BMessage(*m);
			    DrawIMString();
			    m->ಕಂಡುಹಿಡಿ_Bool("be:confirmed", &confirmed);
			    ಆದರೆ (confirmed)
			    {
				m->ಕಂಡುಹಿಡಿ_String("be:string", &str);
				char_u *chars = (char_u*)str.String();
				ರಚನೆ Vim_ಕೀಲಿ_ಸಂದೇಶ km;
				km.csi_escape = ನಿಜ;
				ಪೂರ್ಣಾಂಕ clen;
				ಪೂರ್ಣಾಂಕ i = 0;
				ಆಗ (i < str.ಉದ್ದಳತೆ_2())
				{
				    clen = utf_ptr2len(chars+i);
				    ಸ್ಮರಣೆ_ನಕಲು(km.chars, chars+i, clen);
				    km.ಉದ್ದಳತೆ = clen;
				    write_ಪೋರ್ಟ್(gui.vdcmp, Vim_ಸಂದೇಶ::ಕೀಲಿ_2, &km, ಗಾತ್ರ(km));
				    i += clen;
				}
			    }
			}
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ B_ಒಳಹರಿವು_ವಿಧಾನ_LOCATION_REQUEST:
			{
			    BMessage ಸಂದೇಶ(B_ಒಳಹರಿವು_ವಿಧಾನ_EVENT);
			    ಸಂದೇಶ.ಕೂಡಿಸು_Int32("be:opcode", B_ಒಳಹರಿವು_ವಿಧಾನ_LOCATION_REQUEST);
			    ಸಂದೇಶ.ಕೂಡಿಸು_ಬಿಂದು("be:location_reply", IMData.location);
			    ಸಂದೇಶ.ಕೂಡಿಸು_Float("be:height_reply", FILL_Y(1));
			    IMData.messenger->Send_ಸಂದೇಶ(&ಸಂದೇಶ);
			}
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ B_ಒಳಹರಿವು_ವಿಧಾನ_STOPPED:
			ಅಳಿಸು IMData.messenger;
			ಅಳಿಸು IMData.ಸಂದೇಶ_2;
			IMData.messenger = ಖಾಲಿ;
			IMData.ಸಂದೇಶ_2 = ಖಾಲಿ;
			ನಿಲ್ಲಿಸು;
		}
	    }
	    // TODO: sz: break here???
#ಆದರೆ_ಅಂತ್ಯ
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಆದರೆ (m->WasDropped()) {
		BWindow *w = ಕಿಟಕಿ();
		w->Detach_Current_ಸಂದೇಶ();
		w->Minimize(ಸುಳ್ಳು);
		VimApp::SendRefs(m, (modifiers() & B_ಸರಿಸು_ಕೀಲಿ) != 0);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		Inherited::ಸಂದೇಶ_Received(m);
	    }
	    ನಿಲ್ಲಿಸು;
    }
}

    ಪೂರ್ಣಾಂಕ
Vim_ಪಠ್ಯ_Area_View::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    ಆದರೆ (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_ಗಣ_font((GuiFont)newFont);
	ಆದರೆ (name && STRCMP(name, "*") != 0)
	    hl_ಗಣ_font_name(name);

	ಗಣ_Drawing_ಬಹುಲಕ(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	ಹಿಂತಿರುಗಿ OK;
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::mchDrawString(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, char_u *s, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    ಆದರೆ (!(flags & DRAW_TRANSP)) {
	ಪೂರ್ಣಾಂಕ cells;
	cells = 0;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i=0; i<len; i++) {
	    ಪೂರ್ಣಾಂಕ cn = utf_ptr2cells((char_u *)(s+i));
	    ಆದರೆ (cn<4) cells += cn;
	}

	BRect r(FILL_X(ಲಂಬಸಾಲು), FILL_Y(ಅಡ್ಡಸಾಲು),
		FILL_X(ಲಂಬಸಾಲು + cells) - PEN_ಅಗಲ, FILL_Y(ಅಡ್ಡಸಾಲು + 1) - PEN_ಅಗಲ);
	FillRect(r, B_SOLID_ಕೆಳಗಿನ);
    }

    BFont font;
    ಇದು->GetFont(&font);
    ಆದರೆ (!font.IsFixed())
    {
	ಅಕ್ಷರ* p = (ಅಕ್ಷರ*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	ಪೂರ್ಣಾಂಕ cells;
	ಆಗ ((p - (ಅಕ್ಷರ*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.ಗಣ(ಪಠ್ಯ_X(ಲಂಬಸಾಲು+lastpos), ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು));
	    DrawString(p, clen, where);
	    ಆದರೆ (flags & DRAW_BOLD) {
		where.x += 1.0;
		ಗಣ_Drawing_ಬಹುಲಕ(B_OP_BLEND);
		DrawString(p, clen, where);
		ಗಣ_Drawing_ಬಹುಲಕ(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    ಆದರೆ (cells<4) lastpos += cells;
	    ಇಲ್ಲದಿದ್ದರೆ	lastpos++;
	    p += clen;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	BPoint where(ಪಠ್ಯ_X(ಲಂಬಸಾಲು), ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು));
	DrawString((ಅಕ್ಷರ*)s, len, where);
	ಆದರೆ (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    ಗಣ_Drawing_ಬಹುಲಕ(B_OP_BLEND);
	    DrawString((ಅಕ್ಷರ*)s, len, where);
	    ಗಣ_Drawing_ಬಹುಲಕ(B_OP_COPY);
	}
    }

    ಆದರೆ (flags & DRAW_UNDERL) {
	ಪೂರ್ಣಾಂಕ cells;
	cells = 0;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i=0; i<len; i++) {
	    ಪೂರ್ಣಾಂಕ cn = utf_ptr2cells((char_u *)(s+i));
	    ಆದರೆ (cn<4) cells += cn;
	}

	BPoint ಆರಂಭ(FILL_X(ಲಂಬಸಾಲು), FILL_Y(ಅಡ್ಡಸಾಲು + 1) - PEN_ಅಗಲ);
	BPoint ಕೊನೆ(FILL_X(ಲಂಬಸಾಲು + cells) - PEN_ಅಗಲ, ಆರಂಭ.y);

	Stroke_ಸಾಲು(ಆರಂಭ, ಕೊನೆ);
    }
}

ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::mch_Clear_ಖಂಡ(
	ಪೂರ್ಣಾಂಕ	row1,
	ಪೂರ್ಣಾಂಕ	col1,
	ಪೂರ್ಣಾಂಕ	row2,
	ಪೂರ್ಣಾಂಕ	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_ಅಗಲ, FILL_Y(row2 + 1) - PEN_ಅಗಲ);
    gui_mch_ಗಣ_bg_color(gui.ಹಿಂಭಾಗ_pixel);
    FillRect(r, B_SOLID_ಕೆಳಗಿನ);
}

    ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::mchClearAll()
{
    gui_mch_ಗಣ_bg_color(gui.ಹಿಂಭಾಗ_pixel);
    FillRect(Bounds(), B_SOLID_ಕೆಳಗಿನ);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::mch_Delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    BRect ಮೂಲ, dest;
    ಮೂಲ.ಎಡ = FILL_X(gui.scroll_region_ಎಡ);
    ಮೂಲ.ತುದಿ = FILL_Y(ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು);
    ಮೂಲ.ಬಲ = FILL_X(gui.scroll_region_ಬಲ + 1) - PEN_ಅಗಲ;
    ಮೂಲ.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_ಅಗಲ;

    dest.ಎಡ = FILL_X(gui.scroll_region_ಎಡ);
    dest.ತುದಿ = FILL_Y(ಅಡ್ಡಸಾಲು);
    dest.ಬಲ = FILL_X(gui.scroll_region_ಬಲ + 1) - PEN_ಅಗಲ;
    dest.bottom = FILL_Y(gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1) - PEN_ಅಗಲ;

    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	// Clear one column more for when bold has spilled over
	Copy_ಬಿಟ್ಟುಗಳು(ಮೂಲ, dest);
	gui_clear_ಖಂಡ(gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1,
		gui.scroll_region_ಎಡ,
		gui.scroll_region_bot, gui.scroll_region_ಬಲ);


	gui.vim_ಕಿಟಕಿ->Unlock();
	/*
	 * The Draw() callback will be called now if some of the source
	 * bits were not in the visible region.
	 */
    }
    // gui_x11_check_copy_area();
    //	}
}

/*
 * mchInsertLines() Lock()s the window by itself.
 */
    ಶೂನ್ಯ
Vim_ಪಠ್ಯ_Area_View::mch_ಸೇರಿಸು_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    BRect ಮೂಲ, dest;

    // XXX Attempt at a hack:
    gui.vim_ಕಿಟಕಿ->UpdateIfNeeded();
    ಮೂಲ.ಎಡ = FILL_X(gui.scroll_region_ಎಡ);
    ಮೂಲ.ತುದಿ = FILL_Y(ಅಡ್ಡಸಾಲು);
    ಮೂಲ.ಬಲ = FILL_X(gui.scroll_region_ಬಲ + 1) - PEN_ಅಗಲ;
    ಮೂಲ.bottom = FILL_Y(gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1) - PEN_ಅಗಲ;

    dest.ಎಡ = FILL_X(gui.scroll_region_ಎಡ);
    dest.ತುದಿ = FILL_Y(ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು);
    dest.ಬಲ = FILL_X(gui.scroll_region_ಬಲ + 1) - PEN_ಅಗಲ;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_ಅಗಲ;

    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	// Clear one column more for when bold has spilled over
	Copy_ಬಿಟ್ಟುಗಳು(ಮೂಲ, dest);
	gui_clear_ಖಂಡ(ಅಡ್ಡಸಾಲು, gui.scroll_region_ಎಡ,
		ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು - 1, gui.scroll_region_ಬಲ);

	gui.vim_ಕಿಟಕಿ->Unlock();
	/*
	 * The Draw() callback will be called now if some of the source
	 * bits were not in the visible region.
	 * However, if we scroll too fast it can't keep up and the
	 * update region gets messed up. This seems to be because copying
	 * un-Draw()n bits does not generate Draw() calls for the copy...
	 * I moved the hack to before the CopyBits() to reduce the
	 * amount of additional waiting needed.
	 */

	// gui_x11_check_copy_area();

    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
/*
 * DrawIMString draws string with IMData.message.
 */
ಶೂನ್ಯ Vim_ಪಠ್ಯ_Area_View::DrawIMString(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s;
    ಪೂರ್ಣಾಂಕ len;
    BMessage* ಸಂದೇಶ = IMData.ಸಂದೇಶ_2;
    ಆದರೆ (!ಸಂದೇಶ)
	ಹಿಂತಿರುಗಿ;
    gui_redraw_ಖಂಡ(IMData.ಅಡ್ಡಸಾಲು, 0,
	    IMData.ಅಡ್ಡಸಾಲು + IMData.ಎಣಿಕೆ_2, W_ಅಗಲ(curwin), GUI_MON_NOCLEAR);
    ತಾರ್ಕಿಕ confirmed = ಸುಳ್ಳು;
    ಸಂದೇಶ->ಕಂಡುಹಿಡಿ_Bool("be:confirmed", &confirmed);
    ಆದರೆ (confirmed)
	ಹಿಂತಿರುಗಿ;
    rgb_color hcolor = ಮೇಲಿನ_Color(), lcolor = ಕೆಳಗಿನ_Color();
    ಸಂದೇಶ->ಕಂಡುಹಿಡಿ_String("be:string", &str);
    s = str.String();
    len = str.ಉದ್ದಳತೆ_2();
    ಗಣ_ಮೇಲಿನ_Color(0, 0, 0);
    IMData.ಅಡ್ಡಸಾಲು = gui.ಅಡ್ಡಸಾಲು;
    IMData.ಲಂಬಸಾಲು = gui.ಲಂಬಸಾಲು;
    int32 sel_ಆರಂಭ = 0, sel_ಕೊನೆ = 0;
    ಸಂದೇಶ->ಕಂಡುಹಿಡಿ_Int32("be:selection", 0, &sel_ಆರಂಭ);
    ಸಂದೇಶ->ಕಂಡುಹಿಡಿ_Int32("be:selection", 1, &sel_ಕೊನೆ);
    ಪೂರ್ಣಾಂಕ clen, cn;
    BPoint pos(IMData.ಲಂಬಸಾಲು, 0);
    BRect r;
    BPoint where;
    IMData.location = Convert_ಗಮ್ಯ_Screen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.ಅಡ್ಡಸಾಲು + pos.y)));
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	ಆದರೆ (pos.x + cn > W_ಅಗಲ(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	ಆದರೆ (sel_ಆರಂಭ<=i && i<sel_ಕೊನೆ)
	{
	    ಗಣ_ಕೆಳಗಿನ_Color(r_highlight);
	    IMData.location = Convert_ಗಮ್ಯ_Screen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.ಅಡ್ಡಸಾಲು + pos.y)));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಣ_ಕೆಳಗಿನ_Color(b_highlight);
	}
	r.ಗಣ(FILL_X(pos.x), FILL_Y(IMData.ಅಡ್ಡಸಾಲು + pos.y),
		FILL_X(pos.x + cn) - PEN_ಅಗಲ,
		FILL_Y(IMData.ಅಡ್ಡಸಾಲು + pos.y + 1) - PEN_ಅಗಲ);
	FillRect(r, B_SOLID_ಕೆಳಗಿನ);
	where.ಗಣ(ಪಠ್ಯ_X(pos.x), ಪಠ್ಯ_Y(IMData.ಅಡ್ಡಸಾಲು + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.ಎಣಿಕೆ_2 = (ಪೂರ್ಣಾಂಕ)pos.y;

    ಗಣ_ಮೇಲಿನ_Color(hcolor);
    ಗಣ_ಕೆಳಗಿನ_Color(lcolor);
}
#ಆದರೆ_ಅಂತ್ಯ
// ---------------- VimScrollBar ----------------

/*
 * BUG: XXX
 * It seems that BScrollBar determine their direction not from
 * "posture" but from if they are "tall" or "wide" in shape...
 *
 * Also, place them out of sight, because Vim enables them before
 * they are positioned.
 */
VimScrollBar::VimScrollBar(scrollbar_T *g, orientation posture):
    BScrollBar(posture == B_HORIZONTAL ?  BRect(-100,-100,-10,-90) :
	    BRect(-100,-100,-90,-10),
	    "vim scrollbar", (BView *)ಖಾಲಿ,
	    0.0, 10.0, posture),
    ignore_ಮೌಲ್ಯ(-1),
    scroll_Event_ಎಣಿಕೆ(0)
{
    gsb = g;
    ಗಣ_Resizing_ಬಹುಲಕ(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    ಶೂನ್ಯ
VimScrollBar::ಮೌಲ್ಯ_ಬದಲಾಯಿತು_2(ದಶಮಾಂಶ new_ಮೌಲ್ಯ)
{
    ಆದರೆ (ignore_ಮೌಲ್ಯ >= 0.0 && new_ಮೌಲ್ಯ == ignore_ಮೌಲ್ಯ) {
	ignore_ಮೌಲ್ಯ = -1;
	ಹಿಂತಿರುಗಿ;
    }
    ignore_ಮೌಲ್ಯ = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_ಕೂಡಿಸು(&scroll_Event_ಎಣಿಕೆ, 1);

    ರಚನೆ Vim_Scroll_Bar_ಸಂದೇಶ sm;

    sm.sb = ಇದು;
    sm.ಮೌಲ್ಯ = (ದೀರ್ಘ) new_ಮೌಲ್ಯ;
    sm.stillDragging = TRUE;

    write_ಪೋರ್ಟ್(gui.vdcmp, Vim_ಸಂದೇಶ::ScrollBar, &sm, ಗಾತ್ರ(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    ಶೂನ್ಯ
VimScrollBar::Mouse_ಮೇಲೆ(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_ಕೂಡಿಸು(&scroll_Event_ಎಣಿಕೆ, 1);

    ರಚನೆ Vim_Scroll_Bar_ಸಂದೇಶ sm;

    sm.sb = ಇದು;
    sm.ಮೌಲ್ಯ = (ದೀರ್ಘ) ಮೌಲ್ಯ_2();
    sm.stillDragging = FALSE;

    write_ಪೋರ್ಟ್(gui.vdcmp, Vim_ಸಂದೇಶ::ScrollBar, &sm, ಗಾತ್ರ(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::Mouse_ಮೇಲೆ(where);
}

    ಶೂನ್ಯ
VimScrollBar::ಗಣ_ಮೌಲ್ಯ(ದಶಮಾಂಶ new_ಮೌಲ್ಯ)
{
    ಆದರೆ (new_ಮೌಲ್ಯ == ಮೌಲ್ಯ_2())
	ಹಿಂತಿರುಗಿ;

    ignore_ಮೌಲ್ಯ = new_ಮೌಲ್ಯ;
    Inherited::ಗಣ_ಮೌಲ್ಯ(new_ಮೌಲ್ಯ);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(ಸ್ಥಿರಾಂಕ VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ಸ್ಥಿರಾಂಕ BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ಸ್ಥಿರಾಂಕ VimFont &rhs): BFont(rhs)
{
    init();
}

VimFont::~VimFont()
{
}

    ಶೂನ್ಯ
VimFont::init()
{
    ಮುಂದಿನ = ಖಾಲಿ;
    refcount = 1;
    name = ಖಾಲಿ;
}

// ---------------- VimDialog ----------------

#ಆದರೆ defined(FEAT_GUI_DIALOG)

ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  k_Vim_Dialog_Button_ಸಂದೇಶ = 'VMDB';
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  k_Vim_Dialog_Icon_Stripe_ಅಗಲ = 30;
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  kVimDialogButtonsSpacingX = 9;
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  kVimDialogButtonsSpacingY = 4;
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  kVimDialogSpacingX = 6;
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  kVimDialogSpacingY = 10;
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  k_Vim_Dialog_ಕನಿಷ್ಠತಮ_ಅಗಲ  = 310;
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  k_Vim_Dialog_ಕನಿಷ್ಠತಮ_ಎತ್ತರ = 75;
ಸ್ಥಿರಾಂಕ BRect	    kDefaultRect =
BRect(0, 0, k_Vim_Dialog_ಕನಿಷ್ಠತಮ_ಅಗಲ, k_Vim_Dialog_ಕನಿಷ್ಠತಮ_ಎತ್ತರ);

VimDialog::VimDialog(ಪೂರ್ಣಾಂಕ type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *title, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ_2,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buttons, ಪೂರ್ಣಾಂಕ dfltbutton, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *textfield, ಪೂರ್ಣಾಂಕ ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_ಕಿಟಕಿ_LOOK, B_MODAL_APP_ಕಿಟಕಿ_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_ಮೌಲ್ಯ(dfltbutton)
    , f_ಸಂದೇಶ_View(ಖಾಲಿ)
    , f_ಒಳಹರಿವು_Control(ಖಾಲಿ)
    , f_ಒಳಹರಿವು_ಮೌಲ್ಯ(textfield)
{
    //	master view
    VimDialog::View* view = ಹೊಸ VimDialog::View(Bounds());
    ಆದರೆ (view == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (title == ಖಾಲಿ)
	ಗಣ_Title("Vim " VIM_ಆವೃತ್ತಿ_MEDIUM);

    ಕೂಡಿಸು_ಮಗು(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    ದಶಮಾಂಶ max_Button_ಅಗಲ  = 0;
    ದಶಮಾಂಶ max_Button_ಎತ್ತರ = 0;
    ದಶಮಾಂಶ buttons_ಅಗಲ	  = 0;
    ದಶಮಾಂಶ buttons_ಎತ್ತರ   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    ಮಾಡು
    {
	int32 ಕೊನೆ = strButtons.ಕಂಡುಹಿಡಿ_ಮೊದಲ('\n');
	ಆದರೆ (ಕೊನೆ != B_ERROR)
	    strButtons.ಗಣ_Byte_At(ಕೊನೆ, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->ಕೂಡಿಸು_ಮಗು(button);
	fButtonsList.ಕೂಡಿಸು_ಅಂಶ(button);

	max_Button_ಅಗಲ	= max_c(max_Button_ಅಗಲ,  button->Bounds().ಅಗಲ_2());
	max_Button_ಎತ್ತರ = max_c(max_Button_ಎತ್ತರ, button->Bounds().ಎತ್ತರ_2());
	buttons_ಅಗಲ   += button->Bounds().ಅಗಲ_2();
	buttons_ಎತ್ತರ  += button->Bounds().ಎತ್ತರ_2();

	ಆದರೆ (ಕೊನೆ == B_ERROR)
	    ನಿಲ್ಲಿಸು;

	strButtons.Remove(0, ಕೊನೆ + 1);
    } ಆಗ (ನಿಜ);

    int32 buttons_ಎಣಿಕೆ = fButtonsList.ಎಣಿಕೆ_ವಸ್ತುಗಳು();
    buttons_ಅಗಲ      += kVimDialogButtonsSpacingX * (buttons_ಎಣಿಕೆ - 1);
    buttons_ಎತ್ತರ     += kVimDialogButtonsSpacingY * (buttons_ಎಣಿಕೆ - 1);
    ದಶಮಾಂಶ dialog_ಅಗಲ  = buttons_ಅಗಲ + k_Vim_Dialog_Icon_Stripe_ಅಗಲ +
	kVimDialogSpacingX * 2;
    ದಶಮಾಂಶ dialog_ಎತ್ತರ = max_Button_ಎತ್ತರ + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    ತಾರ್ಕಿಕ vertical = (vim_strchr(p_go, GO_VERTICAL) != ಖಾಲಿ) ||
	dialog_ಅಗಲ >= gui.vim_ಕಿಟಕಿ->Bounds().ಅಗಲ_2();
    ಆದರೆ (vertical) {
	dialog_ಅಗಲ  -= buttons_ಅಗಲ;
	dialog_ಅಗಲ  += max_Button_ಅಗಲ;
	dialog_ಎತ್ತರ -= max_Button_ಎತ್ತರ;
	dialog_ಎತ್ತರ += buttons_ಎತ್ತರ;
    }

    dialog_ಅಗಲ  = max_c(dialog_ಅಗಲ,  k_Vim_Dialog_ಕನಿಷ್ಠತಮ_ಅಗಲ);

    //	message view
    BRect rect(0, 0, dialog_ಅಗಲ, 0);
    rect.ಎಡ  += k_Vim_Dialog_Icon_Stripe_ಅಗಲ + 16 + kVimDialogSpacingX;
    rect.ತುದಿ   += kVimDialogSpacingY;
    rect.ಬಲ -= kVimDialogSpacingX;
    rect.bottom = rect.ತುದಿ;
    f_ಸಂದೇಶ_View = ಹೊಸ BTextView(rect, "_tv_", rect.ಸ್ಥಾನಾಂತರ_By_Copy(B_ORIGIN),
	    B_FOLLOW_ಎಡ | B_FOLLOW_ತುದಿ, B_WILL_DRAW);

    f_ಸಂದೇಶ_View->ಗಣ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color ಪಠ್ಯ_Color = ui_color(B_PANEL_ಪಠ್ಯ_COLOR);
    f_ಸಂದೇಶ_View->ಗಣ_Font_And_Color(be_plain_font, B_FONT_ALL, &ಪಠ್ಯ_Color);
    f_ಸಂದೇಶ_View->ಗಣ_ಪಠ್ಯ(ಸಂದೇಶ_2);
    f_ಸಂದೇಶ_View->MakeEditable(ಸುಳ್ಳು);
    f_ಸಂದೇಶ_View->MakeSelectable(ಸುಳ್ಳು);
    f_ಸಂದೇಶ_View->ಗಣ_ಶಬ್ದ_Wrap(ನಿಜ);
    ಕೂಡಿಸು_ಮಗು(f_ಸಂದೇಶ_View);

    ದಶಮಾಂಶ ಸಂದೇಶ_ಎತ್ತರ = f_ಸಂದೇಶ_View->ಪಠ್ಯ_ಎತ್ತರ(0, f_ಸಂದೇಶ_View->ಎಣಿಕೆ_ಸಾಲುಗಳು());
    f_ಸಂದೇಶ_View->ResizeBy(0, ಸಂದೇಶ_ಎತ್ತರ);
    f_ಸಂದೇಶ_View->ಗಣ_ಪಠ್ಯ_Rect(BRect(0, 0, rect.ಅಗಲ_2(), ಸಂದೇಶ_ಎತ್ತರ));

    dialog_ಎತ್ತರ += ಸಂದೇಶ_ಎತ್ತರ;

    //	input view
    ಆದರೆ (f_ಒಳಹರಿವು_ಮೌಲ್ಯ != ಖಾಲಿ) {
	rect.ತುದಿ     =
	    rect.bottom += ಸಂದೇಶ_ಎತ್ತರ + kVimDialogSpacingY;
	f_ಒಳಹರಿವು_Control = ಹೊಸ BTextControl(rect, "_iv_", ಖಾಲಿ, f_ಒಳಹರಿವು_ಮೌಲ್ಯ, ಖಾಲಿ,
		B_FOLLOW_ಎಡ | B_FOLLOW_ತುದಿ, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_ಒಳಹರಿವು_Control->ಪಠ್ಯ_View()->ಗಣ_ಪಠ್ಯ(f_ಒಳಹರಿವು_ಮೌಲ್ಯ);
	f_ಒಳಹರಿವು_Control->ಪಠ್ಯ_View()->ಗಣ_ಶಬ್ದ_Wrap(ಸುಳ್ಳು);
	ಕೂಡಿಸು_ಮಗು(f_ಒಳಹರಿವು_Control);

	ದಶಮಾಂಶ ಅಗಲ = 0.f, ಎತ್ತರ = 0.f;
	f_ಒಳಹರಿವು_Control->Get_Preferred_ಗಾತ್ರ(&ಅಗಲ, &ಎತ್ತರ);
	f_ಒಳಹರಿವು_Control->MakeFocus(ನಿಜ);

	dialog_ಎತ್ತರ += ಎತ್ತರ + kVimDialogSpacingY * 1.5;
    }

    dialog_ಎತ್ತರ = max_c(dialog_ಎತ್ತರ, k_Vim_Dialog_ಕನಿಷ್ಠತಮ_ಎತ್ತರ);

    Resize_ಗಮ್ಯ(dialog_ಅಗಲ, dialog_ಎತ್ತರ);
    Move_ಗಮ್ಯ((gui.vim_ಕಿಟಕಿ->Bounds().ಅಗಲ_2() - dialog_ಅಗಲ) / 2,
	    (gui.vim_ಕಿಟಕಿ->Bounds().ಎತ್ತರ_2() - dialog_ಎತ್ತರ) / 2);

    //	adjust layout of buttons
    ದಶಮಾಂಶ button_ಅಗಲ = max_c(max_Button_ಅಗಲ, rect.ಅಗಲ_2() * 0.66);
    BPoint origin(dialog_ಅಗಲ, dialog_ಎತ್ತರ);
    origin.x -= kVimDialogSpacingX + (vertical ? button_ಅಗಲ : buttons_ಅಗಲ);
    origin.y -= kVimDialogSpacingY + (vertical ? buttons_ಎತ್ತರ	: max_Button_ಎತ್ತರ);

    ಗಾಗಿ (int32 i = 0 ; i < buttons_ಎಣಿಕೆ; i++) {
	BButton *button = (BButton*)fButtonsList.ಅಂಶ_At(i);
	button->Move_ಗಮ್ಯ(origin);
	ಆದರೆ (vertical) {
	    origin.y += button->ಚೌಕಟ್ಟು_2().ಎತ್ತರ_2() + kVimDialogButtonsSpacingY;
	    button->Resize_ಗಮ್ಯ(button_ಅಗಲ, button->ಚೌಕಟ್ಟು_2().ಎತ್ತರ_2());
	} ಇಲ್ಲದಿದ್ದರೆ
	    origin.x += button->ಚೌಕಟ್ಟು_2().ಅಗಲ_2() + kVimDialogButtonsSpacingX;

	ಆದರೆ (dfltbutton == i + 1) {
	    button->MakeDefault(ನಿಜ);
	    button->MakeFocus(f_ಒಳಹರಿವು_Control == ಖಾಲಿ);
	}
    }
}

VimDialog::~VimDialog()
{
    ಆದರೆ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    ಪೂರ್ಣಾಂಕ
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    ಆದರೆ (fDialogSem < B_OK) {
	Quit();
	ಹಿಂತಿರುಗಿ f_Dialog_ಮೌಲ್ಯ;
    }

    Show();

    ಆಗ (acquire_sem(fDialogSem) == B_INTERRUPTED);

    ಪೂರ್ಣಾಂಕ ret_ಮೌಲ್ಯ = f_Dialog_ಮೌಲ್ಯ;
    ಆದರೆ (f_ಒಳಹರಿವು_ಮೌಲ್ಯ != ಖಾಲಿ)
	vim_strncpy((char_u*)f_ಒಳಹರಿವು_ಮೌಲ್ಯ, (char_u*)f_ಒಳಹರಿವು_Control->ಪಠ್ಯ_2(), IOSIZE - 1);

    ಆದರೆ (ಬೀಗ_2())
	Quit();

    ಹಿಂತಿರುಗಿ ret_ಮೌಲ್ಯ;
}

ಶೂನ್ಯ VimDialog::ಸಂದೇಶ_Received(BMessage *ಸಂದೇಶ)
{
    int32 which = 0;
    ಆದರೆ (ಸಂದೇಶ->what != k_Vim_Dialog_Button_ಸಂದೇಶ ||
	    ಸಂದೇಶ->ಕಂಡುಹಿಡಿ_Int32("which", &which) != B_OK)
	ಹಿಂತಿರುಗಿ BWindow::ಸಂದೇಶ_Received(ಸಂದೇಶ);

    f_Dialog_ಮೌಲ್ಯ = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* label)
{
    BMessage *ಸಂದೇಶ_2 = ಹೊಸ BMessage(k_Vim_Dialog_Button_ಸಂದೇಶ);
    ಸಂದೇಶ_2->ಕೂಡಿಸು_Int32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = ಹೊಸ BButton(rect, name.String(), label, ಸಂದೇಶ_2,
	    B_FOLLOW_ಬಲ | B_FOLLOW_BOTTOM);

    ದಶಮಾಂಶ ಅಗಲ = 0.f, ಎತ್ತರ = 0.f;
    button->Get_Preferred_ಗಾತ್ರ(&ಅಗಲ, &ಎತ್ತರ);
    button->Resize_ಗಮ್ಯ(ಅಗಲ, ಎತ್ತರ);

    ಹಿಂತಿರುಗಿ button;
}

VimDialog::View::View(BRect ಚೌಕಟ್ಟು)
    :	BView(ಚೌಕಟ್ಟು, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(ಖಾಲಿ)
{
    ಗಣ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    ಅಳಿಸು fIconBitmap;
}

ಶೂನ್ಯ VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.ಬಲ = k_Vim_Dialog_Icon_Stripe_ಅಗಲ;
    ಗಣ_ಮೇಲಿನ_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    ಆದರೆ (fIconBitmap == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಣ_Drawing_ಬಹುಲಕ(B_OP_ALPHA);
    ಗಣ_Blending_ಬಹುಲಕ(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

ಶೂನ್ಯ VimDialog::View::InitIcon(int32 type)
{
    ಆದರೆ (type == VIM_GENERIC)
	ಹಿಂತಿರುಗಿ;

    BPath ಮಾರ್ಗ;
    ಸ್ಥಿತಿ_t ಸ್ಥಿತಿ_2 = ಕಂಡುಹಿಡಿ_directory(B_BEOS_SERVERS_DIRECTORY, &ಮಾರ್ಗ);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(ಸ್ಥಿತಿ_2));
	ಹಿಂತಿರುಗಿ;
    }

    ಮಾರ್ಗ.Append("app_server");

    BFile file(ಮಾರ್ಗ.ಮಾರ್ಗ_2(), O_RDONLY);
    ಆದರೆ (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	ಹಿಂತಿರುಗಿ;
    }

    BResources resources(&file);
    ಆದರೆ (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name = "";
    ಆಯ್ಕೆ(type) {
	ಸ್ಥಿತಿ VIM_ERROR:	    name = "stop"; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VIM_WARNING:   name = "warn"; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VIM_INFO:	    name = "info"; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VIM_QUESTION:  name = "idea"; ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ;
    }

    int32 icon_ಗಾತ್ರ_2 = 32;
    fIconBitmap = ಹೊಸ BBitmap(BRect(0, 0, icon_ಗಾತ್ರ_2 - 1, icon_ಗಾತ್ರ_2 - 1), 0, B_RGBA32);
    ಆದರೆ (fIconBitmap == ಖಾಲಿ || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == ಖಾಲಿ) ? "null" : strerror(fIconBitmap->InitCheck()));
	ಹಿಂತಿರುಗಿ;
    }

    size_t ಗಾತ್ರ_2 = 0;
    ಸ್ಥಿರಾಂಕ uint8* icon_ದತ್ತಾಂಶ = ಖಾಲಿ;
    //	try vector icon first?
    icon_ದತ್ತಾಂಶ = (ಸ್ಥಿರಾಂಕ uint8*)resources.ತುಂಬು_Resource(B_ಸದಿಶ_ICON_TYPE, name, &ಗಾತ್ರ_2);
    ಆದರೆ (icon_ದತ್ತಾಂಶ != ಖಾಲಿ && BIconUtils::Get_ಸದಿಶ_Icon(icon_ದತ್ತಾಂಶ, ಗಾತ್ರ_2, fIconBitmap) == B_OK)
	ಹಿಂತಿರುಗಿ;

    //	try bitmap icon now
    icon_ದತ್ತಾಂಶ = (ಸ್ಥಿರಾಂಕ uint8*)resources.ತುಂಬು_Resource(B_LARGE_ICON_TYPE, name, &ಗಾತ್ರ_2);
    ಆದರೆ (icon_ದತ್ತಾಂಶ == ಖಾಲಿ) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	ಅಳಿಸು fIconBitmap;
	fIconBitmap = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(icon_ದತ್ತಾಂಶ, icon_ಗಾತ್ರ_2, icon_ಗಾತ್ರ_2, icon_ಗಾತ್ರ_2, fIconBitmap);
}

ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  k_Vim_Dialog_OKButton_ಸಂದೇಶ = 'FDOK';
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  k_Vim_Dialog_Cancel_Button_ಸಂದೇಶ = 'FDCN';
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  k_Vim_Dialog_ಗಾತ್ರ_ಒಳಹರಿವು_ಸಂದೇಶ = 'SICH';
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  k_Vim_Dialog_Family_Select_ಸಂದೇಶ = 'MSFM';
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  k_Vim_Dialog_Style_Select_ಸಂದೇಶ = 'MSST';
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  k_Vim_Dialog_ಗಾತ್ರ_Select_ಸಂದೇಶ = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, ದಶಮಾಂಶ* ಗಾತ್ರ_2)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_ಕಿಟಕಿ_LOOK, B_MODAL_APP_ಕಿಟಕಿ_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , f_ಸ್ಥಿತಿ(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_ಮೌಲ್ಯ(ಸುಳ್ಳು)
    , fFamily(family)
    , fStyle(style)
    , f_ಗಾತ್ರ(ಗಾತ್ರ_2)
    , f_Font_ಗಾತ್ರ(*ಗಾತ್ರ_2)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_ಒಳಹರಿವು(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_ಉದ್ದಳತೆ);
    strncpy(fFontStyle, *style, B_FONT_STYLE_ಉದ್ದಳತೆ);

    //	"client" area view
    BBox *clientBox = ಹೊಸ BBox(Bounds(), B_ಖಾಲಿಗಳು_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_ಚೌಕಟ್ಟು_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    ಕೂಡಿಸು_ಮಗು(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.ಎಡ_ತುದಿ(), RC.ಎಡ_ತುದಿ());

    //	at first create all controls
    fPreview = ಹೊಸ BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->ಕೂಡಿಸು_ಮಗು(fPreview);

    BBox* boxDivider = ಹೊಸ BBox(rc, B_ಖಾಲಿಗಳು_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->ಕೂಡಿಸು_ಮಗು(boxDivider);

    BStringView *labelFamily = ಹೊಸ BStringView(rc, "labelFamily", "Family:");
    clientBox->ಕೂಡಿಸು_ಮಗು(labelFamily);
    labelFamily->Resize_ಗಮ್ಯ_Preferred();

    BStringView *labelStyle = ಹೊಸ BStringView(rc, "labelStyle", "Style:");
    clientBox->ಕೂಡಿಸು_ಮಗು(labelStyle);
    labelStyle->Resize_ಗಮ್ಯ_Preferred();

    BStringView *label_ಗಾತ್ರ = ಹೊಸ BStringView(rc, "labelSize", "Size:");
    clientBox->ಕೂಡಿಸು_ಮಗು(label_ಗಾತ್ರ);
    label_ಗಾತ್ರ->Resize_ಗಮ್ಯ_Preferred();

    fFamiliesList = ಹೊಸ BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = ಹೊಸ BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_ಎಡ_ಬಲ, 0, ಸುಳ್ಳು, ನಿಜ);
    clientBox->ಕೂಡಿಸು_ಮಗು(scrollFamilies);

    fStylesList= ಹೊಸ BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = ಹೊಸ BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_ಎಡ_ಬಲ, 0, ಸುಳ್ಳು, ನಿಜ);
    clientBox->ಕೂಡಿಸು_ಮಗು(scrollStyles);

    f_Sizes_ಒಳಹರಿವು = ಹೊಸ BTextControl(rc, "inputSize", ಖಾಲಿ, "???",
	    ಹೊಸ BMessage(k_Vim_Dialog_ಗಾತ್ರ_ಒಳಹರಿವು_ಸಂದೇಶ));
    clientBox->ಕೂಡಿಸು_ಮಗು(f_Sizes_ಒಳಹರಿವು);
    f_Sizes_ಒಳಹರಿವು->Resize_ಗಮ್ಯ_Preferred();

    fSizesList = ಹೊಸ BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = ಹೊಸ BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_ಎಡ_ಬಲ, 0, ಸುಳ್ಳು, ನಿಜ);
    clientBox->ಕೂಡಿಸು_ಮಗು(scrollSizes);

    BButton *buttonOK = ಹೊಸ BButton(rc, "buttonOK", "OK",
			ಹೊಸ BMessage(k_Vim_Dialog_OKButton_ಸಂದೇಶ));
    clientBox->ಕೂಡಿಸು_ಮಗು(buttonOK);
    buttonOK->Resize_ಗಮ್ಯ_Preferred();

    BButton *buttonCancel = ಹೊಸ BButton(rc, "buttonCancel", "Cancel",
			ಹೊಸ BMessage(k_Vim_Dialog_Cancel_Button_ಸಂದೇಶ));
    clientBox->ಕೂಡಿಸು_ಮಗು(buttonCancel);
    buttonCancel->Resize_ಗಮ್ಯ_Preferred();

    //	layout controls
    ದಶಮಾಂಶ ಸಾಲು_ಎತ್ತರ = labelFamily->Bounds().ಎತ್ತರ_2();
    ದಶಮಾಂಶ preview_ಎತ್ತರ = ಸಾಲು_ಎತ್ತರ * 3;
    ದಶಮಾಂಶ ಸ್ಥಾನಾಂತರ_YLabels = preview_ಎತ್ತರ + kVimDialogSpacingY;
    ದಶಮಾಂಶ ಸ್ಥಾನಾಂತರ_YLists = ಸ್ಥಾನಾಂತರ_YLabels + ಸಾಲು_ಎತ್ತರ + kVimDialogSpacingY / 2;
    ದಶಮಾಂಶ ಸ್ಥಾನಾಂತರ_YSizes = ಸ್ಥಾನಾಂತರ_YLists + f_Sizes_ಒಳಹರಿವು->Bounds().ಎತ್ತರ_2() + kVimDialogSpacingY / 2;
    ದಶಮಾಂಶ lists_ಎತ್ತರ = ಸಾಲು_ಎತ್ತರ * 9;
    ದಶಮಾಂಶ ಸ್ಥಾನಾಂತರ_YButtons = ಸ್ಥಾನಾಂತರ_YLists + lists_ಎತ್ತರ +  kVimDialogSpacingY;
    ದಶಮಾಂಶ max_Controls_ಎತ್ತರ = ಸ್ಥಾನಾಂತರ_YButtons + buttonOK->Bounds().ಎತ್ತರ_2();
    ದಶಮಾಂಶ families_ಅಗಲ = labelFamily->Bounds().ಅಗಲ_2() * 5;
    ದಶಮಾಂಶ ಸ್ಥಾನಾಂತರ_XStyles = families_ಅಗಲ + kVimDialogSpacingX;
    ದಶಮಾಂಶ styles_ಅಗಲ = labelStyle->Bounds().ಅಗಲ_2() * 4;
    ದಶಮಾಂಶ ಸ್ಥಾನಾಂತರ_XSizes = ಸ್ಥಾನಾಂತರ_XStyles + styles_ಅಗಲ + kVimDialogSpacingX;
    ದಶಮಾಂಶ sizes_ಅಗಲ = label_ಗಾತ್ರ->Bounds().ಅಗಲ_2() * 2;
    ದಶಮಾಂಶ max_Controls_ಅಗಲ = ಸ್ಥಾನಾಂತರ_XSizes + sizes_ಅಗಲ;

    Resize_ಗಮ್ಯ(max_Controls_ಅಗಲ + kVimDialogSpacingX * 2,
	max_Controls_ಎತ್ತರ + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vim_ಕಿಟಕಿ->ಚೌಕಟ್ಟು_2();
    Move_ಗಮ್ಯ(rcVim.ಎಡ + (rcVim.ಅಗಲ_2() - ಚೌಕಟ್ಟು_2().ಅಗಲ_2()) / 2,
	    rcVim.ತುದಿ + (rcVim.ಎತ್ತರ_2() - ಚೌಕಟ್ಟು_2().ಎತ್ತರ_2()) / 2);

    fPreview->Resize_ಗಮ್ಯ(max_Controls_ಅಗಲ, preview_ಎತ್ತರ);
    fPreview->ಗಣ_Alignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, preview_ಎತ್ತರ + kVimDialogSpacingY / 2);
    boxDivider->Resize_ಗಮ್ಯ(max_Controls_ಅಗಲ, 1.f);

    labelFamily->MoveBy(0.f, ಸ್ಥಾನಾಂತರ_YLabels);
    labelStyle->MoveBy(ಸ್ಥಾನಾಂತರ_XStyles, ಸ್ಥಾನಾಂತರ_YLabels);
    label_ಗಾತ್ರ->MoveBy(ಸ್ಥಾನಾಂತರ_XSizes, ಸ್ಥಾನಾಂತರ_YLabels);

    //	text control alignment issues
    ದಶಮಾಂಶ insetX = f_Sizes_ಒಳಹರಿವು->ಪಠ್ಯ_View()->Bounds().ಅಗಲ_2() - f_Sizes_ಒಳಹರಿವು->Bounds().ಅಗಲ_2();
    ದಶಮಾಂಶ insetY = f_Sizes_ಒಳಹರಿವು->ಪಠ್ಯ_View()->Bounds().ಅಗಲ_2() - f_Sizes_ಒಳಹರಿವು->Bounds().ಅಗಲ_2();

    scrollFamilies->MoveBy(0.f, ಸ್ಥಾನಾಂತರ_YLists);
    scrollStyles->MoveBy(ಸ್ಥಾನಾಂತರ_XStyles, ಸ್ಥಾನಾಂತರ_YLists);
    f_Sizes_ಒಳಹರಿವು->MoveBy(ಸ್ಥಾನಾಂತರ_XSizes + insetX / 2, ಸ್ಥಾನಾಂತರ_YLists + insetY / 2);
    scrollSizes->MoveBy(ಸ್ಥಾನಾಂತರ_XSizes, ಸ್ಥಾನಾಂತರ_YSizes);

    f_Sizes_ಒಳಹರಿವು->ಗಣ_Alignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->Resize_ಗಮ್ಯ(families_ಅಗಲ, lists_ಎತ್ತರ);
    scrollStyles->Resize_ಗಮ್ಯ(styles_ಅಗಲ, lists_ಎತ್ತರ);
    f_Sizes_ಒಳಹರಿವು->Resize_ಗಮ್ಯ(sizes_ಅಗಲ, f_Sizes_ಒಳಹರಿವು->Bounds().ಎತ್ತರ_2());
    scrollSizes->Resize_ಗಮ್ಯ(sizes_ಅಗಲ,
	    lists_ಎತ್ತರ - (ಸ್ಥಾನಾಂತರ_YSizes - ಸ್ಥಾನಾಂತರ_YLists));

    buttonOK->MoveBy(max_Controls_ಅಗಲ - buttonOK->Bounds().ಅಗಲ_2(), ಸ್ಥಾನಾಂತರ_YButtons);
    buttonCancel->MoveBy(max_Controls_ಅಗಲ - buttonOK->Bounds().ಅಗಲ_2()
	    - buttonCancel->Bounds().ಅಗಲ_2() - kVimDialogSpacingX, ಸ್ಥಾನಾಂತರ_YButtons);

    //	fill lists
    ಪೂರ್ಣಾಂಕ sel_ಸೂಚ್ಯಂಕ = -1;
    ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = ಎಣಿಕೆ_font_families();
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಎಣಿಕೆ_2; i++) {
	font_family family;
	ಆದರೆ (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->ಕೂಡಿಸು_ಅಂಶ(ಹೊಸ BString_ಅಂಶ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)family));
	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(family, fFontFamily, B_FONT_FAMILY_ಉದ್ದಳತೆ) == 0)
		sel_ಸೂಚ್ಯಂಕ = i;
	}
    }

    ಆದರೆ (sel_ಸೂಚ್ಯಂಕ >= 0) {
	fFamiliesList->Select(sel_ಸೂಚ್ಯಂಕ);
	fFamiliesList->Scroll_ಗಮ್ಯ_Selection();
    }

    _UpdateFontStyles();

    sel_ಸೂಚ್ಯಂಕ = -1;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = 8, ಸೂಚ್ಯಂಕ = 0; ಗಾತ್ರ_2 <= 18; ಗಾತ್ರ_2++, ಸೂಚ್ಯಂಕ++) {
	BString str;
	str << ಗಾತ್ರ_2;
	fSizesList->ಕೂಡಿಸು_ಅಂಶ(ಹೊಸ BString_ಅಂಶ(str));
	ಆದರೆ (ಗಾತ್ರ_2 == f_Font_ಗಾತ್ರ)
	    sel_ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;

    }

    ಆದರೆ (sel_ಸೂಚ್ಯಂಕ >= 0) {
	fSizesList->Select(sel_ಸೂಚ್ಯಂಕ);
	fSizesList->Scroll_ಗಮ್ಯ_Selection();
    }

    fFamiliesList->ಗಣ_Selection_ಸಂದೇಶ(ಹೊಸ BMessage(k_Vim_Dialog_Family_Select_ಸಂದೇಶ));
    fStylesList->ಗಣ_Selection_ಸಂದೇಶ(ಹೊಸ BMessage(k_Vim_Dialog_Style_Select_ಸಂದೇಶ));
    fSizesList->ಗಣ_Selection_ಸಂದೇಶ(ಹೊಸ BMessage(k_Vim_Dialog_ಗಾತ್ರ_Select_ಸಂದೇಶ));
    f_Sizes_ಒಳಹರಿವು->ಗಣ_Modification_ಸಂದೇಶ(ಹೊಸ BMessage(k_Vim_Dialog_ಗಾತ್ರ_ಒಳಹರಿವು_ಸಂದೇಶ));

    Update_ಗಾತ್ರ_ಒಳಹರಿವು_Preview();
    _UpdateFontPreview();

    f_ಸ್ಥಿತಿ = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    ಆದರೆ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    ಶೂನ್ಯ
VimSelectFontDialog::_CleanList(BListView* list)
{
    ಆಗ (0 < list->ಎಣಿಕೆ_ವಸ್ತುಗಳು())
	ಅಳಿಸು (dynamic_cast<BString_ಅಂಶ*>(list->Remove_ಅಂಶ((int32)0)));
}

    ತಾರ್ಕಿಕ
VimSelectFontDialog::Go()
{
    ಆದರೆ (f_ಸ್ಥಿತಿ != B_OK) {
	Quit();
	ಹಿಂತಿರುಗಿ NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    ಆದರೆ (fDialogSem < B_OK) {
	Quit();
	ಹಿಂತಿರುಗಿ f_Dialog_ಮೌಲ್ಯ;
    }

    Show();

    ಆಗ (acquire_sem(fDialogSem) == B_INTERRUPTED);

    ತಾರ್ಕಿಕ ret_ಮೌಲ್ಯ = f_Dialog_ಮೌಲ್ಯ;

    ಆದರೆ (ಬೀಗ_2())
	Quit();

    ಹಿಂತಿರುಗಿ ret_ಮೌಲ್ಯ;
}


ಶೂನ್ಯ VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 sel_ಸೂಚ್ಯಂಕ = -1;
    int32 ಎಣಿಕೆ_2 = ಎಣಿಕೆ_font_styles(fFontFamily);
    ಗಾಗಿ (int32 i = 0; i < ಎಣಿಕೆ_2; i++) {
	font_style style;
	uint32 flags = 0;
	ಆದರೆ (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->ಕೂಡಿಸು_ಅಂಶ(ಹೊಸ BString_ಅಂಶ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)style));
	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(style, fFontStyle, B_FONT_STYLE_ಉದ್ದಳತೆ) == 0)
		sel_ಸೂಚ್ಯಂಕ = i;
	}
    }

    ಆದರೆ (sel_ಸೂಚ್ಯಂಕ >= 0) {
	fStylesList->Select(sel_ಸೂಚ್ಯಂಕ);
	fStylesList->Scroll_ಗಮ್ಯ_Selection();
    } ಇಲ್ಲದಿದ್ದರೆ
	fStylesList->Select(0);
}


ಶೂನ್ಯ VimSelectFontDialog::Update_ಗಾತ್ರ_ಒಳಹರಿವು_Preview()
{
    ಅಕ್ಷರ buf[10] = {0};
    vim_snprintf(buf, ಗಾತ್ರ(buf), (ಅಕ್ಷರ*)"%.0f", f_Font_ಗಾತ್ರ);
    f_Sizes_ಒಳಹರಿವು->ಗಣ_ಪಠ್ಯ(buf);
}


ಶೂನ್ಯ VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.ಗಣ_ಗಾತ್ರ(f_Font_ಗಾತ್ರ);
    font.ಗಣ_Family_And_Style(fFontFamily, fFontStyle);
    fPreview->ಗಣ_Font(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_ಗಾತ್ರ);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (ಪೂರ್ಣಾಂಕ)f_Font_ಗಾತ್ರ << " pt.";
    fPreview->ಗಣ_ಪಠ್ಯ(str);
}


    ತಾರ್ಕಿಕ
VimSelectFontDialog::Update_From_List_ಅಂಶ(BListView* list, ಅಕ್ಷರ* ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ ಪಠ್ಯ_ಗಾತ್ರ)
{
    int32 ಸೂಚ್ಯಂಕ = list->CurrentSelection();
    ಆದರೆ (ಸೂಚ್ಯಂಕ < 0)
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    BString_ಅಂಶ* ಅಂಶ = (BString_ಅಂಶ*)list->ಅಂಶ_At(ಸೂಚ್ಯಂಕ);
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    strncpy(ಪಠ್ಯ, ಅಂಶ->ಪಠ್ಯ_2(), ಪಠ್ಯ_ಗಾತ್ರ);
    ಹಿಂತಿರುಗಿ ನಿಜ;
}


ಶೂನ್ಯ VimSelectFontDialog::ಸಂದೇಶ_Received(BMessage *ಸಂದೇಶ)
{
    ಆಯ್ಕೆ (ಸಂದೇಶ->what) {
	ಸ್ಥಿತಿ k_Vim_Dialog_OKButton_ಸಂದೇಶ:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_ಉದ್ದಳತೆ);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_ಉದ್ದಳತೆ);
	    *f_ಗಾತ್ರ = f_Font_ಗಾತ್ರ;
	    f_Dialog_ಮೌಲ್ಯ = ನಿಜ;
	ಸ್ಥಿತಿ k_Vim_Dialog_Cancel_Button_ಸಂದೇಶ:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    ಹಿಂತಿರುಗಿ;
	ಸ್ಥಿತಿ B_ಕೀಲಿ_ಮೇಲೆ:
	    {
		int32 ಕೀಲಿ = 0;
		ಆದರೆ (ಸಂದೇಶ->ಕಂಡುಹಿಡಿ_Int32("raw_char", &ಕೀಲಿ) == B_OK
			&& ಕೀಲಿ == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ k_Vim_Dialog_Family_Select_ಸಂದೇಶ:
	    ಆದರೆ (Update_From_List_ಅಂಶ(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_ಉದ್ದಳತೆ)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ k_Vim_Dialog_Style_Select_ಸಂದೇಶ:
	    ಆದರೆ (Update_From_List_ಅಂಶ(fStylesList,
		    fFontStyle, B_FONT_STYLE_ಉದ್ದಳತೆ))
		_UpdateFontPreview();
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ k_Vim_Dialog_ಗಾತ್ರ_Select_ಸಂದೇಶ:
	    {
		ಅಕ್ಷರ buf[10] = {0};
		ಆದರೆ (Update_From_List_ಅಂಶ(fSizesList, buf, ಗಾತ್ರ(buf))) {
		    ದಶಮಾಂಶ ಗಾತ್ರ_2 = atof(buf);
		    ಆದರೆ (ಗಾತ್ರ_2 > 0.f) {
			f_Font_ಗಾತ್ರ = ಗಾತ್ರ_2;
			Update_ಗಾತ್ರ_ಒಳಹರಿವು_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ k_Vim_Dialog_ಗಾತ್ರ_ಒಳಹರಿವು_ಸಂದೇಶ:
	    {
		ದಶಮಾಂಶ ಗಾತ್ರ_2 = atof(f_Sizes_ಒಳಹರಿವು->ಪಠ್ಯ_2());
		ಆದರೆ (ಗಾತ್ರ_2 > 0.f) {
		    f_Font_ಗಾತ್ರ = ಗಾತ್ರ_2;
		    _UpdateFontPreview();
		}
	    }
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ BWindow::ಸಂದೇಶ_Received(ಸಂದೇಶ);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_DIALOG

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
ಸ್ಥಿರ BMessage * Menu_ಸಂದೇಶ(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect ಚೌಕಟ್ಟು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) :
    BBox(ಚೌಕಟ್ಟು, name, B_FOLLOW_ಎಡ | B_FOLLOW_ತುದಿ, B_WILL_DRAW | B_ಚೌಕಟ್ಟು_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 ಎಣಿಕೆ_2 = fButtonsList.ಎಣಿಕೆ_ವಸ್ತುಗಳು();
    ಗಾಗಿ (int32 i = 0; i < ಎಣಿಕೆ_2; i++)
	ಅಳಿಸು (BPictureButton*)fButtonsList.ಅಂಶ_At(i);
    fButtonsList.Make_ಖಾಲಿಗಳು();

    ಅಳಿಸು ಸಾಮಾನ್ಯ_Buttons_Bitmap;
    ಅಳಿಸು grayedButtonsBitmap;
    ಸಾಮಾನ್ಯ_Buttons_Bitmap    = ಖಾಲಿ;
    grayedButtonsBitmap  = ಖಾಲಿ;
}

    ಶೂನ್ಯ
VimToolbar::Attached_ಗಮ್ಯ_ಕಿಟಕಿ()
{
    BBox::Attached_ಗಮ್ಯ_ಕಿಟಕಿ();

    ಗಣ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    ದಶಮಾಂಶ
VimToolbar::Toolbar_ಎತ್ತರ() ಸ್ಥಿರಾಂಕ
{
    ದಶಮಾಂಶ ಗಾತ್ರ_2 = ಖಾಲಿ == ಸಾಮಾನ್ಯ_Buttons_Bitmap ? 18. : ಸಾಮಾನ್ಯ_Buttons_Bitmap->Bounds().ಎತ್ತರ_2();
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2 + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    ತಾರ್ಕಿಕ
VimToolbar::Modify_Bitmap_ಗಮ್ಯ_Grayed(BBitmap *bitmap)
{
    ದಶಮಾಂಶ ಎತ್ತರ = bitmap->Bounds().ಎತ್ತರ_2();
    ದಶಮಾಂಶ ಅಗಲ  = bitmap->Bounds().ಅಗಲ_2();

    rgb_color *ಬಿಟ್ಟುಗಳು = (rgb_color*)bitmap->ಬಿಟ್ಟುಗಳು_2();
    int32 pixels = bitmap->ಬಿಟ್ಟುಗಳು_ಉದ್ದಳತೆ() / 4;
    ಗಾಗಿ (int32 i = 0; i < pixels; i++) {
	ಬಿಟ್ಟುಗಳು[i].red = ಬಿಟ್ಟುಗಳು[i].green =
	ಬಿಟ್ಟುಗಳು[i].blue = ((uint32)ಬಿಟ್ಟುಗಳು[i].red + ಬಿಟ್ಟುಗಳು[i].green + ಬಿಟ್ಟುಗಳು[i].blue) / 3;
	ಬಿಟ್ಟುಗಳು[i].alpha /= 4;
    }

    ಹಿಂತಿರುಗಿ ನಿಜ;
}

    ತಾರ್ಕಿಕ
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    ಸಾಮಾನ್ಯ_Buttons_Bitmap = ತುಂಬು_Vim_Bitmap("builtin-tools.png");
    ಆದರೆ (ಸಾಮಾನ್ಯ_Buttons_Bitmap == ಖಾಲಿ)
	//  customized not found? dig application resources for "builtin-tools" one
	ಸಾಮಾನ್ಯ_Buttons_Bitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    ಆದರೆ (ಸಾಮಾನ್ಯ_Buttons_Bitmap == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

    BMessage archive;
    ಸಾಮಾನ್ಯ_Buttons_Bitmap->Archive(&archive);

    grayedButtonsBitmap = ಹೊಸ BBitmap(&archive);
    ಆದರೆ (grayedButtonsBitmap == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

    //	modify grayed bitmap
    Modify_Bitmap_ಗಮ್ಯ_Grayed(grayedButtonsBitmap);

    ಹಿಂತಿರುಗಿ ನಿಜ;
}

BBitmap *VimToolbar::ತುಂಬು_Vim_Bitmap(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* fileName)
{
    BBitmap *bitmap = ಖಾಲಿ;

    ಪೂರ್ಣಾಂಕ mustfree = 0;
    char_u* runtime_ಮಾರ್ಗ = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    ಆದರೆ (runtime_ಮಾರ್ಗ != ಖಾಲಿ && fileName != ಖಾಲಿ) {
	BString str_ಮಾರ್ಗ((ಅಕ್ಷರ*)runtime_ಮಾರ್ಗ);
	str_ಮಾರ್ಗ << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(str_ಮಾರ್ಗ.String());
    }

    ಆದರೆ (mustfree)
	vim_free(runtime_ಮಾರ್ಗ);

    ಹಿಂತಿರುಗಿ bitmap;
}

    ತಾರ್ಕಿಕ
VimToolbar::GetPictureFromBitmap(BPicture *picture_ಗಮ್ಯ, int32 ಸೂಚ್ಯಂಕ, BBitmap *bitmapFrom, ತಾರ್ಕಿಕ pressed)
{
    ದಶಮಾಂಶ ಗಾತ್ರ_2 = bitmapFrom->Bounds().ಎತ್ತರ_2() + 1.;

    BView view(BRect(0, 0, ಗಾತ್ರ_2, ಗಾತ್ರ_2), "", 0, 0);

    ಕೂಡಿಸು_ಮಗು(&view);
    view.BeginPicture(picture_ಗಮ್ಯ);

    view.ಗಣ_ಮೇಲಿನ_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.ಗಣ_Drawing_ಬಹುಲಕ(B_OP_OVER);

    BRect ಮೂಲ(0, 0, ಗಾತ್ರ_2 - 1, ಗಾತ್ರ_2 - 1);
    BRect destination(ಮೂಲ);

    ಮೂಲ.ಸ್ಥಾನಾಂತರ_By(ಗಾತ್ರ_2 * ಸೂಚ್ಯಂಕ, 0);
    destination.ಸ್ಥಾನಾಂತರ_By(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, ಮೂಲ, destination);

    ಆದರೆ (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	ಗಾತ್ರ_2 += ButtonMargin * 2 - 1;
	view.Begin_ಸಾಲು_Array(4);
	view.ಕೂಡಿಸು_ಸಾಲು(BPoint(0, 0),	 BPoint(ಗಾತ್ರ_2, 0),    shadowColor);
	view.ಕೂಡಿಸು_ಸಾಲು(BPoint(ಗಾತ್ರ_2, 0),	 BPoint(ಗಾತ್ರ_2, ಗಾತ್ರ_2), shineColor);
	view.ಕೂಡಿಸು_ಸಾಲು(BPoint(ಗಾತ್ರ_2, ಗಾತ್ರ_2), BPoint(0, ಗಾತ್ರ_2),    shineColor);
	view.ಕೂಡಿಸು_ಸಾಲು(BPoint(0, ಗಾತ್ರ_2),	 BPoint(0, 0),	     shadowColor);
	view.ಕೊನೆ_ಸಾಲು_Array();
    }

    view.ಕೊನೆ_Picture();
    Remove_ಮಗು(&view);

    ಹಿಂತಿರುಗಿ ನಿಜ;
}

    ತಾರ್ಕಿಕ
VimToolbar::ಕೂಡಿಸು_Button(int32 ಸೂಚ್ಯಂಕ, vimmenu_T *menu)
{
    BPictureButton *button = ಖಾಲಿ;
    ಆದರೆ (!menu_is_separator(menu->name)) {
	ದಶಮಾಂಶ ಗಾತ್ರ_2 = ಸಾಮಾನ್ಯ_Buttons_Bitmap ?
	    ಸಾಮಾನ್ಯ_Buttons_Bitmap->Bounds().ಎತ್ತರ_2() + 1. + ButtonMargin * 2 : 18.;
	BRect ಚೌಕಟ್ಟು(0, 0, ಗಾತ್ರ_2, ಗಾತ್ರ_2);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture picture_ಬೂದು;

	ಆದರೆ (menu->iconfile == ಖಾಲಿ && menu->iconidx >= 0 && ಸಾಮಾನ್ಯ_Buttons_Bitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, ಸಾಮಾನ್ಯ_Buttons_Bitmap, ನಿಜ);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, ಸಾಮಾನ್ಯ_Buttons_Bitmap, ಸುಳ್ಳು);
	    GetPictureFromBitmap(&picture_ಬೂದು, menu->iconidx, grayedButtonsBitmap, ಸುಳ್ಳು);
	} ಇಲ್ಲದಿದ್ದರೆ {

	    char_u ಬಫರ್[MAXPATHL] = {0};
	    BBitmap *bitmap = ಖಾಲಿ;

	    ಆದರೆ (menu->iconfile) {
		gui_ಕಂಡುಹಿಡಿ_iconfile(menu->iconfile, ಬಫರ್, (ಅಕ್ಷರ*)"png");
		bitmap = BTranslationUtils::GetBitmap((ಅಕ್ಷರ*)ಬಫರ್);
	    }

	    ಆದರೆ (bitmap == ಖಾಲಿ && gui_ಕಂಡುಹಿಡಿ_bitmap(menu->name, ಬಫರ್, (ಅಕ್ಷರ*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((ಅಕ್ಷರ*)ಬಫರ್);

	    ಆದರೆ (bitmap == ಖಾಲಿ)
		bitmap = ಹೊಸ BBitmap(BRect(0, 0, ಗಾತ್ರ_2, ಗಾತ್ರ_2), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, ನಿಜ);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, ಸುಳ್ಳು);
	    Modify_Bitmap_ಗಮ್ಯ_Grayed(bitmap);
	    GetPictureFromBitmap(&picture_ಬೂದು, 0, bitmap, ಸುಳ್ಳು);

	    ಅಳಿಸು bitmap;
	}

	button = ಹೊಸ BPictureButton(ಚೌಕಟ್ಟು, (ಅಕ್ಷರ*)menu->name,
		    &pictureOff, &pictureOn, Menu_ಸಂದೇಶ(menu));

	button->ಗಣ_Disabled_On(&picture_ಬೂದು);
	button->ಗಣ_Disabled_Off(&picture_ಬೂದು);

	button->ಗಣ_ಗುರಿ(gui.vim_ಪಠ್ಯ_Area);

	ಕೂಡಿಸು_ಮಗು(button);

	menu->button = button;
    }

    ತಾರ್ಕಿಕ ಫಲಿತಾಂಶ = fButtonsList.ಕೂಡಿಸು_ಅಂಶ(button, ಸೂಚ್ಯಂಕ);
    InvalidateLayout();
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

    ತಾರ್ಕಿಕ
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    ಆದರೆ (menu->button) {
	ಆದರೆ (fButtonsList.Remove_ಅಂಶ(menu->button)) {
	    ಅಳಿಸು menu->button;
	    menu->button = ಖಾಲಿ;
	}
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

    ತಾರ್ಕಿಕ
VimToolbar::ಬೂದು_Button(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ grey)
{
    ಆದರೆ (menu->button) {
	int32 ಸೂಚ್ಯಂಕ = fButtonsList.ಸೂಚ್ಯಂಕ_ದ(menu->button);
	ಆದರೆ (ಸೂಚ್ಯಂಕ >= 0)
	    menu->button->ಗಣ_Enabled(grey ? ಸುಳ್ಳು : ನಿಜ);
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

    ಶೂನ್ಯ
VimToolbar::InvalidateLayout()
{
    int32 ಸ್ಥಾನಾಂತರ = ToolbarMargin;
    int32 ಎಣಿಕೆ_2 = fButtonsList.ಎಣಿಕೆ_ವಸ್ತುಗಳು();
    ಗಾಗಿ (int32 i = 0; i < ಎಣಿಕೆ_2; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.ಅಂಶ_At(i);
	ಆದರೆ (button) {
	    button->Move_ಗಮ್ಯ(ಸ್ಥಾನಾಂತರ, ToolbarMargin);
	    ಸ್ಥಾನಾಂತರ += button->Bounds().ಅಗಲ_2() + ToolbarMargin;
	} ಇಲ್ಲದಿದ್ದರೆ
	    ಸ್ಥಾನಾಂತರ += ToolbarMargin * 3;
    }
}

#ಆದರೆ_ಅಂತ್ಯ /*FEAT_TOOLBAR*/

#ಆದರೆ defined(FEAT_GUI_TABLINE)

    ದಶಮಾಂಶ
Vim_Tab_ಸಾಲು::Tabline_ಎತ್ತರ() ಸ್ಥಿರಾಂಕ
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    ಹಿಂತಿರುಗಿ Tab_ಎತ್ತರ(); //  + ToolbarMargin;
}

ಶೂನ್ಯ
Vim_Tab_ಸಾಲು::Mouse_ಕೆಳಕ್ಕೆ(BPoint ಬಿಂದು)
{
    ಆದರೆ (!gui_mch_showing_tabline())
	ಹಿಂತಿರುಗಿ;

    BMessage *m = ಕಿಟಕಿ()->Current_ಸಂದೇಶ();
    ಪ್ರತಿಜ್ಞಾ(m);

    int32 buttons = 0;
    m->ಕಂಡುಹಿಡಿ_Int32("buttons", &buttons);

    int32 clicks = 0;
    m->ಕಂಡುಹಿಡಿ_Int32("clicks", &clicks);

    ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = 0; //  0 means here - no tab found
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಎಣಿಕೆ_Tabs(); i++) {
	ಆದರೆ (Tab_ಚೌಕಟ್ಟು(i).ಒಳಗೊಂಡಿದೆ_2(ಬಿಂದು)) {
	    ಸೂಚ್ಯಂಕ = i + 1; //  indexes are 1-based
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಪೂರ್ಣಾಂಕ event = -1;

    ಆದರೆ ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = ಸೂಚ್ಯಂಕ > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPop_ಮೇಲೆ_Menu* ತೆಗೆ_ಮೇಲೆ_Menu = ಹೊಸ BPop_ಮೇಲೆ_Menu("tabLineContextMenu", ಸುಳ್ಳು, ಸುಳ್ಳು);
	ತೆಗೆ_ಮೇಲೆ_Menu->ಕೂಡಿಸು_ಅಂಶ(ಹೊಸ BMenu_ಅಂಶ(_("Close tabi R"), ಹೊಸ BMessage(TABLINE_MENU_CLOSE)));
	ತೆಗೆ_ಮೇಲೆ_Menu->ಕೂಡಿಸು_ಅಂಶ(ಹೊಸ BMenu_ಅಂಶ(_("New tab    T"), ಹೊಸ BMessage(TABLINE_MENU_NEW)));
	ತೆಗೆ_ಮೇಲೆ_Menu->ಕೂಡಿಸು_ಅಂಶ(ಹೊಸ BMenu_ಅಂಶ(_("Open tab..."), ಹೊಸ BMessage(TABLINE_MENU_OPEN)));

	Convert_ಗಮ್ಯ_Screen(&ಬಿಂದು);
	BMenu_ಅಂಶ* ಅಂಶ = ತೆಗೆ_ಮೇಲೆ_Menu->Go(ಬಿಂದು);
	ಆದರೆ (ಅಂಶ != ಖಾಲಿ) {
	    event = ಅಂಶ->Command();
	}

	ಅಳಿಸು ತೆಗೆ_ಮೇಲೆ_Menu;

    } ಇಲ್ಲದಿದ್ದರೆ {
	//  default processing
	BTabView::Mouse_ಕೆಳಕ್ಕೆ(ಬಿಂದು);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (event < 0)
	ಹಿಂತಿರುಗಿ;

    Vim_Tabline_Menu_ಸಂದೇಶ tmm;
    tmm.ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;
    tmm.event = event;
    write_ಪೋರ್ಟ್(gui.vdcmp, Vim_ಸಂದೇಶ::TablineMenu, &tmm, ಗಾತ್ರ(tmm));
}

ಶೂನ್ಯ
Vim_Tab_ಸಾಲು::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_ಸಾಲು *tab_ಸಾಲು = gui.vimForm->Tab_ಸಾಲು();
    ಆದರೆ (tab_ಸಾಲು != ಖಾಲಿ) {

	int32 i = 0;
	ಗಾಗಿ (; i < tab_ಸಾಲು->ಎಣಿಕೆ_Tabs(); i++)
	    ಆದರೆ (ಇದು == tab_ಸಾಲು->TabAt(i))
		ನಿಲ್ಲಿಸು;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	ಆದರೆ (i < tab_ಸಾಲು->ಎಣಿಕೆ_Tabs()) {
	    Vim_Tabline_ಸಂದೇಶ tm;
	    tm.ಸೂಚ್ಯಂಕ = i + 1;
	    write_ಪೋರ್ಟ್(gui.vdcmp, Vim_ಸಂದೇಶ::Tabline, &tm, ಗಾತ್ರ(tm));
	}
    }
}

#ಆದರೆ_ಅಂತ್ಯ //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
ಸ್ಥಿರ ಅಕ್ಷರ appsig[] = "application/x-vnd.Haiku-Vim-8";
ಕೀಲಿ_map *ಕೀಲಿ_Map;
ಅಕ್ಷರ *ಕೀಲಿ_Map_Chars;
ಪೂರ್ಣಾಂಕ main_exitcode = 127;

    ಸ್ಥಿತಿ_t
gui_haiku_process_event(bigtime_t timeout)
{
    ರಚನೆ Vim_ಸಂದೇಶ vm;
    int32 what;
    ssize_t ಗಾತ್ರ_2;

    ಗಾತ್ರ_2 = read_ಪೋರ್ಟ್_etc(gui.vdcmp, &what, &vm, ಗಾತ್ರ(vm),
	    B_TIMEOUT, timeout);

    ಆದರೆ (ಗಾತ್ರ_2 >= 0) {
	ಆಯ್ಕೆ (what) {
	    ಸ್ಥಿತಿ Vim_ಸಂದೇಶ::ಕೀಲಿ_2:
		{
		    char_u *string = vm.u.ಕೀಲಿ_2.chars;
		    ಪೂರ್ಣಾಂಕ len = vm.u.ಕೀಲಿ_2.ಉದ್ದಳತೆ;
		    ಆದರೆ (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_ಒಳಹರಿವು_buf();
			got_int = TRUE;
		    }

		    ಆದರೆ (vm.u.ಕೀಲಿ_2.csi_escape)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_MBYTE_IME
		    {
			ಪೂರ್ಣಾಂಕ	i;
			char_u	buf[2];

			ಗಾಗಿ (i = 0; i < len; ++i)
			{
			    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string + i, 1);
			    ಆದರೆ (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (ಪೂರ್ಣಾಂಕ)KE_CSI;
				ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(buf, 2);
			    }
			}
		    }
#ಇಲ್ಲದಿದ್ದರೆ
			ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf_csi(string, len);
#ಆದರೆ_ಅಂತ್ಯ
		    ಇಲ್ಲದಿದ್ದರೆ
			ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, len);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Vim_ಸಂದೇಶ::Resize:
		gui_resize_shell(vm.u.New_ಗಾತ್ರ.ಅಗಲ, vm.u.New_ಗಾತ್ರ.ಎತ್ತರ);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Vim_ಸಂದೇಶ::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_ಎಣಿಕೆ =
			atomic_ಕೂಡಿಸು(&vm.u.Scroll.sb->scroll_Event_ಎಣಿಕೆ, -1);
		    ಆದರೆ (old_ಎಣಿಕೆ <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.ಮೌಲ್ಯ, vm.u.Scroll.stillDragging);
		}
		ನಿಲ್ಲಿಸು;
#ಆದರೆ defined(FEAT_MENU)
	    ಸ್ಥಿತಿ Vim_ಸಂದೇಶ::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ Vim_ಸಂದೇಶ::Mouse:
		{
		    int32 old_ಎಣಿಕೆ;
		    ಆದರೆ (vm.u.Mouse.button == MOUSE_DRAG)
			old_ಎಣಿಕೆ =
			    atomic_ಕೂಡಿಸು(&gui.vim_ಪಠ್ಯ_Area->mouse_Drag_Event_ಎಣಿಕೆ, -1);
		    ಇಲ್ಲದಿದ್ದರೆ
			old_ಎಣಿಕೆ = 0;
		    ಆದರೆ (old_ಎಣಿಕೆ <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Vim_ಸಂದೇಶ::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Vim_ಸಂದೇಶ::Focus:
		gui.ಒಳ_focus = vm.u.Focus.ಸಕ್ರಿಯ;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		ಆದರೆ (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Vim_ಸಂದೇಶ::Refs:
		::RefsReceived(vm.u.Refs.ಸಂದೇಶ_2, vm.u.Refs.changedir);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Vim_ಸಂದೇಶ::Tabline:
		send_tabline_event(vm.u.Tabline.ಸೂಚ್ಯಂಕ);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Vim_ಸಂದೇಶ::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.ಸೂಚ್ಯಂಕ, vm.u.TablineMenu.event);
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		//  unrecognised message, ignore it
		ನಿಲ್ಲಿಸು;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

/*
 * Here are some functions to protect access to ScreenLines[] and
 * LineOffset[]. These are used from the window thread to respond
 * to a Draw() callback. When that occurs, the window is already
 * locked by the system.
 *
 * Other code that needs to lock is any code that changes these
 * variables. Other read-only access, or access merely to the
 * contents of the screen buffer, need not be locked.
 *
 * If there is no window, don't call Lock() but do succeed.
 */

    ಪೂರ್ಣಾಂಕ
vim_ಬೀಗ_screen()
{
    ಹಿಂತಿರುಗಿ !gui.vim_ಕಿಟಕಿ || gui.vim_ಕಿಟಕಿ->ಬೀಗ_2();
}

    ಶೂನ್ಯ
vim_unlock_screen()
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ)
	gui.vim_ಕಿಟಕಿ->Unlock();
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಸತತ_BAPPLICATION_ಒಳ_NEW_THREAD	0

#ಆದರೆ ಸತತ_BAPPLICATION_ಒಳ_NEW_THREAD

    int32
ಸತತ_vimapp(ಶೂನ್ಯ *ವಾದಗಳು)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.ಸತತ_2();		    // Run until Quit() called

    ಹಿಂತಿರುಗಿ 0;
}

#ಇಲ್ಲದಿದ್ದರೆ

    int32
call_main(ಶೂನ್ಯ *ವಾದಗಳು)
{
    ರಚನೆ Main_ವಾದಗಳು *ma = (Main_ವಾದಗಳು *)ವಾದಗಳು;

    ಹಿಂತಿರುಗಿ ಮುಖ್ಯ(ma->argc, ma->argv);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    ಶೂನ್ಯ
gui_mch_prepare(
	ಪೂರ್ಣಾಂಕ	*argc,
	ಅಕ್ಷರ	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    ಆದರೆ (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(ಕಂಡುಹಿಡಿ_thread(ಖಾಲಿ), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_ಪೋರ್ಟ್(B_MAX_ಪೋರ್ಟ್_ಎಣಿಕೆ, "vim VDCMP");

#ಆದರೆ ಸತತ_BAPPLICATION_ಒಳ_NEW_THREAD
	thread_id tid = spawn_thread(ಸತತ_vimapp, "vim VimApp",
		tinfo.ಆದ್ಯತೆ, ಖಾಲಿ);
	ಆದರೆ (tid >= B_OK) {
	    resume_thread(tid);
	} ಇಲ್ಲದಿದ್ದರೆ {
	    getout(1);
	}
#ಇಲ್ಲದಿದ್ದರೆ
	Main_ವಾದಗಳು ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.ಆದ್ಯತೆ, &ma);
	ಆದರೆ (tid >= B_OK) {
	    VimApp app(appsig);

	    gui.vimApp = &app;
	    resume_thread(tid);
	    /*
	     * This is rather horrible.
	     * call_main will call main() again...
	     * There will be no infinite recursion since
	     * gui.vimApp is set now.
	     */
	    app.ಸತತ_2();		    // Run until Quit() called
	    // fprintf(stderr, "app.Run() returned...\n");
	    ಸ್ಥಿತಿ_t dummy_exitcode;
	    (ಶೂನ್ಯ)wait_for_thread(tid, &dummy_exitcode);

	    /*
	     * This path should be the normal one taken to exit Vim.
	     * The main() thread calls mch_exit() which calls
	     * gui_mch_exit() which terminates its thread.
	     */
	    ನಿರ್ಗಮಿಸು(main_exitcode);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    // Don't fork() when starting the GUI. Spawned threads are not
    // duplicated with a fork(). The result is a mess.
    gui.dofork = FALSE;
    /*
     * XXX Try to determine whether we were started from
     * the Tracker or the terminal.
     * It would be nice to have this work, because the Tracker
     * follows symlinks, so even if you double-click on gvim,
     * when it is a link to vim it will still pass a command name
     * of vim...
     * We try here to see if stdin comes from /dev/null. If so,
     * (or if there is an error, which should never happen) start the GUI.
     * This does the wrong thing for vim - </dev/null, and we're
     * too early to see the command line parsing. Tough.
     * On the other hand, it starts the gui for vim file & which is nice.
     */
    ಆದರೆ (!isatty(0)) {
	ರಚನೆ stat stat_stdin, stat_dev_null;

	ಆದರೆ (fstat(0, &stat_stdin) == -1 ||
		stat("/dev/null", &stat_dev_null) == -1 ||
		(stat_stdin.st_dev == stat_dev_null.st_dev &&
		 stat_stdin.st_ino == stat_dev_null.st_ino))
	    gui.starting = TRUE;
    }
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_init_check(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_ಹಿಂಭಾಗ_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.ಹಿಂಭಾಗ_pixel = gui.def_ಹಿಂಭಾಗ_pixel;

    gui.scrollbar_ಅಗಲ = (ಪೂರ್ಣಾಂಕ) B_V_SCROLL_BAR_ಅಗಲ;
    gui.scrollbar_ಎತ್ತರ = (ಪೂರ್ಣಾಂಕ) B_H_SCROLL_BAR_ಎತ್ತರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    gui.menu_ಎತ್ತರ = 19;   //	initial guess -
    //	correct for my default settings
#ಆದರೆ_ಅಂತ್ಯ
    gui.border_ಸ್ಥಾನಾಂತರ = 3;  //	coordinates are inside window borders

    ಆದರೆ (gui.vdcmp < B_OK)
	ಹಿಂತಿರುಗಿ FAIL;
    get_ಕೀಲಿ_map(&ಕೀಲಿ_Map, &ಕೀಲಿ_Map_Chars);

    gui.vim_ಕಿಟಕಿ = ಹೊಸ Vim_ಕಿಟಕಿ();	// hidden and locked
    ಆದರೆ (!gui.vim_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    gui.vim_ಕಿಟಕಿ->ಸತತ_2();	// Run() unlocks but does not show

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    ಗಣ_ಸಾಮಾನ್ಯ_colors();

    /*
     * Check that none of the colors are the same as the background color
     */
    gui_check_colors();

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();	    // re-init colors and fonts

    gui_mch_new_colors();	// window must exist for this

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    ಶೂನ್ಯ
gui_mch_new_colors()
{
    rgb_color rgb = GUI_ಗಮ್ಯ_RGB(gui.ಹಿಂಭಾಗ_pixel);

    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vimForm->ಗಣ_View_Color(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_open()
{
    ಆದರೆ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_ಗಣ_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಕಿಟಕಿ->Show();
	gui.vim_ಕಿಟಕಿ->Unlock();
	ಹಿಂತಿರುಗಿ OK;
    }

    ಹಿಂತಿರುಗಿ FAIL;
}

    ಶೂನ್ಯ
gui_mch_exit(ಪೂರ್ಣಾಂಕ vim_exitcode)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ) {
	thread_id tid = gui.vim_ಕಿಟಕಿ->Thread();
	gui.vim_ಕಿಟಕಿ->ಬೀಗ_2();
	gui.vim_ಕಿಟಕಿ->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_ಪೋರ್ಟ್(gui.vdcmp);
#ಆದರೆ !ಸತತ_BAPPLICATION_ಒಳ_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (gui.vimApp) {
	Vim_ಪಠ್ಯ_Area_View::guiBlankMouse(ಸುಳ್ಳು);

	main_exitcode = vim_exitcode;
#ಆದರೆ ಸತತ_BAPPLICATION_ಒಳ_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->ಬೀಗ_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#ಇಲ್ಲದಿದ್ದರೆ
	gui.vimApp->ಬೀಗ_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	// suicide
	exit_thread(vim_exitcode);
#ಆದರೆ_ಅಂತ್ಯ
    }
    // If we are somehow still here, let mch_exit() handle things.
}

/*
 * Get the position of the top left corner of the window.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_get_winpos(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	BRect r;
	r = gui.vim_ಕಿಟಕಿ->ಚೌಕಟ್ಟು_2();
	gui.vim_ಕಿಟಕಿ->Unlock();
	*x = (ಪೂರ್ಣಾಂಕ)r.ಎಡ;
	*y = (ಪೂರ್ಣಾಂಕ)r.ತುದಿ;
	ಹಿಂತಿರುಗಿ OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_winpos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಕಿಟಕಿ->Move_ಗಮ್ಯ(x, y);
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
ಶೂನ್ಯ
gui_mch_ಗಣ_shellsize(
	ಪೂರ್ಣಾಂಕ	ಅಗಲ,
	ಪೂರ್ಣಾಂಕ	ಎತ್ತರ,
	ಪೂರ್ಣಾಂಕ	min_ಅಗಲ,
	ಪೂರ್ಣಾಂಕ	min_ಎತ್ತರ,
	ಪೂರ್ಣಾಂಕ	ತಳ_ಅಗಲ,
	ಪೂರ್ಣಾಂಕ	ತಳ_ಎತ್ತರ,
	ಪೂರ್ಣಾಂಕ	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಕಿಟಕಿ->Resize_ಗಮ್ಯ(ಅಗಲ - PEN_ಅಗಲ, ಎತ್ತರ - PEN_ಅಗಲ);

	// set size limits
	ದಶಮಾಂಶ min_ಅಗಲ_2, max_ಅಗಲ, min_ಎತ್ತರ_2, max_ಎತ್ತರ;

	gui.vim_ಕಿಟಕಿ->Get_ಗಾತ್ರ_Limits(&min_ಅಗಲ_2, &max_ಅಗಲ,
		&min_ಎತ್ತರ_2, &max_ಎತ್ತರ);
	gui.vim_ಕಿಟಕಿ->ಗಣ_ಗಾತ್ರ_Limits(min_ಅಗಲ, max_ಅಗಲ,
		min_ಎತ್ತರ, max_ಎತ್ತರ);

	/*
	 * Set the resizing alignment depending on font size.
	 */
	gui.vim_ಕಿಟಕಿ->ಗಣ_ಕಿಟಕಿ_Alignment(
		B_PIXEL_ALIGNMENT,	//  window_alignment mode,
		1,		//  int32 h,
		0,		//  int32 hOffset = 0,
		gui.char_ಅಗಲ,	    //	int32 width = 0,
		ತಳ_ಅಗಲ,	    //	int32 widthOffset = 0,
		1,		//  int32 v = 0,
		0,		//  int32 vOffset = 0,
		gui.char_ಎತ್ತರ,	//  int32 height = 0,
		ತಳ_ಎತ್ತರ	    //	int32 heightOffset = 0
		);

	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

ಶೂನ್ಯ
gui_mch_get_screen_dimensions(
	ಪೂರ್ಣಾಂಕ	*screen_w,
	ಪೂರ್ಣಾಂಕ	*screen_h)
{
    BRect ಚೌಕಟ್ಟು;

    {
	BScreen screen(gui.vim_ಕಿಟಕಿ);

	ಆದರೆ (screen.IsValid()) {
	    ಚೌಕಟ್ಟು = screen.ಚೌಕಟ್ಟು_2();
	} ಇಲ್ಲದಿದ್ದರೆ {
	    ಚೌಕಟ್ಟು.ಬಲ = 640;
	    ಚೌಕಟ್ಟು.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (ಪೂರ್ಣಾಂಕ) ಚೌಕಟ್ಟು.ಬಲ - 2 * gui.scrollbar_ಅಗಲ - 20;
    *screen_h = (ಪೂರ್ಣಾಂಕ) ಚೌಕಟ್ಟು.bottom - gui.scrollbar_ಎತ್ತರ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	- gui.menu_ಎತ್ತರ
#ಆದರೆ_ಅಂತ್ಯ
	- 30;
}

ಶೂನ್ಯ
gui_mch_ಗಣ_ಪಠ್ಯ_area_pos(
	ಪೂರ್ಣಾಂಕ	x,
	ಪೂರ್ಣಾಂಕ	y,
	ಪೂರ್ಣಾಂಕ	w,
	ಪೂರ್ಣಾಂಕ	h)
{
    ಆದರೆ (!gui.vim_ಪಠ್ಯ_Area)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಪಠ್ಯ_Area->Move_ಗಮ್ಯ(x, y);
	gui.vim_ಪಠ್ಯ_Area->Resize_ಗಮ್ಯ(w - PEN_ಅಗಲ, h - PEN_ಅಗಲ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
	ಆದರೆ (gui.vimForm->Tab_ಸಾಲು() != ಖಾಲಿ) {
	    gui.vimForm->Tab_ಸಾಲು()->Resize_ಗಮ್ಯ(w, gui.vimForm->Tabline_ಎತ್ತರ());
	}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_TABLINE

	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

ಶೂನ್ಯ
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	ಪೂರ್ಣಾಂಕ	flag)
{
    VimScrollBar *vsb = sb->id;
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	ಆದರೆ (flag) {
	    ಆದರೆ (vsb->IsHidden()) {
		vsb->Show();
	    }
	} ಇಲ್ಲದಿದ್ದರೆ {
	    ಆದರೆ (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

ಶೂನ್ಯ
gui_mch_ಗಣ_scrollbar_thumb(
	scrollbar_T *sb,
	ಪೂರ್ಣಾಂಕ	val,
	ಪೂರ್ಣಾಂಕ	ಗಾತ್ರ_2,
	ಪೂರ್ಣಾಂಕ	max)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	VimScrollBar *s = sb->id;
	ಆದರೆ (max == 0) {
	    s->ಗಣ_ಮೌಲ್ಯ(0);
	    s->ಗಣ_Range(0.0, 0.0);
	} ಇಲ್ಲದಿದ್ದರೆ {
	    s->ಗಣ_Proportion((ದಶಮಾಂಶ)ಗಾತ್ರ_2 / (max + 1.0));
	    s->ಗಣ_ಹೆಜ್ಜೆಗಳು(1.0, ಗಾತ್ರ_2 > 5 ? ಗಾತ್ರ_2 - 2 : ಗಾತ್ರ_2);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SCROLL_PAST_ಕೊನೆ	    //	really only defined in gui.c...
	    max = max + 1 - ಗಾತ್ರ_2;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (max < s->ಮೌಲ್ಯ_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->ಗಣ_ಮೌಲ್ಯ(val);
		s->ಗಣ_Range(0.0, max);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->ಗಣ_Range(0.0, max);
		s->ಗಣ_ಮೌಲ್ಯ(val);
	    }
	}
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

ಶೂನ್ಯ
gui_mch_ಗಣ_scrollbar_pos(
	scrollbar_T *sb,
	ಪೂರ್ಣಾಂಕ	x,
	ಪೂರ್ಣಾಂಕ	y,
	ಪೂರ್ಣಾಂಕ	w,
	ಪೂರ್ಣಾಂಕ	h)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	BRect winb = gui.vim_ಕಿಟಕಿ->Bounds();
	ದಶಮಾಂಶ vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->Resize_ಗಮ್ಯ(w - PEN_ಅಗಲ, h - PEN_ಅಗಲ);
	ಆದರೆ (winb.ಬಲ-(x+w)<w) vsbx = winb.ಬಲ - (w - PEN_ಅಗಲ);
	vsb->Move_ಗಮ್ಯ(vsbx, vsby);
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

    ಪೂರ್ಣಾಂಕ
gui_mch_get_scrollbar_xpadding(ಶೂನ್ಯ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ಹಿಂತಿರುಗಿ 0;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_get_scrollbar_ypadding(ಶೂನ್ಯ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	ಪೂರ್ಣಾಂಕ	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = ಹೊಸ VimScrollBar(sb, posture);
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	vsb->ಗಣ_ಗುರಿ(gui.vim_ಪಠ್ಯ_Area);
	vsb->Hide();
	gui.vimForm->ಕೂಡಿಸು_ಮಗು(vsb);
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

#ಆದರೆ defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
ಶೂನ್ಯ
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	sb->id->RemoveSelf();
	ಅಳಿಸು sb->id;
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Cursor does not flash
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_is_blink_off(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON blinking, cursor is shown
 */

#ವ್ಯಾಖ್ಯಾನಿಸು BLINK_NONE  0
#ವ್ಯಾಖ್ಯಾನಿಸು BLINK_OFF   1
#ವ್ಯಾಖ್ಯಾನಿಸು BLINK_ON    2

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	blink_state = BLINK_NONE;
ಸ್ಥಿರ long_u	    blink_waittime = 700;
ಸ್ಥಿರ long_u	    blink_ontime = 400;
ಸ್ಥಿರ long_u	    blink_offtime = 250;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  blink_timer = 0;

ಶೂನ್ಯ
gui_mch_ಗಣ_blinking(
	ದೀರ್ಘ	waittime,
	ದೀರ್ಘ	on,
	ದೀರ್ಘ	off)
{
    // TODO
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ಶೂನ್ಯ
gui_mch_stop_blink(ಪೂರ್ಣಾಂಕ may_call_gui_update_cursor)
{
    // TODO
    ಆದರೆ (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    ಆದರೆ (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    ಶೂನ್ಯ
gui_mch_ಆರಂಭ_blink()
{
    // TODO
    ಆದರೆ (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    ಆದರೆ (blink_waittime && blink_ontime && blink_offtime && gui.ಒಳ_focus)
    {
	blink_timer = 1; // XtAppAddTimeOut(app_context, blink_waittime,
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

/*
 * Initialise vim to use the font with the given name.	Return FAIL if the font
 * could not be loaded, OK otherwise.
 */
ಪೂರ್ಣಾಂಕ
gui_mch_init_font(
	char_u	    *font_name,
	ಪೂರ್ಣಾಂಕ	    fontset)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2())
    {
	ಪೂರ್ಣಾಂಕ rc = gui.vim_ಪಠ್ಯ_Area->mchInitFont(font_name);
	gui.vim_ಕಿಟಕಿ->Unlock();

	ಹಿಂತಿರುಗಿ rc;
    }

    ಹಿಂತಿರುಗಿ FAIL;
}


    ಪೂರ್ಣಾಂಕ
gui_mch_adjust_charsize()
{
    ಹಿಂತಿರುಗಿ FAIL;
}


    ಪೂರ್ಣಾಂಕ
gui_mch_font_dialog(font_family* family, font_style* style, ದಶಮಾಂಶ* ಗಾತ್ರ_2)
{
#ಆದರೆ defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = ಹೊಸ VimSelectFontDialog(family, style, ಗಾತ್ರ_2);
    ತಾರ್ಕಿಕ ret = dialog->Go();
	ಅಳಿಸು dialog;
	ಹಿಂತಿರುಗಿ ret;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ NOFONT;
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	ಪೂರ್ಣಾಂಕ	    giveErrorIfMissing)
{
    ಸ್ಥಿರ VimFont *fontList = ಖಾಲಿ;

    ಆದರೆ (!gui.ಒಳ_use)	//  can't do this when GUI not running
	ಹಿಂತಿರುಗಿ NOFONT;

    //	storage for locally modified name;
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ buff_ಗಾತ್ರ = B_FONT_FAMILY_ಉದ್ದಳತೆ + B_FONT_STYLE_ಉದ್ದಳತೆ + 20;
    ಸ್ಥಿರ ಅಕ್ಷರ font_name[buff_ಗಾತ್ರ] = {0};
    font_family family = {0};
    font_style	style  = {0};
    ದಶಮಾಂಶ ಗಾತ್ರ_2 = 0.f;

    ಆದರೆ (name == 0 && be_fixed_font == 0) {
	ಆದರೆ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	ಹಿಂತಿರುಗಿ NOFONT;
    }

    ತಾರ್ಕಿಕ useSelectGUI = ಸುಳ್ಳು;
    ಆದರೆ (name != ಖಾಲಿ)
	ಆದರೆ (STRCMP(name, "*") == 0) {
	    useSelectGUI = ನಿಜ;
	    STRNCPY(font_name, hl_get_font_name(), buff_ಗಾತ್ರ);
	} ಇಲ್ಲದಿದ್ದರೆ
	    STRNCPY(font_name, name, buff_ಗಾತ್ರ);

    ಆದರೆ (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	ಗಾತ್ರ_2 = be_fixed_font->ಗಾತ್ರ_3();
	vim_snprintf(font_name, buff_ಗಾತ್ರ,
	    (ಅಕ್ಷರ*)"%s/%s/%.0f", family, style, ಗಾತ್ರ_2);
    }

    //	replace underscores with spaces
    ಅಕ್ಷರ* ಕೊನೆ = 0;
    ಆಗ (ಕೊನೆ = strchr((ಅಕ್ಷರ *)font_name, '_'))
	*ಕೊನೆ = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    ಸ್ಥಿರ ಅಕ್ಷರ buff[buff_ಗಾತ್ರ] = {0};
    STRNCPY(buff, font_name, buff_ಗಾತ್ರ);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_ಉದ್ದಳತೆ);
    ಅಕ್ಷರ* style_s = strtok(0, "/\0");
    ಆದರೆ (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_ಉದ್ದಳತೆ);
    ಗಾತ್ರ_2 = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    ಆದರೆ (useSelectGUI) {
	ಆದರೆ (gui_mch_font_dialog(&family, &style, &ಗಾತ್ರ_2) == NOFONT)
	    ಹಿಂತಿರುಗಿ FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_ಗಾತ್ರ,
		(ಅಕ್ಷರ*)"%s/%s/%.0f", family, style, ಗಾತ್ರ_2);
	hl_ಗಣ_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	ಆದರೆ (new_p_guifont != ಖಾಲಿ) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    ಗಾಗಿ ( ; *new_p_guifont; ++new_p_guifont)
		ಆದರೆ (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    ಗಾಗಿ (flp = fontList; flp; flp = flp->ಮುಂದಿನ) {
	ಆದರೆ (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    ಹಿಂತಿರುಗಿ (GuiFont)flp;
	}
    }

    VimFont *font = ಹೊಸ VimFont();
    font->name = vim_strsave((char_u*)font_name);

    ಆದರೆ (ಎಣಿಕೆ_font_styles(family) <= 0) {
	ಆದರೆ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	ಅಳಿಸು font;
	ಹಿಂತಿರುಗಿ NOFONT;
    }

    //	Remember font in the static list for later use
    font->ಮುಂದಿನ = fontList;
    fontList = font;

    font->ಗಣ_Family_And_Style(family, style);
    ಆದರೆ (ಗಾತ್ರ_2 > 0.f)
	font->ಗಣ_ಗಾತ್ರ(ಗಾತ್ರ_2);

    font->ಗಣ_Spacing(B_FIXED_SPACING);
    font->ಗಣ_Encoding(B_UNICODE_UTF8);

    ಹಿಂತಿರುಗಿ (GuiFont)font;
}

/*
 * Set the current text font.
 */
ಶೂನ್ಯ
gui_mch_ಗಣ_font(
	GuiFont	font)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	VimFont *vf = (VimFont *)font;

	gui.vim_ಪಠ್ಯ_Area->ಗಣ_Font(vf);

	gui.char_ಅಗಲ = (ಪೂರ್ಣಾಂಕ) vf->String_ಅಗಲ("n");
	font_ಎತ್ತರ fh;
	vf->Get_ಎತ್ತರ(&fh);
	gui.char_ಎತ್ತರ = (ಪೂರ್ಣಾಂಕ)(fh.ascent + 0.9999)
	    + (ಪೂರ್ಣಾಂಕ)(fh.descent + 0.9999) + (ಪೂರ್ಣಾಂಕ)(fh.leading + 0.9999);
	gui.char_ascent = (ಪೂರ್ಣಾಂಕ)(fh.ascent + 0.9999);

	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

// XXX TODO This is apparently never called...
ಶೂನ್ಯ
gui_mch_free_font(
	GuiFont	font)
{
    ಆದರೆ (font == NOFONT)
	ಹಿಂತಿರುಗಿ;
    VimFont *f = (VimFont *)font;
    ಆದರೆ (--f->refcount <= 0) {
	ಆದರೆ (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	ಅಳಿಸು f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_adjust_charheight()
{

    // TODO: linespace support?

// #ifdef FEAT_XFONTSET
//  if (gui.fontset != NOFONTSET)
//  {
//  gui.char_height = fontset_height((XFontSet)gui.fontset) + p_linespace;
//  gui.char_ascent = fontset_ascent((XFontSet)gui.fontset)
//  + p_linespace / 2;
//  }
//  else
// #endif
    {
	VimFont *font = (VimFont *)gui.norm_font;
	font_ಎತ್ತರ fh = {0};
	font->Get_ಎತ್ತರ(&fh);
	gui.char_ಎತ್ತರ = (ಪೂರ್ಣಾಂಕ)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (ಪೂರ್ಣಾಂಕ)(fh.ascent + 0.5) + p_linespace / 2;
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
gui_mch_getmouse(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y)
{
    fprintf(stderr, "gui_mch_getmouse");

    /*int	rootx, rooty, winx, winy;
      Window	root, child;
      unsigned int mask;

      if (gui.wid && XQueryPointer(gui.dpy, gui.wid, &root, &child,
      &rootx, &rooty, &winx, &winy, &mask)) {
     *x = winx;
     *y = winy;
     } else*/ {
	 *x = -1;
	 *y = -1;
     }
}

    ಶೂನ್ಯ
gui_mch_mousehide(ಪೂರ್ಣಾಂಕ hide)
{
    fprintf(stderr, "gui_mch_getmouse");
    //	TODO
}

/*
 * This function has been lifted from gui_w32.c and extended a bit.
 *
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
guicolor_T
gui_mch_get_color(
	char_u	*name)
{
    ಹಿಂತಿರುಗಿ gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
ಶೂನ್ಯ
gui_mch_ಗಣ_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_ಗಮ್ಯ_RGB(color);
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಪಠ್ಯ_Area->ಗಣ_ಮೇಲಿನ_Color(rgb);
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

/*
 * Set the current text background color.
 */
ಶೂನ್ಯ
gui_mch_ಗಣ_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_ಗಮ್ಯ_RGB(color);
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಪಠ್ಯ_Area->ಗಣ_ಕೆಳಗಿನ_Color(rgb);
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

ಶೂನ್ಯ
gui_mch_draw_string(
	ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು,
	char_u	*s,
	ಪೂರ್ಣಾಂಕ	len,
	ಪೂರ್ಣಾಂಕ	flags)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಪಠ್ಯ_Area->mchDrawString(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, s, len, flags);
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(ಪೂರ್ಣಾಂಕ r, ಪೂರ್ಣಾಂಕ g, ಪೂರ್ಣಾಂಕ b)
{
    ಹಿಂತಿರುಗಿ gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
ಪೂರ್ಣಾಂಕ
gui_mch_haskey(
	char_u	*name)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; special_ಕೀಲಿಗಳು[i].Be_ಕೀಲಿಗಳು != 0; i++)
	ಆದರೆ (name[0] == special_ಕೀಲಿಗಳು[i].vim_code0 &&
		name[1] == special_ಕೀಲಿಗಳು[i].vim_code1)
	    ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಶೂನ್ಯ
gui_mch_beep()
{
    ::beep();
}

    ಶೂನ್ಯ
gui_mch_flash(ಪೂರ್ಣಾಂಕ msec)
{
    // Do a visual beep by reversing the foreground and background colors

    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	BRect rect = gui.vim_ಪಠ್ಯ_Area->Bounds();

	gui.vim_ಪಠ್ಯ_Area->ಗಣ_Drawing_ಬಹುಲಕ(B_OP_INVERT);
	gui.vim_ಪಠ್ಯ_Area->FillRect(rect);
	gui.vim_ಪಠ್ಯ_Area->Sync();
	snooze(msec * 1000);	 // wait for a few msec
	gui.vim_ಪಠ್ಯ_Area->FillRect(rect);
	gui.vim_ಪಠ್ಯ_Area->ಗಣ_Drawing_ಬಹುಲಕ(B_OP_COPY);
	gui.vim_ಪಠ್ಯ_Area->Flush();
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
ಶೂನ್ಯ
gui_mch_invert_rectangle(
	ಪೂರ್ಣಾಂಕ	r,
	ಪೂರ್ಣಾಂಕ	c,
	ಪೂರ್ಣಾಂಕ	nr,
	ಪೂರ್ಣಾಂಕ	nc)
{
    BRect rect;
    rect.ಎಡ = FILL_X(c);
    rect.ತುದಿ = FILL_Y(r);
    rect.ಬಲ = rect.ಎಡ + nc * gui.char_ಅಗಲ - PEN_ಅಗಲ;
    rect.bottom = rect.ತುದಿ + nr * gui.char_ಎತ್ತರ - PEN_ಅಗಲ;

    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಪಠ್ಯ_Area->ಗಣ_Drawing_ಬಹುಲಕ(B_OP_INVERT);
	gui.vim_ಪಠ್ಯ_Area->FillRect(rect);
	gui.vim_ಪಠ್ಯ_Area->ಗಣ_Drawing_ಬಹುಲಕ(B_OP_COPY);
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    ಶೂನ್ಯ
gui_mch_iconify()
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಕಿಟಕಿ->Minimize(ನಿಜ);
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_foreground(ಶೂನ್ಯ)
{
    // TODO
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the window title
 */
ಶೂನ್ಯ
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಕಿಟಕಿ->ಗಣ_Title((ಅಕ್ಷರ *)title);
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    ಶೂನ್ಯ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_ಗಣ_fg_color(color);

    BRect r;
    r.ಎಡ = FILL_X(gui.ಲಂಬಸಾಲು);
    r.ತುದಿ = FILL_Y(gui.ಅಡ್ಡಸಾಲು);
    ಪೂರ್ಣಾಂಕ cells = utf_off2cells(ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.ಅಡ್ಡಸಾಲು] + gui.ಲಂಬಸಾಲು, 100); // TODO-TODO
    ಆದರೆ (cells>=4) cells = 1;
    r.ಬಲ = r.ಎಡ + cells*gui.char_ಅಗಲ - PEN_ಅಗಲ;
    r.bottom = r.ತುದಿ + gui.char_ಎತ್ತರ - PEN_ಅಗಲ;

    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಪಠ್ಯ_Area->StrokeRect(r);
	gui.vim_ಕಿಟಕಿ->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
ಶೂನ್ಯ
gui_mch_draw_part_cursor(
	ಪೂರ್ಣಾಂಕ	w,
	ಪೂರ್ಣಾಂಕ	h,
	guicolor_T  color)
{
    gui_mch_ಗಣ_fg_color(color);

    BRect r;
    r.ಎಡ =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_ಬಲ ? FILL_X(gui.ಲಂಬಸಾಲು + 1) - w :
#ಆದರೆ_ಅಂತ್ಯ
	FILL_X(gui.ಲಂಬಸಾಲು);
    r.ಬಲ = r.ಎಡ + w - PEN_ಅಗಲ;
    r.bottom = FILL_Y(gui.ಅಡ್ಡಸಾಲು + 1) - PEN_ಅಗಲ;
    r.ತುದಿ = r.bottom - h + PEN_ಅಗಲ;

    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಪಠ್ಯ_Area->FillRect(r);
	gui.vim_ಕಿಟಕಿ->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    ಶೂನ್ಯ
gui_mch_update()
{
    gui_mch_flush();
    ಆಗ (ಪೋರ್ಟ್_ಎಣಿಕೆ(gui.vdcmp) > 0 &&
	    !vim_is_ಒಳಹರಿವು_buf_ತುಂಬಿದ() &&
	    gui_haiku_process_event(0) >= B_OK)
	/* nothing */ ;
}

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1	    Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
ಪೂರ್ಣಾಂಕ
gui_mch_wait_for_chars(
	ದೀರ್ಘ	wtime)
{
    ಪೂರ್ಣಾಂಕ		focus;
    bigtime_t	until, timeout;
    ಸ್ಥಿತಿ_t	st;

    ಆದರೆ (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_ಸಮಯ() + timeout;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.ಒಳ_focus;
    ಗಾಗಿ (;;)
    {
	// Stop or start blinking when focus changes
	ಆದರೆ (gui.ಒಳ_focus != focus)
	{
	    ಆದರೆ (gui.ಒಳ_focus)
		gui_mch_ಆರಂಭ_blink();
	    ಇಲ್ಲದಿದ್ದರೆ
		gui_mch_stop_blink(TRUE);
	    focus = gui.ಒಳ_focus;
	}

	gui_mch_flush();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಂದೇಶ_ಸರತಿ
# ifdef FEAT_TIMERS
	did_ಕೂಡಿಸು_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ಆದರೆ (did_ಕೂಡಿಸು_timer)
	    // Need to recompute the waiting time.
	    ನಿಲ್ಲಿಸು;
# endif
# ifdef FEAT_ಕೆಲಸ_CHANNEL
	ಆದರೆ (has_any_channel())
	{
	    ಆದರೆ (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wtime < 0)
	    timeout = B_INFINITE_TIMEOUT;
# endif
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use gui_haiku_process_event() to hang until
	 * an event arrives.  No need to check for input_buf_full because we
	 * are returning as soon as it contains a single char.
	 */
	st = gui_haiku_process_event(timeout);

	ಆದರೆ (ಒಳಹರಿವು_ಲಭ್ಯ())
	    ಹಿಂತಿರುಗಿ OK;
	ಆದರೆ (st < B_OK)		// includes B_TIMED_OUT
	    ಹಿಂತಿರುಗಿ FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	ಆದರೆ (wtime >= 0)
	{
	    timeout = until - system_ಸಮಯ();
	    ಆದರೆ (timeout < 0)
		ನಿಲ್ಲಿಸು;
	}
    }
    ಹಿಂತಿರುಗಿ FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    ಶೂನ್ಯ
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಕಿಟಕಿ->Flush();
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
    ಹಿಂತಿರುಗಿ;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
ಶೂನ್ಯ
gui_mch_clear_ಖಂಡ(
	ಪೂರ್ಣಾಂಕ	row1,
	ಪೂರ್ಣಾಂಕ	col1,
	ಪೂರ್ಣಾಂಕ	row2,
	ಪೂರ್ಣಾಂಕ	col2)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಪಠ್ಯ_Area->mch_Clear_ಖಂಡ(row1, col1, row2, col2);
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

    ಶೂನ್ಯ
gui_mch_clear_all()
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	gui.vim_ಪಠ್ಯ_Area->mchClearAll();
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
ಶೂನ್ಯ
gui_mch_delete_ಸಾಲುಗಳು(
	ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	ಅಂಕೆ_ಸಾಲುಗಳು)
{
    gui.vim_ಪಠ್ಯ_Area->mch_Delete_ಸಾಲುಗಳು(ಅಡ್ಡಸಾಲು, ಅಂಕೆ_ಸಾಲುಗಳು);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
ಶೂನ್ಯ
gui_mch_ಸೇರಿಸು_ಸಾಲುಗಳು(
	ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	ಅಂಕೆ_ಸಾಲುಗಳು)
{
    gui.vim_ಪಠ್ಯ_Area->mch_ಸೇರಿಸು_ಸಾಲುಗಳು(ಅಡ್ಡಸಾಲು, ಅಂಕೆ_ಸಾಲುಗಳು);
}

#ಆದರೆ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ಶೂನ್ಯ
gui_mch_enable_menu(
	ಪೂರ್ಣಾಂಕ	flag)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->ಗಣ_Enabled(flag);
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

ಶೂನ್ಯ
gui_mch_ಗಣ_menu_pos(
	ಪೂರ್ಣಾಂಕ	x,
	ಪೂರ್ಣಾಂಕ	y,
	ಪೂರ್ಣಾಂಕ	w,
	ಪೂರ್ಣಾಂಕ	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
ಶೂನ್ಯ
gui_mch_ಕೂಡಿಸು_menu(
	vimmenu_T   *menu,
	ಪೂರ್ಣಾಂಕ	idx)
{
    vimmenu_T	*ಪೋಷಕ = menu->ಪೋಷಕ;

    //	popup menu - just create it unattached
    ಆದರೆ (menu_is_popup(menu->name) && ಪೋಷಕ == ಖಾಲಿ) {
	BPop_ಮೇಲೆ_Menu* ತೆಗೆ_ಮೇಲೆ_Menu = ಹೊಸ BPop_ಮೇಲೆ_Menu((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)menu->name, ಸುಳ್ಳು, ಸುಳ್ಳು);
	menu->submenu_id = ತೆಗೆ_ಮೇಲೆ_Menu;
	menu->id = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (!menu_is_menubar(menu->name)
	    || (ಪೋಷಕ != ಖಾಲಿ && ಪೋಷಕ->submenu_id == ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	ಆದರೆ ( ಪೋಷಕ )
	    tmp = ಪೋಷಕ->submenu_id;
	ಇಲ್ಲದಿದ್ದರೆ
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	ಆದರೆ ( ! tmp->ಕಂಡುಹಿಡಿ_ಅಂಶ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) menu->dname)) {

	    BMenu *bmenu = ಹೊಸ BMenu((ಅಕ್ಷರ *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->ಕೂಡಿಸು_ಅಂಶ(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->ಕಂಡುಹಿಡಿ_ಅಂಶ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) menu->dname);

	}
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

    ಶೂನ್ಯ
gui_mch_toggle_tearoffs(ಪೂರ್ಣಾಂಕ enable)
{
    // no tearoff menus
}

    ಸ್ಥಿರ BMessage *
Menu_ಸಂದೇಶ(vimmenu_T *menu)
{
    BMessage *m = ಹೊಸ BMessage('menu');
    m->ಕೂಡಿಸು_ಸೂಚಕ("VimMenu", (ಶೂನ್ಯ *)menu);

    ಹಿಂತಿರುಗಿ m;
}

/*
 * Add a menu item to a menu
 */
ಶೂನ್ಯ
gui_mch_ಕೂಡಿಸು_menu_ಅಂಶ(
	vimmenu_T   *menu,
	ಪೂರ್ಣಾಂಕ	idx)
{
    ಪೂರ್ಣಾಂಕ	    mnemonic = 0;
    vimmenu_T	*ಪೋಷಕ = menu->ಪೋಷಕ;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2())
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
	ಆದರೆ (menu_is_toolbar(ಪೋಷಕ->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ಆದರೆ (toolbar != ಖಾಲಿ) {
		toolbar->ಕೂಡಿಸು_Button(idx, menu);
	    }
	} ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (ಪೋಷಕ->submenu_id != ಖಾಲಿ || menu_is_popup(ಪೋಷಕ->name)) {
	    ಆದರೆ (menu_is_separator(menu->name)) {
		BSeparator_ಅಂಶ *ಅಂಶ = ಹೊಸ BSeparator_ಅಂಶ();
		ಪೋಷಕ->submenu_id->ಕೂಡಿಸು_ಅಂಶ(ಅಂಶ);
		menu->id = ಅಂಶ;
		menu->submenu_id = ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ {
		BMenu_ಅಂಶ *ಅಂಶ = ಹೊಸ BMenu_ಅಂಶ((ಅಕ್ಷರ *)menu->dname,
			Menu_ಸಂದೇಶ(menu));
		ಅಂಶ->ಗಣ_ಗುರಿ(gui.vim_ಪಠ್ಯ_Area);
		ಅಂಶ->ಗಣ_Trigger((ಅಕ್ಷರ) menu->mnemonic);
		ಪೋಷಕ->submenu_id->ಕೂಡಿಸು_ಅಂಶ(ಅಂಶ);
		menu->id = ಅಂಶ;
		menu->submenu_id = ಖಾಲಿ;
	    }
	}
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
ಶೂನ್ಯ
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2())
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
	ಆದರೆ (menu->ಪೋಷಕ && menu_is_toolbar(menu->ಪೋಷಕ->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ಆದರೆ (toolbar != ಖಾಲಿ) {
		toolbar->RemoveButton(menu);
	    }
	} ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    ಪ್ರತಿಜ್ಞಾ(menu->submenu_id == ಖಾಲಿ || menu->submenu_id->ಎಣಿಕೆ_ವಸ್ತುಗಳು() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    ಆದರೆ (bmenu)
	    {
		bmenu->Remove_ಅಂಶ(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		ಆದರೆ (bmenu == gui.vimForm->MenuBar() && bmenu->ಎಣಿಕೆ_ವಸ್ತುಗಳು() == 0)
		{
		    bmenu->Resize_ಗಮ್ಯ(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    ಅಳಿಸು menu->id;
	    menu->id = ಖಾಲಿ;
	    menu->submenu_id = ಖಾಲಿ;

	    gui.menu_ಎತ್ತರ = (ಪೂರ್ಣಾಂಕ) gui.vimForm->Menu_ಎತ್ತರ();
	}
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
ಶೂನ್ಯ
gui_mch_menu_grey(
	vimmenu_T   *menu,
	ಪೂರ್ಣಾಂಕ	grey)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    ಆದರೆ (menu->ಪೋಷಕ && menu_is_toolbar(menu->ಪೋಷಕ->name)) {
	ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ಆದರೆ (toolbar != ಖಾಲಿ) {
		toolbar->ಬೂದು_Button(menu, grey);
	    }
	    gui.vim_ಕಿಟಕಿ->Unlock();
	}
    } ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (menu->id != ಖಾಲಿ)
	menu->id->ಗಣ_Enabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
ಶೂನ್ಯ
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	ಪೂರ್ಣಾಂಕ	hidden)
{
    ಆದರೆ (menu->id != ಖಾಲಿ)
	menu->id->ಗಣ_Enabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    ಶೂನ್ಯ
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    ಶೂನ್ಯ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    ಆದರೆ (!menu_is_popup(menu->name) || menu->submenu_id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    BPop_ಮೇಲೆ_Menu* popupMenu = dynamic_cast<BPop_ಮೇಲೆ_Menu*>(menu->submenu_id);
    ಆದರೆ (popupMenu == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    BPoint ಬಿಂದು;
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	uint32 buttons = 0;
	gui.vim_ಪಠ್ಯ_Area->GetMouse(&ಬಿಂದು, &buttons);
	gui.vim_ಪಠ್ಯ_Area->Convert_ಗಮ್ಯ_Screen(&ಬಿಂದು);
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
    popupMenu->Go(ಬಿಂದು, ನಿಜ);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU

// Mouse stuff

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
ಅಕ್ಷರ textplain[] = "text/plain";
ಅಕ್ಷರ vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    ಶೂನ್ಯ
clip_mch_request_selection(Clipboard_T *cbd)
{
    ಆದರೆ (be_clipboard->ಬೀಗ_2())
    {
	BMessage *m = be_clipboard->ದತ್ತಾಂಶ_2();
	// m->PrintToStream();

	char_u *string = ಖಾಲಿ;
	ssize_t stringlen = -1;

	ಆದರೆ (m->ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ(textplain, B_MIME_TYPE,
		    (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **)&string, &stringlen) == B_OK
		|| m->ಕಂಡುಹಿಡಿ_String("text", (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **)&string) == B_OK)
	{
	    ಆದರೆ (stringlen == -1)
		stringlen = STRLEN(string);

	    ಪೂರ್ಣಾಂಕ type;
	    ಅಕ್ಷರ *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    ಆದರೆ (m->ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ(vimselectiontype, B_MIME_TYPE,
			(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **)&seltype, &seltypelen) == B_OK)
	    {
		ಆಯ್ಕೆ (*seltype)
		{
		    ಪೂರ್ವನಿಯೋಜಿತ:
		    ಸ್ಥಿತಿ 'L':	type = MLINE;	ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'C':	type = MCHAR;	ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VISUAL
		    ಸ್ಥಿತಿ 'B':	type = MBLOCK;	ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Otherwise use heuristic as documented
		type = memchr(string, stringlen, '\n') ? MLINE : MCHAR;
	    }
	    clip_yank_selection(type, string, (ದೀರ್ಘ)stringlen, cbd);
	}
	be_clipboard->Unlock();
    }
}
/*
 * Make vim the owner of the current selection.
 */
    ಶೂನ್ಯ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    ಪೂರ್ಣಾಂಕ
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    ಶೂನ್ಯ
clip_mch_ಗಣ_selection(Clipboard_T *cbd)
{
    ಆದರೆ (be_clipboard->ಬೀಗ_2())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->ದತ್ತಾಂಶ_2();
	ಪ್ರತಿಜ್ಞಾ(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = ಖಾಲಿ;
	long_u	ಎಣಿಕೆ_2;
	ಪೂರ್ಣಾಂಕ type;

	type = clip_convert_selection(&str, &ಎಣಿಕೆ_2, cbd);

	ಆದರೆ (type < 0)
	    ಹಿಂತಿರುಗಿ;

	m->ಕೂಡಿಸು_ದತ್ತಾಂಶ(textplain, B_MIME_TYPE, (ಶೂನ್ಯ *)str, ಎಣಿಕೆ_2);

	// Add type of selection
	ಅಕ್ಷರ	vtype;
	ಆಯ್ಕೆ (type)
	{
	    ಪೂರ್ವನಿಯೋಜಿತ:
	    ಸ್ಥಿತಿ MLINE:    vtype = 'L';    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MCHAR:    vtype = 'C';    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VISUAL
	    ಸ್ಥಿತಿ MBLOCK:   vtype = 'B';    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	}
	m->ಕೂಡಿಸು_ದತ್ತಾಂಶ(vimselectiontype, B_MIME_TYPE, (ಶೂನ್ಯ *)&vtype, 1);

	vim_free(str);

	be_clipboard->Commit();
	be_clipboard->Unlock();
    }
}

#ಆದರೆ_ಅಂತ್ಯ	// FEAT_CLIPBOARD

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
/*
 * Pop open a file browser and return the file selected, in allocated memory,
 * or NULL if Cancel is hit.
 *  saving  - TRUE if the file will be saved to, FALSE if it will be opened.
 *  title   - Title message for the file browser dialog.
 *  dflt    - Default name of file.
 *  ext     - Default extension to be added to files without extensions.
 *  initdir - directory in which to open the browser (NULL = current dir)
 *  filter  - Filter for matched files to choose from.
 *  Has a format like this:
 *  "C Files (*.c)\0*.c\0"
 *  "All Files\0*.*\0\0"
 *  If these two strings were concatenated, then a choice of two file
 *  filters will be selectable to the user.  Then only matching files will
 *  be shown in the browser.  If NULL, the default allows all files.
 *
 *  *NOTE* - the filter string must be terminated with TWO nulls.
 */
char_u *
gui_mch_browse(
	ಪೂರ್ಣಾಂಕ saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = ಹೊಸ BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    ಖಾಲಿ, ಖಾಲಿ, 0, ಸುಳ್ಳು,
	    ಹೊಸ BMessage((saving == TRUE) ? 'save' : 'open'), ಖಾಲಿ, ನಿಜ);

    gui.vimApp->f_Browsed_ಮಾರ್ಗ.Unset();

    gui.vimApp->fFilePanel->ಕಿಟಕಿ()->ಗಣ_Title((ಅಕ್ಷರ*)title);
    gui.vimApp->fFilePanel->ಗಣ_Panel_Directory((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    ಆಗ (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = ಖಾಲಿ;
    ಸ್ಥಿತಿ_t ಫಲಿತಾಂಶ = gui.vimApp->f_Browsed_ಮಾರ್ಗ.InitCheck();
    ಆದರೆ (ಫಲಿತಾಂಶ == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->f_Browsed_ಮಾರ್ಗ.ಮಾರ್ಗ_2());
    } ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (ಫಲಿತಾಂಶ != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    ಫಲಿತಾಂಶ, strerror(ಫಲಿತಾಂಶ));
	}

    ಅಳಿಸು gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ fileName;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_BROWSE


#ಆದರೆ defined(FEAT_GUI_DIALOG)

/*
 * Create a dialog dynamically from the parameter strings.
 * type	    = type of dialog (question, alert, etc.)
 * title    = dialog title. may be NULL for default title.
 * message  = text to display. Dialog sizes to accommodate it.
 * buttons  = '\n' separated list of button captions, default first.
 * dfltbutton	= number of default button.
 *
 * This routine returns 1 if the first button is pressed,
 *	    2 for the second, etc.
 *
 *	    0 indicates Esc was pressed.
 *	    -1 for unexpected error
 *
 * If stubbing out this fn, return 1.
 */

ಪೂರ್ಣಾಂಕ
gui_mch_dialog(
	ಪೂರ್ಣಾಂಕ	 type,
	char_u	*title,
	char_u	*ಸಂದೇಶ_2,
	char_u	*buttons,
	ಪೂರ್ಣಾಂಕ	 dfltbutton,
	char_u	*textfield,
	ಪೂರ್ಣಾಂಕ ex_cmd)
{
    VimDialog *dialog = ಹೊಸ VimDialog(type, (ಅಕ್ಷರ*)title, (ಅಕ್ಷರ*)ಸಂದೇಶ_2,
	    (ಅಕ್ಷರ*)buttons, dfltbutton, (ಅಕ್ಷರ*)textfield, ex_cmd);
    ತಾರ್ಕಿಕ ret = dialog->Go();
    ಅಳಿಸು dialog;
	ಹಿಂತಿರುಗಿ ret;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_ಗಮ್ಯ_RGB(pixel);

    ಹಿಂತಿರುಗಿ ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    ಶೂನ್ಯ
gui_mch_setmouse(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    TRACE();
    // TODO
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
    ಶೂನ್ಯ
im_ಗಣ_position(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2())
    {
	gui.vim_ಪಠ್ಯ_Area->DrawIMString();
	gui.vim_ಕಿಟಕಿ->Unlock();
    }
    ಹಿಂತಿರುಗಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
gui_mch_show_toolbar(ಪೂರ್ಣಾಂಕ showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_ಎತ್ತರ = (toolbar && showit) ? toolbar->Toolbar_ಎತ್ತರ() : 0.;
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_toolbar_pos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    ಆದರೆ (toolbar != ಖಾಲಿ) {
	ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	    toolbar->Move_ಗಮ್ಯ(x, y);
	    toolbar->Resize_ಗಮ್ಯ(w - 1, h - 1);
	    gui.vim_ಕಿಟಕಿ->Unlock();
	}
    }
}

#ಆದರೆ defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    ಶೂನ್ಯ
gui_mch_show_tabline(ಪೂರ್ಣಾಂಕ showit)
{
    Vim_Tab_ಸಾಲು *tab_ಸಾಲು = gui.vimForm->Tab_ಸಾಲು();

    ಆದರೆ (tab_ಸಾಲು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!showit != !gui.vimForm->Is_Showing_Tab_ಸಾಲು()) {
	gui.vimForm->ಗಣ_Showing_Tab_ಸಾಲು(showit != 0);
	gui.tabline_ಎತ್ತರ = gui.vimForm->Tabline_ಎತ್ತರ();
    }
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_tabline_pos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h)
{
    Vim_Tab_ಸಾಲು *tab_ಸಾಲು = gui.vimForm->Tab_ಸಾಲು();
    ಆದರೆ (tab_ಸಾಲು != ಖಾಲಿ) {
	ಆದರೆ (gui.vim_ಕಿಟಕಿ->ಬೀಗ_2()) {
	    tab_ಸಾಲು->Move_ಗಮ್ಯ(x, y);
	    tab_ಸಾಲು->Resize_ಗಮ್ಯ(w - 1, h - 1);
	    gui.vim_ಕಿಟಕಿ->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_showing_tabline()
{
    Vim_Tab_ಸಾಲು *tab_ಸಾಲು = gui.vimForm->Tab_ಸಾಲು();
    ಹಿಂತಿರುಗಿ tab_ಸಾಲು != ಖಾಲಿ && gui.vimForm->Is_Showing_Tab_ಸಾಲು();
}

/*
 * Update the labels of the tabline.
 */
    ಶೂನ್ಯ
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಂಕ	    nr = 0;
    ಪೂರ್ಣಾಂಕ	    curtabidx = 0;

    Vim_Tab_ಸಾಲು *tab_ಸಾಲು = gui.vimForm->Tab_ಸಾಲು();

    ಆದರೆ (tab_ಸಾಲು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    gui.vim_ಕಿಟಕಿ->ಬೀಗ_2();

    // Add a label for each tab page.  They all contain the same text area.
    ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp != ಖಾಲಿ; tp = tp->tp_ಮುಂದಿನ, ++nr) {
	ಆದರೆ (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_ಸಾಲು->TabAt(nr);

	ಆದರೆ (tab == ಖಾಲಿ) {
	    tab = ಹೊಸ Vim_Tab_ಸಾಲು::VimTab();
	    tab_ಸಾಲು->ಕೂಡಿಸು_Tab(ಖಾಲಿ, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->ಗಣ_Label((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)NameBuff);
	tab_ಸಾಲು->Invalidate();
    }

    // Remove any old labels.
    ಆಗ (nr < tab_ಸಾಲು->ಎಣಿಕೆ_Tabs())
	tab_ಸಾಲು->RemoveTab(nr);

    ಆದರೆ (tab_ಸಾಲು->Selection() != curtabidx)
	tab_ಸಾಲು->Select(curtabidx);

    gui.vim_ಕಿಟಕಿ->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_curtab(ಪೂರ್ಣಾಂಕ nr)
{
    Vim_Tab_ಸಾಲು *tab_ಸಾಲು = gui.vimForm->Tab_ಸಾಲು();
    ಆದರೆ (tab_ಸಾಲು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    gui.vim_ಕಿಟಕಿ->ಬೀಗ_2();

    ಆದರೆ (tab_ಸಾಲು->Selection() != nr -1)
	tab_ಸಾಲು->Select(nr -1);

    gui.vim_ಕಿಟಕಿ->Unlock();
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_TABLINE
