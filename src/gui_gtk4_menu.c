/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#અન્તર્ભાવ "vim.h"

#ifdef FEAT_MENU

#અન્તર્ભાવ <gtk/gtk.h>
#અન્તર્ભાવ "gui_gtk4_menu.h"

// Note that this may return NULL for popup menus
#define GET_MENU_BAR(m) VIM_MENU_BAR(gtk_widget_get_ancestor( \
	    GTK_WIDGET(m), VIM_TYPE_MENU_BAR))

/*
 * Similar as GtkButton but set CSS name to "item" to emulate GtkPopoverMenuBar
 * styling. Always has a submenu.
 */
સંરચના _VimMenuBarItem
{
    GtkButton parent;

    GtkWidget *menu;
};

G_DEFINE_TYPE(VimMenuBarItem, vim_menu_bar_item, GTK_TYPE_BUTTON)

    સ્થિર શૂન્ય
vim_menu_bar_item_dispose(GObject *object)
{
    VimMenuBarItem *self = VIM_MENU_BAR_ITEM(object);

    g_clear_pointer((GtkWidget **)&self->menu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_item_parent_class)->dispose(object);
}

    સ્થિર શૂન્ય
vim_menu_bar_item_class_init(VimMenuBarItemClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_item_dispose;

    gtk_widget_class_set_css_name(widget_class, "item");
}

    સ્થિર શૂન્ય
vim_menu_bar_item_init(VimMenuBarItem *self)
{
    // Enable mnemonics
    gtk_button_set_use_underline(GTK_BUTTON(self), TRUE);
}

    GtkWidget *
vim_menu_bar_item_new(અચલ અક્ષર *text, VimMenu *menu)
{
    VimMenuBarItem *item = g_object_new(VIM_TYPE_MENU_BAR_ITEM, ખાલી);

    gtk_button_set_label(GTK_BUTTON(item), text);

    item->menu = GTK_WIDGET(menu);
    gtk_popover_set_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    // Make popover start at top left corner
    gtk_widget_set_halign(GTK_WIDGET(menu), GTK_ALIGN_START);
    gtk_widget_set_parent(GTK_WIDGET(menu), GTK_WIDGET(item));

    પાછા GTK_WIDGET(item);
}

    શૂન્ય
vim_menu_bar_item_set_text(VimMenuBarItem *self, અચલ અક્ષર *text)
{
    gtk_button_set_label(GTK_BUTTON(self), text);
}

/*
 * Similar to GtkPopoverMenuBar
 */
સંરચના _VimMenuBar
{
    GtkWidget parent;

    GList *વસ્તુઓ;

    // Currently visible item that has submenu popped up, else NULL
    GtkWidget *active_item;
};

G_DEFINE_TYPE(VimMenuBar, vim_menu_bar, GTK_TYPE_WIDGET)

    સ્થિર શૂન્ય
vim_menu_bar_dispose(GObject *object)
{
    VimMenuBar *self = VIM_MENU_BAR(object);

    g_clear_list(&self->વસ્તુઓ, (GDestroyNotify)gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_parent_class)->dispose(object);
}

    સ્થિર શૂન્ય
vim_menu_bar_class_init(VimMenuBarClass *class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_dispose;

    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_set_css_name(widget_class, "menubar");
}

    સ્થિર શૂન્ય
