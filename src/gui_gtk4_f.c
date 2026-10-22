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
} Vim_Form_بچہ;

/*
 * Similar to the GtkFixed widget, allows absolute position and sizing of child
 * widgets within. Vim already has logic for positioning and sizing UI elements,
 * so this is needed to take advantage of that. We don't use GtkFixed directly
 * since we need to override some vfuncs.
 */
بنية _VimForm
{
    GtkWidget ماپا;

    GList *children;

    // See vim_form_size_allocate()
    guint resize_idle_id;
    صحيح آخری_چوڑائی;
    صحيح آخری_اچائی;
};

G_DEFINE_TYPE(VimForm, vim_form, GTK_TYPE_WIDGET)

ساكن فراغ vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);
ساكن فراغ vim_form_ناپ_allocate(GtkWidget *widget, صحيح چوڑائی, صحيح اچائی, صحيح baseline);
ساكن فراغ vim_form_measure(GtkWidget *widget, GtkOrientation orientation, صحيح for_ناپ, صحيح *minimum, صحيح *natural, صحيح *minimum_baseline, صحيح *natural_baseline);
ساكن gboolean vim_form_شامل_اے(GtkWidget *widget, مزدوج x, مزدوج y);

    ساكن فراغ
vim_form_dispose(GObject *obj)
{
    VimForm *self = VIM_FORM(obj);
    GList   *ele = self->children;

    طالما (ele != عدم)
    {
	Vim_Form_بچہ *بچہ = ele->انکڑے;

	ele = ele->اگلا;
	gtk_widget_unparent(بچہ->widget);
	g_free(بچہ);
    }
    g_list_free(self->children);
    self->children = عدم;

    G_OBJECT_CLASS(vim_form_ماپا_class)->dispose(obj);
}

    ساكن فراغ
vim_form_class_init(VimFormClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_form_snapshot;
    widget_class->ناپ_allocate = vim_form_ناپ_allocate;
    widget_class->measure = vim_form_measure;
    widget_class->شامل_اے = vim_form_شامل_اے;

    obj_class->dispose = vim_form_dispose;
}

    ساكن فراغ
vim_form_init(VimForm *self بن_ورتیا)
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
vim_form_position_بچہ(VimForm	*self, Vim_Form_بچہ *بچہ)
{
    GtkRequisition  requisition;
    GskTransform    *transform;
    صحيح		    w, h;

    gtk_widget_get_preferred_ناپ(بچہ->widget, &requisition, عدم);
    w = requisition.چوڑائی;
    h = requisition.اچائی;

    // If widget has no size request, use parent size
    إذا (w <= 0)
	w = gtk_widget_get_چوڑائی(GTK_WIDGET(self));
    إذا (h <= 0)
	h = gtk_widget_get_اچائی(GTK_WIDGET(self));
    إذا (w <= 0) w = 1;
    إذا (h <= 0) h = 1;

    transform = gsk_transform_translate(عدم,
	    &GRAPHENE_نقطہ_INIT((عائم)بچہ->x, (عائم)بچہ->y));
    gtk_widget_allocate(بچہ->widget, w, h, -1, transform);
}

/*
 * Place the given widget at the point (x, y).
 */
    فراغ
vim_form_put(VimForm *self, GtkWidget *widget, صحيح x, صحيح y)
{
    Vim_Form_بچہ *بچہ;

    بچہ = g_new(Vim_Form_بچہ, 1);

    بچہ->widget = widget;
    بچہ->x = x;
    بچہ->y = y;

    gtk_widget_سیٹ_ناپ_request(بچہ->widget, -1, -1);

    self->children = g_list_append(self->children, بچہ);

    gtk_widget_سیٹ_ماپا(widget, GTK_WIDGET(self));
    vim_form_position_بچہ(self, بچہ);
}

/*
 * Move the widget (which should have already been added using vim_form_put())
 * to the given point (x, y).
 */
    فراغ
