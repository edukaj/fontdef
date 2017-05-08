# fontdef
This utility allow you to generate font atlas from a ttf font with
desiderated font size and resolution. 
A fontdef file is also generated to allow ogre to import it as a resource.
This utility is also used for general purpose games
The generated image as power of two side dimensions.

# usage

	-h [ --help ]                         produce this message
	-i [ --input-ttf ] arg                input ttf filename
	-t [ --title-font-resource ] arg      name of font to use
	-f [ --image-filename ] arg           output image filename
	-s [ --size ] arg (=32)               True type size
	-r [ --resolution ] arg (=96)         True type resolution
	--charachter-space arg (=5)           Spacing between characters to prevent 
	                                      overlap artifacts
	--pixel-size arg (=1)                 pixel size in bytes of output image
	-o [ --fontdef-filename ] arg (=out.fontdef)
	                                      outupt fontdef file
	-a [ --append ]                       use if you want to append to an 
	                                      existing fontdef
	-v [ --verbose ] arg (=0)             verbose level [0-3]
	--use-antialias-color arg (=0)        use antialias color
	-c [ --codepoint ] arg (=33-166)      range of cod points nn-nn ...


# Dependencies 
fontdef depends from `boost-program-options`, `boost-filesystem`,`freeimage` and `freetype`

# Build

	git clone https://github.com/edukaj/fontdef
    cd fontdef
	mkdir build
	cd build
	cmake ..
	make

## Linux
On linux just type 

	sudo apt install libboost-program-options-dev libboost-filesystem-dev libboost-system-dev libfreeimage-dev libfreetype6-dev
code

or for older Ubuntu (like 14.04 and previous)

	sudo apt-get install libboost-program-options-dev libboost-filesystem-dev libboost-system-dev libfreeimage-dev libfreetype6-dev
	
after installed this dependencies follow the instruction below:


To create a redistributable debian package I've also created a simple script:
        
	cd ..
	./make-deb
    sudo dpkg -i fontdef*.deb

In this way you can use the really useful auto-complete when using the utility

## Package
I've alse created a bash script called `build-deb` that allow you to 
create a distributable package for an easier installation.

The usage is really simple `build-deb version generated-binary` where version
must be something like `major.minor-revision` for example 

	build-deb 1.0-0 
	sudo dpkg -i fontdef_1.0-0.deb

## windows

``Not tested``

download that libraries and set fontdef.pro 

Maybe some modification are required. 
