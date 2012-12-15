#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "sync.h"
#include "main.h"

/*
 * Appel: 
 * -> depuis le main(int argc, char **argv)
 * Condition d'appel : si argc == 4 
 * Utilisation : on verifie que (char *)argv_1 contienne bien 
 * r, n, i ou s et qu'il commence par un tiret  
 */
int options_valides(char *argv_1){
	if(argv_1[0] != '-')
		return 0;
	else
	{	
		int r;
		int n;
		int i;
		int s;
		r = (strchr(argv_1, 'r') == NULL) ? 0 : 1;	
		n = (strchr(argv_1, 'n') == NULL) ? 0 : 1;
		i = (strchr(argv_1, 'i') == NULL) ? 0 : 1;	
		s = (strchr(argv_1, 's') == NULL) ? 0 : 1;
  		return r || n || i || s;
	}
}

/*
 * Appel: depuis le main
 * Condition d'appel: si argc == 4(et options valides )
 * ou si argc == 3
 * Utilisation : verifier qu'un repertoire existe
 * Retour : renvoit 1 si repertoire existe, 0 sinon
 */
int dir_exist(char *dir)
{
  DIR *test_exist;
  if(NULL == (test_exist = opendir(dir)))
    {
      printf("Le repertoire '%s' n'existe pas\n", dir);
      return 0;
    }
  closedir(test_exist);
  return 1;
}

/*
 * Appel: depuis le main
 * Condition d'appel: 
 * (argc(de main) != 4 et options invalides) ou si argc != 3
 * Utilisation: 
 * explique l'utilisation du programme
 * note: la valeur de retour est sans importance
 */
int help(int i)
{
  puts("il faut lancer le programme comme ceci :");
  puts("synchro [-{n, r , i, s}] destination source");
  puts("destinaton et sources sont des repertoires.");
  return 0;
} 

int main(int argc, char **argv)
{
  if(argc == 4 && options_valides(argv[1]))
    { 
      int r = (strchr(argv[1], 'r') == NULL) ? 0 : 1000;
      int n = (strchr(argv[1], 'n') == NULL) ? 0 : 100;
      int i = (strchr(argv[1], 'i') == NULL) ? 0 : 10;
      int s = (strchr(argv[1], 's') == NULL) ? 0 : 1;
      if(!dir_exist(argv[2]) || !dir_exist(argv[3]))
	return 0;
      else
	{
	  getstat(argv[2], &dest_stat);
	  synchro(argv[3], argv[2], r + n + i +s);
	}
    }
  else if(argc == 3)
    {
      if(!dir_exist(argv[1]) || !dir_exist(argv[2]))
	return (0);
      getstat(argv[1], &dest_stat);
      synchro(argv[2], argv[1], 0);
    }
  else
    {
      help(0);
    }
  return 0;
}
