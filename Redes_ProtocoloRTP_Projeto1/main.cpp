#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

char app_payload[20] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't'};
int sequencia = 0;
int acknum = 0;

typedef struct
{
    int seqnum;
    int acknum;
    int cheksum;
    char payload[20];
} packet_t;

typedef struct
{
    char data[20];
} msg_t;

packet_t filaPacotes[20];
/*------------------------------------------------------
Definição das funções que definem uma camada de aplicação
Funciona igual uma API chamada pelo software de aplicação
*/
//API para Alice
msg_t app_layer_A()
{
    msg_t message;
    printf("Escrevendo mensagem: ");
    for (int i = 0; i < 20; i++)
    {
        message.data[i] = app_payload[i];
        printf("%c", message.data[i]);
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

int cheksum(packet_t* pkt)
{
	int soma = pkt->seqnum + pkt->acknum;
	int i=0;

	for(i; i<20 ; i++)
	{
		soma += pkt->payload[i];
	}

	return(soma);

}


void A_output(msg_t message)
{
    printf("A_output - Criando pacotes.\n", message.data);

    packet_t packet;
    packet.seqnum = sequencia;
    for(int i = 0; i<20; i++){
        packet.payload[i] = message.data[i];
    }
    packet.cheksum = cheksum(&packet);
    packet.acknum = acknum;
    filaPacotes[sequencia] = packet;
}

void A_input(packet_t packet)
{
    printf("A_input - Recebendo pacote.\n");


    printf("Pacote recebido.\n");

    printf("Decodificando mensagem; \n");
    msg_t message;

    for(int i = 0; i<=sequencia ; i++){
        message.data[i] = packet.payload[i];
    }

    printf("Lendo mensagem: ");
    for(int i = 0; i<20 ; i++){
        printf("%c",message.data[i]);
    }

}

void A_init(){
    sequencia = 0;
    memset(filaPacotes, 0, sizeof(filaPacotes));
}

void B_init(){
    sequencia = 0;
    memset(filaPacotes, 0, sizeof(filaPacotes));
}

void B_input(packet_t packet)
{
    printf("A_input - Recebendo pacote\n");
    printf("Pacote recebido.\n");

    printf("Decodificando mensagem; \n");
    msg_t message;

    if(packet.cheksum == cheksum(&packet)){
        for(int i = 0; i<=20 ; i++) {
            message.data[i] = packet.payload[i];
        }

        app_layer_B(message);
    } else {
        printf("Arquivo corrompido!\n");
    }
}

void to_network_layer(){
    //Faz o envio dos pacotes
    for(int i = 0; i<=sequencia ; i++){
        B_input(filaPacotes[i]);
    }
}



int main()
{
    //Gera a mensagem
    msg_t message = app_layer_A();

    A_init();
    B_init();

    A_output(message);


    //Envia para camada de rede
    to_network_layer();


    return 0;
}
