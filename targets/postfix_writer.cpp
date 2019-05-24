#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "targets/frame_size_calculator.h"
#include "ast/all.h"  // all.h is automatically generated

//---------------------------------------------------------------------------

void m19::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void m19::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
void m19::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
	if (_inFunctionBody){
  	_pf.DOUBLE(node->value()); // push a double
	} else {
		_pf.SDOUBLE(node->value());
	}
}
void m19::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  _pf.INT(0);
  _pf.EQ();
}
void m19::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  _pf.JZ(mklbl(++_lbl));
  node->right()->accept(this, lvl);
  _pf.AND();
  _pf.LABEL(mklbl(_lbl));
}
void m19::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  _pf.JNZ(mklbl(++_lbl));
  node->right()->accept(this, lvl);
  _pf.OR();
  _pf.LABEL(mklbl(_lbl));
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
	if(_inFunctionBody){
  _pf.INT(node->value()); // push an integer
	} else if (node->type()->name() == basic_type::TYPE_DOUBLE){//Jojo: nasty hack!
		_pf.SDOUBLE(node->value());
	} else {
		_pf.SINT(node->value());
	}
}

void m19::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;

  /* generate the string */
  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value()); // output string characters

  /* leave the address on the stack */
  if (_function) {
  	_pf.TEXT(); // return to the TEXT segment
  	_pf.ADDR(mklbl(lbl1)); // the string to be printed
	} else {
		_pf.DATA();
		_pf.SADDR(mklbl(lbl1));
	}
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG(); // 2-complement
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT){
    _pf.I2D();
  }
	else if(node->type()->name() == basic_type::TYPE_POINTER && node->left()->type()->name() == basic_type::TYPE_INT){
    _pf.INT(node->type()->subtype()->size());
		_pf.MUL();
  }
  node->right()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();

  } else if(node->type()->name() == basic_type::TYPE_POINTER && node->right()->type()->name() == basic_type::TYPE_INT){
    _pf.INT(node->type()->subtype()->size());
		_pf.MUL();
  }
  if(node->type()->name() == basic_type::TYPE_INT || node->type()->name() == basic_type::TYPE_POINTER){
    _pf.ADD();
  }
  else{
    _pf.DADD();
  }
}

void m19::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  } else if(node->type()->name() == basic_type::TYPE_POINTER && node->right()->type()->name() == basic_type::TYPE_INT){
    _pf.INT(node->type()->subtype()->size());
		_pf.MUL();
  }
  node->right()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  } else if(node->type()->name() == basic_type::TYPE_POINTER && node->right()->type()->name() == basic_type::TYPE_INT){
    _pf.INT(node->type()->subtype()->size());
		_pf.MUL();
  }
  if(node->type()->name() == basic_type::TYPE_INT || node->type()->name() == basic_type::TYPE_POINTER){
    _pf.SUB();
  }
  else{
    _pf.DSUB();
  }
}

void m19::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  node->right()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  if(node->type()->name() == basic_type::TYPE_INT){
    _pf.MUL();
  }
  else{
    _pf.DMUL();
  }
}

void m19::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
if(node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  node->right()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  if(node->type()->name() == basic_type::TYPE_INT){
	  _pf.DIV();
  }
  else{
    _pf.DDIV();
  }
}

void m19::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}
void m19::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  node->right()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  if(node->type()->name() == basic_type::TYPE_INT){
  _pf.LT();
  }
  else{
  _pf.DCMP();
  }
}

void m19::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  node->right()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  if(node->type()->name() == basic_type::TYPE_INT){
  _pf.LE();
  }
  else{
    _pf.DCMP();
  }
}

void m19::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  node->right()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  if(node->type()->name() == basic_type::TYPE_INT){
  _pf.GE();
  }
  else{
  _pf.DCMP();
  }
}
void m19::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  node->right()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  if(node->type()->name() == basic_type::TYPE_INT){
  _pf.GT();
  }
  else{
    _pf.DCMP();
  }
}

void m19::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  node->right()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  if(node->type()->name() == basic_type::TYPE_INT){
  _pf.NE();
  }
  else{
    _pf.DCMP();
  }
}

void m19::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  node->right()->accept(this, lvl);
  if(node->type()->name() == basic_type::TYPE_DOUBLE && node->right()->type()->name() == basic_type::TYPE_INT){
   _pf.I2D();
  }
  if(node->type()->name() == basic_type::TYPE_INT){
  _pf.EQ();
  }
  else{
    _pf.DCMP();
  }
}
//---------------------------------------------------------------------------

