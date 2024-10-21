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

def process_bulk_data_file(input_file, output_file):
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
                
            if stripped_line.startswith("Frame Length: 16448 bytes (131584 bits)"):
                max_frame_length_16448 = True
            
            # BULK Data handling
            if stripped_line.startswith("Leftover Capture Data [truncated]:") or stripped_line.startswith("Leftover Capture Data:") or stripped_line.startswith("Leftover Capture Data […]:"):
                if max_frame_length_16448 == True:
                    max_frame_length_16448 = False
                    collecting_data = True
                    data = line.split(": ")[1]
                    outfile.write(data)

                    
                elif max_frame_length_16448 == False:
                    max_frame_length_16448 = True
                    collecting_data = False
                    
                # if collecting_data:
                #     outfile.write("\n")
            elif collecting_data:
                if 'Padding' in line or not stripped_line:
                    collecting_data = False
                    # outfile.write("\n")
                else:
                    outfile.write(stripped_line)

input_file = 'insta_bulk_shk.txt'
output_file = 'insta_new.txt'
process_bulk_data_file(input_file, output_file)
