#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <conio.h>
#include <windows.h>

#define SIZE 11            // Tamanho do tabuleiro (11x11)
#define LOOP_LEN 38        // Numero de casas do caminho principal (loop fechado)
#define HOME_LEN 3         // Casas da reta final (antes do centro)
#define FINISH (LOOP_LEN + HOME_LEN) // progresso necessario para chegar ao centro

// Estados possiveis de uma peca
#define BASE  0   // dentro de casa (fora do tabuleiro)
#define TRACK 1   // andando pelo caminho
#define DONE  2   // chegou ao centro

// ESTRUTURA DE CADA PECA
typedef struct {
    int x;        // linha atual no tabuleiro
    int y;        // coluna atual no tabuleiro
    int state;    // BASE / TRACK / DONE
    int progress; // passos dados desde que saiu da base (0..FINISH)
    int homeX;    // posicao na base
    int homeY;
} Peca;

// ESTRUTURA DOS JOGADORES
typedef struct {
    Peca pecas[4];
    char cor;
    int startX;
    int startY;
    int startPathIndex;              // onde essa cor entra no caminho principal
    const int (*caminhoFinal)[2];    // reta final exclusiva da cor (HOME_LEN casas)
} Jogador;

// CAMINHO PRINCIPAL: loop fechado de 38 casas (validado casa a casa).
// Depois da casa 37 volta para a casa 0.
const int caminho[LOOP_LEN][2] = {
    {6, 9}, {6, 8}, {6, 7}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6}, {10, 5}, {10, 4},
    {9, 4}, {8, 4}, {7, 4}, {6, 4}, {6, 3}, {6, 2}, {6, 1}, {6, 0}, {5, 0}, {4, 0},
    {4, 1}, {4, 2}, {4, 3}, {4, 4}, {3, 4}, {2, 4}, {1, 4}, {0, 4}, {0, 5}, {0, 6},
    {1, 6}, {2, 6}, {3, 6}, {4, 6}, {4, 7}, {4, 8}, {4, 9}, {5, 9}
};

// RETAS FINAIS: da entrada em direcao ao centro (a ultima casa fica colada no centro).
const int caminhos_finais_vermelho[HOME_LEN][2] = {{2, 5}, {3, 5}, {4, 5}}; // braco de cima
const int caminhos_finais_amarelo[HOME_LEN][2]  = {{5, 2}, {5, 3}, {5, 4}}; // braco da esquerda
const int caminhos_finais_azul[HOME_LEN][2]     = {{5, 8}, {5, 7}, {5, 6}}; // braco da direita
const int caminhos_finais_verde[HOME_LEN][2]    = {{8, 5}, {7, 5}, {6, 5}}; // braco de baixo
const int posicao_final[2] = {5, 5}; // Centro "W" (chegada)

// POSICIONA O CURSOR NO CONSOLE
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = (SHORT)x;
    coord.Y = (SHORT)y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// INICIALIZA O DESENHO DO TABULEIRO
void inicializarTabuleiro(char tabuleiro[SIZE][SIZE]) {
    int i, j;
    char tabuleiroInicial[SIZE][SIZE] = {
        {'Y','Y','Y','Y','D','D','D','Y','Y','Y','Y'},
        {'Y',' ',' ','Y','D','X','D','Y',' ',' ','Y'},
        {'Y',' ',' ','Y','D','X','D','Y',' ',' ','Y'},
        {'Y','Y','Y','Y','D','X','D','Y','Y','Y','Y'},
        {'D','D','D','D','D','X','D','D','D','D','D'},
        {'D','X','X','X','X','W','X','X','X','X','D'},
        {'D','D','D','D','D','X','D','D','D','D','D'},
        {'Y','Y','Y','Y','D','X','D','Y','Y','Y','Y'},
        {'Y',' ',' ','Y','D','X','D','Y',' ',' ','Y'},
        {'Y',' ',' ','Y','D','X','D','Y',' ',' ','Y'},
        {'Y','Y','Y','Y','D','D','D','Y','Y','Y','Y'}
    };
    for(i = 0; i < SIZE; i++)
        for(j = 0; j < SIZE; j++)
            tabuleiro[i][j] = tabuleiroInicial[i][j];
}

