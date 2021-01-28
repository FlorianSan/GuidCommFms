About Ivy
=========

Ivy is a software bus designed at the French [Centre d'Etudes de la Navigation Aérienne](http://www.cena.fr/) (CENA) and developed since 1996.

The official homepage of the Ivy library can be found on its [dedicated CENA page](http://www.eei.cena.fr/products/ivy/about.shtml). This is also where the official (upstream) [SVN repository](https://svn.tls.cena.fr/wsvn/ivy/?sc=0) can be found.

A software bus
--------------

A software bus is a system that allows software applications to exchange information with the illusion of broadcasting that information, selection being performed by the receiving applications based on the contents of the messages. Other denominations are sometimes "publish-subscribe notification services" or "message-oriented middleware". Software buses federate pieces of software written in different programming languages on different platforms.

for more information, [see this example](http://www.eei.cena.fr/products/ivy/example.shtml)

The main features of Ivy:
-------------------------

* Ivy is not based on a centralised server. Actually, Ivy is mostly a communication convention, implemented through a collection of libraries for various languages and platforms. The current version of the Ivy protocol is version 3, which has been stable for the last 3 years.
* Language bindings are available in C (Unix and Windows), C++ (Mac, Unix, Windows), Java and Perl. There have been successful uses through the C library
* Messages are formatted in text, and subscriptions are based on regular expressions. Plans to move to an XML-based subscription language are on their way.
* From the programmer's point of view, Ivy is an information broadcasting channel. The main functions are:
 * connecting to a bus. Example: `IvyInit (b, "192.126:2011")`
 * sending a message. Example: `IvySend (b, "HELLO %s", world)`
 * binding a message pattern to a callback function. Example: `IvyBind (b, "HELLO (.*)", cb)`
 * the main loop. Example : `IvyLoop ()`
* Subscriptions are managed on the emitter's side, which limits the actual network traffic.
* Direct point-to-point messages are also available.

Ivy was designed by a research group in Human-Computer Interaction, with the goals of connecting applications written on different toolkits/languages/platforms (such as an OpenGL application on a SGI connected to a PerlTk application on a Linux box), while keeping it simple: no server to be lauched and supervised, a simplistic API, and a communication model compatible with classical event-based GUI progamming. We think we have somewhat reached our goal... 
