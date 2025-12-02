#ifndef UI_H
#define UI_H

#include <stdint.h>

/*
 * Simple on-device UI driven by GPIO buttons and LEDs.
 * - Long-press power to turn the device on/off.
 * - Short-press power to start the motor, callbacks for in-run use.
 * - Inc/Dec short-press callbacks for rpm changes once running.
 */

void ui_init(void);
void ui_update(void);
void ui_request_shutdown(void);
void ui_set_target_rpm(uint32_t rpm);

#endif /* UI_H */
