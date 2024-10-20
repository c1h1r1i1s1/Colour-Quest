import requests
import json
import basetest

# Set the API endpoint and API key
url = "https://texttospeech.googleapis.com/v1/text:synthesize"
api_key = "AIzaSyDhxVkeF1MJvMuGrjT4BcovRg8HviN1LkE"  # Replace this with your Google Cloud API key

# Request parameters
headers = {
    "Content-Type": "application/json"
}

data = {
    "input": {
        "text": "Hello, world!"  # The text to be converted to speech
    },
    "voice": {
        "languageCode": "en-US",
        "name": "en-US-Wavenet-D",  # Ensure you use a valid voice name
        "ssmlGender": "MALE"
    },
    "audioConfig": {
        "audioEncoding": "MP3"
    }
}

# Make the request
response = requests.post(url, headers=headers, params={"key": api_key}, data=json.dumps(data))

# Handle the response
if response.status_code == 200:
    response_data = response.json()

    # Check if audio content is present
    if 'audioContent' in response_data:
        # Decode Base64 audio data and save it as an MP3 file
        audio_content = response_data['audioContent']
        audio_data = base64.b64decode(audio_content)

        with open("output.mp3", "wb") as audio_file:
            audio_file.write(audio_data)

        print("Audio saved as output.mp3")
    else:
        print("No audio content found in the response.")
else:
    print(f"Request failed with status code {response.status_code}.")
    print(f"Error response: {response.text}")
