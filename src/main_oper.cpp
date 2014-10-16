#include "main.hpp"
#include "main_oper.hpp"


class OperOptionGroupCallback : public IOptionGroupCallback {
public:
	inline EOptionGroup groupId() const {
		return OPTG_OPER;
	}


	bool operator()(ArgState& A, Operand* operand = NULL) const {
		bool keepgoing = true;
		return keepgoing;
	}


	void help(std::ostream& os, EHelpContents which) const {

	}
};


static const OperOptionGroupCallback oper_operationCallback_;
const IOptionGroupCallback* const oper_operationCallback = &oper_operationCallback_;
