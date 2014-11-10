========================================================================
                       compression experiments
========================================================================

Some experiments with lossless compression.

Lookup
--------------------------------------------------

Split input file to blocks, count distinct bytes in a block (``n``),
encode bytes with fixed-width words of size ``k = ceil(lg(n))`` bits.

This gives compression 20% for plain texts data, as ``k`` usually
is 6 or 7.


N-grams
--------------------------------------------------

Count all 2-grams, 3-grams and 4-grams. Save list of most frequently
occurring grams as dictionary of limited size -- it's not larger
than ``2^n - 256``, where ``n`` is predefined value (greater than 8).
Uncompressed values and dictionary indices are stored on
fixed-width n-bit words.

This compression is also good for textual data, for ``n=12`` 
compression ratio could be 40-50%.


Other
--------------------------------------------------

You may also find interesting article `Encoding array of unsigned integers`__.

__ http://0x80.pl/articles/integer-sequence-encoding.html

