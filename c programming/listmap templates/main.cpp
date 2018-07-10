// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <regex>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   str_str_map test;
   str_str_pair pair("test", "test");
   test.insert(pair);
   test.erase(test.begin());

   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};
   if (argc == 1) {
      int linenr = 0;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) break;
         linenr++;
         //-: 1: a=b
         cout << "-: " << linenr << ": " << line << endl;
         smatch result;
         if (regex_search (line, result, comment_regex)) {
            //cout << "Comment or empty line." << endl;
            continue;
         }
         if (regex_search (line, result, key_value_regex)) {
            //cout << "key  : \"" << result[1] << "\"" << endl;
            //cout << "value: \"" << result[2] << "\"" << endl;
            // key=value
            if(result[1] != "" && result[2] != "") {
               str_str_pair pair(result[1], result[2]);
               test.insert(pair);
               cout << result[1] << " = " << result[2] << endl;
            }
            // =value
            else if(result[1] == "" && result[2] != "") {
               //cout << "find value: " << result[2] << ": " << endl;
               for (str_str_map::iterator itor = test.begin();
                  itor != test.end(); ++itor) {
                  itor = test.find_value(itor, result[2]);
                  if (itor == test.end())
                     break;
                  //cout << *itor << endl;
                  test.display(itor);
               }
            }
            // key=
            else if(result[1] != "" && result[2] == "") {
               str_str_map::iterator itor = test.find(result[1]);
               test.erase(itor);
            }
            // =
            else if(result[1] == "" && result[2] == "") {
               for (str_str_map::iterator itor = test.begin();
                  itor != test.end(); ++itor) {
                  //cout << *itor << endl;
                  test.display(itor);
               }
            }
            // key
         }else if (regex_search (line, result, trimmed_regex)) {
            //cout << "query: \"" << result[1] << "\"" << endl;
            str_str_map::iterator itor = test.find(result[1]);
            //cout << *itor << endl;
            if (itor == test.end())
               cout << result[1] << ": key not found" << endl;
            test.display(itor);
         }else {
            cout << "This can not happen." << endl;
         }
      }
   }
   else
   {
      fstream infile;
      int linenr = 0;
        
      for (int i = 1; i < argc; i++)
      {
         infile.open(argv[i]);
         if (infile.is_open())
         {
            for (;;)
            {
               string line;
               getline(infile, line);
               if (infile.eof())
                  break;
               //test1.in: 1: a=b
               linenr++;
               cout << argv[i] << ": " << linenr
                  << ": " << line << endl;
                    
               smatch result;
               if (regex_search (line, result, comment_regex)) {
                  //cout << "Comment or empty line." << endl;
                  continue;
               }
               if (regex_search (line, result, key_value_regex)) {
                  //cout << "key  : \"" << result[1] << "\"" << endl;
                  //cout << "value: \"" << result[2] << "\"" << endl;
                  // key=value
               if(result[1] != "" && result[2] != "") {
                  str_str_pair pair(result[1], result[2]);
                  test.insert(pair);
                  cout << result[1] << " = " << result[2] << endl;
               }
               // =value
               else if(result[1] == "" && result[2] != "") {
                  //cout << "find value: " << result[2] << ": " << endl;
                  for (str_str_map::iterator itor = test.begin();
                     itor != test.end(); ++itor) {
                     itor = test.find_value(itor, result[2]);
                     if (itor == test.end())
                        break;
                     //cout << *itor << endl;
                     test.display(itor);
                  }
               }
               // key=
               else if(result[1] != "" && result[2] == "") {
                  str_str_map::iterator itor = test.find(result[1]);
                  test.erase(itor);
               }
               // =
               else if(result[1] == "" && result[2] == "") {
                  for (str_str_map::iterator itor = test.begin();
                     itor != test.end(); ++itor) {
                        //cout << *itor << endl;
                        test.display(itor);
                     }
                  }
               // key
               }else if (regex_search (line, result, trimmed_regex)) {
                  //cout << "query: \"" << result[1] << "\"" << endl;
                  str_str_map::iterator itor = test.find(result[1]);
                  //cout << *itor << endl;
                  if (itor == test.end())
                     cout << result[1] << ": key not found" << endl;
                  test.display(itor);
               }else {
                  cout << "This can not happen." << endl;
               }
            }
            infile.close();
            linenr = 0;
         }
         else
         {
            cout << argv[0] << ": " << argv[i]
               << ": No such file or directory" << endl;
         }
      }
   }
   return 0;
}

