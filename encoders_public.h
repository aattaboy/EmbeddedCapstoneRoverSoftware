#ifndef ENCODERS_PUBLIC_H
#define ENCODERS_PUBLIC_H

typedef enum { ENCODERS_LEFT = 0, ENCODERS_RIGHT = 1 } EncoderID;

void sendToEncodersQueueFromISR(EncoderID encoder_id,
                                BaseType_t *higherPriorityTaskWoken);

struct EncoderCounts {
  int64_t left;
  int64_t right;
};

typedef int (*encoders_callback_t)(struct EncoderCounts *);

int registerEncodersCallback(encoders_callback_t callback);

extern uint8_t encoders_base_duty_cycle;

#endif /* ENCODERS_PUBLIC_H */
