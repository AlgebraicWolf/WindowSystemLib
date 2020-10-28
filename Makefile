CFLAGS = -O3 -Wall -Werror -Wextra -pedantic -pedantic-errors
SFMLLIB = -lsfml-system -lsfml-graphics -lsfml-window

WindowSystem: WindowSystem/Window.cpp WindowSystem/Window.hpp
	clang++ $(CFLAGS) -c -o Window.o WindowSystem/Window.cpp

SFMLRenderEngine: SFMLRenderEngine/RenderEngine.cpp RenderEngine.hpp
	clang++ $(CFLAGS) $(SFMLLIB) -c -o RenderEngine.o SFMLRenderEngine/SFMLRenderEngine.cpp

Application: main.cpp
	clang++ $(CFLAGS) -c -o app.o main.cpp

build_sfml: Application WindowSystem SFMLRenderEngine
	clang++ $(CFLAGS) -o main app.o RenderEngine.o Window.o