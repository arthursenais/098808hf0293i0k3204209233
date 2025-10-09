#include <cstddef> //Utilizada para apelidar tipos
#include <fstream>
#include <iostream>
#include <limits> //Utilizada para obter limites de tipos numéricos
#include <sstream> //Utilizada para separar uma string (stringstream)
#include <string>
#include <vector> //Utilizda para utilizar vetores e para alocação de forma simples
#include <ctime> //Utilizada para a manipulação de data e hora

using namespace std; 

#define ARQUIVO "produtos.txt" //Constante para o nome do arquivo

template <typename I> //Template genérico: Lê qualquer tipo numérico
    I campoNumero(const string & prompt, I min = numeric_limits <I> ::lowest(), I max = numeric_limits <I> ::max(), const string & msg_erro = "Entrada inválida. Por favor, tente novamente. \n") { //Parâmetros (passados quando a função é chamada). Recebe um template que facilita a análise de todos os tipos numéricos
        I valor;
        string entrada;
        while (true) { //Pede a entrada até que seja válida
            cout << prompt;
            getline(cin, entrada); //Evita problemas com espaços, lendo a linha inteira
            stringstream ss(entrada);

            if (!(ss >> valor) || !(ss.eof()) || valor < min || valor > max) { //Verifica se a entrada é um número, convertendo para o tipo especificado; verifica se há algo que não é um número e verifica se o valor está no intervalo permitido, tudo isso respectivamente
                cout << msg_erro;
                continue; //Se a validação falhar, envia a mensagem de erro definida e o loop continua
            }
            return valor; //Se a validação for bem sucedida, retorna o respectivo valor e sai do loop
        }
    }

string campoTexto(const string & prompt, const string & msg_erro = "Entrada inválida. Por favor, tente novamente. \n") { //Parâmetros (passados quando a função é chamada)
    string entrada;

    while (true) { //Pede a entrada até que seja válida
        cout << prompt;
        getline(cin, entrada);

        if (entrada.empty()) {
            cout << msg_erro;
            continue; //Se a validação falhar, envia a mensagem de erro definida e o loop continua
        }
        return entrada; //Se a validação for bem sucedida, retorna o respectivo valor e sai do loop
    }
}

typedef int t_quantidade; //Adicionado para a semântica do código - Faz "t_quantidade" ser um apelido de "int"
typedef double t_valor; 
struct Produto {
    string nome;
    t_quantidade quantidade;
    t_valor valor;
};

vector <Produto> lerProdutos(const string &nomeArquivo = ARQUIVO) { //Os motivos do "const (tipo) &(variável)" em algumas funções é o seguinte: "const" juntamente do "&" faz com que a função receba o endereço de memória da variável, economizando processamento, evitando cópias, além de impedir que a variável seja modificada
    vector <Produto> produtos;
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cout << "Não foi possível abrir o arquivo " << nomeArquivo << endl;
        return produtos; //Retorna o vetor vazio;
    }

    string linha;
    while (getline(arquivo, linha)) {
        if (linha.empty()) continue; //Avança para a próxima linha caso a linha lida estiver vazia

        stringstream ss(linha); //Separa as partes da linha (string)
        Produto p; //Receberá valores e seus valores serão atribuídos ao vetor "produtos" a cada iteração, alimentando o vetor;
        string quantidadeStr, valorStr;

        if (!getline(ss, p.nome, ',')) continue; //Avança para a próxima linha caso não receba o valor esperado
        if (!getline(ss, quantidadeStr, ',')) continue; 
        if (!getline(ss, valorStr, ',')) continue; 
        try { //Tenta executar as operações abaixo, o que é relativamente perigoso
            p.quantidade = stoi(quantidadeStr); //Converte "quantidadeStr" para "int" e armazena em "p.quantidade"
            p.valor = stod(valorStr); 
        } catch (...) { //Caso alguma das execuções acima falhe, exibe a mensagem abaixo ao invés de carshar e pula para a próxima linha
            cout << "Erro ao converter quantidade/valor do produto: \"" << p.nome << "\"" << endl;
            continue;
        }
        produtos.push_back(p); //Aloca "p" em "produtos"
    }
    arquivo.close();
    return produtos; //Retorna o vetor de produtos
}

