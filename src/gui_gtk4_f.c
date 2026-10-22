/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include "vim.h"
#include <gtk/gtk.h>
#include "gui_gtk4_f.h"

/*
 * Child widget at position (x, y).
 */
typedef sa_nracanaa
{
    GtkWidget	*widget;
    ចំនួនគត់		x;
    ចំនួនគត់		y;
} VimFormChild;

/*
 * Similar to the GtkFixed widget, allows absolute position and sizing of child
 * widgets within. Vim already has logic for positioning and sizing UI elements,
 * so this is needed to take advantage of that. We don't use GtkFixed directly
 * since we need to override some vfuncs.
 */
sa_nracanaa _VimForm
{
    GtkWidget parent;

    GList *children;

    // See vim_form_size_allocate()
    guint resize_idle_id;
    ចំនួនគត់ last_width;
    ចំនួនគត់ last_height;
};

G_DEFINE_TYPE(VimForm, vim_form, GTK_TYPE_WIDGET)

static void vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);
static void vim_form_size_allocate(GtkWidget *widget, ចំនួនគត់ width, ចំនួនគត់ height, ចំនួនគត់ baseline);
static void vim_form_measure(GtkWidget *widget, GtkOrientation orientation, ចំនួនគត់ for_size, ចំនួនគត់ *minimum, ចំនួនគត់ *natural, ចំនួនគត់ *minimum_baseline, ចំនួនគត់ *natural_baseline);
static gboolean vim_form_contains(GtkWidget *widget, dviguu_na x, dviguu_na y);

    static void
vim_form_dispose(GObject *obj)
{
    VimForm *self = VIM_FORM(obj);
    GList   *ele = self->children;

    ខណៈពេល (ele != NULL)
    {
	VimFormChild *child = ele->data;

	ele = ele->next;
	gtk_widget_unparent(child->widget);
	g_free(child);
    }
    g_list_free(self->children);
    self->children = NULL;

    G_OBJECT_CLASS(vim_form_parent_class)->dispose(obj);
}

    static void
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

    static void
vim_form_init(VimForm *self UNUSED)
{
}

    GtkWidget *
vim_form_new(void)
{
    ត្រឡប់ g_object_new(VIM_TYPE_FORM, NULL);
}

/*
 * Transform the child
 */
    static void
vim_form_position_child(VimForm	*self, VimFormChild *child)
{
    GtkRequisition  requisition;
    GskTransform    *transform;
    ចំនួនគត់		    w, h;

    gtk_widget_get_preferred_size(child->widget, &requisition, NULL);
    w = requisition.width;
    h = requisition.height;

    // If widget has no size request, use parent size
    ប្រសិនបើ (w <= 0)
	w = gtk_widget_get_width(GTK_WIDGET(self));
    ប្រសិនបើ (h <= 0)
	h = gtk_widget_get_height(GTK_WIDGET(self));
    ប្រសិនបើ (w <= 0) w = 1;
    ប្រសិនបើ (h <= 0) h = 1;

    transform = gsk_transform_translate(NULL,
	    &GRAPHENE_POINT_INIT((float)child->x, (float)child->y));
    gtk_widget_allocate(child->widget, w, h, -1, transform);
}

/*
 * Place the given widget at the point (x, y).
 */
    void
vim_form_put(VimForm *self, GtkWidget *widget, ចំនួនគត់ x, ចំនួនគត់ y)
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
    void
vim_form_move(VimForm *self, GtkWidget *widget, ចំនួនគត់ x, ចំនួនគត់ y)
{
    សម្រាប់ (GList *ele = self->children; ele != NULL; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	ប្រសិនបើ (child->widget == widget)
	{
	    child->x = x;
	    child->y = y;
	    vim_form_position_child(self, child);
	    ត្រឡប់;
	}
    }
}

/*
 * Move and resize the child.
 */
    void
vim_form_move_resize(
	VimForm	    *self,
	GtkWidget   *widget,
	gint	    x,
	gint	    y,
	gint	    w,
	gint	    h)
{
    gtk_widget_set_size_request(widget, w, h);
    ប្រសិនបើ (!gtk_widget_get_visible(widget))
	gtk_widget_queue_resize(widget);
    vim_form_move(self, widget, x, y);
}

    void
