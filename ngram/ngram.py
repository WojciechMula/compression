from sys import stderr

def bytes(bits):
    return (bits + 7)/8

gram_size = [5, 4, 3, 2]
gram_size = [5, 4, 3, 2]

def scan_trigrams(data):
    
    lookup = {}

    for i in xrange(len(data)):
        for k in gram_size:
            gram = data[i:i+k]
            lookup[gram] = lookup.get(gram, 0) + 1

    return lookup

class Local:
    pass

def encode(data, bits, lookup):
    l = Local()
    l.size = 0
    
    def save(start, end):
        #stderr.write(data[start:end])
        pass

    def emit_uncompressed(start, end):
        n = end - start + 1
        save(start, end + 1)
        if n == 0:
            return 0
        
        raw_size  = bits * n
        copy_size = bits + (n+1)*8
        #if copy_size < raw_size:
        #    print "xxx", copy_size, raw_size

        return min(raw_size, copy_size)


    i = 0
    last = 0
    while i < len(data):

        def get_gram():
            for k in gram_size:
                gram = data[i:i+k]
                if gram in lookup:
                    return gram

        gram = get_gram()
        if gram is not None:
            l.size += emit_uncompressed(last, i - 1)
            l.size += bits
            save(i, i + len(gram))
            i += len(gram)
            last = i
            continue
        else:
            i += 1
    
    l.size += emit_uncompressed(last, i)

    return sum(map(len, lookup)) + bytes(l.size)


def main(path):
    with open(path) as f:
        data = f.read()

    trigrams = list((item for item in scan_trigrams(data).iteritems()))
    trigrams.sort(key=lambda item: item[1], reverse=True)

    for bits in [10, 11, 12]:
        n = 2**bits - 256 - 1

        uncompressed = len(data)
        compressed   = encode(data, bits, dict(trigrams[:n]))

        ratio = 100*float(uncompressed - compressed)/uncompressed
        print 
        print "bits:         %d" % bits
        print "uncompressed: %d" % uncompressed
        print "compressed:   %d" % compressed
        print "ratio:        %.2f%%" % ratio


if __name__ == '__main__':
    from sys import argv
    for path in argv[1:]:
        main(path)
