# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/douglas/.local/lib/python3.10/site-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /home/douglas/.local/lib/python3.10/site-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/douglas/desktop/TCC/imu/xbow440/serial

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/douglas/desktop/TCC/imu/xbow440/serial/build

# Utility rule file for run_tests_serial.

# Include any custom commands dependencies for this target.
include tests/CMakeFiles/run_tests_serial.dir/compiler_depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/run_tests_serial.dir/progress.make

run_tests_serial: tests/CMakeFiles/run_tests_serial.dir/build.make
.PHONY : run_tests_serial

# Rule to build all files generated by this target.
tests/CMakeFiles/run_tests_serial.dir/build: run_tests_serial
.PHONY : tests/CMakeFiles/run_tests_serial.dir/build

tests/CMakeFiles/run_tests_serial.dir/clean:
	cd /home/douglas/desktop/TCC/imu/xbow440/serial/build/tests && $(CMAKE_COMMAND) -P CMakeFiles/run_tests_serial.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/run_tests_serial.dir/clean

tests/CMakeFiles/run_tests_serial.dir/depend:
	cd /home/douglas/desktop/TCC/imu/xbow440/serial/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/douglas/desktop/TCC/imu/xbow440/serial /home/douglas/desktop/TCC/imu/xbow440/serial/tests /home/douglas/desktop/TCC/imu/xbow440/serial/build /home/douglas/desktop/TCC/imu/xbow440/serial/build/tests /home/douglas/desktop/TCC/imu/xbow440/serial/build/tests/CMakeFiles/run_tests_serial.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/run_tests_serial.dir/depend

