#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define ARQUIVO "produtos.txt"

template <typename I>
I campoNumero(const string &prompt, I min = numeric_limits<I>::lowest(), I max = numeric_limits<I>::max(), const string &msg_erro = "Entrada inválida. Por favor, tente novamente. \n") {
  I valor;
  string entrada;
  while (true) {
    cout << prompt;
    getline(cin, entrada);
    stringstream ss(entrada);

    if (!(ss >> valor) || !(ss.eof()) || valor < min || valor > max) {
      cout << msg_erro;
      continue;
    }
    return valor;
  }
}

string campoTexto(const string &prompt, const string &msg_erro = "Entrada inválida. Por favor, tente novamente. \n") {
  string entrada;

  while (true) {
    cout << prompt;
    getline(cin, entrada);

    if (entrada.empty()) {
      cout << msg_erro;
      continue;
    }

    return entrada;
  }
}


typedef int t_quantidade;
typedef double t_valor;
struct Produto {
  string nome;
  t_quantidade quantidade;
  t_valor valor;
};

vector<Produto> lerProdutos(const string& nomeArquivo = ARQUIVO) {
    vector<Produto> produtos;
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cout << "Não foi possível abrir o arquivo " << nomeArquivo << endl;
        return produtos;
    }

    string linha;
    while (getline(arquivo, linha)) {
        if (linha.empty()) continue;

        stringstream ss(linha);
        Produto p;
        string quantidadeStr, valorStr;

        if (!getline(ss, p.nome, ',')) continue;
        if (!getline(ss, quantidadeStr, ',')) continue;
        if (!getline(ss, valorStr, ',')) continue;
      
        p.quantidade = stoi(quantidadeStr);
        p.valor = stod(valorStr);
      
        produtos.push_back(p);
    }

    arquivo.close();
    return produtos;
}
bool atualizarArquivo( const vector<Produto> &p, const string& nomeArquivo = ARQUIVO) {
    ofstream arquivo(nomeArquivo, ios::trunc);

    if (!arquivo.is_open()) {
        cout << "Não foi possível abrir o arquivo " << nomeArquivo << endl;
        return false;
    }

    for (const auto& prod : p) {
        arquivo << prod.nome << ","
                << prod.quantidade << ","
                << prod.valor << "\n";
    }

    arquivo.close();
    return true;
}

void cadastrarProduto(vector<Produto> &p) {
  int quantidade_cadastrar;

  quantidade_cadastrar = campoNumero<int>(
      "\nQuantos itens você deseja cadastrar?\n[0] - cancelar\n---> ", 0, 50);

  if (quantidade_cadastrar == 0)
    return;

  for (int i = 0; i < quantidade_cadastrar; i++) {
    Produto pdt;
    pdt.nome = campoTexto("\nInsira o nome do produto:\n[0] - cancelar\n---> ");
    if (pdt.nome == "0")
      return;

    pdt.quantidade = campoNumero<int>("\nInsira a quantidade do produto '" + pdt.nome + "'\n[0] - cancelar\n---> ",0, 5000);
    if (pdt.quantidade == 0)
      return;

    pdt.valor = campoNumero<double>("\nInsira o valor do produto '" + pdt.nome + "'\n[0] - cancelar\n---> ", 0, 5000);
    if (pdt.valor == 0)
      return;

    p.push_back(pdt);
    if(!atualizarArquivo(p))  {
      cout << "\n Obs.: As alterações não serão salvas quando você sair";
    }
  }

  for (size_t i = 0; i < p.size(); i++) {
    for (size_t j = i + 1; j < p.size(); j++) {
      if (p[i].nome == p[j].nome) {
        p[i].quantidade += p[j].quantidade;
        p[i].valor = p[j].valor;
        p.erase(p.begin() + j);
      }
    }
  }
}

