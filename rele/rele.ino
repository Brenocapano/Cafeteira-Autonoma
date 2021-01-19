#include "virtuabotixRTC.h"
#include "EEPROM.h"
//IRRIGADOR
#define RELE_PIN 4

//Variaves globais
char COMMAND = 0;
String agenda = "";
int qtdAgua;

//Instancia do RTC
virtuabotixRTC clook(6, 7, 5);

/*
----------------------------------
  Metodos de manipulação serial
----------------------------------
*/

//Limpa serial
void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
} 

/*
Função para recuperar dados do serial enviados pelo java
------------------------------------------------------------------------------------------------------------------------------
Parametros:
  (tipo) (nome) (explicação)
------------------------------------------------------------------------------------------------------------------------------
  (int) limit        Define a quantidade de caracteres que vai ser recebida
------------------------------------------------------------------------------------------------------------------------------
  (int) timeout      Define uma quantidade de tempo em milisegundos que a função vai esperar a resposta caso a resposta não 
                     Atinja a quantidade de caracteres nessesários.
------------------------------------------------------------------------------------------------------------------------------
  (bool) useExclude  Ativar suporta para informaçoes de tamanho variado definindo um caractere a ser excluido no recebimento
                     Exemplo: 100### = 100
------------------------------------------------------------------------------------------------------------------------------
  (char) exclude     Define caractere que sera excluido no processamento.
------------------------------------------------------------------------------------------------------------------------------
  Retorna uma String com o dado recebido do seria.
*/
String getData(int limit, int timeout = 1000, bool useExclude = false,char exclude = '#'){
      String buffe = "";
      long int time = millis();
      while ((time + timeout) > millis())
      {
        //Enquando houver dados na Serial;
        while (Serial.available())
        {
          char c = Serial.read();
          //Verifica se o sistema de exclude é ativado e se o caractere recebido é igual ao excluido
          if (!(useExclude && c == exclude)){
            buffe += c; 
          }
          //Verifica se resposta já etingiu o limite;
          if (buffe.length() == limit){
            return buffe;
          }
        }
      }
      serialFlush();
      return buffe;
}

//Metodos auxiliares de resposta serial
void successMsg(){
  Serial.print('S');
}
void errorMsg(){
  Serial.print('F');
}

/*
------------------------------------
  FIM Metodos de manipulação serial
------------------------------------
*/

/*
--------------------
   Funcionalidades
--------------------
*/

//Metodo para programa o programa java se certificar que o arduino esta conectado
//e com o programa certo.
void hi(){
  Serial.println("hi");
}

//Função que formata é retorna a hora do RTC
String getHora(){
  //clook.updateTime();
  String data = "";
  data += clook.dayofweek;
  data += ";";
    if (clook.dayofmonth <= 9){
    data += "0";
  }
  data += clook.dayofmonth;
  data += ";";
    if (clook.month <= 9){
    data += "0";
  }
  data += clook.month;
  data += ";";
  data += clook.year;
  data += ";";
  if (clook.hours <= 9){
    data += "0";
  }
  data += clook.hours;
  data += ";";
  if (clook.minutes <= 9){
    data += "0";
  }
  data += clook.minutes;
  data += ";";
  if (clook.seconds <= 9){
    data += "0";
  }
  data += clook.seconds;
  return data;
}

/*
Matodo que seta a hora na memoria do arduino
----------------------------------
Parametros:
  (tipo) (nome) (explicação)
----------------------------------
  (int) w  Dia da semana 1-7       
----------------------------------
  (int) d  Dia do mês        
----------------------------------
  (int) m  Mês      
----------------------------------
  (int) y  Ano      
----------------------------------
  (int) h  Hora      
----------------------------------
  (int) i  Minuto      
----------------------------------
  (int) s  Segundo      
----------------------------------
*/
void setHora(int w, int d, int m, int y, int h, int i, int s){
  //Validação de data.
  if (w > 7 || w < 1 || d > 31 || d < 1 
      || m > 12 || m < 1 || y > 3000 || y < 2000 
      || h > 23 || h < 0 || i > 59 || i < 0 || s > 59 || s < 0){   
    errorMsg();
    return;
  }
  //Definir hora;
  clook.setDS1302Time(s, i, h, w, d, m, y);
  //Retornar mensagem de sucesso no serial;
  successMsg();
}

