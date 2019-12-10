#include <LCD5110_Graph.h>
#include <EEPROM.h>
LCD5110 LCD(9,10,11,12,13);

extern unsigned char SmallFont[];
#define ANALOG_X_pin 0
#define ANALOG_Y_pin 1
#define RIGHT_pin 3
#define LEFT_pin 5
#define UP_pin 2
#define DOWN_pin 4
#define BUTTON_E 6
#define BUTTON_F 7
#define BUTTON_G 8

//Функция сброса
void(* resetFunc) (void) = 0;

uint8_t RotatedNumbers[10][6] // Массив букв на размере 3
{
  { 62, 81, 73, 69, 62, 0},
  { 0, 1, 127, 33, 0, 0},
  { 49, 73, 69, 67, 33, 0},
  { 70, 105, 81, 65, 66, 0},
  { 4, 127, 36, 20, 12, 0},
  { 78, 81, 81, 81, 114, 0},
  { 6, 73, 73, 41, 30, 0},
  { 96, 80, 72, 71, 64, 0},
  { 54, 73, 73, 73, 54, 0},
  { 60, 74, 73, 73, 48, 0}
};



uint8_t R_score[6][6] // Массив чисел в размере 3
{
  {0, 0, 54, 54, 0, 0},
  {12, 21, 21, 21, 14, 0},
  {8, 16, 16, 8, 31, 0},
  {14, 17, 17, 17, 14, 0},
  {2, 17, 17, 17, 14, 0},
  {70, 73, 73, 73, 49, 0}
};

class BreakOut
{
  uint8_t level = 1;
  int score = 0;
  int x_Dir;
  int y_Dir;
  int speed = 1000;// скорость игры
  uint8_t field[18][11];
  bool gameOver = false;
  bool onBoard;
  uint8_t ball_x = 6;
  uint8_t ball_y = 15;
  int x = 4;
  int y = 16;

  //Настройки: размер, контраст, сложность
    uint8_t size = 2;
    uint8_t contrast = 70;
    uint8_t difficulty = 0;
    uint8_t _chosenOption = 0; //Выбранные в данный момент параметры в меню//Выбранные в данный момент параметры в меню
    uint8_t _section = 0;
    //Переменная, которая показывает, играем мы в данный момент или нет
    uint8_t _isPlaying = 0;
    //Таблица рекордов
    String Top1 = "ZZZ";
    uint8_t Top1level = 0;
    int Top1score = 0;

    String Top2 = "ZZZ";
    uint8_t Top2level = 0;
    int Top2score = 0;

    String Top3 = "ZZZ";
    uint8_t Top3level = 0;
    int Top3score = 0;

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
      if (digitalRead(RIGHT_pin)==LOW||(analogRead(ANALOG_X_pin)> 553))
        if(size !=3)
          MoveBoard(1);
        else MoveBoard(-1);
      else if (digitalRead(LEFT_pin)==LOW||(analogRead(ANALOG_X_pin)< 320))
        if(size !=3)
          MoveBoard(-1);
        else MoveBoard(1);

