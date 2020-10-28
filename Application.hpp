

// Singletone application class
class Application {
   public:
    static void Init();
    static void Finalize();
    static bool Run();

   private:
    Application();  // Ensure that class is indeed singletone by prohibiting object construction
};

void Application::Init() {

}

void Application::Finalize() {

}

bool Application::Run() {
    return 0;
}