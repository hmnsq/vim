/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#अन्तर्भाव "vim.h"

#ifdef FEAT_MENU

#अन्तर्भाव <gtk/gtk.h>
#अन्तर्भाव "gui_gtk4_menu.h"

// Note that this may return NULL for popup menus
#define GET_MENU_BAR(m) VIM_MENU_BAR(gtk_widget_get_ancestor( \
	    GTK_WIDGET(m), VIM_TYPE_MENU_BAR))

/*
 * Similar as GtkButton but set CSS name to "item" to emulate GtkPopoverMenuBar
 * styling. Always has a submenu.
 */
संरचना _VimMenuBarItem
{
    GtkButton parent;

    GtkWidget *menu;
};

G_DEFINE_TYPE(VimMenuBarItem, vim_menu_bar_item, GTK_TYPE_BUTTON)

    स्थिर शून्य
vim_menu_bar_item_dispose(GObject *object)
{
    VimMenuBarItem *self = VIM_MENU_BAR_ITEM(object);

    g_clear_pointer((GtkWidget **)&self->menu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_item_parent_class)->dispose(object);
}

    स्थिर शून्य
vim_menu_bar_item_class_init(VimMenuBarItemClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_item_dispose;

    gtk_widget_class_set_css_name(widget_class, "item");
}

    स्थिर शून्य
vim_menu_bar_item_init(VimMenuBarItem *self)
{
    // Enable mnemonics
    gtk_button_set_use_underline(GTK_BUTTON(self), TRUE);
}

    GtkWidget *
vim_menu_bar_item_new(अचर वर्ण *text, VimMenu *menu)
{
    VimMenuBarItem *item = g_object_new(VIM_TYPE_MENU_BAR_ITEM, रिक्त);

    gtk_button_set_label(GTK_BUTTON(item), text);

    item->menu = GTK_WIDGET(menu);
    gtk_popover_set_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    // Make popover start at top left corner
    gtk_widget_set_halign(GTK_WIDGET(menu), GTK_ALIGN_START);
    gtk_widget_set_parent(GTK_WIDGET(menu), GTK_WIDGET(item));

    फर्काऊ GTK_WIDGET(item);
}

    शून्य
vim_menu_bar_item_set_text(VimMenuBarItem *self, अचर वर्ण *text)
{
    gtk_button_set_label(GTK_BUTTON(self), text);
}

/*
 * Similar to GtkPopoverMenuBar
 */
संरचना _VimMenuBar
{
    GtkWidget parent;

    GList *वस्तुहरू;

    // Currently visible item that has submenu popped up, else NULL
    GtkWidget *active_item;
};

G_DEFINE_TYPE(VimMenuBar, vim_menu_bar, GTK_TYPE_WIDGET)

    स्थिर शून्य
vim_menu_bar_dispose(GObject *object)
{
    VimMenuBar *self = VIM_MENU_BAR(object);

    g_clear_list(&self->वस्तुहरू, (GDestroyNotify)gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_parent_class)->dispose(object);
}

    स्थिर शून्य
vim_menu_bar_class_init(VimMenuBarClass *class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_dispose;

    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_set_css_name(widget_class, "menubar");
}

    स्थिर शून्य
