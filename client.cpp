#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>
#include <dirent.h>
#include <vector>

using namespace std;

long long getTimestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long tempo = tv.tv_sec * 1000000;
    tempo = tempo + tv.tv_usec;
    return tempo;
}

vector<string> listFiles(const string& diretorio) {
    vector<string> arquivos;
    DIR* dir = opendir(diretorio.c_str());
    
    if (dir == NULL) {
        cerr << "Erro ao abrir diretório: " << diretorio << endl;
        return arquivos;
    }
    
    struct dirent* entrada = readdir(dir);
    while (entrada != NULL) {
        string nome = entrada->d_name;
        if (nome != "." && nome != "..") {
            arquivos.push_back(nome);
        }
        entrada = readdir(dir);
    }
    
    closedir(dir);
    return arquivos;
}

int main() {
    string host, porta, diretorio;
    
    cout << "Digite o host do servidor: ";
    cin >> host;
    cout << "Digite a porta do servidor: ";
    cin >> porta;
    cout << "Digite o nome do diretório: ";
    cin >> diretorio;
    
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if (getaddrinfo(host.c_str(), porta.c_str(), &hints, &res) != 0) {
        perror("getaddrinfo falhou");
        return 1;
    }
    
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
        cout << "Erro ao criar socket" << endl;
        freeaddrinfo(res);
        return 1;
    }
    
    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        cout << "Falha na conexão com " << host << ":" << porta << endl;
        close(sock);
        freeaddrinfo(res);
        return 1;
    }
    
    freeaddrinfo(res);
    cout << "Conectado ao servidor " << host << ":" << porta << endl;
    
    long long tempoInicial = getTimestamp();
    
    string mensagemReady = "READY";
    send(sock, mensagemReady.c_str(), mensagemReady.length(), 0);
    cout << "Mensagem enviada: " << mensagemReady << endl;
    
    char buffer[1024] = {0};
    int bytesLidos = read(sock, buffer, 1024);
    if (bytesLidos > 0) {
        string resposta = buffer;
        cout << "Resposta do servidor: " << resposta << endl;
        
        if (resposta == "READY ACK") {
            vector<string> arquivos = listFiles(diretorio);
            
            if (arquivos.empty()) {
                cout << "Nenhum arquivo encontrado no diretório." << endl;
            } else {
                string listaArquivos = "";
                for (int i = 0; i < arquivos.size(); i++) {
                    if (arquivos[i] == "bye") {
                        listaArquivos = listaArquivos + "ESCAPED_bye\n";
                        cout << "Arquivo 'bye' detectado - aplicando byte stuffing" << endl;
                    } else {
                        listaArquivos = listaArquivos + arquivos[i] + "\n";
                    }
                }
                
                send(sock, listaArquivos.c_str(), listaArquivos.length(), 0);
                cout << "Lista de arquivos enviada (" << arquivos.size() << " arquivos)" << endl;
            }
        }
    }
    
    string mensagemBye = "bye";
    send(sock, mensagemBye.c_str(), mensagemBye.length(), 0);
    cout << "Mensagem 'bye' enviada. Fechando conexão..." << endl;
    
    long long tempoFinal = getTimestamp();
    long long tempoDecorrido = tempoFinal - tempoInicial;
    double tempoMs = tempoDecorrido / 1000.0;
    
    cout << "Tempo gasto: " << tempoDecorrido << " microssegundos (" << tempoMs << " ms)" << endl;
    
    close(sock);
    
    return 0;
}
