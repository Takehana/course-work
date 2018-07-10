// $Id: cix.cpp,v 1.4 2016-05-09 16:01:56-07 - - $

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream log (cout);
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS  },
   {"get" , cix_command::GET },
   {"put" , cix_command::PUT },
   {"rm"  , cix_command::RM  },
};

void cix_help() {
   static const vector<string> help = {
      "exit         - Exit the program.  Equivalent to EOF.",
      "get filename - Copy remote file to local host.",
      "help         - Print help summary.",
      "ls           - List names of files on remote server.",
      "put filename - Copy local file to remote host.",
      "rm filename  - Remove file from remote server.",
   };
   for (const auto& line: help) cout << line << endl;
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != cix_command::LSOUT) {
      log << "sent LS, server did not return LSOUT" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.nbytes + 1];
      recv_packet (server, buffer, header.nbytes);
      log << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer;
   }
}

void cix_get (client_socket& server, string filename) {
   cix_header header;
   header.command = cix_command::GET;
   strcpy (header.filename, filename.c_str());
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != cix_command::FILE) {
      log << "server returned " << header << endl;
      log << filename << " : No such file on server." << endl;
   }else {
      char buffer[header.nbytes + 1];
      recv_packet (server, buffer, header.nbytes);
      log << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      
      ofstream writter (header.filename, ios::binary);
      if (writter) {
         writter.write(buffer, header.nbytes);
         writter.close();
         log << "copied " << filename << " from server." << endl;
      }
      else {
         log << filename << ": write file not successful." << endl;
      }
   }
}

void cix_put (client_socket& server, string filename) {
   cix_header header;
   ifstream reader (filename, ios::binary);
   
   if (reader) {
      reader.seekg(0, reader.end);
      int length = reader.tellg();
      reader.seekg(0, reader.beg);

      char * buffer = new char [length];
      reader.read(buffer, length);
      reader.close();

      header.command = cix_command::PUT;
      strcpy (header.filename, filename.c_str());
      header.nbytes = length;
      log << "sending header " << header << endl;
      send_packet (server, &header, sizeof header);
      send_packet (server, buffer, length);
      log << "sent " << length << " bytes" << endl;
   }
   else {
      log << filename << ": No such file in current directory." << endl;
      return;
   }
    
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != cix_command::ACK) {
      log << "server returned " << header << endl;
      log << "putting " << filename;
      log << " on server is not successful." << endl;
   }else {
      log << "successfully put " << filename << " on server." << endl;
   }
}

void cix_rm (client_socket& server, string filename) {
   cix_header header;
   header.command = cix_command::RM;
   strcpy (header.filename, filename.c_str());
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != cix_command::ACK) {
      log << "sent RM, server did not return ACK" << endl;
      log << "server returned " << header << endl;
      log << "removing " << filename
         << "from server was not successful" << endl;
   }else {
      log << "removed " << filename << " from server." << endl;
   }
}

void usage() {
   cerr << "Usage: " << log.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

int main (int argc, char** argv) {
   log.execname (basename (argv[0]));
   log << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   log << to_string (hostinfo()) << endl;
   try {
      log << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      log << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         vector<string> tok;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         istringstream iss(line);
         while (iss) {
            string word;
            iss >> word;
            tok.push_back(word);
         }
         log << "command " << line << endl;
         const auto& itor = command_map.find (tok[0]);
         cix_command cmd = itor == command_map.end()
                         ? cix_command::ERROR : itor->second;
         switch (cmd) {
            case cix_command::EXIT:
               throw cix_exit();
               break;
            case cix_command::HELP:
               cix_help();
               break;
            case cix_command::LS:
               cix_ls (server);
               break;
            case cix_command::GET:
               cix_get(server, tok[1]);
               tok.clear();
               break;
            case cix_command::PUT:
               cix_put(server, tok[1]);
               tok.clear();
               break;
            case cix_command::RM:
               cix_rm(server, tok[1]);
               tok.clear();
               break;
            default:
               log << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      log << error.what() << endl;
   }catch (cix_exit& error) {
      log << "caught cix_exit" << endl;
   }
   log << "finishing" << endl;
   return 0;
}

