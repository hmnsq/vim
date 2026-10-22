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
قسمتعریف ساخت
{
    GtkWidget	*widget;
    صحیح		x;
    صحیح		y;
} Vim_Form_بچہ;

/*
 * Similar to the GtkFixed widget, allows absolute position and sizing of child
 * widgets within. Vim already has logic for positioning and sizing UI elements,
 * so this is needed to take advantage of that. We don't use GtkFixed directly
 * since we need to override some vfuncs.
 */
ساخت _VimForm
{
    GtkWidget والد;

    GList *children;

    // See vim_form_size_allocate()
    guint resize_idle_id;
    صحیح آخری_چوڑائی;
    صحیح آخری_اونچائی;
};

G_DEFINE_TYPE(VimForm, vim_form, GTK_TYPE_WIDGET)

ساکن خالی vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);
ساکن خالی vim_form_سائز_allocate(GtkWidget *widget, صحیح چوڑائی, صحیح اونچائی, صحیح baseline);
ساکن خالی vim_form_measure(GtkWidget *widget, GtkOrientation orientation, صحیح for_سائز, صحیح *minimum, صحیح *natural, صحیح *minimum_baseline, صحیح *natural_baseline);
ساکن gboolean vim_form_شامل_ہے(GtkWidget *widget, دوہرا x, دوہرا y);

    ساکن خالی
vim_form_dispose(GObject *obj)
{
    VimForm *self = VIM_FORM(obj);
    GList   *ele = self->children;

    جب (ele != تہی)
    {
	Vim_Form_بچہ *بچہ = ele->ڈیٹا;

	ele = ele->اگلا;
	gtk_widget_unparent(بچہ->widget);
	g_free(بچہ);
    }
    g_list_free(self->children);
    self->children = تہی;

    G_OBJECT_CLASS(vim_form_والد_class)->dispose(obj);
}

    ساکن خالی
vim_form_class_init(VimFormClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->snapshot = vim_form_snapshot;
    widget_class->سائز_allocate = vim_form_سائز_allocate;
    widget_class->measure = vim_form_measure;
    widget_class->شامل_ہے = vim_form_شامل_ہے;

    obj_class->dispose = vim_form_dispose;
}

    ساکن خالی
vim_form_init(VimForm *self غیر_مستعمل)
{
}

    GtkWidget *
vim_form_new(خالی)
{
    واپس g_object_new(VIM_TYPE_FORM, تہی);
}

/*
 * Transform the child
 */
    ساکن خالی
vim_form_position_بچہ(VimForm	*self, Vim_Form_بچہ *بچہ)
{
    GtkRequisition  requisition;
    GskTransform    *transform;
    صحیح		    w, h;

    gtk_widget_get_preferred_سائز(بچہ->widget, &requisition, تہی);
    w = requisition.چوڑائی;
    h = requisition.اونچائی;

    // If widget has no size request, use parent size
    اگر (w <= 0)
	w = gtk_widget_get_چوڑائی(GTK_WIDGET(self));
    اگر (h <= 0)
	h = gtk_widget_get_اونچائی(GTK_WIDGET(self));
    اگر (w <= 0) w = 1;
    اگر (h <= 0) h = 1;

    transform = gsk_transform_translate(تہی,
	    &GRAPHENE_نقطہ_INIT((اعشاری)بچہ->x, (اعشاری)بچہ->y));
    gtk_widget_allocate(بچہ->widget, w, h, -1, transform);
}

/*
 * Place the given widget at the point (x, y).
 */
    خالی
vim_form_put(VimForm *self, GtkWidget *widget, صحیح x, صحیح y)
{
    Vim_Form_بچہ *بچہ;

    بچہ = g_new(Vim_Form_بچہ, 1);

    بچہ->widget = widget;
    بچہ->x = x;
    بچہ->y = y;

    gtk_widget_سیٹ_سائز_request(بچہ->widget, -1, -1);

    self->children = g_list_append(self->children, بچہ);

    gtk_widget_سیٹ_والد(widget, GTK_WIDGET(self));
    vim_form_position_بچہ(self, بچہ);
}

/*
 * Move the widget (which should have already been added using vim_form_put())
 * to the given point (x, y).
 */
    خالی
vim_form_move(VimForm *self, GtkWidget *widget, صحیح x, صحیح y)
{
    برائے (GList *ele = self->children; ele != تہی; ele = ele->اگلا)
    {
	Vim_Form_بچہ *بچہ = ele->ڈیٹا;
	اگر (بچہ->widget == widget)
	{
	    بچہ->x = x;
	    بچہ->y = y;
	    vim_form_position_بچہ(self, بچہ);
	    واپس;
	}
    }
}

/*
 * Move and resize the child.
 */
    خالی
vim_form_move_resize(
	VimForm	    *self,
	GtkWidget   *widget,
	gint	    x,
	gint	    y,
	gint	    w,
	gint	    h)
{
    gtk_widget_سیٹ_سائز_request(widget, w, h);
    اگر (!gtk_widget_get_visible(widget))
	gtk_widget_قطار_resize(widget);
    vim_form_move(self, widget, x, y);
}

    خالی
