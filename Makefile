build:
	@gcc -I./sdl/include -L. -o snake.bin \
	src/main.c src/render.c src/timer.c src/gamesave.c src/bestplayers.c \
	src/objects/snake.c src/objects/dot.c src/objects/bluedot.c src/objects/reddot.c \
	-lSDL2 -lm -lpthread -ldl -lrt;
debug:
	@gcc -g -I./sdl/include -L. -o snake.bin \
	src/main.c src/render.c src/timer.c src/gamesave.c src/bestplayers.c \
	src/objects/snake.c src/objects/dot.c src/objects/bluedot.c src/objects/reddot.c \
	-lSDL2 -lm -lpthread -ldl -lrt;