vim_menu_bar_init(VimMenuBar *self)
{
    GtkLayoutManager *lm = gtk_widget_get_layout_manager(GTK_WIDGET(self));

    gtk_orientable_set_orientation(GTK_ORIENTABLE(lm),
	    GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_set_spacing(GTK_BOX_LAYOUT(lm), 0);
}

    GtkWidget *
vim_menu_bar_new(શૂન્ય)
{
    પાછા g_object_new(VIM_TYPE_MENU_BAR, ખાલી);
}

/*
 * Create a VimMenu widget with the menus of the menu bar as its submenus. Note
 * that it is a deep copy.
 */
    GtkWidget *
vim_menu_bar_to_menu(VimMenuBar *self)
{
    GtkWidget	*menu = vim_menu_new();
    પૂર્ણાંક		i = 0;

    માટે (GList *l = self->વસ્તુઓ; l != ખાલી; l = l->next, i++)
    {
	VimMenuBarItem	*baritem = l->data;
	GtkWidget	*item;

	item = vim_menu_item_new(
		gtk_button_get_label(GTK_BUTTON(baritem)), ખાલી, ખાલી);

	vim_menu_item_set_submenu(VIM_MENU_ITEM(item),
		VIM_MENU(vim_menu_copy(VIM_MENU(baritem->menu))));

	vim_menu_ઉમેરો_item(VIM_MENU(menu), VIM_MENU_ITEM(item), i);
    }
    પાછા menu;
}

/*
 * Set the currently active menu of the menubar to "item". If NULL, then close
 * any submenus.
 */
    સ્થિર શૂન્ય
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
    જો ((!force && self->active_item == ખાલી)
	    || self->active_item == GTK_WIDGET(item))
    {
	જો (self->active_item == ખાલી)
	    gtk_widget_set_state_flags(GTK_WIDGET(item),
		    GTK_STATE_FLAG_SELECTED, FALSE);
	પાછા;
    }

    જો (self->active_item != ખાલી)
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
    જો (item != ખાલી)
    {
	gtk_popover_popup(GTK_POPOVER(item->menu));
	gtk_widget_set_state_flags(GTK_WIDGET(item),
		GTK_STATE_FLAG_SELECTED, FALSE);
    }
}

    સ્થિર શૂન્ય
vim_menu_bar_item_enter_cb(
	GtkEventController  *controller,
	દ્વિગુણ		    x UNUSED,
	દ્વિગુણ		    y UNUSED,
	VimMenuBar	    *menubar)
{
    VimMenuBarItem  *self;

    self = VIM_MENU_BAR_ITEM(gtk_event_controller_get_widget(controller));
    vim_menu_bar_set_active_item(menubar, self, FALSE);
}

    સ્થિર શૂન્ય
vim_menu_bar_item_leave_cb(
	GtkEventController  *controller,
	VimMenuBar	    *menubar)
{
    VimMenuBarItem *self;

    self = VIM_MENU_BAR_ITEM(gtk_event_controller_get_widget(controller));

    // If the item is the currently active item, then don't deselect it.
    જો (menubar->active_item != GTK_WIDGET(self))
	gtk_widget_unset_state_flags(GTK_WIDGET(self),
		GTK_STATE_FLAG_SELECTED);
}

    સ્થિર શૂન્ય
vim_menu_bar_item_clicked_cb(VimMenuBarItem *self, VimMenuBar *menubar)
{
    vim_menu_bar_set_active_item(menubar, self, TRUE);
}

    સ્થિર શૂન્ય
vim_menu_bar_item_menu_closed_cb(VimMenu *menu UNUSED, VimMenuBar *menubar)
{
    જો (menubar->active_item != ખાલી)
	gtk_widget_unset_state_flags(GTK_WIDGET(menubar->active_item),
		GTK_STATE_FLAG_SELECTED);
    vim_menu_bar_set_active_item(menubar, ખાલી, TRUE);
    // Make sure to focus drawarea
    gtk_widget_grab_focus(gui.drawarea);
}

/*
 * Insert the menu item at the given index in the menu bar.
 */
    શૂન્ય
