#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>    // Para a semente de números aleatórios
#include <stdbool.h> // Para usar o tipo bool

// ============================================================================
// --- Constantes Globais ---
// ============================================================================
#define CAPACIDADE_MAXIMA 5 // Tamanho fixo da fila de peças futuras

// ============================================================================
// --- Estrutura de Dados ---
// ============================================================================

// Atributos de cada peça
typedef struct {
    char nome;  // Tipo da peça: 'I', 'O', 'T', 'L', 'J', 'S', 'Z'
    int id;     // Identificador único da peça
} Peca;

// Estrutura para gerenciar a Fila Circular
typedef struct {
    Peca elementos[CAPACIDADE_MAXIMA];
    int frente;      // Índice da primeira peça (dequeue)
    int fundo;       // Índice do último elemento inserido (enqueue)
    int count;       // Contador de elementos atuais na fila
    int proximo_id;  // Contador global para gerar IDs únicos
} FilaPecas;


// ============================================================================
// --- Protótipos das Funções ---
// ============================================================================

// Funções de Gerenciamento da Fila
void inicializar_fila(FilaPecas *fila);
bool fila_vazia(const FilaPecas *fila);
bool fila_cheia(const FilaPecas *fila);
void jogar_peca(FilaPecas *fila);         // Dequeue
void inserir_nova_peca(FilaPecas *fila);  // Enqueue

// Funções Utilitárias e Interface
Peca gerar_peca(int id_peca);
void exibir_menu();
void exibir_estado_fila(const FilaPecas *fila);
void limpar_buffer();


// ============================================================================
// --- Função Principal (main) ---
// ============================================================================
int main() {
    FilaPecas fila_pecas;
    int opcao;

    srand((unsigned int)time(NULL)); // Inicializa a semente para rand()
    inicializar_fila(&fila_pecas);

    printf("--- Tetris Stack: Fila de Peças Futuras (Nível Novato) ---\n");

    // Preenche a fila inicial com o número máximo de peças (5)
    printf("Inicializando a fila com %d peças...\n", CAPACIDADE_MAXIMA);
    for (int i = 0; i < CAPACIDADE_MAXIMA; i++) {
        inserir_nova_peca(&fila_pecas);
    }
    printf("Fila inicializada. Agora você pode jogar e inserir peças.\n");


    do {
        exibir_estado_fila(&fila_pecas);
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
            case 2: // Inserir nova peça (enqueue)
                inserir_nova_peca(&fila_pecas);
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
// --- Implementação das Funções de Gerenciamento da Fila ---
// ============================================================================

void inicializar_fila(FilaPecas *fila) {
    // Inicializa os ponteiros e o contador
    fila->frente = 0;
    fila->fundo = -1; // Indica que o fundo ainda não foi preenchido
    fila->count = 0;
    fila->proximo_id = 1; // ID inicial
}

bool fila_vazia(const FilaPecas *fila) {
    return fila->count == 0;
}

bool fila_cheia(const FilaPecas *fila) {
    return fila->count == CAPACIDADE_MAXIMA;
}

void jogar_peca(FilaPecas *fila) {
    // Dequeue: remove a peça da frente da fila
    if (fila_vazia(fila)) {
        printf("\nERRO: A fila de peças futuras está vazia. Nada para jogar (dequeue)!\n");
        return;
    }

    Peca peca_jogada = fila->elementos[fila->frente];

    // Avança o ponteiro 'frente' de forma circular
    fila->frente = (fila->frente + 1) % CAPACIDADE_MAXIMA;

    // Decrementa o contador de elementos
    fila->count--;

    printf("\nSUCESSO (Dequeue): Peça jogada! Tipo: '%c', ID: %d.\n", peca_jogada.nome, peca_jogada.id);
}

void inserir_nova_peca(FilaPecas *fila) {
    // Enqueue: adiciona uma nova peça ao final da fila
    if (fila_cheia(fila)) {
        printf("\nAVISO: A fila está cheia! Jogue uma peça antes de adicionar uma nova (enqueue).\n");
        return;
    }

    // Avança o ponteiro 'fundo' de forma circular
    fila->fundo = (fila->fundo + 1) % CAPACIDADE_MAXIMA;

    // Gera a nova peça com o ID atual e incrementa o contador global de IDs
    Peca nova_peca = gerar_peca(fila->proximo_id++);

    // Insere a peça no novo fundo
    fila->elementos[fila->fundo] = nova_peca;

    // Incrementa o contador de elementos
    fila->count++;

    printf("\nSUCESSO (Enqueue): Peça '%c' (ID %d) adicionada ao final da fila.\n", nova_peca.nome, nova_peca.id);
}


// ============================================================================
// --- Implementação das Funções Utilitárias e Interface ---
// ============================================================================

Peca gerar_peca(int id_peca) {
    // Gera uma peça com tipo aleatório e ID único
    Peca p;
    p.id = id_peca;

    // Peças Tetris: I, O, T, L, J, S, Z (7 tipos)
    char tipos[] = {'I', 'O', 'T', 'L', 'J', 'S', 'Z'};
    int indice_aleatorio = rand() % 7;

    p.nome = tipos[indice_aleatorio];

    return p;
}

void exibir_menu() {
    printf("\n--- Opções de Ação ---\n");
    printf("Código | Ação\n");
    printf("---|---------------------------------\n");
    printf(" 1 | Jogar peça (dequeue)\n");
    printf(" 2 | Inserir nova peça (enqueue)\n");
    printf(" 0 | Sair\n");
    printf("-----------------------------------\n");
}

void exibir_estado_fila(const FilaPecas *fila) {
    printf("\n==================== ESTADO ATUAL DA FILA ====================\n");
    printf("Fila de peças (%d/%d): ", fila->count, CAPACIDADE_MAXIMA);

    if (fila_vazia(fila)) {
        printf("[VAZIA]\n");
        printf("--------------------------------------------------------------\n");
        return;
    }

    // Variáveis para iteração na fila circular
    int i = fila->frente;
    int j = 0; // Contador de peças exibidas

    // Percorre a fila a partir da frente, até que todas as peças (count) sejam exibidas
    while (j < fila->count) {
        // Formato: [Tipo ID]
        printf("[%c %d]", fila->elementos[i].nome, fila->elementos[i].id);

        if (j == 0) {
            printf(" <- FRENTE ");
        }
        if (j == fila->count - 1) {
            printf(" <- FUNDO");
        }
        
        printf(" ");

        i = (i + 1) % CAPACIDADE_MAXIMA; // Avança o índice circularmente
        j++;
    }

    printf("\n--------------------------------------------------------------\n");
}

void limpar_buffer() {
    // Função utilitária para limpar o buffer de entrada após o scanf
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}