#include <WiFi.h>
#include <AdafruitIO_WiFi.h>
#include <DHT.h>

// Configurações do Wi-Fi
const char *ssid = "";
const char *password = "";

// Configurações do Adafruit IO
#define IO_USERNAME  ""
#define IO_KEY       ""

// Defina o pino de conexão do sensor DHT
#define DHTPIN          4     // Pino conectado ao sensor DHT (pode ser alterado)
#define DHTTYPE         DHT22 // DHT11 ou DHT22

// Instancia o sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Instâncias do Adafruit IO
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, ssid, password);
AdafruitIO_Feed *temperatureFeed = io.feed("temperatura");
AdafruitIO_Feed *toggleFeed = io.feed("ligadesliga");

// Variáveis de controle
bool sendingData = false;  // Controla se os dados estão sendo enviados
unsigned long elapsedTime = 0;  // Controla o tempo para o eixo X

void setup() {
  Serial.begin(115200);
  
  // Conectar-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado ao Wi-Fi!");

  // Conectar-se ao Adafruit IO
  io.connect();

  // Configura o botão toggle
  toggleFeed->onMessage(toggleHandler);

  Serial.println("Sistema pronto para ler e enviar dados.");
}

void loop() {
  io.run();  // Mantém a conexão com o Adafruit IO

  // Verifica se o envio de dados está habilitado
  if (sendingData) {
    // Gera um valor de temperatura aleatória entre 20°C e 30°C
    float temperature = random(200, 700) / 10.0;  // Temperatura aleatória de 20.0 a 30.0

    // Incrementa o tempo para o eixo X
    elapsedTime++;

    // Envia os dados para o Adafruit IO
    temperatureFeed->save(temperature);
    Serial.print("Enviado para Adafruit IO - Tempo: ");
    Serial.print(elapsedTime);
    Serial.print(" min, Temperatura: ");
    Serial.println(temperature);

    delay(10000); // Aguarda 1 minuto (60000ms) para a próxima atualização
  } else {
    // Caso o envio de dados esteja desligado, apenas aguarda sem enviar
    delay(1000);
  }
}

// Função chamada quando o botão toggle é pressionado
void toggleHandler(AdafruitIO_Data *data) {
  // Pega o valor enviado (1 ou 0) do botão
  if (data->toInt() == 1) {
    sendingData = true;  // Inicia o envio de dados
    Serial.println("Enviando dados...");
  } else {
    sendingData = false; // Para o envio de dados
    Serial.println("Envio de dados interrompido.");
  }
}
