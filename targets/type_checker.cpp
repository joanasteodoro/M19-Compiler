#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

#define ASSERT_UNSPEC \
    { if (node->type() != nullptr && \
          node->type()->name() != basic_type::TYPE_UNSPEC) return; }

//---------------------------------------------------------------------------

void m19::type_checker::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}

//---------------------------------------------------------------------------

void m19::type_checker::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void m19::type_checker::do_data_node(cdk::data_node * const node, int lvl) {
}
void m19::type_checker::do_double_node(cdk::double_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
}
void m19::type_checker::do_not_node(cdk::not_node * const node, int lvl) {
   node->argument()->accept(this, lvl);
        if(node->argument()->type()->name() != basic_type::TYPE_INT){
          throw "invalid operand: not an integer";
        }

  node->type(new basic_type(4, basic_type::TYPE_INT)); 
}

void m19::type_checker::do_and_node(cdk::and_node * const node, int lvl) {
  node->left()->accept(this, lvl);
        if(node->left()->type()->name() != basic_type::TYPE_INT){
          throw "invalid left operand: not an integer";
        }
  node->right()->accept(this, lvl);
    if(node->right()->type()->name() != basic_type::TYPE_INT){
      throw "invalid right operand: not an integer";
    }
    node->type(new basic_type(4, basic_type::TYPE_INT));
}

void m19::type_checker::do_or_node(cdk::or_node * const node, int lvl) {
  node->left()->accept(this, lvl);
        if(node->left()->type()->name() != basic_type::TYPE_INT){
          throw "invalid left operand: not an integer";
        }
  node->right()->accept(this, lvl);
  if(node->right()->type()->name() != basic_type::TYPE_INT){
    throw "invalid right operand: not an integer";
  }
  node->type(new basic_type(4, basic_type::TYPE_INT));

}

//---------------------------------------------------------------------------

void m19::type_checker::do_integer_node(cdk::integer_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void m19::type_checker::do_string_node(cdk::string_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_STRING));
}

//---------------------------------------------------------------------------

void m19::type_checker::processUnaryExpression(cdk::unary_expression_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  node->type(node->argument()->type());

  if (node->argument()->type()->name() == basic_type::TYPE_INT )
    node->type(new basic_type(4, basic_type::TYPE_INT));
  else if (node->argument()->type()->name() == basic_type::TYPE_DOUBLE)
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
}

void m19::type_checker::do_neg_node(cdk::neg_node * const node, int lvl) {
  processUnaryExpression(node, lvl);
  if (node->argument()->type()->name() != basic_type::TYPE_INT && node->argument()->type()->name() != basic_type::TYPE_DOUBLE) throw std::string("wrong type in argument of negation expression");
}

void m19::type_checker::do_identity_node(m19::identity_node * const node, int lvl) {
  processUnaryExpression(node, lvl);
  if (node->argument()->type()->name() != basic_type::TYPE_INT && node->argument()->type()->name() != basic_type::TYPE_DOUBLE) throw std::string("wrong type in argument of identity expression");
}

void m19::type_checker::do_address_of_node(m19::address_of_node * const node, int lvl) {
  node->lvalue()->accept(this, lvl);
  node->type(new basic_type(4, basic_type::TYPE_POINTER));
  node->type()->_subtype=node->lvalue()->type();
}

//---------------------------------------------------------------------------

