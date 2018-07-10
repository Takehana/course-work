#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__


#include <bitset>
#include <string>
#include <unordered_map>
#include <vector>

#include "auxlib.h"
#include "astree.h"

extern char* filenamesym;

struct symbol;
struct astree;

enum { ATTR_void, ATTR_int, ATTR_null, ATTR_string,
       ATTR_struct, ATTR_array, ATTR_function, ATTR_variable,
       ATTR_field, ATTR_typeid, ATTR_param, ATTR_lval,
       ATTR_const, ATTR_vreg, ATTR_vaddr, ATTR_bitset_size,
};

using attr_bitset = bitset<ATTR_bitset_size>;

using symbol_table = unordered_map<const string*,symbol*>;
using symbol_entry = pair<const string*, symbol*>;

struct symbol
{
    attr_bitset attributes;
    symbol_table* fields;
    size_t filenr, linenr, offset;
    size_t blocknr;
    vector<symbol*>* parameters;
};

void scan_attr(FILE* out, astree* node);
void increase_block(astree* node);
void set_attr(astree* node);
void print_attr(FILE* out, astree* node);
void traverse(FILE* out, astree* node);
void insert_symbol(FILE* out, symbol_table table, const string* key,
                   symbol* sym, astree* node);
void table_insert(symbol_table* table, astree* node);

#endif
