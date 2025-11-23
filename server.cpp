#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fstream>

using namespace std;

int main() {
    string host, porta;
    
    cout << "Digite o host do servidor (ex: 127.0.0.1, 0.0.0.0, ::1): ";
    cin >> host;
    cout << "Digite a porta do servidor: ";
    cin >> porta;
    
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    const char* hostPtr = host.c_str();
    if (host == "0.0.0.0" || host == "::") {
        hostPtr = NULL;
    }
    
    if (getaddrinfo(hostPtr, porta.c_str(), &hints, &res) != 0) {
        perror("getaddrinfo falhou");
        return 1;
    }
    
    int server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (server_fd < 0) {
        perror("Falha ao criar socket");
        freeaddrinfo(res);
        return 1;
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    if (bind(server_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Bind falhou");
        close(server_fd);
        freeaddrinfo(res);
        return 1;
    }
    
    freeaddrinfo(res);
    
    if (listen(server_fd, 3) < 0) {
        perror("Listen falhou");
        close(server_fd);
        return 1;
    }
    
    cout << "Servidor aguardando conexões em " << host << ":" << porta << "..." << endl;
    
    struct sockaddr_storage client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    
    if (client_socket < 0) {
        perror("Accept falhou");
        close(server_fd);
        return 1;
    }
    
    cout << "Cliente conectado!" << endl;
    
    char buffer[4096] = {0};
    int bytesLidos = read(client_socket, buffer, sizeof(buffer));
    
    if (bytesLidos > 0) {
        string mensagem(buffer, bytesLidos);
        cout << "Mensagem recebida: " << mensagem << endl;
        
        if (mensagem == "READY") {
            const char* resposta = "READY ACK";
            send(client_socket, resposta, strlen(resposta), 0);
            cout << "Resposta enviada: " << resposta << endl;
            
            memset(buffer, 0, sizeof(buffer));
            bytesLidos = read(client_socket, buffer, sizeof(buffer));
            
            if (bytesLidos > 0) {
                string listaArquivos(buffer, bytesLidos);
                
                cout << "Lista de arquivos recebida:" << endl;
                
                string processado = "";
                string linhaAtual = "";
                
                for (int i = 0; i < listaArquivos.length(); i++) {
                    if (listaArquivos[i] == '\n') {
                        if (linhaAtual == "ESCAPED_bye") {
                            processado = processado + "bye\n";
                            cout << "bye (byte stuffing desfeito)" << endl;
                        } else if (linhaAtual.length() > 0) {
                            processado = processado + linhaAtual + "\n";
                            cout << linhaAtual << endl;
                        }
                        linhaAtual = "";
                    } else {
                        linhaAtual = linhaAtual + listaArquivos[i];
                    }
                }
                
                if (linhaAtual.length() > 0) {
                    if (linhaAtual == "ESCAPED_bye") {
                        processado = processado + "bye\n";
                        cout << "bye (byte stuffing desfeito)" << endl;
                    } else {
                        processado = processado + linhaAtual + "\n";
                        cout << linhaAtual << endl;
                    }
                }
                
                string nomeArquivo = host + "_" + porta + "_files.txt";
                ofstream arquivo(nomeArquivo);
                
                if (arquivo.is_open()) {
                    arquivo << processado;
                    arquivo.close();
                    cout << "Lista salva no arquivo: " << nomeArquivo << endl;
                } else {
                    cerr << "Erro ao salvar arquivo" << endl;
                }
                
                memset(buffer, 0, sizeof(buffer));
                bytesLidos = read(client_socket, buffer, sizeof(buffer));
                if (bytesLidos > 0) {
                    string mensagemBye(buffer, bytesLidos);
                    cout << "Mensagem de encerramento recebida: " << mensagemBye << endl;
                }
            }
        }
    }
    
    close(client_socket);
    close(server_fd);
    
    cout << "Servidor encerrado." << endl;
    
    return 0;
}