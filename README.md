# PLD-Comp

Hexanome : H2442
Membres :
- BARATOVA Malika
- SAADALLA Nadine
- COCHET Mélisse
- VARJABEDIAN Noémie
- CHARLOTTE Matéo
- DOAN Adrien
- HUE Simon

## Lancement du code

> Depuis le dossier compiler.
Afin de lancer le code, faites : 
```
make
```

Ensuite, vous pouvez compiler les fichiers test avec :
```
./ifcc ../testfiles/14_test_vf.c > output.s
```
Cette dernière commande redirige la sortie (code assembleur) vers output.s qui sera utilisé par notre script shell pour executer le code assembleur généré : 
```
./s_exec.sh
```

Pour éxécuter tous les tests, exécuter la commande :
```
../ifcc-test.py ../testfiles/
```

## Ce qui est présent

Dans le programme de mi parcours nous avons implémentés ces fonctionnalités:
- l'unique fonction présente est main, sans arguments et contient une seule parie d'accolades
- les déclarations et définitions de variables peuvent être faites de cette manière : ```int a; int b, c; int a = 42;```
- la varaible de retour peut être une expression
- les expressions prises en charge sont : ```(), +, -(unaire et soustraction), *, /, %`, !, <, >, ==, !=, &, ~, | ```

Les logs de l'exécution du code compilé peuvent être trouvés dans le fichier ```erreur.s``` (contenu de la table des symboles, si une variable est mal utilisée, etc.)

## Organisation du code

- ```CodeGenVisitor``` : parcourt le programme et genère le code sur cout
- ```VariableCheckVisitor``` : parcourt le programme et ajoute les variables rencontrés dans la table des symboles
- ```SymboleTable``` : gère la table des symboles (variable déclarée une seule fois, variable définie avant d'être utilisée, etc), les logs peuvent être trouvés dans le fichier ```erreur.s``` à la fin de la compilation

