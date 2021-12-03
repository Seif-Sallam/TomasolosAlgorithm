CC=g++
SRC_INC_DIR= ./headers/
SRC_DIR= ./src/
SFML_INC_DIR= ./Thirdparty/SFML/include/
IMGUI_INC_DIR= ./Thirdparty/imgui/
IMGUI_DIALOG_DIR= ./Thirdparty/ImGuiDialog/ImGuiFileDialog/
LIB_DIR= ./Thirdparty/SFML/lib/
IMGUI_SRC_DIR= ./Thirdparty/imgui/
LIBS= -lGL -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
IMGUI_OBJECTS_DIR= ./imguiObjects/
IMGUI_OBJECTS = $(subst $(IMGUI_INC_DIR)imgui/, $(IMGUI_OBJECTS_DIR),  $(patsubst %.cpp, %.o, $(wildcard $(IMGUI_INC_DIR)imgui/*.cpp)))
IMGUI_DIALOG_OBJECTS = $(subst $(IMGUI_DIALOG_DIR), $(IMGUI_OBJECTS_DIR), $(patsubst %.cpp, %.o, $(wildcard $(IMGUI_DIALOG_DIR)*.cpp)))
OBJECTS_DIR = ./Objs/
OBJECTS = $(subst $(SRC_DIR), $(OBJECTS_DIR), $(patsubst %.cpp, %.o, $(wildcard $(SRC_DIR)*.cpp)) )
INC= -I$(SFML_INC_DIR) -I$(IMGUI_INC_DIR) -I$(IMGUI_DIALOG_DIR) -I$(IMGUI_INC_DIR)imgui/
CCFLAGS= $(INC)

all: | Dir Proj.out 

Dir:
	mkdir -p $(IMGUI_OBJECTS_DIR)
	mkdir -p $(OBJECTS_DIR)

Proj.out: $(OBJECTS) $(IMGUI_OBJECTS)  $(IMGUI_DIALOG_OBJECTS)
	$(CC) $(OBJECTS) $(IMGUI_OBJECTS) $(IMGUI_DIALOG_OBJECTS) -o Proj.out -L$(LIB_DIR) $(LIBS)

$(OBJECTS): $(OBJECTS_DIR)%.o : $(SRC_DIR)%.cpp	
	$(CC) -c $(CCFLAGS) $< -o $@

$(IMGUI_OBJECTS): $(IMGUI_OBJECTS_DIR)%.o : $(IMGUI_INC_DIR)imgui/%.cpp
	$(CC) -c $(CCFLAGS) $< -o $@

$(IMGUI_DIALOG_OBJECTS): $(IMGUI_OBJECTS_DIR)%.o : $(IMGUI_DIALOG_DIR)%.cpp
	$(CC) -c $(CCFLAGS) $< -o $@

.PHONY: clean

clean:
	$(RM) $(OBJECTS)

print:
	for obj in $(OBJECTS); do\
		echo $$obj;\
	done