/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ <gtk/gtk.h>
#ଅନ୍ତର୍ଭାଵ "gui_gtk4_f.h"

/*
 * Child widget at position (x, y).
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    GtkWidget	*widget;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y;
} VimFormChild;

/*
 * Similar to the GtkFixed widget, allows absolute position and sizing of child
 * widgets within. Vim already has logic for positioning and sizing UI elements,
 * so this is needed to take advantage of that. We don't use GtkFixed directly
 * since we need to override some vfuncs.
 */
ସଂରଚନା _VimForm
{
    GtkWidget parent;

    GList *children;

    // See vim_form_size_allocate()
    guint resize_idle_id;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_height;
};

G_DEFINE_TYPE(VimForm, vim_form, GTK_TYPE_WIDGET)

ସ୍ଥିର ଶୂନ୍ୟ vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);
ସ୍ଥିର ଶୂନ୍ୟ vim_form_size_allocate(GtkWidget *widget, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height, ପୂର୍ଣ୍ଣସଂଖ୍ୟା baseline);
ସ୍ଥିର ଶୂନ୍ୟ vim_form_measure(GtkWidget *widget, GtkOrientation orientation, ପୂର୍ଣ୍ଣସଂଖ୍ୟା for_size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *minimum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *natural, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *minimum_baseline, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *natural_baseline);
ସ୍ଥିର gboolean vim_form_contains(GtkWidget *widget, ଦ୍ୱିଗୁଣ x, ଦ୍ୱିଗୁଣ y);

    ସ୍ଥିର ଶୂନ୍ୟ
vim_form_dispose(GObject *obj)
{
    VimForm *self = VIM_FORM(obj);
    GList   *ele = self->children;

    ଯେତେବେଳେ (ele != ଖାଲି)
    {
	VimFormChild *child = ele->data;

	ele = ele->next;
	gtk_widget_unparent(child->widget);
	g_free(child);
    }
    g_list_free(self->children);
    self->children = ଖାଲି;

    G_OBJECT_CLASS(vim_form_parent_class)->dispose(obj);
}

    ସ୍ଥିର ଶୂନ୍ୟ
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

    ସ୍ଥିର ଶୂନ୍ୟ
vim_form_init(VimForm *self UNUSED)
{
}

    GtkWidget *
vim_form_new(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ g_object_new(VIM_TYPE_FORM, ଖାଲି);
}

/*
 * Transform the child
 */
    ସ୍ଥିର ଶୂନ୍ୟ
vim_form_position_child(VimForm	*self, VimFormChild *child)
{
    GtkRequisition  requisition;
    GskTransform    *transform;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    w, h;

    gtk_widget_get_preferred_size(child->widget, &requisition, ଖାଲି);
    w = requisition.width;
    h = requisition.height;

    // If widget has no size request, use parent size
    ଯଦି (w <= 0)
	w = gtk_widget_get_width(GTK_WIDGET(self));
    ଯଦି (h <= 0)
	h = gtk_widget_get_height(GTK_WIDGET(self));
    ଯଦି (w <= 0) w = 1;
    ଯଦି (h <= 0) h = 1;

    transform = gsk_transform_translate(ଖାଲି,
	    &GRAPHENE_POINT_INIT((ଦଶମିକ)child->x, (ଦଶମିକ)child->y));
    gtk_widget_allocate(child->widget, w, h, -1, transform);
}

/*
 * Place the given widget at the point (x, y).
 */
    ଶୂନ୍ୟ