bool atualizarArquivo(const vector <Produto> & p,
    const string & nomeArquivo = ARQUIVO) {
    ofstream arquivo(nomeArquivo, ios::trunc); //"trunc" apaga completamente o arquivo, caso já exista, gerando uma cópia exata do vetor "p", de maneira que,"" sempre que a função seja chamada, o arquivo seja atualizado

    if (!arquivo.is_open()) {
        cout << "Não foi possível abrir o arquivo " << nomeArquivo << endl;
        return false;
    }

    for (const auto &prod : p) { //Recebe "p"
        arquivo << prod.nome << ", " <<
            prod.quantidade << ", " <<
            prod.valor << "\n";
    }
    arquivo.close();
    return true; //Indica que o loop terminou e tudo foi salvo corretamente no arquivo
}

void cadastrarProduto(vector <Produto> & p) {
    int quantidade_cadastrar;

    quantidade_cadastrar = campoNumero <int> ("\nQuantos itens você deseja cadastrar?\n[0] - Cancelar\n---> ", 0, 50);

    if (quantidade_cadastrar == 0)
        return; //Cancela a operação

    for (int i = 0; i < quantidade_cadastrar; i++) {
        Produto pdt;
        pdt.nome = campoTexto("\nInsira o nome do produto:\n[0] - Cancelar\n---> ");
        if (pdt.nome == "0")
            return;

        pdt.quantidade = campoNumero <int> ("\nInsira a quantidade do produto '" + pdt.nome + "'\n[0] - Cancelar\n---> ", 0, 5000);
        if (pdt.quantidade == 0)
            return;

        pdt.valor = campoNumero <double> ("\nInsira o valor do produto '" + pdt.nome + "'\n[0] - Cancelar\n---> ", 0, 5000);
        if (pdt.valor == 0)
            return;

        p.push_back(pdt);

    }

    for (size_t i = 0; i < p.size(); i++) { //"size()" retorna sempre um número sem sinal, ou seja, não negativo: size_t, por isso ele é usado, para a compatibilidade
        for (size_t j = i + 1; j < p.size(); j++) {
            if (p[i].nome == p[j].nome) {
                p[i].quantidade += p[j].quantidade; //Adiciona quantidade caso o produto seja o mesmo
                p[i].valor = p[j].valor; //Atualiza o valor caso o profuto seja o mesmo
                p.erase(p.begin() + j); //Apaga a cópia, permanecendo só o produto atualizado
            }
        }
    }

    if (!atualizarArquivo(p)) { //Tenta atualizar o arquivo
        cout << "\n Obs.: As alterações não serão salvas quando você sair.\n";
    }
}

