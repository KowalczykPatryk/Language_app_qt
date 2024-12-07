from flask import Flask, request, jsonify
import json
import ollama

app = Flask(__name__)

@app.route('/prompt/', methods=['POST'])
def get_request():
    # Parse the JSON request
    data = request.get_json()
    front_side = data.get('front_side')
    back_side = data.get('back_side')
    response_message = prompt_olama(f'Please create a sentence using the word {front_side}, but output it with this word replaces by _ and output only this sentence')
    response = {"response": response_message}
    return jsonify(response)

def prompt_olama(prompt_text):
    response = ollama.chat(
        model="llama3",
        messages=[
            {
                "role": "user",
                "content": prompt_text,
            },
        ],
    )
    return response["message"]["content"]

if __name__ == '__main__':
    app.run(debug=True, host='127.0.0.1', port=8000)
