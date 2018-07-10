// $Id: graphics.cpp,v 1.3 2016-07-20 21:33:16-07 - - $

#include <iostream>
using namespace std;

#include <GL/freeglut.h>

#include "graphics.h"
#include "util.h"

int window::width = 640; // in pixels
int window::height = 480; // in pixels
int window::border_thickness = 0;
rgbcolor window::border_color {255, 0, 0};
bool window::has_border = false;
int window::distance = 10;
vector<object> window::objects;
size_t window::selected_obj = 0;
mouse window::mus;

// Executed when window system signals to shut down.
void window::close() {
   DEBUGF ('g', sys_info::execname() << ": exit ("
           << sys_info::exit_status() << ")");
   exit (sys_info::exit_status());
}

// Executed when mouse enters or leaves window.
void window::entry (int mouse_entered) {
   DEBUGF ('g', "mouse_entered=" << mouse_entered);
   window::mus.entered = mouse_entered;
   if (window::mus.entered == GLUT_ENTERED) {
      DEBUGF ('g', sys_info::execname() << ": width=" << window::width
           << ", height=" << window::height);
   }
   glutPostRedisplay();
}

// Called to display the objects in the window.
void window::display() {
   glClear (GL_COLOR_BUFFER_BIT);
    for (auto& object: window::objects) {
        object.draw();
        /*if (object.have_border()) {
            object.draw_border();
        }*/
    }
   mus.draw();
   glutSwapBuffers();
}

// Called when window is opened and when resized.
void window::reshape (int width, int height) {
   DEBUGF ('g', "width=" << width << ", height=" << height);
   window::width = width;
   window::height = height;
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D (0, window::width, 0, window::height);
   glMatrixMode (GL_MODELVIEW);
   glViewport (0, 0, window::width, window::height);
   glClearColor (0.25, 0.25, 0.25, 1.0);
   glutPostRedisplay();
}


