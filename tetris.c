#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// ============================================================================
// --- Constantes Globais ---
// ============================================================================
#define CAPACIDADE_FILA 5
#define CAPACIDADE_PILHA 3
#define CAPACIDADE_HISTORICO 1 // Apenas a última jogada pode ser desfeita

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

// Estrutura para gerenciar a Pilha Linear (Reserva e Histórico)
typedef struct {
    Peca elementos[CAPACIDADE_PILHA > CAPACIDADE_HISTORICO ? CAPACIDADE_PILHA : CAPACIDADE_HISTORICO];
    int topo;
    int max_cap;
} PilhaGenerica;

// Variável global para IDs únicos
static int proximo_id = 1;

// ============================================================================
// --- Protótipos das Funções ---
// ============================================================================

// Inicialização
void inicializar_fila(FilaPecas *fila);
void inicializar_pilha(PilhaGenerica *pilha, int capacidade);

// Pilha Genérica (Push/Pop)
bool pilha_vazia(const PilhaGenerica *pilha);
bool pilha_cheia(const PilhaGenerica *pilha);
void push_peca(PilhaGenerica *pilha, Peca peca);
Peca pop_peca(PilhaGenerica *pilha);

// Fila Circular (Enqueue/Dequeue)
bool fila_vazia(const FilaPecas *fila);
bool fila_cheia(const FilaPecas *fila);
Peca gerar_peca();
void inserir_na_fila(FilaPecas *fila, Peca peca_inserida);

// Funções de Ação do Jogo (Modificadas/Novas)
void jogar_peca(FilaPecas *fila, PilhaGenerica *historico);              // Modificada
void reservar_peca(FilaPecas *fila, PilhaGenerica *pilha);
void usar_peca_reservada(PilhaGenerica *pilha);
void trocar_topo_fila_pilha(FilaPecas *fila, PilhaGenerica *pilha);      // NOVA
void desfazer_jogada(FilaPecas *fila, PilhaGenerica *historico);         // NOVA
void inverter_fila_pilha(FilaPecas *fila, PilhaGenerica *pilha);         // NOVA

// Utilitárias e Interface
void exibir_menu();
void exibir_estado_fila(const FilaPecas *fila);
void exibir_estado_pilha(const PilhaGenerica *pilha, const char* nome);
void limpar_buffer();
Peca criar_peca_nula();

