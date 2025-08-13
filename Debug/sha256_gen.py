import hashlib
import os
import json
import re
import shutil


def list_bin_files():
    """List .bin files in current directory and prompt for .bin and .json file selection"""
    current_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.join(current_dir, 'Firmware')
    
    bin_files = [f for f in os.listdir(output_dir) if f.lower().endswith('.bin')]
    if not bin_files:
        print("No .bin files found in the current directory!")
    else:
        print("Available .bin files:")
        for index, file in enumerate(bin_files, start=1):
            print(f"{index}: {file}")
    
    # Select .bin file
    while True:
        try:
            if bin_files:
                choice = input("Select a .bin file number or enter file path: ")
            else:
                choice = input("Enter .bin file path: ")
            
            # Check if input is a number
            try:
                choice_num = int(choice)
                if not bin_files:
                    print("Invalid choice. Please enter a valid .bin file path!")
                elif 1 <= choice_num <= len(bin_files):
                    bin_file = os.path.join(output_dir, bin_files[choice_num - 1])
                    break
                else:
                    print(f"Invalid choice. Please select 1 to {len(bin_files)} or a valid .bin file path.")
            except ValueError:
                # Check if input is a valid file path
                file_path = choice.strip().strip('"\' ')
                if os.path.isfile(file_path) and file_path.lower().endswith('.bin') and os.path.exists(file_path):
                    bin_file = file_path
                    break
                print("Invalid file path or file is not a .bin file.")
        except KeyboardInterrupt:
            print("\nOperation cancelled by user.")
            return None, None
    
    return bin_file


def calculate_sha256(file_path):
    sha256_hash = hashlib.sha256()
    try:
        with open(file_path, "rb") as file:
            for byte_block in iter(lambda: file.read(4096), b""):
                sha256_hash.update(byte_block)
        return sha256_hash.hexdigest(), None
    except FileNotFoundError:
        return None, "Error: File not found"
    except Exception as e:
        return None, f"Error: {str(e)}"

def get_file_size(file_path):
    try:
        return os.path.getsize(file_path), None
    except FileNotFoundError:
        return None, "Error: File not found!"
    except Exception as e:
        return None, f"Error: {str(e)}!"

def save_to_json(file_path, version, hash_value, file_size, output_file):
    file_name = os.path.basename(file_path)
    data = [{
        "file_name": file_name,
        "version": version,
        "sha256_hash": hash_value,
        "file_size": file_size
    }]
    
    try:
        with open(output_file, "w") as json_file:
            json.dump(data, json_file, indent=2)
    except Exception as e:
        return f"Error saving JSON file: {str(e)}"
    return None

def main():

    file_path = list_bin_files()

    while True:
        version = input("Enter version (e.g., 1.0.0): ")
        if bool(re.match(r"^\d+\.\d+\.\d+$", version)):
            break
        print("Error: Version must follow the format m.n.p (e.g., 1.0.0)!")
    
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.join(script_dir, "Output")
    os.makedirs(output_dir, exist_ok=True)
    
    file_name = os.path.basename(file_path)
    output_json = os.path.join(output_dir, os.path.splitext(file_name)[0] + '.json')
    output_bin = os.path.join(output_dir, file_name)

    hash_value, sha_error = calculate_sha256(file_path)
    if hash_value:
        file_size, fs_eror = get_file_size(file_path)
        if file_size is not None:
            error = save_to_json(file_path, version, hash_value, file_size, output_json)
            if error:
                print(error)
            else:
                shutil.copy2(file_path, output_bin)
                print(f"SHA-256 hash: {hash_value}")
                print(f"File size: {file_size} bytes")
                print(f"Saved to {output_json}!")
                print(f"Copied .bin file to {output_bin}")
        else: print(fs_eror)
    else: print(sha_error)
if __name__ == "__main__":
    main()