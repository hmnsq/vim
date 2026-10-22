/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <gtk/gtk.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "gui_gtk4_f.h"

/*
 * Child widget at position (x, y).
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    GtkWidget	*widget;
    ਪੂਰਨ		x;
    ਪੂਰਨ		y;
} Vim_Form_ਬੱਚਾ;

/*
 * Similar to the GtkFixed widget, allows absolute position and sizing of child
 * widgets within. Vim already has logic for positioning and sizing UI elements,
 * so this is needed to take advantage of that. We don't use GtkFixed directly
 * since we need to override some vfuncs.
 */
ਬਣਤਰ _VimForm
{
    GtkWidget ਮਾਪਾ;

    GList *children;

    // See vim_form_size_allocate()
    guint resize_idle_id;
    ਪੂਰਨ ਆਖਰੀ_ਚੌੜਾਈ;
    ਪੂਰਨ ਆਖਰੀ_ਉਚਾਈ;
};

G_DEFINE_TYPE(VimForm, vim_form, GTK_TYPE_WIDGET)

ਸਥਿਰ ਖਾਲੀ vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);
ਸਥਿਰ ਖਾਲੀ vim_form_ਮਾਪ_allocate(GtkWidget *widget, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ, ਪੂਰਨ baseline);
ਸਥਿਰ ਖਾਲੀ vim_form_measure(GtkWidget *widget, GtkOrientation orientation, ਪੂਰਨ for_ਮਾਪ, ਪੂਰਨ *minimum, ਪੂਰਨ *natural, ਪੂਰਨ *minimum_baseline, ਪੂਰਨ *natural_baseline);
ਸਥਿਰ gboolean vim_form_ਸ਼ਾਮਲ_ਹੈ(GtkWidget *widget, ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y);

    ਸਥਿਰ ਖਾਲੀ
vim_form_dispose(GObject *obj)
{
    VimForm *self = VIM_FORM(obj);
    GList   *ele = self->children;

    ਜਦੋਂ (ele != ਨਿਰਾ)
    {
	Vim_Form_ਬੱਚਾ *ਬੱਚਾ = ele->ਅੰਕੜੇ;

	ele = ele->ਅਗਲਾ;
	gtk_widget_unparent(ਬੱਚਾ->widget);
	g_free(ਬੱਚਾ);
    }
    g_list_free(self->children);
    self->children = ਨਿਰਾ;

    G_OBJECT_CLASS(vim_form_ਮਾਪਾ_class)->dispose(obj);
}

    ਸਥਿਰ ਖਾਲੀ
vim_form_class_init(VimFormClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_form_snapshot;
    widget_class->ਮਾਪ_allocate = vim_form_ਮਾਪ_allocate;
    widget_class->measure = vim_form_measure;
    widget_class->ਸ਼ਾਮਲ_ਹੈ = vim_form_ਸ਼ਾਮਲ_ਹੈ;

    obj_class->dispose = vim_form_dispose;
}

    ਸਥਿਰ ਖਾਲੀ
vim_form_init(VimForm *self ਅਣਵਰਤਿਆ)
{
}

    GtkWidget *
vim_form_new(ਖਾਲੀ)
{
    ਵਾਪਸ g_object_new(VIM_TYPE_FORM, ਨਿਰਾ);
}

/*
 * Transform the child
 */
    ਸਥਿਰ ਖਾਲੀ
vim_form_position_ਬੱਚਾ(VimForm	*self, Vim_Form_ਬੱਚਾ *ਬੱਚਾ)
{
    GtkRequisition  requisition;
    GskTransform    *transform;
    ਪੂਰਨ		    w, h;

    gtk_widget_get_preferred_ਮਾਪ(ਬੱਚਾ->widget, &requisition, ਨਿਰਾ);
    w = requisition.ਚੌੜਾਈ;
    h = requisition.ਉਚਾਈ;

    // If widget has no size request, use parent size
    ਜੇ (w <= 0)
	w = gtk_widget_get_ਚੌੜਾਈ(GTK_WIDGET(self));
    ਜੇ (h <= 0)
	h = gtk_widget_get_ਉਚਾਈ(GTK_WIDGET(self));
    ਜੇ (w <= 0) w = 1;
    ਜੇ (h <= 0) h = 1;

    transform = gsk_transform_translate(ਨਿਰਾ,
	    &GRAPHENE_ਬਿੰਦੂ_INIT((ਦਸ਼ਮਲਵ)ਬੱਚਾ->x, (ਦਸ਼ਮਲਵ)ਬੱਚਾ->y));
    gtk_widget_allocate(ਬੱਚਾ->widget, w, h, -1, transform);
}

