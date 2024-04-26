#include <Keypad.h>             // biblioteca do teclado matricial
#include <LiquidCrystal_I2C.h>  // biblioteca do display lcd i2c

// variáveis de hardware

#define DISPLAY_ADRESS 0x27    // substituir pelo endereço i2c do seu display
#define DISPLAY_ROWS 4         // linhas do display
#define DISPLAY_CHARACTERS 20  // caracteres por linha do display
#define KEYPAD_ROWS 3          // qunatidade linhas do teclado
#define KEYPAD_COLUMNS 3       // quantidade colunas do teclado

byte keypadRowPins[KEYPAD_ROWS] = { 2, 3, 4 };        // pinos associados às linhas do teclado
byte keypadColumnPins[KEYPAD_COLUMNS] = { 6, 7, 8 };  // pinos associados às colunas do teclado
// mapeamento das teclas do teclado
char keypadKeyMap[KEYPAD_ROWS][KEYPAD_COLUMNS] = { '0', '1', '2',
                                                   '3', '4', '5',
                                                   '6', '7', '8'};
// instanciamento do teclado
Keypad keypad = Keypad(makeKeymap(keypadKeyMap), keypadRowPins, keypadColumnPins, KEYPAD_ROWS, KEYPAD_COLUMNS);

// instanciamento do display
LiquidCrystal_I2C display(DISPLAY_ADRESS, DISPLAY_CHARACTERS, DISPLAY_ROWS);
char quadradoCode = 255;
String quadrado = String(quadradoCode);

// variáveis de jogo

int turno = 1;                              // contador de turnos
String simbolos[2] = { "O", "X" };          // símbolos do jogo
String jogadores[2] = { "2", "1" };          // jogadores
String jogadorAtual = jogadores[turno % 2]; // jogador atual
String simboloAtual = simbolos[turno % 2];  // símbolo a ser desenhado na tela
// mapeamento do tabuleiro
String tabuleiro[3][3]{ quadrado, quadrado, quadrado,
                        quadrado, quadrado, quadrado,
                        quadrado, quadrado, quadrado };
bool andamento = true;                                                // true = em andamento, false = finalizado
bool vitoriaX, vitoriaO, velha = false;                               // possíveis estados finais do jogo
bool linhaX, colunaX, diagonalX, linhaO, colunaO, diagonalO = false;  // possíveis combinações de vitória

void (*resetarJogo) (void) = 0; // função de reset do jogo

void setup() {
  display.init();      // inicializa o display
  display.backlight();  // liga a iluminação de backlight do display

  desenharTabuleiro();  // exibe o grid na tela
}

void loop() {
  char tecla = keypad.getKey();  // leitura do teclado
  if (andamento) {                   // verifica se uma tecla foi pressionada
    if (tecla) {             // verifica se o jogo está em andamento
      switch (tecla) { // verifica qual foi a tecla pressionada e atualiza o tabuleiro
        case '0':
          atualizarEspaco(0, 0);
          break;
        case '1':
          atualizarEspaco(0, 1);
          break;
        case '2':
          atualizarEspaco(0, 2);
          break;
        case '3':
          atualizarEspaco(1, 0);
          break;
        case '4':
          atualizarEspaco(1, 1);
          break;
        case '5':
          atualizarEspaco(1, 2);
          break;
        case '6':
          atualizarEspaco(2, 0);
          break;
        case '7':
          atualizarEspaco(2, 1);
          break;
        case '8':
          atualizarEspaco(2, 2);
          break;
      }
      desenharTabuleiro();  // atualiza a tela do tabuleiro
    }
    if(turno > 5) {
      verificarVencedor();
      if(linhaX || colunaX || diagonalX) {
        vitoriaX = true;
        andamento = false;
      }
      else if(linhaO || colunaO || diagonalO) {
        vitoriaO = true;
        andamento = false;
      }
    }
    // encerra o jogo com resultado de velha ao finalizar o nono turno sem um vencedor
    if(turno > 9 && !vitoriaO && !vitoriaX) {
      andamento = false;
      velha = true;
    }
  }
  else {
    delay(1000);
    telaFinal();
  }
}

