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
} Vim_Form_ماشوم;

/*
 * Similar to the GtkFixed widget, allows absolute position and sizing of child
 * widgets within. Vim already has logic for positioning and sizing UI elements,
 * so this is needed to take advantage of that. We don't use GtkFixed directly
 * since we need to override some vfuncs.
 */
بنية _VimForm
{
    GtkWidget پلار;

    GList *children;

    // See vim_form_size_allocate()
    guint resize_idle_id;
    صحيح وروستی_پلنوالی;
    صحيح وروستی_لوړوالی;
};

G_DEFINE_TYPE(VimForm, vim_form, GTK_TYPE_WIDGET)

ساكن فراغ vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);
ساكن فراغ vim_form_اندازه_allocate(GtkWidget *widget, صحيح پلنوالی, صحيح لوړوالی, صحيح baseline);
ساكن فراغ vim_form_measure(GtkWidget *widget, GtkOrientation orientation, صحيح for_اندازه, صحيح *minimum, صحيح *natural, صحيح *minimum_baseline, صحيح *natural_baseline);
ساكن gboolean vim_form_لري(GtkWidget *widget, مزدوج x, مزدوج y);

    ساكن فراغ
vim_form_dispose(GObject *obj)
{
    VimForm *self = VIM_FORM(obj);
    GList   *ele = self->children;

    طالما (ele != عدم)
    {
	Vim_Form_ماشوم *ماشوم = ele->معلومات;

	ele = ele->بل;
	gtk_widget_unparent(ماشوم->widget);
	g_free(ماشوم);
    }
    g_list_free(self->children);
    self->children = عدم;

    G_OBJECT_CLASS(vim_form_پلار_class)->dispose(obj);
}

    ساكن فراغ
vim_form_class_init(VimFormClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_form_snapshot;
    widget_class->اندازه_allocate = vim_form_اندازه_allocate;
    widget_class->measure = vim_form_measure;
    widget_class->لري = vim_form_لري;

    obj_class->dispose = vim_form_dispose;
}

    ساكن فراغ
vim_form_init(VimForm *self ناکارول_شوی)
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
vim_form_position_ماشوم(VimForm	*self, Vim_Form_ماشوم *ماشوم)
{
    GtkRequisition  requisition;
    GskTransform    *transform;
    صحيح		    w, h;

    gtk_widget_get_preferred_اندازه(ماشوم->widget, &requisition, عدم);
    w = requisition.پلنوالی;
    h = requisition.لوړوالی;

    // If widget has no size request, use parent size
    إذا (w <= 0)
	w = gtk_widget_get_پلنوالی(GTK_WIDGET(self));
    إذا (h <= 0)
	h = gtk_widget_get_لوړوالی(GTK_WIDGET(self));
    إذا (w <= 0) w = 1;
    إذا (h <= 0) h = 1;

    transform = gsk_transform_translate(عدم,
	    &GRAPHENE_ټکی_INIT((عائم)ماشوم->x, (عائم)ماشوم->y));
    gtk_widget_allocate(ماشوم->widget, w, h, -1, transform);
}

/*
 * Place the given widget at the point (x, y).
 */
    فراغ
vim_form_put(VimForm *self, GtkWidget *widget, صحيح x, صحيح y)
{
    Vim_Form_ماشوم *ماشوم;

    ماشوم = g_new(Vim_Form_ماشوم, 1);

    ماشوم->widget = widget;
    ماشوم->x = x;
    ماشوم->y = y;

    gtk_widget_سېټ_اندازه_request(ماشوم->widget, -1, -1);

    self->children = g_list_append(self->children, ماشوم);

    gtk_widget_سېټ_پلار(widget, GTK_WIDGET(self));
    vim_form_position_ماشوم(self, ماشوم);
}

/*
 * Move the widget (which should have already been added using vim_form_put())
 * to the given point (x, y).
 */
    فراغ