/*
 * Place the given widget at the point (x, y).
 */
    ਖਾਲੀ
vim_form_put(VimForm *self, GtkWidget *widget, ਪੂਰਨ x, ਪੂਰਨ y)
{
    Vim_Form_ਬੱਚਾ *ਬੱਚਾ;

    ਬੱਚਾ = g_new(Vim_Form_ਬੱਚਾ, 1);

    ਬੱਚਾ->widget = widget;
    ਬੱਚਾ->x = x;
    ਬੱਚਾ->y = y;

    gtk_widget_ਸੈੱਟ_ਮਾਪ_request(ਬੱਚਾ->widget, -1, -1);

    self->children = g_list_append(self->children, ਬੱਚਾ);

    gtk_widget_ਸੈੱਟ_ਮਾਪਾ(widget, GTK_WIDGET(self));
    vim_form_position_ਬੱਚਾ(self, ਬੱਚਾ);
}

/*
 * Move the widget (which should have already been added using vim_form_put())
 * to the given point (x, y).
 */
    ਖਾਲੀ
vim_form_move(VimForm *self, GtkWidget *widget, ਪੂਰਨ x, ਪੂਰਨ y)
{
    ਲਈ (GList *ele = self->children; ele != ਨਿਰਾ; ele = ele->ਅਗਲਾ)
    {
	Vim_Form_ਬੱਚਾ *ਬੱਚਾ = ele->ਅੰਕੜੇ;
	ਜੇ (ਬੱਚਾ->widget == widget)
	{
	    ਬੱਚਾ->x = x;
	    ਬੱਚਾ->y = y;
	    vim_form_position_ਬੱਚਾ(self, ਬੱਚਾ);
	    ਵਾਪਸ;
	}
    }
}

/*
 * Move and resize the child.
 */
    ਖਾਲੀ
vim_form_move_resize(
	VimForm	    *self,
	GtkWidget   *widget,
	gint	    x,
	gint	    y,
	gint	    w,
	gint	    h)
{
    gtk_widget_ਸੈੱਟ_ਮਾਪ_request(widget, w, h);
    ਜੇ (!gtk_widget_get_visible(widget))
	gtk_widget_ਕਤਾਰ_resize(widget);
    vim_form_move(self, widget, x, y);
}

    ਖਾਲੀ
