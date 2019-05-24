#ifndef __M19_AST__FUNCTION_DECLARATION_NODE_H__
#define __M19_AST__FUNCTION_DECLARATION_NODE_H__

#include <cdk/ast/basic_node.h>
#include <string>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>

namespace m19 {

  /**
   * Class for describing a function declaration.
   */
  class function_declaration_node: public cdk::basic_node {
      int _qualifier;
      basic_type *_returnType;
      cdk::expression_node *_returnValue;
      std::string _identifier;
      cdk::sequence_node *_arguments;

    public:
        function_declaration_node(int lineno, int qualifier, const std::string &identifier,
                                  cdk::sequence_node *arguments) :
            cdk::basic_node(lineno), _qualifier(qualifier), _returnType(new basic_type(0, basic_type::TYPE_VOID)), _returnValue(nullptr), _identifier(identifier), _arguments(arguments) {
        }

        function_declaration_node(int lineno, int qualifier, basic_type *returnType, cdk::expression_node *returnValue, const std::string &identifier,
                                  cdk::sequence_node *arguments) :
            cdk::basic_node(lineno), _qualifier(qualifier), _returnType(returnType), _returnValue(returnValue), _identifier(identifier), _arguments(arguments){
        }

    public:
        int qualifier(){
            return _qualifier;
        }
        
        basic_type *returnType() {
            return _returnType;
        }
        
	cdk::expression_node *returnValue() {
            return _returnValue;
        }
        
        const std::string &identifier() const {
            return _identifier;
        }
        cdk::sequence_node *arguments(){
            return _arguments;
        }
        
        void accept(basic_ast_visitor *sp, int level) {
            sp->do_function_declaration_node(this, level);
        }
    };

} //m19

#endif
