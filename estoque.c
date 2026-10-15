#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct {
    int id;
    char nome[100];
    char codigoBarras[100];
    float preco;
    char marca[100];
    char categoria[100];
    char lote[100];
    char validade[100];
    int quantidade;
    int quantidadeMinima;
} Produto;

Produto *produtos = NULL;
int (*estoqueMensal)[12] = NULL;
int totalProdutos = 0;
int capacidade = 0;

int garantirCapacidade(void) {
    if (totalProdutos < capacidade) return 1;

    int novaCapacidade = capacidade == 0 ? 16 : capacidade * 2;

    Produto *novosProdutos = realloc(produtos, novaCapacidade * sizeof(Produto));
    if (novosProdutos == NULL) return 0;
    produtos = novosProdutos;

    int (*novoEstoque)[12] = realloc(estoqueMensal, novaCapacidade * sizeof(*estoqueMensal));
    if (novoEstoque == NULL) return 0;
    estoqueMensal = novoEstoque;

    memset(estoqueMensal + capacidade, 0, (novaCapacidade - capacidade) * sizeof(*estoqueMensal));

    capacidade = novaCapacidade;
    return 1;
}

int mesAtual(void) {
    time_t agora = time(NULL);
    struct tm *data = localtime(&agora);
    return data->tm_mon;
}

void lerTexto(const char *mensagem, char *destino, int tamanho) {
    do {
        printf("%s", mensagem);
        if (fgets(destino, tamanho, stdin) == NULL) {
            destino[0] = '\0';
            continue;
        }
        destino[strcspn(destino, "\n")] = '\0';

        if (strlen(destino) == 0)
            printf("Este campo nao pode ficar vazio. Tente novamente.\n");
    } while (strlen(destino) == 0);
}

float lerPreco(const char *mensagem) {
    char buffer[100];
    float preco;
    char *endptr;

    while (1) {
        printf("%s", mensagem);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        for (int i = 0; buffer[i] != '\0'; i++) {
            if (buffer[i] == ',') {
                buffer[i] = '.';
            }
        }

        char *ptr = buffer;
        while (isspace((unsigned char)*ptr)) ptr++;

        if (*ptr == '\0') {
            printf("O preco nao pode ficar vazio. Tente novamente.\n");
            continue;
        }

        preco = strtof(ptr, &endptr);

        while (isspace((unsigned char)*endptr)) endptr++;

        if (endptr == ptr || *endptr != '\0') {
            printf("Entrada invalida! Digite apenas um numero valido (ex: 15.50).\n");
            continue;
        }

        if (preco <= 0) {
            printf("O preco deve ser maior que zero. Tente novamente.\n");
            continue;
        }

        return preco;
    }
}

int lerInteiro(const char *mensagem, int minValor) {
    char buffer[100];
    int valor;
    char *endptr;

    while (1) {
        printf("%s", mensagem);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        char *ptr = buffer;
        while (isspace((unsigned char)*ptr)) ptr++;

        if (*ptr == '\0') {
            printf("Este campo nao pode ficar vazio. Tente novamente.\n");
            continue;
        }

        valor = (int)strtol(ptr, &endptr, 10);

        while (isspace((unsigned char)*endptr)) endptr++;

        if (endptr == ptr || *endptr != '\0') {
            printf("Entrada invalida! Digite apenas um numero inteiro valido.\n");
            continue;
        }

        if (valor < minValor) {
            printf("O valor deve ser igual ou maior que %d. Tente novamente.\n", minValor);
            continue;
        }

        return valor;
    }
}

int dataValida(const char *data) {
    if (strlen(data) != 10) return 0;
    if (data[2] != '/' || data[5] != '/') return 0;

    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit((unsigned char)data[i])) return 0;
    }

    int dia = (data[0] - '0') * 10 + (data[1] - '0');
    int mes = (data[3] - '0') * 10 + (data[4] - '0');
    int ano = (data[6] - '0') * 1000 + (data[7] - '0') * 100 + (data[8] - '0') * 10 + (data[9] - '0');

    if (ano < 1) return 0;
    if (mes < 1 || mes > 12) return 0;

    int diasNoMes[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0)
        diasNoMes[1] = 29;

    if (dia < 1 || dia > diasNoMes[mes - 1]) return 0;

    return 1;
}

int dataParaNumero(const char *data) {
    int dia = (data[0] - '0') * 10 + (data[1] - '0');
    int mes = (data[3] - '0') * 10 + (data[4] - '0');
    int ano = (data[6] - '0') * 1000 + (data[7] - '0') * 100 + (data[8] - '0') * 10 + (data[9] - '0');
    return ano * 10000 + mes * 100 + dia;
}

