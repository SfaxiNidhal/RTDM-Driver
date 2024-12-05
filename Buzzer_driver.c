#include <linux/version.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <rtdm/rtdm.h>
#include <rtdm/driver.h>
#include <linux/gpio.h>

#define GPIO_BUZZER 5 // Numéro de broche GPIO pour le buzzer
#define BUZZER_DATA_SIZE 128

static char Buzzer_data[BUZZER_DATA_SIZE];
static int buzzer = 0;                // État actuel du buzzer
static int mode = 1;                  // Mode de fonctionnement par défaut
static int periode = 700 * 1000 * 1000; // Période initiale (40 secondes)
rtdm_task_t task_buzzer;             // Tâche en temps réel pour faire sonner le buzzer

// Fonction pour ajuster la période selon le mode
void update_period_by_mode(int new_mode) {
    switch (new_mode) {
        case 1: periode = 500 * 1000 * 1000; break; // 40 secondes
        case 2: periode = 200 * 1000 * 1000; break; // 20 secondes
        case 3: periode = 50 * 1000 * 1000; break; // 10 secondes (urgence)
        default: periode = 700 * 1000 * 1000; break; // Mode par défaut
    }
    mode = new_mode;
    rtdm_task_set_period(&task_buzzer, 0, periode); // Met à jour la période de la tâche
}

void taskBuzzer(void *arg) {
    int value = 0;
    rtdm_printk("Lancement de la tâche du buzzer\n");
    while (!rtdm_task_should_stop()) {
        gpio_set_value(GPIO_BUZZER, value);  // Allume/éteint le buzzer
        value = !value;                      // Change l’état
        rtdm_task_wait_period(NULL);         // Attend la prochaine période
    }
}

// Fonction d'ouverture
static int Buzzer_open_function(struct rtdm_fd *fd, int flags) {
    rtdm_printk(KERN_INFO "Buzzer ouvert\n");
    return 0;
}

// Fonction de fermeture
static void Buzzer_close_function(struct rtdm_fd *fd) {
    rtdm_printk(KERN_INFO "Buzzer fermé\n");
}

// Fonction de lecture pour lire l'état du buzzer
static int Buzzer_read_nrt_function(struct rtdm_fd *fd, void __user *buffer, size_t lg) {
    char msg[64];
    snprintf(msg, sizeof(msg), "Mode: %d, Buzzer: %s\n", mode, (buzzer == 1) ? "Allumé" : "Éteint");
    if (lg > 0) {
        if (rtdm_safe_copy_to_user(fd, buffer, msg, lg) != 0) {
            return -EFAULT;
        }
    }
    return lg;
}

// Fonction d'écriture pour activer/désactiver le buzzer ou changer le mode
static int Buzzer_write_nrt_function(struct rtdm_fd *fd, const void __user *buffer, size_t lg) {
    if (lg > 0 && rtdm_safe_copy_from_user(fd, Buzzer_data, buffer, lg) == 0) {
        if (Buzzer_data[0] == '1') {   // Mode 1
            buzzer = 1;
            update_period_by_mode(1);
            rtdm_task_set_period(&task_buzzer, 0, periode);
            printk(KERN_INFO "Mode 1 activé, période : %d ns\n", periode); // Correction ici
        } else if (Buzzer_data[0] == '2') { // Mode 2
            buzzer = 1;
            update_period_by_mode(2);
            rtdm_task_set_period(&task_buzzer, 0, periode);
            printk(KERN_INFO "Mode 2 activé, période : %d ns\n", periode); // Correction ici
        } else if (Buzzer_data[0] == '3') { // Mode 3
            buzzer = 1;
            update_period_by_mode(3);
            rtdm_task_set_period(&task_buzzer, 0, periode);
            printk(KERN_INFO "Mode 3 activé, période : %d ns\n", periode); // Correction ici
        } else if (Buzzer_data[0] == '0') { // Désactiver le buzzer
            buzzer = 0;
            rtdm_task_set_period(&task_buzzer, 0, 0);
            gpio_set_value(GPIO_BUZZER, 0);
            printk(KERN_INFO "Buzzer désactivé\n");
        }
    }
    return lg;
}

/*static int Buzzer_write_nrt_function(struct rtdm_fd *fd, const void __user *buffer, size_t lg) {
    if (lg > 0 && rtdm_safe_copy_from_user(fd, Buzzer_data, buffer, lg) == 0) {
        if (Buzzer_data[0] == '1') {   // Activer le buzzer
            buzzer = 1;
            rtdm_task_set_period(&task_buzzer, 0, periode);
        } else if (Buzzer_data[0] == '0') { // Désactiver le buzzer
            buzzer = 0;
            rtdm_task_set_period(&task_buzzer, 0, 0);
            gpio_set_value(GPIO_BUZZER, 0);
        } else if (Buzzer_data[0] >= '1' && Buzzer_data[0] <= '3') { // Changer de mode
            int new_mode = Buzzer_data[0] - '0';
            update_period_by_mode(new_mode);
        }
    }
    return lg;
}*/

static struct rtdm_driver Buzzer_rt_driver = {
    .profile_info = RTDM_PROFILE_INFO(Buzzer_example, RTDM_CLASS_TESTING, 1, 1),
    .device_flags = RTDM_NAMED_DEVICE,
    .device_count = 1,
    .ops = {
        .open = Buzzer_open_function,
        .close = Buzzer_close_function,
        .read_nrt = Buzzer_read_nrt_function,
        .write_nrt = Buzzer_write_nrt_function,
    },
};

static struct rtdm_device Buzzer_rt_device = {
    .driver = &Buzzer_rt_driver,
    .label = "Buzzer_rtdm_driver_%d",
};

static int __init Buzzer_module_init(void) {
    int err;
    rtdm_dev_register(&Buzzer_rt_device);
    if ((err = gpio_request(GPIO_BUZZER, THIS_MODULE->name)) == 0) {
        if ((err = gpio_direction_output(GPIO_BUZZER, 0)) == 0) {
            if (rtdm_task_init(&task_buzzer, "rtdm-task-buzzer", taskBuzzer, NULL, 30, periode) == 0) {
                return 0;
            }
        }
        gpio_free(GPIO_BUZZER);
    }
    return err;
}

static void __exit Buzzer_module_exit(void) {
    rtdm_dev_unregister(&Buzzer_rt_device);
    gpio_free(GPIO_BUZZER);
    rtdm_task_destroy(&task_buzzer);
}

module_init(Buzzer_module_init);
module_exit(Buzzer_module_exit);

MODULE_LICENSE("GPL");

