/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ <gtk/gtk.h>
#અન્તર્ભાવ "gui_gtk4_f.h"

/*
 * Child widget at position (x, y).
 */
પ્રકારવ્યાખ્યા સંરચના
{
    GtkWidget	*widget;
    પૂર્ણાંક		x;
    પૂર્ણાંક		y;
} VimFormChild;

/*
 * Similar to the GtkFixed widget, allows absolute position and sizing of child
 * widgets within. Vim already has logic for positioning and sizing UI elements,
 * so this is needed to take advantage of that. We don't use GtkFixed directly
 * since we need to override some vfuncs.
 */
સંરચના _VimForm
{
    GtkWidget parent;

    GList *children;

    // See vim_form_size_allocate()
    guint resize_idle_id;
    પૂર્ણાંક last_width;
    પૂર્ણાંક last_height;
};

G_DEFINE_TYPE(VimForm, vim_form, GTK_TYPE_WIDGET)

સ્થિર શૂન્ય vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);
સ્થિર શૂન્ય vim_form_size_allocate(GtkWidget *widget, પૂર્ણાંક width, પૂર્ણાંક height, પૂર્ણાંક baseline);
સ્થિર શૂન્ય vim_form_measure(GtkWidget *widget, GtkOrientation orientation, પૂર્ણાંક for_size, પૂર્ણાંક *minimum, પૂર્ણાંક *natural, પૂર્ણાંક *minimum_baseline, પૂર્ણાંક *natural_baseline);
સ્થિર gboolean vim_form_contains(GtkWidget *widget, દ્વિગુણ x, દ્વિગુણ y);

    સ્થિર શૂન્ય
vim_form_dispose(GObject *obj)
{
    VimForm *self = VIM_FORM(obj);
    GList   *ele = self->children;

    જ્યારે (ele != ખાલી)
    {
	VimFormChild *child = ele->data;

	ele = ele->next;
	gtk_widget_unparent(child->widget);
	g_free(child);
    }
    g_list_free(self->children);
    self->children = ખાલી;

    G_OBJECT_CLASS(vim_form_parent_class)->dispose(obj);
}

    સ્થિર શૂન્ય
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

    સ્થિર શૂન્ય
vim_form_init(VimForm *self UNUSED)
{
}

    GtkWidget *
vim_form_new(શૂન્ય)
{
    પાછા g_object_new(VIM_TYPE_FORM, ખાલી);
}

/*
 * Transform the child
 */
    સ્થિર શૂન્ય
vim_form_position_child(VimForm	*self, VimFormChild *child)
{
    GtkRequisition  requisition;
    GskTransform    *transform;
    પૂર્ણાંક		    w, h;

    gtk_widget_get_preferred_size(child->widget, &requisition, ખાલી);
    w = requisition.width;
    h = requisition.height;

    // If widget has no size request, use parent size
    જો (w <= 0)
	w = gtk_widget_get_width(GTK_WIDGET(self));
    જો (h <= 0)
	h = gtk_widget_get_height(GTK_WIDGET(self));
    જો (w <= 0) w = 1;
    જો (h <= 0) h = 1;

    transform = gsk_transform_translate(ખાલી,
	    &GRAPHENE_POINT_INIT((દશાંશ)child->x, (દશાંશ)child->y));
    gtk_widget_allocate(child->widget, w, h, -1, transform);
}

/*
 * Place the given widget at the point (x, y).
 */
    શૂન્ય
vim_form_put(VimForm *self, GtkWidget *widget, પૂર્ણાંક x, પૂર્ણાંક y)
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
    શૂન્ય
vim_form_move(VimForm *self, GtkWidget *widget, પૂર્ણાંક x, પૂર્ણાંક y)
{
    માટે (GList *ele = self->children; ele != ખાલી; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	જો (child->widget == widget)
	{
	    child->x = x;
	    child->y = y;
	    vim_form_position_child(self, child);
	    પાછા;
	}
    }
}

/*
 * Move and resize the child.
 */
    શૂન્ય
vim_form_move_resize(
	VimForm	    *self,
	GtkWidget   *widget,
	gint	    x,
	gint	    y,
	gint	    w,
	gint	    h)
{
    gtk_widget_set_size_request(widget, w, h);
    જો (!gtk_widget_get_visible(widget))
	gtk_widget_queue_resize(widget);
    vim_form_move(self, widget, x, y);
}

    શૂન્ય
