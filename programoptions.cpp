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
			 "output image filename")

			("size,s", po::value<int>(&mSize)
			 ->default_value(32),
			 "size of font in pixels")

			("resolution,r",
			 po::value<int>(&mResolution)
			 ->default_value(96),
			 "resolution in dpi")

			("fontdef-filename,o",
			 po::value<string>(&mOutputFontDef)
			 ->default_value("out.fontdef"),
			 "outupt fontdef file")

			("append,a",
			 "use if you want to append to an existing fontdef")

			("verbose,v",
			 po::value<int>(&mVerboseLevel)->default_value(false),
			 "verbose level [0-3]")

			("codepoint,c",
			 po::value<vector<CodePointRange>>(&mCodePoints)
			 ->multitoken()
			 ->default_value(CodePoints{CodePointRange{33,166}}, "33-166"),"range of cod points");				;

	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
		mShowOnlyHelp = true;

	if (showOnlyHelp())
	{
		cout << desc << endl;
		return;
	}

	if (!exist("image-filename"))
	{
		ostringstream os;

		mImageFilename = vm["input-ttf"].as<std::string>();

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

	mIsAppend = exist("append");

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
				<< "\nfont filename:      " << inputFont()
				<< "\title font resource: " << fontName()
				<< "\noutput image:       " << imageFilename()
				<< "\nextension:          " << imageExtension()
				<< "\nresolution:         " << resolution()
				<< "\nsize:               " << size()
				<< "\nfontdef filename:   " << output()
				<< "\nappend mode:        " << boolalpha << isAppend()
				<< "\nverbose level:      " << (int)verboseLevel()
				<< "\ncode point:         ";

		for(const auto& cp : mCodePoints)
			cout << cp << ' ';

		cout
				<< "\nverbose level:  " << static_cast<int>(verboseLevel())
				<< endl;
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

//const string&ProgramOptions::destPath() const
//{
//	return mDestPath;
//}

int ProgramOptions::size() const noexcept
{
	return mSize;
}

int ProgramOptions::resolution() const noexcept
{
	return mResolution;
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

//ProgramOptions::CodePointssCIt ProgramOptions::cpBegin() const noexcept
//{
//	return std::begin(mCodePoints);
//}

//ProgramOptions::CodePointssCIt ProgramOptions::cpEnd() const noexcept
//{
//	return std::end(mCodePoints);
//}

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

	// remove the dot .png

}
