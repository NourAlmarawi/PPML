from flask import Flask, render_template, request
import subprocess
import os
import uuid

app = Flask(__name__)
UPLOAD_FOLDER = 'uploads'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

@app.route('/', methods=['GET', 'POST'])
def index():
    result = None
    if request.method == 'POST':
        uploaded_file = request.files['file']
        if uploaded_file and uploaded_file.filename.endswith('.txt'):
            unique_name = str(uuid.uuid4()) + ".txt"
            file_path = os.path.join(UPLOAD_FOLDER, unique_name)
            uploaded_file.save(file_path)
            print("✅File received:", uploaded_file.filename)

            base_dir = os.path.dirname(os.path.abspath(__file__))
            classifier_path = os.path.join(base_dir, "PPMLproject.exe")
            features_path = os.path.join(base_dir, "selected_features.txt")
            TVHamProbs_path = os.path.join(base_dir, "TVHamProbs.txt")
            TVSpamProbs_path = os.path.join(base_dir, "TVSpamProbs.txt")
            print("Checking file existence:")
            print("Classifier path:", classifier_path, "Exists:", os.path.exists(classifier_path))
            print("Features path:", features_path, "Exists:", os.path.exists(features_path))
            print("TVHamProbs path:", TVHamProbs_path, "Exists:", os.path.exists(TVHamProbs_path))
            print("TVSpamProbs path:", TVSpamProbs_path, "Exists:", os.path.exists(TVSpamProbs_path))
            try:
                output = subprocess.check_output([classifier_path, file_path, features_path, TVHamProbs_path, TVSpamProbs_path], text=True)
                result = output.strip()
            except subprocess.CalledProcessError as e:
                result = "Error: Could not classify the file."

            # os.remove(file_path)
    return render_template('index.html', result=result)

if __name__ == '__main__':
    app.run(debug=True)
