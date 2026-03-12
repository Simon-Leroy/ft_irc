## Arthur part -> transport reseau
Deja effectue :
-init_socket = creation socket serveur avec mise sur ecoute de cette derniere (bind)
-run = fonction permettant dajouter dynamiquement des clients et dafficher lorsque un join

A faire :
-Gerer autres flags de revent + faire attention a l'odre de traitement (eviter de sortir avant davoir execute certaines demandes)
-Commencer a implementer la fonction permettant de ne pas bloquer le flux (ne pas bloquer les fd)

Update 11/03 19h22:
-> l'envoie des msg nest pas recu pas le serveur, le bug provient du '\r' qui nest pas genere automatiquement sur mac (a tester sur autre environnement), donc parsing plus light en attendant avec uniquemnt "\n"


MERGE :

Server.hpp -> pesner a ajuster constructeur cote Simon / modif appel attribut _client cote Arthur + voir si on garde pointeur sur CLietn dans map
A FAIRE : chaucn met au propre ses fichiers puis call pour tester un main commun 

## PROCESS visé :
1. créer la socket serveur
2. attacher la socket au port
3. mettre la socket en mode écoute
4. ajouter la socket serveur dans poll(), -1 == att indefiniment
//-> poll surveille si un client veut se connecter
5. si la socket serveur devient lisible
//-> accept() une nouvelle connexion
//-> on obtient un nouveau clientFd
6. mettre le clientFd en non-blocking
7. ajouter le clientFd à poll()
//-> poll surveillera maintenant les messages du client
8. quand poll indique que le client est lisible
//-> recv() les données
9. stocker les données dans inputBuffer
//-> reconstruire les lignes terminées par \r\n
10. appeler la couche protocole IRC
handleLine(fd, line)
11. si une réponse doit être envoyée
-> ajouter dans outputBuffer
12. quand poll indique que la socket est writable
-> send() les données
13. si recv == 0 ou erreur
-> déconnecter le client

  
