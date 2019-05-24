#ifndef __M19_AST_SECTION_NODE_H__
#define __M19_AST_SECTION_NODE_H__

#include <cdk/ast/basic_node.h>
#include "targets/basic_ast_visitor.h"

namespace m19 {

  /**
   * Class for describing a section node.
   */
  class section_node: public cdk::basic_node {
		cdk::expression_node *_condition;
		m19::block_node *_block;
		bool _inclusive;

    public:
        section_node(int lineno, cdk::expression_node *condition, m19::block_node *block, bool inclusive) :
            cdk::basic_node(lineno), _condition(condition), _block(block), _inclusive(inclusive) {
        }

    public:
        m19::block_node *block() {
            return _block;
        }

		bool inclusive() {
			return _inclusive;
		}
	
        cdk::expression_node *condition() {
            return _condition;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_section_node(this, level);
        }
    };

} //m19

#endif
