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
#include <grub/term.h>
 
#include "fm.h"

struct grubfm_file_ext grubfm_file_table[] =
{
  {".iso", "iso", ISO},
  {".img", "img", DISK},
  {".ima", "img", DISK},
  {".vhd", "vhd", VHD},
  {".vhdx", "vhd", VHD},
  {".efi", "efi", EFI},
  {".wim", "wim", WIM},
};

static char defaule_icon[] = "file";

char *
grubfm_get_file_type (struct grubfm_enum_file_info *info)
{
  char *icon = defaule_icon;
  if (!info || !info->name)
    goto ret;
  info->type = UNKNOWN;
  char *ext = grub_strrchr (info->name, '.');
  if (!ext)
    goto ret;
  int i;
  int n = sizeof (grubfm_file_table) / sizeof (struct grubfm_file_ext);
  for (i = 0; i < n; i++)
  {
    if (grub_strcasecmp (ext, grubfm_file_table[i].ext) == 0)
    {
      icon = grubfm_file_table[i].icon;
      info->type = grubfm_file_table[i].type;
      break;
    }
  }
ret:
  return icon;
}
