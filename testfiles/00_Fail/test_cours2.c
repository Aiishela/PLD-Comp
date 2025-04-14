int fibo(int n)
{
    if ( n < 0 || n == 0)  // <= ne fonctionne pas
    {
        return 0;
    }
    if (n == 1) // else if ne fonctionne pas 
    {
        return 1;
    }
    return fibo(n-1) + fibo(n-2);
}

int main()
    {
        return 2*fibo(8);
    }
