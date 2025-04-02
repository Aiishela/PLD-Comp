int main (){
    int var1 = 0;
    int var2 = 14;
    int a;
    int b = 0;
    while (var1 < var2) {
        if (var1==4) {
            b = 5;
        }
        var1++;
    }
    return var1 + var2 + b;    
}