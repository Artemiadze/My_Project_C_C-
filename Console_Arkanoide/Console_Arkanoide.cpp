#define _CRT_SECURE_NO_WARNINGS
#define M_PI 3.14159265358979323846   // pi

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <Windows.h>
#include <math.h>


#define stroki 25
#define stolb 65

char mas[stroki][stolb + 1];
int point = 0, maxpoint = 0;
//создание ракетки
typedef struct {
	int x, y;
	int w; //ширина
}TRacket;
TRacket racket;

//её инициализация
void initRacket()
{
	racket.w = 8;
	racket.x = (stolb - racket.w) / 2;
	racket.y = stroki - 2;
}

//помещение  ракетки в поле
void PutRacket()
{
	for (int i = racket.x; i < racket.x + racket.w; i++) {
		mas[racket.y][i] = '@';
	}
}
//мячик
typedef struct {
	float x, y;
	int ix, iy;
	float alfa;
	float speed;
}TBall;
TBall  ball;

void move_ball(float x, float y) {
	ball.x = x;
	ball.y = y;
	ball.ix = (int)round(ball.x);
	ball.iy = (int)round(ball.y);
}

void init_ball() {
	move_ball(2, 2);
	ball.alfa = -1;
	ball.speed = 0.5;
}

void put_ball() {
	mas[ball.iy][ball.ix] = '*';
}

void automove_ball() {
	if (ball.alfa < 0) ball.alfa += M_PI * 2;
	if (ball.alfa > M_PI) ball.alfa -= M_PI * 2;
	if (mas[ball.iy + 1][ball.ix] == '@') {
		point += 1;
		Sleep(0.001);
	}

	TBall b1 = ball;
	move_ball(ball.x + cos(ball.alfa) * ball.speed, ball.y + sin(ball.alfa) * ball.speed);

	if ((mas[ball.iy][ball.ix] == '#') || (mas[ball.iy][ball.ix] == '@'))
	{
		if ((ball.ix != b1.ix) && (ball.iy != b1.iy))
		{
			if (mas[b1.iy][b1.ix] == mas[ball.iy][ball.ix])
				b1.alfa = b1.alfa + M_PI;
			else
			{
				if (mas[b1.iy][b1.ix] == '#') {
					b1.alfa = (2 * M_PI - b1.alfa) + M_PI;
				}
				else {
					b1.alfa = (2 * M_PI - b1.alfa);
				}
			}
		}
		else if (ball.iy == b1.iy)
		{
			b1.alfa = (2 * M_PI - b1.alfa) + M_PI;
		}
		else
			b1.alfa = (2 * M_PI - b1.alfa);

		ball = b1;
		automove_ball();
	}
}
//создание поля
void init()
{
	for (int i = 0; i < stolb; i++) {
		mas[0][i] = '#';
	}
	mas[0][stolb] = '\0';
	for (int i = 1; i < stroki; i++) {
		strncpy(mas[i], mas[0], stolb);
	}

	//пустые места
	for (int i = 1; i < stroki - 1; i++) {
		for (int j = 1; j < stolb - 1; j++) {
			mas[i][j] = ' ';
		}
	}
}

//функция перемещения ракетки
void move_racket(int x) {
	racket.x = x;
	if (racket.x < 1)
		racket.x = 1;
	else if (racket.x + racket.w > stolb)
		racket.x = stolb - 1 - racket.w;
}
void print()
{
	for (int i = 0; i < stroki; i++)
	{
		printf("%s", mas[i]);
		if (i == 4) printf("     points: %d  ", point / 5);
		if (i == 5) {
			if (point / 5 > maxpoint) maxpoint = point / 5;
			printf("     Max number: %d", maxpoint);
		}
		if (i < stroki - 1) printf("\n");

	}
}

//вывод курсора без мигания
void setcur(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
int main() {
	char knopka;
	int flag = 0;
	BOOL run = FALSE;

	initRacket();
	init_ball();
	do {
		setcur(0, 0);
		if (flag != 1)
			printf("W - start the game. ECS - excape the game \n");
		if (run)
			automove_ball();
		if (ball.iy > stroki - 3) {
			run = FALSE;
			point = 0;
		}
		init();
		PutRacket();
		put_ball();
		print();

		if (GetKeyState('A') < 0) move_racket(racket.x - 1);
		if (GetKeyState('D') < 0) move_racket(racket.x + 1);
		if (GetKeyState('W') < 0)
		{
			run = TRUE;
		}
		if (!run)
			move_ball(racket.x + racket.w / 2, racket.y - 1);
		Sleep(10);

	} while ((GetKeyState(VK_ESCAPE) >= 0));

	return 0;
}

