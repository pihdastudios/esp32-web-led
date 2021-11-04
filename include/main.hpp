#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_vfs_dev.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include <sys/param.h>

#include <esp_http_server.h>

// #include "cJSON.h"

#define LED_PIN GPIO_NUM_23
#define LED_PIN2 GPIO_NUM_5

#define EXAMPLE_ESP_WIFI_SSID "null"
#define EXAMPLE_ESP_WIFI_PASS "027412345678"
#define EXAMPLE_ESP_MAXIMUM_RETRY 5

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

TaskHandle_t ISR = nullptr;
static bool led_status = false;

static SemaphoreHandle_t mutex;

void event_handler(void *arg, esp_event_base_t event_base,
                   int32_t event_id, void *event_data);
static int s_retry_num = 0;

char ip_address[16];

void button_task(void *pvParameter);
void serial_task(void *pvParameter);
void init_gpio();
void init_wifi();

esp_err_t root_get_handler(httpd_req_t *req);
esp_err_t hello_get_handler(httpd_req_t *req);
esp_err_t echo_post_handler(httpd_req_t *req);
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);
esp_err_t ctrl_put_handler(httpd_req_t *req);
httpd_handle_t start_webserver();
void stop_webserver(httpd_handle_t server);

void disconnect_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data);
void connect_handler(void *arg, esp_event_base_t event_base,
                     int32_t event_id, void *event_data);

const char TAG[] = "EXAMPLE";

extern const char root_start[] asm("_binary_root_html_start");
extern const char root_end[] asm("_binary_root_html_end");

static const httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler,
    .user_ctx = nullptr
};

static const httpd_uri_t hello = {
    .uri = "/hello",
    .method = HTTP_GET,
    .handler = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = (void *)"Hello World!"};

static const httpd_uri_t echo = {
    .uri = "/echo",
    .method = HTTP_POST,
    .handler = echo_post_handler,
    .user_ctx = nullptr};

static const httpd_uri_t ctrl = {
    .uri = "/ctrl",
    .method = HTTP_PUT,
    .handler = ctrl_put_handler,
    .user_ctx = nullptr};

extern "C"
{
    void app_main();
}