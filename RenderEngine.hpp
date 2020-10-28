#ifndef RENDERENGINE_HPP_
#define RENDERENGINE_HPP_

class RenderEngine {
   public:
    static void Init(unsigned int width, unsigned int height); // Initialization
    static void Finalize(); // Finalization
    static bool Run(); // Do one loop iteration
   private:
    RenderEngine();  // Private constructor ensures that class is a singletone indeed
};
#endif  // RENDERENGINE_HPP_