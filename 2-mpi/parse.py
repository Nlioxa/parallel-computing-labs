import random
import string


__all__ = ["find_pattern", "generate_text"]


def __find_str(text, pattern):
    """Returns the first location of the pattern in the text"""
    index = 0
    if pattern in text:
        fst_chr = pattern[0]
        for chr in text:
            if chr == fst_chr and pattern == text[index : index + len(pattern)]:
                return index
            index += 1
    return -1


def find_pattern(text, pattern, count=-1):
    """Find 'count' of places where the pattern is located in the text"""
    locations = list()

    idx = 0
    pad = 1

    while count == -1 or len(locations) < count:
        #
        # find another location in the text
        #
        i = __find_str(text[idx:], pattern)
        if i == -1:
            break
        #
        # note the location and the pattern with some neighbour characters around
        #
        beg = idx + i
        end = idx + i + len(pattern)
        locations.append((text[max(0, beg - pad) : min(len(text) - 1, end + pad)], beg))
        #
        idx = end

    return locations


def generate_text(size: int, alphabet: str = string.ascii_letters):
    """Returns a string of length 'size' composed from a random combination of characters in 'alphabet'"""
    return "".join(random.choice(alphabet) for x in range(size))
