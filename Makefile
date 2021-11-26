CC=g++
SRC_INC_DIR= ./headers/
SRC_DIR= ./src/
SFML_INC_DIR= ./Thirdpaty/SFML/include/
IMGUI_INC_DIR= ./Thirdpaty/imgui/
LIB_DIR= ./Thirdpaty/SFML/lib/
IMGUI_SRC_DIR= ./Thirdparty/imgui/
LIBS= -lGL -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
IMGUI_OBJECTS_DIR= ./imguiObjects/
IMGUI_OBJECTS= $(IMGUI_OBJECTS_DIR)imgui_demo.obj $(IMGUI_OBJECTS_DIR)imgui_draw.obj $(IMGUI_OBJECTS_DIR)imgui_tables.obj $(IMGUI_OBJECTS_DIR)imgui.obj $(IMGUI_OBJECTS_DIR)imgui-SFML.obj $(IMGUI_OBJECTS_DIR)imgui_widgets.obj 
IMGUI_DIALOG_DIR= ./Thirdparty/dir/ImGuiFileDialog/
IMGUI_DIALOG_OBJECTS= $(IMGUI_OBJECTS_DIR)ImGuiFileDialog.obj 
OBJECTS= main.obj ReservationStation.obj Application.obj Controller.obj Instruction.obj
CCFLAGS=-c -I$(SFML_INC_DIR) -I$(IMGUI_INC_DIR) -I$(IMGUI_DIALOG_DIR)

all: Proj.out

Proj.out: $(OBJECTS) $(IMGUI_OBJECTS)  $(IMGUI_DIALOG_OBJECTS)
	$(CC) $(OBJECTS) $(IMGUI_OBJECTS) $(IMGUI_DIALOG_OBJECTS) -o Proj.out -L$(LIB_DIR) $(LIBS)

main.obj: $(SRC_DIR)main.cpp $(IMGUI_OBJECTS)
	$(CC) $(SRC_DIR)main.cpp $(CCFLAGS) -o main.obj

ReservationStation.obj: $(SRC_DIR)ReservationStation.cpp $(SRC_INC_DIR)ReservationStation.h
	$(CC) $(SRC_DIR)ReservationStation.cpp $(CCFLAGS) -o ReservationStation.obj

Application.obj: $(SRC_DIR)Application.cpp
	$(CC) $(SRC_DIR)Application.cpp $(CCFLAGS) -o Application.obj

Controller.obj: $(SRC_DIR)Controller.cpp 
	$(CC) $(SRC_DIR)Controller.cpp $(CCFLAGS) -o Controller.obj

Instruction.obj: $(SRC_DIR)Instruction.cpp
	$(CC) $(SRC_DIR)Instruction.cpp $(CCFLAGS) -o Instruction.obj

$(IMGUI_OBJECTS_DIR)ImGuiFileDialog.obj:
	$(CC) $(IMGUI_DIALOG_DIR)ImGuiFileDialog.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)ImGuiFileDialog.obj

$(IMGUI_OBJECTS_DIR)imgui_demo.obj: $(IMGUI_SRC_DIR)imgui_demo.cpp 
	$(CC) $(IMGUI_SRC_DIR)imgui_demo.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)imgui_demo.obj

$(IMGUI_OBJECTS_DIR)imgui_draw.obj:
	$(CC) $(IMGUI_SRC_DIR)imgui_draw.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)imgui_draw.obj

$(IMGUI_OBJECTS_DIR)imgui_tables.obj:
	$(CC) $(IMGUI_SRC_DIR)imgui_tables.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)imgui_tables.obj

$(IMGUI_OBJECTS_DIR)imgui.obj:
	$(CC) $(IMGUI_SRC_DIR)imgui.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)imgui.obj

$(IMGUI_OBJECTS_DIR)imgui_widgets.obj:
	$(CC) $(IMGUI_SRC_DIR)imgui_widgets.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)imgui_widgets.obj

$(IMGUI_OBJECTS_DIR)imgui-SFML.obj: $(IMGUI_SRC_DIR)imgui-SFML.cpp $(IMGUI_SRC_DIR)imgui.h
	$(CC) $(IMGUI_SRC_DIR)imgui-SFML.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)imgui-SFML.obj

clean:
	rm $(OBJECTS)