# ft_irc

*Projet réalisé dans le cadre du cursus 42 par msingh et hnayel.*

Un serveur **IRC** (Internet Relay Chat) écrit en **C++98**. L'objectif est de comprendre le fonctionnement bas niveau d'un serveur réseau : gérer plusieurs clients simultanés avec des entrées-sorties non bloquantes et un seul `poll()`, analyser le protocole IRC et implémenter ses commandes principales (authentification, canaux, messages, modération).

Le serveur accepte les vrais clients IRC (irssi, WeeChat, HexChat…) comme les connexions brutes avec `nc`.

## Compilation et lancement

```bash
make                         # produit ./ircserv  (-Wall -Wextra -Werror -std=c++98)
./ircserv <port> <password>
./ircserv 6667 secret
```

Connexion avec irssi :

```
/connect -network myirc localhost 6667 secret
```

Connexion brute avec `nc` (les lignes se terminent par `\r\n` ; `nc -C` les ajoute) :

```bash
nc -C localhost 6667
PASS secret
NICK alice
USER alice 0 * :Alice
JOIN #42
PRIVMSG #42 :salut tout le monde
```

## Commandes gérées

| Catégorie | Commandes |
|---|---|
| Enregistrement | `PASS`, `NICK`, `USER`, `CAP` (réponse vide à `CAP LS`) |
| Connexion | `PING`, `QUIT` |
| Canaux | `JOIN` (avec clé), `PART`, `NAMES`, `TOPIC` |
| Messages | `PRIVMSG`, `NOTICE` (vers un utilisateur ou un canal) |
| Opérateurs | `KICK`, `INVITE`, `MODE` |

Modes de canal : `i` (sur invitation), `t` (sujet modifiable par les opérateurs seulement), `k` (mot de passe), `o` (donner ou retirer le statut d'opérateur), `l` (limite d'utilisateurs).

```
MODE #test +i              invitation obligatoire
MODE #test +k secret       mot de passe du canal
MODE #test +l 3            3 utilisateurs maximum
MODE #test +o bob          bob devient opérateur
MODE #test +itkl secret 5  plusieurs modes en une commande
```

Les changements appliqués sont renvoyés groupés sous un seul signe (`+itkl secret 5`, ou `+it-k+l 9` quand le signe change).

## Architecture

| Fichier | Rôle |
|---|---|
| `srcs/main.cpp` | vérifie les arguments et lance le serveur |
| `server/ServerInit.cpp` | socket d'écoute : `socket`, `setsockopt(SO_REUSEADDR)`, `bind`, `listen`, mode non bloquant, `SIGPIPE` ignoré |
| `server/ServerLoop.cpp` | **boucle principale** : un seul `poll()` sur la socket d'écoute et toutes les sockets clientes |
| `server/ServerIO.cpp` | `recv` et `send`, avec un buffer par client |
| `server/ServerClients.cpp` | acceptation, déconnexion et nettoyage des clients |
| `protocol/Parser.cpp` | découpe une ligne IRC en préfixe, commande, paramètres et paramètre final (après `:`) |
| `protocol/CommandHandler.cpp` | aiguillage vers le bon handler ; refuse les commandes de canal tant que le client n'est pas enregistré (`451`) |
| `protocol/Replies.cpp` | construction des réponses numériques (`001`, `353`, `433`, `461`…) |
| `commands/registration/` | `PASS`, `NICK`, `USER` |
| `commands/channel/` | `JOIN`, `PART`, `PRIVMSG`, `NOTICE`, `NAMES`, `TOPIC`, `KICK`, `INVITE`, `MODE` |
| `client/Client.cpp`, `channel/Channel.cpp` | état d'un client (pseudo, buffers, enregistrement) et d'un canal (membres, opérateurs, invités, modes) |
| `bot.cpp` | bonus : bot de jeu « juste prix » sur les canaux |

### Points techniques

- **Non bloquant de bout en bout** : toutes les sockets sont en `O_NONBLOCK` et chaque `recv`/`send` ne se fait qu'après un événement signalé par `poll()`. Pas de thread ni de `fork`.
- **Messages fragmentés** : les données reçues s'accumulent dans le buffer du client. Seules les lignes complètes (terminées par `\r\n` ou `\n`) sont traitées, ce qui gère à la fois une commande envoyée en plusieurs morceaux (test `nc` + `Ctrl-D`) et plusieurs commandes reçues dans un seul `recv`.
- **Notifications sans doublon** : quand un client change de pseudo ou quitte, chaque autre client concerné reçoit un seul message, même s'il partage plusieurs canaux avec lui.
- `NOTICE` se comporte comme `PRIVMSG` mais ne renvoie jamais d'erreur automatique à l'émetteur.

## Bonus

- **Bot** : dans un canal, `!start` lance une partie de « juste prix » (nombre entre 1 et 100). Le bot répond « + haut » ou « + bas » à chaque proposition, puis félicite le gagnant.
- **Transfert de fichiers DCC** entre clients (avec irssi) :

```
/dcc send <pseudo> <chemin>          envoyer un fichier
/set dcc_download_path ~/Downloads   dossier de réception
/dcc get <pseudo>                    accepter le fichier
```

## Tests

Scénarios testés avec plusieurs clients `nc` : enregistrement correct et mot de passe faux (`464`), pseudo déjà pris (`433`), destinataire inconnu (`401`), commande inconnue (`421`), `JOIN`/`PART` sur plusieurs canaux, `TOPIC`, `MODE +k`, `KICK` avec diffusion aux membres, `PING`/`PONG`, commande envoyée en plusieurs fragments.

## Ressources

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/)
- [poll(2)](https://man7.org/linux/man-pages/man2/poll.2.html), [fcntl(2)](https://man7.org/linux/man-pages/man2/fcntl.2.html)

### Usage de l'IA

Un assistant IA a été utilisé pendant la phase de débogage et de consolidation d'une implémentation qui fonctionnait déjà, et non pour la conception initiale ni pour écrire le projet de zéro. Il a servi à relire l'implémentation par rapport au sujet, à diagnostiquer des problèmes de protocole, d'aiguillage des commandes et de Makefile, et à lancer les tests (unitaires, d'intégration, AddressSanitizer). Chaque modification a été relue manuellement, et le projet a été recompilé avec `-Wall -Wextra -Werror -std=c++98` après chaque changement.
