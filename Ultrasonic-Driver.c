

// Inclusion des bibléothèques
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/delay.h> // Inclusion pour `udelay`
#include <rtdm/driver.h>
#include <linux/ktime.h>

// Déclaration des constantes et des variables du driver
#define GPIO_SIG 16                   // GPIO utilisé pour Trigger et Echo
#define DEVICE_NAME "ultrasound_rtdm" // Nom du driver

// Timeout en microsecondes (1 seconde maximum)
#define TIMEOUT_US 1000000

// Variables pour les mesures
static rtdm_lock_t lock;
static long distance_cm = 0; // Distance mesurée en cm

// Fonction simulant `pulseIn` pour détecter un front haut avec timeout
static long pulse_in(uint32_t gpio_pin, uint32_t state, uint32_t timeout_us)
{
    ktime_t start_time, now;
    long duration = 0;

    start_time = ktime_get();

    // Attendre que tout signal précédent se termine
    while (gpio_get_value(gpio_pin) == state)
    {
        now = ktime_get();
        if (ktime_us_delta(now, start_time) > timeout_us)
        {
            return 0; // Timeout
        }
    }

    // Attendre le front haut
    while (gpio_get_value(gpio_pin) != state)
    {
        now = ktime_get();
        if (ktime_us_delta(now, start_time) > timeout_us)
        {
            return 0; // Timeout
        }
    }
    start_time = ktime_get(); // Début du signal haut

    // Attendre la fin du signal haut
    while (gpio_get_value(gpio_pin) == state)
    {
        now = ktime_get();
        if (ktime_us_delta(now, start_time) > timeout_us)
        {
            return 0; // Timeout
        }
    }

    duration = ktime_us_delta(ktime_get(), start_time); // Durée en µs
    return duration;
}

// Fonction d'envoi de la pulsation 'send_pulse() '
static void send_pulse(void)
{
    gpio_direction_output(GPIO_SIG, 1); // Configurer en sortie
    gpio_set_value(GPIO_SIG, 0);        // S'assurer que le signal est bas
    udelay(2);                          // Pause de 2 µs
    gpio_set_value(GPIO_SIG, 1);        // Envoyer un signal haut
    udelay(10);                         // Maintenir pendant 10 µs
    gpio_set_value(GPIO_SIG, 0);        // Envoyer un signal bas
    gpio_direction_input(GPIO_SIG);     // Reconfigurer en entrée pour attendre l'écho
}

// Fonction de mesure de la distance en centimètres
static long measure_distance(void)
{
    long duration, distance;

    send_pulse(); // Envoyer le signal Trigger

    // Attendre et mesurer la durée du signal Echo
    duration = pulse_in(GPIO_SIG, 1, TIMEOUT_US);

    // Vérifier si une durée valide a été mesurée
    if (duration == 0)
    {
        pr_info("Aucune impulsion détectée (timeout)\n");
        return -1; // Erreur de mesure
    }

    // Calculer la distance en centimètres
    distance = (duration * 343) / 20000; // Vitesse du son (343 m/s)
    pr_info("Durée: %ld µs, Distance: %ld cm\n", duration, distance);
    return distance;
}

// Fonction de lecture pour récupérer la distance
static ssize_t ultrasound_read(struct rtdm_fd *fd, void __user *buf, size_t len)
{
    long distance;

    // Mesurer la distance
    distance = measure_distance();

    // Protéger la variable avec un verrou pour éviter les accès concurrents
    rtdm_lock_get(&lock);
    distance_cm = distance;
    rtdm_lock_put(&lock);

    // Copier la distance mesurée vers l'espace utilisateur
    if (len >= sizeof(long))
    {
        if (rtdm_safe_copy_to_user(fd, buf, &distance, sizeof(long)) != 0)
        {
            return -EFAULT;
        }
    }

    return sizeof(long);
}

// Fonctions d'ouverture et de fermeture
static int ultrasound_open(struct rtdm_fd *fd, int flags)
{
    return 0;
}

static void ultrasound_close(struct rtdm_fd *fd)
{
}

// Structure du driver
static struct rtdm_driver ultrasound_driver = {
    .profile_info = RTDM_PROFILE_INFO(ultrasound_rtdm, RTDM_CLASS_TESTING, 1, 0),
    .device_flags = RTDM_NAMED_DEVICE,
    .device_count = 1,
    .ops = {
        .open = ultrasound_open,
        .close = ultrasound_close,
        .read_nrt = ultrasound_read,
    },
};

// Structure du périphérique
static struct rtdm_device ultrasound_device = {
    .driver = &ultrasound_driver,
    .label = DEVICE_NAME,
};

// Initialisation du module
static int __init ultrasound_init(void)
{
    int ret;

    rtdm_lock_init(&lock);

    // Initialiser le GPIO
    if ((ret = gpio_request(GPIO_SIG, THIS_MODULE->name)) != 0)
    {
        pr_err("Erreur: impossible de demander le GPIO %d\n", GPIO_SIG);
        return ret;
    }

    gpio_direction_input(GPIO_SIG); // Configurer initialement comme entrée

    // Enregistrer le périphérique
    ret = rtdm_dev_register(&ultrasound_device);
    if (ret != 0)
    {
        gpio_free(GPIO_SIG);
        pr_err("Erreur: impossible d'enregistrer le périphérique\n");
        return ret;
    }

    pr_info("Module ultrason chargé avec succès\n");
    return 0;
}

// Déchargement du module
static void __exit ultrasound_exit(void)
{
    rtdm_dev_unregister(&ultrasound_device);
    gpio_free(GPIO_SIG);
    pr_info("Module ultrason déchargé\n");
}

module_init(ultrasound_init);
module_exit(ultrasound_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Votre Nom");
MODULE_DESCRIPTION("Driver RTDM pour capteur ultrason ");