void venderProdutos(vector<Produto> &p, vector<Produto> &pComprados,t_valor &preco) {
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
      vector<size_t> mapa;
      size_t idx = 1;
      for (size_t i = 0; i < p.size(); i++) {
        if (p[i].quantidade > 0) {
          cout << "[" << idx << "]: " << p[i].nome
               << ";\tQuantidade: " << p[i].quantidade
               << ";\tValor: " << p[i].valor << endl;
          mapa.push_back(i);
          idx++;
        }
      } 

      escolha = campoNumero<size_t>("\nSelecione um produto:\n[0] cancelar\n---> ", 0, mapa.size(),"Produto inválido. Por favor, tente novamente. \n");
      
      if (escolha == 0) return;
      
      size_t indice = mapa[escolha - 1];

      quantidade = campoNumero<t_quantidade>("\nQuanto do produto '" + p[indice].nome + "' você deseja comprar?\n[0] - cancelar\n---> ",0,p[indice].quantidade, "Quantidade inválida. Por favor, tente novamente. \n");
      if (quantidade == 0) return;

      t_valor precoTemp = p[indice].valor * quantidade;
      cout << "Valor da compra: " << precoTemp;
      continuar = campoNumero<int>("\nVocê deseja continuar comprando?\n[1] - Sim\n[2] - Não\n[3] - Cancelar operação\n---> ",0,3);

      if (continuar != 3) {
        pdtComprado.nome = p[indice].nome;
        pdtComprado.quantidade = quantidade;
        pdtComprado.valor = p[indice].valor * pdtComprado.quantidade;
        pComprados.push_back(pdtComprado);
        p[indice].quantidade -= quantidade;
        preco += p[indice].valor * quantidade;
      }
      if (continuar == 3 && pComprados.empty()) {
        preco = 0;
      }
    } while (continuar == 1);
    cout << "\nA soma dos itens é de R$ " << preco;
    if(!atualizarArquivo(p))  {
      cout << "\n Obs.: As alterações não serão salvas quando você sair";
    }
  }
}

void pagarProdutos(vector<Produto> pComprados, float custo) {
  if (pComprados.empty()) {
    cout << endl << "Sem produtos seleciondos." << endl;
  } else {
    cout << endl << "\t=== Produtos Selecionados ===" << endl << endl;
    for (size_t i = 0; i < pComprados.size(); i++) {
      cout << "Produto " << i + 1 << ": " << pComprados[i].nome
           << "; Quantidade: " << pComprados[i].quantidade
           << "; Valor: " << pComprados[i].valor << endl;
    }
    cout << endl << "Estimativa da compra: R$ " << custo << endl;

    int forma_pagamento = campoNumero<int>("Escolha a forma de pagamento:\n[1] - A vista (5% de desconto)\n[2] - Parcelado (até 3x sem juros, até 12x com 10% de juros)\n[0] Cancelar\n---> ",0,2);
    if (forma_pagamento == 0) {
      return;
    } 
    int vezes = 1;
    float final = custo;
    if (forma_pagamento == 2 || forma_pagamento == 3) {
      vezes = campoNumero<int>("Escolha o nº de vezes:\n[0] Cancelar\n---> ", 0,12);
      if (vezes == 0) return;
      if (vezes > 3) final += final * 0.1;
    }
    if (vezes == 1) {
      final -= final * 0.05;
    }

 cout << "\n\t===================\n";
    for (int i = 0; i < pComprados.size(); i++) {
      cout << "\t(" << pComprados[i].quantidade << "x) " << pComprados[i].nome << ":\t" << pComprados[i].valor<< endl;
    } 
    cout << "\t = R$ " << custo << endl; 

    cout << "\t=== Pagamentos ===\n";
    if (vezes == 1) 
       cout << "\t Desconto aplicado: 5%\n"; 
    
    if (vezes > 3) 
     cout << "\t Juros aplicado: 10%\n";  
   
    for (int i = 1; i <= vezes; i++) {
      cout <<"\t"<<i << ": " << "R$ " << final / vezes << endl; //colocar a data
    }
    cout << "\n\t=== Valor total ===\n" << "\tR$ " << final << "\n\t===================\n";
  }
}

int main() {
  system("chcp 65001");

  vector<Produto> produtos = lerProdutos(ARQUIVO);
  vector<Produto> produtosComprados;
  
  
  int opcao;
  t_valor valor;

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
      ;
      break;

    case 2:
      venderProdutos(produtos, produtosComprados, valor);
      break;

    case 3:
      pagarProdutos(produtosComprados, valor);
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
