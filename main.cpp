#include <conio.h>
#include <iostream>
#include <time.h>
#include <windows.h>
#include <thread>
#include <chrono>
using namespace std;

#define MAX_TAMANHO 30
struct Player
{
	int x;
	int y;
	bool em_alavanca;
};

struct Nivel
{
	int tamanho;
	int rotacao;
	bool alavanca_ativa;
	bool nivel_completo;
	bool perdeu;
};

void ajuda();
void atualizaJogo(int mapa[MAX_TAMANHO][MAX_TAMANHO], char tecla, Nivel &nivel, Player &player, bool &executando);
void aplicaGravidade(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, Player &player);
void carregaMapa(int level, int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, Player &player);
void copiaMatriz(int matriz[MAX_TAMANHO][MAX_TAMANHO], int copia[MAX_TAMANHO][MAX_TAMANHO], int tamanho);
void defineTamanho(int level, Nivel &nivel);
void efeitoTexto(const string &texto, int atraso_ms = 40);
void escondeCursor();
void gotoxy(int XPos, int YPos);
void imprimeJogo(const int mapa[MAX_TAMANHO][MAX_TAMANHO], const Nivel &nivel, const Player &player);
void limpaTela();
void loopJogo(int level, bool &jogo_salvo);
void menu();
void mostrarTitulo();
void pegaInput(char &tecla, bool &executando);
void processaMenu(bool &executando, bool &jogo_em_andamento, int &level);
void rotacionaMapa(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, Player &player, char direcao);
void sobre();
void zeraMatriz(int matriz[MAX_TAMANHO][MAX_TAMANHO]);

// do jogo do rpg de texto
void efeitoTexto(const string &texto, int atraso_ms)
{
	for (char c : texto) {
		cout << c << flush;
		this_thread::sleep_for(chrono::milliseconds(atraso_ms));
	}
	cout << endl;
}

void mostrarTitulo() 
{
	cout << "======================================" << endl;
	this_thread::sleep_for(chrono::milliseconds(500));

	efeitoTexto("         O N E   C A R D   G A M E     ", 50);

	this_thread::sleep_for(chrono::milliseconds(500));
	cout << "======================================" << endl;
	cout << endl;

	this_thread::sleep_for(chrono::milliseconds(800));
}

void escondeCursor()
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
	cout << "4. Sobre " << endl;
	cout << endl;
	cout << "Escolha uma opção "<< endl;
	cout << endl;
	cout << "Pressione ESC para sair do jogo" << endl;
}

void sobre(){
	limpaTela();
	cout << "Equipe de desenvolvimento:\n- Pedro Henrique R. J. "
		"Nicolini\n- "
		"Luiz Eduardo F. N. Goncalves\n";
	cout << "Setembro/2026\n";
	cout << "Professor: Thiago Felski\nDisciplina: Algoritmos e "
		"Programacao II\n\n";
	cout << "Pressione qualquer tecla para voltar ao menu...";
	getch();
}

void ajuda(){
    limpaTela();

    cout << "======= AJUDA =======" << endl;
    cout << endl;

    cout << "CONTROLES:" << endl;
    cout << "W A S D - Mover" << endl;
    cout << "Q / E   - Girar cenario (na alavanca)" << endl;
    cout << "R       - Reiniciar fase" << endl;
    cout << "ESC     - Voltar ao menu" << endl;
    cout << endl;

    cout << "REGRAS:" << endl;
    cout << "- Nao atravessa paredes, blocos ou portas fechadas." << endl;
    cout << "- Rotacao gira tudo: paredes, blocos, portas, saida e jogador." << endl;
    cout << "- Portas fechadas bloqueiam e sustentam blocos; abertas nao." << endl;
    cout << "- Apos girar, blocos caem ate encontrar apoio." << endl;
    cout << "- Porta fechando sobre bloco: bloco e destruido." << endl;
    cout << "- Porta fechando sobre jogador: fase e perdida." << endl;
    cout << "- Objetivo: alcancar a saida (S)." << endl;
    cout << endl;

    cout << "Pressione qualquer tecla para voltar..." << endl;
    getch();
}

