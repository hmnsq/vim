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
ವರ್ಗ VimTextAreaView;
ವರ್ಗ VimWindow;
ವರ್ಗ VimToolbar;
ವರ್ಗ Vim_Tab_ಸಾಲು;

ಬಾಹ್ಯ ಕೀಲಿ_map *ಕೀಲಿ_Map;
ಬಾಹ್ಯ ವರ್ಣ *ಕೀಲಿ_Map_Chars;

ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv);

#ifndef B_MAX_PORT_ಎಣಿಕೆ
#define B_MAX_PORT_ಎಣಿಕೆ    255
#endif

// VimApp seems comparable to the X "vimShell"
ವರ್ಗ VimApp: ಸಾರ್ವಜನಿಕ BApplication
{
    ಪ್ರಕಾರನಿರ್ದೇಶ BApplication Inherited;
    ಸಾರ್ವಜನಿಕ:
    VimApp(ಧ್ರುವ ವರ್ಣ *appsig);
    ~VimApp();

    // callbacks:
#ಯದಿ 0
    ಆಭಾಸ ಶೂನ್ಯ DispatchMessage(BMessage *m, BHandler *h)
    {
	m->PrintToStream();
	Inherited::DispatchMessage(m, h);
    }
#endif
    ಆಭಾಸ ಶೂನ್ಯ ReadyToRun();
    ಆಭಾಸ ಶೂನ್ಯ ArgvReceived(int32 argc, ವರ್ಣ **argv);
    ಆಭಾಸ ಶೂನ್ಯ RefsReceived(BMessage *m);
    ಆಭಾಸ ತಾರ್ಕಿಕ QuitRequested();
    ಆಭಾಸ ಶೂನ್ಯ MessageReceived(BMessage *m);

    ಸ್ಥಿರ ಶೂನ್ಯ SendRefs(BMessage *m, ತಾರ್ಕಿಕ changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	fBrowsedPath;
    ಗುಪ್ತ:
};

ವರ್ಗ VimWindow: ಸಾರ್ವಜನಿಕ BWindow
{
    ಪ್ರಕಾರನಿರ್ದೇಶ BWindow Inherited;
    ಸಾರ್ವಜನಿಕ:
    VimWindow();
    ~VimWindow();

    ಆಭಾಸ ಶೂನ್ಯ DispatchMessage(BMessage *m, BHandler *h);
    ಆಭಾಸ ಶೂನ್ಯ WindowActivated(ತಾರ್ಕಿಕ active);
    ಆಭಾಸ ತಾರ್ಕಿಕ QuitRequested();

    VimFormView	    *formView;

    ಗುಪ್ತ:
    ಶೂನ್ಯ init();

    ತಾರ್ಕಿಕ is_fullscreen = ಅಸತ್ಯ;
    BRect saved_frame;
    window_look saved_look;
};

ವರ್ಗ VimFormView: ಸಾರ್ವಜನಿಕ BView
{
    ಪ್ರಕಾರನಿರ್ದೇಶ BView Inherited;
    ಸಾರ್ವಜನಿಕ:
    VimFormView(BRect frame);
    ~VimFormView();

    // callbacks:
    ಆಭಾಸ ಶೂನ್ಯ AllAttached();
    ಆಭಾಸ ಶೂನ್ಯ FrameResized(ಭಿನ್ನ new_width, ಭಿನ್ನ new_height);

#define MENUBAR_MARGIN	1
    ಭಿನ್ನ MenuHeight() ಧ್ರುವ
    { ಪ್ರತ್ಯಾಗಮ menuBar ? menuBar->Frame().Height() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() ಧ್ರುವ
    { ಪ್ರತ್ಯಾಗಮ menuBar; }

    ಗುಪ್ತ:
    ಶೂನ್ಯ init(BRect);

    BMenuBar	    *menuBar;
    VimTextAreaView *textArea;

#ifdef FEAT_TOOLBAR
    ಸಾರ್ವಜನಿಕ:
    ಭಿನ್ನ ToolbarHeight() ಧ್ರುವ;
    VimToolbar *ToolBar() ಧ್ರುವ
    { ಪ್ರತ್ಯಾಗಮ toolBar; }
    ಗುಪ್ತ:
    VimToolbar	    *toolBar;
#endif

#ifdef FEAT_GUI_TABLINE
    ಸಾರ್ವಜನಿಕ:
    Vim_Tab_ಸಾಲು *Tab_ಸಾಲು() ಧ್ರುವ	{ ಪ್ರತ್ಯಾಗಮ tab_ಸಾಲು; }
    ತಾರ್ಕಿಕ Is_Showing_Tab_ಸಾಲು() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ showing_Tab_ಸಾಲು; }
    ಶೂನ್ಯ Set_Showing_Tab_ಸಾಲು(ತಾರ್ಕಿಕ showing) { showing_Tab_ಸಾಲು = showing;	}
    ಭಿನ್ನ TablineHeight() ಧ್ರುವ;
    ಗುಪ್ತ:
    Vim_Tab_ಸಾಲು	*tab_ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ	showing_Tab_ಸಾಲು;
#endif
};

ವರ್ಗ VimTextAreaView: ಸಾರ್ವಜನಿಕ BView
{
    ಪ್ರಕಾರನಿರ್ದೇಶ BView Inherited;
    ಸಾರ್ವಜನಿಕ:
    VimTextAreaView(BRect frame);
    ~VimTextAreaView();

    // callbacks:
    ಆಭಾಸ ಶೂನ್ಯ Draw(BRect updateRect);
    ಆಭಾಸ ಶೂನ್ಯ ಕೀಲಿ_Down(ಧ್ರುವ ವರ್ಣ *bytes, int32 ಅಂಕೆ_Bytes);
    ಆಭಾಸ ಶೂನ್ಯ MouseDown(BPoint point);
    ಆಭಾಸ ಶೂನ್ಯ MouseUp(BPoint point);
    ಆಭಾಸ ಶೂನ್ಯ MouseMoved(BPoint point, uint32 transit, ಧ್ರುವ BMessage *message);
    ಆಭಾಸ ಶೂನ್ಯ MessageReceived(BMessage *m);

    // own functions:
    ಪೂರ್ಣಾಙ್ಕ mchInitFont(char_u *name);
    ಶೂನ್ಯ mchDrawString(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, char_u *s, ಪೂರ್ಣಾಙ್ಕ len, ಪೂರ್ಣಾಙ್ಕ flags);
    ಶೂನ್ಯ mchClearBlock(ಪೂರ್ಣಾಙ್ಕ row1, ಪೂರ್ಣಾಙ್ಕ col1, ಪೂರ್ಣಾಙ್ಕ row2, ಪೂರ್ಣಾಙ್ಕ col2);
    ಶೂನ್ಯ mchClearAll();
    ಶೂನ್ಯ mch_Delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ಸಾಲುಗಳು);
    ಶೂನ್ಯ mch_ಸೇರಿಸು_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ಸಾಲುಗಳು);

    ಸ್ಥಿರ ಶೂನ್ಯ guiSendMouseEvent(ಪೂರ್ಣಾಙ್ಕ button, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ repeated_click, int_u modifiers);
    ಸ್ಥಿರ ಶೂನ್ಯ guiMouseMoved(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y);
    ಸ್ಥಿರ ಶೂನ್ಯ guiBlankMouse(ತಾರ್ಕಿಕ should_hide);
    ಸ್ಥಿರ int_u mouseModifiersToVim(int32 beModifiers);

    int32 mouse_Drag_Event_ಎಣಿಕೆ;

#ifdef FEAT_MBYTE_IME
    ಶೂನ್ಯ DrawIMString(ಶೂನ್ಯ);
#endif

    ಗುಪ್ತ:
    ಶೂನ್ಯ init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#ifdef FEAT_MBYTE_IME
    ಸಂರಚನಾ {
	BMessenger* messenger;
	BMessage* message;
	BPoint location;
	ಪೂರ್ಣಾಙ್ಕ row;
	ಪೂರ್ಣಾಙ್ಕ col;
	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ;
    } IMData;
#endif
};

ವರ್ಗ VimScrollBar: ಸಾರ್ವಜನಿಕ BScrollBar
{
    ಪ್ರಕಾರನಿರ್ದೇಶ BScrollBar Inherited;
    ಸಾರ್ವಜನಿಕ:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    ಆಭಾಸ ಶೂನ್ಯ ಮೌಲ್ಯ_Changed(ಭಿನ್ನ new_ಮೌಲ್ಯ);
    ಆಭಾಸ ಶೂನ್ಯ MouseUp(BPoint where);
    ಶೂನ್ಯ Set_ಮೌಲ್ಯ(ಭಿನ್ನ newval);
    scrollbar_T *getGsb()
    { ಪ್ರತ್ಯಾಗಮ gsb; }

    int32	scroll_Event_ಎಣಿಕೆ;

    ಗುಪ್ತ:
    scrollbar_T *gsb;
    ಭಿನ್ನ   ignore_ಮೌಲ್ಯ;
};


#ifdef FEAT_TOOLBAR

ವರ್ಗ VimToolbar : ಸಾರ್ವಜನಿಕ BBox
{
    ಸ್ಥಿರ BBitmap *normalButtonsBitmap;
    ಸ್ಥಿರ BBitmap *grayedButtonsBitmap;

    BBitmap *LoadVimBitmap(ಧ್ರುವ ವರ್ಣ* fileName);
    ತಾರ್ಕಿಕ GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, ತಾರ್ಕಿಕ pressed);
    ತಾರ್ಕಿಕ ModifyBitmapToGrayed(BBitmap *bitmap);

    BList fButtonsList;
    ಶೂನ್ಯ InvalidateLayout();

    ಸಾರ್ವಜನಿಕ:
    VimToolbar(BRect frame, ಧ್ರುವ ವರ್ಣ * name);
    ~VimToolbar();

    ತಾರ್ಕಿಕ PrepareButtonBitmaps();

    ತಾರ್ಕಿಕ AddButton(int32 index, vimmenu_T *menu);
    ತಾರ್ಕಿಕ RemoveButton(vimmenu_T *menu);
    ತಾರ್ಕಿಕ GrayButton(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ grey);

    ಭಿನ್ನ ToolbarHeight() ಧ್ರುವ;
    ಆಭಾಸ ಶೂನ್ಯ AttachedToWindow();
};

BBitmap *VimToolbar::normalButtonsBitmap  = ರಿಕ್ತ;
BBitmap *VimToolbar::grayedButtonsBitmap  = ರಿಕ್ತ;

ಧ್ರುವ ಭಿನ್ನ ToolbarMargin = 3.;
ಧ್ರುವ ಭಿನ್ನ ButtonMargin  = 3.;

#endif //FEAT_TOOLBAR

#ifdef FEAT_GUI_TABLINE

ವರ್ಗ Vim_Tab_ಸಾಲು : ಸಾರ್ವಜನಿಕ BTabView
{
    ಸಾರ್ವಜನಿಕ:
	ವರ್ಗ VimTab : ಸಾರ್ವಜನಿಕ BTab {
	    ಸಾರ್ವಜನಿಕ:
		VimTab() : BTab(ನೂತನ BView(BRect(), "-Empty-", 0, 0)) {}

	    ಆಭಾಸ ಶೂನ್ಯ Select(BView* owner);
	};

	Vim_Tab_ಸಾಲು(BRect r) : BTabView(r, "vimTabLine", B_WIDTH_FROM_LABEL,
	       B_FOLLOW_ಎಡ | B_FOLLOW_ತುದಿ | B_FOLLOW_ಬಲ, B_WILL_DRAW | B_FRAME_EVENTS) {}

    ಭಿನ್ನ TablineHeight() ಧ್ರುವ;
    ಆಭಾಸ ಶೂನ್ಯ MouseDown(BPoint point);
};

#endif //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
ವರ್ಗ VimFont: ಸಾರ್ವಜನಿಕ BFont
{
    ಪ್ರಕಾರನಿರ್ದೇಶ BFont Inherited;
    ಸಾರ್ವಜನಿಕ:
    VimFont();
    VimFont(ಧ್ರುವ VimFont *rhs);
    VimFont(ಧ್ರುವ BFont *rhs);
    VimFont(ಧ್ರುವ VimFont &rhs);
    ~VimFont();

    VimFont *next;
    ಪೂರ್ಣಾಙ್ಕ refcount;
    char_u *name;

    ಗುಪ್ತ:
    ಶೂನ್ಯ init();
};

#ಯದಿ defined(FEAT_GUI_DIALOG)

ವರ್ಗ VimDialog : ಸಾರ್ವಜನಿಕ BWindow
{
    ಪ್ರಕಾರನಿರ್ದೇಶ BWindow Inherited;

    BButton* _CreateButton(int32 which, ಧ್ರುವ ವರ್ಣ* label);

    ಸಾರ್ವಜನಿಕ:

    ವರ್ಗ View : ಸಾರ್ವಜನಿಕ BView {
	ಪ್ರಕಾರನಿರ್ದೇಶ BView Inherited;

	ಸಾರ್ವಜನಿಕ:
	View(BRect frame);
	~View();

	ಆಭಾಸ ಶೂನ್ಯ Draw(BRect updateRect);
	ಶೂನ್ಯ InitIcon(int32 type);

	ಗುಪ್ತ:
	BBitmap*    fIconBitmap;
    };

    VimDialog(ಪೂರ್ಣಾಙ್ಕ type, ಧ್ರುವ ವರ್ಣ *title, ಧ್ರುವ ವರ್ಣ *message,
	    ಧ್ರುವ ವರ್ಣ *buttons, ಪೂರ್ಣಾಙ್ಕ dfltbutton, ಧ್ರುವ ವರ್ಣ *textfield,
	    ಪೂರ್ಣಾಙ್ಕ ex_cmd);
    ~VimDialog();

    ಪೂರ್ಣಾಙ್ಕ Go();

    ಆಭಾಸ ಶೂನ್ಯ MessageReceived(BMessage *ಸಂದೇಶ);

    ಗುಪ್ತ:
    sem_id	    fDialogSem;
    ಪೂರ್ಣಾಙ್ಕ		    f_Dialog_ಮೌಲ್ಯ;
    BList	    fButtonsList;
    BTextView*	    fMessageView;
    BTextControl*   f_ಒಳಹರಿವು_Control;
    ಧ್ರುವ ವರ್ಣ*	    f_ಒಳಹರಿವು_ಮೌಲ್ಯ;
};

ವರ್ಗ VimSelectFontDialog : ಸಾರ್ವಜನಿಕ BWindow
{
    ಪ್ರಕಾರನಿರ್ದೇಶ BWindow Inherited;

    ಶೂನ್ಯ _CleanList(BListView* list);
    ಶೂನ್ಯ _UpdateFontStyles();
    ಶೂನ್ಯ Update_Size_ಒಳಹರಿವು_Preview();
    ಶೂನ್ಯ _UpdateFontPreview();
    ತಾರ್ಕಿಕ _UpdateFromListItem(BListView* list, ವರ್ಣ* text, ಪೂರ್ಣಾಙ್ಕ textSize);
    ಸಾರ್ವಜನಿಕ:

    VimSelectFontDialog(font_family* family, font_style* style, ಭಿನ್ನ* size);
    ~VimSelectFontDialog();

    ತಾರ್ಕಿಕ Go();

    ಆಭಾಸ ಶೂನ್ಯ MessageReceived(BMessage *ಸಂದೇಶ);

    ಗುಪ್ತ:
    status_t	    fStatus;
    sem_id	    fDialogSem;
    ತಾರ್ಕಿಕ	    f_Dialog_ಮೌಲ್ಯ;
    font_family*    fFamily;
    font_style*	    fStyle;
    ಭಿನ್ನ*	    fSize;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    ಭಿನ್ನ	    fFontSize;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_ಒಳಹರಿವು;
};

#endif // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

