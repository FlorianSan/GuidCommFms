Ce dépot constitue un projet d'un groupe d'étudiant de l'ENAC.
Il a pour but de développer un FMS. Nous sommes le groupe Guid-COMM qui effectue l'interface entre le groupe Guid-SEQ et le Pilote Automatique.

# Librairie
Toutes ces installations sont faites dans le install.sh du module FMS

Normalement toutes les distributions d’Ubuntu intègrent gcc et g++ mais au cas oû:
```console
sudo apt update

sudo apt install -y build-essential xorg openbox libxt-dev libpcre3-dev tcl8.6-dev libglib2.0-dev

```

Après en ligne commande tu vas dans /ivy-c_3.15.1/src

tu lances un :
```console
make 
```
puis un :
```console
sudo make install
```


# Compilation

Il te faut modifier dans le hellotranslater.c l’adresse que tu veux pour ton réseau.

Ligne de code de compilation :
```console
gcc -o app app_guid_comm.c errors.c computing.c -I ivy-c_3.15.1/src -L ivy-c_3.15.1/src -livy -lpcre -lpthread -lm
```

        
# Lancement
```console
./app -t 
```
Lancement avec redirection dans un fichier texte
```console
./app -t > terminal.txt 2>&1
```
Tu lances ivyprobe en ligne de commande:
```console
ivyprobe ‘(.*)’
```
Comme ça tu t’abonnes à tous les messages qui passent sur le bus

Version de gcc 9.3.0

Test modif Ienac17
