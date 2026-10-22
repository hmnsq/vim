/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#അന്തര്ഭാവ "vim.h"

#ifdef FEAT_TOOLBAR

#അന്തര്ഭാവ <gtk/gtk.h>
#അന്തര്ഭാവ "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
ഘടന _VimToolbar
{
    GtkWidget parent;

    പൂർണ്ണസംഖ്യ style; // TOOLBAR_* flags
    പൂർണ്ണസംഖ്യ iconsize; // TBIS_* values

    GList *ഇനങ്ങൾ;

    GtkWidget	*വേര്;
    GtkWidget	*strip;

    GtkWidget	*overflow_btn;
    GtkWidget	*overflow_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

സ്ഥിതം ശൂന്യം vim_toolbar_size_allocate(GtkWidget *widget, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ height, പൂർണ്ണസംഖ്യ baseline);
സ്ഥിതം ശൂന്യം vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, പൂർണ്ണസംഖ്യ for_size, പൂർണ്ണസംഖ്യ *minimum, പൂർണ്ണസംഖ്യ *natural, പൂർണ്ണസംഖ്യ *minimum_baseline, പൂർണ്ണസംഖ്യ *natural_baseline);

    സ്ഥിതം ശൂന്യം
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->ഇനങ്ങൾ, g_object_unref);
    g_clear_pointer(&self->വേര്, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_parent_class)->dispose(object);
}

    സ്ഥിതം ശൂന്യം
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->size_allocate = vim_toolbar_size_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_set_css_name(widget_class, "toolbar");
}

    സ്ഥിതം ശൂന്യം
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->വേര് = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_parent(self->വേര്, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(self->strip, TRUE);
    gtk_widget_set_halign(self->strip, GTK_ALIGN_START);
    gtk_widget_set_overflow(self->strip, GTK_OVERFLOW_HIDDEN);
    gtk_box_append(GTK_BOX(self->വേര്), self->strip);

    self->overflow_btn = gtk_menu_button_new();
    gtk_widget_set_hexpand(self->overflow_btn, FALSE);
    gtk_widget_set_halign(self->overflow_btn, GTK_ALIGN_END);
    gtk_widget_add_css_class(self->overflow_btn, "flat");
    gtk_box_append(GTK_BOX(self->വേര്), self->overflow_btn);
    g_object_set_data(G_OBJECT(self->overflow_btn),
	    "toolbar-width", GINT_TO_POINTER(-1));

    popover = gtk_popover_new();
    self->overflow_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_popover_set_child(GTK_POPOVER(popover), self->overflow_box);
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), FALSE);
    // Make sure popover aligns to top right of button
    gtk_widget_set_halign(popover, GTK_ALIGN_END);

    gtk_menu_button_set_popover(GTK_MENU_BUTTON(self->overflow_btn), popover);
}

    GtkWidget *
