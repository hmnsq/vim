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

ਬਾਹਰੀ "C" {

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <syslog.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Application.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Beep.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Bitmap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Box.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Button.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Clipboard.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <File.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <FilePanel.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <FindDirectory.h>
//#include <Font.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <IconUtils.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Input.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ListView.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <MenuBar.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Path.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <PictureButton.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <PopUpMenu.h>
//#include <Region.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Resources.h>
//#include <Roster.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Screen.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ScrollBar.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ScrollView.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <String.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <StringView.h>
//#include <SupportDefs.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <TabView.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <TextControl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <TextView.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <TranslationUtils.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <TranslatorFormats.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <View.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Window.h>

ਵਰਗ VimApp;
ਵਰਗ VimFormView;
ਵਰਗ Vim_ਪਾਠ_Area_View;
ਵਰਗ Vim_ਖਿੜਕੀ;
ਵਰਗ VimToolbar;
ਵਰਗ Vim_Tab_ਸਤਰ;

ਬਾਹਰੀ ਕੁੰਜੀ_map *ਕੁੰਜੀ_Map;
ਬਾਹਰੀ ਅੱਖਰ *ਕੁੰਜੀ_Map_Chars;

ਬਾਹਰੀ ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ B_MAX_ਪੋਰਟ_ਗਿਣਤੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B_MAX_ਪੋਰਟ_ਗਿਣਤੀ    255
#ਜੇ_ਅੰਤ

// VimApp seems comparable to the X "vimShell"
ਵਰਗ VimApp: ਜਨਤਕ BApplication
{
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ BApplication Inherited;
    ਜਨਤਕ:
    VimApp(ਅਚਲ ਅੱਖਰ *appsig);
    ~VimApp();

    // callbacks:
#ਜੇ 0
    ਆਭਾਸੀ ਖਾਲੀ Dispatch_ਸੁਨੇਹਾ(BMessage *m, BHandler *h)
    {
	m->Print_ਮੰਜ਼ਿਲ_ਧਾਰਾ();
	Inherited::Dispatch_ਸੁਨੇਹਾ(m, h);
    }
#ਜੇ_ਅੰਤ
    ਆਭਾਸੀ ਖਾਲੀ Ready_ਮੰਜ਼ਿਲ_ਲੜੀਵਾਰ();
    ਆਭਾਸੀ ਖਾਲੀ ArgvReceived(int32 argc, ਅੱਖਰ **argv);
    ਆਭਾਸੀ ਖਾਲੀ RefsReceived(BMessage *m);
    ਆਭਾਸੀ ਤਾਰਕਿਕ QuitRequested();
    ਆਭਾਸੀ ਖਾਲੀ ਸੁਨੇਹਾ_Received(BMessage *m);

    ਸਥਿਰ ਖਾਲੀ SendRefs(BMessage *m, ਤਾਰਕਿਕ changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	f_Browsed_ਰਸਤਾ;
    ਨਿਜੀ:
};

ਵਰਗ Vim_ਖਿੜਕੀ: ਜਨਤਕ BWindow
{
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ BWindow Inherited;
    ਜਨਤਕ:
    Vim_ਖਿੜਕੀ();
    ~Vim_ਖਿੜਕੀ();

    ਆਭਾਸੀ ਖਾਲੀ Dispatch_ਸੁਨੇਹਾ(BMessage *m, BHandler *h);
    ਆਭਾਸੀ ਖਾਲੀ ਖਿੜਕੀ_Activated(ਤਾਰਕਿਕ ਸਰਗਰਮ);
    ਆਭਾਸੀ ਤਾਰਕਿਕ QuitRequested();

    VimFormView	    *formView;

    ਨਿਜੀ:
    ਖਾਲੀ init();

    ਤਾਰਕਿਕ is_fullscreen = ਝੂਠ;
    BRect saved_ਚੌਖਟਾ;
    ਖਿੜਕੀ_look saved_look;
};

ਵਰਗ VimFormView: ਜਨਤਕ BView
{
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ BView Inherited;
    ਜਨਤਕ:
    VimFormView(BRect ਚੌਖਟਾ);
    ~VimFormView();

    // callbacks:
    ਆਭਾਸੀ ਖਾਲੀ AllAttached();
    ਆਭਾਸੀ ਖਾਲੀ ਚੌਖਟਾ_Resized(ਦਸ਼ਮਲਵ new_ਚੌੜਾਈ, ਦਸ਼ਮਲਵ new_ਉਚਾਈ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MENUBAR_MARGIN	1
    ਦਸ਼ਮਲਵ Menu_ਉਚਾਈ() ਅਚਲ
    { ਵਾਪਸ menuBar ? menuBar->ਚੌਖਟਾ_2().ਉਚਾਈ_2() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() ਅਚਲ
    { ਵਾਪਸ menuBar; }

    ਨਿਜੀ:
    ਖਾਲੀ init(BRect);

    BMenuBar	    *menuBar;
    Vim_ਪਾਠ_Area_View *ਪਾਠ_Area;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    ਜਨਤਕ:
    ਦਸ਼ਮਲਵ Toolbar_ਉਚਾਈ() ਅਚਲ;
    VimToolbar *ToolBar() ਅਚਲ
    { ਵਾਪਸ toolBar; }
    ਨਿਜੀ:
    VimToolbar	    *toolBar;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    ਜਨਤਕ:
    Vim_Tab_ਸਤਰ *Tab_ਸਤਰ() ਅਚਲ	{ ਵਾਪਸ tab_ਸਤਰ; }
    ਤਾਰਕਿਕ Is_Showing_Tab_ਸਤਰ() ਅਚਲ { ਵਾਪਸ showing_Tab_ਸਤਰ; }
    ਖਾਲੀ ਸੈੱਟ_Showing_Tab_ਸਤਰ(ਤਾਰਕਿਕ showing) { showing_Tab_ਸਤਰ = showing;	}
    ਦਸ਼ਮਲਵ Tabline_ਉਚਾਈ() ਅਚਲ;
    ਨਿਜੀ:
    Vim_Tab_ਸਤਰ	*tab_ਸਤਰ;
    ਪੂਰਨ	showing_Tab_ਸਤਰ;
#ਜੇ_ਅੰਤ
};

ਵਰਗ Vim_ਪਾਠ_Area_View: ਜਨਤਕ BView
{
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ BView Inherited;
    ਜਨਤਕ:
    Vim_ਪਾਠ_Area_View(BRect ਚੌਖਟਾ);
    ~Vim_ਪਾਠ_Area_View();

    // callbacks:
    ਆਭਾਸੀ ਖਾਲੀ Draw(BRect updateRect);
    ਆਭਾਸੀ ਖਾਲੀ ਕੁੰਜੀ_ਥੱਲੇ(ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, int32 ਅੰਕ_ਬਾਈਟਾਂ_2);
    ਆਭਾਸੀ ਖਾਲੀ Mouse_ਥੱਲੇ(BPoint ਬਿੰਦੂ);
    ਆਭਾਸੀ ਖਾਲੀ Mouse_ਉੱਪਰ(BPoint ਬਿੰਦੂ);
    ਆਭਾਸੀ ਖਾਲੀ MouseMoved(BPoint ਬਿੰਦੂ, uint32 transit, ਅਚਲ BMessage *ਸੁਨੇਹਾ_2);
    ਆਭਾਸੀ ਖਾਲੀ ਸੁਨੇਹਾ_Received(BMessage *m);

    // own functions:
    ਪੂਰਨ mchInitFont(char_u *name);
    ਖਾਲੀ mchDrawString(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, char_u *s, ਪੂਰਨ len, ਪੂਰਨ flags);
    ਖਾਲੀ mch_Clear_ਖੰਡ(ਪੂਰਨ row1, ਪੂਰਨ col1, ਪੂਰਨ row2, ਪੂਰਨ col2);
    ਖਾਲੀ mchClearAll();
    ਖਾਲੀ mch_Delete_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਅੰਕ_ਸਤਰਾਂ);
    ਖਾਲੀ mch_ਪਾਓ_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਅੰਕ_ਸਤਰਾਂ);

    ਸਥਿਰ ਖਾਲੀ guiSendMouseEvent(ਪੂਰਨ button, ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ repeated_click, int_u modifiers);
    ਸਥਿਰ ਖਾਲੀ guiMouseMoved(ਪੂਰਨ x, ਪੂਰਨ y);
    ਸਥਿਰ ਖਾਲੀ guiBlankMouse(ਤਾਰਕਿਕ should_hide);
    ਸਥਿਰ int_u mouse_Modifiers_ਮੰਜ਼ਿਲ_Vim(int32 beModifiers);

    int32 mouse_Drag_Event_ਗਿਣਤੀ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
    ਖਾਲੀ DrawIMString(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

    ਨਿਜੀ:
    ਖਾਲੀ init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
    ਬਣਤਰ {
	BMessenger* messenger;
	BMessage* ਸੁਨੇਹਾ_2;
	BPoint location;
	ਪੂਰਨ ਪੰਕਤੀ;
	ਪੂਰਨ ਖਾਨਾ;
	ਪੂਰਨ ਗਿਣਤੀ_2;
    } IMData;
#ਜੇ_ਅੰਤ
};

ਵਰਗ VimScrollBar: ਜਨਤਕ BScrollBar
{
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ BScrollBar Inherited;
    ਜਨਤਕ:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    ਆਭਾਸੀ ਖਾਲੀ ਮੁੱਲ_ਬਦਲਿਆ_2(ਦਸ਼ਮਲਵ new_ਮੁੱਲ);
    ਆਭਾਸੀ ਖਾਲੀ Mouse_ਉੱਪਰ(BPoint where);
    ਖਾਲੀ ਸੈੱਟ_ਮੁੱਲ(ਦਸ਼ਮਲਵ newval);
    scrollbar_T *getGsb()
    { ਵਾਪਸ gsb; }

    int32	scroll_Event_ਗਿਣਤੀ;

    ਨਿਜੀ:
    scrollbar_T *gsb;
    ਦਸ਼ਮਲਵ   ignore_ਮੁੱਲ;
};


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR

ਵਰਗ VimToolbar : ਜਨਤਕ BBox
{
    ਸਥਿਰ BBitmap *ਸਾਧਾਰਨ_Buttons_Bitmap;
    ਸਥਿਰ BBitmap *grayedButtonsBitmap;

    BBitmap *ਲੱਦੋ_Vim_Bitmap(ਅਚਲ ਅੱਖਰ* fileName);
    ਤਾਰਕਿਕ GetPictureFromBitmap(BPicture *picture_ਮੰਜ਼ਿਲ, int32 ਸੂਚਕਾਂਕ, BBitmap *bitmapFrom, ਤਾਰਕਿਕ pressed);
    ਤਾਰਕਿਕ Modify_Bitmap_ਮੰਜ਼ਿਲ_Grayed(BBitmap *bitmap);

    BList fButtonsList;
    ਖਾਲੀ InvalidateLayout();

    ਜਨਤਕ:
    VimToolbar(BRect ਚੌਖਟਾ, ਅਚਲ ਅੱਖਰ * name);
    ~VimToolbar();

    ਤਾਰਕਿਕ PrepareButtonBitmaps();

    ਤਾਰਕਿਕ ਜੋੜੋ_Button(int32 ਸੂਚਕਾਂਕ, vimmenu_T *menu);
    ਤਾਰਕਿਕ RemoveButton(vimmenu_T *menu);
    ਤਾਰਕਿਕ ਸਲੇਟੀ_Button(vimmenu_T *menu, ਪੂਰਨ grey);

    ਦਸ਼ਮਲਵ Toolbar_ਉਚਾਈ() ਅਚਲ;
    ਆਭਾਸੀ ਖਾਲੀ Attached_ਮੰਜ਼ਿਲ_ਖਿੜਕੀ();
};

BBitmap *VimToolbar::ਸਾਧਾਰਨ_Buttons_Bitmap  = ਨਿਰਾ;
BBitmap *VimToolbar::grayedButtonsBitmap  = ਨਿਰਾ;

ਅਚਲ ਦਸ਼ਮਲਵ ToolbarMargin = 3.;
ਅਚਲ ਦਸ਼ਮਲਵ ButtonMargin  = 3.;

#ਜੇ_ਅੰਤ //FEAT_TOOLBAR

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE

ਵਰਗ Vim_Tab_ਸਤਰ : ਜਨਤਕ BTabView
{
    ਜਨਤਕ:
	ਵਰਗ VimTab : ਜਨਤਕ BTab {
	    ਜਨਤਕ:
		VimTab() : BTab(ਨਵਾਂ BView(BRect(), "-Empty-", 0, 0)) {}

	    ਆਭਾਸੀ ਖਾਲੀ Select(BView* owner);
	};

	Vim_Tab_ਸਤਰ(BRect r) : BTabView(r, "vimTabLine", B_ਚੌੜਾਈ_FROM_LABEL,
	       B_FOLLOW_ਖੱਬਾ | B_FOLLOW_ਸਿਖਰ | B_FOLLOW_ਸੱਜਾ, B_WILL_DRAW | B_ਚੌਖਟਾ_EVENTS) {}

    ਦਸ਼ਮਲਵ Tabline_ਉਚਾਈ() ਅਚਲ;
    ਆਭਾਸੀ ਖਾਲੀ Mouse_ਥੱਲੇ(BPoint ਬਿੰਦੂ);
};

#ਜੇ_ਅੰਤ //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
ਵਰਗ VimFont: ਜਨਤਕ BFont
{
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ BFont Inherited;
    ਜਨਤਕ:
    VimFont();
    VimFont(ਅਚਲ VimFont *rhs);
    VimFont(ਅਚਲ BFont *rhs);
    VimFont(ਅਚਲ VimFont &rhs);
    ~VimFont();

    VimFont *ਅਗਲਾ;
    ਪੂਰਨ refcount;
    char_u *name;

    ਨਿਜੀ:
    ਖਾਲੀ init();
};

#ਜੇ defined(FEAT_GUI_DIALOG)

ਵਰਗ VimDialog : ਜਨਤਕ BWindow
{
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ BWindow Inherited;

    BButton* _CreateButton(int32 which, ਅਚਲ ਅੱਖਰ* label);

    ਜਨਤਕ:

    ਵਰਗ View : ਜਨਤਕ BView {
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ BView Inherited;

	ਜਨਤਕ:
	View(BRect ਚੌਖਟਾ);
	~View();

	ਆਭਾਸੀ ਖਾਲੀ Draw(BRect updateRect);
	ਖਾਲੀ InitIcon(int32 type);

	ਨਿਜੀ:
	BBitmap*    fIconBitmap;
    };

    VimDialog(ਪੂਰਨ type, ਅਚਲ ਅੱਖਰ *title, ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ_2,
	    ਅਚਲ ਅੱਖਰ *buttons, ਪੂਰਨ dfltbutton, ਅਚਲ ਅੱਖਰ *textfield,
	    ਪੂਰਨ ex_cmd);
    ~VimDialog();

    ਪੂਰਨ Go();

    ਆਭਾਸੀ ਖਾਲੀ ਸੁਨੇਹਾ_Received(BMessage *ਸੁਨੇਹਾ);

    ਨਿਜੀ:
    sem_id	    fDialogSem;
    ਪੂਰਨ		    f_Dialog_ਮੁੱਲ;
    BList	    fButtonsList;
    BTextView*	    f_ਸੁਨੇਹਾ_View;
    BTextControl*   f_ਨਿਵੇਸ਼_Control;
    ਅਚਲ ਅੱਖਰ*	    f_ਨਿਵੇਸ਼_ਮੁੱਲ;
};

ਵਰਗ VimSelectFontDialog : ਜਨਤਕ BWindow
{
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ BWindow Inherited;

    ਖਾਲੀ _CleanList(BListView* list);
    ਖਾਲੀ _UpdateFontStyles();
    ਖਾਲੀ Update_ਮਾਪ_ਨਿਵੇਸ਼_Preview();
    ਖਾਲੀ _UpdateFontPreview();
    ਤਾਰਕਿਕ Update_From_List_ਵਸਤੂ(BListView* list, ਅੱਖਰ* ਪਾਠ, ਪੂਰਨ ਪਾਠ_ਮਾਪ);
    ਜਨਤਕ:

    VimSelectFontDialog(font_family* family, font_style* style, ਦਸ਼ਮਲਵ* ਮਾਪ);
    ~VimSelectFontDialog();

    ਤਾਰਕਿਕ Go();

    ਆਭਾਸੀ ਖਾਲੀ ਸੁਨੇਹਾ_Received(BMessage *ਸੁਨੇਹਾ);

    ਨਿਜੀ:
    ਸਥਿਤੀ_t	    f_ਸਥਿਤੀ;
    sem_id	    fDialogSem;
    ਤਾਰਕਿਕ	    f_Dialog_ਮੁੱਲ;
    font_family*    fFamily;
    font_style*	    fStyle;
    ਦਸ਼ਮਲਵ*	    f_ਮਾਪ;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    ਦਸ਼ਮਲਵ	    f_Font_ਮਾਪ;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_ਨਿਵੇਸ਼;
};

#ਜੇ_ਅੰਤ // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

ਬਣਤਰ Main_ਦਲੀਲਾਂ {
    ਪੂਰਨ	     argc;
    ਅੱਖਰ    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	ਕੁੰਜੀ_ਸੁਨੇਹਾ_BUFSIZ	7
#ਜੇ ਕੁੰਜੀ_ਸੁਨੇਹਾ_BUFSIZ < MAX_ਕੁੰਜੀ_ਕੋਡ_LEN
#ਗਲਤੀ Increase ਕੁੰਜੀ_ਸੁਨੇਹਾ_BUFSIZ!
#ਜੇ_ਅੰਤ

ਬਣਤਰ Vim_ਕੁੰਜੀ_ਸੁਨੇਹਾ {
    char_u  ਲੰਮਾਈ;
    char_u  chars[ਕੁੰਜੀ_ਸੁਨੇਹਾ_BUFSIZ];  // contains Vim encoding
    ਤਾਰਕਿਕ    csi_escape;
};

ਬਣਤਰ Vim_Resize_ਸੁਨੇਹਾ {
    ਪੂਰਨ	    ਚੌੜਾਈ;
    ਪੂਰਨ	    ਉਚਾਈ;
};

ਬਣਤਰ Vim_Scroll_Bar_ਸੁਨੇਹਾ {
    VimScrollBar *sb;
    ਲੰਮਾ    ਮੁੱਲ;
    ਪੂਰਨ	    stillDragging;
};

ਬਣਤਰ Vim_Menu_ਸੁਨੇਹਾ {
    vimmenu_T	*guiMenu;
};

ਬਣਤਰ Vim_Mouse_ਸੁਨੇਹਾ {
    ਪੂਰਨ	    button;
    ਪੂਰਨ	    x;
    ਪੂਰਨ	    y;
    ਪੂਰਨ	    repeated_click;
    int_u   modifiers;
};

ਬਣਤਰ Vim_Mouse_Moved_ਸੁਨੇਹਾ {
    ਪੂਰਨ	    x;
    ਪੂਰਨ	    y;
};

ਬਣਤਰ Vim_Focus_ਸੁਨੇਹਾ {
    ਤਾਰਕਿਕ    ਸਰਗਰਮ;
};

ਬਣਤਰ Vim_Refs_ਸੁਨੇਹਾ {
    BMessage   *ਸੁਨੇਹਾ_2;
    ਤਾਰਕਿਕ    changedir;
};

ਬਣਤਰ Vim_Tabline_ਸੁਨੇਹਾ {
    ਪੂਰਨ	    ਸੂਚਕਾਂਕ;
};

ਬਣਤਰ Vim_Tabline_Menu_ਸੁਨੇਹਾ {
    ਪੂਰਨ	    ਸੂਚਕਾਂਕ;
    ਪੂਰਨ	    event;
};

ਬਣਤਰ Vim_ਸੁਨੇਹਾ {
    ਗਿਣਤੀ Vim_ਸੁਨੇਹਾ_Type {
	ਕੁੰਜੀ_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    ਸੰਘ {
	ਬਣਤਰ Vim_ਕੁੰਜੀ_ਸੁਨੇਹਾ    ਕੁੰਜੀ_2;
	ਬਣਤਰ Vim_Resize_ਸੁਨੇਹਾ New_ਮਾਪ;
	ਬਣਤਰ Vim_Scroll_Bar_ਸੁਨੇਹਾ	Scroll;
	ਬਣਤਰ Vim_Menu_ਸੁਨੇਹਾ   Menu;
	ਬਣਤਰ Vim_Mouse_ਸੁਨੇਹਾ  Mouse;
	ਬਣਤਰ Vim_Mouse_Moved_ਸੁਨੇਹਾ	MouseMoved;
	ਬਣਤਰ Vim_Focus_ਸੁਨੇਹਾ  Focus;
	ਬਣਤਰ Vim_Refs_ਸੁਨੇਹਾ   Refs;
	ਬਣਤਰ Vim_Tabline_ਸੁਨੇਹਾ	Tabline;
	ਬਣਤਰ Vim_Tabline_Menu_ਸੁਨੇਹਾ    TablineMenu;
    } u;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUI_ਮੰਜ਼ਿਲ_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

ਸਥਿਰ ਬਣਤਰ specialkey
{
    uint16  Be_ਕੁੰਜੀਆਂ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਕੁੰਜੀ_3(a,b)    ((a)<<8|(b))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K(a)	    ਕੁੰਜੀ_3(0,a)		// for ASCII codes
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F(b)	    ਕੁੰਜੀ_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_ਕੁੰਜੀਆਂ[] =
{
    {K(B_ਉੱਪਰ_ARROW),	'k', 'u'},
    {K(B_ਥੱਲੇ_ARROW),	    'k', 'd'},
    {K(B_ਖੱਬਾ_ARROW),	    'k', 'l'},
    {K(B_ਸੱਜਾ_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_ਪਾਓ),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_ਸਿਰਾ),		'@', '7'},
    {K(B_ਪੰਨਾ_ਉੱਪਰ),	'k', 'P'},	// XK_Prior
    {K(B_ਪੰਨਾ_ਥੱਲੇ),	    'k', 'N'},	    // XK_Next,

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਹਿਲਾ_FUNCTION_ਕੁੰਜੀ  11
    {F(B_F1_ਕੁੰਜੀ),	'k', '1'},
    {F(B_F2_ਕੁੰਜੀ),	'k', '2'},
    {F(B_F3_ਕੁੰਜੀ),	'k', '3'},
    {F(B_F4_ਕੁੰਜੀ),	'k', '4'},
    {F(B_F5_ਕੁੰਜੀ),	'k', '5'},
    {F(B_F6_ਕੁੰਜੀ),	'k', '6'},
    {F(B_F7_ਕੁੰਜੀ),	'k', '7'},
    {F(B_F8_ਕੁੰਜੀ),	'k', '8'},
    {F(B_F9_ਕੁੰਜੀ),	'k', '9'},
    {F(B_F10_ਕੁੰਜੀ),	'k', ';'},

    {F(B_F11_ਕੁੰਜੀ),	'F', '1'},
    {F(B_F12_ਕੁੰਜੀ),	'F', '2'},
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
    {F(B_PRINT_ਕੁੰਜੀ),	    '%', '9'},

#ਜੇ 0
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
#ਜੇ_ਅੰਤ

    // End of list marker:
    {0,		    0, 0}
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਕ_SPECIAL_ਕੁੰਜੀਆਂ    ARRAY_ਲੰਮਾਈ(special_ਕੁੰਜੀਆਂ)

// ---------------- VimApp ----------------

    ਸਥਿਰ ਖਾਲੀ
docd(BPath &ਰਸਤਾ)
{
    mch_chdir((ਅੱਖਰ *)ਰਸਤਾ.ਰਸਤਾ_2());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	ਸਥਿਰ ਖਾਲੀ
drop_callback(ਖਾਲੀ *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	ਸਥਿਰ ਖਾਲੀ
RefsReceived(BMessage *m, ਤਾਰਕਿਕ changedir)
{
    uint32 type;
    int32 ਗਿਣਤੀ_2;

    m->Print_ਮੰਜ਼ਿਲ_ਧਾਰਾ();
    ਚੋਣ (m->what) {
	ਹਾਲਤ B_REFS_RECEIVED:
	ਹਾਲਤ B_SIMPLE_ਅੰਕੜੇ:
	    m->GetInfo("refs", &type, &ਗਿਣਤੀ_2);
	    ਜੇ (type != B_REF_TYPE)
		ਜਾਓ bad;
	    ਰੋਕੋ;
	ਹਾਲਤ B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &ਗਿਣਤੀ_2);
	    ਜੇ (type != B_STRING_TYPE)
		ਜਾਓ bad;
	    ਜੇ (changedir) {
		ਅੱਖਰ *dirname;
		ਜੇ (m->ਲੱਭੋ_String("cwd", (ਅਚਲ ਅੱਖਰ **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    ਰੋਕੋ;
	ਮੂਲ:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    ਮਿਟਾਓ m;
	    ਵਾਪਸ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VISUAL
    reset_VIsual();
#ਜੇ_ਅੰਤ

    char_u  **fnames;
    fnames = (char_u **) alloc(ਗਿਣਤੀ_2 * ਆਕਾਰ(char_u *));
    ਪੂਰਨ fname_ਸੂਚਕਾਂਕ = 0;

    ਚੋਣ (m->what) {
	ਹਾਲਤ B_REFS_RECEIVED:
	ਹਾਲਤ B_SIMPLE_ਅੰਕੜੇ:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    ਲਈ (ਪੂਰਨ i = 0; i < ਗਿਣਤੀ_2; ++i)
	    {
		ਇੰਦਰਾਜ_ref ref;
		ਜੇ (m->ਲੱਭੋ_Ref("refs", i, &ref) == B_OK) {
		    BEntry ਇੰਦਰਾਜ(&ref, ਝੂਠ);
		    BPath ਰਸਤਾ;
		    ਇੰਦਰਾਜ.Get_ਰਸਤਾ(&ਰਸਤਾ);

		    // Change to parent directory?
		    ਜੇ (changedir) {
			BPath parentpath;
			ਰਸਤਾ.Get_ਮਾਪਾ(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    ਜੇ (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			ਜੇ (!changedir)
			    docd(ਰਸਤਾ);
		    } ਨਹੀਂਤਾਂ {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)ਰਸਤਾ.ਰਸਤਾ_2(), IObuff);
			ਜੇ (fname == ਨਿਰਾ)
			    fname = (char_u *)ਰਸਤਾ.ਰਸਤਾ_2();
			fnames[fname_ਸੂਚਕਾਂਕ++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = ਝੂਠ;
		}
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    ਲਈ (ਪੂਰਨ i = 1; i < ਗਿਣਤੀ_2; ++i)
	    {
		ਅੱਖਰ *fname;

		ਜੇ (m->ਲੱਭੋ_String("argv", i, (ਅਚਲ ਅੱਖਰ **) &fname) == B_OK) {
		    fnames[fname_ਸੂਚਕਾਂਕ++] = vim_strsave((char_u *)fname);
		}
	    }
	    ਰੋਕੋ;
	ਮੂਲ:
	    // fprintf(stderr, "case default\n");
	    ਰੋਕੋ;
    }

    ਮਿਟਾਓ m;

    // Handle the drop, :edit to get to the file
    ਜੇ (fname_ਸੂਚਕਾਂਕ > 0) {
	handle_drop(fname_ਸੂਚਕਾਂਕ, fnames, FALSE, drop_callback, ਨਿਰਾ);

	setcursor();
	ਬਾਹਰਲਾ_flush();
    } ਨਹੀਂਤਾਂ {
	vim_free(fnames);
    }
}

VimApp::VimApp(ਅਚਲ ਅੱਖਰ *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(ਨਿਰਾ)
{
}

VimApp::~VimApp()
{
}

    ਖਾਲੀ
VimApp::Ready_ਮੰਜ਼ਿਲ_ਲੜੀਵਾਰ()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    ਖਾਲੀ
VimApp::ArgvReceived(int32 arg_argc, ਅੱਖਰ **arg_argv)
{
    ਜੇ (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	ਜੇ (gui.vim_ਖਿੜਕੀ)
	    gui.vim_ਖਿੜਕੀ->Minimize(ਝੂਠ);
	BMessage *m = Current_ਸੁਨੇਹਾ();
	Detach_Current_ਸੁਨੇਹਾ();
	SendRefs(m, ਸੱਚ);
    }
}

    ਖਾਲੀ
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    ਪੂਰਨ ਹੱਦਬੰਦੀ = 15;
    ਜਦੋਂ (--ਹੱਦਬੰਦੀ >= 0 && (curbuf == ਨਿਰਾ || curbuf->b_ਸ਼ੁਰੂ_ffc == 0))
	snooze(100000);    //  0.1 s
    ਜੇ (gui.vim_ਖਿੜਕੀ)
	gui.vim_ਖਿੜਕੀ->Minimize(ਝੂਠ);
    Detach_Current_ਸੁਨੇਹਾ();
    SendRefs(m, ਸੱਚ);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    ਖਾਲੀ
VimApp::SendRefs(BMessage *m, ਤਾਰਕਿਕ changedir)
{
    Vim_Refs_ਸੁਨੇਹਾ rm;
    rm.ਸੁਨੇਹਾ_2 = m;
    rm.changedir = changedir;

    write_ਪੋਰਟ(gui.vdcmp, Vim_ਸੁਨੇਹਾ::Refs, &rm, ਆਕਾਰ(rm));
    //	calls ::RefsReceived
}

    ਖਾਲੀ
VimApp::ਸੁਨੇਹਾ_Received(BMessage *m)
{
    ਚੋਣ (m->what) {
	ਹਾਲਤ 'save':
	    {
		ਇੰਦਰਾਜ_ref refDirectory;
		m->ਲੱਭੋ_Ref("directory", &refDirectory);
		f_Browsed_ਰਸਤਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(&refDirectory);
		BString strName;
		m->ਲੱਭੋ_String("name", &strName);
		f_Browsed_ਰਸਤਾ.Append(strName.String());
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ 'open':
	    {
		ਇੰਦਰਾਜ_ref ref;
		m->ਲੱਭੋ_Ref("refs", &ref);
		f_Browsed_ਰਸਤਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(&ref);
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ B_CANCEL:
	    {
		BFilePanel *panel;
		m->ਲੱਭੋ_ਸੂਚਕ("source", (ਖਾਲੀ**)&panel);
		ਜੇ (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    ਰੋਕੋ;
	ਮੂਲ:
	    Inherited::ਸੁਨੇਹਾ_Received(m);
	    ਰੋਕੋ;
    }
}

    ਤਾਰਕਿਕ
VimApp::QuitRequested()
{
    (ਖਾਲੀ)Inherited::QuitRequested();
    ਵਾਪਸ ਝੂਠ;
}

// ---------------- VimWindow ----------------

Vim_ਖਿੜਕੀ::Vim_ਖਿੜਕੀ():
    BWindow(BRect(40, 40, 150, 150),
	    "Vim",
	    B_TITLED_ਖਿੜਕੀ,
	    0,
	    B_CURRENT_WORKSPACE)

{
    init();
}

Vim_ਖਿੜਕੀ::~Vim_ਖਿੜਕੀ()
{
    ਜੇ (formView) {
	Remove_ਬੱਚਾ(formView);
	ਮਿਟਾਓ formView;
    }
    gui.vim_ਖਿੜਕੀ = ਨਿਰਾ;
}

    ਖਾਲੀ
Vim_ਖਿੜਕੀ::init()
{
    // Attach the VimFormView
    formView = ਨਵਾਂ VimFormView(Bounds());
    ਜੇ (formView != ਨਿਰਾ) {
	ਜੋੜੋ_ਬੱਚਾ(formView);
    }
}

#ਜੇ 0  //  disabled in zeta patch
    ਖਾਲੀ
Vim_ਖਿੜਕੀ::Dispatch_ਸੁਨੇਹਾ(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    ਚੋਣ (m->what) {
	ਹਾਲਤ B_MOUSE_ਉੱਪਰ:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		ਜੇ (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->ਲੱਭੋ_ਬਿੰਦੂ("where", &where);
		    v->Mouse_ਉੱਪਰ(where);
		} ਨਹੀਂਤਾਂ {
		    Inherited::Dispatch_ਸੁਨੇਹਾ(m, h);
		}
	    }
	    ਰੋਕੋ;
	ਮੂਲ:
	    Inherited::Dispatch_ਸੁਨੇਹਾ(m, h);
    }
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
Vim_ਖਿੜਕੀ::ਖਿੜਕੀ_Activated(ਤਾਰਕਿਕ ਸਰਗਰਮ)
{
    Inherited::ਖਿੜਕੀ_Activated(ਸਰਗਰਮ);
    // the textArea gets the keyboard action
    ਜੇ (ਸਰਗਰਮ && gui.vim_ਪਾਠ_Area)
	gui.vim_ਪਾਠ_Area->MakeFocus(ਸੱਚ);

    ਬਣਤਰ Vim_Focus_ਸੁਨੇਹਾ fm;
    fm.ਸਰਗਰਮ = ਸਰਗਰਮ;

    write_ਪੋਰਟ(gui.vdcmp, Vim_ਸੁਨੇਹਾ::Focus, &fm, ਆਕਾਰ(fm));
}

    ਤਾਰਕਿਕ
Vim_ਖਿੜਕੀ::QuitRequested()
{
    ਬਣਤਰ Vim_ਕੁੰਜੀ_ਸੁਨੇਹਾ km;
    km.ਲੰਮਾਈ = 5;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਅੱਖਰ *)km.chars, "\033:qa\r", km.ਲੰਮਾਈ);
    km.csi_escape = ਝੂਠ;
    write_ਪੋਰਟ(gui.vdcmp, Vim_ਸੁਨੇਹਾ::ਕੁੰਜੀ_2, &km, ਆਕਾਰ(km));
    ਵਾਪਸ ਝੂਠ;
}
    ਖਾਲੀ
Vim_ਖਿੜਕੀ::Dispatch_ਸੁਨੇਹਾ(BMessage *m, BHandler *h)
{
    ਤਾਰਕਿਕ should_propagate = ਸੱਚ;

    ਚੋਣ (m->what)
    {
	ਹਾਲਤ B_ਕੁੰਜੀ_ਥੱਲੇ:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->ਲੱਭੋ_Int32("raw_char", &scancode);
		m->ਲੱਭੋ_Int32("modifiers", &beModifiers);

		ਜੇ (scancode == B_ENTER && (beModifiers & B_ਖੱਬਾ_COMMAND_ਕੁੰਜੀ))
		    {
			should_propagate = ਝੂਠ;
			ਜੇ (ਇਹ->is_fullscreen)
			    {
				ਇਹ->is_fullscreen = ਝੂਠ;
				Resize_ਮੰਜ਼ਿਲ(ਇਹ->saved_ਚੌਖਟਾ.ਚੌੜਾਈ_2(), ਇਹ->saved_ਚੌਖਟਾ.ਉਚਾਈ_2());
				Move_ਮੰਜ਼ਿਲ(ਇਹ->saved_ਚੌਖਟਾ.ਖੱਬੇ, ਇਹ->saved_ਚੌਖਟਾ.ਸਿਖਰ);
				ਸੈੱਟ_Look(ਇਹ->saved_look);
				ਸੈੱਟ_Flags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } ਨਹੀਂਤਾਂ {
				ਇਹ->saved_ਚੌਖਟਾ = ਚੌਖਟਾ_2();
				ਇਹ->saved_look = Look();
				ਇਹ->is_fullscreen = ਸੱਚ;
				BScreen s(ਇਹ);
				ਸੈੱਟ_Look(B_NO_BORDER_ਖਿੜਕੀ_LOOK);
				Resize_ਮੰਜ਼ਿਲ(s.ਚੌਖਟਾ_2().ਚੌੜਾਈ_2() + 1, s.ਚੌਖਟਾ_2().ਉਚਾਈ_2() + 1);
				Move_ਮੰਜ਼ਿਲ(s.ਚੌਖਟਾ_2().ਖੱਬੇ, s.ਚੌਖਟਾ_2().ਸਿਖਰ);
				ਸੈੱਟ_Flags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    ਜੇ (should_propagate)
	Inherited::Dispatch_ਸੁਨੇਹਾ(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect ਚੌਖਟਾ):
    BView(ਚੌਖਟਾ, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_ਚੌਖਟਾ_EVENTS),
    menuBar(ਨਿਰਾ),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    toolBar(ਨਿਰਾ),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_ਸਤਰ(ਨਿਰਾ),
#ਜੇ_ਅੰਤ
    ਪਾਠ_Area(ਨਿਰਾ)
{
    init(ਚੌਖਟਾ);
}

VimFormView::~VimFormView()
{
    ਜੇ (menuBar) {
	Remove_ਬੱਚਾ(menuBar);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ never
	//  deleting the menuBar leads to SEGV on exit
	//  richard@whitequeen.com Jul 99
	ਮਿਟਾਓ menuBar;
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    ਮਿਟਾਓ toolBar;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    ਮਿਟਾਓ tab_ਸਤਰ;
#ਜੇ_ਅੰਤ

    ਜੇ (ਪਾਠ_Area) {
	Remove_ਬੱਚਾ(ਪਾਠ_Area);
	ਮਿਟਾਓ ਪਾਠ_Area;
    }
    gui.vimForm = ਨਿਰਾ;
}

    ਖਾਲੀ
VimFormView::init(BRect ਚੌਖਟਾ)
{
    menuBar = ਨਵਾਂ BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    ਜੋੜੋ_ਬੱਚਾ(menuBar);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    toolBar = ਨਵਾਂ VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    ਜੋੜੋ_ਬੱਚਾ(toolBar);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    tab_ਸਤਰ = ਨਵਾਂ Vim_Tab_ਸਤਰ(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    ਜੋੜੋ_ਬੱਚਾ(tab_ਸਤਰ);
#ਜੇ_ਅੰਤ

    BRect ਬਚਿਆ = ਚੌਖਟਾ;
    ਪਾਠ_Area = ਨਵਾਂ Vim_ਪਾਠ_Area_View(ਬਚਿਆ);
    ਜੋੜੋ_ਬੱਚਾ(ਪਾਠ_Area);
    // The textArea will be resized later when menus are added

    gui.vimForm = ਇਹ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    ਦਸ਼ਮਲਵ
VimFormView::Toolbar_ਉਚਾਈ() ਅਚਲ
{
    ਵਾਪਸ toolBar ? toolBar->Toolbar_ਉਚਾਈ() : 0.;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    ਦਸ਼ਮਲਵ
VimFormView::Tabline_ਉਚਾਈ() ਅਚਲ
{
    ਵਾਪਸ (tab_ਸਤਰ && Is_Showing_Tab_ਸਤਰ()) ? tab_ਸਤਰ->Tabline_ਉਚਾਈ() : 0.;
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    ਜੇ (menuBar && ਪਾਠ_Area) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect ਬਚਿਆ = Bounds();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	ਬਚਿਆ.ਸਿਖਰ += Menu_ਉਚਾਈ();
	menuBar->Resize_ਮੰਜ਼ਿਲ(ਬਚਿਆ.ਸੱਜੇ, ਬਚਿਆ.ਸਿਖਰ);
	gui.menu_ਉਚਾਈ = (ਪੂਰਨ) Menu_ਉਚਾਈ();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
	toolBar->Move_ਮੰਜ਼ਿਲ(ਬਚਿਆ.ਖੱਬੇ, ਬਚਿਆ.ਸਿਖਰ);
	toolBar->Resize_ਮੰਜ਼ਿਲ(ਬਚਿਆ.ਸੱਜੇ, Toolbar_ਉਚਾਈ());
	ਬਚਿਆ.ਸਿਖਰ += Toolbar_ਉਚਾਈ();
	gui.toolbar_ਉਚਾਈ = Toolbar_ਉਚਾਈ();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
	tab_ਸਤਰ->Move_ਮੰਜ਼ਿਲ(ਬਚਿਆ.ਖੱਬੇ, ਬਚਿਆ.ਸਿਖਰ);
	tab_ਸਤਰ->Resize_ਮੰਜ਼ਿਲ(ਬਚਿਆ.ਸੱਜੇ + 1, Tabline_ਉਚਾਈ());
	ਬਚਿਆ.ਸਿਖਰ += Tabline_ਉਚਾਈ();
	gui.tabline_ਉਚਾਈ = Tabline_ਉਚਾਈ();
#ਜੇ_ਅੰਤ

	ਪਾਠ_Area->Resize_ਮੰਜ਼ਿਲ(ਬਚਿਆ.ਚੌੜਾਈ_2(), ਬਚਿਆ.ਉਚਾਈ_2());
	ਪਾਠ_Area->Move_ਮੰਜ਼ਿਲ(ਬਚਿਆ.ਖੱਬੇ, ਬਚਿਆ.ਸਿਖਰ);
    }


    Inherited::AllAttached();
}

    ਖਾਲੀ
VimFormView::ਚੌਖਟਾ_Resized(ਦਸ਼ਮਲਵ new_ਚੌੜਾਈ, ਦਸ਼ਮਲਵ new_ਉਚਾਈ)
{
    ਬਣਤਰ Vim_Resize_ਸੁਨੇਹਾ sm;
    ਪੂਰਨ adjust_h, adjust_w;

    new_ਚੌੜਾਈ += 1;	//  adjust from width to number of pixels occupied
    new_ਉਚਾਈ += 1;

    sm.ਚੌੜਾਈ = (ਪੂਰਨ) new_ਚੌੜਾਈ;
    sm.ਉਚਾਈ = (ਪੂਰਨ) new_ਉਚਾਈ;
    adjust_w = ((ਪੂਰਨ)new_ਚੌੜਾਈ - gui_get_ਅਧਾਰ_ਚੌੜਾਈ()) % gui.char_ਚੌੜਾਈ;
    adjust_h = ((ਪੂਰਨ)new_ਉਚਾਈ - gui_get_ਅਧਾਰ_ਉਚਾਈ()) % gui.char_ਉਚਾਈ;

    ਜੇ (adjust_w > 0 || adjust_h > 0) {
	sm.ਚੌੜਾਈ  -= adjust_w;
	sm.ਉਚਾਈ -= adjust_h;
    }

    write_ਪੋਰਟ(gui.vdcmp, Vim_ਸੁਨੇਹਾ::Resize, &sm, ਆਕਾਰ(sm));
    //	calls gui_resize_shell(new_width, new_height);

    ਵਾਪਸ;

    /*
     * The area below the vertical scrollbar is erased to the colour
     * set with SetViewColor() automatically, because we had set
     * B_WILL_DRAW. Resizing the window tight around the vertical
     * scroll bar also helps to avoid debris.
     */
}

// ---------------- VimTextAreaView ----------------

Vim_ਪਾਠ_Area_View::Vim_ਪਾਠ_Area_View(BRect ਚੌਖਟਾ):
    BView(ਚੌਖਟਾ, "VimTextAreaView", B_FOLLOW_ALL_SIDES,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
	B_WILL_DRAW | B_ਭਰਿਆ_UPDATE_ON_RESIZE | B_ਨਿਵੇਸ਼_ਵਿਧੀ_AWARE
#ਨਹੀਂਤਾਂ
	B_WILL_DRAW | B_ਭਰਿਆ_UPDATE_ON_RESIZE
#ਜੇ_ਅੰਤ
	),
    mouse_Drag_Event_ਗਿਣਤੀ(0)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
    IMData.messenger = ਨਿਰਾ;
    IMData.ਸੁਨੇਹਾ_2 = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    init(ਚੌਖਟਾ);
}

Vim_ਪਾਠ_Area_View::~Vim_ਪਾਠ_Area_View()
{
    gui.vim_ਪਾਠ_Area = ਨਿਰਾ;
}

    ਖਾਲੀ
Vim_ਪਾਠ_Area_View::init(BRect ਚੌਖਟਾ)
{
    // set up global var for fast access
    gui.vim_ਪਾਠ_Area = ਇਹ;

    /*
     * Tell the app server not to erase the view: we will
     * fill it in completely by ourselves.
     * (Does this really work? Even if not, it won't harm either.)
     */
    ਸੈੱਟ_View_Color(B_TRANSPARENT_32_ਦੋਆਧਾਰੀ_ਅੰਕ);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PEN_ਚੌੜਾਈ   1
    ਸੈੱਟ_Pen_ਮਾਪ(PEN_ਚੌੜਾਈ);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ W_ਚੌੜਾਈ(curwin)   0
}

    ਖਾਲੀ
Vim_ਪਾਠ_Area_View::Draw(BRect updateRect)
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
    rgb_color rgb = GUI_ਮੰਜ਼ਿਲ_RGB(gui.ਪਿੱਠ_pixel);
    ਸੈੱਟ_ਨੀਵਾਂ_Color(rgb);
    FillRect(updateRect, B_SOLID_ਨੀਵਾਂ);
    gui_redraw((ਪੂਰਨ) updateRect.ਖੱਬੇ, (ਪੂਰਨ) updateRect.ਸਿਖਰ,
	    (ਪੂਰਨ) (updateRect.ਚੌੜਾਈ_2() + PEN_ਚੌੜਾਈ), (ਪੂਰਨ) (updateRect.ਉਚਾਈ_2() + PEN_ਚੌੜਾਈ));

    // Clear the border areas if needed
    ਸੈੱਟ_ਨੀਵਾਂ_Color(rgb);

    ਜੇ (updateRect.ਖੱਬੇ < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.ਖੱਬੇ, updateRect.ਸਿਖਰ,
		    FILL_X(0)-PEN_ਚੌੜਾਈ, updateRect.bottom), B_SOLID_ਨੀਵਾਂ);
    ਜੇ (updateRect.ਸਿਖਰ < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.ਖੱਬੇ, updateRect.ਸਿਖਰ,
		    updateRect.ਸੱਜੇ, FILL_Y(0)-PEN_ਚੌੜਾਈ), B_SOLID_ਨੀਵਾਂ);
    ਜੇ (updateRect.ਸੱਜੇ >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((ਪੂਰਨ)Columns), updateRect.ਸਿਖਰ,
		    updateRect.ਸੱਜੇ, updateRect.bottom), B_SOLID_ਨੀਵਾਂ);
    ਜੇ (updateRect.bottom >= FILL_Y(ਪੰਕਤੀਆਂ))   //  bottom border
	FillRect(BRect(updateRect.ਖੱਬੇ, FILL_Y((ਪੂਰਨ)ਪੰਕਤੀਆਂ),
		    updateRect.ਸੱਜੇ, updateRect.bottom), B_SOLID_ਨੀਵਾਂ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
    DrawIMString();
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
Vim_ਪਾਠ_Area_View::ਕੁੰਜੀ_ਥੱਲੇ(ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, int32 ਅੰਕ_ਬਾਈਟਾਂ_2)
{
    ਬਣਤਰ Vim_ਕੁੰਜੀ_ਸੁਨੇਹਾ km;
    char_u *dest = km.chars;

    ਤਾਰਕਿਕ canHaveVimModifiers = ਝੂਠ;

    BMessage *ਸੁਨੇਹਾ = ਖਿੜਕੀ()->Current_ਸੁਨੇਹਾ();
    ਪ੍ਰਤਿਜ੍ਞਾ(ਸੁਨੇਹਾ);
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
    ਜੇ (ਅੰਕ_ਬਾਈਟਾਂ_2 > 1) {
	// This cannot be a special key
	ਜੇ (ਅੰਕ_ਬਾਈਟਾਂ_2 > ਕੁੰਜੀ_ਸੁਨੇਹਾ_BUFSIZ)
	    ਅੰਕ_ਬਾਈਟਾਂ_2 = ਕੁੰਜੀ_ਸੁਨੇਹਾ_BUFSIZ;	    //	should never happen... ???
	km.ਲੰਮਾਈ = ਅੰਕ_ਬਾਈਟਾਂ_2;
	ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਅੱਖਰ *)dest, ਬਾਈਟਾਂ, ਅੰਕ_ਬਾਈਟਾਂ_2);
	km.csi_escape = ਸੱਚ;
    } ਨਹੀਂਤਾਂ {
	int32 scancode = 0;
	ਸੁਨੇਹਾ->ਲੱਭੋ_Int32("key", &scancode);

	int32 beModifiers = 0;
	ਸੁਨੇਹਾ->ਲੱਭੋ_Int32("modifiers", &beModifiers);

	char_u string[3];
	ਪੂਰਨ len = 0;
	km.ਲੰਮਾਈ = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	ਪ੍ਰਤਿਜ੍ਞਾ(B_BACKSPACE <= 0x20);
	ਪ੍ਰਤਿਜ੍ਞਾ(B_DELETE == 0x7F);
	ਜੇ (((char_u)ਬਾਈਟਾਂ[0] <= 0x20 || (char_u)ਬਾਈਟਾਂ[0] == 0x7F) &&
		ਅੰਕ_ਬਾਈਟਾਂ_2 == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    ਜੇ (beModifiers & B_CONTROL_ਕੁੰਜੀ) {
		ਪੂਰਨ ਸੂਚਕਾਂਕ = ਕੁੰਜੀ_Map->ਸਾਧਾਰਨ_map[scancode];
		ਪੂਰਨ new_ਅੰਕ_ਬਾਈਟਾਂ = ਕੁੰਜੀ_Map_Chars[ਸੂਚਕਾਂਕ];
		char_u *new_ਬਾਈਟਾਂ = (char_u *)&ਕੁੰਜੀ_Map_Chars[ਸੂਚਕਾਂਕ + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		ਜੇ (new_ਅੰਕ_ਬਾਈਟਾਂ != 1 || (*new_ਬਾਈਟਾਂ > 0x20 &&
			    *new_ਬਾਈਟਾਂ != 0x7F )) {
		    ਜਾਓ notspecial;
		}
		ਬਾਈਟਾਂ = (ਅੱਖਰ *)new_ਬਾਈਟਾਂ;
	    }
	    canHaveVimModifiers = ਸੱਚ;

	    uint16 beoskey;
	    ਪੂਰਨ ਪਹਿਲਾ, ਆਖਰੀ;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    ਜੇ (ਅੰਕ_ਬਾਈਟਾਂ_2 == 0 || ਬਾਈਟਾਂ[0] == B_FUNCTION_ਕੁੰਜੀ) {
		beoskey = F(scancode);
		ਪਹਿਲਾ = ਪਹਿਲਾ_FUNCTION_ਕੁੰਜੀ;
		ਆਖਰੀ = ਅੰਕ_SPECIAL_ਕੁੰਜੀਆਂ;
	    } ਨਹੀਂਤਾਂ ਜੇ (*ਬਾਈਟਾਂ == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		ਪਹਿਲਾ = ਆਖਰੀ = 0;
	    } ਨਹੀਂਤਾਂ {
		beoskey = K(ਬਾਈਟਾਂ[0]);
		ਪਹਿਲਾ = 0;
		ਆਖਰੀ = ਪਹਿਲਾ_FUNCTION_ਕੁੰਜੀ;
	    }

	    ਲਈ (ਪੂਰਨ i = ਪਹਿਲਾ; i < ਆਖਰੀ; i++) {
		ਜੇ (special_ਕੁੰਜੀਆਂ[i].Be_ਕੁੰਜੀਆਂ == beoskey) {
		    string[0] = CSI;
		    string[1] = special_ਕੁੰਜੀਆਂ[i].vim_code0;
		    string[2] = special_ਕੁੰਜੀਆਂ[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	ਜੇ (len == 0) {
	    string[0] = ਬਾਈਟਾਂ[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#ਜੇ 0
	ਜੇ (len == 3 ||
		ਬਾਈਟਾਂ[0] == B_SPACE || ਬਾਈਟਾਂ[0] == B_TAB ||
		ਬਾਈਟਾਂ[0] == B_RETURN || ਬਾਈਟਾਂ[0] == '\r' ||
		ਬਾਈਟਾਂ[0] == B_ESCAPE)
#ਨਹੀਂਤਾਂ
	    ਜੇ (canHaveVimModifiers)
#ਜੇ_ਅੰਤ
	    {
		ਪੂਰਨ modifiers;
		modifiers = 0;
		ਜੇ (beModifiers & B_ਖਿਸਕਾਓ_ਕੁੰਜੀ)
		    modifiers |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
		ਜੇ (beModifiers & B_CONTROL_ਕੁੰਜੀ)
		    modifiers |= MOD_ਨਕਾਬ_CTRL;
		ਜੇ (beModifiers & B_OPTION_ਕੁੰਜੀ)
		    modifiers |= MOD_ਨਕਾਬ_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		ਪੂਰਨ ਕੁੰਜੀ;
		ਜੇ (string[0] == CSI && len == 3)
		    ਕੁੰਜੀ = ਮੰਜ਼ਿਲ_SPECIAL(string[1], string[2]);
		ਨਹੀਂਤਾਂ
		    ਕੁੰਜੀ = string[0];
		ਕੁੰਜੀ = simplify_ਕੁੰਜੀ(ਕੁੰਜੀ, &modifiers);
		ਜੇ (IS_SPECIAL(ਕੁੰਜੀ))
		{
		    string[0] = CSI;
		    string[1] = K_ਦੂਜਾ(ਕੁੰਜੀ);
		    string[2] = K_ਤੀਜਾ(ਕੁੰਜੀ);
		    len = 3;
		}
		ਨਹੀਂਤਾਂ
		{
		    string[0] = ਕੁੰਜੀ;
		    len = 1;
		}

		ਜੇ (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.ਲੰਮਾਈ = 3;
		}
	    }
	ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਅੱਖਰ *)dest, string, len);
	km.ਲੰਮਾਈ += len;
	km.csi_escape = ਝੂਠ;
    }

    write_ਪੋਰਟ(gui.vdcmp, Vim_ਸੁਨੇਹਾ::ਕੁੰਜੀ_2, &km, ਆਕਾਰ(km));

    /*
     * blank out the pointer if necessary
     */
    ਜੇ (p_mh && !gui.ਸੂਚਕ_hidden)
    {
	guiBlankMouse(ਸੱਚ);
	gui.ਸੂਚਕ_hidden = TRUE;
    }
}
ਖਾਲੀ
Vim_ਪਾਠ_Area_View::guiSendMouseEvent(
	ਪੂਰਨ	button,
	ਪੂਰਨ	x,
	ਪੂਰਨ	y,
	ਪੂਰਨ	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_ਸੁਨੇਹਾ mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_ਪੋਰਟ(gui.vdcmp, Vim_ਸੁਨੇਹਾ::Mouse, &mm, ਆਕਾਰ(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    ਜੇ (gui.ਸੂਚਕ_hidden)
    {
	guiBlankMouse(ਝੂਠ);
	gui.ਸੂਚਕ_hidden = FALSE;
    }
}

ਖਾਲੀ
Vim_ਪਾਠ_Area_View::guiMouseMoved(
	ਪੂਰਨ	x,
	ਪੂਰਨ	y)
{
    Vim_Mouse_Moved_ਸੁਨੇਹਾ mm;

    mm.x = x;
    mm.y = y;

    write_ਪੋਰਟ(gui.vdcmp, Vim_ਸੁਨੇਹਾ::MouseMoved, &mm, ਆਕਾਰ(mm));

    ਜੇ (gui.ਸੂਚਕ_hidden)
    {
	guiBlankMouse(ਝੂਠ);
	gui.ਸੂਚਕ_hidden = FALSE;
    }
}

    ਖਾਲੀ
Vim_ਪਾਠ_Area_View::guiBlankMouse(ਤਾਰਕਿਕ should_hide)
{
    ਜੇ (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } ਨਹੀਂਤਾਂ {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
Vim_ਪਾਠ_Area_View::mouse_Modifiers_ਮੰਜ਼ਿਲ_Vim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    ਜੇ (beModifiers & B_ਖਿਸਕਾਓ_ਕੁੰਜੀ)
	vim_modifiers |= MOUSE_ਖਿਸਕਾਓ;
    ਜੇ (beModifiers & B_CONTROL_ਕੁੰਜੀ)
	vim_modifiers |= MOUSE_CTRL;
    ਜੇ (beModifiers & B_OPTION_ਕੁੰਜੀ)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    ਵਾਪਸ vim_modifiers;
}

    ਖਾਲੀ
Vim_ਪਾਠ_Area_View::Mouse_ਥੱਲੇ(BPoint ਬਿੰਦੂ)
{
    BMessage *m = ਖਿੜਕੀ()->Current_ਸੁਨੇਹਾ();
    ਪ੍ਰਤਿਜ੍ਞਾ(m);

    int32 buttons = 0;
    m->ਲੱਭੋ_Int32("buttons", &buttons);

    ਪੂਰਨ vimButton;

    ਜੇ (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_ਖੱਬਾ;
    ਨਹੀਂਤਾਂ ਜੇ (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_ਸੱਜਾ;
    ਨਹੀਂਤਾਂ ਜੇ (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_ਵਿਚਕਾਰ;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ;		// Unknown button

    vimMouseButton = 1;	    // don't care which one

    // Handle multiple clicks
    int32 clicks = 0;
    m->ਲੱਭੋ_Int32("clicks", &clicks);

    int32 modifiers = 0;
    m->ਲੱਭੋ_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_ਮੰਜ਼ਿਲ_Vim(modifiers);

    guiSendMouseEvent(vimButton, ਬਿੰਦੂ.x, ਬਿੰਦੂ.y,
	    clicks > 1 /* = repeated_click*/, vimMouseModifiers);
}

    ਖਾਲੀ
Vim_ਪਾਠ_Area_View::Mouse_ਉੱਪਰ(BPoint ਬਿੰਦੂ)
{
    vimMouseButton = 0;

    BMessage *m = ਖਿੜਕੀ()->Current_ਸੁਨੇਹਾ();
    ਪ੍ਰਤਿਜ੍ਞਾ(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->ਲੱਭੋ_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_ਮੰਜ਼ਿਲ_Vim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, ਬਿੰਦੂ.x, ਬਿੰਦੂ.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::Mouse_ਉੱਪਰ(ਬਿੰਦੂ);
}

    ਖਾਲੀ
Vim_ਪਾਠ_Area_View::MouseMoved(BPoint ਬਿੰਦੂ, uint32 transit, ਅਚਲ BMessage *ਸੁਨੇਹਾ_2)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    ਜੇ (gui.ਸੂਚਕ_hidden)
    {
	guiBlankMouse(ਝੂਠ);
	gui.ਸੂਚਕ_hidden = FALSE;
    }

    ਜੇ (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(ਬਿੰਦੂ.x, ਬਿੰਦੂ.y);
	ਵਾਪਸ;
    }

    atomic_ਜੋੜੋ(&mouse_Drag_Event_ਗਿਣਤੀ, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, ਬਿੰਦੂ.x, ਬਿੰਦੂ.y, 0, vimMouseModifiers);
}

    ਖਾਲੀ
Vim_ਪਾਠ_Area_View::ਸੁਨੇਹਾ_Received(BMessage *m)
{
    ਚੋਣ (m->what) {
	ਹਾਲਤ 'menu':
	    {
		Vim_Menu_ਸੁਨੇਹਾ mm;
		mm.guiMenu = ਨਿਰਾ;  // in case no pointer in msg
		m->ਲੱਭੋ_ਸੂਚਕ("VimMenu", (ਖਾਲੀ **)&mm.guiMenu);
		write_ਪੋਰਟ(gui.vdcmp, Vim_ਸੁਨੇਹਾ::Menu, &mm, ਆਕਾਰ(mm));
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ B_MOUSE_WHEEL_ਬਦਲਿਆ:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		ਦਸ਼ਮਲਵ small=0, big=0, dy=0;
		m->ਲੱਭੋ_Float("be:wheel_delta_y", &dy);
		scb->Get_ਕਦਮਾਂ(&small, &big);
		scb->ਸੈੱਟ_ਮੁੱਲ(scb->ਮੁੱਲ_2()+small*dy*3);
		scb->ਮੁੱਲ_ਬਦਲਿਆ_2(scb->ਮੁੱਲ_2());
#ਜੇ 0
		scb = curwin->w_scrollbars[0].id;
		scb->Get_ਕਦਮਾਂ(&small, &big);
		scb->ਸੈੱਟ_ਮੁੱਲ(scb->ਮੁੱਲ_2()+small*dy);
		scb->ਮੁੱਲ_ਬਦਲਿਆ_2(scb->ਮੁੱਲ_2());
#ਜੇ_ਅੰਤ
	    }
	    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
	ਹਾਲਤ B_ਨਿਵੇਸ਼_ਵਿਧੀ_EVENT:
	    {
		int32 opcode;
		m->ਲੱਭੋ_Int32("be:opcode", &opcode);
		ਚੋਣ(opcode)
		{
		    ਹਾਲਤ B_ਨਿਵੇਸ਼_ਵਿਧੀ_STARTED:
			ਜੇ (!IMData.messenger) ਮਿਟਾਓ IMData.messenger;
			IMData.messenger = ਨਵਾਂ BMessenger();
			m->ਲੱਭੋ_Messenger("be:reply_to", IMData.messenger);
			ਰੋਕੋ;
		    ਹਾਲਤ B_ਨਿਵੇਸ਼_ਵਿਧੀ_ਬਦਲਿਆ:
			{
			    BString str;
			    ਤਾਰਕਿਕ confirmed;
			    ਜੇ (IMData.ਸੁਨੇਹਾ_2) *(IMData.ਸੁਨੇਹਾ_2) = *m;
			    ਨਹੀਂਤਾਂ	       IMData.ਸੁਨੇਹਾ_2 = ਨਵਾਂ BMessage(*m);
			    DrawIMString();
			    m->ਲੱਭੋ_Bool("be:confirmed", &confirmed);
			    ਜੇ (confirmed)
			    {
				m->ਲੱਭੋ_String("be:string", &str);
				char_u *chars = (char_u*)str.String();
				ਬਣਤਰ Vim_ਕੁੰਜੀ_ਸੁਨੇਹਾ km;
				km.csi_escape = ਸੱਚ;
				ਪੂਰਨ clen;
				ਪੂਰਨ i = 0;
				ਜਦੋਂ (i < str.ਲੰਮਾਈ_2())
				{
				    clen = utf_ptr2len(chars+i);
				    ਸਮ੍ਰਿਤੀ_ਨਕਲ(km.chars, chars+i, clen);
				    km.ਲੰਮਾਈ = clen;
				    write_ਪੋਰਟ(gui.vdcmp, Vim_ਸੁਨੇਹਾ::ਕੁੰਜੀ_2, &km, ਆਕਾਰ(km));
				    i += clen;
				}
			    }
			}
			ਰੋਕੋ;
		    ਹਾਲਤ B_ਨਿਵੇਸ਼_ਵਿਧੀ_LOCATION_REQUEST:
			{
			    BMessage ਸੁਨੇਹਾ(B_ਨਿਵੇਸ਼_ਵਿਧੀ_EVENT);
			    ਸੁਨੇਹਾ.ਜੋੜੋ_Int32("be:opcode", B_ਨਿਵੇਸ਼_ਵਿਧੀ_LOCATION_REQUEST);
			    ਸੁਨੇਹਾ.ਜੋੜੋ_ਬਿੰਦੂ("be:location_reply", IMData.location);
			    ਸੁਨੇਹਾ.ਜੋੜੋ_Float("be:height_reply", FILL_Y(1));
			    IMData.messenger->Send_ਸੁਨੇਹਾ(&ਸੁਨੇਹਾ);
			}
			ਰੋਕੋ;
		    ਹਾਲਤ B_ਨਿਵੇਸ਼_ਵਿਧੀ_STOPPED:
			ਮਿਟਾਓ IMData.messenger;
			ਮਿਟਾਓ IMData.ਸੁਨੇਹਾ_2;
			IMData.messenger = ਨਿਰਾ;
			IMData.ਸੁਨੇਹਾ_2 = ਨਿਰਾ;
			ਰੋਕੋ;
		}
	    }
	    // TODO: sz: break here???
#ਜੇ_ਅੰਤ
	ਮੂਲ:
	    ਜੇ (m->WasDropped()) {
		BWindow *w = ਖਿੜਕੀ();
		w->Detach_Current_ਸੁਨੇਹਾ();
		w->Minimize(ਝੂਠ);
		VimApp::SendRefs(m, (modifiers() & B_ਖਿਸਕਾਓ_ਕੁੰਜੀ) != 0);
	    } ਨਹੀਂਤਾਂ {
		Inherited::ਸੁਨੇਹਾ_Received(m);
	    }
	    ਰੋਕੋ;
    }
}

    ਪੂਰਨ
Vim_ਪਾਠ_Area_View::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    ਜੇ (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_ਸੈੱਟ_font((GuiFont)newFont);
	ਜੇ (name && STRCMP(name, "*") != 0)
	    hl_ਸੈੱਟ_font_name(name);

	ਸੈੱਟ_Drawing_ਬਹੁਲਕ(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	ਵਾਪਸ OK;
    }
    ਵਾਪਸ FAIL;
}

    ਖਾਲੀ
Vim_ਪਾਠ_Area_View::mchDrawString(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, char_u *s, ਪੂਰਨ len, ਪੂਰਨ flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    ਜੇ (!(flags & DRAW_TRANSP)) {
	ਪੂਰਨ cells;
	cells = 0;
	ਲਈ (ਪੂਰਨ i=0; i<len; i++) {
	    ਪੂਰਨ cn = utf_ptr2cells((char_u *)(s+i));
	    ਜੇ (cn<4) cells += cn;
	}

	BRect r(FILL_X(ਖਾਨਾ), FILL_Y(ਪੰਕਤੀ),
		FILL_X(ਖਾਨਾ + cells) - PEN_ਚੌੜਾਈ, FILL_Y(ਪੰਕਤੀ + 1) - PEN_ਚੌੜਾਈ);
	FillRect(r, B_SOLID_ਨੀਵਾਂ);
    }

    BFont font;
    ਇਹ->GetFont(&font);
    ਜੇ (!font.IsFixed())
    {
	ਅੱਖਰ* p = (ਅੱਖਰ*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	ਪੂਰਨ cells;
	ਜਦੋਂ ((p - (ਅੱਖਰ*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.ਸੈੱਟ(ਪਾਠ_X(ਖਾਨਾ+lastpos), ਪਾਠ_Y(ਪੰਕਤੀ));
	    DrawString(p, clen, where);
	    ਜੇ (flags & DRAW_BOLD) {
		where.x += 1.0;
		ਸੈੱਟ_Drawing_ਬਹੁਲਕ(B_OP_BLEND);
		DrawString(p, clen, where);
		ਸੈੱਟ_Drawing_ਬਹੁਲਕ(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    ਜੇ (cells<4) lastpos += cells;
	    ਨਹੀਂਤਾਂ	lastpos++;
	    p += clen;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	BPoint where(ਪਾਠ_X(ਖਾਨਾ), ਪਾਠ_Y(ਪੰਕਤੀ));
	DrawString((ਅੱਖਰ*)s, len, where);
	ਜੇ (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    ਸੈੱਟ_Drawing_ਬਹੁਲਕ(B_OP_BLEND);
	    DrawString((ਅੱਖਰ*)s, len, where);
	    ਸੈੱਟ_Drawing_ਬਹੁਲਕ(B_OP_COPY);
	}
    }

    ਜੇ (flags & DRAW_UNDERL) {
	ਪੂਰਨ cells;
	cells = 0;
	ਲਈ (ਪੂਰਨ i=0; i<len; i++) {
	    ਪੂਰਨ cn = utf_ptr2cells((char_u *)(s+i));
	    ਜੇ (cn<4) cells += cn;
	}

	BPoint ਸ਼ੁਰੂ(FILL_X(ਖਾਨਾ), FILL_Y(ਪੰਕਤੀ + 1) - PEN_ਚੌੜਾਈ);
	BPoint ਸਿਰਾ(FILL_X(ਖਾਨਾ + cells) - PEN_ਚੌੜਾਈ, ਸ਼ੁਰੂ.y);

	Stroke_ਸਤਰ(ਸ਼ੁਰੂ, ਸਿਰਾ);
    }
}

ਖਾਲੀ
Vim_ਪਾਠ_Area_View::mch_Clear_ਖੰਡ(
	ਪੂਰਨ	row1,
	ਪੂਰਨ	col1,
	ਪੂਰਨ	row2,
	ਪੂਰਨ	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_ਚੌੜਾਈ, FILL_Y(row2 + 1) - PEN_ਚੌੜਾਈ);
    gui_mch_ਸੈੱਟ_bg_color(gui.ਪਿੱਠ_pixel);
    FillRect(r, B_SOLID_ਨੀਵਾਂ);
}

    ਖਾਲੀ
Vim_ਪਾਠ_Area_View::mchClearAll()
{
    gui_mch_ਸੈੱਟ_bg_color(gui.ਪਿੱਠ_pixel);
    FillRect(Bounds(), B_SOLID_ਨੀਵਾਂ);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    ਖਾਲੀ
Vim_ਪਾਠ_Area_View::mch_Delete_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਅੰਕ_ਸਤਰਾਂ)
{
    BRect ਸਰੋਤ, dest;
    ਸਰੋਤ.ਖੱਬੇ = FILL_X(gui.scroll_region_ਖੱਬਾ);
    ਸਰੋਤ.ਸਿਖਰ = FILL_Y(ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ);
    ਸਰੋਤ.ਸੱਜੇ = FILL_X(gui.scroll_region_ਸੱਜਾ + 1) - PEN_ਚੌੜਾਈ;
    ਸਰੋਤ.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_ਚੌੜਾਈ;

    dest.ਖੱਬੇ = FILL_X(gui.scroll_region_ਖੱਬਾ);
    dest.ਸਿਖਰ = FILL_Y(ਪੰਕਤੀ);
    dest.ਸੱਜੇ = FILL_X(gui.scroll_region_ਸੱਜਾ + 1) - PEN_ਚੌੜਾਈ;
    dest.bottom = FILL_Y(gui.scroll_region_bot - ਅੰਕ_ਸਤਰਾਂ + 1) - PEN_ਚੌੜਾਈ;

    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	// Clear one column more for when bold has spilled over
	Copy_ਬਿੱਟ(ਸਰੋਤ, dest);
	gui_clear_ਖੰਡ(gui.scroll_region_bot - ਅੰਕ_ਸਤਰਾਂ + 1,
		gui.scroll_region_ਖੱਬਾ,
		gui.scroll_region_bot, gui.scroll_region_ਸੱਜਾ);


	gui.vim_ਖਿੜਕੀ->Unlock();
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
    ਖਾਲੀ
Vim_ਪਾਠ_Area_View::mch_ਪਾਓ_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਅੰਕ_ਸਤਰਾਂ)
{
    BRect ਸਰੋਤ, dest;

    // XXX Attempt at a hack:
    gui.vim_ਖਿੜਕੀ->UpdateIfNeeded();
    ਸਰੋਤ.ਖੱਬੇ = FILL_X(gui.scroll_region_ਖੱਬਾ);
    ਸਰੋਤ.ਸਿਖਰ = FILL_Y(ਪੰਕਤੀ);
    ਸਰੋਤ.ਸੱਜੇ = FILL_X(gui.scroll_region_ਸੱਜਾ + 1) - PEN_ਚੌੜਾਈ;
    ਸਰੋਤ.bottom = FILL_Y(gui.scroll_region_bot - ਅੰਕ_ਸਤਰਾਂ + 1) - PEN_ਚੌੜਾਈ;

    dest.ਖੱਬੇ = FILL_X(gui.scroll_region_ਖੱਬਾ);
    dest.ਸਿਖਰ = FILL_Y(ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ);
    dest.ਸੱਜੇ = FILL_X(gui.scroll_region_ਸੱਜਾ + 1) - PEN_ਚੌੜਾਈ;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_ਚੌੜਾਈ;

    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	// Clear one column more for when bold has spilled over
	Copy_ਬਿੱਟ(ਸਰੋਤ, dest);
	gui_clear_ਖੰਡ(ਪੰਕਤੀ, gui.scroll_region_ਖੱਬਾ,
		ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ - 1, gui.scroll_region_ਸੱਜਾ);

	gui.vim_ਖਿੜਕੀ->Unlock();
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
/*
 * DrawIMString draws string with IMData.message.
 */
ਖਾਲੀ Vim_ਪਾਠ_Area_View::DrawIMString(ਖਾਲੀ)
{
    ਸਥਿਰ ਅਚਲ rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    ਅਚਲ ਅੱਖਰ* s;
    ਪੂਰਨ len;
    BMessage* ਸੁਨੇਹਾ = IMData.ਸੁਨੇਹਾ_2;
    ਜੇ (!ਸੁਨੇਹਾ)
	ਵਾਪਸ;
    gui_redraw_ਖੰਡ(IMData.ਪੰਕਤੀ, 0,
	    IMData.ਪੰਕਤੀ + IMData.ਗਿਣਤੀ_2, W_ਚੌੜਾਈ(curwin), GUI_MON_NOCLEAR);
    ਤਾਰਕਿਕ confirmed = ਝੂਠ;
    ਸੁਨੇਹਾ->ਲੱਭੋ_Bool("be:confirmed", &confirmed);
    ਜੇ (confirmed)
	ਵਾਪਸ;
    rgb_color hcolor = ਉੱਚਾ_Color(), lcolor = ਨੀਵਾਂ_Color();
    ਸੁਨੇਹਾ->ਲੱਭੋ_String("be:string", &str);
    s = str.String();
    len = str.ਲੰਮਾਈ_2();
    ਸੈੱਟ_ਉੱਚਾ_Color(0, 0, 0);
    IMData.ਪੰਕਤੀ = gui.ਪੰਕਤੀ;
    IMData.ਖਾਨਾ = gui.ਖਾਨਾ;
    int32 sel_ਸ਼ੁਰੂ = 0, sel_ਸਿਰਾ = 0;
    ਸੁਨੇਹਾ->ਲੱਭੋ_Int32("be:selection", 0, &sel_ਸ਼ੁਰੂ);
    ਸੁਨੇਹਾ->ਲੱਭੋ_Int32("be:selection", 1, &sel_ਸਿਰਾ);
    ਪੂਰਨ clen, cn;
    BPoint pos(IMData.ਖਾਨਾ, 0);
    BRect r;
    BPoint where;
    IMData.location = Convert_ਮੰਜ਼ਿਲ_Screen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.ਪੰਕਤੀ + pos.y)));
    ਲਈ (ਪੂਰਨ i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	ਜੇ (pos.x + cn > W_ਚੌੜਾਈ(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	ਜੇ (sel_ਸ਼ੁਰੂ<=i && i<sel_ਸਿਰਾ)
	{
	    ਸੈੱਟ_ਨੀਵਾਂ_Color(r_highlight);
	    IMData.location = Convert_ਮੰਜ਼ਿਲ_Screen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.ਪੰਕਤੀ + pos.y)));
	}
	ਨਹੀਂਤਾਂ
	{
	    ਸੈੱਟ_ਨੀਵਾਂ_Color(b_highlight);
	}
	r.ਸੈੱਟ(FILL_X(pos.x), FILL_Y(IMData.ਪੰਕਤੀ + pos.y),
		FILL_X(pos.x + cn) - PEN_ਚੌੜਾਈ,
		FILL_Y(IMData.ਪੰਕਤੀ + pos.y + 1) - PEN_ਚੌੜਾਈ);
	FillRect(r, B_SOLID_ਨੀਵਾਂ);
	where.ਸੈੱਟ(ਪਾਠ_X(pos.x), ਪਾਠ_Y(IMData.ਪੰਕਤੀ + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.ਗਿਣਤੀ_2 = (ਪੂਰਨ)pos.y;

    ਸੈੱਟ_ਉੱਚਾ_Color(hcolor);
    ਸੈੱਟ_ਨੀਵਾਂ_Color(lcolor);
}
#ਜੇ_ਅੰਤ
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
	    "vim scrollbar", (BView *)ਨਿਰਾ,
	    0.0, 10.0, posture),
    ignore_ਮੁੱਲ(-1),
    scroll_Event_ਗਿਣਤੀ(0)
{
    gsb = g;
    ਸੈੱਟ_Resizing_ਬਹੁਲਕ(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    ਖਾਲੀ
VimScrollBar::ਮੁੱਲ_ਬਦਲਿਆ_2(ਦਸ਼ਮਲਵ new_ਮੁੱਲ)
{
    ਜੇ (ignore_ਮੁੱਲ >= 0.0 && new_ਮੁੱਲ == ignore_ਮੁੱਲ) {
	ignore_ਮੁੱਲ = -1;
	ਵਾਪਸ;
    }
    ignore_ਮੁੱਲ = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_ਜੋੜੋ(&scroll_Event_ਗਿਣਤੀ, 1);

    ਬਣਤਰ Vim_Scroll_Bar_ਸੁਨੇਹਾ sm;

    sm.sb = ਇਹ;
    sm.ਮੁੱਲ = (ਲੰਮਾ) new_ਮੁੱਲ;
    sm.stillDragging = TRUE;

    write_ਪੋਰਟ(gui.vdcmp, Vim_ਸੁਨੇਹਾ::ScrollBar, &sm, ਆਕਾਰ(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    ਖਾਲੀ
VimScrollBar::Mouse_ਉੱਪਰ(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_ਜੋੜੋ(&scroll_Event_ਗਿਣਤੀ, 1);

    ਬਣਤਰ Vim_Scroll_Bar_ਸੁਨੇਹਾ sm;

    sm.sb = ਇਹ;
    sm.ਮੁੱਲ = (ਲੰਮਾ) ਮੁੱਲ_2();
    sm.stillDragging = FALSE;

    write_ਪੋਰਟ(gui.vdcmp, Vim_ਸੁਨੇਹਾ::ScrollBar, &sm, ਆਕਾਰ(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::Mouse_ਉੱਪਰ(where);
}

    ਖਾਲੀ
VimScrollBar::ਸੈੱਟ_ਮੁੱਲ(ਦਸ਼ਮਲਵ new_ਮੁੱਲ)
{
    ਜੇ (new_ਮੁੱਲ == ਮੁੱਲ_2())
	ਵਾਪਸ;

    ignore_ਮੁੱਲ = new_ਮੁੱਲ;
    Inherited::ਸੈੱਟ_ਮੁੱਲ(new_ਮੁੱਲ);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(ਅਚਲ VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ਅਚਲ BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ਅਚਲ VimFont &rhs): BFont(rhs)
{
    init();
}

VimFont::~VimFont()
{
}

    ਖਾਲੀ
VimFont::init()
{
    ਅਗਲਾ = ਨਿਰਾ;
    refcount = 1;
    name = ਨਿਰਾ;
}

// ---------------- VimDialog ----------------

#ਜੇ defined(FEAT_GUI_DIALOG)

ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  k_Vim_Dialog_Button_ਸੁਨੇਹਾ = 'VMDB';
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  k_Vim_Dialog_Icon_Stripe_ਚੌੜਾਈ = 30;
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  kVimDialogButtonsSpacingX = 9;
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  kVimDialogButtonsSpacingY = 4;
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  kVimDialogSpacingX = 6;
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  kVimDialogSpacingY = 10;
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  k_Vim_Dialog_ਨਿਊਨਤਮ_ਚੌੜਾਈ  = 310;
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  k_Vim_Dialog_ਨਿਊਨਤਮ_ਉਚਾਈ = 75;
ਅਚਲ BRect	    kDefaultRect =
BRect(0, 0, k_Vim_Dialog_ਨਿਊਨਤਮ_ਚੌੜਾਈ, k_Vim_Dialog_ਨਿਊਨਤਮ_ਉਚਾਈ);

VimDialog::VimDialog(ਪੂਰਨ type, ਅਚਲ ਅੱਖਰ *title, ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ_2,
	ਅਚਲ ਅੱਖਰ *buttons, ਪੂਰਨ dfltbutton, ਅਚਲ ਅੱਖਰ *textfield, ਪੂਰਨ ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_ਖਿੜਕੀ_LOOK, B_MODAL_APP_ਖਿੜਕੀ_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_ਮੁੱਲ(dfltbutton)
    , f_ਸੁਨੇਹਾ_View(ਨਿਰਾ)
    , f_ਨਿਵੇਸ਼_Control(ਨਿਰਾ)
    , f_ਨਿਵੇਸ਼_ਮੁੱਲ(textfield)
{
    //	master view
    VimDialog::View* view = ਨਵਾਂ VimDialog::View(Bounds());
    ਜੇ (view == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (title == ਨਿਰਾ)
	ਸੈੱਟ_Title("Vim " VIM_ਸੰਸਕਰਣ_MEDIUM);

    ਜੋੜੋ_ਬੱਚਾ(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    ਦਸ਼ਮਲਵ max_Button_ਚੌੜਾਈ  = 0;
    ਦਸ਼ਮਲਵ max_Button_ਉਚਾਈ = 0;
    ਦਸ਼ਮਲਵ buttons_ਚੌੜਾਈ	  = 0;
    ਦਸ਼ਮਲਵ buttons_ਉਚਾਈ   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    ਕਰੋ
    {
	int32 ਸਿਰਾ = strButtons.ਲੱਭੋ_ਪਹਿਲਾ('\n');
	ਜੇ (ਸਿਰਾ != B_ERROR)
	    strButtons.ਸੈੱਟ_Byte_At(ਸਿਰਾ, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->ਜੋੜੋ_ਬੱਚਾ(button);
	fButtonsList.ਜੋੜੋ_ਵਸਤੂ(button);

	max_Button_ਚੌੜਾਈ	= max_c(max_Button_ਚੌੜਾਈ,  button->Bounds().ਚੌੜਾਈ_2());
	max_Button_ਉਚਾਈ = max_c(max_Button_ਉਚਾਈ, button->Bounds().ਉਚਾਈ_2());
	buttons_ਚੌੜਾਈ   += button->Bounds().ਚੌੜਾਈ_2();
	buttons_ਉਚਾਈ  += button->Bounds().ਉਚਾਈ_2();

	ਜੇ (ਸਿਰਾ == B_ERROR)
	    ਰੋਕੋ;

	strButtons.Remove(0, ਸਿਰਾ + 1);
    } ਜਦੋਂ (ਸੱਚ);

    int32 buttons_ਗਿਣਤੀ = fButtonsList.ਗਿਣਤੀ_ਚੀਜ਼ਾਂ();
    buttons_ਚੌੜਾਈ      += kVimDialogButtonsSpacingX * (buttons_ਗਿਣਤੀ - 1);
    buttons_ਉਚਾਈ     += kVimDialogButtonsSpacingY * (buttons_ਗਿਣਤੀ - 1);
    ਦਸ਼ਮਲਵ dialog_ਚੌੜਾਈ  = buttons_ਚੌੜਾਈ + k_Vim_Dialog_Icon_Stripe_ਚੌੜਾਈ +
	kVimDialogSpacingX * 2;
    ਦਸ਼ਮਲਵ dialog_ਉਚਾਈ = max_Button_ਉਚਾਈ + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    ਤਾਰਕਿਕ vertical = (vim_strchr(p_go, GO_VERTICAL) != ਨਿਰਾ) ||
	dialog_ਚੌੜਾਈ >= gui.vim_ਖਿੜਕੀ->Bounds().ਚੌੜਾਈ_2();
    ਜੇ (vertical) {
	dialog_ਚੌੜਾਈ  -= buttons_ਚੌੜਾਈ;
	dialog_ਚੌੜਾਈ  += max_Button_ਚੌੜਾਈ;
	dialog_ਉਚਾਈ -= max_Button_ਉਚਾਈ;
	dialog_ਉਚਾਈ += buttons_ਉਚਾਈ;
    }

    dialog_ਚੌੜਾਈ  = max_c(dialog_ਚੌੜਾਈ,  k_Vim_Dialog_ਨਿਊਨਤਮ_ਚੌੜਾਈ);

    //	message view
    BRect rect(0, 0, dialog_ਚੌੜਾਈ, 0);
    rect.ਖੱਬੇ  += k_Vim_Dialog_Icon_Stripe_ਚੌੜਾਈ + 16 + kVimDialogSpacingX;
    rect.ਸਿਖਰ   += kVimDialogSpacingY;
    rect.ਸੱਜੇ -= kVimDialogSpacingX;
    rect.bottom = rect.ਸਿਖਰ;
    f_ਸੁਨੇਹਾ_View = ਨਵਾਂ BTextView(rect, "_tv_", rect.ਵਿੱਥ_By_Copy(B_ORIGIN),
	    B_FOLLOW_ਖੱਬਾ | B_FOLLOW_ਸਿਖਰ, B_WILL_DRAW);

    f_ਸੁਨੇਹਾ_View->ਸੈੱਟ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color ਪਾਠ_Color = ui_color(B_PANEL_ਪਾਠ_COLOR);
    f_ਸੁਨੇਹਾ_View->ਸੈੱਟ_Font_And_Color(be_plain_font, B_FONT_ALL, &ਪਾਠ_Color);
    f_ਸੁਨੇਹਾ_View->ਸੈੱਟ_ਪਾਠ(ਸੁਨੇਹਾ_2);
    f_ਸੁਨੇਹਾ_View->MakeEditable(ਝੂਠ);
    f_ਸੁਨੇਹਾ_View->MakeSelectable(ਝੂਠ);
    f_ਸੁਨੇਹਾ_View->ਸੈੱਟ_ਸ਼ਬਦ_Wrap(ਸੱਚ);
    ਜੋੜੋ_ਬੱਚਾ(f_ਸੁਨੇਹਾ_View);

    ਦਸ਼ਮਲਵ ਸੁਨੇਹਾ_ਉਚਾਈ = f_ਸੁਨੇਹਾ_View->ਪਾਠ_ਉਚਾਈ(0, f_ਸੁਨੇਹਾ_View->ਗਿਣਤੀ_ਸਤਰਾਂ());
    f_ਸੁਨੇਹਾ_View->ResizeBy(0, ਸੁਨੇਹਾ_ਉਚਾਈ);
    f_ਸੁਨੇਹਾ_View->ਸੈੱਟ_ਪਾਠ_Rect(BRect(0, 0, rect.ਚੌੜਾਈ_2(), ਸੁਨੇਹਾ_ਉਚਾਈ));

    dialog_ਉਚਾਈ += ਸੁਨੇਹਾ_ਉਚਾਈ;

    //	input view
    ਜੇ (f_ਨਿਵੇਸ਼_ਮੁੱਲ != ਨਿਰਾ) {
	rect.ਸਿਖਰ     =
	    rect.bottom += ਸੁਨੇਹਾ_ਉਚਾਈ + kVimDialogSpacingY;
	f_ਨਿਵੇਸ਼_Control = ਨਵਾਂ BTextControl(rect, "_iv_", ਨਿਰਾ, f_ਨਿਵੇਸ਼_ਮੁੱਲ, ਨਿਰਾ,
		B_FOLLOW_ਖੱਬਾ | B_FOLLOW_ਸਿਖਰ, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_ਨਿਵੇਸ਼_Control->ਪਾਠ_View()->ਸੈੱਟ_ਪਾਠ(f_ਨਿਵੇਸ਼_ਮੁੱਲ);
	f_ਨਿਵੇਸ਼_Control->ਪਾਠ_View()->ਸੈੱਟ_ਸ਼ਬਦ_Wrap(ਝੂਠ);
	ਜੋੜੋ_ਬੱਚਾ(f_ਨਿਵੇਸ਼_Control);

	ਦਸ਼ਮਲਵ ਚੌੜਾਈ = 0.f, ਉਚਾਈ = 0.f;
	f_ਨਿਵੇਸ਼_Control->Get_Preferred_ਮਾਪ(&ਚੌੜਾਈ, &ਉਚਾਈ);
	f_ਨਿਵੇਸ਼_Control->MakeFocus(ਸੱਚ);

	dialog_ਉਚਾਈ += ਉਚਾਈ + kVimDialogSpacingY * 1.5;
    }

    dialog_ਉਚਾਈ = max_c(dialog_ਉਚਾਈ, k_Vim_Dialog_ਨਿਊਨਤਮ_ਉਚਾਈ);

    Resize_ਮੰਜ਼ਿਲ(dialog_ਚੌੜਾਈ, dialog_ਉਚਾਈ);
    Move_ਮੰਜ਼ਿਲ((gui.vim_ਖਿੜਕੀ->Bounds().ਚੌੜਾਈ_2() - dialog_ਚੌੜਾਈ) / 2,
	    (gui.vim_ਖਿੜਕੀ->Bounds().ਉਚਾਈ_2() - dialog_ਉਚਾਈ) / 2);

    //	adjust layout of buttons
    ਦਸ਼ਮਲਵ button_ਚੌੜਾਈ = max_c(max_Button_ਚੌੜਾਈ, rect.ਚੌੜਾਈ_2() * 0.66);
    BPoint origin(dialog_ਚੌੜਾਈ, dialog_ਉਚਾਈ);
    origin.x -= kVimDialogSpacingX + (vertical ? button_ਚੌੜਾਈ : buttons_ਚੌੜਾਈ);
    origin.y -= kVimDialogSpacingY + (vertical ? buttons_ਉਚਾਈ	: max_Button_ਉਚਾਈ);

    ਲਈ (int32 i = 0 ; i < buttons_ਗਿਣਤੀ; i++) {
	BButton *button = (BButton*)fButtonsList.ਵਸਤੂ_At(i);
	button->Move_ਮੰਜ਼ਿਲ(origin);
	ਜੇ (vertical) {
	    origin.y += button->ਚੌਖਟਾ_2().ਉਚਾਈ_2() + kVimDialogButtonsSpacingY;
	    button->Resize_ਮੰਜ਼ਿਲ(button_ਚੌੜਾਈ, button->ਚੌਖਟਾ_2().ਉਚਾਈ_2());
	} ਨਹੀਂਤਾਂ
	    origin.x += button->ਚੌਖਟਾ_2().ਚੌੜਾਈ_2() + kVimDialogButtonsSpacingX;

	ਜੇ (dfltbutton == i + 1) {
	    button->MakeDefault(ਸੱਚ);
	    button->MakeFocus(f_ਨਿਵੇਸ਼_Control == ਨਿਰਾ);
	}
    }
}

VimDialog::~VimDialog()
{
    ਜੇ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    ਪੂਰਨ
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    ਜੇ (fDialogSem < B_OK) {
	Quit();
	ਵਾਪਸ f_Dialog_ਮੁੱਲ;
    }

    Show();

    ਜਦੋਂ (acquire_sem(fDialogSem) == B_INTERRUPTED);

    ਪੂਰਨ ret_ਮੁੱਲ = f_Dialog_ਮੁੱਲ;
    ਜੇ (f_ਨਿਵੇਸ਼_ਮੁੱਲ != ਨਿਰਾ)
	vim_strncpy((char_u*)f_ਨਿਵੇਸ਼_ਮੁੱਲ, (char_u*)f_ਨਿਵੇਸ਼_Control->ਪਾਠ_2(), IOSIZE - 1);

    ਜੇ (ਤਾਲਾ_2())
	Quit();

    ਵਾਪਸ ret_ਮੁੱਲ;
}

ਖਾਲੀ VimDialog::ਸੁਨੇਹਾ_Received(BMessage *ਸੁਨੇਹਾ)
{
    int32 which = 0;
    ਜੇ (ਸੁਨੇਹਾ->what != k_Vim_Dialog_Button_ਸੁਨੇਹਾ ||
	    ਸੁਨੇਹਾ->ਲੱਭੋ_Int32("which", &which) != B_OK)
	ਵਾਪਸ BWindow::ਸੁਨੇਹਾ_Received(ਸੁਨੇਹਾ);

    f_Dialog_ਮੁੱਲ = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, ਅਚਲ ਅੱਖਰ* label)
{
    BMessage *ਸੁਨੇਹਾ_2 = ਨਵਾਂ BMessage(k_Vim_Dialog_Button_ਸੁਨੇਹਾ);
    ਸੁਨੇਹਾ_2->ਜੋੜੋ_Int32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = ਨਵਾਂ BButton(rect, name.String(), label, ਸੁਨੇਹਾ_2,
	    B_FOLLOW_ਸੱਜਾ | B_FOLLOW_BOTTOM);

    ਦਸ਼ਮਲਵ ਚੌੜਾਈ = 0.f, ਉਚਾਈ = 0.f;
    button->Get_Preferred_ਮਾਪ(&ਚੌੜਾਈ, &ਉਚਾਈ);
    button->Resize_ਮੰਜ਼ਿਲ(ਚੌੜਾਈ, ਉਚਾਈ);

    ਵਾਪਸ button;
}

VimDialog::View::View(BRect ਚੌਖਟਾ)
    :	BView(ਚੌਖਟਾ, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(ਨਿਰਾ)
{
    ਸੈੱਟ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    ਮਿਟਾਓ fIconBitmap;
}

ਖਾਲੀ VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.ਸੱਜੇ = k_Vim_Dialog_Icon_Stripe_ਚੌੜਾਈ;
    ਸੈੱਟ_ਉੱਚਾ_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    ਜੇ (fIconBitmap == ਨਿਰਾ)
	ਵਾਪਸ;

    ਸੈੱਟ_Drawing_ਬਹੁਲਕ(B_OP_ALPHA);
    ਸੈੱਟ_Blending_ਬਹੁਲਕ(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

ਖਾਲੀ VimDialog::View::InitIcon(int32 type)
{
    ਜੇ (type == VIM_GENERIC)
	ਵਾਪਸ;

    BPath ਰਸਤਾ;
    ਸਥਿਤੀ_t ਸਥਿਤੀ = ਲੱਭੋ_directory(B_BEOS_SERVERS_DIRECTORY, &ਰਸਤਾ);
    ਜੇ (ਸਥਿਤੀ != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(ਸਥਿਤੀ));
	ਵਾਪਸ;
    }

    ਰਸਤਾ.Append("app_server");

    BFile file(ਰਸਤਾ.ਰਸਤਾ_2(), O_RDONLY);
    ਜੇ (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	ਵਾਪਸ;
    }

    BResources resources(&file);
    ਜੇ (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	ਵਾਪਸ;
    }

    ਅਚਲ ਅੱਖਰ *name = "";
    ਚੋਣ(type) {
	ਹਾਲਤ VIM_ERROR:	    name = "stop"; ਰੋਕੋ;
	ਹਾਲਤ VIM_WARNING:   name = "warn"; ਰੋਕੋ;
	ਹਾਲਤ VIM_INFO:	    name = "info"; ਰੋਕੋ;
	ਹਾਲਤ VIM_QUESTION:  name = "idea"; ਰੋਕੋ;
	ਮੂਲ: ਵਾਪਸ;
    }

    int32 icon_ਮਾਪ_2 = 32;
    fIconBitmap = ਨਵਾਂ BBitmap(BRect(0, 0, icon_ਮਾਪ_2 - 1, icon_ਮਾਪ_2 - 1), 0, B_RGBA32);
    ਜੇ (fIconBitmap == ਨਿਰਾ || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == ਨਿਰਾ) ? "null" : strerror(fIconBitmap->InitCheck()));
	ਵਾਪਸ;
    }

    size_t ਮਾਪ = 0;
    ਅਚਲ uint8* icon_ਅੰਕੜੇ = ਨਿਰਾ;
    //	try vector icon first?
    icon_ਅੰਕੜੇ = (ਅਚਲ uint8*)resources.ਲੱਦੋ_Resource(B_ਸਦਿਸ਼_ICON_TYPE, name, &ਮਾਪ);
    ਜੇ (icon_ਅੰਕੜੇ != ਨਿਰਾ && BIconUtils::Get_ਸਦਿਸ਼_Icon(icon_ਅੰਕੜੇ, ਮਾਪ, fIconBitmap) == B_OK)
	ਵਾਪਸ;

    //	try bitmap icon now
    icon_ਅੰਕੜੇ = (ਅਚਲ uint8*)resources.ਲੱਦੋ_Resource(B_LARGE_ICON_TYPE, name, &ਮਾਪ);
    ਜੇ (icon_ਅੰਕੜੇ == ਨਿਰਾ) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	ਮਿਟਾਓ fIconBitmap;
	fIconBitmap = ਨਿਰਾ;
	ਵਾਪਸ;
    }

    ਜੇ (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(icon_ਅੰਕੜੇ, icon_ਮਾਪ_2, icon_ਮਾਪ_2, icon_ਮਾਪ_2, fIconBitmap);
}

ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  k_Vim_Dialog_OKButton_ਸੁਨੇਹਾ = 'FDOK';
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  k_Vim_Dialog_Cancel_Button_ਸੁਨੇਹਾ = 'FDCN';
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  k_Vim_Dialog_ਮਾਪ_ਨਿਵੇਸ਼_ਸੁਨੇਹਾ = 'SICH';
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  k_Vim_Dialog_Family_Select_ਸੁਨੇਹਾ = 'MSFM';
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  k_Vim_Dialog_Style_Select_ਸੁਨੇਹਾ = 'MSST';
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  k_Vim_Dialog_ਮਾਪ_Select_ਸੁਨੇਹਾ = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, ਦਸ਼ਮਲਵ* ਮਾਪ)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_ਖਿੜਕੀ_LOOK, B_MODAL_APP_ਖਿੜਕੀ_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , f_ਸਥਿਤੀ(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_ਮੁੱਲ(ਝੂਠ)
    , fFamily(family)
    , fStyle(style)
    , f_ਮਾਪ(ਮਾਪ)
    , f_Font_ਮਾਪ(*ਮਾਪ)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_ਨਿਵੇਸ਼(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_ਲੰਮਾਈ);
    strncpy(fFontStyle, *style, B_FONT_STYLE_ਲੰਮਾਈ);

    //	"client" area view
    BBox *clientBox = ਨਵਾਂ BBox(Bounds(), B_ਖਾਲੀ_ਥਾਵਾਂ_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_ਚੌਖਟਾ_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    ਜੋੜੋ_ਬੱਚਾ(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.ਖੱਬਾ_ਸਿਖਰ(), RC.ਖੱਬਾ_ਸਿਖਰ());

    //	at first create all controls
    fPreview = ਨਵਾਂ BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->ਜੋੜੋ_ਬੱਚਾ(fPreview);

    BBox* boxDivider = ਨਵਾਂ BBox(rc, B_ਖਾਲੀ_ਥਾਵਾਂ_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->ਜੋੜੋ_ਬੱਚਾ(boxDivider);

    BStringView *labelFamily = ਨਵਾਂ BStringView(rc, "labelFamily", "Family:");
    clientBox->ਜੋੜੋ_ਬੱਚਾ(labelFamily);
    labelFamily->Resize_ਮੰਜ਼ਿਲ_Preferred();

    BStringView *labelStyle = ਨਵਾਂ BStringView(rc, "labelStyle", "Style:");
    clientBox->ਜੋੜੋ_ਬੱਚਾ(labelStyle);
    labelStyle->Resize_ਮੰਜ਼ਿਲ_Preferred();

    BStringView *label_ਮਾਪ = ਨਵਾਂ BStringView(rc, "labelSize", "Size:");
    clientBox->ਜੋੜੋ_ਬੱਚਾ(label_ਮਾਪ);
    label_ਮਾਪ->Resize_ਮੰਜ਼ਿਲ_Preferred();

    fFamiliesList = ਨਵਾਂ BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = ਨਵਾਂ BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_ਖੱਬਾ_ਸੱਜਾ, 0, ਝੂਠ, ਸੱਚ);
    clientBox->ਜੋੜੋ_ਬੱਚਾ(scrollFamilies);

    fStylesList= ਨਵਾਂ BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = ਨਵਾਂ BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_ਖੱਬਾ_ਸੱਜਾ, 0, ਝੂਠ, ਸੱਚ);
    clientBox->ਜੋੜੋ_ਬੱਚਾ(scrollStyles);

    f_Sizes_ਨਿਵੇਸ਼ = ਨਵਾਂ BTextControl(rc, "inputSize", ਨਿਰਾ, "???",
	    ਨਵਾਂ BMessage(k_Vim_Dialog_ਮਾਪ_ਨਿਵੇਸ਼_ਸੁਨੇਹਾ));
    clientBox->ਜੋੜੋ_ਬੱਚਾ(f_Sizes_ਨਿਵੇਸ਼);
    f_Sizes_ਨਿਵੇਸ਼->Resize_ਮੰਜ਼ਿਲ_Preferred();

    fSizesList = ਨਵਾਂ BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = ਨਵਾਂ BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_ਖੱਬਾ_ਸੱਜਾ, 0, ਝੂਠ, ਸੱਚ);
    clientBox->ਜੋੜੋ_ਬੱਚਾ(scrollSizes);

    BButton *buttonOK = ਨਵਾਂ BButton(rc, "buttonOK", "OK",
			ਨਵਾਂ BMessage(k_Vim_Dialog_OKButton_ਸੁਨੇਹਾ));
    clientBox->ਜੋੜੋ_ਬੱਚਾ(buttonOK);
    buttonOK->Resize_ਮੰਜ਼ਿਲ_Preferred();

    BButton *buttonCancel = ਨਵਾਂ BButton(rc, "buttonCancel", "Cancel",
			ਨਵਾਂ BMessage(k_Vim_Dialog_Cancel_Button_ਸੁਨੇਹਾ));
    clientBox->ਜੋੜੋ_ਬੱਚਾ(buttonCancel);
    buttonCancel->Resize_ਮੰਜ਼ਿਲ_Preferred();

    //	layout controls
    ਦਸ਼ਮਲਵ ਸਤਰ_ਉਚਾਈ = labelFamily->Bounds().ਉਚਾਈ_2();
    ਦਸ਼ਮਲਵ preview_ਉਚਾਈ = ਸਤਰ_ਉਚਾਈ * 3;
    ਦਸ਼ਮਲਵ ਵਿੱਥ_YLabels = preview_ਉਚਾਈ + kVimDialogSpacingY;
    ਦਸ਼ਮਲਵ ਵਿੱਥ_YLists = ਵਿੱਥ_YLabels + ਸਤਰ_ਉਚਾਈ + kVimDialogSpacingY / 2;
    ਦਸ਼ਮਲਵ ਵਿੱਥ_YSizes = ਵਿੱਥ_YLists + f_Sizes_ਨਿਵੇਸ਼->Bounds().ਉਚਾਈ_2() + kVimDialogSpacingY / 2;
    ਦਸ਼ਮਲਵ lists_ਉਚਾਈ = ਸਤਰ_ਉਚਾਈ * 9;
    ਦਸ਼ਮਲਵ ਵਿੱਥ_YButtons = ਵਿੱਥ_YLists + lists_ਉਚਾਈ +  kVimDialogSpacingY;
    ਦਸ਼ਮਲਵ max_Controls_ਉਚਾਈ = ਵਿੱਥ_YButtons + buttonOK->Bounds().ਉਚਾਈ_2();
    ਦਸ਼ਮਲਵ families_ਚੌੜਾਈ = labelFamily->Bounds().ਚੌੜਾਈ_2() * 5;
    ਦਸ਼ਮਲਵ ਵਿੱਥ_XStyles = families_ਚੌੜਾਈ + kVimDialogSpacingX;
    ਦਸ਼ਮਲਵ styles_ਚੌੜਾਈ = labelStyle->Bounds().ਚੌੜਾਈ_2() * 4;
    ਦਸ਼ਮਲਵ ਵਿੱਥ_XSizes = ਵਿੱਥ_XStyles + styles_ਚੌੜਾਈ + kVimDialogSpacingX;
    ਦਸ਼ਮਲਵ sizes_ਚੌੜਾਈ = label_ਮਾਪ->Bounds().ਚੌੜਾਈ_2() * 2;
    ਦਸ਼ਮਲਵ max_Controls_ਚੌੜਾਈ = ਵਿੱਥ_XSizes + sizes_ਚੌੜਾਈ;

    Resize_ਮੰਜ਼ਿਲ(max_Controls_ਚੌੜਾਈ + kVimDialogSpacingX * 2,
	max_Controls_ਉਚਾਈ + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vim_ਖਿੜਕੀ->ਚੌਖਟਾ_2();
    Move_ਮੰਜ਼ਿਲ(rcVim.ਖੱਬੇ + (rcVim.ਚੌੜਾਈ_2() - ਚੌਖਟਾ_2().ਚੌੜਾਈ_2()) / 2,
	    rcVim.ਸਿਖਰ + (rcVim.ਉਚਾਈ_2() - ਚੌਖਟਾ_2().ਉਚਾਈ_2()) / 2);

    fPreview->Resize_ਮੰਜ਼ਿਲ(max_Controls_ਚੌੜਾਈ, preview_ਉਚਾਈ);
    fPreview->ਸੈੱਟ_Alignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, preview_ਉਚਾਈ + kVimDialogSpacingY / 2);
    boxDivider->Resize_ਮੰਜ਼ਿਲ(max_Controls_ਚੌੜਾਈ, 1.f);

    labelFamily->MoveBy(0.f, ਵਿੱਥ_YLabels);
    labelStyle->MoveBy(ਵਿੱਥ_XStyles, ਵਿੱਥ_YLabels);
    label_ਮਾਪ->MoveBy(ਵਿੱਥ_XSizes, ਵਿੱਥ_YLabels);

    //	text control alignment issues
    ਦਸ਼ਮਲਵ insetX = f_Sizes_ਨਿਵੇਸ਼->ਪਾਠ_View()->Bounds().ਚੌੜਾਈ_2() - f_Sizes_ਨਿਵੇਸ਼->Bounds().ਚੌੜਾਈ_2();
    ਦਸ਼ਮਲਵ insetY = f_Sizes_ਨਿਵੇਸ਼->ਪਾਠ_View()->Bounds().ਚੌੜਾਈ_2() - f_Sizes_ਨਿਵੇਸ਼->Bounds().ਚੌੜਾਈ_2();

    scrollFamilies->MoveBy(0.f, ਵਿੱਥ_YLists);
    scrollStyles->MoveBy(ਵਿੱਥ_XStyles, ਵਿੱਥ_YLists);
    f_Sizes_ਨਿਵੇਸ਼->MoveBy(ਵਿੱਥ_XSizes + insetX / 2, ਵਿੱਥ_YLists + insetY / 2);
    scrollSizes->MoveBy(ਵਿੱਥ_XSizes, ਵਿੱਥ_YSizes);

    f_Sizes_ਨਿਵੇਸ਼->ਸੈੱਟ_Alignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->Resize_ਮੰਜ਼ਿਲ(families_ਚੌੜਾਈ, lists_ਉਚਾਈ);
    scrollStyles->Resize_ਮੰਜ਼ਿਲ(styles_ਚੌੜਾਈ, lists_ਉਚਾਈ);
    f_Sizes_ਨਿਵੇਸ਼->Resize_ਮੰਜ਼ਿਲ(sizes_ਚੌੜਾਈ, f_Sizes_ਨਿਵੇਸ਼->Bounds().ਉਚਾਈ_2());
    scrollSizes->Resize_ਮੰਜ਼ਿਲ(sizes_ਚੌੜਾਈ,
	    lists_ਉਚਾਈ - (ਵਿੱਥ_YSizes - ਵਿੱਥ_YLists));

    buttonOK->MoveBy(max_Controls_ਚੌੜਾਈ - buttonOK->Bounds().ਚੌੜਾਈ_2(), ਵਿੱਥ_YButtons);
    buttonCancel->MoveBy(max_Controls_ਚੌੜਾਈ - buttonOK->Bounds().ਚੌੜਾਈ_2()
	    - buttonCancel->Bounds().ਚੌੜਾਈ_2() - kVimDialogSpacingX, ਵਿੱਥ_YButtons);

    //	fill lists
    ਪੂਰਨ sel_ਸੂਚਕਾਂਕ = -1;
    ਪੂਰਨ ਗਿਣਤੀ_2 = ਗਿਣਤੀ_font_families();
    ਲਈ (ਪੂਰਨ i = 0; i < ਗਿਣਤੀ_2; i++) {
	font_family family;
	ਜੇ (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->ਜੋੜੋ_ਵਸਤੂ(ਨਵਾਂ BString_ਵਸਤੂ((ਅਚਲ ਅੱਖਰ*)family));
	    ਜੇ (ਲੜੀ_ਤੁਲਨਾ(family, fFontFamily, B_FONT_FAMILY_ਲੰਮਾਈ) == 0)
		sel_ਸੂਚਕਾਂਕ = i;
	}
    }

    ਜੇ (sel_ਸੂਚਕਾਂਕ >= 0) {
	fFamiliesList->Select(sel_ਸੂਚਕਾਂਕ);
	fFamiliesList->Scroll_ਮੰਜ਼ਿਲ_Selection();
    }

    _UpdateFontStyles();

    sel_ਸੂਚਕਾਂਕ = -1;
    ਲਈ (ਪੂਰਨ ਮਾਪ = 8, ਸੂਚਕਾਂਕ = 0; ਮਾਪ <= 18; ਮਾਪ++, ਸੂਚਕਾਂਕ++) {
	BString str;
	str << ਮਾਪ;
	fSizesList->ਜੋੜੋ_ਵਸਤੂ(ਨਵਾਂ BString_ਵਸਤੂ(str));
	ਜੇ (ਮਾਪ == f_Font_ਮਾਪ)
	    sel_ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;

    }

    ਜੇ (sel_ਸੂਚਕਾਂਕ >= 0) {
	fSizesList->Select(sel_ਸੂਚਕਾਂਕ);
	fSizesList->Scroll_ਮੰਜ਼ਿਲ_Selection();
    }

    fFamiliesList->ਸੈੱਟ_Selection_ਸੁਨੇਹਾ(ਨਵਾਂ BMessage(k_Vim_Dialog_Family_Select_ਸੁਨੇਹਾ));
    fStylesList->ਸੈੱਟ_Selection_ਸੁਨੇਹਾ(ਨਵਾਂ BMessage(k_Vim_Dialog_Style_Select_ਸੁਨੇਹਾ));
    fSizesList->ਸੈੱਟ_Selection_ਸੁਨੇਹਾ(ਨਵਾਂ BMessage(k_Vim_Dialog_ਮਾਪ_Select_ਸੁਨੇਹਾ));
    f_Sizes_ਨਿਵੇਸ਼->ਸੈੱਟ_Modification_ਸੁਨੇਹਾ(ਨਵਾਂ BMessage(k_Vim_Dialog_ਮਾਪ_ਨਿਵੇਸ਼_ਸੁਨੇਹਾ));

    Update_ਮਾਪ_ਨਿਵੇਸ਼_Preview();
    _UpdateFontPreview();

    f_ਸਥਿਤੀ = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    ਜੇ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    ਖਾਲੀ
VimSelectFontDialog::_CleanList(BListView* list)
{
    ਜਦੋਂ (0 < list->ਗਿਣਤੀ_ਚੀਜ਼ਾਂ())
	ਮਿਟਾਓ (dynamic_cast<BString_ਵਸਤੂ*>(list->Remove_ਵਸਤੂ((int32)0)));
}

    ਤਾਰਕਿਕ
VimSelectFontDialog::Go()
{
    ਜੇ (f_ਸਥਿਤੀ != B_OK) {
	Quit();
	ਵਾਪਸ NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    ਜੇ (fDialogSem < B_OK) {
	Quit();
	ਵਾਪਸ f_Dialog_ਮੁੱਲ;
    }

    Show();

    ਜਦੋਂ (acquire_sem(fDialogSem) == B_INTERRUPTED);

    ਤਾਰਕਿਕ ret_ਮੁੱਲ = f_Dialog_ਮੁੱਲ;

    ਜੇ (ਤਾਲਾ_2())
	Quit();

    ਵਾਪਸ ret_ਮੁੱਲ;
}


ਖਾਲੀ VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 sel_ਸੂਚਕਾਂਕ = -1;
    int32 ਗਿਣਤੀ_2 = ਗਿਣਤੀ_font_styles(fFontFamily);
    ਲਈ (int32 i = 0; i < ਗਿਣਤੀ_2; i++) {
	font_style style;
	uint32 flags = 0;
	ਜੇ (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->ਜੋੜੋ_ਵਸਤੂ(ਨਵਾਂ BString_ਵਸਤੂ((ਅਚਲ ਅੱਖਰ*)style));
	    ਜੇ (ਲੜੀ_ਤੁਲਨਾ(style, fFontStyle, B_FONT_STYLE_ਲੰਮਾਈ) == 0)
		sel_ਸੂਚਕਾਂਕ = i;
	}
    }

    ਜੇ (sel_ਸੂਚਕਾਂਕ >= 0) {
	fStylesList->Select(sel_ਸੂਚਕਾਂਕ);
	fStylesList->Scroll_ਮੰਜ਼ਿਲ_Selection();
    } ਨਹੀਂਤਾਂ
	fStylesList->Select(0);
}


ਖਾਲੀ VimSelectFontDialog::Update_ਮਾਪ_ਨਿਵੇਸ਼_Preview()
{
    ਅੱਖਰ buf[10] = {0};
    vim_snprintf(buf, ਆਕਾਰ(buf), (ਅੱਖਰ*)"%.0f", f_Font_ਮਾਪ);
    f_Sizes_ਨਿਵੇਸ਼->ਸੈੱਟ_ਪਾਠ(buf);
}


ਖਾਲੀ VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.ਸੈੱਟ_ਮਾਪ(f_Font_ਮਾਪ);
    font.ਸੈੱਟ_Family_And_Style(fFontFamily, fFontStyle);
    fPreview->ਸੈੱਟ_Font(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_ਮਾਪ);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (ਪੂਰਨ)f_Font_ਮਾਪ << " pt.";
    fPreview->ਸੈੱਟ_ਪਾਠ(str);
}


    ਤਾਰਕਿਕ
VimSelectFontDialog::Update_From_List_ਵਸਤੂ(BListView* list, ਅੱਖਰ* ਪਾਠ, ਪੂਰਨ ਪਾਠ_ਮਾਪ)
{
    int32 ਸੂਚਕਾਂਕ = list->CurrentSelection();
    ਜੇ (ਸੂਚਕਾਂਕ < 0)
	ਵਾਪਸ ਝੂਠ;
    BString_ਵਸਤੂ* ਵਸਤੂ = (BString_ਵਸਤੂ*)list->ਵਸਤੂ_At(ਸੂਚਕਾਂਕ);
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ ਝੂਠ;
    strncpy(ਪਾਠ, ਵਸਤੂ->ਪਾਠ_2(), ਪਾਠ_ਮਾਪ);
    ਵਾਪਸ ਸੱਚ;
}


ਖਾਲੀ VimSelectFontDialog::ਸੁਨੇਹਾ_Received(BMessage *ਸੁਨੇਹਾ)
{
    ਚੋਣ (ਸੁਨੇਹਾ->what) {
	ਹਾਲਤ k_Vim_Dialog_OKButton_ਸੁਨੇਹਾ:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_ਲੰਮਾਈ);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_ਲੰਮਾਈ);
	    *f_ਮਾਪ = f_Font_ਮਾਪ;
	    f_Dialog_ਮੁੱਲ = ਸੱਚ;
	ਹਾਲਤ k_Vim_Dialog_Cancel_Button_ਸੁਨੇਹਾ:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    ਵਾਪਸ;
	ਹਾਲਤ B_ਕੁੰਜੀ_ਉੱਪਰ:
	    {
		int32 ਕੁੰਜੀ = 0;
		ਜੇ (ਸੁਨੇਹਾ->ਲੱਭੋ_Int32("raw_char", &ਕੁੰਜੀ) == B_OK
			&& ਕੁੰਜੀ == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ k_Vim_Dialog_Family_Select_ਸੁਨੇਹਾ:
	    ਜੇ (Update_From_List_ਵਸਤੂ(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_ਲੰਮਾਈ)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ k_Vim_Dialog_Style_Select_ਸੁਨੇਹਾ:
	    ਜੇ (Update_From_List_ਵਸਤੂ(fStylesList,
		    fFontStyle, B_FONT_STYLE_ਲੰਮਾਈ))
		_UpdateFontPreview();
	    ਰੋਕੋ;
	ਹਾਲਤ k_Vim_Dialog_ਮਾਪ_Select_ਸੁਨੇਹਾ:
	    {
		ਅੱਖਰ buf[10] = {0};
		ਜੇ (Update_From_List_ਵਸਤੂ(fSizesList, buf, ਆਕਾਰ(buf))) {
		    ਦਸ਼ਮਲਵ ਮਾਪ = atof(buf);
		    ਜੇ (ਮਾਪ > 0.f) {
			f_Font_ਮਾਪ = ਮਾਪ;
			Update_ਮਾਪ_ਨਿਵੇਸ਼_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ k_Vim_Dialog_ਮਾਪ_ਨਿਵੇਸ਼_ਸੁਨੇਹਾ:
	    {
		ਦਸ਼ਮਲਵ ਮਾਪ = atof(f_Sizes_ਨਿਵੇਸ਼->ਪਾਠ_2());
		ਜੇ (ਮਾਪ > 0.f) {
		    f_Font_ਮਾਪ = ਮਾਪ;
		    _UpdateFontPreview();
		}
	    }
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਰੋਕੋ;
    }
    ਵਾਪਸ BWindow::ਸੁਨੇਹਾ_Received(ਸੁਨੇਹਾ);
}

#ਜੇ_ਅੰਤ // FEAT_GUI_DIALOG

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
ਸਥਿਰ BMessage * Menu_ਸੁਨੇਹਾ(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect ਚੌਖਟਾ, ਅਚਲ ਅੱਖਰ *name) :
    BBox(ਚੌਖਟਾ, name, B_FOLLOW_ਖੱਬਾ | B_FOLLOW_ਸਿਖਰ, B_WILL_DRAW | B_ਚੌਖਟਾ_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 ਗਿਣਤੀ_2 = fButtonsList.ਗਿਣਤੀ_ਚੀਜ਼ਾਂ();
    ਲਈ (int32 i = 0; i < ਗਿਣਤੀ_2; i++)
	ਮਿਟਾਓ (BPictureButton*)fButtonsList.ਵਸਤੂ_At(i);
    fButtonsList.Make_ਖਾਲੀ_ਥਾਵਾਂ();

    ਮਿਟਾਓ ਸਾਧਾਰਨ_Buttons_Bitmap;
    ਮਿਟਾਓ grayedButtonsBitmap;
    ਸਾਧਾਰਨ_Buttons_Bitmap    = ਨਿਰਾ;
    grayedButtonsBitmap  = ਨਿਰਾ;
}

    ਖਾਲੀ
VimToolbar::Attached_ਮੰਜ਼ਿਲ_ਖਿੜਕੀ()
{
    BBox::Attached_ਮੰਜ਼ਿਲ_ਖਿੜਕੀ();

    ਸੈੱਟ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    ਦਸ਼ਮਲਵ
VimToolbar::Toolbar_ਉਚਾਈ() ਅਚਲ
{
    ਦਸ਼ਮਲਵ ਮਾਪ = ਨਿਰਾ == ਸਾਧਾਰਨ_Buttons_Bitmap ? 18. : ਸਾਧਾਰਨ_Buttons_Bitmap->Bounds().ਉਚਾਈ_2();
    ਵਾਪਸ ਮਾਪ + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    ਤਾਰਕਿਕ
VimToolbar::Modify_Bitmap_ਮੰਜ਼ਿਲ_Grayed(BBitmap *bitmap)
{
    ਦਸ਼ਮਲਵ ਉਚਾਈ = bitmap->Bounds().ਉਚਾਈ_2();
    ਦਸ਼ਮਲਵ ਚੌੜਾਈ  = bitmap->Bounds().ਚੌੜਾਈ_2();

    rgb_color *ਬਿੱਟ = (rgb_color*)bitmap->ਬਿੱਟ_2();
    int32 pixels = bitmap->ਬਿੱਟ_ਲੰਮਾਈ() / 4;
    ਲਈ (int32 i = 0; i < pixels; i++) {
	ਬਿੱਟ[i].red = ਬਿੱਟ[i].green =
	ਬਿੱਟ[i].blue = ((uint32)ਬਿੱਟ[i].red + ਬਿੱਟ[i].green + ਬਿੱਟ[i].blue) / 3;
	ਬਿੱਟ[i].alpha /= 4;
    }

    ਵਾਪਸ ਸੱਚ;
}

    ਤਾਰਕਿਕ
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    ਸਾਧਾਰਨ_Buttons_Bitmap = ਲੱਦੋ_Vim_Bitmap("builtin-tools.png");
    ਜੇ (ਸਾਧਾਰਨ_Buttons_Bitmap == ਨਿਰਾ)
	//  customized not found? dig application resources for "builtin-tools" one
	ਸਾਧਾਰਨ_Buttons_Bitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    ਜੇ (ਸਾਧਾਰਨ_Buttons_Bitmap == ਨਿਰਾ)
	ਵਾਪਸ ਝੂਠ;

    BMessage archive;
    ਸਾਧਾਰਨ_Buttons_Bitmap->Archive(&archive);

    grayedButtonsBitmap = ਨਵਾਂ BBitmap(&archive);
    ਜੇ (grayedButtonsBitmap == ਨਿਰਾ)
	ਵਾਪਸ ਝੂਠ;

    //	modify grayed bitmap
    Modify_Bitmap_ਮੰਜ਼ਿਲ_Grayed(grayedButtonsBitmap);

    ਵਾਪਸ ਸੱਚ;
}

BBitmap *VimToolbar::ਲੱਦੋ_Vim_Bitmap(ਅਚਲ ਅੱਖਰ* fileName)
{
    BBitmap *bitmap = ਨਿਰਾ;

    ਪੂਰਨ mustfree = 0;
    char_u* runtime_ਰਸਤਾ = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    ਜੇ (runtime_ਰਸਤਾ != ਨਿਰਾ && fileName != ਨਿਰਾ) {
	BString str_ਰਸਤਾ((ਅੱਖਰ*)runtime_ਰਸਤਾ);
	str_ਰਸਤਾ << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(str_ਰਸਤਾ.String());
    }

    ਜੇ (mustfree)
	vim_free(runtime_ਰਸਤਾ);

    ਵਾਪਸ bitmap;
}

    ਤਾਰਕਿਕ
VimToolbar::GetPictureFromBitmap(BPicture *picture_ਮੰਜ਼ਿਲ, int32 ਸੂਚਕਾਂਕ, BBitmap *bitmapFrom, ਤਾਰਕਿਕ pressed)
{
    ਦਸ਼ਮਲਵ ਮਾਪ = bitmapFrom->Bounds().ਉਚਾਈ_2() + 1.;

    BView view(BRect(0, 0, ਮਾਪ, ਮਾਪ), "", 0, 0);

    ਜੋੜੋ_ਬੱਚਾ(&view);
    view.BeginPicture(picture_ਮੰਜ਼ਿਲ);

    view.ਸੈੱਟ_ਉੱਚਾ_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.ਸੈੱਟ_Drawing_ਬਹੁਲਕ(B_OP_OVER);

    BRect ਸਰੋਤ(0, 0, ਮਾਪ - 1, ਮਾਪ - 1);
    BRect destination(ਸਰੋਤ);

    ਸਰੋਤ.ਵਿੱਥ_By(ਮਾਪ * ਸੂਚਕਾਂਕ, 0);
    destination.ਵਿੱਥ_By(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, ਸਰੋਤ, destination);

    ਜੇ (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	ਮਾਪ += ButtonMargin * 2 - 1;
	view.Begin_ਸਤਰ_Array(4);
	view.ਜੋੜੋ_ਸਤਰ(BPoint(0, 0),	 BPoint(ਮਾਪ, 0),    shadowColor);
	view.ਜੋੜੋ_ਸਤਰ(BPoint(ਮਾਪ, 0),	 BPoint(ਮਾਪ, ਮਾਪ), shineColor);
	view.ਜੋੜੋ_ਸਤਰ(BPoint(ਮਾਪ, ਮਾਪ), BPoint(0, ਮਾਪ),    shineColor);
	view.ਜੋੜੋ_ਸਤਰ(BPoint(0, ਮਾਪ),	 BPoint(0, 0),	     shadowColor);
	view.ਸਿਰਾ_ਸਤਰ_Array();
    }

    view.ਸਿਰਾ_Picture();
    Remove_ਬੱਚਾ(&view);

    ਵਾਪਸ ਸੱਚ;
}

    ਤਾਰਕਿਕ
VimToolbar::ਜੋੜੋ_Button(int32 ਸੂਚਕਾਂਕ, vimmenu_T *menu)
{
    BPictureButton *button = ਨਿਰਾ;
    ਜੇ (!menu_is_separator(menu->name)) {
	ਦਸ਼ਮਲਵ ਮਾਪ = ਸਾਧਾਰਨ_Buttons_Bitmap ?
	    ਸਾਧਾਰਨ_Buttons_Bitmap->Bounds().ਉਚਾਈ_2() + 1. + ButtonMargin * 2 : 18.;
	BRect ਚੌਖਟਾ(0, 0, ਮਾਪ, ਮਾਪ);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture picture_ਸਲੇਟੀ;

	ਜੇ (menu->iconfile == ਨਿਰਾ && menu->iconidx >= 0 && ਸਾਧਾਰਨ_Buttons_Bitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, ਸਾਧਾਰਨ_Buttons_Bitmap, ਸੱਚ);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, ਸਾਧਾਰਨ_Buttons_Bitmap, ਝੂਠ);
	    GetPictureFromBitmap(&picture_ਸਲੇਟੀ, menu->iconidx, grayedButtonsBitmap, ਝੂਠ);
	} ਨਹੀਂਤਾਂ {

	    char_u ਬਫਰ[MAXPATHL] = {0};
	    BBitmap *bitmap = ਨਿਰਾ;

	    ਜੇ (menu->iconfile) {
		gui_ਲੱਭੋ_iconfile(menu->iconfile, ਬਫਰ, (ਅੱਖਰ*)"png");
		bitmap = BTranslationUtils::GetBitmap((ਅੱਖਰ*)ਬਫਰ);
	    }

	    ਜੇ (bitmap == ਨਿਰਾ && gui_ਲੱਭੋ_bitmap(menu->name, ਬਫਰ, (ਅੱਖਰ*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((ਅੱਖਰ*)ਬਫਰ);

	    ਜੇ (bitmap == ਨਿਰਾ)
		bitmap = ਨਵਾਂ BBitmap(BRect(0, 0, ਮਾਪ, ਮਾਪ), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, ਸੱਚ);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, ਝੂਠ);
	    Modify_Bitmap_ਮੰਜ਼ਿਲ_Grayed(bitmap);
	    GetPictureFromBitmap(&picture_ਸਲੇਟੀ, 0, bitmap, ਝੂਠ);

	    ਮਿਟਾਓ bitmap;
	}

	button = ਨਵਾਂ BPictureButton(ਚੌਖਟਾ, (ਅੱਖਰ*)menu->name,
		    &pictureOff, &pictureOn, Menu_ਸੁਨੇਹਾ(menu));

	button->ਸੈੱਟ_Disabled_On(&picture_ਸਲੇਟੀ);
	button->ਸੈੱਟ_Disabled_Off(&picture_ਸਲੇਟੀ);

	button->ਸੈੱਟ_ਨਿਸ਼ਾਨਾ(gui.vim_ਪਾਠ_Area);

	ਜੋੜੋ_ਬੱਚਾ(button);

	menu->button = button;
    }

    ਤਾਰਕਿਕ ਨਤੀਜਾ = fButtonsList.ਜੋੜੋ_ਵਸਤੂ(button, ਸੂਚਕਾਂਕ);
    InvalidateLayout();
    ਵਾਪਸ ਨਤੀਜਾ;
}

    ਤਾਰਕਿਕ
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    ਜੇ (menu->button) {
	ਜੇ (fButtonsList.Remove_ਵਸਤੂ(menu->button)) {
	    ਮਿਟਾਓ menu->button;
	    menu->button = ਨਿਰਾ;
	}
    }
    ਵਾਪਸ ਸੱਚ;
}

    ਤਾਰਕਿਕ
VimToolbar::ਸਲੇਟੀ_Button(vimmenu_T *menu, ਪੂਰਨ grey)
{
    ਜੇ (menu->button) {
	int32 ਸੂਚਕਾਂਕ = fButtonsList.ਸੂਚਕਾਂਕ_ਦਾ(menu->button);
	ਜੇ (ਸੂਚਕਾਂਕ >= 0)
	    menu->button->ਸੈੱਟ_Enabled(grey ? ਝੂਠ : ਸੱਚ);
    }
    ਵਾਪਸ ਸੱਚ;
}

    ਖਾਲੀ
VimToolbar::InvalidateLayout()
{
    int32 ਵਿੱਥ = ToolbarMargin;
    int32 ਗਿਣਤੀ_2 = fButtonsList.ਗਿਣਤੀ_ਚੀਜ਼ਾਂ();
    ਲਈ (int32 i = 0; i < ਗਿਣਤੀ_2; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.ਵਸਤੂ_At(i);
	ਜੇ (button) {
	    button->Move_ਮੰਜ਼ਿਲ(ਵਿੱਥ, ToolbarMargin);
	    ਵਿੱਥ += button->Bounds().ਚੌੜਾਈ_2() + ToolbarMargin;
	} ਨਹੀਂਤਾਂ
	    ਵਿੱਥ += ToolbarMargin * 3;
    }
}

#ਜੇ_ਅੰਤ /*FEAT_TOOLBAR*/

#ਜੇ defined(FEAT_GUI_TABLINE)

    ਦਸ਼ਮਲਵ
Vim_Tab_ਸਤਰ::Tabline_ਉਚਾਈ() ਅਚਲ
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    ਵਾਪਸ Tab_ਉਚਾਈ(); //  + ToolbarMargin;
}

ਖਾਲੀ
Vim_Tab_ਸਤਰ::Mouse_ਥੱਲੇ(BPoint ਬਿੰਦੂ)
{
    ਜੇ (!gui_mch_showing_tabline())
	ਵਾਪਸ;

    BMessage *m = ਖਿੜਕੀ()->Current_ਸੁਨੇਹਾ();
    ਪ੍ਰਤਿਜ੍ਞਾ(m);

    int32 buttons = 0;
    m->ਲੱਭੋ_Int32("buttons", &buttons);

    int32 clicks = 0;
    m->ਲੱਭੋ_Int32("clicks", &clicks);

    ਪੂਰਨ ਸੂਚਕਾਂਕ = 0; //  0 means here - no tab found
    ਲਈ (ਪੂਰਨ i = 0; i < ਗਿਣਤੀ_Tabs(); i++) {
	ਜੇ (Tab_ਚੌਖਟਾ(i).ਸ਼ਾਮਲ_ਹੈ_2(ਬਿੰਦੂ)) {
	    ਸੂਚਕਾਂਕ = i + 1; //  indexes are 1-based
	    ਰੋਕੋ;
	}
    }

    ਪੂਰਨ event = -1;

    ਜੇ ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    ਨਹੀਂਤਾਂ ਜੇ (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = ਸੂਚਕਾਂਕ > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    ਨਹੀਂਤਾਂ ਜੇ (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPop_ਉੱਪਰ_Menu* ਕੱਢੋ_ਉੱਪਰ_Menu = ਨਵਾਂ BPop_ਉੱਪਰ_Menu("tabLineContextMenu", ਝੂਠ, ਝੂਠ);
	ਕੱਢੋ_ਉੱਪਰ_Menu->ਜੋੜੋ_ਵਸਤੂ(ਨਵਾਂ BMenu_ਵਸਤੂ(_("Close tabi R"), ਨਵਾਂ BMessage(TABLINE_MENU_CLOSE)));
	ਕੱਢੋ_ਉੱਪਰ_Menu->ਜੋੜੋ_ਵਸਤੂ(ਨਵਾਂ BMenu_ਵਸਤੂ(_("New tab    T"), ਨਵਾਂ BMessage(TABLINE_MENU_NEW)));
	ਕੱਢੋ_ਉੱਪਰ_Menu->ਜੋੜੋ_ਵਸਤੂ(ਨਵਾਂ BMenu_ਵਸਤੂ(_("Open tab..."), ਨਵਾਂ BMessage(TABLINE_MENU_OPEN)));

	Convert_ਮੰਜ਼ਿਲ_Screen(&ਬਿੰਦੂ);
	BMenu_ਵਸਤੂ* ਵਸਤੂ = ਕੱਢੋ_ਉੱਪਰ_Menu->Go(ਬਿੰਦੂ);
	ਜੇ (ਵਸਤੂ != ਨਿਰਾ) {
	    event = ਵਸਤੂ->Command();
	}

	ਮਿਟਾਓ ਕੱਢੋ_ਉੱਪਰ_Menu;

    } ਨਹੀਂਤਾਂ {
	//  default processing
	BTabView::Mouse_ਥੱਲੇ(ਬਿੰਦੂ);
	ਵਾਪਸ;
    }

    ਜੇ (event < 0)
	ਵਾਪਸ;

    Vim_Tabline_Menu_ਸੁਨੇਹਾ tmm;
    tmm.ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;
    tmm.event = event;
    write_ਪੋਰਟ(gui.vdcmp, Vim_ਸੁਨੇਹਾ::TablineMenu, &tmm, ਆਕਾਰ(tmm));
}

ਖਾਲੀ
Vim_Tab_ਸਤਰ::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_ਸਤਰ *tab_ਸਤਰ = gui.vimForm->Tab_ਸਤਰ();
    ਜੇ (tab_ਸਤਰ != ਨਿਰਾ) {

	int32 i = 0;
	ਲਈ (; i < tab_ਸਤਰ->ਗਿਣਤੀ_Tabs(); i++)
	    ਜੇ (ਇਹ == tab_ਸਤਰ->TabAt(i))
		ਰੋਕੋ;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	ਜੇ (i < tab_ਸਤਰ->ਗਿਣਤੀ_Tabs()) {
	    Vim_Tabline_ਸੁਨੇਹਾ tm;
	    tm.ਸੂਚਕਾਂਕ = i + 1;
	    write_ਪੋਰਟ(gui.vdcmp, Vim_ਸੁਨੇਹਾ::Tabline, &tm, ਆਕਾਰ(tm));
	}
    }
}

#ਜੇ_ਅੰਤ //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
ਸਥਿਰ ਅੱਖਰ appsig[] = "application/x-vnd.Haiku-Vim-8";
ਕੁੰਜੀ_map *ਕੁੰਜੀ_Map;
ਅੱਖਰ *ਕੁੰਜੀ_Map_Chars;
ਪੂਰਨ main_exitcode = 127;

    ਸਥਿਤੀ_t
gui_haiku_process_event(bigtime_t timeout)
{
    ਬਣਤਰ Vim_ਸੁਨੇਹਾ vm;
    int32 what;
    ssize_t ਮਾਪ;

    ਮਾਪ = read_ਪੋਰਟ_etc(gui.vdcmp, &what, &vm, ਆਕਾਰ(vm),
	    B_TIMEOUT, timeout);

    ਜੇ (ਮਾਪ >= 0) {
	ਚੋਣ (what) {
	    ਹਾਲਤ Vim_ਸੁਨੇਹਾ::ਕੁੰਜੀ_2:
		{
		    char_u *string = vm.u.ਕੁੰਜੀ_2.chars;
		    ਪੂਰਨ len = vm.u.ਕੁੰਜੀ_2.ਲੰਮਾਈ;
		    ਜੇ (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_ਨਿਵੇਸ਼_buf();
			got_int = TRUE;
		    }

		    ਜੇ (vm.u.ਕੁੰਜੀ_2.csi_escape)
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
		    {
			ਪੂਰਨ	i;
			char_u	buf[2];

			ਲਈ (i = 0; i < len; ++i)
			{
			    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string + i, 1);
			    ਜੇ (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (ਪੂਰਨ)KE_CSI;
				ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(buf, 2);
			    }
			}
		    }
#ਨਹੀਂਤਾਂ
			ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf_csi(string, len);
#ਜੇ_ਅੰਤ
		    ਨਹੀਂਤਾਂ
			ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, len);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ Vim_ਸੁਨੇਹਾ::Resize:
		gui_resize_shell(vm.u.New_ਮਾਪ.ਚੌੜਾਈ, vm.u.New_ਮਾਪ.ਉਚਾਈ);
		ਰੋਕੋ;
	    ਹਾਲਤ Vim_ਸੁਨੇਹਾ::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_ਗਿਣਤੀ =
			atomic_ਜੋੜੋ(&vm.u.Scroll.sb->scroll_Event_ਗਿਣਤੀ, -1);
		    ਜੇ (old_ਗਿਣਤੀ <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.ਮੁੱਲ, vm.u.Scroll.stillDragging);
		}
		ਰੋਕੋ;
#ਜੇ defined(FEAT_MENU)
	    ਹਾਲਤ Vim_ਸੁਨੇਹਾ::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	    ਹਾਲਤ Vim_ਸੁਨੇਹਾ::Mouse:
		{
		    int32 old_ਗਿਣਤੀ;
		    ਜੇ (vm.u.Mouse.button == MOUSE_DRAG)
			old_ਗਿਣਤੀ =
			    atomic_ਜੋੜੋ(&gui.vim_ਪਾਠ_Area->mouse_Drag_Event_ਗਿਣਤੀ, -1);
		    ਨਹੀਂਤਾਂ
			old_ਗਿਣਤੀ = 0;
		    ਜੇ (old_ਗਿਣਤੀ <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ Vim_ਸੁਨੇਹਾ::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		ਰੋਕੋ;
	    ਹਾਲਤ Vim_ਸੁਨੇਹਾ::Focus:
		gui.ਵਿੱਚ_focus = vm.u.Focus.ਸਰਗਰਮ;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		ਜੇ (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		ਰੋਕੋ;
	    ਹਾਲਤ Vim_ਸੁਨੇਹਾ::Refs:
		::RefsReceived(vm.u.Refs.ਸੁਨੇਹਾ_2, vm.u.Refs.changedir);
		ਰੋਕੋ;
	    ਹਾਲਤ Vim_ਸੁਨੇਹਾ::Tabline:
		send_tabline_event(vm.u.Tabline.ਸੂਚਕਾਂਕ);
		ਰੋਕੋ;
	    ਹਾਲਤ Vim_ਸੁਨੇਹਾ::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.ਸੂਚਕਾਂਕ, vm.u.TablineMenu.event);
		ਰੋਕੋ;
	    ਮੂਲ:
		//  unrecognised message, ignore it
		ਰੋਕੋ;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    ਵਾਪਸ ਮਾਪ;
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

    ਪੂਰਨ
vim_ਤਾਲਾ_screen()
{
    ਵਾਪਸ !gui.vim_ਖਿੜਕੀ || gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2();
}

    ਖਾਲੀ
vim_unlock_screen()
{
    ਜੇ (gui.vim_ਖਿੜਕੀ)
	gui.vim_ਖਿੜਕੀ->Unlock();
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੜੀਵਾਰ_BAPPLICATION_ਵਿੱਚ_NEW_THREAD	0

#ਜੇ ਲੜੀਵਾਰ_BAPPLICATION_ਵਿੱਚ_NEW_THREAD

    int32
ਲੜੀਵਾਰ_vimapp(ਖਾਲੀ *ਦਲੀਲਾਂ)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.ਲੜੀਵਾਰ_2();		    // Run until Quit() called

    ਵਾਪਸ 0;
}

#ਨਹੀਂਤਾਂ

    int32
call_main(ਖਾਲੀ *ਦਲੀਲਾਂ)
{
    ਬਣਤਰ Main_ਦਲੀਲਾਂ *ma = (Main_ਦਲੀਲਾਂ *)ਦਲੀਲਾਂ;

    ਵਾਪਸ ਮੁੱਖ(ma->argc, ma->argv);
}
#ਜੇ_ਅੰਤ

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    ਖਾਲੀ
gui_mch_prepare(
	ਪੂਰਨ	*argc,
	ਅੱਖਰ	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    ਜੇ (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(ਲੱਭੋ_thread(ਨਿਰਾ), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_ਪੋਰਟ(B_MAX_ਪੋਰਟ_ਗਿਣਤੀ, "vim VDCMP");

#ਜੇ ਲੜੀਵਾਰ_BAPPLICATION_ਵਿੱਚ_NEW_THREAD
	thread_id tid = spawn_thread(ਲੜੀਵਾਰ_vimapp, "vim VimApp",
		tinfo.ਤਰਜੀਹ, ਨਿਰਾ);
	ਜੇ (tid >= B_OK) {
	    resume_thread(tid);
	} ਨਹੀਂਤਾਂ {
	    getout(1);
	}
#ਨਹੀਂਤਾਂ
	Main_ਦਲੀਲਾਂ ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.ਤਰਜੀਹ, &ma);
	ਜੇ (tid >= B_OK) {
	    VimApp app(appsig);

	    gui.vimApp = &app;
	    resume_thread(tid);
	    /*
	     * This is rather horrible.
	     * call_main will call main() again...
	     * There will be no infinite recursion since
	     * gui.vimApp is set now.
	     */
	    app.ਲੜੀਵਾਰ_2();		    // Run until Quit() called
	    // fprintf(stderr, "app.Run() returned...\n");
	    ਸਥਿਤੀ_t dummy_exitcode;
	    (ਖਾਲੀ)wait_for_thread(tid, &dummy_exitcode);

	    /*
	     * This path should be the normal one taken to exit Vim.
	     * The main() thread calls mch_exit() which calls
	     * gui_mch_exit() which terminates its thread.
	     */
	    ਬਾਹਰ_ਨਿਕਲੋ(main_exitcode);
	}
#ਜੇ_ਅੰਤ
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
    ਜੇ (!isatty(0)) {
	ਬਣਤਰ stat stat_stdin, stat_dev_null;

	ਜੇ (fstat(0, &stat_stdin) == -1 ||
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
    ਪੂਰਨ
gui_mch_init_check(ਖਾਲੀ)
{
    ਵਾਪਸ OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    ਪੂਰਨ
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_ਪਿੱਠ_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.ਪਿੱਠ_pixel = gui.def_ਪਿੱਠ_pixel;

    gui.scrollbar_ਚੌੜਾਈ = (ਪੂਰਨ) B_V_SCROLL_BAR_ਚੌੜਾਈ;
    gui.scrollbar_ਉਚਾਈ = (ਪੂਰਨ) B_H_SCROLL_BAR_ਉਚਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    gui.menu_ਉਚਾਈ = 19;   //	initial guess -
    //	correct for my default settings
#ਜੇ_ਅੰਤ
    gui.border_ਵਿੱਥ = 3;  //	coordinates are inside window borders

    ਜੇ (gui.vdcmp < B_OK)
	ਵਾਪਸ FAIL;
    get_ਕੁੰਜੀ_map(&ਕੁੰਜੀ_Map, &ਕੁੰਜੀ_Map_Chars);

    gui.vim_ਖਿੜਕੀ = ਨਵਾਂ Vim_ਖਿੜਕੀ();	// hidden and locked
    ਜੇ (!gui.vim_ਖਿੜਕੀ)
	ਵਾਪਸ FAIL;

    gui.vim_ਖਿੜਕੀ->ਲੜੀਵਾਰ_2();	// Run() unlocks but does not show

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    ਸੈੱਟ_ਸਾਧਾਰਨ_colors();

    /*
     * Check that none of the colors are the same as the background color
     */
    gui_check_colors();

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();	    // re-init colors and fonts

    gui_mch_new_colors();	// window must exist for this

    ਵਾਪਸ OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    ਖਾਲੀ
gui_mch_new_colors()
{
    rgb_color rgb = GUI_ਮੰਜ਼ਿਲ_RGB(gui.ਪਿੱਠ_pixel);

    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vimForm->ਸੈੱਟ_View_Color(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ਪੂਰਨ
gui_mch_open()
{
    ਜੇ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_ਸੈੱਟ_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਖਿੜਕੀ->Show();
	gui.vim_ਖਿੜਕੀ->Unlock();
	ਵਾਪਸ OK;
    }

    ਵਾਪਸ FAIL;
}

    ਖਾਲੀ
gui_mch_exit(ਪੂਰਨ vim_exitcode)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ) {
	thread_id tid = gui.vim_ਖਿੜਕੀ->Thread();
	gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2();
	gui.vim_ਖਿੜਕੀ->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_ਪੋਰਟ(gui.vdcmp);
#ਜੇ !ਲੜੀਵਾਰ_BAPPLICATION_ਵਿੱਚ_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#ਜੇ_ਅੰਤ
    ਜੇ (gui.vimApp) {
	Vim_ਪਾਠ_Area_View::guiBlankMouse(ਝੂਠ);

	main_exitcode = vim_exitcode;
#ਜੇ ਲੜੀਵਾਰ_BAPPLICATION_ਵਿੱਚ_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->ਤਾਲਾ_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#ਨਹੀਂਤਾਂ
	gui.vimApp->ਤਾਲਾ_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	// suicide
	exit_thread(vim_exitcode);
#ਜੇ_ਅੰਤ
    }
    // If we are somehow still here, let mch_exit() handle things.
}

/*
 * Get the position of the top left corner of the window.
 */
    ਪੂਰਨ
gui_mch_get_winpos(ਪੂਰਨ *x, ਪੂਰਨ *y)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	BRect r;
	r = gui.vim_ਖਿੜਕੀ->ਚੌਖਟਾ_2();
	gui.vim_ਖਿੜਕੀ->Unlock();
	*x = (ਪੂਰਨ)r.ਖੱਬੇ;
	*y = (ਪੂਰਨ)r.ਸਿਖਰ;
	ਵਾਪਸ OK;
    }
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_winpos(ਪੂਰਨ x, ਪੂਰਨ y)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਖਿੜਕੀ->Move_ਮੰਜ਼ਿਲ(x, y);
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
ਖਾਲੀ
gui_mch_ਸੈੱਟ_shellsize(
	ਪੂਰਨ	ਚੌੜਾਈ,
	ਪੂਰਨ	ਉਚਾਈ,
	ਪੂਰਨ	min_ਚੌੜਾਈ,
	ਪੂਰਨ	min_ਉਚਾਈ,
	ਪੂਰਨ	ਅਧਾਰ_ਚੌੜਾਈ,
	ਪੂਰਨ	ਅਧਾਰ_ਉਚਾਈ,
	ਪੂਰਨ	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਖਿੜਕੀ->Resize_ਮੰਜ਼ਿਲ(ਚੌੜਾਈ - PEN_ਚੌੜਾਈ, ਉਚਾਈ - PEN_ਚੌੜਾਈ);

	// set size limits
	ਦਸ਼ਮਲਵ min_ਚੌੜਾਈ_2, max_ਚੌੜਾਈ, min_ਉਚਾਈ_2, max_ਉਚਾਈ;

	gui.vim_ਖਿੜਕੀ->Get_ਮਾਪ_Limits(&min_ਚੌੜਾਈ_2, &max_ਚੌੜਾਈ,
		&min_ਉਚਾਈ_2, &max_ਉਚਾਈ);
	gui.vim_ਖਿੜਕੀ->ਸੈੱਟ_ਮਾਪ_Limits(min_ਚੌੜਾਈ, max_ਚੌੜਾਈ,
		min_ਉਚਾਈ, max_ਉਚਾਈ);

	/*
	 * Set the resizing alignment depending on font size.
	 */
	gui.vim_ਖਿੜਕੀ->ਸੈੱਟ_ਖਿੜਕੀ_Alignment(
		B_PIXEL_ALIGNMENT,	//  window_alignment mode,
		1,		//  int32 h,
		0,		//  int32 hOffset = 0,
		gui.char_ਚੌੜਾਈ,	    //	int32 width = 0,
		ਅਧਾਰ_ਚੌੜਾਈ,	    //	int32 widthOffset = 0,
		1,		//  int32 v = 0,
		0,		//  int32 vOffset = 0,
		gui.char_ਉਚਾਈ,	//  int32 height = 0,
		ਅਧਾਰ_ਉਚਾਈ	    //	int32 heightOffset = 0
		);

	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

ਖਾਲੀ
gui_mch_get_screen_dimensions(
	ਪੂਰਨ	*screen_w,
	ਪੂਰਨ	*screen_h)
{
    BRect ਚੌਖਟਾ;

    {
	BScreen screen(gui.vim_ਖਿੜਕੀ);

	ਜੇ (screen.IsValid()) {
	    ਚੌਖਟਾ = screen.ਚੌਖਟਾ_2();
	} ਨਹੀਂਤਾਂ {
	    ਚੌਖਟਾ.ਸੱਜੇ = 640;
	    ਚੌਖਟਾ.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (ਪੂਰਨ) ਚੌਖਟਾ.ਸੱਜੇ - 2 * gui.scrollbar_ਚੌੜਾਈ - 20;
    *screen_h = (ਪੂਰਨ) ਚੌਖਟਾ.bottom - gui.scrollbar_ਉਚਾਈ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	- gui.menu_ਉਚਾਈ
#ਜੇ_ਅੰਤ
	- 30;
}

ਖਾਲੀ
gui_mch_ਸੈੱਟ_ਪਾਠ_area_pos(
	ਪੂਰਨ	x,
	ਪੂਰਨ	y,
	ਪੂਰਨ	w,
	ਪੂਰਨ	h)
{
    ਜੇ (!gui.vim_ਪਾਠ_Area)
	ਵਾਪਸ;

    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਪਾਠ_Area->Move_ਮੰਜ਼ਿਲ(x, y);
	gui.vim_ਪਾਠ_Area->Resize_ਮੰਜ਼ਿਲ(w - PEN_ਚੌੜਾਈ, h - PEN_ਚੌੜਾਈ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
	ਜੇ (gui.vimForm->Tab_ਸਤਰ() != ਨਿਰਾ) {
	    gui.vimForm->Tab_ਸਤਰ()->Resize_ਮੰਜ਼ਿਲ(w, gui.vimForm->Tabline_ਉਚਾਈ());
	}
#ਜੇ_ਅੰਤ // FEAT_GUI_TABLINE

	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

ਖਾਲੀ
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	ਪੂਰਨ	flag)
{
    VimScrollBar *vsb = sb->id;
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	ਜੇ (flag) {
	    ਜੇ (vsb->IsHidden()) {
		vsb->Show();
	    }
	} ਨਹੀਂਤਾਂ {
	    ਜੇ (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

ਖਾਲੀ
gui_mch_ਸੈੱਟ_scrollbar_thumb(
	scrollbar_T *sb,
	ਪੂਰਨ	val,
	ਪੂਰਨ	ਮਾਪ,
	ਪੂਰਨ	max)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	VimScrollBar *s = sb->id;
	ਜੇ (max == 0) {
	    s->ਸੈੱਟ_ਮੁੱਲ(0);
	    s->ਸੈੱਟ_Range(0.0, 0.0);
	} ਨਹੀਂਤਾਂ {
	    s->ਸੈੱਟ_Proportion((ਦਸ਼ਮਲਵ)ਮਾਪ / (max + 1.0));
	    s->ਸੈੱਟ_ਕਦਮਾਂ(1.0, ਮਾਪ > 5 ? ਮਾਪ - 2 : ਮਾਪ);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SCROLL_PAST_ਸਿਰਾ	    //	really only defined in gui.c...
	    max = max + 1 - ਮਾਪ;
#ਜੇ_ਅੰਤ
	    ਜੇ (max < s->ਮੁੱਲ_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->ਸੈੱਟ_ਮੁੱਲ(val);
		s->ਸੈੱਟ_Range(0.0, max);
	    } ਨਹੀਂਤਾਂ {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->ਸੈੱਟ_Range(0.0, max);
		s->ਸੈੱਟ_ਮੁੱਲ(val);
	    }
	}
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

ਖਾਲੀ
gui_mch_ਸੈੱਟ_scrollbar_pos(
	scrollbar_T *sb,
	ਪੂਰਨ	x,
	ਪੂਰਨ	y,
	ਪੂਰਨ	w,
	ਪੂਰਨ	h)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	BRect winb = gui.vim_ਖਿੜਕੀ->Bounds();
	ਦਸ਼ਮਲਵ vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->Resize_ਮੰਜ਼ਿਲ(w - PEN_ਚੌੜਾਈ, h - PEN_ਚੌੜਾਈ);
	ਜੇ (winb.ਸੱਜੇ-(x+w)<w) vsbx = winb.ਸੱਜੇ - (w - PEN_ਚੌੜਾਈ);
	vsb->Move_ਮੰਜ਼ਿਲ(vsbx, vsby);
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

    ਪੂਰਨ
gui_mch_get_scrollbar_xpadding(ਖਾਲੀ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ਵਾਪਸ 0;
}

    ਪੂਰਨ
gui_mch_get_scrollbar_ypadding(ਖਾਲੀ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ਵਾਪਸ 0;
}

ਖਾਲੀ
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	ਪੂਰਨ	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = ਨਵਾਂ VimScrollBar(sb, posture);
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	vsb->ਸੈੱਟ_ਨਿਸ਼ਾਨਾ(gui.vim_ਪਾਠ_Area);
	vsb->Hide();
	gui.vimForm->ਜੋੜੋ_ਬੱਚਾ(vsb);
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

#ਜੇ defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
ਖਾਲੀ
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	sb->id->RemoveSelf();
	ਮਿਟਾਓ sb->id;
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}
#ਜੇ_ਅੰਤ

/*
 * Cursor does not flash
 */
    ਪੂਰਨ
gui_mch_is_blink_off(ਖਾਲੀ)
{
    ਵਾਪਸ FALSE;
}

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON blinking, cursor is shown
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLINK_NONE  0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLINK_OFF   1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLINK_ON    2

ਸਥਿਰ ਪੂਰਨ	blink_state = BLINK_NONE;
ਸਥਿਰ long_u	    blink_waittime = 700;
ਸਥਿਰ long_u	    blink_ontime = 400;
ਸਥਿਰ long_u	    blink_offtime = 250;
ਸਥਿਰ ਪੂਰਨ  blink_timer = 0;

ਖਾਲੀ
gui_mch_ਸੈੱਟ_blinking(
	ਲੰਮਾ	waittime,
	ਲੰਮਾ	on,
	ਲੰਮਾ	off)
{
    // TODO
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ਖਾਲੀ
gui_mch_stop_blink(ਪੂਰਨ may_call_gui_update_cursor)
{
    // TODO
    ਜੇ (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    ਜੇ (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    ਖਾਲੀ
gui_mch_ਸ਼ੁਰੂ_blink()
{
    // TODO
    ਜੇ (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    ਜੇ (blink_waittime && blink_ontime && blink_offtime && gui.ਵਿੱਚ_focus)
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
ਪੂਰਨ
gui_mch_init_font(
	char_u	    *font_name,
	ਪੂਰਨ	    fontset)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2())
    {
	ਪੂਰਨ rc = gui.vim_ਪਾਠ_Area->mchInitFont(font_name);
	gui.vim_ਖਿੜਕੀ->Unlock();

	ਵਾਪਸ rc;
    }

    ਵਾਪਸ FAIL;
}


    ਪੂਰਨ
gui_mch_adjust_charsize()
{
    ਵਾਪਸ FAIL;
}


    ਪੂਰਨ
gui_mch_font_dialog(font_family* family, font_style* style, ਦਸ਼ਮਲਵ* ਮਾਪ)
{
#ਜੇ defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = ਨਵਾਂ VimSelectFontDialog(family, style, ਮਾਪ);
    ਤਾਰਕਿਕ ret = dialog->Go();
	ਮਿਟਾਓ dialog;
	ਵਾਪਸ ret;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ NOFONT;
#ਜੇ_ਅੰਤ // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	ਪੂਰਨ	    giveErrorIfMissing)
{
    ਸਥਿਰ VimFont *fontList = ਨਿਰਾ;

    ਜੇ (!gui.ਵਿੱਚ_use)	//  can't do this when GUI not running
	ਵਾਪਸ NOFONT;

    //	storage for locally modified name;
    ਅਚਲ ਪੂਰਨ buff_ਮਾਪ = B_FONT_FAMILY_ਲੰਮਾਈ + B_FONT_STYLE_ਲੰਮਾਈ + 20;
    ਸਥਿਰ ਅੱਖਰ font_name[buff_ਮਾਪ] = {0};
    font_family family = {0};
    font_style	style  = {0};
    ਦਸ਼ਮਲਵ ਮਾਪ = 0.f;

    ਜੇ (name == 0 && be_fixed_font == 0) {
	ਜੇ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	ਵਾਪਸ NOFONT;
    }

    ਤਾਰਕਿਕ useSelectGUI = ਝੂਠ;
    ਜੇ (name != ਨਿਰਾ)
	ਜੇ (STRCMP(name, "*") == 0) {
	    useSelectGUI = ਸੱਚ;
	    STRNCPY(font_name, hl_get_font_name(), buff_ਮਾਪ);
	} ਨਹੀਂਤਾਂ
	    STRNCPY(font_name, name, buff_ਮਾਪ);

    ਜੇ (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	ਮਾਪ = be_fixed_font->ਮਾਪ_2();
	vim_snprintf(font_name, buff_ਮਾਪ,
	    (ਅੱਖਰ*)"%s/%s/%.0f", family, style, ਮਾਪ);
    }

    //	replace underscores with spaces
    ਅੱਖਰ* ਸਿਰਾ = 0;
    ਜਦੋਂ (ਸਿਰਾ = strchr((ਅੱਖਰ *)font_name, '_'))
	*ਸਿਰਾ = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    ਸਥਿਰ ਅੱਖਰ buff[buff_ਮਾਪ] = {0};
    STRNCPY(buff, font_name, buff_ਮਾਪ);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_ਲੰਮਾਈ);
    ਅੱਖਰ* style_s = strtok(0, "/\0");
    ਜੇ (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_ਲੰਮਾਈ);
    ਮਾਪ = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    ਜੇ (useSelectGUI) {
	ਜੇ (gui_mch_font_dialog(&family, &style, &ਮਾਪ) == NOFONT)
	    ਵਾਪਸ FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_ਮਾਪ,
		(ਅੱਖਰ*)"%s/%s/%.0f", family, style, ਮਾਪ);
	hl_ਸੈੱਟ_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	ਜੇ (new_p_guifont != ਨਿਰਾ) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    ਲਈ ( ; *new_p_guifont; ++new_p_guifont)
		ਜੇ (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    ਲਈ (flp = fontList; flp; flp = flp->ਅਗਲਾ) {
	ਜੇ (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    ਵਾਪਸ (GuiFont)flp;
	}
    }

    VimFont *font = ਨਵਾਂ VimFont();
    font->name = vim_strsave((char_u*)font_name);

    ਜੇ (ਗਿਣਤੀ_font_styles(family) <= 0) {
	ਜੇ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	ਮਿਟਾਓ font;
	ਵਾਪਸ NOFONT;
    }

    //	Remember font in the static list for later use
    font->ਅਗਲਾ = fontList;
    fontList = font;

    font->ਸੈੱਟ_Family_And_Style(family, style);
    ਜੇ (ਮਾਪ > 0.f)
	font->ਸੈੱਟ_ਮਾਪ(ਮਾਪ);

    font->ਸੈੱਟ_Spacing(B_FIXED_SPACING);
    font->ਸੈੱਟ_Encoding(B_UNICODE_UTF8);

    ਵਾਪਸ (GuiFont)font;
}

/*
 * Set the current text font.
 */
ਖਾਲੀ
gui_mch_ਸੈੱਟ_font(
	GuiFont	font)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	VimFont *vf = (VimFont *)font;

	gui.vim_ਪਾਠ_Area->ਸੈੱਟ_Font(vf);

	gui.char_ਚੌੜਾਈ = (ਪੂਰਨ) vf->String_ਚੌੜਾਈ("n");
	font_ਉਚਾਈ fh;
	vf->Get_ਉਚਾਈ(&fh);
	gui.char_ਉਚਾਈ = (ਪੂਰਨ)(fh.ascent + 0.9999)
	    + (ਪੂਰਨ)(fh.descent + 0.9999) + (ਪੂਰਨ)(fh.leading + 0.9999);
	gui.char_ascent = (ਪੂਰਨ)(fh.ascent + 0.9999);

	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

// XXX TODO This is apparently never called...
ਖਾਲੀ
gui_mch_free_font(
	GuiFont	font)
{
    ਜੇ (font == NOFONT)
	ਵਾਪਸ;
    VimFont *f = (VimFont *)font;
    ਜੇ (--f->refcount <= 0) {
	ਜੇ (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	ਮਿਟਾਓ f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ਪੂਰਨ
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
	font_ਉਚਾਈ fh = {0};
	font->Get_ਉਚਾਈ(&fh);
	gui.char_ਉਚਾਈ = (ਪੂਰਨ)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (ਪੂਰਨ)(fh.ascent + 0.5) + p_linespace / 2;
    }
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
gui_mch_getmouse(ਪੂਰਨ *x, ਪੂਰਨ *y)
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

    ਖਾਲੀ
gui_mch_mousehide(ਪੂਰਨ hide)
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
    ਵਾਪਸ gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
ਖਾਲੀ
gui_mch_ਸੈੱਟ_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_ਮੰਜ਼ਿਲ_RGB(color);
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਪਾਠ_Area->ਸੈੱਟ_ਉੱਚਾ_Color(rgb);
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

/*
 * Set the current text background color.
 */
ਖਾਲੀ
gui_mch_ਸੈੱਟ_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_ਮੰਜ਼ਿਲ_RGB(color);
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਪਾਠ_Area->ਸੈੱਟ_ਨੀਵਾਂ_Color(rgb);
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

ਖਾਲੀ
gui_mch_draw_string(
	ਪੂਰਨ	ਪੰਕਤੀ,
	ਪੂਰਨ	ਖਾਨਾ,
	char_u	*s,
	ਪੂਰਨ	len,
	ਪੂਰਨ	flags)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਪਾਠ_Area->mchDrawString(ਪੰਕਤੀ, ਖਾਨਾ, s, len, flags);
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(ਪੂਰਨ r, ਪੂਰਨ g, ਪੂਰਨ b)
{
    ਵਾਪਸ gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
ਪੂਰਨ
gui_mch_haskey(
	char_u	*name)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; special_ਕੁੰਜੀਆਂ[i].Be_ਕੁੰਜੀਆਂ != 0; i++)
	ਜੇ (name[0] == special_ਕੁੰਜੀਆਂ[i].vim_code0 &&
		name[1] == special_ਕੁੰਜੀਆਂ[i].vim_code1)
	    ਵਾਪਸ OK;
    ਵਾਪਸ FAIL;
}

    ਖਾਲੀ
gui_mch_beep()
{
    ::beep();
}

    ਖਾਲੀ
gui_mch_flash(ਪੂਰਨ msec)
{
    // Do a visual beep by reversing the foreground and background colors

    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	BRect rect = gui.vim_ਪਾਠ_Area->Bounds();

	gui.vim_ਪਾਠ_Area->ਸੈੱਟ_Drawing_ਬਹੁਲਕ(B_OP_INVERT);
	gui.vim_ਪਾਠ_Area->FillRect(rect);
	gui.vim_ਪਾਠ_Area->Sync();
	snooze(msec * 1000);	 // wait for a few msec
	gui.vim_ਪਾਠ_Area->FillRect(rect);
	gui.vim_ਪਾਠ_Area->ਸੈੱਟ_Drawing_ਬਹੁਲਕ(B_OP_COPY);
	gui.vim_ਪਾਠ_Area->Flush();
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
ਖਾਲੀ
gui_mch_invert_rectangle(
	ਪੂਰਨ	r,
	ਪੂਰਨ	c,
	ਪੂਰਨ	nr,
	ਪੂਰਨ	nc)
{
    BRect rect;
    rect.ਖੱਬੇ = FILL_X(c);
    rect.ਸਿਖਰ = FILL_Y(r);
    rect.ਸੱਜੇ = rect.ਖੱਬੇ + nc * gui.char_ਚੌੜਾਈ - PEN_ਚੌੜਾਈ;
    rect.bottom = rect.ਸਿਖਰ + nr * gui.char_ਉਚਾਈ - PEN_ਚੌੜਾਈ;

    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਪਾਠ_Area->ਸੈੱਟ_Drawing_ਬਹੁਲਕ(B_OP_INVERT);
	gui.vim_ਪਾਠ_Area->FillRect(rect);
	gui.vim_ਪਾਠ_Area->ਸੈੱਟ_Drawing_ਬਹੁਲਕ(B_OP_COPY);
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    ਖਾਲੀ
gui_mch_iconify()
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਖਿੜਕੀ->Minimize(ਸੱਚ);
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_foreground(ਖਾਲੀ)
{
    // TODO
}
#ਜੇ_ਅੰਤ

/*
 * Set the window title
 */
ਖਾਲੀ
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਖਿੜਕੀ->ਸੈੱਟ_Title((ਅੱਖਰ *)title);
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    ਖਾਲੀ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_ਸੈੱਟ_fg_color(color);

    BRect r;
    r.ਖੱਬੇ = FILL_X(gui.ਖਾਨਾ);
    r.ਸਿਖਰ = FILL_Y(gui.ਪੰਕਤੀ);
    ਪੂਰਨ cells = utf_off2cells(ਸਤਰ_ਵਿੱਥ[gui.ਪੰਕਤੀ] + gui.ਖਾਨਾ, 100); // TODO-TODO
    ਜੇ (cells>=4) cells = 1;
    r.ਸੱਜੇ = r.ਖੱਬੇ + cells*gui.char_ਚੌੜਾਈ - PEN_ਚੌੜਾਈ;
    r.bottom = r.ਸਿਖਰ + gui.char_ਉਚਾਈ - PEN_ਚੌੜਾਈ;

    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਪਾਠ_Area->StrokeRect(r);
	gui.vim_ਖਿੜਕੀ->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
ਖਾਲੀ
gui_mch_draw_part_cursor(
	ਪੂਰਨ	w,
	ਪੂਰਨ	h,
	guicolor_T  color)
{
    gui_mch_ਸੈੱਟ_fg_color(color);

    BRect r;
    r.ਖੱਬੇ =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_ਸੱਜਾ ? FILL_X(gui.ਖਾਨਾ + 1) - w :
#ਜੇ_ਅੰਤ
	FILL_X(gui.ਖਾਨਾ);
    r.ਸੱਜੇ = r.ਖੱਬੇ + w - PEN_ਚੌੜਾਈ;
    r.bottom = FILL_Y(gui.ਪੰਕਤੀ + 1) - PEN_ਚੌੜਾਈ;
    r.ਸਿਖਰ = r.bottom - h + PEN_ਚੌੜਾਈ;

    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਪਾਠ_Area->FillRect(r);
	gui.vim_ਖਿੜਕੀ->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    ਖਾਲੀ
gui_mch_update()
{
    gui_mch_flush();
    ਜਦੋਂ (ਪੋਰਟ_ਗਿਣਤੀ(gui.vdcmp) > 0 &&
	    !vim_is_ਨਿਵੇਸ਼_buf_ਭਰਿਆ() &&
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
ਪੂਰਨ
gui_mch_wait_for_chars(
	ਲੰਮਾ	wtime)
{
    ਪੂਰਨ		focus;
    bigtime_t	until, timeout;
    ਸਥਿਤੀ_t	st;

    ਜੇ (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_ਸਮਾਂ() + timeout;
    }
    ਨਹੀਂਤਾਂ
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.ਵਿੱਚ_focus;
    ਲਈ (;;)
    {
	// Stop or start blinking when focus changes
	ਜੇ (gui.ਵਿੱਚ_focus != focus)
	{
	    ਜੇ (gui.ਵਿੱਚ_focus)
		gui_mch_ਸ਼ੁਰੂ_blink();
	    ਨਹੀਂਤਾਂ
		gui_mch_stop_blink(TRUE);
	    focus = gui.ਵਿੱਚ_focus;
	}

	gui_mch_flush();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੁਨੇਹਾ_ਕਤਾਰ
# ifdef FEAT_TIMERS
	did_ਜੋੜੋ_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ਜੇ (did_ਜੋੜੋ_timer)
	    // Need to recompute the waiting time.
	    ਰੋਕੋ;
# endif
# ifdef FEAT_ਕੰਮ_CHANNEL
	ਜੇ (has_any_channel())
	{
	    ਜੇ (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	ਨਹੀਂਤਾਂ ਜੇ (wtime < 0)
	    timeout = B_INFINITE_TIMEOUT;
# endif
#ਜੇ_ਅੰਤ

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use gui_haiku_process_event() to hang until
	 * an event arrives.  No need to check for input_buf_full because we
	 * are returning as soon as it contains a single char.
	 */
	st = gui_haiku_process_event(timeout);

	ਜੇ (ਨਿਵੇਸ਼_ਉਪਲਬਧ())
	    ਵਾਪਸ OK;
	ਜੇ (st < B_OK)		// includes B_TIMED_OUT
	    ਵਾਪਸ FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	ਜੇ (wtime >= 0)
	{
	    timeout = until - system_ਸਮਾਂ();
	    ਜੇ (timeout < 0)
		ਰੋਕੋ;
	}
    }
    ਵਾਪਸ FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    ਖਾਲੀ
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਖਿੜਕੀ->Flush();
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
    ਵਾਪਸ;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
ਖਾਲੀ
gui_mch_clear_ਖੰਡ(
	ਪੂਰਨ	row1,
	ਪੂਰਨ	col1,
	ਪੂਰਨ	row2,
	ਪੂਰਨ	col2)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਪਾਠ_Area->mch_Clear_ਖੰਡ(row1, col1, row2, col2);
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

    ਖਾਲੀ
gui_mch_clear_all()
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	gui.vim_ਪਾਠ_Area->mchClearAll();
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
ਖਾਲੀ
gui_mch_delete_ਸਤਰਾਂ(
	ਪੂਰਨ	ਪੰਕਤੀ,
	ਪੂਰਨ	ਅੰਕ_ਸਤਰਾਂ)
{
    gui.vim_ਪਾਠ_Area->mch_Delete_ਸਤਰਾਂ(ਪੰਕਤੀ, ਅੰਕ_ਸਤਰਾਂ);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
ਖਾਲੀ
gui_mch_ਪਾਓ_ਸਤਰਾਂ(
	ਪੂਰਨ	ਪੰਕਤੀ,
	ਪੂਰਨ	ਅੰਕ_ਸਤਰਾਂ)
{
    gui.vim_ਪਾਠ_Area->mch_ਪਾਓ_ਸਤਰਾਂ(ਪੰਕਤੀ, ਅੰਕ_ਸਤਰਾਂ);
}

#ਜੇ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ਖਾਲੀ
gui_mch_enable_menu(
	ਪੂਰਨ	flag)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->ਸੈੱਟ_Enabled(flag);
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

ਖਾਲੀ
gui_mch_ਸੈੱਟ_menu_pos(
	ਪੂਰਨ	x,
	ਪੂਰਨ	y,
	ਪੂਰਨ	w,
	ਪੂਰਨ	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
ਖਾਲੀ
gui_mch_ਜੋੜੋ_menu(
	vimmenu_T   *menu,
	ਪੂਰਨ	idx)
{
    vimmenu_T	*ਮਾਪਾ = menu->ਮਾਪਾ;

    //	popup menu - just create it unattached
    ਜੇ (menu_is_popup(menu->name) && ਮਾਪਾ == ਨਿਰਾ) {
	BPop_ਉੱਪਰ_Menu* ਕੱਢੋ_ਉੱਪਰ_Menu = ਨਵਾਂ BPop_ਉੱਪਰ_Menu((ਅਚਲ ਅੱਖਰ*)menu->name, ਝੂਠ, ਝੂਠ);
	menu->submenu_id = ਕੱਢੋ_ਉੱਪਰ_Menu;
	menu->id = ਨਿਰਾ;
	ਵਾਪਸ;
    }

    ਜੇ (!menu_is_menubar(menu->name)
	    || (ਮਾਪਾ != ਨਿਰਾ && ਮਾਪਾ->submenu_id == ਨਿਰਾ))
	ਵਾਪਸ;

    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	ਜੇ ( ਮਾਪਾ )
	    tmp = ਮਾਪਾ->submenu_id;
	ਨਹੀਂਤਾਂ
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	ਜੇ ( ! tmp->ਲੱਭੋ_ਵਸਤੂ((ਅਚਲ ਅੱਖਰ *) menu->dname)) {

	    BMenu *bmenu = ਨਵਾਂ BMenu((ਅੱਖਰ *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->ਜੋੜੋ_ਵਸਤੂ(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->ਲੱਭੋ_ਵਸਤੂ((ਅਚਲ ਅੱਖਰ *) menu->dname);

	}
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

    ਖਾਲੀ
gui_mch_toggle_tearoffs(ਪੂਰਨ enable)
{
    // no tearoff menus
}

    ਸਥਿਰ BMessage *
Menu_ਸੁਨੇਹਾ(vimmenu_T *menu)
{
    BMessage *m = ਨਵਾਂ BMessage('menu');
    m->ਜੋੜੋ_ਸੂਚਕ("VimMenu", (ਖਾਲੀ *)menu);

    ਵਾਪਸ m;
}

/*
 * Add a menu item to a menu
 */
ਖਾਲੀ
gui_mch_ਜੋੜੋ_menu_ਵਸਤੂ(
	vimmenu_T   *menu,
	ਪੂਰਨ	idx)
{
    ਪੂਰਨ	    mnemonic = 0;
    vimmenu_T	*ਮਾਪਾ = menu->ਮਾਪਾ;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2())
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
	ਜੇ (menu_is_toolbar(ਮਾਪਾ->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ਜੇ (toolbar != ਨਿਰਾ) {
		toolbar->ਜੋੜੋ_Button(idx, menu);
	    }
	} ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ

	ਜੇ (ਮਾਪਾ->submenu_id != ਨਿਰਾ || menu_is_popup(ਮਾਪਾ->name)) {
	    ਜੇ (menu_is_separator(menu->name)) {
		BSeparator_ਵਸਤੂ *ਵਸਤੂ = ਨਵਾਂ BSeparator_ਵਸਤੂ();
		ਮਾਪਾ->submenu_id->ਜੋੜੋ_ਵਸਤੂ(ਵਸਤੂ);
		menu->id = ਵਸਤੂ;
		menu->submenu_id = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ {
		BMenu_ਵਸਤੂ *ਵਸਤੂ = ਨਵਾਂ BMenu_ਵਸਤੂ((ਅੱਖਰ *)menu->dname,
			Menu_ਸੁਨੇਹਾ(menu));
		ਵਸਤੂ->ਸੈੱਟ_ਨਿਸ਼ਾਨਾ(gui.vim_ਪਾਠ_Area);
		ਵਸਤੂ->ਸੈੱਟ_Trigger((ਅੱਖਰ) menu->mnemonic);
		ਮਾਪਾ->submenu_id->ਜੋੜੋ_ਵਸਤੂ(ਵਸਤੂ);
		menu->id = ਵਸਤੂ;
		menu->submenu_id = ਨਿਰਾ;
	    }
	}
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
ਖਾਲੀ
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2())
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
	ਜੇ (menu->ਮਾਪਾ && menu_is_toolbar(menu->ਮਾਪਾ->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ਜੇ (toolbar != ਨਿਰਾ) {
		toolbar->RemoveButton(menu);
	    }
	} ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    ਪ੍ਰਤਿਜ੍ਞਾ(menu->submenu_id == ਨਿਰਾ || menu->submenu_id->ਗਿਣਤੀ_ਚੀਜ਼ਾਂ() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    ਜੇ (bmenu)
	    {
		bmenu->Remove_ਵਸਤੂ(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		ਜੇ (bmenu == gui.vimForm->MenuBar() && bmenu->ਗਿਣਤੀ_ਚੀਜ਼ਾਂ() == 0)
		{
		    bmenu->Resize_ਮੰਜ਼ਿਲ(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    ਮਿਟਾਓ menu->id;
	    menu->id = ਨਿਰਾ;
	    menu->submenu_id = ਨਿਰਾ;

	    gui.menu_ਉਚਾਈ = (ਪੂਰਨ) gui.vimForm->Menu_ਉਚਾਈ();
	}
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
ਖਾਲੀ
gui_mch_menu_grey(
	vimmenu_T   *menu,
	ਪੂਰਨ	grey)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    ਜੇ (menu->ਮਾਪਾ && menu_is_toolbar(menu->ਮਾਪਾ->name)) {
	ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ਜੇ (toolbar != ਨਿਰਾ) {
		toolbar->ਸਲੇਟੀ_Button(menu, grey);
	    }
	    gui.vim_ਖਿੜਕੀ->Unlock();
	}
    } ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (menu->id != ਨਿਰਾ)
	menu->id->ਸੈੱਟ_Enabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
ਖਾਲੀ
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	ਪੂਰਨ	hidden)
{
    ਜੇ (menu->id != ਨਿਰਾ)
	menu->id->ਸੈੱਟ_Enabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    ਖਾਲੀ
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    ਖਾਲੀ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    ਜੇ (!menu_is_popup(menu->name) || menu->submenu_id == ਨਿਰਾ)
	ਵਾਪਸ;

    BPop_ਉੱਪਰ_Menu* popupMenu = dynamic_cast<BPop_ਉੱਪਰ_Menu*>(menu->submenu_id);
    ਜੇ (popupMenu == ਨਿਰਾ)
	ਵਾਪਸ;

    BPoint ਬਿੰਦੂ;
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	uint32 buttons = 0;
	gui.vim_ਪਾਠ_Area->GetMouse(&ਬਿੰਦੂ, &buttons);
	gui.vim_ਪਾਠ_Area->Convert_ਮੰਜ਼ਿਲ_Screen(&ਬਿੰਦੂ);
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
    popupMenu->Go(ਬਿੰਦੂ, ਸੱਚ);
}

#ਜੇ_ਅੰਤ // FEAT_MENU

// Mouse stuff

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
ਅੱਖਰ textplain[] = "text/plain";
ਅੱਖਰ vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    ਖਾਲੀ
clip_mch_request_selection(Clipboard_T *cbd)
{
    ਜੇ (be_clipboard->ਤਾਲਾ_2())
    {
	BMessage *m = be_clipboard->ਅੰਕੜੇ_2();
	// m->PrintToStream();

	char_u *string = ਨਿਰਾ;
	ssize_t stringlen = -1;

	ਜੇ (m->ਲੱਭੋ_ਅੰਕੜੇ(textplain, B_MIME_TYPE,
		    (ਅਚਲ ਖਾਲੀ **)&string, &stringlen) == B_OK
		|| m->ਲੱਭੋ_String("text", (ਅਚਲ ਅੱਖਰ **)&string) == B_OK)
	{
	    ਜੇ (stringlen == -1)
		stringlen = STRLEN(string);

	    ਪੂਰਨ type;
	    ਅੱਖਰ *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    ਜੇ (m->ਲੱਭੋ_ਅੰਕੜੇ(vimselectiontype, B_MIME_TYPE,
			(ਅਚਲ ਖਾਲੀ **)&seltype, &seltypelen) == B_OK)
	    {
		ਚੋਣ (*seltype)
		{
		    ਮੂਲ:
		    ਹਾਲਤ 'L':	type = MLINE;	ਰੋਕੋ;
		    ਹਾਲਤ 'C':	type = MCHAR;	ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VISUAL
		    ਹਾਲਤ 'B':	type = MBLOCK;	ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Otherwise use heuristic as documented
		type = memchr(string, stringlen, '\n') ? MLINE : MCHAR;
	    }
	    clip_yank_selection(type, string, (ਲੰਮਾ)stringlen, cbd);
	}
	be_clipboard->Unlock();
    }
}
/*
 * Make vim the owner of the current selection.
 */
    ਖਾਲੀ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    ਪੂਰਨ
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    ਵਾਪਸ FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    ਖਾਲੀ
clip_mch_ਸੈੱਟ_selection(Clipboard_T *cbd)
{
    ਜੇ (be_clipboard->ਤਾਲਾ_2())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->ਅੰਕੜੇ_2();
	ਪ੍ਰਤਿਜ੍ਞਾ(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = ਨਿਰਾ;
	long_u	ਗਿਣਤੀ_2;
	ਪੂਰਨ type;

	type = clip_convert_selection(&str, &ਗਿਣਤੀ_2, cbd);

	ਜੇ (type < 0)
	    ਵਾਪਸ;

	m->ਜੋੜੋ_ਅੰਕੜੇ(textplain, B_MIME_TYPE, (ਖਾਲੀ *)str, ਗਿਣਤੀ_2);

	// Add type of selection
	ਅੱਖਰ	vtype;
	ਚੋਣ (type)
	{
	    ਮੂਲ:
	    ਹਾਲਤ MLINE:    vtype = 'L';    ਰੋਕੋ;
	    ਹਾਲਤ MCHAR:    vtype = 'C';    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VISUAL
	    ਹਾਲਤ MBLOCK:   vtype = 'B';    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	}
	m->ਜੋੜੋ_ਅੰਕੜੇ(vimselectiontype, B_MIME_TYPE, (ਖਾਲੀ *)&vtype, 1);

	vim_free(str);

	be_clipboard->Commit();
	be_clipboard->Unlock();
    }
}

#ਜੇ_ਅੰਤ	// FEAT_CLIPBOARD

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
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
	ਪੂਰਨ saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = ਨਵਾਂ BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    ਨਿਰਾ, ਨਿਰਾ, 0, ਝੂਠ,
	    ਨਵਾਂ BMessage((saving == TRUE) ? 'save' : 'open'), ਨਿਰਾ, ਸੱਚ);

    gui.vimApp->f_Browsed_ਰਸਤਾ.Unset();

    gui.vimApp->fFilePanel->ਖਿੜਕੀ()->ਸੈੱਟ_Title((ਅੱਖਰ*)title);
    gui.vimApp->fFilePanel->ਸੈੱਟ_Panel_Directory((ਅਚਲ ਅੱਖਰ*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    ਜਦੋਂ (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = ਨਿਰਾ;
    ਸਥਿਤੀ_t ਨਤੀਜਾ = gui.vimApp->f_Browsed_ਰਸਤਾ.InitCheck();
    ਜੇ (ਨਤੀਜਾ == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->f_Browsed_ਰਸਤਾ.ਰਸਤਾ_2());
    } ਨਹੀਂਤਾਂ
	ਜੇ (ਨਤੀਜਾ != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    ਨਤੀਜਾ, strerror(ਨਤੀਜਾ));
	}

    ਮਿਟਾਓ gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = ਨਿਰਾ;

    ਵਾਪਸ fileName;
}
#ਜੇ_ਅੰਤ // FEAT_BROWSE


#ਜੇ defined(FEAT_GUI_DIALOG)

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

ਪੂਰਨ
gui_mch_dialog(
	ਪੂਰਨ	 type,
	char_u	*title,
	char_u	*ਸੁਨੇਹਾ_2,
	char_u	*buttons,
	ਪੂਰਨ	 dfltbutton,
	char_u	*textfield,
	ਪੂਰਨ ex_cmd)
{
    VimDialog *dialog = ਨਵਾਂ VimDialog(type, (ਅੱਖਰ*)title, (ਅੱਖਰ*)ਸੁਨੇਹਾ_2,
	    (ਅੱਖਰ*)buttons, dfltbutton, (ਅੱਖਰ*)textfield, ex_cmd);
    ਤਾਰਕਿਕ ret = dialog->Go();
    ਮਿਟਾਓ dialog;
	ਵਾਪਸ ret;
}

#ਜੇ_ਅੰਤ // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_ਮੰਜ਼ਿਲ_RGB(pixel);

    ਵਾਪਸ ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    ਖਾਲੀ
gui_mch_setmouse(ਪੂਰਨ x, ਪੂਰਨ y)
{
    TRACE();
    // TODO
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
    ਖਾਲੀ
im_ਸੈੱਟ_position(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2())
    {
	gui.vim_ਪਾਠ_Area->DrawIMString();
	gui.vim_ਖਿੜਕੀ->Unlock();
    }
    ਵਾਪਸ;
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
gui_mch_show_toolbar(ਪੂਰਨ showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_ਉਚਾਈ = (toolbar && showit) ? toolbar->Toolbar_ਉਚਾਈ() : 0.;
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_toolbar_pos(ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ w, ਪੂਰਨ h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    ਜੇ (toolbar != ਨਿਰਾ) {
	ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	    toolbar->Move_ਮੰਜ਼ਿਲ(x, y);
	    toolbar->Resize_ਮੰਜ਼ਿਲ(w - 1, h - 1);
	    gui.vim_ਖਿੜਕੀ->Unlock();
	}
    }
}

#ਜੇ defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    ਖਾਲੀ
gui_mch_show_tabline(ਪੂਰਨ showit)
{
    Vim_Tab_ਸਤਰ *tab_ਸਤਰ = gui.vimForm->Tab_ਸਤਰ();

    ਜੇ (tab_ਸਤਰ == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (!showit != !gui.vimForm->Is_Showing_Tab_ਸਤਰ()) {
	gui.vimForm->ਸੈੱਟ_Showing_Tab_ਸਤਰ(showit != 0);
	gui.tabline_ਉਚਾਈ = gui.vimForm->Tabline_ਉਚਾਈ();
    }
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_tabline_pos(ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ w, ਪੂਰਨ h)
{
    Vim_Tab_ਸਤਰ *tab_ਸਤਰ = gui.vimForm->Tab_ਸਤਰ();
    ਜੇ (tab_ਸਤਰ != ਨਿਰਾ) {
	ਜੇ (gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2()) {
	    tab_ਸਤਰ->Move_ਮੰਜ਼ਿਲ(x, y);
	    tab_ਸਤਰ->Resize_ਮੰਜ਼ਿਲ(w - 1, h - 1);
	    gui.vim_ਖਿੜਕੀ->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    ਪੂਰਨ
gui_mch_showing_tabline()
{
    Vim_Tab_ਸਤਰ *tab_ਸਤਰ = gui.vimForm->Tab_ਸਤਰ();
    ਵਾਪਸ tab_ਸਤਰ != ਨਿਰਾ && gui.vimForm->Is_Showing_Tab_ਸਤਰ();
}

/*
 * Update the labels of the tabline.
 */
    ਖਾਲੀ
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    ਪੂਰਨ	    nr = 0;
    ਪੂਰਨ	    curtabidx = 0;

    Vim_Tab_ਸਤਰ *tab_ਸਤਰ = gui.vimForm->Tab_ਸਤਰ();

    ਜੇ (tab_ਸਤਰ == ਨਿਰਾ)
	ਵਾਪਸ;

    gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2();

    // Add a label for each tab page.  They all contain the same text area.
    ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp != ਨਿਰਾ; tp = tp->tp_ਅਗਲਾ, ++nr) {
	ਜੇ (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_ਸਤਰ->TabAt(nr);

	ਜੇ (tab == ਨਿਰਾ) {
	    tab = ਨਵਾਂ Vim_Tab_ਸਤਰ::VimTab();
	    tab_ਸਤਰ->ਜੋੜੋ_Tab(ਨਿਰਾ, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->ਸੈੱਟ_Label((ਅਚਲ ਅੱਖਰ*)NameBuff);
	tab_ਸਤਰ->Invalidate();
    }

    // Remove any old labels.
    ਜਦੋਂ (nr < tab_ਸਤਰ->ਗਿਣਤੀ_Tabs())
	tab_ਸਤਰ->RemoveTab(nr);

    ਜੇ (tab_ਸਤਰ->Selection() != curtabidx)
	tab_ਸਤਰ->Select(curtabidx);

    gui.vim_ਖਿੜਕੀ->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_curtab(ਪੂਰਨ nr)
{
    Vim_Tab_ਸਤਰ *tab_ਸਤਰ = gui.vimForm->Tab_ਸਤਰ();
    ਜੇ (tab_ਸਤਰ == ਨਿਰਾ)
	ਵਾਪਸ;

    gui.vim_ਖਿੜਕੀ->ਤਾਲਾ_2();

    ਜੇ (tab_ਸਤਰ->Selection() != nr -1)
	tab_ਸਤਰ->Select(nr -1);

    gui.vim_ਖਿੜਕੀ->Unlock();
}

#ਜੇ_ਅੰਤ // FEAT_GUI_TABLINE