/*
GRAVAR quantidade de agua na memoria do arduino.
----------------------------------
Parametros:
  (tipo) (nome) (explicação)
----------------------------------
  (int) qtd  quantidade de agua em ml       
----------------------------------
*/
void setQtdAgua(String qtd){
  //Verifica se o valor não é o meso
  if (qtd.toInt() == qtdAgua){
    //Caso seja não faça nada.
    return;
  }
  //Grava na memoria
  for(int i = 0; i <= 5; i++ ){
    EEPROM.write(i, qtd.charAt(i));
  }
  //Atualiza a variavel local com o valor.
  qtdAgua = qtd.toInt();
  return;
}

/*
Metodo para o preparo do café de acordo com a quantidade de água.
----------------------------------
Parametros:
  (tipo) (nome) (explicação)
----------------------------------
  (int) ml  quantidade de agua em ml       
----------------------------------
*/
void fazerCafe(int ml){
  if (ml <= 0){
    return;
  }
  //Calcula o tempo baseado na vazão da bomba.
  int tempo = (60 * ml / 1250) * 1000;
  digitalWrite(RELE_PIN, HIGH);
  delay(tempo);
  digitalWrite(RELE_PIN, LOW);
  return;
}


/*
Metodo para salvar os dados de agendamento na memoria.
----------------------------------
Parametros:
  (tipo) (nome) (explicação)
----------------------------------
  (String) data  String com informações codificadas
                 Exemplo: 0203411111101 
----------------------------------
Codificação:
  0  - Ativado(1) | Desativado (0)
  00 - Hora (0 - 23)
  00 - Minuto (0 -59)
  0 - fazerCafe domingo (1) | Não fazerCafe domingo (0)
  0 - fazerCafe Segunda-feira (1) | Não fazerCafe Segunda-Feira (0)
  0 - fazerCafe Terça-feira (1) | Não fazerCafe Terça-feira (0) 
  0 - fazerCafe Quarta-Feira (1) | Não fazerCafe Quarta-Feira (0)
  0 - fazerCafe Quinta-feira (1) | Não fazerCafe Quinta-feira (0)
  0 - fazerCafe Sexta-feira (1) | Não fazerCafe Sexta-feira (0) 
  0 - fazerCafe Sábado (1) | Não fazerCafe Sábado (0)
  
*/
void setAgenda(String data){
    if (data.substring(0, 1).toInt() == 0){
      if (data == agenda){
        return;
      }
      EEPROM.write(20, (char)'0');
      agenda = (String)data;
      return;
    }
    //Validação da string
    if (data.substring(0, 1).toInt() < 0      || data.substring(0, 1).toInt()   > 1 
        || data.substring(1, 3).toInt() > 23  || data.substring(1, 3).toInt()   < 0
        || data.substring(3, 5).toInt() > 59  || data.substring(3, 5).toInt()   < 0
        || data.substring(5, 6).toInt() < 0   || data.substring(5, 6).toInt()   > 1
        || data.substring(6, 7).toInt() < 0   || data.substring(6, 7).toInt()   > 1
        || data.substring(7, 8).toInt() < 0   || data.substring(7, 8).toInt()   > 1
        || data.substring(8, 9).toInt() < 0   || data.substring(8, 9).toInt()   > 1   
        || data.substring(9, 10).toInt() < 0  || data.substring(9, 10).toInt()  > 1
        || data.substring(10, 11).toInt() < 0 || data.substring(10, 11).toInt() > 1
        || data.substring(11, 12).toInt() < 0 || data.substring(11, 12).toInt() > 1
        || data.substring(12, 13).toInt() < 0 || data.substring(12, 13).toInt() > 1                    
        ){
    errorMsg();
    return;
  }
  //Previne que a mesma agenda seja salva.
  if (data == agenda){
    return;
  }
  //Grava na memoria.
  for(int i = 0; i <= 13; i++ ){
    EEPROM.write(20+i, data.charAt(i));
  }
  //Atualiza variavel local.
  agenda = (String)data;
}

/*
======================
  FIM Funcionalidades
======================
*/

