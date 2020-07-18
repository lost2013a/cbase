Quick Start Guide

This program "Encode_WAV_To_MP3" encodes a set of WAV files into MP3 format by using LAME library(3.99.5).
It supports both Windows and Linux environment.

Here is an overview of the deliverable contentes.

Encode_WAV_To_MP3/
├── Encode_WAV_To_MP3.c			//main source file
├── lame
│  ├── lame.h			//header of LAME library
│  ├── libmp3lame.a			//prebuilt LAME static library for Linux
│  ├── libmp3lame-static.lib	//prebuilt LAME static library for Windows
│  └── libmpghip-static.lib	//prebuilt LAME static library for Windows
├── PreBuiltExe
│  ├── Encode_WAV_To_MP3		//prebuilt console executable for Linux(x86_64)
│  └── Encode_WAV_To_MP3.exe	//prebuilt console executable for Windows(Intel 80386)
├── QuickStartGuide.txt			//this text file
└── wav_samples
    ├── bird.wav					//wav file samples obtained from the link below
    ├── cow2.wav					//http://www.externalharddrive.com/waves/animal/index.html
    ├── cow.wav
    ├── dog.wav
    ├── dolphin.wav
    ├── ...


In this package, LAME MP3 encoder is already built as static libraries, which source code can be downloaded from the link below.
http://sourceforge.net/projects/lame/files/lame/3.99/lame-3.99.5.tar.gz/download

This package contains both source code and prebuilt executables.
Please refer the folloiwing instrction for environment preparation, how to compile, and how to run the program 
depending on the OS you are working.


------------------For Windows environment-------------------

Compilation has been confirmed with Microsoft Visual C++ 2013 express on Windows7 as follows.

Environment Preparation:
(1) Install Microsoft Visual C++ 2013 express with default configuration
http://www.visualstudio.com/ja-jp/products/visual-studio-express-vs.aspx

(2) Configure command-line compiler environment variables
Launch Command Prompt, then navigate to a folder where "vcvarsall.bat" is located.
ex.) C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC

Then, run "vcvarsall.bat" on the Command Prompt.
> vcvarsall.bat

For compilation, use the same Command Prompt where "vcvarsall.bat" has been executed.

(3) Install "dirent.h"
Download and unzip the file below.
http://www.softagalleria.net/download/dirent/dirent-1.20.1.zip

Find "include/dirent.h", then put "dirent.h" into the include path.
ex.) C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include


Compile & Run:
(1) Put source file and library files as the following structure.

Encode_WAV_To_MP3
├── Encode_WAV_To_MP3.c			//main source file
└── lame
    ├── lame.h					//header of LAME library
    ├── libmp3lame-static.lib	//prebuilt LAME static library for Windows
    └── libmpghip-static.lib		//prebuilt LAME static library for Windows

Then navigate Command Prompt where "Encode_WAV_To_MP3.c" is located.

(2) Compile it with the following options.
> cl Encode_WAV_To_MP3.c /link lame/libmp3lame-static.lib lame/libmpghip-static.lib /NODEFAULTLIB:libcmt.lib

Note that "vcvarsall.bat" needs to have been executed with the same Command Prompt.
"Encode_WAV_To_MP3.exe" will be generated if the compilation is successful.

(3) Run "Encode_WAV_To_MP3.exe" with a folder path that contains WAV files.
> Encode_WAV_To_MP3 "folder path"
ex.) > Encode_WAV_To_MP3 "C:\Documents and Settings\wav_samples"

For folder path, please enclose the string by double quotations(""). Note that multibyte characers are invalid.
Either absolute or relative path are acceptable.

After the program finishes, the encoded MP3 files will be generated in the same folder.
------------------------------------------------------------


--------------------For Linux environment-------------------

Compilation has been confirmed with gcc 4.6.3 on Ubuntu-12.04 LTS x86_64 as follows.

Environment Preparation:
(1) Install GNU Compiler Collection.
ex.) $ sudo apt-get install gcc-4.6


Compile & Run:
(1) Put source and library files as the following structure.

Encode_WAV_To_MP3
├── Encode_WAV_To_MP3.c	//main source file
└── lame
    ├── lame.h			//header of LAME library
    └── libmp3lame.a		//prebuilt main executable for Linux

Then navigate the directory where "Encode_WAV_To_MP3.c" is located.

(2) Compile it with the following options.
$ gcc Encode_WAV_To_MP3.c -L./lame -lmp3lame -lm -lpthread -o Encode_WAV_To_MP3

"Encode_WAV_To_MP3" will be generated if the compilation is successful.

(3) Run "Encode_WAV_To_MP3" with a folder path that contains WAV files.
$ ./Encode_WAV_To_MP3 <folder path>
ex.) $ ./Encode_WAV_To_MP3 ~/wav_samples

Either absolute or relative path are acceptable.
In Linux environment, you may NOT enclose the string by double quotations(""), also multibyte characers are valid.

After the program finishes, the encoded MP3 files will be generated in the same folder.
-------------------------------------------------------------
