from pytube import Playlist
import string

def normalise_string(text):
    # Remove special characters (keep letters, numbers, and underscores)
    allowed_chars = set(string.ascii_letters + string.digits + ' ')
    normalised_text = ''.join([char for char in text if char in allowed_chars])
    
    # remove undesired words
    normalised_text = normalised_text.replace("Official", "")
    normalised_text = normalised_text.replace("Video", "")
    normalised_text = normalised_text.replace("Lyrics", "")
    normalised_text = normalised_text.replace("Lyric", "")
    normalised_text = normalised_text.replace("Music", "")
    normalised_text = normalised_text.replace("Remix", "")
    normalised_text = normalised_text.replace("remix", "")

    # Replace spaces with underscores
    normalised_text = normalised_text.replace(" ", "_")

    # remove multiple underscores
    normalised_text = normalised_text.replace("___", "_")
    normalised_text = normalised_text.replace("__", "_")

    # remove underscore at the end
    if normalised_text[-1] == "_":
        normalised_text = normalised_text[:-1]

    return normalised_text

# Get the playlist URL
playlist_url = "https://www.youtube.com/watch?v=Oa_RSwwpPaA&list=PLBMPbZf5dAhOfbAfgLiRv4V726aw3kJvu"

# Create a playlist object
playlist = Playlist(playlist_url)

for video in playlist.videos:
    filename = f"data/{normalise_string(video.author)}-{normalise_string(video.title)}.mp3"
    print(f"Downloading {filename}...")

    # Download the video
    video.streams.filter(only_audio=True).first().download(filename=filename)
    print(f"{filename} downloaded successfully!")
