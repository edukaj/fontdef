#pragma once
#ifndef CODEPOINT_H
#define CODEPOINT_H

#include <iosfwd>
#include <string>
#include <unordered_map>

using Real = float;

class CodePointRange {
public:
	explicit CodePointRange(int first = 0, int last = 0)
		: mFirst{first}, mLast{last}
	{}

	int first() const noexcept { return mFirst; }
	int last() const noexcept { return mLast; }

	CodePointRange& first(int val) noexcept { mFirst = val; return *this; }
	CodePointRange& last(int val) noexcept { mLast = val; return *this; }

	friend bool operator == (const CodePointRange& lhs, const CodePointRange& rhs);

	friend std::ostream& operator << (std::ostream& os, const CodePointRange& cp);
	friend std::istream& operator >> (std::istream& is, CodePointRange& cp);

private:
	int mFirst;
	int mLast;
};

#endif // CODEPOINT_H
