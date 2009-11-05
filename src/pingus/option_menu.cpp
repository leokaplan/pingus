//  Pingus - A free Lemmings clone
//  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
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

#include "pingus/option_menu.hpp"

#include <set>
#include <boost/bind.hpp>
#include <sstream>
#include <iostream>

#include "pingus/config_manager.hpp"
#include "pingus/globals.hpp"
#include "gettext.h"
#include "pingus/resource.hpp"
#include "engine/screen/screen_manager.hpp"
#include "pingus/fonts.hpp"
#include "engine/display/display.hpp"
#include "engine/display/drawing_context.hpp"
#include "pingus/components/label.hpp"
#include "pingus/components/check_box.hpp"
#include "pingus/components/slider_box.hpp"
#include "pingus/components/choice_box.hpp"
#include "engine/gui/gui_manager.hpp"
#include "engine/sound/sound.hpp"
#include "tinygettext/dictionary_manager.hpp"
#include "tinygettext/language.hpp"

#define C(x) connections.push_back(x)

extern tinygettext::DictionaryManager dictionary_manager;

class OptionMenuCloseButton
  : public GUI::SurfaceButton
{
private:
  OptionMenu* parent;
public:
  OptionMenuCloseButton(OptionMenu* p, int x, int y)
    : GUI::SurfaceButton(x, y,
                         "core/start/ok",
                         "core/start/ok_clicked",
                         "core/start/ok_hover"),
      parent(p)
  {
  }

  void on_pointer_enter ()
  {
    SurfaceButton::on_pointer_enter();
    Sound::PingusSound::play_sound("tick");
  }

  void on_click() {
    parent->save_language();
    config_manager.save();
    parent->close_screen();
    Sound::PingusSound::play_sound("yipee");
  }

private:
  OptionMenuCloseButton(const OptionMenuCloseButton&);
  OptionMenuCloseButton & operator=(const OptionMenuCloseButton&);
};

