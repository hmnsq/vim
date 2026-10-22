/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#অন্তর্ভাব "vim.h"

#ifdef FEAT_TOOLBAR

#অন্তর্ভাব <gtk/gtk.h>
#অন্তর্ভাব "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
সংরচনা _VimToolbar
{
    GtkWidget parent;

    পূর্ণাঙ্ক style; // TOOLBAR_* flags
    পূর্ণাঙ্ক iconsize; // TBIS_* values

    GList *বস্তুসমূহ;

    GtkWidget	*মূল;
    GtkWidget	*strip;

    GtkWidget	*overflow_btn;
    GtkWidget	*overflow_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

স্থির শূন্য vim_toolbar_size_allocate(GtkWidget *widget, পূর্ণাঙ্ক width, পূর্ণাঙ্ক height, পূর্ণাঙ্ক baseline);
স্থির শূন্য vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, পূর্ণাঙ্ক for_size, পূর্ণাঙ্ক *minimum, পূর্ণাঙ্ক *natural, পূর্ণাঙ্ক *minimum_baseline, পূর্ণাঙ্ক *natural_baseline);

    স্থির শূন্য
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->বস্তুসমূহ, g_object_unref);
    g_clear_pointer(&self->মূল, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_parent_class)->dispose(object);
}

    স্থির শূন্য
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->size_allocate = vim_toolbar_size_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_set_css_name(widget_class, "toolbar");
}

    স্থির শূন্য
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->মূল = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_parent(self->মূল, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(self->strip, TRUE);
    gtk_widget_set_halign(self->strip, GTK_ALIGN_START);
    gtk_widget_set_overflow(self->strip, GTK_OVERFLOW_HIDDEN);
    gtk_box_append(GTK_BOX(self->মূল), self->strip);

    self->overflow_btn = gtk_menu_button_new();
    gtk_widget_set_hexpand(self->overflow_btn, FALSE);
    gtk_widget_set_halign(self->overflow_btn, GTK_ALIGN_END);
    gtk_widget_add_css_class(self->overflow_btn, "flat");
    gtk_box_append(GTK_BOX(self->মূল), self->overflow_btn);
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
vim_toolbar_new(শূন্য)
{
    প্রত্যাগম g_object_new(VIM_TYPE_TOOLBAR, রিক্ত);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    স্থির শূন্য
vim_toolbar_ঢোকাও(VimToolbar *self, GtkWidget *widget, পূর্ণাঙ্ক idx)
{
    GtkWidget	*cur_child = gtk_widget_get_first_child(self->strip);
    পূর্ণাঙ্ক		i = 0;

    যাবত্ (cur_child != রিক্ত && i != idx - 1)
    {
	cur_child = gtk_widget_get_next_sibling(cur_child);
	i++;
    }

    gtk_box_ঢোকাও_child_after(GTK_BOX(self->strip), widget, cur_child);
    self->বস্তুসমূহ = g_list_ঢোকাও(self->বস্তুসমূহ, g_object_ref(widget), idx);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    স্থির শূন্য
set_button_style(GtkWidget *btn, পূর্ণাঙ্ক style, পূর্ণাঙ্ক iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    যদি (GTK_IS_SEPARATOR(btn))
	প্রত্যাগম;

    box = gtk_button_get_child(GTK_BUTTON(btn));
    যদি (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_set_spacing(GTK_BOX(box), 8);
    }
    অন্যথা
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_set_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_set_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_data(G_OBJECT(btn), "icon");
    যদি (icon != রিক্ত)
    {
	যদি (style & TOOLBAR_ICONS)
	{
	    GtkIconSize size;

	    বিকল্প (iconsize)
	    {
		পক্ষ TBIS_TINY:
		পক্ষ TBIS_SMALL:
		পক্ষ TBIS_MEDIUM:
		    size = GTK_ICON_SIZE_NORMAL;
		    ভঙ্গ;
		পক্ষ TBIS_LARGE:
		পক্ষ TBIS_HUGE:
		পক্ষ TBIS_GIANT:
		    size = GTK_ICON_SIZE_LARGE;
		    ভঙ্গ;
		স্বভাব:
		    size = GTK_ICON_SIZE_NORMAL;
		    ভঙ্গ;
	    }
	    gtk_image_set_icon_size(GTK_IMAGE(icon), size);
	    gtk_widget_set_visible(icon, TRUE);
	}
	অন্যথা
	    gtk_widget_set_visible(icon, FALSE);
    }

    label = g_object_get_data(G_OBJECT(btn), "label");
    যদি (label != রিক্ত)
	gtk_widget_set_visible(label, style & TOOLBAR_TEXT);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    যদি (invalidate)
	g_object_set_data(G_OBJECT(btn), "toolbar-width", GINT_TO_POINTER(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    স্থির শূন্য
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
vim_toolbar_ঢোকাও_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	ধ্রুব বর্ণ  *text,
	পূর্ণাঙ্ক	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_add_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_set_child(GTK_BUTTON(btn), box);

    যদি (icon != রিক্ত)
    {
	gtk_widget_set_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_set_data(G_OBJECT(btn), "icon", icon);
    }
    যদি (text != রিক্ত)
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

    vim_toolbar_ঢোকাও(self, btn, idx);
    প্রত্যাগম btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_ঢোকাও_separator(VimToolbar *self, পূর্ণাঙ্ক idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_set_data(G_OBJECT(sep), "toolbar-width", GINT_TO_POINTER(-1));
    vim_toolbar_ঢোকাও(self, sep, idx);
    প্রত্যাগম sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    শূন্য
vim_toolbar_set_style(VimToolbar *self, পূর্ণাঙ্ক style, পূর্ণাঙ্ক iconsize)
{
    GtkWidget *cur_child = gtk_widget_get_first_child(self->strip);

    যদি (style == self->style && self->iconsize == iconsize)
	প্রত্যাগম;

    self->style = style;
    self->iconsize = iconsize;

    যাবত্ (cur_child != রিক্ত)
    {
	যদি (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }

    cur_child = gtk_widget_get_first_child(self->overflow_box);
    যাবত্ (cur_child != রিক্ত)
    {
	যদি (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }
    // Sizes may have changed
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    শূন্য
vim_toolbar_remove(VimToolbar *self, GtkWidget *item)
{
    যদি (gtk_widget_get_parent(item) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), item);
    অন্যথা
	gtk_box_remove(GTK_BOX(self->overflow_box), item);

    self->বস্তুসমূহ = g_list_remove(self->বস্তুসমূহ, item);
    g_object_unref(item);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    স্থির শূন্য
vim_toolbar_move_item_to(
	VimToolbar  *self,
	GtkWidget   *item,
	gboolean    overflow)
{
    GtkBox *from, *to;

    to = overflow ? GTK_BOX(self->overflow_box) : GTK_BOX(self->strip);
    যদি (gtk_widget_get_parent(item) == GTK_WIDGET(to))
	প্রত্যাগম;

    from = overflow ? GTK_BOX(self->strip) : GTK_BOX(self->overflow_box);

    g_object_ref(item);
    gtk_box_remove(from, item);
    gtk_box_append(to, item);
    g_object_unref(item);

    যদি (GTK_IS_SEPARATOR(item))
	gtk_widget_set_visible(item, !overflow);
    অন্যথা
    {
	যদি (overflow)
	{
	    পূর্ণাঙ্ক style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_TEXT;
	    style |= TOOLBAR_ICONS;
	    set_button_style(item, style, self->iconsize, FALSE);
	}
	অন্যথা
	    set_button_style(item, self->style, self->iconsize, FALSE);
    }
}

    স্থির পূর্ণাঙ্ক
get_item_width(GtkWidget *item)
{
    পূর্ণাঙ্ক toolbar_width = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_width = GPOINTER_TO_INT(
	    g_object_get_data(G_OBJECT(item), "toolbar-width"));

    যদি (toolbar_width == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_size(item, &min_req, &nat_req);
	toolbar_width = MAX(min_req.width, nat_req.width);

	// Save width for later
	g_object_set_data(G_OBJECT(item),
		"toolbar-width", GINT_TO_POINTER(toolbar_width));
    }
    প্রত্যাগম toolbar_width;
}

    স্থির শূন্য
vim_toolbar_size_allocate(
	GtkWidget   *widget,
	পূর্ণাঙ্ক	    width,
	পূর্ণাঙ্ক	    height,
	পূর্ণাঙ্ক	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    GtkAllocation   alloc;
    পূর্ণাঙ্ক		    used = 0;
    পূর্ণাঙ্ক		    avail;
    GList	    *overflow_ele = রিক্ত;
    GtkWidget	    *child;

    avail = width - get_item_width(self->overflow_btn);

    প্রতি (GList *ele = self->বস্তুসমূহ; ele != রিক্ত; ele = ele->next)
    {
	GtkWidget   *item = ele->data;
	পূর্ণাঙ্ক	    toolbar_width = get_item_width(item);

	যদি (used + toolbar_width > avail)
	{
	    overflow_ele = ele;
	    ভঙ্গ;
	}
	অন্যথা
	    vim_toolbar_move_item_to(self, item, FALSE);
	used += toolbar_width;
    }

    যদি (overflow_ele != রিক্ত)
    {
	পূর্ণাঙ্ক	    remaining = width - used;
	gboolean    need_overflow_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	যাবত্ ((child = gtk_widget_get_first_child(self->overflow_box))
		!= রিক্ত)
	    vim_toolbar_move_item_to(self, child, FALSE);

	প্রতি (GList *ele = overflow_ele; ele != রিক্ত; ele = ele->next)
	{
	    remaining -= get_item_width(ele->data);
	    যদি (remaining < 0)
	    {
		need_overflow_btn = TRUE;
		ভঙ্গ;
	    }
	}

	যদি (need_overflow_btn)
	    প্রতি (; overflow_ele != রিক্ত; overflow_ele = overflow_ele->next)
		vim_toolbar_move_item_to(self, overflow_ele->data, TRUE);
	gtk_widget_set_visible(self->overflow_btn, need_overflow_btn);
    }
    অন্যথা
	gtk_widget_set_visible(self->overflow_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.width = width;
    alloc.height = height;

    gtk_widget_size_allocate(self->মূল, &alloc, baseline);
}

    স্থির শূন্য
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	পূর্ণাঙ্ক		for_size,
	পূর্ণাঙ্ক		*min,
	পূর্ণাঙ্ক		*nat,
	পূর্ণাঙ্ক		*min_baseline,
	পূর্ণাঙ্ক		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    পূর্ণাঙ্ক		strip_min = 0, strip_nat = 0;
    পূর্ণাঙ্ক		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_size,
	    &strip_min, &strip_nat, রিক্ত, রিক্ত);
    gtk_widget_measure(self->overflow_btn, orientation, for_size,
	    &obtn_min, &obtn_nat, রিক্ত, রিক্ত);

    যদি (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	যদি (min != রিক্ত)
	    *min = obtn_min;
	যদি (nat != রিক্ত)
	    *nat = strip_nat + obtn_nat;
    }
    অন্যথা
    {
	যদি (min != রিক্ত)
	    *min = MAX(strip_min, obtn_min);
	যদি (nat != রিক্ত)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    যদি (min_baseline != রিক্ত)
	*min_baseline = -1;
    যদি (nat_baseline != রিক্ত)
	*nat_baseline = -1;
}

#endif // FEAT_TOOLBAR
