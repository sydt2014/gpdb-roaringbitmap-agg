#include "roaringbitmap.h"

/* Created by ZEROMAX on 2017/3/20.*/
/* Modify by changyue.zhou on 2018/20/10.*/

bool
ArrayContainsNulls(ArrayType *array) {
    int nelems;
    bits8 *bitmap;
    int bitmask;

    /* Easy answer if there's no null bitmap */
    if (!ARR_HASNULL(array))
        return false;

    nelems = ArrayGetNItems(ARR_NDIM(array), ARR_DIMS(array));

    bitmap = ARR_NULLBITMAP(array);

    /* check whole bytes of the bitmap byte-at-a-time */
    while (nelems >= 8) {
        if (*bitmap != 0xFF)
            return true;
        bitmap++;
        nelems -= 8;
    }

    /* check last partial byte */
    bitmask = 1;
    while (nelems > 0) {
        if ((*bitmap & bitmask) == 0)
            return true;
        bitmask <<= 1;
        nelems--;
    }

    return false;
}


//roaringbitmap
Datum roaringbitmap(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(roaringbitmap);

Datum
roaringbitmap(PG_FUNCTION_ARGS) {
    bytea *serializedbytes = PG_GETARG_BYTEA_P(0);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_free(r1);
    PG_RETURN_BYTEA_P(serializedbytes);
}


//roaringbitmap_in
Datum roaringbitmap_in(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(roaringbitmap_in);

Datum
roaringbitmap_in(PG_FUNCTION_ARGS) {
    Datum dd = DirectFunctionCall1(byteain, PG_GETARG_DATUM(0));

    bytea *bp = DatumGetByteaP(dd);
    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(bp));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_free(r1);
    return dd;
}


//roaringbitmap_out
Datum roaringbitmap_out(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(roaringbitmap_out);

Datum
roaringbitmap_out(PG_FUNCTION_ARGS) {
    Datum dd = DirectFunctionCall1(byteaout, PG_GETARG_DATUM(0));
    return dd;
}


//roaringbitmap_recv
Datum roaringbitmap_recv(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(roaringbitmap_recv);

Datum
roaringbitmap_recv(PG_FUNCTION_ARGS) {
    Datum dd = DirectFunctionCall1(bytearecv, PG_GETARG_DATUM(0));
    return dd;
}


//roaringbitmap_send
Datum roaringbitmap_send(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(roaringbitmap_send);

Datum
roaringbitmap_send(PG_FUNCTION_ARGS) {
    Datum dd = PG_GETARG_DATUM(0);

    bytea *bp = DatumGetByteaP(dd);
    StringInfoData buf;
    pq_begintypsend(&buf);
    pq_sendbytes(&buf, VARDATA(bp), VARSIZE(bp) - VARHDRSZ);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}


//bitmap_or
PG_FUNCTION_INFO_V1(rb_or);
Datum rb_or(PG_FUNCTION_ARGS);

Datum
rb_or(PG_FUNCTION_ARGS) {
    bytea *serializedbytes1 = PG_GETARG_BYTEA_P(0);
    bytea *serializedbytes2 = PG_GETARG_BYTEA_P(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_t *r2 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes2));
    if (!r2) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_or_inplace(r1, r2);
    roaring_bitmap_free(r2);
    size_t expectedsize = roaring_bitmap_portable_size_in_bytes(r1);
    bytea *serializedbytes = (bytea *) palloc(VARHDRSZ + expectedsize);
    roaring_bitmap_portable_serialize(r1, VARDATA(serializedbytes));
    roaring_bitmap_free(r1);

    SET_VARSIZE(serializedbytes, VARHDRSZ + expectedsize);
    PG_RETURN_BYTEA_P(serializedbytes);
}


//bitmap_or_cardinality
PG_FUNCTION_INFO_V1(rb_or_cardinality);
Datum rb_or_cardinality(PG_FUNCTION_ARGS);

Datum
rb_or_cardinality(PG_FUNCTION_ARGS) {
    bytea *serializedbytes1 = PG_GETARG_BYTEA_P(0);
    bytea *serializedbytes2 = PG_GETARG_BYTEA_P(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_t *r2 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes2));
    if (!r2) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    int32 card1 = (int) roaring_bitmap_or_cardinality(r1, r2);

    roaring_bitmap_free(r1);
    roaring_bitmap_free(r2);
    PG_RETURN_INT32(card1);
}

//bitmap_and
PG_FUNCTION_INFO_V1(rb_and);
Datum rb_and(PG_FUNCTION_ARGS);

Datum
rb_and(PG_FUNCTION_ARGS) {
    bytea *serializedbytes1 = PG_GETARG_BYTEA_P(0);
    bytea *serializedbytes2 = PG_GETARG_BYTEA_P(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_t *r2 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes2));
    if (!r2) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_and_inplace(r1, r2);
    roaring_bitmap_free(r2);
    size_t expectedsize = roaring_bitmap_portable_size_in_bytes(r1);
    bytea *serializedbytes = (bytea *) palloc(VARHDRSZ + expectedsize);
    roaring_bitmap_portable_serialize(r1, VARDATA(serializedbytes));
    roaring_bitmap_free(r1);

    SET_VARSIZE(serializedbytes, VARHDRSZ + expectedsize);
    PG_RETURN_BYTEA_P(serializedbytes);
}


//bitmap_and_cardinality
PG_FUNCTION_INFO_V1(rb_and_cardinality);
Datum rb_and_cardinality(PG_FUNCTION_ARGS);

Datum
rb_and_cardinality(PG_FUNCTION_ARGS) {
    bytea *serializedbytes1 = PG_GETARG_BYTEA_P(0);
    bytea *serializedbytes2 = PG_GETARG_BYTEA_P(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_t *r2 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes2));
    if (!r2) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    int32 card1 = (int) roaring_bitmap_and_cardinality(r1, r2);

    roaring_bitmap_free(r1);
    roaring_bitmap_free(r2);
    PG_RETURN_INT32(card1);
}