vim_form_move(VimForm *self, GtkWidget *widget, صحيح x, صحيح y)
{
    لكل (GList *ele = self->children; ele != عدم; ele = ele->اگلا)
    {
	Vim_Form_بچہ *بچہ = ele->انکڑے;
	إذا (بچہ->widget == widget)
	{
	    بچہ->x = x;
	    بچہ->y = y;
	    vim_form_position_بچہ(self, بچہ);
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
    gtk_widget_سیٹ_ناپ_request(widget, w, h);
    إذا (!gtk_widget_get_visible(widget))
	gtk_widget_قطار_resize(widget);
    vim_form_move(self, widget, x, y);
}

    فراغ
vim_form_remove(VimForm *self, GtkWidget *widget)
{
    لكل (GList *ele = self->children; ele != عدم; ele = ele->اگلا)
    {
	Vim_Form_بچہ *بچہ = ele->انکڑے;
	إذا (بچہ->widget == widget)
	{
	    self->children = g_list_remove_link(self->children, ele);
	    g_list_free_1(ele);
	    gtk_widget_unparent(widget);
	    g_free(بچہ);
	    أرجع;
	}
    }
}

    ساكن فراغ
vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimForm *self = VIM_FORM(widget);

    لكل (GList *ele = self->children; ele != عدم; ele = ele->اگلا)
    {
	Vim_Form_بچہ *بچہ = ele->انکڑے;
	gtk_widget_snapshot_بچہ(widget, بچہ->widget, snapshot);
    }
}

    ساكن gboolean
vim_form_resize_idle_cb(VimForm *self)
{
    self->resize_idle_id = 0;

    إذا (gui.drawarea == عدم)
	اذهب باہر_نکلو;

    إذا (self->آخری_چوڑائی > 1 && self->آخری_اچائی > 1)
    {
	// Ignore an allocation that does not answer the size that was last
	// asked for: it was computed before the request and using it would
	// compute Rows and Columns from the old size together with the new
	// base size, losing columns. Give up after one allocation in case
	// the request is never answered exactly.
	إذا (gui.pending_form_w > 0
		&& (self->آخری_چوڑائی != gui.pending_form_w
		    || self->آخری_اچائی != gui.pending_form_h)
		&& --gui.pending_form_skip >= 0)
	    اذهب باہر_نکلو;

	صحيح	req_w, req_h;

	gui.pending_form_w = 0;

	// The size request that Vim made has been answered.  Keep the size and
	// drop the request, otherwise the window could not be made smaller.
	gtk_widget_get_ناپ_request(GTK_WIDGET(self), &req_w, &req_h);
	إذا (req_w != -1 || req_h != -1)
	{
	    gtk_کھڑکی_سیٹ_default_ناپ(GTK_کھڑکی(gui.mainwin),
		    gtk_widget_get_چوڑائی(gui.mainwin),
		    gtk_widget_get_اچائی(gui.mainwin));
	    gtk_widget_سیٹ_ناپ_request(GTK_WIDGET(self), -1, -1);
	}

	gui_resize_shell(self->آخری_چوڑائی, self->آخری_اچائی);
    }

باہر_نکلو:
    g_object_unref(self);
    أرجع G_سروت_REMOVE;
}

    ساكن فراغ
vim_form_ناپ_allocate(
	GtkWidget *widget,
	صحيح چوڑائی,
	صحيح اچائی,
	صحيح baseline بن_ورتیا)
{
    VimForm *self = VIM_FORM(widget);

    لكل (GList *ele = self->children; ele != عدم; ele = ele->اگلا)
	vim_form_position_بچہ(self, ele->انکڑے);

    // Notify Vim about size change via idle callback
    إذا (چوڑائی != self->آخری_چوڑائی || اچائی != self->آخری_اچائی)
    {
	self->آخری_چوڑائی = چوڑائی;
	self->آخری_اچائی = اچائی;

	إذا (self->resize_idle_id == 0)
	    self->resize_idle_id = g_idle_جوڑو(
		    (GSourceFunc)vim_form_resize_idle_cb, g_object_ref(self));
    }
}

    ساكن فراغ
vim_form_measure(
	GtkWidget	*widget بن_ورتیا,
	GtkOrientation	orientation,
	صحيح		for_ناپ بن_ورتیا,
	صحيح		*minimum,
	صحيح		*natural,
	صحيح		*minimum_baseline,
	صحيح		*natural_baseline)
{
    إذا (orientation == GTK_ORIENTATION_VERTICAL)
    {
	// Set minimum height of form widget to 4 rows.
	إذا (minimum != عدم)
	    *minimum  = gui.char_اچائی * 4;
	إذا (natural != عدم)
	    *natural = gui.char_اچائی * 4;
    }
    وإلا
    {
	// Set minimum width of form widget to 20 columns. Any less and the draw
	// area seems to glitch out...
	إذا (minimum != عدم)
	    *minimum  = gui.char_چوڑائی * 20;
	إذا (natural != عدم)
	    *natural = gui.char_چوڑائی * 20;
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
vim_form_شامل_اے(GtkWidget *widget بن_ورتیا, مزدوج x بن_ورتیا, مزدوج y بن_ورتیا)
{
    أرجع FALSE;
}
