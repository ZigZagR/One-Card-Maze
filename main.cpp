#include <conio.h>
#include <iostream>
#include <time.h>
#include <windows.h>
using namespace std;

const int MAX_TAMANHO = 30;

void ajuda();
void atualizaJogo(int mapa[MAX_TAMANHO][MAX_TAMANHO], char tecla, int tamanho,
		  int &rotacao, int &plinha, int &pcoluna, int &sob_jogador,
		  bool &nivel_completo, bool &perdeu, bool &executando,
		  int &total_rotacoes, int &movimentos);
void aplicaGravidade(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho,
		     int rotacao);
void carregaMapa(int level, int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho,
		 int &plinha, int &pcoluna);
void copiaMatriz(int matriz[MAX_TAMANHO][MAX_TAMANHO],
		 int copia[MAX_TAMANHO][MAX_TAMANHO], int tamanho);
void defineTamanho(int level, int &tamanho);
void efeitoTexto(const string &texto, int atraso_ms = 40);
void escondeCursor();
void fechaPortas(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int rotacao,
		 int plinha,
		 int pcoluna bool &perdeu) void gotoxy(HANDLE h, int XPos,
						       int YPos);
void hud(int level, int rotacao, int movimentos, int total_rotacoes);
void imprimeJogo(HANDLE h, const int mapa[MAX_TAMANHO][MAX_TAMANHO],
		 int tamanho, int rotacao, int plinha, int pcoluna, int level,
		 int movimentos, int total_rotacoes);
void limpaTela();
void localizaJogador(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho,
		     int &plinha, int &pcoluna);
void loopJogo(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int level,
	      int &rotacao, int &plinha, int &pcoluna, int &sob_jogador,
	      int &movimentos, int &total_rotacoes, bool &jogo_em_andamento);
void menu();
void mostrarTitulo();
bool movimentoValido(int l, int c, int mapa[MAX_TAMANHO][MAX_TAMANHO],
		     int tamanho, int rotacao);
void pegaInput(char &tecla, bool &executando);
void processaMenu(bool &executando, bool &jogo_em_andamento, int &level,
		  int mapa[MAX_TAMANHO][MAX_TAMANHO], int &tamanho,
		  int &rotacao, int &plinha, int &pcoluna, int &sob_jogador,
		  int &movimentos, int &total_rotacoes);
void rotacionaMapa(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int &plinha,
		   int &pcoluna, char direcao, int &rotacao, bool &perdeu);
int selecionaMapa();
void sobre();
void zeraMatriz(int matriz[MAX_TAMANHO][MAX_TAMANHO]);

// do jogo do rpg de texto
void efeitoTexto(const string &texto, int atraso_ms)
{
	for (unsigned int i = 0; i < texto.size(); i++)
	{
		cout << texto[i] << flush;
		Sleep(atraso_ms);
	}
	cout << endl;
}

void hud(int level, int rotacao, int movimentos, int total_rotacoes)
{
	// TODO: Fazer hud do jogo
}

void mostrarTitulo()
{
	cout << "======================================" << endl;
	Sleep(500);

	efeitoTexto("         O N E   C A R D   G A M E     ", 50);

	Sleep(500);
	cout << "======================================" << endl;
	cout << endl;

	Sleep(800);
}

void escondeCursor()
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.dwSize = 100;
	cursor.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}

void menu()
{
	cout << "1. Novo Jogo " << endl;
	cout << "2. Continuar " << endl;
	cout << "3. Ajuda " << endl;
	cout << "4. Sobre " << endl;
	cout << endl;
	cout << "Escolha uma opção " << endl;
	cout << endl;
	cout << "Pressione ESC para sair do jogo" << endl;
}

