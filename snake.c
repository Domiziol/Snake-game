#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include "winbgi2.h"


void init(int* x, int* y, int n, int r); //inicjalizacja węża
void head(int a, int b, int r); //rysowanie głowy w innym miejscu
void body(int* x, int* y, int n); //przesuwanie ciała
void tail(int a, int b, int r); //usuwanie starego ogona
void move(int* x, int* y, int n, int r, int c); //poruszanie wężem w zależności od podanego kierunku
void losuj(int* a, int* b); //losowanie współrzędnych dla jedzenia
void edges_check(int* x, int* y, int* w, int* re, FILE* f); //sprawdzenie czy wąż wyszedł poza okno graficzne
void body_check(int* x, int* y, int n, int* w, int* re, FILE* f); //sprawdzenie czy głowa węża nie zderza się z ciałem
void end(int* re, int* w, FILE* f); //zakonczenie rozgrywki

void main()
{
	MoveWindow(GetConsoleWindow(), 300, 100, 800, 500, TRUE); 
	int a = 0, b = 0, k = 1; //zmienne pomocnicze
	int r = 5; //promień koła (części węża i jedzenia)
	int n = 5; //liczba elementów węża
	int c = 68; //domyślny kierunek ruchu (68 to "D" w ASCII)
	int x[485], y[485]; //tablice przechowujące współrzędne każdego elementu węża
	int* p = &a;
	int* g = &b;
	int rec = 0; //przechowuje aktualny rekord
	int points = 0; //przechowuje aktualną liczbę punktów w każdej rozgrywce
	int* w = &points;


	FILE* f = fopen("wynik.txt", "r");
	if (f == NULL)
	{
		f = fopen("wynik.txt", "w");	//jeśli plik z rekordami nie istnieje to utworzenie go
		fclose(f);
	}
	else
	{
		while (!feof(f))				//wczytanie największego rekordu z pliku z wynikami
		{
			fscanf(f, "%d", &b);
			if (b > rec)
				rec = b;
		}
		fclose(f);
	}
	int* re = &rec;

	graphics(220, 220);					//rysowanie okna graficznego i kolorowanie go na zielono
	setfillstyle(SOLID_FILL, GREEN);
	floodfill(1, 1, WHITE);

	init(x, y, n, r);
	srand(time(NULL));

	losuj(p, g);						//rysowanie jedzenia w losowym miejscu
	k = 0;
	while (k < n)
	{
		if (x[k] == a && y[k] == b)
		{
			losuj(p, g);
			k = -1;
		}
		k++;
	}
	setcolor(RED);
	circle(a, b, r);
	setfillstyle(SOLID_FILL, LIGHTMAGENTA);
	floodfill(a, b, BLUE);

	Sleep(1000);

	while (1)
	{
		if (kbhit())		//sprawdzenie czy użytkownik wcisnął przycisk
		{
			c = getch();	//wczytanie jaki to przycisk
		}					//obsługiwane są W A S D

		move(x, y, n, r, c); 

		if (x[0] == a && y[0] == b)				//sprawdzenie czy głowa najeżdża na jedzenie
		{										
			n++;								//zwiększenie liczby elementów węża i zdobytych punktów
			points++;
			x[n - 1] = x[n - 2];
			y[n - 1] = y[n - 2];
			circle(x[n - 1], y[n - 1], r);
			setfillstyle(SOLID_FILL, BLUE);
			floodfill(x[n - 1], y[n - 1], BLUE);

			losuj(p, g);						
			k = 0;
			while (k < n)
			{
				if (x[k] == a && y[k] == b)
				{
					losuj(p, g);
					k = -1;
				}
				k++;
			}
			setcolor(RED);						//rysowanie nowego jedzenia
			circle(a, b, r);
			setfillstyle(SOLID_FILL, LIGHTMAGENTA);
			floodfill(a, b, BLUE);
		}
		printf("%d\r", points);				//wyświetlanie aktualnego wyniku
		edges_check(x, y, w, re, f);
		body_check(x, y, n, w, re, f);

		Sleep(200);
	}

	wait();
}
void init(int* x, int* y, int n, int r)
{
	setcolor(BLACK);
	x[0] = 40;					//przypisanie współrzędnych pierwszemu wężowi
	x[1] = 30;
	x[2] = 20;
	x[3] = 10;
	x[4] = 0;
	y[0] = y[1] = y[2] = y[3] = y[4] = 10;

	for (int i = 0; i < n; i++)	//rysowanie początkowego węża
	{
		circle(x[i], y[i], r);
		setfillstyle(SOLID_FILL, BLUE);
		floodfill(x[i], y[i], BLUE);
	}
	for (int i = n; i < 485; i++) //pozostałe współrzędne przyjmują wartość "-1"
	{
		x[i] = -1;
		y[i] = -1;
	}
}

