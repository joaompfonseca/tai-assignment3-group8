import os
import pandas as pd
import re
import random
import multiprocessing

DATABASE_SIGNATURE_PATH = 'data/database/signature'
QUERY_SIGNATURE_PATH = 'data/queries/raw30/signature'

# os.system(f"rm -rf {DATABASE_SIGNATURE_PATH}[0-9]*")
# os.system(f"rm -rf {QUERY_SIGNATURE_PATH}[0-9]*")
# os.system(f"rm -rf data/results/db_size_*.csv")
# exit(1)

def custom_error_handler(e):
    print(f"Error: {e}")
    exit(1)


def run_benchmark(transformation, compression, db_size):
    print(f"Running benchmark for {transformation} with {compression} compression")

    # remove cache to have a clean database
    num_cache_files = len([name for name in os.listdir(DATABASE_SIGNATURE_PATH) if name.endswith('.cache')]) 
    if num_cache_files > 0:    
        os.system(f"rm -f {DATABASE_SIGNATURE_PATH}/*.cache")
    
    command = f"bin/what_the_music -d {DATABASE_SIGNATURE_PATH} -q data/queries/{transformation}/signature{db_size} -l data/results/db_size_{db_size}_{compression}.csv -c {compression} -k 10"
    command += f" -t {DATABASE_SIGNATURE_PATH}{db_size}" # use signature foldere as temporary file location
    os.system(command)
    res = pd.read_csv(f"data/results/db_size_{db_size}_{compression}.csv", header=None)
    
    # remove the segment information from the filename
    res[0] = res[0].str.split("_split_").str[0]

    transformation_name = re.sub(r'\d+', '', transformation)
    numbers = re.findall(r'\d+', transformation)

    res["compression"] = compression
    res["transformation"] = transformation_name
    res["arg"] = numbers[0] if numbers else None
    res["db_size"] = db_size

    res.to_csv(f"data/results/db_size_{db_size}_{compression}.csv", header=False, index=False)
    os.system(f"rm -rf {DATABASE_SIGNATURE_PATH}{db_size}")
    os.system(f"rm -rf {QUERY_SIGNATURE_PATH}{db_size}")

    return res.to_dict(orient='records')


results = []
for compression in ['gzip', 'bzip2', 'lzma', 'zstd']:
    pool = multiprocessing.Pool(processes=multiprocessing.cpu_count())

    calls = []
    for db_size in [1] + list(range(5, 95, 5)):
        os.makedirs(f"{DATABASE_SIGNATURE_PATH}{db_size}")
        os.makedirs(f"{QUERY_SIGNATURE_PATH}{db_size}")

        # get a sample of db_size songs from DATABASE_SIGNATURE_PATH
        songs = os.listdir(DATABASE_SIGNATURE_PATH)
        random.shuffle(songs)
        songs = [f for f in songs if f not in {'gzip.cache', 'bzip2.cache', 'lzma.cache', 'zstd.cache'}]
        songs = songs[:db_size]

        for song in songs:
            os.system(f'cp "{DATABASE_SIGNATURE_PATH}/{song}" "{DATABASE_SIGNATURE_PATH}{db_size}/{song}"')

        for song in songs:
            # fix song names
            song = song.split('.')[0]
            song = song.replace(':', '')
            song = song.replace('?','')

            song = song.replace("Mr. Brightside. Brightside", "Mr. Brightside")
            signature_file = [f for f in os.listdir(QUERY_SIGNATURE_PATH) if f.startswith(song)]
            try:
                signature_file = signature_file[0]
            except:
                print("Error", song, os.listdir(QUERY_SIGNATURE_PATH))
                
                # cancel all processes
                pool.terminate()
                pool.join()

                exit(1)

            os.system(f'cp "{QUERY_SIGNATURE_PATH}/{signature_file}" "{QUERY_SIGNATURE_PATH}{db_size}/{song}"')

        calls.append(pool.apply_async(run_benchmark, ("raw30", compression, db_size), error_callback=custom_error_handler))

    db_size_res = [call.get() for call in calls]
    results.append([item for sublist in db_size_res for item in sublist])

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
results.to_csv("data/db_size_benchmark.csv", mode='a', header=False, index=False)