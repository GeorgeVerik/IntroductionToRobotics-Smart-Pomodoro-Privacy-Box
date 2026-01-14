#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

/* ---------- PIN MAP ---------- */
const int DIN_PIN = 12;
const int CLK_PIN = 11;
const int CS_PIN = 10;

const int JOY_X = A0;
const int JOY_Y = A1;
const int JOY_SW = 2;

const int LCD_RS = 9;
const int LCD_EN = 8;
const int LCD_D4 = 7;
const int LCD_D5 = 6;
const int LCD_D6 = 5;
const int LCD_D7 = 4;

const int BUZZER = 3;

/* ---------- ULTRASONIC SENSOR ---------- */
const int TRIG_PIN = A2;
const int ECHO_PIN = A3;
unsigned long lastUltrasonicRead = 0;
const int ultrasonicInterval = 100;  // ms
int lastDistance = 50;               // default far distance
const int normalEnemySpeed = 500;    // normal speed
const int fastEnemySpeed = 150;      // fast speed when hand is close

/* ---------- HARDWARE ---------- */
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

/* ---------- MENU ---------- */
String menuItems[] = { "Start", "Highscore", "About" };
const int menuCount = 3;
int menuIndex = 0;

/* ---------- GAME VARS ---------- */
int playerX = 3;
int score = 0;
int highscore = 0;
int level = 1;
bool gameOver = false;
unsigned long lastEnemyMove = 0;
unsigned long lastPlayerMove = 0;
int enemySpeed = normalEnemySpeed;

const int maxEnemies = 4;
int enemyX[maxEnemies], enemyY[maxEnemies];

const int maxBullets = 2;
int bulletX[maxBullets], bulletY[maxBullets];
bool bulletActive[maxBullets];

/* ---------- MOTHER SHIP ---------- */
bool motherShipActive = false;
int motherShipX = 0;
int motherShipDirection = 1;
unsigned long lastMotherMove = 0;
int motherShipSpeed = 300;  // ms per move
int motherShipSize = 3;     // 2 LEDs wide
int motherShipMaxHealth = 5;
int motherShipHealth;
int motherShipPoints = 150;
int passedEnemies = 0;
unsigned long motherStartTime = 0;     // track when mother ship appears
const int motherShipDuration = 15000;  // 15 seconds

/* ---------- NAME ENTRY ---------- */
const int nameLength = 3;
char playerName[nameLength + 1];  // +1 for null terminator
int nameIndex = 0;
char currentLetter = 'A';

/* ---------- FUNCTIONS ---------- */
void showMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  SPACE INVADERS  ");
  lcd.setCursor(0, 1);
  lcd.print(">");
  lcd.print(menuItems[menuIndex]);
  lcd.print("<");
}

