#include <iostream>
#include <conio.h>
#include <windows.h>


using namespace std;

class BreakOut
{
	int level = 1;
	int score = 0;
	int x_Dir;
	int y_Dir;
	int speed = 2000;
	int field[18][11];
	bool gameOver = false;
	bool onBoard;
	int ball_x = 6;
	int ball_y = 15;
	int x = 4;
	int y = 16;

public:

	void MoveBoard(int DeltaX) // отвечает за передвижение платформы и шарика на нем
	{
		if (x + DeltaX < 1 || x+DeltaX+3 > 9) // убирает возможность выхода платформы за рамки
			return;
		for (int i = 0; i < 4; ++i) // удаляет платформу
			field[y][x + i] = 0;
		x += DeltaX;
		for (int i = 0; i < 4; ++i) // создает платформу на передвинутом месте
			field[y][x + i] = 7;

		if (onBoard) // если шарик лежит на платформе, то двигается с ней
		{
			field[ball_y][ball_x] = 0;
			ball_x += DeltaX;
			field[ball_y][ball_x] = 5;

		}

		display();
	}

	void GetInput() // отвечает за ввод клавиш
	{
		if (_kbhit())
		{
			char key = _getch();
			switch (key)
			{
			case 'd': MoveBoard(1); break; // движение вправо
			case 'a': MoveBoard(-1); break; // движение влево
			case ' ': onBoard = false; x_Dir = 1; y_Dir = 1;  break; // отпускание шарика от платформы

			}
		}
	}

	void display() // отвечает за отобравжение игры
	{

		system("cls");
		for (int i = 0; i < 18; ++i)
		{
			for (int j = 0; j < 11; ++j)
				switch (field[i][j])
				{
				case 8: cout << '|'; break; // границы поля
				case 0: cout << " "; break; // Пустое пространство
				default: cout << "#"; break; // платформа, шарик, плитки
				}
			cout << endl;
		}
		cout << score << endl; // вывод счета

		if (gameOver)
		{
			system("cls");
			GameOver();
		}

	}

	void GameOver() // отвечает за вывод текста при проигрыше и продолжении игры заново
	{
		cout << "Game OVER !!!!!" << endl;
		cout << score << endl;
		cout << "PRESS ANY BUTTON" << endl;
		while (true)
		{
			if (_kbhit())
			{
				loop();
			}
		}
	}

	void update() // отвечает за обновление движения шарика
	{
		if (!onBoard)
			{
				field[ball_y][ball_x] = 0; 
				if (ball_x + x_Dir == 10 || ball_x + x_Dir == 0)// Условие стен для шарика
					x_Dir *= -1;
				if (ball_y - y_Dir == 0 || field[ball_y - y_Dir][ball_x + x_Dir] == 7) // также учитывается платформа
					y_Dir *= -1;
				

				Collide(); // условие плиток для шарика
			
				if (ball_y == 16) // В случае падения шарика
				{
					GameOver();
					return;
				}
				

				ball_x += x_Dir;
				ball_y -= y_Dir;
				field[ball_y][ball_x] = 5;
				display();
		}
		
	}
	void Destroy(int _x, int _y) // уничтожение плитки и зачисление очков
	{
		field[_y][_x] = 0;
		score += 100;
	}

	void Collide() // отвечает за взаимодействие шарика с плитками
	{
		if(field[ball_y][ball_x + x_Dir] == 2 & field[ball_y - y_Dir][ball_x] == 2)
		{
			Destroy(ball_x + x_Dir, ball_y);
			Destroy(ball_x, ball_y - y_Dir);
			y_Dir *= -1;
			x_Dir *= -1;
			return;
		}

		if (field[ball_y][ball_x + x_Dir] == 2)
		{
			Destroy(ball_x + x_Dir, ball_y);
			x_Dir *= -1;
			return;
		}

		if (field[ball_y - y_Dir][ball_x] == 2)
		{
			Destroy( ball_x, ball_y - y_Dir);
			y_Dir *= -1;
			return;
		}
		if (field[ball_y - y_Dir][ball_x + x_Dir] == 2)
		{
			Destroy(ball_x + x_Dir, ball_y - y_Dir);
			y_Dir *= -1;
			x_Dir *= -1;
			return;
		}


	}

	void start() // отвечает за начальную позицию игры
	{
		for (int i = 0; i < 18; ++i)
			for (int j = 0; j < 11; ++j)
			{
				if (i == 17 || i == 0 || j == 10 || j == 0)
					field[i][j] = 8;
				else field[i][j] = 0;
			}
		x = 4;
		y = 16;
		for (int i = 0; i < 4; ++i)
			field[y][x + i] = 7;
		ball_x = 6;
		ball_y = 15;
		field[ball_y][ball_x] = 5;
		DrawLevel();
		display();
	}

	void DrawLevel() // "Прорисовка" уровня
	{
		for (int i = 2; i < 9; ++i)
		{
			field[2][i] = 2;
			field[4][i] = 2;
			field[6][i] = 2;
		}
		for (int i = 3; i < 8; ++i)
		{
			field[3][i] = 2;
			field[5][i] = 2;
		}
		display();
	}

	void loop() //отвечает за бесконечный цикл игры
	{
		level = 1;
		int count = 0;
		onBoard = true;
		start();
		while (!gameOver)
		{
			GetInput();
			if (count < speed)
				count++;
			else
			{
				count = 0;
				update();
			}
			//cout << count << " ";
		}
	}


};

int main() {

	BreakOut A;
	A.loop();


	return 0;
}
