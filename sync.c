#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <fcntl.h>
#include <utime.h>
#include "sync.h"


/*alias avec (struct stat) */
#define PLUSRECENT ((long)buf[0].st_mtime > (long)buf[1].st_mtime)
/* alias avec (struct dirent*) */
#define STRCMP(a,b) (strcmp(a->d_name, b->d_name))
#define IS_DEST(a) (a.st_ino != dest_stat.st_ino)

/*
 * Appel: depuis sync
 * Condition d'appel: aucune
 * Utiliter: permet de recuprer les informations d'un fichier
 */
int getstat(char *str, struct stat *buffer)
{
    if(-1 == lstat(str, buffer))
        return 1;
    return 0;
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
int modifier_fichier(char **way)
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

/*
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
*/

/* Appel depuis sync
 * Condition d'appel: way[0](source) et way[1](destinationt
 * sont identitiques mais la source est plus recente
 * Utilisation : de
 */
void modifier(char **way, int options)
{
  struct stat droits;
  struct stat temps_way_1;
  struct utimbuf temps;
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
  stat(way[0],	&temps_way_1);
  temps.actime = temps_way_1.st_atime;
  temps.modtime = temps_way_1.st_mtime;
  utime(way[1], &temps);
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

void option_n(char *way[], dirent *fichier[],char *dest, char *src)
{
    way[1] = addstr(dest, fichier[0]->d_name,'/'); 
    if(S_ISREG(buf[0].st_mode))
    {
        modifier(way,options);
        free(way[1]);
    }
    else if(OPT_R(options, buf[0]))
    {
        mkdir(way[1],buf[0].st_mode);
        free(way[1]);
        option_r(way, dest, src, fichier);
    }
    else if(OPT_S(options, buf[0]))
    {
        char source[1024];
        ssize_t len = readlink(way[0], source, 1023);
        source[len] = '\0';
        symlink(source, way[1]);
        free(way[1]);
        option_s(src, dest, way, fichier);
    }
    free(way[1]);
}

void option_s(char *src, char *dest, char **way, dirent *fichier[])
{
    char *ptr;
    char actualpath[1024];
    char source1[1024];
    char source2[1024];
    ssize_t len1;
    ssize_t len2;
    ptr = realpath(src, actualpath);
    way[1] = addstr(dest, fichier[0]->d_name,'/');
    len1 = readlink(way[0], source1, 1023);
    len2 = readlink(way[1], source2, 1023);
    source1[len1] = '\0';
    source2[len2] = '\0';
    if(strcmp(source1, source2))
    {
        free(way[0]);
        way[0] = addstr(ptr, source1,'/');
        unlink(way[1]);
        symlink(way[0], way[1]);
    }
    free(way[1]);
}

void option_r(char **way, char *dest, char *src, dirent *fichier[])
{
    way[1] = addstr(dest, fichier[0]->d_name,'/');
    synchro(way[0], way[1]);
    free(way[1]);
}

/*
 * Appel: depuis synchro
 * Contexte : appliquer les options
 */
void option(int no_exist, char **way, char *src, char *dest, dirent *fichier[])
{
    if(OPT_N(options) && no_exist)
        option_n(way, fichier, dest, src);
    else if((!no_exist) && OPT_R(options, buf[0]) && IS_DEST(buf[0]))
        option_r(way, dest, src, fichier);
    else if(!no_exist && OPT_S(options, buf[0]))
        option_s(src, dest, way, fichier);
}

/*
 * Appel: depuis synchro
 * Fonctionnement : verifier qu'un fichier d'une source existe deja dans la destination
 * Si oui 
 *    si le fichier est antierieur a celui de la source on le remplace
 */
int synchro_recherche(char **w, char *s, char *d, dirent *f[])
{
    DIR *dest_rep;
    int no_exist = 1;
    if(getstat(w[0], &buf[0]))
        return -1;
    dest_rep = opendir(d);
    while(NULL != (f[1] = readdir(dest_rep)))
    {
        if(!strcmp(f[1]->d_name,".") || !strcmp(f[1]->d_name, ".."))
            continue;
        w[1] = addstr(d, f[1]->d_name, '/');
        if(getstat(w[1], &buf[1]))
            continue;
        if(!(STRCMP(f[0], f[1])))
        {
            no_exist = 0;
            if(PLUSRECENT && S_ISREG(buf[1].st_mode) && S_ISREG(buf[0].st_mode))
            {
                modifier(w, options);
            }
        }
        free(w[1]);
    }
    closedir(dest_rep);
    return no_exist;
}

/*
 * Appel : synchro
 * Contexte : droits d'un fichier
 * Utilisation : recuperer les droits d'un fichier, si on ne peut pas recuperer
 * ses droits, alors on renvoit que le fichier n'existe pas 
 */
int synchro_droits(char *src, struct stat *buf_droits, int droits_init)
{
    stat(src, buf_droits);
    if((buf_droits->st_mode & 00400) == 0)
    {
        printf("Impossible d'ouvrir '%s'.\n", src);
        return -1;
    }
    droits_init = buf_droits->st_mode;
    chmod(src, buf_droits->st_mode & 00555);
    if((buf_droits->st_mode & 00500) == 0)
    {
        printf("Impossible d'ouvrir '%s'.\n", src);
        return -1;
    }
    return droits_init;
}

/*
 * Appel: main
 * Fonctionnement : execute les options sur 
 * 1) On commence par regarder si on a les droits pour explorer le repertoire source
 * 2) Pour chaque fichier du repertoire source:
 * 3)      On execute synchro_recherche()
 * 4)      Puis option()
 */
void synchro(char *src, char *dest)
{
    char **way;
    int droits_init = 0;
    int no_exist;
    struct stat buf_droits;  
    struct dirent *fichier[2];
    DIR *src_rep;
    way = calloc(2, sizeof(char *));
    if((droits_init = synchro_droits(src, &buf_droits, droits_init)) == -1)
        return ;
    src_rep = opendir(src);
    while(NULL != (fichier[0] = readdir(src_rep)))
    {
        if(!strcmp(fichier[0]->d_name,".") || !strcmp(fichier[0]->d_name, ".."))
            continue;
        way[0] = addstr(src, fichier[0]->d_name, '/');
        no_exist = synchro_recherche(way, src, dest, fichier);
        option(no_exist, way, src, dest, fichier);
        free(way[0]);
    }
    chmod(src, droits_init);
    free(way);  
    closedir(src_rep);
}

/*
 * Appel: depuis syncho, ajouter, 
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
