#include "Application.hpp"
#include "GraphicEditor/GraphicEditor.hpp"

int main() {
    Application::Init(1600, 900);

    DrawingManager *dm = new DrawingManager;
    dm->createCanvas(1200, 700);
    Application::Attach(dm);

    while (Application::Run()) {}

    Application::Finalize();
    return 0;   
}