//bitmap_andnot
PG_FUNCTION_INFO_V1(rb_andnot);
Datum rb_andnot(PG_FUNCTION_ARGS);

Datum
rb_andnot(PG_FUNCTION_ARGS) {
    bytea *serializedbytes1 = PG_GETARG_BYTEA_P(0);
    bytea *serializedbytes2 = PG_GETARG_BYTEA_P(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_t *r2 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes2));
    if (!r2) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_andnot_inplace(r1, r2);
    roaring_bitmap_free(r2);
    size_t expectedsize = roaring_bitmap_portable_size_in_bytes(r1);
    bytea *serializedbytes = (bytea *) palloc(VARHDRSZ + expectedsize);
    roaring_bitmap_portable_serialize(r1, VARDATA(serializedbytes));
    roaring_bitmap_free(r1);

    SET_VARSIZE(serializedbytes, VARHDRSZ + expectedsize);
    PG_RETURN_BYTEA_P(serializedbytes);
}


//bitmap_andnot_cardinality
PG_FUNCTION_INFO_V1(rb_andnot_cardinality);
Datum rb_andnot_cardinality(PG_FUNCTION_ARGS);

Datum
rb_andnot_cardinality(PG_FUNCTION_ARGS) {
    bytea *serializedbytes1 = PG_GETARG_BYTEA_P(0);
    bytea *serializedbytes2 = PG_GETARG_BYTEA_P(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_t *r2 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes2));
    if (!r2) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    int32 card1 = (int) roaring_bitmap_andnot_cardinality(r1, r2);

    roaring_bitmap_free(r1);
    roaring_bitmap_free(r2);
    PG_RETURN_INT32(card1);
}


//bitmap_xor
PG_FUNCTION_INFO_V1(rb_xor);
Datum rb_xor(PG_FUNCTION_ARGS);

