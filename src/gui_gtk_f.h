/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#جے_بنا_تعریف __GTK_FORM_H__
#تعریف_کرو __GTK_FORM_H__

#جے_تعریف_شدہ USE_GTK3
# include <gtk/gtk.h>
#وإلا
# include <gdk/gdk.h>
# include <gtk/gtkcontainer.h>
#جے_انت


#جے_تعریف_شدہ __cplusplus
خارجي "C" {
#جے_انت

#تعریف_کرو GTK_TYPE_FORM		       (gui_gtk_form_get_type ())
#جے_تعریف_شدہ USE_GTK3
# define GTK_FORM(obj)		       (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_FORM))
#وإلا
# define GTK_FORM(obj)		       (GTK_CHECK_CAST ((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (GTK_CHECK_CLASS_CAST ((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (GTK_CHECK_TYPE ((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_FORM))
#جے_انت


تعريفنوع بنية _GtkForm GtkForm;
تعريفنوع بنية _GtkFormClass GtkFormClass;

بنية _GtkForm
{
    GtkContainer container;

    GList *children;
    Gdk_کھڑکی *bin_کھڑکی;
    gint freeze_گنتی;
};

بنية _GtkFormClass
{
    GtkContainerClass ماپا_class;
};

#جے_تعریف_شدہ USE_GTK3
GType gui_gtk_form_get_type(فراغ);
#وإلا
GtkType gui_gtk_form_get_type(فراغ);
#جے_انت

GtkWidget *gui_gtk_form_new(فراغ);

فراغ gui_gtk_form_put(GtkForm * form, GtkWidget * widget, gint x, gint y);

فراغ gui_gtk_form_move(GtkForm *form, GtkWidget * widget, gint x, gint y);

فراغ gui_gtk_form_move_resize(GtkForm * form, GtkWidget * widget, gint x, gint y, gint w, gint h);

// These disable and enable moving and repainting respectively.  If you
// want to update the layout's offsets but do not want it to repaint
// itself, you should use these functions.

فراغ gui_gtk_form_freeze(GtkForm *form);
فراغ gui_gtk_form_thaw(GtkForm *form);


#جے_تعریف_شدہ __cplusplus
}
#جے_انت
#جے_انت	// __GTK_FORM_H__
