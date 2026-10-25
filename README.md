# Minitalk

> Un petit programme client / serveur qui fait transiter des messages texte entre deux processus Unix en utilisant uniquement les signaux `SIGUSR1` et `SIGUSR2`.

![Langage C](https://img.shields.io/badge/langage-C-blue)
![École 42](https://img.shields.io/badge/école-42-black)
![Unix](https://img.shields.io/badge/Unix-signaux-lightgrey)

## À propos

Minitalk est un projet de l'école 42 sur la communication inter-processus. Le serveur affiche son PID puis attend ; le client reçoit ce PID et une chaîne de caractères, et transmet la chaîne au serveur **bit par bit**, chaque bit étant codé par un signal : `SIGUSR1` pour 0, `SIGUSR2` pour 1. Le serveur reconstitue les octets et les affiche.

Aucun autre canal (pipe, socket, fichier, mémoire partagée) n'est utilisé.

## Fonctionnalités

- **Serveur** (`server`) : affiche son PID au démarrage, reçoit les messages de façon continue et peut servir plusieurs clients à la suite sans redémarrer.
- **Client** (`client`) : envoie une chaîne au serveur désigné par son PID, suivie d'un octet `\0` de fin de message.
- Détection du changement de client : si un signal arrive d'un PID différent, le serveur réinitialise son octet en cours de reconstitution.
- Message d'usage si le client n'est pas lancé avec exactement deux arguments.
- L'affichage utilise mon propre [`ft_printf`](https://github.com/MarkusLeGrand/ft_printf), intégré dans `printf_42/`.

## Compilation & utilisation

```bash
make        # compile les deux exécutables : server et client
make clean  # supprime les fichiers objets
make fclean # supprime les objets et les exécutables
make re     # recompile entièrement
```

Le Makefile compile avec `gcc -Wall -Wextra -Werror` et lie les sources de `ft_printf` directement dans chaque exécutable.

Dans un premier terminal :

```bash
$ ./server
Server PID: 4242
```

Dans un second terminal :

```bash
$ ./client 4242 "Bonjour 42 !"
```

Le message apparaît alors dans le terminal du serveur :

```
Server PID: 4242
Bonjour 42 !
```

## Approche technique

### Côté client : sérialisation bit par bit

Pour chaque caractère, `send_char` envoie ses 8 bits **du bit de poids faible au bit de poids fort** :

```c
for (int i = 0; i < 8; i++)
{
    if ((c >> i) & 1)
        kill(pid, SIGUSR2);   // bit à 1
    else
        kill(pid, SIGUSR1);   // bit à 0
    usleep(100);
}
```

Une pause de 100 µs sépare deux signaux pour laisser au serveur le temps de traiter le précédent. Après le dernier caractère, le client envoie un `\0`.

### Côté serveur : reconstitution avec `sigaction`

Le serveur installe un seul gestionnaire pour les deux signaux avec `sigaction` et le flag `SA_SIGINFO`, ce qui donne accès à la structure `siginfo_t` et donc au **PID de l'émetteur** (`info->si_pid`). Le gestionnaire conserve son état dans des variables statiques :

- `c` : l'octet en cours de construction ;
- `bit` : la position du prochain bit à écrire ;
- `client_pid` : le dernier émetteur connu.

À chaque signal, le bit reçu est placé à sa position (`c |= (sig == SIGUSR2) << bit`). Au 8e bit, l'octet complet est affiché, puis l'état est remis à zéro. Entre deux signaux, la boucle principale dort avec `pause()` : le serveur ne consomme pas de CPU en attente.

### Limites connues

- Le protocole est **sans accusé de réception** : le client ne fait qu'espacer ses envois avec `usleep(100)`. Les signaux standard ne sont pas mis en file d'attente par le noyau, donc sur une machine chargée ou très lente un bit peut être perdu et le texte reçu corrompu.
- Les octets sont transmis tels quels : des caractères UTF-8 multi-octets passent, mais rien n'est fait spécifiquement pour eux.

Ces deux points correspondent au bonus du sujet (accusé de réception serveur → client, support Unicode), qui n'est pas implémenté ici.

## Structure du projet

```
Minitalk/
├── Makefile
├── client.c          # parsing du PID, envoi bit par bit
├── serveur.c         # gestionnaire de signaux, reconstitution des octets
├── inc/
│   └── ft_printf.h
└── printf_42/        # sources de mon ft_printf
    ├── ft_printf.c
    ├── ft_handle_format.c
    ├── ft_handle_conversion.c
    └── ft_print_*.c, ft_utils.c
```

## Ce que j'ai appris

- Le fonctionnement des **signaux Unix** : envoi avec `kill`, réception avec `sigaction`, masque de signaux (`sigemptyset`), attente passive avec `pause`.
- Récupérer des informations sur l'émetteur d'un signal grâce à **`SA_SIGINFO`** et `siginfo_t`.
- Concevoir un **protocole de transmission minimal** : encodage binaire, ordre des bits, marqueur de fin de message.
- Les **contraintes de synchronisation** entre processus : les signaux ne s'empilent pas, d'où la nécessité d'un délai ou d'un accusé de réception.
- Gérer un état dans un gestionnaire de signal via des **variables statiques**.
