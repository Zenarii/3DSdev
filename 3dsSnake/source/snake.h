//
//~defines
//
#define TILE_SIZE 20
#define BOARD_WIDTH  (SCREEN_WIDTH  / TILE_SIZE)
#define BOARD_HEIGHT (SCREEN_HEIGHT / TILE_SIZE)
#define MAX_SNAKE_LENGTH ((BOARD_WIDTH * BOARD_HEIGHT) + 1)

#define SNAKE_COLOUR (0xFFFFFFFF)
#define BG_COLOUR    (C2D_Color32(0x00, 0x00, 0x00, 0xFF))
#define FOOD_COLOUR  (C2D_Color32(0xFF, 0x00, 0x00, 0xFF))

#define TIME_STEPS_PER_MOVE 10

#define SAVE_FILE_PATH "snake_save.txt"

enum Directions {
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN,
};

enum States {
    STATE_GAME,
    STATE_MENU,
    STATE_DEATH
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

//
// ~fileio
//


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

void DrawFood(v2i food) {
    v4 food_rect = v4(food.x * TILE_SIZE, food.y * TILE_SIZE,
                      TILE_SIZE, TILE_SIZE);
    DrawRect(food_rect, FOOD_COLOUR);
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

//
//~GameState
//


typedef struct GameState GameState;
struct GameState {
    u32 steps_since_move;
    Snake snake;
    v2i food;
    int score;
    int high_score;
    int snake_self_collide;
};

void GameStep(GameState * game_state) {
    game_state->steps_since_move++;
    Snake * snake = &game_state->snake;
    
    // note(abiab): player input
    
    u32 keys_down = hidKeysDown();
    if((keys_down & KEY_LEFT)  && snake->last_move_direction != DIR_RIGHT)
        snake->direction = DIR_LEFT;
    if((keys_down & KEY_UP)    && snake->last_move_direction != DIR_DOWN)
        snake->direction = DIR_UP;
    if((keys_down & KEY_RIGHT) && snake->last_move_direction != DIR_LEFT)
        snake->direction = DIR_RIGHT;
    if((keys_down & KEY_DOWN)  && snake->last_move_direction != DIR_UP)
        snake->direction = DIR_DOWN;
    
    // note(abiab): movement step
    
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
            if(game_state->score > game_state->high_score)
                game_state->high_score = game_state->score;
            
            game_state->food = GetFreeSpace(snake);
        }
        
        int out_of_bounds = (snake->body[0].x >= BOARD_WIDTH  ||
                             snake->body[0].x <  0            ||
                             snake->body[0].y >= BOARD_HEIGHT ||
                             snake->body[0].y <  0);
        
        if(out_of_bounds || game_state->snake_self_collide) {
            current_state = STATE_DEATH;
            FILE * file = fopen(SAVE_FILE_PATH, "w");
            if(file) {
                fprintf(file, "%d", game_state->high_score);
            }
            fclose(file);
        }
    }
    
    //
    // Checking for the self collision across multiple frames
    //
    
    for(int i = game_state->steps_since_move+1; i < snake->body_length; i += TIME_STEPS_PER_MOVE) {
        if(snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y) {
            game_state->snake_self_collide = 1;
            break;
        }
    }
    
    
    
    //
    // Draw Everything
    //
    
    DrawSnake(snake);
    DrawFood(game_state->food);
    
    //print out the score
    printf("\x1b[2;1HYour Score: %d\x1b[K", game_state->score);
    printf("\x1b[3;1HHigh Score: %d\x1b[K", game_state->high_score);
}

GameState GameStateInit() {
    GameState game_state = {0};
    
    // note(abiab): load high score
    FILE * file = fopen(SAVE_FILE_PATH, "r");
    if(file) {
        fscanf(file, "%d", &game_state.high_score);
    }
    fclose(file);
    
    // note(abiab): init snake 
    srandom(time(0));
    game_state.snake.body_length = 3;
    game_state.snake.body[0].x = 2;
    game_state.snake.body[1].x = 1;
    game_state.snake.direction   = DIR_RIGHT;
    
    game_state.food = GetFreeSpace(&game_state.snake);
    
    
    return game_state;
}

//
//~DeathState
//
#define DEATH_TIME_STEP 5

typedef struct DeathState DeathState;
struct DeathState {
    int frames_dead;
};

void DeathStep(DeathState * death_state, GameState * game_state) {
    death_state->frames_dead++;
    
    DrawFood(game_state->food);
    
    // Death Animation; todo sound
    Snake * snake = &game_state->snake;
    if(death_state->frames_dead < DEATH_TIME_STEP * snake->body_length)
    {
        
        int front_index = death_state->frames_dead / DEATH_TIME_STEP;
        for(int i = front_index + 1; i < snake->body_length; ++i) {
            v4 body_rect = v4(snake->body[i].x * TILE_SIZE,
                              snake->body[i].y * TILE_SIZE,
                              TILE_SIZE, TILE_SIZE);
            DrawRect(body_rect, SNAKE_COLOUR);
        }
        
        int expand = death_state->frames_dead % DEATH_TIME_STEP;
        v4 front_rect = v4(snake->body[front_index].x * TILE_SIZE,
                           snake->body[front_index].y * TILE_SIZE,
                           TILE_SIZE, TILE_SIZE);
        front_rect.x -= expand;
        front_rect.y -= expand;
        front_rect.z += expand * 2;
        front_rect.w += expand * 2;
        
        DrawRect(front_rect, SNAKE_COLOUR);
    }
    else {
        death_state->frames_dead = 0;
        current_state = STATE_GAME;
        *game_state = GameStateInit();
    }
    
    // todo: You died text and menu
}