void m19::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::shared_ptr<m19::symbol> symbol = _symtab.find(node->name());
	if (symbol->global())
  	_pf.ADDR(symbol->name());
	else {
		_pf.LOCAL(symbol->offset());
	}
}

void m19::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
	if (node->type()->name() == basic_type::TYPE_DOUBLE){
		_pf.LDDOUBLE();
	}
	else { //TODO if int, pointer, string
  	_pf.LDINT(); 	
	}
}

void m19::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl); // determine the new value
	if(node->type()->name()==basic_type::TYPE_INT || node->type()->name()==basic_type::TYPE_POINTER || node->type()->name()==basic_type::TYPE_STRING){
		_pf.DUP32();
	}
	else if(node->type()->name()==basic_type::TYPE_DOUBLE){
		if(node->rvalue()->type()->name()==basic_type::TYPE_INT){
			_pf.I2D();
		}
		_pf.DUP64();
	}
  //if (new_symbol() == nullptr) {
    node->lvalue()->accept(this, lvl); // where to store the value
  //} else {
   // _pf.DATA(); 
   // _pf.ALIGN(); 
   // _pf.LABEL(new_symbol()->name()); 
   // reset_new_symbol();
   // _pf.SINT(0); 
   // _pf.TEXT(); 
   // node->lvalue()->accept(this, lvl);  
  //}
	if(node->type()->name()==basic_type::TYPE_INT || node->type()->name()==basic_type::TYPE_POINTER || node->type()->name()==basic_type::TYPE_STRING){
  	_pf.STINT(); // store the value at address
	}
	else if(node->type()->name()==basic_type::TYPE_DOUBLE){
  	_pf.STDOUBLE(); // store the value at address
	}
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_evaluation_node(m19::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.TRASH(node->argument()->type()->size()); // delete the evaluated value
}

