/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#నిర్వచితం_కాకపోతే __GTK_FORM_H__
#నిర్వచించు __GTK_FORM_H__

#నిర్వచితమైతే USE_GTK3
# include <gtk/gtk.h>
#లేకపోతే
# include <gdk/gdk.h>
# include <gtk/gtkcontainer.h>
#అయితే_ముగింపు


#నిర్వచితమైతే __cplusplus
బాహ్య "C" {
#అయితే_ముగింపు

#నిర్వచించు GTK_TYPE_FORM		       (gui_gtk_form_get_type ())
#నిర్వచితమైతే USE_GTK3
# define GTK_FORM(obj)		       (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_FORM))
#లేకపోతే
# define GTK_FORM(obj)		       (GTK_CHECK_CAST ((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (GTK_CHECK_CLASS_CAST ((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (GTK_CHECK_TYPE ((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_FORM))
#అయితే_ముగింపు


రకనిర్వచనం నిర్మాణం _GtkForm GtkForm;
రకనిర్వచనం నిర్మాణం _GtkFormClass GtkFormClass;

నిర్మాణం _GtkForm
{
    GtkContainer container;

    GList *children;
    Gdk_కిటికీ *bin_కిటికీ;
    gint freeze_లెక్క;
};

నిర్మాణం _GtkFormClass
{
    GtkContainerClass మాతృ_class;
};

#నిర్వచితమైతే USE_GTK3
GType gui_gtk_form_get_type(శూన్య);
#లేకపోతే
GtkType gui_gtk_form_get_type(శూన్య);
#అయితే_ముగింపు

GtkWidget *gui_gtk_form_new(శూన్య);

శూన్య gui_gtk_form_put(GtkForm * form, GtkWidget * widget, gint x, gint y);

శూన్య gui_gtk_form_move(GtkForm *form, GtkWidget * widget, gint x, gint y);

శూన్య gui_gtk_form_move_resize(GtkForm * form, GtkWidget * widget, gint x, gint y, gint w, gint h);

// These disable and enable moving and repainting respectively.  If you
// want to update the layout's offsets but do not want it to repaint
// itself, you should use these functions.

శూన్య gui_gtk_form_freeze(GtkForm *form);
శూన్య gui_gtk_form_thaw(GtkForm *form);


#నిర్వచితమైతే __cplusplus
}
#అయితే_ముగింపు
#అయితే_ముగింపు	// __GTK_FORM_H__
