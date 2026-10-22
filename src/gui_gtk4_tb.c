/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

#جے_تعریف_شدہ FEAT_TOOLBAR

#ضمن <gtk/gtk.h>
#ضمن "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
بنية _VimToolbar
{
    GtkWidget ماپا;

    صحيح style; // TOOLBAR_* flags
    صحيح iconsize; // TBIS_* values

    GList *اشیاء;

    GtkWidget	*جڑ;
    GtkWidget	*strip;

    GtkWidget	*حد_توں_ودھ_btn;
    GtkWidget	*حد_توں_ودھ_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

ساكن فراغ vim_toolbar_ناپ_allocate(GtkWidget *widget, صحيح چوڑائی, صحيح اچائی, صحيح baseline);
ساكن فراغ vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, صحيح for_ناپ, صحيح *minimum, صحيح *natural, صحيح *minimum_baseline, صحيح *natural_baseline);

    ساكن فراغ
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->اشیاء, g_object_unref);
    g_clear_اشارہ_گر(&self->جڑ, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_ماپا_class)->dispose(object);
}

    ساكن فراغ
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->ناپ_allocate = vim_toolbar_ناپ_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_سیٹ_css_name(widget_class, "toolbar");
}

    ساكن فراغ
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->جڑ = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_سیٹ_ماپا(self->جڑ, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_سیٹ_hexpand(self->strip, TRUE);
    gtk_widget_سیٹ_halign(self->strip, GTK_ALIGN_شروع);
    gtk_widget_سیٹ_حد_توں_ودھ(self->strip, GTK_حد_توں_ودھ_HIDDEN);
    gtk_box_append(GTK_BOX(self->جڑ), self->strip);

    self->حد_توں_ودھ_btn = gtk_menu_button_new();
    gtk_widget_سیٹ_hexpand(self->حد_توں_ودھ_btn, FALSE);
    gtk_widget_سیٹ_halign(self->حد_توں_ودھ_btn, GTK_ALIGN_سرا);
    gtk_widget_جوڑو_css_class(self->حد_توں_ودھ_btn, "flat");
    gtk_box_append(GTK_BOX(self->جڑ), self->حد_توں_ودھ_btn);
    g_object_سیٹ_انکڑے(G_OBJECT(self->حد_توں_ودھ_btn),
	    "toolbar-width", GINT_منزل_اشارہ_گر(-1));

    popover = gtk_popover_new();
    self->حد_توں_ودھ_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_popover_سیٹ_بچہ(GTK_POPOVER(popover), self->حد_توں_ودھ_box);
    gtk_popover_سیٹ_has_arrow(GTK_POPOVER(popover), FALSE);
    // Make sure popover aligns to top right of button
    gtk_widget_سیٹ_halign(popover, GTK_ALIGN_سرا);

    gtk_menu_button_سیٹ_popover(GTK_MENU_BUTTON(self->حد_توں_ودھ_btn), popover);
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
vim_toolbar_داخل_کرو(VimToolbar *self, GtkWidget *widget, صحيح idx)
{
    GtkWidget	*cur_بچہ = gtk_widget_get_پہلا_بچہ(self->strip);
    صحيح		i = 0;

    طالما (cur_بچہ != عدم && i != idx - 1)
    {
	cur_بچہ = gtk_widget_get_اگلا_sibling(cur_بچہ);
	i++;
    }

    gtk_box_داخل_کرو_بچہ_after(GTK_BOX(self->strip), widget, cur_بچہ);
    self->اشیاء = g_list_داخل_کرو(self->اشیاء, g_object_ref(widget), idx);
    gtk_widget_قطار_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    ساكن فراغ
سیٹ_button_style(GtkWidget *btn, صحيح style, صحيح iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    إذا (GTK_IS_SEPARATOR(btn))
	أرجع;

    box = gtk_button_get_بچہ(GTK_BUTTON(btn));
    إذا (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_سیٹ_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_سیٹ_spacing(GTK_BOX(box), 8);
    }
    وإلا
    {
	gtk_orientable_سیٹ_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_سیٹ_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_سیٹ_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_انکڑے(G_OBJECT(btn), "icon");
    إذا (icon != عدم)
    {
	إذا (style & TOOLBAR_ICONS)
	{
	    Gtk_Icon_ناپ ناپ;

	    بدل (iconsize)
	    {
		حالة TBIS_TINY:
		حالة TBIS_SMALL:
		حالة TBIS_MEDIUM:
		    ناپ = GTK_ICON_ناپ_عام;
		    اكسر;
		حالة TBIS_LARGE:
		حالة TBIS_HUGE:
		حالة TBIS_GIANT:
		    ناپ = GTK_ICON_ناپ_LARGE;
		    اكسر;
		افتراضي:
		    ناپ = GTK_ICON_ناپ_عام;
		    اكسر;
	    }
	    gtk_image_سیٹ_icon_ناپ(GTK_IMAGE(icon), ناپ);
	    gtk_widget_سیٹ_visible(icon, TRUE);
	}
	وإلا
	    gtk_widget_سیٹ_visible(icon, FALSE);
    }

    label = g_object_get_انکڑے(G_OBJECT(btn), "label");
    إذا (label != عدم)
	gtk_widget_سیٹ_visible(label, style & TOOLBAR_متن);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    إذا (invalidate)
	g_object_سیٹ_انکڑے(G_OBJECT(btn), "toolbar-width", GINT_منزل_اشارہ_گر(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    ساكن فراغ
button_clicked(GtkWidget *widget بن_ورتیا, VimToolbar *self)
{
    gtk_menu_button_popdown(GTK_MENU_BUTTON(self->حد_توں_ودھ_btn));
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
vim_toolbar_داخل_کرو_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	ثابت حرف  *متن,
	صحيح	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_جوڑو_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_سیٹ_بچہ(GTK_BUTTON(btn), box);

    إذا (icon != عدم)
    {
	gtk_widget_سیٹ_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_سیٹ_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_سیٹ_انکڑے(G_OBJECT(btn), "icon", icon);
    }
    إذا (متن != عدم)
    {
	GtkWidget *label = gtk_label_new(متن);

	gtk_widget_سیٹ_valign (label, GTK_ALIGN_CENTER);
	gtk_widget_سیٹ_vexpand (label, TRUE);
	gtk_box_append(GTK_BOX(box), label);
	g_object_سیٹ_انکڑے(G_OBJECT(btn), "label", label);
    }

    سیٹ_button_style(btn, self->style, self->iconsize, TRUE);

    g_signal_connect_object(btn, "clicked",
	    G_CALLBACK(button_clicked), self, G_CONNECT_DEFAULT);

    vim_toolbar_داخل_کرو(self, btn, idx);
    أرجع btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_داخل_کرو_separator(VimToolbar *self, صحيح idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_سیٹ_انکڑے(G_OBJECT(sep), "toolbar-width", GINT_منزل_اشارہ_گر(-1));
    vim_toolbar_داخل_کرو(self, sep, idx);
    أرجع sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    فراغ
vim_toolbar_سیٹ_style(VimToolbar *self, صحيح style, صحيح iconsize)
{
    GtkWidget *cur_بچہ = gtk_widget_get_پہلا_بچہ(self->strip);

    إذا (style == self->style && self->iconsize == iconsize)
	أرجع;

    self->style = style;
    self->iconsize = iconsize;

    طالما (cur_بچہ != عدم)
    {
	إذا (!GTK_IS_SEPARATOR(cur_بچہ))
	    سیٹ_button_style(cur_بچہ, style, iconsize, TRUE);
	cur_بچہ = gtk_widget_get_اگلا_sibling(cur_بچہ);
    }

    cur_بچہ = gtk_widget_get_پہلا_بچہ(self->حد_توں_ودھ_box);
    طالما (cur_بچہ != عدم)
    {
	إذا (!GTK_IS_SEPARATOR(cur_بچہ))
	    سیٹ_button_style(cur_بچہ, style, iconsize, TRUE);
	cur_بچہ = gtk_widget_get_اگلا_sibling(cur_بچہ);
    }
    // Sizes may have changed
    gtk_widget_قطار_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    فراغ
vim_toolbar_remove(VimToolbar *self, GtkWidget *شے)
{
    إذا (gtk_widget_get_ماپا(شے) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), شے);
    وإلا
	gtk_box_remove(GTK_BOX(self->حد_توں_ودھ_box), شے);

    self->اشیاء = g_list_remove(self->اشیاء, شے);
    g_object_unref(شے);
    gtk_widget_قطار_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    ساكن فراغ
vim_toolbar_move_شے_منزل(
	VimToolbar  *self,
	GtkWidget   *شے,
	gboolean    حد_توں_ودھ)
{
    GtkBox *from, *منزل;

    منزل = حد_توں_ودھ ? GTK_BOX(self->حد_توں_ودھ_box) : GTK_BOX(self->strip);
    إذا (gtk_widget_get_ماپا(شے) == GTK_WIDGET(منزل))
	أرجع;

    from = حد_توں_ودھ ? GTK_BOX(self->strip) : GTK_BOX(self->حد_توں_ودھ_box);

    g_object_ref(شے);
    gtk_box_remove(from, شے);
    gtk_box_append(منزل, شے);
    g_object_unref(شے);

    إذا (GTK_IS_SEPARATOR(شے))
	gtk_widget_سیٹ_visible(شے, !حد_توں_ودھ);
    وإلا
    {
	إذا (حد_توں_ودھ)
	{
	    صحيح style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_متن;
	    style |= TOOLBAR_ICONS;
	    سیٹ_button_style(شے, style, self->iconsize, FALSE);
	}
	وإلا
	    سیٹ_button_style(شے, self->style, self->iconsize, FALSE);
    }
}

    ساكن صحيح
get_شے_چوڑائی(GtkWidget *شے)
{
    صحيح toolbar_چوڑائی = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_چوڑائی = GPOINTER_منزل_INT(
	    g_object_get_انکڑے(G_OBJECT(شے), "toolbar-width"));

    إذا (toolbar_چوڑائی == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_ناپ(شے, &min_req, &nat_req);
	toolbar_چوڑائی = MAX(min_req.چوڑائی, nat_req.چوڑائی);

	// Save width for later
	g_object_سیٹ_انکڑے(G_OBJECT(شے),
		"toolbar-width", GINT_منزل_اشارہ_گر(toolbar_چوڑائی));
    }
    أرجع toolbar_چوڑائی;
}

    ساكن فراغ
vim_toolbar_ناپ_allocate(
	GtkWidget   *widget,
	صحيح	    چوڑائی,
	صحيح	    اچائی,
	صحيح	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    Gtk_ونڈ   alloc;
    صحيح		    ورتیا = 0;
    صحيح		    avail;
    GList	    *حد_توں_ودھ_ele = عدم;
    GtkWidget	    *بچہ;

    avail = چوڑائی - get_شے_چوڑائی(self->حد_توں_ودھ_btn);

    لكل (GList *ele = self->اشیاء; ele != عدم; ele = ele->اگلا)
    {
	GtkWidget   *شے = ele->انکڑے;
	صحيح	    toolbar_چوڑائی = get_شے_چوڑائی(شے);

	إذا (ورتیا + toolbar_چوڑائی > avail)
	{
	    حد_توں_ودھ_ele = ele;
	    اكسر;
	}
	وإلا
	    vim_toolbar_move_شے_منزل(self, شے, FALSE);
	ورتیا += toolbar_چوڑائی;
    }

    إذا (حد_توں_ودھ_ele != عدم)
    {
	صحيح	    بچیا = چوڑائی - ورتیا;
	gboolean    need_حد_توں_ودھ_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	طالما ((بچہ = gtk_widget_get_پہلا_بچہ(self->حد_توں_ودھ_box))
		!= عدم)
	    vim_toolbar_move_شے_منزل(self, بچہ, FALSE);

	لكل (GList *ele = حد_توں_ودھ_ele; ele != عدم; ele = ele->اگلا)
	{
	    بچیا -= get_شے_چوڑائی(ele->انکڑے);
	    إذا (بچیا < 0)
	    {
		need_حد_توں_ودھ_btn = TRUE;
		اكسر;
	    }
	}

	إذا (need_حد_توں_ودھ_btn)
	    لكل (; حد_توں_ودھ_ele != عدم; حد_توں_ودھ_ele = حد_توں_ودھ_ele->اگلا)
		vim_toolbar_move_شے_منزل(self, حد_توں_ودھ_ele->انکڑے, TRUE);
	gtk_widget_سیٹ_visible(self->حد_توں_ودھ_btn, need_حد_توں_ودھ_btn);
    }
    وإلا
	gtk_widget_سیٹ_visible(self->حد_توں_ودھ_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.چوڑائی = چوڑائی;
    alloc.اچائی = اچائی;

    gtk_widget_ناپ_allocate(self->جڑ, &alloc, baseline);
}

    ساكن فراغ
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	صحيح		for_ناپ,
	صحيح		*min,
	صحيح		*nat,
	صحيح		*min_baseline,
	صحيح		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    صحيح		strip_min = 0, strip_nat = 0;
    صحيح		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_ناپ,
	    &strip_min, &strip_nat, عدم, عدم);
    gtk_widget_measure(self->حد_توں_ودھ_btn, orientation, for_ناپ,
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

#جے_انت // FEAT_TOOLBAR