Datum
rb_xor(PG_FUNCTION_ARGS) {
    bytea *serializedbytes1 = PG_GETARG_BYTEA_P(0);
    bytea *serializedbytes2 = PG_GETARG_BYTEA_P(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_t *r2 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes2));
    if (!r2) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_xor_inplace(r1, r2);
    roaring_bitmap_free(r2);
    size_t expectedsize = roaring_bitmap_portable_size_in_bytes(r1);
    bytea *serializedbytes = (bytea *) palloc(VARHDRSZ + expectedsize);
    roaring_bitmap_portable_serialize(r1, VARDATA(serializedbytes));
    roaring_bitmap_free(r1);

    SET_VARSIZE(serializedbytes, VARHDRSZ + expectedsize);
    PG_RETURN_BYTEA_P(serializedbytes);
}


//bitmap_xor_cardinality
PG_FUNCTION_INFO_V1(rb_xor_cardinality);
Datum rb_xor_cardinality(PG_FUNCTION_ARGS);

Datum
rb_xor_cardinality(PG_FUNCTION_ARGS) {
    bytea *serializedbytes1 = PG_GETARG_BYTEA_P(0);
    bytea *serializedbytes2 = PG_GETARG_BYTEA_P(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_t *r2 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes2));
    if (!r2) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    int32 card1 = (int) roaring_bitmap_xor_cardinality(r1, r2);

    roaring_bitmap_free(r1);
    roaring_bitmap_free(r2);
    PG_RETURN_INT32(card1);
}


//bitmap cardinality
PG_FUNCTION_INFO_V1(rb_cardinality);
Datum rb_cardinality(PG_FUNCTION_ARGS);

Datum
rb_cardinality(PG_FUNCTION_ARGS) {
    bytea *data = PG_GETARG_BYTEA_P(0);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(data));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    int32 card1 = (int) roaring_bitmap_get_cardinality(r1);

    roaring_bitmap_free(r1);
    PG_RETURN_INT32(card1);
}


//bitmap is empty
PG_FUNCTION_INFO_V1(rb_is_empty);
Datum rb_is_empty(PG_FUNCTION_ARGS);

Datum
rb_is_empty(PG_FUNCTION_ARGS) {
    bytea *data = PG_GETARG_BYTEA_P(0);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(data));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    bool isempty = roaring_bitmap_is_empty(r1);

    roaring_bitmap_free(r1);
    PG_RETURN_BOOL(isempty);
}


//bitmap equals
PG_FUNCTION_INFO_V1(rb_equals);
Datum rb_equals(PG_FUNCTION_ARGS);

Datum
rb_equals(PG_FUNCTION_ARGS) {
    bytea *serializedbytes1 = PG_GETARG_BYTEA_P(0);
    bytea *serializedbytes2 = PG_GETARG_BYTEA_P(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_t *r2 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes2));
    if (!r2) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    bool isequal = roaring_bitmap_equals(r1, r2);

    roaring_bitmap_free(r1);
    roaring_bitmap_free(r2);
    PG_RETURN_BOOL(isequal);
}


//bitmap intersect
PG_FUNCTION_INFO_V1(rb_intersect);
Datum rb_intersect(PG_FUNCTION_ARGS);

Datum
rb_intersect(PG_FUNCTION_ARGS) {
    bytea *serializedbytes1 = PG_GETARG_BYTEA_P(0);
    bytea *serializedbytes2 = PG_GETARG_BYTEA_P(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_t *r2 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes2));
    if (!r2) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    bool isintersect = roaring_bitmap_intersect(r1, r2);

    roaring_bitmap_free(r1);
    roaring_bitmap_free(r2);
    PG_RETURN_BOOL(isintersect);
}


//bitmap remove
PG_FUNCTION_INFO_V1(rb_remove);
Datum rb_remove(PG_FUNCTION_ARGS);

