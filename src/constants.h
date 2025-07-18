#define SCREEN_WIDTH 640 // pixels
#define SCREEN_HEIGHT 480 // pixels
#define FRAMES_PER_SECOND 30

#define GAME_TITLE "Snake" // displayed in the title bar

// These messages are displayed in the terminal
#define SDL_INIT_FAILED "SDL init failed"
#define SDL_CREATE_WINDOW_AND_RENDERER_FAILED "SDL window and renderer create failed"

#define FONT_FILEPATH "./font.bmp"
#define FONT_ROW_SIZE 16 // chars in one row
#define CHAR_SIZE 8 // width and height in pixels

#define SNAKE_SKIN_FILEPATH "./snakebody.bmp"
#define SNAKE_HEAD_FILEPATH "./snakehead.bmp"
#define SNAKE_TAIL_FILEPATH "./snaketail.bmp"

#define BOARD_SECTION_WIDTH 30 // squares
#define BOARD_SECTION_HEIGHT 20 // squares

#define STATUS_SECTION_MARGIN 8 // margin between window border and status section border in pixels
#define STATUS_SECTION_LINES 2

// these strings are displayed in the status section
#define IMPLEMENTED_REQUIREMENTS_HEADER "Implemented requirements:"
#define IMPLEMENTED_REQUIREMENTS "all mandatory (1-4), some optional (A-G)"
#define RED_DOT_APPEAR_BAR_DESCRIPTION "RED DOT"
#define SNAKE_DEATH_UNSPECIFIED_INFO "Snake killed."
#define SNAKE_HIT_ITSELF_INFO "Snake hit itself."
#define SNAKE_HIT_WALL_INFO "Snake hit the wall."
#define SNAKE_DEATH_KEY_GUIDE "Press N to retry or ESC to quit."
#define LEADERBOARD_KEY_GUIDE "Press ENTER to confirm the name or ESC to refuse."

#define LEADERBOARD_HEADER "LEADERBOARD"

#define SNAKE_INIT_POS_X 10 // square
#define SNAKE_INIT_POS_Y 10 // square
#define SNAKE_INIT_DIRECTION SNAKE_LEFT // which direction snake moves just after the game starts
#define SNAKE_INIT_LENGTH 3 // segments
#define SNAKE_SEGMENT_SIZE 20 // pixels
#define SNAKE_SMALL_SEGMENT_MARGIN 2 // pixels
#define SNAKE_INIT_COOLDOWN 200 // milliseconds
#define SNAKE_SPEEDUP_INTERVAL 60000 // milliseconds
#define SNAKE_COOLDOWN_CHANGE_RATE 0.8 // 20% faster

#define DOT_BLINK_INTERVAL 500 // milliseconds
#define SMALL_DOT_MARGIN 3 // pixels

#define BLUE_DOT_POINTS 100

#define RED_DOT_MIN_APPEAR_DELAY 30000 // milliseconds
#define RED_DOT_MAX_APPEAR_DELAY 60000 // milliseconds
#define RED_DOT_DISPLAY_TIME 15000 // milliseconds
#define RED_DOT_SNAKE_SEGMENT_DETACH_COUNT 3 // how much segments are detached when the snake eats the red dot
#define RED_DOT_POINTS 300

#define SAVE_FILENAME "save.txt"

#define BEST_PLAYERS_FILEPATH "bestplayers.txt"
#define BEST_PLAYER_COUNT 3 // how much players are listed
#define BEST_PLAYER_NAME_MAX_LENGTH 128 // chars (-1 for \0)
#define BEST_PLAYER_DEFAULT_NAME "Player" // passed when the player refuses to enter their name

#define TEXT_CURSOR "_"
