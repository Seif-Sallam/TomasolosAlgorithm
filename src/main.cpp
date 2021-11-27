#include "../headers/Application.h"
#include "../headers/Instruction.h"
int main()
{
    Application app(sf::Vector2u(640, 480), "Simulation :)");
    app.Run();
}