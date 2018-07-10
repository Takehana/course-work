// $Id: commands.cpp,v 1.17 2018-01-25 14:02:55-08 - - $

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
   {"#"     , fn_nothing},
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    
    inode_ptr t = state.get_cwd();
    
    //t->get_base()->readfile();
    
    for (unsigned int i = 1; i < words.size(); i++)
    {
        t->get_base()->readfile(words[i]);
    }
    
    //cout << "root = " << state.get_root()
    //<< ", cwd = " << state.get_cwd()
    //<< ", prompt = \"" << state.prompt() << "\"" << endl;
    
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    
    bool validDir = false;
    //cout << "cd ->> : " << words << endl;
    
    if (words.size() > 2)
    {
        cout << "cd: " << words << ": More than one item" << endl;
        return;
    }
    if (words[1] == "" || words[1] == "/")
        validDir = true;
    
    std::map<string, inode_ptr> dir =
        state.get_cwd()->get_base()->get_dirents();
    for (auto i = dir.begin(); i != dir.end(); i++)
    {
        if (i->first == words[1])
        {
            //cout << "found " << i->first << endl;
            state.get_cwd() = i->second;
            validDir = true;
            break;
        }
    }
    if (!validDir)
        cout << "cd : " << words[1]
            << ": No such file or directory" << endl;
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    
    inode_ptr t = state.get_cwd();
    wordvec dotdot;
    dotdot.push_back("w/e");
    dotdot.push_back("..");
    
    if ( words.size() == 1)
    {
        cout << t->get_path() << ":" << endl;
        t->get_base()->print_dirents();
    }
    
    if ( words.size() == 2)
    {
        if (words[1] == "/")
        {
            cout << "/: " << endl;
            t->get_base()->print_dirents();
        }
        else if (words[1] == ".")
        {
            cout << ".: " << endl;
            t->get_base()->print_dirents();
        }
        else
        {
            fn_cd(state, words);
            t = state.get_cwd();
            cout << t->get_path() << ":" << endl;
            t->get_base()->print_dirents();
            fn_cd(state, dotdot);
        }
    }
    
    if (words.size() > 2)
    {
        cout << "ls: " << words << ": Too many arguments" << endl;
    }
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    
    inode_ptr t = state.get_cwd();
    wordvec dotdot;
    dotdot.push_back("w/e");
    dotdot.push_back("..");
    
    //cout << "current directory: ";
    //fn_pwd(s, words);
    //cout << endl;
    
    if ( words.size() == 1)
    {
        cout << t->get_path() << ":" << endl;
        t->get_base()->print_dirents();
    }
    
    if ( words.size() == 2)
    {
        if (words[1] == "/")
        {
            cout << "/: " << endl;
            t->get_base()->print_dirents();
        }
        else if (words[1] == ".")
        {
            cout << ".: " << endl;
            t->get_base()->print_dirents();
        }
        else
        {
            fn_cd(state, words);
            t = state.get_cwd();
            cout << t->get_path() << ":" << endl;
            t->get_base()->print_dirents();
            //fn_cd(state, dotdot);
        }
    }
    
    if (words.size() > 2)
    {
        cout << "ls: " << words << ": Too many arguments" << endl;
    }
    
    
    
    //traverse
    for (auto i = t->get_base()->get_dirents().begin();
         i != t->get_base()->get_dirents().end(); i++)
    {
        //cout << "now at: ";
        //fn_pwd(s, words);
        //cout << "visiting: " << i->first << endl;
        if (i->second->get_base()->isDir()
            && i->first != "." && i->first != "..")
        {
            wordvec temp;
            temp.clear();
            temp.push_back("w/e");
            temp.push_back(i->first);
            fn_lsr(state, temp);
            //cout << "back to parent, from current: ";
            //fn_pwd(s, words);
            fn_cd(state, dotdot);
        }
    }
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    
    inode_ptr t = state.get_cwd();
    t->get_base()->mkfile(words[1]);
    
    wordvec content;
    
    for (unsigned int i = 2; i < words.size(); i++)
    {
        content.push_back(words[i]);
    }
    t->get_base()->writefile(content);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    
    inode_ptr t = state.get_cwd();
    wordvec content;
    
    for(unsigned int i = 1; i < words.size(); i++)
    {
        string dir = words[i] + "/";
        content.push_back(dir);
        t->get_base()->mkdir(words[i], state);
        //cout << dir << endl;
    }
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    
    string prompt = "";
    
    for (unsigned int i = 1; i < words.size(); i++)
    {
        prompt += words[i] + " ";
    }
    state.inode_state::set_prompt(prompt);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    
    inode_ptr t = state.get_cwd();
    t->print_path();
    cout << endl;
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    
    inode_ptr t = state.get_cwd();
    for (unsigned int k = 1; k < words.size(); k++)
    {
        //t->get_base()->remove(words[k]);
        for (auto i = t->get_base()->get_dirents().begin();
             i != t->get_base()->get_dirents().end(); i++)
        {
            if (i->first == words[k])
            {
                if (i->second->get_base()->isDir() == true &&
                    i->second->get_base()->size() > 2)
                {
                    cout << "rm: " << words[k]
                        << ": Directory not empty" << endl;
                    continue;
                }
                else
                    t->get_base()->remove(words[k]);
            }
                
        }
    }
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    
    inode_ptr t = state.get_cwd();
    for (unsigned int k = 1; k < words.size(); k++)
    {
        t->get_base()->remove(words[k]);
    }
}

void fn_nothing(inode_state& state, const wordvec& words) {}