// ============================================================================
// --- Função Principal (main) ---
// ============================================================================
int main() {
    FilaPecas fila_pecas;
    PilhaGenerica pilha_reserva;
    PilhaGenerica pilha_historico;
    int opcao;

    srand((unsigned int)time(NULL));
    inicializar_fila(&fila_pecas);
    inicializar_pilha(&pilha_reserva, CAPACIDADE_PILHA);
    inicializar_pilha(&pilha_historico, CAPACIDADE_HISTORICO); // Pilha para "Desfazer"

    printf("--- Tetris Stack: Integração Total (Nível Mestre) ---\n");

    // Inicializa a fila CHEIA (5 peças)
    printf("Inicializando a fila com %d peças...\n", CAPACIDADE_FILA);
    while (!fila_cheia(&fila_pecas)) {
        inserir_na_fila(&fila_pecas, gerar_peca());
    }
    printf("Fila inicializada cheia. Pilha vazia.\n");

    do {
        exibir_estado_fila(&fila_pecas);
        exibir_estado_pilha(&pilha_reserva, "Pilha de Reserva");
        exibir_estado_pilha(&pilha_historico, "Histórico (Desfazer)");
        exibir_menu();

        printf("Escolha uma ação: ");
        if (scanf("%d", &opcao) != 1) {
            opcao = -1;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:
                jogar_peca(&fila_pecas, &pilha_historico);
                break;
            case 2:
                reservar_peca(&fila_pecas, &pilha_reserva);
                break;
            case 3:
                usar_peca_reservada(&pilha_reserva);
                break;
            case 4:
                trocar_topo_fila_pilha(&fila_pecas, &pilha_reserva);
                break;
            case 5:
                desfazer_jogada(&fila_pecas, &pilha_historico);
                break;
            case 6:
                inverter_fila_pilha(&fila_pecas, &pilha_reserva);
                break;
            case 0:
                printf("\nEncerrando o sistema. Fim de jogo!\n");
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
// --- Funções de Inicialização e Utilitárias ---
// ============================================================================

void inicializar_fila(FilaPecas *fila) {
    fila->frente = 0;
    fila->fundo = -1;
    fila->count = 0;
}

void inicializar_pilha(PilhaGenerica *pilha, int capacidade) {
    pilha->topo = -1;
    pilha->max_cap = capacidade;
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

Peca gerar_peca() {
    Peca p;
    p.id = proximo_id++;
    char tipos[] = {'I', 'O', 'T', 'L', 'J', 'S', 'Z'};
    p.nome = tipos[rand() % 7];
    return p;
}

Peca criar_peca_nula() {
    // Retorna uma peça de erro/nula
    Peca p = {'?', -1};
    return p;
}

void exibir_menu() {
    printf("\n--- Opções de Ação (Mestre) ---\n");
    printf("Código | Ação\n");
    printf("---|---------------------------------------\n");
    printf(" 1 | Jogar peça da Fila (Dequeue + Enqueue)\n");
    printf(" 2 | Reservar peça (Fila -> Pilha)\n");
    printf(" 3 | Usar peça reservada (Pop)\n");
    printf(" 4 | Trocar (Topo Pilha <-> Frente Fila)\n");
    printf(" 5 | Desfazer última jogada (Requer histórico)\n");
    printf(" 6 | Inverter Fila com Pilha (Troca Conteúdo Total)\n");
    printf(" 0 | Sair\n");
    printf("---------------------------------------\n");
}

// ============================================================================
// --- Fila Circular e Pilha Genérica (Push/Pop) ---
// ============================================================================

// --- Pilha Genérica (Push/Pop) ---
bool pilha_vazia(const PilhaGenerica *pilha) {
    return pilha->topo == -1;
}

bool pilha_cheia(const PilhaGenerica *pilha) {
    return pilha->topo == pilha->max_cap - 1;
}

void push_peca(PilhaGenerica *pilha, Peca peca) {
    if (pilha_cheia(pilha)) return;
    pilha->topo++;
    pilha->elementos[pilha->topo] = peca;
}

Peca pop_peca(PilhaGenerica *pilha) {
    if (pilha_vazia(pilha)) return criar_peca_nula();
    Peca peca_pop = pilha->elementos[pilha->topo];
    pilha->topo--;
    return peca_pop;
}

// --- Fila Circular (Enqueue/Dequeue) ---
bool fila_vazia(const FilaPecas *fila) {
    return fila->count == 0;
}

bool fila_cheia(const FilaPecas *fila) {
    return fila->count == CAPACIDADE_FILA;
}

void inserir_na_fila(FilaPecas *fila, Peca peca_inserida) {
    // Apenas insere uma peça, assume que a verificação de CHEIA é feita pela lógica do jogo.
    if (fila_cheia(fila)) return;
    fila->fundo = (fila->fundo + 1) % CAPACIDADE_FILA;
    fila->elementos[fila->fundo] = peca_inserida;
    fila->count++;
}

Peca remover_da_fila(FilaPecas *fila) {
    // Dequeue simples (usado internamente em trocas/reservas)
    if (fila_vazia(fila)) return criar_peca_nula();

    Peca peca_removida = fila->elementos[fila->frente];
    fila->frente = (fila->frente + 1) % CAPACIDADE_FILA;
    fila->count--;
    return peca_removida;
}

// ============================================================================
// --- Funções de Ação do Jogo (Nível Mestre) ---
// ============================================================================

void jogar_peca(FilaPecas *fila, PilhaGenerica *historico) {
    // Joga a peça, salva no histórico e repõe a fila
    if (fila_vazia(fila)) {
        printf("\nERRO: A fila está vazia.\n");
        return;
    }

    Peca peca_jogada = fila->elementos[fila->frente];

    // 1. Salva no histórico (Desfaz a jogada anterior, pois só armazena 1)
    if (!pilha_vazia(historico)) pop_peca(historico);
    push_peca(historico, peca_jogada);

    // 2. Dequeue
    fila->frente = (fila->frente + 1) % CAPACIDADE_FILA;
    fila->count--;

    printf("\nSUCESSO (JOGAR): Peça '%c' (ID %d) jogada! (Salva no histórico).\n", peca_jogada.nome, peca_jogada.id);

    // 3. Enqueue automático para manter a fila cheia
    inserir_na_fila(fila, gerar_peca());
}

void reservar_peca(FilaPecas *fila, PilhaGenerica *pilha) {
    // Move a peça da Fila para a Pilha e repõe a Fila
    if (pilha_cheia(pilha)) {
        printf("\nERRO: A Pilha de Reserva está cheia (%d/%d).\n", pilha->max_cap, pilha->max_cap);
        return;
    }
    if (fila_vazia(fila)) {
        printf("\nERRO: A fila está vazia. Nada para reservar.\n");
        return;
    }

    Peca peca_reservada = remover_da_fila(fila);
    push_peca(pilha, peca_reservada);

    printf("\nSUCESSO (RESERVAR): Peça '%c' (ID %d) movida da Fila para a Pilha.\n", peca_reservada.nome, peca_reservada.id);
    inserir_na_fila(fila, gerar_peca()); // Repõe a fila
}

void usar_peca_reservada(PilhaGenerica *pilha) {
    // Pop: Retira a peça do topo da pilha e a "usa" (joga)
    if (pilha_vazia(pilha)) {
        printf("\nERRO: A Pilha de Reserva está vazia. Nada para usar.\n");
        return;
    }
    Peca peca_usada = pop_peca(pilha);
    printf("\nSUCESSO (USAR RESERVA): Peça '%c' (ID %d) retirada da Pilha e usada.\n", peca_usada.nome, peca_usada.id);
}

void trocar_topo_fila_pilha(FilaPecas *fila, PilhaGenerica *pilha) {
    // Troca a peça da frente da fila com a peça do topo da pilha
    if (fila_vazia(fila) || pilha_vazia(pilha)) {
        printf("\nERRO: Fila e Pilha devem ter pelo menos uma peça para realizar a troca.\n");
        return;
    }

    // 1. Armazena as peças
    Peca peca_frente = fila->elementos[fila->frente];
    Peca peca_topo = pilha->elementos[pilha->topo];

    // 2. Troca (Atribuição direta nos índices)
    fila->elementos[fila->frente] = peca_topo;
    pilha->elementos[pilha->topo] = peca_frente;

    printf("\nSUCESSO (TROCA): Peça '%c' (Fila) trocada com Peça '%c' (Pilha).\n", peca_frente.nome, peca_topo.nome);
}

void desfazer_jogada(FilaPecas *fila, PilhaGenerica *historico) {
    // Reverte a última peça jogada, voltando-a para a frente da fila
    if (pilha_vazia(historico)) {
        printf("\nERRO: Não há jogada recente para desfazer.\n");
        return;
    }
    if (fila_cheia(fila)) {
        printf("\nERRO: A fila está cheia. Não há espaço para inserir a peça desfeita. Jogue uma peça primeiro.\n");
        return;
    }

    // 1. Retira a peça do histórico
    Peca peca_desfeita = pop_peca(historico);

    // 2. Coloca a peça na frente da fila
    // Isso é complexo na fila circular. A maneira mais fácil é reverter a 'frente' e o 'count'.
    fila->frente = (fila->frente - 1 + CAPACIDADE_FILA) % CAPACIDADE_FILA;
    fila->elementos[fila->frente] = peca_desfeita;
    fila->count++;

    // 3. Uma peça deve ser retirada do fundo (a peça que foi reposta automaticamente)
    fila->fundo = (fila->fundo - 1 + CAPACIDADE_FILA) % CAPACIDADE_FILA;
    fila->count--;

    printf("\nSUCESSO (DESFAZER): Jogada desfeita. Peça '%c' (ID %d) retornou à frente da Fila.\n", peca_desfeita.nome, peca_desfeita.id);
    printf("(Nota: A peça adicionada automaticamente ao fundo foi removida).\n");
}

void inverter_fila_pilha(FilaPecas *fila, PilhaGenerica *pilha) {
    // Troca todo o conteúdo da Fila com o conteúdo da Pilha

    // 1. Cria pilhas temporárias para manipular os conteúdos
    PilhaGenerica temp_fila, temp_pilha;
    inicializar_pilha(&temp_fila, CAPACIDADE_FILA); // Pilha para armazenar Fila
    inicializar_pilha(&temp_pilha, CAPACIDADE_PILHA); // Pilha para armazenar Pilha

    // 2. Move todo o conteúdo da FILA para TEMP_FILA (inverte a ordem da Fila)
    while (!fila_vazia(fila)) {
        push_peca(&temp_fila, remover_da_fila(fila));
    }

    // 3. Move todo o conteúdo da PILHA para TEMP_PILHA (inverte a ordem da Pilha)
    while (!pilha_vazia(pilha)) {
        push_peca(&temp_pilha, pop_peca(pilha));
    }

    // 4. Move o conteúdo de TEMP_PILHA para FILA (Inverte a ordem da Pilha e a coloca na Fila)
    while (!pilha_vazia(&temp_pilha)) {
        inserir_na_fila(fila, pop_peca(&temp_pilha));
    }

    // 5. Move o conteúdo de TEMP_FILA para PILHA (Inverte a ordem da Fila e a coloca na Pilha)
    while (!pilha_vazia(&temp_fila)) {
        // Verifica se a Pilha tem capacidade (CAPACIDADE_PILHA < CAPACIDADE_FILA)
        if (!pilha_cheia(pilha)) {
            push_peca(pilha, pop_peca(&temp_fila));
        } else {
            // Descarta o excesso da Fila que não cabe na Pilha
            pop_peca(&temp_fila);
        }
    }
    
    printf("\nSUCESSO (INVERTER): Conteúdo da Fila e da Pilha foram trocados.\n");
    printf("(A Fila foi invertida e o excesso de itens foi descartado da Pilha).\n");
}

// ============================================================================
// --- Funções de Exibição ---
// ============================================================================

void exibir_estado_fila(const FilaPecas *fila) {
    printf("\n==================== FILA DE PEÇAS FUTURAS ====================\n");
    printf("Fila (%d/%d): ", fila->count, CAPACIDADE_FILA);

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

void exibir_estado_pilha(const PilhaGenerica *pilha, const char* nome) {
    printf("==================== %s ===========================\n", nome);
    printf("Pilha (%d/%d): ", pilha->topo + 1, pilha->max_cap);

    if (pilha_vazia(pilha)) {
        printf("[VAZIA]\n");
        printf("----------------------------------------------------------------\n");
        return;
    }

    printf("TOPO -> ");
    for (int i = pilha->topo; i >= 0; i--) {
        printf("[%c %d]", pilha->elementos[i].nome, pilha->elementos[i].id);
        if (i > 0) printf(" | ");
    }
    printf(" <- BASE\n");
    printf("----------------------------------------------------------------\n");
}