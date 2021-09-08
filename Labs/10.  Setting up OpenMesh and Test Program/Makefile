#  ============================================================================
#  Author:       Jonathan Edwards
#  Date:         21/07/2021 (V2)
#  Description:  Compiles and links all object files to create an exe output 
#                app for Windows.
#  ============================================================================



# App Name
TARGET = MeshViewer

# Definitions
CC = g++
CFLAGS = -c -g -Wall -std=c++11
INCLUDES = -I./include -I./usr/include/ -I./usr/local/include/
OPEN_MESH = -lOpenMeshCore -lOpenMeshTools
DEVIL = -lIL -lILU -lILUT
OPENGL_LIB = -lGL -lGLU -lGLEW -lglut $(DEVIL) $(OPEN_MESH)
exe = -no-pie  # changes (x-libshare -> x-executable)


# Generating Object Name
SOURCES_LOCATION  = ./src
SOURCES = $(wildcard $(SOURCES_LOCATION)/*.cpp)
OBJECTS = $(patsubst $(SOURCES_LOCATION)/%.cpp, %.o, $(SOURCES))


# Pretty Printing
PRINT_BEGIN      = "======================================================================================================================"
PRINT_END        = "======================================================================================================================\n\n"
REMOVE_MESSAGE   = "-------------------------------------- Removing all object and executable files --------------------------------------"
LINK_NAN_MESSAGE = "------------------------------------------ Creating non linked object files ------------------------------------------"
LINK_OBJ_MESSAGE = "-------------------------------- Linkng all object files to create an executable file --------------------------------"
COMPLETE_MESSAGE = Succesfully Compiled Program...



# Default target
all: clean $(TARGET)
	@echo $(COMPLETE_MESSAGE)


# Compile: create object files from C++ source files
%.o: $(SOURCES_LOCATION)/%.cpp
	$(CC) $(CFLAGS) -o build/$@ $< $(INCLUDES)


# Link: create exe output file from object files
$(TARGET): $(OBJECTS)
	@echo $(PRINT_END)
	@echo $(LINK_OBJ_MESSAGE)
	@echo $(PRINT_BEGIN)
	$(CC) $(exe) build/*.o -o $@ $(OPENGL_LIB) 
	@echo $(PRINT_END)
	

# Target: Clean Project.
.PHONY: cleanProject
clean:
	@echo $(REMOVE_MESSAGE)
	@echo $(PRINT_BEGIN)
	-rm -f build/*.o main
	@echo $(PRINT_END)
	@echo $(LINK_NAN_MESSAGE)
	@echo $(PRINT_BEGIN)

program: 
	./$(TARGET)
