#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <time.h>

char app_payload[20] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't'};
int sequence = 0;
int acknum = 0;
clock_t tempoInicial, tempoFinal;

typedef struct {
    int seqnum;
    int acknum;
    int checksum;
    char payload[20];
} packet_t;

typedef struct {
    char data[20];
} msg_t;

packet_t packetsQueue[20];

//API para Alice
msg_t app_layer_A() {
    msg_t message;
    printf("Escrevendo mensagem: ");
    for (int i = 0; i < 20; i++) {
        message.data[i] = app_payload[i];
        printf("%c ", message.data[i]);
    }
    printf("\n");
    return message;
}

//API para Bob
void app_layer_B(msg_t message) {
    //é chamado para detectar quando um dado chegou algo
    printf("Lendo mensagem: ");
    for(int i = 0; i<20 ; i++){
        printf("%c ", message.data[i]);
    }
}

//Temporizador

void timer_start(){
    tempoInicial = clock();
}

void timer_stop(){
    tempoFinal = clock();
}

void A_time_interrupt(){
    timer_stop();
    clock_t difference = tempoFinal - tempoInicial;
    float msec = difference * 1000 / CLOCKS_PER_SEC;

    if(msec > 2500){
        printf("Time out: A execução excedeu o tempo limite (2500 milissegundos).");
        exit(0);
    }
}
void B_time_interrupt(){
    timer_stop();
    clock_t difference = tempoFinal - tempoInicial;
    float msec = difference * 1000 / CLOCKS_PER_SEC;

    if(msec > 2500){
        printf("Time out: A execução excedeu o tempo limite (2500 milissegundos).");
        exit(0);
    }
}

//Validador
int checksum(packet_t* pkt) {
	int soma = pkt->seqnum + pkt->acknum;
	int i = 0;

	for(i; i < 20; i++) {
		soma += pkt->payload[i];
	}

	return(soma);
}

//Camadas de transporte

//Alice
void A_output(msg_t message) {
    printf("A_output - Criando pacotes.\n");
    packet_t packet;
    packet.seqnum = sequence;
    for(int i = 0; i<20; i++){
        packet.payload[i] = message.data[i];
    }
    packet.checksum = checksum(&packet);
    packet.acknum = acknum;
    packetsQueue[sequence] = packet;
    A_time_interrupt();
    sequence++;
}

void A_input(packet_t packet) {
    printf("A_input - Recebendo pacote.\n");
    printf("Pacote recebido.\n");
    printf("Decodificando mensagem; \n");

    msg_t message;

    for(int i = 0; i < sequence ; i++) {
        message.data[i] = packet.payload[i];
    }

    printf("Lendo mensagem: ");
    for(int i = 0; i < 20 ; i++) {
        printf("%c ",message.data[i]);
    }
}

void A_init() {
    sequence = 0;
    acknum = 0;
    memset(packetsQueue, 0, sizeof(packetsQueue));
}


//Bob
void B_init() {
    sequence = 0;
    acknum = 0;
    memset(packetsQueue, 0, sizeof(packetsQueue));
}

void B_input(packet_t packet) {
    printf("A_input - Recebendo pacote\n");
    msg_t message;

    if(packet.checksum == checksum(&packet)){
        printf("Pacote recebido.\n");
        printf("Decodificando mensagem; \n");
        for(int i = 0; i<20 ; i++) {
            message.data[i] = packet.payload[i];
        }
    } else {
        printf("Arquivo corrompido!\n");
    }

    app_layer_B(message);

}

void to_network_layer() {
    timer_start();
    //Faz o envio dos pacotes
    for(int i = 0; i<sequence ; i++){
        B_input(packetsQueue[i]);
        B_time_interrupt();
    }
}

int main() {
    //Gera a mensagem
    msg_t message = app_layer_A();
    A_init();
    B_init();

    //Gera os pacotes
    tempoInicial = clock();
    A_output(message);

    //Envia para camada de rede
    to_network_layer();

    return 0;
}
