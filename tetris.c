#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// ============================================================================
// --- Constantes Globais ---
// ============================================================================
#define CAPACIDADE_FILA 5   // Tamanho fixo da fila de peças futuras
#define CAPACIDADE_PILHA 3  // Capacidade máxima da pilha de reserva

// ============================================================================
// --- Estrutura de Dados ---
// ============================================================================

// Atributos de cada peça
typedef struct {
    char nome;
    int id;
} Peca;

// Estrutura para gerenciar a Fila Circular
typedef struct {
    Peca elementos[CAPACIDADE_FILA];
    int frente;
    int fundo;
    int count;
} FilaPecas;

// Estrutura para gerenciar a Pilha Linear
typedef struct {
    Peca elementos[CAPACIDADE_PILHA];
    int topo; // Índice da última peça inserida
    int count;
} PilhaReserva;

// Variável global para IDs únicos
static int proximo_id = 1;

// ============================================================================
// --- Protótipos das Funções ---
// ============================================================================

// Funções de Gerenciamento de Ambas Estruturas
void inicializar_fila(FilaPecas *fila);
void inicializar_pilha(PilhaReserva *pilha);

// Funções da Fila (Enqueue/Dequeue)
bool fila_vazia(const FilaPecas *fila);
bool fila_cheia(const FilaPecas *fila);
Peca gerar_peca();
void inserir_na_fila(FilaPecas *fila); // Enqueue

// Funções da Pilha (Push/Pop)
bool pilha_vazia(const PilhaReserva *pilha);
bool pilha_cheia(const PilhaReserva *pilha);
void push_peca(PilhaReserva *pilha, Peca peca);
Peca pop_peca(PilhaReserva *pilha);

// Funções de Ação do Jogo
void jogar_peca(FilaPecas *fila);
void reservar_peca(FilaPecas *fila, PilhaReserva *pilha);  // NOVIDADE
void usar_peca_reservada(PilhaReserva *pilha);             // NOVIDADE

// Funções Utilitárias e Interface
void exibir_menu();
void exibir_estado_fila(const FilaPecas *fila);
void exibir_estado_pilha(const PilhaReserva *pilha); // NOVIDADE
void limpar_buffer();


// ============================================================================
// --- Função Principal (main) ---
// ============================================================================
int main() {
    FilaPecas fila_pecas;
    PilhaReserva pilha_reserva;
    int opcao;

    srand((unsigned int)time(NULL));
    inicializar_fila(&fila_pecas);
    inicializar_pilha(&pilha_reserva);

    printf("--- Tetris Stack: Reserva de Peças com Pilha (Nível Aventureiro) ---\n");

    // Inicializa a fila CHEIA (5 peças)
    printf("Inicializando a fila com %d peças...\n", CAPACIDADE_FILA);
    while (!fila_cheia(&fila_pecas)) {
        inserir_na_fila(&fila_pecas);
    }
    printf("Fila inicializada cheia.\n");

    do {
        exibir_estado_fila(&fila_pecas);
        exibir_estado_pilha(&pilha_reserva);
        exibir_menu();

        printf("Escolha uma ação: ");
        if (scanf("%d", &opcao) != 1) {
            opcao = -1;
        }
        limpar_buffer();

        switch (opcao) {
            case 1: // Jogar peça (dequeue)
                jogar_peca(&fila_pecas);
                break;
            case 2: // Reservar peça (Fila -> Pilha, depois Fila <- Nova)
                reservar_peca(&fila_pecas, &pilha_reserva);
                break;
            case 3: // Usar peça reservada (Pop)
                usar_peca_reservada(&pilha_reserva);
                break;
            case 0: // Sair
                printf("\nEncerrando o sistema de controle de peças. Fim de jogo!\n");
                break;
            default:
                printf("\nOpção inválida. Tente novamente.\n");
                break;
        }

        if (opcao != 0) {
            printf("\nPressione ENTER para continuar...");
            getchar();
        }

    } while (opcao != 0);

    return 0;
}

// ============================================================================
// --- Implementação das Funções de Estruturas ---
// ============================================================================

void inicializar_fila(FilaPecas *fila) {
    fila->frente = 0;
    fila->fundo = -1;
    fila->count = 0;
}

void inicializar_pilha(PilhaReserva *pilha) {
    pilha->topo = -1; // Indica pilha vazia
    pilha->count = 0;
}

// --- Fila Circular (Simplificado) ---

bool fila_vazia(const FilaPecas *fila) {
    return fila->count == 0;
}

bool fila_cheia(const FilaPecas *fila) {
    return fila->count == CAPACIDADE_FILA;
}

Peca gerar_peca() {
    Peca p;
    p.id = proximo_id++;
    char tipos[] = {'I', 'O', 'T', 'L', 'J', 'S', 'Z'};
    p.nome = tipos[rand() % 7];
    return p;
}

void inserir_na_fila(FilaPecas *fila) {
    // Apenas insere uma peça, assume que a verificação de CHEIA é feita pela lógica do jogo.
    if (fila_cheia(fila)) return;

    fila->fundo = (fila->fundo + 1) % CAPACIDADE_FILA;
    fila->elementos[fila->fundo] = gerar_peca();
    fila->count++;
}

// --- Pilha Linear ---

bool pilha_vazia(const PilhaReserva *pilha) {
    return pilha->count == 0;
}

bool pilha_cheia(const PilhaReserva *pilha) {
    return pilha->count == CAPACIDADE_PILHA;
}

