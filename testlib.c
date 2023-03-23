#include "pieces.c"
#include "primlib.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MARGIN_Y 50
#define MARGIN_X 300
#define BLOCKS_X 20
#define BLOCKS_Y 30
#define MAX_BOARD_WIDTH (gfx_screenWidth() - 2 * MARGIN_X)
#define MAX_BOARD_HEIGHT (gfx_screenHeight() - 2 * MARGIN_Y)
#define BLOCK_WIDTH MAX_BOARD_WIDTH / BLOCKS_X
#define BLOCK_HEIGHT MAX_BOARD_HEIGHT / BLOCKS_Y
#define BLOCK_SIZE (BLOCK_WIDTH > BLOCK_HEIGHT ? BLOCK_HEIGHT : BLOCK_WIDTH)
#define INITIAL_X (gfx_screenWidth() - BLOCK_SIZE * BLOCKS_X) / 2
#define INITIAL_Y (gfx_screenHeight() - BLOCK_SIZE * BLOCKS_Y) / 2
#define INITIAL_SPEED_CONTROLER 20
#define BOARD_WIDTH BLOCKS_X* BLOCK_SIZE
#define BOARD_HEIGHT BLOCKS_Y* BLOCK_SIZE

int board[BLOCKS_Y][BLOCKS_X] = {0};

typedef struct {
	int block[4][4][4];
	int variant;
	int x;
	int y;
} piece;

void draw_block(int x, int y, enum color c, int block_size)
{
	if (block_size == 0)
		block_size = BLOCK_SIZE;
	gfx_filledRect(x, y, x + block_size, y + block_size, c);
	gfx_rect(x, y, x + block_size + 1, y + block_size + 1, BLACK);
}

void draw_next_block(piece* p)
{
	int size = 200;
	int x = INITIAL_X + BLOCKS_X * BLOCK_SIZE + 50;
	int y = (gfx_screenHeight() - size) / 2;
	int block_size = 30;
	int p_width = get_width(p) * block_size;
	int p_height = get_height(p) * block_size;
	int p_x = (size - p_width) / 2 + x;
	int p_y = (size - p_height) / 2 + y;

	gfx_rect(x, y, x + size, y + size, WHITE);
	gfx_textout(x + 10, y + 10, "Next block:", WHITE);

	draw_next_piece(p, p_x, p_y, block_size);
}

void clean_row(int row)
{
	for (int i = row; i >= 1; i--) {
		for (int j = 0; j < BLOCKS_X; j++) {
			board[i][j] = board[i - 1][j];
		}
	}
}

void draw_board()
{
	int x1 = INITIAL_X - 1;
	int y1 = INITIAL_Y + 1;
	int x2 = x1 + BOARD_WIDTH + 2;
	int y2 = y1 + BOARD_HEIGHT;
	gfx_line(x1, y1, x1, y2, WHITE);
	gfx_line(x1, y2, x2, y2, WHITE);
	gfx_line(x2, y2, x2, y1, WHITE);

	for (int i = 0; i < BLOCKS_Y; i++) {
		int sum = 0;
		for (int j = 0; j < BLOCKS_X; j++) {
			int x = INITIAL_X + BLOCK_SIZE * j;
			int y = INITIAL_Y + BLOCK_SIZE * i;
			// gfx_rect(x, y, x + BLOCK_SIZE + 1, y + BLOCK_SIZE + 1, WHITE);
			if (board[i][j] != 0) {
				draw_block(x, y, RED, 0);
				sum += 1;
			}
		}
		if (sum == BLOCKS_X) {
			clean_row(i);
		}
	}
}

void get_block(piece* p)
{
	int r1 = rand() % 7;
	int r2 = rand() % 4;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				p->block[k][i][j] = pieces[r1][k][i][j];
			}
		}
	}

	p->variant = r2;
	p->x = BLOCKS_X / 2;
	p->y = 0;
}

void draw_next_piece(piece* p, int x, int y, int block_size)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (p->block[p->variant][i][j] != 0) {
				draw_block(x + j * block_size, y + i * block_size, BLUE,
						   block_size);
			}
		}
	}
}

void draw_piece(piece* p)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (p->block[p->variant][i][j] != 0) {
				int x = p->x;
				int y = p->y;
				draw_block((x + j) * BLOCK_SIZE + INITIAL_X,
						   (y + i) * BLOCK_SIZE + INITIAL_Y, BLUE, 0);
			}
		}
	}
}

int get_width(piece* p)
{
	int min_idx_x = 3;
	int max_idx_x = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (p->block[p->variant][i][j] != 0) {
				if (j > max_idx_x) {
					max_idx_x = j;
				}
				else if (j < min_idx_x) {
					min_idx_x = j;
				}
			}
		}
	}
	return max_idx_x - min_idx_x + 1;
}

