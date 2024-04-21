#include <driver/gpio.h>
#include <driver/rtc_io.h>
#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <config.h>

#define GPIO_LED_GREEN  GPIO_NUM_18   /* Briefly flash if door is closed */
#define GPIO_LED_RED    GPIO_NUM_19   /* Briefly flash if door is open */

#define GPIO_SWITCH     GPIO_NUM_15   /* Door switch connected to this GPIO */
#define SLEEP_FOR_US    10000000      /* Microseconds = 10 seconds */

void setup() {
  Serial.begin(115200);

  /* LED outputs. */
  gpio_set_direction(GPIO_LED_GREEN, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_LED_RED, GPIO_MODE_OUTPUT);

  rtc_gpio_deinit(GPIO_SWITCH);
  
  print_wakeup_cause();
  const int pin_state = digitalRead(GPIO_SWITCH);
  Serial.printf("Pin state is : %d\n ", pin_state);
  flash_led(pin_state);     

  // Make sure to pull up the sensor input,
  rtc_gpio_pullup_en(GPIO_SWITCH);

  // Set up the conditions for waking up.
  esp_sleep_enable_ext0_wakeup(GPIO_SWITCH, pin_state ^ 1);
  //Serial.printf("Pin state XOR 1 is : %d\n ", pin_state ^ 1);
  esp_sleep_enable_timer_wakeup(SLEEP_FOR_US);
      
  Serial.println("ZZZzzz");
  esp_deep_sleep_start(); 
}

void print_wakeup_cause() {
  esp_sleep_wakeup_cause_t sleep_wakeup_cause;

  sleep_wakeup_cause = esp_sleep_get_wakeup_cause();

  switch(sleep_wakeup_cause) 
  {
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by GPIO switch pin"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

void flash_led(const int pin_state) {
  gpio_set_level(pin_state ? GPIO_LED_RED : GPIO_LED_GREEN, 1);
  gpio_set_level(pin_state ? GPIO_LED_GREEN : GPIO_LED_RED, 0);
  
  delay(50);  
  gpio_set_level(GPIO_LED_RED, 1);
  gpio_set_level(GPIO_LED_GREEN, 1);  
}

void loop() {  
}
