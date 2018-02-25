#include <iostream>

#include <sqlite3.h>


int main() {

    sqlite3  *db = nullptr;
    char *errMsg = nullptr;
    int rc;

    sqlite3_initialize( );

    // open the database
    rc = sqlite3_open_v2( "/Users/ice/Development/candlestick_sqlite/data/ticks.db", &db, SQLITE_OPEN_READONLY, nullptr );
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
    rc = sqlite3_load_extension(db, "../lib/libcandlestick",
                                "sqlite3_candlestick_init", &errMsg);
    if( rc != SQLITE_OK ) {
        std::cerr << errMsg;
        sqlite3_free(errMsg);
        exit( -1 );
    }


    // query to get 5 min interval ohlc values
    auto selectSQL = "SELECT\n"
            "  candlestick_open(t.trade_price, t.timestamp),\n"
            "  Min(t.trade_price),\n"
            "  Max(t.trade_price),\n"
            "  candlestick_close(t.trade_price, t.timestamp),\n"
            "  sum(t.trade_volume),\n"
            "  SUBSTR(t.date_time, 1, 11) || ':' ||\n"
            "  SUBSTR(((SUBSTR(t.date_time, 13, 2) / 5) * 5) || '00', 1, 2)\n"
            "  || ':' || '00' AS rounded_dt\n"
            "FROM ticks t\n"
            "GROUP BY rounded_dt";

    // execute the query
    rc = sqlite3_exec(db, selectSQL, nullptr, nullptr, &errMsg);
    if( rc != SQLITE_OK ) {
        std::cerr << errMsg;
        sqlite3_free(errMsg);
        exit( -1 );
    }

    // close and shutdown the db
    sqlite3_close( db );
    sqlite3_shutdown( );

    std::cout << "Hello, World!" << std::endl;

    return 0;
}