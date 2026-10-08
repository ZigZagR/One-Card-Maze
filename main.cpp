#include <conio.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <windows.h>

using namespace std;
namespace fs = std::filesystem;

// ============================================================================
// CONSTANTES
// ============================================================================
const int MAX_TAMANHO = 30;
const int MAX_MAPAS = 3;
const int MAX_JOGADORES_POR_MAPA = 10;
const int MAX_JOGADORES = MAX_MAPAS * MAX_JOGADORES_POR_MAPA;

enum TipoCelula
{
	VAZIO = 0,
	PAREDE = 1,
	JOGADOR = 2,
	CAIXA = 3,
	ALAVANCA = 4,
	SAIDA = 5,
	PORTA_A = 6,
	PORTA_B = 7,
	CAIXA_SOBRE_A = 8,
	CAIXA_SOBRE_B = 9,
	CAIXA_SOBRE_SAIDA = 10
};

struct Posicao
{
	int l = 0;
	int c = 0;
};

struct Jogador
{
	Posicao pos;
	int sob_jogador = VAZIO; // Célula que estava embaixo antes de pisar
	bool sob_alavanca = false;
};

struct Mapa
{
	int tamanho = 0;
	int grade[MAX_TAMANHO][MAX_TAMANHO];
	Posicao inicio_jogador;
};

struct Partida
{
	int nivel = 0;
	int rotacao = 0;

	int movimentos = 0;
	int total_rotacoes = 0;
	int movimento_caixas = 0;

	// Jogo
	bool em_andamento = false;
	bool nivel_completo = false;
	bool perdeu = false;
	bool reiniciar = false;

	Mapa mapa_original; // Para poder reiniciar a fase ('R')
	Mapa mapa_atual;
	Jogador jogador;
};
struct RegistroPlacar
{
	int mapa = 0;
	string nome;
	int movimentos = 0;
};
struct Placar
{
	RegistroPlacar registros[MAX_JOGADORES];
	int quantidade = 0;
};

// ============================================================================
// FUNÇÕES
// ============================================================================

// PARTIDA
void iniciaPartida(Partida &partida, int nivel);
void reiniciaPartida(Partida &partida);

// MAPA
void copiaMatriz(int origem[MAX_TAMANHO][MAX_TAMANHO],
		 int destino[MAX_TAMANHO][MAX_TAMANHO],
		 int tamanho);
void zeraMatriz(int matriz[MAX_TAMANHO][MAX_TAMANHO]);
void localizaJogador(const Mapa &mapa, Jogador &jogador);
int defineTamanho(int nivel);
void carregaMapa(int nivel, Mapa &mapa, Jogador &jogador);

// REGRAS SIMPLES
bool ehCaixa(int celula);
bool estaSobreAlavanca(int sob_jogador);
bool portaFechada(int porta, int rotacao);
bool celulaPodeSerAtravessada(int celula, int rotacao);
bool celulaSustentaBloco(int celula, int rotacao);

// MOVIMENTO E FISICA
bool movimentoValido(Posicao destino,
		     const Mapa &mapa, int rotacao);

void moveJogador(Partida &partida, Posicao destino);
void esmagaCaixas(Mapa &mapa, int rotacao);
void aplicaGravidade(Partida &partida);
void rotacionaMapa(Partida &partida, char direcao);
void atualizaJogo(Partida &partida, char tecla);

// INTERFACE
void hud(const Partida &partida);
void imprimeJogo(const Partida &partida);
void tela_derrota();
void tela_vitoria(const Partida &partida);

// PLACAR
void arrumaPlacar(Placar &placar);
void adicionaResultado(Placar &placar, const RegistroPlacar &registro);
void carregaArquivo(const string &nome_arquivo, Placar &placar);
void salvaArquivo(const string &nome_arquivo, const Placar &placar);
void mostraPlacar(const Placar &placar, int mapa);
void registraResultado(Placar &placar, int mapa, int movimentos);

// FLUXO DO JOGO
int selecionaMapa();
void loopJogo(Partida &partida, Placar &placar);
void processaMenu(Partida &partida, Placar &placar, bool &executando);

// ============================================================================
// CONSOLE
// ============================================================================

