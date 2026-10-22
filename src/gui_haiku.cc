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

બાહ્ય "C" {

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <float.h>
#અન્તર્ભાવ <syslog.h>

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#અન્તર્ભાવ <Application.h>
#અન્તર્ભાવ <Beep.h>
#અન્તર્ભાવ <Bitmap.h>
#અન્તર્ભાવ <Box.h>
#અન્તર્ભાવ <Button.h>
#અન્તર્ભાવ <Clipboard.h>
#અન્તર્ભાવ <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#અન્તર્ભાવ <File.h>
#અન્તર્ભાવ <FilePanel.h>
#અન્તર્ભાવ <FindDirectory.h>
//#include <Font.h>
#અન્તર્ભાવ <IconUtils.h>
#અન્તર્ભાવ <Input.h>
#અન્તર્ભાવ <ListView.h>
#અન્તર્ભાવ <MenuBar.h>
#અન્તર્ભાવ <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#અન્તર્ભાવ <Path.h>
#અન્તર્ભાવ <PictureButton.h>
#અન્તર્ભાવ <PopUpMenu.h>
//#include <Region.h>
#અન્તર્ભાવ <Resources.h>
//#include <Roster.h>
#અન્તર્ભાવ <Screen.h>
#અન્તર્ભાવ <ScrollBar.h>
#અન્તર્ભાવ <ScrollView.h>
#અન્તર્ભાવ <String.h>
#અન્તર્ભાવ <StringView.h>
//#include <SupportDefs.h>
#અન્તર્ભાવ <TabView.h>
#અન્તર્ભાવ <TextControl.h>
#અન્તર્ભાવ <TextView.h>
#અન્તર્ભાવ <TranslationUtils.h>
#અન્તર્ભાવ <TranslatorFormats.h>
#અન્તર્ભાવ <View.h>
#અન્તર્ભાવ <Window.h>

વર્ગ VimApp;
વર્ગ VimFormView;
વર્ગ VimTextAreaView;
વર્ગ VimWindow;
વર્ગ VimToolbar;
વર્ગ Vim_Tab_લીટી;

બાહ્ય ચાવી_map *ચાવી_Map;
બાહ્ય અક્ષર *ચાવી_Map_Chars;

બાહ્ય પૂર્ણાંક મુખ્ય(પૂર્ણાંક argc, અક્ષર **argv);

#ifndef B_MAX_PORT_ગણતરી
#define B_MAX_PORT_ગણતરી    255
#endif

// VimApp seems comparable to the X "vimShell"
વર્ગ VimApp: જાહેર BApplication
{
    પ્રકારવ્યાખ્યા BApplication Inherited;
    જાહેર:
    VimApp(અચલ અક્ષર *appsig);
    ~VimApp();

    // callbacks:
#જો 0
    આભાસી શૂન્ય DispatchMessage(BMessage *m, BHandler *h)
    {
	m->PrintToStream();
	Inherited::DispatchMessage(m, h);
    }
#endif
    આભાસી શૂન્ય ReadyToRun();
    આભાસી શૂન્ય ArgvReceived(int32 argc, અક્ષર **argv);
    આભાસી શૂન્ય RefsReceived(BMessage *m);
    આભાસી તાર્કિક QuitRequested();
    આભાસી શૂન્ય MessageReceived(BMessage *m);

    સ્થિર શૂન્ય SendRefs(BMessage *m, તાર્કિક changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	fBrowsedPath;
    ખાનગી:
};

વર્ગ VimWindow: જાહેર BWindow
{
    પ્રકારવ્યાખ્યા BWindow Inherited;
    જાહેર:
    VimWindow();
    ~VimWindow();

    આભાસી શૂન્ય DispatchMessage(BMessage *m, BHandler *h);
    આભાસી શૂન્ય WindowActivated(તાર્કિક active);
    આભાસી તાર્કિક QuitRequested();

    VimFormView	    *formView;

    ખાનગી:
    શૂન્ય init();

    તાર્કિક is_fullscreen = ખોટું;
    BRect saved_frame;
    window_look saved_look;
};

વર્ગ VimFormView: જાહેર BView
{
    પ્રકારવ્યાખ્યા BView Inherited;
    જાહેર:
    VimFormView(BRect frame);
    ~VimFormView();

    // callbacks:
    આભાસી શૂન્ય AllAttached();
    આભાસી શૂન્ય FrameResized(દશાંશ new_width, દશાંશ new_height);

#define MENUBAR_MARGIN	1
    દશાંશ MenuHeight() અચલ
    { પાછા menuBar ? menuBar->Frame().Height() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() અચલ
    { પાછા menuBar; }

    ખાનગી:
    શૂન્ય init(BRect);

    BMenuBar	    *menuBar;
    VimTextAreaView *textArea;

#ifdef FEAT_TOOLBAR
    જાહેર:
    દશાંશ ToolbarHeight() અચલ;
    VimToolbar *ToolBar() અચલ
    { પાછા toolBar; }
    ખાનગી:
    VimToolbar	    *toolBar;
#endif

#ifdef FEAT_GUI_TABLINE
    જાહેર:
    Vim_Tab_લીટી *Tab_લીટી() અચલ	{ પાછા tab_લીટી; }
    તાર્કિક Is_Showing_Tab_લીટી() અચલ { પાછા showing_Tab_લીટી; }
    શૂન્ય Set_Showing_Tab_લીટી(તાર્કિક showing) { showing_Tab_લીટી = showing;	}
    દશાંશ TablineHeight() અચલ;
    ખાનગી:
    Vim_Tab_લીટી	*tab_લીટી;
    પૂર્ણાંક	showing_Tab_લીટી;
#endif
};

વર્ગ VimTextAreaView: જાહેર BView
{
    પ્રકારવ્યાખ્યા BView Inherited;
    જાહેર:
    VimTextAreaView(BRect frame);
    ~VimTextAreaView();

    // callbacks:
    આભાસી શૂન્ય Draw(BRect updateRect);
    આભાસી શૂન્ય ચાવી_Down(અચલ અક્ષર *bytes, int32 અંક_Bytes);
    આભાસી શૂન્ય MouseDown(BPoint point);
    આભાસી શૂન્ય MouseUp(BPoint point);
    આભાસી શૂન્ય MouseMoved(BPoint point, uint32 transit, અચલ BMessage *message);
    આભાસી શૂન્ય MessageReceived(BMessage *m);

    // own functions:
    પૂર્ણાંક mchInitFont(char_u *name);
    શૂન્ય mchDrawString(પૂર્ણાંક row, પૂર્ણાંક col, char_u *s, પૂર્ણાંક len, પૂર્ણાંક flags);
    શૂન્ય mchClearBlock(પૂર્ણાંક row1, પૂર્ણાંક col1, પૂર્ણાંક row2, પૂર્ણાંક col2);
    શૂન્ય mchClearAll();
    શૂન્ય mch_Delete_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક અંક_લીટીઓ);
    શૂન્ય mch_ઉમેરો_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક અંક_લીટીઓ);

    સ્થિર શૂન્ય guiSendMouseEvent(પૂર્ણાંક button, પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક repeated_click, int_u modifiers);
    સ્થિર શૂન્ય guiMouseMoved(પૂર્ણાંક x, પૂર્ણાંક y);
    સ્થિર શૂન્ય guiBlankMouse(તાર્કિક should_hide);
    સ્થિર int_u mouseModifiersToVim(int32 beModifiers);

    int32 mouse_Drag_Event_ગણતરી;

#ifdef FEAT_MBYTE_IME
    શૂન્ય DrawIMString(શૂન્ય);
#endif

    ખાનગી:
    શૂન્ય init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#ifdef FEAT_MBYTE_IME
    સંરચના {
	BMessenger* messenger;
	BMessage* message;
	BPoint location;
	પૂર્ણાંક row;
	પૂર્ણાંક col;
	પૂર્ણાંક ગણતરી;
    } IMData;
#endif
};

વર્ગ VimScrollBar: જાહેર BScrollBar
{
    પ્રકારવ્યાખ્યા BScrollBar Inherited;
    જાહેર:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    આભાસી શૂન્ય કિંમત_Changed(દશાંશ new_કિંમત);
    આભાસી શૂન્ય MouseUp(BPoint where);
    શૂન્ય Set_કિંમત(દશાંશ newval);
    scrollbar_T *getGsb()
    { પાછા gsb; }

    int32	scroll_Event_ગણતરી;

    ખાનગી:
    scrollbar_T *gsb;
    દશાંશ   ignore_કિંમત;
};


#ifdef FEAT_TOOLBAR

વર્ગ VimToolbar : જાહેર BBox
{
    સ્થિર BBitmap *normalButtonsBitmap;
    સ્થિર BBitmap *grayedButtonsBitmap;

    BBitmap *LoadVimBitmap(અચલ અક્ષર* fileName);
    તાર્કિક GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, તાર્કિક pressed);
    તાર્કિક ModifyBitmapToGrayed(BBitmap *bitmap);

    BList fButtonsList;
    શૂન્ય InvalidateLayout();

    જાહેર:
    VimToolbar(BRect frame, અચલ અક્ષર * name);
    ~VimToolbar();

    તાર્કિક PrepareButtonBitmaps();

    તાર્કિક AddButton(int32 index, vimmenu_T *menu);
    તાર્કિક RemoveButton(vimmenu_T *menu);
    તાર્કિક GrayButton(vimmenu_T *menu, પૂર્ણાંક grey);

    દશાંશ ToolbarHeight() અચલ;
    આભાસી શૂન્ય AttachedToWindow();
};

BBitmap *VimToolbar::normalButtonsBitmap  = ખાલી;
BBitmap *VimToolbar::grayedButtonsBitmap  = ખાલી;

અચલ દશાંશ ToolbarMargin = 3.;
અચલ દશાંશ ButtonMargin  = 3.;

#endif //FEAT_TOOLBAR

#ifdef FEAT_GUI_TABLINE

વર્ગ Vim_Tab_લીટી : જાહેર BTabView
{
    જાહેર:
	વર્ગ VimTab : જાહેર BTab {
	    જાહેર:
		VimTab() : BTab(નવું BView(BRect(), "-Empty-", 0, 0)) {}

	    આભાસી શૂન્ય Select(BView* owner);
	};

	Vim_Tab_લીટી(BRect r) : BTabView(r, "vimTabLine", B_WIDTH_FROM_LABEL,
	       B_FOLLOW_ડાબું | B_FOLLOW_ટોચ | B_FOLLOW_જમણું, B_WILL_DRAW | B_FRAME_EVENTS) {}

    દશાંશ TablineHeight() અચલ;
    આભાસી શૂન્ય MouseDown(BPoint point);
};

#endif //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
વર્ગ VimFont: જાહેર BFont
{
    પ્રકારવ્યાખ્યા BFont Inherited;
    જાહેર:
    VimFont();
    VimFont(અચલ VimFont *rhs);
    VimFont(અચલ BFont *rhs);
    VimFont(અચલ VimFont &rhs);
    ~VimFont();

    VimFont *next;
    પૂર્ણાંક refcount;
    char_u *name;

    ખાનગી:
    શૂન્ય init();
};

#જો defined(FEAT_GUI_DIALOG)

વર્ગ VimDialog : જાહેર BWindow
{
    પ્રકારવ્યાખ્યા BWindow Inherited;

    BButton* _CreateButton(int32 which, અચલ અક્ષર* label);

    જાહેર:

    વર્ગ View : જાહેર BView {
	પ્રકારવ્યાખ્યા BView Inherited;

	જાહેર:
	View(BRect frame);
	~View();

	આભાસી શૂન્ય Draw(BRect updateRect);
	શૂન્ય InitIcon(int32 type);

	ખાનગી:
	BBitmap*    fIconBitmap;
    };

    VimDialog(પૂર્ણાંક type, અચલ અક્ષર *title, અચલ અક્ષર *message,
	    અચલ અક્ષર *buttons, પૂર્ણાંક dfltbutton, અચલ અક્ષર *textfield,
	    પૂર્ણાંક ex_cmd);
    ~VimDialog();

    પૂર્ણાંક Go();

    આભાસી શૂન્ય MessageReceived(BMessage *સંદેશ);

    ખાનગી:
    sem_id	    fDialogSem;
    પૂર્ણાંક		    f_Dialog_કિંમત;
    BList	    fButtonsList;
    BTextView*	    fMessageView;
    BTextControl*   f_આગત_Control;
    અચલ અક્ષર*	    f_આગત_કિંમત;
};

વર્ગ VimSelectFontDialog : જાહેર BWindow
{
    પ્રકારવ્યાખ્યા BWindow Inherited;

    શૂન્ય _CleanList(BListView* list);
    શૂન્ય _UpdateFontStyles();
    શૂન્ય Update_Size_આગત_Preview();
    શૂન્ય _UpdateFontPreview();
    તાર્કિક _UpdateFromListItem(BListView* list, અક્ષર* text, પૂર્ણાંક textSize);
    જાહેર:

    VimSelectFontDialog(font_family* family, font_style* style, દશાંશ* size);
    ~VimSelectFontDialog();

    તાર્કિક Go();

    આભાસી શૂન્ય MessageReceived(BMessage *સંદેશ);

    ખાનગી:
    status_t	    fStatus;
    sem_id	    fDialogSem;
    તાર્કિક	    f_Dialog_કિંમત;
    font_family*    fFamily;
    font_style*	    fStyle;
    દશાંશ*	    fSize;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    દશાંશ	    fFontSize;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_આગત;
};

#endif // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

