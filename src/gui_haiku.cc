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

ବାହ୍ୟ "C" {

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <float.h>
#ଅନ୍ତର୍ଭାଵ <syslog.h>

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#ଅନ୍ତର୍ଭାଵ <Application.h>
#ଅନ୍ତର୍ଭାଵ <Beep.h>
#ଅନ୍ତର୍ଭାଵ <Bitmap.h>
#ଅନ୍ତର୍ଭାଵ <Box.h>
#ଅନ୍ତର୍ଭାଵ <Button.h>
#ଅନ୍ତର୍ଭାଵ <Clipboard.h>
#ଅନ୍ତର୍ଭାଵ <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#ଅନ୍ତର୍ଭାଵ <File.h>
#ଅନ୍ତର୍ଭାଵ <FilePanel.h>
#ଅନ୍ତର୍ଭାଵ <FindDirectory.h>
//#include <Font.h>
#ଅନ୍ତର୍ଭାଵ <IconUtils.h>
#ଅନ୍ତର୍ଭାଵ <Input.h>
#ଅନ୍ତର୍ଭାଵ <ListView.h>
#ଅନ୍ତର୍ଭାଵ <MenuBar.h>
#ଅନ୍ତର୍ଭାଵ <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#ଅନ୍ତର୍ଭାଵ <Path.h>
#ଅନ୍ତର୍ଭାଵ <PictureButton.h>
#ଅନ୍ତର୍ଭାଵ <PopUpMenu.h>
//#include <Region.h>
#ଅନ୍ତର୍ଭାଵ <Resources.h>
//#include <Roster.h>
#ଅନ୍ତର୍ଭାଵ <Screen.h>
#ଅନ୍ତର୍ଭାଵ <ScrollBar.h>
#ଅନ୍ତର୍ଭାଵ <ScrollView.h>
#ଅନ୍ତର୍ଭାଵ <String.h>
#ଅନ୍ତର୍ଭାଵ <StringView.h>
//#include <SupportDefs.h>
#ଅନ୍ତର୍ଭାଵ <TabView.h>
#ଅନ୍ତର୍ଭାଵ <TextControl.h>
#ଅନ୍ତର୍ଭାଵ <TextView.h>
#ଅନ୍ତର୍ଭାଵ <TranslationUtils.h>
#ଅନ୍ତର୍ଭାଵ <TranslatorFormats.h>
#ଅନ୍ତର୍ଭାଵ <View.h>
#ଅନ୍ତର୍ଭାଵ <Window.h>

ଶ୍ରେଣୀ VimApp;
ଶ୍ରେଣୀ VimFormView;
ଶ୍ରେଣୀ VimTextAreaView;
ଶ୍ରେଣୀ VimWindow;
ଶ୍ରେଣୀ VimToolbar;
ଶ୍ରେଣୀ Vim_Tab_ଧାଡ଼ି;

ବାହ୍ୟ ଚାବି_map *ଚାବି_Map;
ବାହ୍ୟ ଅକ୍ଷର *ଚାବି_Map_Chars;

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv);

#ifndef B_MAX_PORT_ଗଣନା
#define B_MAX_PORT_ଗଣନା    255
#endif

// VimApp seems comparable to the X "vimShell"
ଶ୍ରେଣୀ VimApp: ସର୍ବସାଧାରଣ BApplication
{
    ପ୍ରକାରବ୍ୟାଖ୍ୟା BApplication Inherited;
    ସର୍ବସାଧାରଣ:
    VimApp(ଅଚଳ ଅକ୍ଷର *appsig);
    ~VimApp();

    // callbacks:
#ଯଦି 0
    ଆଭାସୀ ଶୂନ୍ୟ DispatchMessage(BMessage *m, BHandler *h)
    {
	m->PrintToStream();
	Inherited::DispatchMessage(m, h);
    }
#endif
    ଆଭାସୀ ଶୂନ୍ୟ ReadyToRun();
    ଆଭାସୀ ଶୂନ୍ୟ ArgvReceived(int32 argc, ଅକ୍ଷର **argv);
    ଆଭାସୀ ଶୂନ୍ୟ RefsReceived(BMessage *m);
    ଆଭାସୀ ତାର୍କିକ QuitRequested();
    ଆଭାସୀ ଶୂନ୍ୟ MessageReceived(BMessage *m);

    ସ୍ଥିର ଶୂନ୍ୟ SendRefs(BMessage *m, ତାର୍କିକ changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	fBrowsedPath;
    ବ୍ୟକ୍ତିଗତ:
};

ଶ୍ରେଣୀ VimWindow: ସର୍ବସାଧାରଣ BWindow
{
    ପ୍ରକାରବ୍ୟାଖ୍ୟା BWindow Inherited;
    ସର୍ବସାଧାରଣ:
    VimWindow();
    ~VimWindow();

    ଆଭାସୀ ଶୂନ୍ୟ DispatchMessage(BMessage *m, BHandler *h);
    ଆଭାସୀ ଶୂନ୍ୟ WindowActivated(ତାର୍କିକ active);
    ଆଭାସୀ ତାର୍କିକ QuitRequested();

    VimFormView	    *formView;

    ବ୍ୟକ୍ତିଗତ:
    ଶୂନ୍ୟ init();

    ତାର୍କିକ is_fullscreen = ମିଥ୍ୟା;
    BRect saved_frame;
    window_look saved_look;
};

ଶ୍ରେଣୀ VimFormView: ସର୍ବସାଧାରଣ BView
{
    ପ୍ରକାରବ୍ୟାଖ୍ୟା BView Inherited;
    ସର୍ବସାଧାରଣ:
    VimFormView(BRect frame);
    ~VimFormView();

    // callbacks:
    ଆଭାସୀ ଶୂନ୍ୟ AllAttached();
    ଆଭାସୀ ଶୂନ୍ୟ FrameResized(ଦଶମିକ new_width, ଦଶମିକ new_height);

#define MENUBAR_MARGIN	1
    ଦଶମିକ MenuHeight() ଅଚଳ
    { ଫେରନ୍ତୁ menuBar ? menuBar->Frame().Height() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() ଅଚଳ
    { ଫେରନ୍ତୁ menuBar; }

    ବ୍ୟକ୍ତିଗତ:
    ଶୂନ୍ୟ init(BRect);

    BMenuBar	    *menuBar;
    VimTextAreaView *textArea;

#ifdef FEAT_TOOLBAR
    ସର୍ବସାଧାରଣ:
    ଦଶମିକ ToolbarHeight() ଅଚଳ;
    VimToolbar *ToolBar() ଅଚଳ
    { ଫେରନ୍ତୁ toolBar; }
    ବ୍ୟକ୍ତିଗତ:
    VimToolbar	    *toolBar;
#endif

#ifdef FEAT_GUI_TABLINE
    ସର୍ବସାଧାରଣ:
    Vim_Tab_ଧାଡ଼ି *Tab_ଧାଡ଼ି() ଅଚଳ	{ ଫେରନ୍ତୁ tab_ଧାଡ଼ି; }
    ତାର୍କିକ Is_Showing_Tab_ଧାଡ଼ି() ଅଚଳ { ଫେରନ୍ତୁ showing_Tab_ଧାଡ଼ି; }
    ଶୂନ୍ୟ Set_Showing_Tab_ଧାଡ଼ି(ତାର୍କିକ showing) { showing_Tab_ଧାଡ଼ି = showing;	}
    ଦଶମିକ TablineHeight() ଅଚଳ;
    ବ୍ୟକ୍ତିଗତ:
    Vim_Tab_ଧାଡ଼ି	*tab_ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	showing_Tab_ଧାଡ଼ି;
#endif
};

ଶ୍ରେଣୀ VimTextAreaView: ସର୍ବସାଧାରଣ BView
{
    ପ୍ରକାରବ୍ୟାଖ୍ୟା BView Inherited;
    ସର୍ବସାଧାରଣ:
    VimTextAreaView(BRect frame);
    ~VimTextAreaView();

    // callbacks:
    ଆଭାସୀ ଶୂନ୍ୟ Draw(BRect updateRect);
    ଆଭାସୀ ଶୂନ୍ୟ ଚାବି_Down(ଅଚଳ ଅକ୍ଷର *bytes, int32 ଅଙ୍କ_Bytes);
    ଆଭାସୀ ଶୂନ୍ୟ MouseDown(BPoint point);
    ଆଭାସୀ ଶୂନ୍ୟ MouseUp(BPoint point);
    ଆଭାସୀ ଶୂନ୍ୟ MouseMoved(BPoint point, uint32 transit, ଅଚଳ BMessage *message);
    ଆଭାସୀ ଶୂନ୍ୟ MessageReceived(BMessage *m);

    // own functions:
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mchInitFont(char_u *name);
    ଶୂନ୍ୟ mchDrawString(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
    ଶୂନ୍ୟ mchClearBlock(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col2);
    ଶୂନ୍ୟ mchClearAll();
    ଶୂନ୍ୟ mch_Delete_ଧାଡ଼ିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ);
    ଶୂନ୍ୟ mch_ଭର୍ତ୍ତି_ଧାଡ଼ିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ);

    ସ୍ଥିର ଶୂନ୍ୟ guiSendMouseEvent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା button, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ପୂର୍ଣ୍ଣସଂଖ୍ୟା repeated_click, int_u modifiers);
    ସ୍ଥିର ଶୂନ୍ୟ guiMouseMoved(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y);
    ସ୍ଥିର ଶୂନ୍ୟ guiBlankMouse(ତାର୍କିକ should_hide);
    ସ୍ଥିର int_u mouseModifiersToVim(int32 beModifiers);

    int32 mouse_Drag_Event_ଗଣନା;

#ifdef FEAT_MBYTE_IME
    ଶୂନ୍ୟ DrawIMString(ଶୂନ୍ୟ);
#endif

    ବ୍ୟକ୍ତିଗତ:
    ଶୂନ୍ୟ init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#ifdef FEAT_MBYTE_IME
    ସଂରଚନା {
	BMessenger* messenger;
	BMessage* message;
	BPoint location;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା row;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା col;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2;
    } IMData;
#endif
};

ଶ୍ରେଣୀ VimScrollBar: ସର୍ବସାଧାରଣ BScrollBar
{
    ପ୍ରକାରବ୍ୟାଖ୍ୟା BScrollBar Inherited;
    ସର୍ବସାଧାରଣ:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    ଆଭାସୀ ଶୂନ୍ୟ ମୂଲ୍ୟ_Changed(ଦଶମିକ new_ମୂଲ୍ୟ);
    ଆଭାସୀ ଶୂନ୍ୟ MouseUp(BPoint where);
    ଶୂନ୍ୟ Set_ମୂଲ୍ୟ(ଦଶମିକ newval);
    scrollbar_T *getGsb()
    { ଫେରନ୍ତୁ gsb; }

    int32	scroll_Event_ଗଣନା;

    ବ୍ୟକ୍ତିଗତ:
    scrollbar_T *gsb;
    ଦଶମିକ   ignore_ମୂଲ୍ୟ;
};


#ifdef FEAT_TOOLBAR

ଶ୍ରେଣୀ VimToolbar : ସର୍ବସାଧାରଣ BBox
{
    ସ୍ଥିର BBitmap *normalButtonsBitmap;
    ସ୍ଥିର BBitmap *grayedButtonsBitmap;

    BBitmap *LoadVimBitmap(ଅଚଳ ଅକ୍ଷର* fileName);
    ତାର୍କିକ GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, ତାର୍କିକ pressed);
    ତାର୍କିକ ModifyBitmapToGrayed(BBitmap *bitmap);

    BList fButtonsList;
    ଶୂନ୍ୟ InvalidateLayout();

    ସର୍ବସାଧାରଣ:
    VimToolbar(BRect frame, ଅଚଳ ଅକ୍ଷର * name);
    ~VimToolbar();

    ତାର୍କିକ PrepareButtonBitmaps();

    ତାର୍କିକ AddButton(int32 index, vimmenu_T *menu);
    ତାର୍କିକ RemoveButton(vimmenu_T *menu);
    ତାର୍କିକ GrayButton(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା grey);

    ଦଶମିକ ToolbarHeight() ଅଚଳ;
    ଆଭାସୀ ଶୂନ୍ୟ AttachedToWindow();
};

BBitmap *VimToolbar::normalButtonsBitmap  = ଖାଲି;
BBitmap *VimToolbar::grayedButtonsBitmap  = ଖାଲି;

ଅଚଳ ଦଶମିକ ToolbarMargin = 3.;
ଅଚଳ ଦଶମିକ ButtonMargin  = 3.;

#endif //FEAT_TOOLBAR

#ifdef FEAT_GUI_TABLINE

ଶ୍ରେଣୀ Vim_Tab_ଧାଡ଼ି : ସର୍ବସାଧାରଣ BTabView
{
    ସର୍ବସାଧାରଣ:
	ଶ୍ରେଣୀ VimTab : ସର୍ବସାଧାରଣ BTab {
	    ସର୍ବସାଧାରଣ:
		VimTab() : BTab(ନୂତନ BView(BRect(), "-Empty-", 0, 0)) {}

	    ଆଭାସୀ ଶୂନ୍ୟ Select(BView* owner);
	};

	Vim_Tab_ଧାଡ଼ି(BRect r) : BTabView(r, "vimTabLine", B_WIDTH_FROM_LABEL,
	       B_FOLLOW_ବାମ | B_FOLLOW_ଶୀର୍ଷ | B_FOLLOW_ଡାହାଣ, B_WILL_DRAW | B_FRAME_EVENTS) {}

    ଦଶମିକ TablineHeight() ଅଚଳ;
    ଆଭାସୀ ଶୂନ୍ୟ MouseDown(BPoint point);
};

#endif //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
ଶ୍ରେଣୀ VimFont: ସର୍ବସାଧାରଣ BFont
{
    ପ୍ରକାରବ୍ୟାଖ୍ୟା BFont Inherited;
    ସର୍ବସାଧାରଣ:
    VimFont();
    VimFont(ଅଚଳ VimFont *rhs);
    VimFont(ଅଚଳ BFont *rhs);
    VimFont(ଅଚଳ VimFont &rhs);
    ~VimFont();

    VimFont *next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା refcount;
    char_u *name;

    ବ୍ୟକ୍ତିଗତ:
    ଶୂନ୍ୟ init();
};

#ଯଦି defined(FEAT_GUI_DIALOG)

ଶ୍ରେଣୀ VimDialog : ସର୍ବସାଧାରଣ BWindow
{
    ପ୍ରକାରବ୍ୟାଖ୍ୟା BWindow Inherited;

    BButton* _CreateButton(int32 which, ଅଚଳ ଅକ୍ଷର* label);

    ସର୍ବସାଧାରଣ:

    ଶ୍ରେଣୀ View : ସର୍ବସାଧାରଣ BView {
	ପ୍ରକାରବ୍ୟାଖ୍ୟା BView Inherited;

	ସର୍ବସାଧାରଣ:
	View(BRect frame);
	~View();

	ଆଭାସୀ ଶୂନ୍ୟ Draw(BRect updateRect);
	ଶୂନ୍ୟ InitIcon(int32 type);

	ବ୍ୟକ୍ତିଗତ:
	BBitmap*    fIconBitmap;
    };

    VimDialog(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଅଚଳ ଅକ୍ଷର *title, ଅଚଳ ଅକ୍ଷର *message,
	    ଅଚଳ ଅକ୍ଷର *buttons, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dfltbutton, ଅଚଳ ଅକ୍ଷର *textfield,
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ex_cmd);
    ~VimDialog();

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା Go();

    ଆଭାସୀ ଶୂନ୍ୟ MessageReceived(BMessage *ସନ୍ଦେଶ);

    ବ୍ୟକ୍ତିଗତ:
    sem_id	    fDialogSem;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    f_Dialog_ମୂଲ୍ୟ;
    BList	    fButtonsList;
    BTextView*	    fMessageView;
    BTextControl*   f_ନିବେଶ_Control;
    ଅଚଳ ଅକ୍ଷର*	    f_ନିବେଶ_ମୂଲ୍ୟ;
};

ଶ୍ରେଣୀ VimSelectFontDialog : ସର୍ବସାଧାରଣ BWindow
{
    ପ୍ରକାରବ୍ୟାଖ୍ୟା BWindow Inherited;

    ଶୂନ୍ୟ _CleanList(BListView* list);
    ଶୂନ୍ୟ _UpdateFontStyles();
    ଶୂନ୍ୟ Update_Size_ନିବେଶ_Preview();
    ଶୂନ୍ୟ _UpdateFontPreview();
    ତାର୍କିକ _UpdateFromListItem(BListView* list, ଅକ୍ଷର* text, ପୂର୍ଣ୍ଣସଂଖ୍ୟା textSize);
    ସର୍ବସାଧାରଣ:

    VimSelectFontDialog(font_family* family, font_style* style, ଦଶମିକ* size);
    ~VimSelectFontDialog();

    ତାର୍କିକ Go();

    ଆଭାସୀ ଶୂନ୍ୟ MessageReceived(BMessage *ସନ୍ଦେଶ);

    ବ୍ୟକ୍ତିଗତ:
    status_t	    fStatus;
    sem_id	    fDialogSem;
    ତାର୍କିକ	    f_Dialog_ମୂଲ୍ୟ;
    font_family*    fFamily;
    font_style*	    fStyle;
    ଦଶମିକ*	    fSize;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    ଦଶମିକ	    fFontSize;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_ନିବେଶ;
};

