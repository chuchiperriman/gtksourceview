/*
 * gtksourcecompletionprovider.h
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

#ifndef __GTK_SOURCE_COMPLETION_PROVIDER_H__
#define __GTK_SOURCE_COMPLETION_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourcecompletionproposal.h>
#include <gtksourceview/gtksourcecompletioninfo.h>

G_BEGIN_DECLS

#define GTK_TYPE_SOURCE_COMPLETION_PROVIDER 			(gtk_source_completion_provider_get_type ())
#define GTK_SOURCE_COMPLETION_PROVIDER(obj) 			(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_SOURCE_COMPLETION_PROVIDER, GtkSourceCompletionProvider))
#define GTK_IS_SOURCE_COMPLETION_PROVIDER(obj) 			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_SOURCE_COMPLETION_PROVIDER))
#define GTK_SOURCE_COMPLETION_PROVIDER_GET_INTERFACE(obj) 	(G_TYPE_INSTANCE_GET_INTERFACE ((obj), GTK_TYPE_SOURCE_COMPLETION_PROVIDER, GtkSourceCompletionProviderIface))

#define GTK_SOURCE_COMPLETION_CAPABILITY_INTERACTIVE "standard::interactive"
#define GTK_SOURCE_COMPLETION_CAPABILITY_AUTOMATIC "standard::automatic"

typedef struct _GtkSourceCompletionProvider GtkSourceCompletionProvider;
typedef struct _GtkSourceCompletionProviderIface GtkSourceCompletionProviderIface;

#include <gtksourceview/gtksourcecompletioncontext.h>

struct _GtkSourceCompletionProviderIface
{
	GTypeInterface g_iface;
	
	const gchar	*(*get_name)       	(GtkSourceCompletionProvider *provider);
	GdkPixbuf	*(*get_icon)       	(GtkSourceCompletionProvider *provider);
	void		 (*populate_completion)	(GtkSourceCompletionProvider *provider,
						 GtkSourceCompletionContext  *context);

	const gchar     *(*get_capabilities)	(GtkSourceCompletionProvider *provider);

	GtkWidget 	*(*get_info_widget)	(GtkSourceCompletionProvider *provider,
						 GtkSourceCompletionProposal *proposal);
	void		 (*update_info)		(GtkSourceCompletionProvider *provider,
						 GtkSourceCompletionProposal *proposal,
						 GtkSourceCompletionInfo     *info);

	gboolean	 (*activate_proposal)	(GtkSourceCompletionProvider *provider,
						 GtkSourceCompletionProposal *proposal,
						 GtkTextIter                 *iter);
};

GType		 gtk_source_completion_provider_get_type		(void);


const gchar	*gtk_source_completion_provider_get_name		(GtkSourceCompletionProvider *provider);

GdkPixbuf	*gtk_source_completion_provider_get_icon		(GtkSourceCompletionProvider *provider);

void		 gtk_source_completion_provider_populate_completion	(GtkSourceCompletionProvider *provider,
									 GtkSourceCompletionContext  *context);

const gchar 	*gtk_source_completion_provider_get_capabilities 	(GtkSourceCompletionProvider *provider);

GtkWidget	*gtk_source_completion_provider_get_info_widget		(GtkSourceCompletionProvider *provider,
									 GtkSourceCompletionProposal *proposal);

void 		 gtk_source_completion_provider_update_info		(GtkSourceCompletionProvider *provider,
									 GtkSourceCompletionProposal *proposal,
									 GtkSourceCompletionInfo     *info);

gboolean	 gtk_source_completion_provider_activate_proposal 	(GtkSourceCompletionProvider *provider,
									 GtkSourceCompletionProposal *proposal,
									 GtkTextIter                 *iter);

G_END_DECLS

#endif
