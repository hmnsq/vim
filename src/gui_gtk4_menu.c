/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

#اگر_تعریف_شده FEAT_MENU

#ضمن <gtk/gtk.h>
#ضمن "gui_gtk4_menu.h"

// Note that this may return NULL for popup menus
#تعریف_کن GET_MENU_BAR(m) VIM_MENU_BAR(gtk_widget_get_ancestor( \
	    GTK_WIDGET(m), VIM_TYPE_MENU_BAR))

/*
 * Similar as GtkButton but set CSS name to "item" to emulate GtkPopoverMenuBar
 * styling. Always has a submenu.
 */
بنية Vim_Menu_Bar_قلم
{
    GtkButton والد;

    GtkWidget *menu;
};

G_DEFINE_TYPE(Vim_Menu_Bar_قلم_2, vim_menu_bar_قلم, GTK_TYPE_BUTTON)

    ساكن فراغ
vim_menu_bar_قلم_dispose(GObject *object)
{
    Vim_Menu_Bar_قلم_2 *self = VIM_MENU_BAR_قلم(object);

    g_clear_اشاره_گر((GtkWidget **)&self->menu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_قلم_والد_class)->dispose(object);
}

    ساكن فراغ
vim_menu_bar_قلم_class_init(Vim_Menu_Bar_قلم_Class *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_قلم_dispose;

    gtk_widget_class_مجموعه_css_name(widget_class, "item");
}

    ساكن فراغ
vim_menu_bar_قلم_init(Vim_Menu_Bar_قلم_2 *self)
{
    // Enable mnemonics
    gtk_button_مجموعه_use_underline(GTK_BUTTON(self), TRUE);
}

    GtkWidget *
vim_menu_bar_قلم_new(ثابت حرف *متن, VimMenu *menu)
{
    Vim_Menu_Bar_قلم_2 *قلم = g_object_new(VIM_TYPE_MENU_BAR_قلم, عدم);

    gtk_button_مجموعه_label(GTK_BUTTON(قلم), متن);

    قلم->menu = GTK_WIDGET(menu);
    gtk_popover_مجموعه_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    // Make popover start at top left corner
    gtk_widget_مجموعه_halign(GTK_WIDGET(menu), GTK_ALIGN_شروع);
    gtk_widget_مجموعه_والد(GTK_WIDGET(menu), GTK_WIDGET(قلم));

    أرجع GTK_WIDGET(قلم);
}

    فراغ
vim_menu_bar_قلم_مجموعه_متن(Vim_Menu_Bar_قلم_2 *self, ثابت حرف *متن)
{
    gtk_button_مجموعه_label(GTK_BUTTON(self), متن);
}

/*
 * Similar to GtkPopoverMenuBar
 */
بنية _VimMenuBar
{
    GtkWidget والد;

    GList *اقلام;

    // Currently visible item that has submenu popped up, else NULL
    GtkWidget *فعال_قلم;
};

G_DEFINE_TYPE(VimMenuBar, vim_menu_bar, GTK_TYPE_WIDGET)

    ساكن فراغ
vim_menu_bar_dispose(GObject *object)
{
    VimMenuBar *self = VIM_MENU_BAR(object);

    g_clear_list(&self->اقلام, (GDestroyNotify)gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_والد_class)->dispose(object);
}

    ساكن فراغ
vim_menu_bar_class_init(VimMenuBarClass *class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_dispose;

    gtk_widget_class_مجموعه_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_مجموعه_css_name(widget_class, "menubar");
}

    ساكن فراغ
