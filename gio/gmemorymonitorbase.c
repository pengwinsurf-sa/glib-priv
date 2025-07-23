/* GIO - GLib Input, Output and Streaming Library
 *
 * Copyright 2025 Red Hat, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "gcancellable.h"
#include "ginitable.h"
#include "gioerror.h"
#include "giomodule-priv.h"
#include "glib/gstdio.h"
#include "glibintl.h"
#include "gmemorymonitor.h"
#include "gmemorymonitorbase.h"

#ifdef HAVE_SYSINFO
#include <sys/sysinfo.h>
#endif

/**
 * GMemoryMonitorBase:
 *
 * An abstract base class for implementations of [iface@Gio.MemoryMonitor] which
 * provides several defined warning levels (`GLowMemoryLevel`) and tracks how
 * often they are notified to the user via [signal@Gio.MemoryMonitor::low-memory-warning]
 * to limit the number of signal emissions to one every 15 seconds for each level.
 * [method@Gio.MemoryMonitorBase.send_event_to_user] is provided for this purpose.
 */

/* The interval between sending a signal in second */
#define RECOVERY_INTERVAL_SEC 15

#define G_MEMORY_MONITOR_BASE_GET_INITABLE_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), G_TYPE_INITABLE, GInitable))

static void g_memory_monitor_base_iface_init (GMemoryMonitorInterface *iface);
static void g_memory_monitor_base_initable_iface_init (GInitableIface *iface);

typedef struct
{
  GObject parent_instance;

  guint64 last_trigger_us[G_MEMORY_MONITOR_LOW_MEMORY_LEVEL_COUNT];
} GMemoryMonitorBasePrivate;


G_DEFINE_ABSTRACT_TYPE_WITH_CODE (GMemoryMonitorBase, g_memory_monitor_base, G_TYPE_OBJECT,
                                  G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE,
                                    g_memory_monitor_base_initable_iface_init)
                                  G_IMPLEMENT_INTERFACE (G_TYPE_MEMORY_MONITOR,
                                      g_memory_monitor_base_iface_init)
                                  G_ADD_PRIVATE (GMemoryMonitorBase))

gdouble
g_memory_monitor_base_query_mem_ratio (void)
{
#ifdef HAVE_SYSINFO
  struct sysinfo info;

  if (sysinfo (&info))
    return -1.0;

  if (info.totalram == 0)
    return -1.0;

  return (gdouble) ((gdouble) info.freeram / (gdouble) info.totalram);
#else
  return -1.0;
#endif
}

GMemoryMonitorWarningLevel
g_memory_monitor_base_level_enum_to_byte (GMemoryMonitorLowMemoryLevel level)
{
  const GMemoryMonitorWarningLevel level_bytes[G_MEMORY_MONITOR_LOW_MEMORY_LEVEL_COUNT] = {
    [G_MEMORY_MONITOR_LOW_MEMORY_LEVEL_LOW] = 50,
    [G_MEMORY_MONITOR_LOW_MEMORY_LEVEL_MEDIUM] = 100,
    [G_MEMORY_MONITOR_LOW_MEMORY_LEVEL_CRITICAL] = 255
  };

  if ((int) level < G_MEMORY_MONITOR_LOW_MEMORY_LEVEL_INVALID ||
      (int) level >= G_MEMORY_MONITOR_LOW_MEMORY_LEVEL_COUNT)
    g_assert_not_reached ();

  if (level == G_MEMORY_MONITOR_LOW_MEMORY_LEVEL_INVALID)
    return 0;

  return level_bytes[level];
}

void
g_memory_monitor_base_send_event_to_user (GMemoryMonitorBase              *monitor,
                                          GMemoryMonitorLowMemoryLevel     warning_level)
{
  gint64 current_time;
  GMemoryMonitorBasePrivate *priv = g_memory_monitor_base_get_instance_private (monitor);

  current_time = g_get_monotonic_time ();

  if (priv->last_trigger_us[warning_level] == 0 ||
      (current_time - priv->last_trigger_us[warning_level]) > (RECOVERY_INTERVAL_SEC * G_USEC_PER_SEC))
    {
      g_debug ("Send low memory signal with warning level %u", warning_level);

      g_signal_emit_by_name (monitor, "low-memory-warning",
                             g_memory_monitor_base_level_enum_to_byte (warning_level));
      priv->last_trigger_us[warning_level] = current_time;
    }
}

static gboolean
g_memory_monitor_base_initable_init (GInitable     *initable,
                                        GCancellable  *cancellable,
                                        GError       **error)
{
  return TRUE;
}

static void
g_memory_monitor_base_init (GMemoryMonitorBase *monitor)
{
}

static void
g_memory_monitor_base_class_init (GMemoryMonitorBaseClass *klass)
{
}

static void
g_memory_monitor_base_iface_init (GMemoryMonitorInterface *monitor_iface)
{
}

static void
g_memory_monitor_base_initable_iface_init (GInitableIface *iface)
{
  iface->init = g_memory_monitor_base_initable_init;
}
