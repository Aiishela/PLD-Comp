int main (){
    int var1 = 0;
    int var2 = 25;
    int a;
    int b = 0;
    int c = 25;
    while (var1 < var2) {
        c-=b;
        if (var1%2==0) {
            b += 5;
        } else {
            var2--;
        }
        var1++;
        b*=2;
    }
    return var1 + var2 + b + c;    
}