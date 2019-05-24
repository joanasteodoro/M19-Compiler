#ifndef __M19_AST__FUNCTION_DEFINITION_NODE_H__
#define __M19_AST__FUNCTION_DEFINITION_NODE_H__

#include <cdk/ast/basic_node.h>
#include <string>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include "ast/section_node.h"
#include "ast/function_declaration_node.h"
#include "ast/initial_section_node.h"
#include "ast/final_section_node.h"

namespace m19 {

  /**
   * Class for describing a function definition.
   */
  class function_definition_node: public cdk::basic_node {
      int _qualifier;
      basic_type *_returnType;
      cdk::expression_node *_returnValue;
      std::string _identifier;
      cdk::sequence_node *_arguments;
      m19::initial_section_node *_initialSection;
      cdk::sequence_node *_sections; 
      m19::final_section_node *_finalSection;

    public:
      function_definition_node(int lineno, int qualifier, const std::string &identifier,
	                            cdk::sequence_node *arguments, cdk::sequence_node *sections,
				  				  					m19::initial_section_node *initial_section, m19::final_section_node *final_section) :
           cdk::basic_node(lineno), _qualifier(qualifier), _returnType(new basic_type(0, basic_type::TYPE_VOID)), _returnValue(nullptr),
					 _identifier(identifier), _arguments(arguments), _sections(sections){
				if (initial_section == nullptr){
					_initialSection = new initial_section_node(lineno);
				} else {
					_initialSection = initial_section;
				}

				if (final_section == nullptr){
					_finalSection = new final_section_node(lineno);
				} else {
					_finalSection = final_section;
				}
		  }

      function_definition_node(int lineno, int qualifier, basic_type *returnType, const std::string &identifier,
                                 cdk::sequence_node *arguments, cdk::expression_node *returnValue, cdk::sequence_node *sections,
				  				 m19::initial_section_node *initial_section, m19::final_section_node *final_section) :
   				  cdk::basic_node(lineno), _qualifier(qualifier), _returnType(returnType), _returnValue(returnValue), _identifier(identifier), _arguments(arguments), _sections(sections){
			  if (initial_section == nullptr){
					_initialSection = new initial_section_node(lineno);
				} else {
					_initialSection = initial_section;
				}
		
				if (final_section == nullptr){
					_finalSection = new final_section_node(lineno);
				} else {
					_finalSection = final_section;
				}
      }

			function_definition_node(int lineno, m19::function_declaration_node *header, cdk::sequence_node *sections,
				  											m19::initial_section_node *initial_section, m19::final_section_node *final_section) :
	    			cdk::basic_node(lineno), _qualifier(header->qualifier()), _returnType(header->returnType()), _returnValue(header->returnValue()),
						 _identifier(header->identifier()), _arguments(header->arguments()), _sections(sections) {
				if (initial_section == nullptr){
					_initialSection = new initial_section_node(lineno);
				} else {
					_initialSection = initial_section;
				}
		
				if (final_section == nullptr){
					_finalSection = new final_section_node(lineno);
				} else {
					_finalSection = final_section;
				}
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

				m19::initial_section_node *initial_section(){
						return _initialSection;
				}

        cdk::sequence_node *sections(){
            return _sections;
        }

				m19::final_section_node *final_section(){
						return _finalSection;
				}
        
        void accept(basic_ast_visitor *sp, int level) {
            sp->do_function_definition_node(this, level);
        }
    };

} //m19

#endif
