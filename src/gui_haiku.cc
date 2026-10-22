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

বাহ্যিক "C" {

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <float.h>
#অন্তর্ভাব <syslog.h>

#অন্তর্ভাব "vim.h"
#অন্তর্ভাব "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#অন্তর্ভাব <Application.h>
#অন্তর্ভাব <Beep.h>
#অন্তর্ভাব <Bitmap.h>
#অন্তর্ভাব <Box.h>
#অন্তর্ভাব <Button.h>
#অন্তর্ভাব <Clipboard.h>
#অন্তর্ভাব <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#অন্তর্ভাব <File.h>
#অন্তর্ভাব <FilePanel.h>
#অন্তর্ভাব <FindDirectory.h>
//#include <Font.h>
#অন্তর্ভাব <IconUtils.h>
#অন্তর্ভাব <Input.h>
#অন্তর্ভাব <ListView.h>
#অন্তর্ভাব <MenuBar.h>
#অন্তর্ভাব <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#অন্তর্ভাব <Path.h>
#অন্তর্ভাব <PictureButton.h>
#অন্তর্ভাব <PopUpMenu.h>
//#include <Region.h>
#অন্তর্ভাব <Resources.h>
//#include <Roster.h>
#অন্তর্ভাব <Screen.h>
#অন্তর্ভাব <ScrollBar.h>
#অন্তর্ভাব <ScrollView.h>
#অন্তর্ভাব <String.h>
#অন্তর্ভাব <StringView.h>
//#include <SupportDefs.h>
#অন্তর্ভাব <TabView.h>
#অন্তর্ভাব <TextControl.h>
#অন্তর্ভাব <TextView.h>
#অন্তর্ভাব <TranslationUtils.h>
#অন্তর্ভাব <TranslatorFormats.h>
#অন্তর্ভাব <View.h>
#অন্তর্ভাব <Window.h>

শ্রেণী VimApp;
শ্রেণী VimFormView;
শ্রেণী VimTextAreaView;
শ্রেণী VimWindow;
শ্রেণী VimToolbar;
শ্রেণী Vim_Tab_সারি;

বাহ্যিক চাবি_map *চাবি_Map;
বাহ্যিক অক্ষর *চাবি_Map_Chars;

বাহ্যিক পূর্ণসংখ্যা প্রধান(পূর্ণসংখ্যা argc, অক্ষর **argv);

#ifndef B_MAX_PORT_গুনতি
#define B_MAX_PORT_গুনতি    255
#endif

// VimApp seems comparable to the X "vimShell"
শ্রেণী VimApp: সর্বজনীন BApplication
{
    প্রকারলেখ BApplication Inherited;
    সর্বজনীন:
    VimApp(ধ্রুবক অক্ষর *appsig);
    ~VimApp();

    // callbacks:
#যদি 0
    আভাসী শূন্য DispatchMessage(BMessage *m, BHandler *h)
    {
	m->PrintToStream();
	Inherited::DispatchMessage(m, h);
    }
#endif
    আভাসী শূন্য ReadyToRun();
    আভাসী শূন্য ArgvReceived(int32 argc, অক্ষর **argv);
    আভাসী শূন্য RefsReceived(BMessage *m);
    আভাসী যৌক্তিক QuitRequested();
    আভাসী শূন্য MessageReceived(BMessage *m);

    স্থির শূন্য SendRefs(BMessage *m, যৌক্তিক changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	fBrowsedPath;
    ব্যক্তিগত:
};

শ্রেণী VimWindow: সর্বজনীন BWindow
{
    প্রকারলেখ BWindow Inherited;
    সর্বজনীন:
    VimWindow();
    ~VimWindow();

    আভাসী শূন্য DispatchMessage(BMessage *m, BHandler *h);
    আভাসী শূন্য WindowActivated(যৌক্তিক active);
    আভাসী যৌক্তিক QuitRequested();

    VimFormView	    *formView;

    ব্যক্তিগত:
    শূন্য init();

    যৌক্তিক is_fullscreen = মিথ্যা;
    BRect saved_frame;
    window_look saved_look;
};

শ্রেণী VimFormView: সর্বজনীন BView
{
    প্রকারলেখ BView Inherited;
    সর্বজনীন:
    VimFormView(BRect frame);
    ~VimFormView();

    // callbacks:
    আভাসী শূন্য AllAttached();
    আভাসী শূন্য FrameResized(দশমিক new_width, দশমিক new_height);

#define MENUBAR_MARGIN	1
    দশমিক MenuHeight() ধ্রুবক
    { ফেরত menuBar ? menuBar->Frame().Height() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() ধ্রুবক
    { ফেরত menuBar; }

    ব্যক্তিগত:
    শূন্য init(BRect);

    BMenuBar	    *menuBar;
    VimTextAreaView *textArea;

#ifdef FEAT_TOOLBAR
    সর্বজনীন:
    দশমিক ToolbarHeight() ধ্রুবক;
    VimToolbar *ToolBar() ধ্রুবক
    { ফেরত toolBar; }
    ব্যক্তিগত:
    VimToolbar	    *toolBar;
#endif

#ifdef FEAT_GUI_TABLINE
    সর্বজনীন:
    Vim_Tab_সারি *Tab_সারি() ধ্রুবক	{ ফেরত tab_সারি; }
    যৌক্তিক Is_Showing_Tab_সারি() ধ্রুবক { ফেরত showing_Tab_সারি; }
    শূন্য Set_Showing_Tab_সারি(যৌক্তিক showing) { showing_Tab_সারি = showing;	}
    দশমিক TablineHeight() ধ্রুবক;
    ব্যক্তিগত:
    Vim_Tab_সারি	*tab_সারি;
    পূর্ণসংখ্যা	showing_Tab_সারি;
#endif
};

শ্রেণী VimTextAreaView: সর্বজনীন BView
{
    প্রকারলেখ BView Inherited;
    সর্বজনীন:
    VimTextAreaView(BRect frame);
    ~VimTextAreaView();

    // callbacks:
    আভাসী শূন্য Draw(BRect updateRect);
    আভাসী শূন্য চাবি_Down(ধ্রুবক অক্ষর *bytes, int32 অঙ্ক_Bytes);
    আভাসী শূন্য MouseDown(BPoint point);
    আভাসী শূন্য MouseUp(BPoint point);
    আভাসী শূন্য MouseMoved(BPoint point, uint32 transit, ধ্রুবক BMessage *message);
    আভাসী শূন্য MessageReceived(BMessage *m);

    // own functions:
    পূর্ণসংখ্যা mchInitFont(char_u *name);
    শূন্য mchDrawString(পূর্ণসংখ্যা row, পূর্ণসংখ্যা col, char_u *s, পূর্ণসংখ্যা len, পূর্ণসংখ্যা flags);
    শূন্য mchClearBlock(পূর্ণসংখ্যা row1, পূর্ণসংখ্যা col1, পূর্ণসংখ্যা row2, পূর্ণসংখ্যা col2);
    শূন্য mchClearAll();
    শূন্য mch_Delete_সারিগুলি(পূর্ণসংখ্যা row, পূর্ণসংখ্যা অঙ্ক_সারিগুলি);
    শূন্য mch_ঢোকাও_সারিগুলি(পূর্ণসংখ্যা row, পূর্ণসংখ্যা অঙ্ক_সারিগুলি);

    স্থির শূন্য guiSendMouseEvent(পূর্ণসংখ্যা button, পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, পূর্ণসংখ্যা repeated_click, int_u modifiers);
    স্থির শূন্য guiMouseMoved(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y);
    স্থির শূন্য guiBlankMouse(যৌক্তিক should_hide);
    স্থির int_u mouseModifiersToVim(int32 beModifiers);

    int32 mouse_Drag_Event_গুনতি;

#ifdef FEAT_MBYTE_IME
    শূন্য DrawIMString(শূন্য);
#endif

    ব্যক্তিগত:
    শূন্য init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#ifdef FEAT_MBYTE_IME
    কাঠামো {
	BMessenger* messenger;
	BMessage* message;
	BPoint location;
	পূর্ণসংখ্যা row;
	পূর্ণসংখ্যা col;
	পূর্ণসংখ্যা গুনতি;
    } IMData;
#endif
};

শ্রেণী VimScrollBar: সর্বজনীন BScrollBar
{
    প্রকারলেখ BScrollBar Inherited;
    সর্বজনীন:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    আভাসী শূন্য মান_Changed(দশমিক new_মান);
    আভাসী শূন্য MouseUp(BPoint where);
    শূন্য Set_মান(দশমিক newval);
    scrollbar_T *getGsb()
    { ফেরত gsb; }

    int32	scroll_Event_গুনতি;

    ব্যক্তিগত:
    scrollbar_T *gsb;
    দশমিক   ignore_মান;
};


#ifdef FEAT_TOOLBAR

শ্রেণী VimToolbar : সর্বজনীন BBox
{
    স্থির BBitmap *normalButtonsBitmap;
    স্থির BBitmap *grayedButtonsBitmap;

    BBitmap *LoadVimBitmap(ধ্রুবক অক্ষর* fileName);
    যৌক্তিক GetPictureFromBitmap(BPicture *pictureTo, int32 সূচক, BBitmap *bitmapFrom, যৌক্তিক pressed);
    যৌক্তিক ModifyBitmapToGrayed(BBitmap *bitmap);

    BList fButtonsList;
    শূন্য InvalidateLayout();

    সর্বজনীন:
    VimToolbar(BRect frame, ধ্রুবক অক্ষর * name);
    ~VimToolbar();

    যৌক্তিক PrepareButtonBitmaps();

    যৌক্তিক AddButton(int32 সূচক, vimmenu_T *menu);
    যৌক্তিক RemoveButton(vimmenu_T *menu);
    যৌক্তিক GrayButton(vimmenu_T *menu, পূর্ণসংখ্যা grey);

    দশমিক ToolbarHeight() ধ্রুবক;
    আভাসী শূন্য AttachedToWindow();
};

BBitmap *VimToolbar::normalButtonsBitmap  = ফাঁকা;
BBitmap *VimToolbar::grayedButtonsBitmap  = ফাঁকা;

ধ্রুবক দশমিক ToolbarMargin = 3.;
ধ্রুবক দশমিক ButtonMargin  = 3.;

#endif //FEAT_TOOLBAR

#ifdef FEAT_GUI_TABLINE

শ্রেণী Vim_Tab_সারি : সর্বজনীন BTabView
{
    সর্বজনীন:
	শ্রেণী VimTab : সর্বজনীন BTab {
	    সর্বজনীন:
		VimTab() : BTab(নতুন BView(BRect(), "-Empty-", 0, 0)) {}

	    আভাসী শূন্য Select(BView* owner);
	};

	Vim_Tab_সারি(BRect r) : BTabView(r, "vimTabLine", B_WIDTH_FROM_LABEL,
	       B_FOLLOW_বাম | B_FOLLOW_শীর্ষ | B_FOLLOW_ডান, B_WILL_DRAW | B_FRAME_EVENTS) {}

    দশমিক TablineHeight() ধ্রুবক;
    আভাসী শূন্য MouseDown(BPoint point);
};

#endif //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
শ্রেণী VimFont: সর্বজনীন BFont
{
    প্রকারলেখ BFont Inherited;
    সর্বজনীন:
    VimFont();
    VimFont(ধ্রুবক VimFont *rhs);
    VimFont(ধ্রুবক BFont *rhs);
    VimFont(ধ্রুবক VimFont &rhs);
    ~VimFont();

    VimFont *পরবর্তী;
    পূর্ণসংখ্যা refcount;
    char_u *name;

    ব্যক্তিগত:
    শূন্য init();
};

#যদি defined(FEAT_GUI_DIALOG)

শ্রেণী VimDialog : সর্বজনীন BWindow
{
    প্রকারলেখ BWindow Inherited;

    BButton* _CreateButton(int32 which, ধ্রুবক অক্ষর* label);

    সর্বজনীন:

    শ্রেণী View : সর্বজনীন BView {
	প্রকারলেখ BView Inherited;

	সর্বজনীন:
	View(BRect frame);
	~View();

	আভাসী শূন্য Draw(BRect updateRect);
	শূন্য InitIcon(int32 type);

	ব্যক্তিগত:
	BBitmap*    fIconBitmap;
    };

    VimDialog(পূর্ণসংখ্যা type, ধ্রুবক অক্ষর *title, ধ্রুবক অক্ষর *message,
	    ধ্রুবক অক্ষর *buttons, পূর্ণসংখ্যা dfltbutton, ধ্রুবক অক্ষর *textfield,
	    পূর্ণসংখ্যা ex_cmd);
    ~VimDialog();

    পূর্ণসংখ্যা Go();

    আভাসী শূন্য MessageReceived(BMessage *বার্তা);

    ব্যক্তিগত:
    sem_id	    fDialogSem;
    পূর্ণসংখ্যা		    f_Dialog_মান;
    BList	    fButtonsList;
    BTextView*	    fMessageView;
    BTextControl*   f_প্রবেশ_Control;
    ধ্রুবক অক্ষর*	    f_প্রবেশ_মান;
};

শ্রেণী VimSelectFontDialog : সর্বজনীন BWindow
{
    প্রকারলেখ BWindow Inherited;

    শূন্য _CleanList(BListView* list);
    শূন্য _UpdateFontStyles();
    শূন্য Update_আকার_প্রবেশ_Preview();
    শূন্য _UpdateFontPreview();
    যৌক্তিক _UpdateFromListItem(BListView* list, অক্ষর* text, পূর্ণসংখ্যা text_আকার);
    সর্বজনীন:

    VimSelectFontDialog(font_family* family, font_style* style, দশমিক* আকার_2);
    ~VimSelectFontDialog();

    যৌক্তিক Go();

    আভাসী শূন্য MessageReceived(BMessage *বার্তা);

    ব্যক্তিগত:
    status_t	    fStatus;
    sem_id	    fDialogSem;
    যৌক্তিক	    f_Dialog_মান;
    font_family*    fFamily;
    font_style*	    fStyle;
    দশমিক*	    f_আকার;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    দশমিক	    f_Font_আকার;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_প্রবেশ;
};

#endif // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

কাঠামো MainArgs {
    পূর্ণসংখ্যা	     argc;
    অক্ষর    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#define	চাবি_বার্তা_BUFSIZ	7
#যদি চাবি_বার্তা_BUFSIZ < MAX_চাবি_CODE_LEN
#error Increase চাবি_বার্তা_BUFSIZ!
#endif

কাঠামো Vim_চাবি_বার্তা {
    char_u  দৈর্ঘ্য;
    char_u  chars[চাবি_বার্তা_BUFSIZ];  // contains Vim encoding
    যৌক্তিক    csi_escape;
};

কাঠামো Vim_Resize_বার্তা {
    পূর্ণসংখ্যা	    width;
    পূর্ণসংখ্যা	    height;
};

কাঠামো Vim_Scroll_Bar_বার্তা {
    VimScrollBar *sb;
    long    মান;
    পূর্ণসংখ্যা	    stillDragging;
};

কাঠামো Vim_Menu_বার্তা {
    vimmenu_T	*guiMenu;
};

কাঠামো Vim_Mouse_বার্তা {
    পূর্ণসংখ্যা	    button;
    পূর্ণসংখ্যা	    x;
    পূর্ণসংখ্যা	    y;
    পূর্ণসংখ্যা	    repeated_click;
    int_u   modifiers;
};

কাঠামো Vim_Mouse_Moved_বার্তা {
    পূর্ণসংখ্যা	    x;
    পূর্ণসংখ্যা	    y;
};

কাঠামো Vim_Focus_বার্তা {
    যৌক্তিক    active;
};

কাঠামো Vim_Refs_বার্তা {
    BMessage   *message;
    যৌক্তিক    changedir;
};

কাঠামো Vim_Tabline_বার্তা {
    পূর্ণসংখ্যা	    সূচক;
};

কাঠামো Vim_Tabline_Menu_বার্তা {
    পূর্ণসংখ্যা	    সূচক;
    পূর্ণসংখ্যা	    event;
};

কাঠামো Vim_বার্তা {
    গণনা Vim_বার্তা_Type {
	চাবি_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    সংঘ {
	কাঠামো Vim_চাবি_বার্তা    চাবি_2;
	কাঠামো Vim_Resize_বার্তা New_আকার;
	কাঠামো Vim_Scroll_Bar_বার্তা	Scroll;
	কাঠামো Vim_Menu_বার্তা   Menu;
	কাঠামো Vim_Mouse_বার্তা  Mouse;
	কাঠামো Vim_Mouse_Moved_বার্তা	MouseMoved;
	কাঠামো Vim_Focus_বার্তা  Focus;
	কাঠামো Vim_Refs_বার্তা   Refs;
	কাঠামো Vim_Tabline_বার্তা	Tabline;
	কাঠামো Vim_Tabline_Menu_বার্তা    TablineMenu;
    } u;
};

#define RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#define GUI_TO_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

স্থির কাঠামো specialkey
{
    uint16  Be_চাবিগুলি;
#define চাবি_3(a,b)    ((a)<<8|(b))
#define K(a)	    চাবি_3(0,a)		// for ASCII codes
#define F(b)	    চাবি_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_চাবিগুলি[] =
{
    {K(B_UP_ARROW),	'k', 'u'},
    {K(B_DOWN_ARROW),	    'k', 'd'},
    {K(B_বাম_ARROW),	    'k', 'l'},
    {K(B_ডান_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_ঢোকাও),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_END),		'@', '7'},
    {K(B_PAGE_UP),	'k', 'P'},	// XK_Prior
    {K(B_PAGE_DOWN),	    'k', 'N'},	    // XK_Next,

#define FIRST_FUNCTION_চাবি  11
    {F(B_F1_চাবি),	'k', '1'},
    {F(B_F2_চাবি),	'k', '2'},
    {F(B_F3_চাবি),	'k', '3'},
    {F(B_F4_চাবি),	'k', '4'},
    {F(B_F5_চাবি),	'k', '5'},
    {F(B_F6_চাবি),	'k', '6'},
    {F(B_F7_চাবি),	'k', '7'},
    {F(B_F8_চাবি),	'k', '8'},
    {F(B_F9_চাবি),	'k', '9'},
    {F(B_F10_চাবি),	'k', ';'},

    {F(B_F11_চাবি),	'F', '1'},
    {F(B_F12_চাবি),	'F', '2'},
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
    {F(B_PRINT_চাবি),	    '%', '9'},

#যদি 0
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

#define অঙ্ক_SPECIAL_চাবিগুলি    ARRAY_দৈর্ঘ্য(special_চাবিগুলি)

// ---------------- VimApp ----------------

    স্থির শূন্য
docd(BPath &path)
{
    mch_chdir((অক্ষর *)path.Path());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	স্থির শূন্য
drop_callback(শূন্য *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	স্থির শূন্য
RefsReceived(BMessage *m, যৌক্তিক changedir)
{
    uint32 type;
    int32 গুনতি;

    m->PrintToStream();
    নির্বাচন (m->what) {
	ক্ষেত্র B_REFS_RECEIVED:
	ক্ষেত্র B_SIMPLE_তথ্য:
	    m->GetInfo("refs", &type, &গুনতি);
	    যদি (type != B_REF_TYPE)
		যাও bad;
	    বিরতি;
	ক্ষেত্র B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &গুনতি);
	    যদি (type != B_STRING_TYPE)
		যাও bad;
	    যদি (changedir) {
		অক্ষর *dirname;
		যদি (m->FindString("cwd", (ধ্রুবক অক্ষর **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    বিরতি;
	পূর্বনির্ধারিত:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    delete m;
	    ফেরত;
    }

#ifdef FEAT_VISUAL
    reset_VIsual();
#endif

    char_u  **fnames;
    fnames = (char_u **) alloc(গুনতি * আকার(char_u *));
    পূর্ণসংখ্যা fname_সূচক = 0;

    নির্বাচন (m->what) {
	ক্ষেত্র B_REFS_RECEIVED:
	ক্ষেত্র B_SIMPLE_তথ্য:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    জন্য (পূর্ণসংখ্যা i = 0; i < গুনতি; ++i)
	    {
		entry_ref ref;
		যদি (m->FindRef("refs", i, &ref) == B_OK) {
		    BEntry entry(&ref, মিথ্যা);
		    BPath path;
		    entry.GetPath(&path);

		    // Change to parent directory?
		    যদি (changedir) {
			BPath parentpath;
			path.GetParent(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    যদি (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			যদি (!changedir)
			    docd(path);
		    } অন্যথা {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)path.Path(), IObuff);
			যদি (fname == ফাঁকা)
			    fname = (char_u *)path.Path();
			fnames[fname_সূচক++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = মিথ্যা;
		}
	    }
	    বিরতি;
	ক্ষেত্র B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    জন্য (পূর্ণসংখ্যা i = 1; i < গুনতি; ++i)
	    {
		অক্ষর *fname;

		যদি (m->FindString("argv", i, (ধ্রুবক অক্ষর **) &fname) == B_OK) {
		    fnames[fname_সূচক++] = vim_strsave((char_u *)fname);
		}
	    }
	    বিরতি;
	পূর্বনির্ধারিত:
	    // fprintf(stderr, "case default\n");
	    বিরতি;
    }

    delete m;

    // Handle the drop, :edit to get to the file
    যদি (fname_সূচক > 0) {
	handle_drop(fname_সূচক, fnames, FALSE, drop_callback, ফাঁকা);

	setcursor();
	out_flush();
    } অন্যথা {
	vim_free(fnames);
    }
}

VimApp::VimApp(ধ্রুবক অক্ষর *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(ফাঁকা)
{
}

VimApp::~VimApp()
{
}

    শূন্য
VimApp::ReadyToRun()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    শূন্য
VimApp::ArgvReceived(int32 arg_argc, অক্ষর **arg_argv)
{
    যদি (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	যদি (gui.vimWindow)
	    gui.vimWindow->Minimize(মিথ্যা);
	BMessage *m = CurrentMessage();
	DetachCurrentMessage();
	SendRefs(m, সত্য);
    }
}

    শূন্য
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    পূর্ণসংখ্যা limit = 15;
    যখন (--limit >= 0 && (curbuf == ফাঁকা || curbuf->b_start_ffc == 0))
	snooze(100000);    //  0.1 s
    যদি (gui.vimWindow)
	gui.vimWindow->Minimize(মিথ্যা);
    DetachCurrentMessage();
    SendRefs(m, সত্য);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    শূন্য
VimApp::SendRefs(BMessage *m, যৌক্তিক changedir)
{
    Vim_Refs_বার্তা rm;
    rm.message = m;
    rm.changedir = changedir;

    write_port(gui.vdcmp, Vim_বার্তা::Refs, &rm, আকার(rm));
    //	calls ::RefsReceived
}

    শূন্য
VimApp::MessageReceived(BMessage *m)
{
    নির্বাচন (m->what) {
	ক্ষেত্র 'save':
	    {
		entry_ref refDirectory;
		m->FindRef("directory", &refDirectory);
		fBrowsedPath.SetTo(&refDirectory);
		BString strName;
		m->FindString("name", &strName);
		fBrowsedPath.Append(strName.String());
	    }
	    বিরতি;
	ক্ষেত্র 'open':
	    {
		entry_ref ref;
		m->FindRef("refs", &ref);
		fBrowsedPath.SetTo(&ref);
	    }
	    বিরতি;
	ক্ষেত্র B_CANCEL:
	    {
		BFilePanel *panel;
		m->FindPointer("source", (শূন্য**)&panel);
		যদি (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    বিরতি;
	পূর্বনির্ধারিত:
	    Inherited::MessageReceived(m);
	    বিরতি;
    }
}

    যৌক্তিক
VimApp::QuitRequested()
{
    (শূন্য)Inherited::QuitRequested();
    ফেরত মিথ্যা;
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
    যদি (formView) {
	RemoveChild(formView);
	delete formView;
    }
    gui.vimWindow = ফাঁকা;
}

    শূন্য
VimWindow::init()
{
    // Attach the VimFormView
    formView = নতুন VimFormView(Bounds());
    যদি (formView != ফাঁকা) {
	AddChild(formView);
    }
}

#যদি 0  //  disabled in zeta patch
    শূন্য
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    নির্বাচন (m->what) {
	ক্ষেত্র B_MOUSE_UP:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		যদি (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->FindPoint("where", &where);
		    v->MouseUp(where);
		} অন্যথা {
		    Inherited::DispatchMessage(m, h);
		}
	    }
	    বিরতি;
	পূর্বনির্ধারিত:
	    Inherited::DispatchMessage(m, h);
    }
}
#endif

    শূন্য
VimWindow::WindowActivated(যৌক্তিক active)
{
    Inherited::WindowActivated(active);
    // the textArea gets the keyboard action
    যদি (active && gui.vimTextArea)
	gui.vimTextArea->MakeFocus(সত্য);

    কাঠামো Vim_Focus_বার্তা fm;
    fm.active = active;

    write_port(gui.vdcmp, Vim_বার্তা::Focus, &fm, আকার(fm));
}

    যৌক্তিক
VimWindow::QuitRequested()
{
    কাঠামো Vim_চাবি_বার্তা km;
    km.দৈর্ঘ্য = 5;
    memcpy((অক্ষর *)km.chars, "\033:qa\r", km.দৈর্ঘ্য);
    km.csi_escape = মিথ্যা;
    write_port(gui.vdcmp, Vim_বার্তা::চাবি_2, &km, আকার(km));
    ফেরত মিথ্যা;
}
    শূন্য
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    যৌক্তিক should_propagate = সত্য;

    নির্বাচন (m->what)
    {
	ক্ষেত্র B_চাবি_DOWN:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->FindInt32("raw_char", &scancode);
		m->FindInt32("modifiers", &beModifiers);

		যদি (scancode == B_ENTER && (beModifiers & B_বাম_COMMAND_চাবি))
		    {
			should_propagate = মিথ্যা;
			যদি (এই->is_fullscreen)
			    {
				এই->is_fullscreen = মিথ্যা;
				ResizeTo(এই->saved_frame.Width(), এই->saved_frame.Height());
				MoveTo(এই->saved_frame.বাম, এই->saved_frame.শীর্ষ_2);
				SetLook(এই->saved_look);
				SetFlags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } অন্যথা {
				এই->saved_frame = Frame();
				এই->saved_look = Look();
				এই->is_fullscreen = সত্য;
				BScreen s(এই);
				SetLook(B_NO_BORDER_WINDOW_LOOK);
				ResizeTo(s.Frame().Width() + 1, s.Frame().Height() + 1);
				MoveTo(s.Frame().বাম, s.Frame().শীর্ষ_2);
				SetFlags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    যদি (should_propagate)
	Inherited::DispatchMessage(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect frame):
    BView(frame, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_FRAME_EVENTS),
    menuBar(ফাঁকা),
#ifdef FEAT_TOOLBAR
    toolBar(ফাঁকা),
#endif
#ifdef FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_সারি(ফাঁকা),
#endif
    textArea(ফাঁকা)
{
    init(frame);
}

VimFormView::~VimFormView()
{
    যদি (menuBar) {
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
    delete tab_সারি;
#endif

    যদি (textArea) {
	RemoveChild(textArea);
	delete textArea;
    }
    gui.vimForm = ফাঁকা;
}

    শূন্য
VimFormView::init(BRect frame)
{
    menuBar = নতুন BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    AddChild(menuBar);

#ifdef FEAT_TOOLBAR
    toolBar = নতুন VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    AddChild(toolBar);
#endif

#ifdef FEAT_GUI_TABLINE
    tab_সারি = নতুন Vim_Tab_সারি(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    AddChild(tab_সারি);
#endif

    BRect remaining = frame;
    textArea = নতুন VimTextAreaView(remaining);
    AddChild(textArea);
    // The textArea will be resized later when menus are added

    gui.vimForm = এই;
}

#ifdef FEAT_TOOLBAR
    দশমিক
VimFormView::ToolbarHeight() ধ্রুবক
{
    ফেরত toolBar ? toolBar->ToolbarHeight() : 0.;
}
#endif

#ifdef FEAT_GUI_TABLINE
    দশমিক
VimFormView::TablineHeight() ধ্রুবক
{
    ফেরত (tab_সারি && Is_Showing_Tab_সারি()) ? tab_সারি->TablineHeight() : 0.;
}
#endif

    শূন্য
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    যদি (menuBar && textArea) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect remaining = Bounds();

#ifdef FEAT_MENU
	remaining.শীর্ষ_2 += MenuHeight();
	menuBar->ResizeTo(remaining.ডান, remaining.শীর্ষ_2);
	gui.menu_height = (পূর্ণসংখ্যা) MenuHeight();
#endif

#ifdef FEAT_TOOLBAR
	toolBar->MoveTo(remaining.বাম, remaining.শীর্ষ_2);
	toolBar->ResizeTo(remaining.ডান, ToolbarHeight());
	remaining.শীর্ষ_2 += ToolbarHeight();
	gui.toolbar_height = ToolbarHeight();
#endif

#ifdef FEAT_GUI_TABLINE
	tab_সারি->MoveTo(remaining.বাম, remaining.শীর্ষ_2);
	tab_সারি->ResizeTo(remaining.ডান + 1, TablineHeight());
	remaining.শীর্ষ_2 += TablineHeight();
	gui.tabline_height = TablineHeight();
#endif

	textArea->ResizeTo(remaining.Width(), remaining.Height());
	textArea->MoveTo(remaining.বাম, remaining.শীর্ষ_2);
    }


    Inherited::AllAttached();
}

    শূন্য
VimFormView::FrameResized(দশমিক new_width, দশমিক new_height)
{
    কাঠামো Vim_Resize_বার্তা sm;
    পূর্ণসংখ্যা adjust_h, adjust_w;

    new_width += 1;	//  adjust from width to number of pixels occupied
    new_height += 1;

    sm.width = (পূর্ণসংখ্যা) new_width;
    sm.height = (পূর্ণসংখ্যা) new_height;
    adjust_w = ((পূর্ণসংখ্যা)new_width - gui_get_base_width()) % gui.char_width;
    adjust_h = ((পূর্ণসংখ্যা)new_height - gui_get_base_height()) % gui.char_height;

    যদি (adjust_w > 0 || adjust_h > 0) {
	sm.width  -= adjust_w;
	sm.height -= adjust_h;
    }

    write_port(gui.vdcmp, Vim_বার্তা::Resize, &sm, আকার(sm));
    //	calls gui_resize_shell(new_width, new_height);

    ফেরত;

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
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_প্রবেশ_METHOD_AWARE
#অন্যথা
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE
#endif
	),
    mouse_Drag_Event_গুনতি(0)
{
#ifdef FEAT_MBYTE_IME
    IMData.messenger = ফাঁকা;
    IMData.message = ফাঁকা;
#endif
    init(frame);
}

VimTextAreaView::~VimTextAreaView()
{
    gui.vimTextArea = ফাঁকা;
}

    শূন্য
VimTextAreaView::init(BRect frame)
{
    // set up global var for fast access
    gui.vimTextArea = এই;

    /*
     * Tell the app server not to erase the view: we will
     * fill it in completely by ourselves.
     * (Does this really work? Even if not, it won't harm either.)
     */
    SetViewColor(B_TRANSPARENT_32_BIT);
#define PEN_WIDTH   1
    Set_Pen_আকার(PEN_WIDTH);
#define W_WIDTH(curwin)   0
}

    শূন্য
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
    Set_নিচের_Color(rgb);
    FillRect(updateRect, B_SOLID_নিচের);
    gui_redraw((পূর্ণসংখ্যা) updateRect.বাম, (পূর্ণসংখ্যা) updateRect.শীর্ষ_2,
	    (পূর্ণসংখ্যা) (updateRect.Width() + PEN_WIDTH), (পূর্ণসংখ্যা) (updateRect.Height() + PEN_WIDTH));

    // Clear the border areas if needed
    Set_নিচের_Color(rgb);

    যদি (updateRect.বাম < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.বাম, updateRect.শীর্ষ_2,
		    FILL_X(0)-PEN_WIDTH, updateRect.bottom), B_SOLID_নিচের);
    যদি (updateRect.শীর্ষ_2 < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.বাম, updateRect.শীর্ষ_2,
		    updateRect.ডান, FILL_Y(0)-PEN_WIDTH), B_SOLID_নিচের);
    যদি (updateRect.ডান >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((পূর্ণসংখ্যা)Columns), updateRect.শীর্ষ_2,
		    updateRect.ডান, updateRect.bottom), B_SOLID_নিচের);
    যদি (updateRect.bottom >= FILL_Y(Rows))   //  bottom border
	FillRect(BRect(updateRect.বাম, FILL_Y((পূর্ণসংখ্যা)Rows),
		    updateRect.ডান, updateRect.bottom), B_SOLID_নিচের);

#ifdef FEAT_MBYTE_IME
    DrawIMString();
#endif
}

    শূন্য
VimTextAreaView::চাবি_Down(ধ্রুবক অক্ষর *bytes, int32 অঙ্ক_Bytes)
{
    কাঠামো Vim_চাবি_বার্তা km;
    char_u *dest = km.chars;

    যৌক্তিক canHaveVimModifiers = মিথ্যা;

    BMessage *বার্তা = Window()->CurrentMessage();
    প্রতিজ্ঞা(বার্তা);
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
    যদি (অঙ্ক_Bytes > 1) {
	// This cannot be a special key
	যদি (অঙ্ক_Bytes > চাবি_বার্তা_BUFSIZ)
	    অঙ্ক_Bytes = চাবি_বার্তা_BUFSIZ;	    //	should never happen... ???
	km.দৈর্ঘ্য = অঙ্ক_Bytes;
	memcpy((অক্ষর *)dest, bytes, অঙ্ক_Bytes);
	km.csi_escape = সত্য;
    } অন্যথা {
	int32 scancode = 0;
	বার্তা->FindInt32("key", &scancode);

	int32 beModifiers = 0;
	বার্তা->FindInt32("modifiers", &beModifiers);

	char_u string[3];
	পূর্ণসংখ্যা len = 0;
	km.দৈর্ঘ্য = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	প্রতিজ্ঞা(B_BACKSPACE <= 0x20);
	প্রতিজ্ঞা(B_DELETE == 0x7F);
	যদি (((char_u)bytes[0] <= 0x20 || (char_u)bytes[0] == 0x7F) &&
		অঙ্ক_Bytes == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    যদি (beModifiers & B_CONTROL_চাবি) {
		পূর্ণসংখ্যা সূচক = চাবি_Map->normal_map[scancode];
		পূর্ণসংখ্যা new_অঙ্ক_Bytes = চাবি_Map_Chars[সূচক];
		char_u *newBytes = (char_u *)&চাবি_Map_Chars[সূচক + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		যদি (new_অঙ্ক_Bytes != 1 || (*newBytes > 0x20 &&
			    *newBytes != 0x7F )) {
		    যাও notspecial;
		}
		bytes = (অক্ষর *)newBytes;
	    }
	    canHaveVimModifiers = সত্য;

	    uint16 beoskey;
	    পূর্ণসংখ্যা first, last;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    যদি (অঙ্ক_Bytes == 0 || bytes[0] == B_FUNCTION_চাবি) {
		beoskey = F(scancode);
		first = FIRST_FUNCTION_চাবি;
		last = অঙ্ক_SPECIAL_চাবিগুলি;
	    } অন্যথা যদি (*bytes == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		first = last = 0;
	    } অন্যথা {
		beoskey = K(bytes[0]);
		first = 0;
		last = FIRST_FUNCTION_চাবি;
	    }

	    জন্য (পূর্ণসংখ্যা i = first; i < last; i++) {
		যদি (special_চাবিগুলি[i].Be_চাবিগুলি == beoskey) {
		    string[0] = CSI;
		    string[1] = special_চাবিগুলি[i].vim_code0;
		    string[2] = special_চাবিগুলি[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	যদি (len == 0) {
	    string[0] = bytes[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#যদি 0
	যদি (len == 3 ||
		bytes[0] == B_SPACE || bytes[0] == B_TAB ||
		bytes[0] == B_RETURN || bytes[0] == '\r' ||
		bytes[0] == B_ESCAPE)
#অন্যথা
	    যদি (canHaveVimModifiers)
#endif
	    {
		পূর্ণসংখ্যা modifiers;
		modifiers = 0;
		যদি (beModifiers & B_SHIFT_চাবি)
		    modifiers |= MOD_MASK_SHIFT;
		যদি (beModifiers & B_CONTROL_চাবি)
		    modifiers |= MOD_MASK_CTRL;
		যদি (beModifiers & B_OPTION_চাবি)
		    modifiers |= MOD_MASK_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		পূর্ণসংখ্যা চাবি;
		যদি (string[0] == CSI && len == 3)
		    চাবি = TO_SPECIAL(string[1], string[2]);
		অন্যথা
		    চাবি = string[0];
		চাবি = simplify_চাবি(চাবি, &modifiers);
		যদি (IS_SPECIAL(চাবি))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(চাবি);
		    string[2] = K_THIRD(চাবি);
		    len = 3;
		}
		অন্যথা
		{
		    string[0] = চাবি;
		    len = 1;
		}

		যদি (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.দৈর্ঘ্য = 3;
		}
	    }
	memcpy((অক্ষর *)dest, string, len);
	km.দৈর্ঘ্য += len;
	km.csi_escape = মিথ্যা;
    }

    write_port(gui.vdcmp, Vim_বার্তা::চাবি_2, &km, আকার(km));

    /*
     * blank out the pointer if necessary
     */
    যদি (p_mh && !gui.pointer_hidden)
    {
	guiBlankMouse(সত্য);
	gui.pointer_hidden = TRUE;
    }
}
শূন্য
VimTextAreaView::guiSendMouseEvent(
	পূর্ণসংখ্যা	button,
	পূর্ণসংখ্যা	x,
	পূর্ণসংখ্যা	y,
	পূর্ণসংখ্যা	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_বার্তা mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_port(gui.vdcmp, Vim_বার্তা::Mouse, &mm, আকার(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    যদি (gui.pointer_hidden)
    {
	guiBlankMouse(মিথ্যা);
	gui.pointer_hidden = FALSE;
    }
}

শূন্য
VimTextAreaView::guiMouseMoved(
	পূর্ণসংখ্যা	x,
	পূর্ণসংখ্যা	y)
{
    Vim_Mouse_Moved_বার্তা mm;

    mm.x = x;
    mm.y = y;

    write_port(gui.vdcmp, Vim_বার্তা::MouseMoved, &mm, আকার(mm));

    যদি (gui.pointer_hidden)
    {
	guiBlankMouse(মিথ্যা);
	gui.pointer_hidden = FALSE;
    }
}

    শূন্য
VimTextAreaView::guiBlankMouse(যৌক্তিক should_hide)
{
    যদি (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } অন্যথা {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
VimTextAreaView::mouseModifiersToVim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    যদি (beModifiers & B_SHIFT_চাবি)
	vim_modifiers |= MOUSE_SHIFT;
    যদি (beModifiers & B_CONTROL_চাবি)
	vim_modifiers |= MOUSE_CTRL;
    যদি (beModifiers & B_OPTION_চাবি)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    ফেরত vim_modifiers;
}

    শূন্য
VimTextAreaView::MouseDown(BPoint point)
{
    BMessage *m = Window()->CurrentMessage();
    প্রতিজ্ঞা(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    পূর্ণসংখ্যা vimButton;

    যদি (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_বাম;
    অন্যথা যদি (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_ডান;
    অন্যথা যদি (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_মধ্য;
    অন্যথা
	ফেরত;		// Unknown button

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

    শূন্য
VimTextAreaView::MouseUp(BPoint point)
{
    vimMouseButton = 0;

    BMessage *m = Window()->CurrentMessage();
    প্রতিজ্ঞা(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->FindInt32("modifiers", &modifiers);

    vimMouseModifiers = mouseModifiersToVim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, point.x, point.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::MouseUp(point);
}

    শূন্য
VimTextAreaView::MouseMoved(BPoint point, uint32 transit, ধ্রুবক BMessage *message)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    যদি (gui.pointer_hidden)
    {
	guiBlankMouse(মিথ্যা);
	gui.pointer_hidden = FALSE;
    }

    যদি (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(point.x, point.y);
	ফেরত;
    }

    atomic_add(&mouse_Drag_Event_গুনতি, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, point.x, point.y, 0, vimMouseModifiers);
}

    শূন্য
VimTextAreaView::MessageReceived(BMessage *m)
{
    নির্বাচন (m->what) {
	ক্ষেত্র 'menu':
	    {
		Vim_Menu_বার্তা mm;
		mm.guiMenu = ফাঁকা;  // in case no pointer in msg
		m->FindPointer("VimMenu", (শূন্য **)&mm.guiMenu);
		write_port(gui.vdcmp, Vim_বার্তা::Menu, &mm, আকার(mm));
	    }
	    বিরতি;
	ক্ষেত্র B_MOUSE_WHEEL_CHANGED:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		দশমিক small=0, big=0, dy=0;
		m->FindFloat("be:wheel_delta_y", &dy);
		scb->GetSteps(&small, &big);
		scb->Set_মান(scb->মান_2()+small*dy*3);
		scb->মান_Changed(scb->মান_2());
#যদি 0
		scb = curwin->w_scrollbars[0].id;
		scb->GetSteps(&small, &big);
		scb->Set_মান(scb->মান_2()+small*dy);
		scb->মান_Changed(scb->মান_2());
#endif
	    }
	    বিরতি;
#ifdef FEAT_MBYTE_IME
	ক্ষেত্র B_প্রবেশ_METHOD_EVENT:
	    {
		int32 opcode;
		m->FindInt32("be:opcode", &opcode);
		নির্বাচন(opcode)
		{
		    ক্ষেত্র B_প্রবেশ_METHOD_STARTED:
			যদি (!IMData.messenger) delete IMData.messenger;
			IMData.messenger = নতুন BMessenger();
			m->FindMessenger("be:reply_to", IMData.messenger);
			বিরতি;
		    ক্ষেত্র B_প্রবেশ_METHOD_CHANGED:
			{
			    BString str;
			    যৌক্তিক confirmed;
			    যদি (IMData.message) *(IMData.message) = *m;
			    অন্যথা	       IMData.message = নতুন BMessage(*m);
			    DrawIMString();
			    m->FindBool("be:confirmed", &confirmed);
			    যদি (confirmed)
			    {
				m->FindString("be:string", &str);
				char_u *chars = (char_u*)str.String();
				কাঠামো Vim_চাবি_বার্তা km;
				km.csi_escape = সত্য;
				পূর্ণসংখ্যা clen;
				পূর্ণসংখ্যা i = 0;
				যখন (i < str.দৈর্ঘ্য_2())
				{
				    clen = utf_ptr2len(chars+i);
				    memcpy(km.chars, chars+i, clen);
				    km.দৈর্ঘ্য = clen;
				    write_port(gui.vdcmp, Vim_বার্তা::চাবি_2, &km, আকার(km));
				    i += clen;
				}
			    }
			}
			বিরতি;
		    ক্ষেত্র B_প্রবেশ_METHOD_LOCATION_REQUEST:
			{
			    BMessage বার্তা(B_প্রবেশ_METHOD_EVENT);
			    বার্তা.AddInt32("be:opcode", B_প্রবেশ_METHOD_LOCATION_REQUEST);
			    বার্তা.AddPoint("be:location_reply", IMData.location);
			    বার্তা.AddFloat("be:height_reply", FILL_Y(1));
			    IMData.messenger->SendMessage(&বার্তা);
			}
			বিরতি;
		    ক্ষেত্র B_প্রবেশ_METHOD_STOPPED:
			delete IMData.messenger;
			delete IMData.message;
			IMData.messenger = ফাঁকা;
			IMData.message = ফাঁকা;
			বিরতি;
		}
	    }
	    // TODO: sz: break here???
#endif
	পূর্বনির্ধারিত:
	    যদি (m->WasDropped()) {
		BWindow *w = Window();
		w->DetachCurrentMessage();
		w->Minimize(মিথ্যা);
		VimApp::SendRefs(m, (modifiers() & B_SHIFT_চাবি) != 0);
	    } অন্যথা {
		Inherited::MessageReceived(m);
	    }
	    বিরতি;
    }
}

    পূর্ণসংখ্যা
VimTextAreaView::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    যদি (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_set_font((GuiFont)newFont);
	যদি (name && STRCMP(name, "*") != 0)
	    hl_set_font_name(name);

	SetDrawingMode(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	ফেরত OK;
    }
    ফেরত FAIL;
}

    শূন্য
VimTextAreaView::mchDrawString(পূর্ণসংখ্যা row, পূর্ণসংখ্যা col, char_u *s, পূর্ণসংখ্যা len, পূর্ণসংখ্যা flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    যদি (!(flags & DRAW_TRANSP)) {
	পূর্ণসংখ্যা cells;
	cells = 0;
	জন্য (পূর্ণসংখ্যা i=0; i<len; i++) {
	    পূর্ণসংখ্যা cn = utf_ptr2cells((char_u *)(s+i));
	    যদি (cn<4) cells += cn;
	}

	BRect r(FILL_X(col), FILL_Y(row),
		FILL_X(col + cells) - PEN_WIDTH, FILL_Y(row + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_নিচের);
    }

    BFont font;
    এই->GetFont(&font);
    যদি (!font.IsFixed())
    {
	অক্ষর* p = (অক্ষর*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	পূর্ণসংখ্যা cells;
	যখন ((p - (অক্ষর*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.Set(TEXT_X(col+lastpos), TEXT_Y(row));
	    DrawString(p, clen, where);
	    যদি (flags & DRAW_BOLD) {
		where.x += 1.0;
		SetDrawingMode(B_OP_BLEND);
		DrawString(p, clen, where);
		SetDrawingMode(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    যদি (cells<4) lastpos += cells;
	    অন্যথা	lastpos++;
	    p += clen;
	}
    }
    অন্যথা
    {
	BPoint where(TEXT_X(col), TEXT_Y(row));
	DrawString((অক্ষর*)s, len, where);
	যদি (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    SetDrawingMode(B_OP_BLEND);
	    DrawString((অক্ষর*)s, len, where);
	    SetDrawingMode(B_OP_COPY);
	}
    }

    যদি (flags & DRAW_UNDERL) {
	পূর্ণসংখ্যা cells;
	cells = 0;
	জন্য (পূর্ণসংখ্যা i=0; i<len; i++) {
	    পূর্ণসংখ্যা cn = utf_ptr2cells((char_u *)(s+i));
	    যদি (cn<4) cells += cn;
	}

	BPoint start(FILL_X(col), FILL_Y(row + 1) - PEN_WIDTH);
	BPoint end(FILL_X(col + cells) - PEN_WIDTH, start.y);

	Stroke_সারি(start, end);
    }
}

শূন্য
VimTextAreaView::mchClearBlock(
	পূর্ণসংখ্যা	row1,
	পূর্ণসংখ্যা	col1,
	পূর্ণসংখ্যা	row2,
	পূর্ণসংখ্যা	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_WIDTH, FILL_Y(row2 + 1) - PEN_WIDTH);
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(r, B_SOLID_নিচের);
}

    শূন্য
VimTextAreaView::mchClearAll()
{
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(Bounds(), B_SOLID_নিচের);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    শূন্য
VimTextAreaView::mch_Delete_সারিগুলি(পূর্ণসংখ্যা row, পূর্ণসংখ্যা অঙ্ক_সারিগুলি)
{
    BRect source, dest;
    source.বাম = FILL_X(gui.scroll_region_বাম);
    source.শীর্ষ_2 = FILL_Y(row + অঙ্ক_সারিগুলি);
    source.ডান = FILL_X(gui.scroll_region_ডান + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    dest.বাম = FILL_X(gui.scroll_region_বাম);
    dest.শীর্ষ_2 = FILL_Y(row);
    dest.ডান = FILL_X(gui.scroll_region_ডান + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot - অঙ্ক_সারিগুলি + 1) - PEN_WIDTH;

    যদি (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(gui.scroll_region_bot - অঙ্ক_সারিগুলি + 1,
		gui.scroll_region_বাম,
		gui.scroll_region_bot, gui.scroll_region_ডান);


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
    শূন্য
VimTextAreaView::mch_ঢোকাও_সারিগুলি(পূর্ণসংখ্যা row, পূর্ণসংখ্যা অঙ্ক_সারিগুলি)
{
    BRect source, dest;

    // XXX Attempt at a hack:
    gui.vimWindow->UpdateIfNeeded();
    source.বাম = FILL_X(gui.scroll_region_বাম);
    source.শীর্ষ_2 = FILL_Y(row);
    source.ডান = FILL_X(gui.scroll_region_ডান + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot - অঙ্ক_সারিগুলি + 1) - PEN_WIDTH;

    dest.বাম = FILL_X(gui.scroll_region_বাম);
    dest.শীর্ষ_2 = FILL_Y(row + অঙ্ক_সারিগুলি);
    dest.ডান = FILL_X(gui.scroll_region_ডান + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    যদি (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(row, gui.scroll_region_বাম,
		row + অঙ্ক_সারিগুলি - 1, gui.scroll_region_ডান);

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
শূন্য VimTextAreaView::DrawIMString(শূন্য)
{
    স্থির ধ্রুবক rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    ধ্রুবক অক্ষর* s;
    পূর্ণসংখ্যা len;
    BMessage* বার্তা = IMData.message;
    যদি (!বার্তা)
	ফেরত;
    gui_redraw_block(IMData.row, 0,
	    IMData.row + IMData.গুনতি, W_WIDTH(curwin), GUI_MON_NOCLEAR);
    যৌক্তিক confirmed = মিথ্যা;
    বার্তা->FindBool("be:confirmed", &confirmed);
    যদি (confirmed)
	ফেরত;
    rgb_color hcolor = উপরের_Color(), lcolor = নিচের_Color();
    বার্তা->FindString("be:string", &str);
    s = str.String();
    len = str.দৈর্ঘ্য_2();
    Set_উপরের_Color(0, 0, 0);
    IMData.row = gui.row;
    IMData.col = gui.col;
    int32 sel_start = 0, sel_end = 0;
    বার্তা->FindInt32("be:selection", 0, &sel_start);
    বার্তা->FindInt32("be:selection", 1, &sel_end);
    পূর্ণসংখ্যা clen, cn;
    BPoint pos(IMData.col, 0);
    BRect r;
    BPoint where;
    IMData.location = ConvertToScreen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
    জন্য (পূর্ণসংখ্যা i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	যদি (pos.x + cn > W_WIDTH(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	যদি (sel_start<=i && i<sel_end)
	{
	    Set_নিচের_Color(r_highlight);
	    IMData.location = ConvertToScreen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
	}
	অন্যথা
	{
	    Set_নিচের_Color(b_highlight);
	}
	r.Set(FILL_X(pos.x), FILL_Y(IMData.row + pos.y),
		FILL_X(pos.x + cn) - PEN_WIDTH,
		FILL_Y(IMData.row + pos.y + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_নিচের);
	where.Set(TEXT_X(pos.x), TEXT_Y(IMData.row + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.গুনতি = (পূর্ণসংখ্যা)pos.y;

    Set_উপরের_Color(hcolor);
    Set_নিচের_Color(lcolor);
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
	    "vim scrollbar", (BView *)ফাঁকা,
	    0.0, 10.0, posture),
    ignore_মান(-1),
    scroll_Event_গুনতি(0)
{
    gsb = g;
    SetResizingMode(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    শূন্য
VimScrollBar::মান_Changed(দশমিক new_মান)
{
    যদি (ignore_মান >= 0.0 && new_মান == ignore_মান) {
	ignore_মান = -1;
	ফেরত;
    }
    ignore_মান = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_add(&scroll_Event_গুনতি, 1);

    কাঠামো Vim_Scroll_Bar_বার্তা sm;

    sm.sb = এই;
    sm.মান = (long) new_মান;
    sm.stillDragging = TRUE;

    write_port(gui.vdcmp, Vim_বার্তা::ScrollBar, &sm, আকার(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    শূন্য
VimScrollBar::MouseUp(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_add(&scroll_Event_গুনতি, 1);

    কাঠামো Vim_Scroll_Bar_বার্তা sm;

    sm.sb = এই;
    sm.মান = (long) মান_2();
    sm.stillDragging = FALSE;

    write_port(gui.vdcmp, Vim_বার্তা::ScrollBar, &sm, আকার(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::MouseUp(where);
}

    শূন্য
VimScrollBar::Set_মান(দশমিক new_মান)
{
    যদি (new_মান == মান_2())
	ফেরত;

    ignore_মান = new_মান;
    Inherited::Set_মান(new_মান);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(ধ্রুবক VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ধ্রুবক BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ধ্রুবক VimFont &rhs): BFont(rhs)
{
    init();
}

VimFont::~VimFont()
{
}

    শূন্য
VimFont::init()
{
    পরবর্তী = ফাঁকা;
    refcount = 1;
    name = ফাঁকা;
}

// ---------------- VimDialog ----------------

#যদি defined(FEAT_GUI_DIALOG)

ধ্রুবক unsigned পূর্ণসংখ্যা  k_Vim_Dialog_Button_বার্তা = 'VMDB';
ধ্রুবক unsigned পূর্ণসংখ্যা  kVimDialogIconStripeWidth = 30;
ধ্রুবক unsigned পূর্ণসংখ্যা  kVimDialogButtonsSpacingX = 9;
ধ্রুবক unsigned পূর্ণসংখ্যা  kVimDialogButtonsSpacingY = 4;
ধ্রুবক unsigned পূর্ণসংখ্যা  kVimDialogSpacingX = 6;
ধ্রুবক unsigned পূর্ণসংখ্যা  kVimDialogSpacingY = 10;
ধ্রুবক unsigned পূর্ণসংখ্যা  kVimDialogMinimalWidth  = 310;
ধ্রুবক unsigned পূর্ণসংখ্যা  kVimDialogMinimalHeight = 75;
ধ্রুবক BRect	    kDefaultRect =
BRect(0, 0, kVimDialogMinimalWidth, kVimDialogMinimalHeight);

VimDialog::VimDialog(পূর্ণসংখ্যা type, ধ্রুবক অক্ষর *title, ধ্রুবক অক্ষর *message,
	ধ্রুবক অক্ষর *buttons, পূর্ণসংখ্যা dfltbutton, ধ্রুবক অক্ষর *textfield, পূর্ণসংখ্যা ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_মান(dfltbutton)
    , fMessageView(ফাঁকা)
    , f_প্রবেশ_Control(ফাঁকা)
    , f_প্রবেশ_মান(textfield)
{
    //	master view
    VimDialog::View* view = নতুন VimDialog::View(Bounds());
    যদি (view == ফাঁকা)
	ফেরত;

    যদি (title == ফাঁকা)
	SetTitle("Vim " VIM_VERSION_MEDIUM);

    AddChild(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    দশমিক maxButtonWidth  = 0;
    দশমিক maxButtonHeight = 0;
    দশমিক buttonsWidth	  = 0;
    দশমিক buttonsHeight   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    করো
    {
	int32 end = strButtons.FindFirst('\n');
	যদি (end != B_ERROR)
	    strButtons.SetByteAt(end, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->AddChild(button);
	fButtonsList.AddItem(button);

	maxButtonWidth	= max_c(maxButtonWidth,  button->Bounds().Width());
	maxButtonHeight = max_c(maxButtonHeight, button->Bounds().Height());
	buttonsWidth   += button->Bounds().Width();
	buttonsHeight  += button->Bounds().Height();

	যদি (end == B_ERROR)
	    বিরতি;

	strButtons.Remove(0, end + 1);
    } যখন (সত্য);

    int32 buttons_গুনতি = fButtonsList.গুনতি_বস্তুসমূহ();
    buttonsWidth      += kVimDialogButtonsSpacingX * (buttons_গুনতি - 1);
    buttonsHeight     += kVimDialogButtonsSpacingY * (buttons_গুনতি - 1);
    দশমিক dialogWidth  = buttonsWidth + kVimDialogIconStripeWidth +
	kVimDialogSpacingX * 2;
    দশমিক dialogHeight = maxButtonHeight + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    যৌক্তিক vertical = (vim_strchr(p_go, GO_VERTICAL) != ফাঁকা) ||
	dialogWidth >= gui.vimWindow->Bounds().Width();
    যদি (vertical) {
	dialogWidth  -= buttonsWidth;
	dialogWidth  += maxButtonWidth;
	dialogHeight -= maxButtonHeight;
	dialogHeight += buttonsHeight;
    }

    dialogWidth  = max_c(dialogWidth,  kVimDialogMinimalWidth);

    //	message view
    BRect rect(0, 0, dialogWidth, 0);
    rect.বাম  += kVimDialogIconStripeWidth + 16 + kVimDialogSpacingX;
    rect.শীর্ষ_2   += kVimDialogSpacingY;
    rect.ডান -= kVimDialogSpacingX;
    rect.bottom = rect.শীর্ষ_2;
    fMessageView = নতুন BTextView(rect, "_tv_", rect.OffsetByCopy(B_ORIGIN),
	    B_FOLLOW_বাম | B_FOLLOW_শীর্ষ, B_WILL_DRAW);

    fMessageView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color textColor = ui_color(B_PANEL_TEXT_COLOR);
    fMessageView->SetFontAndColor(be_plain_font, B_FONT_ALL, &textColor);
    fMessageView->SetText(message);
    fMessageView->MakeEditable(মিথ্যা);
    fMessageView->MakeSelectable(মিথ্যা);
    fMessageView->SetWordWrap(সত্য);
    AddChild(fMessageView);

    দশমিক messageHeight = fMessageView->TextHeight(0, fMessageView->গুনতি_সারিগুলি());
    fMessageView->ResizeBy(0, messageHeight);
    fMessageView->SetTextRect(BRect(0, 0, rect.Width(), messageHeight));

    dialogHeight += messageHeight;

    //	input view
    যদি (f_প্রবেশ_মান != ফাঁকা) {
	rect.শীর্ষ_2     =
	    rect.bottom += messageHeight + kVimDialogSpacingY;
	f_প্রবেশ_Control = নতুন BTextControl(rect, "_iv_", ফাঁকা, f_প্রবেশ_মান, ফাঁকা,
		B_FOLLOW_বাম | B_FOLLOW_শীর্ষ, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_প্রবেশ_Control->TextView()->SetText(f_প্রবেশ_মান);
	f_প্রবেশ_Control->TextView()->SetWordWrap(মিথ্যা);
	AddChild(f_প্রবেশ_Control);

	দশমিক width = 0.f, height = 0.f;
	f_প্রবেশ_Control->Get_Preferred_আকার(&width, &height);
	f_প্রবেশ_Control->MakeFocus(সত্য);

	dialogHeight += height + kVimDialogSpacingY * 1.5;
    }

    dialogHeight = max_c(dialogHeight, kVimDialogMinimalHeight);

    ResizeTo(dialogWidth, dialogHeight);
    MoveTo((gui.vimWindow->Bounds().Width() - dialogWidth) / 2,
	    (gui.vimWindow->Bounds().Height() - dialogHeight) / 2);

    //	adjust layout of buttons
    দশমিক buttonWidth = max_c(maxButtonWidth, rect.Width() * 0.66);
    BPoint origin(dialogWidth, dialogHeight);
    origin.x -= kVimDialogSpacingX + (vertical ? buttonWidth : buttonsWidth);
    origin.y -= kVimDialogSpacingY + (vertical ? buttonsHeight	: maxButtonHeight);

    জন্য (int32 i = 0 ; i < buttons_গুনতি; i++) {
	BButton *button = (BButton*)fButtonsList.ItemAt(i);
	button->MoveTo(origin);
	যদি (vertical) {
	    origin.y += button->Frame().Height() + kVimDialogButtonsSpacingY;
	    button->ResizeTo(buttonWidth, button->Frame().Height());
	} অন্যথা
	    origin.x += button->Frame().Width() + kVimDialogButtonsSpacingX;

	যদি (dfltbutton == i + 1) {
	    button->MakeDefault(সত্য);
	    button->MakeFocus(f_প্রবেশ_Control == ফাঁকা);
	}
    }
}

VimDialog::~VimDialog()
{
    যদি (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    পূর্ণসংখ্যা
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    যদি (fDialogSem < B_OK) {
	Quit();
	ফেরত f_Dialog_মান;
    }

    Show();

    যখন (acquire_sem(fDialogSem) == B_INTERRUPTED);

    পূর্ণসংখ্যা ret_মান = f_Dialog_মান;
    যদি (f_প্রবেশ_মান != ফাঁকা)
	vim_strncpy((char_u*)f_প্রবেশ_মান, (char_u*)f_প্রবেশ_Control->Text(), IOSIZE - 1);

    যদি (Lock())
	Quit();

    ফেরত ret_মান;
}

শূন্য VimDialog::MessageReceived(BMessage *বার্তা)
{
    int32 which = 0;
    যদি (বার্তা->what != k_Vim_Dialog_Button_বার্তা ||
	    বার্তা->FindInt32("which", &which) != B_OK)
	ফেরত BWindow::MessageReceived(বার্তা);

    f_Dialog_মান = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, ধ্রুবক অক্ষর* label)
{
    BMessage *message = নতুন BMessage(k_Vim_Dialog_Button_বার্তা);
    message->AddInt32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = নতুন BButton(rect, name.String(), label, message,
	    B_FOLLOW_ডান | B_FOLLOW_BOTTOM);

    দশমিক width = 0.f, height = 0.f;
    button->Get_Preferred_আকার(&width, &height);
    button->ResizeTo(width, height);

    ফেরত button;
}

VimDialog::View::View(BRect frame)
    :	BView(frame, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(ফাঁকা)
{
    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    delete fIconBitmap;
}

শূন্য VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.ডান = kVimDialogIconStripeWidth;
    Set_উপরের_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    যদি (fIconBitmap == ফাঁকা)
	ফেরত;

    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

শূন্য VimDialog::View::InitIcon(int32 type)
{
    যদি (type == VIM_GENERIC)
	ফেরত;

    BPath path;
    status_t status = find_directory(B_BEOS_SERVERS_DIRECTORY, &path);
    যদি (status != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(status));
	ফেরত;
    }

    path.Append("app_server");

    BFile file(path.Path(), O_RDONLY);
    যদি (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	ফেরত;
    }

    BResources resources(&file);
    যদি (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	ফেরত;
    }

    ধ্রুবক অক্ষর *name = "";
    নির্বাচন(type) {
	ক্ষেত্র VIM_ERROR:	    name = "stop"; বিরতি;
	ক্ষেত্র VIM_WARNING:   name = "warn"; বিরতি;
	ক্ষেত্র VIM_INFO:	    name = "info"; বিরতি;
	ক্ষেত্র VIM_QUESTION:  name = "idea"; বিরতি;
	পূর্বনির্ধারিত: ফেরত;
    }

    int32 icon_আকার_2 = 32;
    fIconBitmap = নতুন BBitmap(BRect(0, 0, icon_আকার_2 - 1, icon_আকার_2 - 1), 0, B_RGBA32);
    যদি (fIconBitmap == ফাঁকা || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == ফাঁকা) ? "null" : strerror(fIconBitmap->InitCheck()));
	ফেরত;
    }

    size_t আকার_2 = 0;
    ধ্রুবক uint8* icon_তথ্য = ফাঁকা;
    //	try vector icon first?
    icon_তথ্য = (ধ্রুবক uint8*)resources.LoadResource(B_VECTOR_ICON_TYPE, name, &আকার_2);
    যদি (icon_তথ্য != ফাঁকা && BIconUtils::GetVectorIcon(icon_তথ্য, আকার_2, fIconBitmap) == B_OK)
	ফেরত;

    //	try bitmap icon now
    icon_তথ্য = (ধ্রুবক uint8*)resources.LoadResource(B_LARGE_ICON_TYPE, name, &আকার_2);
    যদি (icon_তথ্য == ফাঁকা) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	delete fIconBitmap;
	fIconBitmap = ফাঁকা;
	ফেরত;
    }

    যদি (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(icon_তথ্য, icon_আকার_2, icon_আকার_2, icon_আকার_2, fIconBitmap);
}

ধ্রুবক unsigned পূর্ণসংখ্যা  k_Vim_Dialog_OKButton_বার্তা = 'FDOK';
ধ্রুবক unsigned পূর্ণসংখ্যা  k_Vim_Dialog_Cancel_Button_বার্তা = 'FDCN';
ধ্রুবক unsigned পূর্ণসংখ্যা  k_Vim_Dialog_আকার_প্রবেশ_বার্তা = 'SICH';
ধ্রুবক unsigned পূর্ণসংখ্যা  k_Vim_Dialog_Family_Select_বার্তা = 'MSFM';
ধ্রুবক unsigned পূর্ণসংখ্যা  k_Vim_Dialog_Style_Select_বার্তা = 'MSST';
ধ্রুবক unsigned পূর্ণসংখ্যা  k_Vim_Dialog_আকার_Select_বার্তা = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, দশমিক* আকার_2)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fStatus(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_মান(মিথ্যা)
    , fFamily(family)
    , fStyle(style)
    , f_আকার(আকার_2)
    , f_Font_আকার(*আকার_2)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_প্রবেশ(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_দৈর্ঘ্য);
    strncpy(fFontStyle, *style, B_FONT_STYLE_দৈর্ঘ্য);

    //	"client" area view
    BBox *clientBox = নতুন BBox(Bounds(), B_EMPTY_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    AddChild(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.বাম_শীর্ষ(), RC.বাম_শীর্ষ());

    //	at first create all controls
    fPreview = নতুন BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->AddChild(fPreview);

    BBox* boxDivider = নতুন BBox(rc, B_EMPTY_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->AddChild(boxDivider);

    BStringView *labelFamily = নতুন BStringView(rc, "labelFamily", "Family:");
    clientBox->AddChild(labelFamily);
    labelFamily->ResizeToPreferred();

    BStringView *labelStyle = নতুন BStringView(rc, "labelStyle", "Style:");
    clientBox->AddChild(labelStyle);
    labelStyle->ResizeToPreferred();

    BStringView *label_আকার = নতুন BStringView(rc, "labelSize", "Size:");
    clientBox->AddChild(label_আকার);
    label_আকার->ResizeToPreferred();

    fFamiliesList = নতুন BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = নতুন BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_বাম_ডান, 0, মিথ্যা, সত্য);
    clientBox->AddChild(scrollFamilies);

    fStylesList= নতুন BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = নতুন BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_বাম_ডান, 0, মিথ্যা, সত্য);
    clientBox->AddChild(scrollStyles);

    f_Sizes_প্রবেশ = নতুন BTextControl(rc, "inputSize", ফাঁকা, "???",
	    নতুন BMessage(k_Vim_Dialog_আকার_প্রবেশ_বার্তা));
    clientBox->AddChild(f_Sizes_প্রবেশ);
    f_Sizes_প্রবেশ->ResizeToPreferred();

    fSizesList = নতুন BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = নতুন BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_বাম_ডান, 0, মিথ্যা, সত্য);
    clientBox->AddChild(scrollSizes);

    BButton *buttonOK = নতুন BButton(rc, "buttonOK", "OK",
			নতুন BMessage(k_Vim_Dialog_OKButton_বার্তা));
    clientBox->AddChild(buttonOK);
    buttonOK->ResizeToPreferred();

    BButton *buttonCancel = নতুন BButton(rc, "buttonCancel", "Cancel",
			নতুন BMessage(k_Vim_Dialog_Cancel_Button_বার্তা));
    clientBox->AddChild(buttonCancel);
    buttonCancel->ResizeToPreferred();

    //	layout controls
    দশমিক সারি_Height = labelFamily->Bounds().Height();
    দশমিক previewHeight = সারি_Height * 3;
    দশমিক offsetYLabels = previewHeight + kVimDialogSpacingY;
    দশমিক offsetYLists = offsetYLabels + সারি_Height + kVimDialogSpacingY / 2;
    দশমিক offsetYSizes = offsetYLists + f_Sizes_প্রবেশ->Bounds().Height() + kVimDialogSpacingY / 2;
    দশমিক listsHeight = সারি_Height * 9;
    দশমিক offsetYButtons = offsetYLists + listsHeight +  kVimDialogSpacingY;
    দশমিক maxControlsHeight = offsetYButtons + buttonOK->Bounds().Height();
    দশমিক familiesWidth = labelFamily->Bounds().Width() * 5;
    দশমিক offsetXStyles = familiesWidth + kVimDialogSpacingX;
    দশমিক stylesWidth = labelStyle->Bounds().Width() * 4;
    দশমিক offsetXSizes = offsetXStyles + stylesWidth + kVimDialogSpacingX;
    দশমিক sizesWidth = label_আকার->Bounds().Width() * 2;
    দশমিক maxControlsWidth = offsetXSizes + sizesWidth;

    ResizeTo(maxControlsWidth + kVimDialogSpacingX * 2,
	maxControlsHeight + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vimWindow->Frame();
    MoveTo(rcVim.বাম + (rcVim.Width() - Frame().Width()) / 2,
	    rcVim.শীর্ষ_2 + (rcVim.Height() - Frame().Height()) / 2);

    fPreview->ResizeTo(maxControlsWidth, previewHeight);
    fPreview->SetAlignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, previewHeight + kVimDialogSpacingY / 2);
    boxDivider->ResizeTo(maxControlsWidth, 1.f);

    labelFamily->MoveBy(0.f, offsetYLabels);
    labelStyle->MoveBy(offsetXStyles, offsetYLabels);
    label_আকার->MoveBy(offsetXSizes, offsetYLabels);

    //	text control alignment issues
    দশমিক insetX = f_Sizes_প্রবেশ->TextView()->Bounds().Width() - f_Sizes_প্রবেশ->Bounds().Width();
    দশমিক insetY = f_Sizes_প্রবেশ->TextView()->Bounds().Width() - f_Sizes_প্রবেশ->Bounds().Width();

    scrollFamilies->MoveBy(0.f, offsetYLists);
    scrollStyles->MoveBy(offsetXStyles, offsetYLists);
    f_Sizes_প্রবেশ->MoveBy(offsetXSizes + insetX / 2, offsetYLists + insetY / 2);
    scrollSizes->MoveBy(offsetXSizes, offsetYSizes);

    f_Sizes_প্রবেশ->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->ResizeTo(familiesWidth, listsHeight);
    scrollStyles->ResizeTo(stylesWidth, listsHeight);
    f_Sizes_প্রবেশ->ResizeTo(sizesWidth, f_Sizes_প্রবেশ->Bounds().Height());
    scrollSizes->ResizeTo(sizesWidth,
	    listsHeight - (offsetYSizes - offsetYLists));

    buttonOK->MoveBy(maxControlsWidth - buttonOK->Bounds().Width(), offsetYButtons);
    buttonCancel->MoveBy(maxControlsWidth - buttonOK->Bounds().Width()
	    - buttonCancel->Bounds().Width() - kVimDialogSpacingX, offsetYButtons);

    //	fill lists
    পূর্ণসংখ্যা sel_সূচক = -1;
    পূর্ণসংখ্যা গুনতি = গুনতি_font_families();
    জন্য (পূর্ণসংখ্যা i = 0; i < গুনতি; i++) {
	font_family family;
	যদি (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->AddItem(নতুন BStringItem((ধ্রুবক অক্ষর*)family));
	    যদি (strncmp(family, fFontFamily, B_FONT_FAMILY_দৈর্ঘ্য) == 0)
		sel_সূচক = i;
	}
    }

    যদি (sel_সূচক >= 0) {
	fFamiliesList->Select(sel_সূচক);
	fFamiliesList->ScrollToSelection();
    }

    _UpdateFontStyles();

    sel_সূচক = -1;
    জন্য (পূর্ণসংখ্যা আকার_2 = 8, সূচক = 0; আকার_2 <= 18; আকার_2++, সূচক++) {
	BString str;
	str << আকার_2;
	fSizesList->AddItem(নতুন BStringItem(str));
	যদি (আকার_2 == f_Font_আকার)
	    sel_সূচক = সূচক;

    }

    যদি (sel_সূচক >= 0) {
	fSizesList->Select(sel_সূচক);
	fSizesList->ScrollToSelection();
    }

    fFamiliesList->SetSelectionMessage(নতুন BMessage(k_Vim_Dialog_Family_Select_বার্তা));
    fStylesList->SetSelectionMessage(নতুন BMessage(k_Vim_Dialog_Style_Select_বার্তা));
    fSizesList->SetSelectionMessage(নতুন BMessage(k_Vim_Dialog_আকার_Select_বার্তা));
    f_Sizes_প্রবেশ->SetModificationMessage(নতুন BMessage(k_Vim_Dialog_আকার_প্রবেশ_বার্তা));

    Update_আকার_প্রবেশ_Preview();
    _UpdateFontPreview();

    fStatus = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    যদি (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    শূন্য
VimSelectFontDialog::_CleanList(BListView* list)
{
    যখন (0 < list->গুনতি_বস্তুসমূহ())
	delete (dynamic_cast<BStringItem*>(list->RemoveItem((int32)0)));
}

    যৌক্তিক
VimSelectFontDialog::Go()
{
    যদি (fStatus != B_OK) {
	Quit();
	ফেরত NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    যদি (fDialogSem < B_OK) {
	Quit();
	ফেরত f_Dialog_মান;
    }

    Show();

    যখন (acquire_sem(fDialogSem) == B_INTERRUPTED);

    যৌক্তিক ret_মান = f_Dialog_মান;

    যদি (Lock())
	Quit();

    ফেরত ret_মান;
}


শূন্য VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 sel_সূচক = -1;
    int32 গুনতি = গুনতি_font_styles(fFontFamily);
    জন্য (int32 i = 0; i < গুনতি; i++) {
	font_style style;
	uint32 flags = 0;
	যদি (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->AddItem(নতুন BStringItem((ধ্রুবক অক্ষর*)style));
	    যদি (strncmp(style, fFontStyle, B_FONT_STYLE_দৈর্ঘ্য) == 0)
		sel_সূচক = i;
	}
    }

    যদি (sel_সূচক >= 0) {
	fStylesList->Select(sel_সূচক);
	fStylesList->ScrollToSelection();
    } অন্যথা
	fStylesList->Select(0);
}


শূন্য VimSelectFontDialog::Update_আকার_প্রবেশ_Preview()
{
    অক্ষর buf[10] = {0};
    vim_snprintf(buf, আকার(buf), (অক্ষর*)"%.0f", f_Font_আকার);
    f_Sizes_প্রবেশ->SetText(buf);
}


শূন্য VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.Set_আকার(f_Font_আকার);
    font.SetFamilyAndStyle(fFontFamily, fFontStyle);
    fPreview->SetFont(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_আকার);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (পূর্ণসংখ্যা)f_Font_আকার << " pt.";
    fPreview->SetText(str);
}


    যৌক্তিক
VimSelectFontDialog::_UpdateFromListItem(BListView* list, অক্ষর* text, পূর্ণসংখ্যা text_আকার)
{
    int32 সূচক = list->CurrentSelection();
    যদি (সূচক < 0)
	ফেরত মিথ্যা;
    BStringItem* item = (BStringItem*)list->ItemAt(সূচক);
    যদি (item == ফাঁকা)
	ফেরত মিথ্যা;
    strncpy(text, item->Text(), text_আকার);
    ফেরত সত্য;
}


শূন্য VimSelectFontDialog::MessageReceived(BMessage *বার্তা)
{
    নির্বাচন (বার্তা->what) {
	ক্ষেত্র k_Vim_Dialog_OKButton_বার্তা:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_দৈর্ঘ্য);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_দৈর্ঘ্য);
	    *f_আকার = f_Font_আকার;
	    f_Dialog_মান = সত্য;
	ক্ষেত্র k_Vim_Dialog_Cancel_Button_বার্তা:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    ফেরত;
	ক্ষেত্র B_চাবি_UP:
	    {
		int32 চাবি = 0;
		যদি (বার্তা->FindInt32("raw_char", &চাবি) == B_OK
			&& চাবি == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    বিরতি;

	ক্ষেত্র k_Vim_Dialog_Family_Select_বার্তা:
	    যদি (_UpdateFromListItem(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_দৈর্ঘ্য)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    বিরতি;
	ক্ষেত্র k_Vim_Dialog_Style_Select_বার্তা:
	    যদি (_UpdateFromListItem(fStylesList,
		    fFontStyle, B_FONT_STYLE_দৈর্ঘ্য))
		_UpdateFontPreview();
	    বিরতি;
	ক্ষেত্র k_Vim_Dialog_আকার_Select_বার্তা:
	    {
		অক্ষর buf[10] = {0};
		যদি (_UpdateFromListItem(fSizesList, buf, আকার(buf))) {
		    দশমিক আকার_2 = atof(buf);
		    যদি (আকার_2 > 0.f) {
			f_Font_আকার = আকার_2;
			Update_আকার_প্রবেশ_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    বিরতি;
	ক্ষেত্র k_Vim_Dialog_আকার_প্রবেশ_বার্তা:
	    {
		দশমিক আকার_2 = atof(f_Sizes_প্রবেশ->Text());
		যদি (আকার_2 > 0.f) {
		    f_Font_আকার = আকার_2;
		    _UpdateFontPreview();
		}
	    }
	    বিরতি;
	পূর্বনির্ধারিত:
	    বিরতি;
    }
    ফেরত BWindow::MessageReceived(বার্তা);
}

#endif // FEAT_GUI_DIALOG

#ifdef FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
স্থির BMessage * MenuMessage(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect frame, ধ্রুবক অক্ষর *name) :
    BBox(frame, name, B_FOLLOW_বাম | B_FOLLOW_শীর্ষ, B_WILL_DRAW | B_FRAME_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 গুনতি = fButtonsList.গুনতি_বস্তুসমূহ();
    জন্য (int32 i = 0; i < গুনতি; i++)
	delete (BPictureButton*)fButtonsList.ItemAt(i);
    fButtonsList.MakeEmpty();

    delete normalButtonsBitmap;
    delete grayedButtonsBitmap;
    normalButtonsBitmap    = ফাঁকা;
    grayedButtonsBitmap  = ফাঁকা;
}

    শূন্য
VimToolbar::AttachedToWindow()
{
    BBox::AttachedToWindow();

    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    দশমিক
VimToolbar::ToolbarHeight() ধ্রুবক
{
    দশমিক আকার_2 = ফাঁকা == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
    ফেরত আকার_2 + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    যৌক্তিক
VimToolbar::ModifyBitmapToGrayed(BBitmap *bitmap)
{
    দশমিক height = bitmap->Bounds().Height();
    দশমিক width  = bitmap->Bounds().Width();

    rgb_color *bits = (rgb_color*)bitmap->Bits();
    int32 pixels = bitmap->Bits_দৈর্ঘ্য() / 4;
    জন্য (int32 i = 0; i < pixels; i++) {
	bits[i].red = bits[i].green =
	bits[i].blue = ((uint32)bits[i].red + bits[i].green + bits[i].blue) / 3;
	bits[i].alpha /= 4;
    }

    ফেরত সত্য;
}

    যৌক্তিক
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    normalButtonsBitmap = LoadVimBitmap("builtin-tools.png");
    যদি (normalButtonsBitmap == ফাঁকা)
	//  customized not found? dig application resources for "builtin-tools" one
	normalButtonsBitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    যদি (normalButtonsBitmap == ফাঁকা)
	ফেরত মিথ্যা;

    BMessage archive;
    normalButtonsBitmap->Archive(&archive);

    grayedButtonsBitmap = নতুন BBitmap(&archive);
    যদি (grayedButtonsBitmap == ফাঁকা)
	ফেরত মিথ্যা;

    //	modify grayed bitmap
    ModifyBitmapToGrayed(grayedButtonsBitmap);

    ফেরত সত্য;
}

BBitmap *VimToolbar::LoadVimBitmap(ধ্রুবক অক্ষর* fileName)
{
    BBitmap *bitmap = ফাঁকা;

    পূর্ণসংখ্যা mustfree = 0;
    char_u* runtimePath = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    যদি (runtimePath != ফাঁকা && fileName != ফাঁকা) {
	BString strPath((অক্ষর*)runtimePath);
	strPath << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(strPath.String());
    }

    যদি (mustfree)
	vim_free(runtimePath);

    ফেরত bitmap;
}

    যৌক্তিক
VimToolbar::GetPictureFromBitmap(BPicture *pictureTo, int32 সূচক, BBitmap *bitmapFrom, যৌক্তিক pressed)
{
    দশমিক আকার_2 = bitmapFrom->Bounds().Height() + 1.;

    BView view(BRect(0, 0, আকার_2, আকার_2), "", 0, 0);

    AddChild(&view);
    view.BeginPicture(pictureTo);

    view.Set_উপরের_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.SetDrawingMode(B_OP_OVER);

    BRect source(0, 0, আকার_2 - 1, আকার_2 - 1);
    BRect destination(source);

    source.OffsetBy(আকার_2 * সূচক, 0);
    destination.OffsetBy(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, source, destination);

    যদি (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	আকার_2 += ButtonMargin * 2 - 1;
	view.Begin_সারি_Array(4);
	view.Add_সারি(BPoint(0, 0),	 BPoint(আকার_2, 0),    shadowColor);
	view.Add_সারি(BPoint(আকার_2, 0),	 BPoint(আকার_2, আকার_2), shineColor);
	view.Add_সারি(BPoint(আকার_2, আকার_2), BPoint(0, আকার_2),    shineColor);
	view.Add_সারি(BPoint(0, আকার_2),	 BPoint(0, 0),	     shadowColor);
	view.End_সারি_Array();
    }

    view.EndPicture();
    RemoveChild(&view);

    ফেরত সত্য;
}

    যৌক্তিক
VimToolbar::AddButton(int32 সূচক, vimmenu_T *menu)
{
    BPictureButton *button = ফাঁকা;
    যদি (!menu_is_separator(menu->name)) {
	দশমিক আকার_2 = normalButtonsBitmap ?
	    normalButtonsBitmap->Bounds().Height() + 1. + ButtonMargin * 2 : 18.;
	BRect frame(0, 0, আকার_2, আকার_2);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture pictureGray;

	যদি (menu->iconfile == ফাঁকা && menu->iconidx >= 0 && normalButtonsBitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, normalButtonsBitmap, সত্য);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, normalButtonsBitmap, মিথ্যা);
	    GetPictureFromBitmap(&pictureGray, menu->iconidx, grayedButtonsBitmap, মিথ্যা);
	} অন্যথা {

	    char_u বাফার[MAXPATHL] = {0};
	    BBitmap *bitmap = ফাঁকা;

	    যদি (menu->iconfile) {
		gui_find_iconfile(menu->iconfile, বাফার, (অক্ষর*)"png");
		bitmap = BTranslationUtils::GetBitmap((অক্ষর*)বাফার);
	    }

	    যদি (bitmap == ফাঁকা && gui_find_bitmap(menu->name, বাফার, (অক্ষর*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((অক্ষর*)বাফার);

	    যদি (bitmap == ফাঁকা)
		bitmap = নতুন BBitmap(BRect(0, 0, আকার_2, আকার_2), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, সত্য);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, মিথ্যা);
	    ModifyBitmapToGrayed(bitmap);
	    GetPictureFromBitmap(&pictureGray, 0, bitmap, মিথ্যা);

	    delete bitmap;
	}

	button = নতুন BPictureButton(frame, (অক্ষর*)menu->name,
		    &pictureOff, &pictureOn, MenuMessage(menu));

	button->SetDisabledOn(&pictureGray);
	button->SetDisabledOff(&pictureGray);

	button->SetTarget(gui.vimTextArea);

	AddChild(button);

	menu->button = button;
    }

    যৌক্তিক ফলাফল = fButtonsList.AddItem(button, সূচক);
    InvalidateLayout();
    ফেরত ফলাফল;
}

    যৌক্তিক
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    যদি (menu->button) {
	যদি (fButtonsList.RemoveItem(menu->button)) {
	    delete menu->button;
	    menu->button = ফাঁকা;
	}
    }
    ফেরত সত্য;
}

    যৌক্তিক
VimToolbar::GrayButton(vimmenu_T *menu, পূর্ণসংখ্যা grey)
{
    যদি (menu->button) {
	int32 সূচক = fButtonsList.সূচক_Of(menu->button);
	যদি (সূচক >= 0)
	    menu->button->SetEnabled(grey ? মিথ্যা : সত্য);
    }
    ফেরত সত্য;
}

    শূন্য
VimToolbar::InvalidateLayout()
{
    int32 offset = ToolbarMargin;
    int32 গুনতি = fButtonsList.গুনতি_বস্তুসমূহ();
    জন্য (int32 i = 0; i < গুনতি; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.ItemAt(i);
	যদি (button) {
	    button->MoveTo(offset, ToolbarMargin);
	    offset += button->Bounds().Width() + ToolbarMargin;
	} অন্যথা
	    offset += ToolbarMargin * 3;
    }
}

#endif /*FEAT_TOOLBAR*/

#যদি defined(FEAT_GUI_TABLINE)

    দশমিক
Vim_Tab_সারি::TablineHeight() ধ্রুবক
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    ফেরত TabHeight(); //  + ToolbarMargin;
}

শূন্য
Vim_Tab_সারি::MouseDown(BPoint point)
{
    যদি (!gui_mch_showing_tabline())
	ফেরত;

    BMessage *m = Window()->CurrentMessage();
    প্রতিজ্ঞা(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    int32 clicks = 0;
    m->FindInt32("clicks", &clicks);

    পূর্ণসংখ্যা সূচক = 0; //  0 means here - no tab found
    জন্য (পূর্ণসংখ্যা i = 0; i < গুনতি_Tabs(); i++) {
	যদি (TabFrame(i).Contains(point)) {
	    সূচক = i + 1; //  indexes are 1-based
	    বিরতি;
	}
    }

    পূর্ণসংখ্যা event = -1;

    যদি ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    অন্যথা যদি (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = সূচক > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    অন্যথা যদি (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPopUpMenu* বের_করো_Up_Menu = নতুন BPopUpMenu("tabLineContextMenu", মিথ্যা, মিথ্যা);
	বের_করো_Up_Menu->AddItem(নতুন BMenuItem(_("Close tabi R"), নতুন BMessage(TABLINE_MENU_CLOSE)));
	বের_করো_Up_Menu->AddItem(নতুন BMenuItem(_("New tab    T"), নতুন BMessage(TABLINE_MENU_NEW)));
	বের_করো_Up_Menu->AddItem(নতুন BMenuItem(_("Open tab..."), নতুন BMessage(TABLINE_MENU_OPEN)));

	ConvertToScreen(&point);
	BMenuItem* item = বের_করো_Up_Menu->Go(point);
	যদি (item != ফাঁকা) {
	    event = item->Command();
	}

	delete বের_করো_Up_Menu;

    } অন্যথা {
	//  default processing
	BTabView::MouseDown(point);
	ফেরত;
    }

    যদি (event < 0)
	ফেরত;

    Vim_Tabline_Menu_বার্তা tmm;
    tmm.সূচক = সূচক;
    tmm.event = event;
    write_port(gui.vdcmp, Vim_বার্তা::TablineMenu, &tmm, আকার(tmm));
}

শূন্য
Vim_Tab_সারি::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_সারি *tab_সারি = gui.vimForm->Tab_সারি();
    যদি (tab_সারি != ফাঁকা) {

	int32 i = 0;
	জন্য (; i < tab_সারি->গুনতি_Tabs(); i++)
	    যদি (এই == tab_সারি->TabAt(i))
		বিরতি;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	যদি (i < tab_সারি->গুনতি_Tabs()) {
	    Vim_Tabline_বার্তা tm;
	    tm.সূচক = i + 1;
	    write_port(gui.vdcmp, Vim_বার্তা::Tabline, &tm, আকার(tm));
	}
    }
}

#endif //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
স্থির অক্ষর appsig[] = "application/x-vnd.Haiku-Vim-8";
চাবি_map *চাবি_Map;
অক্ষর *চাবি_Map_Chars;
পূর্ণসংখ্যা main_exitcode = 127;

    status_t
gui_haiku_process_event(bigtime_t timeout)
{
    কাঠামো Vim_বার্তা vm;
    int32 what;
    ssize_t আকার_2;

    আকার_2 = read_port_etc(gui.vdcmp, &what, &vm, আকার(vm),
	    B_TIMEOUT, timeout);

    যদি (আকার_2 >= 0) {
	নির্বাচন (what) {
	    ক্ষেত্র Vim_বার্তা::চাবি_2:
		{
		    char_u *string = vm.u.চাবি_2.chars;
		    পূর্ণসংখ্যা len = vm.u.চাবি_2.দৈর্ঘ্য;
		    যদি (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_প্রবেশ_buf();
			got_int = TRUE;
		    }

		    যদি (vm.u.চাবি_2.csi_escape)
#ifndef FEAT_MBYTE_IME
		    {
			পূর্ণসংখ্যা	i;
			char_u	buf[2];

			জন্য (i = 0; i < len; ++i)
			{
			    add_to_প্রবেশ_buf(string + i, 1);
			    যদি (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (পূর্ণসংখ্যা)KE_CSI;
				add_to_প্রবেশ_buf(buf, 2);
			    }
			}
		    }
#অন্যথা
			add_to_প্রবেশ_buf_csi(string, len);
#endif
		    অন্যথা
			add_to_প্রবেশ_buf(string, len);
		}
		বিরতি;
	    ক্ষেত্র Vim_বার্তা::Resize:
		gui_resize_shell(vm.u.New_আকার.width, vm.u.New_আকার.height);
		বিরতি;
	    ক্ষেত্র Vim_বার্তা::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_গুনতি =
			atomic_add(&vm.u.Scroll.sb->scroll_Event_গুনতি, -1);
		    যদি (old_গুনতি <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.মান, vm.u.Scroll.stillDragging);
		}
		বিরতি;
#যদি defined(FEAT_MENU)
	    ক্ষেত্র Vim_বার্তা::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		বিরতি;
#endif
	    ক্ষেত্র Vim_বার্তা::Mouse:
		{
		    int32 old_গুনতি;
		    যদি (vm.u.Mouse.button == MOUSE_DRAG)
			old_গুনতি =
			    atomic_add(&gui.vimTextArea->mouse_Drag_Event_গুনতি, -1);
		    অন্যথা
			old_গুনতি = 0;
		    যদি (old_গুনতি <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		বিরতি;
	    ক্ষেত্র Vim_বার্তা::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		বিরতি;
	    ক্ষেত্র Vim_বার্তা::Focus:
		gui.in_focus = vm.u.Focus.active;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		যদি (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		বিরতি;
	    ক্ষেত্র Vim_বার্তা::Refs:
		::RefsReceived(vm.u.Refs.message, vm.u.Refs.changedir);
		বিরতি;
	    ক্ষেত্র Vim_বার্তা::Tabline:
		send_tabline_event(vm.u.Tabline.সূচক);
		বিরতি;
	    ক্ষেত্র Vim_বার্তা::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.সূচক, vm.u.TablineMenu.event);
		বিরতি;
	    পূর্বনির্ধারিত:
		//  unrecognised message, ignore it
		বিরতি;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    ফেরত আকার_2;
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

    পূর্ণসংখ্যা
vim_lock_screen()
{
    ফেরত !gui.vimWindow || gui.vimWindow->Lock();
}

    শূন্য
vim_unlock_screen()
{
    যদি (gui.vimWindow)
	gui.vimWindow->Unlock();
}

#define RUN_BAPPLICATION_IN_NEW_THREAD	0

#যদি RUN_BAPPLICATION_IN_NEW_THREAD

    int32
run_vimapp(শূন্য *args)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.Run();		    // Run until Quit() called

    ফেরত 0;
}

#অন্যথা

    int32
call_main(শূন্য *args)
{
    কাঠামো MainArgs *ma = (MainArgs *)args;

    ফেরত প্রধান(ma->argc, ma->argv);
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    শূন্য
gui_mch_prepare(
	পূর্ণসংখ্যা	*argc,
	অক্ষর	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    যদি (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(find_thread(ফাঁকা), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_port(B_MAX_PORT_গুনতি, "vim VDCMP");

#যদি RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = spawn_thread(run_vimapp, "vim VimApp",
		tinfo.priority, ফাঁকা);
	যদি (tid >= B_OK) {
	    resume_thread(tid);
	} অন্যথা {
	    getout(1);
	}
#অন্যথা
	MainArgs ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.priority, &ma);
	যদি (tid >= B_OK) {
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
	    (শূন্য)wait_for_thread(tid, &dummy_exitcode);

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
    যদি (!isatty(0)) {
	কাঠামো stat stat_stdin, stat_dev_null;

	যদি (fstat(0, &stat_stdin) == -1 ||
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
    পূর্ণসংখ্যা
gui_mch_init_check(শূন্য)
{
    ফেরত OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    পূর্ণসংখ্যা
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_back_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    gui.scrollbar_width = (পূর্ণসংখ্যা) B_V_SCROLL_BAR_WIDTH;
    gui.scrollbar_height = (পূর্ণসংখ্যা) B_H_SCROLL_BAR_HEIGHT;
#ifdef FEAT_MENU
    gui.menu_height = 19;   //	initial guess -
    //	correct for my default settings
#endif
    gui.border_offset = 3;  //	coordinates are inside window borders

    যদি (gui.vdcmp < B_OK)
	ফেরত FAIL;
    get_চাবি_map(&চাবি_Map, &চাবি_Map_Chars);

    gui.vimWindow = নতুন VimWindow();	// hidden and locked
    যদি (!gui.vimWindow)
	ফেরত FAIL;

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

    ফেরত OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    শূন্য
gui_mch_new_colors()
{
    rgb_color rgb = GUI_TO_RGB(gui.back_pixel);

    যদি (gui.vimWindow->Lock()) {
	gui.vimForm->SetViewColor(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vimWindow->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    পূর্ণসংখ্যা
gui_mch_open()
{
    যদি (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    যদি (gui.vimWindow->Lock()) {
	gui.vimWindow->Show();
	gui.vimWindow->Unlock();
	ফেরত OK;
    }

    ফেরত FAIL;
}

    শূন্য
gui_mch_exit(পূর্ণসংখ্যা vim_exitcode)
{
    যদি (gui.vimWindow) {
	thread_id tid = gui.vimWindow->Thread();
	gui.vimWindow->Lock();
	gui.vimWindow->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_port(gui.vdcmp);
#যদি !RUN_BAPPLICATION_IN_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#endif
    যদি (gui.vimApp) {
	VimTextAreaView::guiBlankMouse(মিথ্যা);

	main_exitcode = vim_exitcode;
#যদি RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->Lock();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#অন্যথা
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
    পূর্ণসংখ্যা
gui_mch_get_winpos(পূর্ণসংখ্যা *x, পূর্ণসংখ্যা *y)
{
    যদি (gui.vimWindow->Lock()) {
	BRect r;
	r = gui.vimWindow->Frame();
	gui.vimWindow->Unlock();
	*x = (পূর্ণসংখ্যা)r.বাম;
	*y = (পূর্ণসংখ্যা)r.শীর্ষ_2;
	ফেরত OK;
    }
    অন্যথা
	ফেরত FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    শূন্য
gui_mch_set_winpos(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y)
{
    যদি (gui.vimWindow->Lock()) {
	gui.vimWindow->MoveTo(x, y);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
শূন্য
gui_mch_set_shellsize(
	পূর্ণসংখ্যা	width,
	পূর্ণসংখ্যা	height,
	পূর্ণসংখ্যা	min_width,
	পূর্ণসংখ্যা	min_height,
	পূর্ণসংখ্যা	base_width,
	পূর্ণসংখ্যা	base_height,
	পূর্ণসংখ্যা	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    যদি (gui.vimWindow->Lock()) {
	gui.vimWindow->ResizeTo(width - PEN_WIDTH, height - PEN_WIDTH);

	// set size limits
	দশমিক minWidth, maxWidth, minHeight, maxHeight;

	gui.vimWindow->Get_আকার_Limits(&minWidth, &maxWidth,
		&minHeight, &maxHeight);
	gui.vimWindow->Set_আকার_Limits(min_width, maxWidth,
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

শূন্য
gui_mch_get_screen_dimensions(
	পূর্ণসংখ্যা	*screen_w,
	পূর্ণসংখ্যা	*screen_h)
{
    BRect frame;

    {
	BScreen screen(gui.vimWindow);

	যদি (screen.IsValid()) {
	    frame = screen.Frame();
	} অন্যথা {
	    frame.ডান = 640;
	    frame.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (পূর্ণসংখ্যা) frame.ডান - 2 * gui.scrollbar_width - 20;
    *screen_h = (পূর্ণসংখ্যা) frame.bottom - gui.scrollbar_height
#ifdef FEAT_MENU
	- gui.menu_height
#endif
	- 30;
}

শূন্য
gui_mch_set_text_area_pos(
	পূর্ণসংখ্যা	x,
	পূর্ণসংখ্যা	y,
	পূর্ণসংখ্যা	w,
	পূর্ণসংখ্যা	h)
{
    যদি (!gui.vimTextArea)
	ফেরত;

    যদি (gui.vimWindow->Lock()) {
	gui.vimTextArea->MoveTo(x, y);
	gui.vimTextArea->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);

#ifdef FEAT_GUI_TABLINE
	যদি (gui.vimForm->Tab_সারি() != ফাঁকা) {
	    gui.vimForm->Tab_সারি()->ResizeTo(w, gui.vimForm->TablineHeight());
	}
#endif // FEAT_GUI_TABLINE

	gui.vimWindow->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

শূন্য
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	পূর্ণসংখ্যা	flag)
{
    VimScrollBar *vsb = sb->id;
    যদি (gui.vimWindow->Lock()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	যদি (flag) {
	    যদি (vsb->IsHidden()) {
		vsb->Show();
	    }
	} অন্যথা {
	    যদি (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vimWindow->Unlock();
    }
}

শূন্য
gui_mch_set_scrollbar_thumb(
	scrollbar_T *sb,
	পূর্ণসংখ্যা	val,
	পূর্ণসংখ্যা	আকার_2,
	পূর্ণসংখ্যা	max)
{
    যদি (gui.vimWindow->Lock()) {
	VimScrollBar *s = sb->id;
	যদি (max == 0) {
	    s->Set_মান(0);
	    s->SetRange(0.0, 0.0);
	} অন্যথা {
	    s->SetProportion((দশমিক)আকার_2 / (max + 1.0));
	    s->SetSteps(1.0, আকার_2 > 5 ? আকার_2 - 2 : আকার_2);
#ifndef SCROLL_PAST_END	    //	really only defined in gui.c...
	    max = max + 1 - আকার_2;
#endif
	    যদি (max < s->মান_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->Set_মান(val);
		s->SetRange(0.0, max);
	    } অন্যথা {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->SetRange(0.0, max);
		s->Set_মান(val);
	    }
	}
	gui.vimWindow->Unlock();
    }
}

শূন্য
gui_mch_set_scrollbar_pos(
	scrollbar_T *sb,
	পূর্ণসংখ্যা	x,
	পূর্ণসংখ্যা	y,
	পূর্ণসংখ্যা	w,
	পূর্ণসংখ্যা	h)
{
    যদি (gui.vimWindow->Lock()) {
	BRect winb = gui.vimWindow->Bounds();
	দশমিক vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);
	যদি (winb.ডান-(x+w)<w) vsbx = winb.ডান - (w - PEN_WIDTH);
	vsb->MoveTo(vsbx, vsby);
	gui.vimWindow->Unlock();
    }
}

    পূর্ণসংখ্যা
gui_mch_get_scrollbar_xpadding(শূন্য)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ফেরত 0;
}

    পূর্ণসংখ্যা
gui_mch_get_scrollbar_ypadding(শূন্য)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ফেরত 0;
}

শূন্য
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	পূর্ণসংখ্যা	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = নতুন VimScrollBar(sb, posture);
    যদি (gui.vimWindow->Lock()) {
	vsb->SetTarget(gui.vimTextArea);
	vsb->Hide();
	gui.vimForm->AddChild(vsb);
	gui.vimWindow->Unlock();
    }
}

#যদি defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
শূন্য
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    যদি (gui.vimWindow->Lock()) {
	sb->id->RemoveSelf();
	delete sb->id;
	gui.vimWindow->Unlock();
    }
}
#endif

/*
 * Cursor does not flash
 */
    পূর্ণসংখ্যা
gui_mch_is_blink_off(শূন্য)
{
    ফেরত FALSE;
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

স্থির পূর্ণসংখ্যা	blink_state = BLINK_NONE;
স্থির long_u	    blink_waittime = 700;
স্থির long_u	    blink_ontime = 400;
স্থির long_u	    blink_offtime = 250;
স্থির পূর্ণসংখ্যা  blink_timer = 0;

শূন্য
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
    শূন্য
gui_mch_stop_blink(পূর্ণসংখ্যা may_call_gui_update_cursor)
{
    // TODO
    যদি (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    যদি (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    শূন্য
gui_mch_start_blink()
{
    // TODO
    যদি (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    যদি (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
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
পূর্ণসংখ্যা
gui_mch_init_font(
	char_u	    *font_name,
	পূর্ণসংখ্যা	    fontset)
{
    যদি (gui.vimWindow->Lock())
    {
	পূর্ণসংখ্যা rc = gui.vimTextArea->mchInitFont(font_name);
	gui.vimWindow->Unlock();

	ফেরত rc;
    }

    ফেরত FAIL;
}


    পূর্ণসংখ্যা
gui_mch_adjust_charsize()
{
    ফেরত FAIL;
}


    পূর্ণসংখ্যা
gui_mch_font_dialog(font_family* family, font_style* style, দশমিক* আকার_2)
{
#যদি defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = নতুন VimSelectFontDialog(family, style, আকার_2);
    যৌক্তিক ret = dialog->Go();
	delete dialog;
	ফেরত ret;
#অন্যথা
    ফেরত NOFONT;
#endif // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	পূর্ণসংখ্যা	    giveErrorIfMissing)
{
    স্থির VimFont *fontList = ফাঁকা;

    যদি (!gui.in_use)	//  can't do this when GUI not running
	ফেরত NOFONT;

    //	storage for locally modified name;
    ধ্রুবক পূর্ণসংখ্যা buff_আকার = B_FONT_FAMILY_দৈর্ঘ্য + B_FONT_STYLE_দৈর্ঘ্য + 20;
    স্থির অক্ষর font_name[buff_আকার] = {0};
    font_family family = {0};
    font_style	style  = {0};
    দশমিক আকার_2 = 0.f;

    যদি (name == 0 && be_fixed_font == 0) {
	যদি (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	ফেরত NOFONT;
    }

    যৌক্তিক useSelectGUI = মিথ্যা;
    যদি (name != ফাঁকা)
	যদি (STRCMP(name, "*") == 0) {
	    useSelectGUI = সত্য;
	    STRNCPY(font_name, hl_get_font_name(), buff_আকার);
	} অন্যথা
	    STRNCPY(font_name, name, buff_আকার);

    যদি (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	আকার_2 = be_fixed_font->আকার_3();
	vim_snprintf(font_name, buff_আকার,
	    (অক্ষর*)"%s/%s/%.0f", family, style, আকার_2);
    }

    //	replace underscores with spaces
    অক্ষর* end = 0;
    যখন (end = strchr((অক্ষর *)font_name, '_'))
	*end = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    স্থির অক্ষর buff[buff_আকার] = {0};
    STRNCPY(buff, font_name, buff_আকার);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_দৈর্ঘ্য);
    অক্ষর* style_s = strtok(0, "/\0");
    যদি (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_দৈর্ঘ্য);
    আকার_2 = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    যদি (useSelectGUI) {
	যদি (gui_mch_font_dialog(&family, &style, &আকার_2) == NOFONT)
	    ফেরত FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_আকার,
		(অক্ষর*)"%s/%s/%.0f", family, style, আকার_2);
	hl_set_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	যদি (new_p_guifont != ফাঁকা) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    জন্য ( ; *new_p_guifont; ++new_p_guifont)
		যদি (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    জন্য (flp = fontList; flp; flp = flp->পরবর্তী) {
	যদি (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    ফেরত (GuiFont)flp;
	}
    }

    VimFont *font = নতুন VimFont();
    font->name = vim_strsave((char_u*)font_name);

    যদি (গুনতি_font_styles(family) <= 0) {
	যদি (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	delete font;
	ফেরত NOFONT;
    }

    //	Remember font in the static list for later use
    font->পরবর্তী = fontList;
    fontList = font;

    font->SetFamilyAndStyle(family, style);
    যদি (আকার_2 > 0.f)
	font->Set_আকার(আকার_2);

    font->SetSpacing(B_FIXED_SPACING);
    font->SetEncoding(B_UNICODE_UTF8);

    ফেরত (GuiFont)font;
}

/*
 * Set the current text font.
 */
শূন্য
gui_mch_set_font(
	GuiFont	font)
{
    যদি (gui.vimWindow->Lock()) {
	VimFont *vf = (VimFont *)font;

	gui.vimTextArea->SetFont(vf);

	gui.char_width = (পূর্ণসংখ্যা) vf->StringWidth("n");
	font_height fh;
	vf->GetHeight(&fh);
	gui.char_height = (পূর্ণসংখ্যা)(fh.ascent + 0.9999)
	    + (পূর্ণসংখ্যা)(fh.descent + 0.9999) + (পূর্ণসংখ্যা)(fh.leading + 0.9999);
	gui.char_ascent = (পূর্ণসংখ্যা)(fh.ascent + 0.9999);

	gui.vimWindow->Unlock();
    }
}

// XXX TODO This is apparently never called...
শূন্য
gui_mch_free_font(
	GuiFont	font)
{
    যদি (font == NOFONT)
	ফেরত;
    VimFont *f = (VimFont *)font;
    যদি (--f->refcount <= 0) {
	যদি (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	delete f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    যদি (name == ফাঁকা)
	ফেরত ফাঁকা;
    ফেরত vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    পূর্ণসংখ্যা
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
	gui.char_height = (পূর্ণসংখ্যা)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (পূর্ণসংখ্যা)(fh.ascent + 0.5) + p_linespace / 2;
    }
    ফেরত OK;
}

    শূন্য
gui_mch_getmouse(পূর্ণসংখ্যা *x, পূর্ণসংখ্যা *y)
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

    শূন্য
gui_mch_mousehide(পূর্ণসংখ্যা hide)
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
    ফেরত gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
শূন্য
gui_mch_set_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    যদি (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_উপরের_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text background color.
 */
শূন্য
gui_mch_set_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    যদি (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_নিচের_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    শূন্য
gui_mch_set_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

শূন্য
gui_mch_draw_string(
	পূর্ণসংখ্যা	row,
	পূর্ণসংখ্যা	col,
	char_u	*s,
	পূর্ণসংখ্যা	len,
	পূর্ণসংখ্যা	flags)
{
    যদি (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchDrawString(row, col, s, len, flags);
	gui.vimWindow->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(পূর্ণসংখ্যা r, পূর্ণসংখ্যা g, পূর্ণসংখ্যা b)
{
    ফেরত gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
পূর্ণসংখ্যা
gui_mch_haskey(
	char_u	*name)
{
    পূর্ণসংখ্যা i;

    জন্য (i = 0; special_চাবিগুলি[i].Be_চাবিগুলি != 0; i++)
	যদি (name[0] == special_চাবিগুলি[i].vim_code0 &&
		name[1] == special_চাবিগুলি[i].vim_code1)
	    ফেরত OK;
    ফেরত FAIL;
}

    শূন্য
gui_mch_beep()
{
    ::beep();
}

    শূন্য
gui_mch_flash(পূর্ণসংখ্যা msec)
{
    // Do a visual beep by reversing the foreground and background colors

    যদি (gui.vimWindow->Lock()) {
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
শূন্য
gui_mch_invert_rectangle(
	পূর্ণসংখ্যা	r,
	পূর্ণসংখ্যা	c,
	পূর্ণসংখ্যা	nr,
	পূর্ণসংখ্যা	nc)
{
    BRect rect;
    rect.বাম = FILL_X(c);
    rect.শীর্ষ_2 = FILL_Y(r);
    rect.ডান = rect.বাম + nc * gui.char_width - PEN_WIDTH;
    rect.bottom = rect.শীর্ষ_2 + nr * gui.char_height - PEN_WIDTH;

    যদি (gui.vimWindow->Lock()) {
	gui.vimTextArea->SetDrawingMode(B_OP_INVERT);
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->SetDrawingMode(B_OP_COPY);
	gui.vimWindow->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    শূন্য
gui_mch_iconify()
{
    যদি (gui.vimWindow->Lock()) {
	gui.vimWindow->Minimize(সত্য);
	gui.vimWindow->Unlock();
    }
}

#যদি defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    শূন্য
gui_mch_set_foreground(শূন্য)
{
    // TODO
}
#endif

/*
 * Set the window title
 */
শূন্য
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    যদি (gui.vimWindow->Lock()) {
	gui.vimWindow->SetTitle((অক্ষর *)title);
	gui.vimWindow->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    শূন্য
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.বাম = FILL_X(gui.col);
    r.শীর্ষ_2 = FILL_Y(gui.row);
    পূর্ণসংখ্যা cells = utf_off2cells(সারি_Offset[gui.row] + gui.col, 100); // TODO-TODO
    যদি (cells>=4) cells = 1;
    r.ডান = r.বাম + cells*gui.char_width - PEN_WIDTH;
    r.bottom = r.শীর্ষ_2 + gui.char_height - PEN_WIDTH;

    যদি (gui.vimWindow->Lock()) {
	gui.vimTextArea->StrokeRect(r);
	gui.vimWindow->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
শূন্য
gui_mch_draw_part_cursor(
	পূর্ণসংখ্যা	w,
	পূর্ণসংখ্যা	h,
	guicolor_T  color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.বাম =
#ifdef FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_ডান ? FILL_X(gui.col + 1) - w :
#endif
	FILL_X(gui.col);
    r.ডান = r.বাম + w - PEN_WIDTH;
    r.bottom = FILL_Y(gui.row + 1) - PEN_WIDTH;
    r.শীর্ষ_2 = r.bottom - h + PEN_WIDTH;

    যদি (gui.vimWindow->Lock()) {
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
    শূন্য
gui_mch_update()
{
    gui_mch_flush();
    যখন (port_গুনতি(gui.vdcmp) > 0 &&
	    !vim_is_প্রবেশ_buf_full() &&
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
পূর্ণসংখ্যা
gui_mch_wait_for_chars(
	long	wtime)
{
    পূর্ণসংখ্যা		focus;
    bigtime_t	until, timeout;
    status_t	st;

    যদি (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_time() + timeout;
    }
    অন্যথা
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.in_focus;
    জন্য (;;)
    {
	// Stop or start blinking when focus changes
	যদি (gui.in_focus != focus)
	{
	    যদি (gui.in_focus)
		gui_mch_start_blink();
	    অন্যথা
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
	যদি (did_add_timer)
	    // Need to recompute the waiting time.
	    বিরতি;
# endif
# ifdef FEAT_JOB_CHANNEL
	যদি (has_any_channel())
	{
	    যদি (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	অন্যথা যদি (wtime < 0)
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

	যদি (প্রবেশ_available())
	    ফেরত OK;
	যদি (st < B_OK)		// includes B_TIMED_OUT
	    ফেরত FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	যদি (wtime >= 0)
	{
	    timeout = until - system_time();
	    যদি (timeout < 0)
		বিরতি;
	}
    }
    ফেরত FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    শূন্য
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    যদি (gui.vimWindow->Lock()) {
	gui.vimWindow->Flush();
	gui.vimWindow->Unlock();
    }
    ফেরত;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
শূন্য
gui_mch_clear_block(
	পূর্ণসংখ্যা	row1,
	পূর্ণসংখ্যা	col1,
	পূর্ণসংখ্যা	row2,
	পূর্ণসংখ্যা	col2)
{
    যদি (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearBlock(row1, col1, row2, col2);
	gui.vimWindow->Unlock();
    }
}

    শূন্য
gui_mch_clear_all()
{
    যদি (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearAll();
	gui.vimWindow->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
শূন্য
gui_mch_delete_সারিগুলি(
	পূর্ণসংখ্যা	row,
	পূর্ণসংখ্যা	অঙ্ক_সারিগুলি)
{
    gui.vimTextArea->mch_Delete_সারিগুলি(row, অঙ্ক_সারিগুলি);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
শূন্য
gui_mch_ঢোকাও_সারিগুলি(
	পূর্ণসংখ্যা	row,
	পূর্ণসংখ্যা	অঙ্ক_সারিগুলি)
{
    gui.vimTextArea->mch_ঢোকাও_সারিগুলি(row, অঙ্ক_সারিগুলি);
}

#যদি defined(FEAT_MENU)
/*
 * Menu stuff.
 */

শূন্য
gui_mch_enable_menu(
	পূর্ণসংখ্যা	flag)
{
    যদি (gui.vimWindow->Lock())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->SetEnabled(flag);
	gui.vimWindow->Unlock();
    }
}

শূন্য
gui_mch_set_menu_pos(
	পূর্ণসংখ্যা	x,
	পূর্ণসংখ্যা	y,
	পূর্ণসংখ্যা	w,
	পূর্ণসংখ্যা	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
শূন্য
gui_mch_add_menu(
	vimmenu_T   *menu,
	পূর্ণসংখ্যা	idx)
{
    vimmenu_T	*parent = menu->parent;

    //	popup menu - just create it unattached
    যদি (menu_is_popup(menu->name) && parent == ফাঁকা) {
	BPopUpMenu* বের_করো_Up_Menu = নতুন BPopUpMenu((ধ্রুবক অক্ষর*)menu->name, মিথ্যা, মিথ্যা);
	menu->submenu_id = বের_করো_Up_Menu;
	menu->id = ফাঁকা;
	ফেরত;
    }

    যদি (!menu_is_menubar(menu->name)
	    || (parent != ফাঁকা && parent->submenu_id == ফাঁকা))
	ফেরত;

    যদি (gui.vimWindow->Lock())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	যদি ( parent )
	    tmp = parent->submenu_id;
	অন্যথা
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	যদি ( ! tmp->FindItem((ধ্রুবক অক্ষর *) menu->dname)) {

	    BMenu *bmenu = নতুন BMenu((অক্ষর *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->AddItem(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->FindItem((ধ্রুবক অক্ষর *) menu->dname);

	}
	gui.vimWindow->Unlock();
    }
}

    শূন্য
gui_mch_toggle_tearoffs(পূর্ণসংখ্যা enable)
{
    // no tearoff menus
}

    স্থির BMessage *
MenuMessage(vimmenu_T *menu)
{
    BMessage *m = নতুন BMessage('menu');
    m->AddPointer("VimMenu", (শূন্য *)menu);

    ফেরত m;
}

/*
 * Add a menu item to a menu
 */
শূন্য
gui_mch_add_menu_item(
	vimmenu_T   *menu,
	পূর্ণসংখ্যা	idx)
{
    পূর্ণসংখ্যা	    mnemonic = 0;
    vimmenu_T	*parent = menu->parent;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    যদি (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	যদি (menu_is_toolbar(parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    যদি (toolbar != ফাঁকা) {
		toolbar->AddButton(idx, menu);
	    }
	} অন্যথা
#endif

	যদি (parent->submenu_id != ফাঁকা || menu_is_popup(parent->name)) {
	    যদি (menu_is_separator(menu->name)) {
		BSeparatorItem *item = নতুন BSeparatorItem();
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = ফাঁকা;
	    }
	    অন্যথা {
		BMenuItem *item = নতুন BMenuItem((অক্ষর *)menu->dname,
			MenuMessage(menu));
		item->SetTarget(gui.vimTextArea);
		item->SetTrigger((অক্ষর) menu->mnemonic);
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = ফাঁকা;
	    }
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
শূন্য
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    যদি (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	যদি (menu->parent && menu_is_toolbar(menu->parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    যদি (toolbar != ফাঁকা) {
		toolbar->RemoveButton(menu);
	    }
	} অন্যথা
#endif
	{
	    প্রতিজ্ঞা(menu->submenu_id == ফাঁকা || menu->submenu_id->গুনতি_বস্তুসমূহ() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    যদি (bmenu)
	    {
		bmenu->RemoveItem(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		যদি (bmenu == gui.vimForm->MenuBar() && bmenu->গুনতি_বস্তুসমূহ() == 0)
		{
		    bmenu->ResizeTo(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    delete menu->id;
	    menu->id = ফাঁকা;
	    menu->submenu_id = ফাঁকা;

	    gui.menu_height = (পূর্ণসংখ্যা) gui.vimForm->MenuHeight();
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
শূন্য
gui_mch_menu_grey(
	vimmenu_T   *menu,
	পূর্ণসংখ্যা	grey)
{
#ifdef FEAT_TOOLBAR
    যদি (menu->parent && menu_is_toolbar(menu->parent->name)) {
	যদি (gui.vimWindow->Lock()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    যদি (toolbar != ফাঁকা) {
		toolbar->GrayButton(menu, grey);
	    }
	    gui.vimWindow->Unlock();
	}
    } অন্যথা
#endif
    যদি (menu->id != ফাঁকা)
	menu->id->SetEnabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
শূন্য
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	পূর্ণসংখ্যা	hidden)
{
    যদি (menu->id != ফাঁকা)
	menu->id->SetEnabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    শূন্য
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    শূন্য
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    যদি (!menu_is_popup(menu->name) || menu->submenu_id == ফাঁকা)
	ফেরত;

    BPopUpMenu* popupMenu = dynamic_cast<BPopUpMenu*>(menu->submenu_id);
    যদি (popupMenu == ফাঁকা)
	ফেরত;

    BPoint point;
    যদি (gui.vimWindow->Lock()) {
	uint32 buttons = 0;
	gui.vimTextArea->GetMouse(&point, &buttons);
	gui.vimTextArea->ConvertToScreen(&point);
	gui.vimWindow->Unlock();
    }
    popupMenu->Go(point, সত্য);
}

#endif // FEAT_MENU

// Mouse stuff

#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
অক্ষর textplain[] = "text/plain";
অক্ষর vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    শূন্য
clip_mch_request_selection(Clipboard_T *cbd)
{
    যদি (be_clipboard->Lock())
    {
	BMessage *m = be_clipboard->তথ্য_2();
	// m->PrintToStream();

	char_u *string = ফাঁকা;
	ssize_t stringlen = -1;

	যদি (m->Find_তথ্য(textplain, B_MIME_TYPE,
		    (ধ্রুবক শূন্য **)&string, &stringlen) == B_OK
		|| m->FindString("text", (ধ্রুবক অক্ষর **)&string) == B_OK)
	{
	    যদি (stringlen == -1)
		stringlen = STRLEN(string);

	    পূর্ণসংখ্যা type;
	    অক্ষর *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    যদি (m->Find_তথ্য(vimselectiontype, B_MIME_TYPE,
			(ধ্রুবক শূন্য **)&seltype, &seltypelen) == B_OK)
	    {
		নির্বাচন (*seltype)
		{
		    পূর্বনির্ধারিত:
		    ক্ষেত্র 'L':	type = MLINE;	বিরতি;
		    ক্ষেত্র 'C':	type = MCHAR;	বিরতি;
#ifdef FEAT_VISUAL
		    ক্ষেত্র 'B':	type = MBLOCK;	বিরতি;
#endif
		}
	    }
	    অন্যথা
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
    শূন্য
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    পূর্ণসংখ্যা
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    ফেরত FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    শূন্য
clip_mch_set_selection(Clipboard_T *cbd)
{
    যদি (be_clipboard->Lock())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->তথ্য_2();
	প্রতিজ্ঞা(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = ফাঁকা;
	long_u	গুনতি;
	পূর্ণসংখ্যা type;

	type = clip_convert_selection(&str, &গুনতি, cbd);

	যদি (type < 0)
	    ফেরত;

	m->Add_তথ্য(textplain, B_MIME_TYPE, (শূন্য *)str, গুনতি);

	// Add type of selection
	অক্ষর	vtype;
	নির্বাচন (type)
	{
	    পূর্বনির্ধারিত:
	    ক্ষেত্র MLINE:    vtype = 'L';    বিরতি;
	    ক্ষেত্র MCHAR:    vtype = 'C';    বিরতি;
#ifdef FEAT_VISUAL
	    ক্ষেত্র MBLOCK:   vtype = 'B';    বিরতি;
#endif
	}
	m->Add_তথ্য(vimselectiontype, B_MIME_TYPE, (শূন্য *)&vtype, 1);

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
	পূর্ণসংখ্যা saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = নতুন BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    ফাঁকা, ফাঁকা, 0, মিথ্যা,
	    নতুন BMessage((saving == TRUE) ? 'save' : 'open'), ফাঁকা, সত্য);

    gui.vimApp->fBrowsedPath.Unset();

    gui.vimApp->fFilePanel->Window()->SetTitle((অক্ষর*)title);
    gui.vimApp->fFilePanel->SetPanelDirectory((ধ্রুবক অক্ষর*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    যখন (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = ফাঁকা;
    status_t ফলাফল = gui.vimApp->fBrowsedPath.InitCheck();
    যদি (ফলাফল == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->fBrowsedPath.Path());
    } অন্যথা
	যদি (ফলাফল != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    ফলাফল, strerror(ফলাফল));
	}

    delete gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = ফাঁকা;

    ফেরত fileName;
}
#endif // FEAT_BROWSE


#যদি defined(FEAT_GUI_DIALOG)

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

পূর্ণসংখ্যা
gui_mch_dialog(
	পূর্ণসংখ্যা	 type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	পূর্ণসংখ্যা	 dfltbutton,
	char_u	*textfield,
	পূর্ণসংখ্যা ex_cmd)
{
    VimDialog *dialog = নতুন VimDialog(type, (অক্ষর*)title, (অক্ষর*)message,
	    (অক্ষর*)buttons, dfltbutton, (অক্ষর*)textfield, ex_cmd);
    যৌক্তিক ret = dialog->Go();
    delete dialog;
	ফেরত ret;
}

#endif // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_TO_RGB(pixel);

    ফেরত ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    শূন্য
gui_mch_setmouse(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y)
{
    TRACE();
    // TODO
}

#ifdef FEAT_MBYTE_IME
    শূন্য
im_set_position(পূর্ণসংখ্যা row, পূর্ণসংখ্যা col)
{
    যদি (gui.vimWindow->Lock())
    {
	gui.vimTextArea->DrawIMString();
	gui.vimWindow->Unlock();
    }
    ফেরত;
}
#endif

    শূন্য
gui_mch_show_toolbar(পূর্ণসংখ্যা showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_height = (toolbar && showit) ? toolbar->ToolbarHeight() : 0.;
}

    শূন্য
gui_mch_set_toolbar_pos(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, পূর্ণসংখ্যা w, পূর্ণসংখ্যা h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    যদি (toolbar != ফাঁকা) {
	যদি (gui.vimWindow->Lock()) {
	    toolbar->MoveTo(x, y);
	    toolbar->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

#যদি defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    শূন্য
gui_mch_show_tabline(পূর্ণসংখ্যা showit)
{
    Vim_Tab_সারি *tab_সারি = gui.vimForm->Tab_সারি();

    যদি (tab_সারি == ফাঁকা)
	ফেরত;

    যদি (!showit != !gui.vimForm->Is_Showing_Tab_সারি()) {
	gui.vimForm->Set_Showing_Tab_সারি(showit != 0);
	gui.tabline_height = gui.vimForm->TablineHeight();
    }
}

    শূন্য
gui_mch_set_tabline_pos(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, পূর্ণসংখ্যা w, পূর্ণসংখ্যা h)
{
    Vim_Tab_সারি *tab_সারি = gui.vimForm->Tab_সারি();
    যদি (tab_সারি != ফাঁকা) {
	যদি (gui.vimWindow->Lock()) {
	    tab_সারি->MoveTo(x, y);
	    tab_সারি->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    পূর্ণসংখ্যা
gui_mch_showing_tabline()
{
    Vim_Tab_সারি *tab_সারি = gui.vimForm->Tab_সারি();
    ফেরত tab_সারি != ফাঁকা && gui.vimForm->Is_Showing_Tab_সারি();
}

/*
 * Update the labels of the tabline.
 */
    শূন্য
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    পূর্ণসংখ্যা	    nr = 0;
    পূর্ণসংখ্যা	    curtabidx = 0;

    Vim_Tab_সারি *tab_সারি = gui.vimForm->Tab_সারি();

    যদি (tab_সারি == ফাঁকা)
	ফেরত;

    gui.vimWindow->Lock();

    // Add a label for each tab page.  They all contain the same text area.
    জন্য (tp = first_tabpage; tp != ফাঁকা; tp = tp->tp_পরবর্তী, ++nr) {
	যদি (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_সারি->TabAt(nr);

	যদি (tab == ফাঁকা) {
	    tab = নতুন Vim_Tab_সারি::VimTab();
	    tab_সারি->AddTab(ফাঁকা, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->SetLabel((ধ্রুবক অক্ষর*)NameBuff);
	tab_সারি->Invalidate();
    }

    // Remove any old labels.
    যখন (nr < tab_সারি->গুনতি_Tabs())
	tab_সারি->RemoveTab(nr);

    যদি (tab_সারি->Selection() != curtabidx)
	tab_সারি->Select(curtabidx);

    gui.vimWindow->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    শূন্য
gui_mch_set_curtab(পূর্ণসংখ্যা nr)
{
    Vim_Tab_সারি *tab_সারি = gui.vimForm->Tab_সারি();
    যদি (tab_সারি == ফাঁকা)
	ফেরত;

    gui.vimWindow->Lock();

    যদি (tab_সারি->Selection() != nr -1)
	tab_সারি->Select(nr -1);

    gui.vimWindow->Unlock();
}

#endif // FEAT_GUI_TABLINE