Datum
rb_remove(PG_FUNCTION_ARGS) {
    bytea *serializedbytes1 = PG_GETARG_BYTEA_P(0);
    int32 offsetid = PG_GETARG_INT32(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_remove(r1, offsetid);

    size_t expectedsize = roaring_bitmap_portable_size_in_bytes(r1);
    bytea *serializedbytes = (bytea *) palloc(VARHDRSZ + expectedsize);
    roaring_bitmap_portable_serialize(r1, VARDATA(serializedbytes));
    roaring_bitmap_free(r1);

    SET_VARSIZE(serializedbytes, VARHDRSZ + expectedsize);
    PG_RETURN_BYTEA_P(serializedbytes);
}


//bitmap flip
PG_FUNCTION_INFO_V1(rb_flip);
Datum rb_flip(PG_FUNCTION_ARGS);

Datum
rb_flip(PG_FUNCTION_ARGS) {
    bytea *serializedbytes1 = PG_GETARG_BYTEA_P(0);
    int32 offsetstart = PG_GETARG_INT32(1);
    int32 offsetend = PG_GETARG_INT32(2);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    roaring_bitmap_flip_inplace(r1, offsetstart, offsetend);

    size_t expectedsize = roaring_bitmap_portable_size_in_bytes(r1);
    bytea *serializedbytes = (bytea *) palloc(VARHDRSZ + expectedsize);
    roaring_bitmap_portable_serialize(r1, VARDATA(serializedbytes));
    roaring_bitmap_free(r1);

    SET_VARSIZE(serializedbytes, VARHDRSZ + expectedsize);
    PG_RETURN_BYTEA_P(serializedbytes);
}


//bitmap build
PG_FUNCTION_INFO_V1(rb_build);
Datum rb_build(PG_FUNCTION_ARGS);

Datum
rb_build(PG_FUNCTION_ARGS) {
    ArrayType *a = (ArrayType *) PG_GETARG_ARRAYTYPE_P(0);

    int na, n;
    int *da;

    CHECKARRVALID(a);

    na = ARRNELEMS(a);
    da = ARRPTR(a);

    roaring_bitmap_t *r1 = roaring_bitmap_create();

    for (n = 0; n < na; n++) {
        roaring_bitmap_add(r1, da[n]);
    }

    size_t expectedsize = roaring_bitmap_portable_size_in_bytes(r1);

    bytea *serializedbytes = (bytea *) palloc(VARHDRSZ + expectedsize);
    roaring_bitmap_portable_serialize(r1, VARDATA(serializedbytes));
    roaring_bitmap_free(r1);

    SET_VARSIZE(serializedbytes, VARHDRSZ + expectedsize);
    PG_RETURN_BYTEA_P(serializedbytes);
}

//bitmap add
PG_FUNCTION_INFO_V1(rb_add);
Datum rb_add(PG_FUNCTION_ARGS);

Datum
rb_add(PG_FUNCTION_ARGS) {
    bytea *serializedbytes1 = PG_GETARG_BYTEA_P(0);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(serializedbytes1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    ArrayType *a = (ArrayType *) PG_GETARG_ARRAYTYPE_P(1);

    int na, n;
    int *da;

    CHECKARRVALID(a);

    na = ARRNELEMS(a);
    da = ARRPTR(a);

    for (n = 0; n < na; n++) {
        roaring_bitmap_add(r1, da[n]);
    }

    size_t expectedsize = roaring_bitmap_portable_size_in_bytes(r1);

    bytea *serializedbytes = (bytea *) palloc(VARHDRSZ + expectedsize);
    roaring_bitmap_portable_serialize(r1, VARDATA(serializedbytes));
    roaring_bitmap_free(r1);

    SET_VARSIZE(serializedbytes, VARHDRSZ + expectedsize);
    PG_RETURN_BYTEA_P(serializedbytes);
}

//bitmap list
PG_FUNCTION_INFO_V1(rb_iterate);
Datum rb_iterate(PG_FUNCTION_ARGS);

Datum
rb_iterate(PG_FUNCTION_ARGS) {
    FuncCallContext *funcctx;
    MemoryContext oldcontext;
    roaring_uint32_iterator_t *fctx;

    if (SRF_IS_FIRSTCALL()) {

        funcctx = SRF_FIRSTCALL_INIT();

        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        bytea *data = PG_GETARG_BYTEA_P(0);
        roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(data));
        if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

        roaring_uint32_iterator_t *fctx = roaring_create_iterator(r1);

        funcctx->user_fctx = fctx;

        MemoryContextSwitchTo(oldcontext);
    }

    funcctx = SRF_PERCALL_SETUP();

    fctx = funcctx->user_fctx;

    if (fctx->has_value) {
        Datum result;
        result = fctx->current_value;
        roaring_advance_uint32_iterator(fctx);
        SRF_RETURN_NEXT(funcctx, result);
    } else {
        roaring_free_uint32_iterator(fctx);
        SRF_RETURN_DONE(funcctx);
    }
}

//bitmap is rb_is_setid
PG_FUNCTION_INFO_V1(rb_is_setid);
Datum rb_is_setid(PG_FUNCTION_ARGS);

Datum
rb_is_setid(PG_FUNCTION_ARGS) {
    bytea *data = PG_GETARG_BYTEA_P(0);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(data));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    bool is_set = roaring_bitmap_contains(r1);

    roaring_bitmap_free(r1);
    PG_RETURN_BOOL(is_set);
}

