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
CMAKE_SOURCE_DIR = /home/moos-ivp-douglas

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/moos-ivp-douglas/build

# Include any dependencies generated for this target.
include src/pExampleApp/CMakeFiles/pExampleApp.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/pExampleApp/CMakeFiles/pExampleApp.dir/compiler_depend.make

# Include the progress variables for this target.
include src/pExampleApp/CMakeFiles/pExampleApp.dir/progress.make

# Include the compile flags for this target's objects.
include src/pExampleApp/CMakeFiles/pExampleApp.dir/flags.make

src/pExampleApp/CMakeFiles/pExampleApp.dir/main.cpp.o: src/pExampleApp/CMakeFiles/pExampleApp.dir/flags.make
src/pExampleApp/CMakeFiles/pExampleApp.dir/main.cpp.o: /home/moos-ivp-douglas/src/pExampleApp/main.cpp
src/pExampleApp/CMakeFiles/pExampleApp.dir/main.cpp.o: src/pExampleApp/CMakeFiles/pExampleApp.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/moos-ivp-douglas/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/pExampleApp/CMakeFiles/pExampleApp.dir/main.cpp.o"
	cd /home/moos-ivp-douglas/build/src/pExampleApp && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/pExampleApp/CMakeFiles/pExampleApp.dir/main.cpp.o -MF CMakeFiles/pExampleApp.dir/main.cpp.o.d -o CMakeFiles/pExampleApp.dir/main.cpp.o -c /home/moos-ivp-douglas/src/pExampleApp/main.cpp

src/pExampleApp/CMakeFiles/pExampleApp.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/pExampleApp.dir/main.cpp.i"
	cd /home/moos-ivp-douglas/build/src/pExampleApp && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/moos-ivp-douglas/src/pExampleApp/main.cpp > CMakeFiles/pExampleApp.dir/main.cpp.i

src/pExampleApp/CMakeFiles/pExampleApp.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/pExampleApp.dir/main.cpp.s"
	cd /home/moos-ivp-douglas/build/src/pExampleApp && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/moos-ivp-douglas/src/pExampleApp/main.cpp -o CMakeFiles/pExampleApp.dir/main.cpp.s

# Object files for target pExampleApp
pExampleApp_OBJECTS = \
"CMakeFiles/pExampleApp.dir/main.cpp.o"

# External object files for target pExampleApp
pExampleApp_EXTERNAL_OBJECTS =

/home/moos-ivp-douglas/bin/pExampleApp: src/pExampleApp/CMakeFiles/pExampleApp.dir/main.cpp.o
/home/moos-ivp-douglas/bin/pExampleApp: src/pExampleApp/CMakeFiles/pExampleApp.dir/build.make
/home/moos-ivp-douglas/bin/pExampleApp: /home/moos-ivp/build/MOOS/MOOSCore/lib/libMOOS.a
/home/moos-ivp-douglas/bin/pExampleApp: src/pExampleApp/CMakeFiles/pExampleApp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/moos-ivp-douglas/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/moos-ivp-douglas/bin/pExampleApp"
	cd /home/moos-ivp-douglas/build/src/pExampleApp && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/pExampleApp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/pExampleApp/CMakeFiles/pExampleApp.dir/build: /home/moos-ivp-douglas/bin/pExampleApp
.PHONY : src/pExampleApp/CMakeFiles/pExampleApp.dir/build

src/pExampleApp/CMakeFiles/pExampleApp.dir/clean:
	cd /home/moos-ivp-douglas/build/src/pExampleApp && $(CMAKE_COMMAND) -P CMakeFiles/pExampleApp.dir/cmake_clean.cmake
.PHONY : src/pExampleApp/CMakeFiles/pExampleApp.dir/clean

src/pExampleApp/CMakeFiles/pExampleApp.dir/depend:
	cd /home/moos-ivp-douglas/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/moos-ivp-douglas /home/moos-ivp-douglas/src/pExampleApp /home/moos-ivp-douglas/build /home/moos-ivp-douglas/build/src/pExampleApp /home/moos-ivp-douglas/build/src/pExampleApp/CMakeFiles/pExampleApp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/pExampleApp/CMakeFiles/pExampleApp.dir/depend

