/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

#اگر_تعریف_شده FEAT_TOOLBAR

#ضمن <gtk/gtk.h>
#ضمن "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
بنية _VimToolbar
{
    GtkWidget والد;

    صحيح style; // TOOLBAR_* flags
    صحيح iconsize; // TBIS_* values

    GList *اقلام;

    GtkWidget	*ریشه;
    GtkWidget	*strip;

    GtkWidget	*سرریز_btn;
    GtkWidget	*سرریز_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

ساكن فراغ vim_toolbar_اندازه_allocate(GtkWidget *widget, صحيح عرض, صحيح ارتفاع, صحيح baseline);
ساكن فراغ vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, صحيح for_اندازه, صحيح *minimum, صحيح *natural, صحيح *minimum_baseline, صحيح *natural_baseline);

    ساكن فراغ
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->اقلام, g_object_unref);
    g_clear_اشاره_گر(&self->ریشه, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_والد_class)->dispose(object);
}

    ساكن فراغ
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->اندازه_allocate = vim_toolbar_اندازه_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_مجموعه_css_name(widget_class, "toolbar");
}

    ساكن فراغ
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->ریشه = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_مجموعه_والد(self->ریشه, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_مجموعه_hexpand(self->strip, TRUE);
    gtk_widget_مجموعه_halign(self->strip, GTK_ALIGN_شروع);
    gtk_widget_مجموعه_سرریز(self->strip, GTK_سرریز_HIDDEN);
    gtk_box_append(GTK_BOX(self->ریشه), self->strip);

    self->سرریز_btn = gtk_menu_button_new();
    gtk_widget_مجموعه_hexpand(self->سرریز_btn, FALSE);
    gtk_widget_مجموعه_halign(self->سرریز_btn, GTK_ALIGN_انتها);
    gtk_widget_جمع_کن_css_class(self->سرریز_btn, "flat");
    gtk_box_append(GTK_BOX(self->ریشه), self->سرریز_btn);
    g_object_مجموعه_داده(G_OBJECT(self->سرریز_btn),
	    "toolbar-width", GINT_مقصد_اشاره_گر(-1));

    popover = gtk_popover_new();
    self->سرریز_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_popover_مجموعه_فرزند(GTK_POPOVER(popover), self->سرریز_box);
    gtk_popover_مجموعه_has_arrow(GTK_POPOVER(popover), FALSE);
    // Make sure popover aligns to top right of button
    gtk_widget_مجموعه_halign(popover, GTK_ALIGN_انتها);

    gtk_menu_button_مجموعه_popover(GTK_MENU_BUTTON(self->سرریز_btn), popover);
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
vim_toolbar_درج(VimToolbar *self, GtkWidget *widget, صحيح idx)
{
    GtkWidget	*cur_فرزند = gtk_widget_get_اولی_فرزند(self->strip);
    صحيح		i = 0;

    طالما (cur_فرزند != عدم && i != idx - 1)
    {
	cur_فرزند = gtk_widget_get_بعدی_sibling(cur_فرزند);
	i++;
    }

    gtk_box_درج_فرزند_after(GTK_BOX(self->strip), widget, cur_فرزند);
    self->اقلام = g_list_درج(self->اقلام, g_object_ref(widget), idx);
    gtk_widget_صف_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    ساكن فراغ
مجموعه_button_style(GtkWidget *btn, صحيح style, صحيح iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    إذا (GTK_IS_SEPARATOR(btn))
	أرجع;

    box = gtk_button_get_فرزند(GTK_BUTTON(btn));
    إذا (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_مجموعه_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_مجموعه_spacing(GTK_BOX(box), 8);
    }
    وإلا
    {
	gtk_orientable_مجموعه_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_مجموعه_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_مجموعه_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_داده(G_OBJECT(btn), "icon");
    إذا (icon != عدم)
    {
	إذا (style & TOOLBAR_ICONS)
	{
	    Gtk_Icon_اندازه اندازه;

	    بدل (iconsize)
	    {
		حالة TBIS_TINY:
		حالة TBIS_SMALL:
		حالة TBIS_MEDIUM:
		    اندازه = GTK_ICON_اندازه_نرمال;
		    اكسر;
		حالة TBIS_LARGE:
		حالة TBIS_HUGE:
		حالة TBIS_GIANT:
		    اندازه = GTK_ICON_اندازه_LARGE;
		    اكسر;
		افتراضي:
		    اندازه = GTK_ICON_اندازه_نرمال;
		    اكسر;
	    }
	    gtk_image_مجموعه_icon_اندازه(GTK_IMAGE(icon), اندازه);
	    gtk_widget_مجموعه_visible(icon, TRUE);
	}
	وإلا
	    gtk_widget_مجموعه_visible(icon, FALSE);
    }

    label = g_object_get_داده(G_OBJECT(btn), "label");
    إذا (label != عدم)
	gtk_widget_مجموعه_visible(label, style & TOOLBAR_متن);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    إذا (invalidate)
	g_object_مجموعه_داده(G_OBJECT(btn), "toolbar-width", GINT_مقصد_اشاره_گر(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    ساكن فراغ
button_clicked(GtkWidget *widget استفادهنشده, VimToolbar *self)
{
    gtk_menu_button_popdown(GTK_MENU_BUTTON(self->سرریز_btn));
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
vim_toolbar_درج_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	ثابت حرف  *متن,
	صحيح	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_جمع_کن_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_مجموعه_فرزند(GTK_BUTTON(btn), box);

    إذا (icon != عدم)
    {
	gtk_widget_مجموعه_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_مجموعه_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_مجموعه_داده(G_OBJECT(btn), "icon", icon);
    }
    إذا (متن != عدم)
    {
	GtkWidget *label = gtk_label_new(متن);

	gtk_widget_مجموعه_valign (label, GTK_ALIGN_CENTER);
	gtk_widget_مجموعه_vexpand (label, TRUE);
	gtk_box_append(GTK_BOX(box), label);
	g_object_مجموعه_داده(G_OBJECT(btn), "label", label);
    }

    مجموعه_button_style(btn, self->style, self->iconsize, TRUE);

    g_signal_connect_object(btn, "clicked",
	    G_CALLBACK(button_clicked), self, G_CONNECT_DEFAULT);

    vim_toolbar_درج(self, btn, idx);
    أرجع btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_درج_separator(VimToolbar *self, صحيح idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_مجموعه_داده(G_OBJECT(sep), "toolbar-width", GINT_مقصد_اشاره_گر(-1));
    vim_toolbar_درج(self, sep, idx);
    أرجع sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    فراغ
vim_toolbar_مجموعه_style(VimToolbar *self, صحيح style, صحيح iconsize)
{
    GtkWidget *cur_فرزند = gtk_widget_get_اولی_فرزند(self->strip);

    إذا (style == self->style && self->iconsize == iconsize)
	أرجع;

    self->style = style;
    self->iconsize = iconsize;

    طالما (cur_فرزند != عدم)
    {
	إذا (!GTK_IS_SEPARATOR(cur_فرزند))
	    مجموعه_button_style(cur_فرزند, style, iconsize, TRUE);
	cur_فرزند = gtk_widget_get_بعدی_sibling(cur_فرزند);
    }

    cur_فرزند = gtk_widget_get_اولی_فرزند(self->سرریز_box);
    طالما (cur_فرزند != عدم)
    {
	إذا (!GTK_IS_SEPARATOR(cur_فرزند))
	    مجموعه_button_style(cur_فرزند, style, iconsize, TRUE);
	cur_فرزند = gtk_widget_get_بعدی_sibling(cur_فرزند);
    }
    // Sizes may have changed
    gtk_widget_صف_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    فراغ
vim_toolbar_remove(VimToolbar *self, GtkWidget *قلم)
{
    إذا (gtk_widget_get_والد(قلم) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), قلم);
    وإلا
	gtk_box_remove(GTK_BOX(self->سرریز_box), قلم);

    self->اقلام = g_list_remove(self->اقلام, قلم);
    g_object_unref(قلم);
    gtk_widget_صف_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    ساكن فراغ
vim_toolbar_move_قلم_مقصد(
	VimToolbar  *self,
	GtkWidget   *قلم,
	gboolean    سرریز)
{
    GtkBox *from, *مقصد;

    مقصد = سرریز ? GTK_BOX(self->سرریز_box) : GTK_BOX(self->strip);
    إذا (gtk_widget_get_والد(قلم) == GTK_WIDGET(مقصد))
	أرجع;

    from = سرریز ? GTK_BOX(self->strip) : GTK_BOX(self->سرریز_box);

    g_object_ref(قلم);
    gtk_box_remove(from, قلم);
    gtk_box_append(مقصد, قلم);
    g_object_unref(قلم);

    إذا (GTK_IS_SEPARATOR(قلم))
	gtk_widget_مجموعه_visible(قلم, !سرریز);
    وإلا
    {
	إذا (سرریز)
	{
	    صحيح style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_متن;
	    style |= TOOLBAR_ICONS;
	    مجموعه_button_style(قلم, style, self->iconsize, FALSE);
	}
	وإلا
	    مجموعه_button_style(قلم, self->style, self->iconsize, FALSE);
    }
}

    ساكن صحيح
get_قلم_عرض(GtkWidget *قلم)
{
    صحيح toolbar_عرض = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_عرض = GPOINTER_مقصد_INT(
	    g_object_get_داده(G_OBJECT(قلم), "toolbar-width"));

    إذا (toolbar_عرض == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_اندازه(قلم, &min_req, &nat_req);
	toolbar_عرض = MAX(min_req.عرض, nat_req.عرض);

	// Save width for later
	g_object_مجموعه_داده(G_OBJECT(قلم),
		"toolbar-width", GINT_مقصد_اشاره_گر(toolbar_عرض));
    }
    أرجع toolbar_عرض;
}

    ساكن فراغ
vim_toolbar_اندازه_allocate(
	GtkWidget   *widget,
	صحيح	    عرض,
	صحيح	    ارتفاع,
	صحيح	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    Gtk_تخصیص   alloc;
    صحيح		    استفاده_شده = 0;
    صحيح		    avail;
    GList	    *سرریز_ele = عدم;
    GtkWidget	    *فرزند;

    avail = عرض - get_قلم_عرض(self->سرریز_btn);

    لكل (GList *ele = self->اقلام; ele != عدم; ele = ele->بعدی)
    {
	GtkWidget   *قلم = ele->داده;
	صحيح	    toolbar_عرض = get_قلم_عرض(قلم);

	إذا (استفاده_شده + toolbar_عرض > avail)
	{
	    سرریز_ele = ele;
	    اكسر;
	}
	وإلا
	    vim_toolbar_move_قلم_مقصد(self, قلم, FALSE);
	استفاده_شده += toolbar_عرض;
    }

    إذا (سرریز_ele != عدم)
    {
	صحيح	    مانده = عرض - استفاده_شده;
	gboolean    need_سرریز_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	طالما ((فرزند = gtk_widget_get_اولی_فرزند(self->سرریز_box))
		!= عدم)
	    vim_toolbar_move_قلم_مقصد(self, فرزند, FALSE);

	لكل (GList *ele = سرریز_ele; ele != عدم; ele = ele->بعدی)
	{
	    مانده -= get_قلم_عرض(ele->داده);
	    إذا (مانده < 0)
	    {
		need_سرریز_btn = TRUE;
		اكسر;
	    }
	}

	إذا (need_سرریز_btn)
	    لكل (; سرریز_ele != عدم; سرریز_ele = سرریز_ele->بعدی)
		vim_toolbar_move_قلم_مقصد(self, سرریز_ele->داده, TRUE);
	gtk_widget_مجموعه_visible(self->سرریز_btn, need_سرریز_btn);
    }
    وإلا
	gtk_widget_مجموعه_visible(self->سرریز_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.عرض = عرض;
    alloc.ارتفاع = ارتفاع;

    gtk_widget_اندازه_allocate(self->ریشه, &alloc, baseline);
}

    ساكن فراغ
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	صحيح		for_اندازه,
	صحيح		*min,
	صحيح		*nat,
	صحيح		*min_baseline,
	صحيح		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    صحيح		strip_min = 0, strip_nat = 0;
    صحيح		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_اندازه,
	    &strip_min, &strip_nat, عدم, عدم);
    gtk_widget_measure(self->سرریز_btn, orientation, for_اندازه,
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

#پایان_اگر // FEAT_TOOLBAR
