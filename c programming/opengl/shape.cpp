// $Id: shape.cpp,v 1.1 2015-07-16 16:47:51-07 - - $

#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include "shape.h"
#include "util.h"
#include "graphics.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (const string& glut_bitmap_font, const string& textdata):
      glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width/2, height/2}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
   polygon({ {-width/2, height/2}, {width/2, height/2},
   {width/2, -height/2}, {-width/2, -height/2} }) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height):
   polygon({ {height/2, 0}, {0,width/2}, {-height/2,0}, {0,-width/2} })
{ }

triangle::triangle (GLfloat width):
   polygon({ {-width/2, -.289f*width}, {width/2, -.289f*width},
   {0, 0.577f*width} }) {
    
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   auto ptr = fontcode.find(glut_bitmap_font);
   glColor3ubv(color.ubvec);
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (ptr->second, reinterpret_cast<const GLubyte*>
      (textdata.c_str()));
}

void text::draw_border (const vertex& center,
                        const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   /*auto ptr = fontcode.find(glut_bitmap_font);
   glColor3ubv(color.ubvec);
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (ptr->second, reinterpret_cast<const GLubyte*>
      (textdata.c_str()));*/
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3ubv (color.ubvec);
   const float delta = 2 * M_PI / 32;
   GLfloat xpos;
   GLfloat ypos;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      xpos = dimension.xpos * cos (theta) + center.xpos;
      ypos = dimension.ypos * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   glEnd();
}

void ellipse::draw_border (const vertex& center,
                           const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   /*glLineWidth(window::get_border_thickness());
   glBegin (GL_LINE_LOOP);
   glEnable (GL_LINE_SMOOTH);
   glColor3ubv (color.ubvec);
   const float delta = 2 * M_PI / 32;
   GLfloat xpos;
   GLfloat ypos;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      xpos = dimension.xpos * cos (theta) + center.xpos;
      ypos = dimension.ypos * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   glEnd();*/
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   GLfloat center_x = 0;
   GLfloat center_y = 0;
    
   for (unsigned int k = 0; k < vertices.size(); k++)
   {
      center_x+= vertices[k].xpos;
      center_y+= vertices[k].ypos;
   }
    
   center_x = center_x / vertices.size();
   center_y = center_y / vertices.size();
    
   glBegin(GL_POLYGON);
   glColor3ubv(color.ubvec);
   GLfloat x;
   GLfloat y;
   for (unsigned int i = 0; i < vertices.size(); i++)
   {
      x = vertices[i].xpos + center.xpos - center_x;
      y = vertices[i].ypos + center.ypos - center_y;
      glVertex2f(x,y);
   }
   glEnd();
}

void polygon::draw_border(const vertex& center,
                          const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");

   /*GLfloat center_x = 0;
   GLfloat center_y = 0;
    
   for (unsigned int k = 0; k < vertices.size(); k++)
   {
      center_x+= vertices[k].xpos;
      center_y+= vertices[k].ypos;
   }
    
   center_x = center_x / vertices.size();
   center_y = center_y / vertices.size();

   glLineWidth(window::get_border_thickness());
   glBegin(GL_LINE_LOOP);
   glColor3ubv(color.ubvec);
   GLfloat x;
   GLfloat y;
   for (unsigned int i = 0; i < vertices.size(); i++)
   {
      x = vertices[i].xpos + center.xpos - center_x;
      y = vertices[i].ypos + center.ypos - center_y;
      glVertex2f(x,y);
   }
   glEnd();*/
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   /*out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";*/
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

