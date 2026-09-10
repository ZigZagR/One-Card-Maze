#include <conio.h>
#include <iostream>
#include <time.h>
#include <windows.h>
using namespace std;

const int MAX_TAMANHO = 30;

void ajuda();
void atualizaJogo(int mapa[MAX_TAMANHO][MAX_TAMANHO], char tecla, int tamanho, int &rotacao, int &plinha, int &pcoluna,
		  int &sob_jogador, bool &sob_alavanca, bool &nivel_completo, bool &perdeu, int &total_rotacoes,
		  int &movimentos);
void aplicaGravidade(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int rotacao);
void carregaMapa(int level, int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int &plinha, int &pcoluna);
bool celulaPodeSerAtravessada(int celula, int rotacao);
bool celulaSustentaBloco(int celula, int rotacao);
void copiaMatriz(int matriz[MAX_TAMANHO][MAX_TAMANHO], int copia[MAX_TAMANHO][MAX_TAMANHO], int tamanho);
void defineTamanho(int level, int &tamanho);
void efeitoTexto(const string &texto, int atraso_ms = 40);
void escondeCursor();
void esmagaCaixas(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int rotacao);
bool estaSobreAlavanca(int sob_jogador);
void fechaPortas(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int rotacao, int plinha, int pcoluna, bool &perdeu,
		 int sob_jogador);
void gotoxy(HANDLE h, int XPos, int YPos);
void hud(int level, int rotacao, int movimentos, int total_rotacoes);
void imprimeJogo(HANDLE h, const int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int rotacao, int plinha, int pcoluna,
		 int level, int movimentos, int total_rotacoes);
void imprime_menu();
void limpaInput();
void limpaTela();
void localizaJogador(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int &plinha, int &pcoluna);
void loopJogo(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int level, int &rotacao, int &plinha, int &pcoluna,
	      int &sob_jogador, bool &sob_alavanca, int &movimentos, int &total_rotacoes, bool &jogo_em_andamento);
void mostrarTitulo(bool delay);
bool movimentoValido(int l, int c, int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int rotacao);
void moveJogador(int mapa[MAX_TAMANHO][MAX_TAMANHO], int prox_l, int prox_c, int &plinha, int &pcoluna,
		 int &sob_jogador, bool &sob_alavanca, int &movimentos, bool &nivel_completo);
void pegaInput(char &tecla, bool &executando);
bool portaFechada(int porta, int rotacao);
void processaMenu(bool &executando, bool &jogo_em_andamento, int &level, int mapa[MAX_TAMANHO][MAX_TAMANHO],
		  int &tamanho, int &rotacao, int &plinha, int &pcoluna, int &sob_jogador, bool &sob_alavanca,
		  int &movimentos, int &total_rotacoes);
void rotacionaMapa(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, char direcao, int &rotacao, int &plinha,
		   int &pcoluna, int &sob_jogador, bool &perdeu);
int selecionaMapa();
void sobre();
void tela_derrota(int movimentos, int total_rotacoes);
void tela_vitoria(int movimentos, int total_rotacoes);
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
	cout << "\n";
	cout << "Mapa: " << (level + 1) << "   ";
	cout << "Orientacao: " << rotacao << " graus   ";
	cout << "Movimentos: " << movimentos << "   ";
	cout << "Rotacoes: " << total_rotacoes << "    \n";
}

void mostrarTitulo(bool delay)
{
	// if tao pra n dar sleep no jogo em si quando chama no imprime jogo
	cout << "======================================" << endl;
	if (delay)
	{
		Sleep(500);
	}

	if (delay)
	{
		efeitoTexto("      O N E   C A R D   G A M E       ", 50);
	}
	else
	{
		cout << "      O N E   C A R D   G A M E       \n";
	}

	if (delay)
	{
		Sleep(500);
	}
	cout << "======================================" << endl;
	cout << endl;

	if (delay)
	{
		Sleep(800);
	}
}

void tela_derrota(int movimentos, int total_rotacoes)
{
	limpaTela();
	cout << "====================================\n";
	cout << "         GAME OVER!                 \n";
	cout << "   Voce foi esmagado pela porta.    \n";
	cout << "====================================\n\n";
	cout << "Pressione qualquer tecla para voltar...";
	limpaInput();
	getch();
}

