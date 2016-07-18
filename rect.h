#pragma once
#ifndef RECT_H
#define RECT_H

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

#endif // RECT_H
