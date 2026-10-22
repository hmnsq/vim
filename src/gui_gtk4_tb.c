/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

#که_تعریف_شوی FEAT_TOOLBAR

#ضمن <gtk/gtk.h>
#ضمن "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
بنية _VimToolbar
{
    GtkWidget پلار;

    صحيح style; // TOOLBAR_* flags
    صحيح iconsize; // TBIS_* values

    GList *توکي;

    GtkWidget	*ریښه;
    GtkWidget	*strip;

    GtkWidget	*ډکېدنه_btn;
    GtkWidget	*ډکېدنه_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

ساكن فراغ vim_toolbar_اندازه_allocate(GtkWidget *widget, صحيح پلنوالی, صحيح لوړوالی, صحيح baseline);
ساكن فراغ vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, صحيح for_اندازه, صحيح *minimum, صحيح *natural, صحيح *minimum_baseline, صحيح *natural_baseline);

    ساكن فراغ
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->توکي, g_object_unref);
    g_clear_اشاره_ګر(&self->ریښه, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_پلار_class)->dispose(object);
}

    ساكن فراغ
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->اندازه_allocate = vim_toolbar_اندازه_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_سېټ_css_name(widget_class, "toolbar");
}

    ساكن فراغ
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->ریښه = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_سېټ_پلار(self->ریښه, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_سېټ_hexpand(self->strip, TRUE);
    gtk_widget_سېټ_halign(self->strip, GTK_ALIGN_پیل);
    gtk_widget_سېټ_ډکېدنه(self->strip, GTK_ډکېدنه_HIDDEN);
    gtk_box_append(GTK_BOX(self->ریښه), self->strip);

    self->ډکېدنه_btn = gtk_menu_button_new();
    gtk_widget_سېټ_hexpand(self->ډکېدنه_btn, FALSE);
    gtk_widget_سېټ_halign(self->ډکېدنه_btn, GTK_ALIGN_پای_ټکی);
    gtk_widget_جمع_کړه_css_class(self->ډکېدنه_btn, "flat");
    gtk_box_append(GTK_BOX(self->ریښه), self->ډکېدنه_btn);
    g_object_سېټ_معلومات(G_OBJECT(self->ډکېدنه_btn),
	    "toolbar-width", GINT_منزل_اشاره_ګر(-1));

    popover = gtk_popover_new();
    self->ډکېدنه_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_popover_سېټ_ماشوم(GTK_POPOVER(popover), self->ډکېدنه_box);
    gtk_popover_سېټ_has_arrow(GTK_POPOVER(popover), FALSE);
    // Make sure popover aligns to top right of button
    gtk_widget_سېټ_halign(popover, GTK_ALIGN_پای_ټکی);

    gtk_menu_button_سېټ_popover(GTK_MENU_BUTTON(self->ډکېدنه_btn), popover);
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
vim_toolbar_ورننباسه(VimToolbar *self, GtkWidget *widget, صحيح idx)
{
    GtkWidget	*cur_ماشوم = gtk_widget_get_لومړی_ماشوم(self->strip);
    صحيح		i = 0;

    طالما (cur_ماشوم != عدم && i != idx - 1)
    {
	cur_ماشوم = gtk_widget_get_بل_sibling(cur_ماشوم);
	i++;
    }

    gtk_box_ورننباسه_ماشوم_after(GTK_BOX(self->strip), widget, cur_ماشوم);
    self->توکي = g_list_ورننباسه(self->توکي, g_object_ref(widget), idx);
    gtk_widget_کتار_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    ساكن فراغ
سېټ_button_style(GtkWidget *btn, صحيح style, صحيح iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    إذا (GTK_IS_SEPARATOR(btn))
	أرجع;

    box = gtk_button_get_ماشوم(GTK_BUTTON(btn));
    إذا (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_سېټ_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_سېټ_spacing(GTK_BOX(box), 8);
    }
    وإلا
    {
	gtk_orientable_سېټ_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_سېټ_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_سېټ_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_معلومات(G_OBJECT(btn), "icon");
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
		    اندازه = GTK_ICON_اندازه_نورمال;
		    اكسر;
		حالة TBIS_LARGE:
		حالة TBIS_HUGE:
		حالة TBIS_GIANT:
		    اندازه = GTK_ICON_اندازه_LARGE;
		    اكسر;
		افتراضي:
		    اندازه = GTK_ICON_اندازه_نورمال;
		    اكسر;
	    }
	    gtk_image_سېټ_icon_اندازه(GTK_IMAGE(icon), اندازه);
	    gtk_widget_سېټ_visible(icon, TRUE);
	}
	وإلا
	    gtk_widget_سېټ_visible(icon, FALSE);
    }

    label = g_object_get_معلومات(G_OBJECT(btn), "label");
    إذا (label != عدم)
	gtk_widget_سېټ_visible(label, style & TOOLBAR_متن);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    إذا (invalidate)
	g_object_سېټ_معلومات(G_OBJECT(btn), "toolbar-width", GINT_منزل_اشاره_ګر(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    ساكن فراغ
button_clicked(GtkWidget *widget ناکارول_شوی, VimToolbar *self)
{
    gtk_menu_button_popdown(GTK_MENU_BUTTON(self->ډکېدنه_btn));
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
vim_toolbar_ورننباسه_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	ثابت حرف  *متن,
	صحيح	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_جمع_کړه_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_سېټ_ماشوم(GTK_BUTTON(btn), box);

    إذا (icon != عدم)
    {
	gtk_widget_سېټ_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_سېټ_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_سېټ_معلومات(G_OBJECT(btn), "icon", icon);
    }
    إذا (متن != عدم)
    {
	GtkWidget *label = gtk_label_new(متن);

	gtk_widget_سېټ_valign (label, GTK_ALIGN_CENTER);
	gtk_widget_سېټ_vexpand (label, TRUE);
	gtk_box_append(GTK_BOX(box), label);
	g_object_سېټ_معلومات(G_OBJECT(btn), "label", label);
    }

    سېټ_button_style(btn, self->style, self->iconsize, TRUE);

    g_signal_connect_object(btn, "clicked",
	    G_CALLBACK(button_clicked), self, G_CONNECT_DEFAULT);

    vim_toolbar_ورننباسه(self, btn, idx);
    أرجع btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_ورننباسه_separator(VimToolbar *self, صحيح idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_سېټ_معلومات(G_OBJECT(sep), "toolbar-width", GINT_منزل_اشاره_ګر(-1));
    vim_toolbar_ورننباسه(self, sep, idx);
    أرجع sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    فراغ
vim_toolbar_سېټ_style(VimToolbar *self, صحيح style, صحيح iconsize)
{
    GtkWidget *cur_ماشوم = gtk_widget_get_لومړی_ماشوم(self->strip);

    إذا (style == self->style && self->iconsize == iconsize)
	أرجع;

    self->style = style;
    self->iconsize = iconsize;

    طالما (cur_ماشوم != عدم)
    {
	إذا (!GTK_IS_SEPARATOR(cur_ماشوم))
	    سېټ_button_style(cur_ماشوم, style, iconsize, TRUE);
	cur_ماشوم = gtk_widget_get_بل_sibling(cur_ماشوم);
    }

    cur_ماشوم = gtk_widget_get_لومړی_ماشوم(self->ډکېدنه_box);
    طالما (cur_ماشوم != عدم)
    {
	إذا (!GTK_IS_SEPARATOR(cur_ماشوم))
	    سېټ_button_style(cur_ماشوم, style, iconsize, TRUE);
	cur_ماشوم = gtk_widget_get_بل_sibling(cur_ماشوم);
    }
    // Sizes may have changed
    gtk_widget_کتار_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    فراغ
vim_toolbar_remove(VimToolbar *self, GtkWidget *توکی)
{
    إذا (gtk_widget_get_پلار(توکی) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), توکی);
    وإلا
	gtk_box_remove(GTK_BOX(self->ډکېدنه_box), توکی);

    self->توکي = g_list_remove(self->توکي, توکی);
    g_object_unref(توکی);
    gtk_widget_کتار_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    ساكن فراغ
vim_toolbar_move_توکی_منزل(
	VimToolbar  *self,
	GtkWidget   *توکی,
	gboolean    ډکېدنه)
{
    GtkBox *from, *منزل;

    منزل = ډکېدنه ? GTK_BOX(self->ډکېدنه_box) : GTK_BOX(self->strip);
    إذا (gtk_widget_get_پلار(توکی) == GTK_WIDGET(منزل))
	أرجع;

    from = ډکېدنه ? GTK_BOX(self->strip) : GTK_BOX(self->ډکېدنه_box);

    g_object_ref(توکی);
    gtk_box_remove(from, توکی);
    gtk_box_append(منزل, توکی);
    g_object_unref(توکی);

    إذا (GTK_IS_SEPARATOR(توکی))
	gtk_widget_سېټ_visible(توکی, !ډکېدنه);
    وإلا
    {
	إذا (ډکېدنه)
	{
	    صحيح style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_متن;
	    style |= TOOLBAR_ICONS;
	    سېټ_button_style(توکی, style, self->iconsize, FALSE);
	}
	وإلا
	    سېټ_button_style(توکی, self->style, self->iconsize, FALSE);
    }
}

    ساكن صحيح
get_توکی_پلنوالی(GtkWidget *توکی)
{
    صحيح toolbar_پلنوالی = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_پلنوالی = GPOINTER_منزل_INT(
	    g_object_get_معلومات(G_OBJECT(توکی), "toolbar-width"));

    إذا (toolbar_پلنوالی == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_اندازه(توکی, &min_req, &nat_req);
	toolbar_پلنوالی = MAX(min_req.پلنوالی, nat_req.پلنوالی);

	// Save width for later
	g_object_سېټ_معلومات(G_OBJECT(توکی),
		"toolbar-width", GINT_منزل_اشاره_ګر(toolbar_پلنوالی));
    }
    أرجع toolbar_پلنوالی;
}

    ساكن فراغ
vim_toolbar_اندازه_allocate(
	GtkWidget   *widget,
	صحيح	    پلنوالی,
	صحيح	    لوړوالی,
	صحيح	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    Gtk_ویش   alloc;
    صحيح		    کارول_شوی = 0;
    صحيح		    avail;
    GList	    *ډکېدنه_ele = عدم;
    GtkWidget	    *ماشوم;

    avail = پلنوالی - get_توکی_پلنوالی(self->ډکېدنه_btn);

    لكل (GList *ele = self->توکي; ele != عدم; ele = ele->بل)
    {
	GtkWidget   *توکی = ele->معلومات;
	صحيح	    toolbar_پلنوالی = get_توکی_پلنوالی(توکی);

	إذا (کارول_شوی + toolbar_پلنوالی > avail)
	{
	    ډکېدنه_ele = ele;
	    اكسر;
	}
	وإلا
	    vim_toolbar_move_توکی_منزل(self, توکی, FALSE);
	کارول_شوی += toolbar_پلنوالی;
    }

    إذا (ډکېدنه_ele != عدم)
    {
	صحيح	    پاتې_شوی = پلنوالی - کارول_شوی;
	gboolean    need_ډکېدنه_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	طالما ((ماشوم = gtk_widget_get_لومړی_ماشوم(self->ډکېدنه_box))
		!= عدم)
	    vim_toolbar_move_توکی_منزل(self, ماشوم, FALSE);

	لكل (GList *ele = ډکېدنه_ele; ele != عدم; ele = ele->بل)
	{
	    پاتې_شوی -= get_توکی_پلنوالی(ele->معلومات);
	    إذا (پاتې_شوی < 0)
	    {
		need_ډکېدنه_btn = TRUE;
		اكسر;
	    }
	}

	إذا (need_ډکېدنه_btn)
	    لكل (; ډکېدنه_ele != عدم; ډکېدنه_ele = ډکېدنه_ele->بل)
		vim_toolbar_move_توکی_منزل(self, ډکېدنه_ele->معلومات, TRUE);
	gtk_widget_سېټ_visible(self->ډکېدنه_btn, need_ډکېدنه_btn);
    }
    وإلا
	gtk_widget_سېټ_visible(self->ډکېدنه_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.پلنوالی = پلنوالی;
    alloc.لوړوالی = لوړوالی;

    gtk_widget_اندازه_allocate(self->ریښه, &alloc, baseline);
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
    gtk_widget_measure(self->ډکېدنه_btn, orientation, for_اندازه,
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

#د_که_پای // FEAT_TOOLBAR
