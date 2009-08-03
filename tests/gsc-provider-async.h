/* 
 *  gsc-provider-async.h - Type here a short description of your plugin
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
#ifndef __ASYNC_PROVIDER_H__
#define __ASYNC_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtksourceview/gtksourcecompletionprovider.h>

G_BEGIN_DECLS

#define GSC_TYPE_PROVIDER_ASYNC (gsc_provider_async_get_type ())
#define GSC_PROVIDER_ASYNC(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_PROVIDER_ASYNC, GscProviderAsync))
#define GSC_PROVIDER_ASYNC_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_PROVIDER_ASYNC, GscProviderAsyncClass))
#define GSC_IS_PROVIDER_ASYNC(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_PROVIDER_ASYNC))
#define GSC_IS_PROVIDER_ASYNC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_PROVIDER_ASYNC))
#define GSC_PROVIDER_ASYNC_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_PROVIDER_ASYNC, GscProviderAsyncClass))

#define GSC_PROVIDER_ASYNC_NAME "GscProviderAsync"

typedef struct _GscProviderAsync GscProviderAsync;
typedef struct _GscProviderAsyncPrivate GscProviderAsyncPrivate;
typedef struct _GscProviderAsyncClass GscProviderAsyncClass;

struct _GscProviderAsync
{
	GObject parent;
	
	GscProviderAsyncPrivate *priv;
};

struct _GscProviderAsyncClass
{
	GObjectClass parent;
};

GType		 gsc_provider_async_get_type	(void) G_GNUC_CONST;

GscProviderAsync *gsc_provider_async_new (const gchar *name,
                                        GdkPixbuf   *icon);

G_END_DECLS

#endif
