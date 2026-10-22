/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

#ifdef FEAT_TOOLBAR

#ضمن <gtk/gtk.h>
#ضمن "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
بنية _VimToolbar
{
    GtkWidget parent;

    صحيح style; // TOOLBAR_* flags
    صحيح iconsize; // TBIS_* values

    GList *کاڵاکان;

    GtkWidget	*ڕەگ;
    GtkWidget	*strip;

    GtkWidget	*overflow_btn;
    GtkWidget	*overflow_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

ساكن فراغ vim_toolbar_size_allocate(GtkWidget *widget, صحيح width, صحيح height, صحيح baseline);
ساكن فراغ vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, صحيح for_size, صحيح *minimum, صحيح *natural, صحيح *minimum_baseline, صحيح *natural_baseline);

    ساكن فراغ
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->کاڵاکان, g_object_unref);
    g_clear_pointer(&self->ڕەگ, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_parent_class)->dispose(object);
}

    ساكن فراغ
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->size_allocate = vim_toolbar_size_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_set_css_name(widget_class, "toolbar");
}

    ساكن فراغ
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->ڕەگ = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_parent(self->ڕەگ, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(self->strip, TRUE);
    gtk_widget_set_halign(self->strip, GTK_ALIGN_START);
    gtk_widget_set_overflow(self->strip, GTK_OVERFLOW_HIDDEN);
    gtk_box_append(GTK_BOX(self->ڕەگ), self->strip);

    self->overflow_btn = gtk_menu_button_new();
    gtk_widget_set_hexpand(self->overflow_btn, FALSE);
    gtk_widget_set_halign(self->overflow_btn, GTK_ALIGN_END);
    gtk_widget_add_css_class(self->overflow_btn, "flat");
    gtk_box_append(GTK_BOX(self->ڕەگ), self->overflow_btn);
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
vim_toolbar_new(فراغ)
{
    أرجع g_object_new(VIM_TYPE_TOOLBAR, عدم);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    ساكن فراغ
vim_toolbar_زیادکردن(VimToolbar *self, GtkWidget *widget, صحيح idx)
{
    GtkWidget	*cur_child = gtk_widget_get_first_child(self->strip);
    صحيح		i = 0;

    طالما (cur_child != عدم && i != idx - 1)
    {
	cur_child = gtk_widget_get_next_sibling(cur_child);
	i++;
    }

    gtk_box_زیادکردن_child_after(GTK_BOX(self->strip), widget, cur_child);
    self->کاڵاکان = g_list_زیادکردن(self->کاڵاکان, g_object_ref(widget), idx);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    ساكن فراغ
set_button_style(GtkWidget *btn, صحيح style, صحيح iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    إذا (GTK_IS_SEPARATOR(btn))
	أرجع;

    box = gtk_button_get_child(GTK_BUTTON(btn));
    إذا (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_set_spacing(GTK_BOX(box), 8);
    }
    وإلا
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_set_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_set_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_data(G_OBJECT(btn), "icon");
    إذا (icon != عدم)
    {
	إذا (style & TOOLBAR_ICONS)
	{
	    GtkIconSize size;

	    بدل (iconsize)
	    {
		حالة TBIS_TINY:
		حالة TBIS_SMALL:
		حالة TBIS_MEDIUM:
		    size = GTK_ICON_SIZE_NORMAL;
		    اكسر;
		حالة TBIS_LARGE:
		حالة TBIS_HUGE:
		حالة TBIS_GIANT:
		    size = GTK_ICON_SIZE_LARGE;
		    اكسر;
		افتراضي:
		    size = GTK_ICON_SIZE_NORMAL;
		    اكسر;
	    }
	    gtk_image_set_icon_size(GTK_IMAGE(icon), size);
	    gtk_widget_set_visible(icon, TRUE);
	}
	وإلا
	    gtk_widget_set_visible(icon, FALSE);
    }

    label = g_object_get_data(G_OBJECT(btn), "label");
    إذا (label != عدم)
	gtk_widget_set_visible(label, style & TOOLBAR_TEXT);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    إذا (invalidate)
	g_object_set_data(G_OBJECT(btn), "toolbar-width", GINT_TO_POINTER(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    ساكن فراغ
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
vim_toolbar_زیادکردن_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	ثابت حرف  *text,
	صحيح	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_add_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_set_child(GTK_BUTTON(btn), box);

    إذا (icon != عدم)
    {
	gtk_widget_set_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_set_data(G_OBJECT(btn), "icon", icon);
    }
    إذا (text != عدم)
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

    vim_toolbar_زیادکردن(self, btn, idx);
    أرجع btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_زیادکردن_separator(VimToolbar *self, صحيح idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_set_data(G_OBJECT(sep), "toolbar-width", GINT_TO_POINTER(-1));
    vim_toolbar_زیادکردن(self, sep, idx);
    أرجع sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    فراغ
vim_toolbar_set_style(VimToolbar *self, صحيح style, صحيح iconsize)
{
    GtkWidget *cur_child = gtk_widget_get_first_child(self->strip);

    إذا (style == self->style && self->iconsize == iconsize)
	أرجع;

    self->style = style;
    self->iconsize = iconsize;

    طالما (cur_child != عدم)
    {
	إذا (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }

    cur_child = gtk_widget_get_first_child(self->overflow_box);
    طالما (cur_child != عدم)
    {
	إذا (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }
    // Sizes may have changed
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    فراغ
vim_toolbar_remove(VimToolbar *self, GtkWidget *item)
{
    إذا (gtk_widget_get_parent(item) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), item);
    وإلا
	gtk_box_remove(GTK_BOX(self->overflow_box), item);

    self->کاڵاکان = g_list_remove(self->کاڵاکان, item);
    g_object_unref(item);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    ساكن فراغ
vim_toolbar_move_item_to(
	VimToolbar  *self,
	GtkWidget   *item,
	gboolean    overflow)
{
    GtkBox *from, *to;

    to = overflow ? GTK_BOX(self->overflow_box) : GTK_BOX(self->strip);
    إذا (gtk_widget_get_parent(item) == GTK_WIDGET(to))
	أرجع;

    from = overflow ? GTK_BOX(self->strip) : GTK_BOX(self->overflow_box);

    g_object_ref(item);
    gtk_box_remove(from, item);
    gtk_box_append(to, item);
    g_object_unref(item);

    إذا (GTK_IS_SEPARATOR(item))
	gtk_widget_set_visible(item, !overflow);
    وإلا
    {
	إذا (overflow)
	{
	    صحيح style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_TEXT;
	    style |= TOOLBAR_ICONS;
	    set_button_style(item, style, self->iconsize, FALSE);
	}
	وإلا
	    set_button_style(item, self->style, self->iconsize, FALSE);
    }
}

    ساكن صحيح
get_item_width(GtkWidget *item)
{
    صحيح toolbar_width = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_width = GPOINTER_TO_INT(
	    g_object_get_data(G_OBJECT(item), "toolbar-width"));

    إذا (toolbar_width == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_size(item, &min_req, &nat_req);
	toolbar_width = MAX(min_req.width, nat_req.width);

	// Save width for later
	g_object_set_data(G_OBJECT(item),
		"toolbar-width", GINT_TO_POINTER(toolbar_width));
    }
    أرجع toolbar_width;
}

    ساكن فراغ
vim_toolbar_size_allocate(
	GtkWidget   *widget,
	صحيح	    width,
	صحيح	    height,
	صحيح	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    GtkAllocation   alloc;
    صحيح		    used = 0;
    صحيح		    avail;
    GList	    *overflow_ele = عدم;
    GtkWidget	    *child;

    avail = width - get_item_width(self->overflow_btn);

    لكل (GList *ele = self->کاڵاکان; ele != عدم; ele = ele->next)
    {
	GtkWidget   *item = ele->data;
	صحيح	    toolbar_width = get_item_width(item);

	إذا (used + toolbar_width > avail)
	{
	    overflow_ele = ele;
	    اكسر;
	}
	وإلا
	    vim_toolbar_move_item_to(self, item, FALSE);
	used += toolbar_width;
    }

    إذا (overflow_ele != عدم)
    {
	صحيح	    remaining = width - used;
	gboolean    need_overflow_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	طالما ((child = gtk_widget_get_first_child(self->overflow_box))
		!= عدم)
	    vim_toolbar_move_item_to(self, child, FALSE);

	لكل (GList *ele = overflow_ele; ele != عدم; ele = ele->next)
	{
	    remaining -= get_item_width(ele->data);
	    إذا (remaining < 0)
	    {
		need_overflow_btn = TRUE;
		اكسر;
	    }
	}

	إذا (need_overflow_btn)
	    لكل (; overflow_ele != عدم; overflow_ele = overflow_ele->next)
		vim_toolbar_move_item_to(self, overflow_ele->data, TRUE);
	gtk_widget_set_visible(self->overflow_btn, need_overflow_btn);
    }
    وإلا
	gtk_widget_set_visible(self->overflow_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.width = width;
    alloc.height = height;

    gtk_widget_size_allocate(self->ڕەگ, &alloc, baseline);
}

    ساكن فراغ
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	صحيح		for_size,
	صحيح		*min,
	صحيح		*nat,
	صحيح		*min_baseline,
	صحيح		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    صحيح		strip_min = 0, strip_nat = 0;
    صحيح		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_size,
	    &strip_min, &strip_nat, عدم, عدم);
    gtk_widget_measure(self->overflow_btn, orientation, for_size,
	    &obtn_min, &obtn_nat, عدم, عدم);

    إذا (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	إذا (min != عدم)
	    *min = obtn_min;
	إذا (nat != عدم)
	    *nat = strip_nat + obtn_nat;
    }
    وإلا
    {
	إذا (min != عدم)
	    *min = MAX(strip_min, obtn_min);
	إذا (nat != عدم)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    إذا (min_baseline != عدم)
	*min_baseline = -1;
    إذا (nat_baseline != عدم)
	*nat_baseline = -1;
}

#endif // FEAT_TOOLBAR
