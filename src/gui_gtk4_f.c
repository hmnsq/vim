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
} VimFormChild;

/*
 * Similar to the GtkFixed widget, allows absolute position and sizing of child
 * widgets within. Vim already has logic for positioning and sizing UI elements,
 * so this is needed to take advantage of that. We don't use GtkFixed directly
 * since we need to override some vfuncs.
 */
بنية _VimForm
{
    GtkWidget parent;

    GList *children;

    // See vim_form_size_allocate()
    guint resize_idle_id;
    صحيح last_width;
    صحيح last_height;
};

G_DEFINE_TYPE(VimForm, vim_form, GTK_TYPE_WIDGET)

ساكن فراغ vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);
ساكن فراغ vim_form_size_allocate(GtkWidget *widget, صحيح width, صحيح height, صحيح baseline);
ساكن فراغ vim_form_measure(GtkWidget *widget, GtkOrientation orientation, صحيح for_size, صحيح *minimum, صحيح *natural, صحيح *minimum_baseline, صحيح *natural_baseline);
ساكن gboolean vim_form_contains(GtkWidget *widget, مزدوج x, مزدوج y);

    ساكن فراغ
vim_form_dispose(GObject *obj)
{
    VimForm *self = VIM_FORM(obj);
    GList   *ele = self->children;

    כלעוד (ele != عدم)
    {
	VimFormChild *child = ele->data;

	ele = ele->next;
	gtk_widget_unparent(child->widget);
	g_free(child);
    }
    g_list_free(self->children);
    self->children = عدم;

    G_OBJECT_CLASS(vim_form_parent_class)->dispose(obj);
}

    ساكن فراغ
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

    ساكن فراغ
vim_form_init(VimForm *self UNUSED)
{
}

    GtkWidget *
vim_form_new(فراغ)
{
    החזר g_object_new(VIM_TYPE_FORM, عدم);
}

/*
 * Transform the child
 */
    ساكن فراغ
vim_form_position_child(VimForm	*self, VimFormChild *child)
{
    GtkRequisition  requisition;
    GskTransform    *transform;
    صحيح		    w, h;

    gtk_widget_get_preferred_size(child->widget, &requisition, عدم);
    w = requisition.width;
    h = requisition.height;

    // If widget has no size request, use parent size
    אם (w <= 0)
	w = gtk_widget_get_width(GTK_WIDGET(self));
    אם (h <= 0)
	h = gtk_widget_get_height(GTK_WIDGET(self));
    אם (w <= 0) w = 1;
    אם (h <= 0) h = 1;

    transform = gsk_transform_translate(عدم,
	    &GRAPHENE_POINT_INIT((عائم)child->x, (عائم)child->y));
    gtk_widget_allocate(child->widget, w, h, -1, transform);
}

/*
 * Place the given widget at the point (x, y).
 */
    فراغ
vim_form_put(VimForm *self, GtkWidget *widget, صحيح x, صحيح y)
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
    فراغ
vim_form_move(VimForm *self, GtkWidget *widget, صحيح x, صحيح y)
{
    עבור (GList *ele = self->children; ele != عدم; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	אם (child->widget == widget)
	{
	    child->x = x;
	    child->y = y;
	    vim_form_position_child(self, child);
	    החזר;
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
    gtk_widget_set_size_request(widget, w, h);
    אם (!gtk_widget_get_visible(widget))
	gtk_widget_queue_resize(widget);
    vim_form_move(self, widget, x, y);
}

    فراغ
vim_form_remove(VimForm *self, GtkWidget *widget)
{
    עבור (GList *ele = self->children; ele != عدم; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	אם (child->widget == widget)
	{
	    self->children = g_list_remove_link(self->children, ele);
	    g_list_free_1(ele);
	    gtk_widget_unparent(widget);
	    g_free(child);
	    החזר;
	}
    }
}

    ساكن فراغ
vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimForm *self = VIM_FORM(widget);

    עבור (GList *ele = self->children; ele != عدم; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	gtk_widget_snapshot_child(widget, child->widget, snapshot);
    }
}

    ساكن gboolean
vim_form_resize_idle_cb(VimForm *self)
{
    self->resize_idle_id = 0;

    אם (gui.drawarea == عدم)
	اذهب exit;

    אם (self->last_width > 1 && self->last_height > 1)
    {
	// Ignore an allocation that does not answer the size that was last
	// asked for: it was computed before the request and using it would
	// compute Rows and Columns from the old size together with the new
	// base size, losing columns. Give up after one allocation in case
	// the request is never answered exactly.
	אם (gui.pending_form_w > 0
		&& (self->last_width != gui.pending_form_w
		    || self->last_height != gui.pending_form_h)
		&& --gui.pending_form_skip >= 0)
	    اذهب exit;

	صحيح	req_w, req_h;

	gui.pending_form_w = 0;

	// The size request that Vim made has been answered.  Keep the size and
	// drop the request, otherwise the window could not be made smaller.
	gtk_widget_get_size_request(GTK_WIDGET(self), &req_w, &req_h);
	אם (req_w != -1 || req_h != -1)
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
    החזר G_SOURCE_REMOVE;
}

    ساكن فراغ
vim_form_size_allocate(
	GtkWidget *widget,
	صحيح width,
	صحيح height,
	صحيح baseline UNUSED)
{
    VimForm *self = VIM_FORM(widget);

    עבור (GList *ele = self->children; ele != عدم; ele = ele->next)
	vim_form_position_child(self, ele->data);

    // Notify Vim about size change via idle callback
    אם (width != self->last_width || height != self->last_height)
    {
	self->last_width = width;
	self->last_height = height;

	אם (self->resize_idle_id == 0)
	    self->resize_idle_id = g_idle_add(
		    (GSourceFunc)vim_form_resize_idle_cb, g_object_ref(self));
    }
}

    ساكن فراغ
vim_form_measure(
	GtkWidget	*widget UNUSED,
	GtkOrientation	orientation,
	صحيح		for_size UNUSED,
	صحيح		*minimum,
	صحيح		*natural,
	صحيح		*minimum_baseline,
	صحيح		*natural_baseline)
{
    אם (orientation == GTK_ORIENTATION_VERTICAL)
    {
	// Set minimum height of form widget to 4 rows.
	אם (minimum != عدم)
	    *minimum  = gui.char_height * 4;
	אם (natural != عدم)
	    *natural = gui.char_height * 4;
    }
    אחרת
    {
	// Set minimum width of form widget to 20 columns. Any less and the draw
	// area seems to glitch out...
	אם (minimum != عدم)
	    *minimum  = gui.char_width * 20;
	אם (natural != عدم)
	    *natural = gui.char_width * 20;
    }

    אם (minimum_baseline != عدم)
	*minimum_baseline = -1;
    אם (natural_baseline != عدم)
	*natural_baseline = -1;
}


/*
 * Make the form itself input-transparent so clicks on its empty area fall
 * through to the drawarea below, while the scrollbar children stay pickable.
 */
    ساكن gboolean
vim_form_contains(GtkWidget *widget UNUSED, مزدوج x UNUSED, مزدوج y UNUSED)
{
    החזר FALSE;
}
