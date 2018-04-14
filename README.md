## Candlestick custom functions for sqlite

Supposing you have a timeseries of prices in a sqlite database, you can calculate the OHLCV with a query, provided you have the function 'open' and 'close'. This project implements `candlestick_open` and `candlestick_close` to write such queries. 

### Compile the library
To compile the library you need to have installed cmake and [conan](https://conan.io), a package manager for C/C++. 

Create (and enter) a build directory in the project directory, like this: 

```
mkdir cmake-build-debug; cd cmake-build-debug
```

Launch conan to install the dependencies (sqlite library)

```
conan install ..
```

You should get something similar

```
(conan) ~candlestick_sqlite/cmake-build-debug (master ✘)✹✭ ᐅ conan install ..
.
.
.
sqlite3/3.21.0@bincrafters/stable: Not found in local cache, looking in remotes...
sqlite3/3.21.0@bincrafters/stable: Trying with 'conan-center'...
Downloading conanmanifest.txt
[==================================================] 232B/232B
Downloading conanfile.py
[==================================================] 2.1KB/2.1KB
Downloading conan_export.tgz
[==================================================] 758B/758B
PROJECT: Installing /Users/ice/Development/candlestick_sqlite/conanfile.txt
Requirements
    sqlite3/3.21.0@bincrafters/stable from conan-center
Packages
    sqlite3/3.21.0@bincrafters/stable:8018a4df6e7d2b4630a814fa40c81b85b9182d2b

PROJECT: Retrieving package 8018a4df6e7d2b4630a814fa40c81b85b9182d2b
sqlite3/3.21.0@bincrafters/stable: Looking for package 8018a4df6e7d2b4630a814fa40c81b85b9182d2b in remote 'conan-center'
Downloading conanmanifest.txt
[==================================================] 269B/269B
Downloading conaninfo.txt
[==================================================] 430B/430B
Downloading conan_package.tgz
[==================================================] 704.7KB/704.7KB
sqlite3/3.21.0@bincrafters/stable: Package installed 8018a4df6e7d2b4630a814fa40c81b85b9182d2b
PROJECT: Generator cmake created conanbuildinfo.cmake
PROJECT: Generator txt created conanbuildinfo.txt
PROJECT: Generated conaninfo.txt
```

Once conan has installed the dependencies, you can run cmake and then make the project

```
cmake . 
make -j2
``` 

Now in `cmake-build-debug/lib` you should have the library `libcandlestick.dylib` (or `libcandlestick.so` on linux). This library is the sqlite extension containing our custom functions. 

### Run the sample program
In `cmake-build-debug/bin` there is a simple driver program that can open a sqlite db, load our extension and perform a query to get the 5min OHLCV aggregate. To run, use the following

```
./bin/aggregate -l <<path to libcandlestick.dylib|so>> -d <<path to data/ticks.db>>
```

The first argument is the path to the sqlite extension and the second is the path to the database. A sample db `ticks.db` you will find in the `data` directory. 

If all goes well, you should see the aggregate printed in the console, like this:

```
(conan) ~/candlestick_sqlite/cmake-build-debug (master ✘)✹✭ ᐅ ./bin/aggregate -l ./lib/libcandlestick.dylib -d ../data/ticks.db

database = ../data/ticks.db
library = ./lib/libcandlestick.dylib

5867.7  5867.7  5847.2  5850.5  63.56572419     2017-10-22 12:35:00
5850.5  5878.9  5850.4  5877.9  60.40248926     2017-10-22 12:40:00
5874.5  5889    5869.2  5871.5  117.2912538     2017-10-22 12:50:00
5871.5  5871.6  5840    5858.1  127.17194738    2017-10-22 12:55:00
5856.9  5877    5848    5871.2  65.87313639     2017-10-22 13:00:00
5860.5  5865.2  5851.5  5853.5  44.37854358     2017-10-22 13:10:00
5852.2  5852.6  5820    5824.4  168.06848503    2017-10-22 13:15:00
5821.5  5848.6  5821.5  5842.1  47.92062093     2017-10-22 13:20:00
5842.0  5842    5821.2  5827.0  67.63175763     2017-10-22 13:25:00
5827.0  5854.7  5826.9  5845.0  53.65033719     2017-10-22 13:30:00
5845.0  5860    5844.9  5860.0  45.07572744     2017-10-22 13:35:00
5860.0  5872    5859.9  5860.6  51.49331592     2017-10-22 13:40:00
.
.
.


```

### Query
The example program load the extension and then run a query like this one:

```
SELECT
   candlestick_open(t.trade_price, t.timestamp) as open,
   Max(t.trade_price) as high,
   Min(t.trade_price) as low,
   candlestick_close(t.trade_price, t.timestamp) as close,
   sum(t.trade_volume) as volume,
   strftime('%s', printf("%s:%.2d:00", SUBSTR(t.date_time, 1, 13), ((SUBSTR(t.date_time, 15, 2) / 5) * 5))) AS rounded_timestamp
FROM ticks t
WHERE t.timestamp BETWEEN 1508575930 AND 1508898453
GROUP BY rounded_timestamp;
```

Here for simplicity I have a varchar column with the date already converted from the integer timestamp value, so the query is a bit simpler. 