void push_peca(PilhaReserva *pilha, Peca peca) {
    // Insere no topo
    if (pilha_cheia(pilha)) return;

    pilha->topo++; // Move o topo para o próximo slot
    pilha->elementos[pilha->topo] = peca;
    pilha->count++;
}

Peca pop_peca(PilhaReserva *pilha) {
    // Remove do topo
    Peca peca_pop;
    if (pilha_vazia(pilha)) {
        // Retorna uma peça nula em caso de erro
        peca_pop.nome = '?';
        peca_pop.id = -1;
        return peca_pop;
    }

    peca_pop = pilha->elementos[pilha->topo];
    pilha->topo--; // Move o topo para baixo
    pilha->count--;
    return peca_pop;
}

// ============================================================================
// --- Implementação das Funções de Ação do Jogo ---
// ============================================================================

void jogar_peca(FilaPecas *fila) {
    // Joga a peça da frente da fila e INSERE UMA NOVA no final para manter a fila cheia
    if (fila_vazia(fila)) {
        printf("\nERRO: A fila está vazia (impossível no Nível Aventureiro se a lógica for seguida).\n");
        return;
    }

    Peca peca_jogada = fila->elementos[fila->frente];

    // 1. Dequeue
    fila->frente = (fila->frente + 1) % CAPACIDADE_FILA;
    fila->count--; // Decrementa temporariamente

    printf("\nSUCESSO (JOGAR): Peça '%c' (ID %d) jogada!\n", peca_jogada.nome, peca_jogada.id);

    // 2. Enqueue automático para manter a fila cheia
    inserir_na_fila(fila);
}

void reservar_peca(FilaPecas *fila, PilhaReserva *pilha) {
    // 1. Verifica se a pilha pode receber
    if (pilha_cheia(pilha)) {
        printf("\nERRO: A Pilha de Reserva está cheia (%d/%d). Use uma peça reservada antes de guardar outra.\n", CAPACIDADE_PILHA, CAPACIDADE_PILHA);
        return;
    }
    if (fila_vazia(fila)) {
        printf("\nERRO: A fila está vazia. Nada para reservar.\n");
        return;
    }

    // Peça a ser movida
    Peca peca_reservada = fila->elementos[fila->frente];

    // 2. Dequeue (remove da frente da fila)
    fila->frente = (fila->frente + 1) % CAPACIDADE_FILA;
    fila->count--;

    // 3. Push (insere no topo da pilha)
    push_peca(pilha, peca_reservada);

    printf("\nSUCESSO (RESERVAR): Peça '%c' (ID %d) movida da Fila para a Pilha.\n", peca_reservada.nome, peca_reservada.id);

    // 4. Enqueue automático para repor a peça na fila (mantendo a Fila cheia)
    inserir_na_fila(fila);
}

void usar_peca_reservada(PilhaReserva *pilha) {
    // Pop: Retira a peça do topo da pilha e a "usa" (joga)
    if (pilha_vazia(pilha)) {
        printf("\nERRO: A Pilha de Reserva está vazia. Nada para usar.\n");
        return;
    }

    // 1. Pop (retira do topo da pilha)
    Peca peca_usada = pop_peca(pilha);

    printf("\nSUCESSO (USAR RESERVA): Peça '%c' (ID %d) retirada da Pilha e usada.\n", peca_usada.nome, peca_usada.id);
}


// ============================================================================
// --- Implementação das Funções Utilitárias e Interface ---
// ============================================================================

void exibir_menu() {
    printf("\n--- Opções de Ação ---\n");
    printf("Código | Ação\n");
    printf("---|---------------------------------------\n");
    printf(" 1 | Jogar peça da Fila (Dequeue + Enqueue)\n");
    printf(" 2 | Reservar peça (Fila -> Pilha)\n");
    printf(" 3 | Usar peça reservada (Pop)\n");
    printf(" 0 | Sair\n");
    printf("---------------------------------------\n");
}

void exibir_estado_fila(const FilaPecas *fila) {
    printf("\n==================== FILA DE PEÇAS FUTURAS ====================\n");
    printf("Fila de peças (%d/%d): ", fila->count, CAPACIDADE_FILA);

    if (fila_vazia(fila)) {
        printf("[VAZIA]\n");
        printf("----------------------------------------------------------------\n");
        return;
    }

    int i = fila->frente;
    int j = 0;

    while (j < fila->count) {
        printf("[%c %d]", fila->elementos[i].nome, fila->elementos[i].id);

        if (j == 0) printf(" <- FRENTE ");
        if (j == fila->count - 1) printf(" <- FUNDO");
        
        printf(" ");
        i = (i + 1) % CAPACIDADE_FILA;
        j++;
    }
    printf("\n----------------------------------------------------------------\n");
}

void exibir_estado_pilha(const PilhaReserva *pilha) {
    printf("==================== PILHA DE RESERVA ===========================\n");
    printf("Pilha de reserva (%d/%d): ", pilha->count, CAPACIDADE_PILHA);

    if (pilha_vazia(pilha)) {
        printf("[VAZIA]\n");
        printf("----------------------------------------------------------------\n");
        return;
    }

    // Pilha é exibida do topo para a base
    printf("TOPO -> ");
    for (int i = pilha->topo; i >= 0; i--) {
        printf("[%c %d]", pilha->elementos[i].nome, pilha->elementos[i].id);
        if (i > 0) printf(" | ");
    }
    printf(" <- BASE\n");
    printf("----------------------------------------------------------------\n");
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}