void processaMenu(bool &executando, bool &jogo_salvo, int &level){
	char tecla = 0;
	
	while (true) {
		if (_kbhit()) 
		{
			tecla = _getch();

			if (tecla >= '1' && tecla <= '4') 
			{
				switch (tecla) 
				{
					case '1': 
						level = 0;
						jogo_salvo = false;
						loopJogo(level, jogo_salvo);
						break;
					case '2': 
						if(jogo_salvo)
						{
							loopJogo(level, jogo_salvo);
						}
						break;
					case '3': 
						ajuda(); 
						break;
					case '4': 
						sobre(); 
						break;
				}
					break;
				}
				else if (tecla == 27) { // ESC
					executando = false;
					break;
			}

		this_thread::sleep_for(chrono::milliseconds(30));
		}
	}
}

void defineTamanho(int level, Nivel &nivel)
{
	if (level == 0) 
	{
		nivel.tamanho = 11;
	}
	else if(level == 1)
	{
		nivel.tamanho = 13;
	}
	else if(level == 2)
	{
		nivel.tamanho = 15;
	}
}

// Código Prof
void gotoxy(HANDLE h, int XPos, int YPos)
{
	COORD coord;
	coord.X = XPos; // Propriedade console
	coord.Y = YPos;
	SetConsoleCursorPosition(h, coord);
}

void limpaTela()
{
	cout << "\033[H\033[2J"; // limpador de tela universal ANSI -> Stackoverflow
}

void atualizaJogo(int mapa[MAX_TAMANHO][MAX_TAMANHO], char tecla, Nivel &nivel, Player &player, bool &executando)
{
	int prox_x = player.x;
	int prox_y = player.y;

	if (tecla == 'w' || tecla == 'W') 
	{
		prox_y--;
	}
	else if (tecla == 'a' || tecla == 'A') 
	{
		prox_x--;
	}
	else if (tecla == 's' || tecla == 'S') 
	{
		prox_y++;
	}
	else if (tecla == 'd' || tecla == 'D') 
	{
		prox_x++;
	}
	else if (tecla == 'r' || tecla == 'R')
        {
                // TODO: Restart
                return;
        }
	else if((tecla == 'q' || tecla == 'Q' || tecla == 'e' || tecla == 'E') && player.em_alavanca)
	{
		rotacionaMapa(mapa, nivel.tamanho, player, tecla);
		if (tecla == 'q' || tecla == 'Q')
		{
			nivel.rotacao = nivel.rotacao + 270 % 360; // faz não ficar com rotação maior que 360
		}
		else
		{
			nivel.rotacao = nivel.rotacao + 90 % 360;
		}
	}
	// TODO: Validar prox_x e prox_y nos limites.
        // TODO: Verificação de colisão:
        //       - Parede / Porta Fechada -> não move
        //       - Alavanca (4): Update player.em_alavanca = true e move player salvando que a alavanca tá embaixo.
        //       - Saída (3): Seta nivel.nivel_completo = true.
        //       - Vazio (0): Move player (player.x = prox_x; player.y = prox_y; player.em_alavanca = false).
}

void imprimeJogo(HANDLE h, const int mapa[MAX_TAMANHO][MAX_TAMANHO], const Nivel &nivel, const Player &player)
{
	// TODO:  gotoxy(h, 0, 0).
        // TODO: itera [0..nivel.tamanho) e mapeia int -> ASCII:
        //       - 0: Vazio (' ')
        //       - 1: Parede ('#')
        //       - 2: Jogador ('@')
	//       - 3: Bloco ('O')
	//       - 4: Alavanca ('A')
        //       - 5: Saída ('S')
	//       - 6: PortaA ('=') / (':') fechada em 0 e 180 de rotação, aberta em 90 e 270
        //       - 5: PortaB ('|') / (';') fechada em 90 e 270 de rotação, aberta em 0 e 180
        // TODO: Barra de status (Rotacao, Level, Tempo).
}

void pegaInput(char &tecla, bool &executando)
{
	if(_kbhit())
	{
		tecla = _getch(); // Escape no ASCII
		if(tecla == 27)
		{
			executando = false;
		} 
	}
	else
        {
                tecla = 0; // Clear key buffer if no input detected
        }
}