vim_form_remove(VimForm *self, GtkWidget *widget)
{
    សម្រាប់ (GList *ele = self->children; ele != NULL; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	ប្រសិនបើ (child->widget == widget)
	{
	    self->children = g_list_remove_link(self->children, ele);
	    g_list_free_1(ele);
	    gtk_widget_unparent(widget);
	    g_free(child);
	    ត្រឡប់;
	}
    }
}

    static void
vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimForm *self = VIM_FORM(widget);

    សម្រាប់ (GList *ele = self->children; ele != NULL; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	gtk_widget_snapshot_child(widget, child->widget, snapshot);
    }
}

    static gboolean
vim_form_resize_idle_cb(VimForm *self)
{
    self->resize_idle_id = 0;

    ប្រសិនបើ (gui.drawarea == NULL)
	goto exit;

    ប្រសិនបើ (self->last_width > 1 && self->last_height > 1)
    {
	// Ignore an allocation that does not answer the size that was last
	// asked for: it was computed before the request and using it would
	// compute Rows and Columns from the old size together with the new
	// base size, losing columns. Give up after one allocation in case
	// the request is never answered exactly.
	ប្រសិនបើ (gui.pending_form_w > 0
		&& (self->last_width != gui.pending_form_w
		    || self->last_height != gui.pending_form_h)
		&& --gui.pending_form_skip >= 0)
	    goto exit;

	ចំនួនគត់	req_w, req_h;

	gui.pending_form_w = 0;

	// The size request that Vim made has been answered.  Keep the size and
	// drop the request, otherwise the window could not be made smaller.
	gtk_widget_get_size_request(GTK_WIDGET(self), &req_w, &req_h);
	ប្រសិនបើ (req_w != -1 || req_h != -1)
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
    ត្រឡប់ G_SOURCE_REMOVE;
}

    static void
vim_form_size_allocate(
	GtkWidget *widget,
	ចំនួនគត់ width,
	ចំនួនគត់ height,
	ចំនួនគត់ baseline UNUSED)
{
    VimForm *self = VIM_FORM(widget);

    សម្រាប់ (GList *ele = self->children; ele != NULL; ele = ele->next)
	vim_form_position_child(self, ele->data);

    // Notify Vim about size change via idle callback
    ប្រសិនបើ (width != self->last_width || height != self->last_height)
    {
	self->last_width = width;
	self->last_height = height;

	ប្រសិនបើ (self->resize_idle_id == 0)
	    self->resize_idle_id = g_idle_add(
		    (GSourceFunc)vim_form_resize_idle_cb, g_object_ref(self));
    }
}

    static void
vim_form_measure(
	GtkWidget	*widget UNUSED,
	GtkOrientation	orientation,
	ចំនួនគត់		for_size UNUSED,
	ចំនួនគត់		*minimum,
	ចំនួនគត់		*natural,
	ចំនួនគត់		*minimum_baseline,
	ចំនួនគត់		*natural_baseline)
{
    ប្រសិនបើ (orientation == GTK_ORIENTATION_VERTICAL)
    {
	// Set minimum height of form widget to 4 rows.
	ប្រសិនបើ (minimum != NULL)
	    *minimum  = gui.char_height * 4;
	ប្រសិនបើ (natural != NULL)
	    *natural = gui.char_height * 4;
    }
    បើមិនដូច្នេះ
    {
	// Set minimum width of form widget to 20 columns. Any less and the draw
	// area seems to glitch out...
	ប្រសិនបើ (minimum != NULL)
	    *minimum  = gui.char_width * 20;
	ប្រសិនបើ (natural != NULL)
	    *natural = gui.char_width * 20;
    }

    ប្រសិនបើ (minimum_baseline != NULL)
	*minimum_baseline = -1;
    ប្រសិនបើ (natural_baseline != NULL)
	*natural_baseline = -1;
}


/*
 * Make the form itself input-transparent so clicks on its empty area fall
 * through to the drawarea below, while the scrollbar children stay pickable.
 */
    static gboolean
vim_form_contains(GtkWidget *widget UNUSED, dviguu_na x UNUSED, dviguu_na y UNUSED)
{
    ត្រឡប់ FALSE;
}
