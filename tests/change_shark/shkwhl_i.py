import time
from datetime import datetime

def convert_to_monotonic(epoch_time):
    # Get current system time and monotonic time
    current_system_time = time.time()  # Equivalent to std::chrono::system_clock::now()
    current_monotonic_time = time.monotonic()  # Equivalent to std::chrono::steady_clock::now()
    
    # Calculate the difference between the current system time and the given epoch time
    time_difference = current_system_time - epoch_time
    
    # Calculate the monotonic time equivalent
    monotonic_time = current_monotonic_time - time_difference
    return monotonic_time

def process_iso_data_file(input_file, output_file):
    with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
        collecting_data = False
        monotonic_time = None  # Monotonic time for the Epoch Arrival Time

        for line in infile:
            stripped_line = line.strip()

            # Look for the Epoch Arrival Time line
            if stripped_line.startswith("Epoch Arrival Time:"):
                # Extract the Epoch Arrival Time value
                epoch_time = float(stripped_line.split(": ")[1])
                
                # Convert Unix epoch time to monotonic time
                monotonic_time = convert_to_monotonic(epoch_time)
                
                # Write the converted monotonic time to the output file
                outfile.write(f"Converted Monotonic Time: {monotonic_time} seconds\n")
            
            # ISO Data handling
            if stripped_line.startswith("ISO Data [truncated]:") or stripped_line.startswith("ISO Data:") or stripped_line.startswith("ISO Data […]"):
                # if collecting_data:
                #     outfile.write("\n")
                collecting_data = True
                data = line.split(": ")[1]
                outfile.write(data)
            elif collecting_data:
                if 'Padding' in line or not stripped_line:
                    collecting_data = False
                    # outfile.write("\n")
                else:
                    outfile.write(stripped_line)

input_file = 'logi_iso_shk.txt'
output_file = 'logi_new.txt'
process_iso_data_file(input_file, output_file)