ಸಂರಚನಾ MainArgs {
    ಪೂರ್ಣಾಙ್ಕ	     argc;
    ವರ್ಣ    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#define	ಕೀಲಿ_ಸಂದೇಶ_BUFSIZ	7
#ಯದಿ ಕೀಲಿ_ಸಂದೇಶ_BUFSIZ < MAX_ಕೀಲಿ_CODE_LEN
#error Increase ಕೀಲಿ_ಸಂದೇಶ_BUFSIZ!
#endif

ಸಂರಚನಾ Vim_ಕೀಲಿ_ಸಂದೇಶ {
    char_u  length;
    char_u  chars[ಕೀಲಿ_ಸಂದೇಶ_BUFSIZ];  // contains Vim encoding
    ತಾರ್ಕಿಕ    csi_escape;
};

ಸಂರಚನಾ Vim_Resize_ಸಂದೇಶ {
    ಪೂರ್ಣಾಙ್ಕ	    width;
    ಪೂರ್ಣಾಙ್ಕ	    height;
};

ಸಂರಚನಾ Vim_Scroll_Bar_ಸಂದೇಶ {
    VimScrollBar *sb;
    long    ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಙ್ಕ	    stillDragging;
};

ಸಂರಚನಾ Vim_Menu_ಸಂದೇಶ {
    vimmenu_T	*guiMenu;
};

ಸಂರಚನಾ Vim_Mouse_ಸಂದೇಶ {
    ಪೂರ್ಣಾಙ್ಕ	    button;
    ಪೂರ್ಣಾಙ್ಕ	    x;
    ಪೂರ್ಣಾಙ್ಕ	    y;
    ಪೂರ್ಣಾಙ್ಕ	    repeated_click;
    int_u   modifiers;
};

ಸಂರಚನಾ Vim_Mouse_Moved_ಸಂದೇಶ {
    ಪೂರ್ಣಾಙ್ಕ	    x;
    ಪೂರ್ಣಾಙ್ಕ	    y;
};

ಸಂರಚನಾ Vim_Focus_ಸಂದೇಶ {
    ತಾರ್ಕಿಕ    active;
};

ಸಂರಚನಾ Vim_Refs_ಸಂದೇಶ {
    BMessage   *message;
    ತಾರ್ಕಿಕ    changedir;
};

ಸಂರಚನಾ Vim_Tabline_ಸಂದೇಶ {
    ಪೂರ್ಣಾಙ್ಕ	    index;
};

ಸಂರಚನಾ Vim_Tabline_Menu_ಸಂದೇಶ {
    ಪೂರ್ಣಾಙ್ಕ	    index;
    ಪೂರ್ಣಾಙ್ಕ	    event;
};

ಸಂರಚನಾ Vim_ಸಂದೇಶ {
    ಗಣನಾ Vim_ಸಂದೇಶ_Type {
	ಕೀಲಿ_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    ಸಙ್ಘ {
	ಸಂರಚನಾ Vim_ಕೀಲಿ_ಸಂದೇಶ    ಕೀಲಿ_2;
	ಸಂರಚನಾ Vim_Resize_ಸಂದೇಶ NewSize;
	ಸಂರಚನಾ Vim_Scroll_Bar_ಸಂದೇಶ	Scroll;
	ಸಂರಚನಾ Vim_Menu_ಸಂದೇಶ   Menu;
	ಸಂರಚನಾ Vim_Mouse_ಸಂದೇಶ  Mouse;
	ಸಂರಚನಾ Vim_Mouse_Moved_ಸಂದೇಶ	MouseMoved;
	ಸಂರಚನಾ Vim_Focus_ಸಂದೇಶ  Focus;
	ಸಂರಚನಾ Vim_Refs_ಸಂದೇಶ   Refs;
	ಸಂರಚನಾ Vim_Tabline_ಸಂದೇಶ	Tabline;
	ಸಂರಚನಾ Vim_Tabline_Menu_ಸಂದೇಶ    TablineMenu;
    } u;
};

#define RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#define GUI_TO_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

ಸ್ಥಿರ ಸಂರಚನಾ specialkey
{
    uint16  Be_ಕೀಲಿಗಳು;
#define ಕೀಲಿ_3(a,b)    ((a)<<8|(b))
#define K(a)	    ಕೀಲಿ_3(0,a)		// for ASCII codes
#define F(b)	    ಕೀಲಿ_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_ಕೀಲಿಗಳು[] =
{
    {K(B_UP_ARROW),	'k', 'u'},
    {K(B_DOWN_ARROW),	    'k', 'd'},
    {K(B_ಎಡ_ARROW),	    'k', 'l'},
    {K(B_ಬಲ_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_ಸೇರಿಸು),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_END),		'@', '7'},
    {K(B_PAGE_UP),	'k', 'P'},	// XK_Prior
    {K(B_PAGE_DOWN),	    'k', 'N'},	    // XK_Next,

#define FIRST_FUNCTION_ಕೀಲಿ  11
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

#ಯದಿ 0
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
#endif

    // End of list marker:
    {0,		    0, 0}
};

#define ಅಂಕೆ_SPECIAL_ಕೀಲಿಗಳು    ARRAY_LENGTH(special_ಕೀಲಿಗಳು)

// ---------------- VimApp ----------------

    ಸ್ಥಿರ ಶೂನ್ಯ
docd(BPath &path)
{
    mch_chdir((ವರ್ಣ *)path.Path());
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
    int32 ಎಣಿಕೆ;

    m->PrintToStream();
    ವಿಕಲ್ಪ (m->what) {
	ಪಕ್ಷ B_REFS_RECEIVED:
	ಪಕ್ಷ B_SIMPLE_DATA:
	    m->GetInfo("refs", &type, &ಎಣಿಕೆ);
	    ಯದಿ (type != B_REF_TYPE)
		ಗಚ್ಛ bad;
	    ಭಙ್ಗ;
	ಪಕ್ಷ B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &ಎಣಿಕೆ);
	    ಯದಿ (type != B_STRING_TYPE)
		ಗಚ್ಛ bad;
	    ಯದಿ (changedir) {
		ವರ್ಣ *dirname;
		ಯದಿ (m->FindString("cwd", (ಧ್ರುವ ವರ್ಣ **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    delete m;
	    ಪ್ರತ್ಯಾಗಮ;
    }

#ifdef FEAT_VISUAL
    reset_VIsual();
#endif

    char_u  **fnames;
    fnames = (char_u **) alloc(ಎಣಿಕೆ * ಪರಿಮಾಣ(char_u *));
    ಪೂರ್ಣಾಙ್ಕ fname_index = 0;

    ವಿಕಲ್ಪ (m->what) {
	ಪಕ್ಷ B_REFS_RECEIVED:
	ಪಕ್ಷ B_SIMPLE_DATA:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಎಣಿಕೆ; ++i)
	    {
		entry_ref ref;
		ಯದಿ (m->FindRef("refs", i, &ref) == B_OK) {
		    BEntry entry(&ref, ಅಸತ್ಯ);
		    BPath path;
		    entry.GetPath(&path);

		    // Change to parent directory?
		    ಯದಿ (changedir) {
			BPath parentpath;
			path.GetParent(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    ಯದಿ (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			ಯದಿ (!changedir)
			    docd(path);
		    } ಅನ್ಯಥಾ {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)path.Path(), IObuff);
			ಯದಿ (fname == ರಿಕ್ತ)
			    fname = (char_u *)path.Path();
			fnames[fname_index++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = ಅಸತ್ಯ;
		}
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < ಎಣಿಕೆ; ++i)
	    {
		ವರ್ಣ *fname;

		ಯದಿ (m->FindString("argv", i, (ಧ್ರುವ ವರ್ಣ **) &fname) == B_OK) {
		    fnames[fname_index++] = vim_strsave((char_u *)fname);
		}
	    }
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    // fprintf(stderr, "case default\n");
	    ಭಙ್ಗ;
    }

    delete m;

    // Handle the drop, :edit to get to the file
    ಯದಿ (fname_index > 0) {
	handle_drop(fname_index, fnames, FALSE, drop_callback, ರಿಕ್ತ);

	setcursor();
	out_flush();
    } ಅನ್ಯಥಾ {
	vim_free(fnames);
    }
}

VimApp::VimApp(ಧ್ರುವ ವರ್ಣ *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(ರಿಕ್ತ)
{
}

VimApp::~VimApp()
{
}

    ಶೂನ್ಯ
VimApp::ReadyToRun()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    ಶೂನ್ಯ
VimApp::ArgvReceived(int32 arg_argc, ವರ್ಣ **arg_argv)
{
    ಯದಿ (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	ಯದಿ (gui.vimWindow)
	    gui.vimWindow->Minimize(ಅಸತ್ಯ);
	BMessage *m = CurrentMessage();
	DetachCurrentMessage();
	SendRefs(m, ಸತ್ಯ);
    }
}

    ಶೂನ್ಯ
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    ಪೂರ್ಣಾಙ್ಕ limit = 15;
    ಯಾವತ್ (--limit >= 0 && (curbuf == ರಿಕ್ತ || curbuf->b_start_ffc == 0))
	snooze(100000);    //  0.1 s
    ಯದಿ (gui.vimWindow)
	gui.vimWindow->Minimize(ಅಸತ್ಯ);
    DetachCurrentMessage();
    SendRefs(m, ಸತ್ಯ);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    ಶೂನ್ಯ
VimApp::SendRefs(BMessage *m, ತಾರ್ಕಿಕ changedir)
{
    Vim_Refs_ಸಂದೇಶ rm;
    rm.message = m;
    rm.changedir = changedir;

    write_port(gui.vdcmp, Vim_ಸಂದೇಶ::Refs, &rm, ಪರಿಮಾಣ(rm));
    //	calls ::RefsReceived
}

    ಶೂನ್ಯ
VimApp::MessageReceived(BMessage *m)
{
    ವಿಕಲ್ಪ (m->what) {
	ಪಕ್ಷ 'save':
	    {
		entry_ref refDirectory;
		m->FindRef("directory", &refDirectory);
		fBrowsedPath.SetTo(&refDirectory);
		BString strName;
		m->FindString("name", &strName);
		fBrowsedPath.Append(strName.String());
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ 'open':
	    {
		entry_ref ref;
		m->FindRef("refs", &ref);
		fBrowsedPath.SetTo(&ref);
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ B_CANCEL:
	    {
		BFilePanel *panel;
		m->FindPointer("source", (ಶೂನ್ಯ**)&panel);
		ಯದಿ (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    Inherited::MessageReceived(m);
	    ಭಙ್ಗ;
    }
}

    ತಾರ್ಕಿಕ
VimApp::QuitRequested()
{
    (ಶೂನ್ಯ)Inherited::QuitRequested();
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

// ---------------- VimWindow ----------------

VimWindow::VimWindow():
    BWindow(BRect(40, 40, 150, 150),
	    "Vim",
	    B_TITLED_WINDOW,
	    0,
	    B_CURRENT_WORKSPACE)

{
    init();
}

VimWindow::~VimWindow()
{
    ಯದಿ (formView) {
	RemoveChild(formView);
	delete formView;
    }
    gui.vimWindow = ರಿಕ್ತ;
}

    ಶೂನ್ಯ
VimWindow::init()
{
    // Attach the VimFormView
    formView = ನೂತನ VimFormView(Bounds());
    ಯದಿ (formView != ರಿಕ್ತ) {
	AddChild(formView);
    }
}

#ಯದಿ 0  //  disabled in zeta patch
    ಶೂನ್ಯ
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    ವಿಕಲ್ಪ (m->what) {
	ಪಕ್ಷ B_MOUSE_UP:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		ಯದಿ (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->FindPoint("where", &where);
		    v->MouseUp(where);
		} ಅನ್ಯಥಾ {
		    Inherited::DispatchMessage(m, h);
		}
	    }
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    Inherited::DispatchMessage(m, h);
    }
}
#endif

    ಶೂನ್ಯ
VimWindow::WindowActivated(ತಾರ್ಕಿಕ active)
{
    Inherited::WindowActivated(active);
    // the textArea gets the keyboard action
    ಯದಿ (active && gui.vimTextArea)
	gui.vimTextArea->MakeFocus(ಸತ್ಯ);

    ಸಂರಚನಾ Vim_Focus_ಸಂದೇಶ fm;
    fm.active = active;

    write_port(gui.vdcmp, Vim_ಸಂದೇಶ::Focus, &fm, ಪರಿಮಾಣ(fm));
}

    ತಾರ್ಕಿಕ
VimWindow::QuitRequested()
{
    ಸಂರಚನಾ Vim_ಕೀಲಿ_ಸಂದೇಶ km;
    km.length = 5;
    memcpy((ವರ್ಣ *)km.chars, "\033:qa\r", km.length);
    km.csi_escape = ಅಸತ್ಯ;
    write_port(gui.vdcmp, Vim_ಸಂದೇಶ::ಕೀಲಿ_2, &km, ಪರಿಮಾಣ(km));
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}
    ಶೂನ್ಯ
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    ತಾರ್ಕಿಕ should_propagate = ಸತ್ಯ;

    ವಿಕಲ್ಪ (m->what)
    {
	ಪಕ್ಷ B_ಕೀಲಿ_DOWN:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->FindInt32("raw_char", &scancode);
		m->FindInt32("modifiers", &beModifiers);

		ಯದಿ (scancode == B_ENTER && (beModifiers & B_ಎಡ_COMMAND_ಕೀಲಿ))
		    {
			should_propagate = ಅಸತ್ಯ;
			ಯದಿ (ಏತತ್->is_fullscreen)
			    {
				ಏತತ್->is_fullscreen = ಅಸತ್ಯ;
				ResizeTo(ಏತತ್->saved_frame.Width(), ಏತತ್->saved_frame.Height());
				MoveTo(ಏತತ್->saved_frame.ಎಡ, ಏತತ್->saved_frame.ತುದಿ);
				SetLook(ಏತತ್->saved_look);
				SetFlags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } ಅನ್ಯಥಾ {
				ಏತತ್->saved_frame = Frame();
				ಏತತ್->saved_look = Look();
				ಏತತ್->is_fullscreen = ಸತ್ಯ;
				BScreen s(ಏತತ್);
				SetLook(B_NO_BORDER_WINDOW_LOOK);
				ResizeTo(s.Frame().Width() + 1, s.Frame().Height() + 1);
				MoveTo(s.Frame().ಎಡ, s.Frame().ತುದಿ);
				SetFlags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    ಯದಿ (should_propagate)
	Inherited::DispatchMessage(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect frame):
    BView(frame, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_FRAME_EVENTS),
    menuBar(ರಿಕ್ತ),
#ifdef FEAT_TOOLBAR
    toolBar(ರಿಕ್ತ),
#endif
#ifdef FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_ಸಾಲು(ರಿಕ್ತ),
#endif
    textArea(ರಿಕ್ತ)
{
    init(frame);
}

VimFormView::~VimFormView()
{
    ಯದಿ (menuBar) {
	RemoveChild(menuBar);
#ifdef never
	//  deleting the menuBar leads to SEGV on exit
	//  richard@whitequeen.com Jul 99
	delete menuBar;
#endif
    }

#ifdef FEAT_TOOLBAR
    delete toolBar;
#endif

#ifdef FEAT_GUI_TABLINE
    delete tab_ಸಾಲು;
#endif

    ಯದಿ (textArea) {
	RemoveChild(textArea);
	delete textArea;
    }
    gui.vimForm = ರಿಕ್ತ;
}

    ಶೂನ್ಯ
VimFormView::init(BRect frame)
{
    menuBar = ನೂತನ BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    AddChild(menuBar);

#ifdef FEAT_TOOLBAR
    toolBar = ನೂತನ VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    AddChild(toolBar);
#endif

#ifdef FEAT_GUI_TABLINE
    tab_ಸಾಲು = ನೂತನ Vim_Tab_ಸಾಲು(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    AddChild(tab_ಸಾಲು);
#endif

    BRect remaining = frame;
    textArea = ನೂತನ VimTextAreaView(remaining);
    AddChild(textArea);
    // The textArea will be resized later when menus are added

    gui.vimForm = ಏತತ್;
}

#ifdef FEAT_TOOLBAR
    ಭಿನ್ನ
VimFormView::ToolbarHeight() ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ toolBar ? toolBar->ToolbarHeight() : 0.;
}
#endif

#ifdef FEAT_GUI_TABLINE
    ಭಿನ್ನ
VimFormView::TablineHeight() ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ (tab_ಸಾಲು && Is_Showing_Tab_ಸಾಲು()) ? tab_ಸಾಲು->TablineHeight() : 0.;
}
#endif

    ಶೂನ್ಯ
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    ಯದಿ (menuBar && textArea) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect remaining = Bounds();

#ifdef FEAT_MENU
	remaining.ತುದಿ += MenuHeight();
	menuBar->ResizeTo(remaining.ಬಲ, remaining.ತುದಿ);
	gui.menu_height = (ಪೂರ್ಣಾಙ್ಕ) MenuHeight();
#endif

#ifdef FEAT_TOOLBAR
	toolBar->MoveTo(remaining.ಎಡ, remaining.ತುದಿ);
	toolBar->ResizeTo(remaining.ಬಲ, ToolbarHeight());
	remaining.ತುದಿ += ToolbarHeight();
	gui.toolbar_height = ToolbarHeight();
#endif

#ifdef FEAT_GUI_TABLINE
	tab_ಸಾಲು->MoveTo(remaining.ಎಡ, remaining.ತುದಿ);
	tab_ಸಾಲು->ResizeTo(remaining.ಬಲ + 1, TablineHeight());
	remaining.ತುದಿ += TablineHeight();
	gui.tabline_height = TablineHeight();
#endif

	textArea->ResizeTo(remaining.Width(), remaining.Height());
	textArea->MoveTo(remaining.ಎಡ, remaining.ತುದಿ);
    }


    Inherited::AllAttached();
}

    ಶೂನ್ಯ
VimFormView::FrameResized(ಭಿನ್ನ new_width, ಭಿನ್ನ new_height)
{
    ಸಂರಚನಾ Vim_Resize_ಸಂದೇಶ sm;
    ಪೂರ್ಣಾಙ್ಕ adjust_h, adjust_w;

    new_width += 1;	//  adjust from width to number of pixels occupied
    new_height += 1;

    sm.width = (ಪೂರ್ಣಾಙ್ಕ) new_width;
    sm.height = (ಪೂರ್ಣಾಙ್ಕ) new_height;
    adjust_w = ((ಪೂರ್ಣಾಙ್ಕ)new_width - gui_get_base_width()) % gui.char_width;
    adjust_h = ((ಪೂರ್ಣಾಙ್ಕ)new_height - gui_get_base_height()) % gui.char_height;

    ಯದಿ (adjust_w > 0 || adjust_h > 0) {
	sm.width  -= adjust_w;
	sm.height -= adjust_h;
    }

    write_port(gui.vdcmp, Vim_ಸಂದೇಶ::Resize, &sm, ಪರಿಮಾಣ(sm));
    //	calls gui_resize_shell(new_width, new_height);

    ಪ್ರತ್ಯಾಗಮ;

    /*
     * The area below the vertical scrollbar is erased to the colour
     * set with SetViewColor() automatically, because we had set
     * B_WILL_DRAW. Resizing the window tight around the vertical
     * scroll bar also helps to avoid debris.
     */
}

// ---------------- VimTextAreaView ----------------

VimTextAreaView::VimTextAreaView(BRect frame):
    BView(frame, "VimTextAreaView", B_FOLLOW_ALL_SIDES,
#ifdef FEAT_MBYTE_IME
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_ಒಳಹರಿವು_METHOD_AWARE
#ಅನ್ಯಥಾ
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE
#endif
	),
    mouse_Drag_Event_ಎಣಿಕೆ(0)
{
#ifdef FEAT_MBYTE_IME
    IMData.messenger = ರಿಕ್ತ;
    IMData.message = ರಿಕ್ತ;
#endif
    init(frame);
}

VimTextAreaView::~VimTextAreaView()
{
    gui.vimTextArea = ರಿಕ್ತ;
}

    ಶೂನ್ಯ
VimTextAreaView::init(BRect frame)
{
    // set up global var for fast access
    gui.vimTextArea = ಏತತ್;

    /*
     * Tell the app server not to erase the view: we will
     * fill it in completely by ourselves.
     * (Does this really work? Even if not, it won't harm either.)
     */
    SetViewColor(B_TRANSPARENT_32_BIT);
#define PEN_WIDTH   1
    SetPenSize(PEN_WIDTH);
#define W_WIDTH(curwin)   0
}

    ಶೂನ್ಯ
VimTextAreaView::Draw(BRect updateRect)
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
    rgb_color rgb = GUI_TO_RGB(gui.back_pixel);
    Set_ಕೆಳಗಿನ_Color(rgb);
    FillRect(updateRect, B_SOLID_ಕೆಳಗಿನ);
    gui_redraw((ಪೂರ್ಣಾಙ್ಕ) updateRect.ಎಡ, (ಪೂರ್ಣಾಙ್ಕ) updateRect.ತುದಿ,
	    (ಪೂರ್ಣಾಙ್ಕ) (updateRect.Width() + PEN_WIDTH), (ಪೂರ್ಣಾಙ್ಕ) (updateRect.Height() + PEN_WIDTH));

    // Clear the border areas if needed
    Set_ಕೆಳಗಿನ_Color(rgb);

    ಯದಿ (updateRect.ಎಡ < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.ಎಡ, updateRect.ತುದಿ,
		    FILL_X(0)-PEN_WIDTH, updateRect.bottom), B_SOLID_ಕೆಳಗಿನ);
    ಯದಿ (updateRect.ತುದಿ < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.ಎಡ, updateRect.ತುದಿ,
		    updateRect.ಬಲ, FILL_Y(0)-PEN_WIDTH), B_SOLID_ಕೆಳಗಿನ);
    ಯದಿ (updateRect.ಬಲ >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((ಪೂರ್ಣಾಙ್ಕ)Columns), updateRect.ತುದಿ,
		    updateRect.ಬಲ, updateRect.bottom), B_SOLID_ಕೆಳಗಿನ);
    ಯದಿ (updateRect.bottom >= FILL_Y(Rows))   //  bottom border
	FillRect(BRect(updateRect.ಎಡ, FILL_Y((ಪೂರ್ಣಾಙ್ಕ)Rows),
		    updateRect.ಬಲ, updateRect.bottom), B_SOLID_ಕೆಳಗಿನ);

#ifdef FEAT_MBYTE_IME
    DrawIMString();
#endif
}

    ಶೂನ್ಯ
VimTextAreaView::ಕೀಲಿ_Down(ಧ್ರುವ ವರ್ಣ *bytes, int32 ಅಂಕೆ_Bytes)
{
    ಸಂರಚನಾ Vim_ಕೀಲಿ_ಸಂದೇಶ km;
    char_u *dest = km.chars;

    ತಾರ್ಕಿಕ canHaveVimModifiers = ಅಸತ್ಯ;

    BMessage *ಸಂದೇಶ = Window()->CurrentMessage();
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
    ಯದಿ (ಅಂಕೆ_Bytes > 1) {
	// This cannot be a special key
	ಯದಿ (ಅಂಕೆ_Bytes > ಕೀಲಿ_ಸಂದೇಶ_BUFSIZ)
	    ಅಂಕೆ_Bytes = ಕೀಲಿ_ಸಂದೇಶ_BUFSIZ;	    //	should never happen... ???
	km.length = ಅಂಕೆ_Bytes;
	memcpy((ವರ್ಣ *)dest, bytes, ಅಂಕೆ_Bytes);
	km.csi_escape = ಸತ್ಯ;
    } ಅನ್ಯಥಾ {
	int32 scancode = 0;
	ಸಂದೇಶ->FindInt32("key", &scancode);

	int32 beModifiers = 0;
	ಸಂದೇಶ->FindInt32("modifiers", &beModifiers);

	char_u string[3];
	ಪೂರ್ಣಾಙ್ಕ len = 0;
	km.length = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	ಪ್ರತಿಜ್ಞಾ(B_BACKSPACE <= 0x20);
	ಪ್ರತಿಜ್ಞಾ(B_DELETE == 0x7F);
	ಯದಿ (((char_u)bytes[0] <= 0x20 || (char_u)bytes[0] == 0x7F) &&
		ಅಂಕೆ_Bytes == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    ಯದಿ (beModifiers & B_CONTROL_ಕೀಲಿ) {
		ಪೂರ್ಣಾಙ್ಕ index = ಕೀಲಿ_Map->normal_map[scancode];
		ಪೂರ್ಣಾಙ್ಕ new_ಅಂಕೆ_Bytes = ಕೀಲಿ_Map_Chars[index];
		char_u *newBytes = (char_u *)&ಕೀಲಿ_Map_Chars[index + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		ಯದಿ (new_ಅಂಕೆ_Bytes != 1 || (*newBytes > 0x20 &&
			    *newBytes != 0x7F )) {
		    ಗಚ್ಛ notspecial;
		}
		bytes = (ವರ್ಣ *)newBytes;
	    }
	    canHaveVimModifiers = ಸತ್ಯ;

	    uint16 beoskey;
	    ಪೂರ್ಣಾಙ್ಕ first, last;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    ಯದಿ (ಅಂಕೆ_Bytes == 0 || bytes[0] == B_FUNCTION_ಕೀಲಿ) {
		beoskey = F(scancode);
		first = FIRST_FUNCTION_ಕೀಲಿ;
		last = ಅಂಕೆ_SPECIAL_ಕೀಲಿಗಳು;
	    } ಅನ್ಯಥಾ ಯದಿ (*bytes == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		first = last = 0;
	    } ಅನ್ಯಥಾ {
		beoskey = K(bytes[0]);
		first = 0;
		last = FIRST_FUNCTION_ಕೀಲಿ;
	    }

	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = first; i < last; i++) {
		ಯದಿ (special_ಕೀಲಿಗಳು[i].Be_ಕೀಲಿಗಳು == beoskey) {
		    string[0] = CSI;
		    string[1] = special_ಕೀಲಿಗಳು[i].vim_code0;
		    string[2] = special_ಕೀಲಿಗಳು[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	ಯದಿ (len == 0) {
	    string[0] = bytes[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#ಯದಿ 0
	ಯದಿ (len == 3 ||
		bytes[0] == B_SPACE || bytes[0] == B_TAB ||
		bytes[0] == B_RETURN || bytes[0] == '\r' ||
		bytes[0] == B_ESCAPE)
#ಅನ್ಯಥಾ
	    ಯದಿ (canHaveVimModifiers)
#endif
	    {
		ಪೂರ್ಣಾಙ್ಕ modifiers;
		modifiers = 0;
		ಯದಿ (beModifiers & B_SHIFT_ಕೀಲಿ)
		    modifiers |= MOD_MASK_SHIFT;
		ಯದಿ (beModifiers & B_CONTROL_ಕೀಲಿ)
		    modifiers |= MOD_MASK_CTRL;
		ಯದಿ (beModifiers & B_OPTION_ಕೀಲಿ)
		    modifiers |= MOD_MASK_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ;
		ಯದಿ (string[0] == CSI && len == 3)
		    ಕೀಲಿ = TO_SPECIAL(string[1], string[2]);
		ಅನ್ಯಥಾ
		    ಕೀಲಿ = string[0];
		ಕೀಲಿ = simplify_ಕೀಲಿ(ಕೀಲಿ, &modifiers);
		ಯದಿ (IS_SPECIAL(ಕೀಲಿ))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(ಕೀಲಿ);
		    string[2] = K_THIRD(ಕೀಲಿ);
		    len = 3;
		}
		ಅನ್ಯಥಾ
		{
		    string[0] = ಕೀಲಿ;
		    len = 1;
		}

		ಯದಿ (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.length = 3;
		}
	    }
	memcpy((ವರ್ಣ *)dest, string, len);
	km.length += len;
	km.csi_escape = ಅಸತ್ಯ;
    }

    write_port(gui.vdcmp, Vim_ಸಂದೇಶ::ಕೀಲಿ_2, &km, ಪರಿಮಾಣ(km));

    /*
     * blank out the pointer if necessary
     */
    ಯದಿ (p_mh && !gui.pointer_hidden)
    {
	guiBlankMouse(ಸತ್ಯ);
	gui.pointer_hidden = TRUE;
    }
}
ಶೂನ್ಯ
VimTextAreaView::guiSendMouseEvent(
	ಪೂರ್ಣಾಙ್ಕ	button,
	ಪೂರ್ಣಾಙ್ಕ	x,
	ಪೂರ್ಣಾಙ್ಕ	y,
	ಪೂರ್ಣಾಙ್ಕ	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_ಸಂದೇಶ mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_port(gui.vdcmp, Vim_ಸಂದೇಶ::Mouse, &mm, ಪರಿಮಾಣ(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    ಯದಿ (gui.pointer_hidden)
    {
	guiBlankMouse(ಅಸತ್ಯ);
	gui.pointer_hidden = FALSE;
    }
}

ಶೂನ್ಯ
VimTextAreaView::guiMouseMoved(
	ಪೂರ್ಣಾಙ್ಕ	x,
	ಪೂರ್ಣಾಙ್ಕ	y)
{
    Vim_Mouse_Moved_ಸಂದೇಶ mm;

    mm.x = x;
    mm.y = y;

    write_port(gui.vdcmp, Vim_ಸಂದೇಶ::MouseMoved, &mm, ಪರಿಮಾಣ(mm));

    ಯದಿ (gui.pointer_hidden)
    {
	guiBlankMouse(ಅಸತ್ಯ);
	gui.pointer_hidden = FALSE;
    }
}

    ಶೂನ್ಯ
VimTextAreaView::guiBlankMouse(ತಾರ್ಕಿಕ should_hide)
{
    ಯದಿ (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } ಅನ್ಯಥಾ {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
VimTextAreaView::mouseModifiersToVim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    ಯದಿ (beModifiers & B_SHIFT_ಕೀಲಿ)
	vim_modifiers |= MOUSE_SHIFT;
    ಯದಿ (beModifiers & B_CONTROL_ಕೀಲಿ)
	vim_modifiers |= MOUSE_CTRL;
    ಯದಿ (beModifiers & B_OPTION_ಕೀಲಿ)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    ಪ್ರತ್ಯಾಗಮ vim_modifiers;
}

    ಶೂನ್ಯ
VimTextAreaView::MouseDown(BPoint point)
{
    BMessage *m = Window()->CurrentMessage();
    ಪ್ರತಿಜ್ಞಾ(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    ಪೂರ್ಣಾಙ್ಕ vimButton;

    ಯದಿ (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_ಎಡ;
    ಅನ್ಯಥಾ ಯದಿ (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_ಬಲ;
    ಅನ್ಯಥಾ ಯದಿ (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_ಮಧ್ಯ;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ;		// Unknown button

    vimMouseButton = 1;	    // don't care which one

    // Handle multiple clicks
    int32 clicks = 0;
    m->FindInt32("clicks", &clicks);

    int32 modifiers = 0;
    m->FindInt32("modifiers", &modifiers);

    vimMouseModifiers = mouseModifiersToVim(modifiers);

    guiSendMouseEvent(vimButton, point.x, point.y,
	    clicks > 1 /* = repeated_click*/, vimMouseModifiers);
}

    ಶೂನ್ಯ
VimTextAreaView::MouseUp(BPoint point)
{
    vimMouseButton = 0;

    BMessage *m = Window()->CurrentMessage();
    ಪ್ರತಿಜ್ಞಾ(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->FindInt32("modifiers", &modifiers);

    vimMouseModifiers = mouseModifiersToVim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, point.x, point.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::MouseUp(point);
}

    ಶೂನ್ಯ
VimTextAreaView::MouseMoved(BPoint point, uint32 transit, ಧ್ರುವ BMessage *message)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    ಯದಿ (gui.pointer_hidden)
    {
	guiBlankMouse(ಅಸತ್ಯ);
	gui.pointer_hidden = FALSE;
    }

    ಯದಿ (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(point.x, point.y);
	ಪ್ರತ್ಯಾಗಮ;
    }

    atomic_add(&mouse_Drag_Event_ಎಣಿಕೆ, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, point.x, point.y, 0, vimMouseModifiers);
}

    ಶೂನ್ಯ
VimTextAreaView::MessageReceived(BMessage *m)
{
    ವಿಕಲ್ಪ (m->what) {
	ಪಕ್ಷ 'menu':
	    {
		Vim_Menu_ಸಂದೇಶ mm;
		mm.guiMenu = ರಿಕ್ತ;  // in case no pointer in msg
		m->FindPointer("VimMenu", (ಶೂನ್ಯ **)&mm.guiMenu);
		write_port(gui.vdcmp, Vim_ಸಂದೇಶ::Menu, &mm, ಪರಿಮಾಣ(mm));
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ B_MOUSE_WHEEL_CHANGED:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		ಭಿನ್ನ small=0, big=0, dy=0;
		m->FindFloat("be:wheel_delta_y", &dy);
		scb->GetSteps(&small, &big);
		scb->Set_ಮೌಲ್ಯ(scb->ಮೌಲ್ಯ_2()+small*dy*3);
		scb->ಮೌಲ್ಯ_Changed(scb->ಮೌಲ್ಯ_2());
#ಯದಿ 0
		scb = curwin->w_scrollbars[0].id;
		scb->GetSteps(&small, &big);
		scb->Set_ಮೌಲ್ಯ(scb->ಮೌಲ್ಯ_2()+small*dy);
		scb->ಮೌಲ್ಯ_Changed(scb->ಮೌಲ್ಯ_2());
#endif
	    }
	    ಭಙ್ಗ;
#ifdef FEAT_MBYTE_IME
	ಪಕ್ಷ B_ಒಳಹರಿವು_METHOD_EVENT:
	    {
		int32 opcode;
		m->FindInt32("be:opcode", &opcode);
		ವಿಕಲ್ಪ(opcode)
		{
		    ಪಕ್ಷ B_ಒಳಹರಿವು_METHOD_STARTED:
			ಯದಿ (!IMData.messenger) delete IMData.messenger;
			IMData.messenger = ನೂತನ BMessenger();
			m->FindMessenger("be:reply_to", IMData.messenger);
			ಭಙ್ಗ;
		    ಪಕ್ಷ B_ಒಳಹರಿವು_METHOD_CHANGED:
			{
			    BString str;
			    ತಾರ್ಕಿಕ confirmed;
			    ಯದಿ (IMData.message) *(IMData.message) = *m;
			    ಅನ್ಯಥಾ	       IMData.message = ನೂತನ BMessage(*m);
			    DrawIMString();
			    m->FindBool("be:confirmed", &confirmed);
			    ಯದಿ (confirmed)
			    {
				m->FindString("be:string", &str);
				char_u *chars = (char_u*)str.String();
				ಸಂರಚನಾ Vim_ಕೀಲಿ_ಸಂದೇಶ km;
				km.csi_escape = ಸತ್ಯ;
				ಪೂರ್ಣಾಙ್ಕ clen;
				ಪೂರ್ಣಾಙ್ಕ i = 0;
				ಯಾವತ್ (i < str.Length())
				{
				    clen = utf_ptr2len(chars+i);
				    memcpy(km.chars, chars+i, clen);
				    km.length = clen;
				    write_port(gui.vdcmp, Vim_ಸಂದೇಶ::ಕೀಲಿ_2, &km, ಪರಿಮಾಣ(km));
				    i += clen;
				}
			    }
			}
			ಭಙ್ಗ;
		    ಪಕ್ಷ B_ಒಳಹರಿವು_METHOD_LOCATION_REQUEST:
			{
			    BMessage ಸಂದೇಶ(B_ಒಳಹರಿವು_METHOD_EVENT);
			    ಸಂದೇಶ.AddInt32("be:opcode", B_ಒಳಹರಿವು_METHOD_LOCATION_REQUEST);
			    ಸಂದೇಶ.AddPoint("be:location_reply", IMData.location);
			    ಸಂದೇಶ.AddFloat("be:height_reply", FILL_Y(1));
			    IMData.messenger->SendMessage(&ಸಂದೇಶ);
			}
			ಭಙ್ಗ;
		    ಪಕ್ಷ B_ಒಳಹರಿವು_METHOD_STOPPED:
			delete IMData.messenger;
			delete IMData.message;
			IMData.messenger = ರಿಕ್ತ;
			IMData.message = ರಿಕ್ತ;
			ಭಙ್ಗ;
		}
	    }
	    // TODO: sz: break here???
#endif
	ಸ್ವಭಾವ:
	    ಯದಿ (m->WasDropped()) {
		BWindow *w = Window();
		w->DetachCurrentMessage();
		w->Minimize(ಅಸತ್ಯ);
		VimApp::SendRefs(m, (modifiers() & B_SHIFT_ಕೀಲಿ) != 0);
	    } ಅನ್ಯಥಾ {
		Inherited::MessageReceived(m);
	    }
	    ಭಙ್ಗ;
    }
}

    ಪೂರ್ಣಾಙ್ಕ
VimTextAreaView::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    ಯದಿ (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_set_font((GuiFont)newFont);
	ಯದಿ (name && STRCMP(name, "*") != 0)
	    hl_set_font_name(name);

	SetDrawingMode(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಶೂನ್ಯ
VimTextAreaView::mchDrawString(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, char_u *s, ಪೂರ್ಣಾಙ್ಕ len, ಪೂರ್ಣಾಙ್ಕ flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    ಯದಿ (!(flags & DRAW_TRANSP)) {
	ಪೂರ್ಣಾಙ್ಕ cells;
	cells = 0;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i=0; i<len; i++) {
	    ಪೂರ್ಣಾಙ್ಕ cn = utf_ptr2cells((char_u *)(s+i));
	    ಯದಿ (cn<4) cells += cn;
	}

	BRect r(FILL_X(col), FILL_Y(row),
		FILL_X(col + cells) - PEN_WIDTH, FILL_Y(row + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_ಕೆಳಗಿನ);
    }

    BFont font;
    ಏತತ್->GetFont(&font);
    ಯದಿ (!font.IsFixed())
    {
	ವರ್ಣ* p = (ವರ್ಣ*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	ಪೂರ್ಣಾಙ್ಕ cells;
	ಯಾವತ್ ((p - (ವರ್ಣ*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.Set(TEXT_X(col+lastpos), TEXT_Y(row));
	    DrawString(p, clen, where);
	    ಯದಿ (flags & DRAW_BOLD) {
		where.x += 1.0;
		SetDrawingMode(B_OP_BLEND);
		DrawString(p, clen, where);
		SetDrawingMode(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    ಯದಿ (cells<4) lastpos += cells;
	    ಅನ್ಯಥಾ	lastpos++;
	    p += clen;
	}
    }
    ಅನ್ಯಥಾ
    {
	BPoint where(TEXT_X(col), TEXT_Y(row));
	DrawString((ವರ್ಣ*)s, len, where);
	ಯದಿ (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    SetDrawingMode(B_OP_BLEND);
	    DrawString((ವರ್ಣ*)s, len, where);
	    SetDrawingMode(B_OP_COPY);
	}
    }

    ಯದಿ (flags & DRAW_UNDERL) {
	ಪೂರ್ಣಾಙ್ಕ cells;
	cells = 0;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i=0; i<len; i++) {
	    ಪೂರ್ಣಾಙ್ಕ cn = utf_ptr2cells((char_u *)(s+i));
	    ಯದಿ (cn<4) cells += cn;
	}

	BPoint start(FILL_X(col), FILL_Y(row + 1) - PEN_WIDTH);
	BPoint end(FILL_X(col + cells) - PEN_WIDTH, start.y);

	Stroke_ಸಾಲು(start, end);
    }
}

ಶೂನ್ಯ
VimTextAreaView::mchClearBlock(
	ಪೂರ್ಣಾಙ್ಕ	row1,
	ಪೂರ್ಣಾಙ್ಕ	col1,
	ಪೂರ್ಣಾಙ್ಕ	row2,
	ಪೂರ್ಣಾಙ್ಕ	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_WIDTH, FILL_Y(row2 + 1) - PEN_WIDTH);
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(r, B_SOLID_ಕೆಳಗಿನ);
}

    ಶೂನ್ಯ
VimTextAreaView::mchClearAll()
{
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(Bounds(), B_SOLID_ಕೆಳಗಿನ);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    ಶೂನ್ಯ
VimTextAreaView::mch_Delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    BRect source, dest;
    source.ಎಡ = FILL_X(gui.scroll_region_ಎಡ);
    source.ತುದಿ = FILL_Y(row + ಅಂಕೆ_ಸಾಲುಗಳು);
    source.ಬಲ = FILL_X(gui.scroll_region_ಬಲ + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    dest.ಎಡ = FILL_X(gui.scroll_region_ಎಡ);
    dest.ತುದಿ = FILL_Y(row);
    dest.ಬಲ = FILL_X(gui.scroll_region_ಬಲ + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1) - PEN_WIDTH;

    ಯದಿ (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1,
		gui.scroll_region_ಎಡ,
		gui.scroll_region_bot, gui.scroll_region_ಬಲ);


	gui.vimWindow->Unlock();
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
VimTextAreaView::mch_ಸೇರಿಸು_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    BRect source, dest;

    // XXX Attempt at a hack:
    gui.vimWindow->UpdateIfNeeded();
    source.ಎಡ = FILL_X(gui.scroll_region_ಎಡ);
    source.ತುದಿ = FILL_Y(row);
    source.ಬಲ = FILL_X(gui.scroll_region_ಬಲ + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1) - PEN_WIDTH;

    dest.ಎಡ = FILL_X(gui.scroll_region_ಎಡ);
    dest.ತುದಿ = FILL_Y(row + ಅಂಕೆ_ಸಾಲುಗಳು);
    dest.ಬಲ = FILL_X(gui.scroll_region_ಬಲ + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    ಯದಿ (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(row, gui.scroll_region_ಎಡ,
		row + ಅಂಕೆ_ಸಾಲುಗಳು - 1, gui.scroll_region_ಬಲ);

	gui.vimWindow->Unlock();
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

#ifdef FEAT_MBYTE_IME
/*
 * DrawIMString draws string with IMData.message.
 */
ಶೂನ್ಯ VimTextAreaView::DrawIMString(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಧ್ರುವ rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    ಧ್ರುವ ವರ್ಣ* s;
    ಪೂರ್ಣಾಙ್ಕ len;
    BMessage* ಸಂದೇಶ = IMData.message;
    ಯದಿ (!ಸಂದೇಶ)
	ಪ್ರತ್ಯಾಗಮ;
    gui_redraw_block(IMData.row, 0,
	    IMData.row + IMData.ಎಣಿಕೆ, W_WIDTH(curwin), GUI_MON_NOCLEAR);
    ತಾರ್ಕಿಕ confirmed = ಅಸತ್ಯ;
    ಸಂದೇಶ->FindBool("be:confirmed", &confirmed);
    ಯದಿ (confirmed)
	ಪ್ರತ್ಯಾಗಮ;
    rgb_color hcolor = ಮೇಲಿನ_Color(), lcolor = ಕೆಳಗಿನ_Color();
    ಸಂದೇಶ->FindString("be:string", &str);
    s = str.String();
    len = str.Length();
    Set_ಮೇಲಿನ_Color(0, 0, 0);
    IMData.row = gui.row;
    IMData.col = gui.col;
    int32 sel_start = 0, sel_end = 0;
    ಸಂದೇಶ->FindInt32("be:selection", 0, &sel_start);
    ಸಂದೇಶ->FindInt32("be:selection", 1, &sel_end);
    ಪೂರ್ಣಾಙ್ಕ clen, cn;
    BPoint pos(IMData.col, 0);
    BRect r;
    BPoint where;
    IMData.location = ConvertToScreen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	ಯದಿ (pos.x + cn > W_WIDTH(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	ಯದಿ (sel_start<=i && i<sel_end)
	{
	    Set_ಕೆಳಗಿನ_Color(r_highlight);
	    IMData.location = ConvertToScreen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
	}
	ಅನ್ಯಥಾ
	{
	    Set_ಕೆಳಗಿನ_Color(b_highlight);
	}
	r.Set(FILL_X(pos.x), FILL_Y(IMData.row + pos.y),
		FILL_X(pos.x + cn) - PEN_WIDTH,
		FILL_Y(IMData.row + pos.y + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_ಕೆಳಗಿನ);
	where.Set(TEXT_X(pos.x), TEXT_Y(IMData.row + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.ಎಣಿಕೆ = (ಪೂರ್ಣಾಙ್ಕ)pos.y;

    Set_ಮೇಲಿನ_Color(hcolor);
    Set_ಕೆಳಗಿನ_Color(lcolor);
}
#endif
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
	    "vim scrollbar", (BView *)ರಿಕ್ತ,
	    0.0, 10.0, posture),
    ignore_ಮೌಲ್ಯ(-1),
    scroll_Event_ಎಣಿಕೆ(0)
{
    gsb = g;
    SetResizingMode(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    ಶೂನ್ಯ
VimScrollBar::ಮೌಲ್ಯ_Changed(ಭಿನ್ನ new_ಮೌಲ್ಯ)
{
    ಯದಿ (ignore_ಮೌಲ್ಯ >= 0.0 && new_ಮೌಲ್ಯ == ignore_ಮೌಲ್ಯ) {
	ignore_ಮೌಲ್ಯ = -1;
	ಪ್ರತ್ಯಾಗಮ;
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
    atomic_add(&scroll_Event_ಎಣಿಕೆ, 1);

    ಸಂರಚನಾ Vim_Scroll_Bar_ಸಂದೇಶ sm;

    sm.sb = ಏತತ್;
    sm.ಮೌಲ್ಯ = (long) new_ಮೌಲ್ಯ;
    sm.stillDragging = TRUE;

    write_port(gui.vdcmp, Vim_ಸಂದೇಶ::ScrollBar, &sm, ಪರಿಮಾಣ(sm));

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
VimScrollBar::MouseUp(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_add(&scroll_Event_ಎಣಿಕೆ, 1);

    ಸಂರಚನಾ Vim_Scroll_Bar_ಸಂದೇಶ sm;

    sm.sb = ಏತತ್;
    sm.ಮೌಲ್ಯ = (long) ಮೌಲ್ಯ_2();
    sm.stillDragging = FALSE;

    write_port(gui.vdcmp, Vim_ಸಂದೇಶ::ScrollBar, &sm, ಪರಿಮಾಣ(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::MouseUp(where);
}

    ಶೂನ್ಯ
VimScrollBar::Set_ಮೌಲ್ಯ(ಭಿನ್ನ new_ಮೌಲ್ಯ)
{
    ಯದಿ (new_ಮೌಲ್ಯ == ಮೌಲ್ಯ_2())
	ಪ್ರತ್ಯಾಗಮ;

    ignore_ಮೌಲ್ಯ = new_ಮೌಲ್ಯ;
    Inherited::Set_ಮೌಲ್ಯ(new_ಮೌಲ್ಯ);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(ಧ್ರುವ VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ಧ್ರುವ BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ಧ್ರುವ VimFont &rhs): BFont(rhs)
{
    init();
}

VimFont::~VimFont()
{
}

    ಶೂನ್ಯ
VimFont::init()
{
    next = ರಿಕ್ತ;
    refcount = 1;
    name = ರಿಕ್ತ;
}

// ---------------- VimDialog ----------------

#ಯದಿ defined(FEAT_GUI_DIALOG)

ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  k_Vim_Dialog_Button_ಸಂದೇಶ = 'VMDB';
ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  kVimDialogIconStripeWidth = 30;
ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  kVimDialogButtonsSpacingX = 9;
ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  kVimDialogButtonsSpacingY = 4;
ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  kVimDialogSpacingX = 6;
ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  kVimDialogSpacingY = 10;
ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  kVimDialogMinimalWidth  = 310;
ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  kVimDialogMinimalHeight = 75;
ಧ್ರುವ BRect	    kDefaultRect =
BRect(0, 0, kVimDialogMinimalWidth, kVimDialogMinimalHeight);

VimDialog::VimDialog(ಪೂರ್ಣಾಙ್ಕ type, ಧ್ರುವ ವರ್ಣ *title, ಧ್ರುವ ವರ್ಣ *message,
	ಧ್ರುವ ವರ್ಣ *buttons, ಪೂರ್ಣಾಙ್ಕ dfltbutton, ಧ್ರುವ ವರ್ಣ *textfield, ಪೂರ್ಣಾಙ್ಕ ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_ಮೌಲ್ಯ(dfltbutton)
    , fMessageView(ರಿಕ್ತ)
    , f_ಒಳಹರಿವು_Control(ರಿಕ್ತ)
    , f_ಒಳಹರಿವು_ಮೌಲ್ಯ(textfield)
{
    //	master view
    VimDialog::View* view = ನೂತನ VimDialog::View(Bounds());
    ಯದಿ (view == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (title == ರಿಕ್ತ)
	SetTitle("Vim " VIM_VERSION_MEDIUM);

    AddChild(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    ಭಿನ್ನ maxButtonWidth  = 0;
    ಭಿನ್ನ maxButtonHeight = 0;
    ಭಿನ್ನ buttonsWidth	  = 0;
    ಭಿನ್ನ buttonsHeight   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    ಕುರು
    {
	int32 end = strButtons.FindFirst('\n');
	ಯದಿ (end != B_ERROR)
	    strButtons.SetByteAt(end, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->AddChild(button);
	fButtonsList.AddItem(button);

	maxButtonWidth	= max_c(maxButtonWidth,  button->Bounds().Width());
	maxButtonHeight = max_c(maxButtonHeight, button->Bounds().Height());
	buttonsWidth   += button->Bounds().Width();
	buttonsHeight  += button->Bounds().Height();

	ಯದಿ (end == B_ERROR)
	    ಭಙ್ಗ;

	strButtons.Remove(0, end + 1);
    } ಯಾವತ್ (ಸತ್ಯ);

    int32 buttons_ಎಣಿಕೆ = fButtonsList.ಎಣಿಕೆ_ವಸ್ತುಗಳು();
    buttonsWidth      += kVimDialogButtonsSpacingX * (buttons_ಎಣಿಕೆ - 1);
    buttonsHeight     += kVimDialogButtonsSpacingY * (buttons_ಎಣಿಕೆ - 1);
    ಭಿನ್ನ dialogWidth  = buttonsWidth + kVimDialogIconStripeWidth +
	kVimDialogSpacingX * 2;
    ಭಿನ್ನ dialogHeight = maxButtonHeight + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    ತಾರ್ಕಿಕ vertical = (vim_strchr(p_go, GO_VERTICAL) != ರಿಕ್ತ) ||
	dialogWidth >= gui.vimWindow->Bounds().Width();
    ಯದಿ (vertical) {
	dialogWidth  -= buttonsWidth;
	dialogWidth  += maxButtonWidth;
	dialogHeight -= maxButtonHeight;
	dialogHeight += buttonsHeight;
    }

    dialogWidth  = max_c(dialogWidth,  kVimDialogMinimalWidth);

    //	message view
    BRect rect(0, 0, dialogWidth, 0);
    rect.ಎಡ  += kVimDialogIconStripeWidth + 16 + kVimDialogSpacingX;
    rect.ತುದಿ   += kVimDialogSpacingY;
    rect.ಬಲ -= kVimDialogSpacingX;
    rect.bottom = rect.ತುದಿ;
    fMessageView = ನೂತನ BTextView(rect, "_tv_", rect.OffsetByCopy(B_ORIGIN),
	    B_FOLLOW_ಎಡ | B_FOLLOW_ತುದಿ, B_WILL_DRAW);

    fMessageView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color textColor = ui_color(B_PANEL_TEXT_COLOR);
    fMessageView->SetFontAndColor(be_plain_font, B_FONT_ALL, &textColor);
    fMessageView->SetText(message);
    fMessageView->MakeEditable(ಅಸತ್ಯ);
    fMessageView->MakeSelectable(ಅಸತ್ಯ);
    fMessageView->SetWordWrap(ಸತ್ಯ);
    AddChild(fMessageView);

    ಭಿನ್ನ messageHeight = fMessageView->TextHeight(0, fMessageView->ಎಣಿಕೆ_ಸಾಲುಗಳು());
    fMessageView->ResizeBy(0, messageHeight);
    fMessageView->SetTextRect(BRect(0, 0, rect.Width(), messageHeight));

    dialogHeight += messageHeight;

    //	input view
    ಯದಿ (f_ಒಳಹರಿವು_ಮೌಲ್ಯ != ರಿಕ್ತ) {
	rect.ತುದಿ     =
	    rect.bottom += messageHeight + kVimDialogSpacingY;
	f_ಒಳಹರಿವು_Control = ನೂತನ BTextControl(rect, "_iv_", ರಿಕ್ತ, f_ಒಳಹರಿವು_ಮೌಲ್ಯ, ರಿಕ್ತ,
		B_FOLLOW_ಎಡ | B_FOLLOW_ತುದಿ, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_ಒಳಹರಿವು_Control->TextView()->SetText(f_ಒಳಹರಿವು_ಮೌಲ್ಯ);
	f_ಒಳಹರಿವು_Control->TextView()->SetWordWrap(ಅಸತ್ಯ);
	AddChild(f_ಒಳಹರಿವು_Control);

	ಭಿನ್ನ width = 0.f, height = 0.f;
	f_ಒಳಹರಿವು_Control->GetPreferredSize(&width, &height);
	f_ಒಳಹರಿವು_Control->MakeFocus(ಸತ್ಯ);

	dialogHeight += height + kVimDialogSpacingY * 1.5;
    }

    dialogHeight = max_c(dialogHeight, kVimDialogMinimalHeight);

    ResizeTo(dialogWidth, dialogHeight);
    MoveTo((gui.vimWindow->Bounds().Width() - dialogWidth) / 2,
	    (gui.vimWindow->Bounds().Height() - dialogHeight) / 2);

    //	adjust layout of buttons
    ಭಿನ್ನ buttonWidth = max_c(maxButtonWidth, rect.Width() * 0.66);
    BPoint origin(dialogWidth, dialogHeight);
    origin.x -= kVimDialogSpacingX + (vertical ? buttonWidth : buttonsWidth);
    origin.y -= kVimDialogSpacingY + (vertical ? buttonsHeight	: maxButtonHeight);

    ಪ್ರತಿ (int32 i = 0 ; i < buttons_ಎಣಿಕೆ; i++) {
	BButton *button = (BButton*)fButtonsList.ItemAt(i);
	button->MoveTo(origin);
	ಯದಿ (vertical) {
	    origin.y += button->Frame().Height() + kVimDialogButtonsSpacingY;
	    button->ResizeTo(buttonWidth, button->Frame().Height());
	} ಅನ್ಯಥಾ
	    origin.x += button->Frame().Width() + kVimDialogButtonsSpacingX;

	ಯದಿ (dfltbutton == i + 1) {
	    button->MakeDefault(ಸತ್ಯ);
	    button->MakeFocus(f_ಒಳಹರಿವು_Control == ರಿಕ್ತ);
	}
    }
}

VimDialog::~VimDialog()
{
    ಯದಿ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    ಪೂರ್ಣಾಙ್ಕ
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    ಯದಿ (fDialogSem < B_OK) {
	Quit();
	ಪ್ರತ್ಯಾಗಮ f_Dialog_ಮೌಲ್ಯ;
    }

    Show();

    ಯಾವತ್ (acquire_sem(fDialogSem) == B_INTERRUPTED);

    ಪೂರ್ಣಾಙ್ಕ ret_ಮೌಲ್ಯ = f_Dialog_ಮೌಲ್ಯ;
    ಯದಿ (f_ಒಳಹರಿವು_ಮೌಲ್ಯ != ರಿಕ್ತ)
	vim_strncpy((char_u*)f_ಒಳಹರಿವು_ಮೌಲ್ಯ, (char_u*)f_ಒಳಹರಿವು_Control->Text(), IOSIZE - 1);

    ಯದಿ (Lock())
	Quit();

    ಪ್ರತ್ಯಾಗಮ ret_ಮೌಲ್ಯ;
}

ಶೂನ್ಯ VimDialog::MessageReceived(BMessage *ಸಂದೇಶ)
{
    int32 which = 0;
    ಯದಿ (ಸಂದೇಶ->what != k_Vim_Dialog_Button_ಸಂದೇಶ ||
	    ಸಂದೇಶ->FindInt32("which", &which) != B_OK)
	ಪ್ರತ್ಯಾಗಮ BWindow::MessageReceived(ಸಂದೇಶ);

    f_Dialog_ಮೌಲ್ಯ = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, ಧ್ರುವ ವರ್ಣ* label)
{
    BMessage *message = ನೂತನ BMessage(k_Vim_Dialog_Button_ಸಂದೇಶ);
    message->AddInt32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = ನೂತನ BButton(rect, name.String(), label, message,
	    B_FOLLOW_ಬಲ | B_FOLLOW_BOTTOM);

    ಭಿನ್ನ width = 0.f, height = 0.f;
    button->GetPreferredSize(&width, &height);
    button->ResizeTo(width, height);

    ಪ್ರತ್ಯಾಗಮ button;
}

VimDialog::View::View(BRect frame)
    :	BView(frame, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(ರಿಕ್ತ)
{
    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    delete fIconBitmap;
}

ಶೂನ್ಯ VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.ಬಲ = kVimDialogIconStripeWidth;
    Set_ಮೇಲಿನ_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    ಯದಿ (fIconBitmap == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

ಶೂನ್ಯ VimDialog::View::InitIcon(int32 type)
{
    ಯದಿ (type == VIM_GENERIC)
	ಪ್ರತ್ಯಾಗಮ;

    BPath path;
    status_t status = find_directory(B_BEOS_SERVERS_DIRECTORY, &path);
    ಯದಿ (status != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(status));
	ಪ್ರತ್ಯಾಗಮ;
    }

    path.Append("app_server");

    BFile file(path.Path(), O_RDONLY);
    ಯದಿ (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	ಪ್ರತ್ಯಾಗಮ;
    }

    BResources resources(&file);
    ಯದಿ (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಧ್ರುವ ವರ್ಣ *name = "";
    ವಿಕಲ್ಪ(type) {
	ಪಕ್ಷ VIM_ERROR:	    name = "stop"; ಭಙ್ಗ;
	ಪಕ್ಷ VIM_WARNING:   name = "warn"; ಭಙ್ಗ;
	ಪಕ್ಷ VIM_INFO:	    name = "info"; ಭಙ್ಗ;
	ಪಕ್ಷ VIM_QUESTION:  name = "idea"; ಭಙ್ಗ;
	ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ;
    }

    int32 iconSize = 32;
    fIconBitmap = ನೂತನ BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1), 0, B_RGBA32);
    ಯದಿ (fIconBitmap == ರಿಕ್ತ || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == ರಿಕ್ತ) ? "null" : strerror(fIconBitmap->InitCheck()));
	ಪ್ರತ್ಯಾಗಮ;
    }

    size_t size = 0;
    ಧ್ರುವ uint8* iconData = ರಿಕ್ತ;
    //	try vector icon first?
    iconData = (ಧ್ರುವ uint8*)resources.LoadResource(B_VECTOR_ICON_TYPE, name, &size);
    ಯದಿ (iconData != ರಿಕ್ತ && BIconUtils::GetVectorIcon(iconData, size, fIconBitmap) == B_OK)
	ಪ್ರತ್ಯಾಗಮ;

    //	try bitmap icon now
    iconData = (ಧ್ರುವ uint8*)resources.LoadResource(B_LARGE_ICON_TYPE, name, &size);
    ಯದಿ (iconData == ರಿಕ್ತ) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	delete fIconBitmap;
	fIconBitmap = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(iconData, iconSize, iconSize, iconSize, fIconBitmap);
}

ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  k_Vim_Dialog_OKButton_ಸಂದೇಶ = 'FDOK';
ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  k_Vim_Dialog_Cancel_Button_ಸಂದೇಶ = 'FDCN';
ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  k_Vim_Dialog_Size_ಒಳಹರಿವು_ಸಂದೇಶ = 'SICH';
ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  k_Vim_Dialog_Family_Select_ಸಂದೇಶ = 'MSFM';
ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  k_Vim_Dialog_Style_Select_ಸಂದೇಶ = 'MSST';
ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ  k_Vim_Dialog_Size_Select_ಸಂದೇಶ = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, ಭಿನ್ನ* size)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fStatus(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_ಮೌಲ್ಯ(ಅಸತ್ಯ)
    , fFamily(family)
    , fStyle(style)
    , fSize(size)
    , fFontSize(*size)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_ಒಳಹರಿವು(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_LENGTH);
    strncpy(fFontStyle, *style, B_FONT_STYLE_LENGTH);

    //	"client" area view
    BBox *clientBox = ನೂತನ BBox(Bounds(), B_EMPTY_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    AddChild(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.ಎಡ_ತುದಿ(), RC.ಎಡ_ತುದಿ());

    //	at first create all controls
    fPreview = ನೂತನ BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->AddChild(fPreview);

    BBox* boxDivider = ನೂತನ BBox(rc, B_EMPTY_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->AddChild(boxDivider);

    BStringView *labelFamily = ನೂತನ BStringView(rc, "labelFamily", "Family:");
    clientBox->AddChild(labelFamily);
    labelFamily->ResizeToPreferred();

    BStringView *labelStyle = ನೂತನ BStringView(rc, "labelStyle", "Style:");
    clientBox->AddChild(labelStyle);
    labelStyle->ResizeToPreferred();

    BStringView *labelSize = ನೂತನ BStringView(rc, "labelSize", "Size:");
    clientBox->AddChild(labelSize);
    labelSize->ResizeToPreferred();

    fFamiliesList = ನೂತನ BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = ನೂತನ BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_ಎಡ_ಬಲ, 0, ಅಸತ್ಯ, ಸತ್ಯ);
    clientBox->AddChild(scrollFamilies);

    fStylesList= ನೂತನ BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = ನೂತನ BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_ಎಡ_ಬಲ, 0, ಅಸತ್ಯ, ಸತ್ಯ);
    clientBox->AddChild(scrollStyles);

    f_Sizes_ಒಳಹರಿವು = ನೂತನ BTextControl(rc, "inputSize", ರಿಕ್ತ, "???",
	    ನೂತನ BMessage(k_Vim_Dialog_Size_ಒಳಹರಿವು_ಸಂದೇಶ));
    clientBox->AddChild(f_Sizes_ಒಳಹರಿವು);
    f_Sizes_ಒಳಹರಿವು->ResizeToPreferred();

    fSizesList = ನೂತನ BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = ನೂತನ BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_ಎಡ_ಬಲ, 0, ಅಸತ್ಯ, ಸತ್ಯ);
    clientBox->AddChild(scrollSizes);

    BButton *buttonOK = ನೂತನ BButton(rc, "buttonOK", "OK",
			ನೂತನ BMessage(k_Vim_Dialog_OKButton_ಸಂದೇಶ));
    clientBox->AddChild(buttonOK);
    buttonOK->ResizeToPreferred();

    BButton *buttonCancel = ನೂತನ BButton(rc, "buttonCancel", "Cancel",
			ನೂತನ BMessage(k_Vim_Dialog_Cancel_Button_ಸಂದೇಶ));
    clientBox->AddChild(buttonCancel);
    buttonCancel->ResizeToPreferred();

    //	layout controls
    ಭಿನ್ನ ಸಾಲು_Height = labelFamily->Bounds().Height();
    ಭಿನ್ನ previewHeight = ಸಾಲು_Height * 3;
    ಭಿನ್ನ offsetYLabels = previewHeight + kVimDialogSpacingY;
    ಭಿನ್ನ offsetYLists = offsetYLabels + ಸಾಲು_Height + kVimDialogSpacingY / 2;
    ಭಿನ್ನ offsetYSizes = offsetYLists + f_Sizes_ಒಳಹರಿವು->Bounds().Height() + kVimDialogSpacingY / 2;
    ಭಿನ್ನ listsHeight = ಸಾಲು_Height * 9;
    ಭಿನ್ನ offsetYButtons = offsetYLists + listsHeight +  kVimDialogSpacingY;
    ಭಿನ್ನ maxControlsHeight = offsetYButtons + buttonOK->Bounds().Height();
    ಭಿನ್ನ familiesWidth = labelFamily->Bounds().Width() * 5;
    ಭಿನ್ನ offsetXStyles = familiesWidth + kVimDialogSpacingX;
    ಭಿನ್ನ stylesWidth = labelStyle->Bounds().Width() * 4;
    ಭಿನ್ನ offsetXSizes = offsetXStyles + stylesWidth + kVimDialogSpacingX;
    ಭಿನ್ನ sizesWidth = labelSize->Bounds().Width() * 2;
    ಭಿನ್ನ maxControlsWidth = offsetXSizes + sizesWidth;

    ResizeTo(maxControlsWidth + kVimDialogSpacingX * 2,
	maxControlsHeight + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vimWindow->Frame();
    MoveTo(rcVim.ಎಡ + (rcVim.Width() - Frame().Width()) / 2,
	    rcVim.ತುದಿ + (rcVim.Height() - Frame().Height()) / 2);

    fPreview->ResizeTo(maxControlsWidth, previewHeight);
    fPreview->SetAlignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, previewHeight + kVimDialogSpacingY / 2);
    boxDivider->ResizeTo(maxControlsWidth, 1.f);

    labelFamily->MoveBy(0.f, offsetYLabels);
    labelStyle->MoveBy(offsetXStyles, offsetYLabels);
    labelSize->MoveBy(offsetXSizes, offsetYLabels);

    //	text control alignment issues
    ಭಿನ್ನ insetX = f_Sizes_ಒಳಹರಿವು->TextView()->Bounds().Width() - f_Sizes_ಒಳಹರಿವು->Bounds().Width();
    ಭಿನ್ನ insetY = f_Sizes_ಒಳಹರಿವು->TextView()->Bounds().Width() - f_Sizes_ಒಳಹರಿವು->Bounds().Width();

    scrollFamilies->MoveBy(0.f, offsetYLists);
    scrollStyles->MoveBy(offsetXStyles, offsetYLists);
    f_Sizes_ಒಳಹರಿವು->MoveBy(offsetXSizes + insetX / 2, offsetYLists + insetY / 2);
    scrollSizes->MoveBy(offsetXSizes, offsetYSizes);

    f_Sizes_ಒಳಹರಿವು->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->ResizeTo(familiesWidth, listsHeight);
    scrollStyles->ResizeTo(stylesWidth, listsHeight);
    f_Sizes_ಒಳಹರಿವು->ResizeTo(sizesWidth, f_Sizes_ಒಳಹರಿವು->Bounds().Height());
    scrollSizes->ResizeTo(sizesWidth,
	    listsHeight - (offsetYSizes - offsetYLists));

    buttonOK->MoveBy(maxControlsWidth - buttonOK->Bounds().Width(), offsetYButtons);
    buttonCancel->MoveBy(maxControlsWidth - buttonOK->Bounds().Width()
	    - buttonCancel->Bounds().Width() - kVimDialogSpacingX, offsetYButtons);

    //	fill lists
    ಪೂರ್ಣಾಙ್ಕ selIndex = -1;
    ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = ಎಣಿಕೆ_font_families();
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಎಣಿಕೆ; i++) {
	font_family family;
	ಯದಿ (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->AddItem(ನೂತನ BStringItem((ಧ್ರುವ ವರ್ಣ*)family));
	    ಯದಿ (strncmp(family, fFontFamily, B_FONT_FAMILY_LENGTH) == 0)
		selIndex = i;
	}
    }

    ಯದಿ (selIndex >= 0) {
	fFamiliesList->Select(selIndex);
	fFamiliesList->ScrollToSelection();
    }

    _UpdateFontStyles();

    selIndex = -1;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ size = 8, index = 0; size <= 18; size++, index++) {
	BString str;
	str << size;
	fSizesList->AddItem(ನೂತನ BStringItem(str));
	ಯದಿ (size == fFontSize)
	    selIndex = index;

    }

    ಯದಿ (selIndex >= 0) {
	fSizesList->Select(selIndex);
	fSizesList->ScrollToSelection();
    }

    fFamiliesList->SetSelectionMessage(ನೂತನ BMessage(k_Vim_Dialog_Family_Select_ಸಂದೇಶ));
    fStylesList->SetSelectionMessage(ನೂತನ BMessage(k_Vim_Dialog_Style_Select_ಸಂದೇಶ));
    fSizesList->SetSelectionMessage(ನೂತನ BMessage(k_Vim_Dialog_Size_Select_ಸಂದೇಶ));
    f_Sizes_ಒಳಹರಿವು->SetModificationMessage(ನೂತನ BMessage(k_Vim_Dialog_Size_ಒಳಹರಿವು_ಸಂದೇಶ));

    Update_Size_ಒಳಹರಿವು_Preview();
    _UpdateFontPreview();

    fStatus = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    ಯದಿ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    ಶೂನ್ಯ
VimSelectFontDialog::_CleanList(BListView* list)
{
    ಯಾವತ್ (0 < list->ಎಣಿಕೆ_ವಸ್ತುಗಳು())
	delete (dynamic_cast<BStringItem*>(list->RemoveItem((int32)0)));
}

    ತಾರ್ಕಿಕ
VimSelectFontDialog::Go()
{
    ಯದಿ (fStatus != B_OK) {
	Quit();
	ಪ್ರತ್ಯಾಗಮ NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    ಯದಿ (fDialogSem < B_OK) {
	Quit();
	ಪ್ರತ್ಯಾಗಮ f_Dialog_ಮೌಲ್ಯ;
    }

    Show();

    ಯಾವತ್ (acquire_sem(fDialogSem) == B_INTERRUPTED);

    ತಾರ್ಕಿಕ ret_ಮೌಲ್ಯ = f_Dialog_ಮೌಲ್ಯ;

    ಯದಿ (Lock())
	Quit();

    ಪ್ರತ್ಯಾಗಮ ret_ಮೌಲ್ಯ;
}


ಶೂನ್ಯ VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 selIndex = -1;
    int32 ಎಣಿಕೆ = ಎಣಿಕೆ_font_styles(fFontFamily);
    ಪ್ರತಿ (int32 i = 0; i < ಎಣಿಕೆ; i++) {
	font_style style;
	uint32 flags = 0;
	ಯದಿ (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->AddItem(ನೂತನ BStringItem((ಧ್ರುವ ವರ್ಣ*)style));
	    ಯದಿ (strncmp(style, fFontStyle, B_FONT_STYLE_LENGTH) == 0)
		selIndex = i;
	}
    }

    ಯದಿ (selIndex >= 0) {
	fStylesList->Select(selIndex);
	fStylesList->ScrollToSelection();
    } ಅನ್ಯಥಾ
	fStylesList->Select(0);
}


ಶೂನ್ಯ VimSelectFontDialog::Update_Size_ಒಳಹರಿವು_Preview()
{
    ವರ್ಣ buf[10] = {0};
    vim_snprintf(buf, ಪರಿಮಾಣ(buf), (ವರ್ಣ*)"%.0f", fFontSize);
    f_Sizes_ಒಳಹರಿವು->SetText(buf);
}


ಶೂನ್ಯ VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.SetSize(fFontSize);
    font.SetFamilyAndStyle(fFontFamily, fFontStyle);
    fPreview->SetFont(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_SIZE);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (ಪೂರ್ಣಾಙ್ಕ)fFontSize << " pt.";
    fPreview->SetText(str);
}


    ತಾರ್ಕಿಕ
VimSelectFontDialog::_UpdateFromListItem(BListView* list, ವರ್ಣ* text, ಪೂರ್ಣಾಙ್ಕ textSize)
{
    int32 index = list->CurrentSelection();
    ಯದಿ (index < 0)
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    BStringItem* item = (BStringItem*)list->ItemAt(index);
    ಯದಿ (item == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    strncpy(text, item->Text(), textSize);
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}


ಶೂನ್ಯ VimSelectFontDialog::MessageReceived(BMessage *ಸಂದೇಶ)
{
    ವಿಕಲ್ಪ (ಸಂದೇಶ->what) {
	ಪಕ್ಷ k_Vim_Dialog_OKButton_ಸಂದೇಶ:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_LENGTH);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_LENGTH);
	    *fSize = fFontSize;
	    f_Dialog_ಮೌಲ್ಯ = ಸತ್ಯ;
	ಪಕ್ಷ k_Vim_Dialog_Cancel_Button_ಸಂದೇಶ:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    ಪ್ರತ್ಯಾಗಮ;
	ಪಕ್ಷ B_ಕೀಲಿ_UP:
	    {
		int32 ಕೀಲಿ = 0;
		ಯದಿ (ಸಂದೇಶ->FindInt32("raw_char", &ಕೀಲಿ) == B_OK
			&& ಕೀಲಿ == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ k_Vim_Dialog_Family_Select_ಸಂದೇಶ:
	    ಯದಿ (_UpdateFromListItem(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_LENGTH)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ k_Vim_Dialog_Style_Select_ಸಂದೇಶ:
	    ಯದಿ (_UpdateFromListItem(fStylesList,
		    fFontStyle, B_FONT_STYLE_LENGTH))
		_UpdateFontPreview();
	    ಭಙ್ಗ;
	ಪಕ್ಷ k_Vim_Dialog_Size_Select_ಸಂದೇಶ:
	    {
		ವರ್ಣ buf[10] = {0};
		ಯದಿ (_UpdateFromListItem(fSizesList, buf, ಪರಿಮಾಣ(buf))) {
		    ಭಿನ್ನ size = atof(buf);
		    ಯದಿ (size > 0.f) {
			fFontSize = size;
			Update_Size_ಒಳಹರಿವು_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ k_Vim_Dialog_Size_ಒಳಹರಿವು_ಸಂದೇಶ:
	    {
		ಭಿನ್ನ size = atof(f_Sizes_ಒಳಹರಿವು->Text());
		ಯದಿ (size > 0.f) {
		    fFontSize = size;
		    _UpdateFontPreview();
		}
	    }
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ BWindow::MessageReceived(ಸಂದೇಶ);
}

#endif // FEAT_GUI_DIALOG

#ifdef FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
ಸ್ಥಿರ BMessage * MenuMessage(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect frame, ಧ್ರುವ ವರ್ಣ *name) :
    BBox(frame, name, B_FOLLOW_ಎಡ | B_FOLLOW_ತುದಿ, B_WILL_DRAW | B_FRAME_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 ಎಣಿಕೆ = fButtonsList.ಎಣಿಕೆ_ವಸ್ತುಗಳು();
    ಪ್ರತಿ (int32 i = 0; i < ಎಣಿಕೆ; i++)
	delete (BPictureButton*)fButtonsList.ItemAt(i);
    fButtonsList.MakeEmpty();

    delete normalButtonsBitmap;
    delete grayedButtonsBitmap;
    normalButtonsBitmap    = ರಿಕ್ತ;
    grayedButtonsBitmap  = ರಿಕ್ತ;
}

    ಶೂನ್ಯ
VimToolbar::AttachedToWindow()
{
    BBox::AttachedToWindow();

    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    ಭಿನ್ನ
VimToolbar::ToolbarHeight() ಧ್ರುವ
{
    ಭಿನ್ನ size = ರಿಕ್ತ == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
    ಪ್ರತ್ಯಾಗಮ size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    ತಾರ್ಕಿಕ
VimToolbar::ModifyBitmapToGrayed(BBitmap *bitmap)
{
    ಭಿನ್ನ height = bitmap->Bounds().Height();
    ಭಿನ್ನ width  = bitmap->Bounds().Width();

    rgb_color *bits = (rgb_color*)bitmap->Bits();
    int32 pixels = bitmap->BitsLength() / 4;
    ಪ್ರತಿ (int32 i = 0; i < pixels; i++) {
	bits[i].red = bits[i].green =
	bits[i].blue = ((uint32)bits[i].red + bits[i].green + bits[i].blue) / 3;
	bits[i].alpha /= 4;
    }

    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

    ತಾರ್ಕಿಕ
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    normalButtonsBitmap = LoadVimBitmap("builtin-tools.png");
    ಯದಿ (normalButtonsBitmap == ರಿಕ್ತ)
	//  customized not found? dig application resources for "builtin-tools" one
	normalButtonsBitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    ಯದಿ (normalButtonsBitmap == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

    BMessage archive;
    normalButtonsBitmap->Archive(&archive);

    grayedButtonsBitmap = ನೂತನ BBitmap(&archive);
    ಯದಿ (grayedButtonsBitmap == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

    //	modify grayed bitmap
    ModifyBitmapToGrayed(grayedButtonsBitmap);

    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

BBitmap *VimToolbar::LoadVimBitmap(ಧ್ರುವ ವರ್ಣ* fileName)
{
    BBitmap *bitmap = ರಿಕ್ತ;

    ಪೂರ್ಣಾಙ್ಕ mustfree = 0;
    char_u* runtimePath = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    ಯದಿ (runtimePath != ರಿಕ್ತ && fileName != ರಿಕ್ತ) {
	BString strPath((ವರ್ಣ*)runtimePath);
	strPath << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(strPath.String());
    }

    ಯದಿ (mustfree)
	vim_free(runtimePath);

    ಪ್ರತ್ಯಾಗಮ bitmap;
}

    ತಾರ್ಕಿಕ
VimToolbar::GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, ತಾರ್ಕಿಕ pressed)
{
    ಭಿನ್ನ size = bitmapFrom->Bounds().Height() + 1.;

    BView view(BRect(0, 0, size, size), "", 0, 0);

    AddChild(&view);
    view.BeginPicture(pictureTo);

    view.Set_ಮೇಲಿನ_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.SetDrawingMode(B_OP_OVER);

    BRect source(0, 0, size - 1, size - 1);
    BRect destination(source);

    source.OffsetBy(size * index, 0);
    destination.OffsetBy(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, source, destination);

    ಯದಿ (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	size += ButtonMargin * 2 - 1;
	view.Begin_ಸಾಲು_Array(4);
	view.Add_ಸಾಲು(BPoint(0, 0),	 BPoint(size, 0),    shadowColor);
	view.Add_ಸಾಲು(BPoint(size, 0),	 BPoint(size, size), shineColor);
	view.Add_ಸಾಲು(BPoint(size, size), BPoint(0, size),    shineColor);
	view.Add_ಸಾಲು(BPoint(0, size),	 BPoint(0, 0),	     shadowColor);
	view.End_ಸಾಲು_Array();
    }

    view.EndPicture();
    RemoveChild(&view);

    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

    ತಾರ್ಕಿಕ
VimToolbar::AddButton(int32 index, vimmenu_T *menu)
{
    BPictureButton *button = ರಿಕ್ತ;
    ಯದಿ (!menu_is_separator(menu->name)) {
	ಭಿನ್ನ size = normalButtonsBitmap ?
	    normalButtonsBitmap->Bounds().Height() + 1. + ButtonMargin * 2 : 18.;
	BRect frame(0, 0, size, size);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture pictureGray;

	ಯದಿ (menu->iconfile == ರಿಕ್ತ && menu->iconidx >= 0 && normalButtonsBitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, normalButtonsBitmap, ಸತ್ಯ);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, normalButtonsBitmap, ಅಸತ್ಯ);
	    GetPictureFromBitmap(&pictureGray, menu->iconidx, grayedButtonsBitmap, ಅಸತ್ಯ);
	} ಅನ್ಯಥಾ {

	    char_u buffer[MAXPATHL] = {0};
	    BBitmap *bitmap = ರಿಕ್ತ;

	    ಯದಿ (menu->iconfile) {
		gui_find_iconfile(menu->iconfile, buffer, (ವರ್ಣ*)"png");
		bitmap = BTranslationUtils::GetBitmap((ವರ್ಣ*)buffer);
	    }

	    ಯದಿ (bitmap == ರಿಕ್ತ && gui_find_bitmap(menu->name, buffer, (ವರ್ಣ*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((ವರ್ಣ*)buffer);

	    ಯದಿ (bitmap == ರಿಕ್ತ)
		bitmap = ನೂತನ BBitmap(BRect(0, 0, size, size), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, ಸತ್ಯ);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, ಅಸತ್ಯ);
	    ModifyBitmapToGrayed(bitmap);
	    GetPictureFromBitmap(&pictureGray, 0, bitmap, ಅಸತ್ಯ);

	    delete bitmap;
	}

	button = ನೂತನ BPictureButton(frame, (ವರ್ಣ*)menu->name,
		    &pictureOff, &pictureOn, MenuMessage(menu));

	button->SetDisabledOn(&pictureGray);
	button->SetDisabledOff(&pictureGray);

	button->SetTarget(gui.vimTextArea);

	AddChild(button);

	menu->button = button;
    }

    ತಾರ್ಕಿಕ result = fButtonsList.AddItem(button, index);
    InvalidateLayout();
    ಪ್ರತ್ಯಾಗಮ result;
}

    ತಾರ್ಕಿಕ
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    ಯದಿ (menu->button) {
	ಯದಿ (fButtonsList.RemoveItem(menu->button)) {
	    delete menu->button;
	    menu->button = ರಿಕ್ತ;
	}
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

    ತಾರ್ಕಿಕ
VimToolbar::GrayButton(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ grey)
{
    ಯದಿ (menu->button) {
	int32 index = fButtonsList.IndexOf(menu->button);
	ಯದಿ (index >= 0)
	    menu->button->SetEnabled(grey ? ಅಸತ್ಯ : ಸತ್ಯ);
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

    ಶೂನ್ಯ
VimToolbar::InvalidateLayout()
{
    int32 offset = ToolbarMargin;
    int32 ಎಣಿಕೆ = fButtonsList.ಎಣಿಕೆ_ವಸ್ತುಗಳು();
    ಪ್ರತಿ (int32 i = 0; i < ಎಣಿಕೆ; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.ItemAt(i);
	ಯದಿ (button) {
	    button->MoveTo(offset, ToolbarMargin);
	    offset += button->Bounds().Width() + ToolbarMargin;
	} ಅನ್ಯಥಾ
	    offset += ToolbarMargin * 3;
    }
}

#endif /*FEAT_TOOLBAR*/

#ಯದಿ defined(FEAT_GUI_TABLINE)

    ಭಿನ್ನ
Vim_Tab_ಸಾಲು::TablineHeight() ಧ್ರುವ
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    ಪ್ರತ್ಯಾಗಮ TabHeight(); //  + ToolbarMargin;
}

ಶೂನ್ಯ
Vim_Tab_ಸಾಲು::MouseDown(BPoint point)
{
    ಯದಿ (!gui_mch_showing_tabline())
	ಪ್ರತ್ಯಾಗಮ;

    BMessage *m = Window()->CurrentMessage();
    ಪ್ರತಿಜ್ಞಾ(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    int32 clicks = 0;
    m->FindInt32("clicks", &clicks);

    ಪೂರ್ಣಾಙ್ಕ index = 0; //  0 means here - no tab found
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಎಣಿಕೆ_Tabs(); i++) {
	ಯದಿ (TabFrame(i).Contains(point)) {
	    index = i + 1; //  indexes are 1-based
	    ಭಙ್ಗ;
	}
    }

    ಪೂರ್ಣಾಙ್ಕ event = -1;

    ಯದಿ ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    ಅನ್ಯಥಾ ಯದಿ (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = index > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    ಅನ್ಯಥಾ ಯದಿ (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPopUpMenu* ತೆಗೆ_Up_Menu = ನೂತನ BPopUpMenu("tabLineContextMenu", ಅಸತ್ಯ, ಅಸತ್ಯ);
	ತೆಗೆ_Up_Menu->AddItem(ನೂತನ BMenuItem(_("Close tabi R"), ನೂತನ BMessage(TABLINE_MENU_CLOSE)));
	ತೆಗೆ_Up_Menu->AddItem(ನೂತನ BMenuItem(_("New tab    T"), ನೂತನ BMessage(TABLINE_MENU_NEW)));
	ತೆಗೆ_Up_Menu->AddItem(ನೂತನ BMenuItem(_("Open tab..."), ನೂತನ BMessage(TABLINE_MENU_OPEN)));

	ConvertToScreen(&point);
	BMenuItem* item = ತೆಗೆ_Up_Menu->Go(point);
	ಯದಿ (item != ರಿಕ್ತ) {
	    event = item->Command();
	}

	delete ತೆಗೆ_Up_Menu;

    } ಅನ್ಯಥಾ {
	//  default processing
	BTabView::MouseDown(point);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (event < 0)
	ಪ್ರತ್ಯಾಗಮ;

    Vim_Tabline_Menu_ಸಂದೇಶ tmm;
    tmm.index = index;
    tmm.event = event;
    write_port(gui.vdcmp, Vim_ಸಂದೇಶ::TablineMenu, &tmm, ಪರಿಮಾಣ(tmm));
}

ಶೂನ್ಯ
Vim_Tab_ಸಾಲು::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_ಸಾಲು *tab_ಸಾಲು = gui.vimForm->Tab_ಸಾಲು();
    ಯದಿ (tab_ಸಾಲು != ರಿಕ್ತ) {

	int32 i = 0;
	ಪ್ರತಿ (; i < tab_ಸಾಲು->ಎಣಿಕೆ_Tabs(); i++)
	    ಯದಿ (ಏತತ್ == tab_ಸಾಲು->TabAt(i))
		ಭಙ್ಗ;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	ಯದಿ (i < tab_ಸಾಲು->ಎಣಿಕೆ_Tabs()) {
	    Vim_Tabline_ಸಂದೇಶ tm;
	    tm.index = i + 1;
	    write_port(gui.vdcmp, Vim_ಸಂದೇಶ::Tabline, &tm, ಪರಿಮಾಣ(tm));
	}
    }
}

#endif //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
ಸ್ಥಿರ ವರ್ಣ appsig[] = "application/x-vnd.Haiku-Vim-8";
ಕೀಲಿ_map *ಕೀಲಿ_Map;
ವರ್ಣ *ಕೀಲಿ_Map_Chars;
ಪೂರ್ಣಾಙ್ಕ main_exitcode = 127;

    status_t
gui_haiku_process_event(bigtime_t timeout)
{
    ಸಂರಚನಾ Vim_ಸಂದೇಶ vm;
    int32 what;
    ssize_t size;

    size = read_port_etc(gui.vdcmp, &what, &vm, ಪರಿಮಾಣ(vm),
	    B_TIMEOUT, timeout);

    ಯದಿ (size >= 0) {
	ವಿಕಲ್ಪ (what) {
	    ಪಕ್ಷ Vim_ಸಂದೇಶ::ಕೀಲಿ_2:
		{
		    char_u *string = vm.u.ಕೀಲಿ_2.chars;
		    ಪೂರ್ಣಾಙ್ಕ len = vm.u.ಕೀಲಿ_2.length;
		    ಯದಿ (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_ಒಳಹರಿವು_buf();
			got_int = TRUE;
		    }

		    ಯದಿ (vm.u.ಕೀಲಿ_2.csi_escape)
#ifndef FEAT_MBYTE_IME
		    {
			ಪೂರ್ಣಾಙ್ಕ	i;
			char_u	buf[2];

			ಪ್ರತಿ (i = 0; i < len; ++i)
			{
			    add_to_ಒಳಹರಿವು_buf(string + i, 1);
			    ಯದಿ (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_CSI;
				add_to_ಒಳಹರಿವು_buf(buf, 2);
			    }
			}
		    }
#ಅನ್ಯಥಾ
			add_to_ಒಳಹರಿವು_buf_csi(string, len);
#endif
		    ಅನ್ಯಥಾ
			add_to_ಒಳಹರಿವು_buf(string, len);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ Vim_ಸಂದೇಶ::Resize:
		gui_resize_shell(vm.u.NewSize.width, vm.u.NewSize.height);
		ಭಙ್ಗ;
	    ಪಕ್ಷ Vim_ಸಂದೇಶ::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_ಎಣಿಕೆ =
			atomic_add(&vm.u.Scroll.sb->scroll_Event_ಎಣಿಕೆ, -1);
		    ಯದಿ (old_ಎಣಿಕೆ <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.ಮೌಲ್ಯ, vm.u.Scroll.stillDragging);
		}
		ಭಙ್ಗ;
#ಯದಿ defined(FEAT_MENU)
	    ಪಕ್ಷ Vim_ಸಂದೇಶ::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		ಭಙ್ಗ;
#endif
	    ಪಕ್ಷ Vim_ಸಂದೇಶ::Mouse:
		{
		    int32 old_ಎಣಿಕೆ;
		    ಯದಿ (vm.u.Mouse.button == MOUSE_DRAG)
			old_ಎಣಿಕೆ =
			    atomic_add(&gui.vimTextArea->mouse_Drag_Event_ಎಣಿಕೆ, -1);
		    ಅನ್ಯಥಾ
			old_ಎಣಿಕೆ = 0;
		    ಯದಿ (old_ಎಣಿಕೆ <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ Vim_ಸಂದೇಶ::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ Vim_ಸಂದೇಶ::Focus:
		gui.in_focus = vm.u.Focus.active;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		ಯದಿ (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		ಭಙ್ಗ;
	    ಪಕ್ಷ Vim_ಸಂದೇಶ::Refs:
		::RefsReceived(vm.u.Refs.message, vm.u.Refs.changedir);
		ಭಙ್ಗ;
	    ಪಕ್ಷ Vim_ಸಂದೇಶ::Tabline:
		send_tabline_event(vm.u.Tabline.index);
		ಭಙ್ಗ;
	    ಪಕ್ಷ Vim_ಸಂದೇಶ::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.index, vm.u.TablineMenu.event);
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		//  unrecognised message, ignore it
		ಭಙ್ಗ;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    ಪ್ರತ್ಯಾಗಮ size;
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

    ಪೂರ್ಣಾಙ್ಕ
vim_lock_screen()
{
    ಪ್ರತ್ಯಾಗಮ !gui.vimWindow || gui.vimWindow->Lock();
}

    ಶೂನ್ಯ
vim_unlock_screen()
{
    ಯದಿ (gui.vimWindow)
	gui.vimWindow->Unlock();
}

#define RUN_BAPPLICATION_IN_NEW_THREAD	0

#ಯದಿ RUN_BAPPLICATION_IN_NEW_THREAD

    int32
run_vimapp(ಶೂನ್ಯ *args)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.Run();		    // Run until Quit() called

    ಪ್ರತ್ಯಾಗಮ 0;
}

#ಅನ್ಯಥಾ

    int32
call_main(ಶೂನ್ಯ *args)
{
    ಸಂರಚನಾ MainArgs *ma = (MainArgs *)args;

    ಪ್ರತ್ಯಾಗಮ main(ma->argc, ma->argv);
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    ಶೂನ್ಯ
gui_mch_prepare(
	ಪೂರ್ಣಾಙ್ಕ	*argc,
	ವರ್ಣ	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    ಯದಿ (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(find_thread(ರಿಕ್ತ), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_port(B_MAX_PORT_ಎಣಿಕೆ, "vim VDCMP");

#ಯದಿ RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = spawn_thread(run_vimapp, "vim VimApp",
		tinfo.priority, ರಿಕ್ತ);
	ಯದಿ (tid >= B_OK) {
	    resume_thread(tid);
	} ಅನ್ಯಥಾ {
	    getout(1);
	}
#ಅನ್ಯಥಾ
	MainArgs ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.priority, &ma);
	ಯದಿ (tid >= B_OK) {
	    VimApp app(appsig);

	    gui.vimApp = &app;
	    resume_thread(tid);
	    /*
	     * This is rather horrible.
	     * call_main will call main() again...
	     * There will be no infinite recursion since
	     * gui.vimApp is set now.
	     */
	    app.Run();		    // Run until Quit() called
	    // fprintf(stderr, "app.Run() returned...\n");
	    status_t dummy_exitcode;
	    (ಶೂನ್ಯ)wait_for_thread(tid, &dummy_exitcode);

	    /*
	     * This path should be the normal one taken to exit Vim.
	     * The main() thread calls mch_exit() which calls
	     * gui_mch_exit() which terminates its thread.
	     */
	    exit(main_exitcode);
	}
#endif
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
    ಯದಿ (!isatty(0)) {
	ಸಂರಚನಾ stat stat_stdin, stat_dev_null;

	ಯದಿ (fstat(0, &stat_stdin) == -1 ||
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
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_init_check(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_back_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    gui.scrollbar_width = (ಪೂರ್ಣಾಙ್ಕ) B_V_SCROLL_BAR_WIDTH;
    gui.scrollbar_height = (ಪೂರ್ಣಾಙ್ಕ) B_H_SCROLL_BAR_HEIGHT;
#ifdef FEAT_MENU
    gui.menu_height = 19;   //	initial guess -
    //	correct for my default settings
#endif
    gui.border_offset = 3;  //	coordinates are inside window borders

    ಯದಿ (gui.vdcmp < B_OK)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    get_ಕೀಲಿ_map(&ಕೀಲಿ_Map, &ಕೀಲಿ_Map_Chars);

    gui.vimWindow = ನೂತನ VimWindow();	// hidden and locked
    ಯದಿ (!gui.vimWindow)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    gui.vimWindow->Run();	// Run() unlocks but does not show

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    set_normal_colors();

    /*
     * Check that none of the colors are the same as the background color
     */
    gui_check_colors();

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();	    // re-init colors and fonts

    gui_mch_new_colors();	// window must exist for this

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    ಶೂನ್ಯ
gui_mch_new_colors()
{
    rgb_color rgb = GUI_TO_RGB(gui.back_pixel);

    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimForm->SetViewColor(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vimWindow->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_open()
{
    ಯದಿ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimWindow->Show();
	gui.vimWindow->Unlock();
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಶೂನ್ಯ
gui_mch_exit(ಪೂರ್ಣಾಙ್ಕ vim_exitcode)
{
    ಯದಿ (gui.vimWindow) {
	thread_id tid = gui.vimWindow->Thread();
	gui.vimWindow->Lock();
	gui.vimWindow->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_port(gui.vdcmp);
#ಯದಿ !RUN_BAPPLICATION_IN_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#endif
    ಯದಿ (gui.vimApp) {
	VimTextAreaView::guiBlankMouse(ಅಸತ್ಯ);

	main_exitcode = vim_exitcode;
#ಯದಿ RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->Lock();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#ಅನ್ಯಥಾ
	gui.vimApp->Lock();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	// suicide
	exit_thread(vim_exitcode);
#endif
    }
    // If we are somehow still here, let mch_exit() handle things.
}

/*
 * Get the position of the top left corner of the window.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_winpos(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y)
{
    ಯದಿ (gui.vimWindow->Lock()) {
	BRect r;
	r = gui.vimWindow->Frame();
	gui.vimWindow->Unlock();
	*x = (ಪೂರ್ಣಾಙ್ಕ)r.ಎಡ;
	*y = (ಪೂರ್ಣಾಙ್ಕ)r.ತುದಿ;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ಶೂನ್ಯ
gui_mch_set_winpos(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimWindow->MoveTo(x, y);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
ಶೂನ್ಯ
gui_mch_set_shellsize(
	ಪೂರ್ಣಾಙ್ಕ	width,
	ಪೂರ್ಣಾಙ್ಕ	height,
	ಪೂರ್ಣಾಙ್ಕ	min_width,
	ಪೂರ್ಣಾಙ್ಕ	min_height,
	ಪೂರ್ಣಾಙ್ಕ	base_width,
	ಪೂರ್ಣಾಙ್ಕ	base_height,
	ಪೂರ್ಣಾಙ್ಕ	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimWindow->ResizeTo(width - PEN_WIDTH, height - PEN_WIDTH);

	// set size limits
	ಭಿನ್ನ minWidth, maxWidth, minHeight, maxHeight;

	gui.vimWindow->GetSizeLimits(&minWidth, &maxWidth,
		&minHeight, &maxHeight);
	gui.vimWindow->SetSizeLimits(min_width, maxWidth,
		min_height, maxHeight);

	/*
	 * Set the resizing alignment depending on font size.
	 */
	gui.vimWindow->SetWindowAlignment(
		B_PIXEL_ALIGNMENT,	//  window_alignment mode,
		1,		//  int32 h,
		0,		//  int32 hOffset = 0,
		gui.char_width,	    //	int32 width = 0,
		base_width,	    //	int32 widthOffset = 0,
		1,		//  int32 v = 0,
		0,		//  int32 vOffset = 0,
		gui.char_height,	//  int32 height = 0,
		base_height	    //	int32 heightOffset = 0
		);

	gui.vimWindow->Unlock();
    }
}

ಶೂನ್ಯ
gui_mch_get_screen_dimensions(
	ಪೂರ್ಣಾಙ್ಕ	*screen_w,
	ಪೂರ್ಣಾಙ್ಕ	*screen_h)
{
    BRect frame;

    {
	BScreen screen(gui.vimWindow);

	ಯದಿ (screen.IsValid()) {
	    frame = screen.Frame();
	} ಅನ್ಯಥಾ {
	    frame.ಬಲ = 640;
	    frame.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (ಪೂರ್ಣಾಙ್ಕ) frame.ಬಲ - 2 * gui.scrollbar_width - 20;
    *screen_h = (ಪೂರ್ಣಾಙ್ಕ) frame.bottom - gui.scrollbar_height
#ifdef FEAT_MENU
	- gui.menu_height
#endif
	- 30;
}

ಶೂನ್ಯ
gui_mch_set_text_area_pos(
	ಪೂರ್ಣಾಙ್ಕ	x,
	ಪೂರ್ಣಾಙ್ಕ	y,
	ಪೂರ್ಣಾಙ್ಕ	w,
	ಪೂರ್ಣಾಙ್ಕ	h)
{
    ಯದಿ (!gui.vimTextArea)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimTextArea->MoveTo(x, y);
	gui.vimTextArea->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);

#ifdef FEAT_GUI_TABLINE
	ಯದಿ (gui.vimForm->Tab_ಸಾಲು() != ರಿಕ್ತ) {
	    gui.vimForm->Tab_ಸಾಲು()->ResizeTo(w, gui.vimForm->TablineHeight());
	}
#endif // FEAT_GUI_TABLINE

	gui.vimWindow->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

ಶೂನ್ಯ
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	ಪೂರ್ಣಾಙ್ಕ	flag)
{
    VimScrollBar *vsb = sb->id;
    ಯದಿ (gui.vimWindow->Lock()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	ಯದಿ (flag) {
	    ಯದಿ (vsb->IsHidden()) {
		vsb->Show();
	    }
	} ಅನ್ಯಥಾ {
	    ಯದಿ (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vimWindow->Unlock();
    }
}

ಶೂನ್ಯ
gui_mch_set_scrollbar_thumb(
	scrollbar_T *sb,
	ಪೂರ್ಣಾಙ್ಕ	val,
	ಪೂರ್ಣಾಙ್ಕ	size,
	ಪೂರ್ಣಾಙ್ಕ	max)
{
    ಯದಿ (gui.vimWindow->Lock()) {
	VimScrollBar *s = sb->id;
	ಯದಿ (max == 0) {
	    s->Set_ಮೌಲ್ಯ(0);
	    s->SetRange(0.0, 0.0);
	} ಅನ್ಯಥಾ {
	    s->SetProportion((ಭಿನ್ನ)size / (max + 1.0));
	    s->SetSteps(1.0, size > 5 ? size - 2 : size);
#ifndef SCROLL_PAST_END	    //	really only defined in gui.c...
	    max = max + 1 - size;
#endif
	    ಯದಿ (max < s->ಮೌಲ್ಯ_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->Set_ಮೌಲ್ಯ(val);
		s->SetRange(0.0, max);
	    } ಅನ್ಯಥಾ {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->SetRange(0.0, max);
		s->Set_ಮೌಲ್ಯ(val);
	    }
	}
	gui.vimWindow->Unlock();
    }
}

ಶೂನ್ಯ
gui_mch_set_scrollbar_pos(
	scrollbar_T *sb,
	ಪೂರ್ಣಾಙ್ಕ	x,
	ಪೂರ್ಣಾಙ್ಕ	y,
	ಪೂರ್ಣಾಙ್ಕ	w,
	ಪೂರ್ಣಾಙ್ಕ	h)
{
    ಯದಿ (gui.vimWindow->Lock()) {
	BRect winb = gui.vimWindow->Bounds();
	ಭಿನ್ನ vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);
	ಯದಿ (winb.ಬಲ-(x+w)<w) vsbx = winb.ಬಲ - (w - PEN_WIDTH);
	vsb->MoveTo(vsbx, vsby);
	gui.vimWindow->Unlock();
    }
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_scrollbar_xpadding(ಶೂನ್ಯ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_scrollbar_ypadding(ಶೂನ್ಯ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	ಪೂರ್ಣಾಙ್ಕ	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = ನೂತನ VimScrollBar(sb, posture);
    ಯದಿ (gui.vimWindow->Lock()) {
	vsb->SetTarget(gui.vimTextArea);
	vsb->Hide();
	gui.vimForm->AddChild(vsb);
	gui.vimWindow->Unlock();
    }
}

#ಯದಿ defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
ಶೂನ್ಯ
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    ಯದಿ (gui.vimWindow->Lock()) {
	sb->id->RemoveSelf();
	delete sb->id;
	gui.vimWindow->Unlock();
    }
}
#endif

/*
 * Cursor does not flash
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_is_blink_off(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON blinking, cursor is shown
 */

#define BLINK_NONE  0
#define BLINK_OFF   1
#define BLINK_ON    2

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	blink_state = BLINK_NONE;
ಸ್ಥಿರ long_u	    blink_waittime = 700;
ಸ್ಥಿರ long_u	    blink_ontime = 400;
ಸ್ಥಿರ long_u	    blink_offtime = 250;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  blink_timer = 0;

ಶೂನ್ಯ
gui_mch_set_blinking(
	long	waittime,
	long	on,
	long	off)
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
gui_mch_stop_blink(ಪೂರ್ಣಾಙ್ಕ may_call_gui_update_cursor)
{
    // TODO
    ಯದಿ (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    ಯದಿ (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    ಶೂನ್ಯ
gui_mch_start_blink()
{
    // TODO
    ಯದಿ (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    ಯದಿ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
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
ಪೂರ್ಣಾಙ್ಕ
gui_mch_init_font(
	char_u	    *font_name,
	ಪೂರ್ಣಾಙ್ಕ	    fontset)
{
    ಯದಿ (gui.vimWindow->Lock())
    {
	ಪೂರ್ಣಾಙ್ಕ rc = gui.vimTextArea->mchInitFont(font_name);
	gui.vimWindow->Unlock();

	ಪ್ರತ್ಯಾಗಮ rc;
    }

    ಪ್ರತ್ಯಾಗಮ FAIL;
}


    ಪೂರ್ಣಾಙ್ಕ
gui_mch_adjust_charsize()
{
    ಪ್ರತ್ಯಾಗಮ FAIL;
}


    ಪೂರ್ಣಾಙ್ಕ
gui_mch_font_dialog(font_family* family, font_style* style, ಭಿನ್ನ* size)
{
#ಯದಿ defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = ನೂತನ VimSelectFontDialog(family, style, size);
    ತಾರ್ಕಿಕ ret = dialog->Go();
	delete dialog;
	ಪ್ರತ್ಯಾಗಮ ret;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ NOFONT;
#endif // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	ಪೂರ್ಣಾಙ್ಕ	    giveErrorIfMissing)
{
    ಸ್ಥಿರ VimFont *fontList = ರಿಕ್ತ;

    ಯದಿ (!gui.in_use)	//  can't do this when GUI not running
	ಪ್ರತ್ಯಾಗಮ NOFONT;

    //	storage for locally modified name;
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ buff_size = B_FONT_FAMILY_LENGTH + B_FONT_STYLE_LENGTH + 20;
    ಸ್ಥಿರ ವರ್ಣ font_name[buff_size] = {0};
    font_family family = {0};
    font_style	style  = {0};
    ಭಿನ್ನ size = 0.f;

    ಯದಿ (name == 0 && be_fixed_font == 0) {
	ಯದಿ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	ಪ್ರತ್ಯಾಗಮ NOFONT;
    }

    ತಾರ್ಕಿಕ useSelectGUI = ಅಸತ್ಯ;
    ಯದಿ (name != ರಿಕ್ತ)
	ಯದಿ (STRCMP(name, "*") == 0) {
	    useSelectGUI = ಸತ್ಯ;
	    STRNCPY(font_name, hl_get_font_name(), buff_size);
	} ಅನ್ಯಥಾ
	    STRNCPY(font_name, name, buff_size);

    ಯದಿ (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	size = be_fixed_font->Size();
	vim_snprintf(font_name, buff_size,
	    (ವರ್ಣ*)"%s/%s/%.0f", family, style, size);
    }

    //	replace underscores with spaces
    ವರ್ಣ* end = 0;
    ಯಾವತ್ (end = strchr((ವರ್ಣ *)font_name, '_'))
	*end = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    ಸ್ಥಿರ ವರ್ಣ buff[buff_size] = {0};
    STRNCPY(buff, font_name, buff_size);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_LENGTH);
    ವರ್ಣ* style_s = strtok(0, "/\0");
    ಯದಿ (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_LENGTH);
    size = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    ಯದಿ (useSelectGUI) {
	ಯದಿ (gui_mch_font_dialog(&family, &style, &size) == NOFONT)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_size,
		(ವರ್ಣ*)"%s/%s/%.0f", family, style, size);
	hl_set_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	ಯದಿ (new_p_guifont != ರಿಕ್ತ) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    ಪ್ರತಿ ( ; *new_p_guifont; ++new_p_guifont)
		ಯದಿ (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    ಪ್ರತಿ (flp = fontList; flp; flp = flp->next) {
	ಯದಿ (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    ಪ್ರತ್ಯಾಗಮ (GuiFont)flp;
	}
    }

    VimFont *font = ನೂತನ VimFont();
    font->name = vim_strsave((char_u*)font_name);

    ಯದಿ (ಎಣಿಕೆ_font_styles(family) <= 0) {
	ಯದಿ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	delete font;
	ಪ್ರತ್ಯಾಗಮ NOFONT;
    }

    //	Remember font in the static list for later use
    font->next = fontList;
    fontList = font;

    font->SetFamilyAndStyle(family, style);
    ಯದಿ (size > 0.f)
	font->SetSize(size);

    font->SetSpacing(B_FIXED_SPACING);
    font->SetEncoding(B_UNICODE_UTF8);

    ಪ್ರತ್ಯಾಗಮ (GuiFont)font;
}

/*
 * Set the current text font.
 */
ಶೂನ್ಯ
gui_mch_set_font(
	GuiFont	font)
{
    ಯದಿ (gui.vimWindow->Lock()) {
	VimFont *vf = (VimFont *)font;

	gui.vimTextArea->SetFont(vf);

	gui.char_width = (ಪೂರ್ಣಾಙ್ಕ) vf->StringWidth("n");
	font_height fh;
	vf->GetHeight(&fh);
	gui.char_height = (ಪೂರ್ಣಾಙ್ಕ)(fh.ascent + 0.9999)
	    + (ಪೂರ್ಣಾಙ್ಕ)(fh.descent + 0.9999) + (ಪೂರ್ಣಾಙ್ಕ)(fh.leading + 0.9999);
	gui.char_ascent = (ಪೂರ್ಣಾಙ್ಕ)(fh.ascent + 0.9999);

	gui.vimWindow->Unlock();
    }
}

// XXX TODO This is apparently never called...
ಶೂನ್ಯ
gui_mch_free_font(
	GuiFont	font)
{
    ಯದಿ (font == NOFONT)
	ಪ್ರತ್ಯಾಗಮ;
    VimFont *f = (VimFont *)font;
    ಯದಿ (--f->refcount <= 0) {
	ಯದಿ (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	delete f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ಪೂರ್ಣಾಙ್ಕ
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
	font_height fh = {0};
	font->GetHeight(&fh);
	gui.char_height = (ಪೂರ್ಣಾಙ್ಕ)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (ಪೂರ್ಣಾಙ್ಕ)(fh.ascent + 0.5) + p_linespace / 2;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
gui_mch_getmouse(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y)
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
gui_mch_mousehide(ಪೂರ್ಣಾಙ್ಕ hide)
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
    ಪ್ರತ್ಯಾಗಮ gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
ಶೂನ್ಯ
gui_mch_set_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_ಮೇಲಿನ_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text background color.
 */
ಶೂನ್ಯ
gui_mch_set_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_ಕೆಳಗಿನ_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    ಶೂನ್ಯ
gui_mch_set_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

ಶೂನ್ಯ
gui_mch_draw_string(
	ಪೂರ್ಣಾಙ್ಕ	row,
	ಪೂರ್ಣಾಙ್ಕ	col,
	char_u	*s,
	ಪೂರ್ಣಾಙ್ಕ	len,
	ಪೂರ್ಣಾಙ್ಕ	flags)
{
    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchDrawString(row, col, s, len, flags);
	gui.vimWindow->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(ಪೂರ್ಣಾಙ್ಕ r, ಪೂರ್ಣಾಙ್ಕ g, ಪೂರ್ಣಾಙ್ಕ b)
{
    ಪ್ರತ್ಯಾಗಮ gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
ಪೂರ್ಣಾಙ್ಕ
gui_mch_haskey(
	char_u	*name)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; special_ಕೀಲಿಗಳು[i].Be_ಕೀಲಿಗಳು != 0; i++)
	ಯದಿ (name[0] == special_ಕೀಲಿಗಳು[i].vim_code0 &&
		name[1] == special_ಕೀಲಿಗಳು[i].vim_code1)
	    ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಶೂನ್ಯ
gui_mch_beep()
{
    ::beep();
}

    ಶೂನ್ಯ
gui_mch_flash(ಪೂರ್ಣಾಙ್ಕ msec)
{
    // Do a visual beep by reversing the foreground and background colors

    ಯದಿ (gui.vimWindow->Lock()) {
	BRect rect = gui.vimTextArea->Bounds();

	gui.vimTextArea->SetDrawingMode(B_OP_INVERT);
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->Sync();
	snooze(msec * 1000);	 // wait for a few msec
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->SetDrawingMode(B_OP_COPY);
	gui.vimTextArea->Flush();
	gui.vimWindow->Unlock();
    }
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
ಶೂನ್ಯ
gui_mch_invert_rectangle(
	ಪೂರ್ಣಾಙ್ಕ	r,
	ಪೂರ್ಣಾಙ್ಕ	c,
	ಪೂರ್ಣಾಙ್ಕ	nr,
	ಪೂರ್ಣಾಙ್ಕ	nc)
{
    BRect rect;
    rect.ಎಡ = FILL_X(c);
    rect.ತುದಿ = FILL_Y(r);
    rect.ಬಲ = rect.ಎಡ + nc * gui.char_width - PEN_WIDTH;
    rect.bottom = rect.ತುದಿ + nr * gui.char_height - PEN_WIDTH;

    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimTextArea->SetDrawingMode(B_OP_INVERT);
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->SetDrawingMode(B_OP_COPY);
	gui.vimWindow->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    ಶೂನ್ಯ
gui_mch_iconify()
{
    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimWindow->Minimize(ಸತ್ಯ);
	gui.vimWindow->Unlock();
    }
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ಶೂನ್ಯ
gui_mch_set_foreground(ಶೂನ್ಯ)
{
    // TODO
}
#endif

/*
 * Set the window title
 */
ಶೂನ್ಯ
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimWindow->SetTitle((ವರ್ಣ *)title);
	gui.vimWindow->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    ಶೂನ್ಯ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.ಎಡ = FILL_X(gui.col);
    r.ತುದಿ = FILL_Y(gui.row);
    ಪೂರ್ಣಾಙ್ಕ cells = utf_off2cells(ಸಾಲು_Offset[gui.row] + gui.col, 100); // TODO-TODO
    ಯದಿ (cells>=4) cells = 1;
    r.ಬಲ = r.ಎಡ + cells*gui.char_width - PEN_WIDTH;
    r.bottom = r.ತುದಿ + gui.char_height - PEN_WIDTH;

    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimTextArea->StrokeRect(r);
	gui.vimWindow->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
ಶೂನ್ಯ
gui_mch_draw_part_cursor(
	ಪೂರ್ಣಾಙ್ಕ	w,
	ಪೂರ್ಣಾಙ್ಕ	h,
	guicolor_T  color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.ಎಡ =
#ifdef FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_ಬಲ ? FILL_X(gui.col + 1) - w :
#endif
	FILL_X(gui.col);
    r.ಬಲ = r.ಎಡ + w - PEN_WIDTH;
    r.bottom = FILL_Y(gui.row + 1) - PEN_WIDTH;
    r.ತುದಿ = r.bottom - h + PEN_WIDTH;

    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimTextArea->FillRect(r);
	gui.vimWindow->Unlock();
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
    ಯಾವತ್ (port_ಎಣಿಕೆ(gui.vdcmp) > 0 &&
	    !vim_is_ಒಳಹರಿವು_buf_full() &&
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
ಪೂರ್ಣಾಙ್ಕ
gui_mch_wait_for_chars(
	long	wtime)
{
    ಪೂರ್ಣಾಙ್ಕ		focus;
    bigtime_t	until, timeout;
    status_t	st;

    ಯದಿ (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_time() + timeout;
    }
    ಅನ್ಯಥಾ
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.in_focus;
    ಪ್ರತಿ (;;)
    {
	// Stop or start blinking when focus changes
	ಯದಿ (gui.in_focus != focus)
	{
	    ಯದಿ (gui.in_focus)
		gui_mch_start_blink();
	    ಅನ್ಯಥಾ
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

	gui_mch_flush();

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ಯದಿ (did_add_timer)
	    // Need to recompute the waiting time.
	    ಭಙ್ಗ;
# endif
# ifdef FEAT_JOB_CHANNEL
	ಯದಿ (has_any_channel())
	{
	    ಯದಿ (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	ಅನ್ಯಥಾ ಯದಿ (wtime < 0)
	    timeout = B_INFINITE_TIMEOUT;
# endif
#endif

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use gui_haiku_process_event() to hang until
	 * an event arrives.  No need to check for input_buf_full because we
	 * are returning as soon as it contains a single char.
	 */
	st = gui_haiku_process_event(timeout);

	ಯದಿ (ಒಳಹರಿವು_available())
	    ಪ್ರತ್ಯಾಗಮ OK;
	ಯದಿ (st < B_OK)		// includes B_TIMED_OUT
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	ಯದಿ (wtime >= 0)
	{
	    timeout = until - system_time();
	    ಯದಿ (timeout < 0)
		ಭಙ್ಗ;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;

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
    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimWindow->Flush();
	gui.vimWindow->Unlock();
    }
    ಪ್ರತ್ಯಾಗಮ;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
ಶೂನ್ಯ
gui_mch_clear_block(
	ಪೂರ್ಣಾಙ್ಕ	row1,
	ಪೂರ್ಣಾಙ್ಕ	col1,
	ಪೂರ್ಣಾಙ್ಕ	row2,
	ಪೂರ್ಣಾಙ್ಕ	col2)
{
    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearBlock(row1, col1, row2, col2);
	gui.vimWindow->Unlock();
    }
}

    ಶೂನ್ಯ
gui_mch_clear_all()
{
    ಯದಿ (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearAll();
	gui.vimWindow->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
ಶೂನ್ಯ
gui_mch_delete_ಸಾಲುಗಳು(
	ಪೂರ್ಣಾಙ್ಕ	row,
	ಪೂರ್ಣಾಙ್ಕ	ಅಂಕೆ_ಸಾಲುಗಳು)
{
    gui.vimTextArea->mch_Delete_ಸಾಲುಗಳು(row, ಅಂಕೆ_ಸಾಲುಗಳು);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
ಶೂನ್ಯ
gui_mch_ಸೇರಿಸು_ಸಾಲುಗಳು(
	ಪೂರ್ಣಾಙ್ಕ	row,
	ಪೂರ್ಣಾಙ್ಕ	ಅಂಕೆ_ಸಾಲುಗಳು)
{
    gui.vimTextArea->mch_ಸೇರಿಸು_ಸಾಲುಗಳು(row, ಅಂಕೆ_ಸಾಲುಗಳು);
}

#ಯದಿ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ಶೂನ್ಯ
gui_mch_enable_menu(
	ಪೂರ್ಣಾಙ್ಕ	flag)
{
    ಯದಿ (gui.vimWindow->Lock())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->SetEnabled(flag);
	gui.vimWindow->Unlock();
    }
}

ಶೂನ್ಯ
gui_mch_set_menu_pos(
	ಪೂರ್ಣಾಙ್ಕ	x,
	ಪೂರ್ಣಾಙ್ಕ	y,
	ಪೂರ್ಣಾಙ್ಕ	w,
	ಪೂರ್ಣಾಙ್ಕ	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
ಶೂನ್ಯ
gui_mch_add_menu(
	vimmenu_T   *menu,
	ಪೂರ್ಣಾಙ್ಕ	idx)
{
    vimmenu_T	*parent = menu->parent;

    //	popup menu - just create it unattached
    ಯದಿ (menu_is_popup(menu->name) && parent == ರಿಕ್ತ) {
	BPopUpMenu* ತೆಗೆ_Up_Menu = ನೂತನ BPopUpMenu((ಧ್ರುವ ವರ್ಣ*)menu->name, ಅಸತ್ಯ, ಅಸತ್ಯ);
	menu->submenu_id = ತೆಗೆ_Up_Menu;
	menu->id = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (!menu_is_menubar(menu->name)
	    || (parent != ರಿಕ್ತ && parent->submenu_id == ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (gui.vimWindow->Lock())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	ಯದಿ ( parent )
	    tmp = parent->submenu_id;
	ಅನ್ಯಥಾ
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	ಯದಿ ( ! tmp->FindItem((ಧ್ರುವ ವರ್ಣ *) menu->dname)) {

	    BMenu *bmenu = ನೂತನ BMenu((ವರ್ಣ *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->AddItem(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->FindItem((ಧ್ರುವ ವರ್ಣ *) menu->dname);

	}
	gui.vimWindow->Unlock();
    }
}

    ಶೂನ್ಯ
gui_mch_toggle_tearoffs(ಪೂರ್ಣಾಙ್ಕ enable)
{
    // no tearoff menus
}

    ಸ್ಥಿರ BMessage *
MenuMessage(vimmenu_T *menu)
{
    BMessage *m = ನೂತನ BMessage('menu');
    m->AddPointer("VimMenu", (ಶೂನ್ಯ *)menu);

    ಪ್ರತ್ಯಾಗಮ m;
}

/*
 * Add a menu item to a menu
 */
ಶೂನ್ಯ
gui_mch_add_menu_item(
	vimmenu_T   *menu,
	ಪೂರ್ಣಾಙ್ಕ	idx)
{
    ಪೂರ್ಣಾಙ್ಕ	    mnemonic = 0;
    vimmenu_T	*parent = menu->parent;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    ಯದಿ (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	ಯದಿ (menu_is_toolbar(parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ಯದಿ (toolbar != ರಿಕ್ತ) {
		toolbar->AddButton(idx, menu);
	    }
	} ಅನ್ಯಥಾ
#endif

	ಯದಿ (parent->submenu_id != ರಿಕ್ತ || menu_is_popup(parent->name)) {
	    ಯದಿ (menu_is_separator(menu->name)) {
		BSeparatorItem *item = ನೂತನ BSeparatorItem();
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = ರಿಕ್ತ;
	    }
	    ಅನ್ಯಥಾ {
		BMenuItem *item = ನೂತನ BMenuItem((ವರ್ಣ *)menu->dname,
			MenuMessage(menu));
		item->SetTarget(gui.vimTextArea);
		item->SetTrigger((ವರ್ಣ) menu->mnemonic);
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = ರಿಕ್ತ;
	    }
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
ಶೂನ್ಯ
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    ಯದಿ (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	ಯದಿ (menu->parent && menu_is_toolbar(menu->parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ಯದಿ (toolbar != ರಿಕ್ತ) {
		toolbar->RemoveButton(menu);
	    }
	} ಅನ್ಯಥಾ
#endif
	{
	    ಪ್ರತಿಜ್ಞಾ(menu->submenu_id == ರಿಕ್ತ || menu->submenu_id->ಎಣಿಕೆ_ವಸ್ತುಗಳು() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    ಯದಿ (bmenu)
	    {
		bmenu->RemoveItem(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		ಯದಿ (bmenu == gui.vimForm->MenuBar() && bmenu->ಎಣಿಕೆ_ವಸ್ತುಗಳು() == 0)
		{
		    bmenu->ResizeTo(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    delete menu->id;
	    menu->id = ರಿಕ್ತ;
	    menu->submenu_id = ರಿಕ್ತ;

	    gui.menu_height = (ಪೂರ್ಣಾಙ್ಕ) gui.vimForm->MenuHeight();
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
ಶೂನ್ಯ
gui_mch_menu_grey(
	vimmenu_T   *menu,
	ಪೂರ್ಣಾಙ್ಕ	grey)
{
#ifdef FEAT_TOOLBAR
    ಯದಿ (menu->parent && menu_is_toolbar(menu->parent->name)) {
	ಯದಿ (gui.vimWindow->Lock()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ಯದಿ (toolbar != ರಿಕ್ತ) {
		toolbar->GrayButton(menu, grey);
	    }
	    gui.vimWindow->Unlock();
	}
    } ಅನ್ಯಥಾ
#endif
    ಯದಿ (menu->id != ರಿಕ್ತ)
	menu->id->SetEnabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
ಶೂನ್ಯ
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	ಪೂರ್ಣಾಙ್ಕ	hidden)
{
    ಯದಿ (menu->id != ರಿಕ್ತ)
	menu->id->SetEnabled(!hidden);
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
    ಯದಿ (!menu_is_popup(menu->name) || menu->submenu_id == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    BPopUpMenu* popupMenu = dynamic_cast<BPopUpMenu*>(menu->submenu_id);
    ಯದಿ (popupMenu == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    BPoint point;
    ಯದಿ (gui.vimWindow->Lock()) {
	uint32 buttons = 0;
	gui.vimTextArea->GetMouse(&point, &buttons);
	gui.vimTextArea->ConvertToScreen(&point);
	gui.vimWindow->Unlock();
    }
    popupMenu->Go(point, ಸತ್ಯ);
}

#endif // FEAT_MENU

// Mouse stuff

#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
ವರ್ಣ textplain[] = "text/plain";
ವರ್ಣ vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    ಶೂನ್ಯ
clip_mch_request_selection(Clipboard_T *cbd)
{
    ಯದಿ (be_clipboard->Lock())
    {
	BMessage *m = be_clipboard->Data();
	// m->PrintToStream();

	char_u *string = ರಿಕ್ತ;
	ssize_t stringlen = -1;

	ಯದಿ (m->FindData(textplain, B_MIME_TYPE,
		    (ಧ್ರುವ ಶೂನ್ಯ **)&string, &stringlen) == B_OK
		|| m->FindString("text", (ಧ್ರುವ ವರ್ಣ **)&string) == B_OK)
	{
	    ಯದಿ (stringlen == -1)
		stringlen = STRLEN(string);

	    ಪೂರ್ಣಾಙ್ಕ type;
	    ವರ್ಣ *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    ಯದಿ (m->FindData(vimselectiontype, B_MIME_TYPE,
			(ಧ್ರುವ ಶೂನ್ಯ **)&seltype, &seltypelen) == B_OK)
	    {
		ವಿಕಲ್ಪ (*seltype)
		{
		    ಸ್ವಭಾವ:
		    ಪಕ್ಷ 'L':	type = MLINE;	ಭಙ್ಗ;
		    ಪಕ್ಷ 'C':	type = MCHAR;	ಭಙ್ಗ;
#ifdef FEAT_VISUAL
		    ಪಕ್ಷ 'B':	type = MBLOCK;	ಭಙ್ಗ;
#endif
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		// Otherwise use heuristic as documented
		type = memchr(string, stringlen, '\n') ? MLINE : MCHAR;
	    }
	    clip_yank_selection(type, string, (long)stringlen, cbd);
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
    ಪೂರ್ಣಾಙ್ಕ
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    ಶೂನ್ಯ
clip_mch_set_selection(Clipboard_T *cbd)
{
    ಯದಿ (be_clipboard->Lock())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->Data();
	ಪ್ರತಿಜ್ಞಾ(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = ರಿಕ್ತ;
	long_u	ಎಣಿಕೆ;
	ಪೂರ್ಣಾಙ್ಕ type;

	type = clip_convert_selection(&str, &ಎಣಿಕೆ, cbd);

	ಯದಿ (type < 0)
	    ಪ್ರತ್ಯಾಗಮ;

	m->AddData(textplain, B_MIME_TYPE, (ಶೂನ್ಯ *)str, ಎಣಿಕೆ);

	// Add type of selection
	ವರ್ಣ	vtype;
	ವಿಕಲ್ಪ (type)
	{
	    ಸ್ವಭಾವ:
	    ಪಕ್ಷ MLINE:    vtype = 'L';    ಭಙ್ಗ;
	    ಪಕ್ಷ MCHAR:    vtype = 'C';    ಭಙ್ಗ;
#ifdef FEAT_VISUAL
	    ಪಕ್ಷ MBLOCK:   vtype = 'B';    ಭಙ್ಗ;
#endif
	}
	m->AddData(vimselectiontype, B_MIME_TYPE, (ಶೂನ್ಯ *)&vtype, 1);

	vim_free(str);

	be_clipboard->Commit();
	be_clipboard->Unlock();
    }
}

#endif	// FEAT_CLIPBOARD

#ifdef FEAT_BROWSE
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
	ಪೂರ್ಣಾಙ್ಕ saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = ನೂತನ BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    ರಿಕ್ತ, ರಿಕ್ತ, 0, ಅಸತ್ಯ,
	    ನೂತನ BMessage((saving == TRUE) ? 'save' : 'open'), ರಿಕ್ತ, ಸತ್ಯ);

    gui.vimApp->fBrowsedPath.Unset();

    gui.vimApp->fFilePanel->Window()->SetTitle((ವರ್ಣ*)title);
    gui.vimApp->fFilePanel->SetPanelDirectory((ಧ್ರುವ ವರ್ಣ*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    ಯಾವತ್ (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = ರಿಕ್ತ;
    status_t result = gui.vimApp->fBrowsedPath.InitCheck();
    ಯದಿ (result == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->fBrowsedPath.Path());
    } ಅನ್ಯಥಾ
	ಯದಿ (result != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    result, strerror(result));
	}

    delete gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ fileName;
}
#endif // FEAT_BROWSE


#ಯದಿ defined(FEAT_GUI_DIALOG)

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

ಪೂರ್ಣಾಙ್ಕ
gui_mch_dialog(
	ಪೂರ್ಣಾಙ್ಕ	 type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	ಪೂರ್ಣಾಙ್ಕ	 dfltbutton,
	char_u	*textfield,
	ಪೂರ್ಣಾಙ್ಕ ex_cmd)
{
    VimDialog *dialog = ನೂತನ VimDialog(type, (ವರ್ಣ*)title, (ವರ್ಣ*)message,
	    (ವರ್ಣ*)buttons, dfltbutton, (ವರ್ಣ*)textfield, ex_cmd);
    ತಾರ್ಕಿಕ ret = dialog->Go();
    delete dialog;
	ಪ್ರತ್ಯಾಗಮ ret;
}

#endif // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_TO_RGB(pixel);

    ಪ್ರತ್ಯಾಗಮ ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    ಶೂನ್ಯ
gui_mch_setmouse(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    TRACE();
    // TODO
}

#ifdef FEAT_MBYTE_IME
    ಶೂನ್ಯ
im_set_position(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಯದಿ (gui.vimWindow->Lock())
    {
	gui.vimTextArea->DrawIMString();
	gui.vimWindow->Unlock();
    }
    ಪ್ರತ್ಯಾಗಮ;
}
#endif

    ಶೂನ್ಯ
gui_mch_show_toolbar(ಪೂರ್ಣಾಙ್ಕ showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_height = (toolbar && showit) ? toolbar->ToolbarHeight() : 0.;
}

    ಶೂನ್ಯ
gui_mch_set_toolbar_pos(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    ಯದಿ (toolbar != ರಿಕ್ತ) {
	ಯದಿ (gui.vimWindow->Lock()) {
	    toolbar->MoveTo(x, y);
	    toolbar->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

#ಯದಿ defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    ಶೂನ್ಯ
gui_mch_show_tabline(ಪೂರ್ಣಾಙ್ಕ showit)
{
    Vim_Tab_ಸಾಲು *tab_ಸಾಲು = gui.vimForm->Tab_ಸಾಲು();

    ಯದಿ (tab_ಸಾಲು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (!showit != !gui.vimForm->Is_Showing_Tab_ಸಾಲು()) {
	gui.vimForm->Set_Showing_Tab_ಸಾಲು(showit != 0);
	gui.tabline_height = gui.vimForm->TablineHeight();
    }
}

    ಶೂನ್ಯ
gui_mch_set_tabline_pos(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h)
{
    Vim_Tab_ಸಾಲು *tab_ಸಾಲು = gui.vimForm->Tab_ಸಾಲು();
    ಯದಿ (tab_ಸಾಲು != ರಿಕ್ತ) {
	ಯದಿ (gui.vimWindow->Lock()) {
	    tab_ಸಾಲು->MoveTo(x, y);
	    tab_ಸಾಲು->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_showing_tabline()
{
    Vim_Tab_ಸಾಲು *tab_ಸಾಲು = gui.vimForm->Tab_ಸಾಲು();
    ಪ್ರತ್ಯಾಗಮ tab_ಸಾಲು != ರಿಕ್ತ && gui.vimForm->Is_Showing_Tab_ಸಾಲು();
}

/*
 * Update the labels of the tabline.
 */
    ಶೂನ್ಯ
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಙ್ಕ	    nr = 0;
    ಪೂರ್ಣಾಙ್ಕ	    curtabidx = 0;

    Vim_Tab_ಸಾಲು *tab_ಸಾಲು = gui.vimForm->Tab_ಸಾಲು();

    ಯದಿ (tab_ಸಾಲು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    gui.vimWindow->Lock();

    // Add a label for each tab page.  They all contain the same text area.
    ಪ್ರತಿ (tp = first_tabpage; tp != ರಿಕ್ತ; tp = tp->tp_next, ++nr) {
	ಯದಿ (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_ಸಾಲು->TabAt(nr);

	ಯದಿ (tab == ರಿಕ್ತ) {
	    tab = ನೂತನ Vim_Tab_ಸಾಲು::VimTab();
	    tab_ಸಾಲು->AddTab(ರಿಕ್ತ, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->SetLabel((ಧ್ರುವ ವರ್ಣ*)NameBuff);
	tab_ಸಾಲು->Invalidate();
    }

    // Remove any old labels.
    ಯಾವತ್ (nr < tab_ಸಾಲು->ಎಣಿಕೆ_Tabs())
	tab_ಸಾಲು->RemoveTab(nr);

    ಯದಿ (tab_ಸಾಲು->Selection() != curtabidx)
	tab_ಸಾಲು->Select(curtabidx);

    gui.vimWindow->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ಶೂನ್ಯ
gui_mch_set_curtab(ಪೂರ್ಣಾಙ್ಕ nr)
{
    Vim_Tab_ಸಾಲು *tab_ಸಾಲು = gui.vimForm->Tab_ಸಾಲು();
    ಯದಿ (tab_ಸಾಲು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    gui.vimWindow->Lock();

    ಯದಿ (tab_ಸಾಲು->Selection() != nr -1)
	tab_ಸಾಲು->Select(nr -1);

    gui.vimWindow->Unlock();
}

#endif // FEAT_GUI_TABLINE
