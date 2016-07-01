#ifndef PROGRAMOPTIONS_H
#define PROGRAMOPTIONS_H

#include "codepage.h"

#include <string>
#include <utility>
#include <vector>
#include <iosfwd>
#include <sstream>

#include <boost/program_options.hpp>

class ProgramOptions {
public:

	using CodePoints = std::vector< CodePoint >;
	using CodePointssCIt = CodePoints::const_iterator;

	ProgramOptions(int argc, char* argv[])
	{
		namespace po = boost::program_options;

		desc.add_options()
				("help,h", "produce help message")
				("filename,f", po::value<std::string>(&mFilename), "ttf font filename")
				("size,s", po::value<int>(&mSize)->default_value(32))
				("resolution,r", po::value<int>(&mResolution)->default_value(96))
				("codepage,c", po::value< std::vector<CodePoint>>(&mCodePages)
				 ->multitoken()
				 ->default_value({{33,166}}, "33-166"),"range of cod pages")
				;

		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help"))
			mMustPrintHelp = true;

		if (exist("filename"))
		{
			std::ostringstream os;
			os << "missing filename arguments!\n"
			   << *this << std::endl;

			throw std::invalid_argument{os.str()};
		}
	}

	std::ostream& printOn(std::ostream& os)
	{
		os << "filename: " << filename()
		   << "\nsize: " << size()
		   << "\nresolution: " << resolution()
		   << "\ncode pages: ";
		for(const auto& cp : *this)
			os << cp << ' ';
		return os << std::endl;
	}

	bool mustPrintHelp() const noexcept
	{
		return mMustPrintHelp;
	}

	friend std::ostream& operator << (std::ostream& os, const ProgramOptions& po)
	{
		return os << po.desc;
	}


	const std::string& filename() const noexcept
	{
		return mFilename;
	}

	int size() const noexcept
	{
		return mSize;
	}

	int resolution() const noexcept
	{
		return mResolution;
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
		return vm.find(str) == std::end(vm);
	}

private:
	boost::program_options::options_description desc{"Options"};
	boost::program_options::variables_map vm;

	bool mMustPrintHelp = false;
	std::string mFilename;
	std::vector<CodePoint> mCodePages;
	int mSize;
	int mResolution;
};

#endif // PROGRAMOPTIONS_H
