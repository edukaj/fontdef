#include <cmath>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <FreeImage.h>

#include "font.h"

#define GRAYSCALE 1

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

Font::Font(const ProgramOptions& po) : mProgramOptions{po}
{
	FT_Library ftLib = nullptr;

	if (FT_Init_FreeType(&ftLib))
		throw std::runtime_error{"unable to init FreeType"};

	FT_Face face;
	if (FT_New_Face(ftLib, po.fontFilename().c_str(), 0, &face ))
		throw std::runtime_error{"unable to init font face"};

	// Convert our point size to freetype 26.6 fixed point format

	FT_F26Dot6 ftSize = (FT_F26Dot6)(po.size() * (1 << 6));
	if( FT_Set_Char_Size( face, ftSize, 0, po.resolution(), po.resolution() ) )
		throw std::runtime_error{"unable to set char size"};

	int max_width = 0;
	int max_height = 0;
	int max_bearing_y = 0;

#if !GRAYSCALE
	bool antialiasColour = false;
#endif

	size_t glyphCount = 0;
	for( const auto& codepage : po )
	{
		for (int i = codepage.first(); i <= codepage.last(); ++i, ++glyphCount)
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

	auto character_spacer = 2;

	// Now work out how big our texture needs to be
	size_t rawSize = (max_width + character_spacer) *
						((max_height >> 6) + character_spacer) * glyphCount;

	uint32_t tex_side = static_cast<uint32_t>(sqrt((double)rawSize));
	// just in case the size might chop a glyph in half, add another glyph width/height
	tex_side += std::max(max_width, (max_height>>6));
	uint32_t roundUpSize = firstPO2From(tex_side);

	// Would we benefit from using a non-square texture (2X width)
	uint32_t finalWidth, finalHeight;
	if (roundUpSize * roundUpSize * 0.5 >= rawSize)
	{
		finalHeight = static_cast<uint32_t>(roundUpSize * 0.5);
	}
	else
	{
		finalHeight = roundUpSize;
	}
	finalWidth = roundUpSize;

	if (mProgramOptions.isVerbose())
		std::cout
			 << "\nmax width:     " << max_width
			 << "\nmax height:    " << max_height
			 << "\nmax Y bearing: " << max_bearing_y
			 << "\nraw size:      " << rawSize
			 << "\ntexture side:  " << tex_side
			 << "\nrounded to ^2: " << roundUpSize
			 << "\n"
			 << "\nfinal image:   " << finalWidth << 'x' << finalHeight
			 << '\n'
			 << std::endl;

	auto textureAspect = (double)finalWidth / (double)finalHeight;

#if GRAYSCALE
	const size_t pixel_bytes = 1;
#else
	const size_t pixel_bytes = 2;
#endif

	size_t data_width = finalWidth * pixel_bytes;
	size_t data_size = finalWidth * finalHeight * pixel_bytes;

	size_t l = 0, m = 0;

	std::vector<uint8_t> imageData(data_size);

	glyphCount = 0;
	for( const auto& codepage : po )
	{
		for (int i = codepage.first(); i <= codepage.last(); ++i)
		{
			if(FT_Load_Char( face, i, FT_LOAD_RENDER ))
			{
				if (mProgramOptions.isVerbose())
					std::cout << "codepoint " << i << " NOT loaded" << std::endl;
				continue;
			}

			unsigned char* buffer = face->glyph->bitmap.buffer;

			if (!buffer)
			{
				std::cerr  << " Freetype returned null for character " << i << " in font " << po.fontFilename() << std::endl;
				continue;
			}

			++glyphCount;
			FT_Pos advance = face->glyph->advance.x >> 6;
			FT_Pos y_bearing = ( max_bearing_y >> 6 ) - ( face->glyph->metrics.horiBearingY >> 6 );
			FT_Pos x_bearing = face->glyph->metrics.horiBearingX >> 6;

			for(unsigned int j = 0; j < face->glyph->bitmap.rows; j++ )
			{
				size_t row = j + m + y_bearing;
				uint8_t* pDest = &imageData[(row * data_width) + (l + x_bearing) * pixel_bytes];
				for(unsigned int k = 0; k < face->glyph->bitmap.width; k++ )
				{
#if GRAYSCALE
					// Always use the greyscale value for alpha
					*pDest++= *buffer++;
#else
					if (antialiasColour)
					{
						// Use the same greyscale pixel for all components RGBA
						*pDest++= *buffer;
					}
					else
					{
						// Always white whether 'on' or 'off' pixel, since alpha
						// will turn off
						*pDest++= 0xFF;
					}
					// Always use the greyscale value for alpha
					*pDest++= *buffer++;
#endif
				}
			}

			setGlyphTexCoords(i,
							  (double)l / (double)finalWidth,  // u1
							  (double)m / (Real)finalHeight,  // v1
							  (double)( l + ( face->glyph->advance.x >> 6 ) ) / (Real)finalWidth, // u2
							  ( m + ( max_height >> 6 ) ) / (Real)finalHeight, // v2
							  textureAspect
							  );

			// Advance a column
			l += (advance + character_spacer);

			// If at end of row
			if( finalWidth - 1 < l + ( advance ) )
			{
				m += ( max_height >> 6 ) + character_spacer;
				l = 0;
			}
		}
	}


	if (ftLib != nullptr)
		FT_Done_FreeType(ftLib);

#if GRAYSCALE
	int bpp = 8;
#else
	int bpp = 16;
#endif

	saveFontImage(imageData, finalWidth, finalHeight, bpp,
				  boost::algorithm::to_lower_copy(mProgramOptions.extension()));


	if (mProgramOptions.isVerbose())
		std::cout << "\nglyph count:   " << glyphCount << std::endl;

	createFontDef();

}

void Font::setGlyphTexCoords(int id, Real u1, Real v1, Real u2, Real v2, Real textureAspect)
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

void Font::createFontDef()
{
	std::ofstream os(mProgramOptions.destination() + std::string{".fontdef"} );

	os << mProgramOptions.fontName()
	   << "\n{\n\t"
		  "type   image\n\t"
		  "source "
	   << mProgramOptions.destination() << '.' << mProgramOptions.extension() << "\n\n";

	for(const auto& glyph : mGlyphMap)
	{
		const auto& glyphInfo = glyph.second;
		os << "\tglyph u" << glyph.first  << ' '
		   << glyphInfo.uvRect.left << ' '
		   << glyphInfo.uvRect.top << ' '
		   << glyphInfo.uvRect.right << ' '
		   << glyphInfo.uvRect.bottom << '\n';
	}

	os << '}' << std::endl;
}

void Font::saveFontImage(const std::vector<uint8_t>& imageData, size_t width, size_t height, int bpp, const std::string& ext)
{

	FIBITMAP* dib = FreeImage_ConvertFromRawBits(const_cast<uint8_t*>(imageData.data()),
												 width, height, width, bpp, 0, 0, 0, TRUE);

	if (ext == std::string{"bmp"})
		FreeImage_Save(FIF_BMP, dib, (mProgramOptions.destination() + std::string{".bmp"}).c_str());
	else if (ext == std::string{"png"})
		FreeImage_Save(FIF_PNG, dib, (mProgramOptions.destination() + std::string{".png"}).c_str());
	else if (ext == std::string{"dds"})
		FreeImage_Save(FIF_DDS, dib, (mProgramOptions.destination() + std::string{".dds"}).c_str());
}
