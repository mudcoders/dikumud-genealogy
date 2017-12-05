#include <stdio.h>

int main( int argc, char **argv )
{
    freopen( argv[2], "a", stdout );
    freopen( argv[2], "a", stderr );

    system( argv[1] );

    exit( 0 );
    return 0;
}
