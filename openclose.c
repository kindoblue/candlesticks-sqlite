//
// Created by Stefano on 23/02/2018.
//

#include <sqlite3.h>
#include <sqlite3ext.h>
#include <stdlib.h>

SQLITE_EXTENSION_INIT1

typedef struct open_state_s {
    double   total_data;  /* sum of (data * weight) values */
    double   total_wt;    /* sum of weight values */
} open_state;


void open_step(sqlite3_context *ctx, int num_values, sqlite3_value **values)
{
    double         row_wt = 1.0;
    int            type = 0;

    open_state   *st = (open_state*)sqlite3_aggregate_context( ctx,
                                                                   sizeof( open_state ) );
    if ( st == NULL ) {
        sqlite3_result_error_nomem( ctx );
        return;
    }

    /* Extract weight, if we have a weight and it looks like a number */
    if ( num_values == 1 ) {
        type = sqlite3_value_numeric_type( values[1] );
        if ( ( type == SQLITE_FLOAT )||( type == SQLITE_INTEGER ) ) {
            row_wt = sqlite3_value_double( values[1] );
        }
    }

    /* Extract data, if we were given something that looks like a number. */
    type = sqlite3_value_numeric_type( values[0] );
    if ( ( type == SQLITE_FLOAT )||( type == SQLITE_INTEGER ) ) {
       st->total_data += row_wt * sqlite3_value_double( values[0] );
       st->total_wt   += row_wt;
    }
}

void close_step(sqlite3_context *ctx, int num_values, sqlite3_value **values)
{

}
void open_final(sqlite3_context *ctx)
{
    double         result = 0.0;
    open_state   *st = (open_state*)sqlite3_aggregate_context( ctx,
                                                                   sizeof( open_state ) );
    if ( st == NULL ) {
        sqlite3_result_error_nomem( ctx );
        return;
    }

    if ( st->total_wt != 0.0 ) {
        result = st->total_data / st->total_wt;
    }
    sqlite3_result_double( ctx, result );
}

void close_final(sqlite3_context *ctx)
{

}

int sqlite3_candlestick_init(
        sqlite3 *db,
        char **pzErrMsg,
        const sqlite3_api_routines *pApi
){
    int rc = SQLITE_OK;

    SQLITE_EXTENSION_INIT2(pApi);

    // register the aggregate open/close functions
    rc += sqlite3_create_function(db, "candlestick_open", 1, SQLITE_UTF8, NULL,
                                  NULL, open_step, open_final);

    rc += sqlite3_create_function(db, "candlestick_close", 1, SQLITE_UTF8, NULL,
                                  NULL, close_step, close_final);

    return rc;
}

