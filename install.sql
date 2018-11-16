DROP TYPE IF EXISTS roaringbitmap CASCADE;
CREATE TYPE roaringbitmap;

--- data type --

CREATE OR REPLACE FUNCTION roaringbitmap_in(cstring)
   RETURNS roaringbitmap
   AS 'roaringbitmap.so','roaringbitmap_in'
   LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION roaringbitmap_out(roaringbitmap)
   RETURNS cstring
   AS 'roaringbitmap.so','roaringbitmap_out'
   LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION roaringbitmap_recv(internal)
   RETURNS roaringbitmap
   AS 'roaringbitmap.so','roaringbitmap_recv'
   LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION roaringbitmap_send(roaringbitmap)
   RETURNS bytea
   AS 'roaringbitmap.so','roaringbitmap_send'
   LANGUAGE C STRICT IMMUTABLE;

CREATE TYPE roaringbitmap (
    INTERNALLENGTH = VARIABLE,
    INPUT = roaringbitmap_in,
    OUTPUT = roaringbitmap_out,
    receive = roaringbitmap_recv,
    send = roaringbitmap_send,
    STORAGE = external
 );

-- functions --

CREATE OR REPLACE FUNCTION rb_build(bigint[])
   RETURNS roaringbitmap 
   AS 'roaringbitmap.so', 'rb_build'
   LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION rb_add(roaringbitmap, bigint[])
   RETURNS roaringbitmap 
   AS 'roaringbitmap.so', 'rb_add'
   LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION rb_or(roaringbitmap, roaringbitmap)
   RETURNS roaringbitmap 
   AS 'roaringbitmap.so', 'rb_or'
   LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION rb_or_cardinality(roaringbitmap, roaringbitmap)
   RETURNS bigint
   AS 'roaringbitmap.so', 'rb_or_cardinality'
   LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION rb_and(roaringbitmap, roaringbitmap)
   RETURNS roaringbitmap 
   AS 'roaringbitmap.so', 'rb_and'
   LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION rb_and_cardinality(roaringbitmap, roaringbitmap)
   RETURNS bigint
   AS 'roaringbitmap.so', 'rb_and_cardinality'
   LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION rb_xor(roaringbitmap, roaringbitmap)
   RETURNS roaringbitmap 
   AS 'roaringbitmap.so', 'rb_xor'
   LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION rb_xor_cardinality(roaringbitmap, roaringbitmap)
   RETURNS bigint
   AS 'roaringbitmap.so', 'rb_xor_cardinality'
   LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION rb_andnot(roaringbitmap, roaringbitmap)
   RETURNS roaringbitmap 
   AS 'roaringbitmap.so', 'rb_andnot'
   LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION rb_andnot_cardinality(roaringbitmap, roaringbitmap)
   RETURNS bigint
   AS 'roaringbitmap.so', 'rb_andnot_cardinality'
   LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION rb_cardinality(roaringbitmap)
   RETURNS bigint 
   AS 'roaringbitmap.so', 'rb_cardinality'
   LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION rb_is_empty(roaringbitmap)
  RETURNS bool
  AS  'roaringbitmap.so', 'rb_is_empty'
   LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION rb_equals(roaringbitmap, roaringbitmap)
  RETURNS bool
  AS  'roaringbitmap.so', 'rb_equals'
   LANGUAGE C STRICT;


CREATE OR REPLACE FUNCTION rb_intersect(roaringbitmap, roaringbitmap)
  RETURNS bool
  AS  'roaringbitmap.so', 'rb_intersect'
   LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION rb_remove(roaringbitmap, bigint)
   RETURNS roaringbitmap
   AS 'roaringbitmap.so', 'rb_remove'
   LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION rb_flip(roaringbitmap, bigint, bigint)
   RETURNS roaringbitmap
   AS 'roaringbitmap.so', 'rb_flip'
   LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION rb_minimum(roaringbitmap)
   RETURNS bigint
   AS 'roaringbitmap.so', 'rb_minimum'
   LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION rb_maximum(roaringbitmap)
   RETURNS bigint
   AS 'roaringbitmap.so', 'rb_maximum'
   LANGUAGE C STRICT;

 CREATE OR REPLACE FUNCTION rb_rank(roaringbitmap, bigint)
   RETURNS bigint
   AS 'roaringbitmap.so', 'rb_rank'
   LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION rb_iterate(roaringbitmap)
   RETURNS SETOF bigint 
   AS 'roaringbitmap.so', 'rb_iterate'
   LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION rb_is_setid(roaringbitmap, bigint)
   RETURNS bool
   AS 'roaringbitmap.so', 'rb_is_setid'
   LANGUAGE C STRICT;


