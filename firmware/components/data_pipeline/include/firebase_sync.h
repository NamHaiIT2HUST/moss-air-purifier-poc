#ifndef FIREBASE_SYNC_H
#define FIREBASE_SYNC_H

#ifdef __cplusplus
extern "C" {
#endif

void firebase_sync_task(void *pvParameters);

void firebase_receive_task(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif // FIREBASE_SYNC_H