સંરચના MainArgs {
    પૂર્ણાંક	     argc;
    અક્ષર    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#define	ચાવી_સંદેશ_BUFSIZ	7
#જો ચાવી_સંદેશ_BUFSIZ < MAX_ચાવી_CODE_LEN
#error Increase ચાવી_સંદેશ_BUFSIZ!
#endif

સંરચના Vim_ચાવી_સંદેશ {
    char_u  length;
    char_u  chars[ચાવી_સંદેશ_BUFSIZ];  // contains Vim encoding
    તાર્કિક    csi_escape;
};

સંરચના Vim_Resize_સંદેશ {
    પૂર્ણાંક	    width;
    પૂર્ણાંક	    height;
};

સંરચના Vim_Scroll_Bar_સંદેશ {
    VimScrollBar *sb;
    long    કિંમત;
    પૂર્ણાંક	    stillDragging;
};

સંરચના Vim_Menu_સંદેશ {
    vimmenu_T	*guiMenu;
};

સંરચના Vim_Mouse_સંદેશ {
    પૂર્ણાંક	    button;
    પૂર્ણાંક	    x;
    પૂર્ણાંક	    y;
    પૂર્ણાંક	    repeated_click;
    int_u   modifiers;
};

સંરચના Vim_Mouse_Moved_સંદેશ {
    પૂર્ણાંક	    x;
    પૂર્ણાંક	    y;
};

સંરચના Vim_Focus_સંદેશ {
    તાર્કિક    active;
};

સંરચના Vim_Refs_સંદેશ {
    BMessage   *message;
    તાર્કિક    changedir;
};

સંરચના Vim_Tabline_સંદેશ {
    પૂર્ણાંક	    index;
};

સંરચના Vim_Tabline_Menu_સંદેશ {
    પૂર્ણાંક	    index;
    પૂર્ણાંક	    event;
};

સંરચના Vim_સંદેશ {
    ગણના Vim_સંદેશ_Type {
	ચાવી_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    સંઘ {
	સંરચના Vim_ચાવી_સંદેશ    ચાવી_2;
	સંરચના Vim_Resize_સંદેશ NewSize;
	સંરચના Vim_Scroll_Bar_સંદેશ	Scroll;
	સંરચના Vim_Menu_સંદેશ   Menu;
	સંરચના Vim_Mouse_સંદેશ  Mouse;
	સંરચના Vim_Mouse_Moved_સંદેશ	MouseMoved;
	સંરચના Vim_Focus_સંદેશ  Focus;
	સંરચના Vim_Refs_સંદેશ   Refs;
	સંરચના Vim_Tabline_સંદેશ	Tabline;
	સંરચના Vim_Tabline_Menu_સંદેશ    TablineMenu;
    } u;
};

#define RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#define GUI_TO_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

સ્થિર સંરચના specialkey
{
    uint16  Be_ચાવીઓ;
#define ચાવી_3(a,b)    ((a)<<8|(b))
#define K(a)	    ચાવી_3(0,a)		// for ASCII codes
#define F(b)	    ચાવી_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_ચાવીઓ[] =
{
    {K(B_UP_ARROW),	'k', 'u'},
    {K(B_DOWN_ARROW),	    'k', 'd'},
    {K(B_ડાબું_ARROW),	    'k', 'l'},
    {K(B_જમણું_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_ઉમેરો),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_END),		'@', '7'},
    {K(B_PAGE_UP),	'k', 'P'},	// XK_Prior
    {K(B_PAGE_DOWN),	    'k', 'N'},	    // XK_Next,

#define FIRST_FUNCTION_ચાવી  11
    {F(B_F1_ચાવી),	'k', '1'},
    {F(B_F2_ચાવી),	'k', '2'},
    {F(B_F3_ચાવી),	'k', '3'},
    {F(B_F4_ચાવી),	'k', '4'},
    {F(B_F5_ચાવી),	'k', '5'},
    {F(B_F6_ચાવી),	'k', '6'},
    {F(B_F7_ચાવી),	'k', '7'},
    {F(B_F8_ચાવી),	'k', '8'},
    {F(B_F9_ચાવી),	'k', '9'},
    {F(B_F10_ચાવી),	'k', ';'},

    {F(B_F11_ચાવી),	'F', '1'},
    {F(B_F12_ચાવી),	'F', '2'},
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
    {F(B_PRINT_ચાવી),	    '%', '9'},

#જો 0
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

#define અંક_SPECIAL_ચાવીઓ    ARRAY_LENGTH(special_ચાવીઓ)

// ---------------- VimApp ----------------

    સ્થિર શૂન્ય
docd(BPath &path)
{
    mch_chdir((અક્ષર *)path.Path());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	સ્થિર શૂન્ય
drop_callback(શૂન્ય *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	સ્થિર શૂન્ય
RefsReceived(BMessage *m, તાર્કિક changedir)
{
    uint32 type;
    int32 ગણતરી;

    m->PrintToStream();
    પસંદગી (m->what) {
	સ્થિતિ B_REFS_RECEIVED:
	સ્થિતિ B_SIMPLE_DATA:
	    m->GetInfo("refs", &type, &ગણતરી);
	    જો (type != B_REF_TYPE)
		જાઓ bad;
	    તોડો;
	સ્થિતિ B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &ગણતરી);
	    જો (type != B_STRING_TYPE)
		જાઓ bad;
	    જો (changedir) {
		અક્ષર *dirname;
		જો (m->FindString("cwd", (અચલ અક્ષર **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    તોડો;
	મૂળભૂત:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    delete m;
	    પાછા;
    }

#ifdef FEAT_VISUAL
    reset_VIsual();
#endif

    char_u  **fnames;
    fnames = (char_u **) alloc(ગણતરી * કદ(char_u *));
    પૂર્ણાંક fname_index = 0;

    પસંદગી (m->what) {
	સ્થિતિ B_REFS_RECEIVED:
	સ્થિતિ B_SIMPLE_DATA:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    માટે (પૂર્ણાંક i = 0; i < ગણતરી; ++i)
	    {
		entry_ref ref;
		જો (m->FindRef("refs", i, &ref) == B_OK) {
		    BEntry entry(&ref, ખોટું);
		    BPath path;
		    entry.GetPath(&path);

		    // Change to parent directory?
		    જો (changedir) {
			BPath parentpath;
			path.GetParent(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    જો (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			જો (!changedir)
			    docd(path);
		    } નહીંતર {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)path.Path(), IObuff);
			જો (fname == ખાલી)
			    fname = (char_u *)path.Path();
			fnames[fname_index++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = ખોટું;
		}
	    }
	    તોડો;
	સ્થિતિ B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    માટે (પૂર્ણાંક i = 1; i < ગણતરી; ++i)
	    {
		અક્ષર *fname;

		જો (m->FindString("argv", i, (અચલ અક્ષર **) &fname) == B_OK) {
		    fnames[fname_index++] = vim_strsave((char_u *)fname);
		}
	    }
	    તોડો;
	મૂળભૂત:
	    // fprintf(stderr, "case default\n");
	    તોડો;
    }

    delete m;

    // Handle the drop, :edit to get to the file
    જો (fname_index > 0) {
	handle_drop(fname_index, fnames, FALSE, drop_callback, ખાલી);

	setcursor();
	out_flush();
    } નહીંતર {
	vim_free(fnames);
    }
}

VimApp::VimApp(અચલ અક્ષર *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(ખાલી)
{
}

VimApp::~VimApp()
{
}

    શૂન્ય
VimApp::ReadyToRun()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    શૂન્ય
VimApp::ArgvReceived(int32 arg_argc, અક્ષર **arg_argv)
{
    જો (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	જો (gui.vimWindow)
	    gui.vimWindow->Minimize(ખોટું);
	BMessage *m = CurrentMessage();
	DetachCurrentMessage();
	SendRefs(m, સાચું);
    }
}

    શૂન્ય
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    પૂર્ણાંક limit = 15;
    જ્યારે (--limit >= 0 && (curbuf == ખાલી || curbuf->b_start_ffc == 0))
	snooze(100000);    //  0.1 s
    જો (gui.vimWindow)
	gui.vimWindow->Minimize(ખોટું);
    DetachCurrentMessage();
    SendRefs(m, સાચું);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    શૂન્ય
VimApp::SendRefs(BMessage *m, તાર્કિક changedir)
{
    Vim_Refs_સંદેશ rm;
    rm.message = m;
    rm.changedir = changedir;

    write_port(gui.vdcmp, Vim_સંદેશ::Refs, &rm, કદ(rm));
    //	calls ::RefsReceived
}

    શૂન્ય
VimApp::MessageReceived(BMessage *m)
{
    પસંદગી (m->what) {
	સ્થિતિ 'save':
	    {
		entry_ref refDirectory;
		m->FindRef("directory", &refDirectory);
		fBrowsedPath.SetTo(&refDirectory);
		BString strName;
		m->FindString("name", &strName);
		fBrowsedPath.Append(strName.String());
	    }
	    તોડો;
	સ્થિતિ 'open':
	    {
		entry_ref ref;
		m->FindRef("refs", &ref);
		fBrowsedPath.SetTo(&ref);
	    }
	    તોડો;
	સ્થિતિ B_CANCEL:
	    {
		BFilePanel *panel;
		m->FindPointer("source", (શૂન્ય**)&panel);
		જો (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    તોડો;
	મૂળભૂત:
	    Inherited::MessageReceived(m);
	    તોડો;
    }
}

    તાર્કિક
VimApp::QuitRequested()
{
    (શૂન્ય)Inherited::QuitRequested();
    પાછા ખોટું;
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
    જો (formView) {
	RemoveChild(formView);
	delete formView;
    }
    gui.vimWindow = ખાલી;
}

    શૂન્ય
VimWindow::init()
{
    // Attach the VimFormView
    formView = નવું VimFormView(Bounds());
    જો (formView != ખાલી) {
	AddChild(formView);
    }
}

#જો 0  //  disabled in zeta patch
    શૂન્ય
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    પસંદગી (m->what) {
	સ્થિતિ B_MOUSE_UP:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		જો (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->FindPoint("where", &where);
		    v->MouseUp(where);
		} નહીંતર {
		    Inherited::DispatchMessage(m, h);
		}
	    }
	    તોડો;
	મૂળભૂત:
	    Inherited::DispatchMessage(m, h);
    }
}
#endif

    શૂન્ય
VimWindow::WindowActivated(તાર્કિક active)
{
    Inherited::WindowActivated(active);
    // the textArea gets the keyboard action
    જો (active && gui.vimTextArea)
	gui.vimTextArea->MakeFocus(સાચું);

    સંરચના Vim_Focus_સંદેશ fm;
    fm.active = active;

    write_port(gui.vdcmp, Vim_સંદેશ::Focus, &fm, કદ(fm));
}

    તાર્કિક
VimWindow::QuitRequested()
{
    સંરચના Vim_ચાવી_સંદેશ km;
    km.length = 5;
    memcpy((અક્ષર *)km.chars, "\033:qa\r", km.length);
    km.csi_escape = ખોટું;
    write_port(gui.vdcmp, Vim_સંદેશ::ચાવી_2, &km, કદ(km));
    પાછા ખોટું;
}
    શૂન્ય
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    તાર્કિક should_propagate = સાચું;

    પસંદગી (m->what)
    {
	સ્થિતિ B_ચાવી_DOWN:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->FindInt32("raw_char", &scancode);
		m->FindInt32("modifiers", &beModifiers);

		જો (scancode == B_ENTER && (beModifiers & B_ડાબું_COMMAND_ચાવી))
		    {
			should_propagate = ખોટું;
			જો (આ->is_fullscreen)
			    {
				આ->is_fullscreen = ખોટું;
				ResizeTo(આ->saved_frame.Width(), આ->saved_frame.Height());
				MoveTo(આ->saved_frame.ડાબું, આ->saved_frame.ટોચ);
				SetLook(આ->saved_look);
				SetFlags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } નહીંતર {
				આ->saved_frame = Frame();
				આ->saved_look = Look();
				આ->is_fullscreen = સાચું;
				BScreen s(આ);
				SetLook(B_NO_BORDER_WINDOW_LOOK);
				ResizeTo(s.Frame().Width() + 1, s.Frame().Height() + 1);
				MoveTo(s.Frame().ડાબું, s.Frame().ટોચ);
				SetFlags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    જો (should_propagate)
	Inherited::DispatchMessage(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect frame):
    BView(frame, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_FRAME_EVENTS),
    menuBar(ખાલી),
#ifdef FEAT_TOOLBAR
    toolBar(ખાલી),
#endif
#ifdef FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_લીટી(ખાલી),
#endif
    textArea(ખાલી)
{
    init(frame);
}

VimFormView::~VimFormView()
{
    જો (menuBar) {
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
    delete tab_લીટી;
#endif

    જો (textArea) {
	RemoveChild(textArea);
	delete textArea;
    }
    gui.vimForm = ખાલી;
}

    શૂન્ય
VimFormView::init(BRect frame)
{
    menuBar = નવું BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    AddChild(menuBar);

#ifdef FEAT_TOOLBAR
    toolBar = નવું VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    AddChild(toolBar);
#endif

#ifdef FEAT_GUI_TABLINE
    tab_લીટી = નવું Vim_Tab_લીટી(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    AddChild(tab_લીટી);
#endif

    BRect remaining = frame;
    textArea = નવું VimTextAreaView(remaining);
    AddChild(textArea);
    // The textArea will be resized later when menus are added

    gui.vimForm = આ;
}

#ifdef FEAT_TOOLBAR
    દશાંશ
VimFormView::ToolbarHeight() અચલ
{
    પાછા toolBar ? toolBar->ToolbarHeight() : 0.;
}
#endif

#ifdef FEAT_GUI_TABLINE
    દશાંશ
VimFormView::TablineHeight() અચલ
{
    પાછા (tab_લીટી && Is_Showing_Tab_લીટી()) ? tab_લીટી->TablineHeight() : 0.;
}
#endif

    શૂન્ય
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    જો (menuBar && textArea) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect remaining = Bounds();

#ifdef FEAT_MENU
	remaining.ટોચ += MenuHeight();
	menuBar->ResizeTo(remaining.જમણું, remaining.ટોચ);
	gui.menu_height = (પૂર્ણાંક) MenuHeight();
#endif

#ifdef FEAT_TOOLBAR
	toolBar->MoveTo(remaining.ડાબું, remaining.ટોચ);
	toolBar->ResizeTo(remaining.જમણું, ToolbarHeight());
	remaining.ટોચ += ToolbarHeight();
	gui.toolbar_height = ToolbarHeight();
#endif

#ifdef FEAT_GUI_TABLINE
	tab_લીટી->MoveTo(remaining.ડાબું, remaining.ટોચ);
	tab_લીટી->ResizeTo(remaining.જમણું + 1, TablineHeight());
	remaining.ટોચ += TablineHeight();
	gui.tabline_height = TablineHeight();
#endif

	textArea->ResizeTo(remaining.Width(), remaining.Height());
	textArea->MoveTo(remaining.ડાબું, remaining.ટોચ);
    }


    Inherited::AllAttached();
}

    શૂન્ય
VimFormView::FrameResized(દશાંશ new_width, દશાંશ new_height)
{
    સંરચના Vim_Resize_સંદેશ sm;
    પૂર્ણાંક adjust_h, adjust_w;

    new_width += 1;	//  adjust from width to number of pixels occupied
    new_height += 1;

    sm.width = (પૂર્ણાંક) new_width;
    sm.height = (પૂર્ણાંક) new_height;
    adjust_w = ((પૂર્ણાંક)new_width - gui_get_base_width()) % gui.char_width;
    adjust_h = ((પૂર્ણાંક)new_height - gui_get_base_height()) % gui.char_height;

    જો (adjust_w > 0 || adjust_h > 0) {
	sm.width  -= adjust_w;
	sm.height -= adjust_h;
    }

    write_port(gui.vdcmp, Vim_સંદેશ::Resize, &sm, કદ(sm));
    //	calls gui_resize_shell(new_width, new_height);

    પાછા;

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
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_આગત_METHOD_AWARE
#નહીંતર
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE
#endif
	),
    mouse_Drag_Event_ગણતરી(0)
{
#ifdef FEAT_MBYTE_IME
    IMData.messenger = ખાલી;
    IMData.message = ખાલી;
#endif
    init(frame);
}

VimTextAreaView::~VimTextAreaView()
{
    gui.vimTextArea = ખાલી;
}

    શૂન્ય
VimTextAreaView::init(BRect frame)
{
    // set up global var for fast access
    gui.vimTextArea = આ;

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

    શૂન્ય
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
    Set_નીચલું_Color(rgb);
    FillRect(updateRect, B_SOLID_નીચલું);
    gui_redraw((પૂર્ણાંક) updateRect.ડાબું, (પૂર્ણાંક) updateRect.ટોચ,
	    (પૂર્ણાંક) (updateRect.Width() + PEN_WIDTH), (પૂર્ણાંક) (updateRect.Height() + PEN_WIDTH));

    // Clear the border areas if needed
    Set_નીચલું_Color(rgb);

    જો (updateRect.ડાબું < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.ડાબું, updateRect.ટોચ,
		    FILL_X(0)-PEN_WIDTH, updateRect.bottom), B_SOLID_નીચલું);
    જો (updateRect.ટોચ < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.ડાબું, updateRect.ટોચ,
		    updateRect.જમણું, FILL_Y(0)-PEN_WIDTH), B_SOLID_નીચલું);
    જો (updateRect.જમણું >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((પૂર્ણાંક)Columns), updateRect.ટોચ,
		    updateRect.જમણું, updateRect.bottom), B_SOLID_નીચલું);
    જો (updateRect.bottom >= FILL_Y(Rows))   //  bottom border
	FillRect(BRect(updateRect.ડાબું, FILL_Y((પૂર્ણાંક)Rows),
		    updateRect.જમણું, updateRect.bottom), B_SOLID_નીચલું);

#ifdef FEAT_MBYTE_IME
    DrawIMString();
#endif
}

    શૂન્ય
VimTextAreaView::ચાવી_Down(અચલ અક્ષર *bytes, int32 અંક_Bytes)
{
    સંરચના Vim_ચાવી_સંદેશ km;
    char_u *dest = km.chars;

    તાર્કિક canHaveVimModifiers = ખોટું;

    BMessage *સંદેશ = Window()->CurrentMessage();
    પ્રતિજ્ઞા(સંદેશ);
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
    જો (અંક_Bytes > 1) {
	// This cannot be a special key
	જો (અંક_Bytes > ચાવી_સંદેશ_BUFSIZ)
	    અંક_Bytes = ચાવી_સંદેશ_BUFSIZ;	    //	should never happen... ???
	km.length = અંક_Bytes;
	memcpy((અક્ષર *)dest, bytes, અંક_Bytes);
	km.csi_escape = સાચું;
    } નહીંતર {
	int32 scancode = 0;
	સંદેશ->FindInt32("key", &scancode);

	int32 beModifiers = 0;
	સંદેશ->FindInt32("modifiers", &beModifiers);

	char_u string[3];
	પૂર્ણાંક len = 0;
	km.length = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	પ્રતિજ્ઞા(B_BACKSPACE <= 0x20);
	પ્રતિજ્ઞા(B_DELETE == 0x7F);
	જો (((char_u)bytes[0] <= 0x20 || (char_u)bytes[0] == 0x7F) &&
		અંક_Bytes == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    જો (beModifiers & B_CONTROL_ચાવી) {
		પૂર્ણાંક index = ચાવી_Map->normal_map[scancode];
		પૂર્ણાંક new_અંક_Bytes = ચાવી_Map_Chars[index];
		char_u *newBytes = (char_u *)&ચાવી_Map_Chars[index + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		જો (new_અંક_Bytes != 1 || (*newBytes > 0x20 &&
			    *newBytes != 0x7F )) {
		    જાઓ notspecial;
		}
		bytes = (અક્ષર *)newBytes;
	    }
	    canHaveVimModifiers = સાચું;

	    uint16 beoskey;
	    પૂર્ણાંક first, last;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    જો (અંક_Bytes == 0 || bytes[0] == B_FUNCTION_ચાવી) {
		beoskey = F(scancode);
		first = FIRST_FUNCTION_ચાવી;
		last = અંક_SPECIAL_ચાવીઓ;
	    } નહીંતર જો (*bytes == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		first = last = 0;
	    } નહીંતર {
		beoskey = K(bytes[0]);
		first = 0;
		last = FIRST_FUNCTION_ચાવી;
	    }

	    માટે (પૂર્ણાંક i = first; i < last; i++) {
		જો (special_ચાવીઓ[i].Be_ચાવીઓ == beoskey) {
		    string[0] = CSI;
		    string[1] = special_ચાવીઓ[i].vim_code0;
		    string[2] = special_ચાવીઓ[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	જો (len == 0) {
	    string[0] = bytes[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#જો 0
	જો (len == 3 ||
		bytes[0] == B_SPACE || bytes[0] == B_TAB ||
		bytes[0] == B_RETURN || bytes[0] == '\r' ||
		bytes[0] == B_ESCAPE)
#નહીંતર
	    જો (canHaveVimModifiers)
#endif
	    {
		પૂર્ણાંક modifiers;
		modifiers = 0;
		જો (beModifiers & B_SHIFT_ચાવી)
		    modifiers |= MOD_MASK_SHIFT;
		જો (beModifiers & B_CONTROL_ચાવી)
		    modifiers |= MOD_MASK_CTRL;
		જો (beModifiers & B_OPTION_ચાવી)
		    modifiers |= MOD_MASK_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		પૂર્ણાંક ચાવી;
		જો (string[0] == CSI && len == 3)
		    ચાવી = TO_SPECIAL(string[1], string[2]);
		નહીંતર
		    ચાવી = string[0];
		ચાવી = simplify_ચાવી(ચાવી, &modifiers);
		જો (IS_SPECIAL(ચાવી))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(ચાવી);
		    string[2] = K_THIRD(ચાવી);
		    len = 3;
		}
		નહીંતર
		{
		    string[0] = ચાવી;
		    len = 1;
		}

		જો (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.length = 3;
		}
	    }
	memcpy((અક્ષર *)dest, string, len);
	km.length += len;
	km.csi_escape = ખોટું;
    }

    write_port(gui.vdcmp, Vim_સંદેશ::ચાવી_2, &km, કદ(km));

    /*
     * blank out the pointer if necessary
     */
    જો (p_mh && !gui.pointer_hidden)
    {
	guiBlankMouse(સાચું);
	gui.pointer_hidden = TRUE;
    }
}
શૂન્ય
VimTextAreaView::guiSendMouseEvent(
	પૂર્ણાંક	button,
	પૂર્ણાંક	x,
	પૂર્ણાંક	y,
	પૂર્ણાંક	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_સંદેશ mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_port(gui.vdcmp, Vim_સંદેશ::Mouse, &mm, કદ(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    જો (gui.pointer_hidden)
    {
	guiBlankMouse(ખોટું);
	gui.pointer_hidden = FALSE;
    }
}

શૂન્ય
VimTextAreaView::guiMouseMoved(
	પૂર્ણાંક	x,
	પૂર્ણાંક	y)
{
    Vim_Mouse_Moved_સંદેશ mm;

    mm.x = x;
    mm.y = y;

    write_port(gui.vdcmp, Vim_સંદેશ::MouseMoved, &mm, કદ(mm));

    જો (gui.pointer_hidden)
    {
	guiBlankMouse(ખોટું);
	gui.pointer_hidden = FALSE;
    }
}

    શૂન્ય
VimTextAreaView::guiBlankMouse(તાર્કિક should_hide)
{
    જો (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } નહીંતર {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
VimTextAreaView::mouseModifiersToVim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    જો (beModifiers & B_SHIFT_ચાવી)
	vim_modifiers |= MOUSE_SHIFT;
    જો (beModifiers & B_CONTROL_ચાવી)
	vim_modifiers |= MOUSE_CTRL;
    જો (beModifiers & B_OPTION_ચાવી)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    પાછા vim_modifiers;
}

    શૂન્ય
VimTextAreaView::MouseDown(BPoint point)
{
    BMessage *m = Window()->CurrentMessage();
    પ્રતિજ્ઞા(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    પૂર્ણાંક vimButton;

    જો (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_ડાબું;
    નહીંતર જો (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_જમણું;
    નહીંતર જો (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_મધ્ય;
    નહીંતર
	પાછા;		// Unknown button

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

    શૂન્ય
VimTextAreaView::MouseUp(BPoint point)
{
    vimMouseButton = 0;

    BMessage *m = Window()->CurrentMessage();
    પ્રતિજ્ઞા(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->FindInt32("modifiers", &modifiers);

    vimMouseModifiers = mouseModifiersToVim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, point.x, point.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::MouseUp(point);
}

    શૂન્ય
VimTextAreaView::MouseMoved(BPoint point, uint32 transit, અચલ BMessage *message)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    જો (gui.pointer_hidden)
    {
	guiBlankMouse(ખોટું);
	gui.pointer_hidden = FALSE;
    }

    જો (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(point.x, point.y);
	પાછા;
    }

    atomic_add(&mouse_Drag_Event_ગણતરી, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, point.x, point.y, 0, vimMouseModifiers);
}

    શૂન્ય
VimTextAreaView::MessageReceived(BMessage *m)
{
    પસંદગી (m->what) {
	સ્થિતિ 'menu':
	    {
		Vim_Menu_સંદેશ mm;
		mm.guiMenu = ખાલી;  // in case no pointer in msg
		m->FindPointer("VimMenu", (શૂન્ય **)&mm.guiMenu);
		write_port(gui.vdcmp, Vim_સંદેશ::Menu, &mm, કદ(mm));
	    }
	    તોડો;
	સ્થિતિ B_MOUSE_WHEEL_CHANGED:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		દશાંશ small=0, big=0, dy=0;
		m->FindFloat("be:wheel_delta_y", &dy);
		scb->GetSteps(&small, &big);
		scb->Set_કિંમત(scb->કિંમત_2()+small*dy*3);
		scb->કિંમત_Changed(scb->કિંમત_2());
#જો 0
		scb = curwin->w_scrollbars[0].id;
		scb->GetSteps(&small, &big);
		scb->Set_કિંમત(scb->કિંમત_2()+small*dy);
		scb->કિંમત_Changed(scb->કિંમત_2());
#endif
	    }
	    તોડો;
#ifdef FEAT_MBYTE_IME
	સ્થિતિ B_આગત_METHOD_EVENT:
	    {
		int32 opcode;
		m->FindInt32("be:opcode", &opcode);
		પસંદગી(opcode)
		{
		    સ્થિતિ B_આગત_METHOD_STARTED:
			જો (!IMData.messenger) delete IMData.messenger;
			IMData.messenger = નવું BMessenger();
			m->FindMessenger("be:reply_to", IMData.messenger);
			તોડો;
		    સ્થિતિ B_આગત_METHOD_CHANGED:
			{
			    BString str;
			    તાર્કિક confirmed;
			    જો (IMData.message) *(IMData.message) = *m;
			    નહીંતર	       IMData.message = નવું BMessage(*m);
			    DrawIMString();
			    m->FindBool("be:confirmed", &confirmed);
			    જો (confirmed)
			    {
				m->FindString("be:string", &str);
				char_u *chars = (char_u*)str.String();
				સંરચના Vim_ચાવી_સંદેશ km;
				km.csi_escape = સાચું;
				પૂર્ણાંક clen;
				પૂર્ણાંક i = 0;
				જ્યારે (i < str.Length())
				{
				    clen = utf_ptr2len(chars+i);
				    memcpy(km.chars, chars+i, clen);
				    km.length = clen;
				    write_port(gui.vdcmp, Vim_સંદેશ::ચાવી_2, &km, કદ(km));
				    i += clen;
				}
			    }
			}
			તોડો;
		    સ્થિતિ B_આગત_METHOD_LOCATION_REQUEST:
			{
			    BMessage સંદેશ(B_આગત_METHOD_EVENT);
			    સંદેશ.AddInt32("be:opcode", B_આગત_METHOD_LOCATION_REQUEST);
			    સંદેશ.AddPoint("be:location_reply", IMData.location);
			    સંદેશ.AddFloat("be:height_reply", FILL_Y(1));
			    IMData.messenger->SendMessage(&સંદેશ);
			}
			તોડો;
		    સ્થિતિ B_આગત_METHOD_STOPPED:
			delete IMData.messenger;
			delete IMData.message;
			IMData.messenger = ખાલી;
			IMData.message = ખાલી;
			તોડો;
		}
	    }
	    // TODO: sz: break here???
#endif
	મૂળભૂત:
	    જો (m->WasDropped()) {
		BWindow *w = Window();
		w->DetachCurrentMessage();
		w->Minimize(ખોટું);
		VimApp::SendRefs(m, (modifiers() & B_SHIFT_ચાવી) != 0);
	    } નહીંતર {
		Inherited::MessageReceived(m);
	    }
	    તોડો;
    }
}

    પૂર્ણાંક
VimTextAreaView::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    જો (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_set_font((GuiFont)newFont);
	જો (name && STRCMP(name, "*") != 0)
	    hl_set_font_name(name);

	SetDrawingMode(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	પાછા OK;
    }
    પાછા FAIL;
}

    શૂન્ય
VimTextAreaView::mchDrawString(પૂર્ણાંક row, પૂર્ણાંક col, char_u *s, પૂર્ણાંક len, પૂર્ણાંક flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    જો (!(flags & DRAW_TRANSP)) {
	પૂર્ણાંક cells;
	cells = 0;
	માટે (પૂર્ણાંક i=0; i<len; i++) {
	    પૂર્ણાંક cn = utf_ptr2cells((char_u *)(s+i));
	    જો (cn<4) cells += cn;
	}

	BRect r(FILL_X(col), FILL_Y(row),
		FILL_X(col + cells) - PEN_WIDTH, FILL_Y(row + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_નીચલું);
    }

    BFont font;
    આ->GetFont(&font);
    જો (!font.IsFixed())
    {
	અક્ષર* p = (અક્ષર*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	પૂર્ણાંક cells;
	જ્યારે ((p - (અક્ષર*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.Set(TEXT_X(col+lastpos), TEXT_Y(row));
	    DrawString(p, clen, where);
	    જો (flags & DRAW_BOLD) {
		where.x += 1.0;
		SetDrawingMode(B_OP_BLEND);
		DrawString(p, clen, where);
		SetDrawingMode(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    જો (cells<4) lastpos += cells;
	    નહીંતર	lastpos++;
	    p += clen;
	}
    }
    નહીંતર
    {
	BPoint where(TEXT_X(col), TEXT_Y(row));
	DrawString((અક્ષર*)s, len, where);
	જો (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    SetDrawingMode(B_OP_BLEND);
	    DrawString((અક્ષર*)s, len, where);
	    SetDrawingMode(B_OP_COPY);
	}
    }

    જો (flags & DRAW_UNDERL) {
	પૂર્ણાંક cells;
	cells = 0;
	માટે (પૂર્ણાંક i=0; i<len; i++) {
	    પૂર્ણાંક cn = utf_ptr2cells((char_u *)(s+i));
	    જો (cn<4) cells += cn;
	}

	BPoint start(FILL_X(col), FILL_Y(row + 1) - PEN_WIDTH);
	BPoint end(FILL_X(col + cells) - PEN_WIDTH, start.y);

	Stroke_લીટી(start, end);
    }
}

શૂન્ય
VimTextAreaView::mchClearBlock(
	પૂર્ણાંક	row1,
	પૂર્ણાંક	col1,
	પૂર્ણાંક	row2,
	પૂર્ણાંક	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_WIDTH, FILL_Y(row2 + 1) - PEN_WIDTH);
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(r, B_SOLID_નીચલું);
}

    શૂન્ય
VimTextAreaView::mchClearAll()
{
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(Bounds(), B_SOLID_નીચલું);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    શૂન્ય
VimTextAreaView::mch_Delete_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક અંક_લીટીઓ)
{
    BRect source, dest;
    source.ડાબું = FILL_X(gui.scroll_region_ડાબું);
    source.ટોચ = FILL_Y(row + અંક_લીટીઓ);
    source.જમણું = FILL_X(gui.scroll_region_જમણું + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    dest.ડાબું = FILL_X(gui.scroll_region_ડાબું);
    dest.ટોચ = FILL_Y(row);
    dest.જમણું = FILL_X(gui.scroll_region_જમણું + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot - અંક_લીટીઓ + 1) - PEN_WIDTH;

    જો (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(gui.scroll_region_bot - અંક_લીટીઓ + 1,
		gui.scroll_region_ડાબું,
		gui.scroll_region_bot, gui.scroll_region_જમણું);


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
    શૂન્ય
VimTextAreaView::mch_ઉમેરો_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક અંક_લીટીઓ)
{
    BRect source, dest;

    // XXX Attempt at a hack:
    gui.vimWindow->UpdateIfNeeded();
    source.ડાબું = FILL_X(gui.scroll_region_ડાબું);
    source.ટોચ = FILL_Y(row);
    source.જમણું = FILL_X(gui.scroll_region_જમણું + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot - અંક_લીટીઓ + 1) - PEN_WIDTH;

    dest.ડાબું = FILL_X(gui.scroll_region_ડાબું);
    dest.ટોચ = FILL_Y(row + અંક_લીટીઓ);
    dest.જમણું = FILL_X(gui.scroll_region_જમણું + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    જો (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(row, gui.scroll_region_ડાબું,
		row + અંક_લીટીઓ - 1, gui.scroll_region_જમણું);

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
શૂન્ય VimTextAreaView::DrawIMString(શૂન્ય)
{
    સ્થિર અચલ rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    અચલ અક્ષર* s;
    પૂર્ણાંક len;
    BMessage* સંદેશ = IMData.message;
    જો (!સંદેશ)
	પાછા;
    gui_redraw_block(IMData.row, 0,
	    IMData.row + IMData.ગણતરી, W_WIDTH(curwin), GUI_MON_NOCLEAR);
    તાર્કિક confirmed = ખોટું;
    સંદેશ->FindBool("be:confirmed", &confirmed);
    જો (confirmed)
	પાછા;
    rgb_color hcolor = ઉપલું_Color(), lcolor = નીચલું_Color();
    સંદેશ->FindString("be:string", &str);
    s = str.String();
    len = str.Length();
    Set_ઉપલું_Color(0, 0, 0);
    IMData.row = gui.row;
    IMData.col = gui.col;
    int32 sel_start = 0, sel_end = 0;
    સંદેશ->FindInt32("be:selection", 0, &sel_start);
    સંદેશ->FindInt32("be:selection", 1, &sel_end);
    પૂર્ણાંક clen, cn;
    BPoint pos(IMData.col, 0);
    BRect r;
    BPoint where;
    IMData.location = ConvertToScreen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
    માટે (પૂર્ણાંક i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	જો (pos.x + cn > W_WIDTH(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	જો (sel_start<=i && i<sel_end)
	{
	    Set_નીચલું_Color(r_highlight);
	    IMData.location = ConvertToScreen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
	}
	નહીંતર
	{
	    Set_નીચલું_Color(b_highlight);
	}
	r.Set(FILL_X(pos.x), FILL_Y(IMData.row + pos.y),
		FILL_X(pos.x + cn) - PEN_WIDTH,
		FILL_Y(IMData.row + pos.y + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_નીચલું);
	where.Set(TEXT_X(pos.x), TEXT_Y(IMData.row + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.ગણતરી = (પૂર્ણાંક)pos.y;

    Set_ઉપલું_Color(hcolor);
    Set_નીચલું_Color(lcolor);
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
	    "vim scrollbar", (BView *)ખાલી,
	    0.0, 10.0, posture),
    ignore_કિંમત(-1),
    scroll_Event_ગણતરી(0)
{
    gsb = g;
    SetResizingMode(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    શૂન્ય
VimScrollBar::કિંમત_Changed(દશાંશ new_કિંમત)
{
    જો (ignore_કિંમત >= 0.0 && new_કિંમત == ignore_કિંમત) {
	ignore_કિંમત = -1;
	પાછા;
    }
    ignore_કિંમત = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_add(&scroll_Event_ગણતરી, 1);

    સંરચના Vim_Scroll_Bar_સંદેશ sm;

    sm.sb = આ;
    sm.કિંમત = (long) new_કિંમત;
    sm.stillDragging = TRUE;

    write_port(gui.vdcmp, Vim_સંદેશ::ScrollBar, &sm, કદ(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    શૂન્ય
VimScrollBar::MouseUp(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_add(&scroll_Event_ગણતરી, 1);

    સંરચના Vim_Scroll_Bar_સંદેશ sm;

    sm.sb = આ;
    sm.કિંમત = (long) કિંમત_2();
    sm.stillDragging = FALSE;

    write_port(gui.vdcmp, Vim_સંદેશ::ScrollBar, &sm, કદ(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::MouseUp(where);
}

    શૂન્ય
VimScrollBar::Set_કિંમત(દશાંશ new_કિંમત)
{
    જો (new_કિંમત == કિંમત_2())
	પાછા;

    ignore_કિંમત = new_કિંમત;
    Inherited::Set_કિંમત(new_કિંમત);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(અચલ VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(અચલ BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(અચલ VimFont &rhs): BFont(rhs)
{
    init();
}

VimFont::~VimFont()
{
}

    શૂન્ય
VimFont::init()
{
    next = ખાલી;
    refcount = 1;
    name = ખાલી;
}

// ---------------- VimDialog ----------------

#જો defined(FEAT_GUI_DIALOG)

અચલ unsigned પૂર્ણાંક  k_Vim_Dialog_Button_સંદેશ = 'VMDB';
અચલ unsigned પૂર્ણાંક  kVimDialogIconStripeWidth = 30;
અચલ unsigned પૂર્ણાંક  kVimDialogButtonsSpacingX = 9;
અચલ unsigned પૂર્ણાંક  kVimDialogButtonsSpacingY = 4;
અચલ unsigned પૂર્ણાંક  kVimDialogSpacingX = 6;
અચલ unsigned પૂર્ણાંક  kVimDialogSpacingY = 10;
અચલ unsigned પૂર્ણાંક  kVimDialogMinimalWidth  = 310;
અચલ unsigned પૂર્ણાંક  kVimDialogMinimalHeight = 75;
અચલ BRect	    kDefaultRect =
BRect(0, 0, kVimDialogMinimalWidth, kVimDialogMinimalHeight);

VimDialog::VimDialog(પૂર્ણાંક type, અચલ અક્ષર *title, અચલ અક્ષર *message,
	અચલ અક્ષર *buttons, પૂર્ણાંક dfltbutton, અચલ અક્ષર *textfield, પૂર્ણાંક ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_કિંમત(dfltbutton)
    , fMessageView(ખાલી)
    , f_આગત_Control(ખાલી)
    , f_આગત_કિંમત(textfield)
{
    //	master view
    VimDialog::View* view = નવું VimDialog::View(Bounds());
    જો (view == ખાલી)
	પાછા;

    જો (title == ખાલી)
	SetTitle("Vim " VIM_VERSION_MEDIUM);

    AddChild(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    દશાંશ maxButtonWidth  = 0;
    દશાંશ maxButtonHeight = 0;
    દશાંશ buttonsWidth	  = 0;
    દશાંશ buttonsHeight   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    કરો
    {
	int32 end = strButtons.FindFirst('\n');
	જો (end != B_ERROR)
	    strButtons.SetByteAt(end, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->AddChild(button);
	fButtonsList.AddItem(button);

	maxButtonWidth	= max_c(maxButtonWidth,  button->Bounds().Width());
	maxButtonHeight = max_c(maxButtonHeight, button->Bounds().Height());
	buttonsWidth   += button->Bounds().Width();
	buttonsHeight  += button->Bounds().Height();

	જો (end == B_ERROR)
	    તોડો;

	strButtons.Remove(0, end + 1);
    } જ્યારે (સાચું);

    int32 buttons_ગણતરી = fButtonsList.ગણતરી_વસ્તુઓ();
    buttonsWidth      += kVimDialogButtonsSpacingX * (buttons_ગણતરી - 1);
    buttonsHeight     += kVimDialogButtonsSpacingY * (buttons_ગણતરી - 1);
    દશાંશ dialogWidth  = buttonsWidth + kVimDialogIconStripeWidth +
	kVimDialogSpacingX * 2;
    દશાંશ dialogHeight = maxButtonHeight + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    તાર્કિક vertical = (vim_strchr(p_go, GO_VERTICAL) != ખાલી) ||
	dialogWidth >= gui.vimWindow->Bounds().Width();
    જો (vertical) {
	dialogWidth  -= buttonsWidth;
	dialogWidth  += maxButtonWidth;
	dialogHeight -= maxButtonHeight;
	dialogHeight += buttonsHeight;
    }

    dialogWidth  = max_c(dialogWidth,  kVimDialogMinimalWidth);

    //	message view
    BRect rect(0, 0, dialogWidth, 0);
    rect.ડાબું  += kVimDialogIconStripeWidth + 16 + kVimDialogSpacingX;
    rect.ટોચ   += kVimDialogSpacingY;
    rect.જમણું -= kVimDialogSpacingX;
    rect.bottom = rect.ટોચ;
    fMessageView = નવું BTextView(rect, "_tv_", rect.OffsetByCopy(B_ORIGIN),
	    B_FOLLOW_ડાબું | B_FOLLOW_ટોચ, B_WILL_DRAW);

    fMessageView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color textColor = ui_color(B_PANEL_TEXT_COLOR);
    fMessageView->SetFontAndColor(be_plain_font, B_FONT_ALL, &textColor);
    fMessageView->SetText(message);
    fMessageView->MakeEditable(ખોટું);
    fMessageView->MakeSelectable(ખોટું);
    fMessageView->SetWordWrap(સાચું);
    AddChild(fMessageView);

    દશાંશ messageHeight = fMessageView->TextHeight(0, fMessageView->ગણતરી_લીટીઓ());
    fMessageView->ResizeBy(0, messageHeight);
    fMessageView->SetTextRect(BRect(0, 0, rect.Width(), messageHeight));

    dialogHeight += messageHeight;

    //	input view
    જો (f_આગત_કિંમત != ખાલી) {
	rect.ટોચ     =
	    rect.bottom += messageHeight + kVimDialogSpacingY;
	f_આગત_Control = નવું BTextControl(rect, "_iv_", ખાલી, f_આગત_કિંમત, ખાલી,
		B_FOLLOW_ડાબું | B_FOLLOW_ટોચ, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_આગત_Control->TextView()->SetText(f_આગત_કિંમત);
	f_આગત_Control->TextView()->SetWordWrap(ખોટું);
	AddChild(f_આગત_Control);

	દશાંશ width = 0.f, height = 0.f;
	f_આગત_Control->GetPreferredSize(&width, &height);
	f_આગત_Control->MakeFocus(સાચું);

	dialogHeight += height + kVimDialogSpacingY * 1.5;
    }

    dialogHeight = max_c(dialogHeight, kVimDialogMinimalHeight);

    ResizeTo(dialogWidth, dialogHeight);
    MoveTo((gui.vimWindow->Bounds().Width() - dialogWidth) / 2,
	    (gui.vimWindow->Bounds().Height() - dialogHeight) / 2);

    //	adjust layout of buttons
    દશાંશ buttonWidth = max_c(maxButtonWidth, rect.Width() * 0.66);
    BPoint origin(dialogWidth, dialogHeight);
    origin.x -= kVimDialogSpacingX + (vertical ? buttonWidth : buttonsWidth);
    origin.y -= kVimDialogSpacingY + (vertical ? buttonsHeight	: maxButtonHeight);

    માટે (int32 i = 0 ; i < buttons_ગણતરી; i++) {
	BButton *button = (BButton*)fButtonsList.ItemAt(i);
	button->MoveTo(origin);
	જો (vertical) {
	    origin.y += button->Frame().Height() + kVimDialogButtonsSpacingY;
	    button->ResizeTo(buttonWidth, button->Frame().Height());
	} નહીંતર
	    origin.x += button->Frame().Width() + kVimDialogButtonsSpacingX;

	જો (dfltbutton == i + 1) {
	    button->MakeDefault(સાચું);
	    button->MakeFocus(f_આગત_Control == ખાલી);
	}
    }
}

VimDialog::~VimDialog()
{
    જો (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    પૂર્ણાંક
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    જો (fDialogSem < B_OK) {
	Quit();
	પાછા f_Dialog_કિંમત;
    }

    Show();

    જ્યારે (acquire_sem(fDialogSem) == B_INTERRUPTED);

    પૂર્ણાંક ret_કિંમત = f_Dialog_કિંમત;
    જો (f_આગત_કિંમત != ખાલી)
	vim_strncpy((char_u*)f_આગત_કિંમત, (char_u*)f_આગત_Control->Text(), IOSIZE - 1);

    જો (Lock())
	Quit();

    પાછા ret_કિંમત;
}

શૂન્ય VimDialog::MessageReceived(BMessage *સંદેશ)
{
    int32 which = 0;
    જો (સંદેશ->what != k_Vim_Dialog_Button_સંદેશ ||
	    સંદેશ->FindInt32("which", &which) != B_OK)
	પાછા BWindow::MessageReceived(સંદેશ);

    f_Dialog_કિંમત = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, અચલ અક્ષર* label)
{
    BMessage *message = નવું BMessage(k_Vim_Dialog_Button_સંદેશ);
    message->AddInt32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = નવું BButton(rect, name.String(), label, message,
	    B_FOLLOW_જમણું | B_FOLLOW_BOTTOM);

    દશાંશ width = 0.f, height = 0.f;
    button->GetPreferredSize(&width, &height);
    button->ResizeTo(width, height);

    પાછા button;
}

VimDialog::View::View(BRect frame)
    :	BView(frame, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(ખાલી)
{
    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    delete fIconBitmap;
}

શૂન્ય VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.જમણું = kVimDialogIconStripeWidth;
    Set_ઉપલું_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    જો (fIconBitmap == ખાલી)
	પાછા;

    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

શૂન્ય VimDialog::View::InitIcon(int32 type)
{
    જો (type == VIM_GENERIC)
	પાછા;

    BPath path;
    status_t status = find_directory(B_BEOS_SERVERS_DIRECTORY, &path);
    જો (status != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(status));
	પાછા;
    }

    path.Append("app_server");

    BFile file(path.Path(), O_RDONLY);
    જો (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	પાછા;
    }

    BResources resources(&file);
    જો (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	પાછા;
    }

    અચલ અક્ષર *name = "";
    પસંદગી(type) {
	સ્થિતિ VIM_ERROR:	    name = "stop"; તોડો;
	સ્થિતિ VIM_WARNING:   name = "warn"; તોડો;
	સ્થિતિ VIM_INFO:	    name = "info"; તોડો;
	સ્થિતિ VIM_QUESTION:  name = "idea"; તોડો;
	મૂળભૂત: પાછા;
    }

    int32 iconSize = 32;
    fIconBitmap = નવું BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1), 0, B_RGBA32);
    જો (fIconBitmap == ખાલી || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == ખાલી) ? "null" : strerror(fIconBitmap->InitCheck()));
	પાછા;
    }

    size_t size = 0;
    અચલ uint8* iconData = ખાલી;
    //	try vector icon first?
    iconData = (અચલ uint8*)resources.LoadResource(B_VECTOR_ICON_TYPE, name, &size);
    જો (iconData != ખાલી && BIconUtils::GetVectorIcon(iconData, size, fIconBitmap) == B_OK)
	પાછા;

    //	try bitmap icon now
    iconData = (અચલ uint8*)resources.LoadResource(B_LARGE_ICON_TYPE, name, &size);
    જો (iconData == ખાલી) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	delete fIconBitmap;
	fIconBitmap = ખાલી;
	પાછા;
    }

    જો (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(iconData, iconSize, iconSize, iconSize, fIconBitmap);
}

અચલ unsigned પૂર્ણાંક  k_Vim_Dialog_OKButton_સંદેશ = 'FDOK';
અચલ unsigned પૂર્ણાંક  k_Vim_Dialog_Cancel_Button_સંદેશ = 'FDCN';
અચલ unsigned પૂર્ણાંક  k_Vim_Dialog_Size_આગત_સંદેશ = 'SICH';
અચલ unsigned પૂર્ણાંક  k_Vim_Dialog_Family_Select_સંદેશ = 'MSFM';
અચલ unsigned પૂર્ણાંક  k_Vim_Dialog_Style_Select_સંદેશ = 'MSST';
અચલ unsigned પૂર્ણાંક  k_Vim_Dialog_Size_Select_સંદેશ = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, દશાંશ* size)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fStatus(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_કિંમત(ખોટું)
    , fFamily(family)
    , fStyle(style)
    , fSize(size)
    , fFontSize(*size)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_આગત(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_LENGTH);
    strncpy(fFontStyle, *style, B_FONT_STYLE_LENGTH);

    //	"client" area view
    BBox *clientBox = નવું BBox(Bounds(), B_EMPTY_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    AddChild(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.ડાબું_ટોચ(), RC.ડાબું_ટોચ());

    //	at first create all controls
    fPreview = નવું BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->AddChild(fPreview);

    BBox* boxDivider = નવું BBox(rc, B_EMPTY_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->AddChild(boxDivider);

    BStringView *labelFamily = નવું BStringView(rc, "labelFamily", "Family:");
    clientBox->AddChild(labelFamily);
    labelFamily->ResizeToPreferred();

    BStringView *labelStyle = નવું BStringView(rc, "labelStyle", "Style:");
    clientBox->AddChild(labelStyle);
    labelStyle->ResizeToPreferred();

    BStringView *labelSize = નવું BStringView(rc, "labelSize", "Size:");
    clientBox->AddChild(labelSize);
    labelSize->ResizeToPreferred();

    fFamiliesList = નવું BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = નવું BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_ડાબું_જમણું, 0, ખોટું, સાચું);
    clientBox->AddChild(scrollFamilies);

    fStylesList= નવું BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = નવું BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_ડાબું_જમણું, 0, ખોટું, સાચું);
    clientBox->AddChild(scrollStyles);

    f_Sizes_આગત = નવું BTextControl(rc, "inputSize", ખાલી, "???",
	    નવું BMessage(k_Vim_Dialog_Size_આગત_સંદેશ));
    clientBox->AddChild(f_Sizes_આગત);
    f_Sizes_આગત->ResizeToPreferred();

    fSizesList = નવું BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = નવું BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_ડાબું_જમણું, 0, ખોટું, સાચું);
    clientBox->AddChild(scrollSizes);

    BButton *buttonOK = નવું BButton(rc, "buttonOK", "OK",
			નવું BMessage(k_Vim_Dialog_OKButton_સંદેશ));
    clientBox->AddChild(buttonOK);
    buttonOK->ResizeToPreferred();

    BButton *buttonCancel = નવું BButton(rc, "buttonCancel", "Cancel",
			નવું BMessage(k_Vim_Dialog_Cancel_Button_સંદેશ));
    clientBox->AddChild(buttonCancel);
    buttonCancel->ResizeToPreferred();

    //	layout controls
    દશાંશ લીટી_Height = labelFamily->Bounds().Height();
    દશાંશ previewHeight = લીટી_Height * 3;
    દશાંશ offsetYLabels = previewHeight + kVimDialogSpacingY;
    દશાંશ offsetYLists = offsetYLabels + લીટી_Height + kVimDialogSpacingY / 2;
    દશાંશ offsetYSizes = offsetYLists + f_Sizes_આગત->Bounds().Height() + kVimDialogSpacingY / 2;
    દશાંશ listsHeight = લીટી_Height * 9;
    દશાંશ offsetYButtons = offsetYLists + listsHeight +  kVimDialogSpacingY;
    દશાંશ maxControlsHeight = offsetYButtons + buttonOK->Bounds().Height();
    દશાંશ familiesWidth = labelFamily->Bounds().Width() * 5;
    દશાંશ offsetXStyles = familiesWidth + kVimDialogSpacingX;
    દશાંશ stylesWidth = labelStyle->Bounds().Width() * 4;
    દશાંશ offsetXSizes = offsetXStyles + stylesWidth + kVimDialogSpacingX;
    દશાંશ sizesWidth = labelSize->Bounds().Width() * 2;
    દશાંશ maxControlsWidth = offsetXSizes + sizesWidth;

    ResizeTo(maxControlsWidth + kVimDialogSpacingX * 2,
	maxControlsHeight + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vimWindow->Frame();
    MoveTo(rcVim.ડાબું + (rcVim.Width() - Frame().Width()) / 2,
	    rcVim.ટોચ + (rcVim.Height() - Frame().Height()) / 2);

    fPreview->ResizeTo(maxControlsWidth, previewHeight);
    fPreview->SetAlignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, previewHeight + kVimDialogSpacingY / 2);
    boxDivider->ResizeTo(maxControlsWidth, 1.f);

    labelFamily->MoveBy(0.f, offsetYLabels);
    labelStyle->MoveBy(offsetXStyles, offsetYLabels);
    labelSize->MoveBy(offsetXSizes, offsetYLabels);

    //	text control alignment issues
    દશાંશ insetX = f_Sizes_આગત->TextView()->Bounds().Width() - f_Sizes_આગત->Bounds().Width();
    દશાંશ insetY = f_Sizes_આગત->TextView()->Bounds().Width() - f_Sizes_આગત->Bounds().Width();

    scrollFamilies->MoveBy(0.f, offsetYLists);
    scrollStyles->MoveBy(offsetXStyles, offsetYLists);
    f_Sizes_આગત->MoveBy(offsetXSizes + insetX / 2, offsetYLists + insetY / 2);
    scrollSizes->MoveBy(offsetXSizes, offsetYSizes);

    f_Sizes_આગત->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->ResizeTo(familiesWidth, listsHeight);
    scrollStyles->ResizeTo(stylesWidth, listsHeight);
    f_Sizes_આગત->ResizeTo(sizesWidth, f_Sizes_આગત->Bounds().Height());
    scrollSizes->ResizeTo(sizesWidth,
	    listsHeight - (offsetYSizes - offsetYLists));

    buttonOK->MoveBy(maxControlsWidth - buttonOK->Bounds().Width(), offsetYButtons);
    buttonCancel->MoveBy(maxControlsWidth - buttonOK->Bounds().Width()
	    - buttonCancel->Bounds().Width() - kVimDialogSpacingX, offsetYButtons);

    //	fill lists
    પૂર્ણાંક selIndex = -1;
    પૂર્ણાંક ગણતરી = ગણતરી_font_families();
    માટે (પૂર્ણાંક i = 0; i < ગણતરી; i++) {
	font_family family;
	જો (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->AddItem(નવું BStringItem((અચલ અક્ષર*)family));
	    જો (strncmp(family, fFontFamily, B_FONT_FAMILY_LENGTH) == 0)
		selIndex = i;
	}
    }

    જો (selIndex >= 0) {
	fFamiliesList->Select(selIndex);
	fFamiliesList->ScrollToSelection();
    }

    _UpdateFontStyles();

    selIndex = -1;
    માટે (પૂર્ણાંક size = 8, index = 0; size <= 18; size++, index++) {
	BString str;
	str << size;
	fSizesList->AddItem(નવું BStringItem(str));
	જો (size == fFontSize)
	    selIndex = index;

    }

    જો (selIndex >= 0) {
	fSizesList->Select(selIndex);
	fSizesList->ScrollToSelection();
    }

    fFamiliesList->SetSelectionMessage(નવું BMessage(k_Vim_Dialog_Family_Select_સંદેશ));
    fStylesList->SetSelectionMessage(નવું BMessage(k_Vim_Dialog_Style_Select_સંદેશ));
    fSizesList->SetSelectionMessage(નવું BMessage(k_Vim_Dialog_Size_Select_સંદેશ));
    f_Sizes_આગત->SetModificationMessage(નવું BMessage(k_Vim_Dialog_Size_આગત_સંદેશ));

    Update_Size_આગત_Preview();
    _UpdateFontPreview();

    fStatus = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    જો (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    શૂન્ય
VimSelectFontDialog::_CleanList(BListView* list)
{
    જ્યારે (0 < list->ગણતરી_વસ્તુઓ())
	delete (dynamic_cast<BStringItem*>(list->RemoveItem((int32)0)));
}

    તાર્કિક
VimSelectFontDialog::Go()
{
    જો (fStatus != B_OK) {
	Quit();
	પાછા NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    જો (fDialogSem < B_OK) {
	Quit();
	પાછા f_Dialog_કિંમત;
    }

    Show();

    જ્યારે (acquire_sem(fDialogSem) == B_INTERRUPTED);

    તાર્કિક ret_કિંમત = f_Dialog_કિંમત;

    જો (Lock())
	Quit();

    પાછા ret_કિંમત;
}


શૂન્ય VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 selIndex = -1;
    int32 ગણતરી = ગણતરી_font_styles(fFontFamily);
    માટે (int32 i = 0; i < ગણતરી; i++) {
	font_style style;
	uint32 flags = 0;
	જો (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->AddItem(નવું BStringItem((અચલ અક્ષર*)style));
	    જો (strncmp(style, fFontStyle, B_FONT_STYLE_LENGTH) == 0)
		selIndex = i;
	}
    }

    જો (selIndex >= 0) {
	fStylesList->Select(selIndex);
	fStylesList->ScrollToSelection();
    } નહીંતર
	fStylesList->Select(0);
}


શૂન્ય VimSelectFontDialog::Update_Size_આગત_Preview()
{
    અક્ષર buf[10] = {0};
    vim_snprintf(buf, કદ(buf), (અક્ષર*)"%.0f", fFontSize);
    f_Sizes_આગત->SetText(buf);
}


શૂન્ય VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.SetSize(fFontSize);
    font.SetFamilyAndStyle(fFontFamily, fFontStyle);
    fPreview->SetFont(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_SIZE);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (પૂર્ણાંક)fFontSize << " pt.";
    fPreview->SetText(str);
}


    તાર્કિક
VimSelectFontDialog::_UpdateFromListItem(BListView* list, અક્ષર* text, પૂર્ણાંક textSize)
{
    int32 index = list->CurrentSelection();
    જો (index < 0)
	પાછા ખોટું;
    BStringItem* item = (BStringItem*)list->ItemAt(index);
    જો (item == ખાલી)
	પાછા ખોટું;
    strncpy(text, item->Text(), textSize);
    પાછા સાચું;
}


શૂન્ય VimSelectFontDialog::MessageReceived(BMessage *સંદેશ)
{
    પસંદગી (સંદેશ->what) {
	સ્થિતિ k_Vim_Dialog_OKButton_સંદેશ:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_LENGTH);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_LENGTH);
	    *fSize = fFontSize;
	    f_Dialog_કિંમત = સાચું;
	સ્થિતિ k_Vim_Dialog_Cancel_Button_સંદેશ:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    પાછા;
	સ્થિતિ B_ચાવી_UP:
	    {
		int32 ચાવી = 0;
		જો (સંદેશ->FindInt32("raw_char", &ચાવી) == B_OK
			&& ચાવી == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    તોડો;

	સ્થિતિ k_Vim_Dialog_Family_Select_સંદેશ:
	    જો (_UpdateFromListItem(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_LENGTH)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    તોડો;
	સ્થિતિ k_Vim_Dialog_Style_Select_સંદેશ:
	    જો (_UpdateFromListItem(fStylesList,
		    fFontStyle, B_FONT_STYLE_LENGTH))
		_UpdateFontPreview();
	    તોડો;
	સ્થિતિ k_Vim_Dialog_Size_Select_સંદેશ:
	    {
		અક્ષર buf[10] = {0};
		જો (_UpdateFromListItem(fSizesList, buf, કદ(buf))) {
		    દશાંશ size = atof(buf);
		    જો (size > 0.f) {
			fFontSize = size;
			Update_Size_આગત_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    તોડો;
	સ્થિતિ k_Vim_Dialog_Size_આગત_સંદેશ:
	    {
		દશાંશ size = atof(f_Sizes_આગત->Text());
		જો (size > 0.f) {
		    fFontSize = size;
		    _UpdateFontPreview();
		}
	    }
	    તોડો;
	મૂળભૂત:
	    તોડો;
    }
    પાછા BWindow::MessageReceived(સંદેશ);
}

#endif // FEAT_GUI_DIALOG

#ifdef FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
સ્થિર BMessage * MenuMessage(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect frame, અચલ અક્ષર *name) :
    BBox(frame, name, B_FOLLOW_ડાબું | B_FOLLOW_ટોચ, B_WILL_DRAW | B_FRAME_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 ગણતરી = fButtonsList.ગણતરી_વસ્તુઓ();
    માટે (int32 i = 0; i < ગણતરી; i++)
	delete (BPictureButton*)fButtonsList.ItemAt(i);
    fButtonsList.MakeEmpty();

    delete normalButtonsBitmap;
    delete grayedButtonsBitmap;
    normalButtonsBitmap    = ખાલી;
    grayedButtonsBitmap  = ખાલી;
}

    શૂન્ય
VimToolbar::AttachedToWindow()
{
    BBox::AttachedToWindow();

    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    દશાંશ
VimToolbar::ToolbarHeight() અચલ
{
    દશાંશ size = ખાલી == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
    પાછા size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    તાર્કિક
VimToolbar::ModifyBitmapToGrayed(BBitmap *bitmap)
{
    દશાંશ height = bitmap->Bounds().Height();
    દશાંશ width  = bitmap->Bounds().Width();

    rgb_color *bits = (rgb_color*)bitmap->Bits();
    int32 pixels = bitmap->BitsLength() / 4;
    માટે (int32 i = 0; i < pixels; i++) {
	bits[i].red = bits[i].green =
	bits[i].blue = ((uint32)bits[i].red + bits[i].green + bits[i].blue) / 3;
	bits[i].alpha /= 4;
    }

    પાછા સાચું;
}

    તાર્કિક
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    normalButtonsBitmap = LoadVimBitmap("builtin-tools.png");
    જો (normalButtonsBitmap == ખાલી)
	//  customized not found? dig application resources for "builtin-tools" one
	normalButtonsBitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    જો (normalButtonsBitmap == ખાલી)
	પાછા ખોટું;

    BMessage archive;
    normalButtonsBitmap->Archive(&archive);

    grayedButtonsBitmap = નવું BBitmap(&archive);
    જો (grayedButtonsBitmap == ખાલી)
	પાછા ખોટું;

    //	modify grayed bitmap
    ModifyBitmapToGrayed(grayedButtonsBitmap);

    પાછા સાચું;
}

BBitmap *VimToolbar::LoadVimBitmap(અચલ અક્ષર* fileName)
{
    BBitmap *bitmap = ખાલી;

    પૂર્ણાંક mustfree = 0;
    char_u* runtimePath = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    જો (runtimePath != ખાલી && fileName != ખાલી) {
	BString strPath((અક્ષર*)runtimePath);
	strPath << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(strPath.String());
    }

    જો (mustfree)
	vim_free(runtimePath);

    પાછા bitmap;
}

    તાર્કિક
VimToolbar::GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, તાર્કિક pressed)
{
    દશાંશ size = bitmapFrom->Bounds().Height() + 1.;

    BView view(BRect(0, 0, size, size), "", 0, 0);

    AddChild(&view);
    view.BeginPicture(pictureTo);

    view.Set_ઉપલું_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.SetDrawingMode(B_OP_OVER);

    BRect source(0, 0, size - 1, size - 1);
    BRect destination(source);

    source.OffsetBy(size * index, 0);
    destination.OffsetBy(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, source, destination);

    જો (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	size += ButtonMargin * 2 - 1;
	view.Begin_લીટી_Array(4);
	view.Add_લીટી(BPoint(0, 0),	 BPoint(size, 0),    shadowColor);
	view.Add_લીટી(BPoint(size, 0),	 BPoint(size, size), shineColor);
	view.Add_લીટી(BPoint(size, size), BPoint(0, size),    shineColor);
	view.Add_લીટી(BPoint(0, size),	 BPoint(0, 0),	     shadowColor);
	view.End_લીટી_Array();
    }

    view.EndPicture();
    RemoveChild(&view);

    પાછા સાચું;
}

    તાર્કિક
VimToolbar::AddButton(int32 index, vimmenu_T *menu)
{
    BPictureButton *button = ખાલી;
    જો (!menu_is_separator(menu->name)) {
	દશાંશ size = normalButtonsBitmap ?
	    normalButtonsBitmap->Bounds().Height() + 1. + ButtonMargin * 2 : 18.;
	BRect frame(0, 0, size, size);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture pictureGray;

	જો (menu->iconfile == ખાલી && menu->iconidx >= 0 && normalButtonsBitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, normalButtonsBitmap, સાચું);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, normalButtonsBitmap, ખોટું);
	    GetPictureFromBitmap(&pictureGray, menu->iconidx, grayedButtonsBitmap, ખોટું);
	} નહીંતર {

	    char_u buffer[MAXPATHL] = {0};
	    BBitmap *bitmap = ખાલી;

	    જો (menu->iconfile) {
		gui_find_iconfile(menu->iconfile, buffer, (અક્ષર*)"png");
		bitmap = BTranslationUtils::GetBitmap((અક્ષર*)buffer);
	    }

	    જો (bitmap == ખાલી && gui_find_bitmap(menu->name, buffer, (અક્ષર*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((અક્ષર*)buffer);

	    જો (bitmap == ખાલી)
		bitmap = નવું BBitmap(BRect(0, 0, size, size), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, સાચું);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, ખોટું);
	    ModifyBitmapToGrayed(bitmap);
	    GetPictureFromBitmap(&pictureGray, 0, bitmap, ખોટું);

	    delete bitmap;
	}

	button = નવું BPictureButton(frame, (અક્ષર*)menu->name,
		    &pictureOff, &pictureOn, MenuMessage(menu));

	button->SetDisabledOn(&pictureGray);
	button->SetDisabledOff(&pictureGray);

	button->SetTarget(gui.vimTextArea);

	AddChild(button);

	menu->button = button;
    }

    તાર્કિક result = fButtonsList.AddItem(button, index);
    InvalidateLayout();
    પાછા result;
}

    તાર્કિક
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    જો (menu->button) {
	જો (fButtonsList.RemoveItem(menu->button)) {
	    delete menu->button;
	    menu->button = ખાલી;
	}
    }
    પાછા સાચું;
}

    તાર્કિક
VimToolbar::GrayButton(vimmenu_T *menu, પૂર્ણાંક grey)
{
    જો (menu->button) {
	int32 index = fButtonsList.IndexOf(menu->button);
	જો (index >= 0)
	    menu->button->SetEnabled(grey ? ખોટું : સાચું);
    }
    પાછા સાચું;
}

    શૂન્ય
VimToolbar::InvalidateLayout()
{
    int32 offset = ToolbarMargin;
    int32 ગણતરી = fButtonsList.ગણતરી_વસ્તુઓ();
    માટે (int32 i = 0; i < ગણતરી; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.ItemAt(i);
	જો (button) {
	    button->MoveTo(offset, ToolbarMargin);
	    offset += button->Bounds().Width() + ToolbarMargin;
	} નહીંતર
	    offset += ToolbarMargin * 3;
    }
}

#endif /*FEAT_TOOLBAR*/

#જો defined(FEAT_GUI_TABLINE)

    દશાંશ
Vim_Tab_લીટી::TablineHeight() અચલ
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    પાછા TabHeight(); //  + ToolbarMargin;
}

શૂન્ય
Vim_Tab_લીટી::MouseDown(BPoint point)
{
    જો (!gui_mch_showing_tabline())
	પાછા;

    BMessage *m = Window()->CurrentMessage();
    પ્રતિજ્ઞા(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    int32 clicks = 0;
    m->FindInt32("clicks", &clicks);

    પૂર્ણાંક index = 0; //  0 means here - no tab found
    માટે (પૂર્ણાંક i = 0; i < ગણતરી_Tabs(); i++) {
	જો (TabFrame(i).Contains(point)) {
	    index = i + 1; //  indexes are 1-based
	    તોડો;
	}
    }

    પૂર્ણાંક event = -1;

    જો ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    નહીંતર જો (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = index > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    નહીંતર જો (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPopUpMenu* કાઢો_Up_Menu = નવું BPopUpMenu("tabLineContextMenu", ખોટું, ખોટું);
	કાઢો_Up_Menu->AddItem(નવું BMenuItem(_("Close tabi R"), નવું BMessage(TABLINE_MENU_CLOSE)));
	કાઢો_Up_Menu->AddItem(નવું BMenuItem(_("New tab    T"), નવું BMessage(TABLINE_MENU_NEW)));
	કાઢો_Up_Menu->AddItem(નવું BMenuItem(_("Open tab..."), નવું BMessage(TABLINE_MENU_OPEN)));

	ConvertToScreen(&point);
	BMenuItem* item = કાઢો_Up_Menu->Go(point);
	જો (item != ખાલી) {
	    event = item->Command();
	}

	delete કાઢો_Up_Menu;

    } નહીંતર {
	//  default processing
	BTabView::MouseDown(point);
	પાછા;
    }

    જો (event < 0)
	પાછા;

    Vim_Tabline_Menu_સંદેશ tmm;
    tmm.index = index;
    tmm.event = event;
    write_port(gui.vdcmp, Vim_સંદેશ::TablineMenu, &tmm, કદ(tmm));
}

શૂન્ય
Vim_Tab_લીટી::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_લીટી *tab_લીટી = gui.vimForm->Tab_લીટી();
    જો (tab_લીટી != ખાલી) {

	int32 i = 0;
	માટે (; i < tab_લીટી->ગણતરી_Tabs(); i++)
	    જો (આ == tab_લીટી->TabAt(i))
		તોડો;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	જો (i < tab_લીટી->ગણતરી_Tabs()) {
	    Vim_Tabline_સંદેશ tm;
	    tm.index = i + 1;
	    write_port(gui.vdcmp, Vim_સંદેશ::Tabline, &tm, કદ(tm));
	}
    }
}

#endif //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
સ્થિર અક્ષર appsig[] = "application/x-vnd.Haiku-Vim-8";
ચાવી_map *ચાવી_Map;
અક્ષર *ચાવી_Map_Chars;
પૂર્ણાંક main_exitcode = 127;

    status_t
gui_haiku_process_event(bigtime_t timeout)
{
    સંરચના Vim_સંદેશ vm;
    int32 what;
    ssize_t size;

    size = read_port_etc(gui.vdcmp, &what, &vm, કદ(vm),
	    B_TIMEOUT, timeout);

    જો (size >= 0) {
	પસંદગી (what) {
	    સ્થિતિ Vim_સંદેશ::ચાવી_2:
		{
		    char_u *string = vm.u.ચાવી_2.chars;
		    પૂર્ણાંક len = vm.u.ચાવી_2.length;
		    જો (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_આગત_buf();
			got_int = TRUE;
		    }

		    જો (vm.u.ચાવી_2.csi_escape)
#ifndef FEAT_MBYTE_IME
		    {
			પૂર્ણાંક	i;
			char_u	buf[2];

			માટે (i = 0; i < len; ++i)
			{
			    add_to_આગત_buf(string + i, 1);
			    જો (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (પૂર્ણાંક)KE_CSI;
				add_to_આગત_buf(buf, 2);
			    }
			}
		    }
#નહીંતર
			add_to_આગત_buf_csi(string, len);
#endif
		    નહીંતર
			add_to_આગત_buf(string, len);
		}
		તોડો;
	    સ્થિતિ Vim_સંદેશ::Resize:
		gui_resize_shell(vm.u.NewSize.width, vm.u.NewSize.height);
		તોડો;
	    સ્થિતિ Vim_સંદેશ::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_ગણતરી =
			atomic_add(&vm.u.Scroll.sb->scroll_Event_ગણતરી, -1);
		    જો (old_ગણતરી <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.કિંમત, vm.u.Scroll.stillDragging);
		}
		તોડો;
#જો defined(FEAT_MENU)
	    સ્થિતિ Vim_સંદેશ::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		તોડો;
#endif
	    સ્થિતિ Vim_સંદેશ::Mouse:
		{
		    int32 old_ગણતરી;
		    જો (vm.u.Mouse.button == MOUSE_DRAG)
			old_ગણતરી =
			    atomic_add(&gui.vimTextArea->mouse_Drag_Event_ગણતરી, -1);
		    નહીંતર
			old_ગણતરી = 0;
		    જો (old_ગણતરી <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		તોડો;
	    સ્થિતિ Vim_સંદેશ::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		તોડો;
	    સ્થિતિ Vim_સંદેશ::Focus:
		gui.in_focus = vm.u.Focus.active;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		જો (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		તોડો;
	    સ્થિતિ Vim_સંદેશ::Refs:
		::RefsReceived(vm.u.Refs.message, vm.u.Refs.changedir);
		તોડો;
	    સ્થિતિ Vim_સંદેશ::Tabline:
		send_tabline_event(vm.u.Tabline.index);
		તોડો;
	    સ્થિતિ Vim_સંદેશ::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.index, vm.u.TablineMenu.event);
		તોડો;
	    મૂળભૂત:
		//  unrecognised message, ignore it
		તોડો;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    પાછા size;
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

    પૂર્ણાંક
vim_lock_screen()
{
    પાછા !gui.vimWindow || gui.vimWindow->Lock();
}

    શૂન્ય
vim_unlock_screen()
{
    જો (gui.vimWindow)
	gui.vimWindow->Unlock();
}

#define RUN_BAPPLICATION_IN_NEW_THREAD	0

#જો RUN_BAPPLICATION_IN_NEW_THREAD

    int32
run_vimapp(શૂન્ય *args)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.Run();		    // Run until Quit() called

    પાછા 0;
}

#નહીંતર

    int32
call_main(શૂન્ય *args)
{
    સંરચના MainArgs *ma = (MainArgs *)args;

    પાછા મુખ્ય(ma->argc, ma->argv);
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    શૂન્ય
gui_mch_prepare(
	પૂર્ણાંક	*argc,
	અક્ષર	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    જો (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(find_thread(ખાલી), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_port(B_MAX_PORT_ગણતરી, "vim VDCMP");

#જો RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = spawn_thread(run_vimapp, "vim VimApp",
		tinfo.priority, ખાલી);
	જો (tid >= B_OK) {
	    resume_thread(tid);
	} નહીંતર {
	    getout(1);
	}
#નહીંતર
	MainArgs ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.priority, &ma);
	જો (tid >= B_OK) {
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
	    (શૂન્ય)wait_for_thread(tid, &dummy_exitcode);

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
    જો (!isatty(0)) {
	સંરચના stat stat_stdin, stat_dev_null;

	જો (fstat(0, &stat_stdin) == -1 ||
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
    પૂર્ણાંક
gui_mch_init_check(શૂન્ય)
{
    પાછા OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    પૂર્ણાંક
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_back_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    gui.scrollbar_width = (પૂર્ણાંક) B_V_SCROLL_BAR_WIDTH;
    gui.scrollbar_height = (પૂર્ણાંક) B_H_SCROLL_BAR_HEIGHT;
#ifdef FEAT_MENU
    gui.menu_height = 19;   //	initial guess -
    //	correct for my default settings
#endif
    gui.border_offset = 3;  //	coordinates are inside window borders

    જો (gui.vdcmp < B_OK)
	પાછા FAIL;
    get_ચાવી_map(&ચાવી_Map, &ચાવી_Map_Chars);

    gui.vimWindow = નવું VimWindow();	// hidden and locked
    જો (!gui.vimWindow)
	પાછા FAIL;

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

    પાછા OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    શૂન્ય
gui_mch_new_colors()
{
    rgb_color rgb = GUI_TO_RGB(gui.back_pixel);

    જો (gui.vimWindow->Lock()) {
	gui.vimForm->SetViewColor(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vimWindow->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    પૂર્ણાંક
gui_mch_open()
{
    જો (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    જો (gui.vimWindow->Lock()) {
	gui.vimWindow->Show();
	gui.vimWindow->Unlock();
	પાછા OK;
    }

    પાછા FAIL;
}

    શૂન્ય
gui_mch_exit(પૂર્ણાંક vim_exitcode)
{
    જો (gui.vimWindow) {
	thread_id tid = gui.vimWindow->Thread();
	gui.vimWindow->Lock();
	gui.vimWindow->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_port(gui.vdcmp);
#જો !RUN_BAPPLICATION_IN_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#endif
    જો (gui.vimApp) {
	VimTextAreaView::guiBlankMouse(ખોટું);

	main_exitcode = vim_exitcode;
#જો RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->Lock();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#નહીંતર
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
    પૂર્ણાંક
gui_mch_get_winpos(પૂર્ણાંક *x, પૂર્ણાંક *y)
{
    જો (gui.vimWindow->Lock()) {
	BRect r;
	r = gui.vimWindow->Frame();
	gui.vimWindow->Unlock();
	*x = (પૂર્ણાંક)r.ડાબું;
	*y = (પૂર્ણાંક)r.ટોચ;
	પાછા OK;
    }
    નહીંતર
	પાછા FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    શૂન્ય
gui_mch_set_winpos(પૂર્ણાંક x, પૂર્ણાંક y)
{
    જો (gui.vimWindow->Lock()) {
	gui.vimWindow->MoveTo(x, y);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
શૂન્ય
gui_mch_set_shellsize(
	પૂર્ણાંક	width,
	પૂર્ણાંક	height,
	પૂર્ણાંક	min_width,
	પૂર્ણાંક	min_height,
	પૂર્ણાંક	base_width,
	પૂર્ણાંક	base_height,
	પૂર્ણાંક	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    જો (gui.vimWindow->Lock()) {
	gui.vimWindow->ResizeTo(width - PEN_WIDTH, height - PEN_WIDTH);

	// set size limits
	દશાંશ minWidth, maxWidth, minHeight, maxHeight;

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

શૂન્ય
gui_mch_get_screen_dimensions(
	પૂર્ણાંક	*screen_w,
	પૂર્ણાંક	*screen_h)
{
    BRect frame;

    {
	BScreen screen(gui.vimWindow);

	જો (screen.IsValid()) {
	    frame = screen.Frame();
	} નહીંતર {
	    frame.જમણું = 640;
	    frame.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (પૂર્ણાંક) frame.જમણું - 2 * gui.scrollbar_width - 20;
    *screen_h = (પૂર્ણાંક) frame.bottom - gui.scrollbar_height
#ifdef FEAT_MENU
	- gui.menu_height
#endif
	- 30;
}

શૂન્ય
gui_mch_set_text_area_pos(
	પૂર્ણાંક	x,
	પૂર્ણાંક	y,
	પૂર્ણાંક	w,
	પૂર્ણાંક	h)
{
    જો (!gui.vimTextArea)
	પાછા;

    જો (gui.vimWindow->Lock()) {
	gui.vimTextArea->MoveTo(x, y);
	gui.vimTextArea->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);

#ifdef FEAT_GUI_TABLINE
	જો (gui.vimForm->Tab_લીટી() != ખાલી) {
	    gui.vimForm->Tab_લીટી()->ResizeTo(w, gui.vimForm->TablineHeight());
	}
#endif // FEAT_GUI_TABLINE

	gui.vimWindow->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

શૂન્ય
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	પૂર્ણાંક	flag)
{
    VimScrollBar *vsb = sb->id;
    જો (gui.vimWindow->Lock()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	જો (flag) {
	    જો (vsb->IsHidden()) {
		vsb->Show();
	    }
	} નહીંતર {
	    જો (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vimWindow->Unlock();
    }
}

શૂન્ય
gui_mch_set_scrollbar_thumb(
	scrollbar_T *sb,
	પૂર્ણાંક	val,
	પૂર્ણાંક	size,
	પૂર્ણાંક	max)
{
    જો (gui.vimWindow->Lock()) {
	VimScrollBar *s = sb->id;
	જો (max == 0) {
	    s->Set_કિંમત(0);
	    s->SetRange(0.0, 0.0);
	} નહીંતર {
	    s->SetProportion((દશાંશ)size / (max + 1.0));
	    s->SetSteps(1.0, size > 5 ? size - 2 : size);
#ifndef SCROLL_PAST_END	    //	really only defined in gui.c...
	    max = max + 1 - size;
#endif
	    જો (max < s->કિંમત_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->Set_કિંમત(val);
		s->SetRange(0.0, max);
	    } નહીંતર {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->SetRange(0.0, max);
		s->Set_કિંમત(val);
	    }
	}
	gui.vimWindow->Unlock();
    }
}

શૂન્ય
gui_mch_set_scrollbar_pos(
	scrollbar_T *sb,
	પૂર્ણાંક	x,
	પૂર્ણાંક	y,
	પૂર્ણાંક	w,
	પૂર્ણાંક	h)
{
    જો (gui.vimWindow->Lock()) {
	BRect winb = gui.vimWindow->Bounds();
	દશાંશ vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);
	જો (winb.જમણું-(x+w)<w) vsbx = winb.જમણું - (w - PEN_WIDTH);
	vsb->MoveTo(vsbx, vsby);
	gui.vimWindow->Unlock();
    }
}

    પૂર્ણાંક
gui_mch_get_scrollbar_xpadding(શૂન્ય)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    પાછા 0;
}

    પૂર્ણાંક
gui_mch_get_scrollbar_ypadding(શૂન્ય)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    પાછા 0;
}

શૂન્ય
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	પૂર્ણાંક	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = નવું VimScrollBar(sb, posture);
    જો (gui.vimWindow->Lock()) {
	vsb->SetTarget(gui.vimTextArea);
	vsb->Hide();
	gui.vimForm->AddChild(vsb);
	gui.vimWindow->Unlock();
    }
}

#જો defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
શૂન્ય
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    જો (gui.vimWindow->Lock()) {
	sb->id->RemoveSelf();
	delete sb->id;
	gui.vimWindow->Unlock();
    }
}
#endif

/*
 * Cursor does not flash
 */
    પૂર્ણાંક
gui_mch_is_blink_off(શૂન્ય)
{
    પાછા FALSE;
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

સ્થિર પૂર્ણાંક	blink_state = BLINK_NONE;
સ્થિર long_u	    blink_waittime = 700;
સ્થિર long_u	    blink_ontime = 400;
સ્થિર long_u	    blink_offtime = 250;
સ્થિર પૂર્ણાંક  blink_timer = 0;

શૂન્ય
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
    શૂન્ય
gui_mch_stop_blink(પૂર્ણાંક may_call_gui_update_cursor)
{
    // TODO
    જો (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    જો (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    શૂન્ય
gui_mch_start_blink()
{
    // TODO
    જો (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    જો (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
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
પૂર્ણાંક
gui_mch_init_font(
	char_u	    *font_name,
	પૂર્ણાંક	    fontset)
{
    જો (gui.vimWindow->Lock())
    {
	પૂર્ણાંક rc = gui.vimTextArea->mchInitFont(font_name);
	gui.vimWindow->Unlock();

	પાછા rc;
    }

    પાછા FAIL;
}


    પૂર્ણાંક
gui_mch_adjust_charsize()
{
    પાછા FAIL;
}


    પૂર્ણાંક
gui_mch_font_dialog(font_family* family, font_style* style, દશાંશ* size)
{
#જો defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = નવું VimSelectFontDialog(family, style, size);
    તાર્કિક ret = dialog->Go();
	delete dialog;
	પાછા ret;
#નહીંતર
    પાછા NOFONT;
#endif // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	પૂર્ણાંક	    giveErrorIfMissing)
{
    સ્થિર VimFont *fontList = ખાલી;

    જો (!gui.in_use)	//  can't do this when GUI not running
	પાછા NOFONT;

    //	storage for locally modified name;
    અચલ પૂર્ણાંક buff_size = B_FONT_FAMILY_LENGTH + B_FONT_STYLE_LENGTH + 20;
    સ્થિર અક્ષર font_name[buff_size] = {0};
    font_family family = {0};
    font_style	style  = {0};
    દશાંશ size = 0.f;

    જો (name == 0 && be_fixed_font == 0) {
	જો (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	પાછા NOFONT;
    }

    તાર્કિક useSelectGUI = ખોટું;
    જો (name != ખાલી)
	જો (STRCMP(name, "*") == 0) {
	    useSelectGUI = સાચું;
	    STRNCPY(font_name, hl_get_font_name(), buff_size);
	} નહીંતર
	    STRNCPY(font_name, name, buff_size);

    જો (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	size = be_fixed_font->Size();
	vim_snprintf(font_name, buff_size,
	    (અક્ષર*)"%s/%s/%.0f", family, style, size);
    }

    //	replace underscores with spaces
    અક્ષર* end = 0;
    જ્યારે (end = strchr((અક્ષર *)font_name, '_'))
	*end = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    સ્થિર અક્ષર buff[buff_size] = {0};
    STRNCPY(buff, font_name, buff_size);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_LENGTH);
    અક્ષર* style_s = strtok(0, "/\0");
    જો (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_LENGTH);
    size = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    જો (useSelectGUI) {
	જો (gui_mch_font_dialog(&family, &style, &size) == NOFONT)
	    પાછા FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_size,
		(અક્ષર*)"%s/%s/%.0f", family, style, size);
	hl_set_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	જો (new_p_guifont != ખાલી) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    માટે ( ; *new_p_guifont; ++new_p_guifont)
		જો (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    માટે (flp = fontList; flp; flp = flp->next) {
	જો (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    પાછા (GuiFont)flp;
	}
    }

    VimFont *font = નવું VimFont();
    font->name = vim_strsave((char_u*)font_name);

    જો (ગણતરી_font_styles(family) <= 0) {
	જો (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	delete font;
	પાછા NOFONT;
    }

    //	Remember font in the static list for later use
    font->next = fontList;
    fontList = font;

    font->SetFamilyAndStyle(family, style);
    જો (size > 0.f)
	font->SetSize(size);

    font->SetSpacing(B_FIXED_SPACING);
    font->SetEncoding(B_UNICODE_UTF8);

    પાછા (GuiFont)font;
}

/*
 * Set the current text font.
 */
શૂન્ય
gui_mch_set_font(
	GuiFont	font)
{
    જો (gui.vimWindow->Lock()) {
	VimFont *vf = (VimFont *)font;

	gui.vimTextArea->SetFont(vf);

	gui.char_width = (પૂર્ણાંક) vf->StringWidth("n");
	font_height fh;
	vf->GetHeight(&fh);
	gui.char_height = (પૂર્ણાંક)(fh.ascent + 0.9999)
	    + (પૂર્ણાંક)(fh.descent + 0.9999) + (પૂર્ણાંક)(fh.leading + 0.9999);
	gui.char_ascent = (પૂર્ણાંક)(fh.ascent + 0.9999);

	gui.vimWindow->Unlock();
    }
}

// XXX TODO This is apparently never called...
શૂન્ય
gui_mch_free_font(
	GuiFont	font)
{
    જો (font == NOFONT)
	પાછા;
    VimFont *f = (VimFont *)font;
    જો (--f->refcount <= 0) {
	જો (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	delete f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    જો (name == ખાલી)
	પાછા ખાલી;
    પાછા vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    પૂર્ણાંક
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
	gui.char_height = (પૂર્ણાંક)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (પૂર્ણાંક)(fh.ascent + 0.5) + p_linespace / 2;
    }
    પાછા OK;
}

    શૂન્ય
gui_mch_getmouse(પૂર્ણાંક *x, પૂર્ણાંક *y)
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

    શૂન્ય
gui_mch_mousehide(પૂર્ણાંક hide)
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
    પાછા gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
શૂન્ય
gui_mch_set_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    જો (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_ઉપલું_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text background color.
 */
શૂન્ય
gui_mch_set_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    જો (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_નીચલું_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    શૂન્ય
gui_mch_set_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

શૂન્ય
gui_mch_draw_string(
	પૂર્ણાંક	row,
	પૂર્ણાંક	col,
	char_u	*s,
	પૂર્ણાંક	len,
	પૂર્ણાંક	flags)
{
    જો (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchDrawString(row, col, s, len, flags);
	gui.vimWindow->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(પૂર્ણાંક r, પૂર્ણાંક g, પૂર્ણાંક b)
{
    પાછા gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
પૂર્ણાંક
gui_mch_haskey(
	char_u	*name)
{
    પૂર્ણાંક i;

    માટે (i = 0; special_ચાવીઓ[i].Be_ચાવીઓ != 0; i++)
	જો (name[0] == special_ચાવીઓ[i].vim_code0 &&
		name[1] == special_ચાવીઓ[i].vim_code1)
	    પાછા OK;
    પાછા FAIL;
}

    શૂન્ય
gui_mch_beep()
{
    ::beep();
}

    શૂન્ય
gui_mch_flash(પૂર્ણાંક msec)
{
    // Do a visual beep by reversing the foreground and background colors

    જો (gui.vimWindow->Lock()) {
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
શૂન્ય
gui_mch_invert_rectangle(
	પૂર્ણાંક	r,
	પૂર્ણાંક	c,
	પૂર્ણાંક	nr,
	પૂર્ણાંક	nc)
{
    BRect rect;
    rect.ડાબું = FILL_X(c);
    rect.ટોચ = FILL_Y(r);
    rect.જમણું = rect.ડાબું + nc * gui.char_width - PEN_WIDTH;
    rect.bottom = rect.ટોચ + nr * gui.char_height - PEN_WIDTH;

    જો (gui.vimWindow->Lock()) {
	gui.vimTextArea->SetDrawingMode(B_OP_INVERT);
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->SetDrawingMode(B_OP_COPY);
	gui.vimWindow->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    શૂન્ય
gui_mch_iconify()
{
    જો (gui.vimWindow->Lock()) {
	gui.vimWindow->Minimize(સાચું);
	gui.vimWindow->Unlock();
    }
}

#જો defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    શૂન્ય
gui_mch_set_foreground(શૂન્ય)
{
    // TODO
}
#endif

/*
 * Set the window title
 */
શૂન્ય
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    જો (gui.vimWindow->Lock()) {
	gui.vimWindow->SetTitle((અક્ષર *)title);
	gui.vimWindow->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    શૂન્ય
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.ડાબું = FILL_X(gui.col);
    r.ટોચ = FILL_Y(gui.row);
    પૂર્ણાંક cells = utf_off2cells(લીટી_Offset[gui.row] + gui.col, 100); // TODO-TODO
    જો (cells>=4) cells = 1;
    r.જમણું = r.ડાબું + cells*gui.char_width - PEN_WIDTH;
    r.bottom = r.ટોચ + gui.char_height - PEN_WIDTH;

    જો (gui.vimWindow->Lock()) {
	gui.vimTextArea->StrokeRect(r);
	gui.vimWindow->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
શૂન્ય
gui_mch_draw_part_cursor(
	પૂર્ણાંક	w,
	પૂર્ણાંક	h,
	guicolor_T  color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.ડાબું =
#ifdef FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_જમણું ? FILL_X(gui.col + 1) - w :
#endif
	FILL_X(gui.col);
    r.જમણું = r.ડાબું + w - PEN_WIDTH;
    r.bottom = FILL_Y(gui.row + 1) - PEN_WIDTH;
    r.ટોચ = r.bottom - h + PEN_WIDTH;

    જો (gui.vimWindow->Lock()) {
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
    શૂન્ય
gui_mch_update()
{
    gui_mch_flush();
    જ્યારે (port_ગણતરી(gui.vdcmp) > 0 &&
	    !vim_is_આગત_buf_full() &&
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
પૂર્ણાંક
gui_mch_wait_for_chars(
	long	wtime)
{
    પૂર્ણાંક		focus;
    bigtime_t	until, timeout;
    status_t	st;

    જો (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_time() + timeout;
    }
    નહીંતર
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.in_focus;
    માટે (;;)
    {
	// Stop or start blinking when focus changes
	જો (gui.in_focus != focus)
	{
	    જો (gui.in_focus)
		gui_mch_start_blink();
	    નહીંતર
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
	જો (did_add_timer)
	    // Need to recompute the waiting time.
	    તોડો;
# endif
# ifdef FEAT_JOB_CHANNEL
	જો (has_any_channel())
	{
	    જો (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	નહીંતર જો (wtime < 0)
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

	જો (આગત_available())
	    પાછા OK;
	જો (st < B_OK)		// includes B_TIMED_OUT
	    પાછા FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	જો (wtime >= 0)
	{
	    timeout = until - system_time();
	    જો (timeout < 0)
		તોડો;
	}
    }
    પાછા FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    શૂન્ય
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    જો (gui.vimWindow->Lock()) {
	gui.vimWindow->Flush();
	gui.vimWindow->Unlock();
    }
    પાછા;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
શૂન્ય
gui_mch_clear_block(
	પૂર્ણાંક	row1,
	પૂર્ણાંક	col1,
	પૂર્ણાંક	row2,
	પૂર્ણાંક	col2)
{
    જો (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearBlock(row1, col1, row2, col2);
	gui.vimWindow->Unlock();
    }
}

    શૂન્ય
gui_mch_clear_all()
{
    જો (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearAll();
	gui.vimWindow->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
શૂન્ય
gui_mch_delete_લીટીઓ(
	પૂર્ણાંક	row,
	પૂર્ણાંક	અંક_લીટીઓ)
{
    gui.vimTextArea->mch_Delete_લીટીઓ(row, અંક_લીટીઓ);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
શૂન્ય
gui_mch_ઉમેરો_લીટીઓ(
	પૂર્ણાંક	row,
	પૂર્ણાંક	અંક_લીટીઓ)
{
    gui.vimTextArea->mch_ઉમેરો_લીટીઓ(row, અંક_લીટીઓ);
}

#જો defined(FEAT_MENU)
/*
 * Menu stuff.
 */

શૂન્ય
gui_mch_enable_menu(
	પૂર્ણાંક	flag)
{
    જો (gui.vimWindow->Lock())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->SetEnabled(flag);
	gui.vimWindow->Unlock();
    }
}

શૂન્ય
gui_mch_set_menu_pos(
	પૂર્ણાંક	x,
	પૂર્ણાંક	y,
	પૂર્ણાંક	w,
	પૂર્ણાંક	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
શૂન્ય
gui_mch_add_menu(
	vimmenu_T   *menu,
	પૂર્ણાંક	idx)
{
    vimmenu_T	*parent = menu->parent;

    //	popup menu - just create it unattached
    જો (menu_is_popup(menu->name) && parent == ખાલી) {
	BPopUpMenu* કાઢો_Up_Menu = નવું BPopUpMenu((અચલ અક્ષર*)menu->name, ખોટું, ખોટું);
	menu->submenu_id = કાઢો_Up_Menu;
	menu->id = ખાલી;
	પાછા;
    }

    જો (!menu_is_menubar(menu->name)
	    || (parent != ખાલી && parent->submenu_id == ખાલી))
	પાછા;

    જો (gui.vimWindow->Lock())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	જો ( parent )
	    tmp = parent->submenu_id;
	નહીંતર
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	જો ( ! tmp->FindItem((અચલ અક્ષર *) menu->dname)) {

	    BMenu *bmenu = નવું BMenu((અક્ષર *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->AddItem(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->FindItem((અચલ અક્ષર *) menu->dname);

	}
	gui.vimWindow->Unlock();
    }
}

    શૂન્ય
gui_mch_toggle_tearoffs(પૂર્ણાંક enable)
{
    // no tearoff menus
}

    સ્થિર BMessage *
MenuMessage(vimmenu_T *menu)
{
    BMessage *m = નવું BMessage('menu');
    m->AddPointer("VimMenu", (શૂન્ય *)menu);

    પાછા m;
}

/*
 * Add a menu item to a menu
 */
શૂન્ય
gui_mch_add_menu_item(
	vimmenu_T   *menu,
	પૂર્ણાંક	idx)
{
    પૂર્ણાંક	    mnemonic = 0;
    vimmenu_T	*parent = menu->parent;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    જો (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	જો (menu_is_toolbar(parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    જો (toolbar != ખાલી) {
		toolbar->AddButton(idx, menu);
	    }
	} નહીંતર
#endif

	જો (parent->submenu_id != ખાલી || menu_is_popup(parent->name)) {
	    જો (menu_is_separator(menu->name)) {
		BSeparatorItem *item = નવું BSeparatorItem();
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = ખાલી;
	    }
	    નહીંતર {
		BMenuItem *item = નવું BMenuItem((અક્ષર *)menu->dname,
			MenuMessage(menu));
		item->SetTarget(gui.vimTextArea);
		item->SetTrigger((અક્ષર) menu->mnemonic);
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = ખાલી;
	    }
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
શૂન્ય
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    જો (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	જો (menu->parent && menu_is_toolbar(menu->parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    જો (toolbar != ખાલી) {
		toolbar->RemoveButton(menu);
	    }
	} નહીંતર
#endif
	{
	    પ્રતિજ્ઞા(menu->submenu_id == ખાલી || menu->submenu_id->ગણતરી_વસ્તુઓ() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    જો (bmenu)
	    {
		bmenu->RemoveItem(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		જો (bmenu == gui.vimForm->MenuBar() && bmenu->ગણતરી_વસ્તુઓ() == 0)
		{
		    bmenu->ResizeTo(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    delete menu->id;
	    menu->id = ખાલી;
	    menu->submenu_id = ખાલી;

	    gui.menu_height = (પૂર્ણાંક) gui.vimForm->MenuHeight();
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
શૂન્ય
gui_mch_menu_grey(
	vimmenu_T   *menu,
	પૂર્ણાંક	grey)
{
#ifdef FEAT_TOOLBAR
    જો (menu->parent && menu_is_toolbar(menu->parent->name)) {
	જો (gui.vimWindow->Lock()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    જો (toolbar != ખાલી) {
		toolbar->GrayButton(menu, grey);
	    }
	    gui.vimWindow->Unlock();
	}
    } નહીંતર
#endif
    જો (menu->id != ખાલી)
	menu->id->SetEnabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
શૂન્ય
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	પૂર્ણાંક	hidden)
{
    જો (menu->id != ખાલી)
	menu->id->SetEnabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    શૂન્ય
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    શૂન્ય
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    જો (!menu_is_popup(menu->name) || menu->submenu_id == ખાલી)
	પાછા;

    BPopUpMenu* popupMenu = dynamic_cast<BPopUpMenu*>(menu->submenu_id);
    જો (popupMenu == ખાલી)
	પાછા;

    BPoint point;
    જો (gui.vimWindow->Lock()) {
	uint32 buttons = 0;
	gui.vimTextArea->GetMouse(&point, &buttons);
	gui.vimTextArea->ConvertToScreen(&point);
	gui.vimWindow->Unlock();
    }
    popupMenu->Go(point, સાચું);
}

#endif // FEAT_MENU

// Mouse stuff

#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
અક્ષર textplain[] = "text/plain";
અક્ષર vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    શૂન્ય
clip_mch_request_selection(Clipboard_T *cbd)
{
    જો (be_clipboard->Lock())
    {
	BMessage *m = be_clipboard->Data();
	// m->PrintToStream();

	char_u *string = ખાલી;
	ssize_t stringlen = -1;

	જો (m->FindData(textplain, B_MIME_TYPE,
		    (અચલ શૂન્ય **)&string, &stringlen) == B_OK
		|| m->FindString("text", (અચલ અક્ષર **)&string) == B_OK)
	{
	    જો (stringlen == -1)
		stringlen = STRLEN(string);

	    પૂર્ણાંક type;
	    અક્ષર *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    જો (m->FindData(vimselectiontype, B_MIME_TYPE,
			(અચલ શૂન્ય **)&seltype, &seltypelen) == B_OK)
	    {
		પસંદગી (*seltype)
		{
		    મૂળભૂત:
		    સ્થિતિ 'L':	type = MLINE;	તોડો;
		    સ્થિતિ 'C':	type = MCHAR;	તોડો;
#ifdef FEAT_VISUAL
		    સ્થિતિ 'B':	type = MBLOCK;	તોડો;
#endif
		}
	    }
	    નહીંતર
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
    શૂન્ય
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    પૂર્ણાંક
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    પાછા FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    શૂન્ય
clip_mch_set_selection(Clipboard_T *cbd)
{
    જો (be_clipboard->Lock())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->Data();
	પ્રતિજ્ઞા(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = ખાલી;
	long_u	ગણતરી;
	પૂર્ણાંક type;

	type = clip_convert_selection(&str, &ગણતરી, cbd);

	જો (type < 0)
	    પાછા;

	m->AddData(textplain, B_MIME_TYPE, (શૂન્ય *)str, ગણતરી);

	// Add type of selection
	અક્ષર	vtype;
	પસંદગી (type)
	{
	    મૂળભૂત:
	    સ્થિતિ MLINE:    vtype = 'L';    તોડો;
	    સ્થિતિ MCHAR:    vtype = 'C';    તોડો;
#ifdef FEAT_VISUAL
	    સ્થિતિ MBLOCK:   vtype = 'B';    તોડો;
#endif
	}
	m->AddData(vimselectiontype, B_MIME_TYPE, (શૂન્ય *)&vtype, 1);

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
	પૂર્ણાંક saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = નવું BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    ખાલી, ખાલી, 0, ખોટું,
	    નવું BMessage((saving == TRUE) ? 'save' : 'open'), ખાલી, સાચું);

    gui.vimApp->fBrowsedPath.Unset();

    gui.vimApp->fFilePanel->Window()->SetTitle((અક્ષર*)title);
    gui.vimApp->fFilePanel->SetPanelDirectory((અચલ અક્ષર*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    જ્યારે (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = ખાલી;
    status_t result = gui.vimApp->fBrowsedPath.InitCheck();
    જો (result == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->fBrowsedPath.Path());
    } નહીંતર
	જો (result != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    result, strerror(result));
	}

    delete gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = ખાલી;

    પાછા fileName;
}
#endif // FEAT_BROWSE


#જો defined(FEAT_GUI_DIALOG)

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

પૂર્ણાંક
gui_mch_dialog(
	પૂર્ણાંક	 type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	પૂર્ણાંક	 dfltbutton,
	char_u	*textfield,
	પૂર્ણાંક ex_cmd)
{
    VimDialog *dialog = નવું VimDialog(type, (અક્ષર*)title, (અક્ષર*)message,
	    (અક્ષર*)buttons, dfltbutton, (અક્ષર*)textfield, ex_cmd);
    તાર્કિક ret = dialog->Go();
    delete dialog;
	પાછા ret;
}

#endif // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_TO_RGB(pixel);

    પાછા ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    શૂન્ય
gui_mch_setmouse(પૂર્ણાંક x, પૂર્ણાંક y)
{
    TRACE();
    // TODO
}

#ifdef FEAT_MBYTE_IME
    શૂન્ય
im_set_position(પૂર્ણાંક row, પૂર્ણાંક col)
{
    જો (gui.vimWindow->Lock())
    {
	gui.vimTextArea->DrawIMString();
	gui.vimWindow->Unlock();
    }
    પાછા;
}
#endif

    શૂન્ય
gui_mch_show_toolbar(પૂર્ણાંક showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_height = (toolbar && showit) ? toolbar->ToolbarHeight() : 0.;
}

    શૂન્ય
gui_mch_set_toolbar_pos(પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક w, પૂર્ણાંક h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    જો (toolbar != ખાલી) {
	જો (gui.vimWindow->Lock()) {
	    toolbar->MoveTo(x, y);
	    toolbar->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

#જો defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    શૂન્ય
gui_mch_show_tabline(પૂર્ણાંક showit)
{
    Vim_Tab_લીટી *tab_લીટી = gui.vimForm->Tab_લીટી();

    જો (tab_લીટી == ખાલી)
	પાછા;

    જો (!showit != !gui.vimForm->Is_Showing_Tab_લીટી()) {
	gui.vimForm->Set_Showing_Tab_લીટી(showit != 0);
	gui.tabline_height = gui.vimForm->TablineHeight();
    }
}

    શૂન્ય
gui_mch_set_tabline_pos(પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક w, પૂર્ણાંક h)
{
    Vim_Tab_લીટી *tab_લીટી = gui.vimForm->Tab_લીટી();
    જો (tab_લીટી != ખાલી) {
	જો (gui.vimWindow->Lock()) {
	    tab_લીટી->MoveTo(x, y);
	    tab_લીટી->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    પૂર્ણાંક
gui_mch_showing_tabline()
{
    Vim_Tab_લીટી *tab_લીટી = gui.vimForm->Tab_લીટી();
    પાછા tab_લીટી != ખાલી && gui.vimForm->Is_Showing_Tab_લીટી();
}

/*
 * Update the labels of the tabline.
 */
    શૂન્ય
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    પૂર્ણાંક	    nr = 0;
    પૂર્ણાંક	    curtabidx = 0;

    Vim_Tab_લીટી *tab_લીટી = gui.vimForm->Tab_લીટી();

    જો (tab_લીટી == ખાલી)
	પાછા;

    gui.vimWindow->Lock();

    // Add a label for each tab page.  They all contain the same text area.
    માટે (tp = first_tabpage; tp != ખાલી; tp = tp->tp_next, ++nr) {
	જો (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_લીટી->TabAt(nr);

	જો (tab == ખાલી) {
	    tab = નવું Vim_Tab_લીટી::VimTab();
	    tab_લીટી->AddTab(ખાલી, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->SetLabel((અચલ અક્ષર*)NameBuff);
	tab_લીટી->Invalidate();
    }

    // Remove any old labels.
    જ્યારે (nr < tab_લીટી->ગણતરી_Tabs())
	tab_લીટી->RemoveTab(nr);

    જો (tab_લીટી->Selection() != curtabidx)
	tab_લીટી->Select(curtabidx);

    gui.vimWindow->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    શૂન્ય
gui_mch_set_curtab(પૂર્ણાંક nr)
{
    Vim_Tab_લીટી *tab_લીટી = gui.vimForm->Tab_લીટી();
    જો (tab_લીટી == ખાલી)
	પાછા;

    gui.vimWindow->Lock();

    જો (tab_લીટી->Selection() != nr -1)
	tab_લીટી->Select(nr -1);

    gui.vimWindow->Unlock();
}

#endif // FEAT_GUI_TABLINE
