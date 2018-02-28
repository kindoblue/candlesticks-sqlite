#include <iostream>
#include <sqlite3.h>
#include <getopt.h>


static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i = 0; i<argc; i++) {
        std::cout << argv[i] << "\t";
    }
    std::cout << "\n";
    return 0;
}

int main(int argc, char **argv) {

    sqlite3  *db = nullptr;
    char *errMsg = nullptr;
    int rc = 0;
    int c = 0;

    char *database = nullptr;
    char *library = nullptr;

    while ((c = getopt (argc, argv, "d:l:")) != -1)
        switch (c)
        {
            case 'd':
                database = optarg;
                break;
            case 'l':
                library = optarg;
                break;
            default:
                abort ();
        }

    if (!database || !library) {
        std::cout << "you need to pass the library (-l) and the database (-d) paths" << std::endl;
        exit(-2);
    }


    std::cout << "database = " << database << "\n" << "library = " << library << "\n\n";

    sqlite3_initialize( );

    // open the database
    rc = sqlite3_open_v2( database, &db, SQLITE_OPEN_READONLY, nullptr );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        exit( -1 );
    }

    // enable extensions
    rc = sqlite3_enable_load_extension(db, 1);
    if( rc != SQLITE_OK ) {
        std::cerr << errMsg;
        sqlite3_free(errMsg);
        exit( -1 );
    }

    // load our extension
    rc = sqlite3_load_extension(db, library, "sqlite3_candlestick_init", &errMsg);
    if( rc != SQLITE_OK ) {
        std::cerr << errMsg;
        sqlite3_free(errMsg);
        exit( -1 );
    }


    // query to get 5 min interval ohlc values
    // 2017-10-22 12:37:55
    auto selectSQL = "SELECT\n"
            "  candlestick_open(t.trade_price, t.timestamp),\n"
            "  Max(t.trade_price),\n"
            "  Min(t.trade_price),\n"
            "  candlestick_close(t.trade_price, t.timestamp),\n"
            "  sum(t.trade_volume),\n"
            "  SUBSTR(t.date_time, 1, 13) || ':' ||\n"
            "  SUBSTR(((SUBSTR(t.date_time, 15, 2) / 5) * 5) || '00', 1, 2)\n"
            "  || ':' || '00' AS rounded_dt\n"
            "FROM ticks t\n"
            "GROUP BY rounded_dt";

    // execute the query
    rc = sqlite3_exec(db, selectSQL, callback, nullptr, &errMsg);
    if( rc != SQLITE_OK ) {
        std::cerr << errMsg;
        sqlite3_free(errMsg);
        exit( -1 );
    }

    // close and shutdown the db
    sqlite3_close( db );
    sqlite3_shutdown( );

    return 0;
}