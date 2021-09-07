#  ============================================================================
#  Author:       Jonathan Edwards
#  Date:         21/07/2021 (V2)
#  Description:  Compiles and links all object files to create an exe output 
#                app for Windows.
#  ============================================================================



# App Name
TARGET = TerrainPatches

# Definitions
CC = g++ $(INCLUDES)
CFLAGS = -g -Wall -std=c++11
INCLUDES = -I./include
OPENGL_LIB = -lOpenGL32 -lGLU32 -lfreeGLUT -lGLEW32

# Generating Object Name
SOURCES_LOCATION  = ./src
SOURCES = $(wildcard $(SOURCES_LOCATION)/*.cpp)
OBJECTS = $(patsubst $(SOURCES_LOCATION)/%.cpp, %.o, $(SOURCES))


# Pretty Printing
PRINT_BEGIN      = ======================================================================================================================
PRINT_END        = ====================================================================================================================== & echo. & echo.
REMOVE_MESSAGE   = --------------------------------------- Removing all objects and executables -----------------------------------------
LINK_NAN_MESSAGE = ----------------------------------------------- Creating object files ------------------------------------------------
LINK_OBJ_MESSAGE = -------------------------------------- Linkng object files to create executable ---------------------------------------
COMPLETE_MESSAGE = Succesfully Compiled Program...



# Default target
all: clean $(TARGET)
	@echo $(COMPLETE_MESSAGE)


# Compile: create object files from C++ source files
%.o: $(SOURCES_LOCATION)/%.cpp
	$(CC) -c -o build/$@ $< $(CFLAGS)


# Link: create exe output file from object files
$(TARGET): $(OBJECTS)
	@echo $(PRINT_END)
	@echo $(LINK_OBJ_MESSAGE)
	@echo $(PRINT_BEGIN)
	cd build && \
	$(CC) -o ../$@ $^ $(OPENGL_LIB) 
	@echo $(PRINT_END)
	

# Target: Clean Project
.PHONY: CleanProject
clean:
	@echo $(REMOVE_MESSAGE)
	@echo $(PRINT_BEGIN)
	del /f build\*.o $(TARGET).exe
	@echo $(PRINT_END)
	@echo $(LINK_NAN_MESSAGE)
	@echo $(PRINT_BEGIN)

program: 
	start "" $(TARGET).exe
	
