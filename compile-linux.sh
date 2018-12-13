g++ -O3 ./src/Light.cpp ./src/Camera.cpp ./src/Mesh.cpp ./src/ModelLoader.cpp ./src/main.cpp -o ./prog -I./res/SDL/include/ -I./res/GLM/ -I./res/ -L./res/SDL/lib/x64 -lSDL2 -lassimp -ldl -lpthread
