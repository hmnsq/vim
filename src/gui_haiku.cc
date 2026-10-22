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

வெளி "C" {

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <float.h>
#அந்தர்பாவ <syslog.h>

#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#அந்தர்பாவ <Application.h>
#அந்தர்பாவ <Beep.h>
#அந்தர்பாவ <Bitmap.h>
#அந்தர்பாவ <Box.h>
#அந்தர்பாவ <Button.h>
#அந்தர்பாவ <Clipboard.h>
#அந்தர்பாவ <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#அந்தர்பாவ <File.h>
#அந்தர்பாவ <FilePanel.h>
#அந்தர்பாவ <FindDirectory.h>
//#include <Font.h>
#அந்தர்பாவ <IconUtils.h>
#அந்தர்பாவ <Input.h>
#அந்தர்பாவ <ListView.h>
#அந்தர்பாவ <MenuBar.h>
#அந்தர்பாவ <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#அந்தர்பாவ <Path.h>
#அந்தர்பாவ <PictureButton.h>
#அந்தர்பாவ <PopUpMenu.h>
//#include <Region.h>
#அந்தர்பாவ <Resources.h>
//#include <Roster.h>
#அந்தர்பாவ <Screen.h>
#அந்தர்பாவ <ScrollBar.h>
#அந்தர்பாவ <ScrollView.h>
#அந்தர்பாவ <String.h>
#அந்தர்பாவ <StringView.h>
//#include <SupportDefs.h>
#அந்தர்பாவ <TabView.h>
#அந்தர்பாவ <TextControl.h>
#அந்தர்பாவ <TextView.h>
#அந்தர்பாவ <TranslationUtils.h>
#அந்தர்பாவ <TranslatorFormats.h>
#அந்தர்பாவ <View.h>
#அந்தர்பாவ <Window.h>

வகுப்பு VimApp;
வகுப்பு VimFormView;
வகுப்பு VimTextAreaView;
வகுப்பு VimWindow;
வகுப்பு VimToolbar;
வகுப்பு Vim_Tab_வரி;

வெளி சாவி_map *சாவி_Map;
வெளி எழுத்து *சாவி_Map_Chars;

வெளி முழு முதன்மை(முழு argc, எழுத்து **argv);

#ifndef B_MAX_PORT_மொத்தம்
#define B_MAX_PORT_மொத்தம்    255
#endif

// VimApp seems comparable to the X "vimShell"
வகுப்பு VimApp: பொது BApplication
{
    வகைவரை BApplication Inherited;
    பொது:
    VimApp(மாறிலி எழுத்து *appsig);
    ~VimApp();

    // callbacks:
#எனில் 0
    மெய்நிகர் வெறுமை DispatchMessage(BMessage *m, BHandler *h)
    {
	m->PrintToStream();
	Inherited::DispatchMessage(m, h);
    }
#endif
    மெய்நிகர் வெறுமை ReadyToRun();
    மெய்நிகர் வெறுமை ArgvReceived(int32 argc, எழுத்து **argv);
    மெய்நிகர் வெறுமை RefsReceived(BMessage *m);
    மெய்நிகர் தருக்க QuitRequested();
    மெய்நிகர் வெறுமை MessageReceived(BMessage *m);

    நிலையான வெறுமை SendRefs(BMessage *m, தருக்க changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	fBrowsedPath;
    தனிப்பட்ட:
};

வகுப்பு VimWindow: பொது BWindow
{
    வகைவரை BWindow Inherited;
    பொது:
    VimWindow();
    ~VimWindow();

    மெய்நிகர் வெறுமை DispatchMessage(BMessage *m, BHandler *h);
    மெய்நிகர் வெறுமை WindowActivated(தருக்க active);
    மெய்நிகர் தருக்க QuitRequested();

    VimFormView	    *formView;

    தனிப்பட்ட:
    வெறுமை init();

    தருக்க is_fullscreen = பொய்;
    BRect saved_frame;
    window_look saved_look;
};

வகுப்பு VimFormView: பொது BView
{
    வகைவரை BView Inherited;
    பொது:
    VimFormView(BRect frame);
    ~VimFormView();

    // callbacks:
    மெய்நிகர் வெறுமை AllAttached();
    மெய்நிகர் வெறுமை FrameResized(தசம new_width, தசம new_height);

#define MENUBAR_MARGIN	1
    தசம MenuHeight() மாறிலி
    { திரும்பு menuBar ? menuBar->Frame().Height() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() மாறிலி
    { திரும்பு menuBar; }

    தனிப்பட்ட:
    வெறுமை init(BRect);

    BMenuBar	    *menuBar;
    VimTextAreaView *textArea;

#ifdef FEAT_TOOLBAR
    பொது:
    தசம ToolbarHeight() மாறிலி;
    VimToolbar *ToolBar() மாறிலி
    { திரும்பு toolBar; }
    தனிப்பட்ட:
    VimToolbar	    *toolBar;
#endif

#ifdef FEAT_GUI_TABLINE
    பொது:
    Vim_Tab_வரி *Tab_வரி() மாறிலி	{ திரும்பு tab_வரி; }
    தருக்க Is_Showing_Tab_வரி() மாறிலி { திரும்பு showing_Tab_வரி; }
    வெறுமை Set_Showing_Tab_வரி(தருக்க showing) { showing_Tab_வரி = showing;	}
    தசம TablineHeight() மாறிலி;
    தனிப்பட்ட:
    Vim_Tab_வரி	*tab_வரி;
    முழு	showing_Tab_வரி;
#endif
};

வகுப்பு VimTextAreaView: பொது BView
{
    வகைவரை BView Inherited;
    பொது:
    VimTextAreaView(BRect frame);
    ~VimTextAreaView();

    // callbacks:
    மெய்நிகர் வெறுமை Draw(BRect updateRect);
    மெய்நிகர் வெறுமை சாவி_Down(மாறிலி எழுத்து *bytes, int32 இலக்கம்_Bytes);
    மெய்நிகர் வெறுமை MouseDown(BPoint point);
    மெய்நிகர் வெறுமை MouseUp(BPoint point);
    மெய்நிகர் வெறுமை MouseMoved(BPoint point, uint32 transit, மாறிலி BMessage *message);
    மெய்நிகர் வெறுமை MessageReceived(BMessage *m);

    // own functions:
    முழு mchInitFont(char_u *name);
    வெறுமை mchDrawString(முழு row, முழு col, char_u *s, முழு len, முழு flags);
    வெறுமை mchClearBlock(முழு row1, முழு col1, முழு row2, முழு col2);
    வெறுமை mchClearAll();
    வெறுமை mch_Delete_வரிகள்(முழு row, முழு இலக்கம்_வரிகள்);
    வெறுமை mch_செருகு_வரிகள்(முழு row, முழு இலக்கம்_வரிகள்);

    நிலையான வெறுமை guiSendMouseEvent(முழு button, முழு x, முழு y, முழு repeated_click, int_u modifiers);
    நிலையான வெறுமை guiMouseMoved(முழு x, முழு y);
    நிலையான வெறுமை guiBlankMouse(தருக்க should_hide);
    நிலையான int_u mouseModifiersToVim(int32 beModifiers);

    int32 mouse_Drag_Event_மொத்தம்;

#ifdef FEAT_MBYTE_IME
    வெறுமை DrawIMString(வெறுமை);
#endif

    தனிப்பட்ட:
    வெறுமை init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#ifdef FEAT_MBYTE_IME
    கட்டமைப்பு {
	BMessenger* messenger;
	BMessage* message;
	BPoint location;
	முழு row;
	முழு col;
	முழு மொத்தம்;
    } IMData;
#endif
};

வகுப்பு VimScrollBar: பொது BScrollBar
{
    வகைவரை BScrollBar Inherited;
    பொது:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    மெய்நிகர் வெறுமை மதிப்பு_Changed(தசம new_மதிப்பு);
    மெய்நிகர் வெறுமை MouseUp(BPoint where);
    வெறுமை Set_மதிப்பு(தசம newval);
    scrollbar_T *getGsb()
    { திரும்பு gsb; }

    int32	scroll_Event_மொத்தம்;

    தனிப்பட்ட:
    scrollbar_T *gsb;
    தசம   ignore_மதிப்பு;
};


#ifdef FEAT_TOOLBAR

வகுப்பு VimToolbar : பொது BBox
{
    நிலையான BBitmap *normalButtonsBitmap;
    நிலையான BBitmap *grayedButtonsBitmap;

    BBitmap *LoadVimBitmap(மாறிலி எழுத்து* fileName);
    தருக்க GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, தருக்க pressed);
    தருக்க ModifyBitmapToGrayed(BBitmap *bitmap);

    BList fButtonsList;
    வெறுமை InvalidateLayout();

    பொது:
    VimToolbar(BRect frame, மாறிலி எழுத்து * name);
    ~VimToolbar();

    தருக்க PrepareButtonBitmaps();

    தருக்க AddButton(int32 index, vimmenu_T *menu);
    தருக்க RemoveButton(vimmenu_T *menu);
    தருக்க GrayButton(vimmenu_T *menu, முழு grey);

    தசம ToolbarHeight() மாறிலி;
    மெய்நிகர் வெறுமை AttachedToWindow();
};

BBitmap *VimToolbar::normalButtonsBitmap  = வெற்று;
BBitmap *VimToolbar::grayedButtonsBitmap  = வெற்று;

மாறிலி தசம ToolbarMargin = 3.;
மாறிலி தசம ButtonMargin  = 3.;

#endif //FEAT_TOOLBAR

#ifdef FEAT_GUI_TABLINE

வகுப்பு Vim_Tab_வரி : பொது BTabView
{
    பொது:
	வகுப்பு VimTab : பொது BTab {
	    பொது:
		VimTab() : BTab(புதிய BView(BRect(), "-Empty-", 0, 0)) {}

	    மெய்நிகர் வெறுமை Select(BView* owner);
	};

	Vim_Tab_வரி(BRect r) : BTabView(r, "vimTabLine", B_WIDTH_FROM_LABEL,
	       B_FOLLOW_இடது | B_FOLLOW_உச்சி | B_FOLLOW_வலது, B_WILL_DRAW | B_FRAME_EVENTS) {}

    தசம TablineHeight() மாறிலி;
    மெய்நிகர் வெறுமை MouseDown(BPoint point);
};

#endif //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
வகுப்பு VimFont: பொது BFont
{
    வகைவரை BFont Inherited;
    பொது:
    VimFont();
    VimFont(மாறிலி VimFont *rhs);
    VimFont(மாறிலி BFont *rhs);
    VimFont(மாறிலி VimFont &rhs);
    ~VimFont();

    VimFont *next;
    முழு refcount;
    char_u *name;

    தனிப்பட்ட:
    வெறுமை init();
};

#எனில் defined(FEAT_GUI_DIALOG)

வகுப்பு VimDialog : பொது BWindow
{
    வகைவரை BWindow Inherited;

    BButton* _CreateButton(int32 which, மாறிலி எழுத்து* label);

    பொது:

    வகுப்பு View : பொது BView {
	வகைவரை BView Inherited;

	பொது:
	View(BRect frame);
	~View();

	மெய்நிகர் வெறுமை Draw(BRect updateRect);
	வெறுமை InitIcon(int32 type);

	தனிப்பட்ட:
	BBitmap*    fIconBitmap;
    };

    VimDialog(முழு type, மாறிலி எழுத்து *title, மாறிலி எழுத்து *message,
	    மாறிலி எழுத்து *buttons, முழு dfltbutton, மாறிலி எழுத்து *textfield,
	    முழு ex_cmd);
    ~VimDialog();

    முழு Go();

    மெய்நிகர் வெறுமை MessageReceived(BMessage *செய்தி);

    தனிப்பட்ட:
    sem_id	    fDialogSem;
    முழு		    f_Dialog_மதிப்பு;
    BList	    fButtonsList;
    BTextView*	    fMessageView;
    BTextControl*   f_உள்ளீடு_Control;
    மாறிலி எழுத்து*	    f_உள்ளீடு_மதிப்பு;
};

வகுப்பு VimSelectFontDialog : பொது BWindow
{
    வகைவரை BWindow Inherited;

    வெறுமை _CleanList(BListView* list);
    வெறுமை _UpdateFontStyles();
    வெறுமை Update_Size_உள்ளீடு_Preview();
    வெறுமை _UpdateFontPreview();
    தருக்க _UpdateFromListItem(BListView* list, எழுத்து* text, முழு textSize);
    பொது:

    VimSelectFontDialog(font_family* family, font_style* style, தசம* size);
    ~VimSelectFontDialog();

    தருக்க Go();

    மெய்நிகர் வெறுமை MessageReceived(BMessage *செய்தி);

    தனிப்பட்ட:
    status_t	    fStatus;
    sem_id	    fDialogSem;
    தருக்க	    f_Dialog_மதிப்பு;
    font_family*    fFamily;
    font_style*	    fStyle;
    தசம*	    fSize;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    தசம	    fFontSize;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_உள்ளீடு;
};

#endif // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

கட்டமைப்பு MainArgs {
    முழு	     argc;
    எழுத்து    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#define	சாவி_செய்தி_BUFSIZ	7
#எனில் சாவி_செய்தி_BUFSIZ < MAX_சாவி_CODE_LEN
#error Increase சாவி_செய்தி_BUFSIZ!
#endif

கட்டமைப்பு Vim_சாவி_செய்தி {
    char_u  length;
    char_u  chars[சாவி_செய்தி_BUFSIZ];  // contains Vim encoding
    தருக்க    csi_escape;
};

கட்டமைப்பு Vim_Resize_செய்தி {
    முழு	    width;
    முழு	    height;
};

கட்டமைப்பு Vim_Scroll_Bar_செய்தி {
    VimScrollBar *sb;
    long    மதிப்பு;
    முழு	    stillDragging;
};

கட்டமைப்பு Vim_Menu_செய்தி {
    vimmenu_T	*guiMenu;
};

கட்டமைப்பு Vim_Mouse_செய்தி {
    முழு	    button;
    முழு	    x;
    முழு	    y;
    முழு	    repeated_click;
    int_u   modifiers;
};

கட்டமைப்பு Vim_Mouse_Moved_செய்தி {
    முழு	    x;
    முழு	    y;
};

கட்டமைப்பு Vim_Focus_செய்தி {
    தருக்க    active;
};

கட்டமைப்பு Vim_Refs_செய்தி {
    BMessage   *message;
    தருக்க    changedir;
};

கட்டமைப்பு Vim_Tabline_செய்தி {
    முழு	    index;
};

கட்டமைப்பு Vim_Tabline_Menu_செய்தி {
    முழு	    index;
    முழு	    event;
};

கட்டமைப்பு Vim_செய்தி {
    எண்ணிக்கை Vim_செய்தி_Type {
	சாவி_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    ஒன்றிப்பு {
	கட்டமைப்பு Vim_சாவி_செய்தி    சாவி_2;
	கட்டமைப்பு Vim_Resize_செய்தி NewSize;
	கட்டமைப்பு Vim_Scroll_Bar_செய்தி	Scroll;
	கட்டமைப்பு Vim_Menu_செய்தி   Menu;
	கட்டமைப்பு Vim_Mouse_செய்தி  Mouse;
	கட்டமைப்பு Vim_Mouse_Moved_செய்தி	MouseMoved;
	கட்டமைப்பு Vim_Focus_செய்தி  Focus;
	கட்டமைப்பு Vim_Refs_செய்தி   Refs;
	கட்டமைப்பு Vim_Tabline_செய்தி	Tabline;
	கட்டமைப்பு Vim_Tabline_Menu_செய்தி    TablineMenu;
    } u;
};

#define RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#define GUI_TO_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

நிலையான கட்டமைப்பு specialkey
{
    uint16  Be_சாவிகள்;
#define சாவி_3(a,b)    ((a)<<8|(b))
#define K(a)	    சாவி_3(0,a)		// for ASCII codes
#define F(b)	    சாவி_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_சாவிகள்[] =
{
    {K(B_UP_ARROW),	'k', 'u'},
    {K(B_DOWN_ARROW),	    'k', 'd'},
    {K(B_இடது_ARROW),	    'k', 'l'},
    {K(B_வலது_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_செருகு),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_END),		'@', '7'},
    {K(B_PAGE_UP),	'k', 'P'},	// XK_Prior
    {K(B_PAGE_DOWN),	    'k', 'N'},	    // XK_Next,

#define FIRST_FUNCTION_சாவி  11
    {F(B_F1_சாவி),	'k', '1'},
    {F(B_F2_சாவி),	'k', '2'},
    {F(B_F3_சாவி),	'k', '3'},
    {F(B_F4_சாவி),	'k', '4'},
    {F(B_F5_சாவி),	'k', '5'},
    {F(B_F6_சாவி),	'k', '6'},
    {F(B_F7_சாவி),	'k', '7'},
    {F(B_F8_சாவி),	'k', '8'},
    {F(B_F9_சாவி),	'k', '9'},
    {F(B_F10_சாவி),	'k', ';'},

    {F(B_F11_சாவி),	'F', '1'},
    {F(B_F12_சாவி),	'F', '2'},
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
    {F(B_PRINT_சாவி),	    '%', '9'},

#எனில் 0
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

#define இலக்கம்_SPECIAL_சாவிகள்    ARRAY_LENGTH(special_சாவிகள்)

// ---------------- VimApp ----------------

    நிலையான வெறுமை
docd(BPath &path)
{
    mch_chdir((எழுத்து *)path.Path());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	நிலையான வெறுமை
drop_callback(வெறுமை *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	நிலையான வெறுமை
RefsReceived(BMessage *m, தருக்க changedir)
{
    uint32 type;
    int32 மொத்தம்;

    m->PrintToStream();
    தேர்வு (m->what) {
	நிலை B_REFS_RECEIVED:
	நிலை B_SIMPLE_DATA:
	    m->GetInfo("refs", &type, &மொத்தம்);
	    எனில் (type != B_REF_TYPE)
		செல் bad;
	    நிறுத்து;
	நிலை B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &மொத்தம்);
	    எனில் (type != B_STRING_TYPE)
		செல் bad;
	    எனில் (changedir) {
		எழுத்து *dirname;
		எனில் (m->FindString("cwd", (மாறிலி எழுத்து **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    நிறுத்து;
	இயல்பு:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    delete m;
	    திரும்பு;
    }

#ifdef FEAT_VISUAL
    reset_VIsual();
#endif

    char_u  **fnames;
    fnames = (char_u **) alloc(மொத்தம் * அளவு(char_u *));
    முழு fname_index = 0;

    தேர்வு (m->what) {
	நிலை B_REFS_RECEIVED:
	நிலை B_SIMPLE_DATA:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    க்கு (முழு i = 0; i < மொத்தம்; ++i)
	    {
		entry_ref ref;
		எனில் (m->FindRef("refs", i, &ref) == B_OK) {
		    BEntry entry(&ref, பொய்);
		    BPath path;
		    entry.GetPath(&path);

		    // Change to parent directory?
		    எனில் (changedir) {
			BPath parentpath;
			path.GetParent(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    எனில் (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			எனில் (!changedir)
			    docd(path);
		    } இல்லையெனில் {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)path.Path(), IObuff);
			எனில் (fname == வெற்று)
			    fname = (char_u *)path.Path();
			fnames[fname_index++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = பொய்;
		}
	    }
	    நிறுத்து;
	நிலை B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    க்கு (முழு i = 1; i < மொத்தம்; ++i)
	    {
		எழுத்து *fname;

		எனில் (m->FindString("argv", i, (மாறிலி எழுத்து **) &fname) == B_OK) {
		    fnames[fname_index++] = vim_strsave((char_u *)fname);
		}
	    }
	    நிறுத்து;
	இயல்பு:
	    // fprintf(stderr, "case default\n");
	    நிறுத்து;
    }

    delete m;

    // Handle the drop, :edit to get to the file
    எனில் (fname_index > 0) {
	handle_drop(fname_index, fnames, FALSE, drop_callback, வெற்று);

	setcursor();
	out_flush();
    } இல்லையெனில் {
	vim_free(fnames);
    }
}

VimApp::VimApp(மாறிலி எழுத்து *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(வெற்று)
{
}

VimApp::~VimApp()
{
}

    வெறுமை
VimApp::ReadyToRun()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    வெறுமை
VimApp::ArgvReceived(int32 arg_argc, எழுத்து **arg_argv)
{
    எனில் (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	எனில் (gui.vimWindow)
	    gui.vimWindow->Minimize(பொய்);
	BMessage *m = CurrentMessage();
	DetachCurrentMessage();
	SendRefs(m, உண்மை);
    }
}

    வெறுமை
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    முழு limit = 15;
    போது (--limit >= 0 && (curbuf == வெற்று || curbuf->b_start_ffc == 0))
	snooze(100000);    //  0.1 s
    எனில் (gui.vimWindow)
	gui.vimWindow->Minimize(பொய்);
    DetachCurrentMessage();
    SendRefs(m, உண்மை);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    வெறுமை
VimApp::SendRefs(BMessage *m, தருக்க changedir)
{
    Vim_Refs_செய்தி rm;
    rm.message = m;
    rm.changedir = changedir;

    write_port(gui.vdcmp, Vim_செய்தி::Refs, &rm, அளவு(rm));
    //	calls ::RefsReceived
}

    வெறுமை
VimApp::MessageReceived(BMessage *m)
{
    தேர்வு (m->what) {
	நிலை 'save':
	    {
		entry_ref refDirectory;
		m->FindRef("directory", &refDirectory);
		fBrowsedPath.SetTo(&refDirectory);
		BString strName;
		m->FindString("name", &strName);
		fBrowsedPath.Append(strName.String());
	    }
	    நிறுத்து;
	நிலை 'open':
	    {
		entry_ref ref;
		m->FindRef("refs", &ref);
		fBrowsedPath.SetTo(&ref);
	    }
	    நிறுத்து;
	நிலை B_CANCEL:
	    {
		BFilePanel *panel;
		m->FindPointer("source", (வெறுமை**)&panel);
		எனில் (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    நிறுத்து;
	இயல்பு:
	    Inherited::MessageReceived(m);
	    நிறுத்து;
    }
}

    தருக்க
VimApp::QuitRequested()
{
    (வெறுமை)Inherited::QuitRequested();
    திரும்பு பொய்;
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
    எனில் (formView) {
	RemoveChild(formView);
	delete formView;
    }
    gui.vimWindow = வெற்று;
}

    வெறுமை
VimWindow::init()
{
    // Attach the VimFormView
    formView = புதிய VimFormView(Bounds());
    எனில் (formView != வெற்று) {
	AddChild(formView);
    }
}

#எனில் 0  //  disabled in zeta patch
    வெறுமை
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    தேர்வு (m->what) {
	நிலை B_MOUSE_UP:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		எனில் (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->FindPoint("where", &where);
		    v->MouseUp(where);
		} இல்லையெனில் {
		    Inherited::DispatchMessage(m, h);
		}
	    }
	    நிறுத்து;
	இயல்பு:
	    Inherited::DispatchMessage(m, h);
    }
}
#endif

    வெறுமை
VimWindow::WindowActivated(தருக்க active)
{
    Inherited::WindowActivated(active);
    // the textArea gets the keyboard action
    எனில் (active && gui.vimTextArea)
	gui.vimTextArea->MakeFocus(உண்மை);

    கட்டமைப்பு Vim_Focus_செய்தி fm;
    fm.active = active;

    write_port(gui.vdcmp, Vim_செய்தி::Focus, &fm, அளவு(fm));
}

    தருக்க
VimWindow::QuitRequested()
{
    கட்டமைப்பு Vim_சாவி_செய்தி km;
    km.length = 5;
    memcpy((எழுத்து *)km.chars, "\033:qa\r", km.length);
    km.csi_escape = பொய்;
    write_port(gui.vdcmp, Vim_செய்தி::சாவி_2, &km, அளவு(km));
    திரும்பு பொய்;
}
    வெறுமை
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    தருக்க should_propagate = உண்மை;

    தேர்வு (m->what)
    {
	நிலை B_சாவி_DOWN:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->FindInt32("raw_char", &scancode);
		m->FindInt32("modifiers", &beModifiers);

		எனில் (scancode == B_ENTER && (beModifiers & B_இடது_COMMAND_சாவி))
		    {
			should_propagate = பொய்;
			எனில் (இது->is_fullscreen)
			    {
				இது->is_fullscreen = பொய்;
				ResizeTo(இது->saved_frame.Width(), இது->saved_frame.Height());
				MoveTo(இது->saved_frame.இடது, இது->saved_frame.உச்சி);
				SetLook(இது->saved_look);
				SetFlags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } இல்லையெனில் {
				இது->saved_frame = Frame();
				இது->saved_look = Look();
				இது->is_fullscreen = உண்மை;
				BScreen s(இது);
				SetLook(B_NO_BORDER_WINDOW_LOOK);
				ResizeTo(s.Frame().Width() + 1, s.Frame().Height() + 1);
				MoveTo(s.Frame().இடது, s.Frame().உச்சி);
				SetFlags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    எனில் (should_propagate)
	Inherited::DispatchMessage(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect frame):
    BView(frame, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_FRAME_EVENTS),
    menuBar(வெற்று),
#ifdef FEAT_TOOLBAR
    toolBar(வெற்று),
#endif
#ifdef FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_வரி(வெற்று),
#endif
    textArea(வெற்று)
{
    init(frame);
}

VimFormView::~VimFormView()
{
    எனில் (menuBar) {
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
    delete tab_வரி;
#endif

    எனில் (textArea) {
	RemoveChild(textArea);
	delete textArea;
    }
    gui.vimForm = வெற்று;
}

    வெறுமை
VimFormView::init(BRect frame)
{
    menuBar = புதிய BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    AddChild(menuBar);

#ifdef FEAT_TOOLBAR
    toolBar = புதிய VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    AddChild(toolBar);
#endif

#ifdef FEAT_GUI_TABLINE
    tab_வரி = புதிய Vim_Tab_வரி(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    AddChild(tab_வரி);
#endif

    BRect remaining = frame;
    textArea = புதிய VimTextAreaView(remaining);
    AddChild(textArea);
    // The textArea will be resized later when menus are added

    gui.vimForm = இது;
}

#ifdef FEAT_TOOLBAR
    தசம
VimFormView::ToolbarHeight() மாறிலி
{
    திரும்பு toolBar ? toolBar->ToolbarHeight() : 0.;
}
#endif

#ifdef FEAT_GUI_TABLINE
    தசம
VimFormView::TablineHeight() மாறிலி
{
    திரும்பு (tab_வரி && Is_Showing_Tab_வரி()) ? tab_வரி->TablineHeight() : 0.;
}
#endif

    வெறுமை
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    எனில் (menuBar && textArea) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect remaining = Bounds();

#ifdef FEAT_MENU
	remaining.உச்சி += MenuHeight();
	menuBar->ResizeTo(remaining.வலது, remaining.உச்சி);
	gui.menu_height = (முழு) MenuHeight();
#endif

#ifdef FEAT_TOOLBAR
	toolBar->MoveTo(remaining.இடது, remaining.உச்சி);
	toolBar->ResizeTo(remaining.வலது, ToolbarHeight());
	remaining.உச்சி += ToolbarHeight();
	gui.toolbar_height = ToolbarHeight();
#endif

#ifdef FEAT_GUI_TABLINE
	tab_வரி->MoveTo(remaining.இடது, remaining.உச்சி);
	tab_வரி->ResizeTo(remaining.வலது + 1, TablineHeight());
	remaining.உச்சி += TablineHeight();
	gui.tabline_height = TablineHeight();
#endif

	textArea->ResizeTo(remaining.Width(), remaining.Height());
	textArea->MoveTo(remaining.இடது, remaining.உச்சி);
    }


    Inherited::AllAttached();
}

    வெறுமை
VimFormView::FrameResized(தசம new_width, தசம new_height)
{
    கட்டமைப்பு Vim_Resize_செய்தி sm;
    முழு adjust_h, adjust_w;

    new_width += 1;	//  adjust from width to number of pixels occupied
    new_height += 1;

    sm.width = (முழு) new_width;
    sm.height = (முழு) new_height;
    adjust_w = ((முழு)new_width - gui_get_base_width()) % gui.char_width;
    adjust_h = ((முழு)new_height - gui_get_base_height()) % gui.char_height;

    எனில் (adjust_w > 0 || adjust_h > 0) {
	sm.width  -= adjust_w;
	sm.height -= adjust_h;
    }

    write_port(gui.vdcmp, Vim_செய்தி::Resize, &sm, அளவு(sm));
    //	calls gui_resize_shell(new_width, new_height);

    திரும்பு;

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
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_உள்ளீடு_METHOD_AWARE
#இல்லையெனில்
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE
#endif
	),
    mouse_Drag_Event_மொத்தம்(0)
{
#ifdef FEAT_MBYTE_IME
    IMData.messenger = வெற்று;
    IMData.message = வெற்று;
#endif
    init(frame);
}

VimTextAreaView::~VimTextAreaView()
{
    gui.vimTextArea = வெற்று;
}

    வெறுமை
VimTextAreaView::init(BRect frame)
{
    // set up global var for fast access
    gui.vimTextArea = இது;

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

    வெறுமை
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
    Set_கீழ்_Color(rgb);
    FillRect(updateRect, B_SOLID_கீழ்);
    gui_redraw((முழு) updateRect.இடது, (முழு) updateRect.உச்சி,
	    (முழு) (updateRect.Width() + PEN_WIDTH), (முழு) (updateRect.Height() + PEN_WIDTH));

    // Clear the border areas if needed
    Set_கீழ்_Color(rgb);

    எனில் (updateRect.இடது < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.இடது, updateRect.உச்சி,
		    FILL_X(0)-PEN_WIDTH, updateRect.bottom), B_SOLID_கீழ்);
    எனில் (updateRect.உச்சி < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.இடது, updateRect.உச்சி,
		    updateRect.வலது, FILL_Y(0)-PEN_WIDTH), B_SOLID_கீழ்);
    எனில் (updateRect.வலது >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((முழு)Columns), updateRect.உச்சி,
		    updateRect.வலது, updateRect.bottom), B_SOLID_கீழ்);
    எனில் (updateRect.bottom >= FILL_Y(Rows))   //  bottom border
	FillRect(BRect(updateRect.இடது, FILL_Y((முழு)Rows),
		    updateRect.வலது, updateRect.bottom), B_SOLID_கீழ்);

#ifdef FEAT_MBYTE_IME
    DrawIMString();
#endif
}

    வெறுமை
VimTextAreaView::சாவி_Down(மாறிலி எழுத்து *bytes, int32 இலக்கம்_Bytes)
{
    கட்டமைப்பு Vim_சாவி_செய்தி km;
    char_u *dest = km.chars;

    தருக்க canHaveVimModifiers = பொய்;

    BMessage *செய்தி = Window()->CurrentMessage();
    ப்ரதிஜ்ஞா(செய்தி);
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
    எனில் (இலக்கம்_Bytes > 1) {
	// This cannot be a special key
	எனில் (இலக்கம்_Bytes > சாவி_செய்தி_BUFSIZ)
	    இலக்கம்_Bytes = சாவி_செய்தி_BUFSIZ;	    //	should never happen... ???
	km.length = இலக்கம்_Bytes;
	memcpy((எழுத்து *)dest, bytes, இலக்கம்_Bytes);
	km.csi_escape = உண்மை;
    } இல்லையெனில் {
	int32 scancode = 0;
	செய்தி->FindInt32("key", &scancode);

	int32 beModifiers = 0;
	செய்தி->FindInt32("modifiers", &beModifiers);

	char_u string[3];
	முழு len = 0;
	km.length = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	ப்ரதிஜ்ஞா(B_BACKSPACE <= 0x20);
	ப்ரதிஜ்ஞா(B_DELETE == 0x7F);
	எனில் (((char_u)bytes[0] <= 0x20 || (char_u)bytes[0] == 0x7F) &&
		இலக்கம்_Bytes == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    எனில் (beModifiers & B_CONTROL_சாவி) {
		முழு index = சாவி_Map->normal_map[scancode];
		முழு new_இலக்கம்_Bytes = சாவி_Map_Chars[index];
		char_u *newBytes = (char_u *)&சாவி_Map_Chars[index + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		எனில் (new_இலக்கம்_Bytes != 1 || (*newBytes > 0x20 &&
			    *newBytes != 0x7F )) {
		    செல் notspecial;
		}
		bytes = (எழுத்து *)newBytes;
	    }
	    canHaveVimModifiers = உண்மை;

	    uint16 beoskey;
	    முழு first, last;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    எனில் (இலக்கம்_Bytes == 0 || bytes[0] == B_FUNCTION_சாவி) {
		beoskey = F(scancode);
		first = FIRST_FUNCTION_சாவி;
		last = இலக்கம்_SPECIAL_சாவிகள்;
	    } இல்லையெனில் எனில் (*bytes == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		first = last = 0;
	    } இல்லையெனில் {
		beoskey = K(bytes[0]);
		first = 0;
		last = FIRST_FUNCTION_சாவி;
	    }

	    க்கு (முழு i = first; i < last; i++) {
		எனில் (special_சாவிகள்[i].Be_சாவிகள் == beoskey) {
		    string[0] = CSI;
		    string[1] = special_சாவிகள்[i].vim_code0;
		    string[2] = special_சாவிகள்[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	எனில் (len == 0) {
	    string[0] = bytes[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#எனில் 0
	எனில் (len == 3 ||
		bytes[0] == B_SPACE || bytes[0] == B_TAB ||
		bytes[0] == B_RETURN || bytes[0] == '\r' ||
		bytes[0] == B_ESCAPE)
#இல்லையெனில்
	    எனில் (canHaveVimModifiers)
#endif
	    {
		முழு modifiers;
		modifiers = 0;
		எனில் (beModifiers & B_SHIFT_சாவி)
		    modifiers |= MOD_MASK_SHIFT;
		எனில் (beModifiers & B_CONTROL_சாவி)
		    modifiers |= MOD_MASK_CTRL;
		எனில் (beModifiers & B_OPTION_சாவி)
		    modifiers |= MOD_MASK_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		முழு சாவி;
		எனில் (string[0] == CSI && len == 3)
		    சாவி = TO_SPECIAL(string[1], string[2]);
		இல்லையெனில்
		    சாவி = string[0];
		சாவி = simplify_சாவி(சாவி, &modifiers);
		எனில் (IS_SPECIAL(சாவி))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(சாவி);
		    string[2] = K_THIRD(சாவி);
		    len = 3;
		}
		இல்லையெனில்
		{
		    string[0] = சாவி;
		    len = 1;
		}

		எனில் (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.length = 3;
		}
	    }
	memcpy((எழுத்து *)dest, string, len);
	km.length += len;
	km.csi_escape = பொய்;
    }

    write_port(gui.vdcmp, Vim_செய்தி::சாவி_2, &km, அளவு(km));

    /*
     * blank out the pointer if necessary
     */
    எனில் (p_mh && !gui.pointer_hidden)
    {
	guiBlankMouse(உண்மை);
	gui.pointer_hidden = TRUE;
    }
}
வெறுமை
VimTextAreaView::guiSendMouseEvent(
	முழு	button,
	முழு	x,
	முழு	y,
	முழு	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_செய்தி mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_port(gui.vdcmp, Vim_செய்தி::Mouse, &mm, அளவு(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    எனில் (gui.pointer_hidden)
    {
	guiBlankMouse(பொய்);
	gui.pointer_hidden = FALSE;
    }
}

வெறுமை
VimTextAreaView::guiMouseMoved(
	முழு	x,
	முழு	y)
{
    Vim_Mouse_Moved_செய்தி mm;

    mm.x = x;
    mm.y = y;

    write_port(gui.vdcmp, Vim_செய்தி::MouseMoved, &mm, அளவு(mm));

    எனில் (gui.pointer_hidden)
    {
	guiBlankMouse(பொய்);
	gui.pointer_hidden = FALSE;
    }
}

    வெறுமை
VimTextAreaView::guiBlankMouse(தருக்க should_hide)
{
    எனில் (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } இல்லையெனில் {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
VimTextAreaView::mouseModifiersToVim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    எனில் (beModifiers & B_SHIFT_சாவி)
	vim_modifiers |= MOUSE_SHIFT;
    எனில் (beModifiers & B_CONTROL_சாவி)
	vim_modifiers |= MOUSE_CTRL;
    எனில் (beModifiers & B_OPTION_சாவி)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    திரும்பு vim_modifiers;
}

    வெறுமை
VimTextAreaView::MouseDown(BPoint point)
{
    BMessage *m = Window()->CurrentMessage();
    ப்ரதிஜ்ஞா(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    முழு vimButton;

    எனில் (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_இடது;
    இல்லையெனில் எனில் (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_வலது;
    இல்லையெனில் எனில் (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_நடு;
    இல்லையெனில்
	திரும்பு;		// Unknown button

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

    வெறுமை
VimTextAreaView::MouseUp(BPoint point)
{
    vimMouseButton = 0;

    BMessage *m = Window()->CurrentMessage();
    ப்ரதிஜ்ஞா(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->FindInt32("modifiers", &modifiers);

    vimMouseModifiers = mouseModifiersToVim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, point.x, point.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::MouseUp(point);
}

    வெறுமை
VimTextAreaView::MouseMoved(BPoint point, uint32 transit, மாறிலி BMessage *message)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    எனில் (gui.pointer_hidden)
    {
	guiBlankMouse(பொய்);
	gui.pointer_hidden = FALSE;
    }

    எனில் (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(point.x, point.y);
	திரும்பு;
    }

    atomic_add(&mouse_Drag_Event_மொத்தம், 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, point.x, point.y, 0, vimMouseModifiers);
}

    வெறுமை
VimTextAreaView::MessageReceived(BMessage *m)
{
    தேர்வு (m->what) {
	நிலை 'menu':
	    {
		Vim_Menu_செய்தி mm;
		mm.guiMenu = வெற்று;  // in case no pointer in msg
		m->FindPointer("VimMenu", (வெறுமை **)&mm.guiMenu);
		write_port(gui.vdcmp, Vim_செய்தி::Menu, &mm, அளவு(mm));
	    }
	    நிறுத்து;
	நிலை B_MOUSE_WHEEL_CHANGED:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		தசம small=0, big=0, dy=0;
		m->FindFloat("be:wheel_delta_y", &dy);
		scb->GetSteps(&small, &big);
		scb->Set_மதிப்பு(scb->மதிப்பு_2()+small*dy*3);
		scb->மதிப்பு_Changed(scb->மதிப்பு_2());
#எனில் 0
		scb = curwin->w_scrollbars[0].id;
		scb->GetSteps(&small, &big);
		scb->Set_மதிப்பு(scb->மதிப்பு_2()+small*dy);
		scb->மதிப்பு_Changed(scb->மதிப்பு_2());
#endif
	    }
	    நிறுத்து;
#ifdef FEAT_MBYTE_IME
	நிலை B_உள்ளீடு_METHOD_EVENT:
	    {
		int32 opcode;
		m->FindInt32("be:opcode", &opcode);
		தேர்வு(opcode)
		{
		    நிலை B_உள்ளீடு_METHOD_STARTED:
			எனில் (!IMData.messenger) delete IMData.messenger;
			IMData.messenger = புதிய BMessenger();
			m->FindMessenger("be:reply_to", IMData.messenger);
			நிறுத்து;
		    நிலை B_உள்ளீடு_METHOD_CHANGED:
			{
			    BString str;
			    தருக்க confirmed;
			    எனில் (IMData.message) *(IMData.message) = *m;
			    இல்லையெனில்	       IMData.message = புதிய BMessage(*m);
			    DrawIMString();
			    m->FindBool("be:confirmed", &confirmed);
			    எனில் (confirmed)
			    {
				m->FindString("be:string", &str);
				char_u *chars = (char_u*)str.String();
				கட்டமைப்பு Vim_சாவி_செய்தி km;
				km.csi_escape = உண்மை;
				முழு clen;
				முழு i = 0;
				போது (i < str.Length())
				{
				    clen = utf_ptr2len(chars+i);
				    memcpy(km.chars, chars+i, clen);
				    km.length = clen;
				    write_port(gui.vdcmp, Vim_செய்தி::சாவி_2, &km, அளவு(km));
				    i += clen;
				}
			    }
			}
			நிறுத்து;
		    நிலை B_உள்ளீடு_METHOD_LOCATION_REQUEST:
			{
			    BMessage செய்தி(B_உள்ளீடு_METHOD_EVENT);
			    செய்தி.AddInt32("be:opcode", B_உள்ளீடு_METHOD_LOCATION_REQUEST);
			    செய்தி.AddPoint("be:location_reply", IMData.location);
			    செய்தி.AddFloat("be:height_reply", FILL_Y(1));
			    IMData.messenger->SendMessage(&செய்தி);
			}
			நிறுத்து;
		    நிலை B_உள்ளீடு_METHOD_STOPPED:
			delete IMData.messenger;
			delete IMData.message;
			IMData.messenger = வெற்று;
			IMData.message = வெற்று;
			நிறுத்து;
		}
	    }
	    // TODO: sz: break here???
#endif
	இயல்பு:
	    எனில் (m->WasDropped()) {
		BWindow *w = Window();
		w->DetachCurrentMessage();
		w->Minimize(பொய்);
		VimApp::SendRefs(m, (modifiers() & B_SHIFT_சாவி) != 0);
	    } இல்லையெனில் {
		Inherited::MessageReceived(m);
	    }
	    நிறுத்து;
    }
}

    முழு
VimTextAreaView::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    எனில் (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_set_font((GuiFont)newFont);
	எனில் (name && STRCMP(name, "*") != 0)
	    hl_set_font_name(name);

	SetDrawingMode(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	திரும்பு OK;
    }
    திரும்பு FAIL;
}

    வெறுமை
VimTextAreaView::mchDrawString(முழு row, முழு col, char_u *s, முழு len, முழு flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    எனில் (!(flags & DRAW_TRANSP)) {
	முழு cells;
	cells = 0;
	க்கு (முழு i=0; i<len; i++) {
	    முழு cn = utf_ptr2cells((char_u *)(s+i));
	    எனில் (cn<4) cells += cn;
	}

	BRect r(FILL_X(col), FILL_Y(row),
		FILL_X(col + cells) - PEN_WIDTH, FILL_Y(row + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_கீழ்);
    }

    BFont font;
    இது->GetFont(&font);
    எனில் (!font.IsFixed())
    {
	எழுத்து* p = (எழுத்து*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	முழு cells;
	போது ((p - (எழுத்து*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.Set(TEXT_X(col+lastpos), TEXT_Y(row));
	    DrawString(p, clen, where);
	    எனில் (flags & DRAW_BOLD) {
		where.x += 1.0;
		SetDrawingMode(B_OP_BLEND);
		DrawString(p, clen, where);
		SetDrawingMode(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    எனில் (cells<4) lastpos += cells;
	    இல்லையெனில்	lastpos++;
	    p += clen;
	}
    }
    இல்லையெனில்
    {
	BPoint where(TEXT_X(col), TEXT_Y(row));
	DrawString((எழுத்து*)s, len, where);
	எனில் (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    SetDrawingMode(B_OP_BLEND);
	    DrawString((எழுத்து*)s, len, where);
	    SetDrawingMode(B_OP_COPY);
	}
    }

    எனில் (flags & DRAW_UNDERL) {
	முழு cells;
	cells = 0;
	க்கு (முழு i=0; i<len; i++) {
	    முழு cn = utf_ptr2cells((char_u *)(s+i));
	    எனில் (cn<4) cells += cn;
	}

	BPoint start(FILL_X(col), FILL_Y(row + 1) - PEN_WIDTH);
	BPoint end(FILL_X(col + cells) - PEN_WIDTH, start.y);

	Stroke_வரி(start, end);
    }
}

வெறுமை
VimTextAreaView::mchClearBlock(
	முழு	row1,
	முழு	col1,
	முழு	row2,
	முழு	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_WIDTH, FILL_Y(row2 + 1) - PEN_WIDTH);
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(r, B_SOLID_கீழ்);
}

    வெறுமை
VimTextAreaView::mchClearAll()
{
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(Bounds(), B_SOLID_கீழ்);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    வெறுமை
VimTextAreaView::mch_Delete_வரிகள்(முழு row, முழு இலக்கம்_வரிகள்)
{
    BRect source, dest;
    source.இடது = FILL_X(gui.scroll_region_இடது);
    source.உச்சி = FILL_Y(row + இலக்கம்_வரிகள்);
    source.வலது = FILL_X(gui.scroll_region_வலது + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    dest.இடது = FILL_X(gui.scroll_region_இடது);
    dest.உச்சி = FILL_Y(row);
    dest.வலது = FILL_X(gui.scroll_region_வலது + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot - இலக்கம்_வரிகள் + 1) - PEN_WIDTH;

    எனில் (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(gui.scroll_region_bot - இலக்கம்_வரிகள் + 1,
		gui.scroll_region_இடது,
		gui.scroll_region_bot, gui.scroll_region_வலது);


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
    வெறுமை
VimTextAreaView::mch_செருகு_வரிகள்(முழு row, முழு இலக்கம்_வரிகள்)
{
    BRect source, dest;

    // XXX Attempt at a hack:
    gui.vimWindow->UpdateIfNeeded();
    source.இடது = FILL_X(gui.scroll_region_இடது);
    source.உச்சி = FILL_Y(row);
    source.வலது = FILL_X(gui.scroll_region_வலது + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot - இலக்கம்_வரிகள் + 1) - PEN_WIDTH;

    dest.இடது = FILL_X(gui.scroll_region_இடது);
    dest.உச்சி = FILL_Y(row + இலக்கம்_வரிகள்);
    dest.வலது = FILL_X(gui.scroll_region_வலது + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    எனில் (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(row, gui.scroll_region_இடது,
		row + இலக்கம்_வரிகள் - 1, gui.scroll_region_வலது);

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
வெறுமை VimTextAreaView::DrawIMString(வெறுமை)
{
    நிலையான மாறிலி rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    மாறிலி எழுத்து* s;
    முழு len;
    BMessage* செய்தி = IMData.message;
    எனில் (!செய்தி)
	திரும்பு;
    gui_redraw_block(IMData.row, 0,
	    IMData.row + IMData.மொத்தம், W_WIDTH(curwin), GUI_MON_NOCLEAR);
    தருக்க confirmed = பொய்;
    செய்தி->FindBool("be:confirmed", &confirmed);
    எனில் (confirmed)
	திரும்பு;
    rgb_color hcolor = உயர்வு_Color(), lcolor = கீழ்_Color();
    செய்தி->FindString("be:string", &str);
    s = str.String();
    len = str.Length();
    Set_உயர்வு_Color(0, 0, 0);
    IMData.row = gui.row;
    IMData.col = gui.col;
    int32 sel_start = 0, sel_end = 0;
    செய்தி->FindInt32("be:selection", 0, &sel_start);
    செய்தி->FindInt32("be:selection", 1, &sel_end);
    முழு clen, cn;
    BPoint pos(IMData.col, 0);
    BRect r;
    BPoint where;
    IMData.location = ConvertToScreen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
    க்கு (முழு i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	எனில் (pos.x + cn > W_WIDTH(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	எனில் (sel_start<=i && i<sel_end)
	{
	    Set_கீழ்_Color(r_highlight);
	    IMData.location = ConvertToScreen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
	}
	இல்லையெனில்
	{
	    Set_கீழ்_Color(b_highlight);
	}
	r.Set(FILL_X(pos.x), FILL_Y(IMData.row + pos.y),
		FILL_X(pos.x + cn) - PEN_WIDTH,
		FILL_Y(IMData.row + pos.y + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_கீழ்);
	where.Set(TEXT_X(pos.x), TEXT_Y(IMData.row + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.மொத்தம் = (முழு)pos.y;

    Set_உயர்வு_Color(hcolor);
    Set_கீழ்_Color(lcolor);
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
	    "vim scrollbar", (BView *)வெற்று,
	    0.0, 10.0, posture),
    ignore_மதிப்பு(-1),
    scroll_Event_மொத்தம்(0)
{
    gsb = g;
    SetResizingMode(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    வெறுமை
VimScrollBar::மதிப்பு_Changed(தசம new_மதிப்பு)
{
    எனில் (ignore_மதிப்பு >= 0.0 && new_மதிப்பு == ignore_மதிப்பு) {
	ignore_மதிப்பு = -1;
	திரும்பு;
    }
    ignore_மதிப்பு = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_add(&scroll_Event_மொத்தம், 1);

    கட்டமைப்பு Vim_Scroll_Bar_செய்தி sm;

    sm.sb = இது;
    sm.மதிப்பு = (long) new_மதிப்பு;
    sm.stillDragging = TRUE;

    write_port(gui.vdcmp, Vim_செய்தி::ScrollBar, &sm, அளவு(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    வெறுமை
VimScrollBar::MouseUp(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_add(&scroll_Event_மொத்தம், 1);

    கட்டமைப்பு Vim_Scroll_Bar_செய்தி sm;

    sm.sb = இது;
    sm.மதிப்பு = (long) மதிப்பு_2();
    sm.stillDragging = FALSE;

    write_port(gui.vdcmp, Vim_செய்தி::ScrollBar, &sm, அளவு(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::MouseUp(where);
}

    வெறுமை
VimScrollBar::Set_மதிப்பு(தசம new_மதிப்பு)
{
    எனில் (new_மதிப்பு == மதிப்பு_2())
	திரும்பு;

    ignore_மதிப்பு = new_மதிப்பு;
    Inherited::Set_மதிப்பு(new_மதிப்பு);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(மாறிலி VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(மாறிலி BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(மாறிலி VimFont &rhs): BFont(rhs)
{
    init();
}

VimFont::~VimFont()
{
}

    வெறுமை
VimFont::init()
{
    next = வெற்று;
    refcount = 1;
    name = வெற்று;
}

// ---------------- VimDialog ----------------

#எனில் defined(FEAT_GUI_DIALOG)

மாறிலி unsigned முழு  k_Vim_Dialog_Button_செய்தி = 'VMDB';
மாறிலி unsigned முழு  kVimDialogIconStripeWidth = 30;
மாறிலி unsigned முழு  kVimDialogButtonsSpacingX = 9;
மாறிலி unsigned முழு  kVimDialogButtonsSpacingY = 4;
மாறிலி unsigned முழு  kVimDialogSpacingX = 6;
மாறிலி unsigned முழு  kVimDialogSpacingY = 10;
மாறிலி unsigned முழு  kVimDialogMinimalWidth  = 310;
மாறிலி unsigned முழு  kVimDialogMinimalHeight = 75;
மாறிலி BRect	    kDefaultRect =
BRect(0, 0, kVimDialogMinimalWidth, kVimDialogMinimalHeight);

VimDialog::VimDialog(முழு type, மாறிலி எழுத்து *title, மாறிலி எழுத்து *message,
	மாறிலி எழுத்து *buttons, முழு dfltbutton, மாறிலி எழுத்து *textfield, முழு ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_மதிப்பு(dfltbutton)
    , fMessageView(வெற்று)
    , f_உள்ளீடு_Control(வெற்று)
    , f_உள்ளீடு_மதிப்பு(textfield)
{
    //	master view
    VimDialog::View* view = புதிய VimDialog::View(Bounds());
    எனில் (view == வெற்று)
	திரும்பு;

    எனில் (title == வெற்று)
	SetTitle("Vim " VIM_VERSION_MEDIUM);

    AddChild(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    தசம maxButtonWidth  = 0;
    தசம maxButtonHeight = 0;
    தசம buttonsWidth	  = 0;
    தசம buttonsHeight   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    செய்
    {
	int32 end = strButtons.FindFirst('\n');
	எனில் (end != B_ERROR)
	    strButtons.SetByteAt(end, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->AddChild(button);
	fButtonsList.AddItem(button);

	maxButtonWidth	= max_c(maxButtonWidth,  button->Bounds().Width());
	maxButtonHeight = max_c(maxButtonHeight, button->Bounds().Height());
	buttonsWidth   += button->Bounds().Width();
	buttonsHeight  += button->Bounds().Height();

	எனில் (end == B_ERROR)
	    நிறுத்து;

	strButtons.Remove(0, end + 1);
    } போது (உண்மை);

    int32 buttons_மொத்தம் = fButtonsList.மொத்தம்_பொருட்கள்();
    buttonsWidth      += kVimDialogButtonsSpacingX * (buttons_மொத்தம் - 1);
    buttonsHeight     += kVimDialogButtonsSpacingY * (buttons_மொத்தம் - 1);
    தசம dialogWidth  = buttonsWidth + kVimDialogIconStripeWidth +
	kVimDialogSpacingX * 2;
    தசம dialogHeight = maxButtonHeight + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    தருக்க vertical = (vim_strchr(p_go, GO_VERTICAL) != வெற்று) ||
	dialogWidth >= gui.vimWindow->Bounds().Width();
    எனில் (vertical) {
	dialogWidth  -= buttonsWidth;
	dialogWidth  += maxButtonWidth;
	dialogHeight -= maxButtonHeight;
	dialogHeight += buttonsHeight;
    }

    dialogWidth  = max_c(dialogWidth,  kVimDialogMinimalWidth);

    //	message view
    BRect rect(0, 0, dialogWidth, 0);
    rect.இடது  += kVimDialogIconStripeWidth + 16 + kVimDialogSpacingX;
    rect.உச்சி   += kVimDialogSpacingY;
    rect.வலது -= kVimDialogSpacingX;
    rect.bottom = rect.உச்சி;
    fMessageView = புதிய BTextView(rect, "_tv_", rect.OffsetByCopy(B_ORIGIN),
	    B_FOLLOW_இடது | B_FOLLOW_உச்சி, B_WILL_DRAW);

    fMessageView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color textColor = ui_color(B_PANEL_TEXT_COLOR);
    fMessageView->SetFontAndColor(be_plain_font, B_FONT_ALL, &textColor);
    fMessageView->SetText(message);
    fMessageView->MakeEditable(பொய்);
    fMessageView->MakeSelectable(பொய்);
    fMessageView->SetWordWrap(உண்மை);
    AddChild(fMessageView);

    தசம messageHeight = fMessageView->TextHeight(0, fMessageView->மொத்தம்_வரிகள்());
    fMessageView->ResizeBy(0, messageHeight);
    fMessageView->SetTextRect(BRect(0, 0, rect.Width(), messageHeight));

    dialogHeight += messageHeight;

    //	input view
    எனில் (f_உள்ளீடு_மதிப்பு != வெற்று) {
	rect.உச்சி     =
	    rect.bottom += messageHeight + kVimDialogSpacingY;
	f_உள்ளீடு_Control = புதிய BTextControl(rect, "_iv_", வெற்று, f_உள்ளீடு_மதிப்பு, வெற்று,
		B_FOLLOW_இடது | B_FOLLOW_உச்சி, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_உள்ளீடு_Control->TextView()->SetText(f_உள்ளீடு_மதிப்பு);
	f_உள்ளீடு_Control->TextView()->SetWordWrap(பொய்);
	AddChild(f_உள்ளீடு_Control);

	தசம width = 0.f, height = 0.f;
	f_உள்ளீடு_Control->GetPreferredSize(&width, &height);
	f_உள்ளீடு_Control->MakeFocus(உண்மை);

	dialogHeight += height + kVimDialogSpacingY * 1.5;
    }

    dialogHeight = max_c(dialogHeight, kVimDialogMinimalHeight);

    ResizeTo(dialogWidth, dialogHeight);
    MoveTo((gui.vimWindow->Bounds().Width() - dialogWidth) / 2,
	    (gui.vimWindow->Bounds().Height() - dialogHeight) / 2);

    //	adjust layout of buttons
    தசம buttonWidth = max_c(maxButtonWidth, rect.Width() * 0.66);
    BPoint origin(dialogWidth, dialogHeight);
    origin.x -= kVimDialogSpacingX + (vertical ? buttonWidth : buttonsWidth);
    origin.y -= kVimDialogSpacingY + (vertical ? buttonsHeight	: maxButtonHeight);

    க்கு (int32 i = 0 ; i < buttons_மொத்தம்; i++) {
	BButton *button = (BButton*)fButtonsList.ItemAt(i);
	button->MoveTo(origin);
	எனில் (vertical) {
	    origin.y += button->Frame().Height() + kVimDialogButtonsSpacingY;
	    button->ResizeTo(buttonWidth, button->Frame().Height());
	} இல்லையெனில்
	    origin.x += button->Frame().Width() + kVimDialogButtonsSpacingX;

	எனில் (dfltbutton == i + 1) {
	    button->MakeDefault(உண்மை);
	    button->MakeFocus(f_உள்ளீடு_Control == வெற்று);
	}
    }
}

VimDialog::~VimDialog()
{
    எனில் (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    முழு
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    எனில் (fDialogSem < B_OK) {
	Quit();
	திரும்பு f_Dialog_மதிப்பு;
    }

    Show();

    போது (acquire_sem(fDialogSem) == B_INTERRUPTED);

    முழு ret_மதிப்பு = f_Dialog_மதிப்பு;
    எனில் (f_உள்ளீடு_மதிப்பு != வெற்று)
	vim_strncpy((char_u*)f_உள்ளீடு_மதிப்பு, (char_u*)f_உள்ளீடு_Control->Text(), IOSIZE - 1);

    எனில் (Lock())
	Quit();

    திரும்பு ret_மதிப்பு;
}

வெறுமை VimDialog::MessageReceived(BMessage *செய்தி)
{
    int32 which = 0;
    எனில் (செய்தி->what != k_Vim_Dialog_Button_செய்தி ||
	    செய்தி->FindInt32("which", &which) != B_OK)
	திரும்பு BWindow::MessageReceived(செய்தி);

    f_Dialog_மதிப்பு = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, மாறிலி எழுத்து* label)
{
    BMessage *message = புதிய BMessage(k_Vim_Dialog_Button_செய்தி);
    message->AddInt32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = புதிய BButton(rect, name.String(), label, message,
	    B_FOLLOW_வலது | B_FOLLOW_BOTTOM);

    தசம width = 0.f, height = 0.f;
    button->GetPreferredSize(&width, &height);
    button->ResizeTo(width, height);

    திரும்பு button;
}

VimDialog::View::View(BRect frame)
    :	BView(frame, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(வெற்று)
{
    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    delete fIconBitmap;
}

வெறுமை VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.வலது = kVimDialogIconStripeWidth;
    Set_உயர்வு_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    எனில் (fIconBitmap == வெற்று)
	திரும்பு;

    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

வெறுமை VimDialog::View::InitIcon(int32 type)
{
    எனில் (type == VIM_GENERIC)
	திரும்பு;

    BPath path;
    status_t status = find_directory(B_BEOS_SERVERS_DIRECTORY, &path);
    எனில் (status != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(status));
	திரும்பு;
    }

    path.Append("app_server");

    BFile file(path.Path(), O_RDONLY);
    எனில் (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	திரும்பு;
    }

    BResources resources(&file);
    எனில் (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	திரும்பு;
    }

    மாறிலி எழுத்து *name = "";
    தேர்வு(type) {
	நிலை VIM_ERROR:	    name = "stop"; நிறுத்து;
	நிலை VIM_WARNING:   name = "warn"; நிறுத்து;
	நிலை VIM_INFO:	    name = "info"; நிறுத்து;
	நிலை VIM_QUESTION:  name = "idea"; நிறுத்து;
	இயல்பு: திரும்பு;
    }

    int32 iconSize = 32;
    fIconBitmap = புதிய BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1), 0, B_RGBA32);
    எனில் (fIconBitmap == வெற்று || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == வெற்று) ? "null" : strerror(fIconBitmap->InitCheck()));
	திரும்பு;
    }

    size_t size = 0;
    மாறிலி uint8* iconData = வெற்று;
    //	try vector icon first?
    iconData = (மாறிலி uint8*)resources.LoadResource(B_VECTOR_ICON_TYPE, name, &size);
    எனில் (iconData != வெற்று && BIconUtils::GetVectorIcon(iconData, size, fIconBitmap) == B_OK)
	திரும்பு;

    //	try bitmap icon now
    iconData = (மாறிலி uint8*)resources.LoadResource(B_LARGE_ICON_TYPE, name, &size);
    எனில் (iconData == வெற்று) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	delete fIconBitmap;
	fIconBitmap = வெற்று;
	திரும்பு;
    }

    எனில் (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(iconData, iconSize, iconSize, iconSize, fIconBitmap);
}

மாறிலி unsigned முழு  k_Vim_Dialog_OKButton_செய்தி = 'FDOK';
மாறிலி unsigned முழு  k_Vim_Dialog_Cancel_Button_செய்தி = 'FDCN';
மாறிலி unsigned முழு  k_Vim_Dialog_Size_உள்ளீடு_செய்தி = 'SICH';
மாறிலி unsigned முழு  k_Vim_Dialog_Family_Select_செய்தி = 'MSFM';
மாறிலி unsigned முழு  k_Vim_Dialog_Style_Select_செய்தி = 'MSST';
மாறிலி unsigned முழு  k_Vim_Dialog_Size_Select_செய்தி = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, தசம* size)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fStatus(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_மதிப்பு(பொய்)
    , fFamily(family)
    , fStyle(style)
    , fSize(size)
    , fFontSize(*size)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_உள்ளீடு(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_LENGTH);
    strncpy(fFontStyle, *style, B_FONT_STYLE_LENGTH);

    //	"client" area view
    BBox *clientBox = புதிய BBox(Bounds(), B_EMPTY_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    AddChild(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.இடது_உச்சி(), RC.இடது_உச்சி());

    //	at first create all controls
    fPreview = புதிய BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->AddChild(fPreview);

    BBox* boxDivider = புதிய BBox(rc, B_EMPTY_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->AddChild(boxDivider);

    BStringView *labelFamily = புதிய BStringView(rc, "labelFamily", "Family:");
    clientBox->AddChild(labelFamily);
    labelFamily->ResizeToPreferred();

    BStringView *labelStyle = புதிய BStringView(rc, "labelStyle", "Style:");
    clientBox->AddChild(labelStyle);
    labelStyle->ResizeToPreferred();

    BStringView *labelSize = புதிய BStringView(rc, "labelSize", "Size:");
    clientBox->AddChild(labelSize);
    labelSize->ResizeToPreferred();

    fFamiliesList = புதிய BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = புதிய BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_இடது_வலது, 0, பொய், உண்மை);
    clientBox->AddChild(scrollFamilies);

    fStylesList= புதிய BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = புதிய BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_இடது_வலது, 0, பொய், உண்மை);
    clientBox->AddChild(scrollStyles);

    f_Sizes_உள்ளீடு = புதிய BTextControl(rc, "inputSize", வெற்று, "???",
	    புதிய BMessage(k_Vim_Dialog_Size_உள்ளீடு_செய்தி));
    clientBox->AddChild(f_Sizes_உள்ளீடு);
    f_Sizes_உள்ளீடு->ResizeToPreferred();

    fSizesList = புதிய BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = புதிய BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_இடது_வலது, 0, பொய், உண்மை);
    clientBox->AddChild(scrollSizes);

    BButton *buttonOK = புதிய BButton(rc, "buttonOK", "OK",
			புதிய BMessage(k_Vim_Dialog_OKButton_செய்தி));
    clientBox->AddChild(buttonOK);
    buttonOK->ResizeToPreferred();

    BButton *buttonCancel = புதிய BButton(rc, "buttonCancel", "Cancel",
			புதிய BMessage(k_Vim_Dialog_Cancel_Button_செய்தி));
    clientBox->AddChild(buttonCancel);
    buttonCancel->ResizeToPreferred();

    //	layout controls
    தசம வரி_Height = labelFamily->Bounds().Height();
    தசம previewHeight = வரி_Height * 3;
    தசம offsetYLabels = previewHeight + kVimDialogSpacingY;
    தசம offsetYLists = offsetYLabels + வரி_Height + kVimDialogSpacingY / 2;
    தசம offsetYSizes = offsetYLists + f_Sizes_உள்ளீடு->Bounds().Height() + kVimDialogSpacingY / 2;
    தசம listsHeight = வரி_Height * 9;
    தசம offsetYButtons = offsetYLists + listsHeight +  kVimDialogSpacingY;
    தசம maxControlsHeight = offsetYButtons + buttonOK->Bounds().Height();
    தசம familiesWidth = labelFamily->Bounds().Width() * 5;
    தசம offsetXStyles = familiesWidth + kVimDialogSpacingX;
    தசம stylesWidth = labelStyle->Bounds().Width() * 4;
    தசம offsetXSizes = offsetXStyles + stylesWidth + kVimDialogSpacingX;
    தசம sizesWidth = labelSize->Bounds().Width() * 2;
    தசம maxControlsWidth = offsetXSizes + sizesWidth;

    ResizeTo(maxControlsWidth + kVimDialogSpacingX * 2,
	maxControlsHeight + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vimWindow->Frame();
    MoveTo(rcVim.இடது + (rcVim.Width() - Frame().Width()) / 2,
	    rcVim.உச்சி + (rcVim.Height() - Frame().Height()) / 2);

    fPreview->ResizeTo(maxControlsWidth, previewHeight);
    fPreview->SetAlignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, previewHeight + kVimDialogSpacingY / 2);
    boxDivider->ResizeTo(maxControlsWidth, 1.f);

    labelFamily->MoveBy(0.f, offsetYLabels);
    labelStyle->MoveBy(offsetXStyles, offsetYLabels);
    labelSize->MoveBy(offsetXSizes, offsetYLabels);

    //	text control alignment issues
    தசம insetX = f_Sizes_உள்ளீடு->TextView()->Bounds().Width() - f_Sizes_உள்ளீடு->Bounds().Width();
    தசம insetY = f_Sizes_உள்ளீடு->TextView()->Bounds().Width() - f_Sizes_உள்ளீடு->Bounds().Width();

    scrollFamilies->MoveBy(0.f, offsetYLists);
    scrollStyles->MoveBy(offsetXStyles, offsetYLists);
    f_Sizes_உள்ளீடு->MoveBy(offsetXSizes + insetX / 2, offsetYLists + insetY / 2);
    scrollSizes->MoveBy(offsetXSizes, offsetYSizes);

    f_Sizes_உள்ளீடு->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->ResizeTo(familiesWidth, listsHeight);
    scrollStyles->ResizeTo(stylesWidth, listsHeight);
    f_Sizes_உள்ளீடு->ResizeTo(sizesWidth, f_Sizes_உள்ளீடு->Bounds().Height());
    scrollSizes->ResizeTo(sizesWidth,
	    listsHeight - (offsetYSizes - offsetYLists));

    buttonOK->MoveBy(maxControlsWidth - buttonOK->Bounds().Width(), offsetYButtons);
    buttonCancel->MoveBy(maxControlsWidth - buttonOK->Bounds().Width()
	    - buttonCancel->Bounds().Width() - kVimDialogSpacingX, offsetYButtons);

    //	fill lists
    முழு selIndex = -1;
    முழு மொத்தம் = மொத்தம்_font_families();
    க்கு (முழு i = 0; i < மொத்தம்; i++) {
	font_family family;
	எனில் (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->AddItem(புதிய BStringItem((மாறிலி எழுத்து*)family));
	    எனில் (strncmp(family, fFontFamily, B_FONT_FAMILY_LENGTH) == 0)
		selIndex = i;
	}
    }

    எனில் (selIndex >= 0) {
	fFamiliesList->Select(selIndex);
	fFamiliesList->ScrollToSelection();
    }

    _UpdateFontStyles();

    selIndex = -1;
    க்கு (முழு size = 8, index = 0; size <= 18; size++, index++) {
	BString str;
	str << size;
	fSizesList->AddItem(புதிய BStringItem(str));
	எனில் (size == fFontSize)
	    selIndex = index;

    }

    எனில் (selIndex >= 0) {
	fSizesList->Select(selIndex);
	fSizesList->ScrollToSelection();
    }

    fFamiliesList->SetSelectionMessage(புதிய BMessage(k_Vim_Dialog_Family_Select_செய்தி));
    fStylesList->SetSelectionMessage(புதிய BMessage(k_Vim_Dialog_Style_Select_செய்தி));
    fSizesList->SetSelectionMessage(புதிய BMessage(k_Vim_Dialog_Size_Select_செய்தி));
    f_Sizes_உள்ளீடு->SetModificationMessage(புதிய BMessage(k_Vim_Dialog_Size_உள்ளீடு_செய்தி));

    Update_Size_உள்ளீடு_Preview();
    _UpdateFontPreview();

    fStatus = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    எனில் (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    வெறுமை
VimSelectFontDialog::_CleanList(BListView* list)
{
    போது (0 < list->மொத்தம்_பொருட்கள்())
	delete (dynamic_cast<BStringItem*>(list->RemoveItem((int32)0)));
}

    தருக்க
VimSelectFontDialog::Go()
{
    எனில் (fStatus != B_OK) {
	Quit();
	திரும்பு NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    எனில் (fDialogSem < B_OK) {
	Quit();
	திரும்பு f_Dialog_மதிப்பு;
    }

    Show();

    போது (acquire_sem(fDialogSem) == B_INTERRUPTED);

    தருக்க ret_மதிப்பு = f_Dialog_மதிப்பு;

    எனில் (Lock())
	Quit();

    திரும்பு ret_மதிப்பு;
}


வெறுமை VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 selIndex = -1;
    int32 மொத்தம் = மொத்தம்_font_styles(fFontFamily);
    க்கு (int32 i = 0; i < மொத்தம்; i++) {
	font_style style;
	uint32 flags = 0;
	எனில் (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->AddItem(புதிய BStringItem((மாறிலி எழுத்து*)style));
	    எனில் (strncmp(style, fFontStyle, B_FONT_STYLE_LENGTH) == 0)
		selIndex = i;
	}
    }

    எனில் (selIndex >= 0) {
	fStylesList->Select(selIndex);
	fStylesList->ScrollToSelection();
    } இல்லையெனில்
	fStylesList->Select(0);
}


வெறுமை VimSelectFontDialog::Update_Size_உள்ளீடு_Preview()
{
    எழுத்து buf[10] = {0};
    vim_snprintf(buf, அளவு(buf), (எழுத்து*)"%.0f", fFontSize);
    f_Sizes_உள்ளீடு->SetText(buf);
}


வெறுமை VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.SetSize(fFontSize);
    font.SetFamilyAndStyle(fFontFamily, fFontStyle);
    fPreview->SetFont(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_SIZE);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (முழு)fFontSize << " pt.";
    fPreview->SetText(str);
}


    தருக்க
VimSelectFontDialog::_UpdateFromListItem(BListView* list, எழுத்து* text, முழு textSize)
{
    int32 index = list->CurrentSelection();
    எனில் (index < 0)
	திரும்பு பொய்;
    BStringItem* item = (BStringItem*)list->ItemAt(index);
    எனில் (item == வெற்று)
	திரும்பு பொய்;
    strncpy(text, item->Text(), textSize);
    திரும்பு உண்மை;
}


வெறுமை VimSelectFontDialog::MessageReceived(BMessage *செய்தி)
{
    தேர்வு (செய்தி->what) {
	நிலை k_Vim_Dialog_OKButton_செய்தி:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_LENGTH);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_LENGTH);
	    *fSize = fFontSize;
	    f_Dialog_மதிப்பு = உண்மை;
	நிலை k_Vim_Dialog_Cancel_Button_செய்தி:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    திரும்பு;
	நிலை B_சாவி_UP:
	    {
		int32 சாவி = 0;
		எனில் (செய்தி->FindInt32("raw_char", &சாவி) == B_OK
			&& சாவி == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    நிறுத்து;

	நிலை k_Vim_Dialog_Family_Select_செய்தி:
	    எனில் (_UpdateFromListItem(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_LENGTH)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    நிறுத்து;
	நிலை k_Vim_Dialog_Style_Select_செய்தி:
	    எனில் (_UpdateFromListItem(fStylesList,
		    fFontStyle, B_FONT_STYLE_LENGTH))
		_UpdateFontPreview();
	    நிறுத்து;
	நிலை k_Vim_Dialog_Size_Select_செய்தி:
	    {
		எழுத்து buf[10] = {0};
		எனில் (_UpdateFromListItem(fSizesList, buf, அளவு(buf))) {
		    தசம size = atof(buf);
		    எனில் (size > 0.f) {
			fFontSize = size;
			Update_Size_உள்ளீடு_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    நிறுத்து;
	நிலை k_Vim_Dialog_Size_உள்ளீடு_செய்தி:
	    {
		தசம size = atof(f_Sizes_உள்ளீடு->Text());
		எனில் (size > 0.f) {
		    fFontSize = size;
		    _UpdateFontPreview();
		}
	    }
	    நிறுத்து;
	இயல்பு:
	    நிறுத்து;
    }
    திரும்பு BWindow::MessageReceived(செய்தி);
}

#endif // FEAT_GUI_DIALOG

#ifdef FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
நிலையான BMessage * MenuMessage(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect frame, மாறிலி எழுத்து *name) :
    BBox(frame, name, B_FOLLOW_இடது | B_FOLLOW_உச்சி, B_WILL_DRAW | B_FRAME_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 மொத்தம் = fButtonsList.மொத்தம்_பொருட்கள்();
    க்கு (int32 i = 0; i < மொத்தம்; i++)
	delete (BPictureButton*)fButtonsList.ItemAt(i);
    fButtonsList.MakeEmpty();

    delete normalButtonsBitmap;
    delete grayedButtonsBitmap;
    normalButtonsBitmap    = வெற்று;
    grayedButtonsBitmap  = வெற்று;
}

    வெறுமை
VimToolbar::AttachedToWindow()
{
    BBox::AttachedToWindow();

    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    தசம
VimToolbar::ToolbarHeight() மாறிலி
{
    தசம size = வெற்று == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
    திரும்பு size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    தருக்க
VimToolbar::ModifyBitmapToGrayed(BBitmap *bitmap)
{
    தசம height = bitmap->Bounds().Height();
    தசம width  = bitmap->Bounds().Width();

    rgb_color *bits = (rgb_color*)bitmap->Bits();
    int32 pixels = bitmap->BitsLength() / 4;
    க்கு (int32 i = 0; i < pixels; i++) {
	bits[i].red = bits[i].green =
	bits[i].blue = ((uint32)bits[i].red + bits[i].green + bits[i].blue) / 3;
	bits[i].alpha /= 4;
    }

    திரும்பு உண்மை;
}

    தருக்க
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    normalButtonsBitmap = LoadVimBitmap("builtin-tools.png");
    எனில் (normalButtonsBitmap == வெற்று)
	//  customized not found? dig application resources for "builtin-tools" one
	normalButtonsBitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    எனில் (normalButtonsBitmap == வெற்று)
	திரும்பு பொய்;

    BMessage archive;
    normalButtonsBitmap->Archive(&archive);

    grayedButtonsBitmap = புதிய BBitmap(&archive);
    எனில் (grayedButtonsBitmap == வெற்று)
	திரும்பு பொய்;

    //	modify grayed bitmap
    ModifyBitmapToGrayed(grayedButtonsBitmap);

    திரும்பு உண்மை;
}

BBitmap *VimToolbar::LoadVimBitmap(மாறிலி எழுத்து* fileName)
{
    BBitmap *bitmap = வெற்று;

    முழு mustfree = 0;
    char_u* runtimePath = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    எனில் (runtimePath != வெற்று && fileName != வெற்று) {
	BString strPath((எழுத்து*)runtimePath);
	strPath << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(strPath.String());
    }

    எனில் (mustfree)
	vim_free(runtimePath);

    திரும்பு bitmap;
}

    தருக்க
VimToolbar::GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, தருக்க pressed)
{
    தசம size = bitmapFrom->Bounds().Height() + 1.;

    BView view(BRect(0, 0, size, size), "", 0, 0);

    AddChild(&view);
    view.BeginPicture(pictureTo);

    view.Set_உயர்வு_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.SetDrawingMode(B_OP_OVER);

    BRect source(0, 0, size - 1, size - 1);
    BRect destination(source);

    source.OffsetBy(size * index, 0);
    destination.OffsetBy(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, source, destination);

    எனில் (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	size += ButtonMargin * 2 - 1;
	view.Begin_வரி_Array(4);
	view.Add_வரி(BPoint(0, 0),	 BPoint(size, 0),    shadowColor);
	view.Add_வரி(BPoint(size, 0),	 BPoint(size, size), shineColor);
	view.Add_வரி(BPoint(size, size), BPoint(0, size),    shineColor);
	view.Add_வரி(BPoint(0, size),	 BPoint(0, 0),	     shadowColor);
	view.End_வரி_Array();
    }

    view.EndPicture();
    RemoveChild(&view);

    திரும்பு உண்மை;
}

    தருக்க
VimToolbar::AddButton(int32 index, vimmenu_T *menu)
{
    BPictureButton *button = வெற்று;
    எனில் (!menu_is_separator(menu->name)) {
	தசம size = normalButtonsBitmap ?
	    normalButtonsBitmap->Bounds().Height() + 1. + ButtonMargin * 2 : 18.;
	BRect frame(0, 0, size, size);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture pictureGray;

	எனில் (menu->iconfile == வெற்று && menu->iconidx >= 0 && normalButtonsBitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, normalButtonsBitmap, உண்மை);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, normalButtonsBitmap, பொய்);
	    GetPictureFromBitmap(&pictureGray, menu->iconidx, grayedButtonsBitmap, பொய்);
	} இல்லையெனில் {

	    char_u buffer[MAXPATHL] = {0};
	    BBitmap *bitmap = வெற்று;

	    எனில் (menu->iconfile) {
		gui_find_iconfile(menu->iconfile, buffer, (எழுத்து*)"png");
		bitmap = BTranslationUtils::GetBitmap((எழுத்து*)buffer);
	    }

	    எனில் (bitmap == வெற்று && gui_find_bitmap(menu->name, buffer, (எழுத்து*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((எழுத்து*)buffer);

	    எனில் (bitmap == வெற்று)
		bitmap = புதிய BBitmap(BRect(0, 0, size, size), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, உண்மை);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, பொய்);
	    ModifyBitmapToGrayed(bitmap);
	    GetPictureFromBitmap(&pictureGray, 0, bitmap, பொய்);

	    delete bitmap;
	}

	button = புதிய BPictureButton(frame, (எழுத்து*)menu->name,
		    &pictureOff, &pictureOn, MenuMessage(menu));

	button->SetDisabledOn(&pictureGray);
	button->SetDisabledOff(&pictureGray);

	button->SetTarget(gui.vimTextArea);

	AddChild(button);

	menu->button = button;
    }

    தருக்க result = fButtonsList.AddItem(button, index);
    InvalidateLayout();
    திரும்பு result;
}

    தருக்க
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    எனில் (menu->button) {
	எனில் (fButtonsList.RemoveItem(menu->button)) {
	    delete menu->button;
	    menu->button = வெற்று;
	}
    }
    திரும்பு உண்மை;
}

    தருக்க
VimToolbar::GrayButton(vimmenu_T *menu, முழு grey)
{
    எனில் (menu->button) {
	int32 index = fButtonsList.IndexOf(menu->button);
	எனில் (index >= 0)
	    menu->button->SetEnabled(grey ? பொய் : உண்மை);
    }
    திரும்பு உண்மை;
}

    வெறுமை
VimToolbar::InvalidateLayout()
{
    int32 offset = ToolbarMargin;
    int32 மொத்தம் = fButtonsList.மொத்தம்_பொருட்கள்();
    க்கு (int32 i = 0; i < மொத்தம்; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.ItemAt(i);
	எனில் (button) {
	    button->MoveTo(offset, ToolbarMargin);
	    offset += button->Bounds().Width() + ToolbarMargin;
	} இல்லையெனில்
	    offset += ToolbarMargin * 3;
    }
}

#endif /*FEAT_TOOLBAR*/

#எனில் defined(FEAT_GUI_TABLINE)

    தசம
Vim_Tab_வரி::TablineHeight() மாறிலி
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    திரும்பு TabHeight(); //  + ToolbarMargin;
}

வெறுமை
Vim_Tab_வரி::MouseDown(BPoint point)
{
    எனில் (!gui_mch_showing_tabline())
	திரும்பு;

    BMessage *m = Window()->CurrentMessage();
    ப்ரதிஜ்ஞா(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    int32 clicks = 0;
    m->FindInt32("clicks", &clicks);

    முழு index = 0; //  0 means here - no tab found
    க்கு (முழு i = 0; i < மொத்தம்_Tabs(); i++) {
	எனில் (TabFrame(i).Contains(point)) {
	    index = i + 1; //  indexes are 1-based
	    நிறுத்து;
	}
    }

    முழு event = -1;

    எனில் ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    இல்லையெனில் எனில் (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = index > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    இல்லையெனில் எனில் (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPopUpMenu* எடு_Up_Menu = புதிய BPopUpMenu("tabLineContextMenu", பொய், பொய்);
	எடு_Up_Menu->AddItem(புதிய BMenuItem(_("Close tabi R"), புதிய BMessage(TABLINE_MENU_CLOSE)));
	எடு_Up_Menu->AddItem(புதிய BMenuItem(_("New tab    T"), புதிய BMessage(TABLINE_MENU_NEW)));
	எடு_Up_Menu->AddItem(புதிய BMenuItem(_("Open tab..."), புதிய BMessage(TABLINE_MENU_OPEN)));

	ConvertToScreen(&point);
	BMenuItem* item = எடு_Up_Menu->Go(point);
	எனில் (item != வெற்று) {
	    event = item->Command();
	}

	delete எடு_Up_Menu;

    } இல்லையெனில் {
	//  default processing
	BTabView::MouseDown(point);
	திரும்பு;
    }

    எனில் (event < 0)
	திரும்பு;

    Vim_Tabline_Menu_செய்தி tmm;
    tmm.index = index;
    tmm.event = event;
    write_port(gui.vdcmp, Vim_செய்தி::TablineMenu, &tmm, அளவு(tmm));
}

வெறுமை
Vim_Tab_வரி::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_வரி *tab_வரி = gui.vimForm->Tab_வரி();
    எனில் (tab_வரி != வெற்று) {

	int32 i = 0;
	க்கு (; i < tab_வரி->மொத்தம்_Tabs(); i++)
	    எனில் (இது == tab_வரி->TabAt(i))
		நிறுத்து;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	எனில் (i < tab_வரி->மொத்தம்_Tabs()) {
	    Vim_Tabline_செய்தி tm;
	    tm.index = i + 1;
	    write_port(gui.vdcmp, Vim_செய்தி::Tabline, &tm, அளவு(tm));
	}
    }
}

#endif //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
நிலையான எழுத்து appsig[] = "application/x-vnd.Haiku-Vim-8";
சாவி_map *சாவி_Map;
எழுத்து *சாவி_Map_Chars;
முழு main_exitcode = 127;

    status_t
gui_haiku_process_event(bigtime_t timeout)
{
    கட்டமைப்பு Vim_செய்தி vm;
    int32 what;
    ssize_t size;

    size = read_port_etc(gui.vdcmp, &what, &vm, அளவு(vm),
	    B_TIMEOUT, timeout);

    எனில் (size >= 0) {
	தேர்வு (what) {
	    நிலை Vim_செய்தி::சாவி_2:
		{
		    char_u *string = vm.u.சாவி_2.chars;
		    முழு len = vm.u.சாவி_2.length;
		    எனில் (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_உள்ளீடு_buf();
			got_int = TRUE;
		    }

		    எனில் (vm.u.சாவி_2.csi_escape)
#ifndef FEAT_MBYTE_IME
		    {
			முழு	i;
			char_u	buf[2];

			க்கு (i = 0; i < len; ++i)
			{
			    add_to_உள்ளீடு_buf(string + i, 1);
			    எனில் (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (முழு)KE_CSI;
				add_to_உள்ளீடு_buf(buf, 2);
			    }
			}
		    }
#இல்லையெனில்
			add_to_உள்ளீடு_buf_csi(string, len);
#endif
		    இல்லையெனில்
			add_to_உள்ளீடு_buf(string, len);
		}
		நிறுத்து;
	    நிலை Vim_செய்தி::Resize:
		gui_resize_shell(vm.u.NewSize.width, vm.u.NewSize.height);
		நிறுத்து;
	    நிலை Vim_செய்தி::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_மொத்தம் =
			atomic_add(&vm.u.Scroll.sb->scroll_Event_மொத்தம், -1);
		    எனில் (old_மொத்தம் <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.மதிப்பு, vm.u.Scroll.stillDragging);
		}
		நிறுத்து;
#எனில் defined(FEAT_MENU)
	    நிலை Vim_செய்தி::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		நிறுத்து;
#endif
	    நிலை Vim_செய்தி::Mouse:
		{
		    int32 old_மொத்தம்;
		    எனில் (vm.u.Mouse.button == MOUSE_DRAG)
			old_மொத்தம் =
			    atomic_add(&gui.vimTextArea->mouse_Drag_Event_மொத்தம், -1);
		    இல்லையெனில்
			old_மொத்தம் = 0;
		    எனில் (old_மொத்தம் <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		நிறுத்து;
	    நிலை Vim_செய்தி::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		நிறுத்து;
	    நிலை Vim_செய்தி::Focus:
		gui.in_focus = vm.u.Focus.active;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		எனில் (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		நிறுத்து;
	    நிலை Vim_செய்தி::Refs:
		::RefsReceived(vm.u.Refs.message, vm.u.Refs.changedir);
		நிறுத்து;
	    நிலை Vim_செய்தி::Tabline:
		send_tabline_event(vm.u.Tabline.index);
		நிறுத்து;
	    நிலை Vim_செய்தி::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.index, vm.u.TablineMenu.event);
		நிறுத்து;
	    இயல்பு:
		//  unrecognised message, ignore it
		நிறுத்து;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    திரும்பு size;
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

    முழு
vim_lock_screen()
{
    திரும்பு !gui.vimWindow || gui.vimWindow->Lock();
}

    வெறுமை
vim_unlock_screen()
{
    எனில் (gui.vimWindow)
	gui.vimWindow->Unlock();
}

#define RUN_BAPPLICATION_IN_NEW_THREAD	0

#எனில் RUN_BAPPLICATION_IN_NEW_THREAD

    int32
run_vimapp(வெறுமை *args)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.Run();		    // Run until Quit() called

    திரும்பு 0;
}

#இல்லையெனில்

    int32
call_main(வெறுமை *args)
{
    கட்டமைப்பு MainArgs *ma = (MainArgs *)args;

    திரும்பு முதன்மை(ma->argc, ma->argv);
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    வெறுமை
gui_mch_prepare(
	முழு	*argc,
	எழுத்து	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    எனில் (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(find_thread(வெற்று), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_port(B_MAX_PORT_மொத்தம், "vim VDCMP");

#எனில் RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = spawn_thread(run_vimapp, "vim VimApp",
		tinfo.priority, வெற்று);
	எனில் (tid >= B_OK) {
	    resume_thread(tid);
	} இல்லையெனில் {
	    getout(1);
	}
#இல்லையெனில்
	MainArgs ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.priority, &ma);
	எனில் (tid >= B_OK) {
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
	    (வெறுமை)wait_for_thread(tid, &dummy_exitcode);

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
    எனில் (!isatty(0)) {
	கட்டமைப்பு stat stat_stdin, stat_dev_null;

	எனில் (fstat(0, &stat_stdin) == -1 ||
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
    முழு
gui_mch_init_check(வெறுமை)
{
    திரும்பு OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    முழு
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_back_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    gui.scrollbar_width = (முழு) B_V_SCROLL_BAR_WIDTH;
    gui.scrollbar_height = (முழு) B_H_SCROLL_BAR_HEIGHT;
#ifdef FEAT_MENU
    gui.menu_height = 19;   //	initial guess -
    //	correct for my default settings
#endif
    gui.border_offset = 3;  //	coordinates are inside window borders

    எனில் (gui.vdcmp < B_OK)
	திரும்பு FAIL;
    get_சாவி_map(&சாவி_Map, &சாவி_Map_Chars);

    gui.vimWindow = புதிய VimWindow();	// hidden and locked
    எனில் (!gui.vimWindow)
	திரும்பு FAIL;

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

    திரும்பு OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    வெறுமை
gui_mch_new_colors()
{
    rgb_color rgb = GUI_TO_RGB(gui.back_pixel);

    எனில் (gui.vimWindow->Lock()) {
	gui.vimForm->SetViewColor(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vimWindow->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    முழு
gui_mch_open()
{
    எனில் (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    எனில் (gui.vimWindow->Lock()) {
	gui.vimWindow->Show();
	gui.vimWindow->Unlock();
	திரும்பு OK;
    }

    திரும்பு FAIL;
}

    வெறுமை
gui_mch_exit(முழு vim_exitcode)
{
    எனில் (gui.vimWindow) {
	thread_id tid = gui.vimWindow->Thread();
	gui.vimWindow->Lock();
	gui.vimWindow->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_port(gui.vdcmp);
#எனில் !RUN_BAPPLICATION_IN_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#endif
    எனில் (gui.vimApp) {
	VimTextAreaView::guiBlankMouse(பொய்);

	main_exitcode = vim_exitcode;
#எனில் RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->Lock();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#இல்லையெனில்
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
    முழு
gui_mch_get_winpos(முழு *x, முழு *y)
{
    எனில் (gui.vimWindow->Lock()) {
	BRect r;
	r = gui.vimWindow->Frame();
	gui.vimWindow->Unlock();
	*x = (முழு)r.இடது;
	*y = (முழு)r.உச்சி;
	திரும்பு OK;
    }
    இல்லையெனில்
	திரும்பு FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    வெறுமை
gui_mch_set_winpos(முழு x, முழு y)
{
    எனில் (gui.vimWindow->Lock()) {
	gui.vimWindow->MoveTo(x, y);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
வெறுமை
gui_mch_set_shellsize(
	முழு	width,
	முழு	height,
	முழு	min_width,
	முழு	min_height,
	முழு	base_width,
	முழு	base_height,
	முழு	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    எனில் (gui.vimWindow->Lock()) {
	gui.vimWindow->ResizeTo(width - PEN_WIDTH, height - PEN_WIDTH);

	// set size limits
	தசம minWidth, maxWidth, minHeight, maxHeight;

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

வெறுமை
gui_mch_get_screen_dimensions(
	முழு	*screen_w,
	முழு	*screen_h)
{
    BRect frame;

    {
	BScreen screen(gui.vimWindow);

	எனில் (screen.IsValid()) {
	    frame = screen.Frame();
	} இல்லையெனில் {
	    frame.வலது = 640;
	    frame.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (முழு) frame.வலது - 2 * gui.scrollbar_width - 20;
    *screen_h = (முழு) frame.bottom - gui.scrollbar_height
#ifdef FEAT_MENU
	- gui.menu_height
#endif
	- 30;
}

வெறுமை
gui_mch_set_text_area_pos(
	முழு	x,
	முழு	y,
	முழு	w,
	முழு	h)
{
    எனில் (!gui.vimTextArea)
	திரும்பு;

    எனில் (gui.vimWindow->Lock()) {
	gui.vimTextArea->MoveTo(x, y);
	gui.vimTextArea->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);

#ifdef FEAT_GUI_TABLINE
	எனில் (gui.vimForm->Tab_வரி() != வெற்று) {
	    gui.vimForm->Tab_வரி()->ResizeTo(w, gui.vimForm->TablineHeight());
	}
#endif // FEAT_GUI_TABLINE

	gui.vimWindow->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

வெறுமை
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	முழு	flag)
{
    VimScrollBar *vsb = sb->id;
    எனில் (gui.vimWindow->Lock()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	எனில் (flag) {
	    எனில் (vsb->IsHidden()) {
		vsb->Show();
	    }
	} இல்லையெனில் {
	    எனில் (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vimWindow->Unlock();
    }
}

வெறுமை
gui_mch_set_scrollbar_thumb(
	scrollbar_T *sb,
	முழு	val,
	முழு	size,
	முழு	max)
{
    எனில் (gui.vimWindow->Lock()) {
	VimScrollBar *s = sb->id;
	எனில் (max == 0) {
	    s->Set_மதிப்பு(0);
	    s->SetRange(0.0, 0.0);
	} இல்லையெனில் {
	    s->SetProportion((தசம)size / (max + 1.0));
	    s->SetSteps(1.0, size > 5 ? size - 2 : size);
#ifndef SCROLL_PAST_END	    //	really only defined in gui.c...
	    max = max + 1 - size;
#endif
	    எனில் (max < s->மதிப்பு_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->Set_மதிப்பு(val);
		s->SetRange(0.0, max);
	    } இல்லையெனில் {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->SetRange(0.0, max);
		s->Set_மதிப்பு(val);
	    }
	}
	gui.vimWindow->Unlock();
    }
}

வெறுமை
gui_mch_set_scrollbar_pos(
	scrollbar_T *sb,
	முழு	x,
	முழு	y,
	முழு	w,
	முழு	h)
{
    எனில் (gui.vimWindow->Lock()) {
	BRect winb = gui.vimWindow->Bounds();
	தசம vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);
	எனில் (winb.வலது-(x+w)<w) vsbx = winb.வலது - (w - PEN_WIDTH);
	vsb->MoveTo(vsbx, vsby);
	gui.vimWindow->Unlock();
    }
}

    முழு
gui_mch_get_scrollbar_xpadding(வெறுமை)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    திரும்பு 0;
}

    முழு
gui_mch_get_scrollbar_ypadding(வெறுமை)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    திரும்பு 0;
}

வெறுமை
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	முழு	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = புதிய VimScrollBar(sb, posture);
    எனில் (gui.vimWindow->Lock()) {
	vsb->SetTarget(gui.vimTextArea);
	vsb->Hide();
	gui.vimForm->AddChild(vsb);
	gui.vimWindow->Unlock();
    }
}

#எனில் defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
வெறுமை
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    எனில் (gui.vimWindow->Lock()) {
	sb->id->RemoveSelf();
	delete sb->id;
	gui.vimWindow->Unlock();
    }
}
#endif

/*
 * Cursor does not flash
 */
    முழு
gui_mch_is_blink_off(வெறுமை)
{
    திரும்பு FALSE;
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

நிலையான முழு	blink_state = BLINK_NONE;
நிலையான long_u	    blink_waittime = 700;
நிலையான long_u	    blink_ontime = 400;
நிலையான long_u	    blink_offtime = 250;
நிலையான முழு  blink_timer = 0;

வெறுமை
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
    வெறுமை
gui_mch_stop_blink(முழு may_call_gui_update_cursor)
{
    // TODO
    எனில் (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    எனில் (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    வெறுமை
gui_mch_start_blink()
{
    // TODO
    எனில் (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    எனில் (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
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
முழு
gui_mch_init_font(
	char_u	    *font_name,
	முழு	    fontset)
{
    எனில் (gui.vimWindow->Lock())
    {
	முழு rc = gui.vimTextArea->mchInitFont(font_name);
	gui.vimWindow->Unlock();

	திரும்பு rc;
    }

    திரும்பு FAIL;
}


    முழு
gui_mch_adjust_charsize()
{
    திரும்பு FAIL;
}


    முழு
gui_mch_font_dialog(font_family* family, font_style* style, தசம* size)
{
#எனில் defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = புதிய VimSelectFontDialog(family, style, size);
    தருக்க ret = dialog->Go();
	delete dialog;
	திரும்பு ret;
#இல்லையெனில்
    திரும்பு NOFONT;
#endif // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	முழு	    giveErrorIfMissing)
{
    நிலையான VimFont *fontList = வெற்று;

    எனில் (!gui.in_use)	//  can't do this when GUI not running
	திரும்பு NOFONT;

    //	storage for locally modified name;
    மாறிலி முழு buff_size = B_FONT_FAMILY_LENGTH + B_FONT_STYLE_LENGTH + 20;
    நிலையான எழுத்து font_name[buff_size] = {0};
    font_family family = {0};
    font_style	style  = {0};
    தசம size = 0.f;

    எனில் (name == 0 && be_fixed_font == 0) {
	எனில் (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	திரும்பு NOFONT;
    }

    தருக்க useSelectGUI = பொய்;
    எனில் (name != வெற்று)
	எனில் (STRCMP(name, "*") == 0) {
	    useSelectGUI = உண்மை;
	    STRNCPY(font_name, hl_get_font_name(), buff_size);
	} இல்லையெனில்
	    STRNCPY(font_name, name, buff_size);

    எனில் (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	size = be_fixed_font->Size();
	vim_snprintf(font_name, buff_size,
	    (எழுத்து*)"%s/%s/%.0f", family, style, size);
    }

    //	replace underscores with spaces
    எழுத்து* end = 0;
    போது (end = strchr((எழுத்து *)font_name, '_'))
	*end = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    நிலையான எழுத்து buff[buff_size] = {0};
    STRNCPY(buff, font_name, buff_size);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_LENGTH);
    எழுத்து* style_s = strtok(0, "/\0");
    எனில் (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_LENGTH);
    size = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    எனில் (useSelectGUI) {
	எனில் (gui_mch_font_dialog(&family, &style, &size) == NOFONT)
	    திரும்பு FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_size,
		(எழுத்து*)"%s/%s/%.0f", family, style, size);
	hl_set_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	எனில் (new_p_guifont != வெற்று) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    க்கு ( ; *new_p_guifont; ++new_p_guifont)
		எனில் (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    க்கு (flp = fontList; flp; flp = flp->next) {
	எனில் (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    திரும்பு (GuiFont)flp;
	}
    }

    VimFont *font = புதிய VimFont();
    font->name = vim_strsave((char_u*)font_name);

    எனில் (மொத்தம்_font_styles(family) <= 0) {
	எனில் (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	delete font;
	திரும்பு NOFONT;
    }

    //	Remember font in the static list for later use
    font->next = fontList;
    fontList = font;

    font->SetFamilyAndStyle(family, style);
    எனில் (size > 0.f)
	font->SetSize(size);

    font->SetSpacing(B_FIXED_SPACING);
    font->SetEncoding(B_UNICODE_UTF8);

    திரும்பு (GuiFont)font;
}

/*
 * Set the current text font.
 */
வெறுமை
gui_mch_set_font(
	GuiFont	font)
{
    எனில் (gui.vimWindow->Lock()) {
	VimFont *vf = (VimFont *)font;

	gui.vimTextArea->SetFont(vf);

	gui.char_width = (முழு) vf->StringWidth("n");
	font_height fh;
	vf->GetHeight(&fh);
	gui.char_height = (முழு)(fh.ascent + 0.9999)
	    + (முழு)(fh.descent + 0.9999) + (முழு)(fh.leading + 0.9999);
	gui.char_ascent = (முழு)(fh.ascent + 0.9999);

	gui.vimWindow->Unlock();
    }
}

// XXX TODO This is apparently never called...
வெறுமை
gui_mch_free_font(
	GuiFont	font)
{
    எனில் (font == NOFONT)
	திரும்பு;
    VimFont *f = (VimFont *)font;
    எனில் (--f->refcount <= 0) {
	எனில் (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	delete f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    எனில் (name == வெற்று)
	திரும்பு வெற்று;
    திரும்பு vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    முழு
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
	gui.char_height = (முழு)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (முழு)(fh.ascent + 0.5) + p_linespace / 2;
    }
    திரும்பு OK;
}

    வெறுமை
gui_mch_getmouse(முழு *x, முழு *y)
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

    வெறுமை
gui_mch_mousehide(முழு hide)
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
    திரும்பு gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
வெறுமை
gui_mch_set_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    எனில் (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_உயர்வு_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text background color.
 */
வெறுமை
gui_mch_set_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    எனில் (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_கீழ்_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    வெறுமை
gui_mch_set_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

வெறுமை
gui_mch_draw_string(
	முழு	row,
	முழு	col,
	char_u	*s,
	முழு	len,
	முழு	flags)
{
    எனில் (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchDrawString(row, col, s, len, flags);
	gui.vimWindow->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(முழு r, முழு g, முழு b)
{
    திரும்பு gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
முழு
gui_mch_haskey(
	char_u	*name)
{
    முழு i;

    க்கு (i = 0; special_சாவிகள்[i].Be_சாவிகள் != 0; i++)
	எனில் (name[0] == special_சாவிகள்[i].vim_code0 &&
		name[1] == special_சாவிகள்[i].vim_code1)
	    திரும்பு OK;
    திரும்பு FAIL;
}

    வெறுமை
gui_mch_beep()
{
    ::beep();
}

    வெறுமை
gui_mch_flash(முழு msec)
{
    // Do a visual beep by reversing the foreground and background colors

    எனில் (gui.vimWindow->Lock()) {
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
வெறுமை
gui_mch_invert_rectangle(
	முழு	r,
	முழு	c,
	முழு	nr,
	முழு	nc)
{
    BRect rect;
    rect.இடது = FILL_X(c);
    rect.உச்சி = FILL_Y(r);
    rect.வலது = rect.இடது + nc * gui.char_width - PEN_WIDTH;
    rect.bottom = rect.உச்சி + nr * gui.char_height - PEN_WIDTH;

    எனில் (gui.vimWindow->Lock()) {
	gui.vimTextArea->SetDrawingMode(B_OP_INVERT);
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->SetDrawingMode(B_OP_COPY);
	gui.vimWindow->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    வெறுமை
gui_mch_iconify()
{
    எனில் (gui.vimWindow->Lock()) {
	gui.vimWindow->Minimize(உண்மை);
	gui.vimWindow->Unlock();
    }
}

#எனில் defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    வெறுமை
gui_mch_set_foreground(வெறுமை)
{
    // TODO
}
#endif

/*
 * Set the window title
 */
வெறுமை
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    எனில் (gui.vimWindow->Lock()) {
	gui.vimWindow->SetTitle((எழுத்து *)title);
	gui.vimWindow->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    வெறுமை
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.இடது = FILL_X(gui.col);
    r.உச்சி = FILL_Y(gui.row);
    முழு cells = utf_off2cells(வரி_Offset[gui.row] + gui.col, 100); // TODO-TODO
    எனில் (cells>=4) cells = 1;
    r.வலது = r.இடது + cells*gui.char_width - PEN_WIDTH;
    r.bottom = r.உச்சி + gui.char_height - PEN_WIDTH;

    எனில் (gui.vimWindow->Lock()) {
	gui.vimTextArea->StrokeRect(r);
	gui.vimWindow->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
வெறுமை
gui_mch_draw_part_cursor(
	முழு	w,
	முழு	h,
	guicolor_T  color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.இடது =
#ifdef FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_வலது ? FILL_X(gui.col + 1) - w :
#endif
	FILL_X(gui.col);
    r.வலது = r.இடது + w - PEN_WIDTH;
    r.bottom = FILL_Y(gui.row + 1) - PEN_WIDTH;
    r.உச்சி = r.bottom - h + PEN_WIDTH;

    எனில் (gui.vimWindow->Lock()) {
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
    வெறுமை
gui_mch_update()
{
    gui_mch_flush();
    போது (port_மொத்தம்(gui.vdcmp) > 0 &&
	    !vim_is_உள்ளீடு_buf_full() &&
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
முழு
gui_mch_wait_for_chars(
	long	wtime)
{
    முழு		focus;
    bigtime_t	until, timeout;
    status_t	st;

    எனில் (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_time() + timeout;
    }
    இல்லையெனில்
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.in_focus;
    க்கு (;;)
    {
	// Stop or start blinking when focus changes
	எனில் (gui.in_focus != focus)
	{
	    எனில் (gui.in_focus)
		gui_mch_start_blink();
	    இல்லையெனில்
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
	எனில் (did_add_timer)
	    // Need to recompute the waiting time.
	    நிறுத்து;
# endif
# ifdef FEAT_JOB_CHANNEL
	எனில் (has_any_channel())
	{
	    எனில் (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	இல்லையெனில் எனில் (wtime < 0)
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

	எனில் (உள்ளீடு_available())
	    திரும்பு OK;
	எனில் (st < B_OK)		// includes B_TIMED_OUT
	    திரும்பு FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	எனில் (wtime >= 0)
	{
	    timeout = until - system_time();
	    எனில் (timeout < 0)
		நிறுத்து;
	}
    }
    திரும்பு FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    வெறுமை
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    எனில் (gui.vimWindow->Lock()) {
	gui.vimWindow->Flush();
	gui.vimWindow->Unlock();
    }
    திரும்பு;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
வெறுமை
gui_mch_clear_block(
	முழு	row1,
	முழு	col1,
	முழு	row2,
	முழு	col2)
{
    எனில் (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearBlock(row1, col1, row2, col2);
	gui.vimWindow->Unlock();
    }
}

    வெறுமை
gui_mch_clear_all()
{
    எனில் (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearAll();
	gui.vimWindow->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
வெறுமை
gui_mch_delete_வரிகள்(
	முழு	row,
	முழு	இலக்கம்_வரிகள்)
{
    gui.vimTextArea->mch_Delete_வரிகள்(row, இலக்கம்_வரிகள்);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
வெறுமை
gui_mch_செருகு_வரிகள்(
	முழு	row,
	முழு	இலக்கம்_வரிகள்)
{
    gui.vimTextArea->mch_செருகு_வரிகள்(row, இலக்கம்_வரிகள்);
}

#எனில் defined(FEAT_MENU)
/*
 * Menu stuff.
 */

வெறுமை
gui_mch_enable_menu(
	முழு	flag)
{
    எனில் (gui.vimWindow->Lock())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->SetEnabled(flag);
	gui.vimWindow->Unlock();
    }
}

வெறுமை
gui_mch_set_menu_pos(
	முழு	x,
	முழு	y,
	முழு	w,
	முழு	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
வெறுமை
gui_mch_add_menu(
	vimmenu_T   *menu,
	முழு	idx)
{
    vimmenu_T	*parent = menu->parent;

    //	popup menu - just create it unattached
    எனில் (menu_is_popup(menu->name) && parent == வெற்று) {
	BPopUpMenu* எடு_Up_Menu = புதிய BPopUpMenu((மாறிலி எழுத்து*)menu->name, பொய், பொய்);
	menu->submenu_id = எடு_Up_Menu;
	menu->id = வெற்று;
	திரும்பு;
    }

    எனில் (!menu_is_menubar(menu->name)
	    || (parent != வெற்று && parent->submenu_id == வெற்று))
	திரும்பு;

    எனில் (gui.vimWindow->Lock())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	எனில் ( parent )
	    tmp = parent->submenu_id;
	இல்லையெனில்
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	எனில் ( ! tmp->FindItem((மாறிலி எழுத்து *) menu->dname)) {

	    BMenu *bmenu = புதிய BMenu((எழுத்து *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->AddItem(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->FindItem((மாறிலி எழுத்து *) menu->dname);

	}
	gui.vimWindow->Unlock();
    }
}

    வெறுமை
gui_mch_toggle_tearoffs(முழு enable)
{
    // no tearoff menus
}

    நிலையான BMessage *
MenuMessage(vimmenu_T *menu)
{
    BMessage *m = புதிய BMessage('menu');
    m->AddPointer("VimMenu", (வெறுமை *)menu);

    திரும்பு m;
}

/*
 * Add a menu item to a menu
 */
வெறுமை
gui_mch_add_menu_item(
	vimmenu_T   *menu,
	முழு	idx)
{
    முழு	    mnemonic = 0;
    vimmenu_T	*parent = menu->parent;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    எனில் (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	எனில் (menu_is_toolbar(parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    எனில் (toolbar != வெற்று) {
		toolbar->AddButton(idx, menu);
	    }
	} இல்லையெனில்
#endif

	எனில் (parent->submenu_id != வெற்று || menu_is_popup(parent->name)) {
	    எனில் (menu_is_separator(menu->name)) {
		BSeparatorItem *item = புதிய BSeparatorItem();
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = வெற்று;
	    }
	    இல்லையெனில் {
		BMenuItem *item = புதிய BMenuItem((எழுத்து *)menu->dname,
			MenuMessage(menu));
		item->SetTarget(gui.vimTextArea);
		item->SetTrigger((எழுத்து) menu->mnemonic);
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = வெற்று;
	    }
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
வெறுமை
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    எனில் (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	எனில் (menu->parent && menu_is_toolbar(menu->parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    எனில் (toolbar != வெற்று) {
		toolbar->RemoveButton(menu);
	    }
	} இல்லையெனில்
#endif
	{
	    ப்ரதிஜ்ஞா(menu->submenu_id == வெற்று || menu->submenu_id->மொத்தம்_பொருட்கள்() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    எனில் (bmenu)
	    {
		bmenu->RemoveItem(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		எனில் (bmenu == gui.vimForm->MenuBar() && bmenu->மொத்தம்_பொருட்கள்() == 0)
		{
		    bmenu->ResizeTo(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    delete menu->id;
	    menu->id = வெற்று;
	    menu->submenu_id = வெற்று;

	    gui.menu_height = (முழு) gui.vimForm->MenuHeight();
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
வெறுமை
gui_mch_menu_grey(
	vimmenu_T   *menu,
	முழு	grey)
{
#ifdef FEAT_TOOLBAR
    எனில் (menu->parent && menu_is_toolbar(menu->parent->name)) {
	எனில் (gui.vimWindow->Lock()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    எனில் (toolbar != வெற்று) {
		toolbar->GrayButton(menu, grey);
	    }
	    gui.vimWindow->Unlock();
	}
    } இல்லையெனில்
#endif
    எனில் (menu->id != வெற்று)
	menu->id->SetEnabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
வெறுமை
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	முழு	hidden)
{
    எனில் (menu->id != வெற்று)
	menu->id->SetEnabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    வெறுமை
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    வெறுமை
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    எனில் (!menu_is_popup(menu->name) || menu->submenu_id == வெற்று)
	திரும்பு;

    BPopUpMenu* popupMenu = dynamic_cast<BPopUpMenu*>(menu->submenu_id);
    எனில் (popupMenu == வெற்று)
	திரும்பு;

    BPoint point;
    எனில் (gui.vimWindow->Lock()) {
	uint32 buttons = 0;
	gui.vimTextArea->GetMouse(&point, &buttons);
	gui.vimTextArea->ConvertToScreen(&point);
	gui.vimWindow->Unlock();
    }
    popupMenu->Go(point, உண்மை);
}

#endif // FEAT_MENU

// Mouse stuff

#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
எழுத்து textplain[] = "text/plain";
எழுத்து vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    வெறுமை
clip_mch_request_selection(Clipboard_T *cbd)
{
    எனில் (be_clipboard->Lock())
    {
	BMessage *m = be_clipboard->Data();
	// m->PrintToStream();

	char_u *string = வெற்று;
	ssize_t stringlen = -1;

	எனில் (m->FindData(textplain, B_MIME_TYPE,
		    (மாறிலி வெறுமை **)&string, &stringlen) == B_OK
		|| m->FindString("text", (மாறிலி எழுத்து **)&string) == B_OK)
	{
	    எனில் (stringlen == -1)
		stringlen = STRLEN(string);

	    முழு type;
	    எழுத்து *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    எனில் (m->FindData(vimselectiontype, B_MIME_TYPE,
			(மாறிலி வெறுமை **)&seltype, &seltypelen) == B_OK)
	    {
		தேர்வு (*seltype)
		{
		    இயல்பு:
		    நிலை 'L':	type = MLINE;	நிறுத்து;
		    நிலை 'C':	type = MCHAR;	நிறுத்து;
#ifdef FEAT_VISUAL
		    நிலை 'B':	type = MBLOCK;	நிறுத்து;
#endif
		}
	    }
	    இல்லையெனில்
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
    வெறுமை
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    முழு
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    திரும்பு FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    வெறுமை
clip_mch_set_selection(Clipboard_T *cbd)
{
    எனில் (be_clipboard->Lock())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->Data();
	ப்ரதிஜ்ஞா(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = வெற்று;
	long_u	மொத்தம்;
	முழு type;

	type = clip_convert_selection(&str, &மொத்தம், cbd);

	எனில் (type < 0)
	    திரும்பு;

	m->AddData(textplain, B_MIME_TYPE, (வெறுமை *)str, மொத்தம்);

	// Add type of selection
	எழுத்து	vtype;
	தேர்வு (type)
	{
	    இயல்பு:
	    நிலை MLINE:    vtype = 'L';    நிறுத்து;
	    நிலை MCHAR:    vtype = 'C';    நிறுத்து;
#ifdef FEAT_VISUAL
	    நிலை MBLOCK:   vtype = 'B';    நிறுத்து;
#endif
	}
	m->AddData(vimselectiontype, B_MIME_TYPE, (வெறுமை *)&vtype, 1);

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
	முழு saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = புதிய BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    வெற்று, வெற்று, 0, பொய்,
	    புதிய BMessage((saving == TRUE) ? 'save' : 'open'), வெற்று, உண்மை);

    gui.vimApp->fBrowsedPath.Unset();

    gui.vimApp->fFilePanel->Window()->SetTitle((எழுத்து*)title);
    gui.vimApp->fFilePanel->SetPanelDirectory((மாறிலி எழுத்து*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    போது (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = வெற்று;
    status_t result = gui.vimApp->fBrowsedPath.InitCheck();
    எனில் (result == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->fBrowsedPath.Path());
    } இல்லையெனில்
	எனில் (result != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    result, strerror(result));
	}

    delete gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = வெற்று;

    திரும்பு fileName;
}
#endif // FEAT_BROWSE


#எனில் defined(FEAT_GUI_DIALOG)

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

முழு
gui_mch_dialog(
	முழு	 type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	முழு	 dfltbutton,
	char_u	*textfield,
	முழு ex_cmd)
{
    VimDialog *dialog = புதிய VimDialog(type, (எழுத்து*)title, (எழுத்து*)message,
	    (எழுத்து*)buttons, dfltbutton, (எழுத்து*)textfield, ex_cmd);
    தருக்க ret = dialog->Go();
    delete dialog;
	திரும்பு ret;
}

#endif // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_TO_RGB(pixel);

    திரும்பு ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    வெறுமை
gui_mch_setmouse(முழு x, முழு y)
{
    TRACE();
    // TODO
}

#ifdef FEAT_MBYTE_IME
    வெறுமை
im_set_position(முழு row, முழு col)
{
    எனில் (gui.vimWindow->Lock())
    {
	gui.vimTextArea->DrawIMString();
	gui.vimWindow->Unlock();
    }
    திரும்பு;
}
#endif

    வெறுமை
gui_mch_show_toolbar(முழு showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_height = (toolbar && showit) ? toolbar->ToolbarHeight() : 0.;
}

    வெறுமை
gui_mch_set_toolbar_pos(முழு x, முழு y, முழு w, முழு h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    எனில் (toolbar != வெற்று) {
	எனில் (gui.vimWindow->Lock()) {
	    toolbar->MoveTo(x, y);
	    toolbar->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

#எனில் defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    வெறுமை
gui_mch_show_tabline(முழு showit)
{
    Vim_Tab_வரி *tab_வரி = gui.vimForm->Tab_வரி();

    எனில் (tab_வரி == வெற்று)
	திரும்பு;

    எனில் (!showit != !gui.vimForm->Is_Showing_Tab_வரி()) {
	gui.vimForm->Set_Showing_Tab_வரி(showit != 0);
	gui.tabline_height = gui.vimForm->TablineHeight();
    }
}

    வெறுமை
gui_mch_set_tabline_pos(முழு x, முழு y, முழு w, முழு h)
{
    Vim_Tab_வரி *tab_வரி = gui.vimForm->Tab_வரி();
    எனில் (tab_வரி != வெற்று) {
	எனில் (gui.vimWindow->Lock()) {
	    tab_வரி->MoveTo(x, y);
	    tab_வரி->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    முழு
gui_mch_showing_tabline()
{
    Vim_Tab_வரி *tab_வரி = gui.vimForm->Tab_வரி();
    திரும்பு tab_வரி != வெற்று && gui.vimForm->Is_Showing_Tab_வரி();
}

/*
 * Update the labels of the tabline.
 */
    வெறுமை
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    முழு	    nr = 0;
    முழு	    curtabidx = 0;

    Vim_Tab_வரி *tab_வரி = gui.vimForm->Tab_வரி();

    எனில் (tab_வரி == வெற்று)
	திரும்பு;

    gui.vimWindow->Lock();

    // Add a label for each tab page.  They all contain the same text area.
    க்கு (tp = first_tabpage; tp != வெற்று; tp = tp->tp_next, ++nr) {
	எனில் (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_வரி->TabAt(nr);

	எனில் (tab == வெற்று) {
	    tab = புதிய Vim_Tab_வரி::VimTab();
	    tab_வரி->AddTab(வெற்று, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->SetLabel((மாறிலி எழுத்து*)NameBuff);
	tab_வரி->Invalidate();
    }

    // Remove any old labels.
    போது (nr < tab_வரி->மொத்தம்_Tabs())
	tab_வரி->RemoveTab(nr);

    எனில் (tab_வரி->Selection() != curtabidx)
	tab_வரி->Select(curtabidx);

    gui.vimWindow->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    வெறுமை
gui_mch_set_curtab(முழு nr)
{
    Vim_Tab_வரி *tab_வரி = gui.vimForm->Tab_வரி();
    எனில் (tab_வரி == வெற்று)
	திரும்பு;

    gui.vimWindow->Lock();

    எனில் (tab_வரி->Selection() != nr -1)
	tab_வரி->Select(nr -1);

    gui.vimWindow->Unlock();
}

#endif // FEAT_GUI_TABLINE
