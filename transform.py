import os
import random
import sox
import wave
import multiprocessing

DATABASE_AUDIO_PATH = 'data/database/audio'
BINARY_GET_MAX_FREQS_PATH = 'GetMaxFreqs/bin/GetMaxFreqs'

# delete files from previous runs
os.system("rm -rf data/queries/*")


def create_necessary_folders(transformation):
    os.makedirs(f"data/queries/{transformation}", exist_ok=True)
    os.makedirs(f"data/queries/{transformation}/audio", exist_ok=True)
    os.makedirs(f"data/queries/{transformation}/signature", exist_ok=True)


def generate_segment(filename, sample_duration):
    # get duration of file
    with wave.open(os.path.join(DATABASE_AUDIO_PATH, filename), 'r') as audio:
        duration = audio.getnframes() / audio.getframerate()

    start = random.randint(30, int(duration) - sample_duration)
    end = start + sample_duration

    print(f'Creating segment of file {filename} ({int(duration//60)}:{int(duration%60):02d}) | Duration={sample_duration}s, from {start}s to {end}s')
    return start, end


def raw_job(sample_duration):
    print(f"Starting raw job with sample duration {sample_duration}")
    create_necessary_folders(f"raw{sample_duration}")

    QUERIES_AUDIO_PATH = f'data/queries/raw{sample_duration}/audio'
    QUERIES_SIGNATURE_PATH = f'data/queries/raw{sample_duration}/signature'

    for filename in os.listdir(DATABASE_AUDIO_PATH):
        # get segment
        start, end = generate_segment(filename, sample_duration)
        
        # create a sox transformer
        transformer = sox.Transformer()
        transformer.trim(start, end)

        # create a new file with the segment
        segment_filename = f"{filename.split('.')[0]}_split_{start}_{end}.wav"
        transformer.build(os.path.join(DATABASE_AUDIO_PATH, filename), os.path.join(QUERIES_AUDIO_PATH, segment_filename))

        # get frequencies
        freq_segment_filename = f"{filename.split('.')[0]}_split_{start}_{end}.freqs"
        command = f'{BINARY_GET_MAX_FREQS_PATH} -w "{os.path.join(QUERIES_SIGNATURE_PATH, freq_segment_filename)}" "{os.path.join(QUERIES_AUDIO_PATH, segment_filename)}"'
        os.system(command)


def noise_job(noise_type, vol):
    sample_duration = 30

    print(f"Starting {noise_type} job with sample duration {sample_duration}")
    foldername = f"{noise_type}{vol}"
    create_necessary_folders(foldername)

    QUERIES_AUDIO_PATH = f'data/queries/{foldername}/audio'
    QUERIES_SIGNATURE_PATH = f'data/queries/{foldername}/signature'

    for filename in os.listdir(DATABASE_AUDIO_PATH):
        # get segment
        start, end = generate_segment(filename, sample_duration)

        # create a sox transformer with noise
        transformer = sox.Transformer()
        transformer.trim(start, end)
        
        # create a new file with the segment
        temp_segment_filename = f"{filename.split('.')[0]}_split_{start}_{end}_temp.wav"
        transformer.build(os.path.join(DATABASE_AUDIO_PATH, filename), os.path.join(QUERIES_AUDIO_PATH, temp_segment_filename))

        # add white noise
        noise_filename = f"data/noise/{noise_type}/{noise_type}{vol}.wav"
        segment_filename = f"{filename.split('.')[0]}_split_{start}_{end}.wav"
        os.system(f'sox "{os.path.join(QUERIES_AUDIO_PATH, temp_segment_filename)}" -m "{noise_filename}" "{os.path.join(QUERIES_AUDIO_PATH, segment_filename)}"')
        
        # remove temp_segment_filename
        os.system(f'rm "{os.path.join(QUERIES_AUDIO_PATH, temp_segment_filename)}"')

        # get frequencies
        freq_segment_filename = f"{filename.split('.')[0]}_split_{start}_{end}.freqs"
        command = f'{BINARY_GET_MAX_FREQS_PATH} -w "{os.path.join(QUERIES_SIGNATURE_PATH, freq_segment_filename)}" "{os.path.join(QUERIES_AUDIO_PATH, segment_filename)}"'
        os.system(command)


def whitenoise_job(vol):
    noise_job("whitenoise", vol)


def pinknoise_job(vol):
    noise_job("pinknoise", vol)


