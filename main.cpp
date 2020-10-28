#include "Application.hpp"

int main() {
    Application::Init();

    while (Application::Run()) {}

    Application::Finalize();
    return 0;   
}