int get_height(piece* p)
{
	int min_idx_y = 3;
	int max_idx_y = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (p->block[p->variant][i][j] != 0) {
				if (i > max_idx_y) {
					max_idx_y = i;
				}
				else if (j < min_idx_y) {
					min_idx_y = i;
				}
			}
		}
	}
	return max_idx_y - min_idx_y + 1;
}

void display_board()
{
	for (int i = 0; i < BLOCKS_Y; i++) {
		for (int j = 0; j < BLOCKS_X; j++) {
			printf("%d ", board[i][j]);
		}
		printf("\n");
	}
}

void add_piece_to_board(piece* p)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			int x = p->x + j;
			int y = p->y + i;
			if (board[y][x] == 0 && p->block[p->variant][i][j] != 0) {
				board[y][x] = 1;
			};
		}
	}
	// display_board();
}

int move_down(piece* p)
{
	/* Returns 1 when the piece collides otherwise returs 0 */
	int height = get_height(p);
	if (p->y + height - 1 <= BLOCKS_Y - 1) {
		p->y += 1;
		if (is_colliding(p) || p->y + height - 1 >= BLOCKS_Y) {
			p->y -= 1;
			add_piece_to_board(p);
			return 1;
		}
	}
	return 0;
}

void move_right(piece* p)
{
	int width = get_width(p);
	if (p->x + width <= BLOCKS_X - 1) {
		p->x += 1;
		if (is_colliding(p)) {
			p->x -= 1;
		}
	}
}

void move_left(piece* p)
{
	if (p->x - 1 >= 0) {
		p->x -= 1;
		if (is_colliding(p)) {
			p->x += 1;
		}
	}
}

int is_colliding(piece* p)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			int x = p->x + j;
			int y = p->y + i;
			if (board[y][x] != 0 && p->block[p->variant][i][j] != 0)
				return 1;
		}
	}
	return 0;
}

int is_colliding_after_rotation(piece* p)
{
	int width = get_width(p);
	int height = get_height(p);
	if (is_colliding(p) || p->x < 0 || p->x + width - 1 >= BLOCKS_X ||
		p->y + height - 1 >= BLOCKS_Y) {
		return 1;
	}
	return 0;
}

void rotate(piece* p)
{
	if (p->variant + 1 <= 3) {
		p->variant += 1;
		if (is_colliding_after_rotation(p)) {
			p->variant -= 1;
		}
	}
	else {
		p->variant = 0;
		if (is_colliding_after_rotation(p)) {
			p->variant = 3;
		}
	}
}

void generate_new_piece(piece* p, piece* np)
{
	p->variant = np->variant;
	p->x = np->x;
	p->y = np->y;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				p->block[k][i][j] = np->block[k][i][j];
			}
		}
	}

	get_block(np);
}

void reset_board()
{
	for (int i = 0; i < BLOCKS_Y; i++) {
		for (int j = 0; j < BLOCKS_X; j++) {
			board[i][j] = 0;
		}
	}
}

void display_game_over(piece* p)
{
	if (is_colliding(p)) {
		reset_board();
		gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
					   BLACK);
		gfx_textout(gfx_screenWidth() / 2 - 20, gfx_screenHeight() / 2,
					"Game over", RED);
		gfx_textout(gfx_screenWidth() / 2 - 70, gfx_screenHeight() / 2 + 20,
					"Press any key to play again", WHITE);
		gfx_updateScreen();
		gfx_getkey();
	}
}

int main(int argc, char* argv[])
{
	if (gfx_init()) {
		exit(3);
	}
	srand(time(NULL));

	int speed_controler = INITIAL_SPEED_CONTROLER;
	int iterations = 0;
	int can_move = 1;
	int playing = 1;

	piece moving_piece;
	piece next_piece;

	get_block(&moving_piece);
	get_block(&next_piece);

	while (playing) {
		int key = gfx_pollkey();
		if (key == SDLK_q) {
			return 0;
		}

		if (can_move) {
			if (key == SDLK_RIGHT) {
				move_right(&moving_piece);
			}
			else if (key == SDLK_LEFT) {
				move_left(&moving_piece);
			}
			else if (key == SDLK_DOWN) {
				speed_controler = 2;
				can_move = 0;
			}
			else if (key == SDLK_SPACE) {
				rotate(&moving_piece);
			}
		}

		gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
					   BLACK);

		if (iterations % speed_controler == 0) {
			int has_landed = move_down(&moving_piece);
			if (has_landed) {
				generate_new_piece(&moving_piece, &next_piece);
				display_game_over(&moving_piece);
				can_move = 1;
				speed_controler = INITIAL_SPEED_CONTROLER;
			}
			iterations = 0;
		}

		draw_board();
		draw_piece(&moving_piece);
		draw_next_block(&next_piece);

		iterations += 1;
		gfx_updateScreen();
		SDL_Delay(10);
	}

	gfx_getkey();
	return 0;
}
