-- functions --
DROP FUNCTION rb_build(integer[]);
DROP FUNCTION rb_add(roaringbitmap, integer[]);
DROP FUNCTION rb_or(roaringbitmap, roaringbitmap);
DROP FUNCTION rb_or_cardinality(roaringbitmap, roaringbitmap);
DROP FUNCTION rb_and(roaringbitmap, roaringbitmap);
DROP FUNCTION rb_and_cardinality(roaringbitmap, roaringbitmap);
DROP FUNCTION rb_xor(roaringbitmap, roaringbitmap);
DROP FUNCTION rb_xor_cardinality(roaringbitmap, roaringbitmap);
DROP FUNCTION rb_andnot(roaringbitmap, roaringbitmap);
DROP FUNCTION rb_andnot_cardinality(roaringbitmap, roaringbitmap);
DROP FUNCTION rb_cardinality(roaringbitmap);
DROP FUNCTION rb_is_empty(roaringbitmap);
DROP FUNCTION rb_equals(roaringbitmap, roaringbitmap);
DROP FUNCTION rb_intersect(roaringbitmap, roaringbitmap);
DROP FUNCTION rb_remove(roaringbitmap, integer);
DROP FUNCTION rb_flip(roaringbitmap, integer, integer);
DROP FUNCTION rb_minimum(roaringbitmap);
DROP FUNCTION rb_maximum(roaringbitmap);

DROP FUNCTION rb_rank(roaringbitmap, integer);
DROP FUNCTION rb_iterate(roaringbitmap);
DROP FUNCTION rb_is_setid(roaringbitmap, integer);

DROP AGGREGATE IF EXISTS rb_or_cardinality_agg(roaringbitmap);
DROP AGGREGATE IF EXISTS rb_or_agg(roaringbitmap);
DROP FUNCTION rb_cardinality_trans(roaringbitmap);
DROP FUNCTION rb_or_trans(roaringbitmap, roaringbitmap);


DROP AGGREGATE IF EXISTS rb_and_cardinality_agg(roaringbitmap);
DROP AGGREGATE IF EXISTS rb_and_agg(roaringbitmap);
DROP FUNCTION rb_and_trans(roaringbitmap, roaringbitmap);

DROP AGGREGATE IF EXISTS rb_xor_cardinality_agg(roaringbitmap);
DROP AGGREGATE IF EXISTS rb_xor_agg(roaringbitmap);
DROP FUNCTION rb_xor_trans(roaringbitmap, roaringbitmap);

DROP TYPE roaringbitmap CASCADE;
DROP FUNCTION roaringbitmap(bytea);

DROP FUNCTION roaringbitmap_in(cstring);
DROP FUNCTION roaringbitmap_out(roaringbitmap);
DROP FUNCTION roaringbitmap_recv(internal);
DROP FUNCTION roaringbitmap_send(roaringbitmap);

