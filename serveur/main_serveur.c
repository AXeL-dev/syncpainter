/* Application serveur */
/* Crée le  : 25/09/2013 */
/* Date de la dernière modification : 28/09/2013 */
/* Commentaire : application client/serveur qui permet de dessiner sur l'ecran du serveur + possiblité de synchroniser avec le client et voir ce qu'on dessine sur le serveur en directe */
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <windows.h> /* Pour MessageBox(); */
#include "const_serveur.h"
#include "socket.h"

/* Fonction Principale */
int main (int argc, char *argv[])
{
/* SOCKET Initialisation */
#if defined (WIN32)
   WSADATA WSAData;
   int erreur = WSAStartup(MAKEWORD(2,2), &WSAData);
#else
   int erreur = 0;
#endif
SOCKET sock;
SOCKADDR_IN sin;
SOCKET csock;
SOCKADDR_IN csin;
socklen_t recsize = sizeof(csin);
int sock_err;
char buffer[SOCKET_TAILLE]=""; /* chaine qui va contenir les données à envoyer */
/* SDL Declaration */
SDL_Surface *ecran=NULL, *arriere_plan=NULL, *objet_paint[N_COULEUR][N_OBJET]={NULL}, *outil_gomme=NULL, *sync_button[2]={NULL}, *effacer_button[2]={NULL}, *objet_selectionner[4]={NULL}, *objet_nn_selectionner[4]={NULL}, *couleur_selectionner[N_COULEUR]={NULL}, *couleur_nn_selectionner[N_COULEUR]={NULL};
SDL_Rect pos_arriere_plan, pos_objet_paint, pos_sync_button, pos_effacer_button, pos_o_petit, pos_o_moyen, pos_o_grand, pos_o_gomme, pos_c_noir, pos_c_rouge, pos_c_bleu, pos_c_vert, pos_c_jaune, pos_c_violet;
SDL_Event event;
int continuer=1, i=0, j=0, clic_s_button=0, clic_e_button=0, sourie_g=0, blit_objet=0, r_ecran=1, objet=PETIT, couleur=NOIR, synchroniser=0;
/* SDL Initialisation */
SDL_Init(SDL_INIT_VIDEO);
/* Icone de l'application */
SDL_WM_SetIcon(IMG_Load("ress\\pserveur.png"), NULL);
/* Initialisation de l'ecran (taille de la fenetre etc) */
ecran=SDL_SetVideoMode(LARGEUR, HAUTEUR, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
/* Nom de l'application */
SDL_WM_SetCaption("SyncPainter serveur", NULL);
/* Images et positionnement */
   /* Images (arriere plan + buttons) */
   arriere_plan=IMG_Load("ress\\pespace.jpg");
   sync_button[0]=IMG_Load("ress\\sybutton.jpg");
   sync_button[1]=IMG_Load("ress\\sgbutton.jpg");
   effacer_button[0]=IMG_Load("ress\\eybutton.jpg");
   effacer_button[1]=IMG_Load("ress\\ebbutton.jpg");
   /* Images Objets selon la couleur : */
   /* NOIR */
   objet_paint[NOIR][PETIT]=IMG_Load("ress\\obj\\npobjet1.png");
   objet_paint[NOIR][MOYEN]=IMG_Load("ress\\obj\\npobjet2.png");
   objet_paint[NOIR][GRAND]=IMG_Load("ress\\obj\\npobjet3.png");
   /* ROUGE */
   objet_paint[ROUGE][PETIT]=IMG_Load("ress\\obj\\rpobjet1.png");
   objet_paint[ROUGE][MOYEN]=IMG_Load("ress\\obj\\rpobjet2.png");
   objet_paint[ROUGE][GRAND]=IMG_Load("ress\\obj\\rpobjet3.png");
   /* BLEU */
   objet_paint[BLEU][PETIT]=IMG_Load("ress\\obj\\bpobjet1.png");
   objet_paint[BLEU][MOYEN]=IMG_Load("ress\\obj\\bpobjet2.png");
   objet_paint[BLEU][GRAND]=IMG_Load("ress\\obj\\bpobjet3.png");
   /* VERT */
   objet_paint[VERT][PETIT]=IMG_Load("ress\\obj\\verpobjet1.png");
   objet_paint[VERT][MOYEN]=IMG_Load("ress\\obj\\verpobjet2.png");
   objet_paint[VERT][GRAND]=IMG_Load("ress\\obj\\verpobjet3.png");
   /* JAUNE */
   objet_paint[JAUNE][PETIT]=IMG_Load("ress\\obj\\jpobjet1.png");
   objet_paint[JAUNE][MOYEN]=IMG_Load("ress\\obj\\jpobjet2.png");
   objet_paint[JAUNE][GRAND]=IMG_Load("ress\\obj\\jpobjet3.png");
   /* VIOLET */
   objet_paint[VIOLET][PETIT]=IMG_Load("ress\\obj\\viopobjet1.png");
   objet_paint[VIOLET][MOYEN]=IMG_Load("ress\\obj\\viopobjet2.png");
   objet_paint[VIOLET][GRAND]=IMG_Load("ress\\obj\\viopobjet3.png");
   /* Gomme */
   outil_gomme=IMG_Load("ress\\gomme.png");
   /* Objet selectionner/nn_selectionner */
   objet_selectionner[PETIT]=IMG_Load("ress\\sobjet1.png");
   objet_selectionner[MOYEN]=IMG_Load("ress\\sobjet2.png");
   objet_selectionner[GRAND]=IMG_Load("ress\\sobjet3.png");
   objet_selectionner[GOMME]=IMG_Load("ress\\sobjet4.png");
   objet_nn_selectionner[PETIT]=IMG_Load("ress\\nsobjet1.png");
   objet_nn_selectionner[MOYEN]=IMG_Load("ress\\nsobjet2.png");
   objet_nn_selectionner[GRAND]=IMG_Load("ress\\nsobjet3.png");
   objet_nn_selectionner[GOMME]=IMG_Load("ress\\nsobjet4.png");
   /* Couleur selectionner/nn_selectionner */
   couleur_selectionner[NOIR]=IMG_Load("ress\\csnoir.png");
   couleur_selectionner[ROUGE]=IMG_Load("ress\\csrouge.png");
   couleur_selectionner[BLEU]=IMG_Load("ress\\csbleu.png");
   couleur_selectionner[VERT]=IMG_Load("ress\\csvert.png");
   couleur_selectionner[JAUNE]=IMG_Load("ress\\csjaune.png");
   couleur_selectionner[VIOLET]=IMG_Load("ress\\csviolet.png");
   couleur_nn_selectionner[NOIR]=IMG_Load("ress\\cnsnoir.png");
   couleur_nn_selectionner[ROUGE]=IMG_Load("ress\\cnsrouge.png");
   couleur_nn_selectionner[BLEU]=IMG_Load("ress\\cnsbleu.png");
   couleur_nn_selectionner[VERT]=IMG_Load("ress\\cnsvert.png");
   couleur_nn_selectionner[JAUNE]=IMG_Load("ress\\cnsjaune.png");
   couleur_nn_selectionner[VIOLET]=IMG_Load("ress\\cnsviolet.png");
/* Transparence des images d'objets */
for (j=0; j<N_COULEUR; j++)
{
   for (i=0; i<N_OBJET; i++)
      SDL_SetColorKey(objet_paint[j][i], SDL_SRCCOLORKEY, SDL_MapRGB(objet_paint[j][i]->format, 0, 0, 255)); /* On enleve la couleur bleu pure de ces images */
}
   /* Positionnement */
      /* arriere plan */
      pos_arriere_plan.x=0;
      pos_arriere_plan.y=0;
      /* sync_button */
      pos_sync_button.x=19;
      pos_sync_button.y=426;
      /* effacer_button */
      pos_effacer_button.x=19;
      pos_effacer_button.y=350;
      /* objet_selectionner/nn_selectionner */
      pos_o_petit.x=19;
      pos_o_petit.y=47;
      pos_o_moyen.x=59;
      pos_o_moyen.y=47;
      pos_o_grand.x=100;
      pos_o_grand.y=47;
      pos_o_gomme.x=19;
      pos_o_gomme.y=101;
      /* couleur_selectionner/nn_selectionner */
      pos_c_noir.x=19;
      pos_c_noir.y=210;
      pos_c_rouge.x=59;
      pos_c_rouge.y=210;
      pos_c_bleu.x=100;
      pos_c_bleu.y=210;
      pos_c_vert.x=19;
      pos_c_vert.y=264;
      pos_c_jaune.x=59;
      pos_c_jaune.y=264;
      pos_c_violet.x=100;
      pos_c_violet.y=264;
/* Placement de la sourie au milieu de l'ecran */
SDL_WarpMouse(ecran->w/2, ecran->h/2);
/* Gestion d'évenements */
while(continuer)
{
SDL_WaitEvent(&event);
   switch(event.type)
   {
   /* Fermeture de l'application */
   case SDL_QUIT: continuer=0; break;
   /* Evénements clic sourie */
   case SDL_MOUSEBUTTONDOWN: /* button sourie enfoncé */
        switch(event.button.button)
        {
           case SDL_BUTTON_LEFT: /* clic sourie gauche */
                if (event.button.x>=19 && event.button.x<=141 && event.button.y>=426 && event.button.y<=480)
                {/* Boutton Syncroniser */
                clic_s_button=1;
                /* Synchronisation */
                if (!synchroniser)
                {
                   /* Si les sockets Windows fonctionnent */
                   if(!erreur)
                   {
                      /* Ouverture de la socket en mode TCP/IP */        
                      sock = socket(AF_INET, SOCK_STREAM, 0);
  
                     /* Si la socket est valide */
                     if(sock != INVALID_SOCKET)
                     {
                             
                        /* Configuration */
                        sin.sin_addr.s_addr    = htonl(INADDR_ANY);   /* Adresse IP automatique */
                        sin.sin_family         = AF_INET;             /* Protocole familial (IP) */
                        sin.sin_port           = htons(PORT);         /* Listage du port */
                        sock_err = bind(sock, (SOCKADDR*)&sin, sizeof(sin));
  
                        /* Si la socket fonctionne */
                        if(sock_err != SOCKET_ERROR)
                        {
                           /* Démarrage du listage (mode server) */
                           sock_err = listen(sock, 5);
  
                           /* Si la socket fonctionne */
                           if(sock_err != SOCKET_ERROR)
                           {
                              /* Attente pendant laquelle le client se connecte */
                              MessageBox(NULL, "     Attente de connexion du client...\nVeuillez lancer le client et cliquer sur ok !", "SyncPainter serveur", MB_OK | MB_ICONINFORMATION);
                              csock = accept(sock, (SOCKADDR*)&csin, &recsize);
                              /* Dès que le client se connecte */
                              MessageBox(NULL, "      Synchronisé !\nl'ecran va etre effacé !", "SyncPainter serveur", MB_OK | MB_ICONINFORMATION);
                              synchroniser=1;
                              r_ecran=1;
                           }
                        }
                     }
                  }
                  else /* Si les sockets Windows ne fonctionnent pas */
                     MessageBox(NULL, "   Erreur !", "SyncPainter serveur", MB_OK | MB_ICONINFORMATION);
                } /* sortie du if(!synchroniser) */
                else /* Si nn si déjà synchroniser */
                   MessageBox(NULL, "   Déjà synchronisé !", "SyncPainter serveur", MB_OK | MB_ICONINFORMATION);
                }
                else if (event.button.x>=19 && event.button.x<=141 && event.button.y>=350 && event.button.y<=404)
                {/* Boutton Effacer */
                r_ecran=1;
                clic_e_button=1;
                }
                else if (event.button.x>=XMAX && event.button.x<=XMIN && event.button.y>=YMAX && event.button.y<=YMIN)
                {/* espace_paint */
                sourie_g=1;
                blit_objet=1;
                }
                else if (event.button.x>=pos_o_petit.x && event.button.x<=pos_o_petit.x+38 && event.button.y>=pos_o_petit.y && event.button.y<=pos_o_petit.y+50)
                {/* objet_selectionner[PETIT] */
                objet=PETIT;
                }
                else if (event.button.x>=pos_o_moyen.x && event.button.x<=pos_o_moyen.x+38 && event.button.y>=pos_o_moyen.y && event.button.y<=pos_o_moyen.y+50)
                {/* objet_selectionner[MOYEN] */
                objet=MOYEN;
                }
                else if (event.button.x>=pos_o_grand.x && event.button.x<=pos_o_grand.x+38 && event.button.y>=pos_o_grand.y && event.button.y<=pos_o_grand.y+50)
                {/* objet_selectionner[GRAND] */
                objet=GRAND;
                }
                else if (event.button.x>=pos_o_gomme.x && event.button.x<=pos_o_gomme.x+123 && event.button.y>=pos_o_gomme.y && event.button.y<=pos_o_gomme.y+50)
                {/* objet_selectionner[GOMME] */
                objet=GOMME;
                }
                else if (event.button.x>=pos_c_noir.x && event.button.x<=pos_c_noir.x+38 && event.button.y>=pos_c_noir.y && event.button.y<=pos_c_noir.y+50)
                {/* couleur_selectionner[NOIR] */
                couleur=NOIR;
                }
                else if (event.button.x>=pos_c_rouge.x && event.button.x<=pos_c_rouge.x+38 && event.button.y>=pos_c_rouge.y && event.button.y<=pos_c_rouge.y+50)
                {/* couleur_selectionner[ROUGE] */
                couleur=ROUGE;
                }
                else if (event.button.x>=pos_c_bleu.x && event.button.x<=pos_c_bleu.x+38 && event.button.y>=pos_c_bleu.y && event.button.y<=pos_c_bleu.y+50)
                {/* couleur_selectionner[BLEU] */
                couleur=BLEU;
                }
                else if (event.button.x>=pos_c_vert.x && event.button.x<=pos_c_vert.x+38 && event.button.y>=pos_c_vert.y && event.button.y<=pos_c_vert.y+50)
                {/* couleur_selectionner[VERT] */
                couleur=VERT;
                }
                else if (event.button.x>=pos_c_jaune.x && event.button.x<=pos_c_jaune.x+38 && event.button.y>=pos_c_jaune.y && event.button.y<=pos_c_jaune.y+50)
                {/* couleur_selectionner[JAUNE] */
                couleur=JAUNE;
                }
                else if (event.button.x>=pos_c_violet.x && event.button.x<=pos_c_violet.x+38 && event.button.y>=pos_c_violet.y && event.button.y<=pos_c_violet.y+50)
                {/* couleur_selectionner[VIOLET] */
                couleur=VIOLET;
                }
           break; /* sortie de SDL_BUTTON_LEFT */
        } 
   break; /* sortie de SDL_MOUSEBUTTONDOWN */
   case SDL_MOUSEBUTTONUP: if (sourie_g) sourie_g=0; break; /* Si button sourie relaché */
   /* Evénements motivation sourie */
   case SDL_MOUSEMOTION:
        /* Récupération des positions de la sourie pour blit l'objet */
        pos_objet_paint.x=event.motion.x;
        pos_objet_paint.y=event.motion.y;
        /* Si on peut afficher l'objet */
        if (sourie_g && event.motion.x>=XMAX && event.motion.x<=XMIN && event.motion.y>=YMAX && event.motion.y<=YMIN)
           blit_objet=1;   
   break; /* sortie de SDL_MOUSEMOTION */
   }
/* Envoie des données au client si synchronisation réussi */
if (synchroniser)
{
   if (blit_objet || r_ecran)
      sprintf(buffer, "%d - %d - %d - %d - %d", r_ecran, objet, couleur, pos_objet_paint.x, pos_objet_paint.y);
   else
      sprintf(buffer, "SYNC VERIF"); /* Vérification de l'état de la synchronisation */
   /* Envoie de la socket */
   sock_err = send(csock, buffer, SOCKET_TAILLE, 0);
   
   if(sock_err == SOCKET_ERROR) /* Si socket nn envoyée */
   {
      SYSTEMTIME Time;
      GetSystemTime(&Time); /* Récupération du temp */
      fprintf(stderr, "%d/%d/%d - %d:%d : Erreur de transmission !\n", Time.wDay, Time.wMonth, Time.wYear, Time.wHour, Time.wMinute);
      MessageBox(NULL, "   Vous n'etes plus synchronisé !", "SyncPainter serveur", MB_OK | MB_ICONINFORMATION);
      if (sourie_g) sourie_g=0; /* Pour eviter la répétition du blit apres la désynchronisation du client */
      synchroniser=0;
      /* Fermeture de la connexion */
      shutdown(csock, 2);
      /* Fermeture de la socket */
      closesocket(sock);
   }
}
/* Blit */
   /* Arriere plan */
   if (r_ecran) /* Si on peut rénitialiser/effacer l'ecran */
   {
      SDL_BlitSurface(arriere_plan, NULL, ecran, &pos_arriere_plan);
      r_ecran=0;
   }
   /* Objet_selectionner/nn_selectionner */
   if (objet==PETIT)
      SDL_BlitSurface(objet_selectionner[PETIT], NULL, ecran, &pos_o_petit);
   else
      SDL_BlitSurface(objet_nn_selectionner[PETIT], NULL, ecran, &pos_o_petit);
   if (objet==MOYEN)
      SDL_BlitSurface(objet_selectionner[MOYEN], NULL, ecran, &pos_o_moyen);
   else
      SDL_BlitSurface(objet_nn_selectionner[MOYEN], NULL, ecran, &pos_o_moyen);
   if (objet==GRAND)
      SDL_BlitSurface(objet_selectionner[GRAND], NULL, ecran, &pos_o_grand);
   else
      SDL_BlitSurface(objet_nn_selectionner[GRAND], NULL, ecran, &pos_o_grand);
   if (objet==GOMME)
      SDL_BlitSurface(objet_selectionner[GOMME], NULL, ecran, &pos_o_gomme);
   else
      SDL_BlitSurface(objet_nn_selectionner[GOMME], NULL, ecran, &pos_o_gomme);
   /* Couleur_selectionner/nn_selectionner */
   if (couleur==NOIR)
      SDL_BlitSurface(couleur_selectionner[NOIR], NULL, ecran, &pos_c_noir);
   else
      SDL_BlitSurface(couleur_nn_selectionner[NOIR], NULL, ecran, &pos_c_noir);
   if (couleur==ROUGE)
      SDL_BlitSurface(couleur_selectionner[ROUGE], NULL, ecran, &pos_c_rouge);
   else
      SDL_BlitSurface(couleur_nn_selectionner[ROUGE], NULL, ecran, &pos_c_rouge);
   if (couleur==BLEU)
      SDL_BlitSurface(couleur_selectionner[BLEU], NULL, ecran, &pos_c_bleu);
   else
      SDL_BlitSurface(couleur_nn_selectionner[BLEU], NULL, ecran, &pos_c_bleu);
   if (couleur==VERT)
      SDL_BlitSurface(couleur_selectionner[VERT], NULL, ecran, &pos_c_vert);
   else
      SDL_BlitSurface(couleur_nn_selectionner[VERT], NULL, ecran, &pos_c_vert);
   if (couleur==JAUNE)
      SDL_BlitSurface(couleur_selectionner[JAUNE], NULL, ecran, &pos_c_jaune);
   else
      SDL_BlitSurface(couleur_nn_selectionner[JAUNE], NULL, ecran, &pos_c_jaune);
   if (couleur==VIOLET)
      SDL_BlitSurface(couleur_selectionner[VIOLET], NULL, ecran, &pos_c_violet);
   else
      SDL_BlitSurface(couleur_nn_selectionner[VIOLET], NULL, ecran, &pos_c_violet);
   /* Button Syncroniser */
   if (clic_s_button)
   {
      SDL_BlitSurface(sync_button[0], NULL, ecran, &pos_sync_button);
      clic_s_button=0;
   }
   else
      SDL_BlitSurface(sync_button[1], NULL, ecran, &pos_sync_button);
   /* Button Effacer */
   if (clic_e_button)
   {
      SDL_BlitSurface(effacer_button[0], NULL, ecran, &pos_effacer_button);
      clic_e_button=0;
   }
   else
      SDL_BlitSurface(effacer_button[1], NULL, ecran, &pos_effacer_button);
   /* Objet_paint (Dessinage) */
   if (blit_objet)
   {
      if (objet==GOMME)
         SDL_BlitSurface(outil_gomme, NULL, ecran, &pos_objet_paint);
      if (couleur==NOIR)
      {
         if (objet==PETIT)
            SDL_BlitSurface(objet_paint[NOIR][PETIT], NULL, ecran, &pos_objet_paint);
         else if (objet==MOYEN)
            SDL_BlitSurface(objet_paint[NOIR][MOYEN], NULL, ecran, &pos_objet_paint);
         else if (objet==GRAND)
            SDL_BlitSurface(objet_paint[NOIR][GRAND], NULL, ecran, &pos_objet_paint);
      }
      else if (couleur==ROUGE)
      {
         if (objet==PETIT)
            SDL_BlitSurface(objet_paint[ROUGE][PETIT], NULL, ecran, &pos_objet_paint);
         else if (objet==MOYEN)
            SDL_BlitSurface(objet_paint[ROUGE][MOYEN], NULL, ecran, &pos_objet_paint);
         else if (objet==GRAND)
            SDL_BlitSurface(objet_paint[ROUGE][GRAND], NULL, ecran, &pos_objet_paint);
      }
      else if (couleur==BLEU)
      {
         if (objet==PETIT)
            SDL_BlitSurface(objet_paint[BLEU][PETIT], NULL, ecran, &pos_objet_paint);
         else if (objet==MOYEN)
            SDL_BlitSurface(objet_paint[BLEU][MOYEN], NULL, ecran, &pos_objet_paint);
         else if (objet==GRAND)
            SDL_BlitSurface(objet_paint[BLEU][GRAND], NULL, ecran, &pos_objet_paint);
      }
      else if (couleur==VERT)
      {
         if (objet==PETIT)
            SDL_BlitSurface(objet_paint[VERT][PETIT], NULL, ecran, &pos_objet_paint);
         else if (objet==MOYEN)
            SDL_BlitSurface(objet_paint[VERT][MOYEN], NULL, ecran, &pos_objet_paint);
         else if (objet==GRAND)
            SDL_BlitSurface(objet_paint[VERT][GRAND], NULL, ecran, &pos_objet_paint);
      }
      else if (couleur==JAUNE)
      {
         if (objet==PETIT)
            SDL_BlitSurface(objet_paint[JAUNE][PETIT], NULL, ecran, &pos_objet_paint);
         else if (objet==MOYEN)
            SDL_BlitSurface(objet_paint[JAUNE][MOYEN], NULL, ecran, &pos_objet_paint);
         else if (objet==GRAND)
            SDL_BlitSurface(objet_paint[JAUNE][GRAND], NULL, ecran, &pos_objet_paint);
      }
      else if (couleur==VIOLET)
      {
         if (objet==PETIT)
            SDL_BlitSurface(objet_paint[VIOLET][PETIT], NULL, ecran, &pos_objet_paint);
         else if (objet==MOYEN)
            SDL_BlitSurface(objet_paint[VIOLET][MOYEN], NULL, ecran, &pos_objet_paint);
         else if (objet==GRAND)
            SDL_BlitSurface(objet_paint[VIOLET][GRAND], NULL, ecran, &pos_objet_paint);
      }
      /* Remise a zero de la valeur de blit_objet */
      if (blit_objet)
         blit_objet=0;
   }
   /* Mise a jour de l'ecran */
   SDL_Flip(ecran);  
}
/* Envoie d'un message de fermeture si synchronisation établie */
if (synchroniser)
{
   sprintf(buffer, "SERVER OUT");
   sock_err = send(csock, buffer, SOCKET_TAILLE, 0);
   if(sock_err == SOCKET_ERROR) /* Si socket nn envoyée */
      fprintf(stderr, "Message de fermeture non envoyé !\n");
}
/* Fermeture de la connexion */
if (synchroniser) 
   shutdown(csock, 2);
/* Fermeture de la socket */
if (synchroniser)
   closesocket(sock);
/* Nettoyage SOCKET */
#if defined (WIN32)
   WSACleanup();
#endif
/* Vidage SDL */
SDL_FreeSurface(arriere_plan);
for (i=0; i<2; i++)
{
   SDL_FreeSurface(sync_button[i]);
   SDL_FreeSurface(effacer_button[i]);
}
for (i=0; i<4; i++)
{
   SDL_FreeSurface(objet_selectionner[i]);
   SDL_FreeSurface(objet_nn_selectionner[i]);
}
for (j=0; j<N_COULEUR; j++)
{
   SDL_FreeSurface(couleur_selectionner[j]);
   SDL_FreeSurface(couleur_nn_selectionner[j]);
   for (i=0; i<N_OBJET; i++)
      SDL_FreeSurface(objet_paint[j][i]);
}
SDL_FreeSurface(outil_gomme);
SDL_Quit();
return EXIT_SUCCESS;
}
