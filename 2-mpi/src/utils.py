import time


def timeit(proc):
    """Returns execution time of the nullary function in seconds"""
    beg = time.time()
    proc()
    end = time.time()
    return end - beg
