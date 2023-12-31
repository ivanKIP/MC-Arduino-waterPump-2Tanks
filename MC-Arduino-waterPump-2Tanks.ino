const byte T1_LEVEL_LOW  = 2;                             // № pin датчик ёмкости 1 "нижний уровень"
const byte T1_LEVEL_HIGH = 3;                             // № pin датчик ёмкости 1 "верхний уровень"
const byte T2_LEVEL_LOW  = 4;                             // № pin датчик ёмкости 2 "нижний уровень"
const byte FILL_T1_OUT   = 11;                            // № pin клапан наполнения ёмкости 1
const byte FILL_T2_OUT   = 12;                            // № pin клапан наполнения ёмкости 2
const unsigned long maxTankFillingTime = 2*60*60*1000UL;  // максимальное время работы насоса в мс (n часов * 60 мин * 60 сек)
unsigned long startTankFillingTime = 0;                   // время запуска насоса
const byte debounceDelay = 40;                            // фильтр дребезга (мс)

bool bT1_LOW, bT1_HIGH, bT2_LOW;                          // сигнал с датчиков
unsigned int step = 0;                                    // номер шага программы

// Переменные для функций подавления дребезга --->
unsigned long lastDebounceTimeT1L = 0;
unsigned long lastDebounceTimeT1H = 0;
unsigned long lastDebounceTimeT2L = 0;
bool lastSensorStateT1L = HIGH;
bool lastSensorStateT1H = HIGH;
bool lastSensorStateT2L = HIGH;
bool SensorStateT1L = HIGH;
bool SensorStateT1H = HIGH;
bool SensorStateT2L = HIGH;
// <---

void setup() {
  // входные пины - с подтяжкой к "+"
  pinMode(T1_LEVEL_LOW, INPUT_PULLUP);
  pinMode(T1_LEVEL_HIGH, INPUT_PULLUP);
  pinMode(T2_LEVEL_LOW, INPUT_PULLUP);
  pinMode(FILL_T1_OUT, OUTPUT);
  pinMode(FILL_T2_OUT, OUTPUT);
  digitalWrite(FILL_T1_OUT, HIGH);
  digitalWrite(FILL_T2_OUT, HIGH);
}

void loop() {
  bT1_HIGH = !readDebounceT1H();
  bT1_LOW  = !readDebounceT1L();
  bT2_LOW  = !readDebounceT2L();
  
  if (step == 0) {
    if (bT2_LOW && !bT1_LOW) {
      step = 2;
    }

    if (bT1_LOW) {
      step = 1;
    }
  }

  switch (step) {
    // наполнение ёмкости 1
    case 1:
      // отключаем выход 2 (на перелив из ёмкости 1 в 2)
      digitalWrite(FILL_T2_OUT, HIGH);
      // включаем выход 1 (насос для ёмкости 1) и светодиод
      digitalWrite(FILL_T1_OUT, LOW);
      digitalWrite(13, HIGH);
      if (startTankFillingTime == 0) {
        startTankFillingTime = millis();
      }
      if (bT1_HIGH || (millis() - startTankFillingTime >= maxTankFillingTime)) {
        step = 0;
      }
      break;
    
    // наполнение ёмкости 2 (перелив из 1)
    case 2:
      // отключаем выход 1 (насос для ёмкости 1) и светодиод
      digitalWrite(FILL_T1_OUT, HIGH);
      digitalWrite(13, LOW);
      // включаем выход 2 (на перелив из ёмкости 1 в 2)
      digitalWrite(FILL_T2_OUT, LOW);
      if (bT1_LOW) {
        step = 0;
      }
      break;

    default:
      // отключаем все выходы
      digitalWrite(FILL_T1_OUT, HIGH);
      digitalWrite(FILL_T2_OUT, HIGH);
      digitalWrite(13, LOW);
      // сбрасываем время запуска насоса
      startTankFillingTime = 0;
      break;
  }
}

bool readDebounceT1L() {
  bool reading = digitalRead(T1_LEVEL_LOW);

  if (reading != lastSensorStateT1L) {
    // reset the debouncing timer
    lastDebounceTimeT1L = millis();
  }

  if ((millis() - lastDebounceTimeT1L) > debounceDelay) {
    // if the button state has changed:
    if (reading != SensorStateT1L) {
      SensorStateT1L = reading;
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastSensorStateT1L = reading;

  return SensorStateT1L;
}

bool readDebounceT1H() {
  bool reading = digitalRead(T1_LEVEL_HIGH);

  if (reading != lastSensorStateT1H) {
    // reset the debouncing timer
    lastDebounceTimeT1H = millis();
  }

  if ((millis() - lastDebounceTimeT1H) > debounceDelay) {
    // if the button state has changed:
    if (reading != SensorStateT1H) {
      SensorStateT1H = reading;
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastSensorStateT1H = reading;

  return SensorStateT1H;
}

bool readDebounceT2L() {
  bool reading = digitalRead(T2_LEVEL_LOW);

  if (reading != lastSensorStateT2L) {
    // reset the debouncing timer
    lastDebounceTimeT2L = millis();
  }

  if ((millis() - lastDebounceTimeT2L) > debounceDelay) {
    // if the button state has changed:
    if (reading != SensorStateT2L) {
      SensorStateT2L = reading;
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastSensorStateT2L = reading;

  return SensorStateT2L;
}