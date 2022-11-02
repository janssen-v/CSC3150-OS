#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <curses.h>
#include <termios.h>
#include <fcntl.h>

#define ROW 10
#define COLUMN 50
#define LOG_LENGTH 20 //Log Length -> lower this to increase difficulty

pthread_mutex_t var_lock_1 = PTHREAD_MUTEX_INITIALIZER;

int game_status;

struct Node {
	int x, y;
	Node(int _x, int _y) : x(_x), y(_y){};
	Node(){};
} frog;

char map[ROW + 10][COLUMN];

// Determine keystroke. If yes, return 1. Else, return 0.
int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);

	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);

	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);

	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if (ch != EOF) {
		ungetc(ch, stdin);
		return 1;
	}
	return 0;
}

void sleep(int row, int sleep_row[ROW])
{
	usleep(sleep_row[row]);
}

void *logs_move(void *t)
{
	srand(time(0));
	int tid = (long)t;
	int row = tid + 1; // HRow
	pthread_mutex_lock(&var_lock_1); // Lock game area

	for (int l = 0; l < LOG_LENGTH; l++) {
		map[row][l] = '=';
	}
	for (int c = LOG_LENGTH; c < COLUMN - 1; c++) {
		map[row][c] = ' ';
	}

	int temp_row[COLUMN];
	int addv = (row % 2 == 1) ? 1 :
				    49; // Div rows into left and right sections
	pthread_mutex_unlock(&var_lock_1);

	// Delay log movement in a staggered fashion per row
	int delay[10];
	int base_delay = 150000; // Base delay of log, the higher the slower
	for (int i = 0; i < 10; i++) {
		delay[i] = base_delay + (rand() % 50 * 1500);
	}

	while (game_status == 1) {
		pthread_mutex_lock(&var_lock_1);
		/*  Move the logs  */
		for (int col = 0; col < COLUMN; col++) {
			if (row != frog.x) {
				if (map[row][col] == '0') {
					temp_row[(col + addv) % COLUMN] =
						'='; // Change frog to 0, previous loc to =
				} else {
					temp_row[(col + addv) % COLUMN] =
						map[row][col];
				}
			} else {
				if (col != frog.y && map[row][col] == '0') {
					temp_row[(col + addv) % COLUMN] = '=';
				} else {
					if (col == frog.y &&
					    map[row][col] != '0') {
						temp_row[(col + addv) % COLUMN] =
							'0';
					} else {
						temp_row[(col + addv) % COLUMN] =
							map[row][col];
					}
				}
			}
		}
		for (int col = 0; col < COLUMN; col++) {
			map[row][col] = temp_row[col];
		}
		/*  Check keyboard hits, to change frog's position or quit the game. */
		if (kbhit()) {
			char key = getchar();
			switch (key) {
			case 'Q':
			case 'q': // Quit
				game_status = 0;
				break;
			case 'W':
			case 'w': // Up
				if (map[frog.x - 1][frog.y] == '=' ||
				    map[frog.x - 1][frog.y] == '|') {
					if (frog.x == ROW) {
						map[frog.x][frog.y] = '|';
					} else {
						map[frog.x][frog.y] = '=';
					}
					frog.x -= 1;
					map[frog.x][frog.y] = '0';
				} else if (map[frog.x - 1][frog.y] ==
					   ' ') { // If move up is water, then drown
					if (frog.x == ROW) {
						map[frog.x][frog.y] = '|';
					} else {
						map[frog.x][frog.y] = '=';
					}
					frog.x -= 1;
					map[frog.x][frog.y] = '0';
					game_status = 3;
				}
				break;
			case 'A':
			case 'a': // Left
				// If frog is at leftmost part of screen, cannot move
				if (frog.y ==
				    1) { // Don't move past left boundary
					break;
				}
				if (map[frog.x][frog.y - 1] == '=' ||
				    map[frog.x][frog.y - 1] == '|') {
					if (frog.x == ROW) {
						map[frog.x][frog.y] = '|';
					} else {
						map[frog.x][frog.y] = '=';
					}
					frog.y -= 1;
					map[frog.x][frog.y] = '0';
				} else if (map[frog.x][frog.y - 1] ==
					   ' ') { // If move left is water, then drown
					if (frog.x == ROW) {
						map[frog.x][frog.y] = '|';
					} else {
						map[frog.x][frog.y] = '=';
					}
					frog.y -= 1;
					map[frog.x][frog.y] = '0';
					game_status = 3;
				}
				break;
			case 'S':
			case 's': // Down
				if (map[frog.x + 1][frog.y] == '=' ||
				    map[frog.x + 1][frog.y] == '|') {
					if (frog.x == ROW) {
						map[frog.x][frog.y] = '|';
					} else {
						map[frog.x][frog.y] = '=';
					}
					frog.x += 1;
					map[frog.x][frog.y] = '0';
				} else if (map[frog.x + 1][frog.y] ==
					   ' ') { // If move down is water, then drown
					if (frog.x == ROW) {
						map[frog.x][frog.y] = '|';
					} else {
						map[frog.x][frog.y] = '=';
					}
					frog.x += 1;
					map[frog.x][frog.y] = '0';
					game_status = 3;
				}
				break;
			case 'D':
			case 'd': // Right
				if (frog.y ==
				    48) { // Don't move past right boundary
					break;
				}
				if (map[frog.x][frog.y + 1] == '=' ||
				    map[frog.x][frog.y + 1] == '|') {
					if (frog.x == ROW) {
						map[frog.x][frog.y] = '|';
					} else {
						map[frog.x][frog.y] = '=';
					}
					frog.y += 1;
					map[frog.x][frog.y] = '0';
				} else if (map[frog.x][frog.y + 1] ==
					   ' ') { // If move right is water, then drown
					if (frog.x == ROW) {
						map[frog.x][frog.y] = '|';
					} else {
						map[frog.x][frog.y] = '=';
					}
					frog.y += 1;
					map[frog.x][frog.y] = '0';
					game_status = 3;
				}
				break;
			default:
				break;
			}
		}

		/*  Print the map on the screen  */
		printf("\033[H");
		for (int r = 0; r <= ROW; r++) {
			for (int c = 0; c < COLUMN; c++) {
				printf("%c", map[r][c]);
			}
			printf("\n");
		}
		// printf("frogX, frogY: %d, %d\n", frog.x, frog.y); Debug Location

		/*  Check game's status  */
		if (frog.x == 0) {
			game_status = 2; // Winner
		}

		if (row == frog.x) {
			frog.y = (frog.y + addv) % COLUMN;
		}

		if (frog.y == 0 || frog.y == COLUMN - 1) {
			game_status = 4; // Out of Bounds
		}

		pthread_mutex_unlock(&var_lock_1);
		sleep(row, delay);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	// Initialize the river map and frog's starting position
	memset(map, 0, sizeof(map));
	int i, j;
	for (i = 1; i < ROW; ++i) {
		for (j = 0; j < COLUMN - 1; ++j)
			map[i][j] = ' ';
	}

	for (j = 0; j < COLUMN - 1; ++j)
		map[ROW][j] = map[0][j] = '|';

	for (j = 0; j < COLUMN - 1; ++j)
		map[0][j] = map[0][j] = '|';

	frog = Node(ROW, (COLUMN - 1) / 2);
	map[frog.x][frog.y] = '0';

	printf("\033[H");
	printf("\033[2J");

	//Print the map into screen
	for (int r = 0; r <= ROW; r++) {
		puts(map[i]);
	}

	printf("\033[H");

	game_status = 1; //Game exits with status other than 1

	/*  Create pthreads for wood move and frog control.  */
	pthread_mutex_init(&var_lock_1, NULL);
	pthread_attr_t attr;
	pthread_t threads[ROW - 1]; // Thread for each row
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (long i = 0; i < ROW - 1; i++) {
		pthread_create(&threads[i], &attr, logs_move, (void *)i);
	}

	for (long i = 0; i < ROW - 1; i++) {
		pthread_join(threads[i], NULL);
	}

	/*  Display the output for user: win, lose or quit.  */
	if (!game_status) {
		printf("\nYou exit the game.\n");
	} else {
		switch (game_status) {
		case 2:
			printf("\nYou win the game!!\n");
			break;
		case 3: // Lose by falling into river
			printf("\nYou lose the game!!\n");
			//printf("You drowned in the river.\n");
			break;
		case 4: // Lose by touching boundary
			printf("\nYou lose the game!!\n");
			//printf("You touched the boundary.\n");
			break;
		default:
			break;
		}
	}

	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&var_lock_1);

	pthread_exit(NULL);

	return 0;
}
