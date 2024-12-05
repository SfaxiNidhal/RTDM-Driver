#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define ULTRASOUND_DEVICE "/dev/ultrasound_rtdm"
#define BUZZER_DEVICE "/dev/Buzzer_rtdm_driver_0"
#define LCD_I2C_ADDRESS 0x3E

int ultrasound_fd, buzzer_fd, lcd_fd;
long distance;

// Fonctions pour le LCD
void lcd_send_command(int fd, unsigned char cmd) {
    unsigned char buffer[2] = {0x80, cmd};
    write(fd, buffer, 2);
}

void lcd_send_data(int fd, const char *data) {
    unsigned char buffer[2];
    while (*data) {
        buffer[0] = 0x40;
        buffer[1] = *data++;
        write(fd, buffer, 2);
    }
}

void lcd_init(int fd) {
    lcd_send_command(fd, 0x01);  // Effacer l'écran
    usleep(2000);
    lcd_send_command(fd, 0x02);  // Retour au début de la ligne
    usleep(2000);
    lcd_send_command(fd, 0x0C);  // Activer l'écran sans curseur
}

int main() {
    char message[32];

    // Ouvrir les périphériques
    ultrasound_fd = open(ULTRASOUND_DEVICE, O_RDWR);
    if (ultrasound_fd < 0) {
        perror("Erreur d'ouverture du périphérique ultrason");
        return -1;
    }

    buzzer_fd = open(BUZZER_DEVICE, O_RDWR);
    if (buzzer_fd < 0) {
        perror("Erreur d'ouverture du périphérique buzzer");
        close(ultrasound_fd);
        return -1;
    }

    lcd_fd = open("/dev/i2c-1", O_RDWR);
    if (lcd_fd < 0) {
        perror("Erreur d'ouverture de l'I2C pour le LCD");
        close(ultrasound_fd);
        close(buzzer_fd);
        return -1;
    }

    if (ioctl(lcd_fd, I2C_SLAVE, LCD_I2C_ADDRESS) < 0) {
        perror("Erreur de configuration de l'adresse I2C");
        close(ultrasound_fd);
        close(buzzer_fd);
        close(lcd_fd);
        return -1;
    }

    // Initialiser le LCD
    lcd_init(lcd_fd);

    // Afficher "Start" sur le LCD
    lcd_send_command(lcd_fd, 0x01);  // Effacer l'écran
    usleep(2000);
    lcd_send_data(lcd_fd, "Start");
    usleep(1000);

    while (1) {
        // Lire la distance du capteur ultrason
        if (read(ultrasound_fd, &distance, sizeof(long)) < 0) {
            perror("Erreur de lecture du capteur ultrason");
            break;
        }

        printf("Distance mesurée : %ld cm\n", distance);

        // Ajuster le mode du buzzer en fonction de la distance
        if (distance > 40 && distance < 70) {
            write(buzzer_fd, "1", 1); // Mode 1
            printf("Mode 1 activé ");
        } else if (distance > 15 && distance <= 40) {
            write(buzzer_fd, "2", 1); // Mode 2
            printf("Mode 2 activé ");
        } else if (distance <= 15) {
            write(buzzer_fd, "3", 1); // Mode 3
            printf("Mode 3 activé : Danger!!");
        } else {
            write(buzzer_fd, "0", 1); // Désactiver le buzzer si aucune condition
            printf("Buzzer désactivé ");
        }

        // Afficher la distance sur le LCD
        snprintf(message, sizeof(message), "Dist: %ld cm", distance);
        lcd_send_command(lcd_fd, 0x01);  // Effacer l'écran
        usleep(2000);
        lcd_send_data(lcd_fd, message);

        usleep(500000); // Pause de 500 ms
    }

    close(ultrasound_fd);
    close(buzzer_fd);
    close(lcd_fd);
    return 0;
}