vim_form_remove(VimForm *self, GtkWidget *widget)
{
    ਲਈ (GList *ele = self->children; ele != ਨਿਰਾ; ele = ele->ਅਗਲਾ)
    {
	Vim_Form_ਬੱਚਾ *ਬੱਚਾ = ele->ਅੰਕੜੇ;
	ਜੇ (ਬੱਚਾ->widget == widget)
	{
	    self->children = g_list_remove_link(self->children, ele);
	    g_list_free_1(ele);
	    gtk_widget_unparent(widget);
	    g_free(ਬੱਚਾ);
	    ਵਾਪਸ;
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimForm *self = VIM_FORM(widget);

    ਲਈ (GList *ele = self->children; ele != ਨਿਰਾ; ele = ele->ਅਗਲਾ)
    {
	Vim_Form_ਬੱਚਾ *ਬੱਚਾ = ele->ਅੰਕੜੇ;
	gtk_widget_snapshot_ਬੱਚਾ(widget, ਬੱਚਾ->widget, snapshot);
    }
}

    ਸਥਿਰ gboolean
vim_form_resize_idle_cb(VimForm *self)
{
    self->resize_idle_id = 0;

    ਜੇ (gui.drawarea == ਨਿਰਾ)
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

    ਜੇ (self->ਆਖਰੀ_ਚੌੜਾਈ > 1 && self->ਆਖਰੀ_ਉਚਾਈ > 1)
    {
	// Ignore an allocation that does not answer the size that was last
	// asked for: it was computed before the request and using it would
	// compute Rows and Columns from the old size together with the new
	// base size, losing columns. Give up after one allocation in case
	// the request is never answered exactly.
	ਜੇ (gui.pending_form_w > 0
		&& (self->ਆਖਰੀ_ਚੌੜਾਈ != gui.pending_form_w
		    || self->ਆਖਰੀ_ਉਚਾਈ != gui.pending_form_h)
		&& --gui.pending_form_skip >= 0)
	    ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

	ਪੂਰਨ	req_w, req_h;

	gui.pending_form_w = 0;

	// The size request that Vim made has been answered.  Keep the size and
	// drop the request, otherwise the window could not be made smaller.
	gtk_widget_get_ਮਾਪ_request(GTK_WIDGET(self), &req_w, &req_h);
	ਜੇ (req_w != -1 || req_h != -1)
	{
	    gtk_ਖਿੜਕੀ_ਸੈੱਟ_default_ਮਾਪ(GTK_ਖਿੜਕੀ(gui.mainwin),
		    gtk_widget_get_ਚੌੜਾਈ(gui.mainwin),
		    gtk_widget_get_ਉਚਾਈ(gui.mainwin));
	    gtk_widget_ਸੈੱਟ_ਮਾਪ_request(GTK_WIDGET(self), -1, -1);
	}

	gui_resize_shell(self->ਆਖਰੀ_ਚੌੜਾਈ, self->ਆਖਰੀ_ਉਚਾਈ);
    }

ਬਾਹਰ_ਨਿਕਲੋ:
    g_object_unref(self);
    ਵਾਪਸ G_ਸਰੋਤ_REMOVE;
}

    ਸਥਿਰ ਖਾਲੀ
vim_form_ਮਾਪ_allocate(
	GtkWidget *widget,
	ਪੂਰਨ ਚੌੜਾਈ,
	ਪੂਰਨ ਉਚਾਈ,
	ਪੂਰਨ baseline ਅਣਵਰਤਿਆ)
{
    VimForm *self = VIM_FORM(widget);

    ਲਈ (GList *ele = self->children; ele != ਨਿਰਾ; ele = ele->ਅਗਲਾ)
	vim_form_position_ਬੱਚਾ(self, ele->ਅੰਕੜੇ);

    // Notify Vim about size change via idle callback
    ਜੇ (ਚੌੜਾਈ != self->ਆਖਰੀ_ਚੌੜਾਈ || ਉਚਾਈ != self->ਆਖਰੀ_ਉਚਾਈ)
    {
	self->ਆਖਰੀ_ਚੌੜਾਈ = ਚੌੜਾਈ;
	self->ਆਖਰੀ_ਉਚਾਈ = ਉਚਾਈ;

	ਜੇ (self->resize_idle_id == 0)
	    self->resize_idle_id = g_idle_ਜੋੜੋ(
		    (GSourceFunc)vim_form_resize_idle_cb, g_object_ref(self));
    }
}

    ਸਥਿਰ ਖਾਲੀ
vim_form_measure(
	GtkWidget	*widget ਅਣਵਰਤਿਆ,
	GtkOrientation	orientation,
	ਪੂਰਨ		for_ਮਾਪ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ		*minimum,
	ਪੂਰਨ		*natural,
	ਪੂਰਨ		*minimum_baseline,
	ਪੂਰਨ		*natural_baseline)
{
    ਜੇ (orientation == GTK_ORIENTATION_VERTICAL)
    {
	// Set minimum height of form widget to 4 rows.
	ਜੇ (minimum != ਨਿਰਾ)
	    *minimum  = gui.char_ਉਚਾਈ * 4;
	ਜੇ (natural != ਨਿਰਾ)
	    *natural = gui.char_ਉਚਾਈ * 4;
    }
    ਨਹੀਂਤਾਂ
    {
	// Set minimum width of form widget to 20 columns. Any less and the draw
	// area seems to glitch out...
	ਜੇ (minimum != ਨਿਰਾ)
	    *minimum  = gui.char_ਚੌੜਾਈ * 20;
	ਜੇ (natural != ਨਿਰਾ)
	    *natural = gui.char_ਚੌੜਾਈ * 20;
    }

    ਜੇ (minimum_baseline != ਨਿਰਾ)
	*minimum_baseline = -1;
    ਜੇ (natural_baseline != ਨਿਰਾ)
	*natural_baseline = -1;
}


/*
 * Make the form itself input-transparent so clicks on its empty area fall
 * through to the drawarea below, while the scrollbar children stay pickable.
 */
    ਸਥਿਰ gboolean
vim_form_ਸ਼ਾਮਲ_ਹੈ(GtkWidget *widget ਅਣਵਰਤਿਆ, ਦੁੱਗਣਾ x ਅਣਵਰਤਿਆ, ਦੁੱਗਣਾ y ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ FALSE;
}
