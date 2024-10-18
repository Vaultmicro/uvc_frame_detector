def process_iso_data_file(input_file, output_file):
    with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
        collecting_data = False
        for line in infile:
            stripped_line = line.strip()
            if stripped_line.startswith("ISO Data [truncated]:") or stripped_line.startswith("ISO Data:"):
                if collecting_data:
                    outfile.write("\n")
                collecting_data = True
                data = line.strip().split(": ")[1]
                outfile.write(data)
            elif collecting_data:
                if 'Padding' in line or not line.strip():
                    collecting_data = False
                    outfile.write("\n")
                else:
                    outfile.write(line.strip())

input_file = 'whole.txt'
output_file = 'x.txt'
process_iso_data_file(input_file, output_file)
