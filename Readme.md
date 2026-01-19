# Simulation d'un Radar de Recul en Temps Réel

**Nom du Projet :** Simulation d'un Radar de Recul en Temps Réel  
**Version :** 0.0  
**Date de Création :** 05/12/2024  
**Auteur :** Nidhal Sfaxi  
**Organisation :** UPHF  
**Licence :** Open Source  

Ce projet décrit la simulation d’un radar de recul fonctionnant en temps réel sous le noyau Xenomai, incluant des drivers RTDM et une application utilisateur.

---

## Arborescence du Dossier `RTDM_Projet_SFAXI_Nidhal`

```
RTDM_Projet_SFAXI_Nidhal/
├── Rapport_Présentation/
│   ├── Rapport_RTDM_Projet.pdf
│   ├── Présentation_Projet.pptx
│
├── Projet/
│   ├── Makefile
│   ├── Ultrasonic_Driver.c
│   ├── Buzzer_Driver.c
│   ├── User_APP.c
│
├── Xenomai/
│   ├── Guide_Installation_Xenomai
```

---

# I. Description du Projet

Ce projet implémente un radar de recul en temps réel à l’aide du noyau Xenomai.  
Il inclut :

- un driver RTDM ultrason,  
- un driver RTDM buzzer,  
- une application utilisateur.

---

# II. Structure du Projet

```
Projet/
├── Makefile
├── Ultrasonic_Driver.c
├── Buzzer_Driver.c
├── User_APP.c
```

---

# III. Pré-requis

- Linux avec Xenomai installé  
- gcc, make, libxenomai-dev

---

# IV. Compilation & Exécution

## 1. Compilation

```
make
```

## 2. Charger les modules

```
sudo insmod ultrasound_rtdm.ko
sudo insmod buzzer_rtdm.ko
lsmod | grep rtdm
dmesg | tail
```

## 3. Exécution

```
sudo ./user_app
```

---

# V. Points Importants

- Charger les modules .ko avant exécution  
- Vérifier les noms générés par le compilateur  
- Décharger un module :

```
sudo rmmod nom_du_module.ko
```

---

# Contact

Email : N**********@gmail.com

---

# Fin
