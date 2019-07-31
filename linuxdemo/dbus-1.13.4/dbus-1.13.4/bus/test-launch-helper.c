/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* test-main.c  main() for the OOM check of the launch helper
 *
 * Copyright (C) 2007 Red Hat, Inc.
 *
 * Licensed under the Academic Free License version 2.1
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <config.h>
#include "test.h"
#include "activation-helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus-internals.h>
#include <dbus/dbus-misc.h>
#include <dbus/dbus-test-tap.h>

#if !defined(DBUS_ENABLE_EMBEDDED_TESTS) || !defined(DBUS_UNIX)
#error This file is only relevant for the embedded tests on Unix
#endif

#ifdef ACTIVATION_LAUNCHER_DO_OOM

/* returns true if good things happen, or if we get OOM */
static dbus_bool_t
bus_activation_helper_oom_test (void        *data,
                                dbus_bool_t  have_memory)
{
  const char *service;
  DBusError error;
  dbus_bool_t retval;

  service = (const char *) data;
  retval = TRUE;

  dbus_error_init (&error);

  if (!run_launch_helper (service, &error))
    {
      _DBUS_ASSERT_ERROR_IS_SET (&error);
      /* we failed, but a OOM is good */
      if (!dbus_error_has_name (&error, DBUS_ERROR_NO_MEMORY))
        {
          _dbus_warn ("FAILED SELF TEST: Error: %s", error.message);
          retval = FALSE;
        }
      dbus_error_free (&error);
    }
  else
    {
      /* we succeeded, yay! */
      _DBUS_ASSERT_ERROR_IS_CLEAR (&error);
    }
  return retval;
}

#endif

int
main (int argc, char **argv)
{
  const char *dir;
  DBusString config_file;

  if (argc > 1 && strcmp (argv[1], "--tap") != 0)
    dir = argv[1];
  else
    dir = _dbus_getenv ("DBUS_TEST_DATA");

  if (dir == NULL)
    _dbus_test_fatal ("Must specify test data directory as argv[1] or in DBUS_TEST_DATA env variable");

  _dbus_test_diag ("%s: Running launch helper OOM checks", argv[0]);

  if (!_dbus_string_init (&config_file) ||
      !_dbus_string_append (&config_file, dir) ||
      !_dbus_string_append (&config_file, "/valid-config-files-system/debug-allow-all-pass.conf"))
    _dbus_test_fatal ("OOM during initialization");

  /* use a config file that will actually work... */
  dbus_setenv ("TEST_LAUNCH_HELPER_CONFIG",
               _dbus_string_get_const_data (&config_file));

  _dbus_string_free (&config_file);

  if (!_dbus_test_oom_handling ("dbus-daemon-launch-helper",
                                bus_activation_helper_oom_test,
                                (char *) "org.freedesktop.DBus.TestSuiteEchoService"))
    _dbus_test_fatal ("OOM test failed");

  /* ... otherwise it must have passed */
  _dbus_test_ok ("%s", argv[0]);

  _dbus_test_check_memleaks (argv[0]);

  return _dbus_test_done_testing ();
}