vim_menu_bar_ઉમેરો_item(VimMenuBar *self, VimMenuBarItem *item, પૂર્ણાંક idx)
{
    GtkEventController	*controller;
    GList		*next_sibling;

    next_sibling = g_list_nth(self->વસ્તુઓ, idx);
    gtk_widget_ઉમેરો_before(GTK_WIDGET(item), GTK_WIDGET(self),
	    next_sibling == ખાલી ? ખાલી : next_sibling->data);

    self->વસ્તુઓ = g_list_ઉમેરો(self->વસ્તુઓ, item, idx);

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
    શૂન્ય
vim_menu_bar_remove(VimMenuBar *self, GtkWidget *item)
{
    self->વસ્તુઓ = g_list_remove(self->વસ્તુઓ, item);
    gtk_widget_unparent(item);
}

/*
 * Show the given menu in the menubar. If "item" is NULL, then show first menu.
 */
    શૂન્ય
vim_menu_bar_show(VimMenuBar *self, VimMenuBarItem *item)
{
    જો (item == ખાલી)
	item = g_list_nth_data(self->વસ્તુઓ, 0);

    vim_menu_bar_set_active_item(self, item, TRUE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    સ્થિર GtkWidget *
vim_menu_bar_move_active_item(VimMenuBar *self, પૂર્ણાંક dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->active_item;

    જો (widget == ખાલી)
	પાછા gtk_widget_get_first_child(GTK_WIDGET(self));

    જો (dir > 0)
    {
	func = gtk_widget_get_next_sibling;
	null_func = gtk_widget_get_first_child;
    }
    નહીંતર
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_last_child;
    }

    જ્યારે (TRUE)
    {
	widget = func(widget);
	જો (widget == ખાલી)
	{
	    જો (null_func == ખાલી)
		તોડો;
	    widget = null_func(GTK_WIDGET(self));
	    null_func = ખાલી;
	    જો (widget == ખાલી)
		તોડો;
	}
	તોડો;
    }
    પાછા widget;
}

/*
 * Menu button that can be used to perform actions, or if there is a submenu,
 * toggle the state of the submenu popover. CSS name is "modelbutton" to make it
 * styled like GtkPopoverMenu
 */
સંરચના _VimMenuItem
{
    GtkButton parent;

    GtkWidget *label;	    // Displays text for button.
    GtkWidget *aux_widget;  // Either an icon or a label showing the accelerator
			    // text.

    GtkWidget *submenu;	    // Submenu popover if any (VimMenu)

    // Callback called when clicked or selected, we store this so that copying a
    // menu item works properly.
    VimMenuItemFunc	func;
    શૂન્ય		*func_udata;
};

G_DEFINE_TYPE(VimMenuItem, vim_menu_item, GTK_TYPE_BUTTON)

    સ્થિર શૂન્ય
vim_menu_item_dispose(GObject *object)
{
    VimMenuItem *self = VIM_MENU_ITEM(object);

    g_clear_pointer(&self->label, gtk_widget_unparent);
    g_clear_pointer(&self->aux_widget, gtk_widget_unparent);
    g_clear_pointer((GtkWidget **)&self->submenu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_item_parent_class)->dispose(object);
}

    સ્થિર શૂન્ય
vim_menu_item_class_init(VimMenuItemClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_item_dispose;

    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_set_css_name(widget_class, "modelbutton");
}

    સ્થિર શૂન્ય
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
vim_menu_item_new(અચલ અક્ષર *text, VimMenuItemFunc func, શૂન્ય *udata)
{
    VimMenuItem *item = g_object_new(VIM_TYPE_MENU_ITEM, ખાલી);

    item->func = func;
    item->func_udata = udata;
    item->label = gtk_label_new_with_mnemonic(text);

    // Make sure label is on the right and pushes everything to the left
    gtk_widget_set_halign(item->label, GTK_ALIGN_START);
    gtk_widget_set_hexpand(item->label, TRUE);
    gtk_widget_set_parent(item->label, GTK_WIDGET(item));

    પાછા GTK_WIDGET(item);
}

/*
 * Update displayed text for menu item
 */
    શૂન્ય
vim_menu_item_set_text(VimMenuItem *self, અચલ અક્ષર *text)
{
    gtk_label_set_text_with_mnemonic(GTK_LABEL(self->label), text);
}

    સ્થિર શૂન્ય
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
    શૂન્ય
vim_menu_item_set_accel(VimMenuItem *self, અચલ અક્ષર *accel_text)
{
    પ્રતિજ્ઞા(self->aux_widget == ખાલી);

    vim_menu_item_set_aux_widget(self, gtk_label_new(accel_text));
    gtk_widget_ઉમેરો_after(self->aux_widget, GTK_WIDGET(self), self->label);
}

/*
 * Set the submenu popover for the menu item
 */
    શૂન્ય
vim_menu_item_set_submenu(VimMenuItem *self, VimMenu *submenu)
{
    GtkWidget *icon;

    પ્રતિજ્ઞા(self->submenu == ખાલી);
    પ્રતિજ્ઞા(self->aux_widget == ખાલી);

    // Add arrow icon pointing to right
    icon = gtk_image_new_from_icon_name("pan-end-symbolic");
    vim_menu_item_set_aux_widget(self, icon);
    gtk_widget_ઉમેરો_after(self->aux_widget, GTK_WIDGET(self), self->label);

    gtk_popover_set_position(GTK_POPOVER(submenu), GTK_POS_જમણું);
    // Make top of popover be aligned with button.
    gtk_widget_set_valign(GTK_WIDGET(submenu), GTK_ALIGN_START);

    self->submenu = GTK_WIDGET(submenu);
    gtk_widget_set_parent(GTK_WIDGET(submenu), GTK_WIDGET(self));
}

/*
 * Create a deep copy of the menu item
 */
    સ્થિર GtkWidget *
vim_menu_item_copy(VimMenuItem *self)
{
    GtkWidget *copy;

    copy = vim_menu_item_new(
	    gtk_label_get_text(GTK_LABEL(self->label)),
	    self->func, self->func_udata);

    જો (self->submenu != ખાલી)
	vim_menu_item_set_submenu(VIM_MENU_ITEM(copy),
		VIM_MENU(vim_menu_copy(VIM_MENU(self->submenu))));
    નહીંતર જો (self->aux_widget != ખાલી)
	vim_menu_item_set_accel(VIM_MENU_ITEM(copy),
		gtk_label_get_text(GTK_LABEL(self->aux_widget)));
    પાછા copy;
}

/*
 * Similar to GtkPopoverMenu, except uses GtkWidgets directly like GTK3, instead
 * of abstracting it into GMenuModel.
 */
સંરચના _VimMenu
{
    GtkPopover parent;

    GtkWidget *box;
    GtkWidget *scr;

    GList *વસ્તુઓ;

    // Currently active item showing submenu popover, or being hovered on, or
    // NULL. Note that item may have submenu but not have it open, when
    // navigating via keyboard.
    GtkWidget *active_item;

    // Used when mouse is hovering over an item and user is navigating with
    // keyboard. When the scrolled window scrolls down or up, this causes a
    // mouse enter event, causing the active item to go to the item that the
    // mouse is hovered on, instead of the next item (from keyboard navigation).
    gboolean	ignore_hover;
    દ્વિગુણ	prev_x;
    દ્વિગુણ	prev_y;
};

G_DEFINE_TYPE(VimMenu, vim_menu, GTK_TYPE_POPOVER)

    સ્થિર શૂન્ય
vim_menu_dispose(GObject *object)
{
    VimMenu *self = VIM_MENU(object);

    g_clear_list(&self->વસ્તુઓ, (GDestroyNotify)gtk_widget_unparent);
    g_clear_pointer(&self->scr, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_parent_class)->dispose(object);
}

    સ્થિર શૂન્ય
vim_menu_class_init(VimMenuClass *class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_dispose;
}

    સ્થિર gboolean
vim_menu_select_active_item(VimMenu *self, gboolean open)
{
    VimMenuItem *item;

    gtk_widget_set_state_flags(self->active_item,
	    GTK_STATE_FLAG_SELECTED, FALSE);

    // Make sure to focus item, so that scrolled window knows what to do.
    gtk_widget_grab_focus(GTK_WIDGET(self->active_item));

    item = VIM_MENU_ITEM(self->active_item);
    જો (item->func != ખાલી)
	item->func(item, VIM_MENU_ITEM_SELECTED, item->func_udata);

    જો (open && VIM_MENU_ITEM(self->active_item)->submenu != ખાલી)
    {
	GtkWidget *submenu = VIM_MENU_ITEM(self->active_item)->submenu;
	gtk_popover_popup(GTK_POPOVER(submenu));
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Set the active item of the menu to "item". If "item" is NULL, then close any
 * submenus. If "open" is FALSE, then don't open the submenu if any.
 */
    સ્થિર શૂન્ય
vim_menu_set_active_item(VimMenu *self, VimMenuItem *item, gboolean open)
{
    જો (self->active_item == GTK_WIDGET(item))
	પાછા;

    જો (self->active_item != ખાલી)
    {
	જો (VIM_MENU_ITEM(self->active_item)->submenu != ખાલી)
	    gtk_popover_popdown(GTK_POPOVER(
			VIM_MENU_ITEM(self->active_item)->submenu
			));
	gtk_widget_unset_state_flags(GTK_WIDGET(self->active_item),
		GTK_STATE_FLAG_SELECTED);
    }

    self->active_item = GTK_WIDGET(item);
    જો (item != ખાલી)
	(શૂન્ય)vim_menu_select_active_item(self, open);
}

    સ્થિર શૂન્ય
vim_menu_closed_cb(VimMenu *self, શૂન્ય *udata UNUSED)
{
    vim_menu_set_active_item(self, ખાલી, FALSE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    સ્થિર GtkWidget *
vim_menu_move_active_item(VimMenu *self, પૂર્ણાંક dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->active_item;

    // If there is no currently active item, then just use the first one
    જો (widget == ખાલી)
    {
	widget = gtk_widget_get_first_child(self->box);
	જ્યારે (widget != ખાલી && !VIM_IS_MENU_ITEM(widget))
	    widget = gtk_widget_get_next_sibling(widget);
	પાછા widget;
    }

    // Could also just use GList functions, but this seems simpler (no
    // difference anyways).
    જો (dir > 0)
    {
	func = gtk_widget_get_next_sibling;
	null_func = gtk_widget_get_first_child;
    }
    નહીંતર
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_last_child;
    }

    જ્યારે (TRUE)
    {
	widget = func(widget);
	જો (widget == ખાલી)
	{
	    જો (null_func == ખાલી)
		તોડો;
	    widget = null_func(self->box);
	    null_func = ખાલી;
	    જો (widget == ખાલી)
		તોડો;
	}
	જો (VIM_IS_MENU_ITEM(widget))
	    તોડો;
    }
    પાછા widget;
}

    સ્થિર શૂન્ય
vim_menu_reset_parent_prelight(VimMenu *self)
{
    GtkWidget	*parent = gtk_widget_get_parent(GTK_WIDGET(self));
    VimMenu	*parent_menu;

    // gtk_widget_get_ancestor assumes the widget itself is also an ancestor, so
    // we must get parent of menu first.
    parent_menu = VIM_MENU(gtk_widget_get_ancestor(parent, VIM_TYPE_MENU));

    જો (parent_menu == ખાલી)
	// TRUE for popup menus
	પાછા;

    જો (parent_menu->active_item != ખાલી)
	gtk_widget_unset_state_flags(GTK_WIDGET(parent_menu->active_item),
		GTK_STATE_FLAG_PRELIGHT);
}

/*
 * Close all submenus in the menubar given a menu widget
 */
    સ્થિર શૂન્ય
vim_menu_close_all(VimMenu *self)
{
    VimMenuBar *menubar = GET_MENU_BAR(self);

    // Must check if NULL, because popup menus don't have a parent.
    જો (menubar != ખાલી)
	vim_menu_bar_set_active_item(menubar, ખાલી, TRUE);
    નહીંતર
	gtk_popover_popdown(GTK_POPOVER(self));

    // Grab focus after popup menus without a menubar are closed
    gtk_widget_grab_focus(gui.drawarea);
}

    સ્થિર gboolean
vim_menu_ચાવી_pressed_cb(
	GtkEventController  *controller UNUSED,
	guint		    keyval,
	guint		    keycode UNUSED,
	GdkModifierType	    state,
	VimMenu		    *self)
{
    GtkWidget	*widget;

    પસંદગી (keyval)
    {
	સ્થિતિ GDK_ચાવી_Down:
	સ્થિતિ GDK_ચાવી_KP_Down:
	સ્થિતિ GDK_ચાવી_Up:
	સ્થિતિ GDK_ચાવી_KP_Up:
	સ્થિતિ GDK_ચાવી_Tab:
	સ્થિતિ GDK_ચાવી_KP_Tab:
	સ્થિતિ GDK_ચાવી_ISO_ડાબું_Tab:
	    // Go to the previous or next item if any
	    widget = vim_menu_move_active_item(self,
		    (state & GDK_SHIFT_MASK)
		    || keyval == GDK_ચાવી_Up ? -1 : 1);
	    vim_menu_set_active_item(self, VIM_MENU_ITEM(widget), FALSE);
	    self->ignore_hover = TRUE;
	    પાછા TRUE;
	સ્થિતિ GDK_ચાવી_ડાબું:
	    // Pressing control switches menu bar item.
	    જો (state & GDK_CONTROL_MASK)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		જો (menubar != ખાલી)
		{
		    widget = vim_menu_bar_move_active_item(menubar, -1);
		    vim_menu_bar_set_active_item(menubar,
			    VIM_MENU_BAR_ITEM(widget), TRUE);
		}
		પાછા TRUE;
	    }
	    // Go to parent menu (if any). We can do this by just closing the
	    // popover.
	    gtk_popover_popdown(GTK_POPOVER(self));
	    // For some reason when pointer is hovered over draw area, the
	    // active item in the parent menu will stay prelighted even when the
	    // active item is moved.
	    vim_menu_reset_parent_prelight(self);
	    પાછા TRUE;
	સ્થિતિ GDK_ચાવી_જમણું:
	    જો (state & GDK_CONTROL_MASK)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		જો (menubar != ખાલી)
		{
		    widget = vim_menu_bar_move_active_item(menubar, 1);
		    vim_menu_bar_set_active_item(menubar,
			    VIM_MENU_BAR_ITEM(widget), TRUE);
		}
		પાછા TRUE;
	    }
	    // Open submenu if active item has one
	    જો (self->active_item != ખાલી
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
	    પાછા TRUE;
	સ્થિતિ GDK_ચાવી_Escape:
	    // Close all popover menus
	    vim_menu_close_all(self);
	    પાછા TRUE;
	સ્થિતિ GDK_ચાવી_ISO_Enter:
	સ્થિતિ GDK_ચાવી_3270_Enter:
	સ્થિતિ GDK_ચાવી_KP_Enter:
	સ્થિતિ GDK_ચાવી_Return:
	    જો (self->active_item != ખાલી)
		g_signal_emit_by_name(self->active_item, "clicked");
	    પાછા TRUE;
	મૂળભૂત:
	    તોડો;
    }
    પાછા FALSE;
}


    સ્થિર શૂન્ય
vim_menu_motion_cb(
	GtkEventController  *controller UNUSED,
	દ્વિગુણ		    x,
	દ્વિગુણ		    y,
	VimMenu		    *self)
{
    જો (self->prev_x == -1 || self->prev_y == -1 ||
	    (fabs(self->prev_x - x) > 0.05 && fabs(self->prev_y - y) > 0.05))
	self->ignore_hover = FALSE;
    self->prev_x = x;
    self->prev_y = y;
}

    સ્થિર શૂન્ય
vim_menu_focus_cb(GtkEventController *controller UNUSED, VimMenu *self)
{
    gtk_popover_set_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    સ્થિર શૂન્ય
vim_menu_notify_mnemonics_cb(
	VimMenu	    *self,
	GParamSpec  *pspec UNUSED,
	શૂન્ય	    *udata UNUSED)
{
    gtk_popover_set_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    સ્થિર શૂન્ય
vim_menu_init(VimMenu *self)
{
    GtkEventController	*controller;
    GtkWidget		*ઢગલો;
    GtkWidget		*parent_box;
    GListModel		*controllers;

    gtk_popover_set_has_arrow(GTK_POPOVER(self), FALSE);
    gtk_popover_set_autohide(GTK_POPOVER(self), TRUE);

    // Do not make child popovers close parent popovers when they are closed.
    gtk_popover_set_cascade_popdown(GTK_POPOVER(self), FALSE);

    ઢગલો = gtk_ઢગલો_new();

    // "stack" and "parent_box" have no use other than to make the css structure
    // of the popup menu be exactly like GtkPopoverMenu. This is so that GTK
    // themes style VimMenu exactly like GtkPopoverMenu.
    parent_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_ઢગલો_add_child(GTK_ઢગલો(ઢગલો), parent_box);
    gtk_ઢગલો_set_visible_child(GTK_ઢગલો(ઢગલો), parent_box);

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

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(self->scr), ઢગલો);
    gtk_popover_set_child(GTK_POPOVER(self), self->scr);

    gtk_widget_add_css_class(GTK_WIDGET(self), "menu");

    // Add key controller for basic movement
    controller = gtk_event_controller_ચાવી_new();
    // Make sure we get the key presses first and handle them if possible
    gtk_event_controller_set_propagation_phase(controller,
	    GTK_PHASE_CAPTURE);
    g_signal_connect_object(controller, "key-pressed",
	    G_CALLBACK(vim_menu_ચાવી_pressed_cb),
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

    g_signal_connect(self, "closed", G_CALLBACK(vim_menu_closed_cb), ખાલી);

    // Set all shortcut controllers in the window to not require a modifier for
    // mnemonics.
    controllers = gtk_widget_observe_controllers(GTK_WIDGET(self));
    માટે (guint i = 0; i < g_list_model_get_n_વસ્તુઓ(controllers); i++)
    {
	controller = g_list_model_get_item(controllers, i);
	જો (GTK_IS_SHORTCUT_CONTROLLER(controller))
	    gtk_shortcut_controller_set_mnemonics_modifiers(
		    GTK_SHORTCUT_CONTROLLER(controller), 0);
    }
    g_object_unref(controllers);

    // Force enable mnemonics underline
    g_signal_connect(self, "notify::mnemonics-visible",
	    G_CALLBACK(vim_menu_notify_mnemonics_cb), ખાલી);

    self->prev_x = self->prev_y = -1;
}

    GtkWidget *
vim_menu_new(શૂન્ય)
{
    પાછા g_object_new(VIM_TYPE_MENU, ખાલી);
}

    સ્થિર GtkWidget *
vim_menu_ઉમેરો(VimMenu *self, GtkWidget *item, પૂર્ણાંક idx)
{
    જો (idx > 0)
    {
	GList *prev = g_list_nth(self->વસ્તુઓ, idx - 1);
	જો (prev == ખાલી)
	    gtk_box_append(GTK_BOX(self->box), item);
	નહીંતર
	    gtk_box_ઉમેરો_child_after(GTK_BOX(self->box), item,
		    prev->data);
    }
    નહીંતર જો (idx == 0)
	gtk_box_prepend(GTK_BOX(self->box), item);
    નહીંતર
	gtk_box_append(GTK_BOX(self->box), item);

    self->વસ્તુઓ = g_list_ઉમેરો(self->વસ્તુઓ, item, idx);
    પાછા item;
}

    સ્થિર શૂન્ય
vim_menu_item_clicked_cb(VimMenuItem *self, VimMenu *menu)
{
    // Only close all menus if item is a regular button (no submenu). If item
    // has a submenu, then just toggle it on and off.
    જો (self->submenu != ખાલી)
    {
	જો (gtk_widget_is_visible(self->submenu))
	    gtk_popover_popdown(GTK_POPOVER(self->submenu));
	નહીંતર
	    gtk_popover_popup(GTK_POPOVER(self->submenu));
	પાછા;
    }

    // Since we set the "cascade-popdown" property to FALSE, we must popdown the
    // toplevel menu/popover, so that all submenus are closed.
    vim_menu_close_all(menu);
    જો (self->func != ખાલી)
	self->func(self, VIM_MENU_ITEM_CLICKED, self->func_udata);
}

    સ્થિર gboolean
vim_menu_item_mnemonic_activate_cb(
	VimMenuItem *self,
	gboolean    group UNUSED,
	VimMenu	    *menu)
{
    vim_menu_set_active_item(menu, self, FALSE);
    // Return FALSE so that menu item emits "clicked" signal
    પાછા FALSE;
}

    સ્થિર શૂન્ય
vim_menu_item_enter_cb(
	GtkEventController  *controller,
	દ્વિગુણ		    x UNUSED,
	દ્વિગુણ		    y UNUSED,
	VimMenu		    *menu)
{
    VimMenuItem  *self;

    જો (menu->ignore_hover || !gtk_event_controller_motion_contains_pointer(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	પાછા;

    self = VIM_MENU_ITEM(gtk_event_controller_get_widget(controller));
    vim_menu_set_active_item(menu, self, TRUE);
}

    સ્થિર શૂન્ય
vim_menu_item_leave_cb(GtkEventController *controller, VimMenu *menu)
{
    VimMenuItem  *self;

    જો (gtk_event_controller_motion_contains_pointer(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	પાછા;

    self = VIM_MENU_ITEM(gtk_event_controller_get_widget(controller));
    જો (menu->active_item == GTK_WIDGET(self))
	vim_menu_set_active_item(menu, ખાલી, FALSE);
}

/*
 * Insert the menu item at the given index in the menu. If "idx" is negative,
 * then append the menu item.
 */
    શૂન્ય
vim_menu_ઉમેરો_item(VimMenu *self, VimMenuItem *item, પૂર્ણાંક idx)
{
    GtkEventController *controller;

    vim_menu_ઉમેરો(self, GTK_WIDGET(item), idx);

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
vim_menu_ઉમેરો_separator(VimMenu *self, પૂર્ણાંક idx)
{
    પાછા vim_menu_ઉમેરો(self,
	    gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), idx);
}

/*
 * Remove the menu item or separator from the menu
 */
    શૂન્ય
vim_menu_remove(VimMenu *self, GtkWidget *item)
{
    self->વસ્તુઓ = g_list_remove(self->વસ્તુઓ, item);
    gtk_box_remove(GTK_BOX(self->box), item);
}

/*
 * Create a deep copy of the menu
 */
    GtkWidget *
vim_menu_copy(VimMenu *self)
{
    GtkWidget	*copy = vim_menu_new();
    પૂર્ણાંક		i = 0;

    માટે (GList *l = self->વસ્તુઓ; l != ખાલી; l = l->next, i++)
    {
	VimMenuItem *item;
	GtkWidget   *item_copy;

	જો (!VIM_IS_MENU_ITEM(l->data))
	{
	    પ્રતિજ્ઞા(GTK_IS_SEPARATOR(l->data));
	    vim_menu_ઉમેરો_separator(VIM_MENU(copy), i);
	    ચાલુ;
	}

	item = l->data;
	item_copy = vim_menu_item_copy(item);

	vim_menu_ઉમેરો_item(VIM_MENU(copy), VIM_MENU_ITEM(item_copy), i);
    }
    પાછા copy;
}

#endif // FEAT_MENU