vim_form_move(VimForm *self, GtkWidget *widget, صحيح x, صحيح y)
{
    لكل (GList *ele = self->children; ele != عدم; ele = ele->بل)
    {
	Vim_Form_ماشوم *ماشوم = ele->معلومات;
	إذا (ماشوم->widget == widget)
	{
	    ماشوم->x = x;
	    ماشوم->y = y;
	    vim_form_position_ماشوم(self, ماشوم);
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
    gtk_widget_سېټ_اندازه_request(widget, w, h);
    إذا (!gtk_widget_get_visible(widget))
	gtk_widget_کتار_resize(widget);
    vim_form_move(self, widget, x, y);
}

    فراغ
vim_form_remove(VimForm *self, GtkWidget *widget)
{
    لكل (GList *ele = self->children; ele != عدم; ele = ele->بل)
    {
	Vim_Form_ماشوم *ماشوم = ele->معلومات;
	إذا (ماشوم->widget == widget)
	{
	    self->children = g_list_remove_link(self->children, ele);
	    g_list_free_1(ele);
	    gtk_widget_unparent(widget);
	    g_free(ماشوم);
	    أرجع;
	}
    }
}

    ساكن فراغ
vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimForm *self = VIM_FORM(widget);

    لكل (GList *ele = self->children; ele != عدم; ele = ele->بل)
    {
	Vim_Form_ماشوم *ماشوم = ele->معلومات;
	gtk_widget_snapshot_ماشوم(widget, ماشوم->widget, snapshot);
    }
}

    ساكن gboolean
vim_form_resize_idle_cb(VimForm *self)
{
    self->resize_idle_id = 0;

    إذا (gui.drawarea == عدم)
	اذهب وتل;

    إذا (self->وروستی_پلنوالی > 1 && self->وروستی_لوړوالی > 1)
    {
	// Ignore an allocation that does not answer the size that was last
	// asked for: it was computed before the request and using it would
	// compute Rows and Columns from the old size together with the new
	// base size, losing columns. Give up after one allocation in case
	// the request is never answered exactly.
	إذا (gui.pending_form_w > 0
		&& (self->وروستی_پلنوالی != gui.pending_form_w
		    || self->وروستی_لوړوالی != gui.pending_form_h)
		&& --gui.pending_form_skip >= 0)
	    اذهب وتل;

	صحيح	req_w, req_h;

	gui.pending_form_w = 0;

	// The size request that Vim made has been answered.  Keep the size and
	// drop the request, otherwise the window could not be made smaller.
	gtk_widget_get_اندازه_request(GTK_WIDGET(self), &req_w, &req_h);
	إذا (req_w != -1 || req_h != -1)
	{
	    gtk_کړکۍ_سېټ_default_اندازه(GTK_کړکۍ(gui.mainwin),
		    gtk_widget_get_پلنوالی(gui.mainwin),
		    gtk_widget_get_لوړوالی(gui.mainwin));
	    gtk_widget_سېټ_اندازه_request(GTK_WIDGET(self), -1, -1);
	}

	gui_resize_shell(self->وروستی_پلنوالی, self->وروستی_لوړوالی);
    }

وتل:
    g_object_unref(self);
    أرجع G_سرچینه_REMOVE;
}

    ساكن فراغ
vim_form_اندازه_allocate(
	GtkWidget *widget,
	صحيح پلنوالی,
	صحيح لوړوالی,
	صحيح baseline ناکارول_شوی)
{
    VimForm *self = VIM_FORM(widget);

    لكل (GList *ele = self->children; ele != عدم; ele = ele->بل)
	vim_form_position_ماشوم(self, ele->معلومات);

    // Notify Vim about size change via idle callback
    إذا (پلنوالی != self->وروستی_پلنوالی || لوړوالی != self->وروستی_لوړوالی)
    {
	self->وروستی_پلنوالی = پلنوالی;
	self->وروستی_لوړوالی = لوړوالی;

	إذا (self->resize_idle_id == 0)
	    self->resize_idle_id = g_idle_جمع_کړه(
		    (GSourceFunc)vim_form_resize_idle_cb, g_object_ref(self));
    }
}

    ساكن فراغ
vim_form_measure(
	GtkWidget	*widget ناکارول_شوی,
	GtkOrientation	orientation,
	صحيح		for_اندازه ناکارول_شوی,
	صحيح		*minimum,
	صحيح		*natural,
	صحيح		*minimum_baseline,
	صحيح		*natural_baseline)
{
    إذا (orientation == GTK_ORIENTATION_VERTICAL)
    {
	// Set minimum height of form widget to 4 rows.
	إذا (minimum != عدم)
	    *minimum  = gui.char_لوړوالی * 4;
	إذا (natural != عدم)
	    *natural = gui.char_لوړوالی * 4;
    }
    وإلا
    {
	// Set minimum width of form widget to 20 columns. Any less and the draw
	// area seems to glitch out...
	إذا (minimum != عدم)
	    *minimum  = gui.char_پلنوالی * 20;
	إذا (natural != عدم)
	    *natural = gui.char_پلنوالی * 20;
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
vim_form_لري(GtkWidget *widget ناکارول_شوی, مزدوج x ناکارول_شوی, مزدوج y ناکارول_شوی)
{
    أرجع FALSE;
}
