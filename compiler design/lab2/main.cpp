// $Id: main.cpp,v 1.16 2016-10-06 16:42:35-07 - - $

#include <string>
#include <vector>
#include <libgen.h>
using namespace std;

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "astree.h"
#include "auxlib.h"
//#include "emitter.h"
#include "lyutils.h"
#include "string_set.h"

const string cpp_name = "/usr/bin/cpp";
string cpp_command;
char* filename;

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
   cpp_popen (filename);
}

bool checkname(char* filename){
    string str(filename);
    string surfix = ".oc";
    if (str.substr(strlen(filename)-3) != surfix)
        return false;
    return true;
}

char* renamestr(char* filename){
    string surfix = ".str";
    string str(filename);
    string name_only = str.substr(0, strlen(filename)-3);
    string new_name = name_only + surfix;
    char* name = new char[0];
    strcpy(name, new_name.c_str());
    
    
    return name;
}

char* renametok(char* filename){
    string surfix = ".tok";
    string str(filename);
    string name_only = str.substr(0, strlen(filename)-3);
    string new_name = name_only + surfix;
    char* name = new char[0];
    strcpy(name, new_name.c_str());
    
    
    return name;
}

int yylval_token (int symbol) {
    char* newname = renametok(filename);
    FILE* outfile = fopen(newname, "a");
    yylval = new astree (symbol, lexer::lloc, yytext);
    astree::dump(outfile, yylval);
    //astree::dump(stdout, yylval);
    fclose(outfile);
    return symbol;
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
   //scan_opts (argc, argv);
    for (int argi = 1; argi < argc; ++argi) {
        filename = argv[argi];
        if (!checkname(filename)){
            exec::exit_status = EXIT_FAILURE;
            fprintf (stderr, "invalid file name: %s\n", filename);
            continue;
        }
        cpp_popen(filename);
        int parse_rc = yyparse();
        cpp_pclose();
        yylex_destroy();
        if (yydebug or yy_flex_debug) {
            //fprintf (stderr, "Dumping parser::root:\n");
            //if (parser::root != nullptr)
            //    parser::root->dump_tree (stderr);
            //fprintf (stderr, "Dumping string_set:\n");
            char* outstr = renamestr(filename);
            FILE* output = fopen(outstr, "w");
            string_set::dump (output);
            fclose(output);
        }
        if (parse_rc) {
            errprintf ("parse failed (%d)\n", parse_rc);
        }else {
            //astree::print (stdout, parser::root);
            //emit_sm_code (parser::root);
            //delete parser::root;
        }
    }
    return exec::exit_status;
}