// função para desenhar o tabuleiro no display lcd
void desenharTabuleiro() {
  String texto = "Turno: Jogador " + jogadorAtual;  // texto do cabeçalho
  display.clear();
  display.setCursor(0, 0);
  display.print(texto);

  // itera pelo tabuleiro para desenhá-lo na tela
  for (int linha = 0; linha < 3; linha++) {
    for (int coluna = 0; coluna < 3; coluna++) {
      display.setCursor(coluna, linha + 1);
      display.print(tabuleiro[linha][coluna]);
    }
  }
}

// função para desenha a tela de fim de jogo
void telaFinal() {
  String texto = "Empate";

  if(vitoriaO) {
    texto = "Jogador 2 Venceu!";
  }
  else if(vitoriaX) {
    texto = "Jogador 1 Venceu!";
  }

  display.clear();

  display.setCursor(centralizarTexto(texto), 1);
  display.print(texto);
  display.setCursor(centralizarTexto("Reiniciando..."), 2);
  display.print("Reiniciando...");
  delay(2000);
  resetarJogo();
}

// função para centralizar o texto exibido no tabuleiro
int centralizarTexto(String texto) {
  int posicao = (20 - texto.length()) / 2;

  if (posicao < 0) {  // se o texto for maior que o aceito pelo display retorna a posição como 0
    return 0;
  }

  return posicao;  // retorna a posição do cursor para centralizar o texto
}

// função para verificar se o espaço do tabuleiro está disponível e alterar seu conteúdo
void atualizarEspaco(int linha, int coluna) {
  if(tabuleiro[linha][coluna] == quadrado) {
    tabuleiro[linha][coluna] = simboloAtual;
    turno++; // incrementa o contador de turnos
    simboloAtual = simbolos[turno % 2]; // atualiza o simbolo de acordo com o turno
  }
}

// função para verificar o vencedor do jogo;
void verificarVencedor() {
  // verificação de linha do X
  for(int linha = 0; linha < 3; linha++) {
    int contLinhaX = 0;
    for(int coluna = 0; coluna < 3; coluna++) {
      if(tabuleiro[linha][coluna] == "X") {
        contLinhaX++;
      }
    }
    if(contLinhaX == 3) {
      linhaX = true;
    }
  }

  //verificação de coluna do X
  for(int coluna = 0; coluna < 3; coluna++) {
    int contColunaX = 0;
    for(int linha = 0; linha < 3; linha++) {
      if(tabuleiro[linha][coluna] == "X") {
        contColunaX++;
      }
    }
    if(contColunaX == 3) {
      colunaX = true;
    }
  }

  // verificação de diagonal do X
  int contDiagonalX = 0;
  for(int linha = 0; linha < 3; linha++ ) {
    int coluna = linha;
    if(tabuleiro[linha][coluna] == "X") {
      contDiagonalX++;
      if(contDiagonalX == 3) {
        diagonalX = true;
      }
    }
  }

  contDiagonalX = 0;
  for(int linha = 0; linha < 3; linha++) {
    int coluna = 2 - linha;
    if(tabuleiro[linha][coluna] == "X") {
      contDiagonalX++;
      if(contDiagonalX == 3) {
        diagonalX = true;
      }
    }
  }

  // verificação de linha do O
  for(int linha = 0; linha < 3; linha++) {
    int contLinhaO = 0;
    for(int coluna = 0; coluna < 3; coluna++) {
      if(tabuleiro[linha][coluna] == "O") {
        contLinhaO++;
      }
    }
    if(contLinhaO == 3) {
      linhaO = true;
    }
  }

  // verificação de coluna do O
  for(int coluna = 0; coluna < 3; coluna++) {
    int contColunaO = 0;
    for(int linha = 0; linha < 3; linha++) {
      if(tabuleiro[linha][coluna] == "O") {
        contColunaO++;
      }
    }
    if(contColunaO == 3) {
      colunaO = true;
    }
  }

  // verificação de diagonal do O
  int contDiagonalO = 0;
  for(int linha = 0; linha < 3; linha++ ) {
    int coluna = linha;
    if(tabuleiro[linha][coluna] == "O") {
      contDiagonalO++;
      if(contDiagonalO == 3) {
        diagonalO = true;
      }
    }
  }

  contDiagonalO = 0;
  for(int linha = 0; linha < 3; linha++) {
    int coluna = 2 - linha;
    if(tabuleiro[linha][coluna] == "O") {
      contDiagonalO++;
      if(contDiagonalO == 3) {
        diagonalO = true;
      }
    }
  }
}
