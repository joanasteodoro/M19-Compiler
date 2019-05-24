#ifndef __M19_AST_VARIABLE_DECLARATION_H__
#define __M19_AST_VARIABLE_DECLARATION_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/expression_node.h>
#include <string>

namespace m19 {

  /**
   * Class for describing a variable declaration.
   */
  class variable_declaration_node: public cdk::basic_node {
      int _qualifier;
      basic_type *_varType;
      std::string _identifier;
      cdk::expression_node *_initializer;

    public:
        variable_declaration_node(int lineno, int qualifier, basic_type *varType, const std::string &identifier,
                                  cdk::expression_node *initializer) :
            cdk::basic_node(lineno), _qualifier(qualifier), _varType(varType), _identifier(identifier), _initializer(initializer) {
        }

    public:
        bool constant() {
            return _varType == _varType->subtype();
        }

        int qualifier(){
            return _qualifier;
        }
        
        basic_type *varType() {
            return _varType;
        }
        
        const std::string &identifier() const {
            return _identifier;
        }
        cdk::expression_node *initializer(){
            return _initializer;
        }
        
        void accept(basic_ast_visitor *sp, int level) {
            sp->do_variable_declaration_node(this, level);
        }
    };

} //m19

#endif