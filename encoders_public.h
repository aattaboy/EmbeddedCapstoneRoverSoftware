#ifndef ENCODERS_PUBLIC_H
#define ENCODERS_PUBLIC_H

typedef enum { ENCODERS_LEFT = 0, ENCODERS_RIGHT = 1 } EncoderID;

struct EncodersISRData {
  EncoderID encoder_id;
  uint32_t cycles;
};

void sendToEncodersQueueFromISR(struct EncodersISRData *data,
                                BaseType_t *higherPriorityTaskWoken);

struct EncoderCounts {
  int64_t left;
  int64_t right;
  uint8_t left_dir : 1;
  uint8_t right_dir : 1;
  uint32_t velocity_left;
  uint32_t velocity_right;
};

typedef int (*encoders_callback_t)(struct EncoderCounts *);

int registerEncodersCallback(encoders_callback_t callback);

#endif /* ENCODERS_PUBLIC_H */
