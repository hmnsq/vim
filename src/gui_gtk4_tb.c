/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#அந்தர்பாவ "vim.h"

#ifdef FEAT_TOOLBAR

#அந்தர்பாவ <gtk/gtk.h>
#அந்தர்பாவ "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
கட்டமைப்பு _VimToolbar
{
    GtkWidget parent;

    முழு style; // TOOLBAR_* flags
    முழு iconsize; // TBIS_* values

    GList *பொருட்கள்;

    GtkWidget	*வேர்;
    GtkWidget	*strip;

    GtkWidget	*overflow_btn;
    GtkWidget	*overflow_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

நிலையான வெறுமை vim_toolbar_size_allocate(GtkWidget *widget, முழு width, முழு height, முழு baseline);
நிலையான வெறுமை vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, முழு for_size, முழு *minimum, முழு *natural, முழு *minimum_baseline, முழு *natural_baseline);

    நிலையான வெறுமை
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->பொருட்கள், g_object_unref);
    g_clear_pointer(&self->வேர், gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_parent_class)->dispose(object);
}

    நிலையான வெறுமை
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->size_allocate = vim_toolbar_size_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_set_css_name(widget_class, "toolbar");
}

    நிலையான வெறுமை
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->வேர் = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_parent(self->வேர், GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(self->strip, TRUE);
    gtk_widget_set_halign(self->strip, GTK_ALIGN_START);
    gtk_widget_set_overflow(self->strip, GTK_OVERFLOW_HIDDEN);
    gtk_box_append(GTK_BOX(self->வேர்), self->strip);

    self->overflow_btn = gtk_menu_button_new();
    gtk_widget_set_hexpand(self->overflow_btn, FALSE);
    gtk_widget_set_halign(self->overflow_btn, GTK_ALIGN_END);
    gtk_widget_add_css_class(self->overflow_btn, "flat");
    gtk_box_append(GTK_BOX(self->வேர்), self->overflow_btn);
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
vim_toolbar_new(வெறுமை)
{
    திரும்பு g_object_new(VIM_TYPE_TOOLBAR, வெற்று);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    நிலையான வெறுமை
vim_toolbar_செருகு(VimToolbar *self, GtkWidget *widget, முழு idx)
{
    GtkWidget	*cur_child = gtk_widget_get_first_child(self->strip);
    முழு		i = 0;

    போது (cur_child != வெற்று && i != idx - 1)
    {
	cur_child = gtk_widget_get_next_sibling(cur_child);
	i++;
    }

    gtk_box_செருகு_child_after(GTK_BOX(self->strip), widget, cur_child);
    self->பொருட்கள் = g_list_செருகு(self->பொருட்கள், g_object_ref(widget), idx);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    நிலையான வெறுமை
set_button_style(GtkWidget *btn, முழு style, முழு iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    எனில் (GTK_IS_SEPARATOR(btn))
	திரும்பு;

    box = gtk_button_get_child(GTK_BUTTON(btn));
    எனில் (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_set_spacing(GTK_BOX(box), 8);
    }
    இல்லையெனில்
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_set_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_set_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_data(G_OBJECT(btn), "icon");
    எனில் (icon != வெற்று)
    {
	எனில் (style & TOOLBAR_ICONS)
	{
	    GtkIconSize size;

	    தேர்வு (iconsize)
	    {
		நிலை TBIS_TINY:
		நிலை TBIS_SMALL:
		நிலை TBIS_MEDIUM:
		    size = GTK_ICON_SIZE_NORMAL;
		    நிறுத்து;
		நிலை TBIS_LARGE:
		நிலை TBIS_HUGE:
		நிலை TBIS_GIANT:
		    size = GTK_ICON_SIZE_LARGE;
		    நிறுத்து;
		இயல்பு:
		    size = GTK_ICON_SIZE_NORMAL;
		    நிறுத்து;
	    }
	    gtk_image_set_icon_size(GTK_IMAGE(icon), size);
	    gtk_widget_set_visible(icon, TRUE);
	}
	இல்லையெனில்
	    gtk_widget_set_visible(icon, FALSE);
    }

    label = g_object_get_data(G_OBJECT(btn), "label");
    எனில் (label != வெற்று)
	gtk_widget_set_visible(label, style & TOOLBAR_TEXT);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    எனில் (invalidate)
	g_object_set_data(G_OBJECT(btn), "toolbar-width", GINT_TO_POINTER(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    நிலையான வெறுமை
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
vim_toolbar_செருகு_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	மாறிலி எழுத்து  *text,
	முழு	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_add_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_set_child(GTK_BUTTON(btn), box);

    எனில் (icon != வெற்று)
    {
	gtk_widget_set_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_set_data(G_OBJECT(btn), "icon", icon);
    }
    எனில் (text != வெற்று)
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

    vim_toolbar_செருகு(self, btn, idx);
    திரும்பு btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_செருகு_separator(VimToolbar *self, முழு idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_set_data(G_OBJECT(sep), "toolbar-width", GINT_TO_POINTER(-1));
    vim_toolbar_செருகு(self, sep, idx);
    திரும்பு sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    வெறுமை
vim_toolbar_set_style(VimToolbar *self, முழு style, முழு iconsize)
{
    GtkWidget *cur_child = gtk_widget_get_first_child(self->strip);

    எனில் (style == self->style && self->iconsize == iconsize)
	திரும்பு;

    self->style = style;
    self->iconsize = iconsize;

    போது (cur_child != வெற்று)
    {
	எனில் (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }

    cur_child = gtk_widget_get_first_child(self->overflow_box);
    போது (cur_child != வெற்று)
    {
	எனில் (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }
    // Sizes may have changed
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    வெறுமை
vim_toolbar_remove(VimToolbar *self, GtkWidget *item)
{
    எனில் (gtk_widget_get_parent(item) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), item);
    இல்லையெனில்
	gtk_box_remove(GTK_BOX(self->overflow_box), item);

    self->பொருட்கள் = g_list_remove(self->பொருட்கள், item);
    g_object_unref(item);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    நிலையான வெறுமை
vim_toolbar_move_item_to(
	VimToolbar  *self,
	GtkWidget   *item,
	gboolean    overflow)
{
    GtkBox *from, *to;

    to = overflow ? GTK_BOX(self->overflow_box) : GTK_BOX(self->strip);
    எனில் (gtk_widget_get_parent(item) == GTK_WIDGET(to))
	திரும்பு;

    from = overflow ? GTK_BOX(self->strip) : GTK_BOX(self->overflow_box);

    g_object_ref(item);
    gtk_box_remove(from, item);
    gtk_box_append(to, item);
    g_object_unref(item);

    எனில் (GTK_IS_SEPARATOR(item))
	gtk_widget_set_visible(item, !overflow);
    இல்லையெனில்
    {
	எனில் (overflow)
	{
	    முழு style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_TEXT;
	    style |= TOOLBAR_ICONS;
	    set_button_style(item, style, self->iconsize, FALSE);
	}
	இல்லையெனில்
	    set_button_style(item, self->style, self->iconsize, FALSE);
    }
}

    நிலையான முழு
get_item_width(GtkWidget *item)
{
    முழு toolbar_width = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_width = GPOINTER_TO_INT(
	    g_object_get_data(G_OBJECT(item), "toolbar-width"));

    எனில் (toolbar_width == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_size(item, &min_req, &nat_req);
	toolbar_width = MAX(min_req.width, nat_req.width);

	// Save width for later
	g_object_set_data(G_OBJECT(item),
		"toolbar-width", GINT_TO_POINTER(toolbar_width));
    }
    திரும்பு toolbar_width;
}

    நிலையான வெறுமை
vim_toolbar_size_allocate(
	GtkWidget   *widget,
	முழு	    width,
	முழு	    height,
	முழு	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    GtkAllocation   alloc;
    முழு		    used = 0;
    முழு		    avail;
    GList	    *overflow_ele = வெற்று;
    GtkWidget	    *child;

    avail = width - get_item_width(self->overflow_btn);

    க்கு (GList *ele = self->பொருட்கள்; ele != வெற்று; ele = ele->next)
    {
	GtkWidget   *item = ele->data;
	முழு	    toolbar_width = get_item_width(item);

	எனில் (used + toolbar_width > avail)
	{
	    overflow_ele = ele;
	    நிறுத்து;
	}
	இல்லையெனில்
	    vim_toolbar_move_item_to(self, item, FALSE);
	used += toolbar_width;
    }

    எனில் (overflow_ele != வெற்று)
    {
	முழு	    remaining = width - used;
	gboolean    need_overflow_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	போது ((child = gtk_widget_get_first_child(self->overflow_box))
		!= வெற்று)
	    vim_toolbar_move_item_to(self, child, FALSE);

	க்கு (GList *ele = overflow_ele; ele != வெற்று; ele = ele->next)
	{
	    remaining -= get_item_width(ele->data);
	    எனில் (remaining < 0)
	    {
		need_overflow_btn = TRUE;
		நிறுத்து;
	    }
	}

	எனில் (need_overflow_btn)
	    க்கு (; overflow_ele != வெற்று; overflow_ele = overflow_ele->next)
		vim_toolbar_move_item_to(self, overflow_ele->data, TRUE);
	gtk_widget_set_visible(self->overflow_btn, need_overflow_btn);
    }
    இல்லையெனில்
	gtk_widget_set_visible(self->overflow_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.width = width;
    alloc.height = height;

    gtk_widget_size_allocate(self->வேர், &alloc, baseline);
}

    நிலையான வெறுமை
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	முழு		for_size,
	முழு		*min,
	முழு		*nat,
	முழு		*min_baseline,
	முழு		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    முழு		strip_min = 0, strip_nat = 0;
    முழு		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_size,
	    &strip_min, &strip_nat, வெற்று, வெற்று);
    gtk_widget_measure(self->overflow_btn, orientation, for_size,
	    &obtn_min, &obtn_nat, வெற்று, வெற்று);

    எனில் (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	எனில் (min != வெற்று)
	    *min = obtn_min;
	எனில் (nat != வெற்று)
	    *nat = strip_nat + obtn_nat;
    }
    இல்லையெனில்
    {
	எனில் (min != வெற்று)
	    *min = MAX(strip_min, obtn_min);
	எனில் (nat != வெற்று)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    எனில் (min_baseline != வெற்று)
	*min_baseline = -1;
    எனில் (nat_baseline != வெற்று)
	*nat_baseline = -1;
}

#endif // FEAT_TOOLBAR
