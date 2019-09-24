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

	void MoveBoard(int DeltaX) 
	{
		if (x + DeltaX < 1 || x+DeltaX+3 > 9)
			return;
		for (int i = 0; i < 4; ++i)
			field[y][x + i] = 0;
		x += DeltaX;
		for (int i = 0; i < 4; ++i)
			field[y][x + i] = 7;

		if (onBoard)
		{
			field[ball_y][ball_x] = 0;
			ball_x += DeltaX;
			field[ball_y][ball_x] = 5;

		}

		display();
	}

	void GetInput()
	{
		if (_kbhit())
		{
			char key = _getch();
			switch (key)
			{
			case 'd': MoveBoard(1); break;
			case 'a': MoveBoard(-1); break;
			case ' ': onBoard = false; x_Dir = 1; y_Dir = 1;  break;

			}
		}
	}

	void display()
	{

		system("cls");
		for (int i = 0; i < 18; ++i)
		{
			for (int j = 0; j < 11; ++j)
				switch (field[i][j])
				{
				case 8: cout << '|'; break;
				case 0: cout << " "; break;
				default: cout << "#"; break;
				}
			cout << endl;
		}
		cout << score << endl;

		if (gameOver)
		{
			system("cls");
			GameOver();
		}

	}

	void GameOver()
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

	void update()
	{
		if (!onBoard)
			{
				field[ball_y][ball_x] = 0;
				if (ball_x + x_Dir == 10 || ball_x + x_Dir == 0)
					x_Dir *= -1;
				if (ball_y - y_Dir == 0 || field[ball_y - y_Dir][ball_x + x_Dir] == 7)
					y_Dir *= -1;
				

				Collide();
			
				if (ball_y == 16)
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
	void Destroy(int _x, int _y)
	{
		field[_y][_x] = 0;
		score += 100;
	}

	void Collide()
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

	void start()
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

	void DrawLevel()
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

	void loop()
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