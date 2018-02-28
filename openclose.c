//
// Created by Stefano on 23/02/2018.
//

#include <sqlite3.h>
#include <sqlite3ext.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <memory.h>

SQLITE_EXTENSION_INIT1

typedef struct open_close_values_S {
    int     init_flag;
    double  open_value;
    double  close_value;
    long    min_timestamp;
    long    max_timestamp;
} open_close_values;


void min_max_step(sqlite3_context *ctx, int num_values, sqlite3_value **values)
{
    long  timestamp = 0;
    int            type = 0;
    bool           is_first = false;
    bool           is_last = false;

    open_close_values   *st = (open_close_values*)sqlite3_aggregate_context( ctx,
                                                                   sizeof( open_close_values ) );
    if ( st == NULL ) {
        sqlite3_result_error_nomem( ctx );
        return;
    }

    // initialize the context the first time
    if ( st->init_flag == 0 ) {
        st->init_flag = 1;
        st->min_timestamp = LONG_MAX;
        st->max_timestamp = LONG_MIN;
     }

    // get the timestamp parameter and check if it is the first or last in time
    // so far
    if ( num_values == 2 ) {
        type = sqlite3_value_numeric_type( values[1] );
        if (type == SQLITE_INTEGER ) {
            timestamp = sqlite3_value_int64( values[1] );

            if (timestamp < st->min_timestamp) {
                is_first = true;
                st->min_timestamp = timestamp;
            }

            if (timestamp > st->max_timestamp) {
                is_last = true;
                st->max_timestamp = timestamp;
            }

        } else {
            const char * msg = "the second parameter should be an integer timestamp";
            sqlite3_result_error(ctx, msg, strlen(msg) );
        }
    }

    // save the price value in case this record is
    // eligible as first or last record
    if (is_first || is_last) {

        type = sqlite3_value_numeric_type(values[0]);

        if (type == SQLITE_FLOAT || type == SQLITE_INTEGER) {
            double price = sqlite3_value_double(values[0]);
            if (is_first) {
                st->open_value = price;
            } else if (is_last) {
                st->close_value = price;
            }
        } else {
            const char * msg = "the first parameter should be the price, integer or float";
            sqlite3_result_error(ctx, msg, strlen(msg) );
        }
    }
}

void open_final(sqlite3_context *ctx)
{
    open_close_values   *st = (open_close_values*)sqlite3_aggregate_context( ctx,
                                                                   sizeof( open_close_values ) );
    if ( st == NULL ) {
        sqlite3_result_error_nomem( ctx );
        return;
    }

    // setting the aggregate result
    sqlite3_result_double( ctx, st->open_value );
}

void close_final(sqlite3_context *ctx)
{
    open_close_values   *st = (open_close_values*)sqlite3_aggregate_context( ctx,
                                                                             sizeof( open_close_values ) );
    if ( st == NULL ) {
        sqlite3_result_error_nomem( ctx );
        return;
    }

    // setting the aggregate result
    sqlite3_result_double( ctx, st->close_value );
}

int sqlite3_candlestick_init(
        sqlite3 *db,
        char **pzErrMsg,
        const sqlite3_api_routines *pApi
){
    int rc = SQLITE_OK;

    SQLITE_EXTENSION_INIT2(pApi);

    // register the aggregate open/close functions
    rc += sqlite3_create_function(db, "candlestick_open", 2, SQLITE_UTF8, NULL,
                                  NULL, min_max_step, open_final);

    rc += sqlite3_create_function(db, "candlestick_close", 2, SQLITE_UTF8, NULL,
                                  NULL, min_max_step, close_final);

    return rc;
}

