# Simulateur de réseau local (LAN) en C

Simulateur de réseau Ethernet écrit en **C**, qui modélise des **stations** et des **switches** reliés par des liens pondérés. Le réseau est décrit dans un fichier de configuration. La simulation avance par **ticks** et se pilote depuis un **terminal interactif** : on y envoie des trames, on suit leur trajet et on consulte les tables MAC des switches.

---

## Fonctionnalités

- **Chargement d'une topologie** depuis un fichier texte (stations, switches, liens et coûts).
- **Simulation à temps discret** : le temps avance par ticks, avec des points d'arrêt réglables.
- **Envoi de trames Ethernet** entre machines, avec le détail des couches physique et liaison.
- **Apprentissage des adresses MAC** : chaque switch remplit sa table de commutation au fil du trafic.
- **Visualisation** : affichage de la topologie, des trames (avec une légende en couleur) et des tables MAC.
- **Tests automatisés** lancés avec `make test`.

## Compétences mises en œuvre

| Domaine | Détails |
|---|---|
| Langage C | Gestion manuelle de la mémoire, structures de données, modularité |
| Réseaux | Ethernet (couche 2), adresses MAC, commutation, coûts des liens |
| Simulation | Boucle à temps discret (ticks), file de paquets, points d'arrêt |
| Outillage | Makefile (build, clean, tests), dossier `build/` séparé |
| Qualité | Tests automatisés dans `tests/` |

---

## Prérequis

- Un compilateur C (`gcc` ou `clang`)
- `make`
- Un système de type Unix (Linux, macOS ou WSL)

## Compilation

```bash
make
```

### Options du Makefile

| Commande | Effet |
|---|---|
| `make` / `make all` | Compile tout le projet (option par défaut). Crée le dossier `build/` s'il n'existe pas et copie l'exécutable `main` à la racine du projet pour qu'il soit facile à lancer. |
| `make clean` | Supprime seulement l'exécutable `main` à la racine du projet. |
| `make purge` | Supprime tout le dossier `build/`. |
| `make test` | Exécute tous les tests du dossier `tests/`. |

---

## Utilisation

Lancez le programme avec le **nom** d'un fichier de configuration. Inutile de donner le chemin : le programme cherche automatiquement dans `data/configs/`.

```bash
./main my_better_lan
```

Deux configurations sont fournies :

| Fichier | Description |
|---|---|
| `my_better_lan` | Réseau complet servant d'exemple principal |
| `my_lan_no_circle` | Réseau sans boucle (aucun chemin cyclique entre les machines) |

### Commandes interactives

Une fois le programme lancé, une invite `>>$` attend vos commandes :

| Commande | Description | Exemple |
|---|---|---|
| `help` | Affiche la liste des commandes | `help` |
| `display` | Affiche tous les nœuds et équipements du réseau | `display` |
| `send` | Envoie un paquet d'une machine vers une autre | `send 1 2 Message_hello` |
| `continue` | Reprend la simulation jusqu'au prochain point d'arrêt | `continue` |
| `bp` | Définit une nouvelle fréquence de point d'arrêt (en ticks) | `bp 1000` |
| `dtrame` | Affiche les informations sur les trames, avec une légende en couleur | `dtrame` |
| `mactable` | Affiche les entrées remplies de la table MAC de chaque switch | `mactable` |
| `exit` | Quitte le programme | `exit` |

### Exemple de session

```text
$ ./main my_better_lan
 Current ticks : 0

>>$ send 1 2 Message_hello
Queued packet successfully sent at tick 0:
 Ethernet(dest:01:45:23:A6:F7:03, src:01:45:23:A6:F7:02)
 Physical(from dev1 to dev0)

Queued packet successfully sent at tick 0:
 Ethernet(dest:01:45:23:A6:F7:03, src:01:45:23:A6:F7:02)
 Physical(from dev1 to dev3)

Queued packet successfully sent at tick 0:
 Ethernet(dest:01:45:23:A6:F7:03, src:01:45:23:A6:F7:02)
 Physical(from dev1 to dev4)

Queued packet successfully sent at tick 0:
 Ethernet(dest:01:45:23:A6:F7:03, src:01:45:23:A6:F7:02)
 Physical(from dev1 to dev7)

>>$ exit
Exiting program
```

Ici, la machine `dev1` envoie une trame vers la machine `2`. Elle est diffusée sur tous les liens voisins (`dev0`, `dev3`, `dev4`, `dev7`) et mise en file d'attente au tick 0.

---

## Format des fichiers de configuration

Les fichiers se trouvent dans `data/configs/`. Exemple :

```text
5 5
2;01:01:01:01:01:01;8;1024
2;02:02:02:02:02:02;8;512
2;03:03:03:03:03:03;8;256
1;AA:AA:AA:AA:AA:AA;10.0.0.1
1;BB:BB:BB:BB:BB:BB;10.0.0.2
0;1;4
0;2;19
1;2;4
0;3;0
2;4;0
```

### 1. En-tête (première ligne)

```text
<nombre d'équipements> <nombre de liens>
```

Dans l'exemple : **5** équipements et **5** liens.

### 2. Équipements (les N lignes suivantes)

Le premier champ indique le type d'équipement : `2` pour un switch, `1` pour une station.

| Type | Format | Exemple |
|---|---|---|
| Switch | `2;<adresse MAC>;<nombre de ports>;<priorité>` | `2;01:01:01:01:01:01;8;1024` |
| Station | `1;<adresse MAC>;<adresse IP>` | `1;AA:AA:AA:AA:AA:AA;10.0.0.1` |

Chaque équipement reçoit un **index** selon son ordre dans le fichier, à partir de 0. Dans l'exemple, les switches ont les index 0, 1 et 2, et les stations les index 3 et 4.

### 3. Liens (les M dernières lignes)

```text
<index machine 1>;<index machine 2>;<coût du lien>
```

Par exemple, `0;1;4` relie l'équipement 0 à l'équipement 1 avec un coût de 4.

Topologie décrite par l'exemple :

```text
        [SW0]
       /  |  \
   (4)/   |(19)\(0)
     /    |     \
 [SW1]-(4)-[SW2] [ST3]
            |
           (0)
            |
          [ST4]
```

---

## Structure du projet

```text
.
├── data/
│   └── configs/        # Fichiers de configuration des réseaux
├── tests/              # Tests automatisés (make test)
├── build/              # Fichiers de compilation (généré)
├── Makefile
├── main                # Exécutable (généré par make)
└── README.md
```

---
