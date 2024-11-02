from flask import Flask, request, render_template
import requests

app = Flask(__name__)

# Store the sensor data in a dictionary
sensor_data = {'temperature': 'N/A', 'humidity': 'N/A'}

# Route to display the data on the webpage
@app.route('/')
def index():
    return render_template('index.html', data=sensor_data)

# Route to handle POST requests from ESP32 to update the data
@app.route('/update', methods=['POST'])
def update():
    global sensor_data
    data = request.get_json()
    sensor_data['temperature'] = data.get('temperature', 'N/A')
    sensor_data['humidity'] = data.get('humidity', 'N/A')
    
    # Debugging print statement to check updated sensor data
    print("Updated sensor data:", sensor_data)
    
    return "Data updated successfully!", 200

# Route to handle image upload
@app.route('/upload', methods=['POST'])
def upload():
    file = request.files['file']
    if file:
        # Here, you should process the image with the Roboflow API
        result = process_image(file)  # Process the image and get the result
        return render_template('index.html', data=sensor_data, result=result)

    return render_template('index.html', data=sensor_data, result='No image uploaded.')

# Function to process the image with Roboflow API
def process_image(file):
    api_key = 'Vr4jpCd0b9tCdvcYvZlN'  # Replace with your API key
    url = 'https://detect.roboflow.com/omcmv3/1'  # Replace with your model endpoint

    # Prepare the image file for upload
    files = {'file': file}

    try:
        # Make a request to the Roboflow API
        response = requests.post(f"{url}?api_key={api_key}", files=files)

        # Print the response
        print("Response from Roboflow:")
        print(response.json())  # This will print the full response as JSON
        
        if response.status_code == 200:
            # Handle the response as needed
            result = response.json()
            
            # Check if 'predictions' is in the result and handle accordingly
            if 'predictions' in result and len(result['predictions']) > 0:
                # Find the prediction with the highest confidence
                highest_confidence_prediction = max(result['predictions'], key=lambda x: x['confidence'])
                mushroom_class = highest_confidence_prediction.get('class', 'Unknown')  # Get the class
                
                return f"{mushroom_class} "
            else:
                return "No predictions found."
        else:
            print("Error:", response.status_code)
            print(response.text)
            return "Error processing image"

    except requests.exceptions.RequestException as e:
        # Log the error message for troubleshooting
        print(f"Error during API request: {e}")
        return "Error processing image"

    except ValueError:
        # Handle JSON decoding errors
        return "Error decoding the response"


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5001, debug=True)
