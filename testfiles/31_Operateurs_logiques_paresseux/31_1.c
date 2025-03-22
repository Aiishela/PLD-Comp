int main() {
    int x = 5;
    int y = 0;

    if (x > 0 || y++ > 0) {  // La seconde condition ne doit pas être évaluée, y doit rester à 0
        return y;  // Doit retourner 0
    }

    return 1;
}