//bitmap or trans
PG_FUNCTION_INFO_V1(rb_or_trans);
Datum rb_or_trans(PG_FUNCTION_ARGS);

Datum
rb_or_trans(PG_FUNCTION_ARGS) {
    bytea * p1 = PG_GETARG_BYTEA_P(0);
    bytea * p2 = PG_GETARG_BYTEA_P(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(p1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap p1 format is error")));

    int32 card1 = (int) roaring_bitmap_get_cardinality(r1);

    roaring_bitmap_t *r2 = roaring_bitmap_portable_deserialize(VARDATA(p2));
    if (!r2) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap p2 format is error")));

#ifdef DEBUG
    int32 card1 = (int) roaring_bitmap_get_cardinality(r1);
    int32 card2 = (int) roaring_bitmap_get_cardinality(r2);

    elog(NOTICE, " *********************** rb_or_trans card1:%d card2: %d ************* ", card1, card2);
#endif

    roaring_bitmap_or_inplace(r1, r2);
    roaring_bitmap_free(r2);

    size_t expectedsize = roaring_bitmap_portable_size_in_bytes(r1);

    bytea *serializedbytes = (bytea *) palloc(VARHDRSZ + expectedsize);
    roaring_bitmap_portable_serialize(r1, VARDATA(serializedbytes));
    roaring_bitmap_free(r1);

    SET_VARSIZE(serializedbytes, VARHDRSZ + expectedsize);
    PG_RETURN_BYTEA_P(serializedbytes);
}


//bitmap and trans
PG_FUNCTION_INFO_V1(rb_and_trans);
Datum rb_and_trans(PG_FUNCTION_ARGS);

Datum
rb_and_trans(PG_FUNCTION_ARGS) {
    bytea * p1 = PG_GETARG_BYTEA_P(0);
    bytea * p2 = PG_GETARG_BYTEA_P(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(p1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap p1 format is error")));

    roaring_bitmap_t *r2 = roaring_bitmap_portable_deserialize(VARDATA(p2));
    if (!r2) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap p2 format is error")));

#ifdef DEBUG
    int32 card1 = (int) roaring_bitmap_get_cardinality(r1);
    int32 card2 = (int) roaring_bitmap_get_cardinality(r2);

    elog(NOTICE, " *********************** rb_and_trans card1:%d card2: %d ************* ", card1, card2);
#endif

    roaring_bitmap_and_inplace(r1, r2);
    roaring_bitmap_free(r2);

    size_t expectedsize = roaring_bitmap_portable_size_in_bytes(r1);

    bytea *serializedbytes = (bytea *) palloc(VARHDRSZ + expectedsize);
    roaring_bitmap_portable_serialize(r1, VARDATA(serializedbytes));
    roaring_bitmap_free(r1);

    SET_VARSIZE(serializedbytes, VARHDRSZ + expectedsize);
    PG_RETURN_BYTEA_P(serializedbytes);
}


//bitmap xor trans
PG_FUNCTION_INFO_V1(rb_xor_trans);
Datum rb_xor_trans(PG_FUNCTION_ARGS);

Datum
rb_xor_trans(PG_FUNCTION_ARGS) {
    bytea * p1 = PG_GETARG_BYTEA_P(0);
    bytea * p2 = PG_GETARG_BYTEA_P(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(p1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap p1 format is error")));

    roaring_bitmap_t *r2 = roaring_bitmap_portable_deserialize(VARDATA(p2));
    if (!r2) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap p2 format is error")));

#ifdef DEBUG
    int32 card1 = (int) roaring_bitmap_get_cardinality(r1);
    int32 card2 = (int) roaring_bitmap_get_cardinality(r2);

    elog(NOTICE, " *********************** rb_xor_trans card1:%d card2: %d ************* ", card1, card2);
#endif

    roaring_bitmap_xor_inplace(r1, r2);
    roaring_bitmap_free(r2);

    size_t expectedsize = roaring_bitmap_portable_size_in_bytes(r1);

    bytea *serializedbytes = (bytea *) palloc(VARHDRSZ + expectedsize);
    roaring_bitmap_portable_serialize(r1, VARDATA(serializedbytes));
    roaring_bitmap_free(r1);

    SET_VARSIZE(serializedbytes, VARHDRSZ + expectedsize);
    PG_RETURN_BYTEA_P(serializedbytes);
}

//bitmap Cardinality trans
PG_FUNCTION_INFO_V1(rb_cardinality_trans);
Datum rb_cardinality_trans(PG_FUNCTION_ARGS);

Datum
rb_cardinality_trans(PG_FUNCTION_ARGS) {
    bytea * p1 = PG_GETARG_BYTEA_P(0);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(p1));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap p1 format is error")));

    int32 card1 = (int) roaring_bitmap_get_cardinality(r1);
    roaring_bitmap_free(r1);

#ifdef DEBUG
    elog(NOTICE, " *********************** rb_cardinality_trans card1:%d ************* ", card1);
#endif    

    PG_RETURN_INT32(card1);
}

//bitmap minimum
PG_FUNCTION_INFO_V1(rb_minimum);
Datum rb_minimum(PG_FUNCTION_ARGS);

Datum
rb_minimum(PG_FUNCTION_ARGS) {
    bytea *data = PG_GETARG_BYTEA_P(0);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(data));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    int32 min1 = (int) roaring_bitmap_minimum(r1);

    roaring_bitmap_free(r1);
    PG_RETURN_INT32(min1);
}


//bitmap maximum
PG_FUNCTION_INFO_V1(rb_maximum);
Datum rb_maximum(PG_FUNCTION_ARGS);

Datum
rb_maximum(PG_FUNCTION_ARGS) {
    bytea *data = PG_GETARG_BYTEA_P(0);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(data));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    int32 max1 = (int) roaring_bitmap_maximum(r1);

    roaring_bitmap_free(r1);
    PG_RETURN_INT32(max1);
}

//bitmap rank
PG_FUNCTION_INFO_V1(rb_rank);
Datum rb_rank(PG_FUNCTION_ARGS);

Datum
rb_rank(PG_FUNCTION_ARGS) {
    bytea *data = PG_GETARG_BYTEA_P(0);
    int32 offsetid = PG_GETARG_INT32(1);

    roaring_bitmap_t *r1 = roaring_bitmap_portable_deserialize(VARDATA(data));
    if (!r1) ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("bitmap format is error")));

    int32 rank1 = (int) roaring_bitmap_rank(r1,offsetid);

    roaring_bitmap_free(r1);
    PG_RETURN_INT32(rank1);
}

