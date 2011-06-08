int main()
{
    int a;
    char b[2];
    for (a=0; a<3; a++)
	b[a] = -30;

    printf("%s",b);
    b[0] = 200;
    b[1] = 100;

    printf("%s",b);
    return 0;
}
