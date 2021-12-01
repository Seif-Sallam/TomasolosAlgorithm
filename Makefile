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
IMGUI_OBJECTS= $(IMGUI_OBJECTS_DIR)imgui_demo.obj $(IMGUI_OBJECTS_DIR)imgui_draw.obj $(IMGUI_OBJECTS_DIR)imgui_tables.obj $(IMGUI_OBJECTS_DIR)imgui.obj $(IMGUI_OBJECTS_DIR)imgui-SFML.obj $(IMGUI_OBJECTS_DIR)imgui_widgets.obj 
IMGUI_DIALOG_OBJECTS= $(IMGUI_OBJECTS_DIR)ImGuiFileDialog.obj 
OBJECTS= main.obj ReservationStation.obj Controller.obj Instruction.obj RegisterFile.obj Application.obj 
INC= -I$(SFML_INC_DIR) -I$(IMGUI_INC_DIR) -I$(IMGUI_DIALOG_DIR) -I$(IMGUI_INC_DIR)imgui/
CCFLAGS=-c $(INC)

all: | Dir Proj.out 

Dir:
	mkdir -p $(IMGUI_OBJECTS_DIR)
	

Proj.out: $(OBJECTS) $(IMGUI_OBJECTS)  $(IMGUI_DIALOG_OBJECTS)
	$(CC) $(OBJECTS) $(IMGUI_OBJECTS) $(IMGUI_DIALOG_OBJECTS) -o Proj.out -L$(LIB_DIR) $(LIBS)

main.obj: $(SRC_DIR)main.cpp $(IMGUI_OBJECTS)
	$(CC) $(SRC_DIR)main.cpp $(CCFLAGS) -o main.obj

RegisterFile.obj: $(SRC_DIR)RegisterFile.cpp $(SRC_INC_DIR)RegisterFile.h
	$(CC) $(SRC_DIR)RegisterFile.cpp $(CCFLAGS) -o RegisterFile.obj

ReservationStation.obj: $(SRC_DIR)ReservationStation.cpp $(SRC_INC_DIR)ReservationStation.h 
	$(CC) $(SRC_DIR)ReservationStation.cpp $(CCFLAGS) -o ReservationStation.obj

Application.obj: $(SRC_DIR)Application.cpp $(SRC_DIR)Controller.cpp  $(SRC_DIR)ReservationStation.cpp $(SRC_DIR)Instruction.cpp $(SRC_DIR)RegisterFile.cpp $(SRC_INC_DIR)Application.h
	$(CC) $(SRC_DIR)Application.cpp $(CCFLAGS) -o Application.obj

Controller.obj: $(SRC_DIR)Controller.cpp  $(SRC_DIR)ReservationStation.cpp $(SRC_DIR)Instruction.cpp $(SRC_DIR)RegisterFile.cpp $(SRC_INC_DIR)Controller.h
	$(CC) $(SRC_DIR)Controller.cpp $(CCFLAGS) -o Controller.obj

Instruction.obj: $(SRC_DIR)Instruction.cpp $(SRC_INC_DIR)Instruction.h
	$(CC) $(SRC_DIR)Instruction.cpp $(CCFLAGS) -o Instruction.obj

$(IMGUI_OBJECTS_DIR)ImGuiFileDialog.obj:
	$(CC) $(IMGUI_DIALOG_DIR)ImGuiFileDialog.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)ImGuiFileDialog.obj

$(IMGUI_OBJECTS_DIR)imgui_demo.obj: $(IMGUI_SRC_DIR)imgui/imgui_demo.cpp 
	$(CC) $(IMGUI_SRC_DIR)imgui/imgui_demo.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)imgui_demo.obj

$(IMGUI_OBJECTS_DIR)imgui_draw.obj:
	$(CC) $(IMGUI_SRC_DIR)imgui/imgui_draw.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)imgui_draw.obj

$(IMGUI_OBJECTS_DIR)imgui_tables.obj:
	$(CC) $(IMGUI_SRC_DIR)imgui/imgui_tables.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)imgui_tables.obj

$(IMGUI_OBJECTS_DIR)imgui.obj:
	$(CC) $(IMGUI_SRC_DIR)imgui/imgui.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)imgui.obj

$(IMGUI_OBJECTS_DIR)imgui_widgets.obj:
	$(CC) $(IMGUI_SRC_DIR)imgui/imgui_widgets.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)imgui_widgets.obj

$(IMGUI_OBJECTS_DIR)imgui-SFML.obj: $(IMGUI_SRC_DIR)imgui/imgui-SFML.cpp $(IMGUI_SRC_DIR)imgui/imgui.h
	$(CC) $(IMGUI_SRC_DIR)imgui/imgui-SFML.cpp $(CCFLAGS) -o $(IMGUI_OBJECTS_DIR)imgui-SFML.obj

clean:
	rm $(OBJECTS)