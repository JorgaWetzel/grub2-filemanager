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

static void
grubfm_open_win (grub_file_t file, char *path)
{
  grub_env_set ("run_file", path);
  grub_env_export ("run_file");
  grub_script_execute_sourcecode ("source ${prefix}/install/install.sh");
}

static void
grubfm_open_map (grub_file_t file UNUSED, char *path)
{
  char *src = NULL;
  src = grub_xasprintf ("map \"%s\"", path);
  if (!src)
    return;
  grub_script_execute_sourcecode (src);
  grub_free (src);
}

static void
grubfm_open_efi (grub_file_t file UNUSED, char *path)
{
  char *src = NULL;
  src = grub_xasprintf ("chainloader -b -t \"%s\"", path);
  if (!src)
    return;
  grub_script_execute_sourcecode (src);
  grub_free (src);
}

static void
grubfm_open_vhd (grub_file_t file UNUSED, char *path)
{
  char *src = NULL;
  src = grub_xasprintf ("ntboot --gui"
                        " --efi=${prefix}/ms/bootmgfw.efi"
                        " \"%s\"", path);
  if (!src)
    return;
  grub_script_execute_sourcecode (src);
  grub_free (src);
}

static void
grubfm_open_wim (grub_file_t file UNUSED, char *path)
{
  char *src = NULL;
  src = grub_xasprintf ("wimboot @:bootmgfw.efi:${prefix}/ms/bootmgfw.efi"
                        " @:bcd:${prefix}/ms/bcd"
                        " @:boot.sdi:${prefix}/ms/boot.sdi"
                        " @:boot.wim:\"%s\"", path);
  if (!src)
    return;
  grub_script_execute_sourcecode (src);
  grub_free (src);
}

void
grubfm_open_file (char *path)
{
  struct grubfm_enum_file_info info;
  grub_file_t file = 0;
  file = grub_file_open (path, GRUB_FILE_TYPE_GET_SIZE |
                           GRUB_FILE_TYPE_NO_DECOMPRESS);
  if (!file)
    return;
  info.name = file->name;
  info.size = (char *) grub_get_human_size (file->size,
                                            GRUB_HUMAN_SIZE_SHORT);
  grubfm_get_file_type (&info);
  switch (info.type)
  {
    case ISO:
      grubfm_open_win (file, path);
      grubfm_open_map (file, path);
      break;
    case DISK:
      grubfm_open_map (file, path);
      break;
    case VHD:
      grubfm_open_vhd (file, path);
      break;
    case EFI:
      grubfm_open_efi (file, path);
      break;
    case WIM:
      grubfm_open_wim (file, path);
      break;
    default:
      break;
  }
  grub_file_close (file);
}
