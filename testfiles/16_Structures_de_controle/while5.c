int main (){
    int var1 = 0;
    int var2 = 25;
    int a = 4;
    int b = 0;
    int c = 25;
    while (var1 < var2) {
        c=c-b;
        if (var1%2==0) {
            b = b+ 5;
        } else {
            var2--;
        }
        var1++;
        b=b*2;
    }
    return c;    
}