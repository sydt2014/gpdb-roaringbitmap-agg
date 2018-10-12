# gpdb-roaringbitmap
RoaringBitmap extension for greenplum-db


# Introduction
Roaring bitmaps are compressed bitmaps which tend to outperform conventional compressed bitmaps such as WAH, EWAH or Concise. In some instances, roaring bitmaps can be hundreds of times faster and they often offer significantly better compression. They can even be faster than uncompressed bitmaps. More information https://github.com/RoaringBitmap/CRoaring .

# Build

If $GPHOME is /usr/local/gpdb .

```
gcc -march=native -O3 -std=c11 -Wall -Wpointer-arith  -Wendif-labels -Wformat-security -fno-strict-aliasing -fwrapv -fexcess-precision=standard -fno-aggressive-loop-optimizations -Wno-unused-but-set-variable -Wno-address -fpic -D_GNU_SOURCE -I/usr/local/gpdb/include/postgresql/server -I/usr/local/gpdb/include/postgresql/internal -c -o roaringbitmap.o roaringbitmap.c

gcc -O3 -std=gnu99 -Wall -Wpointer-arith  -Wendif-labels -Wformat-security -fno-strict-aliasing -fwrapv -fexcess-precision=standard -fno-aggressive-loop-optimizations -Wno-unused-but-set-variable -Wno-address  -fpic -shared --enable-new-dtags -o roaringbitmap.so roaringbitmap.o

cp ./roaringbitmap.so /usr/local/gpdb/lib/postgresql/

psql -f ./roaringbitmap.sql
```

# Usage

## Create table
```
CREATE TABLE t1 (id integer, bitmap roaringbitmap);
```

## Build bitmap
```
INSERT INTO t1 SELECT 1,RB_BUILD(ARRAY[1,2,3,4,5,6,7,8,9,200]);

INSERT INTO t1 SELECT 2,RB_BUILD_AGG(e) FROM GENERATE_SERIES(1,100) e;
```

## Bitmap Calculation (OR, AND, XOR, ANDNOT)
```
SELECT RB_OR(a.bitmap,b.bitmap) FORM (SELECT bitmap FROM t1 WHERE id = 1) AS a,(SELECT bitmap FROM t1 WHERE id = 2) AS b;
```

## Bitmap Aggregate (OR, AND, XOR, BUILD)
```
SELECT RB_OR_AGG(bitmap) FROM t1;
SELECT RB_AND_AGG(bitmap) FORM t1;
SELECT RB_XOR_AGG(bitmap) FROM t1;
SELECT RB_BUILD_AGG(e) FROM GENERATE_SERIES(1,100) e;
```

## Cardinality
```
SELECT RB_CARDINALITY(bitmap) FROM t1;
```

## Bitmap to SETOF integer
```
SELECT RB_ITERATE(bitmap) FROM t1 WHERE id = 1;
```