// IMPRIME O TABULEIRO COLORIDO
void imprimirTabuleiro(char tabuleiro[SIZE][SIZE]) {
    int i, j;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    system("cls");
    for(i = 0; i < SIZE; i++) {
        for(j = 0; j < SIZE; j++) {
            gotoxy(j * 2, i);
            char c = tabuleiro[i][j];
            if (c == 'R') {
                SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_INTENSITY);
            } else if (c == 'A') {
                SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            } else if (c == 'B') {
                SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            } else if (c == 'G') {
                SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            } else {
                SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            printf("%c", c);
        }
    }
    SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    gotoxy(0, SIZE);
}

// INICIALIZA OS JOGADORES (todas as pecas comecam na base)
void inicializarJogadores(Jogador jogadores[4]) {
    // Vermelho (base: canto superior esquerdo)
    jogadores[0] = (Jogador){{{1,1,BASE,0,1,1},{2,1,BASE,0,2,1},{1,2,BASE,0,1,2},{2,2,BASE,0,2,2}},
                             'R', 4, 1, 20, caminhos_finais_vermelho};
    // Amarelo (base: canto superior direito)
    jogadores[1] = (Jogador){{{1,8,BASE,0,1,8},{2,8,BASE,0,2,8},{1,9,BASE,0,1,9},{2,9,BASE,0,2,9}},
                             'A', 1, 6, 30, caminhos_finais_amarelo};
    // Azul (base: canto inferior direito)
    jogadores[2] = (Jogador){{{8,8,BASE,0,8,8},{9,8,BASE,0,9,8},{8,9,BASE,0,8,9},{9,9,BASE,0,9,9}},
                             'B', 6, 9, 0, caminhos_finais_azul};
    // Verde (base: canto inferior esquerdo)
    jogadores[3] = (Jogador){{{8,1,BASE,0,8,1},{9,1,BASE,0,9,1},{8,2,BASE,0,8,2},{9,2,BASE,0,9,2}},
                             'G', 9, 4, 10, caminhos_finais_verde};
}

// ROLA O DADO (1-6)
int rolarDado() {
    return rand() % 6 + 1;
}

// CALCULA (x,y) a partir do progresso de uma peca
void posicaoNoCaminho(const Jogador *j, int progress, int *x, int *y) {
    if (progress < LOOP_LEN) {
        int idx = (j->startPathIndex + progress) % LOOP_LEN;
        *x = caminho[idx][0];
        *y = caminho[idx][1];
    } else if (progress < FINISH) {
        *x = j->caminhoFinal[progress - LOOP_LEN][0];
        *y = j->caminhoFinal[progress - LOOP_LEN][1];
    } else { // progress == FINISH -> centro
        *x = posicao_final[0];
        *y = posicao_final[1];
    }
}

// A peca 'idx' consegue se mover com esse valor de dado?
int pecaPodeMover(const Jogador *j, int idx, int passos) {
    if (idx < 0 || idx > 3) return 0;
    const Peca *p = &j->pecas[idx];
    if (p->state == DONE) return 0;                 // ja chegou
    if (p->state == BASE) return (passos == 6);     // so sai da base com 6
    return (p->progress + passos <= FINISH);        // precisa cair exato no centro
}

// MOVE A PECA. Retorna 1 se moveu, 0 se a jogada era invalida.
int moverPeca(Jogador *jogador, int indicePeca, int passos) {
    if (!pecaPodeMover(jogador, indicePeca, passos)) return 0;
    Peca *peca = &jogador->pecas[indicePeca];

    if (peca->state == BASE) {
        peca->state = TRACK;
        peca->progress = 0;
    } else {
        peca->progress += passos;
        if (peca->progress == FINISH) peca->state = DONE;
    }
    posicaoNoCaminho(jogador, peca->progress, &peca->x, &peca->y);
    return 1;
}

// CAPTURA: se a peca recem-movida cair sobre peca adversaria no caminho
// principal, a adversaria volta para a base. (Nao captura na reta final/centro.)
void verificarCaptura(Jogador jogadores[4], int jogadorAtual, int indicePeca) {
    Peca *movida = &jogadores[jogadorAtual].pecas[indicePeca];
    if (movida->state != TRACK || movida->progress >= LOOP_LEN) return;

    for (int i = 0; i < 4; i++) {
        if (i == jogadorAtual) continue;
        for (int k = 0; k < 4; k++) {
            Peca *alvo = &jogadores[i].pecas[k];
            if (alvo->state == TRACK && alvo->progress < LOOP_LEN &&
                alvo->x == movida->x && alvo->y == movida->y) {
                alvo->state = BASE;
                alvo->progress = 0;
                alvo->x = alvo->homeX;
                alvo->y = alvo->homeY;
            }
        }
    }
}

