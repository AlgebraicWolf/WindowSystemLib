CFLAGS = -std=c++20 -O3 -Wall -Werror -Wextra -pedantic -pedantic-errors -g 
SFMLLIB = -lsfml-system -lsfml-graphics -lsfml-window

Window.o: WindowSystem/Window.cpp WindowSystem/Window.hpp
	clang++ $(CFLAGS) -c -o Window.o WindowSystem/Window.cpp

SFMLRenderEngine.o: SFMLRenderEngine/SFMLRenderEngine.cpp SFMLRenderEngine/RenderEngine.hpp
	clang++ $(CFLAGS) -c -o SFMLRenderEngine.o SFMLRenderEngine/SFMLRenderEngine.cpp

app.o: main.cpp Application.hpp
	clang++ $(CFLAGS) -c -o app.o main.cpp

build_sfml: app.o SFMLRenderEngine.o Window.o 
	clang++ $(CFLAGS) $(SFMLLIB) -o main app.o SFMLRenderEngine.o Window.o

clean:
	rm -rf *.o main