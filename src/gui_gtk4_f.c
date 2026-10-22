/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#شامل "vim.h"
#شامل <gtk/gtk.h>
#شامل "gui_gtk4_f.h"

/*
 * Child widget at position (x, y).
 */
تعریفنوع ساختار
{
    GtkWidget	*widget;
    صحیح		x;
    صحیح		y;
} VimFormChild;

/*
 * Similar to the GtkFixed widget, allows absolute position and sizing of child
 * widgets within. Vim already has logic for positioning and sizing UI elements,
 * so this is needed to take advantage of that. We don't use GtkFixed directly
 * since we need to override some vfuncs.
 */
ساختار _VimForm
{
    GtkWidget parent;

    GList *children;

    // See vim_form_size_allocate()
    guint resize_idle_id;
    صحیح last_width;
    صحیح last_height;
};

G_DEFINE_TYPE(VimForm, vim_form, GTK_TYPE_WIDGET)

ایستا تهی vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);
ایستا تهی vim_form_size_allocate(GtkWidget *widget, صحیح width, صحیح height, صحیح baseline);
ایستا تهی vim_form_measure(GtkWidget *widget, GtkOrientation orientation, صحیح for_size, صحیح *minimum, صحیح *natural, صحیح *minimum_baseline, صحیح *natural_baseline);
ایستا gboolean vim_form_contains(GtkWidget *widget, دوگانه x, دوگانه y);

    ایستا تهی
vim_form_dispose(GObject *obj)
{
    VimForm *self = VIM_FORM(obj);
    GList   *ele = self->children;

    وقتی (ele != پوچ)
    {
	VimFormChild *child = ele->data;

	ele = ele->next;
	gtk_widget_unparent(child->widget);
	g_free(child);
    }
    g_list_free(self->children);
    self->children = پوچ;

    G_OBJECT_CLASS(vim_form_parent_class)->dispose(obj);
}

    ایستا تهی
vim_form_class_init(VimFormClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_form_snapshot;
    widget_class->size_allocate = vim_form_size_allocate;
    widget_class->measure = vim_form_measure;
    widget_class->contains = vim_form_contains;

    obj_class->dispose = vim_form_dispose;
}

    ایستا تهی
vim_form_init(VimForm *self UNUSED)
{
}

    GtkWidget *
vim_form_new(تهی)
{
    بازگشت g_object_new(VIM_TYPE_FORM, پوچ);
}

/*
 * Transform the child
 */
    ایستا تهی
vim_form_position_child(VimForm	*self, VimFormChild *child)
{
    GtkRequisition  requisition;
    GskTransform    *transform;
    صحیح		    w, h;

    gtk_widget_get_preferred_size(child->widget, &requisition, پوچ);
    w = requisition.width;
    h = requisition.height;

    // If widget has no size request, use parent size
    اگر (w <= 0)
	w = gtk_widget_get_width(GTK_WIDGET(self));
    اگر (h <= 0)
	h = gtk_widget_get_height(GTK_WIDGET(self));
    اگر (w <= 0) w = 1;
    اگر (h <= 0) h = 1;

    transform = gsk_transform_translate(پوچ,
	    &GRAPHENE_POINT_INIT((اعشاری)child->x, (اعشاری)child->y));
    gtk_widget_allocate(child->widget, w, h, -1, transform);
}

/*
 * Place the given widget at the point (x, y).
 */
    تهی
vim_form_put(VimForm *self, GtkWidget *widget, صحیح x, صحیح y)
{
    VimFormChild *child;

    child = g_new(VimFormChild, 1);

    child->widget = widget;
    child->x = x;
    child->y = y;

    gtk_widget_set_size_request(child->widget, -1, -1);

    self->children = g_list_append(self->children, child);

    gtk_widget_set_parent(widget, GTK_WIDGET(self));
    vim_form_position_child(self, child);
}

/*
 * Move the widget (which should have already been added using vim_form_put())
 * to the given point (x, y).
 */
    تهی
vim_form_move(VimForm *self, GtkWidget *widget, صحیح x, صحیح y)
{
    برای (GList *ele = self->children; ele != پوچ; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	اگر (child->widget == widget)
	{
	    child->x = x;
	    child->y = y;
	    vim_form_position_child(self, child);
	    بازگشت;
	}
    }
}

/*
 * Move and resize the child.
 */
    تهی
