import csv

def clean_and_convert_txt_to_csv(input_txt, output_csv):
    with open(input_txt, 'r') as file:
        lines = file.readlines()
    
    # Find the start of the data (skip header lines)
    data_lines = []
    for line in lines:
        line = line.strip()
        if line and (line[0].isdigit() or line.startswith('-')):
            data_lines.append(line)
    
    # Process and clean the data
    cleaned_data = []
    for line in data_lines:
        values = line.split(',')
        if len(values) < 4:  # Ensure all four values are present
            continue
        
        try:
            angle_x = round(float(values[0].strip()), 2)
            angle_y = round(float(values[1].strip()), 2)
            angle_z = round(float(values[2].strip()), 2)
            label = int(values[3].strip())
            cleaned_data.append([angle_x, angle_y, angle_z, label])
        except ValueError:
            continue  # Skip lines with conversion errors
    
    # Write to CSV
    with open(output_csv, 'w', newline='') as csvfile:
        csv_writer = csv.writer(csvfile)
        csv_writer.writerow(["angleX", "angleY", "angleZ", "label"])
        csv_writer.writerows(cleaned_data)
    
    print(f"Successfully converted {input_txt} to {output_csv}")

# Example usage
clean_and_convert_txt_to_csv('raw data\CAPTURE 8.txt', 'Test_Data_R1.csv')
