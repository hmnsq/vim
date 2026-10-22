/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"
#ضمن <gtk/gtk.h>
#ضمن "gui_gtk4_f.h"

/*
 * Child widget at position (x, y).
 */
تعريفنوع بنية
{
    GtkWidget	*widget;
    صحيح		x;
    صحيح		y;
} Vim_Form_ٻار;

/*
 * Similar to the GtkFixed widget, allows absolute position and sizing of child
 * widgets within. Vim already has logic for positioning and sizing UI elements,
 * so this is needed to take advantage of that. We don't use GtkFixed directly
 * since we need to override some vfuncs.
 */
بنية _VimForm
{
    GtkWidget والد;

    GList *children;

    // See vim_form_size_allocate()
    guint resize_idle_id;
    صحيح آخري_ويڪر;
    صحيح آخري_اوچائي;
};

G_DEFINE_TYPE(VimForm, vim_form, GTK_TYPE_WIDGET)

ساكن فراغ vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);
ساكن فراغ vim_form_سائيز_allocate(GtkWidget *widget, صحيح ويڪر, صحيح اوچائي, صحيح baseline);
ساكن فراغ vim_form_measure(GtkWidget *widget, GtkOrientation orientation, صحيح for_سائيز, صحيح *minimum, صحيح *natural, صحيح *minimum_baseline, صحيح *natural_baseline);
ساكن gboolean vim_form_شامل_آهي(GtkWidget *widget, مزدوج x, مزدوج y);

    ساكن فراغ
vim_form_dispose(GObject *obj)
{
    VimForm *self = VIM_FORM(obj);
    GList   *ele = self->children;

    طالما (ele != عدم)
    {
	Vim_Form_ٻار *ٻار = ele->معلومات;

	ele = ele->ايندڙ;
	gtk_widget_unparent(ٻار->widget);
	g_free(ٻار);
    }
    g_list_free(self->children);
    self->children = عدم;

    G_OBJECT_CLASS(vim_form_والد_class)->dispose(obj);
}

    ساكن فراغ
vim_form_class_init(VimFormClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_form_snapshot;
    widget_class->سائيز_allocate = vim_form_سائيز_allocate;
    widget_class->measure = vim_form_measure;
    widget_class->شامل_آهي = vim_form_شامل_آهي;

    obj_class->dispose = vim_form_dispose;
}

    ساكن فراغ
vim_form_init(VimForm *self اڻ_استعمال_ٿيل)
{
}

    GtkWidget *
vim_form_new(فراغ)
{
    أرجع g_object_new(VIM_TYPE_FORM, عدم);
}

/*
 * Transform the child
 */
    ساكن فراغ
vim_form_position_ٻار(VimForm	*self, Vim_Form_ٻار *ٻار)
{
    GtkRequisition  requisition;
    GskTransform    *transform;
    صحيح		    w, h;

    gtk_widget_get_preferred_سائيز(ٻار->widget, &requisition, عدم);
    w = requisition.ويڪر;
    h = requisition.اوچائي;

    // If widget has no size request, use parent size
    إذا (w <= 0)
	w = gtk_widget_get_ويڪر(GTK_WIDGET(self));
    إذا (h <= 0)
	h = gtk_widget_get_اوچائي(GTK_WIDGET(self));
    إذا (w <= 0) w = 1;
    إذا (h <= 0) h = 1;

    transform = gsk_transform_translate(عدم,
	    &GRAPHENE_نقطو_INIT((عائم)ٻار->x, (عائم)ٻار->y));
    gtk_widget_allocate(ٻار->widget, w, h, -1, transform);
}

/*
 * Place the given widget at the point (x, y).
 */
    فراغ
vim_form_put(VimForm *self, GtkWidget *widget, صحيح x, صحيح y)
{
    Vim_Form_ٻار *ٻار;

    ٻار = g_new(Vim_Form_ٻار, 1);

    ٻار->widget = widget;
    ٻار->x = x;
    ٻار->y = y;

    gtk_widget_سيٽ_سائيز_request(ٻار->widget, -1, -1);

    self->children = g_list_append(self->children, ٻار);

    gtk_widget_سيٽ_والد(widget, GTK_WIDGET(self));
    vim_form_position_ٻار(self, ٻار);
}

/*
 * Move the widget (which should have already been added using vim_form_put())
 * to the given point (x, y).
 */
    فراغ
vim_form_move(VimForm *self, GtkWidget *widget, صحيح x, صحيح y)
{
    لكل (GList *ele = self->children; ele != عدم; ele = ele->ايندڙ)
    {
	Vim_Form_ٻار *ٻار = ele->معلومات;
	إذا (ٻار->widget == widget)
	{
	    ٻار->x = x;
	    ٻار->y = y;
	    vim_form_position_ٻار(self, ٻار);
	    أرجع;
	}
    }
}

/*
 * Move and resize the child.
 */
    فراغ
vim_form_move_resize(
	VimForm	    *self,
	GtkWidget   *widget,
	gint	    x,
	gint	    y,
	gint	    w,
	gint	    h)
{
    gtk_widget_سيٽ_سائيز_request(widget, w, h);
    إذا (!gtk_widget_get_visible(widget))
	gtk_widget_قطار_resize(widget);
    vim_form_move(self, widget, x, y);
}

    فراغ
