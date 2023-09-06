#!/bin/bash

(

# Start the JSON array
echo "{\"datasets\":["

# Initialize a flag to track whether the first file is being printed
first_file=true

# Print all files in the "viewer/data" directory, with file extensions removed, enclosed in double quotes and separated by commas
for file in data/*; do
    # Check if this is the first file
    if [ "$first_file" = true ]; then
        first_file=false
    else
        echo ","
    fi
    # Use basename to remove the file extension
    filename=$(basename "$file")
    echo -n "\"${filename%.*}\""
done

# End the JSON array
echo
echo "]}"

) > datasets.json
