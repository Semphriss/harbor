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

#ifndef _HEADER_HARBOR_FS_FILE_HPP
#define _HEADER_HARBOR_FS_FILE_HPP

#include <string>
#include <vector>

#include "physfs.h"

class File final
{
public:
  enum class Mode
  {
    READ,
    WRITE,
    APPEND
  };

private:
  static std::vector<PHYSFS_File*> s_undeleted_files;

private:
  static void error(const std::string& msg);

public:
  static std::vector<std::string> get_mount_points();
  static std::string get_write_mount_point();
  static void mount(const std::string& path, const std::string& point = "/",
                    bool append = true);
  static void mount_file(File& file, const std::string& point = "/",
                         bool append = true);
  static void unmount(const std::string& path);
  static std::vector<std::string> list_dir(const std::string& point);
  static std::string get_mount_for(const std::string& point);
  static std::string get_base_dir();
  static std::string get_pref_dir(const std::string& org_name,
                                  const std::string& app_name);
  static void set_write_dir(const std::string& path);
  static std::string get_write_dir();

  static bool exists(const std::string& point);
  static void remove(const std::string& point);

  static std::string build_os_path(const std::vector<std::string>& names);
  static std::vector<std::string> split_os_path(const std::string& path);
  static std::string build_path(const std::vector<std::string>& names);
  static std::vector<std::string> split_path(const std::string& point);
  static std::string get_os_path(const std::string& point);

public:
  File(const std::string& point, Mode mode);
  ~File();

  uint64_t read_bytes(uint64_t len, void* out_buffer);
  std::string read_text(uint64_t len = 2147463846);
  void write_bytes(uint64_t len, const void* buffer);
  void write_text(const std::string& data);
  void seek(uint64_t pos);
  uint64_t tell();
  bool eof();
  void set_buffer(uint64_t size);
  void flush();

  uint64_t get_access_time();
  uint64_t get_create_time();
  uint64_t get_modif_time();
  uint64_t get_size();
  bool is_file();
  bool is_directory();
  bool is_symlink();
  bool is_readonly();

  std::string get_path();
  std::string get_os_path();

private:
  PHYSFS_Stat get_stat();

private:
  PHYSFS_File* m_file;
  std::string m_path;

private:
  File(const File&) = delete;
  File& operator=(const File&) = delete;
};

#endif