int textosIguais(const char *a, const char *b) {
    while (*a != '\0' && *b != '\0') {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

int existeNomeELote(const char *nome, const char *lote) {
    for (int i = 0; i < totalProdutos; i++) {
        if (textosIguais(produtos[i].nome, nome) && textosIguais(produtos[i].lote, lote))
            return 1;
    }
    return 0;
}

int indiceConflitoIdentidade(const char *nome, const char *codigo) {
    for (int i = 0; i < totalProdutos; i++) {
        int mesmoNome = textosIguais(produtos[i].nome, nome);
        int mesmoCodigo = strcmp(produtos[i].codigoBarras, codigo) == 0;
        if (mesmoNome != mesmoCodigo)
            return i;
    }
    return -1;
}

int existeProduto(const char *nome) {
    for (int i = 0; i < totalProdutos; i++) {
        if (textosIguais(produtos[i].nome, nome))
            return 1;
    }
    return 0;
}

void cadastrarProduto(void) {
    Produto p;
    int conflito;

    if (!garantirCapacidade()) {
        printf("Memoria insuficiente para cadastrar novos produtos.\n");
        return;
    }

    do {
        lerTexto("Nome do produto: ", p.nome, 100);
        lerTexto("Codigo de barras: ", p.codigoBarras, 100);

        conflito = indiceConflitoIdentidade(p.nome, p.codigoBarras);
        if (conflito != -1) {
            printf("Conflito: o produto \"%s\" ja esta cadastrado com o codigo de barras %s.\n",
                   produtos[conflito].nome, produtos[conflito].codigoBarras);
            printf("O mesmo nome e codigo de barras devem identificar o mesmo produto. Informe novamente.\n");
        }
    } while (conflito != -1);

    int produtoExistente = existeProduto(p.nome);
    if (produtoExistente)
        printf("Produto ja cadastrado. Sera registrado um novo lote.\n");

    p.preco = lerPreco("Preco de venda: ");
    lerTexto("Marca: ", p.marca, 100);
    lerTexto("Categoria: ", p.categoria, 100);

    do {
        lerTexto("Lote: ", p.lote, 100);
        if (strchr(p.lote, ';') != NULL) {
            printf("O lote nao pode conter ponto e virgula.\n");
            p.lote[0] = '\0';
            continue;
        }
        if (existeNomeELote(p.nome, p.lote))
            printf("Produto ja cadastrado neste lote. Informe um lote diferente.\n");
    } while (strlen(p.lote) == 0 || existeNomeELote(p.nome, p.lote));

    do {
        lerTexto("Data de validade (dd/mm/aaaa): ", p.validade, 100);
        if (!dataValida(p.validade))
            printf("Data invalida! Use o formato dd/mm/aaaa (ex: 25/12/2026).\n");
    } while (!dataValida(p.validade));

    p.quantidade = lerInteiro("Quantidade em estoque: ", 0);
    p.quantidadeMinima = lerInteiro("Quantidade minima: ", 0);

    p.id = totalProdutos + 1;
    produtos[totalProdutos] = p;
    estoqueMensal[totalProdutos][mesAtual()] = p.quantidade;
    totalProdutos = totalProdutos + 1;

    printf("Produto cadastrado com sucesso!\n");
}

void procurarProdutoPorNome(void) {
    char nomeBusca[100];
    int encontrado = 0;

    lerTexto("Nome do produto: ", nomeBusca, 100);

    for (int i = 0; i < totalProdutos; i++) {
        if (textosIguais(produtos[i].nome, nomeBusca)) {
            printf("\nID: %d\n", produtos[i].id);
            printf("Nome: %s\n", produtos[i].nome);
            printf("Codigo de barras: %s\n", produtos[i].codigoBarras);
            printf("Preco: R$ %.2f\n", produtos[i].preco);
            printf("Marca: %s\n", produtos[i].marca);
            printf("Categoria: %s\n", produtos[i].categoria);
            printf("Lote: %s\n", produtos[i].lote);
            printf("Validade: %s\n", produtos[i].validade);
            printf("Quantidade: %d\n", produtos[i].quantidade);
            printf("Quantidade minima: %d\n\n", produtos[i].quantidadeMinima);
            encontrado = 1;
        }
    }

    if (encontrado == 0)
        printf("Produto nao encontrado.\n");
}

void filtrarPorMarca(void) {
    char marcaBusca[100];
    int encontrado = 0;

    lerTexto("Marca: ", marcaBusca, 100);

    for (int i = 0; i < totalProdutos; i++) {
        if (textosIguais(produtos[i].marca, marcaBusca)) {
            printf("%d - %s | Lote: %s (Qtd: %d)\n", produtos[i].id, produtos[i].nome,
                   produtos[i].lote, produtos[i].quantidade);
            encontrado = 1;
        }
    }

    if (encontrado == 0)
        printf("Nenhum produto encontrado para essa marca.\n");
}

void listarProdutos(void) {
    if (totalProdutos == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }

    for (int i = 0; i < totalProdutos; i++) {
        printf("%d - %s | Marca: %s | Lote: %s | Validade: %s | Qtd: %d\n",
               produtos[i].id, produtos[i].nome, produtos[i].marca,
               produtos[i].lote, produtos[i].validade, produtos[i].quantidade);
    }
}

int indiceLotePrioritario(const char *codigo) {
    int indice = -1;
    for (int i = 0; i < totalProdutos; i++) {
        if (strcmp(produtos[i].codigoBarras, codigo) != 0) continue;
        if (produtos[i].quantidade <= 0) continue;
        if (indice == -1 ||
            dataParaNumero(produtos[i].validade) < dataParaNumero(produtos[indice].validade))
            indice = i;
    }
    return indice;
}

void registrarSaida(void) {
    char codigo[100];
    int quantidadeSaida;
    int encontrado = 0;
    int totalDisponivel = 0;

    lerTexto("Codigo de barras: ", codigo, 100);

    for (int i = 0; i < totalProdutos; i++) {
        if (strcmp(produtos[i].codigoBarras, codigo) == 0) {
            encontrado = 1;
            totalDisponivel = totalDisponivel + produtos[i].quantidade;
        }
    }

    if (!encontrado) {
        printf("Produto nao encontrado.\n");
        return;
    }

    printf("Estoque atual (todos os lotes): %d\n", totalDisponivel);
    quantidadeSaida = lerInteiro("Quantidade a retirar: ", 1);

    if (quantidadeSaida > totalDisponivel) {
        printf("Quantidade invalida (maior que o estoque em loja).\n");
        return;
    }

    int restante = quantidadeSaida;
    while (restante > 0) {
        int indice = indiceLotePrioritario(codigo);
        int retirada = restante < produtos[indice].quantidade ? restante : produtos[indice].quantidade;

        produtos[indice].quantidade = produtos[indice].quantidade - retirada;
        estoqueMensal[indice][mesAtual()] = produtos[indice].quantidade;
        restante = restante - retirada;

        printf("Retirado %d un. do lote %s (validade %s). Saldo do lote: %d\n",
               retirada, produtos[indice].lote, produtos[indice].validade, produtos[indice].quantidade);
    }

    printf("Saida registrada.\n");
}

void exibirEstoqueMensal(void) {
    char *meses[12] = {"Jan", "Fev", "Mar", "Abr", "Mai", "Jun",
                        "Jul", "Ago", "Set", "Out", "Nov", "Dez"};

    if (totalProdutos == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }

    printf("\n%-20s%-12s", "Produto", "Lote");
    for (int mes = 0; mes < 12; mes++)
        printf("%5s", meses[mes]);
    printf("\n");

    for (int i = 0; i < totalProdutos; i++) {
        printf("%-20.19s%-12.11s", produtos[i].nome, produtos[i].lote);
        for (int mes = 0; mes < 12; mes++)
            printf("%5d", estoqueMensal[i][mes]);
        printf("\n");
    }
}

void salvarDados(void) {
    FILE *arquivo = fopen("produtos.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao salvar os dados.\n");
        return;
    }

    for (int i = 0; i < totalProdutos; i++) {
        fprintf(arquivo, "%d;%s;%s;%.2f;%s;%s;%s;%s;%d;%d\n",
                produtos[i].id, produtos[i].nome, produtos[i].codigoBarras,
                produtos[i].preco, produtos[i].marca, produtos[i].categoria,
                produtos[i].lote, produtos[i].validade,
                produtos[i].quantidade, produtos[i].quantidadeMinima);
    }
    fclose(arquivo);
}

void carregarDados(void) {
    FILE *arquivo = fopen("produtos.txt", "r");
    if (arquivo == NULL) return;

    Produto p;

    while (fscanf(arquivo, "%d;%99[^;];%99[^;];%f;%99[^;];%99[^;];%99[^;];%99[^;];%d;%d\n",
                  &p.id, p.nome, p.codigoBarras, &p.preco, p.marca, p.categoria,
                  p.lote, p.validade, &p.quantidade, &p.quantidadeMinima) == 10) {
        if (!garantirCapacidade()) {
            printf("Memoria insuficiente para carregar todos os produtos.\n");
            break;
        }
        produtos[totalProdutos] = p;
        totalProdutos = totalProdutos + 1;
    }
    fclose(arquivo);
}

void exibirMenu(void) {
    printf("\n1 - Cadastrar produto\n");
    printf("2 - Procurar produto pelo nome\n");
    printf("3 - Filtrar produtos por marca\n");
    printf("4 - Listar todos os produtos\n");
    printf("5 - Registrar saida de estoque\n");
    printf("6 - Exibir estoque mensal\n");
    printf("0 - Sair\n");
}

int main(void) {
    int opcao;

    carregarDados();

    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ", 0);

        switch (opcao) {
            case 1: cadastrarProduto(); break;
            case 2: procurarProdutoPorNome(); break;
            case 3: filtrarPorMarca(); break;
            case 4: listarProdutos(); break;
            case 5: registrarSaida(); break;
            case 6: exibirEstoqueMensal(); break;
            case 0:
                salvarDados();
                printf("Dados salvos. Encerrando...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    free(produtos);
    free(estoqueMensal);

    return 0;
}