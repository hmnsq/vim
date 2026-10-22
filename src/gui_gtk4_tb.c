/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ifdef FEAT_TOOLBAR

#ଅନ୍ତର୍ଭାଵ <gtk/gtk.h>
#ଅନ୍ତର୍ଭାଵ "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
ସଂରଚନା _VimToolbar
{
    GtkWidget parent;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା style; // TOOLBAR_* flags
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା iconsize; // TBIS_* values

    GList *ବସ୍ତୁଗୁଡ଼ିକ;

    GtkWidget	*ମୂଳ;
    GtkWidget	*strip;

    GtkWidget	*overflow_btn;
    GtkWidget	*overflow_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

ସ୍ଥିର ଶୂନ୍ୟ vim_toolbar_size_allocate(GtkWidget *widget, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height, ପୂର୍ଣ୍ଣସଂଖ୍ୟା baseline);
ସ୍ଥିର ଶୂନ୍ୟ vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, ପୂର୍ଣ୍ଣସଂଖ୍ୟା for_size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *minimum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *natural, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *minimum_baseline, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *natural_baseline);

    ସ୍ଥିର ଶୂନ୍ୟ
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->ବସ୍ତୁଗୁଡ଼ିକ, g_object_unref);
    g_clear_pointer(&self->ମୂଳ, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_parent_class)->dispose(object);
}

    ସ୍ଥିର ଶୂନ୍ୟ
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->size_allocate = vim_toolbar_size_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_set_css_name(widget_class, "toolbar");
}

    ସ୍ଥିର ଶୂନ୍ୟ
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->ମୂଳ = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_parent(self->ମୂଳ, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(self->strip, TRUE);
    gtk_widget_set_halign(self->strip, GTK_ALIGN_START);
    gtk_widget_set_overflow(self->strip, GTK_OVERFLOW_HIDDEN);
    gtk_box_append(GTK_BOX(self->ମୂଳ), self->strip);

    self->overflow_btn = gtk_menu_button_new();
    gtk_widget_set_hexpand(self->overflow_btn, FALSE);
    gtk_widget_set_halign(self->overflow_btn, GTK_ALIGN_END);
    gtk_widget_add_css_class(self->overflow_btn, "flat");
    gtk_box_append(GTK_BOX(self->ମୂଳ), self->overflow_btn);
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
vim_toolbar_new(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ g_object_new(VIM_TYPE_TOOLBAR, ଖାଲି);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
vim_toolbar_ଭର୍ତ୍ତି(VimToolbar *self, GtkWidget *widget, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    GtkWidget	*cur_child = gtk_widget_get_first_child(self->strip);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;

    ଯେତେବେଳେ (cur_child != ଖାଲି && i != idx - 1)
    {
	cur_child = gtk_widget_get_next_sibling(cur_child);
	i++;
    }

    gtk_box_ଭର୍ତ୍ତି_child_after(GTK_BOX(self->strip), widget, cur_child);
    self->ବସ୍ତୁଗୁଡ଼ିକ = g_list_ଭର୍ତ୍ତି(self->ବସ୍ତୁଗୁଡ଼ିକ, g_object_ref(widget), idx);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_button_style(GtkWidget *btn, ପୂର୍ଣ୍ଣସଂଖ୍ୟା style, ପୂର୍ଣ୍ଣସଂଖ୍ୟା iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    ଯଦି (GTK_IS_SEPARATOR(btn))
	ଫେରନ୍ତୁ;

    box = gtk_button_get_child(GTK_BUTTON(btn));
    ଯଦି (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_set_spacing(GTK_BOX(box), 8);
    }
    ଅନ୍ୟଥା
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_set_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_set_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_data(G_OBJECT(btn), "icon");
    ଯଦି (icon != ଖାଲି)
    {
	ଯଦି (style & TOOLBAR_ICONS)
	{
	    GtkIconSize size;

	    ବାଛ (iconsize)
	    {
		ସ୍ଥିତି TBIS_TINY:
		ସ୍ଥିତି TBIS_SMALL:
		ସ୍ଥିତି TBIS_MEDIUM:
		    size = GTK_ICON_SIZE_NORMAL;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି TBIS_LARGE:
		ସ୍ଥିତି TBIS_HUGE:
		ସ୍ଥିତି TBIS_GIANT:
		    size = GTK_ICON_SIZE_LARGE;
		    ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    size = GTK_ICON_SIZE_NORMAL;
		    ଭାଙ୍ଗ;
	    }
	    gtk_image_set_icon_size(GTK_IMAGE(icon), size);
	    gtk_widget_set_visible(icon, TRUE);
	}
	ଅନ୍ୟଥା
	    gtk_widget_set_visible(icon, FALSE);
    }

    label = g_object_get_data(G_OBJECT(btn), "label");
    ଯଦି (label != ଖାଲି)
	gtk_widget_set_visible(label, style & TOOLBAR_TEXT);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    ଯଦି (invalidate)
	g_object_set_data(G_OBJECT(btn), "toolbar-width", GINT_TO_POINTER(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
vim_toolbar_ଭର୍ତ୍ତି_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	ଅଚଳ ଅକ୍ଷର  *text,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_add_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_set_child(GTK_BUTTON(btn), box);

    ଯଦି (icon != ଖାଲି)
    {
	gtk_widget_set_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_set_data(G_OBJECT(btn), "icon", icon);
    }
    ଯଦି (text != ଖାଲି)
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

    vim_toolbar_ଭର୍ତ୍ତି(self, btn, idx);
    ଫେରନ୍ତୁ btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_ଭର୍ତ୍ତି_separator(VimToolbar *self, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_set_data(G_OBJECT(sep), "toolbar-width", GINT_TO_POINTER(-1));
    vim_toolbar_ଭର୍ତ୍ତି(self, sep, idx);
    ଫେରନ୍ତୁ sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    ଶୂନ୍ୟ
vim_toolbar_set_style(VimToolbar *self, ପୂର୍ଣ୍ଣସଂଖ୍ୟା style, ପୂର୍ଣ୍ଣସଂଖ୍ୟା iconsize)
{
    GtkWidget *cur_child = gtk_widget_get_first_child(self->strip);

    ଯଦି (style == self->style && self->iconsize == iconsize)
	ଫେରନ୍ତୁ;

    self->style = style;
    self->iconsize = iconsize;

    ଯେତେବେଳେ (cur_child != ଖାଲି)
    {
	ଯଦି (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }

    cur_child = gtk_widget_get_first_child(self->overflow_box);
    ଯେତେବେଳେ (cur_child != ଖାଲି)
    {
	ଯଦି (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }
    // Sizes may have changed
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    ଶୂନ୍ୟ
vim_toolbar_remove(VimToolbar *self, GtkWidget *item)
{
    ଯଦି (gtk_widget_get_parent(item) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), item);
    ଅନ୍ୟଥା
	gtk_box_remove(GTK_BOX(self->overflow_box), item);

    self->ବସ୍ତୁଗୁଡ଼ିକ = g_list_remove(self->ବସ୍ତୁଗୁଡ଼ିକ, item);
    g_object_unref(item);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
vim_toolbar_move_item_to(
	VimToolbar  *self,
	GtkWidget   *item,
	gboolean    overflow)
{
    GtkBox *from, *to;

    to = overflow ? GTK_BOX(self->overflow_box) : GTK_BOX(self->strip);
    ଯଦି (gtk_widget_get_parent(item) == GTK_WIDGET(to))
	ଫେରନ୍ତୁ;

    from = overflow ? GTK_BOX(self->strip) : GTK_BOX(self->overflow_box);

    g_object_ref(item);
    gtk_box_remove(from, item);
    gtk_box_append(to, item);
    g_object_unref(item);

    ଯଦି (GTK_IS_SEPARATOR(item))
	gtk_widget_set_visible(item, !overflow);
    ଅନ୍ୟଥା
    {
	ଯଦି (overflow)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_TEXT;
	    style |= TOOLBAR_ICONS;
	    set_button_style(item, style, self->iconsize, FALSE);
	}
	ଅନ୍ୟଥା
	    set_button_style(item, self->style, self->iconsize, FALSE);
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_item_width(GtkWidget *item)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା toolbar_width = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_width = GPOINTER_TO_INT(
	    g_object_get_data(G_OBJECT(item), "toolbar-width"));

    ଯଦି (toolbar_width == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_size(item, &min_req, &nat_req);
	toolbar_width = MAX(min_req.width, nat_req.width);

	// Save width for later
	g_object_set_data(G_OBJECT(item),
		"toolbar-width", GINT_TO_POINTER(toolbar_width));
    }
    ଫେରନ୍ତୁ toolbar_width;
}

    ସ୍ଥିର ଶୂନ୍ୟ
vim_toolbar_size_allocate(
	GtkWidget   *widget,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    height,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    GtkAllocation   alloc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    used = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    avail;
    GList	    *overflow_ele = ଖାଲି;
    GtkWidget	    *child;

    avail = width - get_item_width(self->overflow_btn);

    ପାଇଁ (GList *ele = self->ବସ୍ତୁଗୁଡ଼ିକ; ele != ଖାଲି; ele = ele->next)
    {
	GtkWidget   *item = ele->data;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    toolbar_width = get_item_width(item);

	ଯଦି (used + toolbar_width > avail)
	{
	    overflow_ele = ele;
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା
	    vim_toolbar_move_item_to(self, item, FALSE);
	used += toolbar_width;
    }

    ଯଦି (overflow_ele != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    remaining = width - used;
	gboolean    need_overflow_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	ଯେତେବେଳେ ((child = gtk_widget_get_first_child(self->overflow_box))
		!= ଖାଲି)
	    vim_toolbar_move_item_to(self, child, FALSE);

	ପାଇଁ (GList *ele = overflow_ele; ele != ଖାଲି; ele = ele->next)
	{
	    remaining -= get_item_width(ele->data);
	    ଯଦି (remaining < 0)
	    {
		need_overflow_btn = TRUE;
		ଭାଙ୍ଗ;
	    }
	}

	ଯଦି (need_overflow_btn)
	    ପାଇଁ (; overflow_ele != ଖାଲି; overflow_ele = overflow_ele->next)
		vim_toolbar_move_item_to(self, overflow_ele->data, TRUE);
	gtk_widget_set_visible(self->overflow_btn, need_overflow_btn);
    }
    ଅନ୍ୟଥା
	gtk_widget_set_visible(self->overflow_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.width = width;
    alloc.height = height;

    gtk_widget_size_allocate(self->ମୂଳ, &alloc, baseline);
}

    ସ୍ଥିର ଶୂନ୍ୟ
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		for_size,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*min,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*nat,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*min_baseline,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		strip_min = 0, strip_nat = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_size,
	    &strip_min, &strip_nat, ଖାଲି, ଖାଲି);
    gtk_widget_measure(self->overflow_btn, orientation, for_size,
	    &obtn_min, &obtn_nat, ଖାଲି, ଖାଲି);

    ଯଦି (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	ଯଦି (min != ଖାଲି)
	    *min = obtn_min;
	ଯଦି (nat != ଖାଲି)
	    *nat = strip_nat + obtn_nat;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (min != ଖାଲି)
	    *min = MAX(strip_min, obtn_min);
	ଯଦି (nat != ଖାଲି)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    ଯଦି (min_baseline != ଖାଲି)
	*min_baseline = -1;
    ଯଦି (nat_baseline != ଖାଲି)
	*nat_baseline = -1;
}

#endif // FEAT_TOOLBAR