      if ((digitalRead(UP_pin)==LOW||(analogRead(ANALOG_Y_pin)> 553))&&onBoard)
      {
        onBoard = false; x_Dir = 1; y_Dir = 1;
      } 

  }

  void Print(uint8_t i, uint8_t j) // Вывод символов в игре при размере 1,2
    {
      
       if(size == 3)
       {
        
        for (uint8_t _i = 0; _i < size; ++_i)
          for (uint8_t _j = 0; _j < size; ++_j)
              LCD.setPixel(size*i+_i,size*j+_j);
       }
       else {
        for (uint8_t _i = 0; _i < size; ++_i)
          for (uint8_t _j = 0; _j < size; ++_j)
              LCD.setPixel(size*j+_j,size*i+_i);
       }
    }

  void display() // отвечает за отображение игры
  {

    LCD.clrScr();
    for (int i = 0; i < 18; ++i)
    {
      for (int j = 0; j < 11; ++j)
        switch (field[i][j])
        {
        case 8: Print(i,j); break; // границы поля
        case 0:  break; // Пустое пространство
        default: Print(i,j); break; // платформа, шарик, плитки
        }
      //cout << endl;
    }
    //cout << score << endl; // вывод счета
    if(size == 3)
      {
        for(int _i = 0; _i < 6; ++_i)
          r_Print(5 - _i, 62, 40 - _i*6, false);
          

        int temp = score;
        int del = 1;
        uint8_t count = 1;
        while(temp / 10 != 0)
        {
          temp /= 10;
          count++;
          del *= 10;
        }
        temp = score;

        for(uint8_t _i = 0; _i < count; ++ _i)
        {
          
          r_Print(temp / del, 70, 40 - _i*6, true);
          
          temp /= del ;
          del /= 10;
        }

        LCD.update();
      }
      else {
        LCD.print("Score:",13*size,size*2);
        LCD.printNumI(score,13*size + 35,size*2);
      }

    if (gameOver)
    {
      LCD.clrScr();
      GameOver();
    }
    LCD.update();
  }

  void r_Print(uint8_t num, uint8_t _x, uint8_t _y, bool cond) // Вывод символов при размере 3
    {
      
      for(uint8_t _i = 0; _i < 7; ++_i)
          for(uint8_t _j = 0; _j < 6; ++_j)
            {
              uint8_t** tmp = deCode(num, cond);
              if(tmp[_j][_i] == 1)
              LCD.setPixel(_x + _i,_y + _j);

              for(int i = 0; i < 6; ++i)
                delete tmp[i];
              delete[] tmp;
            }

      
    }

    uint8_t** deCode(uint8_t num, bool cond)
    {
      uint8_t** tempMassive;
      tempMassive = new uint8_t*[6];
      for(int i = 0; i < 6; ++i)
        tempMassive[i] = new uint8_t[7];
        
      for(int i = 0; i < 6; ++i)
      {
        uint8_t tmp;
        if(cond)
          tmp = RotatedNumbers[num][i];
          else tmp = R_score[num][i];
        for(int j = 0; j < 7; ++j)
          {
            tempMassive[i][6-j] = tmp % 2;
              tmp /= 2;
            
          }
      }

          return tempMassive;
    }

  void GameOver()
    {
      //Выводим информацию: уровень и очки
      LCD.clrScr();
      LCD.print("Game Over",0,0);
      LCD.print("Level:",0,8);
      LCD.printNumI(level,40,8);
      
      LCD.print("Score:",0,16);
      LCD.printNumI(score,40,16);
      LCD.print("Press Any Key",0,24);
      LCD.print("To Continue",0,32);
      LCD.update();
      //Бесконечный цикл, ждущий ввода
      while (true)
        if (digitalRead(UP_pin)==LOW||digitalRead(DOWN_pin)==LOW||digitalRead(LEFT_pin)==LOW||digitalRead(RIGHT_pin)==LOW||digitalRead(BUTTON_E)==LOW||digitalRead(BUTTON_F)==LOW||digitalRead(BUTTON_G)==LOW)
        {
          String _name = "AAA";
          uint8_t _chosenChar = 0;
          while (true)
          {
            //Ввод имени игрока
            if(digitalRead(LEFT_pin)==LOW)
              if(_chosenChar == 0)
                _chosenChar = 2;
                else _chosenChar -= 1;

            if(digitalRead(RIGHT_pin)==LOW)
              if(_chosenChar == 2)
                _chosenChar = 0;
                else _chosenChar += 1;
                
            if(digitalRead(DOWN_pin)==LOW)
              if(_name[_chosenChar] == 'A')
                _name[_chosenChar] = 'Z';
                else _name[_chosenChar] -= 1;

            if(digitalRead(UP_pin)==LOW)
              if(_name[_chosenChar] == 'Z')
                _name[_chosenChar] = 'A';
                else _name[_chosenChar] += 1;
 
            LCD.clrScr();
            LCD.print("Enter Name:",0,0);
            LCD.print(_name,64,0);
            LCD.drawRect(64 + _chosenChar*6,8,70 + _chosenChar*6,10);

            LCD.print("Press G",0,12);
            LCD.print("To Try Again",0,20);
            LCD.print("Press E,F",0,28); 
            LCD.print("To Menu",0,36);

            

            LCD.update();
            delay(200);
            //Далее просчитываем, надо ли менять позиции в таблице рекордов и по кнопке G начинаем игру заново
            //А по кнопкам E и F переходим в меню

            if (digitalRead(BUTTON_G)==LOW)
            {
              
              delay(1000);
              CalcRecords(_name, level, score);
              
              gameOver = false;
              LCD.clrScr();
              circleOfGame();//!
            }
            if (digitalRead(BUTTON_E)==LOW||digitalRead(BUTTON_F)==LOW)
            {
              LCD.clrScr();
              CalcRecords(_name, level, score);
              _isPlaying = 0;
              gameOver = false;
              //return;
              LCD.update();
              delay(1000);
              resetFunc();
              
              
            }
          }
        }
    }

    void CalcRecords(String _name, int _level, int _score)
    {
      
      if (_score >= Top1score)
      {
        if (_score == Top1score)
        {
          if (_name.compareTo(Top1) < 0)
          {
            for (int i = 0; i<3;++i)
              Top3[i] = Top2[i];
            Top3score = Top2score;
            Top3level = Top2level;
  
            for (int i = 0; i<3;++i)
              Top2[i] = Top1[i];
            Top2score = Top1score;
            Top2level = Top1level;
            
            for (int i = 0; i<3;++i)
              Top1[i] = _name[i];
            Top1score = _score;
            Top1level = _level;
          }
          else if(_name.compareTo(Top1) > 0)
          {
            for (int i = 0; i<3;++i)
              Top3[i] = Top2[i];
            Top3score = Top2score;
            Top3level = Top2level;
  
            for (int i = 0; i<3;++i)
              Top2[i] = _name[i];
            Top2score = _score;
            Top2level = _level;
          }
          else if (_name.compareTo(Top1) == 0)
          {
            for (int i = 0; i<3;++i)
              Top1[i] = _name[i];
            Top1score = _score;
            Top1level = _level;
          }
        }
        else
        {
            for (int i = 0; i<3;++i)
              Top3[i] = Top2[i];
            Top3score = Top2score;
            Top3level = Top2level;
  
            for (int i = 0; i<3;++i)
              Top2[i] = Top1[i];
            Top2score = Top1score;
            Top2level = Top1level;
            
            for (int i = 0; i<3;++i)
              Top1[i] = _name[i];
            Top1score = _score;
            Top1level = _level;
        }
      }
      else if (_score >= Top2score)
      {
        if (_score == Top2score)
        {
          if(_name.compareTo(Top2) < 0)
          {
            for (int i = 0; i<3;++i)
              Top3[i] = Top2[i];
            Top3score = Top2score;
            Top3level = Top2level;
  
            for (int i = 0; i<3;++i)
              Top2[i] = _name[i];
            Top2score = _score;
            Top2level = _level;
          }
          else if(_name.compareTo(Top2) > 0)
          {
            for (int i = 0; i<3;++i)
              Top3[i] = _name[i];
            Top3score = _score;
            Top3level = _level;
          }
          else if(_name.compareTo(Top2) == 0)
          {
            for (int i = 0; i<3;++i)
              Top2[i] = _name[i];
            Top2score = _score;
            Top2level = _level;
          }
        }
        else
        {
            for (int i = 0; i<3;++i)
              Top3[i] = Top2[i];
            Top3score = Top2score;
            Top3level = Top2level;
  
            for (int i = 0; i<3;++i)
              Top2[i] = _name[i];
            Top2score = _score;
            Top2level = _level;
        }
      }
      else if (_score >= Top3score)
      {
            for (int i = 0; i<3;++i)
              Top3[i] = _name[i];
            Top3score = _score;
            Top3level = _level;
      }

      //Запоминаем топ3 в памяти
      for (int i = 0; i < 3; ++i)
        EEPROM.write(237 + i*9, Top1[i]);
      for (int i = 0; i < 3; ++i)
        EEPROM.write(362 + i*9, Top2[i]);
      for (int i = 0; i < 3; ++i)
        EEPROM.write(487 + i*9, Top3[i]);

      EEPROM.put(111, Top1score);
      EEPROM.put(128, Top2score);
      EEPROM.put(145, Top3score);
      EEPROM.put(162, Top1level);
      EEPROM.put(171, Top2level);
      EEPROM.put(180, Top3level);
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

  void circleOfGame()  //отвечает за бесконечный цикл игры
  {
    level = 1;
    //int count = 0;
    onBoard = true;
    LCD.setContrast(contrast);
      speed = 1000;
      speed -= difficulty * 200;

    
    start();
    while (!gameOver)
    {
      GetInput();
      update();
      //cout << count << " ";
      delay(speed);
      if(digitalRead(BUTTON_F)==LOW)
        {
          ShowMenu();
          delay(1000);
        }
    }
    

  }

  
    void SetVals()//Метод ввода настроек из памяти//Метод ввода настроек из памяти
    {
      size = 2;
      difficulty = 0;
      contrast = 70;

      Top1 = "ZZZ";
      Top1level = 0;
      Top1score = 0;

      Top2 = "ZZZ";
      Top2level = 0;
      Top2score = 0;

      Top3 = "ZZZ";
      Top3level = 0;
      Top3score = 0;
      
      if (EEPROM.read(0) >= 1 && EEPROM.read(0) <= 3)
        EEPROM.get(0,size);
      if (EEPROM.read(9) >= 0 && EEPROM.read(9) <= 3)
        EEPROM.get(9,difficulty);
      if (EEPROM.read(18) >= 50 && EEPROM.read(18) <= 70 && EEPROM.read(18) % 2 == 0)
        EEPROM.get(18,contrast);

        int flag = 0;
        for (int i = 0; i < 3; ++i)
          if(EEPROM.read(237 + i*9) >= 'A' && EEPROM.read(237 + i*9) <= 'Z')
            flag++;
            
        for (int i = 0; i < 3; ++i)
        if(flag == 3)
          EEPROM.get(237 + i*9, Top1[i]);

        flag = 0;
        for (int i = 0; i < 3; ++i)
          if(EEPROM.read(362 + i*9) >= 'A' && EEPROM.read(362 + i*9) <= 'Z')
            flag++;
          
        for (int i = 0; i < 3; ++i)
        if(flag == 3)
          EEPROM.get(362 + i*9, Top2[i]);

        flag = 0;
        for (int i = 0; i < 3; ++i)
          if(EEPROM.read(487 + i*9) >= 'A' && EEPROM.read(487 + i*9) <= 'Z')
            flag++;
          
        for (int i = 0; i < 3; ++i)
        if(flag == 3)
          EEPROM.get(487 + i*9, Top3[i]);
        
      if(EEPROM.read(111) >= 0 && EEPROM.read(111) <= 32000)
        EEPROM.get(111, Top1score);
      if(EEPROM.read(128) >= 0 && EEPROM.read(128) <= 32000)
        EEPROM.get(128, Top2score);
      if(EEPROM.read(145) >= 0 && EEPROM.read(145) <= 32000)
        EEPROM.get(145, Top3score);

      if(EEPROM.read(162) >= 0 && EEPROM.read(162) <= 127)
        EEPROM.get(162, Top1level);
      if(EEPROM.read(171) >= 0 && EEPROM.read(171) <= 127)
        EEPROM.get(171, Top2level);
      if(EEPROM.read(180) >= 0 && EEPROM.read(180) <= 127)
        EEPROM.get(180, Top3level);
    }

    int ShowMenu() // Полная реализация меню
    {
      while(true)
      {
        //Меняем выбранную опцию кнопками вверх и вниз
        if(digitalRead(UP_pin)==LOW)
          if(_chosenOption == 0)
            _chosenOption = 3;
            else _chosenOption -= 1;

        if(digitalRead(DOWN_pin)==LOW)
          if(_chosenOption == 3)
            _chosenOption = 0;
            else _chosenOption += 1;
        LCD.clrScr();
        LCD.drawCircle(13,8 + _chosenOption*8,3);
        LCD.drawCircle(71,8 + _chosenOption*8,3);
        //Вывод начальной панели меню
        if(_section == 0)
        {
              LCD.print("GO",36,5);
              LCD.print("TOP",32,13);
              LCD.print("SETTINGS",18,21);
              LCD.print("EXIT",30,29);
              LCD.update();
              if(digitalRead(BUTTON_E)==LOW||digitalRead(BUTTON_G)==LOW)
                //Действия для кнопки START
                if(_chosenOption == 0)
                {
                  //Начинаем игру
                  LCD.clrScr();
                  _isPlaying = 1;
                  circleOfGame();
                }
                //Для кнопки RECORDS
                else if(_chosenOption == 1)
                {
                  //Переходим к таблице рекордов
                  LCD.clrScr();
                  _section = 2;
                  _chosenOption = 0;
                  delay(100);
                }
                //Для кнопки SETTINGS
                else if(_chosenOption == 2)
                {
                  //Переходим к меню настроек
                  LCD.clrScr();
                  _section = 1;
                  _chosenOption = 0;
                  delay(100);
                }
                //Для кнопки EXIT
                else if(_chosenOption == 3)
                {
                  //Выводим прощальное сообщение и выключаем экран
                  LCD.clrScr();
                  LCD.print("Goodbye!",0,0);
                  LCD.update();
                  delay(1000);
                  LCD.clrScr();
                  LCD.update();
                  LCD.enableSleep();
                  _isPlaying = 5;
                }
        }
        //Вывод меню настроек
        else if(_section == 1)
        {
              
              LCD.print("BACK",8,5);
              LCD.print("SIZE",8,13);
              LCD.printNumI(size,76,13);
              LCD.print("DIFFICULT",8,21);
              LCD.printNumI(difficulty,76,21);
              LCD.print("CONTRAST",8,29);
              LCD.printNumI(contrast,70,29);
              LCD.update();
              if(digitalRead(BUTTON_E)==LOW||digitalRead(BUTTON_G)==LOW)
                if(_chosenOption == 0)
                {
                  //Возвращаемся в изначальное меню
                  LCD.clrScr();
                  _section = 0;
                  _chosenOption = 0;
                  delay(300);
                }
                if(_chosenOption == 1)
                {
                  //Меняем размер от 1 до 3
                  if(digitalRead(RIGHT_pin)==LOW)
                  if(size == 3)
                    size = 1;
                  else size++;
                else if(digitalRead(LEFT_pin)==LOW)
                  if(size == 1)
                    size = 3;
                  else size--;
                  EEPROM.put(0, size);
                }
                else if(_chosenOption == 2)
                {
                  //Меняем сложность(скорость) от 0 до 3
                  if(digitalRead(RIGHT_pin)==LOW)
                  if(difficulty == 3)
                    difficulty = 0;
                  else difficulty++;
                else if(digitalRead(LEFT_pin)==LOW)
                  if(difficulty == 0)
                    difficulty = 3;
                  else difficulty--;
                  EEPROM.put(9, difficulty);
                }
                else if(_chosenOption == 3)
                {
                  //Меняем контраст
                  if(digitalRead(RIGHT_pin)==LOW)
                  if(contrast == 70)
                    contrast = 50;
                    else contrast += 2;
                else if(digitalRead(LEFT_pin)==LOW)
                  if(contrast == 50)
                    contrast = 70;
                    else contrast -= 2;
                    LCD.setContrast(contrast);

                   EEPROM.put(18, contrast);
                }
        }
        //Вывод таблицы рекордов
        else if (_section == 2)
        {
          LCD.print("BACK",8,5);
          LCD.print("1]",8,13);
          LCD.print(Top1,20,13);
          LCD.printNumI(Top1level,40,13);
          LCD.printNumI(Top1score,47,13);

          LCD.print("2]",8,21);
          LCD.print(Top2,20,21);
          LCD.printNumI(Top2level,40,21);
          LCD.printNumI(Top2score,47,21);

          LCD.print("3]",8,29);
          LCD.print(Top3,20,29);
          LCD.printNumI(Top3level,40,29);
          LCD.printNumI(Top3score,47,29);

          LCD.update();
          if(digitalRead(BUTTON_E)==LOW||digitalRead(BUTTON_F)==LOW||digitalRead(BUTTON_G)==LOW)
                if(_chosenOption == 0)
                {
                  LCD.clrScr();
                  _section = 0;
                  _chosenOption = 0;
                  delay(300);
                }
        }
        delay(100);        
      }
    }


};
BreakOut breakout;

void setup() {
  
  pinMode(UP_pin, INPUT_PULLUP);
  pinMode(DOWN_pin, INPUT_PULLUP);
  pinMode(RIGHT_pin, INPUT_PULLUP);
  pinMode(LEFT_pin, INPUT_PULLUP);
  pinMode(BUTTON_E, INPUT_PULLUP);
  pinMode(BUTTON_F, INPUT_PULLUP);
  pinMode(BUTTON_G, INPUT_PULLUP);

    LCD.InitLCD();
  LCD.setFont(SmallFont);
  randomSeed(analogRead(0));

  breakout.SetVals();
  breakout.ShowMenu();
}

void loop() {
  
  LCD.clrScr();
  breakout.ShowMenu();
}
