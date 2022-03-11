//  Harbor - A portable and highly customisable game engine
//  Copyright (C) 2022 Semphris <semphris@protonmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "fs/file.hpp"

#include <cstdlib>
#include <stdexcept>

#include <physfs.h>
#include <string>

std::vector<PHYSFS_File*> File::s_undeleted_files;

void
File::error(const std::string& msg)
{
  auto code = PHYSFS_getLastErrorCode();
  std::string physfs_msg(PHYSFS_getErrorByCode(code));
  throw std::runtime_error(msg + " (Physfs error): " + physfs_msg + " (code " +
                           std::to_string(code) + ")");
}

std::vector<std::string>
File::get_mount_points()
{
  std::vector<std::string> mounts;

  char** physfs_mounts = PHYSFS_getSearchPath();

  if (!physfs_mounts)
  {
    error("Couldn't list mount points");
  }

  while(*physfs_mounts != nullptr)
  {
    mounts.push_back(*physfs_mounts);
    physfs_mounts++;
  } 

  PHYSFS_freeList(physfs_mounts);

  return mounts;
}

std::string
File::get_write_mount_point()
{
  const char* dir = PHYSFS_getWriteDir();
  return dir ? dir : "";
}

void
File::mount(const std::string& path, const std::string& point, bool append)
{
  if (!PHYSFS_mount(path.c_str(), point.c_str(), append))
    error("Couldn't mount '" + path + "' to '" + point + "'");
}

void
File::mount_file(File& file, const std::string& point, bool append)
{
  auto path = file.get_os_path();
  bool s = PHYSFS_mount(path.c_str(), point.c_str(), append);

  if (!s)
    error("Couldn't mount '" + path + "' to '" + point + "'");
}

void
File::unmount(const std::string& path)
{
  if (!PHYSFS_unmount(path.c_str()))
    error("Couldn't unmount '" + path);
}

std::vector<std::string>
File::list_dir(const std::string& point)
{
  std::vector<std::string> files;

  char** physfs_files = PHYSFS_enumerateFiles(point.c_str());

  if (!physfs_files)
  {
    error("Couldn't list mount points");
  }

  while(*physfs_files != nullptr)
  {
    files.push_back(*physfs_files);
    physfs_files++;
  } 

  PHYSFS_freeList(physfs_files);

  return files;
}

std::string
File::get_mount_for(const std::string& point)
{
  const char* path = PHYSFS_getMountPoint(point.c_str());

  if (!path)
    error("Couldn't get mount point for '" + point + "'");

  return path;
}

std::string
File::get_base_dir()
{
  return PHYSFS_getBaseDir();
}

std::string
File::get_pref_dir(const std::string& org_name, const std::string& app_name)
{
  const char* path = PHYSFS_getPrefDir(org_name.c_str(), app_name.c_str());

  if (!path)
    error("Couldn't get prefdir for '" + org_name + "' and '" + app_name + "'");

  return path;
}

void
File::set_write_dir(const std::string& path)
{
  if (!PHYSFS_setWriteDir(path.c_str()))
    error("Could not set write dir to '" + path + "'");
}

std::string
File::get_write_dir()
{
  const char* dir = PHYSFS_getWriteDir();
  return dir ? dir : "";
}

bool
File::exists(const std::string& point)
{
  return PHYSFS_exists(point.c_str());
}

void
File::remove(const std::string& point)
{
  if (!PHYSFS_delete(point.c_str()))
    error("Couldn't delete file or directory");
}

std::string
File::build_os_path(const std::vector<std::string>& names)
{
  std::string result;

  for (const auto& name : names)
  {
    if (!result.empty())
      result += PHYSFS_getDirSeparator();

    result += name;
  }

  return result;
}

std::vector<std::string>
File::split_os_path(const std::string& path)
{
  std::vector<std::string> names;

  std::string p = path;
  std::string separator = PHYSFS_getDirSeparator();

  size_t pos = 0;
  while ((pos = p.find(separator)) != std::string::npos) {
    names.push_back(p.substr(0, pos));
    p.erase(0, pos + separator.length());
  }
  names.push_back(p);

  return names;
}

std::string
File::build_path(const std::vector<std::string>& names)
{
  std::string result;

  for (const auto& name : names)
  {
    if (!result.empty())
      result += "/";

    result += name;
  }

  return result;
}

