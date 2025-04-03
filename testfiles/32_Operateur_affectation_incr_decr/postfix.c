int main() {
    int b = 4; 
    int a;
    int c;
    a = 2;
    b++;     
    b--;
    b--;   
    b = 2* b + a++;   
    c = a * b--;
    b = b *c;
    return c;
}