              
def process_hex_file(input_file, output_file):
    with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
        for line in infile:
            data_part = line[6:].strip()
            
            if data_part:
                hex_values = data_part.split()
                hex_string = ' '.join(hex_values)
                outfile.write(hex_string + ' ')

input_file = 'input.txt'
output_file = 'output.txt'

process_hex_file(input_file, output_file)