vim_form_move_resize(
	VimForm	    *self,
	GtkWidget   *widget,
	gint	    x,
	gint	    y,
	gint	    w,
	gint	    h)
{
    gtk_widget_set_size_request(widget, w, h);
    اگر (!gtk_widget_get_visible(widget))
	gtk_widget_queue_resize(widget);
    vim_form_move(self, widget, x, y);
}

    تهی
vim_form_remove(VimForm *self, GtkWidget *widget)
{
    برای (GList *ele = self->children; ele != پوچ; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	اگر (child->widget == widget)
	{
	    self->children = g_list_remove_link(self->children, ele);
	    g_list_free_1(ele);
	    gtk_widget_unparent(widget);
	    g_free(child);
	    بازگشت;
	}
    }
}

    ایستا تهی
vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimForm *self = VIM_FORM(widget);

    برای (GList *ele = self->children; ele != پوچ; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	gtk_widget_snapshot_child(widget, child->widget, snapshot);
    }
}

    ایستا gboolean
vim_form_resize_idle_cb(VimForm *self)
{
    self->resize_idle_id = 0;

    اگر (gui.drawarea == پوچ)
	برو exit;

    اگر (self->last_width > 1 && self->last_height > 1)
    {
	// Ignore an allocation that does not answer the size that was last
	// asked for: it was computed before the request and using it would
	// compute Rows and Columns from the old size together with the new
	// base size, losing columns. Give up after one allocation in case
	// the request is never answered exactly.
	اگر (gui.pending_form_w > 0
		&& (self->last_width != gui.pending_form_w
		    || self->last_height != gui.pending_form_h)
		&& --gui.pending_form_skip >= 0)
	    برو exit;

	صحیح	req_w, req_h;

	gui.pending_form_w = 0;

	// The size request that Vim made has been answered.  Keep the size and
	// drop the request, otherwise the window could not be made smaller.
	gtk_widget_get_size_request(GTK_WIDGET(self), &req_w, &req_h);
	اگر (req_w != -1 || req_h != -1)
	{
	    gtk_window_set_default_size(GTK_WINDOW(gui.mainwin),
		    gtk_widget_get_width(gui.mainwin),
		    gtk_widget_get_height(gui.mainwin));
	    gtk_widget_set_size_request(GTK_WIDGET(self), -1, -1);
	}

	gui_resize_shell(self->last_width, self->last_height);
    }

exit:
    g_object_unref(self);
    بازگشت G_SOURCE_REMOVE;
}

    ایستا تهی
vim_form_size_allocate(
	GtkWidget *widget,
	صحیح width,
	صحیح height,
	صحیح baseline UNUSED)
{
    VimForm *self = VIM_FORM(widget);

    برای (GList *ele = self->children; ele != پوچ; ele = ele->next)
	vim_form_position_child(self, ele->data);

    // Notify Vim about size change via idle callback
    اگر (width != self->last_width || height != self->last_height)
    {
	self->last_width = width;
	self->last_height = height;

	اگر (self->resize_idle_id == 0)
	    self->resize_idle_id = g_idle_add(
		    (GSourceFunc)vim_form_resize_idle_cb, g_object_ref(self));
    }
}

    ایستا تهی
vim_form_measure(
	GtkWidget	*widget UNUSED,
	GtkOrientation	orientation,
	صحیح		for_size UNUSED,
	صحیح		*minimum,
	صحیح		*natural,
	صحیح		*minimum_baseline,
	صحیح		*natural_baseline)
{
    اگر (orientation == GTK_ORIENTATION_VERTICAL)
    {
	// Set minimum height of form widget to 4 rows.
	اگر (minimum != پوچ)
	    *minimum  = gui.char_height * 4;
	اگر (natural != پوچ)
	    *natural = gui.char_height * 4;
    }
    وگرنه
    {
	// Set minimum width of form widget to 20 columns. Any less and the draw
	// area seems to glitch out...
	اگر (minimum != پوچ)
	    *minimum  = gui.char_width * 20;
	اگر (natural != پوچ)
	    *natural = gui.char_width * 20;
    }

    اگر (minimum_baseline != پوچ)
	*minimum_baseline = -1;
    اگر (natural_baseline != پوچ)
	*natural_baseline = -1;
}


/*
 * Make the form itself input-transparent so clicks on its empty area fall
 * through to the drawarea below, while the scrollbar children stay pickable.
 */
    ایستا gboolean
vim_form_contains(GtkWidget *widget UNUSED, دوگانه x UNUSED, دوگانه y UNUSED)
{
    بازگشت FALSE;
}
