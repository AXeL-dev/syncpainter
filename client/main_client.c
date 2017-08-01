/* Application client */
/* Crée le  : 27/09/2013 */
/* Date de la dernière modification : 28/09/2013 */
/* Commentaire : application client/serveur qui permet de dessiner sur l'ecran du serveur + possiblité de synchroniser avec le client et voir ce qu'on dessine sur le serveur en directe */
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <windows.h>
#include <pthread.h>
#include <string.h> /* Pour strcmp(); */
#include "const_client.h"
#include "socket.h"

/* Variables globales */
SDL_Rect pos_objet_paint;
int connexion=1, continuer=1, blit_objet=0, r_ecran=1, objet=PETIT, couleur=NOIR;


/* Thread de récéption de données */
void *recepteur(void *data)
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
char buffer[SOCKET_TAILLE]="";
int synchroniser=0;
/* Si les sockets Windows fonctionnent */
    if(!erreur)
    {
        /* Création de la socket */
        sock = socket(AF_INET, SOCK_STREAM, 0);
  
        /* Configuration de la connexion */
        sin.sin_addr.s_addr = inet_addr("127.0.0.1");//"192.168.1.9");
        sin.sin_family = AF_INET;
        sin.sin_port = htons(PORT);
        
        while (continuer)
        {
           /* Si l'on a réussi à se connecter */
           if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR)
           {
              fprintf(stderr, "Connection a %s sur le port %d .\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
              while(connexion)
              {
                 /* Réception des données */
                 if(recv(sock, buffer, SOCKET_TAILLE, 0) != SOCKET_ERROR) /* Si l'on reçoit des informations */
                 {
                    //fprintf(stderr, "%s\n", buffer);
                    if (strcmp(buffer, "SERVER OUT") == 0)
                    {
                       fprintf(stderr, "Connexion perdu avec le serveur !\n");
                       connexion=0;
                    }
                    else if (strcmp(buffer, "SYNC VERIF") == 0)
                       blit_objet=0; /* ne rien dessiner */
                    else
                    {
                       sscanf(buffer, "%d - %d - %d - %d - %d", &r_ecran, &objet, &couleur, &pos_objet_paint.x, &pos_objet_paint.y);
                       pos_objet_paint.x-=PIXEL_DIF;
                       if (!r_ecran)
                          blit_objet=1;
                    }
                 }
                 //sleep(6); /* ça ne sert a rien , deja la fonction recv() attend la réception de données */
              }
           }
           else
           {
              fprintf(stderr, "Impossible de se connecter !\n");
              MessageBox(NULL, "               Pas de synchronisation...\nVeuillez synchroniser le serveur avec le client !", "Fermeture de SyncPainter client", MB_OK | MB_ICONINFORMATION);
              continuer=0; /* On quitte la boucle while du thread actuel + la fenetre SDL (thread 2) */
              return EXIT_SUCCESS; /* On quitte le thread */
           }
        }
        /* Fermeture de la socket */
        closesocket(sock);
        #if defined (WIN32)
           WSACleanup();
        #endif    
    }
    else
       MessageBox(NULL, "          Erreur !\nVeuillez relancer l'application !", "SyncPainter client", MB_OK | MB_ICONINFORMATION);
return EXIT_SUCCESS;
}


