# README #

This README would document whatever steps are necessary to get your application up and running.

### What is this repository for? ###

This repository is to manage the source code of the configuration API, which will be delivered to the ECOSCALE project.

### RELEASE HISTORY ###

Version: 0.2
Date: 15/05/2017
    - Remove the obsolete PCAP driver for Zynq-7000 chip
    - Fix errors when compile due to ARM 32-bit assembly codes

Version: 0.1
Date: 31/03/2017
    - First release
    - The ZedBoard support for prototype

### How do I get set up? ###

* Summary of set up
* Configuration

  * How to build and install the libxml2 locally in your machine?
        Untar the libxml2.tar.gz
	Build it with native compiler
        There are many different ways to accomplish this. Here is one way to do this under Linux. Suppose your home directory is /home/user. Then:

        Create a subdirectory, let's call it myxml, unpack the libxml2 distribution into that subdirectory, chdir into the unpacked distribution (/home/user/myxml/libxml2 )
        configure the library using the "--prefix" switch, specifying an installation subdirectory in /home/user/myxml, e.g.

            ./configure --prefix /home/user/myxml/xmlinst {other configuration options}

        now run
            make
        followed by
            make install
        At this point, the installation subdirectory contains the complete "private" include files, library files and binary program files (e.g. xmllint), located in

            /home/user/myxml/xmlinst/lib, /home/user/myxml/xmlinst/include and /home/user/myxml/xmlinst/bin
        respectively.
        In order to use this "private" library, you should first add it to the beginning of your default PATH (so that your own private program files such as xmllint will be used instead of the normal system ones). To do this, the Bash command would be

            export PATH=/home/user/myxml/xmlinst/bin:$PATH
            export LD_LIB_PATH=/home/user/myxml/xmlinst/lib:$LD_LIB_PATH
        Now suppose you have a program test1.c that you would like to compile with your "private" library. Simply compile it using the command

            gcc `xml2-config --cflags --libs` -o app app.c
        Note that, because your PATH has been set with /home/user/myxml/xmlinst/bin at the beginning, the xml2-config program which you just installed will be used instead of the system default one, and this will automatically get the correct libraries linked with your program.

  * How to build this API
	Put the source code of this API on an ARM-core based machine
        Temporarily move the app.c out of the API directory
        In the config-api.c, enable the main() function by commenting out the #if 0 ... #endif
	Build it with native compiler
		export CFLAGS=`xml2-config --cflags`
		export LIBS=`xml2-config --libs`
		gcc -o config-api *.c $CFLAGS $LIBS
* API interfaces
	1.	place_accelerator_ptr(File *BitFile, int island): take the accelerator stored at the BitFile UNIMEM address and place it into the region given by island.
		For this command, data is fetched by the configuration controller in a DMA fashion through an AXI master port (basically a DMA engine). We assume the same UNIMEN memory access mechanism that will be used by the accelerators themselves. 
	2.	reset_island(int island): trigger the reset signal in the resource island.
	3.	report_available_regions(): report the number of available regions.
	4.	report_running_regions(): report all regions occupied by the hw accelerators and their process statuses.

* Hardware accelerator library generation
  This part is to describe how to construct the hardware accelerator library in the XML format. The XML library would be sent to the run-time API for partial reconfiguration.
  These steps could be finished off-line and separatedly from the run-time API.

  * How to generate the partial bitstream from the full bitstream
        There is a BitMan's command line to cut out configuration data from the full bitstream to construct a partial bitstream
            bitman -x CLB COL1 ROW1 COL2 ROW2 full_bitstream.bi[t/n] -M partial_bitstream.bi[t/n]

  * How to embed the bitstream to the xml metadata file?
        Because of difficulty to parse the pure binary data from an xml file, we decided to translate binary to ascii then embed it into the xml metadata file.
        Use the bin2ascii.py Python script to do the conversion:
            python bin2ascii.py bitstream.bi[t/n] bit_ascii.bi[t/n]
        Then copy the bitstream in the ascii format into the xml metadata between the <bitstream></bitstream> tags
        Get the number of words at the end of bit_ascii file and put them into the xml metadata between the <length></length> tags
