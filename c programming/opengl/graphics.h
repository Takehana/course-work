// $Id: graphics.h,v 1.1 2015-07-16 16:47:51-07 - - $

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <memory>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "rgbcolor.h"
#include "shape.h"
#include "interp.h"

class object {
   private:
      shared_ptr<shape> pshape;
      vertex center { 320, 240 };
      rgbcolor color {255, 0, 0};
      string name;
      rgbcolor border_color {255, 0, 0};
      int border_thickness = 4;
      bool has_border;
   public:
      // Default copiers, movers, dtor all OK.
      void draw() { pshape->draw (center, color); }
      void draw_border() { pshape->draw_border (center, color); }
      void move (GLfloat delta_x, GLfloat delta_y) {
         center.xpos += delta_x;
         center.ypos += delta_y;
      }
      object(string name1, shared_ptr<shape> ptr) {
        name = name1;
        pshape = ptr;
      }
      void set_center (vertex v) {
         center.xpos = v.xpos;
         center.ypos = v.ypos;
      }
      void set_color (const rgbcolor& color1) { color = color1; }
      string get_name() { return name; }
      void set_border_thickness (const int thick) {
         border_thickness = thick;
      }
      void set_border_color (const rgbcolor& color) {
         border_color = color;
      }
      void enable_border() { has_border = true; }
      int get_border_thickness() { return border_thickness; }
      rgbcolor get_border_color() { return border_color; }
      bool have_border() { return has_border; }
};

class mouse {
      friend class window;
   private:
      int xpos {0};
      int ypos {0};
      int entered {GLUT_LEFT};
      int left_state {GLUT_UP};
      int middle_state {GLUT_UP};
      int right_state {GLUT_UP};
   private:
      void set (int x, int y) { xpos = x; ypos = y; }
      void state (int button, int state);
      void draw();
};


class window {
      friend class mouse;
   private:
      static int width;         // in pixels
      static int height;        // in pixels
      static vector<object> objects;
      static size_t selected_obj;
      static mouse mus;
      static int border_thickness;
      static rgbcolor border_color;
      static bool has_border;
   private:
      static void close();
      static void entry (int mouse_entered);
      static void display();
      static void reshape (int width, int height);
      static void keyboard (GLubyte key, int, int);
      static void special (int key, int, int);
      static void motion (int x, int y);
      static void passivemotion (int x, int y);
      static void mousefn (int button, int state, int x, int y);
   public:
      static void push_back (const object& obj) {
                  objects.push_back (obj); }
      static void setwidth (int width_) { width = width_; }
      static void setheight (int height_) { height = height_; }
      static void main();
      static void set_center(string name, vertex v);
      static void set_color(string name, const rgbcolor& color);
      static void set_border_thickness(const int thick);
      static void set_border_color(const rgbcolor& color);
      static int get_border_thickness();
      static rgbcolor get_border_color();
      static void enable_border();
      static bool have_border();
      static int distance;
};

#endif