// Todas as 4 pecas do jogador chegaram ao centro?
int jogadorVenceu(const Jogador *j) {
    for (int i = 0; i < 4; i++)
        if (j->pecas[i].state != DONE) return 0;
    return 1;
}

// ATUALIZA O DESENHO DO TABULEIRO COM AS PECAS
void atualizarTabuleiro(char tabuleiro[SIZE][SIZE], Jogador jogadores[4]) {
    inicializarTabuleiro(tabuleiro);
    for (int i = 0; i < 4; i++) {
        Jogador *jogador = &jogadores[i];
        for (int j = 0; j < 4; j++) {
            Peca *peca = &jogador->pecas[j];
            if (peca->state == BASE)
                tabuleiro[peca->homeX][peca->homeY] = jogador->cor;
            else
                tabuleiro[peca->x][peca->y] = jogador->cor;
        }
    }
}

// Le um inteiro do teclado com seguranca (limpa o buffer em caso de erro)
int lerInteiro() {
    int v;
    if (scanf("%d", &v) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { /* descarta lixo */ }
        return INT_MIN;
    }
    return v;
}

#ifndef UNIT_TEST
// LOOP PRINCIPAL DO JOGO
void jogarLudo() {
    srand((unsigned)time(NULL));
    char tabuleiro[SIZE][SIZE];
    Jogador jogadores[4];
    int jogadorAtual = 0;

    inicializarTabuleiro(tabuleiro);
    inicializarJogadores(jogadores);
    atualizarTabuleiro(tabuleiro, jogadores);

    while (1) {
        imprimirTabuleiro(tabuleiro);
        int dado = rolarDado();
        printf("Jogador %c, voce tirou %d no dado.\n", jogadores[jogadorAtual].cor, dado);

        // Quais pecas podem se mover com esse dado?
        int movable[4], nm = 0;
        for (int i = 0; i < 4; i++)
            if (pecaPodeMover(&jogadores[jogadorAtual], i, dado))
                movable[nm++] = i;

        if (nm == 0) {
            printf("Nenhuma jogada possivel. Passando a vez...\n");
            printf("Pressione uma tecla para continuar...\n");
            _getch();
        } else {
            printf("Pecas que podem mover: ");
            for (int i = 0; i < nm; i++) printf("%d ", movable[i] + 1);
            printf("\n");

            int pecaEscolhida = -1;
            do {
                printf("Escolha uma peca para mover: ");
                pecaEscolhida = lerInteiro() - 1;
                if (!pecaPodeMover(&jogadores[jogadorAtual], pecaEscolhida, dado))
                    printf("Jogada invalida! Tente novamente.\n");
            } while (!pecaPodeMover(&jogadores[jogadorAtual], pecaEscolhida, dado));

            moverPeca(&jogadores[jogadorAtual], pecaEscolhida, dado);
            verificarCaptura(jogadores, jogadorAtual, pecaEscolhida);
        }

        atualizarTabuleiro(tabuleiro, jogadores);

        if (jogadorVenceu(&jogadores[jogadorAtual])) {
            imprimirTabuleiro(tabuleiro);
            printf("\n*** JOGADOR %c VENCEU O JOGO! Parabens! ***\n", jogadores[jogadorAtual].cor);
            printf("Pressione uma tecla para voltar ao menu...\n");
            _getch();
            return;
        }

        // Tirar 6 da direito a jogar de novo
        if (dado != 6)
            jogadorAtual = (jogadorAtual + 1) % 4;
    }
}

