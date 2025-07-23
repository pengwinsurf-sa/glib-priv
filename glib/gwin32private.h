/*
 * Copyright © 2025 Luca Bacci
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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Luca Bacci <luca.bacci@outlook.com>
 */

#ifndef __G_WIN32PRIVATE_H__
#define __G_WIN32PRIVATE_H__

#ifdef G_PLATFORM_WIN32

#include "config.h"

#include <windows.h>
#include <stdbool.h>

bool
g_win32_substitute_pid_and_event (wchar_t       *local_debugger,
                                  gsize          debugger_size,
                                  const wchar_t *cmdline,
                                  DWORD          pid,
                                  guintptr       event);


#endif /* G_PLATFORM_WIN32 */

#endif /* __G_WIN32PRIVATE_H__ */
