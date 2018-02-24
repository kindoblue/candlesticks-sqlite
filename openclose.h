//
// Created by Stefano on 23/02/2018.
//

#ifndef AGGREGATE_WTAVG_H
#define AGGREGATE_WTAVG_H

#ifdef __cplusplus
extern "C"
{
#endif

struct sqlite3_context;
struct sqlite3_value;

void open_step(sqlite3_context *ctx, int num_values, sqlite3_value **values);
void open_final(sqlite3_context *ctx);

#ifdef __cplusplus
}
#endif

#endif //AGGREGATE_WTAVG_H
