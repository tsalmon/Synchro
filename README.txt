Algorithme du programme:
Pour chaque fichier F du repertoire S:   , on verifie que ce fichier existe dans le repertoire D.
	Pour chaque fichier F de D:
		Si F est un fichier regulier
			Si F existe a la fois dans S et dans D: 
				Si la date du fichier F de S est plus recente que celle de F de D:
					Si l'option -i est déclaré:
						on demande la permission de l'utilisateur
						Si la reponse est non:
							On continue 
					On remplace F de D par F de S
		Sinon si F est un lien symbolique et que l'option -s est déclaré:
			Si la cible de F de D est differente de la cible de F de S:
				la cible de F de D est la cible de F de S
	Si F de S n'existe pas dans D et que l'option -n est déclaré:
		Si l'option -i est déclaré:
			on demande la permission de l'utilisateur
			Si la reponse est non:
				On passe a un autre fichier de S
		On ajoute F de S dans D
	Sinon si F est repertoire et que l'option -r est déclaré:
		On réitere l'algorithme ou F de S est S et F de D est D
