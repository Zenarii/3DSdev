//
//~defines
//
#define TILE_SIZE 20
#define BOARD_WIDTH  (SCREEN_WIDTH  / TILE_SIZE)
#define BOARD_HEIGHT (SCREEN_HEIGHT / TILE_SIZE)
#define MAX_SNAKE_LENGTH 128

#define SNAKE_COLOUR (0xFFFFFFFF)
#define BG_COLOUR    (C2D_Color32(0x00, 0x00, 0x00, 0xFF))

#define TIME_STEPS_PER_MOVE 30

enum Directions {
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN,
};

//
//~data
//
typedef struct v4 v4;
struct v4 {
    float x, y, z, w;
};
#define v4(x, y, z, w) ((v4){x, y, z, w})

typedef struct v2i v2i;
struct v2i {
    int x, y;
};

typedef struct Snake Snake;
struct Snake {
    int direction;
    int body_length;
    v2i body[MAX_SNAKE_LENGTH];
};

typedef struct GameState GameState;
struct GameState {
    u32 steps_since_move;
    Snake snake;
};

//
//~output
//

void DrawBox(v4 box, u32 colour) {
    C2D_DrawRectangle(box.x, box.y, 0, box.z, box.w,
                      colour, colour, colour, colour);
    C2D_DrawRectangle(box.x + 1, box.y + 1, 0, box.z - 2, box.w - 2,
                      BG_COLOUR, BG_COLOUR, BG_COLOUR, BG_COLOUR);
}

void DrawRect(v4 rect, u32 colour) {
    C2D_DrawRectangle(rect.x, rect.y, 0, rect.z, rect.w,
                      colour, colour, colour, colour);
}

void DrawSnake(Snake * snake) {
    {
        v4 head_rect = v4(snake->body[0].x * TILE_SIZE, snake->body[0].y * TILE_SIZE, 
                          TILE_SIZE, TILE_SIZE); 
        DrawRect(head_rect, SNAKE_COLOUR);
    }
    for(int i = 1; i < snake->body_length; ++i) {
        v4 body_rect = v4(snake->body[i].x * TILE_SIZE,
                          snake->body[i].y * TILE_SIZE, 
                          TILE_SIZE, TILE_SIZE);
        DrawBox(body_rect, SNAKE_COLOUR);
    }
}

//
//~
//

void MoveSnake(Snake * snake) {
    for(int i = snake->body_length-1; i >= 1; ++i) {
        snake->body[i] = snake->body[i-1];
    }
    switch (snake->direction) {
        case DIR_LEFT:  snake->body[0].x--; break;
        case DIR_RIGHT: snake->body[0].x++; break;
        case DIR_UP:    snake->body[0].y--; break;
        case DIR_DOWN:  snake->body[0].y++; break;
    }
}

void GameStep(GameState * game_state) {
    game_state->steps_since_move++;
    Snake * snake = &game_state->snake;
    
    if(game_state->steps_since_move >= TIME_STEPS_PER_MOVE) {
        game_state->steps_since_move = 0;
        MoveSnake(snake);
    }
    
    
    DrawSnake(snake);
}

GameState GameStateInit() {
    GameState game_state = {0};
    
    // note(abiab): init snake 
    game_state.snake.body_length = 1;
    game_state.snake.direction   = DIR_RIGHT;
    
    return game_state;
}