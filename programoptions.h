#ifndef PROGRAMOPTIONS_H
#define PROGRAMOPTIONS_H

#include "codepage.h"

#include <string>
#include <utility>
#include <vector>
#include <iosfwd>
#include <sstream>

#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

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

	ProgramOptions(int argc, char* argv[])
	{
		namespace po = boost::program_options;

		desc.add_options()
				("help,h", "produce help message")
				("input,i", po::value<std::string>(&mFontFilename), "input ttf filename")
				("font,f", po::value<std::string>(&mFontName))
				("size,s", po::value<int>(&mSize)->default_value(32))
				("resolution,r", po::value<int>(&mResolution)->default_value(96))
				("destination,o", po::value<std::string>(&mDestination)->default_value("out.fontdef"))
				("extension,e", po::value<std::string>(&mExtension)->default_value("png"))
				("verbose,v", po::value<int>(&mVerboseLevel)->default_value(false))
				("codepage,c", po::value< std::vector<CodePointRange>>(&mCodePages)
				 ->multitoken()
				 ->default_value({{33,166}}, "33-166"),"range of cod pages")
				;

		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help"))
			mMustPrintHelp = true;

		bool must_throw = false;
		std::ostringstream os;

		if (!exist("input"))
		{
			os << "missing filename arguments!\n";
			must_throw = true;
		}

		if (!exist("font"))
		{
			os << "missing font arguments!\n";
			must_throw = true;
		}

		if (must_throw)
		{
			os << *this << std::endl;
			throw std::invalid_argument{os.str()};
		}

		if (mustPrintHelp())
			std::cout << desc << std::endl;

		logParameters();
	}

	void logParameters()
	{
		if (verboseLevel() == LogLevel::HIGH)
		{
			std::cout
			   << "font filename: " << fontFilename()
			   << "\nfontname: " << fontName()
			   << "\nsize: " << size()
			   << "\nextension: " << extension()
			   << "\nresolution: " << resolution()
			   << "\ndestination: " << destination()
			   << "\nverbose level: " << static_cast<int>(verboseLevel())
			   << "\ncode pages: ";
			for(const auto& cp : *this)
				std::cout << cp << ' ';
			std::cout << std::endl;
		}
	}

	bool mustPrintHelp() const noexcept
	{
		return mMustPrintHelp;
	}

	friend std::ostream& operator << (std::ostream& os, const ProgramOptions& po)
	{
		return os << po.desc;
	}

	const std::string& fontName() const noexcept
	{
		return mFontName;
	}

	const std::string& destination() const noexcept
	{
		return mDestination;
	}

	const std::string& extension() const noexcept
	{
		return mExtension;
	}


	const std::string& fontFilename() const noexcept
	{
		return mFontFilename;
	}

	int size() const noexcept
	{
		return mSize;
	}

	int resolution() const noexcept
	{
		return mResolution;
	}

	LogLevel verboseLevel() const noexcept
	{
		return static_cast<LogLevel>(mVerboseLevel);
	}

	CodePointssCIt begin() const noexcept
	{
		return std::begin(mCodePages);
	}

	CodePointssCIt end() const noexcept
	{
		return std::end(mCodePages);
	}

private:
	bool exist(const std::string& str) const noexcept
	{
		return vm.find(str) != std::end(vm);
	}

private:
	boost::program_options::options_description desc{"Options"};
	boost::program_options::variables_map vm;

	std::string mFontFilename;
	std::string mDestination;
	std::string mFontName;
	std::string mExtension;
	std::vector<CodePointRange> mCodePages;
	int mSize;
	int mResolution;

	int mVerboseLevel = 0;
	bool mMustPrintHelp = false;

	static const int HIGH_VERBOSE = 3;
	static const int MEDIUM_VERBOSE = 2;
	static const int LOW_VERBOSE = 1;
	static const int NO_VERBOSE = 0;
};

#endif // PROGRAMOPTIONS_H