#endif // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

ସଂରଚନା MainArgs {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	     argc;
    ଅକ୍ଷର    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#define	ଚାବି_ସନ୍ଦେଶ_BUFSIZ	7
#ଯଦି ଚାବି_ସନ୍ଦେଶ_BUFSIZ < MAX_ଚାବି_CODE_LEN
#error Increase ଚାବି_ସନ୍ଦେଶ_BUFSIZ!
#endif

ସଂରଚନା Vim_ଚାବି_ସନ୍ଦେଶ {
    char_u  length;
    char_u  chars[ଚାବି_ସନ୍ଦେଶ_BUFSIZ];  // contains Vim encoding
    ତାର୍କିକ    csi_escape;
};

ସଂରଚନା Vim_Resize_ସନ୍ଦେଶ {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    height;
};

ସଂରଚନା Vim_Scroll_Bar_ସନ୍ଦେଶ {
    VimScrollBar *sb;
    long    ମୂଲ୍ୟ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    stillDragging;
};

ସଂରଚନା Vim_Menu_ସନ୍ଦେଶ {
    vimmenu_T	*guiMenu;
};

ସଂରଚନା Vim_Mouse_ସନ୍ଦେଶ {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    button;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    x;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    y;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    repeated_click;
    int_u   modifiers;
};

ସଂରଚନା Vim_Mouse_Moved_ସନ୍ଦେଶ {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    x;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    y;
};

ସଂରଚନା Vim_Focus_ସନ୍ଦେଶ {
    ତାର୍କିକ    active;
};

ସଂରଚନା Vim_Refs_ସନ୍ଦେଶ {
    BMessage   *message;
    ତାର୍କିକ    changedir;
};

ସଂରଚନା Vim_Tabline_ସନ୍ଦେଶ {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    index;
};

ସଂରଚନା Vim_Tabline_Menu_ସନ୍ଦେଶ {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    index;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    event;
};

ସଂରଚନା Vim_ସନ୍ଦେଶ {
    ଗଣନା Vim_ସନ୍ଦେଶ_Type {
	ଚାବି_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    ସଂଘ {
	ସଂରଚନା Vim_ଚାବି_ସନ୍ଦେଶ    ଚାବି_2;
	ସଂରଚନା Vim_Resize_ସନ୍ଦେଶ NewSize;
	ସଂରଚନା Vim_Scroll_Bar_ସନ୍ଦେଶ	Scroll;
	ସଂରଚନା Vim_Menu_ସନ୍ଦେଶ   Menu;
	ସଂରଚନା Vim_Mouse_ସନ୍ଦେଶ  Mouse;
	ସଂରଚନା Vim_Mouse_Moved_ସନ୍ଦେଶ	MouseMoved;
	ସଂରଚନା Vim_Focus_ସନ୍ଦେଶ  Focus;
	ସଂରଚନା Vim_Refs_ସନ୍ଦେଶ   Refs;
	ସଂରଚନା Vim_Tabline_ସନ୍ଦେଶ	Tabline;
	ସଂରଚନା Vim_Tabline_Menu_ସନ୍ଦେଶ    TablineMenu;
    } u;
};

#define RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#define GUI_TO_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

ସ୍ଥିର ସଂରଚନା specialkey
{
    uint16  Be_ଚାବିଗୁଡ଼ିକ;
#define ଚାବି_3(a,b)    ((a)<<8|(b))
#define K(a)	    ଚାବି_3(0,a)		// for ASCII codes
#define F(b)	    ଚାବି_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_ଚାବିଗୁଡ଼ିକ[] =
{
    {K(B_UP_ARROW),	'k', 'u'},
    {K(B_DOWN_ARROW),	    'k', 'd'},
    {K(B_ବାମ_ARROW),	    'k', 'l'},
    {K(B_ଡାହାଣ_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_ଭର୍ତ୍ତି),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_END),		'@', '7'},
    {K(B_PAGE_UP),	'k', 'P'},	// XK_Prior
    {K(B_PAGE_DOWN),	    'k', 'N'},	    // XK_Next,

#define FIRST_FUNCTION_ଚାବି  11
    {F(B_F1_ଚାବି),	'k', '1'},
    {F(B_F2_ଚାବି),	'k', '2'},
    {F(B_F3_ଚାବି),	'k', '3'},
    {F(B_F4_ଚାବି),	'k', '4'},
    {F(B_F5_ଚାବି),	'k', '5'},
    {F(B_F6_ଚାବି),	'k', '6'},
    {F(B_F7_ଚାବି),	'k', '7'},
    {F(B_F8_ଚାବି),	'k', '8'},
    {F(B_F9_ଚାବି),	'k', '9'},
    {F(B_F10_ଚାବି),	'k', ';'},

    {F(B_F11_ଚାବି),	'F', '1'},
    {F(B_F12_ଚାବି),	'F', '2'},
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
    {F(B_PRINT_ଚାବି),	    '%', '9'},

#ଯଦି 0
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

#define ଅଙ୍କ_SPECIAL_ଚାବିଗୁଡ଼ିକ    ARRAY_LENGTH(special_ଚାବିଗୁଡ଼ିକ)

// ---------------- VimApp ----------------

    ସ୍ଥିର ଶୂନ୍ୟ
docd(BPath &path)
{
    mch_chdir((ଅକ୍ଷର *)path.Path());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	ସ୍ଥିର ଶୂନ୍ୟ
drop_callback(ଶୂନ୍ୟ *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	ସ୍ଥିର ଶୂନ୍ୟ
RefsReceived(BMessage *m, ତାର୍କିକ changedir)
{
    uint32 type;
    int32 ଗଣନା_2;

    m->PrintToStream();
    ବାଛ (m->what) {
	ସ୍ଥିତି B_REFS_RECEIVED:
	ସ୍ଥିତି B_SIMPLE_DATA:
	    m->GetInfo("refs", &type, &ଗଣନା_2);
	    ଯଦି (type != B_REF_TYPE)
		ଯାଅ bad;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &ଗଣନା_2);
	    ଯଦି (type != B_STRING_TYPE)
		ଯାଅ bad;
	    ଯଦି (changedir) {
		ଅକ୍ଷର *dirname;
		ଯଦି (m->FindString("cwd", (ଅଚଳ ଅକ୍ଷର **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    delete m;
	    ଫେରନ୍ତୁ;
    }

#ifdef FEAT_VISUAL
    reset_VIsual();
#endif

    char_u  **fnames;
    fnames = (char_u **) alloc(ଗଣନା_2 * ଆକାର(char_u *));
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fname_index = 0;

    ବାଛ (m->what) {
	ସ୍ଥିତି B_REFS_RECEIVED:
	ସ୍ଥିତି B_SIMPLE_DATA:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଗଣନା_2; ++i)
	    {
		entry_ref ref;
		ଯଦି (m->FindRef("refs", i, &ref) == B_OK) {
		    BEntry entry(&ref, ମିଥ୍ୟା);
		    BPath path;
		    entry.GetPath(&path);

		    // Change to parent directory?
		    ଯଦି (changedir) {
			BPath parentpath;
			path.GetParent(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    ଯଦି (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			ଯଦି (!changedir)
			    docd(path);
		    } ଅନ୍ୟଥା {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)path.Path(), IObuff);
			ଯଦି (fname == ଖାଲି)
			    fname = (char_u *)path.Path();
			fnames[fname_index++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = ମିଥ୍ୟା;
		}
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < ଗଣନା_2; ++i)
	    {
		ଅକ୍ଷର *fname;

		ଯଦି (m->FindString("argv", i, (ଅଚଳ ଅକ୍ଷର **) &fname) == B_OK) {
		    fnames[fname_index++] = vim_strsave((char_u *)fname);
		}
	    }
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    // fprintf(stderr, "case default\n");
	    ଭାଙ୍ଗ;
    }

    delete m;

    // Handle the drop, :edit to get to the file
    ଯଦି (fname_index > 0) {
	handle_drop(fname_index, fnames, FALSE, drop_callback, ଖାଲି);

	setcursor();
	out_flush();
    } ଅନ୍ୟଥା {
	vim_free(fnames);
    }
}

VimApp::VimApp(ଅଚଳ ଅକ୍ଷର *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(ଖାଲି)
{
}

VimApp::~VimApp()
{
}

    ଶୂନ୍ୟ
VimApp::ReadyToRun()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    ଶୂନ୍ୟ
VimApp::ArgvReceived(int32 arg_argc, ଅକ୍ଷର **arg_argv)
{
    ଯଦି (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	ଯଦି (gui.vimWindow)
	    gui.vimWindow->Minimize(ମିଥ୍ୟା);
	BMessage *m = CurrentMessage();
	DetachCurrentMessage();
	SendRefs(m, ସତ୍ୟ);
    }
}

    ଶୂନ୍ୟ
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା limit = 15;
    ଯେତେବେଳେ (--limit >= 0 && (curbuf == ଖାଲି || curbuf->b_start_ffc == 0))
	snooze(100000);    //  0.1 s
    ଯଦି (gui.vimWindow)
	gui.vimWindow->Minimize(ମିଥ୍ୟା);
    DetachCurrentMessage();
    SendRefs(m, ସତ୍ୟ);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    ଶୂନ୍ୟ
VimApp::SendRefs(BMessage *m, ତାର୍କିକ changedir)
{
    Vim_Refs_ସନ୍ଦେଶ rm;
    rm.message = m;
    rm.changedir = changedir;

    write_port(gui.vdcmp, Vim_ସନ୍ଦେଶ::Refs, &rm, ଆକାର(rm));
    //	calls ::RefsReceived
}

    ଶୂନ୍ୟ
VimApp::MessageReceived(BMessage *m)
{
    ବାଛ (m->what) {
	ସ୍ଥିତି 'save':
	    {
		entry_ref refDirectory;
		m->FindRef("directory", &refDirectory);
		fBrowsedPath.SetTo(&refDirectory);
		BString strName;
		m->FindString("name", &strName);
		fBrowsedPath.Append(strName.String());
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'open':
	    {
		entry_ref ref;
		m->FindRef("refs", &ref);
		fBrowsedPath.SetTo(&ref);
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି B_CANCEL:
	    {
		BFilePanel *panel;
		m->FindPointer("source", (ଶୂନ୍ୟ**)&panel);
		ଯଦି (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    Inherited::MessageReceived(m);
	    ଭାଙ୍ଗ;
    }
}

    ତାର୍କିକ
VimApp::QuitRequested()
{
    (ଶୂନ୍ୟ)Inherited::QuitRequested();
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
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
    ଯଦି (formView) {
	RemoveChild(formView);
	delete formView;
    }
    gui.vimWindow = ଖାଲି;
}

    ଶୂନ୍ୟ
VimWindow::init()
{
    // Attach the VimFormView
    formView = ନୂତନ VimFormView(Bounds());
    ଯଦି (formView != ଖାଲି) {
	AddChild(formView);
    }
}

#ଯଦି 0  //  disabled in zeta patch
    ଶୂନ୍ୟ
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    ବାଛ (m->what) {
	ସ୍ଥିତି B_MOUSE_UP:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		ଯଦି (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->FindPoint("where", &where);
		    v->MouseUp(where);
		} ଅନ୍ୟଥା {
		    Inherited::DispatchMessage(m, h);
		}
	    }
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    Inherited::DispatchMessage(m, h);
    }
}
#endif

    ଶୂନ୍ୟ
VimWindow::WindowActivated(ତାର୍କିକ active)
{
    Inherited::WindowActivated(active);
    // the textArea gets the keyboard action
    ଯଦି (active && gui.vimTextArea)
	gui.vimTextArea->MakeFocus(ସତ୍ୟ);

    ସଂରଚନା Vim_Focus_ସନ୍ଦେଶ fm;
    fm.active = active;

    write_port(gui.vdcmp, Vim_ସନ୍ଦେଶ::Focus, &fm, ଆକାର(fm));
}

    ତାର୍କିକ
VimWindow::QuitRequested()
{
    ସଂରଚନା Vim_ଚାବି_ସନ୍ଦେଶ km;
    km.length = 5;
    memcpy((ଅକ୍ଷର *)km.chars, "\033:qa\r", km.length);
    km.csi_escape = ମିଥ୍ୟା;
    write_port(gui.vdcmp, Vim_ସନ୍ଦେଶ::ଚାବି_2, &km, ଆକାର(km));
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}
    ଶୂନ୍ୟ
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    ତାର୍କିକ should_propagate = ସତ୍ୟ;

    ବାଛ (m->what)
    {
	ସ୍ଥିତି B_ଚାବି_DOWN:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->FindInt32("raw_char", &scancode);
		m->FindInt32("modifiers", &beModifiers);

		ଯଦି (scancode == B_ENTER && (beModifiers & B_ବାମ_COMMAND_ଚାବି))
		    {
			should_propagate = ମିଥ୍ୟା;
			ଯଦି (ଏହି->is_fullscreen)
			    {
				ଏହି->is_fullscreen = ମିଥ୍ୟା;
				ResizeTo(ଏହି->saved_frame.Width(), ଏହି->saved_frame.Height());
				MoveTo(ଏହି->saved_frame.ବାମ, ଏହି->saved_frame.ଶୀର୍ଷ);
				SetLook(ଏହି->saved_look);
				SetFlags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } ଅନ୍ୟଥା {
				ଏହି->saved_frame = Frame();
				ଏହି->saved_look = Look();
				ଏହି->is_fullscreen = ସତ୍ୟ;
				BScreen s(ଏହି);
				SetLook(B_NO_BORDER_WINDOW_LOOK);
				ResizeTo(s.Frame().Width() + 1, s.Frame().Height() + 1);
				MoveTo(s.Frame().ବାମ, s.Frame().ଶୀର୍ଷ);
				SetFlags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    ଯଦି (should_propagate)
	Inherited::DispatchMessage(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect frame):
    BView(frame, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_FRAME_EVENTS),
    menuBar(ଖାଲି),
#ifdef FEAT_TOOLBAR
    toolBar(ଖାଲି),
#endif
#ifdef FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_ଧାଡ଼ି(ଖାଲି),
#endif
    textArea(ଖାଲି)
{
    init(frame);
}

VimFormView::~VimFormView()
{
    ଯଦି (menuBar) {
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
    delete tab_ଧାଡ଼ି;
#endif

    ଯଦି (textArea) {
	RemoveChild(textArea);
	delete textArea;
    }
    gui.vimForm = ଖାଲି;
}

    ଶୂନ୍ୟ
VimFormView::init(BRect frame)
{
    menuBar = ନୂତନ BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    AddChild(menuBar);

#ifdef FEAT_TOOLBAR
    toolBar = ନୂତନ VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    AddChild(toolBar);
#endif

#ifdef FEAT_GUI_TABLINE
    tab_ଧାଡ଼ି = ନୂତନ Vim_Tab_ଧାଡ଼ି(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    AddChild(tab_ଧାଡ଼ି);
#endif

    BRect remaining = frame;
    textArea = ନୂତନ VimTextAreaView(remaining);
    AddChild(textArea);
    // The textArea will be resized later when menus are added

    gui.vimForm = ଏହି;
}

#ifdef FEAT_TOOLBAR
    ଦଶମିକ
VimFormView::ToolbarHeight() ଅଚଳ
{
    ଫେରନ୍ତୁ toolBar ? toolBar->ToolbarHeight() : 0.;
}
#endif

#ifdef FEAT_GUI_TABLINE
    ଦଶମିକ
VimFormView::TablineHeight() ଅଚଳ
{
    ଫେରନ୍ତୁ (tab_ଧାଡ଼ି && Is_Showing_Tab_ଧାଡ଼ି()) ? tab_ଧାଡ଼ି->TablineHeight() : 0.;
}
#endif

    ଶୂନ୍ୟ
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    ଯଦି (menuBar && textArea) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect remaining = Bounds();

#ifdef FEAT_MENU
	remaining.ଶୀର୍ଷ += MenuHeight();
	menuBar->ResizeTo(remaining.ଡାହାଣ, remaining.ଶୀର୍ଷ);
	gui.menu_height = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) MenuHeight();
#endif

#ifdef FEAT_TOOLBAR
	toolBar->MoveTo(remaining.ବାମ, remaining.ଶୀର୍ଷ);
	toolBar->ResizeTo(remaining.ଡାହାଣ, ToolbarHeight());
	remaining.ଶୀର୍ଷ += ToolbarHeight();
	gui.toolbar_height = ToolbarHeight();
#endif

#ifdef FEAT_GUI_TABLINE
	tab_ଧାଡ଼ି->MoveTo(remaining.ବାମ, remaining.ଶୀର୍ଷ);
	tab_ଧାଡ଼ି->ResizeTo(remaining.ଡାହାଣ + 1, TablineHeight());
	remaining.ଶୀର୍ଷ += TablineHeight();
	gui.tabline_height = TablineHeight();
#endif

	textArea->ResizeTo(remaining.Width(), remaining.Height());
	textArea->MoveTo(remaining.ବାମ, remaining.ଶୀର୍ଷ);
    }


    Inherited::AllAttached();
}

    ଶୂନ୍ୟ
VimFormView::FrameResized(ଦଶମିକ new_width, ଦଶମିକ new_height)
{
    ସଂରଚନା Vim_Resize_ସନ୍ଦେଶ sm;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା adjust_h, adjust_w;

    new_width += 1;	//  adjust from width to number of pixels occupied
    new_height += 1;

    sm.width = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) new_width;
    sm.height = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) new_height;
    adjust_w = ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)new_width - gui_get_base_width()) % gui.char_width;
    adjust_h = ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)new_height - gui_get_base_height()) % gui.char_height;

    ଯଦି (adjust_w > 0 || adjust_h > 0) {
	sm.width  -= adjust_w;
	sm.height -= adjust_h;
    }

    write_port(gui.vdcmp, Vim_ସନ୍ଦେଶ::Resize, &sm, ଆକାର(sm));
    //	calls gui_resize_shell(new_width, new_height);

    ଫେରନ୍ତୁ;

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
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_ନିବେଶ_METHOD_AWARE
#ଅନ୍ୟଥା
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE
#endif
	),
    mouse_Drag_Event_ଗଣନା(0)
{
#ifdef FEAT_MBYTE_IME
    IMData.messenger = ଖାଲି;
    IMData.message = ଖାଲି;
#endif
    init(frame);
}

VimTextAreaView::~VimTextAreaView()
{
    gui.vimTextArea = ଖାଲି;
}

    ଶୂନ୍ୟ
VimTextAreaView::init(BRect frame)
{
    // set up global var for fast access
    gui.vimTextArea = ଏହି;

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

    ଶୂନ୍ୟ
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
    Set_ତଳ_Color(rgb);
    FillRect(updateRect, B_SOLID_ତଳ);
    gui_redraw((ପୂର୍ଣ୍ଣସଂଖ୍ୟା) updateRect.ବାମ, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) updateRect.ଶୀର୍ଷ,
	    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) (updateRect.Width() + PEN_WIDTH), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) (updateRect.Height() + PEN_WIDTH));

    // Clear the border areas if needed
    Set_ତଳ_Color(rgb);

    ଯଦି (updateRect.ବାମ < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.ବାମ, updateRect.ଶୀର୍ଷ,
		    FILL_X(0)-PEN_WIDTH, updateRect.bottom), B_SOLID_ତଳ);
    ଯଦି (updateRect.ଶୀର୍ଷ < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.ବାମ, updateRect.ଶୀର୍ଷ,
		    updateRect.ଡାହାଣ, FILL_Y(0)-PEN_WIDTH), B_SOLID_ତଳ);
    ଯଦି (updateRect.ଡାହାଣ >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns), updateRect.ଶୀର୍ଷ,
		    updateRect.ଡାହାଣ, updateRect.bottom), B_SOLID_ତଳ);
    ଯଦି (updateRect.bottom >= FILL_Y(Rows))   //  bottom border
	FillRect(BRect(updateRect.ବାମ, FILL_Y((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows),
		    updateRect.ଡାହାଣ, updateRect.bottom), B_SOLID_ତଳ);

#ifdef FEAT_MBYTE_IME
    DrawIMString();
#endif
}

    ଶୂନ୍ୟ
VimTextAreaView::ଚାବି_Down(ଅଚଳ ଅକ୍ଷର *bytes, int32 ଅଙ୍କ_Bytes)
{
    ସଂରଚନା Vim_ଚାବି_ସନ୍ଦେଶ km;
    char_u *dest = km.chars;

    ତାର୍କିକ canHaveVimModifiers = ମିଥ୍ୟା;

    BMessage *ସନ୍ଦେଶ = Window()->CurrentMessage();
    ପ୍ରତିଜ୍ଞା(ସନ୍ଦେଶ);
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
    ଯଦି (ଅଙ୍କ_Bytes > 1) {
	// This cannot be a special key
	ଯଦି (ଅଙ୍କ_Bytes > ଚାବି_ସନ୍ଦେଶ_BUFSIZ)
	    ଅଙ୍କ_Bytes = ଚାବି_ସନ୍ଦେଶ_BUFSIZ;	    //	should never happen... ???
	km.length = ଅଙ୍କ_Bytes;
	memcpy((ଅକ୍ଷର *)dest, bytes, ଅଙ୍କ_Bytes);
	km.csi_escape = ସତ୍ୟ;
    } ଅନ୍ୟଥା {
	int32 scancode = 0;
	ସନ୍ଦେଶ->FindInt32("key", &scancode);

	int32 beModifiers = 0;
	ସନ୍ଦେଶ->FindInt32("modifiers", &beModifiers);

	char_u string[3];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = 0;
	km.length = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	ପ୍ରତିଜ୍ଞା(B_BACKSPACE <= 0x20);
	ପ୍ରତିଜ୍ଞା(B_DELETE == 0x7F);
	ଯଦି (((char_u)bytes[0] <= 0x20 || (char_u)bytes[0] == 0x7F) &&
		ଅଙ୍କ_Bytes == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    ଯଦି (beModifiers & B_CONTROL_ଚାବି) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = ଚାବି_Map->normal_map[scancode];
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_ଅଙ୍କ_Bytes = ଚାବି_Map_Chars[index];
		char_u *newBytes = (char_u *)&ଚାବି_Map_Chars[index + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		ଯଦି (new_ଅଙ୍କ_Bytes != 1 || (*newBytes > 0x20 &&
			    *newBytes != 0x7F )) {
		    ଯାଅ notspecial;
		}
		bytes = (ଅକ୍ଷର *)newBytes;
	    }
	    canHaveVimModifiers = ସତ୍ୟ;

	    uint16 beoskey;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା first, last;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    ଯଦି (ଅଙ୍କ_Bytes == 0 || bytes[0] == B_FUNCTION_ଚାବି) {
		beoskey = F(scancode);
		first = FIRST_FUNCTION_ଚାବି;
		last = ଅଙ୍କ_SPECIAL_ଚାବିଗୁଡ଼ିକ;
	    } ଅନ୍ୟଥା ଯଦି (*bytes == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		first = last = 0;
	    } ଅନ୍ୟଥା {
		beoskey = K(bytes[0]);
		first = 0;
		last = FIRST_FUNCTION_ଚାବି;
	    }

	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = first; i < last; i++) {
		ଯଦି (special_ଚାବିଗୁଡ଼ିକ[i].Be_ଚାବିଗୁଡ଼ିକ == beoskey) {
		    string[0] = CSI;
		    string[1] = special_ଚାବିଗୁଡ଼ିକ[i].vim_code0;
		    string[2] = special_ଚାବିଗୁଡ଼ିକ[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	ଯଦି (len == 0) {
	    string[0] = bytes[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#ଯଦି 0
	ଯଦି (len == 3 ||
		bytes[0] == B_SPACE || bytes[0] == B_TAB ||
		bytes[0] == B_RETURN || bytes[0] == '\r' ||
		bytes[0] == B_ESCAPE)
#ଅନ୍ୟଥା
	    ଯଦି (canHaveVimModifiers)
#endif
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା modifiers;
		modifiers = 0;
		ଯଦି (beModifiers & B_SHIFT_ଚାବି)
		    modifiers |= MOD_MASK_SHIFT;
		ଯଦି (beModifiers & B_CONTROL_ଚାବି)
		    modifiers |= MOD_MASK_CTRL;
		ଯଦି (beModifiers & B_OPTION_ଚାବି)
		    modifiers |= MOD_MASK_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି;
		ଯଦି (string[0] == CSI && len == 3)
		    ଚାବି = TO_SPECIAL(string[1], string[2]);
		ଅନ୍ୟଥା
		    ଚାବି = string[0];
		ଚାବି = simplify_ଚାବି(ଚାବି, &modifiers);
		ଯଦି (IS_SPECIAL(ଚାବି))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(ଚାବି);
		    string[2] = K_THIRD(ଚାବି);
		    len = 3;
		}
		ଅନ୍ୟଥା
		{
		    string[0] = ଚାବି;
		    len = 1;
		}

		ଯଦି (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.length = 3;
		}
	    }
	memcpy((ଅକ୍ଷର *)dest, string, len);
	km.length += len;
	km.csi_escape = ମିଥ୍ୟା;
    }

    write_port(gui.vdcmp, Vim_ସନ୍ଦେଶ::ଚାବି_2, &km, ଆକାର(km));

    /*
     * blank out the pointer if necessary
     */
    ଯଦି (p_mh && !gui.pointer_hidden)
    {
	guiBlankMouse(ସତ୍ୟ);
	gui.pointer_hidden = TRUE;
    }
}
ଶୂନ୍ୟ
VimTextAreaView::guiSendMouseEvent(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	button,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	x,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	y,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_ସନ୍ଦେଶ mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_port(gui.vdcmp, Vim_ସନ୍ଦେଶ::Mouse, &mm, ଆକାର(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    ଯଦି (gui.pointer_hidden)
    {
	guiBlankMouse(ମିଥ୍ୟା);
	gui.pointer_hidden = FALSE;
    }
}

ଶୂନ୍ୟ
VimTextAreaView::guiMouseMoved(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	x,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	y)
{
    Vim_Mouse_Moved_ସନ୍ଦେଶ mm;

    mm.x = x;
    mm.y = y;

    write_port(gui.vdcmp, Vim_ସନ୍ଦେଶ::MouseMoved, &mm, ଆକାର(mm));

    ଯଦି (gui.pointer_hidden)
    {
	guiBlankMouse(ମିଥ୍ୟା);
	gui.pointer_hidden = FALSE;
    }
}

    ଶୂନ୍ୟ
VimTextAreaView::guiBlankMouse(ତାର୍କିକ should_hide)
{
    ଯଦି (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } ଅନ୍ୟଥା {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
VimTextAreaView::mouseModifiersToVim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    ଯଦି (beModifiers & B_SHIFT_ଚାବି)
	vim_modifiers |= MOUSE_SHIFT;
    ଯଦି (beModifiers & B_CONTROL_ଚାବି)
	vim_modifiers |= MOUSE_CTRL;
    ଯଦି (beModifiers & B_OPTION_ଚାବି)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    ଫେରନ୍ତୁ vim_modifiers;
}

    ଶୂନ୍ୟ
VimTextAreaView::MouseDown(BPoint point)
{
    BMessage *m = Window()->CurrentMessage();
    ପ୍ରତିଜ୍ଞା(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vimButton;

    ଯଦି (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_ବାମ;
    ଅନ୍ୟଥା ଯଦି (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_ଡାହାଣ;
    ଅନ୍ୟଥା ଯଦି (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_ମଧ୍ୟ;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ;		// Unknown button

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

    ଶୂନ୍ୟ
VimTextAreaView::MouseUp(BPoint point)
{
    vimMouseButton = 0;

    BMessage *m = Window()->CurrentMessage();
    ପ୍ରତିଜ୍ଞା(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->FindInt32("modifiers", &modifiers);

    vimMouseModifiers = mouseModifiersToVim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, point.x, point.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::MouseUp(point);
}

    ଶୂନ୍ୟ
VimTextAreaView::MouseMoved(BPoint point, uint32 transit, ଅଚଳ BMessage *message)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    ଯଦି (gui.pointer_hidden)
    {
	guiBlankMouse(ମିଥ୍ୟା);
	gui.pointer_hidden = FALSE;
    }

    ଯଦି (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(point.x, point.y);
	ଫେରନ୍ତୁ;
    }

    atomic_add(&mouse_Drag_Event_ଗଣନା, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, point.x, point.y, 0, vimMouseModifiers);
}

    ଶୂନ୍ୟ
VimTextAreaView::MessageReceived(BMessage *m)
{
    ବାଛ (m->what) {
	ସ୍ଥିତି 'menu':
	    {
		Vim_Menu_ସନ୍ଦେଶ mm;
		mm.guiMenu = ଖାଲି;  // in case no pointer in msg
		m->FindPointer("VimMenu", (ଶୂନ୍ୟ **)&mm.guiMenu);
		write_port(gui.vdcmp, Vim_ସନ୍ଦେଶ::Menu, &mm, ଆକାର(mm));
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି B_MOUSE_WHEEL_CHANGED:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		ଦଶମିକ small=0, big=0, dy=0;
		m->FindFloat("be:wheel_delta_y", &dy);
		scb->GetSteps(&small, &big);
		scb->Set_ମୂଲ୍ୟ(scb->ମୂଲ୍ୟ_2()+small*dy*3);
		scb->ମୂଲ୍ୟ_Changed(scb->ମୂଲ୍ୟ_2());
#ଯଦି 0
		scb = curwin->w_scrollbars[0].id;
		scb->GetSteps(&small, &big);
		scb->Set_ମୂଲ୍ୟ(scb->ମୂଲ୍ୟ_2()+small*dy);
		scb->ମୂଲ୍ୟ_Changed(scb->ମୂଲ୍ୟ_2());
#endif
	    }
	    ଭାଙ୍ଗ;
#ifdef FEAT_MBYTE_IME
	ସ୍ଥିତି B_ନିବେଶ_METHOD_EVENT:
	    {
		int32 opcode;
		m->FindInt32("be:opcode", &opcode);
		ବାଛ(opcode)
		{
		    ସ୍ଥିତି B_ନିବେଶ_METHOD_STARTED:
			ଯଦି (!IMData.messenger) delete IMData.messenger;
			IMData.messenger = ନୂତନ BMessenger();
			m->FindMessenger("be:reply_to", IMData.messenger);
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି B_ନିବେଶ_METHOD_CHANGED:
			{
			    BString str;
			    ତାର୍କିକ confirmed;
			    ଯଦି (IMData.message) *(IMData.message) = *m;
			    ଅନ୍ୟଥା	       IMData.message = ନୂତନ BMessage(*m);
			    DrawIMString();
			    m->FindBool("be:confirmed", &confirmed);
			    ଯଦି (confirmed)
			    {
				m->FindString("be:string", &str);
				char_u *chars = (char_u*)str.String();
				ସଂରଚନା Vim_ଚାବି_ସନ୍ଦେଶ km;
				km.csi_escape = ସତ୍ୟ;
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା clen;
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
				ଯେତେବେଳେ (i < str.Length())
				{
				    clen = utf_ptr2len(chars+i);
				    memcpy(km.chars, chars+i, clen);
				    km.length = clen;
				    write_port(gui.vdcmp, Vim_ସନ୍ଦେଶ::ଚାବି_2, &km, ଆକାର(km));
				    i += clen;
				}
			    }
			}
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି B_ନିବେଶ_METHOD_LOCATION_REQUEST:
			{
			    BMessage ସନ୍ଦେଶ(B_ନିବେଶ_METHOD_EVENT);
			    ସନ୍ଦେଶ.AddInt32("be:opcode", B_ନିବେଶ_METHOD_LOCATION_REQUEST);
			    ସନ୍ଦେଶ.AddPoint("be:location_reply", IMData.location);
			    ସନ୍ଦେଶ.AddFloat("be:height_reply", FILL_Y(1));
			    IMData.messenger->SendMessage(&ସନ୍ଦେଶ);
			}
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି B_ନିବେଶ_METHOD_STOPPED:
			delete IMData.messenger;
			delete IMData.message;
			IMData.messenger = ଖାଲି;
			IMData.message = ଖାଲି;
			ଭାଙ୍ଗ;
		}
	    }
	    // TODO: sz: break here???
#endif
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଯଦି (m->WasDropped()) {
		BWindow *w = Window();
		w->DetachCurrentMessage();
		w->Minimize(ମିଥ୍ୟା);
		VimApp::SendRefs(m, (modifiers() & B_SHIFT_ଚାବି) != 0);
	    } ଅନ୍ୟଥା {
		Inherited::MessageReceived(m);
	    }
	    ଭାଙ୍ଗ;
    }
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
VimTextAreaView::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    ଯଦି (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_set_font((GuiFont)newFont);
	ଯଦି (name && STRCMP(name, "*") != 0)
	    hl_set_font_name(name);

	SetDrawingMode(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	ଫେରନ୍ତୁ OK;
    }
    ଫେରନ୍ତୁ FAIL;
}

    ଶୂନ୍ୟ
VimTextAreaView::mchDrawString(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    ଯଦି (!(flags & DRAW_TRANSP)) {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cells;
	cells = 0;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<len; i++) {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cn = utf_ptr2cells((char_u *)(s+i));
	    ଯଦି (cn<4) cells += cn;
	}

	BRect r(FILL_X(col), FILL_Y(row),
		FILL_X(col + cells) - PEN_WIDTH, FILL_Y(row + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_ତଳ);
    }

    BFont font;
    ଏହି->GetFont(&font);
    ଯଦି (!font.IsFixed())
    {
	ଅକ୍ଷର* p = (ଅକ୍ଷର*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cells;
	ଯେତେବେଳେ ((p - (ଅକ୍ଷର*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.Set(TEXT_X(col+lastpos), TEXT_Y(row));
	    DrawString(p, clen, where);
	    ଯଦି (flags & DRAW_BOLD) {
		where.x += 1.0;
		SetDrawingMode(B_OP_BLEND);
		DrawString(p, clen, where);
		SetDrawingMode(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    ଯଦି (cells<4) lastpos += cells;
	    ଅନ୍ୟଥା	lastpos++;
	    p += clen;
	}
    }
    ଅନ୍ୟଥା
    {
	BPoint where(TEXT_X(col), TEXT_Y(row));
	DrawString((ଅକ୍ଷର*)s, len, where);
	ଯଦି (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    SetDrawingMode(B_OP_BLEND);
	    DrawString((ଅକ୍ଷର*)s, len, where);
	    SetDrawingMode(B_OP_COPY);
	}
    }

    ଯଦି (flags & DRAW_UNDERL) {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cells;
	cells = 0;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<len; i++) {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cn = utf_ptr2cells((char_u *)(s+i));
	    ଯଦି (cn<4) cells += cn;
	}

	BPoint start(FILL_X(col), FILL_Y(row + 1) - PEN_WIDTH);
	BPoint end(FILL_X(col + cells) - PEN_WIDTH, start.y);

	Stroke_ଧାଡ଼ି(start, end);
    }
}

ଶୂନ୍ୟ
VimTextAreaView::mchClearBlock(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row1,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col1,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row2,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_WIDTH, FILL_Y(row2 + 1) - PEN_WIDTH);
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(r, B_SOLID_ତଳ);
}

    ଶୂନ୍ୟ
VimTextAreaView::mchClearAll()
{
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(Bounds(), B_SOLID_ତଳ);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    ଶୂନ୍ୟ
VimTextAreaView::mch_Delete_ଧାଡ଼ିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    BRect source, dest;
    source.ବାମ = FILL_X(gui.scroll_region_ବାମ);
    source.ଶୀର୍ଷ = FILL_Y(row + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ);
    source.ଡାହାଣ = FILL_X(gui.scroll_region_ଡାହାଣ + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    dest.ବାମ = FILL_X(gui.scroll_region_ବାମ);
    dest.ଶୀର୍ଷ = FILL_Y(row);
    dest.ଡାହାଣ = FILL_X(gui.scroll_region_ଡାହାଣ + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot - ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ + 1) - PEN_WIDTH;

    ଯଦି (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(gui.scroll_region_bot - ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ + 1,
		gui.scroll_region_ବାମ,
		gui.scroll_region_bot, gui.scroll_region_ଡାହାଣ);


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
    ଶୂନ୍ୟ
VimTextAreaView::mch_ଭର୍ତ୍ତି_ଧାଡ଼ିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    BRect source, dest;

    // XXX Attempt at a hack:
    gui.vimWindow->UpdateIfNeeded();
    source.ବାମ = FILL_X(gui.scroll_region_ବାମ);
    source.ଶୀର୍ଷ = FILL_Y(row);
    source.ଡାହାଣ = FILL_X(gui.scroll_region_ଡାହାଣ + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot - ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ + 1) - PEN_WIDTH;

    dest.ବାମ = FILL_X(gui.scroll_region_ବାମ);
    dest.ଶୀର୍ଷ = FILL_Y(row + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ);
    dest.ଡାହାଣ = FILL_X(gui.scroll_region_ଡାହାଣ + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    ଯଦି (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(row, gui.scroll_region_ବାମ,
		row + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ - 1, gui.scroll_region_ଡାହାଣ);

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
ଶୂନ୍ୟ VimTextAreaView::DrawIMString(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ଅଚଳ rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    ଅଚଳ ଅକ୍ଷର* s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
    BMessage* ସନ୍ଦେଶ = IMData.message;
    ଯଦି (!ସନ୍ଦେଶ)
	ଫେରନ୍ତୁ;
    gui_redraw_block(IMData.row, 0,
	    IMData.row + IMData.ଗଣନା_2, W_WIDTH(curwin), GUI_MON_NOCLEAR);
    ତାର୍କିକ confirmed = ମିଥ୍ୟା;
    ସନ୍ଦେଶ->FindBool("be:confirmed", &confirmed);
    ଯଦି (confirmed)
	ଫେରନ୍ତୁ;
    rgb_color hcolor = ଉପର_Color(), lcolor = ତଳ_Color();
    ସନ୍ଦେଶ->FindString("be:string", &str);
    s = str.String();
    len = str.Length();
    Set_ଉପର_Color(0, 0, 0);
    IMData.row = gui.row;
    IMData.col = gui.col;
    int32 sel_start = 0, sel_end = 0;
    ସନ୍ଦେଶ->FindInt32("be:selection", 0, &sel_start);
    ସନ୍ଦେଶ->FindInt32("be:selection", 1, &sel_end);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା clen, cn;
    BPoint pos(IMData.col, 0);
    BRect r;
    BPoint where;
    IMData.location = ConvertToScreen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	ଯଦି (pos.x + cn > W_WIDTH(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	ଯଦି (sel_start<=i && i<sel_end)
	{
	    Set_ତଳ_Color(r_highlight);
	    IMData.location = ConvertToScreen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
	}
	ଅନ୍ୟଥା
	{
	    Set_ତଳ_Color(b_highlight);
	}
	r.Set(FILL_X(pos.x), FILL_Y(IMData.row + pos.y),
		FILL_X(pos.x + cn) - PEN_WIDTH,
		FILL_Y(IMData.row + pos.y + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_ତଳ);
	where.Set(TEXT_X(pos.x), TEXT_Y(IMData.row + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.ଗଣନା_2 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pos.y;

    Set_ଉପର_Color(hcolor);
    Set_ତଳ_Color(lcolor);
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
	    "vim scrollbar", (BView *)ଖାଲି,
	    0.0, 10.0, posture),
    ignore_ମୂଲ୍ୟ(-1),
    scroll_Event_ଗଣନା(0)
{
    gsb = g;
    SetResizingMode(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    ଶୂନ୍ୟ
VimScrollBar::ମୂଲ୍ୟ_Changed(ଦଶମିକ new_ମୂଲ୍ୟ)
{
    ଯଦି (ignore_ମୂଲ୍ୟ >= 0.0 && new_ମୂଲ୍ୟ == ignore_ମୂଲ୍ୟ) {
	ignore_ମୂଲ୍ୟ = -1;
	ଫେରନ୍ତୁ;
    }
    ignore_ମୂଲ୍ୟ = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_add(&scroll_Event_ଗଣନା, 1);

    ସଂରଚନା Vim_Scroll_Bar_ସନ୍ଦେଶ sm;

    sm.sb = ଏହି;
    sm.ମୂଲ୍ୟ = (long) new_ମୂଲ୍ୟ;
    sm.stillDragging = TRUE;

    write_port(gui.vdcmp, Vim_ସନ୍ଦେଶ::ScrollBar, &sm, ଆକାର(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    ଶୂନ୍ୟ
VimScrollBar::MouseUp(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_add(&scroll_Event_ଗଣନା, 1);

    ସଂରଚନା Vim_Scroll_Bar_ସନ୍ଦେଶ sm;

    sm.sb = ଏହି;
    sm.ମୂଲ୍ୟ = (long) ମୂଲ୍ୟ_2();
    sm.stillDragging = FALSE;

    write_port(gui.vdcmp, Vim_ସନ୍ଦେଶ::ScrollBar, &sm, ଆକାର(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::MouseUp(where);
}

    ଶୂନ୍ୟ
VimScrollBar::Set_ମୂଲ୍ୟ(ଦଶମିକ new_ମୂଲ୍ୟ)
{
    ଯଦି (new_ମୂଲ୍ୟ == ମୂଲ୍ୟ_2())
	ଫେରନ୍ତୁ;

    ignore_ମୂଲ୍ୟ = new_ମୂଲ୍ୟ;
    Inherited::Set_ମୂଲ୍ୟ(new_ମୂଲ୍ୟ);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(ଅଚଳ VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ଅଚଳ BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ଅଚଳ VimFont &rhs): BFont(rhs)
{
    init();
}

VimFont::~VimFont()
{
}

    ଶୂନ୍ୟ
VimFont::init()
{
    next = ଖାଲି;
    refcount = 1;
    name = ଖାଲି;
}

// ---------------- VimDialog ----------------

#ଯଦି defined(FEAT_GUI_DIALOG)

ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  k_Vim_Dialog_Button_ସନ୍ଦେଶ = 'VMDB';
ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  kVimDialogIconStripeWidth = 30;
ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  kVimDialogButtonsSpacingX = 9;
ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  kVimDialogButtonsSpacingY = 4;
ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  kVimDialogSpacingX = 6;
ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  kVimDialogSpacingY = 10;
ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  kVimDialogMinimalWidth  = 310;
ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  kVimDialogMinimalHeight = 75;
ଅଚଳ BRect	    kDefaultRect =
BRect(0, 0, kVimDialogMinimalWidth, kVimDialogMinimalHeight);

VimDialog::VimDialog(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଅଚଳ ଅକ୍ଷର *title, ଅଚଳ ଅକ୍ଷର *message,
	ଅଚଳ ଅକ୍ଷର *buttons, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dfltbutton, ଅଚଳ ଅକ୍ଷର *textfield, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_ମୂଲ୍ୟ(dfltbutton)
    , fMessageView(ଖାଲି)
    , f_ନିବେଶ_Control(ଖାଲି)
    , f_ନିବେଶ_ମୂଲ୍ୟ(textfield)
{
    //	master view
    VimDialog::View* view = ନୂତନ VimDialog::View(Bounds());
    ଯଦି (view == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (title == ଖାଲି)
	SetTitle("Vim " VIM_VERSION_MEDIUM);

    AddChild(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    ଦଶମିକ maxButtonWidth  = 0;
    ଦଶମିକ maxButtonHeight = 0;
    ଦଶମିକ buttonsWidth	  = 0;
    ଦଶମିକ buttonsHeight   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    କର
    {
	int32 end = strButtons.FindFirst('\n');
	ଯଦି (end != B_ERROR)
	    strButtons.SetByteAt(end, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->AddChild(button);
	fButtonsList.AddItem(button);

	maxButtonWidth	= max_c(maxButtonWidth,  button->Bounds().Width());
	maxButtonHeight = max_c(maxButtonHeight, button->Bounds().Height());
	buttonsWidth   += button->Bounds().Width();
	buttonsHeight  += button->Bounds().Height();

	ଯଦି (end == B_ERROR)
	    ଭାଙ୍ଗ;

	strButtons.Remove(0, end + 1);
    } ଯେତେବେଳେ (ସତ୍ୟ);

    int32 buttons_ଗଣନା = fButtonsList.ଗଣନା_ବସ୍ତୁଗୁଡ଼ିକ();
    buttonsWidth      += kVimDialogButtonsSpacingX * (buttons_ଗଣନା - 1);
    buttonsHeight     += kVimDialogButtonsSpacingY * (buttons_ଗଣନା - 1);
    ଦଶମିକ dialogWidth  = buttonsWidth + kVimDialogIconStripeWidth +
	kVimDialogSpacingX * 2;
    ଦଶମିକ dialogHeight = maxButtonHeight + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    ତାର୍କିକ vertical = (vim_strchr(p_go, GO_VERTICAL) != ଖାଲି) ||
	dialogWidth >= gui.vimWindow->Bounds().Width();
    ଯଦି (vertical) {
	dialogWidth  -= buttonsWidth;
	dialogWidth  += maxButtonWidth;
	dialogHeight -= maxButtonHeight;
	dialogHeight += buttonsHeight;
    }

    dialogWidth  = max_c(dialogWidth,  kVimDialogMinimalWidth);

    //	message view
    BRect rect(0, 0, dialogWidth, 0);
    rect.ବାମ  += kVimDialogIconStripeWidth + 16 + kVimDialogSpacingX;
    rect.ଶୀର୍ଷ   += kVimDialogSpacingY;
    rect.ଡାହାଣ -= kVimDialogSpacingX;
    rect.bottom = rect.ଶୀର୍ଷ;
    fMessageView = ନୂତନ BTextView(rect, "_tv_", rect.OffsetByCopy(B_ORIGIN),
	    B_FOLLOW_ବାମ | B_FOLLOW_ଶୀର୍ଷ, B_WILL_DRAW);

    fMessageView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color textColor = ui_color(B_PANEL_TEXT_COLOR);
    fMessageView->SetFontAndColor(be_plain_font, B_FONT_ALL, &textColor);
    fMessageView->SetText(message);
    fMessageView->MakeEditable(ମିଥ୍ୟା);
    fMessageView->MakeSelectable(ମିଥ୍ୟା);
    fMessageView->SetWordWrap(ସତ୍ୟ);
    AddChild(fMessageView);

    ଦଶମିକ messageHeight = fMessageView->TextHeight(0, fMessageView->ଗଣନା_ଧାଡ଼ିଗୁଡ଼ିକ());
    fMessageView->ResizeBy(0, messageHeight);
    fMessageView->SetTextRect(BRect(0, 0, rect.Width(), messageHeight));

    dialogHeight += messageHeight;

    //	input view
    ଯଦି (f_ନିବେଶ_ମୂଲ୍ୟ != ଖାଲି) {
	rect.ଶୀର୍ଷ     =
	    rect.bottom += messageHeight + kVimDialogSpacingY;
	f_ନିବେଶ_Control = ନୂତନ BTextControl(rect, "_iv_", ଖାଲି, f_ନିବେଶ_ମୂଲ୍ୟ, ଖାଲି,
		B_FOLLOW_ବାମ | B_FOLLOW_ଶୀର୍ଷ, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_ନିବେଶ_Control->TextView()->SetText(f_ନିବେଶ_ମୂଲ୍ୟ);
	f_ନିବେଶ_Control->TextView()->SetWordWrap(ମିଥ୍ୟା);
	AddChild(f_ନିବେଶ_Control);

	ଦଶମିକ width = 0.f, height = 0.f;
	f_ନିବେଶ_Control->GetPreferredSize(&width, &height);
	f_ନିବେଶ_Control->MakeFocus(ସତ୍ୟ);

	dialogHeight += height + kVimDialogSpacingY * 1.5;
    }

    dialogHeight = max_c(dialogHeight, kVimDialogMinimalHeight);

    ResizeTo(dialogWidth, dialogHeight);
    MoveTo((gui.vimWindow->Bounds().Width() - dialogWidth) / 2,
	    (gui.vimWindow->Bounds().Height() - dialogHeight) / 2);

    //	adjust layout of buttons
    ଦଶମିକ buttonWidth = max_c(maxButtonWidth, rect.Width() * 0.66);
    BPoint origin(dialogWidth, dialogHeight);
    origin.x -= kVimDialogSpacingX + (vertical ? buttonWidth : buttonsWidth);
    origin.y -= kVimDialogSpacingY + (vertical ? buttonsHeight	: maxButtonHeight);

    ପାଇଁ (int32 i = 0 ; i < buttons_ଗଣନା; i++) {
	BButton *button = (BButton*)fButtonsList.ItemAt(i);
	button->MoveTo(origin);
	ଯଦି (vertical) {
	    origin.y += button->Frame().Height() + kVimDialogButtonsSpacingY;
	    button->ResizeTo(buttonWidth, button->Frame().Height());
	} ଅନ୍ୟଥା
	    origin.x += button->Frame().Width() + kVimDialogButtonsSpacingX;

	ଯଦି (dfltbutton == i + 1) {
	    button->MakeDefault(ସତ୍ୟ);
	    button->MakeFocus(f_ନିବେଶ_Control == ଖାଲି);
	}
    }
}

VimDialog::~VimDialog()
{
    ଯଦି (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    ଯଦି (fDialogSem < B_OK) {
	Quit();
	ଫେରନ୍ତୁ f_Dialog_ମୂଲ୍ୟ;
    }

    Show();

    ଯେତେବେଳେ (acquire_sem(fDialogSem) == B_INTERRUPTED);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret_ମୂଲ୍ୟ = f_Dialog_ମୂଲ୍ୟ;
    ଯଦି (f_ନିବେଶ_ମୂଲ୍ୟ != ଖାଲି)
	vim_strncpy((char_u*)f_ନିବେଶ_ମୂଲ୍ୟ, (char_u*)f_ନିବେଶ_Control->Text(), IOSIZE - 1);

    ଯଦି (Lock())
	Quit();

    ଫେରନ୍ତୁ ret_ମୂଲ୍ୟ;
}

ଶୂନ୍ୟ VimDialog::MessageReceived(BMessage *ସନ୍ଦେଶ)
{
    int32 which = 0;
    ଯଦି (ସନ୍ଦେଶ->what != k_Vim_Dialog_Button_ସନ୍ଦେଶ ||
	    ସନ୍ଦେଶ->FindInt32("which", &which) != B_OK)
	ଫେରନ୍ତୁ BWindow::MessageReceived(ସନ୍ଦେଶ);

    f_Dialog_ମୂଲ୍ୟ = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, ଅଚଳ ଅକ୍ଷର* label)
{
    BMessage *message = ନୂତନ BMessage(k_Vim_Dialog_Button_ସନ୍ଦେଶ);
    message->AddInt32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = ନୂତନ BButton(rect, name.String(), label, message,
	    B_FOLLOW_ଡାହାଣ | B_FOLLOW_BOTTOM);

    ଦଶମିକ width = 0.f, height = 0.f;
    button->GetPreferredSize(&width, &height);
    button->ResizeTo(width, height);

    ଫେରନ୍ତୁ button;
}

VimDialog::View::View(BRect frame)
    :	BView(frame, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(ଖାଲି)
{
    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    delete fIconBitmap;
}

ଶୂନ୍ୟ VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.ଡାହାଣ = kVimDialogIconStripeWidth;
    Set_ଉପର_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    ଯଦି (fIconBitmap == ଖାଲି)
	ଫେରନ୍ତୁ;

    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

ଶୂନ୍ୟ VimDialog::View::InitIcon(int32 type)
{
    ଯଦି (type == VIM_GENERIC)
	ଫେରନ୍ତୁ;

    BPath path;
    status_t status = find_directory(B_BEOS_SERVERS_DIRECTORY, &path);
    ଯଦି (status != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(status));
	ଫେରନ୍ତୁ;
    }

    path.Append("app_server");

    BFile file(path.Path(), O_RDONLY);
    ଯଦି (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	ଫେରନ୍ତୁ;
    }

    BResources resources(&file);
    ଯଦି (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	ଫେରନ୍ତୁ;
    }

    ଅଚଳ ଅକ୍ଷର *name = "";
    ବାଛ(type) {
	ସ୍ଥିତି VIM_ERROR:	    name = "stop"; ଭାଙ୍ଗ;
	ସ୍ଥିତି VIM_WARNING:   name = "warn"; ଭାଙ୍ଗ;
	ସ୍ଥିତି VIM_INFO:	    name = "info"; ଭାଙ୍ଗ;
	ସ୍ଥିତି VIM_QUESTION:  name = "idea"; ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ;
    }

    int32 iconSize = 32;
    fIconBitmap = ନୂତନ BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1), 0, B_RGBA32);
    ଯଦି (fIconBitmap == ଖାଲି || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == ଖାଲି) ? "null" : strerror(fIconBitmap->InitCheck()));
	ଫେରନ୍ତୁ;
    }

    size_t size = 0;
    ଅଚଳ uint8* iconData = ଖାଲି;
    //	try vector icon first?
    iconData = (ଅଚଳ uint8*)resources.LoadResource(B_VECTOR_ICON_TYPE, name, &size);
    ଯଦି (iconData != ଖାଲି && BIconUtils::GetVectorIcon(iconData, size, fIconBitmap) == B_OK)
	ଫେରନ୍ତୁ;

    //	try bitmap icon now
    iconData = (ଅଚଳ uint8*)resources.LoadResource(B_LARGE_ICON_TYPE, name, &size);
    ଯଦି (iconData == ଖାଲି) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	delete fIconBitmap;
	fIconBitmap = ଖାଲି;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(iconData, iconSize, iconSize, iconSize, fIconBitmap);
}

ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  k_Vim_Dialog_OKButton_ସନ୍ଦେଶ = 'FDOK';
ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  k_Vim_Dialog_Cancel_Button_ସନ୍ଦେଶ = 'FDCN';
ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  k_Vim_Dialog_Size_ନିବେଶ_ସନ୍ଦେଶ = 'SICH';
ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  k_Vim_Dialog_Family_Select_ସନ୍ଦେଶ = 'MSFM';
ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  k_Vim_Dialog_Style_Select_ସନ୍ଦେଶ = 'MSST';
ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  k_Vim_Dialog_Size_Select_ସନ୍ଦେଶ = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, ଦଶମିକ* size)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fStatus(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_ମୂଲ୍ୟ(ମିଥ୍ୟା)
    , fFamily(family)
    , fStyle(style)
    , fSize(size)
    , fFontSize(*size)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_ନିବେଶ(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_LENGTH);
    strncpy(fFontStyle, *style, B_FONT_STYLE_LENGTH);

    //	"client" area view
    BBox *clientBox = ନୂତନ BBox(Bounds(), B_EMPTY_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    AddChild(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.ବାମ_ଶୀର୍ଷ(), RC.ବାମ_ଶୀର୍ଷ());

    //	at first create all controls
    fPreview = ନୂତନ BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->AddChild(fPreview);

    BBox* boxDivider = ନୂତନ BBox(rc, B_EMPTY_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->AddChild(boxDivider);

    BStringView *labelFamily = ନୂତନ BStringView(rc, "labelFamily", "Family:");
    clientBox->AddChild(labelFamily);
    labelFamily->ResizeToPreferred();

    BStringView *labelStyle = ନୂତନ BStringView(rc, "labelStyle", "Style:");
    clientBox->AddChild(labelStyle);
    labelStyle->ResizeToPreferred();

    BStringView *labelSize = ନୂତନ BStringView(rc, "labelSize", "Size:");
    clientBox->AddChild(labelSize);
    labelSize->ResizeToPreferred();

    fFamiliesList = ନୂତନ BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = ନୂତନ BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_ବାମ_ଡାହାଣ, 0, ମିଥ୍ୟା, ସତ୍ୟ);
    clientBox->AddChild(scrollFamilies);

    fStylesList= ନୂତନ BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = ନୂତନ BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_ବାମ_ଡାହାଣ, 0, ମିଥ୍ୟା, ସତ୍ୟ);
    clientBox->AddChild(scrollStyles);

    f_Sizes_ନିବେଶ = ନୂତନ BTextControl(rc, "inputSize", ଖାଲି, "???",
	    ନୂତନ BMessage(k_Vim_Dialog_Size_ନିବେଶ_ସନ୍ଦେଶ));
    clientBox->AddChild(f_Sizes_ନିବେଶ);
    f_Sizes_ନିବେଶ->ResizeToPreferred();

    fSizesList = ନୂତନ BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = ନୂତନ BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_ବାମ_ଡାହାଣ, 0, ମିଥ୍ୟା, ସତ୍ୟ);
    clientBox->AddChild(scrollSizes);

    BButton *buttonOK = ନୂତନ BButton(rc, "buttonOK", "OK",
			ନୂତନ BMessage(k_Vim_Dialog_OKButton_ସନ୍ଦେଶ));
    clientBox->AddChild(buttonOK);
    buttonOK->ResizeToPreferred();

    BButton *buttonCancel = ନୂତନ BButton(rc, "buttonCancel", "Cancel",
			ନୂତନ BMessage(k_Vim_Dialog_Cancel_Button_ସନ୍ଦେଶ));
    clientBox->AddChild(buttonCancel);
    buttonCancel->ResizeToPreferred();

    //	layout controls
    ଦଶମିକ ଧାଡ଼ି_Height = labelFamily->Bounds().Height();
    ଦଶମିକ previewHeight = ଧାଡ଼ି_Height * 3;
    ଦଶମିକ offsetYLabels = previewHeight + kVimDialogSpacingY;
    ଦଶମିକ offsetYLists = offsetYLabels + ଧାଡ଼ି_Height + kVimDialogSpacingY / 2;
    ଦଶମିକ offsetYSizes = offsetYLists + f_Sizes_ନିବେଶ->Bounds().Height() + kVimDialogSpacingY / 2;
    ଦଶମିକ listsHeight = ଧାଡ଼ି_Height * 9;
    ଦଶମିକ offsetYButtons = offsetYLists + listsHeight +  kVimDialogSpacingY;
    ଦଶମିକ maxControlsHeight = offsetYButtons + buttonOK->Bounds().Height();
    ଦଶମିକ familiesWidth = labelFamily->Bounds().Width() * 5;
    ଦଶମିକ offsetXStyles = familiesWidth + kVimDialogSpacingX;
    ଦଶମିକ stylesWidth = labelStyle->Bounds().Width() * 4;
    ଦଶମିକ offsetXSizes = offsetXStyles + stylesWidth + kVimDialogSpacingX;
    ଦଶମିକ sizesWidth = labelSize->Bounds().Width() * 2;
    ଦଶମିକ maxControlsWidth = offsetXSizes + sizesWidth;

    ResizeTo(maxControlsWidth + kVimDialogSpacingX * 2,
	maxControlsHeight + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vimWindow->Frame();
    MoveTo(rcVim.ବାମ + (rcVim.Width() - Frame().Width()) / 2,
	    rcVim.ଶୀର୍ଷ + (rcVim.Height() - Frame().Height()) / 2);

    fPreview->ResizeTo(maxControlsWidth, previewHeight);
    fPreview->SetAlignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, previewHeight + kVimDialogSpacingY / 2);
    boxDivider->ResizeTo(maxControlsWidth, 1.f);

    labelFamily->MoveBy(0.f, offsetYLabels);
    labelStyle->MoveBy(offsetXStyles, offsetYLabels);
    labelSize->MoveBy(offsetXSizes, offsetYLabels);

    //	text control alignment issues
    ଦଶମିକ insetX = f_Sizes_ନିବେଶ->TextView()->Bounds().Width() - f_Sizes_ନିବେଶ->Bounds().Width();
    ଦଶମିକ insetY = f_Sizes_ନିବେଶ->TextView()->Bounds().Width() - f_Sizes_ନିବେଶ->Bounds().Width();

    scrollFamilies->MoveBy(0.f, offsetYLists);
    scrollStyles->MoveBy(offsetXStyles, offsetYLists);
    f_Sizes_ନିବେଶ->MoveBy(offsetXSizes + insetX / 2, offsetYLists + insetY / 2);
    scrollSizes->MoveBy(offsetXSizes, offsetYSizes);

    f_Sizes_ନିବେଶ->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->ResizeTo(familiesWidth, listsHeight);
    scrollStyles->ResizeTo(stylesWidth, listsHeight);
    f_Sizes_ନିବେଶ->ResizeTo(sizesWidth, f_Sizes_ନିବେଶ->Bounds().Height());
    scrollSizes->ResizeTo(sizesWidth,
	    listsHeight - (offsetYSizes - offsetYLists));

    buttonOK->MoveBy(maxControlsWidth - buttonOK->Bounds().Width(), offsetYButtons);
    buttonCancel->MoveBy(maxControlsWidth - buttonOK->Bounds().Width()
	    - buttonCancel->Bounds().Width() - kVimDialogSpacingX, offsetYButtons);

    //	fill lists
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା selIndex = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = ଗଣନା_font_families();
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଗଣନା_2; i++) {
	font_family family;
	ଯଦି (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->AddItem(ନୂତନ BStringItem((ଅଚଳ ଅକ୍ଷର*)family));
	    ଯଦି (strncmp(family, fFontFamily, B_FONT_FAMILY_LENGTH) == 0)
		selIndex = i;
	}
    }

    ଯଦି (selIndex >= 0) {
	fFamiliesList->Select(selIndex);
	fFamiliesList->ScrollToSelection();
    }

    _UpdateFontStyles();

    selIndex = -1;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = 8, index = 0; size <= 18; size++, index++) {
	BString str;
	str << size;
	fSizesList->AddItem(ନୂତନ BStringItem(str));
	ଯଦି (size == fFontSize)
	    selIndex = index;

    }

    ଯଦି (selIndex >= 0) {
	fSizesList->Select(selIndex);
	fSizesList->ScrollToSelection();
    }

    fFamiliesList->SetSelectionMessage(ନୂତନ BMessage(k_Vim_Dialog_Family_Select_ସନ୍ଦେଶ));
    fStylesList->SetSelectionMessage(ନୂତନ BMessage(k_Vim_Dialog_Style_Select_ସନ୍ଦେଶ));
    fSizesList->SetSelectionMessage(ନୂତନ BMessage(k_Vim_Dialog_Size_Select_ସନ୍ଦେଶ));
    f_Sizes_ନିବେଶ->SetModificationMessage(ନୂତନ BMessage(k_Vim_Dialog_Size_ନିବେଶ_ସନ୍ଦେଶ));

    Update_Size_ନିବେଶ_Preview();
    _UpdateFontPreview();

    fStatus = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    ଯଦି (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    ଶୂନ୍ୟ
VimSelectFontDialog::_CleanList(BListView* list)
{
    ଯେତେବେଳେ (0 < list->ଗଣନା_ବସ୍ତୁଗୁଡ଼ିକ())
	delete (dynamic_cast<BStringItem*>(list->RemoveItem((int32)0)));
}

    ତାର୍କିକ
VimSelectFontDialog::Go()
{
    ଯଦି (fStatus != B_OK) {
	Quit();
	ଫେରନ୍ତୁ NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    ଯଦି (fDialogSem < B_OK) {
	Quit();
	ଫେରନ୍ତୁ f_Dialog_ମୂଲ୍ୟ;
    }

    Show();

    ଯେତେବେଳେ (acquire_sem(fDialogSem) == B_INTERRUPTED);

    ତାର୍କିକ ret_ମୂଲ୍ୟ = f_Dialog_ମୂଲ୍ୟ;

    ଯଦି (Lock())
	Quit();

    ଫେରନ୍ତୁ ret_ମୂଲ୍ୟ;
}


ଶୂନ୍ୟ VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 selIndex = -1;
    int32 ଗଣନା_2 = ଗଣନା_font_styles(fFontFamily);
    ପାଇଁ (int32 i = 0; i < ଗଣନା_2; i++) {
	font_style style;
	uint32 flags = 0;
	ଯଦି (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->AddItem(ନୂତନ BStringItem((ଅଚଳ ଅକ୍ଷର*)style));
	    ଯଦି (strncmp(style, fFontStyle, B_FONT_STYLE_LENGTH) == 0)
		selIndex = i;
	}
    }

    ଯଦି (selIndex >= 0) {
	fStylesList->Select(selIndex);
	fStylesList->ScrollToSelection();
    } ଅନ୍ୟଥା
	fStylesList->Select(0);
}


ଶୂନ୍ୟ VimSelectFontDialog::Update_Size_ନିବେଶ_Preview()
{
    ଅକ୍ଷର buf[10] = {0};
    vim_snprintf(buf, ଆକାର(buf), (ଅକ୍ଷର*)"%.0f", fFontSize);
    f_Sizes_ନିବେଶ->SetText(buf);
}


ଶୂନ୍ୟ VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.SetSize(fFontSize);
    font.SetFamilyAndStyle(fFontFamily, fFontStyle);
    fPreview->SetFont(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_SIZE);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fFontSize << " pt.";
    fPreview->SetText(str);
}


    ତାର୍କିକ
VimSelectFontDialog::_UpdateFromListItem(BListView* list, ଅକ୍ଷର* text, ପୂର୍ଣ୍ଣସଂଖ୍ୟା textSize)
{
    int32 index = list->CurrentSelection();
    ଯଦି (index < 0)
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
    BStringItem* item = (BStringItem*)list->ItemAt(index);
    ଯଦି (item == ଖାଲି)
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
    strncpy(text, item->Text(), textSize);
    ଫେରନ୍ତୁ ସତ୍ୟ;
}


ଶୂନ୍ୟ VimSelectFontDialog::MessageReceived(BMessage *ସନ୍ଦେଶ)
{
    ବାଛ (ସନ୍ଦେଶ->what) {
	ସ୍ଥିତି k_Vim_Dialog_OKButton_ସନ୍ଦେଶ:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_LENGTH);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_LENGTH);
	    *fSize = fFontSize;
	    f_Dialog_ମୂଲ୍ୟ = ସତ୍ୟ;
	ସ୍ଥିତି k_Vim_Dialog_Cancel_Button_ସନ୍ଦେଶ:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    ଫେରନ୍ତୁ;
	ସ୍ଥିତି B_ଚାବି_UP:
	    {
		int32 ଚାବି = 0;
		ଯଦି (ସନ୍ଦେଶ->FindInt32("raw_char", &ଚାବି) == B_OK
			&& ଚାବି == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି k_Vim_Dialog_Family_Select_ସନ୍ଦେଶ:
	    ଯଦି (_UpdateFromListItem(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_LENGTH)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି k_Vim_Dialog_Style_Select_ସନ୍ଦେଶ:
	    ଯଦି (_UpdateFromListItem(fStylesList,
		    fFontStyle, B_FONT_STYLE_LENGTH))
		_UpdateFontPreview();
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି k_Vim_Dialog_Size_Select_ସନ୍ଦେଶ:
	    {
		ଅକ୍ଷର buf[10] = {0};
		ଯଦି (_UpdateFromListItem(fSizesList, buf, ଆକାର(buf))) {
		    ଦଶମିକ size = atof(buf);
		    ଯଦି (size > 0.f) {
			fFontSize = size;
			Update_Size_ନିବେଶ_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି k_Vim_Dialog_Size_ନିବେଶ_ସନ୍ଦେଶ:
	    {
		ଦଶମିକ size = atof(f_Sizes_ନିବେଶ->Text());
		ଯଦି (size > 0.f) {
		    fFontSize = size;
		    _UpdateFontPreview();
		}
	    }
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ BWindow::MessageReceived(ସନ୍ଦେଶ);
}

#endif // FEAT_GUI_DIALOG

#ifdef FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
ସ୍ଥିର BMessage * MenuMessage(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect frame, ଅଚଳ ଅକ୍ଷର *name) :
    BBox(frame, name, B_FOLLOW_ବାମ | B_FOLLOW_ଶୀର୍ଷ, B_WILL_DRAW | B_FRAME_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 ଗଣନା_2 = fButtonsList.ଗଣନା_ବସ୍ତୁଗୁଡ଼ିକ();
    ପାଇଁ (int32 i = 0; i < ଗଣନା_2; i++)
	delete (BPictureButton*)fButtonsList.ItemAt(i);
    fButtonsList.MakeEmpty();

    delete normalButtonsBitmap;
    delete grayedButtonsBitmap;
    normalButtonsBitmap    = ଖାଲି;
    grayedButtonsBitmap  = ଖାଲି;
}

    ଶୂନ୍ୟ
VimToolbar::AttachedToWindow()
{
    BBox::AttachedToWindow();

    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    ଦଶମିକ
VimToolbar::ToolbarHeight() ଅଚଳ
{
    ଦଶମିକ size = ଖାଲି == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
    ଫେରନ୍ତୁ size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    ତାର୍କିକ
VimToolbar::ModifyBitmapToGrayed(BBitmap *bitmap)
{
    ଦଶମିକ height = bitmap->Bounds().Height();
    ଦଶମିକ width  = bitmap->Bounds().Width();

    rgb_color *bits = (rgb_color*)bitmap->Bits();
    int32 pixels = bitmap->BitsLength() / 4;
    ପାଇଁ (int32 i = 0; i < pixels; i++) {
	bits[i].red = bits[i].green =
	bits[i].blue = ((uint32)bits[i].red + bits[i].green + bits[i].blue) / 3;
	bits[i].alpha /= 4;
    }

    ଫେରନ୍ତୁ ସତ୍ୟ;
}

    ତାର୍କିକ
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    normalButtonsBitmap = LoadVimBitmap("builtin-tools.png");
    ଯଦି (normalButtonsBitmap == ଖାଲି)
	//  customized not found? dig application resources for "builtin-tools" one
	normalButtonsBitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    ଯଦି (normalButtonsBitmap == ଖାଲି)
	ଫେରନ୍ତୁ ମିଥ୍ୟା;

    BMessage archive;
    normalButtonsBitmap->Archive(&archive);

    grayedButtonsBitmap = ନୂତନ BBitmap(&archive);
    ଯଦି (grayedButtonsBitmap == ଖାଲି)
	ଫେରନ୍ତୁ ମିଥ୍ୟା;

    //	modify grayed bitmap
    ModifyBitmapToGrayed(grayedButtonsBitmap);

    ଫେରନ୍ତୁ ସତ୍ୟ;
}

BBitmap *VimToolbar::LoadVimBitmap(ଅଚଳ ଅକ୍ଷର* fileName)
{
    BBitmap *bitmap = ଖାଲି;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mustfree = 0;
    char_u* runtimePath = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    ଯଦି (runtimePath != ଖାଲି && fileName != ଖାଲି) {
	BString strPath((ଅକ୍ଷର*)runtimePath);
	strPath << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(strPath.String());
    }

    ଯଦି (mustfree)
	vim_free(runtimePath);

    ଫେରନ୍ତୁ bitmap;
}

    ତାର୍କିକ
VimToolbar::GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, ତାର୍କିକ pressed)
{
    ଦଶମିକ size = bitmapFrom->Bounds().Height() + 1.;

    BView view(BRect(0, 0, size, size), "", 0, 0);

    AddChild(&view);
    view.BeginPicture(pictureTo);

    view.Set_ଉପର_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.SetDrawingMode(B_OP_OVER);

    BRect source(0, 0, size - 1, size - 1);
    BRect destination(source);

    source.OffsetBy(size * index, 0);
    destination.OffsetBy(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, source, destination);

    ଯଦି (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	size += ButtonMargin * 2 - 1;
	view.Begin_ଧାଡ଼ି_Array(4);
	view.Add_ଧାଡ଼ି(BPoint(0, 0),	 BPoint(size, 0),    shadowColor);
	view.Add_ଧାଡ଼ି(BPoint(size, 0),	 BPoint(size, size), shineColor);
	view.Add_ଧାଡ଼ି(BPoint(size, size), BPoint(0, size),    shineColor);
	view.Add_ଧାଡ଼ି(BPoint(0, size),	 BPoint(0, 0),	     shadowColor);
	view.End_ଧାଡ଼ି_Array();
    }

    view.EndPicture();
    RemoveChild(&view);

    ଫେରନ୍ତୁ ସତ୍ୟ;
}

    ତାର୍କିକ
VimToolbar::AddButton(int32 index, vimmenu_T *menu)
{
    BPictureButton *button = ଖାଲି;
    ଯଦି (!menu_is_separator(menu->name)) {
	ଦଶମିକ size = normalButtonsBitmap ?
	    normalButtonsBitmap->Bounds().Height() + 1. + ButtonMargin * 2 : 18.;
	BRect frame(0, 0, size, size);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture pictureGray;

	ଯଦି (menu->iconfile == ଖାଲି && menu->iconidx >= 0 && normalButtonsBitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, normalButtonsBitmap, ସତ୍ୟ);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, normalButtonsBitmap, ମିଥ୍ୟା);
	    GetPictureFromBitmap(&pictureGray, menu->iconidx, grayedButtonsBitmap, ମିଥ୍ୟା);
	} ଅନ୍ୟଥା {

	    char_u buffer[MAXPATHL] = {0};
	    BBitmap *bitmap = ଖାଲି;

	    ଯଦି (menu->iconfile) {
		gui_find_iconfile(menu->iconfile, buffer, (ଅକ୍ଷର*)"png");
		bitmap = BTranslationUtils::GetBitmap((ଅକ୍ଷର*)buffer);
	    }

	    ଯଦି (bitmap == ଖାଲି && gui_find_bitmap(menu->name, buffer, (ଅକ୍ଷର*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((ଅକ୍ଷର*)buffer);

	    ଯଦି (bitmap == ଖାଲି)
		bitmap = ନୂତନ BBitmap(BRect(0, 0, size, size), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, ସତ୍ୟ);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, ମିଥ୍ୟା);
	    ModifyBitmapToGrayed(bitmap);
	    GetPictureFromBitmap(&pictureGray, 0, bitmap, ମିଥ୍ୟା);

	    delete bitmap;
	}

	button = ନୂତନ BPictureButton(frame, (ଅକ୍ଷର*)menu->name,
		    &pictureOff, &pictureOn, MenuMessage(menu));

	button->SetDisabledOn(&pictureGray);
	button->SetDisabledOff(&pictureGray);

	button->SetTarget(gui.vimTextArea);

	AddChild(button);

	menu->button = button;
    }

    ତାର୍କିକ result = fButtonsList.AddItem(button, index);
    InvalidateLayout();
    ଫେରନ୍ତୁ result;
}

    ତାର୍କିକ
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    ଯଦି (menu->button) {
	ଯଦି (fButtonsList.RemoveItem(menu->button)) {
	    delete menu->button;
	    menu->button = ଖାଲି;
	}
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

    ତାର୍କିକ
VimToolbar::GrayButton(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା grey)
{
    ଯଦି (menu->button) {
	int32 index = fButtonsList.IndexOf(menu->button);
	ଯଦି (index >= 0)
	    menu->button->SetEnabled(grey ? ମିଥ୍ୟା : ସତ୍ୟ);
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

    ଶୂନ୍ୟ
VimToolbar::InvalidateLayout()
{
    int32 offset = ToolbarMargin;
    int32 ଗଣନା_2 = fButtonsList.ଗଣନା_ବସ୍ତୁଗୁଡ଼ିକ();
    ପାଇଁ (int32 i = 0; i < ଗଣନା_2; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.ItemAt(i);
	ଯଦି (button) {
	    button->MoveTo(offset, ToolbarMargin);
	    offset += button->Bounds().Width() + ToolbarMargin;
	} ଅନ୍ୟଥା
	    offset += ToolbarMargin * 3;
    }
}

#endif /*FEAT_TOOLBAR*/

#ଯଦି defined(FEAT_GUI_TABLINE)

    ଦଶମିକ
Vim_Tab_ଧାଡ଼ି::TablineHeight() ଅଚଳ
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    ଫେରନ୍ତୁ TabHeight(); //  + ToolbarMargin;
}

ଶୂନ୍ୟ
Vim_Tab_ଧାଡ଼ି::MouseDown(BPoint point)
{
    ଯଦି (!gui_mch_showing_tabline())
	ଫେରନ୍ତୁ;

    BMessage *m = Window()->CurrentMessage();
    ପ୍ରତିଜ୍ଞା(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    int32 clicks = 0;
    m->FindInt32("clicks", &clicks);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = 0; //  0 means here - no tab found
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଗଣନା_Tabs(); i++) {
	ଯଦି (TabFrame(i).Contains(point)) {
	    index = i + 1; //  indexes are 1-based
	    ଭାଙ୍ଗ;
	}
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା event = -1;

    ଯଦି ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    ଅନ୍ୟଥା ଯଦି (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = index > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    ଅନ୍ୟଥା ଯଦି (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPopUpMenu* କାଢ଼_Up_Menu = ନୂତନ BPopUpMenu("tabLineContextMenu", ମିଥ୍ୟା, ମିଥ୍ୟା);
	କାଢ଼_Up_Menu->AddItem(ନୂତନ BMenuItem(_("Close tabi R"), ନୂତନ BMessage(TABLINE_MENU_CLOSE)));
	କାଢ଼_Up_Menu->AddItem(ନୂତନ BMenuItem(_("New tab    T"), ନୂତନ BMessage(TABLINE_MENU_NEW)));
	କାଢ଼_Up_Menu->AddItem(ନୂତନ BMenuItem(_("Open tab..."), ନୂତନ BMessage(TABLINE_MENU_OPEN)));

	ConvertToScreen(&point);
	BMenuItem* item = କାଢ଼_Up_Menu->Go(point);
	ଯଦି (item != ଖାଲି) {
	    event = item->Command();
	}

	delete କାଢ଼_Up_Menu;

    } ଅନ୍ୟଥା {
	//  default processing
	BTabView::MouseDown(point);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (event < 0)
	ଫେରନ୍ତୁ;

    Vim_Tabline_Menu_ସନ୍ଦେଶ tmm;
    tmm.index = index;
    tmm.event = event;
    write_port(gui.vdcmp, Vim_ସନ୍ଦେଶ::TablineMenu, &tmm, ଆକାର(tmm));
}

ଶୂନ୍ୟ
Vim_Tab_ଧାଡ଼ି::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_ଧାଡ଼ି *tab_ଧାଡ଼ି = gui.vimForm->Tab_ଧାଡ଼ି();
    ଯଦି (tab_ଧାଡ଼ି != ଖାଲି) {

	int32 i = 0;
	ପାଇଁ (; i < tab_ଧାଡ଼ି->ଗଣନା_Tabs(); i++)
	    ଯଦି (ଏହି == tab_ଧାଡ଼ି->TabAt(i))
		ଭାଙ୍ଗ;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	ଯଦି (i < tab_ଧାଡ଼ି->ଗଣନା_Tabs()) {
	    Vim_Tabline_ସନ୍ଦେଶ tm;
	    tm.index = i + 1;
	    write_port(gui.vdcmp, Vim_ସନ୍ଦେଶ::Tabline, &tm, ଆକାର(tm));
	}
    }
}

#endif //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
ସ୍ଥିର ଅକ୍ଷର appsig[] = "application/x-vnd.Haiku-Vim-8";
ଚାବି_map *ଚାବି_Map;
ଅକ୍ଷର *ଚାବି_Map_Chars;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା main_exitcode = 127;

    status_t
gui_haiku_process_event(bigtime_t timeout)
{
    ସଂରଚନା Vim_ସନ୍ଦେଶ vm;
    int32 what;
    ssize_t size;

    size = read_port_etc(gui.vdcmp, &what, &vm, ଆକାର(vm),
	    B_TIMEOUT, timeout);

    ଯଦି (size >= 0) {
	ବାଛ (what) {
	    ସ୍ଥିତି Vim_ସନ୍ଦେଶ::ଚାବି_2:
		{
		    char_u *string = vm.u.ଚାବି_2.chars;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = vm.u.ଚାବି_2.length;
		    ଯଦି (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_ନିବେଶ_buf();
			got_int = TRUE;
		    }

		    ଯଦି (vm.u.ଚାବି_2.csi_escape)
#ifndef FEAT_MBYTE_IME
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
			char_u	buf[2];

			ପାଇଁ (i = 0; i < len; ++i)
			{
			    add_to_ନିବେଶ_buf(string + i, 1);
			    ଯଦି (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CSI;
				add_to_ନିବେଶ_buf(buf, 2);
			    }
			}
		    }
#ଅନ୍ୟଥା
			add_to_ନିବେଶ_buf_csi(string, len);
#endif
		    ଅନ୍ୟଥା
			add_to_ନିବେଶ_buf(string, len);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Vim_ସନ୍ଦେଶ::Resize:
		gui_resize_shell(vm.u.NewSize.width, vm.u.NewSize.height);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Vim_ସନ୍ଦେଶ::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_ଗଣନା =
			atomic_add(&vm.u.Scroll.sb->scroll_Event_ଗଣନା, -1);
		    ଯଦି (old_ଗଣନା <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.ମୂଲ୍ୟ, vm.u.Scroll.stillDragging);
		}
		ଭାଙ୍ଗ;
#ଯଦି defined(FEAT_MENU)
	    ସ୍ଥିତି Vim_ସନ୍ଦେଶ::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		ଭାଙ୍ଗ;
#endif
	    ସ୍ଥିତି Vim_ସନ୍ଦେଶ::Mouse:
		{
		    int32 old_ଗଣନା;
		    ଯଦି (vm.u.Mouse.button == MOUSE_DRAG)
			old_ଗଣନା =
			    atomic_add(&gui.vimTextArea->mouse_Drag_Event_ଗଣନା, -1);
		    ଅନ୍ୟଥା
			old_ଗଣନା = 0;
		    ଯଦି (old_ଗଣନା <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Vim_ସନ୍ଦେଶ::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Vim_ସନ୍ଦେଶ::Focus:
		gui.in_focus = vm.u.Focus.active;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		ଯଦି (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Vim_ସନ୍ଦେଶ::Refs:
		::RefsReceived(vm.u.Refs.message, vm.u.Refs.changedir);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Vim_ସନ୍ଦେଶ::Tabline:
		send_tabline_event(vm.u.Tabline.index);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Vim_ସନ୍ଦେଶ::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.index, vm.u.TablineMenu.event);
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		//  unrecognised message, ignore it
		ଭାଙ୍ଗ;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    ଫେରନ୍ତୁ size;
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

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_lock_screen()
{
    ଫେରନ୍ତୁ !gui.vimWindow || gui.vimWindow->Lock();
}

    ଶୂନ୍ୟ
vim_unlock_screen()
{
    ଯଦି (gui.vimWindow)
	gui.vimWindow->Unlock();
}

#define RUN_BAPPLICATION_IN_NEW_THREAD	0

#ଯଦି RUN_BAPPLICATION_IN_NEW_THREAD

    int32
run_vimapp(ଶୂନ୍ୟ *args)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.Run();		    // Run until Quit() called

    ଫେରନ୍ତୁ 0;
}

#ଅନ୍ୟଥା

    int32
call_main(ଶୂନ୍ୟ *args)
{
    ସଂରଚନା MainArgs *ma = (MainArgs *)args;

    ଫେରନ୍ତୁ ମୁଖ୍ୟ(ma->argc, ma->argv);
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    ଶୂନ୍ୟ
gui_mch_prepare(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*argc,
	ଅକ୍ଷର	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    ଯଦି (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(find_thread(ଖାଲି), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_port(B_MAX_PORT_ଗଣନା, "vim VDCMP");

#ଯଦି RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = spawn_thread(run_vimapp, "vim VimApp",
		tinfo.priority, ଖାଲି);
	ଯଦି (tid >= B_OK) {
	    resume_thread(tid);
	} ଅନ୍ୟଥା {
	    getout(1);
	}
#ଅନ୍ୟଥା
	MainArgs ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.priority, &ma);
	ଯଦି (tid >= B_OK) {
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
	    (ଶୂନ୍ୟ)wait_for_thread(tid, &dummy_exitcode);

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
    ଯଦି (!isatty(0)) {
	ସଂରଚନା stat stat_stdin, stat_dev_null;

	ଯଦି (fstat(0, &stat_stdin) == -1 ||
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init_check(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_back_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    gui.scrollbar_width = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) B_V_SCROLL_BAR_WIDTH;
    gui.scrollbar_height = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) B_H_SCROLL_BAR_HEIGHT;
#ifdef FEAT_MENU
    gui.menu_height = 19;   //	initial guess -
    //	correct for my default settings
#endif
    gui.border_offset = 3;  //	coordinates are inside window borders

    ଯଦି (gui.vdcmp < B_OK)
	ଫେରନ୍ତୁ FAIL;
    get_ଚାବି_map(&ଚାବି_Map, &ଚାବି_Map_Chars);

    gui.vimWindow = ନୂତନ VimWindow();	// hidden and locked
    ଯଦି (!gui.vimWindow)
	ଫେରନ୍ତୁ FAIL;

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

    ଫେରନ୍ତୁ OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    ଶୂନ୍ୟ
gui_mch_new_colors()
{
    rgb_color rgb = GUI_TO_RGB(gui.back_pixel);

    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimForm->SetViewColor(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vimWindow->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_open()
{
    ଯଦି (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimWindow->Show();
	gui.vimWindow->Unlock();
	ଫେରନ୍ତୁ OK;
    }

    ଫେରନ୍ତୁ FAIL;
}

    ଶୂନ୍ୟ
gui_mch_exit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା vim_exitcode)
{
    ଯଦି (gui.vimWindow) {
	thread_id tid = gui.vimWindow->Thread();
	gui.vimWindow->Lock();
	gui.vimWindow->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_port(gui.vdcmp);
#ଯଦି !RUN_BAPPLICATION_IN_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#endif
    ଯଦି (gui.vimApp) {
	VimTextAreaView::guiBlankMouse(ମିଥ୍ୟା);

	main_exitcode = vim_exitcode;
#ଯଦି RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->Lock();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_winpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y)
{
    ଯଦି (gui.vimWindow->Lock()) {
	BRect r;
	r = gui.vimWindow->Frame();
	gui.vimWindow->Unlock();
	*x = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)r.ବାମ;
	*y = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)r.ଶୀର୍ଷ;
	ଫେରନ୍ତୁ OK;
    }
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ଶୂନ୍ୟ
gui_mch_set_winpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
{
    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimWindow->MoveTo(x, y);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
ଶୂନ୍ୟ
gui_mch_set_shellsize(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	width,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	height,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	min_width,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	min_height,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	base_width,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	base_height,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimWindow->ResizeTo(width - PEN_WIDTH, height - PEN_WIDTH);

	// set size limits
	ଦଶମିକ minWidth, maxWidth, minHeight, maxHeight;

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

ଶୂନ୍ୟ
gui_mch_get_screen_dimensions(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*screen_w,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*screen_h)
{
    BRect frame;

    {
	BScreen screen(gui.vimWindow);

	ଯଦି (screen.IsValid()) {
	    frame = screen.Frame();
	} ଅନ୍ୟଥା {
	    frame.ଡାହାଣ = 640;
	    frame.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) frame.ଡାହାଣ - 2 * gui.scrollbar_width - 20;
    *screen_h = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) frame.bottom - gui.scrollbar_height
#ifdef FEAT_MENU
	- gui.menu_height
#endif
	- 30;
}

ଶୂନ୍ୟ
gui_mch_set_text_area_pos(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	x,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	y,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	w,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	h)
{
    ଯଦି (!gui.vimTextArea)
	ଫେରନ୍ତୁ;

    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimTextArea->MoveTo(x, y);
	gui.vimTextArea->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);

#ifdef FEAT_GUI_TABLINE
	ଯଦି (gui.vimForm->Tab_ଧାଡ଼ି() != ଖାଲି) {
	    gui.vimForm->Tab_ଧାଡ଼ି()->ResizeTo(w, gui.vimForm->TablineHeight());
	}
#endif // FEAT_GUI_TABLINE

	gui.vimWindow->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

ଶୂନ୍ୟ
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	flag)
{
    VimScrollBar *vsb = sb->id;
    ଯଦି (gui.vimWindow->Lock()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	ଯଦି (flag) {
	    ଯଦି (vsb->IsHidden()) {
		vsb->Show();
	    }
	} ଅନ୍ୟଥା {
	    ଯଦି (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vimWindow->Unlock();
    }
}

ଶୂନ୍ୟ
gui_mch_set_scrollbar_thumb(
	scrollbar_T *sb,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	val,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	size,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	max)
{
    ଯଦି (gui.vimWindow->Lock()) {
	VimScrollBar *s = sb->id;
	ଯଦି (max == 0) {
	    s->Set_ମୂଲ୍ୟ(0);
	    s->SetRange(0.0, 0.0);
	} ଅନ୍ୟଥା {
	    s->SetProportion((ଦଶମିକ)size / (max + 1.0));
	    s->SetSteps(1.0, size > 5 ? size - 2 : size);
#ifndef SCROLL_PAST_END	    //	really only defined in gui.c...
	    max = max + 1 - size;
#endif
	    ଯଦି (max < s->ମୂଲ୍ୟ_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->Set_ମୂଲ୍ୟ(val);
		s->SetRange(0.0, max);
	    } ଅନ୍ୟଥା {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->SetRange(0.0, max);
		s->Set_ମୂଲ୍ୟ(val);
	    }
	}
	gui.vimWindow->Unlock();
    }
}

ଶୂନ୍ୟ
gui_mch_set_scrollbar_pos(
	scrollbar_T *sb,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	x,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	y,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	w,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	h)
{
    ଯଦି (gui.vimWindow->Lock()) {
	BRect winb = gui.vimWindow->Bounds();
	ଦଶମିକ vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);
	ଯଦି (winb.ଡାହାଣ-(x+w)<w) vsbx = winb.ଡାହାଣ - (w - PEN_WIDTH);
	vsb->MoveTo(vsbx, vsby);
	gui.vimWindow->Unlock();
    }
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_scrollbar_xpadding(ଶୂନ୍ୟ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ଫେରନ୍ତୁ 0;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_scrollbar_ypadding(ଶୂନ୍ୟ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = ନୂତନ VimScrollBar(sb, posture);
    ଯଦି (gui.vimWindow->Lock()) {
	vsb->SetTarget(gui.vimTextArea);
	vsb->Hide();
	gui.vimForm->AddChild(vsb);
	gui.vimWindow->Unlock();
    }
}

#ଯଦି defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
ଶୂନ୍ୟ
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    ଯଦି (gui.vimWindow->Lock()) {
	sb->id->RemoveSelf();
	delete sb->id;
	gui.vimWindow->Unlock();
    }
}
#endif

/*
 * Cursor does not flash
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_is_blink_off(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ FALSE;
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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	blink_state = BLINK_NONE;
ସ୍ଥିର long_u	    blink_waittime = 700;
ସ୍ଥିର long_u	    blink_ontime = 400;
ସ୍ଥିର long_u	    blink_offtime = 250;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  blink_timer = 0;

ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
gui_mch_stop_blink(ପୂର୍ଣ୍ଣସଂଖ୍ୟା may_call_gui_update_cursor)
{
    // TODO
    ଯଦି (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    ଯଦି (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    ଶୂନ୍ୟ
gui_mch_start_blink()
{
    // TODO
    ଯଦି (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    ଯଦି (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init_font(
	char_u	    *font_name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    fontset)
{
    ଯଦି (gui.vimWindow->Lock())
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = gui.vimTextArea->mchInitFont(font_name);
	gui.vimWindow->Unlock();

	ଫେରନ୍ତୁ rc;
    }

    ଫେରନ୍ତୁ FAIL;
}


    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_adjust_charsize()
{
    ଫେରନ୍ତୁ FAIL;
}


    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_font_dialog(font_family* family, font_style* style, ଦଶମିକ* size)
{
#ଯଦି defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = ନୂତନ VimSelectFontDialog(family, style, size);
    ତାର୍କିକ ret = dialog->Go();
	delete dialog;
	ଫେରନ୍ତୁ ret;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ NOFONT;
#endif // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    giveErrorIfMissing)
{
    ସ୍ଥିର VimFont *fontList = ଖାଲି;

    ଯଦି (!gui.in_use)	//  can't do this when GUI not running
	ଫେରନ୍ତୁ NOFONT;

    //	storage for locally modified name;
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା buff_size = B_FONT_FAMILY_LENGTH + B_FONT_STYLE_LENGTH + 20;
    ସ୍ଥିର ଅକ୍ଷର font_name[buff_size] = {0};
    font_family family = {0};
    font_style	style  = {0};
    ଦଶମିକ size = 0.f;

    ଯଦି (name == 0 && be_fixed_font == 0) {
	ଯଦି (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	ଫେରନ୍ତୁ NOFONT;
    }

    ତାର୍କିକ useSelectGUI = ମିଥ୍ୟା;
    ଯଦି (name != ଖାଲି)
	ଯଦି (STRCMP(name, "*") == 0) {
	    useSelectGUI = ସତ୍ୟ;
	    STRNCPY(font_name, hl_get_font_name(), buff_size);
	} ଅନ୍ୟଥା
	    STRNCPY(font_name, name, buff_size);

    ଯଦି (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	size = be_fixed_font->Size();
	vim_snprintf(font_name, buff_size,
	    (ଅକ୍ଷର*)"%s/%s/%.0f", family, style, size);
    }

    //	replace underscores with spaces
    ଅକ୍ଷର* end = 0;
    ଯେତେବେଳେ (end = strchr((ଅକ୍ଷର *)font_name, '_'))
	*end = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    ସ୍ଥିର ଅକ୍ଷର buff[buff_size] = {0};
    STRNCPY(buff, font_name, buff_size);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_LENGTH);
    ଅକ୍ଷର* style_s = strtok(0, "/\0");
    ଯଦି (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_LENGTH);
    size = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    ଯଦି (useSelectGUI) {
	ଯଦି (gui_mch_font_dialog(&family, &style, &size) == NOFONT)
	    ଫେରନ୍ତୁ FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_size,
		(ଅକ୍ଷର*)"%s/%s/%.0f", family, style, size);
	hl_set_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	ଯଦି (new_p_guifont != ଖାଲି) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    ପାଇଁ ( ; *new_p_guifont; ++new_p_guifont)
		ଯଦି (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    ପାଇଁ (flp = fontList; flp; flp = flp->next) {
	ଯଦି (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    ଫେରନ୍ତୁ (GuiFont)flp;
	}
    }

    VimFont *font = ନୂତନ VimFont();
    font->name = vim_strsave((char_u*)font_name);

    ଯଦି (ଗଣନା_font_styles(family) <= 0) {
	ଯଦି (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	delete font;
	ଫେରନ୍ତୁ NOFONT;
    }

    //	Remember font in the static list for later use
    font->next = fontList;
    fontList = font;

    font->SetFamilyAndStyle(family, style);
    ଯଦି (size > 0.f)
	font->SetSize(size);

    font->SetSpacing(B_FIXED_SPACING);
    font->SetEncoding(B_UNICODE_UTF8);

    ଫେରନ୍ତୁ (GuiFont)font;
}

/*
 * Set the current text font.
 */
ଶୂନ୍ୟ
gui_mch_set_font(
	GuiFont	font)
{
    ଯଦି (gui.vimWindow->Lock()) {
	VimFont *vf = (VimFont *)font;

	gui.vimTextArea->SetFont(vf);

	gui.char_width = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) vf->StringWidth("n");
	font_height fh;
	vf->GetHeight(&fh);
	gui.char_height = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(fh.ascent + 0.9999)
	    + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(fh.descent + 0.9999) + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(fh.leading + 0.9999);
	gui.char_ascent = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(fh.ascent + 0.9999);

	gui.vimWindow->Unlock();
    }
}

// XXX TODO This is apparently never called...
ଶୂନ୍ୟ
gui_mch_free_font(
	GuiFont	font)
{
    ଯଦି (font == NOFONT)
	ଫେରନ୍ତୁ;
    VimFont *f = (VimFont *)font;
    ଯଦି (--f->refcount <= 0) {
	ଯଦି (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	delete f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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
	gui.char_height = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(fh.ascent + 0.5) + p_linespace / 2;
    }
    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
gui_mch_getmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y)
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

    ଶୂନ୍ୟ
gui_mch_mousehide(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hide)
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
    ଫେରନ୍ତୁ gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
ଶୂନ୍ୟ
gui_mch_set_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_ଉପର_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text background color.
 */
ଶୂନ୍ୟ
gui_mch_set_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_ତଳ_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    ଶୂନ୍ୟ
gui_mch_set_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

ଶୂନ୍ୟ
gui_mch_draw_string(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col,
	char_u	*s,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	flags)
{
    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchDrawString(row, col, s, len, flags);
	gui.vimWindow->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ପୂର୍ଣ୍ଣସଂଖ୍ୟା g, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b)
{
    ଫେରନ୍ତୁ gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_haskey(
	char_u	*name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; special_ଚାବିଗୁଡ଼ିକ[i].Be_ଚାବିଗୁଡ଼ିକ != 0; i++)
	ଯଦି (name[0] == special_ଚାବିଗୁଡ଼ିକ[i].vim_code0 &&
		name[1] == special_ଚାବିଗୁଡ଼ିକ[i].vim_code1)
	    ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ FAIL;
}

    ଶୂନ୍ୟ
gui_mch_beep()
{
    ::beep();
}

    ଶୂନ୍ୟ
gui_mch_flash(ପୂର୍ଣ୍ଣସଂଖ୍ୟା msec)
{
    // Do a visual beep by reversing the foreground and background colors

    ଯଦି (gui.vimWindow->Lock()) {
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
ଶୂନ୍ୟ
gui_mch_invert_rectangle(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	r,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nc)
{
    BRect rect;
    rect.ବାମ = FILL_X(c);
    rect.ଶୀର୍ଷ = FILL_Y(r);
    rect.ଡାହାଣ = rect.ବାମ + nc * gui.char_width - PEN_WIDTH;
    rect.bottom = rect.ଶୀର୍ଷ + nr * gui.char_height - PEN_WIDTH;

    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimTextArea->SetDrawingMode(B_OP_INVERT);
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->SetDrawingMode(B_OP_COPY);
	gui.vimWindow->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    ଶୂନ୍ୟ
gui_mch_iconify()
{
    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimWindow->Minimize(ସତ୍ୟ);
	gui.vimWindow->Unlock();
    }
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ଶୂନ୍ୟ
gui_mch_set_foreground(ଶୂନ୍ୟ)
{
    // TODO
}
#endif

/*
 * Set the window title
 */
ଶୂନ୍ୟ
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimWindow->SetTitle((ଅକ୍ଷର *)title);
	gui.vimWindow->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    ଶୂନ୍ୟ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.ବାମ = FILL_X(gui.col);
    r.ଶୀର୍ଷ = FILL_Y(gui.row);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cells = utf_off2cells(ଧାଡ଼ି_Offset[gui.row] + gui.col, 100); // TODO-TODO
    ଯଦି (cells>=4) cells = 1;
    r.ଡାହାଣ = r.ବାମ + cells*gui.char_width - PEN_WIDTH;
    r.bottom = r.ଶୀର୍ଷ + gui.char_height - PEN_WIDTH;

    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimTextArea->StrokeRect(r);
	gui.vimWindow->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
ଶୂନ୍ୟ
gui_mch_draw_part_cursor(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	w,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	h,
	guicolor_T  color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.ବାମ =
#ifdef FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_ଡାହାଣ ? FILL_X(gui.col + 1) - w :
#endif
	FILL_X(gui.col);
    r.ଡାହାଣ = r.ବାମ + w - PEN_WIDTH;
    r.bottom = FILL_Y(gui.row + 1) - PEN_WIDTH;
    r.ଶୀର୍ଷ = r.bottom - h + PEN_WIDTH;

    ଯଦି (gui.vimWindow->Lock()) {
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
    ଶୂନ୍ୟ
gui_mch_update()
{
    gui_mch_flush();
    ଯେତେବେଳେ (port_ଗଣନା(gui.vdcmp) > 0 &&
	    !vim_is_ନିବେଶ_buf_full() &&
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_wait_for_chars(
	long	wtime)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		focus;
    bigtime_t	until, timeout;
    status_t	st;

    ଯଦି (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_time() + timeout;
    }
    ଅନ୍ୟଥା
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.in_focus;
    ପାଇଁ (;;)
    {
	// Stop or start blinking when focus changes
	ଯଦି (gui.in_focus != focus)
	{
	    ଯଦି (gui.in_focus)
		gui_mch_start_blink();
	    ଅନ୍ୟଥା
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
	ଯଦି (did_add_timer)
	    // Need to recompute the waiting time.
	    ଭାଙ୍ଗ;
# endif
# ifdef FEAT_JOB_CHANNEL
	ଯଦି (has_any_channel())
	{
	    ଯଦି (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	ଅନ୍ୟଥା ଯଦି (wtime < 0)
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

	ଯଦି (ନିବେଶ_available())
	    ଫେରନ୍ତୁ OK;
	ଯଦି (st < B_OK)		// includes B_TIMED_OUT
	    ଫେରନ୍ତୁ FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	ଯଦି (wtime >= 0)
	{
	    timeout = until - system_time();
	    ଯଦି (timeout < 0)
		ଭାଙ୍ଗ;
	}
    }
    ଫେରନ୍ତୁ FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    ଶୂନ୍ୟ
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimWindow->Flush();
	gui.vimWindow->Unlock();
    }
    ଫେରନ୍ତୁ;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
ଶୂନ୍ୟ
gui_mch_clear_block(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row1,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col1,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row2,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	col2)
{
    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearBlock(row1, col1, row2, col2);
	gui.vimWindow->Unlock();
    }
}

    ଶୂନ୍ୟ
gui_mch_clear_all()
{
    ଯଦି (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearAll();
	gui.vimWindow->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
ଶୂନ୍ୟ
gui_mch_delete_ଧାଡ଼ିଗୁଡ଼ିକ(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    gui.vimTextArea->mch_Delete_ଧାଡ଼ିଗୁଡ଼ିକ(row, ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
ଶୂନ୍ୟ
gui_mch_ଭର୍ତ୍ତି_ଧାଡ଼ିଗୁଡ଼ିକ(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    gui.vimTextArea->mch_ଭର୍ତ୍ତି_ଧାଡ଼ିଗୁଡ଼ିକ(row, ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ);
}

#ଯଦି defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ଶୂନ୍ୟ
gui_mch_enable_menu(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	flag)
{
    ଯଦି (gui.vimWindow->Lock())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->SetEnabled(flag);
	gui.vimWindow->Unlock();
    }
}

ଶୂନ୍ୟ
gui_mch_set_menu_pos(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	x,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	y,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	w,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
ଶୂନ୍ୟ
gui_mch_add_menu(
	vimmenu_T   *menu,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx)
{
    vimmenu_T	*parent = menu->parent;

    //	popup menu - just create it unattached
    ଯଦି (menu_is_popup(menu->name) && parent == ଖାଲି) {
	BPopUpMenu* କାଢ଼_Up_Menu = ନୂତନ BPopUpMenu((ଅଚଳ ଅକ୍ଷର*)menu->name, ମିଥ୍ୟା, ମିଥ୍ୟା);
	menu->submenu_id = କାଢ଼_Up_Menu;
	menu->id = ଖାଲି;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (!menu_is_menubar(menu->name)
	    || (parent != ଖାଲି && parent->submenu_id == ଖାଲି))
	ଫେରନ୍ତୁ;

    ଯଦି (gui.vimWindow->Lock())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	ଯଦି ( parent )
	    tmp = parent->submenu_id;
	ଅନ୍ୟଥା
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	ଯଦି ( ! tmp->FindItem((ଅଚଳ ଅକ୍ଷର *) menu->dname)) {

	    BMenu *bmenu = ନୂତନ BMenu((ଅକ୍ଷର *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->AddItem(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->FindItem((ଅଚଳ ଅକ୍ଷର *) menu->dname);

	}
	gui.vimWindow->Unlock();
    }
}

    ଶୂନ୍ୟ
gui_mch_toggle_tearoffs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା enable)
{
    // no tearoff menus
}

    ସ୍ଥିର BMessage *
MenuMessage(vimmenu_T *menu)
{
    BMessage *m = ନୂତନ BMessage('menu');
    m->AddPointer("VimMenu", (ଶୂନ୍ୟ *)menu);

    ଫେରନ୍ତୁ m;
}

/*
 * Add a menu item to a menu
 */
ଶୂନ୍ୟ
gui_mch_add_menu_item(
	vimmenu_T   *menu,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    mnemonic = 0;
    vimmenu_T	*parent = menu->parent;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    ଯଦି (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	ଯଦି (menu_is_toolbar(parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ଯଦି (toolbar != ଖାଲି) {
		toolbar->AddButton(idx, menu);
	    }
	} ଅନ୍ୟଥା
#endif

	ଯଦି (parent->submenu_id != ଖାଲି || menu_is_popup(parent->name)) {
	    ଯଦି (menu_is_separator(menu->name)) {
		BSeparatorItem *item = ନୂତନ BSeparatorItem();
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = ଖାଲି;
	    }
	    ଅନ୍ୟଥା {
		BMenuItem *item = ନୂତନ BMenuItem((ଅକ୍ଷର *)menu->dname,
			MenuMessage(menu));
		item->SetTarget(gui.vimTextArea);
		item->SetTrigger((ଅକ୍ଷର) menu->mnemonic);
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = ଖାଲି;
	    }
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
ଶୂନ୍ୟ
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    ଯଦି (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	ଯଦି (menu->parent && menu_is_toolbar(menu->parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ଯଦି (toolbar != ଖାଲି) {
		toolbar->RemoveButton(menu);
	    }
	} ଅନ୍ୟଥା
#endif
	{
	    ପ୍ରତିଜ୍ଞା(menu->submenu_id == ଖାଲି || menu->submenu_id->ଗଣନା_ବସ୍ତୁଗୁଡ଼ିକ() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    ଯଦି (bmenu)
	    {
		bmenu->RemoveItem(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		ଯଦି (bmenu == gui.vimForm->MenuBar() && bmenu->ଗଣନା_ବସ୍ତୁଗୁଡ଼ିକ() == 0)
		{
		    bmenu->ResizeTo(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    delete menu->id;
	    menu->id = ଖାଲି;
	    menu->submenu_id = ଖାଲି;

	    gui.menu_height = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) gui.vimForm->MenuHeight();
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
ଶୂନ୍ୟ
gui_mch_menu_grey(
	vimmenu_T   *menu,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	grey)
{
#ifdef FEAT_TOOLBAR
    ଯଦି (menu->parent && menu_is_toolbar(menu->parent->name)) {
	ଯଦି (gui.vimWindow->Lock()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ଯଦି (toolbar != ଖାଲି) {
		toolbar->GrayButton(menu, grey);
	    }
	    gui.vimWindow->Unlock();
	}
    } ଅନ୍ୟଥା
#endif
    ଯଦି (menu->id != ଖାଲି)
	menu->id->SetEnabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
ଶୂନ୍ୟ
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	hidden)
{
    ଯଦି (menu->id != ଖାଲି)
	menu->id->SetEnabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    ଶୂନ୍ୟ
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    ଶୂନ୍ୟ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    ଯଦି (!menu_is_popup(menu->name) || menu->submenu_id == ଖାଲି)
	ଫେରନ୍ତୁ;

    BPopUpMenu* popupMenu = dynamic_cast<BPopUpMenu*>(menu->submenu_id);
    ଯଦି (popupMenu == ଖାଲି)
	ଫେରନ୍ତୁ;

    BPoint point;
    ଯଦି (gui.vimWindow->Lock()) {
	uint32 buttons = 0;
	gui.vimTextArea->GetMouse(&point, &buttons);
	gui.vimTextArea->ConvertToScreen(&point);
	gui.vimWindow->Unlock();
    }
    popupMenu->Go(point, ସତ୍ୟ);
}

#endif // FEAT_MENU

// Mouse stuff

#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
ଅକ୍ଷର textplain[] = "text/plain";
ଅକ୍ଷର vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    ଶୂନ୍ୟ
clip_mch_request_selection(Clipboard_T *cbd)
{
    ଯଦି (be_clipboard->Lock())
    {
	BMessage *m = be_clipboard->Data();
	// m->PrintToStream();

	char_u *string = ଖାଲି;
	ssize_t stringlen = -1;

	ଯଦି (m->FindData(textplain, B_MIME_TYPE,
		    (ଅଚଳ ଶୂନ୍ୟ **)&string, &stringlen) == B_OK
		|| m->FindString("text", (ଅଚଳ ଅକ୍ଷର **)&string) == B_OK)
	{
	    ଯଦି (stringlen == -1)
		stringlen = STRLEN(string);

	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
	    ଅକ୍ଷର *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    ଯଦି (m->FindData(vimselectiontype, B_MIME_TYPE,
			(ଅଚଳ ଶୂନ୍ୟ **)&seltype, &seltypelen) == B_OK)
	    {
		ବାଛ (*seltype)
		{
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    ସ୍ଥିତି 'L':	type = MLINE;	ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'C':	type = MCHAR;	ଭାଙ୍ଗ;
#ifdef FEAT_VISUAL
		    ସ୍ଥିତି 'B':	type = MBLOCK;	ଭାଙ୍ଗ;
#endif
		}
	    }
	    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    ଶୂନ୍ୟ
clip_mch_set_selection(Clipboard_T *cbd)
{
    ଯଦି (be_clipboard->Lock())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->Data();
	ପ୍ରତିଜ୍ଞା(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = ଖାଲି;
	long_u	ଗଣନା_2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;

	type = clip_convert_selection(&str, &ଗଣନା_2, cbd);

	ଯଦି (type < 0)
	    ଫେରନ୍ତୁ;

	m->AddData(textplain, B_MIME_TYPE, (ଶୂନ୍ୟ *)str, ଗଣନା_2);

	// Add type of selection
	ଅକ୍ଷର	vtype;
	ବାଛ (type)
	{
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ସ୍ଥିତି MLINE:    vtype = 'L';    ଭାଙ୍ଗ;
	    ସ୍ଥିତି MCHAR:    vtype = 'C';    ଭାଙ୍ଗ;
#ifdef FEAT_VISUAL
	    ସ୍ଥିତି MBLOCK:   vtype = 'B';    ଭାଙ୍ଗ;
#endif
	}
	m->AddData(vimselectiontype, B_MIME_TYPE, (ଶୂନ୍ୟ *)&vtype, 1);

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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = ନୂତନ BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    ଖାଲି, ଖାଲି, 0, ମିଥ୍ୟା,
	    ନୂତନ BMessage((saving == TRUE) ? 'save' : 'open'), ଖାଲି, ସତ୍ୟ);

    gui.vimApp->fBrowsedPath.Unset();

    gui.vimApp->fFilePanel->Window()->SetTitle((ଅକ୍ଷର*)title);
    gui.vimApp->fFilePanel->SetPanelDirectory((ଅଚଳ ଅକ୍ଷର*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    ଯେତେବେଳେ (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = ଖାଲି;
    status_t result = gui.vimApp->fBrowsedPath.InitCheck();
    ଯଦି (result == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->fBrowsedPath.Path());
    } ଅନ୍ୟଥା
	ଯଦି (result != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    result, strerror(result));
	}

    delete gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = ଖାଲି;

    ଫେରନ୍ତୁ fileName;
}
#endif // FEAT_BROWSE


#ଯଦି defined(FEAT_GUI_DIALOG)

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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_dialog(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	 type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	 dfltbutton,
	char_u	*textfield,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ex_cmd)
{
    VimDialog *dialog = ନୂତନ VimDialog(type, (ଅକ୍ଷର*)title, (ଅକ୍ଷର*)message,
	    (ଅକ୍ଷର*)buttons, dfltbutton, (ଅକ୍ଷର*)textfield, ex_cmd);
    ତାର୍କିକ ret = dialog->Go();
    delete dialog;
	ଫେରନ୍ତୁ ret;
}

#endif // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_TO_RGB(pixel);

    ଫେରନ୍ତୁ ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    ଶୂନ୍ୟ
gui_mch_setmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
{
    TRACE();
    // TODO
}

#ifdef FEAT_MBYTE_IME
    ଶୂନ୍ୟ
im_set_position(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ଯଦି (gui.vimWindow->Lock())
    {
	gui.vimTextArea->DrawIMString();
	gui.vimWindow->Unlock();
    }
    ଫେରନ୍ତୁ;
}
#endif

    ଶୂନ୍ୟ
gui_mch_show_toolbar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_height = (toolbar && showit) ? toolbar->ToolbarHeight() : 0.;
}

    ଶୂନ୍ୟ
gui_mch_set_toolbar_pos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    ଯଦି (toolbar != ଖାଲି) {
	ଯଦି (gui.vimWindow->Lock()) {
	    toolbar->MoveTo(x, y);
	    toolbar->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

#ଯଦି defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    ଶୂନ୍ୟ
gui_mch_show_tabline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା showit)
{
    Vim_Tab_ଧାଡ଼ି *tab_ଧାଡ଼ି = gui.vimForm->Tab_ଧାଡ଼ି();

    ଯଦି (tab_ଧାଡ଼ି == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (!showit != !gui.vimForm->Is_Showing_Tab_ଧାଡ଼ି()) {
	gui.vimForm->Set_Showing_Tab_ଧାଡ଼ି(showit != 0);
	gui.tabline_height = gui.vimForm->TablineHeight();
    }
}

    ଶୂନ୍ୟ
gui_mch_set_tabline_pos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h)
{
    Vim_Tab_ଧାଡ଼ି *tab_ଧାଡ଼ି = gui.vimForm->Tab_ଧାଡ଼ି();
    ଯଦି (tab_ଧାଡ଼ି != ଖାଲି) {
	ଯଦି (gui.vimWindow->Lock()) {
	    tab_ଧାଡ଼ି->MoveTo(x, y);
	    tab_ଧାଡ଼ି->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_showing_tabline()
{
    Vim_Tab_ଧାଡ଼ି *tab_ଧାଡ଼ି = gui.vimForm->Tab_ଧାଡ଼ି();
    ଫେରନ୍ତୁ tab_ଧାଡ଼ି != ଖାଲି && gui.vimForm->Is_Showing_Tab_ଧାଡ଼ି();
}

/*
 * Update the labels of the tabline.
 */
    ଶୂନ୍ୟ
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nr = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    curtabidx = 0;

    Vim_Tab_ଧାଡ଼ି *tab_ଧାଡ଼ି = gui.vimForm->Tab_ଧାଡ଼ି();

    ଯଦି (tab_ଧାଡ଼ି == ଖାଲି)
	ଫେରନ୍ତୁ;

    gui.vimWindow->Lock();

    // Add a label for each tab page.  They all contain the same text area.
    ପାଇଁ (tp = first_tabpage; tp != ଖାଲି; tp = tp->tp_next, ++nr) {
	ଯଦି (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_ଧାଡ଼ି->TabAt(nr);

	ଯଦି (tab == ଖାଲି) {
	    tab = ନୂତନ Vim_Tab_ଧାଡ଼ି::VimTab();
	    tab_ଧାଡ଼ି->AddTab(ଖାଲି, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->SetLabel((ଅଚଳ ଅକ୍ଷର*)NameBuff);
	tab_ଧାଡ଼ି->Invalidate();
    }

    // Remove any old labels.
    ଯେତେବେଳେ (nr < tab_ଧାଡ଼ି->ଗଣନା_Tabs())
	tab_ଧାଡ଼ି->RemoveTab(nr);

    ଯଦି (tab_ଧାଡ଼ି->Selection() != curtabidx)
	tab_ଧାଡ଼ି->Select(curtabidx);

    gui.vimWindow->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ଶୂନ୍ୟ
gui_mch_set_curtab(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr)
{
    Vim_Tab_ଧାଡ଼ି *tab_ଧାଡ଼ି = gui.vimForm->Tab_ଧାଡ଼ି();
    ଯଦି (tab_ଧାଡ଼ି == ଖାଲି)
	ଫେରନ୍ତୁ;

    gui.vimWindow->Lock();

    ଯଦି (tab_ଧାଡ଼ି->Selection() != nr -1)
	tab_ଧାଡ଼ି->Select(nr -1);

    gui.vimWindow->Unlock();
}

#endif // FEAT_GUI_TABLINE
