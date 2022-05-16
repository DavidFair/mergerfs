/*
  ISC License

  Copyright (c) 2022, Antonio SJ Musumeci <trapexit@spawn.link>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "fuse_mkdir_policy_ff.hpp"
#include "fuse_mkdir_mkdir.hpp"

#include "fs_clonepath_branches.hpp"


FUSE::MKDIR::POLICY::FF::FF(const toml::value &toml_)
  : _branches(toml_)
{

}

int
FUSE::MKDIR::POLICY::FF::operator()(const gfs::path &fusepath_,
                                    const mode_t     mode_,
                                    const mode_t     umask_)
{
  int rv;
  gfs::path fullpath;

  for(const auto &branch_group : _branches)
    {
      for(const auto &branch : branch_group)
        {
          fullpath  = branch.path / fusepath_;

          rv = FUSE::MKDIR::mkdir(fullpath,mode_,umask_);
          if(rv == -ENOENT)
            {
              rv = fs::clonepath_as_root(_branches,branch.path,fusepath_);
              if(rv >= 0)
                rv = FUSE::MKDIR::mkdir(fullpath,mode_,umask_);
            }

          return rv;
        }
    }

  return -ENOENT;
}