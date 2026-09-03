#include <conio.h>
#include <iostream>
#include <time.h>
#include <windows.h>
#include <thread>
#include <chrono>
using namespace std;

void limpa_tela();
void menu();
void define_tamanho(int level, int &tamanho);
void gotoxy(int XPos, int YPos);
void esconde_cursor();
void pega_input(char &tecla, bool &executando);

// do jogo do rpg de texto
void efeitoTexto(const string& texto, int atrasoMs = 40) {
	for (char c : texto) {
		cout << c << flush;
		this_thread::sleep_for(chrono::milliseconds(atrasoMs));
	}
	cout << endl;
}

void mostrarTitulo() {
	cout << "======================================" << endl;
	this_thread::sleep_for(chrono::milliseconds(500));

	efeitoTexto("         O N E   C A R D   G A M E     ", 50);

	this_thread::sleep_for(chrono::milliseconds(500));
	cout << "======================================" << endl;
	cout << endl;

	this_thread::sleep_for(chrono::milliseconds(800));
}

void esconde_cursor()
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.dwSize = 100;
	cursor.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}

void menu(){
	cout << "1. Novo Jogo " << endl;
	cout << "2. Carregar Jogo " << endl;
	cout << "3. Ajuda " << endl;
	cout << "4. Sair " << endl;
	cout << endl;
	cout << "Escolha uma opção: ";
}

void define_tamanho(int level, int &tamanho)
{
	if (level == 0) 
	{
		tamanho = 11;
	}
	else if(level == 1)
	{
		tamanho = 13;
	}
	else if(level == 2)
	{
		tamanho = 15;
	}
}

// Código Prof
void gotoxy(int XPos, int YPos)
{
	COORD coord;
	coord.X = XPos; // Propriedade console
	coord.Y = YPos;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void limpa_tela()
{
	cout << "\033[H\033[2J"; // limpador de tela universal ANSI -> Stackoverflow
}

void atualiza_jogo()
{

}

void imprime_jogo()
{

}

void pega_input(char &tecla, bool &executando)
{
	if(_kbhit())
	{
		tecla = _getch(); // Escape no ASCII
		if(tecla == 27)
		{
			executando = false;
		} 
	}
}

/*
void loop_jogo(int level)   
{
	// desativa o cursor no console
	int tamanho = 0;
	int player_x = 0;
	int player_y = 0;
	bool nivel_incompleto = true;
	bool executando = true;
	char input = 0;
	
	define_tamanho(level, tamanho);
	esconde_cursor();
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); // handle da saída do console
	
	while(executando && nivel_incompleto)
	{

	}
}
*/

int main() 
{
	mostrarTitulo();
	return 0;
}