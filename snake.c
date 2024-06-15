#include "snake.h"

#define WINDOW_X 0
#define WINDOW_Y 0
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define GRID_SIZE 20
#define GRID_DIM 1000
#define THICK 10

// SNAKE
enum
{
    SNAKE_UP,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT,
};

typedef struct apple
{
    int x;
    int y;

} apple;

apple Apple;

struct snake
{
    int x;
    int y;
    int dir;

    struct snake *next;
};
typedef struct snake Snake;

Snake *head;
Snake *tail;

// DEPLOY SNAKE
void deploy_snake()
{
    Snake *new = malloc(sizeof(Snake));
    new->x = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->y = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->dir = SNAKE_UP;
    new->next = NULL;

    head = new;
    tail = new;

    return;
}

// APPEND SNAKE WHEN APPLE EATEN
void append_snake()
{
    Snake *new = malloc(sizeof(Snake));

    switch (tail->dir)
    {
    case SNAKE_UP:
        new->x = tail->x;
        new->y = tail->y + 1;
        break;
    case SNAKE_DOWN:
        new->x = tail->x;
        new->y = tail->y - 1;
        break;
    case SNAKE_LEFT:
        new->x = tail->x + 1;
        new->y = tail->y;
        break;
    case SNAKE_RIGHT:
        new->x = tail->x - 1;
        new->y = tail->y;
        break;
    }

    new->dir = tail->dir;

    new->next = NULL;
    tail->next = new;

    tail = new;

    return;
}

// RENDER SNAKE
void render_snake(SDL_Renderer *renderer, int x, int y)
{
    int seg_size = GRID_DIM / GRID_SIZE;
    SDL_Rect seg;
    seg.w = seg_size;
    seg.h = seg_size;

    Snake *track = head;
    int segment_index = 0;

    while (track != NULL)
    {
        seg.x = x + (track->x * seg_size);
        seg.y = y + (track->y * seg_size);

        if (segment_index % 2 == 0)
        {
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 255); // Cyan color
        }
        else
        {
            // SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 255); // Pink color
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 255); // GREEN
        }

        SDL_RenderFillRect(renderer, &seg);

        track = track->next;
        segment_index++;
    }
    return;
}

// MOVE SNAKE
void move_snake()
{
    int prev_x = head->x;
    int prev_y = head->y;
    int prev_dir = head->dir;

    switch (head->dir)
    {
    case SNAKE_UP:
        head->y--;
        break;
    case SNAKE_DOWN:
        head->y++;
        break;
    case SNAKE_LEFT:
        head->x--;
        break;
    case SNAKE_RIGHT:
        head->x++;
        break;
    }

    Snake *track = head;

    if (track->next != NULL)
    {
        track = track->next;
    }

    while (track != NULL)
    {
        int save_x = track->x;
        int save_y = track->y;
        int save_dir = track->dir;

        track->x = prev_x;
        track->y = prev_y;
        track->dir = prev_dir;

        track = track->next;

        prev_x = save_x;
        prev_y = save_y;
        prev_dir = save_dir;
    }
    return;
}

// APPLE
void gen_apple()
{
    bool in_snake;

    do
    {
        in_snake = false;
        Apple.x = rand() % GRID_SIZE;
        Apple.y = rand() % GRID_SIZE;

        Snake *track = head;

        while (track != NULL)
        {
            if (track->x == Apple.x && track->y == Apple.y)
            {
                in_snake = true;
            }
            track = track->next;
        }
    } while (in_snake);
}

void render_apple(SDL_Renderer *renderer, int x, int y)
{
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 255); // RED

    int apple_size = GRID_DIM / GRID_SIZE;

    SDL_Rect app;
    app.w = apple_size;
    app.h = apple_size;
    app.x = x + (Apple.x * apple_size);
    app.y = y + (Apple.y * apple_size);

    SDL_RenderFillRect(renderer, &app);
}

void detect_apple()
{
    if (head->x == Apple.x && head->y == Apple.y)
    {
        gen_apple();
        append_snake();
    }

    return;
}

// RESET
void reset_snake()
{
    Snake *track = head;
    Snake *temp;

    while (track != NULL)
    {
        temp = track;
        track = track->next;
        free(temp);
    }

    deploy_snake();
    append_snake();
    append_snake();
    append_snake();

    return;
}

// COLLISION
void detect_collision()
{
    if (head->x < 0 || head->x >= GRID_SIZE || head->y < 0 || head->y >= GRID_SIZE)
    {
        reset_snake();
    }

    Snake *track = head;

    if (track->next != NULL)
    {
        track = track->next;
    }

    while (track != NULL)
    {
        if (track->x == head->x && track->y == head->y)
        {
            reset_snake();
        }
        track = track->next;
    }

    return;
}

// GRID
void render_grid(SDL_Renderer *renderer, int x, int y)
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 255);

    SDL_Rect outline;
    outline.x = x - THICK;
    outline.y = y - THICK;
    outline.w = GRID_DIM + 2 * THICK;
    outline.h = GRID_DIM + 2 * THICK;

    for (int i = 0; i < THICK; ++i)
    {
        SDL_RenderDrawRect(renderer, &outline);
        outline.x += 1; // Shift the outline inward
        outline.y += 1;
        outline.w -= 2; // Decrease width and height
        outline.h -= 2;
    }

    return;
}

// MAIN FUNCTION
int main()
{
    srand(time(0));
    // UNLEASHING SNAKE
    deploy_snake();
    append_snake();
    append_snake();
    append_snake();
    // GEN APPLE
    gen_apple();
    // WINDOW AND RENDER POINTERS
    SDL_Window *window;
    SDL_Renderer *renderer;

    // ERROR HANDLING
    if (SDL_INIT_VIDEO < 0)
    {
        fprintf(stderr, "ERROR: SDL_INIT_VIDEO");
    }

    // Create window
    window = SDL_CreateWindow(
        "Snake",
        WINDOW_X,
        WINDOW_Y,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS);

    // ERROR HANDLING 2
    if (!window)
    {
        fprintf(stderr, "ERROR: !window");
    }

    // Set renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // ERROR HANDLING 3
    if (!renderer)
    {
        fprintf(stderr, "ERROR: !renderer");
    }

    // CENTER GRID
    int grid_x = (WINDOW_WIDTH / 2) - (GRID_DIM / 2);
    int grid_y = (WINDOW_HEIGHT / 2) - (GRID_DIM / 2);

    // SET QUIT VARIABLE
    bool quit = false;
    SDL_Event event;

    // MAIN LOOP
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;

            case SDL_KEYUP:
                break;
            // ESC = QUIT
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_w:
                    head->dir = SNAKE_UP;
                    break;
                case SDLK_s:
                    head->dir = SNAKE_DOWN;
                    break;
                case SDLK_a:
                    head->dir = SNAKE_LEFT;
                    break;
                case SDLK_d:
                    head->dir = SNAKE_RIGHT;
                    break;
                }
                break;
            }
        }

        // CLEAN PREV RENDER
        SDL_RenderClear(renderer);

        // MOVE SNAKE
        move_snake();

        // DETECTIONS
        detect_apple();
        detect_collision();

        // START RENDER
        render_grid(renderer, grid_x, grid_y);

        render_apple(renderer, grid_x, grid_y);

        render_snake(renderer, grid_x, grid_y);
        // END RENDER

        // SHOW RENDER
        SDL_SetRenderDrawColor(renderer, 0x12, 0x12, 0x12, 255);
        SDL_RenderPresent(renderer);

        SDL_Delay(100);
    }

    // ClEAN UP AFTER LOOP END
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}