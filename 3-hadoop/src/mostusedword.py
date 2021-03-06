from mrjob.job import MRJob
from mrjob.step import MRStep
import re
from time import time

WORD_RE = re.compile(r"[\w']+")


class MostUsedWord(MRJob):

    def steps(self):
        return [
            MRStep(mapper=self.mapper_get_words,
                   combiner=self.combiner_count_words,
                   reducer=self.reducer_count_words),
            MRStep(reducer=self.reducer_find_max_word)
        ]

    def mapper_get_words(self, _, line):
        # yield each word in the line
        for word in WORD_RE.findall(line):
            yield (word.lower(), 1)

    def combiner_count_words(self, word, counts):
        # optimization: sum the words we've seen so far
        yield (word, sum(counts))

    def reducer_count_words(self, word, counts):
        # send all (num_occurrences, word) pairs to the same reducer.
        # num_occurrences is so we can easily use Python's max() function.
        yield None, (sum(counts), word)

    # discard the key; it is just None
    def reducer_find_max_word(self, _, word_count_pairs):
        # each item of word_count_pairs is (count, word),
        # so yielding one results in key=counts, value=word
        yield max(word_count_pairs)


from time import time
import sys

if __name__ == '__main__':
    start_time = time()
    MostUsedWord.run()
    end_time = time()
    elapsed_time = end_time - start_time
    sys.stderr.write(f"\n{elapsed_time} msecs\n\n")

# if __name__ == '__main__':
#     MostUsedWord.run()