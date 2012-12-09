#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "sync.h"

/*alias avec (struct stat) */
#define PLUSRECENT(a,b) (a.st_mtime < b.st_mtime)
/* alias avec (struct dirent*) */
#define STRCMP(a,b) (strcmp(a->d_name, b->d_name))
/*
 * Appel : depuis le main 
 * Condition d'appel: si
 * (options valides et argc(de main) == 4 et -r signalé)
 * ou si argc == 3                                     
 *      
 */
int sync_rec(char *src, char *dest, int options)
{
  return 0;
}

/*
 * Appel: depuis sync
 * Condition d'appel: aucune
 * Utiliter: permet de recuprer les informations d'un fichier
 */
void getstat(char *str, struct stat *buffer)
{
  if(-1 == stat(str, buffer))
    {
      puts("Une erreur a arreté le programme");
      exit(EXIT_FAILURE);
    }
}

int reponse_positive(char *str)
{
  return !strcmp(str, "y") || !strcmp(str,"Y") || !strcmp(str, "yes") || !strcmp(str, "YES");
}

char *saisie()
{
  char c;
  char *resultat = NULL;
  int taille;
  if(NULL == (resultat = calloc(1,sizeof(char))))
    {
      return NULL;
    } 
  taille = 0;
  while('\n' != (c = getchar()))
    {
      if(NULL == (resultat = realloc( resultat, sizeof(char)*(taille+2))))
	{
	  return NULL;
	}
      resultat[taille] = c;
      resultat[taille+1] = '\0';
      taille++;
    }
  return resultat;
}

/*
 * Appel : depuis modifier_fichier
 * Condition d'appel: si le fichier n'a pas pu 
 * etre copié
 * Utilisation: Permet de copier des fichiers binaires
 */
int modifier_fichier_binaire(char **way)
{
  FILE *f, *f2;
  char *buf;
  int taille;
  
  if(NULL == (f = fopen(way[0],"rb")))
    {
      printf("Impossible d'ouvrir le fichier ");
      return 0;
    }
  if(NULL == (f2 =  fopen(way[1],"wb")))
    {
      puts("erreur 2");
      fclose(f);
      return 0;
    }
  
  fseek(f, 0, SEEK_END);
  taille = ftell(f);
  fseek(f, 0, SEEK_SET);
  
  buf = malloc(sizeof(char)*taille);
  
  fread(buf, sizeof(char), taille, f);
  fwrite(buf, sizeof(char), taille,f2);
  
  free(buf); fclose(f2); fclose(f);
  return 0;
}


int modifier_fichier(char **way)
{
  int binaire = 1;
  char c;
  FILE *f;
  FILE *f2;
  if(NULL == (f = fopen(way[0],"r")))
    {
      printf("Impossible d'ouvrir le fichier %s.\n", way[0]);
      return -1;
    }
  if(NULL == (f2 = fopen(way[1],"w+")))
    {
      printf("Impossible d'ouvrir le fichier %s.\n", way[1]);
      fclose(f);
      return -1;
    }
  while(EOF != (c = fgetc(f) ) )
    {
      binaire = 0;
      fputc(c, f2);
    }
  fclose(f); fclose(f2);
  if(binaire)
    modifier_fichier_binaire(way);
  return 0;
} 

/* Appel depuis sync
 * Condition d'appel: way[0](source) et way[1](destinationt
 * sont identitiques mais la source est plus recente
 * Utilisation : de
 */
void modifier(char **way, int options)
{
  struct stat droits;
  getstat(way[0], &droits);
  if(OPT_I(options))
    {
      char *saisie_utilisateur;
      printf("Ecraser %s et le remplacer par %s ?[Y/n] ", way[1], way[0]);
      saisie_utilisateur = saisie();
      if(reponse_positive(saisie_utilisateur))
	modifier_fichier(way);
      free(saisie_utilisateur);
    }
  else
    {
      printf("%s remplacé par %s.\n", way[1], way[0]);
      modifier_fichier(way);      
    }
  chmod(way[1], droits.st_mode);
}

/*
 * Appel: depuis sync
 * Contexte: si un fichier de la source ne se 
 * trouve pas dans le repertoire de destination 
 * Utilisation: permet de l'ajouter (en tenant compte
 * des droits
 */
