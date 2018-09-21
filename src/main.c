#include "api/syscall.h"
#include "api/types.h"
#include "api/print.h"

enum led_state_t {OFF, ON};
enum led_state_t display_leds = ON;

device_t    leds;
int         desc_leds;

int _main(uint32_t my_id)
{
    uint8_t         id, id_button;
    char            msg;
    logsize_t       msg_size;
    e_syscall_ret   ret;

    printf("Hello, I'm LEDs task. My id is %x\n", my_id);

    ret = sys_init(INIT_GETTASKID, "button", &id_button);
    if (ret != SYS_E_DONE) {
        printf ("Task BUTTON not present. Exiting.\n");
        return 1;
    }

    /* Zeroing the structure to avoid improper values detected
     * by the kernel */
    memset (&leds, 0, sizeof (leds));

    strncpy (leds.name, "LEDs", sizeof (leds.name));

    /* Configuring the GPIOs. Note: the related clocks are automatically set
     * by the kernel
     */

    leds.gpio_num = 4; /* Number of configured GPIO */

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
        printf ("error: sys_init() %s\n", strerror(ret));
    } else {
        printf ("sys_init() - success\n");
    }

    /*
     * Devices and ressources registration is finished
     */

    ret = sys_init(INIT_DONE);
    if (ret) {
        printf ("error INIT_DONE: %s\n", strerror(ret));
        return 1;
    }

    printf ("init done.\n");

    /*
     * Main task
     */

    while (1) {
        id = id_button;
        msg_size = 1;
        ret = sys_ipc(IPC_RECV_SYNC, &id, &msg_size, &msg);
        if (ret != SYS_E_DONE) {
            printf ("sys_ipc(): error. Exiting.\n");
            return 1;
        }

        // printf ("BUTTON sent message: %x\n", msg);

        switch (msg) {
            case 0: display_leds = OFF; break;
            case 1: display_leds = ON; break;
            default:
                break; /* Invalid value - do nothing */
        }

        if (display_leds == ON) {
            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[0].kref.val, 1);
            if (ret != SYS_E_DONE) {
                printf ("sys_cfg(): failed\n");
                return 1;
            }

            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[1].kref.val, 1);
            if (ret != SYS_E_DONE) {
                printf ("sys_cfg(): failed\n");
                return 1;
            }

            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[2].kref.val, 1);
            if (ret != SYS_E_DONE) {
                printf ("sys_cfg(): failed\n");
                return 1;
            }

            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[3].kref.val, 1);
            if (ret != SYS_E_DONE) {
                printf ("sys_cfg(): failed\n");
                return 1;
            }
        } else {
            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[0].kref.val, 0);
            if (ret != SYS_E_DONE) {
                printf ("sys_cfg(): failed\n");
                return 1;
            }
            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[1].kref.val, 0);
            if (ret != SYS_E_DONE) {
                printf ("sys_cfg(): failed\n");
                return 1;
            }
            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[2].kref.val, 0);
            if (ret != SYS_E_DONE) {
                printf ("sys_cfg(): failed\n");
                return 1;
            }
            ret = sys_cfg(CFG_GPIO_SET, (uint8_t) leds.gpios[3].kref.val, 0);
            if (ret != SYS_E_DONE) {
                printf ("sys_cfg(): failed\n");
                return 1;
            }
        }

        sys_sleep (500, SLEEP_MODE_INTERRUPTIBLE);
    }

    return 0;
}

