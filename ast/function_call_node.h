#ifndef __M19_AST__FUNCTION_CALL_NODE_H__
#define __M19_AST__FUNCTION_CALL_NODE_H__

#include <cdk/ast/basic_node.h>
#include <string>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include "targets/basic_ast_visitor.h"
#include <cdk/ast/nil_node.h>

namespace m19 {

  /**
   * Class for describing a function call node.
   */
  class function_call_node: public cdk::expression_node {
      std::string _identifier;
      cdk::sequence_node *_arguments;

    public:
        // constructor for a function with no arguments
        function_call_node(int lineno, const std::string &identifier) :
            cdk::expression_node(lineno), _identifier(identifier), _arguments(new cdk::sequence_node(lineno)) {
        }

        // constructor for a function with arguments
        function_call_node(int lineno, const std::string &identifier, cdk::sequence_node *arguments) :
            cdk::expression_node(lineno), _identifier(identifier), _arguments(arguments){
        }

    public: 
        const std::string &identifier() const {
            return _identifier;
        }

        cdk::sequence_node *arguments(){
            return _arguments;
        }
        
        void accept(basic_ast_visitor *sp, int level) {
            sp->do_function_call_node(this, level);
        }
    };

} //m19

#endif
