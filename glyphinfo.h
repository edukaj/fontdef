#pragma once

#include "rect.h"

using Real = float;
using UVRect = Rect<float>;

struct GlyphInfo
{
	UVRect uvRect;
	Real aspectRatio;

	GlyphInfo(const UVRect& rect, Real aspect)
		: uvRect(rect), aspectRatio(aspect)
	{
	}
};
