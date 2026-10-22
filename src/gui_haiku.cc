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

خارجی "C" {

#شامل <assert.h>
#شامل <float.h>
#شامل <syslog.h>

#شامل "vim.h"
#شامل "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#شامل <Application.h>
#شامل <Beep.h>
#شامل <Bitmap.h>
#شامل <Box.h>
#شامل <Button.h>
#شامل <Clipboard.h>
#شامل <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#شامل <File.h>
#شامل <FilePanel.h>
#شامل <FindDirectory.h>
//#include <Font.h>
#شامل <IconUtils.h>
#شامل <Input.h>
#شامل <ListView.h>
#شامل <MenuBar.h>
#شامل <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#شامل <Path.h>
#شامل <PictureButton.h>
#شامل <PopUpMenu.h>
//#include <Region.h>
#شامل <Resources.h>
//#include <Roster.h>
#شامل <Screen.h>
#شامل <ScrollBar.h>
#شامل <ScrollView.h>
#شامل <String.h>
#شامل <StringView.h>
//#include <SupportDefs.h>
#شامل <TabView.h>
#شامل <TextControl.h>
#شامل <TextView.h>
#شامل <TranslationUtils.h>
#شامل <TranslatorFormats.h>
#شامل <View.h>
#شامل <Window.h>

جماعت VimApp;
جماعت VimFormView;
جماعت Vim_متن_Area_View;
جماعت Vim_کھڑکی;
جماعت VimToolbar;
جماعت Vim_Tab_سطر;

خارجی کلید_map *کلید_Map;
خارجی حرف *کلید_Map_Chars;

خارجی صحیح مین(صحیح argc, حرف **argv);

#اگر_غیر_تعریف_شدہ B_MAX_پورٹ_گنتی
#تعریف_کرو B_MAX_پورٹ_گنتی    255
#اگر_ختم

// VimApp seems comparable to the X "vimShell"
جماعت VimApp: عوامی BApplication
{
    قسمتعریف BApplication Inherited;
    عوامی:
    VimApp(ثابت حرف *appsig);
    ~VimApp();

    // callbacks:
#اگر 0
    مجازی خالی Dispatch_پیغام(BMessage *m, BHandler *h)
    {
	m->Print_مقصد_دھارا();
	Inherited::Dispatch_پیغام(m, h);
    }
#اگر_ختم
    مجازی خالی Ready_مقصد_تسلسل();
    مجازی خالی ArgvReceived(int32 argc, حرف **argv);
    مجازی خالی RefsReceived(BMessage *m);
    مجازی منطقی QuitRequested();
    مجازی خالی پیغام_Received(BMessage *m);

    ساکن خالی SendRefs(BMessage *m, منطقی changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	f_Browsed_راستہ;
    نجی:
};

جماعت Vim_کھڑکی: عوامی BWindow
{
    قسمتعریف BWindow Inherited;
    عوامی:
    Vim_کھڑکی();
    ~Vim_کھڑکی();

    مجازی خالی Dispatch_پیغام(BMessage *m, BHandler *h);
    مجازی خالی کھڑکی_Activated(منطقی فعال);
    مجازی منطقی QuitRequested();

    VimFormView	    *formView;

    نجی:
    خالی init();

    منطقی is_fullscreen = جھوٹ;
    BRect saved_چوکھٹا;
    کھڑکی_look saved_look;
};

جماعت VimFormView: عوامی BView
{
    قسمتعریف BView Inherited;
    عوامی:
    VimFormView(BRect چوکھٹا);
    ~VimFormView();

    // callbacks:
    مجازی خالی AllAttached();
    مجازی خالی چوکھٹا_Resized(اعشاری new_چوڑائی, اعشاری new_اونچائی);

#تعریف_کرو MENUBAR_MARGIN	1
    اعشاری Menu_اونچائی() ثابت
    { واپس menuBar ? menuBar->چوکھٹا_2().اونچائی_2() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() ثابت
    { واپس menuBar; }

    نجی:
    خالی init(BRect);

    BMenuBar	    *menuBar;
    Vim_متن_Area_View *متن_Area;

#اگر_تعریف_شدہ FEAT_TOOLBAR
    عوامی:
    اعشاری Toolbar_اونچائی() ثابت;
    VimToolbar *ToolBar() ثابت
    { واپس toolBar; }
    نجی:
    VimToolbar	    *toolBar;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    عوامی:
    Vim_Tab_سطر *Tab_سطر() ثابت	{ واپس tab_سطر; }
    منطقی Is_Showing_Tab_سطر() ثابت { واپس showing_Tab_سطر; }
    خالی سیٹ_Showing_Tab_سطر(منطقی showing) { showing_Tab_سطر = showing;	}
    اعشاری Tabline_اونچائی() ثابت;
    نجی:
    Vim_Tab_سطر	*tab_سطر;
    صحیح	showing_Tab_سطر;
#اگر_ختم
};

جماعت Vim_متن_Area_View: عوامی BView
{
    قسمتعریف BView Inherited;
    عوامی:
    Vim_متن_Area_View(BRect چوکھٹا);
    ~Vim_متن_Area_View();

    // callbacks:
    مجازی خالی Draw(BRect updateRect);
    مجازی خالی کلید_نیچے_کو(ثابت حرف *بائٹیں, int32 عدد_بائٹیں_2);
    مجازی خالی Mouse_نیچے_کو(BPoint نقطہ);
    مجازی خالی Mouse_اوپر(BPoint نقطہ);
    مجازی خالی MouseMoved(BPoint نقطہ, uint32 transit, ثابت BMessage *پیغام_2);
    مجازی خالی پیغام_Received(BMessage *m);

    // own functions:
    صحیح mchInitFont(char_u *name);
    خالی mchDrawString(صحیح صف, صحیح ستون, char_u *s, صحیح len, صحیح flags);
    خالی mch_Clear_بلاک(صحیح row1, صحیح col1, صحیح row2, صحیح col2);
    خالی mchClearAll();
    خالی mch_Delete_سطریں(صحیح صف, صحیح عدد_سطریں);
    خالی mch_داخل_کرو_سطریں(صحیح صف, صحیح عدد_سطریں);

    ساکن خالی guiSendMouseEvent(صحیح button, صحیح x, صحیح y, صحیح repeated_click, int_u modifiers);
    ساکن خالی guiMouseMoved(صحیح x, صحیح y);
    ساکن خالی guiBlankMouse(منطقی should_hide);
    ساکن int_u mouse_Modifiers_مقصد_Vim(int32 beModifiers);

    int32 mouse_Drag_Event_گنتی;

#اگر_تعریف_شدہ FEAT_MBYTE_IME
    خالی DrawIMString(خالی);
#اگر_ختم

    نجی:
    خالی init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#اگر_تعریف_شدہ FEAT_MBYTE_IME
    ساخت {
	BMessenger* messenger;
	BMessage* پیغام_2;
	BPoint location;
	صحیح صف;
	صحیح ستون;
	صحیح گنتی;
    } IMData;
#اگر_ختم
};

جماعت VimScrollBar: عوامی BScrollBar
{
    قسمتعریف BScrollBar Inherited;
    عوامی:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    مجازی خالی قدر_بدلا_2(اعشاری new_قدر);
    مجازی خالی Mouse_اوپر(BPoint where);
    خالی سیٹ_قدر(اعشاری newval);
    scrollbar_T *getGsb()
    { واپس gsb; }

    int32	scroll_Event_گنتی;

    نجی:
    scrollbar_T *gsb;
    اعشاری   ignore_قدر;
};


#اگر_تعریف_شدہ FEAT_TOOLBAR

جماعت VimToolbar : عوامی BBox
{
    ساکن BBitmap *معمول_Buttons_Bitmap;
    ساکن BBitmap *grayedButtonsBitmap;

    BBitmap *بھرو_Vim_Bitmap(ثابت حرف* fileName);
    منطقی GetPictureFromBitmap(BPicture *picture_مقصد, int32 اشاریہ, BBitmap *bitmapFrom, منطقی pressed);
    منطقی Modify_Bitmap_مقصد_Grayed(BBitmap *bitmap);

    BList fButtonsList;
    خالی InvalidateLayout();

    عوامی:
    VimToolbar(BRect چوکھٹا, ثابت حرف * name);
    ~VimToolbar();

    منطقی PrepareButtonBitmaps();

    منطقی جوڑو_Button(int32 اشاریہ, vimmenu_T *menu);
    منطقی RemoveButton(vimmenu_T *menu);
    منطقی سرمئی_Button(vimmenu_T *menu, صحیح grey);

    اعشاری Toolbar_اونچائی() ثابت;
    مجازی خالی Attached_مقصد_کھڑکی();
};

BBitmap *VimToolbar::معمول_Buttons_Bitmap  = تہی;
BBitmap *VimToolbar::grayedButtonsBitmap  = تہی;

ثابت اعشاری ToolbarMargin = 3.;
ثابت اعشاری ButtonMargin  = 3.;

#اگر_ختم //FEAT_TOOLBAR

#اگر_تعریف_شدہ FEAT_GUI_TABLINE

جماعت Vim_Tab_سطر : عوامی BTabView
{
    عوامی:
	جماعت VimTab : عوامی BTab {
	    عوامی:
		VimTab() : BTab(نیا BView(BRect(), "-Empty-", 0, 0)) {}

	    مجازی خالی Select(BView* owner);
	};

	Vim_Tab_سطر(BRect r) : BTabView(r, "vimTabLine", B_چوڑائی_FROM_LABEL,
	       B_FOLLOW_بایاں | B_FOLLOW_چوٹی | B_FOLLOW_دایاں, B_WILL_DRAW | B_چوکھٹا_EVENTS) {}

    اعشاری Tabline_اونچائی() ثابت;
    مجازی خالی Mouse_نیچے_کو(BPoint نقطہ);
};

#اگر_ختم //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
جماعت VimFont: عوامی BFont
{
    قسمتعریف BFont Inherited;
    عوامی:
    VimFont();
    VimFont(ثابت VimFont *rhs);
    VimFont(ثابت BFont *rhs);
    VimFont(ثابت VimFont &rhs);
    ~VimFont();

    VimFont *اگلا;
    صحیح refcount;
    char_u *name;

    نجی:
    خالی init();
};

#اگر defined(FEAT_GUI_DIALOG)

جماعت VimDialog : عوامی BWindow
{
    قسمتعریف BWindow Inherited;

    BButton* _CreateButton(int32 which, ثابت حرف* label);

    عوامی:

    جماعت View : عوامی BView {
	قسمتعریف BView Inherited;

	عوامی:
	View(BRect چوکھٹا);
	~View();

	مجازی خالی Draw(BRect updateRect);
	خالی InitIcon(int32 type);

	نجی:
	BBitmap*    fIconBitmap;
    };

    VimDialog(صحیح type, ثابت حرف *title, ثابت حرف *پیغام_2,
	    ثابت حرف *buttons, صحیح dfltbutton, ثابت حرف *textfield,
	    صحیح ex_cmd);
    ~VimDialog();

    صحیح Go();

    مجازی خالی پیغام_Received(BMessage *پیغام);

    نجی:
    sem_id	    fDialogSem;
    صحیح		    f_Dialog_قدر;
    BList	    fButtonsList;
    BTextView*	    f_پیغام_View;
    BTextControl*   f_داخلہ_Control;
    ثابت حرف*	    f_داخلہ_قدر;
};

جماعت VimSelectFontDialog : عوامی BWindow
{
    قسمتعریف BWindow Inherited;

    خالی _CleanList(BListView* list);
    خالی _UpdateFontStyles();
    خالی Update_سائز_داخلہ_Preview();
    خالی _UpdateFontPreview();
    منطقی Update_From_List_شے(BListView* list, حرف* متن, صحیح متن_سائز);
    عوامی:

    VimSelectFontDialog(font_family* family, font_style* style, اعشاری* سائز);
    ~VimSelectFontDialog();

    منطقی Go();

    مجازی خالی پیغام_Received(BMessage *پیغام);

    نجی:
    حالت_t	    f_حالت;
    sem_id	    fDialogSem;
    منطقی	    f_Dialog_قدر;
    font_family*    fFamily;
    font_style*	    fStyle;
    اعشاری*	    f_سائز;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    اعشاری	    f_Font_سائز;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_داخلہ;
};

#اگر_ختم // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

ساخت Main_دلائل {
    صحیح	     argc;
    حرف    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#تعریف_کرو	کلید_پیغام_BUFSIZ	7
#اگر کلید_پیغام_BUFSIZ < MAX_کلید_رمز_LEN
#غلطی Increase کلید_پیغام_BUFSIZ!
#اگر_ختم

ساخت Vim_کلید_پیغام {
    char_u  لمبائی;
    char_u  chars[کلید_پیغام_BUFSIZ];  // contains Vim encoding
    منطقی    csi_escape;
};

ساخت Vim_Resize_پیغام {
    صحیح	    چوڑائی;
    صحیح	    اونچائی;
};

ساخت Vim_Scroll_Bar_پیغام {
    VimScrollBar *sb;
    طویل    قدر;
    صحیح	    stillDragging;
};

ساخت Vim_Menu_پیغام {
    vimmenu_T	*guiMenu;
};

ساخت Vim_Mouse_پیغام {
    صحیح	    button;
    صحیح	    x;
    صحیح	    y;
    صحیح	    repeated_click;
    int_u   modifiers;
};

ساخت Vim_Mouse_Moved_پیغام {
    صحیح	    x;
    صحیح	    y;
};

ساخت Vim_Focus_پیغام {
    منطقی    فعال;
};

ساخت Vim_Refs_پیغام {
    BMessage   *پیغام_2;
    منطقی    changedir;
};

ساخت Vim_Tabline_پیغام {
    صحیح	    اشاریہ;
};

ساخت Vim_Tabline_Menu_پیغام {
    صحیح	    اشاریہ;
    صحیح	    event;
};

ساخت Vim_پیغام {
    شمار Vim_پیغام_Type {
	کلید_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    اتحاد {
	ساخت Vim_کلید_پیغام    کلید_2;
	ساخت Vim_Resize_پیغام New_سائز;
	ساخت Vim_Scroll_Bar_پیغام	Scroll;
	ساخت Vim_Menu_پیغام   Menu;
	ساخت Vim_Mouse_پیغام  Mouse;
	ساخت Vim_Mouse_Moved_پیغام	MouseMoved;
	ساخت Vim_Focus_پیغام  Focus;
	ساخت Vim_Refs_پیغام   Refs;
	ساخت Vim_Tabline_پیغام	Tabline;
	ساخت Vim_Tabline_Menu_پیغام    TablineMenu;
    } u;
};

#تعریف_کرو RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#تعریف_کرو GUI_مقصد_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

ساکن ساخت specialkey
{
    uint16  Be_کلیدیں;
#تعریف_کرو کلید_3(a,b)    ((a)<<8|(b))
#تعریف_کرو K(a)	    کلید_3(0,a)		// for ASCII codes
#تعریف_کرو F(b)	    کلید_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_کلیدیں[] =
{
    {K(B_اوپر_ARROW),	'k', 'u'},
    {K(B_نیچے_کو_ARROW),	    'k', 'd'},
    {K(B_بایاں_ARROW),	    'k', 'l'},
    {K(B_دایاں_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_داخل_کرو),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_انتہا),		'@', '7'},
    {K(B_صفحہ_اوپر),	'k', 'P'},	// XK_Prior
    {K(B_صفحہ_نیچے_کو),	    'k', 'N'},	    // XK_Next,

#تعریف_کرو پہلا_FUNCTION_کلید  11
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

#اگر 0
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
#اگر_ختم

    // End of list marker:
    {0,		    0, 0}
};

#تعریف_کرو عدد_SPECIAL_کلیدیں    ARRAY_لمبائی(special_کلیدیں)

// ---------------- VimApp ----------------

    ساکن خالی
docd(BPath &راستہ)
{
    mch_chdir((حرف *)راستہ.راستہ_2());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	ساکن خالی
drop_callback(خالی *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	ساکن خالی
RefsReceived(BMessage *m, منطقی changedir)
{
    uint32 type;
    int32 گنتی;

    m->Print_مقصد_دھارا();
    انتخاب (m->what) {
	حالت B_REFS_RECEIVED:
	حالت B_SIMPLE_ڈیٹا:
	    m->GetInfo("refs", &type, &گنتی);
	    اگر (type != B_REF_TYPE)
		جاؤ bad;
	    توڑو;
	حالت B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &گنتی);
	    اگر (type != B_STRING_TYPE)
		جاؤ bad;
	    اگر (changedir) {
		حرف *dirname;
		اگر (m->ڈھونڈو_String("cwd", (ثابت حرف **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    توڑو;
	طےشدہ:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    حذف m;
	    واپس;
    }

#اگر_تعریف_شدہ FEAT_VISUAL
    reset_VIsual();
#اگر_ختم

    char_u  **fnames;
    fnames = (char_u **) alloc(گنتی * حجم(char_u *));
    صحیح fname_اشاریہ = 0;

    انتخاب (m->what) {
	حالت B_REFS_RECEIVED:
	حالت B_SIMPLE_ڈیٹا:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    برائے (صحیح i = 0; i < گنتی; ++i)
	    {
		اندراج_ref ref;
		اگر (m->ڈھونڈو_Ref("refs", i, &ref) == B_OK) {
		    BEntry اندراج_2(&ref, جھوٹ);
		    BPath راستہ;
		    اندراج_2.Get_راستہ(&راستہ);

		    // Change to parent directory?
		    اگر (changedir) {
			BPath parentpath;
			راستہ.Get_والد(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    اگر (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			اگر (!changedir)
			    docd(راستہ);
		    } ورنہ {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)راستہ.راستہ_2(), IObuff);
			اگر (fname == تہی)
			    fname = (char_u *)راستہ.راستہ_2();
			fnames[fname_اشاریہ++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = جھوٹ;
		}
	    }
	    توڑو;
	حالت B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    برائے (صحیح i = 1; i < گنتی; ++i)
	    {
		حرف *fname;

		اگر (m->ڈھونڈو_String("argv", i, (ثابت حرف **) &fname) == B_OK) {
		    fnames[fname_اشاریہ++] = vim_strsave((char_u *)fname);
		}
	    }
	    توڑو;
	طےشدہ:
	    // fprintf(stderr, "case default\n");
	    توڑو;
    }

    حذف m;

    // Handle the drop, :edit to get to the file
    اگر (fname_اشاریہ > 0) {
	handle_drop(fname_اشاریہ, fnames, FALSE, drop_callback, تہی);

	setcursor();
	برآمد_flush();
    } ورنہ {
	vim_free(fnames);
    }
}

VimApp::VimApp(ثابت حرف *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(تہی)
{
}

VimApp::~VimApp()
{
}

    خالی
VimApp::Ready_مقصد_تسلسل()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    خالی
VimApp::ArgvReceived(int32 arg_argc, حرف **arg_argv)
{
    اگر (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	اگر (gui.vim_کھڑکی)
	    gui.vim_کھڑکی->Minimize(جھوٹ);
	BMessage *m = Current_پیغام();
	Detach_Current_پیغام();
	SendRefs(m, سچ);
    }
}

    خالی
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    صحیح حد_بندی = 15;
    جب (--حد_بندی >= 0 && (curbuf == تہی || curbuf->b_آغاز_ffc == 0))
	snooze(100000);    //  0.1 s
    اگر (gui.vim_کھڑکی)
	gui.vim_کھڑکی->Minimize(جھوٹ);
    Detach_Current_پیغام();
    SendRefs(m, سچ);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    خالی
VimApp::SendRefs(BMessage *m, منطقی changedir)
{
    Vim_Refs_پیغام rm;
    rm.پیغام_2 = m;
    rm.changedir = changedir;

    write_پورٹ(gui.vdcmp, Vim_پیغام::Refs, &rm, حجم(rm));
    //	calls ::RefsReceived
}

    خالی
VimApp::پیغام_Received(BMessage *m)
{
    انتخاب (m->what) {
	حالت 'save':
	    {
		اندراج_ref refDirectory;
		m->ڈھونڈو_Ref("directory", &refDirectory);
		f_Browsed_راستہ.سیٹ_مقصد(&refDirectory);
		BString strName;
		m->ڈھونڈو_String("name", &strName);
		f_Browsed_راستہ.Append(strName.String());
	    }
	    توڑو;
	حالت 'open':
	    {
		اندراج_ref ref;
		m->ڈھونڈو_Ref("refs", &ref);
		f_Browsed_راستہ.سیٹ_مقصد(&ref);
	    }
	    توڑو;
	حالت B_CANCEL:
	    {
		BFilePanel *panel;
		m->ڈھونڈو_اشارہ_گر("source", (خالی**)&panel);
		اگر (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    توڑو;
	طےشدہ:
	    Inherited::پیغام_Received(m);
	    توڑو;
    }
}

    منطقی
VimApp::QuitRequested()
{
    (خالی)Inherited::QuitRequested();
    واپس جھوٹ;
}

// ---------------- VimWindow ----------------

Vim_کھڑکی::Vim_کھڑکی():
    BWindow(BRect(40, 40, 150, 150),
	    "Vim",
	    B_TITLED_کھڑکی,
	    0,
	    B_CURRENT_WORKSPACE)

{
    init();
}

Vim_کھڑکی::~Vim_کھڑکی()
{
    اگر (formView) {
	Remove_بچہ(formView);
	حذف formView;
    }
    gui.vim_کھڑکی = تہی;
}

    خالی
Vim_کھڑکی::init()
{
    // Attach the VimFormView
    formView = نیا VimFormView(Bounds());
    اگر (formView != تہی) {
	جوڑو_بچہ(formView);
    }
}

#اگر 0  //  disabled in zeta patch
    خالی
Vim_کھڑکی::Dispatch_پیغام(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    انتخاب (m->what) {
	حالت B_MOUSE_اوپر:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		اگر (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->ڈھونڈو_نقطہ("where", &where);
		    v->Mouse_اوپر(where);
		} ورنہ {
		    Inherited::Dispatch_پیغام(m, h);
		}
	    }
	    توڑو;
	طےشدہ:
	    Inherited::Dispatch_پیغام(m, h);
    }
}
#اگر_ختم

    خالی
Vim_کھڑکی::کھڑکی_Activated(منطقی فعال)
{
    Inherited::کھڑکی_Activated(فعال);
    // the textArea gets the keyboard action
    اگر (فعال && gui.vim_متن_Area)
	gui.vim_متن_Area->MakeFocus(سچ);

    ساخت Vim_Focus_پیغام fm;
    fm.فعال = فعال;

    write_پورٹ(gui.vdcmp, Vim_پیغام::Focus, &fm, حجم(fm));
}

    منطقی
Vim_کھڑکی::QuitRequested()
{
    ساخت Vim_کلید_پیغام km;
    km.لمبائی = 5;
    حافظہ_نقل((حرف *)km.chars, "\033:qa\r", km.لمبائی);
    km.csi_escape = جھوٹ;
    write_پورٹ(gui.vdcmp, Vim_پیغام::کلید_2, &km, حجم(km));
    واپس جھوٹ;
}
    خالی
Vim_کھڑکی::Dispatch_پیغام(BMessage *m, BHandler *h)
{
    منطقی should_propagate = سچ;

    انتخاب (m->what)
    {
	حالت B_کلید_نیچے_کو:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->ڈھونڈو_Int32("raw_char", &scancode);
		m->ڈھونڈو_Int32("modifiers", &beModifiers);

		اگر (scancode == B_ENTER && (beModifiers & B_بایاں_COMMAND_کلید))
		    {
			should_propagate = جھوٹ;
			اگر (یہ->is_fullscreen)
			    {
				یہ->is_fullscreen = جھوٹ;
				Resize_مقصد(یہ->saved_چوکھٹا.چوڑائی_2(), یہ->saved_چوکھٹا.اونچائی_2());
				Move_مقصد(یہ->saved_چوکھٹا.بائیں, یہ->saved_چوکھٹا.چوٹی);
				سیٹ_Look(یہ->saved_look);
				سیٹ_Flags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } ورنہ {
				یہ->saved_چوکھٹا = چوکھٹا_2();
				یہ->saved_look = Look();
				یہ->is_fullscreen = سچ;
				BScreen s(یہ);
				سیٹ_Look(B_NO_BORDER_کھڑکی_LOOK);
				Resize_مقصد(s.چوکھٹا_2().چوڑائی_2() + 1, s.چوکھٹا_2().اونچائی_2() + 1);
				Move_مقصد(s.چوکھٹا_2().بائیں, s.چوکھٹا_2().چوٹی);
				سیٹ_Flags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    اگر (should_propagate)
	Inherited::Dispatch_پیغام(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect چوکھٹا):
    BView(چوکھٹا, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_چوکھٹا_EVENTS),
    menuBar(تہی),
#اگر_تعریف_شدہ FEAT_TOOLBAR
    toolBar(تہی),
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_سطر(تہی),
#اگر_ختم
    متن_Area(تہی)
{
    init(چوکھٹا);
}

VimFormView::~VimFormView()
{
    اگر (menuBar) {
	Remove_بچہ(menuBar);
#اگر_تعریف_شدہ never
	//  deleting the menuBar leads to SEGV on exit
	//  richard@whitequeen.com Jul 99
	حذف menuBar;
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_TOOLBAR
    حذف toolBar;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    حذف tab_سطر;
#اگر_ختم

    اگر (متن_Area) {
	Remove_بچہ(متن_Area);
	حذف متن_Area;
    }
    gui.vimForm = تہی;
}

    خالی
VimFormView::init(BRect چوکھٹا)
{
    menuBar = نیا BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    جوڑو_بچہ(menuBar);

#اگر_تعریف_شدہ FEAT_TOOLBAR
    toolBar = نیا VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    جوڑو_بچہ(toolBar);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    tab_سطر = نیا Vim_Tab_سطر(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    جوڑو_بچہ(tab_سطر);
#اگر_ختم

    BRect بقایا = چوکھٹا;
    متن_Area = نیا Vim_متن_Area_View(بقایا);
    جوڑو_بچہ(متن_Area);
    // The textArea will be resized later when menus are added

    gui.vimForm = یہ;
}

#اگر_تعریف_شدہ FEAT_TOOLBAR
    اعشاری
VimFormView::Toolbar_اونچائی() ثابت
{
    واپس toolBar ? toolBar->Toolbar_اونچائی() : 0.;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    اعشاری
VimFormView::Tabline_اونچائی() ثابت
{
    واپس (tab_سطر && Is_Showing_Tab_سطر()) ? tab_سطر->Tabline_اونچائی() : 0.;
}
#اگر_ختم

    خالی
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    اگر (menuBar && متن_Area) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect بقایا = Bounds();

#اگر_تعریف_شدہ FEAT_MENU
	بقایا.چوٹی += Menu_اونچائی();
	menuBar->Resize_مقصد(بقایا.دائیں, بقایا.چوٹی);
	gui.menu_اونچائی = (صحیح) Menu_اونچائی();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TOOLBAR
	toolBar->Move_مقصد(بقایا.بائیں, بقایا.چوٹی);
	toolBar->Resize_مقصد(بقایا.دائیں, Toolbar_اونچائی());
	بقایا.چوٹی += Toolbar_اونچائی();
	gui.toolbar_اونچائی = Toolbar_اونچائی();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
	tab_سطر->Move_مقصد(بقایا.بائیں, بقایا.چوٹی);
	tab_سطر->Resize_مقصد(بقایا.دائیں + 1, Tabline_اونچائی());
	بقایا.چوٹی += Tabline_اونچائی();
	gui.tabline_اونچائی = Tabline_اونچائی();
#اگر_ختم

	متن_Area->Resize_مقصد(بقایا.چوڑائی_2(), بقایا.اونچائی_2());
	متن_Area->Move_مقصد(بقایا.بائیں, بقایا.چوٹی);
    }


    Inherited::AllAttached();
}

    خالی
VimFormView::چوکھٹا_Resized(اعشاری new_چوڑائی, اعشاری new_اونچائی)
{
    ساخت Vim_Resize_پیغام sm;
    صحیح adjust_h, adjust_w;

    new_چوڑائی += 1;	//  adjust from width to number of pixels occupied
    new_اونچائی += 1;

    sm.چوڑائی = (صحیح) new_چوڑائی;
    sm.اونچائی = (صحیح) new_اونچائی;
    adjust_w = ((صحیح)new_چوڑائی - gui_get_بنیاد_چوڑائی()) % gui.char_چوڑائی;
    adjust_h = ((صحیح)new_اونچائی - gui_get_بنیاد_اونچائی()) % gui.char_اونچائی;

    اگر (adjust_w > 0 || adjust_h > 0) {
	sm.چوڑائی  -= adjust_w;
	sm.اونچائی -= adjust_h;
    }

    write_پورٹ(gui.vdcmp, Vim_پیغام::Resize, &sm, حجم(sm));
    //	calls gui_resize_shell(new_width, new_height);

    واپس;

    /*
     * The area below the vertical scrollbar is erased to the colour
     * set with SetViewColor() automatically, because we had set
     * B_WILL_DRAW. Resizing the window tight around the vertical
     * scroll bar also helps to avoid debris.
     */
}

// ---------------- VimTextAreaView ----------------

Vim_متن_Area_View::Vim_متن_Area_View(BRect چوکھٹا):
    BView(چوکھٹا, "VimTextAreaView", B_FOLLOW_ALL_SIDES,
#اگر_تعریف_شدہ FEAT_MBYTE_IME
	B_WILL_DRAW | B_بھرا_UPDATE_ON_RESIZE | B_داخلہ_طریقہ_AWARE
#ورنہ
	B_WILL_DRAW | B_بھرا_UPDATE_ON_RESIZE
#اگر_ختم
	),
    mouse_Drag_Event_گنتی(0)
{
#اگر_تعریف_شدہ FEAT_MBYTE_IME
    IMData.messenger = تہی;
    IMData.پیغام_2 = تہی;
#اگر_ختم
    init(چوکھٹا);
}

Vim_متن_Area_View::~Vim_متن_Area_View()
{
    gui.vim_متن_Area = تہی;
}

    خالی
Vim_متن_Area_View::init(BRect چوکھٹا)
{
    // set up global var for fast access
    gui.vim_متن_Area = یہ;

    /*
     * Tell the app server not to erase the view: we will
     * fill it in completely by ourselves.
     * (Does this really work? Even if not, it won't harm either.)
     */
    سیٹ_View_Color(B_TRANSPARENT_32_ثنائی_ہندسہ);
#تعریف_کرو PEN_چوڑائی   1
    سیٹ_Pen_سائز(PEN_چوڑائی);
#تعریف_کرو W_چوڑائی(curwin)   0
}

    خالی
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
    rgb_color rgb = GUI_مقصد_RGB(gui.عقب_pixel);
    سیٹ_کم_Color(rgb);
    FillRect(updateRect, B_SOLID_کم);
    gui_redraw((صحیح) updateRect.بائیں, (صحیح) updateRect.چوٹی,
	    (صحیح) (updateRect.چوڑائی_2() + PEN_چوڑائی), (صحیح) (updateRect.اونچائی_2() + PEN_چوڑائی));

    // Clear the border areas if needed
    سیٹ_کم_Color(rgb);

    اگر (updateRect.بائیں < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.بائیں, updateRect.چوٹی,
		    FILL_X(0)-PEN_چوڑائی, updateRect.bottom), B_SOLID_کم);
    اگر (updateRect.چوٹی < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.بائیں, updateRect.چوٹی,
		    updateRect.دائیں, FILL_Y(0)-PEN_چوڑائی), B_SOLID_کم);
    اگر (updateRect.دائیں >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((صحیح)Columns), updateRect.چوٹی,
		    updateRect.دائیں, updateRect.bottom), B_SOLID_کم);
    اگر (updateRect.bottom >= FILL_Y(صفیں))   //  bottom border
	FillRect(BRect(updateRect.بائیں, FILL_Y((صحیح)صفیں),
		    updateRect.دائیں, updateRect.bottom), B_SOLID_کم);

#اگر_تعریف_شدہ FEAT_MBYTE_IME
    DrawIMString();
#اگر_ختم
}

    خالی
Vim_متن_Area_View::کلید_نیچے_کو(ثابت حرف *بائٹیں, int32 عدد_بائٹیں_2)
{
    ساخت Vim_کلید_پیغام km;
    char_u *dest = km.chars;

    منطقی canHaveVimModifiers = جھوٹ;

    BMessage *پیغام = کھڑکی()->Current_پیغام();
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
    اگر (عدد_بائٹیں_2 > 1) {
	// This cannot be a special key
	اگر (عدد_بائٹیں_2 > کلید_پیغام_BUFSIZ)
	    عدد_بائٹیں_2 = کلید_پیغام_BUFSIZ;	    //	should never happen... ???
	km.لمبائی = عدد_بائٹیں_2;
	حافظہ_نقل((حرف *)dest, بائٹیں, عدد_بائٹیں_2);
	km.csi_escape = سچ;
    } ورنہ {
	int32 scancode = 0;
	پیغام->ڈھونڈو_Int32("key", &scancode);

	int32 beModifiers = 0;
	پیغام->ڈھونڈو_Int32("modifiers", &beModifiers);

	char_u string[3];
	صحیح len = 0;
	km.لمبائی = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	أكد(B_BACKSPACE <= 0x20);
	أكد(B_DELETE == 0x7F);
	اگر (((char_u)بائٹیں[0] <= 0x20 || (char_u)بائٹیں[0] == 0x7F) &&
		عدد_بائٹیں_2 == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    اگر (beModifiers & B_CONTROL_کلید) {
		صحیح اشاریہ = کلید_Map->معمول_map[scancode];
		صحیح new_عدد_بائٹیں = کلید_Map_Chars[اشاریہ];
		char_u *new_بائٹیں = (char_u *)&کلید_Map_Chars[اشاریہ + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		اگر (new_عدد_بائٹیں != 1 || (*new_بائٹیں > 0x20 &&
			    *new_بائٹیں != 0x7F )) {
		    جاؤ notspecial;
		}
		بائٹیں = (حرف *)new_بائٹیں;
	    }
	    canHaveVimModifiers = سچ;

	    uint16 beoskey;
	    صحیح پہلا, آخری;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    اگر (عدد_بائٹیں_2 == 0 || بائٹیں[0] == B_FUNCTION_کلید) {
		beoskey = F(scancode);
		پہلا = پہلا_FUNCTION_کلید;
		آخری = عدد_SPECIAL_کلیدیں;
	    } ورنہ اگر (*بائٹیں == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		پہلا = آخری = 0;
	    } ورنہ {
		beoskey = K(بائٹیں[0]);
		پہلا = 0;
		آخری = پہلا_FUNCTION_کلید;
	    }

	    برائے (صحیح i = پہلا; i < آخری; i++) {
		اگر (special_کلیدیں[i].Be_کلیدیں == beoskey) {
		    string[0] = CSI;
		    string[1] = special_کلیدیں[i].vim_code0;
		    string[2] = special_کلیدیں[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	اگر (len == 0) {
	    string[0] = بائٹیں[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#اگر 0
	اگر (len == 3 ||
		بائٹیں[0] == B_SPACE || بائٹیں[0] == B_TAB ||
		بائٹیں[0] == B_RETURN || بائٹیں[0] == '\r' ||
		بائٹیں[0] == B_ESCAPE)
#ورنہ
	    اگر (canHaveVimModifiers)
#اگر_ختم
	    {
		صحیح modifiers;
		modifiers = 0;
		اگر (beModifiers & B_سرکاؤ_کلید)
		    modifiers |= MOD_نقاب_سرکاؤ;
		اگر (beModifiers & B_CONTROL_کلید)
		    modifiers |= MOD_نقاب_CTRL;
		اگر (beModifiers & B_OPTION_کلید)
		    modifiers |= MOD_نقاب_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		صحیح کلید;
		اگر (string[0] == CSI && len == 3)
		    کلید = مقصد_SPECIAL(string[1], string[2]);
		ورنہ
		    کلید = string[0];
		کلید = simplify_کلید(کلید, &modifiers);
		اگر (IS_SPECIAL(کلید))
		{
		    string[0] = CSI;
		    string[1] = K_دوسرا(کلید);
		    string[2] = K_تیسرا(کلید);
		    len = 3;
		}
		ورنہ
		{
		    string[0] = کلید;
		    len = 1;
		}

		اگر (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.لمبائی = 3;
		}
	    }
	حافظہ_نقل((حرف *)dest, string, len);
	km.لمبائی += len;
	km.csi_escape = جھوٹ;
    }

    write_پورٹ(gui.vdcmp, Vim_پیغام::کلید_2, &km, حجم(km));

    /*
     * blank out the pointer if necessary
     */
    اگر (p_mh && !gui.اشارہ_گر_hidden)
    {
	guiBlankMouse(سچ);
	gui.اشارہ_گر_hidden = TRUE;
    }
}
خالی
Vim_متن_Area_View::guiSendMouseEvent(
	صحیح	button,
	صحیح	x,
	صحیح	y,
	صحیح	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_پیغام mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_پورٹ(gui.vdcmp, Vim_پیغام::Mouse, &mm, حجم(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    اگر (gui.اشارہ_گر_hidden)
    {
	guiBlankMouse(جھوٹ);
	gui.اشارہ_گر_hidden = FALSE;
    }
}

خالی
Vim_متن_Area_View::guiMouseMoved(
	صحیح	x,
	صحیح	y)
{
    Vim_Mouse_Moved_پیغام mm;

    mm.x = x;
    mm.y = y;

    write_پورٹ(gui.vdcmp, Vim_پیغام::MouseMoved, &mm, حجم(mm));

    اگر (gui.اشارہ_گر_hidden)
    {
	guiBlankMouse(جھوٹ);
	gui.اشارہ_گر_hidden = FALSE;
    }
}

    خالی
Vim_متن_Area_View::guiBlankMouse(منطقی should_hide)
{
    اگر (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } ورنہ {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
Vim_متن_Area_View::mouse_Modifiers_مقصد_Vim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    اگر (beModifiers & B_سرکاؤ_کلید)
	vim_modifiers |= MOUSE_سرکاؤ;
    اگر (beModifiers & B_CONTROL_کلید)
	vim_modifiers |= MOUSE_CTRL;
    اگر (beModifiers & B_OPTION_کلید)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    واپس vim_modifiers;
}

    خالی
Vim_متن_Area_View::Mouse_نیچے_کو(BPoint نقطہ)
{
    BMessage *m = کھڑکی()->Current_پیغام();
    أكد(m);

    int32 buttons = 0;
    m->ڈھونڈو_Int32("buttons", &buttons);

    صحیح vimButton;

    اگر (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_بایاں;
    ورنہ اگر (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_دایاں;
    ورنہ اگر (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_درمیان;
    ورنہ
	واپس;		// Unknown button

    vimMouseButton = 1;	    // don't care which one

    // Handle multiple clicks
    int32 clicks = 0;
    m->ڈھونڈو_Int32("clicks", &clicks);

    int32 modifiers = 0;
    m->ڈھونڈو_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_مقصد_Vim(modifiers);

    guiSendMouseEvent(vimButton, نقطہ.x, نقطہ.y,
	    clicks > 1 /* = repeated_click*/, vimMouseModifiers);
}

    خالی
Vim_متن_Area_View::Mouse_اوپر(BPoint نقطہ)
{
    vimMouseButton = 0;

    BMessage *m = کھڑکی()->Current_پیغام();
    أكد(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->ڈھونڈو_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_مقصد_Vim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, نقطہ.x, نقطہ.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::Mouse_اوپر(نقطہ);
}

    خالی
Vim_متن_Area_View::MouseMoved(BPoint نقطہ, uint32 transit, ثابت BMessage *پیغام_2)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    اگر (gui.اشارہ_گر_hidden)
    {
	guiBlankMouse(جھوٹ);
	gui.اشارہ_گر_hidden = FALSE;
    }

    اگر (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(نقطہ.x, نقطہ.y);
	واپس;
    }

    atomic_جوڑو(&mouse_Drag_Event_گنتی, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, نقطہ.x, نقطہ.y, 0, vimMouseModifiers);
}

    خالی
Vim_متن_Area_View::پیغام_Received(BMessage *m)
{
    انتخاب (m->what) {
	حالت 'menu':
	    {
		Vim_Menu_پیغام mm;
		mm.guiMenu = تہی;  // in case no pointer in msg
		m->ڈھونڈو_اشارہ_گر("VimMenu", (خالی **)&mm.guiMenu);
		write_پورٹ(gui.vdcmp, Vim_پیغام::Menu, &mm, حجم(mm));
	    }
	    توڑو;
	حالت B_MOUSE_WHEEL_بدلا:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		اعشاری small=0, big=0, dy=0;
		m->ڈھونڈو_Float("be:wheel_delta_y", &dy);
		scb->Get_اقدام(&small, &big);
		scb->سیٹ_قدر(scb->قدر_2()+small*dy*3);
		scb->قدر_بدلا_2(scb->قدر_2());
#اگر 0
		scb = curwin->w_scrollbars[0].id;
		scb->Get_اقدام(&small, &big);
		scb->سیٹ_قدر(scb->قدر_2()+small*dy);
		scb->قدر_بدلا_2(scb->قدر_2());
#اگر_ختم
	    }
	    توڑو;
#اگر_تعریف_شدہ FEAT_MBYTE_IME
	حالت B_داخلہ_طریقہ_EVENT:
	    {
		int32 opcode;
		m->ڈھونڈو_Int32("be:opcode", &opcode);
		انتخاب(opcode)
		{
		    حالت B_داخلہ_طریقہ_STARTED:
			اگر (!IMData.messenger) حذف IMData.messenger;
			IMData.messenger = نیا BMessenger();
			m->ڈھونڈو_Messenger("be:reply_to", IMData.messenger);
			توڑو;
		    حالت B_داخلہ_طریقہ_بدلا:
			{
			    BString str;
			    منطقی confirmed;
			    اگر (IMData.پیغام_2) *(IMData.پیغام_2) = *m;
			    ورنہ	       IMData.پیغام_2 = نیا BMessage(*m);
			    DrawIMString();
			    m->ڈھونڈو_Bool("be:confirmed", &confirmed);
			    اگر (confirmed)
			    {
				m->ڈھونڈو_String("be:string", &str);
				char_u *chars = (char_u*)str.String();
				ساخت Vim_کلید_پیغام km;
				km.csi_escape = سچ;
				صحیح clen;
				صحیح i = 0;
				جب (i < str.لمبائی_2())
				{
				    clen = utf_ptr2len(chars+i);
				    حافظہ_نقل(km.chars, chars+i, clen);
				    km.لمبائی = clen;
				    write_پورٹ(gui.vdcmp, Vim_پیغام::کلید_2, &km, حجم(km));
				    i += clen;
				}
			    }
			}
			توڑو;
		    حالت B_داخلہ_طریقہ_LOCATION_REQUEST:
			{
			    BMessage پیغام(B_داخلہ_طریقہ_EVENT);
			    پیغام.جوڑو_Int32("be:opcode", B_داخلہ_طریقہ_LOCATION_REQUEST);
			    پیغام.جوڑو_نقطہ("be:location_reply", IMData.location);
			    پیغام.جوڑو_Float("be:height_reply", FILL_Y(1));
			    IMData.messenger->Send_پیغام(&پیغام);
			}
			توڑو;
		    حالت B_داخلہ_طریقہ_STOPPED:
			حذف IMData.messenger;
			حذف IMData.پیغام_2;
			IMData.messenger = تہی;
			IMData.پیغام_2 = تہی;
			توڑو;
		}
	    }
	    // TODO: sz: break here???
#اگر_ختم
	طےشدہ:
	    اگر (m->WasDropped()) {
		BWindow *w = کھڑکی();
		w->Detach_Current_پیغام();
		w->Minimize(جھوٹ);
		VimApp::SendRefs(m, (modifiers() & B_سرکاؤ_کلید) != 0);
	    } ورنہ {
		Inherited::پیغام_Received(m);
	    }
	    توڑو;
    }
}

    صحیح
Vim_متن_Area_View::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    اگر (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_سیٹ_font((GuiFont)newFont);
	اگر (name && STRCMP(name, "*") != 0)
	    hl_سیٹ_font_name(name);

	سیٹ_Drawing_منوال(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	واپس OK;
    }
    واپس FAIL;
}

    خالی
Vim_متن_Area_View::mchDrawString(صحیح صف, صحیح ستون, char_u *s, صحیح len, صحیح flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    اگر (!(flags & DRAW_TRANSP)) {
	صحیح cells;
	cells = 0;
	برائے (صحیح i=0; i<len; i++) {
	    صحیح cn = utf_ptr2cells((char_u *)(s+i));
	    اگر (cn<4) cells += cn;
	}

	BRect r(FILL_X(ستون), FILL_Y(صف),
		FILL_X(ستون + cells) - PEN_چوڑائی, FILL_Y(صف + 1) - PEN_چوڑائی);
	FillRect(r, B_SOLID_کم);
    }

    BFont font;
    یہ->GetFont(&font);
    اگر (!font.IsFixed())
    {
	حرف* p = (حرف*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	صحیح cells;
	جب ((p - (حرف*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.سیٹ(متن_X(ستون+lastpos), متن_Y(صف));
	    DrawString(p, clen, where);
	    اگر (flags & DRAW_BOLD) {
		where.x += 1.0;
		سیٹ_Drawing_منوال(B_OP_BLEND);
		DrawString(p, clen, where);
		سیٹ_Drawing_منوال(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    اگر (cells<4) lastpos += cells;
	    ورنہ	lastpos++;
	    p += clen;
	}
    }
    ورنہ
    {
	BPoint where(متن_X(ستون), متن_Y(صف));
	DrawString((حرف*)s, len, where);
	اگر (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    سیٹ_Drawing_منوال(B_OP_BLEND);
	    DrawString((حرف*)s, len, where);
	    سیٹ_Drawing_منوال(B_OP_COPY);
	}
    }

    اگر (flags & DRAW_UNDERL) {
	صحیح cells;
	cells = 0;
	برائے (صحیح i=0; i<len; i++) {
	    صحیح cn = utf_ptr2cells((char_u *)(s+i));
	    اگر (cn<4) cells += cn;
	}

	BPoint آغاز(FILL_X(ستون), FILL_Y(صف + 1) - PEN_چوڑائی);
	BPoint انتہا(FILL_X(ستون + cells) - PEN_چوڑائی, آغاز.y);

	Stroke_سطر(آغاز, انتہا);
    }
}

خالی
Vim_متن_Area_View::mch_Clear_بلاک(
	صحیح	row1,
	صحیح	col1,
	صحیح	row2,
	صحیح	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_چوڑائی, FILL_Y(row2 + 1) - PEN_چوڑائی);
    gui_mch_سیٹ_bg_color(gui.عقب_pixel);
    FillRect(r, B_SOLID_کم);
}

    خالی
Vim_متن_Area_View::mchClearAll()
{
    gui_mch_سیٹ_bg_color(gui.عقب_pixel);
    FillRect(Bounds(), B_SOLID_کم);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    خالی
Vim_متن_Area_View::mch_Delete_سطریں(صحیح صف, صحیح عدد_سطریں)
{
    BRect ماخذ, dest;
    ماخذ.بائیں = FILL_X(gui.scroll_region_بایاں);
    ماخذ.چوٹی = FILL_Y(صف + عدد_سطریں);
    ماخذ.دائیں = FILL_X(gui.scroll_region_دایاں + 1) - PEN_چوڑائی;
    ماخذ.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_چوڑائی;

    dest.بائیں = FILL_X(gui.scroll_region_بایاں);
    dest.چوٹی = FILL_Y(صف);
    dest.دائیں = FILL_X(gui.scroll_region_دایاں + 1) - PEN_چوڑائی;
    dest.bottom = FILL_Y(gui.scroll_region_bot - عدد_سطریں + 1) - PEN_چوڑائی;

    اگر (gui.vim_کھڑکی->تالا_2()) {
	// Clear one column more for when bold has spilled over
	Copy_بٹ(ماخذ, dest);
	gui_clear_بلاک(gui.scroll_region_bot - عدد_سطریں + 1,
		gui.scroll_region_بایاں,
		gui.scroll_region_bot, gui.scroll_region_دایاں);


	gui.vim_کھڑکی->Unlock();
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
    خالی
Vim_متن_Area_View::mch_داخل_کرو_سطریں(صحیح صف, صحیح عدد_سطریں)
{
    BRect ماخذ, dest;

    // XXX Attempt at a hack:
    gui.vim_کھڑکی->UpdateIfNeeded();
    ماخذ.بائیں = FILL_X(gui.scroll_region_بایاں);
    ماخذ.چوٹی = FILL_Y(صف);
    ماخذ.دائیں = FILL_X(gui.scroll_region_دایاں + 1) - PEN_چوڑائی;
    ماخذ.bottom = FILL_Y(gui.scroll_region_bot - عدد_سطریں + 1) - PEN_چوڑائی;

    dest.بائیں = FILL_X(gui.scroll_region_بایاں);
    dest.چوٹی = FILL_Y(صف + عدد_سطریں);
    dest.دائیں = FILL_X(gui.scroll_region_دایاں + 1) - PEN_چوڑائی;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_چوڑائی;

    اگر (gui.vim_کھڑکی->تالا_2()) {
	// Clear one column more for when bold has spilled over
	Copy_بٹ(ماخذ, dest);
	gui_clear_بلاک(صف, gui.scroll_region_بایاں,
		صف + عدد_سطریں - 1, gui.scroll_region_دایاں);

	gui.vim_کھڑکی->Unlock();
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

#اگر_تعریف_شدہ FEAT_MBYTE_IME
/*
 * DrawIMString draws string with IMData.message.
 */
خالی Vim_متن_Area_View::DrawIMString(خالی)
{
    ساکن ثابت rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    ثابت حرف* s;
    صحیح len;
    BMessage* پیغام = IMData.پیغام_2;
    اگر (!پیغام)
	واپس;
    gui_redraw_بلاک(IMData.صف, 0,
	    IMData.صف + IMData.گنتی, W_چوڑائی(curwin), GUI_MON_NOCLEAR);
    منطقی confirmed = جھوٹ;
    پیغام->ڈھونڈو_Bool("be:confirmed", &confirmed);
    اگر (confirmed)
	واپس;
    rgb_color hcolor = بلند_Color(), lcolor = کم_Color();
    پیغام->ڈھونڈو_String("be:string", &str);
    s = str.String();
    len = str.لمبائی_2();
    سیٹ_بلند_Color(0, 0, 0);
    IMData.صف = gui.صف;
    IMData.ستون = gui.ستون;
    int32 sel_آغاز = 0, sel_انتہا = 0;
    پیغام->ڈھونڈو_Int32("be:selection", 0, &sel_آغاز);
    پیغام->ڈھونڈو_Int32("be:selection", 1, &sel_انتہا);
    صحیح clen, cn;
    BPoint pos(IMData.ستون, 0);
    BRect r;
    BPoint where;
    IMData.location = Convert_مقصد_Screen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.صف + pos.y)));
    برائے (صحیح i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	اگر (pos.x + cn > W_چوڑائی(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	اگر (sel_آغاز<=i && i<sel_انتہا)
	{
	    سیٹ_کم_Color(r_highlight);
	    IMData.location = Convert_مقصد_Screen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.صف + pos.y)));
	}
	ورنہ
	{
	    سیٹ_کم_Color(b_highlight);
	}
	r.سیٹ(FILL_X(pos.x), FILL_Y(IMData.صف + pos.y),
		FILL_X(pos.x + cn) - PEN_چوڑائی,
		FILL_Y(IMData.صف + pos.y + 1) - PEN_چوڑائی);
	FillRect(r, B_SOLID_کم);
	where.سیٹ(متن_X(pos.x), متن_Y(IMData.صف + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.گنتی = (صحیح)pos.y;

    سیٹ_بلند_Color(hcolor);
    سیٹ_کم_Color(lcolor);
}
#اگر_ختم
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
	    "vim scrollbar", (BView *)تہی,
	    0.0, 10.0, posture),
    ignore_قدر(-1),
    scroll_Event_گنتی(0)
{
    gsb = g;
    سیٹ_Resizing_منوال(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    خالی
VimScrollBar::قدر_بدلا_2(اعشاری new_قدر)
{
    اگر (ignore_قدر >= 0.0 && new_قدر == ignore_قدر) {
	ignore_قدر = -1;
	واپس;
    }
    ignore_قدر = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_جوڑو(&scroll_Event_گنتی, 1);

    ساخت Vim_Scroll_Bar_پیغام sm;

    sm.sb = یہ;
    sm.قدر = (طویل) new_قدر;
    sm.stillDragging = TRUE;

    write_پورٹ(gui.vdcmp, Vim_پیغام::ScrollBar, &sm, حجم(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    خالی
VimScrollBar::Mouse_اوپر(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_جوڑو(&scroll_Event_گنتی, 1);

    ساخت Vim_Scroll_Bar_پیغام sm;

    sm.sb = یہ;
    sm.قدر = (طویل) قدر_2();
    sm.stillDragging = FALSE;

    write_پورٹ(gui.vdcmp, Vim_پیغام::ScrollBar, &sm, حجم(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::Mouse_اوپر(where);
}

    خالی
VimScrollBar::سیٹ_قدر(اعشاری new_قدر)
{
    اگر (new_قدر == قدر_2())
	واپس;

    ignore_قدر = new_قدر;
    Inherited::سیٹ_قدر(new_قدر);
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

    خالی
VimFont::init()
{
    اگلا = تہی;
    refcount = 1;
    name = تہی;
}

// ---------------- VimDialog ----------------

#اگر defined(FEAT_GUI_DIALOG)

ثابت غیرعلامتی صحیح  k_Vim_Dialog_Button_پیغام = 'VMDB';
ثابت غیرعلامتی صحیح  k_Vim_Dialog_Icon_Stripe_چوڑائی = 30;
ثابت غیرعلامتی صحیح  kVimDialogButtonsSpacingX = 9;
ثابت غیرعلامتی صحیح  kVimDialogButtonsSpacingY = 4;
ثابت غیرعلامتی صحیح  kVimDialogSpacingX = 6;
ثابت غیرعلامتی صحیح  kVimDialogSpacingY = 10;
ثابت غیرعلامتی صحیح  k_Vim_Dialog_اقل_چوڑائی  = 310;
ثابت غیرعلامتی صحیح  k_Vim_Dialog_اقل_اونچائی = 75;
ثابت BRect	    kDefaultRect =
BRect(0, 0, k_Vim_Dialog_اقل_چوڑائی, k_Vim_Dialog_اقل_اونچائی);

VimDialog::VimDialog(صحیح type, ثابت حرف *title, ثابت حرف *پیغام_2,
	ثابت حرف *buttons, صحیح dfltbutton, ثابت حرف *textfield, صحیح ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_کھڑکی_LOOK, B_MODAL_APP_کھڑکی_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_قدر(dfltbutton)
    , f_پیغام_View(تہی)
    , f_داخلہ_Control(تہی)
    , f_داخلہ_قدر(textfield)
{
    //	master view
    VimDialog::View* view = نیا VimDialog::View(Bounds());
    اگر (view == تہی)
	واپس;

    اگر (title == تہی)
	سیٹ_Title("Vim " VIM_نسخہ_MEDIUM);

    جوڑو_بچہ(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    اعشاری max_Button_چوڑائی  = 0;
    اعشاری max_Button_اونچائی = 0;
    اعشاری buttons_چوڑائی	  = 0;
    اعشاری buttons_اونچائی   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    کرو
    {
	int32 انتہا = strButtons.ڈھونڈو_پہلا('\n');
	اگر (انتہا != B_ERROR)
	    strButtons.سیٹ_Byte_At(انتہا, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->جوڑو_بچہ(button);
	fButtonsList.جوڑو_شے(button);

	max_Button_چوڑائی	= max_c(max_Button_چوڑائی,  button->Bounds().چوڑائی_2());
	max_Button_اونچائی = max_c(max_Button_اونچائی, button->Bounds().اونچائی_2());
	buttons_چوڑائی   += button->Bounds().چوڑائی_2();
	buttons_اونچائی  += button->Bounds().اونچائی_2();

	اگر (انتہا == B_ERROR)
	    توڑو;

	strButtons.Remove(0, انتہا + 1);
    } جب (سچ);

    int32 buttons_گنتی = fButtonsList.گنتی_اشیاء();
    buttons_چوڑائی      += kVimDialogButtonsSpacingX * (buttons_گنتی - 1);
    buttons_اونچائی     += kVimDialogButtonsSpacingY * (buttons_گنتی - 1);
    اعشاری dialog_چوڑائی  = buttons_چوڑائی + k_Vim_Dialog_Icon_Stripe_چوڑائی +
	kVimDialogSpacingX * 2;
    اعشاری dialog_اونچائی = max_Button_اونچائی + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    منطقی vertical = (vim_strchr(p_go, GO_VERTICAL) != تہی) ||
	dialog_چوڑائی >= gui.vim_کھڑکی->Bounds().چوڑائی_2();
    اگر (vertical) {
	dialog_چوڑائی  -= buttons_چوڑائی;
	dialog_چوڑائی  += max_Button_چوڑائی;
	dialog_اونچائی -= max_Button_اونچائی;
	dialog_اونچائی += buttons_اونچائی;
    }

    dialog_چوڑائی  = max_c(dialog_چوڑائی,  k_Vim_Dialog_اقل_چوڑائی);

    //	message view
    BRect rect(0, 0, dialog_چوڑائی, 0);
    rect.بائیں  += k_Vim_Dialog_Icon_Stripe_چوڑائی + 16 + kVimDialogSpacingX;
    rect.چوٹی   += kVimDialogSpacingY;
    rect.دائیں -= kVimDialogSpacingX;
    rect.bottom = rect.چوٹی;
    f_پیغام_View = نیا BTextView(rect, "_tv_", rect.انحراف_By_Copy(B_ORIGIN),
	    B_FOLLOW_بایاں | B_FOLLOW_چوٹی, B_WILL_DRAW);

    f_پیغام_View->سیٹ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color متن_Color = ui_color(B_PANEL_متن_COLOR);
    f_پیغام_View->سیٹ_Font_And_Color(be_plain_font, B_FONT_ALL, &متن_Color);
    f_پیغام_View->سیٹ_متن(پیغام_2);
    f_پیغام_View->MakeEditable(جھوٹ);
    f_پیغام_View->MakeSelectable(جھوٹ);
    f_پیغام_View->سیٹ_لفظ_Wrap(سچ);
    جوڑو_بچہ(f_پیغام_View);

    اعشاری پیغام_اونچائی = f_پیغام_View->متن_اونچائی(0, f_پیغام_View->گنتی_سطریں());
    f_پیغام_View->ResizeBy(0, پیغام_اونچائی);
    f_پیغام_View->سیٹ_متن_Rect(BRect(0, 0, rect.چوڑائی_2(), پیغام_اونچائی));

    dialog_اونچائی += پیغام_اونچائی;

    //	input view
    اگر (f_داخلہ_قدر != تہی) {
	rect.چوٹی     =
	    rect.bottom += پیغام_اونچائی + kVimDialogSpacingY;
	f_داخلہ_Control = نیا BTextControl(rect, "_iv_", تہی, f_داخلہ_قدر, تہی,
		B_FOLLOW_بایاں | B_FOLLOW_چوٹی, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_داخلہ_Control->متن_View()->سیٹ_متن(f_داخلہ_قدر);
	f_داخلہ_Control->متن_View()->سیٹ_لفظ_Wrap(جھوٹ);
	جوڑو_بچہ(f_داخلہ_Control);

	اعشاری چوڑائی = 0.f, اونچائی = 0.f;
	f_داخلہ_Control->Get_Preferred_سائز(&چوڑائی, &اونچائی);
	f_داخلہ_Control->MakeFocus(سچ);

	dialog_اونچائی += اونچائی + kVimDialogSpacingY * 1.5;
    }

    dialog_اونچائی = max_c(dialog_اونچائی, k_Vim_Dialog_اقل_اونچائی);

    Resize_مقصد(dialog_چوڑائی, dialog_اونچائی);
    Move_مقصد((gui.vim_کھڑکی->Bounds().چوڑائی_2() - dialog_چوڑائی) / 2,
	    (gui.vim_کھڑکی->Bounds().اونچائی_2() - dialog_اونچائی) / 2);

    //	adjust layout of buttons
    اعشاری button_چوڑائی = max_c(max_Button_چوڑائی, rect.چوڑائی_2() * 0.66);
    BPoint origin(dialog_چوڑائی, dialog_اونچائی);
    origin.x -= kVimDialogSpacingX + (vertical ? button_چوڑائی : buttons_چوڑائی);
    origin.y -= kVimDialogSpacingY + (vertical ? buttons_اونچائی	: max_Button_اونچائی);

    برائے (int32 i = 0 ; i < buttons_گنتی; i++) {
	BButton *button = (BButton*)fButtonsList.شے_At(i);
	button->Move_مقصد(origin);
	اگر (vertical) {
	    origin.y += button->چوکھٹا_2().اونچائی_2() + kVimDialogButtonsSpacingY;
	    button->Resize_مقصد(button_چوڑائی, button->چوکھٹا_2().اونچائی_2());
	} ورنہ
	    origin.x += button->چوکھٹا_2().چوڑائی_2() + kVimDialogButtonsSpacingX;

	اگر (dfltbutton == i + 1) {
	    button->MakeDefault(سچ);
	    button->MakeFocus(f_داخلہ_Control == تہی);
	}
    }
}

VimDialog::~VimDialog()
{
    اگر (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    صحیح
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    اگر (fDialogSem < B_OK) {
	Quit();
	واپس f_Dialog_قدر;
    }

    Show();

    جب (acquire_sem(fDialogSem) == B_INTERRUPTED);

    صحیح ret_قدر = f_Dialog_قدر;
    اگر (f_داخلہ_قدر != تہی)
	vim_strncpy((char_u*)f_داخلہ_قدر, (char_u*)f_داخلہ_Control->متن_2(), IOSIZE - 1);

    اگر (تالا_2())
	Quit();

    واپس ret_قدر;
}

خالی VimDialog::پیغام_Received(BMessage *پیغام)
{
    int32 which = 0;
    اگر (پیغام->what != k_Vim_Dialog_Button_پیغام ||
	    پیغام->ڈھونڈو_Int32("which", &which) != B_OK)
	واپس BWindow::پیغام_Received(پیغام);

    f_Dialog_قدر = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, ثابت حرف* label)
{
    BMessage *پیغام_2 = نیا BMessage(k_Vim_Dialog_Button_پیغام);
    پیغام_2->جوڑو_Int32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = نیا BButton(rect, name.String(), label, پیغام_2,
	    B_FOLLOW_دایاں | B_FOLLOW_BOTTOM);

    اعشاری چوڑائی = 0.f, اونچائی = 0.f;
    button->Get_Preferred_سائز(&چوڑائی, &اونچائی);
    button->Resize_مقصد(چوڑائی, اونچائی);

    واپس button;
}

VimDialog::View::View(BRect چوکھٹا)
    :	BView(چوکھٹا, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(تہی)
{
    سیٹ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    حذف fIconBitmap;
}

خالی VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.دائیں = k_Vim_Dialog_Icon_Stripe_چوڑائی;
    سیٹ_بلند_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    اگر (fIconBitmap == تہی)
	واپس;

    سیٹ_Drawing_منوال(B_OP_ALPHA);
    سیٹ_Blending_منوال(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

خالی VimDialog::View::InitIcon(int32 type)
{
    اگر (type == VIM_GENERIC)
	واپس;

    BPath راستہ;
    حالت_t حالت_2 = ڈھونڈو_directory(B_BEOS_SERVERS_DIRECTORY, &راستہ);
    اگر (حالت_2 != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(حالت_2));
	واپس;
    }

    راستہ.Append("app_server");

    BFile file(راستہ.راستہ_2(), O_RDONLY);
    اگر (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	واپس;
    }

    BResources resources(&file);
    اگر (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	واپس;
    }

    ثابت حرف *name = "";
    انتخاب(type) {
	حالت VIM_ERROR:	    name = "stop"; توڑو;
	حالت VIM_WARNING:   name = "warn"; توڑو;
	حالت VIM_INFO:	    name = "info"; توڑو;
	حالت VIM_QUESTION:  name = "idea"; توڑو;
	طےشدہ: واپس;
    }

    int32 icon_سائز_2 = 32;
    fIconBitmap = نیا BBitmap(BRect(0, 0, icon_سائز_2 - 1, icon_سائز_2 - 1), 0, B_RGBA32);
    اگر (fIconBitmap == تہی || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == تہی) ? "null" : strerror(fIconBitmap->InitCheck()));
	واپس;
    }

    size_t سائز = 0;
    ثابت uint8* icon_ڈیٹا = تہی;
    //	try vector icon first?
    icon_ڈیٹا = (ثابت uint8*)resources.بھرو_Resource(B_سمتیہ_ICON_TYPE, name, &سائز);
    اگر (icon_ڈیٹا != تہی && BIconUtils::Get_سمتیہ_Icon(icon_ڈیٹا, سائز, fIconBitmap) == B_OK)
	واپس;

    //	try bitmap icon now
    icon_ڈیٹا = (ثابت uint8*)resources.بھرو_Resource(B_LARGE_ICON_TYPE, name, &سائز);
    اگر (icon_ڈیٹا == تہی) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	حذف fIconBitmap;
	fIconBitmap = تہی;
	واپس;
    }

    اگر (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(icon_ڈیٹا, icon_سائز_2, icon_سائز_2, icon_سائز_2, fIconBitmap);
}

ثابت غیرعلامتی صحیح  k_Vim_Dialog_OKButton_پیغام = 'FDOK';
ثابت غیرعلامتی صحیح  k_Vim_Dialog_Cancel_Button_پیغام = 'FDCN';
ثابت غیرعلامتی صحیح  k_Vim_Dialog_سائز_داخلہ_پیغام = 'SICH';
ثابت غیرعلامتی صحیح  k_Vim_Dialog_Family_Select_پیغام = 'MSFM';
ثابت غیرعلامتی صحیح  k_Vim_Dialog_Style_Select_پیغام = 'MSST';
ثابت غیرعلامتی صحیح  k_Vim_Dialog_سائز_Select_پیغام = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, اعشاری* سائز)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_کھڑکی_LOOK, B_MODAL_APP_کھڑکی_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , f_حالت(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_قدر(جھوٹ)
    , fFamily(family)
    , fStyle(style)
    , f_سائز(سائز)
    , f_Font_سائز(*سائز)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_داخلہ(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_لمبائی);
    strncpy(fFontStyle, *style, B_FONT_STYLE_لمبائی);

    //	"client" area view
    BBox *clientBox = نیا BBox(Bounds(), B_خالی_خانے_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_چوکھٹا_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    جوڑو_بچہ(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.بایاں_چوٹی(), RC.بایاں_چوٹی());

    //	at first create all controls
    fPreview = نیا BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->جوڑو_بچہ(fPreview);

    BBox* boxDivider = نیا BBox(rc, B_خالی_خانے_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->جوڑو_بچہ(boxDivider);

    BStringView *labelFamily = نیا BStringView(rc, "labelFamily", "Family:");
    clientBox->جوڑو_بچہ(labelFamily);
    labelFamily->Resize_مقصد_Preferred();

    BStringView *labelStyle = نیا BStringView(rc, "labelStyle", "Style:");
    clientBox->جوڑو_بچہ(labelStyle);
    labelStyle->Resize_مقصد_Preferred();

    BStringView *label_سائز = نیا BStringView(rc, "labelSize", "Size:");
    clientBox->جوڑو_بچہ(label_سائز);
    label_سائز->Resize_مقصد_Preferred();

    fFamiliesList = نیا BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = نیا BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_بایاں_دایاں, 0, جھوٹ, سچ);
    clientBox->جوڑو_بچہ(scrollFamilies);

    fStylesList= نیا BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = نیا BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_بایاں_دایاں, 0, جھوٹ, سچ);
    clientBox->جوڑو_بچہ(scrollStyles);

    f_Sizes_داخلہ = نیا BTextControl(rc, "inputSize", تہی, "???",
	    نیا BMessage(k_Vim_Dialog_سائز_داخلہ_پیغام));
    clientBox->جوڑو_بچہ(f_Sizes_داخلہ);
    f_Sizes_داخلہ->Resize_مقصد_Preferred();

    fSizesList = نیا BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = نیا BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_بایاں_دایاں, 0, جھوٹ, سچ);
    clientBox->جوڑو_بچہ(scrollSizes);

    BButton *buttonOK = نیا BButton(rc, "buttonOK", "OK",
			نیا BMessage(k_Vim_Dialog_OKButton_پیغام));
    clientBox->جوڑو_بچہ(buttonOK);
    buttonOK->Resize_مقصد_Preferred();

    BButton *buttonCancel = نیا BButton(rc, "buttonCancel", "Cancel",
			نیا BMessage(k_Vim_Dialog_Cancel_Button_پیغام));
    clientBox->جوڑو_بچہ(buttonCancel);
    buttonCancel->Resize_مقصد_Preferred();

    //	layout controls
    اعشاری سطر_اونچائی = labelFamily->Bounds().اونچائی_2();
    اعشاری preview_اونچائی = سطر_اونچائی * 3;
    اعشاری انحراف_YLabels = preview_اونچائی + kVimDialogSpacingY;
    اعشاری انحراف_YLists = انحراف_YLabels + سطر_اونچائی + kVimDialogSpacingY / 2;
    اعشاری انحراف_YSizes = انحراف_YLists + f_Sizes_داخلہ->Bounds().اونچائی_2() + kVimDialogSpacingY / 2;
    اعشاری lists_اونچائی = سطر_اونچائی * 9;
    اعشاری انحراف_YButtons = انحراف_YLists + lists_اونچائی +  kVimDialogSpacingY;
    اعشاری max_Controls_اونچائی = انحراف_YButtons + buttonOK->Bounds().اونچائی_2();
    اعشاری families_چوڑائی = labelFamily->Bounds().چوڑائی_2() * 5;
    اعشاری انحراف_XStyles = families_چوڑائی + kVimDialogSpacingX;
    اعشاری styles_چوڑائی = labelStyle->Bounds().چوڑائی_2() * 4;
    اعشاری انحراف_XSizes = انحراف_XStyles + styles_چوڑائی + kVimDialogSpacingX;
    اعشاری sizes_چوڑائی = label_سائز->Bounds().چوڑائی_2() * 2;
    اعشاری max_Controls_چوڑائی = انحراف_XSizes + sizes_چوڑائی;

    Resize_مقصد(max_Controls_چوڑائی + kVimDialogSpacingX * 2,
	max_Controls_اونچائی + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vim_کھڑکی->چوکھٹا_2();
    Move_مقصد(rcVim.بائیں + (rcVim.چوڑائی_2() - چوکھٹا_2().چوڑائی_2()) / 2,
	    rcVim.چوٹی + (rcVim.اونچائی_2() - چوکھٹا_2().اونچائی_2()) / 2);

    fPreview->Resize_مقصد(max_Controls_چوڑائی, preview_اونچائی);
    fPreview->سیٹ_Alignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, preview_اونچائی + kVimDialogSpacingY / 2);
    boxDivider->Resize_مقصد(max_Controls_چوڑائی, 1.f);

    labelFamily->MoveBy(0.f, انحراف_YLabels);
    labelStyle->MoveBy(انحراف_XStyles, انحراف_YLabels);
    label_سائز->MoveBy(انحراف_XSizes, انحراف_YLabels);

    //	text control alignment issues
    اعشاری insetX = f_Sizes_داخلہ->متن_View()->Bounds().چوڑائی_2() - f_Sizes_داخلہ->Bounds().چوڑائی_2();
    اعشاری insetY = f_Sizes_داخلہ->متن_View()->Bounds().چوڑائی_2() - f_Sizes_داخلہ->Bounds().چوڑائی_2();

    scrollFamilies->MoveBy(0.f, انحراف_YLists);
    scrollStyles->MoveBy(انحراف_XStyles, انحراف_YLists);
    f_Sizes_داخلہ->MoveBy(انحراف_XSizes + insetX / 2, انحراف_YLists + insetY / 2);
    scrollSizes->MoveBy(انحراف_XSizes, انحراف_YSizes);

    f_Sizes_داخلہ->سیٹ_Alignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->Resize_مقصد(families_چوڑائی, lists_اونچائی);
    scrollStyles->Resize_مقصد(styles_چوڑائی, lists_اونچائی);
    f_Sizes_داخلہ->Resize_مقصد(sizes_چوڑائی, f_Sizes_داخلہ->Bounds().اونچائی_2());
    scrollSizes->Resize_مقصد(sizes_چوڑائی,
	    lists_اونچائی - (انحراف_YSizes - انحراف_YLists));

    buttonOK->MoveBy(max_Controls_چوڑائی - buttonOK->Bounds().چوڑائی_2(), انحراف_YButtons);
    buttonCancel->MoveBy(max_Controls_چوڑائی - buttonOK->Bounds().چوڑائی_2()
	    - buttonCancel->Bounds().چوڑائی_2() - kVimDialogSpacingX, انحراف_YButtons);

    //	fill lists
    صحیح sel_اشاریہ = -1;
    صحیح گنتی = گنتی_font_families();
    برائے (صحیح i = 0; i < گنتی; i++) {
	font_family family;
	اگر (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->جوڑو_شے(نیا BString_شے((ثابت حرف*)family));
	    اگر (لڑی_موازنہ(family, fFontFamily, B_FONT_FAMILY_لمبائی) == 0)
		sel_اشاریہ = i;
	}
    }

    اگر (sel_اشاریہ >= 0) {
	fFamiliesList->Select(sel_اشاریہ);
	fFamiliesList->Scroll_مقصد_Selection();
    }

    _UpdateFontStyles();

    sel_اشاریہ = -1;
    برائے (صحیح سائز = 8, اشاریہ = 0; سائز <= 18; سائز++, اشاریہ++) {
	BString str;
	str << سائز;
	fSizesList->جوڑو_شے(نیا BString_شے(str));
	اگر (سائز == f_Font_سائز)
	    sel_اشاریہ = اشاریہ;

    }

    اگر (sel_اشاریہ >= 0) {
	fSizesList->Select(sel_اشاریہ);
	fSizesList->Scroll_مقصد_Selection();
    }

    fFamiliesList->سیٹ_Selection_پیغام(نیا BMessage(k_Vim_Dialog_Family_Select_پیغام));
    fStylesList->سیٹ_Selection_پیغام(نیا BMessage(k_Vim_Dialog_Style_Select_پیغام));
    fSizesList->سیٹ_Selection_پیغام(نیا BMessage(k_Vim_Dialog_سائز_Select_پیغام));
    f_Sizes_داخلہ->سیٹ_Modification_پیغام(نیا BMessage(k_Vim_Dialog_سائز_داخلہ_پیغام));

    Update_سائز_داخلہ_Preview();
    _UpdateFontPreview();

    f_حالت = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    اگر (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    خالی
VimSelectFontDialog::_CleanList(BListView* list)
{
    جب (0 < list->گنتی_اشیاء())
	حذف (dynamic_cast<BString_شے*>(list->Remove_شے((int32)0)));
}

    منطقی
VimSelectFontDialog::Go()
{
    اگر (f_حالت != B_OK) {
	Quit();
	واپس NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    اگر (fDialogSem < B_OK) {
	Quit();
	واپس f_Dialog_قدر;
    }

    Show();

    جب (acquire_sem(fDialogSem) == B_INTERRUPTED);

    منطقی ret_قدر = f_Dialog_قدر;

    اگر (تالا_2())
	Quit();

    واپس ret_قدر;
}


خالی VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 sel_اشاریہ = -1;
    int32 گنتی = گنتی_font_styles(fFontFamily);
    برائے (int32 i = 0; i < گنتی; i++) {
	font_style style;
	uint32 flags = 0;
	اگر (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->جوڑو_شے(نیا BString_شے((ثابت حرف*)style));
	    اگر (لڑی_موازنہ(style, fFontStyle, B_FONT_STYLE_لمبائی) == 0)
		sel_اشاریہ = i;
	}
    }

    اگر (sel_اشاریہ >= 0) {
	fStylesList->Select(sel_اشاریہ);
	fStylesList->Scroll_مقصد_Selection();
    } ورنہ
	fStylesList->Select(0);
}


خالی VimSelectFontDialog::Update_سائز_داخلہ_Preview()
{
    حرف buf[10] = {0};
    vim_snprintf(buf, حجم(buf), (حرف*)"%.0f", f_Font_سائز);
    f_Sizes_داخلہ->سیٹ_متن(buf);
}


خالی VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.سیٹ_سائز(f_Font_سائز);
    font.سیٹ_Family_And_Style(fFontFamily, fFontStyle);
    fPreview->سیٹ_Font(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_سائز);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (صحیح)f_Font_سائز << " pt.";
    fPreview->سیٹ_متن(str);
}


    منطقی
VimSelectFontDialog::Update_From_List_شے(BListView* list, حرف* متن, صحیح متن_سائز)
{
    int32 اشاریہ = list->CurrentSelection();
    اگر (اشاریہ < 0)
	واپس جھوٹ;
    BString_شے* شے = (BString_شے*)list->شے_At(اشاریہ);
    اگر (شے == تہی)
	واپس جھوٹ;
    strncpy(متن, شے->متن_2(), متن_سائز);
    واپس سچ;
}


خالی VimSelectFontDialog::پیغام_Received(BMessage *پیغام)
{
    انتخاب (پیغام->what) {
	حالت k_Vim_Dialog_OKButton_پیغام:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_لمبائی);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_لمبائی);
	    *f_سائز = f_Font_سائز;
	    f_Dialog_قدر = سچ;
	حالت k_Vim_Dialog_Cancel_Button_پیغام:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    واپس;
	حالت B_کلید_اوپر:
	    {
		int32 کلید = 0;
		اگر (پیغام->ڈھونڈو_Int32("raw_char", &کلید) == B_OK
			&& کلید == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    توڑو;

	حالت k_Vim_Dialog_Family_Select_پیغام:
	    اگر (Update_From_List_شے(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_لمبائی)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    توڑو;
	حالت k_Vim_Dialog_Style_Select_پیغام:
	    اگر (Update_From_List_شے(fStylesList,
		    fFontStyle, B_FONT_STYLE_لمبائی))
		_UpdateFontPreview();
	    توڑو;
	حالت k_Vim_Dialog_سائز_Select_پیغام:
	    {
		حرف buf[10] = {0};
		اگر (Update_From_List_شے(fSizesList, buf, حجم(buf))) {
		    اعشاری سائز = atof(buf);
		    اگر (سائز > 0.f) {
			f_Font_سائز = سائز;
			Update_سائز_داخلہ_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    توڑو;
	حالت k_Vim_Dialog_سائز_داخلہ_پیغام:
	    {
		اعشاری سائز = atof(f_Sizes_داخلہ->متن_2());
		اگر (سائز > 0.f) {
		    f_Font_سائز = سائز;
		    _UpdateFontPreview();
		}
	    }
	    توڑو;
	طےشدہ:
	    توڑو;
    }
    واپس BWindow::پیغام_Received(پیغام);
}

#اگر_ختم // FEAT_GUI_DIALOG

#اگر_تعریف_شدہ FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
ساکن BMessage * Menu_پیغام(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect چوکھٹا, ثابت حرف *name) :
    BBox(چوکھٹا, name, B_FOLLOW_بایاں | B_FOLLOW_چوٹی, B_WILL_DRAW | B_چوکھٹا_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 گنتی = fButtonsList.گنتی_اشیاء();
    برائے (int32 i = 0; i < گنتی; i++)
	حذف (BPictureButton*)fButtonsList.شے_At(i);
    fButtonsList.Make_خالی_خانے();

    حذف معمول_Buttons_Bitmap;
    حذف grayedButtonsBitmap;
    معمول_Buttons_Bitmap    = تہی;
    grayedButtonsBitmap  = تہی;
}

    خالی
VimToolbar::Attached_مقصد_کھڑکی()
{
    BBox::Attached_مقصد_کھڑکی();

    سیٹ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    اعشاری
VimToolbar::Toolbar_اونچائی() ثابت
{
    اعشاری سائز = تہی == معمول_Buttons_Bitmap ? 18. : معمول_Buttons_Bitmap->Bounds().اونچائی_2();
    واپس سائز + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    منطقی
VimToolbar::Modify_Bitmap_مقصد_Grayed(BBitmap *bitmap)
{
    اعشاری اونچائی = bitmap->Bounds().اونچائی_2();
    اعشاری چوڑائی  = bitmap->Bounds().چوڑائی_2();

    rgb_color *بٹ = (rgb_color*)bitmap->بٹ_2();
    int32 pixels = bitmap->بٹ_لمبائی() / 4;
    برائے (int32 i = 0; i < pixels; i++) {
	بٹ[i].red = بٹ[i].green =
	بٹ[i].blue = ((uint32)بٹ[i].red + بٹ[i].green + بٹ[i].blue) / 3;
	بٹ[i].alpha /= 4;
    }

    واپس سچ;
}

    منطقی
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    معمول_Buttons_Bitmap = بھرو_Vim_Bitmap("builtin-tools.png");
    اگر (معمول_Buttons_Bitmap == تہی)
	//  customized not found? dig application resources for "builtin-tools" one
	معمول_Buttons_Bitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    اگر (معمول_Buttons_Bitmap == تہی)
	واپس جھوٹ;

    BMessage archive;
    معمول_Buttons_Bitmap->Archive(&archive);

    grayedButtonsBitmap = نیا BBitmap(&archive);
    اگر (grayedButtonsBitmap == تہی)
	واپس جھوٹ;

    //	modify grayed bitmap
    Modify_Bitmap_مقصد_Grayed(grayedButtonsBitmap);

    واپس سچ;
}

BBitmap *VimToolbar::بھرو_Vim_Bitmap(ثابت حرف* fileName)
{
    BBitmap *bitmap = تہی;

    صحیح mustfree = 0;
    char_u* runtime_راستہ = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    اگر (runtime_راستہ != تہی && fileName != تہی) {
	BString str_راستہ((حرف*)runtime_راستہ);
	str_راستہ << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(str_راستہ.String());
    }

    اگر (mustfree)
	vim_free(runtime_راستہ);

    واپس bitmap;
}

    منطقی
VimToolbar::GetPictureFromBitmap(BPicture *picture_مقصد, int32 اشاریہ, BBitmap *bitmapFrom, منطقی pressed)
{
    اعشاری سائز = bitmapFrom->Bounds().اونچائی_2() + 1.;

    BView view(BRect(0, 0, سائز, سائز), "", 0, 0);

    جوڑو_بچہ(&view);
    view.BeginPicture(picture_مقصد);

    view.سیٹ_بلند_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.سیٹ_Drawing_منوال(B_OP_OVER);

    BRect ماخذ(0, 0, سائز - 1, سائز - 1);
    BRect destination(ماخذ);

    ماخذ.انحراف_By(سائز * اشاریہ, 0);
    destination.انحراف_By(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, ماخذ, destination);

    اگر (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	سائز += ButtonMargin * 2 - 1;
	view.Begin_سطر_Array(4);
	view.جوڑو_سطر(BPoint(0, 0),	 BPoint(سائز, 0),    shadowColor);
	view.جوڑو_سطر(BPoint(سائز, 0),	 BPoint(سائز, سائز), shineColor);
	view.جوڑو_سطر(BPoint(سائز, سائز), BPoint(0, سائز),    shineColor);
	view.جوڑو_سطر(BPoint(0, سائز),	 BPoint(0, 0),	     shadowColor);
	view.انتہا_سطر_Array();
    }

    view.انتہا_Picture();
    Remove_بچہ(&view);

    واپس سچ;
}

    منطقی
VimToolbar::جوڑو_Button(int32 اشاریہ, vimmenu_T *menu)
{
    BPictureButton *button = تہی;
    اگر (!menu_is_separator(menu->name)) {
	اعشاری سائز = معمول_Buttons_Bitmap ?
	    معمول_Buttons_Bitmap->Bounds().اونچائی_2() + 1. + ButtonMargin * 2 : 18.;
	BRect چوکھٹا(0, 0, سائز, سائز);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture picture_سرمئی;

	اگر (menu->iconfile == تہی && menu->iconidx >= 0 && معمول_Buttons_Bitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, معمول_Buttons_Bitmap, سچ);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, معمول_Buttons_Bitmap, جھوٹ);
	    GetPictureFromBitmap(&picture_سرمئی, menu->iconidx, grayedButtonsBitmap, جھوٹ);
	} ورنہ {

	    char_u بفر[MAXPATHL] = {0};
	    BBitmap *bitmap = تہی;

	    اگر (menu->iconfile) {
		gui_ڈھونڈو_iconfile(menu->iconfile, بفر, (حرف*)"png");
		bitmap = BTranslationUtils::GetBitmap((حرف*)بفر);
	    }

	    اگر (bitmap == تہی && gui_ڈھونڈو_bitmap(menu->name, بفر, (حرف*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((حرف*)بفر);

	    اگر (bitmap == تہی)
		bitmap = نیا BBitmap(BRect(0, 0, سائز, سائز), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, سچ);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, جھوٹ);
	    Modify_Bitmap_مقصد_Grayed(bitmap);
	    GetPictureFromBitmap(&picture_سرمئی, 0, bitmap, جھوٹ);

	    حذف bitmap;
	}

	button = نیا BPictureButton(چوکھٹا, (حرف*)menu->name,
		    &pictureOff, &pictureOn, Menu_پیغام(menu));

	button->سیٹ_Disabled_On(&picture_سرمئی);
	button->سیٹ_Disabled_Off(&picture_سرمئی);

	button->سیٹ_ہدف(gui.vim_متن_Area);

	جوڑو_بچہ(button);

	menu->button = button;
    }

    منطقی نتیجہ = fButtonsList.جوڑو_شے(button, اشاریہ);
    InvalidateLayout();
    واپس نتیجہ;
}

    منطقی
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    اگر (menu->button) {
	اگر (fButtonsList.Remove_شے(menu->button)) {
	    حذف menu->button;
	    menu->button = تہی;
	}
    }
    واپس سچ;
}

    منطقی
VimToolbar::سرمئی_Button(vimmenu_T *menu, صحیح grey)
{
    اگر (menu->button) {
	int32 اشاریہ = fButtonsList.اشاریہ_کا(menu->button);
	اگر (اشاریہ >= 0)
	    menu->button->سیٹ_Enabled(grey ? جھوٹ : سچ);
    }
    واپس سچ;
}

    خالی
VimToolbar::InvalidateLayout()
{
    int32 انحراف = ToolbarMargin;
    int32 گنتی = fButtonsList.گنتی_اشیاء();
    برائے (int32 i = 0; i < گنتی; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.شے_At(i);
	اگر (button) {
	    button->Move_مقصد(انحراف, ToolbarMargin);
	    انحراف += button->Bounds().چوڑائی_2() + ToolbarMargin;
	} ورنہ
	    انحراف += ToolbarMargin * 3;
    }
}

#اگر_ختم /*FEAT_TOOLBAR*/

#اگر defined(FEAT_GUI_TABLINE)

    اعشاری
Vim_Tab_سطر::Tabline_اونچائی() ثابت
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    واپس Tab_اونچائی(); //  + ToolbarMargin;
}

خالی
Vim_Tab_سطر::Mouse_نیچے_کو(BPoint نقطہ)
{
    اگر (!gui_mch_showing_tabline())
	واپس;

    BMessage *m = کھڑکی()->Current_پیغام();
    أكد(m);

    int32 buttons = 0;
    m->ڈھونڈو_Int32("buttons", &buttons);

    int32 clicks = 0;
    m->ڈھونڈو_Int32("clicks", &clicks);

    صحیح اشاریہ = 0; //  0 means here - no tab found
    برائے (صحیح i = 0; i < گنتی_Tabs(); i++) {
	اگر (Tab_چوکھٹا(i).شامل_ہے_2(نقطہ)) {
	    اشاریہ = i + 1; //  indexes are 1-based
	    توڑو;
	}
    }

    صحیح event = -1;

    اگر ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    ورنہ اگر (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = اشاریہ > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    ورنہ اگر (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPop_اوپر_Menu* نکالو_اوپر_Menu = نیا BPop_اوپر_Menu("tabLineContextMenu", جھوٹ, جھوٹ);
	نکالو_اوپر_Menu->جوڑو_شے(نیا BMenu_شے(_("Close tabi R"), نیا BMessage(TABLINE_MENU_CLOSE)));
	نکالو_اوپر_Menu->جوڑو_شے(نیا BMenu_شے(_("New tab    T"), نیا BMessage(TABLINE_MENU_NEW)));
	نکالو_اوپر_Menu->جوڑو_شے(نیا BMenu_شے(_("Open tab..."), نیا BMessage(TABLINE_MENU_OPEN)));

	Convert_مقصد_Screen(&نقطہ);
	BMenu_شے* شے = نکالو_اوپر_Menu->Go(نقطہ);
	اگر (شے != تہی) {
	    event = شے->Command();
	}

	حذف نکالو_اوپر_Menu;

    } ورنہ {
	//  default processing
	BTabView::Mouse_نیچے_کو(نقطہ);
	واپس;
    }

    اگر (event < 0)
	واپس;

    Vim_Tabline_Menu_پیغام tmm;
    tmm.اشاریہ = اشاریہ;
    tmm.event = event;
    write_پورٹ(gui.vdcmp, Vim_پیغام::TablineMenu, &tmm, حجم(tmm));
}

خالی
Vim_Tab_سطر::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    اگر (tab_سطر != تہی) {

	int32 i = 0;
	برائے (; i < tab_سطر->گنتی_Tabs(); i++)
	    اگر (یہ == tab_سطر->TabAt(i))
		توڑو;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	اگر (i < tab_سطر->گنتی_Tabs()) {
	    Vim_Tabline_پیغام tm;
	    tm.اشاریہ = i + 1;
	    write_پورٹ(gui.vdcmp, Vim_پیغام::Tabline, &tm, حجم(tm));
	}
    }
}

#اگر_ختم //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
ساکن حرف appsig[] = "application/x-vnd.Haiku-Vim-8";
کلید_map *کلید_Map;
حرف *کلید_Map_Chars;
صحیح main_exitcode = 127;

    حالت_t
gui_haiku_process_event(bigtime_t timeout)
{
    ساخت Vim_پیغام vm;
    int32 what;
    ssize_t سائز;

    سائز = read_پورٹ_etc(gui.vdcmp, &what, &vm, حجم(vm),
	    B_TIMEOUT, timeout);

    اگر (سائز >= 0) {
	انتخاب (what) {
	    حالت Vim_پیغام::کلید_2:
		{
		    char_u *string = vm.u.کلید_2.chars;
		    صحیح len = vm.u.کلید_2.لمبائی;
		    اگر (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_داخلہ_buf();
			got_int = TRUE;
		    }

		    اگر (vm.u.کلید_2.csi_escape)
#اگر_غیر_تعریف_شدہ FEAT_MBYTE_IME
		    {
			صحیح	i;
			char_u	buf[2];

			برائے (i = 0; i < len; ++i)
			{
			    جوڑو_مقصد_داخلہ_buf(string + i, 1);
			    اگر (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (صحیح)KE_CSI;
				جوڑو_مقصد_داخلہ_buf(buf, 2);
			    }
			}
		    }
#ورنہ
			جوڑو_مقصد_داخلہ_buf_csi(string, len);
#اگر_ختم
		    ورنہ
			جوڑو_مقصد_داخلہ_buf(string, len);
		}
		توڑو;
	    حالت Vim_پیغام::Resize:
		gui_resize_shell(vm.u.New_سائز.چوڑائی, vm.u.New_سائز.اونچائی);
		توڑو;
	    حالت Vim_پیغام::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_گنتی =
			atomic_جوڑو(&vm.u.Scroll.sb->scroll_Event_گنتی, -1);
		    اگر (old_گنتی <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.قدر, vm.u.Scroll.stillDragging);
		}
		توڑو;
#اگر defined(FEAT_MENU)
	    حالت Vim_پیغام::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		توڑو;
#اگر_ختم
	    حالت Vim_پیغام::Mouse:
		{
		    int32 old_گنتی;
		    اگر (vm.u.Mouse.button == MOUSE_DRAG)
			old_گنتی =
			    atomic_جوڑو(&gui.vim_متن_Area->mouse_Drag_Event_گنتی, -1);
		    ورنہ
			old_گنتی = 0;
		    اگر (old_گنتی <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		توڑو;
	    حالت Vim_پیغام::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		توڑو;
	    حالت Vim_پیغام::Focus:
		gui.اندر_focus = vm.u.Focus.فعال;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		اگر (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		توڑو;
	    حالت Vim_پیغام::Refs:
		::RefsReceived(vm.u.Refs.پیغام_2, vm.u.Refs.changedir);
		توڑو;
	    حالت Vim_پیغام::Tabline:
		send_tabline_event(vm.u.Tabline.اشاریہ);
		توڑو;
	    حالت Vim_پیغام::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.اشاریہ, vm.u.TablineMenu.event);
		توڑو;
	    طےشدہ:
		//  unrecognised message, ignore it
		توڑو;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    واپس سائز;
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

    صحیح
vim_تالا_screen()
{
    واپس !gui.vim_کھڑکی || gui.vim_کھڑکی->تالا_2();
}

    خالی
vim_unlock_screen()
{
    اگر (gui.vim_کھڑکی)
	gui.vim_کھڑکی->Unlock();
}

#تعریف_کرو تسلسل_BAPPLICATION_اندر_NEW_THREAD	0

#اگر تسلسل_BAPPLICATION_اندر_NEW_THREAD

    int32
تسلسل_vimapp(خالی *دلائل)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.تسلسل_2();		    // Run until Quit() called

    واپس 0;
}

#ورنہ

    int32
call_main(خالی *دلائل)
{
    ساخت Main_دلائل *ma = (Main_دلائل *)دلائل;

    واپس مین(ma->argc, ma->argv);
}
#اگر_ختم

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    خالی
gui_mch_prepare(
	صحیح	*argc,
	حرف	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    اگر (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(ڈھونڈو_thread(تہی), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_پورٹ(B_MAX_پورٹ_گنتی, "vim VDCMP");

#اگر تسلسل_BAPPLICATION_اندر_NEW_THREAD
	thread_id tid = spawn_thread(تسلسل_vimapp, "vim VimApp",
		tinfo.ترجیح, تہی);
	اگر (tid >= B_OK) {
	    resume_thread(tid);
	} ورنہ {
	    getout(1);
	}
#ورنہ
	Main_دلائل ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.ترجیح, &ma);
	اگر (tid >= B_OK) {
	    VimApp app(appsig);

	    gui.vimApp = &app;
	    resume_thread(tid);
	    /*
	     * This is rather horrible.
	     * call_main will call main() again...
	     * There will be no infinite recursion since
	     * gui.vimApp is set now.
	     */
	    app.تسلسل_2();		    // Run until Quit() called
	    // fprintf(stderr, "app.Run() returned...\n");
	    حالت_t dummy_exitcode;
	    (خالی)wait_for_thread(tid, &dummy_exitcode);

	    /*
	     * This path should be the normal one taken to exit Vim.
	     * The main() thread calls mch_exit() which calls
	     * gui_mch_exit() which terminates its thread.
	     */
	    باہر_نکلو(main_exitcode);
	}
#اگر_ختم
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
    اگر (!isatty(0)) {
	ساخت stat stat_stdin, stat_dev_null;

	اگر (fstat(0, &stat_stdin) == -1 ||
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
    صحیح
gui_mch_init_check(خالی)
{
    واپس OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    صحیح
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_عقب_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.عقب_pixel = gui.def_عقب_pixel;

    gui.scrollbar_چوڑائی = (صحیح) B_V_SCROLL_BAR_چوڑائی;
    gui.scrollbar_اونچائی = (صحیح) B_H_SCROLL_BAR_اونچائی;
#اگر_تعریف_شدہ FEAT_MENU
    gui.menu_اونچائی = 19;   //	initial guess -
    //	correct for my default settings
#اگر_ختم
    gui.border_انحراف = 3;  //	coordinates are inside window borders

    اگر (gui.vdcmp < B_OK)
	واپس FAIL;
    get_کلید_map(&کلید_Map, &کلید_Map_Chars);

    gui.vim_کھڑکی = نیا Vim_کھڑکی();	// hidden and locked
    اگر (!gui.vim_کھڑکی)
	واپس FAIL;

    gui.vim_کھڑکی->تسلسل_2();	// Run() unlocks but does not show

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    سیٹ_معمول_colors();

    /*
     * Check that none of the colors are the same as the background color
     */
    gui_check_colors();

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();	    // re-init colors and fonts

    gui_mch_new_colors();	// window must exist for this

    واپس OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    خالی
gui_mch_new_colors()
{
    rgb_color rgb = GUI_مقصد_RGB(gui.عقب_pixel);

    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vimForm->سیٹ_View_Color(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحیح
gui_mch_open()
{
    اگر (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_سیٹ_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_کھڑکی->Show();
	gui.vim_کھڑکی->Unlock();
	واپس OK;
    }

    واپس FAIL;
}

    خالی
gui_mch_exit(صحیح vim_exitcode)
{
    اگر (gui.vim_کھڑکی) {
	thread_id tid = gui.vim_کھڑکی->Thread();
	gui.vim_کھڑکی->تالا_2();
	gui.vim_کھڑکی->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_پورٹ(gui.vdcmp);
#اگر !تسلسل_BAPPLICATION_اندر_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#اگر_ختم
    اگر (gui.vimApp) {
	Vim_متن_Area_View::guiBlankMouse(جھوٹ);

	main_exitcode = vim_exitcode;
#اگر تسلسل_BAPPLICATION_اندر_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->تالا_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#ورنہ
	gui.vimApp->تالا_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	// suicide
	exit_thread(vim_exitcode);
#اگر_ختم
    }
    // If we are somehow still here, let mch_exit() handle things.
}

/*
 * Get the position of the top left corner of the window.
 */
    صحیح
gui_mch_get_winpos(صحیح *x, صحیح *y)
{
    اگر (gui.vim_کھڑکی->تالا_2()) {
	BRect r;
	r = gui.vim_کھڑکی->چوکھٹا_2();
	gui.vim_کھڑکی->Unlock();
	*x = (صحیح)r.بائیں;
	*y = (صحیح)r.چوٹی;
	واپس OK;
    }
    ورنہ
	واپس FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    خالی
gui_mch_سیٹ_winpos(صحیح x, صحیح y)
{
    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_کھڑکی->Move_مقصد(x, y);
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
خالی
gui_mch_سیٹ_shellsize(
	صحیح	چوڑائی,
	صحیح	اونچائی,
	صحیح	min_چوڑائی,
	صحیح	min_اونچائی,
	صحیح	بنیاد_چوڑائی,
	صحیح	بنیاد_اونچائی,
	صحیح	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_کھڑکی->Resize_مقصد(چوڑائی - PEN_چوڑائی, اونچائی - PEN_چوڑائی);

	// set size limits
	اعشاری min_چوڑائی_2, max_چوڑائی, min_اونچائی_2, max_اونچائی;

	gui.vim_کھڑکی->Get_سائز_Limits(&min_چوڑائی_2, &max_چوڑائی,
		&min_اونچائی_2, &max_اونچائی);
	gui.vim_کھڑکی->سیٹ_سائز_Limits(min_چوڑائی, max_چوڑائی,
		min_اونچائی, max_اونچائی);

	/*
	 * Set the resizing alignment depending on font size.
	 */
	gui.vim_کھڑکی->سیٹ_کھڑکی_Alignment(
		B_PIXEL_ALIGNMENT,	//  window_alignment mode,
		1,		//  int32 h,
		0,		//  int32 hOffset = 0,
		gui.char_چوڑائی,	    //	int32 width = 0,
		بنیاد_چوڑائی,	    //	int32 widthOffset = 0,
		1,		//  int32 v = 0,
		0,		//  int32 vOffset = 0,
		gui.char_اونچائی,	//  int32 height = 0,
		بنیاد_اونچائی	    //	int32 heightOffset = 0
		);

	gui.vim_کھڑکی->Unlock();
    }
}

خالی
gui_mch_get_screen_dimensions(
	صحیح	*screen_w,
	صحیح	*screen_h)
{
    BRect چوکھٹا;

    {
	BScreen screen(gui.vim_کھڑکی);

	اگر (screen.IsValid()) {
	    چوکھٹا = screen.چوکھٹا_2();
	} ورنہ {
	    چوکھٹا.دائیں = 640;
	    چوکھٹا.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (صحیح) چوکھٹا.دائیں - 2 * gui.scrollbar_چوڑائی - 20;
    *screen_h = (صحیح) چوکھٹا.bottom - gui.scrollbar_اونچائی
#اگر_تعریف_شدہ FEAT_MENU
	- gui.menu_اونچائی
#اگر_ختم
	- 30;
}

خالی
gui_mch_سیٹ_متن_area_pos(
	صحیح	x,
	صحیح	y,
	صحیح	w,
	صحیح	h)
{
    اگر (!gui.vim_متن_Area)
	واپس;

    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_متن_Area->Move_مقصد(x, y);
	gui.vim_متن_Area->Resize_مقصد(w - PEN_چوڑائی, h - PEN_چوڑائی);

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
	اگر (gui.vimForm->Tab_سطر() != تہی) {
	    gui.vimForm->Tab_سطر()->Resize_مقصد(w, gui.vimForm->Tabline_اونچائی());
	}
#اگر_ختم // FEAT_GUI_TABLINE

	gui.vim_کھڑکی->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

خالی
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	صحیح	flag)
{
    VimScrollBar *vsb = sb->id;
    اگر (gui.vim_کھڑکی->تالا_2()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	اگر (flag) {
	    اگر (vsb->IsHidden()) {
		vsb->Show();
	    }
	} ورنہ {
	    اگر (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vim_کھڑکی->Unlock();
    }
}

خالی
gui_mch_سیٹ_scrollbar_thumb(
	scrollbar_T *sb,
	صحیح	val,
	صحیح	سائز,
	صحیح	max)
{
    اگر (gui.vim_کھڑکی->تالا_2()) {
	VimScrollBar *s = sb->id;
	اگر (max == 0) {
	    s->سیٹ_قدر(0);
	    s->سیٹ_Range(0.0, 0.0);
	} ورنہ {
	    s->سیٹ_Proportion((اعشاری)سائز / (max + 1.0));
	    s->سیٹ_اقدام(1.0, سائز > 5 ? سائز - 2 : سائز);
#اگر_غیر_تعریف_شدہ SCROLL_PAST_انتہا	    //	really only defined in gui.c...
	    max = max + 1 - سائز;
#اگر_ختم
	    اگر (max < s->قدر_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->سیٹ_قدر(val);
		s->سیٹ_Range(0.0, max);
	    } ورنہ {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->سیٹ_Range(0.0, max);
		s->سیٹ_قدر(val);
	    }
	}
	gui.vim_کھڑکی->Unlock();
    }
}

خالی
gui_mch_سیٹ_scrollbar_pos(
	scrollbar_T *sb,
	صحیح	x,
	صحیح	y,
	صحیح	w,
	صحیح	h)
{
    اگر (gui.vim_کھڑکی->تالا_2()) {
	BRect winb = gui.vim_کھڑکی->Bounds();
	اعشاری vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->Resize_مقصد(w - PEN_چوڑائی, h - PEN_چوڑائی);
	اگر (winb.دائیں-(x+w)<w) vsbx = winb.دائیں - (w - PEN_چوڑائی);
	vsb->Move_مقصد(vsbx, vsby);
	gui.vim_کھڑکی->Unlock();
    }
}

    صحیح
gui_mch_get_scrollbar_xpadding(خالی)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    واپس 0;
}

    صحیح
gui_mch_get_scrollbar_ypadding(خالی)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    واپس 0;
}

خالی
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	صحیح	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = نیا VimScrollBar(sb, posture);
    اگر (gui.vim_کھڑکی->تالا_2()) {
	vsb->سیٹ_ہدف(gui.vim_متن_Area);
	vsb->Hide();
	gui.vimForm->جوڑو_بچہ(vsb);
	gui.vim_کھڑکی->Unlock();
    }
}

#اگر defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
خالی
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    اگر (gui.vim_کھڑکی->تالا_2()) {
	sb->id->RemoveSelf();
	حذف sb->id;
	gui.vim_کھڑکی->Unlock();
    }
}
#اگر_ختم

/*
 * Cursor does not flash
 */
    صحیح
gui_mch_is_blink_off(خالی)
{
    واپس FALSE;
}

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON blinking, cursor is shown
 */

#تعریف_کرو BLINK_NONE  0
#تعریف_کرو BLINK_OFF   1
#تعریف_کرو BLINK_ON    2

ساکن صحیح	blink_state = BLINK_NONE;
ساکن long_u	    blink_waittime = 700;
ساکن long_u	    blink_ontime = 400;
ساکن long_u	    blink_offtime = 250;
ساکن صحیح  blink_timer = 0;

خالی
gui_mch_سیٹ_blinking(
	طویل	waittime,
	طویل	on,
	طویل	off)
{
    // TODO
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    خالی
gui_mch_stop_blink(صحیح may_call_gui_update_cursor)
{
    // TODO
    اگر (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    اگر (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    خالی
gui_mch_آغاز_blink()
{
    // TODO
    اگر (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    اگر (blink_waittime && blink_ontime && blink_offtime && gui.اندر_focus)
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
صحیح
gui_mch_init_font(
	char_u	    *font_name,
	صحیح	    fontset)
{
    اگر (gui.vim_کھڑکی->تالا_2())
    {
	صحیح rc = gui.vim_متن_Area->mchInitFont(font_name);
	gui.vim_کھڑکی->Unlock();

	واپس rc;
    }

    واپس FAIL;
}


    صحیح
gui_mch_adjust_charsize()
{
    واپس FAIL;
}


    صحیح
gui_mch_font_dialog(font_family* family, font_style* style, اعشاری* سائز)
{
#اگر defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = نیا VimSelectFontDialog(family, style, سائز);
    منطقی ret = dialog->Go();
	حذف dialog;
	واپس ret;
#ورنہ
    واپس NOFONT;
#اگر_ختم // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	صحیح	    giveErrorIfMissing)
{
    ساکن VimFont *fontList = تہی;

    اگر (!gui.اندر_use)	//  can't do this when GUI not running
	واپس NOFONT;

    //	storage for locally modified name;
    ثابت صحیح buff_سائز = B_FONT_FAMILY_لمبائی + B_FONT_STYLE_لمبائی + 20;
    ساکن حرف font_name[buff_سائز] = {0};
    font_family family = {0};
    font_style	style  = {0};
    اعشاری سائز = 0.f;

    اگر (name == 0 && be_fixed_font == 0) {
	اگر (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	واپس NOFONT;
    }

    منطقی useSelectGUI = جھوٹ;
    اگر (name != تہی)
	اگر (STRCMP(name, "*") == 0) {
	    useSelectGUI = سچ;
	    STRNCPY(font_name, hl_get_font_name(), buff_سائز);
	} ورنہ
	    STRNCPY(font_name, name, buff_سائز);

    اگر (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	سائز = be_fixed_font->سائز_2();
	vim_snprintf(font_name, buff_سائز,
	    (حرف*)"%s/%s/%.0f", family, style, سائز);
    }

    //	replace underscores with spaces
    حرف* انتہا = 0;
    جب (انتہا = strchr((حرف *)font_name, '_'))
	*انتہا = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    ساکن حرف buff[buff_سائز] = {0};
    STRNCPY(buff, font_name, buff_سائز);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_لمبائی);
    حرف* style_s = strtok(0, "/\0");
    اگر (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_لمبائی);
    سائز = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    اگر (useSelectGUI) {
	اگر (gui_mch_font_dialog(&family, &style, &سائز) == NOFONT)
	    واپس FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_سائز,
		(حرف*)"%s/%s/%.0f", family, style, سائز);
	hl_سیٹ_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	اگر (new_p_guifont != تہی) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    برائے ( ; *new_p_guifont; ++new_p_guifont)
		اگر (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    برائے (flp = fontList; flp; flp = flp->اگلا) {
	اگر (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    واپس (GuiFont)flp;
	}
    }

    VimFont *font = نیا VimFont();
    font->name = vim_strsave((char_u*)font_name);

    اگر (گنتی_font_styles(family) <= 0) {
	اگر (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	حذف font;
	واپس NOFONT;
    }

    //	Remember font in the static list for later use
    font->اگلا = fontList;
    fontList = font;

    font->سیٹ_Family_And_Style(family, style);
    اگر (سائز > 0.f)
	font->سیٹ_سائز(سائز);

    font->سیٹ_Spacing(B_FIXED_SPACING);
    font->سیٹ_Encoding(B_UNICODE_UTF8);

    واپس (GuiFont)font;
}

/*
 * Set the current text font.
 */
خالی
gui_mch_سیٹ_font(
	GuiFont	font)
{
    اگر (gui.vim_کھڑکی->تالا_2()) {
	VimFont *vf = (VimFont *)font;

	gui.vim_متن_Area->سیٹ_Font(vf);

	gui.char_چوڑائی = (صحیح) vf->String_چوڑائی("n");
	font_اونچائی fh;
	vf->Get_اونچائی(&fh);
	gui.char_اونچائی = (صحیح)(fh.ascent + 0.9999)
	    + (صحیح)(fh.descent + 0.9999) + (صحیح)(fh.leading + 0.9999);
	gui.char_ascent = (صحیح)(fh.ascent + 0.9999);

	gui.vim_کھڑکی->Unlock();
    }
}

// XXX TODO This is apparently never called...
خالی
gui_mch_free_font(
	GuiFont	font)
{
    اگر (font == NOFONT)
	واپس;
    VimFont *f = (VimFont *)font;
    اگر (--f->refcount <= 0) {
	اگر (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	حذف f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    اگر (name == تہی)
	واپس تہی;
    واپس vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحیح
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
	font_اونچائی fh = {0};
	font->Get_اونچائی(&fh);
	gui.char_اونچائی = (صحیح)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (صحیح)(fh.ascent + 0.5) + p_linespace / 2;
    }
    واپس OK;
}

    خالی
gui_mch_getmouse(صحیح *x, صحیح *y)
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

    خالی
gui_mch_mousehide(صحیح hide)
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
    واپس gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
خالی
gui_mch_سیٹ_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_مقصد_RGB(color);
    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_متن_Area->سیٹ_بلند_Color(rgb);
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Set the current text background color.
 */
خالی
gui_mch_سیٹ_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_مقصد_RGB(color);
    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_متن_Area->سیٹ_کم_Color(rgb);
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    خالی
gui_mch_سیٹ_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

خالی
gui_mch_draw_string(
	صحیح	صف,
	صحیح	ستون,
	char_u	*s,
	صحیح	len,
	صحیح	flags)
{
    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_متن_Area->mchDrawString(صف, ستون, s, len, flags);
	gui.vim_کھڑکی->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(صحیح r, صحیح g, صحیح b)
{
    واپس gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
صحیح
gui_mch_haskey(
	char_u	*name)
{
    صحیح i;

    برائے (i = 0; special_کلیدیں[i].Be_کلیدیں != 0; i++)
	اگر (name[0] == special_کلیدیں[i].vim_code0 &&
		name[1] == special_کلیدیں[i].vim_code1)
	    واپس OK;
    واپس FAIL;
}

    خالی
gui_mch_beep()
{
    ::beep();
}

    خالی
gui_mch_flash(صحیح msec)
{
    // Do a visual beep by reversing the foreground and background colors

    اگر (gui.vim_کھڑکی->تالا_2()) {
	BRect rect = gui.vim_متن_Area->Bounds();

	gui.vim_متن_Area->سیٹ_Drawing_منوال(B_OP_INVERT);
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->Sync();
	snooze(msec * 1000);	 // wait for a few msec
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->سیٹ_Drawing_منوال(B_OP_COPY);
	gui.vim_متن_Area->Flush();
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
خالی
gui_mch_invert_rectangle(
	صحیح	r,
	صحیح	c,
	صحیح	nr,
	صحیح	nc)
{
    BRect rect;
    rect.بائیں = FILL_X(c);
    rect.چوٹی = FILL_Y(r);
    rect.دائیں = rect.بائیں + nc * gui.char_چوڑائی - PEN_چوڑائی;
    rect.bottom = rect.چوٹی + nr * gui.char_اونچائی - PEN_چوڑائی;

    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_متن_Area->سیٹ_Drawing_منوال(B_OP_INVERT);
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->سیٹ_Drawing_منوال(B_OP_COPY);
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    خالی
gui_mch_iconify()
{
    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_کھڑکی->Minimize(سچ);
	gui.vim_کھڑکی->Unlock();
    }
}

#اگر defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    خالی
gui_mch_سیٹ_foreground(خالی)
{
    // TODO
}
#اگر_ختم

/*
 * Set the window title
 */
خالی
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_کھڑکی->سیٹ_Title((حرف *)title);
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    خالی
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_سیٹ_fg_color(color);

    BRect r;
    r.بائیں = FILL_X(gui.ستون);
    r.چوٹی = FILL_Y(gui.صف);
    صحیح cells = utf_off2cells(سطر_انحراف[gui.صف] + gui.ستون, 100); // TODO-TODO
    اگر (cells>=4) cells = 1;
    r.دائیں = r.بائیں + cells*gui.char_چوڑائی - PEN_چوڑائی;
    r.bottom = r.چوٹی + gui.char_اونچائی - PEN_چوڑائی;

    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_متن_Area->StrokeRect(r);
	gui.vim_کھڑکی->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
خالی
gui_mch_draw_part_cursor(
	صحیح	w,
	صحیح	h,
	guicolor_T  color)
{
    gui_mch_سیٹ_fg_color(color);

    BRect r;
    r.بائیں =
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_دایاں ? FILL_X(gui.ستون + 1) - w :
#اگر_ختم
	FILL_X(gui.ستون);
    r.دائیں = r.بائیں + w - PEN_چوڑائی;
    r.bottom = FILL_Y(gui.صف + 1) - PEN_چوڑائی;
    r.چوٹی = r.bottom - h + PEN_چوڑائی;

    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_متن_Area->FillRect(r);
	gui.vim_کھڑکی->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    خالی
gui_mch_update()
{
    gui_mch_flush();
    جب (پورٹ_گنتی(gui.vdcmp) > 0 &&
	    !vim_is_داخلہ_buf_بھرا() &&
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
صحیح
gui_mch_wait_for_chars(
	طویل	wtime)
{
    صحیح		focus;
    bigtime_t	until, timeout;
    حالت_t	st;

    اگر (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_وقت() + timeout;
    }
    ورنہ
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.اندر_focus;
    برائے (;;)
    {
	// Stop or start blinking when focus changes
	اگر (gui.اندر_focus != focus)
	{
	    اگر (gui.اندر_focus)
		gui_mch_آغاز_blink();
	    ورنہ
		gui_mch_stop_blink(TRUE);
	    focus = gui.اندر_focus;
	}

	gui_mch_flush();

#اگر_تعریف_شدہ پیغام_قطار
# ifdef FEAT_TIMERS
	did_جوڑو_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	اگر (did_جوڑو_timer)
	    // Need to recompute the waiting time.
	    توڑو;
# endif
# ifdef FEAT_کام_CHANNEL
	اگر (has_any_channel())
	{
	    اگر (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	ورنہ اگر (wtime < 0)
	    timeout = B_INFINITE_TIMEOUT;
# endif
#اگر_ختم

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use gui_haiku_process_event() to hang until
	 * an event arrives.  No need to check for input_buf_full because we
	 * are returning as soon as it contains a single char.
	 */
	st = gui_haiku_process_event(timeout);

	اگر (داخلہ_دستیاب())
	    واپس OK;
	اگر (st < B_OK)		// includes B_TIMED_OUT
	    واپس FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	اگر (wtime >= 0)
	{
	    timeout = until - system_وقت();
	    اگر (timeout < 0)
		توڑو;
	}
    }
    واپس FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    خالی
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_کھڑکی->Flush();
	gui.vim_کھڑکی->Unlock();
    }
    واپس;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
خالی
gui_mch_clear_بلاک(
	صحیح	row1,
	صحیح	col1,
	صحیح	row2,
	صحیح	col2)
{
    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_متن_Area->mch_Clear_بلاک(row1, col1, row2, col2);
	gui.vim_کھڑکی->Unlock();
    }
}

    خالی
gui_mch_clear_all()
{
    اگر (gui.vim_کھڑکی->تالا_2()) {
	gui.vim_متن_Area->mchClearAll();
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
خالی
gui_mch_delete_سطریں(
	صحیح	صف,
	صحیح	عدد_سطریں)
{
    gui.vim_متن_Area->mch_Delete_سطریں(صف, عدد_سطریں);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
خالی
gui_mch_داخل_کرو_سطریں(
	صحیح	صف,
	صحیح	عدد_سطریں)
{
    gui.vim_متن_Area->mch_داخل_کرو_سطریں(صف, عدد_سطریں);
}

#اگر defined(FEAT_MENU)
/*
 * Menu stuff.
 */

خالی
gui_mch_enable_menu(
	صحیح	flag)
{
    اگر (gui.vim_کھڑکی->تالا_2())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->سیٹ_Enabled(flag);
	gui.vim_کھڑکی->Unlock();
    }
}

خالی
gui_mch_سیٹ_menu_pos(
	صحیح	x,
	صحیح	y,
	صحیح	w,
	صحیح	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
خالی
gui_mch_جوڑو_menu(
	vimmenu_T   *menu,
	صحیح	idx)
{
    vimmenu_T	*والد = menu->والد;

    //	popup menu - just create it unattached
    اگر (menu_is_popup(menu->name) && والد == تہی) {
	BPop_اوپر_Menu* نکالو_اوپر_Menu = نیا BPop_اوپر_Menu((ثابت حرف*)menu->name, جھوٹ, جھوٹ);
	menu->submenu_id = نکالو_اوپر_Menu;
	menu->id = تہی;
	واپس;
    }

    اگر (!menu_is_menubar(menu->name)
	    || (والد != تہی && والد->submenu_id == تہی))
	واپس;

    اگر (gui.vim_کھڑکی->تالا_2())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	اگر ( والد )
	    tmp = والد->submenu_id;
	ورنہ
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	اگر ( ! tmp->ڈھونڈو_شے((ثابت حرف *) menu->dname)) {

	    BMenu *bmenu = نیا BMenu((حرف *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->جوڑو_شے(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->ڈھونڈو_شے((ثابت حرف *) menu->dname);

	}
	gui.vim_کھڑکی->Unlock();
    }
}

    خالی
gui_mch_toggle_tearoffs(صحیح enable)
{
    // no tearoff menus
}

    ساکن BMessage *
Menu_پیغام(vimmenu_T *menu)
{
    BMessage *m = نیا BMessage('menu');
    m->جوڑو_اشارہ_گر("VimMenu", (خالی *)menu);

    واپس m;
}

/*
 * Add a menu item to a menu
 */
خالی
gui_mch_جوڑو_menu_شے(
	vimmenu_T   *menu,
	صحیح	idx)
{
    صحیح	    mnemonic = 0;
    vimmenu_T	*والد = menu->والد;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    اگر (gui.vim_کھڑکی->تالا_2())
    {
#اگر_تعریف_شدہ FEAT_TOOLBAR
	اگر (menu_is_toolbar(والد->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    اگر (toolbar != تہی) {
		toolbar->جوڑو_Button(idx, menu);
	    }
	} ورنہ
#اگر_ختم

	اگر (والد->submenu_id != تہی || menu_is_popup(والد->name)) {
	    اگر (menu_is_separator(menu->name)) {
		BSeparator_شے *شے = نیا BSeparator_شے();
		والد->submenu_id->جوڑو_شے(شے);
		menu->id = شے;
		menu->submenu_id = تہی;
	    }
	    ورنہ {
		BMenu_شے *شے = نیا BMenu_شے((حرف *)menu->dname,
			Menu_پیغام(menu));
		شے->سیٹ_ہدف(gui.vim_متن_Area);
		شے->سیٹ_Trigger((حرف) menu->mnemonic);
		والد->submenu_id->جوڑو_شے(شے);
		menu->id = شے;
		menu->submenu_id = تہی;
	    }
	}
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
خالی
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    اگر (gui.vim_کھڑکی->تالا_2())
    {
#اگر_تعریف_شدہ FEAT_TOOLBAR
	اگر (menu->والد && menu_is_toolbar(menu->والد->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    اگر (toolbar != تہی) {
		toolbar->RemoveButton(menu);
	    }
	} ورنہ
#اگر_ختم
	{
	    أكد(menu->submenu_id == تہی || menu->submenu_id->گنتی_اشیاء() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    اگر (bmenu)
	    {
		bmenu->Remove_شے(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		اگر (bmenu == gui.vimForm->MenuBar() && bmenu->گنتی_اشیاء() == 0)
		{
		    bmenu->Resize_مقصد(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    حذف menu->id;
	    menu->id = تہی;
	    menu->submenu_id = تہی;

	    gui.menu_اونچائی = (صحیح) gui.vimForm->Menu_اونچائی();
	}
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
خالی
gui_mch_menu_grey(
	vimmenu_T   *menu,
	صحیح	grey)
{
#اگر_تعریف_شدہ FEAT_TOOLBAR
    اگر (menu->والد && menu_is_toolbar(menu->والد->name)) {
	اگر (gui.vim_کھڑکی->تالا_2()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    اگر (toolbar != تہی) {
		toolbar->سرمئی_Button(menu, grey);
	    }
	    gui.vim_کھڑکی->Unlock();
	}
    } ورنہ
#اگر_ختم
    اگر (menu->id != تہی)
	menu->id->سیٹ_Enabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
خالی
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	صحیح	hidden)
{
    اگر (menu->id != تہی)
	menu->id->سیٹ_Enabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    خالی
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    خالی
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    اگر (!menu_is_popup(menu->name) || menu->submenu_id == تہی)
	واپس;

    BPop_اوپر_Menu* popupMenu = dynamic_cast<BPop_اوپر_Menu*>(menu->submenu_id);
    اگر (popupMenu == تہی)
	واپس;

    BPoint نقطہ;
    اگر (gui.vim_کھڑکی->تالا_2()) {
	uint32 buttons = 0;
	gui.vim_متن_Area->GetMouse(&نقطہ, &buttons);
	gui.vim_متن_Area->Convert_مقصد_Screen(&نقطہ);
	gui.vim_کھڑکی->Unlock();
    }
    popupMenu->Go(نقطہ, سچ);
}

#اگر_ختم // FEAT_MENU

// Mouse stuff

#اگر_تعریف_شدہ FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
حرف textplain[] = "text/plain";
حرف vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    خالی
clip_mch_request_selection(Clipboard_T *cbd)
{
    اگر (be_clipboard->تالا_2())
    {
	BMessage *m = be_clipboard->ڈیٹا_2();
	// m->PrintToStream();

	char_u *string = تہی;
	ssize_t stringlen = -1;

	اگر (m->ڈھونڈو_ڈیٹا(textplain, B_MIME_TYPE,
		    (ثابت خالی **)&string, &stringlen) == B_OK
		|| m->ڈھونڈو_String("text", (ثابت حرف **)&string) == B_OK)
	{
	    اگر (stringlen == -1)
		stringlen = STRLEN(string);

	    صحیح type;
	    حرف *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    اگر (m->ڈھونڈو_ڈیٹا(vimselectiontype, B_MIME_TYPE,
			(ثابت خالی **)&seltype, &seltypelen) == B_OK)
	    {
		انتخاب (*seltype)
		{
		    طےشدہ:
		    حالت 'L':	type = MLINE;	توڑو;
		    حالت 'C':	type = MCHAR;	توڑو;
#اگر_تعریف_شدہ FEAT_VISUAL
		    حالت 'B':	type = MBLOCK;	توڑو;
#اگر_ختم
		}
	    }
	    ورنہ
	    {
		// Otherwise use heuristic as documented
		type = memchr(string, stringlen, '\n') ? MLINE : MCHAR;
	    }
	    clip_yank_selection(type, string, (طویل)stringlen, cbd);
	}
	be_clipboard->Unlock();
    }
}
/*
 * Make vim the owner of the current selection.
 */
    خالی
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    صحیح
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    واپس FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    خالی
clip_mch_سیٹ_selection(Clipboard_T *cbd)
{
    اگر (be_clipboard->تالا_2())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->ڈیٹا_2();
	أكد(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = تہی;
	long_u	گنتی;
	صحیح type;

	type = clip_convert_selection(&str, &گنتی, cbd);

	اگر (type < 0)
	    واپس;

	m->جوڑو_ڈیٹا(textplain, B_MIME_TYPE, (خالی *)str, گنتی);

	// Add type of selection
	حرف	vtype;
	انتخاب (type)
	{
	    طےشدہ:
	    حالت MLINE:    vtype = 'L';    توڑو;
	    حالت MCHAR:    vtype = 'C';    توڑو;
#اگر_تعریف_شدہ FEAT_VISUAL
	    حالت MBLOCK:   vtype = 'B';    توڑو;
#اگر_ختم
	}
	m->جوڑو_ڈیٹا(vimselectiontype, B_MIME_TYPE, (خالی *)&vtype, 1);

	vim_free(str);

	be_clipboard->Commit();
	be_clipboard->Unlock();
    }
}

#اگر_ختم	// FEAT_CLIPBOARD

#اگر_تعریف_شدہ FEAT_BROWSE
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
	صحیح saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = نیا BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    تہی, تہی, 0, جھوٹ,
	    نیا BMessage((saving == TRUE) ? 'save' : 'open'), تہی, سچ);

    gui.vimApp->f_Browsed_راستہ.Unset();

    gui.vimApp->fFilePanel->کھڑکی()->سیٹ_Title((حرف*)title);
    gui.vimApp->fFilePanel->سیٹ_Panel_Directory((ثابت حرف*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    جب (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = تہی;
    حالت_t نتیجہ = gui.vimApp->f_Browsed_راستہ.InitCheck();
    اگر (نتیجہ == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->f_Browsed_راستہ.راستہ_2());
    } ورنہ
	اگر (نتیجہ != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    نتیجہ, strerror(نتیجہ));
	}

    حذف gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = تہی;

    واپس fileName;
}
#اگر_ختم // FEAT_BROWSE


#اگر defined(FEAT_GUI_DIALOG)

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

صحیح
gui_mch_dialog(
	صحیح	 type,
	char_u	*title,
	char_u	*پیغام_2,
	char_u	*buttons,
	صحیح	 dfltbutton,
	char_u	*textfield,
	صحیح ex_cmd)
{
    VimDialog *dialog = نیا VimDialog(type, (حرف*)title, (حرف*)پیغام_2,
	    (حرف*)buttons, dfltbutton, (حرف*)textfield, ex_cmd);
    منطقی ret = dialog->Go();
    حذف dialog;
	واپس ret;
}

#اگر_ختم // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_مقصد_RGB(pixel);

    واپس ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    خالی
gui_mch_setmouse(صحیح x, صحیح y)
{
    TRACE();
    // TODO
}

#اگر_تعریف_شدہ FEAT_MBYTE_IME
    خالی
im_سیٹ_position(صحیح صف, صحیح ستون)
{
    اگر (gui.vim_کھڑکی->تالا_2())
    {
	gui.vim_متن_Area->DrawIMString();
	gui.vim_کھڑکی->Unlock();
    }
    واپس;
}
#اگر_ختم

    خالی
gui_mch_show_toolbar(صحیح showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_اونچائی = (toolbar && showit) ? toolbar->Toolbar_اونچائی() : 0.;
}

    خالی
gui_mch_سیٹ_toolbar_pos(صحیح x, صحیح y, صحیح w, صحیح h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    اگر (toolbar != تہی) {
	اگر (gui.vim_کھڑکی->تالا_2()) {
	    toolbar->Move_مقصد(x, y);
	    toolbar->Resize_مقصد(w - 1, h - 1);
	    gui.vim_کھڑکی->Unlock();
	}
    }
}

#اگر defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    خالی
gui_mch_show_tabline(صحیح showit)
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();

    اگر (tab_سطر == تہی)
	واپس;

    اگر (!showit != !gui.vimForm->Is_Showing_Tab_سطر()) {
	gui.vimForm->سیٹ_Showing_Tab_سطر(showit != 0);
	gui.tabline_اونچائی = gui.vimForm->Tabline_اونچائی();
    }
}

    خالی
gui_mch_سیٹ_tabline_pos(صحیح x, صحیح y, صحیح w, صحیح h)
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    اگر (tab_سطر != تہی) {
	اگر (gui.vim_کھڑکی->تالا_2()) {
	    tab_سطر->Move_مقصد(x, y);
	    tab_سطر->Resize_مقصد(w - 1, h - 1);
	    gui.vim_کھڑکی->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحیح
gui_mch_showing_tabline()
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    واپس tab_سطر != تہی && gui.vimForm->Is_Showing_Tab_سطر();
}

/*
 * Update the labels of the tabline.
 */
    خالی
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    صحیح	    nr = 0;
    صحیح	    curtabidx = 0;

    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();

    اگر (tab_سطر == تہی)
	واپس;

    gui.vim_کھڑکی->تالا_2();

    // Add a label for each tab page.  They all contain the same text area.
    برائے (tp = پہلا_tabpage; tp != تہی; tp = tp->tp_اگلا, ++nr) {
	اگر (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_سطر->TabAt(nr);

	اگر (tab == تہی) {
	    tab = نیا Vim_Tab_سطر::VimTab();
	    tab_سطر->جوڑو_Tab(تہی, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->سیٹ_Label((ثابت حرف*)NameBuff);
	tab_سطر->Invalidate();
    }

    // Remove any old labels.
    جب (nr < tab_سطر->گنتی_Tabs())
	tab_سطر->RemoveTab(nr);

    اگر (tab_سطر->Selection() != curtabidx)
	tab_سطر->Select(curtabidx);

    gui.vim_کھڑکی->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    خالی
gui_mch_سیٹ_curtab(صحیح nr)
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    اگر (tab_سطر == تہی)
	واپس;

    gui.vim_کھڑکی->تالا_2();

    اگر (tab_سطر->Selection() != nr -1)
	tab_سطر->Select(nr -1);

    gui.vim_کھڑکی->Unlock();
}

#اگر_ختم // FEAT_GUI_TABLINE
