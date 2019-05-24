#ifndef __M19_SEMANTICS_POSTFIX_WRITER_H__
#define __M19_SEMANTICS_POSTFIX_WRITER_H__

#include <string>
#include <iostream>
#include <stack>
#include <set>
#include <cdk/symbol_table.h>
#include <cdk/emitters/basic_postfix_emitter.h>
#include "targets/basic_ast_visitor.h"
#include "targets/symbol.h"

namespace m19 {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<m19::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl, _offset; //offset is current offset for variable declaration,
    int _nextSection, _finalSection; //used for jumps, these are lbls
    int _section; // section is current section number (not counting initial section and final section)
		std::stack<int> _forEnd;
		std::stack<int> _forStep;
		std::shared_ptr<m19::symbol> _function;
		bool _inFunctionArgs, _inFunctionBody;
		std::set<std::string> _functionsToDeclare;

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<m19::symbol> &symtab, cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0), _offset(0), _nextSection(0), _finalSection(0), _section(0), _function(nullptr), _inFunctionArgs(false), _inFunctionBody(false) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

    void error(int lineno, std::string s) {
      std::cerr << "error: " << lineno << ": " << s << std::endl;
    }

  public:
    std::set<std::string> functionsToDeclare(){
	    return _functionsToDeclare;
    }

    void addFunctionToDeclare(std::string id){
	    _functionsToDeclare.insert(id);
    }
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // m19

#endif
