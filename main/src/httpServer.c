#include "httpServer.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "Fusion.h"

static const char *TAG = "HTTP_SERVER";

esp_err_t angles_get_handler(httpd_req_t *req)
{
    char* resp_str;
    asprintf(&resp_str, "%.3f, %.3f, %.3f", global_yaw, global_pitch, global_roll);
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    free(resp_str);
    return ESP_OK;
}

httpd_uri_t angles = {
    .uri       = "/angles",
    .method    = HTTP_GET,
    .handler   = angles_get_handler,
    .user_ctx  = NULL
};

void http_server_start(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &angles);
    }
}

