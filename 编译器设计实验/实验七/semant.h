#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"
#include <map>

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.
// You may use it or not as you like: it is only here to provide
// a container for the supplied methods.

class ClassTable {
private:
    int semant_errors;
    void install_basic_classes();
    void install_classes(Classes &classes);
    void check_inheritance();
    std::ostream& error_stream;
    std::map<Symbol, Class_> symbol_table;
    std::map<Class_, std::vector<method_class*>> method_table;

public:
    ClassTable(Classes);
    void check_main();
    void install_methods();
    int errors() { return semant_errors; }
    std::ostream& semant_error();
    std::ostream& semant_error(Class_ c);
    std::ostream& semant_error(Symbol filename, tree_node *t);
};

#endif