vim_form_remove(VimForm *self, GtkWidget *widget)
{
    لكل (GList *ele = self->children; ele != عدم; ele = ele->ايندڙ)
    {
	Vim_Form_ٻار *ٻار = ele->معلومات;
	إذا (ٻار->widget == widget)
	{
	    self->children = g_list_remove_link(self->children, ele);
	    g_list_free_1(ele);
	    gtk_widget_unparent(widget);
	    g_free(ٻار);
	    أرجع;
	}
    }
}

    ساكن فراغ
vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimForm *self = VIM_FORM(widget);

    لكل (GList *ele = self->children; ele != عدم; ele = ele->ايندڙ)
    {
	Vim_Form_ٻار *ٻار = ele->معلومات;
	gtk_widget_snapshot_ٻار(widget, ٻار->widget, snapshot);
    }
}

    ساكن gboolean
vim_form_resize_idle_cb(VimForm *self)
{
    self->resize_idle_id = 0;

    إذا (gui.drawarea == عدم)
	اذهب ٻاهر_نڪرو;

    إذا (self->آخري_ويڪر > 1 && self->آخري_اوچائي > 1)
    {
	// Ignore an allocation that does not answer the size that was last
	// asked for: it was computed before the request and using it would
	// compute Rows and Columns from the old size together with the new
	// base size, losing columns. Give up after one allocation in case
	// the request is never answered exactly.
	إذا (gui.pending_form_w > 0
		&& (self->آخري_ويڪر != gui.pending_form_w
		    || self->آخري_اوچائي != gui.pending_form_h)
		&& --gui.pending_form_skip >= 0)
	    اذهب ٻاهر_نڪرو;

	صحيح	req_w, req_h;

	gui.pending_form_w = 0;

	// The size request that Vim made has been answered.  Keep the size and
	// drop the request, otherwise the window could not be made smaller.
	gtk_widget_get_سائيز_request(GTK_WIDGET(self), &req_w, &req_h);
	إذا (req_w != -1 || req_h != -1)
	{
	    gtk_دري_سيٽ_default_سائيز(GTK_دري(gui.mainwin),
		    gtk_widget_get_ويڪر(gui.mainwin),
		    gtk_widget_get_اوچائي(gui.mainwin));
	    gtk_widget_سيٽ_سائيز_request(GTK_WIDGET(self), -1, -1);
	}

	gui_resize_shell(self->آخري_ويڪر, self->آخري_اوچائي);
    }

ٻاهر_نڪرو:
    g_object_unref(self);
    أرجع G_ذريعو_REMOVE;
}

    ساكن فراغ
vim_form_سائيز_allocate(
	GtkWidget *widget,
	صحيح ويڪر,
	صحيح اوچائي,
	صحيح baseline اڻ_استعمال_ٿيل)
{
    VimForm *self = VIM_FORM(widget);

    لكل (GList *ele = self->children; ele != عدم; ele = ele->ايندڙ)
	vim_form_position_ٻار(self, ele->معلومات);

    // Notify Vim about size change via idle callback
    إذا (ويڪر != self->آخري_ويڪر || اوچائي != self->آخري_اوچائي)
    {
	self->آخري_ويڪر = ويڪر;
	self->آخري_اوچائي = اوچائي;

	إذا (self->resize_idle_id == 0)
	    self->resize_idle_id = g_idle_جوڙيو(
		    (GSourceFunc)vim_form_resize_idle_cb, g_object_ref(self));
    }
}

    ساكن فراغ
vim_form_measure(
	GtkWidget	*widget اڻ_استعمال_ٿيل,
	GtkOrientation	orientation,
	صحيح		for_سائيز اڻ_استعمال_ٿيل,
	صحيح		*minimum,
	صحيح		*natural,
	صحيح		*minimum_baseline,
	صحيح		*natural_baseline)
{
    إذا (orientation == GTK_ORIENTATION_VERTICAL)
    {
	// Set minimum height of form widget to 4 rows.
	إذا (minimum != عدم)
	    *minimum  = gui.char_اوچائي * 4;
	إذا (natural != عدم)
	    *natural = gui.char_اوچائي * 4;
    }
    وإلا
    {
	// Set minimum width of form widget to 20 columns. Any less and the draw
	// area seems to glitch out...
	إذا (minimum != عدم)
	    *minimum  = gui.char_ويڪر * 20;
	إذا (natural != عدم)
	    *natural = gui.char_ويڪر * 20;
    }

    إذا (minimum_baseline != عدم)
	*minimum_baseline = -1;
    إذا (natural_baseline != عدم)
	*natural_baseline = -1;
}


/*
 * Make the form itself input-transparent so clicks on its empty area fall
 * through to the drawarea below, while the scrollbar children stay pickable.
 */
    ساكن gboolean
vim_form_شامل_آهي(GtkWidget *widget اڻ_استعمال_ٿيل, مزدوج x اڻ_استعمال_ٿيل, مزدوج y اڻ_استعمال_ٿيل)
{
    أرجع FALSE;
}