std::vector<std::string>
File::split_path(const std::string& path)
{
  std::vector<std::string> names;

  std::string p = path;
  std::string separator = "/";

  size_t pos = 0;
  while ((pos = p.find(separator)) != std::string::npos) {
    names.push_back(p.substr(0, pos));
    p.erase(0, pos + separator.length());
  }
  names.push_back(p);

  return names;
}

std::string
File::get_os_path(const std::string& point)
{
  std::vector<std::string> paths;

  paths.push_back(PHYSFS_getRealDir(point.c_str()));
  for (const auto& name : split_path(point))
  {
    paths.push_back(name);
  }

  return build_os_path(paths);
}

File::File(const std::string& point, Mode mode) :
  m_file(),
  m_path(point)
{
  switch(mode)
  {
    case Mode::APPEND:
      m_file = PHYSFS_openAppend(point.c_str());
      break;

    case Mode::READ:
      m_file = PHYSFS_openRead(point.c_str());
      break;

    case Mode::WRITE:
      m_file = PHYSFS_openWrite(point.c_str());
      break;
  }

  if (!m_file)
  {
    error("Could not open file '" + point + "' for operation "
          + std::to_string(static_cast<int>(mode)));
  }
}

File::~File()
{
  if (!PHYSFS_close(m_file))
    s_undeleted_files.push_back(m_file);
}

uint64_t
File::read_bytes(uint64_t len, void* out_buffer)
{
  auto read = PHYSFS_readBytes(m_file, out_buffer, len);

  if (read == -1)
  {
    error("Couldn't read " + std::to_string(len) + " bytes from '" + m_path +
          "'");
  }

  return read;
}

std::string
File::read_text(uint64_t len)
{
  // FIXME: This is done the C way. Should be redone the C++ way to avoid
  // messy memory management.
  char* buffer = (char*) malloc(len);

  auto read = PHYSFS_readBytes(m_file, buffer, len);

  if (read == -1)
  {
    free(buffer);
    error("Couldn't read " + std::to_string(len) + " bytes from '" + m_path +
          "'");
  }

  std::string text(buffer, read);
  free(buffer);
  return text;
}

void
File::write_bytes(uint64_t len, const void* buffer)
{
  auto written = PHYSFS_writeBytes(m_file, buffer, len);

  if (written != len)
  {
    error("Couldn't write " + std::to_string(len) + " bytes to '" + m_path +
          "', wrote " + std::to_string(written));
  }
}

void
File::write_text(const std::string& data)
{
  write_bytes(data.length(), data.c_str());
}

void
File::seek(uint64_t pos)
{
  if (!PHYSFS_seek(m_file, pos))
  {
    error("Couldn't seek at " + std::to_string(pos) + " in file '" + m_path +
          "'");
  }
}

uint64_t
File::tell()
{
  auto pos = PHYSFS_tell(m_file);

  if (pos < 0)
  {
    error("Couldn't tell offset in file '" + m_path + "'");
  }

  return pos;
}

bool
File::eof()
{
  return PHYSFS_eof(m_file);
}

void
File::set_buffer(uint64_t size)
{
  if (!PHYSFS_setBuffer(m_file, size))
    error("Couldn't set buffer of '" + m_path + "' to " + std::to_string(size));
}

void
File::flush()
{
  if (!PHYSFS_flush(m_file))
    error("Couldn't flush '" + m_path + "'");
}

uint64_t
File::get_access_time()
{
  return get_stat().accesstime;
}

uint64_t
File::get_create_time()
{
  return get_stat().createtime;
}

uint64_t
File::get_modif_time()
{
  return get_stat().modtime;
}

uint64_t
File::get_size()
{
  return get_stat().filesize;
}

bool
File::is_file()
{
  return get_stat().filetype == PHYSFS_FileType::PHYSFS_FILETYPE_REGULAR;
}

bool
File::is_directory()
{
  return get_stat().filetype == PHYSFS_FileType::PHYSFS_FILETYPE_DIRECTORY;
}

bool
File::is_symlink()
{
  return get_stat().filetype == PHYSFS_FileType::PHYSFS_FILETYPE_SYMLINK;
}

bool
File::is_readonly()
{
  return get_stat().readonly;
}

std::string
File::get_path()
{
  return m_path;
}

std::string
File::get_os_path()
{
  return get_os_path(get_path());
}

PHYSFS_Stat
File::get_stat()
{
  PHYSFS_Stat stat;

  if (!PHYSFS_stat(m_path.c_str(), &stat))
    error("Couldn't fetch stat for '" + m_path + "'");

  return stat;
}