// Executed when a regular keyboard key is pressed.
void window::keyboard (GLubyte key, int x, int y) {
   enum {BS = 8, TAB = 9, ESC = 27, SPACE = 32, DEL = 127};
   DEBUGF ('g', "key=" << unsigned (key) << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   switch (key) {
      case 'Q': case 'q': case ESC:
         window::close();
         break;
      case 'H': case 'h':
      {
         //move_selected_object (
         objects[selected_obj].move(-1 * distance, 0.0);
         break;
      }
      case 'J': case 'j':
      {
         //move_selected_object (
         objects[selected_obj].move(0.0, -1 * distance);
         break;
      }
      case 'K': case 'k':
      {
         //move_selected_object (
         objects[selected_obj].move(0.0, distance);
         break;
      }
      case 'L': case 'l':
      {
         //move_selected_object (
         objects[selected_obj].move(distance, 0.0);
         break;
      }
      case 'N': case 'n': case SPACE: case TAB:
      {
         if (selected_obj == objects.size() - 1)
            selected_obj = 0;
         else
            ++selected_obj;
         break;
      }
      case 'P': case 'p': case BS:
      {
         if (selected_obj == 0)
            selected_obj = objects.size() - 1;
         else
            --selected_obj;
         break;
      }
      case '0'...'9':
      {
         size_t selection = key - '0';
         if (selection >= objects.size())
         {
            cerr << "out of range" << endl;
         }
         else
         {
            selected_obj = selection;
         }
         break;
      }
      default:
         cerr << unsigned (key) << ": invalid keystroke" << endl;
         break;
   }
   glutPostRedisplay();
}


// Executed when a special function key is pressed.
void window::special (int key, int x, int y) {
   DEBUGF ('g', "key=" << key << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   switch (key) {
      case GLUT_KEY_LEFT: //move_selected_object (-1, 0); break;
      {
          //move_selected_object (
          objects[selected_obj].move(-1 * distance, 0.0);
          break;
      }
      case GLUT_KEY_DOWN: //move_selected_object (0, -1); break;
      {
         //move_selected_object (
         objects[selected_obj].move(0.0, -1 * distance);
         break;
      }
      case GLUT_KEY_UP: //move_selected_object (0, +1); break;
      {
         //move_selected_object (
         objects[selected_obj].move(0.0, distance);
         break;
      }
      case GLUT_KEY_RIGHT: //move_selected_object (+1, 0); break;
      {
         //move_selected_object (
         objects[selected_obj].move(distance, 0.0);
         break;
      }
      case GLUT_KEY_F1: //select_object (1); break;
      {
         if (1 >= objects.size())
         {
            cerr << "out of range" << endl;
         }
         else
         {
            selected_obj = 1;
         }
         break;
      }
      case GLUT_KEY_F2: //select_object (2); break;
      {
         if (2 >= objects.size())
         {
            cerr << "out of range" << endl;
         }
         else
         {
            selected_obj = 2;
         }
         break;
      }
      case GLUT_KEY_F3: //select_object (3); break;
      {
         if (3 >= objects.size())
         {
            cerr << "out of range" << endl;
         }
         else
         {
            selected_obj = 3;
         }
         break;
      }
      case GLUT_KEY_F4: //select_object (4); break;
      {
         if (4 >= objects.size())
         {
            cerr << "out of range" << endl;
         }
         else
         {
            selected_obj = 4;
         }
         break;
      }
      case GLUT_KEY_F5: //select_object (5); break;
      {
         if (5 >= objects.size())
         {
            cerr << "out of range" << endl;
         }
         else
         {
            selected_obj = 5;
         }
         break;
      }
      case GLUT_KEY_F6: //select_object (6); break;
      {
         if (6 >= objects.size())
         {
            cerr << "out of range" << endl;
         }
         else
         {
            selected_obj = 6;
         }
         break;
      }
      case GLUT_KEY_F7: //select_object (7); break;
      {
         if (7 >= objects.size())
         {
            cerr << "out of range" << endl;
         }
         else
         {
            selected_obj = 7;
         }
         break;
      }
      case GLUT_KEY_F8: //select_object (8); break;
      {
         if (8 >= objects.size())
         {
            cerr << "out of range" << endl;
         }
         else
         {
            selected_obj = 8;
         }
         break;
      }
      case GLUT_KEY_F9: //select_object (9); break;
      {
         if (9 >= objects.size())
         {
            cerr << "out of range" << endl;
         }
         else
         {
            selected_obj = 9;
         }
         break;
      }
      case GLUT_KEY_F10: //select_object (10); break;
      {
         if (10 >= objects.size())
         {
            cerr << "out of range" << endl;
         }
         else
         {
            selected_obj = 10;
         }
         break;
      }
      case GLUT_KEY_F11: //select_object (11); break;
      {
         if (11 >= objects.size())
         {
            cerr << "out of range" << endl;
         }
         else
         {
            selected_obj = 11;
         }
         break;
      }
      case GLUT_KEY_F12: //select_object (12); break;
      {
         if (12 >= objects.size())
         {
            cerr << "out of range" << endl;
         }
         else
         {
            selected_obj = 12;
         }
         break;
      }
      default:
         cerr << unsigned (key) << ": invalid function key" << endl;
         break;
   }
   glutPostRedisplay();
}


void window::motion (int x, int y) {
   DEBUGF ('g', "x=" << x << ", y=" << y);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::passivemotion (int x, int y) {
   DEBUGF ('g', "x=" << x << ", y=" << y);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::mousefn (int button, int state, int x, int y) {
   DEBUGF ('g', "button=" << button << ", state=" << state
           << ", x=" << x << ", y=" << y);
   window::mus.state (button, state);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::main () {
   static int argc = 0;
   glutInit (&argc, nullptr);
   glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
   glutInitWindowSize (window::width, window::height);
   glutInitWindowPosition (128, 128);
   glutCreateWindow (sys_info::execname().c_str());
   glutCloseFunc (window::close);
   glutEntryFunc (window::entry);
   glutDisplayFunc (window::display);
   glutReshapeFunc (window::reshape);
   glutKeyboardFunc (window::keyboard);
   glutSpecialFunc (window::special);
   glutMotionFunc (window::motion);
   glutPassiveMotionFunc (window::passivemotion);
   glutMouseFunc (window::mousefn);
   DEBUGF ('g', "Calling glutMainLoop()");
   glutMainLoop();
}


void mouse::state (int button, int state) {
   switch (button) {
      case GLUT_LEFT_BUTTON: left_state = state; break;
      case GLUT_MIDDLE_BUTTON: middle_state = state; break;
      case GLUT_RIGHT_BUTTON: right_state = state; break;
   }
}

void mouse::draw() {
   static rgbcolor color ("green");
   ostringstream text;
   text << "(" << xpos << "," << window::height - ypos << ")";
   if (left_state == GLUT_DOWN) text << "L"; 
   if (middle_state == GLUT_DOWN) text << "M"; 
   if (right_state == GLUT_DOWN) text << "R"; 
   if (entered == GLUT_ENTERED) {
      void* font = GLUT_BITMAP_HELVETICA_18;
      glColor3ubv (color.ubvec);
      glRasterPos2i (10, 10);
      auto ubytes = reinterpret_cast<const GLubyte*>
                    (text.str().c_str());
      glutBitmapString (font, ubytes);
   }
}

void window::set_center(string name, vertex v) {
   for (unsigned int i = 0; i < objects.size(); i++)
   {
      if (objects[i].get_name() == name)
      {
         objects[i].set_center(v);
      }
   }
}

void window::set_color(string name, const rgbcolor& color) {
   for (unsigned int i = 0; i < objects.size(); i++)
   {
      if (objects[i].get_name() == name)
      {
         objects[i].set_color(color);
      }
   }
}

/*void window::set_border_color(const rgbcolor& color) {
   for (auto& object: window::objects)
      object.set_border_color(color);
}

void window::set_border_thickness(const int thick) {
   for (auto& object: window::objects)
      object.set_border_thickness(thick);
}

void window::enable_border() {
   for (auto& object: window::objects)
      object.enable_border();
}

int window::get_border_thickness() {
   return objects[0].get_border_thickness();
}

rgbcolor window::get_border_color() {
   return objects[0].get_border_color();
}

bool window::have_border() {
   return objects[0].have_border();
}*/



