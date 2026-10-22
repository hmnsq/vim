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

خارجي "C" {

#ضمن <assert.h>
#ضمن <float.h>
#ضمن <syslog.h>

#ضمن "vim.h"
#ضمن "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#ضمن <Application.h>
#ضمن <Beep.h>
#ضمن <Bitmap.h>
#ضمن <Box.h>
#ضمن <Button.h>
#ضمن <Clipboard.h>
#ضمن <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#ضمن <File.h>
#ضمن <FilePanel.h>
#ضمن <FindDirectory.h>
//#include <Font.h>
#ضمن <IconUtils.h>
#ضمن <Input.h>
#ضمن <ListView.h>
#ضمن <MenuBar.h>
#ضمن <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#ضمن <Path.h>
#ضمن <PictureButton.h>
#ضمن <PopUpMenu.h>
//#include <Region.h>
#ضمن <Resources.h>
//#include <Roster.h>
#ضمن <Screen.h>
#ضمن <ScrollBar.h>
#ضمن <ScrollView.h>
#ضمن <String.h>
#ضمن <StringView.h>
//#include <SupportDefs.h>
#ضمن <TabView.h>
#ضمن <TextControl.h>
#ضمن <TextView.h>
#ضمن <TranslationUtils.h>
#ضمن <TranslatorFormats.h>
#ضمن <View.h>
#ضمن <Window.h>

صنف VimApp;
صنف VimFormView;
صنف Vim_متن_Area_View;
صنف Vim_کړکۍ;
صنف VimToolbar;
صنف Vim_Tab_کرښه;

خارجي کلی_map *کلی_Map;
خارجي حرف *کلی_Map_Chars;

خارجي صحيح اصلي(صحيح argc, حرف **argv);

#که_نه_تعریف_شوی B_MAX_پورټ_شمېر
#تعریف_کړه B_MAX_پورټ_شمېر    255
#د_که_پای

// VimApp seems comparable to the X "vimShell"
صنف VimApp: عام BApplication
{
    تعريفنوع BApplication Inherited;
    عام:
    VimApp(ثابت حرف *appsig);
    ~VimApp();

    // callbacks:
#إذا 0
    مجازي فراغ Dispatch_پیغام(BMessage *m, BHandler *h)
    {
	m->Print_منزل_بهیر();
	Inherited::Dispatch_پیغام(m, h);
    }
#د_که_پای
    مجازي فراغ Ready_منزل_پرلپسې();
    مجازي فراغ ArgvReceived(int32 argc, حرف **argv);
    مجازي فراغ RefsReceived(BMessage *m);
    مجازي منطقي QuitRequested();
    مجازي فراغ پیغام_Received(BMessage *m);

    ساكن فراغ SendRefs(BMessage *m, منطقي changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	f_Browsed_لار;
    خاص:
};

صنف Vim_کړکۍ: عام BWindow
{
    تعريفنوع BWindow Inherited;
    عام:
    Vim_کړکۍ();
    ~Vim_کړکۍ();

    مجازي فراغ Dispatch_پیغام(BMessage *m, BHandler *h);
    مجازي فراغ کړکۍ_Activated(منطقي فعال);
    مجازي منطقي QuitRequested();

    VimFormView	    *formView;

    خاص:
    فراغ init();

    منطقي is_fullscreen = خطأ;
    BRect saved_چوکاټ;
    کړکۍ_look saved_look;
};

صنف VimFormView: عام BView
{
    تعريفنوع BView Inherited;
    عام:
    VimFormView(BRect چوکاټ);
    ~VimFormView();

    // callbacks:
    مجازي فراغ AllAttached();
    مجازي فراغ چوکاټ_Resized(عائم new_پلنوالی, عائم new_لوړوالی);

#تعریف_کړه MENUBAR_MARGIN	1
    عائم Menu_لوړوالی() ثابت
    { أرجع menuBar ? menuBar->چوکاټ_2().لوړوالی_2() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() ثابت
    { أرجع menuBar; }

    خاص:
    فراغ init(BRect);

    BMenuBar	    *menuBar;
    Vim_متن_Area_View *متن_Area;

#که_تعریف_شوی FEAT_TOOLBAR
    عام:
    عائم Toolbar_لوړوالی() ثابت;
    VimToolbar *ToolBar() ثابت
    { أرجع toolBar; }
    خاص:
    VimToolbar	    *toolBar;
#د_که_پای

#که_تعریف_شوی FEAT_GUI_TABLINE
    عام:
    Vim_Tab_کرښه *Tab_کرښه() ثابت	{ أرجع tab_کرښه; }
    منطقي Is_Showing_Tab_کرښه() ثابت { أرجع showing_Tab_کرښه; }
    فراغ سېټ_Showing_Tab_کرښه(منطقي showing) { showing_Tab_کرښه = showing;	}
    عائم Tabline_لوړوالی() ثابت;
    خاص:
    Vim_Tab_کرښه	*tab_کرښه;
    صحيح	showing_Tab_کرښه;
#د_که_پای
};

صنف Vim_متن_Area_View: عام BView
{
    تعريفنوع BView Inherited;
    عام:
    Vim_متن_Area_View(BRect چوکاټ);
    ~Vim_متن_Area_View();

    // callbacks:
    مجازي فراغ Draw(BRect updateRect);
    مجازي فراغ کلی_ښکته(ثابت حرف *بایټونه, int32 عدد_بایټونه_2);
    مجازي فراغ Mouse_ښکته(BPoint ټکی);
    مجازي فراغ Mouse_پورته(BPoint ټکی);
    مجازي فراغ MouseMoved(BPoint ټکی, uint32 transit, ثابت BMessage *پیغام_2);
    مجازي فراغ پیغام_Received(BMessage *m);

    // own functions:
    صحيح mchInitFont(char_u *name);
    فراغ mchDrawString(صحيح لیکه, صحيح ستون, char_u *s, صحيح len, صحيح flags);
    فراغ mch_Clear_بلاک(صحيح row1, صحيح col1, صحيح row2, صحيح col2);
    فراغ mchClearAll();
    فراغ mch_Delete_کرښې(صحيح لیکه, صحيح عدد_کرښې);
    فراغ mch_ورننباسه_کرښې(صحيح لیکه, صحيح عدد_کرښې);

    ساكن فراغ guiSendMouseEvent(صحيح button, صحيح x, صحيح y, صحيح repeated_click, int_u modifiers);
    ساكن فراغ guiMouseMoved(صحيح x, صحيح y);
    ساكن فراغ guiBlankMouse(منطقي should_hide);
    ساكن int_u mouse_Modifiers_منزل_Vim(int32 beModifiers);

    int32 mouse_Drag_Event_شمېر;

#که_تعریف_شوی FEAT_MBYTE_IME
    فراغ DrawIMString(فراغ);
#د_که_پای

    خاص:
    فراغ init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#که_تعریف_شوی FEAT_MBYTE_IME
    بنية {
	BMessenger* messenger;
	BMessage* پیغام_2;
	BPoint location;
	صحيح لیکه;
	صحيح ستون;
	صحيح شمېر;
    } IMData;
#د_که_پای
};

صنف VimScrollBar: عام BScrollBar
{
    تعريفنوع BScrollBar Inherited;
    عام:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    مجازي فراغ ارزښت_بدل_شوی_2(عائم new_ارزښت);
    مجازي فراغ Mouse_پورته(BPoint where);
    فراغ سېټ_ارزښت(عائم newval);
    scrollbar_T *getGsb()
    { أرجع gsb; }

    int32	scroll_Event_شمېر;

    خاص:
    scrollbar_T *gsb;
    عائم   ignore_ارزښت;
};


#که_تعریف_شوی FEAT_TOOLBAR

صنف VimToolbar : عام BBox
{
    ساكن BBitmap *نورمال_Buttons_Bitmap;
    ساكن BBitmap *grayedButtonsBitmap;

    BBitmap *بارول_Vim_Bitmap(ثابت حرف* fileName);
    منطقي GetPictureFromBitmap(BPicture *picture_منزل, int32 شاخص, BBitmap *bitmapFrom, منطقي pressed);
    منطقي Modify_Bitmap_منزل_Grayed(BBitmap *bitmap);

    BList fButtonsList;
    فراغ InvalidateLayout();

    عام:
    VimToolbar(BRect چوکاټ, ثابت حرف * name);
    ~VimToolbar();

    منطقي PrepareButtonBitmaps();

    منطقي جمع_کړه_Button(int32 شاخص, vimmenu_T *menu);
    منطقي RemoveButton(vimmenu_T *menu);
    منطقي خړ_Button(vimmenu_T *menu, صحيح grey);

    عائم Toolbar_لوړوالی() ثابت;
    مجازي فراغ Attached_منزل_کړکۍ();
};

BBitmap *VimToolbar::نورمال_Buttons_Bitmap  = عدم;
BBitmap *VimToolbar::grayedButtonsBitmap  = عدم;

ثابت عائم ToolbarMargin = 3.;
ثابت عائم ButtonMargin  = 3.;

#د_که_پای //FEAT_TOOLBAR

#که_تعریف_شوی FEAT_GUI_TABLINE

صنف Vim_Tab_کرښه : عام BTabView
{
    عام:
	صنف VimTab : عام BTab {
	    عام:
		VimTab() : BTab(جديد BView(BRect(), "-Empty-", 0, 0)) {}

	    مجازي فراغ Select(BView* owner);
	};

	Vim_Tab_کرښه(BRect r) : BTabView(r, "vimTabLine", B_پلنوالی_FROM_LABEL,
	       B_FOLLOW_کیڼ | B_FOLLOW_سر | B_FOLLOW_ښی, B_WILL_DRAW | B_چوکاټ_EVENTS) {}

    عائم Tabline_لوړوالی() ثابت;
    مجازي فراغ Mouse_ښکته(BPoint ټکی);
};

#د_که_پای //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
صنف VimFont: عام BFont
{
    تعريفنوع BFont Inherited;
    عام:
    VimFont();
    VimFont(ثابت VimFont *rhs);
    VimFont(ثابت BFont *rhs);
    VimFont(ثابت VimFont &rhs);
    ~VimFont();

    VimFont *بل;
    صحيح refcount;
    char_u *name;

    خاص:
    فراغ init();
};

#إذا defined(FEAT_GUI_DIALOG)

صنف VimDialog : عام BWindow
{
    تعريفنوع BWindow Inherited;

    BButton* _CreateButton(int32 which, ثابت حرف* label);

    عام:

    صنف View : عام BView {
	تعريفنوع BView Inherited;

	عام:
	View(BRect چوکاټ);
	~View();

	مجازي فراغ Draw(BRect updateRect);
	فراغ InitIcon(int32 type);

	خاص:
	BBitmap*    fIconBitmap;
    };

    VimDialog(صحيح type, ثابت حرف *title, ثابت حرف *پیغام_2,
	    ثابت حرف *buttons, صحيح dfltbutton, ثابت حرف *textfield,
	    صحيح ex_cmd);
    ~VimDialog();

    صحيح Go();

    مجازي فراغ پیغام_Received(BMessage *پیغام);

    خاص:
    sem_id	    fDialogSem;
    صحيح		    f_Dialog_ارزښت;
    BList	    fButtonsList;
    BTextView*	    f_پیغام_View;
    BTextControl*   f_ننوت_Control;
    ثابت حرف*	    f_ننوت_ارزښت;
};

صنف VimSelectFontDialog : عام BWindow
{
    تعريفنوع BWindow Inherited;

    فراغ _CleanList(BListView* list);
    فراغ _UpdateFontStyles();
    فراغ Update_اندازه_ننوت_Preview();
    فراغ _UpdateFontPreview();
    منطقي Update_From_List_توکی(BListView* list, حرف* متن, صحيح متن_اندازه);
    عام:

    VimSelectFontDialog(font_family* family, font_style* style, عائم* اندازه);
    ~VimSelectFontDialog();

    منطقي Go();

    مجازي فراغ پیغام_Received(BMessage *پیغام);

    خاص:
    حالت_t	    f_حالت;
    sem_id	    fDialogSem;
    منطقي	    f_Dialog_ارزښت;
    font_family*    fFamily;
    font_style*	    fStyle;
    عائم*	    f_اندازه;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    عائم	    f_Font_اندازه;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_ننوت;
};

#د_که_پای // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

بنية Main_دلیلونه {
    صحيح	     argc;
    حرف    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#تعریف_کړه	کلی_پیغام_BUFSIZ	7
#إذا کلی_پیغام_BUFSIZ < MAX_کلی_کوډ_LEN
#تېروتنه Increase کلی_پیغام_BUFSIZ!
#د_که_پای

بنية Vim_کلی_پیغام {
    char_u  اوږدتیا;
    char_u  chars[کلی_پیغام_BUFSIZ];  // contains Vim encoding
    منطقي    csi_escape;
};

بنية Vim_Resize_پیغام {
    صحيح	    پلنوالی;
    صحيح	    لوړوالی;
};

بنية Vim_Scroll_Bar_پیغام {
    VimScrollBar *sb;
    اوږد    ارزښت;
    صحيح	    stillDragging;
};

بنية Vim_Menu_پیغام {
    vimmenu_T	*guiMenu;
};

بنية Vim_Mouse_پیغام {
    صحيح	    button;
    صحيح	    x;
    صحيح	    y;
    صحيح	    repeated_click;
    int_u   modifiers;
};

بنية Vim_Mouse_Moved_پیغام {
    صحيح	    x;
    صحيح	    y;
};

بنية Vim_Focus_پیغام {
    منطقي    فعال;
};

بنية Vim_Refs_پیغام {
    BMessage   *پیغام_2;
    منطقي    changedir;
};

بنية Vim_Tabline_پیغام {
    صحيح	    شاخص;
};

بنية Vim_Tabline_Menu_پیغام {
    صحيح	    شاخص;
    صحيح	    event;
};

بنية Vim_پیغام {
    تعداد Vim_پیغام_Type {
	کلی_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    اتحاد {
	بنية Vim_کلی_پیغام    کلی_2;
	بنية Vim_Resize_پیغام New_اندازه;
	بنية Vim_Scroll_Bar_پیغام	Scroll;
	بنية Vim_Menu_پیغام   Menu;
	بنية Vim_Mouse_پیغام  Mouse;
	بنية Vim_Mouse_Moved_پیغام	MouseMoved;
	بنية Vim_Focus_پیغام  Focus;
	بنية Vim_Refs_پیغام   Refs;
	بنية Vim_Tabline_پیغام	Tabline;
	بنية Vim_Tabline_Menu_پیغام    TablineMenu;
    } u;
};

#تعریف_کړه RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#تعریف_کړه GUI_منزل_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

ساكن بنية specialkey
{
    uint16  Be_کلیګانې;
#تعریف_کړه کلی_3(a,b)    ((a)<<8|(b))
#تعریف_کړه K(a)	    کلی_3(0,a)		// for ASCII codes
#تعریف_کړه F(b)	    کلی_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_کلیګانې[] =
{
    {K(B_پورته_ARROW),	'k', 'u'},
    {K(B_ښکته_ARROW),	    'k', 'd'},
    {K(B_کیڼ_ARROW),	    'k', 'l'},
    {K(B_ښی_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_ورننباسه),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_پای_ټکی),		'@', '7'},
    {K(B_پاڼه_پورته),	'k', 'P'},	// XK_Prior
    {K(B_پاڼه_ښکته),	    'k', 'N'},	    // XK_Next,

#تعریف_کړه لومړی_FUNCTION_کلی  11
    {F(B_F1_کلی),	'k', '1'},
    {F(B_F2_کلی),	'k', '2'},
    {F(B_F3_کلی),	'k', '3'},
    {F(B_F4_کلی),	'k', '4'},
    {F(B_F5_کلی),	'k', '5'},
    {F(B_F6_کلی),	'k', '6'},
    {F(B_F7_کلی),	'k', '7'},
    {F(B_F8_کلی),	'k', '8'},
    {F(B_F9_کلی),	'k', '9'},
    {F(B_F10_کلی),	'k', ';'},

    {F(B_F11_کلی),	'F', '1'},
    {F(B_F12_کلی),	'F', '2'},
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
    {F(B_PRINT_کلی),	    '%', '9'},

#إذا 0
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
#د_که_پای

    // End of list marker:
    {0,		    0, 0}
};

#تعریف_کړه عدد_SPECIAL_کلیګانې    ARRAY_اوږدتیا(special_کلیګانې)

// ---------------- VimApp ----------------

    ساكن فراغ
docd(BPath &لار)
{
    mch_chdir((حرف *)لار.لار_2());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	ساكن فراغ
drop_callback(فراغ *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	ساكن فراغ
RefsReceived(BMessage *m, منطقي changedir)
{
    uint32 type;
    int32 شمېر;

    m->Print_منزل_بهیر();
    بدل (m->what) {
	حالة B_REFS_RECEIVED:
	حالة B_SIMPLE_معلومات:
	    m->GetInfo("refs", &type, &شمېر);
	    إذا (type != B_REF_TYPE)
		اذهب bad;
	    اكسر;
	حالة B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &شمېر);
	    إذا (type != B_STRING_TYPE)
		اذهب bad;
	    إذا (changedir) {
		حرف *dirname;
		إذا (m->پیدا_کړه_String("cwd", (ثابت حرف **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    اكسر;
	افتراضي:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    ړنګول m;
	    أرجع;
    }

#که_تعریف_شوی FEAT_VISUAL
    reset_VIsual();
#د_که_پای

    char_u  **fnames;
    fnames = (char_u **) alloc(شمېر * حجم(char_u *));
    صحيح fname_شاخص = 0;

    بدل (m->what) {
	حالة B_REFS_RECEIVED:
	حالة B_SIMPLE_معلومات:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    لكل (صحيح i = 0; i < شمېر; ++i)
	    {
		ثبت_ref ref;
		إذا (m->پیدا_کړه_Ref("refs", i, &ref) == B_OK) {
		    BEntry ثبت(&ref, خطأ);
		    BPath لار;
		    ثبت.Get_لار(&لار);

		    // Change to parent directory?
		    إذا (changedir) {
			BPath parentpath;
			لار.Get_پلار(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    إذا (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			إذا (!changedir)
			    docd(لار);
		    } وإلا {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)لار.لار_2(), IObuff);
			إذا (fname == عدم)
			    fname = (char_u *)لار.لار_2();
			fnames[fname_شاخص++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = خطأ;
		}
	    }
	    اكسر;
	حالة B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    لكل (صحيح i = 1; i < شمېر; ++i)
	    {
		حرف *fname;

		إذا (m->پیدا_کړه_String("argv", i, (ثابت حرف **) &fname) == B_OK) {
		    fnames[fname_شاخص++] = vim_strsave((char_u *)fname);
		}
	    }
	    اكسر;
	افتراضي:
	    // fprintf(stderr, "case default\n");
	    اكسر;
    }

    ړنګول m;

    // Handle the drop, :edit to get to the file
    إذا (fname_شاخص > 0) {
	handle_drop(fname_شاخص, fnames, FALSE, drop_callback, عدم);

	setcursor();
	بهر_flush();
    } وإلا {
	vim_free(fnames);
    }
}

VimApp::VimApp(ثابت حرف *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(عدم)
{
}

VimApp::~VimApp()
{
}

    فراغ
VimApp::Ready_منزل_پرلپسې()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    فراغ
VimApp::ArgvReceived(int32 arg_argc, حرف **arg_argv)
{
    إذا (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	إذا (gui.vim_کړکۍ)
	    gui.vim_کړکۍ->Minimize(خطأ);
	BMessage *m = Current_پیغام();
	Detach_Current_پیغام();
	SendRefs(m, ریښتیا);
    }
}

    فراغ
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    صحيح حد = 15;
    طالما (--حد >= 0 && (curbuf == عدم || curbuf->b_پیل_ffc == 0))
	snooze(100000);    //  0.1 s
    إذا (gui.vim_کړکۍ)
	gui.vim_کړکۍ->Minimize(خطأ);
    Detach_Current_پیغام();
    SendRefs(m, ریښتیا);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    فراغ
VimApp::SendRefs(BMessage *m, منطقي changedir)
{
    Vim_Refs_پیغام rm;
    rm.پیغام_2 = m;
    rm.changedir = changedir;

    write_پورټ(gui.vdcmp, Vim_پیغام::Refs, &rm, حجم(rm));
    //	calls ::RefsReceived
}

    فراغ
VimApp::پیغام_Received(BMessage *m)
{
    بدل (m->what) {
	حالة 'save':
	    {
		ثبت_ref refDirectory;
		m->پیدا_کړه_Ref("directory", &refDirectory);
		f_Browsed_لار.سېټ_منزل(&refDirectory);
		BString strName;
		m->پیدا_کړه_String("name", &strName);
		f_Browsed_لار.Append(strName.String());
	    }
	    اكسر;
	حالة 'open':
	    {
		ثبت_ref ref;
		m->پیدا_کړه_Ref("refs", &ref);
		f_Browsed_لار.سېټ_منزل(&ref);
	    }
	    اكسر;
	حالة B_CANCEL:
	    {
		BFilePanel *panel;
		m->پیدا_کړه_اشاره_ګر("source", (فراغ**)&panel);
		إذا (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    اكسر;
	افتراضي:
	    Inherited::پیغام_Received(m);
	    اكسر;
    }
}

    منطقي
VimApp::QuitRequested()
{
    (فراغ)Inherited::QuitRequested();
    أرجع خطأ;
}

// ---------------- VimWindow ----------------

Vim_کړکۍ::Vim_کړکۍ():
    BWindow(BRect(40, 40, 150, 150),
	    "Vim",
	    B_TITLED_کړکۍ,
	    0,
	    B_CURRENT_WORKSPACE)

{
    init();
}

Vim_کړکۍ::~Vim_کړکۍ()
{
    إذا (formView) {
	Remove_ماشوم(formView);
	ړنګول formView;
    }
    gui.vim_کړکۍ = عدم;
}

    فراغ
Vim_کړکۍ::init()
{
    // Attach the VimFormView
    formView = جديد VimFormView(Bounds());
    إذا (formView != عدم) {
	جمع_کړه_ماشوم(formView);
    }
}

#إذا 0  //  disabled in zeta patch
    فراغ
Vim_کړکۍ::Dispatch_پیغام(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    بدل (m->what) {
	حالة B_MOUSE_پورته:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		إذا (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->پیدا_کړه_ټکی("where", &where);
		    v->Mouse_پورته(where);
		} وإلا {
		    Inherited::Dispatch_پیغام(m, h);
		}
	    }
	    اكسر;
	افتراضي:
	    Inherited::Dispatch_پیغام(m, h);
    }
}
#د_که_پای

    فراغ
Vim_کړکۍ::کړکۍ_Activated(منطقي فعال)
{
    Inherited::کړکۍ_Activated(فعال);
    // the textArea gets the keyboard action
    إذا (فعال && gui.vim_متن_Area)
	gui.vim_متن_Area->MakeFocus(ریښتیا);

    بنية Vim_Focus_پیغام fm;
    fm.فعال = فعال;

    write_پورټ(gui.vdcmp, Vim_پیغام::Focus, &fm, حجم(fm));
}

    منطقي
Vim_کړکۍ::QuitRequested()
{
    بنية Vim_کلی_پیغام km;
    km.اوږدتیا = 5;
    حافظه_نقل((حرف *)km.chars, "\033:qa\r", km.اوږدتیا);
    km.csi_escape = خطأ;
    write_پورټ(gui.vdcmp, Vim_پیغام::کلی_2, &km, حجم(km));
    أرجع خطأ;
}
    فراغ
Vim_کړکۍ::Dispatch_پیغام(BMessage *m, BHandler *h)
{
    منطقي should_propagate = ریښتیا;

    بدل (m->what)
    {
	حالة B_کلی_ښکته:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->پیدا_کړه_Int32("raw_char", &scancode);
		m->پیدا_کړه_Int32("modifiers", &beModifiers);

		إذا (scancode == B_ENTER && (beModifiers & B_کیڼ_COMMAND_کلی))
		    {
			should_propagate = خطأ;
			إذا (هذا->is_fullscreen)
			    {
				هذا->is_fullscreen = خطأ;
				Resize_منزل(هذا->saved_چوکاټ.پلنوالی_2(), هذا->saved_چوکاټ.لوړوالی_2());
				Move_منزل(هذا->saved_چوکاټ.کیڼ, هذا->saved_چوکاټ.سر_2);
				سېټ_Look(هذا->saved_look);
				سېټ_Flags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } وإلا {
				هذا->saved_چوکاټ = چوکاټ_2();
				هذا->saved_look = Look();
				هذا->is_fullscreen = ریښتیا;
				BScreen s(هذا);
				سېټ_Look(B_NO_BORDER_کړکۍ_LOOK);
				Resize_منزل(s.چوکاټ_2().پلنوالی_2() + 1, s.چوکاټ_2().لوړوالی_2() + 1);
				Move_منزل(s.چوکاټ_2().کیڼ, s.چوکاټ_2().سر_2);
				سېټ_Flags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    إذا (should_propagate)
	Inherited::Dispatch_پیغام(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect چوکاټ):
    BView(چوکاټ, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_چوکاټ_EVENTS),
    menuBar(عدم),
#که_تعریف_شوی FEAT_TOOLBAR
    toolBar(عدم),
#د_که_پای
#که_تعریف_شوی FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_کرښه(عدم),
#د_که_پای
    متن_Area(عدم)
{
    init(چوکاټ);
}

VimFormView::~VimFormView()
{
    إذا (menuBar) {
	Remove_ماشوم(menuBar);
#که_تعریف_شوی never
	//  deleting the menuBar leads to SEGV on exit
	//  richard@whitequeen.com Jul 99
	ړنګول menuBar;
#د_که_پای
    }

#که_تعریف_شوی FEAT_TOOLBAR
    ړنګول toolBar;
#د_که_پای

#که_تعریف_شوی FEAT_GUI_TABLINE
    ړنګول tab_کرښه;
#د_که_پای

    إذا (متن_Area) {
	Remove_ماشوم(متن_Area);
	ړنګول متن_Area;
    }
    gui.vimForm = عدم;
}

    فراغ
VimFormView::init(BRect چوکاټ)
{
    menuBar = جديد BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    جمع_کړه_ماشوم(menuBar);

#که_تعریف_شوی FEAT_TOOLBAR
    toolBar = جديد VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    جمع_کړه_ماشوم(toolBar);
#د_که_پای

#که_تعریف_شوی FEAT_GUI_TABLINE
    tab_کرښه = جديد Vim_Tab_کرښه(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    جمع_کړه_ماشوم(tab_کرښه);
#د_که_پای

    BRect پاتې_شوی = چوکاټ;
    متن_Area = جديد Vim_متن_Area_View(پاتې_شوی);
    جمع_کړه_ماشوم(متن_Area);
    // The textArea will be resized later when menus are added

    gui.vimForm = هذا;
}

#که_تعریف_شوی FEAT_TOOLBAR
    عائم
VimFormView::Toolbar_لوړوالی() ثابت
{
    أرجع toolBar ? toolBar->Toolbar_لوړوالی() : 0.;
}
#د_که_پای

#که_تعریف_شوی FEAT_GUI_TABLINE
    عائم
VimFormView::Tabline_لوړوالی() ثابت
{
    أرجع (tab_کرښه && Is_Showing_Tab_کرښه()) ? tab_کرښه->Tabline_لوړوالی() : 0.;
}
#د_که_پای

    فراغ
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    إذا (menuBar && متن_Area) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect پاتې_شوی = Bounds();

#که_تعریف_شوی FEAT_MENU
	پاتې_شوی.سر_2 += Menu_لوړوالی();
	menuBar->Resize_منزل(پاتې_شوی.ښی, پاتې_شوی.سر_2);
	gui.menu_لوړوالی = (صحيح) Menu_لوړوالی();
#د_که_پای

#که_تعریف_شوی FEAT_TOOLBAR
	toolBar->Move_منزل(پاتې_شوی.کیڼ, پاتې_شوی.سر_2);
	toolBar->Resize_منزل(پاتې_شوی.ښی, Toolbar_لوړوالی());
	پاتې_شوی.سر_2 += Toolbar_لوړوالی();
	gui.toolbar_لوړوالی = Toolbar_لوړوالی();
#د_که_پای

#که_تعریف_شوی FEAT_GUI_TABLINE
	tab_کرښه->Move_منزل(پاتې_شوی.کیڼ, پاتې_شوی.سر_2);
	tab_کرښه->Resize_منزل(پاتې_شوی.ښی + 1, Tabline_لوړوالی());
	پاتې_شوی.سر_2 += Tabline_لوړوالی();
	gui.tabline_لوړوالی = Tabline_لوړوالی();
#د_که_پای

	متن_Area->Resize_منزل(پاتې_شوی.پلنوالی_2(), پاتې_شوی.لوړوالی_2());
	متن_Area->Move_منزل(پاتې_شوی.کیڼ, پاتې_شوی.سر_2);
    }


    Inherited::AllAttached();
}

    فراغ
VimFormView::چوکاټ_Resized(عائم new_پلنوالی, عائم new_لوړوالی)
{
    بنية Vim_Resize_پیغام sm;
    صحيح adjust_h, adjust_w;

    new_پلنوالی += 1;	//  adjust from width to number of pixels occupied
    new_لوړوالی += 1;

    sm.پلنوالی = (صحيح) new_پلنوالی;
    sm.لوړوالی = (صحيح) new_لوړوالی;
    adjust_w = ((صحيح)new_پلنوالی - gui_get_بنسټ_پلنوالی()) % gui.char_پلنوالی;
    adjust_h = ((صحيح)new_لوړوالی - gui_get_بنسټ_لوړوالی()) % gui.char_لوړوالی;

    إذا (adjust_w > 0 || adjust_h > 0) {
	sm.پلنوالی  -= adjust_w;
	sm.لوړوالی -= adjust_h;
    }

    write_پورټ(gui.vdcmp, Vim_پیغام::Resize, &sm, حجم(sm));
    //	calls gui_resize_shell(new_width, new_height);

    أرجع;

    /*
     * The area below the vertical scrollbar is erased to the colour
     * set with SetViewColor() automatically, because we had set
     * B_WILL_DRAW. Resizing the window tight around the vertical
     * scroll bar also helps to avoid debris.
     */
}

// ---------------- VimTextAreaView ----------------

Vim_متن_Area_View::Vim_متن_Area_View(BRect چوکاټ):
    BView(چوکاټ, "VimTextAreaView", B_FOLLOW_ALL_SIDES,
#که_تعریف_شوی FEAT_MBYTE_IME
	B_WILL_DRAW | B_ډک_UPDATE_ON_RESIZE | B_ننوت_لاره_AWARE
#وإلا
	B_WILL_DRAW | B_ډک_UPDATE_ON_RESIZE
#د_که_پای
	),
    mouse_Drag_Event_شمېر(0)
{
#که_تعریف_شوی FEAT_MBYTE_IME
    IMData.messenger = عدم;
    IMData.پیغام_2 = عدم;
#د_که_پای
    init(چوکاټ);
}

Vim_متن_Area_View::~Vim_متن_Area_View()
{
    gui.vim_متن_Area = عدم;
}

    فراغ
Vim_متن_Area_View::init(BRect چوکاټ)
{
    // set up global var for fast access
    gui.vim_متن_Area = هذا;

    /*
     * Tell the app server not to erase the view: we will
     * fill it in completely by ourselves.
     * (Does this really work? Even if not, it won't harm either.)
     */
    سېټ_View_Color(B_TRANSPARENT_32_بټ);
#تعریف_کړه PEN_پلنوالی   1
    سېټ_Pen_اندازه(PEN_پلنوالی);
#تعریف_کړه W_پلنوالی(curwin)   0
}

    فراغ
Vim_متن_Area_View::Draw(BRect updateRect)
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
    rgb_color rgb = GUI_منزل_RGB(gui.شا_pixel);
    سېټ_ټیټ_Color(rgb);
    FillRect(updateRect, B_SOLID_ټیټ);
    gui_redraw((صحيح) updateRect.کیڼ, (صحيح) updateRect.سر_2,
	    (صحيح) (updateRect.پلنوالی_2() + PEN_پلنوالی), (صحيح) (updateRect.لوړوالی_2() + PEN_پلنوالی));

    // Clear the border areas if needed
    سېټ_ټیټ_Color(rgb);

    إذا (updateRect.کیڼ < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.کیڼ, updateRect.سر_2,
		    FILL_X(0)-PEN_پلنوالی, updateRect.bottom), B_SOLID_ټیټ);
    إذا (updateRect.سر_2 < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.کیڼ, updateRect.سر_2,
		    updateRect.ښی, FILL_Y(0)-PEN_پلنوالی), B_SOLID_ټیټ);
    إذا (updateRect.ښی >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((صحيح)Columns), updateRect.سر_2,
		    updateRect.ښی, updateRect.bottom), B_SOLID_ټیټ);
    إذا (updateRect.bottom >= FILL_Y(لیکې))   //  bottom border
	FillRect(BRect(updateRect.کیڼ, FILL_Y((صحيح)لیکې),
		    updateRect.ښی, updateRect.bottom), B_SOLID_ټیټ);

#که_تعریف_شوی FEAT_MBYTE_IME
    DrawIMString();
#د_که_پای
}

    فراغ
Vim_متن_Area_View::کلی_ښکته(ثابت حرف *بایټونه, int32 عدد_بایټونه_2)
{
    بنية Vim_کلی_پیغام km;
    char_u *dest = km.chars;

    منطقي canHaveVimModifiers = خطأ;

    BMessage *پیغام = کړکۍ()->Current_پیغام();
    أكد(پیغام);
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
    إذا (عدد_بایټونه_2 > 1) {
	// This cannot be a special key
	إذا (عدد_بایټونه_2 > کلی_پیغام_BUFSIZ)
	    عدد_بایټونه_2 = کلی_پیغام_BUFSIZ;	    //	should never happen... ???
	km.اوږدتیا = عدد_بایټونه_2;
	حافظه_نقل((حرف *)dest, بایټونه, عدد_بایټونه_2);
	km.csi_escape = ریښتیا;
    } وإلا {
	int32 scancode = 0;
	پیغام->پیدا_کړه_Int32("key", &scancode);

	int32 beModifiers = 0;
	پیغام->پیدا_کړه_Int32("modifiers", &beModifiers);

	char_u string[3];
	صحيح len = 0;
	km.اوږدتیا = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	أكد(B_BACKSPACE <= 0x20);
	أكد(B_DELETE == 0x7F);
	إذا (((char_u)بایټونه[0] <= 0x20 || (char_u)بایټونه[0] == 0x7F) &&
		عدد_بایټونه_2 == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    إذا (beModifiers & B_CONTROL_کلی) {
		صحيح شاخص = کلی_Map->نورمال_map[scancode];
		صحيح new_عدد_بایټونه = کلی_Map_Chars[شاخص];
		char_u *new_بایټونه = (char_u *)&کلی_Map_Chars[شاخص + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		إذا (new_عدد_بایټونه != 1 || (*new_بایټونه > 0x20 &&
			    *new_بایټونه != 0x7F )) {
		    اذهب notspecial;
		}
		بایټونه = (حرف *)new_بایټونه;
	    }
	    canHaveVimModifiers = ریښتیا;

	    uint16 beoskey;
	    صحيح لومړی, وروستی;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    إذا (عدد_بایټونه_2 == 0 || بایټونه[0] == B_FUNCTION_کلی) {
		beoskey = F(scancode);
		لومړی = لومړی_FUNCTION_کلی;
		وروستی = عدد_SPECIAL_کلیګانې;
	    } وإلا إذا (*بایټونه == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		لومړی = وروستی = 0;
	    } وإلا {
		beoskey = K(بایټونه[0]);
		لومړی = 0;
		وروستی = لومړی_FUNCTION_کلی;
	    }

	    لكل (صحيح i = لومړی; i < وروستی; i++) {
		إذا (special_کلیګانې[i].Be_کلیګانې == beoskey) {
		    string[0] = CSI;
		    string[1] = special_کلیګانې[i].vim_code0;
		    string[2] = special_کلیګانې[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	إذا (len == 0) {
	    string[0] = بایټونه[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#إذا 0
	إذا (len == 3 ||
		بایټونه[0] == B_SPACE || بایټونه[0] == B_TAB ||
		بایټونه[0] == B_RETURN || بایټونه[0] == '\r' ||
		بایټونه[0] == B_ESCAPE)
#وإلا
	    إذا (canHaveVimModifiers)
#د_که_پای
	    {
		صحيح modifiers;
		modifiers = 0;
		إذا (beModifiers & B_ښويدنه_کلی)
		    modifiers |= MOD_ماسک_ښويدنه;
		إذا (beModifiers & B_CONTROL_کلی)
		    modifiers |= MOD_ماسک_CTRL;
		إذا (beModifiers & B_OPTION_کلی)
		    modifiers |= MOD_ماسک_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		صحيح کلی;
		إذا (string[0] == CSI && len == 3)
		    کلی = منزل_SPECIAL(string[1], string[2]);
		وإلا
		    کلی = string[0];
		کلی = simplify_کلی(کلی, &modifiers);
		إذا (IS_SPECIAL(کلی))
		{
		    string[0] = CSI;
		    string[1] = K_دوهم(کلی);
		    string[2] = K_دریم(کلی);
		    len = 3;
		}
		وإلا
		{
		    string[0] = کلی;
		    len = 1;
		}

		إذا (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.اوږدتیا = 3;
		}
	    }
	حافظه_نقل((حرف *)dest, string, len);
	km.اوږدتیا += len;
	km.csi_escape = خطأ;
    }

    write_پورټ(gui.vdcmp, Vim_پیغام::کلی_2, &km, حجم(km));

    /*
     * blank out the pointer if necessary
     */
    إذا (p_mh && !gui.اشاره_ګر_hidden)
    {
	guiBlankMouse(ریښتیا);
	gui.اشاره_ګر_hidden = TRUE;
    }
}
فراغ
Vim_متن_Area_View::guiSendMouseEvent(
	صحيح	button,
	صحيح	x,
	صحيح	y,
	صحيح	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_پیغام mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_پورټ(gui.vdcmp, Vim_پیغام::Mouse, &mm, حجم(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    إذا (gui.اشاره_ګر_hidden)
    {
	guiBlankMouse(خطأ);
	gui.اشاره_ګر_hidden = FALSE;
    }
}

فراغ
Vim_متن_Area_View::guiMouseMoved(
	صحيح	x,
	صحيح	y)
{
    Vim_Mouse_Moved_پیغام mm;

    mm.x = x;
    mm.y = y;

    write_پورټ(gui.vdcmp, Vim_پیغام::MouseMoved, &mm, حجم(mm));

    إذا (gui.اشاره_ګر_hidden)
    {
	guiBlankMouse(خطأ);
	gui.اشاره_ګر_hidden = FALSE;
    }
}

    فراغ
Vim_متن_Area_View::guiBlankMouse(منطقي should_hide)
{
    إذا (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } وإلا {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
Vim_متن_Area_View::mouse_Modifiers_منزل_Vim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    إذا (beModifiers & B_ښويدنه_کلی)
	vim_modifiers |= MOUSE_ښويدنه;
    إذا (beModifiers & B_CONTROL_کلی)
	vim_modifiers |= MOUSE_CTRL;
    إذا (beModifiers & B_OPTION_کلی)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    أرجع vim_modifiers;
}

    فراغ
Vim_متن_Area_View::Mouse_ښکته(BPoint ټکی)
{
    BMessage *m = کړکۍ()->Current_پیغام();
    أكد(m);

    int32 buttons = 0;
    m->پیدا_کړه_Int32("buttons", &buttons);

    صحيح vimButton;

    إذا (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_کیڼ;
    وإلا إذا (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_ښی;
    وإلا إذا (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_منځ;
    وإلا
	أرجع;		// Unknown button

    vimMouseButton = 1;	    // don't care which one

    // Handle multiple clicks
    int32 clicks = 0;
    m->پیدا_کړه_Int32("clicks", &clicks);

    int32 modifiers = 0;
    m->پیدا_کړه_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_منزل_Vim(modifiers);

    guiSendMouseEvent(vimButton, ټکی.x, ټکی.y,
	    clicks > 1 /* = repeated_click*/, vimMouseModifiers);
}

    فراغ
Vim_متن_Area_View::Mouse_پورته(BPoint ټکی)
{
    vimMouseButton = 0;

    BMessage *m = کړکۍ()->Current_پیغام();
    أكد(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->پیدا_کړه_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_منزل_Vim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, ټکی.x, ټکی.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::Mouse_پورته(ټکی);
}

    فراغ
Vim_متن_Area_View::MouseMoved(BPoint ټکی, uint32 transit, ثابت BMessage *پیغام_2)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    إذا (gui.اشاره_ګر_hidden)
    {
	guiBlankMouse(خطأ);
	gui.اشاره_ګر_hidden = FALSE;
    }

    إذا (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(ټکی.x, ټکی.y);
	أرجع;
    }

    atomic_جمع_کړه(&mouse_Drag_Event_شمېر, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, ټکی.x, ټکی.y, 0, vimMouseModifiers);
}

    فراغ
Vim_متن_Area_View::پیغام_Received(BMessage *m)
{
    بدل (m->what) {
	حالة 'menu':
	    {
		Vim_Menu_پیغام mm;
		mm.guiMenu = عدم;  // in case no pointer in msg
		m->پیدا_کړه_اشاره_ګر("VimMenu", (فراغ **)&mm.guiMenu);
		write_پورټ(gui.vdcmp, Vim_پیغام::Menu, &mm, حجم(mm));
	    }
	    اكسر;
	حالة B_MOUSE_WHEEL_بدل_شوی:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		عائم small=0, big=0, dy=0;
		m->پیدا_کړه_Float("be:wheel_delta_y", &dy);
		scb->Get_ګامونه(&small, &big);
		scb->سېټ_ارزښت(scb->ارزښت_2()+small*dy*3);
		scb->ارزښت_بدل_شوی_2(scb->ارزښت_2());
#إذا 0
		scb = curwin->w_scrollbars[0].id;
		scb->Get_ګامونه(&small, &big);
		scb->سېټ_ارزښت(scb->ارزښت_2()+small*dy);
		scb->ارزښت_بدل_شوی_2(scb->ارزښت_2());
#د_که_پای
	    }
	    اكسر;
#که_تعریف_شوی FEAT_MBYTE_IME
	حالة B_ننوت_لاره_EVENT:
	    {
		int32 opcode;
		m->پیدا_کړه_Int32("be:opcode", &opcode);
		بدل(opcode)
		{
		    حالة B_ننوت_لاره_STARTED:
			إذا (!IMData.messenger) ړنګول IMData.messenger;
			IMData.messenger = جديد BMessenger();
			m->پیدا_کړه_Messenger("be:reply_to", IMData.messenger);
			اكسر;
		    حالة B_ننوت_لاره_بدل_شوی:
			{
			    BString str;
			    منطقي confirmed;
			    إذا (IMData.پیغام_2) *(IMData.پیغام_2) = *m;
			    وإلا	       IMData.پیغام_2 = جديد BMessage(*m);
			    DrawIMString();
			    m->پیدا_کړه_Bool("be:confirmed", &confirmed);
			    إذا (confirmed)
			    {
				m->پیدا_کړه_String("be:string", &str);
				char_u *chars = (char_u*)str.String();
				بنية Vim_کلی_پیغام km;
				km.csi_escape = ریښتیا;
				صحيح clen;
				صحيح i = 0;
				طالما (i < str.اوږدتیا_2())
				{
				    clen = utf_ptr2len(chars+i);
				    حافظه_نقل(km.chars, chars+i, clen);
				    km.اوږدتیا = clen;
				    write_پورټ(gui.vdcmp, Vim_پیغام::کلی_2, &km, حجم(km));
				    i += clen;
				}
			    }
			}
			اكسر;
		    حالة B_ننوت_لاره_LOCATION_REQUEST:
			{
			    BMessage پیغام(B_ننوت_لاره_EVENT);
			    پیغام.جمع_کړه_Int32("be:opcode", B_ننوت_لاره_LOCATION_REQUEST);
			    پیغام.جمع_کړه_ټکی("be:location_reply", IMData.location);
			    پیغام.جمع_کړه_Float("be:height_reply", FILL_Y(1));
			    IMData.messenger->Send_پیغام(&پیغام);
			}
			اكسر;
		    حالة B_ننوت_لاره_STOPPED:
			ړنګول IMData.messenger;
			ړنګول IMData.پیغام_2;
			IMData.messenger = عدم;
			IMData.پیغام_2 = عدم;
			اكسر;
		}
	    }
	    // TODO: sz: break here???
#د_که_پای
	افتراضي:
	    إذا (m->WasDropped()) {
		BWindow *w = کړکۍ();
		w->Detach_Current_پیغام();
		w->Minimize(خطأ);
		VimApp::SendRefs(m, (modifiers() & B_ښويدنه_کلی) != 0);
	    } وإلا {
		Inherited::پیغام_Received(m);
	    }
	    اكسر;
    }
}

    صحيح
Vim_متن_Area_View::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    إذا (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_سېټ_font((GuiFont)newFont);
	إذا (name && STRCMP(name, "*") != 0)
	    hl_سېټ_font_name(name);

	سېټ_Drawing_ډېرپېښ(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	أرجع OK;
    }
    أرجع FAIL;
}

    فراغ
Vim_متن_Area_View::mchDrawString(صحيح لیکه, صحيح ستون, char_u *s, صحيح len, صحيح flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    إذا (!(flags & DRAW_TRANSP)) {
	صحيح cells;
	cells = 0;
	لكل (صحيح i=0; i<len; i++) {
	    صحيح cn = utf_ptr2cells((char_u *)(s+i));
	    إذا (cn<4) cells += cn;
	}

	BRect r(FILL_X(ستون), FILL_Y(لیکه),
		FILL_X(ستون + cells) - PEN_پلنوالی, FILL_Y(لیکه + 1) - PEN_پلنوالی);
	FillRect(r, B_SOLID_ټیټ);
    }

    BFont font;
    هذا->GetFont(&font);
    إذا (!font.IsFixed())
    {
	حرف* p = (حرف*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	صحيح cells;
	طالما ((p - (حرف*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.سېټ(متن_X(ستون+lastpos), متن_Y(لیکه));
	    DrawString(p, clen, where);
	    إذا (flags & DRAW_BOLD) {
		where.x += 1.0;
		سېټ_Drawing_ډېرپېښ(B_OP_BLEND);
		DrawString(p, clen, where);
		سېټ_Drawing_ډېرپېښ(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    إذا (cells<4) lastpos += cells;
	    وإلا	lastpos++;
	    p += clen;
	}
    }
    وإلا
    {
	BPoint where(متن_X(ستون), متن_Y(لیکه));
	DrawString((حرف*)s, len, where);
	إذا (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    سېټ_Drawing_ډېرپېښ(B_OP_BLEND);
	    DrawString((حرف*)s, len, where);
	    سېټ_Drawing_ډېرپېښ(B_OP_COPY);
	}
    }

    إذا (flags & DRAW_UNDERL) {
	صحيح cells;
	cells = 0;
	لكل (صحيح i=0; i<len; i++) {
	    صحيح cn = utf_ptr2cells((char_u *)(s+i));
	    إذا (cn<4) cells += cn;
	}

	BPoint پیل(FILL_X(ستون), FILL_Y(لیکه + 1) - PEN_پلنوالی);
	BPoint پای_ټکی(FILL_X(ستون + cells) - PEN_پلنوالی, پیل.y);

	Stroke_کرښه(پیل, پای_ټکی);
    }
}

فراغ
Vim_متن_Area_View::mch_Clear_بلاک(
	صحيح	row1,
	صحيح	col1,
	صحيح	row2,
	صحيح	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_پلنوالی, FILL_Y(row2 + 1) - PEN_پلنوالی);
    gui_mch_سېټ_bg_color(gui.شا_pixel);
    FillRect(r, B_SOLID_ټیټ);
}

    فراغ
Vim_متن_Area_View::mchClearAll()
{
    gui_mch_سېټ_bg_color(gui.شا_pixel);
    FillRect(Bounds(), B_SOLID_ټیټ);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    فراغ
Vim_متن_Area_View::mch_Delete_کرښې(صحيح لیکه, صحيح عدد_کرښې)
{
    BRect سرچینه, dest;
    سرچینه.کیڼ = FILL_X(gui.scroll_region_کیڼ);
    سرچینه.سر_2 = FILL_Y(لیکه + عدد_کرښې);
    سرچینه.ښی = FILL_X(gui.scroll_region_ښی + 1) - PEN_پلنوالی;
    سرچینه.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_پلنوالی;

    dest.کیڼ = FILL_X(gui.scroll_region_کیڼ);
    dest.سر_2 = FILL_Y(لیکه);
    dest.ښی = FILL_X(gui.scroll_region_ښی + 1) - PEN_پلنوالی;
    dest.bottom = FILL_Y(gui.scroll_region_bot - عدد_کرښې + 1) - PEN_پلنوالی;

    إذا (gui.vim_کړکۍ->قلف_2()) {
	// Clear one column more for when bold has spilled over
	Copy_بټونه(سرچینه, dest);
	gui_clear_بلاک(gui.scroll_region_bot - عدد_کرښې + 1,
		gui.scroll_region_کیڼ,
		gui.scroll_region_bot, gui.scroll_region_ښی);


	gui.vim_کړکۍ->Unlock();
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
    فراغ
Vim_متن_Area_View::mch_ورننباسه_کرښې(صحيح لیکه, صحيح عدد_کرښې)
{
    BRect سرچینه, dest;

    // XXX Attempt at a hack:
    gui.vim_کړکۍ->UpdateIfNeeded();
    سرچینه.کیڼ = FILL_X(gui.scroll_region_کیڼ);
    سرچینه.سر_2 = FILL_Y(لیکه);
    سرچینه.ښی = FILL_X(gui.scroll_region_ښی + 1) - PEN_پلنوالی;
    سرچینه.bottom = FILL_Y(gui.scroll_region_bot - عدد_کرښې + 1) - PEN_پلنوالی;

    dest.کیڼ = FILL_X(gui.scroll_region_کیڼ);
    dest.سر_2 = FILL_Y(لیکه + عدد_کرښې);
    dest.ښی = FILL_X(gui.scroll_region_ښی + 1) - PEN_پلنوالی;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_پلنوالی;

    إذا (gui.vim_کړکۍ->قلف_2()) {
	// Clear one column more for when bold has spilled over
	Copy_بټونه(سرچینه, dest);
	gui_clear_بلاک(لیکه, gui.scroll_region_کیڼ,
		لیکه + عدد_کرښې - 1, gui.scroll_region_ښی);

	gui.vim_کړکۍ->Unlock();
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

#که_تعریف_شوی FEAT_MBYTE_IME
/*
 * DrawIMString draws string with IMData.message.
 */
فراغ Vim_متن_Area_View::DrawIMString(فراغ)
{
    ساكن ثابت rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    ثابت حرف* s;
    صحيح len;
    BMessage* پیغام = IMData.پیغام_2;
    إذا (!پیغام)
	أرجع;
    gui_redraw_بلاک(IMData.لیکه, 0,
	    IMData.لیکه + IMData.شمېر, W_پلنوالی(curwin), GUI_MON_NOCLEAR);
    منطقي confirmed = خطأ;
    پیغام->پیدا_کړه_Bool("be:confirmed", &confirmed);
    إذا (confirmed)
	أرجع;
    rgb_color hcolor = لوړ_Color(), lcolor = ټیټ_Color();
    پیغام->پیدا_کړه_String("be:string", &str);
    s = str.String();
    len = str.اوږدتیا_2();
    سېټ_لوړ_Color(0, 0, 0);
    IMData.لیکه = gui.لیکه;
    IMData.ستون = gui.ستون;
    int32 sel_پیل = 0, sel_پای_ټکی = 0;
    پیغام->پیدا_کړه_Int32("be:selection", 0, &sel_پیل);
    پیغام->پیدا_کړه_Int32("be:selection", 1, &sel_پای_ټکی);
    صحيح clen, cn;
    BPoint pos(IMData.ستون, 0);
    BRect r;
    BPoint where;
    IMData.location = Convert_منزل_Screen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.لیکه + pos.y)));
    لكل (صحيح i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	إذا (pos.x + cn > W_پلنوالی(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	إذا (sel_پیل<=i && i<sel_پای_ټکی)
	{
	    سېټ_ټیټ_Color(r_highlight);
	    IMData.location = Convert_منزل_Screen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.لیکه + pos.y)));
	}
	وإلا
	{
	    سېټ_ټیټ_Color(b_highlight);
	}
	r.سېټ(FILL_X(pos.x), FILL_Y(IMData.لیکه + pos.y),
		FILL_X(pos.x + cn) - PEN_پلنوالی,
		FILL_Y(IMData.لیکه + pos.y + 1) - PEN_پلنوالی);
	FillRect(r, B_SOLID_ټیټ);
	where.سېټ(متن_X(pos.x), متن_Y(IMData.لیکه + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.شمېر = (صحيح)pos.y;

    سېټ_لوړ_Color(hcolor);
    سېټ_ټیټ_Color(lcolor);
}
#د_که_پای
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
	    "vim scrollbar", (BView *)عدم,
	    0.0, 10.0, posture),
    ignore_ارزښت(-1),
    scroll_Event_شمېر(0)
{
    gsb = g;
    سېټ_Resizing_ډېرپېښ(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    فراغ
VimScrollBar::ارزښت_بدل_شوی_2(عائم new_ارزښت)
{
    إذا (ignore_ارزښت >= 0.0 && new_ارزښت == ignore_ارزښت) {
	ignore_ارزښت = -1;
	أرجع;
    }
    ignore_ارزښت = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_جمع_کړه(&scroll_Event_شمېر, 1);

    بنية Vim_Scroll_Bar_پیغام sm;

    sm.sb = هذا;
    sm.ارزښت = (اوږد) new_ارزښت;
    sm.stillDragging = TRUE;

    write_پورټ(gui.vdcmp, Vim_پیغام::ScrollBar, &sm, حجم(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    فراغ
VimScrollBar::Mouse_پورته(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_جمع_کړه(&scroll_Event_شمېر, 1);

    بنية Vim_Scroll_Bar_پیغام sm;

    sm.sb = هذا;
    sm.ارزښت = (اوږد) ارزښت_2();
    sm.stillDragging = FALSE;

    write_پورټ(gui.vdcmp, Vim_پیغام::ScrollBar, &sm, حجم(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::Mouse_پورته(where);
}

    فراغ
VimScrollBar::سېټ_ارزښت(عائم new_ارزښت)
{
    إذا (new_ارزښت == ارزښت_2())
	أرجع;

    ignore_ارزښت = new_ارزښت;
    Inherited::سېټ_ارزښت(new_ارزښت);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(ثابت VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ثابت BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ثابت VimFont &rhs): BFont(rhs)
{
    init();
}

VimFont::~VimFont()
{
}

    فراغ
VimFont::init()
{
    بل = عدم;
    refcount = 1;
    name = عدم;
}

// ---------------- VimDialog ----------------

#إذا defined(FEAT_GUI_DIALOG)

ثابت بینښه صحيح  k_Vim_Dialog_Button_پیغام = 'VMDB';
ثابت بینښه صحيح  k_Vim_Dialog_Icon_Stripe_پلنوالی = 30;
ثابت بینښه صحيح  kVimDialogButtonsSpacingX = 9;
ثابت بینښه صحيح  kVimDialogButtonsSpacingY = 4;
ثابت بینښه صحيح  kVimDialogSpacingX = 6;
ثابت بینښه صحيح  kVimDialogSpacingY = 10;
ثابت بینښه صحيح  k_Vim_Dialog_اصغري_پلنوالی  = 310;
ثابت بینښه صحيح  k_Vim_Dialog_اصغري_لوړوالی = 75;
ثابت BRect	    kDefaultRect =
BRect(0, 0, k_Vim_Dialog_اصغري_پلنوالی, k_Vim_Dialog_اصغري_لوړوالی);

VimDialog::VimDialog(صحيح type, ثابت حرف *title, ثابت حرف *پیغام_2,
	ثابت حرف *buttons, صحيح dfltbutton, ثابت حرف *textfield, صحيح ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_کړکۍ_LOOK, B_MODAL_APP_کړکۍ_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_ارزښت(dfltbutton)
    , f_پیغام_View(عدم)
    , f_ننوت_Control(عدم)
    , f_ننوت_ارزښت(textfield)
{
    //	master view
    VimDialog::View* view = جديد VimDialog::View(Bounds());
    إذا (view == عدم)
	أرجع;

    إذا (title == عدم)
	سېټ_Title("Vim " VIM_نسخه_MEDIUM);

    جمع_کړه_ماشوم(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    عائم max_Button_پلنوالی  = 0;
    عائم max_Button_لوړوالی = 0;
    عائم buttons_پلنوالی	  = 0;
    عائم buttons_لوړوالی   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    افعل
    {
	int32 پای_ټکی = strButtons.پیدا_کړه_لومړی('\n');
	إذا (پای_ټکی != B_ERROR)
	    strButtons.سېټ_Byte_At(پای_ټکی, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->جمع_کړه_ماشوم(button);
	fButtonsList.جمع_کړه_توکی(button);

	max_Button_پلنوالی	= max_c(max_Button_پلنوالی,  button->Bounds().پلنوالی_2());
	max_Button_لوړوالی = max_c(max_Button_لوړوالی, button->Bounds().لوړوالی_2());
	buttons_پلنوالی   += button->Bounds().پلنوالی_2();
	buttons_لوړوالی  += button->Bounds().لوړوالی_2();

	إذا (پای_ټکی == B_ERROR)
	    اكسر;

	strButtons.Remove(0, پای_ټکی + 1);
    } طالما (ریښتیا);

    int32 buttons_شمېر = fButtonsList.شمېر_توکي();
    buttons_پلنوالی      += kVimDialogButtonsSpacingX * (buttons_شمېر - 1);
    buttons_لوړوالی     += kVimDialogButtonsSpacingY * (buttons_شمېر - 1);
    عائم dialog_پلنوالی  = buttons_پلنوالی + k_Vim_Dialog_Icon_Stripe_پلنوالی +
	kVimDialogSpacingX * 2;
    عائم dialog_لوړوالی = max_Button_لوړوالی + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    منطقي vertical = (vim_strchr(p_go, GO_VERTICAL) != عدم) ||
	dialog_پلنوالی >= gui.vim_کړکۍ->Bounds().پلنوالی_2();
    إذا (vertical) {
	dialog_پلنوالی  -= buttons_پلنوالی;
	dialog_پلنوالی  += max_Button_پلنوالی;
	dialog_لوړوالی -= max_Button_لوړوالی;
	dialog_لوړوالی += buttons_لوړوالی;
    }

    dialog_پلنوالی  = max_c(dialog_پلنوالی,  k_Vim_Dialog_اصغري_پلنوالی);

    //	message view
    BRect rect(0, 0, dialog_پلنوالی, 0);
    rect.کیڼ  += k_Vim_Dialog_Icon_Stripe_پلنوالی + 16 + kVimDialogSpacingX;
    rect.سر_2   += kVimDialogSpacingY;
    rect.ښی -= kVimDialogSpacingX;
    rect.bottom = rect.سر_2;
    f_پیغام_View = جديد BTextView(rect, "_tv_", rect.دننه_واټن_By_Copy(B_ORIGIN),
	    B_FOLLOW_کیڼ | B_FOLLOW_سر, B_WILL_DRAW);

    f_پیغام_View->سېټ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color متن_Color = ui_color(B_PANEL_متن_COLOR);
    f_پیغام_View->سېټ_Font_And_Color(be_plain_font, B_FONT_ALL, &متن_Color);
    f_پیغام_View->سېټ_متن(پیغام_2);
    f_پیغام_View->MakeEditable(خطأ);
    f_پیغام_View->MakeSelectable(خطأ);
    f_پیغام_View->سېټ_کلمه_Wrap(ریښتیا);
    جمع_کړه_ماشوم(f_پیغام_View);

    عائم پیغام_لوړوالی = f_پیغام_View->متن_لوړوالی(0, f_پیغام_View->شمېر_کرښې());
    f_پیغام_View->ResizeBy(0, پیغام_لوړوالی);
    f_پیغام_View->سېټ_متن_Rect(BRect(0, 0, rect.پلنوالی_2(), پیغام_لوړوالی));

    dialog_لوړوالی += پیغام_لوړوالی;

    //	input view
    إذا (f_ننوت_ارزښت != عدم) {
	rect.سر_2     =
	    rect.bottom += پیغام_لوړوالی + kVimDialogSpacingY;
	f_ننوت_Control = جديد BTextControl(rect, "_iv_", عدم, f_ننوت_ارزښت, عدم,
		B_FOLLOW_کیڼ | B_FOLLOW_سر, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_ننوت_Control->متن_View()->سېټ_متن(f_ننوت_ارزښت);
	f_ننوت_Control->متن_View()->سېټ_کلمه_Wrap(خطأ);
	جمع_کړه_ماشوم(f_ننوت_Control);

	عائم پلنوالی = 0.f, لوړوالی = 0.f;
	f_ننوت_Control->Get_Preferred_اندازه(&پلنوالی, &لوړوالی);
	f_ننوت_Control->MakeFocus(ریښتیا);

	dialog_لوړوالی += لوړوالی + kVimDialogSpacingY * 1.5;
    }

    dialog_لوړوالی = max_c(dialog_لوړوالی, k_Vim_Dialog_اصغري_لوړوالی);

    Resize_منزل(dialog_پلنوالی, dialog_لوړوالی);
    Move_منزل((gui.vim_کړکۍ->Bounds().پلنوالی_2() - dialog_پلنوالی) / 2,
	    (gui.vim_کړکۍ->Bounds().لوړوالی_2() - dialog_لوړوالی) / 2);

    //	adjust layout of buttons
    عائم button_پلنوالی = max_c(max_Button_پلنوالی, rect.پلنوالی_2() * 0.66);
    BPoint origin(dialog_پلنوالی, dialog_لوړوالی);
    origin.x -= kVimDialogSpacingX + (vertical ? button_پلنوالی : buttons_پلنوالی);
    origin.y -= kVimDialogSpacingY + (vertical ? buttons_لوړوالی	: max_Button_لوړوالی);

    لكل (int32 i = 0 ; i < buttons_شمېر; i++) {
	BButton *button = (BButton*)fButtonsList.توکی_At(i);
	button->Move_منزل(origin);
	إذا (vertical) {
	    origin.y += button->چوکاټ_2().لوړوالی_2() + kVimDialogButtonsSpacingY;
	    button->Resize_منزل(button_پلنوالی, button->چوکاټ_2().لوړوالی_2());
	} وإلا
	    origin.x += button->چوکاټ_2().پلنوالی_2() + kVimDialogButtonsSpacingX;

	إذا (dfltbutton == i + 1) {
	    button->MakeDefault(ریښتیا);
	    button->MakeFocus(f_ننوت_Control == عدم);
	}
    }
}

VimDialog::~VimDialog()
{
    إذا (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    صحيح
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    إذا (fDialogSem < B_OK) {
	Quit();
	أرجع f_Dialog_ارزښت;
    }

    Show();

    طالما (acquire_sem(fDialogSem) == B_INTERRUPTED);

    صحيح ret_ارزښت = f_Dialog_ارزښت;
    إذا (f_ننوت_ارزښت != عدم)
	vim_strncpy((char_u*)f_ننوت_ارزښت, (char_u*)f_ننوت_Control->متن_2(), IOSIZE - 1);

    إذا (قلف_2())
	Quit();

    أرجع ret_ارزښت;
}

فراغ VimDialog::پیغام_Received(BMessage *پیغام)
{
    int32 which = 0;
    إذا (پیغام->what != k_Vim_Dialog_Button_پیغام ||
	    پیغام->پیدا_کړه_Int32("which", &which) != B_OK)
	أرجع BWindow::پیغام_Received(پیغام);

    f_Dialog_ارزښت = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, ثابت حرف* label)
{
    BMessage *پیغام_2 = جديد BMessage(k_Vim_Dialog_Button_پیغام);
    پیغام_2->جمع_کړه_Int32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = جديد BButton(rect, name.String(), label, پیغام_2,
	    B_FOLLOW_ښی | B_FOLLOW_BOTTOM);

    عائم پلنوالی = 0.f, لوړوالی = 0.f;
    button->Get_Preferred_اندازه(&پلنوالی, &لوړوالی);
    button->Resize_منزل(پلنوالی, لوړوالی);

    أرجع button;
}

VimDialog::View::View(BRect چوکاټ)
    :	BView(چوکاټ, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(عدم)
{
    سېټ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    ړنګول fIconBitmap;
}

فراغ VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.ښی = k_Vim_Dialog_Icon_Stripe_پلنوالی;
    سېټ_لوړ_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    إذا (fIconBitmap == عدم)
	أرجع;

    سېټ_Drawing_ډېرپېښ(B_OP_ALPHA);
    سېټ_Blending_ډېرپېښ(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

فراغ VimDialog::View::InitIcon(int32 type)
{
    إذا (type == VIM_GENERIC)
	أرجع;

    BPath لار;
    حالت_t حالت = پیدا_کړه_directory(B_BEOS_SERVERS_DIRECTORY, &لار);
    إذا (حالت != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(حالت));
	أرجع;
    }

    لار.Append("app_server");

    BFile file(لار.لار_2(), O_RDONLY);
    إذا (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	أرجع;
    }

    BResources resources(&file);
    إذا (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	أرجع;
    }

    ثابت حرف *name = "";
    بدل(type) {
	حالة VIM_ERROR:	    name = "stop"; اكسر;
	حالة VIM_WARNING:   name = "warn"; اكسر;
	حالة VIM_INFO:	    name = "info"; اكسر;
	حالة VIM_QUESTION:  name = "idea"; اكسر;
	افتراضي: أرجع;
    }

    int32 icon_اندازه_2 = 32;
    fIconBitmap = جديد BBitmap(BRect(0, 0, icon_اندازه_2 - 1, icon_اندازه_2 - 1), 0, B_RGBA32);
    إذا (fIconBitmap == عدم || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == عدم) ? "null" : strerror(fIconBitmap->InitCheck()));
	أرجع;
    }

    size_t اندازه = 0;
    ثابت uint8* icon_معلومات = عدم;
    //	try vector icon first?
    icon_معلومات = (ثابت uint8*)resources.بارول_Resource(B_ویکټور_ICON_TYPE, name, &اندازه);
    إذا (icon_معلومات != عدم && BIconUtils::Get_ویکټور_Icon(icon_معلومات, اندازه, fIconBitmap) == B_OK)
	أرجع;

    //	try bitmap icon now
    icon_معلومات = (ثابت uint8*)resources.بارول_Resource(B_LARGE_ICON_TYPE, name, &اندازه);
    إذا (icon_معلومات == عدم) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	ړنګول fIconBitmap;
	fIconBitmap = عدم;
	أرجع;
    }

    إذا (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(icon_معلومات, icon_اندازه_2, icon_اندازه_2, icon_اندازه_2, fIconBitmap);
}

ثابت بینښه صحيح  k_Vim_Dialog_OKButton_پیغام = 'FDOK';
ثابت بینښه صحيح  k_Vim_Dialog_Cancel_Button_پیغام = 'FDCN';
ثابت بینښه صحيح  k_Vim_Dialog_اندازه_ننوت_پیغام = 'SICH';
ثابت بینښه صحيح  k_Vim_Dialog_Family_Select_پیغام = 'MSFM';
ثابت بینښه صحيح  k_Vim_Dialog_Style_Select_پیغام = 'MSST';
ثابت بینښه صحيح  k_Vim_Dialog_اندازه_Select_پیغام = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, عائم* اندازه)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_کړکۍ_LOOK, B_MODAL_APP_کړکۍ_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , f_حالت(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_ارزښت(خطأ)
    , fFamily(family)
    , fStyle(style)
    , f_اندازه(اندازه)
    , f_Font_اندازه(*اندازه)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_ننوت(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_اوږدتیا);
    strncpy(fFontStyle, *style, B_FONT_STYLE_اوږدتیا);

    //	"client" area view
    BBox *clientBox = جديد BBox(Bounds(), B_تشه_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_چوکاټ_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    جمع_کړه_ماشوم(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.کیڼ_سر(), RC.کیڼ_سر());

    //	at first create all controls
    fPreview = جديد BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->جمع_کړه_ماشوم(fPreview);

    BBox* boxDivider = جديد BBox(rc, B_تشه_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->جمع_کړه_ماشوم(boxDivider);

    BStringView *labelFamily = جديد BStringView(rc, "labelFamily", "Family:");
    clientBox->جمع_کړه_ماشوم(labelFamily);
    labelFamily->Resize_منزل_Preferred();

    BStringView *labelStyle = جديد BStringView(rc, "labelStyle", "Style:");
    clientBox->جمع_کړه_ماشوم(labelStyle);
    labelStyle->Resize_منزل_Preferred();

    BStringView *label_اندازه = جديد BStringView(rc, "labelSize", "Size:");
    clientBox->جمع_کړه_ماشوم(label_اندازه);
    label_اندازه->Resize_منزل_Preferred();

    fFamiliesList = جديد BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = جديد BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_کیڼ_ښی, 0, خطأ, ریښتیا);
    clientBox->جمع_کړه_ماشوم(scrollFamilies);

    fStylesList= جديد BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = جديد BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_کیڼ_ښی, 0, خطأ, ریښتیا);
    clientBox->جمع_کړه_ماشوم(scrollStyles);

    f_Sizes_ننوت = جديد BTextControl(rc, "inputSize", عدم, "???",
	    جديد BMessage(k_Vim_Dialog_اندازه_ننوت_پیغام));
    clientBox->جمع_کړه_ماشوم(f_Sizes_ننوت);
    f_Sizes_ننوت->Resize_منزل_Preferred();

    fSizesList = جديد BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = جديد BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_کیڼ_ښی, 0, خطأ, ریښتیا);
    clientBox->جمع_کړه_ماشوم(scrollSizes);

    BButton *buttonOK = جديد BButton(rc, "buttonOK", "OK",
			جديد BMessage(k_Vim_Dialog_OKButton_پیغام));
    clientBox->جمع_کړه_ماشوم(buttonOK);
    buttonOK->Resize_منزل_Preferred();

    BButton *buttonCancel = جديد BButton(rc, "buttonCancel", "Cancel",
			جديد BMessage(k_Vim_Dialog_Cancel_Button_پیغام));
    clientBox->جمع_کړه_ماشوم(buttonCancel);
    buttonCancel->Resize_منزل_Preferred();

    //	layout controls
    عائم کرښه_لوړوالی = labelFamily->Bounds().لوړوالی_2();
    عائم preview_لوړوالی = کرښه_لوړوالی * 3;
    عائم دننه_واټن_YLabels = preview_لوړوالی + kVimDialogSpacingY;
    عائم دننه_واټن_YLists = دننه_واټن_YLabels + کرښه_لوړوالی + kVimDialogSpacingY / 2;
    عائم دننه_واټن_YSizes = دننه_واټن_YLists + f_Sizes_ننوت->Bounds().لوړوالی_2() + kVimDialogSpacingY / 2;
    عائم lists_لوړوالی = کرښه_لوړوالی * 9;
    عائم دننه_واټن_YButtons = دننه_واټن_YLists + lists_لوړوالی +  kVimDialogSpacingY;
    عائم max_Controls_لوړوالی = دننه_واټن_YButtons + buttonOK->Bounds().لوړوالی_2();
    عائم families_پلنوالی = labelFamily->Bounds().پلنوالی_2() * 5;
    عائم دننه_واټن_XStyles = families_پلنوالی + kVimDialogSpacingX;
    عائم styles_پلنوالی = labelStyle->Bounds().پلنوالی_2() * 4;
    عائم دننه_واټن_XSizes = دننه_واټن_XStyles + styles_پلنوالی + kVimDialogSpacingX;
    عائم sizes_پلنوالی = label_اندازه->Bounds().پلنوالی_2() * 2;
    عائم max_Controls_پلنوالی = دننه_واټن_XSizes + sizes_پلنوالی;

    Resize_منزل(max_Controls_پلنوالی + kVimDialogSpacingX * 2,
	max_Controls_لوړوالی + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vim_کړکۍ->چوکاټ_2();
    Move_منزل(rcVim.کیڼ + (rcVim.پلنوالی_2() - چوکاټ_2().پلنوالی_2()) / 2,
	    rcVim.سر_2 + (rcVim.لوړوالی_2() - چوکاټ_2().لوړوالی_2()) / 2);

    fPreview->Resize_منزل(max_Controls_پلنوالی, preview_لوړوالی);
    fPreview->سېټ_Alignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, preview_لوړوالی + kVimDialogSpacingY / 2);
    boxDivider->Resize_منزل(max_Controls_پلنوالی, 1.f);

    labelFamily->MoveBy(0.f, دننه_واټن_YLabels);
    labelStyle->MoveBy(دننه_واټن_XStyles, دننه_واټن_YLabels);
    label_اندازه->MoveBy(دننه_واټن_XSizes, دننه_واټن_YLabels);

    //	text control alignment issues
    عائم insetX = f_Sizes_ننوت->متن_View()->Bounds().پلنوالی_2() - f_Sizes_ننوت->Bounds().پلنوالی_2();
    عائم insetY = f_Sizes_ننوت->متن_View()->Bounds().پلنوالی_2() - f_Sizes_ننوت->Bounds().پلنوالی_2();

    scrollFamilies->MoveBy(0.f, دننه_واټن_YLists);
    scrollStyles->MoveBy(دننه_واټن_XStyles, دننه_واټن_YLists);
    f_Sizes_ننوت->MoveBy(دننه_واټن_XSizes + insetX / 2, دننه_واټن_YLists + insetY / 2);
    scrollSizes->MoveBy(دننه_واټن_XSizes, دننه_واټن_YSizes);

    f_Sizes_ننوت->سېټ_Alignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->Resize_منزل(families_پلنوالی, lists_لوړوالی);
    scrollStyles->Resize_منزل(styles_پلنوالی, lists_لوړوالی);
    f_Sizes_ننوت->Resize_منزل(sizes_پلنوالی, f_Sizes_ننوت->Bounds().لوړوالی_2());
    scrollSizes->Resize_منزل(sizes_پلنوالی,
	    lists_لوړوالی - (دننه_واټن_YSizes - دننه_واټن_YLists));

    buttonOK->MoveBy(max_Controls_پلنوالی - buttonOK->Bounds().پلنوالی_2(), دننه_واټن_YButtons);
    buttonCancel->MoveBy(max_Controls_پلنوالی - buttonOK->Bounds().پلنوالی_2()
	    - buttonCancel->Bounds().پلنوالی_2() - kVimDialogSpacingX, دننه_واټن_YButtons);

    //	fill lists
    صحيح sel_شاخص = -1;
    صحيح شمېر = شمېر_font_families();
    لكل (صحيح i = 0; i < شمېر; i++) {
	font_family family;
	إذا (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->جمع_کړه_توکی(جديد BString_توکی((ثابت حرف*)family));
	    إذا (مزي_پرتله(family, fFontFamily, B_FONT_FAMILY_اوږدتیا) == 0)
		sel_شاخص = i;
	}
    }

    إذا (sel_شاخص >= 0) {
	fFamiliesList->Select(sel_شاخص);
	fFamiliesList->Scroll_منزل_Selection();
    }

    _UpdateFontStyles();

    sel_شاخص = -1;
    لكل (صحيح اندازه = 8, شاخص = 0; اندازه <= 18; اندازه++, شاخص++) {
	BString str;
	str << اندازه;
	fSizesList->جمع_کړه_توکی(جديد BString_توکی(str));
	إذا (اندازه == f_Font_اندازه)
	    sel_شاخص = شاخص;

    }

    إذا (sel_شاخص >= 0) {
	fSizesList->Select(sel_شاخص);
	fSizesList->Scroll_منزل_Selection();
    }

    fFamiliesList->سېټ_Selection_پیغام(جديد BMessage(k_Vim_Dialog_Family_Select_پیغام));
    fStylesList->سېټ_Selection_پیغام(جديد BMessage(k_Vim_Dialog_Style_Select_پیغام));
    fSizesList->سېټ_Selection_پیغام(جديد BMessage(k_Vim_Dialog_اندازه_Select_پیغام));
    f_Sizes_ننوت->سېټ_Modification_پیغام(جديد BMessage(k_Vim_Dialog_اندازه_ننوت_پیغام));

    Update_اندازه_ننوت_Preview();
    _UpdateFontPreview();

    f_حالت = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    إذا (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    فراغ
VimSelectFontDialog::_CleanList(BListView* list)
{
    طالما (0 < list->شمېر_توکي())
	ړنګول (dynamic_cast<BString_توکی*>(list->Remove_توکی((int32)0)));
}

    منطقي
VimSelectFontDialog::Go()
{
    إذا (f_حالت != B_OK) {
	Quit();
	أرجع NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    إذا (fDialogSem < B_OK) {
	Quit();
	أرجع f_Dialog_ارزښت;
    }

    Show();

    طالما (acquire_sem(fDialogSem) == B_INTERRUPTED);

    منطقي ret_ارزښت = f_Dialog_ارزښت;

    إذا (قلف_2())
	Quit();

    أرجع ret_ارزښت;
}


فراغ VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 sel_شاخص = -1;
    int32 شمېر = شمېر_font_styles(fFontFamily);
    لكل (int32 i = 0; i < شمېر; i++) {
	font_style style;
	uint32 flags = 0;
	إذا (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->جمع_کړه_توکی(جديد BString_توکی((ثابت حرف*)style));
	    إذا (مزي_پرتله(style, fFontStyle, B_FONT_STYLE_اوږدتیا) == 0)
		sel_شاخص = i;
	}
    }

    إذا (sel_شاخص >= 0) {
	fStylesList->Select(sel_شاخص);
	fStylesList->Scroll_منزل_Selection();
    } وإلا
	fStylesList->Select(0);
}


فراغ VimSelectFontDialog::Update_اندازه_ننوت_Preview()
{
    حرف buf[10] = {0};
    vim_snprintf(buf, حجم(buf), (حرف*)"%.0f", f_Font_اندازه);
    f_Sizes_ننوت->سېټ_متن(buf);
}


فراغ VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.سېټ_اندازه(f_Font_اندازه);
    font.سېټ_Family_And_Style(fFontFamily, fFontStyle);
    fPreview->سېټ_Font(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_اندازه);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (صحيح)f_Font_اندازه << " pt.";
    fPreview->سېټ_متن(str);
}


    منطقي
VimSelectFontDialog::Update_From_List_توکی(BListView* list, حرف* متن, صحيح متن_اندازه)
{
    int32 شاخص = list->CurrentSelection();
    إذا (شاخص < 0)
	أرجع خطأ;
    BString_توکی* توکی = (BString_توکی*)list->توکی_At(شاخص);
    إذا (توکی == عدم)
	أرجع خطأ;
    strncpy(متن, توکی->متن_2(), متن_اندازه);
    أرجع ریښتیا;
}


فراغ VimSelectFontDialog::پیغام_Received(BMessage *پیغام)
{
    بدل (پیغام->what) {
	حالة k_Vim_Dialog_OKButton_پیغام:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_اوږدتیا);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_اوږدتیا);
	    *f_اندازه = f_Font_اندازه;
	    f_Dialog_ارزښت = ریښتیا;
	حالة k_Vim_Dialog_Cancel_Button_پیغام:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    أرجع;
	حالة B_کلی_پورته:
	    {
		int32 کلی = 0;
		إذا (پیغام->پیدا_کړه_Int32("raw_char", &کلی) == B_OK
			&& کلی == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    اكسر;

	حالة k_Vim_Dialog_Family_Select_پیغام:
	    إذا (Update_From_List_توکی(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_اوږدتیا)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    اكسر;
	حالة k_Vim_Dialog_Style_Select_پیغام:
	    إذا (Update_From_List_توکی(fStylesList,
		    fFontStyle, B_FONT_STYLE_اوږدتیا))
		_UpdateFontPreview();
	    اكسر;
	حالة k_Vim_Dialog_اندازه_Select_پیغام:
	    {
		حرف buf[10] = {0};
		إذا (Update_From_List_توکی(fSizesList, buf, حجم(buf))) {
		    عائم اندازه = atof(buf);
		    إذا (اندازه > 0.f) {
			f_Font_اندازه = اندازه;
			Update_اندازه_ننوت_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    اكسر;
	حالة k_Vim_Dialog_اندازه_ننوت_پیغام:
	    {
		عائم اندازه = atof(f_Sizes_ننوت->متن_2());
		إذا (اندازه > 0.f) {
		    f_Font_اندازه = اندازه;
		    _UpdateFontPreview();
		}
	    }
	    اكسر;
	افتراضي:
	    اكسر;
    }
    أرجع BWindow::پیغام_Received(پیغام);
}

#د_که_پای // FEAT_GUI_DIALOG

#که_تعریف_شوی FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
ساكن BMessage * Menu_پیغام(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect چوکاټ, ثابت حرف *name) :
    BBox(چوکاټ, name, B_FOLLOW_کیڼ | B_FOLLOW_سر, B_WILL_DRAW | B_چوکاټ_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 شمېر = fButtonsList.شمېر_توکي();
    لكل (int32 i = 0; i < شمېر; i++)
	ړنګول (BPictureButton*)fButtonsList.توکی_At(i);
    fButtonsList.Make_تشه();

    ړنګول نورمال_Buttons_Bitmap;
    ړنګول grayedButtonsBitmap;
    نورمال_Buttons_Bitmap    = عدم;
    grayedButtonsBitmap  = عدم;
}

    فراغ
VimToolbar::Attached_منزل_کړکۍ()
{
    BBox::Attached_منزل_کړکۍ();

    سېټ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    عائم
VimToolbar::Toolbar_لوړوالی() ثابت
{
    عائم اندازه = عدم == نورمال_Buttons_Bitmap ? 18. : نورمال_Buttons_Bitmap->Bounds().لوړوالی_2();
    أرجع اندازه + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    منطقي
VimToolbar::Modify_Bitmap_منزل_Grayed(BBitmap *bitmap)
{
    عائم لوړوالی = bitmap->Bounds().لوړوالی_2();
    عائم پلنوالی  = bitmap->Bounds().پلنوالی_2();

    rgb_color *بټونه = (rgb_color*)bitmap->بټونه_2();
    int32 pixels = bitmap->بټونه_اوږدتیا() / 4;
    لكل (int32 i = 0; i < pixels; i++) {
	بټونه[i].red = بټونه[i].green =
	بټونه[i].blue = ((uint32)بټونه[i].red + بټونه[i].green + بټونه[i].blue) / 3;
	بټونه[i].alpha /= 4;
    }

    أرجع ریښتیا;
}

    منطقي
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    نورمال_Buttons_Bitmap = بارول_Vim_Bitmap("builtin-tools.png");
    إذا (نورمال_Buttons_Bitmap == عدم)
	//  customized not found? dig application resources for "builtin-tools" one
	نورمال_Buttons_Bitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    إذا (نورمال_Buttons_Bitmap == عدم)
	أرجع خطأ;

    BMessage archive;
    نورمال_Buttons_Bitmap->Archive(&archive);

    grayedButtonsBitmap = جديد BBitmap(&archive);
    إذا (grayedButtonsBitmap == عدم)
	أرجع خطأ;

    //	modify grayed bitmap
    Modify_Bitmap_منزل_Grayed(grayedButtonsBitmap);

    أرجع ریښتیا;
}

BBitmap *VimToolbar::بارول_Vim_Bitmap(ثابت حرف* fileName)
{
    BBitmap *bitmap = عدم;

    صحيح mustfree = 0;
    char_u* runtime_لار = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    إذا (runtime_لار != عدم && fileName != عدم) {
	BString str_لار((حرف*)runtime_لار);
	str_لار << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(str_لار.String());
    }

    إذا (mustfree)
	vim_free(runtime_لار);

    أرجع bitmap;
}

    منطقي
VimToolbar::GetPictureFromBitmap(BPicture *picture_منزل, int32 شاخص, BBitmap *bitmapFrom, منطقي pressed)
{
    عائم اندازه = bitmapFrom->Bounds().لوړوالی_2() + 1.;

    BView view(BRect(0, 0, اندازه, اندازه), "", 0, 0);

    جمع_کړه_ماشوم(&view);
    view.BeginPicture(picture_منزل);

    view.سېټ_لوړ_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.سېټ_Drawing_ډېرپېښ(B_OP_OVER);

    BRect سرچینه(0, 0, اندازه - 1, اندازه - 1);
    BRect destination(سرچینه);

    سرچینه.دننه_واټن_By(اندازه * شاخص, 0);
    destination.دننه_واټن_By(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, سرچینه, destination);

    إذا (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	اندازه += ButtonMargin * 2 - 1;
	view.Begin_کرښه_Array(4);
	view.جمع_کړه_کرښه(BPoint(0, 0),	 BPoint(اندازه, 0),    shadowColor);
	view.جمع_کړه_کرښه(BPoint(اندازه, 0),	 BPoint(اندازه, اندازه), shineColor);
	view.جمع_کړه_کرښه(BPoint(اندازه, اندازه), BPoint(0, اندازه),    shineColor);
	view.جمع_کړه_کرښه(BPoint(0, اندازه),	 BPoint(0, 0),	     shadowColor);
	view.پای_ټکی_کرښه_Array();
    }

    view.پای_ټکی_Picture();
    Remove_ماشوم(&view);

    أرجع ریښتیا;
}

    منطقي
VimToolbar::جمع_کړه_Button(int32 شاخص, vimmenu_T *menu)
{
    BPictureButton *button = عدم;
    إذا (!menu_is_separator(menu->name)) {
	عائم اندازه = نورمال_Buttons_Bitmap ?
	    نورمال_Buttons_Bitmap->Bounds().لوړوالی_2() + 1. + ButtonMargin * 2 : 18.;
	BRect چوکاټ(0, 0, اندازه, اندازه);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture picture_خړ;

	إذا (menu->iconfile == عدم && menu->iconidx >= 0 && نورمال_Buttons_Bitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, نورمال_Buttons_Bitmap, ریښتیا);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, نورمال_Buttons_Bitmap, خطأ);
	    GetPictureFromBitmap(&picture_خړ, menu->iconidx, grayedButtonsBitmap, خطأ);
	} وإلا {

	    char_u بفر[MAXPATHL] = {0};
	    BBitmap *bitmap = عدم;

	    إذا (menu->iconfile) {
		gui_پیدا_کړه_iconfile(menu->iconfile, بفر, (حرف*)"png");
		bitmap = BTranslationUtils::GetBitmap((حرف*)بفر);
	    }

	    إذا (bitmap == عدم && gui_پیدا_کړه_bitmap(menu->name, بفر, (حرف*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((حرف*)بفر);

	    إذا (bitmap == عدم)
		bitmap = جديد BBitmap(BRect(0, 0, اندازه, اندازه), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, ریښتیا);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, خطأ);
	    Modify_Bitmap_منزل_Grayed(bitmap);
	    GetPictureFromBitmap(&picture_خړ, 0, bitmap, خطأ);

	    ړنګول bitmap;
	}

	button = جديد BPictureButton(چوکاټ, (حرف*)menu->name,
		    &pictureOff, &pictureOn, Menu_پیغام(menu));

	button->سېټ_Disabled_On(&picture_خړ);
	button->سېټ_Disabled_Off(&picture_خړ);

	button->سېټ_موخه(gui.vim_متن_Area);

	جمع_کړه_ماشوم(button);

	menu->button = button;
    }

    منطقي پایله = fButtonsList.جمع_کړه_توکی(button, شاخص);
    InvalidateLayout();
    أرجع پایله;
}

    منطقي
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    إذا (menu->button) {
	إذا (fButtonsList.Remove_توکی(menu->button)) {
	    ړنګول menu->button;
	    menu->button = عدم;
	}
    }
    أرجع ریښتیا;
}

    منطقي
VimToolbar::خړ_Button(vimmenu_T *menu, صحيح grey)
{
    إذا (menu->button) {
	int32 شاخص = fButtonsList.شاخص_د(menu->button);
	إذا (شاخص >= 0)
	    menu->button->سېټ_Enabled(grey ? خطأ : ریښتیا);
    }
    أرجع ریښتیا;
}

    فراغ
VimToolbar::InvalidateLayout()
{
    int32 دننه_واټن = ToolbarMargin;
    int32 شمېر = fButtonsList.شمېر_توکي();
    لكل (int32 i = 0; i < شمېر; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.توکی_At(i);
	إذا (button) {
	    button->Move_منزل(دننه_واټن, ToolbarMargin);
	    دننه_واټن += button->Bounds().پلنوالی_2() + ToolbarMargin;
	} وإلا
	    دننه_واټن += ToolbarMargin * 3;
    }
}

#د_که_پای /*FEAT_TOOLBAR*/

#إذا defined(FEAT_GUI_TABLINE)

    عائم
Vim_Tab_کرښه::Tabline_لوړوالی() ثابت
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    أرجع Tab_لوړوالی(); //  + ToolbarMargin;
}

فراغ
Vim_Tab_کرښه::Mouse_ښکته(BPoint ټکی)
{
    إذا (!gui_mch_showing_tabline())
	أرجع;

    BMessage *m = کړکۍ()->Current_پیغام();
    أكد(m);

    int32 buttons = 0;
    m->پیدا_کړه_Int32("buttons", &buttons);

    int32 clicks = 0;
    m->پیدا_کړه_Int32("clicks", &clicks);

    صحيح شاخص = 0; //  0 means here - no tab found
    لكل (صحيح i = 0; i < شمېر_Tabs(); i++) {
	إذا (Tab_چوکاټ(i).لري_2(ټکی)) {
	    شاخص = i + 1; //  indexes are 1-based
	    اكسر;
	}
    }

    صحيح event = -1;

    إذا ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    وإلا إذا (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = شاخص > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    وإلا إذا (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPop_پورته_Menu* راوباسه_پورته_Menu = جديد BPop_پورته_Menu("tabLineContextMenu", خطأ, خطأ);
	راوباسه_پورته_Menu->جمع_کړه_توکی(جديد BMenu_توکی(_("Close tabi R"), جديد BMessage(TABLINE_MENU_CLOSE)));
	راوباسه_پورته_Menu->جمع_کړه_توکی(جديد BMenu_توکی(_("New tab    T"), جديد BMessage(TABLINE_MENU_NEW)));
	راوباسه_پورته_Menu->جمع_کړه_توکی(جديد BMenu_توکی(_("Open tab..."), جديد BMessage(TABLINE_MENU_OPEN)));

	Convert_منزل_Screen(&ټکی);
	BMenu_توکی* توکی = راوباسه_پورته_Menu->Go(ټکی);
	إذا (توکی != عدم) {
	    event = توکی->Command();
	}

	ړنګول راوباسه_پورته_Menu;

    } وإلا {
	//  default processing
	BTabView::Mouse_ښکته(ټکی);
	أرجع;
    }

    إذا (event < 0)
	أرجع;

    Vim_Tabline_Menu_پیغام tmm;
    tmm.شاخص = شاخص;
    tmm.event = event;
    write_پورټ(gui.vdcmp, Vim_پیغام::TablineMenu, &tmm, حجم(tmm));
}

فراغ
Vim_Tab_کرښه::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_کرښه *tab_کرښه = gui.vimForm->Tab_کرښه();
    إذا (tab_کرښه != عدم) {

	int32 i = 0;
	لكل (; i < tab_کرښه->شمېر_Tabs(); i++)
	    إذا (هذا == tab_کرښه->TabAt(i))
		اكسر;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	إذا (i < tab_کرښه->شمېر_Tabs()) {
	    Vim_Tabline_پیغام tm;
	    tm.شاخص = i + 1;
	    write_پورټ(gui.vdcmp, Vim_پیغام::Tabline, &tm, حجم(tm));
	}
    }
}

#د_که_پای //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
ساكن حرف appsig[] = "application/x-vnd.Haiku-Vim-8";
کلی_map *کلی_Map;
حرف *کلی_Map_Chars;
صحيح main_exitcode = 127;

    حالت_t
gui_haiku_process_event(bigtime_t timeout)
{
    بنية Vim_پیغام vm;
    int32 what;
    ssize_t اندازه;

    اندازه = read_پورټ_etc(gui.vdcmp, &what, &vm, حجم(vm),
	    B_TIMEOUT, timeout);

    إذا (اندازه >= 0) {
	بدل (what) {
	    حالة Vim_پیغام::کلی_2:
		{
		    char_u *string = vm.u.کلی_2.chars;
		    صحيح len = vm.u.کلی_2.اوږدتیا;
		    إذا (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_ننوت_buf();
			got_int = TRUE;
		    }

		    إذا (vm.u.کلی_2.csi_escape)
#که_نه_تعریف_شوی FEAT_MBYTE_IME
		    {
			صحيح	i;
			char_u	buf[2];

			لكل (i = 0; i < len; ++i)
			{
			    جمع_کړه_منزل_ننوت_buf(string + i, 1);
			    إذا (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (صحيح)KE_CSI;
				جمع_کړه_منزل_ننوت_buf(buf, 2);
			    }
			}
		    }
#وإلا
			جمع_کړه_منزل_ننوت_buf_csi(string, len);
#د_که_پای
		    وإلا
			جمع_کړه_منزل_ننوت_buf(string, len);
		}
		اكسر;
	    حالة Vim_پیغام::Resize:
		gui_resize_shell(vm.u.New_اندازه.پلنوالی, vm.u.New_اندازه.لوړوالی);
		اكسر;
	    حالة Vim_پیغام::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_شمېر =
			atomic_جمع_کړه(&vm.u.Scroll.sb->scroll_Event_شمېر, -1);
		    إذا (old_شمېر <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.ارزښت, vm.u.Scroll.stillDragging);
		}
		اكسر;
#إذا defined(FEAT_MENU)
	    حالة Vim_پیغام::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		اكسر;
#د_که_پای
	    حالة Vim_پیغام::Mouse:
		{
		    int32 old_شمېر;
		    إذا (vm.u.Mouse.button == MOUSE_DRAG)
			old_شمېر =
			    atomic_جمع_کړه(&gui.vim_متن_Area->mouse_Drag_Event_شمېر, -1);
		    وإلا
			old_شمېر = 0;
		    إذا (old_شمېر <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		اكسر;
	    حالة Vim_پیغام::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		اكسر;
	    حالة Vim_پیغام::Focus:
		gui.دننه_focus = vm.u.Focus.فعال;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		إذا (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		اكسر;
	    حالة Vim_پیغام::Refs:
		::RefsReceived(vm.u.Refs.پیغام_2, vm.u.Refs.changedir);
		اكسر;
	    حالة Vim_پیغام::Tabline:
		send_tabline_event(vm.u.Tabline.شاخص);
		اكسر;
	    حالة Vim_پیغام::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.شاخص, vm.u.TablineMenu.event);
		اكسر;
	    افتراضي:
		//  unrecognised message, ignore it
		اكسر;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    أرجع اندازه;
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

    صحيح
vim_قلف_screen()
{
    أرجع !gui.vim_کړکۍ || gui.vim_کړکۍ->قلف_2();
}

    فراغ
vim_unlock_screen()
{
    إذا (gui.vim_کړکۍ)
	gui.vim_کړکۍ->Unlock();
}

#تعریف_کړه پرلپسې_BAPPLICATION_دننه_NEW_THREAD	0

#إذا پرلپسې_BAPPLICATION_دننه_NEW_THREAD

    int32
پرلپسې_vimapp(فراغ *دلیلونه)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.پرلپسې_2();		    // Run until Quit() called

    أرجع 0;
}

#وإلا

    int32
call_main(فراغ *دلیلونه)
{
    بنية Main_دلیلونه *ma = (Main_دلیلونه *)دلیلونه;

    أرجع اصلي(ma->argc, ma->argv);
}
#د_که_پای

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    فراغ
gui_mch_prepare(
	صحيح	*argc,
	حرف	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    إذا (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(پیدا_کړه_thread(عدم), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_پورټ(B_MAX_پورټ_شمېر, "vim VDCMP");

#إذا پرلپسې_BAPPLICATION_دننه_NEW_THREAD
	thread_id tid = spawn_thread(پرلپسې_vimapp, "vim VimApp",
		tinfo.لومړیتوب, عدم);
	إذا (tid >= B_OK) {
	    resume_thread(tid);
	} وإلا {
	    getout(1);
	}
#وإلا
	Main_دلیلونه ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.لومړیتوب, &ma);
	إذا (tid >= B_OK) {
	    VimApp app(appsig);

	    gui.vimApp = &app;
	    resume_thread(tid);
	    /*
	     * This is rather horrible.
	     * call_main will call main() again...
	     * There will be no infinite recursion since
	     * gui.vimApp is set now.
	     */
	    app.پرلپسې_2();		    // Run until Quit() called
	    // fprintf(stderr, "app.Run() returned...\n");
	    حالت_t dummy_exitcode;
	    (فراغ)wait_for_thread(tid, &dummy_exitcode);

	    /*
	     * This path should be the normal one taken to exit Vim.
	     * The main() thread calls mch_exit() which calls
	     * gui_mch_exit() which terminates its thread.
	     */
	    وتل(main_exitcode);
	}
#د_که_پای
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
    إذا (!isatty(0)) {
	بنية stat stat_stdin, stat_dev_null;

	إذا (fstat(0, &stat_stdin) == -1 ||
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
    صحيح
gui_mch_init_check(فراغ)
{
    أرجع OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    صحيح
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_شا_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.شا_pixel = gui.def_شا_pixel;

    gui.scrollbar_پلنوالی = (صحيح) B_V_SCROLL_BAR_پلنوالی;
    gui.scrollbar_لوړوالی = (صحيح) B_H_SCROLL_BAR_لوړوالی;
#که_تعریف_شوی FEAT_MENU
    gui.menu_لوړوالی = 19;   //	initial guess -
    //	correct for my default settings
#د_که_پای
    gui.border_دننه_واټن = 3;  //	coordinates are inside window borders

    إذا (gui.vdcmp < B_OK)
	أرجع FAIL;
    get_کلی_map(&کلی_Map, &کلی_Map_Chars);

    gui.vim_کړکۍ = جديد Vim_کړکۍ();	// hidden and locked
    إذا (!gui.vim_کړکۍ)
	أرجع FAIL;

    gui.vim_کړکۍ->پرلپسې_2();	// Run() unlocks but does not show

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    سېټ_نورمال_colors();

    /*
     * Check that none of the colors are the same as the background color
     */
    gui_check_colors();

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();	    // re-init colors and fonts

    gui_mch_new_colors();	// window must exist for this

    أرجع OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    فراغ
gui_mch_new_colors()
{
    rgb_color rgb = GUI_منزل_RGB(gui.شا_pixel);

    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vimForm->سېټ_View_Color(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vim_کړکۍ->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحيح
gui_mch_open()
{
    إذا (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_سېټ_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_کړکۍ->Show();
	gui.vim_کړکۍ->Unlock();
	أرجع OK;
    }

    أرجع FAIL;
}

    فراغ
gui_mch_exit(صحيح vim_exitcode)
{
    إذا (gui.vim_کړکۍ) {
	thread_id tid = gui.vim_کړکۍ->Thread();
	gui.vim_کړکۍ->قلف_2();
	gui.vim_کړکۍ->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_پورټ(gui.vdcmp);
#إذا !پرلپسې_BAPPLICATION_دننه_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#د_که_پای
    إذا (gui.vimApp) {
	Vim_متن_Area_View::guiBlankMouse(خطأ);

	main_exitcode = vim_exitcode;
#إذا پرلپسې_BAPPLICATION_دننه_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->قلف_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#وإلا
	gui.vimApp->قلف_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	// suicide
	exit_thread(vim_exitcode);
#د_که_پای
    }
    // If we are somehow still here, let mch_exit() handle things.
}

/*
 * Get the position of the top left corner of the window.
 */
    صحيح
gui_mch_get_winpos(صحيح *x, صحيح *y)
{
    إذا (gui.vim_کړکۍ->قلف_2()) {
	BRect r;
	r = gui.vim_کړکۍ->چوکاټ_2();
	gui.vim_کړکۍ->Unlock();
	*x = (صحيح)r.کیڼ;
	*y = (صحيح)r.سر_2;
	أرجع OK;
    }
    وإلا
	أرجع FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    فراغ
gui_mch_سېټ_winpos(صحيح x, صحيح y)
{
    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_کړکۍ->Move_منزل(x, y);
	gui.vim_کړکۍ->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
فراغ
gui_mch_سېټ_shellsize(
	صحيح	پلنوالی,
	صحيح	لوړوالی,
	صحيح	min_پلنوالی,
	صحيح	min_لوړوالی,
	صحيح	بنسټ_پلنوالی,
	صحيح	بنسټ_لوړوالی,
	صحيح	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_کړکۍ->Resize_منزل(پلنوالی - PEN_پلنوالی, لوړوالی - PEN_پلنوالی);

	// set size limits
	عائم min_پلنوالی_2, max_پلنوالی, min_لوړوالی_2, max_لوړوالی;

	gui.vim_کړکۍ->Get_اندازه_Limits(&min_پلنوالی_2, &max_پلنوالی,
		&min_لوړوالی_2, &max_لوړوالی);
	gui.vim_کړکۍ->سېټ_اندازه_Limits(min_پلنوالی, max_پلنوالی,
		min_لوړوالی, max_لوړوالی);

	/*
	 * Set the resizing alignment depending on font size.
	 */
	gui.vim_کړکۍ->سېټ_کړکۍ_Alignment(
		B_PIXEL_ALIGNMENT,	//  window_alignment mode,
		1,		//  int32 h,
		0,		//  int32 hOffset = 0,
		gui.char_پلنوالی,	    //	int32 width = 0,
		بنسټ_پلنوالی,	    //	int32 widthOffset = 0,
		1,		//  int32 v = 0,
		0,		//  int32 vOffset = 0,
		gui.char_لوړوالی,	//  int32 height = 0,
		بنسټ_لوړوالی	    //	int32 heightOffset = 0
		);

	gui.vim_کړکۍ->Unlock();
    }
}

فراغ
gui_mch_get_screen_dimensions(
	صحيح	*screen_w,
	صحيح	*screen_h)
{
    BRect چوکاټ;

    {
	BScreen screen(gui.vim_کړکۍ);

	إذا (screen.IsValid()) {
	    چوکاټ = screen.چوکاټ_2();
	} وإلا {
	    چوکاټ.ښی = 640;
	    چوکاټ.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (صحيح) چوکاټ.ښی - 2 * gui.scrollbar_پلنوالی - 20;
    *screen_h = (صحيح) چوکاټ.bottom - gui.scrollbar_لوړوالی
#که_تعریف_شوی FEAT_MENU
	- gui.menu_لوړوالی
#د_که_پای
	- 30;
}

فراغ
gui_mch_سېټ_متن_area_pos(
	صحيح	x,
	صحيح	y,
	صحيح	w,
	صحيح	h)
{
    إذا (!gui.vim_متن_Area)
	أرجع;

    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_متن_Area->Move_منزل(x, y);
	gui.vim_متن_Area->Resize_منزل(w - PEN_پلنوالی, h - PEN_پلنوالی);

#که_تعریف_شوی FEAT_GUI_TABLINE
	إذا (gui.vimForm->Tab_کرښه() != عدم) {
	    gui.vimForm->Tab_کرښه()->Resize_منزل(w, gui.vimForm->Tabline_لوړوالی());
	}
#د_که_پای // FEAT_GUI_TABLINE

	gui.vim_کړکۍ->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

فراغ
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	صحيح	flag)
{
    VimScrollBar *vsb = sb->id;
    إذا (gui.vim_کړکۍ->قلف_2()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	إذا (flag) {
	    إذا (vsb->IsHidden()) {
		vsb->Show();
	    }
	} وإلا {
	    إذا (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vim_کړکۍ->Unlock();
    }
}

فراغ
gui_mch_سېټ_scrollbar_thumb(
	scrollbar_T *sb,
	صحيح	val,
	صحيح	اندازه,
	صحيح	max)
{
    إذا (gui.vim_کړکۍ->قلف_2()) {
	VimScrollBar *s = sb->id;
	إذا (max == 0) {
	    s->سېټ_ارزښت(0);
	    s->سېټ_Range(0.0, 0.0);
	} وإلا {
	    s->سېټ_Proportion((عائم)اندازه / (max + 1.0));
	    s->سېټ_ګامونه(1.0, اندازه > 5 ? اندازه - 2 : اندازه);
#که_نه_تعریف_شوی SCROLL_PAST_پای_ټکی	    //	really only defined in gui.c...
	    max = max + 1 - اندازه;
#د_که_پای
	    إذا (max < s->ارزښت_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->سېټ_ارزښت(val);
		s->سېټ_Range(0.0, max);
	    } وإلا {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->سېټ_Range(0.0, max);
		s->سېټ_ارزښت(val);
	    }
	}
	gui.vim_کړکۍ->Unlock();
    }
}

فراغ
gui_mch_سېټ_scrollbar_pos(
	scrollbar_T *sb,
	صحيح	x,
	صحيح	y,
	صحيح	w,
	صحيح	h)
{
    إذا (gui.vim_کړکۍ->قلف_2()) {
	BRect winb = gui.vim_کړکۍ->Bounds();
	عائم vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->Resize_منزل(w - PEN_پلنوالی, h - PEN_پلنوالی);
	إذا (winb.ښی-(x+w)<w) vsbx = winb.ښی - (w - PEN_پلنوالی);
	vsb->Move_منزل(vsbx, vsby);
	gui.vim_کړکۍ->Unlock();
    }
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    أرجع 0;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    أرجع 0;
}

فراغ
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	صحيح	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = جديد VimScrollBar(sb, posture);
    إذا (gui.vim_کړکۍ->قلف_2()) {
	vsb->سېټ_موخه(gui.vim_متن_Area);
	vsb->Hide();
	gui.vimForm->جمع_کړه_ماشوم(vsb);
	gui.vim_کړکۍ->Unlock();
    }
}

#إذا defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
فراغ
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    إذا (gui.vim_کړکۍ->قلف_2()) {
	sb->id->RemoveSelf();
	ړنګول sb->id;
	gui.vim_کړکۍ->Unlock();
    }
}
#د_که_پای

/*
 * Cursor does not flash
 */
    صحيح
gui_mch_is_blink_off(فراغ)
{
    أرجع FALSE;
}

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON blinking, cursor is shown
 */

#تعریف_کړه BLINK_NONE  0
#تعریف_کړه BLINK_OFF   1
#تعریف_کړه BLINK_ON    2

ساكن صحيح	blink_state = BLINK_NONE;
ساكن long_u	    blink_waittime = 700;
ساكن long_u	    blink_ontime = 400;
ساكن long_u	    blink_offtime = 250;
ساكن صحيح  blink_timer = 0;

فراغ
gui_mch_سېټ_blinking(
	اوږد	waittime,
	اوږد	on,
	اوږد	off)
{
    // TODO
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    فراغ
gui_mch_stop_blink(صحيح may_call_gui_update_cursor)
{
    // TODO
    إذا (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    إذا (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    فراغ
gui_mch_پیل_blink()
{
    // TODO
    إذا (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.دننه_focus)
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
صحيح
gui_mch_init_font(
	char_u	    *font_name,
	صحيح	    fontset)
{
    إذا (gui.vim_کړکۍ->قلف_2())
    {
	صحيح rc = gui.vim_متن_Area->mchInitFont(font_name);
	gui.vim_کړکۍ->Unlock();

	أرجع rc;
    }

    أرجع FAIL;
}


    صحيح
gui_mch_adjust_charsize()
{
    أرجع FAIL;
}


    صحيح
gui_mch_font_dialog(font_family* family, font_style* style, عائم* اندازه)
{
#إذا defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = جديد VimSelectFontDialog(family, style, اندازه);
    منطقي ret = dialog->Go();
	ړنګول dialog;
	أرجع ret;
#وإلا
    أرجع NOFONT;
#د_که_پای // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	صحيح	    giveErrorIfMissing)
{
    ساكن VimFont *fontList = عدم;

    إذا (!gui.دننه_use)	//  can't do this when GUI not running
	أرجع NOFONT;

    //	storage for locally modified name;
    ثابت صحيح buff_اندازه = B_FONT_FAMILY_اوږدتیا + B_FONT_STYLE_اوږدتیا + 20;
    ساكن حرف font_name[buff_اندازه] = {0};
    font_family family = {0};
    font_style	style  = {0};
    عائم اندازه = 0.f;

    إذا (name == 0 && be_fixed_font == 0) {
	إذا (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	أرجع NOFONT;
    }

    منطقي useSelectGUI = خطأ;
    إذا (name != عدم)
	إذا (STRCMP(name, "*") == 0) {
	    useSelectGUI = ریښتیا;
	    STRNCPY(font_name, hl_get_font_name(), buff_اندازه);
	} وإلا
	    STRNCPY(font_name, name, buff_اندازه);

    إذا (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	اندازه = be_fixed_font->اندازه_2();
	vim_snprintf(font_name, buff_اندازه,
	    (حرف*)"%s/%s/%.0f", family, style, اندازه);
    }

    //	replace underscores with spaces
    حرف* پای_ټکی = 0;
    طالما (پای_ټکی = strchr((حرف *)font_name, '_'))
	*پای_ټکی = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    ساكن حرف buff[buff_اندازه] = {0};
    STRNCPY(buff, font_name, buff_اندازه);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_اوږدتیا);
    حرف* style_s = strtok(0, "/\0");
    إذا (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_اوږدتیا);
    اندازه = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    إذا (useSelectGUI) {
	إذا (gui_mch_font_dialog(&family, &style, &اندازه) == NOFONT)
	    أرجع FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_اندازه,
		(حرف*)"%s/%s/%.0f", family, style, اندازه);
	hl_سېټ_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	إذا (new_p_guifont != عدم) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    لكل ( ; *new_p_guifont; ++new_p_guifont)
		إذا (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    لكل (flp = fontList; flp; flp = flp->بل) {
	إذا (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    أرجع (GuiFont)flp;
	}
    }

    VimFont *font = جديد VimFont();
    font->name = vim_strsave((char_u*)font_name);

    إذا (شمېر_font_styles(family) <= 0) {
	إذا (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	ړنګول font;
	أرجع NOFONT;
    }

    //	Remember font in the static list for later use
    font->بل = fontList;
    fontList = font;

    font->سېټ_Family_And_Style(family, style);
    إذا (اندازه > 0.f)
	font->سېټ_اندازه(اندازه);

    font->سېټ_Spacing(B_FIXED_SPACING);
    font->سېټ_Encoding(B_UNICODE_UTF8);

    أرجع (GuiFont)font;
}

/*
 * Set the current text font.
 */
فراغ
gui_mch_سېټ_font(
	GuiFont	font)
{
    إذا (gui.vim_کړکۍ->قلف_2()) {
	VimFont *vf = (VimFont *)font;

	gui.vim_متن_Area->سېټ_Font(vf);

	gui.char_پلنوالی = (صحيح) vf->String_پلنوالی("n");
	font_لوړوالی fh;
	vf->Get_لوړوالی(&fh);
	gui.char_لوړوالی = (صحيح)(fh.ascent + 0.9999)
	    + (صحيح)(fh.descent + 0.9999) + (صحيح)(fh.leading + 0.9999);
	gui.char_ascent = (صحيح)(fh.ascent + 0.9999);

	gui.vim_کړکۍ->Unlock();
    }
}

// XXX TODO This is apparently never called...
فراغ
gui_mch_free_font(
	GuiFont	font)
{
    إذا (font == NOFONT)
	أرجع;
    VimFont *f = (VimFont *)font;
    إذا (--f->refcount <= 0) {
	إذا (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	ړنګول f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    إذا (name == عدم)
	أرجع عدم;
    أرجع vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحيح
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
	font_لوړوالی fh = {0};
	font->Get_لوړوالی(&fh);
	gui.char_لوړوالی = (صحيح)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (صحيح)(fh.ascent + 0.5) + p_linespace / 2;
    }
    أرجع OK;
}

    فراغ
gui_mch_getmouse(صحيح *x, صحيح *y)
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

    فراغ
gui_mch_mousehide(صحيح hide)
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
    أرجع gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
فراغ
gui_mch_سېټ_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_منزل_RGB(color);
    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_متن_Area->سېټ_لوړ_Color(rgb);
	gui.vim_کړکۍ->Unlock();
    }
}

/*
 * Set the current text background color.
 */
فراغ
gui_mch_سېټ_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_منزل_RGB(color);
    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_متن_Area->سېټ_ټیټ_Color(rgb);
	gui.vim_کړکۍ->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    فراغ
gui_mch_سېټ_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

فراغ
gui_mch_draw_string(
	صحيح	لیکه,
	صحيح	ستون,
	char_u	*s,
	صحيح	len,
	صحيح	flags)
{
    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_متن_Area->mchDrawString(لیکه, ستون, s, len, flags);
	gui.vim_کړکۍ->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(صحيح r, صحيح g, صحيح b)
{
    أرجع gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
صحيح
gui_mch_haskey(
	char_u	*name)
{
    صحيح i;

    لكل (i = 0; special_کلیګانې[i].Be_کلیګانې != 0; i++)
	إذا (name[0] == special_کلیګانې[i].vim_code0 &&
		name[1] == special_کلیګانې[i].vim_code1)
	    أرجع OK;
    أرجع FAIL;
}

    فراغ
gui_mch_beep()
{
    ::beep();
}

    فراغ
gui_mch_flash(صحيح msec)
{
    // Do a visual beep by reversing the foreground and background colors

    إذا (gui.vim_کړکۍ->قلف_2()) {
	BRect rect = gui.vim_متن_Area->Bounds();

	gui.vim_متن_Area->سېټ_Drawing_ډېرپېښ(B_OP_INVERT);
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->Sync();
	snooze(msec * 1000);	 // wait for a few msec
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->سېټ_Drawing_ډېرپېښ(B_OP_COPY);
	gui.vim_متن_Area->Flush();
	gui.vim_کړکۍ->Unlock();
    }
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
فراغ
gui_mch_invert_rectangle(
	صحيح	r,
	صحيح	c,
	صحيح	nr,
	صحيح	nc)
{
    BRect rect;
    rect.کیڼ = FILL_X(c);
    rect.سر_2 = FILL_Y(r);
    rect.ښی = rect.کیڼ + nc * gui.char_پلنوالی - PEN_پلنوالی;
    rect.bottom = rect.سر_2 + nr * gui.char_لوړوالی - PEN_پلنوالی;

    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_متن_Area->سېټ_Drawing_ډېرپېښ(B_OP_INVERT);
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->سېټ_Drawing_ډېرپېښ(B_OP_COPY);
	gui.vim_کړکۍ->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    فراغ
gui_mch_iconify()
{
    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_کړکۍ->Minimize(ریښتیا);
	gui.vim_کړکۍ->Unlock();
    }
}

#إذا defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    فراغ
gui_mch_سېټ_foreground(فراغ)
{
    // TODO
}
#د_که_پای

/*
 * Set the window title
 */
فراغ
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_کړکۍ->سېټ_Title((حرف *)title);
	gui.vim_کړکۍ->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    فراغ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_سېټ_fg_color(color);

    BRect r;
    r.کیڼ = FILL_X(gui.ستون);
    r.سر_2 = FILL_Y(gui.لیکه);
    صحيح cells = utf_off2cells(کرښه_دننه_واټن[gui.لیکه] + gui.ستون, 100); // TODO-TODO
    إذا (cells>=4) cells = 1;
    r.ښی = r.کیڼ + cells*gui.char_پلنوالی - PEN_پلنوالی;
    r.bottom = r.سر_2 + gui.char_لوړوالی - PEN_پلنوالی;

    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_متن_Area->StrokeRect(r);
	gui.vim_کړکۍ->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
فراغ
gui_mch_draw_part_cursor(
	صحيح	w,
	صحيح	h,
	guicolor_T  color)
{
    gui_mch_سېټ_fg_color(color);

    BRect r;
    r.کیڼ =
#که_تعریف_شوی FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_ښی ? FILL_X(gui.ستون + 1) - w :
#د_که_پای
	FILL_X(gui.ستون);
    r.ښی = r.کیڼ + w - PEN_پلنوالی;
    r.bottom = FILL_Y(gui.لیکه + 1) - PEN_پلنوالی;
    r.سر_2 = r.bottom - h + PEN_پلنوالی;

    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_متن_Area->FillRect(r);
	gui.vim_کړکۍ->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    فراغ
gui_mch_update()
{
    gui_mch_flush();
    طالما (پورټ_شمېر(gui.vdcmp) > 0 &&
	    !vim_is_ننوت_buf_ډک() &&
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
صحيح
gui_mch_wait_for_chars(
	اوږد	wtime)
{
    صحيح		focus;
    bigtime_t	until, timeout;
    حالت_t	st;

    إذا (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_وخت() + timeout;
    }
    وإلا
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.دننه_focus;
    لكل (;;)
    {
	// Stop or start blinking when focus changes
	إذا (gui.دننه_focus != focus)
	{
	    إذا (gui.دننه_focus)
		gui_mch_پیل_blink();
	    وإلا
		gui_mch_stop_blink(TRUE);
	    focus = gui.دننه_focus;
	}

	gui_mch_flush();

#که_تعریف_شوی پیغام_کتار
# ifdef FEAT_TIMERS
	did_جمع_کړه_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	إذا (did_جمع_کړه_timer)
	    // Need to recompute the waiting time.
	    اكسر;
# endif
# ifdef FEAT_دنده_CHANNEL
	إذا (has_any_channel())
	{
	    إذا (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	وإلا إذا (wtime < 0)
	    timeout = B_INFINITE_TIMEOUT;
# endif
#د_که_پای

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use gui_haiku_process_event() to hang until
	 * an event arrives.  No need to check for input_buf_full because we
	 * are returning as soon as it contains a single char.
	 */
	st = gui_haiku_process_event(timeout);

	إذا (ننوت_شته())
	    أرجع OK;
	إذا (st < B_OK)		// includes B_TIMED_OUT
	    أرجع FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	إذا (wtime >= 0)
	{
	    timeout = until - system_وخت();
	    إذا (timeout < 0)
		اكسر;
	}
    }
    أرجع FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    فراغ
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_کړکۍ->Flush();
	gui.vim_کړکۍ->Unlock();
    }
    أرجع;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
فراغ
gui_mch_clear_بلاک(
	صحيح	row1,
	صحيح	col1,
	صحيح	row2,
	صحيح	col2)
{
    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_متن_Area->mch_Clear_بلاک(row1, col1, row2, col2);
	gui.vim_کړکۍ->Unlock();
    }
}

    فراغ
gui_mch_clear_all()
{
    إذا (gui.vim_کړکۍ->قلف_2()) {
	gui.vim_متن_Area->mchClearAll();
	gui.vim_کړکۍ->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
فراغ
gui_mch_delete_کرښې(
	صحيح	لیکه,
	صحيح	عدد_کرښې)
{
    gui.vim_متن_Area->mch_Delete_کرښې(لیکه, عدد_کرښې);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
فراغ
gui_mch_ورننباسه_کرښې(
	صحيح	لیکه,
	صحيح	عدد_کرښې)
{
    gui.vim_متن_Area->mch_ورننباسه_کرښې(لیکه, عدد_کرښې);
}

#إذا defined(FEAT_MENU)
/*
 * Menu stuff.
 */

فراغ
gui_mch_enable_menu(
	صحيح	flag)
{
    إذا (gui.vim_کړکۍ->قلف_2())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->سېټ_Enabled(flag);
	gui.vim_کړکۍ->Unlock();
    }
}

فراغ
gui_mch_سېټ_menu_pos(
	صحيح	x,
	صحيح	y,
	صحيح	w,
	صحيح	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
فراغ
gui_mch_جمع_کړه_menu(
	vimmenu_T   *menu,
	صحيح	idx)
{
    vimmenu_T	*پلار = menu->پلار;

    //	popup menu - just create it unattached
    إذا (menu_is_popup(menu->name) && پلار == عدم) {
	BPop_پورته_Menu* راوباسه_پورته_Menu = جديد BPop_پورته_Menu((ثابت حرف*)menu->name, خطأ, خطأ);
	menu->submenu_id = راوباسه_پورته_Menu;
	menu->id = عدم;
	أرجع;
    }

    إذا (!menu_is_menubar(menu->name)
	    || (پلار != عدم && پلار->submenu_id == عدم))
	أرجع;

    إذا (gui.vim_کړکۍ->قلف_2())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	إذا ( پلار )
	    tmp = پلار->submenu_id;
	وإلا
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	إذا ( ! tmp->پیدا_کړه_توکی((ثابت حرف *) menu->dname)) {

	    BMenu *bmenu = جديد BMenu((حرف *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->جمع_کړه_توکی(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->پیدا_کړه_توکی((ثابت حرف *) menu->dname);

	}
	gui.vim_کړکۍ->Unlock();
    }
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
    // no tearoff menus
}

    ساكن BMessage *
Menu_پیغام(vimmenu_T *menu)
{
    BMessage *m = جديد BMessage('menu');
    m->جمع_کړه_اشاره_ګر("VimMenu", (فراغ *)menu);

    أرجع m;
}

/*
 * Add a menu item to a menu
 */
فراغ
gui_mch_جمع_کړه_menu_توکی(
	vimmenu_T   *menu,
	صحيح	idx)
{
    صحيح	    mnemonic = 0;
    vimmenu_T	*پلار = menu->پلار;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    إذا (gui.vim_کړکۍ->قلف_2())
    {
#که_تعریف_شوی FEAT_TOOLBAR
	إذا (menu_is_toolbar(پلار->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->جمع_کړه_Button(idx, menu);
	    }
	} وإلا
#د_که_پای

	إذا (پلار->submenu_id != عدم || menu_is_popup(پلار->name)) {
	    إذا (menu_is_separator(menu->name)) {
		BSeparator_توکی *توکی = جديد BSeparator_توکی();
		پلار->submenu_id->جمع_کړه_توکی(توکی);
		menu->id = توکی;
		menu->submenu_id = عدم;
	    }
	    وإلا {
		BMenu_توکی *توکی = جديد BMenu_توکی((حرف *)menu->dname,
			Menu_پیغام(menu));
		توکی->سېټ_موخه(gui.vim_متن_Area);
		توکی->سېټ_Trigger((حرف) menu->mnemonic);
		پلار->submenu_id->جمع_کړه_توکی(توکی);
		menu->id = توکی;
		menu->submenu_id = عدم;
	    }
	}
	gui.vim_کړکۍ->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
فراغ
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    إذا (gui.vim_کړکۍ->قلف_2())
    {
#که_تعریف_شوی FEAT_TOOLBAR
	إذا (menu->پلار && menu_is_toolbar(menu->پلار->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->RemoveButton(menu);
	    }
	} وإلا
#د_که_پای
	{
	    أكد(menu->submenu_id == عدم || menu->submenu_id->شمېر_توکي() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    إذا (bmenu)
	    {
		bmenu->Remove_توکی(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		إذا (bmenu == gui.vimForm->MenuBar() && bmenu->شمېر_توکي() == 0)
		{
		    bmenu->Resize_منزل(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    ړنګول menu->id;
	    menu->id = عدم;
	    menu->submenu_id = عدم;

	    gui.menu_لوړوالی = (صحيح) gui.vimForm->Menu_لوړوالی();
	}
	gui.vim_کړکۍ->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
فراغ
gui_mch_menu_grey(
	vimmenu_T   *menu,
	صحيح	grey)
{
#که_تعریف_شوی FEAT_TOOLBAR
    إذا (menu->پلار && menu_is_toolbar(menu->پلار->name)) {
	إذا (gui.vim_کړکۍ->قلف_2()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->خړ_Button(menu, grey);
	    }
	    gui.vim_کړکۍ->Unlock();
	}
    } وإلا
#د_که_پای
    إذا (menu->id != عدم)
	menu->id->سېټ_Enabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
فراغ
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	صحيح	hidden)
{
    إذا (menu->id != عدم)
	menu->id->سېټ_Enabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    فراغ
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    فراغ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    إذا (!menu_is_popup(menu->name) || menu->submenu_id == عدم)
	أرجع;

    BPop_پورته_Menu* popupMenu = dynamic_cast<BPop_پورته_Menu*>(menu->submenu_id);
    إذا (popupMenu == عدم)
	أرجع;

    BPoint ټکی;
    إذا (gui.vim_کړکۍ->قلف_2()) {
	uint32 buttons = 0;
	gui.vim_متن_Area->GetMouse(&ټکی, &buttons);
	gui.vim_متن_Area->Convert_منزل_Screen(&ټکی);
	gui.vim_کړکۍ->Unlock();
    }
    popupMenu->Go(ټکی, ریښتیا);
}

#د_که_پای // FEAT_MENU

// Mouse stuff

#که_تعریف_شوی FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
حرف textplain[] = "text/plain";
حرف vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    فراغ
clip_mch_request_selection(Clipboard_T *cbd)
{
    إذا (be_clipboard->قلف_2())
    {
	BMessage *m = be_clipboard->معلومات_2();
	// m->PrintToStream();

	char_u *string = عدم;
	ssize_t stringlen = -1;

	إذا (m->پیدا_کړه_معلومات(textplain, B_MIME_TYPE,
		    (ثابت فراغ **)&string, &stringlen) == B_OK
		|| m->پیدا_کړه_String("text", (ثابت حرف **)&string) == B_OK)
	{
	    إذا (stringlen == -1)
		stringlen = STRLEN(string);

	    صحيح type;
	    حرف *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    إذا (m->پیدا_کړه_معلومات(vimselectiontype, B_MIME_TYPE,
			(ثابت فراغ **)&seltype, &seltypelen) == B_OK)
	    {
		بدل (*seltype)
		{
		    افتراضي:
		    حالة 'L':	type = MLINE;	اكسر;
		    حالة 'C':	type = MCHAR;	اكسر;
#که_تعریف_شوی FEAT_VISUAL
		    حالة 'B':	type = MBLOCK;	اكسر;
#د_که_پای
		}
	    }
	    وإلا
	    {
		// Otherwise use heuristic as documented
		type = memchr(string, stringlen, '\n') ? MLINE : MCHAR;
	    }
	    clip_yank_selection(type, string, (اوږد)stringlen, cbd);
	}
	be_clipboard->Unlock();
    }
}
/*
 * Make vim the owner of the current selection.
 */
    فراغ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    صحيح
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    أرجع FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    فراغ
clip_mch_سېټ_selection(Clipboard_T *cbd)
{
    إذا (be_clipboard->قلف_2())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->معلومات_2();
	أكد(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = عدم;
	long_u	شمېر;
	صحيح type;

	type = clip_convert_selection(&str, &شمېر, cbd);

	إذا (type < 0)
	    أرجع;

	m->جمع_کړه_معلومات(textplain, B_MIME_TYPE, (فراغ *)str, شمېر);

	// Add type of selection
	حرف	vtype;
	بدل (type)
	{
	    افتراضي:
	    حالة MLINE:    vtype = 'L';    اكسر;
	    حالة MCHAR:    vtype = 'C';    اكسر;
#که_تعریف_شوی FEAT_VISUAL
	    حالة MBLOCK:   vtype = 'B';    اكسر;
#د_که_پای
	}
	m->جمع_کړه_معلومات(vimselectiontype, B_MIME_TYPE, (فراغ *)&vtype, 1);

	vim_free(str);

	be_clipboard->Commit();
	be_clipboard->Unlock();
    }
}

#د_که_پای	// FEAT_CLIPBOARD

#که_تعریف_شوی FEAT_BROWSE
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
	صحيح saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = جديد BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    عدم, عدم, 0, خطأ,
	    جديد BMessage((saving == TRUE) ? 'save' : 'open'), عدم, ریښتیا);

    gui.vimApp->f_Browsed_لار.Unset();

    gui.vimApp->fFilePanel->کړکۍ()->سېټ_Title((حرف*)title);
    gui.vimApp->fFilePanel->سېټ_Panel_Directory((ثابت حرف*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    طالما (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = عدم;
    حالت_t پایله = gui.vimApp->f_Browsed_لار.InitCheck();
    إذا (پایله == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->f_Browsed_لار.لار_2());
    } وإلا
	إذا (پایله != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    پایله, strerror(پایله));
	}

    ړنګول gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = عدم;

    أرجع fileName;
}
#د_که_پای // FEAT_BROWSE


#إذا defined(FEAT_GUI_DIALOG)

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

صحيح
gui_mch_dialog(
	صحيح	 type,
	char_u	*title,
	char_u	*پیغام_2,
	char_u	*buttons,
	صحيح	 dfltbutton,
	char_u	*textfield,
	صحيح ex_cmd)
{
    VimDialog *dialog = جديد VimDialog(type, (حرف*)title, (حرف*)پیغام_2,
	    (حرف*)buttons, dfltbutton, (حرف*)textfield, ex_cmd);
    منطقي ret = dialog->Go();
    ړنګول dialog;
	أرجع ret;
}

#د_که_پای // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_منزل_RGB(pixel);

    أرجع ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    فراغ
gui_mch_setmouse(صحيح x, صحيح y)
{
    TRACE();
    // TODO
}

#که_تعریف_شوی FEAT_MBYTE_IME
    فراغ
im_سېټ_position(صحيح لیکه, صحيح ستون)
{
    إذا (gui.vim_کړکۍ->قلف_2())
    {
	gui.vim_متن_Area->DrawIMString();
	gui.vim_کړکۍ->Unlock();
    }
    أرجع;
}
#د_که_پای

    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_لوړوالی = (toolbar && showit) ? toolbar->Toolbar_لوړوالی() : 0.;
}

    فراغ
gui_mch_سېټ_toolbar_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    إذا (toolbar != عدم) {
	إذا (gui.vim_کړکۍ->قلف_2()) {
	    toolbar->Move_منزل(x, y);
	    toolbar->Resize_منزل(w - 1, h - 1);
	    gui.vim_کړکۍ->Unlock();
	}
    }
}

#إذا defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    فراغ
gui_mch_show_tabline(صحيح showit)
{
    Vim_Tab_کرښه *tab_کرښه = gui.vimForm->Tab_کرښه();

    إذا (tab_کرښه == عدم)
	أرجع;

    إذا (!showit != !gui.vimForm->Is_Showing_Tab_کرښه()) {
	gui.vimForm->سېټ_Showing_Tab_کرښه(showit != 0);
	gui.tabline_لوړوالی = gui.vimForm->Tabline_لوړوالی();
    }
}

    فراغ
gui_mch_سېټ_tabline_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    Vim_Tab_کرښه *tab_کرښه = gui.vimForm->Tab_کرښه();
    إذا (tab_کرښه != عدم) {
	إذا (gui.vim_کړکۍ->قلف_2()) {
	    tab_کرښه->Move_منزل(x, y);
	    tab_کرښه->Resize_منزل(w - 1, h - 1);
	    gui.vim_کړکۍ->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحيح
gui_mch_showing_tabline()
{
    Vim_Tab_کرښه *tab_کرښه = gui.vimForm->Tab_کرښه();
    أرجع tab_کرښه != عدم && gui.vimForm->Is_Showing_Tab_کرښه();
}

/*
 * Update the labels of the tabline.
 */
    فراغ
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    صحيح	    nr = 0;
    صحيح	    curtabidx = 0;

    Vim_Tab_کرښه *tab_کرښه = gui.vimForm->Tab_کرښه();

    إذا (tab_کرښه == عدم)
	أرجع;

    gui.vim_کړکۍ->قلف_2();

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = لومړی_tabpage; tp != عدم; tp = tp->tp_بل, ++nr) {
	إذا (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_کرښه->TabAt(nr);

	إذا (tab == عدم) {
	    tab = جديد Vim_Tab_کرښه::VimTab();
	    tab_کرښه->جمع_کړه_Tab(عدم, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->سېټ_Label((ثابت حرف*)NameBuff);
	tab_کرښه->Invalidate();
    }

    // Remove any old labels.
    طالما (nr < tab_کرښه->شمېر_Tabs())
	tab_کرښه->RemoveTab(nr);

    إذا (tab_کرښه->Selection() != curtabidx)
	tab_کرښه->Select(curtabidx);

    gui.vim_کړکۍ->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_سېټ_curtab(صحيح nr)
{
    Vim_Tab_کرښه *tab_کرښه = gui.vimForm->Tab_کرښه();
    إذا (tab_کرښه == عدم)
	أرجع;

    gui.vim_کړکۍ->قلف_2();

    إذا (tab_کرښه->Selection() != nr -1)
	tab_کرښه->Select(nr -1);

    gui.vim_کړکۍ->Unlock();
}

#د_که_پای // FEAT_GUI_TABLINE
