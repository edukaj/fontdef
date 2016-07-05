# fontdef
Simple tool that helps create pre generated fonts for ogre

# usage

	Options:
	  -h [ --help ]                      produce help message
	  -i [ --input-ttf ] arg             input ttf filename
	  -f [ --font ] arg                  name of font to use
	  --image-filename arg (=image.bmp)  output image filename
	  -s [ --size ] arg (=32)            size of font in pixels
	  -r [ --resolution ] arg (=96)      resolution in dpi
	  -o [ --output ] arg (=out.fontdef) outupt fontdef file
	  -a [ --append-mode ]               use if you want to append to an existing 
	                                     fontdef
	  -v [ --verbose ] arg (=0)
	  -c [ --codepoint ] arg (=33-166)   range of cod points


`--input-ttf`: ttf font filename
`--image-filename`: image filename containg all glyphs. The filename must contain a valid extension between bmp, png, tiff, jpg, fpeg
`--font`: title of the font used by Ogre 
`--size`: size of the font in pixels
`--resolution`: resolution in dpi
`--output`: output fontdef filename
`-a`: if used the output file will not be overwritten: all write operations are done in append.
`--codepoints`: list of code points range (for example 33-166 2048-5060 ...)

