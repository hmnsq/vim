/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

#جيڪڏهن_وصف_ٿيل FEAT_MENU

#ضمن <gtk/gtk.h>
#ضمن "gui_gtk4_menu.h"

// Note that this may return NULL for popup menus
#وصف_ڪريو GET_MENU_BAR(m) VIM_MENU_BAR(gtk_widget_get_ancestor( \
	    GTK_WIDGET(m), VIM_TYPE_MENU_BAR))

/*
 * Similar as GtkButton but set CSS name to "item" to emulate GtkPopoverMenuBar
 * styling. Always has a submenu.
 */
بنية Vim_Menu_Bar_شي
{
    GtkButton والد;

    GtkWidget *menu;
};

G_DEFINE_TYPE(Vim_Menu_Bar_شي_2, vim_menu_bar_شي, GTK_TYPE_BUTTON)

    ساكن فراغ
vim_menu_bar_شي_dispose(GObject *object)
{
    Vim_Menu_Bar_شي_2 *self = VIM_MENU_BAR_شي(object);

    g_clear_اشارو((GtkWidget **)&self->menu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_شي_والد_class)->dispose(object);
}

    ساكن فراغ
vim_menu_bar_شي_class_init(Vim_Menu_Bar_شي_Class *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_شي_dispose;

    gtk_widget_class_سيٽ_css_name(widget_class, "item");
}

    ساكن فراغ
vim_menu_bar_شي_init(Vim_Menu_Bar_شي_2 *self)
{
    // Enable mnemonics
    gtk_button_سيٽ_use_underline(GTK_BUTTON(self), TRUE);
}

    GtkWidget *
vim_menu_bar_شي_new(ثابت حرف *متن, VimMenu *menu)
{
    Vim_Menu_Bar_شي_2 *شي = g_object_new(VIM_TYPE_MENU_BAR_شي, عدم);

    gtk_button_سيٽ_label(GTK_BUTTON(شي), متن);

    شي->menu = GTK_WIDGET(menu);
    gtk_popover_سيٽ_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    // Make popover start at top left corner
    gtk_widget_سيٽ_halign(GTK_WIDGET(menu), GTK_ALIGN_شروعات);
    gtk_widget_سيٽ_والد(GTK_WIDGET(menu), GTK_WIDGET(شي));

    أرجع GTK_WIDGET(شي);
}

    فراغ
vim_menu_bar_شي_سيٽ_متن(Vim_Menu_Bar_شي_2 *self, ثابت حرف *متن)
{
    gtk_button_سيٽ_label(GTK_BUTTON(self), متن);
}

/*
 * Similar to GtkPopoverMenuBar
 */
بنية _VimMenuBar
{
    GtkWidget والد;

    GList *شيون;

    // Currently visible item that has submenu popped up, else NULL
    GtkWidget *سرگرم_شي;
};

G_DEFINE_TYPE(VimMenuBar, vim_menu_bar, GTK_TYPE_WIDGET)

    ساكن فراغ
vim_menu_bar_dispose(GObject *object)
{
    VimMenuBar *self = VIM_MENU_BAR(object);

    g_clear_list(&self->شيون, (GDestroyNotify)gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_والد_class)->dispose(object);
}

    ساكن فراغ
vim_menu_bar_class_init(VimMenuBarClass *class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_dispose;

    gtk_widget_class_سيٽ_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_سيٽ_css_name(widget_class, "menubar");
}

    ساكن فراغ