/*
Metodo que "escuta" entradas Serial é executa os devidos comandos de acordo com a tabela abaixo:
Comandos: (char)
--------------------------
  0 - Comando de verificação;
  1 - Retorna a hora;
  2 - Liga bomba de agua pelo determinado tempo e retorna o tempo que vai demorar;
  3 - Setar hora;
  4 - Setar quantidade de água;
  5 - Retorna a quantidade de água;
  6 - Mostra as informaçoes de agendamento;
  7 - Seta o agendamento;
  r - Reseta uma parte da EEPROM.
--------------------------
*/
void actionsListner(){
    if (Serial.available() > 0 ){
    COMMAND = Serial.read();
    
    //Handshake
    if(COMMAND == 'h'){ 
      hi();
    }

    //Envia a hora
    if (COMMAND == '2'){ 
      Serial.print("hora#");
      Serial.println(getHora());
    }

    //Comando fazerCafe simples
    if (COMMAND == '3'){
      //Retorna o tempo que a bomba ficará ligada
      //Para melhor experiencia de usuário na interface
      Serial.print((60 * qtdAgua / 1250) * 1000);
      fazerCafe(qtdAgua);
    }

    //Setar hora
    if (COMMAND == '4'){     
      //Exemplo de data recebida: 2231020170711
      String data = getData(15);
      setHora(
          //Formata data
          data.substring(0, 1).toInt(),
          data.substring(1, 3).toInt(),
          data.substring(3, 5).toInt(),
          data.substring(5, 9).toInt(),
          data.substring(9, 11).toInt(),
          data.substring(11, 13).toInt(),
          data.substring(13, 15).toInt()
        );
    }
    
    //Setar qtd de agua 
    if (COMMAND == '5'){
      serialFlush();
      String data = getData(5, 1000, true);
      setQtdAgua(data);
      //setQtdAgua(getData(5, 1000, true));
      Serial.print(qtdAgua);
    }

    //Mostrar quantidade de agua setada
    if (COMMAND == '6'){
      Serial.print("qtdAgua#");
      Serial.print(qtdAgua);
    }

    //Mostra o agendamento
    if (COMMAND == '7'){
      Serial.print("agenda#");
      Serial.println(agenda);
    }
    // Seta agenda
    if (COMMAND == '8'){
      serialFlush();
      String data = getData(13);
      setAgenda(data);
      Serial.println(agenda);
    }
    
    //Resetar a memoria interna (EEPROM) do arduino!
    if (COMMAND == 'r'){
        for (int i = 0 ; i < 200; i++) {
          EEPROM.write(i, 0);
        }
    }
    
  }
  //Previne que 2 comandos sejam executados ao mesmo tempo.
  serialFlush();
}

//Variavel de proteção;
bool cafePronto = false;

//Metodo que verifica se é hora de fazerCafe
void agendaListner(){
  //Verifica se agendamento esta desativado.
  if (agenda == "0"){
    return;  
  }
  
  //Verifica se o horario corresponde ao agendado.
  if (
    agenda.substring(1, 3).toInt() == clook.hours
    && agenda.substring(3, 5).toInt() == clook.minutes){

    //Verifica se o dia da semana esta agendado.
    if (   (agenda.substring(5, 6).toInt() == 1 && clook.dayofweek == 1) 
        || (agenda.substring(6, 7).toInt() == 1 && clook.dayofweek == 2)
        || (agenda.substring(7, 8).toInt() == 1 && clook.dayofweek == 3)
        || (agenda.substring(8, 9).toInt() == 1 && clook.dayofweek == 4)
        || (agenda.substring(9, 10).toInt() == 1 && clook.dayofweek == 5)
        || (agenda.substring(10, 11).toInt() == 1 && clook.dayofweek == 6)
        || (agenda.substring(11, 12).toInt() == 1 && clook.dayofweek == 7)
        && clook.seconds == 0                                     
       ){
          cafePronto = true;
          fazerCafe(qtdAgua);
          return;
      }
      return;
  }
}

void setup()
{
  //Inicia serial
  Serial.begin(115200);
  
  //Recupera da memoria do arduino as informações de agendamento
  //e guarda em uma variavel local para diminuir as consultas na EEPROM
  for(int i = 0; i <= 13; i++ ){
    char c = (char)EEPROM.read(20+i);
    agenda += c;
  }

  //Recupera da memoria do arduino as informações de quantidade de água
  //e guarda em uma variavel local para diminuir as consultas na EEPROM
  String qtdAguaTemp = "";
  for(int i = 0; i <= 5; i++ ){
    char c = (char)EEPROM.read(i);
    qtdAguaTemp += c;
  }
  //Aloca quantidade de água recebida para variavel local  
  qtdAgua = qtdAguaTemp.toInt();
  
  //Define o pino do rele como output;
  pinMode(RELE_PIN, OUTPUT);
}

void loop() 
{
  clook.updateTime();
  actionsListner();
  //Condição auxiliar que faz a função fazerCafe seja chamada apenas uma unica vez.
  if (agenda.substring(3, 5).toInt() != clook.minutes && cafePronto){
    cafePronto = false; 
    return;
  }
  if (!cafePronto){
    agendaListner();
  }
}