OptionMenu::OptionMenu() :
  background(),
  ok_button(),
  x_pos(),
  y_pos(),
  options(),
  fullscreen_box(),
  swcursor_box(),
  autoscroll_box(),
  mousegrab_box(),
  printfps_box(),
  master_volume_box(),
  sound_volume_box(),
  music_volume_box(),
  defaults_label(),
  defaults_box(),
  connections(),
  language(),
  language_map()
{
  background = Sprite("core/menu/optionmenu");
  gui_manager->add(ok_button = new OptionMenuCloseButton(this, 
                                                         Display::get_width()/2 + 225,
                                                         Display::get_height()/2 + 125));

  x_pos = 0;
  y_pos = 0;

  int resolutions[][2] = {
    { 640, 480 },   // 4:3, VGA
    { 768, 576 },   // 4:3, PAL
    { 800, 480 },   // Nokia N770, N800
    { 800, 600 },   // 4:3, SVGA
    { 1024, 768 },  // 4:3, XGA
    { 1152, 864 },  // 4:3
    { 1280, 720 },  // 16:9, HD-TV, 720p
    { 1280, 960 },  // 4:3
    { 1280, 1024 }, // 5:4
    { 1366, 768 },  // ~16:9, Wide XGA
    { 1440, 900, }, // 16:10
    { 1600, 1200 }, // 4:3, UXGA
    { 1680, 1050 }, // 16:10, WSXGA
    { 1920, 1080 }, // 16:9, HD-TV, 1080p
    { 1920, 1200 }, // 16:10
    { -1, -1 }
  };
  int current_choice = -1;
  int n;

  ChoiceBox* resolution_box = new ChoiceBox(Rect());
  for (n = 0; resolutions[n][0] != -1; ++n)
  {
    std::ostringstream ostr;
    ostr << resolutions[n][0] << "x" << resolutions[n][1];
    resolution_box->add_choice(ostr.str());
    if (Display::get_width()  == resolutions[n][0] &&
        Display::get_height() == resolutions[n][1])
    {
      current_choice = n;
    }
  }
  resolution_box->add_choice("Custom");
  if (current_choice == -1)
    current_choice = n;

  resolution_box->set_current_choice(current_choice);

  tinygettext::Language current_language = dictionary_manager.get_language();
  language = current_language;
  n = 0;

  ChoiceBox* language_box = new ChoiceBox(Rect());
  std::set<tinygettext::Language> languages = dictionary_manager.get_languages();

  for (std::set<tinygettext::Language>::iterator i = languages.begin(); i != languages.end(); ++i)
  {
    language_box->add_choice(i->str());
    if (current_language == *i)
      language_box->set_current_choice(current_choice);
  }

  ChoiceBox* scroll_box = new ChoiceBox(Rect());
  scroll_box->add_choice("Drag&Drop");
  scroll_box->add_choice("Rubberband");
  
  swcursor_box      = new CheckBox(Rect());
  fullscreen_box    = new CheckBox(Rect());
  autoscroll_box    = new CheckBox(Rect());
  mousegrab_box     = new CheckBox(Rect());
  printfps_box      = new CheckBox(Rect());

  master_volume_box = new SliderBox(Rect());
  sound_volume_box  = new SliderBox(Rect());
  music_volume_box  = new SliderBox(Rect());

  C(swcursor_box->on_change.connect(boost::bind(&OptionMenu::on_swcursor_change, this, _1)));
  C(fullscreen_box->on_change.connect(boost::bind(&OptionMenu::on_fullscreen_change, this, _1)));
  C(autoscroll_box->on_change.connect(boost::bind(&OptionMenu::on_autoscroll_change, this, _1)));
  C(mousegrab_box->on_change.connect(boost::bind(&OptionMenu::on_mousegrab_change, this, _1)));
  C(printfps_box->on_change.connect(boost::bind(&OptionMenu::on_printfps_change, this, _1)));

  C(master_volume_box->on_change.connect(boost::bind(&OptionMenu::on_master_volume_change, this, _1)));
  C(sound_volume_box->on_change.connect(boost::bind(&OptionMenu::on_sound_volume_change, this, _1)));
  C(music_volume_box->on_change.connect(boost::bind(&OptionMenu::on_music_volume_change, this, _1)));

  C(language_box->on_change.connect(boost::bind(&OptionMenu::on_language_change, this, _1)));
  C(resolution_box->on_change.connect(boost::bind(&OptionMenu::on_resolution_change, this, _1)));

  add_item(_("Language:"),        language_box);
  //  add_item(_("Scroll Mode:"),     scroll_box);
  add_item(_("Resolution:"),      resolution_box);
  add_item(_("Fullscreen:"),      fullscreen_box);
  add_item(_("Master Volume:"),   master_volume_box);
  add_item(_("Sound Volume:"),    sound_volume_box);
  add_item(_("Music Volume:"),    music_volume_box);
  add_item(_("Autoscrolling:"),   autoscroll_box);
  add_item(_("Print FPS:"),       printfps_box);
  add_item(_("Mouse Grab:"),      mousegrab_box);
  add_item(_("Software Cursor:"), swcursor_box);

  // Connect with ConfigManager
  mousegrab_box->set_state(config_manager.get_mouse_grab(), false);
  C(config_manager.on_mouse_grab_change.connect(boost::bind(&CheckBox::set_state, mousegrab_box, _1, false)));

  printfps_box->set_state(config_manager.get_print_fps(), false);
  C(config_manager.on_print_fps_change.connect(boost::bind(&CheckBox::set_state, printfps_box, _1, false)));

  fullscreen_box->set_state(config_manager.get_fullscreen(), false);
  C(config_manager.on_fullscreen_change.connect(boost::bind(&CheckBox::set_state, fullscreen_box, _1, false)));

  swcursor_box->set_state(config_manager.get_swcursor(), false);
  C(config_manager.on_swcursor_change.connect(boost::bind(&CheckBox::set_state, swcursor_box, _1, false)));

  autoscroll_box->set_state(config_manager.get_autoscroll(), false);
  C(config_manager.on_autoscroll_change.connect(boost::bind(&CheckBox::set_state, autoscroll_box, _1, false)));

  defaults_label = new Label(_("Reset to Defaults:"), Rect(Vector2i(Display::get_width()/2 - 100, Display::get_height()/2 + 160), Size(170, 32)));
  gui_manager->add(defaults_label);
  defaults_box = new CheckBox(Rect(Vector2i(Display::get_width()/2 - 100 + 170, Display::get_height()/2 + 160), Size(32, 32)));
  gui_manager->add(defaults_box);
}

void
OptionMenu::add_item(const std::string& label, GUI::RectComponent* control)
{
  int x_offset = (Display::get_width() - 800) / 2;
  int y_offset = (Display::get_height() - 600) / 2;

  Label* label_component = new Label(label, Rect(Vector2i(120 + x_pos * 312 + x_offset, 177 + y_pos*32 + y_offset), 
                                                 Size(140, 32)));
  gui_manager->add(label_component);
  gui_manager->add(control);

  if (dynamic_cast<ChoiceBox*>(control))
    {
      control->set_rect(Rect(120 + x_pos * 312 + 140 + x_offset, 177 + y_pos*32 + y_offset,
                             120 + x_pos * 312 + 256 + x_offset, 177 + y_pos*32 + 32 + y_offset));                             
    }
  else if (dynamic_cast<SliderBox*>(control))
    {
      control->set_rect(Rect(120 + x_pos * 312 + 140 + x_offset, 177 + y_pos*32 + y_offset,
                             120 + x_pos * 312 + 256 + x_offset, 177 + y_pos*32 + 32 + y_offset));
    }
  else if (dynamic_cast<CheckBox*>(control))
    {
      control->set_rect(Rect(Vector2i(120 + x_pos * 312 + 156 + 32+28+8 + x_offset, 177 + y_pos*32 + y_offset), 
                             Size(32, 32)));
    }
  else
    {
      assert(!"Unhandled control type");
    }

  options.push_back(Option(label_component, control));

  y_pos += 1;
  if (y_pos > 5)
    {
      y_pos = 0; 
      x_pos += 1;
    }
}