/* Thread de dessinage */
void *dessineur(void *data)
{
/* SDL Declaration */
SDL_Surface *ecran=NULL, *arriere_plan=NULL, *objet_paint[N_COULEUR][N_OBJET]={NULL}, *outil_gomme=NULL;
SDL_Rect pos_arriere_plan;
SDL_Event event;
int i=0, j=0;
/* SDL Initialisation */
SDL_Init(SDL_INIT_VIDEO);
/* Icone de l'application */
SDL_WM_SetIcon(IMG_Load("resc\\pclient.png"), NULL);
/* Initialisation de l'ecran (taille de la fenetre etc) */
ecran=SDL_SetVideoMode(LARGEUR, HAUTEUR, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
/* Nom de l'application */
SDL_WM_SetCaption("SyncPainter client", NULL);
/* Images et positionnement */
   /* Image arriere plan */
   arriere_plan=IMG_Load("resc\\pespace.jpg");
   /* Images Objets selon la couleur : */
   /* NOIR */
   objet_paint[NOIR][PETIT]=IMG_Load("resc\\obj\\npobjet1.png");
   objet_paint[NOIR][MOYEN]=IMG_Load("resc\\obj\\npobjet2.png");
   objet_paint[NOIR][GRAND]=IMG_Load("resc\\obj\\npobjet3.png");
   /* ROUGE */
   objet_paint[ROUGE][PETIT]=IMG_Load("resc\\obj\\rpobjet1.png");
   objet_paint[ROUGE][MOYEN]=IMG_Load("resc\\obj\\rpobjet2.png");
   objet_paint[ROUGE][GRAND]=IMG_Load("resc\\obj\\rpobjet3.png");
   /* BLEU */
   objet_paint[BLEU][PETIT]=IMG_Load("resc\\obj\\bpobjet1.png");
   objet_paint[BLEU][MOYEN]=IMG_Load("resc\\obj\\bpobjet2.png");
   objet_paint[BLEU][GRAND]=IMG_Load("resc\\obj\\bpobjet3.png");
   /* VERT */
   objet_paint[VERT][PETIT]=IMG_Load("resc\\obj\\verpobjet1.png");
   objet_paint[VERT][MOYEN]=IMG_Load("resc\\obj\\verpobjet2.png");
   objet_paint[VERT][GRAND]=IMG_Load("resc\\obj\\verpobjet3.png");
   /* JAUNE */
   objet_paint[JAUNE][PETIT]=IMG_Load("resc\\obj\\jpobjet1.png");
   objet_paint[JAUNE][MOYEN]=IMG_Load("resc\\obj\\jpobjet2.png");
   objet_paint[JAUNE][GRAND]=IMG_Load("resc\\obj\\jpobjet3.png");
   /* VIOLET */
   objet_paint[VIOLET][PETIT]=IMG_Load("resc\\obj\\viopobjet1.png");
   objet_paint[VIOLET][MOYEN]=IMG_Load("resc\\obj\\viopobjet2.png");
   objet_paint[VIOLET][GRAND]=IMG_Load("resc\\obj\\viopobjet3.png");
   /* Gomme */
   outil_gomme=IMG_Load("resc\\gomme.png");
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
      /* objet_paint */
      pos_objet_paint.x=0;
      pos_objet_paint.y=0;
/* Placement de la sourie au milieu de l'ecran */
SDL_WarpMouse(ecran->w/2, ecran->h/2);
/* Gestion d'évenements */
while(continuer)
{
SDL_PollEvent(&event);
   switch(event.type)
   {
   /* Fermeture de l'application */
   case SDL_QUIT: connexion=0; continuer=0; break;
   }
/* Blit */
   /* Arriere plan */
   if (r_ecran) /* Si on peut rénitialiser/effacer l'ecran */
   {
      SDL_BlitSurface(arriere_plan, NULL, ecran, &pos_arriere_plan);
      r_ecran=0;
   }
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
/* On réduit la vitesse de la boucle */
//sleep(1);
}
/* Vidage SDL */
SDL_FreeSurface(arriere_plan);
for (j=0; j<N_COULEUR; j++)
{
   for (i=0; i<N_OBJET; i++)
      SDL_FreeSurface(objet_paint[j][i]);
}
SDL_FreeSurface(outil_gomme);
SDL_Quit();
return EXIT_SUCCESS;
}


/* Fonction Principale */
int main (int argc, char *argv[])
{
pthread_t thread[2];
pthread_create(&thread[0], NULL, recepteur, NULL);
pthread_create(&thread[1], NULL, dessineur, NULL);
pthread_join(thread[0], NULL);
pthread_join(thread[1], NULL);
return EXIT_SUCCESS;
}
