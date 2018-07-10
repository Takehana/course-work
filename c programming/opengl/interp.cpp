// $Id: interp.cpp,v 1.2 2015-07-16 16:57:30-07 - - $

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"border" , &interpreter::do_border },
   {"moveby" , &interpreter::do_moveby },
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"diamond"  , &interpreter::make_diamond   },
   {"triangle" , &interpreter::make_polygon   },
   {"equilateral" , &interpreter::make_equilateral   },
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   shape_ptr ptr = make_shape(++begin, end);
   objmap.emplace (name, ptr);
   object obj(name, ptr);
   window::push_back(obj);
}


void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   rgbcolor color {begin[0]};
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   //itor->second->draw (where, color);
   window::set_center(name, where);
   window::set_color(name, color);
}

void interpreter::do_border (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   /*rgbcolor clr {*begin};
   window::set_border_color(clr);
   window::set_border_thickness(std::stof((begin[1]).c_str(), 0));
   window::enable_border();*/
}

void interpreter::do_moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   /*++begin;
   window::distance = std::stof((*begin).c_str(), 0);
   return;*/
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //return make_shared<text> (nullptr, string());
   string font = *begin;
   string content = "";
    
   while(++begin != end)
   {
      content += (*begin) + " ";
   }
   return make_shared<text> (font, content);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //return make_shared<ellipse> (GLfloat(), GLfloat());
   GLfloat width = std::stof((*begin).c_str(),0);
   ++begin;
   GLfloat height = std::stof((*begin).c_str(),0);
    
   return make_shared<ellipse> (width, height);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //return make_shared<circle> (GLfloat());
   GLfloat radius = std::stof((*begin).c_str(), 0);
    
   return make_shared<circle> (radius);
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //return make_shared<polygon> (vertex_list());
   vertex_list list;
   GLfloat x;
   GLfloat y;
    
   while(begin != end)
   {
      x = std::stof((*begin).c_str(), 0);
      ++begin;
      y = std::stof((*begin).c_str(), 0);
      ++begin;
      list.push_back({x,y});
   }
   return make_shared<polygon> (list);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //return make_shared<rectangle> (GLfloat(), GLfloat());
    
   GLfloat width = std::stof((*begin).c_str(),0);
   ++begin;
   GLfloat height = std::stof((*begin).c_str(),0);

   return make_shared<rectangle> (width, height);
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //return make_shared<square> (GLfloat());
    
   GLfloat edge = std::stof((*begin).c_str(), 0);
    
   return make_shared<square> (edge);
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));

   GLfloat width = std::stof((*begin).c_str(), 0);
   ++begin;
   GLfloat height = std::stof((*begin).c_str(), 0);
    
   return make_shared<diamond> (width, height);
}

shape_ptr interpreter::make_equilateral (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //return make_shared<square> (GLfloat());
    
   GLfloat edge = std::stof((*begin).c_str(), 0);
    
   return make_shared<triangle> (edge);
}
