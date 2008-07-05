//  Pingus - A free Lemmings clone
//  Copyright (C) 2008 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <string>
#include "../pingus_error.hpp"
#include "../file_reader.hpp"
#include "drawable_factory.hpp"
#include "pingus_worldmap.hpp"

using namespace WorldmapNS;

class PingusWorldmapImpl
{
public:
  std::string filename;

  std::string name;
  std::string short_name;
  std::string description;
  std::string music;
  std::string author;
  std::string email;
  int width;
  int height;

  std::string default_node;
  std::string final_node;

  FileReader intro_story;
  FileReader end_story;
  FileReader path_graph;

  std::vector<FileReader> objects;
};

PingusWorldmap::PingusWorldmap()
{
}

PingusWorldmap::PingusWorldmap(const Pathname& pathname)
  : impl(new PingusWorldmapImpl())
{
  parse_file(FileReader::parse(pathname));
}
 
void
PingusWorldmap::parse_file(FileReader reader)
{
  if (reader.get_name() == "pingus-worldmap")
    {
      if (!reader.read_section("graph", impl->path_graph))
        {
          PingusError::raise("Worldmap: " + impl->filename + " is missed 'graph' section");
        }

      impl->objects = reader.read_section("objects").get_sections();

      parse_properties(reader.read_section("head"));

      reader.read_section("intro_story", impl->intro_story);
      reader.read_section("end_story", impl->end_story);
    }
  else
    {
      PingusError::raise("Worldmap:" + impl->filename + ": not a Worldmap file");
    }
}

void
PingusWorldmap::parse_properties(FileReader reader)
{
  reader.read_string("music",  impl->music);
  reader.read_string("author", impl->author);
  reader.read_string("name",   impl->name);
  reader.read_string("short-name", impl->short_name);
  reader.read_string("email",  impl->email);
  reader.read_int("width",     impl->width);
  reader.read_int("height",    impl->height);

  reader.read_string("default-node", impl->default_node);
  reader.read_string("final-node",   impl->final_node);
}

std::string
PingusWorldmap::get_name() const 
{
  return impl->name; 
}

std::string
PingusWorldmap::get_short_name() const
{
  return impl->short_name;
}

std::string
PingusWorldmap::get_description() const
{
  return impl->description;
}

std::string
PingusWorldmap::get_music() const
{
  return impl->music;
}

std::string
PingusWorldmap::get_author() const
{
  return impl->author;
}

std::string
PingusWorldmap::get_email() const
{
  return impl->email;
}

int
PingusWorldmap::get_width() const
{
  return impl->width;
}

int
PingusWorldmap::get_height() const
{
  return impl->height;
}

std::string
PingusWorldmap::get_default_node() const
{
  return impl->default_node;
}

std::string
PingusWorldmap::get_final_node() const
{
  return impl->final_node;
}

FileReader
PingusWorldmap::get_graph() const
{
  return impl->path_graph;
}

const std::vector<FileReader>&
PingusWorldmap::get_objects() const
{
  return impl->objects;
}

/* EOF */
