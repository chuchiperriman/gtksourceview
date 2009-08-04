/* 
 *  gsc-provider-async.c - Type here a short description of your plugin
 *
 *  Copyright (C) 2008 - perriman
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gsc-provider-async.h"
#include <gtksourceview/gtksourcecompletion.h>
#include <gtksourceview/gtksourcecompletionitem.h>

#define GSC_PROVIDER_ASYNC_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE((object), GSC_TYPE_PROVIDER_ASYNC, GscProviderAsyncPrivate))

static void	 gsc_provider_async_iface_init	(GtkSourceCompletionProviderIface *iface);

struct _GscProviderAsyncPrivate
{
	gchar		*name;
	GdkPixbuf 	*icon;
	GdkPixbuf 	*proposal_icon;
	GThread		*th;
	GtkSourceCompletionContext *context;
	guint		nth;
};

G_DEFINE_TYPE_WITH_CODE (GscProviderAsync,
			 gsc_provider_async,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_SOURCE_COMPLETION_PROVIDER,
				 		gsc_provider_async_iface_init))

static const gchar * 
gsc_provider_async_get_name (GtkSourceCompletionProvider *self)
{
	return GSC_PROVIDER_ASYNC (self)->priv->name;
}

static GdkPixbuf * 
gsc_provider_async_get_icon (GtkSourceCompletionProvider *self)
{
	return GSC_PROVIDER_ASYNC (self)->priv->icon;
}


static GList *
append_item (GList *list, const gchar *name, GdkPixbuf *icon, const gchar *info)
{
	GtkSourceCompletionItem *prop;
	
	prop = gtk_source_completion_item_new (name, name, icon, info);
	return g_list_append (list, prop);
}

static gpointer
thread_add_proposals (GscProviderAsync *self)
{
	guint num = 0;
	GList *list = NULL;
	gchar *name;
	GtkSourceCompletionContext *context = self->priv->context;
	guint nth = self->priv->nth;

	g_object_ref (context);
	while (num++ < 5 && gtk_source_completion_context_is_valid (context))
	{
		g_debug ("thread");
		list = NULL;
		name = g_strdup_printf ("th%i-%i", nth, num);
		list = append_item (list, name, self->priv->proposal_icon, name);
		g_free (name);
		name = g_strdup_printf ("thb%i-%i", nth, num);
		list = append_item (list, name, self->priv->proposal_icon, name);
		g_free (name);

		gtk_source_completion_context_add_proposals (context, GTK_SOURCE_COMPLETION_PROVIDER (self), list);
		g_usleep (2000000);
	}
	g_object_unref (context);
	self->priv->th = NULL;
	return NULL;
}

static void
context_finished (GtkSourceCompletionContext *context,
		  GscProviderAsync *self)
{
	/*
	g_debug ("con fini");
	g_object_unref (self->priv->context);
	g_debug ("unref");
	self->priv->context = NULL;
	*/
}

static void
gsc_provider_async_populate_completion (GtkSourceCompletionProvider *base,
				       GtkSourceCompletionContext  *context)
{
	//TODO If there is a context active, clear all
	GscProviderAsync *self = GSC_PROVIDER_ASYNC (base);

	g_debug ("populate async");

	self->priv->nth++;
	self->priv->context = context;
	self->priv->th = g_thread_create (thread_add_proposals,
					  self,
					  FALSE,
					  NULL);
	g_signal_connect (context,
			  "finished",
			  context_finished,
			  self);

	//TODO Who frees the list?
}

/*
static gboolean
gsc_provider_async_filter_proposal (GtkSourceCompletionProvider *provider,
                                   GtkSourceCompletionProposal *proposal,
                                   GtkTextIter                 *iter,
                                   const gchar                 *criteria)
{
	const gchar *label;
	
	label = gtk_source_completion_proposal_get_label (proposal);
	return g_str_has_prefix (label, criteria);
}
*/

static const gchar *
gsc_provider_async_get_capabilities (GtkSourceCompletionProvider *provider)
{
	return GTK_SOURCE_COMPLETION_CAPABILITY_INTERACTIVE ","
	       GTK_SOURCE_COMPLETION_CAPABILITY_AUTOMATIC;
}

static void 
gsc_provider_async_finalize (GObject *object)
{
	GscProviderAsync *provider = GSC_PROVIDER_ASYNC (object);
	
	g_free (provider->priv->name);
	
	if (provider->priv->icon != NULL)
	{
		g_object_unref (provider->priv->icon);
	}
	
	if (provider->priv->proposal_icon != NULL)
	{
		g_object_unref (provider->priv->proposal_icon);
	}

	G_OBJECT_CLASS (gsc_provider_async_parent_class)->finalize (object);
}

static void 
gsc_provider_async_class_init (GscProviderAsyncClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	object_class->finalize = gsc_provider_async_finalize;
	
	g_type_class_add_private (object_class, sizeof(GscProviderAsyncPrivate));
}

static void
gsc_provider_async_iface_init (GtkSourceCompletionProviderIface *iface)
{
	iface->get_name = gsc_provider_async_get_name;
	iface->get_icon = gsc_provider_async_get_icon;

	iface->populate_completion = gsc_provider_async_populate_completion;
	//iface->filter_proposal = gsc_provider_async_filter_proposal;
	iface->get_capabilities = gsc_provider_async_get_capabilities;
}

static void 
gsc_provider_async_init (GscProviderAsync * self)
{
	GtkIconTheme *theme;
	gint width;
	
	self->priv = GSC_PROVIDER_ASYNC_GET_PRIVATE (self);
	
	theme = gtk_icon_theme_get_default ();

	gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &width, NULL);
	self->priv->proposal_icon = gtk_icon_theme_load_icon (theme,
	                                                      GTK_STOCK_YES,
	                                                      width,
	                                                      GTK_ICON_LOOKUP_USE_BUILTIN,
	                                                      NULL);
	self->priv->th = NULL;
	self->priv->nth = 0;
}

GscProviderAsync *
gsc_provider_async_new (const gchar *name,
                       GdkPixbuf   *icon)
{
	GscProviderAsync *ret = g_object_new (GSC_TYPE_PROVIDER_ASYNC, NULL);
	
	ret->priv->name = g_strdup (name);
	
	if (icon != NULL)
	{
		ret->priv->icon = g_object_ref (icon);
	}

	return ret;
}
