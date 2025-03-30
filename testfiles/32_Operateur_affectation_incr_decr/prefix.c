int main() {
    int b = 4; 
    int a;
    a = --b + b + 42 + b++;
    b = --b + --a + a;
    return b;
}