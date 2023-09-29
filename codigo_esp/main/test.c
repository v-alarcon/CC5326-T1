#include <stdio.h>
#include <string.h>


#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
#include "lwip/sockets.h" // Para sockets
#include "time.h" // Para obtener el tiempo
#include "esp_sntp.h"

//Credenciales de WiFi

#define WIFI_SSID "Depto H06 2.4"
#define WIFI_PASSWORD "ra2147av"
#define SERVER_IP     "192.168.1.166" // IP del servidor
#define SERVER_PORT   1234

// Variables de WiFi
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
static const char* TAG = "WIFI";
static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;



void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 10) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(char* ssid, char* password) {
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));

    // Set the specific fields
    strcpy((char*)wifi_config.sta.ssid, WIFI_SSID);
    strcpy((char*)wifi_config.sta.password, WIFI_PASSWORD);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", ssid,
                 password);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", ssid,
                 password);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
        IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
        WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}

void nvs_init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}


char* socket_tcp(char* msg, int len){
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr.s_addr);

    // Crear un socket
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Error al crear el socket");
        return "Error al crear el socket";
    }

    // Conectar al servidor
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        ESP_LOGE(TAG, "Error al conectar");
        close(sock);
        return "Error al crear el socket";
    }

    // Enviar mensaje al servidor
    send(sock, msg, len, 0);

    // Recibir respuesta

    char rx_buffer[128];
    int rx_len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    if (rx_len < 0) {
        ESP_LOGE(TAG, "Error al recibir datos");
        return "Error al crear el socket";
    }
    rx_buffer[rx_len] = '\0'; // Para que no haya basura al final
    ESP_LOGI(TAG, "Datos recibidos: %s", rx_buffer);
    
    // Cerrar el socket
    close(sock);
    char* ans = malloc(sizeof(char)*rx_len);
    strcpy(ans, rx_buffer);
    return ans;
}
// funtion to get the battery level, it returns a random int between 1 and 100
int getBatteryLevel(){
    time_t t;
    srand((unsigned) time(&t));
    int n = rand() % 100 + 1;
    return n;
}

char* header(char protocol, char transportLayer){
	// 12 bytes for the header
    char* head = malloc(12);

	// generate a random short of 2 bytes to use in the id
    char* id = "D1";
    // copy the id into the head first 2 bytes of the header
    memcpy(head, id, 2);
    
	// 6 bytes para MAC Adress
    uint8_t mac[6];
    esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
    // copy the mac into the head 2 to 8 bytes of the header
    memcpy(head+2, mac, 6);

	// 1 byte for the Transport Layer
    head[8] = transportLayer;

	// 1 byte for the ID Protocol
    head[9] = protocol;

	// 2 bytes para Length Message
    uint16_t *length = malloc(sizeof(uint16_t));

    if (protocol == '0')
    {
        *length = 13;
    }

    if (protocol == '1')
    {
        *length = 17;
    }

    if (protocol == '2')
    {
        *length = 27;
    }

    // copy the length into the head 10 to 12 bytes of the header
    memcpy(head+10, length, 2);
    
	return head;
}
// function to get the temperature, humidity, pressure and CO level
char* get_thpc(){
    // create a list of 10 bytes to store the data
    char* data = malloc(10);
    // create the data for the temperature using random, its a int between 5 and 30
    time_t t;
    srand((unsigned) time(&t));
    int n = rand() % 25 + 5;
    // copy the temperature into the data
    memcpy(data, &n, 1);
    // create the data for the pressure using random, its a int between 1000 and 1200
    n = rand() % 200 + 1000;
    // copy the pressure into the data
    memcpy(data+1, &n, 4);
    // create the data for the humidity using random, its a int between 30 and 80
    n = rand() % 50 + 30;
    // copy the humidity into the data
    memcpy(data+5, &n, 1);
    // create the data for the CO level using random, its a float between 30 and 200
    float f = (rand() % 170 + 30);
    //add random decimal
    f = f + (rand() % 100) / 100.0;
    // copy the CO level into the data
    memcpy(data+6, &f, 4);
    // return the data
    return data;
}

