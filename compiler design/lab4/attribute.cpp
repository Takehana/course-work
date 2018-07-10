#include <iostream>
#include <bitset>
#include <cstring>
#include <unordered_map>
#include <vector>

#include "astree.h"
#include "lyutils.h"
#include "string_set.h"
#include "attribute.h"

vector<int> block_count;
int next_block = 1;

vector<symbol_table*> symbol_stack;
symbol_table* type = new symbol_table;
astree* struct_name = nullptr;

symbol* newsymbol(astree* node)
{
    symbol* sym = new symbol();
    sym->attributes = node->attributes;
    sym->fields = nullptr;
    sym->filenr = node->lloc.filenr;
    sym->linenr = node->lloc.linenr;
    sym->offset = node->lloc.offset;
    sym->blocknr = node->blocknr;
    sym->parameters = nullptr;
    return sym;
}

/*void table_insert(symbol_table* table, astree* node)
{
    symbol* sym = newsymbol(node);
    const string* key = node->lexinfo;
    if ((table != NULL) && (node != nullptr))
    {
        table->insert(symbol_entry(key, sym));
    }
}*/

void increase_block(astree* node)
{
    for( size_t index = 0; index < node->children.size(); ++index)
    {
        node->children[index]->blocknr++;
        increase_block(node->children[index]);
    }
}

void set_attr(astree* node)
{
    for( size_t index = 0; index < node->children.size(); ++index)
    {
        set_attr(node->children[index]);
    }
    
    switch(node->symbol)
    {
        case TOK_VOID:
            if (node->children.size() > 0)
                node->children[0]->
                attributes[ATTR_void] = true;
            break;
        case TOK_INT:
            if (node->children.size() > 0)
                node->children[0]->
                attributes[ATTR_int] = true;
            break;
        case TOK_STRING:
            if (node->children.size() > 0)
                node->children[0]->
                attributes[ATTR_string] = true;
            break;
        case TOK_STRUCT:
            node->children[0]->attributes[ATTR_struct] = true;
            node->children[0]->attributes[ATTR_variable] = false;
            node->children[0]->attributes[ATTR_lval] = false;
            for (size_t index = 1; index < node->children.size();
                 ++index)
            {
                increase_block(node->children[index]);
            }
            break;
            
        case TOK_FUNCTION:
            node->children[0]->children[0]->
                attributes[ATTR_function] = true;
            node->children[0]->children[0]->
                attributes[ATTR_variable] = false;
            node->children[0]->children[0]->
                attributes[ATTR_lval] = false;
            for (size_t index = 1; index < node->children.size();
                 ++index)
            {
                increase_block(node->children[index]);
            }
        case TOK_PARAMLIST:
            for (size_t index = 0;
                 index < node->children[1]->children.size(); ++index)
                node->children[1]->children[index]->children[0]->
                    attributes[ATTR_param] = true;
            break;
        case TOK_FIELD:
            if (strcmp(node->lexinfo->c_str(), ".")!= 0)
            {
                node->attributes[ATTR_field] = true;
            }
            break;
        case TOK_TYPEID:
            node->attributes[ATTR_typeid] = true;
            for(size_t index = 0;
                index < node->children.size(); ++index)
            {
                node->children[index]->
                attributes[ATTR_typeid] = true;
                node->children[index]->
                attributes[ATTR_struct] = true;
                break;
            }
        case TOK_IDENT:
            if (node->children.size() > 0)
                node->children[0]->
                    attributes[ATTR_struct] = true;
            break;
        case TOK_VARDECL:
            node->children[0]->children[0]
                ->attributes[ATTR_variable] = true;
            break;
        case TOK_DECLID:
            node->attributes[ATTR_lval] = true;
            node->attributes[ATTR_variable] = true;
            break;
        case TOK_NEW:
            node->children[0]->attributes.reset();
            break;
        case TOK_IFELSE:
            for (size_t index = 1; index < node->children.size();
                 ++index)
            {
                increase_block(node->children[index]);
            }
            break;
        default:
            break;

    }
    
}

const char* check_attr(astree* node){
    string out = "";
    attr_bitset attr = node->attributes;
    //if (attr[ATTR_field]) { out += "field ";}
    //if (attr[ATTR_struct]) { out += "struct ";}
    if (attr[ATTR_void]) { out += "void ";}
    if (attr[ATTR_int]) { out += "int ";}
    if (attr[ATTR_null]) { out += "null ";}
    if (attr[ATTR_string]) { out += "string ";}
    if (attr[ATTR_array]) { out += "array ";}
    if (attr[ATTR_function]) { out += "function ";}
    if (attr[ATTR_variable]) { out += "variable ";}
    //if (attr[ATTR_typeid]) { out += "typeid ";}
    if (attr[ATTR_lval]) { out += "lval ";}
    if (attr[ATTR_param]) { out += "param ";}
    if (attr[ATTR_const]) { out += "const ";}
    if (attr[ATTR_vreg]) { out += "vreg ";}
    if (attr[ATTR_vaddr]) { out += "vaddr ";}
    return out.c_str();
}

void print_attr(FILE* out, astree* node)
{
    const char* key = node->lexinfo->c_str();
    if (node->attributes.none())
    {
        for( size_t index = 0; index < node->children.size(); ++index)
        {
            print_attr(out, node->children[index]);
        }
        return;
    }
    for (int i = 0; i < node->blocknr; ++i)
    {
        fprintf(out, "    ");
    }
    if (node->attributes[ATTR_field])
    {
        fprintf(out, "%s (%zu.%zu.%zu) field {%s} ",
                key,
                 node->lloc.linenr, node->lloc.filenr,
                 node->lloc.offset,
                 struct_name->lexinfo->c_str());
    }
    else
    {
        fprintf(out, "%s (%zu.%zu.%zu) {%d} ",
                key,
                 node->lloc.linenr, node->lloc.filenr,
                 node->lloc.offset, node->blocknr);
    }
    
    if (node->attributes[ATTR_typeid])
    {
        struct_name = node;
    }
    
    if (node->attributes[ATTR_struct])
    {
        fprintf(out, "struct \"%s\" ", struct_name->lexinfo->c_str());
        //struct_name = node;//->children[0];
    }
                
    fprintf(out, "%s\n", check_attr(node));
    
    for( size_t index = 0; index < node->children.size(); ++index)
    {
        print_attr(out, node->children[index]);
    }
}
                
void scan_attr(FILE* out, astree* node)
{
    
    /*for( size_t index = 0; index < node->children.size(); ++index)
    {
        run_attr(out, node->children[index]);
    }*/
    
    set_attr(node);
    print_attr(out, node);
    //traverse(out, node);
}

void traverse(FILE* out, astree* node)
{
    fprintf(out, "%s -> %s\n",
        node->lexinfo->c_str(), check_attr(node));
    for( size_t index = 0; index < node->children.size(); ++index)
    {
        traverse(out, node->children[index]);
    }
}
