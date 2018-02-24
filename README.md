-- 20171022 12:37:55.000000

PRAGMA integrity_check;

select 1000*strftime('%s', substr(date_time, 1, 4)||"-"||substr(date_time, 5, 2)||"-"||substr(date_time, 7, 11)) as timestamp from exch_bitfinex_btcusd;

SELECT date_time from exch_bitfinex_btcusd;

SELECT strftime('%s',  date_time) from exch_bitfinex_btcusd;

SELECT strftime('%s','2014-10-08 02:34:56.222222');

DROP TABLE exch_bitfinex_btcusd_adj;
create table exch_bitfinex_btcusd_adj
(
	timestamp integer,
	trade_id text
		primary key,
	trade_price decimal(10,5),
	trade_volume decimal(20,8)
);
COMMIT;

CREATE INDEX tidxx ON exch_bitfinex_btcusd(date_time);

INSERT INTO exch_bitfinex_btcusd_adj (timestamp, trade_id, trade_price, trade_volume)
SELECT 1000*strftime('%s', substr(date_time, 1, 4)||"-"||substr(date_time, 5, 2)||"-"||substr(date_time, 7, 11)), trade_id, trade_price, trade_volume
FROM   exch_bitfinex_btcusd;

--AAPL    19990602    15:59:54    46,50000
-- 20171022 12:37:55.000000	80269718	5867.70000	0.99290000

SELECT
  t.trade_price,
  Min(t.trade_price),
  Max(t.trade_price),
  sum(t.trade_volume),
  SUBSTR(t.date_time, 1, 11) || ':' ||
  SUBSTR(((SUBSTR(t.date_time, 13, 2) / 5) * 5) || '00', 1, 2)
  || ':' || '00' AS rounded_dt
FROM exch_bitfinex_btcusd t
GROUP BY rounded_dt
