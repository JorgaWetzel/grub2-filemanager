/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2019  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <grub/types.h>
#include <grub/misc.h>
#include <grub/mm.h>
#include <grub/err.h>
#include <grub/dl.h>
#include <grub/file.h>
#include <grub/term.h>
#include <grub/env.h>
#include <grub/normal.h>
#include <grub/command.h>
#include <grub/script_sh.h>

#include "fm.h"

static const char *num[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};

static void
grubfm_add_menu_back (const char *filename)
{
  char *dir = NULL;
  char *src = NULL;
  dir = grub_strdup (filename);
  *grub_strrchr (dir, '/') = 0;

  src = grub_xasprintf ("grubfm \"%s/\"", dir);

  grubfm_add_menu (_("Back"), "go-previous", NULL, src, 0);
  grub_free (src);
  if (dir)
    grub_free (dir);
}

void
grubfm_open_file (char *path)
{
  int i;
  grubfm_add_menu_back (path);
  struct grubfm_enum_file_info info;
  struct grubfm_ini_enum_list *ctx = &grubfm_ext_table;
  grub_file_t file = 0;
  file = grub_file_open (path, GRUB_FILE_TYPE_GET_SIZE |
                           GRUB_FILE_TYPE_NO_DECOMPRESS);
  if (!file)
    return;
  info.name = file->name;
  info.size = (char *) grub_get_human_size (file->size,
                                            GRUB_HUMAN_SIZE_SHORT);
  grubfm_get_file_icon (&info);

  for (i = 0; i < 10; i++)
  {
    char *src = NULL;
    const char *script = NULL;
    const char *icon = NULL;
    const char *title = NULL;
#ifdef GRUB_MACHINE_EFI
    char platform = 'e';
#elif defined (GRUB_MACHINE_PCBIOS)
    char platform = 'b';
#else
    char platform = 'u';
#endif
    const char *enable = NULL;
    script = ini_get (ctx->config[info.ext], num[i], "menu");
    if (! script)
      break;
    if (ini_get (ctx->config[info.ext], num[i], "hidden"))
      continue;
    enable = ini_get (ctx->config[info.ext], num[i], "enable");
    if (enable && enable[0] != 'a' && enable[0] != platform)
      continue;
    icon = ini_get (ctx->config[info.ext], num[i], "icon");
    if (! icon)
      icon = "file";
    title = ini_get (ctx->config[info.ext], num[i], "title");
    if (! title)
      title = "MENU";
    src = grub_xasprintf ("export grubfm_file=\"%s\"\n"
                          "configfile ${prefix}/rules/%s\n", path, script);
    grubfm_add_menu (_(title), icon, NULL, src, 0);
    if (src)
      grub_free (src);
  }

  grub_file_close (file);
}