void m19::type_checker::processBinaryExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() != node->left()->type()->name()){
    if((node->right()->type()->name() == basic_type::TYPE_INT && node->left()->type()->name() == basic_type::TYPE_DOUBLE) || (node->right()->type()->name() == basic_type::TYPE_DOUBLE && node->left()->type()->name() == basic_type::TYPE_INT)){
      node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
    }	else if((node->right()->type()->name() == basic_type::TYPE_INT && node->left()->type()->name() == basic_type::TYPE_POINTER) || (node->right()->type()->name() == basic_type::TYPE_POINTER && node->left()->type()->name() == basic_type::TYPE_INT)){
			
      node->type(new basic_type(4, basic_type::TYPE_POINTER));

			basic_type *tempType;
			if(node->left()->type()->name() == basic_type::TYPE_POINTER){
				tempType = node->left()->type();
			} else {
				tempType = node->right()->type();
			}

			basic_type *_tempType = node->type();

			while(tempType->subtype()!=nullptr){
				tempType = tempType->subtype();
				_tempType->_subtype = new basic_type(tempType->size(), tempType->name());
				_tempType=_tempType->subtype();
			}
		} else {
 	   throw std::string("mismatching types in arguments of binary expression");
 		}
  } else {
  	node->type(node->right()->type());
	}
}

void m19::type_checker::do_add_node(cdk::add_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  if(node->type()->name() != basic_type::TYPE_INT && node->type()->name() != basic_type::TYPE_DOUBLE && node->type()->name() != basic_type::TYPE_POINTER){
    throw std::string("invalid operand types in add operand");
  }
}

void m19::type_checker::do_sub_node(cdk::sub_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
	if(node->type()->name() != basic_type::TYPE_INT && node->type()->name() != basic_type::TYPE_DOUBLE && node->type()->name() != basic_type::TYPE_POINTER){
		throw std::string("invalid operand types in sub operand");
  } 
}

void m19::type_checker::do_mul_node(cdk::mul_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  if(node->type()->name() != basic_type::TYPE_INT && node->type()->name() != basic_type::TYPE_DOUBLE ){
    throw std::string("invalid operand types in mul operand");
  }
}

void m19::type_checker::do_div_node(cdk::div_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  if(node->type()->name() != basic_type::TYPE_INT && node->type()->name() != basic_type::TYPE_DOUBLE ){
    throw std::string("invalid operand types in div operand");
  }
}

void m19::type_checker::do_mod_node(cdk::mod_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  if(node->type()->name() != basic_type::TYPE_INT ){
    throw std::string("invalid operand types in mod operand");
  }
}

void m19::type_checker::do_lt_node(cdk::lt_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  if(node->type()->name() != basic_type::TYPE_INT && node->type()->name() != basic_type::TYPE_DOUBLE ){
    throw std::string("invalid operand types in less-than operand");
  }
}

void m19::type_checker::do_le_node(cdk::le_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  if(node->type()->name() != basic_type::TYPE_INT && node->type()->name() != basic_type::TYPE_DOUBLE ){
    throw std::string("invalid operand types in less-or-equals-than operand");
  }
}

void m19::type_checker::do_ge_node(cdk::ge_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  if(node->type()->name() != basic_type::TYPE_INT && node->type()->name() != basic_type::TYPE_DOUBLE ){
    throw std::string("invalid operand types in greater-or-equal-than operand");
  }
}

void m19::type_checker::do_gt_node(cdk::gt_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  if(node->type()->name() != basic_type::TYPE_INT && node->type()->name() != basic_type::TYPE_DOUBLE ){
    throw std::string("invalid operand types in greater-than operand");
  }
}

void m19::type_checker::do_ne_node(cdk::ne_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  if(node->type()->name() != basic_type::TYPE_INT && node->type()->name() != basic_type::TYPE_DOUBLE && node->type()->name() != basic_type::TYPE_POINTER){
    throw std::string("invalid operand types in not-equal operand");
  }
}

void m19::type_checker::do_eq_node(cdk::eq_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  if(node->type()->name() != basic_type::TYPE_INT && node->type()->name() != basic_type::TYPE_DOUBLE && node->type()->name() != basic_type::TYPE_POINTER){
    throw std::string("invalid operand types in equal operand");
  }
}

//---------------------------------------------------------------------------

void m19::type_checker::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<m19::symbol> symbol = _symtab.find(id);

  if (symbol != nullptr) {
    node->type(symbol->type());
  } else {
    throw id;
  }
}

void m19::type_checker::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw "undeclared variable '" + id + "'";
  }
}

