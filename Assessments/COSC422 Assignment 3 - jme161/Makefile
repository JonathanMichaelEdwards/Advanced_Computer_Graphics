# ============================================================================================
# 
#  Author:       Jonathan Edwards
#  Date:         08/09/2021 (V5)
# 
#  Description:  Compiles and links all object files to create an output 
#                app for Linux.
#  ============================================================================================


# App Name
TARGET = main.out

# Definitions
CC = g++
DEBUG = -g
OPT = -O2
CFLAGS = -c $(DEBUG) -Wall -std=c++11
EXE = -no-pie# changes (x-libshare -> x-executable)

# Libraries & Includes
INCLUDES = -I./include -I./usr/include/ -I./usr/local/include/
OPENGL_LIB = -lGL -lGLU -lGLEW -lglut
DEVIL = -lIL -lILU -lILUT
ASSIMP = -lassimp
CC_LIBS = -lpthread

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
	$(CC) $(CFLAGS) $(OPT) -o build/$@ $< $(INCLUDES)


# Link: create output file from object files
$(TARGET): $(OBJECTS)
	@echo $(PRINT_END)
	@echo $(LINK_OBJ_MESSAGE)
	@echo $(PRINT_BEGIN)
	$(CC) $(EXE) $(OPT) build/*.o -o $@ $(CC_LIBS) $(OPENGL_LIB) $(DEVIL) $(ASSIMP)
	@echo $(PRINT_END)
	

# Target: Clean Project.
.PHONY: cleanProject
clean:
	@echo $(REMOVE_MESSAGE)
	@echo $(PRINT_BEGIN)
	-rm -f build/*.o $(TARGET)
	@echo $(PRINT_END)
	@echo $(LINK_NAN_MESSAGE)
	@echo $(PRINT_BEGIN)


program: 
	./$(TARGET)

