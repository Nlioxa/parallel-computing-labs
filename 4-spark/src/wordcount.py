import sys
from pyspark.sql import SparkSession


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: wordcount <file>", file=sys.stderr)
        sys.exit(-1)

    spark_session = SparkSession\
        .builder\
        .appName("Lab3WordCount")\
        .getOrCreate()

    lines = spark_session\
        .read.text(sys.argv[1])\
        .rdd.map(lambda r: r[0])
    
    words_counts = lines.flatMap(lambda x: x.split(' ')) \
                  .map(lambda x: (x, 1)) \
                  .reduceByKey(lambda a, b: a + b) \
                  .sortBy(lambda a:-a[1]) \
    
    output = words_counts.take(1)
    
    for (word, count) in output:
        print(f"{word}: {i}")

    spark.stop()
