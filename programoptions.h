#pragma once

#include "codepoint.h"
#include <boost/program_options.hpp>
#include <iosfwd>

class ProgramOptions {
public:

	using CodePoints = std::vector< CodePointRange >;
	using CodePointssCIt = CodePoints::const_iterator;

	enum class FileFormat {
		BMP, JPEG, PNG, DDS
	};

	enum class LogLevel {
		NONE, LOW, MEDIUM, HIGH
	};

	ProgramOptions(int argc, char* argv[]);

	bool showOnlyHelp() const noexcept;

	friend std::ostream& operator << (std::ostream& os, const ProgramOptions& po);

	const std::string& fontName() const noexcept;
	const std::string& output() const noexcept;
	const std::string& inputFont() const noexcept;
	float size() const noexcept;
	int resolution() const noexcept;
	int charachterSpace() const noexcept;
	int pixelSize() const noexcept;
	bool useAntialiasColor() const noexcept;

	const std::string& imageFilename() const noexcept;
	const std::string& imageExtension() const noexcept;
	bool isAppend() const noexcept;
	LogLevel verboseLevel() const noexcept;
	const CodePoints& codepoints() const noexcept;

private:
	bool exist(const std::string& str) const noexcept;
	void logParameters();
	void extractExtension();


private:
	boost::program_options::options_description desc{"Options"};
	boost::program_options::variables_map vm;

	std::string mInputFont;
	std::string mOutputFontDef;
	std::string mFontName;
	std::string mImageFilename;
	std::string mImageExtension;
	std::vector<CodePointRange> mCodePoints;
	float mSize;
	int mResolution;
	int mCharSpace;
	int mVerboseLevel;
	int mPixelSize;
	bool mShowOnlyHelp = false;
	bool mIsAppend = false;
	bool mUseAntialiasColor;
};