void head(int a, int b, int r)
{
	setcolor(BLACK);			//rysowanie nowego koła (głowy)
	circle(a, b, r);
	setfillstyle(SOLID_FILL, BLUE);
	floodfill(a, b, BLUE);
}

void body(int* x, int* y, int n)
{
	for (int i = n - 1; i > 0; i--) //przepisywanie współrzędnych do kolejnych elementów węża
	{
		x[i] = x[i - 1];
		y[i] = y[i - 1];
	}
}

void tail(int a, int b, int r)
{
	setcolor(0.5);			//usunięcie ogona, rysowanie zielonego (kolor tła) koła w jego miejsce 
	circle(a, b, r);
	floodfill(a, b, BLUE);
}

void move(int* x, int* y, int n, int r, int c)
{
	if (c == 68 || c == 100) //ruch w prawo, "d" lub "D"
	{
		tail(x[n - 1], y[n - 1], r);
		body(x, y, n);
		x[0] += 2 * r;
		head(x[0], y[0], r);
	}
	if (c == 115 || c == 83) //ruch w dół, "s" lub "S"
	{
		tail(x[n - 1], y[n - 1], r);
		body(x, y, n);
		y[0] += 2 * r;
		head(x[0], y[0], r);
	}
	if (c == 97 || c == 65) //ruch w lewo, "a" lub "A"
	{
		tail(x[n - 1], y[n - 1], r);
		body(x, y, n);
		x[0] -= 2 * r;
		head(x[0], y[0], r);
	}
	if (c == 87 || c == 119) //ruch w górę, "w" lub "W"
	{
		tail(x[n - 1], y[n - 1], r);
		body(x, y, n);
		y[0] -= 2 * r;
		head(x[0], y[0], r);
	}
}

void losuj(int* a, int* b)
{
	*a = 1 + rand() % 20; //współrzędne mają być z przedziału 1 - 20
	*b = 1 + rand() % 20;
	*a *= 10;			  //mnożenie *2r żeby jedzenie zostało "zjedzone w całości"
	*b *= 10;
}

void edges_check(int* x, int* y, int* w, int* re, FILE* f)
{
	if (x[0] > 210) //wyjście poza prawą krawędź
	{
		end(re, w, f);
	}
	if (x[0] <= 0) //wyjście poza lewą krawędź
	{
		end(re, w, f);
	}
	if (y[0] >= 210) //wyjście poza dolną krawędź
	{
		end(re, w, f);
	}
	if (y[0] < 10) //wyjście poza górną krawedź
	{
		end(re, w, f);
	}
}

void body_check(int* x, int* y, int n, int* w, int* re, FILE* f)
{
	int k = 1;
	while (k < n)
	{
		if (x[0] == x[k])	//jeśli głowa jest w tym samym miejscu co część ciała
			if (y[0] == y[k])
			{
				end(re, w, f);
			}
		k++;
	}
}
void end(int* re, int* w, FILE* f)
{
	if (*w == 1)
	{
		printf("GAME OVER\nYou scored %d point\n\n", *w);
	}
	else printf("GAME OVER\nYou scored %d points\n\n", *w);
	if (*w > * re)												//oznacza że rekord został właśnie pobity i zapisany do pliku "wyniki.txt"
	{
		printf("Congratulations!!! You have a new record!!\n");
		f = fopen("wynik.txt", "a");
		fprintf(f, "%d\n", *w);
		fclose(f);
	}
	exit(1);
}