vim_menu_bar_init(VimMenuBar *self)
{
    GtkLayoutManager *lm = gtk_widget_get_layout_manager(GTK_WIDGET(self));

    gtk_orientable_set_orientation(GTK_ORIENTABLE(lm),
	    GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_set_spacing(GTK_BOX_LAYOUT(lm), 0);
}

    GtkWidget *
vim_menu_bar_new(शून्य)
{
    फर्काऊ g_object_new(VIM_TYPE_MENU_BAR, रिक्त);
}

/*
 * Create a VimMenu widget with the menus of the menu bar as its submenus. Note
 * that it is a deep copy.
 */
    GtkWidget *
vim_menu_bar_to_menu(VimMenuBar *self)
{
    GtkWidget	*menu = vim_menu_new();
    पूर्णाङ्क		i = 0;

    लागि (GList *l = self->वस्तुहरू; l != रिक्त; l = l->next, i++)
    {
	VimMenuBarItem	*baritem = l->data;
	GtkWidget	*item;

	item = vim_menu_item_new(
		gtk_button_get_label(GTK_BUTTON(baritem)), रिक्त, रिक्त);

	vim_menu_item_set_submenu(VIM_MENU_ITEM(item),
		VIM_MENU(vim_menu_copy(VIM_MENU(baritem->menu))));

	vim_menu_घुसाउ_item(VIM_MENU(menu), VIM_MENU_ITEM(item), i);
    }
    फर्काऊ menu;
}

/*
 * Set the currently active menu of the menubar to "item". If NULL, then close
 * any submenus.
 */
    स्थिर शून्य
vim_menu_bar_set_active_item(
	VimMenuBar	*self,
	VimMenuBarItem	*item,
	gboolean	force)
{
    // Do nothing if currently active item is "item", or if there is not
    // currently active item. User must click a menu item first for menus to
    // automatically appear on hover. This is unless "force" is TRUE.
    //
    // Only make item selected if there is no active item (no submenu open), or
    // if the item was set as the active item..
    यदि ((!force && self->active_item == रिक्त)
	    || self->active_item == GTK_WIDGET(item))
    {
	यदि (self->active_item == रिक्त)
	    gtk_widget_set_state_flags(GTK_WIDGET(item),
		    GTK_STATE_FLAG_SELECTED, FALSE);
	फर्काऊ;
    }

    यदि (self->active_item != रिक्त)
    {
	// Call this before popdown, since "closed" signal may be emitted
	// immediately.
	gtk_widget_unset_state_flags(self->active_item,
		GTK_STATE_FLAG_SELECTED);
	gtk_popover_popdown(GTK_POPOVER(
		    VIM_MENU_BAR_ITEM(self->active_item)->menu)
		);
    }

    self->active_item = GTK_WIDGET(item);
    यदि (item != रिक्त)
    {
	gtk_popover_popup(GTK_POPOVER(item->menu));
	gtk_widget_set_state_flags(GTK_WIDGET(item),
		GTK_STATE_FLAG_SELECTED, FALSE);
    }
}

    स्थिर शून्य
vim_menu_bar_item_enter_cb(
	GtkEventController  *controller,
	द्विगुण		    x UNUSED,
	द्विगुण		    y UNUSED,
	VimMenuBar	    *menubar)
{
    VimMenuBarItem  *self;

    self = VIM_MENU_BAR_ITEM(gtk_event_controller_get_widget(controller));
    vim_menu_bar_set_active_item(menubar, self, FALSE);
}

    स्थिर शून्य
vim_menu_bar_item_leave_cb(
	GtkEventController  *controller,
	VimMenuBar	    *menubar)
{
    VimMenuBarItem *self;

    self = VIM_MENU_BAR_ITEM(gtk_event_controller_get_widget(controller));

    // If the item is the currently active item, then don't deselect it.
    यदि (menubar->active_item != GTK_WIDGET(self))
	gtk_widget_unset_state_flags(GTK_WIDGET(self),
		GTK_STATE_FLAG_SELECTED);
}

    स्थिर शून्य
vim_menu_bar_item_clicked_cb(VimMenuBarItem *self, VimMenuBar *menubar)
{
    vim_menu_bar_set_active_item(menubar, self, TRUE);
}

    स्थिर शून्य
vim_menu_bar_item_menu_closed_cb(VimMenu *menu UNUSED, VimMenuBar *menubar)
{
    यदि (menubar->active_item != रिक्त)
	gtk_widget_unset_state_flags(GTK_WIDGET(menubar->active_item),
		GTK_STATE_FLAG_SELECTED);
    vim_menu_bar_set_active_item(menubar, रिक्त, TRUE);
    // Make sure to focus drawarea
    gtk_widget_grab_focus(gui.drawarea);
}

/*
 * Insert the menu item at the given index in the menu bar.
 */
    शून्य
vim_menu_bar_घुसाउ_item(VimMenuBar *self, VimMenuBarItem *item, पूर्णाङ्क idx)
{
    GtkEventController	*controller;
    GList		*next_sibling;

    next_sibling = g_list_nth(self->वस्तुहरू, idx);
    gtk_widget_घुसाउ_before(GTK_WIDGET(item), GTK_WIDGET(self),
	    next_sibling == रिक्त ? रिक्त : next_sibling->data);

    self->वस्तुहरू = g_list_घुसाउ(self->वस्तुहरू, item, idx);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_bar_item_enter_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(controller, "leave",
	    G_CALLBACK(vim_menu_bar_item_leave_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_add_controller(GTK_WIDGET(item), controller);

    g_signal_connect_object(item, "clicked",
	    G_CALLBACK(vim_menu_bar_item_clicked_cb), self, G_CONNECT_DEFAULT);

    g_signal_connect_object(item->menu, "closed",
	    G_CALLBACK(vim_menu_bar_item_menu_closed_cb),
	    self, G_CONNECT_DEFAULT);
}

/*
 * Remove the menu item or separator from the menu bar
 */
    शून्य
vim_menu_bar_remove(VimMenuBar *self, GtkWidget *item)
{
    self->वस्तुहरू = g_list_remove(self->वस्तुहरू, item);
    gtk_widget_unparent(item);
}

/*
 * Show the given menu in the menubar. If "item" is NULL, then show first menu.
 */
    शून्य
vim_menu_bar_show(VimMenuBar *self, VimMenuBarItem *item)
{
    यदि (item == रिक्त)
	item = g_list_nth_data(self->वस्तुहरू, 0);

    vim_menu_bar_set_active_item(self, item, TRUE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    स्थिर GtkWidget *
vim_menu_bar_move_active_item(VimMenuBar *self, पूर्णाङ्क dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->active_item;

    यदि (widget == रिक्त)
	फर्काऊ gtk_widget_get_first_child(GTK_WIDGET(self));

    यदि (dir > 0)
    {
	func = gtk_widget_get_next_sibling;
	null_func = gtk_widget_get_first_child;
    }
    अन्यथा
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_last_child;
    }

    जबसम्म (TRUE)
    {
	widget = func(widget);
	यदि (widget == रिक्त)
	{
	    यदि (null_func == रिक्त)
		रोक;
	    widget = null_func(GTK_WIDGET(self));
	    null_func = रिक्त;
	    यदि (widget == रिक्त)
		रोक;
	}
	रोक;
    }
    फर्काऊ widget;
}

/*
 * Menu button that can be used to perform actions, or if there is a submenu,
 * toggle the state of the submenu popover. CSS name is "modelbutton" to make it
 * styled like GtkPopoverMenu
 */
संरचना _VimMenuItem
{
    GtkButton parent;

    GtkWidget *label;	    // Displays text for button.
    GtkWidget *aux_widget;  // Either an icon or a label showing the accelerator
			    // text.

    GtkWidget *submenu;	    // Submenu popover if any (VimMenu)

    // Callback called when clicked or selected, we store this so that copying a
    // menu item works properly.
    VimMenuItemFunc	func;
    शून्य		*func_udata;
};

G_DEFINE_TYPE(VimMenuItem, vim_menu_item, GTK_TYPE_BUTTON)

    स्थिर शून्य
vim_menu_item_dispose(GObject *object)
{
    VimMenuItem *self = VIM_MENU_ITEM(object);

    g_clear_pointer(&self->label, gtk_widget_unparent);
    g_clear_pointer(&self->aux_widget, gtk_widget_unparent);
    g_clear_pointer((GtkWidget **)&self->submenu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_item_parent_class)->dispose(object);
}

    स्थिर शून्य
vim_menu_item_class_init(VimMenuItemClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_item_dispose;

    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_set_css_name(widget_class, "modelbutton");
}

    स्थिर शून्य
vim_menu_item_init(VimMenuItem *self)
{
    GtkLayoutManager *lm = gtk_widget_get_layout_manager(GTK_WIDGET(self));

    gtk_orientable_set_orientation(GTK_ORIENTABLE(lm),
	    GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_set_spacing(GTK_BOX_LAYOUT(lm), 0);
}

/*
 * Create a new menu item with the given text to display. "func" may be NULL if
 * not needed.
 */
    GtkWidget *
vim_menu_item_new(अचर वर्ण *text, VimMenuItemFunc func, शून्य *udata)
{
    VimMenuItem *item = g_object_new(VIM_TYPE_MENU_ITEM, रिक्त);

    item->func = func;
    item->func_udata = udata;
    item->label = gtk_label_new_with_mnemonic(text);

    // Make sure label is on the right and pushes everything to the left
    gtk_widget_set_halign(item->label, GTK_ALIGN_START);
    gtk_widget_set_hexpand(item->label, TRUE);
    gtk_widget_set_parent(item->label, GTK_WIDGET(item));

    फर्काऊ GTK_WIDGET(item);
}

/*
 * Update displayed text for menu item
 */
    शून्य
vim_menu_item_set_text(VimMenuItem *self, अचर वर्ण *text)
{
    gtk_label_set_text_with_mnemonic(GTK_LABEL(self->label), text);
}

    स्थिर शून्य
vim_menu_item_set_aux_widget(VimMenuItem *self, GtkWidget *aux)
{
    self->aux_widget = aux;
    gtk_widget_set_halign(self->aux_widget, GTK_ALIGN_END);
    gtk_widget_set_hexpand(self->aux_widget, FALSE);
    gtk_widget_set_margin_start(self->aux_widget, 50);
}

/*
 * Set the accelerator text for the menu item.
 */
    शून्य
vim_menu_item_set_accel(VimMenuItem *self, अचर वर्ण *accel_text)
{
    प्रतिज्ञा(self->aux_widget == रिक्त);

    vim_menu_item_set_aux_widget(self, gtk_label_new(accel_text));
    gtk_widget_घुसाउ_after(self->aux_widget, GTK_WIDGET(self), self->label);
}

/*
 * Set the submenu popover for the menu item
 */
    शून्य
vim_menu_item_set_submenu(VimMenuItem *self, VimMenu *submenu)
{
    GtkWidget *icon;

    प्रतिज्ञा(self->submenu == रिक्त);
    प्रतिज्ञा(self->aux_widget == रिक्त);

    // Add arrow icon pointing to right
    icon = gtk_image_new_from_icon_name("pan-end-symbolic");
    vim_menu_item_set_aux_widget(self, icon);
    gtk_widget_घुसाउ_after(self->aux_widget, GTK_WIDGET(self), self->label);

    gtk_popover_set_position(GTK_POPOVER(submenu), GTK_POS_दायाँ);
    // Make top of popover be aligned with button.
    gtk_widget_set_valign(GTK_WIDGET(submenu), GTK_ALIGN_START);

    self->submenu = GTK_WIDGET(submenu);
    gtk_widget_set_parent(GTK_WIDGET(submenu), GTK_WIDGET(self));
}

/*
 * Create a deep copy of the menu item
 */
    स्थिर GtkWidget *
vim_menu_item_copy(VimMenuItem *self)
{
    GtkWidget *copy;

    copy = vim_menu_item_new(
	    gtk_label_get_text(GTK_LABEL(self->label)),
	    self->func, self->func_udata);

    यदि (self->submenu != रिक्त)
	vim_menu_item_set_submenu(VIM_MENU_ITEM(copy),
		VIM_MENU(vim_menu_copy(VIM_MENU(self->submenu))));
    अन्यथा यदि (self->aux_widget != रिक्त)
	vim_menu_item_set_accel(VIM_MENU_ITEM(copy),
		gtk_label_get_text(GTK_LABEL(self->aux_widget)));
    फर्काऊ copy;
}

/*
 * Similar to GtkPopoverMenu, except uses GtkWidgets directly like GTK3, instead
 * of abstracting it into GMenuModel.
 */
संरचना _VimMenu
{
    GtkPopover parent;

    GtkWidget *box;
    GtkWidget *scr;

    GList *वस्तुहरू;

    // Currently active item showing submenu popover, or being hovered on, or
    // NULL. Note that item may have submenu but not have it open, when
    // navigating via keyboard.
    GtkWidget *active_item;

    // Used when mouse is hovering over an item and user is navigating with
    // keyboard. When the scrolled window scrolls down or up, this causes a
    // mouse enter event, causing the active item to go to the item that the
    // mouse is hovered on, instead of the next item (from keyboard navigation).
    gboolean	ignore_hover;
    द्विगुण	prev_x;
    द्विगुण	prev_y;
};

G_DEFINE_TYPE(VimMenu, vim_menu, GTK_TYPE_POPOVER)

    स्थिर शून्य
vim_menu_dispose(GObject *object)
{
    VimMenu *self = VIM_MENU(object);

    g_clear_list(&self->वस्तुहरू, (GDestroyNotify)gtk_widget_unparent);
    g_clear_pointer(&self->scr, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_parent_class)->dispose(object);
}

    स्थिर शून्य
vim_menu_class_init(VimMenuClass *class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_dispose;
}

    स्थिर gboolean
vim_menu_select_active_item(VimMenu *self, gboolean खोलो)
{
    VimMenuItem *item;

    gtk_widget_set_state_flags(self->active_item,
	    GTK_STATE_FLAG_SELECTED, FALSE);

    // Make sure to focus item, so that scrolled window knows what to do.
    gtk_widget_grab_focus(GTK_WIDGET(self->active_item));

    item = VIM_MENU_ITEM(self->active_item);
    यदि (item->func != रिक्त)
	item->func(item, VIM_MENU_ITEM_SELECTED, item->func_udata);

    यदि (खोलो && VIM_MENU_ITEM(self->active_item)->submenu != रिक्त)
    {
	GtkWidget *submenu = VIM_MENU_ITEM(self->active_item)->submenu;
	gtk_popover_popup(GTK_POPOVER(submenu));
	फर्काऊ TRUE;
    }
    फर्काऊ FALSE;
}

/*
 * Set the active item of the menu to "item". If "item" is NULL, then close any
 * submenus. If "open" is FALSE, then don't open the submenu if any.
 */
    स्थिर शून्य
vim_menu_set_active_item(VimMenu *self, VimMenuItem *item, gboolean खोलो)
{
    यदि (self->active_item == GTK_WIDGET(item))
	फर्काऊ;

    यदि (self->active_item != रिक्त)
    {
	यदि (VIM_MENU_ITEM(self->active_item)->submenu != रिक्त)
	    gtk_popover_popdown(GTK_POPOVER(
			VIM_MENU_ITEM(self->active_item)->submenu
			));
	gtk_widget_unset_state_flags(GTK_WIDGET(self->active_item),
		GTK_STATE_FLAG_SELECTED);
    }

    self->active_item = GTK_WIDGET(item);
    यदि (item != रिक्त)
	(शून्य)vim_menu_select_active_item(self, खोलो);
}

    स्थिर शून्य
vim_menu_closed_cb(VimMenu *self, शून्य *udata UNUSED)
{
    vim_menu_set_active_item(self, रिक्त, FALSE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    स्थिर GtkWidget *
vim_menu_move_active_item(VimMenu *self, पूर्णाङ्क dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->active_item;

    // If there is no currently active item, then just use the first one
    यदि (widget == रिक्त)
    {
	widget = gtk_widget_get_first_child(self->box);
	जबसम्म (widget != रिक्त && !VIM_IS_MENU_ITEM(widget))
	    widget = gtk_widget_get_next_sibling(widget);
	फर्काऊ widget;
    }

    // Could also just use GList functions, but this seems simpler (no
    // difference anyways).
    यदि (dir > 0)
    {
	func = gtk_widget_get_next_sibling;
	null_func = gtk_widget_get_first_child;
    }
    अन्यथा
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_last_child;
    }

    जबसम्म (TRUE)
    {
	widget = func(widget);
	यदि (widget == रिक्त)
	{
	    यदि (null_func == रिक्त)
		रोक;
	    widget = null_func(self->box);
	    null_func = रिक्त;
	    यदि (widget == रिक्त)
		रोक;
	}
	यदि (VIM_IS_MENU_ITEM(widget))
	    रोक;
    }
    फर्काऊ widget;
}

    स्थिर शून्य
vim_menu_reset_parent_prelight(VimMenu *self)
{
    GtkWidget	*parent = gtk_widget_get_parent(GTK_WIDGET(self));
    VimMenu	*parent_menu;

    // gtk_widget_get_ancestor assumes the widget itself is also an ancestor, so
    // we must get parent of menu first.
    parent_menu = VIM_MENU(gtk_widget_get_ancestor(parent, VIM_TYPE_MENU));

    यदि (parent_menu == रिक्त)
	// TRUE for popup menus
	फर्काऊ;

    यदि (parent_menu->active_item != रिक्त)
	gtk_widget_unset_state_flags(GTK_WIDGET(parent_menu->active_item),
		GTK_STATE_FLAG_PRELIGHT);
}

/*
 * Close all submenus in the menubar given a menu widget
 */
    स्थिर शून्य
vim_menu_close_all(VimMenu *self)
{
    VimMenuBar *menubar = GET_MENU_BAR(self);

    // Must check if NULL, because popup menus don't have a parent.
    यदि (menubar != रिक्त)
	vim_menu_bar_set_active_item(menubar, रिक्त, TRUE);
    अन्यथा
	gtk_popover_popdown(GTK_POPOVER(self));

    // Grab focus after popup menus without a menubar are closed
    gtk_widget_grab_focus(gui.drawarea);
}

    स्थिर gboolean
vim_menu_साँचो_pressed_cb(
	GtkEventController  *controller UNUSED,
	guint		    keyval,
	guint		    keycode UNUSED,
	GdkModifierType	    state,
	VimMenu		    *self)
{
    GtkWidget	*widget;

    विकल्प (keyval)
    {
	पक्ष GDK_साँचो_Down:
	पक्ष GDK_साँचो_KP_Down:
	पक्ष GDK_साँचो_Up:
	पक्ष GDK_साँचो_KP_Up:
	पक्ष GDK_साँचो_Tab:
	पक्ष GDK_साँचो_KP_Tab:
	पक्ष GDK_साँचो_ISO_बायाँ_Tab:
	    // Go to the previous or next item if any
	    widget = vim_menu_move_active_item(self,
		    (state & GDK_SHIFT_MASK)
		    || keyval == GDK_साँचो_Up ? -1 : 1);
	    vim_menu_set_active_item(self, VIM_MENU_ITEM(widget), FALSE);
	    self->ignore_hover = TRUE;
	    फर्काऊ TRUE;
	पक्ष GDK_साँचो_बायाँ:
	    // Pressing control switches menu bar item.
	    यदि (state & GDK_CONTROL_MASK)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		यदि (menubar != रिक्त)
		{
		    widget = vim_menu_bar_move_active_item(menubar, -1);
		    vim_menu_bar_set_active_item(menubar,
			    VIM_MENU_BAR_ITEM(widget), TRUE);
		}
		फर्काऊ TRUE;
	    }
	    // Go to parent menu (if any). We can do this by just closing the
	    // popover.
	    gtk_popover_popdown(GTK_POPOVER(self));
	    // For some reason when pointer is hovered over draw area, the
	    // active item in the parent menu will stay prelighted even when the
	    // active item is moved.
	    vim_menu_reset_parent_prelight(self);
	    फर्काऊ TRUE;
	पक्ष GDK_साँचो_दायाँ:
	    यदि (state & GDK_CONTROL_MASK)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		यदि (menubar != रिक्त)
		{
		    widget = vim_menu_bar_move_active_item(menubar, 1);
		    vim_menu_bar_set_active_item(menubar,
			    VIM_MENU_BAR_ITEM(widget), TRUE);
		}
		फर्काऊ TRUE;
	    }
	    // Open submenu if active item has one
	    यदि (self->active_item != रिक्त
		    && vim_menu_select_active_item(self, TRUE))
	    {
		// Select first item in opened submenu
		VimMenu *submenu = VIM_MENU(
			VIM_MENU_ITEM(self->active_item)->submenu
			);

		vim_menu_set_active_item(submenu,
			VIM_MENU_ITEM(
			    gtk_widget_get_first_child(submenu->box)),
			FALSE);
	    }
	    self->ignore_hover = TRUE;
	    फर्काऊ TRUE;
	पक्ष GDK_साँचो_Escape:
	    // Close all popover menus
	    vim_menu_close_all(self);
	    फर्काऊ TRUE;
	पक्ष GDK_साँचो_ISO_Enter:
	पक्ष GDK_साँचो_3270_Enter:
	पक्ष GDK_साँचो_KP_Enter:
	पक्ष GDK_साँचो_Return:
	    यदि (self->active_item != रिक्त)
		g_signal_emit_by_name(self->active_item, "clicked");
	    फर्काऊ TRUE;
	स्वभाव:
	    रोक;
    }
    फर्काऊ FALSE;
}


    स्थिर शून्य
vim_menu_motion_cb(
	GtkEventController  *controller UNUSED,
	द्विगुण		    x,
	द्विगुण		    y,
	VimMenu		    *self)
{
    यदि (self->prev_x == -1 || self->prev_y == -1 ||
	    (भ_असल(self->prev_x - x) > 0.05 && भ_असल(self->prev_y - y) > 0.05))
	self->ignore_hover = FALSE;
    self->prev_x = x;
    self->prev_y = y;
}

    स्थिर शून्य
vim_menu_focus_cb(GtkEventController *controller UNUSED, VimMenu *self)
{
    gtk_popover_set_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    स्थिर शून्य
vim_menu_notify_mnemonics_cb(
	VimMenu	    *self,
	GParamSpec  *pspec UNUSED,
	शून्य	    *udata UNUSED)
{
    gtk_popover_set_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    स्थिर शून्य
vim_menu_init(VimMenu *self)
{
    GtkEventController	*controller;
    GtkWidget		*थाक;
    GtkWidget		*parent_box;
    GListModel		*controllers;

    gtk_popover_set_has_arrow(GTK_POPOVER(self), FALSE);
    gtk_popover_set_autohide(GTK_POPOVER(self), TRUE);

    // Do not make child popovers close parent popovers when they are closed.
    gtk_popover_set_cascade_popdown(GTK_POPOVER(self), FALSE);

    थाक = gtk_थाक_new();

    // "stack" and "parent_box" have no use other than to make the css structure
    // of the popup menu be exactly like GtkPopoverMenu. This is so that GTK
    // themes style VimMenu exactly like GtkPopoverMenu.
    parent_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_थाक_add_child(GTK_थाक(थाक), parent_box);
    gtk_थाक_set_visible_child(GTK_थाक(थाक), parent_box);

    self->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(self->box, TRUE);
    gtk_widget_set_vexpand(self->box, TRUE);
    gtk_box_append(GTK_BOX(parent_box), self->box);

    self->scr = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(self->scr),
	    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_propagate_natural_width(
	    GTK_SCROLLED_WINDOW(self->scr), TRUE);
    gtk_scrolled_window_set_propagate_natural_height(
	    GTK_SCROLLED_WINDOW(self->scr), TRUE);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(self->scr), थाक);
    gtk_popover_set_child(GTK_POPOVER(self), self->scr);

    gtk_widget_add_css_class(GTK_WIDGET(self), "menu");

    // Add key controller for basic movement
    controller = gtk_event_controller_साँचो_new();
    // Make sure we get the key presses first and handle them if possible
    gtk_event_controller_set_propagation_phase(controller,
	    GTK_PHASE_CAPTURE);
    g_signal_connect_object(controller, "key-pressed",
	    G_CALLBACK(vim_menu_साँचो_pressed_cb),
	    self, G_CONNECT_DEFAULT);
    gtk_widget_add_controller(GTK_WIDGET(self), controller);

    // Show mnemonic underline always
    controller = gtk_event_controller_focus_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_focus_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_add_controller(GTK_WIDGET(self), controller);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "motion",
	    G_CALLBACK(vim_menu_motion_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_add_controller(GTK_WIDGET(self), controller);

    g_signal_connect(self, "closed", G_CALLBACK(vim_menu_closed_cb), रिक्त);

    // Set all shortcut controllers in the window to not require a modifier for
    // mnemonics.
    controllers = gtk_widget_observe_controllers(GTK_WIDGET(self));
    लागि (guint i = 0; i < g_list_model_get_n_वस्तुहरू(controllers); i++)
    {
	controller = g_list_model_get_item(controllers, i);
	यदि (GTK_IS_SHORTCUT_CONTROLLER(controller))
	    gtk_shortcut_controller_set_mnemonics_modifiers(
		    GTK_SHORTCUT_CONTROLLER(controller), 0);
    }
    g_object_unref(controllers);

    // Force enable mnemonics underline
    g_signal_connect(self, "notify::mnemonics-visible",
	    G_CALLBACK(vim_menu_notify_mnemonics_cb), रिक्त);

    self->prev_x = self->prev_y = -1;
}

    GtkWidget *
vim_menu_new(शून्य)
{
    फर्काऊ g_object_new(VIM_TYPE_MENU, रिक्त);
}

    स्थिर GtkWidget *
vim_menu_घुसाउ(VimMenu *self, GtkWidget *item, पूर्णाङ्क idx)
{
    यदि (idx > 0)
    {
	GList *prev = g_list_nth(self->वस्तुहरू, idx - 1);
	यदि (prev == रिक्त)
	    gtk_box_append(GTK_BOX(self->box), item);
	अन्यथा
	    gtk_box_घुसाउ_child_after(GTK_BOX(self->box), item,
		    prev->data);
    }
    अन्यथा यदि (idx == 0)
	gtk_box_prepend(GTK_BOX(self->box), item);
    अन्यथा
	gtk_box_append(GTK_BOX(self->box), item);

    self->वस्तुहरू = g_list_घुसाउ(self->वस्तुहरू, item, idx);
    फर्काऊ item;
}

    स्थिर शून्य
vim_menu_item_clicked_cb(VimMenuItem *self, VimMenu *menu)
{
    // Only close all menus if item is a regular button (no submenu). If item
    // has a submenu, then just toggle it on and off.
    यदि (self->submenu != रिक्त)
    {
	यदि (gtk_widget_is_visible(self->submenu))
	    gtk_popover_popdown(GTK_POPOVER(self->submenu));
	अन्यथा
	    gtk_popover_popup(GTK_POPOVER(self->submenu));
	फर्काऊ;
    }

    // Since we set the "cascade-popdown" property to FALSE, we must popdown the
    // toplevel menu/popover, so that all submenus are closed.
    vim_menu_close_all(menu);
    यदि (self->func != रिक्त)
	self->func(self, VIM_MENU_ITEM_CLICKED, self->func_udata);
}

    स्थिर gboolean
vim_menu_item_mnemonic_activate_cb(
	VimMenuItem *self,
	gboolean    group UNUSED,
	VimMenu	    *menu)
{
    vim_menu_set_active_item(menu, self, FALSE);
    // Return FALSE so that menu item emits "clicked" signal
    फर्काऊ FALSE;
}

    स्थिर शून्य
vim_menu_item_enter_cb(
	GtkEventController  *controller,
	द्विगुण		    x UNUSED,
	द्विगुण		    y UNUSED,
	VimMenu		    *menu)
{
    VimMenuItem  *self;

    यदि (menu->ignore_hover || !gtk_event_controller_motion_contains_pointer(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	फर्काऊ;

    self = VIM_MENU_ITEM(gtk_event_controller_get_widget(controller));
    vim_menu_set_active_item(menu, self, TRUE);
}

    स्थिर शून्य
vim_menu_item_leave_cb(GtkEventController *controller, VimMenu *menu)
{
    VimMenuItem  *self;

    यदि (gtk_event_controller_motion_contains_pointer(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	फर्काऊ;

    self = VIM_MENU_ITEM(gtk_event_controller_get_widget(controller));
    यदि (menu->active_item == GTK_WIDGET(self))
	vim_menu_set_active_item(menu, रिक्त, FALSE);
}

/*
 * Insert the menu item at the given index in the menu. If "idx" is negative,
 * then append the menu item.
 */
    शून्य
vim_menu_घुसाउ_item(VimMenu *self, VimMenuItem *item, पूर्णाङ्क idx)
{
    GtkEventController *controller;

    vim_menu_घुसाउ(self, GTK_WIDGET(item), idx);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_item_enter_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(controller, "leave",
	    G_CALLBACK(vim_menu_item_leave_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_add_controller(GTK_WIDGET(item), controller);

    g_signal_connect_object(item, "clicked",
	    G_CALLBACK(vim_menu_item_clicked_cb),
	    self, G_CONNECT_DEFAULT);
    g_signal_connect_object(item, "mnemonic-activate",
	    G_CALLBACK(vim_menu_item_mnemonic_activate_cb),
	    self, G_CONNECT_DEFAULT);
}

/*
 * Insert a separator at the given position and return it.
 */
    GtkWidget *
vim_menu_घुसाउ_separator(VimMenu *self, पूर्णाङ्क idx)
{
    फर्काऊ vim_menu_घुसाउ(self,
	    gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), idx);
}

/*
 * Remove the menu item or separator from the menu
 */
    शून्य
vim_menu_remove(VimMenu *self, GtkWidget *item)
{
    self->वस्तुहरू = g_list_remove(self->वस्तुहरू, item);
    gtk_box_remove(GTK_BOX(self->box), item);
}

/*
 * Create a deep copy of the menu
 */
    GtkWidget *
vim_menu_copy(VimMenu *self)
{
    GtkWidget	*copy = vim_menu_new();
    पूर्णाङ्क		i = 0;

    लागि (GList *l = self->वस्तुहरू; l != रिक्त; l = l->next, i++)
    {
	VimMenuItem *item;
	GtkWidget   *item_copy;

	यदि (!VIM_IS_MENU_ITEM(l->data))
	{
	    प्रतिज्ञा(GTK_IS_SEPARATOR(l->data));
	    vim_menu_घुसाउ_separator(VIM_MENU(copy), i);
	    जारी;
	}

	item = l->data;
	item_copy = vim_menu_item_copy(item);

	vim_menu_घुसाउ_item(VIM_MENU(copy), VIM_MENU_ITEM(item_copy), i);
    }
    फर्काऊ copy;
}

#endif // FEAT_MENU
