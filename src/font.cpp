/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include <iostream>
#include <vector>
#include "SDL.h"
#include "SDL_image.h"
#include "font.hpp"
#include "font_description.hpp"

static bool vline_empty(SDL_Surface* surface, int x, Uint8 threshold)
{
  Uint8* pixels = (Uint8*)surface->pixels;

  for(int y = 0; y < surface->h; ++y)
    {
      const Uint8& p = pixels[surface->pitch*y + x*surface->format->BytesPerPixel + 3];
      if (p > threshold)
        {
          return false;
        }
    }
  return true;
}

class FontImpl
{
public:
  SDL_Surface* surface;
  SDL_Rect chrs[256];
  int space_length;
  
  FontImpl(const FontDescription& desc)
    : surface(0),
      space_length(desc.space_length)
  {
    //std::cout << "desc.image: " << desc.image << std::endl;
    //std::cout << "desc.space: " << desc.space_length << std::endl;
    //std::cout << "Characters: " << desc.characters << std::endl;

    for(int i = 0; i < 256; ++i)
      chrs[i].x = chrs[i].y = chrs[i].w = chrs[i].h = 0;

    surface = IMG_Load(desc.image.c_str());
    assert(surface);

    SDL_LockSurface(surface);
    
    //std::cout << "Surface: " << surface->w << std::endl;
    int first = -1; // -1 signals no character start found yet
    int idx = 0;
    for(int x = 0; x < surface->w; ++x)
      {
        ///std::cout << x << " " << surface->w << std::endl;
        if (vline_empty(surface, x, desc.alpha_threshold))
          {
            if (first != -1) // skipping empty space
              {
                if (idx < int(desc.characters.size()))
                  {
                    //std::cout << idx << " " << desc.characters[idx]
                    //<< " Empty: " << first << " - " << x << std::endl;

                    SDL_Rect& rect = chrs[static_cast<unsigned char>(desc.characters[idx])];
                    rect.x = first;
                    rect.y = 0;
                    rect.w = x - first;
                    rect.h = surface->h;
                  }
                else
                  std::cout << "Error: Found more desc.characters then are mapped" << std::endl;

                idx += 1;
                first = -1;
              }
          }
        else
          {
            if (first == -1) // found the start of a character
              first = x;
          }
      }
    
    if (idx-1 != int(desc.characters.size())) // FIXME: is that -1 correct?!
      {
        std::cout << "Font: " << desc.image << "\n"
                  << "  Error: " << idx-1 << " expected "  << desc.characters.size() << "\n"
                  << "  Format: bpp: " << int(surface->format->BitsPerPixel) << "\n"
                  << "  Size: " << surface->w << "x" << surface->h
          //      << "  RMask: " << hex << surface->format->Rmask << "\n"
          //      << "  GMask: " << hex << surface->format->Gmask << "\n"
          //      << "  BMask: " << hex << surface->format->Bmask << "\n"
          //      << "  AMask: " << hex << surface->format->Amask << "\n"
                  << std::endl;
      }

    SDL_UnlockSurface(surface);
    //std::cout << "Font created successfully" << std::endl;
  }

  ~FontImpl()
  {
    SDL_FreeSurface(surface);
  }

  void draw(Origin origin, int x, int y, const std::string& text, SDL_Surface* target)
  {
    Vector2i offset = calc_origin(origin, get_size(text));

    SDL_Rect dstrect;
    dstrect.x = x - offset.x;
    dstrect.y = y - offset.y;


    for(std::string::size_type i = 0; i < text.size(); ++i)
      {
        if (text[i] == ' ')
          {
            dstrect.x += space_length;
          }
        else if (text[i] == '\n')
          {
            dstrect.x = x;
            dstrect.y += surface->h;
          }
        else
          {
            SDL_Rect& srcrect = chrs[static_cast<unsigned char>(text[i])];
            if (srcrect.w != 0 && srcrect.h != 0)
              {
                SDL_BlitSurface(surface, &srcrect, target, &dstrect);
                dstrect.x += srcrect.w+1;
              }
            else
              {
                //std::cout << "Font: character " << static_cast<unsigned char>(text[i]) << " missing in font" << std::endl;
              }
          }
      }
  }

  int get_height() const
  {
    return surface->h;
  }

  int get_width(char idx) const
  {
    return chrs[static_cast<unsigned char>(idx)].w;
  }

  int  get_width(const std::string& text) const
  {
    int width = 0;
    int last_width = 0;
    for(std::string::size_type i = 0; i < text.size(); ++i)
      {
        if (text[i] == ' ')
          {
            width += space_length;
          }
        else if (text[i] == '\n')
          {
            last_width = std::max(last_width, width);
            width = 0;
          }
        else
          {
            width += chrs[static_cast<unsigned char>(text[i])].w+1;
          }
      }
    return std::max(width, last_width);
  }

  Size get_size(const std::string& text) const
  {
    return Size(get_width(text), surface->h);
  }

  Rect bounding_rect(int x, int y, const std::string& str) const
  {
    return Rect(Vector2i(x, y), get_size(str));
  }
};

Font::Font()
  : impl(0)
{
}

Font::Font(const FontDescription& desc)
  : impl(new FontImpl(desc))
{
}

void
Font::draw(int x, int y, const std::string& text, SDL_Surface* target)
{
  if (impl)
    impl->draw(origin_top_left, x,y,text, target);
}

void
Font::draw(Origin origin, int x, int y, const std::string& text, SDL_Surface* target)
{
  if (impl)
    impl->draw(origin, x,y,text, target); 
}

int
Font::get_height() const
{
  if (impl)
    return impl->get_height();
  else
    return 0;
}

int
Font::get_width(char c) const
{
  if (impl)
    return impl->get_width(c);
  else
    return 0; 
}

int
Font::get_width(const std::string& text) const
{
  if (impl)
    return impl->get_width(text);
  else
    return 0;  
}

Size
Font::get_size(const std::string& str) const
{
  if (impl)
    return impl->get_size(str);
  else
    return Size(); 
}

Rect
Font::bounding_rect(int x, int y, const std::string& str) const
{
  if (impl)
    return impl->bounding_rect(x, y, str);
  else
    return Rect();
}

/* EOF */