# PLD-Comp

## Lancement du code :
Afin de lancer le code, faites : 
```
make
```

Ensuite, vous pouvez compiler les fichiers test avec :
```
./ifcc ../testfiles/14_test_vf.c > output.s
```
Cette dernière commande redirige la sortie (code assembleur) vers output.s qui sera utilisé par notre script shell pour executer le code assembleur générer : 
```
./s_exec.sh
```