void copiaMatriz(int matriz[MAX_TAMANHO][MAX_TAMANHO], int copia[MAX_TAMANHO][MAX_TAMANHO], int tamanho)
{
	for (int l = 0; l < tamanho; l++)
	{
		for (int c = 0; c < tamanho; c++)
		{
			copia[l][c] = matriz[l][c];
		}
	}
}

void zeraMatriz(int matriz[MAX_TAMANHO][MAX_TAMANHO])
{
	for (int l = 0; l < MAX_TAMANHO; l++)
	{
		for (int c = 0; c < MAX_TAMANHO; c++)
		{
			matriz[l][c] = 0;
		}
	}
}

void carregaMapa(int level, int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, Player &player) 
{
	zeraMatriz(mapa);

	switch(level)
	{
		case 0: {
			player.x = 1; // arrumar dps
			player.y = 1; // arrumar dps

			int base[MAX_TAMANHO][MAX_TAMANHO] = {
				{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				{1, 0, 0, 0, 1, 0, 0, 0, 0, 2, 1},
				{1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
				{1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
				{1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1},
				{1, 0, 0, 0, 4, 0, 1, 0, 0, 0, 1},
				{1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
				{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
				{1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1},
				{1, 2, 0, 0, 0, 0, 0, 0, 0, 3, 1},
				{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
			};
			copiaMatriz(base, mapa, tamanho);

			break;
		}
		case 1: {
			player.x = 1; // arrumar dps
			player.y = 1; // arrumar dps

			int base[MAX_TAMANHO][MAX_TAMANHO] = {
				{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				{1, 0, 0, 0, 1, 0, 0, 0, 0, 2, 1},
				{1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
				{1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
				{1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1},
				{1, 0, 0, 0, 4, 0, 1, 0, 0, 0, 1},
				{1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
				{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
				{1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1},
				{1, 2, 0, 0, 0, 0, 0, 0, 0, 3, 1},
				{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
			};

			copiaMatriz(base, mapa, tamanho);
			break;
		}

		default: 
			break;
	}
}

void rotacionaMapa(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, Player &player, char direcao)
{
	// TODO: Rotação direita, esquerda
        // TODO: Mudar toda a matriz inclusive player
        // TODO: Aplica gravidade em tudo até no player.
}

void aplicaGravidade(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, Player &player)
{
        // TODO: Vai pra baixo até bater em objeto sólido (porta fechada ou parede).
        // TODO: Checa se uma porta fechando mata o player ou destroi uma caixa.
}

void loopJogo(int level, bool &jogo_salvo)   
{
	// setar lógica de continuar ignorando os resets abaixo:
	int mapa[MAX_TAMANHO][MAX_TAMANHO] = {0};

	bool executando = true;
	char tecla = 0;

	Player player;
	player.x = 0;
	player.y = 0;
	player.em_alavanca = false;

	Nivel nivel;
	nivel.alavanca_ativa = false;
	nivel.rotacao = 0;
	nivel.tamanho = 11; // default
	nivel.nivel_completo = false;
	nivel.perdeu = false;
	
	defineTamanho(level, nivel);
	carregaMapa(level, mapa, nivel.tamanho, player);
	

	// desativa o cursor no console
	escondeCursor();
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); // handle da saída do console
	limpaTela();
	
	while(executando && !(nivel.nivel_completo))
	{
		pegaInput(tecla, executando);

		if (tecla != 0)
		{
			atualizaJogo(mapa, tecla, nivel, player, executando);
			imprimeJogo(mapa, nivel, player);
		}

		this_thread::sleep_for(chrono::milliseconds(30));
	}

	// TODO: Passar o level caso tenha ganho
	if(!executando && !nivel.nivel_completo && !nivel.perdeu)
	{
		jogo_salvo = true;
	}
	else 
	{
		jogo_salvo = false;
	}
}
	
int main() {
	bool executando = true;
	int level = 0;
	bool jogo_salvo = false;

	setlocale(LC_ALL, ""); // local do pt br sistema

	limpaTela();
	mostrarTitulo();

	//adicionar
	while (executando) 
	{
		menu();
		processaMenu(executando, jogo_salvo, level);
		limpaTela();
	}

	limpaTela();
	cout << "Saindo do jogo..." << endl;

	return 0;
}