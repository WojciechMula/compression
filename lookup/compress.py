from math import log, ceil

def bytes(bits):
    return (bits + 7)/8


class Block:
    def __init__(self, data):
        self.data = data

    def encoded_size(self):
        chars = set(self.data)

        n = len(chars)
        k = int(ceil(log(n, 2)))

        b = map(ord, chars)
        a, b = min(b), max(b)
        r    = (b+7/8) - a/8

        return 4 + r/8 + bytes(k*len(self.data))

    def unencoded_size(self):
        return len(self.data)

    def size(self):
        return min(2 + self.unencoded_size(), self.encoded_size())

def main(path):
    with open(path) as f:
        data = f.read()

    for block_size in [128, 256, 512, 256*3, 1024, 1024+512, 1024*2, 1024*3, 1024*4, 100000000]:
        compressed   = 0
        uncompressed = len(data)

        for i in xrange(0, len(data), block_size):
            b = Block(data[i:i+block_size])

            compressed += b.size()

        ratio = 100*float(uncompressed - compressed)/uncompressed
        if False:
            print 
            print "block size:   %d" % block_size
            print "uncompressed: %d" % uncompressed
            print "compressed:   %d" % compressed
            print "ratio:        %.2f%%" % ratio
        else:
            print "%5d %.2f%%" % (block_size, ratio)
        

if __name__ == '__main__':
    from sys import argv
    for path in argv[1:]:
        main(path)
