#ifndef __GTK_SOURCE_COMPLETION_ITEM_H__
#define __GTK_SOURCE_COMPLETION_ITEM_H__

#include <glib-object.h>
#include <gtksourceview/gtksourcecompletionproposal.h>

G_BEGIN_DECLS

#define GTK_TYPE_SOURCE_COMPLETION_ITEM			(gtk_source_completion_item_get_type ())
#define GTK_SOURCE_COMPLETION_ITEM(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_SOURCE_COMPLETION_ITEM, GtkSourceCompletionItem))
#define GTK_SOURCE_COMPLETION_ITEM_CONST(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_SOURCE_COMPLETION_ITEM, GtkSourceCompletionItem const))
#define GTK_SOURCE_COMPLETION_ITEM_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_SOURCE_COMPLETION_ITEM, GtkSourceCompletionItemClass))
#define GTK_IS_SOURCE_COMPLETION_ITEM(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_SOURCE_COMPLETION_ITEM))
#define GTK_IS_SOURCE_COMPLETION_ITEM_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_SOURCE_COMPLETION_ITEM))
#define GTK_SOURCE_COMPLETION_ITEM_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_SOURCE_COMPLETION_ITEM, GtkSourceCompletionItemClass))

typedef struct _GtkSourceCompletionItem		GtkSourceCompletionItem;
typedef struct _GtkSourceCompletionItemClass	GtkSourceCompletionItemClass;
typedef struct _GtkSourceCompletionItemPrivate	GtkSourceCompletionItemPrivate;

struct _GtkSourceCompletionItem {
	GObject parent;
	
	GtkSourceCompletionItemPrivate *priv;
};

struct _GtkSourceCompletionItemClass {
	GObjectClass parent_class;
};

GType 			 gtk_source_completion_item_get_type 		(void) G_GNUC_CONST;

GtkSourceCompletionItem *gtk_source_completion_item_new 		(const gchar *label,
									 GdkPixbuf   *icon,
									 const gchar *info);

GtkSourceCompletionItem *gtk_source_completion_item_new_from_stock	(const gchar *label,
								 	 const gchar *stock,
								 	 const gchar *info);

G_END_DECLS

#endif /* __GTK_SOURCE_COMPLETION_ITEM_H__ */