void m19::postfix_writer::do_print_node(m19::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value to print
  if (node->argument()->type()->name() == basic_type::TYPE_INT) {
    addFunctionToDeclare("printi");
      _pf.CALL("printi");
    _pf.TRASH(4); // delete the printed value
  } else if (node->argument()->type()->name() == basic_type::TYPE_STRING) {
	  addFunctionToDeclare("prints");
      _pf.CALL("prints");
    _pf.TRASH(4); // delete the printed value's address
  }else if(node->argument()->type()->name() == basic_type::TYPE_DOUBLE){
      addFunctionToDeclare("printd");
      _pf.CALL("printd");
      _pf.TRASH(8);
  } else {
	  std::cout << node->argument()->type()->name();
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
  if(node->newline()){
      addFunctionToDeclare("println");
  	_pf.CALL("println"); // print a newline
  }
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_read_node(m19::read_node * const node, int lvl) { //TODO :(
  ASSERT_SAFE_EXPRESSIONS;
    addFunctionToDeclare("readi");
        _pf.CALL("readi");
    _pf.LDFVAL32();

}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_for_node(m19::for_node * const node, int lvl) {
	//init before ASSERT_SAFE in case there are declarations
	_symtab.push();
  os() << "; for init" <<std::endl;
  node->init()->accept(this, lvl);
  ASSERT_SAFE_EXPRESSIONS;
  int lbl;
  _forStep.push(++_lbl);
  _forEnd.push(++_lbl);

  _pf.LABEL(mklbl(lbl = ++_lbl));
  os() << "; for condition test" <<std::endl;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(_forEnd.top()));
  os() << "; for block" <<std::endl;
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(_forStep.top()));
  os() << "; for step" <<std::endl;
  node->increment()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl));
  _pf.LABEL(mklbl(_forEnd.top()));
  _forStep.pop();
  _forEnd.pop();

	_symtab.pop();
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_if_node(m19::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void m19::postfix_writer::do_if_else_node(m19::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

//---------------------------------------------------------------------------
void m19::postfix_writer::do_break_node(m19::break_node * const node, int lvl) {
	if(_forEnd.size() != 0){
		_pf.JMP(mklbl(_forEnd.top()));
	} else {
		error(node->lineno(), "'break' instruction outside for");
	}
}

void m19::postfix_writer::do_continue_node(m19::continue_node * const node, int lvl) {
	if(_forStep.size() != 0){
		_pf.JMP(mklbl(_forStep.top()));
	} else {
		error(node->lineno(), "'continue' instruction outside for");
	}
}

void m19::postfix_writer::do_variable_declaration_node(m19::variable_declaration_node * const node, int lvl) {
	ASSERT_SAFE_EXPRESSIONS;
  std::shared_ptr<m19::symbol> symbol = new_symbol();
	int offset;
	if(_inFunctionArgs){
		offset=_offset;
		_offset+=node->varType()->size();
	}
	else if(_inFunctionBody){
		_offset-=node->varType()->size();
		offset=_offset;
	}else{
		offset=0;
  }
  if(symbol){
		symbol->set_offset(offset);
		reset_new_symbol();
  }
	if(node->qualifier() != 0){
		if(_inFunctionArgs)
			error(node->lineno(), "variable declared with a global symbol as a function argument");
		if(_inFunctionBody)
			error(node->lineno(), "variable declared with a global symbol in function body ");
	}
	//function arguments have been fully declared by this point
	
	
	if(_inFunctionBody){// in a function body
		if(node->initializer()){
			node->initializer()->accept(this, lvl);

			if(node->initializer()->type()->name()==basic_type::TYPE_INT && node->varType()->name() == basic_type::TYPE_DOUBLE){
				_pf.I2D();
				_pf.LOCAL(symbol->offset());
				_pf.STDOUBLE();
			}
			else if(node->initializer()->type()->name()==basic_type::TYPE_INT || node->initializer()->type()->name()==basic_type::TYPE_POINTER || node->initializer()->type()->name()==basic_type::TYPE_STRING){
				_pf.LOCAL(symbol->offset());
				_pf.STINT();
			}
			else if(node->initializer()->type()->name()==basic_type::TYPE_DOUBLE){
				_pf.LOCAL(symbol->offset());
				_pf.STDOUBLE();
			}
		}
	} 
  else if (!_function){ //outside a function
		if(node->initializer()){
			_pf.DATA();
			_pf.ALIGN();
			if(node->qualifier() == 1){
	 		 _pf.GLOBAL(node->identifier(), _pf.OBJ());
 			}
			if(node->qualifier() == 2){
				error(node->lineno(), "Can't initialize external variable");
			} 
			_pf.LABEL(node->identifier());
      //TODO missing a lot of checks, namely strings and integer to double
      if(node->varType()->name() == basic_type::TYPE_INT && node->initializer()->type()->name() != basic_type::TYPE_INT){
        throw "Value of initializer: not an integer";
      }
      if(node->varType()->name() == basic_type::TYPE_DOUBLE){
        if(node->initializer()->type()->name() == basic_type::TYPE_INT){
          node->initializer()->type(new basic_type(8, basic_type::TYPE_DOUBLE));
        }
        else{
          throw "Value of initializer: not a double or integer";
        }
      }
			node->initializer()->accept(this,lvl); 
      if(node->varType()->name() == basic_type::TYPE_POINTER && node->initializer()->type()->name() != basic_type::TYPE_POINTER){
        throw "Value of initializer: not a pointer";
      }
		} 
    else{
			_pf.BSS();
			_pf.ALIGN();
			if(node->qualifier() == 1){
	 		 _pf.GLOBAL(node->identifier(), _pf.OBJ());
 			}
			_pf.LABEL(node->identifier());
			_pf.SALLOC(node->varType()->size());
		}
	}	
}


void m19::postfix_writer::do_function_declaration_node(m19::function_declaration_node * const node, int lvl) {
	ASSERT_SAFE_EXPRESSIONS;
	if (new_symbol()){
        addFunctionToDeclare(node->identifier());
		reset_new_symbol();
	}
}

void m19::postfix_writer::do_function_definition_node(m19::function_definition_node * const node, int lvl) {
	ASSERT_SAFE_EXPRESSIONS;
	_function = new_symbol();
	set_function_name(_function->name());
	_functionsToDeclare.erase(node->identifier());
	reset_new_symbol();

	frame_size_calculator fsc(_compiler, _symtab);
	node->accept(&fsc,lvl);
	int frame_size = fsc.localsize() + node->returnType()->size();

	_offset = 8;
	_symtab.push(); //arguments scope being pushed
	_inFunctionArgs=true;
	for (size_t i = 0; i< node->arguments()->size(); i++){
		cdk::basic_node *arg = node->arguments()->node(i);
		if (arg == nullptr)
			break; //empty argument list,
		arg->accept(this, 0);
	}
	_inFunctionArgs=false;

	_pf.TEXT();
	_pf.ALIGN();
	if (_function->qualifier() == 1){ //if it is global
		_pf.GLOBAL(_function->name(), _pf.FUNC());
  }
	_pf.LABEL(_function->name());
	_pf.ENTER(frame_size);
	
	_inFunctionBody=true;
	_offset = 0;
	if (node->returnType()->name() != basic_type::TYPE_VOID){
		m19::variable_declaration_node returnValueNode(node->lineno(), 0, node->returnType(), "@", node->returnValue()); 
		returnValueNode.accept(this, lvl);
	}
	_section=0;

	_nextSection=++_lbl;
	_finalSection=++_lbl;

	node->initial_section()->accept(this, lvl); 
	node->sections()->accept(this, lvl); 
	node->final_section()->accept(this, lvl); 

	os() << ";; end of body " << std::endl;
	_pf.LABEL(mklbl(_finalSection));
	_inFunctionBody=false;
	_symtab.pop(); //arguments scope being popped

  if( node->returnType()->name() == basic_type::TYPE_INT || node->returnType()->name() == basic_type::TYPE_POINTER || node->returnType()->name() == basic_type::TYPE_STRING){
		_pf.LOCAL(-4);
		_pf.LDINT();
    _pf.STFVAL32();
  }
  else if(node->returnType()->name() == basic_type::TYPE_DOUBLE){
		_pf.LOCAL(-8);
		_pf.LDDOUBLE();
    _pf.STFVAL64();
  }

	_pf.LEAVE();
    	_pf.RET();
}

void m19::postfix_writer::do_function_call_node(m19::function_call_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
 
	std::string id;

	if (node->identifier() == "m19"){
		id = "_main";
	} else if (node->identifier() == "_main"){
		id = "._main";
	} else if (node->identifier() == "@"){
		id = function_name();
	} else {
		id = node->identifier();
	}

  std::shared_ptr<m19::symbol> symbol = _symtab.find(id);

	size_t argsSize = 0;
  if(node->arguments()){
    for(int nArg = node->arguments()->size(); nArg > 0; nArg--){
      cdk::expression_node *arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(nArg-1));
      arg->accept(this, lvl + 2);
			if (symbol->argTypes()[nArg-1]->name() == basic_type::TYPE_DOUBLE && arg->type()->name() == basic_type::TYPE_INT){
				_pf.I2D();
				argsSize += 8;
			} else {
      	argsSize += arg->type()->size();
			}
    }
  }
  _pf.CALL(id);
  if(argsSize!=0){
    _pf.TRASH(argsSize);
  }
  if( symbol->type()->name() == basic_type::TYPE_INT || symbol->type()->name() == basic_type::TYPE_POINTER || symbol->type()->name() == basic_type::TYPE_STRING){
    _pf.LDFVAL32();
  }
  else if(symbol->type()->name() == basic_type::TYPE_DOUBLE){
    _pf.LDFVAL64();
  }
  else{}
}

void m19::postfix_writer::do_address_of_node(m19::address_of_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl+2);
}

