#  ============================================================================
#  Author:       Jonathan Edwards
#  Date:         02/05/2020 (V1)
#  Description:  Compiles and links all object files to create an exe output 
#                file for Windows.
#  ============================================================================



# App Name.
TARGET = main

# Definitions.
CC = g++ $(INCLUDES)
CFLAGS = -g -Wall -std=c++11
INCLUDES = -I./../include
OPENGL_LIB = -lpthread -lOpenGL32 -lGLU32 -lfreeGLUT -lGLEW32

# Getting Object Name Automatically.
SOURCES_LOCATION  = ../src
SOURCES = $(wildcard $(SOURCES_LOCATION)/*.cpp)
OBJECTS = $(patsubst $(SOURCES_LOCATION)/%.cpp, %.o, $(SOURCES))


# Printing pretty.
PRINT_BEGIN      = ======================================================================================================================
PRINT_END        = ====================================================================================================================== & echo. & echo.
REMOVE_MESSAGE   = -------------------------------------- Removing all object and executable files --------------------------------------
LINK_NAN_MESSAGE = ------------------------------------------ Creating non linked object files ------------------------------------------
LINK_OBJ_MESSAGE = -------------------------------- Linkng all object files to create an executable file --------------------------------
COMPLETE_MESSAGE = Succesfully Compiled Program...



# Default target.
all: clean $(TARGET)
	@echo $(COMPLETE_MESSAGE)


# Compile: create object files from C source files.
%.o: $(SOURCES_LOCATION)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)


# Link: create ELF output file from object files.
$(TARGET): $(OBJECTS)
	@echo $(PRINT_END)
	@echo $(LINK_OBJ_MESSAGE)
	@echo $(PRINT_BEGIN)
	$(CC) -o $@ $^ $(OPENGL_LIB) 
	@echo $(PRINT_END)
	

# Target: Clean Project.
.PHONY: cleanProject
clean:
	@echo $(REMOVE_MESSAGE)
	@echo $(PRINT_BEGIN)
	del /f *.o main.exe
	@echo $(PRINT_END)
	@echo $(LINK_NAN_MESSAGE)
	@echo $(PRINT_BEGIN)

run: 
	main.exe 0