def effect_job(effect, args):
    sample_duration = 30

    print(f"Starting reverb job with sample duration {sample_duration}")
    foldername = f"{effect}{args[0]}"
    create_necessary_folders(foldername)

    QUERIES_AUDIO_PATH = f'data/queries/{foldername}/audio'
    QUERIES_SIGNATURE_PATH = f'data/queries/{foldername}/signature'

    for filename in os.listdir(DATABASE_AUDIO_PATH):
        # get segment
        start, end = generate_segment(filename, sample_duration)

        # create a sox transformer with noise
        transformer = sox.Transformer()
        transformer.trim(start, end)
        # get effect function from transformer
        effect_func = getattr(transformer, effect)
        effect_func(*args)

        # create a new file with the segment
        segment_filename = f"{filename.split('.')[0]}_split_{start}_{end}.wav"
        transformer.build(os.path.join(DATABASE_AUDIO_PATH, filename), os.path.join(QUERIES_AUDIO_PATH, segment_filename))

        # get frequencies
        freq_segment_filename = f"{filename.split('.')[0]}_split_{start}_{end}.freqs"
        command = f'{BINARY_GET_MAX_FREQS_PATH} -w "{os.path.join(QUERIES_SIGNATURE_PATH, freq_segment_filename)}" "{os.path.join(QUERIES_AUDIO_PATH, segment_filename)}"'
        os.system(command)


def ambience_job():
    sample_duration = 30

    print(f"Starting ambience job with sample duration {sample_duration}")
    foldername = "ambience"
    create_necessary_folders(foldername)

    QUERIES_AUDIO_PATH = f'data/queries/{foldername}/audio'
    QUERIES_SIGNATURE_PATH = f'data/queries/{foldername}/signature'

    for filename in os.listdir(DATABASE_AUDIO_PATH):
        # get segment
        start, end = generate_segment(filename, sample_duration)

        # create a sox transformer with noise
        transformer = sox.Transformer()
        transformer.trim(start, end)

        # create a new file with the segment
        temp_segment_filename = f"{filename.split('.')[0]}_split_{start}_{end}_temp.wav"
        transformer.build(os.path.join(DATABASE_AUDIO_PATH, filename), os.path.join(QUERIES_AUDIO_PATH, temp_segment_filename))

        # add ambience sound
        ambience_filename = "data/noise/cafe-ambience.wav"
        segment_filename = f"{filename.split('.')[0]}_split_{start}_{end}.wav"
        os.system(f'sox "{os.path.join(QUERIES_AUDIO_PATH, temp_segment_filename)}" -m "{ambience_filename}" "{os.path.join(QUERIES_AUDIO_PATH, segment_filename)}"')

        # remove temp_segment_filename
        os.system(f'rm "{os.path.join(QUERIES_AUDIO_PATH, temp_segment_filename)}"')

        # get frequencies
        freq_segment_filename = f"{filename.split('.')[0]}_split_{start}_{end}.freqs"
        command = f'{BINARY_GET_MAX_FREQS_PATH} -w "{os.path.join(QUERIES_SIGNATURE_PATH, freq_segment_filename)}" "{os.path.join(QUERIES_AUDIO_PATH, segment_filename)}"'
        os.system(command)


def custom_error_handler(e):
    print(f"Error: {e}")


transformations = {
    "raw5": (raw_job, (5,)),
    "raw10": (raw_job, (10,)),
    "raw15": (raw_job, (15,)),
    "raw30": (raw_job, (30,)),
    "raw60": (raw_job, (60,)),
    "whitenoise_10": (whitenoise_job, (10,)),
    "whitenoise_20": (whitenoise_job, (20,)),
    "whitenoise_30": (whitenoise_job, (30,)),
    "whitenoise_40": (whitenoise_job, (40,)),
    "whitenoise_50": (whitenoise_job, (50,)),
    "whitenoise_60": (whitenoise_job, (60,)),
    "whitenoise_70": (whitenoise_job, (70,)),
    "whitenoise_80": (whitenoise_job, (80,)),
    "whitenoise_90": (whitenoise_job, (90,)),
    "whitenoise_100": (whitenoise_job, (100,)),
    "pinknoise_10": (pinknoise_job, (10,)),
    "pinknoise_20": (pinknoise_job, (20,)),
    "pinknoise_30": (pinknoise_job, (30,)),
    "pinknoise_40": (pinknoise_job, (40,)),
    "pinknoise_50": (pinknoise_job, (50,)),
    "pinknoise_60": (pinknoise_job, (60,)),
    "pinknoise_70": (pinknoise_job, (70,)),
    "pinknoise_80": (pinknoise_job, (80,)),
    "pinknoise_90": (pinknoise_job, (90,)),
    "pinknoise_100": (pinknoise_job, (100,)),
    "reverb_10": (effect_job, ("reverb", (10,))),
    "reverb_20": (effect_job, ("reverb", (20,))),
    "reverb_30": (effect_job, ("reverb", (30,))),
    "reverb_40": (effect_job, ("reverb", (40,))),
    "reverb_50": (effect_job, ("reverb", (50,))),
    "reverb_60": (effect_job, ("reverb", (60,))),
    "reverb_70": (effect_job, ("reverb", (70,))),
    "reverb_80": (effect_job, ("reverb", (80,))),
    "reverb_90": (effect_job, ("reverb", (90,))),
    "reverb_100": (effect_job, ("reverb", (100,))),
    "reverse": (effect_job, ("reverse", ())),
    "ambience": (ambience_job, ())
}

# use a pool of processes
pool = multiprocessing.Pool(processes=multiprocessing.cpu_count())
for func, args in transformations.values():
    pool.apply_async(func, args, error_callback=custom_error_handler)

pool.close()
pool.join()
