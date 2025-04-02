int main (){
    int var1 = 0;
    int var2 = 25;
    int b= 0;
    int c = 5;
    while (var1 < var2) {
        if (var1%5==0) {
            b += 5;
            return var1 + var2 + b + 100;
        } else {
            var2--;
        }

        c *= var1 + var2;

        if (var1%3>1) {
            b+=17;
        }
        var1++;
    }
    return var1 + var2 + b;    
}