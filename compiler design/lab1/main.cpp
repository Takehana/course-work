// $Id: main.cpp,v 1.2 2016-08-18 15:13:48-07 - - $

#include <string>
using namespace std;

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <libgen.h>

#include "string_set.h"
#include "cppstrtok.h"


const string CPP = "/usr/bin/cpp -nostdinc";

bool checkname(char* filename){
    string str(filename);
    string surfix = ".oc";
    if (str.substr(strlen(filename)-3) != surfix)
        return false;
    return true;
}

char* rename(char* filename){
    string surfix = ".str";
    string str(filename);
    string name_only = str.substr(0, strlen(filename)-3);
    string new_name = name_only + surfix;
    char* name = new char[0];
    strcpy(name, new_name.c_str());
    
    
    return name;
}

int main(int argc, char** argv) {
    const char* execname = basename (argv[0]);
    int exit_status = EXIT_SUCCESS;
    for (int argi = 1; argi < argc; ++argi) {
        char* filename = argv[argi];
        if (!checkname(filename)){
            exit_status = EXIT_FAILURE;
            fprintf (stderr, "invalid file name: %s\n", filename);
            continue;
        }
            
        string command = CPP + " " + filename;
        //string cat = "cat";
        //string command = cat + " " + filename;
        //printf ("command=\"%s\"\n", command.c_str());
        FILE* pipe = popen (command.c_str(), "r");
        if (pipe == NULL) {
            exit_status = EXIT_FAILURE;
            fprintf (stderr, "%s: %s: %s\n",
                     execname, command.c_str(), strerror (errno));
        }else {
            cpplines (pipe, filename);
            int pclose_rc = pclose (pipe);
            eprint_status (command.c_str(), pclose_rc);
            if (pclose_rc != 0) exit_status = EXIT_FAILURE;
        }
        
        char* output_file = rename(filename);
        FILE* output = fopen(output_file, "w");
        string_set::dump(output);
        fclose(output);
    }
    return exit_status;
}

