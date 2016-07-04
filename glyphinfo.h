#ifndef GLYPHINFO_H
#define GLYPHINFO_H

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

#endif // GLYPHINFO_H
