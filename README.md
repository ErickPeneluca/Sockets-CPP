# Sockets-C++

Implementação de comunicação cliente-servidor usando sockets TCP em C++, com suporte para IPv4 e IPv6.

## Descrição

Este projeto implementa um sistema de comunicação cliente-servidor onde:
- O cliente se conecta ao servidor e envia uma lista de arquivos de um diretório
- O servidor recebe a lista e salva em um arquivo
- Utiliza byte stuffing para evitar conflitos com a mensagem de encerramento "bye"

## Compilação

Utiliza o compilador GCC (g++).

```bash
make
```

## Execução

### Servidor

```bash
./server
```

O servidor pedirá:
- Host (ex: 127.0.0.1, 0.0.0.0, ::1)
- Porta (ex: 8080)

### Cliente

```bash
./client
```

O cliente pedirá:
- Host do servidor (ex: 127.0.0.1)
- Porta do servidor (ex: 8080)
- Diretório para listar arquivos (ex: /tmp)

## Funcionalidades

- Suporte para IPv4 e IPv6
- Byte stuffing para nomes de arquivos que coincidem com "bye"
- Medição de tempo de transmissão
- Salvamento da lista de arquivos em formato texto

## Limpeza

```bash
make clean
```