OptionMenu::~OptionMenu()
{
  for(Connections::iterator i = connections.begin(); i != connections.end(); ++i)
    {
      (*i).disconnect();
    }
}
  
struct OptionEntry {
  OptionEntry(const std::string& left_,
              const std::string& right_)
    : left(left_), right(right_)
  {}
  
  std::string left;
  std::string right;
};

void
OptionMenu::draw_background(DrawingContext& gc)
{
  gc.fill_screen(Color(0, 0, 0));

  // gc.draw_fillrect(Rect(100, 100, 400, 400), Color(255, 0, 0));
  gc.draw(background, Vector2i(gc.get_width()/2 - background.get_width()/2, gc.get_height()/2 - background.get_height()/2));

  gc.print_center(Fonts::chalk_large,
                  Vector2i(gc.get_width()/2,
                           gc.get_height()/2 - 210),
                  _("Option Menu"));

  gc.print_center(Fonts::chalk_normal, Vector2i(gc.get_width()/2 + 225 + 30, gc.get_height()/2 + 125 - 20), _("Close"));
}

void
OptionMenu::on_escape_press()
{
  std::cout << "OptionMenu: popping screen" << std::endl;
  ScreenManager::instance()->pop_screen();
}

void
OptionMenu::resize(const Size& size_)
{
  GUIScreen::resize(size_);

  if (ok_button)
    ok_button->set_pos(size.width/2 + 225, size.height/2 + 125);
  if (defaults_label)
    defaults_label->set_rect(Rect(Vector2i(Display::get_width()/2 - 100, Display::get_height()/2 + 160), Size(170, 32)));
  if (defaults_box)
    defaults_box->set_rect(Rect(Vector2i(Display::get_width()/2 - 100 + 170, Display::get_height()/2 + 160), Size(32, 32)));

  if (options.empty())
    return;

  Rect rect;
  rect = options.front().label->get_rect();
  int x_diff = 120 + (size.width - 800) / 2 - rect.left;
  int y_diff = 177 + (size.height - 600) / 2 - rect.top;

  for(std::vector<Option>::iterator i = options.begin(); i != options.end(); ++i)
    {
      rect = (*i).label->get_rect();
      (*i).label->set_rect(Rect(Vector2i(rect.left + x_diff, rect.top + y_diff), rect.get_size()));
      rect = (*i).control->get_rect();
      (*i).control->set_rect(Rect(Vector2i(rect.left + x_diff, rect.top + y_diff), rect.get_size()));
    }
}

void
OptionMenu::close_screen()
{
  ScreenManager::instance()->pop_screen();
}

void
OptionMenu::on_swcursor_change(bool v)
{
  config_manager.set_swcursor(v);
}

void
OptionMenu::on_fullscreen_change(bool v)
{
  config_manager.set_fullscreen(v);
}

void
OptionMenu::on_autoscroll_change(bool v)
{
  config_manager.set_autoscroll(v);
}

void
OptionMenu::on_mousegrab_change(bool v)
{
  config_manager.set_mouse_grab(v);
}

void
OptionMenu::on_printfps_change(bool v)
{
  config_manager.set_print_fps(v);
}

void
OptionMenu::on_master_volume_change(int v)
{
  config_manager.set_master_volume(v);
}

void
OptionMenu::on_sound_volume_change(int v)
{
  config_manager.set_sound_volume(v);
}

void
OptionMenu::on_music_volume_change(int v)
{
  config_manager.set_music_volume(v);
}

void
OptionMenu::on_language_change(const std::string &str)
{
  language = str;
}

void
OptionMenu::on_resolution_change(const std::string& str)
{
  if (str != "Custom")
    {
      Size size_;
      if (sscanf(str.c_str(), "%dx%d", &size_.width, &size_.height) == 2)
        {
          config_manager.set_resolution(size_); 
        }
    }
}

void
OptionMenu::save_language()
{
  config_manager.set_language(language_map[language]);
}

/* EOF */
