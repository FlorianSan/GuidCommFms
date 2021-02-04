Ce dépot constitue un projet d'un groupe d'étudiant de l'ENAC.
Il a pour but de développer un FMS. Nous sommes le groupe Guid-COMM qui effectue l'interface entre le groupe Guid-SEQ et le Pilote Automatique.

Pour compiler : 

Tu vas installer tout ça:

Normalement toutes les distributions d’Ubuntu intègrent gcc et g++ mais au cas oû:
```console
sudo apt update

sudo apt install build-essential

sudo apt-get install xorg openbox

tu reponds par: 'o' quand on te pose une question 

sudo apt-get install libxt-dev

sudo apt-get install libpcre3-dev

sudo apt-get install tcl8.6-dev

sudo apt-get install libglib2.0-dev
```
Tu vas dans /ivy-c_3.15.1/src et tu ouvres le Makefile. Tu fais un ctrl+f pour chercher tcl8.4. Tu commentes les deux lignes suivante :

TCLINCL = -I/usr/include/tcl$(TCLVERS)

TCLLIB = - ltcl$(TCLVERS)

et tu de-commentes 

TCLINCL = -I/usr/include/tcl8.4

TCLLIB = - ltcl8.4

Puis tu remplaces le 4 par un 6, tu obtiens

#TCLINCL = -I/usr/include/tcl$(TCLVERS)

#TCLLIB = - ltcl$(TCLVERS)

TCLINCL = -I/usr/include/tcl8.6

TCLLIB = - ltcl8.6

Après en ligne commande tu vas dans /ivy-c_3.15.1/src

tu lances un :
```console
make 
```
puis un :
```console
sudo make install
```
Et maintenant tu peux compiler ton code

         Compilation

Il te faut modifier dans le hellotranslater.c l’adresse que tu veux pour ton réseau.

Ligne de code de compilation :
```console
gcc -o app app_guid_comm.c errors.c computing.c -I ivy-c_3.15.1/src -L ivy-c_3.15.1/src -livy -lpcre -lpthread -lm
```


        Test
        
Lancement
```console
./app -t 
```
Lancement avec redirection dans un fichier texte
```console
./app -t > terminal.txt 2>&1
```
Tu lances ivyprobe en ligne de commande:
```console
ivyprobe -b 10.3.141.120:2010 ‘(.*)’
```
Comme ça tu t’abonnes à tous les messages qui passent sur le bus, il faut que l’adresse soit la même que celle dans le fichier hellotranslater.c.

Après si tu tapes un ‘Hello de florian’ par exemple sur l’ivyprobe, ‘de florian’ doit s’afficher dans la console de ton application. 

Version de gcc 9.3.0

