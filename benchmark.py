import os
import pandas as pd
import re
import multiprocessing

DATABASE_SIGNATURE_PATH = 'data/database/signature'

available_transformations = list(os.listdir("data/queries"))
print(available_transformations)


def custom_error_handler(e):
    print(f"Error: {e}")
    exit(1)


def run_benchmark(transformation, compression):
    print(f"Running benchmark for {transformation} with {compression} compression")

    # remove cache to have a clean database
    num_cache_files = len([name for name in os.listdir(DATABASE_SIGNATURE_PATH) if name.endswith('.cache')]) 
    if num_cache_files > 0:    
        os.system(f"rm -f {DATABASE_SIGNATURE_PATH}/*.cache")
    
    os.system(f"bin/what_the_music -d {DATABASE_SIGNATURE_PATH} -q data/queries/{transformation}/signature -l data/results/{transformation}_{compression}.csv -c {compression} -k 10")
    res = pd.read_csv(f"data/results/{transformation}_{compression}.csv", header=None)
    
    # remove the segment information from the filename
    res[0] = res[0].str.split("_split_").str[0]

    transformation_name = re.sub(r'\d+', '', transformation)
    numbers = re.findall(r'\d+', transformation)

    res["compression"] = compression
    res["transformation"] = transformation_name
    res["arg"] = numbers[0] if numbers else None

    res.to_csv(f"data/results/{transformation}_{compression}.csv", header=False, index=False)

    return res.to_dict(orient='records')


# note: the queries of each transformation will be accessed 4 times, one for each compression algorithm
# as what_the_music creates temporary files, we must parallelise inside the for loop to avoid conflicts

results = []
for compression in ['gzip', 'bzip2', 'lzma', 'zstd']:
    pool = multiprocessing.Pool(processes=multiprocessing.cpu_count())

    calls = []
    for transformation in available_transformations:
        calls.append(pool.apply_async(run_benchmark, (transformation, compression), error_callback=custom_error_handler))

    compression_res = [call.get() for call in calls]
    results.append([item for sublist in compression_res for item in sublist])

    pool.close()
    pool.join()

results = [item for sublist in results for item in sublist]
results = pd.DataFrame(results)

# fix conflict names with dots to ensure a proper match
songs = [f.replace('.freqs', '') for f in os.listdir(DATABASE_SIGNATURE_PATH)]
conflict_songs = [f for f in songs if '.' in f]

for song in conflict_songs:
    song_no_dot = song.split('.')[0]
    results[0] = results[0].str.replace(song_no_dot, song)

# append to 'benchmark.csv'
results.to_csv("data/benchmark.csv", mode='a', header=False, index=False)