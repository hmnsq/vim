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
صنف Vim_پنجره;
صنف VimToolbar;
صنف Vim_Tab_سطر;

خارجي کلید_map *کلید_Map;
خارجي حرف *کلید_Map_Chars;

خارجي صحيح اصلی(صحيح argc, حرف **argv);

#اگر_تعریف_نشده B_MAX_پورت_شمار
#تعریف_کن B_MAX_پورت_شمار    255
#پایان_اگر

// VimApp seems comparable to the X "vimShell"
صنف VimApp: عام BApplication
{
    تعريفنوع BApplication Inherited;
    عام:
    VimApp(ثابت حرف *appsig);
    ~VimApp();

    // callbacks:
#إذا 0
    مجازی فراغ Dispatch_پیام(BMessage *m, BHandler *h)
    {
	m->Print_مقصد_جریان();
	Inherited::Dispatch_پیام(m, h);
    }
#پایان_اگر
    مجازی فراغ Ready_مقصد_پیاپی();
    مجازی فراغ ArgvReceived(int32 argc, حرف **argv);
    مجازی فراغ RefsReceived(BMessage *m);
    مجازی منطقي QuitRequested();
    مجازی فراغ پیام_Received(BMessage *m);

    ساكن فراغ SendRefs(BMessage *m, منطقي changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	f_Browsed_مسیر;
    خاص:
};

صنف Vim_پنجره: عام BWindow
{
    تعريفنوع BWindow Inherited;
    عام:
    Vim_پنجره();
    ~Vim_پنجره();

    مجازی فراغ Dispatch_پیام(BMessage *m, BHandler *h);
    مجازی فراغ پنجره_Activated(منطقي فعال);
    مجازی منطقي QuitRequested();

    VimFormView	    *formView;

    خاص:
    فراغ init();

    منطقي is_fullscreen = خطأ;
    BRect saved_قاب;
    پنجره_look saved_look;
};

صنف VimFormView: عام BView
{
    تعريفنوع BView Inherited;
    عام:
    VimFormView(BRect قاب);
    ~VimFormView();

    // callbacks:
    مجازی فراغ AllAttached();
    مجازی فراغ قاب_Resized(عائم new_عرض, عائم new_ارتفاع);

#تعریف_کن MENUBAR_MARGIN	1
    عائم Menu_ارتفاع() ثابت
    { أرجع menuBar ? menuBar->قاب_2().ارتفاع_2() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() ثابت
    { أرجع menuBar; }

    خاص:
    فراغ init(BRect);

    BMenuBar	    *menuBar;
    Vim_متن_Area_View *متن_Area;

#اگر_تعریف_شده FEAT_TOOLBAR
    عام:
    عائم Toolbar_ارتفاع() ثابت;
    VimToolbar *ToolBar() ثابت
    { أرجع toolBar; }
    خاص:
    VimToolbar	    *toolBar;
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_TABLINE
    عام:
    Vim_Tab_سطر *Tab_سطر() ثابت	{ أرجع tab_سطر; }
    منطقي Is_Showing_Tab_سطر() ثابت { أرجع showing_Tab_سطر; }
    فراغ مجموعه_Showing_Tab_سطر(منطقي showing) { showing_Tab_سطر = showing;	}
    عائم Tabline_ارتفاع() ثابت;
    خاص:
    Vim_Tab_سطر	*tab_سطر;
    صحيح	showing_Tab_سطر;
#پایان_اگر
};

صنف Vim_متن_Area_View: عام BView
{
    تعريفنوع BView Inherited;
    عام:
    Vim_متن_Area_View(BRect قاب);
    ~Vim_متن_Area_View();

    // callbacks:
    مجازی فراغ Draw(BRect updateRect);
    مجازی فراغ کلید_پایین(ثابت حرف *بایتها, int32 رقم_بایتها_2);
    مجازی فراغ Mouse_پایین(BPoint نقطه);
    مجازی فراغ Mouse_به_بالا(BPoint نقطه);
    مجازی فراغ MouseMoved(BPoint نقطه, uint32 transit, ثابت BMessage *پیام_2);
    مجازی فراغ پیام_Received(BMessage *m);

    // own functions:
    صحيح mchInitFont(char_u *name);
    فراغ mchDrawString(صحيح ردیف, صحيح ستون, char_u *s, صحيح len, صحيح flags);
    فراغ mch_Clear_بلوک(صحيح row1, صحيح col1, صحيح row2, صحيح col2);
    فراغ mchClearAll();
    فراغ mch_Delete_سطرها(صحيح ردیف, صحيح رقم_سطرها);
    فراغ mch_درج_سطرها(صحيح ردیف, صحيح رقم_سطرها);

    ساكن فراغ guiSendMouseEvent(صحيح button, صحيح x, صحيح y, صحيح repeated_click, int_u modifiers);
    ساكن فراغ guiMouseMoved(صحيح x, صحيح y);
    ساكن فراغ guiBlankMouse(منطقي should_hide);
    ساكن int_u mouse_Modifiers_مقصد_Vim(int32 beModifiers);

    int32 mouse_Drag_Event_شمار;

#اگر_تعریف_شده FEAT_MBYTE_IME
    فراغ DrawIMString(فراغ);
#پایان_اگر

    خاص:
    فراغ init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#اگر_تعریف_شده FEAT_MBYTE_IME
    بنية {
	BMessenger* messenger;
	BMessage* پیام_2;
	BPoint location;
	صحيح ردیف;
	صحيح ستون;
	صحيح شمار;
    } IMData;
#پایان_اگر
};

صنف VimScrollBar: عام BScrollBar
{
    تعريفنوع BScrollBar Inherited;
    عام:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    مجازی فراغ مقدار_تغییریافته_2(عائم new_مقدار);
    مجازی فراغ Mouse_به_بالا(BPoint where);
    فراغ مجموعه_مقدار(عائم newval);
    scrollbar_T *getGsb()
    { أرجع gsb; }

    int32	scroll_Event_شمار;

    خاص:
    scrollbar_T *gsb;
    عائم   ignore_مقدار;
};


#اگر_تعریف_شده FEAT_TOOLBAR

صنف VimToolbar : عام BBox
{
    ساكن BBitmap *نرمال_Buttons_Bitmap;
    ساكن BBitmap *grayedButtonsBitmap;

    BBitmap *بارگیری_Vim_Bitmap(ثابت حرف* fileName);
    منطقي GetPictureFromBitmap(BPicture *picture_مقصد, int32 شاخص, BBitmap *bitmapFrom, منطقي pressed);
    منطقي Modify_Bitmap_مقصد_Grayed(BBitmap *bitmap);

    BList fButtonsList;
    فراغ InvalidateLayout();

    عام:
    VimToolbar(BRect قاب, ثابت حرف * name);
    ~VimToolbar();

    منطقي PrepareButtonBitmaps();

    منطقي جمع_کن_Button(int32 شاخص, vimmenu_T *menu);
    منطقي RemoveButton(vimmenu_T *menu);
    منطقي خاکستری_Button(vimmenu_T *menu, صحيح grey);

    عائم Toolbar_ارتفاع() ثابت;
    مجازی فراغ Attached_مقصد_پنجره();
};

BBitmap *VimToolbar::نرمال_Buttons_Bitmap  = عدم;
BBitmap *VimToolbar::grayedButtonsBitmap  = عدم;

ثابت عائم ToolbarMargin = 3.;
ثابت عائم ButtonMargin  = 3.;

#پایان_اگر //FEAT_TOOLBAR

#اگر_تعریف_شده FEAT_GUI_TABLINE

صنف Vim_Tab_سطر : عام BTabView
{
    عام:
	صنف VimTab : عام BTab {
	    عام:
		VimTab() : BTab(جديد BView(BRect(), "-Empty-", 0, 0)) {}

	    مجازی فراغ Select(BView* owner);
	};

	Vim_Tab_سطر(BRect r) : BTabView(r, "vimTabLine", B_عرض_FROM_LABEL,
	       B_FOLLOW_چپ | B_FOLLOW_بالاترین | B_FOLLOW_راست, B_WILL_DRAW | B_قاب_EVENTS) {}

    عائم Tabline_ارتفاع() ثابت;
    مجازی فراغ Mouse_پایین(BPoint نقطه);
};

#پایان_اگر //FEAT_GUI_TABLINE


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

    VimFont *بعدی;
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
	View(BRect قاب);
	~View();

	مجازی فراغ Draw(BRect updateRect);
	فراغ InitIcon(int32 type);

	خاص:
	BBitmap*    fIconBitmap;
    };

    VimDialog(صحيح type, ثابت حرف *title, ثابت حرف *پیام_2,
	    ثابت حرف *buttons, صحيح dfltbutton, ثابت حرف *textfield,
	    صحيح ex_cmd);
    ~VimDialog();

    صحيح Go();

    مجازی فراغ پیام_Received(BMessage *پیام);

    خاص:
    sem_id	    fDialogSem;
    صحيح		    f_Dialog_مقدار;
    BList	    fButtonsList;
    BTextView*	    f_پیام_View;
    BTextControl*   f_درون_داد_Control;
    ثابت حرف*	    f_درون_داد_مقدار;
};

صنف VimSelectFontDialog : عام BWindow
{
    تعريفنوع BWindow Inherited;

    فراغ _CleanList(BListView* list);
    فراغ _UpdateFontStyles();
    فراغ Update_اندازه_درون_داد_Preview();
    فراغ _UpdateFontPreview();
    منطقي Update_From_List_قلم(BListView* list, حرف* متن, صحيح متن_اندازه);
    عام:

    VimSelectFontDialog(font_family* family, font_style* style, عائم* اندازه);
    ~VimSelectFontDialog();

    منطقي Go();

    مجازی فراغ پیام_Received(BMessage *پیام);

    خاص:
    وضعیت_t	    f_وضعیت;
    sem_id	    fDialogSem;
    منطقي	    f_Dialog_مقدار;
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
    BTextControl*   f_Sizes_درون_داد;
};

#پایان_اگر // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

