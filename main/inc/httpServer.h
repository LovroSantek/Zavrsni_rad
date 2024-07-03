#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#ifdef __cplusplus
extern "C" {
#endif

extern float global_yaw, global_pitch, global_roll;

void http_server_start(void);

#ifdef __cplusplus
}
#endif

#endif