// Código Prof
void gotoxy(int XPos, int YPos)
{
	COORD coord;
	coord.X = XPos; // Propriedade console
	coord.Y = YPos;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void escondeCursor() // prof.
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.dwSize = 100;
	cursor.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}

void limpaTela()
{
	system("cls");
}

void limpaInput()
{
	while (_kbhit())
	{
		_getch();
	}
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

// ============================================================================
// Utils partida
// ============================================================================
void reiniciaPartida(Partida &partida)
{
	// Reinicia a partida com o mapa original
	partida.mapa_atual = partida.mapa_original;

	partida.jogador = Jogador();
	partida.jogador.pos = partida.mapa_original.inicio_jogador;

	partida.rotacao = 0;
	partida.movimentos = 0;
	partida.total_rotacoes = 0;
	partida.movimento_caixas = 0;

	partida.em_andamento = true;
	partida.nivel_completo = false;
	partida.perdeu = false;
	partida.reiniciar = false;
}

void iniciaPartida(Partida &partida, int level)
{
	partida.nivel = level;

	partida.mapa_original.tamanho = defineTamanho(level);
	if (partida.mapa_original.tamanho == 0)
	{
		cout << "Nivel invalido!" << endl;
		return;
	}

	carregaMapa(level, partida.mapa_original, partida.jogador);

	partida.mapa_original.inicio_jogador = partida.jogador.pos;

	reiniciaPartida(partida);
}

// ============================================================================
// Utils mapas
// ============================================================================

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
			matriz[l][c] = VAZIO;
		}
	}
}

void localizaJogador(const Mapa &mapa, Jogador &jogador)
{
	for (int l = 0; l < mapa.tamanho; l++)
	{
		for (int c = 0; c < mapa.tamanho; c++)
		{
			if (mapa.grade[l][c] == JOGADOR)
			{
				jogador.pos.c = c;
				jogador.pos.l = l;
				return;
			}
		}
	}
}

int defineTamanho(int level)
{
	if (level == 0)
	{
		return 11;
	}
	else if (level == 1)
	{
		return 13;
	}
	else if (level == 2)
	{
		return 14;
	}
	return 0;
}

