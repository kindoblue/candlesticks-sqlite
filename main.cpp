#include <iostream>

#include <sqlite3.h>


int main() {

    sqlite3  *db = nullptr;
    char *errMsg = nullptr;
    int rc;

    sqlite3_initialize( );

    rc = sqlite3_open_v2( "", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        exit( -1 );
    }

    rc = sqlite3_enable_load_extension(db, 1);
    if( rc != SQLITE_OK ) {
        std::cerr << errMsg;
        sqlite3_free(errMsg);
        exit( -1 );
    }

    rc = sqlite3_load_extension(db, "../lib/libcandlestick",
                                "sqlite3_candlestick_init", &errMsg);
    if( rc != SQLITE_OK ) {
        std::cerr << errMsg;
        sqlite3_free(errMsg);
        exit( -1 );
    }

    auto sql = "PRAGMA journal_mode=OFF";
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if( rc != SQLITE_OK ) {
        std::cerr << errMsg;
        sqlite3_free(errMsg);
        exit( -1 );
    }

    auto createSQL ="CREATE TABLE Person(Class INT, Value REAL, Weight REAL);";
    rc = sqlite3_exec(db, createSQL, nullptr, nullptr, &errMsg);
    if( rc != SQLITE_OK ) {
        std::cerr << errMsg;
        sqlite3_free(errMsg);
        exit( -1 );
    }

    auto insertSQL1 = "INSERT INTO Person VALUES(1, 3.4, 1.0);";
    rc = sqlite3_exec(db, insertSQL1, nullptr, nullptr, &errMsg);
    if( rc != SQLITE_OK ) {
        std::cerr << errMsg;
        sqlite3_free(errMsg);
        exit( -1 );
    }

    auto insertSQL2 = "INSERT INTO Person VALUES(1, 6.4, 2.3);";
    rc = sqlite3_exec(db, insertSQL2, nullptr, nullptr, &errMsg);
    if( rc != SQLITE_OK ) {
        std::cerr << errMsg;
        sqlite3_free(errMsg);
        exit( -1 );
    }

    auto insertSQL3 = "INSERT INTO Person VALUES(3, 5.4, 1.3);";
    rc = sqlite3_exec(db, insertSQL3, nullptr, nullptr, &errMsg);
    if( rc != SQLITE_OK ) {
        std::cerr << errMsg;
        sqlite3_free(errMsg);
        exit( -1 );
    }


    auto selectSQL = "SELECT Class, candlestick_open( Value ), candlestick_close( Value ) FROM Person GROUP BY Class;";
    //auto selectSQL = "SELECT class, value FROM Person GROUP BY Class;";
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