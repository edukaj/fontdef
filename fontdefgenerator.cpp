#include <cmath>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <functional>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <FreeImage.h>

#include "fontdefgenerator.h"

using namespace std;

class ScopeExit {
public:
	ScopeExit(std::function<void()> f): f_(f) {}
	~ScopeExit() { f_(); }
private:
	std::function<void()> f_;
};



std::ostream& operator << (std::ostream& os, FT_Face face)
{
	const int DESC_WIDTH = 50;

	const auto aviableSize = face->available_sizes;

	os
	   << std::left
	   << std::setw(DESC_WIDTH) << "size:" << face->size<< '\n'
	   << std::setw(DESC_WIDTH) << "number of glyphs:" << face->num_glyphs << '\n'
	   << std::setw(DESC_WIDTH) << "unit covered by EM:" << face->units_per_EM<< '\n'
	   << std::setw(DESC_WIDTH) << "number of embedded bitmap strikes:" << face->num_fixed_sizes << '\n';

	if (aviableSize)
	   os  << std::setw(DESC_WIDTH) << "aviable size "
		   << aviableSize->width << 'x' << aviableSize->height << ' '
		   << aviableSize->size << ' ' << aviableSize->x_ppem << 'x' << aviableSize->y_ppem;

	return os << std::endl;
}

static inline uint32_t firstPO2From(uint32_t n)
{
	--n;
	n |= n >> 16;
	n |= n >> 8;
	n |= n >> 4;
	n |= n >> 2;
	n |= n >> 1;
	++n;
	return n;
}

