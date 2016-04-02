#ifndef ENCODERS_PUBLIC_H
#define ENCODERS_PUBLIC_H

typedef enum { LEFT = 0, RIGHT = 1 } EncoderID;

void sendToEncodersQueueFromISR(EncoderID encoder_id,
                                BaseType_t *higherPriorityTaskWoken);

struct EncoderCounts {
  int64_t left;
  int64_t right;
};

typedef int (*encoders_callback_t)(struct EncoderCounts *);

int registerEncodersCallback(encoders_callback_t callback);

#endif /* ENCODERS_PUBLIC_H */