void app_main(void){
    // Connect via wifi
    nvs_init();
    wifi_init_sta(WIFI_SSID, WIFI_PASSWORD);
    ESP_LOGI(TAG,"Conectado a WiFi!\n");

    // Send message asking for the protocol and transport layer
    char* ans = socket_tcp("protocol_and_transport_layer",strlen("protocol_and_transport_layer"));
    char* ans2 = malloc(sizeof(char)*3);
    memcpy(ans2, ans, 2);
    ans2[2] = '\0';

    time_t t;
    //copy bytes from ans to t
    memcpy(&t, ans+2, 4);
    struct timeval tv;
    tv.tv_sec = t;
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);



    ESP_LOGI(TAG, "Protocol and transport layer: %s", ans2);
    sntp_sync_status_t sntp_get_sync_status(void);

    // the first column is the id of the protocol '0' means Protocolo 0, '1' means Protocolo 1, etc.
    // the second one is the transport layer '0' means TCP, '1' means UDP

    //protocol 0 via tcp
    if (ans[0]=='0' && ans[1]=='0') // 
    {
        //free ans variable
        free(ans);
        // create a list of 13 bytes to store the header and the data
        char* msg = malloc(13);
        ESP_LOGI(TAG, "Executing protocol 0 via tcp");
        //create header
        char* head = header('0', '0');
        // copy the header into the msg
        memcpy(msg, head, 12);
        // create the data for the battery level using random
        uint8_t batteryLevel = getBatteryLevel();
        // copy the battery level into the msg
        msg[12] = batteryLevel;
        // send the msg to the server
        ans = socket_tcp(msg, 13);
        // free the msg variable
        free(msg);
        // print the answer
        ESP_LOGI(TAG, "Answer: %s", ans);
    }

    //protocol 1 via tcp
    if (ans[0]=='1' && ans[1]=='0') // 
    {
        ESP_LOGI(TAG, "Executing protocol 1 via tcp");
        //free ans variable
        free(ans);
        // create a list of 17 bytes to store the header and the data
        char* msg = malloc(17);
        //create header
        char* head = header('1', '0');
        // copy the header into the msg
        memcpy(msg, head, 12);
        // create the data for the battery level using random
        uint8_t batteryLevel = getBatteryLevel();
        // copy the battery level into the msg
        msg[12] = batteryLevel;
        // create the data for the timestamp
        time_t ti;
        time(&ti);
        // copy the timestamp into the msg
        memcpy(msg+13, &t, 4);
        // send the msg to the server
        ans = socket_tcp(msg, 17);
        // free the msg variable
        free(msg);
        // print the answer
        ESP_LOGI(TAG, "Answer: %s", ans);
    }


    //protocol 2 via tcp
    if (ans[0]=='2' && ans[1]=='0') // 
    {
        ESP_LOGI(TAG, "Executing protocol 2 via tcp");
        //free ans variable
        free(ans);
        // create a list of 27 bytes to store the header and the data
        char* msg = malloc(27);
        //create header
        char* head = header('2', '0');
        // copy the header into the msg
        memcpy(msg, head, 12);
        // create the data for the battery level using random
        uint8_t batteryLevel = getBatteryLevel();
        // copy the battery level into the msg
        msg[12] = batteryLevel;
        // create the data for the timestamp
        time_t ti;
        time(&ti);
        // copy the timestamp into the msg
        memcpy(msg+13, &t, 4);
        // create the data for the temperature, humidity, pressure and CO level
        char* thpc = get_thpc();
        // copy the thpc into the msg
        memcpy(msg+17, thpc, 10);
        // send the msg to the server
        ans = socket_tcp(msg, 27);
        // free the msg variable
        free(msg);
        // print the answer
        ESP_LOGI(TAG, "Answer: %s", ans);

    }


    //protocol 3 via tcp
    if (ans[0]=='3' && ans[1]=='0') // 
    {
        ESP_LOGI(TAG, "do protocol 3 via tcp");
    }
    //protocol 4 via tcp
    if (ans[0]=='4' && ans[1]=='0') // 
    {
        ESP_LOGI(TAG, "do protocol 4 via tcp");
    }


    //protocol 0 via udp
    if (ans[0]=='0' && ans[1]=='1') // 
    {
        ESP_LOGI(TAG, "do protocol 0 via udp");
    }
    //protocol 1 via udp
    if (ans[0]=='1' && ans[1]=='1') // 
    {
        ESP_LOGI(TAG, "do protocol 1 via udp");
    }
    //protocol 2 via udp
    if (ans[0]=='2' && ans[1]=='1') // 
    {
        ESP_LOGI(TAG, "do protocol 2 via udp");
    }
    //protocol 3 via udp
    if (ans[0]=='3' && ans[1]=='1') // 
    {
        ESP_LOGI(TAG, "do protocol 3 via udp");
    }
    //protocol 4 via udp
    if (ans[0]=='4' && ans[1]=='1') // 
    {
        ESP_LOGI(TAG, "do protocol 4 via udp");
    }

}
