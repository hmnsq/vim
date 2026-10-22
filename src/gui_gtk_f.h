/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#اگر_غیر_تعریف_شدہ __GTK_FORM_H__
#تعریف_کرو __GTK_FORM_H__

#اگر_تعریف_شدہ USE_GTK3
# include <gtk/gtk.h>
#ورنہ
# include <gdk/gdk.h>
# include <gtk/gtkcontainer.h>
#اگر_ختم


#اگر_تعریف_شدہ __cplusplus
خارجی "C" {
#اگر_ختم

#تعریف_کرو GTK_TYPE_FORM		       (gui_gtk_form_get_type ())
#اگر_تعریف_شدہ USE_GTK3
# define GTK_FORM(obj)		       (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_FORM))
#ورنہ
# define GTK_FORM(obj)		       (GTK_CHECK_CAST ((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (GTK_CHECK_CLASS_CAST ((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (GTK_CHECK_TYPE ((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_FORM))
#اگر_ختم


قسمتعریف ساخت _GtkForm GtkForm;
قسمتعریف ساخت _GtkFormClass GtkFormClass;

ساخت _GtkForm
{
    GtkContainer container;

    GList *children;
    Gdk_کھڑکی *bin_کھڑکی;
    gint freeze_گنتی;
};

ساخت _GtkFormClass
{
    GtkContainerClass والد_class;
};

#اگر_تعریف_شدہ USE_GTK3
GType gui_gtk_form_get_type(خالی);
#ورنہ
GtkType gui_gtk_form_get_type(خالی);
#اگر_ختم

GtkWidget *gui_gtk_form_new(خالی);

خالی gui_gtk_form_put(GtkForm * form, GtkWidget * widget, gint x, gint y);

خالی gui_gtk_form_move(GtkForm *form, GtkWidget * widget, gint x, gint y);

خالی gui_gtk_form_move_resize(GtkForm * form, GtkWidget * widget, gint x, gint y, gint w, gint h);

// These disable and enable moving and repainting respectively.  If you
// want to update the layout's offsets but do not want it to repaint
// itself, you should use these functions.

خالی gui_gtk_form_freeze(GtkForm *form);
خالی gui_gtk_form_thaw(GtkForm *form);


#اگر_تعریف_شدہ __cplusplus
}
#اگر_ختم
#اگر_ختم	// __GTK_FORM_H__
