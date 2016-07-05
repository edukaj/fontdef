#pragma once

#include <iosfwd>
#include <string>
#include <unordered_map>

using Real = float;

class CodePointRange {
public:
	CodePointRange(int first = 0, int last = 0)
		: mFirst{first}, mLast{last}
	{
	}

	int first() const noexcept { return mFirst; }
	int last() const noexcept { return mLast; }

	CodePointRange& first(int val) noexcept { mFirst = val; return *this; }
	CodePointRange& last(int val) noexcept { mLast = val; return *this; }

	friend bool operator == (const CodePointRange& lhs, const CodePointRange& rhs)
	{
		return (lhs.first() == rhs.first()) && (lhs.last() != rhs.last());
	}

	friend std::ostream& operator << (std::ostream& os, const CodePointRange& cp)
	{
		os << cp.first() << '-' << cp.last();
		return os;
	}

	friend std::istream& operator >> (std::istream& is, CodePointRange& cp)
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

private:
	int mFirst;
	int mLast;
};
