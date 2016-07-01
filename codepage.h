#pragma once

#include <iosfwd>
#include <string>
#include <unordered_map>

using Real = float;

class CodePoint {
public:
	CodePoint(int first = 0, int last = 0)
		: mFirst{first}, mLast{last}
	{
	}

	int first() const noexcept { return mFirst; }
	int last() const noexcept { return mLast; }

	CodePoint& first(int val) noexcept { mFirst = val; return *this; }
	CodePoint& last(int val) noexcept { mLast = val; return *this; }

	friend bool operator == (const CodePoint& lhs, const CodePoint& rhs)
	{
		return (lhs.first() == rhs.first()) && (lhs.last() != rhs.last());
	}

	friend std::ostream& operator << (std::ostream& os, const CodePoint& cp)
	{
		os << cp.first() << '-' << cp.last();
		return os;
	}

	friend std::istream& operator >> (std::istream& is, CodePoint& cp)
	{
		std::string toParse;
		is >> toParse;
		auto separatorPos = toParse.find_first_of('-');

		if (separatorPos == std::string::npos)
			throw std::invalid_argument{"unable to parse codepage"};

		auto firstNumber = std::stoi(toParse.substr(0, separatorPos));
		auto secondNumber = std::stoi(toParse.substr(separatorPos + 1));

		cp.first(firstNumber).last(secondNumber);
		return is;
	}

private:
	int mFirst;
	int mLast;
};

struct CodePointHash {
	std::size_t operator()(const CodePoint& k) const
	{
		return std::hash<int>()(k.first()) ^
				std::hash<int>()(k.last());
	}
};

template< typename T>
struct Rect {
	T left, top, right, bottom;

	Rect() : left(0), top(0), right(0), bottom(0) {}
	Rect( T const & l, T const & t, T const & r, T const & b )
		: left( l ), top( t ), right( r ), bottom( b )
	{
	}
	Rect( Rect const & o )
		: left( o.left ), top( o.top ), right( o.right ), bottom( o.bottom )
	{
	}
	Rect & operator=( Rect const & o )
	{
		left = o.left;
		top = o.top;
		right = o.right;
		bottom = o.bottom;
		return *this;
	}
	T width() const
	{
		return right - left;
	}
	T height() const
	{
		return bottom - top;
	}
	bool isNull() const
	{
		return width() == 0 || height() == 0;
	}
	void setNull()
	{
		left = right = top = bottom = 0;
	}
	Rect & merge(const Rect& rhs)
	{
		if (isNull())
		{
			*this = rhs;
		}
		else if (!rhs.isNull())
		{
			left = std::min(left, rhs.left);
			right = std::max(right, rhs.right);
			top = std::min(top, rhs.top);
			bottom = std::max(bottom, rhs.bottom);
		}

		return *this;

	}
	Rect intersect(const Rect& rhs) const
	{
		Rect ret;
		if (isNull() || rhs.isNull())
		{
			// empty
			return ret;
		}
		else
		{
			ret.left = std::max(left, rhs.left);
			ret.right = std::min(right, rhs.right);
			ret.top = std::max(top, rhs.top);
			ret.bottom = std::min(bottom, rhs.bottom);
		}

		if (ret.left > ret.right || ret.top > ret.bottom)
		{
			// no intersection, return empty
			ret.left = ret.top = ret.right = ret.bottom = 0;
		}

		return ret;
	}
};

using UVRect = Rect<float>;

struct GlyphInfo
{
	CodePoint codePoint;
	UVRect uvRect;
	Real aspectRatio;

	GlyphInfo(CodePoint id, const UVRect& rect, Real aspect)
		: codePoint(id), uvRect(rect), aspectRatio(aspect)
	{
	}
};

using CodePointMap = std::unordered_map<CodePoint, GlyphInfo, CodePointHash>;
