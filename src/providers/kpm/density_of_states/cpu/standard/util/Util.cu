int maxOutNumThreads(int n) 
{
    unsigned int power = 1;

    if (n == 0)
    { 
        return power;
    }

    while (power < n) 
    {
        power *= 4; 
    }

    return power;
}