
# 🛠️ Fox32 File Recovery 



Un outil robuste pour récupérer des fichiers supprimés par erreur sur un disque formaté en FAT32.  
Oui, même ce fichier `.c` que tu pensais avoir perdu à jamais après un `rm` trop violent un soir de fatigue.

## Description

Cet outil permet de :

- Lire les informations du système de fichiers FAT32 (secteurs, clusters, etc.)
- Lister les fichiers dans le répertoire racine
- Récupérer des fichiers contigus (allocations continues sur le disque)
- Récupérer des fichiers **non contigus** via une recherche intelligente à l’aide de **SHA-1**
- Faire tout ça proprement, sans magie noire (ou presque)

> FAT32, c’est old school. Mais dans le monde embarqué, il est toujours roi.  
> Ce projet, c’est donc un peu comme un archéologue digital : on explore des fossiles binaires.

---

##  Commandes disponibles

```bash
[root@...]# ./nyufile
Usage: ./nyufile disk <options>
  -i                     Print the file system information.
  -l                     List the root directory.
  -r filename [-s sha1]  Recover a contiguous file.
  -R filename -s sha1    Recover a possibly non-contiguous file.
```

### Options détaillées :

| Option | Description |
|--------|-------------|
| `-i` | Affiche les infos de base du système de fichiers (FATs, taille des secteurs, etc.) |
| `-l` | Liste les fichiers et répertoires dans la racine du disque |
| `-r filename` | Tente de récupérer un fichier contigu supprimé |
| `-r filename -s sha1` | Tente de récupérer un fichier contigu avec vérification SHA-1 |
| `-R filename -s sha1` | Recherche et reconstruit un fichier **non contigu** via SHA-1 |

---

## Fonctionnement (vue d'ensemble technique)

### 📁 Lecture des fichiers
Le programme analyse le secteur de boot (`BootEntry`) et accède au répertoire racine via les structures FAT. Les fichiers supprimés sont identifiés par le marqueur `0xE5`.

### SHA-1 et récupération intelligente
Pour éviter de restaurer un mauvais fichier (ou plusieurs fichiers nommés pareil), on compare les **SHA-1** des blocs de données pour confirmer l'identité du fichier avant la récupération.

- Si le fichier est contigu : facile.
- S’il est fragmenté : on devient ninja 🥷 avec une recherche récursive pour reconstruire l’ordre des clusters.

> On utilise l’empreinte SHA-1 comme ADN digital du fichier.

---

## Architecture du code

Le code est divisé en plusieurs fichiers pour plus de clarté :

| Fichier | Rôle |
|--------|------|
| `fsinfo.c` | Contient la logique de parsing FAT32, listing, et récupération |
| `fsinfo.h` | Définition des structures FAT32 (`BootEntry`, `DirEntry`) |
| `global.h` | Fichier d’en-têtes partagé entre les modules (non montré ici) |
| `nyufile.c` | Fichier principal : parse les options CLI et appelle les fonctions |

---

## Exemple d'utilisation

```bash
# Afficher les infos du disque
./nyufile /dev/sdb1 -i

# Lister le contenu du répertoire racine
./nyufile /dev/sdb1 -l

# Récupérer un fichier contigu supprimé
./nyufile /dev/sdb1 -r secret.txt

# Récupérer un fichier contigu avec SHA-1
./nyufile /dev/sdb1 -r secret.txt -s 8d777f385d3dfec8815d20f7496026dc9c9d8a8f

# Récupérer un fichier non contigu (via une quête SHA-1)
./nyufile /dev/sdb1 -R hacknote.pdf -s 1f8ac10f23c5b5bc1167bda84b833e5c057a77d2
```

---

## Pré-requis

- Système Linux
- Compilation avec GCC
- Accès root pour monter les disques bruts (`/dev/sdX`)
- Et une petite dose de bon sens (non inclus dans le repo malheureusement)

---

## Sécurité et bonnes pratiques

> ⚠️ **Important :** Ce genre d’outil opère bas niveau. Il faut éviter d’écrire sur le disque contenant les fichiers à récupérer.

Recommandé :

- Travailler sur une **copie du disque** (image `.img`)
- Monter l’image en lecture seule
- Ne jamais lancer sur un disque de travail sans backup

---

##  Bonus: Pourquoi ce projet existe

Ce projet a été développé dans un cadre académique sérieux... mais on ne va pas se mentir :  
C’est aussi un kiff de geek de voir un fichier “revenu d’entre les morts”.

---

## Auteur

**Fox / Donfack Pascal Arthur**  
École Nationale Supérieure Polytechnique de Yaoundé  
Contact : *à compléter*

---

## Licence

MIT License – Fais ce que tu veux avec, mais n’oublie pas de mentionner le dev si tu gagnes un prix Nobel avec.

---

## À venir (potentiel)

- Récupération multi-fichiers
- Interface graphique (genre un mini FTK en C, qui sait)
- Support d'autres systèmes de fichiers (exFAT, NTFS, ext4... mais une guerre à la fois)

---

*Ce projet sent le fer à souder, les octets cramés et la sueur . Bref, c’est du solide.*


---
