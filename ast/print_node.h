#ifndef __M19_AST_PRINT_H__
#define __M19_AST_PRINT_H__

#include "targets/basic_ast_visitor.h"
#include <cdk/ast/expression_node.h>

namespace m19 {

  /**
   * Class for describing a print node.
   */
  class print_node: public cdk::basic_node {
      cdk::expression_node *_argument;
      bool _newline = false;

    public:
        print_node(int lineno, cdk::expression_node *argument, bool newline=false) :
            cdk::basic_node(lineno), _argument(argument), _newline(newline){
        }

    public:
        cdk::expression_node *argument(){
            return _argument;
        }
        
        bool newline(){
            return _newline;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_print_node(this, level);
        }
    };

} //m19

#endif