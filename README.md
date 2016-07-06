# fontdef
Simple tool that helps create pre generated fonts for ogre

# usage

    -h [ --help ]                         produce this message
    -i [ --input-ttf ] arg                input ttf filename
    -n [ --fontname ] arg                 name of font to use
    -f [ --image-filename ] arg (=image.bmp)
                                        output image filename
    -s [ --size ] arg (=32)               size of font in pixels
    -r [ --resolution ] arg (=96)         resolution in dpi
    -o [ --output ] arg (=out.fontdef)    outupt fontdef file
    -a [ --append-mode ]                  use if you want to append to an 
                                        existing fontdef
    -v [ --verbose ] arg (=0)             verbose level [0-3]
    -c [ --codepoint ] arg (=33-166)      range of cod points



* `--input-ttf`: ttf font filename
* `--fontname`: title of the font used by Ogre 
* `--image-filename`: image filename containg all glyphs. The filename must contain a valid extension between bmp, png, tiff, jpg, jpeg
* `--size`: size of the font in pixels
* `--resolution`: resolution in dpi
* `--output`: output fontdef filename
* `--append-mode`: if present all write operation are done in append mode without overwrite existing fontdef content
* `--codepoints`: list of code points range (for example 33-166 2048-5060 ...)

# Build
Build this project is really simple just open it with qtcreator and build

# Dependencies 
fontdef depends from `boost-program-options` and `freeimage`

## Linux
On linux just type 

	sudo apt install libboost-program-options-dev libfreeimage-dev

or

	sudo apt-get install libboost-program-options-dev libfreeimage-dev
	
## windows
download that libraries and set fontdef.pro 

Maybe some modification are required. 
