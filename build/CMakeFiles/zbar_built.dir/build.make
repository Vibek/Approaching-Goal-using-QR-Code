# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/vibek/QRcode_detection

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/vibek/QRcode_detection/build

# Utility rule file for zbar_built.

# Include the progress variables for this target.
include CMakeFiles/zbar_built.dir/progress.make

CMakeFiles/zbar_built: lib/libzbar.so

lib/libzbar.so:
	$(CMAKE_COMMAND) -E cmake_progress_report /home/vibek/QRcode_detection/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating lib/libzbar.so"
	cd /home/vibek/QRcode_detection/3rdparty/zbar && make INSTALL_DIR=/home/vibek/QRcode_detection CFLAGS=-02

zbar_built: CMakeFiles/zbar_built
zbar_built: lib/libzbar.so
zbar_built: CMakeFiles/zbar_built.dir/build.make
.PHONY : zbar_built

# Rule to build all files generated by this target.
CMakeFiles/zbar_built.dir/build: zbar_built
.PHONY : CMakeFiles/zbar_built.dir/build

CMakeFiles/zbar_built.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/zbar_built.dir/cmake_clean.cmake
.PHONY : CMakeFiles/zbar_built.dir/clean

CMakeFiles/zbar_built.dir/depend:
	cd /home/vibek/QRcode_detection/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/vibek/QRcode_detection /home/vibek/QRcode_detection /home/vibek/QRcode_detection/build /home/vibek/QRcode_detection/build /home/vibek/QRcode_detection/build/CMakeFiles/zbar_built.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/zbar_built.dir/depend

