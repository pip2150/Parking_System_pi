# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/project/Parking_System_pi

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/project/Parking_System_pi

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/pi/project/Parking_System_pi/CMakeFiles /home/pi/project/Parking_System_pi/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/pi/project/Parking_System_pi/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named main

# Build rule for target.
main: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 main
.PHONY : main

# fast build rule for target.
main/fast:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/build
.PHONY : main/fast

Network/Socket.o: Network/Socket.cpp.o

.PHONY : Network/Socket.o

# target to build an object file
Network/Socket.cpp.o:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Network/Socket.cpp.o
.PHONY : Network/Socket.cpp.o

Network/Socket.i: Network/Socket.cpp.i

.PHONY : Network/Socket.i

# target to preprocess a source file
Network/Socket.cpp.i:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Network/Socket.cpp.i
.PHONY : Network/Socket.cpp.i

Network/Socket.s: Network/Socket.cpp.s

.PHONY : Network/Socket.s

# target to generate assembly for a file
Network/Socket.cpp.s:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Network/Socket.cpp.s
.PHONY : Network/Socket.cpp.s

Network/http.o: Network/http.cpp.o

.PHONY : Network/http.o

# target to build an object file
Network/http.cpp.o:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Network/http.cpp.o
.PHONY : Network/http.cpp.o

Network/http.i: Network/http.cpp.i

.PHONY : Network/http.i

# target to preprocess a source file
Network/http.cpp.i:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Network/http.cpp.i
.PHONY : Network/http.cpp.i

Network/http.s: Network/http.cpp.s

.PHONY : Network/http.s

# target to generate assembly for a file
Network/http.cpp.s:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Network/http.cpp.s
.PHONY : Network/http.cpp.s

Opencv/OCR.o: Opencv/OCR.cpp.o

.PHONY : Opencv/OCR.o

# target to build an object file
Opencv/OCR.cpp.o:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Opencv/OCR.cpp.o
.PHONY : Opencv/OCR.cpp.o

Opencv/OCR.i: Opencv/OCR.cpp.i

.PHONY : Opencv/OCR.i

# target to preprocess a source file
Opencv/OCR.cpp.i:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Opencv/OCR.cpp.i
.PHONY : Opencv/OCR.cpp.i

Opencv/OCR.s: Opencv/OCR.cpp.s

.PHONY : Opencv/OCR.s

# target to generate assembly for a file
Opencv/OCR.cpp.s:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Opencv/OCR.cpp.s
.PHONY : Opencv/OCR.cpp.s

Opencv/Plate.o: Opencv/Plate.cpp.o

.PHONY : Opencv/Plate.o

# target to build an object file
Opencv/Plate.cpp.o:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Opencv/Plate.cpp.o
.PHONY : Opencv/Plate.cpp.o

Opencv/Plate.i: Opencv/Plate.cpp.i

.PHONY : Opencv/Plate.i

# target to preprocess a source file
Opencv/Plate.cpp.i:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Opencv/Plate.cpp.i
.PHONY : Opencv/Plate.cpp.i

Opencv/Plate.s: Opencv/Plate.cpp.s

.PHONY : Opencv/Plate.s

# target to generate assembly for a file
Opencv/Plate.cpp.s:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Opencv/Plate.cpp.s
.PHONY : Opencv/Plate.cpp.s

Opencv/Svm.o: Opencv/Svm.cpp.o

.PHONY : Opencv/Svm.o

# target to build an object file
Opencv/Svm.cpp.o:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Opencv/Svm.cpp.o
.PHONY : Opencv/Svm.cpp.o

Opencv/Svm.i: Opencv/Svm.cpp.i

.PHONY : Opencv/Svm.i

# target to preprocess a source file
Opencv/Svm.cpp.i:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Opencv/Svm.cpp.i
.PHONY : Opencv/Svm.cpp.i

Opencv/Svm.s: Opencv/Svm.cpp.s

.PHONY : Opencv/Svm.s

# target to generate assembly for a file
Opencv/Svm.cpp.s:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Opencv/Svm.cpp.s
.PHONY : Opencv/Svm.cpp.s

Opencv/Utils.o: Opencv/Utils.cpp.o

.PHONY : Opencv/Utils.o

# target to build an object file
Opencv/Utils.cpp.o:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Opencv/Utils.cpp.o
.PHONY : Opencv/Utils.cpp.o

Opencv/Utils.i: Opencv/Utils.cpp.i

.PHONY : Opencv/Utils.i

# target to preprocess a source file
Opencv/Utils.cpp.i:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Opencv/Utils.cpp.i
.PHONY : Opencv/Utils.cpp.i

Opencv/Utils.s: Opencv/Utils.cpp.s

.PHONY : Opencv/Utils.s

# target to generate assembly for a file
Opencv/Utils.cpp.s:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/Opencv/Utils.cpp.s
.PHONY : Opencv/Utils.cpp.s

main.o: main.cpp.o

.PHONY : main.o

# target to build an object file
main.cpp.o:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/main.cpp.o
.PHONY : main.cpp.o

main.i: main.cpp.i

.PHONY : main.i

# target to preprocess a source file
main.cpp.i:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/main.cpp.i
.PHONY : main.cpp.i

main.s: main.cpp.s

.PHONY : main.s

# target to generate assembly for a file
main.cpp.s:
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/main.cpp.s
.PHONY : main.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... main"
	@echo "... Network/Socket.o"
	@echo "... Network/Socket.i"
	@echo "... Network/Socket.s"
	@echo "... Network/http.o"
	@echo "... Network/http.i"
	@echo "... Network/http.s"
	@echo "... Opencv/OCR.o"
	@echo "... Opencv/OCR.i"
	@echo "... Opencv/OCR.s"
	@echo "... Opencv/Plate.o"
	@echo "... Opencv/Plate.i"
	@echo "... Opencv/Plate.s"
	@echo "... Opencv/Svm.o"
	@echo "... Opencv/Svm.i"
	@echo "... Opencv/Svm.s"
	@echo "... Opencv/Utils.o"
	@echo "... Opencv/Utils.i"
	@echo "... Opencv/Utils.s"
	@echo "... main.o"
	@echo "... main.i"
	@echo "... main.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

