// $Id: cppstrtok.cpp,v 1.7 2016-09-30 11:45:04-07 - - $

// Use cpp to scan a file and print line numbers.
// Print out each input line read in, then strtok it for
// tokens.

#include <string>
using namespace std;

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <wait.h>

#include "cppstrtok.h"
#include "string_set.h"

constexpr size_t LINESIZE = 1024;

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

// Print the meaning of a signal.
static void eprint_signal (const char* kind, int signal) {
   fprintf (stderr, ", %s %d", kind, signal);
   const char* sigstr = strsignal (signal);
   if (sigstr != NULL) fprintf (stderr, " %s", sigstr);
}

// Print the status returned from a subprocess.
void eprint_status (const char* command, int status) {
   if (status == 0) return; 
   fprintf (stderr, "%s: status 0x%04X", command, status);
   if (WIFEXITED (status)) {
      fprintf (stderr, ", exit %d", WEXITSTATUS (status));
   }
   if (WIFSIGNALED (status)) {
      eprint_signal ("Terminated", WTERMSIG (status));
      #ifdef WCOREDUMP
      if (WCOREDUMP (status)) fprintf (stderr, ", core dumped");
      #endif
   }
   if (WIFSTOPPED (status)) {
      eprint_signal ("Stopped", WSTOPSIG (status));
   }
   if (WIFCONTINUED (status)) {
      fprintf (stderr, ", Continued");
   }
   fprintf (stderr, "\n");
}


// Run cpp against the lines of the file.
void cpplines (FILE* pipe, const char* filename) {
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy (inputname, filename);
   for (;;) {
      char buffer[LINESIZE];
      char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == NULL) break;
      chomp (buffer, '\n');
      printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
      // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, inputname);
      if (sscanf_rc == 2) {
         printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, inputname);
         continue;
      }
      char* savepos = NULL;
      char* bufptr = buffer;
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = NULL;
         if (token == NULL) break;
          string_set::intern(token);
         printf ("token %d.%d: [%s]\n",
                 linenr, tokenct, token);
      }
      ++linenr;
   }
}

