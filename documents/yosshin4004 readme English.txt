# xdev68k


## Commentary

xdev68k is a cross compilation environment for SHARP X680x0 series.
X68K compatible executable files can be created using the latest gcc.
Unix-compatible environments such as msys+mingw, cygwin, linux, WSL, etc. are available for the host environment (mac is probably available, but not verified).

xdev68k evolved from an older project, x68k_gcc_has_converter (https://github.com/yosshin4004/x68k_gcc_has_converter).
The old project was closed and merged into this project.


## Environment construction procedure

1. Installation and environment construction of Unix compatible environment (work time: about 10 minutes)
	Prepare a Unix compatible environment such as msys+mingw, cygwin, linux, WSL, etc.
	Here, only the installation procedure when using the recommended environment msys+mingw is shown.

	The msys installer is available from https://www.msys2.org/.
	After installation, execute the following on the msys console to install gcc, perl, and other tools necessary for building the environment.
	```bash
	pacman -S base-devel
	pacman -S mingw-w64-i686-toolchain
	pacman -S mingw-w64-x86_64-toolchain
	pacman -S autoconf-wrapper
	pacman -S msys/autoconf
	pacman -S msys/automake-wrapper
	pacman -S unzip
	pacman -S cmake
	pacman -S libiconv
	pacman -S git
	pacman -S pax
	```
	In msys perl, the locale is not set correctly in the initial state, and the following warning is issued each time perl is started.
	```bash
	perl: warning: Setting locale failed.
	perl: warning: Please check that your locale settings:
	```
	To solve this, add the following specification to C:/msys64/home/username/.bashrc.

	```bash
	# Perl's locale warning countermeasures
	export LC_ALL="C"
	export LC_CTYPE="C"
	export LANG="en_US.UTF-8"
	```
	Start the bash console of msys from the shortcut of "MSYS2 MinGW 64bit" in the start menu.
	Please note that if you do not start from here, the path to the native compiler environment will not pass and cross-compiler construction will fail.

	![screen_shot_gfx](https://user-images.githubusercontent.com/11882108/154822283-4b208ca4-8a69-4b34-a160-5b7845cbaa2a.png)

2. Get xdev68k (working time: about 1 minute)
	Clone this repository.
	```bash
	git clone https://github.com/yosshin4004/xdev68k.git
	```
	From now on, we will proceed with the explanation assuming that it has been cloned to the directory xdev68k.

3. Creating a cross-compiler (work time: several hours depending on the environment)
	After confirming that there is enough disk space (about 10 GB),
	On the bash console of the host environment, move to the cloned directory xdev68k and execute the following command.
	```bash
	bash ./build_m68k-toolchain.sh
	```
	Under xdev68k/m68k-toolchain, m68k-elf-gcc, a cross-compiler for Motorola 680x0 series, is built.
	If the following message is output to the console, it is completed.
	```
	The building process is completed successfully.
	```

4. Utility installation (work time: about a few minutes)
	This operation automatically downloads or installs the following files.
	You can check the details of the target file at the following URL.

	* SHARP C Compiler PRO-68K ver2.1 System Disk 1 & 2
	http://retropc.net/x68000/software/sharp/xc21/
	(File name XC2101.LZH, XC2102_02.LZH)
	* HAS060.X  
	http://retropc.net/x68000/software/develop/as/has060/
	(File name HAS06091.LZH)
	* HLK v3.01  
	http://retropc.net/x68000/software/develop/lk/hlk/
	(File name HLK301B.LZH)
	* g2as g2lk (part of Charlie's GCC)
	http://retropc.net/x68000/software/develop/c/gcc2/
	(File name G3_20.LZH)
	* X68K command line emulator run68 
	https://github.com/GOROman/run68mac

	On the bash console of the host environment, execute the following command from the same directory (xdev68k) as before.
	```bash
	bash ./install_xdev68k-utils.sh
	```
	Installs utility executables, headers, and libraries needed in a cross-compile environment.
	If the following message is output to the console, it is completed.
	```
	The installation process is completed successfully.
	Please set the current directory path to environment variable XDEV68K_DIR.
	```

5. Environment variable setting (work time: about 1 minute)
	Set the environment variable XDEV68K_DIR to the full path of the directory xdev68k.
	In the case of msys, it is a good idea to write the following in C:/msys64/home/username/.bashrc.
	```bash
	export XDEV68K_DIR=Full path of directory xdev68k
	```
	Specify the full path in unix style starting with /c, not windows style starting with drive name such as C:.

## file organization

The directory structure when the environment construction is completed correctly is as follows.
```
xdev68k/
│
├ archive/
│	│	Archive files of software used
│	├ readme.txt
│	│		Text that summarizes the original author, source information, and terms of use
│	├ libgcc_src.tar.gz
│	│		Source code of libgcc
│	├ libstdc++_src.tar.gz
│	│		Source code of libstdc++
│	├ *.zip *.lzh
│	│		Original archive file
│	└ download/
│			Downloaded software archive file
├ build_gcc/
│		Intermediate files generated by cross-compiler source code and build
│ 		Nearly 180,000 files exist under this directory. You can safely delete it.
├ example/
│		sample code
├ include/
│	│	header file
│	├ xc/
│	│		Header file of SHARP C Compiler PRO-68K ver2.1
│	└ xdev68k/
│			Header files added in xdev68k environment
├ m68k-toolchain/
│		Cross compiler build result
│ 		There are about 1700 files under this directory. Do not delete.
├ run68/
│	│	X68K command line emulator run68
│	├ run68.exe
│	│		run68 executable
│	└ run68.ini
│			run68 configuration file
├ lib/
│	│	library file
│	├ xc/
│	│		Library file of SHARP C Compiler PRO-68K ver2.1
│	└ m68k_elf/
│		│
│		├ m68000/ m68020/ m68040/ m68060/ 
│		│	└ *.a
│		│			Library files for various CPU configurations
│		└ *.a		
│				Library files that are CPU-agnostic
├ util/
│	│
│	├ atomic.lock
│	│		Lock file used by atomic.pl
│	├ atomic.pl
│	│		A script that executes a given command line in a single thread
│	├ db_pop_states.txt
│	│		Debugger command to restore state of interrupt handling
│	├ db_push_states.txt
│	│		Debugger command for saving interrupt processing state
│	├ xeij_*.bat
│	│		Batch file for XEiJ control
│	├ xeij_remote_debug.sh
│	│		Script to debug and execute specified file on XEiJ
│	└ x68k_gas2has.pl
│			GAS to HAS converter
├ x68k_bin/
│	│	X68K command line utilities
│	├ AR.X
│	│		X68k Archiver v1.00
│	├ DB.X
│	│		X68k Debugger v2.00
│	├ g2as.x
│	│		X68k High-speed Assembler v3.08 modified for GCC
│	├ g2lk.x
│	│		X68k SILK Hi-Speed Linker v2.29 modified for GCC
│	├ HAS060.X
│	│		High-speed Assembler 68060 対応版 version 3.09+91
│	├ hlk301.x
│	│		HLK v3.01
│	└ MEMSIZE.X
│			Output free memory size to console (for run68 operation test)
├ build_m68k-toolchain.sh
│		cross compiler build script
├ build_x68k-libgcc.sh
│		build script for libgcc
├ build_x68k-libstdc++.sh
│		build script for libstdc++
└ install_xdev68k-utils.sh
		utility install script
```

## Running the Hello World example

After building the environment, let's build and run the basic sample as a test.
On the bash console of your host environment, go to the directory xdev68k and run:
```bash
cd example/hello
make
```
A file called MAIN.X will be generated in the current directory. Here is her X68K executable.
Copy MAIN.X to the X68K real machine or emulator environment and run it (manually because it is not automated by makefile).
It is successful if the following is output to the X68K console.
```bash
hello world.
```


## Details from Compilation to Executable File Generation

Using the previous Hello World sample source file main.c as an example,
I will explain the flow from compilation to execution file generation.
Build work is done on the bash console of the host environment.

1. compile  
	Compile main.c with the cross-compiler m68k-elf-gcc.
	```bash
	# Compile main.c.
	# -I${XDEV68K_DIR}/include/xc : Specifying include paths
	# -Os : Size-first optimization
	# -fcall-used-d2 -fcall-used-a2 : Mark d2 a2 as destructive register to match X68K and ABI
	# -Wno-builtin-declaration-mismatch : Suppress warnings
	${XDEV68K_DIR}/m68k-toolchain/bin/m68k-elf-gcc main.c -I${XDEV68K_DIR}/include/xc -S -Os -m68000 -fcall-used-d2 -fcall-used-a2 -Wno-builtin-declaration-mismatch -o main.m68k-gas.s
	```
	A source file main.m68k-gas.s is generated in the current directory.

2. convert assembler source  
	main.m68k-gas.s is written in a format called GAS format.
	Use x68k_gas2has.pl to convert to HAS format that can be used on X68K.
	```bash
	# Converts to a form that HAS.X can assemble.
	# -cpu option : Target CPU type
	# -inc option : Files to include at the beginning of the source
	perl ${XDEV68K_DIR}/util/x68k_gas2has.pl -i main.m68k-gas.s -o main.s -cpu 68000 -inc doscall.mac,iocscall.mac
	```
	A source file main.s in HAS format will be generated in the current directory.

3. Assemble  
	Assemble main.s with the X68K compatible assembler HAS060.X.
	The assembler is executed with the X68K command line emulator run68.
	```bash
	# Assemble main.s.
	# -u : 未定義シンボルを外部参照にする 
	# -e : Make an undefined symbol an external reference 
	# -w0 : Suppress warnings
	# -I${XDEV68K_DIR}/include/xc : include パスの指定
	HAS="${XDEV68K_DIR}/run68/run68 ${XDEV68K_DIR}/x68k_bin/HAS060.X"
	${HAS} -e -u -w0 -I${XDEV68K_DIR}/include/xc -o main.o main.s
	```
	An object file main.o is generated in the current directory.

4. Link  
	Link main.o with the X68K compatible linker hlk301.x.
	Run the linker with the X68K command line emulator run68.
	You need to link the runtime library included in this repository.
	```bash
	# Link main.o.
	# Since it's difficult to give the HLK a long path character,
	# As a workaround, copy the file to be linked under lk_tmp,
	# Causes the link to be performed using a short relative path.
	rm -rf lk_tmp
	mkdir -p lk_tmp
	cp main.o lk_tmp/
	cp ${XDEV68K_DIR}/lib/xc/CLIB.L lk_tmp/
	cp ${XDEV68K_DIR}/lib/xc/FLOATFNC.L lk_tmp/
	cp ${XDEV68K_DIR}/lib/m68k_elf/m68000/libgcc.a lk_tmp/
	ls lk_tmp/ > lk_list.txt
	HLK="${XDEV68K_DIR}/run68/run68 ${XDEV68K_DIR}/x68k_bin/hlk301.x"
	${HLK} -Llk_tmp/ -o MAIN.X -i lk_list.txt
	```
	An executable file MAIN.X will be generated in the current directory.


## GAS format → HAS format conversion example

x68k_gas2has.pl located under the directory xdev68k/util is 
a converter that converts assembler source from GAS format to HAS format.
The source code generated by x68k_gas2has.pl has his HAS format 
on the left and the original GAS format on the right, as shown below.
```
* NO_APP
RUNS_HUMAN_VERSION      equ     3
        .cpu 68000
* X68 GCC Develop
                                                        *#NO_APP
        .file   "adler32.c"                             *       .file   "adler32.c"
        .text                                           *       .text
        .globl ___umodsi3                               *       .globl  __umodsi3
        .globl ___modsi3                                *       .globl  __modsi3
        .globl ___mulsi3                                *       .globl  __mulsi3
        .align  2                                       *       .align  2
                                                        *       .type   adler32_combine_, @function
_adler32_combine_:                                      *adler32_combine_:
        movem.l d3/d4/d5/d6/d7/a3,-(sp)                 *       movem.l #7952,-(%sp)
        move.l 28(sp),d3                                *       move.l 28(%sp),%d3
        move.l 32(sp),d6                                *       move.l 32(%sp),%d6
        move.l 36(sp),d0                                *       move.l 36(%sp),%d0
        jbmi _?L6                                       *       jmi .L6
        lea ___umodsi3,a3                               *       lea __umodsi3,%a3
        move.l #65521,-(sp)                             *       move.l #65521,-(%sp)
        move.l d0,-(sp)                                 *       move.l %d0,-(%sp)
        jbsr (a3)                                       *       jsr (%a3)
        addq.l #8,sp                                    *       addq.l #8,%sp
        move.l d0,d5                                    *       move.l %d0,%d5
        move.l d3,d7                                    *       move.l %d3,%d7
        and.l #65535,d7                                 *       and.l #65535,%d7
        move.l d7,-(sp)                                 *       move.l %d7,-(%sp)
        move.l d0,-(sp)                                 *       move.l %d0,-(%sp)
```

The GAS format sometimes uses a notation called MIT syntax (right).
HAS.X format (left) translates to Motorola syntax.
```
                                    * .type __mulsi3,function
 .globl ___mulsi3                   * .globl __mulsi3
___mulsi3:                          *__mulsi3:
 move.w 4(sp),d0                    * movew %sp@(4), %d0
 mulu.w 10(sp),d0                   * muluw %sp@(10), %d0
 move.w 6(sp),d1                    * movew %sp@(6), %d1
 mulu.w 8(sp),d1                    * muluw %sp@(8), %d1
                                    *
 add.w d1,d0                        * addw %d1, %d0
                                    *
                                    *
                                    *
 swap d0                            * swap %d0
 clr.w d0                           * clrw %d0
 move.w 6(sp),d1                    * movew %sp@(6), %d1
 mulu.w 10(sp),d1                   * muluw %sp@(10), %d1
 add.l d1,d0                        * addl %d1, %d0
                                    *
 rts                                * rts
```

## Types and uses of runtime libraries

The gcc runtime library is placed under the 
directory xdev68k/lib/m68k_elf.
You need to link libgcc.a for C language based projects 
and libgcc.a and libstdc++.a for C++ based projects.

>:warning:
>Be sure to use the runtime library under xdev68k/lib/m68k_elf.
>There are files with the same names under >xdev68k/m68k-toolchain, but they cannot be linked with X68K object files.
>The runtime library included in the conventional X68K ported gcc can also be linked, but there is no guarantee of operation due to incompatibility.

There are multiple types of runtime libraries, select and use the one that matches the build settings of your application.

* xdev68k/lib/m68k_elf/m68000/*.a  
	Consists of the MC68000 instruction set.
	Link when creating an executable file that can run on all generations of X680x0.
	Also link here for X68030 environments without FPU.

* xdev68k/lib/m68k_elf/m68020/*.a  
	It consists of MC68020 instruction set + FPU's MC68881 instruction set.
	Link when creating an executable file that can run on the X68030 with FPU.
	Please note that it does not work on X68030 without FPU.
	Also, please note that there is no guarantee of operation in the environment of MC68040 / MC68060, etc., as it may include floating point 
	arithmetic instructions (FMOVECR, etc.) that do not exist in the built-in FPU of MC68040 and later.

* xdev68k/lib/m68k_elf/m68040/*.a  
	Consists of the MC68040 instruction set.
	Link to create an executable file that can run on X680x0 with 68040 accelerator.

* xdev68k/lib/m68k_elf/m68060/*.a  
	Consists of the MC68060 instruction set.
	Link to create an executable file that can run on X680x0 with 68060 accelerator.


## Runtime library rebuild procedure

The runtime library is already built and included under xdev68k/lib in this repository, 
so there is no need for users to build it themselves.
If for some reason you need to rebuild, go to the directory xdev68k on your host's 
bash console and run:
```bash
./build_x68k-libgcc.sh -m68000 -m68020 -m68040 -m68060  
./build_x68k-libstdc++.sh -m68000 -m68020 -m68040 -m68060  
```
A successful build will print the following to the console:
```bash
The building process is completed successfully.
```
The directories build_libgcc/ and build_libstdc++/ are intermediate files.
There is no problem even if you delete it after the build is completed.


## Compatibility issues with legacy X68K-aware compilers

There is a compatibility issue between the previous X68K compatible compilers (SHARP C Compiler PRO-68K and gcc Mariko version) and the latest m68k-elf-gcc.  
*Compatibility with gcc2 Charlie version, gcc2.95.2, etc. has not been verified.

### 1. ABI mismatch
ABI is an abbreviation for Application Binary Interface, and defines the arrangement 
of data types in memory and the rules for passing arguments and return values when calling functions.
There is an ABI mismatch between his legacy X68K capable compiler and the latest m68k-elf-gcc.
This is a problem when linking binaries created with older compilers without recompiling.

* Destruction register difference (avoidable)  
	The destruction register at the time of function call is different between the conventional X68K compatible compiler and the latest m68k-elf-gcc.
	```
	SHARP C Compiler PRO-68K, gcc Mariko version : d0-d2/a0-a2/fp0-fp1  
	m68k-elf-gcc                           		 : d0-d1/a0-a1/fp0-fp1  
	```
	This problem can be solved by specifying compile options -fcall-used-d2 -fcall-used-a2 on the m68k-elf-gcc side.

* Differences in registers that store return values (avoidable)  
	The X68K ABI followed the convention of MC680x0, with the rule that the return value of a function should be stored in the d0 register.
	On the other hand, in modern gcc, some configures also store the return value in the a0 register.
	The idea is that for functions that obviously return a pointer, such as malloc(), 
	the overhead can be avoided by returning the return value in the address register.
	However, in practice, the operation is such that the same value is returned to both a0 and d0 for 
	safety and compatibility, which is actually a source of overhead.
	And the generation of code that reads the result from the a0 register makes it impossible to reuse past software assets.
	
	To avoid this problem, you should use a gcc built with configure that stores function return values only in the d0 register.
	As shown in "Environment building procedure", there is no problem if build_m68k-toolchain.sh is used. (like gcc), be careful.

* Endian difference of long long type (unavoidable)  
	The binary representation of the long long type, which is a 64-bit integer type, 
	differs between the conventional X68K compatible compiler and m68k-elf-gcc.
	```
	SHARP C Compiler PRO-68K, gcc Mariko version: Stored in the order of lower 32 bits and upper 32 bits (that is, not in big endian arrangement)  
	m68k-elf-gcc                           		: Stored in the order of upper 32 bits and lower 32 bits (strictly big-endian arrangement)  
	```
	There is currently no workaround for this issue.
	(Fortunately, the long long type rarely appears in past software assets, and rarely develops into problems.
	At least it doesn't appear in the SHARP C Compiler PRO-68K header. )

* Difference in bit width of long double type (unavoidable)  
	The binary representation of the long double type, which is an extended double-precision floating point type, 
	is different between the conventional X68K compatible compiler and m68k-elf-gcc.
	```
	SHARP C Compiler PRO-68K, gcc Mariko version	: long double = 8 bytes type (double type compatible)  
	m68k-elf-gcc                           			: long double = 12 bytes type 
	* The gcc2 Charlie version is also 12 bytes type.  
	```
	There is currently no workaround for this issue.
	(Fortunately, the long double type rarely appears in past software assets, and rarely develops into problems.
	At least it doesn't appear in the SHARP C Compiler PRO-68K header. )

### 2. Different behavior when casting NaN Inf values

In the latest m68k-elf-gcc, the IEEE754 implementation for non-coprocessor environments 
is not fully compliant with the IEEE754 specification because it doesn't handle NaN and Inf.

A typical example of encountering this problem is a float to double conversion, 
where converting a NaN or Inf float value to a double results in an incorrect finite value.
This conversion is done by a function called __extendsfdf2 in the libgcc source fpgnulib.c.
The float to double conversion of NaN or Inf is correct when resolved at compile time because 
it is handled in a way that matches the IEEE754 specification inside the compiler, 
but is incorrect when resolved at run time because it is handled with __extendsfdf2. result.
Whether or not it is resolved at compile time depends on gcc's mood (whether or not inlined 
functions are actually inlined, etc.), so it is impossible to predict the result with certainty.

When pushing arguments onto the stack in printf format, the compiler converts floats to double values.
So we often run into this problem when we want to check float values with printf.
As an example, the result of generating NaN Inf on the latest m68k-elf-gcc and outputting it with the printf function is shown.
```
Inf (generated by calculating 1.0f/0.0f)
	Binary representation: 0x7F800000 (correct as Inf of IEEE754)
	Output by printf: 340282366920940000000000000000000000000.000000 (incorrect)
NaN (generated by computing 0.0f/0.0f)
	Binary representation: 0xFFFFFFFF (correct for IEEE754 NaN)
	Output by printf: -680564693277060000000000000000000000000.000000 (incorrect)
```

This problem did not occur with the conventional X68K compatible compiler (SHARP C Compiler PRO-68K, gcc Mariko version).
Here is the result of compiling and executing the same code with gcc Mariko version.

```
Inf (generated by calculating 1.0f/0.0f)
	Binary representation: 0x7FFFFFFF
	Output by printf : #NAN.000000 (correct)
NaN (generated by computing 0.0f/0.0f)
	Binary representation: 0x7FFFFFFF
	Output by printf : #NAN.000000 (correct)
```

## C/C++ standard header conflict issue
If the header file specified by include is found on the search path specified by the -I option to the compiler, 
that file will be included, and if not found, the file that exists on m68k-toolchain will be included.
As an example, if you specify the path of SHARP C Compiler PRO-68K (XC) with the -I option, like the 
sample code in xdev68k/example/, the behavior will be as follows.
```C
*Good example
#include <stdbool.h>	// m68k-toolchain side is read because it is not included in XC
#include <stdint.h>		// m68k-toolchain side is read because it is not included in XC
#include <stdlib.h> 	// XC side is read
#include <stdio.h> 		// XC side is read
```
This behavior is useful for partially including new generation C standard header files.
On the other hand, however, this causes confusion and conflicts between the C/C++ standard 
header files derived from old software assets and the new generation C/C++ standard 
header files existing on the m68k-toolchain, causing problems. may be.
```C
*bad example
#include <iostream> // Since it is not included in XC, the m68k-toolchain side is read.
                    // some headers included by iostream are read from XC side (confusing).
                    // As a result, a large number of compilation errors are output.
```
To work around this problem, 
you need C standard headers and libraries (like modern glibc, BSD lib or newlib) 
that are in line with the m68k-elf-gcc generation.
They are currently undeveloped and there is no workaround for this issue.



## Recommended usage style

In summary, the recommended usage style of m68k-elf-gcc is as follows.

1. Use m68k-elf-gcc built by yourself with build_m68k-toolchain.sh.
2. Specify -fcall-used-d2 -fcall-used-a2 on the m68k-elf-gcc side.
3. When using past assets without recompiling, limit them to those that do not include long long type and long double type.
4. When casting NaN or Inf to another type, consider the possibility of incorrect handling.
5. The use of C/C++ standard header files is limited to those that do not cause conflicts between old generation headers and new generation headers.


## Use library environment other than SHARP C Compiler PRO-68K

install_xdev68k-utils.sh installs SHARP C Compiler PRO-68K as the library environment, 
but this is just the default library environment of xdev68k and can be replaced by the user.
The library environment can be replaced simply by changing the header search path given to the 
compiler and the library file specification at linking.
Using the xdev68k/example/ sample code as an example, 
do the following in the makefile.
```bash
# header search path
INCLUDE_FLAGS = (Write the replacement header search path after -I here)

(Omitted)

# library file to be linked
LIBS =\
	(Write the replacement library group here) \
	${XDEV68K_DIR}/lib/m68k_elf/m68000/libgcc.a \
	${XDEV68K_DIR}/lib/m68k_elf/m68000/libstdc++.a \
```


## Compilation speed with xdev68k

### 1. Differences in compilation speed depending on the host environment
Compilation speed with xdev68k depends on the host Unix-compatible environment.
Comparing the compilation speed of each environment, the general trends are as follows.
```bash
linux native >>> WSL (on linux path) > msys == cygwin >> WSL (on windows path)
* Compilation speed on mac is not evaluated
```

### 2. Speed up by parallel compilation
Compilation times with xdev68k are dramatically reduced by running make in parallel.
Parallel execution is possible simply by specifying the degree of parallelism with the -j option to the make command.
For example, in a host environment with 8 physical cores, you should run:
```bash
# It is generally said that specifying the number of physical cores + 1 for parallel compilation with CPU 8 cores 
# is the fastest, but here the number of physical cores is used as it is in order to avoid the PC from operating 
# erratically during compilation.
# Adjust accordingly to your liking. 
make -j8
```
If you specify make command options in the environment variable MAKEFLAGS, you do not need to give them as command line arguments each time.
Also, the number of physical cores in the host environment can be collected from /proc/cpuinfo in a Unix compatible environment.
Therefore, by putting the following description in bashrc (for msys, C:/msys64/home/username/.bashrc), 
you can automatically set the degree of parallelism for make.
```bash
# Determine the degree of parallelism of make from the number of physical cores in the execution environment
export MAKEFLAGS=-j$(($(grep cpu.cores /proc/cpuinfo | sort -u | sed 's/[^0-9]//g')))
```

There is one point of caution here.
The X68K command line emulator, run68, may not work properly when running in parallel.
The sample code from xdev68k/example/ does the following to avoid this problem:
```bash
ATOMIC = perl ${XDEV68K_DIR}/util/atomic.pl
RUN68 = $(ATOMIC) ${XDEV68K_DIR}/run68/run68
HAS = $(RUN68) ${XDEV68K_DIR}/x68k_bin/HAS060.X
HLK = $(RUN68) ${XDEV68K_DIR}/x68k_bin/hlk301.x
```
\$(ATOMIC) above is a simple script that single-threads the specified command line.
Make sure to run X68K commands such as HAS and HLK on run68 via $(ATOMIC).
By going through \$(ATOMIC), unfortunately only X68K commands will not be able to benefit from the speedup of parallel compilation.
However, the speedup from parallel compilation is still dramatic and well worth taking advantage of.


## Debug execution linked with XEiJ

XEiJ https://stdkmd.net/xeij/ is an open source X680x0 emulator created by M.Kamada.
It is based on the Java language, so it can run on various platforms.
XEiJ can use any directory on the host machine as the boot drive for Human68k.
In the windows environment, using a dedicated named pipe allows X68K 
under emulation to execute arbitrary commands.
Debugging using XEiJ behaves like remote execution in a general cross-development environment.
By using the function called "terminal window", it is possible to output debug log etc. 
on an independent window instead of the main screen of X68K.

>:warning:
>Linking with XEiJ is currently not available outside of msys.

### State of debugging

This is a screenshot of the sample code xdev68k/example/run_xeij running.
The upper left is the screen of the program to be debugged, 
and the lower left is the terminal window.
The right side of the screen is a group of windows that display the analysis information of the running program.

![debug_with_xeij](https://user-images.githubusercontent.com/11882108/230971524-56ba9039-d11c-4903-8738-68f743fbabe2.png)


### Environmental setting

In order to debug the result built with xdev68k with XEiJ, it is necessary to set up the environment as follows.

1. Install XEiJ (0.23.04.10 or newer)
	Download the XEiJ archive from the official site and extract it. 
	https://stdkmd.net/xeij/#download  
	Please refer to the official website for the environment settings of XEiJ.  
	https://stdkmd.net/xeij/environment.htm  


2. Create a Human68k boot drive
	X68000 LIBRARY From http://retropc.net/x68000/
	"Human68k version 3.02 system disk (HUMAN302.LZH)" http://retropc.net/x68000/software/sharp/human302/
	Download and extract.
	The name of the destination directory can be anything, but here we will use xeij_boot for the sake of explanation.
	```
	xeij_boot/ (Hereafter, files obtained by expanding HUMAN302.LZH)
	├ ASK/
	├ BASIC2/
	├ BIN/
	omission
	├ AUTOECEC.BAT
	Omitted below
	```
	Set the environment variable XEIJ_BOOT_DIR to the full path of the directory xeij_boot.
	Specify the full path in unix style starting with /c, 
	not windows style starting with drive name such as C:.
	For msys, execute as follows in .bashrc.
	```bash
	export XEIJ_BOOT_DIR=Full path of directory xeij_boot
	```

	Next, incorporate TwentyOne.x in order to relax the Human68k 
	file name character limit (8+3 characters).
	Download tw136c14.lzh from X68000 LIBRARY http://retropc.net/x68000/software/disk/filename/twentyone/, 
	expand it under the directory xeij_boot, 
	and add the following line to xeij_boot/CONFIG.SYS .	
	```
	DEVICE = \tw136c14\TwentyOne.x +TPS
	```
	After restarting X68K on XEiJ the file name limit is extended to 18+3 characters.


3. Place directory xdev68k under Human68k boot drive
	In order for xdev68k related files to be visible from X68K, 
	the directory xdev68k must be placed under the directory xeij_boot.
	```
	xeij_boot/ 
	├ xdev68k/ (← Placed here)
	├ ASK/
	├ BASIC2/
	├ BIN/
	omission
	├ AUTOECEC.BAT
	Omitted below
	```

4. Start XEiJ  
	To start XEiJ with the directory xeij_boot as the Human68k boot drive, 
	execute the following from the command line of the host machine.
	```
	java -jar XEiJ.jar -boot=xeij_boot
	```

	After starting XEiJ, open the terminal window by selecting “Settings” → “Terminal” on the main window of XEiJ.
	This window can be used as a log output window for debugging.
	There are many other windows that are useful for debugging, so keep them open.


5. Setting up XEiJ
	Enable XEiJ's paste pipe functionality to allow XEiJ to be controlled from the make command.

	![xeij_settings](https://user-images.githubusercontent.com/11882108/231141913-1ba22a68-295d-4057-8f52-93811865ef07.png)


6. Build & Debug Run & Interrupt on xdev68k 
	Debug and run the xdev68k sample code as a test.
	Start XEiJ, confirm that the X68K side is waiting for command line input, 
	move to the directory of the sample code xdev68k/example/run_xeij on a console such as msys, and execute the following command .
	```
	make run_xeij
	```
	If the environment construction up to this point is successful, the sample code will be executed on XEiJ.
	To break debugging, press Enter on the console of the host environment (such as msys).
	When interrupted, the current register information, the program counter, 
	and the program code at the location pointed to by the program counter are output to the terminal window.


### makefile description

Debug execution with XEiJ is possible by adding the following description to the makefile.
```
run_xeij : executable file name
	bash ${XDEV68K_DIR}/util/xeij_remote_debug.sh Executable file name Argument
```
Please refer to the implementation of xdev68k/util/xeij_remote_debug.sh, 
as detailed processing contents are not explained here.


### important point

Program interruption during debugging is forced by software triggering the interrupt switch input.
For this reason, in the case of a program that uses interrupt processing, 
the program will be terminated without stopping the interrupt, 
and subsequent system operation will become unstable.

Regarding VSYNC interrupts and raster interrupts, which are often used in games, 
the state is saved and restored at the start and end of debugging on the xdev68k side 
(xdev68k/util/xeij_remote_debug.sh), so the above problem does not occur.

## troubleshooting

Here are some problems that xdev68k users often encounter and how to solve them.

* malloc of large memory region fails 
	When using XC, the default memory size that can be secured is up to 64K bytes.
	allmem() must be run to expand this to the maximum size available on the hardware.
	See example/heap_size for details.

* IRTE() is undefined when compiling old generation code  
	The way interrupt functions are written has changed in the current gcc.
	See example/interrupt for details.

* Error when resuming from supervisor mode  
	In the current gcc, due to the fact that optimization such as stack batch correction 
	is actively performed, stack pointer consistency may not be maintained when switching between user mode and supervisor mode.
	Please refer to example/b_super for details such as workarounds.


## Limitations

Currently xdev68k has many limitations.

* Lack of standard C library 
	The SHARP C Compiler PRO-68K ver2.1 installed by xdev68k as the default library environment is from around 1990, 
	and the standard C library included here is quite different from the current C standard specifications and styles.
	To fix this problem, you need to port something like the latest glibc, BSD lib or newlib.

* The function to link with the IDE environment is not developed  
	The mechanism for linking xdev68k with IDE environments such as visual studio and Eclipse has not yet been developed.

*Limited C++ support 
	When using SHARP C Compiler PRO-68K ver2.1, the default constructor/destructor is not automatically 
	executed in the startup routine, so the application has to do it by itself.
	Compiling C++ code that uses exceptions and RTTI fails to assemble because x68k_gas2has.pl outputs unsupported GAS directives.
	To disable exceptions and RTTI, you need to specify the following compile options:
	```bash
	-fno-rtti
	-fno-exceptions
	```

* GAS format assembler code only supports the format output by gcc  
	x68k_gas2has.pl can recognize only the formats that gcc can output among the methods of writing GAS format assembler code.

* Restrictions on assembler code that can be written in inline asm  
	x68k_gas2has.pl does not support all specifications of macro control instructions (such as macro local endm exitm rept irp irpc of HAS).
	Special symbols ('&' '!' , '<'~'>' , '%' etc. of HAS) will cause parse errors.

* Not supported to output debug information 
	GAS format → HAS format conversion of debug information is not supported.
	Due to this restriction, the generated executable file cannot support source code viewing on the debugger.

## Under rave test

Currently, we are conducting operation tests under various conditions.
Corrections are being made frequently.
Please note that for the time being, it is expected that compatibility breaks will occur without notice due to corrections.

The toolchain is updated infrequently, but the utilities and sample code are frequently updated.
If you run into any problems, getting the latest version and reinstalling the utilities (running install_xdev68k-utils.sh) may help.
If you just reinstall the utility, it will be completed in a few minutes, so we recommend that you do it regularly.

If you encounter any kind of problem, such as an error when building the 
environment or an error during assembler source conversion, it would be helpful if you 
could report it with information such as the situation and reproduction code.
(Although I may not have time to deal with all the problems.)


## Acknowledgments

xdev68k utilizes freely available Sharp software, the Free Software Foundation's gcc and other toolchains, 
and software assets created by X68K users.
We would like to thank the companies, organizations, 
and volunteers who have created and published the software.

In addition, many of the X68K related software assets used in xdev68k are 
downloaded from the X68000 LIBRARY http://retropc.net/x68000/.
He is also the author of HAS060 and his XEiJ, and maintains the archive. 
I would like to thank M.Kamada, the manager of the X68000 LIBRARY.

In the *.lhz archive extraction process of install_xdev68k-utils.sh,
I am using LHa for UNIX with Autoconf https://github.com/jca02266/lha/.
I would like to thank Mr. Tsugio Okamoto, the original author of LHa for UNIX, and Mr. 
Koji Arai, the author of LHa for UNIX with Autoconf.


## License

* libgcc.a and libstdc++.a
GNU GENERAL PUBLIC LICENSE Version 3 and GCC RUNTIME LIBRARY EXCEPTION Version 3.1 apply.
(When distributing an archive file as a single binary, the GPL applies, so it is necessary to disclose the source code or the means of obtaining it.
If the archive file is used by linking with the application, the GPL will not be propagated to the application and there will be no obligation to disclose the source. )

* Each file downloaded or installed by install_xdev68k-utils.sh has its own license and distribution terms.
See xdev68k/license/readme.txt for details.

* Other than those above
Apache License Version 2.0 applies.
(It is a license with loose restrictions equivalent to the MIT license, 
and it is an excellent license that prevents open source users from being exposed to 
unexpected risks by deterring patent acquisition by third parties like patent trolls.) license.)