vim_form_put(VimForm *self, GtkWidget *widget, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
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
    ଶୂନ୍ୟ
vim_form_move(VimForm *self, GtkWidget *widget, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
{
    ପାଇଁ (GList *ele = self->children; ele != ଖାଲି; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	ଯଦି (child->widget == widget)
	{
	    child->x = x;
	    child->y = y;
	    vim_form_position_child(self, child);
	    ଫେରନ୍ତୁ;
	}
    }
}

/*
 * Move and resize the child.
 */
    ଶୂନ୍ୟ
vim_form_move_resize(
	VimForm	    *self,
	GtkWidget   *widget,
	gint	    x,
	gint	    y,
	gint	    w,
	gint	    h)
{
    gtk_widget_set_size_request(widget, w, h);
    ଯଦି (!gtk_widget_get_visible(widget))
	gtk_widget_queue_resize(widget);
    vim_form_move(self, widget, x, y);
}

    ଶୂନ୍ୟ
vim_form_remove(VimForm *self, GtkWidget *widget)
{
    ପାଇଁ (GList *ele = self->children; ele != ଖାଲି; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	ଯଦି (child->widget == widget)
	{
	    self->children = g_list_remove_link(self->children, ele);
	    g_list_free_1(ele);
	    gtk_widget_unparent(widget);
	    g_free(child);
	    ଫେରନ୍ତୁ;
	}
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimForm *self = VIM_FORM(widget);

    ପାଇଁ (GList *ele = self->children; ele != ଖାଲି; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	gtk_widget_snapshot_child(widget, child->widget, snapshot);
    }
}

    ସ୍ଥିର gboolean
vim_form_resize_idle_cb(VimForm *self)
{
    self->resize_idle_id = 0;

    ଯଦି (gui.drawarea == ଖାଲି)
	ଯାଅ exit;

    ଯଦି (self->last_width > 1 && self->last_height > 1)
    {
	// Ignore an allocation that does not answer the size that was last
	// asked for: it was computed before the request and using it would
	// compute Rows and Columns from the old size together with the new
	// base size, losing columns. Give up after one allocation in case
	// the request is never answered exactly.
	ଯଦି (gui.pending_form_w > 0
		&& (self->last_width != gui.pending_form_w
		    || self->last_height != gui.pending_form_h)
		&& --gui.pending_form_skip >= 0)
	    ଯାଅ exit;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	req_w, req_h;

	gui.pending_form_w = 0;

	// The size request that Vim made has been answered.  Keep the size and
	// drop the request, otherwise the window could not be made smaller.
	gtk_widget_get_size_request(GTK_WIDGET(self), &req_w, &req_h);
	ଯଦି (req_w != -1 || req_h != -1)
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
    ଫେରନ୍ତୁ G_SOURCE_REMOVE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
vim_form_size_allocate(
	GtkWidget *widget,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା height,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା baseline UNUSED)
{
    VimForm *self = VIM_FORM(widget);

    ପାଇଁ (GList *ele = self->children; ele != ଖାଲି; ele = ele->next)
	vim_form_position_child(self, ele->data);

    // Notify Vim about size change via idle callback
    ଯଦି (width != self->last_width || height != self->last_height)
    {
	self->last_width = width;
	self->last_height = height;

	ଯଦି (self->resize_idle_id == 0)
	    self->resize_idle_id = g_idle_add(
		    (GSourceFunc)vim_form_resize_idle_cb, g_object_ref(self));
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
vim_form_measure(
	GtkWidget	*widget UNUSED,
	GtkOrientation	orientation,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		for_size UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*minimum,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*natural,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*minimum_baseline,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*natural_baseline)
{
    ଯଦି (orientation == GTK_ORIENTATION_VERTICAL)
    {
	// Set minimum height of form widget to 4 rows.
	ଯଦି (minimum != ଖାଲି)
	    *minimum  = gui.char_height * 4;
	ଯଦି (natural != ଖାଲି)
	    *natural = gui.char_height * 4;
    }
    ଅନ୍ୟଥା
    {
	// Set minimum width of form widget to 20 columns. Any less and the draw
	// area seems to glitch out...
	ଯଦି (minimum != ଖାଲି)
	    *minimum  = gui.char_width * 20;
	ଯଦି (natural != ଖାଲି)
	    *natural = gui.char_width * 20;
    }

    ଯଦି (minimum_baseline != ଖାଲି)
	*minimum_baseline = -1;
    ଯଦି (natural_baseline != ଖାଲି)
	*natural_baseline = -1;
}


/*
 * Make the form itself input-transparent so clicks on its empty area fall
 * through to the drawarea below, while the scrollbar children stay pickable.
 */
    ସ୍ଥିର gboolean
vim_form_contains(GtkWidget *widget UNUSED, ଦ୍ୱିଗୁଣ x UNUSED, ଦ୍ୱିଗୁଣ y UNUSED)
{
    ଫେରନ୍ତୁ FALSE;
}