void tela_vitoria(int movimentos, int total_rotacoes)
{
	limpaTela();
	cout << "====================================\n";
	cout << "         VOCE VENCEU!               \n";
	cout << "====================================\n";
	cout << "Movimentos: " << movimentos << "\n";
	cout << "Rotacoes: " << total_rotacoes << "\n\n";
	cout << "Pressione qualquer tecla para voltar...";
	limpaInput();
	getch();
}

void escondeCursor() // prof.
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.dwSize = 100;
	cursor.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}

void imprime_menu()
{
	cout << "1. Novo Jogo " << endl;
	cout << "2. Continuar " << endl;
	cout << "3. Sobre / Ajuda " << endl;
	cout << "4. Fim " << endl; // Em vez de apenas deixar o ESC
	cout << endl;
	cout << "Escolha uma opcao: " << endl;
}

void sobre()
{
	cout << "======= Sobre =======" << endl;
	cout << endl;

	cout << "Equipe de desenvolvimento:\n- Pedro Henrique R. J. "
		"Nicolini\n- "
		"Luiz Eduardo F. N. Goncalves\n";
	cout << "Setembro/2026\n";
	cout << "Professor: Thiago Felski\nDisciplina: Algoritmos e "
		"Programacao II\n\n";
}

void ajuda()
{
	limpaTela();
	sobre();

	cout << "======= AJUDA =======" << endl;
	cout << endl;

	cout << "CONTROLES:" << endl;
	cout << "W A S D - Mover" << endl;
	cout << "Q / E   - Girar cenario (na alavanca)" << endl;
	cout << "R       - Reiniciar fase" << endl;
	cout << "ESC     - Voltar ao menu" << endl;
	cout << endl;

	cout << "REGRAS:" << endl;
	cout << "- O jogador nao atravessa paredes, blocos ou portas fechadas." << endl;
	cout << "- Rotacao gira tudo: paredes, blocos, portas, saida e jogador." << endl;
	cout << "- Portas fechadas bloqueiam e sustentam blocos; abertas nao." << endl;
	cout << "- Apos girar, blocos caem ate encontrar apoio." << endl;
	cout << "- Porta fechando sobre bloco: bloco e destruido." << endl;
	cout << "- Porta fechando sobre jogador: fase e perdida." << endl;
	cout << "- Objetivo: alcancar a saida (S)." << endl;
	cout << endl;

	cout << "Pressione qualquer tecla para voltar..." << endl;
	limpaInput();
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

void processaMenu(bool &executando, bool &jogo_em_andamento, int &level, int mapa[MAX_TAMANHO][MAX_TAMANHO],
		  int &tamanho, int &rotacao, int &plinha, int &pcoluna, int &sob_jogador, bool &sob_alavanca,
		  int &movimentos, int &total_rotacoes)
{
	char tecla = 0;

	limpaInput();

	while (true)
	{
		if (_kbhit())
		{
			tecla = _getch();

			if (tecla == '1')
			{
				limpaInput();
				int mapa_escolhido = selecionaMapa();

				if (mapa_escolhido != -1)
				{

					level = mapa_escolhido;
					defineTamanho(level, tamanho);
					carregaMapa(level, mapa, tamanho, plinha, pcoluna);

					sob_jogador = 0;
					sob_alavanca = false;
					rotacao = 0;
					movimentos = 0;
					total_rotacoes = 0;
					jogo_em_andamento = true;

					loopJogo(mapa, tamanho, level, rotacao, plinha, pcoluna, sob_jogador,
						 sob_alavanca, movimentos, total_rotacoes, jogo_em_andamento);
				}

				break;
			}
			else if (tecla == '2')
			{
				if (jogo_em_andamento)
				{
					loopJogo(mapa, tamanho, level, rotacao, plinha, pcoluna, sob_jogador,
						 sob_alavanca, movimentos, total_rotacoes, jogo_em_andamento);
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
				executando = false;
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
		tamanho = 13;
	}
	else if (level == 2)
	{
		tamanho = 15;
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

bool ehCaixa(int celula) { return celula == 3 || celula == 8 || celula == 9; }

bool movimentoValido(int l, int c, int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int rotacao)
{
	if (l < 0 || l >= tamanho || c < 0 || c >= tamanho)
	{
		return false; // fora dos limites
	}
	return celulaPodeSerAtravessada(mapa[l][c], rotacao);
}

void moveJogador(int mapa[MAX_TAMANHO][MAX_TAMANHO], int prox_l, int prox_c, int &plinha, int &pcoluna,
		 int &sob_jogador, bool &sob_alavanca, int &movimentos, bool &nivel_completo)
{
	mapa[plinha][pcoluna] = sob_jogador; // bota base antiga
	sob_jogador = mapa[prox_l][prox_c];  // salva
	sob_alavanca = estaSobreAlavanca(sob_jogador);

	mapa[prox_l][prox_c] = 2; // move
	plinha = prox_l;
	pcoluna = prox_c;

	movimentos++;

	if (sob_jogador == 5)
	{
		nivel_completo = true;
	}
}

void atualizaJogo(int mapa[MAX_TAMANHO][MAX_TAMANHO], char tecla, int tamanho, int &rotacao, int &plinha, int &pcoluna,
		  int &sob_jogador, bool &sob_alavanca, bool &nivel_completo, bool &perdeu, bool &reiniciar,
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
		reiniciar = true;
		return;
	}
	else if ((tecla == 'q' || tecla == 'Q' || tecla == 'e' || tecla == 'E') && sob_alavanca)
	{
		rotacionaMapa(mapa, tamanho, tecla, rotacao, plinha, pcoluna, sob_jogador, perdeu);
		sob_alavanca = estaSobreAlavanca(sob_jogador);

		total_rotacoes++;
		return;
	}
	else
	{
		return;
	}

	if (movimentoValido(prox_l, prox_c, mapa, tamanho, rotacao))
	{
		moveJogador(mapa, prox_l, prox_c, plinha, pcoluna, sob_jogador, sob_alavanca, movimentos,
			    nivel_completo);
	}
}

void imprimeJogo(HANDLE h, const int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int rotacao, int plinha, int pcoluna,
		 int level, int movimentos, int total_rotacoes)
{
	gotoxy(h, 0, 0);

	mostrarTitulo(false);

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
				case 8: // caixa sobre A
				case 9: // caixa sobre B
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
		tecla = 0; // limpa
	}
}

void limpaInput()
{
	while (_kbhit())
	{
		_getch();
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

void carregaMapa(int level, int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int &plinha, int &pcoluna)
{
	zeraMatriz(mapa);

	switch (level)
	{
	case 0:
	{
		int base[MAX_TAMANHO][MAX_TAMANHO] = {
			// se a formatação tá ruim , ela está ruim.
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    			{1, 2, 0, 0, 4, 0, 0, 0, 0, 0, 1},
    			{1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    			{1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
    			{1, 0, 1, 0, 7, 0, 0, 0, 1, 0, 1},
    			{1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
    			{1, 0, 1, 1, 1, 6, 1, 1, 1, 0, 1},
    			{1, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1},
    			{1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    			{1, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1},
    			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

		copiaMatriz(base, mapa, tamanho);
		localizaJogador(mapa, tamanho, plinha, pcoluna);

		break;
	}
	case 1:
	{
		int base[MAX_TAMANHO][MAX_TAMANHO] = {
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    			{1, 2, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 1},
    			{1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    			{1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    			{1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1},
    			{1, 0, 1, 0, 1, 0, 0, 3, 1, 0, 1, 0, 1},
    			{1, 0, 7, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1},
    			{1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1},
    			{1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    			{1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
    			{1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1},
    			{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 6, 5},
    			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

		copiaMatriz(base, mapa, tamanho);
		localizaJogador(mapa, tamanho, plinha, pcoluna);

		break;
	}
	case 2:
	{
		int base[MAX_TAMANHO][MAX_TAMANHO] = {
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    			{1, 2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    			{1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1},
    			{1, 0, 1, 4, 0, 1, 0, 1, 0, 1, 0, 1, 3, 0, 1},
    			{1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1},
    			{1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    			{1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    			{1, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    			{1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    			{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    			{1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
    			{1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    			{1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    			{1, 0, 0, 0, 1, 0, 0, 0, 6, 0, 0, 0, 0, 5, 1},
    			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

		copiaMatriz(base, mapa, tamanho);
		localizaJogador(mapa, tamanho, plinha, pcoluna);

		break;
	}
	default:
		break;
	}
}

void localizaJogador(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int &plinha, int &pcoluna)
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

bool estaSobreAlavanca(int sob_jogador) { return sob_jogador == 4; }

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

void fechaPortaJogador(int rotacao, int sob_jogador, bool &perdeu)
{
	if ((sob_jogador == 6 || sob_jogador == 7) && portaFechada(sob_jogador, rotacao))
	{
		perdeu = true;
	}
}

bool celulaPodeSerAtravessada(int celula, int rotacao)
{
	if (celula == 1 || ehCaixa(celula) || celula == 2) // 2 pra caixa parar no player
	{
		return false; // Parede e caixa
	}
	if (celula == 6 && portaFechada(6, rotacao))
	{
		return false; // A
	}
	if (celula == 7 && portaFechada(7, rotacao))
	{
		return false; // B
	}
	return true;
}

bool celulaSustentaBloco(int celula, int rotacao)
{
	return !celulaPodeSerAtravessada(celula, rotacao); // n pode atravessar == solido
}

void esmagaCaixas(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int rotacao)
{
	for (int l = 0; l < tamanho; l++)
	{
		for (int c = 0; c < tamanho; c++)
		{
			if (mapa[l][c] == 8 && portaFechada(6, rotacao))
			{
				mapa[l][c] = 6; // kaboom A
			}
			else if (mapa[l][c] == 9 && portaFechada(7, rotacao))
			{
				mapa[l][c] = 7; // kaboom B
			}
		}
	}
}

void rotacionaMapa(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, char direcao, int &rotacao, int &plinha,
		   int &pcoluna, int &sob_jogador, bool &perdeu)
{
	int nova_rotacao = rotacao;

	if (direcao == 'e' || direcao == 'E')
	{
		nova_rotacao = (rotacao + 90) % 360;
	}
	else if (direcao == 'q' || direcao == 'Q')
	{
		nova_rotacao = (rotacao + 270) % 360;
	}

	// Bota oq tá embaixo do jogador na matriz p n perder
	int origem[MAX_TAMANHO][MAX_TAMANHO];
	copiaMatriz(mapa, origem, tamanho);
	origem[plinha][pcoluna] = sob_jogador;

	int novo_mapa[MAX_TAMANHO][MAX_TAMANHO];
	zeraMatriz(novo_mapa);

	for (int l = 0; l < tamanho; l++)
	{
		for (int c = 0; c < tamanho; c++)
		{
			int nova_linha, nova_coluna;

			if (direcao == 'e' || direcao == 'E') // Inverte pra direita / relógio
			{
				nova_linha = c;
				nova_coluna = tamanho - 1 - l;
			}
			else // Inverta pra esquerda / antihorário
			{
				nova_linha = tamanho - 1 - c;
				nova_coluna = l;
			}

			novo_mapa[nova_linha][nova_coluna] = origem[l][c];
		}
	}

	// acha jogador pelo índice
	int plinha_antigo = plinha;
	int pcoluna_antigo = pcoluna;

	if (direcao == 'e' || direcao == 'E')
	{
		plinha = pcoluna_antigo;
		pcoluna = tamanho - 1 - plinha_antigo;
	}
	else
	{
		plinha = tamanho - 1 - pcoluna_antigo;
		pcoluna = plinha_antigo;
	}

	sob_jogador = novo_mapa[plinha][pcoluna]; // guarda
	novo_mapa[plinha][pcoluna] = 2;

	copiaMatriz(novo_mapa, mapa, tamanho);
	rotacao = nova_rotacao;

	fechaPortaJogador(rotacao, sob_jogador, perdeu); // morreu ou n

	if (perdeu)
	{
		return;
	}

	// Aplica gravidade nos blocos que sobraram
	aplicaGravidade(mapa, tamanho, rotacao);

	// Esmaga qualquer caixa que tenha ficado sobre uma porta que fechou
	esmagaCaixas(mapa, tamanho, rotacao);
}

bool porta(int alvo) { return (alvo == 6 || alvo == 7); }

void aplicaGravidade(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int rotacao)
{

	// baixo pra cima
	for (int l = tamanho - 2; l >= 0; l--)
	{
		for (int c = 0; c < tamanho; c++)
		{
			int caixa = mapa[l][c];

			if (!ehCaixa(caixa))
			{
				continue;
			}

			int destino = l + 1; // p n bater fora do limite

			while (destino < tamanho)
			{
				int alvo = mapa[destino][c];

				if (celulaSustentaBloco(alvo, rotacao))
				{
					break;
				}
				destino++;
			}

			int nova_linha = destino - 1;

			if (nova_linha == l) // apoiada
			{
				continue;
			}

			// Revela debaixo
			if (caixa == 8)
			{
				mapa[l][c] = 6; // A
			}
			else if (caixa == 9)
			{
				mapa[l][c] = 7; // B
			}
			else
			{
				mapa[l][c] = 0;
			}

			int terreno_destino = mapa[nova_linha][c]; // n sumir a porta

			if (terreno_destino == 6)
			{
				mapa[nova_linha][c] = 8;
			}
			else if (terreno_destino == 7)
			{
				mapa[nova_linha][c] = 9;
			}
			else
			{
				mapa[nova_linha][c] = 3;
			}
		}
	}
}

void loopJogo(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int level, int &rotacao, int &plinha, int &pcoluna,
	      int &sob_jogador, bool &sob_alavanca, int &movimentos, int &total_rotacoes, bool &jogo_em_andamento)
{
	limpaInput();
	bool executando_level = true;
	char tecla = 0;
	bool nivel_completo = false;
	bool perdeu = false;
	bool reiniciar = false;

	// desativa o cursor no console
	escondeCursor();
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); // handle da saída do console
	limpaTela();

	imprimeJogo(h, mapa, tamanho, rotacao, plinha, pcoluna, level, movimentos, total_rotacoes);

	while (executando_level && !nivel_completo && !perdeu)
	{
		pegaInput(tecla, executando_level);

		if (tecla != 0)
		{
			atualizaJogo(mapa, tecla, tamanho, rotacao, plinha, pcoluna, sob_jogador, sob_alavanca,
				     nivel_completo, perdeu, reiniciar, total_rotacoes, movimentos);

			if (reiniciar)
			{
				carregaMapa(level, mapa, tamanho, plinha, pcoluna);
				sob_jogador = 0;
				sob_alavanca = false;
				rotacao = 0;
				movimentos = 0;
				total_rotacoes = 0;
				reiniciar = false;
				limpaTela();
			}

			imprimeJogo(h, mapa, tamanho, rotacao, plinha, pcoluna, level, movimentos, total_rotacoes);
		}
		Sleep(30);
	}

	if (nivel_completo)
	{
		tela_vitoria(movimentos, total_rotacoes);
		jogo_em_andamento = true;
	}
	else if (perdeu)
	{
		tela_derrota(movimentos, total_rotacoes);
		jogo_em_andamento = true;
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
	bool sob_alavanca = false;
	int movimentos = 0;
	int total_rotacoes = 0;

	srand(time(NULL));

	SetConsoleOutputCP(850);
	SetConsoleCP(850);
	setlocale(LC_ALL, "Portuguese_Brazil.850");

	limpaTela();
	mostrarTitulo(true);

	// adicionar
	while (executando)
	{
		imprime_menu();
		processaMenu(executando, jogo_em_andamento, level, mapa, tamanho, rotacao, plinha, pcoluna, sob_jogador,
			     sob_alavanca, movimentos, total_rotacoes);
		limpaTela();
	}

	limpaTela();
	cout << "Saindo do jogo..." << endl;

	return 0;
}
