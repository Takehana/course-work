// $Id: main.cpp,v 1.16 2016-10-06 16:42:35-07 - - $

#include <string>
#include <vector>
using namespace std;

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include "astree.h"
#include "auxlib.h"
//#include "emitter.h"
#include "lyutils.h"
#include "string_set.h"

const string cpp_name = "/usr/bin/cpp";
string cpp_command;
const string str = ".str";
const string tok = ".tok";
const string ast = ".ast";
char* filenamestr;
char* filenametok;
char* filenameast;

// Open a pipe from the C preprocessor.
// Exit failure if can't.
// Assigns opened pipe to FILE* yyin.
void cpp_popen (const char* filename) {
   cpp_command = cpp_name + " " + filename;
   yyin = popen (cpp_command.c_str(), "r");
   if (yyin == NULL) {
      syserrprintf (cpp_command.c_str());
      exit (exec::exit_status);
   }else {
      if (yy_flex_debug) {
         fprintf (stderr, "-- popen (%s), fileno(yyin) = %d\n",
                  cpp_command.c_str(), fileno (yyin));
      }
      lexer::newfilename (cpp_command);
   }
}

void cpp_pclose() {
   int pclose_rc = pclose (yyin);
   eprint_status (cpp_command.c_str(), pclose_rc);
   if (pclose_rc != 0) exec::exit_status = EXIT_FAILURE;
}
void scan_opts (int argc, char** argv) {
   opterr = 0;
   yy_flex_debug = 0;
   yydebug = 0;
   lexer::interactive = isatty (fileno (stdin))
                    and isatty (fileno (stdout));
   for(;;) {
      int opt = getopt (argc, argv, "@:ly");
      if (opt == EOF) break;
      switch (opt) {
         case '@': set_debugflags (optarg);   break;
         case 'l': yy_flex_debug = 1;         break;
         case 'y': yydebug = 1;               break;
         default:  errprintf ("bad option (%c)\n", optopt); break;
      }
   }
   if (optind > argc) {
      errprintf ("Usage: %s [-ly] [filename]\n",
                 exec::execname.c_str());
      exit (exec::exit_status);
   }
   const char* filename = optind == argc ? "-" : argv[optind];
    //rename tok
    string st(filename);
    string name_only = st.substr(0, strlen(filename)-3);
    string new_nametok = name_only + tok;
    char* nametok = new char[0];
    strcpy(nametok, new_nametok.c_str());
    filenametok = nametok;
    //rename str
    string new_namestr = name_only + str;
    char* namestr = new char[0];
    strcpy(namestr, new_namestr.c_str());
    filenamestr = namestr;
    //rename ast
    string new_nameast = name_only + ast;
    char* nameast = new char[0];
    strcpy(nameast, new_nameast.c_str());
    filenameast = nameast;
   cpp_popen (filename);
}

void fileout() {
    FILE* outstr = fopen(filenamestr, "w");
    string_set::dump(outstr);
    fclose(outstr);
    FILE* outast = fopen(filenameast, "w");
    astree::print(outast, parser::root);
    delete parser::root;
    fclose(outast);
}

int main (int argc, char** argv) {
   exec::execname = basename (argv[0]);
   if (yydebug or yy_flex_debug) {
      fprintf (stderr, "Command:");
      for (char** arg = &argv[0]; arg < &argv[argc]; ++arg) {
            fprintf (stderr, " %s", *arg);
      }
      fprintf (stderr, "\n");
   }
   scan_opts (argc, argv);
   int parse_rc = yyparse();
   cpp_pclose();
   yylex_destroy();
   if (yydebug or yy_flex_debug) {
      fprintf (stderr, "Dumping parser::root:\n");
      if (parser::root != nullptr) parser::root->dump_tree (stderr);
      fprintf (stderr, "Dumping string_set:\n");
      string_set::dump (stderr);
   }
   if (parse_rc) {
      errprintf ("parse failed (%d)\n", parse_rc);
   }else {
      /*
      FILE* output = fopen(filenamestr, "w");
      string_set::dump(output);
      astree::print (stdout, parser::root);
      //emit_sm_code (parser::root);
      delete parser::root;
       */
      fileout();
   }
   return exec::exit_status;
}

