// $Id: file_sys.cpp,v 1.5 2016-01-14 16:16:52-08 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <string>
#include <map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
    
    root = make_shared<inode>(file_type::DIRECTORY_TYPE);
    cwd = root;
    root->path = "/";
    std::pair<string, inode_ptr> dot = std::make_pair(".", root);
    std::pair<string, inode_ptr> dotdot = std::make_pair ("..", root);

    root->contents->get_dirents().insert(dot);
    root->contents->get_dirents().insert(dotdot);
}

const string& inode_state::prompt() const { return prompt_; }

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);

    for (unsigned int i = 0; i < data.size(); i++)
    {
        size += data[i].length();
    }
    
   return size;
}

void plain_file::readfile(const string& filename) const {
   DEBUGF ('i', data);
    
    for (unsigned int i = 0; i < data.size() - 1; i++)
    {
        cout << data[i] << " ";
    }
    cout << data[data.size() - 1] << endl;
    
    //cout << "read file here" << endl;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
    
    data = words;
}

void plain_file::remove (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (const string&, inode_state& state) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkfile (const string&) {
   throw file_error ("is a plain file");
}


size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
    size = dirents.size();
   return size;
}

void directory::readfile(const string& filename) const {
   //throw file_error ("is a directory");
    
    //cout << "read file here" << endl;
    bool isFile = false;
    
    for (auto i = dirents.begin(); i != dirents.end(); i++)
    {
        if (i->first == filename)
        {
            isFile = true;
            inode_ptr t = i->second;
            t->get_base()->readfile(filename);
        }
    }
    
    if(!isFile)
    {
        cout << "cat: " << filename
            << ": No such file or directory" << endl;
    }
}

void directory::writefile (const wordvec& words) {
   //throw file_error ("is a directory");
    
    std::shared_ptr<inode> plain =
        make_shared<inode>(file_type::PLAIN_TYPE);
    plain->get_base()->writefile(words);
    
    dirents.insert(std::pair<string, inode_ptr>(filename, plain));
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);

    std::map<string, inode_ptr>::iterator i;
    i = dirents.find(filename);
    if (i != dirents.end())
        dirents.erase(i);
    else
        cout << "rm: " << filename
            << ": No such file or directory" << endl;
}

inode_ptr directory::mkdir (const string& dirname, inode_state& state) {
   DEBUGF ('i', dirname);
    
    //cout << "mkdir here " << endl;
    
    inode_ptr dir = make_shared<inode>(file_type::DIRECTORY_TYPE);
    dir->set_root(state.get_cwd());
    
    inode_ptr root_ptr = dir->get_root();
    
    std::pair<string, inode_ptr> cur = std::make_pair(".", dir);
    std::pair<string, inode_ptr> root = std::make_pair ("..", root_ptr);
    std::pair<string, inode_ptr> this_dir
        = std::make_pair(dirname, dir);
    
    dir->get_base()->get_dirents().insert(cur);
    dir->get_base()->get_dirents().insert(root);
    
    if (root_ptr->get_path() != "/")
    {
        dir->get_path() += root_ptr->get_path();
    }
    dir->get_path() += "/" + dirname;
    root_ptr->get_base()->get_dirents().insert(this_dir);
    return nullptr;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
    this->filename = filename;
   return nullptr;
}

void directory::print_dirents() {
    for (auto t = dirents.begin(); t != dirents.end(); t++)
    {
        cout <<"   " << t->second->get_inode_num() << "   " <<
            t->second->get_base()->size() << "   " << t->first;
        if (t->second->get_base()->isDir())
            cout << "/";
        cout << endl;
    }
}
//map<string, inode_ptr>& directory::get_dirents() { }
//map<string, inode_ptr>& directory::get_dirents() {return dirents; }
