#ifndef __M19_SEMANTICS_SYMBOL_H__
#define __M19_SEMANTICS_SYMBOL_H__

#include <string>
#include <cdk/basic_type.h>

namespace m19 {

  class symbol {
    basic_type *_type;
    std::string _name;
    int _qualifier;
    bool _function;
		std::vector<basic_type *> _argTypes;
    int _offset; //0 offset means global

  public:
    symbol(basic_type *type, const std::string &name, int qualifier, bool function, int offset=0) :
        _type(new basic_type(type->size(), type->name())), _name(name), _qualifier(qualifier), _function(function), _offset(offset){
		basic_type *tempType = type;
		basic_type *_tempType = _type;
		while(tempType->subtype()!=nullptr){
			tempType = tempType->subtype();
			_tempType->_subtype = new basic_type(tempType->size(), tempType->name());
			_tempType=_tempType->subtype();
		}
  }


    virtual ~symbol() {
      	delete _type;
    }

    basic_type *type() const {
      return _type;
    }
    const std::string &name() const {
      return _name;
    }

    bool global(){
			return _offset==0;
		}

		bool function(){
			return _function;
		}

		std::vector<basic_type *> argTypes(){
			return _argTypes;
		}
		
		void set_offset(int offset){
			_offset=offset;
		}

		int offset(){
			return _offset;
		}

		int qualifier(){
			return _qualifier;
		}

		void add_arg(basic_type *type){
			_argTypes.push_back(type);
		}
  };

} // m19

#endif