void FontdefGenerator::generate()
{
	FT_Library ftLib = nullptr;

	if (FT_Init_FreeType(&ftLib))
		throw runtime_error{"unable to init FreeType"};

	FT_Face face;
	if (FT_New_Face(ftLib, mProgramOptions.inputFont().c_str(), 0, &face ))
		throw runtime_error{"unable to init font face"};

	// Convert our point size to freetype 26.6 fixed point format
	const auto ttfSize = (FT_F26Dot6)(mProgramOptions.size() * (1 << 6));
	const auto ttfResolution = mProgramOptions.resolution();

	if( FT_Set_Char_Size( face, ttfSize, 0, ttfResolution, ttfResolution ) )
		throw std::runtime_error{"unable to set char size"};

	int max_width = 0;
	int max_height = 0;
	int max_bearing_y = 0;

	size_t glyphCount = 0;
	for( const auto& codePoints : mProgramOptions.codepoints() )
	{
		for (int i = codePoints.first(); i <= codePoints.last(); ++i, ++glyphCount)
		{
			FT_Load_Char( face, i, FT_LOAD_RENDER );

			if( ( 2 * ( ((int)face->glyph->bitmap.rows) << 6 ) - face->glyph->metrics.horiBearingY ) > max_height )
				max_height = ( 2 * ( ((int)face->glyph->bitmap.rows) << 6 ) - face->glyph->metrics.horiBearingY );

			if( face->glyph->metrics.horiBearingY > max_bearing_y )
				max_bearing_y = static_cast<int>(face->glyph->metrics.horiBearingY);

			if( (face->glyph->advance.x >> 6 ) + ( face->glyph->metrics.horiBearingX >> 6 ) > max_width)
				max_width = (face->glyph->advance.x >> 6 ) + ( face->glyph->metrics.horiBearingX >> 6 );
		}
	}

	const auto character_spacer = mProgramOptions.charachterSpace();

	// Now work out how big our texture needs to be
	size_t rawSize = (max_width + character_spacer) * ((max_height >> 6) + character_spacer) * glyphCount;

	uint32_t tex_side = static_cast<uint32_t>(std::sqrt((Real)rawSize));
	// just in case the size might chop a glyph in half, add another glyph width/height
	tex_side += std::max(max_width, (max_height>>6));
	uint32_t roundUpSize = firstPO2From(tex_side);

	// Would we benefit from using a non-square texture (2X width)
	uint32_t finalWidth, finalHeight;
	if (roundUpSize * roundUpSize * 0.5 >= rawSize)
		finalHeight = static_cast<uint32_t>(roundUpSize * 0.5);
	else
		finalHeight = roundUpSize;

	finalWidth = roundUpSize;

	if ((int)mProgramOptions.verboseLevel() >= (int)ProgramOptions::LogLevel::HIGH)
		std::cout
			 << "\nmax width:     " << max_width
			 << "\nmax height:    " << (max_height >> 6)
			 << "\nmax Y bearing: " << max_bearing_y
			 << "\nraw size:      " << rawSize
			 << "\ntexture side:  " << tex_side
			 << "\nrounded to ^2: " << roundUpSize
			 << "\n"
			 << "\nfinal image:   " << finalWidth << 'x' << finalHeight
			 << '\n'
			 << std::endl;

	auto textureAspect = (Real)finalWidth / (Real)finalHeight;

	const auto pixel_bytes = mProgramOptions.pixelSize();
	size_t data_width = finalWidth * pixel_bytes;
	size_t data_size = finalWidth * finalHeight * pixel_bytes;

	std::vector<uint8_t> imageData(data_size, 0);

	if (pixel_bytes == 2)
	{
		for (size_t i = 0; i < data_size; i += pixel_bytes)
			imageData[i] = 0xFF; // luminance
	}

	size_t x = 0, y = 0;

	glyphCount = 0;
	for( const auto& codepoint : mProgramOptions.codepoints() )
	{
		for (int i = codepoint.first(); i <= codepoint.last(); ++i)
		{
			if(FT_Load_Char( face, i, FT_LOAD_RENDER ))
			{
				std::cerr << "codepoint " << i << " NOT loaded" << std::endl;
				continue;
			}

			FT_Pos advance = face->glyph->advance.x >> 6;
			const uint8_t* buffer = face->glyph->bitmap.buffer;

			if (!buffer)
			{
				std::cerr  << "WARNING: Freetype returned null for character "
						   << i << " in font "
						   << mProgramOptions.inputFont() << std::endl;
				continue;
			}

			++glyphCount;
			FT_Pos y_bearing = ( max_bearing_y >> 6 ) - ( face->glyph->metrics.horiBearingY >> 6 );
			FT_Pos x_bearing = face->glyph->metrics.horiBearingX >> 6;

			if (pixel_bytes == 2)
			{
				for(unsigned int j = 0; j < face->glyph->bitmap.rows; j++ )
				{
					size_t row = j + y + y_bearing;
					uint8_t* pDest = &imageData[(row * data_width) + (x + x_bearing) * pixel_bytes];

					for(unsigned int k = 0; k < face->glyph->bitmap.width; k++ )
					{
						if (mProgramOptions.useAntialiasColor())
							*pDest++= *buffer++;
						else
							*pDest++= 0xFF;
						// Always use the greyscale value for alpha
						*pDest++= *buffer++;
					}
				}
			}
			else
			{
				const size_t bitmapRows =  face->glyph->bitmap.rows;
				const size_t bitmapWidth = face->glyph->bitmap.width;

				uint8_t* pDest = &imageData[((y + y_bearing) * data_width) + (x + x_bearing) * pixel_bytes];

				for(unsigned int j = 0; j < bitmapRows; j++, buffer += bitmapWidth, pDest += bitmapWidth)
				{
					const size_t row = j + y + y_bearing;
					uint8_t* pDest = &imageData[(row * data_width) + (x + x_bearing) * pixel_bytes];

					copy(buffer, buffer + bitmapWidth, pDest);
				}
			}

			setGlyphTexCoords(i,
							  (Real)x / (Real)finalWidth,  // u1
							  (Real)y / (Real)finalHeight,  // v1
							  (Real)( x + ( face->glyph->advance.x >> 6 ) ) / (Real)finalWidth, // u2
							  ( y + ( max_height >> 6 ) ) / (Real)finalHeight, // v2
							  textureAspect
							  );

			// Advance a column
			x += (advance + character_spacer);

			// If at end of row
			if( finalWidth - 1 < x + ( advance ) )
			{
				y += ( max_height >> 6 ) + character_spacer;
				x = 0;
			}
		}
	}

	FT_Done_FreeType(ftLib);

	saveFontImage(imageData, finalWidth, finalHeight, pixel_bytes * 8,
				  boost::algorithm::to_lower_copy(mProgramOptions.imageExtension()));

	if ((int)mProgramOptions.verboseLevel() >= (int)ProgramOptions::LogLevel::MEDIUM)
		std::cout << "\nglyph count:   " << glyphCount << std::endl;

	createFontDef();
}