vim_form_remove(VimForm *self, GtkWidget *widget)
{
    برائے (GList *ele = self->children; ele != تہی; ele = ele->اگلا)
    {
	Vim_Form_بچہ *بچہ = ele->ڈیٹا;
	اگر (بچہ->widget == widget)
	{
	    self->children = g_list_remove_link(self->children, ele);
	    g_list_free_1(ele);
	    gtk_widget_unparent(widget);
	    g_free(بچہ);
	    واپس;
	}
    }
}

    ساکن خالی
vim_form_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    VimForm *self = VIM_FORM(widget);

    برائے (GList *ele = self->children; ele != تہی; ele = ele->اگلا)
    {
	Vim_Form_بچہ *بچہ = ele->ڈیٹا;
	gtk_widget_snapshot_بچہ(widget, بچہ->widget, snapshot);
    }
}

    ساکن gboolean
vim_form_resize_idle_cb(VimForm *self)
{
    self->resize_idle_id = 0;

    اگر (gui.drawarea == تہی)
	جاؤ باہر_نکلو;

    اگر (self->آخری_چوڑائی > 1 && self->آخری_اونچائی > 1)
    {
	// Ignore an allocation that does not answer the size that was last
	// asked for: it was computed before the request and using it would
	// compute Rows and Columns from the old size together with the new
	// base size, losing columns. Give up after one allocation in case
	// the request is never answered exactly.
	اگر (gui.pending_form_w > 0
		&& (self->آخری_چوڑائی != gui.pending_form_w
		    || self->آخری_اونچائی != gui.pending_form_h)
		&& --gui.pending_form_skip >= 0)
	    جاؤ باہر_نکلو;

	صحیح	req_w, req_h;

	gui.pending_form_w = 0;

	// The size request that Vim made has been answered.  Keep the size and
	// drop the request, otherwise the window could not be made smaller.
	gtk_widget_get_سائز_request(GTK_WIDGET(self), &req_w, &req_h);
	اگر (req_w != -1 || req_h != -1)
	{
	    gtk_کھڑکی_سیٹ_default_سائز(GTK_کھڑکی(gui.mainwin),
		    gtk_widget_get_چوڑائی(gui.mainwin),
		    gtk_widget_get_اونچائی(gui.mainwin));
	    gtk_widget_سیٹ_سائز_request(GTK_WIDGET(self), -1, -1);
	}

	gui_resize_shell(self->آخری_چوڑائی, self->آخری_اونچائی);
    }

باہر_نکلو:
    g_object_unref(self);
    واپس G_ماخذ_REMOVE;
}

    ساکن خالی
vim_form_سائز_allocate(
	GtkWidget *widget,
	صحیح چوڑائی,
	صحیح اونچائی,
	صحیح baseline غیر_مستعمل)
{
    VimForm *self = VIM_FORM(widget);

    برائے (GList *ele = self->children; ele != تہی; ele = ele->اگلا)
	vim_form_position_بچہ(self, ele->ڈیٹا);

    // Notify Vim about size change via idle callback
    اگر (چوڑائی != self->آخری_چوڑائی || اونچائی != self->آخری_اونچائی)
    {
	self->آخری_چوڑائی = چوڑائی;
	self->آخری_اونچائی = اونچائی;

	اگر (self->resize_idle_id == 0)
	    self->resize_idle_id = g_idle_جوڑو(
		    (GSourceFunc)vim_form_resize_idle_cb, g_object_ref(self));
    }
}

    ساکن خالی
vim_form_measure(
	GtkWidget	*widget غیر_مستعمل,
	GtkOrientation	orientation,
	صحیح		for_سائز غیر_مستعمل,
	صحیح		*minimum,
	صحیح		*natural,
	صحیح		*minimum_baseline,
	صحیح		*natural_baseline)
{
    اگر (orientation == GTK_ORIENTATION_VERTICAL)
    {
	// Set minimum height of form widget to 4 rows.
	اگر (minimum != تہی)
	    *minimum  = gui.char_اونچائی * 4;
	اگر (natural != تہی)
	    *natural = gui.char_اونچائی * 4;
    }
    ورنہ
    {
	// Set minimum width of form widget to 20 columns. Any less and the draw
	// area seems to glitch out...
	اگر (minimum != تہی)
	    *minimum  = gui.char_چوڑائی * 20;
	اگر (natural != تہی)
	    *natural = gui.char_چوڑائی * 20;
    }

    اگر (minimum_baseline != تہی)
	*minimum_baseline = -1;
    اگر (natural_baseline != تہی)
	*natural_baseline = -1;
}


/*
 * Make the form itself input-transparent so clicks on its empty area fall
 * through to the drawarea below, while the scrollbar children stay pickable.
 */
    ساکن gboolean
vim_form_شامل_ہے(GtkWidget *widget غیر_مستعمل, دوہرا x غیر_مستعمل, دوہرا y غیر_مستعمل)
{
    واپس FALSE;
}
