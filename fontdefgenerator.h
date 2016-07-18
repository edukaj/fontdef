#pragma once
#ifndef FONTDEF_GENERATOR_H
#define FONTDEF_GENERATOR_H

#include "codepoint.h"
#include "glyphinfo.h"
#include "programoptions.h"
#include <unordered_map>

class FontdefGenerator
{
public:
	FontdefGenerator(const ProgramOptions& po);

	void generate();

private:
	void setGlyphTexCoords(int id, Real u1, Real v1, Real u2, Real v2, Real textureAspect);
	void createFontDef();
	void saveFontImage(const std::vector<uint8_t>& imageData,
					   size_t width, size_t height, int bpp,
					   const std::string& ext);

private:
	using GlyphInfoMap = std::unordered_map<int, GlyphInfo>;
	const ProgramOptions& mProgramOptions;
	GlyphInfoMap mGlyphMap;
	int extractFreeImageExtensionFrom(const std::string& ext);
};

#endif // FONTDEF_GENERATOR_H
