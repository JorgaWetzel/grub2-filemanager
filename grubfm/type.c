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
#include <grub/device.h>
#include <grub/fs.h>
#include <grub/misc.h>
#include <grub/mm.h>
#include <grub/err.h>
#include <grub/term.h>

#include <ini.h>

#include "fm.h"

struct grubfm_ini_enum_list grubfm_ext_table = {0, 0, NULL, NULL, NULL};

static int
grubfm_ini_enum_count (const char *filename,
                       const struct grub_dirhook_info *info,
                       void *data)
{
  struct grubfm_ini_enum_list *ctx = data;

  if (!info->dir)
    ctx->n++;
  return 0;
}

static int
grubfm_ini_enum_iter (const char *filename,
                      const struct grub_dirhook_info *info,
                      void *data)
{
  struct grubfm_ini_enum_list *ctx = data;
  char *pathname;
  pathname = grub_xasprintf ("/boot/grub/types/%s", filename);
  if (!pathname)
    return 1;
  if (! info->dir)
  {
    ctx->ext[ctx->i] = grub_strdup (filename);
    ctx->i++;
  }
  grub_free (pathname);
  return 0;
}

void
grubfm_ini_enum (const char *devname)
{
  grub_fs_t fs;
  const char *path = "/boot/grub/types/";
  grub_device_t dev;

  dev = grub_device_open (devname);
  if (!dev)
    goto fail;

  fs = grub_fs_probe (dev);

  if (fs)
  {
    struct grubfm_ini_enum_list *ctx = &grubfm_ext_table;
    (fs->fs_dir) (dev, path, grubfm_ini_enum_count, ctx);
    ctx->ext = grub_zalloc (ctx->n * sizeof (ctx->ext[0]));
    ctx->icon = grub_zalloc (ctx->n * sizeof (ctx->icon[0]));
    ctx->config = grub_zalloc (ctx->n * sizeof (ctx->config[0]));
    (fs->fs_dir) (dev, path, grubfm_ini_enum_iter, ctx);
    for (ctx->i = 0; ctx->i < ctx->n; ctx->i++)
    {
      char *ini_name = NULL;
      ini_name = grub_xasprintf ("(%s)/boot/grub/types/%s",
                                 devname, ctx->ext[ctx->i]);
      if (!ini_name)
        break;
      ini_t *config = ini_load (ini_name);
      grub_free (ini_name);
      if (!config)
        continue;
      ctx->icon[ctx->i] = grub_strdup (ini_get(config, "type", "icon"));
      ctx->config[ctx->i] = config;
    }
  }

 fail:
  if (dev)
    grub_device_close (dev);
}

const char *
grubfm_get_file_icon (struct grubfm_enum_file_info *info)
{
  const char *icon = "file";
  if (!info || !info->name)
    goto ret;
  info->ext = 0;
  char *ext = grub_strrchr (info->name, '.');
  if (!ext || *ext == '\0' || *(ext++) == '\0')
    goto ret;
  struct grubfm_ini_enum_list *ctx = &grubfm_ext_table;
  for (ctx->i = 0; ctx->i < ctx->n; ctx->i++)
  {
    grub_printf ("ext checking %s ", ctx->ext[ctx->i]);
    if (grub_strcasecmp (ext, ctx->ext[ctx->i]) == 0)
    {
      icon = ctx->icon[ctx->i];
      info->ext = ctx->i;
      break;
    }
  }
ret:
  return icon;
}