void venderProdutos(vector <Produto> &p, vector <Produto> &pComprados, t_valor &preco) {
    Produto pdtComprado;
    int continuar = 1;
    size_t escolha = 0;
    t_quantidade quantidade = 0;

    if (p.empty()) {
        cout << endl << "Sem produtos cadastrados." << endl;
    } else {
        cout << endl;
        do {
            cout << "\t===== Produtos =====" << endl << endl;
            vector <size_t> mapa; //Armazena os valores reais do índice, iniciando em 0
            size_t idx = 1; //Índice amigável para a visualização do usuário
            for (size_t i = 0; i < p.size(); i++) {
                if (p[i].quantidade > 0) {
                    cout << "[" << idx << "]: " << p[i].nome <<
                        "; Quantidade: " << p[i].quantidade <<
                        "; Valor: " << p[i].valor << endl;
                    mapa.push_back(i); //Aloca o valor real do índice em "mapa"
                    idx++;
                }
            }

            escolha = campoNumero <size_t> ("\nSelecione um produto:\n[0] Cancelar\n---> ", 0, mapa.size(), "Produto inválido. Por favor, tente novamente.\n");

            if (escolha == 0) return;

            size_t indice = mapa[escolha - 1]; //Remove 1 do índice amigável, acessando o índice real do mapa

            quantidade = campoNumero <t_quantidade> ("\nQuanto do produto '" + p[indice].nome + "' você deseja comprar?\n[0] - Cancelar\n---> ", 0, p[indice].quantidade, "Quantidade inválida. Por favor, tente novamente.\n");
            if (quantidade == 0) return;

            t_valor precoTemp = p[indice].valor * quantidade;
            cout << "Valor da compra: " << precoTemp; //Exibe o valor da compra até o momento
            continuar = campoNumero <int> ("\n\nVocê deseja continuar comprando?\n[1] - Sim\n[2] - Não\n[3] - Cancelar operação\n---> ", 0, 3);

            if (continuar != 3) {
                pdtComprado.nome = p[indice].nome; //Recebe o valor do produto selecionado
                pdtComprado.quantidade = quantidade; //Recebe o valor do produto selecionado
                pdtComprado.valor = p[indice].valor * pdtComprado.quantidade; //Efetua o cálculo do valor
                pComprados.push_back(pdtComprado); //Aloca as informações no vetor passado por referência
                p[indice].quantidade -= quantidade; //Atualiza a quantidade do produto
                preco += p[indice].valor * quantidade; //Atualiza o valor da compra
            }

            if (continuar == 3 && pComprados.empty()) {
                preco = 0;
            }

        } while (continuar == 1);
        cout << "\nA soma dos itens é de R$: " << preco << endl;

        if (!atualizarArquivo(p)) {
            cout << "\nObs.: As alterações não serão salvas quando você sair\n";
        }
    }
}

string somaMes(int meses) { //https://www.w3schools.com/cpp/cpp_date.asp https://www.w3schools.com/cpp/cpp_ref_ctime.asp
    time_t hoje; //Armazena o tempo atual do sistema
    struct tm datetime; //Estrutura que armazena a data e hora em componentes separados (ano, mês, dia, hora)
    char r[50]; //Armazena a data

    hoje = time(NULL); //Armazena o tempo atual
    datetime = * localtime(&hoje); //Recebe o tempo atual através de um ponteiro que aponta para "hoje"
    datetime.tm_mon += meses; //Adiciona os meses ao mês atual, indo de 0 a 11
    mktime(&datetime); //Ajusta o valor do mês caso ultrapasse o limite lógico | https://www.w3schools.com/cpp/ref_ctime_mktime.asp

    strftime(r, 50, "%d/%m/%y", & datetime); //Formata a data e a armazena em "r". Obs.: Essa função não reconhece strings
    return r;
}