void m19::postfix_writer::do_block_node(m19::block_node * const node, int lvl) {
	if (_section)
		_symtab.push();
	node->declarations()->accept(this, lvl);
	node->instructions()->accept(this, lvl);
	if (_section)
		_symtab.pop();
}

void m19::postfix_writer::do_section_node(m19::section_node * const node, int lvl) {
	os() << ";; middle section" << std::endl;
	node->condition()->accept(this, lvl);
	_pf.JZ(mklbl(++_lbl));
	_nextSection=_lbl;
	node->block()->accept(this, lvl);
	if (!node->inclusive())
		_pf.JMP(mklbl(_finalSection));
	_pf.LABEL(mklbl(_nextSection));
}

void m19::postfix_writer::do_initial_section_node(m19::initial_section_node * const node, int lvl) {
	node->block()->accept(this, lvl);
	_section=1;
}

void m19::postfix_writer::do_final_section_node(m19::final_section_node * const node, int lvl) {
	os() << ";; final section" << std::endl;
	_pf.LABEL(mklbl(_finalSection));
	_finalSection=++_lbl;
	node->block()->accept(this, lvl);
}

void m19::postfix_writer::do_index_node(m19::index_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
	node->base()->accept(this,lvl);
	node->index()->accept(this,lvl);
	_pf.INT(node->type()->size());
	_pf.MUL();
	_pf.ADD();
}

void m19::postfix_writer::do_return_node(m19::return_node * const node, int lvl) {
	_pf.JMP(mklbl(_finalSection));
}

void m19::postfix_writer::do_identity_node(m19::identity_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
}

void m19::postfix_writer::do_alloc_node(m19::alloc_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this,lvl);
  _pf.INT(8);
  _pf.MUL();
  _pf.ALLOC();
  _pf.SP();
}