// Carregamento e localizacao dos mapas
void carregaMapa(int nivel, Mapa &mapa, Jogador &jogador)
{
	zeraMatriz(mapa.grade);

	switch (nivel)
	{
	case 0:
	{
		int base[MAX_TAMANHO][MAX_TAMANHO] = {
		    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		    {1, 2, 0, 4, 0, 0, 0, 0, 0, 0, 1},
		    {1, 1, 1, 6, 1, 1, 1, 0, 0, 0, 1},
		    {1, 0, 1, 0, 0, 0, 1, 1, 6, 1, 1},
		    {1, 0, 7, 0, 0, 1, 1, 3, 0, 0, 1},
		    {1, 0, 1, 1, 6, 1, 4, 0, 0, 1, 1},
		    {1, 0, 1, 1, 0, 1, 1, 1, 6, 1, 1},
		    {1, 0, 1, 0, 3, 0, 0, 7, 0, 0, 1},
		    {1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1},
		    {1, 4, 1, 0, 0, 0, 0, 1, 0, 5, 1},
		    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

		copiaMatriz(base, mapa.grade, mapa.tamanho);
		localizaJogador(mapa, jogador);

		break;
	}
	case 1:
	{
		int base[MAX_TAMANHO][MAX_TAMANHO] =
		    {
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{1, 2, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1},
			{1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
			{1, 0, 1, 3, 0, 0, 0, 3, 0, 0, 1, 0, 1},
			{1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
			{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
			{1, 0, 0, 3, 0, 6, 0, 0, 0, 0, 0, 0, 1},
			{1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{1, 0, 7, 0, 0, 3, 0, 0, 0, 0, 0, 0, 1},
			{1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
			{1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 5, 1},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

		copiaMatriz(base, mapa.grade, mapa.tamanho);
		localizaJogador(mapa, jogador);

		break;
	}
	case 2:
	{
		int base[MAX_TAMANHO][MAX_TAMANHO] = {
		    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		    {1, 2, 0, 0, 1, 0, 4, 0, 1, 3, 0, 7, 5, 1},
		    {1, 1, 1, 4, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1},
		    {1, 0, 6, 0, 1, 0, 1, 0, 6, 0, 1, 3, 0, 1},
		    {1, 0, 1, 1, 1, 6, 1, 6, 1, 0, 1, 0, 1, 1},
		    {1, 0, 1, 4, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1},
		    {1, 0, 1, 0, 1, 1, 1, 0, 1, 7, 1, 0, 1, 1},
		    {1, 0, 0, 0, 1, 3, 0, 0, 1, 4, 7, 0, 1, 1},
		    {1, 1, 1, 6, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1},
		    {1, 0, 0, 0, 0, 4, 1, 3, 0, 0, 1, 0, 1, 1},
		    {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1},
		    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 6, 0, 0, 1},
		    {1, 1, 1, 1, 1, 0, 0, 6, 0, 0, 1, 1, 4, 1},
		    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

		copiaMatriz(base, mapa.grade, mapa.tamanho);
		localizaJogador(mapa, jogador);

		break;
	}
	default:
		break;
	}
}

// ============================================================================
// REGRAS DE COLISÃO, MOVIMENTO E FÍSICA (GRAVIDADE / ROTAÇÃO)
// ============================================================================

bool ehCaixa(int celula)
{
	return celula == CAIXA || celula == CAIXA_SOBRE_A || celula == CAIXA_SOBRE_B || celula == CAIXA_SOBRE_SAIDA;
}

bool estaSobreAlavanca(int sob_jogador)
{
	return sob_jogador == ALAVANCA;
}

bool portaFechada(int porta, int rotacao)
{
	if (porta == PORTA_A)
	{
		return rotacao == 0 || rotacao == 180;
	}
	else if (porta == PORTA_B)
	{
		return rotacao == 90 || rotacao == 270;
	}
	return false;
}

/*Alterar p matar
void fechaPortaJogador(int rotacao, int sob_jogador, bool &perdeu)
{
	if ((sob_jogador == PORTA_A || sob_jogador == PORTA_B) && portaFechada(sob_jogador, rotacao))
	{
		perdeu = true;
	}
}
*/

bool celulaPodeSerAtravessada(int celula, int rotacao)
{
	if (celula == PAREDE || ehCaixa(celula) || celula == JOGADOR) // Alterar p matar
	{
		return false; // Parede e caixa
	}
	if (celula == PORTA_A && portaFechada(PORTA_A, rotacao))
	{
		return false; // A
	}
	if (celula == PORTA_B && portaFechada(PORTA_B, rotacao))
	{
		return false; // B
	}
	return true;
}

bool celulaSustentaBloco(int celula, int rotacao)
{
	return !celulaPodeSerAtravessada(celula, rotacao) || celula == ALAVANCA; // n pode atravessar == solido
}

bool movimentoValido(Posicao destino, const Mapa &mapa, int rotacao)
{
	if (destino.l < 0 || destino.l >= mapa.tamanho || destino.c < 0 || destino.c >= mapa.tamanho)
	{
		return false; // fora dos limites
	}
	return celulaPodeSerAtravessada(mapa.grade[destino.l][destino.c], rotacao);
}

void moveJogador(Partida &partida, Posicao destino)
{
	partida.mapa_atual.grade[partida.jogador.pos.l][partida.jogador.pos.c] = partida.jogador.sob_jogador; // bota base antiga
	partida.jogador.sob_jogador = partida.mapa_atual.grade[destino.l][destino.c];			      // salva
	partida.jogador.sob_alavanca = estaSobreAlavanca(partida.jogador.sob_jogador);

	partida.mapa_atual.grade[destino.l][destino.c] = JOGADOR; // move
	partida.jogador.pos.l = destino.l;
	partida.jogador.pos.c = destino.c;

	partida.movimentos++;

	if (partida.jogador.sob_jogador == SAIDA)
	{
		partida.nivel_completo = true;
	}
}

// Rotacao e gravidade das caixas
void esmagaCaixas(Mapa &mapa, int rotacao)
{
	for (int l = 0; l < mapa.tamanho; l++)
	{
		for (int c = 0; c < mapa.tamanho; c++)
		{
			if (mapa.grade[l][c] == CAIXA_SOBRE_A && portaFechada(PORTA_A, rotacao))
			{
				mapa.grade[l][c] = PORTA_A; // kaboom A
			}
			else if (mapa.grade[l][c] == CAIXA_SOBRE_B && portaFechada(PORTA_B, rotacao))
			{
				mapa.grade[l][c] = PORTA_B; // kaboom B
			}
		}
	}
}

/* Alterar p matar
void aplicaGravidadeJogador(int mapa[MAX_TAMANHO][MAX_TAMANHO], int tamanho, int rotacao,
			    int &plinha, int pcoluna, int &sob_jogador,
			    bool &sob_alavanca, bool &nivel_completo)
{
	int destino = plinha + 1;

	// desce.
	while (destino < tamanho)
	{
		int alvo = mapa[destino][pcoluna];

		if (celulaSustentaBloco(alvo, rotacao))
		{
			break;
		}
		destino++;
	}

	int nova_linha = destino - 1;

	if (nova_linha > plinha)
	{
		mapa[plinha][pcoluna] = sob_jogador;     // restaura
		sob_jogador = mapa[nova_linha][pcoluna]; // salva
		sob_alavanca = estaSobreAlavanca(sob_jogador);

		mapa[nova_linha][pcoluna] = JOGADOR;
		plinha = nova_linha;

		if (sob_jogador == SAIDA) // saída né
		{
			nivel_completo = true;
		}
	}
}
*/

void aplicaGravidade(Partida &partida) // add perdeu se for matar
{
	// baixo pra cima
	for (int l = partida.mapa_atual.tamanho - 2; l >= 0; l--)
	{
		for (int c = 0; c < partida.mapa_atual.tamanho; c++)
		{
			int caixa = partida.mapa_atual.grade[l][c];

			if (!ehCaixa(caixa))
			{
				continue;
			}

			int destino = l + 1; // p n bater fora do limite

			while (destino < partida.mapa_atual.tamanho)
			{
				int alvo = partida.mapa_atual.grade[destino][c];

				/* Alterar p matar
				if (alvo == 2)
				{
					perdeu = true;
				}
				*/

				if (celulaSustentaBloco(alvo, partida.rotacao))
				{
					break;
				}
				partida.movimento_caixas++;
				destino++;
			}

			int nova_linha = destino - 1;

			if (nova_linha == l) // apoiada
			{
				continue;
			}

			// Revela debaixo
			if (caixa == CAIXA_SOBRE_A)
			{
				partida.mapa_atual.grade[l][c] = PORTA_A; // A
			}
			else if (caixa == CAIXA_SOBRE_B)
			{
				partida.mapa_atual.grade[l][c] = PORTA_B; // B
			}
			else if (caixa == CAIXA_SOBRE_SAIDA)
			{
				partida.mapa_atual.grade[l][c] = SAIDA; // s
			}
			else
			{
				partida.mapa_atual.grade[l][c] = VAZIO;
			}

			int terreno_destino = partida.mapa_atual.grade[nova_linha][c]; // n sumir a porta

			if (terreno_destino == PORTA_A)
			{
				partida.mapa_atual.grade[nova_linha][c] = CAIXA_SOBRE_A;
			}
			else if (terreno_destino == PORTA_B)
			{
				partida.mapa_atual.grade[nova_linha][c] = CAIXA_SOBRE_B;
			}
			else if (terreno_destino == SAIDA)
			{
				partida.mapa_atual.grade[nova_linha][c] = CAIXA_SOBRE_SAIDA;
			}
			else
			{
				partida.mapa_atual.grade[nova_linha][c] = CAIXA;
			}
		}
	}
}

void rotacionaMapa(Partida &partida, char direcao)
{
	int nova_rotacao = partida.rotacao;

	if (direcao == 'e' || direcao == 'E')
	{
		nova_rotacao = (partida.rotacao + 90) % 360;
	}
	else if (direcao == 'q' || direcao == 'Q')
	{
		nova_rotacao = (partida.rotacao + 270) % 360;
	}

	// Bota oq tá embaixo do jogador na matriz p n perder
	int origem[MAX_TAMANHO][MAX_TAMANHO];
	copiaMatriz(partida.mapa_atual.grade, origem, partida.mapa_atual.tamanho);
	origem[partida.jogador.pos.l][partida.jogador.pos.c] = partida.jogador.sob_jogador;

	int novo_mapa[MAX_TAMANHO][MAX_TAMANHO];
	zeraMatriz(novo_mapa);

	for (int l = 0; l < partida.mapa_atual.tamanho; l++)
	{
		for (int c = 0; c < partida.mapa_atual.tamanho; c++)
		{
			int nova_linha, nova_coluna;

			if (direcao == 'e' || direcao == 'E') // Inverte pra direita / relógio
			{
				nova_linha = c;
				nova_coluna = partida.mapa_atual.tamanho - 1 - l;
			}
			else // Inverta pra esquerda / antihorário
			{
				nova_linha = partida.mapa_atual.tamanho - 1 - c;
				nova_coluna = l;
			}

			novo_mapa[nova_linha][nova_coluna] = origem[l][c];
		}
	}

	// acha jogador pelo índice
	int plinha_antigo = partida.jogador.pos.l;
	int pcoluna_antigo = partida.jogador.pos.c;

	if (direcao == 'e' || direcao == 'E')
	{
		partida.jogador.pos.l = pcoluna_antigo;
		partida.jogador.pos.c = partida.mapa_atual.tamanho - 1 - plinha_antigo;
	}
	else
	{
		partida.jogador.pos.l = partida.mapa_atual.tamanho - 1 - pcoluna_antigo;
		partida.jogador.pos.c = plinha_antigo;
	}

	partida.jogador.sob_jogador = novo_mapa[partida.jogador.pos.l][partida.jogador.pos.c]; // guarda
	partida.jogador.sob_alavanca = estaSobreAlavanca(partida.jogador.sob_jogador);
	novo_mapa[partida.jogador.pos.l][partida.jogador.pos.c] = JOGADOR;

	copiaMatriz(novo_mapa, partida.mapa_atual.grade, partida.mapa_atual.tamanho);
	partida.rotacao = nova_rotacao;

	// Esmaga qualquer caixa que tenha ficado sobre uma porta que fechou
	esmagaCaixas(partida.mapa_atual, partida.rotacao);

	// Aplica gravidade nos blocos que sobraram
	aplicaGravidade(partida); // add perdeu se for matar

	/* Alterar p matar
	aplicaGravidadeJogador(partida.mapa_atual.grade, partida.mapa_atual.tamanho, partida.rotacao, partida.jogador.pos.l, partida.jogador.pos.c, partida.sob_jogador, partida.sob_alavanca, partida.nivel_completo);

	fechaPortaJogador(partida.rotacao, partida.sob_jogador, partida.perdeu); // Caso precise matar o jogador com a gravidade

	if (partida.perdeu)
	{
		return;
	}
	*/
}

// Entrada e regras de movimento
void atualizaJogo(Partida &partida, char tecla)
{
	int prox_l = partida.jogador.pos.l;
	int prox_c = partida.jogador.pos.c;

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
		partida.reiniciar = true;
		return;
	}
	else if ((tecla == 'q' || tecla == 'Q' || tecla == 'e' || tecla == 'E') && partida.jogador.sob_alavanca)
	{
		rotacionaMapa(partida, tecla);

		partida.total_rotacoes++;
		return;
	}
	else
	{
		return;
	}

	if (movimentoValido({prox_l, prox_c}, partida.mapa_atual, partida.rotacao))
	{
		moveJogador(partida, {prox_l, prox_c});
	}
}

// ============================================================================
// GUI
// ============================================================================

// INFO do HUD
void hud(const Partida &partida)
{
	cout << "\n";
	cout << "Mapa: " << (partida.nivel + 1) << "   ";
	cout << "Orientacao: " << partida.rotacao << " graus   ";
	cout << "Movimentos: " << partida.movimentos << "   ";
	cout << "Rotacoes: " << partida.total_rotacoes << "    ";
	cout << "Caixas: " << partida.movimento_caixas << "    \n";
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
	_getch();
}

void imprimeJogo(const Partida &partida)
{
	gotoxy(0, 0);

	mostrarTitulo(false);

	for (int l = 0; l < partida.mapa_atual.tamanho; l++)
	{
		cout << "  ";
		for (int c = 0; c < partida.mapa_atual.tamanho; c++)
		{
			if ((l == partida.jogador.pos.l && c == partida.jogador.pos.c) || partida.mapa_atual.grade[l][c] == JOGADOR)
			{
				cout << "@ ";
			}
			else
			{
				switch (partida.mapa_atual.grade[l][c])
				{
				case VAZIO:
					cout << "  ";
					break;
				case PAREDE:
					cout << "# ";
					break;
				case CAIXA:
				case CAIXA_SOBRE_A:	// caixa sobre A
				case CAIXA_SOBRE_B:	// caixa sobre B
				case CAIXA_SOBRE_SAIDA: // caixa sobre S
					cout << "O ";
					break;
				case ALAVANCA:
					cout << "A ";
					break;
				case SAIDA:
					cout << "S ";
					break;
				case PORTA_A:
					if (partida.rotacao == 0 || partida.rotacao == 180)
					{
						cout << "= ";
					}
					else
					{
						cout << ": ";
					}
					break;
				case PORTA_B:
					if (partida.rotacao == 90 || partida.rotacao == 270)
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

	hud(partida);
}

void tela_derrota()
{
	limpaTela();
	cout << "====================================\n";
	cout << "             GAME OVER!             \n";
	cout << "          Voce foi esmagado.  	     \n";
	cout << "====================================\n\n";
	cout << "Pressione qualquer tecla para voltar...";
	limpaInput();
	_getch();
}

void tela_vitoria(const Partida &partida)
{
	limpaTela();
	cout << "====================================\n";
	cout << "         VOCE VENCEU!               \n";
	cout << "====================================\n";
	cout << "Movimentos: " << partida.movimentos << "\n";
	cout << "Rotacoes: " << partida.total_rotacoes << "\n\n";
	cout << "Pressione qualquer tecla para voltar...";
	limpaInput();
	_getch();
}

// ============================================================================
// PLACAR
// ============================================================================

// Ordena por mapa e depois pelo menor numero de movimentos
void arrumaPlacar(Placar &placar)
{
	sort(
	    placar.registros,
	    placar.registros + placar.quantidade,
	    [](const RegistroPlacar &a, const RegistroPlacar &b)
	    {
		    if (a.mapa != b.mapa)
			    return a.mapa < b.mapa;

		    return a.movimentos < b.movimentos;
	    });
}

// Adiciona um resultado, mantendo somente os 10 melhores por mapa
void adicionaResultado(Placar &placar, const RegistroPlacar &registro)
{
	if (registro.mapa < 1 || registro.mapa > MAX_MAPAS ||
	    registro.movimentos < 0)
		return;

	// Conta os resultados do mapa e procura o pior
	int quantidade_mapa = 0;
	int indice_pior = -1;

	for (int i = 0; i < placar.quantidade; i++)
	{
		if (placar.registros[i].mapa == registro.mapa)
		{
			quantidade_mapa++;

			if (indice_pior == -1 ||
			    placar.registros[i].movimentos >
				placar.registros[indice_pior].movimentos)
			{
				indice_pior = i;
			}
		}
	}

	// Se ja existem 10 resultados, so aceita um melhor
	if (quantidade_mapa >= MAX_JOGADORES_POR_MAPA)
	{
		if (registro.movimentos >=
		    placar.registros[indice_pior].movimentos)
		{
			return;
		}

		// Substitui o pior resultado
		placar.registros[indice_pior] = registro;
	}
	else if (placar.quantidade < MAX_JOGADORES)
	{
		// Ainda existe espaco no placar
		placar.registros[placar.quantidade] = registro;
		placar.quantidade++;
	}
	else
	{
		// Capacidade total atingida
		return;
	}

	arrumaPlacar(placar);
}

// Carrega os resultados salvos anteriormente
void carregaArquivo(const string &nome_arquivo, Placar &placar)
{
	ifstream arquivo(nome_arquivo);

	placar.quantidade = 0;

	if (!arquivo.is_open())
		return;

	RegistroPlacar registro;

	// Formato: mapa movimentos nome
	while (arquivo >> registro.mapa >> registro.movimentos)
	{
		arquivo >> ws;
		getline(arquivo, registro.nome);

		if (!arquivo)
			break;

		if (registro.nome.empty())
			registro.nome = "Jogador";

		adicionaResultado(placar, registro);
	}

	arquivo.close();
}

// Salva todos os resultados
void salvaArquivo(const string &nome_arquivo, const Placar &placar)
{
	ofstream arquivo(nome_arquivo);

	if (!arquivo.is_open())
	{
		cout << "Erro ao salvar o placar!\n";
		return;
	}

	for (int i = 0; i < placar.quantidade; i++)
	{
		const RegistroPlacar &registro = placar.registros[i];

		arquivo << registro.mapa << " "
			<< registro.movimentos << " "
			<< registro.nome << "\n";
	}

	arquivo.close();
}

// Mostra os melhores resultados de um mapa
void mostraPlacar(const Placar &placar, int mapa)
{
	cout << "\n========== TOP 10 - MAPA " << mapa
	     << " ==========\n\n";

	int posicao = 1;

	for (int i = 0; i < placar.quantidade; i++)
	{
		const RegistroPlacar &registro = placar.registros[i];

		if (registro.mapa != mapa)
			continue;

		cout << posicao << ". "
		     << registro.nome
		     << " - " << registro.movimentos
		     << " movimentos\n";

		posicao++;
	}

	if (posicao == 1)
		cout << "Nenhum resultado registrado.\n";

	cout << "\n====================================\n";
}

// Solicita o nome e tenta registrar a pontuacao
void registraResultado(Placar &placar, int mapa, int movimentos)
{
	RegistroPlacar registro;

	registro.mapa = mapa;
	registro.movimentos = movimentos;

	cout << "\nDigite seu nome: ";
	getline(cin, registro.nome);

	if (registro.nome.empty())
		registro.nome = "Jogador";

	// Mantem o formato do arquivo: um registro por linha
	for (char &caractere : registro.nome)
	{
		if (caractere == '\n' || caractere == '\r')
			caractere = ' ';
	}

	adicionaResultado(placar, registro);
}

// ============================================================================
// JOGO e MENU
// ============================================================================

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
		char opcao = _getch();

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
				char opcao = _getch();

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

void loopJogo(Partida &partida, Placar &placar)
{
	limpaInput();
	char tecla = 0;
	bool executando_level = true;

	// desativa o cursor no console
	escondeCursor();
	limpaTela();

	imprimeJogo(partida);

	while (executando_level && !partida.nivel_completo && !partida.perdeu)
	{
		pegaInput(tecla, executando_level);

		if (tecla != 0)
		{
			atualizaJogo(partida, tecla);

			if (partida.reiniciar)
			{
				reiniciaPartida(partida);
			}

			imprimeJogo(partida);
		}
		Sleep(30);
	}

	if (partida.nivel_completo)
	{
		limpaInput();

		tela_vitoria(partida);

		registraResultado(
		    placar,
		    partida.nivel + 1,
		    partida.movimentos);

		salvaArquivo("placar.txt", placar);

		limpaTela();

		mostraPlacar(placar, partida.nivel + 1);

		cout << "\nPressione qualquer tecla para voltar...";
		_getch();

		partida.em_andamento = false;
	}
	else if (partida.perdeu)
	{
		tela_derrota();
		partida.em_andamento = false;
	}
	else
	{
		// ESC
		partida.em_andamento = true;
	}
}

void processaMenu(Partida &partida, Placar &placar, bool &executando)
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
					iniciaPartida(partida, mapa_escolhido);

					loopJogo(partida, placar);
				}

				break;
			}
			else if (tecla == '2')
			{
				if (partida.em_andamento)
				{
					loopJogo(partida, placar);
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

// ============================================================================
// MAIN
// ============================================================================

int main()
{
	Partida partida{};

	Placar placar{};

	bool executando = true;
	bool primeira_vez = true;

	carregaArquivo("placar.txt", placar);

	srand(time(NULL));

	SetConsoleOutputCP(850);
	SetConsoleCP(850);
	setlocale(LC_ALL, "Portuguese_Brazil.850");

	limpaTela();

	while (executando)
	{
		if (primeira_vez)
		{
			mostrarTitulo(true);
			primeira_vez = false;
		}
		else
		{
			mostrarTitulo(false);
		}

		imprime_menu();
		processaMenu(partida, placar, executando);
		limpaTela();
	}

	limpaTela();
	cout << "Saindo do jogo..." << endl;

	return 0;
}