void pagarProdutos(vector <Produto> &pComprados, t_valor &custo, vector <Produto> & p) {
    if (pComprados.empty()) {
        cout << endl << "Sem produtos selecionados" << endl;
        return;
    } else {
        cout << endl << "\t=== Produtos Selecionados ===" << endl << endl;
        for (size_t i = 0; i < pComprados.size(); i++) {
            cout << "Produto " << i + 1 << ": " << pComprados[i].nome <<
                "; Quantidade: " << pComprados[i].quantidade <<
                "; Valor: " << pComprados[i].valor << endl;
        }
        cout << endl << "Estimativa da compra: R$ " << custo << endl;

        int forma_pagamento = campoNumero <int> ("\nEscolha a forma de pagamento:\n[1] - À vista (5% de desconto)\n[2] - Parcelado (até 3x sem juros, até 12x com 10% de juros)\n[3] - Cancelar e devolver itens\n[0] - Cancelar Operação\n---> ", 0, 3);

        if (forma_pagamento == 0) {
            return;
        }
        
        if (forma_pagamento == 3) {
            cout << "\nOperação cancelada. Devolvendo itens ao estoque...\n";
            for (const auto &item_devolvido : pComprados) { //Recebe os itens que seriam comprados
                bool encontrado = false; //Verifica se o item existe no estoque
                for (auto &estoque_item : p) { 
                    if (estoque_item.nome == item_devolvido.nome) { //Recebe os itens do estoque
                        estoque_item.quantidade += item_devolvido.quantidade; //Devolve a quantidade
                        encontrado = true;
                        break; 
                    }
                }
                if (!encontrado) {
                    Produto pRetornado = item_devolvido;
                    pRetornado.valor = item_devolvido.valor / item_devolvido.quantidade; //Faz o inverso do cálculo do valor, descobrindo a quantidade
                    p.push_back(pRetornado); //Envia para o vetor de produtos
                }
            }
            pComprados.clear(); //Limpa o vetor de produtos comprados "carrinho"
            custo = 0.0;        

            if (!atualizarArquivo(p)) {
              cout << "\n Obs.: Ocorreu um erro ao devolver os itens.\n";
            } 
            return;
        }

        int vezes = 1;
        t_valor final = custo;

        if (forma_pagamento == 2) {
            vezes = campoNumero <int> ("Escolha o nº de vezes:\n[0] Cancelar operação\n---> ", 0, 12);
            if (vezes == 0) return;
            if (vezes > 3) final += final * 0.1;
        }

        if (vezes == 1) {
          final -= final * 0.05;
        }

        cout << "\n\t===================\n";
        for (size_t i = 0; i < pComprados.size(); i++) {
          cout << "(" << pComprados[i].quantidade << "x) " << pComprados[i].nome << ": " << pComprados[i].valor << endl;
        }
        cout << " = R$ " << custo << endl;

        cout << "\t=== Pagamentos ===\n";
        if (vezes == 1)
          cout << "Desconto aplicado: 5%\n";

        if (vezes > 3)
          cout << "Juros aplicado: 10%\n";

        for (int i = 1; i <= vezes; i++) {
          string mes = (forma_pagamento == 1) ? somaMes(0) : somaMes(i);
          cout << mes << ": " << "R$: " << final / vezes << endl;
        }

        cout << "\n=== Valor total ===\n" << " R$: " << final << endl;

        int escolha = campoNumero <int> ("[1] - Prosseguir\n[0] - Cancelar operação\n---> ", 0, 1);
        if (escolha == 0) {
            return;
        }
        pComprados.clear(); //Limpa os produtos comprados "carrinho"
    }
}

int main() {
    system("chcp 65001"); //Permite o uso de acentos

    vector <Produto> produtos = lerProdutos(ARQUIVO); //Recebe os produtos do arquivo
    vector <Produto> produtosComprados;

    int opcao;
    t_valor valor = 0;

    do {
        cout << endl << "\t===== Mercado =====" << endl << endl;
        cout << "Opção 1: Cadastrar produtos" << endl;
        cout << "Opção 2: Selecionar produtos para a venda" << endl;
        cout << "Opção 3: Pagar produtos" << endl;
        cout << "Opção 4: Fechar sistema" << endl << endl;
        cout << "Selecione a opção que lhe convém:\n---> ";
        cin >> opcao;
        cin.ignore();
        cout << endl;

        switch (opcao) {
        case 1:
            cadastrarProduto(produtos);
            break;

        case 2:
            venderProdutos(produtos, produtosComprados, valor);
            break;

        case 3:
            pagarProdutos(produtosComprados, valor, produtos);
            break;

        case 4:
            cout << endl << "Programa encerrado." << endl;
            break;

        default:
            cout << endl << "Opção inválida." << endl;
            break;
        }
    } while (opcao != 4);

    return 0;
}