// TELA DE BOAS VINDAS
void mostrarBoasVindas() {
    printf("[][][][][][][][][][][][][][][][][][][][][][][][][]\n");
    printf("[][][][][][][][][][][][][][][][][][][][][][][][][]\n");
    printf("[]                                              []\n");
    printf("[]                                              []\n");
    printf("[]          Bem-vindo ao Jogo de Ludo!          []\n");
    printf("[]                                              []\n");
    printf("[]                                              []\n");
    printf("[]  Prepare-se para muita diversao e desafios!  []\n");
    printf("[]                                              []\n");
    printf("[]            Pressione uma tecla!              []\n");
    printf("[]                                              []\n");
    printf("[]                                              []\n");
    printf("[]                                              []\n");
    printf("[]             ####################             []\n");
    printf("[]             ####################             []\n");
    printf("[]             ###              ###             []\n");
    printf("[]             ###  ####  ####  ###             []\n");
    printf("[]             ###  ####  ####  ###             []\n");
    printf("[]             ###              ###             []\n");
    printf("[]             ###  ####  ####  ###             []\n");
    printf("[]             ###  ####  ####  ###             []\n");
    printf("[]             ###              ###             []\n");
    printf("[]             ####################             []\n");
    printf("[]             ####################             []\n");
    printf("[]                                              []\n");
    printf("[][][][][][][][][][][][][][][][][][][][][][][][][]\n");
    printf("[][][][][][][][][][][][][][][][][][][][][][][][][]\n");
    printf("[][][][][][][][][][][][][][][][][][][][][][][][][]\n");
    printf("\nAperte qualquer tecla!\n");
    _getch();
}

// REGRAS
void mostrarRegras() {
    system("cls");
    printf("Regras do Jogo Ludo:\n");
    printf("1. Cada jogador tem 4 pecas.\n");
    printf("2. O objetivo eh levar todas as pecas ate o centro.\n");
    printf("3. As pecas se movem com base no numero obtido no dado.\n");
    printf("4. Voce so tira uma peca da base se tirar 6 no dado.\n");
    printf("5. Tirar 6 da direito a jogar novamente.\n");
    printf("6. Cair sobre uma peca adversaria manda ela de volta pra base.\n");
    printf("7. Para chegar ao centro e preciso tirar o numero exato.\n");
    printf("\nPressione qualquer tecla para voltar ao menu...\n");
    _getch();
}

// RANKING (placeholder)
void mostrarRanking() {
    system("cls");
    printf("Ranking dos Jogadores:\n");
    printf("1. Jogador A: 100 pontos\n");
    printf("2. Jogador B: 80 pontos\n");
    printf("3. Jogador C: 60 pontos\n");
    printf("4. Jogador D: 40 pontos\n");
    printf("\nPressione qualquer tecla para voltar ao menu...\n");
    _getch();
}

// MENU PRINCIPAL
void mostrarMenu() {
    int escolha;
    do {
        system("cls");
        printf(" _________        .---\"\"\"      \"\"\"---.              \n");
        printf(":______.-':      :  .--------------.  :             \n");
        printf("| ______  |      | :                : |             \n");
        printf("|:______B:|      | |  LUDO GAME !   | |             \n");
        printf("|:______B:|      | |                | |             \n");
        printf("|:______B:|      | |  1.NovoJogo    | |             \n");
        printf("|         |      | |  2.Regras      | |             \n");
        printf("|:_____:  |      | |  3.Rank        | |             \n");
        printf("|    ==   |      | :  4.Sair        : |             \n");
        printf("|       O |      :  '--------------'  :             \n");
        printf("|       o |      :'---...______...---'              \n");
        printf("|       o |-._.-i___/'             \\._              \n");
        printf("|'-.____o_|   '-.   '-...______...-'  `-._          \n");
        printf(":_________:      `.____________________   `-.___.-. \n");
        printf("                 .'.eeeeeeeeeeeeeeeeee.'.      :___:\n");
        printf("               .'.eeeeeeeeeeeeeeeeeeeeee.'.         \n");
        printf("              :____________________________:\n");
        printf("Digite a opcao desejada: ");
        escolha = lerInteiro();

        switch (escolha) {
            case 1: jogarLudo();      break;
            case 2: mostrarRegras();  break;
            case 3: mostrarRanking(); break;
            case 4:
                system("cls");
                printf("Saindo do jogo...\n");
                printf("------------------------------------------------------\n");
                printf("|             ~OBRIGADO! VOLTE SEMPRE!~              |\n");
                printf("------------------------------------------------------\n");
                printf("\nAperte qualquer tecla!");
                _getch();
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
                printf("Pressione uma tecla...\n");
                _getch();
                break;
        }
    } while (escolha != 4);
}

// FUNCAO PRINCIPAL
int main() {
    mostrarBoasVindas();
    mostrarMenu();
    return 0;
}
#endif // !UNIT_TEST