void sobre()
{
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

void ajuda()
{
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
	cout << "- O jogador nao atravessa paredes, blocos ou portas fechadas."
	     << endl;
	cout << "- Rotacao gira tudo: paredes, blocos, portas, saida e jogador."
	     << endl;
	cout << "- Portas fechadas bloqueiam e sustentam blocos; abertas nao."
	     << endl;
	cout << "- Apos girar, blocos caem ate encontrar apoio." << endl;
	cout << "- Porta fechando sobre bloco: bloco e destruido." << endl;
	cout << "- Porta fechando sobre jogador: fase e perdida." << endl;
	cout << "- Objetivo: alcancar a saida (S)." << endl;
	cout << endl;

	cout << "Pressione qualquer tecla para voltar..." << endl;
	getch();
}

int selecionaMapa()
{
	limpaTela();
	cout << "Novo Jogo" << endl;
	cout << "1. Escolher mapa" << endl;
	cout << "2. Mapa aleatorio" << endl;
	cout << endl;
	cout << "Pressione ESC para voltar" << endl;

	while (true)
	{
		char opcao = getch();

		if (opcao == '1')
		{
			limpaTela();
			cout << "Escolher o mapa (1 a 3):" << endl;
			cout << "1 - Mapa 1" << endl;
			cout << "2 - Mapa 2" << endl;
			cout << "3 - Mapa 3" << endl;
			cout << endl;
			cout << "Pressione ESC para voltar" << endl;

			while (true)
			{
				char opcao = getch();

				if (opcao == '1')
				{
					return 0;
				}
				else if (opcao == '2')
				{
					return 1;
				}
				else if (opcao == '3')
				{
					return 2;
				}
				else if (opcao == 27) // ESC
				{
					return -1;
				}
			}
		}
		else if (opcao == '2')
		{
			return rand() % 3;
		}
		else if (opcao == 27)
		{
			return -1;
		}
	}
}

void processaMenu(bool &executando, bool &jogo_em_andamento, int &level,
		  int mapa[MAX_TAMANHO][MAX_TAMANHO], int &tamanho,
		  int &rotacao, int &plinha, int &pcoluna, int &sob_jogador,
		  int &movimentos, int &total_rotacoes)
{
	char tecla = 0;

	while (true)
	{
		if (_kbhit())
		{
			tecla = _getch();

			if (tecla == '1')
			{
				int mapa_escolhido = selecionaMapa();

				if (mapa_escolhido != -1)
				{

					level = mapa_escolhido;
					defineTamanho(level, tamanho);
					carregaMapa(level, mapa, tamanho,
						    plinha, pcoluna);

					sob_jogador = 0;
					rotacao = 0;
					movimentos = 0;
					total_rotacoes = 0;
					jogo_em_andamento = true;

					loopJogo(mapa, tamanho, level, rotacao,
						 plinha, pcoluna, sob_jogador,
						 movimentos, total_rotacoes,
						 jogo_em_andamento);
				}

				break;
			}
			else if (tecla == '2')
			{
				if (jogo_em_andamento)
				{
					loopJogo(mapa, tamanho, level, rotacao,
						 plinha, pcoluna, sob_jogador,
						 movimentos, total_rotacoes,
						 jogo_em_andamento);
				}
				else
				{
					cout << "Nenhum jogo em andamento!\n";
					Sleep(1000);
				}
				break;
			}
			else if (tecla == '3')
			{
				ajuda();
				break;
			}
			else if (tecla == '4')
			{
				sobre();
				break;
			}
			else if (tecla == 27) // ESC
			{
				executando = false;
				break;
			}

			Sleep(30);
		}
	}
}

void defineTamanho(int level, int &tamanho)
{
	if (level == 0)
	{
		tamanho = 11;
	}
	else if (level == 1)
	{
		tamanho = 11;
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
	cout << "\033[H\033[2J"; // limpador de tela universal ANSI ->
				 // Stackoverflow
}

bool movimentoValido(int l, int c, int mapa[MAX_TAMANHO][MAX_TAMANHO],
		     int tamanho, int rotacao)
{
	if (l < 0 || l >= tamanho || c < 0 || c >= tamanho)
	{
		return false; // fora dos limites
	}
	if (mapa[l][c] == 1)
	{
		return false; // parede
	}
	if (mapa[l][c] == 3)
	{
		return false; // bloco
	}
	if ((mapa[l][c] == 6) && (rotacao == 0 || rotacao == 180))
	{
		return false; // porta A
	}
	if ((mapa[l][c] == 7) && (rotacao == 90 || rotacao == 270))
	{
		return false; // porta B
	}

	return true;
}

void atualizaJogo(int mapa[MAX_TAMANHO][MAX_TAMANHO], char tecla, int tamanho,
		  int &rotacao, int &plinha, int &pcoluna, int &sob_jogador,
		  bool &nivel_completo, bool &perdeu, bool &executando,
		  int &total_rotacoes, int &movimentos)
{
	int prox_l = plinha;
	int prox_c = pcoluna;

	if (tecla == 'w' || tecla == 'W')
	{
		prox_l--;
	}
	else if (tecla == 'a' || tecla == 'A')
	{
		prox_c--;
	}
	else if (tecla == 's' || tecla == 'S')
	{
		prox_l++;
	}
	else if (tecla == 'd' || tecla == 'D')
	{
		prox_c++;
	}
	else if (tecla == 'r' || tecla == 'R')
	{
		// TODO: Restart
		return;
	}
	else if ((tecla == 'q' || tecla == 'Q' || tecla == 'e' ||
		  tecla == 'E') &&
		 sob_jogador == 4)
	{
		rotacionaMapa(mapa, tamanho, plinha, pcoluna, tecla, rotacao,
			      perdeu);

		if (tecla == 'q' || tecla == 'Q')
		{
			rotacao = (rotacao + 270) % 360;
		}
		else
		{
			rotacao = (rotacao + 90) % 360;
		}

		total_rotacoes++;
		return;
	}
	else
	{
		return;
	}

	if (movimentoValido(prox_l, prox_c, mapa, tamanho, rotacao))
	{
		mapa[plinha][pcoluna] = sob_jogador; // bota oq tava embaixo

		sob_jogador =
			mapa[prox_l][prox_c]; // salva oq vai estar embaixo

		mapa[prox_l][prox_c] = 2;

		plinha = prox_l;
		pcoluna = prox_c;

		movimentos++;

		// verifica vitoria
		if (sob_jogador == 5)
		{
			nivel_completo = true;
		}
	}
}

void imprimeJogo(HANDLE h, const int mapa[MAX_TAMANHO][MAX_TAMANHO],
		 int tamanho, int rotacao, int plinha, int pcoluna, int level,
		 int movimentos, int total_rotacoes)
{
	gotoxy(h, 0, 0);

	cout << "========================================\n";
	cout << "          LABIRINTO GIRATORIO          \n";
	cout << "========================================\n\n";

	for (int l = 0; l < tamanho; l++)
	{
		cout << "  ";
		for (int c = 0; c < tamanho; c++)
		{
			if ((l == plinha && c == pcoluna) || mapa[l][c] == 2)
			{
				cout << "@ ";
			}
			else
			{
				switch (mapa[l][c])
				{
				case 0:
					cout << "  ";
					break;
				case 1:
					cout << "# ";
					break;
				case 3:
					cout << "O ";
					break;
				case 4:
					cout << "A ";
					break;
				case 5:
					cout << "S ";
					break;
				case 6:
					if (rotacao == 0 || rotacao == 180)
					{
						cout << "= ";
					}
					else
					{
						cout << ": ";
					}
					break;
				case 7:
					if (rotacao == 90 || rotacao == 270)
					{
						cout << "| ";
					}
					else
					{
						cout << "; ";
					}
					break;
				default:
					cout << "  ";
					break;
				}
			}
		}
		cout << "\n";
	}

	hud(level, rotacao, movimentos, total_rotacoes);
}

void pegaInput(char &tecla, bool &executando)
{
	if (_kbhit())
	{
		tecla = _getch(); // Escape no ASCII
		if (tecla == 27)
		{
			executando = false;
		}
	}
	else
	{
		tecla = 0; // Clear key buffer if no input detected
	}
}

void copiaMatriz(int matriz[MAX_TAMANHO][MAX_TAMANHO],
		 int copia[MAX_TAMANHO][MAX_TAMANHO], int tamanho)
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

void carregaMapa(int level, int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho,
		 int &plinha, int &pcoluna)
{
	zeraMatriz(mapa);

	switch (level)
	{
	case 0:
	{
		int base[MAX_TAMANHO][MAX_TAMANHO] = {
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1},
			{1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
			{1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
			{1, 0, 1, 0, 4, 0, 0, 0, 1, 0, 1},
			{1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
			{1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
			{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
			{1, 0, 1, 1, 1, 6, 1, 1, 0, 0, 1},
			{1, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}

		};

		copiaMatriz(base, mapa, tamanho);
		localizaJogador(mapa, tamanho, plinha, pcoluna);

		break;
	}
	case 1:
	{
		int base[MAX_TAMANHO][MAX_TAMANHO] = {
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{1, 2, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
			{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1},
			{1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
			{1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1},
			{1, 0, 0, 0, 4, 0, 0, 0, 0, 0, 1, 0, 1},
			{1, 1, 1, 7, 1, 1, 1, 0, 1, 0, 1, 0, 1},
			{1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
			{1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1},
			{1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1},
			{1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
			{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 5},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}

		};

		copiaMatriz(base, mapa, tamanho);
		localizaJogador(mapa, tamanho, plinha, pcoluna);

		break;
	}
	case 2:
	{
		int base[MAX_TAMANHO][MAX_TAMANHO] = {
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{1, 2, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
			{1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1},
			{1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1},
			{1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1},
			{1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1},
			{1, 0, 1, 0, 0, 4, 0, 1, 0, 0, 0, 1, 1, 1, 1},
			{1, 0, 7, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 3, 1},
			{1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1},
			{1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1},
			{1, 0, 0, 0, 1, 0, 1, 6, 0, 1, 0, 1, 1, 0, 1},
			{1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1},
			{1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1},
			{1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 5, 1},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}

		};

		copiaMatriz(base, mapa, tamanho);
		localizaJogador(mapa, tamanho, plinha, pcoluna);

		break;
	}
	default:
		break;
	}
}

void localizaJogador(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho,
		     int &plinha, int &pcoluna)
{
	for (int l = 0; l < tamanho; l++)
	{
		for (int c = 0; c < tamanho; c++)
		{
			if (mapa[l][c] == 2)
			{
				plinha = l;
				pcoluna = c;
				return;
			}
		}
	}
}

bool portaFechada(int porta, int rotacao)
{

	if (porta == 6)
	{
		return rotacao == 0 || rotacao == 180;
	}
	else if (porta == 7)
	{
		return rotacao == 90 || rotacao == 270;
	}
	return false;
}

void fechaPortas(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int rotacao,
		 int plinha, int pcoluna, bool &perdeu, int sob_jogador)
{
	for (int l = 0; l < tamanho; l++)
	{
		for (int c = 0; c < tamanho; c++)
		{
			int atual = mapa[l][c];
			bool porta_fechada = false;

			if (porta_fechada)
			{
			}
		}
	}
}

void rotacionaMapa(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int &plinha,
		   int &pcoluna, char direcao, int &rotacao, bool &perdeu)
{
	// TODO: Rotação direita, esquerda Ok
	// TODO: Mudar toda a matriz inclusive player // Falta player
	// TODO: Aplica gravidade nos blocos. Ok -> Fazer saída sólida?
	int temp[MAX_TAMANHO][MAX_TAMANHO];

	for (int l = 0; l < tamanho; l++)
	{
		for (int c = 0; c < tamanho; c++)
		{
			if (direcao == 'e' || direcao == 'E')
			{
				temp[c][tamanho - 1 - l] = mapa[l][c];
			}
			else if (direcao == 'q' || direcao == 'Q')
			{
				temp[tamanho - 1 - c][l] = mapa[l][c];
			}
		}
	}

	copiaMatriz(temp, mapa, tamanho);

	aplicaGravidade(mapa, tamanho, rotacao);
	localizaJogador(mapa, tamanho, plinha, pcoluna);
	fechaPortas(mapa, tamanho, rotacao, plinha, pcoluna, perdeu);
}

bool movimentoValidoBloco(int alvo, int rotacao)
{
	if (alvo == 1 || alvo == 2 || alvo == 3 || alvo == 5)
	{
		return false;
	}
	if (alvo == 6 && (rotacao == 90 || rotacao == 270))
	{
		return false;
	}
	if (alvo == 7 && (rotacao == 0 || rotacao == 180))
	{
		return false;
	}
	return true;
}

void aplicaGravidade(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho,
		     int rotacao)
{
	// TODO: Vai pra baixo até bater em objeto sólido (porta fechada
	// ou parede).
	// TODO: Checa se uma porta fechando mata o player ou destroi
	// uma caixa.
	for (int l = tamanho - 2; l >= 0;
	     l--) // de cima pra baixo começando na penultima fileira
	{
		for (int c = 0; c < tamanho; c++)
		{
			if (mapa[l][c] == 3)
			{
				int linha = l;
				int coluna = c;
				while (linha + 1 < tamanho)
				{
					int prox = mapa[linha + 1][c];

					if (movimentoValidoBloco(prox, rotacao))
					{
						mapa[linha + 1][coluna] = 3;
						mapa[linha][coluna] = 0;
						linha++;
					}
					else
					{
						break;
					}
				}
			}
		}
	}
}

void loopJogo(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int level,
	      int &rotacao, int &plinha, int &pcoluna, int &sob_jogador,
	      int &movimentos, int &total_rotacoes, bool &jogo_em_andamento)
{
	bool executando_level = true;
	char tecla = 0;
	bool nivel_completo = false;
	bool perdeu = false;

	// desativa o cursor no console
	escondeCursor();
	HANDLE h =
		GetStdHandle(STD_OUTPUT_HANDLE); // handle da saída do console
	limpaTela();

	imprimeJogo(h, mapa, tamanho, rotacao, plinha, pcoluna, level,
		    movimentos, total_rotacoes);

	while (executando_level && !nivel_completo && !perdeu)
	{
		pegaInput(tecla, executando_level);

		if (tecla != 0)
		{
			atualizaJogo(mapa, tecla, tamanho, rotacao, plinha,
				     pcoluna, sob_jogador, nivel_completo,
				     perdeu, executando_level, total_rotacoes,
				     movimentos);
			imprimeJogo(h, mapa, tamanho, rotacao, plinha, pcoluna,
				    level, movimentos, total_rotacoes);
		}
		Sleep(30);
	}

	if (nivel_completo || perdeu)
	{
		jogo_em_andamento = false;
	}
	else
	{
		// ESC
		jogo_em_andamento = true;
	}
}

int main()
{
	bool executando = true;
	bool jogo_em_andamento = false;
	int level = 0;

	int mapa[MAX_TAMANHO][MAX_TAMANHO];
	int tamanho = 11;
	int rotacao = 0;
	int plinha = 0, pcoluna = 0;
	int sob_jogador = 0;
	int movimentos = 0;
	int total_rotacoes = 0;

	srand(time(NULL));
	setlocale(LC_ALL, ""); // local do pt br sistema

	limpaTela();
	mostrarTitulo();

	// adicionar
	while (executando)
	{
		menu();
		processaMenu(executando, jogo_em_andamento, level, mapa,
			     tamanho, rotacao, plinha, pcoluna, sob_jogador,
			     movimentos, total_rotacoes);
		limpaTela();
	}

	limpaTela();
	cout << "Saindo do jogo..." << endl;

	return 0;
}