void m19::type_checker::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_UNSPEC;

  try {
    node->lvalue()->accept(this, lvl);
  } catch (const std::string &id) {
    throw "undeclared variable '" + id + "'";
  }

  node->rvalue()->accept(this, lvl + 2);


  if (node->lvalue()->type()->name() != node->rvalue()->type()->name() && node->rvalue()->type()->name() != basic_type::TYPE_INT && node->lvalue()->type()->name() != basic_type::TYPE_DOUBLE) throw std::string(
      "mismatching types in arguments of assignment expression");

  node->type(node->lvalue()->type());
}

//---------------------------------------------------------------------------

void m19::type_checker::do_evaluation_node(m19::evaluation_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void m19::type_checker::do_print_node(m19::print_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void m19::type_checker::do_read_node(m19::read_node * const node, int lvl) {
  //EMPTY
}

//---------------------------------------------------------------------------

void m19::type_checker::do_for_node(m19::for_node * const node, int lvl) {
  cdk::expression_node *condition = (cdk::expression_node *) node->condition()->node(node->condition()->size()-1); 
  condition->accept(this, lvl + 4);
  if(condition->type()->name() != basic_type::TYPE_INT){
    throw "Condition operand must be an integer";
  }
}

//---------------------------------------------------------------------------

void m19::type_checker::do_if_node(m19::if_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void m19::type_checker::do_if_else_node(m19::if_else_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------
void m19::type_checker::do_break_node(m19::break_node * const node, int lvl) {
  // EMPTY
}

void m19::type_checker::do_continue_node(m19::continue_node * const node, int lvl) {
  // EMPTY
}

void m19::type_checker::do_variable_declaration_node(m19::variable_declaration_node * const node, int lvl) {
  if (node->initializer() != nullptr){
    node->initializer()->accept(this, lvl);
    if (node->initializer()->type()->name() != node->varType()->name() && node->initializer()->type()->name() != basic_type::TYPE_INT && node->varType()->name() != basic_type::TYPE_DOUBLE)
      throw std::string("variable type doesn't match initializer type");
  }
  std::shared_ptr<m19::symbol> symbol = std::make_shared<m19::symbol>(node->varType(), node->identifier(), false, node->qualifier());
	if (_symtab.insert(node->identifier(), symbol))
		_parent->set_new_symbol(symbol);
	else
		throw std::string("redeclared variable");
}

void m19::type_checker::do_function_declaration_node(m19::function_declaration_node * const node, int lvl) {
	std::string id;
	if (node->identifier() == "m19"){
		id = "_main";
	} else if (node->identifier() == "_main"){
		id = "._main";
	} else if (node->identifier() == "@"){
		id = "this";
	} else {
		id = node->identifier();
	}
  std::shared_ptr<m19::symbol> previous = _symtab.find(node->identifier()); 
	if (node->returnValue())
		node->returnValue()->accept(this,lvl);
  if(previous){
    throw std::string("conflicting declaration for '" + node->identifier() + "'");
  }
  else{
    std::shared_ptr<m19::symbol> symbol = std::make_shared<m19::symbol>(node->returnType(), id, node->qualifier(), true, -node->returnType()->size());
    _symtab.insert(id, symbol);
		for (size_t i = 0; i< node->arguments()->size(); i++){
			m19::variable_declaration_node *arg = dynamic_cast<m19::variable_declaration_node *>(node->arguments()->node(i));
			basic_type *argType = new basic_type(arg->varType()->size(),arg->varType()->name());
			symbol->add_arg(argType);
		}

    _parent->set_new_symbol(symbol);
  }
}

void m19::type_checker::do_function_definition_node(m19::function_definition_node * const node, int lvl) {

	std::string id;

	if (node->qualifier() == 2)
		throw "defining an external function";

	if (node->identifier() == "m19"){
		id = "_main";
	} else if (node->identifier() == "_main"){
		id = "._main";
	} else {
		id = node->identifier();
	}

  std::shared_ptr<m19::symbol> symbol = std::make_shared<m19::symbol>(node->returnType(), id, node->qualifier(), true, -node->returnType()->size());
  std::shared_ptr<m19::symbol> previous = _symtab.find_local(id);
	if (node->returnValue())
		node->returnValue()->accept(this,lvl);

	if (previous == nullptr) {

		for (size_t i = 0; i < node->arguments()->size(); i++){
			m19::variable_declaration_node *arg = dynamic_cast<m19::variable_declaration_node *>(node->arguments()->node(i));
			basic_type *argType = new basic_type(arg->varType()->size(),arg->varType()->name());
			symbol->add_arg(argType);
		}

		_symtab.insert(id, symbol);
    _parent->set_new_symbol(symbol);

	} else {
		
		if(previous->function() == false){
			throw std::string("using an identifier already reserved for a variable in a function definition");
		}

		for (size_t i = 0; i< node->arguments()->size(); i++){
			if(node->arguments()->node(i) == nullptr)
				break;
			m19::variable_declaration_node *arg = dynamic_cast<m19::variable_declaration_node *>(node->arguments()->node(i));
			if (arg->varType()->name() != previous->argTypes()[i]->name())
				throw std::string("Invalid function definition, mismatching arguments");
		}

		_symtab.replace(id, symbol);
		_parent->set_new_symbol(symbol);
	}
}

void m19::type_checker::do_function_call_node(m19::function_call_node * const node, int lvl) {
	std::string id;

	if (node->identifier() == "m19"){
		id = "_main";
	} else if (node->identifier() == "_main"){
		id = "._main";
	} else if (node->identifier() == "@"){
		id = _parent->function_name();
	} else {
		id = node->identifier();
	}

	std::shared_ptr<m19::symbol> symbol = _symtab.find(id);
	
	node->arguments()->accept(this,lvl);

	if (symbol == nullptr){
		throw std::string("Calling undeclared function");
	}

	for (size_t i = 0; i< node->arguments()->size(); i++){
		if(node->arguments()->node(i) == nullptr)
			break;
		if(i>=symbol->argTypes().size())
			throw std::string("Invalid function call, mismatching arguments");
		cdk::expression_node *param = dynamic_cast<cdk::expression_node *>(node->arguments()->node(i));
		if (param->type()->name() != symbol->argTypes()[i]->name() && param->type()->name() != basic_type::TYPE_INT && symbol->argTypes()[i]->name() != basic_type::TYPE_DOUBLE)
			throw std::string("Invalid function call, mismatching arguments");
	}

	node->type(symbol->type());
}

void m19::type_checker::do_block_node(m19::block_node * const node, int lvl) {
}

void m19::type_checker::do_section_node(m19::section_node * const node, int lvl) {
}

void m19::type_checker::do_initial_section_node(m19::initial_section_node * const node, int lvl) {
}

void m19::type_checker::do_final_section_node(m19::final_section_node * const node, int lvl) {
}

void m19::type_checker::do_index_node(m19::index_node * const node, int lvl) {
  node->base()->accept(this, lvl);
  node->index()->accept(this, lvl);
  if (node->base()->type()->name() != basic_type::TYPE_POINTER){
    throw "indexing a nonpointer expression";
  }
  if (node->index()->type()->name() != basic_type::TYPE_INT){
    throw "index is non integer type";
  }
  node->type(node->base()->type()->subtype());
}

void m19::type_checker::do_return_node(m19::return_node * const node, int lvl) {
  //empty
}

void m19::type_checker::do_alloc_node(m19::alloc_node * const node, int lvl) {
	ASSERT_UNSPEC;
	node->argument()->accept(this,lvl);
	if (node->argument()->type()->name() != basic_type::TYPE_INT){
    throw "index is non integer type in allocation operation";
	}
	node->type(new basic_type(4, basic_type::TYPE_POINTER)); //subtype is unknown
}