## Function List
<table>
    <thead>
           <th>Function</th>
           <th>Input</th>
           <th>Output</th>
           <th>Desc</th>
           <th>Example</th>
    </thead>
    <tr>
        <td><code>rb_build</code></td>
        <td><code>integer[]</code></td>
        <td><code>roaringbitmap</code></td>
        <td>Build a roaringbitmap from integer array.</td>
        <td><code>rb_build('{1,2,3,4,5}')</code></td>
    </tr>
    <tr>
        <td><code>rb_and</code></td>
        <td><code>roraingbitmap,roaringbitmap</code></td>
        <td><code>roaringbitmap</code></td>
        <td>Two roaringbitmap and calculation.</td>
        <td><code>rb_and(rb_build('{1,2,3}'),rb_build('{3,4,5}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_or</code></td>
        <td><code>roraingbitmap,roaringbitmap</code></td>
        <td><code>roaringbitmap</code></td>
        <td>Two roaringbitmap or calculation.</td>
        <td><code>rb_or(rb_build('{1,2,3}'),rb_build('{3,4,5}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_xor</code></td>
        <td><code>roraingbitmap,roaringbitmap</code></td>
        <td><code>roaringbitmap</code></td>
        <td>Two roaringbitmap xor calculation.</td>
        <td><code>rb_xor(rb_build('{1,2,3}'),rb_build('{3,4,5}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_andnot</code></td>
        <td><code>roraingbitmap,roaringbitmap</code></td>
        <td><code>roaringbitmap</code></td>
        <td>Two roaringbitmap andnot calculation.</td>
        <td><code>rb_andnot(rb_build('{1,2,3}'),rb_build('{3,4,5}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_cardinality</code></td>
        <td><code>roraingbitmap</code></td>
        <td><code>integer</code></td>
        <td>Retrun roaringbitmap cardinality.</td>
        <td><code>rb_cardinality(rb_build('{1,2,3,4,5}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_and_cardinality</code></td>
        <td><code>roraingbitmap,roaringbitmap</code></td>
        <td><code>integer</code></td>
        <td>Two roaringbitmap and calculation, return cardinality.</td>
        <td><code>rb_and_cardinality(rb_build('{1,2,3}'),rb_build('{3,4,5}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_or_cardinality</code></td>
        <td><code>roraingbitmap,roaringbitmap</code></td>
        <td><code>integer</code></td>
        <td>Two roaringbitmap or calculation, return cardinality.</td>
        <td><code>rb_or_cardinality(rb_build('{1,2,3}'),rb_build('{3,4,5}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_xor_cardinality</code></td>
        <td><code>roraingbitmap,roaringbitmap</code></td>
        <td><code>integer</code></td>
        <td>Two roaringbitmap xor calculation, return cardinality.</td>
        <td><code>rb_xor_cardinality(rb_build('{1,2,3}'),rb_build('{3,4,5}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_andnot_cardinality</code></td>
        <td><code>roraingbitmap,roaringbitmap</code></td>
        <td><code>integer</code></td>
        <td>Two roaringbitmap andnot calculation, return cardinality.</td>
        <td><code>rb_andnot_cardinality(rb_build('{1,2,3}'),rb_build('{3,4,5}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_is_empty</code></td>
        <td><code>roraingbitmap</code></td>
        <td><code>boolean</code></td>
        <td>Check if roaringbitmap is empty.</td>
        <td><code>rb_is_empty(rb_build('{1,2,3,4,5}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_equals</code></td>
        <td><code>roraingbitmap,roaringbitmap</code></td>
        <td><code>boolean</code></td>
        <td>Check two roaringbitmap are equal.</td>
        <td><code>rb_equals(rb_build('{1,2,3}'),rb_build('{3,4,5}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_intersect</code></td>
        <td><code>roraingbitmap,roaringbitmap</code></td>
        <td><code>boolean</code></td>
        <td>Check two roaringbitmap are intersect.</td>
        <td><code>rb_intersect(rb_build('{1,2,3}'),rb_build('{3,4,5}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_remove</code></td>
        <td><code>roraingbitmap,integer</code></td>
        <td><code>roraingbitmap</code></td>
        <td>Remove the specified offset from roaringbitmap.</td>
        <td><code>rb_remove(rb_build('{1,2,3}'),3)</code></td>
    </tr>
    <tr>
        <td><code>rb_flip</code></td>
        <td><code>roraingbitmap,integer,integer</code></td>
        <td><code>roraingbitmap</code></td>
        <td>Flip the specified offsets range (not include the end) from roaringbitmap.</td>
        <td><code>rb_flip(rb_build('{1,2,3}'),2,3)</code></td>
    </tr>
    <tr>
        <td><code>rb_minimum</code></td>
        <td><code>roraingbitmap</code></td>
        <td><code>integer</code></td>
        <td>Return the smallest offset in roaringbitmap. Return UINT32_MAX if the bitmap is empty.</td>
        <td><code>rb_minimum(rb_build('{1,2,3}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_maximum</code></td>
        <td><code>roraingbitmap</code></td>
        <td><code>integer</code></td>
        <td>Return the greatest offset in roaringbitmap. Return 0 if the bitmap is empty.</td>
        <td><code>rb_maximum(rb_build('{1,2,3}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_rank</code></td>
        <td><code>roraingbitmap,integer</code></td>
        <td><code>integer</code></td>
        <td>Return the number of offsets that are smaller or equal to the specified offset.</td>
        <td><code>rb_rank(rb_build('{1,2,3}'),3)</code></td>
    </tr>
</table>

## Aggregation List
<table>
    <thead>
           <th>Function</th>
           <th>Input</th>
           <th>Output</th>
           <th>Desc</th>
           <th>Example</th>
    </thead>
    <tr>
        <td><code>rb_build_agg</code></td>
        <td><code>integer</code></td>
        <td><code>roraingbitmap</code></td>
        <td>Build a roaringbitmap from a integer set.</td>
        <td><code>rb_build_agg(1)</code></td>
    </tr> 
    <tr>
        <td><code>rb_or_agg</code></td>
        <td><code>roraingbitmap</code></td>
        <td><code>roraingbitmap</code></td>
        <td>Or Aggregate calculations from a roraingbitmap set.</td>
        <td><code>rb_or_agg(rb_build('{1,2,3}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_and_agg</code></td>
        <td><code>roraingbitmap</code></td>
        <td><code>roraingbitmap</code></td>
        <td>And Aggregate calculations from a roraingbitmap set.</td>
        <td><code>rb_and_agg(rb_build('{1,2,3}'))</code></td>
    </tr>
    <tr>
        <td><code>rb_xor_agg</code></td>
        <td><code>roraingbitmap</code></td>
        <td><code>roraingbitmap</code></td>
        <td>Xor Aggregate calculations from a roraingbitmap set.</td>
        <td><code>rb_xor_agg(rb_build('{1,2,3}'))</code></td>
    </tr>    
    <tr>
        <td><code>rb_or_cardinality_agg</code></td>
        <td><code>roraingbitmap</code></td>
        <td><code>integer</code></td>
        <td>Or Aggregate calculations from a roraingbitmap set, return cardinality.</td>
        <td><code>rb_or_cardinality_agg(rb_build('{1,2,3}'))</code></td>
    </tr>   
    <tr>
        <td><code>rb_and_cardinality_agg</code></td>
        <td><code>roraingbitmap</code></td>
        <td><code>integer</code></td>
        <td>And Aggregate calculations from a roraingbitmap set, return cardinality.</td>
        <td><code>rb_and_cardinality_agg(rb_build('{1,2,3}'))</code></td>
    </tr>  
    <tr>
        <td><code>rb_xor_cardinality_agg</code></td>
        <td><code>roraingbitmap</code></td>
        <td><code>integer</code></td>
        <td>Xor Aggregate calculations from a roraingbitmap set, return cardinality.</td>
        <td><code>rb_xor_cardinality_agg(rb_build('{1,2,3}'))</code></td>
    </tr>
</table>