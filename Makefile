build:
	@gcc -I./SDL2-2.0.10/include -L. -o snake.bin \
	src/main.c src/render.c src/timer.c src/gamesave.c src/bestplayers.c \
	src/objects/snake.c src/objects/dot.c src/objects/bluedot.c src/objects/reddot.c \
	-lSDL2 -lm -lpthread -ldl -lrt;
debug:
	@gcc -g -I./SDL2-2.0.10/include -L. -o snake.bin \
	src/main.c src/render.c src/timer.c src/gamesave.c src/bestplayers.c \
	src/objects/snake.c src/objects/dot.c src/objects/bluedot.c src/objects/reddot.c \
	-lSDL2 -lm -lpthread -ldl -lrt;