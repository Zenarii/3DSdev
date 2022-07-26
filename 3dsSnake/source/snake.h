//
//~defines
//
#define TILE_SIZE 20
#define BOARD_WIDTH  (SCREEN_WIDTH  / TILE_SIZE)
#define BOARD_HEIGHT (SCREEN_HEIGHT / TILE_SIZE)
#define MAX_SNAKE_LENGTH 128

#define SNAKE_COLOUR (0xFFFFFFFF)
#define BG_COLOUR    (C2D_Color32(0x00, 0x00, 0x00, 0xFF))
#define FOOD_COLOUR  (C2D_Color32(0xFF, 0x00, 0x00, 0xFF))

#define TIME_STEPS_PER_MOVE 10

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
    int last_move_direction;
    int body_length;
    v2i body[MAX_SNAKE_LENGTH];
};

typedef struct GameState GameState;
struct GameState {
    u32 steps_since_move;
    Snake snake;
    v2i food;
    int score;
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
        DrawBox(head_rect, SNAKE_COLOUR);
    }
    for(int i = 1; i < snake->body_length; ++i) {
        v4 body_rect = v4(snake->body[i].x * TILE_SIZE,
                          snake->body[i].y * TILE_SIZE, 
                          TILE_SIZE, TILE_SIZE);
        DrawRect(body_rect, SNAKE_COLOUR);
    }
}

//
//~
//

v2i GetFreeSpace(Snake * snake) {
    v2i food_position = {0};
    
    StartPlaceFood:;
    
    food_position.x = random() % BOARD_WIDTH;
    food_position.y = random() % BOARD_HEIGHT;
    
    for(int i = 0; i < snake->body_length; ++i) {
        int collided = ((food_position.x == snake->body[i].x) &&
                        (food_position.y == snake->body[i].y));
        if(collided) goto StartPlaceFood;
    }
    
    return food_position;
}

void MoveSnake(Snake * snake) {
    for(int i = snake->body_length-1; i >= 1; --i) {
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
        snake->last_move_direction = snake->direction;
        
        // check if food eaten
        int food_eaten = ((snake->body[0].x == game_state->food.x)
                          && (snake->body[0].y == game_state->food.y));
        if(food_eaten) {
            snake->body[snake->body_length] = snake->body[snake->body_length-1];
            snake->body_length++;
            game_state->score++;
            game_state->food = GetFreeSpace(snake);
        }
    }
    // todo, last move direction?
    u32 keys_down = hidKeysDown();
    if((keys_down & KEY_LEFT)  && snake->last_move_direction != DIR_RIGHT)
        snake->direction = DIR_LEFT;
    if((keys_down & KEY_UP)    && snake->last_move_direction != DIR_DOWN)
        snake->direction = DIR_UP;
    if((keys_down & KEY_RIGHT) && snake->last_move_direction != DIR_LEFT)
        snake->direction = DIR_RIGHT;
    if((keys_down & KEY_DOWN)  && snake->last_move_direction != DIR_UP)
        snake->direction = DIR_DOWN;
    
    //
    // Draw Everything
    //
    
    DrawSnake(snake);
    {
        v4 food_rect = v4(game_state->food.x * TILE_SIZE,
                          game_state->food.y * TILE_SIZE,
                          TILE_SIZE,
                          TILE_SIZE);
        DrawRect(food_rect, FOOD_COLOUR);
    }
    
    //print out the score
    printf("\x1b[2;1HYour Score: %d\x1b[K", game_state->score);
}

GameState GameStateInit() {
    GameState game_state = {0};
    
    // note(abiab): init snake 
    srandom(time(0));
    game_state.snake.body_length = 3;
    game_state.snake.direction   = DIR_RIGHT;
    
    game_state.food = GetFreeSpace(&game_state.snake);
    
    return game_state;
}