-- aggragations --

CREATE OR REPLACE FUNCTION rb_build_trans(roaringbitmap, bigint)
     RETURNS roaringbitmap
      AS 'roaringbitmap.so', 'rb_build_trans'
     STRICT LANGUAGE C IMMUTABLE;


CREATE OR REPLACE FUNCTION rb_build_trans_by_array(roaringbitmap, bigint[])
     RETURNS roaringbitmap
      AS 'roaringbitmap.so', 'rb_build_trans_by_array'
     STRICT LANGUAGE C IMMUTABLE;     


CREATE OR REPLACE FUNCTION rb_build_trans_pre(roaringbitmap, roaringbitmap)
     RETURNS roaringbitmap
      AS 'roaringbitmap.so', 'rb_build_trans_pre'
     STRICT LANGUAGE C IMMUTABLE;    


DROP AGGREGATE IF EXISTS rb_build_agg(bigint);

CREATE AGGREGATE rb_build_agg(bigint)(
       SFUNC = rb_build_trans,
       STYPE = roaringbitmap,
       PREFUNC = rb_build_trans_pre,
       INITCOND = ':0\000\000\001\000\000\000\000\000\000\000\020\000\000\000\000\000'
);

DROP AGGREGATE IF EXISTS rb_build_by_array_agg(bigint[]);

CREATE AGGREGATE rb_build_by_array_agg(bigint[])(
       SFUNC = rb_build_trans_by_array,
       STYPE = roaringbitmap,
       PREFUNC = rb_build_trans_pre,
       INITCOND = ':0\000\000\001\000\000\000\000\000\000\000\020\000\000\000\000\000'
);

CREATE OR REPLACE FUNCTION rb_cardinality_trans(roaringbitmap)
     RETURNS bigint
     AS 'roaringbitmap.so', 'rb_cardinality_trans'
     STRICT LANGUAGE C IMMUTABLE;


CREATE OR REPLACE FUNCTION rb_or_trans(roaringbitmap, roaringbitmap)
     RETURNS roaringbitmap
      AS 'roaringbitmap.so', 'rb_or_trans'
     STRICT LANGUAGE C IMMUTABLE;

DROP AGGREGATE IF EXISTS rb_or_agg(roaringbitmap);

CREATE AGGREGATE rb_or_agg(roaringbitmap)(
       SFUNC = rb_or_trans,
       STYPE = roaringbitmap,
       PREFUNC = rb_or_trans
);

DROP AGGREGATE IF EXISTS rb_or_cardinality_agg(roaringbitmap);

CREATE AGGREGATE rb_or_cardinality_agg(roaringbitmap)(
       SFUNC = rb_or_trans,
       STYPE = roaringbitmap,
       PREFUNC = rb_or_trans,
       FINALFUNC = rb_cardinality_trans
);


CREATE OR REPLACE FUNCTION rb_and_trans(roaringbitmap, roaringbitmap)
     RETURNS roaringbitmap
      AS 'roaringbitmap.so', 'rb_and_trans'
    STRICT LANGUAGE C IMMUTABLE;

DROP AGGREGATE IF EXISTS rb_and_agg(roaringbitmap);

CREATE AGGREGATE rb_and_agg(roaringbitmap)(
       SFUNC = rb_and_trans,
       STYPE = roaringbitmap,
       PREFUNC = rb_and_trans
);


DROP AGGREGATE IF EXISTS rb_and_cardinality_agg(roaringbitmap);

CREATE AGGREGATE rb_and_cardinality_agg(roaringbitmap)(
       SFUNC = rb_and_trans,
       STYPE = roaringbitmap,
       PREFUNC = rb_and_trans,
       FINALFUNC = rb_cardinality_trans
);

CREATE OR REPLACE FUNCTION rb_xor_trans(roaringbitmap, roaringbitmap)
     RETURNS roaringbitmap
      AS 'roaringbitmap.so', 'rb_xor_trans'
     STRICT LANGUAGE C IMMUTABLE;


DROP AGGREGATE IF EXISTS rb_xor_agg(roaringbitmap);

CREATE AGGREGATE rb_xor_agg(roaringbitmap)(
       SFUNC = rb_xor_trans,
       STYPE = roaringbitmap,
       PREFUNC = rb_xor_trans
);


DROP AGGREGATE IF EXISTS rb_xor_cardinality_agg(roaringbitmap);

CREATE AGGREGATE rb_xor_cardinality_agg(roaringbitmap)(
       SFUNC = rb_xor_trans,
       STYPE = roaringbitmap,
       PREFUNC = rb_xor_trans,
       FINALFUNC = rb_cardinality_trans
);
