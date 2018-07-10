// $Id: listmap.tcc,v 1.11 2018-01-25 14:19:14-08 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next, node* prev,
                                     const value_type& value):
            link (next, prev), value (value) {
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
}


//
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   //return iterator();
   if (size == 0)
   {
      node* empty = new node(nullptr, nullptr, pair);
      root = empty;
      node* newnode = new node(root, root, pair);
      root->prev = newnode;
      root->next = newnode;
      size++;
      return iterator(newnode);
   }
   else
   {
      iterator t = find(pair.first);
      if (t != iterator(root))
         erase(t);

      node* it = root->next;
      while (less(it->value.first, pair.first) && it != root)
         it = it->next;
      node* newnode = new node(it, it->prev, pair);
      it->prev->next = newnode;
      it->prev = newnode;
      size++;
      return iterator(newnode);
   }
}

//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
   DEBUGF ('l', that);
   node* it = root->next;
   while(it->value.first != that && it != root)
      it = it->next;
   return iterator(it);
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find_value (iterator position,
const key_type& that) {
   DEBUGF ('l', that);
   node* it = root->next;
   while(iterator(it) != position && it != root)
      it = it->next;
   if (it == root)
      return iterator(it);
   while(it->value.second != that && it != root)
      it = it->next;
   return iterator(it);
}

//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
   DEBUGF ('l', &*position);
   if (size == 0)
      return iterator(root);

   node* it = root->next;
   while (iterator(it) != position && it != root)
      it = it->next;

   if (it == root)
      return iterator(it);

   node* ret = it->next;
   it->prev->next = it->next;
   it->next->prev = it->prev;
   it->next = nullptr;
   it->prev = nullptr;
   delete it;
   size--;
   return iterator(ret);
}

template <typename Key, typename Value, class Less>
void listmap<Key,Value,Less>::display (iterator pos) {
   node* it = root->next;
   while (iterator(it) != pos && it != root)
      it = it->next;
   if (it == root)
      return;
   cout << it->value.first << " = " << it->value.second << endl;
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   DEBUGF ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->() {
   DEBUGF ('l', where);
   return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   DEBUGF ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   DEBUGF ('l', where);
   where = where->prev;
   return *this;
}


//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that) const {
   return this->where != that.where;
}
