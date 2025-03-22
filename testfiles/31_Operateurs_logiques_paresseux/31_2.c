int main() {
    int x = 0;
    int y = 0;

    if (x > 0 && y++ > 0) {  // La seconde condition ne doit pas être évaluée, y doit rester à 0
        return y;
    }

    return y;  // Doit retourner 0
}