vim_toolbar_new(ശൂന്യം)
{
    തിരികെ g_object_new(VIM_TYPE_TOOLBAR, ശൂന്യ);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    സ്ഥിതം ശൂന്യം
vim_toolbar_തിരുകുക(VimToolbar *self, GtkWidget *widget, പൂർണ്ണസംഖ്യ idx)
{
    GtkWidget	*cur_child = gtk_widget_get_first_child(self->strip);
    പൂർണ്ണസംഖ്യ		i = 0;

    ആകുമ്പോൾ (cur_child != ശൂന്യ && i != idx - 1)
    {
	cur_child = gtk_widget_get_next_sibling(cur_child);
	i++;
    }

    gtk_box_തിരുകുക_child_after(GTK_BOX(self->strip), widget, cur_child);
    self->ഇനങ്ങൾ = g_list_തിരുകുക(self->ഇനങ്ങൾ, g_object_ref(widget), idx);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    സ്ഥിതം ശൂന്യം
set_button_style(GtkWidget *btn, പൂർണ്ണസംഖ്യ style, പൂർണ്ണസംഖ്യ iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    എങ്കിൽ (GTK_IS_SEPARATOR(btn))
	തിരികെ;

    box = gtk_button_get_child(GTK_BUTTON(btn));
    എങ്കിൽ (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_set_spacing(GTK_BOX(box), 8);
    }
    അല്ലെങ്കിൽ
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_set_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_set_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_data(G_OBJECT(btn), "icon");
    എങ്കിൽ (icon != ശൂന്യ)
    {
	എങ്കിൽ (style & TOOLBAR_ICONS)
	{
	    GtkIconSize size;

	    തിരഞ്ഞെടുപ്പ് (iconsize)
	    {
		അവസ്ഥ TBIS_TINY:
		അവസ്ഥ TBIS_SMALL:
		അവസ്ഥ TBIS_MEDIUM:
		    size = GTK_ICON_SIZE_NORMAL;
		    നിർത്തുക;
		അവസ്ഥ TBIS_LARGE:
		അവസ്ഥ TBIS_HUGE:
		അവസ്ഥ TBIS_GIANT:
		    size = GTK_ICON_SIZE_LARGE;
		    നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    size = GTK_ICON_SIZE_NORMAL;
		    നിർത്തുക;
	    }
	    gtk_image_set_icon_size(GTK_IMAGE(icon), size);
	    gtk_widget_set_visible(icon, TRUE);
	}
	അല്ലെങ്കിൽ
	    gtk_widget_set_visible(icon, FALSE);
    }

    label = g_object_get_data(G_OBJECT(btn), "label");
    എങ്കിൽ (label != ശൂന്യ)
	gtk_widget_set_visible(label, style & TOOLBAR_TEXT);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    എങ്കിൽ (invalidate)
	g_object_set_data(G_OBJECT(btn), "toolbar-width", GINT_TO_POINTER(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    സ്ഥിതം ശൂന്യം
button_clicked(GtkWidget *widget UNUSED, VimToolbar *self)
{
    gtk_menu_button_popdown(GTK_MENU_BUTTON(self->overflow_btn));
    // Bring focus to drawarea, popping overflow menu down does not change
    // focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_mch_flush();
}

/*
 * Add a new toolbar button at the given index and return the widget. "icon" and
 * "text" may be NULL..
 */
    GtkWidget *
vim_toolbar_തിരുകുക_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	സ്ഥിരം അക്ഷരം  *text,
	പൂർണ്ണസംഖ്യ	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_add_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_set_child(GTK_BUTTON(btn), box);

    എങ്കിൽ (icon != ശൂന്യ)
    {
	gtk_widget_set_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_set_data(G_OBJECT(btn), "icon", icon);
    }
    എങ്കിൽ (text != ശൂന്യ)
    {
	GtkWidget *label = gtk_label_new(text);

	gtk_widget_set_valign (label, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand (label, TRUE);
	gtk_box_append(GTK_BOX(box), label);
	g_object_set_data(G_OBJECT(btn), "label", label);
    }

    set_button_style(btn, self->style, self->iconsize, TRUE);

    g_signal_connect_object(btn, "clicked",
	    G_CALLBACK(button_clicked), self, G_CONNECT_DEFAULT);

    vim_toolbar_തിരുകുക(self, btn, idx);
    തിരികെ btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_തിരുകുക_separator(VimToolbar *self, പൂർണ്ണസംഖ്യ idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_set_data(G_OBJECT(sep), "toolbar-width", GINT_TO_POINTER(-1));
    vim_toolbar_തിരുകുക(self, sep, idx);
    തിരികെ sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    ശൂന്യം
vim_toolbar_set_style(VimToolbar *self, പൂർണ്ണസംഖ്യ style, പൂർണ്ണസംഖ്യ iconsize)
{
    GtkWidget *cur_child = gtk_widget_get_first_child(self->strip);

    എങ്കിൽ (style == self->style && self->iconsize == iconsize)
	തിരികെ;

    self->style = style;
    self->iconsize = iconsize;

    ആകുമ്പോൾ (cur_child != ശൂന്യ)
    {
	എങ്കിൽ (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }

    cur_child = gtk_widget_get_first_child(self->overflow_box);
    ആകുമ്പോൾ (cur_child != ശൂന്യ)
    {
	എങ്കിൽ (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }
    // Sizes may have changed
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    ശൂന്യം
vim_toolbar_remove(VimToolbar *self, GtkWidget *item)
{
    എങ്കിൽ (gtk_widget_get_parent(item) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), item);
    അല്ലെങ്കിൽ
	gtk_box_remove(GTK_BOX(self->overflow_box), item);

    self->ഇനങ്ങൾ = g_list_remove(self->ഇനങ്ങൾ, item);
    g_object_unref(item);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    സ്ഥിതം ശൂന്യം
vim_toolbar_move_item_to(
	VimToolbar  *self,
	GtkWidget   *item,
	gboolean    overflow)
{
    GtkBox *from, *to;

    to = overflow ? GTK_BOX(self->overflow_box) : GTK_BOX(self->strip);
    എങ്കിൽ (gtk_widget_get_parent(item) == GTK_WIDGET(to))
	തിരികെ;

    from = overflow ? GTK_BOX(self->strip) : GTK_BOX(self->overflow_box);

    g_object_ref(item);
    gtk_box_remove(from, item);
    gtk_box_append(to, item);
    g_object_unref(item);

    എങ്കിൽ (GTK_IS_SEPARATOR(item))
	gtk_widget_set_visible(item, !overflow);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (overflow)
	{
	    പൂർണ്ണസംഖ്യ style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_TEXT;
	    style |= TOOLBAR_ICONS;
	    set_button_style(item, style, self->iconsize, FALSE);
	}
	അല്ലെങ്കിൽ
	    set_button_style(item, self->style, self->iconsize, FALSE);
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_item_width(GtkWidget *item)
{
    പൂർണ്ണസംഖ്യ toolbar_width = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_width = GPOINTER_TO_INT(
	    g_object_get_data(G_OBJECT(item), "toolbar-width"));

    എങ്കിൽ (toolbar_width == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_size(item, &min_req, &nat_req);
	toolbar_width = MAX(min_req.width, nat_req.width);

	// Save width for later
	g_object_set_data(G_OBJECT(item),
		"toolbar-width", GINT_TO_POINTER(toolbar_width));
    }
    തിരികെ toolbar_width;
}

    സ്ഥിതം ശൂന്യം
vim_toolbar_size_allocate(
	GtkWidget   *widget,
	പൂർണ്ണസംഖ്യ	    width,
	പൂർണ്ണസംഖ്യ	    height,
	പൂർണ്ണസംഖ്യ	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    GtkAllocation   alloc;
    പൂർണ്ണസംഖ്യ		    used = 0;
    പൂർണ്ണസംഖ്യ		    avail;
    GList	    *overflow_ele = ശൂന്യ;
    GtkWidget	    *child;

    avail = width - get_item_width(self->overflow_btn);

    വേണ്ടി (GList *ele = self->ഇനങ്ങൾ; ele != ശൂന്യ; ele = ele->next)
    {
	GtkWidget   *item = ele->data;
	പൂർണ്ണസംഖ്യ	    toolbar_width = get_item_width(item);

	എങ്കിൽ (used + toolbar_width > avail)
	{
	    overflow_ele = ele;
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ
	    vim_toolbar_move_item_to(self, item, FALSE);
	used += toolbar_width;
    }

    എങ്കിൽ (overflow_ele != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	    remaining = width - used;
	gboolean    need_overflow_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	ആകുമ്പോൾ ((child = gtk_widget_get_first_child(self->overflow_box))
		!= ശൂന്യ)
	    vim_toolbar_move_item_to(self, child, FALSE);

	വേണ്ടി (GList *ele = overflow_ele; ele != ശൂന്യ; ele = ele->next)
	{
	    remaining -= get_item_width(ele->data);
	    എങ്കിൽ (remaining < 0)
	    {
		need_overflow_btn = TRUE;
		നിർത്തുക;
	    }
	}

	എങ്കിൽ (need_overflow_btn)
	    വേണ്ടി (; overflow_ele != ശൂന്യ; overflow_ele = overflow_ele->next)
		vim_toolbar_move_item_to(self, overflow_ele->data, TRUE);
	gtk_widget_set_visible(self->overflow_btn, need_overflow_btn);
    }
    അല്ലെങ്കിൽ
	gtk_widget_set_visible(self->overflow_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.width = width;
    alloc.height = height;

    gtk_widget_size_allocate(self->വേര്, &alloc, baseline);
}

    സ്ഥിതം ശൂന്യം
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	പൂർണ്ണസംഖ്യ		for_size,
	പൂർണ്ണസംഖ്യ		*min,
	പൂർണ്ണസംഖ്യ		*nat,
	പൂർണ്ണസംഖ്യ		*min_baseline,
	പൂർണ്ണസംഖ്യ		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    പൂർണ്ണസംഖ്യ		strip_min = 0, strip_nat = 0;
    പൂർണ്ണസംഖ്യ		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_size,
	    &strip_min, &strip_nat, ശൂന്യ, ശൂന്യ);
    gtk_widget_measure(self->overflow_btn, orientation, for_size,
	    &obtn_min, &obtn_nat, ശൂന്യ, ശൂന്യ);

    എങ്കിൽ (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	എങ്കിൽ (min != ശൂന്യ)
	    *min = obtn_min;
	എങ്കിൽ (nat != ശൂന്യ)
	    *nat = strip_nat + obtn_nat;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (min != ശൂന്യ)
	    *min = MAX(strip_min, obtn_min);
	എങ്കിൽ (nat != ശൂന്യ)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    എങ്കിൽ (min_baseline != ശൂന്യ)
	*min_baseline = -1;
    എങ്കിൽ (nat_baseline != ശൂന്യ)
	*nat_baseline = -1;
}

#endif // FEAT_TOOLBAR
