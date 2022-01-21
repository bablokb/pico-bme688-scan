// --------------------------------------------------------------------------
// Read BME688 sensor values with a Raspberry Pi Pico using the official Bosch-API
//
// Read sensor in forced-mode scanning the parameter space (heater/duration)
//
// Author: Bernhard Bablok
//
// https://github.com/bablokb/pico-bme688-scan
// --------------------------------------------------------------------------

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "bme68x_platform.h"

static float alt_fac;

// ---------------------------------------------------------------------------
// print sensor data to console

void print_data(uint32_t ts, struct bme68x_data *data, uint8_t index) {
  float temp, press, hum, gas;

  temp  = 0.01f  * data->temperature;
  press = 0.01f  * data->pressure/alt_fac;
  hum   = 0.001f * data->humidity;
  gas   = 0.001f * data->gas_resistance;

  #ifdef DEBUG
  // print every observation on a single line
  printf("%lu ms, %0.1f deg C, %0.0f hPa, %0.0f%%, %0.0f kOhm, 0x%x, %d\n",
         ts,temp,press,hum,gas,data->status,index);
  #else
  // print a single line for all measurements
  if (index) {
    printf(",%0.0f",gas);
  } else {
    // first measurement in cycle, print main sensor data
    printf("%lu,%0.1f,%0.0f,%0.0f,%0.0f",
            ts,temp,press,hum,gas);
  }
  #endif
}

// ---------------------------------------------------------------------------
// set heater configuration

void set_heater_conf(struct bme68x_dev *bme, struct bme68x_heatr_conf *conf, uint16_t temp, uint16_t dur) {
  conf->heatr_temp = temp;
  conf->heatr_dur  = dur;
  int8_t rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, conf, bme);
  bme68x_print_result("bme68x_set_heatr_conf", rslt);
}

// ---------------------------------------------------------------------------
// read sensor-data
void read_data(struct bme68x_dev *bme, struct bme68x_conf *conf,
               struct bme68x_heatr_conf *heatr_conf, struct bme68x_data *data) {
  int8_t rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, bme);
  bme68x_print_result("bme68x_set_op_mode", rslt);

  // Calculate delay period in microseconds
  uint32_t del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE,conf,bme) + (heatr_conf->heatr_dur*1000);
  bme->delay_us(del_period,bme->intf_ptr);

  // get data
  uint8_t n_fields;
  rslt = bme68x_get_data(BME68X_FORCED_MODE,data,&n_fields,bme);
  bme68x_print_result("bme68x_get_data",rslt);
}

// ---------------------------------------------------------------------------
// main loop: read data and print data

int main(void) {
  struct bme68x_dev bme;
  int8_t rslt;
  struct bme68x_conf conf;
  struct bme68x_heatr_conf heatr_conf;
  uint16_t heater_temp[] = {HEATER_TEMP};
  uint8_t  temp_n        = sizeof(heater_temp)/sizeof(uint16_t);
  uint16_t heater_dur[]  = {HEATER_DURATION};
  uint8_t dur_n          = sizeof(heater_dur)/sizeof(uint16_t);
  struct bme68x_data data[temp_n*dur_n];

  // basic initialization
  stdio_init_all();
  alt_fac = pow(1.0-ALTITUDE_AT_LOC/44330.0, 5.255);

  // interface initialization
  rslt = platform_interface_init(&bme);
  bme68x_print_result("bme68x_interface_init", rslt);

  // sensor initialization
  rslt = bme68x_init(&bme);
  bme68x_print_result("bme68x_init", rslt);

  // basic sensor configuration
  heatr_conf.enable = BME68X_ENABLE;
  conf.filter       = BME68X_FILTER_OFF;
  conf.odr          = BME68X_ODR_NONE;
  conf.os_hum       = BME68X_OS_16X;
  conf.os_pres      = BME68X_OS_1X;
  conf.os_temp      = BME68X_OS_2X;
  rslt = bme68x_set_conf(&conf, &bme);
  bme68x_print_result("bme68x_set_conf", rslt);

  // print header
  printf("TimeStamp(ms),Temp(deg C),Press(Pa),Hum(%%)");
  for (uint8_t t=0; t<temp_n; ++t) {
    for (uint8_t d=0; d<dur_n; ++d) {
      printf(",Gas(%d C,%d ms:kOhm)",heater_temp[t],heater_dur[d]);
    }
  }
  #ifdef DEBUG
  printf(",Status,Index");
  #endif
  printf("\n");

  // loop
  while (true) {
    uint32_t start_ts = platform_get_timestamp();
    for (uint8_t t=0; t<temp_n; ++t) {
      // for all temperatures ...
      for (uint8_t d=0; d<dur_n; ++d) {
        // ... and all durations:
        set_heater_conf(&bme,&heatr_conf,heater_temp[t],heater_dur[d]);
        uint32_t time_ms = platform_get_timestamp();
        read_data(&bme,&conf,&heatr_conf,&data[t*temp_n+d]);
        // print data
        print_data(time_ms,&data[t*temp_n+d],t*temp_n+d);
      }
    }
    #ifndef DEBUG
    printf("\n");
    #endif
    uint32_t sleep_time = 1000*UPDATE_INTERVAL - (platform_get_timestamp()-start_ts);
    platform_sleep_ms(sleep_time);
  }
  return rslt;
}
