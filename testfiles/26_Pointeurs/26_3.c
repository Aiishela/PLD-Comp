// Shadowing avec pointeurs

int main() {
    int a = 10;
    int* ptr = &a;
    {
        int a = 20;  // Nouveau a dans une portée plus petite
        *ptr = 30;   // Doit modifier l'ancien a (10 devient 30)
    }
    return a;  // Retourne 30
}