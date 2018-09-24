#include "api/syscall.h"
#include "api/types.h"
#include "api/print.h"

typedef enum {OFF = 0, ON = 1} led_state_t;

/* Leds state */
led_state_t green_state  = ON;
led_state_t orange_state = OFF;
led_state_t red_state    = ON;
led_state_t blue_state   = OFF;

/* Blink state */
led_state_t display_leds = ON;

bool        button_pressed = false;
device_t    leds;
int         desc_leds;

int _main(uint32_t my_id)
{
    uint8_t         id, id_button;
    logsize_t       msg_size;
    e_syscall_ret   ret;

    printf("Hello, I'm LEDs task. My id is %x\n", my_id);

    /* Get the button task id to be able to communicate with it using IPCs */
    ret = sys_init(INIT_GETTASKID, "button", &id_button);
    if (ret != SYS_E_DONE) {
        printf("Task BUTTON not present. Exiting.\n");
        return 1;
    }

    /* Zeroing the structure to avoid improper values detected by the kernel */
    memset(&leds, 0, sizeof(leds));

    strncpy(leds.name, "LEDs", sizeof(leds.name));

    /*
     * Configuring the LED GPIOs. Note: the related clocks are automatically set
     * by the kernel.
     * We configure 4 GPIOs here corresponding to the STM32 Discovery F407 LEDs (LD4, LD3, LD5, LD6):
     *     - PD12, PD13, PD14 and PD15 are in output mode
     * See the datasheet of the board here for more information:
     * https://www.st.com/content/ccc/resource/technical/document/user_manual/70/fe/4a/3f/e7/e1/4f/7d/DM00039084.pdf/files/DM00039084.pdf/jcr:content/translations/en.DM00039084.pdf
     *
     * NOTE: since we do not need an ISR handler for the LED gpios, we do not configure it (we only need to
     * synchronously set the LEDs)
     */
    /* Number of configured GPIO */
    leds.gpio_num = 4;

    leds.gpios[0].kref.port = GPIO_PD;
    leds.gpios[0].kref.pin = 12;
    leds.gpios[0].mask     = GPIO_MASK_SET_MODE | GPIO_MASK_SET_PUPD |
                             GPIO_MASK_SET_TYPE | GPIO_MASK_SET_SPEED;
    leds.gpios[0].mode     = GPIO_PIN_OUTPUT_MODE;
    leds.gpios[0].pupd     = GPIO_PULLDOWN;
    leds.gpios[0].type     = GPIO_PIN_OTYPER_PP;
    leds.gpios[0].speed    = GPIO_PIN_HIGH_SPEED;

    leds.gpios[1].kref.port = GPIO_PD;
    leds.gpios[1].kref.pin = 13;
    leds.gpios[1].mask     = GPIO_MASK_SET_MODE | GPIO_MASK_SET_PUPD |
                             GPIO_MASK_SET_TYPE | GPIO_MASK_SET_SPEED;
    leds.gpios[1].mode     = GPIO_PIN_OUTPUT_MODE;
    leds.gpios[1].pupd     = GPIO_PULLDOWN;
    leds.gpios[1].type     = GPIO_PIN_OTYPER_PP;
    leds.gpios[1].speed    = GPIO_PIN_HIGH_SPEED;

    leds.gpios[2].kref.port = GPIO_PD;
    leds.gpios[2].kref.pin = 14;
    leds.gpios[2].mask     = GPIO_MASK_SET_MODE | GPIO_MASK_SET_PUPD |
                             GPIO_MASK_SET_TYPE | GPIO_MASK_SET_SPEED;
    leds.gpios[2].mode     = GPIO_PIN_OUTPUT_MODE;
    leds.gpios[2].pupd     = GPIO_PULLDOWN;
    leds.gpios[2].type     = GPIO_PIN_OTYPER_PP;
    leds.gpios[2].speed    = GPIO_PIN_HIGH_SPEED;

    leds.gpios[3].kref.port = GPIO_PD;
    leds.gpios[3].kref.pin = 15;
    leds.gpios[3].mask     = GPIO_MASK_SET_MODE | GPIO_MASK_SET_PUPD |
                             GPIO_MASK_SET_TYPE | GPIO_MASK_SET_SPEED;
    leds.gpios[3].mode     = GPIO_PIN_OUTPUT_MODE;
    leds.gpios[3].pupd     = GPIO_PULLDOWN;
    leds.gpios[3].type     = GPIO_PIN_OTYPER_PP;
    leds.gpios[3].speed    = GPIO_PIN_HIGH_SPEED;

    ret = sys_init(INIT_DEVACCESS, &leds, &desc_leds);

    if (ret) {
        printf("error: sys_init() %s\n", strerror(ret));
    } else {
        printf("sys_init() - success\n");
    }

    /*
     * Devices and ressources registration is finished
     */

    ret = sys_init(INIT_DONE);
    if (ret) {
        printf("error INIT_DONE: %s\n", strerror(ret));
        return 1;
    }

    printf("init done.\n");

    /*
     * Main task
     */

    while (1) {
        id = id_button;
        msg_size = sizeof(button_pressed);

        ret = sys_ipc(IPC_RECV_ASYNC, &id, &msg_size, (char*) &button_pressed);

        switch (ret) {
            case SYS_E_DONE:
                printf("BUTTON sent message: %x\n", button_pressed);

                if (button_pressed == true) {
                    /* Change leds state */
                    green_state   = (green_state == ON) ? OFF : ON;
                    orange_state  = (orange_state == ON) ? OFF : ON;
                    red_state     = (red_state == ON) ? OFF : ON;
                    blue_state    = (blue_state == ON) ? OFF : ON;

                    /* Show leds */
                    display_leds  = ON;
                }

                break;
            case SYS_E_BUSY:
                break;
            case SYS_E_DENIED:
            case SYS_E_INVAL:
            default:
                printf("sys_ipc(): error. Exiting.\n");
                return 1;
        }

        if (display_leds == ON) {
            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[0].kref.val, green_state);
            if (ret != SYS_E_DONE) {
                printf("sys_cfg(): failed\n");
                return 1;
            }

            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[1].kref.val, orange_state);
            if (ret != SYS_E_DONE) {
                printf("sys_cfg(): failed\n");
                return 1;
            }

            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[2].kref.val, red_state);
            if (ret != SYS_E_DONE) {
                printf("sys_cfg(): failed\n");
                return 1;
            }

            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[3].kref.val, blue_state);
            if (ret != SYS_E_DONE) {
                printf("sys_cfg(): failed\n");
                return 1;
            }
        } else {
            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[0].kref.val, 0);
            if (ret != SYS_E_DONE) {
                printf("sys_cfg(): failed\n");
                return 1;
            }
            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[1].kref.val, 0);
            if (ret != SYS_E_DONE) {
                printf("sys_cfg(): failed\n");
                return 1;
            }
            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[2].kref.val, 0);
            if (ret != SYS_E_DONE) {
                printf("sys_cfg(): failed\n");
                return 1;
            }
            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[3].kref.val, 0);
            if (ret != SYS_E_DONE) {
                printf("sys_cfg(): failed\n");
                return 1;
            }
        }

        /* Make the leds blink */
        display_leds = (display_leds == ON) ? OFF : ON;

        /* Sleeping for 500 ms */
        sys_sleep(500, SLEEP_MODE_INTERRUPTIBLE);
    }

    return 0;
}

