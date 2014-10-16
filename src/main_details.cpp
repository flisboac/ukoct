#include "main.hpp"
#include "main_details.hpp"


class DetailsOptionGroupCallback : public IOptionGroupCallback {
public:
	inline EOptionGroup groupId() const {
		return OPTG_DETAILS;
	}


	bool operator()(ArgState& A, Operand* operand = NULL) const {
		bool keepgoing = true;
		return keepgoing;
	}


	void help(std::ostream& os, EHelpContents which) const {

	}
};


static const DetailsOptionGroupCallback details_operationCallback_;
const IOptionGroupCallback* const details_operationCallback = &details_operationCallback_;
