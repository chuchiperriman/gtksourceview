/*
 * gtksourcecompletion.h
 * This file is part of gtksourcecompletion
 *
 * Copyright (C) 2007 -2009 Jesús Barbero Rodríguez <chuchiperriman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA.
 */
 
#ifndef GTK_SOURCE_COMPLETION_H
#define GTK_SOURCE_COMPLETION_H

#include <gtk/gtk.h>
#include <gtksourceview/gtksourcecompletioninfo.h>
#include <gtksourceview/gtksourcecompletionprovider.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTK_TYPE_SOURCE_COMPLETION              (gtk_source_completion_get_type())
#define GTK_SOURCE_COMPLETION(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_SOURCE_COMPLETION, GtkSourceCompletion))
#define GTK_SOURCE_COMPLETION_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_SOURCE_COMPLETION, GtkSourceCompletionClass))
#define GTK_IS_SOURCE_COMPLETION(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_SOURCE_COMPLETION))
#define GTK_IS_SOURCE_COMPLETION_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_SOURCE_COMPLETION))
#define GTK_SOURCE_COMPLETION_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_SOURCE_COMPLETION, GtkSourceCompletionClass))

#define GTK_SOURCE_COMPLETION_ERROR		(gtk_source_completion_error_quark ())

typedef struct _GtkSourceCompletionPrivate GtkSourceCompletionPrivate;
typedef struct _GtkSourceCompletion GtkSourceCompletion;
typedef struct _GtkSourceCompletionClass GtkSourceCompletionClass;

typedef enum
{
	GTK_SOURCE_COMPLETION_ERROR_ALREADY_BOUND = 0,
	GTK_SOURCE_COMPLETION_ERROR_NOT_BOUND,
} GtkSourceCompletionError;

/* Forward declaration of GtkSourceView */
struct _GtkSourceView;

struct _GtkSourceCompletion
{
	GObject parent;

	GtkSourceCompletionPrivate *priv;
};

struct _GtkSourceCompletionClass
{
	GObjectClass parent_class;

	gboolean 	(* proposal_activated)		(GtkSourceCompletion         *completion,
	                                                 GtkSourceCompletionProvider *provider,
							 GtkSourceCompletionProposal *proposal);
	void 		(* show)			(GtkSourceCompletion         *completion);
	void		(* hide)			(GtkSourceCompletion         *completion);
};

GType		 gtk_source_completion_get_type			(void) G_GNUC_CONST;

GQuark		 gtk_source_completion_error_quark		(void);

gboolean	 gtk_source_completion_add_provider		(GtkSourceCompletion          *completion,
								 GtkSourceCompletionProvider  *provider,
								 GError                      **error);

gboolean	 gtk_source_completion_remove_provider		(GtkSourceCompletion          *completion,
								 GtkSourceCompletionProvider  *provider,
								 GError                      **error);

GList		*gtk_source_completion_get_providers		(GtkSourceCompletion         *completion,
                                                                 const gchar                 *capabilities);
gboolean	 gtk_source_completion_show			(GtkSourceCompletion         *completion,
								 GList                       *providers,
								 GtkTextIter                 *place);

void		 gtk_source_completion_hide			(GtkSourceCompletion         *completion);

GtkSourceCompletionInfo *
		 gtk_source_completion_get_info_window		(GtkSourceCompletion         *completion);

struct _GtkSourceView *
		 gtk_source_completion_get_view			(GtkSourceCompletion	     *completion);

G_END_DECLS

#endif 