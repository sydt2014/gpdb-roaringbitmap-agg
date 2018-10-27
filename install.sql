SET search_path = public;

DROP TYPE IF EXISTS RBP CASCADE;
CREATE TYPE RBP;

--- data type --

CREATE OR REPLACE FUNCTION RBP_IN(cstring)
   RETURNS RBP
   AS 'roaringbitmap.so','rbp_in'
   LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION RBP_OUT(RBP)
   RETURNS cstring
   AS 'roaringbitmap.so','rbp_out'
   LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION RBP_RECV(internal)
   RETURNS RBP
   AS 'roaringbitmap.so','rbp_recv'
   LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION RBP_SEND(RBP)
   RETURNS bytea
   AS 'roaringbitmap.so','rbp_send'
   LANGUAGE C STRICT IMMUTABLE;

CREATE TYPE RBP (
    INTERNALLENGTH = VARIABLE,
    INPUT = RBP_IN,
    OUTPUT = RBP_OUT,
    receive = RBP_RECV,
    send = RBP_SEND,
    STORAGE = external,
    alignment = int4
 );

-- functions --

CREATE OR REPLACE FUNCTION RBP_CREATE(integer[])
   RETURNS RBP 
   AS 'roaringbitmap.so', 'rbp_create'
   STRICT LANGUAGE C IMMUTABLE;

CREATE OR REPLACE FUNCTION RBP_CARDINALITY(RBP)
   RETURNS integer 
   AS 'roaringbitmap.so', 'rbp_cardinality'
   STRICT LANGUAGE C IMMUTABLE;

CREATE OR REPLACE FUNCTION RBP_AND_AGG_SFUNC(RBP, RBP)
   RETURNS RBP
   AS 'roaringbitmap.so', 'rbp_and_agg_sfunc'
   LANGUAGE C IMMUTABLE;

DROP AGGREGATE IF EXISTS RBP_AND_AGG(RBP);

CREATE AGGREGATE RBP_AND_AGG(RBP) (  
       SFUNC = RBP_AND_AGG_SFUNC,  
       STYPE = RBP,
       INITCOND = NULL
);  

DROP TABLE t_customer_tag;
CREATE TABLE t_customer_tag (tagid varchar(64), bitmap RBP, start_date date, end_date date)
       DISTRIBUTED BY (tagid);
       
CREATE INDEX customer_tag_tagid_ix ON t_customer_tag (tagid);

insert into t_customer_tag (tagid, bitmap, start_date, end_date) values ('1', RBP_CREATE(array[1]), '2018-10-17', '2099-12-31');      
