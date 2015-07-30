#ifndef LED_H_
#define LED_H_

/* LEDs Pin and port definitions */

typedef enum LED_id {
    LED_BLUE,
    LED_GREEN,
    LED_RED
} LED_id;

static void vToggleLED( LED_id led );

#endif /* LED_H_ */
