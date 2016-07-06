#include "codepoint.h"
using namespace std;

bool operator ==(const CodePointRange& lhs, const CodePointRange& rhs)
{
	return (lhs.first() == rhs.first()) && (lhs.last() != rhs.last());
}

ostream& operator <<(ostream& os, const CodePointRange& cp)
{
	os << cp.first() << '-' << cp.last();
	return os;
}

istream& operator >>(istream& is, CodePointRange& cp)
{
	std::string toParse;
	is >> toParse;
	auto separatorPos = toParse.find_first_of('-');

	if (separatorPos == std::string::npos)
		throw std::invalid_argument{"unable to parse code points"};

	auto firstNumber = std::stoi(toParse.substr(0, separatorPos));
	auto secondNumber = std::stoi(toParse.substr(separatorPos + 1));

	cp.first(firstNumber).last(secondNumber);
	return is;
}