بنية Main_آرگومانها {
    صحيح	     argc;
    حرف    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#تعریف_کن	کلید_پیام_BUFSIZ	7
#إذا کلید_پیام_BUFSIZ < MAX_کلید_کد_LEN
#خطا Increase کلید_پیام_BUFSIZ!
#پایان_اگر

بنية Vim_کلید_پیام {
    char_u  درازی;
    char_u  chars[کلید_پیام_BUFSIZ];  // contains Vim encoding
    منطقي    csi_escape;
};

بنية Vim_Resize_پیام {
    صحيح	    عرض;
    صحيح	    ارتفاع;
};

بنية Vim_Scroll_Bar_پیام {
    VimScrollBar *sb;
    دراز    مقدار;
    صحيح	    stillDragging;
};

بنية Vim_Menu_پیام {
    vimmenu_T	*guiMenu;
};

بنية Vim_Mouse_پیام {
    صحيح	    button;
    صحيح	    x;
    صحيح	    y;
    صحيح	    repeated_click;
    int_u   modifiers;
};

بنية Vim_Mouse_Moved_پیام {
    صحيح	    x;
    صحيح	    y;
};

بنية Vim_Focus_پیام {
    منطقي    فعال;
};

بنية Vim_Refs_پیام {
    BMessage   *پیام_2;
    منطقي    changedir;
};

بنية Vim_Tabline_پیام {
    صحيح	    شاخص;
};

بنية Vim_Tabline_Menu_پیام {
    صحيح	    شاخص;
    صحيح	    event;
};

بنية Vim_پیام {
    تعداد Vim_پیام_Type {
	کلید_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    اتحاد {
	بنية Vim_کلید_پیام    کلید_2;
	بنية Vim_Resize_پیام New_اندازه;
	بنية Vim_Scroll_Bar_پیام	Scroll;
	بنية Vim_Menu_پیام   Menu;
	بنية Vim_Mouse_پیام  Mouse;
	بنية Vim_Mouse_Moved_پیام	MouseMoved;
	بنية Vim_Focus_پیام  Focus;
	بنية Vim_Refs_پیام   Refs;
	بنية Vim_Tabline_پیام	Tabline;
	بنية Vim_Tabline_Menu_پیام    TablineMenu;
    } u;
};

#تعریف_کن RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#تعریف_کن GUI_مقصد_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

ساكن بنية specialkey
{
    uint16  Be_کلیدها;
#تعریف_کن کلید_3(a,b)    ((a)<<8|(b))
#تعریف_کن K(a)	    کلید_3(0,a)		// for ASCII codes
#تعریف_کن F(b)	    کلید_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_کلیدها[] =
{
    {K(B_به_بالا_ARROW),	'k', 'u'},
    {K(B_پایین_ARROW),	    'k', 'd'},
    {K(B_چپ_ARROW),	    'k', 'l'},
    {K(B_راست_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_درج),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_انتها),		'@', '7'},
    {K(B_صفحه_به_بالا),	'k', 'P'},	// XK_Prior
    {K(B_صفحه_پایین),	    'k', 'N'},	    // XK_Next,

#تعریف_کن اولی_FUNCTION_کلید  11
    {F(B_F1_کلید),	'k', '1'},
    {F(B_F2_کلید),	'k', '2'},
    {F(B_F3_کلید),	'k', '3'},
    {F(B_F4_کلید),	'k', '4'},
    {F(B_F5_کلید),	'k', '5'},
    {F(B_F6_کلید),	'k', '6'},
    {F(B_F7_کلید),	'k', '7'},
    {F(B_F8_کلید),	'k', '8'},
    {F(B_F9_کلید),	'k', '9'},
    {F(B_F10_کلید),	'k', ';'},

    {F(B_F11_کلید),	'F', '1'},
    {F(B_F12_کلید),	'F', '2'},
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
    {F(B_PRINT_کلید),	    '%', '9'},

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
#پایان_اگر

    // End of list marker:
    {0,		    0, 0}
};

#تعریف_کن رقم_SPECIAL_کلیدها    ARRAY_درازی(special_کلیدها)

// ---------------- VimApp ----------------

    ساكن فراغ
docd(BPath &مسیر)
{
    mch_chdir((حرف *)مسیر.مسیر_2());
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
    int32 شمار;

    m->Print_مقصد_جریان();
    بدل (m->what) {
	حالة B_REFS_RECEIVED:
	حالة B_SIMPLE_داده:
	    m->GetInfo("refs", &type, &شمار);
	    إذا (type != B_REF_TYPE)
		اذهب bad;
	    اكسر;
	حالة B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &شمار);
	    إذا (type != B_STRING_TYPE)
		اذهب bad;
	    إذا (changedir) {
		حرف *dirname;
		إذا (m->پیدا_کن_String("cwd", (ثابت حرف **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    اكسر;
	افتراضي:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    حذف m;
	    أرجع;
    }

#اگر_تعریف_شده FEAT_VISUAL
    reset_VIsual();
#پایان_اگر

    char_u  **fnames;
    fnames = (char_u **) alloc(شمار * حجم(char_u *));
    صحيح fname_شاخص = 0;

    بدل (m->what) {
	حالة B_REFS_RECEIVED:
	حالة B_SIMPLE_داده:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    لكل (صحيح i = 0; i < شمار; ++i)
	    {
		مدخل_ref ref;
		إذا (m->پیدا_کن_Ref("refs", i, &ref) == B_OK) {
		    BEntry مدخل(&ref, خطأ);
		    BPath مسیر;
		    مدخل.Get_مسیر(&مسیر);

		    // Change to parent directory?
		    إذا (changedir) {
			BPath parentpath;
			مسیر.Get_والد(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    إذا (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			إذا (!changedir)
			    docd(مسیر);
		    } وإلا {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)مسیر.مسیر_2(), IObuff);
			إذا (fname == عدم)
			    fname = (char_u *)مسیر.مسیر_2();
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
	    لكل (صحيح i = 1; i < شمار; ++i)
	    {
		حرف *fname;

		إذا (m->پیدا_کن_String("argv", i, (ثابت حرف **) &fname) == B_OK) {
		    fnames[fname_شاخص++] = vim_strsave((char_u *)fname);
		}
	    }
	    اكسر;
	افتراضي:
	    // fprintf(stderr, "case default\n");
	    اكسر;
    }

    حذف m;

    // Handle the drop, :edit to get to the file
    إذا (fname_شاخص > 0) {
	handle_drop(fname_شاخص, fnames, FALSE, drop_callback, عدم);

	setcursor();
	بیرون_flush();
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
VimApp::Ready_مقصد_پیاپی()
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
	إذا (gui.vim_پنجره)
	    gui.vim_پنجره->Minimize(خطأ);
	BMessage *m = Current_پیام();
	Detach_Current_پیام();
	SendRefs(m, درست);
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
    طالما (--حد >= 0 && (curbuf == عدم || curbuf->b_شروع_ffc == 0))
	snooze(100000);    //  0.1 s
    إذا (gui.vim_پنجره)
	gui.vim_پنجره->Minimize(خطأ);
    Detach_Current_پیام();
    SendRefs(m, درست);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    فراغ
VimApp::SendRefs(BMessage *m, منطقي changedir)
{
    Vim_Refs_پیام rm;
    rm.پیام_2 = m;
    rm.changedir = changedir;

    write_پورت(gui.vdcmp, Vim_پیام::Refs, &rm, حجم(rm));
    //	calls ::RefsReceived
}

    فراغ
VimApp::پیام_Received(BMessage *m)
{
    بدل (m->what) {
	حالة 'save':
	    {
		مدخل_ref refDirectory;
		m->پیدا_کن_Ref("directory", &refDirectory);
		f_Browsed_مسیر.مجموعه_مقصد(&refDirectory);
		BString strName;
		m->پیدا_کن_String("name", &strName);
		f_Browsed_مسیر.Append(strName.String());
	    }
	    اكسر;
	حالة 'open':
	    {
		مدخل_ref ref;
		m->پیدا_کن_Ref("refs", &ref);
		f_Browsed_مسیر.مجموعه_مقصد(&ref);
	    }
	    اكسر;
	حالة B_CANCEL:
	    {
		BFilePanel *panel;
		m->پیدا_کن_اشاره_گر("source", (فراغ**)&panel);
		إذا (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    اكسر;
	افتراضي:
	    Inherited::پیام_Received(m);
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

Vim_پنجره::Vim_پنجره():
    BWindow(BRect(40, 40, 150, 150),
	    "Vim",
	    B_TITLED_پنجره,
	    0,
	    B_CURRENT_WORKSPACE)

{
    init();
}

Vim_پنجره::~Vim_پنجره()
{
    إذا (formView) {
	Remove_فرزند(formView);
	حذف formView;
    }
    gui.vim_پنجره = عدم;
}

    فراغ
Vim_پنجره::init()
{
    // Attach the VimFormView
    formView = جديد VimFormView(Bounds());
    إذا (formView != عدم) {
	جمع_کن_فرزند(formView);
    }
}

#إذا 0  //  disabled in zeta patch
    فراغ
Vim_پنجره::Dispatch_پیام(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    بدل (m->what) {
	حالة B_MOUSE_به_بالا:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		إذا (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->پیدا_کن_نقطه("where", &where);
		    v->Mouse_به_بالا(where);
		} وإلا {
		    Inherited::Dispatch_پیام(m, h);
		}
	    }
	    اكسر;
	افتراضي:
	    Inherited::Dispatch_پیام(m, h);
    }
}
#پایان_اگر

    فراغ
Vim_پنجره::پنجره_Activated(منطقي فعال)
{
    Inherited::پنجره_Activated(فعال);
    // the textArea gets the keyboard action
    إذا (فعال && gui.vim_متن_Area)
	gui.vim_متن_Area->MakeFocus(درست);

    بنية Vim_Focus_پیام fm;
    fm.فعال = فعال;

    write_پورت(gui.vdcmp, Vim_پیام::Focus, &fm, حجم(fm));
}

    منطقي
Vim_پنجره::QuitRequested()
{
    بنية Vim_کلید_پیام km;
    km.درازی = 5;
    کپی_حافظه((حرف *)km.chars, "\033:qa\r", km.درازی);
    km.csi_escape = خطأ;
    write_پورت(gui.vdcmp, Vim_پیام::کلید_2, &km, حجم(km));
    أرجع خطأ;
}
    فراغ
Vim_پنجره::Dispatch_پیام(BMessage *m, BHandler *h)
{
    منطقي should_propagate = درست;

    بدل (m->what)
    {
	حالة B_کلید_پایین:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->پیدا_کن_Int32("raw_char", &scancode);
		m->پیدا_کن_Int32("modifiers", &beModifiers);

		إذا (scancode == B_ENTER && (beModifiers & B_چپ_COMMAND_کلید))
		    {
			should_propagate = خطأ;
			إذا (هذا->is_fullscreen)
			    {
				هذا->is_fullscreen = خطأ;
				Resize_مقصد(هذا->saved_قاب.عرض_2(), هذا->saved_قاب.ارتفاع_2());
				Move_مقصد(هذا->saved_قاب.چپ, هذا->saved_قاب.بالاترین);
				مجموعه_Look(هذا->saved_look);
				مجموعه_Flags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } وإلا {
				هذا->saved_قاب = قاب_2();
				هذا->saved_look = Look();
				هذا->is_fullscreen = درست;
				BScreen s(هذا);
				مجموعه_Look(B_NO_BORDER_پنجره_LOOK);
				Resize_مقصد(s.قاب_2().عرض_2() + 1, s.قاب_2().ارتفاع_2() + 1);
				Move_مقصد(s.قاب_2().چپ, s.قاب_2().بالاترین);
				مجموعه_Flags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    إذا (should_propagate)
	Inherited::Dispatch_پیام(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect قاب):
    BView(قاب, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_قاب_EVENTS),
    menuBar(عدم),
#اگر_تعریف_شده FEAT_TOOLBAR
    toolBar(عدم),
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_سطر(عدم),
#پایان_اگر
    متن_Area(عدم)
{
    init(قاب);
}

VimFormView::~VimFormView()
{
    إذا (menuBar) {
	Remove_فرزند(menuBar);
#اگر_تعریف_شده never
	//  deleting the menuBar leads to SEGV on exit
	//  richard@whitequeen.com Jul 99
	حذف menuBar;
#پایان_اگر
    }

#اگر_تعریف_شده FEAT_TOOLBAR
    حذف toolBar;
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_TABLINE
    حذف tab_سطر;
#پایان_اگر

    إذا (متن_Area) {
	Remove_فرزند(متن_Area);
	حذف متن_Area;
    }
    gui.vimForm = عدم;
}

    فراغ
VimFormView::init(BRect قاب)
{
    menuBar = جديد BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    جمع_کن_فرزند(menuBar);

#اگر_تعریف_شده FEAT_TOOLBAR
    toolBar = جديد VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    جمع_کن_فرزند(toolBar);
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_TABLINE
    tab_سطر = جديد Vim_Tab_سطر(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    جمع_کن_فرزند(tab_سطر);
#پایان_اگر

    BRect مانده = قاب;
    متن_Area = جديد Vim_متن_Area_View(مانده);
    جمع_کن_فرزند(متن_Area);
    // The textArea will be resized later when menus are added

    gui.vimForm = هذا;
}

#اگر_تعریف_شده FEAT_TOOLBAR
    عائم
VimFormView::Toolbar_ارتفاع() ثابت
{
    أرجع toolBar ? toolBar->Toolbar_ارتفاع() : 0.;
}
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_TABLINE
    عائم
VimFormView::Tabline_ارتفاع() ثابت
{
    أرجع (tab_سطر && Is_Showing_Tab_سطر()) ? tab_سطر->Tabline_ارتفاع() : 0.;
}
#پایان_اگر

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
	BRect مانده = Bounds();

#اگر_تعریف_شده FEAT_MENU
	مانده.بالاترین += Menu_ارتفاع();
	menuBar->Resize_مقصد(مانده.راست, مانده.بالاترین);
	gui.menu_ارتفاع = (صحيح) Menu_ارتفاع();
#پایان_اگر

#اگر_تعریف_شده FEAT_TOOLBAR
	toolBar->Move_مقصد(مانده.چپ, مانده.بالاترین);
	toolBar->Resize_مقصد(مانده.راست, Toolbar_ارتفاع());
	مانده.بالاترین += Toolbar_ارتفاع();
	gui.toolbar_ارتفاع = Toolbar_ارتفاع();
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_TABLINE
	tab_سطر->Move_مقصد(مانده.چپ, مانده.بالاترین);
	tab_سطر->Resize_مقصد(مانده.راست + 1, Tabline_ارتفاع());
	مانده.بالاترین += Tabline_ارتفاع();
	gui.tabline_ارتفاع = Tabline_ارتفاع();
#پایان_اگر

	متن_Area->Resize_مقصد(مانده.عرض_2(), مانده.ارتفاع_2());
	متن_Area->Move_مقصد(مانده.چپ, مانده.بالاترین);
    }


    Inherited::AllAttached();
}

    فراغ
VimFormView::قاب_Resized(عائم new_عرض, عائم new_ارتفاع)
{
    بنية Vim_Resize_پیام sm;
    صحيح adjust_h, adjust_w;

    new_عرض += 1;	//  adjust from width to number of pixels occupied
    new_ارتفاع += 1;

    sm.عرض = (صحيح) new_عرض;
    sm.ارتفاع = (صحيح) new_ارتفاع;
    adjust_w = ((صحيح)new_عرض - gui_get_پایه_عرض()) % gui.char_عرض;
    adjust_h = ((صحيح)new_ارتفاع - gui_get_پایه_ارتفاع()) % gui.char_ارتفاع;

    إذا (adjust_w > 0 || adjust_h > 0) {
	sm.عرض  -= adjust_w;
	sm.ارتفاع -= adjust_h;
    }

    write_پورت(gui.vdcmp, Vim_پیام::Resize, &sm, حجم(sm));
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

Vim_متن_Area_View::Vim_متن_Area_View(BRect قاب):
    BView(قاب, "VimTextAreaView", B_FOLLOW_ALL_SIDES,
#اگر_تعریف_شده FEAT_MBYTE_IME
	B_WILL_DRAW | B_پر_UPDATE_ON_RESIZE | B_درون_داد_روش_AWARE
#وإلا
	B_WILL_DRAW | B_پر_UPDATE_ON_RESIZE
#پایان_اگر
	),
    mouse_Drag_Event_شمار(0)
{
#اگر_تعریف_شده FEAT_MBYTE_IME
    IMData.messenger = عدم;
    IMData.پیام_2 = عدم;
#پایان_اگر
    init(قاب);
}

Vim_متن_Area_View::~Vim_متن_Area_View()
{
    gui.vim_متن_Area = عدم;
}

    فراغ
Vim_متن_Area_View::init(BRect قاب)
{
    // set up global var for fast access
    gui.vim_متن_Area = هذا;

    /*
     * Tell the app server not to erase the view: we will
     * fill it in completely by ourselves.
     * (Does this really work? Even if not, it won't harm either.)
     */
    مجموعه_View_Color(B_TRANSPARENT_32_بیت);
#تعریف_کن PEN_عرض   1
    مجموعه_Pen_اندازه(PEN_عرض);
#تعریف_کن W_عرض(curwin)   0
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
    rgb_color rgb = GUI_مقصد_RGB(gui.پشت_pixel);
    مجموعه_کم_Color(rgb);
    FillRect(updateRect, B_SOLID_کم);
    gui_redraw((صحيح) updateRect.چپ, (صحيح) updateRect.بالاترین,
	    (صحيح) (updateRect.عرض_2() + PEN_عرض), (صحيح) (updateRect.ارتفاع_2() + PEN_عرض));

    // Clear the border areas if needed
    مجموعه_کم_Color(rgb);

    إذا (updateRect.چپ < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.چپ, updateRect.بالاترین,
		    FILL_X(0)-PEN_عرض, updateRect.bottom), B_SOLID_کم);
    إذا (updateRect.بالاترین < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.چپ, updateRect.بالاترین,
		    updateRect.راست, FILL_Y(0)-PEN_عرض), B_SOLID_کم);
    إذا (updateRect.راست >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((صحيح)Columns), updateRect.بالاترین,
		    updateRect.راست, updateRect.bottom), B_SOLID_کم);
    إذا (updateRect.bottom >= FILL_Y(ردیفها))   //  bottom border
	FillRect(BRect(updateRect.چپ, FILL_Y((صحيح)ردیفها),
		    updateRect.راست, updateRect.bottom), B_SOLID_کم);

#اگر_تعریف_شده FEAT_MBYTE_IME
    DrawIMString();
#پایان_اگر
}

    فراغ
Vim_متن_Area_View::کلید_پایین(ثابت حرف *بایتها, int32 رقم_بایتها_2)
{
    بنية Vim_کلید_پیام km;
    char_u *dest = km.chars;

    منطقي canHaveVimModifiers = خطأ;

    BMessage *پیام = پنجره()->Current_پیام();
    أكد(پیام);
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
    إذا (رقم_بایتها_2 > 1) {
	// This cannot be a special key
	إذا (رقم_بایتها_2 > کلید_پیام_BUFSIZ)
	    رقم_بایتها_2 = کلید_پیام_BUFSIZ;	    //	should never happen... ???
	km.درازی = رقم_بایتها_2;
	کپی_حافظه((حرف *)dest, بایتها, رقم_بایتها_2);
	km.csi_escape = درست;
    } وإلا {
	int32 scancode = 0;
	پیام->پیدا_کن_Int32("key", &scancode);

	int32 beModifiers = 0;
	پیام->پیدا_کن_Int32("modifiers", &beModifiers);

	char_u string[3];
	صحيح len = 0;
	km.درازی = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	أكد(B_BACKSPACE <= 0x20);
	أكد(B_DELETE == 0x7F);
	إذا (((char_u)بایتها[0] <= 0x20 || (char_u)بایتها[0] == 0x7F) &&
		رقم_بایتها_2 == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    إذا (beModifiers & B_CONTROL_کلید) {
		صحيح شاخص = کلید_Map->نرمال_map[scancode];
		صحيح new_رقم_بایتها = کلید_Map_Chars[شاخص];
		char_u *new_بایتها = (char_u *)&کلید_Map_Chars[شاخص + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		إذا (new_رقم_بایتها != 1 || (*new_بایتها > 0x20 &&
			    *new_بایتها != 0x7F )) {
		    اذهب notspecial;
		}
		بایتها = (حرف *)new_بایتها;
	    }
	    canHaveVimModifiers = درست;

	    uint16 beoskey;
	    صحيح اولی, آخرین;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    إذا (رقم_بایتها_2 == 0 || بایتها[0] == B_FUNCTION_کلید) {
		beoskey = F(scancode);
		اولی = اولی_FUNCTION_کلید;
		آخرین = رقم_SPECIAL_کلیدها;
	    } وإلا إذا (*بایتها == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		اولی = آخرین = 0;
	    } وإلا {
		beoskey = K(بایتها[0]);
		اولی = 0;
		آخرین = اولی_FUNCTION_کلید;
	    }

	    لكل (صحيح i = اولی; i < آخرین; i++) {
		إذا (special_کلیدها[i].Be_کلیدها == beoskey) {
		    string[0] = CSI;
		    string[1] = special_کلیدها[i].vim_code0;
		    string[2] = special_کلیدها[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	إذا (len == 0) {
	    string[0] = بایتها[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#إذا 0
	إذا (len == 3 ||
		بایتها[0] == B_SPACE || بایتها[0] == B_TAB ||
		بایتها[0] == B_RETURN || بایتها[0] == '\r' ||
		بایتها[0] == B_ESCAPE)
#وإلا
	    إذا (canHaveVimModifiers)
#پایان_اگر
	    {
		صحيح modifiers;
		modifiers = 0;
		إذا (beModifiers & B_لغزش_کلید)
		    modifiers |= MOD_ماسک_لغزش;
		إذا (beModifiers & B_CONTROL_کلید)
		    modifiers |= MOD_ماسک_CTRL;
		إذا (beModifiers & B_OPTION_کلید)
		    modifiers |= MOD_ماسک_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		صحيح کلید;
		إذا (string[0] == CSI && len == 3)
		    کلید = مقصد_SPECIAL(string[1], string[2]);
		وإلا
		    کلید = string[0];
		کلید = simplify_کلید(کلید, &modifiers);
		إذا (IS_SPECIAL(کلید))
		{
		    string[0] = CSI;
		    string[1] = K_دومی(کلید);
		    string[2] = K_سوم(کلید);
		    len = 3;
		}
		وإلا
		{
		    string[0] = کلید;
		    len = 1;
		}

		إذا (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.درازی = 3;
		}
	    }
	کپی_حافظه((حرف *)dest, string, len);
	km.درازی += len;
	km.csi_escape = خطأ;
    }

    write_پورت(gui.vdcmp, Vim_پیام::کلید_2, &km, حجم(km));

    /*
     * blank out the pointer if necessary
     */
    إذا (p_mh && !gui.اشاره_گر_hidden)
    {
	guiBlankMouse(درست);
	gui.اشاره_گر_hidden = TRUE;
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
    Vim_Mouse_پیام mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_پورت(gui.vdcmp, Vim_پیام::Mouse, &mm, حجم(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    إذا (gui.اشاره_گر_hidden)
    {
	guiBlankMouse(خطأ);
	gui.اشاره_گر_hidden = FALSE;
    }
}

فراغ
Vim_متن_Area_View::guiMouseMoved(
	صحيح	x,
	صحيح	y)
{
    Vim_Mouse_Moved_پیام mm;

    mm.x = x;
    mm.y = y;

    write_پورت(gui.vdcmp, Vim_پیام::MouseMoved, &mm, حجم(mm));

    إذا (gui.اشاره_گر_hidden)
    {
	guiBlankMouse(خطأ);
	gui.اشاره_گر_hidden = FALSE;
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
Vim_متن_Area_View::mouse_Modifiers_مقصد_Vim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    إذا (beModifiers & B_لغزش_کلید)
	vim_modifiers |= MOUSE_لغزش;
    إذا (beModifiers & B_CONTROL_کلید)
	vim_modifiers |= MOUSE_CTRL;
    إذا (beModifiers & B_OPTION_کلید)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    أرجع vim_modifiers;
}

    فراغ
Vim_متن_Area_View::Mouse_پایین(BPoint نقطه)
{
    BMessage *m = پنجره()->Current_پیام();
    أكد(m);

    int32 buttons = 0;
    m->پیدا_کن_Int32("buttons", &buttons);

    صحيح vimButton;

    إذا (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_چپ;
    وإلا إذا (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_راست;
    وإلا إذا (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_میانه;
    وإلا
	أرجع;		// Unknown button

    vimMouseButton = 1;	    // don't care which one

    // Handle multiple clicks
    int32 clicks = 0;
    m->پیدا_کن_Int32("clicks", &clicks);

    int32 modifiers = 0;
    m->پیدا_کن_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_مقصد_Vim(modifiers);

    guiSendMouseEvent(vimButton, نقطه.x, نقطه.y,
	    clicks > 1 /* = repeated_click*/, vimMouseModifiers);
}

    فراغ
Vim_متن_Area_View::Mouse_به_بالا(BPoint نقطه)
{
    vimMouseButton = 0;

    BMessage *m = پنجره()->Current_پیام();
    أكد(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->پیدا_کن_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_مقصد_Vim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, نقطه.x, نقطه.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::Mouse_به_بالا(نقطه);
}

    فراغ
Vim_متن_Area_View::MouseMoved(BPoint نقطه, uint32 transit, ثابت BMessage *پیام_2)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    إذا (gui.اشاره_گر_hidden)
    {
	guiBlankMouse(خطأ);
	gui.اشاره_گر_hidden = FALSE;
    }

    إذا (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(نقطه.x, نقطه.y);
	أرجع;
    }

    atomic_جمع_کن(&mouse_Drag_Event_شمار, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, نقطه.x, نقطه.y, 0, vimMouseModifiers);
}

    فراغ
Vim_متن_Area_View::پیام_Received(BMessage *m)
{
    بدل (m->what) {
	حالة 'menu':
	    {
		Vim_Menu_پیام mm;
		mm.guiMenu = عدم;  // in case no pointer in msg
		m->پیدا_کن_اشاره_گر("VimMenu", (فراغ **)&mm.guiMenu);
		write_پورت(gui.vdcmp, Vim_پیام::Menu, &mm, حجم(mm));
	    }
	    اكسر;
	حالة B_MOUSE_WHEEL_تغییریافته:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		عائم small=0, big=0, dy=0;
		m->پیدا_کن_Float("be:wheel_delta_y", &dy);
		scb->Get_گامها(&small, &big);
		scb->مجموعه_مقدار(scb->مقدار_2()+small*dy*3);
		scb->مقدار_تغییریافته_2(scb->مقدار_2());
#إذا 0
		scb = curwin->w_scrollbars[0].id;
		scb->Get_گامها(&small, &big);
		scb->مجموعه_مقدار(scb->مقدار_2()+small*dy);
		scb->مقدار_تغییریافته_2(scb->مقدار_2());
#پایان_اگر
	    }
	    اكسر;
#اگر_تعریف_شده FEAT_MBYTE_IME
	حالة B_درون_داد_روش_EVENT:
	    {
		int32 opcode;
		m->پیدا_کن_Int32("be:opcode", &opcode);
		بدل(opcode)
		{
		    حالة B_درون_داد_روش_STARTED:
			إذا (!IMData.messenger) حذف IMData.messenger;
			IMData.messenger = جديد BMessenger();
			m->پیدا_کن_Messenger("be:reply_to", IMData.messenger);
			اكسر;
		    حالة B_درون_داد_روش_تغییریافته:
			{
			    BString str;
			    منطقي confirmed;
			    إذا (IMData.پیام_2) *(IMData.پیام_2) = *m;
			    وإلا	       IMData.پیام_2 = جديد BMessage(*m);
			    DrawIMString();
			    m->پیدا_کن_Bool("be:confirmed", &confirmed);
			    إذا (confirmed)
			    {
				m->پیدا_کن_String("be:string", &str);
				char_u *chars = (char_u*)str.String();
				بنية Vim_کلید_پیام km;
				km.csi_escape = درست;
				صحيح clen;
				صحيح i = 0;
				طالما (i < str.درازی_2())
				{
				    clen = utf_ptr2len(chars+i);
				    کپی_حافظه(km.chars, chars+i, clen);
				    km.درازی = clen;
				    write_پورت(gui.vdcmp, Vim_پیام::کلید_2, &km, حجم(km));
				    i += clen;
				}
			    }
			}
			اكسر;
		    حالة B_درون_داد_روش_LOCATION_REQUEST:
			{
			    BMessage پیام(B_درون_داد_روش_EVENT);
			    پیام.جمع_کن_Int32("be:opcode", B_درون_داد_روش_LOCATION_REQUEST);
			    پیام.جمع_کن_نقطه("be:location_reply", IMData.location);
			    پیام.جمع_کن_Float("be:height_reply", FILL_Y(1));
			    IMData.messenger->Send_پیام(&پیام);
			}
			اكسر;
		    حالة B_درون_داد_روش_STOPPED:
			حذف IMData.messenger;
			حذف IMData.پیام_2;
			IMData.messenger = عدم;
			IMData.پیام_2 = عدم;
			اكسر;
		}
	    }
	    // TODO: sz: break here???
#پایان_اگر
	افتراضي:
	    إذا (m->WasDropped()) {
		BWindow *w = پنجره();
		w->Detach_Current_پیام();
		w->Minimize(خطأ);
		VimApp::SendRefs(m, (modifiers() & B_لغزش_کلید) != 0);
	    } وإلا {
		Inherited::پیام_Received(m);
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
	gui_mch_مجموعه_font((GuiFont)newFont);
	إذا (name && STRCMP(name, "*") != 0)
	    hl_مجموعه_font_name(name);

	مجموعه_Drawing_نما(B_OP_COPY);

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
Vim_متن_Area_View::mchDrawString(صحيح ردیف, صحيح ستون, char_u *s, صحيح len, صحيح flags)
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

	BRect r(FILL_X(ستون), FILL_Y(ردیف),
		FILL_X(ستون + cells) - PEN_عرض, FILL_Y(ردیف + 1) - PEN_عرض);
	FillRect(r, B_SOLID_کم);
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
	    where.مجموعه(متن_X(ستون+lastpos), متن_Y(ردیف));
	    DrawString(p, clen, where);
	    إذا (flags & DRAW_BOLD) {
		where.x += 1.0;
		مجموعه_Drawing_نما(B_OP_BLEND);
		DrawString(p, clen, where);
		مجموعه_Drawing_نما(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    إذا (cells<4) lastpos += cells;
	    وإلا	lastpos++;
	    p += clen;
	}
    }
    وإلا
    {
	BPoint where(متن_X(ستون), متن_Y(ردیف));
	DrawString((حرف*)s, len, where);
	إذا (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    مجموعه_Drawing_نما(B_OP_BLEND);
	    DrawString((حرف*)s, len, where);
	    مجموعه_Drawing_نما(B_OP_COPY);
	}
    }

    إذا (flags & DRAW_UNDERL) {
	صحيح cells;
	cells = 0;
	لكل (صحيح i=0; i<len; i++) {
	    صحيح cn = utf_ptr2cells((char_u *)(s+i));
	    إذا (cn<4) cells += cn;
	}

	BPoint شروع(FILL_X(ستون), FILL_Y(ردیف + 1) - PEN_عرض);
	BPoint انتها(FILL_X(ستون + cells) - PEN_عرض, شروع.y);

	Stroke_سطر(شروع, انتها);
    }
}

فراغ
Vim_متن_Area_View::mch_Clear_بلوک(
	صحيح	row1,
	صحيح	col1,
	صحيح	row2,
	صحيح	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_عرض, FILL_Y(row2 + 1) - PEN_عرض);
    gui_mch_مجموعه_bg_color(gui.پشت_pixel);
    FillRect(r, B_SOLID_کم);
}

    فراغ
Vim_متن_Area_View::mchClearAll()
{
    gui_mch_مجموعه_bg_color(gui.پشت_pixel);
    FillRect(Bounds(), B_SOLID_کم);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    فراغ
Vim_متن_Area_View::mch_Delete_سطرها(صحيح ردیف, صحيح رقم_سطرها)
{
    BRect منبع, dest;
    منبع.چپ = FILL_X(gui.scroll_region_چپ);
    منبع.بالاترین = FILL_Y(ردیف + رقم_سطرها);
    منبع.راست = FILL_X(gui.scroll_region_راست + 1) - PEN_عرض;
    منبع.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_عرض;

    dest.چپ = FILL_X(gui.scroll_region_چپ);
    dest.بالاترین = FILL_Y(ردیف);
    dest.راست = FILL_X(gui.scroll_region_راست + 1) - PEN_عرض;
    dest.bottom = FILL_Y(gui.scroll_region_bot - رقم_سطرها + 1) - PEN_عرض;

    إذا (gui.vim_پنجره->قفل_2()) {
	// Clear one column more for when bold has spilled over
	Copy_بیتها(منبع, dest);
	gui_clear_بلوک(gui.scroll_region_bot - رقم_سطرها + 1,
		gui.scroll_region_چپ,
		gui.scroll_region_bot, gui.scroll_region_راست);


	gui.vim_پنجره->Unlock();
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
Vim_متن_Area_View::mch_درج_سطرها(صحيح ردیف, صحيح رقم_سطرها)
{
    BRect منبع, dest;

    // XXX Attempt at a hack:
    gui.vim_پنجره->UpdateIfNeeded();
    منبع.چپ = FILL_X(gui.scroll_region_چپ);
    منبع.بالاترین = FILL_Y(ردیف);
    منبع.راست = FILL_X(gui.scroll_region_راست + 1) - PEN_عرض;
    منبع.bottom = FILL_Y(gui.scroll_region_bot - رقم_سطرها + 1) - PEN_عرض;

    dest.چپ = FILL_X(gui.scroll_region_چپ);
    dest.بالاترین = FILL_Y(ردیف + رقم_سطرها);
    dest.راست = FILL_X(gui.scroll_region_راست + 1) - PEN_عرض;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_عرض;

    إذا (gui.vim_پنجره->قفل_2()) {
	// Clear one column more for when bold has spilled over
	Copy_بیتها(منبع, dest);
	gui_clear_بلوک(ردیف, gui.scroll_region_چپ,
		ردیف + رقم_سطرها - 1, gui.scroll_region_راست);

	gui.vim_پنجره->Unlock();
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

#اگر_تعریف_شده FEAT_MBYTE_IME
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
    BMessage* پیام = IMData.پیام_2;
    إذا (!پیام)
	أرجع;
    gui_redraw_بلوک(IMData.ردیف, 0,
	    IMData.ردیف + IMData.شمار, W_عرض(curwin), GUI_MON_NOCLEAR);
    منطقي confirmed = خطأ;
    پیام->پیدا_کن_Bool("be:confirmed", &confirmed);
    إذا (confirmed)
	أرجع;
    rgb_color hcolor = بالا_Color(), lcolor = کم_Color();
    پیام->پیدا_کن_String("be:string", &str);
    s = str.String();
    len = str.درازی_2();
    مجموعه_بالا_Color(0, 0, 0);
    IMData.ردیف = gui.ردیف;
    IMData.ستون = gui.ستون;
    int32 sel_شروع = 0, sel_انتها = 0;
    پیام->پیدا_کن_Int32("be:selection", 0, &sel_شروع);
    پیام->پیدا_کن_Int32("be:selection", 1, &sel_انتها);
    صحيح clen, cn;
    BPoint pos(IMData.ستون, 0);
    BRect r;
    BPoint where;
    IMData.location = Convert_مقصد_Screen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.ردیف + pos.y)));
    لكل (صحيح i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	إذا (pos.x + cn > W_عرض(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	إذا (sel_شروع<=i && i<sel_انتها)
	{
	    مجموعه_کم_Color(r_highlight);
	    IMData.location = Convert_مقصد_Screen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.ردیف + pos.y)));
	}
	وإلا
	{
	    مجموعه_کم_Color(b_highlight);
	}
	r.مجموعه(FILL_X(pos.x), FILL_Y(IMData.ردیف + pos.y),
		FILL_X(pos.x + cn) - PEN_عرض,
		FILL_Y(IMData.ردیف + pos.y + 1) - PEN_عرض);
	FillRect(r, B_SOLID_کم);
	where.مجموعه(متن_X(pos.x), متن_Y(IMData.ردیف + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.شمار = (صحيح)pos.y;

    مجموعه_بالا_Color(hcolor);
    مجموعه_کم_Color(lcolor);
}
#پایان_اگر
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
    ignore_مقدار(-1),
    scroll_Event_شمار(0)
{
    gsb = g;
    مجموعه_Resizing_نما(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    فراغ
VimScrollBar::مقدار_تغییریافته_2(عائم new_مقدار)
{
    إذا (ignore_مقدار >= 0.0 && new_مقدار == ignore_مقدار) {
	ignore_مقدار = -1;
	أرجع;
    }
    ignore_مقدار = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_جمع_کن(&scroll_Event_شمار, 1);

    بنية Vim_Scroll_Bar_پیام sm;

    sm.sb = هذا;
    sm.مقدار = (دراز) new_مقدار;
    sm.stillDragging = TRUE;

    write_پورت(gui.vdcmp, Vim_پیام::ScrollBar, &sm, حجم(sm));

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
VimScrollBar::Mouse_به_بالا(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_جمع_کن(&scroll_Event_شمار, 1);

    بنية Vim_Scroll_Bar_پیام sm;

    sm.sb = هذا;
    sm.مقدار = (دراز) مقدار_2();
    sm.stillDragging = FALSE;

    write_پورت(gui.vdcmp, Vim_پیام::ScrollBar, &sm, حجم(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::Mouse_به_بالا(where);
}

    فراغ
VimScrollBar::مجموعه_مقدار(عائم new_مقدار)
{
    إذا (new_مقدار == مقدار_2())
	أرجع;

    ignore_مقدار = new_مقدار;
    Inherited::مجموعه_مقدار(new_مقدار);
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
    بعدی = عدم;
    refcount = 1;
    name = عدم;
}

// ---------------- VimDialog ----------------

#إذا defined(FEAT_GUI_DIALOG)

ثابت بیعلامت صحيح  k_Vim_Dialog_Button_پیام = 'VMDB';
ثابت بیعلامت صحيح  k_Vim_Dialog_Icon_Stripe_عرض = 30;
ثابت بیعلامت صحيح  kVimDialogButtonsSpacingX = 9;
ثابت بیعلامت صحيح  kVimDialogButtonsSpacingY = 4;
ثابت بیعلامت صحيح  kVimDialogSpacingX = 6;
ثابت بیعلامت صحيح  kVimDialogSpacingY = 10;
ثابت بیعلامت صحيح  k_Vim_Dialog_کمینه_عرض  = 310;
ثابت بیعلامت صحيح  k_Vim_Dialog_کمینه_ارتفاع = 75;
ثابت BRect	    kDefaultRect =
BRect(0, 0, k_Vim_Dialog_کمینه_عرض, k_Vim_Dialog_کمینه_ارتفاع);

VimDialog::VimDialog(صحيح type, ثابت حرف *title, ثابت حرف *پیام_2,
	ثابت حرف *buttons, صحيح dfltbutton, ثابت حرف *textfield, صحيح ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_پنجره_LOOK, B_MODAL_APP_پنجره_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_مقدار(dfltbutton)
    , f_پیام_View(عدم)
    , f_درون_داد_Control(عدم)
    , f_درون_داد_مقدار(textfield)
{
    //	master view
    VimDialog::View* view = جديد VimDialog::View(Bounds());
    إذا (view == عدم)
	أرجع;

    إذا (title == عدم)
	مجموعه_Title("Vim " VIM_نسخه_MEDIUM);

    جمع_کن_فرزند(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    عائم max_Button_عرض  = 0;
    عائم max_Button_ارتفاع = 0;
    عائم buttons_عرض	  = 0;
    عائم buttons_ارتفاع   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    افعل
    {
	int32 انتها = strButtons.پیدا_کن_اولی('\n');
	إذا (انتها != B_ERROR)
	    strButtons.مجموعه_Byte_At(انتها, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->جمع_کن_فرزند(button);
	fButtonsList.جمع_کن_قلم(button);

	max_Button_عرض	= max_c(max_Button_عرض,  button->Bounds().عرض_2());
	max_Button_ارتفاع = max_c(max_Button_ارتفاع, button->Bounds().ارتفاع_2());
	buttons_عرض   += button->Bounds().عرض_2();
	buttons_ارتفاع  += button->Bounds().ارتفاع_2();

	إذا (انتها == B_ERROR)
	    اكسر;

	strButtons.Remove(0, انتها + 1);
    } طالما (درست);

    int32 buttons_شمار = fButtonsList.شمار_اقلام();
    buttons_عرض      += kVimDialogButtonsSpacingX * (buttons_شمار - 1);
    buttons_ارتفاع     += kVimDialogButtonsSpacingY * (buttons_شمار - 1);
    عائم dialog_عرض  = buttons_عرض + k_Vim_Dialog_Icon_Stripe_عرض +
	kVimDialogSpacingX * 2;
    عائم dialog_ارتفاع = max_Button_ارتفاع + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    منطقي vertical = (vim_strchr(p_go, GO_VERTICAL) != عدم) ||
	dialog_عرض >= gui.vim_پنجره->Bounds().عرض_2();
    إذا (vertical) {
	dialog_عرض  -= buttons_عرض;
	dialog_عرض  += max_Button_عرض;
	dialog_ارتفاع -= max_Button_ارتفاع;
	dialog_ارتفاع += buttons_ارتفاع;
    }

    dialog_عرض  = max_c(dialog_عرض,  k_Vim_Dialog_کمینه_عرض);

    //	message view
    BRect rect(0, 0, dialog_عرض, 0);
    rect.چپ  += k_Vim_Dialog_Icon_Stripe_عرض + 16 + kVimDialogSpacingX;
    rect.بالاترین   += kVimDialogSpacingY;
    rect.راست -= kVimDialogSpacingX;
    rect.bottom = rect.بالاترین;
    f_پیام_View = جديد BTextView(rect, "_tv_", rect.جابه_جایی_By_Copy(B_ORIGIN),
	    B_FOLLOW_چپ | B_FOLLOW_بالاترین, B_WILL_DRAW);

    f_پیام_View->مجموعه_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color متن_Color = ui_color(B_PANEL_متن_COLOR);
    f_پیام_View->مجموعه_Font_And_Color(be_plain_font, B_FONT_ALL, &متن_Color);
    f_پیام_View->مجموعه_متن(پیام_2);
    f_پیام_View->MakeEditable(خطأ);
    f_پیام_View->MakeSelectable(خطأ);
    f_پیام_View->مجموعه_کلمه_Wrap(درست);
    جمع_کن_فرزند(f_پیام_View);

    عائم پیام_ارتفاع = f_پیام_View->متن_ارتفاع(0, f_پیام_View->شمار_سطرها());
    f_پیام_View->ResizeBy(0, پیام_ارتفاع);
    f_پیام_View->مجموعه_متن_Rect(BRect(0, 0, rect.عرض_2(), پیام_ارتفاع));

    dialog_ارتفاع += پیام_ارتفاع;

    //	input view
    إذا (f_درون_داد_مقدار != عدم) {
	rect.بالاترین     =
	    rect.bottom += پیام_ارتفاع + kVimDialogSpacingY;
	f_درون_داد_Control = جديد BTextControl(rect, "_iv_", عدم, f_درون_داد_مقدار, عدم,
		B_FOLLOW_چپ | B_FOLLOW_بالاترین, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_درون_داد_Control->متن_View()->مجموعه_متن(f_درون_داد_مقدار);
	f_درون_داد_Control->متن_View()->مجموعه_کلمه_Wrap(خطأ);
	جمع_کن_فرزند(f_درون_داد_Control);

	عائم عرض = 0.f, ارتفاع = 0.f;
	f_درون_داد_Control->Get_Preferred_اندازه(&عرض, &ارتفاع);
	f_درون_داد_Control->MakeFocus(درست);

	dialog_ارتفاع += ارتفاع + kVimDialogSpacingY * 1.5;
    }

    dialog_ارتفاع = max_c(dialog_ارتفاع, k_Vim_Dialog_کمینه_ارتفاع);

    Resize_مقصد(dialog_عرض, dialog_ارتفاع);
    Move_مقصد((gui.vim_پنجره->Bounds().عرض_2() - dialog_عرض) / 2,
	    (gui.vim_پنجره->Bounds().ارتفاع_2() - dialog_ارتفاع) / 2);

    //	adjust layout of buttons
    عائم button_عرض = max_c(max_Button_عرض, rect.عرض_2() * 0.66);
    BPoint origin(dialog_عرض, dialog_ارتفاع);
    origin.x -= kVimDialogSpacingX + (vertical ? button_عرض : buttons_عرض);
    origin.y -= kVimDialogSpacingY + (vertical ? buttons_ارتفاع	: max_Button_ارتفاع);

    لكل (int32 i = 0 ; i < buttons_شمار; i++) {
	BButton *button = (BButton*)fButtonsList.قلم_At(i);
	button->Move_مقصد(origin);
	إذا (vertical) {
	    origin.y += button->قاب_2().ارتفاع_2() + kVimDialogButtonsSpacingY;
	    button->Resize_مقصد(button_عرض, button->قاب_2().ارتفاع_2());
	} وإلا
	    origin.x += button->قاب_2().عرض_2() + kVimDialogButtonsSpacingX;

	إذا (dfltbutton == i + 1) {
	    button->MakeDefault(درست);
	    button->MakeFocus(f_درون_داد_Control == عدم);
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
	أرجع f_Dialog_مقدار;
    }

    Show();

    طالما (acquire_sem(fDialogSem) == B_INTERRUPTED);

    صحيح ret_مقدار = f_Dialog_مقدار;
    إذا (f_درون_داد_مقدار != عدم)
	vim_strncpy((char_u*)f_درون_داد_مقدار, (char_u*)f_درون_داد_Control->متن_2(), IOSIZE - 1);

    إذا (قفل_2())
	Quit();

    أرجع ret_مقدار;
}

فراغ VimDialog::پیام_Received(BMessage *پیام)
{
    int32 which = 0;
    إذا (پیام->what != k_Vim_Dialog_Button_پیام ||
	    پیام->پیدا_کن_Int32("which", &which) != B_OK)
	أرجع BWindow::پیام_Received(پیام);

    f_Dialog_مقدار = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, ثابت حرف* label)
{
    BMessage *پیام_2 = جديد BMessage(k_Vim_Dialog_Button_پیام);
    پیام_2->جمع_کن_Int32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = جديد BButton(rect, name.String(), label, پیام_2,
	    B_FOLLOW_راست | B_FOLLOW_BOTTOM);

    عائم عرض = 0.f, ارتفاع = 0.f;
    button->Get_Preferred_اندازه(&عرض, &ارتفاع);
    button->Resize_مقصد(عرض, ارتفاع);

    أرجع button;
}

VimDialog::View::View(BRect قاب)
    :	BView(قاب, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(عدم)
{
    مجموعه_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    حذف fIconBitmap;
}

فراغ VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.راست = k_Vim_Dialog_Icon_Stripe_عرض;
    مجموعه_بالا_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    إذا (fIconBitmap == عدم)
	أرجع;

    مجموعه_Drawing_نما(B_OP_ALPHA);
    مجموعه_Blending_نما(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

فراغ VimDialog::View::InitIcon(int32 type)
{
    إذا (type == VIM_GENERIC)
	أرجع;

    BPath مسیر;
    وضعیت_t وضعیت = پیدا_کن_directory(B_BEOS_SERVERS_DIRECTORY, &مسیر);
    إذا (وضعیت != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(وضعیت));
	أرجع;
    }

    مسیر.Append("app_server");

    BFile file(مسیر.مسیر_2(), O_RDONLY);
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
    ثابت uint8* icon_داده = عدم;
    //	try vector icon first?
    icon_داده = (ثابت uint8*)resources.بارگیری_Resource(B_وکتور_ICON_TYPE, name, &اندازه);
    إذا (icon_داده != عدم && BIconUtils::Get_وکتور_Icon(icon_داده, اندازه, fIconBitmap) == B_OK)
	أرجع;

    //	try bitmap icon now
    icon_داده = (ثابت uint8*)resources.بارگیری_Resource(B_LARGE_ICON_TYPE, name, &اندازه);
    إذا (icon_داده == عدم) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	حذف fIconBitmap;
	fIconBitmap = عدم;
	أرجع;
    }

    إذا (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(icon_داده, icon_اندازه_2, icon_اندازه_2, icon_اندازه_2, fIconBitmap);
}

ثابت بیعلامت صحيح  k_Vim_Dialog_OKButton_پیام = 'FDOK';
ثابت بیعلامت صحيح  k_Vim_Dialog_Cancel_Button_پیام = 'FDCN';
ثابت بیعلامت صحيح  k_Vim_Dialog_اندازه_درون_داد_پیام = 'SICH';
ثابت بیعلامت صحيح  k_Vim_Dialog_Family_Select_پیام = 'MSFM';
ثابت بیعلامت صحيح  k_Vim_Dialog_Style_Select_پیام = 'MSST';
ثابت بیعلامت صحيح  k_Vim_Dialog_اندازه_Select_پیام = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, عائم* اندازه)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_پنجره_LOOK, B_MODAL_APP_پنجره_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , f_وضعیت(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_مقدار(خطأ)
    , fFamily(family)
    , fStyle(style)
    , f_اندازه(اندازه)
    , f_Font_اندازه(*اندازه)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_درون_داد(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_درازی);
    strncpy(fFontStyle, *style, B_FONT_STYLE_درازی);

    //	"client" area view
    BBox *clientBox = جديد BBox(Bounds(), B_خالی_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_قاب_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    جمع_کن_فرزند(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.چپ_بالاترین(), RC.چپ_بالاترین());

    //	at first create all controls
    fPreview = جديد BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->جمع_کن_فرزند(fPreview);

    BBox* boxDivider = جديد BBox(rc, B_خالی_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->جمع_کن_فرزند(boxDivider);

    BStringView *labelFamily = جديد BStringView(rc, "labelFamily", "Family:");
    clientBox->جمع_کن_فرزند(labelFamily);
    labelFamily->Resize_مقصد_Preferred();

    BStringView *labelStyle = جديد BStringView(rc, "labelStyle", "Style:");
    clientBox->جمع_کن_فرزند(labelStyle);
    labelStyle->Resize_مقصد_Preferred();

    BStringView *label_اندازه = جديد BStringView(rc, "labelSize", "Size:");
    clientBox->جمع_کن_فرزند(label_اندازه);
    label_اندازه->Resize_مقصد_Preferred();

    fFamiliesList = جديد BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = جديد BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_چپ_راست, 0, خطأ, درست);
    clientBox->جمع_کن_فرزند(scrollFamilies);

    fStylesList= جديد BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = جديد BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_چپ_راست, 0, خطأ, درست);
    clientBox->جمع_کن_فرزند(scrollStyles);

    f_Sizes_درون_داد = جديد BTextControl(rc, "inputSize", عدم, "???",
	    جديد BMessage(k_Vim_Dialog_اندازه_درون_داد_پیام));
    clientBox->جمع_کن_فرزند(f_Sizes_درون_داد);
    f_Sizes_درون_داد->Resize_مقصد_Preferred();

    fSizesList = جديد BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = جديد BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_چپ_راست, 0, خطأ, درست);
    clientBox->جمع_کن_فرزند(scrollSizes);

    BButton *buttonOK = جديد BButton(rc, "buttonOK", "OK",
			جديد BMessage(k_Vim_Dialog_OKButton_پیام));
    clientBox->جمع_کن_فرزند(buttonOK);
    buttonOK->Resize_مقصد_Preferred();

    BButton *buttonCancel = جديد BButton(rc, "buttonCancel", "Cancel",
			جديد BMessage(k_Vim_Dialog_Cancel_Button_پیام));
    clientBox->جمع_کن_فرزند(buttonCancel);
    buttonCancel->Resize_مقصد_Preferred();

    //	layout controls
    عائم سطر_ارتفاع = labelFamily->Bounds().ارتفاع_2();
    عائم preview_ارتفاع = سطر_ارتفاع * 3;
    عائم جابه_جایی_YLabels = preview_ارتفاع + kVimDialogSpacingY;
    عائم جابه_جایی_YLists = جابه_جایی_YLabels + سطر_ارتفاع + kVimDialogSpacingY / 2;
    عائم جابه_جایی_YSizes = جابه_جایی_YLists + f_Sizes_درون_داد->Bounds().ارتفاع_2() + kVimDialogSpacingY / 2;
    عائم lists_ارتفاع = سطر_ارتفاع * 9;
    عائم جابه_جایی_YButtons = جابه_جایی_YLists + lists_ارتفاع +  kVimDialogSpacingY;
    عائم max_Controls_ارتفاع = جابه_جایی_YButtons + buttonOK->Bounds().ارتفاع_2();
    عائم families_عرض = labelFamily->Bounds().عرض_2() * 5;
    عائم جابه_جایی_XStyles = families_عرض + kVimDialogSpacingX;
    عائم styles_عرض = labelStyle->Bounds().عرض_2() * 4;
    عائم جابه_جایی_XSizes = جابه_جایی_XStyles + styles_عرض + kVimDialogSpacingX;
    عائم sizes_عرض = label_اندازه->Bounds().عرض_2() * 2;
    عائم max_Controls_عرض = جابه_جایی_XSizes + sizes_عرض;

    Resize_مقصد(max_Controls_عرض + kVimDialogSpacingX * 2,
	max_Controls_ارتفاع + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vim_پنجره->قاب_2();
    Move_مقصد(rcVim.چپ + (rcVim.عرض_2() - قاب_2().عرض_2()) / 2,
	    rcVim.بالاترین + (rcVim.ارتفاع_2() - قاب_2().ارتفاع_2()) / 2);

    fPreview->Resize_مقصد(max_Controls_عرض, preview_ارتفاع);
    fPreview->مجموعه_Alignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, preview_ارتفاع + kVimDialogSpacingY / 2);
    boxDivider->Resize_مقصد(max_Controls_عرض, 1.f);

    labelFamily->MoveBy(0.f, جابه_جایی_YLabels);
    labelStyle->MoveBy(جابه_جایی_XStyles, جابه_جایی_YLabels);
    label_اندازه->MoveBy(جابه_جایی_XSizes, جابه_جایی_YLabels);

    //	text control alignment issues
    عائم insetX = f_Sizes_درون_داد->متن_View()->Bounds().عرض_2() - f_Sizes_درون_داد->Bounds().عرض_2();
    عائم insetY = f_Sizes_درون_داد->متن_View()->Bounds().عرض_2() - f_Sizes_درون_داد->Bounds().عرض_2();

    scrollFamilies->MoveBy(0.f, جابه_جایی_YLists);
    scrollStyles->MoveBy(جابه_جایی_XStyles, جابه_جایی_YLists);
    f_Sizes_درون_داد->MoveBy(جابه_جایی_XSizes + insetX / 2, جابه_جایی_YLists + insetY / 2);
    scrollSizes->MoveBy(جابه_جایی_XSizes, جابه_جایی_YSizes);

    f_Sizes_درون_داد->مجموعه_Alignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->Resize_مقصد(families_عرض, lists_ارتفاع);
    scrollStyles->Resize_مقصد(styles_عرض, lists_ارتفاع);
    f_Sizes_درون_داد->Resize_مقصد(sizes_عرض, f_Sizes_درون_داد->Bounds().ارتفاع_2());
    scrollSizes->Resize_مقصد(sizes_عرض,
	    lists_ارتفاع - (جابه_جایی_YSizes - جابه_جایی_YLists));

    buttonOK->MoveBy(max_Controls_عرض - buttonOK->Bounds().عرض_2(), جابه_جایی_YButtons);
    buttonCancel->MoveBy(max_Controls_عرض - buttonOK->Bounds().عرض_2()
	    - buttonCancel->Bounds().عرض_2() - kVimDialogSpacingX, جابه_جایی_YButtons);

    //	fill lists
    صحيح sel_شاخص = -1;
    صحيح شمار = شمار_font_families();
    لكل (صحيح i = 0; i < شمار; i++) {
	font_family family;
	إذا (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->جمع_کن_قلم(جديد BString_قلم((ثابت حرف*)family));
	    إذا (مقایسه_رشته(family, fFontFamily, B_FONT_FAMILY_درازی) == 0)
		sel_شاخص = i;
	}
    }

    إذا (sel_شاخص >= 0) {
	fFamiliesList->Select(sel_شاخص);
	fFamiliesList->Scroll_مقصد_Selection();
    }

    _UpdateFontStyles();

    sel_شاخص = -1;
    لكل (صحيح اندازه = 8, شاخص = 0; اندازه <= 18; اندازه++, شاخص++) {
	BString str;
	str << اندازه;
	fSizesList->جمع_کن_قلم(جديد BString_قلم(str));
	إذا (اندازه == f_Font_اندازه)
	    sel_شاخص = شاخص;

    }

    إذا (sel_شاخص >= 0) {
	fSizesList->Select(sel_شاخص);
	fSizesList->Scroll_مقصد_Selection();
    }

    fFamiliesList->مجموعه_Selection_پیام(جديد BMessage(k_Vim_Dialog_Family_Select_پیام));
    fStylesList->مجموعه_Selection_پیام(جديد BMessage(k_Vim_Dialog_Style_Select_پیام));
    fSizesList->مجموعه_Selection_پیام(جديد BMessage(k_Vim_Dialog_اندازه_Select_پیام));
    f_Sizes_درون_داد->مجموعه_Modification_پیام(جديد BMessage(k_Vim_Dialog_اندازه_درون_داد_پیام));

    Update_اندازه_درون_داد_Preview();
    _UpdateFontPreview();

    f_وضعیت = B_OK;
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
    طالما (0 < list->شمار_اقلام())
	حذف (dynamic_cast<BString_قلم*>(list->Remove_قلم((int32)0)));
}

    منطقي
VimSelectFontDialog::Go()
{
    إذا (f_وضعیت != B_OK) {
	Quit();
	أرجع NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    إذا (fDialogSem < B_OK) {
	Quit();
	أرجع f_Dialog_مقدار;
    }

    Show();

    طالما (acquire_sem(fDialogSem) == B_INTERRUPTED);

    منطقي ret_مقدار = f_Dialog_مقدار;

    إذا (قفل_2())
	Quit();

    أرجع ret_مقدار;
}


فراغ VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 sel_شاخص = -1;
    int32 شمار = شمار_font_styles(fFontFamily);
    لكل (int32 i = 0; i < شمار; i++) {
	font_style style;
	uint32 flags = 0;
	إذا (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->جمع_کن_قلم(جديد BString_قلم((ثابت حرف*)style));
	    إذا (مقایسه_رشته(style, fFontStyle, B_FONT_STYLE_درازی) == 0)
		sel_شاخص = i;
	}
    }

    إذا (sel_شاخص >= 0) {
	fStylesList->Select(sel_شاخص);
	fStylesList->Scroll_مقصد_Selection();
    } وإلا
	fStylesList->Select(0);
}


فراغ VimSelectFontDialog::Update_اندازه_درون_داد_Preview()
{
    حرف buf[10] = {0};
    vim_snprintf(buf, حجم(buf), (حرف*)"%.0f", f_Font_اندازه);
    f_Sizes_درون_داد->مجموعه_متن(buf);
}


فراغ VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.مجموعه_اندازه(f_Font_اندازه);
    font.مجموعه_Family_And_Style(fFontFamily, fFontStyle);
    fPreview->مجموعه_Font(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_اندازه);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (صحيح)f_Font_اندازه << " pt.";
    fPreview->مجموعه_متن(str);
}


    منطقي
VimSelectFontDialog::Update_From_List_قلم(BListView* list, حرف* متن, صحيح متن_اندازه)
{
    int32 شاخص = list->CurrentSelection();
    إذا (شاخص < 0)
	أرجع خطأ;
    BString_قلم* قلم = (BString_قلم*)list->قلم_At(شاخص);
    إذا (قلم == عدم)
	أرجع خطأ;
    strncpy(متن, قلم->متن_2(), متن_اندازه);
    أرجع درست;
}


فراغ VimSelectFontDialog::پیام_Received(BMessage *پیام)
{
    بدل (پیام->what) {
	حالة k_Vim_Dialog_OKButton_پیام:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_درازی);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_درازی);
	    *f_اندازه = f_Font_اندازه;
	    f_Dialog_مقدار = درست;
	حالة k_Vim_Dialog_Cancel_Button_پیام:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    أرجع;
	حالة B_کلید_به_بالا:
	    {
		int32 کلید = 0;
		إذا (پیام->پیدا_کن_Int32("raw_char", &کلید) == B_OK
			&& کلید == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    اكسر;

	حالة k_Vim_Dialog_Family_Select_پیام:
	    إذا (Update_From_List_قلم(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_درازی)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    اكسر;
	حالة k_Vim_Dialog_Style_Select_پیام:
	    إذا (Update_From_List_قلم(fStylesList,
		    fFontStyle, B_FONT_STYLE_درازی))
		_UpdateFontPreview();
	    اكسر;
	حالة k_Vim_Dialog_اندازه_Select_پیام:
	    {
		حرف buf[10] = {0};
		إذا (Update_From_List_قلم(fSizesList, buf, حجم(buf))) {
		    عائم اندازه = atof(buf);
		    إذا (اندازه > 0.f) {
			f_Font_اندازه = اندازه;
			Update_اندازه_درون_داد_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    اكسر;
	حالة k_Vim_Dialog_اندازه_درون_داد_پیام:
	    {
		عائم اندازه = atof(f_Sizes_درون_داد->متن_2());
		إذا (اندازه > 0.f) {
		    f_Font_اندازه = اندازه;
		    _UpdateFontPreview();
		}
	    }
	    اكسر;
	افتراضي:
	    اكسر;
    }
    أرجع BWindow::پیام_Received(پیام);
}

#پایان_اگر // FEAT_GUI_DIALOG

#اگر_تعریف_شده FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
ساكن BMessage * Menu_پیام(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect قاب, ثابت حرف *name) :
    BBox(قاب, name, B_FOLLOW_چپ | B_FOLLOW_بالاترین, B_WILL_DRAW | B_قاب_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 شمار = fButtonsList.شمار_اقلام();
    لكل (int32 i = 0; i < شمار; i++)
	حذف (BPictureButton*)fButtonsList.قلم_At(i);
    fButtonsList.Make_خالی();

    حذف نرمال_Buttons_Bitmap;
    حذف grayedButtonsBitmap;
    نرمال_Buttons_Bitmap    = عدم;
    grayedButtonsBitmap  = عدم;
}

    فراغ
VimToolbar::Attached_مقصد_پنجره()
{
    BBox::Attached_مقصد_پنجره();

    مجموعه_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    عائم
VimToolbar::Toolbar_ارتفاع() ثابت
{
    عائم اندازه = عدم == نرمال_Buttons_Bitmap ? 18. : نرمال_Buttons_Bitmap->Bounds().ارتفاع_2();
    أرجع اندازه + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    منطقي
VimToolbar::Modify_Bitmap_مقصد_Grayed(BBitmap *bitmap)
{
    عائم ارتفاع = bitmap->Bounds().ارتفاع_2();
    عائم عرض  = bitmap->Bounds().عرض_2();

    rgb_color *بیتها = (rgb_color*)bitmap->بیتها_2();
    int32 pixels = bitmap->بیتها_درازی() / 4;
    لكل (int32 i = 0; i < pixels; i++) {
	بیتها[i].red = بیتها[i].green =
	بیتها[i].blue = ((uint32)بیتها[i].red + بیتها[i].green + بیتها[i].blue) / 3;
	بیتها[i].alpha /= 4;
    }

    أرجع درست;
}

    منطقي
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    نرمال_Buttons_Bitmap = بارگیری_Vim_Bitmap("builtin-tools.png");
    إذا (نرمال_Buttons_Bitmap == عدم)
	//  customized not found? dig application resources for "builtin-tools" one
	نرمال_Buttons_Bitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    إذا (نرمال_Buttons_Bitmap == عدم)
	أرجع خطأ;

    BMessage archive;
    نرمال_Buttons_Bitmap->Archive(&archive);

    grayedButtonsBitmap = جديد BBitmap(&archive);
    إذا (grayedButtonsBitmap == عدم)
	أرجع خطأ;

    //	modify grayed bitmap
    Modify_Bitmap_مقصد_Grayed(grayedButtonsBitmap);

    أرجع درست;
}

BBitmap *VimToolbar::بارگیری_Vim_Bitmap(ثابت حرف* fileName)
{
    BBitmap *bitmap = عدم;

    صحيح mustfree = 0;
    char_u* runtime_مسیر = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    إذا (runtime_مسیر != عدم && fileName != عدم) {
	BString str_مسیر((حرف*)runtime_مسیر);
	str_مسیر << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(str_مسیر.String());
    }

    إذا (mustfree)
	vim_free(runtime_مسیر);

    أرجع bitmap;
}

    منطقي
VimToolbar::GetPictureFromBitmap(BPicture *picture_مقصد, int32 شاخص, BBitmap *bitmapFrom, منطقي pressed)
{
    عائم اندازه = bitmapFrom->Bounds().ارتفاع_2() + 1.;

    BView view(BRect(0, 0, اندازه, اندازه), "", 0, 0);

    جمع_کن_فرزند(&view);
    view.BeginPicture(picture_مقصد);

    view.مجموعه_بالا_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.مجموعه_Drawing_نما(B_OP_OVER);

    BRect منبع(0, 0, اندازه - 1, اندازه - 1);
    BRect destination(منبع);

    منبع.جابه_جایی_By(اندازه * شاخص, 0);
    destination.جابه_جایی_By(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, منبع, destination);

    إذا (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	اندازه += ButtonMargin * 2 - 1;
	view.Begin_سطر_Array(4);
	view.جمع_کن_سطر(BPoint(0, 0),	 BPoint(اندازه, 0),    shadowColor);
	view.جمع_کن_سطر(BPoint(اندازه, 0),	 BPoint(اندازه, اندازه), shineColor);
	view.جمع_کن_سطر(BPoint(اندازه, اندازه), BPoint(0, اندازه),    shineColor);
	view.جمع_کن_سطر(BPoint(0, اندازه),	 BPoint(0, 0),	     shadowColor);
	view.انتها_سطر_Array();
    }

    view.انتها_Picture();
    Remove_فرزند(&view);

    أرجع درست;
}

    منطقي
VimToolbar::جمع_کن_Button(int32 شاخص, vimmenu_T *menu)
{
    BPictureButton *button = عدم;
    إذا (!menu_is_separator(menu->name)) {
	عائم اندازه = نرمال_Buttons_Bitmap ?
	    نرمال_Buttons_Bitmap->Bounds().ارتفاع_2() + 1. + ButtonMargin * 2 : 18.;
	BRect قاب(0, 0, اندازه, اندازه);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture picture_خاکستری;

	إذا (menu->iconfile == عدم && menu->iconidx >= 0 && نرمال_Buttons_Bitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, نرمال_Buttons_Bitmap, درست);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, نرمال_Buttons_Bitmap, خطأ);
	    GetPictureFromBitmap(&picture_خاکستری, menu->iconidx, grayedButtonsBitmap, خطأ);
	} وإلا {

	    char_u میانگیر[MAXPATHL] = {0};
	    BBitmap *bitmap = عدم;

	    إذا (menu->iconfile) {
		gui_پیدا_کن_iconfile(menu->iconfile, میانگیر, (حرف*)"png");
		bitmap = BTranslationUtils::GetBitmap((حرف*)میانگیر);
	    }

	    إذا (bitmap == عدم && gui_پیدا_کن_bitmap(menu->name, میانگیر, (حرف*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((حرف*)میانگیر);

	    إذا (bitmap == عدم)
		bitmap = جديد BBitmap(BRect(0, 0, اندازه, اندازه), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, درست);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, خطأ);
	    Modify_Bitmap_مقصد_Grayed(bitmap);
	    GetPictureFromBitmap(&picture_خاکستری, 0, bitmap, خطأ);

	    حذف bitmap;
	}

	button = جديد BPictureButton(قاب, (حرف*)menu->name,
		    &pictureOff, &pictureOn, Menu_پیام(menu));

	button->مجموعه_Disabled_On(&picture_خاکستری);
	button->مجموعه_Disabled_Off(&picture_خاکستری);

	button->مجموعه_هدف(gui.vim_متن_Area);

	جمع_کن_فرزند(button);

	menu->button = button;
    }

    منطقي نتیجه = fButtonsList.جمع_کن_قلم(button, شاخص);
    InvalidateLayout();
    أرجع نتیجه;
}

    منطقي
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    إذا (menu->button) {
	إذا (fButtonsList.Remove_قلم(menu->button)) {
	    حذف menu->button;
	    menu->button = عدم;
	}
    }
    أرجع درست;
}

    منطقي
VimToolbar::خاکستری_Button(vimmenu_T *menu, صحيح grey)
{
    إذا (menu->button) {
	int32 شاخص = fButtonsList.شاخص_از(menu->button);
	إذا (شاخص >= 0)
	    menu->button->مجموعه_Enabled(grey ? خطأ : درست);
    }
    أرجع درست;
}

    فراغ
VimToolbar::InvalidateLayout()
{
    int32 جابه_جایی = ToolbarMargin;
    int32 شمار = fButtonsList.شمار_اقلام();
    لكل (int32 i = 0; i < شمار; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.قلم_At(i);
	إذا (button) {
	    button->Move_مقصد(جابه_جایی, ToolbarMargin);
	    جابه_جایی += button->Bounds().عرض_2() + ToolbarMargin;
	} وإلا
	    جابه_جایی += ToolbarMargin * 3;
    }
}

#پایان_اگر /*FEAT_TOOLBAR*/

#إذا defined(FEAT_GUI_TABLINE)

    عائم
Vim_Tab_سطر::Tabline_ارتفاع() ثابت
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    أرجع Tab_ارتفاع(); //  + ToolbarMargin;
}

فراغ
Vim_Tab_سطر::Mouse_پایین(BPoint نقطه)
{
    إذا (!gui_mch_showing_tabline())
	أرجع;

    BMessage *m = پنجره()->Current_پیام();
    أكد(m);

    int32 buttons = 0;
    m->پیدا_کن_Int32("buttons", &buttons);

    int32 clicks = 0;
    m->پیدا_کن_Int32("clicks", &clicks);

    صحيح شاخص = 0; //  0 means here - no tab found
    لكل (صحيح i = 0; i < شمار_Tabs(); i++) {
	إذا (Tab_قاب(i).شامل_است_2(نقطه)) {
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
	BPop_به_بالا_Menu* برداشت_به_بالا_Menu = جديد BPop_به_بالا_Menu("tabLineContextMenu", خطأ, خطأ);
	برداشت_به_بالا_Menu->جمع_کن_قلم(جديد BMenu_قلم(_("Close tabi R"), جديد BMessage(TABLINE_MENU_CLOSE)));
	برداشت_به_بالا_Menu->جمع_کن_قلم(جديد BMenu_قلم(_("New tab    T"), جديد BMessage(TABLINE_MENU_NEW)));
	برداشت_به_بالا_Menu->جمع_کن_قلم(جديد BMenu_قلم(_("Open tab..."), جديد BMessage(TABLINE_MENU_OPEN)));

	Convert_مقصد_Screen(&نقطه);
	BMenu_قلم* قلم = برداشت_به_بالا_Menu->Go(نقطه);
	إذا (قلم != عدم) {
	    event = قلم->Command();
	}

	حذف برداشت_به_بالا_Menu;

    } وإلا {
	//  default processing
	BTabView::Mouse_پایین(نقطه);
	أرجع;
    }

    إذا (event < 0)
	أرجع;

    Vim_Tabline_Menu_پیام tmm;
    tmm.شاخص = شاخص;
    tmm.event = event;
    write_پورت(gui.vdcmp, Vim_پیام::TablineMenu, &tmm, حجم(tmm));
}

فراغ
Vim_Tab_سطر::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    إذا (tab_سطر != عدم) {

	int32 i = 0;
	لكل (; i < tab_سطر->شمار_Tabs(); i++)
	    إذا (هذا == tab_سطر->TabAt(i))
		اكسر;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	إذا (i < tab_سطر->شمار_Tabs()) {
	    Vim_Tabline_پیام tm;
	    tm.شاخص = i + 1;
	    write_پورت(gui.vdcmp, Vim_پیام::Tabline, &tm, حجم(tm));
	}
    }
}

#پایان_اگر //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
ساكن حرف appsig[] = "application/x-vnd.Haiku-Vim-8";
کلید_map *کلید_Map;
حرف *کلید_Map_Chars;
صحيح main_exitcode = 127;

    وضعیت_t
gui_haiku_process_event(bigtime_t timeout)
{
    بنية Vim_پیام vm;
    int32 what;
    ssize_t اندازه;

    اندازه = read_پورت_etc(gui.vdcmp, &what, &vm, حجم(vm),
	    B_TIMEOUT, timeout);

    إذا (اندازه >= 0) {
	بدل (what) {
	    حالة Vim_پیام::کلید_2:
		{
		    char_u *string = vm.u.کلید_2.chars;
		    صحيح len = vm.u.کلید_2.درازی;
		    إذا (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_درون_داد_buf();
			got_int = TRUE;
		    }

		    إذا (vm.u.کلید_2.csi_escape)
#اگر_تعریف_نشده FEAT_MBYTE_IME
		    {
			صحيح	i;
			char_u	buf[2];

			لكل (i = 0; i < len; ++i)
			{
			    جمع_کن_مقصد_درون_داد_buf(string + i, 1);
			    إذا (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (صحيح)KE_CSI;
				جمع_کن_مقصد_درون_داد_buf(buf, 2);
			    }
			}
		    }
#وإلا
			جمع_کن_مقصد_درون_داد_buf_csi(string, len);
#پایان_اگر
		    وإلا
			جمع_کن_مقصد_درون_داد_buf(string, len);
		}
		اكسر;
	    حالة Vim_پیام::Resize:
		gui_resize_shell(vm.u.New_اندازه.عرض, vm.u.New_اندازه.ارتفاع);
		اكسر;
	    حالة Vim_پیام::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_شمار =
			atomic_جمع_کن(&vm.u.Scroll.sb->scroll_Event_شمار, -1);
		    إذا (old_شمار <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.مقدار, vm.u.Scroll.stillDragging);
		}
		اكسر;
#إذا defined(FEAT_MENU)
	    حالة Vim_پیام::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		اكسر;
#پایان_اگر
	    حالة Vim_پیام::Mouse:
		{
		    int32 old_شمار;
		    إذا (vm.u.Mouse.button == MOUSE_DRAG)
			old_شمار =
			    atomic_جمع_کن(&gui.vim_متن_Area->mouse_Drag_Event_شمار, -1);
		    وإلا
			old_شمار = 0;
		    إذا (old_شمار <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		اكسر;
	    حالة Vim_پیام::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		اكسر;
	    حالة Vim_پیام::Focus:
		gui.درون_focus = vm.u.Focus.فعال;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		إذا (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		اكسر;
	    حالة Vim_پیام::Refs:
		::RefsReceived(vm.u.Refs.پیام_2, vm.u.Refs.changedir);
		اكسر;
	    حالة Vim_پیام::Tabline:
		send_tabline_event(vm.u.Tabline.شاخص);
		اكسر;
	    حالة Vim_پیام::TablineMenu:
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
vim_قفل_screen()
{
    أرجع !gui.vim_پنجره || gui.vim_پنجره->قفل_2();
}

    فراغ
vim_unlock_screen()
{
    إذا (gui.vim_پنجره)
	gui.vim_پنجره->Unlock();
}

#تعریف_کن پیاپی_BAPPLICATION_درون_NEW_THREAD	0

#إذا پیاپی_BAPPLICATION_درون_NEW_THREAD

    int32
پیاپی_vimapp(فراغ *آرگومانها)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.پیاپی_2();		    // Run until Quit() called

    أرجع 0;
}

#وإلا

    int32
call_main(فراغ *آرگومانها)
{
    بنية Main_آرگومانها *ma = (Main_آرگومانها *)آرگومانها;

    أرجع اصلی(ma->argc, ma->argv);
}
#پایان_اگر

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
	get_thread_info(پیدا_کن_thread(عدم), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_پورت(B_MAX_پورت_شمار, "vim VDCMP");

#إذا پیاپی_BAPPLICATION_درون_NEW_THREAD
	thread_id tid = spawn_thread(پیاپی_vimapp, "vim VimApp",
		tinfo.اولویت, عدم);
	إذا (tid >= B_OK) {
	    resume_thread(tid);
	} وإلا {
	    getout(1);
	}
#وإلا
	Main_آرگومانها ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.اولویت, &ma);
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
	    app.پیاپی_2();		    // Run until Quit() called
	    // fprintf(stderr, "app.Run() returned...\n");
	    وضعیت_t dummy_exitcode;
	    (فراغ)wait_for_thread(tid, &dummy_exitcode);

	    /*
	     * This path should be the normal one taken to exit Vim.
	     * The main() thread calls mch_exit() which calls
	     * gui_mch_exit() which terminates its thread.
	     */
	    خروج(main_exitcode);
	}
#پایان_اگر
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
    gui.def_پشت_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.پشت_pixel = gui.def_پشت_pixel;

    gui.scrollbar_عرض = (صحيح) B_V_SCROLL_BAR_عرض;
    gui.scrollbar_ارتفاع = (صحيح) B_H_SCROLL_BAR_ارتفاع;
#اگر_تعریف_شده FEAT_MENU
    gui.menu_ارتفاع = 19;   //	initial guess -
    //	correct for my default settings
#پایان_اگر
    gui.border_جابه_جایی = 3;  //	coordinates are inside window borders

    إذا (gui.vdcmp < B_OK)
	أرجع FAIL;
    get_کلید_map(&کلید_Map, &کلید_Map_Chars);

    gui.vim_پنجره = جديد Vim_پنجره();	// hidden and locked
    إذا (!gui.vim_پنجره)
	أرجع FAIL;

    gui.vim_پنجره->پیاپی_2();	// Run() unlocks but does not show

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    مجموعه_نرمال_colors();

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
    rgb_color rgb = GUI_مقصد_RGB(gui.پشت_pixel);

    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vimForm->مجموعه_View_Color(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vim_پنجره->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحيح
gui_mch_open()
{
    إذا (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_مجموعه_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_پنجره->Show();
	gui.vim_پنجره->Unlock();
	أرجع OK;
    }

    أرجع FAIL;
}

    فراغ
gui_mch_exit(صحيح vim_exitcode)
{
    إذا (gui.vim_پنجره) {
	thread_id tid = gui.vim_پنجره->Thread();
	gui.vim_پنجره->قفل_2();
	gui.vim_پنجره->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_پورت(gui.vdcmp);
#إذا !پیاپی_BAPPLICATION_درون_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#پایان_اگر
    إذا (gui.vimApp) {
	Vim_متن_Area_View::guiBlankMouse(خطأ);

	main_exitcode = vim_exitcode;
#إذا پیاپی_BAPPLICATION_درون_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->قفل_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#وإلا
	gui.vimApp->قفل_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	// suicide
	exit_thread(vim_exitcode);
#پایان_اگر
    }
    // If we are somehow still here, let mch_exit() handle things.
}

/*
 * Get the position of the top left corner of the window.
 */
    صحيح
gui_mch_get_winpos(صحيح *x, صحيح *y)
{
    إذا (gui.vim_پنجره->قفل_2()) {
	BRect r;
	r = gui.vim_پنجره->قاب_2();
	gui.vim_پنجره->Unlock();
	*x = (صحيح)r.چپ;
	*y = (صحيح)r.بالاترین;
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
gui_mch_مجموعه_winpos(صحيح x, صحيح y)
{
    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_پنجره->Move_مقصد(x, y);
	gui.vim_پنجره->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
فراغ
gui_mch_مجموعه_shellsize(
	صحيح	عرض,
	صحيح	ارتفاع,
	صحيح	min_عرض,
	صحيح	min_ارتفاع,
	صحيح	پایه_عرض,
	صحيح	پایه_ارتفاع,
	صحيح	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_پنجره->Resize_مقصد(عرض - PEN_عرض, ارتفاع - PEN_عرض);

	// set size limits
	عائم min_عرض_2, max_عرض, min_ارتفاع_2, max_ارتفاع;

	gui.vim_پنجره->Get_اندازه_Limits(&min_عرض_2, &max_عرض,
		&min_ارتفاع_2, &max_ارتفاع);
	gui.vim_پنجره->مجموعه_اندازه_Limits(min_عرض, max_عرض,
		min_ارتفاع, max_ارتفاع);

	/*
	 * Set the resizing alignment depending on font size.
	 */
	gui.vim_پنجره->مجموعه_پنجره_Alignment(
		B_PIXEL_ALIGNMENT,	//  window_alignment mode,
		1,		//  int32 h,
		0,		//  int32 hOffset = 0,
		gui.char_عرض,	    //	int32 width = 0,
		پایه_عرض,	    //	int32 widthOffset = 0,
		1,		//  int32 v = 0,
		0,		//  int32 vOffset = 0,
		gui.char_ارتفاع,	//  int32 height = 0,
		پایه_ارتفاع	    //	int32 heightOffset = 0
		);

	gui.vim_پنجره->Unlock();
    }
}

فراغ
gui_mch_get_screen_dimensions(
	صحيح	*screen_w,
	صحيح	*screen_h)
{
    BRect قاب;

    {
	BScreen screen(gui.vim_پنجره);

	إذا (screen.IsValid()) {
	    قاب = screen.قاب_2();
	} وإلا {
	    قاب.راست = 640;
	    قاب.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (صحيح) قاب.راست - 2 * gui.scrollbar_عرض - 20;
    *screen_h = (صحيح) قاب.bottom - gui.scrollbar_ارتفاع
#اگر_تعریف_شده FEAT_MENU
	- gui.menu_ارتفاع
#پایان_اگر
	- 30;
}

فراغ
gui_mch_مجموعه_متن_area_pos(
	صحيح	x,
	صحيح	y,
	صحيح	w,
	صحيح	h)
{
    إذا (!gui.vim_متن_Area)
	أرجع;

    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_متن_Area->Move_مقصد(x, y);
	gui.vim_متن_Area->Resize_مقصد(w - PEN_عرض, h - PEN_عرض);

#اگر_تعریف_شده FEAT_GUI_TABLINE
	إذا (gui.vimForm->Tab_سطر() != عدم) {
	    gui.vimForm->Tab_سطر()->Resize_مقصد(w, gui.vimForm->Tabline_ارتفاع());
	}
#پایان_اگر // FEAT_GUI_TABLINE

	gui.vim_پنجره->Unlock();
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
    إذا (gui.vim_پنجره->قفل_2()) {
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
	gui.vim_پنجره->Unlock();
    }
}

فراغ
gui_mch_مجموعه_scrollbar_thumb(
	scrollbar_T *sb,
	صحيح	val,
	صحيح	اندازه,
	صحيح	max)
{
    إذا (gui.vim_پنجره->قفل_2()) {
	VimScrollBar *s = sb->id;
	إذا (max == 0) {
	    s->مجموعه_مقدار(0);
	    s->مجموعه_Range(0.0, 0.0);
	} وإلا {
	    s->مجموعه_Proportion((عائم)اندازه / (max + 1.0));
	    s->مجموعه_گامها(1.0, اندازه > 5 ? اندازه - 2 : اندازه);
#اگر_تعریف_نشده SCROLL_PAST_انتها	    //	really only defined in gui.c...
	    max = max + 1 - اندازه;
#پایان_اگر
	    إذا (max < s->مقدار_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->مجموعه_مقدار(val);
		s->مجموعه_Range(0.0, max);
	    } وإلا {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->مجموعه_Range(0.0, max);
		s->مجموعه_مقدار(val);
	    }
	}
	gui.vim_پنجره->Unlock();
    }
}

فراغ
gui_mch_مجموعه_scrollbar_pos(
	scrollbar_T *sb,
	صحيح	x,
	صحيح	y,
	صحيح	w,
	صحيح	h)
{
    إذا (gui.vim_پنجره->قفل_2()) {
	BRect winb = gui.vim_پنجره->Bounds();
	عائم vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->Resize_مقصد(w - PEN_عرض, h - PEN_عرض);
	إذا (winb.راست-(x+w)<w) vsbx = winb.راست - (w - PEN_عرض);
	vsb->Move_مقصد(vsbx, vsby);
	gui.vim_پنجره->Unlock();
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
    إذا (gui.vim_پنجره->قفل_2()) {
	vsb->مجموعه_هدف(gui.vim_متن_Area);
	vsb->Hide();
	gui.vimForm->جمع_کن_فرزند(vsb);
	gui.vim_پنجره->Unlock();
    }
}

#إذا defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
فراغ
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    إذا (gui.vim_پنجره->قفل_2()) {
	sb->id->RemoveSelf();
	حذف sb->id;
	gui.vim_پنجره->Unlock();
    }
}
#پایان_اگر

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

#تعریف_کن BLINK_NONE  0
#تعریف_کن BLINK_OFF   1
#تعریف_کن BLINK_ON    2

ساكن صحيح	blink_state = BLINK_NONE;
ساكن long_u	    blink_waittime = 700;
ساكن long_u	    blink_ontime = 400;
ساكن long_u	    blink_offtime = 250;
ساكن صحيح  blink_timer = 0;

فراغ
gui_mch_مجموعه_blinking(
	دراز	waittime,
	دراز	on,
	دراز	off)
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
gui_mch_شروع_blink()
{
    // TODO
    إذا (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.درون_focus)
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
    إذا (gui.vim_پنجره->قفل_2())
    {
	صحيح rc = gui.vim_متن_Area->mchInitFont(font_name);
	gui.vim_پنجره->Unlock();

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
	حذف dialog;
	أرجع ret;
#وإلا
    أرجع NOFONT;
#پایان_اگر // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	صحيح	    giveErrorIfMissing)
{
    ساكن VimFont *fontList = عدم;

    إذا (!gui.درون_use)	//  can't do this when GUI not running
	أرجع NOFONT;

    //	storage for locally modified name;
    ثابت صحيح buff_اندازه = B_FONT_FAMILY_درازی + B_FONT_STYLE_درازی + 20;
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
	    useSelectGUI = درست;
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
    حرف* انتها = 0;
    طالما (انتها = strchr((حرف *)font_name, '_'))
	*انتها = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    ساكن حرف buff[buff_اندازه] = {0};
    STRNCPY(buff, font_name, buff_اندازه);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_درازی);
    حرف* style_s = strtok(0, "/\0");
    إذا (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_درازی);
    اندازه = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    إذا (useSelectGUI) {
	إذا (gui_mch_font_dialog(&family, &style, &اندازه) == NOFONT)
	    أرجع FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_اندازه,
		(حرف*)"%s/%s/%.0f", family, style, اندازه);
	hl_مجموعه_font_name((char_u*)font_name);

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
    لكل (flp = fontList; flp; flp = flp->بعدی) {
	إذا (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    أرجع (GuiFont)flp;
	}
    }

    VimFont *font = جديد VimFont();
    font->name = vim_strsave((char_u*)font_name);

    إذا (شمار_font_styles(family) <= 0) {
	إذا (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	حذف font;
	أرجع NOFONT;
    }

    //	Remember font in the static list for later use
    font->بعدی = fontList;
    fontList = font;

    font->مجموعه_Family_And_Style(family, style);
    إذا (اندازه > 0.f)
	font->مجموعه_اندازه(اندازه);

    font->مجموعه_Spacing(B_FIXED_SPACING);
    font->مجموعه_Encoding(B_UNICODE_UTF8);

    أرجع (GuiFont)font;
}

/*
 * Set the current text font.
 */
فراغ
gui_mch_مجموعه_font(
	GuiFont	font)
{
    إذا (gui.vim_پنجره->قفل_2()) {
	VimFont *vf = (VimFont *)font;

	gui.vim_متن_Area->مجموعه_Font(vf);

	gui.char_عرض = (صحيح) vf->String_عرض("n");
	font_ارتفاع fh;
	vf->Get_ارتفاع(&fh);
	gui.char_ارتفاع = (صحيح)(fh.ascent + 0.9999)
	    + (صحيح)(fh.descent + 0.9999) + (صحيح)(fh.leading + 0.9999);
	gui.char_ascent = (صحيح)(fh.ascent + 0.9999);

	gui.vim_پنجره->Unlock();
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
	حذف f;
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
	font_ارتفاع fh = {0};
	font->Get_ارتفاع(&fh);
	gui.char_ارتفاع = (صحيح)(fh.ascent + fh.descent + 0.5) + p_linespace;
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
gui_mch_مجموعه_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_مقصد_RGB(color);
    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_متن_Area->مجموعه_بالا_Color(rgb);
	gui.vim_پنجره->Unlock();
    }
}

/*
 * Set the current text background color.
 */
فراغ
gui_mch_مجموعه_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_مقصد_RGB(color);
    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_متن_Area->مجموعه_کم_Color(rgb);
	gui.vim_پنجره->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    فراغ
gui_mch_مجموعه_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

فراغ
gui_mch_draw_string(
	صحيح	ردیف,
	صحيح	ستون,
	char_u	*s,
	صحيح	len,
	صحيح	flags)
{
    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_متن_Area->mchDrawString(ردیف, ستون, s, len, flags);
	gui.vim_پنجره->Unlock();
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

    لكل (i = 0; special_کلیدها[i].Be_کلیدها != 0; i++)
	إذا (name[0] == special_کلیدها[i].vim_code0 &&
		name[1] == special_کلیدها[i].vim_code1)
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

    إذا (gui.vim_پنجره->قفل_2()) {
	BRect rect = gui.vim_متن_Area->Bounds();

	gui.vim_متن_Area->مجموعه_Drawing_نما(B_OP_INVERT);
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->Sync();
	snooze(msec * 1000);	 // wait for a few msec
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->مجموعه_Drawing_نما(B_OP_COPY);
	gui.vim_متن_Area->Flush();
	gui.vim_پنجره->Unlock();
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
    rect.چپ = FILL_X(c);
    rect.بالاترین = FILL_Y(r);
    rect.راست = rect.چپ + nc * gui.char_عرض - PEN_عرض;
    rect.bottom = rect.بالاترین + nr * gui.char_ارتفاع - PEN_عرض;

    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_متن_Area->مجموعه_Drawing_نما(B_OP_INVERT);
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->مجموعه_Drawing_نما(B_OP_COPY);
	gui.vim_پنجره->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    فراغ
gui_mch_iconify()
{
    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_پنجره->Minimize(درست);
	gui.vim_پنجره->Unlock();
    }
}

#إذا defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    فراغ
gui_mch_مجموعه_foreground(فراغ)
{
    // TODO
}
#پایان_اگر

/*
 * Set the window title
 */
فراغ
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_پنجره->مجموعه_Title((حرف *)title);
	gui.vim_پنجره->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    فراغ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_مجموعه_fg_color(color);

    BRect r;
    r.چپ = FILL_X(gui.ستون);
    r.بالاترین = FILL_Y(gui.ردیف);
    صحيح cells = utf_off2cells(سطر_جابه_جایی[gui.ردیف] + gui.ستون, 100); // TODO-TODO
    إذا (cells>=4) cells = 1;
    r.راست = r.چپ + cells*gui.char_عرض - PEN_عرض;
    r.bottom = r.بالاترین + gui.char_ارتفاع - PEN_عرض;

    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_متن_Area->StrokeRect(r);
	gui.vim_پنجره->Unlock();
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
    gui_mch_مجموعه_fg_color(color);

    BRect r;
    r.چپ =
#اگر_تعریف_شده FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_راست ? FILL_X(gui.ستون + 1) - w :
#پایان_اگر
	FILL_X(gui.ستون);
    r.راست = r.چپ + w - PEN_عرض;
    r.bottom = FILL_Y(gui.ردیف + 1) - PEN_عرض;
    r.بالاترین = r.bottom - h + PEN_عرض;

    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_متن_Area->FillRect(r);
	gui.vim_پنجره->Unlock();
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
    طالما (پورت_شمار(gui.vdcmp) > 0 &&
	    !vim_is_درون_داد_buf_پر() &&
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
	دراز	wtime)
{
    صحيح		focus;
    bigtime_t	until, timeout;
    وضعیت_t	st;

    إذا (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_زمان() + timeout;
    }
    وإلا
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.درون_focus;
    لكل (;;)
    {
	// Stop or start blinking when focus changes
	إذا (gui.درون_focus != focus)
	{
	    إذا (gui.درون_focus)
		gui_mch_شروع_blink();
	    وإلا
		gui_mch_stop_blink(TRUE);
	    focus = gui.درون_focus;
	}

	gui_mch_flush();

#اگر_تعریف_شده پیام_صف
# ifdef FEAT_TIMERS
	did_جمع_کن_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	إذا (did_جمع_کن_timer)
	    // Need to recompute the waiting time.
	    اكسر;
# endif
# ifdef FEAT_کار_CHANNEL
	إذا (has_any_channel())
	{
	    إذا (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	وإلا إذا (wtime < 0)
	    timeout = B_INFINITE_TIMEOUT;
# endif
#پایان_اگر

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use gui_haiku_process_event() to hang until
	 * an event arrives.  No need to check for input_buf_full because we
	 * are returning as soon as it contains a single char.
	 */
	st = gui_haiku_process_event(timeout);

	إذا (درون_داد_موجود())
	    أرجع OK;
	إذا (st < B_OK)		// includes B_TIMED_OUT
	    أرجع FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	إذا (wtime >= 0)
	{
	    timeout = until - system_زمان();
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
    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_پنجره->Flush();
	gui.vim_پنجره->Unlock();
    }
    أرجع;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
فراغ
gui_mch_clear_بلوک(
	صحيح	row1,
	صحيح	col1,
	صحيح	row2,
	صحيح	col2)
{
    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_متن_Area->mch_Clear_بلوک(row1, col1, row2, col2);
	gui.vim_پنجره->Unlock();
    }
}

    فراغ
gui_mch_clear_all()
{
    إذا (gui.vim_پنجره->قفل_2()) {
	gui.vim_متن_Area->mchClearAll();
	gui.vim_پنجره->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
فراغ
gui_mch_delete_سطرها(
	صحيح	ردیف,
	صحيح	رقم_سطرها)
{
    gui.vim_متن_Area->mch_Delete_سطرها(ردیف, رقم_سطرها);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
فراغ
gui_mch_درج_سطرها(
	صحيح	ردیف,
	صحيح	رقم_سطرها)
{
    gui.vim_متن_Area->mch_درج_سطرها(ردیف, رقم_سطرها);
}

#إذا defined(FEAT_MENU)
/*
 * Menu stuff.
 */

فراغ
gui_mch_enable_menu(
	صحيح	flag)
{
    إذا (gui.vim_پنجره->قفل_2())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->مجموعه_Enabled(flag);
	gui.vim_پنجره->Unlock();
    }
}

فراغ
gui_mch_مجموعه_menu_pos(
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
gui_mch_جمع_کن_menu(
	vimmenu_T   *menu,
	صحيح	idx)
{
    vimmenu_T	*والد = menu->والد;

    //	popup menu - just create it unattached
    إذا (menu_is_popup(menu->name) && والد == عدم) {
	BPop_به_بالا_Menu* برداشت_به_بالا_Menu = جديد BPop_به_بالا_Menu((ثابت حرف*)menu->name, خطأ, خطأ);
	menu->submenu_id = برداشت_به_بالا_Menu;
	menu->id = عدم;
	أرجع;
    }

    إذا (!menu_is_menubar(menu->name)
	    || (والد != عدم && والد->submenu_id == عدم))
	أرجع;

    إذا (gui.vim_پنجره->قفل_2())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	إذا ( والد )
	    tmp = والد->submenu_id;
	وإلا
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	إذا ( ! tmp->پیدا_کن_قلم((ثابت حرف *) menu->dname)) {

	    BMenu *bmenu = جديد BMenu((حرف *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->جمع_کن_قلم(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->پیدا_کن_قلم((ثابت حرف *) menu->dname);

	}
	gui.vim_پنجره->Unlock();
    }
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
    // no tearoff menus
}

    ساكن BMessage *
Menu_پیام(vimmenu_T *menu)
{
    BMessage *m = جديد BMessage('menu');
    m->جمع_کن_اشاره_گر("VimMenu", (فراغ *)menu);

    أرجع m;
}

/*
 * Add a menu item to a menu
 */
فراغ
gui_mch_جمع_کن_menu_قلم(
	vimmenu_T   *menu,
	صحيح	idx)
{
    صحيح	    mnemonic = 0;
    vimmenu_T	*والد = menu->والد;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    إذا (gui.vim_پنجره->قفل_2())
    {
#اگر_تعریف_شده FEAT_TOOLBAR
	إذا (menu_is_toolbar(والد->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->جمع_کن_Button(idx, menu);
	    }
	} وإلا
#پایان_اگر

	إذا (والد->submenu_id != عدم || menu_is_popup(والد->name)) {
	    إذا (menu_is_separator(menu->name)) {
		BSeparator_قلم *قلم = جديد BSeparator_قلم();
		والد->submenu_id->جمع_کن_قلم(قلم);
		menu->id = قلم;
		menu->submenu_id = عدم;
	    }
	    وإلا {
		BMenu_قلم *قلم = جديد BMenu_قلم((حرف *)menu->dname,
			Menu_پیام(menu));
		قلم->مجموعه_هدف(gui.vim_متن_Area);
		قلم->مجموعه_Trigger((حرف) menu->mnemonic);
		والد->submenu_id->جمع_کن_قلم(قلم);
		menu->id = قلم;
		menu->submenu_id = عدم;
	    }
	}
	gui.vim_پنجره->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
فراغ
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    إذا (gui.vim_پنجره->قفل_2())
    {
#اگر_تعریف_شده FEAT_TOOLBAR
	إذا (menu->والد && menu_is_toolbar(menu->والد->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->RemoveButton(menu);
	    }
	} وإلا
#پایان_اگر
	{
	    أكد(menu->submenu_id == عدم || menu->submenu_id->شمار_اقلام() == 0);
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
		bmenu->Remove_قلم(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		إذا (bmenu == gui.vimForm->MenuBar() && bmenu->شمار_اقلام() == 0)
		{
		    bmenu->Resize_مقصد(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    حذف menu->id;
	    menu->id = عدم;
	    menu->submenu_id = عدم;

	    gui.menu_ارتفاع = (صحيح) gui.vimForm->Menu_ارتفاع();
	}
	gui.vim_پنجره->Unlock();
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
#اگر_تعریف_شده FEAT_TOOLBAR
    إذا (menu->والد && menu_is_toolbar(menu->والد->name)) {
	إذا (gui.vim_پنجره->قفل_2()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->خاکستری_Button(menu, grey);
	    }
	    gui.vim_پنجره->Unlock();
	}
    } وإلا
#پایان_اگر
    إذا (menu->id != عدم)
	menu->id->مجموعه_Enabled(!grey);
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
	menu->id->مجموعه_Enabled(!hidden);
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

    BPop_به_بالا_Menu* popupMenu = dynamic_cast<BPop_به_بالا_Menu*>(menu->submenu_id);
    إذا (popupMenu == عدم)
	أرجع;

    BPoint نقطه;
    إذا (gui.vim_پنجره->قفل_2()) {
	uint32 buttons = 0;
	gui.vim_متن_Area->GetMouse(&نقطه, &buttons);
	gui.vim_متن_Area->Convert_مقصد_Screen(&نقطه);
	gui.vim_پنجره->Unlock();
    }
    popupMenu->Go(نقطه, درست);
}

#پایان_اگر // FEAT_MENU

// Mouse stuff

#اگر_تعریف_شده FEAT_CLIPBOARD
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
    إذا (be_clipboard->قفل_2())
    {
	BMessage *m = be_clipboard->داده_2();
	// m->PrintToStream();

	char_u *string = عدم;
	ssize_t stringlen = -1;

	إذا (m->پیدا_کن_داده(textplain, B_MIME_TYPE,
		    (ثابت فراغ **)&string, &stringlen) == B_OK
		|| m->پیدا_کن_String("text", (ثابت حرف **)&string) == B_OK)
	{
	    إذا (stringlen == -1)
		stringlen = STRLEN(string);

	    صحيح type;
	    حرف *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    إذا (m->پیدا_کن_داده(vimselectiontype, B_MIME_TYPE,
			(ثابت فراغ **)&seltype, &seltypelen) == B_OK)
	    {
		بدل (*seltype)
		{
		    افتراضي:
		    حالة 'L':	type = MLINE;	اكسر;
		    حالة 'C':	type = MCHAR;	اكسر;
#اگر_تعریف_شده FEAT_VISUAL
		    حالة 'B':	type = MBLOCK;	اكسر;
#پایان_اگر
		}
	    }
	    وإلا
	    {
		// Otherwise use heuristic as documented
		type = memchr(string, stringlen, '\n') ? MLINE : MCHAR;
	    }
	    clip_yank_selection(type, string, (دراز)stringlen, cbd);
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
clip_mch_مجموعه_selection(Clipboard_T *cbd)
{
    إذا (be_clipboard->قفل_2())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->داده_2();
	أكد(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = عدم;
	long_u	شمار;
	صحيح type;

	type = clip_convert_selection(&str, &شمار, cbd);

	إذا (type < 0)
	    أرجع;

	m->جمع_کن_داده(textplain, B_MIME_TYPE, (فراغ *)str, شمار);

	// Add type of selection
	حرف	vtype;
	بدل (type)
	{
	    افتراضي:
	    حالة MLINE:    vtype = 'L';    اكسر;
	    حالة MCHAR:    vtype = 'C';    اكسر;
#اگر_تعریف_شده FEAT_VISUAL
	    حالة MBLOCK:   vtype = 'B';    اكسر;
#پایان_اگر
	}
	m->جمع_کن_داده(vimselectiontype, B_MIME_TYPE, (فراغ *)&vtype, 1);

	vim_free(str);

	be_clipboard->Commit();
	be_clipboard->Unlock();
    }
}

#پایان_اگر	// FEAT_CLIPBOARD

#اگر_تعریف_شده FEAT_BROWSE
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
	    جديد BMessage((saving == TRUE) ? 'save' : 'open'), عدم, درست);

    gui.vimApp->f_Browsed_مسیر.Unset();

    gui.vimApp->fFilePanel->پنجره()->مجموعه_Title((حرف*)title);
    gui.vimApp->fFilePanel->مجموعه_Panel_Directory((ثابت حرف*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    طالما (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = عدم;
    وضعیت_t نتیجه = gui.vimApp->f_Browsed_مسیر.InitCheck();
    إذا (نتیجه == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->f_Browsed_مسیر.مسیر_2());
    } وإلا
	إذا (نتیجه != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    نتیجه, strerror(نتیجه));
	}

    حذف gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = عدم;

    أرجع fileName;
}
#پایان_اگر // FEAT_BROWSE


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
	char_u	*پیام_2,
	char_u	*buttons,
	صحيح	 dfltbutton,
	char_u	*textfield,
	صحيح ex_cmd)
{
    VimDialog *dialog = جديد VimDialog(type, (حرف*)title, (حرف*)پیام_2,
	    (حرف*)buttons, dfltbutton, (حرف*)textfield, ex_cmd);
    منطقي ret = dialog->Go();
    حذف dialog;
	أرجع ret;
}

#پایان_اگر // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_مقصد_RGB(pixel);

    أرجع ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    فراغ
gui_mch_setmouse(صحيح x, صحيح y)
{
    TRACE();
    // TODO
}

#اگر_تعریف_شده FEAT_MBYTE_IME
    فراغ
im_مجموعه_position(صحيح ردیف, صحيح ستون)
{
    إذا (gui.vim_پنجره->قفل_2())
    {
	gui.vim_متن_Area->DrawIMString();
	gui.vim_پنجره->Unlock();
    }
    أرجع;
}
#پایان_اگر

    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_ارتفاع = (toolbar && showit) ? toolbar->Toolbar_ارتفاع() : 0.;
}

    فراغ
gui_mch_مجموعه_toolbar_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    إذا (toolbar != عدم) {
	إذا (gui.vim_پنجره->قفل_2()) {
	    toolbar->Move_مقصد(x, y);
	    toolbar->Resize_مقصد(w - 1, h - 1);
	    gui.vim_پنجره->Unlock();
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
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();

    إذا (tab_سطر == عدم)
	أرجع;

    إذا (!showit != !gui.vimForm->Is_Showing_Tab_سطر()) {
	gui.vimForm->مجموعه_Showing_Tab_سطر(showit != 0);
	gui.tabline_ارتفاع = gui.vimForm->Tabline_ارتفاع();
    }
}

    فراغ
gui_mch_مجموعه_tabline_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    إذا (tab_سطر != عدم) {
	إذا (gui.vim_پنجره->قفل_2()) {
	    tab_سطر->Move_مقصد(x, y);
	    tab_سطر->Resize_مقصد(w - 1, h - 1);
	    gui.vim_پنجره->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحيح
gui_mch_showing_tabline()
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    أرجع tab_سطر != عدم && gui.vimForm->Is_Showing_Tab_سطر();
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

    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();

    إذا (tab_سطر == عدم)
	أرجع;

    gui.vim_پنجره->قفل_2();

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = اولی_tabpage; tp != عدم; tp = tp->tp_بعدی, ++nr) {
	إذا (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_سطر->TabAt(nr);

	إذا (tab == عدم) {
	    tab = جديد Vim_Tab_سطر::VimTab();
	    tab_سطر->جمع_کن_Tab(عدم, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->مجموعه_Label((ثابت حرف*)NameBuff);
	tab_سطر->Invalidate();
    }

    // Remove any old labels.
    طالما (nr < tab_سطر->شمار_Tabs())
	tab_سطر->RemoveTab(nr);

    إذا (tab_سطر->Selection() != curtabidx)
	tab_سطر->Select(curtabidx);

    gui.vim_پنجره->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_مجموعه_curtab(صحيح nr)
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    إذا (tab_سطر == عدم)
	أرجع;

    gui.vim_پنجره->قفل_2();

    إذا (tab_سطر->Selection() != nr -1)
	tab_سطر->Select(nr -1);

    gui.vim_پنجره->Unlock();
}

#پایان_اگر // FEAT_GUI_TABLINE