void FontdefGenerator::setGlyphTexCoords(int id, Real u1, Real v1, Real u2, Real v2, Real textureAspect)
{
	GlyphInfoMap::iterator i = mGlyphMap.find(id);
	if (i != mGlyphMap.end())
	{
		i->second.uvRect.left = u1;
		i->second.uvRect.top = v1;
		i->second.uvRect.right = u2;
		i->second.uvRect.bottom = v2;
		i->second.aspectRatio = textureAspect * (u2 - u1)  / (v2 - v1);
	}
	else
	{
		mGlyphMap.insert(GlyphInfoMap::value_type(
							 id, GlyphInfo(
								 UVRect(u1, v1, u2, v2),
								 textureAspect * (u2 - u1)  / (v2 - v1))
							 )
						 );
	}
}

void FontdefGenerator::createFontDef()
{
	std::ios_base::openmode openMode = mProgramOptions.isAppend() ?
				 (std::ios_base::out | std::ios_base::app) :
				 (std::ios_base::out | std::ios_base::trunc);

	std::fstream os(mProgramOptions.output(), openMode );
	os << mProgramOptions.fontName()
	   << "\n{\n\t"
		  "type   image\n\t"
		  "source "
	   << mProgramOptions.imageFilename() << "\n\n";

	for(const auto& glyph : mGlyphMap)
	{
		const auto& glyphInfo = glyph.second;
		os << "\tglyph u" << glyph.first  << ' '
		   << glyphInfo.uvRect.left << ' '
		   << glyphInfo.uvRect.top << ' '
		   << glyphInfo.uvRect.right << ' '
		   << glyphInfo.uvRect.bottom << '\n';
	}

	os << "}\n\n" << std::flush;
}

int FontdefGenerator::extractFreeImageExtensionFrom(const std::string& ext)
{
	auto freeImageExt = FIF_BMP;

	if (ext == "bmp")
		freeImageExt = FIF_BMP;
	else if (ext == "png")
		freeImageExt = FIF_PNG;
	else if (ext == "jpg")
		freeImageExt = FIF_JPEG;
	else if (ext == "jpeg")
		freeImageExt = FIF_JPEG;
	else if (ext == "tga")
		freeImageExt = FIF_TARGA;
	else if (ext == "tiff")
		freeImageExt = FIF_TIFF;
	else
		throw std::invalid_argument{ ext + " is an unsuppordet image extension"};

	return freeImageExt;
}

void FontdefGenerator::saveFontImage(const std::vector<uint8_t>& imageData, size_t width, size_t height, int bpp, const std::string& ext)
{

	FIBITMAP* dib = FreeImage_ConvertFromRawBits(const_cast<uint8_t*>(imageData.data()),
												 width, height, width, bpp, 0, 0, 0, TRUE);

	auto freeImageExt = static_cast<FREE_IMAGE_FORMAT>(
				extractFreeImageExtensionFrom(ext));

	FreeImage_Save(freeImageExt, dib, mProgramOptions.imageFilename().c_str());
}