vim_form_remove(VimForm *self, GtkWidget *widget)
{
    માટે (GList *ele = self->children; ele != ખાલી; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	જો (child->widget == widget)
	{
	    self->children = g_list_remove_link(self->children, ele);
	    g_list_free_1(ele);
	    gtk_widget_unparent(widget);
	    g_free(child);
	    પાછા;
	}
    }
}

    સ્થિર શૂન્ય
vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimForm *self = VIM_FORM(widget);

    માટે (GList *ele = self->children; ele != ખાલી; ele = ele->next)
    {
	VimFormChild *child = ele->data;
	gtk_widget_snapshot_child(widget, child->widget, snapshot);
    }
}

    સ્થિર gboolean
vim_form_resize_idle_cb(VimForm *self)
{
    self->resize_idle_id = 0;

    જો (gui.drawarea == ખાલી)
	જાઓ exit;

    જો (self->last_width > 1 && self->last_height > 1)
    {
	// Ignore an allocation that does not answer the size that was last
	// asked for: it was computed before the request and using it would
	// compute Rows and Columns from the old size together with the new
	// base size, losing columns. Give up after one allocation in case
	// the request is never answered exactly.
	જો (gui.pending_form_w > 0
		&& (self->last_width != gui.pending_form_w
		    || self->last_height != gui.pending_form_h)
		&& --gui.pending_form_skip >= 0)
	    જાઓ exit;

	પૂર્ણાંક	req_w, req_h;

	gui.pending_form_w = 0;

	// The size request that Vim made has been answered.  Keep the size and
	// drop the request, otherwise the window could not be made smaller.
	gtk_widget_get_size_request(GTK_WIDGET(self), &req_w, &req_h);
	જો (req_w != -1 || req_h != -1)
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
    પાછા G_SOURCE_REMOVE;
}

    સ્થિર શૂન્ય
vim_form_size_allocate(
	GtkWidget *widget,
	પૂર્ણાંક width,
	પૂર્ણાંક height,
	પૂર્ણાંક baseline UNUSED)
{
    VimForm *self = VIM_FORM(widget);

    માટે (GList *ele = self->children; ele != ખાલી; ele = ele->next)
	vim_form_position_child(self, ele->data);

    // Notify Vim about size change via idle callback
    જો (width != self->last_width || height != self->last_height)
    {
	self->last_width = width;
	self->last_height = height;

	જો (self->resize_idle_id == 0)
	    self->resize_idle_id = g_idle_add(
		    (GSourceFunc)vim_form_resize_idle_cb, g_object_ref(self));
    }
}

    સ્થિર શૂન્ય
vim_form_measure(
	GtkWidget	*widget UNUSED,
	GtkOrientation	orientation,
	પૂર્ણાંક		for_size UNUSED,
	પૂર્ણાંક		*minimum,
	પૂર્ણાંક		*natural,
	પૂર્ણાંક		*minimum_baseline,
	પૂર્ણાંક		*natural_baseline)
{
    જો (orientation == GTK_ORIENTATION_VERTICAL)
    {
	// Set minimum height of form widget to 4 rows.
	જો (minimum != ખાલી)
	    *minimum  = gui.char_height * 4;
	જો (natural != ખાલી)
	    *natural = gui.char_height * 4;
    }
    નહીંતર
    {
	// Set minimum width of form widget to 20 columns. Any less and the draw
	// area seems to glitch out...
	જો (minimum != ખાલી)
	    *minimum  = gui.char_width * 20;
	જો (natural != ખાલી)
	    *natural = gui.char_width * 20;
    }

    જો (minimum_baseline != ખાલી)
	*minimum_baseline = -1;
    જો (natural_baseline != ખાલી)
	*natural_baseline = -1;
}


/*
 * Make the form itself input-transparent so clicks on its empty area fall
 * through to the drawarea below, while the scrollbar children stay pickable.
 */
    સ્થિર gboolean
vim_form_contains(GtkWidget *widget UNUSED, દ્વિગુણ x UNUSED, દ્વિગુણ y UNUSED)
{
    પાછા FALSE;
}