void resetGame() {
  lc.clearDisplay(0);
  playerX = 3;
  score = 0;
  level = 1;
  enemySpeed = normalEnemySpeed;
  gameOver = false;
  passedEnemies = 0;
  motherShipActive = false;

  for (int i = 0; i < maxEnemies; i++) {
    enemyX[i] = random(0, 8);
    enemyY[i] = random(0, 3);
  }

  for (int i = 0; i < maxBullets; i++) {
    bulletActive[i] = false;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SPACE INVADERS");
  delay(800);
  lcd.clear();
}

void spawnBullet() {
  for (int i = 0; i < maxBullets; i++) {
    if (!bulletActive[i]) {
      bulletActive[i] = true;
      bulletX[i] = playerX;
      bulletY[i] = 6;
      break;
    }
  }
}

void levelUp() {
  level++;
  enemySpeed = max(120, enemySpeed - 50);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("LEVEL UP!");
  lcd.setCursor(3, 1);
  lcd.print("Level ");
  lcd.print(level);

  tone(BUZZER, 800, 200);
  delay(250);
  tone(BUZZER, 1000, 200);
  delay(250);
  tone(BUZZER, 1200, 300);
  delay(300);
}

void enterName() {
  lcd.clear();
  nameIndex = 0;
  currentLetter = 'A';

  while (nameIndex < nameLength) {
    lcd.setCursor(0, 0);
    lcd.print("Enter Name:");

    lcd.setCursor(0, 1);
    for (int i = 0; i < nameLength; i++) {
      if (i < nameIndex) lcd.print(playerName[i]);
      else if (i == nameIndex) lcd.print(currentLetter);
      else lcd.print("_");
    }

    int x = analogRead(JOY_X);
    if (x < 300) {
      currentLetter--;
      if (currentLetter < 'A') currentLetter = 'Z';
      delay(200);
    }
    if (x > 700) {
      currentLetter++;
      if (currentLetter > 'Z') currentLetter = 'A';
      delay(200);
    }
    if (digitalRead(JOY_SW) == LOW) {
      playerName[nameIndex] = currentLetter;
      nameIndex++;
      currentLetter = 'A';
      delay(300);
    }
  }
  playerName[nameLength] = '\0';
}

/* ---------- ULTRASONIC SENSOR ---------- */
void updateDistance() {
  if (millis() - lastUltrasonicRead >= ultrasonicInterval) {
    lastUltrasonicRead = millis();

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30ms timeout
    if (duration > 0) {
      lastDistance = duration * 0.034 / 2;
      lastDistance = constrain(lastDistance, 5, 50);
    }
  }
}

void adjustEnemySpeed() {
  if (lastDistance <= 15) {
    enemySpeed = fastEnemySpeed;
  } else {
    enemySpeed = normalEnemySpeed;
  }
}

/* ---------- MOTHER SHIP ---------- */
void checkMotherShip() {
  if (passedEnemies >= 10 && !motherShipActive) {
    motherShipActive = true;
    motherShipX = random(0, 8 - motherShipSize);
    motherShipHealth = motherShipMaxHealth;
    motherShipDirection = 1;
    motherStartTime = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MOTHER SHIP!");
    lcd.setCursor(0, 1);
    lcd.print("Shoot it now!");
    tone(BUZZER, 1500, 800);
    delay(1000);
  }

  if (motherShipActive) {
    if (millis() - lastMotherMove > motherShipSpeed) {
      motherShipX += motherShipDirection;
      if (motherShipX > 7 - (motherShipSize - 1)) motherShipDirection = -1;
      if (motherShipX < 0) motherShipDirection = 1;
      lastMotherMove = millis();
    }

    for (int i = 0; i < motherShipSize; i++) lc.setLed(0, 0, motherShipX + i, true);

    int remainingTime = (motherShipDuration - (millis() - motherStartTime)) / 1000;
    if (remainingTime < 0) remainingTime = 0;
    lcd.setCursor(10, 1);
    lcd.print("T:");
    lcd.print(remainingTime);
    lcd.print(" ");

    for (int b = 0; b < maxBullets; b++) {
      if (bulletActive[b] && bulletY[b] == 0) {
        for (int i = 0; i < motherShipSize; i++) {
          if (bulletX[b] == motherShipX + i) {
            motherShipHealth--;
            bulletActive[b] = false;
            tone(BUZZER, 1800, 100);
            delay(100);
            if (motherShipHealth <= 0) {
              motherShipActive = false;
              score += motherShipPoints;
              lcd.clear();
              lcd.setCursor(1, 0);
              lcd.print("MOTHER SHIP");
              lcd.setCursor(2, 1);
              lcd.print("DESTROYED!");
              tone(BUZZER, 2000, 200);
              delay(150);
              tone(BUZZER, 1800, 200);
              delay(150);
              tone(BUZZER, 2200, 300);
              delay(300);
              passedEnemies = 0;
            }
          }
        }
      }
    }

    if (millis() - motherStartTime >= motherShipDuration) {
      motherShipActive = false;
      gameOver = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MOTHER SHIP");
      lcd.setCursor(0, 1);
      lcd.print("ESCAPED! GAME OVER");
      tone(BUZZER, 150, 1000);
    }
  }
}

/* ---------- RUN GAME ---------- */
void runGame() {
  updateDistance();
  adjustEnemySpeed();

  unsigned long now = millis();

  int x = analogRead(JOY_X);
  if (now - lastPlayerMove > 120) {
    if (x < 300 && playerX > 0) playerX--;
    if (x > 700 && playerX < 7) playerX++;
    lastPlayerMove = now;
  }

  if (digitalRead(JOY_SW) == LOW) spawnBullet();

  if (now - lastEnemyMove > enemySpeed) {
    for (int i = 0; i < maxEnemies; i++) enemyY[i]++;
    lastEnemyMove = now;
  }

  for (int i = 0; i < maxBullets; i++) {
    if (bulletActive[i]) {
      bulletY[i]--;
      if (bulletY[i] < 0) bulletActive[i] = false;
    }
  }

  for (int i = 0; i < maxEnemies; i++) {
    if (enemyY[i] == 7 && enemyX[i] == playerX) {
      gameOver = true;
      if (score > highscore) {
        highscore = score;
        EEPROM.update(0, highscore);
        enterName();
        for (int j = 0; j < nameLength; j++) { EEPROM.update(2 + j, playerName[j]); }
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("GAME OVER");
      lcd.setCursor(0, 1);
      lcd.print("Score: ");
      lcd.print(score);
      tone(BUZZER, 200, 300);
      delay(1500);
      return;
    }

    for (int b = 0; b < maxBullets; b++) {
      if (bulletActive[b] && bulletX[b] == enemyX[i] && bulletY[b] == enemyY[i]) {
        score += 10;
        bulletActive[b] = false;
        enemyX[i] = random(0, 8);
        enemyY[i] = 0;
        passedEnemies = 0;
        tone(BUZZER, 1000, 100);
        delay(100);
        if (score % 40 == 0) levelUp();
      }
    }

    if (enemyY[i] > 7) {
      enemyY[i] = 0;
      enemyX[i] = random(0, 8);
      passedEnemies++;
    }
  }

  lc.clearDisplay(0);
  lc.setLed(0, 7, playerX, true);
  for (int i = 0; i < maxEnemies; i++) 
  lc.setLed(0, enemyY[i], enemyX[i], true);
  for (int i = 0; i < maxBullets; i++)
    if (bulletActive[i]) lc.setLed(0, bulletY[i], bulletX[i], true);

  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.print(score);
  lcd.setCursor(0, 1);
  lcd.print("Lvl:");
  lcd.print(level);

  checkMotherShip();
}

/* ---------- SETUP ---------- */
void setup() {
  pinMode(JOY_SW, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  lc.shutdown(0, false);
  lc.clearDisplay(0);
  lc.setIntensity(0, 8);

  lcd.begin(16, 2);
  highscore = EEPROM.read(0);

  showMenu();
}

/* ---------- LOOP ---------- */
enum Mode { MENU,
            GAME };
Mode mode = MENU;

void loop() {
  if (mode == MENU) {
    int x = analogRead(JOY_X);
    if (x < 300) {
      menuIndex = (menuIndex - 1 + menuCount) % menuCount;
      showMenu();
      delay(250);
    }
    if (x > 700) {
      menuIndex = (menuIndex + 1) % menuCount;
      showMenu();
      delay(250);
    }

    if (digitalRead(JOY_SW) == LOW) {
      delay(200);
      if (menuIndex == 0) {
        resetGame();
        mode = GAME;
      } else if (menuIndex == 1) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Highscore:");
        char savedName[nameLength + 1];
        for (int i = 0; i < nameLength; i++) { savedName[i] = EEPROM.read(2 + i); }
        savedName[nameLength] = '\0';
        lcd.setCursor(0, 1);
        lcd.print(savedName);
        lcd.print(":");
        lcd.print(highscore);
        delay(2000);
        showMenu();
      } else if (menuIndex == 2) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Space Invaders");
        lcd.setCursor(0, 1);
        lcd.print("By George");
        delay(1500);
        showMenu();
      }
    }
  } else if (mode == GAME) {
    if (!gameOver) runGame();
    else {
      mode = MENU;
      showMenu();
    }
  }
}
