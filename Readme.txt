******************************************************************************************************************************************************
                                                    Simulation d'un Radar de Recul en Temps Réel
******************************************************************************************************************************************************
Nom du Projet       : Simulation d'un Radar de Recul en Temps Réel  
Version             : 0.0  
Date de Création    : 05/12/2024  
Auteur              : Nidhal Sfaxi  
Organisation        : UPHF 
Licence             : Open Source  
Description         : Ce fichier explique le fonctionnement et les étapes nécessaires pour simuler un Radar de recul .
******************************************************************************************************************************************************
Arborescence du Dossier compressé RTDM_Projet_SFAXI_Nidhal:

Voici la structure détaillée du dossier RTDM_Projet_SFAXI_Nidhal 
    RTDM_Projet_SFAXI_Nidhal/
    ├── Rapport_Présentation/
    │   ├── Rapport_RTDM_Projet.pdf       : Rapport détaillant les étapes nécessaires pour implémenter un driver RTDM pour un capteur ultrason.
    │   ├── Présentation_Projet.pptx      : Présentation décrivant l'architecture globale du projet.
    │
    ├── Projet/
    │   ├── Makefile                      : Fichier contenant les règles pour compiler tous les composants du projet.
    │   ├── Ultrasonic_Driver.c           : Driver RTDM pour le capteur ultrason.
    │   ├── Buzzer_Driver.c               : Driver RTDM pour le buzzer.
    │   ├── User_APP.c                    : Application utilisateur orchestrant le fonctionnement du système.
    │
    ├── Xenomai/
    │   ├── Guide_Installation_Xenomai : Démarche pour installer et patcher le noyau Xenomai.
                                         

Détails des Dossiers et Fichiers
    1. Rapport/
        Rapport_RTDM_Projet.pdf : Un document en PDF expliquant les étapes nécessaires pour développer un driver RTDM pour un capteur ultrason.
        Présentation_Projet.pptx : Une présentation PowerPoint expliquant l'architecture du projet et le rôle des composants.
    2. Projet/
        Makefile : Définit les règles de compilation pour l'ensemble des fichiers sources.
        Ultrasonic_Driver.c : Contient le code du driver RTDM pour le capteur ultrason.
        Buzzer_Driver.c : Contient le code du driver RTDM pour le buzzer.
        User_APP.c : Implémente l'application utilisateur qui gère l'ensemble du système.
    3. Xenomai/
        Documents pdf décrivent les étapes nécessaires pour installer et patcher le noyau Xenomai.

******************************************************************************************************************************************************
I/ Description du Projet:
**********************

Ce projet implémente un système de radar de recul fonctionnant en temps réel grâce au noyau temps réel Xenomai.
Le système inclut :

Un driver RTDM pour un capteur ultrasonique.
Un driver RTDM pour un buzzer.
Une application utilisateur pour orchestrer le fonctionnement global.

II/ Structure du Projet :
*********************
Le projet est organisé comme suit :
Projet/
├── Makefile              : Contient les règles pour compiler tous les composants du projet.
├── Ultrasonic_Driver.c   : Driver RTDM pour le capteur ultrason.
├── Buzzer_Driver.c       : Driver RTDM pour le buzzer.
├── User_APP.c            : Application utilisateur simulant le système complet.
        

III/ Pré-requis :
*********************
Avant de commencer, assurez-vous de :
Utiliser un noyau Linux avec Xenomai installé et configuré.
Astuce : Suivez les étapes du dossier Noyau Xenomai pour installer et configurer correctement Xenomai.
Installer les dépendances nécessaires à la compilation, notamment :
gcc, make, libxenomai-dev.
Étapes pour la Compilation et l'Exécution
    1. Compilation du Projet
    Naviguez vers le répertoire contenant le fichier Makefile et exécutez :
    >make
    Cela génère :

    Les modules noyau RTDM (ultrasound_rtdm.ko et buzzer_rtdm.ko).
    L'application utilisateur (user_app).

    2. Insertion des Modules dans le Noyau
        a. Charger le module pour le capteur ultrason :
         >sudo insmod ultrasound_rtdm.ko
        b. Charger le module pour le buzzer :
         > sudo insmod buzzer_rtdm.ko
        c. Vérifier que les modules sont chargés :
         > lsmod | grep rtdm
        d. Vérifier les journaux système :
         > dmesg | tail
    3. Exécution de l'Application Utilisateur
    Lancez l'application utilisateur :
         > sudo ./user_app


IV/ Points à Noter (IMPORTANT !!):
*********************
1. Chargement Préalable des Modules:
Avant d'exécuter l'application utilisateur, assurez-vous que les modules RTDM (ultrasound_rtdm.ko et buzzer_rtdm.ko) sont correctement compilés et chargés dans le noyau.

2. Vérification des Noms des Modules:
Attention : Après la compilation, le compilateur génère les fichiers du noyau avec l'extension (.ko). Ces fichiers contiennent les modules RTDM.
Vérifiez les noms exacts des fichiers générés par le compilateur pour les modules, par exemple :
==> Un fichier kernel pour le capteur ultrason : ultrasound_rtdm.ko.
==> Un fichier kernel pour le buzzer : buzzer_rtdm.ko.
Lors de l'insertion des modules, copiez exactement le nom généré par le compilateur pour éviter toute erreur.
Par exemple :
        > sudo insmod ultrasound_rtdm.ko
        > sudo insmod buzzer_rtdm.ko

3/Désinstallation des modules :
Pour décharger un module avant de le modifier ou de le recompiler :
    > sudo rmmod nom_du_module.ko
V/ Contact:
*********************
Email : N**********@gmail.com
******************************************************************************************************************************************************
                                                                          Fin
******************************************************************************************************************************************************
******************************************************************************************************************************************************