# PLD-Comp

## Lancement du code :

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
- la fonction main est composé de statement suivi d'un return statement
- déclarations et définitions de variables : ```int a; int b, c; int a = 42;```
- la varaible de retour peut être une expression
- les expressions prises en charge sont : ```(), +, -(unaire et soustraction), *, /, %`, !, <, >, ==, !=, &, ~, |, ```