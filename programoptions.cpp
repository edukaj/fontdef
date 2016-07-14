#include "programoptions.h"
using namespace std;

#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>

boost::filesystem::path getFilenameWithoutExtension(const boost::filesystem::path& p)
{
	boost::filesystem::path result = p;
	return result.replace_extension("");
}

ProgramOptions::ProgramOptions(int argc, char* argv[])
{
	namespace po = boost::program_options;

	desc.add_options()
			("help,h", "produce this message")

			("input-ttf,i",
			 po::value<string>(&mInputFont),
			 "input ttf filename")

			("title-font-resource,t",
			 po::value<string>(&mFontName),
			 "name of font to use")

			("image-filename,f",
			 po::value<string>(&mImageFilename),
			 "output image filename")

			("size,s", po::value<float>(&mSize)
			 ->default_value(32.0),
			 "True type size")

			("resolution,r",
			 po::value<int>(&mResolution)
			 ->default_value(96),
			 "True type resolution")

			("charachter-space",
			 po::value<int>(&mCharSpace)
			 ->default_value(5),
			 "Spacing between characters to prevent overlap artifacts")

			("pixel-size",
			 po::value<int>(&mPixelSize)
			 ->default_value(1),
			 "pixel size in bytes of output image")

			("fontdef-filename,o",
			 po::value<string>(&mOutputFontDef)
			 ->default_value("out.fontdef"),
			 "outupt fontdef file")

			("append,a",
			 po::value<bool>(&mIsAppend)
			 ->default_value(false),
			 "use if you want to append to an existing fontdef")

			("verbose,v",
			 po::value<int>(&mVerboseLevel)
			 ->default_value(0),
			 "verbose level [0-3]")

			("use-antialias-color",
			 po::value<bool>(&mUseAntialiasColor)
			 ->default_value(false),
			 "use antialias color")

			("codepoint,c",
			 po::value<vector<CodePointRange>>(&mCodePoints)
			 ->multitoken()
			 ->default_value(CodePoints{CodePointRange{33,166}}, "33-166"),"range of cod points nn-nn ...");				;

	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (exist("help") || (argc == 1))
		mShowOnlyHelp = true;

	if (showOnlyHelp())
	{
		cout << desc << endl;
		return;
	}

	if (!exist("image-filename"))
	{
		ostringstream os;

		mImageFilename = vm["input-ttf"].as<string>();

		boost::filesystem::path p{mInputFont};

		os << getFilenameWithoutExtension(p.filename()).c_str()
		   << '_' << size() << '_' << resolution() << ".png";

		mImageFilename = os.str();
	}

	bool must_throw = false;
	ostringstream os;

	if (!exist("input-ttf"))
	{
		os << "missing filename arguments!\n";
		must_throw = true;
	}

	if (!exist("fontdef-filename"))
	{
		os << "missing font arguments!\n";
		must_throw = true;
	}

	if (!exist("title-font-resource"))
	{
		os << "missing font title resource\n";
		must_throw = true;
	}

	if (must_throw)
	{
		os << *this << endl;
		throw invalid_argument{os.str()};
	}

	extractExtension();

	logParameters();
}

void ProgramOptions::logParameters()
{
	if ((int)verboseLevel() >= (int)LogLevel::MEDIUM)
	{
		cout
				<< "\nfont filename:       " << inputFont()
				<< "\ntitle font resource: " << fontName()
				<< "\noutput image:        " << imageFilename()
				<< "\nextension:           " << imageExtension()
				<< "\nsize:                " << size()
				<< "\nresolution:          " << resolution()
				<< "\ncharchter space:     " << charachterSpace()
				<< "\npixel size:          " << pixelSize()
				<< "\nfontdef filename:    " << output()
				<< "\nappend mode:         " << boolalpha << isAppend()
				<< "\nuse antialias color: " << boolalpha << useAntialiasColor()
				<< "\nverbose level:       " << (int)verboseLevel()
				<< "\ncode point:          ";

		for(const auto& cp : mCodePoints)
			cout << cp << ' ';

		cout << endl;
	}
}

bool ProgramOptions::showOnlyHelp() const noexcept
{
	return mShowOnlyHelp;
}

ostream& operator << (ostream& os, const ProgramOptions& po)
{
	return os << po.desc;
}

const string&ProgramOptions::fontName() const noexcept
{
	return mFontName;
}

const string&ProgramOptions::output() const noexcept
{
	return mOutputFontDef;
}

const string&ProgramOptions::inputFont() const noexcept
{
	return mInputFont;
}

float ProgramOptions::size() const noexcept
{
	return mSize;
}

int ProgramOptions::resolution() const noexcept
{
	return mResolution;
}

int ProgramOptions::charachterSpace() const noexcept
{
	return mCharSpace;
}

int ProgramOptions::pixelSize() const noexcept
{
	return mPixelSize;
}

bool ProgramOptions::useAntialiasColor() const noexcept
{
	return mUseAntialiasColor;
}

const string&ProgramOptions::imageFilename() const noexcept
{
	return mImageFilename;
}

const string&ProgramOptions::imageExtension() const noexcept
{
	return mImageExtension;
}

bool ProgramOptions::isAppend() const noexcept
{
	return mIsAppend;
}

ProgramOptions::LogLevel ProgramOptions::verboseLevel() const noexcept
{
	return static_cast<LogLevel>(mVerboseLevel);
}

const ProgramOptions::CodePoints&ProgramOptions::codepoints() const noexcept
{
	return mCodePoints;
}

bool ProgramOptions::exist(const string& str) const noexcept
{
	return vm.find(str) != std::end(vm);
}

void ProgramOptions::extractExtension()
{
	boost::filesystem::path p{mImageFilename};

	if (!p.has_extension())
		throw invalid_argument{"image filename without extension"};

	auto extWithDot = std::string{p.extension().c_str()};
	mImageExtension = extWithDot.substr(1);
}
