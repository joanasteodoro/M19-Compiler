#ifndef __M19_AST_ADDRESS_OF_H__
#define __M19_AST_ADDRESS_OF_H__

#include "targets/basic_ast_visitor.h"
#include <cdk/ast/unary_expression_node.h>

namespace m19 {

  /**
   * Class for describing an address.
   */
  class address_of_node: public cdk::expression_node {
      cdk::lvalue_node *_lvalue;
    public:
        address_of_node(int lineno, cdk::lvalue_node *argument) :
            cdk::expression_node(lineno), _lvalue(argument){
        }

    public:
        cdk::lvalue_node *lvalue(){
            return _lvalue;
        }
        
        void accept(basic_ast_visitor *sp, int level) {
            sp->do_address_of_node(this, level);
        }
    };

} //m19

#endif