vim_menu_bar_init(VimMenuBar *self)
{
    GtkLayoutManager *lm = gtk_widget_get_layout_manager(GTK_WIDGET(self));

    gtk_orientable_مجموعه_orientation(GTK_ORIENTABLE(lm),
	    GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_مجموعه_spacing(GTK_BOX_LAYOUT(lm), 0);
}

    GtkWidget *
vim_menu_bar_new(فراغ)
{
    أرجع g_object_new(VIM_TYPE_MENU_BAR, عدم);
}

/*
 * Create a VimMenu widget with the menus of the menu bar as its submenus. Note
 * that it is a deep copy.
 */
    GtkWidget *
vim_menu_bar_مقصد_menu(VimMenuBar *self)
{
    GtkWidget	*menu = vim_menu_new();
    صحيح		i = 0;

    لكل (GList *l = self->اقلام; l != عدم; l = l->بعدی, i++)
    {
	Vim_Menu_Bar_قلم_2	*baritem = l->داده;
	GtkWidget	*قلم;

	قلم = vim_menu_قلم_new(
		gtk_button_get_label(GTK_BUTTON(baritem)), عدم, عدم);

	vim_menu_قلم_مجموعه_submenu(VIM_MENU_قلم(قلم),
		VIM_MENU(vim_menu_copy(VIM_MENU(baritem->menu))));

	vim_menu_درج_قلم(VIM_MENU(menu), VIM_MENU_قلم(قلم), i);
    }
    أرجع menu;
}

/*
 * Set the currently active menu of the menubar to "item". If NULL, then close
 * any submenus.
 */
    ساكن فراغ
vim_menu_bar_مجموعه_فعال_قلم(
	VimMenuBar	*self,
	Vim_Menu_Bar_قلم_2	*قلم,
	gboolean	force)
{
    // Do nothing if currently active item is "item", or if there is not
    // currently active item. User must click a menu item first for menus to
    // automatically appear on hover. This is unless "force" is TRUE.
    //
    // Only make item selected if there is no active item (no submenu open), or
    // if the item was set as the active item..
    إذا ((!force && self->فعال_قلم == عدم)
	    || self->فعال_قلم == GTK_WIDGET(قلم))
    {
	إذا (self->فعال_قلم == عدم)
	    gtk_widget_مجموعه_state_flags(GTK_WIDGET(قلم),
		    GTK_STATE_FLAG_SELECTED, FALSE);
	أرجع;
    }

    إذا (self->فعال_قلم != عدم)
    {
	// Call this before popdown, since "closed" signal may be emitted
	// immediately.
	gtk_widget_unset_state_flags(self->فعال_قلم,
		GTK_STATE_FLAG_SELECTED);
	gtk_popover_popdown(GTK_POPOVER(
		    VIM_MENU_BAR_قلم(self->فعال_قلم)->menu)
		);
    }

    self->فعال_قلم = GTK_WIDGET(قلم);
    إذا (قلم != عدم)
    {
	gtk_popover_popup(GTK_POPOVER(قلم->menu));
	gtk_widget_مجموعه_state_flags(GTK_WIDGET(قلم),
		GTK_STATE_FLAG_SELECTED, FALSE);
    }
}

    ساكن فراغ
vim_menu_bar_قلم_enter_cb(
	GtkEventController  *controller,
	مزدوج		    x استفادهنشده,
	مزدوج		    y استفادهنشده,
	VimMenuBar	    *menubar)
{
    Vim_Menu_Bar_قلم_2  *self;

    self = VIM_MENU_BAR_قلم(gtk_event_controller_get_widget(controller));
    vim_menu_bar_مجموعه_فعال_قلم(menubar, self, FALSE);
}

    ساكن فراغ
vim_menu_bar_قلم_leave_cb(
	GtkEventController  *controller,
	VimMenuBar	    *menubar)
{
    Vim_Menu_Bar_قلم_2 *self;

    self = VIM_MENU_BAR_قلم(gtk_event_controller_get_widget(controller));

    // If the item is the currently active item, then don't deselect it.
    إذا (menubar->فعال_قلم != GTK_WIDGET(self))
	gtk_widget_unset_state_flags(GTK_WIDGET(self),
		GTK_STATE_FLAG_SELECTED);
}

    ساكن فراغ
vim_menu_bar_قلم_clicked_cb(Vim_Menu_Bar_قلم_2 *self, VimMenuBar *menubar)
{
    vim_menu_bar_مجموعه_فعال_قلم(menubar, self, TRUE);
}

    ساكن فراغ
vim_menu_bar_قلم_menu_closed_cb(VimMenu *menu استفادهنشده, VimMenuBar *menubar)
{
    إذا (menubar->فعال_قلم != عدم)
	gtk_widget_unset_state_flags(GTK_WIDGET(menubar->فعال_قلم),
		GTK_STATE_FLAG_SELECTED);
    vim_menu_bar_مجموعه_فعال_قلم(menubar, عدم, TRUE);
    // Make sure to focus drawarea
    gtk_widget_grab_focus(gui.drawarea);
}

/*
 * Insert the menu item at the given index in the menu bar.
 */
    فراغ
vim_menu_bar_درج_قلم(VimMenuBar *self, Vim_Menu_Bar_قلم_2 *قلم, صحيح idx)
{
    GtkEventController	*controller;
    GList		*بعدی_sibling;

    بعدی_sibling = g_list_nth(self->اقلام, idx);
    gtk_widget_درج_before(GTK_WIDGET(قلم), GTK_WIDGET(self),
	    بعدی_sibling == عدم ? عدم : بعدی_sibling->داده);

    self->اقلام = g_list_درج(self->اقلام, قلم, idx);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_bar_قلم_enter_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(controller, "leave",
	    G_CALLBACK(vim_menu_bar_قلم_leave_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_جمع_کن_controller(GTK_WIDGET(قلم), controller);

    g_signal_connect_object(قلم, "clicked",
	    G_CALLBACK(vim_menu_bar_قلم_clicked_cb), self, G_CONNECT_DEFAULT);

    g_signal_connect_object(قلم->menu, "closed",
	    G_CALLBACK(vim_menu_bar_قلم_menu_closed_cb),
	    self, G_CONNECT_DEFAULT);
}

/*
 * Remove the menu item or separator from the menu bar
 */
    فراغ
vim_menu_bar_remove(VimMenuBar *self, GtkWidget *قلم)
{
    self->اقلام = g_list_remove(self->اقلام, قلم);
    gtk_widget_unparent(قلم);
}

/*
 * Show the given menu in the menubar. If "item" is NULL, then show first menu.
 */
    فراغ
vim_menu_bar_show(VimMenuBar *self, Vim_Menu_Bar_قلم_2 *قلم)
{
    إذا (قلم == عدم)
	قلم = g_list_nth_داده(self->اقلام, 0);

    vim_menu_bar_مجموعه_فعال_قلم(self, قلم, TRUE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    ساكن GtkWidget *
vim_menu_bar_move_فعال_قلم(VimMenuBar *self, صحيح dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->فعال_قلم;

    إذا (widget == عدم)
	أرجع gtk_widget_get_اولی_فرزند(GTK_WIDGET(self));

    إذا (dir > 0)
    {
	func = gtk_widget_get_بعدی_sibling;
	null_func = gtk_widget_get_اولی_فرزند;
    }
    وإلا
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_آخرین_فرزند;
    }

    طالما (TRUE)
    {
	widget = func(widget);
	إذا (widget == عدم)
	{
	    إذا (null_func == عدم)
		اكسر;
	    widget = null_func(GTK_WIDGET(self));
	    null_func = عدم;
	    إذا (widget == عدم)
		اكسر;
	}
	اكسر;
    }
    أرجع widget;
}

/*
 * Menu button that can be used to perform actions, or if there is a submenu,
 * toggle the state of the submenu popover. CSS name is "modelbutton" to make it
 * styled like GtkPopoverMenu
 */
بنية Vim_Menu_قلم_2
{
    GtkButton والد;

    GtkWidget *label;	    // Displays text for button.
    GtkWidget *aux_widget;  // Either an icon or a label showing the accelerator
			    // text.

    GtkWidget *submenu;	    // Submenu popover if any (VimMenu)

    // Callback called when clicked or selected, we store this so that copying a
    // menu item works properly.
    Vim_Menu_قلم_Func	func;
    فراغ		*func_udata;
};

G_DEFINE_TYPE(Vim_Menu_قلم, vim_menu_قلم, GTK_TYPE_BUTTON)

    ساكن فراغ
vim_menu_قلم_dispose(GObject *object)
{
    Vim_Menu_قلم *self = VIM_MENU_قلم(object);

    g_clear_اشاره_گر(&self->label, gtk_widget_unparent);
    g_clear_اشاره_گر(&self->aux_widget, gtk_widget_unparent);
    g_clear_اشاره_گر((GtkWidget **)&self->submenu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_قلم_والد_class)->dispose(object);
}

    ساكن فراغ
vim_menu_قلم_class_init(Vim_Menu_قلم_Class *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_قلم_dispose;

    gtk_widget_class_مجموعه_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_مجموعه_css_name(widget_class, "modelbutton");
}

    ساكن فراغ
vim_menu_قلم_init(Vim_Menu_قلم *self)
{
    GtkLayoutManager *lm = gtk_widget_get_layout_manager(GTK_WIDGET(self));

    gtk_orientable_مجموعه_orientation(GTK_ORIENTABLE(lm),
	    GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_مجموعه_spacing(GTK_BOX_LAYOUT(lm), 0);
}

/*
 * Create a new menu item with the given text to display. "func" may be NULL if
 * not needed.
 */
    GtkWidget *
vim_menu_قلم_new(ثابت حرف *متن, Vim_Menu_قلم_Func func, فراغ *udata)
{
    Vim_Menu_قلم *قلم = g_object_new(VIM_TYPE_MENU_قلم, عدم);

    قلم->func = func;
    قلم->func_udata = udata;
    قلم->label = gtk_label_new_with_mnemonic(متن);

    // Make sure label is on the right and pushes everything to the left
    gtk_widget_مجموعه_halign(قلم->label, GTK_ALIGN_شروع);
    gtk_widget_مجموعه_hexpand(قلم->label, TRUE);
    gtk_widget_مجموعه_والد(قلم->label, GTK_WIDGET(قلم));

    أرجع GTK_WIDGET(قلم);
}

/*
 * Update displayed text for menu item
 */
    فراغ
vim_menu_قلم_مجموعه_متن(Vim_Menu_قلم *self, ثابت حرف *متن)
{
    gtk_label_مجموعه_متن_with_mnemonic(GTK_LABEL(self->label), متن);
}

    ساكن فراغ
vim_menu_قلم_مجموعه_aux_widget(Vim_Menu_قلم *self, GtkWidget *aux)
{
    self->aux_widget = aux;
    gtk_widget_مجموعه_halign(self->aux_widget, GTK_ALIGN_انتها);
    gtk_widget_مجموعه_hexpand(self->aux_widget, FALSE);
    gtk_widget_مجموعه_margin_شروع(self->aux_widget, 50);
}

/*
 * Set the accelerator text for the menu item.
 */
    فراغ
vim_menu_قلم_مجموعه_accel(Vim_Menu_قلم *self, ثابت حرف *accel_متن)
{
    أكد(self->aux_widget == عدم);

    vim_menu_قلم_مجموعه_aux_widget(self, gtk_label_new(accel_متن));
    gtk_widget_درج_after(self->aux_widget, GTK_WIDGET(self), self->label);
}

/*
 * Set the submenu popover for the menu item
 */
    فراغ
vim_menu_قلم_مجموعه_submenu(Vim_Menu_قلم *self, VimMenu *submenu)
{
    GtkWidget *icon;

    أكد(self->submenu == عدم);
    أكد(self->aux_widget == عدم);

    // Add arrow icon pointing to right
    icon = gtk_image_new_from_icon_name("pan-end-symbolic");
    vim_menu_قلم_مجموعه_aux_widget(self, icon);
    gtk_widget_درج_after(self->aux_widget, GTK_WIDGET(self), self->label);

    gtk_popover_مجموعه_position(GTK_POPOVER(submenu), GTK_POS_راست);
    // Make top of popover be aligned with button.
    gtk_widget_مجموعه_valign(GTK_WIDGET(submenu), GTK_ALIGN_شروع);

    self->submenu = GTK_WIDGET(submenu);
    gtk_widget_مجموعه_والد(GTK_WIDGET(submenu), GTK_WIDGET(self));
}

/*
 * Create a deep copy of the menu item
 */
    ساكن GtkWidget *
vim_menu_قلم_copy(Vim_Menu_قلم *self)
{
    GtkWidget *copy;

    copy = vim_menu_قلم_new(
	    gtk_label_get_متن(GTK_LABEL(self->label)),
	    self->func, self->func_udata);

    إذا (self->submenu != عدم)
	vim_menu_قلم_مجموعه_submenu(VIM_MENU_قلم(copy),
		VIM_MENU(vim_menu_copy(VIM_MENU(self->submenu))));
    وإلا إذا (self->aux_widget != عدم)
	vim_menu_قلم_مجموعه_accel(VIM_MENU_قلم(copy),
		gtk_label_get_متن(GTK_LABEL(self->aux_widget)));
    أرجع copy;
}

/*
 * Similar to GtkPopoverMenu, except uses GtkWidgets directly like GTK3, instead
 * of abstracting it into GMenuModel.
 */
بنية _VimMenu
{
    GtkPopover والد;

    GtkWidget *box;
    GtkWidget *scr;

    GList *اقلام;

    // Currently active item showing submenu popover, or being hovered on, or
    // NULL. Note that item may have submenu but not have it open, when
    // navigating via keyboard.
    GtkWidget *فعال_قلم;

    // Used when mouse is hovering over an item and user is navigating with
    // keyboard. When the scrolled window scrolls down or up, this causes a
    // mouse enter event, causing the active item to go to the item that the
    // mouse is hovered on, instead of the next item (from keyboard navigation).
    gboolean	ignore_hover;
    مزدوج	prev_x;
    مزدوج	prev_y;
};

G_DEFINE_TYPE(VimMenu, vim_menu, GTK_TYPE_POPOVER)

    ساكن فراغ
vim_menu_dispose(GObject *object)
{
    VimMenu *self = VIM_MENU(object);

    g_clear_list(&self->اقلام, (GDestroyNotify)gtk_widget_unparent);
    g_clear_اشاره_گر(&self->scr, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_والد_class)->dispose(object);
}

    ساكن فراغ
vim_menu_class_init(VimMenuClass *class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_dispose;
}

    ساكن gboolean
vim_menu_select_فعال_قلم(VimMenu *self, gboolean open)
{
    Vim_Menu_قلم *قلم;

    gtk_widget_مجموعه_state_flags(self->فعال_قلم,
	    GTK_STATE_FLAG_SELECTED, FALSE);

    // Make sure to focus item, so that scrolled window knows what to do.
    gtk_widget_grab_focus(GTK_WIDGET(self->فعال_قلم));

    قلم = VIM_MENU_قلم(self->فعال_قلم);
    إذا (قلم->func != عدم)
	قلم->func(قلم, VIM_MENU_قلم_SELECTED, قلم->func_udata);

    إذا (open && VIM_MENU_قلم(self->فعال_قلم)->submenu != عدم)
    {
	GtkWidget *submenu = VIM_MENU_قلم(self->فعال_قلم)->submenu;
	gtk_popover_popup(GTK_POPOVER(submenu));
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Set the active item of the menu to "item". If "item" is NULL, then close any
 * submenus. If "open" is FALSE, then don't open the submenu if any.
 */
    ساكن فراغ
vim_menu_مجموعه_فعال_قلم(VimMenu *self, Vim_Menu_قلم *قلم, gboolean open)
{
    إذا (self->فعال_قلم == GTK_WIDGET(قلم))
	أرجع;

    إذا (self->فعال_قلم != عدم)
    {
	إذا (VIM_MENU_قلم(self->فعال_قلم)->submenu != عدم)
	    gtk_popover_popdown(GTK_POPOVER(
			VIM_MENU_قلم(self->فعال_قلم)->submenu
			));
	gtk_widget_unset_state_flags(GTK_WIDGET(self->فعال_قلم),
		GTK_STATE_FLAG_SELECTED);
    }

    self->فعال_قلم = GTK_WIDGET(قلم);
    إذا (قلم != عدم)
	(فراغ)vim_menu_select_فعال_قلم(self, open);
}

    ساكن فراغ
vim_menu_closed_cb(VimMenu *self, فراغ *udata استفادهنشده)
{
    vim_menu_مجموعه_فعال_قلم(self, عدم, FALSE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    ساكن GtkWidget *
vim_menu_move_فعال_قلم(VimMenu *self, صحيح dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->فعال_قلم;

    // If there is no currently active item, then just use the first one
    إذا (widget == عدم)
    {
	widget = gtk_widget_get_اولی_فرزند(self->box);
	طالما (widget != عدم && !VIM_IS_MENU_قلم(widget))
	    widget = gtk_widget_get_بعدی_sibling(widget);
	أرجع widget;
    }

    // Could also just use GList functions, but this seems simpler (no
    // difference anyways).
    إذا (dir > 0)
    {
	func = gtk_widget_get_بعدی_sibling;
	null_func = gtk_widget_get_اولی_فرزند;
    }
    وإلا
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_آخرین_فرزند;
    }

    طالما (TRUE)
    {
	widget = func(widget);
	إذا (widget == عدم)
	{
	    إذا (null_func == عدم)
		اكسر;
	    widget = null_func(self->box);
	    null_func = عدم;
	    إذا (widget == عدم)
		اكسر;
	}
	إذا (VIM_IS_MENU_قلم(widget))
	    اكسر;
    }
    أرجع widget;
}

    ساكن فراغ
vim_menu_reset_والد_prelight(VimMenu *self)
{
    GtkWidget	*والد = gtk_widget_get_والد(GTK_WIDGET(self));
    VimMenu	*والد_menu;

    // gtk_widget_get_ancestor assumes the widget itself is also an ancestor, so
    // we must get parent of menu first.
    والد_menu = VIM_MENU(gtk_widget_get_ancestor(والد, VIM_TYPE_MENU));

    إذا (والد_menu == عدم)
	// TRUE for popup menus
	أرجع;

    إذا (والد_menu->فعال_قلم != عدم)
	gtk_widget_unset_state_flags(GTK_WIDGET(والد_menu->فعال_قلم),
		GTK_STATE_FLAG_PRELIGHT);
}

/*
 * Close all submenus in the menubar given a menu widget
 */
    ساكن فراغ
vim_menu_close_all(VimMenu *self)
{
    VimMenuBar *menubar = GET_MENU_BAR(self);

    // Must check if NULL, because popup menus don't have a parent.
    إذا (menubar != عدم)
	vim_menu_bar_مجموعه_فعال_قلم(menubar, عدم, TRUE);
    وإلا
	gtk_popover_popdown(GTK_POPOVER(self));

    // Grab focus after popup menus without a menubar are closed
    gtk_widget_grab_focus(gui.drawarea);
}

    ساكن gboolean
vim_menu_کلید_pressed_cb(
	GtkEventController  *controller استفادهنشده,
	guint		    keyval,
	guint		    keycode استفادهنشده,
	GdkModifierType	    state,
	VimMenu		    *self)
{
    GtkWidget	*widget;

    بدل (keyval)
    {
	حالة GDK_کلید_پایین:
	حالة GDK_کلید_KP_پایین:
	حالة GDK_کلید_به_بالا:
	حالة GDK_کلید_KP_به_بالا:
	حالة GDK_کلید_Tab:
	حالة GDK_کلید_KP_Tab:
	حالة GDK_کلید_ISO_چپ_Tab:
	    // Go to the previous or next item if any
	    widget = vim_menu_move_فعال_قلم(self,
		    (state & GDK_لغزش_ماسک)
		    || keyval == GDK_کلید_به_بالا ? -1 : 1);
	    vim_menu_مجموعه_فعال_قلم(self, VIM_MENU_قلم(widget), FALSE);
	    self->ignore_hover = TRUE;
	    أرجع TRUE;
	حالة GDK_کلید_چپ:
	    // Pressing control switches menu bar item.
	    إذا (state & GDK_CONTROL_ماسک)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		إذا (menubar != عدم)
		{
		    widget = vim_menu_bar_move_فعال_قلم(menubar, -1);
		    vim_menu_bar_مجموعه_فعال_قلم(menubar,
			    VIM_MENU_BAR_قلم(widget), TRUE);
		}
		أرجع TRUE;
	    }
	    // Go to parent menu (if any). We can do this by just closing the
	    // popover.
	    gtk_popover_popdown(GTK_POPOVER(self));
	    // For some reason when pointer is hovered over draw area, the
	    // active item in the parent menu will stay prelighted even when the
	    // active item is moved.
	    vim_menu_reset_والد_prelight(self);
	    أرجع TRUE;
	حالة GDK_کلید_راست:
	    إذا (state & GDK_CONTROL_ماسک)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		إذا (menubar != عدم)
		{
		    widget = vim_menu_bar_move_فعال_قلم(menubar, 1);
		    vim_menu_bar_مجموعه_فعال_قلم(menubar,
			    VIM_MENU_BAR_قلم(widget), TRUE);
		}
		أرجع TRUE;
	    }
	    // Open submenu if active item has one
	    إذا (self->فعال_قلم != عدم
		    && vim_menu_select_فعال_قلم(self, TRUE))
	    {
		// Select first item in opened submenu
		VimMenu *submenu = VIM_MENU(
			VIM_MENU_قلم(self->فعال_قلم)->submenu
			);

		vim_menu_مجموعه_فعال_قلم(submenu,
			VIM_MENU_قلم(
			    gtk_widget_get_اولی_فرزند(submenu->box)),
			FALSE);
	    }
	    self->ignore_hover = TRUE;
	    أرجع TRUE;
	حالة GDK_کلید_Escape:
	    // Close all popover menus
	    vim_menu_close_all(self);
	    أرجع TRUE;
	حالة GDK_کلید_ISO_Enter:
	حالة GDK_کلید_3270_Enter:
	حالة GDK_کلید_KP_Enter:
	حالة GDK_کلید_Return:
	    إذا (self->فعال_قلم != عدم)
		g_signal_emit_by_name(self->فعال_قلم, "clicked");
	    أرجع TRUE;
	افتراضي:
	    اكسر;
    }
    أرجع FALSE;
}


    ساكن فراغ
vim_menu_motion_cb(
	GtkEventController  *controller استفادهنشده,
	مزدوج		    x,
	مزدوج		    y,
	VimMenu		    *self)
{
    إذا (self->prev_x == -1 || self->prev_y == -1 ||
	    (fabs(self->prev_x - x) > 0.05 && fabs(self->prev_y - y) > 0.05))
	self->ignore_hover = FALSE;
    self->prev_x = x;
    self->prev_y = y;
}

    ساكن فراغ
vim_menu_focus_cb(GtkEventController *controller استفادهنشده, VimMenu *self)
{
    gtk_popover_مجموعه_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    ساكن فراغ
vim_menu_notify_mnemonics_cb(
	VimMenu	    *self,
	GParamSpec  *pspec استفادهنشده,
	فراغ	    *udata استفادهنشده)
{
    gtk_popover_مجموعه_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    ساكن فراغ
vim_menu_init(VimMenu *self)
{
    GtkEventController	*controller;
    GtkWidget		*پشته;
    GtkWidget		*والد_box;
    GListModel		*controllers;

    gtk_popover_مجموعه_has_arrow(GTK_POPOVER(self), FALSE);
    gtk_popover_مجموعه_autohide(GTK_POPOVER(self), TRUE);

    // Do not make child popovers close parent popovers when they are closed.
    gtk_popover_مجموعه_cascade_popdown(GTK_POPOVER(self), FALSE);

    پشته = gtk_پشته_new();

    // "stack" and "parent_box" have no use other than to make the css structure
    // of the popup menu be exactly like GtkPopoverMenu. This is so that GTK
    // themes style VimMenu exactly like GtkPopoverMenu.
    والد_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_پشته_جمع_کن_فرزند(GTK_پشته(پشته), والد_box);
    gtk_پشته_مجموعه_visible_فرزند(GTK_پشته(پشته), والد_box);

    self->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_مجموعه_hexpand(self->box, TRUE);
    gtk_widget_مجموعه_vexpand(self->box, TRUE);
    gtk_box_append(GTK_BOX(والد_box), self->box);

    self->scr = gtk_scrolled_پنجره_new();
    gtk_scrolled_پنجره_مجموعه_policy(GTK_SCROLLED_پنجره(self->scr),
	    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_پنجره_مجموعه_propagate_natural_عرض(
	    GTK_SCROLLED_پنجره(self->scr), TRUE);
    gtk_scrolled_پنجره_مجموعه_propagate_natural_ارتفاع(
	    GTK_SCROLLED_پنجره(self->scr), TRUE);

    gtk_scrolled_پنجره_مجموعه_فرزند(GTK_SCROLLED_پنجره(self->scr), پشته);
    gtk_popover_مجموعه_فرزند(GTK_POPOVER(self), self->scr);

    gtk_widget_جمع_کن_css_class(GTK_WIDGET(self), "menu");

    // Add key controller for basic movement
    controller = gtk_event_controller_کلید_new();
    // Make sure we get the key presses first and handle them if possible
    gtk_event_controller_مجموعه_انتشار_phase(controller,
	    GTK_PHASE_CAPTURE);
    g_signal_connect_object(controller, "key-pressed",
	    G_CALLBACK(vim_menu_کلید_pressed_cb),
	    self, G_CONNECT_DEFAULT);
    gtk_widget_جمع_کن_controller(GTK_WIDGET(self), controller);

    // Show mnemonic underline always
    controller = gtk_event_controller_focus_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_focus_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_جمع_کن_controller(GTK_WIDGET(self), controller);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "motion",
	    G_CALLBACK(vim_menu_motion_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_جمع_کن_controller(GTK_WIDGET(self), controller);

    g_signal_connect(self, "closed", G_CALLBACK(vim_menu_closed_cb), عدم);

    // Set all shortcut controllers in the window to not require a modifier for
    // mnemonics.
    controllers = gtk_widget_observe_controllers(GTK_WIDGET(self));
    لكل (guint i = 0; i < g_list_model_get_n_اقلام(controllers); i++)
    {
	controller = g_list_model_get_قلم(controllers, i);
	إذا (GTK_IS_SHORTCUT_CONTROLLER(controller))
	    gtk_shortcut_controller_مجموعه_mnemonics_modifiers(
		    GTK_SHORTCUT_CONTROLLER(controller), 0);
    }
    g_object_unref(controllers);

    // Force enable mnemonics underline
    g_signal_connect(self, "notify::mnemonics-visible",
	    G_CALLBACK(vim_menu_notify_mnemonics_cb), عدم);

    self->prev_x = self->prev_y = -1;
}

    GtkWidget *
vim_menu_new(فراغ)
{
    أرجع g_object_new(VIM_TYPE_MENU, عدم);
}

    ساكن GtkWidget *
vim_menu_درج(VimMenu *self, GtkWidget *قلم, صحيح idx)
{
    إذا (idx > 0)
    {
	GList *prev = g_list_nth(self->اقلام, idx - 1);
	إذا (prev == عدم)
	    gtk_box_append(GTK_BOX(self->box), قلم);
	وإلا
	    gtk_box_درج_فرزند_after(GTK_BOX(self->box), قلم,
		    prev->داده);
    }
    وإلا إذا (idx == 0)
	gtk_box_prepend(GTK_BOX(self->box), قلم);
    وإلا
	gtk_box_append(GTK_BOX(self->box), قلم);

    self->اقلام = g_list_درج(self->اقلام, قلم, idx);
    أرجع قلم;
}

    ساكن فراغ
vim_menu_قلم_clicked_cb(Vim_Menu_قلم *self, VimMenu *menu)
{
    // Only close all menus if item is a regular button (no submenu). If item
    // has a submenu, then just toggle it on and off.
    إذا (self->submenu != عدم)
    {
	إذا (gtk_widget_is_visible(self->submenu))
	    gtk_popover_popdown(GTK_POPOVER(self->submenu));
	وإلا
	    gtk_popover_popup(GTK_POPOVER(self->submenu));
	أرجع;
    }

    // Since we set the "cascade-popdown" property to FALSE, we must popdown the
    // toplevel menu/popover, so that all submenus are closed.
    vim_menu_close_all(menu);
    إذا (self->func != عدم)
	self->func(self, VIM_MENU_قلم_CLICKED, self->func_udata);
}

    ساكن gboolean
vim_menu_قلم_mnemonic_activate_cb(
	Vim_Menu_قلم *self,
	gboolean    گروه استفادهنشده,
	VimMenu	    *menu)
{
    vim_menu_مجموعه_فعال_قلم(menu, self, FALSE);
    // Return FALSE so that menu item emits "clicked" signal
    أرجع FALSE;
}

    ساكن فراغ
vim_menu_قلم_enter_cb(
	GtkEventController  *controller,
	مزدوج		    x استفادهنشده,
	مزدوج		    y استفادهنشده,
	VimMenu		    *menu)
{
    Vim_Menu_قلم  *self;

    إذا (menu->ignore_hover || !gtk_event_controller_motion_شامل_است_اشاره_گر(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	أرجع;

    self = VIM_MENU_قلم(gtk_event_controller_get_widget(controller));
    vim_menu_مجموعه_فعال_قلم(menu, self, TRUE);
}

    ساكن فراغ
vim_menu_قلم_leave_cb(GtkEventController *controller, VimMenu *menu)
{
    Vim_Menu_قلم  *self;

    إذا (gtk_event_controller_motion_شامل_است_اشاره_گر(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	أرجع;

    self = VIM_MENU_قلم(gtk_event_controller_get_widget(controller));
    إذا (menu->فعال_قلم == GTK_WIDGET(self))
	vim_menu_مجموعه_فعال_قلم(menu, عدم, FALSE);
}

/*
 * Insert the menu item at the given index in the menu. If "idx" is negative,
 * then append the menu item.
 */
    فراغ
vim_menu_درج_قلم(VimMenu *self, Vim_Menu_قلم *قلم, صحيح idx)
{
    GtkEventController *controller;

    vim_menu_درج(self, GTK_WIDGET(قلم), idx);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_قلم_enter_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(controller, "leave",
	    G_CALLBACK(vim_menu_قلم_leave_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_جمع_کن_controller(GTK_WIDGET(قلم), controller);

    g_signal_connect_object(قلم, "clicked",
	    G_CALLBACK(vim_menu_قلم_clicked_cb),
	    self, G_CONNECT_DEFAULT);
    g_signal_connect_object(قلم, "mnemonic-activate",
	    G_CALLBACK(vim_menu_قلم_mnemonic_activate_cb),
	    self, G_CONNECT_DEFAULT);
}

/*
 * Insert a separator at the given position and return it.
 */
    GtkWidget *
vim_menu_درج_separator(VimMenu *self, صحيح idx)
{
    أرجع vim_menu_درج(self,
	    gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), idx);
}

/*
 * Remove the menu item or separator from the menu
 */
    فراغ
vim_menu_remove(VimMenu *self, GtkWidget *قلم)
{
    self->اقلام = g_list_remove(self->اقلام, قلم);
    gtk_box_remove(GTK_BOX(self->box), قلم);
}

/*
 * Create a deep copy of the menu
 */
    GtkWidget *
vim_menu_copy(VimMenu *self)
{
    GtkWidget	*copy = vim_menu_new();
    صحيح		i = 0;

    لكل (GList *l = self->اقلام; l != عدم; l = l->بعدی, i++)
    {
	Vim_Menu_قلم *قلم;
	GtkWidget   *قلم_copy;

	إذا (!VIM_IS_MENU_قلم(l->داده))
	{
	    أكد(GTK_IS_SEPARATOR(l->داده));
	    vim_menu_درج_separator(VIM_MENU(copy), i);
	    استمر;
	}

	قلم = l->داده;
	قلم_copy = vim_menu_قلم_copy(قلم);

	vim_menu_درج_قلم(VIM_MENU(copy), VIM_MENU_قلم(قلم_copy), i);
    }
    أرجع copy;
}

#پایان_اگر // FEAT_MENU