vim_menu_bar_init(VimMenuBar *self)
{
    GtkLayoutManager *lm = gtk_widget_get_layout_manager(GTK_WIDGET(self));

    gtk_orientable_سيٽ_orientation(GTK_ORIENTABLE(lm),
	    GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_سيٽ_spacing(GTK_BOX_LAYOUT(lm), 0);
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
vim_menu_bar_منزل_menu(VimMenuBar *self)
{
    GtkWidget	*menu = vim_menu_new();
    صحيح		i = 0;

    لكل (GList *l = self->شيون; l != عدم; l = l->ايندڙ, i++)
    {
	Vim_Menu_Bar_شي_2	*baritem = l->معلومات;
	GtkWidget	*شي;

	شي = vim_menu_شي_new(
		gtk_button_get_label(GTK_BUTTON(baritem)), عدم, عدم);

	vim_menu_شي_سيٽ_submenu(VIM_MENU_شي(شي),
		VIM_MENU(vim_menu_copy(VIM_MENU(baritem->menu))));

	vim_menu_وجهو_شي(VIM_MENU(menu), VIM_MENU_شي(شي), i);
    }
    أرجع menu;
}

/*
 * Set the currently active menu of the menubar to "item". If NULL, then close
 * any submenus.
 */
    ساكن فراغ
vim_menu_bar_سيٽ_سرگرم_شي(
	VimMenuBar	*self,
	Vim_Menu_Bar_شي_2	*شي,
	gboolean	force)
{
    // Do nothing if currently active item is "item", or if there is not
    // currently active item. User must click a menu item first for menus to
    // automatically appear on hover. This is unless "force" is TRUE.
    //
    // Only make item selected if there is no active item (no submenu open), or
    // if the item was set as the active item..
    إذا ((!force && self->سرگرم_شي == عدم)
	    || self->سرگرم_شي == GTK_WIDGET(شي))
    {
	إذا (self->سرگرم_شي == عدم)
	    gtk_widget_سيٽ_state_flags(GTK_WIDGET(شي),
		    GTK_STATE_FLAG_SELECTED, FALSE);
	أرجع;
    }

    إذا (self->سرگرم_شي != عدم)
    {
	// Call this before popdown, since "closed" signal may be emitted
	// immediately.
	gtk_widget_unset_state_flags(self->سرگرم_شي,
		GTK_STATE_FLAG_SELECTED);
	gtk_popover_popdown(GTK_POPOVER(
		    VIM_MENU_BAR_شي(self->سرگرم_شي)->menu)
		);
    }

    self->سرگرم_شي = GTK_WIDGET(شي);
    إذا (شي != عدم)
    {
	gtk_popover_popup(GTK_POPOVER(شي->menu));
	gtk_widget_سيٽ_state_flags(GTK_WIDGET(شي),
		GTK_STATE_FLAG_SELECTED, FALSE);
    }
}

    ساكن فراغ
vim_menu_bar_شي_enter_cb(
	GtkEventController  *controller,
	مزدوج		    x اڻ_استعمال_ٿيل,
	مزدوج		    y اڻ_استعمال_ٿيل,
	VimMenuBar	    *menubar)
{
    Vim_Menu_Bar_شي_2  *self;

    self = VIM_MENU_BAR_شي(gtk_event_controller_get_widget(controller));
    vim_menu_bar_سيٽ_سرگرم_شي(menubar, self, FALSE);
}

    ساكن فراغ
vim_menu_bar_شي_leave_cb(
	GtkEventController  *controller,
	VimMenuBar	    *menubar)
{
    Vim_Menu_Bar_شي_2 *self;

    self = VIM_MENU_BAR_شي(gtk_event_controller_get_widget(controller));

    // If the item is the currently active item, then don't deselect it.
    إذا (menubar->سرگرم_شي != GTK_WIDGET(self))
	gtk_widget_unset_state_flags(GTK_WIDGET(self),
		GTK_STATE_FLAG_SELECTED);
}

    ساكن فراغ
vim_menu_bar_شي_clicked_cb(Vim_Menu_Bar_شي_2 *self, VimMenuBar *menubar)
{
    vim_menu_bar_سيٽ_سرگرم_شي(menubar, self, TRUE);
}

    ساكن فراغ
vim_menu_bar_شي_menu_closed_cb(VimMenu *menu اڻ_استعمال_ٿيل, VimMenuBar *menubar)
{
    إذا (menubar->سرگرم_شي != عدم)
	gtk_widget_unset_state_flags(GTK_WIDGET(menubar->سرگرم_شي),
		GTK_STATE_FLAG_SELECTED);
    vim_menu_bar_سيٽ_سرگرم_شي(menubar, عدم, TRUE);
    // Make sure to focus drawarea
    gtk_widget_grab_focus(gui.drawarea);
}

/*
 * Insert the menu item at the given index in the menu bar.
 */
    فراغ
vim_menu_bar_وجهو_شي(VimMenuBar *self, Vim_Menu_Bar_شي_2 *شي, صحيح idx)
{
    GtkEventController	*controller;
    GList		*ايندڙ_sibling;

    ايندڙ_sibling = g_list_nth(self->شيون, idx);
    gtk_widget_وجهو_before(GTK_WIDGET(شي), GTK_WIDGET(self),
	    ايندڙ_sibling == عدم ? عدم : ايندڙ_sibling->معلومات);

    self->شيون = g_list_وجهو(self->شيون, شي, idx);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_bar_شي_enter_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(controller, "leave",
	    G_CALLBACK(vim_menu_bar_شي_leave_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_جوڙيو_controller(GTK_WIDGET(شي), controller);

    g_signal_connect_object(شي, "clicked",
	    G_CALLBACK(vim_menu_bar_شي_clicked_cb), self, G_CONNECT_DEFAULT);

    g_signal_connect_object(شي->menu, "closed",
	    G_CALLBACK(vim_menu_bar_شي_menu_closed_cb),
	    self, G_CONNECT_DEFAULT);
}

/*
 * Remove the menu item or separator from the menu bar
 */
    فراغ
vim_menu_bar_remove(VimMenuBar *self, GtkWidget *شي)
{
    self->شيون = g_list_remove(self->شيون, شي);
    gtk_widget_unparent(شي);
}

/*
 * Show the given menu in the menubar. If "item" is NULL, then show first menu.
 */
    فراغ
vim_menu_bar_show(VimMenuBar *self, Vim_Menu_Bar_شي_2 *شي)
{
    إذا (شي == عدم)
	شي = g_list_nth_معلومات(self->شيون, 0);

    vim_menu_bar_سيٽ_سرگرم_شي(self, شي, TRUE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    ساكن GtkWidget *
vim_menu_bar_move_سرگرم_شي(VimMenuBar *self, صحيح dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->سرگرم_شي;

    إذا (widget == عدم)
	أرجع gtk_widget_get_پهريون_ٻار(GTK_WIDGET(self));

    إذا (dir > 0)
    {
	func = gtk_widget_get_ايندڙ_sibling;
	null_func = gtk_widget_get_پهريون_ٻار;
    }
    وإلا
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_آخري_ٻار;
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
بنية Vim_Menu_شي_2
{
    GtkButton والد;

    GtkWidget *label;	    // Displays text for button.
    GtkWidget *aux_widget;  // Either an icon or a label showing the accelerator
			    // text.

    GtkWidget *submenu;	    // Submenu popover if any (VimMenu)

    // Callback called when clicked or selected, we store this so that copying a
    // menu item works properly.
    Vim_Menu_شي_Func	func;
    فراغ		*func_udata;
};

G_DEFINE_TYPE(Vim_Menu_شي, vim_menu_شي, GTK_TYPE_BUTTON)

    ساكن فراغ
vim_menu_شي_dispose(GObject *object)
{
    Vim_Menu_شي *self = VIM_MENU_شي(object);

    g_clear_اشارو(&self->label, gtk_widget_unparent);
    g_clear_اشارو(&self->aux_widget, gtk_widget_unparent);
    g_clear_اشارو((GtkWidget **)&self->submenu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_شي_والد_class)->dispose(object);
}

    ساكن فراغ
vim_menu_شي_class_init(Vim_Menu_شي_Class *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_شي_dispose;

    gtk_widget_class_سيٽ_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_سيٽ_css_name(widget_class, "modelbutton");
}

    ساكن فراغ
vim_menu_شي_init(Vim_Menu_شي *self)
{
    GtkLayoutManager *lm = gtk_widget_get_layout_manager(GTK_WIDGET(self));

    gtk_orientable_سيٽ_orientation(GTK_ORIENTABLE(lm),
	    GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_سيٽ_spacing(GTK_BOX_LAYOUT(lm), 0);
}

/*
 * Create a new menu item with the given text to display. "func" may be NULL if
 * not needed.
 */
    GtkWidget *
vim_menu_شي_new(ثابت حرف *متن, Vim_Menu_شي_Func func, فراغ *udata)
{
    Vim_Menu_شي *شي = g_object_new(VIM_TYPE_MENU_شي, عدم);

    شي->func = func;
    شي->func_udata = udata;
    شي->label = gtk_label_new_with_mnemonic(متن);

    // Make sure label is on the right and pushes everything to the left
    gtk_widget_سيٽ_halign(شي->label, GTK_ALIGN_شروعات);
    gtk_widget_سيٽ_hexpand(شي->label, TRUE);
    gtk_widget_سيٽ_والد(شي->label, GTK_WIDGET(شي));

    أرجع GTK_WIDGET(شي);
}

/*
 * Update displayed text for menu item
 */
    فراغ
vim_menu_شي_سيٽ_متن(Vim_Menu_شي *self, ثابت حرف *متن)
{
    gtk_label_سيٽ_متن_with_mnemonic(GTK_LABEL(self->label), متن);
}

    ساكن فراغ
vim_menu_شي_سيٽ_aux_widget(Vim_Menu_شي *self, GtkWidget *aux)
{
    self->aux_widget = aux;
    gtk_widget_سيٽ_halign(self->aux_widget, GTK_ALIGN_پڇاڙي);
    gtk_widget_سيٽ_hexpand(self->aux_widget, FALSE);
    gtk_widget_سيٽ_margin_شروعات(self->aux_widget, 50);
}

/*
 * Set the accelerator text for the menu item.
 */
    فراغ
vim_menu_شي_سيٽ_accel(Vim_Menu_شي *self, ثابت حرف *accel_متن)
{
    أكد(self->aux_widget == عدم);

    vim_menu_شي_سيٽ_aux_widget(self, gtk_label_new(accel_متن));
    gtk_widget_وجهو_after(self->aux_widget, GTK_WIDGET(self), self->label);
}

/*
 * Set the submenu popover for the menu item
 */
    فراغ
vim_menu_شي_سيٽ_submenu(Vim_Menu_شي *self, VimMenu *submenu)
{
    GtkWidget *icon;

    أكد(self->submenu == عدم);
    أكد(self->aux_widget == عدم);

    // Add arrow icon pointing to right
    icon = gtk_image_new_from_icon_name("pan-end-symbolic");
    vim_menu_شي_سيٽ_aux_widget(self, icon);
    gtk_widget_وجهو_after(self->aux_widget, GTK_WIDGET(self), self->label);

    gtk_popover_سيٽ_position(GTK_POPOVER(submenu), GTK_POS_ساڄو);
    // Make top of popover be aligned with button.
    gtk_widget_سيٽ_valign(GTK_WIDGET(submenu), GTK_ALIGN_شروعات);

    self->submenu = GTK_WIDGET(submenu);
    gtk_widget_سيٽ_والد(GTK_WIDGET(submenu), GTK_WIDGET(self));
}

/*
 * Create a deep copy of the menu item
 */
    ساكن GtkWidget *
vim_menu_شي_copy(Vim_Menu_شي *self)
{
    GtkWidget *copy;

    copy = vim_menu_شي_new(
	    gtk_label_get_متن(GTK_LABEL(self->label)),
	    self->func, self->func_udata);

    إذا (self->submenu != عدم)
	vim_menu_شي_سيٽ_submenu(VIM_MENU_شي(copy),
		VIM_MENU(vim_menu_copy(VIM_MENU(self->submenu))));
    وإلا إذا (self->aux_widget != عدم)
	vim_menu_شي_سيٽ_accel(VIM_MENU_شي(copy),
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

    GList *شيون;

    // Currently active item showing submenu popover, or being hovered on, or
    // NULL. Note that item may have submenu but not have it open, when
    // navigating via keyboard.
    GtkWidget *سرگرم_شي;

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

    g_clear_list(&self->شيون, (GDestroyNotify)gtk_widget_unparent);
    g_clear_اشارو(&self->scr, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_والد_class)->dispose(object);
}

    ساكن فراغ
vim_menu_class_init(VimMenuClass *class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_dispose;
}

    ساكن gboolean
vim_menu_select_سرگرم_شي(VimMenu *self, gboolean open)
{
    Vim_Menu_شي *شي;

    gtk_widget_سيٽ_state_flags(self->سرگرم_شي,
	    GTK_STATE_FLAG_SELECTED, FALSE);

    // Make sure to focus item, so that scrolled window knows what to do.
    gtk_widget_grab_focus(GTK_WIDGET(self->سرگرم_شي));

    شي = VIM_MENU_شي(self->سرگرم_شي);
    إذا (شي->func != عدم)
	شي->func(شي, VIM_MENU_شي_SELECTED, شي->func_udata);

    إذا (open && VIM_MENU_شي(self->سرگرم_شي)->submenu != عدم)
    {
	GtkWidget *submenu = VIM_MENU_شي(self->سرگرم_شي)->submenu;
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
vim_menu_سيٽ_سرگرم_شي(VimMenu *self, Vim_Menu_شي *شي, gboolean open)
{
    إذا (self->سرگرم_شي == GTK_WIDGET(شي))
	أرجع;

    إذا (self->سرگرم_شي != عدم)
    {
	إذا (VIM_MENU_شي(self->سرگرم_شي)->submenu != عدم)
	    gtk_popover_popdown(GTK_POPOVER(
			VIM_MENU_شي(self->سرگرم_شي)->submenu
			));
	gtk_widget_unset_state_flags(GTK_WIDGET(self->سرگرم_شي),
		GTK_STATE_FLAG_SELECTED);
    }

    self->سرگرم_شي = GTK_WIDGET(شي);
    إذا (شي != عدم)
	(فراغ)vim_menu_select_سرگرم_شي(self, open);
}

    ساكن فراغ
vim_menu_closed_cb(VimMenu *self, فراغ *udata اڻ_استعمال_ٿيل)
{
    vim_menu_سيٽ_سرگرم_شي(self, عدم, FALSE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    ساكن GtkWidget *
vim_menu_move_سرگرم_شي(VimMenu *self, صحيح dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->سرگرم_شي;

    // If there is no currently active item, then just use the first one
    إذا (widget == عدم)
    {
	widget = gtk_widget_get_پهريون_ٻار(self->box);
	طالما (widget != عدم && !VIM_IS_MENU_شي(widget))
	    widget = gtk_widget_get_ايندڙ_sibling(widget);
	أرجع widget;
    }

    // Could also just use GList functions, but this seems simpler (no
    // difference anyways).
    إذا (dir > 0)
    {
	func = gtk_widget_get_ايندڙ_sibling;
	null_func = gtk_widget_get_پهريون_ٻار;
    }
    وإلا
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_آخري_ٻار;
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
	إذا (VIM_IS_MENU_شي(widget))
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

    إذا (والد_menu->سرگرم_شي != عدم)
	gtk_widget_unset_state_flags(GTK_WIDGET(والد_menu->سرگرم_شي),
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
	vim_menu_bar_سيٽ_سرگرم_شي(menubar, عدم, TRUE);
    وإلا
	gtk_popover_popdown(GTK_POPOVER(self));

    // Grab focus after popup menus without a menubar are closed
    gtk_widget_grab_focus(gui.drawarea);
}

    ساكن gboolean
vim_menu_ڪنجي_pressed_cb(
	GtkEventController  *controller اڻ_استعمال_ٿيل,
	guint		    keyval,
	guint		    keycode اڻ_استعمال_ٿيل,
	GdkModifierType	    state,
	VimMenu		    *self)
{
    GtkWidget	*widget;

    بدل (keyval)
    {
	حالة GDK_ڪنجي_هيٺ_ڏانهن:
	حالة GDK_ڪنجي_KP_هيٺ_ڏانهن:
	حالة GDK_ڪنجي_مٿي:
	حالة GDK_ڪنجي_KP_مٿي:
	حالة GDK_ڪنجي_Tab:
	حالة GDK_ڪنجي_KP_Tab:
	حالة GDK_ڪنجي_ISO_کاٻو_Tab:
	    // Go to the previous or next item if any
	    widget = vim_menu_move_سرگرم_شي(self,
		    (state & GDK_سرڪايو_نقاب)
		    || keyval == GDK_ڪنجي_مٿي ? -1 : 1);
	    vim_menu_سيٽ_سرگرم_شي(self, VIM_MENU_شي(widget), FALSE);
	    self->ignore_hover = TRUE;
	    أرجع TRUE;
	حالة GDK_ڪنجي_کاٻو:
	    // Pressing control switches menu bar item.
	    إذا (state & GDK_CONTROL_نقاب)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		إذا (menubar != عدم)
		{
		    widget = vim_menu_bar_move_سرگرم_شي(menubar, -1);
		    vim_menu_bar_سيٽ_سرگرم_شي(menubar,
			    VIM_MENU_BAR_شي(widget), TRUE);
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
	حالة GDK_ڪنجي_ساڄو:
	    إذا (state & GDK_CONTROL_نقاب)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		إذا (menubar != عدم)
		{
		    widget = vim_menu_bar_move_سرگرم_شي(menubar, 1);
		    vim_menu_bar_سيٽ_سرگرم_شي(menubar,
			    VIM_MENU_BAR_شي(widget), TRUE);
		}
		أرجع TRUE;
	    }
	    // Open submenu if active item has one
	    إذا (self->سرگرم_شي != عدم
		    && vim_menu_select_سرگرم_شي(self, TRUE))
	    {
		// Select first item in opened submenu
		VimMenu *submenu = VIM_MENU(
			VIM_MENU_شي(self->سرگرم_شي)->submenu
			);

		vim_menu_سيٽ_سرگرم_شي(submenu,
			VIM_MENU_شي(
			    gtk_widget_get_پهريون_ٻار(submenu->box)),
			FALSE);
	    }
	    self->ignore_hover = TRUE;
	    أرجع TRUE;
	حالة GDK_ڪنجي_Escape:
	    // Close all popover menus
	    vim_menu_close_all(self);
	    أرجع TRUE;
	حالة GDK_ڪنجي_ISO_Enter:
	حالة GDK_ڪنجي_3270_Enter:
	حالة GDK_ڪنجي_KP_Enter:
	حالة GDK_ڪنجي_Return:
	    إذا (self->سرگرم_شي != عدم)
		g_signal_emit_by_name(self->سرگرم_شي, "clicked");
	    أرجع TRUE;
	افتراضي:
	    اكسر;
    }
    أرجع FALSE;
}


    ساكن فراغ
vim_menu_motion_cb(
	GtkEventController  *controller اڻ_استعمال_ٿيل,
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
vim_menu_focus_cb(GtkEventController *controller اڻ_استعمال_ٿيل, VimMenu *self)
{
    gtk_popover_سيٽ_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    ساكن فراغ
vim_menu_notify_mnemonics_cb(
	VimMenu	    *self,
	GParamSpec  *pspec اڻ_استعمال_ٿيل,
	فراغ	    *udata اڻ_استعمال_ٿيل)
{
    gtk_popover_سيٽ_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    ساكن فراغ
vim_menu_init(VimMenu *self)
{
    GtkEventController	*controller;
    GtkWidget		*ڍير;
    GtkWidget		*والد_box;
    GListModel		*controllers;

    gtk_popover_سيٽ_has_arrow(GTK_POPOVER(self), FALSE);
    gtk_popover_سيٽ_autohide(GTK_POPOVER(self), TRUE);

    // Do not make child popovers close parent popovers when they are closed.
    gtk_popover_سيٽ_cascade_popdown(GTK_POPOVER(self), FALSE);

    ڍير = gtk_ڍير_new();

    // "stack" and "parent_box" have no use other than to make the css structure
    // of the popup menu be exactly like GtkPopoverMenu. This is so that GTK
    // themes style VimMenu exactly like GtkPopoverMenu.
    والد_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_ڍير_جوڙيو_ٻار(GTK_ڍير(ڍير), والد_box);
    gtk_ڍير_سيٽ_visible_ٻار(GTK_ڍير(ڍير), والد_box);

    self->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_سيٽ_hexpand(self->box, TRUE);
    gtk_widget_سيٽ_vexpand(self->box, TRUE);
    gtk_box_append(GTK_BOX(والد_box), self->box);

    self->scr = gtk_scrolled_دري_new();
    gtk_scrolled_دري_سيٽ_policy(GTK_SCROLLED_دري(self->scr),
	    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_دري_سيٽ_propagate_natural_ويڪر(
	    GTK_SCROLLED_دري(self->scr), TRUE);
    gtk_scrolled_دري_سيٽ_propagate_natural_اوچائي(
	    GTK_SCROLLED_دري(self->scr), TRUE);

    gtk_scrolled_دري_سيٽ_ٻار(GTK_SCROLLED_دري(self->scr), ڍير);
    gtk_popover_سيٽ_ٻار(GTK_POPOVER(self), self->scr);

    gtk_widget_جوڙيو_css_class(GTK_WIDGET(self), "menu");

    // Add key controller for basic movement
    controller = gtk_event_controller_ڪنجي_new();
    // Make sure we get the key presses first and handle them if possible
    gtk_event_controller_سيٽ_ڦهلاء_phase(controller,
	    GTK_PHASE_CAPTURE);
    g_signal_connect_object(controller, "key-pressed",
	    G_CALLBACK(vim_menu_ڪنجي_pressed_cb),
	    self, G_CONNECT_DEFAULT);
    gtk_widget_جوڙيو_controller(GTK_WIDGET(self), controller);

    // Show mnemonic underline always
    controller = gtk_event_controller_focus_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_focus_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_جوڙيو_controller(GTK_WIDGET(self), controller);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "motion",
	    G_CALLBACK(vim_menu_motion_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_جوڙيو_controller(GTK_WIDGET(self), controller);

    g_signal_connect(self, "closed", G_CALLBACK(vim_menu_closed_cb), عدم);

    // Set all shortcut controllers in the window to not require a modifier for
    // mnemonics.
    controllers = gtk_widget_observe_controllers(GTK_WIDGET(self));
    لكل (guint i = 0; i < g_list_model_get_n_شيون(controllers); i++)
    {
	controller = g_list_model_get_شي(controllers, i);
	إذا (GTK_IS_SHORTCUT_CONTROLLER(controller))
	    gtk_shortcut_controller_سيٽ_mnemonics_modifiers(
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
vim_menu_وجهو(VimMenu *self, GtkWidget *شي, صحيح idx)
{
    إذا (idx > 0)
    {
	GList *prev = g_list_nth(self->شيون, idx - 1);
	إذا (prev == عدم)
	    gtk_box_append(GTK_BOX(self->box), شي);
	وإلا
	    gtk_box_وجهو_ٻار_after(GTK_BOX(self->box), شي,
		    prev->معلومات);
    }
    وإلا إذا (idx == 0)
	gtk_box_prepend(GTK_BOX(self->box), شي);
    وإلا
	gtk_box_append(GTK_BOX(self->box), شي);

    self->شيون = g_list_وجهو(self->شيون, شي, idx);
    أرجع شي;
}

    ساكن فراغ
vim_menu_شي_clicked_cb(Vim_Menu_شي *self, VimMenu *menu)
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
	self->func(self, VIM_MENU_شي_CLICKED, self->func_udata);
}

    ساكن gboolean
vim_menu_شي_mnemonic_activate_cb(
	Vim_Menu_شي *self,
	gboolean    گروهه اڻ_استعمال_ٿيل,
	VimMenu	    *menu)
{
    vim_menu_سيٽ_سرگرم_شي(menu, self, FALSE);
    // Return FALSE so that menu item emits "clicked" signal
    أرجع FALSE;
}

    ساكن فراغ
vim_menu_شي_enter_cb(
	GtkEventController  *controller,
	مزدوج		    x اڻ_استعمال_ٿيل,
	مزدوج		    y اڻ_استعمال_ٿيل,
	VimMenu		    *menu)
{
    Vim_Menu_شي  *self;

    إذا (menu->ignore_hover || !gtk_event_controller_motion_شامل_آهي_اشارو(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	أرجع;

    self = VIM_MENU_شي(gtk_event_controller_get_widget(controller));
    vim_menu_سيٽ_سرگرم_شي(menu, self, TRUE);
}

    ساكن فراغ
vim_menu_شي_leave_cb(GtkEventController *controller, VimMenu *menu)
{
    Vim_Menu_شي  *self;

    إذا (gtk_event_controller_motion_شامل_آهي_اشارو(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	أرجع;

    self = VIM_MENU_شي(gtk_event_controller_get_widget(controller));
    إذا (menu->سرگرم_شي == GTK_WIDGET(self))
	vim_menu_سيٽ_سرگرم_شي(menu, عدم, FALSE);
}

/*
 * Insert the menu item at the given index in the menu. If "idx" is negative,
 * then append the menu item.
 */
    فراغ
vim_menu_وجهو_شي(VimMenu *self, Vim_Menu_شي *شي, صحيح idx)
{
    GtkEventController *controller;

    vim_menu_وجهو(self, GTK_WIDGET(شي), idx);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_شي_enter_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(controller, "leave",
	    G_CALLBACK(vim_menu_شي_leave_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_جوڙيو_controller(GTK_WIDGET(شي), controller);

    g_signal_connect_object(شي, "clicked",
	    G_CALLBACK(vim_menu_شي_clicked_cb),
	    self, G_CONNECT_DEFAULT);
    g_signal_connect_object(شي, "mnemonic-activate",
	    G_CALLBACK(vim_menu_شي_mnemonic_activate_cb),
	    self, G_CONNECT_DEFAULT);
}

/*
 * Insert a separator at the given position and return it.
 */
    GtkWidget *
vim_menu_وجهو_separator(VimMenu *self, صحيح idx)
{
    أرجع vim_menu_وجهو(self,
	    gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), idx);
}

/*
 * Remove the menu item or separator from the menu
 */
    فراغ
vim_menu_remove(VimMenu *self, GtkWidget *شي)
{
    self->شيون = g_list_remove(self->شيون, شي);
    gtk_box_remove(GTK_BOX(self->box), شي);
}

/*
 * Create a deep copy of the menu
 */
    GtkWidget *
vim_menu_copy(VimMenu *self)
{
    GtkWidget	*copy = vim_menu_new();
    صحيح		i = 0;

    لكل (GList *l = self->شيون; l != عدم; l = l->ايندڙ, i++)
    {
	Vim_Menu_شي *شي;
	GtkWidget   *شي_copy;

	إذا (!VIM_IS_MENU_شي(l->معلومات))
	{
	    أكد(GTK_IS_SEPARATOR(l->معلومات));
	    vim_menu_وجهو_separator(VIM_MENU(copy), i);
	    استمر;
	}

	شي = l->معلومات;
	شي_copy = vim_menu_شي_copy(شي);

	vim_menu_وجهو_شي(VIM_MENU(copy), VIM_MENU_شي(شي_copy), i);
    }
    أرجع copy;
}

#جيڪڏهن_پورو // FEAT_MENU