#ifdef UNIT_TEST
// ======================= TESTES AUTOMATIZADOS =======================
static int falhas = 0;
#define CHECK(cond, msg) do { if (cond) { printf("  [OK]   %s\n", msg); } \
    else { printf("  [FAIL] %s\n", msg); falhas++; } } while(0)

int main() {
    Jogador js[4];

    // Teste 1: distancia ate o centro eh igual para todas as cores (corrida justa)
    printf("Teste 1 - corrida justa (mesma distancia para todas as cores):\n");
    for (int p = 0; p < 4; p++) {
        inicializarJogadores(js);
        CHECK(moverPeca(&js[p], 0, 6) == 1, "sai da base com 6");
        int passos = 0;
        while (js[p].pecas[0].state != DONE && passos < 100) {
            moverPeca(&js[p], 0, 1);
            passos++;
        }
        char m[64]; sprintf(m, "cor %c chega ao centro em %d passos (esperado %d)", js[p].cor, passos, FINISH);
        CHECK(passos == FINISH, m);
        CHECK(js[p].pecas[0].x == 5 && js[p].pecas[0].y == 5, "peca termina no centro (5,5)");
    }

    // Teste 2: precisa cair exato no centro
    printf("Teste 2 - chegada exata:\n");
    inicializarJogadores(js);
    js[0].pecas[0].state = TRACK;
    js[0].pecas[0].progress = FINISH - 2;
    CHECK(moverPeca(&js[0], 0, 3) == 0, "recusa passar do centro (excesso)");
    CHECK(js[0].pecas[0].progress == FINISH - 2, "peca nao se move em jogada invalida");
    CHECK(moverPeca(&js[0], 0, 2) == 1, "aceita cair exato no centro");
    CHECK(js[0].pecas[0].state == DONE, "peca marcada como DONE");

    // Teste 3: indices invalidos nao quebram nada
    printf("Teste 3 - validacao de indice:\n");
    inicializarJogadores(js);
    CHECK(pecaPodeMover(&js[0], -1, 6) == 0, "indice -1 rejeitado");
    CHECK(pecaPodeMover(&js[0],  4, 6) == 0, "indice 4 rejeitado");
    CHECK(moverPeca(&js[0], -1, 6) == 0, "moverPeca com -1 nao faz nada");
    CHECK(moverPeca(&js[0],  4, 6) == 0, "moverPeca com 4 nao faz nada");
    CHECK(pecaPodeMover(&js[0], 0, 5) == 0, "peca na base nao sai sem 6");
    CHECK(pecaPodeMover(&js[0], 0, 6) == 1, "peca na base sai com 6");

    // Teste 4: captura
    printf("Teste 4 - captura de adversario:\n");
    inicializarJogadores(js);
    // Coloca peca do jogador 0 e do jogador 1 na mesma casa do caminho principal
    js[1].pecas[0].state = TRACK; js[1].pecas[0].progress = 5;
    posicaoNoCaminho(&js[1], 5, &js[1].pecas[0].x, &js[1].pecas[0].y);
    js[0].pecas[0].state = TRACK; js[0].pecas[0].progress = 3;
    js[0].pecas[0].x = js[1].pecas[0].x;   // forca mesma coordenada
    js[0].pecas[0].y = js[1].pecas[0].y;
    verificarCaptura(js, 0, 0);
    CHECK(js[1].pecas[0].state == BASE, "adversario volta pra base ao ser capturado");
    CHECK(js[0].pecas[0].state == TRACK, "peca que capturou permanece no tabuleiro");

    // Teste 5: deteccao de vitoria
    printf("Teste 5 - vitoria:\n");
    inicializarJogadores(js);
    CHECK(jogadorVenceu(&js[0]) == 0, "no inicio ninguem venceu");
    for (int i = 0; i < 4; i++) {
        moverPeca(&js[0], i, 6);              // sai da base
        for (int s = 0; s < FINISH; s++)      // anda ate o centro
            moverPeca(&js[0], i, 1);
        CHECK(js[0].pecas[i].state == DONE, "peca chegou ao centro");
    }
    CHECK(jogadorVenceu(&js[0]) == 1, "jogador com 4 pecas no centro venceu");

    printf("\n==== %s (%d falha(s)) ====\n", falhas == 0 ? "TODOS OS TESTES PASSARAM" : "HOUVE FALHAS", falhas);
    return falhas == 0 ? 0 : 1;
}
#endif // UNIT_TEST