void ajouter(char *dest, char *src, char *fichier)
{
  struct stat droits;
  char **way = calloc(2, sizeof(char *));
  way[0] = addstr(src, fichier,'/');
  getstat(way[0],&droits);
  way[1] = addstr(dest, fichier,'/');
  modifier_fichier(way);
  chmod(way[1],droits.st_mode);
  free(way[0]);
  free(way[1]);
  free(way);
  printf("ajout de %s dans le repertoire de destination\n", fichier);
}

/* Appel: depuis le main(int argc, char **argv)                                  
 * Contexte: immédiat
 * Utilisation: synchronisation avec options              
 */
int sync(char *src, char *dest, int options)
{
  char **way = calloc(2, sizeof(char *));
  struct stat buf[2];
  struct dirent *fichier[2];
  DIR *dest_rep, *src_rep = opendir(src);

  while(NULL != (fichier[0] = readdir(src_rep)))
    {
      int no_exist = 1;
      if(!strcmp(fichier[0]->d_name,".") || !strcmp(fichier[0]->d_name, ".."))
	continue;

      way[0] = addstr(src, fichier[0]->d_name, '/');
      dest_rep = opendir(dest);
      getstat(way[0], &buf[0]);
      while(NULL != (fichier[1] = readdir(dest_rep)))
	{
	
	  if(!strcmp(fichier[1]->d_name,".") || !strcmp(fichier[1]->d_name, ".."))
	    continue;

	  way[1] = addstr(dest, fichier[1]->d_name, '/');
	  getstat(way[1], &buf[1]);
	  
	  if(!(STRCMP(fichier[0], fichier[1])))
	    {
	      no_exist = 0;
	      if(PLUSRECENT(buf[1], buf[0]) && S_ISREG(buf[1].st_mode) &&  S_ISREG(buf[0].st_mode))
		modifier(way, options);
	    }
	
	  free(way[1]);
	}
      /* option n */
      if(no_exist && OPT_N(options) )
	{
	  if(S_ISREG(buf[0].st_mode))
	    ajouter(dest, src, fichier[0]->d_name);	
	  else if(OPT_R(options) && S_ISDIR(buf[0].st_mode) && buf[0].st_ino != dest_stat.st_ino)
	    {
	      way[0] = addstr(src, fichier[0]->d_name, '/');
	      way[1] = addstr(dest, fichier[0]->d_name, '/');
	      getstat(way[0], &buf[0]);
	      mkdir(way[1], buf[0].st_mode);
	      sync(way[0], way[1], options);
	      free(way[1]);
	    }
	}
      /* option -r */
      else if(!no_exist && OPT_R(options) && S_ISDIR(buf[0].st_mode) && buf[0].st_ino != dest_stat.st_ino)
	{
	  way[0] = addstr(src, fichier[0]->d_name,'/');
	  way[1] = addstr(dest, fichier[0]->d_name, '/');
	  sync(way[0], way[1], options);
	  free(way[1]);
	}
      closedir(dest_rep);
      free(way[0]);
    }
  free(way);  closedir(src_rep);
  return 0;
}

/*
 * Appel: depuis sync, ajouter, 
 * Contexte: besoin de regrouper des information (char*)
 * Utilisation, permet de concatener deux pointeurs de 
 * chaines et de les séparer par un caractere spécifié
 */
char *addstr(char *str, char *str2, char sep)
{
  int l1;
  int l2;
  int ls;
  char *resultat = NULL;
  l1      =       strlen(str);
  l2  =   strlen(str2);
  ls      =       1;
  if(NULL == (resultat = calloc(l1 + l2 + ls + 1, sizeof * resultat)))
    {
      return NULL;
    }
  memcpy(resultat , str, l1);
  resultat[l1] = sep;
  memcpy(resultat+l1 + ls, str2, l2+1);
  return resultat;
}

/*
 * Appel depuis sync
 * Contexte: exploration d'arborescence
 * Utilisation: permet de verifier qu'une chaine ne
 * correspond pas au repertoire courant ou au repertoire
 * parent.
 */
int est_sousrep(char *str)
{
  return ( (strcmp(str, ".") != 0) && (strcmp(str, "..") != 0) );
}
